//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __PUMPEQNS_CPP

#include "pumpeqns.h"
#include "m_bstres.h"
#include "tspline.h"

//#include "optoff.h"

#define DllImportExport

//=========================================================================

const byte PCLoSpeed_Free=0;
const byte PCLoSpeed_Close=1;

static DDBValueLst DDBLoSpd[]={
  {PCLoSpeed_Free,  "FreeFlow"},
  {PCLoSpeed_Close,  "Close"},
  {0}};

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Mass Flow model for a pump
#X:#h<General Description>#nThis pump model forces a fixed mass flow through
the attached piping network, regardless of pressure drop.  The method of
solution also means that the network will ignore restrictions within the
piping network, i.e. if there is a closed valve in the pipe line, it will be
ignored and the set mass flow will still be forced through the pipe.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Qm_Rqd> : The required mass flow of the pump.#n
#i<Efficiency> : The efficiency of the pump, as a percentage.  Any inefficiencies
will be translated into heating the fluid being pumped.#n
#i<ShaftPower> :The installed motor power for the pump.#n
#n#n
#n#h<Other>#n
Short name:PC_FixQm#n
Model type:Pump Flow Equation#n
#G:Pump Flow Equations
*/

IMPLEMENT_FLWEQN(PC_FixQm, PumpGroup.Name(), "PC_FixQm", "", TOC_SYSTEM,
                 "Fixed Mass Flow",
                 "Fixed Mass Flow model for a Pump");

PC_FixQm::PC_FixQm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  dReqd_Qm=1.0;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  dMaxPress=dNAN;
  dMaxPSlew=10;
  dMaxNdPress=1000;
  }

//--------------------------------------------------------------------------

PC_FixQm::~PC_FixQm()
  {
  }

//--------------------------------------------------------------------------

void PC_FixQm::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("", "Qm_Rqd",      DC_Qm,    "kg/s",  &dReqd_Qm    ,this  ,isParm);
  DDB.Double ("", "Max_Press",   DC_P,     "kPa",   &dMaxPress   ,this  ,isParm|NAN_OK);
  DDB.Double ("", "Max_PSlew",   DC_PpS,   "kPa/s", &dMaxPSlew   ,this  ,isParm);
  DDB.Text("Power");
  DDB.Double ("", "Efficiency",  DC_Frac,  "%",   &m_dEfficiency, this  ,isParm);
  DDB.Double ("", "ShaftPower",  DC_Pwr,   "kW",  &m_dShaftPower, this  ,isResult|0);
  BuildDataDefnOveride(DDB);

  }

//--------------------------------------------------------------------------

flag PC_FixQm::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);
  FE.SetQmReqd(dReqd_Qm*Regulation, true, (float)dMaxNdPress);
  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void PC_FixQm::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);
  m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/m_dEfficiency;
  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);
  }

//--------------------------------------------------------------------------

void PC_FixQm::StepFlowInfo(CFlwBlkBase & FE)
  {
  CFlwEqn::StepFlowInfo(FE);
  if (Valid(dMaxPress))
    dMaxNdPress=Min(dMaxPress, FE.NodePress()+dMaxPSlew*ICGetTimeInc());
  else
    dMaxNdPress=dNAN;
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(PC_FixQv, PumpGroup.Name(), "PC_FixQv", "", TOC_SYSTEM,
                 "Fixed Volume Flow",
                 "Fixed Volume Flow model for a Pump");

PC_FixQv::PC_FixQv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_bAtInlet=true;
  dReqd_Qv=1.0;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  dMaxPress=dNAN;
  dMaxPSlew=10;
  dMaxNdPress=1000;
  }

//--------------------------------------------------------------------------

PC_FixQv::~PC_FixQv()
  {
  }

//--------------------------------------------------------------------------

void PC_FixQv::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDBFP[]={
    {false,    "Outlet"},
    {true,     "Inlet"},
    {0}};

  DDB.Bool   ("", "QvPosn",      DC_,      "",      &m_bAtInlet,    this,  isParm, DDBFP);
  DDB.Double ("", "Qv_Rqd",      DC_Qv,    "L/s",   &dReqd_Qv,      this,  isParm);
  DDB.Double ("", "Max_Press",   DC_P,     "kPa",   &dMaxPress,     this,  isParm|NAN_OK);
  DDB.Double ("", "Max_PSlew",   DC_PpS,   "kPa/s", &dMaxPSlew,     this,  isParm);
  DDB.Text("Power");
  DDB.Double ("", "Efficiency",  DC_Frac,  "%",     &m_dEfficiency, this,  isParm);
  DDB.Double ("", "ShaftPower",  DC_Pwr,   "kW",    &m_dShaftPower, this,  isResult|0);
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

flag PC_FixQv::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);
  
  double P=m_bAtInlet ? FE.SrcPB().PEst:FE.DstPB().PEst;
  double Dens=FE.Properties(pProps)->Rho(P, dNAN);
  dbgpln("PC_FixQv::EvaluateFlwEqn P:%10.3f Rho:%10.5f = QmRqd:%12.5f  %s", P, Dens, dReqd_Qv*Dens*Regulation, FullObjTag());
  
  FE.SetQmReqd(dReqd_Qv*Dens*Regulation, true, (float)dMaxNdPress);
  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void PC_FixQv::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);
  m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/m_dEfficiency;
  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);
  }

//--------------------------------------------------------------------------

void PC_FixQv::StepFlowInfo(CFlwBlkBase & FE)
  {
  CFlwEqn::StepFlowInfo(FE);
  if (Valid(dMaxPress))
    dMaxNdPress=Min(dMaxPress, FE.NodePress()+dMaxPSlew*ICGetTimeInc());
  else
    dMaxNdPress=dNAN;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Boost model for a pump
#X:#h<General Description>#nThis model simulates a pump with a fixed pressure
boost, i.e. no change to the pump boost with changes in flow.  The pump will
add the required pressure boost to the fluid regardless of any changes in the
associated piping network.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Boost> : The required pressure boost of the pump.#n
#i<Efficiency> : The efficiency of the pump as a percentage.  If the pump is
inefficient, the fluid passing through the pump will be heated.#n
#i<ShaftPower> :The installed motor power of the pump.#n
#n#n
#n#h<Other>#n
Short name:PC_FixPB#n
Model type:Pump Flow Equation#n
#G:Pump Flow Equations
*/

IMPLEMENT_FLWEQN(PC_FixBst, PumpGroup.Name(), "PC_FixPB", "", TOC_SYSTEM,
                 "Fixed Boost",
                 "Fixed Boost model for a Pump");

PC_FixBst::PC_FixBst(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_P_Boost=100.0;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  m_bLoSpeedAction=PCLoSpeed_Free;
  m_DP=0;
  }

//--------------------------------------------------------------------------

PC_FixBst::~PC_FixBst()
  {
  }

//--------------------------------------------------------------------------

void PC_FixBst::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Boost",          "",    DC_DP,    "kPa", &m_P_Boost,        this, isParm);
  DDB.Double ("FlowDrop",       "DP",  DC_DP,    "kPa", &m_DP,             this, isResult);
  DDB.Byte   ("LoSpeedAction",  "",    DC_,      "",    &m_bLoSpeedAction, this, isParm, DDBLoSpd);
  DDB.Text("Power");
  DDB.Double ("Efficiency",     "",    DC_Frac,  "%",   &m_dEfficiency,    this, isParm);
  DDB.Double ("ShaftPower",     "",    DC_Pwr,   "kW",  &m_dShaftPower,    this, isResult);
  if (m_dShaftPower>0)
    DDB.TagComment("Pumping");
  else if (m_dShaftPower<0)
    DDB.TagComment("Turbining");
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

flag PC_FixBst::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);
  if (m_bLoSpeedAction==PCLoSpeed_Close && Regulation<1.0e-6)
    {
    FE.SetQmReqd(0.0);
    FE.SetDPb(0.0, 0.0);
    FE.SetDPq(0.0, 0.0);
    }
  else
    {
    double dDensMeas=Min_FE_Rho(FE.MeanRho(pProps));
    double dViscMeas=FE.MeanViscosity(pProps);

    double dPipeArea=0;
    if (pPhD0 && pPhD0->IsPipe())
      dPipeArea=pPhD0->Area();
    else if (pPhD1 && pPhD1->IsPipe())
      dPipeArea=pPhD1->Area();
    else
      {};
    m_PhD.SetArea(dPipeArea);

    double D=Area2Diam(dPipeArea);
    double L=10*D; // a piece of pipe 10 times as long as its diameter - fully turbulent
    double dFricFac=m_FF.Turbulent(D, 5.0e-5, dDensMeas, dViscMeas);

    FE.SetVelMeasRange(dDensMeas, dPipeArea, 0.001); 

    double Vel0=FE.VelMeas();
    double Vel1=FE.VelMeas(1.001);
    double DP0=0.0005*dFricFac*L*dDensMeas*Vel0*Vel0/D;
    double DP1=0.0005*dFricFac*L*dDensMeas*Vel1*Vel1/D;

    m_DP=DP0;
    FE.SetQmFree();
    FE.SetDPb(m_P_Boost*Regulation, 0.0);
    FE.SetDPq(DP0, (DP1-DP0)/FE.DQmMeas(1.001));
    }

  return true;
  }

//--------------------------------------------------------------------------

void PC_FixBst::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);


  if (Po>=Pi)
    {
    double Power=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho());
    m_dShaftPower=Power/m_dEfficiency;
    FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);
    }
  else
    {
    double Power1=C.totHf(som_ALL, C.Temp(), Pi);;
    double Power2=C.totHf(som_ALL, C.SaturationT(Po), Po);

    FTB.AddWorkPower(-(Power1-Power2), Po);
    m_dShaftPower=-(Power1-Power2)*m_dEfficiency;
    }  
  double QV=C.QVolume();
  SetCI(1, QV<0.0);
  }

//--------------------------------------------------------------------------

flag PC_FixBst::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tReverse Flow";       return 1;
    case 2: pS="W\t";                   return 1;
    case 3: pS="W\t";                   return 1;
    case 4: pS="W\t";                   return 1;
    default:
      return CFlwEqn::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Head model for a pump
#X:#h<General Description>#nThis model allows the user to set a fixed head to which
the pump will perform.  Changes in the piping network will have an effect on the
amount of fluid being pumped.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Head> : The required head which the pump must produce.#n
#i<Efficiency> : The efficiency of the pump as a percentage.#n
#i<ShaftPower> :The installed motor power of the pump.#n
#n#n
#n#h<Other>#n
Short name:PC_FixHd#n
Model type:Pump Flow Equation#n
#G:Pump Flow Equations
*/
IMPLEMENT_FLWEQN(PC_FixHd, PumpGroup.Name(), "PC_FixHd", "", TOC_SYSTEM,
                 "Fixed Head",
                 "Fixed Head model for a Pump");

PC_FixHd::PC_FixHd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_P_Head=1.0;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  m_dPrimeRho=800.0;
  m_bLoSpeedAction=PCLoSpeed_Free;
  m_DP=0;
  }

//--------------------------------------------------------------------------

PC_FixHd::~PC_FixHd()
  {
  }

//--------------------------------------------------------------------------

void PC_FixHd::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Head",           "",    DC_L,     "m",      &m_P_Head,          this, isParm);
  DDB.Double ("PrimeDens",      "",    DC_Rho,   "kg/m^3", &m_dPrimeRho,       this, isParm);
  DDB.Double ("FlowDrop",       "DP",  DC_DP,    "kPa",    &m_DP,              this, isResult);
  DDB.Byte   ("LoSpeedAction",  "",    DC_,      "",       &m_bLoSpeedAction,  this, isParm, DDBLoSpd);
  DDB.Text("Power");
  DDB.Double ("Efficiency",     "",    DC_Frac,  "%",      &m_dEfficiency,     this  ,isParm);
  DDB.Double ("ShaftPower",     "",    DC_Pwr,   "kW",     &m_dShaftPower,     this  ,isResult|0);
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

