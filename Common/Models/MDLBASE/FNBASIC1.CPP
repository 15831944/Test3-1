//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __FNBASIC1_CPP

#include "sc_defs.h"
#include "fnbasic1.h"
#include "errorlog.h"

//===========================================================================

IMPLEMENT_CLASSFACTORY0(CFn1_Base)
IMPLEMENT_CLASSFACTORY0(CFn2_Base)

//===========================================================================
// Required for Factory Macro's
#define DllImportExport

//===========================================================================
//
//
//
//===========================================================================

flag CFn1_Linear::Parse(int NParms, char ** f) 
  {
  if (NParms>=3)
    {
    a = SafeAtoF(f[1]); 
    b = SafeAtoF(f[2]); 
    if (NParms>=4)
      mn = SafeAtoF(f[3]); 
    if (NParms>=5)
      mx = SafeAtoF(f[4]); 
    return f[2]!=NULL; 
    }
  LogError("Linear Fn", 0, "Invalid Number of Parmameters");
  return False;
  };

//--------------------------------------------------------------------------

IMPLEMENT_CLASSBUILDER0(CFn1_Linear, CFn1_Base, "Linear");

//===========================================================================
//
//
//
//===========================================================================

flag CFn1_Quadratic::Parse(int NParms, char ** f) 
  {
  if (NParms>=4)
    {
    a = SafeAtoF(f[1]); 
    b = SafeAtoF(f[2]); 
    c = SafeAtoF(f[3]); 
    if (NParms>=5)
      mn = SafeAtoF(f[4]); 
    if (NParms>=6)
      mx = SafeAtoF(f[5]); 
    return f[3]!=NULL; 
    }
  LogError("Quadratic Fn", 0, "Invalid Number of Parmameters");
  return False;
  };

//--------------------------------------------------------------------------

IMPLEMENT_CLASSBUILDER0(CFn1_Quadratic, CFn1_Base, "Quadratic");

//===========================================================================
//
//
//
//===========================================================================

flag CFn1_Cubic::Parse(int NParms, char ** f) 
  {
  if (NParms>=5)
    {
    a = SafeAtoF(f[1]); 
    b = SafeAtoF(f[2]); 
    c = SafeAtoF(f[3]); 
    d = SafeAtoF(f[3]); 
    if (NParms>=5)
      mn = SafeAtoF(f[4]); 
    if (NParms>=6)
      mx = SafeAtoF(f[5]); 
    return f[3]!=NULL; 
    }
  LogError("Cubic Fn", 0, "Invalid Number of Parmameters");
  return False;
  };

//--------------------------------------------------------------------------

IMPLEMENT_CLASSBUILDER0(CFn1_Cubic, CFn1_Base, "Cubic");

//===========================================================================
//
//
//
//===========================================================================

int CFn1_TSpline::SetParm(char *Fld, flag Start)
  {
  char *p=strchr(Fld, '=');
  if (p)
    {
    *p=0;
    p++;
    }
  XStrTrim(Fld);
  byte Type;
  double Slp;

  if ((_stricmp(Fld, "Sig")==0) && p!=NULL)
    {
    Slp=SafeAtoF(p);
    Spln.SetSigma(Slp);
    return 1;
    }

  if (_stricmp(Fld, "Free")==0)
    Type=TSpline::iSlopeFree;
  else if (_stricmp(Fld, "Seg")==0)
    Type=TSpline::iSlopeOfSegment;
  else if ((_stricmp(Fld, "Slp")==0) && p!=NULL)
    {
    Type=TSpline::iSlopeSpecd;
    Slp=SafeAtoF(p);
    }
  else
    {
    LogError("TSpline Fn", 0, "Bad Parmameter");
    return 0;
    }

  if (Start)
    {
    Spln.SetSlopeLo(Type, Slp);
    Spln.SetSlope1(Type, Slp);
    }
  else
    {
    Spln.SetSlopeHi(Type, Slp);
    Spln.SetSlopeN(Type, Slp);
    }

  return 2;
  }

//--------------------------------------------------------------------------

flag CFn1_TSpline::Parse(int NParms, char ** f)
  {
  if (NParms<6)
    return False;
 
  int fi=1; // Skip "SPLINE"
  
  if (SetParm(f[fi++], True)==0) // in case of Sigma being Specd.
    return False;
  if (SetParm(f[fi++], True)==0)
    return False;
  
  while (fi<NParms-1)
    {
    Spln.AddPt(SafeAtoF(f[fi]), SafeAtoF(f[fi+1]));
    fi+=2;
    }
  
  if (fi!=NParms-1)
    return False;
  if (SetParm(f[fi], False)==0)
    return False;

  //check that x increases
  double x=-1.0e20;
  double y=1.0e20;
  for (int i=0; i<Spln.NPts(); i++)
    {
    if (Spln.X[i]<=x)
      return False;
    //if (Spln.Y[i]>y)
    //  return False;
    x=Spln.X[i];
    y=Spln.Y[i];
    }
  
  if (!Spln.Fit())
    {
    LogError("TSpline Fn", 0, "Data not Fitted");
    return False;
    }
  return True;

  };

//--------------------------------------------------------------------------

IMPLEMENT_CLASSBUILDER0(CFn1_TSpline, CFn1_Base, "TSpline");

//===========================================================================
//
//
//
//===========================================================================
