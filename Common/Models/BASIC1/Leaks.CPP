//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"

#define __LEAKS_CPP

#include "leaks.h"
#include "m_xfer.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================

XID xidHConst = MdlBsXID(17001);
XID xidKFact  = MdlBsXID(17000);
XID xidPDiam  = MdlBsXID(17002);
XID xidVDiam  = MdlBsXID(17003);


//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(LE_EquivDiam, Leak2AreaGroup.Name(), "LE_EquivDiam", "", TOC_SYSTEM,
                 "Equivalent Diameter",
                 "Orifice");

LE_EquivDiam::LE_EquivDiam(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dEquivDiam=0.001;//1mm
  m_dDP=0.0;
  m_dVelSonicMeas=0;
  // K for Pipe Entry + Exit
  double K=1.0+0.5;
  m_PhD.KFact.SetVal(K, this);
  }

//--------------------------------------------------------------------------

LE_EquivDiam::~LE_EquivDiam()
  {
  }

//--------------------------------------------------------------------------

void LE_EquivDiam::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Equivalent_Diameter","EquivDiam",  DC_L,   "mm",     &m_dEquivDiam,  this, isParm);
  m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "",   xidKFact,   NULL, "Calculated", "Required");
  DDB.Double ("Density",            "Rho",        DC_Rho, "kg/m^3", &m_dDensMeas,     NULL, isResult);
  DDB.Double ("Velocity",           "Vel",        DC_Ldt, "m/s",    &m_dVelMeas,      NULL, isResult);
  DDB.Double ("SonicVelocity",      "VelSonic",   DC_Ldt, "m/s",    &m_dVelSonicMeas, NULL, isResult);
  DDB.Double ("PressDrop",          "DP",         DC_DP,  "kPa",    &m_dDP,           NULL, isResult);
  }

// --------------------------------------------------------------------------

flag LE_EquivDiam::DataXchg(DataChangeBlk & DCB)
  {
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  return CFlwEqn::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

//void LE_EquivDiam::StartSolution(rFlwBlkBase FE)
//  {
//
//  };

//--------------------------------------------------------------------------

flag LE_EquivDiam::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double DensMeas=Min_FE_Rho(FE.MeanRho(pProps));
  double EquivArea=Diam2Area(m_dEquivDiam);

  // K for Pipe Entry + Exit
  double K=1.0+0.5;
  m_PhD.KFact.SetVal(K, this);

  FE.SetVelMeasRange(DensMeas, EquivArea, 0.001);
  double Qm0=FE.QmMeas();
  double Qm1=FE.QmMeas(1.001);
  double Vel0=FE.VelMeas();
  double Vel1=FE.VelMeas(1.001);
  FE.SetVelocity(Vel0);
  m_dVelMeas=Vel0;

  double DP0, DP1;
  if (FE.Linearised())
    {
    DP0=FE.LinearisedDP(Qm0, Regulation);
    DP1=FE.LinearisedDP(Qm1, Regulation);
    }
  else
    {
    DP0=PressDropKFact(Vel0, DensMeas, m_PhD.KFact());
    DP1=PressDropKFact(Vel1, DensMeas, m_PhD.KFact());
    }
  FE.SetDPq(DP0, (DP1-DP0)/FE.DQmMeas(1.001));
  FE.SetDPb(0.0, 0.0);
  m_dDP=DP0;
  m_dVelSonicMeas=Sqrt(1.4*FE.FlngPB().P*1000/GTZ(FE.MeanRhoV(pProps, FE.FlngPB().P)));

/*
  //double dPipeArea=Diam2Area(m_dDiamPipe);
  //if (pPhD0 && pPhD0->IsPipe())
  //  dPipeArea=pPhD0->Area();
  //else if (pPhD1 && pPhD1->IsPipe())
  //  dPipeArea=pPhD1->Area();
  //else
  //  {};
  //m_dDiamPipe=dPipeArea=Area2Diam(dPipeArea);

  FE.SetQmMeasRange(0.0001);
  double m_dDiamPipe=0.1;

  //double aQm = fabs(FE.Qm);
  //int    sQm = Sign(FE.Qm);
  double Rho = FE.MeanRho(pProps);
  //double Rho=
  //m_dEquivDiam = Range(0.2*m_dDiamPipe, m_dEquivDiam, 0.8*m_dDiamPipe);
  m_dEquivDiam = Range(0.001*m_dDiamPipe, m_dEquivDiam, 0.8*m_dDiamPipe);
  double DR = m_dEquivDiam/GTZ(m_dDiamPipe);//diameter ratio
  double DRpowFour = Pow(DR,4.0);//diameter ratio raised to power of 4
  double DRexpr = 1.0-DRpowFour;//1-(diam ratio to power of 4)
  double E = Pow(DRexpr,-0.5);//Velocity_of_Approach factor
  double DC = 0.5959+0.0312*Pow(DR,2.1)-0.184*Pow(DR,8.0)+0.039
             *DRpowFour*Pow(DRexpr,-1.0)-0.01584*Pow(DR,3.0);//Discharge coefficient
  m_dDP=Sqr((4000.0*FE.QmMeas())/(DC*E*PI*m_dEquivDiam*Rho))/(2.0*Rho);
  double DPDQ=(FE.QmMeas()/Rho)*Sqr(4000.0/(DC*E*PI*m_dEquivDiam*Rho));
  
  m_dDensMeas=Max(0.001, FE.MeanRho(pProps));
  m_dVelMeas=FE.VelMeas();

  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(-FE.QmSign()*m_dDP, -FE.QmSign()*DPDQ);
  FE.SetVelocity(m_dVelMeas);
*/

    
  return True;
  }

