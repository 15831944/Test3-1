//$T Onetank.c GC 1.136 01/11/02 11:22:49
// Onetank.f -- translated by f2c (version of 23 April 1993  18:34:30).
//   You must link the resulting object file with the libraries:
//	-lf2c -lm   (in that order)
//
#ifndef  __PRECIPPSD_H
#define  __PRECIPPSD_H

#define SIZEDST1
#include "Models.h"
#include "flwnode.h"
#include "alspmdlb.h"

#ifdef __PRECIPPSD_CPP
  #define DllImportExport DllExport
#elif !defined(QALPRCP1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
//#pragma once
#include "myf2c.h"
#include <stdio.h>
#include "lsoda.h"

const long eDensCorrCRC           = 1;
const long eDensCorrQAL1999       = 2;

const long eSolCorrMisra1970      = 1;
const long eSolCorrRodda1989      = 2;
const long eSolCorrRodda1990      = 3;
const long eSolCorrRodda1991      = 4;
const long eSolCorrRosenberg1996  = 5;
const long eSolCorrYoung1997      = 6;
const long eSolCorrQAL1999        = 7;

const long eGrowCorrWhite1988     = 1;
const long eGrowCorrCRC1998       = 2;
const long eGrowCorrQAL2001       = 3;
//const long eGrowCorrQAL2001a      = 4;

const long eNucCorrMisra1970      = 1;

const long eAgglCorrIlievski1991  = 1;

const long eSodaCorrSang1988      = 1;
const long eSodaCorrOhkawa1985    = 2;
const long eSodaCorrQAL1988       = 3;
const long eSodaCorrCRC1993       = 4;
const long eSodaCorrQAL1999       = 5;

struct CPSDCorr
  {
  long   m_DensCorr, m_GrowCorr, m_SolCorr, m_AgglCorr, m_NucCorr, m_SodaCorr, m_GrowDis, m_AgglDis;
  //double m_Temp_Sol, m_Temp_SSA;
  };
struct CPSDDerivsPars
  {
  double m_G; 
  double m_Nuc; 
  double m_Agg; 
  double m_ResTime; 
  double m_Mass; 
  double m_l32; 
  double m_SodaR;
  double m_SodaC;
  double m_SodaRate;
  double m_OrgSodaRate;
  double m_SolConc;
  double m_SSA;
  double m_dTRate;
  double m_dThermalConst; 
  double m_dAmbientTemp;  
  double m_dThermalLoss;  
  flag   m_bWithHOR;      
  double m_dReactionHeat; 
  double m_totCp;
  };

struct CPSDGlobalVars
  {
//  double xm_FdVolSlurry;
  double m_OverallRF, m_GrowthRF, m_NucleationRF, m_AgglomRF, m_AggKernalF[3];
  double m_K1_BoundSoda;
  double m_G_ActivationEnergy;
  double m_G_K0;
  double m_G_k_TOC; //Rate term for TOC effect on rate - current value is 0.0023
  double m_G_n_s;   //exponent for soda-dependence of rate constant, current value is -1
  double m_G_n_fc;  //exponent for free-caustic-dependence of rate constant, current value is -2
  double m_G_n_eq;  //exponent for equilibrium-A/C-dependence of rate constant, current value is 0
  double m_G_n_;    //exponent for ?
  double m_G_n_sol; //exponent for solids-dependence of precipitation rate, current value is 1
  double m_G_n_ssa; //exponent for SSA-dependence of precipitation rate, current value is 0.6
  double m_TankVol;
  double relerr;
  long   maxit, ntanks;
  };

const long MaxPSDGridPts=200;
struct CPSDGridPts
  {
  double x[MaxPSDGridPts];//, xmin, ratio;
  //long   s[20];
  long   npts, q;//, grid_exp__;
  };

struct CPSDGrowCoeff
  {
  long   nu, nl;
  double grow_coeff__[MaxPSDGridPts*4]	/* was [4][200] */, aggl_coeff__[MaxPSDGridPts];
  };

struct CPSDModelParms
  {
  double m_RhoH;
  double m_Hyd2A;
  double m_Cs2C;
  double m_C2NaOH;
  double m_NaOH2Cs;
  double m_Temp0;
  double m_C2OrgSoda;
  };

const long MaxPSDDivs=200;

// NBNB this struct maps onto an array of doubles which gets passed to LSoda
// NB No Virtual methods
struct __declspec (novtable) CPSDStates
  {
  protected:
    double  m_dStates[100];  // this never actually gets allocated - it is just an image on an array of states; 

    static long s_iNSpecies;
    static long s_iNPSD;

    // the States of Alumina (Al[OH]3), Water are not explicitly integrated
    // the States of Caustic + SodiumAluminate are combined into rC
    // state index
    // 0                  Temp()  Temperature
    // 1                  rE()    Mass exhausted water/Mass slurry ratio
    // ..........2                  rA()    Mass Alumina/Mass slurry ratio
    // 2                  rC()    Mass NaOH/Mass slurry ratio
    // 3+i                MFrac() Specie Mass i / Mass Slurry  
    // ..
    // 3+(NSpecies-1)             Last Specie
    // 3+(NSpecies-1)+1   PSD()   Particle Count in 1st Interval / Kg Slurry
    // ..
    // 3+(NSpecies-1)+n           Particle Count in Last Interval / Kg Slurry

    // double  xre;               // 0 - mass exhausted water/mass slurry ratio
    // double  rc;               // 1 - mass Na2O/mass slurry ratio
    // double  rCl;              // 2 - mass NaCl/mass slurry ratio
    // double  rF;               // 3 - mass NaF/mass slurry ratio
    // double  rCO3;             // 4 - mass Na2CO3/mass slurry ratio
    // double  rSO4;             // 5 - mass Na2SO4/mass slurry ratio
    // double  rSi;              // 6 - mass SiO2(impurity)/mass slurry ratio
    // double  rFe;              // 7 - mass Fe2O3(impurity)/mass slurry ratio
    // double  rOX;              // 8 - mass Na2C2O4/mass slurry ratio
    // double  rOC;              // 9 - mass other carbon/mass slurry ratio
    // double  rBndSoda;         // 10  mass BndSoda/mass slurry ratio
    // double  rBndSodaOrg;      // 11  mass BndOrgSoda/mass (BndSoda+BndOrgSoda)
    // double  Temp;             // 12  Temperature
    // double  xpsd[MaxPSDDivs];  // 13 ....

  public:
    static void  SetNStates(long nSpc, long nPsd);
    static long  NSpecies()       { return s_iNSpecies; };
    static long  NPSD()           { return s_iNPSD; };
    static long  NStates()        { return 6+s_iNSpecies+s_iNPSD; };
    static long  NNonPSDStates()  { return 6+s_iNSpecies; };
                                  
    double & Temp()               { return *(m_dStates); };
    double & rXLiq()              { return *(m_dStates+1); };
    //double & rA()                 { return *(m_dStates+2); };
    double & rSoda()              { return *(m_dStates+2); };
    double & rOrgSoda()           { return *(m_dStates+3); };
    double & rBndSoda()           { return *(m_dStates+4); };
    double & rBndOrgSoda()        { return *(m_dStates+5); };
    double & MFrac(long i)        { return *(m_dStates+6+i); };
    double * PSD()                { return   m_dStates+6+s_iNSpecies; };
    double & PSD(long i)          { return *(m_dStates+6+s_iNSpecies+i); };
    double & operator[](long i)   { return   m_dStates[i]; };
  };


class CPSDConc
  {
  public:
    double  acs;            //  mass Al2O3/mass Na2CO3 ratio                     gpl  =a/cs
    double  cs;             //  mass Na2CO3/volume liquor ratio                  gpl
    double  cl;             //  mass NaCl/volume liquor ratio                    gpl
    double  f;              //  mass NaF/volume liquor ratio                     gpl
    double  co3;            //  mass Na2CO3(impurity)/volume liquor ratio        gpl
    double  so4;            //  mass Na2SO4(impurity)/volume liquor ratio        gpl
    double  si;             //  mass SiO2(impurity)/volume liquor ratio          gpl
    double  fe;             //  mass Fe2O3(impurity)/volume liquor ratio         gpl
    double  ox;             //  mass Na2C2O4(Oxalate)/volume liquor ratio        gpl
    double  toc;            //  mass Total Organic Carbon/volume liquor ratio    gpl
    double  gpl;            //  mass Al(OH)3/volume slurry ratio                 gpl
    double  a;              //  mass Al2O3/volume liquor ratio                   gpl =acs*cs
    double  c;              //  mass Na2O/volume liquor ratio                    gpl =cs*0.585

    double  m_BndSoda;      //  Frac bound soda
    double  m_BndSodaAsOrg; //  Frac of bound soda that is Organic
    
    //other info...       
    double m_Moment0;
    double m_Moment1;
    double m_Moment2;
    double m_Moment3;
    double m_L10;
    double m_L21;
    double m_L32;
    
    double m_Dens;
    double m_SAM;

    SMBayerBase  *m_pBm; 

    CPSDConc()
      {
      m_pBm=NULL;
      };

    void  ClearOverrides();
  };

