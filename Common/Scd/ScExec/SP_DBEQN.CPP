//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SP_DBEQN_CPP
#include "sp_db.h"
#include "tknparse.h"
#include "surfaces.h"
//#include "optoff.h"

//#define dbgBuildSDB    0

#if dbgBuildSDB
#include "dbgmngr.h"
static CDbgMngr dbgBuild("Specie_DB", "Build"); 
#endif

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_Const : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         C;

    SDBE_Const(CSpecieData * Sp) : CSpDBEqn(Sp) { C=0.0;};
    virtual ~SDBE_Const() {};
    static const char * Name() { return "Const"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
  	  {
  	  if (strlen(Cols[0])==0) return 2;
  	  C=SpDB_TokenValue(Cols[0], 0.0);
  	  return 0;
  	  };

    virtual void Copy(CSpDBEqn* p) { C=((SDBE_Const*)p)->C;};
    
    virtual double AtV() { return C; }; 
    virtual double AtT(double T) { return C; }; 
    virtual double AtP(double P) { return C; };
    virtual double AtTP(double T, double P) { return C; };
    virtual double Integrate_dT(double T1, double T2, double P) { return C*(T2-T1); };
    virtual double IntegrateOverT_dT(double T1, double T2, double P) { return C * log(T2 / T1); };

  };

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_Poly : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d,e,f;

    SDBE_Poly(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=e=f=0.0;};
    virtual ~SDBE_Poly() {};
    static const char * Name() { return "Poly"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
      {
  	  //if (strlen(Cols[4])==0) return 2;
  	  a=SpDB_TokenValue(Cols[0], 1.0);
  	  b=SpDB_TokenValue(Cols[1], 0.0);
  	  c=SpDB_TokenValue(Cols[2], 0.0);
  	  d=SpDB_TokenValue(Cols[3], 0.0);
  	  e=SpDB_TokenValue(Cols[4], 0.0);
  	  f=SpDB_TokenValue(Cols[5], 0.0);
  	  return 0;
  	  };
    virtual void Copy(CSpDBEqn* p) 
      { 
      SDBE_Poly*pc=(SDBE_Poly*)p; 
      a=pc->a; b=pc->b; c=pc->c; d=pc->d; e=pc->e; f=pc->f;
      };
    virtual double AtT(double T)
      { 
      return a+T*(b+T*(c+T*(d+T*(e+T*f))));
      };
    virtual double AtTP(double T, double P)
      { 
      return a+T*(b+T*(c+T*(d+T*(e+T*f))));
      };
  };


// ===========================================================================

class SDBE_LinearGasDensity : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         Rho;

    SDBE_LinearGasDensity(CSpecieData * Sp) : CSpDBEqn(Sp) { Rho=1.0;};
    virtual ~SDBE_LinearGasDensity() {};
    static const char * Name() { return "LinearGasDensity"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
  	  {
  	  if (strlen(Cols[0])==0) return 2;
  	  Rho=SpDB_TokenValue(Cols[0], 1.0);
  	  return 0;
  	  };

    virtual void Copy(CSpDBEqn* p) { Rho=((SDBE_LinearGasDensity*)p)->Rho;};
    
    virtual double AtV() { DoBreak(); return dNAN; }; 
    virtual double AtT(double T) { DoBreak(); return dNAN; }; 
    virtual double AtP(double P) { DoBreak(); return dNAN; };
    virtual double AtTP(double T, double P) { return Rho*P/Std_P*ZeroCinK/GTZ(T); };
    virtual double Integrate_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };
    virtual double IntegrateOverT_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };

  };

// ===========================================================================

class SDBE_IdealGasDensity : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         m_dMoleWt;

    SDBE_IdealGasDensity(CSpecieData * Sp) : CSpDBEqn(Sp) { m_dMoleWt=1; };
    virtual ~SDBE_IdealGasDensity() {};
    static const char * Name() { return "IdealGasDensity"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
  	  {
  	  if (strlen(Cols[0])==0) return 2;
  	  m_dMoleWt=SpDB_TokenValue(Cols[0], 1.0);
  	  return 0;
  	  };

    virtual void Copy(CSpDBEqn* p) { m_dMoleWt=((SDBE_IdealGasDensity*)p)->m_dMoleWt;};
    
    virtual double AtV() { DoBreak(); return dNAN; }; 
    virtual double AtT(double T) { DoBreak(); return dNAN; }; 
    virtual double AtP(double P) { DoBreak(); return dNAN; };
    virtual double AtTP(double T, double P) { return P*m_dMoleWt/GTZ(8.314*T); };
    virtual double Integrate_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };
    virtual double IntegrateOverT_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };

  };

// ===========================================================================

class SDBE_LiqH2ORho : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:

    SDBE_LiqH2ORho(CSpecieData * Sp) : CSpDBEqn(Sp) { };
    virtual ~SDBE_LiqH2ORho() {};
    static const char * Name() { return "LiqH2ORho"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
  	  {
  	  if (strlen(Cols[0])!=0) return 2;
  	  return 0;
  	  };

    virtual void Copy(CSpDBEqn* p) { };
    
    virtual double AtV() { DoBreak(); return dNAN; }; 
    virtual double AtT(double T) { DoBreak(); return dNAN; }; 
    virtual double AtP(double P) { DoBreak(); return dNAN; };
    virtual double AtTP(double T, double P) 
      { 
      T=Range(-30.0, K_2_C(T), 300.0);
      return (999.83952 +
             T * (16.945176 + 
             T * (-7.9870401e-3 + 
             T * (-46.170461e-6 + 
             T * (105.56302e-9 +
             T * (-280.54253e-12))))))/(1.0+16.879850e-3*T); 
      };
    virtual double Integrate_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };
    virtual double IntegrateOverT_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };

  };

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_Ts : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d;

    SDBE_Ts(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=0.0;};
    virtual ~SDBE_Ts() {};
    static const char * Name() { return "Ts"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
      {
  	  if (strlen(Cols[3])==0) return 2;
  	  a=SpDB_TokenValue(Cols[0], 1.0);
  	  b=SpDB_TokenValue(Cols[1], 0.0);
  	  c=SpDB_TokenValue(Cols[2], 0.0);
  	  d=SpDB_TokenValue(Cols[3], 0.0);
  	  return 0;
  	  };
    virtual void Copy(CSpDBEqn* p) 
      { 
      SDBE_Ts*pc=(SDBE_Ts*)p; 
      a=pc->a; b=pc->b; c=pc->c; d=pc->d;
      };
    virtual double AtP(double P)
      { 
      double lnp=log(GTZ(P*0.01));
      return exp(a+(b+(c+d*lnp)*lnp)*lnp);
      };
  };

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a;

    SDBE_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=0.0;};
    virtual ~SDBE_Cp() {};
    static const char * Name() { return "Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_Cp*pc=(SDBE_Cp*)p; a=pc->a;;};
    
    virtual double AtT(double T) { return a; };
    virtual double AtTP(double T, double P) { return a; };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

//---------------------------------------------------------------------------

int SDBE_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[0])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0);
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_Cp::Integrate_dT(double T1, double T2, double P)
  {
  return a * (T2 - T1);
  } 
  
// --------------------------------------------------------------------------

