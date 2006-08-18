//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"

#define __PIPEEQNS_CPP

#include "pipeeqns.h"
#include "m_link.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Darcy DP for a pipe
#X:#h<General Description>#nThis is a method of calculating the pressure drop
versus flow relationship for a pipe. The Darcy equation which is used for calculating
the pressure drop within a pipe is usually given in the following form:
#nF = (4fL/D)V^2/2gc
#nwhere
#nF  = Friction loss in specific energy
#nD  = Pipe diameter
#nL  = Pipe length
#nV  = Fluid velocity
#ngc = Dimensional constant
#nf  = Fanning friction factor.

#nHowever, since the Fanning friction factor is a function of the Reynolds number, as is
usually read from a graph.  Various equations have been devloped which allow
the Fanning friction factor to be calculated directly.  The equation used in this
model is the Churchill equation.#n
Ref: Churchill, S.W., Friction factor equation spans all flow regimes,
Chem. Eng., Nov. 7, 1977.

#n#n
#n#h<Variables to be supplied by the user>#n
#i<Roughn> : The average roughness of the pipe.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#i<Viscosity> : This is a user input.#n
#n
#n#h<Other>#n
Short name:FE_Darcy#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/

#if WithOEP
double  FE_Darcy::dFitLenScl=1.0;
#endif

IMPLEMENT_FLWEQN(FE_Darcy, PipeGroup.Name(), "FE_Darcy", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Darcy DP",
                 "Darcy DP due to flow in a pipe");

FE_Darcy::FE_Darcy(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dResMagnFac = 1.0;
  m_dResPowrFac = 1.0;
  m_dResRevMult = 1.0;
  m_dAgeResFac  = 0.0;
  m_dRough      = 0.00005;// 0.05 mm   Commercial steel
  m_iFlwRgm     = 0; // laminar flow
#if WithOEP
  Len_Fit       = 0.0;// 1 m
#endif
  m_dLoLimVel   = 0.001;
  m_dFricFac=0.0;
  m_dDP=0;
  m_dRe=0;
  }

//--------------------------------------------------------------------------

FE_Darcy::~FE_Darcy()
  {
  }

//--------------------------------------------------------------------------

#define RQDLEN (PrjFileVerNo()<36? "Act_Len" : "Rqd_Len")
const byte FTLiq = 0;
const byte FTVap = 1;
const byte FTMix = 2;
static DDBValueLst DDBFT[]={
  {FTLiq, "Liquid"},
  {FTVap, "Gas"},
  {FTMix, "Mixed"},
  {0}};

void FE_Darcy::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDEF_Flags ViscFlags=isResult;
  if (PrjFileVerNo()<34) // && DDB.ForFileSnpScn())
    ViscFlags=isParm;


  DDB.Double ("PipeRoughness",   "Roughn",    DC_L,    "mm",     &m_dRough,           NULL, isParm);
  DDB.Double ("Diameter",        "Diam",      DC_L,    "mm",     xidFlwEqnDiam,       NULL, isParmStopped);
  DDB.Double ("Length",          RQDLEN,      DC_L,    "m",      xidFlwEqnRqdLen,     NULL, isParmStopped);
  DDB.Double ("FittingLength",   "Fit_Len",   DC_L,    "m",      xidFlwEqnFitLen,     NULL, isParmStopped);
  DDB.Double ("CalcLength",      "CalcLen",   DC_L,    "m",      xidFlwEqnActLen,     NULL, isResult);
  DDB.Double ("Magnitude_Factor","",          DC_Frac, "%",      &m_dResMagnFac,      NULL, isParm|InitHidden);
  DDB.Double ("PowerLaw_Factor", "",          DC_Frac, "%",      &m_dResPowrFac,      NULL, isParm|InitHidden);
  DDB.Double ("RevFlowMult",     "",          DC_Frac, "%",      &m_dResRevMult,      NULL, isParm|InitHidden);
  DDB.Byte   ("FlwType",         "",          DC_,     "",       &m_iFlwType,         NULL, isResult, DDBFT);
  DDB.Double ("Density",         "",          DC_Rho,  "kg/m^3", &m_dDensMeas,        NULL, isResult);
  DDB.Double ("Viscosity",       "",          DC_Visc, "cP",     &m_dViscMeas,        NULL, ViscFlags);
  DDB.Double ("Velocity",        "Vel",       DC_Ldt,  "m/s",    &m_dVelMeas,         NULL, isResult);
  DDB.Double ("SonicVelocity",   "VelSonic",  DC_Ldt,  "m/s",    &m_dVelSonicMeas,    NULL, isResult|NAN_OK);
  DDB.Double ("ReynoldsNo",      "Re",        DC_,     "",       &m_dRe,              NULL, isResult);
  DDB.Double ("FricFac",         "f",         DC_,     "",       &m_dFricFac,         NULL, isResult);
  if (PrjFileVerNo()<36)
    {
    DDB.Double ("K_Ovr",           "",          DC_,     "",       xidFlwEqnKFactOvr, NULL, isParm|NAN_OK);
    DDB.Double ("K",               "",          DC_,     "",       xidFlwEqnKFact,    NULL, isResult);
    }
  else
    KFact().BuildDataDefn(DDB, "K",  "", DC_, "",   xidFlwEqnKFact,  NULL, "Calculated", "Required");
  //DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);

  BuildDataDefnOveride(DDB);
#if WithOEP
  if (OEPOptions())
    DDB.Double ("FitLenScl",       "",          DC_Frac, "%",      &dFitLenScl,        NULL, isParm|InitHidden);
#endif

  if (PrjFileVerNo()<34 && DDB.ForFileSnpScn() && DDB.DoingPutData())
    m_dOverideViscRqd=m_dViscMeas;

  };

//--------------------------------------------------------------------------

flag FE_Darcy::ValidateData(ValidateDataBlk & VDB)
  {
//  if (!PhDOK())
//    return true;

  if (NetProbalMethod())
    return true;

  SetDiam(ValidateRange(VDB, "Diameter", 0.001, Diam()));

  flag OK=CFlwEqn::ValidateData(VDB);

  m_dRough=ValidateRange(VDB, "Roughness", 1e-12, m_dRough);
  m_dResMagnFac=ValidateRange(VDB, "Magn_Fac", 0.1, m_dResMagnFac, 10.);
  m_dResPowrFac=ValidateRange(VDB, "Powr_Fac", 0.1, m_dResPowrFac, 10.);
  m_dResRevMult=ValidateRange(VDB, "RevMult", 1.0, m_dResRevMult, 1000.);
  
  ValidateRange(VDB, "Length", 0.1,  TotLength(), dNAN, &OK);
  ValidateRange(VDB, "K_Fact_Over", 1e-6, m_PhD.KFact.dOvr, dNAN, &OK);

  //if (PrjFileVerNo()<34)
  //  {
  //  dOverideViscRqd=ViscMeas;
  //  }
  return OK;
  }

//--------------------------------------------------------------------------

void FE_Darcy::SetPhysData(double DZ)
  {
  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
  SetRise(DZ);
#if WithOEP
  if (OEPOptions())
    SetFitLength(Len_Fit*dFitLenScl);
#endif
  };

//--------------------------------------------------------------------------