flag PC_FixHd::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);
  double Rho=FE.MeanRho(pProps);
  
  if (m_bLoSpeedAction==PCLoSpeed_Close && Regulation<1.0e-6)
    {
    FE.SetQmReqd(0.0);
    FE.SetDPb(0.0, 0.0);
    }
  else
    {
    double dDensMeas=Min_FE_Rho(FE.MeanRho(pProps));
    double dViscMeas=FE.MeanViscosity(pProps);

    double dPipeArea=0;
    if (pPhD0 && pPhD0->IsPipe())
      dPipeArea=pPhD0->Area();
    else if (pPhD1 && pPhD1->IsPipe())
      dPipeArea=pPhD1->Area();
    else
      {};
    m_PhD.SetArea(dPipeArea);

    double D=Area2Diam(dPipeArea);
    double L=10*D; // a piece of pipe 10 times as long as its diameter - fully turbulent
    double dFricFac=m_FF.Turbulent(D, 5.0e-5, dDensMeas, dViscMeas);

    FE.SetVelMeasRange(dDensMeas, dPipeArea, 0.001); 

    double Vel0=FE.VelMeas();
    double Vel1=FE.VelMeas(1.001);
    double DP0=0.0005*dFricFac*L*dDensMeas*Vel0*Vel0/D;
    double DP1=0.0005*dFricFac*L*dDensMeas*Vel1*Vel1/D;

    m_DP=DP0;
    FE.SetQmFree();
    FE.SetDPb(Head2Press(m_P_Head*Regulation, Max(m_dPrimeRho, Rho)), 0.0);
    FE.SetDPq(DP0, (DP1-DP0)/FE.DQmMeas(1.001));
    }
  return true;
  }

//--------------------------------------------------------------------------

void PC_FixHd::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);
  m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/m_dEfficiency;
  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);

  double QV=C.QVolume();
  SetCI(1, QV<0.0);
  }

//--------------------------------------------------------------------------

flag PC_FixHd::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tReverse Flow";       return 1;
    case 2: pS="W\t";                   return 1;
    case 3: pS="W\t";                   return 1;
    case 4: pS="W\t";                   return 1;
    default:
      return CFlwEqn::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(PC_SmplCrvBst, PumpGroup.Name(), "PC_SmplCrvBst", "", TOC_SYSTEM,
                 "Simple Bst Crv",
                 "Simple Boost Curve model for a Pump");

PC_SmplCrvBst::PC_SmplCrvBst(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dBstMx=100.0;
  m_dBstOp=100.0;
  m_dQvOp=0.01;
  m_dQvMx=0.01;
  m_dQmOp=10;
  m_dQmMx=10;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  m_dPrimeRho=800.0;
  m_bQmOp=false;
  bLoSpeedAction=PCLoSpeed_Free;
  }

//--------------------------------------------------------------------------

PC_SmplCrvBst::~PC_SmplCrvBst()
  {
  }

//--------------------------------------------------------------------------

void PC_SmplCrvBst::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.CheckBoxBtn("MassParms","",  DC_,   "",       &m_bQmOp,  this, isParm);//|SetOnChange);
  DDB.Double ("", "BstMx",      DC_DP,    "kPa",    &m_dBstMx, this,  isParm);
  DDB.Double ("", "BstOp",      DC_DP,    "kPa",    &m_dBstOp, this,  isParm);
  DDB.Visibility(SHM_All, !m_bQmOp);
  DDB.Double ("", "QvOp",       DC_Qv,    "L/s",    &m_dQvOp,  this,  isParm);
  DDB.Double ("", "QvMx",       DC_Qv,    "L/s",    &m_dQvMx,  this,  isParm);
  DDB.Visibility(SHM_All, m_bQmOp);
  DDB.Double ("", "QmOp",       DC_Qm,    "kg/s",   &m_dQmOp,  this,  isParm);
  DDB.Double ("", "QmMx",       DC_Qm,    "kg/s",   &m_dQmMx,  this,  isParm);
  DDB.Visibility();
  DDB.Double ("", "PrimeDens",  DC_Rho,   "kg/m^3", &m_dPrimeRho, this, isParm);
  DDB.Byte   ("LoSpeedAction", "", DC_,   "",       &bLoSpeedAction, this, isParm, DDBLoSpd);
  DDB.Text("Power");
  DDB.Double ("", "Efficiency",  DC_Frac, "%",      &m_dEfficiency, this,  isParm);
  DDB.Double ("", "ShaftPower",  DC_Pwr,  "kW",     &m_dShaftPower, this,  isResult|0);
  BuildDataDefnOveride(DDB);
  }

// --------------------------------------------------------------------------

flag PC_SmplCrvBst::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CFlwEqn::ValidateData(VDB);

  m_dBstOp = Max(0.0, m_dBstOp);
  m_dBstMx = Range(m_dBstOp*1.1, m_dBstMx, m_dBstOp*2.0); 
  m_dQvOp  = Max(0.00001, m_dQvOp);
  m_dQvMx  = Range(m_dQvOp*1.2, m_dQvMx, m_dQvOp*2);  
  m_dQmOp  = Max(0.00001, m_dQmOp);
  m_dQmMx  = Range(m_dQmOp*1.2, m_dQmMx, m_dQmOp*2);  
  return OK;
  }

//--------------------------------------------------------------------------

flag PC_SmplCrvBst::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  m_dBstOp = Max(0.0, m_dBstOp);
  m_dBstMx = Range(m_dBstOp*1.1, m_dBstMx, m_dBstOp*2.0); 
  m_dQvOp  = Max(0.00001, m_dQvOp);
  m_dQvMx  = Range(m_dQvOp*1.2, m_dQvMx, m_dQvOp*2);  
  m_dQmOp  = Max(0.00001, m_dQmOp);
  m_dQmMx  = Range(m_dQmOp*1.2, m_dQmMx, m_dQmOp*2);  

  if (DoDbgBrk())
    { int xxx=0; }

  Regulation=Range(0.0, Regulation, 2.0);
  if (bLoSpeedAction==PCLoSpeed_Close && Regulation<1.0e-6)
    {
    FE.SetQmReqd(0.0);
    FE.SetDPb(0.0, 0.0);
    FE.SetDPq(0.0, 0.0);
    }
  else
    {
    double N, A, Qm, Q0, Dens;
    Dens=Max(m_dPrimeRho, FE.MeanRho(pProps));
    if (m_bQmOp)
      {
      N=log(1-m_dBstOp/m_dBstMx)/log(m_dQmOp/m_dQmMx);
      A=1/Pow(m_dQmMx, N);
      Qm=GEZ(FE.GetQm());
      Q0=GEZ(Qm);
      }
    else
      {    
      N=log(1-m_dBstOp/m_dBstMx)/log(m_dQvOp/m_dQvMx);
      A=1/Pow(m_dQvMx, N);
    
      Qm=GEZ(FE.GetQm());
      Q0=GEZ(Qm/Dens);
      }
    double Q1=1.01*Q0;

    double BstMax=m_dBstMx*Regulation;
    //double BstKnee=BstMax*(1-A*Pow(QvKnee,N));

    double Scl0=GEZ(1-A*Pow(Q0,N));
    double Scl1=GEZ(1-A*Pow(Q1,N));
    double Bst0=BstMax*Scl0;
    double Bst1=BstMax*Scl1;

    // always assume derivs are bad - 
    // incase the curve is a long way from linear
    FE.SetDerivsBad(true); 
    
    if(fabs(BstMax-Bst0)>10000)
      { int x=0; }; 
   // FE.SetDerivsBad();
    
    FE.SetQmFree();
    FE.SetDPb(BstMax, 0.0);
    if (0)
      {
      dbgp("PmpSmplBst %6.2f %6.2f %s %6.2f %6.2f %10.4g %10.4g",
        m_dBstOp,m_dBstMx,m_bQmOp?"Qm":"Qv",m_bQmOp?m_dQmOp:m_dQvOp,m_bQmOp?m_dQmMx:m_dQvMx,N, A);
      dbgp(" %10.4g %10.4g %14.0g %10.4g %10.4g %10.4g %10.4g %10.4g %10.4g %10.4g",
        Qm, FE.MeanRho(pProps), Q0, Q1, Scl0, Scl1, Bst0, Bst1, BstMax, Regulation);
      dbgpln("");
      }
    FE.SetDPq(-GEZ(BstMax-Bst0), -(Bst0-Bst1)/GTZ(0.01*Qm));
    }

  //FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void PC_SmplCrvBst::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);
  m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/m_dEfficiency;
  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);

  double Q=m_bQmOp ? C.QMass() : C.QVolume();
  SetCI(1, Q<0.0);
  SetCI(2, Q>(m_bQmOp ? m_dQmMx : m_dQvMx));
  }

//--------------------------------------------------------------------------

flag PC_SmplCrvBst::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tReverse Flow";       return 1;
    case 2: pS="W\tHigh Flow";          return 1;
    case 3: pS="W\t";                   return 1;
    case 4: pS="W\t";                   return 1;
    default:
      return CFlwEqn::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(PC_SmplCrvHead, PumpGroup.Name(), "PC_SmplCrvHead", "", TOC_SYSTEM,
                 "Simple Head Crv",
                 "Simple Head Curve model for a Pump");

PC_SmplCrvHead::PC_SmplCrvHead(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dHeadMx=10.0;
  m_dHeadOp=10.0;
  m_dQvOp=0.01;
  m_dQvMx=0.01;
  m_dQmOp=0.01;
  m_dQmMx=0.01;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  m_dPrimeRho=800.0;
  m_bQmOp=false;
  bLoSpeedAction=PCLoSpeed_Free;
  }

//--------------------------------------------------------------------------

PC_SmplCrvHead::~PC_SmplCrvHead()
  {
  }

//--------------------------------------------------------------------------

void PC_SmplCrvHead::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.CheckBoxBtn("MassParms","",  DC_,   "",       &m_bQmOp,   this, isParm);//|SetOnChange);
  DDB.Double ("", "HeadMx",     DC_L,     "m",      &m_dHeadMx, this  ,isParm);
  DDB.Double ("", "HeadOp",     DC_L,     "m",      &m_dHeadOp, this  ,isParm);
  DDB.Visibility(SHM_All, !m_bQmOp);
  DDB.Double ("", "QvOp",       DC_Qv,    "L/s",    &m_dQvOp,  this  ,isParm);
  DDB.Double ("", "QvMx",       DC_Qv,    "L/s",    &m_dQvMx,  this  ,isParm);
  DDB.Visibility(SHM_All, m_bQmOp);
  DDB.Double ("", "QmOp",       DC_Qm,    "kg/s",   &m_dQmOp,  this  ,isParm);
  DDB.Double ("", "QmMx",       DC_Qm,    "kg/s",   &m_dQmMx,  this  ,isParm);
  DDB.Visibility();
  DDB.Double ("", "PrimeDens",  DC_Rho,   "kg/m^3", &m_dPrimeRho, this, isParm);
  DDB.Byte   ("LoSpeedAction", "", DC_,   "",       &bLoSpeedAction, this, isParm, DDBLoSpd);
  DDB.Text("Power");
  DDB.Double ("", "Efficiency", DC_Frac,  "%",      &m_dEfficiency, this  ,isParm);
  DDB.Double ("", "ShaftPower", DC_Pwr,   "kW",     &m_dShaftPower, this  ,isResult|0);
  BuildDataDefnOveride(DDB);
  }

// --------------------------------------------------------------------------

