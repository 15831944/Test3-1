//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __TSPLINE_H
#define  __TSPLINE_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __DATACNVS_H
#include "datacnvs.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif  

#ifdef __TSPLINE_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================

class DllImportExport TSpline 
  {
  public:
    static const byte iSlopeFree;
    static const byte iSlopeOfSegment;
    static const byte iSlopeSpecd;

  protected:
    long           CurSeg;
    double         dSigma;
    byte           iSlpLo, iSlpHi, iSlpN, iSlp1;
    double         dSlpLo, dSlpHi, dSlpN, dSlp1;
    //TSplinePts   & Pt;
  
  public:
    CDVector       X, Y, T;
    
  public:
    TSpline();
    ~TSpline();
    void Clear();
    TSpline& operator=(TSpline * pSp);
    
    void SetSigma(double Sig) { dSigma=Sig; };
    void SetSlopeLo(byte Type, double Slp) { iSlpLo=Type; dSlpLo=Slp; };
    void SetSlope1 (byte Type, double Slp) { iSlp1 =Type; dSlp1 =Slp; };
    void SetSlopeN (byte Type, double Slp) { iSlpN =Type; dSlpN =Slp; };
    void SetSlopeHi(byte Type, double Slp) { iSlpHi=Type; dSlpHi=Slp; };
    void AddPt(double Xi, double Yi);
    double Yx(double Xi);
    flag Fit();

    int NPts() { return X.GetLen();};
    double XPt(int i) { return X[i];};
    double YPt(int i) { return Y[i];};

  };


// =========================================================================

#undef DllImportExport

#endif