double SDBE_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  return a * log(T2 / T1);
  }      

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_CRC_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d,lo,hi;
    flag           rng;
    SDBE_CRC_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=lo=hi=0.0;rng=0;};
    virtual ~SDBE_CRC_Cp() {};
    static const char * Name() { return "CRC_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_CRC_Cp*pc=(SDBE_CRC_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; lo=pc->lo; hi=pc->hi; rng=pc->rng;};

    virtual double AtT(double T) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return a + (b * T) + (c / GTZ(T*T)) + (d * T*T); 
      };
    virtual double AtTP(double T, double P) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return a + (b * T) + (c / GTZ(T*T)) + (d * T*T); 
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

//---------------------------------------------------------------------------

int SDBE_CRC_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0) * 1.0e-3;
  c=SpDB_TokenValue(Cols[2], 0.0) * 1.0e+5;
  d=SpDB_TokenValue(Cols[3], 0.0) * 1.0e-6;
  if (strlen(Cols[4])>0 && strlen(Cols[5])>0)
    {
    lo=SpDB_TokenValue(Cols[4], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[5], C_2_K(1000.));
    rng=1;
    }
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_CRC_Cp::Integrate_dT(double T1, double T2, double P)
  {
  //  T1 to T2 Cp°dT = a(T1-T2) + b/2(T1^2-T2^2) - c(1/T1-1/T2)  
  //                    + d/3(T1^3-T2^3)     J/mol
  //  hss &kcg we think not rather Cp(T2) * dt
  //  where Cp(T2) = f(a,b,c,d);       Eqn (1) from SpecieDB
  //        dt    = difference between T2 and reference values for Enthalpy in SpecieDB
  //  NBNB Eqn (1) is based on Moles but this equation gets multiplied by Mass (kgs)               

  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = rng ? lo : m_pData->LoT();
  double HiT = rng ? hi : m_pData->HiT();
  
  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }
  
    HCp += Sgn*(a * (T2 - T1) + 
               ((b / 2.0) * (Sqr(T2) - Sqr(T1))) - 
               (c * (1.0 / T2 - 1.0 / T1)) + 
               ((d / 3.0) * (Pow(T2,3) - Pow(T1,3))));
    }
  
  //dbgpln("                    %8.3f > %8.3f %14.7g",Sgn>0?T1:T2,Sgn>0?T2:T1,HCp);
  return HCp;
  } 
  
// --------------------------------------------------------------------------

double SDBE_CRC_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  //  (T1 to T2) Cp°/T.dT = a*ln(T2/T1) + b(T2-T1)            
  //                          - c/2(1/T2^2-1/T1^2)           
  //                          + d/2(T2^2-T1^2)     J/mol.K  

  return a * log(T2 / T1) + 
        (b * (T2 - T1)) - 
        ((c / 2.) * (1. / Sqr(T2) - 1. / Sqr(T1))) + 
        ((d / 2.) * (Sqr(T2) - Sqr(T1)));
  }      

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_CRC1_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d,lo,hi;
    flag           rng;
    SDBE_CRC1_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=lo=hi=0.0;rng=0;};
    virtual ~SDBE_CRC1_Cp() {};
    static const char * Name() { return "CRC1_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_CRC1_Cp*pc=(SDBE_CRC1_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; lo=pc->lo; hi=pc->hi; rng=pc->rng;};
    
    virtual double AtT(double T) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return a + (b * T) + (c * T*T) + (d / GTZ(T*T)); 
      };
    virtual double AtTP(double T, double P) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return a + (b * T) + (c * T*T) + (d / GTZ(T*T)); 
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

//---------------------------------------------------------------------------

int SDBE_CRC1_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0) * 1.0e-3;
  c=SpDB_TokenValue(Cols[2], 0.0) * 1.0e-6;
  d=SpDB_TokenValue(Cols[3], 0.0) * 1.0e+5;
  if (strlen(Cols[4])>0 && strlen(Cols[5])>0)
    {
    lo=SpDB_TokenValue(Cols[4], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[5], C_2_K(1000.));
    rng=1;
    }
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_CRC1_Cp::Integrate_dT(double T1, double T2, double P)
  {
  //  T1 to T2 Cp°dT = a(T1-T2) + b/2(T1^2-T2^2)
  //                 - d(1/T1-1/T2)  + c/3(T1^3-T2^3) J/mol
  //  hss &kcg we think not rather Cp(T2) * dt
  //  where Cp(T2) = f(a,b,c,d);       Eqn (1) from SpecieDB
  //        dt    = difference between T2 and reference values for Enthalpy in SpecieDB
  //  NBNB Eqn (1) is based on Moles but this equation gets multiplied by Mass (kgs)               

  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = rng ? lo : m_pData->LoT();
  double HiT = rng ? hi : m_pData->HiT();
  
  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }
  
    HCp += Sgn*(a * (T2 - T1) + 
               ((b / 2.0) * (Sqr(T2) - Sqr(T1))) + 
               ((c / 3.0) * (Pow(T2,3) - Pow(T1,3))) - 
               (d * (1.0 / T2 - 1.0 / T1)));
    }
  
  //dbgpln("                    %8.3f > %8.3f %14.7g",Sgn>0?T1:T2,Sgn>0?T2:T1,HCp);
  return HCp;
  } 
  
// --------------------------------------------------------------------------

double SDBE_CRC1_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  //  (T1 to T2) Cp°/T.dT = a*ln(T2/T1) + b(T2-T1)            
  //                          + c/2(T2^2-T1^2)     J/mol.K  
  //                          - d/2(1/T2^2-1/T1^2)           

  return a * log(T2 / T1) + 
        (b * (T2 - T1)) + 
        ((c / 2.) * (Sqr(T2) - Sqr(T1))) - 
        ((d / 2.) * (1. / Sqr(T2) - 1. / Sqr(T1)));
  }      

// ===========================================================================
//
//
//
// ===========================================================================

class SDBE_HSC_Cp : protected SDBE_CRC_Cp
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
//    double         a,b,c,d,lo,hi;
//    flag           rng;
    SDBE_HSC_Cp(CSpecieData * Sp) : SDBE_CRC_Cp(Sp) { };
//    virtual ~SDBE_CRC_Cp() {};
    static const char * Name() { return "HSC_Cp"; };
//    virtual const char * VName() { return Name(); };
//    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
//    virtual void Copy(CSpDBEqn* p) { SDBE_CRC_Cp*pc=(SDBE_CRC_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; lo=pc->lo; hi=pc->hi; rng=pc->rng;};
//    
//    virtual double AtT(double T) 
//      { 
//      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
//      return a + (b * T) + (c / GTZ(T*T)) + (d * T*T); 
//      };
//    virtual double Integrate_dT(double T1, double T2, double P);
//    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

// ==========================================================================
//
//
//
// ===========================================================================

//Poly_Cp used in Appendix A: The Properties of GASES & LIQUIDS Fourth Edition; Robert C. Reid, John M. Prausnitz, Bruce E. Poling; McGraw-Hill;ISBN 0-07-051799-1
class SDBE_Poly_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d,lo,hi;
    flag           rng;
    SDBE_Poly_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=lo=hi=0.0;rng=0;};
    virtual ~SDBE_Poly_Cp() {};
    static const char * Name() { return "Poly_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_Poly_Cp*pc=(SDBE_Poly_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; lo=pc->lo; hi=pc->hi; rng=pc->rng;};

    virtual double AtT(double T) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return a + T * (b + T * (c + T * d));
      };
    virtual double AtTP(double T, double P) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return a + T * (b + T * (c + T * d));
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