flag PC_SmplCrvHead::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CFlwEqn::ValidateData(VDB);

  m_dHeadOp = Max(0.0, m_dHeadOp);
  m_dHeadMx = Range(m_dHeadOp*1.1, m_dHeadMx, m_dHeadOp*2.0); 
  m_dQvOp  = Max(0.00001, m_dQvOp);
  m_dQvMx  = Range(m_dQvOp*1.2, m_dQvMx, m_dQvOp*2);  
  m_dQmOp  = Max(0.00001, m_dQmOp);
  m_dQmMx  = Range(m_dQmOp*1.2, m_dQmMx, m_dQmOp*2);  

  return OK;
  }

//--------------------------------------------------------------------------

flag PC_SmplCrvHead::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  m_dHeadOp = Max(0.0, m_dHeadOp);
  m_dHeadMx = Range(m_dHeadOp*1.1, m_dHeadMx, m_dHeadOp*2.0); 
  m_dQvOp  = Max(0.00001, m_dQvOp);
  m_dQvMx  = Range(m_dQvOp*1.2, m_dQvMx, m_dQvOp*2);  
  m_dQmOp  = Max(0.00001, m_dQmOp);
  m_dQmMx  = Range(m_dQmOp*1.2, m_dQmMx, m_dQmOp*2);  

  double Rho=FE.MeanRho(pProps);
  if (DoDbgBrk())
    { int xxx=0; }

  Rho=Max(m_dPrimeRho, Rho);

  Regulation=Range(0.0, Regulation, 2.0);
  if (bLoSpeedAction==PCLoSpeed_Close && Regulation<1.0e-6)
    {
    FE.SetQmReqd(0.0);
    FE.SetDPb(0.0, 0.0);
    FE.SetDPq(0.0, 0.0);
    }
  else
    {
    double N, A, Qm, Q0, Dens;
    Dens=Max(m_dPrimeRho, FE.MeanRho(pProps));
    if (m_bQmOp)
      {
      N=log(1-m_dHeadOp/m_dHeadMx)/log(m_dQmOp/m_dQmMx);
      A=1/Pow(m_dQmMx, N);
      Qm=GEZ(FE.GetQm());
      Q0=GEZ(Qm);
      }
    else
      {    
      N=log(1-m_dHeadOp/m_dHeadMx)/log(m_dQvOp/m_dQvMx);
      A=1/Pow(m_dQvMx, N);
    
      Qm=GEZ(FE.GetQm());
      Q0=GEZ(Qm/Dens);
      }
    double Q1=1.01*Q0;

    double BstMax=Head2Press(m_dHeadMx*Regulation, Max(m_dPrimeRho, Rho));
    //double BstMax=HeadMx*Regulation;

    double Scl0=GEZ(1-A*Pow(Q0,N));
    double Scl1=GEZ(1-A*Pow(Q1,N));
    double Bst0=BstMax*Scl0;
    double Bst1=BstMax*Scl1;
    FE.SetDerivsBad(Scl1<0.01);
    
    if(fabs(BstMax-Bst0)>10000)
      { int x=0; }; 
   // FE.SetDerivsBad();
    FE.SetQmFree();
    FE.SetDPb(BstMax, 0.0);
      FE.SetDPq(-GEZ(BstMax-Bst0), -(Bst0-Bst1)/GTZ(0.01*Qm));
    }

  //FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void PC_SmplCrvHead::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);
  m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/m_dEfficiency;
  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);

  double Q=m_bQmOp ? C.QMass() : C.QVolume();
  SetCI(1, Q<0.0);
  SetCI(2, Q>(m_bQmOp ? m_dQmMx : m_dQvMx));
  }

//--------------------------------------------------------------------------

flag PC_SmplCrvHead::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tReverse Flow";       return 1;
    case 2: pS="W\tHigh Flow";          return 1;
    case 3: pS="W\t";                   return 1;
    case 4: pS="W\t";                   return 1;
    default:
      return CFlwEqn::CIStrng(No, pS);
    }
  };

//==========================================================================
//
//
//
//==========================================================================
// NOT YET
//IMPLEMENT_FLWEQN(PC_SmplCrvHd, PumpGroup.Name(), "PC_SmplCrvHd", "", TOC_SYSTEM,
//                 "Simple Head Crv",
//                 "Simple Head Curve model for a Pump");
//
PC_SmplCrvHd::PC_SmplCrvHd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  HdMx=100.0;
  HdOp=100.0;
  QvOp=100.0;
  QvMx=100.0;
  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  dPrimeRho=800.0;
  bLoSpeedAction=PCLoSpeed_Free;
  }

//--------------------------------------------------------------------------

PC_SmplCrvHd::~PC_SmplCrvHd()
  {
  }

//--------------------------------------------------------------------------

void PC_SmplCrvHd::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("", "HdMx",      DC_DP,    "kPa",    &HdMx, this  ,isParm);
  DDB.Double ("", "HdOp",      DC_DP,    "kPa",    &HdOp, this  ,isParm);
  DDB.Double ("", "QvOp",       DC_Qv,    "L/s",    &QvOp,  this  ,isParm);
  DDB.Double ("", "QvMx",       DC_Qv,    "L/s",    &QvMx,  this  ,isParm);
  DDB.Double ("", "PrimeDens",      DC_Rho,   "kg/m^3", &dPrimeRho,       this, isParm);
  DDB.Byte   ("LoSpeedAction", "",  DC_,      "",       &bLoSpeedAction,  this, isParm, DDBLoSpd);
  DDB.Text("Power");
  DDB.Double ("", "Efficiency",  DC_Frac,  "%",   &m_dEfficiency, this  ,isParm);
  DDB.Double ("", "ShaftPower",  DC_Pwr,   "kW",  &m_dShaftPower, this  ,isResult|0);
  BuildDataDefnOveride(DDB);
  }

// --------------------------------------------------------------------------

flag PC_SmplCrvHd::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CFlwEqn::ValidateData(VDB);

  HdOp = Max(0.0, HdOp);
  QvOp  = Max(0.00001, QvOp);
  HdMx = Range(HdOp*1.1, HdMx, HdOp*2.0); 
  QvMx  = Range(QvOp*1.2, 0.0, QvMx*2);  

  return OK;
  }

//--------------------------------------------------------------------------

flag PC_SmplCrvHd::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  HdOp = Max(0.0, HdOp);
  QvOp  = Max(0.00001, QvOp);
  HdMx = Range(HdOp*1.1, HdMx, HdOp*2.0); 
  QvMx  = Range(QvOp*1.2, QvMx, QvOp*3);  

  double N=log(1-HdOp/HdMx)/log(QvOp/QvMx);
  double A=1/Pow(QvMx, N);

  Regulation=Range(0.0, Regulation, 2.0);
  double Rho=FE.MeanRho(pProps);
//#pragma chCHECKIT(HSS - This needs to be improved )
//  double SG = (Rho > 500.0) ? Rho/1000.0 : 1.0; // hss - this is not a good test, try to change.
//  double mtokPa = 9.80707 * SG;

  if (bLoSpeedAction==PCLoSpeed_Close && Regulation<1.0e-6)
    {
    FE.SetQmReqd(0.0);
    FE.SetDPb(0.0, 0.0);
    }
  else
    {
    FE.SetQmFree();
    FE.SetDPb(Head2Press(0*Regulation, Max(dPrimeRho, Rho)), 0.0);
    }
  FE.SetDPq(0.0, 0.0);
  return true;
  }

//--------------------------------------------------------------------------

void PC_SmplCrvHd::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  SpConduit &C=FE.Cd();
  m_dEfficiency=Max(1.0e-3,m_dEfficiency);
  m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/m_dEfficiency;
  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);
  }

//===========================================================================
//
//
//
//===========================================================================

static char* ColNames[] = {"Specification", "Control", "Type", "SurgePt", "Head", "Efficiency", 
#if WITHPWRCRV
  "Power",
#endif
  "Description", NULL};
enum PumpSpecCols {enSpec, enControl, enType, enSurgePt, enHead, enEff,
#if WITHPWRCRV
  enPower, 
#endif
  enDesc};

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CPC_FS1 : public CFn2_Base
  {
  protected:
    double a,b,c, mn1, mx1, mn2, mx2;
  public:
    CPC_FS1() { a=b=c=0.0; mn1=mn2=-1e99; mx1=mx2=1e99; };
    virtual flag Parse(int NParms, char ** f)
      {
      a = SafeAtoF(f[1]);
      b = SafeAtoF(f[2]);
      c = SafeAtoF(f[3]);
      if (NParms>=5)
        mn1 = SafeAtoF(f[4]);
      if (NParms>=6)
        mx1 = SafeAtoF(f[5]);
      if (NParms>=7)
        mn2 = SafeAtoF(f[6]);
      if (NParms>=8)
        mx2 = SafeAtoF(f[7]);

      return NParms>=3;
      };
    virtual double Value(double Qv, double Reg)
      {
      return a*Qv*Qv + b*Qv +c;
      };
    virtual double MinIndVar1() { return mn1; };
    virtual double MaxIndVar1() { return mx1; };
    virtual double MinIndVar2() { return mn2; };
    virtual double MaxIndVar2() { return mx2; };
  };

DEFINE_CLASSBUILDER0(CPC_FS1, CFn2_Base);
IMPLEMENT_CLASSBUILDER0(CPC_FS1, CFn2_Base, "Fxd_1");

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CPC_VS1 : public CFn2_Base
  {
  protected:
    double a,b,c,d,e, mn1, mx1, mn2, mx2;
  public:
    CPC_VS1() { a=b=c=d=e=0.0; mn1=mn2=-1e99; mx1=mx2=1e99; };
    virtual flag Parse(int NParms, char ** f)
      {
      if (NParms<5)
        return false;
      a = SafeAtoF(f[1]);
      b = SafeAtoF(f[2]);
      c = SafeAtoF(f[3]);
      d = SafeAtoF(f[4]);
      e = SafeAtoF(f[5]);
      if (NParms>=7)
        mn1 = SafeAtoF(f[6]);
      if (NParms>=8)
        mx1 = SafeAtoF(f[7]);
      if (NParms>=9)
        mn2 = SafeAtoF(f[8]);
      if (NParms>=10)
        mx2 = SafeAtoF(f[9]);
      return NParms>=5;
      };
    virtual double Value(double Qv, double Reg)
      {
      return a*Qv*Qv + b*Qv + c*Reg*Reg + d*Reg + e;
      };
    virtual double MinIndVar1() { return mn1; };
    virtual double MaxIndVar1() { return mx1; };
    virtual double MinIndVar2() { return mn2; };
    virtual double MaxIndVar2() { return mx2; };
  };

DEFINE_CLASSBUILDER0(CPC_VS1, CFn2_Base);
IMPLEMENT_CLASSBUILDER0(CPC_VS1, CFn2_Base, "Var_1");

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CPC_Spline : public CFn2_Base
  {
  protected:
    TSpline       Spln;
  public:
    CPC_Spline() { };

    int SetParm(char *Fld, flag Start);
    virtual flag Parse(int NParms, char ** f);
    virtual double Value(double Qv, double Reg);
    virtual double MinIndVar1() { return Spln.XPt(0); };
    virtual double MaxIndVar1() { return Spln.XPt(Spln.NPts()-1); };
    virtual double MinIndVar2() { return -1e99; };
    virtual double MaxIndVar2() { return 1e99; };
  };

//--------------------------------------------------------------------------

int CPC_Spline::SetParm(char *Fld, flag Start)
  {
  char *p=strchr(Fld, '=');
  if (p)
    {
    *p=0;
    p++;
    }
  XStrTrim(Fld);
  byte Type;
  double Slp;

  if ((_stricmp(Fld, "Sig")==0) && p!=NULL)
    {
    Slp=SafeAtoF(p);
    Spln.SetSigma(Slp);
    return 1;
    }

  if (_stricmp(Fld, "Free")==0)
    Type=TSpline::iSlopeFree;
  else if (_stricmp(Fld, "Seg")==0)
    Type=TSpline::iSlopeOfSegment;
  else if ((_stricmp(Fld, "Slp")==0) && p!=NULL)
    {
    Type=TSpline::iSlopeSpecd;
    Slp=SafeAtoF(p);
    }
  else
    return 0;

  if (Start)
    {
    Spln.SetSlopeLo(Type, Slp);
    Spln.SetSlope1(Type, Slp);
    }
  else
    {
    Spln.SetSlopeHi(Type, Slp);
    Spln.SetSlopeN(Type, Slp);
    }

  return 2;
  }

