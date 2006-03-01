//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-02-18 09:43:53 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#include "precip.h"
//---------------------------------------------------------------------------

/**/  #define TTDEBUG 0





// Have all the mwts constant for a particular model.

static  const double MW_H2O       = 18.015;            
static  const double MW_NaOH      = 39.997;      
static  const double MW_Alumina     = 118.001;
static  const double MW_THA= 78.084;              
static  const double MW_Na2CO3    = 105.989;
static  const double MW_OrganicSoda = 223.632;  
static  const double MW_BoundSoda = MW_NaOH;
static  const double MW_BoundOrganicSoda = MW_OrganicSoda;
static  const double MW_Na2O = 61.9789;
static  const double MW_Al2O3 = 101.96;
static  const double MW_C = 12.011;
//Alpha Ratio of molecular weights (NaAl[OH]4(l)/Al[OH]3(s))
static  const double Alpha = MW_Alumina/MW_THA;

static const int ITERMAX = 200;

class CPrecipitator: public CPrecip
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
  MStream Evap;
  

  void EvalLosses(MStream & Prod);   // General routine for finding thermal and evaporative losses
  void AdjustMasses(MStream & Prod);  // Adjust the masses of the streams
  void AdjustT(MStream & Prod);  // Adjust the temperature for heat losses, reaction etc.
  void PrecipRates(MStream & Prod, double t=NAN);   // Calculate the precip rates of components in Prod
  void DoPrecip(MStream & Prod);
  void DoResults(MStream & Prod);
  void AdjustSSA(MStream & Prod);
  void AdjustPSD(MStream & Prod) {};
  
  

  void MassXferDamping();
  bool ConvergenceTest();

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

  

  /// Various precipitation models
  /// White
  double ER_White;     // E/R factor for growth rate (K)
  double K_White;      // "k" factor for growth rate (um/h * (gNa2O/l)^2/(g Al2O3/l)^2)
  double gF_White;     // Extra Growth (impurity) factor (-)

  
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
  double m_dPoisonFactor;  // MJM
  double m_dBndOrgSoda;
  


  bool m_bUpdateT;
  
#if TTDEBUG
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
