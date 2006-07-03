//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __CC_EQN_CPP

#include "cc_eqn.h"
#include "m_bstres.h"

//==========================================================================
//
//
//
//==========================================================================

CC_EvalProducts::CC_EvalProducts(TaggedObject * pThis)
  {
  m_Oper          = IsoThermal;
  //m_PVConst.SetVal(1, pThis);
  m_PVConst      = 1;
  m_Gamma.SetVal(1.39, pThis);
  m_ShaftPower   = 0.0;
  m_MechEff      = 0.8;
  //m_AdiabaticEff = 0.76;
  };

CC_EvalProducts::~CC_EvalProducts()
  {
  }

void CC_EvalProducts::BuildDataDefn(TaggedObject * pThis, DataDefnBlk & DDB)
  {
  static DDBValueLst DDBOper[]={
    {IsoThermal,    "IsoThermal"},
    {Adiabatic ,    "Adiabatic"},
    {PolyTropic ,   "PolyTropic"},
    {0}};
    

  DDB.Text("Power");
  DDB.Byte   ("",           "Operation",      DC_,      "",      &m_Oper,          pThis, isParm|SetOnChange, DDBOper);
  //m_PVConst.BuildDataDefn(DDB, "", "PVConst", DC_,      "",      xidPVConst,       pThis, "Defn",  "Reqd");//, 0, m_Oper==PolyTropic ? isParm:0);
  DDB.Double("",         "PVConst",        DC_,      "",      &m_PVConst,       pThis, m_Oper==PolyTropic ? isParm:0);
  m_Gamma.BuildDataDefn(DDB,   "", "Gamma",   DC_,      "",      xidGamma,         pThis, "Meas",  "Reqd");//, 0, m_Oper==PolyTropic ? isParm:0);
  //DDB.Double ("",           "K",              DC_,      "",      &m_Gamma,            pThis, isParm);
  //DDB.Double ("",           "AdiabaticEff",   DC_Frac,  "%",     &m_dAdiabaticEff, pThis, isParm);
  DDB.Double ("Efficiency", "MechanicalEff",  DC_Frac,  "%",     &m_MechEff,       pThis, isParm);
  DDB.Double ("",           "ShaftPower",     DC_Pwr,   "kW",    &m_ShaftPower,    pThis, isResult|0);
  }

//--------------------------------------------------------------------------

flag CC_EvalProducts::DataXchg(TaggedObject * pThis, DataChangeBlk & DCB)
  {
  //if (m_PVConst.DataXchg(DCB, xidPVConst, pThis))
  //  return 1;
  if (m_Gamma.DataXchg(DCB, xidGamma, pThis))
    return 1;
  return 0;
  }

//--------------------------------------------------------------------------

flag CC_EvalProducts::ValidateData(TaggedObject * pThis, ValidateDataBlk & VDB)
  {
  //m_dAdiabaticEff=pThis->ValidateRange(VDB, "AdiabaticEff", 0.5, m_dAdiabaticEff, 1.0);
  m_MechEff=pThis->ValidateRange(VDB, "Efficiency", 1.0e-3, m_MechEff, 1.0);
  m_Gamma=pThis->ValidateRange(VDB, "Gamma", 1.3, m_Gamma, 1.5);
  switch (m_Oper)
    {
    case IsoThermal: 
      m_PVConst = 1; 
      break;
    case Adiabatic :
      /*m_PVConst       = 1.4;*/ 
      break;
    case PolyTropic: 
      m_PVConst = m_Gamma=pThis->ValidateRange(VDB, "PVConst", 1.0, m_PVConst, 1.5); 
      break;
    }
  return 1;
  }

//--------------------------------------------------------------------------

void CC_EvalProducts::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();

  if (Pi>Po)
    Exchange(Pi,Po);
  Pi=Max(1.0,Pi);
  Po=Max(1.0,Po);

  double Tin=C.Temp();
  double MoleWt=Max(1.0, C.MoleWt(som_ALL));
  double R=8314.0/MoleWt;

  if (C.QMass()>SmallPosFlow)
    m_Gamma = C.CpCv(); 
  switch (m_Oper)
    {
    case IsoThermal:  m_PVConst = 1;          /*m_PVConst.ClrOvr();*/ break;
    case Adiabatic :  m_PVConst = m_Gamma ;   /*m_PVConst.ClrOvr();*/ break;
    case PolyTropic:  /*m_PVConst = ;*/       break;
    }
  FTB.PVConst() = m_PVConst;

  double GTo=Tin *Pow(Po/Pi,(FTB.PVConst()-1.)/FTB.PVConst());
  double LTo=Tin;
  double GCp=C.totCp(som_Gas);
  double LCp=C.totCp(som_SL);
  double ToutActual=(GCp+LCp)>1.0e-10 ? (LTo*LCp+GTo*GCp)/(GCp+LCp) : Tin;
  //FTB.AddExpPower((Tout-Tin)*C.totCp(), Po);

  //// The following is from Perry (99) section 10-37
  double Qm=C.QMass();
  double Qv=C.QVolume();
  double X=Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0;
  double AdiabaticHead=m_Gamma/(m_Gamma-1)*R*Tin*(X-1.0);                       // (10-64a)
  double AdiabaticPowerM=AdiabaticHead*C.QMass()/**9.806*//1e3;           // (10-65a) this 9.806 seems to be incorrect
  double AdiabaticPowerV=2.777778e-4*m_Gamma/(m_Gamma-1)*C.QVolume()*3600*Pi*X; // (10-66a)
  double ToutAdiabatic=Tin*(X+1);                                             // (10-77)
  //double ToutActual=Tin*(X/m_dAdiabaticEff+1);
  double dHAdiabatic=C.totHf(som_ALL, ToutAdiabatic, Po)-C.totHf();
  double dHActual=C.totHf(som_ALL, ToutActual, Po)-C.totHf();
  //
  //double xK=Pi*pow(Qv, m_Gamma)*1000;
  //double xdShaftPower=m_Gamma/(m_Gamma-1)*C.QMass()*R/9804*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_MechEff;
  //double GTo=Ti*Pow(Po/Pi,(1-1.)/1.0);
  //double LTo=Ti;
  //double GCp=C.totCp(som_Gas);
  //double LCp=C.totCp(som_SL);
  //double To=(GCp+LCp)>1.0e-10 ? (LTo*LCp+GTo*GCp)/(GCp+LCp) : Ti;
  //double xIsoW=C.totHf(som_ALL, Tin, Po)-C.totHf();

  FTB.AddHeatPower(dHActual-dHAdiabatic, Po);
  FTB.AddWorkPower(dHAdiabatic, Po);

  m_ShaftPower=dHAdiabatic/m_MechEff;
  double m_dCoolPower=dHAdiabatic-dHActual;

  double xxx=0;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Mass Flow for a gas pump
