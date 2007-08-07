//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __MATHLIB_CPP
#include "sc_defs.h"
#include "errorlog.h"
//#include "alphaops.h"

// ===========================================================================

const double ZeroLimit          = 1.0e-30;//the numerical limit below which all numbers are considered to be zero
const double UsableMass         = 1.0e-16;
const double TraceMass          = 0.5e-18;
const double SmallMassFrac      = 1.0e-30;

extern double gs_DisplayZeroLimit    = 1.0e-20;
extern double gs_DisplayZeroFlow     = 1.0e-6;
extern double gs_StatusVapTestFrac   = 0.99;
extern double gs_StatusSolTestFrac   = 0.9; 

//NaNs come in two flavors, "quiet" and "signaling." 
//The NaN function in the IEEE754 unit returns the "quiet" NaN, 
//which is the NaN that quietly propagates through any subsequent calculations. 
//The high-order bit of the fraction field for a "Quiet" NaN is 1, 
//while it's zero for the signaling NaN. 
//NaN values are not necessarily unique and can take on many possible values for the 
//fraction field (except 0).

union FNanValueUnion { __int32 I32; float   F; };

class CFNanValueUnion 
  {
  public: 
    CFNanValueUnion(__int32 I32)  { m_F.I32=I32; }
    operator float()              { return m_F.F; };

  protected:
    union { __int64 I32; float F; } m_F;
  };

class CDNanValueUnion 
  {
  public: 
    CDNanValueUnion(__int64 I64)  { m_D.I64=I64; }
    operator double()             { return m_D.D; };

  protected:
    union { __int64 I64; double D; } m_D;
  };


const float fQuietNAN  = CFNanValueUnion (0x7fffffff);
const float fSignalNAN = CFNanValueUnion (0x7fbfffff);
const float fNAN=fQuietNAN;

const double dQuietNAN  = CDNanValueUnion (0x7fffffff00000000);
const double dSignalNAN = CDNanValueUnion (0x7ff7ffff00000000);
const double dNAN=dQuietNAN;////dSignalNAN;  

// ===========================================================================
//find next number that might be a prime number...
UINT FindNextPrimeNumber(UINT n)
  {
  n = Max(n, (UINT)23);
  if (n%2==0)
    n++;
  while (n%2==0 || n%3==0 || n%5==0 || n%7==0 || n%11==0 || n%13==0 || n%17==0 || n%19==0)
    n += 2;
  return n;
  }

int FindNextPrimeNumber(int n)
  {
  n = Max(n, (int)23);
  if (n%2==0)
    n++;
  while (n%2==0 || n%3==0 || n%5==0 || n%7==0 || n%11==0 || n%13==0 || n%17==0 || n%19==0)
    n += 2;
  return n;
  }

long FindNextPrimeNumber(long n)
  {
  n = Max(n, (long)23);
  if (n%2==0)
    n++;
  while (n%2==0 || n%3==0 || n%5==0 || n%7==0 || n%11==0 || n%13==0 || n%17==0 || n%19==0)
    n += 2;
  return n;
  }

// ===========================================================================
// Power raises a base number to a specified power    

DllExport double Pow(double base, int exponent)
  {
  double sign,
         expon;

  if ((exponent == 0))
    return 1.;
  else if ((fabs(base) < 1.0e-100))//20))
    return 0.;
  else if (base > 0.)
    {
    expon = exponent * log(base);
    if ((expon > 690.))
      return exp(690.);
    else
      return exp(expon);
    }
  else
    {
    if ((exponent & 1) == 0) /* even */
      sign = 1;
    else  /* odd */
      sign =  - 1;

    expon = exponent * log( - base);
    if ((expon > 690.))
      return exp(690.);
    else
      return sign * exp(expon);
    }
  }         

// ===========================================================================
// Swap exchanges the values of the two parameters                      

static void dSwap (double &a, double &b)
  {
  double tempval;

  tempval = a;
  a = b;
  b = tempval;
  }

// ===========================================================================
// CubeRoot extracts the cube root of a number

static double  CubeRoot (double number)
  {
  double third;

  third = 1.0 / 3.0;

  if (number < 0)
    /* Number can be negative */

    return - exp(third * log( - number));
  else if (number > 0)
    return exp(third * log(number));
  else
    return 0.;
  } 

// ===========================================================================
// Cubic solves the general cubic equation  X^3 + B.X^2 + C.X + D = 0

