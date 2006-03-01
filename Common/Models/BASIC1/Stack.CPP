//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __STACK_CPP
#include "Stack.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Stack(1)
#X:#h<General Description>#nThe Stack is based on the arithmetic in Perry Pg
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Base        Input          1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Z_Reqd> : Datum.#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#n
#n#h<Other>#n
Default model prefix:STK#n
Short name:Stack#n
Model type:Unit#n
#G:Units
*/
//==========================================================================
// To Improve
double Drw_Stack[] = { DD_Poly, -3,10, -5,-10, 5,-10, 3,10, -3,10,
                          DD_End };
//long Drw_B_Stack[] = { DD_Arc, 0, 0, 4,
//                      DD_Poly, 0,-4, 5,-4, 5,-2, 0,-2,
//                      DD_End };

static IOAreaRec StackIOAreaList[] =
  {{"Base of Stack",       "Base"  , 0, LIO_In ,  nc_MLnk, 0, 1},
   {NULL}};
//
//long Drw_Stack[] = { DD_Poly, -10,3, 7,3, 10,0, 7,-3, -10,-3 , -7,0, -10,3,
//                      DD_End };
//
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNITHIDDEN(Stack, "Stack", "1", Drw_Stack, "Tank", "STK", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT,
                          "Process:Unit:Stack(1)",
                    "Simple Stack")

Stack::Stack(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_SrcSnk(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(StackIOAreaList);
  Contents.SetView(SVV_AsMassFrac);
  Flows.SetView(SVV_AsMassFlow);
  //Initialise();
  SSMode = SSM_Sink|SSM_ConstP;

  AmbientT  = C_2_K(20.0);
  MeanT     = C_2_K(20.0);
  TDecayTau = 300.0;
  StackHgt  = 30.0;
  StackDiam = 1.0;
  FricFact  = 0.02;
  fOn       = true;
  dDampFact = 0.0;
  }

//--------------------------------------------------------------------------

void Stack::PrepareDefinedData()
  {
#if WITHSPECSHEETS
  if (sSpecSheet.XStrCmp("-")!=0)
    sSpecSheet= Contents.Model()->ShortDesc();
#endif
  }

//--------------------------------------------------------------------------

void Stack::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  DDB.Text   ("");

  DDB.CheckBoxBtn("On",           "", DC_,   "",        &fOn,     this, isParm);

  DDB.Double ("Pressure",     "P",    DC_P,    "kPag",  xidPMean,        this, noFile|noSnap);
  DDB.Double ("Temperature",  "T",    DC_T,    "C",     xidTemp,         this, noFile|noSnap);
  DDB.Double ("Mass_Flow",    "Qm",   DC_Qm,   "kg/s",  xidQm,           this, noFile|noSnap);
  DDB.Double ("Vol_Flow",     "Qv",   DC_Qv,   "L/s",   xidQv,           this, noFile|noSnap);
  DDB.Double ("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,         this, noFile|noSnap);


  DDB.Double("BarometicPress", "", DC_P,   "mmH2Og", xidPRqd,       this, fOn ? isParm : 0);
  DDB.Double("AmbientT",       "", DC_T,   "C",      &AmbientT,     this, fOn ? isParm : 0);
  DDB.Double("MeanT",          "", DC_T,   "C",      &MeanT,        this, fOn ? isParmStopped : 0);
  DDB.Double("TDecayTau",      "", DC_Time,"s",      &TDecayTau,    this, fOn ? isParm : 0);
  DDB.Double("StackHgt",       "", DC_L,   "m",      &StackHgt,     this, fOn ? isParm : 0);
  DDB.Double("StackDiam",      "", DC_L,   "mm",     &StackDiam,    this, fOn ? isParm : 0);
  DDB.Text("");

  DDB.Double("StackDraught",   "", DC_DP, "mmH2O",   &StackDraught, this, 0);
  DDB.Double("FlowLoss",       "", DC_DP, "mmH2O",   &FlowLoss,     this, 0);

  BuildDataDefnElevation(DDB);
  
  DDB.Visibility();
  //DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
  DDB.Object(&Flows, this, NULL, NULL, DDB_RqdPage);

  DDB.Text("");
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag Stack::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_SrcSnk::DataXchg(DCB))
    return 1;
  return 0;
  }

