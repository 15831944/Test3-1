//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "ndtree.h"
#include "flwnode.h"
#define  __MG_TWOPH_CPP
#include "mg_twoph.h"
#include "net_err.h"
#include "mathlib.h"

#include "dbgmngr.h"
//#include "optoff.h"

#define dbgPipeline                   01

#if dbgPipeline
static CDbgMngr dbgAddSegs      ("MG", "AddSegs");
static CDbgMngr dbgVelocities   ("MG", "Velocities");
static CDbgMngr dbgCalcPress    ("MG", "CalcPress");
static CDbgMngr dbgEstDP        ("MG", "EstDP");
static CDbgMngr dbgTempDerivs   ("MG", "TempDerivs");
static CDbgMngr dbgPosition     ("MG", "Position");
static CDbgMngr dbgTPPDump      ("MG", "TPPDump");
#endif


#define PressCalc1    01

const int IgnoreStepStart=1;

//#pragma message("NB NB NB optimise off -------------------------------------------------------")
//#pragma optimize("", off)

//==========================================================================
//
//
//
//==========================================================================

TPPGasProps::TPPGasProps()
  {
  dGasDens=DefGasDens;
  dNormGasDens=DefNormGasDens;
  dGasCp=2.0;
  dGasSum=0.0;
  }

//--------------------------------------------------------------------------

void TPPGasProps::InitSum(double Gas)
  {
  dGasSum=Gas;
  if (dGasSum<=1.0e-6)
    {
    dGasDens=DefGasDens;
    dNormGasDens=DefNormGasDens;
    dGasCp=2.0;
    }
  }

//--------------------------------------------------------------------------

void TPPGasProps::Sum(TPPGasProps & Other, double OtherGMass)
  {
  double TotGMass=dGasSum+OtherGMass;
  if (TotGMass>1.0e-6)
    {
    dGasCp=(dGasSum*dGasCp+OtherGMass*Other.dGasCp)/TotGMass;
    dNormGasDens=(dGasSum*dNormGasDens+OtherGMass*Other.dNormGasDens)/TotGMass;
    dGasDens=(dGasSum*dGasDens+OtherGMass*Other.dGasDens)/TotGMass;
    }
  dGasSum=TotGMass;
  }

//--------------------------------------------------------------------------

TPPLiqProps::TPPLiqProps()
  {
  dLiqDens=DefLiqDens;
  dLiqCp=3.0;
  dLiqSum=0.0;
  }

//--------------------------------------------------------------------------

void TPPLiqProps::InitSum(double Liq)
  {
  dLiqSum=Liq;
  if (dLiqSum<=1.0e-6)
    {
    dLiqDens=DefLiqDens;
    dLiqCp=3.0;
    }
  }

//--------------------------------------------------------------------------

void TPPLiqProps::Sum(TPPLiqProps & Other, double OtherLMass)
  {
  double TotLMass=dLiqSum+OtherLMass;
  if (TotLMass>1.0e-6)
    {
    dLiqCp=(dLiqSum*dLiqCp+OtherLMass*Other.dLiqCp)/TotLMass;
    dLiqDens=(dLiqSum*dLiqDens+OtherLMass*Other.dLiqDens)/TotLMass;
    }
  dLiqSum=TotLMass;
  }

//--------------------------------------------------------------------------

TPPProps::TPPProps()
  {
  }

//--------------------------------------------------------------------------

void TPPProps::InitSum(double Gas, double Liq)
  {
  TPPGasProps::InitSum(Gas);
  TPPLiqProps::InitSum(Liq);
  }

//--------------------------------------------------------------------------

void TPPProps::Sum(TPPProps & Other, double OtherGMass, double OtherLMass)
  {
  TPPGasProps::Sum(Other, OtherGMass);
  TPPLiqProps::Sum(Other, OtherLMass);
  }

//--------------------------------------------------------------------------

TPPProps & TPPProps::operator=(TPPGasProps & Other)
  {
  dGasDens=Other.dGasDens;
  dNormGasDens=Other.dNormGasDens;
  dGasCp=Other.dGasCp;
  dGasSum=Other.dGasSum;
  return *this;
  };

//--------------------------------------------------------------------------

TPPProps & TPPProps::operator=(TPPLiqProps & Other)
  {
  dLiqDens=Other.dLiqDens;
  dLiqCp=Other.dLiqCp;
  dLiqSum=Other.dLiqSum;
  return *this;
  };

//==========================================================================
//
//
//
//==========================================================================

TPPSection::TPPSection()
  {
  iMyIndex=0;

  dRelDatum=0.0;

  nSlugs=0;
  dTotLiqHoldup=0;

  dStart=0.0;
  dEnd=0.0;
  dAngle=0.0;
  }

TPPSection::~TPPSection()
  {
  }

//==========================================================================
//
//
//
//==========================================================================

TPPJoin::TPPJoin()
  {
  dMeasP=Std_P;
  dMeasT=Std_T;
  dMeasGasVel=0.0;
  dMeasLiqVel=0.0;
  bMeasHorFlowRegime=0;
  bMeasFlowRegime=0;
  }

//--------------------------------------------------------------------------

TPPJoin::TPPJoin(TPPJoin & Other)
  {
  dMeasP   = Other.dMeasP;
  dMeasT   = Other.dMeasT;
  dMeasGasVel=Other.dMeasGasVel;
  dMeasLiqVel=Other.dMeasLiqVel;
  bMeasHorFlowRegime=Other.bMeasHorFlowRegime;
  bMeasFlowRegime   =Other.bMeasFlowRegime;
  };

//--------------------------------------------------------------------------

TPPJoin & TPPJoin::operator=(TPPJoin & Other)
  {
  dMeasP   = Other.dMeasP;
  dMeasT   = Other.dMeasT;
  dMeasGasVel=Other.dMeasGasVel;
  dMeasLiqVel=Other.dMeasLiqVel;
  bMeasHorFlowRegime=Other.bMeasHorFlowRegime;
  bMeasFlowRegime   =Other.bMeasFlowRegime;
  return *this;
  };

//--------------------------------------------------------------------------

TPPJoin::~TPPJoin()
  {
  };

//==========================================================================
//
//
//
//==========================================================================

TPPFlowStuff::TPPFlowStuff()
  {

  bHorFlowRegime=0;    // DP Calc FLow regime
  bFlowRegime=0;       // Flow Regime
//  dQmRng=0;            // Ranged Flow due to DP
  dQmEst=0;            // Calc Flow due to DP
  dQmG=0;              // Calc Flow due to DP
  dQmL=0;              // Calc Flow due to DP
  //dQv=0;               // Volume flow
  dQvG=0;              // Gas Vol Flow due to DP
  dQvL=0;              // Liq Vol FLow due to DP
  dVapFrac=1;          // Mean vapour fraction
  sLiqVolFrac=0;       // Slip Liq Volume Fraction
  dDarcyCorrection=0;

  dSupGasVel=0;        // Superficial Gas Velocity
  dSupLiqVel=0;        // Superficial Liquid Velocity
  dTotalVel=0;         // Total Velocity

  dDPq=0;
  dDPqDq=0;
  dDPmB=0;
  dDPmQ=0;
  dDPmDq=0;
  dDPApplied=0;
  dDPz=0;
  dVelStart=0;
  dGasVelo=0;
  dLiqVelo=0;
  }

//--------------------------------------------------------------------------

TPPFlowStuff::TPPFlowStuff(double Position, double Velocity, double Mass, double Temperature, TPPProps & Properties)
  {

  bHorFlowRegime=0;    // DP Calc FLow regime
  bFlowRegime=0;       // Flow Regime
//  dQmRng=0;            // Ranged Flow due to DP
  dQmEst=0;            // Calc Flow due to DP
  dQmG=0;              // Calc Flow due to DP
  dQmL=0;              // Calc Flow due to DP
  //dQv=0;               // Volume flow
  dQvG=0;              // Gas Vol Flow due to DP
  dQvL=0;              // Liq Vol FLow due to DP
  dVapFrac=1;          // Mean vapour fraction
  sLiqVolFrac=0;       // Slip Liq Volume Fraction
  dDarcyCorrection=0;

  dSupGasVel=0;        // Superficial Gas Velocity
  dSupLiqVel=0;        // Superficial Liquid Velocity
  dTotalVel=0;         // Total Velocity

  dDPq=0;
  dDPqDq=0;
  dDPmB=0;
  dDPmQ=0;
  dDPmDq=0;
  dDPApplied=0;
  dDPz=0;
  dVelStart=0;
  dGasVelo=0;
  dLiqVelo=0;
  }

//--------------------------------------------------------------------------

TPPFlowStuff::TPPFlowStuff(TPPFlowStuff & Other)
  {
  bHorFlowRegime    = Other.bHorFlowRegime;
  bFlowRegime       = Other.bFlowRegime;
//  dQmRng            = Other.dQmRng;
  dQmEst            = Other.dQmEst;
  dQmG              = Other.dQmG;
  dQmL              = Other.dQmL;
  dQvG              = Other.dQvG;
  dQvL              = Other.dQvL;
  dVapFrac          = Other.dVapFrac;
  sLiqVolFrac       = Other.sLiqVolFrac;
  dDarcyCorrection  = Other.dDarcyCorrection;
  dSupGasVel        = Other.dSupGasVel;
  dSupLiqVel        = Other.dSupLiqVel;
  dTotalVel         = Other.dTotalVel;
  dDPq              = Other.dDPq;
  dDPqDq            = Other.dDPqDq;
  dDPmB             = Other.dDPmB;
  dDPmQ             = Other.dDPmQ;
  dDPmDq            = Other.dDPmDq;
  dDPApplied        = Other.dDPApplied;
  dDPz              = Other.dDPz;
  dVelStart         = Other.dVelStart;
  dGasVelo          = Other.dGasVelo;
  dLiqVelo          = Other.dLiqVelo;
  }

//--------------------------------------------------------------------------

TPPFlowStuff & TPPFlowStuff::operator=(TPPFlowStuff & Other)
  {

  bHorFlowRegime    = Other.bHorFlowRegime;
  bFlowRegime       = Other.bFlowRegime;
//  dQmRng            = Other.dQmRng;
  dQmEst            = Other.dQmEst;
  dQmG              = Other.dQmG;
  dQmL              = Other.dQmL;
  dQvG              = Other.dQvG;
  dQvL              = Other.dQvL;
  dVapFrac          = Other.dVapFrac;
  sLiqVolFrac       = Other.sLiqVolFrac;
  dDarcyCorrection  = Other.dDarcyCorrection;
  dSupGasVel        = Other.dSupGasVel;
  dSupLiqVel        = Other.dSupLiqVel;
  dTotalVel         = Other.dTotalVel;
  dDPq              = Other.dDPq;
  dDPqDq            = Other.dDPqDq;
  dDPmB             = Other.dDPmB;
  dDPmQ             = Other.dDPmQ;
  dDPmDq            = Other.dDPmDq;
  dDPApplied        = Other.dDPApplied;
  dDPz              = Other.dDPz;
  dVelStart         = Other.dVelStart;
  dGasVelo          = Other.dGasVelo;
  dLiqVelo          = Other.dLiqVelo;
  return *this;
  }

//--------------------------------------------------------------------------

TPPFlowStuff::~TPPFlowStuff()
  {
  }

double TPPFlowStuff::sHorLiqFlowFrac(double a, double b, double c, double nVolFrac, double Nfr)
// returns the liquid holdup in a horizontal pipe assuming slip between the liquid and gas.  Used in the Beggs and Brill Correlation
  {
  if (Nfr == 0)
    return 0;
  else
    return a*pow(nVolFrac, b)/pow(Nfr, c);

  }

//--------------------------------------------------------------------------

double TPPFlowStuff::sElevFactor(double e, double f, double g, double h, double nVolFrac, double Nlv, double Nfr, double angle)
  // returns the factor to correct the liquid holdup in an elevated pipe, -90 < angle < 90 degrees
  {
  if ((angle == 0) || (nVolFrac == 0))
    return 1;
  else
    {
    double ElevFact = 1+(1-nVolFrac)*log(h*pow(nVolFrac, e)*pow(Nlv, f)*pow(Nfr, g))*(sin(1.8*angle)-0.333*pow(sin(1.8*angle), 3));
    if (ElevFact >= 1)
      return ElevFact;
    else
      return 1;
    }

  }

//--------------------------------------------------------------------------

double TPPFlowStuff::DarcyDP(double Vel, double Rho, double Length, double Diam, double Rough, double Viscosity)
  {

  if ((Vel == 0) || (Rho == 0 ) || (Diam == 0) || (Viscosity == 0))
    return 0;
  else
    {
    double Re, SG, pwrFac;
    Re      = (Diam*Vel*Rho)/Viscosity;        //Reynold's number
    SG      = Rho / 1000.0;
    pwrFac  = 2.0;

    // Churchill equation Ref: Churchill, S.W., Friction factor equation spans all flow regimes,
    // Chem. Eng., Nov. 7, 1977.
    double A, B, F_Fac, const_num;
    A         = Pow((-2.457 * log(Pow(7.0/Re,0.9) + 0.27 * Rough/Diam)), 16);
    B         = Pow((37530.0/Re), 16);
    F_Fac     = 8 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833);
    const_num = 2.0 * F_Fac * SG * Length * Pow(Vel,pwrFac) / Diam;

    return const_num;
    }
  }
//--------------------------------------------------------------------------

void TPPFlowStuff::BeggsAndBrill(TwoPhasePipe & Pipe,
                                 TPPProps &Props,
                                 double Len,
                                 double Angle,
                                 double Q,
                                 double &DPq,
                                 double &DPz,
                                 flag StartOfStep)
  {
  // Beggs and Brill correlation

  double Viscosity, LSurfTension; //Physical values need to be calculated (short term fix here) - AKS
  double nDensity, sDensity;
  //double dP_friction, dP_elevation;
  double Nfr, Nlv;
  double Vm, nLiqVolFrac, sLiqVolFrac_0, ElevAngle, L1, L2, L3, L4;
  //bHorFlowRegime;

  LSurfTension = 15e-3; // Hardwired for now, from example, Gas Processors Association

  if (0 && (fabs(dQvL) < 1e-5) && (fabs(dQvG) < 1e-4))
    {
    DPq=0;
    DPz=0;
    }
  else
    {
    Vm = fabs(dQvL + dQvG) / Pipe.Area();

    ElevAngle = Angle;

    // Calculate the Froude Number Nfr = Sqr(Vm) / gd

    if (Pipe.dDiam == 0)
      Nfr = 0; // Error message ??? AKS
    else
      Nfr        = Sqr(Vm)/(9.81 * Pipe.dDiam);
    if (LSurfTension == 0)
      Nlv = 0;
    else
      Nlv        = dSupLiqVel * pow((Props.dLiqDens/(9.81*LSurfTension)),0.25);
    nLiqVolFrac = dQvL / GTZ(dQvL + dQvG);
    // no slip density
    nDensity = Props.dLiqDens*nLiqVolFrac+(1-nLiqVolFrac)*Props.dGasDens;

    Viscosity = nLiqVolFrac*20e-3+(1-nLiqVolFrac)*0.015e-3; //   Hardwired, Pa.s for now
    //   From example, Gas processors association

    double y_L;
    if (nLiqVolFrac <= 1e-6)
      {
      sDensity=nDensity;
      y_L = 1.0; // gives modifying factor of 1 for dP_friction
      bHorFlowRegime = HFR_Segregated;
      bFlowRegime = TPR_Stratified;
      }
    else
      {
      L1 = 316.0 * pow(nLiqVolFrac, 0.302);
      L2 = 0.0009252 * pow(nLiqVolFrac, -2.4684);
      L3 = 0.10 * pow(nLiqVolFrac, -1.4516);
      L4 = 0.5 * pow(nLiqVolFrac, -6.738);

      // Find flow regime in horizontal pipe
      if (((nLiqVolFrac <= 0.01) && (Nfr <= L1)) || ((nLiqVolFrac > 0.01) && (Nfr <= L2)))
        {
        bHorFlowRegime = HFR_Segregated;  // Sectregated flow
        bFlowRegime = TPR_Stratified;
        }
      else if ((nLiqVolFrac > 0.01) && (Nfr > L2) && (Nfr <= L3))
        {
        bHorFlowRegime = HFR_Transition;  // Transition flow
        bFlowRegime = TPR_Transition;
        }
      else if (((nLiqVolFrac > 0.01) && (nLiqVolFrac <= 0.4) && (Nfr > L3) && (Nfr <= L1)) ||
         ((nLiqVolFrac > 0.4)  && ((Nfr > L3) && (Nfr <= L4))))
        {
        bHorFlowRegime = HFR_Intermittent;  // Intermittent flow - slug or plug flow
        bFlowRegime = TPR_Slug;
        }
      else if (((nLiqVolFrac <= 0.4) && (Nfr > L1)) || ((nLiqVolFrac > 0.4)  && (Nfr > L4)))
        {
        bHorFlowRegime = HFR_Distributed;  // Distributed flow
        bFlowRegime = TPR_Bubble;
        }
      else
        {
        bHorFlowRegime = HFR_Segregated;  // Catch All - Sectregated flow
        bFlowRegime = TPR_Stratified;
        }
      //calculate liquid holdup
      switch (bHorFlowRegime)
        {
        case HFR_Segregated:
          {
          sLiqVolFrac_0 = sHorLiqFlowFrac(0.98, 0.4846, 0.0868, nLiqVolFrac, Nfr);
          if (ElevAngle >= 0)
            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-3.768,3.539,-1.614,0.011,nLiqVolFrac, Nlv, Nfr, ElevAngle);
          else
            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
          break;
          }
        case HFR_Transition:
          {
          double sLiqVolFrac_1;
          double AFactor = (L3-Nfr)/(L3-L2);
          sLiqVolFrac_0  = sHorLiqFlowFrac(0.98, 0.4846, 0.0868, nLiqVolFrac, Nfr); //Sectregated flow
          if (ElevAngle >= 0 )
            sLiqVolFrac_1  = sLiqVolFrac_0*sElevFactor(-3.768,3.539,-1.614,0.011,nLiqVolFrac, Nlv, Nfr, ElevAngle); // Sectregated flow
          else
            sLiqVolFrac_1 = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
          sLiqVolFrac    = AFactor*sLiqVolFrac_1;
          sLiqVolFrac_0  = sHorLiqFlowFrac(0.845, 0.5351, 0.0173, nLiqVolFrac, Nfr); // Intermittent flow
          if (ElevAngle >= 0)
            sLiqVolFrac_1  = sLiqVolFrac_0*sElevFactor(0.305,-0.4473,0.0978,2.96,nLiqVolFrac, Nlv, Nfr, ElevAngle); // Intermittent flow
          else
            sLiqVolFrac_1 = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
          sLiqVolFrac    = sLiqVolFrac + (1-AFactor)*sLiqVolFrac_1;
          break;
          }
        case HFR_Intermittent:
          {
          sLiqVolFrac_0  = sHorLiqFlowFrac(0.845, 0.5351, 0.0173, nLiqVolFrac, Nfr); // Intermittent flow
          if (ElevAngle >= 0)
            sLiqVolFrac  = sLiqVolFrac_0*sElevFactor(0.305,-0.4473,0.0978,2.96,nLiqVolFrac, Nlv, Nfr, ElevAngle); // Intermittent flow
          else
            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
          break;
          }
        case HFR_Distributed:
          {
          sLiqVolFrac_0 = sHorLiqFlowFrac(1.065,0.6824,0.0609, nLiqVolFrac, Nfr);
          if (ElevAngle >= 0)
            sLiqVolFrac = sLiqVolFrac_0;
          else
            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
          break;
          }
        }
      if (ElevAngle > 0)
        sLiqVolFrac = sLiqVolFrac*0.924;
      else if (ElevAngle < 0)
        sLiqVolFrac = sLiqVolFrac*0.685; //Payne et al correction factor

      // need to create a FE_Darcy object - do not know how to do?? Instead hav coppied code

      sDensity = Props.dLiqDens*sLiqVolFrac+(1-sLiqVolFrac)*Props.dGasDens; //Slip density (gasses slipping past liquid)
      y_L = nLiqVolFrac/Sqr(sLiqVolFrac);
      }

    //Calculate the pressure drop due to friction in the pipe
    DPq = DarcyDP(dTotalVel, nDensity, Len, Pipe.dDiam, Pipe.dRough, Viscosity);//Darcy

    // TOGO BACK maybe !
    //modify darcy for 2 phase flow
    //double S;
    //if ((y_L > 1.0) && (y_L <= 1.200517))
    //  {
    //  double S2=log(1.200517)/(-0.0523+3.182*log(1.200517)-0.8725*Sqr(log(1.200517))+0.01853*pow(log(1.200517), 4));
    //  double S1=0.0;
    //  S=(S2-S1)/(1.200517-1.0)*(y_L-1.0)+S1;//Fitted straight line for discontinuity area
    //                      //given function very close to a straight line!
    //  }
    //else
    //  S=log(y_L)/(-0.0523+3.182*log(y_L)-0.8725*Sqr(log(y_L))+0.01853*pow(log(y_L), 4));
    //dDarcyCorrection=exp(S);
    dDarcyCorrection = 0.5; //fudge to make system stable.
    DPq = -Sign(Q)*dDarcyCorrection*DPq;
    // Slug Noise;
    //DPq *= 1.0+Pipe.dSlugResInc*dTotLiqHoldup/Pipe.SectionVolume();

    //Calculate change in pressure due to change in height
    DPz = 9.81*sDensity*Len*sin(ElevAngle)/1000; //modified for 2 phase flow by using slip density
    }
  }

