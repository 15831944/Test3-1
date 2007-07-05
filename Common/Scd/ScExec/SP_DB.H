//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SP_DB_H
#define  __SP_DB_H

#include "sc_defs.h"
#include "scdver.h"
#include "datacnvs.h"
#include "vectors.h"
#include "scdtempl.h"
#include "datatype.h"
#include "tagobj.h"
#include "propertybase.h"

#if defined(__SP_DB_CPP) || defined(__SP_DBEQN_CPP)
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define dbgBuildSDB    (0 || WITHDEBUG)
#define dbgSysVec      (0 /*|| WITHDEBUG*/)

#define WITHSPVECTORTIMERS (WITHTIMERS)
#if WITHSPVECTORTIMERS
  //#define SPMDLTIMERLAP(x) CStopWatchLap SW##__LINE__(SpModel::x)
  //#define SPMDLTIMESTUFF(x) x;
  //#define SPMARRAYTIMERLAP(x) CStopWatchLap SW##__LINE__(CSysVector::x)
  #define SPMVECTORTIMERLAP(x) CStopWatchLap SW##__LINE__(CSysVector::x)
#else
  //#define SPMDLTIMERLAP(x)
  //#define SPMDLTIMESTUFF(x)
  //#define SPMARRAYTIMERLAP(x)
  #define SPMVECTORTIMERLAP(x)
#endif

#define WITHDEBUGDUMPENTHALPY  0

// ===========================================================================

// Switch on to do Solution Stuff
#define WITHSOLNINFO     1

// ===========================================================================
//
//  Basic Units are MKS.
//
//  length m
//  mass   kg
//  time   secs
//  mole   kg-Mole
//  energy kJ
//  pres   kPa
// ===========================================================================

#define  WithSIMPLESPMDL 1
#define  BaseSpModelName        "Standard"
#define  SimpleSpModelName      "Simple"
#define  MassWtMnSpModelName    "Mass WT Mean"
#define  SteamWaterSpModelName  "Steam/Water"

#if WithSatH2OMdl
#define  SaturatedH2OName       "SaturatedH2O"
#endif

#if WithIAFH2OMdl
#define  IFC67H2OName           "IFC67H2O"
#endif

#if WithIF97H2OMdl
#define  IF97H2OName            "IF97H2O"
#endif

#if WithNBSH2OMdl
#define  NBSH2OName             "NBSH2O"
#endif

inline void CheckSpMdlName(Strng & Mdl)
  {
  if (Mdl.XStrICmp(MassWtMnSpModelName)==0)
    Mdl=BaseSpModelName;
  else if (Mdl.XStrICmp(SteamWaterSpModelName)==0)
    Mdl=BaseSpModelName;
  else if (Mdl.XStrICmp(SimpleSpModelName)==0)
    Mdl=BaseSpModelName;
  else if (Mdl.XStrICmp("BaseSpMdl")==0)
    Mdl=BaseSpModelName;
  };

const double IF97_MaxSatT = 647.096; //critical temperature in K
const double IF97_MaxSatP = 22064.0; //critical pressure in kPa

// ===========================================================================

const long MaxPhases = 32; /* Upto 32 : number of bits in dword. */
typedef dword PhMask;

// ===========================================================================
// Constants
const int MaxElements       = 120;
const int MaxSpecies        = 2000;
const int MaxAttributes     = 2000;
const int MaxCalculations   = 2000;
const int MaxComponents     = 2000;
const int MaxSPDBCols       = 100;
const int MaxSpeciesEx      = MaxSpecies+10;         // Allow Space for Extended SpIds
const int MaxComponentsEx   = MaxComponents+10;
const int MaxFidelity       = 2;

//Basic Occurence Type
const int BOT_Solid         = 0;
const int BOT_Liquid        = 1;
const int BOT_Gas           = 2;
const int BOT_Last          = 3;
const int BOT_All           = BOT_Last;
const int BOT_Null          = 4;

//SpecieList Types;
const int spl_SpecieId      = 0;
const int spl_SolSum        = 1;
const int spl_LiqSum        = 2;
const int spl_VapSum        = 3;
const int spl_AllSum        = 4;
const int spl_Text          = 5;
const int spl_Page          = 6;

const int spl_Text_Old      = 4;
const int spl_Page_Old      = 5;
const int spl_AllSum_Old    = 6;

const int SpVwPhMaskIndex_All        = 0;
const int SpVwPhMaskIndex_Solid      = 1;
const int SpVwPhMaskIndex_Liquid     = 2;
const int SpVwPhMaskIndex_Gas        = 3;
const int SpVwPhMaskIndex_Phase0     = 4;
const int SpVwPhMaskIndex_Count      = SpVwPhMaskIndex_Phase0+MaxPhases;

extern DllImportExport double BigVapP;
extern DllImportExport double DefaultRho;
extern DllImportExport double MeasTolerance;
extern DllImportExport double FracMeasTolerance;

inline double IsTraceMass(double Qm) { return Qm >= TraceMass; };
inline double IsUsableMass(double Qm) { return Qm >= UsableMass; };

class CSVLongArray : public CArray<long, long> {};
class CSVBoolArray : public CArray<bool, bool> {};

// ===========================================================================

typedef double *CSVData;

// ===========================================================================

extern DllImportExport long FixSpecieTypeId(long Type, long CfgFilePrjFileVerNo);
extern double SpDB_TokenValue(pchar p, double Default);
extern void SpDB_ParseTokens(char * Buff, char* C[MaxSPDBCols]);

// ===========================================================================

class CElementD;
class CCElementDataBase;
class CSpConstData;
class CSpCommonData;
class CSpecie;
class CSpecieDataBase;
class CComponent;
class CDBInitRec;
class CDBPhaseSeq;
class CComponentDataBase;
class CSpecieBlk;
class CH2ODataBase;
class SpVector;
class CSpFidelData;
class CSysVector;
class SpMArray;
class CSpeciePropDataBase;
class H2OPropertiesBlock;

// ===========================================================================

class CCompFrac
  {
  public:
    int            iSpNo;
    double         dMassFrac;
  };

typedef CArray <CCompFrac, CCompFrac&> CompFracArray;

class CElemComp
  {
  public:
    CElemComp() { iENo=-1; dMoles=0.0; };
    CElemComp(int ENo, double Moles) { iENo=ENo; dMoles=Moles; };

  public:
    int            iENo;
    double         dMoles;
  };

typedef CArray <CElemComp, CElemComp&> ElemCompArray;

class CElementD
  {
  public:
    CElementD()
      {
      m_Name=NULL;
      m_AtmNo=0;
      m_AtmWt=1.0;
      m_SpComp=NULL;
      m_fAllocdMem=false;
      }
    CElementD(pchar  pName, int AtomicNo, double AtomicWt)
      {
      m_Name=pName;
      m_AtmNo=AtomicNo;
      m_AtmWt=AtomicWt;
      m_SpComp=NULL;
      m_fAllocdMem=false;
      };

  public:
    LPSTR            m_Name;
    int              m_AtmNo;
    double           m_AtmWt;

    CompFracArray   *m_SpComp;
    flag             m_fAllocdMem;
  };

class DllImportExport CCElementDataBase
  {
  friend class MElementsInUse;
  friend class MPeriodicTable;
  friend class CComponentDataBase;
  public:
    CCElementDataBase();
    ~CCElementDataBase();

    void           Init();
    void           Term();

    CElementD     & operator [](int s);
    int            Find(const char * pElName);
    flag           CheckElDef(Strng &ElDef);
    int            Count() {return m_iNo;};

  protected:
    int            m_iNo, m_iNoStart;

    CArray <CElementD, CElementD&> m_Data;
    CArray <CElementD*, CElementD*> m_InUse;

  public:
    DDBValueLstMem        DDBElementList;
  };


// ===========================================================================

DllImportExport void GetElements (Strng &ElDef, Strng &Els);
DllImportExport double GetElemAmt (Strng &ElDef, Strng& Els);

// ===========================================================================
//
//
//
// ==========================================================================

class DllImportExport CSpDBEqn
  {
  friend class CSpDBEqn;
  friend class CSpecie;
  friend class CSolnItem;
  friend class CComponent;
  friend class CSpecieDataBase;
  friend class CComponentDataBase;

  public:
    CSpDBEqn(CSpConstData *pConst, CSpFidelData *pFData);
    virtual ~CSpDBEqn();
    virtual int    Load(const char * pInitString, char* Cols[MaxSPDBCols]) = 0;
    virtual void   Copy(CSpDBEqn *p)=0;
    virtual const char * VName()=0;
    // return Values 0 = OK
    //               1 = Invalid Parameter
    //               2 = Invalid # of Parameters

    void           Attach(CSpConstData *pConst, CSpFidelData *pFData);

    double         PCrit();
    double         TCrit();
    double         VCrit();
    double         ACent();
    bool           PcOK();
    bool           TcOK();
    bool           VcOK();
    bool           AcOK();
    double         MolarVolume();
    double         MolecularDiam();

    // One or more of these to be implemented
  public:
    virtual double AtV() { if (m_bDoTheBreak) DoBreak(); return dNAN; };
    virtual double AtT(double T) { if (m_bDoTheBreak) DoBreak(); return dNAN; };
    virtual double AtP(double P) { if (m_bDoTheBreak) DoBreak(); return dNAN; };
    virtual double AtTP(double T, double P) { if (m_bDoTheBreak) DoBreak(); return dNAN; };
    virtual double Integrate_dT(double T1, double T2, double P) { if (m_bDoTheBreak) DoBreak(); return dNAN; };
    virtual double IntegrateOverT_dT(double T1, double T2, double P) { if (m_bDoTheBreak) DoBreak(); return dNAN; };

    static CSpDBEqn *Construct(CSpConstData *pConst, CSpFidelData *pFData, const char *pInitString, CSpDBEqn *pCopyFrom=NULL);
    static CSpDBEqn *ConstructCopy(CSpConstData *pConst, CSpFidelData *pFData);

  public:
    CSpFidelData    * m_pFData;
    CSpConstData    * m_pConst;
    static bool       m_bDoTheBreak;
  };

// ===========================================================================
//
//
//
// ===========================================================================

//class DllImportExport CSolnItem
//  {
//  public:
//    int      m_iSoluteA;
//    int      m_iSoluteS;             //'OtherPhase' required by SolubleFn
//
//    CSpDBEqn *m_pDensCorrFn;          // Density Correction Function
//    double   m_dDensCorrZero;        // Density Correction Function
//
//    CSpDBEqn *m_pCpCorrFn;            // Cp Correction Function
//    double   m_dCpCorrZero;          // Cp Correction Function
//
//    CSpDBEqn *m_pSolubleFn;           // Solubility Function
//    CSpDBEqn *m_pHeatOfMixFn;
//
//    CSolnItem()
//      {
//      m_iSoluteA=-1;
//      m_iSoluteS=-1;
//      m_pDensCorrFn=NULL;
//      m_dDensCorrZero=0.0;
//      m_pCpCorrFn=NULL;
//      m_dCpCorrZero=0.0;
//      m_pSolubleFn=NULL;
//      m_pHeatOfMixFn=NULL;
//      };
//
//    CSolnItem(int SoluteA, int SoluteS,
//              CSpDBEqn *DensCorrFn, double DensCorrZero,
//              CSpDBEqn *CpCorrFn, double CpCorrZero,
//              CSpDBEqn *SolubleFn, CSpDBEqn *HeatOfMixFn)
//      {
//      m_iSoluteA       = SoluteA;
//      m_iSoluteS       = SoluteS;
//      m_pDensCorrFn    = DensCorrFn;
//      m_dDensCorrZero  = DensCorrZero;
//      m_pCpCorrFn      = CpCorrFn;
//      m_dCpCorrZero    = CpCorrZero;
//      m_pSolubleFn     = SolubleFn;
//      m_pHeatOfMixFn   = HeatOfMixFn;
//      };
//
//    ~CSolnItem()
//      {
//      };
//
//    void Clear()
//      {
//      delete m_pDensCorrFn;
//      delete m_pCpCorrFn;
//      delete m_pSolubleFn;
//      delete m_pHeatOfMixFn;
//      m_pDensCorrFn=NULL;
//      m_pCpCorrFn=NULL;
//      m_pSolubleFn=NULL;
//      m_pHeatOfMixFn=NULL;
//      };
//
//    double DensCorr(double MF) { return m_pDensCorrFn->AtT(MF)-m_dDensCorrZero; }
//    double CpCorr(double MF)   { return m_pCpCorrFn->AtT(MF)-m_dCpCorrZero; }
//  };
//
//class CSolnArray : public CArray<CSolnItem, CSolnItem&> {};

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CDensCorr
  {
  public:
    CDensCorr();
    CDensCorr(int Solute, int Solvent,
              CSpDBEqn *DensCorrFn, double DensCorrZero,
              double LimitFrac, double PureValue, bool DoWarn);
    ~CDensCorr();

    void Clear();
    double DensCorr(double MF) { return m_pDensCorrFn->AtT(MF)-m_DensCorrZero; }
    double Evaluate(long Fidelity, double T, double P, CSVData M);

  public:
    int      m_iSolute;
    int      m_iSolvent;

    CSpDBEqn *m_pDensCorrFn;          // Density Correction Function
    double   m_DensCorrZero; 
    double   m_LimitFrac;        
    double   m_PureValue;
    bool     m_DoWarn;

  };

class CDensCorrs : public CArray<CDensCorr, CDensCorr&> {};

// ===========================================================================

class DllImportExport CCpCorr
  {
  public:
    CCpCorr();
    CCpCorr(int Solute, CSpDBEqn *CpCorrFn, double CpCorrZero);
    ~CCpCorr();

    void Clear();
    double CpCorr(double MF)   { return m_pCpCorrFn->AtT(MF)-m_dCpCorrZero; }

  public:
    int        m_iSolute;
    CSpDBEqn * m_pCpCorrFn;            // Cp Correction Function
    double     m_dCpCorrZero;          // Cp Correction Function

  };

class CCpCorrs : public CArray<CCpCorr, CCpCorr&> {};

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CSolubility
  {
  public:
    CSolubility();
    CSolubility(int SoluteA, int SoluteS,
              CSpDBEqn *SolubleFn, CSpDBEqn *HeatOfMixFn);
    ~CSolubility();

    void Clear();

  public:
    int      m_iSoluteA;
    int      m_iSoluteS;             //'OtherPhase' required by SolubleFn

    CSpDBEqn *m_pSolubleFn;           // Solubility Function
    CSpDBEqn *m_pHeatOfMixFn;

  };

class CSolubilities : public CArray<CSolubility, CSolubility&> {};

// ===========================================================================
//
//
//
// ===========================================================================

DllImportExport char * CheckSymbolName(const char * In, Strng & Buff);

// ===========================================================================
//
//
//
// ===========================================================================

typedef word SPOType;

const SPOType SPO_NULL  = 0x0000;

const SPOType SPO_VDens     = 0x0001;
const SPOType SPO_LDens     = 0x0002;
const SPOType SPO_SDens     = 0x0004;
const SPOType SPO_VCp       = 0x0010;
const SPOType SPO_LCp       = 0x0020;
const SPOType SPO_SCp       = 0x0040;
const SPOType SPO_VDynVisc  = 0x0100;
const SPOType SPO_LDynVisc  = 0x0200;
const SPOType SPO_VThermCnd = 0x0400;
const SPOType SPO_LThermCnd = 0x0800;
const SPOType SPO_SurfT     = 0x1000;
const SPOType SPO_BPEle     = 0x2000;

const byte  SPIndex_Vap = 0;
const byte  SPIndex_Liq = 1;
const byte  SPIndex_Sol = 2;

class DllImportExport SpPropOveride
  {
  //public:
  friend class CSpFidelData;
  friend class CSpecie;
  friend class SpModel;
  friend class SpConduit;
  friend class SpContainer;
  friend class CSpecieDataBase;

  public:
    SpPropOveride();
    void CopySrc(SpPropOveride & Src);
    void AddSrc(SpPropOveride & Src);
    flag Use(SPOType Flg) { return (bUse&Flg)!=0;};

  protected:
    SPOType        bAdd;
    SPOType        bRem;
    SPOType        bUse;

  public:
    union
      {
      struct
        {
        float          fVDens;
        float          fLDens;
        float          fSDens;
        };
      float fDens[3];
      };

    union
      {
      struct
        {
        float          fVCp;
        float          fLCp;
        float          fSCp;
        };
      float fCp[3];
      };

    float          fVDynVisc;
    float          fLDynVisc;
    float          fVThermCnd;
    float          fLThermCnd;

    float          fSurfT;

    float          fBPEle;
  };

// ===========================================================================
//
//
//
// ===========================================================================

struct CAcidBaseData 
  {
  LPCTSTR   m_sName;
  bool      m_Acid;
  double    m_dDa[3];
  };

extern DllImportExport CAcidBaseData * FindAcidBaseData(LPCSTR Compound);

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CSpConstData
  {
  public:
    CSpConstData();

    double        MoleWt()           { return m_dMoleWt; };
    double        PCrit()            { return m_dPc; };
    double        TCrit()            { return m_dTc; };
    double        VCrit()            { return m_dVc; };
    double        ACent()            { return m_dAc; };
    double        MolarVolume()      { return m_dMv; };
    double        MolecularDiam()    { return m_dMd; };

    bool          PcOK()             { return m_bPcOK; };
    bool          TcOK()             { return m_bTcOK; };
    bool          VcOK()             { return m_bVcOK; };
    bool          AcOK()             { return m_bAcOK; };
  public:
    double        m_dMoleWt;
    bool          m_bPcOK;
    bool          m_bTcOK;
    bool          m_bVcOK;
    bool          m_bAcOK;
    bool          m_bMvOK;
    bool          m_bMdOK;

    double        m_dPc;                  // Critical Pressure
    double        m_dTc;                  // Critical Temp
    double        m_dVc;                  // Critical Volume
    double        m_dAc;                  // Acentricity
    double        m_dMv;                  // MolarVolume
    double        m_dMd;                  // MolecularDiam

    Strng         m_sPc; 
    Strng         m_sTc; 
    Strng         m_sVc; 
    Strng         m_sAc; 
    Strng         m_sMv; 
    Strng         m_sMd; 

  };

// ---------------------------------------------------------------------------

class DllImportExport CSpCommonData
  {
  public:
    CSpCommonData();

    double        VapourP(double T) { return m_pVp->AtT(T); };
    double        VapourT(double P, double dPrvVapT=dNAN);

    bool          VpOK()            { return m_bVpOK; };

  public:
    bool            m_bVpOK;
    CSpDBEqn      * m_pVp;           // Vapour Pressure Eqn
    Strng           m_sVp;           // 

    CSpConstData  * m_pConst;
    CComponent    * m_pCmp;

  };

typedef CSpCommonData CSpCommonDataA[MaxFidelity];

// ---------------------------------------------------------------------------

const int MaxTPoints=10;
const int SpDBEqnCnt=4;

class DllImportExport CSpFidelData
  {
  friend class CSpDBEqn;
  friend class CComponent;
  friend class CSpecieDataBase;
  friend class CComponentDataBase;
  friend class CSpecie;

  public:
    CSpFidelData();
    ~CSpFidelData();

    void           AddSpecieTData(CSpFidelData *m_pData);

    double         MoleWt();

    bool           H25OK();
    bool           S25OK();

    bool           CpOK();

    double         SpRho(double T, double P);
    double         LoT();
    double         HiT();
    double         TestLoT();
    double         TestHiT();

    double         mlCp(double T, double P) { int i=FindTIndex(T); return m_pCp[i]->AtTP(T, P); };
    double         mlSf(double T, double P) { int i=FindTIndex(T); return (m_pS[i] ? m_pS[i]->AtTP(T, P) : m_pCp[i]->IntegrateOverT_dT(m_dTs[i], GTZ(T), P)); };
    double         mlHf(double T, double P) { int i=FindTIndex(T); return (m_pH[i] ? m_pH[i]->AtTP(T, P) : m_pCp[i]->Integrate_dT(m_dTs[i], T, P)); };

  protected:
    int            FindTIndex(double T);

    double         CpOverTdT(double Tex, double Tad);

    double         CalcH(double T, bool AddConst, int iSeg=-1);
    double         CalcS(double T, bool AddConst, int iSeg=-1);
    double         CalcDH(double T1, double T2, int iSeg=-1);
    double         CalcDS(double T1, double T2, int iSeg=-1);

  public://protected:
    CSpecie        * m_pSp;
    CSpConstData   * m_pConst;
    CSpCommonDataA * m_pCommon;

    bool           m_bIsCopy;
    int            m_iHfBase;
    int            m_iLastTPoint;

    double         m_dTs[MaxTPoints];      // Heat Data validity Range Start
    double         m_dTe[MaxTPoints];      // Heat Data validity Range End

    union
      {
      CSpDBEqn *    m_pEqns[SpDBEqnCnt][MaxTPoints];    //
      struct
        {
        CSpDBEqn *  m_pRho[MaxTPoints];    // Density Eqn
        CSpDBEqn *  m_pCp[MaxTPoints];     // Cp Eqn
        CSpDBEqn *  m_pH[MaxTPoints];      // Enthalpy Eqn
        CSpDBEqn *  m_pS[MaxTPoints];      // Entropy Eqn
        };
      };

    double         m_dH25[MaxTPoints];    // Standard Heat of formation @ 298K
    double         m_dCZRef[MaxTPoints];  // Const of Integration for Zero Ref
    double         m_dCdHf[MaxTPoints];   // Const of Integration for Hf Diff
    double         m_dCHf[MaxTPoints];    // Const of Integration with Heat of Formation

    double         m_dS25[MaxTPoints];    // Standard Entropy @ 298K
    double         m_dCSf[MaxTPoints];    // Const of Integration with Standard Entropy

    double         m_dHf0;                // Standard Heat of formation @ 0C

    double         m_dHfBase0;            // Standard Heat of formation Diff @ 0C
    double         m_dS0;                 // Standard Entropy @ 0C

    double         m_dSBase0;             // Standard Entropy Diff @ 0C
    double         m_dTRef;               // Ref Temp for this Specie

    double         m_dCpCv;

    bool           m_bHasHFn;
    bool           m_bHasSFn;

    bool           m_bH25OK;
    bool           m_bS25OK;

    bool           m_bCpOK;

    byte           m_bChecked:1;

  };

typedef CSpFidelData CSpFidelDataA[MaxFidelity];

// ---------------------------------------------------------------------------