double FE_Darcy::dP_Vel_Fn(double VVolFrac, double Vel, double QmL, double QmV, double RhoM, double Visc, double Length, CFBPhysData & PhD, CFlwBlkBase &FE, flag &BadDerivs)
  {
  const double VF=VVolFrac;
  if (VF>0.99)
    m_iFlwType=FTVap;
  else if (VF<0.01)
    m_iFlwType=FTLiq;
  else
    m_iFlwType=FTMix;

  double DPl,DPv,DP;
  double Diam=m_PhD.Diam();
  //ViscMeas= FE.MeanViscosity(pProps);
  //DensMeas= Rho;
  m_dRe   = (Diam*Vel*RhoM)/Visc;        //Reynold's number

  // Churchill equation Ref: Churchill, S.W., Friction factor equation spans all flow regimes,
  // Chem. Eng., Nov. 7, 1977.
  const double A = Pow((-2.457 * log(Pow(7.0/m_dRe,0.9) + 0.27 * m_dRough/Diam)), 16);
  const double B = Pow((37530.0/m_dRe), 16);
  m_dFricFac     = 8.0 * Pow((Pow((8/m_dRe), 12) + 1.0/Pow((A + B), 1.5)),0.0833);
  //m_PhD.KFact.dVal = FricFac* Length / Diam;
  SetKFact(m_dFricFac* Length / Diam);
  if (m_iFlwType!=FTVap)
    {
    const double SG      = RhoM / 1000.0;
    const double Age_Fac = 1.0 + 0.03 * m_dAgeResFac;
    const double pwrFac  = 2.0 * m_dResPowrFac;
    DPl = 0.5 * m_PhD.KFact() * SG * m_dResMagnFac * Age_Fac * Pow(Vel,pwrFac);
    }

  if (m_iFlwType!=FTLiq)
    {
    //compress flow Eqn 1.7 CRANE
    double P1=((FE.QmSign()>0) ? FE.FlngPress() : FE.NodePress())*1000;
    double Area=PI*Diam*Diam/4.0* Max(0.01, VVolFrac);
    double spVol=1.0/RhoM;
    double X=Diam*Area*Area/(spVol*m_dFricFac*Length);
    double Y=P1*P1-QmV*QmV*P1/X;
    double P2=Sqrt(GTZ(Y));
    if (P2<P1*0.05)
      {
      P2=P1*0.05;
      }
    DPv=(P1-P2)*0.001; // to kPa
    }

  // Is this Velocity Limiting ??
  //double QmVMax=Sqrt(GTZ(0.95*P1*X));
  //if (QmV>QmVMax)
  //  BadDerivs=true;

  if (m_iFlwType==FTVap)
    DP=DPv;
  else if (m_iFlwType==FTLiq)
    DP=DPl;
  else
    DP=DPv*VF + DPl*(1.0-VF);

//  dbgpln("Dar %10.4e %10.4e %10.2f %10.2f", FE.QmMeas(), Vel, VVolFrac, DP); 
  return DP;
  }

//--------------------------------------------------------------------------

#define DumpCrv 0

flag FE_Darcy::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Diam=m_PhD.Diam();
  #if DumpCrv
  if (CFlwEqn::DoDumpTest)
    CFlwEqn::DumpTest("\\darcy.csv", 0. 250., 5., FE, Regulation, P, Rho, C, IC);
  #endif

  #ifndef _RELEASE
  if (pAttachedTo->fDoDbgBrk)
    { int xxx=0; }
  #endif

  flag BadDerivs=false;

  double MnVel=0;
  m_dDP=0;
  FE.ClrDPq();
  FE.ClrMomentum();
  FE.ClrDPz();
  long iProp=0;
  for (;;) 
    {
    double P1=Max(1.0, ((FE.QmSign()>0) ? FE.FlngPress() : FE.NodePress()));
    double Vf=FE.NettVVolFrac(pProps);
    double Lf=FE.NettLVolFrac(pProps);
    double RhoL=FE.MeanRhoL(pProps);
    double RhoV=FE.MeanRhoV(pProps, P1);
    m_dDensMeas=FE.MeanRho(pProps, P1);
    m_dViscMeas=FE.MeanViscosity(pProps);

    double rhoArea = m_dDensMeas * PI * Sqr(Diam/2.0);
    
    FE.SetArea(PI * Sqr(Diam/2.0));
    FE.SetVelMeasRange(m_dDensMeas, PI * Sqr(Diam/2.0), m_dLoLimVel);
    
    double Qm1  = FE.QmMeas();
    double dQm  = FE.DQmMeas(1.001);
    double Qm2  = FE.QmMeas(1.001);
    double Vel1 = FE.VelMeas();
    double Vel2 = FE.VelMeas(1.001);
    double FlngP=Max(1.0, FE.FlngPB().P);
    double QlFrac=(RhoL*Lf)/GTZ(RhoL*Lf+RhoV*Vf);
    double QvFrac=1-QlFrac;
    m_dVelSonicMeas=Sqrt(1.4*FlngP*1000/GTZ(FE.MeanRhoV(pProps, FlngP)));


    //if (DoDbgBrk())
    //  dbgpln("DAR %10.2e %10.2e %10.2e %10.2e %10.2e %10.2e", FE.m_Qm, Qm1, dQm, Qm2, Vel1, Vel2);

    if (Vel1>(4*m_dVelSonicMeas)) // catch ridiculous flows
      {
      double Scl=Vel1/(4*m_dVelSonicMeas);
      Vel1/=Scl;
      Vel2/=Scl;
      Qm1/=Scl;
      Qm2/=Scl;
      dQm/=Scl;
      }
    m_dVelMeas=Vel1;

    double dPq2,dPq1;
    if (FE.Linearised())
      {
      dPq2 = FE.LinearisedDP(Qm2, Regulation);
      dPq1 = FE.LinearisedDP(Qm1, Regulation);
      }
    else
      {
      double Len = pProps ? pProps->m_dLenFraction*FE.TotLength() : FE.TotLength();

      // Eval @ Actual flow last
      dPq2 = dP_Vel_Fn(Vf, Vel2, Qm2*QlFrac, Qm2*QvFrac, m_dDensMeas, m_dViscMeas, Len, m_PhD, FE, BadDerivs);
      dPq1 = dP_Vel_Fn(Vf, Vel1, Qm1*QlFrac, Qm1*QvFrac, m_dDensMeas, m_dViscMeas, Len, m_PhD, FE, BadDerivs);
      if (dPq2 == dPq1)
        {
        dPq2 = dPq1 * 1.0001; // allow it to move
        }
      }

    if (FE.m_iDirnRel2Connect * FE.QmSign() < 0)
      {
      dPq1*=m_dResRevMult;
      dPq2*=m_dResRevMult;
      }

    #ifndef _RELEASE
    if (pAttachedTo->fDoDbgBrk)
      { int xxx=0; }
    #endif

    FE.AddDPq(-FE.QmSign() * dPq1, -FE.QmSign() * (dPq2 - dPq1)/dQm);
    FE.AddMomentum(pProps, FE.MeanFBPress());
    FE.AddDPz(FE.StaticHead(pProps), 0);

    MnVel+=FE.GetQm()/rhoArea;
    m_dDP+=dPq1;
    iProp++;

    if (DoDbgBrk())
      {
      dbgpln("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f",
             P1,Vf,Lf,RhoL,RhoV,m_dDensMeas,m_dViscMeas,rhoArea,Qm1,dQm,Qm2,Vel1,Vel2,dPq1,dPq2,(dPq2 - dPq1)/dQm);
      //dbgpln("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f",
      //       P1,Vf,Lf,RhoL,RhoV,m_dDensMeas,m_dViscMeas,rhoArea,Qm1,QlFrac,QvFrac,dQm,Qm2,Vel1,Vel2);
      }

    if (!pProps || pProps->m_bLastSegment)
      break;
    pProps++;
    }

  FE.SetDerivsBad(BadDerivs);
  FE.SetVelocity(MnVel/iProp, m_dVelSonicMeas);
  //FE.SetFunctOfPress();

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe
#X:#h<General Description>#nThis is a very simple pressure drop versus flow
equation for a pipe.  The pressure drop in the pipe due to flow is assumed to be
proportional to the mass flow within the pipe, i.e a linear equation is used to
calculate the pressure drop.
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

IMPLEMENT_FLWEQN(FE_Linear, PipeGroup.Name(), "FE_Line", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                  "DP Prop to Qm",
                  " Pressure drop relationship dP proportioal to Qm");

FE_Linear::FE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=10000.0;
  //Len_Rqd=10.0;
//  Diam=0.1;
  }

//--------------------------------------------------------------------------

FE_Linear::~FE_Linear()
  {
  }

//--------------------------------------------------------------------------

void FE_Linear::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Resistance",   "Resistn",  DC_,     "",      &R_Fxd,          NULL,  isParm);
  DDB.Double ("Length",       "",         DC_L,    "m",     xidFlwEqnRqdLen, NULL,  isParmStopped);
  DDB.Double ("Diam",         "",         DC_L,    "mm",    xidFlwEqnDiam,   NULL,  isParm);
  //DDB.Double ("TempRise",     "",         DC_dT,   "C",     &dTRiseRqd,      NULL,  isParm|NAN_OK);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