//--------------------------------------------------------------------------

flag CPC_Spline::Parse(int NParms, char ** f)
  {
  if (NParms<6)
    return false;

  int fi=1; // Skip "SPLINE"

//  if (SetParm(f[fi++], true)==0) // in case of Sigma being Specd.
//    return false;
//  if (SetParm(f[fi++], true)==0)
//    return false;
  while (fi<=NParms && isalpha(f[fi][0]))
    if (SetParm(f[fi++], true)==0)
      return false;

  while (fi<NParms-1)
    {
    Spln.AddPt(SafeAtoF(f[fi]), SafeAtoF(f[fi+1]));
    fi+=2;
    }

  if (fi>NParms)
    return false;
  while (fi<=NParms && isalpha(f[fi][0]))
    if (SetParm(f[fi++], true)==0)
      return false;

  //check that x increases
  double x=-1.0e20;
  double y=1.0e20;
  for (int i=0; i<Spln.NPts(); i++)
    {
    if (Spln.X[i]<=x)
      return false;
//TODO This must go back - Only for the head curve
//CNM    if (Spln.Y[i]>y)
//      return false;
    x=Spln.X[i];
    y=Spln.Y[i];
    }

  return Spln.Fit();
  };

//--------------------------------------------------------------------------

double CPC_Spline::Value(double Qv, double Reg)
  {
  return Spln.Yx(Qv);
  };

//--------------------------------------------------------------------------

DEFINE_CLASSBUILDER0(CPC_Spline, CFn2_Base);
IMPLEMENT_CLASSBUILDER0(CPC_Spline, CFn2_Base, "TSpline");

//=========================================================================
//
//
//
//=========================================================================

CPumpDataSet::CPumpDataSet()
  {
  m_pHdEqn =new CFn2_Const;
  m_pEffEqn=new CFn2_Const;
#if WITHPWRCRV
  m_pPwrEqn=new CFn2_Const;
#endif
  m_RegMin=0.0;
  m_RegMax=1.0;
  m_dSurgePt=0.0;
  iHdType=PHT_Head;
  iHdCnv=DC_L;
  m_iRegCnv=DC_;
  };

//-------------------------------------------------------------------------

CPumpDataSet::~CPumpDataSet()
  {;
  delete m_pHdEqn;
  delete m_pEffEqn;
#if WITHPWRCRV
  delete m_pPwrEqn;
#endif
  };

//-------------------------------------------------------------------------

bool CPumpDataSet::SetUp(CStringArray & Values/*char **C*/, char * PumpDBSpec)
  {
  int Quote;
  CSVColArray Prms;

  char * pN=(char*)strchr(Values[enControl], '='), *pMn, *pMx, *pCnv;
  pCnv=NULL;
  if (pN)
    {
    *pN=0;
    m_RegName=Values[enControl];
    pMn=pN+1;
    pMx=strchr(pMn, ',');
    if (pMx)
      {
      *pMx=0;
      pMx++;
      while (*pMx==' ')
        pMx++;
      if (!isdigit(*pMx) && *pMx!='+' && *pMx!='-')
        {
        pCnv=pMx;
        pMx=NULL;
        }
      }
    m_RegMin=SafeAtoF(pMn);
    m_RegMax=pMx ? SafeAtoF(pMx) : m_RegMin;
    }
  else
    LogError("PumpDB", 0, "%s : Invalid Control Name", PumpDBSpec);

  m_iRegCnv=DC_;
  m_sRegCnvTxt="";
  CSVColArray f;
  if (pCnv && strlen(pCnv)>0)
    {
    int nParms = ParseCSVFunction(pCnv, f, Quote);
    if (nParms>=1)
      {
      m_iRegCnv=gs_CnvsMngr.FindCnv(f[0]);
      m_sRegCnvTxt=f[1];
      if (m_iRegCnv>0)
        {
        m_RegMin=Cnvs[m_iRegCnv]->Normal(m_RegMin, m_sRegCnvTxt());
        m_RegMax=Cnvs[m_iRegCnv]->Normal(m_RegMax, m_sRegCnvTxt());
        }
      }

    int xxx=0;
    }

/*
  CnvIndex CnvIndex = gs_CnvsMngr.FindCnv(Tkn);
  if (CnvIndex<0)
    IB.Err(ErrBuild, 146);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  CDataCnv* pCnv1 = Cnvs[CnvIndex]->Find(Tkn);
  if (pCnv1==NULL)
    {
    char ErrBuff[256];
    sprintf(ErrBuff, "Conversion family : '%s'", Cnvs[CnvIndex]->Fam());
    IB.Err(ErrBuild, 147, ErrBuff);
    }
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  if (Tkn[0] != ',')
    IB.Err(ErrBuild, 106);
  strncpy(Tkn, IB.m_pH->m_pTknParser->NextToken(), MaxTknSize);
  CDataCnv* pCnv2 = Cnvs[CnvIndex]->Find(Tkn);
*/


  iHdType=PHT_Head;
  iHdCnv=DC_L;

  if (strchr(Values[enType], '(')!=NULL)
    {
    int nParms = ParseCSVFunction(Values[enType].GetBuffer(256), Prms, Quote)-1;
    if (_stricmp(Prms[0], "Head")==0)
      {
      iHdType=PHT_Head;
      iHdCnv=DC_L;
      }
    else if (_stricmp(Prms[0], "Press")==0 || _stricmp(Prms[0], "Boost")==0)
      {
      iHdType=PHT_Press;
      iHdCnv=DC_DP;
      }
    else if (_stricmp(Prms[0], "Isentropic")==0)
      {
      iHdType=PHT_Isentropic;
      iHdCnv=DC_HMs;
      }
    else if (_stricmp(Prms[0], "Adiabatic")==0)
      {
      iHdType=PHT_Adiabatic;
      iHdCnv=DC_L;
      }
    if (nParms>=1)
      sHdCnvTxt=Prms[1];
    }
  else
    {
    LogError("PumpDB", 0, "%s : Invalid Type Function", PumpDBSpec);
    }

  if (Values[enSurgePt].GetLength())
    m_dSurgePt=SafeAtoF(Values[enSurgePt]);
  else
    m_dSurgePt=0.0;


  if (strchr(Values[enHead], '(')!=NULL)
    {
    int nParms = ParseCSVFunction(Values[enHead].GetBuffer(256), Prms, Quote)-1;

    m_pHdEqn=CFn2_BaseFactory::Construct(Prms[0]);
    if (m_pHdEqn)
      {
      if (!m_pHdEqn->Parse(nParms, Prms))
        LogError("PumpDB", 0, "%s : Invalid Head Function", PumpDBSpec);
      }
    else
      m_pHdEqn = new CFn2_Const;
    }
  else
    {
    char *Prms[2];
    Prms[0]="Const";
    Prms[1]=Values[enHead].GetBuffer(256);
    m_pHdEqn = new CFn2_Const;
    m_pHdEqn->Parse(1, Prms);
    }

  if (strchr(Values[enEff], '(')!=NULL)
    {
    int nParms = ParseCSVFunction(Values[enEff].GetBuffer(256), Prms, Quote)-1;

    m_pEffEqn=CFn2_BaseFactory::Construct(Prms[0]);
    if (m_pEffEqn)
      {
      if (!m_pEffEqn->Parse(nParms, Prms))
        LogError("PumpDB", 0, "%s : Invalid Efficiency Function", PumpDBSpec);
      }
    else
      m_pEffEqn = new CFn2_Const;
    }
  else if (Values[enEff].GetLength())
    {
    char *Prms[2];
    Prms[0]="Const";
    Prms[1]=Values[enEff].GetBuffer(256);
    m_pEffEqn = new CFn2_Const;
    m_pEffEqn->Parse(1, Prms);
    }
  else
    m_pEffEqn = NULL;

#if WITHPWRCRV
  if (strchr(Values[enPower], '(')!=NULL)
    {
    int nParms = ParseCSVFunction(Values[enPower].GetBuffer(256), Prms, Quote)-1;

    m_pPwrEqn=CFn2_BaseFactory::Construct(Prms[0]);
    if (m_pPwrEqn)
      {
      if (!m_pPwrEqn->Parse(nParms, Prms))
        LogError("PumpDB", 0, "%s : Invalid Power Function", PumpDBSpec);
      }
    else
      m_pPwrEqn = new CFn2_Const;
    }
  else if (Values[enPower].GetLength())
    {
    char *Prms[2];
    Prms[0]="Const";
    Prms[1]=Values[enPower].GetBuffer(256);
    m_pPwrEqn = new CFn2_Const;
    m_pPwrEqn->Parse(1, Prms);
    }
  else
    m_pPwrEqn = NULL;
#endif

  return true;
  }

//==========================================================================

const int PC_DBase::MaxPumpDBCols=100;
//Strng_List PC_DBase::PumpNameList;

PC_DBase::PC_DBase(char * pDBName, char * pDBSheet)
  {
  m_DBName=pDBName;
  m_DBSheet=pDBSheet;

  ///Head=0.0;
//  PumpDBFile=NULL;
  PumpDBSpec=" ";

  iCurDS=0;
  bCrvOK=0;
  }

//--------------------------------------------------------------------------

PC_DBase::~PC_DBase()
  {
  for (int i=0; i<m_DS.GetSize(); i++)
    delete m_DS[i];
  m_DS.SetSize(0);
  }

//==========================================================================
//
//
//
//==========================================================================