#X:#n#u<#l<#b<General Description>>>#PThis model, allows the user
to set the required mass flow of the pump.
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Qm_Rqd :>#NThe required mass flow of the pump, in kilograms per second.
The default is 1 kg/s.
#n
#n#h<Other>#n
Short name:CC_FixQm#n
Model type:Gas Pump Flow Equation#n
#G:Gas Pump Flow Equations
*/

IMPLEMENT_FLWEQN(CC_FixQm, GasPumpGroup.Name(), "CC_FixQm", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Fixed Mass Flow",
                 "Fixed Mass Flow model for a Gas Pump");

CC_FixQm::CC_FixQm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  CC_EvalProducts(pAttach)
  {
  m_Reqd_Qm=1.0;
  }

//--------------------------------------------------------------------------

CC_FixQm::~CC_FixQm()
  {
  }

//--------------------------------------------------------------------------

void CC_FixQm::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("", "Qm_Rqd",      DC_Qm,    "kg/s",  &m_Reqd_Qm    , this  ,isParm);
  CC_EvalProducts::BuildDataDefn(this, DDB);
  }

//--------------------------------------------------------------------------

flag CC_FixQm::DataXchg(DataChangeBlk & DCB)
  {
  if (CC_EvalProducts::DataXchg(this, DCB))
    return 1;

  return CFlwEqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag CC_FixQm::ValidateData(ValidateDataBlk & VDB)
  {
  return CC_EvalProducts::ValidateData(this, VDB);
  }
//--------------------------------------------------------------------------

flag CC_FixQm::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);

  FE.SetQmReqd(m_Reqd_Qm*Regulation);
  FE.SetDPq(0.0, 0.0);
  return True;
  }

//--------------------------------------------------------------------------

void CC_FixQm::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  CC_EvalProducts::EvalProducts(FE, Pi, Po, FTB);
  //SpConduit &C=FE.Cd();
  //Pi=Max(1.0,Pi);
  //Po=Max(1.0,Po);

  //double Tin=C.Temp();
  //double MoleWt=Max(1.0, C.MoleWt(som_ALL));
  //double R=8314.0;

  //m_dShaftPower=m_Gamma/(m_Gamma-1)*C.QMass()*R/9804/MoleWt*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_dEfficiency;
  //double X=Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma);
  //double dH=C.totHf(som_ALL, Tin*((X-1)/m_dAdiabaticEff+1), Po)-C.totHf();
  //FTB.AddWorkPower(dH, Po);
  }

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(CC_FixQv, GasPumpGroup.Name(), "CC_FixQv", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Fixed Volume Flow",
                 "Fixed Volume Flow model for a Gas Pump");

double  CC_FixQv::sm_Damping=0.5;

CC_FixQv::CC_FixQv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  CC_EvalProducts(pAttach)
  {
  m_bAtInlet=true;
  m_dReqd_Qv=1.0;
  m_dMaxPress=dNAN;
  m_dMaxPSlew=10;
  m_dMaxNdPress=1000;
  m_Press=Std_P;
  m_Density=1.0;
  m_QmRqd=0.0;
  m_QmRqd=dNAN;
  m_WhatDamping=eGlblDamping;
  m_Damping=0.8;
  }

//--------------------------------------------------------------------------

CC_FixQv::~CC_FixQv()
  {
  }

//--------------------------------------------------------------------------

void CC_FixQv::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDBFP[]={
    {false,    "Outlet"},
    {true,     "Inlet"},
    {0}};

  DDB.Bool   ("", "QvPosn",      DC_,      "",        &m_bAtInlet,      this,  isParm, DDBFP);
  DDB.Double ("", "Qv_Rqd",      DC_Qv,    "L/s",     &m_dReqd_Qv,      this,  isParm);
  //DDB.Double ("", "Max_Press",   DC_P,     "kPa",   &dMaxPress,     this,  isParm|NAN_OK);
  //DDB.Double ("", "Max_PSlew",   DC_PpS,   "kPa/s", &dMaxPSlew,     this,  isParm);
  CC_EvalProducts::BuildDataDefn(this, DDB);

  DDB.Text("Solution");
  static DDBValueLst DDBDamping[]={
    {eNoDamping,     "NoDamping"},
    {eLclDamping,    "LclDamping"},
    {eGlblDamping,   "GlblDamping"},
    {0}};

  DDB.Byte("",    "Damping",        DC_,      "",        &m_WhatDamping,   this  ,isParm, DDBDamping);
  DDB.Visibility(SHM_All, m_WhatDamping==eGlblDamping);
  DDB.Double ("", "Glbl.Damping",   DC_Frac,  "%",       &sm_Damping,      this  ,isParm);
  DDB.Visibility(SHM_All, m_WhatDamping==eLclDamping);
  DDB.Double ("", "Lcl.Damping",    DC_Frac,  "%",       &m_Damping,       this  ,isParm);
  DDB.Visibility();
  DDB.Double ("", "Pressure",       DC_P,     "kPa",     &m_Press,         this  ,isResult);
  DDB.Double ("", "Density",        DC_Rho,   "kg/m^3",  &m_Density,       this  ,isResult);
  DDB.Double ("", "QmRqd",          DC_Qm,    "kg/s",    &m_QmRqd,         this  ,isResult);

  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