//--------------------------------------------------------------------------
/*
void TPPFlowStuff::BeggsAndBrillInv(TwoPhasePipe & Pipe,
                                 TPPProps &Props,
                                 double Len,
                                 double Angle,
                                 double Q,
                                 double &DPq,
                                 double &DPz,
                                 flag StartOfStep)
  {
  // Beggs and Brill correlation

  double Viscosity, LSurfTension; //Physical values need to be calculated (short term fix here) - AKS
  double nDensity, sDensity;
  //double dP_friction, dP_elevation;
  double Nfr, Nlv;
  double Vm, nLiqVolFrac, sLiqVolFrac_0, ElevAngle, L1, L2, L3, L4;
  //bHorFlowRegime;

  LSurfTension = 15e-3; // Hardwired for now, from example, Gas Processors Association

  if (0 && (fabs(dQvL) < 1e-5) && (fabs(dQvG) < 1e-4))
    {
    DPq=0;
    DPz=0;
    }
  else
    {
    // Calc Atual

//  dQmL=Q*(1.0-dVapFrac);
//  dQmG=Q-dQmL;
//  dQvL=dQmL/Props.dLiqDens;
//  dQvG=dQmG/Props.dGasDens;
//  dSupLiqVel=dQvL/Pipe.Area();
//  dSupGasVel=dQvG/Pipe.Area();
//  dTotalVel=dSupLiqVel+dSupGasVel;

    sLiqVolFrac=(dQvL)/GTZ(dQvG+dQvL);


//    Vm = fabs(dQvL + dQvG) / Pipe.Area();
//
//    ElevAngle = Angle;
//
//    // Calculate the Froude Number Nfr = Sqr(Vm) / gd
//
//    if (Pipe.dDiam == 0)
//      Nfr = 0; // Error message ??? AKS
//    else
//      Nfr        = Sqr(Vm)/(9.81 * Pipe.dDiam);
//    if (LSurfTension == 0)
//      Nlv = 0;
//    else
//      Nlv        = dSupLiqVel * pow((Props.dLiqDens/(9.81*LSurfTension)),0.25);
/    nLiqVolFrac = dQvL / GTZ(dQvL + dQvG);
    // no slip density
    nDensity = Props.dLiqDens*nLiqVolFrac+(1-nLiqVolFrac)*Props.dGasDens;
//
    Viscosity = nLiqVolFrac*20e-3+(1-nLiqVolFrac)*0.015e-3; //   Hardwired, Pa.s for now
//    //   From example, Gas processors association
//
//    double y_L;
//    if (nLiqVolFrac <= 1e-6)
//      {
//      sDensity=nDensity;
//      y_L = 1.0; // gives modifying factor of 1 for dP_friction
//      bHorFlowRegime = HFR_Segregated;
//      bFlowRegime = TPR_Stratified;
//      }
//    else
//      {
//      L1 = 316.0 * pow(nLiqVolFrac, 0.302);
//      L2 = 0.0009252 * pow(nLiqVolFrac, -2.4684);
//      L3 = 0.10 * pow(nLiqVolFrac, -1.4516);
//      L4 = 0.5 * pow(nLiqVolFrac, -6.738);
//
//      // Find flow regime in horizontal pipe
//      if (((nLiqVolFrac <= 0.01) && (Nfr <= L1)) || ((nLiqVolFrac > 0.01) && (Nfr <= L2)))
//        {
//        bHorFlowRegime = HFR_Segregated;  // Sectregated flow
//        bFlowRegime = TPR_Stratified;
//        }
//      else if ((nLiqVolFrac > 0.01) && (Nfr > L2) && (Nfr <= L3))
//        {
//        bHorFlowRegime = HFR_Transition;  // Transition flow
//        bFlowRegime = TPR_Transition;
//        }
//      else if (((nLiqVolFrac > 0.01) && (nLiqVolFrac <= 0.4) && (Nfr > L3) && (Nfr <= L1)) ||
//         ((nLiqVolFrac > 0.4)  && ((Nfr > L3) && (Nfr <= L4))))
//        {
//        bHorFlowRegime = HFR_Intermittent;  // Intermittent flow - slug or plug flow
//        bFlowRegime = TPR_Slug;
//        }
//      else if (((nLiqVolFrac <= 0.4) && (Nfr > L1)) || ((nLiqVolFrac > 0.4)  && (Nfr > L4)))
//        {
//        bHorFlowRegime = HFR_Distributed;  // Distributed flow
//        bFlowRegime = TPR_Bubble;
//        }
//      else
//        {
//        bHorFlowRegime = HFR_Segregated;  // Catch All - Sectregated flow
//        bFlowRegime = TPR_Stratified;
//        }
//      //calculate liquid holdup
//      switch (bHorFlowRegime)
//        {
//        case HFR_Segregated:
//          {
//          sLiqVolFrac_0 = sHorLiqFlowFrac(0.98, 0.4846, 0.0868, nLiqVolFrac, Nfr);
//          if (ElevAngle >= 0)
//            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-3.768,3.539,-1.614,0.011,nLiqVolFrac, Nlv, Nfr, ElevAngle);
//          else
//            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
//          break;
//          }
//        case HFR_Transition:
//          {
//          double sLiqVolFrac_1;
//          double AFactor = (L3-Nfr)/(L3-L2);
//          sLiqVolFrac_0  = sHorLiqFlowFrac(0.98, 0.4846, 0.0868, nLiqVolFrac, Nfr); //Sectregated flow
//          if (ElevAngle >= 0 )
//            sLiqVolFrac_1  = sLiqVolFrac_0*sElevFactor(-3.768,3.539,-1.614,0.011,nLiqVolFrac, Nlv, Nfr, ElevAngle); // Sectregated flow
//          else
//            sLiqVolFrac_1 = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
//          sLiqVolFrac    = AFactor*sLiqVolFrac_1;
//          sLiqVolFrac_0  = sHorLiqFlowFrac(0.845, 0.5351, 0.0173, nLiqVolFrac, Nfr); // Intermittent flow
//          if (ElevAngle >= 0)
//            sLiqVolFrac_1  = sLiqVolFrac_0*sElevFactor(0.305,-0.4473,0.0978,2.96,nLiqVolFrac, Nlv, Nfr, ElevAngle); // Intermittent flow
//          else
//            sLiqVolFrac_1 = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
//          sLiqVolFrac    = sLiqVolFrac + (1-AFactor)*sLiqVolFrac_1;
//          break;
//          }
//        case HFR_Intermittent:
//          {
//          sLiqVolFrac_0  = sHorLiqFlowFrac(0.845, 0.5351, 0.0173, nLiqVolFrac, Nfr); // Intermittent flow
//          if (ElevAngle >= 0)
//            sLiqVolFrac  = sLiqVolFrac_0*sElevFactor(0.305,-0.4473,0.0978,2.96,nLiqVolFrac, Nlv, Nfr, ElevAngle); // Intermittent flow
//          else
//            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
//          break;
//          }
//        case HFR_Distributed:
//          {
//          sLiqVolFrac_0 = sHorLiqFlowFrac(1.065,0.6824,0.0609, nLiqVolFrac, Nfr);
//          if (ElevAngle >= 0)
//            sLiqVolFrac = sLiqVolFrac_0;
//          else
//            sLiqVolFrac = sLiqVolFrac_0*sElevFactor(-0.3692,0.1244,-0.5056,4.7,nLiqVolFrac, Nlv, Nfr, ElevAngle);
//          break;
//          }
//        }
//      if (ElevAngle > 0)
//        sLiqVolFrac = sLiqVolFrac*0.924;
//      else if (ElevAngle < 0)
//        sLiqVolFrac = sLiqVolFrac*0.685; //Payne et al correction factor
//
//      // need to create a FE_Darcy object - do not know how to do?? Instead hav coppied code
//
//      sDensity = Props.dLiqDens*sLiqVolFrac+(1-sLiqVolFrac)*Props.dGasDens; //Slip density (gasses slipping past liquid)
//      y_L = nLiqVolFrac/Sqr(sLiqVolFrac);
//      }
//
//    //Calculate the pressure drop due to friction in the pipe
//    DPq = DarcyDP(dTotalVel, nDensity, Len, Pipe.dDiam, Pipe.dRough, Viscosity);//Darcy
//
//    // TOGO BACK maybe !
//    //modify darcy for 2 phase flow
//    //double S;
//    //if ((y_L > 1.0) && (y_L <= 1.200517))
//    //  {
//    //  double S2=log(1.200517)/(-0.0523+3.182*log(1.200517)-0.8725*Sqr(log(1.200517))+0.01853*pow(log(1.200517), 4));
//    //  double S1=0.0;
//    //  S=(S2-S1)/(1.200517-1.0)*(y_L-1.0)+S1;//Fitted straight line for discontinuity area
//    //                      //given function very close to a straight line!
//    //  }
//    //else
//    //  S=log(y_L)/(-0.0523+3.182*log(y_L)-0.8725*Sqr(log(y_L))+0.01853*pow(log(y_L), 4));
//    //dDarcyCorrection=exp(S);
//    dDarcyCorrection = 0.5; //fudge to make system stable.
//    DPq = -Sign(Q)*dDarcyCorrection*DPq;
//    // Slug Noise;
//    //DPq *= 1.0+Pipe.dSlugResInc*dTotLiqHoldup/Pipe.SectionVolume();
//
//    //Calculate change in pressure due to change in height
//    DPz = 9.81*sDensity*Len*sin(ElevAngle)/1000; //modified for 2 phase flow by using slip density
//    }
  }

*/
  //--------------------------------------------------------------------------

void TPPFlowStuff::Coker(TwoPhasePipe & Pipe,
                         TPPProps &Props,
                         double Len,
                         double Angle,
                         double Q,
                         double &DPq,
                         double &DPz,
                         flag StartOfStep)
  {
  // Not checked yet - flow eregimes look incorrect - AKS

  //Coker, A. K. "Understanding Two-Phasse Flow in Process Piping"


  //Establish flow regimes by Baker Parameters.


  double LSurfTension = 15.0; // dynes/cm, Hardwired for now, from example, Gas Processors Association
  double LViscosity = 20.0; //cP, Hardwired, for now, From example, Gas processors association
  double GViscosity = 0.015; //cP, Hardwired, for now, From example, Gas processors association

  sLiqVolFrac= dQvL/GTZ(dQvG+dQvL);

  if ((fabs(dQmL) < 1e-5) && (fabs(dQmG) < 1e-5))
    {
    DPq=0;
    DPz=0;
    }
  else if (fabs(dQmG) < 1e-5)
    {
    DPq= DarcyDP(dSupLiqVel, Props.dLiqDens, Len, Pipe.dDiam, Pipe.dRough, LViscosity*1e-3);//Darcy
    DPz = 9.81*Props.dLiqDens*1.0*sin(Angle)/1000;
    }
  else if (fabs(dQmL) < 1e-5)
    {
    DPq= DarcyDP(dSupGasVel, Props.dGasDens, Len, Pipe.dDiam, Pipe.dRough, GViscosity*1e-3);//Darcy
    DPz = 9.81*Props.dGasDens*1.0*sin(Angle)/1000;
    }
  else
    {
    // Calc Pressure Drop for liquid and the for gas alone
    double DPL, DPG, LMmod, YG;
    DPL= DarcyDP(dSupLiqVel, Props.dLiqDens, Len, Pipe.dDiam, Pipe.dRough, LViscosity*1e-3);//Darcy
    DPG= DarcyDP(dSupGasVel, Props.dGasDens, Len, Pipe.dDiam, Pipe.dRough, GViscosity*1e-3);//Darcy
    LMmod= sqrt(DPL/DPG); // Lockhart-Martinelli two-phase flow modulus


    //Establish flow regimes by Baker Parameters.

    double Bx, By;
    Bx= fabs(531*dQmL/dQmG*(sqrt(Props.dLiqDens*Props.dGasDens)/(Props.dLiqDens*2/3))*(pow(LViscosity,1/3)/LSurfTension));
    By= fabs(2.16*dQmG/Pipe.Area()/(sqrt(Props.dLiqDens*Props.dGasDens)));//in m/s
    By= By*3600/0.3048; //converted to ft/h

    //Calc Constants for flow regime range
    double By1=exp(9.774459-0.6548*log(Bx));
    double By2=exp(8.67694-0.1901*log(Bx));
    double By3=exp(11.3976-0.6084*log(Bx)+0.0779*log(Bx)*log(Bx));
    double By4=exp(10.7448-1.6265*log(Bx)+0.2839*log(Bx)*log(Bx));
    double By5=exp(14.569802-1.0173*log(Bx));
    double By6=exp(7.8206-0.2189*log(Bx));

    if ((By < By1) && (By < By2))
      {
      bFlowRegime = TPR_Stratified;
      }
    else if (By < By1)
      {
      bFlowRegime = TPR_Wave;
      }
    else if ((By < By6) && (By < By5))
      {
      bFlowRegime = TPR_Plug;
      }
    else if ((Bx < 7.418) && (By < By3))
      {
      bFlowRegime = TPR_Annular;
      }
    else if ((Bx > 7.418) && (By < By4) && (By < By5))
      {
      bFlowRegime = TPR_Slug;
      }
    else if ((Bx > 7.418) && (By < By3) && (By > By4))
      {
      bFlowRegime = TPR_Annular;
      }
    else if ((Bx > 7.418) && (By < By4))
      {
      bFlowRegime = TPR_Bubble;
      }
    else
      {
      bFlowRegime = TPR_Dispersed;
      }

    switch (bFlowRegime)
      {
      case TPR_Bubble:
        {
        YG= Sqr(14.2*pow(LMmod,0.75)/(pow((dQmL/Pipe.Area()*0.453592/0.3048/0.3048/3600),0.1)));
        break;
        }
      case TPR_Plug:
        {
        YG= Sqr(27.315*pow(LMmod,0.855)/(pow((dQmL/Pipe.Area()*0.453592/0.3048/0.3048/3600),0.17)));
        break;
        }
      case TPR_Stratified:
        {
        YG= Sqr(15.4*LMmod/(pow((dQmL/Pipe.Area()*0.453592/0.3048/0.3048/3600),0.8)));
        break;
        }
      case TPR_Slug:
        {
        YG= 1.19*pow(LMmod,0.815)/(pow((dQmL/Pipe.Area()*0.453592/0.3048/0.3048/3600),0.5));
        break;
        }
      case TPR_Annular:
        {
        double dInch;
        if (Pipe.dDiam > 10*0.0254)
        dInch= 10;
        else
        dInch= Pipe.dDiam/0.0254;
        YG= Sqr((4.8-0.3125*dInch)*pow(LMmod,(0.343-0.021*dInch)));
        break;
        }
      case TPR_Dispersed:
        {
        YG= Sqr(exp(1.4659+0.49138*log(LMmod)+0.04887*Sqr(log(LMmod))-0.000349*(log(LMmod)*log(LMmod)*log(LMmod))));
            break;
        }
      case TPR_Wave:
        {
        // the Huntington friction factor is used
        double Hx= dQmL/dQmG*LViscosity/GViscosity;
        double HF= exp(0.211*log(Hx)-3.993);
        DPq= 0.0336*HF*Sqr(dQmG*3600/0.453592)/(pow((Pipe.dDiam/0.0254),5)*Props.dGasDens*pow(0.3048,3)/0.453592);// in psig/ft
        DPq= DPq*Len*0.3048/6.894757; // in kPa
        break;
        }
      }
    if (bFlowRegime != TPR_Wave)
      {
      DPq= -Sign(Q)*DPG*YG;
      }
    double tDensity = (Props.dGasDens*dQvG+Props.dLiqDens*dQvL)/GTZ(dQvL+dQvG);
    DPz = 9.81*tDensity*Len*sin(Angle)/1000;
    }
  }

//--------------------------------------------------------------------------

void TPPFlowStuff::SetFlwInfo(TwoPhasePipe & Pipe,
                              TPPProps &Props,
                              double Q)
  {
  //dVapFrac=1.0; // Kludge

  dQmL=Q*(1.0-dVapFrac);
  dQmG=Q-dQmL;
  dQvL=dQmL/Props.dLiqDens;
  dQvG=dQmG/Props.dGasDens;
  dSupLiqVel=dQvL/Pipe.Area();
  dSupGasVel=dQvG/Pipe.Area();
  dTotalVel=dSupLiqVel+dSupGasVel;

  }

//--------------------------------------------------------------------------

void TPPFlowStuff::StartStep()
  {
  dVelStart=dTotalVel;
  }

//--------------------------------------------------------------------------

void TPPFlowStuff::EstimateDP1(TwoPhasePipe & Pipe,
                               TPPProps &Props,
                               double Len,
                               double Angle,
                               double Mass,
                               double Q,
                               double &DPq,
                               double &DPz,
                               double &DPmB,
                               double &DPmQ,
                               flag StartOfStep)
  {

  double dQmRng=FlwEqn::RangeFlow(Q, 0.001);
  //dQmRng=GTZ(fabs(Q));
  SetFlwInfo(Pipe, Props, dQmRng);

  DPq=0.0;
  DPmB=0.0;
  DPmQ=0.0;
  switch (1)
    {
    case 1: // Beggs & Brill
      BeggsAndBrill(Pipe, Props, Len, Angle, dQmRng, DPq, DPz, StartOfStep);
      // Coker(Pipe, Q, DPq, DPz, StartOfStep);
      //DPq=-Sign(Q)*fabs(DPq);
      break;
    default:
      {
      double R=Pipe.dLineRes*1.0;
      DPq=-Sign(Q)*(dQvL+dQvG)*R;
      break;
      }
    }
  // Reconstruct DPq
  double R=fabs(DPq)/dQmRng;
  DPq = -Q*R;

  // Reset Vel's etc to actual flows.
  SetFlwInfo(Pipe, Props, Q);

  double Denom;
  if (StartOfStep)
    {
    DPmB=0.0;
    DPmQ=0.0;
    Denom=-1;
    }
  else
    {
    Denom=(Max(ICGetTimeInc()*0.01, ICGetTimeIncFromStartOfStep())*Pipe.Area());
    DPmB=Mass*(dVelStart)*0.001/Denom;
    DPmQ=-Mass*(dTotalVel)*0.001/Denom;
    }

  #if dbgPipeline
  if (dbgEstDP() && Pipe.fDoDbgBrk)
  dbgpln("%10.10s Vs:%14.6g Vt:%14.6g Dn:%14.6g dT:%14.6g dTs:%14.6g Pa:%14.6g",
    Pipe.Tag(),dVelStart,dTotalVel,Denom, ICGetTimeInc(), ICGetTimeIncFromStartOfStep(), Pipe.Area());
  #endif

  //DPm=0.0;
  }

//--------------------------------------------------------------------------

void TPPFlowStuff::EstimateDP(TwoPhasePipe & Pipe, TPPProps &Props, double Len, double Angle, double Mass, double Q)
  {
  flag StartOfStep= !IgnoreStepStart && ICStepStart();

  //double dQmRng=Sign(Q)*FlwEqn::RangeFlow(Max(0.01, Q), 10.0);
  //double dQmRng1=Sign(Q)*FlwEqn::RangeFlow(Max(0.01, Q)*1.01, 10.0);

  double dQ=Sign(Q)*Max(0.01, fabs(Q)*1.01);
  double dQ0=Q;
  double dQ1=Q+dQ;

  //double dQ=Sign(Q)*Max(1e-8, fabs(Q)*0.1);
  //double dQ=fabs(dQmRng)*0.1;
  //double dQ=dQ1-dQ0;

  double dPq1,dPz1,dPmB1,dPmQ1,dPq0,dPz0,dPmB0,dPmQ0, dPm1, dPm0;
  EstimateDP1(Pipe, Props, Len, Angle, Mass, dQ1, dPq1, dPz1, dPmB1, dPmQ1, StartOfStep);
  EstimateDP1(Pipe, Props, Len, Angle, Mass, dQ0, dPq0, dPz0, dPmB0, dPmQ0, StartOfStep);

  dDPz=dPz0;
  dDPq=dPq0;
  dDPqDq=(dPq1-dPq0)/(dQ);
  dPm0=dPmB0+dPmQ0;
  dPm1=dPmB1+dPmQ1;

  dDPmB=dPmB0;
  dDPmQ=dPmQ0;
  dDPmDq=(dPm1-dPm0)/(dQ);

  // Reconstruct DPq
  //double R=fabs(dDPq/dQmRng);
  //dDPq=-Sign(Q)*R*fabs(Q);

  SetFlwInfo(Pipe, Props, Q);

  if (StartOfStep)
    dVelStart=dTotalVel;

  if (sLiqVolFrac < 0.99999999)
    dGasVelo=Sign(Q)*fabs(dQvG)/((1-sLiqVolFrac)*Pipe.Area());
  else
    dGasVelo=0;

  if (sLiqVolFrac > 0.00000001)
    {
    dLiqVelo=Sign(Q)*fabs(dQvL)/(sLiqVolFrac*Pipe.Area());
    if (sLiqVolFrac >= 0.99999999)
      dGasVelo=dLiqVelo;
    }
  else
    dLiqVelo=dGasVelo;

//  dGasVelo=Sign(Q)*fabs(dTotalVel);///((1-sLiqVolFrac)*Pipe.Area());
//  dLiqVelo=0.35*dGasVelo;

//  if (sLiqVolFrac > 0.00000001)
//    {
//    dLiqVelo=Sign(Q)*fabs(dQvL)/(sLiqVolFrac*Pipe.Area());
//    if (sLiqVolFrac >= 0.99999999)
//      dGasVelo=dLiqVelo;
//    }
//  else
//    dLiqVelo=dGasVelo;

  if (fabs(dGasVelo)>100 || fabs(dLiqVelo)>100)
    {
    int xxx=0;
    }

  #if dbgPipeline
  if (dbgEstDP() && Pipe.fDoDbgBrk)
  dbgpln("%10.10s %2i %2i Q:%14.6g DPq:%14.6g DPm:%14.6g LVF:%14.6g TotVel:%14.6g",
    Pipe.Tag(),bHorFlowRegime,bFlowRegime,Q,dDPq,dDPmB+dDPmQ,sLiqVolFrac,dTotalVel);
  #endif

  }

