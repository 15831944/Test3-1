//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DATACNVS_H
#define  __DATACNVS_H

#undef DllImportExport

#ifdef __DATACNVS_CPP
#define DllImportExport DllExport
#elif !defined(SCDLIB)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif

#include "..\..\..\SMDK\Include\md_share1.h"

//===========================================================================
// User Configurable Constants

// Other Functions are in md_share1.h

extern DllImportExport void SetStdPress(double P);
extern DllImportExport void SetStdTemp(double T);
extern DllImportExport void SetNormPress(double P);
extern DllImportExport void SetNormTemp(double T);

// for backward compatibility
#define Std_P           StdPress()
#define Std_T           StdTemp()
#define Norm_P          NormPress()
#define Norm_T          NormTemp()

//===========================================================================
// Atmospheric Conditions

// Other Functions are in md_share1.h

extern DllImportExport void  SetBaseElevation(double E);
extern DllImportExport void  SetWindSpeed(double V);
extern DllImportExport void  SetWindDirection(double V);
extern DllImportExport void  SetRelHumidity(double V);
extern DllImportExport void  SetAmbientTemp(double V);
extern DllImportExport void  InitialiseEnvironVars();

//===========================================================================

inline double PBPress()  { return AtmosPress(); }; //ProBal Pressure used in ""most"" units

//============================================================================

extern DllImportExport CCnvIndex    DC_        ;
extern DllImportExport CCnvIndex    DC_Ang     ;
extern DllImportExport CCnvIndex    DC_DP      ;
extern DllImportExport CCnvIndex    DC_PpS     ;
extern DllImportExport CCnvIndex    DC_PpM     ;
extern DllImportExport CCnvIndex    DC_PpVol   ;
extern DllImportExport CCnvIndex    DC_E       ;
extern DllImportExport CCnvIndex    DC_Frac    ;
extern DllImportExport CCnvIndex    DC_FracRate;
extern DllImportExport CCnvIndex    DC_Assay   ;
extern DllImportExport CCnvIndex    DC_HCap    ;
extern DllImportExport CCnvIndex    DC_HCapF   ;
extern DllImportExport CCnvIndex    DC_L       ;
extern DllImportExport CCnvIndex    DC_Ldt     ;
extern DllImportExport CCnvIndex    DC_M       ;
extern DllImportExport CCnvIndex    DC_Qm      ;
extern DllImportExport CCnvIndex    DC_QmSlew  ;
extern DllImportExport CCnvIndex    DC_Qv      ;
extern DllImportExport CCnvIndex    DC_QvSlew  ;
extern DllImportExport CCnvIndex    DC_Qe      ;
extern DllImportExport CCnvIndex    DC_NQv     ;
extern DllImportExport CCnvIndex    DC_NVol    ;
extern DllImportExport CCnvIndex    DC_P       ;
extern DllImportExport CCnvIndex    DC_Pwr     ;
extern DllImportExport CCnvIndex    DC_pS      ;
extern DllImportExport CCnvIndex    DC_Rho     ;
extern DllImportExport CCnvIndex    DC_T       ;
extern DllImportExport CCnvIndex    DC_Time    ;
extern DllImportExport CCnvIndex    DC_Vol     ;
extern DllImportExport CCnvIndex    DC_WI      ;
extern DllImportExport CCnvIndex    DC_HMs     ;
extern DllImportExport CCnvIndex    DC_HMl     ;
extern DllImportExport CCnvIndex    DC_HV      ;
extern DllImportExport CCnvIndex    DC_HNV     ;
extern DllImportExport CCnvIndex    DC_SMs     ;
extern DllImportExport CCnvIndex    DC_SMl     ;
extern DllImportExport CCnvIndex    DC_CpMs    ;
extern DllImportExport CCnvIndex    DC_CpMl    ;
extern DllImportExport CCnvIndex    DC_Conc    ;
extern DllImportExport CCnvIndex    DC_Area    ;
extern DllImportExport CCnvIndex    DC_HTC     ;
extern DllImportExport CCnvIndex    DC_HCond   ;
extern DllImportExport CCnvIndex    DC_UA      ;
extern DllImportExport CCnvIndex    DC_dT      ;
extern DllImportExport CCnvIndex    DC_dTdt    ;
extern DllImportExport CCnvIndex    DC_I       ;
extern DllImportExport CCnvIndex    DC_V       ;
extern DllImportExport CCnvIndex    DC_Epm     ;
extern DllImportExport CCnvIndex    DC_Visc    ;
extern DllImportExport CCnvIndex    DC_MpL     ;
extern DllImportExport CCnvIndex    DC_KgMl    ;
extern DllImportExport CCnvIndex    DC_QKgMl   ;
extern DllImportExport CCnvIndex    DC_Cond    ;
extern DllImportExport CCnvIndex    DC_SpCnt   ;
extern DllImportExport CCnvIndex    DC_Cnt     ;
extern DllImportExport CCnvIndex    DC_CntRate ;
extern DllImportExport CCnvIndex    DC_SurfTens;
extern DllImportExport CCnvIndex    DC_EntRate ;
extern DllImportExport CCnvIndex    DC_DPperVol;
extern DllImportExport CCnvIndex    DC_SurfAreaM;
extern DllImportExport CCnvIndex    DC_SurfAreaL;
extern DllImportExport CCnvIndex    DC_FracSol ;
extern DllImportExport CCnvIndex    DC_FracLiq ;
extern DllImportExport CCnvIndex    DC_FracVap ;
extern DllImportExport CCnvIndex    DC_FracPh00;
extern DllImportExport CCnvIndex    DC_FracPh01;
extern DllImportExport CCnvIndex    DC_FracPh02;
extern DllImportExport CCnvIndex    DC_FracPh03;
extern DllImportExport CCnvIndex    DC_FracPh04;
extern DllImportExport CCnvIndex    DC_FracPh05;
extern DllImportExport CCnvIndex    DC_FracPh06;
extern DllImportExport CCnvIndex    DC_FracPh07;
extern DllImportExport CCnvIndex    DC_FracPh08;
extern DllImportExport CCnvIndex    DC_FracPh09;
extern DllImportExport CCnvIndex    DC_FracPh10;
extern DllImportExport CCnvIndex    DC_FracPh11;
extern DllImportExport CCnvIndex    DC_FracPh12;
extern DllImportExport CCnvIndex    DC_FracPh13;
extern DllImportExport CCnvIndex    DC_FracPh14;
extern DllImportExport CCnvIndex    DC_FracPh15;
extern DllImportExport CCnvIndex    DC_FracPh16;
extern DllImportExport CCnvIndex    DC_FracPh17;
extern DllImportExport CCnvIndex    DC_FracPh18;
extern DllImportExport CCnvIndex    DC_FracPh19;
extern DllImportExport CCnvIndex    DC_FracPh20;
extern DllImportExport CCnvIndex    DC_FracPh21;
extern DllImportExport CCnvIndex    DC_FracPh22;
extern DllImportExport CCnvIndex    DC_FracPh23;
extern DllImportExport CCnvIndex    DC_FracPh24;
extern DllImportExport CCnvIndex    DC_FracPh25;
extern DllImportExport CCnvIndex    DC_FracPh26;
extern DllImportExport CCnvIndex    DC_FracPh27;
extern DllImportExport CCnvIndex    DC_FracPh28;
extern DllImportExport CCnvIndex    DC_FracPh29;
extern DllImportExport CCnvIndex    DC_FracPh30;
extern DllImportExport CCnvIndex    DC_FracPh31;

extern DllImportExport CCnvIndex    DC_ThermTau;
extern DllImportExport CCnvIndex    DC_CosPhi  ;
extern DllImportExport CCnvIndex    DC_Ohms    ;
extern DllImportExport CCnvIndex    DC_Bytes   ;
extern DllImportExport CCnvIndex    DC_ConcRate;
extern DllImportExport CCnvIndex    DC_KinVisc ;
extern DllImportExport CCnvIndex    DC_ElectVA ;
extern DllImportExport CCnvIndex    DC_Humidity;

