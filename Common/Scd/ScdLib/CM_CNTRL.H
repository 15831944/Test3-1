//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __CM_CNTRL_H
#define  __CM_CNTRL_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif

#ifndef __CMINIMUM_H
#include "cminimum.h"
#endif

#ifdef __CM_CNTRL_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

_FWDDEF(CMFControls);

class DllImportExport CMFControls : public CMFCommon
  {
  protected:

    CDVector       DeltaParms;
    double         Sa; 

    int            iIter;
    int            iItMax;
    double         dTol;

  public:
    CMFControls(CCMinFinder *pFn_, double Tol=1.0e-6, int ItMax_=100);
    
    double         SetRestrParms(double Alpha, CDVector & NextParms, CDVector & DeltaParms);
    flag           ConstructLBMatrix(CDVector &NParms, CDVector &ParmTols, CDMatrix &Z, CDVector &C);
    int            Solve();
  };

// ===========================================================================

#undef DllImportExport 

#endif

