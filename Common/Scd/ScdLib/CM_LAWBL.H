//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __CM_LAWBL_H
#define  __CM_LAWBL_H

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

#ifdef __CM_LAWBL_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

_FWDDEF(CMFLawBailey);

class DllImportExport CMFLawBailey : public CMFCommon
  {
  protected:
    CDVector       NParmMins, NParmMaxs;

    CDVector       DeltaParms;
    double         Sa; 

    int            iIter;
    int            iItMax;
    double         dTol;
  //long N;          //number of points (Y values with corresponding X's
  //long NIV;        //number of independent variables (X's)

  public:
    CMFLawBailey(CCMinFinder *pFn_, double Tol=1.0e-6, int ItMax_=100);
    
    //void           SetItMax(int ItMax_=100)  { iItMax=ItMax_; };
    //void           SetTol(double Tol=1.0e-6) { dTol=Tol; };
    double         SetRestrParms(double Alpha, CDVector & NextParms, CDVector & DeltaParms);
    flag           ConstructLBMatrix(CDVector &NParms, CDVector &ParmTols, CDMatrix &Z, CDVector &C);
    int            Solve();
  };

// ===========================================================================

#undef DllImportExport 

#endif

