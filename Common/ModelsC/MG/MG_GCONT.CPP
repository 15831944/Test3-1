//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "mg_gcont.h"
#include "net_err.h"
#include "debugnew.h" 

SPECIEBLK_L(Water,       "H2O"   , false  ); 
SPECIEBLK_V(Steam,       "H2O"   , false  ); 
SPECIEBLK_L(Teg,        "TEG"   , false  ); 

long Drw_Gcont[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5, 
                     DD_End }; 

//===========================================================================

#define dbgAbsorbFull 0x0
#define dbgAbsorb     0x0

const byte idIn       = 0;
const byte idGasOut   = 1;
const byte idLiqOut   = 2;
const byte idTegIn    = 3;
const byte idTegOut   = 4;


static IOAreaRec GcontIOAreaList[] =
  {{ "In"       , "In"           , idIn,      LIO_In ,  nc_MLnk, 1,  1, 0, 0.0f, ""},
   { "GasOut"   , "GasOut"       , idGasOut,  LIO_Out,  nc_MLnk, 1,  1, 0, 0.0f, ""},
   { "LiqOut"   , "LiqOut"       , idLiqOut,  LIO_Out,  nc_MLnk, 1,  1, 0, 0.0f, ""},
   { "GlycolIn" , "GlycolIn"     , idTegIn,   LIO_In,   nc_MLnk, 1,  1, 0, 0.0f, ""},
   { "GlycolOut", "GlycolOut"    , idTegOut,  LIO_Out,  nc_MLnk, 1,  1, 0, 0.0f, ""},
   {NULL}};


IMPLEMENT_MODELUNIT(GlycolCont, "GlycCont", "", Drw_Gcont, "GLY", TOC_ALL|TOC_GRP_ENERGY|TOC_USER, 
                    "GlycolContactor", 
                    "GlycolContactor")  
 
// -------------------------------------------------------------------------

GlycolCont::GlycolCont(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach): 
  FlashTank(pClass_, TagIn, pAttach, eAttach)//, 
  {
  AttachClassInfo(nc_Process, GcontIOAreaList),
  TEG_P       = Contents.Press();
  TEG_T       = Contents.Temp();
  TEG_Lvl     = 0.0;
  Act_Lvl     = 0.0;
  TEG_TrayVol = 2.0;

  BedEfficiency=0.25;
  X2Est = dNAN();
  FArEst = dNAN();
  RemoveFrac=dNAN();
  };

//--------------------------------------------------------------------------

void GlycolCont::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  
  DDB.Text(""); 
  DDB.Double ("",          "Datum",       DC_L,     "m",      &dDatum,					this  ,isParm); 
  DDB.Text(""); 


  DDB.Double ("",      "Datum_Rqd",       DC_L,     "m",      &dDatum_Rqd,		  this, isParm); 
  DDB.Double ("",        "Bed_Eff",				DC_Frac,  "%",			&BedEfficiency,		this, isParm); 
  DDB.Double ("",              "P",       DC_P,     "kPag",   xidPMean,					this, 0); 
  DDB.Double ("",              "T",       DC_T,     "C",      xidTemp,					this, 0); 
  DDB.Double ("",          "TEG_P",       DC_P,			"kPag",   &TEG_P,						this, 0);  
  DDB.Double ("",          "TEG_T",       DC_T,     "C",      &TEG_T,						this, 0);  
  DDB.Double ("",        "TEG_Lvl",				DC_Frac,  "%",      &TEG_Lvl,					this, 0);  
  DDB.Double ("",        "Act_Lvl",				DC_Frac,  "%",      &Act_Lvl,					this, 0);  
  DDB.Double ("",    "TEG_TrayVol",       DC_Vol,   "m^3",    &TEG_TrayVol,     this, isParm);  
  DDB.Double ("",      "RemoveEff",				DC_Frac,  "%",      &RemoveFrac,      this, 0);  
  DDB.Text(""); 
  DDB.Text(""); 
  DDB.Double ("",   "Dew_Point_In",       DC_T,     "C", 			&Dew_Point_In,    this, 0);  
  DDB.Double ("",  "Dew_Point_Out",       DC_T,     "C", 			&Dew_Point_Out,   this, 0);  
  DDB.Text("");
	
  DDB.CheckBoxBtn("Networked", "",		DC_,			"",				 xidNetPressDamp,					this, 
    isParmStopped|(!gs_Environment.OK() || !Contents.Closed() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);
  
	DDB.Text("");
  VLE.Add_OnOff(DDB);

  VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
  if (SolveMode()==DYNMODE)
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&PresetImg, this, NULL, NULL, DDB_RqdPage);
    }
  DDB.Text(""); 

  DDB.EndStruct();

  };


