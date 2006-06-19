//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __DUCTUNIT_CPP
#include "ductunit.h"

//#pragma message("NB NB NB optimise off -------------------------------------------------------")
//#pragma optimize("", off)

//==========================================================================
//
//
//
//==========================================================================

/*#D:#T:Tie(1)
#X:#h<General Description>#nThis enables pipes to be split eg doubles as 
a T-piece or cross piece, depending upon its connections.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
IO       Input/Output       2              20#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Z_Reqd> : Datum.#n
#i<General_Model> : This can be used to switch the #j<general split model>#d<General Model>
on. If it is on then the associated variables must be configured.#n
#n#n
#h<Associated variables>#n
#i<P> : The pressure of the material within the unit.#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#n
#n#h<Other>#n
Default model prefix:X#n
Short name:Tie#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

static IOAreaRec DuctUnitIOAreaList[] =
  {{"Inputs     ",      "In"   , 0, LIO_In0   ,    nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry},
   {"Outputs    ",      "Out"  , 1, LIO_Out0  ,    nc_MLnk, 1, 20, IOGRP(1)|IOPipeEntry},
   {NULL}};

double Drw_DuctUnit[] = { DD_Poly, -10,10, 10,10, 10,-10, -10,-10, -10,10,
                     DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNITHIDDEN(DuctUnit, "DuctUnit", "1", Drw_DuctUnit, "Piping", "U", TOC_SYSTEM,
                    "Process:Unit:DuctUnit(1)",   
                    "General Purpose Mixed DuctUnit") 

DuctUnit::DuctUnit(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(DuctUnitIOAreaList);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);
  FlashSplit=0.1;
  SS_Lvl=0.25;

  dTInLowest=C_2_K(20.0); 
  dTInHighest=C_2_K(1000.0);
  dRqdTOutMin=C_2_K(20.0);
  dRqdTOutMax=C_2_K(1000.0);
  dRqdTOut=dNAN;
  dRqddT=dNAN;
    
  dStructMass=0.0;
  dStructTemp=C_2_K(25.0);
  dStructUA=1000.0;
  dStructCp=0.12;
  dAmbientTemp=C_2_K(25.0);
  dHeatXferCoeff=0.0;
  dHeatLossRate=0.0;
  dRqdH2OVapFrac=dNAN;

  dQmSink=0.0;
  dQmSinkMeas=dNAN;
  dQmSinkTau=2.0;
  };

// -------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void DuctUnit::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility();

  BuildDataDefnElevation(DDB);
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, noFile|noSnap);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, noFile|noSnap);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, noFile|noSnap);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, noFile|noSnap);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, noFile|noSnap);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, noFile|noSnap);
  
  //DDB.Text    ("");
  //DDB.Visibility(SM_Direct|HM_All|SSMODE);
  //DDB.Double  ("FlashSplit",   "",      DC_Frac, "%",      &FlashSplit,    this, 0);
  //DDB.Double  ("SS_Lvl",       "",      DC_Frac, "%",      &SS_Lvl,        this, isParm);
  DDB.Visibility();
  DDB.Text    ("");
  DDB.CheckBoxBtn("Closed",        "", DC_     , "",      xidClosed,      this, 
    isParmStopped|(!gs_Environment.HasGas() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);
  DDB.CheckBoxBtn("NetPressDamping", "", DC_     , "",      xidNetPressDamp,       this, 
    isParmStopped|(!gs_Environment.HasGas() || !Contents.Closed() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);

  GSM.Add_OnOff(DDB);
  VLE.Add_OnOff(DDB);
  HX.Add_OnOff(DDB);
  RB.Add_OnOff(DDB);
  BuildDataDefnElevation(DDB);
  DDB.Text("");
  DDB.Visibility();
  DDB.Double  ("RqdTOut",      "", DC_T,    "C",      &dRqdTOut,     this, isParm|SetOnChange|NAN_OK);
  DDB.Double  ("RqddT",        "", DC_dT,   "C",      &dRqddT,       this, isParm|SetOnChange|NAN_OK);
  DDB.Visibility(SM_DynBoth|HM_All, Valid(dRqddT));
  DDB.Double  ("RqdTOutMin",   "", DC_T,    "C",      &dRqdTOutMin,  this, isParm);
  DDB.Double  ("RqdTOutMax",   "", DC_T,    "C",      &dRqdTOutMax,  this, isParm);
  DDB.Visibility(SM_DynBoth|HM_All, Valid(dRqddT)||Valid(dRqdTOut));
  DDB.Double  ("TInLowest",    "", DC_T,    "C",      &dTInLowest,   this, isParm);
  DDB.Double  ("TInHighest",   "", DC_T,    "C",      &dTInHighest,  this, isParm);

  DDB.Visibility(SM_DynBoth|HM_All, !Valid(dRqddT)&&!Valid(dRqdTOut));
  DDB.Double  ("AmbientTemp",   "", DC_T,    "C",       &dAmbientTemp,  this, isParm);
  DDB.Double  ("HeatLoss",      "", DC_HCapF,"kJ/dC/s", &dHeatXferCoeff,  this, isParm);
  DDB.Double  ("StructMass",    "", DC_M,    "kg",      &dStructMass,   this, isParm);
  DDB.Double  ("StructTemp",    "", DC_T,    "C",       &dStructTemp,   this, isParm);
  DDB.Double  ("StructCp",      "", DC_CpMs,   "kJ/kg.C", &dStructCp,     this, isParm);
  DDB.Visibility();
  DDB.Double  ("RqdH2OVapFrac", "", DC_Frac, "%",      &dRqdH2OVapFrac, this, isParm);
  DDB.Double  ("HeatLossRate",  "", DC_Pwr,  "kW",     &dHeatLossRate,  this, 0);

  DDB.Double  ("QmSink",     "", DC_Qm,     "kg/s",   &dQmSink,     this, 0);
  DDB.Double  ("QmSinkMeas", "", DC_Qm,     "kg/s",   &dQmSinkMeas, this, NAN_OK);
  DDB.Double  ("QmSinkTau",  "", DC_Time,   "s",      &dQmSinkTau,  this, isParm);

//  QmSink=0.0;
//  QmSinkTau=3.0;

  DDB.Text("");

  BuildDataDefnIOOptions(DDB);
  DDB.Text("");

  DDB.Visibility();
  BuildDataDefnShowIOs(DDB);

  HX.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
  RB.BuildDataDefn(DDB);
  VLE.BuildDataDefn(DDB);
  
  GSM.BuildDataDefn(DDB);

  if (SolveDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  //if (Contents.SpillExists())
  //  DDB.Object(&Contents.OverFlow(), this, NULL, NULL, DDB_RqdPage);
  //DDB.Object(&OverFlow, this, NULL, DDB_RqdPage);
  DDB.EndStruct();
 
  };

//--------------------------------------------------------------------------

flag DuctUnit::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  if (VLE.DataXchg(DCB))
    return 1;

//  switch (DCB.lHandle)
//    {
//    case xidClosed:
//      if (DCB.rB)
//        Contents.SetClosed(*DCB.rB); 
//      DCB.B=Contents.Closed(); 
//      return 1;
//    case xidNetPressDamp:
//      if (DCB.rB)
//        Contents.SetNetPressDamp(*DCB.rB, DCB.ForView()); 
//      DCB.B=Contents.NetPressDamp(); 
//      return 1;
//    }
  return 0;
  }

//---------------------------------------------------------------------------

flag DuctUnit::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
//  Contents.
//  bool TAdj=!Contents.Closed() || !VLE.Enabled();
//  Contents.GetPresetInfo().bTAdj=TAdj;
  return OK;
  }

//--------------------------------------------------------------------------

void DuctUnit::StartStep()
  {
  if (Valid(dQmSinkMeas))
    {
    double A=1.0-exp(-Range(0.0, ICGetTimeInc()/Max(1.0e-3, dQmSinkTau), 87.0));
    dQmSink=A*dQmSinkMeas + (1.0-A)*dQmSink;
    dQmSinkMeas=dNAN;
    }

  };

//--------------------------------------------------------------------------

void DuctUnit::EvalProducts(long JoinMask)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    default:
      //SetProdMakeup(PMU_IOId|PMU_Passing, ioid_Condens, Contents, som_SL);
      MN_Surge::EvalProducts(JoinMask);

    }
  }
    
//--------------------------------------------------------------------------

flag DuctUnit::EvalPressureSens()
  {
  double BigDiff=0.0;
  
  //if (Valid(FlwNode::StaticPressSens))
  //  Set_JoinP_Sens(0, FlwNode::StaticPressSens);
  //else 
//  if (Valid(JoinP_SensRqd(0)))
//    Set_JoinP_Sens(0, JoinP_SensRqd(0));
  if (!Contents.Closed())
    {
    ZeroVLSensitivities(0);
    return True;
    }

  CSysVector &ContentsMA=Contents.MArray();
  SpMArray MassStartA(ContentsMA);
  double totH0=Contents.totHf();
  //double dtotH0=Contents.dtotHf();
  double PresStart=Contents.Press();
  double MassStart=Contents.Mass();
  double MassStartV=Contents.Mass(som_Gas);

  Contents.SetHoldVentExpand();
  double TQ=0.0;
  double TQSens=0.0;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    SpConduit &Cd=*IOConduit(i);
    double QMass=Cd.QMass();
    if (TstGTZ(QMass))//>1.0e-6*MassStart)
      {
      double DeltaFrac=1.0;
      double QMassV=Cd.QMass(som_Gas);

      DeltaFrac=0.1*Max(1.0, MassStartV)/NZ(fabs(QMassV));//*dTime);
      DeltaFrac=Range(1.0e-6, DeltaFrac,1.0);

      Contents.AddM(Cd, som_ALL, DeltaFrac);

      // Pressure 
      double PNow=Contents.Press();
      double PSens=(PNow-PresStart)/(DeltaFrac);

      Contents.MArray().Set(MassStartA, som_ALL, 1.0);
      Contents.Set_totHf(totH0);

      TQ+=QMass;
      TQSens+=PSens*QMass;
// Temp
      Set_IO_PSensVL(i, PSens, PSens*0.001);

#pragma chCHECKIT(removed)
//      Set_IO_V2LQmFact(i, (IO_In(i) ? -0.5 : 0.0));
      }
    }
//  if (fabs(TQSens)>1.0e-100)
//    Set_JoinP_SensVL(0, TQSens/GTZ(TQ), 0.001*TQSens/GTZ(TQ));
//  else
//    Set_JoinP_SensVL(0, 1.0, 0.0);

  Contents.ClrHoldVentExpand();

  SetJoin_QmVSink(0, dQmSink);
//    double         GetQmLSink() { return dQmLSink; };

  return False;
  };

//    double         GetQmVSink() { return dQmVSink; };
//    double         GetQmLSink() { return dQmLSink; };
//    double         GetQmSink()  { return dQmVSink+dQmLSink; };
//    double         GetQmSink(int Lp)  { return Lp==0 ? dQmVSink : dQmLSink; };
//    void           SetQmVSink(double A) { ASSERT(Valid(A)); dQmVSink=A; };
//    void           SetQmLSink(double A) { ASSERT(Valid(A)); dQmLSink=A; };
//    void           SetQmSink(int Lp, double A) { ASSERT(Valid(A)); if (Lp==0) dQmVSink=A; else dQmLSink=A; };
//==========================================================================