flag CC_FixQv::DataXchg(DataChangeBlk & DCB)
  {
  if (CC_EvalProducts::DataXchg(this, DCB))
    return 1;

  return CFlwEqn::DataXchg(DCB);
  };

//--------------------------------------------------------------------------

flag CC_FixQv::ValidateData(ValidateDataBlk & VDB)
  {
  return CC_EvalProducts::ValidateData(this, VDB);
  }
//--------------------------------------------------------------------------

flag CC_FixQv::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);

  m_Press=m_bAtInlet ? FE.SrcPB().PEst:FE.DstPB().PEst;
  m_Density=FE.Properties(pProps)->Rho(Range(1.0, m_Press, gs_CDB.MaxP), dNAN);
  m_Density=Max(0.001, m_Density);

  m_QmRqd=m_dReqd_Qv*Regulation*m_Density;
  if (!Valid(m_QmRqdPrv))
    m_QmRqdPrv=m_QmRqd;


  switch (m_WhatDamping)
    {
    case eNoDamping:                                                            break;
    case eLclDamping:  m_QmRqd=m_QmRqdPrv*m_Damping+(1.0-m_Damping)*m_QmRqd;    break;
    case eGlblDamping: m_QmRqd=m_QmRqdPrv*sm_Damping+(1.0-sm_Damping)*m_QmRqd;  break;
    };

  //dbgpln("CC_FixQv::EvaluateFlwEqn P:%10.3f Rho:%10.5f = QmRqdPrv:%12.5f QmRqd:%12.5f  %s", m_Press, m_Density, m_QmRqdPrv, m_QmRqd, FullObjTag());

  FE.SetQmReqd(m_QmRqd, true, (float)m_dMaxNdPress);
  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void CC_FixQv::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  CC_EvalProducts::EvalProducts(FE, Pi, Po, FTB);
  //SpConduit &C=FE.Cd();
  //Pi=Max(1.0,Pi);
  //Po=Max(1.0,Po);

  //double Tin=C.Temp();
  //double MoleWt=Max(1.0, C.MoleWt(som_ALL));
  //double R=8314.0;

  //m_dShaftPower=m_Gamma/(m_Gamma-1)*C.QMass()*R/9804/MoleWt*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_dEfficiency;
  //double X=Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma);
  //double dH=C.totHf(som_ALL, Tin*((X-1)/m_dAdiabaticEff+1), Po)-C.totHf();
  //FTB.AddWorkPower(dH, Po);
  }

//--------------------------------------------------------------------------

void CC_FixQv::StepFlowInfo(CFlwBlkBase & FE)
  {
  CFlwEqn::StepFlowInfo(FE);
  if (Valid(m_dMaxPress))
    m_dMaxNdPress=Min(m_dMaxPress, FE.NodePress()+m_dMaxPSlew*ICGetTimeInc());
  else
    m_dMaxNdPress=dNAN;

  m_QmRqdPrv=m_QmRqd;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Boost for gas pump
#X:#n#u<#l<#b<General Description>>>#PThis allows the user to set the required pressure boost
of the pump.
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Boost :>#NThe required pressure boost of the pump, in kiloPascals.
The default is 100 kPa
#n
#n#h<Other>#n
Short name:CC_FixPB#n
Model type:Gas Pump Flow Equation#n
#G:Gas Pump Flow Equations
*/

IMPLEMENT_FLWEQN(CC_FixBst, GasPumpGroup.Name(), "CC_FixPB", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Fixed Boost",
                 "Fixed Boost model for a Gas Pump");

CC_FixBst::CC_FixBst(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  CC_EvalProducts(pAttach)
  {
  m_P_Boost=100.0;
  }

//--------------------------------------------------------------------------

CC_FixBst::~CC_FixBst()
  {
  }

//--------------------------------------------------------------------------

void CC_FixBst::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("", "Boost",       DC_DP,    "kPa", &m_P_Boost,     this  ,isParm);
  CC_EvalProducts::BuildDataDefn(this, DDB);
  }

//--------------------------------------------------------------------------

flag CC_FixBst::DataXchg(DataChangeBlk & DCB)
  {
  return CFlwEqn::DataXchg(DCB) && CC_EvalProducts::DataXchg(this, DCB);
  };

//--------------------------------------------------------------------------

flag CC_FixBst::ValidateData(ValidateDataBlk & VDB)
  {
  return CC_EvalProducts::ValidateData(this, VDB);
  }

//--------------------------------------------------------------------------

flag CC_FixBst::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);

  FE.SetQmMeasRange(FE.MeanRho(pProps), 0.01);
  FE.SetQmFree();
  FE.SetDPb(m_P_Boost*Regulation, 0.0);
  FE.SetDPq(0.0, 0.0);
  return True;
  }

//--------------------------------------------------------------------------

void CC_FixBst::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  CC_EvalProducts::EvalProducts(FE, Pi, Po, FTB);
  //SpConduit &C=FE.Cd();
  //Pi=Max(1.0,Pi);
  //Po=Max(1.0,Po);

  //double Tin=C.Temp();
  //double MoleWt=Max(1.0, C.MoleWt(som_ALL));
  //double R=8314.0/MoleWt;

  //// The following is from Perry (99) section 10-37
  //double Qm=C.QMass();
  //double Qv=C.QVolume();
  //double X=Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0;
  //double AdiabaticHead=m_Gamma/(m_Gamma-1)*R*Tin*(X-1.0);                       // (10-64a)
  //double AdiabaticPowerM=AdiabaticHead*C.QMass()/**9.806*//1e3;           // (10-65a) this 9.806 seems to be incorrect
  //double AdiabaticPowerV=2.777778e-4*m_Gamma/(m_Gamma-1)*C.QVolume()*3600*Pi*X; // (10-66a)
  //double ToutIdeal=Tin*(X+1);                                             // (10-77)
  //double ToutActual=Tin*(X/m_dAdiabaticEff+1);
  //double dHIdeal=C.totHf(som_ALL, ToutIdeal, Po)-C.totHf();
  //double dHActual=C.totHf(som_ALL, ToutActual, Po)-C.totHf();
  //FTB.AddExpPower(dHIdeal, Po);
  //FTB.AddWorkPower(dHActual-dHIdeal, Po);

  //m_dShaftPower=dHActual/m_MechEff;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Head for gas pump
