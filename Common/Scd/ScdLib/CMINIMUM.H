//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __CMINIMUM_H
#define  __CMINIMUM_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif

#ifdef __CMINIMUM_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

const byte CMM_None=0;
const byte CMM_Controls=1;
const byte CMM_LawBailey=2;
const byte CMM_FlexiPlex=3;

const byte CMM_Last=CMM_FlexiPlex;
// ===========================================================================

_FWDDEF(CCMinFinder);

// ===========================================================================
// Common base to derive all Minimization methods from

class DllImportExport CMFCommon
  {
  protected:
    CCMinFinder   *pFn;
    
    long           NumParms;       //number of parameters describing equation
    long           NumParmsUsed;   //number of parameters describing equation which are not "HOLD"
    CDVector       NormParms;
    CLVector       ParmHolds;
    CDVector       ParmMins;
    CDVector       ParmMaxs;
    CDVector       ParmTols;
    flag           DoParmsRange;

    CDVector       UserParms;
  
    void           Normalise_Parms(CDVector &UParms, CDVector &NParms);
    void           DeNormalise_Parms(CDVector &UParms, CDVector &NParms);
    void           InitParameters();
  };

// ===========================================================================
// Root Finder Class 

class DllImportExport CCMinFinder
  {
  public:
    virtual int    NParms()=0;
    virtual void   InitParms(CDVector &Parms, CLVector &ParmHolds)=0;
    virtual flag   GetParmRanges(CDVector &ParmMins, CDVector &ParmMaxs);
    virtual void   GetParmWeights(CDVector &ParmWghts);
    virtual double CostFn(CDVector &Parms)=0;
    
    virtual double EvalDeriv(CDVector &Parms, CDVector &ParmTols, long i, long k );
    virtual flag   ConstructLBMatrix (CDVector &Parms, CDVector &ParmTols, CDMatrix &Z, CDVector &C);

    virtual flag   InitConvergence(double &StepSize, double &ConverTol);
    virtual int    NEqualityConstraints();
    virtual void   EqualityConstraints(CDVector &Parms, CDVector &Residual);
    virtual int    NInEqualityConstraints();
    virtual void   InEqualityConstraints(CDVector &Parms, CDVector &Residual);

    int            FindMinimum(byte Meth);
  };

//----------------------------------------------------------------------------

inline flag   CCMinFinder::GetParmRanges(CDVector &ParmMins, CDVector &ParmMaxs) 
  { ParmMins.SetAll(-1.0e38); ParmMaxs.SetAll(1.0e38); return False; };
inline void   CCMinFinder::GetParmWeights(CDVector &ParmWghts) 
  { ParmWghts.SetAll(1.0); };
    
inline double CCMinFinder::EvalDeriv(CDVector &Parms, CDVector &ParmTols, long i, long k ) 
  { return 0.0;};
inline flag   CCMinFinder::ConstructLBMatrix (CDVector &Parms, CDVector &ParmTols, CDMatrix &Z, CDVector &C) 
  { return False; };

inline flag   CCMinFinder::InitConvergence(double &StepSize, double &ConverTol) 
  { return False;};
inline int    CCMinFinder::NEqualityConstraints() 
  {return 0;};

inline void   CCMinFinder::EqualityConstraints(CDVector &Parms, CDVector &Residual) 
  {};
inline int    CCMinFinder::NInEqualityConstraints() 
  {return 0;};
inline void   CCMinFinder::InEqualityConstraints(CDVector &Parms, CDVector &Residual) 
  {};

// ===========================================================================

#undef DllImportExport 

#endif