DllExport long FindNCubeRoots(double b, double c, double d, CubeRoots x)
  {
  double a1, a2, p, q, rr, phi;
  int sign, i;
  /*

     newdbgln;
     writedbg('BCD');
     writedbg(rstr(B,12,2));
     writedbg(rstr(C,12,2));
     writedbgln(rstr(D,12,2));
   */
  p = (3. * c - b * b) / 3.0;
  q = (27. * d - 9. * b * c + 2. * b * b * b) / 27.;
  rr = p * p * p / 27. + q * q / 4.;
  if (rr > 0)
    {
    /* one Double root */
    a1 = CubeRoot( - q / 2. + sqrt(rr));
    a2 = CubeRoot( - q / 2. - sqrt(rr));
    x[0] = a1 + a2 - b / 3.;
    return 1;
    }
  else if (rr == 0)
    {
    /* two Double roots */
    x[0] = 2. * CubeRoot( - q / 2.) - b / 3.;
    x[1] =  - CubeRoot( - q / 2.) - b / 3.;
    if (x[0] > x[1])
      dSwap(x[0],x[1]);
    return 2;
    }
  else
    {
    /* three Double roots */
    phi = sqrt( - q * q * 27. / 4. / (p * p * p));
    phi = acos(phi);
    if (q > 0)
      sign =  - 1;
    else
      sign = 1;
    for (i = 0; i < 3; i++)
      x[i] = sign * 2. * sqrt( - p / 3.) * cos(phi / 3. + 2. *
        PI / 3. * i) - b / 3.;

    if (x[0] > x[1])
      dSwap(x[0],x[1]);
    if (x[1] > x[2])
      dSwap(x[1],x[2]);
    if (x[0] > x[1])
      dSwap(x[0],x[1]);
    return 3;
    }
  }

// --------------------------------------------------------------------------

double FindLargestCubeRoot(double a1, double a2, double a3)
  {
  double Q, R;
  double a12 = a1*a1;
  double Z;
  Q = (a12-3*a2)/9; R = (2*a1*a12-9*a1*a2+27*a3)/54;
  Z = Q*Q*Q - R*R;
  if (Z == 0.0) 
    {
    if (Q==0.0) 
      {
      return -a1/3;
      1;                     // Single root of order 3
      }
    else 
      {
      return 2*sqrt(Q)-a1/3;  //  Two Real roots, one of order 2
      }
    } 
  else if (Z < 0.0) 
    {
    double z1 = sqrt(-Z);
    double z2 = pow(z1 + fabs(R), 1.0/3.0);
    z1 = z2 + Q/z2;
    if (R>0) z1 = -z1;
    return  z1 - a1/3;    // Single real root (two complex roots)
    } 
  else 
    {
    double x, xtmp;
    double theta = R/sqrt(Q*Q*Q);
    double Q2 = -2.0*sqrt(Q);
    theta = acos(theta);
    x = Q2*cos(theta/3.0)-a1/3.0;
    xtmp = Q2*cos((theta+2*PI)/3.0)-a1/3.0;
    if (xtmp>x) x=xtmp;
    xtmp = Q2*cos((theta+4*PI)/3.0)-a1/3.0;
    if (xtmp>x) x=xtmp;
    return x;                          // Three real roots
    }                                      
  }

// --------------------------------------------------------------------------
//
//
//
// --------------------------------------------------------------------------

//    int       m_N;
//    double    m_SigmaXn[5];
//    double    m_SigmaXnY[3];

void LeastSquareFit::Initialise() 
  {
  m_N=0;
  for (int i=0; i<MaxSigmaXn; i++)
    m_Xn[i]=0.0;
  for (i=0; i<MaxSigmaXnY; i++)
    m_XnY[i]=0.0;
  };
void LeastSquareFit::AddPt(double X, double Y)
  {
  m_N++;
  m_Xn[0]+=1.0;
  m_Xn[1]+=X;
  m_Xn[2]+=X*X;
  m_XnY[0]+=Y;
  m_XnY[1]+=X*Y;
  };
void LeastSquareFit::RemovePt(double X, double Y)
  {
  m_N--;
  m_Xn[0]-=1.0;
  m_Xn[1]-=X;
  m_Xn[2]-=X*X;
  m_XnY[0]-=Y;
  m_XnY[1]-=X*Y;
  };
