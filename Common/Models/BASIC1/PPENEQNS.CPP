//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"

#define __PPENEQNS_CPP

#include "ppeneqns.h"
#include "m_link.h"
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

IMPLEMENT_FLWEQN(PE_KFact, PipeEntryGroup.Name(), "PE_KFact", "", TOC_SYSTEM,
                  "K Factor",
                  "K Factor - Darcy");

PE_KFact::PE_KFact(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_PhD.KFact.SetVal(0.5, this);
  //KFactOp=0.5;
  m_dDP=0;
  }

//--------------------------------------------------------------------------

PE_KFact::~PE_KFact()
  {
  }

//--------------------------------------------------------------------------

void PE_KFact::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Density",     "",        DC_Rho, "kg/m^3", &m_dDensMeas,   NULL, isResult);
  DDB.Double ("Velocity",    "Vel",     DC_Ldt, "m/s",    &m_dVelMeas,    NULL, isResult);
  KFact().BuildDataDefn(DDB, "K",  "",  DC_,    "",       xidFlwEqnKFact, NULL, "Calculated", "Required");
  DDB.Double ("PressDrop",   "DP",      DC_DP,  "kPa",    &m_dDP,         NULL, isResult);

  
  BuildDataDefnOveride(DDB);
  };

// --------------------------------------------------------------------------

flag PE_KFact::DataXchg(DataChangeBlk & DCB)
  {
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  return CFlwEqn::DataXchg(DCB);
  }

// --------------------------------------------------------------------------

flag PE_KFact::ValidateData(ValidateDataBlk & VDB)
  {
  m_PhD.KFact.SetVal(ValidateRange(VDB, "K", 0.0, m_PhD.KFact.Val(), 1.0E6), this);
  return True;
  }

//--------------------------------------------------------------------------

flag PE_KFact::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  const double DqScl=1.001;

  m_dDensMeas=Max(0.001, FE.MeanRho(pProps));
  FE.SetVelMeasRange(m_dDensMeas, FE.Area(), 0.001);
  m_dVelMeas=FE.VelMeas();
  m_dVelMeas=Min(m_dVelMeas, 1000.0); // Abs Max Velocity - a bit ridiculous

  // flow into tank K = 1
  m_PhD.KFact.SetVal(FE.QmSign()>=0? 1.0 : 0.5, this);

  double dPq2, dPq1;
  if (FE.Linearised())
    {
    double Qm1  = FE.QmMeas();
    double dQm  = FE.DQmMeas(DqScl);
    double Qm2  = FE.QmMeas(DqScl);
    dPq2=FE.LinearisedDP(Qm2, Regulation);
    dPq1=FE.LinearisedDP(Qm1, Regulation);
    FE.SetDPq(-FE.QmSign() * dPq1, -FE.QmSign() * (dPq2 - dPq1)/dQm);
    }
  else
    {
    dPq1=PressDropKFact(m_dVelMeas, m_dDensMeas, m_PhD.KFact(), 1.0);
    dPq2=PressDropKFact(m_dVelMeas*DqScl, m_dDensMeas, m_PhD.KFact(), 1.0);

    FE.SetDPq(-FE.QmSign()*dPq1, -FE.QmSign()*(dPq2-dPq1)/(FE.DQmMeas(DqScl)));
    }

  double OnePhDPZ=-0.001*9.81*FE.Rise()*m_dDensMeas;

  FE.SetVelocity(FE.QmSign()*m_dVelMeas);
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  //FE.XSetTempOut(4);

  FE.SetDPz(OnePhDPZ);

  m_dDP=dPq1;

  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe
#X:#h<General Description>#nThis is a flow equation for a pipe.
It is a linear flow equation, as it assumes that the pressure drop within a
pipe is proportional to the mass flow within the pipe.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the pipe.#n
#i<Length> : The physical length of the pipe.#n
#i<Diam> : The inner pipe diameter.#n
#n
#n#h<Other>#n
Short name:FE_Line#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/

