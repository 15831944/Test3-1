//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CPrecipitator : public MBaseMethod
  {
  friend class CSimpleSolverFn;
  public:
    CPrecipitator(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~CPrecipitator(void);

    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ValidateDataFields();
    virtual void EvalProducts();
    virtual void ClosureInfo(MClosureInfo & CI);

  protected:
    // Params
    bool           bOnLine;
    double         dTankVol;      // Tank volume (m^3)
    long           iThermalLossMethod;
    double         dTempDropRqd;
    double         dThermalLossRqd;
    double         dThermalLossFrac;
    double         dReactionHeat;
    long           iRctHeatMethod;
    double         dHOR_a;
    double         dHOR_b;
    double         dHORDiff;
    long           iGrowthRateMethod;
    long           iSodaPrecipMethod;
    double         AggParm;      // Agglomeration parameter (-)
    double         ER_White;     // E/R factor for growth rate (K)
    double         ER_Cresswell; // E/R factor for growth rate (K)
    double         ER_WhBateman; // E/R factor for growth rate (K)
    double         K_White;      // "k" factor for growth rate (um/h * (gNa2O/l)^2/(g Al2O3/l)^2)
    double         K_Cresswell;  // "k" factor for growth rate (um/h * (gNa2O/l)^2/(g Al2O3/l)^2)
    double         K_WhBateman;  // "k" factor for growth rate (um/h * (gNa2O/l)^2/(g Al2O3/l)^2)
  	double         Tref;         // Reference temperature for growth rate (model 1 only) (K)
  	double         gF_White;     // Extra Growth (impurity) factor (-)
  	double         gF_Cresswell; // Extra Growth (impurity) factor (-)
  	double         gF_WhBateman; // Extra Growth (impurity) factor (-)
    double         KSoda_Sang;   // "k" factor for Na2O precipitation (-)
    double         KSoda_Ohkawa; // "k" factor for Na2O precipitation (-)
    double         Approach;     // Oxalate equilibrium approach (g/l)

    // Results
    double         dGrowthRateFactor;  //Growth Rate Factor (m/s)
    double         dGrowthRate;
    double         dSodaFac;     //Soda precipitation factor
    double         dShapeRatio;  //feed to prod shapefactor ratio
    double         dResidenceTime;
    double         dYield;
    double         dThermalLoss;
    double         dThermTempDrop;
		double				 dSolPrecip;   //THA solids precip (kg/s)
    double         dOccSoda;     // (kg/s)
    double         dSALin, dSALout;
    double         dTin, dTout;
    double         dDiamin, dDiamout;
    double         dACin, dACout;
    double         dASat;
    double         dQvin,dQvout;

    // Locals
    double         m_AOutEst; // AluminaConc Out
    double         m_TOutEst;
    double         m_xPrev;
    double         m_XSodaPrev;

  protected:
    void           AdjustMasses(MVector & Prod, double & x, double SodaFac);
    double         PerformAluminaSolubility(MVector & Prod, double TRqd, double ARqd, double SSat, double Cin, bool & ConvergeErr);
    void           DoPrecip(MVector & Prod);
  };

//---------------------------------------------------------------------------