void FE_Linear::SetPhysData(double DZ)
  {
//  CFBPhysData &D=PhDCfg();
  SetActLength(Max(fabs(DZ), RqdLength()));
  SetRise(DZ);

//  D.SetDiam(Diam);
  };

//--------------------------------------------------------------------------

flag FE_Linear::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  FE.SetDPq(-FE.QmSign()*FE.QmMeas()*FE.TotLength()*R_Fxd, -FE.QmSign()*FE.TotLength()*R_Fxd);
//  FE.SetArea(PI/4.0*Sqr(Diam));
  FE.SetMomentum(pProps, FE.MeanFBPress());
  FE.SetKFact(0);

  FE.SetDPz(FE.StaticHead(pProps));

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe
#X:#h<General Description>#nThis is a very simple pressure drop versus flow
equation for a pipe.  The pressure drop in the pipe due to flow is assumed to be
proportional to the mass flow within the pipe, i.e a linear equation is used to
calculate the pressure drop.
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

IMPLEMENT_FLWEQN(FE_DPperQV, PipeGroup.Name(), "FE_DPQv", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                  "DP Prop to Qv",
                  "Pressure drop relationship dP proportioal to Qv");

FE_DPperQV::FE_DPperQV(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=1000.0;
  //Len_Rqd=10.0;
//  Diam=0.1;
  }

//--------------------------------------------------------------------------

FE_DPperQV::~FE_DPperQV()
  {
  }

//--------------------------------------------------------------------------

void FE_DPperQV::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Resistance",   "Resistn",  DC_DPperVol, "kPa/m^3/s",  &R_Fxd,          NULL,  isParm);
  DDB.Double ("Length",       "",         DC_L,        "m",          xidFlwEqnRqdLen, NULL,  isParmStopped);
  DDB.Double ("Diam",         "",         DC_L,        "mm",         xidFlwEqnDiam,   NULL,  isParmStopped);
  //DDB.Double ("TempRise",     "",         DC_dT,       "C",        &dTRiseRqd,      NULL,  isParm|NAN_OK);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag FE_DPperQV::ValidateData(ValidateDataBlk & VDB)
  {
  if (NetProbalMethod())
    return true;

  SetDiam(ValidateRange(VDB, "Diameter", 0.001, Diam()));

  flag OK=CFlwEqn::ValidateData(VDB);

  ValidateRange(VDB, "Length", 0.1,  TotLength(), dNAN, &OK);

  return OK;
  }

//--------------------------------------------------------------------------

void FE_DPperQV::SetPhysData(double DZ)
  {
  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
  SetRise(DZ);
  };

//--------------------------------------------------------------------------

flag FE_DPperQV::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Qm=GTZ(fabs(FE.QmMeas()));
  FE.SetQvMeasRange(FE.MeanRho(pProps), 1e-6);
  double Qv0=FE.QvMeas();
  double Qv1=FE.QvMeas(1.01);
  double DP0=Qv0*FE.TotLength()*R_Fxd;
  double DP1=Qv1*FE.TotLength()*R_Fxd;

  FE.SetDPq(-FE.QmSign()*DP0, -FE.QmSign()*(DP1-DP0)/(FE.DQmMeas(1.01)));
  FE.SetMomentum(pProps, FE.MeanFBPress());
  FE.SetKFact(0);

  FE.SetDPz(FE.StaticHead(pProps));

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Hazen-Williams for a pipe
#X:#h<General Description>#nThis is a method of calculating the pressure drop
versus flow relationship for a pipe.  The Hazen and Williams method of calculating
energy loss as a function of mass flow is used.  This method tends to be conservative,
but is popular for use with long slurry pipe lines.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<AgeRF> : The resistance factor due to the age of the pipe,this is either a
a value from 0.0 to 1.0 or the age of the pipe in years.#n
#i<RMagnF> : The resistance magnitude factor, a value from 0.0 to 1.0.#n
#i<RPowrF> : The resistance power factor, a value from 0.0 to 1.0.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#n
#n#h<Other>#n
Short name:FE_Hazen#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/
IMPLEMENT_FLWEQN(FE_Hazen, PipeGroup.Name(), "FE_Hazen", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Hazen-Williams",
                 "Hazen-Williams DP due to flow in a pipe");

FE_Hazen::FE_Hazen(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam=0.1; // 100 mm inner diam
  Res_Magn_Fac = 1.0;
  Res_Powr_Fac = 1.0;
  Age_Res_Fac = 1.0;
  FlwRgm=0; // laminar flow
//  Len_Rqd=1.0;
//  Len_Fit=0.0;
  LoLimVel=0.001;
  }

//--------------------------------------------------------------------------

FE_Hazen::~FE_Hazen()
  {
  }

//--------------------------------------------------------------------------

void FE_Hazen::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("AgeResFactor", "AgeRF",    DC_,   "",     &Age_Res_Fac,  NULL,  isParm);
  DDB.Double ("ResMagnFactor","RMagnF",   DC_,   "",     &Res_Magn_Fac, NULL,  isParm);
  DDB.Double ("ResPowrFactor","RPowrF",   DC_,   "",     &Res_Powr_Fac, NULL,  isParm);
  DDB.Double ("Diameter",     "Diam",     DC_L,  "mm",   xidFlwEqnDiam,        NULL,  isParmStopped);
  DDB.Double ("Length",       RQDLEN,     DC_L,  "m",    xidFlwEqnRqdLen,      NULL,  isParmStopped);
  DDB.Double ("FittingLength","Fit_Len",  DC_L,  "m",    xidFlwEqnFitLen,      NULL,  isParmStopped);
  DDB.Double ("CalcLength",    "CalcLen", DC_L,  "m",    xidFlwEqnActLen,      NULL,  isResult);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

void FE_Hazen::SetPhysData(double DZ)
  {
  //CFBPhysData &D=PhDCfg();
  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
  SetRise(DZ);

  //D.SetDiam(Diam);
  };

//--------------------------------------------------------------------------

flag FE_Hazen::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  //pressure drop
  double Rho=FE.MeanRho(pProps);
  //FE.SetArea(PI/4.0*Sqr(Diam));
  double radius = m_PhD.Diam()/2;
  double rhoArea = Rho*PI*Sqr(radius);
  double Vel = FE.SetVelMeasRange(Rho, PI * PI*Sqr(radius), LoLimVel);
  double diam_exprssn = Pow((0.5*radius),0.63);
  double pwr_fac_fnt = 1.852*Res_Powr_Fac;
  double Age_Fac = 1+0.03*Age_Res_Fac;
  double const_num = 9.807*FE.TotLength()*Age_Fac;
  double const_den = 106.08*Res_Magn_Fac*diam_exprssn;
  FE.SetVelocity(FE.GetQm()/rhoArea);
  //FE.SetQmMeas(Vel*rhoArea);
  FE.SetDPq(-FE.QmSign()*const_num*Pow((Vel/const_den),pwr_fac_fnt),
            -FE.QmSign()*pwr_fac_fnt*const_num*Pow(FE.QmMeas(),pwr_fac_fnt-1)/Pow((const_den*rhoArea),pwr_fac_fnt));


  FE.SetDPz(FE.StaticHead(pProps));

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Chamber DP for a pipe
#X:#h<General Description>#nThe Chamber equation is used to calculate the pressure
drop across a pipe due to mass flow.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<AgeRF> : The resistance factor due to the age of the pipe,this is either a
a value from 0.0 to 1.0 or the age of the pipe in years.#n
#i<RMagnF> : The resistance magnitude factor, a value from 0.0 to 1.0.#n
#i<RPowrF> : The resistance power factor, a value from 0.0 to 1.0.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#n#n
#h<Associated variables>#n
#i<Regime> : The type of flow of the fluid within the pipe :#n
#u<Laminar> - A flow in which each particle of the fluid follows a
smooth path and the paths do not cross over one another.#n
#u<Turbulent> - A flow characterised by small whirlpool-like currents or eddies which
absorb a large amount of energy and increase the viscosity (internal friction) of the fluid.#n
#n
#n#h<Other>#n
Short name:FE_Chmbr#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/
IMPLEMENT_FLWEQN(FE_Chmbr, PipeGroup.Name(), "FE_Chmbr", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Chamber DP",
                 "Chamber DP due to flow in a pipe");