class DllImportExport CSpecie : public CPropertyBase
  {
  friend class CSpDBEqn;
  friend class CComponent;
  friend class CSpecieDataBase;
  friend class CComponentDataBase;
  friend class CSpFidelData;


  public:
    CSpecie(CComponent *pComp, CSpecieDataBase & SDB, CComponentDataBase & CDB, CH2ODataBase & H2ODB, const char * pCmpTag);
    ~CSpecie();

    void SetData(CSpCommonDataA &C, CSpFidelDataA &D);
    // Tag could be blank
    char *        Tag();
    char *        Sym();
    char *        CmpTag();

    flag          LoadADO(CComponent * pC, CStringArray& Values, flag UseH2OFns);

    flag          FixMissingData(CComponent * pC);
    flag          CopyDataForMultiFidelity();
    int           AddSolnInfo(int iSoluteA, int iSoluteS, int iSolvent, char* dDensCorr, char* dHCapCorr, char* pSoluble, char* pHeatOfMix);

    flag          PhaseIs(PhMask OccM__);
    flag          IsSol();
    flag          IsLiq();
    //flag          IsAqs();
    flag          IsGas();
    flag          IsAqueous();
    int           BaseOccType();

    int           CId();

    double        DensCorrMass(CSVData M);
    double        DensCorrFactor(CSVData M);
    double        CpCorrFactor(CSVData M);
    int           DensCorrCount();
    CDensCorr   & DensCorr(int i);
    CDensCorr   & DensCalc();
    int           CpCorrCount();
    CCpCorr     & CpCorr(int i);
    int           SolubilityCount();
    CSolubility & Solubility(int i);
    int           Solvent();

    flag          HasDensCalc() { return m_pDensCalc!=0; };

    flag          CpDirect();
    flag          CpInDirect();


    char *        SymOrTag();
    flag          PrjDB();

    void          Dump(LPCTSTR Location=NULL, long NPts=50);

    int           FindTIndex(long Fidelity, double T);

    double        SpRho(long Fidelity, double T, double P);
    double        msVolume(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        Density(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        Volume(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        NmsVolume(long Fidelity, SpPropOveride *Ovr, CSVData M);
    double        NDensity(long Fidelity, SpPropOveride *Ovr, CSVData M);
    double        NVolume(long Fidelity, SpPropOveride *Ovr, CSVData M);

    double        LoT(long Fidelity);
    double        HiT(long Fidelity);
    double        TestLoT(long Fidelity);
    double        TestHiT(long Fidelity);

    double        CpCv(long Fidelity);

    double        mlHs(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        mlHz(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        mlHf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        mlCp(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        mlSf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        mlGf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        mlHf0(long Fidelity);
    double        mlHfBase0(long Fidelity);
    double        mlCpMean(long Fidelity, double T, double P, double T0, double P0, SpPropOveride *Ovr, CSVData M);

    double        msHs(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        msHz(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        msHf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        msCp(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        msSf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        msGf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M);
    double        msHf0(long Fidelity);
    double        msHfBase0(long Fidelity);

    double        msdHIdeal(long Fidelity, double T1, double T2);
    double        msdSIdeal(long Fidelity, double T1, double T2);

    double        MoleWt()           { return m_Const.m_dMoleWt; };

    double        PCrit()            { return m_Const.m_dPc; };
    double        TCrit()            { return m_Const.m_dTc; };
    double        VCrit()            { return m_Const.m_dVc; };
    double        ACent()            { return m_Const.m_dAc; };
    double        MolarVolume()      { return m_Const.m_dMv; };
    double        MolecularDiam()    { return m_Const.m_dMd; };
    double        VapourP(long Fidelity, double T)                        { return m_Common[Fidelity].VapourP(T); };
    double        VapourT(long Fidelity, double P, double dPrvVapT=dNAN)  { return m_Common[Fidelity].VapourT(P, dPrvVapT); }

    bool          PcOK()             { return m_Const.m_bPcOK; };
    bool          TcOK()             { return m_Const.m_bTcOK; };
    bool          VcOK()             { return m_Const.m_bVcOK; };
    bool          AcOK()             { return m_Const.m_bAcOK; };
    bool          VpOK(long Fidelity)  { return m_Common[Fidelity].m_bVpOK; };

    bool          H25OK();
    bool          S25OK();

    bool          CpOK();

    double        Ka(int i)          { return m_iDaType==DaType_IsAcid ? m_dDa[i]:0.0; };
    double        Kb()               { return m_iDaType==DaType_IsBase ? m_dDa[0]:0.0; };
    LPSTR         KaKbDesc()         { return m_sKaKbDesc(); }
    bool          DaOK()             { return m_bDaOK; };

    // CPropertyBase virtuals
    virtual LPCTSTR DefinedPropertyMapName()                              { return "SpecieDefn"; }
    virtual long    DefinedPropertyCount();
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    virtual DWORD   GetPropertyVisibility(long Index) throw(...);
    virtual void    GetPropertyValue(long Index, DWORD Phase, double T, double P, MPropertyValue &Value) throw(...);
    virtual void    PutPropertyValue(long Index, MPropertyValue &Value) throw(...);

  public:
    Strng           sCmpTag;
    Strng           sTag;
    Strng           sSym;
    Strng           m_sPhaseSymVisible; // this is required for water special
    Strng           m_sPhaseSymActual;
    int             m_eOcc;
    PhMask          m_OccMsk;
    PhMask          m_PhMsk;
    int             m_PhInx;
    int             m_iBOccT;
    double          m_dTsTol;               // Heat Data Test Tolerance Start
    double          m_dTeTol;               // Heat Data Test Tolerance End

    int             iSId;
    int             iCId;

    CComponent     & m_Cmp;
    CSpConstData   & m_Const;
    CSpCommonDataA & m_Common;
    CSpFidelDataA    m_FData;

    CSpecieDataBase     & m_SDB;
    CComponentDataBase  & m_CDB;
    CH2ODataBase        & m_H2O;

    Strng_List       m_sVpErrors;
    flag             m_bVpSpecd;
    flag             m_bTcSpecd;
    flag             m_bPcSpecd;
    flag             m_bVcSpecd;
    flag             m_bAcSpecd;

  protected:

    CSpecie*         pPrev;
    CSpecie*         pNext;
    CSpecie*         pFirst;

    byte             m_bReference:1,
                     m_bPrjDB:1,
                     m_bIsAqueous:1,
                     m_bHasCpCorrs:1,
                     m_bCpInDirect:1;

    SPOType          m_bCpOvrMask;
    SPOType          m_bDensOvrMask;
    byte             m_iOverIndex;

    int              m_iSolvent;
    CDensCorr *      m_pDensCalc;

    CDensCorrs       m_DensCorrs;
    CCpCorrs         m_CpCorrs;
    CSolubilities    m_Solubilities;

    static const byte      DaType_IsNull = 0;
    static const byte      DaType_IsAcid = 1;
    static const byte      DaType_IsBase = 2;

    bool            m_bDaOK;
    byte            m_iDaType;
    double          m_dDa[3];        // Dissociation numbers
    //Strng           m_sDa;           // 
    Strng           m_sKaKbDesc;

  };

// ==========================================================================
//
//
//
//===========================================================================

class DllImportExport CSpIndexArray : public CArray <long, long> {};
class DllImportExport CSpecieDataBase
  {
  friend class CComponentDataBase;
  friend class SpecieIter;
  friend class SpecieIterAll;
  friend class SpModel;
  public:
    CSpecieDataBase();
    ~CSpecieDataBase();

    void           Init();
    void           Term();

    CSpecie &      operator [](int s) { return *m_SpD[s]; };
    
    void           Add(CSpecie *SD);
    CSpecie *      Desc(int s) { return m_SpD[s];};
    pchar          Tag(int s) { return m_SpD[s]->Tag();};
    pchar          Sym(int s) { return m_SpD[s]->Sym();};
    pchar          SymOrTag(int s) { return m_SpD[s]->sSym.Length()>0 ? m_SpD[s]->sSym() : m_SpD[s]->sTag();};

    int            Find(LPCTSTR pSpName);
    int            First(PhMask PhaseM);
    int            Next(int sn, PhMask PhaseM);
    int            Count() { return m_iNo; };
    int            Count(PhMask Ph) { int n=0; for (int i=0; i<m_iNo; i++) if (m_SpD[i]->PhaseIs(Ph)) n++; return n; };
    int            GetIndexArray(PhMask Phases, CSVLongArray*pInx=NULL);  //number of species in Phase
    int            Lookup(LPCTSTR pSpName) { int i; return m_Map.Lookup(pSpName, i) ? i:-1; };

    void           SetExtraProps(CSpeciePropDataBase* pSPDB) { m_pExtraProps = pSPDB; };
    CSpeciePropDataBase* ExtraProps() { return m_pExtraProps; };

    double         msVolume(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         msVolumeFrac(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, CSVData M, CSVData Frac);
    double         Density(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         Density(long Fidelity, CIArray & SpIds, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         Density(long Fidelity, CSysVector & Scale, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         Volume(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         Volume(long Fidelity, CIArray & SpIds, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         Volume(long Fidelity, CSysVector & Scale, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         NDensity(long Fidelity, PhMask Phase, SpPropOveride *Ovr, CSVData M);
    double         NDensity(long Fidelity, CIArray & SpIds, SpPropOveride *Ovr, CSVData M);
    double         NDensity(long Fidelity, CSysVector & Scale, SpPropOveride *Ovr, CSVData M);
    double         NVolume(long Fidelity, PhMask Phase, SpPropOveride *Ovr, CSVData M);
    double         NVolume(long Fidelity, CIArray & SpIds, SpPropOveride *Ovr, CSVData M);
    double         NVolume(long Fidelity, CSysVector & Scale, SpPropOveride *Ovr, CSVData M);
    double         VolFrac(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, CSVData M);
    double         VolFrac(long Fidelity, CIArray & SpIds, double T, double P, SpPropOveride *Ovr, CSVData M);

    double         CpCv(long Fidelity, PhMask Phase, CSVData M);

    double         msCp(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M, double *pTotalM=NULL);
    double         msHs(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M, double *pTotalM=NULL);
    double         msHz(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M, double *pTotalM=NULL);
    double         msHf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M, double *pTotalM=NULL);
    double         msSf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M, double *pTotalM=NULL);
    double         msGf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M, double *pTotalM=NULL);

    double         totCp(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         totHs(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         totHz(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         totHf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         totSf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         totGf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, CSVData M);

    double         msHmMix(long Fidelity, double Fm, double Hm, double Fo, CSVData Mo, double T_, double P_, SpPropOveride *Ovr);
    double         msHmMix(long Fidelity, double FSol, double Hs, double FLiq, double Hl, double FVap, double Hv, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         msCpMix(long Fidelity, double FSol, double Cs, double FLiq, double Cl, double FVap, double Cv, double T_, double P_, SpPropOveride *Ovr, CSVData M);
    double         RhoMix(long Fidelity, double FSol, double Ds, double FLiq, double Dl, double FVap, double Dv, double T_, double P_, SpPropOveride *Ovr, CSVData M);

    double         HeatOfCombustionLo(long Fidelity, PhMask Phase, double T_, double P_, CSVData M);
    double         HeatOfCombustionHi(long Fidelity, PhMask Phase, double T_, double P_, CSVData M);

    static void    AddSpCnv(CCnvIndex DC_Family, char * pSpName, char * CnvRqd, CCnvIndex & dc, pchar & pCnvTxt);

  private:
    CSpecie *      m_SpD[MaxSpecies];
    CMap<LPCTSTR, LPCTSTR, int, int> m_Map;

    CSpeciePropDataBase* m_pExtraProps;

  public:

    CSVLongArray   m_DensCorrSps;
    CSVLongArray   m_CpCorrSps;
    CSVLongArray   m_SolubleSps;
    
    CSVLongArray   m_Acids;
    CSVLongArray   m_Bases;

    int            m_iNo;
    int            m_iOC_Dissociation;
    int            m_iOC_MolecularDiam;
    int            m_iOC_MolVol;
    int            m_iOC_CpCv;
    int            m_iOC_Reference;

    long           m_iH2Ol;
    long           m_iCO2l;
    long           m_iH2Og;
    long           m_iCO2g;

    DDBValueLstMem DDBSpList;
    DDBValueLstMem DDBSpListDash;
    DDBValueLstMem DDBSolSpList;
    DDBValueLstMem DDBSolSpListDash;
    DDBValueLstMem DDBLiqSpList;
    DDBValueLstMem DDBLiqSpListDash;
    DDBValueLstMem DDBVapSpList;
    DDBValueLstMem DDBVapSpListDash;

#if WITHDEBUGDUMPENTHALPY
    static int DebugHDumpOn;
#endif
#ifndef _RELEASE
    static int gs_nDoTestEstTP;
#endif
  };

inline int CSpecieDataBase::First(PhMask PhaseM)
  {
  return Next(-1, PhaseM);
  };
inline int CSpecieDataBase::Next(int sn, PhMask PhaseM)
  {
  sn++;
  while (1)
    {
    if (sn>=m_iNo)
      return -1;
    else if (m_SpD[sn]->PhaseIs(PhaseM))
      return sn;
    else sn++;
    };
  };

//===========================================================================
//
//
//
//===========================================================================

#define H2OPropsGroup "H2OPropertiesGroup"

class DllImportExport H2OPropertiesBlock  : public TaggedObject
  {
  public:
    CSpFidelData  m_Sol[MaxFidelity];
    CSpFidelData  m_Liq[MaxFidelity];
    CSpFidelData  m_Vap[MaxFidelity];
    CSpConstData   m_Const;
    CSpCommonDataA m_Common;

  public:
    H2OPropertiesBlock(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~H2OPropertiesBlock();

    virtual void    BuildDataDefn(DataDefnBlk & DDB)    { };
    virtual flag    DataXchg(DataChangeBlk & DCB)       { return 0;};
    virtual flag    ValidateData(ValidateDataBlk & VDB) { return 0;};
  };

const int MaxH2OMdls=8;
class DllImportExport CH2ODataBase
  {
  protected:
    long    m_nMdls;
    long    m_iSelected;
    H2OPropertiesBlock *m_Mdl[MaxH2OMdls];

  public:
    CH2ODataBase() { m_nMdls=0; };
    ~CH2ODataBase() { Terminate(); };
    void Initialise();
    void Terminate();
    DDBValueLst  * DDBList();
    void Select(long l);
    void Select(const char * ClassId);
    long Selected();
    char* SelectedClassId();
    H2OPropertiesBlock & SelectedBlk();
    };

// ---------------------------------------------------------------------------

#define H2OEQNBEGIN(TYPE, TEXT)                                           \
class TYPE : public CSpDBEqn                                               \
  {                                                                       \
  friend class CSpDBEqn;                                                   \
  friend class H2OPropertiesBlock;                                        \
  public:/*protected:*/                                                   \
                                                                          \
    TYPE (CSpConstData * Const, CSpFidelData * Sp) : CSpDBEqn(Const, Sp) { };  \
    virtual ~TYPE () {};                                                  \
    static const char * Name() { return TEXT; };                          \
    virtual const char * VName() { return Name(); };                      \
    virtual int Load(const char * pInitString, char* Cols[MaxSPDBCols])   \
            { if (strlen(Cols[0])!=0) return 2; return 0;};               \
    virtual void Copy(CSpDBEqn* p) { };                                    \

#define H2OEQNEND };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CComponent : public CSpConstData //CSpCommonData 
  {
  friend class CSpecie;
  friend class CSpecieDataBase;
  friend class CComponentDataBase;

  public:
    CComponent(CSpecieDataBase & SDB, CComponentDataBase & CDB, CH2ODataBase & H2ODB, const char * Sym__);
    ~CComponent();

    char *          SymOrTag();

    double          VapourP(long Fidelity, double T)                        { return m_Common[Fidelity].VapourP(T); }
    double          VapourT(long Fidelity, double P, double dPrvVapT=dNAN)  { return m_Common[Fidelity].VapourT(P, dPrvVapT); }
    bool            VpOK()                      { for (int i=0; i<MaxFidelity; i++) { if (!m_Common[i].VpOK()) return false; } return true; }

    CSpecie *       FindPhaseData(char * pPhaseName);

    int             SolPhCnt();
    int             LiqPhCnt();
    int             VapPhCnt();

    int             SolPhInx(int i=0);
    int             LiqPhInx(int i=0);
    int             VapPhInx(int i=0);
    //int             AqsPhInx();
    int             SolPhInx(LPCSTR PhNm, bool OtherwiseFirst=false);
    int             LiqPhInx(LPCSTR PhNm, bool OtherwiseFirst=false);
    int             VapPhInx(LPCSTR PhNm, bool OtherwiseFirst=false);
    int             CmpInx();

    CSpecie *       pSpecie(int occ);
    int             iSpecie(int occ);
    int             NSpecies();
    CSpecie *       operator[](int occ);

  public:
    Strng           Sym;
    Strng           m_sElDef;
    ElemCompArray & m_ElComp;

  protected:
    CSpecieDataBase & m_SDB;
    CComponentDataBase & m_CDB;
    CH2ODataBase  & m_H2O;

    int             iCId;
    CSpecie *       m_SpPhaseD[MaxPhases]; //Phase Desc Indexed by Phase

    CSpCommonDataA  m_Common; 

    int             m_nPhaseData;
    CSpecie *       m_PhaseData[MaxPhases];  // Phase Desc Packed
    int             m_iPhaseData[MaxPhases]; // Phase Index Packed

    int             FindPhaseDataIndex(double &T);
    int             FindPhaseDataIndex(char * pName);

    byte            m_nAssumedCps;
    byte            m_nDiscarded;
    flag            m_bLiqDone;
    flag            m_bVapDone;

  };

// ===========================================================================

class CRqdSpItem
  {
  public:
    CRqdSpItem()
      {
      m_dLoTol=-1;
      m_dHiTol=-1;
      m_bUseIdeal=true;
      m_iDuplicate=-1;
      };
    CRqdSpItem(LPCTSTR Name)
      {
      m_Name=Name;
      m_dLoTol=-1;
      m_dHiTol=-1;
      m_bUseIdeal=true;
      m_iDuplicate=-1;
      };

  public:
    Strng   m_Name;
    double  m_dLoTol;
    double  m_dHiTol;
    bool    m_bUseIdeal;
    int     m_iDuplicate;
  };

class CRqdSpList : public CList <CRqdSpItem, CRqdSpItem&>
  {
  public:
    CRqdSpItem * Find(LPCTSTR What, flag CaseSensitive = 0)
      {
      POSITION Pos=GetHeadPosition();
      while (Pos)
        {
        CRqdSpItem * p=&GetNext(Pos);
        if (CaseSensitive)
          {
          if (p->m_Name.XStrCmp(What)==0)
            return p;
          }
        else
          {
          if (p->m_Name.XStrICmp(What)==0)
            return p;
          }
        }
      return NULL;
      }
  };
typedef CSCDListIter<CRqdSpList, CRqdSpItem*> CRqdSpListIter;

class DllImportExport CDBInitRec
  {
  public:
    CRqdSpList     RqdSpecies;
    Strng_List     RqdAnnotation;
    double         MinT;
    double         MaxT;
    double         MinP;
    double         MaxP;
    flag           UseStdFns4H2O;
    Strng_List     m_PhNames;

    int            NSpPrj; //number of species loaded from project database
    int            NSpDensCorrPrj; //number of species with DensityCorrFn loaded from project database
    int            NSpSolFnPrj; //number of species with SolubilityFn loaded from project database

    CDBInitRec(double MinT_, double MaxT_, double MinP_, double MaxP_, flag UseStdFns4H2O_);
  };

// ===========================================================================

class DllImportExport CDBPhaseSeq
  {
  public:
    double         T;
    CSpecie *      pPD;
  };

// ===========================================================================

class CPhaseInfo
  {
  public:
    CPhaseInfo()
      {
      m_eOcc  = BOT_Solid;
      m_PhMsk = 0;
      m_PhInx  = 0;
      m_nSpecies = 0;
      };

    void Clear()
      {
      m_Sym="";
      m_Tag="";
      m_eOcc  = BOT_Solid;
      m_PhMsk = 0;
      m_PhInx  = 0;
      m_nSpecies = 0;
      };

  public:
    Strng       m_Sym;
    Strng       m_Tag;
    int         m_eOcc;
    PhMask      m_PhMsk;
    int         m_PhInx;
    int         m_nSpecies;
  };

// ===========================================================================

const int SMFM_Default     = -1;
const int SMFM_PartialP    = 0;
const int SMFM_TotalP      = 1;
const int SMFM_Undefined   = 2;

class CSaturationDefn
  {
  public:
    CSaturationDefn() 
      {
      m_iMethod    = SMFM_Default;
      m_iCmpIndex  = -1;
      };
    CSaturationDefn(CSaturationDefn * pSatDefn) 
      {
      if (pSatDefn)
        {
        m_iMethod    = pSatDefn->m_iMethod;
        m_iCmpIndex  = pSatDefn->m_iCmpIndex;
        }
      else
        {
        m_iMethod    = SMFM_Default;
        m_iCmpIndex  = -1;
        }
      };
    CSaturationDefn(int Method, int CmpIndex) 
      {
      m_iMethod   = Method;
      m_iCmpIndex = CmpIndex;
      };
    CSaturationDefn(const CSaturationDefn & V) 
      {
      m_iMethod   = V.m_iMethod;
      m_iCmpIndex = V.m_iCmpIndex;
      };
    CSaturationDefn & operator=(const CSaturationDefn & V) 
      {
      m_iMethod   = V.m_iMethod;
      m_iCmpIndex = V.m_iCmpIndex;
      return *this;
      };

    int Method()
      {
      //if (m_iMethod==SMFM_Default)
      //  return MdlMethod;
      return m_iMethod;
      }
    LPCSTR MethodStr();
      //{
      //int i=Method(MdlMethod);

      //return "???";
      //}
    int  CmpIndex() { return m_iCmpIndex; };

  protected:
    int  m_iMethod;
    int  m_iCmpIndex;
  };


class DllImportExport CComponentDataBase
  {
  friend class CSpecieDataBase;
  public:
    CComponentDataBase(flag ForTestingOnly=false);
    ~CComponentDataBase();

    void           Init();
    void           Term();

    flag           LoadComponents(CDBInitRec & CDBInit, char* DataFile, Strng_List & RqdCmp, CRqdSpList& RqdSpc, flag H2OAsAqueous);
    flag           LoadComponentsADO(CDBInitRec & CDBInit, char* DataFile, Strng_List & RqdCmp, CRqdSpList& RqdSpc, flag H2OAsAqueous);
    flag           LoadSolnInfoADO(CDBInitRec & CDBInit, char* DataFile);
    void           AddSpecieD(int c, CComponent & C, CSpecie *pSpD, int p);
    flag           CheckCommonData();
    flag           BuildSpecies(CDBInitRec & CDBInit, Strng_List & RqdCmp, CRqdSpList& RqdSpc);
    flag           LoadBIPs(CDBInitRec & CDBInit, char* FilePath);
    flag           LoadSolnInfo(CDBInitRec & CDBInit, char* FilePath);
    flag           CopyDataForMultiFidelity();
    flag           FixOffsetConstants();
    flag           Initialise(CDBInitRec & CDBInit, char* PrjDBFn, flag UseLibDB, flag H2OAsAqueous, flag DoTest=true);
    flag           Terminate();
    void           DumpTest();

    DDBValueLst  * H2ODDBList();
    void           SelectH2OProperties(long l);
    void           SelectH2OProperties(const char * ClassId);
    long           H2OPropertiesSelected();
    char*          H2OPropertiesSelectedClassId();
    H2OPropertiesBlock & H2OPropertiesSelectedBlk();

    CComponent &    operator[](int c) {return *CmD[c];};
    int            Add(CComponent * SD);
    int            Find(const char * pSpName);
    int            No() { return m_iNo; };

    flag           Bad() { return bCDBBad; };

    int            MixtureId() { return No(); };
    int            ImageId()   { return No()+1; };

    int            PhaseCount(int Occ=BOT_All)  { return m_PhInfoCount[Occ]; };
    int            PhaseFirst(int Occ=BOT_All)  { return m_PhInfoFirst[Occ]; };
    int            PhaseLast(int Occ=BOT_All)   { return m_PhInfoLast[Occ]; };
    CPhaseInfo &   PhaseInfo(int i) { ASSERT(i>=0 && i<m_PhInfoCount[BOT_All]); return m_PhInfo[i]; }
    
    CSaturationDefn DefFlashDefn()     { return CSaturationDefn(m_DefFlashMethod, m_DefFlashCmpIndex); }
    int            DefFlashMethod()    { return m_DefFlashMethod; };
    int            DefFlashCmpIndex()  { return m_DefFlashCmpIndex; };
    int            DefFlashLiqIndex()  { return m_DefFlashLiqIndex; };
    int            DefFlashVapIndex()  { return m_DefFlashVapIndex; };
    CComponent &   DefFlashCmpDesc()   { return *(m_DefFlashCmpIndex >=0 ? CmD[m_DefFlashCmpIndex] : NULL); };
    CSpecie &      DefFlashLiqDesc()   { return m_DefFlashLiqIndex >=0  ? m_SDB[m_DefFlashLiqIndex] : *((CSpecie*)NULL); };
    CSpecie &      DefFlashVapDesc()   { return m_DefFlashVapIndex >=0  ? m_SDB[m_DefFlashVapIndex] : *((CSpecie*)NULL); };

  private:
    CComponent *          CmD[MaxComponents];
    flag                  bCDBBad;

    Strng                 PrjDataFile;

    flag                  m_ForTestingOnly;

    int                   m_iNo;

  public:
    CDMatrix              BIP;

    double                MinT;
    double                MaxT;
    double                MinP;
    double                MaxP;

    flag                  UseStdFns4H2O;
    CSpecie             * m_pWater;
    CSpecie             * m_pSteam;

    CComponentDataBase  & m_CDB;
    CSpecieDataBase       m_SDB;
    CCElementDataBase     m_EDB;
    CH2ODataBase          m_H2O;
    CArray<int, int>      m_Flashable;


    int                   m_ph_SolS;
    int                   m_ph_SolE;
    int                   m_ph_LiqS;
    int                   m_ph_LiqE;
    int                   m_ph_GasS;
    int                   m_ph_GasE;
    int                   m_ph_ALL;
    int                   m_ph_AqsInx;

    PhMask                m_som_Sol;
    PhMask                m_som_Liq;
    PhMask                m_som_Gas;
    PhMask                m_som_SL;
    PhMask                m_som_ALL;

    int                   m_PhInfoCount[BOT_All+1];
    int                   m_PhInfoFirst[BOT_All+1];
    int                   m_PhInfoLast[BOT_All+1];
    CPhaseInfo            m_PhInfo[MaxPhases];

    int                   m_DefFlashMethod;
    int                   m_DefFlashCmpIndex;
    int                   m_DefFlashLiqIndex;
    int                   m_DefFlashVapIndex;


    DDBValueLstMem        m_DDBFlashable;
    CArray<Strng, Strng>  m_FlashCmps;
    CArray<Strng, Strng>  m_FlashCmpsI;

    DDBValueLstMem        m_DDBFlashMethods;
    CArray<Strng, Strng>  m_FlashMethods;
    CArray<Strng, Strng>  m_FlashMethodsI;

  public:
    DDBValueLstMem        DDBCompList;
    DDBValueLstMem        DDBCompListMix;
    DDBValueLstMem        DDBCompListMixImg;
    DDBValueLstMem        DDBCompListVapLiq;
    DDBValueLstMem        DDBCompListDash;
    DDBValueLstMem        DDBCompListDashMix;
    DDBValueLstMem        DDBCompListDashMixImg;
    DDBValueLstMem        DDBCompListDashVapLiq;

  };

// ===========================================================================
//
//
//
// ===========================================================================
// inlines

inline double     CSpDBEqn::PCrit()            { return m_pConst->PCrit();       };
inline double     CSpDBEqn::TCrit()            { return m_pConst->TCrit();       };
inline double     CSpDBEqn::VCrit()            { return m_pConst->VCrit();       };
inline double     CSpDBEqn::ACent()            { return m_pConst->ACent();       };
inline bool       CSpDBEqn::PcOK()             { return m_pConst->PcOK();        };
inline bool       CSpDBEqn::TcOK()             { return m_pConst->TcOK();        };
inline bool       CSpDBEqn::VcOK()             { return m_pConst->VcOK();        };
inline bool       CSpDBEqn::AcOK()             { return m_pConst->AcOK();        };
inline double     CSpDBEqn::MolarVolume()      { return m_pConst->MolarVolume(); };
inline double     CSpDBEqn::MolecularDiam()    { return m_pConst->MolecularDiam(); };

// ===========================================================================

inline double     CSpFidelData::LoT()              { return m_dTs[0]; };
inline double     CSpFidelData::HiT()              { return m_dTe[m_iLastTPoint]; };
inline double     CSpFidelData::TestLoT()          { return m_dTs[0]-m_pSp->m_dTsTol; };
inline double     CSpFidelData::TestHiT()          { return m_dTe[m_iLastTPoint]+m_pSp->m_dTeTol; };

inline int        CSpFidelData::FindTIndex(double T) { int i=0; while (i<m_iLastTPoint && T>m_dTe[i]) i++; return i;};
inline double     CSpFidelData::SpRho(double T, double P) { return m_pRho[FindTIndex(T)]->AtTP(T,P);};
inline double     CSpFidelData::MoleWt()           { return m_pConst->MoleWt(); };

inline bool       CSpFidelData::H25OK()            { return m_pSp->H25OK();             };
inline bool       CSpFidelData::S25OK()            { return m_pSp->S25OK();             };

inline bool       CSpFidelData::CpOK()             { return m_pSp->CpOK();              };
//inline bool       CSpFidelData::VpOK(long Fidelity){ return m_pSp->m_pCommon[long Fidelity]->VpOK();              };

// ===========================================================================

inline double     CSpecie::SpRho(long Fidelity, double T, double P)  { return m_FData[Fidelity].SpRho(T,P);};
inline flag       CSpecie::CpDirect()                                { return !m_bCpInDirect; };
inline flag       CSpecie::CpInDirect()                              { return m_bCpInDirect; };

inline double     CSpecie::LoT(long Fidelity)                        { return m_FData[Fidelity].m_dTs[0]; };
inline double     CSpecie::HiT(long Fidelity)                        { return m_FData[Fidelity].m_dTe[m_FData[Fidelity].m_iLastTPoint]; };
inline double     CSpecie::TestLoT(long Fidelity)                    { return m_FData[Fidelity].m_dTs[0]-m_dTsTol; };
inline double     CSpecie::TestHiT(long Fidelity)                    { return m_FData[Fidelity].m_dTe[m_FData[Fidelity].m_iLastTPoint]+m_dTeTol; };

inline double     CSpecie::CpCv(long Fidelity)                       { return m_FData[Fidelity].m_dCpCv; }
inline double     CSpecie::mlHf0(long Fidelity)                      { return m_FData[Fidelity].m_dHf0; };
inline double     CSpecie::mlHfBase0(long Fidelity)                  { return m_FData[Fidelity].m_dHfBase0; };

inline double     CSpecie::msHs(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)      { return mlHs(Fidelity, T, P, Ovr, M)/m_Const.m_dMoleWt;};
inline double     CSpecie::msHz(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)      { return mlHz(Fidelity, T, P, Ovr, M)/m_Const.m_dMoleWt;};
inline double     CSpecie::msHf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)      { return mlHf(Fidelity, T, P, Ovr, M)/m_Const.m_dMoleWt;};
inline double     CSpecie::msCp(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)      { return mlCp(Fidelity, T, P, Ovr, M)/m_Const.m_dMoleWt;};
inline double     CSpecie::msSf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)      { return mlSf(Fidelity, T, P, Ovr, M)/m_Const.m_dMoleWt;};
inline double     CSpecie::msGf(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)      { return mlGf(Fidelity, T, P, Ovr, M)/m_Const.m_dMoleWt;};
inline double     CSpecie::msHf0(long Fidelity)            { return mlHf0(Fidelity)/m_Const.m_dMoleWt;};
inline double     CSpecie::msHfBase0(long Fidelity)        { return mlHfBase0(Fidelity)/m_Const.m_dMoleWt;};

//inline double     CSpecie::VapourP(long Fidelity, double T)  { return m_FData[Fidelity].m_pVp[m_FData[Fidelity].FindTIndex(T)]->AtT(T); };

inline bool       CSpecie::CpOK()             { return m_FData[0].m_bCpOK; };
inline bool       CSpecie::H25OK()            { return m_FData[0].m_bH25OK; };
inline bool       CSpecie::S25OK()            { return m_FData[0].m_bS25OK; };
//inline bool       CSpecie::VpOK()             { return m_pCommon->m_bVpOK; };

// ===========================================================================

inline char *     CSpecie::Tag()                  { return sTag() ? sTag() : ""; };
inline char *     CSpecie::Sym()                  { return sSym(); };
inline char *     CSpecie::CmpTag()               { return sCmpTag(); };

inline int        CSpecie::FindTIndex(long Fidelity, double T)   { return m_FData[Fidelity].FindTIndex(T);};

inline flag       CSpecie::PhaseIs(PhMask Ph)     { return (m_PhMsk & Ph)!=0; };
inline flag       CSpecie::IsSol()                { return (m_PhMsk & m_CDB.m_som_Sol)!=0; };
inline flag       CSpecie::IsLiq()                { return (m_PhMsk & m_CDB.m_som_Liq)!=0; };
inline flag       CSpecie::IsGas()                { return (m_PhMsk & m_CDB.m_som_Gas)!=0; };
inline flag       CSpecie::IsAqueous()            { return m_bIsAqueous; };
inline int        CSpecie::BaseOccType()          { return m_iBOccT; };
inline int        CSpecie::CId()                  { return iCId; };
inline int        CSpecie::DensCorrCount()        { return m_DensCorrs.GetSize(); };
inline CDensCorr & CSpecie::DensCorr(int i)       { return m_DensCorrs[i]; };
inline CDensCorr & CSpecie::DensCalc()            { return *m_pDensCalc; };
inline int        CSpecie::CpCorrCount()          { return m_CpCorrs.GetSize(); };
inline CCpCorr  & CSpecie::CpCorr(int i)          { return m_CpCorrs[i]; };
inline int        CSpecie::SolubilityCount()      { return m_Solubilities.GetSize(); };
inline CSolubility & CSpecie::Solubility(int i)   { return m_Solubilities[i]; };

inline int        CSpecie::Solvent()              { return m_iSolvent; };

inline char *     CSpecie::SymOrTag()             { return sSym.Length()>0 ? sSym(): sTag();};
inline flag       CSpecie::PrjDB()                { return m_bPrjDB; };

// ===========================================================================

inline CSpecie *  CComponent::operator[](int occ) { return m_SpPhaseD[occ];};

inline CSpecie *  CComponent::FindPhaseData(char * pPhaseName) { int i=FindPhaseDataIndex(pPhaseName); return (i>=0) ? m_PhaseData[i] : NULL; };

inline int        CComponent::SolPhCnt()          { return m_CDB.m_ph_SolE-m_CDB.m_ph_SolS+1; };
inline int        CComponent::LiqPhCnt()          { return m_CDB.m_ph_LiqE-m_CDB.m_ph_LiqS+1; };
inline int        CComponent::VapPhCnt()          { return m_CDB.m_ph_GasE-m_CDB.m_ph_GasS+1; };

inline int        CComponent::SolPhInx(int i)     { return m_SpPhaseD[m_CDB.m_ph_SolS+i] ? m_SpPhaseD[m_CDB.m_ph_SolS+i]->iSId : -1; };
inline int        CComponent::LiqPhInx(int i)     { return m_SpPhaseD[m_CDB.m_ph_LiqS+i] ? m_SpPhaseD[m_CDB.m_ph_LiqS+i]->iSId : -1; };
inline int        CComponent::VapPhInx(int i)     { return m_SpPhaseD[m_CDB.m_ph_GasS+i] ? m_SpPhaseD[m_CDB.m_ph_GasS+i]->iSId : -1; };
inline int        CComponent::CmpInx()            { return iCId; };

//inline int        CComponent::AqsPhInx()            { return m_CDB.m_ph_AqsInx>=0 && m_SpPhaseD[m_CDB.m_ph_AqsInx] ? m_SpPhaseD[m_CDB.m_ph_AqsInx]->iSId : -1; };

inline CSpecie *  CComponent::pSpecie(int occ)    { return m_PhaseData[occ];};
inline int        CComponent::iSpecie(int occ)    { return m_iPhaseData[occ];};
inline int        CComponent::NSpecies()          { return m_nPhaseData;};

// ===========================================================================

extern DllImportExport bool gs_WithSpecieCnvs;

extern DllImportExport CComponentDataBase &gs_CDB; // the real database

extern DllImportExport CComponentDataBase &CDB;
extern DllImportExport CSpecieDataBase &SDB;
extern DllImportExport CCElementDataBase &EDB;
extern DllImportExport CH2ODataBase &H2ODB;
extern DllImportExport LPCTSTR OccurenceNames[];
extern DllImportExport int &ph_SolS;
extern DllImportExport int &ph_SolE;
extern DllImportExport int &ph_LiqS;
extern DllImportExport int &ph_LiqE;
//extern DllImportExport int &ph_AqsS;
//extern DllImportExport int &ph_AqsE;
extern DllImportExport int &ph_GasS;
extern DllImportExport int &ph_GasE;
extern DllImportExport int &ph_ALL;

extern DllImportExport PhMask &som_Sol;
extern DllImportExport PhMask &som_Liq;
extern DllImportExport PhMask &som_Gas;
extern DllImportExport PhMask &som_SL;
extern DllImportExport PhMask &som_ALL;

// ===========================================================================
//
//
//
// ===========================================================================
// ===========================================================================

class DllImportExport CSpecieBlk// : public CTagObjSelectFn
  {
  friend class CSpecieDataBase;
  friend class CComponentDataBase;
  friend class MSpeciePtr;
  friend class MAqSpeciePtr;
  public:

    int             operator()()  { return iSId; };
    int             sid()         { return iSId; };
    CSpecie &       D()           { return SDB[iSId]; };
    int             ci()          { return iCId; };
    CComponent &    cd()          { return CDB[iCId]; };
    int     &       alti()        { return iAltId; };

    double          MW;

    CSpecieBlk(MInitialiseTest & InitTest, LPCTSTR pName, LPCTSTR pFile, LPCTSTR pDll);
    ~CSpecieBlk();

    static CSpecieBlk * First()   { return sm_pFirst; };
    CSpecieBlk *    Next()        { return m_pNxt; };

    bool            Exists()      { return iSId>=0; };

    //// CTagObjSelectFn overide
    //virtual bool    Selectable()  { return Exists(); }
    //static void     AttachSelectableFns();
    //static void     DetachSelectableFns();
    static void     TestInitialiseOK();

  public:
    Strng           m_sCmpName;
    Strng           m_sSpName;
    Strng           m_sSpPostfix;
    Strng           m_sFile;
    Strng           m_sDll;
    flag            m_fOpt;
    flag            m_fAllowAq;

  protected:
    static CSpecieBlk *sm_pFirst;
    CSpecieBlk    * m_pNxt;

    MInitialiseTest & m_InitTest;

    int             iCId;
    int             iSId;
    int             iAltId;

  };

// ===========================================================================

#define SPECIEBLK_S(InitTest, Sp, Name, Opt) \
  class Sp##Blk : public CSpecieBlk \
    { \
    public: \
      Sp##Blk(MInitialiseTest & IT, pchar pName, pchar pFile, pchar pDll) : \
        CSpecieBlk(IT, pName, pFile, pDll) {/*eOcc=BOT_Solid*/; m_fOpt=Opt; }; \
      int            SolPhInx() { return iSId; }; \
      CSpecie &      SolPhDef() { return SDB[iSId]; }; \
    }; \
static Sp##Blk Sp(InitTest, Name, __FILE__, MDLLIBNAME);

#define SPECIEBLK_L(InitTest, Sp, Name, Opt) \
  class Sp##Blk : public CSpecieBlk \
    { \
    public: \
      Sp##Blk(MInitialiseTest & IT, pchar pName, pchar pFile, pchar pDll) : \
        CSpecieBlk(IT, pName, pFile, pDll) {/*eOcc=BOT_Liquid*/; m_fOpt=Opt; }; \
      int            LiqPhInx() { return iSId; }; \
      CSpecie &      LiqPhDef() { return SDB[iSId]; }; \
    }; \
static Sp##Blk Sp(InitTest, Name, __FILE__, MDLLIBNAME);

#define SPECIEBLK_A(InitTest, Sp, Name, Opt) \
  class Sp##Blk : public CSpecieBlk \
    { \
    public: \
      Sp##Blk(MInitialiseTest & IT, pchar pName, pchar pFile, pchar pDll) : \
        CSpecieBlk(IT, pName, pFile, pDll) {/*eOcc=BOT_Liquid*/; m_fOpt=Opt; m_fAllowAq=true; }; \
      int            LiqPhInx() { return iSId; }; \
      CSpecie &      LiqPhDef() { return SDB[iSId]; }; \
    }; \
static Sp##Blk Sp(InitTest, Name, __FILE__, MDLLIBNAME);

#define SPECIEBLK_V(InitTest, Sp, Name, Opt) \
  class Sp##Blk : public CSpecieBlk \
    { \
    public: \
      Sp##Blk(MInitialiseTest & IT, pchar pName, pchar pFile, pchar pDll) : \
        CSpecieBlk(IT, pName, pFile, pDll) {/*eOcc=BOT_Gas*/; m_fOpt=Opt; }; \
      int            VapPhInx() { return iSId; }; \
      CSpecie &      VapPhDef() { return SDB[iSId]; }; \
    }; \
static Sp##Blk Sp(InitTest, Name, __FILE__, MDLLIBNAME);

// ===========================================================================
//
//
//
// ===========================================================================

// SysVec Operators for values
const byte SVOP_     = 0;
const byte SVOP_Mass = 1;
const byte SVOP_Mole = 2;
const byte SVOP_Vol  = 3;
const byte SVOP_NVol = 4;

inline eScdSVViewBasis operator++( eScdSVViewBasis &b, int ) { return b = (eScdSVViewBasis )(b + 1); }

extern DllImportExport const LPCTSTR SV_ViewStr[];
extern DllImportExport const LPCTSTR SV_ViewBasisStr[];


XID xidSpFirst          = SpcXID(110000)    + SVV_NULL             ;
XID xidSpRawFrac        = SpcXID(110000)    + SVV_AsRawFrac        ;
XID xidSpRawMass        = SpcXID(110000)    + SVV_AsRawMass        ;
XID xidSpRawMassFlow    = SpcXID(110000)    + SVV_AsRawMassFlow    ;
XID xidSpMass           = SpcXID(110000)    + SVV_AsMass           ;
XID xidSpMassFlow       = SpcXID(110000)    + SVV_AsMassFlow       ;
XID xidSpMassFrac       = SpcXID(110000)    + SVV_AsMassFrac       ;
XID xidSpMole           = SpcXID(110000)    + SVV_AsMole           ;
XID xidSpMoleFlow       = SpcXID(110000)    + SVV_AsMoleFlow       ;
XID xidSpMoleFrac       = SpcXID(110000)    + SVV_AsMoleFrac       ;
XID xidSpVol            = SpcXID(110000)    + SVV_AsVol            ;
XID xidSpVolFlow        = SpcXID(110000)    + SVV_AsVolFlow        ;
XID xidSpVolFrac        = SpcXID(110000)    + SVV_AsVolFrac        ;
XID xidSpPartialP       = SpcXID(110000)    + SVV_AsPartialP       ;
XID xidSpPartialPFrac   = SpcXID(110000)    + SVV_AsPartialPFrac   ;
XID xidSpNVol           = SpcXID(110000)    + SVV_AsNVol           ;
XID xidSpNVolFlow       = SpcXID(110000)    + SVV_AsNVolFlow       ;
XID xidSpNVolFrac       = SpcXID(110000)    + SVV_AsNVolFrac       ;
XID xidSpConc           = SpcXID(110000)    + SVV_AsSpcConc        ;
XID xidSpElem           = SpcXID(110000)    + SVV_AsElem           ;
XID xidSpElemFlow       = SpcXID(110000)    + SVV_AsElemFlow       ;
XID xidSpElemFrac       = SpcXID(110000)    + SVV_AsElemFrac       ;
XID xidSpElemConc       = SpcXID(110000)    + SVV_AsElemConc       ;
XID xidSpElemConc25     = SpcXID(110000)    + SVV_AsElemConc25     ;
XID xidSpComp           = SpcXID(110000)    + SVV_AsComp           ;
XID xidSpCompFlow       = SpcXID(110000)    + SVV_AsCompFlow       ;
XID xidSpCompFrac       = SpcXID(110000)    + SVV_AsCompFrac       ;
XID xidSpCompConc       = SpcXID(110000)    + SVV_AsCompConc       ;
XID xidSpCompConc25     = SpcXID(110000)    + SVV_AsCompConc25     ;
XID xidCalcViews        = xidSpCompConc25   + 1;                         // these are calculations which are dependent on the View
XID xidViewCount        = xidCalcViews-xidSpFirst;
XID xidCalcSimple       = xidCalcViews      + xidViewCount; // This is not
XID xidSpInUse          = xidCalcSimple     + MaxCalculations;
XID xidAttrViews        = xidSpInUse        + 1;
XID xidAttrSimple       = xidAttrViews      + xidViewCount;
XID xidLastAttribute    = xidAttrSimple     + MaxAttributes;

// ===========================================================================
// Variable Available Masks (general)
const long VAMsk_Overides     = 0x00000001;
const long VAMsk_Thermo       = 0x00000002;
const long VAMsk_Viscosity    = 0x00000004;
const long VAMsk_HeatValues   = 0x00000008;
const long VAMsk_Acidity      = 0x00000010;
const long VAMsk_Inited       = 0x80000000;

// Variable Available Masks (specie vector)
const long VAMsk_SpRaw        = 0x00000001;
const long VAMsk_SpMass       = 0x00000002;      
const long VAMsk_SpMole       = 0x00000004;      
const long VAMsk_SpVol        = 0x00000008;       
const long VAMsk_SpPartialP   = 0x00000010;  
const long VAMsk_SpNVol       = 0x00000020;      
const long VAMsk_SpConc       = 0x00000040;      
const long VAMsk_SpElem       = 0x00000080;      
const long VAMsk_SpComp       = 0x00000100;      
const long VAMsk_SpInited     = 0x80000000;

extern DllImportExport byte SVOperatorsXID(XID xid);

struct CSV_ViewCnv
  {
  CCnvIndex     * m_pInx;
  CString         m_Txt;
  };
struct CSV_ViewBasis
{
  eScdSVViewBasis    m_Basis;
  char          * m_Txt;
  };
struct CSV_ViewInfo
  {
  char          * FullName;
  char          * Nm;
  long            xid;
  CSV_ViewCnv  ** Cnvs;
  bool            m_IsFrac;
  CSV_ViewBasis * m_Basis;
  long            m_VAMsk;
  };

extern DllImportExport const CSV_ViewInfo gs_ViewInfo[];

// ===========================================================================
//
//
//
// ===========================================================================

enum  CSysVecItemType { SVT_Null, SVT_Specie, SVT_Calculation, SVT_Attribute, SVT_Annotation};
enum  CSysVecItemAnno { SVA_Null, SVA_Text, SVA_Marker, SVA_OptPage, SVA_RqdPage};

//--------------------------------------------------------------------------

class CSysVecFn; // forward

enum SV_CalcTypes {SVCT_Null, SVCT_General, SVCT_Sum, SVCT_Total, SVCT_PhSum, SVCT_PhTotal, SVCT_IPhSum, SVCT_IPhTotal };
enum SV_AttrTypes {SVAT_Null, SVAT_Qual, SVAT_Amnt, SVAT_Calc};
extern DllImportExport const LPCTSTR SV_CalcTypesStr[];
extern DllImportExport const LPCTSTR SV_AttrTypesStr[];
  
class DllImportExport CSysVecItem
  {
  friend class CSysVecInfo;
  friend class CSysVector;
  friend class CFlwSolver;
  friend class CSysVecFn;
  friend class SpModel;
  public:
    CSysVecItem(CSysVecItemType Type);
    virtual ~CSysVecItem();

    long                CfgIndex()                      { return m_iCfgIndex; }
    long                SVIndex()                       { return m_iSVIndex; }
    long                TypeIndex()                     { return m_iTypeIndex; }
    LPSTR               ParentTag()                     { return m_sParent(); };
    long                ParentCfgIndex()                { return m_iCfgParent; };
    CSysVecItem       * ParentSVItem()                  { return m_pSVParent; };
    LPSTR               Tag()                           { return m_sTag(); };
    LPSTR               Sym()                           { return m_sSym(); };
    LPSTR               FullTag()                       { return m_sFullTag(); };
    LPSTR               FullSym()                       { return m_sFullSym(); };
    LPSTR               SymOrTag()                      { return m_sFullSym.Length()>0 ? m_sFullSym(): m_sFullTag();};
    long                Level()                         { return m_iLevel; };
    CSysVecItemType     Type()                          { return m_eType; }
    int                 CnvIndex()                      { return m_iCnv; }
    LPSTR               CnvString()                     { return m_sCnv(); }
    CSysVecItemAnno     Annotation()                    { return m_iAnnotation; };
    long                PhaseCount()                    { return m_iPhCnt; };
    PhMask              PhaseMask()                     { return m_iPhMsk; };
    PhMask              OccMask()                       { return m_iOccMsk; };
    long                PhaseInx()                      { return m_iPhInx; };
    long                OccInx()                        { return m_iOccInx; };
    bool                PhaseIs(PhMask PhM)             { return (PhaseMask() & PhM)!=0; };
    bool                OccIs(PhMask PhM)               { return (OccMask() & PhM)!=0; };

    CSV_ViewCnv       * ViewBasisCnv(eScdSVView w, eScdSVViewBasis Ph);
    PhMask              ViewBasisMask(eScdSVViewBasis Ph);
    LPCSTR              ViewBasisName(eScdSVViewBasis Ph);
    long                ViewBasisEditable(eScdSVView Vw, eScdSVViewBasis Ph);
    long                ViewBasisUsed(eScdSVViewBasis Ph);

    LPCSTR              FnString()                      { return m_sFormula(); };
    bool                FnIsSpcsSum()                   { return m_bIsSpcsSum; };
    CIArray           & FnIndicesOfSumSpcs()            { return m_IndicesOfSumSpcs; };;

    LPSTR               SDBTag()                        { return (LPSTR)m_sSDBTag(); };
    long                SDBIndex()                      { return m_iSDBIndex; };

    long                ChildCount()                    { return m_ChildrenSVIndex.GetSize(); };
    long                ChildSVIndex(int i)             { return m_ChildrenSVIndex[i]; };

    SV_CalcTypes        CalcType()                      { return m_iCalcType; };
    SV_AttrTypes        AttrType()                      { return m_iAttrType; };

    //bool                IsTotalCalc()                   { return m_bIsTotalCalc; };
    bool                InitHidden()                    { return m_bInitHidden; };

    const static dword DuplicateOK=1;

  protected:
    bool                ParseBase(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options=0);
    bool                ParseCnv(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options=0);

  protected:
    CSysVecItemType     m_eType;
    Strng               m_sParent;
    Strng               m_sTag;
    Strng               m_sSym;
    Strng               m_sFullTag;
    Strng               m_sFullSym;
    bool                m_bTagSpecd;
    bool                m_bSymSpecd;
    long                m_iLevel;
    long                m_iPhCnt;
    PhMask              m_iPhMsk;
    PhMask              m_iOccMsk;
    long                m_iPhInx;
    long                m_iOccInx;

    long                m_iCfgParent;
    long                m_iSVParent;
    CSysVecItem       * m_pSVParent;
    long                m_iCfgIndex;
    long                m_iSVIndex;
    long                m_iTypeIndex;
    int                 m_iCnv;
    Strng               m_sCnv;

    dword               m_dwOptions;

    CSysVecItemAnno     m_iAnnotation;
    CSVLongArray        m_ChildrenCfgIndex;
    CSVLongArray        m_ChildrenSVIndex;

    Strng               m_sSDBTag;
    long                m_iSDBIndex;

    CSysVecFn         & m_Fn;
    CSysVecFn         & m_Add;
    CSysVecFn         & m_Sub;
    CSysVecFn         & m_Scl;
    CSysVecFn         & m_AddDeriv;
    SV_AttrTypes        m_iAttrType;

    Strng             & m_sFormula;
    bool              & m_bIsSpcsSum;
    CIArray           & m_IndicesOfSumSpcs;
    SV_CalcTypes        m_iCalcType;

    bool                m_bInitHidden;

    double              m_dMinValue;
    double              m_dMaxValue;

    struct CSpViewBasisRule
      {
      long              m_iSpVwMaskIndex;
      bool              m_bEditable;
      bool              m_bUsed;
      CSpViewBasisRule()
        {
        m_iSpVwMaskIndex= -1;
        m_bEditable     = false;
        m_bUsed         = true;
        };
      }                 m_ViewBasisRules[SVVB_End];

  };

//--------------------------------------------------------------------------

class DllImportExport CSysVecTemplateItem
  {
  public:
    CString                 m_Name;
    bool                    m_SaveIt;
    CArray<double, double>  m_Values;

    CSysVecTemplateItem(LPCTSTR Name=NULL);
    CSysVecTemplateItem(const CSysVecTemplateItem & O);
    CSysVecTemplateItem & operator=(const CSysVecTemplateItem & O);
    CSysVecTemplateItem & operator=(CSysVector & O);
    bool operator==(const CSysVector & O);
  };

class DllImportExport CSysVecTemplateList : public CList<CSysVecTemplateItem*, CSysVecTemplateItem*> 
  {
  public:
    CSysVecTemplateItem * FindTemplate(LPCTSTR Name, bool AddIt=false);
  };

//--------------------------------------------------------------------------

class DllImportExport CSysVecItemArray : public CArray<CSysVecItem*, CSysVecItem*> {};

class DllImportExport CSysVecInfo
  {
  public:
    CSysVecInfo();
    ~CSysVecInfo();

    CSysVecItem     * ParseSpecie(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options=0, int InsertIndex=100000);
    CSysVecItem     * ParseCalculation(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options=0, int InsertIndex=100000);
    CSysVecItem     * ParseAttribute(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options=0, int InsertIndex=100000);
    CSysVecItem     * ParseAnnotation(int nToks, CSVColArray & C, CDBInitRec &CDBI, CSysVecItemAnno What, dword Options=0, int InsertIndex=100000);

    CSysVecItem     * ParseSpecie(LPSTR Tag, LPSTR Sym, LPSTR LoTol, LPSTR HiTol, LPSTR UseIdeal, CDBInitRec &CDBI, dword Options=0, int InsertIndex=100000);
    CSysVecItem     * ParseCalculation(LPSTR Tag, LPSTR Sym, LPSTR CalcStr, CDBInitRec &CDBI, dword Options=0, int InsertIndex=100000);
    CSysVecItem     * ParseAnnotation(LPSTR What, LPSTR Tag, CDBInitRec &CDBI, dword Options=0, int InsertIndex=100000);

    bool              ParseSpecieTemplateNames2(int nToks, CSVColArray & C, CDBInitRec &CDBI);
    bool              ParseSpecieTemplateValues2(int nToks, CSVColArray & C, CDBInitRec &CDBI);
    void              FinaliseSpecieTemplateValues();

    bool              Init();
    bool              Finalise(CDBInitRec &CDBI);
    bool              Term();

    CSysVecItemType   Type(int i)   { return m_Cfg[i]->m_eType; }

    CSysVecItem     * Cfg(int i)  { return m_Cfg[i]; };
    CSysVecItem     * Img(int i)  { return m_Img[i]/*[i+m_nSVClcCount]*/; };

    CSysVecItem     * Spc(int i)  { return m_Spc[i]; };
    CSysVecItem     * Att(int i)  { return m_Att[i]; };
    CSysVecItem     * Clc(int i)  { return m_Clc[i]; };

    int               GetShowGroups()      { return m_ShowGroups; };
    void              SetShowGroups(int On){ m_ShowGroups=On!=0; };

    int *             SetUpSkipList(PhMask Mask);
    int *             SetUpSkipListRev(PhMask Mask);

    static PhMask     SpVwPhMask(int VwPhMaskIndex) { return sm_SpVwPhMask[VwPhMaskIndex].m_iMsk; };
    static LPCTSTR    SpVwPhMaskName(int VwPhMaskIndex) { return sm_SpVwPhMask[VwPhMaskIndex].m_Sym; };

  public: //protected:
    long m_nSVSpcCount;
    long m_nSVAttCount;
    long m_nSVClcCount;
    long m_nSVTxtCount;

    long m_nSVValueCount;

  //protected:
    CSysVecItem**     m_Img;
    CSysVecItemArray  m_Cfg;
    CSysVecItemArray  m_Spc;
    CSysVecItemArray  m_Att;
    CSysVecItemArray  m_Clc;

    CSysVecItemArray  m_ImgArray;

    CSVBoolArray      m_HasCalcsArray;
    CSVLongArray      m_ChildCountArray;
    bool            * m_HasCalcs; // for speed
    long            * m_ChildCount;

    CSVLongArray      m_CalcsWithChildren;

    CSysVecTemplateList m_Templates;

    bool              m_ShowGroups;

    int               * m_SkipListFwdAll, *m_SkipListRevAll;
    int               * m_SkipListFwdSL,  *m_SkipListRevSL;
    int               * m_SkipListFwdSol, *m_SkipListRevSol;
    int               * m_SkipListFwdLiq, *m_SkipListRevLiq;
    int               * m_SkipListFwdGas, *m_SkipListRevGas;
    CMap<PhMask, PhMask, int*, int*> m_SkipListFwdMap, m_SkipListRevMap;

    static struct CSpViewMask
      {
      PhMask            m_iMsk;
      CString           m_Sym;
      CString           m_Tag;
      }                 sm_SpVwPhMask[SpVwPhMaskIndex_Count];

  };

// ---------------------------------------------------------------------------

extern DllImportExport CSysVecInfo SVI;

// ---------------------------------------------------------------------------

inline long           SVSpcCount()    { return SVI.m_nSVSpcCount; }
inline long           SVAttCount()    { return SVI.m_nSVAttCount; }
inline long           SVClcCount()    { return SVI.m_nSVClcCount; }
inline long           SVTxtCount()    { return SVI.m_nSVTxtCount; }
inline long           SVCfgCount()    { return SVI.m_Cfg.GetCount(); }
inline long           SVValueCount()  { return SVI.m_nSVValueCount; }
inline CSysVecItem &  SVImg(int i)    { return *SVI.m_Img[i]; }


inline long           SVSpc1()        { return 0; }
inline long           SVSpcN()        { return SVI.m_nSVSpcCount; }
inline long           SVAtt1()        { return SVI.m_nSVSpcCount; }
inline long           SVAttN()        { return SVI.m_nSVValueCount; }
inline long           SVClc1()        { return -SVI.m_nSVClcCount; }
inline long           SVClcN()        { return -1; }


// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport CSysVector
  {
  //friend class SpModel;
  //friend class SpConduit;
  //friend class SpContainer;
  //friend class SpImage;
  protected:
    CSysVector() {}; // protected - prevent from being instantiated

#if WITHSPVECTORTIMERS
  public:
    static CGlblStopWatch sm_swZero;
    static CGlblStopWatch sm_swSet;
    static CGlblStopWatch sm_swSum;
    static CGlblStopWatch sm_swAdd;
    static CGlblStopWatch sm_swMass;
    static CGlblStopWatch sm_swNorm;
    static CGlblStopWatch sm_swDens;
    static CGlblStopWatch sm_swHeat;
    static CGlblStopWatch sm_swOther;
#endif

  //protected: // prevent this being used
  //  double & operator       [](int i)                           { ASSERT(i>=0 && i<SVValueCount()); return m_d[i]; };

  public: // the get may be used
    double operator         [](int SVIndex)                         { return SVIndex>=0 ? m_d[SVIndex]:EvalFn(SVIndex); };
    //double &operator        [](int SVIndex, CSysVector &Vec)        { ASSERT(SVIndex>=0 && SVIndex<SVValueCount()); return m_d[SVIndex]; };

  public:
    double      getVValue(int SVIndex)                              { return SVIndex>=0 ? m_d[SVIndex]:EvalFn(SVIndex); };
    double      getVMass(int SVIndex)                               { ASSERT(SVIndex>=0 && SVIndex<SVSpcCount()); return m_d[SVIndex]; };
    double      getVMoles(int SVIndex)                              { ASSERT(SVIndex>=0 && SVIndex<SVSpcCount()); return m_d[SVIndex]/SDB[SVIndex].MoleWt(); };
    double      getVAttr(int SVIndex)                               { ASSERT(SVIndex>=0 && SVIndex<SVAttCount()); return m_d[SVIndex+SVSpcCount()]; };
    double      getVCalc(int SVIndex)                               { ASSERT(SVIndex>=0 && SVIndex<SVClcCount()); return EvalFn(SVIndex-SVClcCount()); };
    double    * getVPtr(int SVIndex)                                { ASSERT(SVIndex>=0 && SVIndex<SVValueCount()); return &m_d[SVIndex]; }

    void        SetVValue(int SVIndex, double Val)                  { if (SVIndex>=0) m_d[SVIndex]=Val; };
    void        SetVMass(int SVIndex, CSysVector &Vec, double Val);
    void        AddVMass(int SVIndex, CSysVector &Vec, double Val);
    void        SclVMass(int SVIndex, double Scl);
    void        ClrVMass(int SVIndex)                               { SclVMass(SVIndex, 0); };
    void        SetVAttr(int SVIndex, double Val)                   { ASSERT(SVIndex>=0 && SVIndex<SVAttCount()); m_d[SVIndex+SVSpcCount()]=Val; };
    void        SetSpcScalar(double Val)                            { for (int i=0; i<SVSpcCount(); i++) m_d[i]=Val; };

    __declspec(property(get=getVValue,put=SetVValue))           double VValue[];
    __declspec(property(get=getVMass))                          double VMass[];
    __declspec(property(get=getVMoles))                         double VMoles[];
    __declspec(property(get=getVAttr,put=SetVAttr))             double VAttr[];
    __declspec(property(get=getVCalc))                          double VCalc[];
    __declspec(property(get=getVPtr))                           double *VPtr[];

    double      ValueOp(int i, byte Op, long Fidelity, double T, double P);
    void        SetValueOP(int i, byte Op, double V, long Fidelity, double T, double P);
    static double Mass2OtherFactor(int i, byte Op, long Fidelity, double T, double P, CSVData M);

    CSVData     SVData()                                            { return &m_d[0]; }

#ifdef _DEBUG
    virtual  void CheckIt() {};
#endif

  protected:
    double         EvalFn(int SVIndex);
    double         EvalFn(int SVIndex, byte Op, long Fidelity, double T, double P);
    //void           SetOneScl(int SVIndex, CSysVector &Data2, double Scale);
    //void           SetOneVal(int SVIndex, CSysVector &Data2, double Value);
    //void           AddOneScl(int SVIndex, CSysVector &Data2, double Scale);
    //void           AddOneVal(int SVIndex, CSysVector &Data2, double Value);
    //void           SubOne(int SVIndex, CSysVector &Data2, double Scale);
    //void           DoAddDeriv(int SVIndex, int Sgn, double dTime, CSysVector &Flw, CSysVector &M, double Scale);

  public:

    CSysVector &   operator=(CSysVector &Vec2);   // assignment
    CSysVector &   operator+=(CSysVector &Vec2);
    CSysVector &   operator-=(CSysVector &Vec2);


    CSysVector &   Set(CSysVector &Vec2);
    CSysVector &   Set(CSysVector &Vec2, PhMask Phase);
    CSysVector &   Set(CSysVector &Vec2, PhMask Phase, double Scl, double LoLimit=0.0);
    CSysVector &   Set(CSysVector &Vec2, PhMask Phase, double Scl, CSVData SclA);
    CSysVector &   Set(CSysVector &Vec2, double SScl, double LScl, double VScl);

    CSysVector &   Add(CSysVector &Vec2, PhMask Phase);
    CSysVector &   Add(CSysVector &Vec2, PhMask Phase, double Scl);
    CSysVector &   Add(CSysVector &Vec2, double SScl, double LScl, double VScl);

    CSysVector &   AddDeriv(int Sgn, double dTime, CSysVector &Flw, CSysVector &M, double Advance);

    flag        AddValid(CSysVector &Vec2);

    void        Mult(PhMask Phase, double Scl);
    void        Mult(CIArray & SpIds, double Scl);
    void        Mult(CSysVector & Scale, double Scl);

    void        Zero();
    void        ZeroPhase(PhMask Phase=som_ALL);
    //void        CopyPhase(SpVector &MA, PhMask Phase=som_ALL);
    double      Normalise();
    double      Normalise(CSysVector &MA);
    void        SetTo(double Value);
    void        SetToOnesAndZeros(flag Invert=false, double Tol=ZeroLimit);
    void        SetToValue(PhMask Phase, double Value=0.0);
    void        SetToValue(int iSpId, double Value=0.0);
    double      Sum(PhMask Phase=som_ALL);
    void        ToMass(CSysVector &MA);
    void        ToMoles(CSysVector &MA);
    double      Mass();
    double      Mass(PhMask Phase);
    double      Mass(CIArray & SpIds);
    double      Mass(CSysVector & Scale);
    double      Moles();
    double      Moles(PhMask Phase);
    double      Moles(CIArray & SpIds);
    double      Moles(CSysVector & Scale);
    double      Volume(long Fidelity, PhMask Phase, double Temp, double Press, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swDens); return SDB.Volume(Fidelity, Phase, Temp, Press, Ovr, m_d); };
    double      Volume(long Fidelity, CIArray & SpIds, double Temp, double Press, SpPropOveride *Ovr)     { SPMVECTORTIMERLAP(sm_swDens); return SDB.Volume(Fidelity, SpIds, Temp, Press, Ovr, m_d); };
    double      Volume(long Fidelity, CSysVector & Scale, double Temp, double Press, SpPropOveride *Ovr)  { SPMVECTORTIMERLAP(sm_swDens); return SDB.Volume(Fidelity, Scale, Temp, Press, Ovr, m_d); };
    double      NVolume(long Fidelity, PhMask Phase, SpPropOveride *Ovr)                                  { SPMVECTORTIMERLAP(sm_swDens); return SDB.NVolume(Fidelity, Phase, Ovr, m_d); };
    double      NVolume(long Fidelity, CIArray & SpIds, SpPropOveride *Ovr)                               { SPMVECTORTIMERLAP(sm_swDens); return SDB.NVolume(Fidelity, SpIds, Ovr, m_d); };
    double      NVolume(long Fidelity, CSysVector & Scale, SpPropOveride *Ovr)                            { SPMVECTORTIMERLAP(sm_swDens); return SDB.NVolume(Fidelity, Scale, Ovr, m_d); };

    double      ElementMass(CIArray & ElementIds, PhMask Phase);

    double      MassFrac(PhMask Phase);
    double      MoleFrac(PhMask Phase);

    double      VolFrac(long Fidelity, PhMask Phase, double Temp, double Press, SpPropOveride *Ovr)       { SPMVECTORTIMERLAP(sm_swDens); return SDB.VolFrac(Fidelity, Phase, Temp, Press, Ovr, m_d); };
    double      Rho(long Fidelity, PhMask Phase, double Temp, double Press, SpPropOveride *Ovr)           { SPMVECTORTIMERLAP(sm_swDens); return SDB.Density(Fidelity, Phase, Temp, Press, Ovr, &m_d[0]); };
    double      Rho(long Fidelity, CSysVector & Scale, double Temp, double Press, SpPropOveride *Ovr)     { SPMVECTORTIMERLAP(sm_swDens); return SDB.Density(Fidelity, Scale, Temp, Press, Ovr, &m_d[0]); };
    double      NRho(long Fidelity, PhMask Phase, SpPropOveride *Ovr)                                     { SPMVECTORTIMERLAP(sm_swDens); return SDB.NDensity(Fidelity, Phase, Ovr, &m_d[0]); };
    double      NRho(long Fidelity, CSysVector & Scale, SpPropOveride *Ovr)                               { SPMVECTORTIMERLAP(sm_swDens); return SDB.NDensity(Fidelity, Scale, Ovr, &m_d[0]); };

    double      CpCv(long Fidelity, PhMask Phase)                                                   { SPMVECTORTIMERLAP(sm_swHeat); return SDB.CpCv(Fidelity, Phase, &m_d[0]); };
    double      msCp(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)         { SPMVECTORTIMERLAP(sm_swHeat); return SDB.msCp(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      msHs(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)         { SPMVECTORTIMERLAP(sm_swHeat); return SDB.msHs(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      msHz(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)         { SPMVECTORTIMERLAP(sm_swHeat); return SDB.msHz(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      msHf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)         { SPMVECTORTIMERLAP(sm_swHeat); return SDB.msHf(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      msSf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)         { SPMVECTORTIMERLAP(sm_swHeat); return SDB.msSf(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      msGf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)         { SPMVECTORTIMERLAP(sm_swHeat); return SDB.msGf(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      totCp(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swHeat); return SDB.totCp(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      totHs(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swHeat); return SDB.totHs(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      totHz(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swHeat); return SDB.totHz(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      totHf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swHeat); return SDB.totHf(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      totSf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swHeat); return SDB.totSf(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };
    double      totGf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr)        { SPMVECTORTIMERLAP(sm_swHeat); return SDB.totGf(Fidelity, Phase, T_, P_, Ovr, &m_d[0]); };

  protected:
    union
      {
      double *m_d;
      double (*m_dbg)[DEBUGARRAYLEN];
      };

  };

class DllImportExport SpMArray : public CSysVector
  {
  public:
    SpMArray(bool DoInit=true)                          { Setup(); if (DoInit) Zero(); };
    //SpMArray(double dInit)                              { Setup(); *this=dInit; };
    SpMArray(CSysVector &Vec2)                             { Setup(); Set(Vec2, som_ALL); };
    SpMArray(CSysVector &Vec2, PhMask Phase)               { Setup(); Set(Vec2, Phase); };
    SpMArray(CSysVector &Vec2, PhMask Phase, double Scl)   { Setup(); Set(Vec2, Phase, Scl); };
    void Setup()                                        { m_d=&m_M[0]; };

    SpMArray&  operator=(CSysVector &Vec2);
    SpMArray&  operator=(SpMArray &Vec2);
    SpMArray&  operator=(SpVector &Vec2);
    //SpMArray&  operator=(double m);

    operator CSVData()                               { return m_d; }

#ifdef _DEBUG
    virtual void CheckIt() { if (m_d!=&m_M[0]) DoBreak(); };
#endif

  protected:
    union
      {
      double         m_M[MaxSpecies+MaxAttributes];       // Data
      double         m_dbg[DEBUGARRAYLEN ];              // For debugging - reduced visibility on size of array
      };
  };

class DllImportExport SpVector : public CSysVector
  {
  public:
    SpVector(bool DoInit=true)                            { Setup(); if (DoInit) Zero(); };
    //SpVector(double dInit)                                { Setup(); *this=dInit; };
    SpVector(CSysVector &Vec2)                            { Setup(); Set(Vec2, som_ALL); };
    SpVector(CSysVector &Vec2, PhMask Phase)              { Setup(); Set(Vec2, Phase); };
    SpVector(CSysVector &Vec2, PhMask Phase, double Scl)  { Setup(); Set(Vec2, Phase, Scl); };
    void Setup()
      {
      long L=SVValueCount();
      if (L==0)
        L=MaxSpecies+MaxAttributes;
      m_M.SetSize(L);
      m_d=&m_M[0];
      };

    SpVector & operator=(CSysVector &Vec2)                { Set(Vec2); return *this; };
    SpVector & operator=(SpMArray &S)                     { Set(S); return *this; };
    SpVector & operator=(SpVector &S)                     { Set(S); return *this; };
    //SpVector & operator=(double m)                        { *(CSysVector*)this=m; return *this; };

    operator CSysVector&()                                { return *this; };
    operator CDVector &()                                 { return m_M; };

#ifdef _DEBUG
    virtual void CheckIt()                                 { if (m_d!=&m_M[0]) DoBreak(); };
#endif

  protected:
    CDVector m_M; // Data
  };

inline SpMArray&  SpMArray::operator=(CSysVector &S)        { Set(S); return *this; };
inline SpMArray&  SpMArray::operator=(SpMArray &S)          { Set(S); return *this; };
inline SpMArray&  SpMArray::operator=(SpVector &S)          { Set(S); return *this; };
//inline SpMArray&  SpMArray::operator=(double m)             { *(CSysVector*)this=m; return *this; };

// ===========================================================================
//
//
//
// ===========================================================================

class DllImportExport SpecieIter
  {
  private:
    int*           m_pSkipList;
    int*           m_pSkipListRev;
    int            m_SpcCount;
    int            m_ValCount;
    flag           m_fMustDelete;
    flag           m_fMustDeleteRev;
    PhMask         m_PhMsk;

  public:
    SpecieIter()
      {
      m_pSkipList=NULL;
      m_pSkipListRev=NULL;
      m_fMustDelete=false;
      m_fMustDeleteRev=false;
      m_SpcCount=0;
      m_ValCount=0;
      m_PhMsk=0;
      }
    SpecieIter(PhMask Mask)
      {
      m_PhMsk = Mask & CDB.m_som_ALL; // for safety
      m_pSkipList=SVI.SetUpSkipList(m_PhMsk);
      m_pSkipListRev=NULL;
      m_fMustDelete=false;
      m_fMustDeleteRev=false;
      m_SpcCount=SVSpcCount();
      m_ValCount=SVValueCount();
      }
    ~SpecieIter()
      {
      if (m_fMustDelete && m_pSkipList)
        delete &m_pSkipList[-SVClcCount()];
      if (m_fMustDeleteRev && m_pSkipListRev)
        delete &m_pSkipListRev[-SVClcCount()];
      }
    int   SpcInx1() { return SVSpc1()-1;}
    int   SpcInxN() { return SVSpcN();}
    int   AttInx1() { return SVAtt1()-1;}
    int   AttInxN() { return SVAttN();}
    int   ClcInx1() { return SVClc1()-1;}
    int   ClcInxN() { return SVClcN();}
    flag  Loop(int &i)
      {
      i=i+1;
      i=i+m_pSkipList[i];
      return i<m_SpcCount;
      };
    flag  LoopClc(int &i)
      {
      i=i+1;
      i=i+m_pSkipList[i];
      return i<0;
      };
    flag  LoopAtt(int &i)
      {
      i=i+1;
      i=i+m_pSkipList[i];
      return i<m_ValCount;
      };
    flag  LoopRevClc(int &i)
      {
      if (m_pSkipListRev==NULL) m_pSkipListRev=SVI.SetUpSkipListRev(m_PhMsk);
      i=i-1;
      i=i-m_pSkipListRev[i];
      return i>=-SVClcCount();
      };
    flag  LoopRevSpc(int &i)
      {
      if (m_pSkipListRev==NULL) m_pSkipListRev=SVI.SetUpSkipListRev(m_PhMsk);
      i=i-1;
      i=i-m_pSkipListRev[i];
      return i>=0;
      };
    flag  LoopRevAtt(int &i)
      {
      i=i-1;
      i=i-m_pSkipList[i];
      return i>=SVSpcCount();
      };
    flag  LoopAll(int &i)
      {
      i=i+1;
      i=i+m_pSkipList[i];
      return i<m_ValCount;
      };
    void  CreateFrom(bool * RqdList, bool Invert);

    static flag FirstSpc(PhMask Mask, int &i);
  };
// ===========================================================================
//
//
//
// ===========================================================================

#if dbgSysVec
extern DllImportExport void DumpSysVec(bool Start, LPSTR Hd, CSysVector & Vec);
#define DMPSYSVEC(Start, Hd, Vec)  DumpSysVec(Start, Hd, Vec);
#else
#define DMPSYSVEC(Start, Hd, Vec) {}
#endif


#define DO_RIGOROUS_CHECKS 01
#if (DO_RIGOROUS_CHECKS && defined(_DEBUG))
  inline flag CheckMassNonNeg(CSysVector * p) { return (p->Mass(som_ALL)>=0.0); };
  #define ASSERT_MASS_OK(p) { p->CheckIt();/*ASSERT(_CrtCheckMemory());*/ if (!CheckMassNonNeg(p)) DoBreak(); };
#else
  #define ASSERT_MASS_OK(p)
#endif

// ===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport

#endif


