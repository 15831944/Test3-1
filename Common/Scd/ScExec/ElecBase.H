//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// generic Sfe DLL Interface

#ifndef __ELECBASE_H
#define __ELECBASE_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
//#include "sfe_base.h"

#ifdef __ELECBASE_CPP
#define DllImportExport DllExport
#elif !defined(SCEXEC)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif

XID xidConnFirst   = PwrXID(900000); 
XID xidConnLast    = PwrXID(999999); 
XID xidConnSrc     = 0;
XID xidConnDst     = 1;
XID xidConnVolts   = 2;
XID xidConnVAng    = 3;
XID xidConnAmps    = 4;
XID xidConnIAng    = 5;
XID xidConnVDrop   = 6; 
XID xidConnHtLoad  = 7; 
XID xidConnRAmps   = 8; 
XID xidConnXAmps   = 9; 
XID xidConnR       = 10;    
XID xidConnX       = 11;    
XID xidConnV       = 12;     
XID xidConnPh      = 13;    

XID xidConnCount   = 15;

// ==========================================================================
//
//
//
//===========================================================================

// forward
class CNodeElectrics;
class CETerminal;
class CECompData;
class CEComponent;
class CETermStrip;
class CTNode;

enum eElecChanges {eECC_Add, /*eECC_Change,*/ eECC_Delete, eECC_Connect, eECC_DisConnect };

// Must match each other
enum eCondMaterial {eCM_Copper, eCM_Aluminium, eCM_CondCount};
extern DllImportExport const double gs_CondResistance[];
extern DllImportExport DDBValueLst DDBCondMaterial[];

// ==========================================================================
//
//
//
//===========================================================================

#include <complex>
class CDComplex : public std::complex<double>
  {
  public:
    CDComplex() : std::complex<double>(0,0)  { };
    CDComplex(std::complex<double> X) : std::complex<double>(X) { };
    CDComplex(double V) : std::complex<double>(V,0) { };
    CDComplex(double R, double I) : std::complex<double>(R,I) { };

    double          Mag() { return sqrt(real()*real()+imag()*imag()); };
    double          Ang() { return atan2(imag(), real()); };
  };

// ==========================================================================
//
//
//
//===========================================================================

const double LargeElecRes=1.0e20;
const double OpenCctElecRes=0.9e20;

// ==========================================================================
//
//
//
//===========================================================================

enum eTSConfigurations
  {
  //eCT_Null    = 0, 
  eCT_User    = 0,
  eCT_1Ph2W   = 2,
  eCT_3Ph3W   = 3, 
  eCT_3Ph4W   = 4 
  };

class CETermDefn
  {
  public:
    //....
    CETermDefn ()                                 { };
    CETermDefn (LPCTSTR Name)                     { m_sName=Name; };
    CETermDefn (const CETermDefn & W)             { *this=W; };
    CETermDefn & operator=(const CETermDefn & W)  { m_sName=W.m_sName; return *this; };

    LPCTSTR         Name()                        { return m_sName; };
    void            PutName(LPCTSTR Nm)           { m_sName=Nm; };
  public:
    CString   m_sName;
  };

//---------------------------------------------------------------------------

class DllImportExport CETermStripDefn
  {
  friend class CScdTerminal;
  friend class CScdTerminals;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  friend class CScdEComponent;
  friend class CScdEComponents;
  friend class CScdWire;
  public :
    CETermStripDefn()
      {
      m_bIn    = true;
      m_bReqd  = false;
      }
    CETermStripDefn(const CETermStripDefn & C)
      {
      *this=C;
      }
    CETermStripDefn& operator=(const CETermStripDefn & C)
      {
      m_sName  = C.m_sName;
      m_bIn    = C.m_bIn;
      m_bReqd  = C.m_bReqd;
      m_Terms.SetSize(C.m_Terms.GetSize());
      for (int i=0; i<C.m_Terms.GetSize(); i++)
        m_Terms[i]=C.m_Terms[i];
      return *this;
      }

    long            TerminalCount() const { return m_Terms.GetSize(); }; 
    LPCTSTR         Name() const          { return m_sName; };
    void            PutName(LPCTSTR Nm)   { m_sName=Nm; };
    CETermDefn    & operator[](long i)    { return m_Terms[i]; };

  protected:
  public:

    CString         m_sName;
    bool            m_bIn;
    bool            m_bReqd;
    CArray <CETermDefn, CETermDefn&> m_Terms;

  };