//--------------------------------------------------------------------------

void TPPFlowStuff::EstimateFlows(TwoPhasePipe & Pipe,
                                 TPPProps &Props,
                                 double Len,
                                 double Angle,
                                 double Mass,
                                 double DP)
  {

  #if dbgPipeline
  if (dbgEstDP() && Pipe.fDoDbgBrk)
    dbgpln("EstFlows DP:%14.6g", DP);
  #endif

  dDPApplied=DP;
  for (int Loop=100; ;Loop--)
    {
    EstimateDP(Pipe, Props, Len, Angle, Mass, dQmEst);
    // Reconstruct DP through R;
//    double R=fabs(dDPq/dQmRng);
//    dDPq=Sign(dDPq)*R*fabs(dQmEst);

    //@ Start of Step dont change estimates - if Momentum Calcs Used
    if (!IgnoreStepStart && ICStepStart())
      break;

    double Sigma_dP=-(dDPq+dDPmB+dDPmQ);
    double P_Applied=DP;
    double Sigma_dPdQ = fabs(dDPqDq) + fabs(dDPmDq);
    double dQUnLim=(fabs(Sigma_dPdQ) > 1.0e-8 ? - (Sigma_dP - P_Applied) / Sigma_dPdQ : 0.0);

    #if dbgPipeline
    if (dbgEstDP() && Pipe.fDoDbgBrk)
      dbgpln("%sDP:%14.6g dDPq:%14.6g dDPm:%14.6g dDPqDq:%14.6g dDPmDq:%14.6g Sigma_dPdQ:%14.6g dQUnLim:%14.6g",
             "                                                          ",
             DP,dDPq,dDPmB+dDPmQ,dDPqDq,dDPmDq,Sigma_dPdQ,dQUnLim);
    #endif

    if (fabs(dQUnLim)<(1.0e-6+1.0e-6*fabs(dQmEst)))
      break;
    if (Loop==0)
      {
      LogError(Pipe.FullObjTag(), 0, "Segment Flows not converged");
      break;
      }
    dQmEst+=dQUnLim;

    #if dbgPipeline
    if (fabs(dQmEst)>1.0e10)
      {
      int xxx=0;
      }
    #endif
    }
  }

//==========================================================================
//
//
//
//==========================================================================

static long dbgSegBaseCnt=0;

TPPSegBase::TPPSegBase()
  {
  iPos        = new Integrator ("SegPos", dPosition, dVelocity, dPosition, ValidFlags);
  iTemp       = new Integrator ("SegTemp", dTemp, dTempDeriv, dTemp, ValidFlags);
  iMass       = new Integrator ("SegMass", dMass, dMassDeriv, dMassTest, ValidFlags);
  dPosition   = 0.0;
  dVelocity   = 0.0;
  dMass       = 0.0;
  dMassDeriv  = 0.0;
  dMassTest   = 0.0;
  dTemp       = Std_T;
  dTempDeriv  = 0.0;
  iAge        = 0;
  //dbgpln("------------------ CTOR SegBase A %i", ++dbgSegBaseCnt);
  pImg = new SpConduit;//(&SM_OilClass, "Tmp", NULL, TOA_Free);
  }

//--------------------------------------------------------------------------

TPPSegBase::TPPSegBase(double Position, double Velocity, double Mass, double Temperature, SpConduit * Img)
  {
  iPos        = new Integrator("SegPos", dPosition, dVelocity, dPosition, ValidFlags);
  iTemp       = new Integrator ("SegTemp", dTemp, dTempDeriv, dTemp, ValidFlags);
  iMass       = new Integrator ("SegMass", dMass, dMassDeriv, dMassTest, ValidFlags);
  dPosition   = Position;
  dVelocity   = Velocity;
  dMass       = Mass;
  dMassDeriv  = 0.0;
  dMassTest   = 0.0;
  dTemp       = Temperature;
  dTempDeriv  = 0.0;
  iAge        = 0;

  //dbgpln("------------------ CTOR SegBase B %i", ++dbgSegBaseCnt);
  pImg = new SpConduit;//M_Oil(&SM_OilClass, "Tmp", NULL, TOA_Free);
  pImg->QCopy(*Img);
  }

//--------------------------------------------------------------------------

TPPSegBase::~TPPSegBase()
  {
  delete iPos;
  delete iTemp;
  delete iMass;
  delete pImg;
  //dbgpln("------------------ DTOR SegBase %i", dbgSegBaseCnt--);

  }

//--------------------------------------------------------------------------

TPPGSegment::TPPGSegment()
  {
  dPress      = Std_P;
  dPressGrad  = 0.0;
  dLiqMass    = 0.0;
  }

//--------------------------------------------------------------------------

TPPGSegment::TPPGSegment(double Position, double Velocity, double Mass, double Temperature, TPPGasProps & Properties, SpConduit * Img) :
  TPPSegBase(Position, Velocity, Mass, Temperature, Img)
  {
  GProps      = Properties;
  dPress      = Std_P;
  dPressGrad  = 0.0;
  dLiqMass    = 0.0;
  }

//--------------------------------------------------------------------------

TPPLSegment::TPPLSegment()
  {
  }

//--------------------------------------------------------------------------

TPPLSegment::TPPLSegment(double Position, double Velocity, double Mass, double Temperature, TPPLiqProps & Properties, SpConduit * Img) :
  TPPSegBase(Position, Velocity, Mass, Temperature, Img)
  {
  LProps      = Properties;
  }

////--------------------------------------------------------------------------
//
//double TPPSection::EiCoeff(int CoeffNo, double Omega, double mu_r, double k)
//// AKS
///* Chung et al. Coefficients to Calculate Ei, Table 9-5,
//   Reid R C, Prausnitz J M, Polling B E,1987,'The Properties of Gases & Liquids',
//   4th Ed, McGraw-Hill, NewYork, pp 427 */
//  {
//  switch (CoeffNo)
//    {
//    case 1:
//      return 6.324+50.412*Omega-51.680*pow(mu_r,4)+1189.0*k;
//    case 2:
//      return 1.21e-3-1.154e-3*Omega-6.257e-3*pow(mu_r,4)+0.03728*k;
//    case 3:
//      return 5.283+254.209*Omega-168.48*pow(mu_r,4)+3898.0*k;
//    case 4:
//      return 6.623+38.096*Omega-8.464*pow(mu_r,4)+31.42*k;
//    case 5:
//      return 19.745+7.630*Omega-14.354*pow(mu_r,4)+31.53*k;
//    case 6:
//      return -1.9-12.537*Omega+4.985*pow(mu_r,4)-18.15*k;
//    case 7:
//      return 24.275+3.450*Omega-11.291*pow(mu_r,4)+69.35*k;
//    case 8:
//      return 0.7972+1.117*Omega+0.01235*pow(mu_r,4)-4.117*k;
//    case 9:
//      return -0.2382+0.06770*Omega-0.8163*pow(mu_r,4)+4.025*k;
//    case 10:
//      return 0.06863+0.3479*Omega+0.5926*pow(mu_r,4)-0.727*k;
//    default:
//      return 1;
//    }
//  }
////--------------------------------------------------------------------------
//double kFac(double NoOHgroups, double MolWeight)
//// for general viscosity calcs, should calc and enter in database
//// AKS
///* Chung et al.
//   Reid R C, Prausnitz J M, Polling B E,1987,'The Properties of Gases & Liquids',
//   4th Ed, McGraw-Hill, NewYork, pp 396*/
//  {
//  if (NoOHgroups == 0)
//    return 0.0;
//  else
//    return 0.0682+0.2767*(17*NoOHgroups/MolWeight);
//  }
//
////--------------------------------------------------------------------------
//
//double TPPSection::CalcGasViscosity(TwoPhasePipe & Pipe)
//// AKS
//
///* Chung et al.
//   Reid R C, Prausnitz J M, Polling B E,1987,'The Properties of Gases & Liquids',
//   4th Ed, McGraw-Hill, NewYork, pp 413,414
//  Mixing rules*/
//
////AKS - Assume have a matrix of acentric factors, dipole moments, and association values (k)
// {
///*  double Sum_e_k, Sum_sigma3, Sum_omega, Sum_k, Sum_M, Sum_dipole4;
//  double ij_e_k, ij_sigma, ij_omega, ij_k, ij_M;
//  double i_e_k, j_e_k, i_sigma, j_sigma, i_omega, j_omega, i_k, j_k, i_M, j_M;
//  double i_y, j_y;
//
//  Sum_e_k   = 0;
//  Sum_sigma = 0;
//  Sum_omega = 0;
//  Sum_k     = 0;
//  Sum_M     = 0;
//
//  for (int j = 0; SDB.No(); j++)
//    {
//    if SDB[j].IsVap()
//      {
//      j_sigma   = 0.809*pow((SDB[j].VCrit()*1e3), (1/3));
//      j_e_k     = SBD[j].TCrit()/1.2593;
//      j_omega   = SBD[j].AcentricFac(); // AKS - assume that we have, currently do not!!
//      j_k       = SBD[j].kFac(); // AKS - assume that we have, currently do not!!
//      j_dipole  = SBD[j].DipoleMom(); // AKS - assume that we have, currently do not!!
//      j_M       = SBD[j].MolWt();
//
//      j_y       = SBD[j].MolFrac();
//
//      for (int i = 0; SDB.No();i++)
//        {
//        if SDB[i].IsVap()
//          {
//          i_sigma   = 0.809*pow((SDB[i].VCrit()*1e3), (1/3));
//          ij_sigma  = sqrt(i_sigma*j_sigma);
//          i_e_k     = SBD[i].TCrit()/1.2593;
//          ij_e_k    = sqrt(i_e_k*j_e_k);
//          i_omega   = SBD[i].AcentricFac(); // AKS - assume that we have, currently do not!!
//          ij_omega  = (i_omega+j_omega)/2;
//          i_k       = SBD[i].kFac(); // AKS - assume that we have, currently do not!!
//          ij_k      = sqrt(i_k*j_k);
//          i_dipole  = SBD[i].DipoleMom(); // AKS - assume that we have, currently do not!!
//          i_M       = SBD[i].MolWt();
//          if (i_M + j_M) == 0
//            ij_M    = 0;
//          else
//            ij_M      = 2*i_M*j_M/(i_M+j_M);
//          i_y       = SBD[i].MolFrac();
//
//          Sum_sigma3 = Sum_sigma3 + i_y*j_y*pow(ij_sigma, 3);
//          Sum_e_k    = Sum_e_k + i_y*j_y*ij_e_k*pow(ij_sigma, 3);
//          Sum_M      = Sum_M + i_y*j_y*ij_e_k*Sqr(ij_sigma)*sqrt(ij_M);
//          Sum_omega  = Sum_omega + i_y*j_y*ij_omega*pow(ij_sigma, 3);
//          if !(ij_sigma == 0)
//            Sum_dipole4= Sum_dipole4 + i_y*j_y*Sqr(i_dipole*j_dipole)/pow(ij_sigma, 3);
//          Sum_k      = Sum_k + i_y*j_y*ij_k;
//          }
//        }
//      }
//    }
//  if (Sum_sigma == 0) // Error, or bad data - AKS
//    {
//    Sum_e_k     = 0;
//    Sum_M       = 0;
//    Sum_omega   = 0;
//    Sum_dipole4 = 0;
//    }
//  else
//    {
//    Sum_e_k     = Sum_e_k/Sum_sigma;
//    Sum_omega   = Sum_omega/Sum_sigma;
//    Sum_dipole4 = Sum_dipole4*Sum_sigma;
//    Sum_sigma   = pow(Sum_sigma, (1/3));
//    if (Sum_e_k == 0)
//      Sum_M  = 0;  // Error, or bad data - AKS
//    else
//      Sum_M     = Sqr(Sum_M/(Sum_e_k*Sqr(Sum_sigma));
//
//    }
//
//
//  double T1, Tc, Vc, MolWeight, Acentric, dipoleMom4,  k;
//
//  double T = Pipe.dTemp; // Temperature of the pipe, check var name - AKS
//
//  if (Sum_e_k == 0)/ / Error, or bad data - AKS
//    T1 = 0;
//  else
//    T1 = T/Sum_e_k;
//
//  MolWeight   = Sum_M;
//  Acentric    = Sum_omega;
//  k           = Sum_k;
//  Tc          = 1.2593*Sum_e_k;
//  Vc          = pow((Sum_sigma/0.809), 3);
//  if ((Vc == 0) || (Tc == 0))  // Error, or bad data - AKS
//    dipoleMom4 = 0;
//  else
//    dipoleMom4  = Sum_dipole4*pow(131.3, 4)/Sqr(Vc*Tc);
//
//*/
//
///* Chung et al.
//   Reid R C, Prausnitz J M, Polling B E,1987,'The Properties of Gases & Liquids',
//   4th Ed, McGraw-Hill, NewYork, pp 396,397,426,427
//
//   Viscosity [Pa.s]
//   MolWeight [g/mol]
//   Tc        [K]
//   Vc        [cm^3/mol]
//   Rho       [kg/m^3]
//   dipoleMom [debyes]
//
//   An accurate estimate of the density at the T and P is required */
//
//
//  /*  double y, G1, G2, Visc1, Visc2, Fc, OmegaV;
//    double E1, E2, E3, E4, E5, E6, E7, E8, E9, E10;
//
//    y = dGasDens*Vc/6;
//    E1 = EiCoeff(1, Acentric, dipoleMom4, k);
//    E2 = EiCoeff(2,Acentric, dipoleMom4, k);
//    E3 = EiCoeff(3 Acentric, dipoleMom4, k);
//    E4= EiCoeff(4, Acentric, dipoleMom4, k);
//    E5= EiCoeff(5,Aentric, dipoleMom4, k);
//    E6= EiCoeff(6,Aentric, dipoleMom4, k);
//    E7= EiCoeff(7, Acentric, dipoleMom4, k);
//    E8= EiCoeff(8, Acentric, dipoleMom4, k);
//    E9= EiCoeff(9, Acentric, dipoleMom4, k);
//    E10= EiCoeff(10, Acentric, dipoleMom4, k);
//
//
//    G1 = (1-0.5*y)/pow((1-y),3);
//    G2  = E1((1-exp(-E4*y))/y)+E2*G1*exp(E5*y)+E3*G1;
//    G2 = G2/(E1*E4+E2+E3);
//
//    Fc = 1-0.2756*Acentric+0.059035*dipoleMom4+k;
//
//    OmegaV = 1.16145*pow(T1,-0.14874)+0.52487*exp(-0.77320*T1)+2.16178*exp(-2.43787*T1);
//
//    Visc1 = E7*Sqr(y)*G2*exp(E8+E9/T1+E10/Sqr(T1));
//    Visc2 = sqrt(T1)/OmegaV*Fc*(1/G2+E6*y)+Visc1;
//
//    return = Visc2*36.344*sqrt(MolWeight*Tc)/pow(Vc, 2/3)*1e-7;
//*/
//return 0;
// }
//==========================================================================
//
//
//
//==========================================================================

//ACCESS_SPECIE(H2o, "H2o");

//==========================================================================
//
//                        Two Phase Pipeline
//
//==========================================================================
/*#D:#T:TwoPhasePipe
#X:#n#u<#l<#b<General Description>>>#N#NThis model is used to calculate the flow
conditons in a pipeline carrying gases and liquids.  The model calculates the flow
regime, pressure drop due to friction for each regime and the heat loss from the pipeline.

#n#n#h<Variables to be supplied by the user>
#n#i<Z_Rqd :>The required datum of the pipe.
#n#i<Rs :>The resistance of the TwoPhasePipe (to do).
#n#i<Tau :>The damping constant of the TwoPhasePipe.
#n#i<Flash :>This determines whether flashing will occur in the pipe , 1 indicates that it occurs, 0
indicates that it does not occur. The default is 1
#n#i<FlashIn :>This determines whether the material is able to flash as it enters the TwoPhasePipe  (to
do) , 1 indicates that it occurs, 0 indicates that it does not occur. The default is 0
#n#i<T_Rqd :>The required temperature of the material within the TwoPhasePipe.
#n#i<P_Rqd :>The required pressure of the material within the TwoPhasePipe.
#n#i<Model :>This allows the selection of the model that is
be used to calculate how the species are combined, there are two options :
'Mass Wt Mean' : A mass weighted mean is used.
'Light/Heavy Oil' : This uses relative density.
#n#i<T :>The temperature of the material within the TwoPhasePipe. The default is  20dC
#n#i<Species List :>This is a list of all the species, used in the particular flow engine, with their
current mass in kilograms. This is where the specie composition of the TwoPhasePipe is determined. The
default, for each species, is 0 kg

#n#n#h<Variables calculated by the model>
#n#i<Z :>The height of the TwoPhasePipe, in relation to the common datum.
#n#i<P :>The pressure of the material within the TwoPhasePipe.
#n#i<Rho :>The density of the material within the TwoPhasePipe.
#n#i<NdP :>The normalised pressure drop across the TwoPhasePipe.
#n#i<Mt :>The total mass of all the species within the TwoPhasePipe.
#n#i<Rho :>The density of the material within the TwoPhasePipe.
#n#i<Sf :>The percentage of solids within the TwoPhasePipe.
#n#i<Lf :>The percentage of liquids within the TwoPhasePipe.
#n#i<Vf :>The percentage of vapours within the TwoPhasePipe.

#n#h<Other>#n
Default model prefix:TPP#n
Short name:TwoPhPip#n
Model type:Link
#G:Links
*/

// To Improve
long Drw_TwoPhasePipe[] = { DD_Poly, -10,1, -10,-1, 10,-1, 10,1, -10,1,
                            DD_End };

IMPLEMENT_MODELUNIT(TwoPhasePipe, "TwoPhPip", "", Drw_TwoPhasePipe, "TPP", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "TwoPhasePipe",
                    "TwoPhasePipe")


const int MinNSects=5;
const int MinNSegs=4;

//--------------------------------------------------------------------------

