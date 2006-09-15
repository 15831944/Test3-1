//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-08-30 11:29:38 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#include "..\TTCommon\precip.h"
//---------------------------------------------------------------------------

//**/  #define TTDEBUG 1





// Have all the mwts constant for a particular model.

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

static const int ITERMAX = 200;

class CPrecipitator: public MBaseMethod, CPrecip
{
 public:


  CPrecipitator(MUnitDefBase * pUnitDef, TaggedObject * pNd);
  virtual ~CPrecipitator(void);

  void Init();
  void BuildDataFields();
  bool ValidateDataFields();
  void EvalProducts();
  void ClosureInfo(MClosureInfo & CI);

 protected:
  MStream Feed;
  MStream Evap;
  MStream CoolIn;
  

  void EvalLosses(MStream & Prod);   // General routine for finding thermal and evaporative losses
  void EvalPrecipRates(MStream & Prod, double t=NAN);   // Calculate the precip rates of components in Prod
  void AdjustMasses(MStream & Prod);  // Adjust the masses of the streams
  void AdjustT(MStream & Prod);  // Adjust the temperature for heat losses, reaction etc.
  void DoPrecip(MStream & Prod);
  void DoResults(MStream & Prod);
  void AdjustSSA(MStream & Prod);
  void AdjustPSD(MStream & Prod) {};

  void CoolingSanityCheck();
  
  
  

  void MassXferDamping();
  bool ConvergenceTest();
  bool m_bEvapConnected;
  bool m_bCoolerOn;
  bool bCoolIn;
  bool bCoolOut;
  


  double dSALin;       // SSA Stuff
  double dDiamin;

  double m_dOldTotDH;
  double dGrowthRate;
  double dGrowthRateFactor;
  double m_dEvapRateK;  
  double dSolPrecip;
  double dSolConc;    // Solids Concentration in outlet
  double dTHAPrecip;
  double dACin;
  double dACout;
  double m_dACeq;
  double dTin;
  double dTout;
  double dTempDropRqd;
  double dThermalLossRqd;
  double dThermalLossAmbient;
  double m_dHIn;
  double m_dHOut;
  double m_dHEvap;
  double m_dHBal;
  double NoPerSec;
  double m_dSSat;      // Supersaturation
  double dQvin;
  double dQvout;
  double m_dUA;
  double m_dLMTD;
  double m_dLiquorTin;
  double m_dLiquorTout;
  

  bool m_bByVolFlow;

  double m_dPrevFlow;   // Previous volume flow
  
  double m_dVolDamping;
  double m_dCoolOutTotHz;
  double m_dCoolWaterFlowVol;
  
  

  /// Various precipitation models
  /// White
  double ER_White;     // E/R factor for growth rate (K)
  double K_White;      // "k" factor for growth rate (um/h * (gNa2O/l)^2/(g Al2O3/l)^2)
  double gF_White;     // Extra Growth (impurity) factor (-)
  double m_dK_Soda;    // K for soda inclusion
  double m_dE_Soda;    // E for soda inclusion
   
  
  /// QAL-CAR Model

  double m_dk_TOC;  //Rate term for TOC effect on rate - current value is 0.0023
  double m_dn_s;    //exponent for soda-dependence of rate constant, current value is -1
  double m_dn_fc;   //exponent for free-caustic-dependence of rate constant, current value is -2
  double m_dn_eq;   //exponent for equilibrium-A/C-dependence of rate constant, current value is 0
  double m_dn_;     //exponent for ?
  double m_dn_sol;  //exponent for solids-dependence of precipitation rate, current value is 1
  double m_dn_ssa;  //exponent for SSA-dependence of precipitation rate, current value is 0.6
  double m_dSSA;    //model where SSA is speicifed
  double m_dSSAin;
  double m_dActivationEnergy;
  double m_dK0;    // Rate Equation Constant
  double m_dK1;    // Rate Equation Constant
  double m_dBndOrgSoda;
  
  double dQmin;
  double dQmout;
  


  bool m_bUpdateT;
  
#ifdef TTDEBUG
#include "ttdebug.h"
#endif //TTDEBUG

  bool bRB;                 // Include reaction block
  MReactionBlk  m_RB;
     
  
};

// Need these till put into SMDK
//-------------------------------------------------------------------------
// FUNCTION Gibbsite Enthalpy of Dissolution (per kJ/kg-Gibbsite).
// Correlation taken from 1995 QAL Tech report by JS McFeaters and converted to 
// MJ per unit of Gibbsite from kJ per unit of Alumina.
//-------------------------------------------------------------------------
double GibbsiteHOR(double t) { return  (412.82 + (t * 0.91166)); }
//-------------------------------------------------------------------------
// FUNCTION Boehmite Enthalpy of Dissoslution (per kJ/kg-Boehmite).
// Correlation taken from 1995 QAL Tech report by JS McFeaters and converted to 
// MJ per unit of Boemite from kJ per unit of Alumina.
//-------------------------------------------------------------------------
double  BoemiteHOR(double t) {  return (144.5 + (t * 0.868)); }



//---------------------------------------------------------------------------