//==========================================================================
//
//
//
//==========================================================================

//IMPLEMENT_FLWEQN(LE_KFact, Leak2AreaGroup.Name(), "LE_KFact", "", TOC_SYSTEM,
//                 "K Factor",
//                 "K Factor - Darcy");
//
//LE_KFact::LE_KFact(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
//CFlwEqn(pClass_, pTag, pAttach, eAttach)
//  {
//  m_PhD.KFact.SetVal(1000, this);
//  //KFactOp=0.5;
//  //m_dDP=0;
//  }
//
////--------------------------------------------------------------------------
//
//LE_KFact::~LE_KFact()
//  {
//  }
//
////--------------------------------------------------------------------------
//
//void LE_KFact::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  DDB.Double ("Density",     "",        DC_Rho, "kg/m^3", &m_dDensMeas,   NULL, isResult);
//  DDB.Double ("Velocity",    "Vel",     DC_Ldt, "m/s",    &m_dVelMeas,    NULL, isResult);
//  KFact().BuildDataDefn(DDB, "K",  "",  DC_,    "",       xidFlwEqnKFact, NULL, "Calculated", "Required");
//  DDB.Double ("PressDrop",   "DP",      DC_DP,  "kPa",    &m_dDP,         NULL, isResult);
//  //DDB.Double ("MassFlow",    "Qm",      DC_Qm,  "g/s",    &m_dQm,         NULL, isResult);
//
//  BuildDataDefnOveride(DDB);
//  };
//
//// --------------------------------------------------------------------------
//
//flag LE_KFact::DataXchg(DataChangeBlk & DCB)
//  {
//  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
//    return 1;
//  return CFlwEqn::DataXchg(DCB);
//  }
//
//// --------------------------------------------------------------------------
//
//flag LE_KFact::ValidateData(ValidateDataBlk & VDB)
//  {
//  m_PhD.KFact.SetVal(ValidateRange(VDB, "K", 0.0, m_PhD.KFact.Val(), 1.0E6), this);
//  return True;
//  }
//
////--------------------------------------------------------------------------
//
//flag LE_KFact::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
//  {
//  Regulation=Range(0.0, Regulation, 1.0);
//  if (Regulation > MinValveOpening)
//    {
//    FE.SetQmFree();
//    const double DqScl=1.001;
//
//    m_dDensMeas=Max(0.001, FE.MeanRho(pProps));
//  
//    m_dVelMeas=FE.SetVelMeasRange(m_dDensMeas*FE.Area(), 0.001);
//    m_dVelMeas=Min(m_dVelMeas, 1000.0); // Abs Max Velocity - a bit ridiculous
//
//    // flow into tank K = 1
//    m_PhD.KFact.SetVal(FE.QmSign()>=0? 1.0 : 0.5, this);
//
//    double dPq2, dPq1;
//    if (FE.Linearised())
//      {
//      double Qm1  = FE.QmMeas();
//      double dQm  = FE.DQmMeas(DqScl);
//      double Qm2  = FE.QmMeas(DqScl);;
//      dPq2=FE.LinearisedDP(Qm2, Regulation);
//      dPq1=FE.LinearisedDP(Qm1, Regulation);
//      FE.SetDPq(-FE.QmSign() * dPq1, -FE.QmSign() * (dPq2 - dPq1)/dQm);
//      }
//    else
//      {
//      double Vel1 = FE.VelMeas();
//      double dQm  = FE.DQmMeas(DqScl);
//      double Vel2 = FE.VelMeas(DqScl);;
//      dPq1=PressDropKFact(Vel1, m_dDensMeas, m_PhD.KFact(), 1.0);
//      dPq2=PressDropKFact(Vel2, m_dDensMeas, m_PhD.KFact(), 1.0);
//
//      FE.SetDPq(-FE.QmSign()*dPq1, -FE.QmSign()*(dPq2-dPq1)/dQm);
//      }
//
//    double OnePhDPZ=-0.001*9.81*FE.Rise()*m_dDensMeas;
//
//    FE.SetQmFree();
//    FE.SetVelocity(FE.QmSign()*m_dVelMeas);
//    FE.SetDPa(0.0, 0.0);
//    FE.SetSlipRatio();
//    FE.SetVoidFraction();
//    FE.SetOnePhPart();
//    FE.SetTempOut(4);
//
//    FE.SetDPz(OnePhDPZ);
//
//    m_dDP=dPq1;
//
//    ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value")
//    }
//  else
//    FE.SetQmReqd(0.0);
//    return True;
//  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(LE_VolFlow, Leak2AreaGroup.Name(), "LE_VolFlw", "", TOC_SYSTEM,
                 "Simple Volumetric Flow",
                 "Simple Volumetric Flow");