FE_Chmbr::FE_Chmbr(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam=0.1; // 100 mm inner diameter
  Res_Magn_Fac = 1.0;
  Res_Powr_Fac = 1.0;
  Age_Res_Fac = 1.0;
  //Len_Rqd=1.0;
  //Len_Fit=0.0;
  FlwRgm=0; // laminar flow
  LoLimQm=0.001;
  }

//--------------------------------------------------------------------------

FE_Chmbr::~FE_Chmbr()
  {
  }

//--------------------------------------------------------------------------

void FE_Chmbr::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("AgeResFactor","AgeRF",  DC_, "",  &Age_Res_Fac,  NULL,  isParm);
  DDB.Double ("ResMagnFactor","RMagnF",DC_, "",  &Res_Magn_Fac, NULL,  isParm);
  DDB.Double ("ResPowrFactor","RPowrF",DC_, "",  &Res_Powr_Fac, NULL,  isParm);
  DDB.Double ("Diameter",     "Diam",  DC_L, "mm",xidFlwEqnDiam,  NULL,  isParmStopped);
  DDB.Double ("Length",       RQDLEN,    DC_L, "m", xidFlwEqnRqdLen,      NULL,  isParmStopped);
  DDB.Double ("FittingLength","Fit_Len", DC_L, "m", xidFlwEqnFitLen,      NULL,  isParmStopped);
  DDB.Double ("CalcLength",   "CalcLen", DC_L, "m", xidFlwEqnActLen,      NULL, isResult);
  static DDBValueLst DDB0[]={
    {0, "Laminar"},
    {1, "Turbulent"},
    {0}};
  DDB.Byte   ("Regime",       "",      DC_,  "",  &FlwRgm,       NULL,  isResult|0, DDB0);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

void FE_Chmbr::SetPhysData(double DZ)
  {
//  CFBPhysData &D=PhDCfg();
  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
  SetRise(DZ);

  //D.SetDiam(Diam);
  };

//--------------------------------------------------------------------------

flag FE_Chmbr::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  //pressure drop
  double Rho=FE.MeanRho(pProps);
  //FE.SetArea(PI/4.0*Sqr(Diam));
  FE.SetQmMeasRange(Rho, LoLimQm);
  double pwr_fac_fnt = 1.92*Res_Powr_Fac;
  double Age_Fac = 1+0.03*Age_Res_Fac;
  double constant = 0.01174*Res_Magn_Fac*FE.TotLength()*Age_Fac*Pow(m_PhD.Diam(),-5.13);

  FE.SetDPq(-FE.QmSign()*constant*Pow(FE.QmMeas()/Rho,pwr_fac_fnt),
            -FE.QmSign()*pwr_fac_fnt*Pow(Rho,-pwr_fac_fnt)*constant*Pow(FE.QmMeas(),(pwr_fac_fnt-1)));

  FE.SetDPz(FE.StaticHead(pProps));

  return True;
  };

//==========================================================================
//
//    Metzner and Reed DP - NOT FOR TURBULENT FLOW   i.e. Re  > 2100
//                              MR
//==========================================================================