#X:#n#u<#l<#b<General Description>>>#PThis allows the user to set the required pressure boost
of the pump.
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Boost :>#NThe required pressure boost of the pump, in kiloPascals.
#n
#n#h<Other>#n
Short name:CC_FixHd#n
Model type:Gas Pump Flow Equation#n
#G:Gas Pump Flow Equations
*/

IMPLEMENT_FLWEQN(CC_FixHd, GasPumpGroup.Name(), "CC_FixHd", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Fixed Head",
                 "Fixed Head model for a Gas Pump");

CC_FixHd::CC_FixHd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  CC_EvalProducts(pAttach)
  {
  m_P_Head=100.0;
  }

//--------------------------------------------------------------------------

CC_FixHd::~CC_FixHd()
  {
  }

//--------------------------------------------------------------------------

void CC_FixHd::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("", "Head",        DC_L,     "m",   &m_P_Head,        this  ,isParm);
  CC_EvalProducts::BuildDataDefn(this, DDB);
  }

//--------------------------------------------------------------------------

flag CC_FixHd::DataXchg(DataChangeBlk & DCB)
  {
  return CFlwEqn::DataXchg(DCB) && CC_EvalProducts::DataXchg(this, DCB);
  };

//--------------------------------------------------------------------------

flag CC_FixHd::ValidateData(ValidateDataBlk & VDB)
  {
  return CC_EvalProducts::ValidateData(this, VDB);
  }

//--------------------------------------------------------------------------

flag CC_FixHd::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Rho=FE.MeanRho(pProps);
  double SG = (Rho > 500.0) ? Rho/1000.0 : 1.0; // hss - this is not a good test, try to change.
  double mtokPa = 9.80707 * SG;

  FE.SetQmMeasRange(Rho, 0.01);
  FE.SetQmFree();
  FE.SetDPb(m_P_Head*mtokPa*Regulation, 0.0);
  FE.SetDPq(0.0, 0.0);

  return True;
  }

//--------------------------------------------------------------------------

void CC_FixHd::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  CC_EvalProducts::EvalProducts(FE, Pi, Po, FTB);
  //SpConduit &C=FE.Cd();
  //Pi=Max(1.0,Pi);
  //Po=Max(1.0,Po);

  //double Tin=C.Temp();
  //double MoleWt=Max(1.0, C.MoleWt(som_ALL));
  //double R=8314.0;

  //m_dShaftPower=m_Gamma/(m_Gamma-1)*C.QMass()*R/9804/MoleWt*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_dEfficiency;
  //double dH=C.totHf(som_ALL, (Tin*Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)/m_dAdiabaticEff), Po)-C.totHf();
  //FTB.AddWorkPower(dH, Po);
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Comp Model using a single HQ curve for Compressor
#X:#n#u<#l<#b<General Description>>>#PThis model allows the user to input a single
Head vs Flow curve and Power vs flow curve for the Comp.
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Maximum Flow :>#NThe maximum flow which the Comp is capable of producing, in l/s.
#p#b<Curves :>#NThe Head vs flow curve for the Comp.  The head should be in m of liquid,
 while the flow is in l/s.
 The Power vs flow curve for the Comp.  The flow is in l/s and the power is in kW.
#n
#n#h<Other>#n
Short name:CC_DBase#n
Model type:Compressor Flow Equation#n
#G:Compressor Flow Equations
*/

//==========================================================================

#if WithPCEdt
IMPLEMENT_FLWEQNEDT(CC_DataBase, GasPumpGroup.Name(), "CC_DBase", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT, CC_DataBaseEdt,
                 "Compressor Database",
                 "Compressor model using a database of Equations");
#else
IMPLEMENT_FLWEQN(CC_DataBase, GasPumpGroup.Name(), "CC_DBase", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Compressor Database",
                 "Compressor model using a database of Equations");
#endif

CC_DataBase::CC_DataBase(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
    PDB("Pumps", "Compressors")//,
  {
  Head=0.0;

  MaxBst=0.0;
  MaxBstQv=0.0;
  OpReg=0.0;
  PrvOpReg=-1.0;

  PressIn=Std_P;
  RhoIn=1.0;
  TempIn=Std_T;
  GammaIn=1.35;
  MoleWtFIn=30.0;

  dShaftPower=0.0;
  dEfficiency=0.8;

  dBstDamping=0.5;
  }

//--------------------------------------------------------------------------

CC_DataBase::~CC_DataBase()
  {
  }

//--------------------------------------------------------------------------

void CC_DataBase::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Text(" ");
//  DDBValueLstMem DDB0;
  DDBPumpList.Empty();
  PDB.InitPumpNameList();
  for (pStrng p=PDB.PumpNameList.First() ; p; p=PDB.PumpNameList.Next())
    DDBPumpList.Add(p->Str());
  if (DDBPumpList.Length()<1)
    DDBPumpList.Add(" ");
  // Recover old flowsheets
  if (DDB.ForFiling() && DDB.DoingPutData())
    DDB.String("",               "Pump Spec",    DC_,     "",    xidGPmpDBSpec, this, isParm, DDBPumpList());
  DDB.String("",               "Specification",    DC_,     "",    xidGPmpDBSpec, this, isParm|SetOnChange, DDBPumpList());

  char * RegName = PDB.m_RegName() ? PDB.m_RegName() : "Reg";
  char * RegCnvTxt = PDB.m_sRegCnvTxt() ? PDB.m_sRegCnvTxt() : "";

  DDB.Text   (" ");
  DDB.Double ("",              RegName,        PDB.m_iRegCnv,     RegCnvTxt,   &OpReg,         this, 0);//isParm);
  DDB.Double ("",              "Head",         DC_L,    "m",  &Head,          this, isResult|0);//HeadFlowCurve.Length()<2 ? isParm : 0);
  //DDB.Double ("",              "Head",         DC_DP,    "kPa",   &Head,          this, 0);//HeadFlowCurve.Length()<2 ? isParm : 0);

  DDB.Text("Power");
  bool SetEff=!PDB.PwrDefined() && !PDB.EffDefined();
  DDB.Double ("", "Efficiency",                 DC_Frac,  "%",   &dEfficiency, this  ,SetEff?isParm:0);
  DDB.Double ("", "ShaftPower",                 DC_Pwr,   "kW",  &dShaftPower, this  ,isResult|0);
  DDB.Double ("", "BstDamping",                 DC_Frac,  "%",   &dBstDamping, this  ,isParm);
//  DDB.Double ("TempOveride",    "",             DC_T,     "C",   &dTempOveride,this  ,isParm|NAN_OK);
//  DDB.Double ("TempOvrTau",     "",             DC_Time,  "s",   &dTempOverTau,this  ,isParm|NAN_OK);
  BuildDataDefnOveride(DDB);
  #if WithPCEdt
  if (DDB.ForView())
    {
    // Instantiate the editor
    DDB.BeginObject(this, "PDB", "CC_DBase", NULL, DDB_OptPage);

    DDB.String("",               "ObjSpec",    DC_,   "", xidGPmpDBSpec, this, isParm, DDBPumpList());
    //static double xxx=0;
    //DDB.Double ("", "XXX",                 DC_Frac,  "%",   &xxx, this  ,isParm);
    //xxx+=1.0;
    //
    DDB.EndObject();
    }
  #endif
  }

