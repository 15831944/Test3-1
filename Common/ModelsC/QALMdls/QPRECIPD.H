//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QPRECIPD_H
#define  __QPRECIPD_H

#define WithSzStuff 0
#if WithSzStuff
#define SIZEDST1
#endif
#include "Models.h"

#ifdef __QPRECIPD_CPP
  #define DllImportExport DllExport
#elif !defined(QALMDLS)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// -------------------------------------------------------------------------

const int PF_Ignore=0;
const int PF_ConstG=1;
const int PF_ConstD=2;
const int PF_LinearG=3;
const int PF_LinearD=4;

class PrecipFn
  {
  public:
    // Common
    byte   iType;
    // Constant
    double C;
    // Linear
    double A, B;   
  
    PrecipFn()
      {
      iType=PF_Ignore;
      C=0.0;
      A=B=0.0;   
      };
  };

// -------------------------------------------------------------------------

class PrecipFnArray : public CArray <PrecipFn, PrecipFn&> {};

// ==========================================================================

DEFINE_TAGOBJ(QPrecipD);
class DllImportExport QPrecipD : public MN_Surge
  {
  public:
    // Params
    static byte    iMethod; //global
    byte           iSecSplitMethod;
    flag           bOnLine;
    double         TankVol;
    double         ActivationEnergy;
    double         K0;            // Rate Equation Constant
    double         PoisonFactor;  //
    double         PercM45;       // % < 45 Microns
    double         ACin;
    double         ACeq;
    double         ACout;
    double         ThermalConst;
    double         AmbientTemp;
    double         SplitRqd;

    // new method Params
    double         k_TOC;  //Rate term for TOC effect on rate - current value is 0.0023
    double         n_s;    //exponent for soda-dependence of rate constant, current value is -1
    double         n_fc;   //exponent for free-caustic-dependence of rate constant, current value is -2
    double         n_eq;   //exponent for equilibrium-A/C-dependence of rate constant, current value is 0
    double         n_;     //exponent for ?
    double         n_sol;  //exponent for solids-dependence of precipitation rate, current value is 1
    double         n_ssa;  //exponent for SSA-dependence of precipitation rate, current value is 0.6
    double         n_s2;   //exponent for soda-dependence of rate constant, current value is ?
    double         n_fc2;  //exponent for free-caustic-dependence of rate constant, current value is ?
    double         Damping;//damping factor
    double         PrecRateDamping;
    byte           MaxACIter;
    double         dMaxT;  //max temp for ranging
    double         dMinC;  //min caustic for ranging
    double         Sol;
    double         S_out;


    // Results
    double         SSA;
    double         TSA;
    double         Yield;
    double         BoundSoda;
    double         IonicStrength;
    double         Aeq;
    double         ResidenceTime;
    double         dSolPrecip;
    double         dThermalLoss;
    double         dExtraDuty;
    flag           bWithHOR;
    double         dReactionHeat;
    double         dPrevTout;
    
    flag           fBoundSpcsOK;
    flag           fDoBoundStuff;
    double         dK1;
    double         dBndOrgSoda;
    double         dSSAAdj;

    // Locals
    double         dPrecipRate;

    // Size Distribution Stuff
    PrecipFnArray  Fn;

    QPrecipD(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QPrecipD();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalSteadyState();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);

    virtual void   ClosureInfo();

    // ConditionBlk Override
    DEFINE_CI(QPrecipD, MN_Surge, 16);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