/*#D:#T:Metzner & Reed DP (laminar) for a pipe
#X:#h<General Description>#nThis is a flow equation for a pipe.
This calculates the pressure drop within a pipe according to the Metzner and Reed flow model, which is only suitable for laminar flow.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<n'> : Determined by fitting a first order regression equation to the Ln(D##P/4L) vs Ln(8V/D)
(where ## =delta) data in the area of interest.#n
#i<K'> : Determined from the intercept of the regression when Ln(8V/D) = 0. Note these calculations are
only valid if the data is obtained from a capillary rheometer viscometer ???.#n
#i<K_> : This represents the kinetic energy-related pressure drops within a pipe. eg. the exit loss from a pipe.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#n
#n#h<Other>#n
Short name:FE_MR#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/
// To go back once tested
// IMPLEMENT_FLWEQN(FE_MR, PipeGroup.Name()), "FE_MR", "", TOC_DYNAMIC|TOC_GRP_GENERAL|TOC_STD_KENWALT,
//                 "Metzner & Reed DP",
//                 "Metzner & Reed DP only for laminar flow");

//FE_MR::FE_MR(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
//  CFlwEqn(pClass_, pTag, pAttach, eAttach)
//  {
//  np           = 0.5; // ??? whats believable
//  Kp           = 1.0; // ??? whats believable
//  K_           = 1.0; // ??? whats believable
//  //Diam         = 0.1;//100 mm
//  FlwRgm       = 0; // laminar flow
//  //Len_Rqd      = 1.0;//1 m
//  //Len_Fit      = 0.0;// 1 m
//  LoLimVel     = 0.00001; // lowest allowable flowrate
//  }
//
////--------------------------------------------------------------------------
//
//FE_MR::~FE_MR()
//  {
//  }
//
////--------------------------------------------------------------------------
//
//void FE_MR::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  DDB.Double ("n'",            "n'",       DC_Frac, "",   &np,       NULL,  isParm);
//  DDB.Double ("K'",            "K'",       DC_Frac, "",   &Kp,       NULL,  isParm);
//  DDB.Double ("K_",            "K_",       DC_Frac, "",   &K_,       NULL,  isParm);
//  DDB.Double ("Diameter",      "Diam",     DC_L,    "mm", xidFlwEqnDiam,    NULL,  isParmStopped);
//  DDB.Double ("Length",        RQDLEN,     DC_L,    "m",  xidFlwEqnRqdLen,  NULL,  isParmStopped);
//  DDB.Double ("FittingLength", "Fit_Len",  DC_L,    "m",  xidFlwEqnFitLen,  NULL,  isParmStopped);
//  DDB.Double ("CalcLength",    "CalcLen",  DC_L,    "m",  xidFlwEqnActLen,  NULL, isResult);
//  BuildDataDefnOveride(DDB);
//  };
//
////--------------------------------------------------------------------------
//
//void FE_MR::SetPhysData(double DZ)
//  {
//  //CFBPhysData &D=PhDCfg();
//  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
//  SetRise(DZ);
//  //D.SetDiam(Diam);
//  };
//
////--------------------------------------------------------------------------
//
//flag FE_MR::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
//  {
//  #if DumpCrv
//  if (CFlwEqn::DoDumpTest)
//    CFlwEqn::DumpTest("\\mr_value.csv", 0. 250., 5., FE, Regulation, P, Rho, C, IC);
//  #endif
//
//  double Rho=FE.MeanRho(pProps);
//  //FE.SetArea(PI/4.0*Sqr(Diam));
//  const double Tur_ff = 0.004;
//  double Diam = m_PhD.Diam();
//  double radius  = Diam / 2.0;
//  double Area = PI * Sqr(radius);
//  double rhoArea = Rho * Area;
//  double RhoSG = Rho/1000.0;
//  FE.SetVelMeasRange(rhoArea, LoLimVel);
//  FE.SetVelocity(FE.VelMeas());
//  double Re = ( Pow(Diam,np)*Pow(FE.Velocity(),(2.0-np))*RhoSG ) / ( Kp*Pow(8.0,(np-1.0)) );// M&R's Reynold number
//  double wall_loss = 4.0*FE.TotLength()*Kp*Pow(8.0*FE.Velocity()/Diam,np)/Diam;
//  double kinetic_loss = 0.5*RhoSG*K_*Sqr(FE.Velocity());
//
//  FE.SetDPq(-FE.QmSign() * (wall_loss + kinetic_loss),
//            -FE.QmSign() *( (4.0*np*Pow(8.0,np)*Kp*FE.TotLength()*Pow(FE.GetQm(),np-1.0))
//                       /(Pow(Diam,1.0+np)*Pow(rhoArea,np)) + (K_*FE.GetQm())/(RhoSG*Sqr(Area))));
//
//  FE.SetDPz(FE.StaticHead(pProps));
//
//  return True;
//  };
//
//////////////////////////////////////////////////////////////////////////////////////////////



//==========================================================================
//
//   Metzner and Reed DP - valid for both laminar and turbulent flow
//    WITH A CONSTANT turbulent friction factor of 0.004
//
//==========================================================================

/*#D:#T:Metzner & Reed DP (turbulent) for a pipe
#X:#h<General Description>#nThis is an extension of the  Metzner and Reed flow
equation which is used to calculate the pressure drop within a pipe.  This version
of the equation is valid for both laminar and turbulent flow, with a constant turbulent
friction factor of 0.004.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<n'> : Determined by fitting a first order regression equation to the Ln(D##P/4L) vs Ln(8V/D)
(where ## =delta) data in the area of interest.#n
#i<K'> : Determined from the intercept of the regression when Ln(8V/D) = 0. Note these calculations are
only valid if the data is obtained from a capillary rheometer viscometer.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#n
#n#h<Other>#n
Short name:FE_MRCTF#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/
// To go back once tested
// IMPLEMENT_FLWEQN(FE_MRCTF, PipeGroup.Grp(), "FE_MRCTF", TOC_DYNAMIC|TOC_GRP_GENERAL|TOC_STD_KENWALT "",
//                 "M & R, Fturb = 0.004",
//                 "Metzner & Reed DP with a constant turbulent friction factor of 0.004");

//FE_MRCTF::FE_MRCTF(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
//  CFlwEqn(pClass_, pTag, pAttach, eAttach)
//  {
//  np           = 0.5; // ??? whats believable
//  Kp           = 1.0; // ??? whats believable
//  //Diam         = 0.1;//100 mm
//  FlwRgm       = 0; // laminar flow
//  //Len_Rqd      = 1.0;//1 m
//  //Len_Fit      = 0.0;// 1 m
//  LoLimVel     = 0.00001; // lowest allowable flowrate
//  }
//
////--------------------------------------------------------------------------
//
//FE_MRCTF::~FE_MRCTF()
//  {
//  }
//
////--------------------------------------------------------------------------
//
//void FE_MRCTF::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  DDB.Double ("n'",           "n'",        DC_Frac,"",       &np,         NULL,  isParm);
//  DDB.Double ("K'",           "K'",        DC_Frac,"",       &Kp,         NULL,  isParm);
//  DDB.Double ("Diameter",     "Diam",      DC_L,   "mm",    xidFlwEqnDiam, NULL,  isParmStopped);
//  DDB.Double ("Length",       RQDLEN,      DC_L,   "m",     xidFlwEqnRqdLen,     NULL,  isParmStopped);
//  DDB.Double ("FittingLength","Fit_Len",   DC_L,   "m",     xidFlwEqnFitLen,     NULL,  isParmStopped);
//  DDB.Double ("CalcLength",    "CalcLen",  DC_L,    "m",  xidFlwEqnActLen,  NULL, isResult);
//  BuildDataDefnOveride(DDB);
//  };
//
////--------------------------------------------------------------------------
//
//void FE_MRCTF::SetPhysData(double DZ)
//  {
//  //CFBPhysData &D=PhDCfg();
//  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
//  SetRise(DZ);
//  //D.SetDiam(Diam);
////  D.dLength=Max(Max(fabs(DZ), Len_Rqd), 0.1*Diam);
////  D.dXLength=Len_Fit;
////  D.dArea=Sqr(Diam)*PI/4.0;
//  };
//
////--------------------------------------------------------------------------
//
//flag FE_MRCTF::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
//  {
//  #if DumpCrv
//  if (CFlwEqn::DoDumpTest)
//    CFlwEqn::DumpTest("\\mr_value.csv", 0. 250., 5., FE, Regulation, P, Rho, C, IC);
//  #endif
//
//  double Rho = FE.MeanRho(pProps);
//  //FE.SetArea(PI/4.0*Sqr(Diam));
//  const double Tur_ff = 0.004;
//  double Diam = m_PhD.Diam();
//  double radius  = Diam / 2.0;
//  double Area = PI * Sqr(radius);
//  double rhoArea = Rho * Area;
//  double RhoSG = Rho/1000.0;
//  FE.SetQmMeas(1.5);
//  FE.SetVelocity(RangeFlow(FE.QmMeas()/rhoArea, LoLimVel));
//  double Re = ( Pow(Diam,np)*Pow(FE.Velocity(),(2.0-np))*RhoSG ) / ( Kp*Pow(8.0,(np-1.0)) );// M&R's Reynold number
//  double lam_ff = 16.0 / Re;
//  double tot_ff = Pow(Pow(lam_ff,8.0) + Pow(Tur_ff,8.0),0.125);
//  double Constant = (16.0*Kp*Pow(8.0,(np-1.0)))
//             /(Pow(RhoSG,(np-1.0))*Pow(Diam,np)*Pow(Area,(np-2.0)));
//  double dPq   = -FE.QmSign() * 0.5*RhoSG*Sqr(FE.Velocity())*((4.0*tot_ff*FE.TotLength())/Diam + Kp);
// // FE.fDerivsGood = 0;
//  double arb =  8.0*np-15.0;
//  double c2b =  Pow(FE.QmMeas(),(8.0*np-15.0));
//  double dPqdQ = -FE.QmSign() * (Pow(2.0*RhoSG*Sqr(Area),-1.0)*((8.0*np-16.0)*Constant*c2b+(2.0*FE.QmMeas())
//               *(Pow(Constant,8.0)*Pow(FE.QmMeas(),(8.0*np-16.0))+Pow(Tur_ff,8.0))+2.0*Kp*FE.QmMeas()));
//
//  FE.SetDPq(dPq, dPqdQ);
//  return True;
//  };

//////////////////////////////////////////////////////////////////////////////////////////////



//==========================================================================
//
//  Metzner and Reed, Dodge and Metzner DP - valid for both laminar and turbulent flow
//   WITH AN iteratively determined turbulent friction factor
//
//==========================================================================

/*#D:#T:M & R, Dodge and Metzner DP for a pipe
#X:#h<General Description>#nThis is an extension of the  Metzner and Reed flow
equation which is used to calculate the pressure drop within a pipe.  This version
of the equation is valid for both laminar and turbulent flow, with an iteratively
determined turbulent friction factor.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<n'> : Determined by fitting a first order regression equation to the Ln(D##P/4L) vs Ln(8V/D)
(where ## =delta) data in the area of interest.#n
#i<K'> : Determined from the intercept of the regression when Ln(8V/D) = 0. Note these calculations are
only valid if the data is obtained from a capillary rheometer viscometer.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#i<N> : Globals to n' & K', used to determine the turbulent factor.#n
#i<A> : Globals to n' & K', used to determine the turbulent factor.#n
#i<B> : Globals to n' & K', used to determine the turbulent factor.#n
#i<MaxIters> :Since this equation is solved iteratively, this can be used to limit the number
of maximum number of iterations.#n
#n
#n#h<Other>#n
Short name:FE_MRDM#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/
IMPLEMENT_FLWEQN(FE_MRDM, PipeGroup.Name(), "FE_MRDM", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "M & R, Dodge & Metz",
                 "M & R, Dodge & Metzner DP");

double FE_MRDM::N_Const=dNAN;
double FE_MRDM::A_Const=dNAN;
double FE_MRDM::B_Const=dNAN;

FE_MRDM::FE_MRDM(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  np            = 0.5; // ??? whats believable
  Kp           = 1.0; // ??? whats believable
  //Diam         = 0.1;//100 mm
  FlwRgm       = 0; // laminar flow
  //Len_Rqd      = 1.0;//1 m
  //Len_Fit      = 0.0;// 1 m
  LoLimVel     = 0.001; // lowest allowable flowrate
  Tur_ff_H     = 0.004;
  Tur_ff_M     = 0.004;
  Tur_ff_L     = 0.004;
  MaxIters     =0;
  }

//--------------------------------------------------------------------------

FE_MRDM::~FE_MRDM()
  {
  }

//--------------------------------------------------------------------------

void FE_MRDM::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("n'",           "n'",        DC_Frac,"",      &np,          NULL,  isParm);
  DDB.Double ("K'",           "K'",        DC_,     "",     &Kp,          NULL,  isParm);
  DDB.Double ("Diameter",     "Diam",      DC_L,   "mm",    xidFlwEqnDiam, NULL,  isParmStopped);
  DDB.Double ("Length",       RQDLEN,      DC_L,   "m",     xidFlwEqnRqdLen,     NULL,  isParmStopped);
  DDB.Double ("FittingLength","Fit_Len",   DC_L,   "m",     xidFlwEqnFitLen,     NULL,  isParmStopped);
  DDB.Double ("CalcLength",    "CalcLen",  DC_L,    "m",  xidFlwEqnActLen,  NULL, isResult);
  //DDB.Double ("",             "MinVel",    DC_Ldt, "m/s",   &LoLimVel,    NULL,  isParm);
  DDB.Text("Globals to n' & K'");
  DDB.Double ("",             "N",         DC_,    "",      &N_Const,     NULL,  isParm|NAN_OK);
  DDB.Double ("",             "A",         DC_,    "",      &A_Const,     NULL,  isParm|NAN_OK);
  DDB.Double ("",             "B",         DC_,    "",      &B_Const,     NULL,  isParm|NAN_OK);

  DDB.Long   ("",             "MaxIters",  DC_,    "",      &MaxIters,    NULL,  isParm);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

void FE_MRDM::SetPhysData(double DZ)
  {
 // CFBPhysData &D=PhDCfg();
  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
  SetRise(DZ);
  //D.SetDiam(Diam);
//  D.dLength=Max(Max(fabs(DZ), Len_Rqd), 0.1*Diam);
//  D.dXLength=Len_Fit;
//  D.dArea=Sqr(Diam)*PI/4.0;
  };

//--------------------------------------------------------------------------
/**
double  FE_MRDM::DetermineTurbFactor(double Reynolds, double Tur_ff_Guess)
  {
  double f, Tur_ff=Tur_ff_Guess;
  double Tmp1 = 4.0 / Pow(np, 0.75);
  double Tmp2 = 0.4 / Pow(np, 1.2);
  double Tmp3 = (1.0 - np / 2.0);
  for (int Iter = 1000; Iter; )
    {
    f = 1.0/Sqr(Tmp1 * log10(Reynolds * Pow(Tur_ff, Tmp3)) - Tmp2);
    Tur_ff = 0.8*Tur_ff + 0.2*f;
    double a1=fabs(Tur_ff);
    double a2=fabs(f);
    if (fabs(Tur_ff - f)/GTZ(Max(a1,a2))<CFlwEqn::Lnk_Eps_R()*0.1)
      break;
    Iter--;
    if (Iter<20)
      dbgpln("  MRDM %14.6g %14.6g", a1,a2);
    }
  if (Iter==0)
    LogError("FE_MRDM", 0, "Not Converged");
  return Tur_ff;
  }
**/
//--------------------------------------------------------------------------
/**/
double  FE_MRDM::DetermineTurbFactor(double Reynolds, double Tur_ff_Guess)
  {
  double A = 4.0 / Pow(np, 0.75);
  double B = 0.4 / Pow(np, 1.2);
  double C = (1.0 - np / 2.0);
  double L = 0.4342944819033;//log10(Exp);
  double MinFF= Pow(Pow(10.0, B/A)/Reynolds, 1.0/C);
  double Tur_ff=Max(MinFF*1.0001, Tur_ff_Guess);
  for (int Iter = 1000; Iter; )
    {
    double X, Fn;
    for (;;)
      {
      X=A * log10(Reynolds * Pow(Tur_ff, C)) - B;
      Fn=Tur_ff*Sqr(X)-1.0;
      if (Fn>-0.99)
        break;
      Tur_ff*=10.0;
      }
    double dFn=Sqr(X)+2.0*X*A*L*C;
    // Newton Raphson Solution to a root
    double Delta =-Fn/dFn;
    Tur_ff=Max(MinFF*1.0001, Tur_ff+Delta);
//TODO must restore 2 lines below 
    INCOMPLETECODE(); // ,must restore 2 lines below 
//    if ((fabs(Delta)/GTZ(Tur_ff))<CFlwEqn::Lnk_Eps_R()*0.1)
//      break;
    Iter--;
    if (Iter<20)
      dbgpln("  MRDM %14.6g %14.6g %14.6g", Fn,dFn,Delta);
    }
  if (Iter==0)
    LogError("FE_MRDM", 0, "Not Converged");
  return Tur_ff;
  }