flag PC_DBase::OpenPumpDataBase(int DBNo)
  {
  Strng DataFile;
  Strng Fn;
  Fn.Set("%s.MDB", m_DBName());
  if (DBNo==0)
    {
    DataFile.FnSearchExpand(Fn(), AF_All|AF_BackupFiles);
//    DataFile.Set(AdaptFilename2(ss, Fn(), AF_PrjFiles|AF_CfgFiles));
//    DataFile.Set(AdaptFilename2(ss, Fn(), AF_PrjFiles|AF_CfgFiles));
    }
  else
    DataFile.Set("%s%s", BaseCfgFiles(), Fn());

  flag OK = false;
  CFileStatus State;
  if (CFile::GetStatus(DataFile(), State))
    {
    m_TheDB.Reset(DataFile(), m_DBSheet());
    if (m_TheDB.Open(DBConnect_Default, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
      {
      if (m_TheDB.CheckFieldNames(ColNames))
        OK=true;
      else
        {
        LogError("Pumps", 0, "Correct column field names for '%s' not found.", m_DBSheet());
        m_TheDB.Close();
        }
      }
    else
      {
      LogError("Pumps", 0, "Unable to open '%s'", DataFile());
      }
    }
  else if (DBNo>0)
    {
    LogError("Pumps", 0, "'%s' not found", DataFile());
    }
//  if (!OK)
//    m_TheDB.Close();

  return OK;
  }

//--------------------------------------------------------------------------

void PC_DBase::ClosePumpDataBase()
  {
  m_TheDB.Close();
  }

//--------------------------------------------------------------------------

flag PC_DBase::ParseTokens(CStringArray & Values)//char *Buff, int BuffSize, char ** C)
  {
  if (m_TheDB.IsEOF())
    return false;

  m_TheDB.GetNextRow(Values);

  LnNo++;
  return true;
  }

//--------------------------------------------------------------------------

void PC_DBase::InitPumpNameList()
  {
  if (PumpNameList.Length()>0)
    return ;

  flag pcok=0;
  for (int DBNo=0; DBNo<2; DBNo++)
    if (OpenPumpDataBase(DBNo))
      {
      CStringArray Values;
      while (ParseTokens(Values))
        if ((Values[0].GetLength()>0) && (PumpNameList.Find(Values[0])==NULL))
          {
          PumpNameList.Append(Values[0]);
          pcok=1;
          }
      ClosePumpDataBase();
      }
  if (!pcok)
    LogError(m_DBName(), 0, "No Pumps Found");
  }

//--------------------------------------------------------------------------

flag PC_DBase::SetPumpTo(char *RqdSpec)
  {
  bCrvOK=0;
  for (int i=0; i<m_DS.GetSize(); i++)
    delete m_DS[i];
  m_DS.SetSize(0);
  iCurDS=0;
  //char * C[MaxPumpDBCols], Buff[4096];
  flag PCFnd=0, PCDone=0, OK=1;
  for (int DBNo=0; !PCDone && DBNo<2; DBNo++)
    if (OpenPumpDataBase(DBNo))
      {
      CStringArray Values;
      while (!PCDone && ParseTokens(Values))
        if (Values[0].GetLength()>0 && Values[0].CompareNoCase(RqdSpec)==0)
          {
          PumpDBSpec=RqdSpec;
          PCFnd=1;
          OK=OK && AddDataSet(Values);
          }
        else if (PCFnd)
          PCDone=1;


//      while (!PCDone && ParseTokens(Buff, sizeof(Buff), C))
//        if (strlen(C[0])>0 && _stricmp(C[0], RqdSpec)==0)
//          {
//          PumpDBSpec=RqdSpec;
//          PCFnd=1;
//          OK=OK && AddDataSet(C);
//          }
//        else if (PCFnd)
//          PCDone=1;
       ClosePumpDataBase();
       }
  if (!PCFnd)
    {
    Strng S(RqdSpec);
    S.LRTrim();
    if (S.Length()>0)
      LogError(m_DBName(), 0, "Pump '%s' Not Found", S());
    return false;
    }

  m_iRegCnv=m_DS[0]->m_iRegCnv;
  m_sRegCnvTxt=m_DS[0]->m_sRegCnvTxt;

  m_RegName=m_DS[0]->m_RegName;
  m_RegMin=m_DS[0]->m_RegMin;
  m_RegMax=m_DS[0]->m_RegMax;
  m_EffDefined=(m_DS[0]->m_pEffEqn!=NULL);
  m_PwrDefined=(m_DS[0]->m_pPwrEqn!=NULL);

  for (int i=1; i<m_DS.GetSize(); i++)
    {
    if (m_iRegCnv==0 && m_DS[i]->m_iRegCnv>0)
      {
      m_iRegCnv=m_DS[i]->m_iRegCnv;
      m_sRegCnvTxt=m_DS[i]->m_sRegCnvTxt;
      }
    m_DS[i-1]->m_RegMax=m_DS[i]->m_RegMin;
    m_RegMin=Min(m_RegMin, m_DS[i]->m_RegMin);
    m_RegMax=Max(m_RegMax, m_DS[i]->m_RegMax);
    }

  bCrvOK=1;
  return OK;
  }

//--------------------------------------------------------------------------

flag PC_DBase::AddDataSet(CStringArray & Values)//char **C)
  {
  int n=m_DS.GetSize();
  m_DS.SetSize(n+1);
  m_DS[n]=new CPumpDataSet;
  flag OK=m_DS[n]->SetUp(Values/*C*/, PumpDBSpec());

  int i=n;
  while ((i>0) && (m_DS[i]->m_RegMin < m_DS[i-1]->m_RegMin))
    {
    //EXCHANGE(CSPACPumpDataSet, m_DS[i], m_DS[i-1]);
    Exchange(m_DS[i], m_DS[i-1]);
    i--;
    }

  return OK;
  }

//--------------------------------------------------------------------------

void PC_DBase::FindCurrentCurve(double OpReg)
  {
  const int n=m_DS.GetSize();
  if (n==1)
    iCurDS = 1;
  else
    {
    while ((iCurDS < n) && (OpReg>=m_DS[Max(0,iCurDS)]->m_RegMax))
      iCurDS++;
    while ((iCurDS >= 0) && (OpReg<m_DS[Min(n-1,iCurDS)]->m_RegMin))
      iCurDS--;
    }
  };

//--------------------------------------------------------------------------

double PC_DBase::LoVolFlw(double OpReg)
  {
  const int n=m_DS.GetSize();
  if (!bCrvOK || n<1)
    return 0.0;

  FindCurrentCurve(OpReg);

  double Qv=0.0;
  if (iCurDS==n)
    {
    CPumpDataSet &ds=*m_DS[iCurDS-1];
    Qv=ds.m_pHdEqn->MinIndVar1();
    }
  else if (iCurDS<0)
    {
    CPumpDataSet &ds=*m_DS[0];
    Qv=ds.m_pHdEqn->MinIndVar1();
    }
  else
    {
    // interpolate between curves
    // Simple - Should maybe improve this
    CPumpDataSet &ds1=*m_DS[iCurDS];
    CPumpDataSet &ds2=*m_DS[iCurDS+1];

    double Rg1=ds1.m_RegMin;
    double Rg2=ds2.m_RegMin;
    double Qv1=ds1.m_pHdEqn->MinIndVar1();
    double Qv2=ds2.m_pHdEqn->MinIndVar1();

    Qv=Qv1+(Qv2-Qv1)*(OpReg-Rg1)/GTZ(Rg2-Rg1);
    }

  return Qv;
  };

//--------------------------------------------------------------------------

double PC_DBase::HiVolFlw(double OpReg)
  {
  const int n=m_DS.GetSize();
  if (!bCrvOK || n<1)
    return 1.0;

  FindCurrentCurve(OpReg);

  double Qv=1.0;
  if (iCurDS==n)
    {
    CPumpDataSet &ds=*m_DS[iCurDS-1];
    Qv=ds.m_pHdEqn->MaxIndVar1();
    }
  else if (iCurDS<0)
    {
    CPumpDataSet &ds=*m_DS[0];
    Qv=ds.m_pHdEqn->MaxIndVar1();
    }
  else
    {
    // interpolate between curves
    // Simple - Should maybe improve this
    CPumpDataSet &ds1=*m_DS[iCurDS];
    CPumpDataSet &ds2=*m_DS[iCurDS+1];

    double Rg1=ds1.m_RegMin;
    double Rg2=ds2.m_RegMin;
    double Qv1=ds1.m_pHdEqn->MaxIndVar1();
    double Qv2=ds2.m_pHdEqn->MaxIndVar1();

    Qv=Qv1+(Qv2-Qv1)*(OpReg-Rg1)/GTZ(Rg2-Rg1);
    }

  return Qv;
  };

//--------------------------------------------------------------------------

double PC_DBase::HeadAt(double Qv, double OpReg)
  {
  const int n=m_DS.GetSize();
  if (!bCrvOK || n<1)
    return 0.0;

  FindCurrentCurve(OpReg);

  double Hd=0.0;
  if (m_DS.GetSize()==1)
    {
    // interpolate between curve && zero
    CPumpDataSet &ds=*m_DS[0];
    double Rg1=(ds.m_RegMin<ds.m_RegMax-0.001) ? ds.m_RegMin : 0.0;
    double Rg2=ds.m_RegMax;
    double Hd1=0.0;
    double Hd2=ds.m_pHdEqn->Value(Qv, 1.0);

    Hd=Hd1+(Hd2-Hd1)*(OpReg-Rg1)/GTZ(Rg2-Rg1);
    Hd=Cnvs[(ds.iHdCnv)]->Normal(Hd, ds.sHdCnvTxt());
    }
  else if (iCurDS==n)
    {
    CPumpDataSet &ds=*m_DS[iCurDS-1];
    Hd=ds.m_pHdEqn->Value(Qv, OpReg);
    Hd=Cnvs[(ds.iHdCnv)]->Normal(Hd, ds.sHdCnvTxt());
    }
  else if (iCurDS<0)
    {
    CPumpDataSet &ds=*m_DS[0];
    Hd=ds.m_pHdEqn->Value(Qv, OpReg);
    Hd=Cnvs[(ds.iHdCnv)]->Normal(Hd, ds.sHdCnvTxt());
    }
  else
    {
    // interpolate between curves
    // Simple - Should maybe improve this
    CPumpDataSet &ds1=*m_DS[iCurDS];
    CPumpDataSet &ds2=*m_DS[iCurDS+1];

    double Rg1=ds1.m_RegMin;
    double Rg2=ds2.m_RegMin;
    double Hd1=ds1.m_pHdEqn->Value(Qv, OpReg);
    double Hd2=ds2.m_pHdEqn->Value(Qv, OpReg);

    Hd=Hd1+(Hd2-Hd1)*(OpReg-Rg1)/GTZ(Rg2-Rg1);
    Hd=Cnvs[(ds1.iHdCnv)]->Normal(Hd, ds1.sHdCnvTxt());
    }

  return Hd;
  }

//--------------------------------------------------------------------------

PumpHeadTypes PC_DBase::HeadType()
  {
  if (m_DS.GetSize()<1)
    return PHT_Null;
  return m_DS[0]->HeadType();
  }

//--------------------------------------------------------------------------

flag PC_DBase::EffAtHead(double Qv, double OpReg, double &Eff)
  {
  const int n=m_DS.GetSize();
  if (!bCrvOK || n<1)
    return false;

  //while ((iCurDS < n) && (OpReg>=m_DS[Max(0,iCurDS)]->m_RegMax))
  //  iCurDS++;
  //while ((iCurDS >= 0) && (OpReg<m_DS[Min(n-1,iCurDS)]->m_RegMin))
  //  iCurDS--;

  //double Eff=0.0;
  if (iCurDS==n)
    {
    CPumpDataSet &ds=*m_DS[iCurDS-1];
    if (ds.m_pEffEqn)
      Eff=ds.m_pEffEqn->Value(Qv, OpReg);
    else
      return false;
    //Hd=Cnvs[ds.iHdCnv]->Normal(Hd, false, false, ds.sHdCnvTxt());
    }
  else if (iCurDS<0)
    {
    CPumpDataSet &ds=*m_DS[0];
    if (ds.m_pEffEqn)
      Eff=ds.m_pEffEqn->Value(Qv, OpReg);
    else
      return false;
    //Hd=Cnvs[ds.iHdCnv]->Normal(Hd, false, false, ds.sHdCnvTxt());
    }
  else
    {
    // interpolate between curves
    // Simple - Should maybe improve this
    CPumpDataSet &ds1=*m_DS[iCurDS];
    CPumpDataSet &ds2=*m_DS[iCurDS+1];
    if (ds1.m_pEffEqn && ds2.m_pEffEqn)
      {
      double Rg1=ds1.m_RegMin;
      double Rg2=ds2.m_RegMin;
      double Eff1=ds1.m_pEffEqn->Value(Qv, OpReg);
      double Eff2=ds2.m_pEffEqn->Value(Qv, OpReg);

      Eff=Eff1+(Eff2-Eff1)*(OpReg-Rg1)/GTZ(Rg2-Rg1);
      }
    else
      return false;
    //Eff=Cnvs[ds1.iEffCnv]->Normal(Eff, false, false, ds1.sEffCnvTxt());
    }

  return true;
  }

//--------------------------------------------------------------------------

#if WITHPWRCRV

flag PC_DBase::PwrAtHead(double Qv, double OpReg, double &Pwr)
  {
  const int n=m_DS.GetSize();
  if (!bCrvOK || n<1)
    return false;

  //while ((iCurDS < n) && (OpReg>=m_DS[Max(0,iCurDS)]->m_RegMax))
  //  iCurDS++;
  //while ((iCurDS >= 0) && (OpReg<m_DS[Min(n-1,iCurDS)]->m_RegMin))
  //  iCurDS--;

  //double Eff=0.0;
  if (iCurDS==n)
    {
    CPumpDataSet &ds=*m_DS[iCurDS-1];
    if (ds.m_pPwrEqn)
      Pwr=ds.m_pPwrEqn->Value(Qv, OpReg);
    else
      return false;
    //Hd=Cnvs[ds.iHdCnv]->Normal(Hd, false, false, ds.sHdCnvTxt());
    }
  else if (iCurDS<0)
    {
    CPumpDataSet &ds=*m_DS[0];
    if (ds.m_pPwrEqn)
      Pwr=ds.m_pPwrEqn->Value(Qv, OpReg);
    else
      return false;
    //Hd=Cnvs[ds.iHdCnv]->Normal(Hd, false, false, ds.sHdCnvTxt());
    }
  else
    {
    // interpolate between curves
    // Simple - Should maybe improve this
    CPumpDataSet &ds1=*m_DS[iCurDS];
    CPumpDataSet &ds2=*m_DS[iCurDS+1];
    if (ds1.m_pPwrEqn && ds2.m_pPwrEqn)
      {
      double Rg1=ds1.m_RegMin;
      double Rg2=ds2.m_RegMin;
      double Pwr1=ds1.m_pPwrEqn->Value(Qv, OpReg);
      double Pwr2=ds2.m_pPwrEqn->Value(Qv, OpReg);

      Pwr=Pwr1+(Pwr2-Pwr1)*(OpReg-Rg1)/GTZ(Rg2-Rg1);
      }
    else
      return false;
    //Pwr=Cnvs[ds1.iPwrCnv]->Normal(Pwr, false, false, ds1.sPwrCnvTxt());
    }

  return true;
  }
#endif

/*XXXXXXX
#D:#T:Pump Model using a single HQ curve
#X:#n#u<#l<#b<General Description>>>#PThis model allows the user to input a single
Head vs Flow curve and Power vs flow curve for the pump.
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Maximum Flow :>#NThe maximum flow which the pump is capable of producing, in l/s.
#p#b<Curves :>#NThe Head vs flow curve for the pump.  The head should be in m of liquid,
 while the flow is in l/s.
 The Power vs flow curve for the pump.  The flow is in l/s and the power is in kW.
*/
//==========================================================================
//
//
//
//==========================================================================

/*#D:#T:Equation database for a pump
#X:#h<General Description>#nThis pump model allows the user to select a pump from
a configured database which contains pump manufacturers curves.  (At this point
the database contains only Warman pumps, but the user can include more pumps)
#nOnce the curve has been selected the pump model will simulate the actual pump
behaviour regarding head versus flow characteristics.
#nEach pump can have more than one curve, with each individual curve representing a
specific pump speed.  With a normal fixed speed pump the user will specify the
required pump speed.  For a variable speed pump, the user must specify the maximum
and minimum speed of the pump.  A controller can then be used to vary the speed, and
hence the performance of the pump.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Pump Spec> : Select a pump from those available in the pump curve/equations database.#n
#i<HeadDerating> : This can be set to simulate the head inefficiencies of the pump.#n
#i<FlowDerating> : This can be set to simulate the flow inefficiencies of the pump.#n
#i<MaxSpeed> : The maximum rpm of the pump#n
#i<MinSpeed> : The minimum rpm of the pump#n
#i<LoSpeedAction> : Select the pump behaviour at low flow speeds. Does the pump act as
a closed valve (closed), or does it allow flow through the pump body (free flow)?.#n
#i<Efficiency> : The efficiency of the pump as a percentage.#n
#i<Regulation Variable> : This Parameter depends on selected pump spec. It is the variable
which will be used to vary pump performance.  Usually this is pump speed for centrifugal
pumps.#n
#i<ShaftPower> :The installed power on the pump.#n
#i<MinDensity> :The minimum expected density of the fluid to be pumped.#n
#n#n
#n#h<Other>#n
Short name:PC_DBase#n
Model type:Pump Flow Equation#n
#G:Pump Flow Equations
*/

#if WithPCEdt
IMPLEMENT_FLWEQNEDT(PC_DataBase, PumpGroup.Name(), "PC_DBase", "", TOC_SYSTEM, PC_DataBaseEdt,
                 "Pump Database",
                 "Pump model using a database of Equations");
#else
IMPLEMENT_FLWEQN(PC_DataBase, PumpGroup.Name(), "PC_DBase", "", TOC_SYSTEM,
                 "Pump Database",
                 "Pump model using a database of Equations");
#endif

PC_DataBase::PC_DataBase(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  m_PDB("Pumps", "Pumps")//,
  //HeadDeratingCurve("DataOnly", "HdDerating", this, TOA_Embedded)
  {
  //HeadDeratingCurve.Xs().SetCnvs(DC_Qv, "m^3/h");
  //HeadDeratingCurve.Ys().SetCnvs(DC_Frac, "%");

  m_Head=0.0;
  LoSpeedAction=PCLoSpeed_Free;

  fWasStepStart=false;
  m_OpReg=0.0;
  m_PrvOpReg=-1.0;

  m_HdDerating=1.0;
  m_FlwDerating=1.0;

  m_dShaftPower=0.0;
  m_dEfficiency=0.8;
  m_dMinRho=500.0;

  dDamp=0.0;
  }

//--------------------------------------------------------------------------

PC_DataBase::~PC_DataBase()
  {
  }

//--------------------------------------------------------------------------

void PC_DataBase::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Text(" ");
  //DDBValueLstMem DDB0;
  DDBPumpList.Empty();
  m_PDB.InitPumpNameList();
  for (pStrng p=m_PDB.PumpNameList.First() ; p; p=m_PDB.PumpNameList.Next())
    DDBPumpList.Add(p->Str());
  if (DDBPumpList.Length()<1)
    DDBPumpList.Add(" ");
  // Recover old flowsheets
  if (DDB.ForFiling() && DDB.DoingPutData())
    DDB.String("",               "Pump_Spec",    DC_,     "",    xidPumpDBSpec, this, isParm, DDBPumpList());
  DDB.String("",               "Specification",DC_,     "",    xidPumpDBSpec, this, isParm|SetOnChange, DDBPumpList());

  DDB.Text(" ");

  DDB.Double ("",              "HeadDerating", DC_Frac, "%",   &m_HdDerating,    this, 0);//HeadDeratingCurve.Length()<2 ? isParm : 0);
  DDB.Double ("",              "FlowDerating", DC_Frac, "%",   &m_FlwDerating,   this, isParm);
  DDB.Byte   ("LoSpeedAction", "",             DC_,     "",    &LoSpeedAction, this, isParm, DDBLoSpd);

  char    * RegName   = m_PDB.m_RegName() ? m_PDB.m_RegName() : "Reg";
  char    * RegCnvTxt = m_PDB.m_sRegCnvTxt() ? m_PDB.m_sRegCnvTxt() : "";
  CCnvIndex RegCnv    = m_PDB.m_iRegCnv;

  DDB.Text   (" ");
  DDB.Double ("",              "MaxSpeed",     DC_pS,   "rpm",    xidPumpDBMxSpd, this, isResult|0);
  DDB.Double ("",              "MinSpeed",     DC_pS,   "rpm",    xidPumpDBMnSpd, this, isResult|0);
  DDB.Double ("",              RegName,        RegCnv,  RegCnvTxt, &m_OpReg,         this, isResult|0);
  DDB.Double ("",              "Head",         DC_L,    "m",      &m_Head,          this, isResult|0);
  DDB.Double ("",              "MinDensity",   DC_Rho,  "kg/m^3", &m_dMinRho,       this, isResult|0|InitHidden);

  DDB.Text("Power");
  bool SetEff=!m_PDB.PwrDefined() && !m_PDB.EffDefined();
  DDB.Double ("", "Efficiency",                DC_Frac,  "%",   &m_dEfficiency, this  ,SetEff?isParm:0);
  DDB.Double ("", "ShaftPower",                DC_Pwr,   "kW",  &m_dShaftPower, this  ,isResult|0);

  DDB.Double ("", "Damping",                   DC_Frac,  "%",   &dDamp,       this  ,isParm);

  BuildDataDefnOveride(DDB);
  #if WithPCEdt
  if (DDB.ForView())
    {
    // Instantiate the editor
    DDB.BeginObject(this, "PDB", "PC_DBase", NULL, DDB_OptPage);

    DDB.String("",               "ObjSpec",    DC_,   "", xidPumpDBSpec, this, isParm, DDBPumpList());
    //static double xxx=0;
    //DDB.Double ("", "XXX",                 DC_Frac,  "%",   &xxx, this  ,isParm);
    //xxx+=1.0;
    //
    DDB.EndObject();
    }
  #endif
//  #if WithPCEdt
//  if (DDB.ForView())
//    {
//    // Instantiate the editor
//    DDB.BeginObject(this, "PDB", "PC_DBase", NULL, DDB_OptPage);
//    //
//    DDB.EndObject();
//    }
//  #endif
  //DDB.Object(&HeadDeratingCurve,  this, NULL, NULL, DDB_RqdPage);
  }


//--------------------------------------------------------------------------

flag PC_DataBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidPumpDBSpec :
      if (DCB.rpC)
        {
        if (m_PDB.SetPumpTo(DCB.rpC))
          StructureChanged(this);
        }
      DCB.pC=m_PDB.PumpDBSpec() ? m_PDB.PumpDBSpec() : "";
      return 1;
    case xidPumpDBMnSpd:
//      DCB.D=m_pHdEqn ? m_pHdEqn->m_RegMin() : dNAN;
      DCB.D=m_PDB.m_RegMin;
      return 1;
    case xidPumpDBMxSpd:
//      DCB.D=m_pHdEqn ? m_pHdEqn->m_RegMax() : dNAN;
      DCB.D=m_PDB.m_RegMax;
      return 1;
    default:;
    }

  return CFlwEqn::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag PC_DataBase::ValidateData(ValidateDataBlk & VDB)
  {
//  if (!PhDOK())
//    return true;

  //HeadDeratingCurve.Xs().SetCnvs(DC_Qv, "m^3/h");
  //HeadDeratingCurve.Ys().SetCnvs(DC_Frac, "%");

  m_PrvOpReg=-1.0;
  return 1;
  }

//--------------------------------------------------------------------------

#define DumpCrv 0

flag PC_DataBase::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 2.0);
  #if DumpCrv
  if (CFlwEqn::DoDumpTest)
    CFlwEqn::DumpTest("\\PMPCR.csv", 0., 1., 0.01, FE, Regulation, P, Rho, C, IC);
  #endif
  double Rho=FE.MeanRho(pProps);
  FE.SetQmFree();
  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(0.0, 0.0);
  FE.ClrDerivsBad();
  m_Head=0.0;

  if (!m_PDB.OK() || Regulation <1.0e-6)
    return true;

