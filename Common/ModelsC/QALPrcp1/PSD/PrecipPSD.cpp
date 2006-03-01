//$T Onetank.c GC 1.136 01/11/02 11:22:49
// Onetank.f -- translated by f2c (version of 23 April 1993  18:34:30).
//   You must link the resulting object file with the libraries:
//	-lf2c -lm   (in that order)
//
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __PRECIPPSD_CPP
//#include "prcptnk2.h"
#include "alspmdlb.h"
#include "qbspmdl.h"
#include "tknpars.h"
#include "precippsd.h"
//#include "optoff.h"

#define dbgModels 01
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgLSFun            ("QALPSD", "LSFun");
static CDbgMngr dbgTransferInputs   ("QALPSD", "TransferInputs");
static CDbgMngr dbgTransferOutputs  ("QALPSD", "TransferOutputs");
#endif

// cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

long CPSDStates::s_iNSpecies=-1;
long CPSDStates::s_iNPSD=-1;
void CPSDStates::SetNStates(long nSpc, long nPsd) 
  { 
  s_iNSpecies=nSpc;
  s_iNPSD=nPsd;
  };

// cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//     A program that implements a dynamic one pass model
//     Program completed April, 2001.
// cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

// Table of constant values
static long    c__3422 = 3422;
static long    c__220 = 220;

void CPSDConc::ClearOverrides()
  {
  cs              = dNAN;//190.8;   // cs input streams
  acs             = dNAN;//.68;     // a/cs input streams
  gpl             = dNAN;//46.7;    // solids in input streams
  //m_Temp          = dNAN;//25;    // Temperature of input streams
  m_BndSoda       = dNAN;//2.48;    // input Soda
  m_BndSodaAsOrg  = dNAN;           // % org soda in bound soda
  cl              = dNAN;//9;       // input Cl
  f               = dNAN;//2.5;     // input F
  co3             = dNAN;//55;      // input CO3
  so4             = dNAN;//.2;      // input SO4
  si              = dNAN;//.57;     // input Si
  fe              = dNAN;//.01;     // input Fe
  ox              = dNAN;//.2;      // input OX
  toc             = dNAN;//25;      // input toc
  a               = 0.0;  //m_FdConc.acs * m_FdConc.cs;
  c               = 0.0;  //m_FdConc.cs * m_ModelParms.m_Cs2C;
  m_Dens          = dNAN;     
  m_SAM           = dNAN;
  }

// cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

