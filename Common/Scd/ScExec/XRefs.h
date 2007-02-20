//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __XREFS_H
#define  __XREFS_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __PGM_E_H
#include "pgm_e.h"
#endif  
#ifndef __SIGFLW_H
#include "sigflw.h"
#endif  

#ifdef __XREFS_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

//return codes for ResolveNearXRef and ResolveFarXRef...
const DWORD FXR_Found             = 0x0001;
const DWORD FXR_NotFound          = 0x0010;
const DWORD FXR_NotParam          = 0x0020;
const DWORD FXR_ParamStopped      = 0x0040;                 
const DWORD FXR_BadValue          = 0x0080;                 
const DWORD FXR_BadCnv            = 0x0100;                 
const DWORD FXR_DynTag            = 0x0200;                 
const DWORD FXR_DynTagMissing     = 0x1000;                 
const DWORD FXR_DynTagNeverFound  = 0x2000;                 
                                    
//return codes for UpdateXRefLists...
const long XR_OK         = 0;
const long XR_InitValue  = -1;
const long XR_BadTags    = -2;

enum eXRefReach
  {
  XR_Near       = 0,
  XR_Far        = 1,
  XR_IO         = 2,
  XR_ReachCnt   = 3,
  XR_Null       = XR_ReachCnt
  };

class CTagRef;
class CXRefItem;
class CNodeXRefMngr;
class CGlblXRefMngr;

class DllImportExport CXRefStatus
  {
  public:
    CXRefStatus() {}
    virtual bool  IsXRefActive() const  = 0;//{ return true; }
  };

class DllImportExport CXRefValue
  {
  friend class CGCXRefItem;
  friend class FlwSlvBlk;
  friend class CFlwSolver;
  friend class CGlblXRefMngr;
  friend class CTagRefInfoItem;

  public:
    CXRefValue();

    double            m_dValue; //for numerical tags
    Strng             m_sValue; //for string tags
    bool              m_bChanged;
    bool              m_bTouched;
    bool              m_bWasTouched;

    CTraceItem        m_Chg;
    long              m_lForceCount;  // <0 continual force, =0 No Force, >0 Force for N iterations

#if WithIOChgMonitor     
    double            m_dValMem;
#endif

  };

class DllImportExport CTagRef
  {
  friend class GCInsMngr;
  friend class CGCXRefItem;
  friend class CXRefItem;
  friend class FlwSlvBlk;
  friend class CFlwSolver;
  friend class CGlblXRefMngr;
  friend class CTagRefInfoItem;
  friend class CTagRefsMapItem;
  friend class CNodeXRefMngr;
  friend class CTgFnIoVar;
  friend class FlwNode;

  
  public:
    CTagRef();
    virtual ~CTagRef();

    void          RemoveMe();

    bool          SameReference(CTagRef * Other);

    void          IncSets();
    void          IncResets();
    void          IncGets();
    void          ClearCnts();
    void          ClearTotal();
    int           SetCnt() const              { return m_nSets;     };
    int           ResetCnt() const            { return m_nResets;   };
    int           GetCnt() const              { return m_nGets;     };
    int           SetCntTotal() const         { return m_nSetsTot;  };
    int           GetCntTotal() const         { return m_nGetsTot;  };
    LPSTR         RefTag()                    { return m_sRefTag(); }
    bool          MustSet()                   { return m_bMustSet;  };
    bool          MustGet()                   { return m_bMustGet;  };
    void          SetNdActive(bool Act)       { m_bIsNdActive=Act; };
    bool          IsNdActive()                { return m_bIsNdActive; };

  protected:
    static long m_nAllocNo;
    long m_nMyAllocNo;
    
  public:
    static CGlblXRefMngr * gs_TagRefGlblFn;

  public:
    CTNode          * m_pOwner;

    Strng             m_sRefID;
    Strng             m_sRefTag;
    Strng             m_sRmtID;
    Strng             m_sRmtTag;
    Strng             m_sCnv;
    CCnvIndex         m_iCnvIndex;
    byte              m_iType;
    int               m_iIOId;
    int               m_iIOInx;

  protected:
    bool              m_bInUse;
    bool              m_bIsLcl;

    byte              m_nSets, m_nSetsTot;
    byte              m_nResets;
    byte              m_nGets, m_nGetsTot;
    bool              m_bMustSet;
    bool              m_bMustGet;
    bool              m_bIsFnTerm;
    bool              m_bIsNdActive;

    POSITION          m_posRefList;
    POSITION          m_posRefMap;

    DEFINE_SPARES(CTagRef);
  };