extern DllImportExport CCnvIndex    DC_TimeStr ;
extern DllImportExport CCnvIndex    DC_dTimeStr;

extern DllImportExport CCnvIndex    DC_Money;
extern DllImportExport CCnvIndex    DC_MoneyFlow;

//CNM extern DllImportExport CCnvIndex    DC_IsenHd;

//ScdComTempl.h
//#define Gc              9.81
//#define Gc_Kpa          (9.81 * 0.001)
// //Universal Gas Constant = 8.314472  +/- 0.000015 J /mol, K
////see: http://nvl.nist.gov/pub/nistpubs/sp958-lide/339-343.pdf  http://physics.nist.gov/cuu/Constants/index.html  http://www.codata.org/
//#define R_c             (8.314472  /* kPa, m^3/kg-mole, K */)
////#define R_c             (8.314  /* kPa, m^3/kg-mole, K */)
//#define ZeroCinK        273.16
//
//#define PSI_per_kPa     0.145038
//#define GPM_per_m3s     (4.403*3600.0)
//#define kg_per_lb       0.4535924
//#define m3_per_ft3      0.028316847
//#define m_per_ft        0.3048

//===========================================================================

_FWDDEF(CDataCnv)
_FWDDEF(CDataCnvFam)

class DllImportExport CDataCnvFam
  {
  friend class CCnvsMngr;
  friend class CDataCnv;
  public :
    CDataCnvFam(char* Fam, char* Desc, double Min, double Max_);
    ~CDataCnvFam();
    inline char* Fam() { return pFam; };
    inline char* Desc() { return pDesc; };
    inline CDataCnv* SICnv() { return pSICnv; };
  private :
    char*     pFam;    //conversion family
    char*     pDesc;   //conversion family description
    double    dMin;    //resonable min for range of SI unit
    double    dMax;    //resonable max for range of SI unit
    CDataCnv* pSICnv;  //pointer to first conversion in family (SI unit)
    Strng     sOvrTxt; //global conversion override selected by user
  };

class DllImportExport CDataCnv
  {
  friend class CCnvsMngr;
  friend class CDataCnvFam;
  public :
    CDataCnv(CDataCnvFam* SI, char* Txt, double Scl, double Off, flag PrjCnv, long iXform=0);
    ~CDataCnv();
    CDataCnv* Find(char* SecTxt);
    CDataCnv* AddSecondaryCnv(char* Txt, double Scl, double Off, flag PrjCnv);

    inline double Normal(double x, char* SecTxt/*=NULL*/)
      { if (!Valid(x)) return x; if (SecTxt) return At(SecTxt)->PrivNormal(x); return PrivNormal(x); };
    inline double Normal(double x) { if (!Valid(x)) return x; return PrivNormal(x); };
    inline double Human(double x, char* SecTxt/*=NULL*/)
      { if (!Valid(x)) return x; if (SecTxt) return At(SecTxt)->PrivHuman(x); return PrivHuman(x); };
    inline double Human(double x) { if (!Valid(x)) return x; return PrivHuman(x); };
    inline double MinValue() { return PrivHuman(pSI->dMin); };
    inline double MaxValue() { return PrivHuman(pSI->dMax); };
    inline CDataCnv* SICnv() { return pSI->pSICnv; };
    inline char* Fam() { return pSI->pFam; };
    inline char* Desc() { return pSI->pDesc; };
    inline char* Txt() { return pTxt; };
    inline double SclValue() { return dScl; };
    inline double OffValue() { return dOff*dOffMult; };
    inline flag PrjCnv() { return bPrjCnv; };

    inline CDataCnv* Next() { return m_pNxt; };

    inline char* OverTxt() { return pSI->sOvrTxt(); };
    inline void  SetOverTxt(char *p) { pSI->sOvrTxt=p; };

    long       Count();
    long       SecIndex() { return m_iInx; };
    CDataCnv* operator[](long Index);
    double     XformNormal(double x);
    double     XformHuman(double x);
    long       Xform() { return m_iXform; };
    LPCTSTR    XformName();

  private:
    CDataCnv* At(char* SecTxt);
    inline double PrivNormal(double x)
      {
      if (m_iXform)
        return XformNormal(x);
      return ((x-dOff*dOffMult)/dScl);
      };
    inline double PrivHuman(double x)
      {
      if (m_iXform)
        return XformHuman(x);
      return ((x*dScl)+dOff*dOffMult);
      };
    void Clear();

    CDataCnvFam* pSI;     //conversion family
    char*        pTxt;    //conversion text
    double       dScl;    //scale
    double       dOff;    //offset
    double       dOffMult;    //offset
    flag         bPrjCnv;        //is cnv defined by project/dll load (ie NOT from master db at load time)
    CDataCnv*    m_pNxt;    //next in family
    long         m_iInx;
    long         m_iXform; // transform formula


  };