CPrecipPSD::CPrecipPSD()
  {
  // Correlations Init
  m_Corr.m_DensCorr = eDensCorrQAL1999;//eDensCorrCRC;
  //m_Corr.m_GrowCorr = eGrowCorrQAL2001a;//eGrowCorrWhite1988;
  m_Corr.m_GrowCorr = eGrowCorrQAL2001;//eGrowCorrWhite1988;
  m_Corr.m_SolCorr  = eSolCorrQAL1999;//eSolCorrRodda1989;
  m_Corr.m_AgglCorr = eAgglCorrIlievski1991;
  m_Corr.m_NucCorr  = eNucCorrMisra1970;
  m_Corr.m_SodaCorr = eSodaCorrQAL1999;//eSodaCorrSang1988;
  m_Corr.m_GrowDis = 4; // what does 4 mean ????  //gtype (discretization for growth)
  m_Corr.m_AgglDis = 1; // what does 1 mean ????  //atype (discretization for agglomeration)

  m_GlobalVars.m_OverallRF     = 1.0; // overall rate factor
  m_GlobalVars.m_GrowthRF      = 0.5; // growth rate factor
  m_GlobalVars.m_NucleationRF  = 0.1; // nucleation rate factor
  m_GlobalVars.m_AgglomRF      = 0.1; // agglomeration rate factor
  // agglomeration kernel factor...
  m_GlobalVars.m_AggKernalF[0] = 0.0;        // Linear 
  m_GlobalVars.m_AggKernalF[1] = 40.0;       // Quadratic
  m_GlobalVars.m_AggKernalF[2] = 500.0*1e-6; // MaxDiam
  m_GlobalVars.m_K1_BoundSoda  = QSMBayer::dK1_BoundSodaSat;//2.35;       // QAL BoundSoda calc factor
  m_GlobalVars.m_G_ActivationEnergy = 7600.0; // Growth equation parameter for QAL correlation
  m_GlobalVars.m_G_K0    = 1.42e10; //6e8; // Growth equation parameter for QAL correlation
  m_GlobalVars.m_G_k_TOC = QSMBayer::dGrowth_k_TOC;//0.0023;  //Rate term for TOC effect on rate - current value is 0.0023
  m_GlobalVars.m_G_n_s   = QSMBayer::dGrowth_n_s  ;//-1.0;    //exponent for soda-dependence of rate constant, current value is -1
  m_GlobalVars.m_G_n_fc  = QSMBayer::dGrowth_n_fc ;//-2.0;    //exponent for free-caustic-dependence of rate constant, current value is -2
  m_GlobalVars.m_G_n_eq  = QSMBayer::dGrowth_n_eq ;//0.0;     //exponent for equilibrium-A/C-dependence of rate constant, current value is 0
  m_GlobalVars.m_G_n_    = QSMBayer::dGrowth_n_   ;//1.7;     //exponent for ?
  m_GlobalVars.m_G_n_sol = QSMBayer::dGrowth_n_sol;//1.0;     //exponent for solids-dependence of precipitation rate, current value is 1
  m_GlobalVars.m_G_n_ssa = QSMBayer::dGrowth_n_ssa;//0.6;     //exponent for SSA-dependence of precipitation rate, current value is 0.6

  m_GlobalVars.m_TankVol = 2200.0;//2221.0;
  m_GlobalVars.relerr = 2200.0;//2221.0;
  m_GlobalVars.relerr = 5e-8;  // relative error for  subroutines
  m_GlobalVars.maxit = 100;  // max number of iterations
  m_GlobalVars.ntanks = 1;  // max number of iterations

  m_ModelParms.m_RhoH    = 2420.0;    // density of hydrate kg/m3 (Al2O3.3H2O Gibbsite)
  m_ModelParms.m_Temp0   = C2K(25.0); // standard temperature

  m_ModelParms.m_Hyd2A         = 0.653568115; // MWAL2O3/(MWALOH3*2) conversion factor Al(OH)3 to AL2O3
  m_ModelParms.m_Cs2C          = 0.584779122; // MWNa2O/MWNa2CO3     conversion factor Na2CO3 to Na2O
  m_ModelParms.m_C2NaOH        = 1.290645628; // (MWNaOH*2)/MWNa2O   conversion factor Na2O to NaOH
  m_ModelParms.m_NaOH2Cs       = 1.324954994; // MWNa2CO3/(MWNaOH*2) conversion factor NaOH to Na2CO3
  m_ModelParms.m_C2OrgSoda     = 3.608180783; // MWNa2C5.2O7.2/MWNa2O  conversion factor Na2O to Na2C5.2O7.2

  m_FdMassSlurry=0.0;

  #ifndef _RELEASE
  m_bXFerOnly=0;
  #endif

  m_bUseFdConcOvr = 0;
  m_FdConcOvr.ClearOverrides();
  m_bUseCnConcOvr = 0;
  m_CnConcOvr.ClearOverrides();

  m_DerivsPars.m_G = 0.0;
  m_DerivsPars.m_Nuc = 0.0;
  m_DerivsPars.m_Agg = 0.0;
  m_DerivsPars.m_ResTime = 0.0;
  m_DerivsPars.m_Mass = 0.0;
  m_DerivsPars.m_l32 = 0.0;
  m_DerivsPars.m_SodaC = 0.0;
  m_DerivsPars.m_SodaR = 0.0;
  m_DerivsPars.m_SodaRate = 0.0;
  m_DerivsPars.m_OrgSodaRate = 0.0;
  m_DerivsPars.m_SolConc = 0;
  m_DerivsPars.m_SSA = 0;
  m_DerivsPars.m_dTRate=0;
  m_DerivsPars.m_dThermalConst=0.006/3600.0; 
  m_DerivsPars.m_dAmbientTemp=C2K(25);  
  m_DerivsPars.m_dThermalLoss=0;  
  m_DerivsPars.m_bWithHOR=1;      
  m_DerivsPars.m_dReactionHeat=0; 
  m_DerivsPars.m_totCp=1;

  m_GrowCoeff.nu = 1;  // upper bandwidth
  m_GrowCoeff.nl = 2;  // lower bandwidth
  for (int i=0; i<sizeof(m_GrowCoeff.grow_coeff__)/sizeof(m_GrowCoeff.grow_coeff__[0]); i++)
    m_GrowCoeff.grow_coeff__[i] = 0.0;
  for (i=0; i<sizeof(m_GrowCoeff.aggl_coeff__)/sizeof(m_GrowCoeff.aggl_coeff__[0]); i++)
    m_GrowCoeff.aggl_coeff__[i] = 0.0;

  m_GridPts.npts=0;
  m_GridPts.q=0;

  memset(&m_FdState, 0, sizeof(m_CnState));

  m_EquilTime = 2*3600.0;

  //m_bBoundSpcsOK=(OrgSodaS.si()>=0) && (CausticS.si()>=0) &&
  //               (OrgSodaL.li()>=0) && (CausticL.li()>=0);
  //m_bDoBoundStuff=false;
  //m_dK1=2.35;
  //m_dBndOrgSoda=0.1;

  };