//---------------------------------------------------------------------------

class DllImportExport CETermStripDefnArray : public CArray <CETermStripDefn, CETermStripDefn&> 
  {
  protected:
  public:
    CETermStripDefnArray() { };
    CETermStripDefnArray(const CETermStripDefnArray & C) { *this=C; };
    CETermStripDefnArray& operator=(const CETermStripDefnArray & C)
      {
      SetSize(C.GetSize());
      for (int i=0; i<C.GetSize(); i++)
        ElementAt(i)=C[i];
      return *this;
      }
    long            TermStripCount() const { return GetSize(); }; 
    long            Find(LPCTSTR Nm) 
      {
      for (int i=0; i<GetSize(); i++)
        if (stricmp(Nm, ElementAt(i).Name())==0)
          return i;
      return -1;
      }
  };

// ==========================================================================

class CEComponentPtrs : public CArray <CEComponent*, CEComponent*> {};
class CETerminalPtrs : public CArray <CETerminal*, CETerminal*> {};
class CETermStripPtrs : public CArray <CETermStrip*, CETermStrip*> {};

class CEPathHelper 
  {
  public:
    long                m_lStartPathNo;
    long                m_iCurrentPathNo;
    long                m_lCPathLen;
    long                m_lTPathLen;
    long                m_nLoadCount;
    CEComponent      * m_pVoltSrc;
    CEComponentPtrs       m_CPath;
    CETerminalPtrs      m_TPath;

    CEPathHelper ()
      {
      Clear(NULL);
      };
    void Clear(CEComponent * VoltSrc)
      {
      m_pVoltSrc=VoltSrc;
      m_lStartPathNo=0;
      m_iCurrentPathNo=0;
      m_lTPathLen=0;
      m_lCPathLen=0;
      m_nLoadCount=0;
      };
  };

//---------------------------------------------------------------------------

//class CSPETerminal : public CSmartPtr<CETerminal> {};

class DllImportExport CETerminal
  {
  friend class CScdTerminal;
  friend class CScdTerminals;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  friend class CScdEComponent;
  friend class CScdEComponents;
  friend class CScdWire;
  public:
    CETerminal(CETermStrip * pTStrip)
      {
      m_pTStrip = pTStrip;
      m_bDoInit = true;
      m_bRefV   = false;
      m_bDone   = false;
      }
    CETerminal(CETermStrip * pTStrip, LPCTSTR Nm, double V)
      {
      m_pTStrip = pTStrip;
      m_sName   = Nm;
      m_V       = CDComplex(V);
      //m_I       = 0;
      m_RefV    = 0; 
      m_bDoInit = true;
      m_bRefV   = false;
      }
    CETerminal(CETermStrip * pTStrip, const CETerminal & T)
      {
      m_pTStrip = pTStrip;
      *this=T;
      }
    CETerminal(CETermStrip * pTStrip, const CETermDefn & D)
      {
      m_pTStrip = pTStrip;
      *this=D;
      }
    ~CETerminal();
    CETerminal & operator=(const CETerminal & T)
      {
      m_sName = T.m_sName;
      m_V     = T.m_V;
      m_RefV  = T.m_RefV;

      m_bDoInit = T.m_bDoInit;
      m_bRefV   = T.m_bRefV;
      return *this;
      }
    CETerminal & operator=(const CETermDefn & T)
      {
      m_sName = T.m_sName;
      return *this;
      }

    LPCTSTR           Name()                                { return m_sName; };
    void              PutName(LPCTSTR Nm)                   { m_sName=Nm; };

    CString           FullName();

    long              AddComponent(CEComponent *pC);
    long              RemoveComponent(CEComponent *pC);
    long              RemoveComponent(long i);
    CEComponent     * FindRemote(long Id, CETerminal * pTRmt);
    bool              FindPaths(CEComponent *pC, CEPathHelper &PH, long Lvl);
    bool              CalcVolts(CEComponent *pC, CEPathHelper &PH);
    bool              HasRefVoltage()                       { return m_bRefV; };
    void              SetRefVoltage(const CDComplex & RefV)   { m_bRefV=true; m_RefV=RefV; };
    void              OnElecComponentChange(eElecChanges EC, CECompData *pC);

    CDComplex         Volts()                               { return m_V; };
    CDComplex         Current(int Id, CETerminal * pT);

  public:
    CETermStrip     * m_pTStrip;
    CString           m_sName;
    bool              m_bDoInit;
    CDComplex         m_V, m_VMem;//, m_I;

    bool              m_bRefV;
    CDComplex         m_RefV;

    bool              m_bBusy, m_bDone;
    CEComponentPtrs m_TrmComponents;

  };

