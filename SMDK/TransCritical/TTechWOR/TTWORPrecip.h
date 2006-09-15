//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-06-27 09:42:49 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"

#endif
#include "..\TTCommon\precip.h"
#include "..\TTCommon\hor.h"
#include "brahmapsd.h"
#include <sstream>
#include <iomanip>
//---------------------------------------------------------------------------



#ifdef TTDEBUG
#include "..\TTCommon\ttdbg.h"
#else
#include "..\TTCommon\ttnodbg.h"
#endif


extern const double L[];
extern const double Lav[];


// Have all the mwts constant for a particular model.
//  const double MW_Al2O3     = spAlumina.MW;          //101.961278
//  const double MW_Al2O3_3H2O= spTHA.MW;              //156.007118




static const double MW_H2O       = 18.015;            
static const double MW_NaOH      = 39.997;      
static const double MW_Alumina     = 101.961278;
static const double MW_THA       = 156.007118;              
static const double MW_Na2CO3    = 105.989;
static const double MW_OrganicSoda = 223.632;  
static const double MW_BoundSoda = MW_NaOH;
static const double MW_BoundOrganicSoda = MW_OrganicSoda;
static const double MW_Na2O = 61.9789;
static const double MW_Al2O3 = 101.96;
static const double MW_C = 12.01;
//Alpha Ratio of molecular weights (NaAl[OH]4(l)/Al[OH]3(s))
static  const double Alpha = MW_Alumina/MW_THA;

static const int ITERMAX = 200;


// Worsley Model
// Based on BRAHMA pascal code





class CPrecipitator: public MBaseMethod, CPrecip
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
  MStream Feed;
  MStream Tank;     // Effective Stream for tank, calculations done on this stream
  MStream SavedTank;  // Save the tank stream contents for efficient recalculation


  MStream Evap;
  MStream CoolIn;
  
  bool alreadySolved;  // If 


  void EvalLosses(MStream &Tank);  // General routine for thermal and evaporative losses
  void EvalPrecipRates(MStream &Tank);   // Calculate the precip rates of components in Tank
  void AdjustMasses();  // Adjust the masses of the streams
  void AdjustT();  // Adjust the temperature for heat losses, reaction etc.
  void DoPrecip();
  void DoResults();
  void AdjustSSA();
  void AdjustPSD();
  void DoClassifier(MStream &, MStream &);
  
  

  void AgglomKernel();
  void Agglomeration(double, double);
  

  bool ConvergenceTest();
  bool m_bEvapConnected;
  bool m_bCoolerOn;
  bool bCoolIn;
  bool bCoolOut;
  bool bTest;      // Try to use saved tank to restart 
  bool bVerbose;   // More verbose error messages



  bool bGrowthOn;
  bool bAgglomOn;
  bool bNuclOn;
  bool bClassOn;


  /// General Worsley Routines
  double GrowthIn( MStream &);
  double NucleationIn( MStream &);
  double Soda(MStream &);
  void Numbers(MStream &);

  BrahmaPSD m_PSDin;     // PSD in inlet stream
  BrahmaPSD m_PSD;       // PSD in tank
  BrahmaPSD m_PSD1;      // New PSD


  //double dSALin;       // SSA Stuff
  //double dDiamin;
  double dSSA;   // Specifica Surface Area, Brahma version

  double m_dOldTotDH;
  double dGrowthRate;
  double dSoda;
  double dGrowthRateFactor;
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
  static long m_lGrowthTerm;
  static long m_lNuclTerm;
  static long m_lSodaTerm;
  static double m_dActEGrowth;
  static double m_dActENucleation    ;
  static double m_dActESoda;
  static double m_dK_G  ;
  static double m_dK_N	 ;
  static double m_dn_s;
  static double m_dgamma_1;
  static double m_dgamma_2;
  static double m_dgamma_g;
  static double m_dgamma_N;
  static double m_dgamma_s;

  double   m_dNuclPlantCorr;
  double   m_dKAgglom;
  double   m_dSurfaceAct;
  double   m_dSodaConst; ;


  /// Classification Data
  long iClassMethod;
  double m_dBypass;       
  double m_dSharp_Index; 
  double m_dCut_Size;    
  double m_dSlurry_split;
  double m_dOverpass;   
  double m_dL0 ;         
  double m_dxS;
  double m_dEff;
  

  /// Tank Data
  double m_dShort_Circ;
  double m_dUFrac;


  double m_dGrowth;
  double m_dNucleation;
  //double m_dL[nClasses+1];
  //double m_dLav[nClasses+1];
  double beta[nClasses+1][nClasses+1];   // Agglomeration Kernel
  long m_lMaxAgglomClass;
  double aggl[nClasses+1];
  double agmin;                 // Number of particles lost to agglomeration

  double n[nClasses+1];         // Numbers for bulk slurry in precipitator
  double NewN[nClasses+1];      // Numbers after doing PSD calcs
  double m_dSumAr;


  double m_dNuclN;   // Number of new particles through nucleation
  double m_dAgglomN;       // Change in number of particles through agglom
     
  double m_dAcc;
  
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
  void displayPSD(BrahmaPSD &, int);
  double dd[12][30];

  long lPBType;   // PSD Model 0: none, 1: SSA, 2: PSD

#ifdef TTDEBUG
    bool bTTDebug;
    TTDbg dbg;
#endif

  

    
  
};