//--------------------------------------------------------------------------

//void CC_DataBase::BuildDataDefnGraph(DataDefnBlk & DDB)
//  {
//  #if WithPCEdt
//  if (DDB.ForView())
//    {
//    // Instantiate the editor
//    DDB.BeginObject(this, "PDB", "CC_DBase", NULL, DDB_OptPage);
//
//    DDB.String("",               "ObjSpec",    DC_,   "", xidGPmpDBSpec, this, isParm, DDBPumpList());
//    //static double xxx=0;
//    //DDB.Double ("", "XXX",                 DC_Frac,  "%",   &xxx, this  ,isParm);
//    //xxx+=1.0;
//    //
//    DDB.EndObject();
//    }
//  #endif
////  #if WithPCEdt
////  if (DDB.ForView())
////    {
////    // Instantiate the editor
////    DDB.BeginObject(this, "PDB", "CC_DBase", NULL, DDB_OptPage);
////
////    DDB.String("",               "ObjSpec",    DC_,   "", xidGPmpDBSpec, this, isParm, DDBPumpList());
////    //static double xxx=0;
////    //DDB.Double ("", "XXX",                 DC_Frac,  "%",   &xxx, this  ,isParm);
////    //xxx+=1.0;
////    //
////    DDB.EndObject();
////    }
////  #endif
//  }

//--------------------------------------------------------------------------

flag CC_DataBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidGPmpDBSpec :
      if (DCB.rpC)
        {
        if (PDB.SetPumpTo(DCB.rpC))
          StructureChanged(this);
        }
      DCB.pC=PDB.PumpDBSpec() ? PDB.PumpDBSpec() : "";
      return 1;
    default:;
    }

  return CFlwEqn::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag CC_DataBase::ValidateData(ValidateDataBlk & VDB)
  {
//  if (!PhDOK())
//    return true;

  //HeadDeratingCurve.Xs().SetCnvs(DC_Qv, "m^3/h");
  //HeadDeratingCurve.Ys().SetCnvs(DC_Frac, "%");
  PrvOpReg=-1.0;
  return 1;
  }

//--------------------------------------------------------------------------

//void CC_DataBase::StartSolution(rFlwBlkBase FE)
//  {
//  };
//
//--------------------------------------------------------------------------

void CC_DataBase::StepFlowInfo(CFlwBlkBase &FE)
  {
  // Filter Reqd Density
//  if (Valid(dTempMeas) && Valid(dTempOverTau))
//    {
//    double A=1.0-exp(-Range(0.0, ICGetTimeInc()/Max(1.0e-3, dTempOverTau), 87.0));
//    dTempOveride=A*dTempMeas + (1.0-A)*dTempOveride;
////    dMeasTemp=dNAN;
//    }
  };

//--------------------------------------------------------------------------

double CC_DataBase::BstAt(double Qv, rSpConduit C)
  {
  double Bst,Hd=PDB.HeadAt(Qv, OpReg);

  PressIn=GTZ(PressIn);
  if ((Qv>1.0e-6) && (C.QMass()>1.0e-6))
    { // Only Change if Direction is Forward
    GammaIn=Range(1.2, C.CpCv(), 1.5);
    MoleWtFIn=C.MoleWt();//*Rho;
    TempIn=C.Temp();
    }

//  if (Valid(dTempOveride))
//    {
//    dTempMeas=TempIn;
//    TempIn=dTempOveride;
//    }

  double R=R_c/MoleWtFIn;
  double X=GammaIn/(GammaIn-1.0);
  switch (PDB.HeadType())
    {
    case PHT_Isentropic:
      {
      // Units of Hd Expected to be kNm/kg == kJ/kg;
      // See Perry P6-17 & WMC-KNS Acid Plant Curves.
//      double P2=PressIn*Pow(GEZ(Hd*dEfficiency*9.806/GTZ(9.806*X*R*TempIn)+1.0), X);
      double P2=PressIn*Pow(GEZ(Hd*dEfficiency/GTZ(X*R*TempIn)+1.0), X);
      Bst=GEZ(P2-PressIn);
      break;
      }
    case PHT_Adiabatic:
      {
      // Units of Hd Expected to be m;
      // See Perry P6-17 & WMC-KNS Acid Plant Curves.
      double P2=PressIn*Pow(GEZ(Hd*dEfficiency*9.806/GTZ(X*R*TempIn)+1.0), X);
      Bst=GEZ(P2-PressIn);
      break;
      }
    case PHT_Head:
      Bst=Hd*(9.806*Max(1.0e-6, PDB.SG(RhoIn)));
      break;
    case PHT_Press:
      Bst=Hd;
      break;
    }
  return Bst;
  }