/**/
//--------------------------------------------------------------------------

double FE_MRDM::CalcDP(CFBPhysData & PhD, CFlwBlkBase & FE, double Vel, double Rho, double &Tur_ff)
  {
  double Diam = m_PhD.Diam();
  double RhoSG = Rho/1000.0;
  double Re = ( Pow(Diam,np)*Pow(Vel,(2.0-np))*Rho) / (Kp*Pow(8.0,(np-1.0)));// M&R's Reynold number
  double lam_ff = 16.0 / Re;
  Tur_ff = DetermineTurbFactor(Re, Tur_ff);
  double tot_ff = Pow(Pow(lam_ff,8.0) + Pow(Tur_ff,8.0),0.125);

  return 0.5*RhoSG*Sqr(Vel)*((4.0*tot_ff*FE.TotLength())/Diam /*+ K this is fitting stuff*/);
  }

//--------------------------------------------------------------------------

flag FE_MRDM::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  if (Valid(N_Const))
    {
    N_Const=Range(0.01, N_Const, 1.0);
    np=N_Const;
    }
  if (Valid(A_Const) && Valid(B_Const))
    {
    A_Const=Range(0.05, A_Const, 1.0);
    B_Const=Range(0.01, B_Const, 0.2);
//TODO FE_MRDM :: TO RESTORE NEXT LINE
//    Kp=A_Const*exp(B_Const*100.0*Range(0.0, FE.MassFracSL(), 0.5));
    }

  #if DumpCrv
  if (CFlwEqn::DoDumpTest)
    CFlwEqn::DumpTest("\\mr_value.csv", 0. 250., 5., FE, Regulation, P, Rho, C, IC);
  #endif

  double Rho = FE.MeanRho(pProps);
  //FE.SetArea(PI/4.0*Sqr(PhD().Diam());
  double radius  = m_PhD.Diam() / 2.0;
  double Area = PI * Sqr(radius);
  double rhoArea = Rho * Area;
  FE.SetVelMeasRange(Rho, Area, LoLimVel);

  double dVel=FE.VelMeas(1.01)-FE.VelMeas();
  double dPqdQ = -FE.QmSign() * (CalcDP(m_PhD, FE, FE.VelMeas()+dVel, Rho, Tur_ff_H)-
                                 CalcDP(m_PhD, FE, FE.VelMeas()-dVel, Rho, Tur_ff_L))
                 /(2.0*dVel*rhoArea);
  // Calc Slope first to leave the correct values in FE ie Vel etc.
  double dPq = -FE.QmSign() * CalcDP(m_PhD, FE, FE.VelMeas(), Rho, Tur_ff_M);

  FE.SetDPq(dPq, dPqdQ);

  return True;
  };

//////////////////////////////////////////////////////////////////////////////////////////////



//==========================================================================
//
//  Metzner and Reed, Hanks & Ricks DP - valid for both laminar and turbulent flow
//   POSSIBLY NOT SUITABLE for fluids WITH A High Yield Stress
//
//==========================================================================