//
LE_VolFlow::LE_VolFlow(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam=0.1;
  m_PwrLaw=2.0;
  m_OpDP=100.0;
  m_OpVol=dNAN;
  m_OpNVol=0.001;
  m_dDP=0;

  }

//--------------------------------------------------------------------------

LE_VolFlow::~LE_VolFlow()
  {
  }

//--------------------------------------------------------------------------

void LE_VolFlow::BuildDataDefn(DataDefnBlk & DDB)
  {
  //  DDB.Double ("Diam",     "",   DC_L,   "mm",    &Diam ,        NULL,  isParm);
  DDB.Double ("PowerLaw",   "",     DC_,      "",       &m_PwrLaw,  NULL, isParm);
  DDB.Double ("Oper_DP",    "",     DC_DP,    "kPa",    &m_OpDP ,   NULL, isParm);
  DDB.Double ("Oper_Vol",   "",     DC_Qv,    "mL/s",  &m_OpVol,   NULL, isParm|NAN_OK);
  DDB.Double ("Oper_NVol",  "",     DC_NQv,   "NmL/s", &m_OpNVol,  NULL, isParm|NAN_OK);
  DDB.Double ("PressDrop",  "DP",   DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
  //DDB.Double ("MassFlow",   "Qm",   DC_Qm,    "g/s",    &m_dQm,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag LE_VolFlow::ValidateData(ValidateDataBlk & VDB)
  {
  if (Valid(m_OpVol))
    m_OpNVol=dNAN;
  else if (!Valid(m_OpNVol))
    m_OpNVol=10.0;
  return True;
  }

//--------------------------------------------------------------------------

flag LE_VolFlow::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmFree();
    double dPq1, dPq2;
    if (Valid(m_OpVol))
      {
      double Rho=Max(0.1, FE.MeanRho(pProps));
      double K=fabs(m_OpDP)/Pow(fabs(m_OpVol*Regulation), m_PwrLaw);
      double Vol1 = FE.SetQvMeasRange(Rho, 0.01);
      double dQm  = FE.DQmMeas(1.001);
      double Vol2 = FE.QvMeas(1.001);

      dPq1 = -FE.QmSign()*K*Pow(Vol1,m_PwrLaw);
      dPq2 = -FE.QmSign()*K*Pow(Vol2,m_PwrLaw);
      FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);
      }
    else
      {
      double NRho=Max(0.1, FE.MeanRho(pProps)*Norm_P/GTZ(FE.MeanPress())*FE.MeanTemp(pProps)/Norm_T);
      double K=fabs(m_OpDP)/Pow(fabs(m_OpNVol), m_PwrLaw);
      double NVol1 = FE.SetQvMeasRange(NRho, 0.01);
      double dQm   = FE.DQmMeas(1.001);
      double NVol2 = FE.QvMeas(1.001);

      dPq1 = -FE.QmSign()*K*Pow(NVol1,m_PwrLaw);
      dPq2 = -FE.QmSign()*K*Pow(NVol2,m_PwrLaw);
      FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);
      }
    m_dDP=fabs(dPq1);
    }
  else
    FE.SetQmReqd(0.0);

  m_dDP=0;
  FE.SetFunctOfPress();

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(LE_MassFlow, Leak2AreaGroup.Name(), "LE_MassFlw", "", TOC_SYSTEM,
                 "Simple Mass Flow",
                 "Simple Mass Flow");
