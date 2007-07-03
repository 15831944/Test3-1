//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    QAL Classifer Model. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2007-05-23 01:13:16 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#ifndef  __QALPRECIP_H
#define  __QALPRECIP_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __QALPRECIP_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

#include <sstream>
#include <iomanip>


#include "QALPSD.h"
#include "..\TTCommon\precip.h"
#include "..\TTCommon\hor.h"


//**/ #define TTDEBUG



#ifdef TTDEBUG
#include "..\TTCommon\ttdbg.h"
#else
#include "..\TTCommon\ttnodbg.h"
#endif



//---------------------------------------------------------------------------
static const double MW_H2O       = 18.015;            
static const double MW_NaOH      = 39.997;      
static const double MW_Alumina     = 118.001;
static const double MW_THA= 78.084;              
static const double MW_Na2CO3    = 105.989;
static const double MW_OrganicSoda = 223.632;  
static const double MW_BoundSoda = MW_NaOH;
static const double MW_BoundOrganicSoda = MW_OrganicSoda;
static const double MW_Na2O = 61.9789;
static const double MW_Al2O3 = 101.96;
static const double MW_C = 12.01;
//Alpha Ratio of molecular weights (NaAl[OH]4(l)/Al[OH]3(s))
static  const double Alpha = MW_Alumina/MW_THA;

const static int qVal = 1;
const double t0 = 323.15;  // Reference temperature for kinetic rate
const double bayerRefTemp = 273.15+25.0;   // Bayer liquor reference temperature


double clAE(double);
double wtSub45(MStream &);








class CPrecipitator : public MBaseMethod, CPrecip
{
 public:


  CPrecipitator(MUnitDefBase *pUnitDef,TaggedObject * pNd);
  virtual ~CPrecipitator(void);

  void Init();
  void BuildDataFields();
  bool ValidateDataFields();
  void EvalProducts();
  void ClosureInfo(MClosureInfo & CI);

 protected:
  MStreamI Feed;
  MStreamI Tank;     // Effective Stream for tank, calculations done on this stream

  MStreamI Evap;
  MStreamI CoolIn;
  
  bool alreadySolved;


  void EvalLosses(MStream & Prod);   // General routine for finding thermal and evaporative losses
  void EvalPrecipRates(MStream & Prod, double t=NAN);   // Calculate the precip rates of components in Prod
  void AdjustMasses(MStream & Prod);  // Adjust the masses of the streams
  void AdjustT(MStream & Prod);  // Adjust the temperature for heat losses, reaction etc.
  void DoPrecip(MStream &);
  void DoResults(MStream & Prod);
  void AdjustSSA(MStream & Prod);
  void AdjustPSD(MStream & Prod);

  
  void AgglomKernel();
  void BinFactors();        // Factors for splitting agglomerate between consecutive bins
  int q;                    // Bin *volume* spacing is 2.**(1/q)
  double alpha;             // Volume ratio of consecutive bins.
  int klf(int i, int j) {       // Determine which bin an agglomeration from i, j ends up in
    return ( (i>j) ? i : i+1);
  }
  
  // For QAL SysCAD model, q=1, alpha = 2
  // DeGroot original examples, q = 5

  bool ConvergenceTest();
  bool m_bEvapConnected;
  bool m_bCoolerOn;
  bool bCoolIn;
  bool bCoolOut;


  bool bGrowthOn;     // Growth
  bool bAgglomOn;     // Agglom
  bool bNuclOn;       // Nucleation
  bool bAttritionOn;


  /// General Precipitation Routines
  void PBRates();  // Calculate all the rates
  double AgglomRate();       //Number per kg solids /hour
  double GrowthRate();      // Microns/hour
  double NucleationRate();  // Number per kg slurry per hour
  void AttritionRate();
  void  Growth();  
  void Nucleation();
  void Agglomeration();
  void Attrition();
  double Soda();

  void getN(MStream &);           // Get stream to local N
  void putN(MStream &);          // Put local N to stream
  

  double bsplit[nClasses][nClasses];
  double L1;    // Size of nucleation bin
   


  double dSALin;       // SSA Stuff
  double dDiamin;

  double m_dOldTotDH;
  double dGrowthRate;
  double dAgglomRate;
  double dNuclRate;
  double dSoda;

  double dNuclYield;
  double dGrowthYield;


  double dGrowthRateFactor;
  double dAgglRateFactor;
  double dNuclRateFactor;

  double m_dEvapRateK;  
  double dSolPrecip;
  double dSolConc;    // Solids Concentration in outlet
  double dTHAPrecip;
  double dAlPrecip;
  double dTin;
  double dTout;
  // Common Display stuff

  double dACin;
  double dACout;
  double m_dACeq;
  double m_dSSat;      // Supersaturation


  double m_dHIn;
  double m_dHOut;
  double m_dHEvap;
  double m_dHBal;
  double NoPerSec;
  double dQvin;
  double dQvout;
  double m_dUA;
  double m_dLMTD;
  double m_dLiquorTin;
  double m_dLiquorTout;


  double dtact;     // Actual Time Step
  

  bool m_bByVolFlow;
  bool bKernelOK;

  
  double m_dCoolOutTotHz;
  double m_dCoolWaterFlowVol;
  
  
  //  Stuff specific to particular model
  /// Various precipitation models
  /// Worsley


  double ER_White;
  double K_White;
  double gF_White;
  double m_dActivationEnergy;
  double m_dK0;
  double m_dk_TOC;
  double m_dn_fc;
  double m_dn_eq;
  double m_dn_;
  double m_dn_s;
  double m_dn_sol;
  double m_dn_ssa;

  double m_dSSAin;   // Specific Surface Area, m^2/kg slurry in
  double m_dSSA;     // Specific Surface Area, m^2/kg slurry tank



  double    m_dKAgglom;
  double    m_dSurfaceAct;
  double    m_dSodaConst; ;


  /// Classification Data
  double m_dBy_Pass;       
  double m_dSharp_Index; 
  double m_dCut_Size;    
  double m_dSlurry_split;
  double m_dOver_pass;   
  double m_dL0 ;         

  /// Tank Data
  double m_dShort_Circ;
  double m_dUFrac;


  double m_dGrowth;
  double m_dNucleation;
  double beta[nClasses+1][nClasses+1];   // Agglomeration Kernel
  double Ndot[nClasses+1];


  QALPSD fpsd;   // Feed PSD
  

  double n[nClasses+1];         // Numbers for bulk slurry in precipitator
  double m_dVol, m_dArea, m_dNumber;  // Tank Numbers
  double NewN[nClasses +1];
  double m_dSolidsIn;
  double m_dSolidsOut;
  

  double m_dNuclN;   // Number of new particles through nucleation
  double m_dAgglomN;       // Change in number of particles through agglom
     
  double m_ddeltaT;  // Quasi - Timestep for solution
  
  double dQmin;
  double dQmout;
  double m_dsigma;

  double dFeedNTot, dProdNTot;
  


  bool m_bUpdateT;
  long m_lIterations;
  long m_lItermax;
  double dAggMinLim;

  long iDType;
  long iPSD;
  void displayStream(MStream &ms, int scrn);
  void displayPSD(MStream &ms, int scrn);
  void displayPSD(int scrn);
  double dd[12][32];

  long lPBType;   // PSD Model 0: none, 1: SSA, 2: PSD

#ifdef TTDEBUG
    bool bTTDebug;
    TTDbg dbg;
#endif




};

#endif