//---------------------------------------------------------------------------

int SDBE_Poly_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0);
  c=SpDB_TokenValue(Cols[2], 0.0);
  d=SpDB_TokenValue(Cols[3], 0.0);
  //e=SpDB_TokenValue(Cols[4], 0.0);
  if (strlen(Cols[4])>0 && strlen(Cols[5])>0)
    {
    lo=SpDB_TokenValue(Cols[4], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[5], C_2_K(1000.));
    rng=1;
    }
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_Poly_Cp::Integrate_dT(double T1, double T2, double P)
  {
  //  T1 to T2 Cp°dT = a(T1-T2) + b/2(T1^2-T2^2) + c/3(T1^3-T2^3)  
  //                   + d/4(T1^4-T2^4) + e/5(T1^5-T2^5)     J/mol

  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = rng ? lo : m_pData->LoT();
  double HiT = rng ? hi : m_pData->HiT();
  
  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }
  
    HCp += Sgn*( T2*(a+T2*(b/2.0+T2*(c/3.0+T2*d/4.0))) -
                 T1*(a+T1*(b/2.0+T1*(c/3.0+T1*d/4.0))) );
    }
  
  return HCp;
  } 
  
// --------------------------------------------------------------------------

double SDBE_Poly_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  //  (T1 to T2) Cp°/T.dT = a*ln(T2/T1) + b(T2-T1) + c/2(T2^2-T1^2)            
  //                          + d/3(T2^3-T1^3) + e/4(T2^4-T1^4)     J/mol.K  

  return a * log(T2 / T1) + 
         T2*(b+T2*(c/2.0+T2*d/3.0)) - 
         T1*(b+T1*(c/2.0+T1*d/3.0));
  }      

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_API_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double          a,b,c,d,e,f,lo,hi;
    flag            rng;

    SDBE_API_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=e=f=lo=hi=0.0;rng=0;};
    virtual ~SDBE_API_Cp() {};
    static const char * Name() { return "API_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_API_Cp*pc=(SDBE_API_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; e=pc->e; f=pc->f; lo=pc->lo; hi=pc->hi; rng=pc->rng;};
    
    virtual double AtT(double T) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return b+T*(2.0*c+T*(3.0*d+T*(4.0*e+T*5.0*f))); 
      };
    virtual double AtTP(double T, double P) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return b+T*(2.0*c+T*(3.0*d+T*(4.0*e+T*5.0*f))); 
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);

  };

int SDBE_API_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[5])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0) * m_pData->MoleWt();
  b=SpDB_TokenValue(Cols[1], 0.0) * m_pData->MoleWt();
  c=SpDB_TokenValue(Cols[2], 0.0) * 1.0e-3 * m_pData->MoleWt();
  d=SpDB_TokenValue(Cols[3], 0.0) * 1.0e-6 * m_pData->MoleWt();
  e=SpDB_TokenValue(Cols[4], 0.0) * 1.0e-9 * m_pData->MoleWt();
  f=SpDB_TokenValue(Cols[5], 0.0) * 1.0e-13 * m_pData->MoleWt();
  if (strlen(Cols[6])>0 && strlen(Cols[7])>0)
    {
    lo=SpDB_TokenValue(Cols[6], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[7], C_2_K(1000.));
    rng=1;
    }
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_API_Cp::Integrate_dT(double T1, double T2, double P)
  {
  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = rng ? lo : m_pData->LoT();
  double HiT = rng ? hi : m_pData->HiT();
  
  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }

    HCp += Sgn*(T2*(b+T2*(c+T2*(d+T2*(e+T2*f)))) - T1*(b+T1*(c+T1*(d+T1*(e+T1*f)))));
    }

  return HCp;
  } 
  
// --------------------------------------------------------------------------

double SDBE_API_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  return b * log(T2 / T1) + 
         T2*(2.*c+T2*(3./2.*d+T2*(4./3.*e+T2*5./4.*f))) - 
         T1*(2.*c+T1*(3./2.*d+T1*(4./3.*e+T1*5./4.*f)));
  }      

// ===========================================================================
//
//
//
// ===========================================================================
//
// Enthalpy Equations used by MetSim to allow use of MinProc Data
//
class SDBE_HTE_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double          a,b,c,d,lo,hi;
    flag            rng;

    SDBE_HTE_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=lo=hi=0.0;rng=0;};
    virtual ~SDBE_HTE_Cp() {};
    static const char * Name() { return "HTE_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_HTE_Cp*pc=(SDBE_HTE_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; lo=pc->lo; hi=pc->hi; rng=pc->rng;};
    
    virtual double AtT(double T) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return 4.186*(b+2.0*c*T-d/(T*T)); 
      };
    virtual double AtTP(double T, double P) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return 4.186*(b+2.0*c*T-d/(T*T)); 
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);

  };

//---------------------------------------------------------------------------

int SDBE_HTE_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 1.0);
  c=SpDB_TokenValue(Cols[2], 0.0) * 1.0e-3;
  d=SpDB_TokenValue(Cols[3], 0.0) * 1.0e5;
  if (strlen(Cols[4])>0 && strlen(Cols[5])>0)
    {
    lo=SpDB_TokenValue(Cols[4], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[5], C_2_K(1000.));
    rng=1;
    }

  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_HTE_Cp::Integrate_dT(double T1, double T2, double P)
  {
  //double Diff = T2 - T1;
  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = rng ? lo : m_pData->LoT();
  double HiT = rng ? hi : m_pData->HiT();
  
  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }
  
    HCp += Sgn*4.186*(b*(T2-T1) + c*(Sqr(T2)-Sqr(T1)) + (d/GTZ(T2)-d/GTZ(T1)));
    }
  
  return HCp;
  } 
  
// --------------------------------------------------------------------------

double SDBE_HTE_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  // integrate (b/t+ 2.0*c - d/(t*t*t)) from t1 to t2 
  // b+2.0*c*t+0.5*d/(t*t)
  return 4.186*(2.0*c*(T2-T1) + d*(0.5/Sqr(T2)-0.5/Sqr(T1)));
  }      

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_QAL_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,lo,hi;
    flag           rng;
    SDBE_QAL_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=lo=hi=0.0;rng=0;};
    virtual ~SDBE_QAL_Cp() {};
    static const char * Name() { return "QAL_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_QAL_Cp*pc=(SDBE_QAL_Cp*)p; a=pc->a; b=pc->b; c=pc->c; lo=pc->lo; hi=pc->hi; rng=pc->rng;};

    virtual double AtT(double T);// { return a + (b * T) - (c / GTZ(T*T)); };
    virtual double AtTP(double T, double P);// { return a + (b * T) - (c / GTZ(T*T)); };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

double SDBE_QAL_Cp::AtT(double T) 
  { 
  T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
  return a + (b * T) - (c / GTZ(T*T)); 
  };

double SDBE_QAL_Cp::AtTP(double T, double P) 
  { 
  T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
  return a + (b * T) - (c / GTZ(T*T)); 
  };

//---------------------------------------------------------------------------

int SDBE_QAL_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[2])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0) * m_pData->MoleWt();
  b=SpDB_TokenValue(Cols[1], 0.0) * m_pData->MoleWt();
  c=SpDB_TokenValue(Cols[2], 0.0) * m_pData->MoleWt();
  if (strlen(Cols[3])>0 && strlen(Cols[4])>0)
    {
    lo=SpDB_TokenValue(Cols[3], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[4], C_2_K(1000.));
    rng=1;
    }
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_QAL_Cp::Integrate_dT(double T1, double T2, double P)
  {

  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = rng ? lo : m_pData->LoT();
  double HiT = rng ? hi : m_pData->HiT();
  
  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }
  
    double X = a * (T2 - T1);
    double Y = (b / 2.0) * (Sqr(T2) - Sqr(T1));
    double Z = c * (1.0 / T2 - 1.0 / T1);
    HCp += Sgn*(X+Y+Z);
    
    //HCp += Sgn*(a * (T2 - T1) + 
    //           ((b / 2.0) * (Sqr(T2) - Sqr(T1))) + 
    //           (c * (1.0 / T2 - 1.0 / T1)) );
    }
  
  return HCp;
  } 
  
