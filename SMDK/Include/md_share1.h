//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_SHARE1_H__2250E114_63A5_4B03)
#define MD_SHARE1_H__2250E114_63A5_4B03

#include <float.h>
#include <math.h>

// ----------------------------------------------------------------------

#undef DllImportExport
#if defined(__MATHLIB_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ----------------------------------------------------------------------

extern DllImportExport const double ZeroLimit;        //the numerical limit below which all numbers are considered to be zero
extern DllImportExport const double UsableMass;
extern DllImportExport const double TraceMass;
extern DllImportExport const double SmallMassFrac;
extern DllImportExport const double DisplayZeroLimit;

// ----------------------------------------------------------------------
//Useful test and functions:
template <class T> T GTZ(const T x) { return (x > (T)ZeroLimit ? x : (T)ZeroLimit); };
template <class T> T GEZ(const T x) { return (x > 0 ? x : (T)0); };
template <class T> T LTZ(const T x) { return (x < (T)-ZeroLimit ? x : (T)-ZeroLimit); };
template <class T> T LEZ(const T x) { return (x < 0 ? x : (T)0); };
template <class T> T NZ(const T x) { return (x >= 0.0) ? GTZ(x) : LTZ(x); };
template <class T> T Sign(const T x) { return (x >= 0) ? 1 : -1; };
template <class T> T ZSign(const T x) { return (x > 0.0) ? 1 : (x < 0.0) ? -1 : 0; };
template <class T> T ChkZero(const T x) { return ((fabs(x) > (T)ZeroLimit) ? x : (T)0.0); };
template <class T> bool TstGTZ(const T x) { return (x > (T)ZeroLimit); };
template <class T> bool TstLTZ(const T x) { return (x < -(T)ZeroLimit); };
template <class T> bool TstNZ(const T x) { return (fabs(x) > (T)ZeroLimit); };
template <class T> T Min(const T x, const T y) { return (x < y) ? x : y; };
template <class T> T Max(const T x, const T y) { return (x > y) ? x : y; };
template <class T> T Sqr(const T x) { return (x * x); };
template <class T> T Sqrt(const T x) { return sqrt(x); };
template <class T> T Range(const T n, const T x, const T m) { return (x >= n) ? ((x <= m) ? x : m ) : n; };
template <class T> bool InRange( const T a, const T b , const T c) { return (a<=b && b<=c); };
template <class T> void Exchange(T&a, T&b) {T c=a; a=b; b=c;};

// ----------------------------------------------------------------------
//NAN functions:
inline bool Valid(float a) { return !_isnan(a); };
inline bool Valid(double a) { return !_isnan(a); };
inline bool Finite(float a) { return _finite(a)!=0; };
inline bool Finite(double a) { return _finite(a)!=0; };

extern DllImportExport const float fQuietNAN;
extern DllImportExport const float fSignalNAN;
extern DllImportExport const double dQuietNAN;
extern DllImportExport const double dSignalNAN;

extern DllImportExport const float fNAN;
extern DllImportExport const double dNAN;

inline void Invalidate(float &x)  { x=fSignalNAN; };
inline void Invalidate(double &x) { x=dSignalNAN; };

inline bool IsQuietNAN(double V) { return (((LARGE_INTEGER*)&V)->HighPart & 0x00040000)!=0; };
inline bool IsSignalNAN(double V) { return (((LARGE_INTEGER*)&V)->HighPart & 0x00040000)==0; };

#undef DllImportExport

const unsigned long NF_All    = 0xffffffff;
const unsigned long NF_Free   = 0x00000001;
const unsigned long NF_Hold   = 0x00000002;
const unsigned long NF_Pulse  = 0x00000004;
const unsigned long NF_Ignore = 0x00000008;
const unsigned long NF_Block  = 0x00000010;
const unsigned long NF_Plus   = 0x00000020;
const unsigned long NF_Minus  = 0x00000040;
const unsigned long NF_Star   = 0x00000080;

const unsigned long NF_No0    = 0xffffffff;
const unsigned long NF_No1    = 0x00000001;
const unsigned long NF_No2    = 0x00000002;
const unsigned long NF_No3    = 0x00000004;
const unsigned long NF_No4    = 0x00000008;
const unsigned long NF_No5    = 0x00000010;
const unsigned long NF_No6    = 0x00000020;
const unsigned long NF_No7    = 0x00000040;
const unsigned long NF_No8    = 0x00000080;

//DBL_MAX         1.7976931348623158e+308
const double dNANMagic_Limit  = 1.789e+308;
const double dNANMagic_Signal = 1.790e+308;
const double dNANMagic_Quiet  = 1.791e+308;
const double dNANMagic_Free   = 1.792e+308;
const double dNANMagic_Hold   = 1.793e+308;
const double dNANMagic_Pulse  = 1.794e+308;
const double dNANMagic_Ignore = 1.795e+308;
const double dNANMagic_Block  = 1.796e+308;
const double dNANMagic_Plus   = 1.79701e+308;
const double dNANMagic_Minus  = 1.79702e+308;
const double dNANMagic_Star   = 1.79703e+308;

//FLT_MAX         3.402823466e+38F
const float fNANMagic_Limit  = 3.400e+38F;
const float fNANMagic_Signal = 3.401e+38F;
const float fNANMagic_Quiet  = 3.402e+38F;

inline bool HasNANFlag(double D, unsigned long Flag=NF_All)
  {
  if (!_isnan(D))
    return false;
  ASSERT(Flag>0 && Flag<=0xffffffff);
  return (((LARGE_INTEGER*)&D)->LowPart & Flag)!=0;
  };
inline double SetNANFlag(unsigned long Flag, double D=dNAN)
  {
  ASSERT(Flag>0 && Flag<=0xffffffff);
  ((LARGE_INTEGER*)&D)->LowPart |= Flag;
  return D;
  };
inline double ClrNANFlag(unsigned long Flag, double D=dNAN)
  {
  ASSERT(Flag>0 && Flag<=0xffffffff);
  ((LARGE_INTEGER*)&D)->LowPart &= ~Flag;
  return D;
  };

inline bool HasNANFlagShft(double D, long Index=0)
  {
  unsigned long Flag=Index>0 ? (0x00000001 << (Index-1)) : 0xffffffff;
  if (!_isnan(D))
    return false;
  ASSERT(Flag>0 && Flag<=0xffffffff);
  return (((LARGE_INTEGER*)&D)->LowPart & Flag)!=0;
  };
inline double SetNANFlagShft(long Index, double D=dNAN)
  {
  unsigned long Flag=Index>0 ? (0x00000001 << (Index-1)) : 0;
  //ASSERT(Flag>0 && Flag<=0xffffffff);
  ((LARGE_INTEGER*)&D)->LowPart |= Flag;
  return D;
  };
inline double ClrNANFlagShft(long Index, double D=dNAN)
  {
  unsigned long Flag=Index>0 ? (0x00000001 << (Index-1)) : 0;
  //ASSERT(Flag>0 && Flag<=0xffffffff);
  ((LARGE_INTEGER*)&D)->LowPart &= ~Flag;
  return D;
  };

// ----------------------------------------------------------------------
//'Safe' log, etc functions:

inline double Exps(double a) { return (a < -230.0 ? exp(-230.0) : (a > 230.0 ? exp(230.0) : exp(a)));};
inline double Lns(double a) { return (a < 1.0e-100 ? log(1.0e-100) : log(a)); };
inline double Ln(double a) { return (a < 1.0e-100 ? log(1.0e-100) : log(a)); };
inline double Log(double a) { return (a < 1.0e-100 ? log(1.0e-100) : log(a)); };
inline double Log10(double a) { return (a < 1.0e-100 ? log10(1.0e-100) : log10(a)); };
inline double Pow(double a, double b) { return (b == 0.0 ? 1.0 : (a == 0.0 ? 0.0 : pow (a,b)));};

// ----------------------------------------------------------------------

#define DoBreak() { __debugbreak(); };
#define BreakPoint() { int xxx=0; }

// ----------------------------------------------------------------------
//Useful constents:
const double PI           =   3.14159265358979;
const double Gc           =   9.81;
const double Gc_Kpa       =   (9.81 * 0.001);
//Universal Gas Constant =   8.314472  +/- 0.000015 J /mol, K
//see: http://nvl.nist.gov/pub/nistpubs/sp958-lide/339-343.pdf  http://physics.nist.gov/cuu/Constants/index.html  http://www.codata.org/
const double R_c          =   (8.314472  /* kPa, m^3/kg-mole, K */);
const double ZeroCinK     =   273.16; //This should be 273.15 NEEDS changing in a future version!

// ----------------------------------------------------------------------
//Conversion factors:
const double PSI_per_kPa  =   0.145038;
const double GPM_per_m3s  =   (4.403*3600.0);
const double kg_per_lb    =   0.4535924;
const double m3_per_ft3   =   0.028316847;
const double m_per_ft     =   0.3048;

// ----------------------------------------------------------------------
//Useful 'hardwired' conversions:
inline double C_2_K(double t) { return t+ZeroCinK;};
inline double K_2_C(double t) { return t-ZeroCinK;};
inline double C_2_F(double t) { return 32.0+t*1.8;};
inline double K_2_F(double t) { return C_2_F(K_2_C(t));};
inline double F_2_C(double t) { return (t-32.0)/1.8;};
inline double F_2_K(double t) { return C_2_K(F_2_C(t));};

inline double C2K(double t) { return t+ZeroCinK;};
inline double K2C(double t) { return t-ZeroCinK;};
inline double C2F(double t) { return 32.0+t*1.8;};
inline double K2F(double t) { return C2F(K2C(t));};
inline double F2C(double t) { return (t-32.0)/1.8;};
inline double F2K(double t) { return C2K(F2C(t));};

inline double Rads2Degs(double a) { return a*57.2957795131; };
inline double Degs2Rads(double a) { return a/57.2957795131; };

inline double kPa_2_PSI(double t) { return t*PSI_per_kPa;};
inline double PSI_2_kPa(double t) { return t/PSI_per_kPa;};

inline double m3s_2_GPM(double t) { return t*GPM_per_m3s;};
inline double GPM_2_m3s(double t) { return t/GPM_per_m3s;};

inline double PSI_GPM_2_kPa_m3s(double t) { return t*GPM_per_m3s/PSI_per_kPa;};

inline double lb_2_kg(double t) { return t*kg_per_lb;};
inline double kg_2_lb(double t) { return t/kg_per_lb;};

inline double kg_m3_2_lb_ft3(double t) { return t/kg_per_lb*m3_per_ft3;};
inline double lb_ft3_2_kg_m3(double t) { return t*kg_per_lb/m3_per_ft3;};

inline double kg_s_2_lb_hr(double t) { return t/kg_per_lb*3600.0;};
inline double lb_hr_2_kg_s(double t) { return t*kg_per_lb/3600.0;};

inline double PSI_lbhr_2_kPa_kgs(double t) { return t/kg_per_lb*3600.0/PSI_per_kPa;};

inline double Nm3_2_MMscf(double t) { return t*35.31466*1.0e-6;};

inline double KCal_2_J(double E) { return E*4.184e3;};
inline double KCal_2_kJ(double E) { return E*4.184;};
inline double BTU_2_J(double E) { return E*1.055056e3;};
inline double BTU_2_kJ(double E) { return E*1.055056;};

inline double BTU_lb_2_J_kg(double E) { return E*1.055056e3/kg_per_lb;};
inline double BTU_lb_2_kJ_kg(double E) { return E*1.055056/kg_per_lb;};