IMPLEMENT_FLWEQN(PE_Linear, PipeEntryGroup.Name(), "PE_Line", "", TOC_SYSTEM,
                  "DP Prop to Qm",
                  " Pressure drop relationship dP proportioal to Qm");

PE_Linear::PE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=0.01;
  m_dDP=0;
  //Len_Rqd=10.0;
//  Diam=0.1;
  }

//--------------------------------------------------------------------------

PE_Linear::~PE_Linear()
  {
  }

//--------------------------------------------------------------------------

void PE_Linear::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Resistance",   "Resistn",  DC_,     "",      &R_Fxd,     NULL,  isParm);
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",  &m_dDP,     NULL, isResult);
  //DDB.Double ("Length",       "",         DC_L,    "m",     &Len_Rqd,      NULL,  isParmStopped);
//  DDB.Double ("Diam",         "",         DC_L,    "mm",    &Diam,         NULL,  isParm);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag PE_Linear::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  m_dDP=fabs(FE.QmMeas()*R_Fxd);
  FE.SetDPq(-FE.QmSign()*FE.QmMeas()*R_Fxd, -FE.QmSign()*R_Fxd);
  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe
#X:#h<General Description>#nThis is a flow equation for a pipe.
It is a linear flow equation, as it assumes that the pressure drop within a
pipe is proportional to the mass flow within the pipe.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the pipe.#n
#i<Length> : The physical length of the pipe.#n
#i<Diam> : The inner pipe diameter.#n
#n
#n#h<Other>#n
Short name:FE_Line#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/

IMPLEMENT_FLWEQN(PE_SimpleVel, PipeEntryGroup.Name(), "PE_SmplV", "", TOC_SYSTEM,
                  "Simple Velocity",
                  "Simple Velocity");

PE_SimpleVel::PE_SimpleVel(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  Diam=0.1;
  PwrLaw=2.0;
  OpDP=1.0;
  OpVel=10.0;
  m_dDP=0;
  }

//--------------------------------------------------------------------------

PE_SimpleVel::~PE_SimpleVel()
  {
  }

//--------------------------------------------------------------------------

void PE_SimpleVel::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Diam",     "",   DC_L,   "mm",    &Diam,         NULL,  isParm);
  DDB.Double ("PowerLaw", "",   DC_,    "",      &PwrLaw,       NULL,  isParm);
  DDB.Double ("Oper_DP",  "",   DC_DP,  "kPa",   &OpDP,         NULL,  isParm);
  DDB.Double ("Oper_Vel", "",   DC_Ldt, "m/s",   &OpVel,        NULL,  isParm);
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag PE_SimpleVel::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  /*
  double Area=PI/4.0*Sqr(Diam);
  double K=fabs(OpDP)/Pow(fabs(OpVel), PwrLaw);
  double RhoArea=Rho*Max(Area, 1.0e-6);
  //double Vel=FE.QmMeas()/RhoArea;

  FE.dPq   = -FE.QmSign()*K*Pow(FE.QmMeas()/RhoArea, PwrLaw);
  FE.dPqdQ = -FE.QmSign()*PwrLaw*K*Pow(FE.QmMeas()/RhoArea, (PwrLaw-1.0));
//  FE.SetArea(PI/4.0*Sqr(Diam));
//  FE.SetMomentum(Rho);


  FE.dPq   = -FE.QmSign()*K*Pow((Vel/const_den),PwrLaw);
  FE.dPqdQ = -FE.QmSign()*pwr_fac_fnt*K*Pow(FE.QmMeas(),PwrLaw-1)/Pow((const_den*rhoArea),PwrLaw);

  */

  double Rho=Max(0.1, FE.MeanRho(pProps));

  double Area=PI/4.0*Sqr(Diam);
  double K=fabs(OpDP)/Pow(fabs(OpVel), PwrLaw);
  double RhoArea=Rho*Max(Area, 1.0e-6);

  //FE.SetVelMeasRange(FE.QmMeas()/RhoArea, 0.001);
  FE.SetVelMeasRange(Rho, Area, 0.001);
  double Vel1 = FE.VelMeas();
  double dQm  = FE.DQmMeas(1.001);
  double Vel2 = FE.VelMeas(1.001);


  double dPq1 = -FE.QmSign()*K*Pow(Vel1,PwrLaw);
  double dPq2 = -FE.QmSign()*K*Pow(Vel2,PwrLaw);

  m_dDP=fabs(dPq1);
  FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);

  FE.SetVelocity(FE.GetQm()/RhoArea);

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe
#X:#h<General Description>#nThis is a flow equation for a pipe.
It is a linear flow equation, as it assumes that the pressure drop within a
pipe is proportional to the mass flow within the pipe.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the pipe.#n
#i<Length> : The physical length of the pipe.#n
#i<Diam> : The inner pipe diameter.#n
#n
#n#h<Other>#n
Short name:FE_Line#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/