// --------------------------------------------------------------------------

double SDBE_QAL_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  return a * log(T2 / T1) + 
        (b * (T2 - T1)) + 
        (c / 2.) * (1. / Sqr(T2) - 1. / Sqr(T1));
  }      

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_Shomate_Cp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class SpecieD; 
  // friend class ComponentD; 
  // friend class SpecieDataBase;
  // friend class ComponentDataBase;

  protected:
    double         a,b,c,d,e,lo,hi;
    flag           rng;

    SDBE_Shomate_Cp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=e=lo=hi=0.0;rng=0;};
    virtual ~SDBE_Shomate_Cp() {};
    static const char * Name() { return "Shomate_Cp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) 
      { 
      SDBE_Shomate_Cp*pc=(SDBE_Shomate_Cp*)p; 
      a=pc->a;b=pc->b;c=pc->c;d=pc->d;e=pc->e;/*f=pc->f;g=pc->g;h=pc->h;*/
      lo=pc->lo; hi=pc->hi; rng=pc->rng;
      };
    
    virtual double AtT(double T) 
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      T/=1000.0; 
      return (a+T*(b+T*(c+T*d))+e/(T*T));//*1000;
      }
    virtual double AtTP(double T, double P)
      { 
      T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      T/=1000.0; 
      return (a+T*(b+T*(c+T*d))+e/(T*T));//*1000;
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

//---------------------------------------------------------------------------

int SDBE_Shomate_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[0])==0) return 2;
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0);
  c=SpDB_TokenValue(Cols[2], 0.0);
  d=SpDB_TokenValue(Cols[3], 0.0);
  e=SpDB_TokenValue(Cols[4], 0.0);
  //f=SpDB_TokenValue(Cols[5], 0.0);
  //g=SpDB_TokenValue(Cols[6], 0.0);
  //h=SpDB_TokenValue(Cols[7], 0.0);
  if (strlen(Cols[5])>0 && strlen(Cols[6])>0)
    {
    lo=SpDB_TokenValue(Cols[5], C_2_K(25.));
    hi=SpDB_TokenValue(Cols[6], C_2_K(1000.));
    rng=1;
    }
  return 0;
  };
      
//---------------------------------------------------------------------------

double SDBE_Shomate_Cp::Integrate_dT(double T1, double T2, double P)
  {
  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  double LoT = (rng ? lo : m_pData->LoT())/1000.0;
  double HiT = (rng ? hi : m_pData->HiT())/1000.0;
  T1/=1000.;
  T2/=1000.;

  if (T2<LoT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  else if (T1>=HiT)
    HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  else
    {
    // Part Of Curve gets used
    if (T2>HiT)
      {
      HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
      T2=HiT;
      }
    if (T1<LoT)
      {
      HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
      T1=LoT;
      }
  
    double T12 = T1*T1;
    double T13 = T1*T12;
    double T14 = T1*T13;
    double T22 = T2*T2;
    double T23 = T2*T22;
    double T24 = T2*T23;

    double v1 = a * (T2 - T1);
    double v2 = (b / 2.0) * (T22 - T12);
    double v3 = (c / 3.0) * (T23 - T13);
    double v4 = (d / 4.0) * (T24 - T14);
    double v5 = - e * (1.0 / T2 - 1.0 / T1);
    HCp += Sgn*(v1+v2+v3+v4+v5);
    }
  
  return HCp*1000.0;
  }  
  
// --------------------------------------------------------------------------

double SDBE_Shomate_Cp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  //  (T1 to T2) Cp°/T.dT = a*ln(T2/T1) + b(T2-T1) + c/2(T2^2-T1^2)            
  //                          + d/3(T2^3-T1^3) + e/2(T2^2-T1^2)     J/mol.K  

  T1/=1000.;
  T2/=1000.;
  return ( a * log(T2 / T1) + 
           T2*(b+T2*(c/2.0+T2*d/3.0)) - 
           T1*(b+T1*(c/2.0+T1*d/3.0)) +
           ((e/2.0)*(Sqr(T2) - Sqr(T1)))
         ) * 1000.0;
  }      

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_Vp: protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d;

    SDBE_Vp(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=0.0;};
    virtual ~SDBE_Vp() {};
    static const char * Name() { return "Vp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_Vp*pc=(SDBE_Vp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d;};
    
    virtual double AtT(double T); 
  };

//---------------------------------------------------------------------------

int SDBE_Vp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) 
    {
    a=1.0;
    b=c=d=0.0;
    return 2;
    }
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0);
  c=SpDB_TokenValue(Cols[2], 0.0);
  d=SpDB_TokenValue(Cols[3], 0.0);
  return 0;
  }

//---------------------------------------------------------------------------

double SDBE_Vp::AtT(double T)
  { 
  //if (!VpOk) return 0.0;
  T=Range(1.0, T, TCrit());
  return Pow(10.0, a/T+b*log10(T)+c*T+d)*0.1333224; // mmHg to kPa
  }

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_VpKPa: protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d;

    SDBE_VpKPa(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=0.0;};
    virtual ~SDBE_VpKPa() {};
    static const char * Name() { return "VpKPa"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_VpKPa*pc=(SDBE_VpKPa*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d;};
    
    virtual double AtT(double T); 
  };

//---------------------------------------------------------------------------

int SDBE_VpKPa::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) 
    {
    a=1.0;
    b=c=d=0.0;
    return 2;
    }
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0);
  c=SpDB_TokenValue(Cols[2], 0.0);
  d=SpDB_TokenValue(Cols[3], 0.0);
  return 0;
  }

//---------------------------------------------------------------------------

double SDBE_VpKPa::AtT(double T)
  { 
  T=Range(1.0, T, TCrit());
  double P=Pow(10.0, a/T+b*log10(T)+c*T+d);
  //dbgpln("T, P %10.4f(%10.4f) %10.4f", T, K2C(T), P); 
  return P;
  }

// ==========================================================================
//
//
//
// ===========================================================================