//--------------------------------------------------------------------------

#define DumpCrv 0

flag CC_DataBase::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  switch (Task)
    {
    case FET_GetMode:
      FE.SetEvalFwdOnly();
      FE.SetFunctOfPress();
      break;
    case FET_SetQm:
      break;
    case FET_CalcDP:
      {
      Regulation=Range(0.0, Regulation, 2.0);
#if DumpCrv
      if (CFlwEqn::DoDumpTest)
        CFlwEqn::DumpTest("\\PMPCR.csv", 0., 1., 0.01, FE, Regulation, P, Rho, C, IC);
#endif

      double Rho=FE.MeanRho(pProps);
      FE.SetBstDamping(dBstDamping);
      FE.SetDPb(0.0, 0.0);
      FE.SetDPq(0.0, 0.0);
      FE.SetFunctOfPress();
      Head=0.0;

      if (!PDB.OK() || Regulation <SmallRegulation)
        return True;

      double RegMin = PDB.m_RegMin;
      double RegMax = PDB.m_RegMax;
      OpReg = RegMin + (RegMax-RegMin) * Regulation;

      FE.SetDerivsBad(); // Do Not Use Derivatives

      double Qm     = Max(0.0, FE.GetQm());

      PressIn=FE.MeanFlngPress();
      if (Qm>1.0e-3)
        {
        RhoIn=Rho;
        }

      double Qv = Max(0.001, Qm/RhoIn);

      double Bst, Drop;
      double QvLo = PDB.LoVolFlw(OpReg);
      double QvHi = PDB.HiVolFlw(OpReg);

      double Bst0, Bst1;
      double Qv0, Qv1;
      double DQv=0.1*(QvHi-QvLo);

      SpConduit &C=FE.Cd();
      if (Qv<=QvLo)
        {
        Bst=BstAt(QvLo, C);

        //Bst=BstAt(QvLo, C);
        DQv=0.5*QvLo;
        Qv0=QvLo-DQv;
        Qv1=QvLo;
        Bst0=BstAt(Qv0, C);
        Bst1=BstAt(Qv1, C);
        Bst=Bst0+(Bst1-Bst0)/(Qv1-Qv0)*(Qv-Qv0);
        // Bst=BstLo;
        Drop=0.0;
        }
      else if (Qv>=QvHi)
        {
        Qv0=QvHi;
        Qv1=QvHi+DQv;
        Bst=BstAt(QvLo, C);
        Bst0=BstAt(Qv0, C);
        Bst1=BstAt(Qv1, C);
        double BstHi=Bst0+(Bst1-Bst0)/(Qv1-Qv0)*(Qv-Qv0);
        Drop=Bst-BstHi;
        }
      else
        {
        Qv0=Qv;
        Qv1=Qv+DQv;
        Bst0=BstAt(Qv0, C);
        Bst1=BstAt(Qv1, C);

        Bst=BstAt(QvLo, C);
        Drop=Bst-BstAt(Qv, C);
        }

      if (0)
        {
        Bst0=BstAt(Qv0, C);
        Bst1=BstAt(Qv1, C);

        double Slp=(Bst0-Bst1)/NZ(Qv0-Qv1);
        Bst=Bst0+Slp*(0-Qv0);
        //    Drop=Bst-BstAt(Qv, C);//-Slp*Qv;
        Drop=-Slp*Qv;
        dbgpln("XX %15.8g %15.8g %15.8g %15.8g %15.8g %15.8g %15.8g %15.8g", Qv, Qv0, Qv1, Bst0, Bst1, Slp, Bst, Drop);
        }
      /*

      double Bst,Hd=PDB.HeadAt(Qv, OpReg);

      switch (PDB.HeadType())
      {
      case PHT_Isentropic:
      {
      // Units of Hd Expected to be Nm/kg;
      // See Perry P6-17 & WMC-KNS Acid Plant Curves.
      PressIn=GTZ(PressIn);
      if ((Qm>1.0e-6) && (C.QMass()>1.0e-6))
      { // Only Change if Direction is Forward
      GammaIn=Range(1.2, C.CpCv(), 1.5);
      MoleWtFIn=C.MoleWt();//*Rho;
      TempIn=C.Temp();
      }
      double K=GammaIn/(GammaIn-1.0)*R_c*MoleWtFIn;//  /9.806;
      double P2=PressIn*Pow(GEZ(Hd/GTZ(K*TempIn)+1.0), GammaIn/(GammaIn-1.0));
      Bst=GEZ(P2-PressIn);
      break;
      }
      case PHT_Adiabatic:
      {
      //NBNB NOT YET Checked ????????????????????????????????????????
      // Units of Hd Expected to be m;
      // See Perry P6-17 & WMC-KNS Acid Plant Curves.
      PressIn=GTZ(PressIn);
      if ((Qm>1.0e-6) && (C.QMass()>1.0e-6))
      { // Only Change if Direction is Forward
      GammaIn=Range(1.2, C.CpCv(), 1.5);
      MoleWtFIn=C.MoleWt()/RhoIn;
      TempIn=C.Temp();
      }
      double K=GammaIn/(GammaIn-1.0)*R_c*MoleWtFIn/9.806;
      double P2=PressIn*Pow(GEZ(Hd/GTZ(K*TempIn)+1.0), GammaIn/(GammaIn-1.0));
      Bst=GEZ(P2-PressIn);
      break;
      }
      case PHT_Head:
      Bst=Hd*(9.806*Max(1.0e-6, PDB.SG(RhoIn)));
      break;
      case PHT_Press:
      Bst=Hd;
      break;

      }
      */
      FE.SetDPb(Bst,0.0);
      //  FE.SetDPq(-Drop*fabs(Qm/Qv), 0.0);
      FE.SetDPq(-Drop, 0.0);
      Head    = (FE.DPb()+FE.DPq())/(9.806*Max(1.0e-6,PDB.SG(RhoIn)));
      PrvOpReg = OpReg;
      break;
      }
    }
  return True;
  }

//--------------------------------------------------------------------------