//
LE_MassFlow::LE_MassFlow(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam=0.1;
  m_PwrLaw=2.0;
  m_OpDP=100.0;
  m_OpQm=0.001;
  m_dDP=0;
  }

//--------------------------------------------------------------------------

LE_MassFlow::~LE_MassFlow()
  {
  }

//--------------------------------------------------------------------------

void LE_MassFlow::BuildDataDefn(DataDefnBlk & DDB)
  {
  //  DDB.Double ("Diam",     "",   DC_L,   "mm",    &Diam ,        NULL,  isParm);
  DDB.Double ("PowerLaw",   "",   DC_,    "",       &m_PwrLaw,  NULL,  isParm);
  DDB.Double ("Oper_DP",    "",   DC_DP,  "kPa",    &m_OpDP ,   NULL,  isParm);
  DDB.Double ("Oper_Qm",    "",   DC_Qm,  "g/s",    &m_OpQm ,   NULL,  isParm);
  DDB.Double ("PressDrop",  "DP", DC_DP,  "kPa",    &m_dDP,     NULL,  isResult);
  //DDB.Double ("MassFlow",   "Qm", DC_Qm,  "g/s",    &m_dQm,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag LE_MassFlow::ValidateData(ValidateDataBlk & VDB)
  {
  m_OpDP=GEZ(m_OpDP);
  m_OpQm=GEZ(m_OpQm);
  return True;
  }

//--------------------------------------------------------------------------

flag LE_MassFlow::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmFree();
    double dPq1, dPq2;
    double Rho=Max(0.1, FE.MeanRho(pProps));
    double K=fabs(m_OpDP)/Pow(fabs(m_OpQm), m_PwrLaw);
    double Qm1 = FE.SetQmMeasRange(Max(0.001, FE.MeanRho(pProps)), 0.001);
    double dQm = FE.DQmMeas(1.001);
    double Qm2 = FE.QmMeas(1.001);

    dPq1 = -FE.QmSign()*K*Pow(Qm1,m_PwrLaw);
    dPq2 = -FE.QmSign()*K*Pow(Qm2,m_PwrLaw);
    FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);

    m_dDP=fabs(FE.DPq());
    //m_dQm=FE.QmSign()*FE.QmMeas();
    }
  else
    FE.SetQmReqd(0.0);
  return True;
  };

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec CPipeLeakIOAreaList[] =
  {{"Input",   "In",   0,              LIO_In0,       nc_MLnk, 1, 1, IOOptsHide|IOPipeJoin|IOGRP(1)},
   {"Output",  "Out",  1,              LIO_Out0,      nc_MLnk, 1, 1, IOOptsHide|IOPipeJoin|IOGRP(1)},
   LEAKS2AREA("Leak", IOId_XferLeak),
   //{"Leak",    "Leak", IOId_XferLeak,  LIO_Out,       nc_MLnk, 0, 1, IOOptsHide|IOEqnsHide|IOPipeJoin|IOHidden},
   {NULL}};