//--------------------------------------------------------------------------

void GlycolCont::EvalProducts()
  {
  int ioIn     = IOWithId_Self(idIn);
  int ioGasOut = IOWithId_Self(idGasOut);
  int ioTegIn  = IOWithId_Self(idTegIn);
  int ioTegOut = IOWithId_Self(idTegOut);
  int ioLiqOut = IOWithId_Self(idLiqOut);

  if( IOQmEst_Out(ioTegOut) < 0.0 && IOQmEst_Out(ioGasOut) < 0.0 )
    {
    FlashTank::EvalProducts();
    return;
    }

  SpMArray TegImg;
  SpMArray GasImg;
  SpMArray LiqAvl;
  double OpTemp = Contents.Temp();
  double OpPres = Contents.Press();


  // Connect the inlet gas to  Contents
  Contents.ZeroDeriv();
  QFeed().QZero();
  SigmaQIn(QFeed(), 0xffffffff); 
  

  RemoveFrac = Absorb(Contents, OpTemp, OpPres, BedEfficiency*100.0, 
                  IOQmEst_Out(ioGasOut), /**IOConduit(ioIn),*/ *IOConduit(ioTegIn), X2Est, FArEst, FullTag());
  if (!Valid(RemoveFrac))
    RemoveFrac=0.0;

  //LiqImg=*Contents.Model();
  //LiqImg.ZeroPhase(som_Vap);
  //LiqImg[Teg.li()] = 0.0;
  //LiqImg.Normalise();

  LiqAvl=1.0;
  LiqAvl.ZeroPhase(som_Vap);
  LiqAvl[Teg.li()] = 0.0;
  //LiqImg.Normalise();

  GasImg=*Contents.Model();
  GasImg.ZeroPhase(som_SL);
  GasImg.Normalise();
  h2oout = RemoveFrac * GasImg[Steam()] * IOQmEst_Out(ioGasOut);
  double x = (IOQmEst_Out(ioTegOut) - h2oout)/GTZ(IOQmEst_Out(ioTegOut));
  x = Range(0.0,x,1.0);

  TegImg.Zero();
  TegImg[Teg.li()] = x;
  TegImg[Water()] = 1.0 - x;

  GasImg[Steam()] *= (1.0-RemoveFrac);
  GasImg.Normalise();
  
  SetProdMakeup(PMU_IOId | PMU_Image,    idGasOut, GasImg, Contents.Temp(), Contents.Press(), Contents.Model());
  SetProdMakeup(PMU_IOId | PMU_AvlImage, idLiqOut, LiqAvl, Contents.Temp(), Contents.Press(), Contents.Model());
  SetProdMakeup(PMU_IOId | PMU_AvlImage, idTegOut, TegImg, Contents.Temp(), Contents.Press(), Contents.Model());

  // set the TEG variables
  if( Contents.ShapeVolume() < TEG_TrayVol/5.0 )
    TEG_TrayVol     = Contents.ShapeVolume()/5.0;
  TEG_P            = Contents.Press();
  TEG_T            = Contents.Temp();

  // hss Try to calculate the correct volume of TEG. 23/1/97
  double TEG_Vol = Contents.SpMass(Teg.li()) / 1129.0;  // Hardwired density of glycol = 1129kg/m^3 

  double tmp2 = TEG_TrayVol;  
  TEG_Lvl     = TEG_Vol/GTZ(TEG_TrayVol);

  double xx1 = Contents.Volume(som_Liq);
  double xx3 = Contents.ShapeVolume();
  double xx4 = Contents.pShape->VapVolScale();
  Act_Lvl          = (Contents.Volume(som_Liq)-TEG_Vol)/GTZ(Contents.ShapeVolume()/Contents.pShape->VapVolScale() - TEG_TrayVol);

  dword IOMsk=Id_2_Mask(idIn)|Id_2_Mask(idGasOut)|Id_2_Mask(idLiqOut)|Id_2_Mask(idTegOut); 
  EvalProducts_SurgeLevel(ContentHgtOrd); 

  Dew_Point_In   = DewPt(IOConduit(ioIn)->Model(),  Contents.Press());
  Dew_Point_Out  = DewPt(IOConduit(ioGasOut)->Model(), Contents.Press());
  }