//  if (Regulation<0.01)
//    {
//    //HdDerating=0.0;
//    //FlwDerating=0.0;
//    //MaxBst=0.0;
//    Head=0.0;
//    OpReg=0.0;
//    return true;
//    }

  double m_RegMin = m_PDB.m_RegMin;
  double m_RegMax = m_PDB.m_RegMax;
  m_OpReg = m_RegMin + (m_RegMax-m_RegMin) * Regulation;
  //if (Valid(m_RegMin)) // Variable Speed
  //  {
  //  if ((OpReg < m_RegMin) && (PrvOpReg>=m_RegMin))
  //    LogNote(pAttachedTo->Tag(), 0, "Pump Speed Low [%g RPM]", OpReg * 60);
  //  }
  //else if (Regulation<0.99)
  //  {
  //  PrvOpReg=OpReg=0.0;
  //  FE.SetDerivsGood();
  //  return true;
  //  }

  FE.SetDerivsBad(); // Do Not Use Derivatives
  m_FlwDerating = Range(0.5, m_FlwDerating, 1.0);

  Rho=Max(Rho, m_dMinRho);

  double Qm     = Max(0.0, FE.GetQm());
  double Qv     = Qm/Rho/m_FlwDerating;

  double Bst=m_PDB.HeadAt(Qv, m_OpReg);
  if (m_PDB.HeadType()==PHT_Head)
    Bst *= 9.806 * m_PDB.SG(Rho);

  FE.SetBstDamping(dDamp);
  FE.SetDPb(GEZ(Bst), 0.0);
  FE.SetDPq(0.0, 0.0);

  m_Head    = (FE.DPb()+FE.DPq())*m_HdDerating/(9.806*m_PDB.SG(Rho));

  m_PrvOpReg = m_OpReg;
  return true;
  }

