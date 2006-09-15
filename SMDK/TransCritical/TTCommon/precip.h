//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-07-07 12:10:33 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================


#ifndef PRECIP_H

#define PRECIP_H

const int MAXCOMP = 8; // Number of components participating in precipitation


//---------------------------------------------------------------------------
// 
//   Interface for general precipitation
//   Should only contain generic stuff related to *any* precip process,
//   not necessarily Alumina. Thus alumina specific stuff - A/C, supersaturation
//   etc should be in derived classes.
//---------------------------------------------------------------------------
class CPrecip
{
 public:
  CPrecip();
  virtual ~CPrecip(void);


 protected:
  // Params
  bool   bOnLine;             // Unit online?
  // Basic parameters

  double dTankVol;      // Tank volume (m^3)


  //bool m_bInternalCool;       // Internal cooling
  //bool m_bExternalCool;       // External cooling
  //
  // Parameters for internal cooling
  //
  double m_dCooldT;           // Cooling Temperature split
  double m_dCooldQ;           // Cooling heat load
  double m_dCoolFlow;         // Cooling (mass) flow
  double m_dIntCoolVolFlow;   // Cooling volume flow 
  double m_dCoolArea;         // Cooling HX area
  double m_dCoolHTC;          // Cooling HTC kW/m^2K
  double m_dCoolWaterFlow;    // Cooling Water flow
  double m_dCoolWaterTin;     // Coolant temperature in
  double m_dCoolWaterTout;    // Coolant temperature out
  double m_dIntCoolRate;      //Heat Removed by internal cooling
  //
  // Parameters for external cooling
  //
  double m_dExtCoolFlow;     // External cooling flow
  double m_dExtCoolVolFlow;  // External cooling volume flow
  double m_dExtCoolTemp;     // Temperature of return stream
  double m_dExtCoolRate;     // Heat removed by external cooling
  double m_dCoolRate;        // Total cooling rate ambient + internal + external + evap

  long iEvapMethod;     // Evaporation method
  double m_dEvapRate;   // Evaporation rate

  long iGrowthRateMethod;    // Precipitation Method

  long   iThermalLossMethod;   // Other thermal losses
  double dTempDropRqd;
  double dFixedTempRqd;
  double dThermalLossRqd;
  double dThermalLossAmbient;
  double m_dTotThermalLoss;   // Total Thermal Loss 
  double m_dEnvironmentLoss;  // Environmental Heat Loss

  double m_dEvapThermalLoss;  // Evaporation Losses
  double m_dEvapMassLoss;
  

  double dReactionHeat; 
  long iRctHeatMethod;
  long iCoolMethod;
  long iCoolType;
  




  // Results
  double dResidenceTime;
  double dYield;
  double dThermalLoss;

  double   x[MAXCOMP];         // Precipitation Rates */
  double   xo[MAXCOMP];        // Old rates in iterative convergence algoritm  */
  double m_dDamping;
  double m_dMassDamping;
  static double m_dConvergenceLimit;  // Criteria for convergence
  

};




#endif

//---------------------------------------------------------------------------
