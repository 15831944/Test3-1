//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __CM_FLEXI_H
#define  __CM_FLEXI_H

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

#ifdef __CM_FLEXI_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

_FWDDEF(CMFFlexiPlex);

class DllImportExport CMFFlexiPlex : public CMFCommon
  {
  protected:
    /*
    CCMinFinder   *pFn;
    
    long           NumParms;       //number of parameters describing equation
    CDVector       Parms;
    CLVector       ParmHolds;
    CDVector       ParmMins;
    CDVector       ParmMaxs;
    CDVector       ParmTols;
    flag           DoParmsRange;
    
    CDVector       NormParms;
    long           NumParmsUsed;  
    */
    CDVector       Residual;

    double         StepSize, ConverTol;
    int NC, NIC;

    int I, J, II, IIN, INF, Icont, IterCnt, Ncont, IterCost, 
        K1, K2, K3, K4, K5, K6, K7, K8, K9,
        L5, L6, L7, L8, L9, Lhigh, Low, Lsec,
        N1, N2, N3, N4, N5, N6, N7, N8, N9, N;
    double ETA, Fdifer, FH, FL, Fold, FS, Fter, R1A, R2A, R3A,
        Scale, Seql, Step, Step1, Step2, Sum2;
    CDVector Sum, F, SR; // double Sum[25], F[25], SR[25];
    CDVector ROld;       // double ROld[50];
    CDMatrix X1, X2;     // double X1[25][25], X2[25][25];


    flag Restart, AbortFit;
    byte CalcErr;
    flag TraceReqd;

  public:
    CMFFlexiPlex(CCMinFinder *pFn_, double Tol=1.0e-6, int ItMax_=100);
    
    //void           SetItMax(int ItMax_=100)  { iItMax=ItMax_; };
    //void           SetTol(double Tol=1.0e-6) { dTol=Tol; };
    //double         SetRestrParms(double Alpha, CDVector & NextParms, CDVector & DeltaParms);

    void           DoIO();
    void           Start();
    void           Sumr();
    void           Feasbl();
    int            Solve();

  };

// ===========================================================================

#undef DllImportExport 

#endif