double LeastSquareFit::Param(int iParam, int iOrder)
  {
  switch (iOrder)
    {
    case 1:
      { 
      if (m_N>=2)
        {
        switch (iParam)
          {
          case 0: return (m_XnY[0]*m_Xn[2]-m_Xn[1]*m_XnY[1])/
                          NZ(m_Xn[0]*m_Xn[2]-Sqr(m_Xn[1]));
          case 1: return (m_Xn[0]*m_XnY[1]-m_Xn[1]*m_XnY[0])/
                          NZ(m_Xn[0]*m_Xn[2]-Sqr(m_Xn[1]));
          }
        }
      }
      break;
    }
  DoBreak();
  return dNAN;
  };

//===========================================================================
//
//
//
//===========================================================================

double ParseNANFlag(const char * Str, double Val/*=dNAN*/)
  {
  if (_stricmp(Str, "*Free")==0 || _strnicmp(Str, "*F", 2)==0) 
    return SetNANFlag(NF_Free, Val);
  if (_stricmp(Str, "*Hold")==0 || _strnicmp(Str, "*H", 2)==0) 
    return SetNANFlag(NF_Hold, Val);
  if (_stricmp(Str, "*Pulse")==0 || _strnicmp(Str, "*Pu", 3)==0) 
    return SetNANFlag(NF_Pulse, Val);
  if (_stricmp(Str, "*Ignore")==0 || _strnicmp(Str, "*I", 2)==0) 
    return SetNANFlag(NF_Ignore, Val);
  if (_stricmp(Str, "*Block")==0 || _strnicmp(Str, "*B", 2)==0) 
    return SetNANFlag(NF_Block, Val);
  if (_stricmp(Str, "*+")==0 || _strnicmp(Str, "*+", 2)==0) 
    return SetNANFlag(NF_Plus, Val);
  if (_stricmp(Str, "*-")==0 || _strnicmp(Str, "*-", 2)==0) 
    return SetNANFlag(NF_Minus, Val);
  if (_stricmp(Str, "**")==0 || _strnicmp(Str, "**", 2)==0) 
    return SetNANFlag(NF_Star, Val);
  if (_stricmp(Str, "*Pass")==0 || _strnicmp(Str, "*Pa", 3)==0) 
    return SetNANFlag(NF_Pass, Val);
  return Val;
  }

//===========================================================================
//
//
//
//===========================================================================

int aotoi(const char* p)
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

int SafeAOtoI(const char * Str, int Default)
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

double SafeAtoF(const char * Str, double Default)
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

long SafeAtoL(const char * Str, long Default)
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

int SafeAtoI(const char * Str, int Default)
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

int SafeAlphatoI(const char * Str, int Default, int Base)
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

#define UseLocal 0

const long    CTimeValue::Divisor       = 1000;
const double  CTimeValue::MinResolution = 1.0/Divisor;
const __int64 CTimeValue::NANMask       = 0x800000000000000;
CTimeValue TimeNAN(CTimeValue::NANMask);//dNAN);

void CTimeValue::putSeconds(double T)
  { 
  T=T*Divisor+0.5*MinResolution;
  if (T>_I64_MAX)
    {
    LogNote("TimeValue", 0, "Time > %6.3e seconds resetting to Half this", _I64_MAX*MinResolution);
    T=_I64_MAX/2.0;
    }
  m_mSecs = __int64(T); 
  };

//---------------------------------------------------------------------------

int CTimeValue::getTimemSeconds() { return int(mSeconds%Divisor); };
int CTimeValue::getTimeSeconds()  { return int(SecondsI64%60); };
int CTimeValue::getTimeMinutes()  { return int((SecondsI64/60)%60); };
int CTimeValue::getTimeHours()    { return int((SecondsI64/3600)%24); };
int CTimeValue::getTimeDays()     { return int(SecondsI64/86400); };
int CTimeValue::getDateDay()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    return DateTime->tm_mday;
  return -1;
  };
int CTimeValue::getDateMonth()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    return DateTime->tm_mon+1; //1=January
  return -1;
  };
int CTimeValue::getDateYear()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    return DateTime->tm_year+1900;
  return -1;
  };

// Leap Year rules 
//A year that is divisible by 4 is a leap year. (Y % 4) == 0 
//Exception to rule 1: a year that is divisible by 100 is not a leap year. (Y % 100) != 0 
//Exception to rule 2: a year that is divisible by 400 is a leap year. (Y % 400) == 0
// IS_LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )</PRE>