//--------------------------------------------------------------------------

void GlycolCont::EvalDerivs() 
  { 
  for (int i = 0; i < NoFlwIOs(); i++)
//    if (IOId_Self(i) <ioid_HX)
      Contents.AddDeriv(*IOConduit(i), IOSign(i));
  if (Contents.OverFlowExists())
    Contents.AddDeriv(Contents.OverFlow(), -1.0);
  
  double T=Contents.Temp();
  double P=Contents.Press();
  QmAcc=0.0;
  QvAcc=0.0;
  SpecieIter SI(som_SL, SDB);
  for (i=-1; SI.Loop(i); )
  //for (i=0; i<SDB.No();i++)
  //  if (!SDB[i].IsVap())
    {
    QmAcc+=Contents.SpdMass(i);
    QvAcc+=Contents.SpdMass(i)*SDB[i].msVolume(T, P, Contents.Ovr(), Contents.MArray());
    }

  Contents.SetSpdMass(Steam(), Contents.SpdMass(Steam()) - h2oout);
  Contents.SetSpdMass(Water(), Contents.SpdMass(Water()) + h2oout);
  }; 

//--------------------------------------------------------------------------

void GlycolCont::ApplyDerivs(double dTime, flag DoDbg) 
  {
  FlashTank::ApplyDerivs(dTime, DoDbg);

//ZZZ  H2oSettled+=dH2oSettled*dT;
//ZZZ  H2oSettled=Range(0.0 , H2oSettled, Contents.SpMass(Water()));
  }

//--------------------------------------------------------------------------

void GlycolCont::SetVLEMixFrac()//double H2OSettled)  
  {
  VLE.SetMixFrac(1.0, 1.0);
  VLE.SetMixFrac(Teg.ci(), 0.0, 0.0);
//  VLE.SetMixFrac(H2O.ci(), 1.0, 1.0-H2oSettled/GTZ(Contents.SpMass(Water())));
  }

//--------------------------------------------------------------------------