/*#D:#T:M & R, Hanks & Ricks DP for a pipe
#X:#h<General Description>#nThis flow equation is used to calculate the pressure drop within
a pipe according to the Metzner and Reed, Hanks and Ricks flow model.  It is valid for both
laminar and turbulent flow but POSSIBLY NOT SUITABLE for fluids with a High Yield Stress.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<n'> : Determined by fitting a first order regression equation to the Ln(D##P/4L) vs Ln(8V/D)
(where ## =delta) data in the area of interest.#n
#i<K'> : Determined from the intercept of the regression when Ln(8V/D) = 0. Note these calculations are
only valid if the data is obtained from a capillary rheometer viscometer.#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#n
#n#h<Other>#n
Short name:FE_MRHR#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/
// To go back once tested
// IMPLEMENT_FLWEQN(FE_MRHR, PipeGroup.Grp(), "FE_MRHR", "", TOC_DYNAMIC|TOC_GRP_GENERAL|TOC_STD_KENWALT,
//                 "M & R, Hanks & Ricks",
//                 "M & R, Hanks & Ricks DP");
//
//FE_MRHR::FE_MRHR(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
//  CFlwEqn(pClass_, pTag, pAttach, eAttach)
//  {
//  np           = 0.5; // ??? whats believable
//  Kp           = 1.0; // ??? whats believable
//  //Diam         = 0.1;//100 mm
//  FlwRgm       = 0; // laminar flow
//  //Len_Rqd      = 1.0;//1 m
//  //Len_Fit      = 0.0;// 1 m
//  LoLimVel     = 0.00001; // lowest allowable flowrate
//  }
//
////--------------------------------------------------------------------------
//
//FE_MRHR::~FE_MRHR()
//  {
//  }
//
////--------------------------------------------------------------------------
//
//void FE_MRHR::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  DDB.Double ("n'",           "n'",        DC_Frac,"",       &np,         NULL,  isParm);
//  DDB.Double ("K'",           "K'",        DC_Frac,"",       &Kp,         NULL,  isParm);
//  DDB.Double ("Diameter",     "Diam",      DC_L,   "mm",    xidFlwEqnDiam, NULL,  isParmStopped);
//  DDB.Double ("Length",       RQDLEN,      DC_L,   "m",     xidFlwEqnRqdLen,     NULL,  isParmStopped);
//  DDB.Double ("FittingLength","Fit_Len",   DC_L,   "m",     xidFlwEqnFitLen,     NULL,  isParmStopped);
//  DDB.Double ("CalcLength",    "CalcLen",  DC_L,    "m",  xidFlwEqnActLen,  NULL, isResult);
//  BuildDataDefnOveride(DDB);
//  //DDB.Byte   ("Regime",       "",          DC_,   "",       &FlwRgm,       NULL,  0);
//  //DDB.IndexedStrValue(0, "Laminar");
//  //DDB.IndexedStrValue(1, "Turbulent");
//  };
//
////--------------------------------------------------------------------------
//
//void FE_MRHR::SetPhysData(double DZ)
//  {
//  //CFBPhysData &D=PhDCfg();
//  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
//  SetRise(DZ);
//  //D.SetDiam(Diam);
////  D.dLength=Max(Max(fabs(DZ), Len_Rqd), 0.1*Diam);
////  D.dXLength=Len_Fit;
////  D.dArea=Sqr(Diam)*PI/4.0;
//  };
//
////--------------------------------------------------------------------------
//
//flag FE_MRHR::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
//  {
//  #if DumpCrv
//  if (CFlwEqn::DoDumpTest)
//    CFlwEqn::DumpTest("\\mr_value.csv", 0. 250., 5., FE, Regulation, P, Rho, C, IC);
//  #endif
//
//  double Rho = FE.MeanRho(pProps);
//  //FE.SetArea(PI/4.0*Sqr(Diam));
//  double Diam = m_PhD.Diam();
//  double radius  = Diam / 2.0;
//  double Area = PI * Sqr(radius);
//  double rhoArea = Rho * Area;
//  double RhoSG = Rho/1000.0;
//  FE.SetVelocity(RangeFlow(FE.QmMeas()/rhoArea, LoLimVel));
//  double Re = ( Pow(Diam,np)*Pow(FE.Velocity(),(2.0-np))*RhoSG ) / ( Kp*Pow(8.0,(np-1.0)) );// M&R's Reynold number
//  double delta = 1.0/(1.0+4.0*(Re-(2100.0+875.0*(1.0-np))));
//  double lam_ff = 16.0 / Re;
//  double Tur_ff = (0.0682*Pow(np,-0.5)) / Pow(Re,(1.0 / (1.87+2.39*np)) );
//  double tra_ff = 0.000179*exp(-5.24*np)*Pow(Re,(0.414+0.757*np));
//  double tot_ff = lam_ff*(1.0 - delta) + delta / Pow(Pow(Tur_ff,-8.0)+Pow(tra_ff,-8.0),0.125);
//  FE.SetDPq(-FE.QmSign() * 0.5*RhoSG*Sqr(FE.Velocity())*((4.0*tot_ff*FE.TotLength())/Diam + Kp), 0.0);
//  FE.SetDerivsBad();
////  FE.dPqdQ = -FE.QmSign() * (FE.Qm/(Rho*Sqr(Area)))*((4.0*tot_ff*FE.TotLength())/Diam + Kp);
//  return True;
//  };
//
//==========================================================================
//
//
//
//==========================================================================
//==========================================================================
/*#D:#T:Compressed Air DP for a pipe
#X:#h<General Description>#nThis is a method of calculating the pressure drop
versus flow relationship for a pipe. The Compressed Air equation which is used
for calculating the pressure drop within a pipe is given in the following form:
#ndP = Rf #* M^2 #* L #* 1e-3/w
#nwhere
#nRf = Resistance factor, calculated from: 1e13.832/D^5.276
#nD  = Pipe diameter in mm.
#nM  = Mass flow of Air (kg/s)
#nL  = Pipe length - m
#nw  = Density of Air, calculated from the following equation:
#nw  = P #* 1e3/R #* T
#nP  = Entry Pressure - kPa
#nR  = Universal constant, 287.0
#nT  = Air Temperature - Kelvin

Ref: J.Burrows etal, Enviromental Engineering in South African Mines, Cape Town,
CTP Book Printers, 1989.

#n#n
#n#h<Variables to be supplied by the user>#n
#i<Diam> : The inner pipe diameter.#n
#i<Act_Len> : The actual pipe length that is required.#n
#i<Fit_Len> : The additional length of pipe, that is equivalent to the resistance caused
by any bends or other modifications to the originally straight pipe.#n
#n
#n#h<Other>#n
Short name:FE_CompAir#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/

IMPLEMENT_FLWEQN(FE_CompAir, PipeGroup.Name(), "FE_CompAir", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "Compressed Air DP",
                 "DP due to compressed air flow in a pipe");

FE_CompAir::FE_CompAir(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam         = 0.1;//100 mm
  //Len_Rqd      = 1.0;//1 m
  //Len_Fit      = 0.0;// 1 m
  LoLimVel     = 0.001;
  EstRf=1.0;
  ManRf=dNAN;
  }

//--------------------------------------------------------------------------

FE_CompAir::~FE_CompAir()
  {
  }

//--------------------------------------------------------------------------

void FE_CompAir::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Diameter",        "Diam",      DC_L,  "mm",       xidFlwEqnDiam,  NULL,  isParmStopped);
  DDB.Double ("Length",          RQDLEN,      DC_L,  "m",        xidFlwEqnRqdLen,NULL,  isParmStopped);
  DDB.Double ("FittingLength",   "Fit_Len",   DC_L,  "m",        xidFlwEqnFitLen,NULL,  isParmStopped);
  DDB.Double ("CalcLength",    "CalcLen",  DC_L,    "m",  xidFlwEqnActLen,  NULL, isResult);
  DDB.Double ("EstRf",           "",          DC_,   "",         &EstRf,         NULL,  isParmStopped);
  DDB.Double ("ManRf",           "",          DC_,   "",         &ManRf,         NULL,  isParmStopped|NAN_OK);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag FE_CompAir::ValidateData(ValidateDataBlk & VDB)
  {
//  if (!PhDOK())
//    return true;

  SetDiam(Max(Diam(), 0.001));
  //double Diam = Diam();
  double mmDiam = Diam() * 1000.0;
  double PrevEstRf=EstRf;
  EstRf       = Pow(10.0,13.832)/Pow(mmDiam,5.276);
  if (Valid(ManRf))
    ManRf*=EstRf/GTZ(PrevEstRf);
  return True;
  }

//--------------------------------------------------------------------------

void FE_CompAir::SetPhysData(double DZ)
  {
 // CFBPhysData &D=PhDCfg();
  SetActLength(Max(Max(fabs(DZ), RqdLength()), 0.1*Diam()));
  SetRise(DZ);
  //D.SetDiam(Diam);
//#if WithOEP
//  if (OEPOptions())
//    D.dFitLen=Len_Fit*dFitLenScl;
//#endif
  };

//--------------------------------------------------------------------------

//void FE_CompAir::StepFlowInfo(CFlwBlkBase &FE)
//  {
//  };

//--------------------------------------------------------------------------

flag FE_CompAir::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Diam = m_PhD.Diam();
  double wRho, R, Pin, rhoArea, TTemp;

  R       = 287.0;
  Pin     = (FE.QmSign()>0) ? FE.FlngPress() : FE.NodePress();
  TTemp   = FE.MeanTemp(pProps);
  wRho    = Pin * 1000.0/(R * TTemp);
  rhoArea = wRho * PI * Sqr(Diam / 2.0);

  FE.SetArea(PI/4.0*Sqr(Diam));
  double Vel = FE.SetVelMeasRange(wRho, PI * Sqr(Diam / 2.0), LoLimVel);
  FE.SetVelocity(FE.GetQm()/rhoArea);

  double Rf=(Valid(ManRf)?ManRf:EstRf);
  double DeltaP = Rf*Sqr(FE.QmMeas())*FE.TotLength()*1e-3/wRho;

  FE.SetDPq(-FE.QmSign()*Rf*Sqr(FE.QmMeas())*FE.TotLength()*1e-3/wRho,
            -FE.QmSign()*2.0*Rf*FE.QmMeas()*FE.TotLength()*1e-3/wRho);
  FE.SetDPz(FE.StaticHead(pProps), 0);

    //if (DoDbgBrk())
    //  {
    //  dbgpln("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f",
    //         P1,Vf,Lf,RhoL,RhoV,m_dDensMeas,m_dViscMeas,rhoArea,Qm1,dQm,Qm2,Vel1,Vel2,dPq1,(dPq2 - dPq1)/dQm);
    //  //dbgpln("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f",
    //  //       P1,Vf,Lf,RhoL,RhoV,m_dDensMeas,m_dViscMeas,rhoArea,Qm1,QlFrac,QvFrac,dQm,Qm2,Vel1,Vel2);
    //  }
  return True;
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(FE_MSBackFill, PipeGroup.Name(), "FE_MS_BackFill", "", TOC_DYNAMICFULL|TOC_GRP_GENERAL|TOC_MINESERVE,
                  "MS BackFill",
                  " MineServe Back Fill Pressure drop relationship");

FE_MSBackFill::FE_MSBackFill(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) : \
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_PrevDiam  = -1;
  m_LoLimVel  = 0.001;
  m_DP        = 0;
  
  }

//--------------------------------------------------------------------------

FE_MSBackFill::~FE_MSBackFill()
  {
  }

//--------------------------------------------------------------------------

void FE_MSBackFill::BuildDataDefn(DataDefnBlk & DDB)
  {
  //DDB.Double ("Resistance",   "Resistn",  DC_,     "",      &R_Fxd,          NULL,  isParm);
  DDB.Double ("Diameter",        "Diam",      DC_L,    "mm",     xidFlwEqnDiam,       NULL, isParmStopped);
  DDB.Double ("Length",          RQDLEN,      DC_L,    "m",      xidFlwEqnRqdLen,     NULL, isParmStopped);
  DDB.Double ("FittingLength",   "Fit_Len",   DC_L,    "m",      xidFlwEqnFitLen,     NULL, isParmStopped);
  DDB.Double ("CalcLength",      "CalcLen",   DC_L,    "m",      xidFlwEqnActLen,     NULL, isResult);

  DDB.Double ("Density",         "",          DC_Rho,  "kg/m^3", &m_dDensMeas,        NULL, isResult);
  DDB.Double ("Viscosity",       "",          DC_Visc, "cP",     &m_dViscMeas,        NULL, isResult);
  DDB.Double ("Velocity",        "Vel",       DC_Ldt,  "m/s",    &m_dVelMeas,         NULL, isResult);

  //DDB.Double ("Length",       "",         DC_L,    "m",     xidFlwEqnRqdLen, NULL,  isParmStopped);
  //DDB.Double ("Diam",         "",         DC_L,    "mm",    xidFlwEqnDiam,   NULL,  isParm);
  //DDB.Double ("TempRise",     "",         DC_dT,   "C",     &dTRiseRqd,      NULL,  isParm|NAN_OK);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag FE_MSBackFill::ValidateData(ValidateDataBlk & VDB)
  {
  return CFlwEqn::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void FE_MSBackFill::SetPhysData(double DZ)
  {
//  CFBPhysData &D=PhDCfg();
  SetActLength(Max(fabs(DZ), RqdLength()));
  SetRise(DZ);
//  D.SetDiam(Diam);
  };

//--------------------------------------------------------------------------

const int NPts=7;
const int NCrvs=5;
// array 
struct CDataPt { double m_Vel, m_DP; };

static struct { double m_Diam; CDataPt m_Data[NPts]; } s_Curves[NCrvs] =
  {
  // Diam   V,DP     V,DP     V,DP     V,DP     V,DP     V,DP     V,DP     V,DP      V,DP       V,DP
    {0.042, {{2,2.00}, {3,4.00}, {4,5.90}, {5,8.40}, {6,10.8}, {7,14.0}, {8,17.0}}},
    {0.058, {{2,1.00}, {3,2.43}, {4,4.15}, {5,6.20}, {6, 8.2}, {7,11.0}, {8,13.7}}},
    {0.074, {{2,0.9},  {3,1.95}, {4,3.30}, {5,5.00}, {6, 6.6}, {7, 8.8}, {8,11.0}}},
    {0.100, {{2,0.85}, {3,1.90}, {4,3.10}, {5,4.40}, {6, 5.9}, {7, 7.3}, {8, 9.8}}},
    {0.154, {{2,0.81}, {3,1.80}, {4,3.00}, {5,4.00}, {6, 5.6}, {7, 7.1}, {8, 8.8}}},
  };

//--------------------------------------------------------------------------

flag FE_MSBackFill::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  if (Diam()!=m_PrevDiam)
    {
    m_PrevDiam=Diam();
    m_Spln.Clear();
    dbgpln("===================");

    for (int p=0; p<NPts; p++)
      {
      TSpline       Spln;

      //double Vel=s_Curves[0].m_Data[p].m_Vel;
      double Vel=s_Curves[0].m_Data[p].m_Vel;
      for (int c=0; c<NCrvs; c++)
        {
        ASSERT_ALWAYS(Vel==s_Curves[c].m_Data[p].m_Vel, "BackFill Bad Curves");
        Spln.AddPt(s_Curves[c].m_Diam, s_Curves[c].m_Data[p].m_DP);  // Add Points ...
        }
      //Spln.SetSigma(1.0);  // optional - tension approx 1e-10 slack approx 1e10 tight (almost straight lines)

      //  Type=TSpline::iSlopeFree;
      //  Type=TSpline::iSlopeOfSegment;
      //  Type=TSpline::iSlopeSpecd;

      //Spln.SetSlopeLo(TSpline::iSlopeSpecd, 0.0); // optional slope at 'beginning' (tends to this)
      Spln.SetSlopeLo(TSpline::iSlopeFree, 0.0); // optional slope at first point
      Spln.SetSlope1(TSpline::iSlopeFree, 0.0); // optional slope at first point
      Spln.SetSlopeN(TSpline::iSlopeFree, 0.0); // optional slope at last point
      Spln.SetSlopeHi(TSpline::iSlopeFree, 0.0); // optional slope at last point
      //Spln.SetSlopeHi(TSpline::iSlopeSpecd, 1.0);// optional slope at 'end' (tends to this)

      Spln.Fit();

      double DP=Spln.Yx(Diam());
      m_Spln.AddPt(Vel, DP);
      dbgpln("%10.4f %10.4f %10.4f", Diam(), Vel, DP); 
      }
    
    m_Spln.Fit();
    dbgpln("===================");
    }

  double Area = PI * Sqr(Diam()/2.0);
  double P1   = Max(1.0, ((FE.QmSign()>0) ? FE.FlngPress() : FE.NodePress()));
  m_dDensMeas = FE.MeanRho(pProps, P1);
  m_dViscMeas = FE.MeanViscosity(pProps);
  FE.SetVelMeasRange(m_dDensMeas, Area, m_LoLimVel);

  m_dVelMeas  = FE.VelMeas();
  double DPL  = m_Spln.Yx(m_dVelMeas);
  dbgpln("%10.4f %10.4f %10.4f", Diam(), m_dVelMeas, DPL); 

  m_DP        = FE.TotLength()*DPL;
  FE.SetDPq(-FE.QmSign()*m_DP, 0);
  FE.SetDerivsBad();

  FE.SetMomentum(pProps, FE.MeanFBPress());
  FE.SetKFact(0);

  FE.SetDPz(FE.StaticHead(pProps));

  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
