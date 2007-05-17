//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __VLEBASE_H
#define  __VLEBASE_H

#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
#include "blockevalbase.h"
#include "scdarray.h"

#if defined(__VLEBASE_CPP)
#define DllImportExport DllExport
#elif !defined(FLWLIB)
#define DllImportExport DllImport
#else
#define DllImportExport
#endif


// =========================================================================

_FWDDEF(CVLEBlock);

#define WITHVLETIMERS WITHTIMERS
#if WITHVLETIMERS 
#define VLETIMERLAP(x) CStopWatchLap SW##__LINE__(CVLEBase::x)
#define VLETIMESTUFF(x) x;
#else
#define VLETIMERLAP(x)
#define VLETIMESTUFF(x)
#endif

//============================================================================
//
// 
//
//============================================================================

XID xidVLEEnable        = MdlBsXID(2000);
XID xidVLEEqnNm         = MdlBsXID(2001);
XID xidVLENettVapQm     = MdlBsXID(2002);
XID xidVLEFlashMethod   = MdlBsXID(2003);
XID xidVLEFlashComp     = MdlBsXID(2004);

//============================================================================

class DllImportExport CVLEData
  {
  friend class  CVLEBlock;
  public:
    int            iCmpNo, li, vi, iIndex;
    double         dData;

    CVLEData()
      {
      iCmpNo=-1;
      dData=1.0;
      li=-1;
      vi=-1;
      iIndex=-1;
      };
    CVLEData(int CmpNo, double Data, int Li, int Vi)
      {
      iCmpNo=CmpNo;
      dData=Data;
      li=Li;
      vi=Vi;
      iIndex=-1;
      };
    ~CVLEData()
      {
      };
  };

typedef CArray   <CVLEData, CVLEData&> CVLEDataArray;

//============================================================================

class DllImportExport CVLEBlock : public TaggedObject
  {
  friend class CVLEBase;
  public:
    static CVLEBlock * Open(pchar TagIn, pTaggedObject pAttach);
    static void    Close(CVLEBlock* &pVLE);


    CVLEBlock(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CVLEBlock();

    virtual CSaturationDefn FlashDefn() { return CSaturationDefn(FlashMethod(), FlashCmpIndex()); };
    virtual int    FlashMethod()=0;
    virtual int    FlashCmpIndex()=0;
    virtual int    FlashLiqIndex()=0;
    virtual int    FlashVapIndex()=0;
    virtual LPTSTR FlashDescription()=0;

    virtual double FlashVapFrac(SpModel &Mdl) = 0;
    virtual void   SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags)  {};
    virtual void   TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags)                          {};
    virtual void   PFlash(SpConduit &Q, double Press, double Duty, dword Flags)                           {};
    virtual void   PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags)           {};
    virtual void   VFlash(SpContainer &C, double Duty, dword Flags)                                       {};
    virtual void   AddQVDerivs(SpContainer &C, double Duty, dword Flags)                                  {};
    virtual void   MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags)                         {};
    virtual void   MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags)         {};
    virtual int    GetVLEData(CVLEDataArray & Array)                                                      { return 0; }; 
    virtual double CondensableFrac(SpConduit *Ql, SpConduit *Qv)                                          { return dNAN; };
    virtual double CondensableFrac(SpContainer*Cn)                                                        { return dNAN; };

    virtual double ApproachTemp()=0;

    virtual flag   HasTPEnvelope() = 0;
  protected:

    void SetMixFrac(SpMArray *pVMixFrac, SpMArray *pLMixFrac);
    void SetMixFrac(double dVMixFrac, double dLMixFrac);
    void SetMixFrac(int CompNo, double dVMixFrac, double dLMixFrac);

  public:
    static const pchar GroupName;

  protected:
    FlwNode        &Nd;
    CDArray         VMixFrac, LMixFrac;
    CVLEBase *      m_pVLEBase;

  };

//============================================================================