class SDBE_VpAtm: protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c,d;

    SDBE_VpAtm(CSpecieData * Sp) : CSpDBEqn(Sp) {a=b=c=d=0.0;};
    virtual ~SDBE_VpAtm() {};
    static const char * Name() { return "VpAtm"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols]);
    virtual void Copy(CSpDBEqn* p) { SDBE_VpAtm*pc=(SDBE_VpAtm*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d;};
    
    virtual double AtT(double T); 
  };

//---------------------------------------------------------------------------

int SDBE_VpAtm::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
  {
  if (strlen(Cols[3])==0) 
    {
    a=1.0;
    b=c=d=0.0;
    return 2;
    }
  a=SpDB_TokenValue(Cols[0], 1.0);
  b=SpDB_TokenValue(Cols[1], 0.0);
  c=SpDB_TokenValue(Cols[2], 0.0);
  d=SpDB_TokenValue(Cols[3], 0.0);
  return 0;
  }

//---------------------------------------------------------------------------

double SDBE_VpAtm::AtT(double T)
  { 
  T=Range(1.0, T, TCrit());
  double P=Pow(10.0, a/T+b*log10(T)+c*T+d);    
  //dbgpln("T, P %10.4f(%10.4f) %10.4f", T, K2C(T), P); 
  return P*gs_StdPress;
  }

// ==========================================================================
//
//
//
// ===========================================================================
#if WithHFC134a

class SDBE_HFC134a_GasCp : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:
    double         a,b,c;//,lo,hi;
    //flag           rng;
    SDBE_HFC134a_GasCp(CSpecieData * Sp) : CSpDBEqn(Sp) 
      {
      a=1.94006e1;
      b=2.58531e-1;
      c=-1.29665e-4;
      //lo=0.0;
      //hi=0.0;
      //rng=0;
      };
    virtual ~SDBE_HFC134a_GasCp() {};
    static const char * Name() { return "HFC134a_GasCp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
   	  {
  	  if (strlen(Cols[0])!=0) return 2;
  	  return 0;
  	  };
    virtual void Copy(CSpDBEqn* p) { /*SDBE_HFC134a_Cp*pc=(SDBE_HFC134a_Cp*)p; a=pc->a; b=pc->b; c=pc->c; d=pc->d; lo=pc->lo; hi=pc->hi; rng=pc->rng;*/ };

    virtual double AtT(double T) 
      { 
      //T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return (a + (b * T) + (c * T*T));///102.03; 
      };
    virtual double AtTP(double T, double P) 
      { 
      //T=rng ? Range(lo, T, hi) : Range(m_pData->LoT(), T, m_pData->HiT());
      return (a + (b * T) + (c * T*T));///102.03; 
      };
    virtual double Integrate_dT(double T1, double T2, double P);
    virtual double IntegrateOverT_dT(double T1, double T2, double P);
  };

//---------------------------------------------------------------------------

//int SDBE_HFC134a_Cp::Load(const char * pInitStr, char* Cols[MaxSPDBCols])
//  {
//  if (strlen(Cols[3])==0) return 2;
//  a=SpDB_TokenValue(Cols[0], 1.0);
//  b=SpDB_TokenValue(Cols[1], 0.0) * 1.0e-3;
//  c=SpDB_TokenValue(Cols[2], 0.0) * 1.0e+5;
//  d=SpDB_TokenValue(Cols[3], 0.0) * 1.0e-6;
//  if (strlen(Cols[4])>0 && strlen(Cols[5])>0)
//    {
//    lo=SpDB_TokenValue(Cols[4], C_2_K(25.));
//    hi=SpDB_TokenValue(Cols[5], C_2_K(1000.));
//    rng=1;
//    }
//  return 0;
//  };
      
//---------------------------------------------------------------------------

double SDBE_HFC134a_GasCp::Integrate_dT(double T1, double T2, double P)
  {
  //  T1 to T2 Cp°dT = a(T1-T2) + b/2(T1^2-T2^2) - c(1/T1-1/T2)  
  //                    + d/3(T1^3-T2^3)     J/mol
  //  hss &kcg we think not rather Cp(T2) * dt
  //  where Cp(T2) = f(a,b,c,d);       Eqn (1) from SpecieDB
  //        dt    = difference between T2 and reference values for Enthalpy in SpecieDB
  //  NBNB Eqn (1) is based on Moles but this equation gets multiplied by Mass (kgs)               

  int Sgn=T2<T1 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  //double LoT = rng ? lo : m_pData->LoT();
  //double HiT = rng ? hi : m_pData->HiT();
  //
  //if (T2<LoT)
  //  HCp = Sgn*(T2-T1)*m_pData->mlCp(LoT, P);
  //else if (T1>=HiT)
  //  HCp = Sgn*(T2-T1)*m_pData->mlCp(HiT, P);
  //else
    {
    // Part Of Curve gets used
    //if (T2>HiT)
    //  {
    //  HCp = HCp + Sgn*(T2-HiT)*m_pData->mlCp(HiT, P);
    //  T2=HiT;
    //  }
    //if (T1<LoT)
    //  {
    //  HCp = HCp + Sgn*(LoT-T1)*m_pData->mlCp(LoT, P);
    //  T1=LoT;
    //  }
  
    HCp += Sgn*(a * (T2 - T1) + 
               ((b / 2.0) * (Sqr(T2) - Sqr(T1))) + 
               ((c / 3.0) * (Pow(T2,3) - Pow(T1,3))));
    }
  
  //dbgpln("                    %8.3f > %8.3f %14.7g",Sgn>0?T1:T2,Sgn>0?T2:T1,HCp);
  return HCp;///102.03;
  } 
  
// --------------------------------------------------------------------------

double SDBE_HFC134a_GasCp::IntegrateOverT_dT(double T1, double T2, double P)
  {
  //  (T1 to T2) Cp°/T.dT = a*ln(T2/T1) + b(T2-T1)            
  //                          - c/2(1/T2^2-1/T1^2)           
  //                          + d/2(T2^2-T1^2)     J/mol.K  

  return (a * log(T2 / T1) + 
        (b * (T2 - T1)) + 
        ((c / 2.) * (Sqr(T2) - Sqr(T1))));///102.03;
  }      

// ==========================================================================

class SDBE_HFC134a_Vp: protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:

    SDBE_HFC134a_Vp(CSpecieData * Sp) : CSpDBEqn(Sp) {};
    virtual ~SDBE_HFC134a_Vp() {};
    static const char * Name() { return "HFC134a_Vp"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
   	  {
  	  if (strlen(Cols[0])!=0) return 2;
  	  return 0;
  	  };
    virtual void Copy(CSpDBEqn* p) {  };
    
    virtual double AtT(double T)
      { 
      T=Range(C2K(-100.0), T, C2K(101.08));
      double FT=3.761111e2-T;
      return Pow(10.0, 4.069889e1 +
                       -2.362540e3/T +
                       -1.306883e1*log10(T) +
                       7.616005e-3*T + 
                       2.342564e-1*(FT/T)*log10(FT));
      }
  };

// ==========================================================================