TwoPhasePipe::TwoPhasePipe(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
//  MdlNode(pClass_, TagIn, pAttach, eAttach),
  Pipe(pClass_, TagIn, pAttach, eAttach),
  FlowImg("FlowImg", this, TOA_Embedded)//,
  {
  AttachIOAreas(TwoIOAreaList);

  bSimple=false;

  fFlash  = 0;
  dLength = 2000.0;
  dDiam   = 12.0*0.0254;
  dRough  = 0.0003; //AKS
  dAngleS = 0.0;
  dAngle  = 0;      //AKS
  dAngleE = 0.0;
  dLengthS = dNAN();
  dLengthE = dNAN();
  dMaxSlugVolume = 2.86;

  dSlugResInc    = 0.1;
  dSlugAppears   = 0.05;
  dSlugMinLen    = 0.5;
  dSlugReduction = 0.4;

  SetNSections(2);
  SetNGSegments(5);
  SetNLSegments(5);
  InitSlug.SetSize(3);
  InitSlug[0].dPosition = -1.0;
  InitSlug[0].dVolume   = 0.0;
  InitSlug[1].dPosition = -1.0;
  InitSlug[1].dVolume   = 0.0;
  InitSlug[2].dPosition = -2.0;
  InitSlug[2].dVolume   = 0.0;

  dSeaTemp = C_2_K(8.0);
  dHTC     = 0.005;
  dODiam   = dDiam + 2.0 * 0.0254;

  dLineRes      = 1.0;
  dRqdLinePress = 5000.0;
  dRqdLineTemp  = Std_T;
  dRqdVapFrac   = 0.94;
  dTotLiqHoldup = 0.0;
  nSlugs        = 0;
  nMaxSlugs     = 5;

  dQmGEntry = 0.0;
  dQmLEntry = 0.0;
  dQmGExit  = 0.0;
  dQmLExit  = 0.0;
  dCpGEntry = 0.0;
  dCpLEntry = 0.0;
  dCpGExit  = 0.0;
  dCpLExit  = 0.0;

  fDoSetLinePress=false;
  fDoSetLineSlugs=false;

  };

//--------------------------------------------------------------------------

TwoPhasePipe::~TwoPhasePipe()
  {
  GasSeg.SetSize(0);
  LiqSeg.SetSize(0);
  //SetNGSegments(0);
  //SetNLSegments(0);
  };

//--------------------------------------------------------------------------

void TwoPhasePipe::BuildDataDefn(DataDefnBlk &DDB)
  {
  DDB.CheckBoxBtn("Simple",  "",       DC_, "",      &bSimple,     this, isParm);//|SetOnChange);
  if (bSimple)
    {
    Pipe::BuildDataDefn(DDB);
    return;
    }
  DDB.BeginStruct(this);

  DDB.Text   ("");
  DDB.Double ("GasDens",        "",      DC_Rho,  "kg/m^3",   &FdProps.dGasDens,        this, 0);
  DDB.Double ("LiqDens",        "",      DC_Rho,  "kg/m^3",   &FdProps.dLiqDens,        this, 0);
  DDB.Double ("GasCp",          "",      DC_CpMs,   "kJ/kg.C",  &FdProps.dGasCp,          this, 0);
  DDB.Double ("LiqCp",          "",      DC_CpMs,   "kJ/kg.C",  &FdProps.dLiqCp,          this, 0);

  DDB.Text   ("");
  DDB.Double ("QmGEntry",       "",      DC_Qm,  "kg/s",      &dQmGEntry,               this, 0);
  DDB.Double ("QmLEntry",       "",      DC_Qm,  "kg/s",      &dQmLEntry,               this, 0);
  DDB.Double ("QmGExit",        "",      DC_Qm,  "kg/s",      &dQmGExit,                this, 0);
  DDB.Double ("QmLExit",        "",      DC_Qm,  "kg/s",      &dQmLExit,                this, 0);
  DDB.Double ("CpGEntry",       "",      DC_CpMs,   "kJ/kg.C",  &dCpGEntry,               this, noView);
  DDB.Double ("CpLEntry",       "",      DC_CpMs,   "kJ/kg.C",  &dCpLEntry,               this, noView);
  DDB.Double ("CpGExit",        "",      DC_CpMs,   "kJ/kg.C",  &dCpGExit,                this, noView);
  DDB.Double ("CpLExit",        "",      DC_CpMs,   "kJ/kg.C",  &dCpLExit,                this, noView);
  DDB.Double ("TempEntry",      "",      DC_T,   "C",         &dTempEntry,              this, 0);
  DDB.Double ("TempExit",       "",      DC_T,   "C",         &dTempExit,               this, 0);

  if (LastFullGSeg()>0)
    {
    DDB.Double ("PSeg0",          "",      DC_P,   "kPa",        &GasSeg[0]->dPress,      this, 0|InitHidden);
    DDB.Double ("PSegN",          "",      DC_P,   "kPa",        &GasSeg[LastFullGSeg()]->dPress, this, 0|InitHidden);
    }

  BuildDataDefnShowIOs(DDB);

  DDB.Page("Parms", DDB_OptPage);
  DDB.Double ("Datum",          "Z",     DC_L,    "m",        &dDatum,                  this, 0);
  DDB.Long   ("NSections",      "",      DC_,     "",         xidNSections,             this, isParm);
  DDB.Long   ("NGSegments",     "",      DC_,     "",         xidNGSegments,            this, 0);
  DDB.Long   ("NLSegments",     "",      DC_,     "",         xidNLSegments,            this, 0);
  DDB.Double ("LengthS",        "",      DC_L,    "m",        &dLengthS,                this, isParm|NAN_OK);
  DDB.Double ("Length",         "",      DC_L,    "m",        &dLength,                 this, isParm);
  DDB.Double ("LengthE",        "",      DC_L,    "m",        &dLengthE,                this, isParm|NAN_OK);
  DDB.Double ("Diameter",       "",      DC_L,    "m",        &dDiam,                   this, isParm);
  DDB.Double ("PipeRoughness","Rough",   DC_L,    "mm",       &dRough,                  this, isParm);
  DDB.Double ("",            "AngleS",   DC_Ang , "deg",      &dAngleS,                 this, isParm);
  DDB.Double ("",            "Angle",    DC_Ang , "deg",      &dAngle,                  this, isParm);
  DDB.Double ("",            "AngleE",   DC_Ang , "deg",      &dAngleE,                 this, isParm);
  DDB.Double ("MaxSlugVolume",  "",      DC_Vol,  "m^3",      &dMaxSlugVolume,          this, isParm);
  DDB.Double ("TotLiqHoldup",   "",      DC_Vol,  "m^3",      &dTotLiqHoldup,           this, 0);
  DDB.Long   ("MaxNSlugs",      "",      DC_,     "",         &nMaxSlugs,               this, isParm);
  DDB.Long   ("NSlugs",         "",      DC_,     "",         &nSlugs,                  this, 0);

  DDB.Text   ("Slug Growth");
  DDB.Double ("dSlugAppears",   "",      DC_Frac, "%",        &dSlugAppears,            this, isParm);
  DDB.Double ("dSlugResInc",    "",      DC_Frac, "%",        &dSlugResInc,             this, isParm);
  DDB.Double ("SlugMinLen",     "",      DC_L,    "m",        &dSlugMinLen,             this, isParm);
  DDB.Double ("SlugReduction",  "",      DC_Frac, "%",        &dSlugReduction,          this, isParm);

  DDB.Text   ("Thermal Loss");
  DDB.Double ("SeaTemp",        "",      DC_T,    "C",        &dSeaTemp,                this, isParm);
  DDB.Double ("ODiameter",      "",      DC_L,    "m",        &dODiam,                  this, isParm);
  DDB.Double ("HTC",            "",      DC_HTC,  "W/m^2K",   &dHTC,                    this, isParm);

  DDB.Text   ("Initial Conditions");
  DDB.Double ("Press_Rqd",      "P_Rqd", DC_P,    "kPag",     &dRqdLinePress,           this, isParm);
  DDB.Double ("Temp_Rqd",       "T_Rqd", DC_T,    "C",        &dRqdLineTemp,            this, isParm);
  DDB.Double ("VapFrac_Rqd",    "",      DC_Frac, "%",        &dRqdVapFrac,             this, isParm);
  DDB.Text   ("Initial Slugs");
  DDB.Double ("SlugPos0",       "",      DC_L,    "m",        &InitSlug[0].dPosition,   this, isParm);
  DDB.Double ("SlugVol0",       "",      DC_Vol,  "m^3",      &InitSlug[0].dVolume,     this, isParm);
  DDB.Double ("SlugPos1",       "",      DC_L,    "m",        &InitSlug[1].dPosition,   this, isParm);
  DDB.Double ("SlugVol1",       "",      DC_Vol,  "m^3",      &InitSlug[1].dVolume,     this, isParm);
  DDB.Double ("SlugPos2",       "",      DC_L,    "m",        &InitSlug[2].dPosition,   this, isParm);
  DDB.Double ("SlugVol2",       "",      DC_Vol,  "m^3",      &InitSlug[2].dVolume,     this, isParm);
  DDB.Button ("Set Line Press", "",      DC_,     "",         xidTPPSetLineP,           this, isParm);
  DDB.Button ("Set Line Slugs", "",      DC_,     "",         xidTPPSetLineSegs,        this, isParm);

  DDB.Text   ("");

  static DDBValueLst DDBRegimes[]={
    {TPR_Stratified, "Stratified"},
    {TPR_Transition, "Transition"},
    {TPR_Slug, "Slug"},
    {TPR_Bubble, "Bubble"},
    {TPR_Spray, "Spray"},
    {TPR_Annular, "Annular"},
    {TPR_Wave, "Wave"},
    {0}};

  static DDBValueLst DDBFlwRegimes[]={
    {HFR_Segregated,   "Segregated"},
    {HFR_Transition,   "Transition"},
    {HFR_Intermittent, "Intermittent"},
    {HFR_Distributed,  "Distributed"},
    {0}};

  DDBPages PgTyp=DDB_RqdPage;
  Strng Tg;
  DDB.BeginArray(this, "Sect", "TPPSects", NSections(), 0);
  for (int i = 0; i<NSections(); i++)
    {
    Tg.Set("%i",i);
    DDB.Visibility(ALLMODES, True, True);
    DDB.BeginElement(this, Tg(), NULL);
    if ((i%10)==0 && i<NSections()-1)
      {
      Tg.Set("Sect%i",i);
      DDB.Page(Tg(), PgTyp);
      //PgTyp=DDB_OptPage;
      }
    DDB.Long   ("NSlugs",         "",   DC_,     "",        &Sect[i].nSlugs,              this, 0);
    DDB.Double ("LiquidHoldup",   "",   DC_Vol,  "m^3",     &Sect[i].dTotLiqHoldup,       this, 0);
//    DDB.Text("");
    DDB.Double ("Start",          "",   DC_L,    "m",       &Sect[i].dStart,              this, 0);
    DDB.Double ("End",            "",   DC_L,    "m",       &Sect[i].dEnd,                this, 0);
    }
  DDB.EndArray();

  PgTyp=DDB_RqdPage;
  DDB.BeginArray(this, "TPJoin", "TPPJoin", NJoins(), 0);
  for (i = 0; i<NJoins(); i++)
    {
    Tg.Set("%i",i);
    DDB.Visibility(ALLMODES, True, True);
    DDB.BeginElement(this, Tg(), NULL);
    if ((i%10)==0 && i<NJoins()-1)
      {
      Tg.Set("Join%i",i);
      DDB.Page(Tg(), PgTyp);
      //PgTyp=DDB_OptPage;
      }

    DDB.Double ("T",              "",   DC_T,    "C",       &Join[i]->dMeasT,               this, 0);
    DDB.Double ("P",              "",   DC_P,    "kPa",     &Join[i]->dMeasP,               this, 0);
    DDB.Double ("GasVelocity",    "",   DC_Ldt, "m/s",      &Join[i]->dMeasGasVel,          this, 0);
    DDB.Double ("LiqVelocity",    "",   DC_Ldt, "m/s",      &Join[i]->dMeasLiqVel,          this, 0);
    DDB.Byte   ("HorFlowRegime",  "",   DC_,    "",         &Join[i]->bMeasHorFlowRegime,   this, 0, DDBFlwRegimes);
    DDB.Byte   ("FlowRegime",     "",   DC_,    "",         &Join[i]->bMeasFlowRegime,      this, 0, DDBRegimes);
    DDB.Double ("Pos",            "",   DC_L,    "m",       xidJoinPos+i,                   this, 0);
    }
  DDB.EndArray();

  PgTyp=DDB_RqdPage;
  DDB.BeginArray(this, "GSeg", "TPPSegs", NSections(), 0);
  for (int sg=0; sg<NGSegments(); sg++)
    {
    TPPGSegment &Sg=GSeg(sg);
    Tg.Set("%i", sg);
    DDB.BeginElement(this, Tg(), NULL);
    if ((sg%5)==0 && sg<NGSegments()-1)
      {
      Tg.Set("GSeg%i",sg);
      DDB.Page(Tg(), PgTyp);
      //PgTyp=DDB_OptPage;
      }

    DDB.Double ("Position",        "",   DC_L,   "m",       &Sg.dPosition,               this, 0);
    if (sg<NFullGSegs())
      {
      //double dLength=GSegLength(sg);
      DDB.Double ("Velocity",        "",   DC_Ldt, "m/s",     &Sg.dVelocity,               this, 0);
      DDB.Double ("Length",          "",   DC_L,   "m",       xidGSegLen+sg,               this, 0);
      DDB.Double ("Mass",            "",   DC_M,   "kg",      &Sg.dMass,                   this, 0);
      DDB.Double ("Temperature",     "",   DC_T,   "C",       &Sg.dTemp,                   this, 0);
      DDB.Double ("VapFrac",         "",   DC_Frac,"%",       &Sg.dVapFrac,                this, 0);
      //DDB.Byte   ("HorFlowRegime",   "",   DC_,    "",        &Sg.bHorFlowRegime,          this, 0, DDBFlwRegimes);
      DDB.Byte   ("FlowRegime",      "",   DC_,    "",        &Sg.bFlowRegime,             this, 0, DDBRegimes);
      DDB.Double ("Press",           "",   DC_P,   "kPa",     &Sg.dPress,                  this, 0);
      DDB.Double ("PressGrad",       "",   DC_P,   "kPa",     &Sg.dPressGrad,              this, 0);
      DDB.Double ("GasVelo",         "",   DC_Ldt, "m/s",     &Sg.dGasVelo,                this, 0);
      DDB.Double ("LiqVelo",         "",   DC_Ldt, "m/s",     &Sg.dLiqVelo,                this, 0);
      DDB.Double ("GasDens",         "",   DC_Rho, "kg/m^3",  &Sg.GProps.dGasDens,         this, 0|InitHidden/*noView*/);
      DDB.Double ("NGasDens",        "",   DC_Rho, "kg/m^3",  &Sg.GProps.dNormGasDens,     this, 0|InitHidden/*noView*/);
      DDB.Double ("GasCp",           "",   DC_CpMs,  "kJ/kg.C", &Sg.GProps.dGasCp,           this, 0|InitHidden/*noView*/);
      DDB.Double ("QmEst",           "",   DC_Qm,  "kg/s",    &Sg.dQmEst,                  this, 0|InitHidden/*noView*/);
      DDB.Double ("TotVel",          "",   DC_Ldt, "m/s",     &Sg.dTotalVel,               this, 0|InitHidden/*noView*/);
      }
    }
  DDB.EndArray();

  PgTyp=DDB_RqdPage;
  DDB.BeginArray(this, "LSeg", "TPPSegs", NSections(), 0);
  for (sg=0; sg<NLSegments(); sg++)
    {
    TPPLSegment &Sg=LSeg(sg);
    Tg.Set("%i", sg);
    DDB.BeginElement(this, Tg(), NULL);
    if ((sg%5)==0 && sg<NLSegments()-1)
      {
      Tg.Set("LSeg%i",sg);
      DDB.Page(Tg(), PgTyp);
      //PgTyp=DDB_OptPage;
      }

    DDB.Double ("Position",        "",   DC_L,   "m",       &Sg.dPosition,                 this, 0);
    if (sg<NFullLSegs())
      {
      //uble dLength=LSegLength(sg);
      DDB.Double ("Velocity",        "",   DC_Ldt, "m/s",     &Sg.dVelocity,               this, 0);
      DDB.Double ("Length",          "",   DC_L,   "m",       xidLSegLen+sg,               this, 0);
      DDB.Double ("Mass",            "",   DC_M,   "kg",      &Sg.dMass,                   this, 0);
      DDB.Double ("Temperature",     "",   DC_T,   "C",       &Sg.dTemp,                   this, 0);
      DDB.Double ("FillFrac",        "",   DC_Frac,"%",       xidLSegFillFrac+sg,          this, 0);
      //DDB.Byte   ("HorFlowRegime",   "",   DC_,    "",        &Sg.bHorFlowRegime,          this, 0, DDBFlwRegimes);
      DDB.Byte   ("FlowRegime",      "",   DC_,    "",        &Sg.bFlowRegime,             this, 0, DDBRegimes);
      //DDB.Double ("Press",           "",   DC_P,   "kPa",     &Sg.dPress,                  this, 0);
      //DDB.Double ("PressGrad",       "",   DC_P,   "kPa",     &Sg.dPressGrad,              this, 0);
      DDB.Double ("GasVelo",         "",   DC_Ldt, "m/s",     &Sg.dGasVelo,                this, 0);
      DDB.Double ("LiqVelo",         "",   DC_Ldt, "m/s",     &Sg.dLiqVelo,                this, 0);
      DDB.Double ("LiqDens",         "",   DC_Rho, "kg/m^3",  &Sg.LProps.dLiqDens,         this, noView);
      DDB.Double ("LiqCp",           "",   DC_CpMs,  "kJ/kg.C", &Sg.LProps.dLiqCp,           this, noView);
      }
    }
  DDB.EndArray();

  DDB.Object(&FlowImg, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

static byte TPP_Cnt=0x38;
static byte TPP_End=0x39;
flag TwoPhasePipe::GetOtherData(FilingControlBlock &FCB)
  {
//  DWORD nBytes;
//  for (int s=0; s<Join.GetSize(); s++)
//    {
//    FCB.WriteFile(&TPP_Cnt, sizeof(TPP_Cnt), &nBytes);
//    Join[s]->GetOtherData(FCB);
//    }
//
//  FCB.WriteFile(&TPP_End, sizeof(TPP_End), &nBytes);

  return True;
  };

// --------------------------------------------------------------------------

flag TwoPhasePipe::PutOtherData(FilingControlBlock &FCB)
  {
//  int RqdSects=2;
//
//  byte What;
//  DWORD nRead;
//  FCB.ReadFile(&What, sizeof(What), &nRead);
//  while (What==TPP_Cnt && nRead==sizeof(What))
//    {
//    SetNSections(Max(NSections(), RqdSects));
//    Join[RqdSects-2]->PutOtherData(FCB);
//    FCB.ReadFile(&What, sizeof(What), &nRead);
//    RqdSects++;
//    }
//  ASSERT(What==TPP_End);

  return True;
  };

// -------------------------------------------------------------------------

void TwoPhasePipe::SetNSections(int NSects)
  {
  NSects=Range(MinNSects,NSects,MaxTPPSections);
  Join.SetSize(NSects-1);
  Sect.SetSize(NSects);

  double SLen=Valid(dLengthS) ? dLengthS : 0.5 * dLength /(NSects-1);
  double ELen=Valid(dLengthE) ? dLengthE : 0.5 * dLength /(NSects-1);
  double MLen=(dLength-(SLen+ELen))/(NSects-2);

  double Pos=0.0;
  for (int s=0; s<Sect.GetSize(); s++)
    {
    Sect[s].iMyIndex=s;
    Sect[s].dStart=Pos;
    Pos+=(s==0 ? SLen : (s<UBSect() ? MLen : ELen));
    Sect[s].dEnd=Pos;
    Sect[s].dAngle=  (s==0 ? dAngleS : (s<UBSect() ? dAngle : dAngleE));
    }
  }

// -------------------------------------------------------------------------

void TwoPhasePipe::SetNGSegments(int NSegs)
  {
  GasSeg.SetSize(Max(MinNSegs,NSegs), 32);
  SegsChanged();
  };

// -------------------------------------------------------------------------

void TwoPhasePipe::SetNLSegments(int NSegs)
  {
  LiqSeg.SetSize(Max(MinNSegs,NSegs), 32);
  SegsChanged();
  };

// --------------------------------------------------------------------------

flag TwoPhasePipe::DataXchg(DataChangeBlk & DCB)
  {
  if (bSimple)
    return Pipe::DataXchg(DCB);

  if (MdlNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case xidNSections:
      {
      if (DCB.rL)
        {
        SetNSections(*DCB.rL);
        SetLinePress();
        }
      DCB.L=NSections();
      return 1;
      }
    case xidNGSegments:
      {
      if (DCB.rL)
        SetNGSegments(*DCB.rL);
      DCB.L=NGSegments();
      return 1;
      }
    case xidNLSegments:
      {
      if (DCB.rL)
        SetNLSegments(*DCB.rL);
      DCB.L=NLSegments();
      return 1;
      }
    case xidTPPSetLineP:
      {
      if (DCB.rB && (*DCB.rB!=0) && (DCB.dwOpts &(TABOpt_ForFile|TABOpt_ForSnapShot|TABOpt_ForScenario))==0)
        {
        SetLinePress();
//        SetLineSegs(False);
        }
      DCB.B=0;
      return 1;
      }
    case xidTPPSetLineSegs:
      {
      if (DCB.rB && (*DCB.rB!=0) && (DCB.dwOpts &(TABOpt_ForFile|TABOpt_ForSnapShot|TABOpt_ForScenario))==0)
        SetLineSegs(True);
      DCB.B=0;
      return 1;
      }
    default :
      {
      if (DCB.lHandle>=xidJoinStart && DCB.lHandle<xidJoinStart+Join.GetSize())
        DCB.D=JoinStart(DCB.lHandle-xidJoinStart);
      else if (DCB.lHandle>=xidJoinEnd && DCB.lHandle<xidJoinEnd+Join.GetSize())
        DCB.D=JoinEnd(DCB.lHandle-xidJoinEnd);
      else if (DCB.lHandle>=xidJoinPos && DCB.lHandle<xidJoinPos+Join.GetSize())
        DCB.D=JoinPos(DCB.lHandle-xidJoinPos);
      else if (DCB.lHandle>=xidLSegFillFrac && DCB.lHandle<xidLSegFillFrac+LiqSeg.GetSize())
        DCB.D=FillFraction(DCB.lHandle-xidLSegFillFrac);
      else if (DCB.lHandle>=xidLSegLen && DCB.lHandle<xidLSegLen+LiqSeg.GetSize())
        DCB.D=LSegLength(DCB.lHandle-xidLSegLen);
      else if (DCB.lHandle>=xidGSegLen && DCB.lHandle<xidGSegLen+GasSeg.GetSize())
        DCB.D=GSegLength(DCB.lHandle-xidGSegLen);
      return 1;
      }
    }
  return 0;
  }

// -------------------------------------------------------------------------

flag TwoPhasePipe::ValidateData(ValidateDataBlk & VDB)
  {
  if (bSimple)
    {
    return MN_Lnk::ValidateData(VDB);
    }

  flag OK=1;

  if (Valid(dLengthS))
    dLengthS = Range(10.0, dLengthS,10000.0);
  dLength  = Range(100.0, dLength, 10000.0);
  if (Valid(dLengthE))
    dLengthE = Range(10.0, dLengthE,10000.0);
  dDiam   = Range(0.001, dDiam  ,10.0);
  dRough  = Range(0.0,   dRough , 10.0);
  dAngleS = Range(-90.0, dAngleS ,90.0);
  dAngle  = Range(-90.0, dAngle ,90.0);
  dAngleE = Range(-90.0, dAngleE ,90.0);

  dArea=PI*Sqr(dDiam)/4.0;
  dVolume=dLength*dArea;
  dODiam=Max(dODiam, dDiam+0.001);

  Sect[0].dAngle=dAngleS;
  for (int SectNo=1; SectNo<UBSect(); SectNo++);
    {
    Sect[SectNo].dAngle = dAngle;
    }
  Sect[UBSect()].dAngle=dAngleE;

  SetNGSegments(Max(2, NGSegments()));
  double Pos2Date=-dLength;
  for (int sg=0; sg<NGSegments(); sg++)
    {
    TPPGSegment & GSg=*GasSeg[sg];
    GSg.dPosition=Max(Pos2Date+0.01, GSg.dPosition);
    Pos2Date=GSg.dPosition;
    if (GSg.pImg->QMass(som_Vap)<1.0e-3 && GSg.dMass>1.0e-3)
      GSg.pImg->QCopy(FlowImg);//.Model());
    }
  SetNLSegments(Max(2, NLSegments()));
  Pos2Date=-dLength;
  for (sg=0; sg<NLSegments(); sg++)
    {
    TPPLSegment & LSg=*LiqSeg[sg];
    LSg.dPosition=Max(Pos2Date+0.01, LSg.dPosition);
    Pos2Date=LSg.dPosition;
    if (LSg.pImg->QMass(som_Liq)<1.0e-3 && LSg.dMass>1.0e-3)
      LSg.pImg->QCopy(FlowImg);//.Model());
    }
  //EvaluateSegment(0.0);
  CalcPressures(0.0);
  DbgDump(0.0);

  return MdlNode::ValidateData(VDB) && OK;
  }

//--------------------------------------------------------------------------

flag TwoPhasePipe::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  //MdlAction {MAT_NULL, MAT_State, MAT_Value};
  MdlAction M0(0, MAT_State, !fDoSetLinePress, "Set Line Press", 1);
  MdlAction M1(1, MAT_State, !fDoSetLineSlugs, "Set Line Slugs", 1);

//  MdlAction M1 = {1, MAT_State, VPB.On(),  "Close", 0};
//  MdlAction M2 = {2, MAT_Value, VPB.On(),  "Manual Posn (%)", true, VPB.ManualPosition(this)*100, 0.0, 100.0};

  Acts.SetSize(0);
  Acts.SetAtGrow(0, M0);
  Acts.SetAtGrow(1, M1);
//  Acts.SetAtGrow(2, M2);
  return True;
  };

//--------------------------------------------------------------------------

flag TwoPhasePipe::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      fDoSetLinePress=true;
      break;
    case 1:
      fDoSetLineSlugs=true;
      break;
    }
  return true;
  };

