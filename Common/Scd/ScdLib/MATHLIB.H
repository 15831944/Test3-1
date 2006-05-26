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

#if !_MANAGED

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
#endif

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

// ===========================================================================

const DWORD   TD_Time               = 0x01;
const DWORD   TD_DateLeft           = 0x02;
const DWORD   TD_DateRight          = 0x04;
const DWORD   TD_Days               = 0x08;
const DWORD   TD_DaysJoined         = 0x10;
const DWORD   TD_IntegralSecs       = 0x20;
const DWORD   TD_TrimTime           = 0x40;
const DWORD   TD_Seconds            = 0x80;

const DWORD   TD_DateTime           = TD_Time|TD_DateLeft;
const DWORD   TD_TimeDate           = TD_Time|TD_DateRight;

class DllImportExport CTimeValue
  {
  public:
    static const long   Divisor;
    static const double MinResolution;
    static const __int64 NANMask;

    CTimeValue()                        { m_mSecs = 0; };
    CTimeValue(double T)                { if (Valid(T)) Seconds = T; else m_mSecs = NANMask; };
    CTimeValue(__int64 T)               { m_mSecs = T; };
    CTimeValue(const CTimeValue & T)    { m_mSecs = T.m_mSecs; };
    CTimeValue(const time_t & T)        { m_mSecs = T*Divisor; };
    CTimeValue(LPCTSTR Str)             { m_mSecs = 0; Parse(Str); };

    __int64    getmSeconds() const      { return m_mSecs; };
    void       putmSeconds(__int64 T)   { m_mSecs = T; };
    double     getSeconds() const       { return m_mSecs / (double)Divisor; };
    void       putSeconds(double T)     { m_mSecs = __int64(T*Divisor+0.5*MinResolution); };
    __int64    getRaw() const           { return m_mSecs; };
    void       putRaw(__int64 T)        { m_mSecs = T; };
    __int64    getSecondsI64() const    { return m_mSecs / Divisor; };
    void       putSecondsI64(__int64 T) { m_mSecs = T*Divisor; };


    __declspec(property(get=getSeconds,put=putSeconds))             double  Seconds;
    __declspec(property(get=getSecondsI64,put=putSecondsI64))       __int64 SecondsI64;
    __declspec(property(get=getmSeconds,put=putmSeconds))           __int64 mSeconds;
    __declspec(property(get=getRaw,put=putRaw))                     __int64 Raw;

    int getTimemSeconds();
    int getTimeSeconds();
    int getTimeMinutes();
    int getTimeHours();
    int getTimeDays();       
    int getDateDay();    
    int getDateMonth();  
    int getDateYear();   
    int getDaysInMonth();
    int getDaysInYear();
    int getDayOfWeek();
    int getDayOfYear();

    __declspec(property(get=getTimemSeconds))                           int     TimemSeconds;
    __declspec(property(get=getTimeSeconds))                            int     TimeSeconds;
    __declspec(property(get=getTimeMinutes))                            int     TimeMinutes;
    __declspec(property(get=getTimeHours))                              int     TimeHours;
    __declspec(property(get=getTimeDays))                               int     TimeDays;
    __declspec(property(get=getDateDay))                            int     DateDay;
    __declspec(property(get=getDateMonth))                          int     DateMonth;
    __declspec(property(get=getDateYear))                           int     DateYear;
    __declspec(property(get=getDaysInMonth))                        int     DaysInMonth;
    __declspec(property(get=getDaysInYear))                         int     DaysInYear;
    __declspec(property(get=getDayOfWeek))                          int     DayOfWeek;
    __declspec(property(get=getDayOfYear))                          int     DayOfYear;

    CTimeValue & operator =(const CTimeValue & T1)      { m_mSecs=T1.m_mSecs; return *this; };
    //CTimeValue & operator =(const double T1)            { *this=CTimeValue(T1); return *this; };
    
    CTimeValue operator +(const CTimeValue & T1) const  { CTimeValue X; X.m_mSecs = m_mSecs+T1.m_mSecs; return X; };
    CTimeValue operator -(const CTimeValue & T1) const  { CTimeValue X; X.m_mSecs = m_mSecs-T1.m_mSecs; return X; };
    double     operator *(const CTimeValue & T1) const  { return Seconds*T1.Seconds; };
    double     operator /(const CTimeValue & T1) const  { return Seconds/T1.Seconds; };
    CTimeValue operator +(const double v) const         { CTimeValue X; X.m_mSecs = m_mSecs+__int64(v*Divisor); return X; };
    CTimeValue operator -(const double v) const         { CTimeValue X; X.m_mSecs = m_mSecs-__int64(v*Divisor); return X; };
    CTimeValue operator *(const double v) const         { CTimeValue X; X.m_mSecs = __int64(m_mSecs*v); return X; };
    CTimeValue operator /(const double v) const         { CTimeValue X; X.m_mSecs = __int64(m_mSecs/v); return X; };
    CTimeValue operator +=(const CTimeValue & T1)       { m_mSecs = m_mSecs+T1.m_mSecs; return *this; };
    CTimeValue operator -=(const CTimeValue & T1)       { m_mSecs = m_mSecs-T1.m_mSecs; return *this; };

    CTimeValue Round(CTimeValue & T1)                   { CTimeValue T2=((m_mSecs + T1.m_mSecs/2)/T1.m_mSecs) * T1.m_mSecs; return T2; };
    CTimeValue GreaterThan(CTimeValue & T1)             { CTimeValue T2=Max(m_mSecs, T1.m_mSecs+1); return T2; };

    bool       operator >=(const CTimeValue & T1) const { return m_mSecs>=T1.m_mSecs; };
    bool       operator > (const CTimeValue & T1) const { return m_mSecs> T1.m_mSecs; };
    bool       operator ==(const CTimeValue & T1) const { return m_mSecs==T1.m_mSecs; };
    bool       operator !=(const CTimeValue & T1) const { return m_mSecs!=T1.m_mSecs; };
    bool       operator < (const CTimeValue & T1) const { return m_mSecs< T1.m_mSecs; };
    bool       operator <=(const CTimeValue & T1) const { return m_mSecs<=T1.m_mSecs; };

    bool       operator >=(const double T1) const { return m_mSecs>=T1*Divisor; };
    bool       operator > (const double T1) const { return m_mSecs> T1*Divisor; };
    bool       operator ==(const double T1) const { return m_mSecs==T1*Divisor; };
    bool       operator !=(const double T1) const { return m_mSecs!=T1*Divisor; };
    bool       operator < (const double T1) const { return m_mSecs< T1*Divisor; };
    bool       operator <=(const double T1) const { return m_mSecs<=T1*Divisor; };

    operator double() { return getSeconds(); }

    LPCTSTR Format(DWORD Options) const;
    LPCTSTR Format(LPCTSTR CnvStr) const;
    bool    Parse(LPCTSTR Str);

  protected:
    __int64    m_mSecs; // in milliSeconds
  };

extern DllImportExport CTimeValue TimeNAN;
inline bool ValidTime(CTimeValue T) { return T!=TimeNAN; }

inline CTimeValue Min(CTimeValue T1, CTimeValue T2) { return T1.mSeconds<T2.mSeconds ? T1:T2; };
inline CTimeValue Max(CTimeValue T1, CTimeValue T2) { return T1.mSeconds>T2.mSeconds ? T1:T2; };

//===========================================================================
//
//  
//
//===========================================================================

#undef DllImportExport 

#endif  /* __MATHLIB_H */