void CC_DataBase::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  if (!PDB.OK())
    return;


  SpConduit &C=FE.Cd();
  double Ti=C.Temp();
  Pi=Max(1.0,Pi);
  Po=Max(1.0,Po);

  dEfficiency=Max(1.0e-3,dEfficiency);
  dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/dEfficiency;

  // Adiabatic TempRise
  double To=Ti*Pow(Range(1.0, Po/GTZ(Pi), 10.0), (GammaIn-1.0)/GammaIn);
  // Temp rise due to inefficiency
  double dT=dShaftPower*(1.0-dEfficiency)/GTZ(C.totCp());

  FTB.AddWorkPower((To-Ti)*C.totCp(), Po);
  FTB.AddHeatPower((dT)*C.totCp(), Po);

  //if (Valid(m_dTRiseRqd))
  //  C.SetTemp(C.Temp()+m_dTRiseRqd);
  //else
  //  {
  //  C.SetTempPress(To, Po);
  //  C.Set_totHf(C.totHf()+dShaftPower*(1.0-dEfficiency));
  //  }
  //FE.m_dTRiseTot=C.Temp()-Ti;
  }

//==========================================================================
//
//
//
//==========================================================================

#if WithPCEdt

CC_DataBaseEdt::CC_DataBaseEdt(FxdEdtView *pView_, CC_DataBase *pPC_) :
  PC_DataBaseEdt(pView_, NULL)
  {
  pPDB=(pPC_ ? pPDB=&pPC_->PDB : NULL);

  XCnv.Set(DC_Qv, "m^3/s");
  YCnv[0].Set(DC_HMs, "kJ/kg");
  YCnv[1].Set(DC_Frac, "%");
  YCnv[2].Set(DC_Pwr, "kW");
  XFmt.Set("", 0, 2, 'f');
  YFmt[0].Set("", 0, 2, 'f');
  YFmt[1].Set("", 0, 2, 'f');
  YFmt[2].Set("", 0, 2, 'f');

  XName="Flow";
  YName[0]="Yt";
  YName[1]="Eff";
  YName[2]="Power";
  };

//--------------------------------------------------------------------------

CC_DataBaseEdt::~CC_DataBaseEdt()
  {
  };

#endif

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(CC_FixQvAGA, GasPumpGroup.Name(), "CC_FixQvAGA", "", TOC_ALL|TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Fixed Volume AGA",
                 "Fixed Volume Flow AGA model for a Gas Pump");

CC_FixQvAGA::CC_FixQvAGA(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_bAtInlet=true;
  
  m_dReqd_Qv=47500.0;
  m_dOpRatio=10.0;
  m_dSrg_Qv=45800.0;
  m_dSrgRatio=12.5;
  m_dCrvDmp=0.9;
  m_dSrgFlwF=0.5;
  m_dLclEff=0.8;
  m_dStCnt=0.0;

  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  m_dMaxPress=dNAN;
  m_dMaxPSlew=1.0;
  m_dMaxNdPress=1000;
  m_dAdiabaticEff=0.76;
  m_dK=1.39;
  }
         
//--------------------------------------------------------------------------

CC_FixQvAGA::~CC_FixQvAGA()
  {
  }

//--------------------------------------------------------------------------

void CC_FixQvAGA::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDBFP[]={
    {false,    "Outlet"},
    {true,     "Inlet"},
    {0}};

  DDB.Bool   ("", "QvPosn",       DC_,      "",      &m_bAtInlet,    this,  isParm, DDBFP);
  DDB.Double ("", "Qv_OpPnt",     DC_Qv,    "L/s",   &m_dReqd_Qv,    this,  isParm);
  DDB.Double ("", "PRatio_OpPnt", DC_,      "",      &m_dOpRatio,    this,  isParm);
  DDB.Double ("", "Qv_Surge",     DC_Qv,    "L/s",   &m_dSrg_Qv,     this,  isParm);
  DDB.Double ("", "PRatio_Surge", DC_,      "",      &m_dSrgRatio,   this,  isParm);
  DDB.Double ("", "Crv_Damping",  DC_,      "",      &m_dCrvDmp,     this,  isParm);
  DDB.Double ("", "SrgFlwFactor", DC_,      "",      &m_dSrgFlwF,    this,  isParm);
  DDB.Double ("", "LocalEff",     DC_Frac,  "%",     &m_dLclEff,     this  ,isParm);
  DDB.Double ("", "Srt_Count",    DC_,      "",      &m_dStCnt,    this,  isParm);

  

  DDB.Double ("", "Max_Press",   DC_P,     "kPa",   &m_dMaxPress,   this,  isParm|NAN_OK);
  DDB.Double ("", "Max_PSlew",   DC_P,     "kPa",   &m_dMaxPSlew,   this,  isParm);
  DDB.Text("Power");
  DDB.Double ("", "Efficiency",  DC_Frac,  "%",     &m_dEfficiency, this,  isParm);
  DDB.Double ("", "ShaftPower",  DC_Pwr,   "kW",    &m_dShaftPower, this,  isResult|0);
  DDB.Double ("", "K",           DC_,      "",      &m_dK,            this  ,isParm);
  DDB.Double ("", "AdiabaticEff",DC_Frac,  "%",     &m_dAdiabaticEff, this  ,isParm);
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

flag CC_FixQvAGA::ValidateData(ValidateDataBlk & VDB)
  {
  m_dAdiabaticEff=ValidateRange(VDB, "AdiabaticEff", 0.5, m_dAdiabaticEff, 1.0);
  m_dEfficiency=ValidateRange(VDB, "Efficiency", 1.0e-3, m_dEfficiency);
  m_dK=ValidateRange(VDB, "K", 1.3, m_dK, 1.5);
  m_dStCnt = 0.0;
  return 1;
  }
//--------------------------------------------------------------------------

