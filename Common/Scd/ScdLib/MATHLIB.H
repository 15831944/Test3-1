//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __MATHLIB_H
#define  __MATHLIB_H

#include "..\..\..\SMDK\Include\md_share1.h"

#if defined(__MATHLIB_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#ifndef __AFXTEMPL_H__
//#include <afxtempl.h> // "xafxtempl.h"  
#endif

//===========================================================================
//
//  
//
//===========================================================================

inline DWORD FPP_EnableExceptions()                               
  { 
  DWORD CW=_controlfp(0,0);                                                 // Get Old fpControl
  _clearfp();                                                               // clear any outstanding exceptions
  _controlfp(/*_EM_ZERODIVIDE|_EM_OVERFLOW|*/_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);	//Set New fpControl
  return CW;
  };

inline DWORD FPP_DisableExceptions()                               
  { 
  DWORD CW=_controlfp(0,0);     // Get Old fpControl
  _clearfp();                   // clear any outstanding exceptions
  _controlfp(_MCW_EM, _MCW_EM);	// Disable Exceptions;
  return CW;
  };

inline DWORD FPP_RestoreExceptions(DWORD CW)                               
  { 
  _clearfp();                       // clear any outstanding exceptions
  return _controlfp(CW, _MCW_EM);	  // Restore fpControl
  };

//extern double Normal (int size, double *vector);
DllExport double Pow(double base, int exponent);

DllExport UINT FindNextPrimeNumber(UINT n);
DllExport int FindNextPrimeNumber(int n);
DllExport long FindNextPrimeNumber(long n);

/*#F:Find the cube roots of a Cubic equation*/
typedef double CubeRoots[3];
DllExport long FindNCubeRoots(double b, double c, double d, CubeRoots x);
DllExport double FindLargestCubeRoot(double a1, double a2, double a3);

//===========================================================================
//
//  
//
//===========================================================================
//Ascii Octal to int 
inline int aotoi(const char* p)
  {
  DWORD O=0;
  if (p)
    while (*p>='0' && *p<='7')
      {
      O*=8;
      O+=(*p)-(char)'0';
      p++;
      }
  return O;
  }

// -----------------------------------------------------------

inline int SafeAOtoI(const char * Str, int Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str))// || *Str=='-' || *Str=='+')
      return aotoi(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

DllExport double ParseNANFlag(const char * Str, double Val=dNAN);

inline double SafeAtoF(const char * Str, double Default=0.0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (*Str=='*')
      return ParseNANFlag(Str);
    if (isdigit(*Str) || *Str=='-' || *Str=='+' || *Str=='.')
      return atof(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

inline long SafeAtoL(const char * Str, long Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str) || *Str=='-' || *Str=='+')
      return atol(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

inline int SafeAtoI(const char * Str, int Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str) || *Str=='-' || *Str=='+')
      return atoi(Str);
    }
  return Default;
  }

//---------------------------------------------------------------------------

inline int SafeAlphatoI(const char * Str, int Default=0, int Base=26)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (Str && isalpha(*Str) && (toupper(*Str)-(char)'A')<Base)
      {
      const char* p = Str;
      int O = 0;
      while (isalpha(*Str) && (toupper(*Str)-(char)'A')<Base)
        {
        O *= Base;
        O += (char)toupper(*Str)-(char)'A'+1;
        Str++;
        }
      return O;
      }
    }
  return Default;
  }

//===========================================================================
//
//  
//
//===========================================================================

inline flag ConvergedDV(double D, double V, double ATol, double RTol, double & Err)
  {
  const double Vt = Max(fabs(V), fabs(V + D));
  Err = fabs(D) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }

inline flag ConvergedVV(double V1, double V2, double ATol, double RTol, double & Err)
  {
  const double Vt = Max(fabs(V1), fabs(V2));
  Err = fabs(V2-V1) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }

inline flag ConvergedDV_NANOK(double D, double V, double ATol, double RTol, double & Err)
  {
  bool f1=Valid(D);
  bool f2=Valid(V);
  if (f1 && f2)
    {
    const double Vt = Max(fabs(V), fabs(V + D));
    Err = fabs(D) / (ATol + Vt * RTol);
    return (Err < 1.0);
    }
  if (f1==f2)
    {
    Err=0.1;
    return true;
    }
  Err=10;
  return true;
  }

inline flag ConvergedVV_NANOK(double V1, double V2, double ATol, double RTol, double & Err)
  {
  bool f1=Valid(V1);
  bool f2=Valid(V2);
  if (f1 && f2)
    {
    const double Vt = Max(fabs(V1), fabs(V2));
    Err = fabs(V2-V1) / (ATol + Vt * RTol);
    return (Err < 1.0);
    }
  if (f1==f2)
    {
    Err=0.1;
    return true;
    }
  Err=10;
  return true;
  }

//===========================================================================
//
//  
//
//===========================================================================

const int MaxSigmaXn=3;
const int MaxSigmaXnY=2;

class DllImportExport LeastSquareFit
  {
  protected:
    int       m_N;
    double    m_Xn[MaxSigmaXn];
    double    m_XnY[MaxSigmaXnY];
  public:

    LeastSquareFit() { Initialise(); }
    void    Initialise();
    void    AddPt(double X, double Y);
    void    RemovePt(double X, double Y);
    double  Param(int iParam, int iOrder=1);

  };

//===========================================================================
//
//  
//
//===========================================================================

#undef DllImportExport 

#endif  /* __MATHLIB_H */