class DllImportExport CVLEBase : public CClosureHfGain, public CBlockEvalBase
  {
  public:
    CVLEBase(pTaggedObject pAttach, dword SelectMask, LPTSTR Name="VLE");
    virtual ~CVLEBase();

    flag           Open(TagObjClass * pVLEClass=NULL, flag Fixed=False);
    flag           Open(byte OnOffSeq, TagObjClass * pVLEClass=NULL, flag Fixed=False) { CBlockEvalBase::Open(OnOffSeq); return Open(pVLEClass, Fixed); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    void           Close();
    void           SetFlags(int Flags) { m_iFlags=Flags; };
    int            Flags()             { return m_iFlags; };


    int TPFlashReqd()     { return (m_dwSelectMask&VLEF_TPFlash)!=0; };
    int QPFlashReqd()     { return (m_dwSelectMask&VLEF_QPFlash)!=0; };
    int QVFlashReqd()     { return (m_dwSelectMask&VLEF_QVFlash)!=0; };
    int QMVapFlashReqd()  { return (m_dwSelectMask&VLEF_QMVapFlash)!=0; };

    CVLEBlock *    operator() () { return m_pVLE; };
    flag           Enabled()     { return m_fEnabled && (m_pVLE!=NULL); };
    void           Enable()      { m_fEnabled = true; };
    void           Disable()     { m_fEnabled = false; };
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped);
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag="VLE", char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);

    virtual CSaturationDefn FlashDefn();
    virtual int    FlashMethod();
    virtual int    FlashCmpIndex();
    virtual int    FlashLiqIndex();
    virtual int    FlashVapIndex();
    LPTSTR         FlashDescription();

    virtual double FlashVapFrac(SpModel &Mdl);
    virtual void   SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags);
    virtual void   SetFlashVapFrac(SpModel &Mdl, double VapFrac, dword Flags);
    virtual void   SetFlashVapFrac(SpConduit &Q, double Temp, double Press, double VapFrac, dword Flags);
    virtual void   SetFlashVapFrac(SpConduit &Q, double VapFrac, dword Flags);
    virtual void   TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags);
    virtual void   PFlash(SpConduit &Q, double Press, double Duty, dword Flags);
    virtual void   PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags);
    virtual void   VFlash(SpContainer &C, double Duty, dword Flags);
    virtual void   AddQVDerivs(SpContainer &C, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags);
    virtual int    GetVLEData(CVLEDataArray & Array);
    virtual double CondensableFrac(SpConduit *Ql, SpConduit *Qv);
    virtual double CondensableFrac(SpContainer*Cn);

    virtual double ApproachTemp();
    virtual bool   ApproachTempOn() { return (m_iFlags & VLEF_ApproachTempOn)!=0; }

    flag           HasTPEnvelope();

    void           SetMixFrac(SpMArray *pVMixFrac=NULL, SpMArray *pLMixFrac=NULL);
    void           SetMixFrac(double dVMixFrac=1.0, double dLMixFrac=1.0);
    void           SetMixFrac(int CompNo, double dVMixFrac=1.0, double dLMixFrac=1.0);

    void           SaveFeedInfo(bool Cvt2Flow, SpModel * Ql, SpModel * Qv=NULL);
    void           SaveProdInfo(bool Cvt2Flow, SpModel * Ql, SpModel * Qv=NULL);

  protected:
    flag            m_fFixed;
    flag            m_fEnabled;
    CVLEBlock *     m_pVLE;
    pTaggedObject   m_pNd;
    dword           m_dwSelectMask;

    int             m_iFlags;

    //bool            m_AsFlow;
    double          m_TempIn;
    double          m_TotalPIn;
    double          m_PartialPIn;
    double          m_VapQmIn;
    double          m_VapFracIn;
    double          m_TotQmIn;
    double          m_FlshVapQmIn;
    double          m_FlshVapFracIn;

    double          m_TempOut;
    double          m_TotalPOut;
    double          m_PartialPOut;
    double          m_VapQmOut;
    double          m_VapFracOut;
    double          m_TotQmOut;
    double          m_FlshVapQmOut;
    double          m_FlshVapFracOut;
    
    double          m_BPE;
    Strng           m_sSpModel;
    Strng           m_sSatMethod;

  public:

#if WITHVLETIMERS 
    static CGlblStopWatch sm_swTPFlash;
    static CGlblStopWatch sm_swQPFlash;
    static CGlblStopWatch sm_swQVFlash;
    static CGlblStopWatch sm_swQVDerivs;
    static CGlblStopWatch sm_swQMVapFlash;
#endif

  };

inline void CVLEBase::SetMixFrac(SpMArray *pVMixFrac, SpMArray *pLMixFrac) 
  { if (m_pVLE) m_pVLE->SetMixFrac(pVMixFrac, pLMixFrac); };
inline void CVLEBase::SetMixFrac(double dVMixFrac, double dLMixFrac) 
  { if (m_pVLE) m_pVLE->SetMixFrac(dVMixFrac, dLMixFrac); };
inline void CVLEBase::SetMixFrac(int CompNo, double dVMixFrac, double dLMixFrac) 
  { if (m_pVLE) m_pVLE->SetMixFrac(CompNo, dVMixFrac, dLMixFrac); };
inline void CVLEBase::SetFlashVapFrac(SpConduit &Q, double Temp, double Press, double VapFrac, dword Flags)
  { SetFlashVapFrac(*Q.Model(), Temp, Press, VapFrac, Flags);}
inline void CVLEBase::SetFlashVapFrac(SpConduit &Q, double VapFrac, dword Flags)
  { SetFlashVapFrac(*Q.Model(), VapFrac, Flags);}

// =========================================================================

enum VLEStates 
  { 
  VLES_CfgError, 
  VLES_OK, 
  VLES_Nothing, 
  VLES_SubCooled, 
  VLES_BubblePt, 
  VLES_TwoPhase, 
  VLES_DewPt, 
  VLES_SuperHeated,
  VLES_NotConverged
  };

// =========================================================================
//
//
//
// =========================================================================

DEFINE_TAGOBJ(CVLEVapPress);
class DllImportExport CVLEVapPress : public CVLEBlock
  {
  friend class CVLEBase;
  friend class VPQVFnd;

  public:

    CVLEVapPress(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CVLEVapPress();
    void           BuildDataDefn(DataDefnBlk &DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);

    virtual int    FlashMethod();
    virtual int    FlashCmpIndex();
    virtual int    FlashLiqIndex();
    virtual int    FlashVapIndex();
    virtual LPTSTR FlashDescription();

    virtual double FlashVapFrac(SpModel &Mdl);
    virtual void   SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags);
    virtual void   TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags);
    virtual void   PFlash(SpConduit &Q, double Press, double Duty, dword Flags);
    virtual void   PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags);
    virtual void   VFlash(SpContainer &C, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags);
    virtual int    GetVLEData(CVLEDataArray & Array);
    virtual double CondensableFrac(SpConduit *Ql, SpConduit *Qv);
    virtual double CondensableFrac(SpContainer*Cn);
    virtual double SaturationTotalP(SpModel * pMdl, double Temp, CSysVector * pMA);
    virtual double SaturationP(SpModel * pMdl, double Temp, CSysVector * pMA);

    virtual double ApproachTemp();

    virtual flag   HasTPEnvelope() { return False;};

    //  protected:
    int            Setup(SpModel *pMdlL_, SpModel *pMdlV_);
    void           SetVapFrac(double VapFrac);//, double RqdH);
    void           PFlash(double Press, double Duty, dword Flags);
    void           MVapFlash(double MVap, double Duty, dword Flags);
    double         CondensableFrac();

  public:  
    DEFINE_CI(CVLEVapPress, CVLEBlock, 4);

  public:
    //  protected:
    //    FlwNode       &Nd;
    SpModel       *pMdlL;
    SpModel       *pMdlV;

    SpecieIter     OtherVapSp;

    long           m_iFlashMethod;
    long           m_iCmpNoReqd, m_iCmpNo, m_iLiqNo, m_iVapNo;
    double         Mt,Ht,Vf;
    long           m_iState;
    byte           m_fModelSatP;
                        
    double         m_dPrvVapFrac;
    double         m_dPrvVapPress;
    double         m_dApproachTemp;

  };

// =========================================================================
//
//
//
// =========================================================================

#define WithFixedPerc 0

#if WithFixedPerc

DEFINE_TAGOBJ(CVLEFixdPerc);
class DllImportExport CVLEFixdPerc : public CVLEBlock
  {
  friend class CVLEBase;

  public:

    CVLEFixdPerc(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CVLEFixdPerc();
    void           BuildDataDefn(DataDefnBlk &DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);

    virtual LPTSTR FlashDescription();

    virtual double FlashVapFrac(SpModel &Mdl);
    virtual void   SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags);
    virtual void   TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags);
    virtual void   PFlash(SpConduit &Q, double Press, double Duty, dword Flags);
    virtual void   PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags);
    virtual void   VFlash(SpContainer &C, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags);
    virtual int    GetVLEData(CVLEDataArray & Array);

    virtual flag   HasTPEnvelope() { return False;}

  protected:
    int            NAvailComps() { return Cmp.GetSize(); };
    int            Setup(SpModel &Mdl);
    void           SetLVRatio(SpModel &Mdl);//, double R);

  public:  
    DEFINE_CI(CVLEFixdPerc, CVLEBlock, 4);

  protected:
    CVLEDataArray   Cmp;
    CDArray        M,F;
    double         Mt,Ht;
    int            iState;
    int            nComps;
  };

#endif

// =========================================================================
//
//
//
// =========================================================================

class DllImportExport CVLERootK : public CVLEBlock
  {
  friend class CVLEBase;
  friend class QPFnd;
  friend class QFnd1;
  friend class QFnd2;

  public:

    CVLERootK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CVLERootK();
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);

    virtual int    FlashMethod()   { return SMFM_Undefined; };
    virtual int    FlashCmpIndex() { return -1; };
    virtual int    FlashLiqIndex() { return -1; };
    virtual int    FlashVapIndex() { return -1; };
    virtual LPTSTR FlashDescription();

    virtual double FlashVapFrac(SpModel &Mdl);
    virtual void   SetFlashVapFrac(SpModel &Mdl, double Temp, double Press, double VapFrac, dword Flags);
    virtual void   TPFlash(SpModel &Mdl, double Temp, double Press, dword Flags);
    virtual void   PFlash(SpConduit &Q, double Press, double Duty, dword Flags);
    virtual void   PFlash(SpConduit &Ql, SpConduit &Qv, double Press, double Duty, dword Flags);
    virtual void   VFlash(SpContainer &C, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Q, double MVap, double Duty, dword Flags);
    virtual void   MVapFlash(SpConduit &Ql, SpConduit &Qv, double MVap, double Duty, dword Flags);    
    virtual int    GetVLEData(CVLEDataArray & Array);

    virtual flag   HasTPEnvelope() { return True;};

    virtual double ApproachTemp()  { return 0.0; };

  protected:
    int            NAvailComps() { return Cmp.GetSize(); };
    VLEStates      Setup(SpModel *pMdlL_, SpModel *pMdlV_);
    void           SetAllVapour();
    void           SetAllLiquid();
    void           SetMass();
    double         QPFlashFun(double T, double P);
    double         QVFlashFun1(double T, double P);
    virtual void   SetKRatios(double T, double P)=0;
    VLEStates      TestState();
    double         BubblePt();
    double         DewPt();
    double         SolveForLVRatio();
    void           SetLVRatio(double R);

  public:  
    DEFINE_CI(CVLERootK, CVLEBlock, 8);

  protected:
    //    FlwNode        &Nd;

    SpModel       *pMdlL;
    SpModel       *pMdlV;

    CVLEDataArray   Cmp;
    CDArray        X,Y,Z,&n,M,L,V,C,K,MvX,MlX;
    double         Mt,Lt,Vt,Ht,Vf;
    VLEStates      iState;
    int            nComps;
    CIArray        VSpIds;

    CArray <CVLEData*, CVLEData*> CData;
    CArray <CComponent*, CComponent*> CDefn;

    double         dPrvTemp, dPrvPress, dPrvVMoleFrac;

  };


// =========================================================================
//
//
//
// =========================================================================

DEFINE_TAGOBJ(CVLEFixdK);
class DllImportExport CVLEFixdK : public CVLERootK
  {
  friend class CVLEBase;
  public:

    CVLEFixdK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CVLEFixdK();
    void           BuildDataDefn(DataDefnBlk &DDB);

  protected:

    virtual void     SetKRatios(double T, double P);
  };

// =========================================================================
//
//
//
// =========================================================================

DEFINE_TAGOBJ(CVLEIdealK);
class DllImportExport CVLEIdealK : public CVLERootK
  {
  public:
    CVLEIdealK(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CVLEIdealK();
    void           BuildDataDefn(DataDefnBlk &DDB);

  protected:
    virtual void     SetKRatios(double T, double P);
  };

// =========================================================================
//
//
//
// =========================================================================

//class DllImportExport SpContPresetVLE : public CSpContPreset
//  {
//  public:
//    SpContPresetVLE(CVLEBase * VLE) { pVLE = VLE; };
//
//    virtual flag ValidateData(ValidateDataBlk & VDB, SpContainer & Cn);
//    virtual flag AdjustConditions(SpContainer & Cn);
//
//  protected:
//    CVLEBase * pVLE;
//  };

// =========================================================================
//
//
//
// =========================================================================

#undef DllImportExport

#endif