IMPLEMENT_FLWEQN(PE_VolFlow, PipeEntryGroup.Name(), "PE_VFlw", "", TOC_SYSTEM,
                  "Simple Volumetric Flow",
                  "Simple Volumetric Flow");
//
PE_VolFlow::PE_VolFlow(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam=0.1;
  PwrLaw=2.0;
  OpDP=1.0;
  OpVol=dNAN;
  OpNVol=10.0;
  m_dDP=0;

  }

//--------------------------------------------------------------------------

PE_VolFlow::~PE_VolFlow()
  {
  }

//--------------------------------------------------------------------------

void PE_VolFlow::BuildDataDefn(DataDefnBlk & DDB)
  {
//  DDB.Double ("Diam",     "",   DC_L,   "mm",    &Diam,         NULL,  isParm);
  DDB.Double ("PowerLaw",  "",   DC_,    "",       &PwrLaw,       NULL,  isParm);
  DDB.Double ("Oper_DP",   "",   DC_DP,  "kPa",    &OpDP,         NULL,  isParm);
  DDB.Double ("Oper_Vol",  "",   DC_Qv,  "m^3/s",  &OpVol,        NULL,  isParm|NAN_OK);
  DDB.Double ("Oper_NVol", "",   DC_NQv, "Nm^3/s", &OpNVol,       NULL,  isParm|NAN_OK);
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag PE_VolFlow::ValidateData(ValidateDataBlk & VDB)
  {
  if (Valid(OpVol))
    OpNVol=dNAN;
  else if (!Valid(OpNVol))
    OpNVol=10.0;
  return True;
  }

//--------------------------------------------------------------------------

flag PE_VolFlow::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double dPq1, dPq2;
  if (Valid(OpVol))
    {
    double Rho=Max(0.1, FE.MeanRho(pProps));
    double K=fabs(OpDP)/Pow(fabs(OpVol), PwrLaw);
    double Vol1 = FE.SetQvMeasRange(Rho, 1.0);
    double dQm  = FE.DQmMeas(1.001);
    double Vol2 = FE.QvMeas(1.001);

    dPq1 = -FE.QmSign()*K*Pow(Vol1,PwrLaw);
    dPq2 = -FE.QmSign()*K*Pow(Vol2,PwrLaw);
    FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);
    }
  else
    {
    double NRho=Max(0.1, FE.MeanRho(pProps)*Norm_P/GTZ(FE.MeanPress())*FE.MeanTemp(pProps)/Norm_T);
    double K=fabs(OpDP)/Pow(fabs(OpNVol), PwrLaw);
    double NVol1 = FE.SetQvMeasRange(NRho, 1.0);
    double dQm  = FE.DQmMeas(1.001);
    double NVol2 = FE.QvMeas(1.001);

    dPq1 = -FE.QmSign()*K*Pow(NVol1,PwrLaw);
    dPq2 = -FE.QmSign()*K*Pow(NVol2,PwrLaw);
    FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);
    }

  m_dDP=fabs(dPq1);
  FE.SetFunctOfPress();

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