class DllImportExport CXRefItem : public CTagRef, public CSFGNodeInfo
  {
  friend class CGCXRefItem;
  friend class FlwSlvBlk;
  friend class CFlwSolver;
  friend class CGlblXRefMngr;
  friend class CTagRefInfoItem;
  friend class CNodeXRefMngr;
  public:
  public:
    CXRefItem(bool LockIt);
    CXRefItem(bool LockIt, bool MustSet, bool MustGet);
    virtual ~CXRefItem();

    void          RemoveMe();
    void          CopyToLcl();
    bool          SameReference(CXRefItem * Other);

    int           ResolveNearXRef(TaggedObject* pSrchRoot, char* pTag, CTNode* Owner);
    int           ResolveFarXRef(CExecObj* pExecObj, char* pTag, CTNode* Owner);
    bool          Validate();
    bool          GetNearXRefValue();
    bool          SetNearXRefValue();
    long          ForceCount() const          { return m_Value.m_lForceCount; };
    void          SetForceCount(long N)       { m_Value.m_lForceCount=N; };

    CTNode       *Owner() const               { return m_pOwner; };
    CTNode       *Other() const               { return m_pOther; };

    void          SetIsTear(bool On)          { m_bIsTear=On; };
    void          SetIsEvalSeqStart(bool On)  { m_bIsEvalSeqStart=On; };
    bool          IsTear() const              { return m_bIsTear; };
    bool          IsEvalSeqStart() const      { return m_bIsEvalSeqStart; };
                  
#ifdef _RELEASE
    void          Lock()   { EnterCriticalSection(&m_LockSect); };
    void          UnLock() { LeaveCriticalSection(&m_LockSect); };
#else
    void          Lock();   //{ EnterCriticalSection(&m_LockSect); };
    void          UnLock(); //{ LeaveCriticalSection(&m_LockSect); };
#endif
    // SFGNodeInfo Overides
    virtual       LPCTSTR SFGNodeTag(TaggedObject * pRefTo=NULL); 
    virtual       CSFGNodeInfo * SFGNodePtr()  { return this; };
    
    CTNode      * SrcNd() const;
    CTNode      * DstNd() const;

    double        TheValue() const   { return m_Value.m_dValue; };
    bool          HasChanged() const { return m_Value.m_bChanged;};
    bool          Touched() const    { return m_Value.m_bTouched;};
    void          ClearChanged()     { m_Value.m_bChanged=false; };

    double        GetXRefValue(bool DoCnts=false);
    void          SetXRefValue(double V, bool DoCnts=false);
    char*         GetXRefStrValue(bool DoCnts=false);
    void          SetXRefStrValue(char* p, bool DoCnts=false);

    double        GetHumanXRefValue() { return Cnvs[m_iCnvIndex]->Human(GetXRefValue(), m_sCnv()); }

  protected:
    static long m_nAllocNo;
    long m_nMyAllocNo;

  public:
    Strng             m_sSFGTag;
    CTagRef           m_Lcl;
    bool              m_bInUse;
    bool              m_bLclRqd,
                      m_bIsTear,
                      m_bIsEvalSeqStart,
                      m_bDynTag;
    byte              m_eReach;
    TagAccessBlk      m_TAB;
                                      // >0 force N times;
    CNodeXRefMngr   * m_pMngr;
    CTNode          * m_pOther;
    TaggedObject    * m_pSrchRoot;

    POSITION          m_posXRefList;
    CRITICAL_SECTION  m_LockSect;

    CXRefValue        m_Value;
    CXRefStatus     * m_pStatus; 

    DEFINE_SPARES(CXRefItem);
  };

//============================================================================

