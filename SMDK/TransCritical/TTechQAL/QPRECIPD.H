//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QPRECIPD_H
#define  __QPRECIPD_H

#define SIZEDST1
#include "Models.h"

#ifdef __QPRECIPD_CPP
  #define DllImportExport DllExport
#elif !defined(QALMDLS)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// -------------------------------------------------------------------------

const PF_Ignore=0;
const PF_ConstG=1;
const PF_ConstD=2;
const PF_LinearG=3;
const PF_LinearD=4;

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
//typedef CSCDList <pPrecipFn, pPrecipFn> JoinPtrList;
//typedef CSCDPtrListIter<JoinPtrList, pPrecipFn> JoinIter;
#if !DotNetAfxTempl
inline void AFXAPI ConstructElements(PrecipFn *pRec, int nCount)
  { for ( int i = 0; i < nCount; i++, pRec++ ) pRec->PrecipFn::PrecipFn(); };
inline void AFXAPI DestructElements(PrecipFn *pRec, int nCount)
  { for ( int i = 0; i < nCount; i++, pRec++ ) pRec->PrecipFn::~PrecipFn(); };
#endif
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
    double         PoisonFactor;  // MJM
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
    virtual void   EvalJoinPressures();
    virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalProducts();

    virtual int    SumNonLinkFlows(SpModel &RefMdl, SpMArray &SpMIn, double &HtIn, SpMArray &SpMOut, double &HtOut);

    // ConditionBlk Override
    DEFINE_CI(QPrecipD, MN_Surge, 16);
   private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
