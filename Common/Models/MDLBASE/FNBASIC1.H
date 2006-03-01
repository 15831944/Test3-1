//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FNBASIC1_H
#define  __FNBASIC1_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __SCDARRAY_H
  #include "scdarray.h"
#endif  
#ifndef __TSPLINE_H
  #include "tspline.h"
#endif

#if defined(__FNBASIC1_CPP)
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn1_Base
  {
  public:
    CFn1_Base(){};
    virtual ~CFn1_Base(){};
    virtual flag Parse(int NParms, char ** f)=0;
    virtual double Value(double Iv1)=0;
    virtual double MinIndVar()=0;
    virtual double MaxIndVar()=0;
  };

DEFINE_CLASSFACTORY0(CFn1_Base);

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn2_Base
  {
  public:
    CFn2_Base(){};
    virtual ~CFn2_Base(){};
    virtual flag Parse(int NParms, char ** f)=0;
    virtual double Value(double Iv1, double Iv2)=0;
    virtual double MinIndVar1()=0;
    virtual double MaxIndVar1()=0;
    virtual double MinIndVar2()=0;
    virtual double MaxIndVar2()=0;
  };

DEFINE_CLASSFACTORY0(CFn2_Base);

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn1_Const: public CFn1_Base
  {
  protected:
    double a, mn, mx;
  public:
    CFn1_Const() { a=0.0; };
    virtual ~CFn1_Const() {};
    virtual flag Parse(int NParms, char ** f) 
      {
      a = SafeAtoF(f[1]); 
      if (NParms>=3)
        mn = SafeAtoF(f[2]); 
      if (NParms>=4)
        mn = SafeAtoF(f[3]); 
      return (NParms>=2); 
      };
    virtual double Value(double Iv1) { return a; };
    virtual double MinIndVar() { return mn; };
    virtual double MaxIndVar() { return mx; };
  };

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn1_Linear : public CFn1_Base
  {
  protected:
    double a,b, mn, mx;
  public:
    CFn1_Linear() { a=b=0.0; mn=-1e99; mx=1e99; };
    virtual ~CFn1_Linear() { };
    virtual flag Parse(int NParms, char ** f);
    virtual double Value(double Iv1) { return a*Iv1 + b; };
    virtual double MinIndVar() { return mn; };
    virtual double MaxIndVar() { return mx; };
  };

DEFINE_CLASSBUILDER0(CFn1_Linear, CFn1_Base);

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn1_Quadratic : public CFn1_Base
  {
  protected:
    double a,b,c,mn,mx;
  public:
    CFn1_Quadratic() { a=b=c=0.0; mn=-1e99; mx=1e99; };
    virtual ~CFn1_Quadratic() { };
    virtual flag Parse(int NParms, char ** f);
    virtual double Value(double Iv1) { return a*Iv1*Iv1 + b*Iv1 +c; };
    virtual double MinIndVar() { return mn; };
    virtual double MaxIndVar() { return mx; };
  };

DEFINE_CLASSBUILDER0(CFn1_Quadratic, CFn1_Base);

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn1_Cubic : public CFn1_Base
  {
  protected:
    double a,b,c,d,mn,mx;
  public:
    CFn1_Cubic() { a=b=c=d=0.0; mn=-1e99; mx=1e99; };
    virtual ~CFn1_Cubic() { };
    virtual flag Parse(int NParms, char ** f);
    virtual double Value(double Iv1) { return a*Iv1*Iv1*Iv1 + b*Iv1*Iv1 + c*Iv1 + d; };
    virtual double MinIndVar() { return mn; };
    virtual double MaxIndVar() { return mx; };
  };

DEFINE_CLASSBUILDER0(CFn1_Cubic, CFn1_Base);

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn1_TSpline : public CFn1_Base
  {
  protected:
    TSpline       Spln;
  public:
    CFn1_TSpline() { }; 
    virtual ~CFn1_TSpline() { }; 

    virtual flag Parse(int NParms, char ** f);
    virtual double Value(double Iv1) { return Spln.Yx(Iv1); };
    virtual double MinIndVar() { return Spln.XPt(0); };
    virtual double MaxIndVar() { return Spln.XPt(Spln.NPts()-1); };
  
  private:
    int SetParm(char *Fld, flag Start);
  };

DEFINE_CLASSBUILDER0(CFn1_TSpline, CFn1_Base);

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CFn2_Const: public CFn2_Base
  {
  protected:
    double a, mn1, mx1, mn2, mx2;
  public:
    CFn2_Const() { a=0.0; mn1=mn2=-1e99; mx1=mx2=1e99; };
    virtual ~CFn2_Const() { };
    flag Parse(int NParms, char ** f) 
      {
      a = SafeAtoF(f[1]); 
      if (NParms>=3)
        mn1 = SafeAtoF(f[2]); 
      if (NParms>=4)
        mx1 = SafeAtoF(f[3]); 
      if (NParms>=5)
        mn2 = SafeAtoF(f[4]); 
      if (NParms>=6)
        mx2 = SafeAtoF(f[5]); 

      return NParms>=2; 
      };
    double Value(double Iv1, double Iv2) { return a; };
    virtual double MinIndVar1() { return mn1; };
    virtual double MaxIndVar1() { return mx1; };
    virtual double MinIndVar2() { return mn2; };
    virtual double MaxIndVar2() { return mx2; };
  };

// ===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport 

#endif