//--------------------------------------------------------------------------

void CPrecipPSD::InitLocals()
  {
  tend=0;
  tinc=0;
  itol=0;
  loop=0;
  iopt=0;
  nout=0;
  t=0;
  memset(&m_CnState, 0, sizeof(m_CnState));

  itask=0;
  memset(thres, 0, sizeof(thres));
  tlast=0;
  memset(iwork, 0, sizeof(iwork));
  twant=0;
  memset(rwork, 0, sizeof(rwork));
  jt=0;
  istate=0;
  tstart=0;
  neq=0;
  tol=0;
  }

//--------------------------------------------------------------------------

long CPrecipPSD::SolveItInit(SpConduit & Feed, SpContainer & Content)
  {
  m_pFeed=&Feed;
  m_pContent=&Content;

  m_pFdBm = dynamic_cast<SMBayerBase*>(m_pFeed->Model());
  m_pCnBm = dynamic_cast<SMBayerBase*>(m_pContent->Model());

  if (!m_pFdBm)
    return -1;

  if (!m_pCnBm)
    {
    m_pContent->SetModel(m_pFdBm->Class());
    m_pCnBm = dynamic_cast<SMBayerBase*>(m_pContent->Model());
    }

  m_pFdSz = SQSzDist1::FindQual(m_pFeed->Model());
  if (m_pFeed->QMass()<=UsableMass || !m_pFdSz || !m_pFdSz->DistributionsExist())
    return -2;

  m_pCnSz= SQSzDist1::FindQual(m_pContent->Model());
  if (!m_pCnSz || !m_pCnSz->DistributionsExist())
    {
    m_pContent->CopyQualities(m_pFdBm);
    m_pCnSz= SQSzDist1::FindQual(m_pContent->Model());
    }
  m_pFdDistribution=&m_pFdSz->Dist(0);
  m_pCnDistribution=&m_pCnSz->Dist(0);
  if (m_pFdDistribution->PriSp[0]==NULL || m_pCnDistribution->PriSp[0]==NULL)
    return -3;
  m_pFdDist=m_pFdDistribution->PriSp[0];
  m_pFdInts=&m_pFdDistribution->Intervals();
  m_pCnDist=m_pCnDistribution->PriSp[0];
  m_pCnInts=&m_pCnDistribution->Intervals();

  if (m_pFdInts->GetSeriesType() != e_Q)
    {
      LogError("PrecidPSD", 0, "Bad Series Set (must be Q Type)"); 
      return -4;
    }

  if (!(m_pFdInts->Q()>0.0) || !(m_pCnInts->Q()>0.0))
    {
    LogError("PrecidPSD", 0, "Bad Interval Set (Q must be > 0)"); 
    return -5;
    }
  m_FdConc.m_pBm=m_pFdBm;
  m_CnConc.m_pBm=m_pCnBm;

  m_dFdAC=0.0;
  m_dCnAC=0.0;
  m_FdMassSlurry=0.0;
  m_dResTime=0.0;
  m_dGrowthRate=0.0;
  m_dYield=0.0;
  m_dACeq=0.0;
  m_dIonicStrength=0.0;
  m_dAeq=0.0;
  return 0;
  }

//--------------------------------------------------------------------------