//--------------------------------------------------------------------------

flag Stack::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  StackDiam=Max(0.001, StackDiam);
  FlwBlk *pFB = IOFB(IONo,0);
  pFB->SetOverideTau(5);
  if (fOn)
    {
    double DFlowLoss=0.0;
    double m3_to_ft3   = 3.281*3.281*3.281;
    double USAmbientT  = AmbientT*1.8;                        // Deg Rankine
    double USFlowT     = MeanT*1.8;                           // Deg Rankine
    double USStackHgt  = StackHgt/m_per_ft;                   // Ft
    double USStackDiam = StackDiam/m_per_ft;                  // Ft
    double USBaroP     = PFeed*7.5/25.4;                      // inHg
    double USArea      = 0.25*PI*Sqr(USStackDiam);            // ft^2
    double USVolFlow   = GEZ(pFB->GetQm())/pFB->MeanRho(pProps)*m3_to_ft3;   // ft^3/s
    double USVel       = USVolFlow/USArea;                    // ft/s
    double USDens      = pFB->MeanRho(pProps)*2.2046/m3_to_ft3;        // lb/ft^3

    double USStackDraught=0.256*USStackHgt*USBaroP*GEZ(1.0/USAmbientT-1.0/USFlowT);         // inH2O
    double USFlowLoss=Sqr(USVel)/(2*32.17)*(1.0+FricFact*USStackHgt/USStackDiam)*(USDens/5.2); // inH2O


    double DQ           = Max(1.0e-6, pFB->GetQm()+1.01);
    double USVolFlow1   = GEZ(pFB->GetQm()+DQ)/pFB->MeanRho(pProps)*m3_to_ft3;   // ft^3/s
    double USVel1       = USVolFlow1/USArea;                    // ft/s
    double USFlowLoss1=Sqr(USVel1)/(2*32.17)*(1.0+FricFact*USStackHgt/USStackDiam)*(USDens/5.2); // inH2O

    double DUSFlowLoss  = (USVolFlow1-USVolFlow)/ DQ;

    StackDraught=USStackDraught*101.978/25.4;                //kPa
    FlowLoss=USFlowLoss*101.978/25.4;                        //kPa;
    DFlowLoss=DUSFlowLoss*101.978/25.4;                        //kPa;
    StackDraught=Min(StackDraught, FlowLoss+0.15);  // 15 mm H20
    //  }

    pFB->SetQmFree();
    pFB->ClrDerivsBad();
    pFB->SetDPb(StackDraught, 0.0); // In is Positive
    pFB->SetDPq(FlowLoss, DFlowLoss);

    return true;
    }
  else
    {
    return false;
    }
  };

//--------------------------------------------------------------------------

void Stack::EvalDiscrete()
  {
  if (NoFlwIOs()>0)
    {
    TDecayTau=Max(10.0, TDecayTau);
    FlwBlk *pFB = IOFB(0,0);
    CSpPropInfo *pProps=IOFlwProps(0,0);
    double Qv=GEZ(pFB->GetQm())/pFB->MeanRho(pProps);
    double FlwVol=Qv*ICGetTimeInc();
    double StkVol=StackHgt*PI/4.0*Sqr(StackDiam);
    MeanT = (MeanT*StkVol+pFB->MeanTemp(pProps)*FlwVol)/GTZ(FlwVol+StkVol);
    MeanT -= (MeanT-AmbientT)*(1.0-exp(-ICGetTimeInc()/GTZ(TDecayTau)));

  //  double USFlowT     = pFB->MeanTemp(pProps)*1.8;                 // Deg Rankine
    }
  }

//==========================================================================
//
//
//
//==========================================================================