flag GlycolCont::EvalPressureSens()
  {
  double ExtraH2oSettled=0.0;
  SetVLEMixFrac();//H2oSettled);

  double BigDiff=0.0;
  
  // Must be closed and have contents
  if (!Contents.Closed() || Contents.Mass()< 1.0e-6)
    {
    ZeroVLSensitivities(0);
    return True;
    }

  Contents.SetHoldVentExpand();
  ContStartInfo Start(Contents);
  
//  ContStartInfo Start(C);
  // Vapour Sensitivity
  double QMassV=1.0;
  double DeltaFrac=Range(1.0e-6, 0.1*Max(1.0, Start.MassV)/NZ(fabs(QMassV)), 1.0);
  Contents.AddM(Contents, som_Vap, DeltaFrac);
  double AvgPSensV=(Contents.Press()-Start.Pres)/DeltaFrac;
  RestoreContents(Contents, Start);

  // Liquid Sensitivity
  double QMassL=1.0;
  DeltaFrac=Range(1.0e-6, 0.1*Max(1.0, Start.MassL)/NZ(fabs(QMassL)), 1.0);
  Contents.AddM(Contents, som_SL, DeltaFrac);
  double AvgPSensL=(Contents.Press()-Start.Pres)/DeltaFrac;
  RestoreContents(Contents, Start);

  SetEquilibratingV2L(0, Contents, Start);
  //SetVLSensitivities(0, Contents, Start);
  SetStaticHeadSensitivities(0, Contents, Start);
  // Flash to Equilibrium


  ContStartInfo Equil(Contents);

  double DeltaMass, Mv0, Mv1, P0, P1;
  JoinRec & J=Joins[0];
  for (int c=0; c<J.NConns(); c++)
    {
    int i=J.IONo(c);
    SpConduit &Cd=*IOConduit(i);

    double XH2OScl=0.0;
//ZZZ    if (IOId_Self(i)==VLLIO_boot)
//ZZZ      XH2OScl=1.0;
//ZZZ    else if (IO_In(i))
//ZZZ      XH2OScl=1.0-H2ORemEff;
//ZZZ    else
//ZZZ      XH2OScl=0.0;

    // Transfer to retain equil due to movement of vapour
    RestoreContents(Contents, Equil);
    
    DeltaMass=Max(1.0e-6, 0.001*Equil.MassV);
    P0=Contents.Press();
    Contents.AddM(Cd, som_Vap, DeltaMass);
    P1=Contents.Press();
    double PSensV=(fabs(P1-P0)>1.0e-6) ? (P1-P0)/DeltaMass : AvgPSensV;

    Mv0=Contents.Mass(som_Vap);
//ZZZ    ExtraH2oSettled=XH2OScl*Cd.Qm(H2O.vi())*DeltaMass/GTZ(Cd.QMass(som_Vap));
    ConvergeStates();
    Mv1=Contents.Mass(som_Vap);
    double V2LQVFrac=(Mv0-Mv1)/DeltaMass;
  
    // Transfer to retain equil due to movement of liquor
    RestoreContents(Contents, Equil);
    
    DeltaMass=Max(1.0e-6, 0.001*Equil.MassL);
    P0=Contents.Press();
    Contents.AddM(Cd, som_SL, DeltaMass);
    P1=Contents.Press();
    //double PSensL=(P1-P0)/DeltaMass;
    double PSensL=(fabs(P1-P0)>1.0e-6) ? (P1-P0)/DeltaMass : AvgPSensL;
    
    Mv0=Contents.Mass(som_Vap);
//ZZZ    ExtraH2oSettled=XH2OScl*Cd.Qm(Water())*DeltaMass/GTZ(Cd.QMass(som_Liq));
    ConvergeStates();
    Mv1=Contents.Mass(som_Vap);
    
    double V2LQLFrac=(Mv0-Mv1)/DeltaMass;
    
    Set_IO_PSensVL(i, PSensV, PSensL);
    Set_IO_V2LQmFact(i, V2LQVFrac, V2LQLFrac);
                     
    }
  
  RestoreContents(Contents, Start);
//ZZZ  ExtraH2oSettled=0.0;

  Contents.ClrHoldVentExpand();

  return true;//False;
  };
  
//--------------------------------------------------------------------------

void GlycolCont::ConvergeStates()
  {
  SetVLEMixFrac();//H2oSettled+ExtraH2oSettled);
  FlashTank::ConvergeStates();
  }

//--------------------------------------------------------------------------

double GlycolCont::DewPt( pSpModel M, double P)
  {
  double VolFlw=M->Mass(som_ALL)*M->NRho(som_Vap);//44Qi);      // m^3/s
  double H2oMassFlw=M->M[Water()]+M->M[Steam()];  // kg/s
  double Y0=kg_2_lb(H2oMassFlw)/GTZ(Nm3_2_MMscf(VolFlw));
  if ((P > 1.0) && (Y0 > 1.0e-10))
    {
    double Den= 27.248 - 0.8446 * log(P) - log(Y0);
    if (fabs(Den) > 1.0e-10)
      return (5214.4 - 38.34 * log(P)) / Den;
    }
  return dNAN();
  }  

//--------------------------------------------------------------------------
// WaterContent calculates the amount of water present in the vapour
//              phase (in lb/MMScf) as a function of liquid phase TEG
//              concentration (in wt %)

double GlycolCont::WaterContent (double OpTempL, double OpPresL, double TEGConc, rdouble dWdTEG)
  {
  double  TD, dTDdTEG, WatCont;
  if (TEGConc > 99.9999) TEGConc = 99.9999;
  TD = 1.0889 * OpTempL - 76.7 + (18.57 - 0.0152 * OpTempL) * log(100.0 - TEGConc);
  dTDdTEG = -(18.57 - 0.0152 * OpTempL) / (100.0 - TEGConc);
  WatCont = exp(27.248 - 5214.4 / TD + (-0.8446 + 38.34 / TD) * log(OpPresL));
  dWdTEG = WatCont * (5214.4 - 38.34 * log(OpPresL)) / Sqr(TD) * dTDdTEG;
  return  WatCont;
  }