long CPrecipPSD::SolveIt(SpConduit & Feed, SpContainer & Content, SpConduit & Prod, bool DoTransferOut)
  {
  try
    {
    InitLocals();

    //  set error control parameters.
    m_GlobalVars.maxit = 100;  // max number of iterations
    m_GlobalVars.relerr = 5e-8;  // relative error for  subroutines

    m_pProd=&Prod;
    long ErrCode = SolveItInit(Feed, Content);
    if (ErrCode!=0)
      return -1;

    if (TransferInputs()!=0)
      return -2;

    #ifndef _RELEASE
    if (!m_bXFerOnly)
      {
    #endif
      long i;

      //     calculate coefficients for growth and agglomeration discretization
      //
      m_GrowCoeff.nu = 1;  // upper bandwidth
      m_GrowCoeff.nl = 2;  // lower bandwidth

      CalcGrowthCoeff(m_GridPts.x, &m_GridPts.npts, &m_Corr.m_GrowDis, m_GrowCoeff.grow_coeff__);
      CalcAgglomCoeff(m_GridPts.x, m_GlobalVars.m_AggKernalF, &m_GridPts.npts, m_GrowCoeff.aggl_coeff__);

      //     parameters for ode solver lsoda
      neq = CPSDStates::NStates() * m_GlobalVars.ntanks;  // total number of eqns in ode
      tol = 5e-4;  // tolerance for ode solver
      itol = 2;    // vector of tolerances
      itask = 1;   // normal task
      istate = 1;  // initialize state
      iopt = 1;    // optional inputs used
      jt = 4;      // user defined Jacobian
      iwork[1-1] = m_GrowCoeff.nl;  // lower bandwidth of Jacobian
      iwork[2-1] = m_GrowCoeff.nu;  // upper bandwidth of Jacobian
      iwork[5-1] = 0; 
      iwork[6-1] = 10000;  // max number of (internal) steps

      //define theshold for zero for ode solver
      for(i = 0; i < CPSDStates::NStates(); ++i)
        thres[i] = 1e-6;  // absolute error tolerance

      //     Initial Setup Completed - now calculate dynamic one-tank model
      //
      //     Set the initial conditions.  Note that tend is taken well past
      //     the last output point, tlast. This is generally desirable for
      //     the numerical solution of odes
      
      //     initialize time and calculate time increments
      tstart = 0.0;  // initial time
      t = tstart;    // initial time
      tend = tlast + 3.0;  // last time for calculation
      tinc = (tlast-tstart)/nout; // time increment


      // CNM - initial stepsize
      //rwork[5-1]   = tinc/5;

      //     Compute answers at nout equally spaced output points. It is good
      //     practice to code the calculation of twant so that the last value
      //     is exactly tlast.

      //     start loop to solve ode at different times
      for (loop=1; loop<=nout; ++loop)
        {
        twant = tlast + (loop - nout) * tinc;  // time for next solution

        // call ode solver
        LSoda(&neq, (double*)&m_CnState, &t, &twant, &itol, &tol, thres, &itask, &istate, &iopt, rwork, &c__3422, iwork, &c__220, &jt);

        // write  elapsed time, number of function evaluations and
        // order of method currently used by ode solver

        if (1)
          LogNote("PSD", 0, "@ %12g %5i %5i %5i", t, iwork[11-1], iwork[12-1], iwork[13-1]);
        // iwork(11) the number of steps taken for the problem so far.
        // iwork(12) the number of LSFun/*f*/ evaluations for the problem so far.
        // iwork(13) the number of jacobian evaluations (and of matrix lu decompositions) for the problem so far.

        //     Test if solver has completed time step or has failed
        if (istate != 2)
          throw new CScdException(0, "Failed to integrate ode");
        }

      //     Write diagnostics from ode solver
      if (1)
        LogNote("PSD", 0, "Steps:%3i LSFun:%3i LSJac:%3i\n", iwork[11-1], iwork[12-1], iwork[13-1]);
    #ifndef _RELEASE
      }
    else
      {
      //make contents = feed...
      m_CnState = m_FdState;
      m_CnConc = m_FdConc; 
      }
    #endif

    if (DoTransferOut)
      TransferOutputs();//Feed, Prod, *pFdBm, *pPrBm, *pFdSz, *pPrSz);

    return 0;
    }
  catch (CScdException *e)
    {
    LogError("PSD", 0, "%i : %s", e->ErrNo(), e->ErrStr());
    delete e;
    }
  return -3;
  }

// ***********************************************************************
//     subroutine fun: Calculates derivative with respect to time of the
//     vector y
// ***********************************************************************