//---------------------------------------------------------------------------

//class CSPETermStrip : public CSmartPtr<CETermStrip> {};

class DllImportExport CETermStrip  
  {
  friend class CEComponent;
  friend class CETerminal;
  friend class CETermStripArray;
  friend class CNodeElectrics;
  friend class CScdTerminal;
  friend class CScdTerminals;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  friend class CScdEComponent;
  friend class CScdEComponents;
  friend class CScdWire;

  public:
    CETermStrip(CTNode * pNd, LPCTSTR pName, eTSConfigurations Cfg) : \
      m_pNd(pNd), 
      m_sName(pName)   
      { 
      m_pTSC=NULL;
      m_bInternal=false;
      SetConfiguration(Cfg); 
      }
    CETermStrip(CTNode * pNd, const CETermStrip& C) : m_pNd(pNd)            { m_pTSC=NULL; *this=C; };
    CETermStrip(CTNode * pNd, const CETermStripDefn& D) : m_pNd(pNd)        { m_pTSC=NULL; *this=D; };
    CETermStrip& operator=(const CETermStrip& C)
      {
      m_sName = C.m_sName;
      m_eCfg = C.m_eCfg;
      m_bInternal = C.m_bInternal;

      SetTerminalCount(C.m_Terms.GetSize());
      for (int i=0; i<C.m_Terms.GetSize(); i++)
        (*m_Terms[i])=(*C.m_Terms[i]);

      return *this;
      }
    CETermStrip& operator=(const CETermStripDefn& D)
      {
      m_sName=D.m_sName;
      m_eCfg=eCT_User;
      m_bInternal = false;
      SetTerminalCount(D.TerminalCount());
      for (int i=0; i<D.TerminalCount(); i++)
        (*m_Terms[i])=D.m_Terms[i];

      return *this;
      }
    ~CETermStrip();
    void          Add(LPCTSTR TrmName)          { m_Terms.Add(new CETerminal(this, TrmName, 0)); };
    void          Add(const CETerminal &T)      { m_Terms.Add(new CETerminal(this, T)); };

    void          SetTerminalCount(long N)
      {
      int M=m_Terms.GetSize();
      for (int i=N; i<M; i++)
        delete m_Terms[i];
      m_Terms.SetSize(N);
      for (i=M; i<N; i++)
        m_Terms[i]=new CETerminal(this);
      }
    long          TerminalCount() const         { return m_Terms.GetSize(); }; 
    CETerminal  * Find(LPCTSTR Nm)
      {
      for (int i=0; i<m_Terms.GetSize(); i++)
        if (stricmp(Nm, m_Terms[i]->Name())==0)
          return m_Terms[i];
      return NULL;
      }
    CETerminal        * operator[](LPCTSTR Nm)  { return Find(Nm); };
    CETerminal        * operator[](long TermNo) { return m_Terms[TermNo]; };
    CETerminal        * Terminal(long TermNo)   { return m_Terms[TermNo]; };

    //long RemoveAllComponents();

    LPCTSTR             Name()                  { return m_sName; };
    void                SetName(LPCTSTR Nm)     { m_sName=Nm; };
    bool                Internal()              { return m_bInternal; };
    void                SetInternal(bool Int)   { m_bInternal=Int; };

    LPCTSTR             NdName();//                { return m_pNd->Tag(); };
    CString             FullName();//              { return m_sName+":"+m_pNd->Tag(); };

    void                SetDoInit(bool Init);

    eTSConfigurations   Configuration() { return m_eCfg; };
    void                SetConfiguration(eTSConfigurations Cfg);
    void                SetWireNames(CETermStripDefn & Terms);

    void                SetLoad(double Power, double PwrFactor);
    void                SetSource(double LineVolts, double Phase, CDComplex Z);
    bool                IsPowerStd();
    bool                PowerPresent();

    void                BuildDataDefn(DataDefnBlk & DDB, long TSIndex, bool WithArray);//, LPCTSTR Tag);
    flag                DataXchg(DataChangeBlk & DCB, long TSIndex);

  protected:
    CString               m_sName;
    CNodeElectrics      * m_pTSC;
    CTNode              * m_pNd;
    eTSConfigurations     m_eCfg;
    bool                  m_bInternal;

    CArray <CETerminal*,  CETerminal*>   m_Terms;
  };