class SDBE_HFC134a_LiqRho : protected CSpDBEqn
  {
  friend class CSpDBEqn; 
  // friend class CSpecieData; 
  // friend class CComponent; 
  // friend class CSpecieDataBase;
  // friend class CComponentDataBase;

  protected:

    SDBE_HFC134a_LiqRho(CSpecieData * Sp) : CSpDBEqn(Sp) { };
    virtual ~SDBE_HFC134a_LiqRho() {};
    static const char * Name() { return "HFC134a_LiqRho"; };
    virtual const char * VName() { return Name(); };
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])
  	  {
  	  if (strlen(Cols[0])!=0) return 2;
  	  return 0;
  	  };

    virtual void Copy(CSpDBEqn* p) { };
    
    virtual double AtV() { DoBreak(); return dNAN; }; 
    virtual double AtT(double T) { DoBreak(); return dNAN; }; 
    virtual double AtP(double P) { DoBreak(); return dNAN; };
    virtual double AtTP(double T, double P) 
      { 
      double Tr=T/374.23;
      return 5.281464e2 +
             7.551834e2 * pow(1.-Tr, 1./3.) +
             1.028676e3 * pow(1.-Tr, 2./3.) +
             -9.491172e2 * (1.-Tr) +
             5.935660e2 * pow(1.-Tr, 4./3.);
      };
    virtual double Integrate_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };
    virtual double IntegrateOverT_dT(double T1, double T2, double P) { DoBreak(); return dNAN; };

  };

#endif
// ==========================================================================
//
//
//
// ===========================================================================

#if WithSatH2OMdl
inline double SatVP(double Temp)
  {
  Temp=Range(273.16, Temp, 647.0);
  return exp(73.649-7258.2/Temp-7.3037*log(Temp)+4.1653e-6*Temp*Temp)*0.001;
  }

H2OEQNBEGIN(SatOnly_WaterHeatCapacity, "SatOnlyWaterCp")
virtual double AtTP(double Temp, double Press) 
  { 
  Temp=Range(0., K_2_C(Temp), 374.15);
  return 18.01528 *(4.183488624+
              Temp*(4.308458533+
              Temp*(-19.02126982e-3+
              Temp*(315.706461e-6+
              Temp*(-1761.078939e-9+
              Temp*(3263.226375e-12))))))/(1+966.8412634e-3*Temp); 
  };
H2OEQNEND

H2OEQNBEGIN(SatOnly_SteamHeatCapacity, "SatOnlySteamCp")
virtual double AtTP(double Temp, double Press)
  { 
  Temp=Range(0., K_2_C(Temp), 374.15);
  return 18.01528 *(1.727221647  +
              Temp*(1.307942724  +
              Temp*(-20.36462093e-3+
              Temp*(331.5117771e-6+
              Temp*(-1806.803455e-9+
              Temp*(3365.736592e-12))))))/(1+530.1357335e-3*Temp); 
  }  
H2OEQNEND

H2OEQNBEGIN(SatOnly_WaterEnthalpy, "SatOnlyWaterH")
virtual double AtTP(double Temp, double Press)
  {
//  double Tkr=Temp;
//  double Vp=SatVP(Temp); 
//  double Pr=Range(0.01, Press, Vp); 
  double T=Range(0., K_2_C(Temp), 374.15);
  double HLiq=18.01528 *(0+
            T*(4.226727454+
            T*(-11.84290541e-3+
            T*(4.48047042e-6+
            T*(0.894027416e-9+
            T*(-20.03367824e-12))))))/(1-2.635979893e-3*T); 
//  if (Tkr!=Temp)
//    HLiq += CP(Tkr, Pr, 1)*(Temp-Tkr);
//  if (Pr!=Press)
//    {
//    double DP=(Pr>(Vp+1.0) ? -0.1 : 0.1);
//    HLiq += (CP(Tkr, Pr+DP, 1)-CP(Tkr, Pr, 1))/DP*(Press-Pr);
//    }
  return HLiq;
  }
H2OEQNEND

H2OEQNBEGIN(SatOnly_SteamEnthalpy, "SatOnlySteamH")
virtual double AtTP(double Temp, double Press)
  {
  double T=Range(0., K_2_C(Temp), 374.15);
  double HVap=18.01528 * (  2500.9     +
            T*(-5.104980358 +
            T*(1.583943105e-3+
            T*(-43.4791753e-6+
            T*(92.13413393e-9+
            T*(-41.2391801e-12))))))/(1-2.623714801e-3*T); 

//  Press*=0.01; 
//  double Tkr=Range(273.16, Temp, 1073.15);
//  double Vp=VAPP(Temp); 
//  double Pr=Range(0.01, Press, Vp); 
//  //if (ForceSatP) Pr=VAPP(Tkr);
//  double ENTH;
//  VSUB2(Tkr, Pr, NULL, &ENTH); 
//  double HVap = ENTH;
//  if (Tkr!=Temp)
//    HVap += CP(Tkr, Pr, 2)*(Temp-Tkr);
//  if (Pr!=Press)
//    {
//    double DP=(Pr<(Vp-1.0) ? 0.1 : -0.1);
//    double cp1=CP(Tkr, Pr+DP, 2);
//    double cp2=CP(Tkr, Pr, 2);
//    HVap += (cp1-cp2)/DP*(Press-Pr);
//    }
  return HVap;
  }
H2OEQNEND

H2OEQNBEGIN(SatOnly_WaterDensity, "SatOnlyWaterRho")
virtual double AtTP(double Temp, double Press)
  {
  Temp=Range(0., K_2_C(Temp), 374.15);
  return (1002.073475  +
    Temp*(-2.693433228+
    Temp*(-1.56286939e-3+
    Temp*(-0.096691388e-6+
    Temp*(26.95003817e-9+
    Temp*(-37.4978378e-12))))))/(1-2.503061829e-3*Temp); 
  }
H2OEQNEND

H2OEQNBEGIN(SatOnly_SteamDensity, "SatOnlySteamRho")
virtual double AtTP(double Temp, double Press)
  {
  double Tc=Range(0., K_2_C(Temp), 374.15);
  double Tk=C_2_K(Tc);
  double P=Range(0.01, Press, SatVP(Temp));
  double Dens=(0.004817767+
    Tc*(0.000165636+
    Tc*(0.026141224e-3+
    Tc*(-0.280516296e-6+
    Tc*(5.502323273e-9+
    Tc*(-11.83736693e-12))))))/(1-2.563454735e-3*Tc);
  Dens*=(Tk/Temp)*(Press/P);
  return Dens;
  }         
H2OEQNEND

H2OEQNBEGIN(SatOnly_WaterVapourPress, "SatOnlyWaterVp")
virtual double AtT(double Temp) 
  {
  Temp=Range(273.16, Temp, 647.0);
  return exp(73.649-7258.2/Temp-7.3037*log(Temp)+4.1653e-6*Temp*Temp)*0.001;
  }
H2OEQNEND

// ==========================================================================
//
//
//
// ===========================================================================

#define UseSimpleH2O 1

#if UseSimpleH2O
#define DllImportExport

DEFINE_TAGOBJ(SimpleH2OProps);
class SimpleH2OProps : public H2OPropertiesBlock
  {
  public:

    SimpleH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    
    virtual ~SimpleH2OProps();
    virtual void   BuildDataDefn(DataDefnBlk & DDB)     { };
    virtual flag   DataXchg(DataChangeBlk & DCB)        { return 0; };
    virtual flag   ValidateData(ValidateDataBlk & VDB)  { return 0; };

  };