//--------------------------------------------------------------------------

void PC_DataBase::EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB)
  {
  if (!m_PDB.OK())
    return;

  SpConduit &C=FE.Cd();
  if (m_PDB.PwrAtHead(C.QVolume(), m_OpReg, m_dShaftPower))
    {
    m_dEfficiency=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/GTZ(m_dShaftPower);
    }
  else
    {
    m_PDB.EffAtHead(C.QVolume(), m_OpReg, m_dEfficiency);
    m_dEfficiency=Max(1.0e-3,m_dEfficiency);
    m_dShaftPower=0.981*fabs((Po-Pi)*C.QMass())/GTZ(C.Rho())/Max(0.001, m_dEfficiency);
    }

  FTB.AddWorkPower(m_dShaftPower*(1.0-m_dEfficiency), Po);
  }

//==========================================================================
//
//
//
//==========================================================================

//===========================================================================
//
//
//
//===========================================================================
#if WithPCEdt

#define WithExtras 0

const int PC_DataBaseEdt::XPix=10000;
const int PC_DataBaseEdt::YPix=10000;
const int PC_DataBaseEdt::CrvPts=256;
int PC_DataBaseEdt::iGraphWidth=46;
int PC_DataBaseEdt::iGraphHeight=30;


const int Id_CurShow       =  1;
const int Id_CurHide       =  2;
const int Id_GrWidth       = 35;
const int Id_GrHeight      = 36;
const int Id_DispRetained  = 38;
const int Id_AutoScale     = 39;
const int Id_Cumulative    = 40;
const int Id_Fractional    = 41;
const int Id_XLog          = 42;
const int Id_XMin          = 43;
const int Id_XMax          = 44;
const int Id_XInt          = 45;
const int Id_YMin          = 50;
const int Id_YMax          = 60;
//const int Id_YData         = 400;

//------------------------------------------------------------------------------

struct MappingSave
  {
  POINT    VOrg;
  SIZE     VExt;
  POINT    WOrg;
  SIZE     WExt;
  };

//---------------------------------------------------------------------------

void PushScaling(rGDIBlk GB, RECT &GraphR, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_ANISOTROPIC);
  SetViewportOrgEx(GB.DC().m_hDC, GraphR.left, GraphR.top, &MapSv.VOrg);
  SetViewportExtEx(GB.DC().m_hDC, GraphR.right-GraphR.left, GraphR.bottom-GraphR.top, &MapSv.VExt);
  SetWindowOrgEx(GB.DC().m_hDC, 0, PC_DataBaseEdt::YPix, &MapSv.WOrg);
  SetWindowExtEx(GB.DC().m_hDC, PC_DataBaseEdt::XPix, -PC_DataBaseEdt::YPix, &MapSv.WExt);
  }

//---------------------------------------------------------------------------

void PopScaling(rGDIBlk GB, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_TEXT);
  SetViewportOrgEx(GB.DC().m_hDC, MapSv.VOrg.x,  MapSv.VOrg.y, NULL);
  SetViewportExtEx(GB.DC().m_hDC, MapSv.VExt.cx, MapSv.VExt.cy, NULL);
  SetWindowOrgEx(GB.DC().m_hDC, MapSv.WOrg.x,  MapSv.WOrg.y, NULL);
  SetWindowExtEx(GB.DC().m_hDC, MapSv.WExt.cx, MapSv.WExt.cy, NULL);
  }

//---------------------------------------------------------------------------

PC_DataBaseEdt::PC_DataBaseEdt(pFxdEdtView pView_, pPC_DataBase pPC_) :
  FxdEdtBookRef(pView_)//,
//  rPC(*pPC_)
  {
  pPDB=(pPC_ ? pPDB=&pPC_->m_PDB : NULL);

  XCnv.Set(DC_Qm, "kg/s");
  YCnv[0].Set(DC_DP, "kPa");
  YCnv[1].Set(DC_Frac, "%");
  YCnv[2].Set(DC_Pwr, "kW");
  XFmt.Set("", 0, 2, 'f');
  YFmt[0].Set("", 0, 2, 'f');
  YFmt[1].Set("", 0, 2, 'f');
  YFmt[2].Set("", 0, 2, 'f');

  XName="Flow";
  YName[0]="Head";
  YName[1]="Eff";
  YName[2]="Power";

  iDragPtNo = -1;

  XMin=0.0;
  XMax=1.0;
  XFactor=1.0;
  iNameWidth=4;
  iPg1=0;
  fAutoScale=1;

  pLabels = new CLabelWnd;
  pLabels->CreateIt(pView_);
  pLabels->SetFont(pView_->ScrGB.pFont[0]);
#if WithExtras
  pLabels->SetLineCnt(4);
#else
  pLabels->SetLineCnt(2);
#endif

  //pWrkCnv=NULL;
  //pWrkFmt=NULL;
  WrkIB.Clear();
  CursPoint.x=-1;
  }

//---------------------------------------------------------------------------

PC_DataBaseEdt::~PC_DataBaseEdt()
  {

  pLabels->DestroyWindow();

  //cnmdelete pLabels;
  }

//---------------------------------------------------------------------------

int PC_DataBaseEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::Build()
  {
  Strng S;

  SetRanges();

  iPg1=pView->Pages;

  StartPage("Curves");
  if (1) // Head Blk
    {
    StartBlk(2, 0, NULL);
    int L=0;
    SetSpace(L, 1);
    if (AutoScale())
      SetButton(L,"Auto>Fixed Scale", Id_AutoScale, 16, 0, "");
    else
      SetButton(L,"Fixed>Auto Scale", Id_AutoScale, 16, 0, "");
    }


  if (1) // Data Blk
    {
    StartBlk(5, 6, NULL);
    int L=0;
    SetSpace(L,5);
    SetDesc(L, XName(),  -1, 10, 2, "");
    SetDesc(L, YName[0](), -1, 10, 2, "");
#if WithExtras
    SetDesc(L, YName[1](), -1, 10, 2, "");
    SetDesc(L, YName[2](), -1, 10, 2, "");
#endif
    //..
    L++;
    SetDesc(L, XCnv.Text(),  -1, 14,  2, " ");
    for (int c=0; c<3;c++)//DD().Columns.GetSize(); c++)
      //if (DD().Columns[c].fOn)
        SetDesc(L, YCnv[c].Text(),  -1, 10,  2, "");

    L++;
    SetDesc(L, "Min",  -1, 5,  0, "");
    SetParm(L, "", Id_XMin,   10, 2, "");
    SetParm(L, "", Id_YMin,   10, 2, "");
#if WithExtras
    SetParm(L, "", Id_YMin+1, 10, 2, "");
    SetParm(L, "", Id_YMin+2, 10, 2, "");
#endif
    L++;
    SetDesc(L, "Max",  -1, 5,  0, "");
    SetParm(L, "", Id_XMax, 10, 2, "");
    SetParm(L, "", Id_YMax, 10, 2, "");
#if WithExtras
    SetParm(L, "", Id_YMax+1, 10, 2, "");
    SetParm(L, "", Id_YMax+2, 10, 2, "");
#endif
    }

  if (1) // Base Blk
    {
    StartBlk(4, 0, NULL);
    int L=0;
    SetDParm(L,"Graph: Width", 13, "", Id_GrWidth, 2, 2, "");
    SetSpace(L,1);
    SetDParm(L,"Height", 7, "", Id_GrHeight, 2, 2, "");
    L++;
    }
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols)
  {
  if (Pg.No-iPg1>=0)
    {
    Pg.MoveRows(-32000);
    Pg.MoveRows(GraphHeight());
    }
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//header
    }

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XMin :
        XFmt.FormatFloat(XCnv.Human(XDispMin()), Str);
        break;
      case Id_XMax :
        XFmt.FormatFloat(XCnv.Human(XDispMax()), Str);
        break;
      default:;
        if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+3)
          {
          int i=EI.FieldId-Id_YMin;
          YFmt[i].FormatFloat(YCnv[i].Human(YDispMin(i)), Str);
          }
        else if (EI.FieldId>=Id_YMax && EI.FieldId<Id_YMax+3)
          {
          int i=EI.FieldId-Id_YMax;
          YFmt[i].FormatFloat(YCnv[i].Human(YDispMax(i)), Str);
          }
      }
    }
  if (CurrentBlk(EI))
    {//other
    switch (EI.FieldId)
      {
      case Id_GrWidth:   Str.Set("%i", iGraphWidth); break;
      case Id_GrHeight:  Str.Set("%i", iGraphHeight); break;
      }
    }
  }

//---------------------------------------------------------------------------

long PC_DataBaseEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (CurrentBlk(EI))
    {//header
    }
  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_XMin :
        SetXDispMin(XCnv.Normal(SafeAtoF(Str, 0.0)));
        View().DoReload();
        break;
      case Id_XMax :
        SetXDispMax(XCnv.Normal(SafeAtoF(Str, 1.0)));
        View().DoReload();
        break;
      case Id_XInt :
        break;
      default:
        if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+3)
          {
          int i=EI.FieldId-Id_YMin;
          SetYDispMin(i, YCnv[i].Normal(SafeAtoF(Str, 0.0)));
          View().DoReload();
          break;
          }
        else if (EI.FieldId>=Id_YMax && EI.FieldId<Id_YMax+3)
          {
          int i=EI.FieldId-Id_YMax;
          SetYDispMax(i, YCnv[i].Normal(SafeAtoF(Str, 1.0)));
          View().DoReload();
          break;
          };
      }
    }

  if (CurrentBlk(EI))
    {//other
    switch (EI.FieldId)
      {
      case Id_GrWidth:
        iGraphWidth = Range(10L, Str.SafeAtoL(), 60L);
        View().DoRebuild();
        break;
      case Id_GrHeight:
        iGraphHeight = Range(5L, Str.SafeAtoL(), 30L);
        View().DoRebuild();
        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long PC_DataBaseEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {//header
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_AutoScale:
        SetAutoScale(!AutoScale());
        SetRanges();
        View().DoRebuild();
        Fix=1;
        break;
      }
    }

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    }
  return Fix;
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::SetRanges()//CDArray& YAxPts, CDArray& XAxPts)
  {
  if (AutoScale())
    {
    if (1)
      {

      double LoVolFlw=Min(0.0, Min(pPDB->LoVolFlw(0.0), pPDB->LoVolFlw(1.0)));
      double HiVolFlw=Max(pPDB->HiVolFlw(0.0), pPDB->HiVolFlw(1.0));

      double LoHd=Min(0.0, Min(pPDB->HeadAt(HiVolFlw, 0.0), pPDB->HeadAt(HiVolFlw, 1.0)));
      double HiHd=Max(pPDB->HeadAt(LoVolFlw, 0.0), pPDB->HeadAt(LoVolFlw, 1.0));

      SetXDispMin(LoVolFlw);
      SetXDispMax(HiVolFlw);
      SetYDispMin(0, LoHd);
      SetYDispMax(0, HiHd);
      SetYDispMin(1, 0.0);
      SetYDispMax(1, 1.0);
      SetYDispMin(2, 0.0);
      SetYDispMax(2, 1.0);
      }
    else
      {
      }
    }
  SetXDispMax(Max(XDispMax(), XDispMin()+1.0e-6));

  for (int c=0; c<3; c++)
    SetYDispMax(c, Max(YDispMax(c), YDispMin(c)+1.0e-6));
  }