//---------------------------------------------------------------------------

class DllImportExport CECompData
  {
  friend class CNodeElectrics;
  friend class CNodeElectricsArray;
  friend class CScdTerminal;
  friend class CScdTerminals;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  friend class CScdEComponent;
  friend class CScdEComponents;
  friend class CScdWire;
  public:
    CEComponent * m_pOwner;
    bool        m_bAsLoad;
    bool        m_bIsNew;

    CDComplex   m_V;
    CDComplex   m_I;
    CDComplex   m_Z, m_Zt;
    CDComplex   m_SrcV; // copy of data when disconnecting
    CDComplex   m_DstV; // copy of data when disconnecting
    //CDComplex   xm_AppV; // copy of data when disconnecting

    enum { eTypeSimple, eTypePwr } m_eType;
    union 
      {
      struct
        {
        double      m_dPwr;
        double      m_dPwrFactor;
        };
      };

    CECompData() : m_Zt(1.0e6,0.0)
      {
      m_pOwner          = NULL;
      m_eType           = eTypeSimple;
      m_bAsLoad         = false;
      m_bIsNew          = true;
      m_dPwr            = 0;
      m_dPwrFactor      = 1;
      }
    CECompData(const CDComplex & V, const CDComplex & Z) : m_Zt(1.0e6,0.0)
      {
      m_pOwner          = NULL;
      m_eType           = eTypeSimple;
      m_bAsLoad         = false;
      m_bIsNew          = true;
      m_V               = V;
      m_Z               = Z;
      m_dPwr            = 0;
      m_dPwrFactor      = 1;
      }
    CECompData(const CECompData & W)  : m_Zt(1.0e6,0.0)
      {
      *this=W;
      }
    CECompData & operator=(const CECompData& W)
      {
      m_bIsNew          = W.m_bIsNew;
      m_eType           = W.m_eType;
      m_bAsLoad         = W.m_bAsLoad;
      m_V               = W.m_V;
      m_I               = W.m_I;
      m_Z               = W.m_Z;
      m_Zt              = W.m_Zt;
      m_SrcV            = W.m_SrcV;
      m_DstV            = W.m_DstV;
      switch (m_eType)
        {
        case eTypeSimple: 
          break;
        case eTypePwr:
          m_dPwr        = W.m_dPwr;
          m_dPwrFactor  = W.m_dPwrFactor;
          break;
        };
      return *this;
      }
    ~CECompData() {};

    bool            HasVoltageSrc() { return m_V.Mag()>1.0e-20; };
    void            SetAsLoad(bool On) { m_bAsLoad=On; };
    bool            IsLoad() { return m_bAsLoad; };

    void            SetLoad(double Pwr, double PwrFactor);
    void            SetImpedance(CDComplex Z, bool AsLoad=false);
    void            SetSource(CDComplex V, CDComplex Z);

    inline CDComplex SrcVoltage();
    inline CDComplex DstVoltage();
    inline CDComplex AppliedVoltage();
    inline bool      VoltagePresent();
    inline bool      ClosedCct();

    CDComplex       TotalImpedance()    { return m_Zt; };
    CDComplex       LoadImpedance()     { return m_Zt-m_Z; };
    CDComplex       InternalImpedance() { return m_Z; };
    CDComplex       Current()           { return m_I; };
    double          HeatLoad()          { return Sqr(m_I.Mag())*m_Z.real()*0.001; }; // heatloss in kW

    void            BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pTagObj, long Id, LPCTSTR Tag);
    flag            DataXchg(DataChangeBlk & DCB, TaggedObject * pTagObj, long Id);
  };

//---------------------------------------------------------------------------