flag CC_FixQvAGA::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);
  double Dens=FE.Properties(pProps)->Rho(m_bAtInlet ? FE.FlngPB().PEst:FE.NodePB().PEst, dNAN);
  double Rqd_MassFlw =m_dReqd_Qv*Dens*Regulation + m_dMaxPSlew;
  double MinQm= 0.1 * m_dReqd_Qv*Dens*Regulation;
  double DsnQm=m_dReqd_Qv*Dens*Regulation;
      if (m_dMaxPSlew < (0.01 * m_dReqd_Qv))
      {
      m_dMaxPSlew = m_dReqd_Qv * 0.01;
      }

  if (m_dReqd_Qv > 0.001)
    {

    if (m_dStCnt > 10)
      {
      FE.SetQmReqd(m_dMaxPSlew, true, (float)m_dMaxNdPress);
      }
    else
      {
      m_dMaxPSlew = m_dReqd_Qv;
      FE.SetQmReqd(m_dSrgFlwF*m_dMaxPSlew, true, (float)m_dMaxNdPress);
      }
/*
if (Rqd_MassFlw < MinQm)
      {
      FE.SetQmReqd(MinQm, true, (float)m_dMaxNdPress);
      m_dMaxPSlew = MinQm - DsnQm; 
      }
    else
      {
      FE.SetQmReqd(Rqd_MassFlw, true, (float)m_dMaxNdPress);
      }
*/
      }

  else
    {
    FE.SetQmReqd(0.0, true, (float)m_dMaxNdPress);
    }
  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void CC_FixQvAGA::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  Pi=Max(1.0,Pi);
  Po=Max(1.0,Po);
  double PRatio = Po/GTZ(Pi);
  double PSrgMax = m_dSrgRatio * Pi;

 // double SrgFlowFactor=0.5;


  // kcg Linear for testing and initial will put real curves in at some stage
  // and a smooth anti surge control have avoid the negative gradient down surge line
  double DsnPower = Pi*m_dReqd_Qv*log(m_dOpRatio)/m_dEfficiency;
//  double Rho= C.
  m_dStCnt = m_dStCnt + 1.0;
  if (m_dReqd_Qv> 0.001)

    {
    double  m_dLclEff = m_dEfficiency - 0.1 * Sqr(C.QMass()/C.Rho() - m_dReqd_Qv)/m_dReqd_Qv;
    m_dLclEff = Range(0.5, m_dLclEff , 0.9);
    m_dMaxPSlew = m_dMaxPSlew*m_dCrvDmp + (((DsnPower * 1.0)/Pi)* m_dEfficiency)/Log(PRatio)* (1.0-m_dCrvDmp);
    if (m_dMaxPSlew < (0.01 * m_dReqd_Qv))
      {
      m_dMaxPSlew = m_dReqd_Qv * 0.01;
      }
    m_dShaftPower= Pi/1.0 * C.QMass()*log(Po/GTZ(Pi))/ m_dLclEff; //  m_Gamma/(m_Gamma-1)*C.QMass()*R/9804/MoleWt*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_dEfficiency;

/*
    if (Po > PSrgMax)
    double RqdFxdPFlw = DsnPower * 
      {
//      m_dMaxPSlew = m_dMaxPSlew*m_dCrvDmp + (m_dSrgFlwF*(PSrgMax - Po)/ m_dOpRatio) * m_dReqd_Qv * (1.0-m_dCrvDmp);
//      m_dMaxPSlew = m_dMaxPSlew*m_dCrvDmp+(100.0*(m_dOpRatio-PRatio)/(m_dSrgRatio-m_dOpRatio)*(m_dReqd_Qv-m_dSrg_Qv))*(1.0-m_dCrvDmp);
      m_dMaxPSlew = m_dMaxPSlew*m_dCrvDmp-(1.0*(m_dOpRatio-PRatio)/(m_dSrgRatio-m_dOpRatio)*(m_dReqd_Qv-m_dSrg_Qv))*(1.0*(m_dOpRatio-PRatio)/(m_dSrgRatio-m_dOpRatio)*(m_dReqd_Qv-m_dSrg_Qv))*(1.0-m_dCrvDmp);
  
      }
    else
      {
      m_dMaxPSlew = m_dMaxPSlew*m_dCrvDmp+((m_dOpRatio - PRatio)/(m_dSrgRatio-m_dOpRatio)*(m_dReqd_Qv-m_dSrg_Qv))*(1.0 - m_dCrvDmp);
      }
    double MaxQvIncrease = 2*fabs(m_dReqd_Qv-m_dSrg_Qv);
    double MaxQvDecrease = -0.99*m_dReqd_Qv;
    if (m_dMaxPSlew > MaxQvIncrease)
      {
      m_dMaxPSlew = MaxQvIncrease;
      }
    if (m_dMaxPSlew < MaxQvDecrease)
      {
      m_dMaxPSlew = MaxQvDecrease;
      }

    double Tin=C.Temp();
    double MoleWt=Max(1.0, C.MoleWt(som_ALL));
    double R=8314.0;
    double Rho = C.Rho();

  //  m_dShaftPower=m_Gamma/(m_Gamma-1)*C.QMass()*R/9804/MoleWt*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_dEfficiency;

    m_dShaftPower= Pi/GTZ(Rho)*C.QMass()*log(Po/GTZ(Pi))/m_dEfficiency; //  m_Gamma/(m_Gamma-1)*C.QMass()*R/9804/MoleWt*Tin*(Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma)-1.0)/m_dEfficiency;

    double X=Pow(Po/GTZ(Pi), (m_Gamma-1)/m_Gamma);
    double dH=C.totHf(som_ALL, Tin*((X-1)/m_dAdiabaticEff+1), Po)-C.totHf();
//    FTB.AddWorkPower(dH, Po);
    C.SetTemp(303.0);
*/
    }
  else
    {
    m_dShaftPower=0.0;
    }
  }

//--------------------------------------------------------------------------

void CC_FixQvAGA::StepFlowInfo(CFlwBlkBase & FE)
  {
  CFlwEqn::StepFlowInfo(FE);
  if (Valid(m_dMaxPress))
    m_dMaxNdPress=Min(m_dMaxPress, FE.NodePress()+m_dMaxPSlew*ICGetTimeInc());
  else
    m_dMaxNdPress=dNAN;
  };

//==========================================================================
//
//
//
//==========================================================================