extern DllImportExport CArray<CDataCnv*, CDataCnv*> Cnvs;

//===========================================================================

class DllImportExport CCnvsMngr
  {
  public:
    CCnvsMngr();
    ~CCnvsMngr();
    void Init();
    void Reset();
    CCnvIndex AddSI(char* Fam, char* Txt, char* Desc, double Min, double Max_, flag PrjCnv, long iXform=0);
    int LoadCnvsADO(char* Filename, flag PrjCnv, BOOL LogErrs = TRUE);
    void DumpCnvs();
    CCnvIndex FindCnv(char* pFamName);
    CCnvIndex AddPri(char* Fam, char* Txt, double Min, double Max_, char* Desc, flag PrjCnv);
    CCnvIndex AddSec(CCnvIndex index, char* Txt, double Scl, double Off, flag PrjCnv);
#if WITHCNVINDEXCLASS
    inline CDataCnv* FindSecCnv(CCnvIndex DC, char* pSecTxt) { return Cnvs[DC.Index]->Find(pSecTxt); };
    inline CCnvIndex CnvCnt() { return Cnvs.GetSize(); };
    inline CDataCnv* SICnv(CCnvIndex DC) { return Cnvs[DC.Index]->SICnv(); };
#else
    inline CDataCnv* FindSecCnv(CCnvIndex DC, char* pSecTxt) { return Cnvs[DC]->Find(pSecTxt); };
    inline CCnvIndex CnvCnt() { return Cnvs.GetSize(); };
    inline CDataCnv* SICnv(CCnvIndex DC) { return Cnvs[DC]->SICnv(); };
#endif
    void  Clear();
    void  Recover();
    void  Save();

    //void SetAtmosPressMult(double M);

  private:

    //BOOL LoadCnvLineCSV(DWORD LineNo, char* pLine, BOOL LogErrs, flag PrjCnv);
    BOOL ParseCnvLineADO(int LineNo, CStringArray& Values, BOOL LogErrs, flag PrjCnv);

  };

//===========================================================================

extern DllImportExport CCnvsMngr gs_CnvsMngr;

//===========================================================================

class DllImportExport CCnvFamily
  {
  public:
    int    iCnv;
    char * pCnvTxt;
    //    char * pCnvFam;
    char * pFamily;

    CCnvFamily()
      {
      iCnv=0;
      pCnvTxt="";
      pFamily="";
      }
  };

//===========================================================================

#define DATACNV_PRI(I, PS, Mn, Mx, Desc, PrjCnv) static CCnvIndex I=gs_CnvsMngr.AddPri(#I, PS, Mn, Mx, Desc, PrjCnv);

#define DATACNV_SEC0(I, N) I##N
#define DATACNV_SEC1(I, N) DATACNV_SEC0(I, N)

#define DATACNV_SEC(I, SS, Scl, Off, Mn, Mx, PrjCnv) static CCnvIndex DATACNV_SEC1(I,__LINE__)=gs_CnvsMngr.AddSec(I, SS, Scl, Off, PrjCnv);


//===========================================================================

inline bool IsTimeCnv(CCnvIndex i) { return i>=DC_TimeStr && i<=DC_dTimeStr; };

//===========================================================================

DllImportExport void AppendCnvText(char *buff, char * CnvTxt);
DllImportExport void AppendCnvText(Strng &buff, char * CnvTxt);

//===========================================================================

#undef DllImportExport

#endif