DllImportExport int TryTestTag(TaggedObject* pSrchRoot, char* pTag, bool TestParamStopped=true); //assumed number tag
DllImportExport int TryWriteTag(TaggedObject* pSrchRoot, char* pTag, double Value, bool TestParamStopped=true); //assumed number tag
DllImportExport int TryWriteTag(TaggedObject* pSrchRoot, char* pTag, char* pValue, bool TestParamStopped=true); //string or number tag

//============================================================================

typedef CSmartPtrReleaseOnly<CXRefItem> CSPAXRefItem;
class DllImportExport CXRefSPArray : public CArray <CSPAXRefItem, CSPAXRefItem&>
  {
  public:
    void Add(CXRefItem * X)
      {
      long i=GetSize();
      SetSize(i+1);
      ElementAt(i)=X;
      }
  };
class DllImportExport CXRefArray : public CArray <CXRefItem*, CXRefItem*>
  {
  public:
    void Add(CXRefItem * X)
      {
      long i=GetSize();
      SetSize(i+1);
      ElementAt(i)=X;
      }
  };
class DllImportExport CXRefPtrList : public CSCDList <CXRefItem*, CXRefItem*> { };
typedef CSCDPtrListIter<CXRefPtrList, CXRefItem*> CXRefPtrIter;

//============================================================================

typedef CSmartPtrReleaseOnly<CTagRef> CSPATagRef;
class DllImportExport CTagRefSPArray : public CArray <CSPATagRef, CSPATagRef&>
  {
  public:
    void Add(CTagRef* X)
      {
      long i=GetSize();
      SetSize(i+1);
      ElementAt(i)=X;
      }
  };
class DllImportExport CTagRefArray : public CArray <CTagRef*, CTagRef*>
  {
  public:
    void Add(CTagRef* X)
      {
      long i=GetSize();
      SetSize(i+1);
      ElementAt(i)=X;
      }
  };
class DllImportExport CTagRefPtrList : public CSCDList <CTagRef*, CTagRef*> { };

//============================================================================

class DllImportExport CTagRefsMapItem
  {
  public:
    CString        m_sLwrTag;
    CString        m_sTag;
    CTagRefPtrList m_pTagRefs;//, m_pLclXRefs;
    bool           m_bErrorDone;

    CTagRefsMapItem() 
      { 
      m_bErrorDone=false; 
      };
    CTagRefStatusWord GetStatusWord();
    DEFINE_SPARES(CTagRefsMapItem);
  };

//class DllImportExport CXRefsMap : public CMap <LPCTSTR, LPCTSTR, CTagRefsMapItem*, CTagRefsMapItem*> { };

//============================================================================

class DllImportExport CTagRefMap : public CMap <LPCTSTR, LPCTSTR, CTagRefsMapItem*, CTagRefsMapItem*> { };

//============================================================================
//Classes for Tag, Function and Extrenal references management...

class CTgFnIoVar;
class CFnMngr    ;

//--------------------------------------------------------------------------

class DllImportExport CXRefBuildResult
  {
  public:
    CXRefBuildResult()
      {
      m_dwReason  = 0;
      }
    CXRefBuildResult(LPCTSTR Tag, DWORD Reason)
      {
      m_sTag      = Tag;
      m_dwReason  = Reason;
      }
    CXRefBuildResult(const CXRefBuildResult & X)
      {
      m_sTag      = X.m_sTag;
      m_dwReason  = X.m_dwReason;
      }
    CXRefBuildResult & operator=(const CXRefBuildResult & X)
      {
      m_sTag      = X.m_sTag;
      m_dwReason  = X.m_dwReason;
      return *this;
      }
  public:
    CString     m_sTag;
    DWORD       m_dwReason;
  };

class DllImportExport CXRefBuildResults
  {
  public:
    CXRefBuildResults()
      {
      Clear();
      };
    void Clear() 
      { 
      m_nMissing=0;
      m_nBadFuncs=0;
      m_nBadXRefs=0;
      m_nBadCnvs=0;
      m_nBadDynTagsCurrent=0;
      m_nBadDynTagsAlways=0;
      
      m_BadTags.RemoveAll();
      };
    long TotalErrors() { return m_nMissing+m_nBadFuncs+m_nBadXRefs+m_nBadCnvs; }

    long      m_nMissing;
    long      m_nBadFuncs;
    long      m_nBadXRefs;
    long      m_nBadCnvs;
    long      m_nBadDynTagsCurrent;
    long      m_nBadDynTagsAlways;
    
    CList<CXRefBuildResult, CXRefBuildResult&> m_BadTags;  
  };

//--------------------------------------------------------------------------

enum eTagOrFunct
  {
  eIsBlank,
  eIsTag,
  eIsFunct,
  eIsCtrlIO
  };

//--------------------------------------------------------------------------

enum BXReturn 
  {
  BXR_NotRequired = -2,
  BXR_Blank       = -1,
  BXR_OK          =  0,
  BXR_FunctErr    =  1,
  BXR_TagErr      =  2,
  BXR_CnvErr      =  3
  };

enum BXOptions
  {
  BXO_Null     = 0x00,
  BXO_Funct    = 0x01,
  BXO_Blank    = 0x02,
  };

class DllImportExport CTgFnIoVar
  {
  friend class CNodeXRefMngr;
  public:
    CTgFnIoVar(BXOptions Options);
    ~CTgFnIoVar();

    CTgFnIoVar & operator=(CTgFnIoVar & X);

    int           SetVar(char* Tag, flag IgnoreCnv=false, char* pForceTag=NULL, Strng *pForTagOnly=NULL);
    int           DoDataXchg(DataChangeBlk & DCB, flag IgnoreCnv=false, char* pForceTag=NULL);
    BXReturn      UpdateXRef(CXRefStatus * pStatus, bool MustSet, bool MustGet, 
                            int& FunctNo, CNodeXRefMngr * pMERH, int IOId, 
                            LPCSTR LclTag, LPCSTR LclID, LPCSTR Desc, CXRefBuildResults &Results);
    bool          UnlinkXRefs();
    static BOOL   ChangeTag(Strng & SrcTag, char* pOldTag, char* pNewTag);
    static BOOL   ContainsTag(Strng & SrcTag, char* pTag);
    BOOL          DoChangeTag(pchar pOldTag, pchar pNewTag);
    BOOL          ContainsTag(pchar pTag);
    void          Clear();
    double        GetCtrlValue();
    void          SetCtrlValue(double D);


    CXRefItem   & XRef() { return * m_pXRef; };
    CTagRef     & TagRef() { return * m_pTagRef; };
    
    bool          GetValue(double &D, bool AsSIValue=false);
    bool          PutValue(double D, bool AsSIValue=false);

  //protected:
    Strng         sVar;
    int           iFunctNo;
    CXRefItem   * m_pXRef;
    CTagRef     * m_pTagRef;
    int           m_iIOId;
    int           m_iIOInx;
    GCVar*        pFunctVar;
    flag          m_bAllowFunct:1,
                  m_bAllowBlank:1,
                  bCnvOK:1;
    eXRefReach    m_eReach:8;
    eTagOrFunct   m_eWhat:8;
    CNodeXRefMngr *   m_pXRM;

    bool IsBlank()  { return m_eWhat==eIsBlank; };
    bool IsFunct()  { return m_eWhat==eIsFunct; };
    bool IsTag()    { return m_eWhat==eIsTag; };
    bool IsCtrlIO() { return m_eWhat==eIsCtrlIO; };


  };

//===========================================================================

class DllImportExport CNodeXRefMngr
  {
  friend class CTgFnIoVar;
  public:
    CNodeXRefMngr();
    virtual ~CNodeXRefMngr();


    bool            XRefsValid() { return m_bXRefsValid; };
    void            RequestUpdateXRefLists();
    bool            UpdateXRefListsReqd() { return !m_bXRefsValid || m_bUpdateXRefLists; };
    long            UpdateXRefLists(bool SetXRefListsChangedFlag);

    virtual void    UnlinkAllXRefs(); // virtual so that Gencon etc can remove vars etc.
    void            RemoveAllXRefs();
    CXRefItem     * AddNearXRef(CXRefItem * pXRef);
    CXRefItem     * AddFarXRef(CXRefItem  * pXRef);
    CTagRef       * AddIORef(CTagRef * pTgRef);

    BOOL            GetValidCnvTxt(CTgFnIoVar & Var, Strng & CnvTxt);

    // XRef Stuff --------------------
    void            GetNearXRefValues();
    void            SetNearXRefValues();

    void            AddXRef2Me(CXRefItem *pRef, bool IsSrc);
    void            RemoveXRef2Me(CXRefItem *pRef);

    long            NoXRefs2Me()      { return m_nXRefs2Me;};

    CFnMngr       & FnMngr();
    bool            FnMngrInUse()              { return m_bFnMngrInUse; }; 
    bool            FnMngrPresent()            { return m_pFnMngr!=NULL; }; 
    void            FnMngrClear();
    void            FnMngrRequired();
    void            FnMngrTryUpdateXRefLists(CXRefBuildResults & Results);
    void            FnMngrTryUnlinkAllXRefs();

    void            BumpNodeXRefsLocked(int Inc)  { m_NodeXRefsLockCount+=Inc; };
    bool            NodeXRefsLocked()             { return m_NodeXRefsLockCount>0; };

    virtual bool    TestXRefListActive()                                { return SetXRefListActive(false); }
    virtual bool    SetXRefListActive(bool Active);
    virtual bool    XRefListActive();
    virtual LPSTR   GetOwnerTag()                                       { return NULL;};
    virtual bool    GetOwnerDbgBrk()                                    { return false; };
    virtual int     UpdateXRefLists(CXRefBuildResults & Results)        { ASSERT_ALWAYS(FALSE, "UpdateXRefLists not Implemented", __FILE__, __LINE__); return 0; };
    virtual int     ResolveNearXRef(CXRefItem * XRef, flag MustBeParm)  { return FXR_NotFound; };
    virtual int     ResolveFarXRef(CXRefItem * XRef)                    { return FXR_NotFound; };
    virtual int     ResolveIORef(int IOId, CTagRef * &pRef)             { return FXR_NotFound; };
    virtual long    GetCIONo(long Id, Strng & ID, Strng & Tg, bool & IsIn, bool &IsOut)        { ID=Tg=""; IsIn=IsOut=false; return -1; };
    virtual double  GetCIOValue(long Id)                                { return dNAN;}; 
    virtual void    SetCIOValue(long Id, double D)                      { }; 
    virtual void    SetXRefCI(long Id, bool On, LPCTSTR Text=NULL)      { }; 
    virtual flag    QuerySubsReqd(CXM_DataRequest &Rqst, long &MdlIndex);
    virtual flag    QuerySubsAvail(CXM_DataRequest &Rqst, long &MdlIndex);
    virtual flag    ReadSubsData(CXM_ObjectData &ObjData, long &MdlIndex);
    virtual flag    WriteSubsData(CXM_ObjectData &ObjData, long &MdlIndex); 

  public:
    CXRefArray      m_XRefs2Me;  // actual count used is in m_nXRefs2Me
    CXRefArray      m_XRefsOwnedByMe[XR_ReachCnt];
    
    CTagRefArray    m_IOTagRefs; // Only those TagRefs associated with IO
    CTagRefArray    m_TagRefsOwnedByMe; // Only those TagRefs associated with IO
    bool            m_bXRefListsChanged;

  private:
    bool            m_bXRefListActive;
    bool            m_bXRefsValid;
    bool            m_bUpdateXRefLists;
    long            m_nXRefs2Me;
    long            m_NodeXRefsLockCount;

    bool            m_bFnMngrInUse;
    CFnMngr        *m_pFnMngr; //instruction/function manager
    
  public:
    CXRefBuildResults m_BuildResults;
  };


//===========================================================================

class DllImportExport CGlblXRefMngr
  {
  friend class CFlwSolver;

  public:
    CGlblXRefMngr();
    ~CGlblXRefMngr();

    virtual void    AddMe(CTagRef * pMe);
    virtual void    RemoveMe(CTagRef * pMe);
    virtual void    AddMe(CXRefItem * pMe);
    virtual void    RemoveMe(CXRefItem * pMe);
    virtual void    ClearRefCnts();

  public:
    CXRefPtrList      m_XRefsList;
    CTagRefPtrList    m_TagRefsList;
    CTagRefMap        m_TagRefsMap;
    CCriticalSection  m_LockSect;
  };

//===========================================================================

#undef DllImportExport

#endif