inline bool IsLeapYear(int Y) { return (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) ; };
static int MonthDays[12] = {31,-2,31,30,31,30,31,31,30,31,30,31};
static char* DayDescriptions[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
static char* MonthDescriptions[12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

int CTimeValue::getDaysInMonth()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    {
    if (DateTime->tm_mon==1) // Feb
      return (IsLeapYear(DateTime->tm_year+1900) ? 29:28);
    return MonthDays[DateTime->tm_mon];
    }
  return -1;
  };
int CTimeValue::getDaysInYear()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    {
    return (IsLeapYear(DateTime->tm_year+1900) ? 366:365);
    }
  return -1;
  };
int CTimeValue::getDayOfWeek()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    {
    const int d=DateTime->tm_wday; // returns 0-6 Sunday = 0; 
    //return d==0 ? 7:d; // we want 1-7 : Mon-Sun
    return d+1; // we want 1-7 : Sun-Sat
    }
  return -1;
  };
int CTimeValue::getDayOfYear()
  {
  __time64_t tt=(__time64_t)SecondsI64;
  #if UseLocal
  tm* DateTime=_localtime64(&tt);
  #else
  tm* DateTime=_gmtime64(&tt);
  #endif
  if (DateTime)
    return DateTime->tm_yday+1;
  return -1;
  };

LPCTSTR CTimeValue::getDayOfWeekDesc()
  {
  const int day = getDayOfWeek();
  if (day==-1)
    return "";
  return DayDescriptions[day-1];
  }
LPCTSTR CTimeValue::getDateMonthDesc()
  {
  const int month = getDateMonth();
  if (month==-1)
    return "";
  return MonthDescriptions[month-1];
  }

//---------------------------------------------------------------------------

#define dbgCTimeValue 0

LPCTSTR CTimeValue::Format(LPCTSTR CnvStr) const
  {
  if (CnvStr==NULL)
    CnvStr="DyTm";
  
#if dbgCTimeValue 
  dbgpln("CTimeValue::Format %s", CnvStr);
#endif

  if (strcmp(CnvStr, "s"   )==0)   return Format(TD_Seconds);

  if (strcmp(CnvStr, "Tm0" )==0)   return Format(TD_Time);
  if (strcmp(CnvStr, "Tm0.")==0)   return Format(TD_Time|TD_IntegralSecs);
  if (strcmp(CnvStr, "Tm"  )==0)   return Format(TD_Time|TD_TrimTime);
  if (strcmp(CnvStr, "Tm." )==0)   return Format(TD_Time|TD_TrimTime|TD_IntegralSecs);

  if (strcmp(CnvStr, "DyTm0" )==0)   return Format(TD_DaysJoined|TD_Time);
  if (strcmp(CnvStr, "DyTm0.")==0)   return Format(TD_DaysJoined|TD_Time|TD_IntegralSecs);
  if (strcmp(CnvStr, "DyTm"  )==0)   return Format(TD_DaysJoined|TD_Time|TD_TrimTime);
  if (strcmp(CnvStr, "DyTm." )==0)   return Format(TD_DaysJoined|TD_Time|TD_TrimTime|TD_IntegralSecs);

  if (strcmp(CnvStr, "DdTm0" )==0)   return Format(TD_Days|TD_Time);
  if (strcmp(CnvStr, "DdTm0.")==0)   return Format(TD_Days|TD_Time|TD_IntegralSecs);
  if (strcmp(CnvStr, "DdTm"  )==0)   return Format(TD_Days|TD_Time|TD_TrimTime);
  if (strcmp(CnvStr, "DdTm." )==0)   return Format(TD_Days|TD_Time|TD_TrimTime|TD_IntegralSecs);

  if (strcmp(CnvStr, "DtTm0" )==0)   return Format(TD_DateLeft|TD_Time);
  if (strcmp(CnvStr, "DtTm0.")==0)   return Format(TD_DateLeft|TD_Time|TD_IntegralSecs);
  if (strcmp(CnvStr, "DtTm"  )==0)   return Format(TD_DateLeft|TD_Time|TD_TrimTime);
  if (strcmp(CnvStr, "DtTm." )==0)   return Format(TD_DateLeft|TD_Time|TD_TrimTime|TD_IntegralSecs);

  if (strcmp(CnvStr, "Tm0Dt" )==0)   return Format(TD_DateRight|TD_Time);
  if (strcmp(CnvStr, "Tm0.Dt")==0)   return Format(TD_DateRight|TD_Time|TD_IntegralSecs);
  if (strcmp(CnvStr, "TmDt"  )==0)   return Format(TD_DateRight|TD_Time|TD_TrimTime);
  if (strcmp(CnvStr, "Tm.Dt" )==0)   return Format(TD_DateRight|TD_Time|TD_TrimTime|TD_IntegralSecs);

  return "TmFmt?";
  }