class DllImportExport CEComponent
  {
  friend class CNodeElectrics;
  friend class CNodeElectricsArray;
  friend class CScdTerminal;
  friend class CScdTerminals;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  friend class CScdEComponent;
  friend class CScdEComponents;
  friend class CScdWire;
  public:
    long            m_Id;
    bool            m_bOwnsCompData;

    long            m_lPathCount;
    long            m_nSolveCount;
    long            m_nSolveCountPrv;

    CECompData    * m_pCompData;
    CTNode        * m_pNd;
    CETerminal    * m_pSrc;
    CETerminal    * m_pDst;

    CEComponent(long Id) 
      {
      m_Id                  = Id;
      m_pCompData           = new CECompData;
      m_pCompData->m_pOwner = this;
      m_bOwnsCompData       = true;
      m_lPathCount          = 0;
      m_pNd                 = NULL;
      m_pSrc                = NULL;
      m_pDst                = NULL;
      m_nSolveCount         = -1;
      m_nSolveCountPrv      = -2;
      }
    CEComponent(long Id, const CDComplex & V, const CDComplex & Z)
      {
      m_Id                  = Id;
      m_pCompData           = new CECompData(V, Z);
      m_pCompData->m_pOwner = this;
      m_bOwnsCompData       = true;
      m_lPathCount          = 0;
      m_pNd                 = NULL;
      m_pSrc                = NULL;
      m_pDst                = NULL;
      m_nSolveCount         = -1;
      m_nSolveCountPrv      = -2;
      }
    CEComponent(const CEComponent & W)
      {
      *this=W;
      }
    CEComponent & operator=(const CEComponent& W)
      {
      m_Id          = W.m_Id;
      *m_pCompData = *W.m_pCompData;
      m_lPathCount  = W.m_lPathCount;
      m_pNd         = W.m_pNd;
      m_pSrc        = W.m_pSrc;
      m_pDst        = W.m_pDst;
      return *this;
      }
    ~CEComponent();

    LPCTSTR         SrcNdName() { return m_pSrc->m_pTStrip->NdName(); };
    LPCTSTR         SrcTSName() { return m_pSrc->m_pTStrip->Name(); };
    LPCTSTR         SrcTName()  { return m_pSrc->Name(); };
    LPCTSTR         DstNdName() { return m_pDst->m_pTStrip->NdName(); };
    LPCTSTR         DstTSName() { return m_pDst->m_pTStrip->Name(); };
    LPCTSTR         DstTName()  { return m_pDst->Name(); };

    LPCTSTR         SrcName(); 
    LPCTSTR         DstName(); 

    void            AddParallel(double R, double X);
    void            AddSeries(double R, double X);
    bool            FindPaths(CETerminal *pT, CEPathHelper &PH, long Lvl);
    bool            CalcVolts(CETerminal *pT, CEPathHelper &PH);

    bool            CalcImpedance();
    bool            CalcPathCurrent(CEPathHelper &PH);
    long            PathCount() { return m_lPathCount; };

    CDComplex       SrcVoltage()        { return m_pCompData->SrcVoltage()        ; };
    CDComplex       DstVoltage()        { return m_pCompData->DstVoltage()        ; };
    CDComplex       HiVoltage()         { CDComplex S=SrcVoltage(), D=DstVoltage(); return S.Mag()>D.Mag() ? S : D; };
    CDComplex       LoVoltage()         { CDComplex S=SrcVoltage(), D=DstVoltage(); return S.Mag()<D.Mag() ? S : D; };
    CDComplex       AppliedVoltage()    { return m_pCompData->AppliedVoltage()    ; };
    CDComplex       TotalImpedance()    { return m_pCompData->TotalImpedance()    ; };
    CDComplex       LoadImpedance()     { return m_pCompData->LoadImpedance()     ; };
    CDComplex       InternalImpedance() { return m_pCompData->InternalImpedance() ; };
    CDComplex       Current()           { return m_pCompData->Current()           ; };
    double          HeatLoad()          { return m_pCompData->HeatLoad()          ; }; // heatloss in kW

  };

//---------------------------------------------------------------------------

inline CDComplex CECompData::SrcVoltage()     { return m_pOwner ? m_pOwner->m_pSrc->m_V : m_SrcV; };
inline CDComplex CECompData::DstVoltage()     { return m_pOwner ? m_pOwner->m_pDst->m_V : m_DstV; };
inline CDComplex CECompData::AppliedVoltage() { return m_pOwner ? (m_pOwner->m_pDst->m_V-m_pOwner->m_pSrc->m_V) : m_DstV-m_SrcV; };
inline bool CECompData::VoltagePresent()      { return (m_pOwner && m_pOwner->m_pSrc) ? (m_pOwner->m_pSrc->m_V.Mag()>=0.001) : false; }
inline bool CECompData::ClosedCct()           { return (m_pOwner) ? (m_pOwner->m_lPathCount>0): false; }