//--------------------------------------------------------------------------

IMPLEMENT_TAGOBJ(SimpleH2OProps, H2OPropsGroup, SaturatedH2OName, "", "", TOC_SYSTEM, 
                 "Saturated Props",
                 "Saturated Properties for Water & Steam");

SimpleH2OProps::SimpleH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  H2OPropertiesBlock(pClass_, pTag, pAttach, eAttach)
  {
  m_Liq.m_pRho[0] = new SatOnly_WaterDensity(NULL);
  m_Liq.m_pCp[0]  = new SatOnly_WaterHeatCapacity(NULL);
  m_Liq.m_pH[0]   = new SatOnly_WaterEnthalpy(NULL);
  m_Liq.m_pVp[0]  = new SatOnly_WaterVapourPress(NULL);
  m_Vap.m_pRho[0] = new SatOnly_SteamDensity(NULL);
  m_Vap.m_pCp[0]  = new SatOnly_SteamHeatCapacity(NULL);
  m_Vap.m_pH[0]   = new SatOnly_SteamEnthalpy(NULL);
  m_Vap.m_pVp[0]  = new SatOnly_WaterVapourPress(NULL);
  }  
    
//--------------------------------------------------------------------------

SimpleH2OProps::~SimpleH2OProps()
  {
  delete m_Liq.m_pRho[0];
  delete m_Liq.m_pCp[0];
  delete m_Liq.m_pH[0];
  delete m_Liq.m_pVp[0];
  delete m_Vap.m_pRho[0];
  delete m_Vap.m_pCp[0];
  delete m_Vap.m_pH[0];
  delete m_Vap.m_pVp[0];

  m_Liq.m_pRho[0]=NULL;
  m_Liq.m_pCp[0]=NULL;
  m_Liq.m_pH[0]=NULL;
  m_Liq.m_pS[0]=NULL;
  m_Liq.m_pVp[0]=NULL;
  m_Vap.m_pRho[0]=NULL;
  m_Vap.m_pCp[0]=NULL;
  m_Vap.m_pH[0]=NULL;
  m_Vap.m_pS[0]=NULL;
  m_Vap.m_pVp[0]=NULL;
  }
#endif
#endif

// ==========================================================================
//
//
//
// ==========================================================================

#if WithIAFH2OMdl
extern bool TryConstructIAF(CSpecieData * pSpD, LPCTSTR Str, LPCTSTR &pn, CSpDBEqn* & p);
#endif
#if WithIF97H2OMdl
extern bool TryConstructIF97(CSpecieData * pSpD, LPCTSTR Str, LPCTSTR &pn, CSpDBEqn* & p);
#endif
#if WithNBSH2OMdl
extern bool TryConstructNBS(CSpecieData * pSpD, LPCTSTR Str, LPCTSTR &pn, CSpDBEqn* & p);
#endif

// ==========================================================================
//
//
//
// ==========================================================================

CSpDBEqn* CSpDBEqn::ConstructCopy(CSpecieData * pSpPh)
  {
  CSpDBEqn *pEqn=pSpPh->m_pRho[pSpPh->m_iLastTPoint];
  return CSpDBEqn::Construct(pSpPh, NULL, pEqn);
  }

CSpDBEqn* CSpDBEqn::Construct(CSpecieData * pSpD, const char *pInitString, CSpDBEqn* pCopyFrom/*=NULL*/)
  {
  CSpDBEqn* p=NULL;
  char Str[1024], *Cols[MaxSPDBCols];
  const char *pn=NULL;
  char *pOpn=NULL, *pCls=NULL;
  if (pCopyFrom==NULL) 
    {
    strcpy(Str, pCopyFrom ? pCopyFrom->VName() : pInitString);
    XStrTrim(Str, "\"");
    pOpn=strchr(Str, '(');
    pCls=strchr(Str, ')');
    }
  else
    strcpy(Str, pCopyFrom->VName());

  if ((pOpn && pCls) || pCopyFrom)
    {
    if (pOpn && pCls)
      {
      SpDB_ParseTokens(pOpn+1, Cols);
      *pOpn=0;
      *pCls=0;
      }
    if (_stricmp(Str, SDBE_Const::Name())==0) 
      {
      pn=SDBE_Const::Name();
      p=new SDBE_Const(pSpD);
      }
    else if (_stricmp(Str, SDBE_Poly::Name())==0) 
      {
      pn=SDBE_Poly::Name();
      p=new SDBE_Poly(pSpD);
      }
    else if (_stricmp(Str, SDBE_LinearGasDensity::Name())==0) 
      {
      pn=SDBE_LinearGasDensity::Name();
      p=new SDBE_LinearGasDensity(pSpD);
      }
    else if (_stricmp(Str, SDBE_IdealGasDensity::Name())==0) 
      {
      pn=SDBE_IdealGasDensity::Name();
      p=new SDBE_IdealGasDensity(pSpD);
      }
    else if (_stricmp(Str, SDBE_LiqH2ORho::Name())==0) 
      {
      pn=SDBE_LiqH2ORho::Name();
      p=new SDBE_LiqH2ORho(pSpD);
      }
    else if (_stricmp(Str, SDBE_Ts::Name())==0) 
      {
      pn=SDBE_Ts::Name();
      p=new SDBE_Ts(pSpD);
      }
    else if (_stricmp(Str, SDBE_CRC_Cp::Name())==0) 
      {
      pn=SDBE_CRC_Cp::Name();
      p=new SDBE_CRC_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_CRC1_Cp::Name())==0) 
      {
      pn=SDBE_CRC1_Cp::Name();
      p=new SDBE_CRC1_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_HSC_Cp::Name())==0) 
      {
      pn=SDBE_HSC_Cp::Name();
      p=new SDBE_HSC_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_Poly_Cp::Name())==0) 
      {
      pn=SDBE_Poly_Cp::Name();
      p=new SDBE_Poly_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_API_Cp::Name())==0) 
      {
      pn=SDBE_API_Cp::Name();
      p=new SDBE_API_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_HTE_Cp::Name())==0) 
      {
      pn=SDBE_HTE_Cp::Name();
      p=new SDBE_HTE_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_Shomate_Cp::Name())==0) 
      {
      pn=SDBE_Shomate_Cp::Name();
      p=new SDBE_Shomate_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_QAL_Cp::Name())==0) 
      {
      pn=SDBE_QAL_Cp::Name();
      p=new SDBE_QAL_Cp(pSpD);
      }
    else if (_stricmp(Str, SDBE_Vp::Name())==0) 
      {
      pn=SDBE_Vp::Name();
      p=new SDBE_Vp(pSpD);
      }
    else if (_stricmp(Str, SDBE_VpKPa::Name())==0) 
      {
      pn=SDBE_VpKPa::Name();
      p=new SDBE_VpKPa(pSpD);
      }
    else if (_stricmp(Str, SDBE_VpAtm::Name())==0) 
      {
      pn=SDBE_VpAtm::Name();
      p=new SDBE_VpAtm(pSpD);
      }

#if WithIAFH2OMdl
    else if (TryConstructIAF(pSpD, Str, pn, p))
      {
      }
#endif

#if WithIF97H2OMdl
    else if (TryConstructIF97(pSpD, Str, pn, p))
      {
      }
#endif