//---------------------------------------------------------------------------

LPCTSTR CTimeValue::Format(DWORD Options) const
  {
  static CString s_Buffers[100];
  static int s_iPtr=-1;
  static CCriticalSection s_Lock;
  
  s_Lock.Lock();
  s_iPtr=(s_iPtr+1)%100;
  CString &S=s_Buffers[s_iPtr];
  s_Lock.Unlock();

  CString Dt,Tm;

  if (!ValidTime(m_mSecs))
    {
    S="*";
    goto Return;
    }

  if (Options & TD_Seconds)
    {
    __int64 lSecs = SecondsI64;                       
    __int64 msecs = m_mSecs % Divisor;
    S.Format("%I64i.%03I64i", lSecs, msecs);
    goto Return;
    }

  S="?";
  __time64_t tt=(__time64_t)SecondsI64;

  tm* DateTime=_gmtime64(&tt);
  if (DateTime)
    {
    const int mday=DateTime->tm_mday;
    const int mon=DateTime->tm_mon+1;
    const int year=DateTime->tm_year+1900;
    
    __int64 lSecs = SecondsI64;                       
    __int64 days = SecondsI64/86400;
    __int64 hour = lSecs/3600;
    __int64 min  = (lSecs/60)%60;
    __int64 secs = lSecs % 60;
    __int64 msecs = m_mSecs % Divisor;

    ASSERT_ALWAYS(Options & (TD_Time|TD_DateLeft|TD_DateRight|TD_DateStrFormat), "CTimeValue::Format - No Options", __FILE__, __LINE__);

    if (Options & (TD_DateLeft|TD_DateRight|TD_DateStrFormat|TD_TimeOnly))
      {
      hour = hour%24;
      Dt.Format("%02i/%02i/%04i", mday, mon, year);
      if (Options & TD_DateStrFormat)
        Dt.Format("%i %s %04i", mday, MonthDescriptions[mon-1], year); //what is the best format??? perhaps "14-Dec-07" or ???
      else
        Dt.Format("%02i/%02i/%04i", mday, mon, year);
      }
    else
      Dt="Date";

    if (Options & TD_Days)
      Options &= ~TD_TrimTime;

    if (Options & (TD_Days|TD_DaysJoined))
      {
      hour = hour%24;
      if (Options & TD_DaysJoined)
        {
        if (Options & TD_IntegralSecs)
          Tm.Format("%I64i:%02I64i:%02I64i:%02I64i", days, hour, min, secs);
        else
          Tm.Format("%I64i:%02I64i:%02I64i:%02I64i.%03I64i", days, hour, min, secs, msecs);
        }
      else
        {
        if (Options & TD_IntegralSecs)
          Tm.Format("%I64i days %02I64i:%02I64i:%02I64i", days, hour, min, secs);
        else
          Tm.Format("%I64i days %02I64i:%02I64i:%02I64i.%03I64i", days, hour, min, secs, msecs);
        }
      }
    else
      {
      if (Options & TD_IntegralSecs)
        Tm.Format("%02I64i:%02I64i:%02I64i", hour, min, secs);
      else
        Tm.Format("%02I64i:%02I64i:%02I64i.%03I64i", hour, min, secs, msecs);
      }

    if ((Options & TD_DateLeft) && (Options & TD_Time))
      S.Format("%s %s", Dt, Tm);
    else if ((Options & TD_DateRight) && (Options & TD_Time))
      S.Format("%s %s", Tm, Dt);
    else if (Options & TD_Time)
      {
      if (Options & TD_TrimTime)
        {
        int iColon=Tm.Find(':');
        while (iColon>0)
          {
          while (Tm[0]=='0')
            Tm.Delete(0, 1);
          if (Tm[0]==':')
            Tm.Delete(0, 1);
          else
            goto Done;
          iColon=Tm.Find(':');
          }
Done:
        while (Tm.Left(2)=="00")
          Tm.Delete(0,1);
        }
      S=Tm;
      }
    else if (Options & (TD_DateLeft|TD_DateRight|TD_DateStrFormat))
      {
      S=Dt;
      }
    }

Return:

#if dbgCTimeValue 
  dbgpln("CTimeValue::Format %08x %15I64i.%03I64i > %s", Options, m_mSecs/1000, m_mSecs%1000, S);
#endif
  
  return S;
  };

//---------------------------------------------------------------------------

bool CTimeValue::Parse(/*byte Options,*/ LPCTSTR Str)
  {
  if (Str==NULL) 
    Str="";
  if (strchr(Str, '*'))
    {
    *this=TimeNAN;
    return true;
    }

  //tm t;
  //memset(&t, 0, sizeof(t));
  int year=-1;
  int mon=-1;
  int mday=-1;
  int days=0;
  int hours=0;
  int mins=0;
  int secs=0;
  //long tm_hsec=0;
  double FracSecs=0;
  char b[256], *p=b;
  strcpy(b, Str);
  int e=strlen(b)-1;
  if (e>=0 && (b[e]==':' || b[e]=='.'))
    strcat(b, "0");
  else if (e>=0 && (b[e]=='/'))
    strcat(b, "96");
  strcat(b, " ");
  int BusyTime=1, TOK=0, DOK=0;//, done=0;
  flag fTime=1, fDate=0;
  while (1)//!done)
    {
    pchar d=strpbrk(p, " :/.");
    int   l=d-p;

    if (d && isdigit(*p))
      {
      if (*d=='/')
        {
        fTime=0;
        fDate=1;
        }
      else if (*d==':')
        {
        fTime=1;
        fDate=0;
        }

      if (fTime)
        {
        BusyTime=1;
        days=hours;
        hours=mins;
        mins=secs;
        secs=atol(p);
        if (*d=='.')
          {
          p=d+1;
          FracSecs=0.0;
          int Div=10;
          for (int n=0; isdigit(*(p+n)); n++) 
            {
            FracSecs+=(*(p+n)-char('0'))/Div;
            Div*=10;
            };
          p+=n;
          fTime=0;
          }
        else
          {
          FracSecs=0;
          p=d+1;
          }
        TOK=1;
        }
      else
        {
        BusyTime=0;
        year=mon;
        mon=mday;
        mday=atol(p);
        DOK=1;
        p=d+1;
        }
      }
    else if (*p==' ')
      {
      p++;
      }
    else
      break;
    }
  if (!DOK)
    m_mSecs = (days*(__int64)86400+hours*(__int64)3600+mins*(__int64)60+secs)*Divisor+(__int64)(FracSecs*(double)Divisor);//+tm_hsec/100.0;
  else
    {
    if (mday>=1900) // swap day & year
      Exchange(mday,year);
    tm DateTime;
    memset(&DateTime, 0, sizeof(DateTime));
    DateTime.tm_year=year-1900;
    DateTime.tm_mon=mon-1;
    DateTime.tm_mday=mday;
    DateTime.tm_hour=hours;
    DateTime.tm_min=mins;
    DateTime.tm_sec=secs;
#if (_MSC_VER>=1400)
    m_mSecs = _mkgmtime64(&DateTime)*Divisor+__int64(FracSecs*Divisor);
#else
    TIME_ZONE_INFORMATION TZI;
    memset(&TZI, 0, sizeof(TZI));
    switch (GetTimeZoneInformation(&TZI))
      {
      case 0:
        m_mSecs = _mktime64(&DateTime)*Divisor+__int64(FracSecs*Divisor);
        m_mSecs -= TZI.Bias*60000;
        break;
      case 1: // Std Time
        m_mSecs = _mktime64(&DateTime)*Divisor+__int64(FracSecs*Divisor);
        m_mSecs -= TZI.Bias*60000;
        m_mSecs -= TZI.StandardBias*60000;
        break;
      case 2: // Daylight Time 
        m_mSecs = _mktime64(&DateTime)*Divisor+__int64(FracSecs*Divisor);
        m_mSecs -= TZI.Bias*60000;
        m_mSecs -= TZI.DaylightBias*60000;
        break;
      }
#endif

    //if (0)
    //  {
    //  time_t ttt= (time_t)m_mSecs /Divisor;
    //  tm DateTime1,DateTime2;
    //  memcpy(&DateTime1, gmtime(&ttt), sizeof(DateTime1));
    //  memcpy(&DateTime2, localtime(&ttt), sizeof(DateTime2));
    //  
    //  time_t ttt1=_mktime64(&DateTime1);
    //  time_t ttt2=_mkgmtime64(&DateTime1);
    //  int xxx=0;
    //  }

    }

#if dbgCTimeValue 
  dbgpln("CTimeValue::Parse       %15I64i.%03I64i < %s", m_mSecs/1000, m_mSecs%1000, Str);
#endif
  return TOK||DOK;
  };

//===========================================================================
//
//
//
//===========================================================================