//--------------------------------------------------------------------------
// Author       : A C Ferramosca
//
// Description  : Absorb performs a simulation of a typical glycol
//                contactor.   When both input streams are specified,
//                the amount of water removed by the TEG stream is
//                calculated and output flowrates updated.   The bed
//                type is indirectly specified via a bed efficiency;
//                this assumption is only valid for a contactor with
//                about two ideal stages.
//
// Variables    : OpTemp         : Contactor operating temperature in K
//                OpPres         : Contactor operating pressure in kPa abs
//                BedEfficiency  : A pseudo-efficiency to indicate bed
//                                 deterioration in %
//                GasInletHeat   : Inlet gas heat in kJ/s
//                GasInletFlows  : Inlet gas component flowrates in kg/s
//                GasOutletFlows : Outlet gas component flowrates in kg/s
//                GasOutletHeat  : Outlet gas heat in kJ/s
//                GasOutletDP    : Outlet water dew point in K
//                GasOutletTemp  : Outlet gas temperature in K
//                TEGInletHeat   : Inlet TEG heat in kJ/s
//                TEGInletFlows  : Inlet TEG component flowrates in kg/s
//                TEGOutletFlows : Outlet TEG component flowrates in kg/s
//                TEGOutletHeat  : Outlet TEG heat in kJ/s
//                ErrFlag        : 0 - no error
//                                 1 -
//                                 2 -
//
// Limitations  : Water must be component 1 (in both streams)
//                Not to be used if no of theoretical stages far from 2
//                Isothermal and isobaric operation