#if WithNBSH2OMdl
    else if (TryConstructNBS(pSpD, Str, pn, p))
      {
      }
#endif

#if WithSatH2OMdl
    else if (_stricmp(Str, SatOnly_WaterDensity::Name())==0) 
      {
      pn=SatOnly_WaterDensity::Name();
      p=new SatOnly_WaterDensity(pSpD);
      }
    else if (_stricmp(Str, SatOnly_SteamDensity::Name())==0) 
      {
      pn=SatOnly_SteamDensity::Name();
      p=new SatOnly_SteamDensity(pSpD);
      }
    else if (_stricmp(Str, SatOnly_WaterHeatCapacity::Name())==0) 
      {
      pn=SatOnly_WaterHeatCapacity::Name();
      p=new SatOnly_WaterHeatCapacity(pSpD);
      }
    else if (_stricmp(Str, SatOnly_SteamHeatCapacity::Name())==0) 
      {
      pn=SatOnly_SteamHeatCapacity::Name();
      p=new SatOnly_SteamHeatCapacity(pSpD);
      }
    else if (_stricmp(Str, SatOnly_WaterEnthalpy::Name())==0) 
      {
      pn=SatOnly_WaterEnthalpy::Name();
      p=new SatOnly_WaterEnthalpy(pSpD);
      }
    else if (_stricmp(Str, SatOnly_SteamEnthalpy::Name())==0) 
      {
      pn=SatOnly_SteamEnthalpy::Name();
      p=new SatOnly_SteamEnthalpy(pSpD);
      }
    else if (_stricmp(Str, SatOnly_WaterVapourPress::Name())==0) 
      {
      pn=SatOnly_WaterVapourPress::Name();
      p=new SatOnly_WaterVapourPress(pSpD);
      }
#endif

#if WithHFC134a
    
    else if (_stricmp(Str, SDBE_HFC134a_GasCp::Name())==0) 
      {
      pn=SDBE_HFC134a_GasCp::Name();
      p=new SDBE_HFC134a_GasCp(pSpD);
      }
    else if (_stricmp(Str, SDBE_HFC134a_LiqRho::Name())==0) 
      {
      pn=SDBE_HFC134a_LiqRho::Name();
      p=new SDBE_HFC134a_LiqRho(pSpD);
      }
    else if (_stricmp(Str, SDBE_HFC134a_Vp::Name())==0) 
      {
      pn=SDBE_HFC134a_Vp::Name();
      p=new SDBE_HFC134a_Vp(pSpD);
      }

#endif

    if (p)
      {
      if (pCopyFrom)
        p->Copy(pCopyFrom);
      else if (p->Load(pInitString, Cols)!=0)
        LogError("SpecieDB", LF_Exclamation, "Bad Parameters %s for %s", pInitString, pSpD->m_pSp->sCmpTag());
      }
    }
  else
    {
    if (strlen(pInitString)>0 && (pOpn || pCls || (!isdigit(Str[0]) && Str[0]!='.' && Str[0]!='-')))
      LogError("SpecieDB", LF_Exclamation, "Bad CSpDBEqn %s for %s", pInitString, pSpD->m_pSp->sCmpTag());
    pn=SDBE_Const::Name();
    SpDB_ParseTokens(Str, Cols);
    p=new SDBE_Const(pSpD);
    p->Load(Str, Cols);
    }
  
  if (!p)
    {
    LogError("SpecieDB", LF_Exclamation, "CSpDBEqn %s not found for %s", pInitString, pSpD->m_pSp->sCmpTag());
    pn=SDBE_Const::Name();
    strcpy(Str, "Const(1.0,0,0,0)");
    SpDB_ParseTokens(Str, Cols);
    p=new SDBE_Const(pSpD);
    p->Load(Str, Cols);
    }

  return p;
  };

// ==========================================================================
//
//
//
// ===========================================================================

H2OPropertiesBlock::H2OPropertiesBlock(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach)
  : TaggedObject(pClass_, pTag, pAttach, eAttach)
  {
  m_Common.m_dMoleWt  = 18.01528;
  m_Common.m_bPcOK    = true;
  m_Common.m_dPc      = 22.06*1000.0;   // Critical Pressure mPa to kPa
  m_Common.m_bTcOK    = true;
  m_Common.m_dTc      = 647.14;         // Critical Temp
  m_Common.m_bVcOK    = true;
  m_Common.m_dVc      = 0.0568;         // Critical Volume
  m_Common.m_bAcOK    = true;
  m_Common.m_dAc      = 0.3442;         // Acentricity
  m_Common.m_dMv      = 22.414;         // Molar Volume - 22.414 is Ideal
  m_Common.m_dMd      = dNAN;           // MolecularDiam
 
  m_Liq[0].m_pSp         = NULL;
  m_Liq[0].m_dTs[0]      = 273.16;
  m_Liq[0].m_dTe[0]      = 623.16;
  m_Liq[0].m_bCpOK       = true;
  m_Liq[0].m_bH25OK      = true;
  m_Liq[0].m_bS25OK      = true;
  m_Liq[0].m_dTRef       = C_2_K(25);
  m_Liq[0].m_dH25[0]     = -285829.970;
  m_Liq[0].m_dS25[0]     = 69.948;
  m_Liq[0].m_dCZRef[0]   = 0;
  m_Liq[0].m_dCdHf[0]    = 0;
  m_Liq[0].m_dCHf[0]     = 0;
  m_Liq[0].m_dCSf[0]     = 0;
  m_Liq[0].m_pRho[0]     = NULL;
  m_Liq[0].m_pCp[0]      = NULL;
  m_Liq[0].m_pH[0]       = NULL;
  m_Liq[0].m_pS[0]       = NULL;
  m_Liq[0].m_bVpOK       = true;
  m_Liq[0].m_pVp[0]      = NULL;
  m_Liq[0].m_dCpCv       = 1.37;           // Cp/Cv Ratio
                        
  m_Vap[0].m_pSp         = NULL;
  m_Vap[0].m_dTs[0]      = 273.16;
  m_Vap[0].m_dTe[0]      = 1073.16;
  m_Vap[0].m_bCpOK       = true;
  m_Vap[0].m_bH25OK      = true;
  m_Vap[0].m_bS25OK      = true;
  m_Vap[0].m_dTRef       = C_2_K(25);
  m_Vap[0].m_dH25[0]     = -241826.833;
  m_Vap[0].m_dS25[0]     = 188.832;
  m_Vap[0].m_dCZRef[0]   = 0;
  m_Vap[0].m_dCdHf[0]    = 0;
  m_Vap[0].m_dCHf[0]     = 0;
  m_Vap[0].m_dCSf[0]     = 0;
  m_Vap[0].m_pRho[0]     = NULL;
  m_Vap[0].m_pCp[0]      = NULL;
  m_Vap[0].m_pH[0]       = NULL;
  m_Vap[0].m_pS[0]       = NULL;
  m_Vap[0].m_bVpOK       = true;
  m_Vap[0].m_pVp[0]      = NULL;
  m_Vap[0].m_dCpCv       = 1.37;           // Cp/Cv Ratio
       
  };

// ==========================================================================

H2OPropertiesBlock::~H2OPropertiesBlock()
  {
  };

// ==========================================================================
//
//
//
// ===========================================================================