double Drw_CLeak[] = {  DD_Poly, -3,0,  3,0, 
                        DD_Poly,  0,0, -1,1, 
                        DD_Poly,  0,0,  0,2, 
                        DD_Poly,  0,0,  1,1, 
                        DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CPipeLeak, "Leak", "1", Drw_CLeak, "Pipe Leak", "L", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT, 
                    "Process:Piping:Pipe Leak(1)",
                    "Unit for inserting Leak in a Pipe") 

CPipeLeak::CPipeLeak(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Xfer(pClass_, TagIn, pAttach, eAttach)//,
  {
  AttachIOAreas(CPipeLeakIOAreaList, &NullFlwGroup);
  Joins.SetSize(1); // PreInit to Allow access to Leak Information
  m_Leak.UsrEnable=true;

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(CPipeLeak), 0, mmio_CONNECT, &typeid(CFT_Mixer));
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model.*/
void CPipeLeak::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text   ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
  DDB.Visibility(SM_DynBoth|HM_All);
  DDB.Double  ("PressureEst",  "PEst",  DC_P,    "kPag",   xidPEstMean,    this, isResult|0);
  BuildDataDefnElevation(DDB);

  DDB.Visibility(SM_Buffered|HM_All);
  m_Leak.Add_OnOff(DDB,0,101);

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  if (m_Leak.Enabled)
    {
    DDB.Page("Leaks", DDB_RqdPage);
    m_Leak.BuildDataDefn(DDB, NULL, DDB_NoPage, 101);
    }

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag CPipeLeak::DataXchg(DataChangeBlk & DCB)
  {
//  for (int i=0; i<NoFlwIOs(); i++)
//    if (DCB.dwUserInfo==i+1 && IOFB(i,0)->DataXchg(DCB))
//      return 1;

  if (MN_Xfer::DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==101 && m_Leak.DataXchg(DCB))
    return 1;

  return 0;
  }

// -------------------------------------------------------------------------

void CPipeLeak::PostConnect(int IONo)
  {
  MN_Xfer::PostConnect(IONo);

  if (IOId_Self(IONo)!=IOId_XferLeak)
    return;

  IOConduit(IONo)->AttachMeToObj(this, TOA_Embedded);
  IOFB(IONo,0)->AssignFlwEqnGroup(Leak2AreaGroup, Leak2AreaGroup.Default(), this);
//  IOFB(IONo,0)->ParentFlwEqn(&m_FEP);
  //_asm int 3;
  IOFB(IONo,0)->SetIsPipe(true);
  IOFB(IONo,0)->SetFBScales(1.0,1.0);
  };

//==========================================================================
//
//
//
//==========================================================================