double GlycolCont::Absorb(rSpContainer Contents, 
            double OpTemp, double OpPres, double BedEfficiency,
            double GasOutEst, rSpConduit TEGI,
            rdouble X2Est, rdouble FArEst, pchar Tag)
  {
  double AFrac = dNAN();
  #define MaxIters  200
  double  FX2S, FArS, NVol,
          Err, Num, Den, FX2, dFX2, Arg, dAr, FAr, dFA,
          G, L, X1, Y1, X0, Y0, X2, Y2;
  int     IterX=0;

  #if dbgAbsorbFull
  dbgpln("==================================================");
  dbgpln(">>> Absorber >>>");
  dbgpln("");
  #endif

  BedEfficiency = 1.19 * BedEfficiency; // just over 2 ideal stages

  // calculate feed gas info 
  NVol=Contents.NVolume(som_Vap, Contents.pMArray());     // Nm^3
  G=Nm3_2_MMscf(NVol);                // MMScf

  // calculate TEG inlet flowrate in kg/s
  L=TEGI.QMass(som_ALL);                         // kg/s

  if ((G > 1.0e-20) && (L > 1.0e-20))
    {
    X1 = 100.0 * (TEGI.Qm(Teg.li())) / L;

    // correct efficiency for flows - still to fix G variation - acf
    BedEfficiency = BedEfficiency * (L / 0.8052) * (0.002359 / G);
    BedEfficiency = Range(1.0, BedEfficiency, 100.0);

    // calculate inlet water composition in lb/MMScf
    Y1 = kg_2_lb(Contents.SpMass(Steam())) / G;

    // iterate for X2
    IterX = 0;
    X2 = (Valid(X2Est) ? X2Est : 99.9);
    FAr = (Valid(FArEst) ? FArEst : 0.08);
    int ErrOK=0;
    double BedEff=BedEfficiency;
    do
      {
      IterX++;

      FX2S = WaterContent (OpTemp, OpPres, X2, dFX2);
//      BedEff=0.5*(BedEff+Max(BedEff, (FAr-0.00001)/(0.01*0.952*(FAr-FX2S))));
      FX2 = FAr - 0.01 * 0.952 * BedEff * (FAr - FX2S);
      Arg = L * X1 / (L + 0.45359 * G * (Y1 - FX2));
      dAr = Arg / (L + 0.45359 * G * (Y1 - FX2)) * 0.45359 * G * dFX2;
      FArS = WaterContent (OpTemp, OpPres, Arg, dFA);
      FAr = Y1 - 0.01 * 0.952 * BedEff * (Y1 - FArS);
      Num = L * (X1 - X2) - 0.45359 * X2 * G * (FAr - FX2);
      Den = -L - 0.45359 * G * (X2 * (dFA * dAr - dFX2) + (FAr - FX2));

      Err = fabs(Num / Den / X2);
      ErrOK=(Err > 1.0E-6 ? 0 : ErrOK+1);
      if (ErrOK<=1)
        X2 = X2 - (Num / Den)*0.5;
      #if dbgAbsorbFull
      dbgpln("Iter %2i)  Arg:%14.6g  X2:%14.6g  Err:%14.6g  BedEff:%14.6g",IterX, Arg, X2, Err, BedEff);
      #endif
      }
    while ((ErrOK <= 1) && (IterX <= MaxIters));

    if (IterX <= MaxIters)
      {
      X0 = Arg;
      Y0 = FX2;
      Y2 = FAr;

      #if dbgAbsorbFull
      dbgpln("X0 :  %14.6g    Y0 :  %14.6g", X0,Y0);
      dbgpln("X1 :  %14.6g    Y1 :  %14.6g", X1,Y1);
      dbgpln("X2 :  %14.6g    Y2 :  %14.6g", X2,Y2);
      #endif

      double W_vap_out = lb_2_kg(Y0 * G);
      double W_liq_out = 0.0;

      double W_vap_teg = 0.0;
      double W_liq_teg = (Contents.SpMass(Steam())-W_vap_out);
      double w_vap_in=Contents.SpMass(Steam());
      AFrac = Range(0.0, 1.0-W_vap_out/GTZ(w_vap_in), 1.0);
      }
    else
      dbgpln("Absorber : Max Iterations %s",Tag);
    X2Est = X2;
    FArEst = FAr;
    }
  else
    IterX = MaxIters+1; // G, L > 0

  if (IterX > MaxIters)
    {
    }

  #if dbgAbsorbFull
  dbgpln("");
  dbgpln(" BedEff    %%           %10.2f", BedEfficiency);
  dbgpln(" Temp      K           %10.2f", OpTemp);
  dbgpln(" Pres      kPa abs     %10.2f", OpPres);
  dbgpln("                         Gas            TEG");
  //double WaterRem = (GasI.Qm(Steam())+GasI.Qm(Water())) - (GasO.Qm(Steam())+GasO.Qm(Water()));
//  dbgpln(" Water rem kg/h        %10.2f", 3600.0*WaterRem);
  dbgpln("");
//  dbgpln(" In  Flow  kg/s        %10.2f     %10.4f", GasI.Qm(som_ALL),TEGI.Qm(som_ALL));
  dbgpln("In   TEG   %% wt     X1                %10.6f", X1);
  dbgpln("     Water lb/MMScf Y1 %10.6f", Y1);
//  dbgpln("           kg/h        %10.2f     %10.2f", GasI.Qm(Water())*3600.0 ,TEGI.Qm(Water())*3600.0);
  dbgpln("     DewPt dC          %10.2f",DewPt(Contents.Model(),OpPres));
  dbgpln("");
  //dbgpln(" Out Flow  kg/s        %10.2f     %10.4f", GasO.Qm(som_ALL), TEGO.Qm(som_ALL));
  dbgpln("Out  TEG   %% wt     X0                %10.6f", X0);
  dbgpln("     Water lb/MMScf Y0 %10.6f", Y0);
  //dbgpln("           kg/h        %10.2f     %10.2f", GasO.Qm(Water())*3600.0 ,
  //                                           TEGO.Qm(Water())*3600.0);
//  dbgpln("     DewPt dC          %10.2f", DewPt(Contents,GasO,OpPres));
  dbgpln("");
  dbgpln(">>> Absorber End >>>");
  dbgpln("==================================================");
  #endif
  #if dbgAbsorb
  dbgp("Absorber : ");
  dbgp(" [%10.6f %10.6f]", Y1, DewPt(Contents.Model(),OpPres));
//  dbgp(" > [%10.6f %10.6f]", Y0, DewPt(Contents,GasO,OpPres));
  dbgp(" Iter %3i", IterX);
  dbgpln(" %s",Tag);
  #endif
  return AFrac;
  }