class CPrecipPSD : public CLSoda
  {
  friend class QPrecipMain2;
  public:

  // Fortan Common blocks
    CPSDCorr       m_Corr;
    CPSDDerivsPars m_DerivsPars;
    CPSDGlobalVars m_GlobalVars;
    CPSDGridPts    m_GridPts;
    CPSDGrowCoeff  m_GrowCoeff;
    CPSDModelParms m_ModelParms;

  protected:

    SpConduit         *m_pFeed,           *m_pProd;
    SpContainer                           *m_pContent;
    SMBayerBase       *m_pFdBm,           *m_pCnBm;
    SQSzDist1         *m_pFdSz,           *m_pCnSz;
    CSD_Distribution  *m_pFdDistribution, *m_pCnDistribution;
    CSD_SpDist        *m_pFdDist,         *m_pCnDist;
    CSD_Intervals     *m_pFdInts,         *m_pCnInts;

    flag              m_bUseFdConcOvr,    m_bUseCnConcOvr;
    CPSDConc          m_FdConc,           m_CnConc;
    CPSDConc          m_FdConcOvr,        m_CnConcOvr;
    CPSDStates        m_FdState,          m_CnState;

    double            m_dFdAC,            m_dCnAC; //A to C ratio

    double            m_FdMassSlurry;


    #ifndef _RELEASE
    flag    m_bXFerOnly;
    #endif

    double            m_dResTime; //copy of DerivsPars result
    double            m_dGrowthRate; //copy of DerivsPars result
    double            m_dYield;
    double            m_dACeq; //Equilibrium A/C ratio
    double            m_dAeq;  //Equilibrium A conc (ASat)
    double            m_dIonicStrength;
                      
    double            tend, tinc;
    long              itol, loop, iopt, nout;
    double            t;

    double            m_EquilTime;

    //// BoundSoda
    //bool              m_bBoundSpcsOK;
    //bool              m_bDoBoundStuff;
    //double            m_dK1;
    //double            m_dBndOrgSoda;

    long    itask;
    double  thres[200], tlast;
    long    iwork[220];
    double  twant, rwork[3422];
    long    jt, istate;
    double  tstart;
    long    neq;
    double  tol;

    // LSoda Overrides
    int LSFun(long *neq, double *t, double *y, double *yp);
    int LSJac(long *neq, double *t, double *y, long *ml, long *mu, double *pd, long *nrowpd);

    // from discrete
    int EqnsToSolve(double *y, double *yin, double *g, double *nuc, double *agg, double *res_time__, double *grow_coeff__, double *aggl_coeff__, long *q, long *npts, double *yp);
    int PureGrowth(double *dist, double *grow_coeff__, long *npts, double *distp);
    int PureAgglom(double *dist, long *q, long *npts, double *aggl_coeff__, double *distp);
    int CalcGrowthCoeff(double *x, long *npts, long *grow_dis__, double *grow_coeff__);
    int CalcAgglomCoeff(double *x, double *akf, long *npts, double *aggl_coeff__);
    int SetGrid(double *xmin, long *q, long *npts, double *x);
    double Moment(double *dist, long j, long *npts, double *x);
    int NumberToMass(double *dist, long *npts, double *x);
    int MassToNumber(double *dist, long *npts, double *x);
    int DistSauter(double *psd, double *l32, long *npts, double *x);
    double SpecificSurfArea(double THAdens, double *dist, long *npts, double *x);

    // from Corr
    double DenLiquor (CPSDConc & Conc, double tempk);
    double Solub(CPSDConc & Conc, double tempk, double * pIonicStrength=NULL);
    double GRate(CPSDConc & Conc, double tempk, double SolConc, double SSA);
    double NRate(CPSDConc & Conc, double surf, double tempk);
    double ARate(CPSDConc & Conc, double tempk);
    double SodaC(CPSDConc & Conc, double tempk);

    // from Serv
    void Model2State(SMBayerBase *pBm, CSD_SpDist *pDist, CPSDStates & State);
    void State2Model(CPSDStates & State, SMBayerBase *pBm, CSD_SpDist *pDist);
    void State2Conc(CPSDStates & State, CPSDConc & Conc, SMBayerBase*pBm, CPSDConc* pConcOvr, bool withdbg=true);

    double DenSlurry(CPSDConc & Conc, double tempk);
    double CorrectSlurryVol25(double vs, CPSDConc & Conc,  double tempk);


  public:
    CPrecipPSD();
    long SolveIt(SpConduit & Feed, SpContainer & Content, SpConduit & Prod, bool DoTransferOut);

    long TestTransferInputs(SpConduit & Feed, SpContainer & Content, char* TestPath);

  private:
    long SolveItInit(SpConduit & Feed, SpContainer & Content);
    void InitLocals();
    int TransferInputs(bool ForTest = false);
    int TransferOutputs();
    int CalcDerivParms(CPSDStates & State, CPSDConc & Conc, CPSDConc* pConcOvr);

};

extern double NaAluminate2Alumina();
extern double NaAluminate2Caustic();

#undef DllImportExport

#endif