// Subroutine
int CPrecipPSD::LSFun(long *neq, double *t, double *y, double *yp)
  {

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     Common Blocks Required
  //  2  /deriv_pars/  G, Nuc, Agg, Res_time, mass, L32, ybar
  //     G             growth rates
  //     Nuc           nucleation rates
  //     Agg           agglomeration rates
  //     Res_time      residence times
  //     mass          masses of slurry in tanks
  //     L32           Sauter mean diameters
  //     Soda_R        bound soda rate
  //     ybar          vector of basic variables into tank
  //  3  /global_vars/ m_FdMassSlurry, yin, m_GrowthRF, m_NucleationRF, m_AgglomRF, vol,
  //                   temp, RelErr, MaxIt, ntanks, nisc
  //     m_FdMassSlurry          input slurry mass flows
  //     yin           input basic variables
  //     m_GrowthRF           fudge factors for growth rates
  //     m_NucleationRF           fudge factors for nucleation rates
  //     m_AgglomRF           fudge factors for agglomeration rates
  //     vol           tank volumes
  //     temp          tank temperatures
  //     RelErr        relative error criterion for iterations
  //     ISC           locations of ISC
  //     MaxIt         maximum number of iterations allowed
  //     ntanks        number opf tanks
  //     nisc          location of ISC
  //  4  /gridpts/     npts,x,xmin,q,s,qroot,qroot1,ratio,rqq
  //     npts          number of points in discretization of psd
  //     x             midpoints of discretization
  //     xmin          minimum particle size
  //     q             parameter for grid generation
  //     s             parameter for agglomeration calculation
  //     qroot         parameter for agglomeration calculation
  //     qroot1        parameter for agglomeration calculation
  //     ratio         ratio of successive gridpoints
  //     rqq           parameter for agglomeration calculation
  //     grid_exp      parameter for grid spacing
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //
  //     i             counter for do loop
  //     neq           number of equations in system of odes
  //     t             time
  //     ws            mass flow of slurry overflow
  //     y             vector of basic variables
  //     yin           vector of basic variables (inflow)
  //     yp            derivative (wrt time) of y
  //     ypp           temporary storage of derivative of psd
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

  CPSDStates & State = *((CPSDStates*) y);
  CalcDerivParms(State, m_CnConc, NULL);

  EqnsToSolve(y, (double*)&m_FdState,
    &m_DerivsPars.m_G,
    &m_DerivsPars.m_Nuc,
    &m_DerivsPars.m_Agg,
    &m_DerivsPars.m_ResTime,
    m_GrowCoeff.grow_coeff__,
    m_GrowCoeff.aggl_coeff__,
    &m_GridPts.q,
    &m_GridPts.npts,
    yp
  );
  CPSDStates & Dot = *((CPSDStates*) yp);
#if 0&&dbgModels 
  if (dbgLSFun())
    {
    if (1)
      dbgpln("LSFun t:%12.8f T:%12.8f G:%12.8f N:%12.8f A:%12.8f Rt:%12.8f"
             " %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f",
        *t,
        K2C(State.Temp(),m_DerivsPars.m_G,m_DerivsPars.m_Nuc,m_DerivsPars.m_Agg,m_DerivsPars.m_ResTime,
        State.re(),State.rc,State.rCO3,State.rSO4,State.rCl,State.rBndSoda);
    if (0)
      dbgpln("LSFun %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f| %12.1f %12.1f %12.1f %12.1f %12.1f %12.1f",
        State.re(),State.rc,State.rCO3,State.rSO4,State.rCl,State.rSi,State.rFe,State.rOX,State.rOC,State.rBndSoda,
        State.xpsd[0],State.xpsd[1],State.xpsd[2],State.xpsd[3],State.xpsd[4],State.xpsd[5]);
    }
#endif
  return 0;
  } // fun_

// ***********************************************************************
//     subroutine jac: Calculates approximate Jacobian of Pure growth
//     term in linpack banded matrix storage form
// ***********************************************************************

// Subroutine
int CPrecipPSD::LSJac(
                      long     *neq,
                      double  *t,
                      double  *y,
                      long     *ml,
                      long     *mu,
                      double  *pd,
                      long     *nrowpd
                    )
  {
  long pd_dim1, pd_offset, i__1;

  long i, j;

  // Parameter adjustments
  pd_dim1 = *nrowpd;
  pd_offset = pd_dim1 + 1;
  pd -= pd_offset;
  --y;

  // Function Body
  i__1 = m_GridPts.npts;
  for(i = 1; i <= i__1; ++i)
    {
    for(j = 1; j <= 4; ++j)
      {
      pd[j + (i + CPSDStates::NNonPSDStates()) * pd_dim1] = m_DerivsPars.m_G * 1e6 * m_GrowCoeff.grow_coeff__[j + (i << 2) - 5];
      }
    }

  //     add advection component
  i__1 = CPSDStates::NStates();//m_GridPts.npts + NNonPSDStates;
  for(i = 1; i <= i__1; ++i)
    {
    pd[i * pd_dim1 + 2] -= 1. / m_DerivsPars.m_ResTime;
    }

  // WHAT ABOUT Bound Soda Calcs

  return 0;
  }

// ***********************************************************************
//     subroutine Calc_Ws:
//     Calculates
//     1. mass balance of precipitation row
//     2. basic variables into tanks
// ***********************************************************************

// Subroutine
int CPrecipPSD::CalcDerivParms(CPSDStates & State, CPSDConc & Conc, CPSDConc* pConcOvr)
  {
  double        rhos, surf;
  double        rHyd;
  double        mom2, mom3;
  
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //     rhos          density of slurry
  //     Si            mass SiO2/volume liquor
  //     SO4           mass Na2SO4/volume liquor
  //     Soda          % bound Soda
  //     Surf          surface area
  //     TOC           mass total carbon/volume liquor
  //     ws            mass flows of slurry overflow
  //     y             vector of basic variables
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

  mom2 = Moment(State.PSD(), 2, &m_GridPts.npts, m_GridPts.x) * 1e-12;  // calc 2nd moment
  mom3 = Moment(State.PSD(), 3, &m_GridPts.npts, m_GridPts.x) * 1e-18;  // calc 3rd moment
  surf = mom2 * 3.141592653589793;  // Surface area
  m_DerivsPars.m_l32 = mom3 / GTZ(mom2);  // Sauter mean

  //     convert basic variables to State2Conc variables
  rHyd = m_ModelParms.m_RhoH * 3.141592653589793 * mom3 / 6.0; // rh in tank

  State2Conc( State, Conc, NULL, pConcOvr, false);

  //     slurry density
  rhos = DenSlurry(Conc, State.Temp());

  //     mass of slurry in tank
  m_DerivsPars.m_Mass = m_GlobalVars.m_TankVol * rhos;

  //     Residence time
  m_DerivsPars.m_ResTime = m_DerivsPars.m_Mass / GTZ(m_FdMassSlurry);
  m_dResTime = m_DerivsPars.m_ResTime*3600.0; //convert from hours to seconds
  if (m_DerivsPars.m_ResTime<0.000001)
    {
    int xxx=0;
    }

  //double HORTempChg=

  double SlurryCp=m_CnConc.m_pBm->msCp(som_ALL, State.Temp(), Std_P, NULL);
  double HOR=36465; // I Frost 15 April 99
  m_DerivsPars.m_dTRate=(HOR/78)/SlurryCp; 
  m_DerivsPars.m_totCp=m_DerivsPars.m_Mass*SlurryCp; 

  //     growth rate
  // res time
  m_DerivsPars.m_SolConc=Conc.gpl;
  m_DerivsPars.m_SSA=SpecificSurfArea(m_ModelParms.m_RhoH, State.PSD(), &m_GridPts.npts, m_GridPts.x);
  m_DerivsPars.m_G = m_GlobalVars.m_OverallRF * m_GlobalVars.m_GrowthRF * GRate(Conc, State.Temp(), m_DerivsPars.m_SolConc, m_DerivsPars.m_SSA);

  //     nucleation rate
  m_DerivsPars.m_Nuc = m_GlobalVars.m_OverallRF * m_GlobalVars.m_NucleationRF * NRate(Conc, surf, State.Temp());

  //     agglomeration rate
  m_DerivsPars.m_Agg = m_GlobalVars.m_OverallRF * m_GlobalVars.m_AgglomRF * ARate( Conc, State.Temp());
  m_DerivsPars.m_Agg = rhos * m_DerivsPars.m_Agg;

  //     Soda Inclusion
  m_DerivsPars.m_SodaC = SodaC(Conc, State.Temp());
  m_DerivsPars.m_SodaR = m_DerivsPars.m_SodaC * 3. * m_DerivsPars.m_G * rHyd / m_DerivsPars.m_l32;

  double rAlum            = (1-rHyd-State.rXLiq())*m_ModelParms.m_Hyd2A;
  double rSoda            = State.rSoda()*m_ModelParms.m_C2NaOH;
  double rOrgSoda         = State.rOrgSoda()*m_ModelParms.m_C2OrgSoda;

  double rSodiumAluminateM = GEZ(rAlum/NaAluminate2Alumina());
  double rCausticSodaM     = GEZ(rSoda - rSodiumAluminateM*NaAluminate2Caustic());
  double rOrgSodaM         = GEZ(rOrgSoda);

  if (0)
    {
    double X1=rCausticSodaM/m_ModelParms.m_C2NaOH;
    double X2=rOrgSodaM/m_ModelParms.m_C2OrgSoda;
    
    double X=X2/GTZ(X2+X1);

    m_DerivsPars.m_SodaRate     = m_DerivsPars.m_SodaR*(1-X);
    m_DerivsPars.m_OrgSodaRate  = m_DerivsPars.m_SodaR*X    ;

    //m_DerivsPars.m_SodaRate     /= m_ModelParms.m_C2NaOH;
    //m_DerivsPars.m_OrgSodaRate  /= m_ModelParms.m_C2OrgSoda;
    }
  else
    {
    double X=rOrgSodaM/GTZ(rOrgSodaM+rCausticSodaM);

    m_DerivsPars.m_SodaRate     = m_DerivsPars.m_SodaR*(1-X);
    m_DerivsPars.m_OrgSodaRate  = m_DerivsPars.m_SodaR*X    ;

    m_DerivsPars.m_SodaRate     /= m_ModelParms.m_C2NaOH;
    m_DerivsPars.m_OrgSodaRate  /= m_ModelParms.m_C2OrgSoda;
    }
  return 0;
  } // CalcWS

// ***********************************************************************
//
//
// ***********************************************************************

int CPrecipPSD::TransferInputs(bool ForTest/*=false*/)
  {
  m_GlobalVars.ntanks = 1;
  tlast = m_EquilTime/3600.0;
  nout = 3;//10;

  //    Calculate grids
  m_GridPts.npts=m_pFdInts->GetSize();
  m_GridPts.q=m_pFdInts->Q();
  ASSERT_RDB(m_GridPts.npts<sizeof(m_GridPts.x)/sizeof(m_GridPts.x[0]), "Array size!");

  //was
  //for (int i=0; i<m_GridPts.npts; i++)
  //  m_GridPts.x[i]=0.5*((*m_pFdInts)[i]+(*m_pFdInts)[i+1])*1.0e6; // x in microns

  // non end pts
  for (int i=1; i<m_GridPts.npts-1; i++)
    m_GridPts.x[i]=0.5*((*m_pFdInts)[i-1]+(*m_pFdInts)[i])*1.0e6; // x in microns

  // first pt
  m_GridPts.x[0]=m_GridPts.x[1]*(m_GridPts.x[1]/m_GridPts.x[2]); // keep ratio the same
  // last pt
  i=m_GridPts.npts-1;
  m_GridPts.x[i]=m_GridPts.x[i-1]*(m_GridPts.x[i-1]/m_GridPts.x[i-2]); // keep ratio the same
    

  //-----------------------------
  // Setup feed input stream data...
  m_FdMassSlurry = m_pFeed->QMass()*3600.0;   
  CPSDConc* pFdConcOvr = (ForTest || !m_bUseFdConcOvr) ? NULL : &m_FdConcOvr;

#if dbgModels
  if (dbgTransferInputs())
    {
    dbgpln("--- TransferInputs");
    dbgpln("Feed      %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s ",
      "Mass", "Temp", "acs", "cs", "co3", "so4", "cl", "ox", "toc", "gpl", "Soda");
    double Tmp=C2K(25);//m_pCnBm->Temp();
    dbgpln("          %12.2f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f ",// %12.6f %12.6f %12.6f %12.6f ",
      m_pFdBm->Mass(), K2C(m_pFdBm->Temp()), m_pFdBm->AtoC()/m_ModelParms.m_NaOH2Cs, 
      m_pFdBm->CausticConc(Tmp)*m_ModelParms.m_NaOH2Cs, m_pFdBm->SodiumCarbonateConc(Tmp),
      m_pFdBm->SodiumSulphateConc(Tmp), m_pFdBm->CltoC() * m_pFdBm->CausticConc(Tmp));
      //, m_pFdBm->OX(Tmp), m_pFdBm->OX(Tmp)+m_pFdBm->OC(Tmp)
      //, m_pFdBm->SolidsConc(Tmp), m_pFdBm->BoundSodaFrac());
    //m_pFdBm->DumpState();
    }
#endif

  Model2State(m_pFdBm, m_pFdDist, m_FdState);
  State2Conc(m_FdState, m_FdConc, m_pFdBm, NULL);

  //other usefull input display info...
  m_dFdAC = m_pFdBm->AtoC();

  if (m_FdConc.m_Moment2==0.0 || m_FdConc.m_Moment3==0.0)
    return -1; //invalid feed distribution data?!?!?

  //-----------------------------
  // Setup initial conditions (contents) data...
  CPSDConc* pCnConcOvr = (ForTest || !m_bUseCnConcOvr) ? NULL : &m_CnConcOvr;

#if dbgModels
  if (dbgTransferInputs())
    {
    dbgpln("Content   %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s ",
      "Mass", "Temp", "acs", "cs", "co3", "so4", "cl", "ox", "toc", "gpl", "Soda");
    double Tmp=C2K(25);//m_pCnBm->Temp();
    dbgpln("          %12.2f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f ",//%12.6f %12.6f %12.6f %12.6f ",
      m_pCnBm->Mass(), K2C(m_pCnBm->Temp()), m_pCnBm->AtoC()/m_ModelParms.m_NaOH2Cs, 
      m_pCnBm->CausticConc(Tmp)*m_ModelParms.m_NaOH2Cs, m_pCnBm->SodiumCarbonateConc(Tmp),
      m_pCnBm->SodiumSulphateConc(Tmp), m_pCnBm->CltoC() * m_pCnBm->CausticConc(Tmp));//,
      //m_pCnBm->OX(Tmp), m_pCnBm->OX(Tmp)+m_pCnBm->OC(Tmp),
      //m_pCnBm->SolidsConc(Tmp), m_pCnBm->BoundSodaFrac());
    //m_pCnBm->DumpState();
    }
#endif

  Model2State(m_pCnBm, m_pCnDist, m_CnState);//, rh);
  State2Conc(m_CnState, m_CnConc, m_pCnBm, NULL);//, rh);

#if dbgModels
  if (dbgTransferInputs())
    dbgpln("-----------------");
#endif

  return 0;
  } // setinputo_

// ***********************************************************************
//     subroutine WritOutO: writes out a summary of results at end of
//     simulation
// ***********************************************************************

int CPrecipPSD::TransferOutputs()
  {
  // Function Body
  CalcDerivParms(m_CnState, m_CnConc, NULL);

  //     Now tank parameters and overflows
  State2Model( m_CnState, m_pCnBm, m_pCnDist);
  State2Conc( m_CnState, m_CnConc, m_pCnBm, NULL);

  //other usefull display info...
  m_dCnAC = m_pCnBm->AtoC();

  m_dResTime = m_DerivsPars.m_ResTime*3600.0; //convert from hours to seconds
  m_dGrowthRate = m_DerivsPars.m_G * 1.0e6; //convert from ?? to ?? microns/hour
  m_dAeq = Solub(m_CnConc, m_CnState.Temp()/*Conc.m_Temp*/, &m_dIonicStrength);
  m_dACeq = m_dAeq/m_CnConc.cs;
  m_dYield = m_CnConc.cs*(m_dFdAC-m_dCnAC);

#if dbgModels
  if (dbgTransferOutputs())
    {
    dbgpln("                     Rt:%12.6f Gr:%12.6f Aeq:%12.6f ACeq:%12.6f Yld:%12.6f ", 
                                 m_dResTime, m_dGrowthRate, m_dAeq, m_dACeq, m_dYield);
    dbgpln("--- TransferOutputs");
    dbgpln("Mdl  %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s ",
      "", "Temp", "acs", "cs", "co3", "so4", "cl", "ox", "toc", "gpl", "Soda");
    double Tmp=m_pCnBm->Temp();
    //double Tmp=C2K(25);//m_pCnBm->Temp();
    dbgpln("      %12s %12.6f", "", K2C(m_CnState.Temp()));
    dbgpln("      %12.2f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f ",//%12.6f %12.6f %12.6f %12.6f ",
      m_pCnBm->Mass(), K2C(m_pCnBm->Temp()), m_pCnBm->AtoC()/m_ModelParms.m_NaOH2Cs, 
      m_pCnBm->CausticConc(Tmp)*m_ModelParms.m_NaOH2Cs, m_pCnBm->SodiumCarbonateConc(Tmp),
      m_pCnBm->SodiumSulphateConc(Tmp), m_pCnBm->CltoC() * m_pCnBm->CausticConc(Tmp));//,
      //m_pCnBm->OX(Tmp), m_pCnBm->OX(Tmp)+m_pCnBm->OC(Tmp),
      //m_pCnBm->SolidsConc(Tmp), m_pCnBm->BoundSodaFrac());
    }
#endif

  if (m_pCnBm->Temp()<273)
    {
    INCOMPLETECODE();
    }

  return 0;
  }

// Main program alias
//#endif