// -------------------------------------------------------------------------

void TwoPhasePipe::PostConnect(int IONo)
  {
  MN_Lnk::PostConnect(IONo);
  FlwBlk &FB = *IOFB(IONo,0);
  pFlwEqn pEqn=FB.AssignFlwEqnGroup(PipeGroup, "FE_Line", this);
  if (typeid(*pEqn)!=typeid(FE_Linear))
    {
    DoBreak();
    }
  if (pEqn==NULL)
    {
    pFlwEqn pEqn=FB.AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
    DoBreak();
    }
  }

//---------------------------------------------------------------------------

void TwoPhasePipe::ConfigureJoins()
  {
  if (bSimple)
    {
    MN_Lnk::ConfigureJoins();
    return;
    }

  SetIO_Open(0, 0, false, ESS_Denied);
  SetIO_Open(1, 0, false, ESS_Denied);
  };

//--------------------------------------------------------------------------

void TwoPhasePipe::AddStdSegment(double Pos, double Len, double Press, double Temp, double VapFrac, TPPProps &Properties, SpConduit * Img)
  {
  double TotVolume=Len*dArea;
  double LiqMass=0.0;
  double GasMass=Properties.dNormGasDens*TotVolume*Press/Std_P*Std_T/Temp;
  for (int loop=0;loop<200;loop++)
    {
    double LiqVol=LiqMass/Properties.dLiqDens;
    if (LiqVol>0.9*TotVolume)
      {
      LiqVol=0.9*TotVolume;
      LiqMass=LiqVol*Properties.dLiqDens;
      }

#if PressCalc1
    GasMass=Properties.dNormGasDens*(TotVolume)*Press/Std_P*Std_T/Temp;
#else
    GasMass=Properties.dNormGasDens*(TotVolume-LiqVol)*Press/Std_P*Std_T/Temp;
#endif
    double Vf=GasMass/GTZ(GasMass+LiqMass);
    if (fabs(Vf-VapFrac)<0.00001)
      break;
    LiqMass=0.9*LiqMass+0.1*(1.0-VapFrac)*(GasMass+LiqMass);
    }

//  TPPGSegment *pG=new TPPGSegment(Pos, 0.0, GasMass, Temp, Properties, Img);
//  pG->iPos->SetTestValue(dLength);
//  GasSeg.Add(pG);
  int iG=GasSeg.GetSize();
  GasSeg.SetSize(iG+1);
  TPPGSegment *pG=GasSeg[iG];
  pG->dPosition=Pos;
  pG->dVelocity=0.0;
  pG->dMass=GasMass;
  pG->dTemp=Temp;
  pG->GProps=Properties;
  pG->dPress      = Std_P;
  pG->dPressGrad  = 0.0;
  pG->dLiqMass    = 0.0;
  pG->pImg->QCopy(*Img);
  pG->iPos->SetTestValue(dLength);


//  TPPLSegment *pL=new TPPLSegment(Pos, 0.0, LiqMass, Temp, Properties, Img);
//  pL->iPos->SetTestValue(dLength);
//  LiqSeg.Add(pL);

  int iL=LiqSeg.GetSize();
  LiqSeg.SetSize(iL+1);
  TPPLSegment *pL=LiqSeg[iL];
  pL->dPosition=Pos;
  pL->dVelocity=0.0;
  pL->dMass=GasMass;
  pL->dTemp=Temp;
  pL->LProps=Properties;
  //pL->dPress      = Std_P;
  //pL->dPressGrad  = 0.0;
  //pL->dLiqMass    = 0.0;
  pL->pImg->QCopy(*Img);
  pL->iPos->SetTestValue(dLength);



  SegsChanged();
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::SetLinePress()
  {
  SetNSections(NSections()); // To Get Positions Correct

  dRqdLinePress=Range(Std_P, dRqdLinePress, 25000.0);
  dRqdVapFrac=Range(0.5, dRqdVapFrac, 1.0);

  GasSeg.SetSize(0);
  LiqSeg.SetSize(0);
  SegsChanged();

  // Must Straddle the Whole Pipeline
  double Pos2Date=0.0;
  int iSect=0;
  int Done=0;
  while (!Done)//Pos2Date<=dLength-0.5*MeanSegLength())
    {
    double SegEnd=Min(dLength, Pos2Date+MeanSegLength());
    double SegLen=SegEnd-Pos2Date;
    if (dLength-SegEnd<0.1*SegLen)
      {
      Done=True;
      SegLen=dLength-Pos2Date;
      }
    AddStdSegment(Pos2Date, SegLen, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps, &FlowImg);//.Model());
    Pos2Date+=SegLen;
    iSect=SectIndex(Pos2Date);
    }
  double SegEnd=dLength;
  double SegLen=SegEnd-Pos2Date;
  //AddStdSegment(Pos2Date, Len, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps);
  // The Dummy one
  AddStdSegment(dLength, SegLen, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps, &FlowImg);//.Model());

  EvaluateSegment(0.0);
  CalcPressures(0.0);
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::SetLineSegs(flag DoSlugs)
  {
  dRqdVapFrac=Range(0.5, dRqdVapFrac, 1.0);

//  GasSeg.SetSize(0);
  LiqSeg.SetSize(0);
  int iNxtSlug=0;
  // Must Straddle the Whole Pipeline
  double Pos2Date=0.0;
  int iSect=0;
  int Done=0;
  double SpareLen=10.0;
  while (!Done && (Pos2Date < dLength-0.1))
    {
    double NxtSlugPos=iNxtSlug<InitSlug.GetSize() ? InitSlug[iNxtSlug].dPosition: dLength*2.0;
    if (NxtSlugPos<0.0 || InitSlug[iNxtSlug].dVolume<=0.001)
      NxtSlugPos=dLength*2.0;
    double SegEnd=Min(dLength, Pos2Date+MeanSegLength());
    flag DoSlug=DoSlugs && (NxtSlugPos<=SegEnd);
    if (DoSlug)
      SegEnd=NxtSlugPos;
    double SegLen=SegEnd-Pos2Date;
    if (dLength-SegEnd<0.1*Max(0.01*MeanSegLength(), SegLen))
      {
      Done=True;
      SegLen=dLength-Pos2Date;
      DoSlug=False;
      }

    if (SegLen<=SpareLen)
      break;

    int s=SectIndex(Pos2Date);
    TPPJoin &Jn0=*Join[Max(0,s-1)];
    TPPJoin &Jn1=*Join[Min(s,UBJoin())];
    double Press=0.5*(Jn0.dMeasP+Jn1.dMeasP);
    double Temp=0.5*(Jn0.dMeasT+Jn1.dMeasT);

//    AddStdSegment(Pos2Date, SegLen, Press, Temp,   dRqdVapFrac, FdProps, &FlowImg);//.Model());

    double TotVolume=SegLen*dArea;
    double LiqMass=(1.0-dRqdVapFrac)*TotVolume*FdProps.dLiqDens;

    TPPLSegment *pL=new TPPLSegment(Pos2Date, 0.0, LiqMass, dRqdLineTemp, FdProps, &FlowImg);
    pL->iPos->SetTestValue(dLength);
    LiqSeg.Add(pL);

    Pos2Date+=SegLen;
    if (DoSlug)
      {
      double SlugS=InitSlug[iNxtSlug].dPosition;
      double LiqVolume=InitSlug[iNxtSlug].dVolume;
      double LiqMass=FdProps.dLiqDens*LiqVolume;
      double LLen=LiqVolume/dArea/0.95;
      double MaxLiqLen=dLength-Pos2Date;//-SpareLen;
      if (LLen>MaxLiqLen)
        {
        LiqMass*=MaxLiqLen/LLen;
        LLen=MaxLiqLen;
        }
      // Add Line 5 % of length
      //AddStdSegment(Pos2Date, 1.05*LLen, Press, Temp,   0.01, FdProps, &FlowImg);//.Model());

      TPPLSegment *pL=new TPPLSegment(Pos2Date, 0.0, LiqMass, dRqdLineTemp, FdProps, &FlowImg);
      pL->iPos->SetTestValue(dLength);
      LiqSeg.Add(pL);

      // Stretch them and add extra liquid
      Pos2Date+=LLen;
      //LiqSeg[UBLSeg()]->dMass+=LiqMass;

      iNxtSlug++;
      }
    iSect=SectIndex(Pos2Date);
    }
  double SegEnd=dLength;
  double SegLen=SegEnd-Pos2Date;
  //AddStdSegment(Pos2Date, Len, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps);
  // The Dummy one
//  AddStdSegment(dLength, SegLen, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps, &FlowImg);//.Model());
  TPPLSegment *pL=new TPPLSegment(dLength, SegLen, 0.0, dRqdLineTemp, FdProps, &FlowImg);
  pL->iPos->SetTestValue(dLength);
  LiqSeg.Add(pL);

  EvaluateSegment(0.0);
  CalcPressures(0.0);

  SegsChanged();
  }

//--------------------------------------------------------------------------

//void TwoPhasePipe::SetLineSegs(flag DoSlugs)
//  {
//  dRqdVapFrac=Range(0.5, dRqdVapFrac, 1.0);
//
//  GasSeg.SetSize(0);
//  LiqSeg.SetSize(0);
//  SegsChanged();
//  int iNxtSlug=0;
//  // Must Straddle the Whole Pipeline
//  double Pos2Date=0.0;
//  int iSect=0;
//  int Done=0;
//  while (!Done)//Pos2Date<=dLength-0.5*MeanSegLength())
//    {
//    double NxtSlugPos=iNxtSlug<InitSlug.GetSize() ? InitSlug[iNxtSlug].dPosition: dLength*2.0;
//    if (NxtSlugPos<0.0 || InitSlug[iNxtSlug].dVolume<=0.001)
//      NxtSlugPos=dLength*2.0;
//    double SegEnd=Min(dLength, Pos2Date+MeanSegLength());
//    flag DoSlug=DoSlugs && (NxtSlugPos<=SegEnd);
//    if (DoSlug)
//      SegEnd=NxtSlugPos;
//    double SegLen=SegEnd-Pos2Date;
//    if (dLength-SegEnd<0.1*SegLen)
//      {
//      Done=True;
//      SegLen=dLength-Pos2Date;
//      DoSlug=False;
//      }
//
//    int s=SectIndex(Pos2Date);
//    TPPJoin &Jn0=*Join[Max(0,s-1)];
//    TPPJoin &Jn1=*Join[Min(s,UBJoin())];
//    double Press=0.5*(Jn0.dMeasP+Jn1.dMeasP);
//    double Temp=0.5*(Jn0.dMeasT+Jn1.dMeasT);
//
//    AddStdSegment(Pos2Date, SegLen, Press, Temp,   dRqdVapFrac, FdProps, &FlowImg);//.Model());
//    Pos2Date+=SegLen;
//    if (DoSlug)
//      {
//      double SlugS=InitSlug[iNxtSlug].dPosition;
//      double LiqVolume=InitSlug[iNxtSlug].dVolume;
//      double LiqMass=FdProps.dLiqDens*LiqVolume;
//      double LLen=LiqVolume/dArea;
//      // Add Line 5 % of length
//      AddStdSegment(Pos2Date, 1.05*LLen, Press, Temp,   0.01, FdProps, &FlowImg);//.Model());
//      // Stretch them and add extra liquid
//      Pos2Date+=1.05*LLen;
//      //LiqSeg[UBLSeg()]->dMass+=LiqMass;
//
//      iNxtSlug++;
//      }
//    iSect=SectIndex(Pos2Date);
//    }
//  double SegEnd=dLength;
//  double SegLen=SegEnd-Pos2Date;
//  //AddStdSegment(Pos2Date, Len, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps);
//  // The Dummy one
//  AddStdSegment(dLength, SegLen, dRqdLinePress, dRqdLineTemp, dRqdVapFrac, FdProps, &FlowImg);//.Model());
//
//  EvaluateSegment(0.0);
//  CalcPressures(0.0);
//  }
//
//--------------------------------------------------------------------------

int TwoPhasePipe::GSegIndex(double Pos)
  {
  if (Pos>0.5*dLength)
    {
    int Index=NFullGSegs()-1;
    while ((Index>0) && (GasSeg[Index]->dPosition >= Pos))
      Index--;
    return Index;
    }
  else
    {
    int Index=0;
    while ((Index<NFullGSegs()-1) && (GasSeg[Index+1]->dPosition < Pos))
      Index++;
    return Index;
    }
  }

//--------------------------------------------------------------------------

int TwoPhasePipe::LSegIndex(double Pos)
  {
  if (Pos>0.5*dLength)
    {
    int Index=NFullLSegs()-1;
    while ((Index>0) && (LiqSeg[Index]->dPosition >= Pos))
      Index--;
    return Index;
    }
  else
    {
    int Index=0;
    while ((Index<NFullLSegs()-1) && (LiqSeg[Index+1]->dPosition < Pos))
      Index++;
    return Index;
    }
  }

//--------------------------------------------------------------------------

int TwoPhasePipe::SectIndex(double Pos)
  {
  if (Pos<=SectionEnd(0))
    return 0;
  double L=SectionLength(1);
  return Range(1, (int)(1.0+GEZ(Pos-SectionStart(1))/L), UBSect());
  }

//--------------------------------------------------------------------------

int TwoPhasePipe::JoinIndex(double Pos)
  {
  if (Pos<=JoinEnd(0))
    return 0;
  double L=JoinLength(1);
  return Range(1, (int)(1.0+GEZ(Pos-JoinStart(1))/L), UBJoin());
  }

//--------------------------------------------------------------------------

double TwoPhasePipe::GVelocity(double Pos)
  {
  int Index=Range(0, GSegIndex(Pos), NFullGSegs()-2);
  double Vel0=GSeg(Index).dGasVelo;
  double Vel1=GSeg(Index+1).dGasVelo;
  double L0=GSegStart(Index);
  double L1=GSegStart(Index+1);
  return (Vel0+(Vel1-Vel0)*(Pos-L0)/(L1-L0));
  }

//--------------------------------------------------------------------------

double TwoPhasePipe::LVelocity(double Pos)
  {
  int Index=Range(0, LSegIndex(Pos), NFullLSegs()-2);
  double Vel0=LSeg(Index).dLiqVelo;
  double Vel1=LSeg(Index+1).dLiqVelo;
  double L0=LSegStart(Index);
  double L1=LSegStart(Index+1);
  return (Vel0+(Vel1-Vel0)*(Pos-L0)/(L1-L0));
  }

//--------------------------------------------------------------------------

double TwoPhasePipe::Pressure(double Pos)
  {
  for (int Index=0; Index<NFullGSegs()-2; Index++)
    if (GSegMid(Index+1)>Pos)
      break;

  double Prs0=GasSeg[Index]->dPress;
  double Prs1=GasSeg[Index+1]->dPress;
  double L0=GSegMid(Index);
  double L1=GSegMid(Index+1);
  return Prs0+(Prs1-Prs0)*(Pos-L0)/(L1-L0);
  }

//--------------------------------------------------------------------------

double TwoPhasePipe::Temperature(double Pos)
  {
  for (int Index=0; Index<NFullGSegs()-2; Index++)
    if (GSegMid(Index+1)>Pos)
      break;

  double Tmp0=GasSeg[Index]->dTemp;
  double Tmp1=GasSeg[Index+1]->dTemp;
  double L0=GSegMid(Index);
  double L1=GSegMid(Index+1);
  return Tmp0+(Tmp1-Tmp0)*(Pos-L0)/(L1-L0);
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::AddMass(double &GMass0, double &Temp0, TPPGasProps &Props0, double GMass1, double Temp1, TPPGasProps & Props1)
  {
  double Cp0=GMass0*Props0.dGasCp; // kJ/C
  double Cp1=GMass1*Props1.dGasCp; // kJ/C
  Props0.Sum(Props1, GMass1);
  Temp0=(Temp0*Cp0+Temp1*Cp1)/GTZ(Cp0+Cp1);
  GMass0+=GEZ(GMass1);
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::AddMass(double &LMass0, double &Temp0, TPPLiqProps & Props0, double LMass1, double Temp1, TPPLiqProps & Props1)
  {
  double Cp0=LMass0*Props0.dLiqCp; // kJ/C
  double Cp1=LMass1*Props1.dLiqCp; // kJ/C
  Props0.Sum(Props1, LMass1);
  Temp0=(Temp0*Cp0+Temp1*Cp1)/GTZ(Cp0+Cp1);
  LMass0+=GEZ(LMass1);

  }

//--------------------------------------------------------------------------

void TwoPhasePipe::AddMass(double &GMass0, double &LMass0, double &Temp0, TPPProps & Props0, double GMass1, double LMass1, double Temp1, TPPProps & Props1)
  {
  double Cp0=GMass0*Props0.dGasCp+LMass0*Props0.dLiqCp; // kJ/C
  double Cp1=GMass1*Props1.dGasCp+LMass1*Props1.dLiqCp; // kJ/C
  Props0.Sum(Props1, GMass1, LMass1);
  Temp0=(Temp0*Cp0+Temp1*Cp1)/GTZ(Cp0+Cp1);
  LMass0+=GEZ(LMass1);
  GMass0+=GEZ(GMass1);
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::AddSweptMass(flag AtStart, double dTime, double &GasMass, double &LiqMass, double &MixTemp)
  {
  double TotalCp=0.0;
  double SweepS, SweepE;
  if (AtStart)
    {
    // Flow is (+) into pipe
    double Vel=IOFlw[0].dQvG/GTZ((1-IOFlw[0].sLiqVolFrac)*Area());
    SweepS=0.0;
    SweepE=GEZ(-Vel*dTime);
    }
  else
    {
    // Flow is (+) into pipe
    double Vel=IOFlw[1].dQvG/GTZ((1-IOFlw[1].sLiqVolFrac)*Area());
    double SweepL=GEZ(Vel*dTime);
    SweepS=dLength-SweepL;
    SweepE=dLength;
    }

  for (int sg=0; sg<NFullGSegs(); sg++)
    {
    TPPGSegment &Sg=*GasSeg[sg];
    double SegS=GSegStart(sg);
    double SegE=GSegEnd(sg);
    double SegL=SegE-SegS;
    if (SegE<SweepS)
      continue;
    if (SegS>SweepE)
      break;
    double Portion=CalcPortion(SegS, SegE, SweepS, SweepE);

    double Gm=Portion*Sg.dMass;
    double GasCp=Gm*FdProps.dGasCp;
    double FdTemp=Sg.Temp();
    MixTemp=(MixTemp*TotalCp+FdTemp*GasCp)/GTZ(TotalCp+GasCp);
    GasMass+=Gm;
    TotalCp+=GasCp;
    }

  if (AtStart)
    {
    double Vel=IOFlw[0].dQvL/GTZ((IOFlw[0].sLiqVolFrac)*Area());
    SweepS=0.0;
    SweepE=GEZ(-Vel*dTime);
    }
  else
    {
    double Vel=IOFlw[1].dQvL/GTZ((IOFlw[1].sLiqVolFrac)*Area());
    double SweepL=GEZ(Vel*dTime);
    SweepS=dLength-SweepL;
    SweepE=dLength;
    }

  for (sg=0; sg<NFullLSegs(); sg++)
    {
    TPPLSegment &Sg=*LiqSeg[sg];
    double SegS=LSegStart(sg);
    double SegE=LSegEnd(sg);
    double SegL=SegE-SegS;
    if (SegE<SweepS)
      continue;
    if (SegS>SweepE)
      break;
    double Portion=CalcPortion(SegS, SegE, SweepS, SweepE);

    double Lm=Portion*Sg.dMass;
    double LiqCp=Lm*FdProps.dLiqCp; // kJ/C
    double FdTemp=Sg.Temp();
    MixTemp=(MixTemp*TotalCp+FdTemp*LiqCp)/GTZ(TotalCp+LiqCp);
    LiqMass+=Lm;
    TotalCp+=LiqCp;
    }
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::LiqMassInSegs(double SegS, double SegE, double &LiqMass, double &LiqTemp, TPPLiqProps & LProps)
  {

  //double MxTemp=Std_T;
  //double GasMass=0.0;
  //LiqMass=0.0;
  //LProps.InitSum();
  //LProps).Sum(Sg.GProps, GasMass);

  int lsg=0;
  for (;;)
    {
    TPPLSegment & LSg=*LiqSeg[lsg];

    double LSegS=LSegStart(lsg);
    double LSegE=LSegEnd(lsg);
    double Portion=CalcPortion(LSegS, LSegE, SegS, SegE);
    double Lm=Portion*LSg.dMass;
//    TPPProps LProps;
//    LProps=LSg.LProps;

    AddMass(LiqMass, LiqTemp, LProps, Lm, LSg.dTemp, LSg.LProps);

    if (LSegE>=SegE || lsg>=LastFullLSeg())
      break;
    lsg++;
    }

//    double LVol=GEZ(LiqMass)/Sg.MxProps.dLiqDens;
//    double GVol=Max(1.0e-6, GSegLength(sg)*dArea-LVol);
//
//    Sg.dVapFrac=GEZ(GasMass)/GTZ(LiqMass+GasMass);
//    Sg.dLiqMass=LiqMass;
//    Sg.GProps.dGasDens=GEZ(GasMass)/GTZ(GVol);
//    // Not Good
//    Sg.MxProps.dGasDens=Sg.GProps.dGasDens;
//    Sg.dPress=Sg.GProps.dGasDens/GTZ(Sg.GProps.dNormGasDens)*Std_P*MxTemp /*Sg.dTemp*/ /Std_T;
  }
//--------------------------------------------------------------------------

void TwoPhasePipe::CalcPressures(double dTime)
  {
  #if dbgPipeline
  if (dbgCalcPress() && fDoDbgBrk)
    dbgpln("CalcPress %s", FullObjTag());
  #endif
  if (NoFlwIOs()==0)
    return;

  #if dbgPipeline
  if (dbgPosition() && fDoDbgBrk)
    {
    for (int sg=0; sg<NFullGSegs(); sg++)
      dbgpln("GSeg %3i P:%14.6g M:%14.6g T:%14.6g L:%14.6g", sg, GasSeg[sg]->dPosition, GasSeg[sg]->dMass, GasSeg[sg]->dTemp, GSegLength(sg));
    for (sg=0; sg<NFullLSegs(); sg++)
      dbgpln("LSeg %3i P:%14.6g M:%14.6g T:%14.6g L:%14.6g", sg, LiqSeg[sg]->dPosition, LiqSeg[sg]->dMass, LiqSeg[sg]->dTemp, LSegLength(sg));
    }
  #endif

  // Init Section & Joins
  int lsg=0;
  for (int sg=0; sg<NFullGSegs(); sg++)
    {
    TPPGSegment &Sg=*GasSeg[sg];

    double GasMass=Sg.dMass;
    double MxTemp=Sg.dTemp;
    double LiqMass=0.0;
    //double LiqTemp=Std_T;
    Sg.MxProps.InitSum();
    Sg.MxProps.GP().Sum(Sg.GProps, GasMass);

    double GSegS=GSegStart(sg);
    double GSegE=GSegEnd(sg);

    for (;;)
      {
      TPPLSegment & LSg=*LiqSeg[lsg];

      double LSegS=LSegStart(lsg);
      double LSegE=LSegEnd(lsg);
      double Portion=CalcPortion(LSegS, LSegE, GSegS, GSegE);
      double Lm=Portion*LSg.dMass;
      TPPProps LProps;
      LProps=LSg.LProps;

      AddMass(GasMass, LiqMass, MxTemp, Sg.MxProps, 0.0, Lm, LSg.dTemp, LProps);

      if (LSegE>=GSegE || lsg>=LastFullLSeg())
        break;
      lsg++;
      }


    double LVol=GEZ(LiqMass)/Sg.MxProps.dLiqDens;
#if PressCalc1
    double GVol=Max(1.0e-6, GSegLength(sg)*dArea);
#else
    double GVol=Max(1.0e-6, GSegLength(sg)*dArea-LVol);
#endif
    Sg.dVapFrac=GEZ(GasMass)/GTZ(LiqMass+GasMass);
    Sg.dLiqMass=LiqMass;
    Sg.GProps.dGasDens=Max(1.0e-3, GEZ(GasMass)/GTZ(GVol));
    // Not Good
    Sg.MxProps.dGasDens=Sg.GProps.dGasDens;
    Sg.dPress=Sg.GProps.dGasDens/GTZ(Sg.GProps.dNormGasDens)*Std_P*MxTemp /*Sg.dTemp*/ /Std_T;
    if (sg==0)
      IOFlw[0].dVapFrac=Sg.dVapFrac;
    else if (sg==LastFullGSeg())
      IOFlw[1].dVapFrac=Sg.dVapFrac;

    if (sg==0)
      Sg.dPressGrad=0.0;
    else
      {
      TPPGSegment &Sg0=*GasSeg[sg-1];
      double Len=GSegMid(sg)-GSegMid(sg-1);
      Sg.dPressGrad=(Sg0.dPress-Sg.dPress)/GTZ(Len);
      if (fabs(Sg.dPressGrad)>1.0e6)
        {
        int xxx=0;
        }
      }

    #if dbgPipeline
    if (dbgCalcPress() && fDoDbgBrk)
      dbgpln("Sg %3i %3i : T:%12.3f Gm:%14.6f Lm:%14.6f Gv:%14.6f Lv:%14.6f Vf:%14.6f Gd:%14.6f P:%14.6f Pgr:%14.6f",
             sg,lsg, Sg.dTemp, GasMass, LiqMass, GVol, LVol,
             Sg.dVapFrac, Sg.GProps.dGasDens, Sg.dPress, Sg.dPressGrad);
    #endif

    }
  // Init Section & Joins
  int gsg=0;
  for (sg=0; sg<NFullLSegs(); sg++)
    {
    TPPLSegment &Sg=*LiqSeg[sg];

    double LiqMass=Sg.dMass;
    double MxTemp=Sg.dTemp;
    double GasMass=0.0;
    //double LiqTemp=Std_T;
    Sg.MxProps.InitSum();
    Sg.MxProps.LP().Sum(Sg.LProps, LiqMass);

    double LSegS=LSegStart(sg);
    double LSegE=LSegEnd(sg);

    for (;;)
      {
      TPPGSegment & GSg=*GasSeg[gsg];

      double GSegS=GSegStart(gsg);
      double GSegE=GSegEnd(gsg);
      double Portion=CalcPortion(GSegS, GSegE, LSegS, LSegE);
      double Gm=Portion*GSg.dMass;
      TPPProps GProps;
      GProps=GSg.GProps;

      AddMass(GasMass, LiqMass, MxTemp, Sg.MxProps, Gm, 0.0, GSg.dTemp, GProps);

      if (GSegE>=LSegE || gsg>=LastFullGSeg())
        break;
      gsg++;
      }


    double LVol=GEZ(LiqMass)/Sg.MxProps.dLiqDens;
#if PressCalc1
    double GVol=Max(1.0e-6, LSegLength(sg)*dArea);
#else
    double GVol=Max(1.0e-6, LSegLength(sg)*dArea-LVol);
#endif

    Sg.dVapFrac=GEZ(GasMass)/GTZ(LiqMass+GasMass);
    Sg.dGasMass=GasMass;
    //Sg.GProps.dGasDens=GEZ(GasMass)/GTZ(GVol);
    // Not Good
    Sg.MxProps.dGasDens=Max(1.0e-3, GEZ(GasMass)/GTZ(GVol));//GProps.dGasDens;
    //Sg.dPress=Sg.GProps.dGasDens/GTZ(Sg.GProps.dNormGasDens)*Std_P*MxTemp /*Sg.dTemp*/ /Std_T;
//    if (sg==0)
//      IOFlw[0].dVapFrac=Sg.dVapFrac;
//    else if (sg==LastFullGSeg())
//      IOFlw[1].dVapFrac=Sg.dVapFrac;

//    if (sg==0)
//      Sg.dPressGrad=0.0;
//    else
//      {
//      TPPGSegment &Sg0=*GasSeg[sg-1];
//      double Len=GSegMid(sg)-GSegMid(sg-1);
//      Sg.dPressGrad=(Sg0.dPress-Sg.dPress)/GTZ(Len);
//      if (fabs(Sg.dPressGrad)>1.0e6)
//        {
//        int xxx=0;
//        }
//      }

    #if dbgPipeline
    if (dbgCalcPress() && fDoDbgBrk)
      dbgpln("Sg %3i %3i : T:%12.3f Gm:%14.6f Lm:%14.6f Gv:%14.6f Lv:%14.6f Vf:%14.6f",
             sg,gsg, Sg.dTemp, GasMass, LiqMass, GVol, LVol,
             Sg.dVapFrac);//, Sg.GProps.dGasDens);
    #endif

    }

  int GIndex=1;
  int LIndex=1;
  for (int j=0; j<NJoins(); j++)
    {
    double  Pos=JoinPos(j);

    while (GIndex<LastFullGSeg() && GSegMid(GIndex)<Pos)
      GIndex++;
    while (LIndex<LastFullLSeg() && LSegMid(LIndex)<Pos)
      LIndex++;

    double L0=GSegMid(GIndex-1);
    double L1=GSegMid(GIndex);

    double Prs0=GasSeg[GIndex-1]->dPress;
    double Prs1=GasSeg[GIndex]->dPress;
    Join[j]->dMeasP=Prs0+(Prs1-Prs0)*(Pos-L0)/(L1-L0);

    double Tmp0=GasSeg[GIndex-1]->dTemp;
    double Tmp1=GasSeg[GIndex]->dTemp;
    Join[j]->dMeasT=Tmp0+(Tmp1-Tmp0)*(Pos-L0)/(L1-L0);

    double GVel0=(GIndex>1) ? GasSeg[GIndex-1]->dGasVelo : IOFlw[0].dGasVelo;
    double GVel1=GasSeg[GIndex]->dGasVelo;
    Join[j]->dMeasGasVel=GVel0+(GVel1-GVel0)*(Pos-L0)/(L1-L0);

    double LVel0=(LIndex>1) ? LiqSeg[LIndex-1]->dLiqVelo : IOFlw[0].dLiqVelo;
    double LVel1=LiqSeg[LIndex]->dLiqVelo;
    Join[j]->dMeasLiqVel=LVel0+(LVel1-LVel0)*(Pos-L0)/(L1-L0);

    Join[j]->bMeasHorFlowRegime=GasSeg[GIndex]->bHorFlowRegime;
    Join[j]->bMeasFlowRegime=GasSeg[GIndex]->bFlowRegime;
    }
  };

//--------------------------------------------------------------------------

void TwoPhasePipe::CreateSlug(double SlugPos, double SlugVol)
  {
  // Find Segment
  // NB Two slugs cannot adjoin each other
  for (int sg=1; sg<NFullLSegs()-1; sg++)
    {
    TPPLSegment &Sg=*LiqSeg[sg];
    if (LSegEnd(sg)>SlugPos)
      {
      if (!IsSlug(sg))
        {
        double SegS=LSegStart(sg);
        double SegE=LSegEnd(sg);
        double SegL=SegE-SegS;
        if (SegL>0.5*MeanSegLength())
          {
          double SegLMass=Sg.dMass;
          double SlugMass=SlugVol*Sg.LProps.dLiqDens;
          SlugMass=Min(0.5*SegLMass, Min(SlugVol, SegL*dArea)*Sg.LProps.dLiqDens);
          SlugVol=1.05*SlugMass/Sg.LProps.dLiqDens;
          double SlugLen=SlugVol/dArea;
          //if (SegL>3.0*SlugLen)
            {
            // Must keep Gas between Slugs
            if (!IsSlug(sg-1) && SlugPos<SegS+0.33*SegL)
              {
              SlugPos=SegS;
              TPPLSegment *pL=new TPPLSegment(SlugPos, Sg.dVelocity, SlugMass, Sg.dTemp, Sg.LProps, Sg.pImg);
              pL->iPos->SetTestValue(dLength);
              LiqSeg.InsertAt(sg, pL);
              SegsChanged();
              Sg.dPosition+=SlugLen;
              Sg.dMass-=SlugMass;
              }
            else if (IsSlug(sg+1) && SlugPos>SegS+0.67*SegL)
              {
              if ((sg+1)<NFullLSegs() && !IsSlug(sg+1))
                {
                SlugPos=SegE-SlugLen;
                TPPLSegment *pL=new TPPLSegment(SlugPos, Sg.dVelocity, SlugMass, Sg.dTemp, Sg.LProps, Sg.pImg);
                pL->iPos->SetTestValue(dLength);
                LiqSeg.InsertAt(sg+1, pL);
                SegsChanged();
                Sg.dMass-=SlugMass;
                }
              }
            else
              {
              // Split This Segment
              double HalfSegLMass=0.5*(SegLMass-SlugMass);
              double HalfSegLen=0.5*(SegL-SlugLen);
              double Seg0Pos=Sg.dPosition+HalfSegLen;
              double Seg1Pos=Seg0Pos+SlugLen;
              TPPLSegment *pL0=new TPPLSegment(Seg0Pos, Sg.dVelocity, SlugMass, Sg.dTemp, Sg.LProps, Sg.pImg);
              TPPLSegment *pL1=new TPPLSegment(Seg1Pos, Sg.dVelocity, HalfSegLMass, Sg.dTemp, Sg.LProps, Sg.pImg);

              pL0->iPos->SetTestValue(dLength);
              pL1->iPos->SetTestValue(dLength);
              LiqSeg.InsertAt(sg+1, pL0);
              LiqSeg.InsertAt(sg+2, pL1);
              SegsChanged();
              Sg.dMass=HalfSegLMass;
              }
            }
          }
        }
      break;
      }
    }
  }

//--------------------------------------------------------------------------

int TwoPhasePipe::RemoveSlug(int iSlug)
  {

  TPPLSegment &Sg=*LiqSeg[iSlug];
  double SlugLen  = LSegLength(iSlug);
  double LostMass = dSlugReduction*Sg.dMass;
  double LostLen  = dSlugReduction*SlugLen;

  if (iSlug>0 && (SlugLen-LostLen<dSlugMinLen))
    {
    // kill this slug
    TPPLSegment &Prv=*LiqSeg[iSlug-1];
    Prv.dMass+=Sg.dMass;
    Prv.LProps.InitSum(Prv.dMass);
    Prv.LProps.Sum(Sg.LProps, Sg.dMass);

    // Assume Props OK && TempOK
    delete LiqSeg[iSlug];
    LiqSeg.RemoveAt(iSlug);
    SegsChanged();
    return True;
    }
  else
    {
    // reduce slug len
    TPPLSegment &Prv=*LiqSeg[iSlug-1];
    Prv.dMass+=LostMass;
    Sg.dMass-=LostMass;
    Sg.dPosition+=LostLen;
    }
  return False;
  }

//--------------------------------------------------------------------------

int TwoPhasePipe::CombineLSegs(int iSeg0, int iSeg1)
  {

  TPPLSegment &Sg0=*LiqSeg[iSeg0];
  TPPLSegment &Sg1=*LiqSeg[iSeg1];

  // kill this slug
  //Sg0.dMass+=Sg1.dMass;
  AddMass(Sg0.dMass, Sg0.dTemp, Sg0.LProps, Sg1.dMass, Sg1.dTemp, Sg1.LProps);
  Sg0.LProps.InitSum(Sg0.dMass);
  Sg0.LProps.Sum(Sg1.LProps, Sg1.dMass);

  delete LiqSeg[iSeg1];
  LiqSeg.RemoveAt(iSeg1);
  SegsChanged();
  return True;
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::EvaluateSegment(double dTime)
  {
  //Add New Existing slugs around
  if (NGSegments()<2)
    return;
  #if dbgPipeline
  if (dbgAddSegs() && fDoDbgBrk)
    dbgpln("EvaluateSegment : ========%14.2g %s", dTime, FullObjTag());
  #endif

  for (int sg=0; sg<NGSegments(); sg++)
    GasSeg[sg]->iAge++;
  for (sg=0; sg<NLSegments(); sg++)
    LiqSeg[sg]->iAge++;

  if (1)
    {
    for (int iG=0; iG<NGSegments()-1; iG++)
      {
      if (GSegLength(iG)<SegMergeFactor*MeanSegLength())
        {
        int xxx=0;
        }
      }
    for (int iL=0; iL<NLSegments()-1; iL++)
      {
      if (LSegLength(iL)<0.1*SegMergeFactor*MeanSegLength())
        {
        double lll=LSegLength(iL);
        TPPLSegment &L=LSeg(iL);
        int xxx=0;
        }
      }
    }

  // Remove short segments at ends / merge at Start
  SetGSegStart(0, 0.0);
  while (NGSegments()>MinNSegs && GSegLength(0)<SegMergeFactor*MeanSegLength())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Remove Gas Seg %i", 1);
    #endif
    TPPGSegment &SgRemove =*GasSeg[0];
    TPPGSegment &SgKeep =*GasSeg[1];
    AddMass(SgKeep.dMass, SgKeep.dTemp, SgKeep.GProps, SgRemove.dMass, SgRemove.dTemp, SgRemove.GProps);
    delete GasSeg[0];
    GasSeg.RemoveAt(0);
    SegsChanged();
    }
  while (LSegEnd(0)<0.0)
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Remove Liq Seg %i", 1);
    #endif
    delete LiqSeg[0];
    LiqSeg.RemoveAt(0);
    SegsChanged();
    }
  while (LSegStart(NLSegments()-1)>Length())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Remove Liq Seg %i", 1);
    #endif
    delete LiqSeg[NLSegments()-1];
    LiqSeg.RemoveAt(NLSegments()-1);
    SegsChanged();
    }

  SetLSegStart(0, 0.0);
  SetLSegStart(NLSegments()-1, Length());
  while (NLSegments()>MinNSegs && LSegLength(0)<0.1*SegMergeFactor*MeanSegLength())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Remove Liq Seg %i", 1);
    #endif
    TPPLSegment &SgRemove =*LiqSeg[0];
    TPPLSegment &SgKeep =*LiqSeg[1];
    AddMass(SgKeep.dMass, SgKeep.dTemp, SgKeep.LProps, SgKeep.dMass*Range(0., LSegLength(0)/LSegLength(1), 1.0), SgRemove.dTemp, SgRemove.LProps);
    delete LiqSeg[0];
    LiqSeg.RemoveAt(0);
    SegsChanged();
    }

  // Remove short segments at ends / merge at Exit
  while (NGSegments()>MinNSegs && GSegLength(LastFullGSeg())<SegMergeFactor*MeanSegLength())
    {
    int iLFGSg = LastFullGSeg();
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Remove Gas Seg %i L:%8.2f Msl:%8.2f", iLFGSg, GSegLength(LastFullGSeg()), MeanSegLength());
    #endif
    TPPGSegment &SgKeep =*GasSeg[iLFGSg-1];
    TPPGSegment &SgRemove =*GasSeg[iLFGSg];
    AddMass(SgKeep.dMass, SgKeep.dTemp, SgKeep.GProps, SgRemove.dMass, SgRemove.dTemp, SgRemove.GProps);
    delete GasSeg[iLFGSg];
    GasSeg.RemoveAt(iLFGSg);
    SegsChanged();
    }

  while (NLSegments()>MinNSegs && LSegLength(LastFullLSeg())<0.1*SegMergeFactor*MeanSegLength())
    {
    int iLFLSg=LastFullLSeg();
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Remove Liq Seg %i L:%8.2f Msl:%8.2f", iLFLSg, LSegLength(iLFLSg), MeanSegLength());
    #endif
    TPPLSegment &SgKeep=*LiqSeg[iLFLSg-1];
    TPPLSegment &SgRemove=*LiqSeg[iLFLSg];
    AddMass(SgKeep.dMass, SgKeep.dTemp, SgKeep.LProps, SgKeep.dMass*Range(0., LSegLength(iLFLSg)/LSegLength(iLFLSg-1), 1.0), SgRemove.dTemp, SgRemove.LProps);
    delete LiqSeg[iLFLSg];
    LiqSeg.RemoveAt(iLFLSg);
    SegsChanged();
    }

  if (GSegLength(0) > SegSplitFactor * MeanSegLength())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Add GasSeg Start L:%8.2f Msl:%8.2f", GSegLength(0), MeanSegLength());
    #endif
    TPPLiqProps LProps0;
    TPPLiqProps LProps1;

    double LiqMass0=0.0;
    double LiqTemp0=0.0;
    double LiqMass1=0.0;
    double LiqTemp1=0.0;
    LiqMassInSegs(GSegStart(0), GSegMid(0), LiqMass0, LiqTemp0, LProps0);
    LiqMassInSegs(GSegMid(0), GSegEnd(0), LiqMass1, LiqTemp1, LProps1);

    double LiqVol0=LiqMass0/GTZ(LProps0.dLiqDens);
    double LiqVol1=LiqMass1/GTZ(LProps1.dLiqDens);

#if PressCalc1
    double GasVol0=0.5*GSegLength(0)*dArea;
    double GasVol1=0.5*GSegLength(0)*dArea;
#else
    double GasVol0=0.5*GSegLength(0)*dArea-LiqVol0;
    double GasVol1=0.5*GSegLength(0)*dArea-LiqVol1;
#endif
    double GasVolM=0.5*(GasVol0+GasVol1);

    TPPGSegment &Sg0 =*GasSeg[0];
    TPPGSegment &Sg1 =*GasSeg[1];
    // Add new segment
    double Scl0, Scl1;
    double TM0 = Temperature(0.25 * GSegEnd(0));
    double TMM = Temperature(0.50 * GSegEnd(0));
    double TM1 = Temperature(0.75 * GSegEnd(0));

    double PM0 = Pressure(0.25 * GSegEnd(0));
    double PMM = Pressure(0.50 * GSegEnd(0));
    double PM1 = Pressure(0.75 * GSegEnd(0));

    double X0 = PM0/GTZ(TM0);
    double X1 = PM1/GTZ(TM1);

    Scl0=X0/GTZ(X0+X1)*GasVol0/GTZ(GasVolM);

    //Scl0 = 0.5 * PM0/GTZ(PMM)*TMM/GTZ(TM0)*GasVol0/GTZ(GasVolM);
    Scl1 = 1.0 - Scl0;

    TPPGSegment *pG = new TPPGSegment(0.0, 0.0/*Sg0.Velocity()*/, Scl0*Sg0.dMass, TM0, Sg0.GProps, Sg0.pImg);
    TPPGSegment &SgN=*pG;

    SgN.iPos->SetTestValue(dLength);
    Sg0.dMass*=Scl1;
    Sg0.dTemp=TM1;
    Sg0.SetPosition(0.5*GSegLength(0));
    // should be Assignment operator
    Sg0.bHorFlowRegime    = Sg1.bHorFlowRegime;
    Sg0.bFlowRegime       = Sg1.bFlowRegime;
//    Sg0.dQmRng            = Sg1.dQmRng;
    Sg0.dQmEst            = Sg1.dQmEst;
    Sg0.dQmG              = Sg1.dQmG;
    Sg0.dQmL              = Sg1.dQmL;
    Sg0.dQvG              = Sg1.dQvG;
    Sg0.dQvL              = Sg1.dQvL;
    Sg0.dVapFrac          = Sg1.dVapFrac;
    Sg0.sLiqVolFrac       = Sg1.sLiqVolFrac;
    Sg0.dDarcyCorrection  = Sg1.dDarcyCorrection;
    Sg0.dSupGasVel        = Sg1.dSupGasVel;
    Sg0.dSupLiqVel        = Sg1.dSupLiqVel;
    Sg0.dTotalVel         = Sg1.dTotalVel;
    Sg0.dDPq              = Sg1.dDPq;
    Sg0.dDPqDq            = Sg1.dDPqDq;
    Sg0.dDPmB             = Sg1.dDPmB;
    Sg0.dDPmQ             = Sg1.dDPmQ;
    Sg0.dDPmDq            = Sg1.dDPmDq;
    Sg0.dDPApplied        = Sg1.dDPApplied;
    Sg0.dDPz              = Sg1.dDPz;
    Sg0.dVelStart         = Sg1.dVelStart;
    Sg0.dGasVelo          = Sg1.dGasVelo;
    Sg0.dLiqVelo          = Sg1.dLiqVelo;
    Sg0.dVelocity         = Sg1.dVelocity;


    GasSeg.InsertAt(0, pG);
    SegsChanged();
    }
  if (LSegLength(0) > SegSplitFactor * MeanSegLength())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Add LiqSeg Start L:%8.2f Msl:%8.2f", LSegLength(0), MeanSegLength());
    #endif
    TPPLSegment &Sg0 =*LiqSeg[0];
    TPPLSegment &Sg1 =*LiqSeg[1];
    // Add new segment

    double TM0 = Temperature(0.25 * LSegEnd(0));
    double TMM = Temperature(0.50 * LSegEnd(0));
    double TM1 = Temperature(0.75 * LSegEnd(0));

    double Scl0, Scl1;
    Scl0 = 0.5;
    Scl1 = 1.0 - Scl0;

    TPPLSegment *pL = new TPPLSegment(0.0, 0.0, Scl0*Sg0.dMass, TM0, Sg0.LProps, Sg0.pImg);
    TPPLSegment &SgN=*pL;

    SgN.iPos->SetTestValue(dLength);
    Sg0.dMass*=Scl1;
    Sg0.dTemp*=TM1;
    Sg0.SetPosition(0.5*LSegLength(0));
    Sg0.bHorFlowRegime    = Sg1.bHorFlowRegime;
    Sg0.bFlowRegime       = Sg1.bFlowRegime;
//    Sg0.dQmRng            = Sg1.dQmRng;
    Sg0.dQmEst            = Sg1.dQmEst;
    Sg0.dQmG              = Sg1.dQmG;
    Sg0.dQmL              = Sg1.dQmL;
    Sg0.dQvG              = Sg1.dQvG;
    Sg0.dQvL              = Sg1.dQvL;
    Sg0.dVapFrac          = Sg1.dVapFrac;
    Sg0.sLiqVolFrac       = Sg1.sLiqVolFrac;
    Sg0.dDarcyCorrection  = Sg1.dDarcyCorrection;
    Sg0.dSupGasVel        = Sg1.dSupGasVel;
    Sg0.dSupLiqVel        = Sg1.dSupLiqVel;
    Sg0.dTotalVel         = Sg1.dTotalVel;
    Sg0.dDPq              = Sg1.dDPq;
    Sg0.dDPqDq            = Sg1.dDPqDq;
    Sg0.dDPmB             = Sg1.dDPmB;
    Sg0.dDPmQ             = Sg1.dDPmQ;
    Sg0.dDPmDq            = Sg1.dDPmDq;
    Sg0.dDPApplied        = Sg1.dDPApplied;
    Sg0.dDPz              = Sg1.dDPz;
    Sg0.dVelStart         = Sg1.dVelStart;
    Sg0.dGasVelo          = Sg1.dGasVelo;
    Sg0.dLiqVelo          = Sg1.dLiqVelo;
    Sg0.dVelocity         = Sg1.dVelocity;

    LiqSeg.InsertAt(0, pL);
    SegsChanged();
    }

  if (GSegLength(LastFullGSeg()) > SegSplitFactor * MeanSegLength())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Add GasSeg End L:%8.2f Msl:%8.2f", GSegLength(LastFullGSeg()), MeanSegLength());
    #endif

    TPPLiqProps LProps0;
    TPPLiqProps LProps1;

    double LiqMass0=0.0;
    double LiqTemp0=0.0;
    double LiqMass1=0.0;
    double LiqTemp1=0.0;
    LiqMassInSegs(GSegStart(LastFullGSeg()), GSegMid(LastFullGSeg()), LiqMass0, LiqTemp0, LProps0);
    LiqMassInSegs(GSegMid(LastFullGSeg()), GSegEnd(LastFullGSeg()), LiqMass1, LiqTemp1, LProps1);

    double LiqVol0=LiqMass0/GTZ(LProps0.dLiqDens);
    double LiqVol1=LiqMass1/GTZ(LProps1.dLiqDens);

#if PressCalc1
    double GasVol0=0.5*GSegLength(LastFullGSeg())*dArea;
    double GasVol1=0.5*GSegLength(LastFullGSeg())*dArea;
#else
    double GasVol0=0.5*GSegLength(LastFullGSeg())*dArea-LiqVol0;
    double GasVol1=0.5*GSegLength(LastFullGSeg())*dArea-LiqVol1;
#endif
    double GasVolM=0.5*(GasVol0+GasVol1);

//  TPPGSegment &Sg0 =*GasSeg[LastFullGSeg()-1];
    TPPGSegment &Sg0 =*GasSeg[LastFullGSeg()];

    double PosS=GSegStart(LastFullGSeg());
    double Pos0=PosS + 0.25 * GSegLength(LastFullGSeg());
    double PosM=PosS + 0.50 * GSegLength(LastFullGSeg());
    double Pos1=PosS + 0.75 * GSegLength(LastFullGSeg());
    // Add new segment
    double Scl0, Scl1;
    double TM0 = Temperature(Pos0);
    double TMM = Temperature(PosM);
    double TM1 = Temperature(Pos1);

    double PM0 = Pressure(Pos0);
    double PMM = Pressure(PosM);
    double PM1 = Pressure(Pos1);

    double X0 = PM0/GTZ(TM0);
    double X1 = PM1/GTZ(TM1);

    Scl0=X0/GTZ(X0+X1)*GasVol0/GTZ(GasVolM);

    //Scl0 = 0.5 * PM0/GTZ(PMM)*TMM/GTZ(TM0)*GasVol0/GTZ(GasVolM);
    Scl1 = 1.0 - Scl0;

    TPPGSegment *pG = new TPPGSegment(PosM, Sg0.Velocity(), Scl1*Sg0.dMass, TM1, Sg0.GProps, Sg0.pImg);
    TPPGSegment &SgN=*pG;

    SgN.iPos->SetTestValue(dLength);
    Sg0.dMass*=Scl0;
    Sg0.dTemp=TM0;
    //0.SetPosition(0.5*GSegEnd(0));
    // should be Assignment operator
    SgN.bHorFlowRegime    = Sg0.bHorFlowRegime;
    SgN.bFlowRegime       = Sg0.bFlowRegime;
//    Sg0.dQmRng            = Sg1.dQmRng;
    SgN.dQmEst            = Sg0.dQmEst;
    SgN.dQmG              = Sg0.dQmG;
    SgN.dQmL              = Sg0.dQmL;
    SgN.dQvG              = Sg0.dQvG;
    SgN.dQvL              = Sg0.dQvL;
    SgN.dVapFrac          = Sg0.dVapFrac;
    SgN.sLiqVolFrac       = Sg0.sLiqVolFrac;
    SgN.dDarcyCorrection  = Sg0.dDarcyCorrection;
    SgN.dSupGasVel        = Sg0.dSupGasVel;
    SgN.dSupLiqVel        = Sg0.dSupLiqVel;
    SgN.dTotalVel         = Sg0.dTotalVel;
    SgN.dDPq              = Sg0.dDPq;
    SgN.dDPqDq            = Sg0.dDPqDq;
    SgN.dDPmB             = Sg0.dDPmB;
    SgN.dDPmQ             = Sg0.dDPmQ;
    SgN.dDPmDq            = Sg0.dDPmDq;
    SgN.dDPApplied        = Sg0.dDPApplied;
    SgN.dDPz              = Sg0.dDPz;
    SgN.dVelStart         = Sg0.dVelStart;
    SgN.dGasVelo          = Sg0.dGasVelo;
    SgN.dLiqVelo          = Sg0.dLiqVelo;
    SgN.dVelocity         = Sg0.dVelocity;


    GasSeg.InsertAt(LastFullGSeg()+1, pG);
    SegsChanged();
    }
  if (LSegLength(LastFullLSeg()) > SegSplitFactor * MeanSegLength())
    {
    #if dbgPipeline
    if (dbgAddSegs() && fDoDbgBrk)
      dbgpln("Add LiqSeg End L:%8.2f Msl:%8.2f", LSegLength(LastFullLSeg()), MeanSegLength());
    #endif
    TPPLSegment &Sg0 =*LiqSeg[LastFullLSeg()];
    //TPPLSegment &Sg1 =*LiqSeg[1];
    // Add new segment
    double PosS=LSegStart(LastFullLSeg());
    double Pos0=PosS + 0.25 * LSegLength(LastFullLSeg());
    double PosM=PosS + 0.50 * LSegLength(LastFullLSeg());
    double Pos1=PosS + 0.75 * LSegLength(LastFullLSeg());

    double TM0 = Temperature(Pos0);
    double TMM = Temperature(PosM);
    double TM1 = Temperature(Pos1);

    double Scl0, Scl1;
    Scl0 = 0.5;
    Scl1 = 1.0 - Scl0;

    TPPLSegment *pL = new TPPLSegment(PosM, Sg0.Velocity(), Scl1*Sg0.dMass, TM1, Sg0.LProps, Sg0.pImg);
    TPPLSegment &SgN=*pL;

    SgN.iPos->SetTestValue(dLength);
    Sg0.dMass*=Scl0;
    Sg0.dTemp*=TM0;
    //Sg0.SetPosition(0.5*LSegEnd(0));
    SgN.dVelocity = Sg0.dVelocity;

    LiqSeg.InsertAt(LastFullLSeg()+1, pL);
    SegsChanged();
    }

  if (dTime>0.0 && nSlugs<nMaxSlugs)
    {

    for (int s=0; s<NFullGSegs(); s++)
      {
      TPPGSegment &Sg=*GasSeg[s];
      // Add to number of slugs slugs if neccessary
      if ((dTime>0.0) && (Sg.bFlowRegime==TPR_Slug))
        {
        if (((float)rand())/RAND_MAX<dSlugAppears)
          {
          double SlugPos=GSegStart(s)+((double)rand())/RAND_MAX*GSegLength(s);
          double SlugVol=((float)rand())/RAND_MAX*dMaxSlugVolume;
          CreateSlug(SlugPos, SlugVol);
          }
        }
      }
    for (int sg=1; sg<NFullLSegs()-1; )
      {
      TPPLSegment &Sg=*LiqSeg[sg];
      if (IsSlug(sg) && LSegEnd(sg)<dLength)
        if (GasSeg[GSegIndex(Sg.Position())]->bFlowRegime!=TPR_Slug)
          if (RemoveSlug(sg))
            continue;
      sg++;
      }

//    double SegCombineLen=MeanSegLength();
//    for (sg=1; sg<NFullLSegs()-1; )
//      {
//      TPPLSegment &Sg=*LiqSeg[sg];
//      if ((LSegLength(sg)+LSegLength(sg+1))<SegCombineLen && !IsSlug(sg) && !IsSlug(sg+1))
//        if (CombineLSegs(sg, sg+1))
//          continue;
//      sg++;
//      }

    }

  nSlugs=0;
  dTotLiqHoldup=0.0;
  for (int s=0; s<Sect.GetSize(); s++)
    {
    TPPSection &Sct=Sect[s];
    Sct.nSlugs=0;
    Sct.dTotLiqHoldup=0.0;
    }

  for (sg=0; sg<NFullLSegs(); sg++)
    {
    TPPLSegment &Sg=*LiqSeg[sg];
    double SegS=LSegStart(sg);
    double SegE=LSegEnd(sg);
    double SegL=GTZ(SegE-SegS);

    int Sct0=SectIndex(SegS);
    int Sct1=SectIndex(SegE);
    // Buildup Mass in sections;
    for (int s=Sct0; s<=Sct1; s++)
      {
      double Portion=GEZ(Min(SectionEnd(s), SegE)-Max(SectionStart(s), SegS))/GTZ(SegE-SegS);
      TPPSection & Sct=Sect[s];
      double LiqVolume=Portion*Sg.dMass/Sg.LProps.dLiqDens;
      Sect[s].dTotLiqHoldup+=LiqVolume;
      dTotLiqHoldup+=LiqVolume;
      }
    if (IsSlug(sg))
      {
      Sect[Sct0].nSlugs++;
      nSlugs++;
      }
    }

  Strng Tg;
  for (sg=0; sg<NGSegments(); sg++)
    {
    GSeg(sg).iPos->Tag().Set("%s.GSPos_%i",sTag(),sg);
    //GSeg(sg).iPos->TagIs(Tg());
    GSeg(sg).iTemp->Tag().Set("%s.GSTmp_%i",sTag(),sg);
    //GSeg(sg).iTemp->TagIs(Tg());
    GSeg(sg).iMass->Tag().Set("%s.GSMas_%i",sTag(),sg);
    //GSeg(sg).iMass->TagIs(Tg());
    }
  for (sg=0; sg<NLSegments(); sg++)
    {
    LSeg(sg).iPos->Tag().Set("%s.LSPos_%i",sTag(),sg);
    //LSeg(sg).iPos->TagIs(Tg());
    LSeg(sg).iTemp->Tag().Set("%s.LSTmp_%i",sTag(),sg);
    //LSeg(sg).iTemp->TagIs(Tg());
    LSeg(sg).iMass->Tag().Set("%s.LSMas_%i",sTag(),sg);
    //LSeg(sg).iMass->TagIs(Tg());
    }
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::StartSolution()
  {
  if (bSimple)
    {
    MN_Lnk::StartSolution();
    return;
    }
  MdlNode::StartSolution();
  EvaluateSegment(0.0);
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::StartStep()
  {
  if (bSimple)
    {
    MN_Lnk::StartStep();
    return;
    }
  MdlNode::StartStep();
  if (fDoSetLinePress)
    {
    SetLinePress();
    fDoSetLinePress=false;
    }
  if (fDoSetLineSlugs)
    {
    SetLinePress();
    SetLineSegs(True);
    fDoSetLineSlugs=false;
    }

//  if (IOConduit(0)->QMass()>1.0e-10)
//    FlowImg.QSetM(*IOConduit(0), som_ALL, IOConduit(0)->QMass(), IOP_Self(0));
  if (IOQmEst_In(0)>0 && IOConduit(0)->QMass()>1.0e-10)
    {
    FlowImg.QSetM(*IOConduit(0), som_ALL, IOConduit(0)->QMass(), IOP_Self(0));
    GasSeg[0]->pImg->QCopy(*IOConduit(0));//->Model());
    LiqSeg[0]->pImg->QCopy(*IOConduit(0));//->Model());
    }

  if (IOQmEst_In(1)>0 && IOConduit(1)->QMass()>1.0e-10)
    {
    FlowImg.QSetM(*IOConduit(1), som_ALL, IOConduit(1)->QMass(), IOP_Self(1));
    GasSeg[LastFullGSeg()]->pImg->QCopy(*IOConduit(1));//->Model());
    LiqSeg[LastFullLSeg()]->pImg->QCopy(*IOConduit(1));//->Model());
    }

  if (FlowImg.QMass()>1.0e-10)
    {
    double T=FlowImg.Temp();
    double P=FlowImg.Press();
    if (FlowImg.QMass(som_Liq)>1.0e-3)
      {
      FdProps.dNormGasDens=FlowImg.NRho(som_Vap);
      FdProps.dGasDens=FlowImg.Rho(som_Vap);
      SpMArray Vap(FlowImg.MArray(), som_Vap);
      FdProps.dGasCp=FlowImg.msCp(som_ALL,T,P,&Vap);
      }
    if (FlowImg.QMass(som_Liq)>1.0e-3)
      {
      FdProps.dLiqDens=FlowImg.Rho(som_Liq);
      SpMArray Liq(FlowImg.MArray(), som_Liq);
      FdProps.dLiqCp=FlowImg.msCp(som_ALL,T,P,&Liq);
      }
    }

  for (int sg=0; sg<NFullGSegs(); sg++)
    GasSeg[sg]->StartStep();

  for (sg=0; sg<NFullLSegs(); sg++)
    LiqSeg[sg]->StartStep();
  }

// -------------------------------------------------------------------------

void TwoPhasePipe::EvalJoinPressures()
  {
  if (bSimple)
    {
    MN_Lnk::EvalJoinPressures();
    return;
    }

  CalcPressures(ICGetTimeIncFromStartOfStep());
  Set_IOP_Self(0, GasSeg[0]->dPress);
  Set_IOP_Self(1, GasSeg[LastFullGSeg()]->dPress);
  };

// -------------------------------------------------------------------------

flag TwoPhasePipe::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  if (bSimple)
    {
    return MN_Lnk::EvalFlowEquations(IONo, FE);
    }

  #if dbgPipeline
  if (dbgEstDP() && fDoDbgBrk)
    dbgpln("EvalFlws");
  #endif
  FlwBlk &FB  = *IOFB(IONo,0);
  double QIn  = FB.QmSign()*FB.QmMeas();
//  pFlwBlk pC = IOFB(IONo);
  FE_Linear* pEqn=(FE_Linear*)FB.ChangeFlwEqn(&FE_LinearClass);
  if (typeid(*pEqn)!=typeid(FE_Linear))
    {
    DoBreak();
    }

  int iSg=IONo==0 ? 0 : LastFullGSeg();
  TPPFlowStuff &Flw=IOFlw[IONo];
  int Sgn=(IONo==0) ? 1 : -1;

  if (IONo==1)
    {
    int xxx=0;
    }

  double Len=0.5*GSegLength(iSg);
  double Angle=IONo==0 ? dAngleS : dAngleE;
  double Mass=0.5*GasSeg[iSg]->dMass;
  TPPProps &Props=GasSeg[iSg]->MxProps;
  Flw.EstimateDP(*this, Props, Len, Angle, Mass, QIn);


  FB.SetActLength(Len);
  FB.SetDPq(Flw.dDPq, Flw.dDPqDq);
  FB.SetDPz(Flw.dDPz, 0.0);
//  FB.SetDPm(-Sgn*Flw.dDPmB, -Sgn*Flw.dDPmQ, -Sgn*Flw.dDPmDq);
  FB.SetArea(dArea);
  FB.SetMomentum(/*IC,*/ Max(10.0, FlowImg.Rho()*4.0)); // *4 is a kludge
  FB.SetQmFree();

//  if (IONo==1)
//    {
//dbgpln("-- L:%11.4g M:%11.4g P:%11.4g Q:%11.4g dPq:%11.4g dPmB:%11.4g DPApp:%11.4g Rho:%11.4g",
//       Len, Mass, GasSeg[LastFullGSeg()]->dPress, QIn, Flw.dDPq, FB.DPmB(), Flw.dDPApplied, Props.dGasDens);
//    }

  Flw.dDPApplied=Sgn*(IOP_Flng(IONo)-IOP_Self(IONo));
  Flw.dQmEst=QIn*Sgn;
  Flw.SetFlwInfo(*this, Props, QIn*Sgn);

  return True;
  };

//--------------------------------------------------------------------------

void TwoPhasePipe::EvalProducts()
  {
  if (bSimple)
    {
    MN_Lnk::EvalProducts();
    return;
    }

  int LastFlw=UBSect();

  for (int sg=1; sg<NFullGSegs(); sg++)
    {
    TPPGSegment &Sg0=*GasSeg[sg-1];
    TPPGSegment &Sg1=*GasSeg[sg];
    double Len=GSegMid(sg)-GSegMid(sg-1);
    double Angle=Sect[SectIndex(GSegStart(sg))].dAngle;
    double Mass=0.5*(Sg0.dMass+Sg1.dMass);

    TPPProps Mx;
    Mx.InitSum();
    Mx.Sum(Sg0.MxProps, Sg0.dMass, Sg0.dLiqMass);
    Mx.Sum(Sg1.MxProps, Sg1.dMass, Sg1.dLiqMass);

    Sg1.EstimateFlows(*this, Mx, Len, Angle, Mass, Sg1.dPressGrad*Len);
    }

  for (sg=1; sg<NFullLSegs(); sg++)
    {
    TPPLSegment &Sg0=*LiqSeg[sg-1];
    TPPLSegment &Sg1=*LiqSeg[sg];
    double Pos0=LSegMid(sg-1);
    double Pos1=LSegMid(sg);
    double Len=Pos1-Pos0;
    double Angle=Sect[SectIndex(LSegStart(sg))].dAngle;
    double Mass=0.5*(Sg0.dMass+Sg1.dMass);

    double DPress=Pressure(Pos0)-Pressure(Pos1);
    //if (DPress)
    TPPProps Mx;

    Mx.InitSum();
    Mx.Sum(Sg0.MxProps, Sg0.dGasMass, Sg0.dMass);
    Mx.Sum(Sg1.MxProps, Sg1.dGasMass, Sg1.dMass);

    Sg1.EstimateFlows(*this, Mx, Len, Angle, Mass, DPress);
    }

  if (IOQmEst_Out(0)>=0.0)
    {
    double GasMass=0.0;
    double LiqMass=0.0;
    double MixTemp=Std_T;
    AddSweptMass(True, ICGetTimeInc(), GasMass, LiqMass, MixTemp);


    IOConduit(0)->QSetM(*(GasSeg[0]->pImg), som_Vap, GasMass/GTZ(ICGetTimeInc()), IOP_Self(0));
    IOConduit(0)->QAddM(*(LiqSeg[0]->pImg), som_Liq, LiqMass/GTZ(ICGetTimeInc()));
    //IOConduit(0)->QSetM(*(GasSeg[0]->pImg), som_Vap, fabs(IOFlw[0].dQmG), IOP_Self(0));
    //IOConduit(0)->QAddM(*(LiqSeg[0]->pImg), som_Liq, fabs(IOFlw[0].dQmL));
    //Conduit(0)->SetTemp(MixTemp);
    IOConduit(0)->SetTemp(GasSeg[0]->dTemp);
    }
  dQmGEntry=Sign(IOQmEst_In(0))*IOConduit(0)->QMass(som_Vap);
  dQmLEntry=Sign(IOQmEst_In(0))*IOConduit(0)->QMass(som_SL);

  double T0=IOConduit(0)->Temp();
  double P0=IOP_Self(0);
  SpMArray G0(IOConduit(0)->MArray(), som_Vap);
  SpMArray L0(IOConduit(0)->MArray(), som_Liq);

  dCpGEntry=IOConduit(0)->msCp(som_ALL,T0, P0, &G0);
  dCpLEntry=IOConduit(0)->msCp(som_ALL,T0, P0, &L0);

  dTempEntry=IOConduit(0)->Temp();

  if (IOQmEst_Out(1)>=0.0)
    {
    double GasMass=0.0;
    double LiqMass=0.0;
    double MixTemp=Std_T;
    AddSweptMass(False, ICGetTimeInc(), GasMass, LiqMass, MixTemp);
    IOConduit(1)->QSetM(*(GasSeg[LastFullGSeg()]->pImg), som_Vap, GasMass/GTZ(ICGetTimeInc()), IOP_Self(1));
    IOConduit(1)->QAddM(*(LiqSeg[LastFullLSeg()]->pImg), som_Liq, LiqMass/GTZ(ICGetTimeInc()));
//    IOConduit(1)->QSetM(FlowImg, som_Vap, GasMass/GTZ(ICGetTimeInc()), IOP_Self(1));
//    IOConduit(1)->QAddM(FlowImg, som_Liq, LiqMass/GTZ(ICGetTimeInc()));
//    IOConduit(1)->QSetM(*(GasSeg[LastFullGSeg()]->pImg), som_Vap, fabs(IOFlw[1].dQmG), IOP_Self(1));
//    IOConduit(1)->QAddM(*(LiqSeg[LastFullLSeg()]->pImg), som_Liq, fabs(IOFlw[1].dQmL));

    IOConduit(1)->SetTemp(GasSeg[LastFullGSeg()]->dTemp);
    }

  dQmGExit=Sign(IOQmEst_Out(1))*IOConduit(1)->QMass(som_Vap);
  dQmLExit=Sign(IOQmEst_Out(1))*IOConduit(1)->QMass(som_SL);

  double T1=IOConduit(1)->Temp();
  double P1=IOP_Self(1);
  SpMArray G1(IOConduit(1)->MArray(), som_Vap);
  SpMArray L1(IOConduit(1)->MArray(), som_Liq);

  dCpGExit=IOConduit(1)->msCp(som_ALL,T1, P1, &G1);
  dCpLExit=IOConduit(1)->msCp(som_ALL,T1, P1, &L1);
//  dCpGExit=IOConduit(1)->msCp(som_Vap);
//  dCpLExit=IOConduit(1)->msCp(som_SL);
  dTempExit=IOConduit(1)->Temp();
  };

//--------------------------------------------------------------------------

const int SmoothVelocities = 1;

void TwoPhasePipe::EvalDerivs()
  {
  if (bSimple)
    {
    MN_Lnk::EvalDerivs();
    return;
    }

  Strng Tg;
  double SteelMassPerLen=7800.0*PI*(Sqr(dODiam)-Sqr(dDiam))/4.0;

  for (int sg=0; sg<NGSegments(); sg++)
    {
    GSeg(sg).dMassDeriv=0.0;
    GSeg(sg).dMassTest=Max(1.0, 100.0*GSeg(sg).dMass);
    GSeg(sg).dTempDeriv=0.0;
    }
  for (sg=0; sg<NLSegments(); sg++)
    {
    LSeg(sg).dMassDeriv=0.0;
    LSeg(sg).dMassTest=Max(1.0, 100.0*LSeg(sg).dMass);
    LSeg(sg).dTempDeriv=0.0;
    }

  GSeg(0).dMassDeriv=Max(dQmGEntry, -GSeg(0).dMass/(ICGetTimeInc()*1.5));
  LSeg(0).dMassDeriv=Max(dQmLEntry, -LSeg(0).dMass/(ICGetTimeInc()*1.5));

  GSeg(LastFullGSeg()).dMassDeriv=Max(-dQmGExit, -GSeg(LastFullGSeg()).dMass/(ICGetTimeInc()*1.5));
  LSeg(LastFullLSeg()).dMassDeriv=Max(-dQmLExit, -LSeg(LastFullLSeg()).dMass/(ICGetTimeInc()*1.5));

  for (sg=1; sg<NFullGSegs(); sg++)
    {
    TPPGSegment &Sg1=GSeg(sg);
    double V=VelDampC*Sg1.dGasVelo +
             VelDampE*GasSeg[Max(1, sg-1)]->dGasVelo +
             VelDampE*GasSeg[Min(NFullGSegs()-1, sg+1)]->dGasVelo;

    GSeg(sg).SetVelocity(V);
    #if dbgPipeline
    if (dbgVelocities() && fDoDbgBrk)
      dbgpln("SgGVel %3i V:%14.6f", sg, GSeg(sg).dVelocity);
    else if (fabs(GSeg(sg).dVelocity)>100.0)
      {
      dbgpln("LARGE GAS VEL %3i V:%14.6f %s ", sg, GSeg(sg).dVelocity, FullObjTag());
      }
    #endif
    GSeg(sg).dVelocity=Range(-100.0, GSeg(sg).dVelocity, 100.0);
    }

  GSeg(0).SetVelocity(0);
  GSeg(NFullGSegs()).SetVelocity(0);

  for (sg=1; sg<NFullLSegs(); sg++)
    {
    double V=VelDampC*LiqSeg[sg]->dLiqVelo +
             VelDampE*LiqSeg[Max(1, sg-1)]->dLiqVelo +
             VelDampE*LiqSeg[Min(NFullLSegs()-1, sg+1)]->dLiqVelo;

    LSeg(sg).SetVelocity(V);
    #if dbgPipeline
    if (dbgVelocities() && fDoDbgBrk)
      dbgpln("SgLVel %3i V:%14.6f", sg, LVelocity(LSeg(sg).Position()));
    else if (fabs(LSeg(sg).dVelocity)>100.0)
      {
      dbgpln("LARGE LIQ VEL %3i V:%14.6f %s ", sg, LSeg(sg).dVelocity, FullObjTag());
      }
    #endif
    LSeg(sg).dVelocity=Range(-100.0, LSeg(sg).dVelocity, 100.0);
    }

  LSeg(0).SetVelocity(0);
  LSeg(NFullLSegs()).SetVelocity(0);

  int lsg=0;
  for (sg=0; sg<NFullGSegs(); sg++)
    {
    TPPGSegment &Sg=*GasSeg[sg];
    double SegLen=GSegLength(sg);

    double TempDiff=dSeaTemp-Sg.dTemp;
    double SegPipeArea=SegLen*dDiam*PI;
    double UA=SegPipeArea*dHTC;

    double SteelMass=SegLen*SteelMassPerLen; // kJ/C
    double SteelHCap=SteelMass*0.449; // kJ/C
    double GasHCap=Sg.dMass*Sg.GProps.dGasCp;
    double LiqHCap=Sg.dLiqMass*Sg.MxProps.dLiqCp;
    double TotHCap=SteelHCap+GasHCap+LiqHCap;

    // Feed at Start
    if (sg==0 && (dQmGEntry+dQmLEntry)>0.0)
      {
      double FlwHCap=dQmGEntry*dCpGEntry+dQmLEntry*dCpLEntry;
      double TempChgRate=(TotHCap*Sg.dTemp+FlwHCap*dTempEntry)/GTZ(TotHCap+FlwHCap)-Sg.dTemp;
      Sg.dTempDeriv+=TempChgRate;
      }
    else if (sg==LastFullGSeg() && (dQmGExit+dQmLExit)<0.0)
      {
      double FlwHCap=-(dQmGExit*dCpGExit+dQmLExit*dCpLExit);
      double TempChgRate=(TotHCap*Sg.dTemp+FlwHCap*dTempExit)/GTZ(TotHCap+FlwHCap)-Sg.dTemp;
      Sg.dTempDeriv+=TempChgRate;
      }

    // Change due to loss to surrounding sea
    Sg.dTempDeriv+=TempDiff*UA/TotHCap;

    #if dbgPipeline
    if (dbgTempDerivs() && fDoDbgBrk)
      dbgp("SgTmp %3i T:%14.6f  dT:%14.6f", sg, Sg.dTemp, Sg.dTempDeriv);
    #endif
    // Change due to movement of segment
    // heat xfer with steel
    if (sg>0)
      {
      double StartVel=GSegStartVel(sg);
      if (StartVel<0.0)
        {
        // start moving backwards
        TPPGSegment &SgS=*GasSeg[sg-1];
        double SteelMassVel=StartVel*SteelMassPerLen; // kg/s
        double SteelHCapVel=SteelMassVel*0.449;     // kJ/C

        double TempChgRate=(TotHCap*Sg.dTemp+fabs(SteelHCapVel)*SgS.dTemp)/GTZ(TotHCap+fabs(SteelHCapVel))-Sg.dTemp;
        Sg.dTempDeriv+=TempChgRate;
        #if dbgPipeline
        if (dbgTempDerivs() && fDoDbgBrk)
          dbgp("dTS:%14.6f", TempChgRate);
        if (fabs(TempChgRate)>100.0)
          {
          int xxx=0;
          }
        #endif
        }
      }
    if (sg<NFullGSegs()-1)
      {
      double EndVel=GSegEndVel(sg);
      if (EndVel>0.0 )
        {
        //end moving forwards
        TPPGSegment &SgE=*GasSeg[sg+1];
        double SteelMassVel=EndVel*SteelMassPerLen; // kg/s
        double SteelHCapVel=SteelMassVel*0.449;     // kJ/C

        double TempChgRate=(TotHCap*Sg.dTemp+SteelHCapVel*SgE.dTemp)/GTZ(TotHCap+SteelHCapVel)-Sg.dTemp;
        Sg.dTempDeriv+=TempChgRate;
        #if dbgPipeline
        if (dbgTempDerivs() && fDoDbgBrk)
          dbgp("dTE:%14.6f", TempChgRate);
        #endif
        }
      }

    #if dbgPipeline
    if (dbgTempDerivs() && fDoDbgBrk)
      dbgpln(" >> dT:%14.6f %s", Sg.dTempDeriv, FullObjTag());
    #endif

    for (;;)
      {
      if (LSegMid(lsg)>GSegEnd(sg) || lsg>=LastFullLSeg())
        break;
      LiqSeg[lsg]->dTempDeriv=Sg.dTempDeriv;

      lsg++;
      }

    }
  // Overide Velocities of Slug Segments
  for (sg=0; sg<NFullLSegs()-1; sg++)
    {
    if (IsSlug(sg))
      {
      TPPLSegment &Sg0=*LiqSeg[sg];
      TPPLSegment &Sg1=*LiqSeg[sg+1];
      double V=(Sg0.dVelocity+Sg1.dVelocity)*0.5;

      Sg0.SetVelocity(V);
      Sg1.SetVelocity(V);
      }
    }
  };

//--------------------------------------------------------------------------

void TwoPhasePipe::ConvergeStates()
  {
  if (bSimple)
    {
    MN_Lnk::ConvergeStates();
    return;
    }

  MdlNode::ConvergeStates();
  double Duty=0.0;
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::ApplyDerivs(double dTime, flag DoDbg)
  {
  if (bSimple)
    {
    MN_Lnk::ApplyDerivs(dTime, DoDbg);
    return;
    }

  for (int sg=0; sg<NGSegments(); sg++)
    {
    GSeg(sg).dMass+=GSeg(sg).dMassDeriv*dTime;
    GSeg(sg).dTemp+=GSeg(sg).dTempDeriv*dTime;
    GSeg(sg).dPosition+=GSeg(sg).dVelocity*dTime;
    }
  for (sg=0; sg<NLSegments(); sg++)
    {
    LSeg(sg).dMass+=LSeg(sg).dMassDeriv*dTime;
    LSeg(sg).dTemp+=LSeg(sg).dTempDeriv*dTime;
    LSeg(sg).dPosition+=LSeg(sg).dVelocity*dTime;
    }

  CDArray DLen, MinLen;
  DLen.SetSize(NFullLSegs()+1);
  MinLen.SetSize(NFullLSegs()+1);
  DLen[0]=0;
  DLen[NFullLSegs()]=0;
  //dbgpln("Filling ---------------------- ");
  for (sg=1; sg<NFullLSegs(); sg++)
    {
    double Filling = (LSeg(sg).dMass/LSeg(sg).MxProps.dLiqDens)/LSegVolume(sg);
    MinLen[sg]=(LSeg(sg).dMass/LSeg(sg).MxProps.dLiqDens)/(0.99*Area());
    DLen[sg]=GEZ(MinLen[sg]-LSegLength(sg));
    //dbgpln("  %3i  Fill:%10.4f%% %10.2f L:%10.2f DL:%10.2f Vel:%10.2f",sg,Range(-9999.,Filling*100, 9990.),LSeg(sg).dPosition,LSegLength(sg),DLen[sg],LSeg(sg).dVelocity);
    }

  bool passesdone=false;
  for (int pass=0; pass<100 && !passesdone; pass++)
    {
    passesdone=true;
    DLen[0]=0;
    DLen[NFullLSegs()]=0;
    for (sg=1; sg<NFullLSegs()-1; sg++)
      {
      DLen[sg]=GEZ(MinLen[sg]-LSegLength(sg));
      LSeg(sg).dPosition+=0.5*(DLen[sg-1]-DLen[sg]);
      if (DLen[sg]>1e-3)
        passesdone=false;
      }
    }
  //dbgpln("  Passes %i", pass);
  //for (sg=1; sg<NFullLSegs(); sg++)
  //  {
  //  double Filling = (LSeg(sg).dMass/LSeg(sg).MxProps.dLiqDens)/LSegVolume(sg);
  //  dbgpln("  %3i  Fill:%10.4f%% Pos:%10.2f L:%10.2f Vel:%10.2f",
  //    sg,Range(-9999.,Filling*100, 9990.),LSeg(sg).dPosition,
  //    LSegLength(sg),LSeg(sg).dVelocity);
  //  }

  };

//--------------------------------------------------------------------------

void TwoPhasePipe::EvalDiscrete()
  {
  if (bSimple)
    {
    MN_Lnk::EvalDiscrete();
    return;
    }

  CalcPressures(ICGetTimeInc());
  EvaluateSegment(ICGetTimeInc());
  DbgDump(ICGetTimeInc());

  // Fix Liquid Temperatures if neccessary
  int lsg=0;
  for (int sg=0; sg<NFullGSegs(); sg++)
    {
    TPPGSegment &GSg=*GasSeg[sg];
    for (;;)
      {
      if (LSegMid(lsg)>GSegEnd(sg) || lsg>=LastFullLSeg())
        break;
      LiqSeg[lsg]->dTemp=GSg.dTemp;
      lsg++;
      }
    }

  CalcPressures(0.0);
  DbgDump(ICGetTimeInc());
  }

//--------------------------------------------------------------------------

void TwoPhasePipe::DbgDump(double dTime)
  {
  #if dbgPipeline
  if (dbgTPPDump() && fDoDbgBrk)
    {

    dbgpln("=====================================================================================");
    dbgpln("DTime %10.3f", dTime);
    dbgpln("Sect  Start             ");
    for (int i=0; i<Sect.GetSize(); i++)
      dbgpln("%2i] %6.0f ", i,SectionStart(i));

    dbgpln("Join             P              T");
    for (i=0; i<Join.GetSize(); i++)
      dbgpln("%2i] %14.6g %14.6g",i,Join[i]->dMeasP,Join[i]->dMeasT);

    dbgpln("Seg  Age            Pos            Len            Vel           Mass           Dens");

    for (int sg=0; sg<NGSegments(); sg++)
      {
      TPPGSegment &Sg=*GasSeg[sg];
      if (sg<NFullGSegs())
        dbgpln("G:%3i] %3i %14.6g %14.6g %14.6g %14.6g %14.6g",
          sg, Sg.iAge, Sg.dPosition, GasSeg[sg+1]->dPosition-Sg.dPosition, Sg.dVelocity, Sg.dMass, Sg.GProps.dGasDens);
      else
        dbgpln("G:%3i] %3i %14.6g %14s %14.6g",
          sg, Sg.iAge,Sg.dPosition,"",Sg.dVelocity);
      }
    for (sg=0; sg<NLSegments(); sg++)
      {
      TPPLSegment &Sg=*LiqSeg[sg];
      if (sg<NFullLSegs())
        dbgpln("L:%3i] %3i %14.6g %14.6g %14.6g %14.6g %14.6g",
          sg, Sg.iAge, Sg.dPosition, LiqSeg[sg+1]->dPosition-Sg.dPosition, Sg.dVelocity, Sg.dMass, Sg.LProps.dLiqDens);
      else
        dbgpln("L:%3i] %3i %14.6g %14s %14.6g",
          sg, Sg.iAge,Sg.dPosition,"",Sg.dVelocity);
      }
    dbgpln("=====================================================================================");
    }
  #endif
  }


//==========================================================================
//
//
//
//==========================================================================