inline double m_sec_2_ft_sec(double E) { return E/m_per_ft;};

inline double um_2_ft(double E) { return E/(m_per_ft*1.0e6);};
inline double ft_2_um(double E) { return E*(m_per_ft*1.0e6);};

//===========================================================================
// Environment / Atmospheric Conditions

#undef DllImportExport
#if defined(__DATACNVS_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

extern DllImportExport double gs_StdPress;
extern DllImportExport double gs_StdTemp;
extern DllImportExport double gs_NormPress;
extern DllImportExport double gs_NormTemp;

inline double StdPress()   { return gs_StdPress;  };
inline double StdTemp()    { return gs_StdTemp;   };
inline double NormPress()  { return gs_NormPress; };
inline double NormTemp()   { return gs_NormTemp;  };

#define StdP           StdPress()
#define StdT           StdTemp()
#define NormP          NormPress()
#define NormT          NormTemp()

extern DllImportExport double gs_BaseElevation;
extern DllImportExport double gs_AmbientTemp;
extern DllImportExport double gs_WindSpeed;
extern DllImportExport double gs_WindDirection;
extern DllImportExport double gs_RelHumidity;

inline double BaseElevation()   { return gs_BaseElevation;  };
inline double WindSpeed()       { return gs_WindSpeed;      };
inline double WindDirection()   { return gs_WindDirection;  };
inline double RelHumidity()     { return gs_RelHumidity;    };
inline double AmbientTemp()     { return gs_AmbientTemp;    };

extern DllImportExport double AtmosPress(double RelElevation=0.0);
extern DllImportExport double AtmosDensity(double RelElevation=0.0);

#undef DllImportExport

//===========================================================================

typedef short CCnvIndex;

//===========================================================================
//Exceptions...

//SMDK Exception:
class MMdlException
  {
  public:
    MMdlException(long Id, LPCSTR Desc)
      {
      m_lId=Id;
      m_Desc=Desc;
      }

    LPCSTR getDescription() { return m_Desc; };
    long   getId()          { return m_lId; };

    __declspec(property(get=getDescription)) LPCSTR Description;
    __declspec(property(get=getId))          long   Id;

  protected:
    long    m_lId;
    CString m_Desc;
  };

//---------------------------------------------------------------------------

//System Exception:
class MSysException
  {
  public:
    //..MSysException() {}
    MSysException( unsigned int n,void * a, LPCTSTR d)
      {
      m_nCode=n;
      m_nAdd=(unsigned int )a;
      strcpy(m_sName, d);
      }
    ~MSysException()
      {
      }

    unsigned int getCode()        { return m_nCode; };
    unsigned int getAddress()     { return m_nAdd;  };
    LPCTSTR      getName()        { return m_sName; };
    CString      getDescription() { CString S; S.Format("Exception %s (%x) occurred @ %08x\n\n", getName(), getCode(), getAddress()); return S; };

    __declspec(property(get=getName))          LPCSTR       Name;
    __declspec(property(get=getCode))          unsigned int Code;
    __declspec(property(get=getAddress))       unsigned int Address;
    __declspec(property(get=getDescription))   CString      Description;
    __declspec(property(get=getIsFPP))         bool         IsFPP;

  private:
    unsigned int m_nCode;
    unsigned int m_nAdd;
    char m_sName[128];
  };

#if !_MANAGED