//---------------------------------------------------------------------------

inline CDComplex CETerminal::Current(int Id, CETerminal * pT) { CEComponent * p=FindRemote(Id, pT); return p ? p->Current() : CDComplex(0,0); };

//---------------------------------------------------------------------------

typedef CSmartPtrAllocate<CECompData> CSPECompData;
typedef CArray<CSPECompData, CSPECompData&> CECompDataArray;

//---------------------------------------------------------------------------

class DllImportExport CETermStripArray : public CArray <CETermStrip*, CETermStrip*> 
  {
  protected:  
    CTNode * m_pNd;
  public:
    CETermStripArray (CTNode * pNd) : m_pNd(pNd) {};
    CETermStripArray &operator=(const CETermStripDefnArray & D);
    ~CETermStripArray () 
      {
      for (int i=0; i<GetSize(); i++)
        delete ElementAt(i);
      };
    void            Add(const CETermStripDefn & D)  {  SetAtGrow(GetSize(), new CETermStrip(m_pNd, D));  };
  }; 

//---------------------------------------------------------------------------

class DllImportExport CNodeElectrics  
  {
  friend class CNodeElectricsArray;
  friend class CEComponent;
  friend class CScdTerminal;
  friend class CScdTerminals;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  friend class CScdEComponent;
  friend class CScdEComponents;
  friend class CScdWire;
  public://protected:
    CTNode        * m_pNd;
    long          * m_pSolveCount;
    long            m_lTermStripCount;
    long            m_lTopoCount;


    CEComponentPtrs m_Components;
    CETermStripPtrs m_TermStrips;

  public:

    CNodeElectrics(CTNode * pNd);
    ~CNodeElectrics();

    long            GetTopoCount()  { return m_lTopoCount; };
    void            BumpTopoCount() { m_lTopoCount=(m_lTopoCount+1)%(LONG_MAX-10); };

    LPCTSTR         NodeName();
    long            ComponentCount()                        { return m_Components.GetSize(); };
    CEComponent   * Component(long CNo)                     { return m_Components[CNo]; };
    CECompData    * CompData(long CNo)                    { return m_Components[CNo]->m_pCompData; };

    long            TermStripCount() const                { return m_lTermStripCount; }; 
    CETermStrip   * TermStrip(long i)                     { return m_TermStrips[i]; }; 
    void            ResetTermStripCount()                 { m_lTermStripCount=0; }; 
    void            AddTermStrip(CETermStrip * pTS) 
      { 
      m_TermStrips.SetAtGrow(m_lTermStripCount++, pTS);
      pTS->m_pTSC=this;
      }; 

    CETermStrip   * Find(LPCTSTR Nm) 
      {
      for (int i=0; i<m_lTermStripCount; i++)
        if (stricmp(Nm, m_TermStrips[i]->Name())==0)
          return m_TermStrips[i];
      return NULL;
      }

    CETermStrip   * operator[](long i)                    { return m_TermStrips[i];};
    CETermStrip   * operator[](LPCTSTR Nm)                { return Find(Nm);};

    long            TerminalCount(long TStripNo)          { return (*m_TermStrips[TStripNo]).TerminalCount(); };
    CETerminal    * Terminal(long TStripNo, long TermNo)  { return (*m_TermStrips[TStripNo])[TermNo]; };
    CETerminal    * Terminal(CETermStrip& T, long TermNo) { return T[TermNo]; };
    CETerminal    * Terminal(CETermStrip& T, LPCTSTR TNm) { return T[TNm]; };
    CETerminal    * Terminal(LPCTSTR TSNm, LPCTSTR TNm)    
      {
      CETermStrip * pTS=Find(TSNm);
      if (pTS)
        return pTS->Find(TNm);
      return NULL; 
      };

    void            SetCompData(CTNode * pNd, long Id, CETerminal * Src, CETerminal * Dst, CECompData * pCompData);//, bool External);
    void            TouchCompData(CECompData * pCompData);
    void            RemoveCompData(CECompData * pCompData);
    void            RemoveAllComponents();

    CECompData   * GetCompData(long Id, CETerminal *Src, CETerminal *Dst, bool AllowNew, bool * IsNew=NULL);

    CECompData   * SetLoad(long Id, CETerminal * Src, CETerminal * Dst, double Pwr, double PwrFactor);

    CECompData   * SetImpedance(long Id, CETerminal * Src, CETerminal * Dst, CDComplex Z, bool AsLoad=false);
    CECompData   * SetImpedance(long Id, CETerminal * Src, CETerminal * Dst, double R, double X, bool AsLoad=false) 
      { return SetImpedance(Id, Src, Dst, CDComplex(R, X), AsLoad); };
    CECompData   * SetSource(long Id, CETerminal * Src, CETerminal * Dst, CDComplex V, CDComplex Z);
    CECompData   * SetSource(long Id, CETerminal * Src, CETerminal * Dst, double V, double Phi, double R, double X) 
      { return SetSource(Id, Src, Dst, CDComplex(std::polar(V,Phi)), CDComplex(R, X)); };
    void            SetReferenceVoltage(CETerminal * Term, CDComplex V);
    void            SetReferenceVoltage(CETerminal * Term, double V, double Phi) 
      { SetReferenceVoltage(Term, CDComplex(std::polar(V,Phi))); };

  protected:
    CEComponent     * GetComponent(long Id, CETerminal *Src, CETerminal *Dst, bool AllowNew, bool * IsNew=NULL);
    void            AddComponent(CTNode * pNd, CETerminal * Src, CETerminal * Dst, CEComponent * pComponent);//, bool External);
    void            TouchComponent(CEComponent * pComponent);
    void            RemoveComponent(CEComponent * pComponent);

  };

// ==========================================================================
//
//
//
//===========================================================================

class DllImportExport CNodeElectricsArray  
  {
  friend class CNodeElectrics;
  friend class CEComponent;
  protected:
    long          m_nSolveCount;
    long          m_lNodeCount;
    long          m_lTermStripCollectCount;

    CArray <CTNode*, CTNode*> m_Nodes;
    CArray <CNodeElectrics*, CNodeElectrics*> m_TermStripCollects;

  public:  
    CNodeElectricsArray()
      {
      m_nSolveCount=0;
      m_lTermStripCollectCount=0;
      m_TermStripCollects.SetSize(0,64);
      m_lNodeCount=0;
      m_Nodes.SetSize(0,64);
      }
    CNodeElectrics * operator[](int i) { return m_TermStripCollects[i]; };

    void          Add(CTNode * pNd, CNodeElectrics * pTS); 

    void          InitialiseCollection(); 
    void          SolveElectrics(bool ConvergeIt); 
    long          SolveCount() { return m_nSolveCount; };
    void          TerminateCollection();

  protected:
    void          InitialiseSolution(); 
    long          StartSolution();
    bool          Solve();
    void          FinaliseSolution();

  };

// ==========================================================================
//
//
//
//===========================================================================

inline CString CETerminal::FullName()
  { 
  CString S(m_pTStrip->Name());
  return S+":"+m_sName;
  };

//--------------------------------------------------------------------------

class CEWire
  {
  public:

    CEWire()
      {
      m_Area=10e-6;
      m_Length=1;
      m_pSrcTerm=NULL;
      m_pDstTerm=NULL;
      }
    ~CEWire() 
      {
      }

    LPCTSTR Tag()               { return m_sTag; };
    LPCTSTR SrcTermStripName()  { return m_sSrcTermStripName; };
    LPCTSTR SrcTerminalName()   { return m_sSrcTerminalName;  };
    LPCTSTR DstTermStripName()  { return m_sDstTermStripName; };
    LPCTSTR DstTerminalName()   { return m_sDstTerminalName;  };
    CString FullSrcName()       { return m_sSrcTermStripName+":"+m_sSrcTerminalName; };
    CString FullDstName()       { return m_sDstTermStripName+":"+m_sDstTerminalName; };
    CETerminal * SrcTerm()      { return m_pSrcTerm; };
    CETerminal * DstTerm()      { return m_pDstTerm; };

  public:
    CString     m_sTag;
    CString     m_sSrcTermStripName;
    CString     m_sSrcTerminalName;
    CString     m_sDstTermStripName;
    CString     m_sDstTerminalName;
    double      m_Area;
    double      m_Length;
    CECompData  m_CompData;
    CETerminal * m_pSrcTerm;
    CETerminal * m_pDstTerm;
  };

// ==========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif
