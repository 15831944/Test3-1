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

const double ZeroLimit        = 1.0e-30;//the numerical limit below which all numbers are considered to be zero
const double UsableMass       = 1.0e-16;
const double TraceMass        = 0.5e-18;
const double SmallMassFrac    = 1.0e-30;
const double DisplayZeroLimit = 1.0e-20;


//const DNanValueUnion DQuietNanValue   = { 0x7fffffffffffffff};
//const FNanValueUnion FQuietNanValue   = { 0x7fffffff};
//const DNanValueUnion DSignalNanValue  = { 0x7ffbffffffffffff};
//const FNanValueUnion FSignalNanValue  = { 0x7f800000 };
//const double NAN = dNAN;


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


const float fQuietNAN = CFNanValueUnion (0x7fffffff);
const float fSignalNAN = CFNanValueUnion (0x7fffffff);
const float fNAN=fQuietNAN;

const double dQuietNAN = CDNanValueUnion (0x7fffffff00000000);
const double dSignalNAN = CDNanValueUnion (0x7fffffff00000000);
const double dNAN=dSignalNAN;  

//inline float  fQuietNAN(void)  { FNanValueUnion FQuietNanValue={0x7fffffff};              return FQuietNanValue.F; };
//inline double dQuietNAN(void)  { DNanValueUnion DQuietNanValue={0x7fffffff00000000};      return DQuietNanValue.D; };
//inline float  fSignalNAN(void) { FNanValueUnion FSignalNanValue={0x7f800000};             return FSignalNanValue.F; };
//inline double dSignalNAN(void) { DNanValueUnion DSignalNanValue={0x7ffbffff00000000};     return DSignalNanValue.D; };
//
//inline float  fNAN(void) { FNanValueUnion FQuietNanValue={0x7fffffff};                    return FQuietNanValue.F; };
//inline double dNAN(void) { DNanValueUnion DSignalNanValue={0x7ffbffff00000000};           return DSignalNanValue.D; };
//
//inline void Invalidate(float &x)  { FNanValueUnion FSignalNanValue={0x7f800000};          x=FSignalNanValue.F; };
//inline void Invalidate(double &x) { DNanValueUnion DSignalNanValue={0x7ffbffff00000000};  x=DSignalNanValue.D; };
//
//inline bool IsQuietNAN(double V) { return (((LARGE_INTEGER*)&V)->HighPart & 0x00040000)!=0; };
//inline bool IsSignalNAN(double V) { return (((LARGE_INTEGER*)&V)->HighPart & 0x00040000)==0; };

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
  if (_stricmp(Str, "*Pulse")==0 || _strnicmp(Str, "*P", 2)==0) 
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
  return Val;
  }