void PC_DataBaseEdt::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {

  if (PgNo-iPg1>=0)
    {

    CDC &DC = GB.DC();
    CDCResChk ResChk(DC);

    SetPosition(GB); //calulate size of GraphR
    flag DoGraph = DC.RectVisible(&GraphR);

    CGdiObject* OldBrush = (CGdiObject*)DC.SelectObject(GB.pBrushGrfBack);

    if (DoGraph)
      {//draw the curve...
      int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);

      COLORREF OldBkColor = DC.SetBkColor(GB.crGrfBack);
      CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);

      DC.DrawEdge(&GraphR, EDGE_SUNKEN, BF_RECT);
      int EdgeX=GetSystemMetrics(SM_CXEDGE);
      int EdgeY=GetSystemMetrics(SM_CYEDGE);
      GraphR.InflateRect(-EdgeX*2, -EdgeY*2);

      DC.Rectangle(&GraphR);
      MappingSave MapSv;
      PushScaling(GB, GraphR, MapSv);

      CRgn ClipRgnGrf;
      ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
      Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);

      CPoint Org = DC.GetWindowOrg();
      CSize Ext = DC.GetWindowExt();

      SetRanges();
      double XDiff = XDispMax()-XDispMin();
      XMin = XDispMin()-0.05*XDiff;
      XMax = XDispMax()+0.05*XDiff;
      //XMin = XDispMin();
      //XMax = XDispMax();
      const double dX = GTZ(XMax-XMin);
      XFactor = XPix/dX;


      for (int c=0; c<3; c++)
        {
        double YDiff = YDispMax(c)-YDispMin(c);
        YMin[c] = YDispMin(c)-0.05*YDiff;
        YMax[c] = YDispMax(c)+0.05*YDiff;
        //YMin[c] = YDispMin(c);
        //YMax[c] = YDispMax(c);
        const double dY = GTZ(YMax[c]-YMin[c]);
        YFactor[c] = YPix/dY;
        }

      //draw axis...
      ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
      int x1,y1;
      x1 = (int)Range((double)-INT_MAX, (0.0-XMin)*XFactor, (double)INT_MAX);
      //x1 = (int)Range((double)-INT_MAX, (rSD.GetNormX(0.0)-XMin)*XFactor, (double)INT_MAX);
      DC.MoveTo(x1,Org.y);
      DC.LineTo(x1,Org.y+Ext.cy);
      for (c=0; c<1/*3*/; c++)
        { // Fix This ??? Make Axes Coincide
        y1 = (int)Range((double)-INT_MAX, (0.0-YMin[c])*YFactor[c], (double)INT_MAX);
        //y1 = (int)Range((double)-INT_MAX, (rSD.GetNormY(0.0)-YMin)*YFactor, (double)INT_MAX);
        DC.MoveTo(Org.x,y1);
        DC.LineTo(Org.x+Ext.cx,y1);
        }

      const FirstPen=3;

      int NXs=50;
      //draw the curves...
      int iPen=FirstPen;
      int XYLen=0;//Xs.GetSize();
      int NDS=pPDB->NDataSets();

      double LoVolFlw=Min(pPDB->LoVolFlw(0.0), pPDB->LoVolFlw(1.0));
      double HiVolFlw=Max(pPDB->HiVolFlw(0.0), pPDB->HiVolFlw(1.0));

      for (c=0; c<NDS; c++)
        {
        CPumpDataSet &CDS=pPDB->DataSet(c);
        POINT XY[CrvPts];
        for (int i=0; i<2; i++)
          {
          double Reg;
          if (i==0)
            {
            Reg=pPDB->DataSet(c).m_RegMin;
            ::SelectObject(DC.m_hDC, GB.hLinePen[iPen]);
            }
          else
            {
            Reg=0.5*(CDS.m_RegMin+CDS.m_RegMax);
            ::SelectObject(DC.m_hDC, GB.hLinePen[iPen+1]);
            }

          XYLen=0;
          for (int j=0; j<=NXs; j++)
            {
            double Qv=XMin+(j*(XMax-XMin))/NXs;
            double Y=pPDB->HeadAt(Qv, Reg);
            int x=CalcXPix(Qv);
            int y=CalcYPix(0, Y);
            XY[XYLen].x = x;
            XY[XYLen].y = y;
            XYLen++;
            }
          DC.Polyline(XY, XYLen);
          }
        }

      // Restore State
      PopScaling(GB, MapSv);
      DC.SelectObject(OldPen);
      DC.SetBkColor(OldBkColor);
      }
    DC.SelectObject(OldBrush);
    }
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::SetPosition(rGDIBlk GB)
  {
  int CW = GB.ColWdt();
  int RH = GB.RowHgt();
  GraphR = GB.LuDataRect();
  GraphR.top -= 1;
  GraphR.right = GraphR.left+GraphWidth()*CW;
  GraphR.bottom = GraphR.top+GraphHeight()*RH;

  GraphR.top+=View().CPg->FixedRows*RH;
  GraphR.bottom+=View().CPg->FixedRows*RH;

  int EdgeX=GetSystemMetrics(SM_CXEDGE);
  int EdgeY=GetSystemMetrics(SM_CYEDGE);
  GraphR.InflateRect(-EdgeX*2, -EdgeY*2);

  //iNameWidth = Max(rSD.XNameLen(), rSD.YNameLen());
  pLabels->SetPosition(GraphR.left, GraphR.bottom+1, iNameWidth + 18);

  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::PointtoLP(POINT &Pt, int c)
  {
  CDC* pDC = &(View().ScrGB.DC());
  CDCResChk ResChk(pDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  pDC->DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::PointtoWP(POINT Pt, double& x, int c, double& y)
  {
  rGDIBlk rGB = View().ScrGB;
  CDC& rDC = rGB.DC();
  CDCResChk ResChk(rDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  rDC.DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);

  x = CalcXVal(Pt.x);
  y = CalcYVal(c, Pt.y);

  }

//---------------------------------------------------------------------------

int PC_DataBaseEdt::ClosestPt(CPoint point, int c)
  {
  double x,y;
  PointtoWP(point, x, c, y);
  int PtNo = -1;
  double dist = DBL_MAX;
  return PtNo;
  }

//---------------------------------------------------------------------------

void PC_DataBaseEdt::ToggleDigCursor(POINT &Pt)
  {
  if (Pt.x>=0 && PtInRect(&GraphR, Pt))
    {
    CClientDC dc(&Wnd(pView));
    CDCResChk ResChk(dc);

    MappingSave MapSv;
    PushScaling(View().ScrGB, GraphR, MapSv);
    CPen APen(PS_DASH,0,RGB(0xFF, 0xFF, 0xFF));//dc.GetNearestColor(Doc().DigColour));
    CGdiObject* OldPen=(CGdiObject*)dc.SelectObject(&APen);
    int OldMode=dc.SetROP2(R2_XORPEN);

    CRgn ClipRgnGrf;
    ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
    int Err = ExtSelectClipRgn(dc.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);

    dc.MoveTo(Pt.x,0);
    dc.LineTo(Pt.x,YPix);
    dc.MoveTo(0, Pt.y);
    dc.LineTo(XPix, Pt.y);
    dc.SetROP2(OldMode);
    PopScaling(View().ScrGB, MapSv);
    CursPoint=Pt;
    }
  else
    {
    CursPoint.x=-1;
    }
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  ToggleDigCursor(point);
  if (PtInRect(&GraphR, point))
    {

    if (nFlags & MK_SHIFT)
      iDragPtNo = ClosestPt(point, 0);
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  ToggleDigCursor(CursPoint);
  if (PtInRect(&GraphR, point))
    {
//    ToggleDigCursor(CursPoint);
    if (iDragPtNo>=0)
      {
      View().DoReload();
      iDragPtNo = -1;
      }
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  FxdEdtView &Vw=View();
  flag ret=true;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      if (EI.Fld)
        {
        if (EI.FieldId==Id_XMin ||
            EI.FieldId==Id_XMax ||
            EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+3 ||
            EI.FieldId>=Id_YMax && EI.FieldId<Id_YMax+3)
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;
          if (EI.FieldId==Id_XMin ||
              EI.FieldId==Id_XMax)
            {
            //pWrkFmt=&XFmt;
            //pWrkCnv=&XCnv;
            WrkIB.Set(EI.Fld->Tag, &XCnv, &XFmt);
            }
          else if (EI.FieldId>=Id_YMin && EI.FieldId<Id_YMin+3)
            {
            //pWrkFmt=&YFmt[EI.FieldId-Id_YMin];
            //pWrkCnv=&YCnv[EI.FieldId-Id_YMin];
            WrkIB.Set(EI.Fld->Tag, &YCnv[EI.FieldId-Id_YMin], &YFmt[EI.FieldId-Id_YMin]);
            }
          else
            {
            //pWrkFmt=&YFmt[EI.FieldId-Id_YMax];
            //pWrkCnv=&YCnv[EI.FieldId-Id_YMax];
            WrkIB.Set(EI.Fld->Tag, &YCnv[EI.FieldId-Id_YMax], &YFmt[EI.FieldId-Id_YMax]);
            }

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          WrkIB.Fmt().AddToMenu(FormatMenu);//, "");
          CMenu CnvMenu;
          CnvMenu.CreatePopupMenu();
          WrkIB.Cnv().AddToMenu(CnvMenu);//, d.iCnv, d.sCnvTxt());

          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
          if ((WrkIB.Cnv().Index())<=0)// || !IsFloatData(d.iType))
            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, /*(CWnd*)*/&View());
          Menu.DestroyMenu();
          }
        }
      }
    }
  return ret;
  //return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoMouseMove(UINT nFlags, CPoint point)
  {
  ToggleDigCursor(CursPoint);
  ToggleDigCursor(point);

  if (PtInRect(&GraphR, point))
    {
    /**/

    double x=0,y0=0,y1=0,y2=0;
    PointtoWP(point, x, 0, y0);
    PointtoWP(point, x, 1, y1);
    PointtoWP(point, x, 2, y2);
    char Buff[256];
    iNameWidth=5;
    sprintf(Buff, "%*s:%#14.5g", iNameWidth, XName, x);
    pLabels->SetText(0, Buff);
    sprintf(Buff, "%*s:%14.5g", iNameWidth, YName[0](), y0);
    pLabels->SetText(1, Buff);
#if WithExtras
    sprintf(Buff, "%*s:%14.5g", iNameWidth, YName[1](), y1);
    pLabels->SetText(2, Buff);
    sprintf(Buff, "%*s:%14.5g", iNameWidth, YName[2](), y2);
    pLabels->SetText(3, Buff);
#endif
    pLabels->Show();
    pLabels->Invalidate();
    if (nFlags & MK_LBUTTON)
      {
      if (nFlags & MK_SHIFT)
        {
        if (iDragPtNo>=0)
          {
          //rSD.MovePt(iDragPtNo, x, y);
          //rSD.bObjModified = 1;
          }
        }
      else
        iDragPtNo = -1;
      //Wnd(pView).InvalidateRect(&GraphR);
      }
    return 1;
    /**/
    }
  else
    {
    /**/
    pLabels->Hide();
    /**/
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoAccCnv(UINT Id)
  {
  pCDataCnv pC=Cnvs[(WrkIB.Cnv().Index())];
  for (int i=Id; i>0; i--)
    pC=pC->Next();

  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoAccFmt(UINT Id)
  {
  for (UINT i=0; i<DefinedFmts.GetSize(); i++)
    if (i==Id)
      break;

  if (i<DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag PC_DataBaseEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

#endif

#undef DllImportExport

//===========================================================================
//
//
//
//===========================================================================
