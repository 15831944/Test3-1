//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PRECIP_H
#define  __PRECIP_H

#include "Models.h"

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif

#ifdef __PRECIP_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Precip);
class DllImportExport Precip : public MN_Surge
  {
  public:
    // Params
    flag           bOnLine;
    double         TankVol;      // Tank volume (m^3)
		byte					 fMode;        // Precip Yield Method
    byte           fFLPostPrecip;// 
    byte           iThermalLossMethod;
    double         TempDropRqd;
    double         ThermalLossRqd;
    double         ThermalLossFrac;
    //double         m_dReactionHeat;
    double         m_StdHfFd0;
    double         m_StdHfPr0;
    double         m_StdHOR0;
    double         m_MdlHfFd0;
    double         m_MdlHfPr0;
    double         m_MdlHOR0;
    double         m_UsedHOR0;
    double         m_StdHfFdT;
    double         m_StdHfPrT;
    double         m_StdHORT;
    double         m_MdlHfFdT;
    double         m_MdlHfPrT;
    double         m_MdlHORT;
    double         m_UsedHORT;
    byte           iRctHeatMethod;
    double         dHOR_a;
    double         dHOR_b;
    double         dHORDiff;

    // Params (method 1)
    double         ER_1;         // E/R factor for growth rate (K)
    double         K_1;          // Rate Equation Constant
    double         Ksoda_1;      // Occluded soda values
    // Params (method 2)
    byte           iGrowthRateMethod;
    byte           iSodaPrecipMethod;
  	double	       FeedShapeFact;// solids shape factor
  	double	       ProdShapeFact;// solids shape factor
    //double         LossFac;      // Heat loss factor (fraction of sensible heat input)
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
    static CToleranceBlock sm_AOutTol;

    Precip(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Precip (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   ClosureInfo();

    // ConditionBlk Override
    DEFINE_CI(Precip, MN_Surge, 16);

		static CToleranceBlock sm_SolTol;
    void          DoPrecip(SpConduit & Qp);
    void          AdjustMasses(SpConduit & Qp, MIBayer & Bm, double & x, double SodaFac);
		double				PerformAluminaSolubility(SpConduit & Qp, MIBayer & Bm, double TRqd, double ARqd, double THARqd, double NoPerSec, double Na2OFac, double SSat, double Cin, flag & ConvergeErr);

   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif


