//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-02-18 09:43:53 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------
// 
//   Interface for general precipitation
//
//---------------------------------------------------------------------------
class CPrecip : public MBaseMethod
{
 public:
  CPrecip(MUnitDefBase * pUnitDef, TaggedObject * pNd);
  virtual ~CPrecip(void);


 protected:
  // Params
  bool   bOnLine;             // Unit online?
  bool m_bInternalCool;       // Internal cooling
  bool m_bExternalCool;       // External cooling

  double m_dCooldT;           // Parameters for internal coolng
  double m_dCooldQ;
  double m_dCoolFlow;
  double m_dCoolArea;
  double m_dCoolHTC;
  double m_dCoolWaterFlow;
  double m_dCoolWaterTin;
  double m_dInternalCooling;  //Heat Removed by internal cooling

  double m_dExtCoolFlow;     // Parameters for external cooling
  double m_dExtCoolVolFlow;
  double m_dExtCoolTemp;     // Temperature of return stream
  double m_dExtCoolRate;     // Heat removed by external cooling
  double dTankVol;      // Tank volume (m^3)

  long iEvapMethod;     // Evaporation method
  double m_dEvapRate;

  long iGrowthRateMethod;    // Precipitation Method

  long   iThermalLossMethod;   // Other thermal losses
  double m_dTotThermalLoss;   // Total Thermal Loss 
  double m_dEnvironmentLoss;  // Environmental Heat Loss

  double m_dEvapThermalLoss;  // Evaporation Losses
  double m_dEvapMassLoss;
  

  double dReactionHeat; 
  long iRctHeatMethod;
  long iCoolMethod;

  // Results
  double dResidenceTime;
  double dYield;
  double dThermalLoss;

  double *x;         // Precipitation Rates
  double *xo;        // Old rates in iterative convergence algoritm 
  double m_dDamping;
  double m_dMassDamping;



  MStream Feed;

 protected:
  virtual void PrecipRates(MStream & Prod, double t=NAN) =0;   // Calculate the precip rates of components in Prod
  virtual void EvalLosses(MStream & Prod) =0;   // General routine for finding thermal and evaporative losses
  virtual void AdjustMasses(MStream & Prod) =0;  // Adjust the masses of the streams
  virtual void AdjustT(MStream & Prod) =0;  // Adjust the temperature for heat losses, reaction etc.
  virtual void AdjustSSA(MStream & Prod) = 0;  // Adjust the SSA
  virtual void AdjustPSD(MStream & Prod) = 0;
  virtual void DoPrecip(MStream & Prod) = 0; // Main Precip iterative solver Routine
  virtual bool ConvergenceTest() = 0;
  

};