//Floating Point Processor Exception:
class MFPPException : public MSysException
  {
  public:
    //..MSysException() {}
    MFPPException( unsigned int n,void * a, LPCTSTR d) : MSysException(n,a,d)
      {
      }
    ~MFPPException()
      {
      }

    void ClearFPP()
      {
      _clearfp();
#if _MSC_VER >=1400
      unsigned int CW;
      errno_t err;
      err=_controlfp_s(&CW, 0,0); // Must Fix
      if (err)
        {
        int xxx=0;
        //LogNote("FltPoint", 0, "Error %08x Getting FP Status", err);
        }
      _fpreset();
      err=_controlfp_s(&CW, CW, _MCW_EM);
      if (err)
        {
        int xxx=0;
        //LogNote("FltPoint", 0, "Error %08x Setting FP Status", err);
        }
#else
      DWORD CW;
      CW=_controlfp(0,0);
      _fpreset();
      _controlfp(CW, _MCW_EM);
#endif
      };
  };
#endif

//---------------------------------------------------------------------------
// Tolerance Block Flags

const DWORD TBF_UseMax        = 0x00000001;
const DWORD TBF_UseAbs        = 0x00000002;
const DWORD TBF_UseRel        = 0x00000004;
const DWORD TBF_UseMaxParent  = 0x00000010;
const DWORD TBF_UseAbsParent  = 0x00000020;
const DWORD TBF_UseRelParent  = 0x00000040;
const DWORD TBF_Hide          = 0x00000100;
const DWORD TBF_Fixed         = 0x00000200;

const DWORD TBF_Sys           = 0x00010000;
const DWORD TBF_PB            = 0x00020000;
const DWORD TBF_Dyn           = 0x00040000;
const DWORD TBF_Both          = (TBF_PB|TBF_Dyn);
const DWORD TBF_PBSys         = (TBF_PB|TBF_Sys);
const DWORD TBF_DynSys        = (TBF_Dyn|TBF_Sys);
const DWORD TBF_BothSys       = (TBF_Both|TBF_Sys);

const DWORD TBF_LocalOnly     = 0x00100000;

// ==========================================================================
#undef DllImportExport
#if defined(__GPFUNCS_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

extern bool DllImportExport MakeVersionOK(LPCTSTR FileName, LPCTSTR MakeName, WORD V0, WORD V1, WORD V2, WORD V3, bool DoMsg=true);

#define DoBreak() { __debugbreak(); };

DllImportExport void DoAssert1(char * pMsg);

#define ASSERT_ALWAYS(a, msg) { if (!(a)) { DoAssert1(msg); DoBreak(); }; };
#ifdef _RELEASE
#define RDBASSERT(a, msg) {};
#else
#define RDBASSERT(a, msg) { if (!(a)) { DoAssert1(msg); DoBreak(); }; };
#endif
#ifdef _RELEASE
#define ASSERT_RDB(a, msg) {};
#else
#define ASSERT_RDB(a, msg) { if (!(a)) { DoAssert1(msg); DoBreak(); }; };
#endif

#define INCOMPLETECODE()     { char Buff[2000]; sprintf(Buff, "INCOMPLETECODE\n\n%s[%i]", __FILE__, __LINE__); DoAssert1(Buff); DoBreak() ; }
#define INCOMPLETECODEMSG(Msg) { char Buff[2000]; sprintf(Buff, "%s\n\n%s[%i]", Msg, __FILE__, __LINE__); DoAssert1(Buff); __debugbreak() ; }

// ========================================================================
// ========================================================================

#define SCD_BUILDNO      120                  /* Build number*/
#define SCD_BUILDDATE    "9 September 2006"        /* Build release date*/
#define SCD_VERINFO_V0   9                     /* Major Version */
#define SCD_VERINFO_V1   1                     /* Minor Version */
#define SCD_VERINFO_V2   4                     /* Incompatible Version, ie check these numbers match in DLLs*/
#define SCD_VERINFO_V3   SCD_BUILDNO           /* Compatible Version - should change every time a version is issued*/

#ifdef _DEBUG
#define _MAKENAME "Debug"
#elif  _RELEASE
#define _MAKENAME "Release"
#else
#define _MAKENAME "Dbg_Release"
#endif

// ========================================================================
// ========================================================================

#undef DllImportExport

#endif // !defined(MD_SHARE1_H__2250E114_63A5_4B03
