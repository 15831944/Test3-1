//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLWNODE_H
#define  __FLWNODE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif
#ifndef __SP_QUEUE_H
  #include "sp_queue.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif
#ifndef __NDTREE_H
  #include "ndtree.h"
#endif
#ifndef __ERRORLOG_H
  #include "errorlog.h"
#endif
#ifndef __MTX_SOLV_H
  #include ".\sparseslv\sparseslv.h"
#endif
#ifndef __FlwEqn_H
  #include "FlwEqn.h"
#endif
#ifndef __EXECLIB_H
  #include "execlib.h"
#endif
#ifndef __EQNSOLVE_H
  #include "eqnsolve.h"
#endif
#ifndef __SCDTEMPL_H
  #include "scdtempl.h"
#endif
#ifndef __SFE_BASE_H
  #include "sfe_base.h"
#endif
#ifndef __ELECBASE_H
  #include "elecbase.h"
#endif
#ifndef __SIGFLW_H
  #include "sigflw.h"
#endif
#ifndef __POWER_H
  #include "power.h"
#endif
#include "dbgmngr.h"

#include <typeinfo.h>

#if defined(__FLWNODE_CPP) || defined(__FLWNETS_CPP)
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#if defined(__FLWNETS_H) || defined(__FLWSOLVE_H) || defined(__FSTHREAD_H)
#if !defined(WITH_GFB)
  #define WITH_GFB
#endif
#endif

// =========================================================================

#define KWIKCONNECT         01
#define DOPBSMOOTHING       0
#define DOPBHSMOOTHING      0
#define DOPB_FILT_ALPHAS    0
#define DOPB_FILT_RESULTS   0
#define NDSTOPWATCHES       0 /*(1 || WITHTIMERS)*/

#define dbgTrackQmModes     0

#define dbgFlwNode          (1 || WITHDEBUG)

#if dbgFlwNode
extern CDbgMngr dbgTopologyChg;
#define DBGTOPOCHG(a) if (dbgTopologyChg()) dbgpln("Topo Change : %s",a);
#define DBGTOPOCHGFB(a,b) if (dbgTopologyChg()) dbgpln("Topo Change : %s fb:[%4i]",a,b);
#else
#define DBGTOPOCHG(a)
#define DBGTOPOCHGFB(a,b)
#endif

// =========================================================================

extern DllImportExport double V2LLimit;

inline int    OtherEnd(int Id)   { return ((Id+1)%2); };

// =========================================================================

inline double Head2Press(double H, double Rho)
  {
  double SG = Rho/1000.0;
  return H * 9.80707 * SG;
  }
inline double Press2Head(double P, double Rho)
  {
  double SG = Rho/1000.0;
  return P/ (9.80707 * SG);
  }

// =========================================================================
// forwards
class FlwBlk;
class CFlange;
class CIORec;
class IOAreaRec;
class CJoinRec;
class CJConnectRec;

_FWDDEF(FlwNode);

class CLinkRec;

class CTraceItem;
class CTraceArray;
class CTraceList;

//===========================================================================
//
//
//
//===========================================================================

const long iNotInEvalOrder = -999;//LONG_MIN;

class CJoinCluster
  {
  public:
    CJoinCluster()
      {
      m_iCluster        = -1;
      m_iJoinMask       = 0;
      m_iProcOrder      = iNotInEvalOrder;
      m_nFlwIOs         = 0;
      m_nFlwIOsIn       = 0;
      m_nFlwIOsOut      = 0;
      m_nFlwIOsInReqd   = 0;
      m_nFlwIOsInDone   = 0;
      };
    CJoinCluster(long Cluster, long JoinMask)
      {
      m_iCluster        = Cluster;
      m_iJoinMask       = JoinMask;
      m_iProcOrder      = iNotInEvalOrder;
      m_nFlwIOs         = 0;
      m_nFlwIOsIn       = 0;
      m_nFlwIOsOut      = 0;
      m_nFlwIOsInReqd   = 0;
      m_nFlwIOsInDone   = 0;
      };

    long            NoIOs() { return m_nFlwIOs; };
    long            IONo(int i) { if (i<m_nFlwIOs) return m_IOs[i]; return -1; };
  //protected:

    long            m_iCluster;
    long            m_iJoinMask;
    long            m_iProcOrder;

    long            m_nFlwIOs;
    long            m_nFlwIOsIn;       // No of FlwIOs in to each node
    long            m_nFlwIOsOut;      // No of FlwIOs out of each node
    long            m_nFlwIOsInReqd;
    long            m_nFlwIOsInDone;

    CIArray         m_IOs;
    CIArray         m_FlwDependentIOs;
  };

class CJoinClusterArray : public CArray <CJoinCluster, CJoinCluster&> {};
//typedef CSCDList <CFlwEPTie*, CFlwEPTie*> CJoinPtrList;
//typedef CSCDPtrListIter<CJoinPtrList, CJoinRec*> JoinIter;

//===========================================================================
//
//
//
//===========================================================================

class CNodeEvalIndex
  {
  public:
    CNodeEvalIndex()
      {
      m_pNd         = NULL;
      m_iCluster    = 0;
      m_iJoinMask   = -1;
      m_bInSequence = false;
      };
    CNodeEvalIndex(FlwNode * pNd, long Cluster, long Joins, bool InSequence)
      {
      m_pNd         = pNd;
      m_iCluster    = Cluster;
      m_iJoinMask   = Joins;
      m_bInSequence = InSequence;
      };

    FlwNode      & getNd();
    CJoinCluster & getCluster();
    long           getJoinMask() { return m_iJoinMask; };

    _declspec(property(get=getNd))         FlwNode      & Nd;
    _declspec(property(get=getCluster))    CJoinCluster & Cluster;
    _declspec(property(get=getJoinMask))   long         & JoinMask;


  //protected:
    FlwNode   * m_pNd;
    long        m_iCluster;
    long        m_iJoinMask;
    bool        m_bInSequence;
  };

template<class CLIST, class ARG_TYPE>
class CSCDListXIter : public CObject
{
protected:
  CLIST &List;
  POSITION pos;
  bool bFwd, bOK;

public:
// Construction
	CSCDListXIter(CLIST &Lst) : List(Lst) {pos=NULL;};

  ARG_TYPE First() { bFwd=1; pos = List.GetHeadPosition(); return Next(); };
  ARG_TYPE Next()  { if (pos) { bOK=1; return List.GetNext(pos); } bOK=0; return ARG_TYPE(); };
  ARG_TYPE Last()  { bFwd=0; pos = List.GetTailPosition(); return Prev(); };
  ARG_TYPE Prev()  { if (pos) { bOK=1; return List.GetPrev(pos); } bOK=0; return ARG_TYPE(); };
  ARG_TYPE MakeCurrent(POSITION Pos) { pos=Pos; return (bFwd ? Next() : Prev()); };
  bool     ItemValid() { return bOK; };
};


typedef CSCDList <FlwNode*, FlwNode*>                     CFlwNodeList;
typedef CSCDPtrListIter<CFlwNodeList, FlwNode*>           CFlwNodeIter;
typedef CSCDPtrListIterWithPos<CFlwNodeList, FlwNode*>    CFlwNodeIterWithPos;

typedef CSCDList <CNodeEvalIndex, CNodeEvalIndex&>        CNodeEvalIndexList;
typedef CSCDListXIter<CNodeEvalIndexList, CNodeEvalIndex> CNodeEvalIndexIter;

// ===========================================================================
//
//
//
//===========================================================================

const byte TOD_OK           = 0;
const byte TOD_NoTag        = 1;
const byte TOD_BadTag       = 2;
const byte TOD_Inactive     = 3;
const byte TOD_BadConn      = 4;
const byte TOD_Disconected  = 5;

template<class TYPE>
class CTODirectPtr
  {
  public:
    CTODirectPtr()                    { m_pNd=NULL; m_Ptr=NULL; m_State=TOD_NoTag; };
    ~CTODirectPtr()                   { };

    operator TYPE*()                  { return m_Ptr;   };
    TYPE* operator->()                { return m_Ptr;   };

    flag Connected()                  { return m_Ptr!=NULL; }
    byte ConnectState()               { return m_State; }

    flag Connect(FlwNode * pNd, int ReqdIOId, LPCTSTR NodeTypeName=NULL)
      {
      m_State=TOD_OK;
      ASSERT(pNd!=NULL);
      m_pNd=pNd;
      m_Ptr=NULL;
      if (m_sTag.GetLength()>0)
        {
        TaggedObject *pRoot = (TaggedObject*)m_pNd->InsertRoot(NULL);
        char TheTag[1024];
        strcpy(TheTag, m_sTag());
        int ObjTagLen=0;
        TaggedObject *p=(TaggedObject *)pRoot->FindObjTag(TheTag, true, ObjTagLen, 0);
        if (p)
          {
          m_Ptr=dynamic_cast<TYPE*>(p);
          if (m_Ptr==NULL)
            {
            pTaggedObject p1=p->pAttachments;
            while (p1)
              {
              Strng S=p1->PrefixTag();
              if (_stricmp(p1->PrefixTag().Str(), &TheTag[ObjTagLen+1])==0)
                break;
              p1=p1->pNxtAttachment;
              }
            if (p1)
              p=p1;
            m_Ptr=dynamic_cast<TYPE*>(p);
            }
          if (!m_Ptr->GetActive())
            m_Ptr=NULL;

          if (ReqdIOId>=0)
            {
            FlwNode * pNd=dynamic_cast<FlwNode*>(p);
            if (pNd && pNd->IOAreaWithIOId(ReqdIOId)<0)
              {
              LogError(m_pNd->FullObjTag(), 0, "%s %s missing Reqd IO Connection",NodeTypeName?NodeTypeName:"Direct Tag",m_sTag());
              m_State=TOD_BadConn;
              return NULL;
              }
            }
          m_State=m_Ptr?TOD_OK:TOD_Inactive;
          return m_Ptr!=NULL;
          }
        LogError(m_pNd->FullObjTag(), 0, "%s %s not Found",NodeTypeName?NodeTypeName:"Direct Tag",m_sTag());
        m_State=TOD_BadTag;
        }
      else
        m_State=TOD_NoTag;
      //if (m_Ptr && !m_Ptr->GetActive())
      //  m_Ptr=NULL;
      return m_Ptr!=NULL;
      };

    void DisConnect()
      {
      m_State=TOD_Disconected;
      m_Ptr=NULL;
      };

  public:
    Strng         m_sTag;
  protected:
    FlwNode      *m_pNd;
    TYPE         *m_Ptr;
    byte          m_State;
  };

// =========================================================================

class DllImportExport CAreaSum
  {
  friend class MN_Area;
  public:
    enum eSumAction { SumNone, SumMass, SumArray };
    static eSumAction  sm_eAction;

    CAreaSum();
    ~CAreaSum();

    void          ZeroFlows();
    void          ZeroTotals();
    void          ZeroTotalIncs();
    void          Set(CSysVector & V, double totHz);
    void          Set(CAreaSum & C);
    void          Set(SpModel & C);
    void          Set(SpQueue & Q);
    void          Add(CSysVector & V, double totHz);
    void          Add(CAreaSum & C);
    void          Add(SpModel & C);
    void          Add(SpQueue & Q);
    void          Integrate(double TimeInc);

    bool          getDoArray() const            { return sm_eAction==SumArray; }
    double        getFlowHz() const             { return m_FlowHz; }
    void          putFlowHz(double H)           { m_FlowHz=H; }
    double        getFlowMass() const           { return m_FlowMass; }
    void          putFlowMass(double M)         { m_FlowMass=M; }
    CSysVector  & getFlowArray()                { if (DoArray && !m_pFlowArray) m_pFlowArray=new SpMArray; return *m_pFlowArray; }
    double        getTotalMass() const          { return m_TotalMass; }
    void          putTotalMass(double M)        { m_TotalMass=M; }
    double        getTotalMassInc() const       { return m_TotalMassInc; }
    void          putTotalMassInc(double M)     { m_TotalMassInc=M; }
    double        getTotalHz() const            { return m_TotalHz; }
    void          putTotalHz(double H)          { m_TotalHz=H; }
    double        getTotalHzInc() const         { return m_TotalHzInc; }
    void          putTotalHzInc(double H)       { m_TotalHzInc=H; }
    CSysVector  & getTotalArray()               { if (DoArray && !m_pTotalArray) m_pTotalArray=new SpMArray; return *m_pTotalArray; }

    _declspec(property(get=getDoArray))                             bool          DoArray;
    _declspec(property(get=getFlowHz,put=putFlowHz))                double        FlowHz;
    _declspec(property(get=getFlowMass,put=putFlowMass))            double        FlowMass;
    _declspec(property(get=getFlowArray))                           CSysVector  & FlowArray;
    _declspec(property(get=getTotalMass,put=putTotalMass))          double        TotalMass;
    _declspec(property(get=getTotalMassInc,put=putTotalMassInc))    double        TotalMassInc;
    _declspec(property(get=getTotalHz,put=putTotalHz))              double        TotalHz;
    _declspec(property(get=getTotalHzInc,put=putTotalHzInc))        double        TotalHzInc;
    _declspec(property(get=getTotalArray))                          CSysVector  & TotalArray;

  protected:
    double          m_FlowHz;
    double          m_FlowMass;
    SpMArray      * m_pFlowArray;

    double          m_TotalMass;
    double          m_TotalMassInc;
    double          m_TotalHz;
    double          m_TotalHzInc;
    SpMArray      * m_pTotalArray;
  };

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CDirectAccCalc
  {
  public:
    CDirectAccCalc(FlwNode * pNd, CDirectFlwIO & In, CDirectFlwIO & Out);
    ~CDirectAccCalc();
    void Start(int Instance, SpContainer & Cn);
    void Start(int Instance, SpQueue & Q);
    void Start(int Instance, CSysVector & V, double totHz);
    void Complete(int Instance, SpContainer & Cn);
    void Complete(int Instance, SpQueue & Q);
    void Complete(int Instance, CSysVector & V, double totHz);

  protected:
    FlwNode       & m_Nd;
    CDirectFlwIO  & m_In;
    CDirectFlwIO  & m_Out;
    int             m_LastInstance;

    void        DoComplete(int Instance);
  };

class CReactionBase; // forward
class DllImportExport CExternAuditCalc : public CAreaSum
  {
  public:
    CExternAuditCalc(FlwNode * pNd);
    ~CExternAuditCalc();
    void Complete(int Instance, CReactionBase * pRB);

    bool            InUse()         { return m_LastInstance>=0; };
    //double          Mass()          { return m_Mass; };
    //double          Heat()          { return m_Heat; };

  protected:
    FlwNode       & m_Nd;
    int             m_LastInstance;
    //double          m_Mass;
    //double          m_Heat;

    //void        DoComplete(int Instance, double TimeInc, CReactionBase * pRB);
  };

//---------------------------------------------------------------------------

XID xidRmtTag           = MdlBsXID(5203);
XID xidRmtTagConnd      = MdlBsXID(5204);
XID xidDrctQm           = MdlBsXID(5205);
XID xidDrctEnable       = MdlBsXID(5206);
XID xidConnectState     = MdlBsXID(5207);

enum eDirectIO
  {
    eDIO_Spill, eDIO_Vent, eDIO_Leak,                   // Real IOs
    eDIO_Makeup, eDIO_Bleed,
    eDIO_Src, eDIO_Snk, eDIO_Accum, eDIO_Deplete        // Direct
  };

enum eDIOConnectSelects
  {
  eDIOSlct_None, eDIOSlct_All, eDIOSlct_Selected
  };

extern DllImportExport Strng gs_DirectFlwIOTagDefault;

//---------------------------------------------------------------------------

const DWORD DFIO_ShowQm = 0x00000001;

class DllImportExport CDirectFlwIO
  {
  friend class FlwNode;
  public:

    CDirectFlwIO(eDirectIO eType, FlwNode* Nd, flag ShowUsr, flag SysEnable, LPCTSTR Tag, int LclDIOId, int RmtDIOId, LPCTSTR RmtTypeName=NULL, LPCTSTR RmtTagDef=NULL);
    CDirectFlwIO(eDirectIO eType, FlwNode* Nd, flag ShowUsr, flag SysEnable, LPCTSTR Tag, LPCTSTR RmtTypeName=NULL, LPCTSTR RmtTagDef=NULL);
    ~CDirectFlwIO();

    void          SetLclIOId(int LclIOId);
    void          Initialise(eDirectIO eType, FlwNode* Nd);
    void          InitialiseIO(flag ShowUsr, flag SysEnable, LPCTSTR Tag, int LclDIOId, int RmtDIOId, int LclIOId);

    void          Add_OnOff(DataDefnBlk &DDB, dword Flags, dword UserInfo);
    void          BuildDataDefn(DataDefnBlk & DDB, LPCTSTR TagPrefix, LPCTSTR Tag, DDBPages PgOpt, dword UserInfo, DWORD Options);
    flag          DataXchg(DataChangeBlk & DCB);
    flag          ValidateData(ValidateDataBlk & VDB);

    void          ManageNdDirectIOs(bool AddIt);

    void          MakeRmtTag();
    flag          GlobalConnect();
    flag          GlobalDisConnect();
    void          GlobalConnectFinal();
    void          PostConnect(int IONo);
    void          PreDisConnect(int IONo);

    flag          EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    flag          EvalFlowEquationsReturn() { return m_EvalFlowEquationsReturn; };
    void          EvalDiscrete();

    void          EvalProducts(SpConduit & Cd, bool ScaleCd);

    void          SetTarget(LPCTSTR T);
    LPCTSTR       Target();
    LPCTSTR       TargetStr(flag ForFiling);
    bool          Flowing()                 { return Cd.QMass(som_ALL)>1.0e-12; };
    int           MyConnectedIO()           { return m_iMyConnectedIO; };
    int           LclDIOId()                { return m_iLclDIOId; };
    int           RmtDIOId()                { return m_iRmtDIOId; };
    int           LclIOId()                 { return m_iLclIOId; };

    void          Add(CSysVector & V, double Hz);
    void          AddF(SpConduit & Cd, PhMask PhaseM=som_ALL, double Fraction=1.0);
    void          AddM(SpConduit & Cd, PhMask PhaseM, double Qm);
    void          Set(CSysVector & V, double Hz);
    void          SetF(SpConduit & Cd, PhMask PhaseM=som_ALL, double Fraction=1.0);
    void          SetM(SpConduit & Cd, PhMask PhaseM, double Qm);

    void          putSysEnable(flag On)     { m_bSysEnable = On; };
    flag          getSysEnable()            { return m_bSysEnable; };
    void          putUsrEnable(flag On)     { m_bUsrEnable = On; ManageNdDirectIOs(Enabled!=0); };
    flag          getUsrEnable()            { return m_bUsrEnable; };
    flag          getEnabled()              { return (m_bSysEnable || m_bUsrEnable) && !m_bMLnkConnd; };
    flag          getConnected()            { return m_bConnected; };
    flag          getTagDeferred()          { m_sRmtTag.LRTrim(); return m_sRmtTag.GetLength()==0; };
    flag          getConnectError();
    FlwNode     & getNd()                   { return *m_pNd; };
    eDirectIO     getType()                 { return m_eType; };
    LPCTSTR       getTag()                  { return (LPCTSTR)m_sTag(); };
    void          putTag(LPCTSTR Tg)        { m_sTag=Tg; };
    LPCTSTR       getAreaTag()              { MakeRmtTag(); return (LPCTSTR)m_pRmt.m_sTag(); };
    void          putAreaTag(LPCTSTR ATg)   { m_sRmtTag=ATg; };
    FlwNode     & getArea()                 { return *m_pRmt; };
    CAreaSum    & getSum()                  { return *m_pSum; };
    int           getNdSignImplied()        { return m_NdSignImp; };

    flag          getHasConduit()           { return m_eType <= eDIO_Bleed; };
    SpConduit   & getConduit();//              { return *m_pFlng->Conduit(false); };
    CFlange     & getFlange()               { return *m_pFlng; };

    _declspec(property(get=getEnabled))                     flag          Enabled;
    _declspec(property(get=getSysEnable,put=putSysEnable))  flag          SysEnable;
    _declspec(property(get=getUsrEnable,put=putUsrEnable))  flag          UsrEnable;
    _declspec(property(get=getConnected))                   flag          Connected;
    _declspec(property(get=getConnectError))                flag          ConnectError;
    _declspec(property(get=getTagDeferred))                 flag          TagDeferred;
    _declspec(property(get=getNd))                          FlwNode     & Nd;
    _declspec(property(get=getType))                        eDirectIO     Type;
    _declspec(property(get=getTag,put=putTag))              LPCTSTR       Tag;
    _declspec(property(get=getAreaTag,put=putAreaTag))      LPCTSTR       AreaTag;
    _declspec(property(get=getArea))                        FlwNode     & Area;
    _declspec(property(get=getSum))                         CAreaSum    & Sum;
    _declspec(property(get=getNdSignImplied))               int           NdSignImplied;

    _declspec(property(get=getHasConduit))                  flag          HasConduit;
    _declspec(property(get=getFlange))                      CFlange     & Flange;
    _declspec(property(get=getConduit))                     SpConduit   & Cd;

  protected:

    Strng         m_sRmtTypeName;

    eDirectIO     m_eType;
    FlwNode     * m_pNd;
    flag          m_bSysEnable;
    flag          m_bUsrEnable;
    flag          m_bMLnkConnd;
    flag          m_bShowUsr;
    bool          m_bIsXfer;
    Strng         m_sTag;
    Strng         m_sRmtTag;
    Strng         m_sRmtTmp;
    Strng         m_sRmtTagDef;
    CFlwEqnPtr     m_FEP;
    flag          m_bConnected;
    int           m_iMyConnectedIO;
    double        m_Opening;
    int           m_iLclDIOId;
    int           m_iRmtDIOId;
    int           m_iLclIOId;
    int           m_NdSignImp;

    CFlange     * m_pFlng;
    CAreaSum    * m_pSum;
    bool          m_bDeleteSum;
    flag          m_EvalFlowEquationsReturn; 

    CTODirectPtr<FlwNode>  m_pRmt;

    POSITION      m_RmtListPos;

  public:
    static eDIOConnectSelects sm_ConnectVentSpills;
    static eDIOConnectSelects sm_ConnectLeaks;
    static eDIOConnectSelects sm_ConnectAudit;
  };

// =========================================================================
//
//
//
// =========================================================================

#if DOPBSMOOTHING

const int CMSOp_None    = 0;
const int CMSOp_SrcMult = 1;
const int CMSOp_IODiff  = 2;
const int CMSOp_RmtMult = 3;

class CMSIO;
class CMsAGData;
class CMSIndex;

class DllImportExport CMSIndexInit
  {
  public:
    int     m_iLnkIndex;
    int     m_iSpIndex;

    CMSIndexInit() { Init();}
    void    Init() { m_iLnkIndex=0; m_iSpIndex=1; };
  };

class DllImportExport CMSIndex
  {
  public:
    flag            m_fMustExist;
    flag            m_fMerged;
  protected:
    CMsAGData       *m_pSrcAGData;
    int             m_iIndex;
    CIArray         m_iSpIndex;

  public:
    CMSIndex();
    void            Clear();
    void            CheckIndex(CMSIndexInit &IndexInit);

    CMsAGData       *SrcAGData() { return m_pSrcAGData; };
    void            SetSrc(CMsAGData * SrcAGData) { m_pSrcAGData=SrcAGData; };
    flag            MustExist() { return m_fMustExist;};
    void            SetMustExist(flag On) { m_fMustExist=On;};
    int            &Index() { return m_iIndex; };
    CIArray        &SpIndex() { return m_iSpIndex; };

  };

class DllImportExport CMSIO
  {
  protected:
    int             m_iIONo;
    FlwNode       * m_pNd;
  public:
    int             IONo() { return m_iIONo; };
    void            SetSrc(FlwNode *pNd, int IONo);
    void            SetDst(FlwNode *pNd, int IONo, CMsAGData * AGData=NULL);

    void            CheckIndex(CMSIndexInit &Index);
    int             SpIndex(int SpNo);
    void            SetSpIndex(int SpNo, int Inx);
    void            SetSpIndexRqd(int SpNo, flag Mrgd);
    CMSIndex      * MSIndex();
    FlwNode       * Nd();
    FlwNode       * Nd_Rmt();
  };
typedef CArray <CMSIO, CMSIO&> CAMSIOs;

inline double ChkMeaningFull(double x) { return fabs(x)>1.0e-12 ? x : 0.0; };

class DllImportExport CMSFlags : public CArray <flag, flag&>
  {
  public:
    void Init()
      {
      SetSize(SDB.No());
      for (int i=0; i<SDB.No(); i++)
        m_pData[i]=false;
      };
    void Clear()   { SetSize(0); };
    flag IsValid() { return GetSize()>0; };
    CMSFlags& CMSFlags::operator=(const CMSFlags &v)
      {
      SetSize(SDB.No());
      for (long i=0; i<GetSize(); i++)
        m_pData[i] = v[i];
      return *this;
      }
    CMSFlags& CMSFlags::operator|=(const CMSFlags &v)
      {
      SetSize(SDB.No());
      for (long i=0; i<GetSize(); i++)
        m_pData[i] = m_pData[i] || v[i];
      return *this;
      }
    CMSFlags& CMSFlags::operator&=(const CMSFlags &v)
      {
      SetSize(SDB.No());
      for (long i=0; i<GetSize(); i++)
        m_pData[i] = m_pData[i] && v[i];
      return *this;
      }
  };

class CMSVector;
class CMSMatrix;

class DllImportExport CMSVector : public CDVector//SpVector
  {
  public:
    void Init();
    void Clear();
    flag IsValid();
    void Copy(SpMArray & M, int Sign);
    void Add(SpMArray & M, int Sign);
    void Mult(CMSMatrix & M, CMSVector & V);
    void Mult(CMSVector & V);
    void Add(CMSVector & V);
    void ChkMeaningFull();
  };

class DllImportExport CMSMatrix : public CDMatrix
  {
  public:
    void Init(flag AsIdentity);
    void Clear();
    flag IsValid();
    void MultRows(CMSVector & V);
    void MultRows(CMSVector & V, CMSMatrix & M);
    void Mult(CMSMatrix & A, CMSMatrix & B);
    void ChkMeaningFull();
  };

inline void CMSVector::Init()
  {
  SetSize(SDB.No());
  SetAll(0.0);
  };
inline void CMSVector::Clear()   { SetSize(0); };
inline flag CMSVector::IsValid()   { return GetLen()>0; };
inline void CMSVector::Copy(SpMArray & M, int Sign)
  {
  SetSize(SDB.No());
  for (int s=0; s<SDB.No(); s++)
    m_d[s]=Sign*M[s];
  };
inline void CMSVector::Add(SpMArray & M, int Sign)
  {
  SetSize(SDB.No());
  for (int s=0; s<SDB.No(); s++)
    m_d[s]+=Sign*M[s];
  };
inline void CMSVector::Mult(CMSMatrix & M, CMSVector & V)
  {
  SetSize(SDB.No());
  for (int s=0; s<SDB.No(); s++)
    {
    m_d[s]=0;
    for (int s1=0; s1<SDB.No(); s1++)
      m_d[s]+=M[s][s1]*V[s1];
    }
  };
inline void CMSVector::Mult(CMSVector & V)
  {
  SetSize(SDB.No());
  for (int s=0; s<SDB.No(); s++)
    m_d[s]*=V[s];
  };
inline void CMSVector::Add(CMSVector & V)
  {
  SetSize(SDB.No());
  for (int s=0; s<SDB.No(); s++)
    m_d[s]+=V[s];
  };
inline void CMSVector::ChkMeaningFull()
  {
  for (int s=0; s<SDB.No(); s++)
    m_d[s]=::ChkMeaningFull(m_d[s]);
  }

inline void CMSMatrix::Init(flag AsIdentity)
  {
  SetSize(SDB.No(), SDB.No());
  if (AsIdentity)
    MakeIntoIdentity();
  else
    SetAll(0.0);
  };
inline void CMSMatrix::Clear()  { SetSize(0,0); };
inline flag CMSMatrix::IsValid()  { return GetRows()>0; };
inline void CMSMatrix::MultRows(CMSVector & V)
  {
  SetSize(SDB.No(), SDB.No());
  for (int s=0; s<SDB.No(); s++)
    {
    for (int s1=0; s1<SDB.No(); s1++)
    m_d[s][s1]*=V[s];
    }
  };
inline void CMSMatrix::MultRows(CMSVector & V, CMSMatrix & M)
  {
  SetSize(SDB.No(), SDB.No());
  for (int s=0; s<SDB.No(); s++)
    {
    for (int s1=0; s1<SDB.No(); s1++)
    m_d[s][s1]=V[s]*M[s][s1];
    }
  };
inline void CMSMatrix::Mult(CMSMatrix & A, CMSMatrix & B)
  {
  SetSize(SDB.No(), SDB.No());
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      {
      m_d[i][j]=0;
      for (long k=0; k<m_Cols; k++)
        m_d[i][j] += (A[i][k] * B[k][j]);
      }
  }
inline void CMSMatrix::ChkMeaningFull()
  {
  for (int s=0; s<SDB.No(); s++)
    for (int s1=0; s1<SDB.No(); s1++)
      m_d[s][s1]=::ChkMeaningFull(m_d[s][s1]);
  };

class DllImportExport CMSDataBlk
  {
  public:
    CMSFlags         m_SpReqd;
    CMSVector        m_MAlpha;
    CMSMatrix        m_MBeta;
    CMSMatrix        m_MGamma;
    CMSVector        m_MPhi;
    CMSVector        m_MConst;
    double           m_RmtScl;
  };
typedef CArray <CMSDataBlk, CMSDataBlk&> CAMSData;

class DllImportExport CMsAGData
  {
  public:
    flag            m_fProdSpRefd;
    //flag            m_fActive;
    flag            m_fMerged;
    flag            m_fSwitched;
    flag            m_fMergeSrc;
    flag            m_fMergeDst;
    flag            m_fMergeDstIfUnity;
    flag            m_fUnity;

    int             m_Op;
    //CFlwNodeArray    m_pNd;
    FlwNode       * m_pNd;
    int             m_iJoinNo;
    CAMSIOs         m_SrcIOs;
    CMSIO           m_DstIO;
    CAMSIOs         m_RmtIOs;
    //CAMSIOs         m_MInIOs;
    CAMSIOs         m_MOutIOs;

#if DOPB_FILT_ALPHAS
    CAMSData        m_Est;
    CMSDataBlk         m_Avg;
#else
    CMSDataBlk         m_Avg;
#endif
    //SpVector        m_MAlpha;
    //CDMatrix        m_MBeta;
    //CDMatrix        m_MGamma;
    //CDArray         m_MPhi;
    //SpVector        m_MConst;
    //double          m_RmtScl;

    CMsAGData      * m_pMrgAGData;

    CMsAGData();
    ~CMsAGData();

    void            Clear();
    flag            Merge(CMsAGData * pDstAGData, CMsAGData * & pMrgdAG);
    void            MarkMsIndices();
    void            Dump(char * Tag, int Jn);
    void            SmoothEsts();
    void            AddMsEqns(SparseSolver & SS, int & Eqn);

    void            SetUnity(flag F) { m_fUnity=F; };
    flag            TestForUnity();

    void            Copy(CMsAGData * pSrc);
    void            DoMerge(CMsAGData * pMrgdAG, flag DstUnity);

    void            DoMSDumpGamma(CMsAGData * pOther);

  };

class DllImportExport CAMsAGDatas : public CArray <CMsAGData, CMsAGData&> {};

class DllImportExport CMSJoin
  {
  public:
    //SpVector   MInC;
    //SpVector   MInP;
    //SpVector   MOutC;
    //SpVector   MOutP;
    flag          m_fDataIsValid;
    CMSVector     m_ConstBG;
    CMSVector     m_ConstAG;
    CMSMatrix     m_PropBG;
    CMSVector     m_PropAG;
    CMSMatrix     m_Gamma;
    //SpVector   MOutC;
    //SpVector   MOutP;
    CMSJoin()
      {
      m_fDataIsValid=false;
      //m_ConstInBG.Init();
      //m_ConstOutAG.Init();
      //m_PropInBG.Init();
      //m_PropOutAG.Init();
      //m_Gamma.Init(true);
      }
    void EstimateData(CMSVector & MTotI, CMSVector & MTotO);

  };
class DllImportExport CAMSJoins : public CArray <CMSJoin, CMSJoin&> {};
#endif

//============================================================================

_FWDDEF(CMacroMdlBase);
DEFINE_TAGOBJ(CMacroMdl);

//============================================================================

inline flag IsImpSolution() { return FALSE; };//(SolutionType()==ImpSoln); };

//============================================================================
// Record to Handle Choked flow in pipes

_FWDDEF(CChokeAtRec)
class CChokeAtRec
  {
  public:
    FlwBlk*        m_At;
    double         m_Qm;
    double         m_Dp;

    CChokeAtRec() { Clear(); };
    void Clear()
      {
      m_At=NULL;
      m_Qm=0.0;
      m_Dp=0.0;
      }
  };

_FWDDEF(CChokeInfoRec)
class CChokeInfoRec
  {
  public:
    FlwBlk       * m_At;
    double         m_QmHi;
    double         m_DpHi;
    double         m_QmLo;
    double         m_DpLo;
    double         m_PX;

    CChokeInfoRec() { Clear(); };
    void Clear()
      {
      m_At=NULL;
      m_QmHi=0;
      m_DpHi=0;
      m_QmLo=0;
      m_DpLo=0;
      m_PX=0;
      };
  };

//============================================================================
// Class to Handle Unidirectional Flow in pipes

// Allowable Directions
const byte UDFD_Both    = 0;
const byte UDFD_Fwd     = 1;
const byte UDFD_Rev     = 2;
const byte UDFD_BothDZ  = 3;

// Required Actions
const byte UDFA_None    = 0;
const byte UDFA_Open    = 1;
const byte UDFA_Close   = 2;
const byte UDFA_OpenF   = 3;
const byte UDFA_CloseF  = 4;

class DllImportExport UDFRec
  {
  public:
    static int     MaxActionCount;
    static double  RestrictMult;

    double         OpenP,       // UnidirectionFlow Opening Pressure
                   CloseP,      // UnidirectionFlow Close   Pressure
                   Restrict;    // UnidirectionFlow Restriction

    byte           Action, DirnAllowed, IterCount;
    sint           ActionCount;
    flag           fInList;

    UDFRec()
      {
      OpenP=1;
      CloseP=0;
      Restrict=0;

      Action=UDFA_None;
      DirnAllowed=UDFD_Both;
      ActionCount=0;
      IterCount=0;
      fInList=false;
      }

    byte SetAction(byte Act)
      {
      switch (Act)
        {
        case UDFA_Open:
        case UDFA_OpenF:
          Action=(ActionCount>0) ? Act : UDFA_None;
          break;
        case UDFA_Close:
        case UDFA_CloseF:
          Action=(ActionCount<MaxActionCount) ? Act : UDFA_None;
          break;
        default:
          Action=Act;
          break;
        }
      return Action;
      };
    char * ActStr()
      {
      switch (Action)
        {
        case UDFA_Open:   return "Open  ";
        case UDFA_OpenF:  return "OpenF ";
        case UDFA_Close:  return "Close ";
        case UDFA_CloseF: return "CloseF";
        default:          return "-     ";
        }
      };
    char * DirnStr()
      {
      switch (DirnAllowed)
        {
        case UDFD_Both  : return "Both  ";
        case UDFD_Fwd   : return "Fwd   ";
        case UDFD_Rev   : return "Rev   ";
        case UDFD_BothDZ: return "BothDZ";
        default:          return "-     ";
        }
      };
  };

//============================================================================

typedef dword DispState;

//============================================================================

_FWDDEF(QmRatioRec)
class QmRatioRec
  {
  DEFINE_MEMSTATS(QmRatioRec)
  public:
    double         Mult;
    FlwBlk*        MFb;
  };

_FWDDEF(GQmRatioRec)
class GQmRatioRec
  {
  DEFINE_MEMSTATS(GQmRatioRec)
  public:
    double         Mult;
    int            QInx;
    FlwBlk*        MFb;
    FlwBlk*        SFb;
  };

//============================================================================

class DllImportExport CFlwBlkIndex
  {
  friend class CGrpFlwBlk;
  friend class CGrpFlwBlkSeq;
  public:

    CFlwBlkIndex()
      {
      m_pNd       = NULL;
      m_iSgn      = 0;
      m_iIONo     = -1;
      m_iFENo     = 0;
      m_iLnkNo    = -1;
      };

    CFlwBlkIndex(FlwNode * pNd, int iSgn, int iIONo, int iFENo, int iLnkNo)
      {
      m_pNd       = pNd;
      m_iSgn      = iSgn;
      m_iIONo     = iIONo;
      m_iFENo     = iFENo;
      m_iLnkNo    = iLnkNo;
      };

    flag            IsIO()                { return m_iIONo>=0; };
    flag            IsLnk()               { return m_iLnkNo>=0; };
    flag            IsJoin()              { return m_iIONo<0 && m_iLnkNo<0; };

  public:
    FlwNode       * m_pNd;
    int             m_iSgn;
    int             m_iIONo;
    int             m_iFENo;
    int             m_iLnkNo;
  };

//============================================================================

const byte FBReg_Off    = 0x00;
const byte FBReg_Pi     = 0x01;
const byte FBReg_Po     = 0x02;
const byte FBReg_Qm     = 0x04;
const byte FBReg_Qv     = 0x08;
const byte FBReg_QvStep = 0x10;
const byte FBReg_All    = 0x1f;

const byte FBRShow_What = 0x01;
const byte FBRShow_Reqd = 0x02;
const byte FBRShow_Min  = 0x04;
const byte FBRShow_Max  = 0x08;
const byte FBRShow_Cur  = 0x10;
const byte FBRShow_Msg  = 0x40;
const byte FBRShow_All  = 0xff;

class DllImportExport CFlwRegBlk
  {
  friend class CGrpFlwRegBlk;
  friend class FlwBlk;
  friend class CGrpFlwBlk;
  friend class CSubNetBlk;
  friend class CRegSolver;

  public:
    CFlwRegBlk( LPCTSTR CtrlName,
                byte ShowWhat=FBRShow_All,
                byte iWhatOk=FBReg_All,
                byte iWhat=FBReg_Off,
                float dTau=1.0,
                float dPiReqd=Std_P,
                float dPoReqd=Std_P,
                float dQmReqd=0,
                float dCtrlMin=0.01f,
                float dCtrlMax=1.0f);
    void    BuildDataDefn(DataDefnBlk &DDB, FlwNode * pNd, bool ModeOK);
    flag    DataXchg(DataChangeBlk & DCB, FlwNode * pNd);
    flag    ValidateData(ValidateDataBlk & VDB, FlwNode * pNd);

    byte What() { return  m_iWhat; };
    flag On() { return  m_iWhat!=FBReg_Off; };
    double FinalTarget();
    double StepTarget();
    void SetMeas(double M);
    void SetMeasStart(double M);
    double CtrlMin() { return m_dCtrlMin; };
    double CtrlMax() { return m_dCtrlMax; };
    double Ctrl()    { return On() ? m_dCtrlCur : m_dCtrlMax; };

  protected:
    FlwBlk      * m_pFB;
    byte          m_iWhat;
    byte          m_iWhatOk;
    byte          m_iShowWhat;
    bool          m_bHoldCvg;
    float         m_dTau;
    float         m_dPiReqd,m_dPiMeas,m_dPiStart;
    float         m_dPoReqd,m_dPoMeas,m_dPoStart;
    float         m_dQmReqd,m_dQmMeas,m_dQmStart;
    float         m_dQvReqd,m_dQvMeas,m_dQvStart;
    float         m_dCtrlMin;
    float         m_dCtrlMax;
    float         m_dCtrlCur;
    Strng         m_sCtrlMinTg;
    Strng         m_sCtrlMaxTg;
    Strng         m_sCtrlCurTg;
  };

class DllImportExport FlwBlk  : public CFlwBlkBase
  {
  DEFINE_MEMSTATS(FlwBlk)

  friend class CGrpFlwBlk;
  friend class CNetBlk;
  friend class CSubNetBlk;
  friend class CJoinRec;
  friend class CFullNetBlk;
  friend class FlwSlvBlk;
  friend class FlwSlvPB;
  friend class FlwSlvDyn;
  friend class CFlwSolver;

  public:

  public:
    FlwBlk();
    virtual ~FlwBlk();
    void           ResetData(flag Complete);
    void           SetXferMode();
    void           SetXferCapacity(double QmCap);
    void           SetQmReqd(double QmReqd, flag IsFree=true, float MaxNode=fNAN, float MaxFlng=fNAN);
    void           SetQmReqdVlv(double QmReqd, double Reg, flag IsFree=true, float MaxNode=fNAN, float MaxFlng=fNAN);
    void           SetQmFree();
    double         RequiredQm();

    void           SetBiDirectionalFlow();
    void           SetUniDirectionalFlow(flag Fwd=true, double OpenPress=1.0, double ClosePress=0.0);

    void           SetChokedQm(double Qm_, double Dp_);

    FlwNode      * GetQmEstSrc();

    byte           UDFReqdAction(double &DP);
    void           UDFActivate();
    void           SetUDFOpenP(double P) { UDF.OpenP=P; };
    double         UDFOpenP() { return UDF.OpenP;};
    flag           UDFIsClosed() { return (UDF.ActionCount==UDF.MaxActionCount);};
    int            UDFActionCount() { return UDF.ActionCount;};
    void           SetUDFActionCount(int i) { UDF.ActionCount=(sint)i;};

    void           SetTopologyChanged() { if (pNetTopoChg) { *pNetTopoChg=1; *pSubNetTopoChg=1; };};

    flag           GetFixed() { return m_fFixed; };
    flag           GetIsDegFree() { return m_fIsDegFree; };
    void           SetXfer(flag On) { m_fXfer=On; };
    flag           GetXfer() { return m_fXfer; };
    void           SetRegulator(byte What=FBReg_Off) { m_iRegulator = What;};

    void           SetInXferNet(flag On) { m_fInXferNet=On; };
    flag           InXferNet() { return m_fInXferNet; };

    flag           IsXfer() { return m_fFixed && m_fXfer; };
    flag           IsFixed() { return m_fFixed; };
    flag           IsDegFree() { return m_fIsDegFree; };

    #if defined(WITH_GFB)
    void           GetFlags(CGrpFlwBlk* pGFB, int FBDirn);
    void           EvalFlow(eScdFlwEqnTasks Task, CGrpFlwBlk* pGFB, int FBDirn, int PassDirn, CFlwBlkIndex & FBI, CSpPropInfo * pNetProps, double DestP, LPCTSTR dbgChar);
    void           EvalFlowX(eScdFlwEqnTasks Task, CGrpFlwBlk* pGFB, int FBDirn, int PassDirn, CFlwBlkIndex & FBI, CSpPropInfo * pNetProps, LPCTSTR dbgChar);
    #else
    void           GetFlags(void* pGFB, int FBDirn);
    void           EvalFlow(eScdFlwEqnTasks Task, void* pGFB, int FBDirn, int PassDirn, CFlwBlkIndex & FBI, CSpPropInfo * pNetProps, double DestP, LPCTSTR dbgChar);
    void           EvalFlowX(eScdFlwEqnTasks Task, void* pGFB, int FBDirn, int PassDirn, CFlwBlkIndex & FBI, CSpPropInfo * pNetProps, LPCTSTR dbgChar);
    #endif


  public: // private:
    static double  RMin;

    FlwNode *      m_pQmEstSrc;

    double         m_QmRqd,         // MassFlow
                   m_QmReg,         // MassFlow Regulation
                   m_QmXfCap;       // MassFlow (Used By SetQmReqd & SetAuto)

    flag           m_fIsDegFree,
                   m_fFixedVlv,
                   m_fFixed,
                   m_fXfer,
                   m_fInXferNet,
                   m_fFixedPrev,
                   m_fXferPrev,
                   m_fOvrSpecd,
                   m_fWasOvrSpecd;
    flag           m_fDebugIt;

    byte           m_iRegulator;
    //double         m_dRegValue;

    int            m_iFxdPOrder;
    sint           PrevDirn;

    QmRatioRec     QmRatio;

    pflag          pNetTopoChg;
    pflag          pSubNetTopoChg;
    CChokeAtRec   *pChoke;

    UDFRec         UDF;
    double         m_DPEstFxdQm; // Used During FixedQm Press Solve
    ///double         m_DPEstFixQm;

    #if defined(WITH_GFB)
    CGrpFlwBlk     *m_pGFb;
    flag           m_bAttached2GFB;
    #else
    void          *m_pGFb;
    flag           m_bAttached2GFB;
    #endif
    flag           m_bPlaced;

#ifdef _DEBUG
  public:
    long           nMaxTraverse;
    float          nAvgTraverse;
#endif
  };

// =========================================================================

typedef CSCDList <FlwBlk*, FlwBlk*> FlwBlkList;
typedef CSCDPtrListIter<FlwBlkList, FlwBlk*> FlwBlkIter;

class FlwBlkArray : public CArray <FlwBlk, FlwBlk&>
  {
  public :
    FlwBlk  & FirstElement() { return ElementAt(0); };
    FlwBlk  & LastElement() { return ElementAt(GetUpperBound()); };
  };

// =========================================================================
//
//
//
// =========================================================================

#if KeepOldTearVarBlkEdit
DEFINE_TAGOBJEDTDERIVED(FlangeTearVarBlk, TearVarBlkClassDef);
#else
DEFINE_TAGOBJDERIVED(FlangeTearVarBlk, TearVarBlkClassDef);
#endif

class DllImportExport FlangeTearVarBlk : public TearVarBlk
  {
  protected:
    CnvAttribute TCnv, PCnv, QCnv;
    FmtAttribute TFmt, PFmt, QFmt;

  public:
    FlangeTearVarBlk(pTagObjClass pClass_, char * Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    FlangeTearVarBlk(char * Tag_, pTaggedObject pAttach);
    virtual ~FlangeTearVarBlk();

    void DoConstruct();

    virtual void   OnEditConstruct();
    virtual void   OnEditDestroy();

  };

// =========================================================================

class DllImportExport CConnectCodeLock
  {
  protected:
    CRITICAL_SECTION CodeLockSect;

  public:
    CConnectCodeLock()   { InitializeCriticalSectionAndSpinCount(&CodeLockSect, 4000); }
    ~CConnectCodeLock()  { DeleteCriticalSection(&CodeLockSect); }
    void    Lock()       { EnterCriticalSection(&CodeLockSect); };
    void    UnLock()     { LeaveCriticalSection(&CodeLockSect); };

  };

extern CConnectCodeLock ConnectCodeLock;

// ---------------------------------------------------------- PMU

typedef dword PMU_DWFLAGS;
const PMU_DWFLAGS PMU_IOId         = 0x00010000;
const PMU_DWFLAGS PMU_IONo         = 0x00020000;

typedef PMU_DWFLAGS PMU_OPTIONS;
const PMU_OPTIONS PMU_Image        = 0x00000001;
const PMU_OPTIONS PMU_AvlImage     = 0x00000002;
const PMU_OPTIONS PMU_Passing      = 0x00000004;
const PMU_OPTIONS PMU_Blocking     = 0x00000008;
const PMU_OPTIONS PMU_SLRatio      = 0x00000010;
const PMU_OPTIONS PMU_SLRatio_Rho  = 0x00000020;

// =========================================================================

const dword Connect_Terminal = 1;
const dword Connect_Flange   = 2;

class DllImportExport CConnect : public CSFGNodeInfo
  {
  protected:
    DEFINE_MEMSTATS(CConnect )
    CRITICAL_SECTION m_LockSect;

  public:
    dword          m_dwID;
    flag           m_fIsEvalSeqStart;
    FlwNode*       m_pNdSrc;
    FlwNode*       m_pNdDst;
    int            m_iIOSrc;
    int            m_iIODst;

    CConnect(dword ID);
    virtual ~CConnect();

    void           Lock() { EnterCriticalSection(&m_LockSect); };
    void           UnLock() { LeaveCriticalSection(&m_LockSect); };
    // Utilities
    void           ClrConnection() { m_pNdSrc=m_pNdDst=NULL; };
    void           SetConnection(FlwNode* pNdSrc_, int iIOSrc_, FlwNode* pNdDst_, int iIODst_)
                               { m_pNdSrc=pNdSrc_; m_iIOSrc=iIOSrc_; m_pNdDst=pNdDst_; m_iIODst=iIODst_; };
    void           AdjustConnection(FlwNode* This, int By)
                               { if (m_pNdSrc==This)
                                   m_iIOSrc=m_iIOSrc+By;
                                 else
                                   m_iIODst=m_iIODst+By; };
    flag           Connected() { return (m_pNdSrc!=NULL); };

    void           SetIsEvalSeqStart(flag On) { m_fIsEvalSeqStart=On; };
    flag           IsEvalSeqStart() { return m_fIsEvalSeqStart; };

    char *         FullTag();

    // CSFGNodeInfo Overides
    virtual LPCTSTR SFGNodeTag(TaggedObject * pRefTo=NULL) { return FullTag(); };
    virtual CSFGNodeInfo * SFGNodePtr() { return this;};
  };

// =========================================================================

class DllImportExport CTerminal : public CConnect
  {
  protected:

  public:
    int            m_Attachments;

    flag           m_fInTerminalList;
    flag           m_fIsTear;

    CTerminal();
    virtual ~CTerminal();

    flag           IsTear() { return m_fIsTear; };
    void           SetIsTear(flag On) { m_fIsTear=On; };
  };

typedef CSCDList <CTerminal*, CTerminal*> CTerminalList;
typedef CSCDPtrListIter<CTerminalList, CTerminal*> CTerminalIter;

// =========================================================================

class DllImportExport CCtrlData
  {
  public:
    double         m_dValue;

    CCtrlData();
  };

// =========================================================================

class DllImportExport CCtrlTermX : public CCtrlData, public CTerminal { };

// =========================================================================

class DllImportExport CETerminalX : /*public CElecConduit,*/ public CTerminal
  {
  public:
    CETerminalX()
      {
      };
    CETerminalX(CETermStripDefn & Terms)
      {
//ELEC       m_TStrip.PutName(Terms.Name());
//ELEC       m_TStrip.SetWireNames(Terms);
      };
  };

// =========================================================================

class DllImportExport CAirTermX : /*public CAirConduit,*/ public CTerminal { };

// =========================================================================

const int MaxFlangeQs = 2;

#if DOPB_FILT_RESULTS
const int MaxMSRqd    = 5;
#endif

class DllImportExport CFlange : public TearPosBlk, public CConnect
  {
  public:
    CFlange(bool MustDelete=false);
    virtual ~CFlange();

    // Tear Flanges - to Break Recycles
    void           SetTearType(byte Type, FlwNode* pNd);
    flag           TearImageExists()                { return m_pTearImg!=NULL; };
    SpImage*       TearImage()                      { return m_pTearImg; };
    void           SetTearImage(FlwNode* pNd);
    void           ClrTearImage(FlwNode* pNd);

    // Tear Pos Block overrides
    virtual TearVarBlk* CreateVarBlk(char * Tag, pTaggedObject pAttach);
    byte           TearVarType();
    void           SetTearInitEstUsage(byte iEu);
    //flag           TearAdvanceReqd();
    //flag           TearCnvTestReqd();
    void           TearInitialiseOutputs(double EstimatePortion);
    void           TearInputs2Outputs();
    char *         TearGetTag(Strng & RootTag);
    void           TearGetInputs(TearVarArray & TV);
    void           TearGetOutputs(TearVarArray & TV);
    void           TearSetOutputs(TearVarArray & TV);

    // Model Config Methods
    SpConduit*     Conduit(flag fIn)                { if (!fIsBuffered && IsTear()) return fIn ? pQIO : pQIn; return pQIO; };
    SpConduit*     ConduitIO()                      { return pQIO; };
    SpConduit*     ConduitIn()                      { return pQIn; };
    SpConduit*     ConduitMem()                     { if (pQMem==NULL) pQMem = new SpConduit("QMem", NULL, TOA_Free); return pQMem; };

    SpVector     * CreatePMUVector();
    void           DeletePMUVector();
    SpVector     * PMUVector()                      { return pPMU; };
    flag           IsPMUImage()                     { return (bPMUOpts & PMU_Image) != 0; };
    void           SetPMUImage(flag On=true)        { bPMUOpts= On ? (bPMUOpts|PMU_Image) : (bPMUOpts&~PMU_Image); };
    flag           IsPMUAvlImage()                  { return (bPMUOpts & PMU_AvlImage) != 0; };
    void           SetPMUAvlImage(flag On=true)     { bPMUOpts= On ? (bPMUOpts|PMU_AvlImage) : (bPMUOpts&~PMU_AvlImage); };
    flag           IsPMUPassing()                   { return (bPMUOpts & PMU_Passing) != 0; };
    void           SetPMUPassing(flag On=true)      { bPMUOpts= On ? (bPMUOpts|PMU_Passing) : (bPMUOpts&~PMU_Passing); };
    flag           IsPMUBlocking()                  { return (bPMUOpts & PMU_Blocking) != 0; };
    void           SetPMUBlocking(flag On=true)     { bPMUOpts= On ? (bPMUOpts|PMU_Blocking) : (bPMUOpts&~PMU_Blocking); };
    flag           IsPMUSLRatio()                   { return (bPMUOpts & PMU_SLRatio) != 0; };
    void           SetPMUSLRatio(flag On=true)      { bPMUOpts= On ? (bPMUOpts|PMU_SLRatio) : (bPMUOpts&~PMU_SLRatio); };
    flag           IsPMUSLRatio_Rho()               { return (bPMUOpts & PMU_SLRatio_Rho) != 0; };
    void           SetPMUSLRatio_Rho(flag On=true)  { bPMUOpts= On ? (bPMUOpts|PMU_SLRatio_Rho) : (bPMUOpts&~PMU_SLRatio_Rho); };
    PMU_OPTIONS    PMUOptions()                     { return bPMUOpts; };
    void           SetPMUOptions(PMU_OPTIONS Opts)  { bPMUOpts=Opts; };

#if DOPB_FILT_RESULTS
    SpVector     * MSRqd(int i)                     { if (pMSRqd[i]==NULL) pMSRqd[i]=new SpVector; return pMSRqd[i]; };
    int            NMSRqd()                         { return nMSRqd; };
    void           SetNMSRqd(int n)                 { nMSRqd=n; };
#endif

    // Model Makeup Methods
    static void    ResetMakeUpCntGlbl();
    static void    BumpMakeUpCntGlbl();
    void           ResetMakeUpCnt()                 { iMkUpRqdOK=0; iMkUpRqdLclOK=0; iMkUpAvlOK=0; };
    void           BumpMakeUpCnt();
    flag           IsMakeUpReqd()                   { return Valid(dMkUpRqdLcl) || Valid(dMkUpRqd); };
    flag           IsMakeUpReqdLcl()                { return Valid(dMkUpRqdLcl); };
    flag           IsMakeUpAvail()                  { return Valid(dMkUpAvl); };

#ifdef _RELEASE
    double         MakeUpReqd()                     { return Valid(dMkUpRqdLcl) ? dMkUpRqdLcl : dMkUpRqd; };
    double         MakeUpReqdLcl()                  { return dMkUpRqdLcl; };
    double         MakeUpAvail()                    { return dMkUpAvl; };

    void           SetMakeUpReqd(double A)          { iMkUpRqdOK=iMkUpOKNext; dMkUpRqd=A; };
    void           SetMakeUpReqdLcl(double A)       { iMkUpRqdLclOK=iMkUpOKNext; dMkUpRqdLcl=A; };
    void           SetMakeUpAvail(double A)         { iMkUpAvlOK=iMkUpOKNext; dMkUpAvl=A; };

    void           ClrMakeUpReqd()                  { iMkUpRqdOK=0; dMkUpRqd=dNAN; };
    void           ClrMakeUpReqdLcl()               { iMkUpRqdLclOK=0; dMkUpRqdLcl=dNAN; };
    void           ClrMakeUpAvail()                 { iMkUpAvlOK=0; dMkUpAvl=dNAN; };
#else
    double         MakeUpReqd();
    double         MakeUpReqdLcl()                  { return dMkUpRqdLcl; };
    double         MakeUpAvail()                    { return dMkUpAvl; };

    void           SetMakeUpReqd(double A);
    void           SetMakeUpReqdLcl(double A)       { iMkUpRqdLclOK=iMkUpOKNext; dMkUpRqdLcl=A; };
    void           SetMakeUpAvail(double A)         { iMkUpAvlOK=iMkUpOKNext; dMkUpAvl=A; };

    void           ClrMakeUpReqd();
    void           ClrMakeUpReqdLcl()               { iMkUpRqdLclOK=0; dMkUpRqdLcl=dNAN; };
    void           ClrMakeUpAvail()                 { iMkUpAvlOK=0; dMkUpAvl=dNAN; };
#endif

    virtual CSFGNodeInfo * SFGNodePtr()             { return this;};

    CAreaSum    & getSum()                          { return m_Sum; };
    _declspec(property(get=getSum))                 CAreaSum    & Sum;

  protected:
    DEFINE_MEMSTATS(CFlange)
  public:
    int            m_Attachments;
    bool           m_bMustDelete;

    double         m_Datum, m_DeltaZ;
    flag           //fFlngSetUp,
                   fIsBufferPos,
                   fIsBuffered,
                   fInFlangeList;

    static byte    iMkUpOKCurr;
    static byte    iMkUpOKNext;
    byte           iMkUpAvlOK;
    byte           iMkUpRqdOK;
    byte           iMkUpRqdLclOK;
    double         dMkUpRqd;
    double         dMkUpRqdLcl;
    double         dMkUpAvl;

    SpConduit*     pQIO;
    SpConduit*     pQIn;
    SpConduit*     pQMem;

    SpImage*       m_pTearImg;
    byte           m_nHoldRqd;
    byte           m_nHoldCnt;

    CPressBlk      PB;
    CSpPropInfo    m_NetProps;

#if DOPBSMOOTHING
    CMSIndex       MS;
#endif

    // ProductMakeup
    SpVector     * pPMU;
    PMU_OPTIONS    bPMUOpts;

    CAreaSum       m_Sum;


#if DOPB_FILT_RESULTS
    int            nMSRqd;
    SpVector     * pMSRqd[MaxMSRqd];
#endif
  };

typedef CSCDList <CFlange*, CFlange*> CFlangeList;
typedef CSCDPtrListIter<CFlangeList, CFlange*> CFlangeIter;

// ----------------------------------------------------------

const int IOJn_Open       = 0x01;
const int IOJn_Closed     = 0x02;
const int IOJn_Joined     = 0x04;
const int IOJn_Direct     = 0x08|IOJn_Open;
const int IOJn_UnKnown    = 0x80;

const int OId_Node        = 1;
const int OId_NodeIO      = 2;
const int OId_NodeData    = 3;

// ---------------------------------------------------------- IO

typedef __int64 IDMask;
const IDMask First64IOIds = 0xffffffffffffffff;
inline IDMask Id_2_Mask(int Id) { ASSERT(Id<64); return IDMask(1)<<Id; };

// ----------------------------------------------------------
// IO Type Ids

const int FlwIOId0        = 0;
const int CtrlIOId0       = 10000;
const int ElecIOId0       = 20000;
const int AirIOId0        = 25000;
const int MaxIOId         = 30000;

#define FlwIOId(n)        ( FlwIOId0  + n )
#define CtrlIOId(n)       ( CtrlIOId0 + n )
#define ElecIOId(n)       ( ElecIOId0 + n )
#define AirIOId(n)        ( AirIOId0  + n )

inline bool IsFlwIOId(int Id)     { return InRange(FlwIOId0,   Id, CtrlIOId0-1);  }
inline bool IsCtrlIOId(int Id)    { return InRange(CtrlIOId0,  Id, ElecIOId0-1); }
inline bool IsElecIOId(int Id)    { return InRange(ElecIOId0,  Id, AirIOId0-1); }
inline bool IsAirIOId(int Id)     { return InRange(AirIOId0,   Id, MaxIOId-1);  }

// ----------------------------------------------------------

const int MaxNdMakeups        = 5;
const int MaxNdBleeds         = 5;

// ----------------------------------------------------------
// FlwIO Type Ids
// Real IO
const int SpillIOId0      = FlwIOId(1000); // Slurry to Environment - No Flw Eqns
const int VentIOId0       = FlwIOId(2000); // Gas to Environment - No Flw Eqns
const int LeakIOId0       = FlwIOId(3000); // Both to Environment - Flw Eqns
const int MakeupIOId0     = FlwIOId(4000); // ???
const int BleedIOId0      = FlwIOId(5000); // ???
const int AAAAIOId0       = FlwIOId(6000); //
const int BBBBIOId0       = FlwIOId(7000); //
const int CCCCIOId0       = FlwIOId(8000); //
const int AreaIOId0       = FlwIOId(9000); // Io into/outof Areas

#define SpillIOId(n)      ( SpillIOId0  + n )
#define VentIOId(n)       ( VentIOId0   + n )
#define LeakIOId(n)       ( LeakIOId0   + n )
#define MakeupIOId(n)     ( MakeupIOId0 + n )
#define BleedIOId(n)      ( BleedIOId0  + n )
#define AAAAIOId(n)       ( AAAAIOId0   + n )
#define BBBBIOId(n)       ( BBBBIOId0   + n )
#define CCCCIOId(n)       ( CCCCIOId0   + n )
#define AreaIOId(n)       ( AreaIOId0   + n )

inline bool IsMLnkIOId(int Id)        { return Id<SpillIOId0;  }
inline bool IsProcessIOId(int Id)     { return Id<MakeupIOId0; }
inline bool IsDirectIOId(int Id)      { return InRange(SpillIOId0,  Id, CtrlIOId0  -1);  }

inline bool IsSpillIOId(int Id)       { return InRange(SpillIOId0,  Id, VentIOId0   -1);  };
inline bool IsVentIOId(int Id)        { return InRange(VentIOId0,   Id, LeakIOId0   -1);  };
inline bool IsLeakIOId(int Id)        { return InRange(LeakIOId0,   Id, MakeupIOId0   -1);  };
inline bool IsMakeupIOId(int Id)      { return InRange(MakeupIOId0, Id, BleedIOId0  -1);  };
inline bool IsBleedIOId(int Id)       { return InRange(BleedIOId0,  Id, AAAAIOId0  -1);  };
//inline bool IsAuditIOId(int Id)       { return false; }//InRange(ABleedIOId0,  Id, AAAAIOId0  -1);  };
//..
inline bool IsAreaIOId(int Id)        { return InRange(AreaIOId0,   Id, CtrlIOId0   -1);  };

const int IOId_2IOIn          = FlwIOId(0); // These two Ids must stay 0 and 1. They are hardwired in a number of models
const int IOId_2IOOut         = FlwIOId(1);

const int IOId_2IOLeakI       = LeakIOId(500);
const int IOId_2IOLeakO       = LeakIOId(501);

const int IOId_Spill2Area     = SpillIOId(500);
const int IOId_Vent2Area      = VentIOId(500);
const int IOId_LeakI2Area     = LeakIOId(500);
const int IOId_LeakO2Area     = LeakIOId(501);
const int IOId_Makeup2Area    = MakeupIOId(500);
const int IOId_Bleed2Area     = BleedIOId(500);

const int IOId_AreaSpillI     = SpillIOId(500);
const int IOId_AreaVentI      = VentIOId(500);
const int IOId_AreaLeakI      = LeakIOId(500);
const int IOId_AreaLeakO      = LeakIOId(501);
const int IOId_AreaMakeupO    = MakeupIOId(500);
const int IOId_AreaBleedI     = BleedIOId(500);


DllImportExport IOAreaRec TwoIOAreaList[];
DllImportExport IOAreaRec TwoIOAreaListWithS[];
DllImportExport IOAreaRec TwoIOAreaListWithSL[];
DllImportExport IOAreaRec TwoIOAreaListWithSV[];
DllImportExport IOAreaRec TwoIOAreaListWithL[];
DllImportExport IOAreaRec TwoIOAreaListWithBS[];
DllImportExport IOAreaRec TwoIOAreaListWithMBS[];

#define MAKEUP2AREA() /*==MaxNdMakeups*/                                                                                                         \
  {"", "Makeup1", IOId_Makeup2Area,   LIO_In,   nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Makeup2", IOId_Makeup2Area+1, LIO_In,   nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Makeup3", IOId_Makeup2Area+2, LIO_In,   nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Makeup4", IOId_Makeup2Area+3, LIO_In,   nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Makeup5", IOId_Makeup2Area+4, LIO_In,   nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup}

#define BLEED2AREA()  /*==MaxNdBleeds*/                                                                                                          \
  {"", "Bleed1",  IOId_Bleed2Area,    LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Bleed2",  IOId_Bleed2Area+1,  LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Bleed3",  IOId_Bleed2Area+2,  LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Bleed4",  IOId_Bleed2Area+3,  LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},  \
  {"", "Bleed5",  IOId_Bleed2Area+4,  LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup}

#define SPILL2AREA(Tg,Id)  {"", Tg, Id, LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup}
#define VENT2AREA(Tg,Id)   {"", Tg, Id, LIO_Out,  nc_MLnk, 0, 1000, IOSetXfer|IOOptsHide|IOEqnsHide|IOHidden, 0.0f, {0,0,0,0}, 0, &NullFlwGroup}
#define LEAKS2AREA(Tg,Id)  {"", Tg, Id, LIO_Out,  nc_MLnk, 0, 1000, IOOptsHide|IOEqnsHide|IOHidden,                       0.0f, {0,0,0,0}, 0, &Leak2AreaGroup}

const int MaxIOList       = 32000;
const int IOPt_MaxJoinId  = MaxIOList-1;
const int IOPt_Open       = MaxIOList+1;
const int IOPt_Closed     = MaxIOList+2;
const int IOPt_UnKnown    = MaxIOList+3;

// ---------------------------------------------------------- IO

inline int Direction(int IOIn, int IOOut) {return (IOIn < IOOut ? 1 : -1);};

const byte LIO_InOut    = eScdDirection_InOut;
const byte LIO_Out      = eScdDirection_Out;
const byte LIO_Out0     = eScdDirection_Out0;
const byte LIO_Out1     = eScdDirection_Out1;
const byte LIO_Out2     = eScdDirection_Out2;
const byte LIO_In       = eScdDirection_In;
const byte LIO_In0      = eScdDirection_In0;
const byte LIO_In1      = eScdDirection_In1;
const byte LIO_In2      = eScdDirection_In2;
const byte LIO_In0Out   = eScdDirection_In0Out;
const byte LIO_InOut0   = eScdDirection_InOut0;

const byte IOER_Vap   = 0x00000001;
const byte IOER_Liq   = 0x00000002;
const byte IOER_Sol   = 0x00000004;
const byte IOER_Img   = 0x00000008;
const byte IOER_Lvl   = 0x00000010;

const byte IOER_All   = IOER_Vap|IOER_Liq|IOER_Sol;
const byte IOER_NoVap = IOER_Liq|IOER_Sol;
const byte IOER_NoLiq = IOER_Vap|IOER_Sol;
const byte IOER_NoSol = IOER_Vap|IOER_Liq;

const byte IOER_ImgVap   = IOER_Img|IOER_Vap;
const byte IOER_ImgLiq   = IOER_Img|IOER_Liq;
const byte IOER_ImgSol   = IOER_Img|IOER_Sol;
const byte IOER_ImgNoVap = IOER_Img|IOER_Liq|IOER_Sol;
const byte IOER_ImgNoLiq = IOER_Img|IOER_Vap|IOER_Sol;
const byte IOER_ImgNoSol = IOER_Img|IOER_Vap|IOER_Liq;
const byte IOER_ImgAll   = IOER_Img|IOER_All;

const sint FD_Out     = -1;
const sint FD_Zero    = 0;
const sint FD_In      = 1;

struct IORecJoinInfo { int Id, ConnNo; };

class CHgtOrdInfo
  {
  public:
    CHgtOrdInfo()
      {
      m_pCn=NULL;
      m_dwIOIdMask=0;
      }

    int NOrd() { return Ord.GetSize(); };
    int NOvr() { return m_Ovr.GetSize(); };
    int NGas() { return m_Gas.GetSize(); };

    SpContainer &Container() { return *m_pCn; };
    SpConduit   &ContentFlw() { return m_FlwCd; };

  public:

    IDMask              m_dwIOIdMask;
    SpContainer       * m_pCn;
    SpConduit           m_FlwCd;
    CArray <int, int>   Ord;
    CArray <int, int>   Seq;
    CArray <int, int>   m_Ovr;
    CArray <int, int>   m_Gas;
  };

class CSrgFlwInfo
  {
  public:
    CSrgFlwInfo()
      {
      m_pHgtInfo=NULL;
      Reset();
      };
    void Reset()
      {
      };

    CHgtOrdInfo &HgtOrder() { return *m_pHgtInfo; };
  public:
    CHgtOrdInfo *m_pHgtInfo;

  };

enum eEvalSeqStart {ESS_Denied, ESS_Allowed, ESS_Required };

// ----------------------------------------------------------

const dword dwIOGRP           = 0x0000000f;
const dword dwIOIsBuffer      = 0x00000100;
const dword dwIOSetXfer       = 0x00000200;

const dword dwIOFlagBase      = 0x00ff0000;
const dword dwIOOverflow      = 0x00010000;
const dword dwIOGasVent       = 0x00020000;
const dword dwIOPipeEntry     = 0x00040000;
const dword dwIOPipeJoin      = 0x00080000;
const dword dwIOApertureHoriz = 0x00100000;
const dword dwIOPassVapours   = 0x00200000;
const dword dwIOPassLiquids   = 0x00400000;
const dword dwIOPassSolids    = 0x00800000;
const dword dwIOShwFracHgt    = 0x01000000;
const dword dwIOShwAperture   = 0x02000000;
const dword dwIOHidden        = 0x04000000;
const dword dwIOOptsHide      = 0x08000000;
const dword dwIOEqnsHide      = 0x10000000;

//const dword dwIOFlagUser      = 0xff000000;

#define IOGRP(x)          ((x) & dwIOGRP)
#define IOISBUFFER(x)     (((x) & dwIOIsBuffer)!=0)
#define IOSetXfer         dwIOSetXfer
#define IOISSETXFER(x)    (((x) & dwIOSetXfer)!=0)
#define IOISSETTEAR(x)    (((x) & dwIOSetTear)!=0)

#define IOIsBuffer        dwIOIsBuffer
#define IOOverflow        dwIOOverflow
#define IOGasVent         dwIOGasVent
#define IOPipeEntry       dwIOPipeEntry
#define IOPipeJoin        dwIOPipeJoin
#define IOApertureHoriz   dwIOApertureHoriz
#define IOPassVapours     dwIOPassVapours
#define IOPassLiquids     dwIOPassLiquids
#define IOPassSolids      dwIOPassSolids
#define IOShwFracHgt      dwIOShwFracHgt
#define IOShwAperture     dwIOShwAperture
#define IOHidden          dwIOHidden
#define IOOptsHide        dwIOOptsHide
#define IOEqnsHide        dwIOEqnsHide

class DllImportExport IOAreaRec
  {
  public:
    LPTSTR         pDesc;
    LPTSTR         pName;
    int            m_Id;
    byte           Dirn;
    DWORD          m_dwType;
    int            m_nIORqd;
    int            m_nIOMax;
    dword          m_dwFlags;
    float          m_dFracHgt;
    CRct_Dbl       m_GrfTarget;
    LPTSTR         m_pMapTo;
    CFlwEqnGrp     *m_pFlwGroup;
    LPTSTR         m_pDefaultTS;
    // Extra Info
    flag           fChecked,
                   fValid;
  };

// ----------------------------------------------------------

class DllImportExport CIORec
  {
  friend class FlwNode;
  friend class CNetBlk;
  friend class CSubNetBlk;
  friend class CJoinRec;
  friend class CFullNetBlk;
  friend class FlwSlvBlk;
  friend class FlwSlvPB;
  friend class FlwSlvDyn;
  friend class CFlwSolver;

  public:
    CIORec();
    ~CIORec();

    CIORec & operator=(CIORec & A);

    FlwBlkArray   &FBs()                            { return (*pFBs); };
    CPressBlkArray &PBs()                            { return (*pPBs); };

    SpConduit*     Conduit()                        { return pFlng->Conduit(FlwPos(FBs()[0].GetQm()));};
    SpConduit*     ConduitIO()                      { return pFlng->ConduitIO();};
    SpConduit*     ConduitIn()                      { return pFlng->ConduitIn();};
    SpConduit*     ConduitMem()                     { return pFlng->ConduitMem();};
    SpVector     * CreatePMUVector()                { return pFlng->CreatePMUVector(); };
    void           DeletePMUVector()                { pFlng->DeletePMUVector(); };
    SpVector     * PMUVector()                      { return pFlng->PMUVector(); };


#if DOPB_FILT_RESULTS
    SpVector     * MSRqd(int i)                     { return pFlng->MSRqd(i); };
    int            NMSRqd()                         { return pFlng->NMSRqd(); };
    void           SetNMSRqd(int n)                 { pFlng->SetNMSRqd(n); };
#endif

    flag           IsPMUImage()                     { return pFlng->IsPMUImage(); };
    void           SetPMUImage(flag On=true)        { pFlng->SetPMUImage(On); };
    flag           IsPMUAvlImage()                  { return pFlng->IsPMUAvlImage(); };
    void           SetPMUAvlImage(flag On=true)     { pFlng->SetPMUAvlImage(On); };
    flag           IsPMUPassing()                   { return pFlng->IsPMUPassing(); };
    void           SetPMUPassing(flag On=true)      { pFlng->SetPMUPassing(On); };
    flag           IsPMUBlocking()                  { return pFlng->IsPMUBlocking(); };
    void           SetPMUBlocking(flag On=true)     { pFlng->SetPMUBlocking(On); };
    flag           IsPMUSLRatio()                   { return pFlng->IsPMUSLRatio(); };
    void           SetPMUSLRatio(flag On=true)      { pFlng->SetPMUSLRatio(On); };
    flag           IsPMUSLRatio_Rho()               { return pFlng->IsPMUSLRatio_Rho(); };
    void           SetPMUSLRatio_Rho(flag On=true)  { pFlng->SetPMUSLRatio_Rho(On); };

    PMU_OPTIONS    PMUOptions()                     { return pFlng->PMUOptions(); };
    void           SetPMUOptions(PMU_OPTIONS Opts)  { pFlng->SetPMUOptions(Opts); };

    flag           V2LReqd()                        { return fV2LReqd; };
//    double         V2LFactor()                     { return dV2LFactor; };
    double         V2LQmVFact()                     { return dV2LQmVFact; };
    double         V2LQmLFact()                     { return dV2LQmLFact; };

    void           SetNFBs(int n);

    CCtrlData    & CtrlData() const                       { return *dynamic_cast<CCtrlData*>(m_pTerm); };
    void           SetCtrlData(const CCtrlData & d)       { *dynamic_cast<CCtrlData*>(m_pTerm)=d; };
    //CElecConduit & ElecConduit() const                    { return *dynamic_cast<CElecConduit*>(m_pTerm); };
    //void           SetElecConduit(const CElecConduit & d) { *dynamic_cast<CElecConduit*>(m_pTerm)=d; };
    //CAirConduit  & AirConduit() const                     { return *dynamic_cast<CAirConduit*>(m_pTerm); };
    //void           SetAirConduit(const CAirConduit & d)   { *dynamic_cast<CAirConduit*>(m_pTerm)=d; };

    int          JoinTp()                           { return iJoinTp; }
    int          JoinId()                           { return iJoinId; }
    int          JoinConnNo()                       { return iJoinConnNo; }

  protected:
    // common
    int            m_iIODescSelf,
                   m_iIODescRmt,
                   m_iIONoRmt;
    eEvalSeqStart  m_eEvalSeqStart;
    CTraceArray   *m_pChgs;

    // PwrCtrl Common
    CTerminal     *m_pTerm;

    // flw specific
    FlwNode       *Rmt;
    CFlange       *pFlng;
    FlwBlkArray   *pFBs;
    CPressBlkArray *pPBs;

    sint           iHgtOrd;
    flag           fSameHgt,
                   fIsBuffer,
                   fIsSetXfer,
                   fIsComplete,
                   fDamped,
                   fV2LReqd,
                   fConnToRmt,
                   fAutoConnect;
    bool           fEvalSeqIn;
    bool           fEvalSeqOut;
    bool           fEvalSeqWasIn;
    bool           fEvalSeqWasOut;

    double/*float*/dFracHgt,
                   dApertureAngle,
                   dAperture,
                   dEntrainRateA,
                   dEntrainRateB,
                   dEstV2LOnEntry,
                   dEstL2VOnEntry;

    byte           iExitRule;
    SpVectorObj  * m_pImg;

    double         dV2LQmVFact;
    double         dV2LQmLFact;
    double         dRhoHSensV;
    double         dRhoHSensL;
    double         dPSensV;
    double         dPSensL;

    int            iJoinTp;
    int            iJoinId;
    int            iJoinConnNo;
    int            iClusterId;

    CSrgFlwInfo    m_Srg;

    #if WithIOChgMonitor
    CDArray        DataMem;
    #endif

  };

class CIORecArray : public CArray<CIORec*, CIORec*>
  {
  public:
    ~CIORecArray()              { SetMySize(0);}
    CIORec & operator[](int i)  { return *ElementAt(i); }
    void SetMySize(int RqdSize)
      {
      int n=GetSize();
      for (int i=RqdSize; i<n; i++)
        delete ElementAt(i);
      SetSize(RqdSize, Max(4, RqdSize/8));
      for (int i=n; i<RqdSize; i++)
        ElementAt(i)=new CIORec;
      };
  };

//============================================================================

extern DllImportExport CFlwEqnGrp PipeGroup;
extern DllImportExport CFlwEqnGrp PipeFitGroup;
extern DllImportExport CFlwEqnGrp PipeEntryGroup;
extern DllImportExport CFlwEqnGrp TeeEntryGroup;
extern DllImportExport CFlwEqnGrp PumpGroup;
extern DllImportExport CFlwEqnGrp ValveGroup;
extern DllImportExport CFlwEqnGrp ChkValveGroup;
extern DllImportExport CFlwEqnGrp OrificeGroup;
extern DllImportExport CFlwEqnGrp GasPumpGroup;
extern DllImportExport CFlwEqnGrp SizeChangeGroup;
extern DllImportExport CFlwEqnGrp Leak2AreaGroup;
extern DllImportExport CFlwEqnGrp Vlv4PortGroup;

// ----------------------------------------------------------

struct DllImportExport IOInfo
  {
  int              OId;
  FlwNode*         p;
  int              AreaInx;
  int              Id;
  int              IOIndex;
  };
typedef IOInfo IOInfoBlk[2];

// ---------------------------------------------------------------------------

const dword SQs_ATLEVEL  = 0x00000001;
const dword SQs_ASMASK   = 0x00000002;
const dword SQs_ASMAKEUP = 0x00000004;
const dword SQs_IMAGE    = 0x00000008;

//---------------------------------------------------------------------------

typedef struct SubNetRec
  {
  sint             Dirn;
  char             NetId[16];

  flag             Lonely,
                   Isolated,
                   Fixed,
                   HasQRatio,
                   NetWrk;
  }  SubNetRec;

//---------------------------------------------------------------------------

enum JoinTypes { // NB The order of these Identifiers is Important
                 JTyp_Unknown,
                 JTyp_Lone,
                 JTyp_Edge,
                 JTyp_Link,
                 JTyp_Tie,
                 JTyp_ImpNd,
                 JTyp_AccLnk,
                 JTyp_Open,
                 JTyp_FxdLnk,
                 JTyp_FxdTie,
                 JTyp_XferTie,
                 JTyp_UnDef,
                 JTyp_NoOf
               };

extern DllImportExport char *cJoinType[JTyp_NoOf+1];
inline DllImportExport char * JoinTypeStr(int Typ) { return Typ>=0 ? cJoinType[Typ] : cJoinType[JTyp_Unknown]; };

//---------------------------------------------------------------------------

class DllImportExport CJConnectRec
  {
  DEFINE_SPARES(CJConnectRec)
  public:
    bool              IsIO()   { return m_iIONo>=0; }
    bool              IsLnk()  { return m_iLnkNo>=0; }

  public:
    int               m_iIONo;
    int               m_iLnkNo;
    int               m_iRmtConn;
    int               m_iRmtJoin;
    FlwNode         * m_pRmtNd;
    CFlange         * m_pFlng;

    FlwBlkArray     * m_pFBs;
    CPressBlkArray  * m_pPBs;
    sint              m_iQSgn;
  };

typedef CArray   <CJConnectRec, CJConnectRec&> CJConnectArray;

//---------------------------------------------------------------------------

class DllImportExport JProdFracRec
  {
  DEFINE_SPARES(JProdFracRec)
  public:
    int            iIONo, iConnNo;
    flag           fRestrict;
    double         dProdFrac;

    JProdFracRec() { iIONo=iConnNo=-1; fRestrict=0; dProdFrac=dNAN; };
  };

typedef CArray   <JProdFracRec, JProdFracRec&> JProdFracArray;

//---------------------------------------------------------------------------

const byte JQA_None        = 0x00;
const byte JQA_Press       = 0x01;
const byte JQA_RqdQm       = 0x02;
const byte JQA_QmRatio     = 0x04;
const byte JQA_IORatio     = 0x08;

struct JQmRatioRec
  {
  int    ConnNo;
  int    IONo;
  };

struct JIORatioRec
  {
  int    ConnNo;
  int    IONo;
  flag   bAdjustable;
  double dRatio;
  };

_FWDDEF(JoinQmAux)
class /*DllImportExport*/ JoinQmAux
  {
  DEFINE_SPARES(JoinQmAux)
  public:
    FlwNode*       pNd;

    int            iCfg, iNextCfg, fChanged;
    double         dRes,
                   dQm,
                   dPress,
                   dRqdQm;
    int            iOtherJoin;
    FlwBlk*        pFB;
    int            iPInx, iQInx;

    // Variables needed to calculate the "Extra" Qm into / outof the node as a ratio
    double         dQmRatio;
    int            nQmRatio;
    CArray <JQmRatioRec, JQmRatioRec&> QmRatio;

    // Variables needed to calculate the relative flowrates of flows around the node
    int            nIORatio;
    CArray <JIORatioRec, JIORatioRec&> IORatio;

    JoinQmAux(FlwNode* pNd_)
      {
      pNd=pNd_;
      Clear();
      };
    ~JoinQmAux()
      {
      Clear();
      };
    void Clear()
      {
      pFB=NULL;
      iCfg=iNextCfg=JQA_None;
      dRes=1.0;
      dRqdQm=0.0;
      dPress=AtmosPress();
      dQm=0.0;
      iPInx=iQInx=-1;

      nQmRatio=0;
      dQmRatio=0.0;
      QmRatio.SetSize(0);

      nIORatio=0;
      IORatio.SetSize(0);

      fChanged=1;
      };

     flag HasPress()   { return ((iCfg & JQA_Press) !=0); };
     flag HasRqdQm()   { return ((iCfg & JQA_RqdQm) !=0); };
     flag HasQmRatio() { return ((iCfg & JQA_QmRatio) !=0); };
     flag HasIORatio() { return ((iCfg & JQA_IORatio) !=0); };

     void SetQm_PressRes(double Press, double Res)
       {
       if (fabs(dRes-Res)/GTZ(fabs(Res)))
         fChanged=1;
       if (fabs(dPress-Press)/GTZ(fabs(Press)))
         fChanged=1;
       dRes=Res;
       dPress=Press;
       };
     void SetQm_FixedQm (double RqdQm)
       {
       if (fabs(dRqdQm-RqdQm)/GTZ(fabs(RqdQm)))
         fChanged=1;
       dRqdQm=RqdQm;
       };
     void SetQm_QmRatio (double Ratio)
       {
       if (fabs(dQmRatio-Ratio)/GTZ(fabs(Ratio)))
         fChanged=1;
       dQmRatio=Ratio;
       };
     void Set_IORatio (int IONo, double Ratio)
       {
       int i;
       for (i=0; i<nIORatio; i++)
         if (IORatio[i].IONo==IONo)
           break;
       ASSERT(i<nIORatio);
       if (fabs(IORatio[i].dRatio-Ratio)/GTZ(fabs(Ratio)))
         fChanged=1;
       IORatio[i].dRatio=Ratio;
       };

  };

//---------------------------------------------------------------------------
const int UnUsedIndex=INT_MIN;

class DllImportExport IndexBlk
  {
  public:
    int            nEqnCnt, iEqnOne;
    JoinTypes      Typ;

    IndexBlk() { Clear(); };

    void Clear()
      {
      nEqnCnt=0;
      iEqnOne=0;
      }

    int            EqnOne()       { return iEqnOne; }
    int            EqnCnt()       { return nEqnCnt; }
  };

//---------------------------------------------------------------------------

class DllImportExport TieInxBlk : public IndexBlk
  {
  public:
    int            iPInx;
    int            iQVAccInx, iQLAccInx;
    int            iQV2LInx;
    int            iQIsoInx;

    TieInxBlk() { Clear(); };

    void Clear()
      {
      iPInx=UnUsedIndex;
      iQVAccInx=UnUsedIndex;
      iQLAccInx=UnUsedIndex;
      iQV2LInx=UnUsedIndex;
      iQIsoInx=UnUsedIndex;
      };

    int            PInx()         { return iPInx; }
    int            TQVAccInx()    { return iQVAccInx; }
    int            TQLAccInx()    { return iQLAccInx; }
    int            TQAccInx(int Lp) { return Lp==0 ? iQVAccInx : iQLAccInx; }
    int            TQIsoInx()     { return iQIsoInx; }
    int            TQV2LInx()     { return iQV2LInx; };
  };

//---------------------------------------------------------------------------

class DllImportExport LnkInxBlk : public IndexBlk
  {
  public:
    int            iPSrcInx, iPDstInx;
    int            iQVInInx, iQLInInx;
    int            iQVAccInx, iQLAccInx;
    int            iQVOutInx, iQLOutInx;

    LnkInxBlk() { Clear(); };

    void Clear()
      {
      iPSrcInx=UnUsedIndex;
      iPDstInx=UnUsedIndex;
      iQVInInx=UnUsedIndex;
      iQLInInx=UnUsedIndex;
      iQVAccInx=UnUsedIndex;
      iQLAccInx=UnUsedIndex;
      iQVOutInx=UnUsedIndex;
      iQLOutInx=UnUsedIndex;
      };

    int            PSrcInx()      { return iPSrcInx; };
    int            PDstInx()      { return iPDstInx; };

    int            LQVInx()       { return iQVInInx; }
    int            LQLInx()       { return iQLInInx; }

    int            LQVInInx()     { return iQVInInx; };
    int            LQVOutInx()    { return iQVOutInx; };
    int            LQVAccInx()    { return iQVAccInx; };
    int            LQLInInx()     { return iQLInInx; };
    int            LQLOutInx()    { return iQLOutInx; };
    int            LQLAccInx()    { return iQLAccInx; };

    int            LQInx(int Lp) { return Lp==0 ? iQVInInx : iQLInInx; }
    int            LQInInx(int Lp)  { return Lp==0 ? iQVInInx  : iQLInInx ; };
    int            LQOutInx(int Lp) { return Lp==0 ? iQVOutInx : iQLOutInx; };
    int            LQAccInx(int Lp) { return Lp==0 ? iQVAccInx : iQLAccInx; };
  };


class DllImportExport CJoinRec
  {
  friend class CGrpFlwBlk;
  friend class CGrpFlwBlkSD;
  friend class FlwBlk;
  friend class FlwNode;
  friend class CNetBlk;
  friend class CSubNetBlk;
  friend class CFullNetBlk;
  friend class FlwSlvBlk;
  friend class FlwSlvPB;
  friend class FlwSlvDyn;
  friend class CFlwSolver;

  DEFINE_SPARES(CJoinRec)

  public:
    CJoinRec();
    ~CJoinRec();
    void           Clear();
    void           Prepare();
    void           SetTieLnkFlags();
    FlwNode*       Nd();
    double         Datum();
    flag           InFixedNet();
    flag           InXferNet();
    flag           IsXferLnk();
    flag           IsFixedLnk();

    //char *         TIndicesStr(char * Buff);
    //char *         LIndicesStr(char * Buff);
    //char *         IndexStr(int I, char * Buff);

    flag           IsImpNd()      { return fIsImpNd; };
    flag           IsAccLnk()     { return fIsAccLnk; };
    flag           IsPumpNd()     { return fIsPumpNd; };
    flag           VFlowAllowed(bool FullMode) { return FullMode ? ((m_iFlwPh_Act & FPh_Gasses)!=0) : false; }
    flag           LFlowAllowed(bool FullMode) { return FullMode ? ((m_iFlwPh_Act & FPh_Liquid)!=0) : true; }
    flag           V2LReqd(bool FullMode) { return FullMode ? fJnV2LReqd||fIOV2LReqd : false; }

    // Common
    int          & EqnOne()       { return TieInx.iEqnOne; }
    int          & EqnCnt()       { return TieInx.nEqnCnt; }
    int          & PInx()         { return TieInx.iPInx; }
    // For a Tie
    int          & TQVAccInx()    { return TieInx.iQVAccInx; }
    int          & TQLAccInx()    { return TieInx.iQLAccInx; }
    int          & TQAccInx(int Lp) { return Lp==0 ? TieInx.iQVAccInx : TieInx.iQLAccInx; }
    int          & TQIsoInx()     { return TieInx.iQIsoInx; }
    int          & TQV2LInx()     { return TieInx.iQV2LInx; };

    int            NConns();
    CJoinRec*       Join_Rmt(int c);
    flag           Conn_Open(int c);
    flag           Connected(int c);
    flag           Conn_ToLink(int c);
    flag           Conn_ToTie(int c);
    CJConnectRec & Conn(int c);
    CJConnectRec & Conn_Rmt(int c);
    int            ConnNo_Rmt(int c);
    FlwNode      * Nd_Rmt(int c);
    flag           IsIO(int c);
    int            IONo(int c);
    int            IONo_Rmt(int c);
    int            QSgn(int c);
    CFlange      * Flng(int c);
    int            NIOFBs(int c);
    int            NIOFBs_Rmt(int c);
    int            IOFBJoinIndex(int c);
    int            IOFBFlngIndex(int c);

    FlwBlk       * IOFB(int c, int fe);
    FlwBlk       * IOFBJoin(int c);
    FlwBlk       * IOFBFlng(int c);
    FlwBlk       * IOFB_Rmt(int c, int fe);
    FlwBlk       * IOFBJoin_Rmt(int c);
    FlwBlk       * IOFBFlng_Rmt(int c);

    int            NIOPBs(int c);
    int            NIOPBs_Rmt(int c);
    CPressBlk    * IOPB(int c, int fe);
    CPressBlk    * IOPB_Self(int c);
    CPressBlk    * IOPB_Flng(int c);
    CPressBlk    * IOPB_Rmt(int c, int fe);
    CIORec        * IO_Self(int c);
    CIORec        * IO_Rmt(int c);

    flag           IsLink(int c);
    int            LinkNo(int c);
    FlwBlk       * LinkFB(int c);
    CPressBlk    * LinkPB(int c, int i);
    CLinkRec     * Link(int c);
    CPressBlk    * LinkPB_Self(int c);
    CPressBlk    * LinkPB_Rmt(int c);

    int            NFBs(int c);
    FlwBlk       * FB(int c, int fe);

    FlwBlk       * FB_Self(int c);
    int            FB_SelfSgn(int c);
    double         FBQm_In(int c);

    CPressBlk    * PB_Self(int c);
    CPressBlk    * PB_Rmt(int c);

    void           Set_IOP_Self(int c, double p);
    void           Set_IOP_Flng(int c, double p);
    void           Set_IOP_Rmt(int c, double p);
    double         IOP_Self(int c);
    double         IOP_Flng(int c);
    double         IOP_Rmt(int c);

    void           Set_IOP_Est_Self(int c, double P);
    void           Set_IOP_Est_Flng(int c, double P);
    void           Set_IOP_Est_Rmt(int c, double P);
    double         IOP_Est_Self(int c);
    double         IOP_Est_Flng(int c);
    double         IOP_Est_Rmt(int c);

    double         IOP_Max_Self(int c);
    double         IOP_Max_Flng(int c);
    double         IOP_Max_Rmt(int c);

    double         IOP_MaxSet_Self(int c);
    double         IOP_MaxSet_Flng(int c);
    double         IOP_MaxSet_Rmt(int c);

#if defined(WITH_GFB)
    CGrpFlwBlk    * GFB(int c);
    CGrpFlwBlk    * GFB_Rmt(int c);
    CSubNetBlk    * SubNet() { return m_pSubNet; };
#else
    void         * GFB(int c);
    void         * GFB_Rmt(int c);
    void         * SubNet() { return m_pSubNet; };
#endif
    flag           Placed(int c);
    flag           Placed_Rmt(int c);

    double         GetQmVAccum() { return dQmVAcc; };
    double         GetQmLAccum() { return dQmLAcc; };
    double         GetQmAccum()  { return dQmVAcc+dQmLAcc; };
    double         GetQmAccum(int Lp)  { return Lp==0 ? dQmVAcc : dQmLAcc; };
    void           SetQmVAccum(double A) { ASSERT(Valid(A)); dQmVAcc=A; };
    void           SetQmLAccum(double A) { ASSERT(Valid(A)); dQmLAcc=A; };
    void           SetQmAccum(int Lp, double A) { ASSERT(Valid(A)); if (Lp==0) dQmVAcc=A; else dQmLAcc=A; };

    double         GetQmVSink() { return dQmVSink; };
    double         GetQmLSink() { return dQmLSink; };
    double         GetQmSink()  { return dQmVSink+dQmLSink; };
    double         GetQmSink(int Lp)  { return Lp==0 ? dQmVSink : dQmLSink; };
    void           SetQmVSink(double A) { ASSERT(Valid(A)); dQmVSink=A; };
    void           SetQmLSink(double A) { ASSERT(Valid(A)); dQmLSink=A; };
    void           SetQmSink(int Lp, double A) { ASSERT(Valid(A)); if (Lp==0) dQmVSink=A; else dQmLSink=A; };

    double         SigmaQmV() { return dSigmaQmV; };
    double         SigmaQmL() { return dSigmaQmL; };
    double         SigmaQm(int Lp) { return Lp==0 ? dSigmaQmV : dSigmaQmL; };
    void           SetSigmaQmV(double A) { ASSERT(Valid(A)); dSigmaQmV=A; };
    void           SetSigmaQmL(double A) { ASSERT(Valid(A)); dSigmaQmL=A; };

    double         Get_QmEst();
    double         Pressure();
    void           SetPressure(double P_);
    CPressBlk    * PB();

    void           SetQm_PressRes(double Press, double Res);
    void           SetQm_FixedQm (double RqdQm);
    void           SetQm_QmRatio (double Ratio);
    void           Set_IORatio (int IONo, double Ratio);

    void           InitFlowPhases();
    bool           TestSetFlowPhases(bool FullFlow);

    double         QmV2L() { return dQmV2L; };
    void           SetQmV2L(double V) { ASSERT(Valid(V)); dQmV2L=V; };

    double         PressEquil() { return dPressEquil; };
    void           SetPressEquil(double P) { ASSERT(Valid(P)); dPressEquil=P; };

  protected:
    static double   dProdFracTol;

    CPressBlk       m_PB;

    FlwNode*        pNd;
    int             iJoinArrayInx;
    sint            Dirn;
    sint            LnkDirn;
    int             iSeqNo;
    int             iQmToSet;

    byte            m_iFlwPh_Rqd;
    byte            m_iFlwPh_Act;

    flag            fIsOpen,
                    fIsTie,
                    fIs2IO,
                    fIsImpNd,
                    fIsAccLnk,
                    fIsPumpNd,
                    fIsMkup,
                    fIsAudit,
                    fVolPrsFX,
                    fVolIsSmall,
                    //m_fVFlowAllowed,
                    //m_fLFlowAllowed,
                    fJnV2LReqd,
                    fIOV2LReqd,
                    fCalcQmIns,
                    fCalcQmOuts,
                    fHasQmAux,
                    fFastConvergeOK,
                    fMatrixChanged,
                    fProdFracsChgd,
                    fHasProdFracs,
                    fLonely,
                    fUnDefined,
                    fIsolatedNet,
                    fIsolatedNode,
                    fIsoNetRef,
                    fInFixedNet,
                    fIsFixedXfer,
                    fIsFixedExtern,
                    fHasXBstHere,
                    fInXferNet,
                    fHasQRatio,
                    fNetWrk,
                    fSelfLoop,
                    fUDFShut,
                    fDamped,
                    fDone,
                    fBusy,
                    fHasDbgBrk;

    CJConnectArray  m_Conns;
    int             m_nConns;

    JProdFracArray ProdFracs;
    int            nProdFracs;

    JoinTypes      Typ;

    TieInxBlk      TieInx;

#if defined(WITH_GFB)
    CSubNetBlk   * m_pSubNet;
#else
    void         * m_pSubNet;
#endif

    int            NdInx;
    int            m_iJoinId;
    int            iFxdQOrder;
    int            iFxdPOrder;
    int            iFxdPSrcCn;
    double         dSigmaQmV; // Sum of Known MassFlows
    double         dSigmaQmL; // Sum of Known MassFlows

    double         dQmVAcc;   // Vapour Accumulation during implicit solution
    double         dQmLAcc;   // Liquid Accumulation during implicit solution
    double         dQmV2L;    // Vapour to  Liquid Transfer

    double         dQmVSink;  // Vapour Sink (into) Due to Other Factors
    double         dQmLSink;  // Liquid Sink (into) Due to Other Factors

    double         dV2LQmReqd;
    double         dPressEquil;
    double         Prv_Sigma_R;

  public:
    JoinQmAux     *pQmAux;
#if defined(WITH_GFB)
    CGrpFlwBlk*     pGFb;
#else
    void*          pGFb;
#endif

    long           m_NetNo;
    long           m_SubNetNo;

    Strng          sNetInfo;

    static long sm_iNo;
    long      m_iNo;

  };


class JoinArray : public CArray <CJoinRec, CJoinRec&> {};
typedef CSCDList <CJoinRec*, CJoinRec*> CJoinPtrList;
typedef CSCDPtrListIter<CJoinPtrList, CJoinRec*> JoinIter;

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CLinkRec
  {
  friend class CGrpFlwBlk;
  friend class CGrpFlwBlkSD;
  friend class FlwBlk;
  friend class FlwNode;
  friend class CNetBlk;
  friend class CSubNetBlk;
  friend class CFullNetBlk;
  friend class FlwSlvBlk;
  friend class FlwSlvPB;
  friend class FlwSlvDyn;
  friend class CFlwSolver;
  friend class CJoinRec;
  friend class CJoinConnPtr;
  friend class CJoinConnList;

  public:
    CLinkRec();
    ~CLinkRec();

    FlwBlk       & getFB()      { return m_FBs[0]; };
    CPressBlk    & PB(int i)    { return m_PBs[i]; };
    SpConduit    & getCd()      { return m_Cd; };

    _declspec(property(get=getFB))    FlwBlk    & FB;
    _declspec(property(get=getCd))    SpConduit & Cd;

    DEFINE_SPARES(CLinkRec);

  protected:
    int            m_iJoinId0;
    int            m_iJoinId1;
    int            m_iJoinConnNo0;
    int            m_iJoinConnNo1;
    int            m_iSgn;

    FlwNode      * m_pNd;
    FlwBlkArray    m_FBs;
    CPressBlkArray m_PBs;
    SpConduit      m_Cd;

#if defined(WITH_GFB)
    CGrpFlwBlk    * m_pGFb;
#else
    void         * m_pGFb;
#endif

  };
class CLinkArray : public CArray <CLinkRec, CLinkRec&> {};
typedef CSCDList <CLinkRec*, CLinkRec*> CLinkPtrList;
typedef CSCDPtrListIter<CLinkPtrList, CLinkRec*> CLinkIter;

//===========================================================================
//
//
//
//===========================================================================

/*#C:Class to solve Global problems
*/

struct CMacroMdlIO
  {
  const type_info * m_pNdTypeId;
  int               m_ioId;
  flag              m_fIsModel;
  const type_info * m_pNdVarTypeId;
  };

const flag mmio_MODEL=true;
const flag mmio_CONNECT=false;

class CMacroMdlNd
  {
  public :
    FlwNode*          m_pFNd;
    int               m_iIONo;
    CMacroMdlIO     * m_pIO;
    const type_info * m_pNdVarTypeId;
    CMacroMdlBase   * m_pVars;
  };

class DllImportExport CMacroMdlNdArray : public CArray <CMacroMdlNd, CMacroMdlNd&> {};
class DllImportExport CMacroMdlIOArray : public CArray <CMacroMdlIO, CMacroMdlIO&> {};

class DllImportExport CMacroMdl : public TaggedObject
  {
  DEFINE_MEMSTATS(CMacroMdl)

  public:
    CMacroMdl(pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CMacroMdl();

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};

    static void    RegisterNode(CMacroMdlIOArray &MMIOs, const type_info * pNdType, int ioid, flag fModel, const type_info * pNdVarTypeId);
    //static void    RegisterNode(CMacroMdlIOArray &MMIOs, TagObjClass* pClass, int ioid, flag fModel, const type_info * pNdVarTypeId);
    //static flag    RegisterNode(CMacroMdlIOArray &MMIOs, char * pClassId, int ioid, flag fModel, const type_info * pNdVarTypeId);
    void           AddNd(FlwNode* pFNd, int iIONo, CMacroMdlIO * pMMIO);

    virtual CMacroMdlIO * ValidNd(flag fIsModel, FlwNode* pNd, int iIONo) { return NULL;};
    virtual CMacroMdlIO * ValidNd(flag fIsModel, FlwNode* pNd, int iIONo, CMacroMdlIOArray &MMNds);
    virtual flag   ValidModel() { return true; };
    virtual flag   PBInitialise() { return true; };
    virtual void   PBPreJoinPressures() {};
    virtual void   PBTerminate() {};
    virtual void   PBPreEvalProducts() {};
    virtual void   PBPostEvalProducts() {};

  public:
    CMacroMdlNdArray Nds;
    Strng           MMList;
  };

typedef CSCDList <CMacroMdl*, CMacroMdl*> MacroMdlList;
typedef CSCDPtrListIter<MacroMdlList, CMacroMdl*> MacroMdlIter;
typedef CSCDPtrListIterWithPos<MacroMdlList, CMacroMdl*> MacroMdlIterPos;

//---------------------------------------------------------------------------

class DllImportExport CMacroMdlBase
  {
  public:
    CMacroMdlBase()
      {
      m_fActive=0;
      m_Iter=0;
      m_pFNd=NULL;
      m_pMNd=NULL;
      };
    virtual void    Activate(CMacroMdl* pMMNd, FlwNode* pNd);
    virtual void    Deactivate();
    virtual flag    Active() { return m_fActive; };
    virtual void    SetState(eScdMdlStateActs RqdState) { };
    CMacroMdlBase *   Address() { return this; };

    Strng  *  MMList() { return m_pMNd? &m_pMNd->MMList : NULL; };

  public:
    flag       m_fActive;
    int        m_Iter;
    FlwNode*   m_pFNd;
    CMacroMdl* m_pMNd;
  };

//==========================================================================
//
//
//
//==========================================================================

enum eFNdSWs {  FSW_Null,
                FSW_EvalJoinPressures,
                FSW_EvalJoinFlows,
                FSW_EvalFlowEquations,
                FSW_EvalState,
                FSW_ConfigureJoins,
                FSW_PostConfigureJoins,
                FSW_StartStep,
                FSW_EvalSteadyState,
                FSW_EvalPressureSens,
                FSW_EvalProductsInit,
                FSW_EvalProducts,
                FSW_EvalProductsSurge,
                FSW_EvalDerivs,
                FSW_EvalIntegral,
                FSW_ODEOperate,
                FSW_EvalDiscrete,
                FSW_EvalAudit,
                FSW_BuildCtrlNdList,
                FSW_EvalCtrlInitialise,
                FSW_EvalCtrlActions,
                FSW_EvalCtrlStrategy,
                FSW_EvalCtrlTerminate,
                FSW_EvalStatistics,
                FSW_PBEvalMakeUpAvail,
                FSW_PBEvalMakeUpReqd,
                FSW_PBEvalConvergence,
                FSW_EvalMsAGData,
                FSW_Max /* Must be last */ };

//---------------------------------------------------------------------------

#if NDSTOPWATCHES
class FNdSWs : public CArray <CStopWatch, CStopWatch&> {};
  #define NDADDSW(p)       {(p)->AddSW();};
  #define NDCLRSW(p)       {(p)->ClrSW();};
  #define NDSTARTSW(p, i)  {(p)->StartSW((i));};
  #define NDSTOPSW(p, i)   {(p)->StopSW((i));};
#else
  #define NDADDSW(p)       {};
  #define NDCLRSW(p)       {};
  #define NDSTARTSW(p, i)  {};
  #define NDSTOPSW(p, i)   {};
#endif


//---------------------------------------------------------------------------

//extern DllImportExport CStopWatch MallocSW;

//---------------------------------------------------------------------------

class DllImportExport ContStartInfo
  {
  public:
    SpMArray MassA;
    double totH0;
    double Pres;
    double Mass;
    double MassV;
    double MassL;

    ContStartInfo(SpContainer & C);
  };

//---------------------------------------------------------------------------

//files used flags...
const byte FU_CopyFile  = 0x01;
const byte FU_EditTxt   = 0x02;
const byte FU_EditExcel = 0x04;
const byte FU_EditMDB   = 0x08;

class FilesUsedItem
  {
  public:
    CString        Name;
    byte           FUFlags;

    FilesUsedItem()
      {
      FUFlags=0;
      };
    FilesUsedItem(char *pName, byte FilesUsedFlags)
      {
      Name=pName;
      FUFlags=FilesUsedFlags;
      };
    inline flag CopyFile()  { return (FUFlags & FU_CopyFile)!=0; };
    inline flag EditTxt()   { return (FUFlags & FU_EditTxt)!=0; };
    inline flag EditExcel() { return (FUFlags & FU_EditExcel)!=0; };
    inline flag EditMDB()   { return (FUFlags & FU_EditMDB)!=0; };
  };

class CFilesUsedArray : public CArray <FilesUsedItem, FilesUsedItem &>
  {
  public:
    void AddFile(char *pName, byte FilesUsedFlags=FU_CopyFile)
      {
      FilesUsedItem Item(pName, FilesUsedFlags);
      Add(Item);
      }
  };

//---------------------------------------------------------------------------

const dword TCO_DoInitialise  = 0x00000001;
const dword TCO_DoCalcs       = 0x00000002;
const dword TCO_DoCollect     = 0x00000004;
const dword TCO_DoSort        = 0x00000008;

//---------------------------------------------------------------------------

class IOP_RhoH_Info
  {
  public:
    IOP_RhoH_Info(SpContainer & C):
      Cn(C)
        {
        Lvl=0.0;
        Hgt=1.0;
        ZTop=1.0;
        RhoV=1.0;
        RhoSL=1000.0;
        Pm=Std_P;
        };

  public:
    double Lvl;
    double Hgt;
    double ZTop;
    double RhoV;
    double RhoSL;
    double Pm;

    SpContainer & Cn;
  };

//---------------------------------------------------------------------------
// Pressure control in PBMode

//const long PC_??? Moved to ScdIF

class DllImportExport CRqdPressCtrl
  {
  friend class FlwSlvPB;
  public:
    CRqdPressCtrl()
      {
      m_iMethod     = PC_MinFd;
      m_fIgnoreLoQm = PrjFileVerNo() < 57 ? false : true;
      m_dLowQmFrac  = 0.0001;
      m_fMdlAvail   = false;
      m_fDoInputs   = false;
      m_fDoOutputs  = true;
      m_dPRqd       = Std_P;
      m_dTRqd       = Std_T;
      m_dPMeas      = Std_P;
      m_dP2Set      = Std_P;
      m_pMdl        = NULL;
      }
    CRqdPressCtrl(long Method, double PRqd, bool MdlAvail)
      {
      m_iMethod     = Method;
      m_fIgnoreLoQm = PrjFileVerNo() < 57 ? false : true;
      m_dLowQmFrac  = 0.0001;
      m_fMdlAvail   = MdlAvail;
      m_fDoInputs   = false;
      m_fDoOutputs  = true;
      m_dPRqd       = PRqd;
      m_dTRqd       = Std_T;
      m_dPMeas      = Std_P;
      m_dP2Set      = Std_P;
      m_pMdl        = NULL;
      }
    void BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pObj, flag ForProbal, long PBAllowed, long DynAllowed);
    void Set(long Method, double PRqd, bool MdlAvail)
      {
      m_iMethod    = Method;
      m_dPRqd      = PRqd;
      m_dTRqd      = Std_T;
      m_fMdlAvail  = MdlAvail;
      }
    void SetMdl(SpModel/*Owner*/ *pMdl)
      {
      m_pMdl       = pMdl;
      m_fMdlAvail  = pMdl!=NULL;
      }
    void CalculateResult();

    void SetMethod(long Method) { m_iMethod = Method; }
    void SetPRqd(double PRqd)   { m_dPRqd   = PRqd; }
    void SetTRqd(double TRqd)   { m_dTRqd   = TRqd; }
    void SetPMeas(double PMeas) { m_dPMeas  = PMeas; }
    void SetP2Set(double P2Set)   { m_dP2Set = P2Set; };
    void SetP2Set(double P2Set, bool DoInputs, bool DoOutputs)
      {
      m_dP2Set = P2Set;
      m_fDoInputs = DoInputs;
      m_fDoOutputs = DoOutputs;
      };
    long          Method()                    { return m_iMethod; }
    double        PRqd()                      { return m_dPRqd; };
    double        TRqd()                      { return m_dTRqd; };
    double        PMeas()                     { return m_dPMeas; };
    double        P2Set()                     { return m_dP2Set; };
    bool          DoInputs()                  { return m_fDoInputs; };
    bool          DoOutputs()                 { return m_fDoOutputs; };
    SpModel     * Mdl()                       { return m_pMdl; };

    flag          IgnoreLoQm()                { return m_fIgnoreLoQm; };
    void          SetIgnoreLoQm(flag On)      { m_fIgnoreLoQm=On; };
    double        LowQmFrac()                 { return m_dLowQmFrac; };
    void          SetLowQmFrac(double LoQm)   { m_dLowQmFrac=LoQm; };
    double        LowQmFracNC()               { return m_dLowQmFracNC; };
    void          SetLowQmFracNC(double LoQm) { m_dLowQmFracNC=LoQm; };

  protected:
    long          m_iMethod;
    flag          m_fIgnoreLoQm;
    double        m_dLowQmFrac;
    static double m_dLowQmFracNC;
    bool          m_fMdlAvail;
    bool          m_fDoInputs;
    bool          m_fDoOutputs;
    double        m_dPRqd;   // Set by User
    double        m_dTRqd;   // Set by User
    double        m_dPMeas;  // Press measured at Inputs
    double        m_dP2Set;  // Press to set node to Set
    SpModel/*Owner*/ *m_pMdl;

  };

//---------------------------------------------------------------------------

//typedef int *IOId_Set;
//const int IDS_To=INT_MAX-1;
//const int IDS_End=INT_MAX-2;

typedef byte Z_States;
const byte Z_Unknown=0;
const byte Z_Guessed=1;
const byte Z_Known=2;
const byte Z_OverSpecified=3;

const long CloseTest_Off  = 0x00;
const long CloseTest_Idle = 0x01;
const long CloseTest_Msgs = 0x02;
const long CloseTest_Cnds = 0x04;
const long CloseTest_Both = (CloseTest_Msgs|CloseTest_Cnds);
const long CloseTest_IdleBoth = (CloseTest_Idle|CloseTest_Msgs|CloseTest_Cnds);

extern DllImportExport long gs_ClosureOption;

const long SpillReports_Off   = 0x00;
const long SpillReports_Idle  = 0x01;
const long SpillReports_Msgs  = 0x02;
const long SpillReports_Cnds  = 0x04;
const long SpillReports_Units = 0x10;
const long SpillReports_Links = 0x20;
const long SpillReports_Both  = (SpillReports_Msgs|SpillReports_Cnds);
const long SpillReports_IdleUnits = (SpillReports_Idle|SpillReports_Units|SpillReports_Msgs|SpillReports_Cnds);
const long SpillReports_IdleLinks = (SpillReports_Idle|SpillReports_Links|SpillReports_Msgs|SpillReports_Cnds);
const long SpillReports_IdleBoth  = (SpillReports_Idle|SpillReports_Units|SpillReports_Links|SpillReports_Msgs|SpillReports_Cnds);

const long Audit_Ignore       = 0;
const long Audit_Off          = 1;
const long Audit_Flows        = 2;
const long Audit_Totals       = 3;
const long Audit_Balance      = 4;

// MdlStatusOptions
const dword MSO_ShowStatus    = 0x00000001;
const dword MSO_ShowRunInEdit = 0x00000002;

//
extern DllImportExport long gs_SpillOption;
extern DllImportExport long gs_AuditOption;
extern DllImportExport bool gs_FSIdleRequested;

enum NodeFlwTasks
  {
    NFT_PBInit,
    NFT_PBQueryRemove,
    NFT_PBRemove,
    NFT_PBStep,
    NFT_PBTerm,
    NFT_PBQueryReInit,
    NFT_PBReInit,
    NFT_DynInit,
    NFT_DynTerm,
  };

enum EvalProductsInitTasks
  {
    EPI_Null,
    EPI_SourceEval,
    EPI_FwdEval,
    EPI_RevEval
  };

class DllImportExport CClosureHfGain
  {
  public:
    CClosureHfGain()  { m_HfGainAtZero=0; };

    void              SetHfInAtZero(SpModel & Mdl);
    void              AddHfInAtZero(SpModel & Mdl);
    void              AddHfOutAtZero(SpModel & Mdl);

    double            HfGainAtZero() { return m_HfGainAtZero; };

  protected:
    double            m_HfGainAtZero;
  };

class DllImportExport CClosureInfo
  {
  public:
    CClosureInfo();
    CClosureInfo(const CClosureInfo & X);
    CClosureInfo    & operator=(const CClosureInfo  & X);
    CClosureInfo    & operator+=(const CClosureInfo  & X);

    void              Clear();
    void              ZeroFlows();
    void              ZeroContent();
  public:
    int               m_JoinId;
    SpModel         * m_pRefMdl;

  //protected:
    double            m_MassFlwIn;
    double            m_MassFlwOut;
    double            m_totHfIn;
    double            m_totHfOut;
    double            m_totHsIn;
    double            m_totHsOut;

    double            m_MassGain;
    double            m_MassLoss;
    double            m_HfGain;
    double            m_HsGain;
    double            m_HfLoss;
    double            m_HsLoss;
    double            m_PowerIn;

    double            m_FeedCpT;
    double            m_ProdCpT;

    double            m_RCTMassGain;
    double            m_RCTHfGain;
    double            m_RCTHsGain;
    double            m_RCTPowerIn;
    double            m_RCTHOR_Std0;
    double            m_RCTHOR_Mdl0;
    double            m_RCTHOR_Used0;
    double            m_RCTHOR_Diff0;
    double            m_HfGainAtZero;
    double            m_EHXPowerIn;

    double            m_ContentMass;
    double            m_ContentHeat;

    double            m_InitialContent;
    double            m_InitialHeat;
    double            m_NettMass;
    double            m_NettHeat;
  };

class CClosureInfoArray : public CArray <CClosureInfo, CClosureInfo&>
  {
  public:
    CClosureInfoArray();
    void              Setup(bool DoFlows, int NJoins);
    void              SetDataAvail(bool Avail)              { m_DataAvail=Avail; };
    void              SetDataValid(bool Valid)              { m_DataValid=Valid; };
    bool              DataAvail()                           { return m_DataAvail; };
    bool              DataValid()                           { return m_DataValid; };
    bool              DoFlows()                             { return m_DoFlows && (GetSize()>0); };
    bool              DoContent()                           { return !m_DoFlows && (GetSize()>0); };
    void              SetTestDone(bool Done)                { m_TestDone=Done; };
    bool              TestDone()                            { return m_TestDone; };
    void              Clear();
    void              ZeroFlows();
    void              ZeroContent();

  protected:
    bool              m_TestDone;
    bool              m_DataAvail;
    bool              m_DataValid;
    bool              m_DoFlows;
  };

class DllImportExport CConvergeStateBlk
  {
  public:
    CConvergeStateBlk(ODE_StateCmds Cmd=eStateConverge, double TimeInc=0.0)
      {
      m_Cmd     = Cmd;
      m_TimeInc = TimeInc;
      m_pODB    = NULL;
      }
    CConvergeStateBlk(CODEDataBlock &ODB)
      {
      m_Cmd     = ODB.m_Cmd;
      m_TimeInc = ODB.m_TimeInc;
      m_pODB    = &ODB;
      }

    ODE_StateCmds   Cmd()     { return m_Cmd; };
    double          TimeInc() { return m_TimeInc; };

  protected:
    ODE_StateCmds   m_Cmd;
    double          m_TimeInc;
    CODEDataBlock  *m_pODB;
  };

#ifdef _DEBUG
#define DoPressTestAndRange 1
#else
//#define DoPressTestAndRange 1
#define DoPressTestAndRange 0
#endif
#if DoPressTestAndRange
double PressTest_(double P);
#endif

/*#C:Class on which all units and links used to describe & solve flow networks are based.
*/
enum eSQ2IO{ SQ2IO_None, SQ2IO_NonLink, SQ2IO_All};
enum AutoWireJob { AW_Prepare };

const DWORD UIIOFlag   = 0x1000000;
const DWORD UIIOFBFlag = 0x2000000;
const DWORD UINdFBFlag = 0x4000000;
const DWORD UIGlobalDX = 0x8000000;

class CPropagateNetInfoCtrl
  {
  public:
    CPropagateNetInfoCtrl(eScdPropagateNetTasks Task, long Info);
    ~CPropagateNetInfoCtrl();

    eScdPropagateNetTasks   m_Task;
    long                    m_Pass;
    long                    m_Info;
    long                    m_ReqdMethod;
    TaggedObject          * m_pObj;
  };

#if KWIKCONNECT
class CIOAreaLcl
  {
  public:
    CIOAreaLcl() { m_nConnected=0; };
    long                    m_nConnected;
  };
#endif

class CFlwNodeIndex
  {
  public:
    CFlwNodeIndex()
      {
      m_pNd  = NULL;
      m_IOIn = -1;
      };
    CFlwNodeIndex(FlwNode * pNd, int IOIn)
      {
      m_pNd  = pNd;
      m_IOIn = IOIn;
      };
  public:
    FlwNode * m_pNd;
    int       m_IOIn;
  };

class CFlwNodeIndexList : public CList <CFlwNodeIndex, CFlwNodeIndex&> {};

class DllImportExport FlwNode : public CTNode
  {
  DEFINE_MEMSTATS(FlwNode)

  public:
    FlwNode(pTagObjClass pClass_, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~FlwNode();


    virtual void   FinaliseConstruction() {};

    virtual void   AttachClassInfo(DWORD Class_, IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp=NULL);
    virtual void   AttachClassInfo(DWORD Class_, IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp0, CFlwEqnGrp * DefFlwEqnGrp1);
    virtual bool   FixElectricalIO(IOAreaRec * FixedIOPts, CETermStripDefnArray & NewTerms);
    //virtual LPTSTR GetElectricalIOTermStripName(int ioNo) { return NULL; };
    void           SwapIOAreas(IOAreaRec * IOAreas_, bool Copy);
    void           CheckIOAreas(bool OneGroup, CFlwEqnGrp * DefFlwEqnGrp0, CFlwEqnGrp * DefFlwEqnGrp1);
    virtual void   AttachIOAreas(IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp=NULL, bool Copy=false);
    virtual void   AttachIOAreas(IOAreaRec * IOAreas_, CFlwEqnGrp * DefFlwEqnGrp0, CFlwEqnGrp * DefFlwEqnGrp1bool, bool Copy=false);
    virtual CFlange * GetFlange(int IoId);// { return NULL; };
    virtual flag   IsLnk()     { return ((m_dwType & (nc_MLnk|nc_MBst))!=0 && NoProcLnkIOs()==2); };
    virtual flag   IsMLnk()    { return ((m_dwType & (nc_MLnk))!=0 && NoProcLnkIOs()==2); };
    virtual flag   IsCLnk()    { return ((m_dwType & (nc_CLnk))!=0 && NoCIOs()==2); };
    virtual flag   IsELnk()    { return ((m_dwType & (nc_ELnk))!=0 && NoEIOs()==2); };
    virtual flag   IsALnk()    { return ((m_dwType & (nc_ALnk))!=0 && NoAIOs()==2); };
    virtual flag   IsProcess() { return ((m_dwType & (nc_Process|nc_MSrcSnk|nc_MXfer))!=0); };
    virtual flag   IsControl() { return ((m_dwType & (nc_Control))!=0); };
    virtual flag   IsElec()    { return ((m_dwType & (nc_Elec))!=0); };
    flag           IsProcessSeq() { return ((m_dwType & (nc_Control|nc_CLnk|nc_Elec|nc_ELnk|nc_ALnk))==0); };
    flag           InPwrCtrlSeq() { return m_InPwrCtrlSeq; };

    void           AddPwrUser(CPwrUser*pPwr);
    void           RemovePwrUser(CPwrUser*pPwr);

    bool           PipeFlowMode()       { return FlowMode()==LFM_Full; };
    bool           LinearFlowMode()     { return FlowMode()==LFM_Linear; };
    bool           SimpleFlowMode()     { return FlowMode()==LFM_Simple; };
    bool           XferFlowMode()       { return FlowMode()==LFM_Xfer; };


    int NoProcessJoins()  { return m_nProcessJoins; };
    int NoMkupJoins()     { return m_nMkupJoins; };
    int NoAuditJoins()    { return m_nAuditJoins; };
    int NoJoins()         { return m_nJoins; };
    int NLinks()          { return m_nLinks; };
    int NClusters()       { return m_Clusters.GetSize(); };

    int FirstMkupJoin()   { return m_nProcessJoins; };
    int LastMkupJoin()    { return m_nProcessJoins+m_nMkupJoins-1; };
    int FirstAuditJoin()  { return m_nProcessJoins+m_nMkupJoins; };
    int LastAuditJoin()   { return m_nProcessJoins+m_nMkupJoins+m_nAuditJoins-1; };

    //CJoinRec & DynJoins(int i) { return Joins[i]; };

    virtual void   BuildCommonStartDefn(DataDefnBlk & DDB);
    virtual void   BuildSystemDefn_FB(bool IsLink, LPTSTR Tg, int i, int fe, DWORD UI, FlwBlk & FB, DataDefnBlk & DDB);

    virtual void   BuildSystemDefn(DataDefnBlk & DDB);
    void           BuildDataDefnElevation(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    static  pchar  TypeString(DWORD Type, Strng & TypeStr);
    virtual char *  TypeString(Strng & TypeStr, flag Clr=1);
    virtual CFlwEqnPtr * GetSystemDefnFlwEqn(int i, Strng *Tg)    { return NULL; };
    virtual long   CheckDataRanges(bool CheckIO)                  { return 0; };

    virtual void   BuildDataDefnAreaIOs_OnOff(DataDefnBlk & DDB);
    virtual void   BuildDataDefnAreaIOs(DataDefnBlk & DDB);
    virtual flag   DataXchgDefnAreaIOs(DataChangeBlk & DCB);
    virtual flag   ValidateDataAreaIOs(ValidateDataBlk & VDB);

    FlwNode*       InsertRoot(char * TagIn);
    FlwNode*       InsertNode(FlwNode* Root, LPTSTR ClassIn, LPTSTR SubClassIn, LPTSTR PrimaryCfgIn, LPTSTR TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    int            ConnectTag(FlwNode* Root, char * Tag1, char * Tag2, int RqdIONo1=-1, int RqdIONo2=-1);
    int            ConnectIOTag(FlwNode* s, char * IOTag1, FlwNode* d, char * IOTag2, IOInfoBlk *pInfo=NULL);

    flag           FindIOTag(char * TagIn, int OId_Rqd, IOInfo &Info);
    int            Attach(int LIONo, FlwNode* R, int RIONo,
                          CFlange* pF, CTerminal* pT,
                          //CCtrlData * pC, CElecConduit * pE, CAirConduit * pA,
                          int RqdIONo, int FlngDirn);
    int            Detach(int IONo);
    //int            ConnectTag(FlwNode* Root, char * Tag1, char * Tag2, /*CFlange* pF, CCtrlTerm * pT,*/ int RqdIONo1=-1, int RqdIONo2=-1);
    //int            ConnectIOTag(FlwNode* s, char * IOTag1, FlwNode* d, char * IOTag2, /*CFlange* pF, CCtrlTerm * pT,*/ IOInfoBlk *pInfo=NULL);

    int            Connect(IOInfo &Info1, IOInfo &Info2, CFlange* pF, CTerminal* pT, int RqdIONo1=-1, int RqdIONo2=-1);
    CFlange*       DisConnect(int IONo);
    int            DisConnectTag(FlwNode* Root, char * Tag1, char * Tag2);
    void           DisConnectAll();

    bool           PostConnectDirect(int IONo);
    bool           PreDisConnectDirect(int IONo);

    void           RequestGlobalReconnect();
    void           DoGlobalDisconnect();
    void           DoGlobalConnect();

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
    virtual void   ConnectionsChanged();
    virtual bool   PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo);
    virtual bool   DoPropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo, bool HasMethod);

    virtual void   OnElecComponentChange(eElecChanges EC, CECompData *pC); // CTNode Overide

    long           FlwIOsIn(int Cluster) { return m_Clusters[Cluster].m_nFlwIOsIn; };
    long           FlwIOsOut(int Cluster) { return m_Clusters[Cluster].m_nFlwIOsOut; };
    long           FlwIOsInReqd(int Cluster) { return m_Clusters[Cluster].m_nFlwIOsInReqd; };
    long           CtrlIOsIn() { return m_nCtrlIOsIn; };
    long           CtrlIOsOut() { return m_nCtrlIOsOut; };
    long           CtrlsIn() { return m_nSeqCtrlIOsIn+m_nSeqXRefsIn; };
    long           CtrlsOut() { return m_nSeqCtrlIOsOut+m_nSeqXRefsOut; };
    long           CtrlsInReqd() { return m_nCtrlsInReqd; };
    bool           CXRef_In(int i) { return m_XRefs2Me[i]->DstNd()==this; };
    bool           CXRef_Out(int i) { return m_XRefs2Me[i]->SrcNd()==this; };
    bool           CXRef_SelfLoop(int i) { return m_XRefs2Me[i]->SrcNd()==m_XRefs2Me[i]->DstNd(); };
    bool           CXRef_IsTear(int i) { return m_XRefs2Me[i]->IsTear(); };
    bool           CXRef_IsEvalSeqStart(int i) { return m_XRefs2Me[i]->IsEvalSeqStart(); };
    flag           CXRef_EvalSeqStartOK(int i) { return false; };

    CXRefItem *    CXRef(int i)       { return m_XRefs2Me[i]; };
    FlwNode   *    CXRef_SrcNd(int i) { return dynamic_cast<FlwNode*>(m_XRefs2Me[i]->SrcNd()); };
    FlwNode   *    CXRef_DstNd(int i) { return dynamic_cast<FlwNode*>(m_XRefs2Me[i]->DstNd()); };
    FlwNode   *    CXRef_RmtNd(int i) { return CXRef_In(i) ? CXRef_SrcNd(i):CXRef_DstNd(i); };

    flag           GetObj(FlwNode * &p) { p = (FlwNode*)((p) ? p->Nxt : MyChildren); return (p!=NULL);};
    flag           GetObj_All(FlwNode * &p);
    flag           GetObj_Tree(FlwNode * &p);
    char *          FullIOTag(int OId, int Inx, TagUniqueness TU, flag InternalNodes, flag SubsDescriptor=false);
    FlwNode*       Parent() {return ((FlwNode*)((CTNode*)this)->MyParent /*er()*/ );};
    FlwNode*       FamilyHead() { return (Parent() ? Parent()->FamilyHead() : this);};

    flag           SolutionBusy() { return fSolutionBusy; };

    FlwNode*       Nd_Rmt(int i)              { return (FlwNode*)(i<m_IOs.GetCount() ? m_IOs[i].Rmt : NULL);};
    flag           SolveInlineMethod_Rmt(int i)  { return Nd_Rmt(i) ? Nd_Rmt(i)->SolveInlineMethod() : false; };

    IOAreaRec     *getIOAreas()           { return m_pIOAreasOwned ? m_pIOAreasOwned : m_pIOAreas; };
    long           IOAreaCount()          { IOAreaRec *p=IOAreas; for (long N=0; p->pDesc!=NULL; p++, N++) {}; return N; };
    long           IOAreaWithIOId(int Id) { IOAreaRec *p=IOAreas; for (long N=0; p->pDesc!=NULL; p++, N++) { if (p->m_Id==Id) return N; }; return -1; };
    IOAreaRec     &IOArea(int i)          { return m_pIOAreasOwned ? m_pIOAreasOwned[i] : m_pIOAreas[i]; };
    IOAreaRec     &IOArea_Rmt(int i)      { return Nd_Rmt(i)->IOArea(m_IOs[i].m_iIONoRmt); };
    CIORec        &IO_Self(int i)         { return m_IOs[i];};
    CIORec        &IO_Rmt(int i)          { return (Nd_Rmt(i)->m_IOs[m_IOs[i].m_iIONoRmt]);};
    IOAreaRec     *IODesc_Self(int i)     { return &(IOAreas[m_IOs[i].m_iIODescSelf]);};
    IOAreaRec     *IODesc_Rmt(int i)      { return &(Nd_Rmt(i)->IOAreas[m_IOs[i].m_iIODescRmt]);};
    int            IODescNo_Self(int i)   { return m_IOs[i].m_iIODescSelf;};
    int            IODescNo_Rmt(int i)    { return m_IOs[i].m_iIODescRmt;};
    int            IOAutoConnect(int i)   { return m_IOs[i].fAutoConnect;};
    void           SetIOAutoConnect(int i, flag On) {m_IOs[i].fAutoConnect=On;};

    _declspec(property(get=getIOAreas))   IOAreaRec     *IOAreas;

    CTraceArray & IOChanges(int i)      {return *m_IOs[i].m_pChgs; }

    void           SetNIOFBs(int i, int n)     { IO_Self(i).SetNFBs(n);};
    void           SetNIOFBs_Rmt(int i, int n) { IO_Rmt(i).SetNFBs(n);};
    int            NIOFBs(int i)          {return IO_Self(i).FBs().GetSize();};
    int            NIOFBs_Rmt(int i)      {return IO_Rmt(i).FBs().GetSize();};
    int            IOFBJoinIndex(int i)   {return NIOFBs(i)-1;};
    int            IOFBFlngIndex(int i)   {return 0;};
    FlwBlkArray   *IOFBs(int i)           {return IO_Self(i).pFBs;};
    FlwBlk        *IOFB(int i, int fe)    {return &IO_Self(i).FBs()[fe];};
    FlwBlk        *IOFBJoin(int i)        {return &IO_Self(i).FBs()[NIOFBs(i)-1];};
    FlwBlk        *IOFBFlng(int i)        {return &IO_Self(i).FBs()[0];};
    FlwBlk        *IOFB_Rmt(int i, int fe) {return &IO_Rmt(i).FBs()[fe];};
    FlwBlk        *IOFBJoin_Rmt(int i)    {return &IO_Rmt(i).FBs()[NIOFBs_Rmt(i)-1];};
    FlwBlk        *IOFBFlng_Rmt(int i)    {return &IO_Rmt(i).FBs()[0];};

#if defined(WITH_GFB)
    CGrpFlwBlk     *GFB(int i)             { return IOFB(i,0)->m_pGFb; };
    CGrpFlwBlk     *GFB_Rmt(int i)         { return IOFB_Rmt(i,0)->m_pGFb; };
#else
    void          *GFB(int i)             { return IOFB(i,0)->m_pGFb; };
    void          *GFB_Rmt(int i)         { return IOFB_Rmt(i,0)->m_pGFb; };
#endif 
    flag           Placed(int i)          { return IOFB(i,0)->m_bPlaced; };
    flag           Placed_Rmt(int i)      { return IOFB_Rmt(i,0)->m_bPlaced; };
    
    CFlange       *IOFlange(int i)        { return m_IOs[i].pFlng; };
    CFlange       *IOFlangeTst(int i)     { return (m_IOs.GetCount() && (i >=0 && i < NoFlwIOs())) ? m_IOs[i].pFlng : NULL; };
    CAreaSum      *IOAreaSum(int i)       { return &IOFlange(i)->Sum; };
    SpConduit     *IOConduit(int i)       { return m_IOs[i].Conduit(); };
    SpConduit     *IOConduitIO(int i)     { return m_IOs[i].ConduitIO(); };
    SpConduit     *IOConduitIn(int i)     { return m_IOs[i].ConduitIn(); };
    SpConduit     *IOConduitMem(int i)    { return m_IOs[i].ConduitMem(); };
#if DOPBSMOOTHING
    CMSIndex      *IOMSIndex(int i)       { return &m_IOs[i].pFlng->MS; };
#endif
    int            IOModifiedIn();

    int            NoAreaSums()           { return NoProcLnkIOs()+NoEnabledDirectIOs(); };
    CAreaSum     * AreaSum(int i)         { return i<NoProcLnkIOs() ? IOAreaSum(i) : &m_EnabledDirectIOs[i-NoProcLnkIOs()]->Sum; }

    virtual CSpPropInfo *IOGetNetProps(int i, double Qm) { return NULL; };
    virtual void   IOSetNetProps(int i, CSpPropInfo * pNP) { m_IOs[i].pFlng->m_NetProps=*pNP; };
    CSpPropInfo   *IONetProps(int i)      { return &m_IOs[i].pFlng->m_NetProps; };
    CSpPropInfo   *IOFlwProps(int i, int fe);
    CSpPropInfo   *IOFwdFlwProps(int i, int fe) { return &IOFB(i,fe)->m_FlwProps[0]; };
    CSpPropInfo   *IORevFlwProps(int i, int fe) { return &IOFB(i,fe)->m_FlwProps[1]; };
    CSrgFlwInfo   *IOSurgeInfo(int i)     { return &m_IOs[i].m_Srg;};
    CHgtOrdInfo   *IOHgtOrder(int i)      { return m_IOs[i].m_Srg.m_pHgtInfo; };

    SpVector     * CreatePMUVector(int i) { return m_IOs[i].CreatePMUVector(); };
    void           DeletePMUVector(int i) { m_IOs[i].DeletePMUVector(); };
    SpVector     * PMUVector(int i)       { return m_IOs[i].PMUVector(); };


#if DOPB_FILT_RESULTS
    SpVector      *IOMSRqd(int i, int j)  { return m_IOs[i].MSRqd(j); };
    int            NMSRqd(int i)          { return m_IOs[i].NMSRqd(); };
    void           SetNMSRqd(int i, int n) { m_IOs[i].SetNMSRqd(n); };
#endif

    SpConduit     *IOConduitTst(int i)    { return (m_IOs.GetCount() && (i >=0 && i < NoFlwIOs())) ? m_IOs[i].Conduit(): NULL; };

    void           LockNode()
      {
      EnterCriticalSection(&m_LockSect);
      };
    void           UnLockNode()
      {
      LeaveCriticalSection(&m_LockSect);
      };

    flag           Ctrl_In(int i)
      {
      byte Dirn=IOAreas[m_IOs[i].m_iIODescSelf].Dirn;
      if ((Dirn & LIO_InOut)==LIO_InOut)
        {
        byte RemDirn=IOArea_Rmt(i).Dirn;
        ASSERT_ALWAYS((RemDirn & LIO_InOut)!=LIO_InOut, "Bad Control In Direction", __FILE__, __LINE__);
        return (RemDirn & LIO_Out)!=0;
        }
      return (Dirn & LIO_In)!=0;
      };
    flag           Ctrl_Out(int i)
      {
      byte Dirn=IOAreas[m_IOs[i].m_iIODescSelf].Dirn;
      if ((Dirn & LIO_InOut)==LIO_InOut)
        {
        byte RemDirn=IOArea_Rmt(i).Dirn;
        ASSERT_ALWAYS((RemDirn & LIO_InOut)!=LIO_InOut, "Bad Control Out Direction", __FILE__, __LINE__);
        return (RemDirn & LIO_In)!=0;
        }
      return (Dirn & LIO_Out)!=0;
      };
    CTerminal     *Ctrl_Terminal(int i)           { return m_IOs[i].m_pTerm; };
    flag           Ctrl_EvalSeqStartOK(int i)     { return false; };
    void           Ctrl_GetConnIDStr(int i, Strng & ID, Strng & Tg)       { ID=Tg=""; m_IOs[i].Rmt->Ctrl_ConnIDStr(IOIONo_Rmt(i), ID, Tg); };
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)          { ID=Tg=FullObjTag(); };
    void           LockCtrls();
    void           UnLockCtrls();

    int            CIOId_Self(int i)              { return IOAreas[m_IOs[i].m_iIODescSelf].m_Id;};
    int            CIOWithId_Self(int  RqdId)     { for (int i = CIO1(); i < CION(); i++) if (IOId_Self(i)==RqdId) return i; return -1;};
    int            NCIOsWithId_Self(int  RqdId)   { for (int i=CIO1(), j=0; i < CION(); i++) if (IOId_Self(i)==RqdId) j++; return j;};
    flag           CIO_In(int i)                  { return (IOAreas[m_IOs[i].m_iIODescSelf].Dirn & LIO_In)!=0; };
    flag           CIO_Out(int i)                 { return (IOAreas[m_IOs[i].m_iIODescSelf].Dirn & LIO_Out)!=0; };
    double         CIO_Value(int i)               { return m_IOs[i].CtrlData().m_dValue; };
    void           SetCIO_Value(int i, double d)  { m_IOs[i].CtrlData().m_dValue=d; };

    int            EIOId_Self(int i)              { return IOAreas[m_IOs[i].m_iIODescSelf].m_Id;};
    int            EIOWithId_Self(int  RqdId)     { for (int i = EIO1(); i < EION(); i++) if (IOId_Self(i)==RqdId) return i; return -1;};
    int            NEIOsWithId_Self(int  RqdId)   { for (int i=EIO1(), j=0; i < EION(); i++) if (IOId_Self(i)==RqdId) j++; return j;};
    flag           EIO_In(int i)                  { return (IOAreas[m_IOs[i].m_iIODescSelf].Dirn & LIO_In)!=0; };
    flag           EIO_Out(int i)                 { return (IOAreas[m_IOs[i].m_iIODescSelf].Dirn & LIO_Out)!=0; };
    //CElecConduit  &EIO_Conduit(int i) const       { return m_IOs[i].ElecConduit(); };
    //void           SetEIO_Conduit(int i, const CElecConduit & v) { m_IOs[i].SetElecConduit(v); };

    int            AIOId_Self(int i)              { return IOAreas[m_IOs[i].m_iIODescSelf].m_Id;};
    int            AIOWithId_Self(int  RqdId)     { for (int i = AIO1(); i < AION(); i++) if (IOId_Self(i)==RqdId) return i; return -1;};
    int            NAIOsWithId_Self(int  RqdId)   { for (int i=AIO1(), j=0; i < AION(); i++) if (IOId_Self(i)==RqdId) j++; return j;};
    flag           AIO_In(int i)                  { return (IOAreas[m_IOs[i].m_iIODescSelf].Dirn & LIO_In)!=0; };
    flag           AIO_Out(int i)                 { return (IOAreas[m_IOs[i].m_iIODescSelf].Dirn & LIO_Out)!=0; };
    //CAirConduit   &AIO_Conduit(int i) const       { return m_IOs[i].AirConduit(); };
    //void           SetAIO_Conduit(int i, const CAirConduit & d) { m_IOs[i].SetAirConduit(d); };

    flag           IOPipeEntry_Self(int i)     { return (IODesc_Self(i)->m_dwFlags & dwIOPipeEntry)!=0; };
    flag           IOPipeEntry_Rmt(int i)      { return (IODesc_Rmt(i)->m_dwFlags & dwIOPipeEntry)!=0; };
    flag           IOPipeJoin_Self(int i)      { return (IODesc_Self(i)->m_dwFlags & dwIOPipeJoin)!=0; };
    flag           IOPipeJoin_Rmt(int i)       { return (IODesc_Rmt(i)->m_dwFlags & dwIOPipeJoin)!=0; };

    // EvalSeqStartOK this to be set true if S Transfer sequence is allowed start here
    void           SetIO_Open(int i, int iJoinId, flag Damped, eEvalSeqStart ESS, byte FlwPhasesAllowed = FPh_Default );
    void           SetIO_Closed(int i, int iJoinId);
    void           SetIO_Join(int i, int iJoinId, eEvalSeqStart ESS=ESS_Denied, byte FlwPhasesAllowed = FPh_Default);
    void           SetIO_Direct(int i, int iJoinId);
    void           Init_NoJoins(int n);
    void           Set_NoJoins(int n);
    void           Init_NLinks(int n);
    void           Set_NLinks(int n);
    void           LinkJoins(int iConnectId, int iJoinId0, int iJoinId1, CFlwEqnPtr *FEP);

    void           SetJoinQm_PressRes(int JoinId);
    void           SetJoinQm_FixedQm (int JoinId);
    void           SetJoinQm_QmRatio (int JoinId, int IONo);
    void           SetJoinQm_Joined  (int JoinId, int OtherJoin);

    void           SetJoin_PumpNd(int JoinId, flag On);

    void           SetJoin_HasVolume(int JoinId, flag HasVolume, flag VolIsSmall);

    double         GetJoin_QmVAccum(int j)                  { return Joins[j].GetQmVAccum(); };
    double         GetJoin_QmLAccum(int j)                  { return Joins[j].GetQmLAccum(); };
    double         GetJoin_QmAccum(int j)                   { return Joins[j].GetQmAccum(); };
    double         GetJoin_QmAccum(int j, int Lp)           { return Joins[j].GetQmAccum(Lp); };
    void           SetJoin_QmVAccum(int j, double A)        { Joins[j].SetQmVAccum(A); };
    void           SetJoin_QmLAccum(int j, double A)        { Joins[j].SetQmLAccum(A); };
    void           SetJoin_QmAccum(int j, int Lp, double A) { Joins[j].SetQmAccum(Lp, A); };

    double         GetJoin_QmVSink(int j)                   { return Joins[j].GetQmVSink(); };
    double         GetJoin_QmLSink(int j)                   { return Joins[j].GetQmLSink(); };
    double         GetJoin_QmSink(int j)                    { return Joins[j].GetQmSink(); };
    double         GetJoin_QmSink(int j, int Lp)            { return Joins[j].GetQmSink(Lp); };
    void           SetJoin_QmVSink(int j, double A)         { Joins[j].SetQmVSink(A); };
    void           SetJoin_QmLSink(int j, double A)         { Joins[j].SetQmLSink(A); };
    void           SetJoin_QmSink(int j, int Lp, double A)  { Joins[j].SetQmSink(Lp, A); };

    void           SetJoin_IORatio(int JoinId, int IONo, flag Adjustable);
    void           SetJoin_IORatioRef(int JoinId, int IONo) { SetJoin_IORatio(JoinId, IONo, false); };
    void           SetJoin_IORatioAdj(int JoinId, int IONo) { SetJoin_IORatio(JoinId, IONo, true); };

    bool           IO_Closed_Self(int i)                    { return (IO_Self(i).iJoinTp&IOJn_Closed)!=0; };
    bool           IO_Closed_Rmt(int i)                     { return (IO_Rmt(i).iJoinTp&IOJn_Closed)!=0; };
    bool           IO_Open_Self(int i)                      { return (IO_Self(i).iJoinTp&IOJn_Open)!=0; };
    bool           IO_Open_Rmt(int i)                       { return (IO_Rmt(i).iJoinTp&IOJn_Open)!=0; };
    bool           IO_Direct_Self(int i)                    { return (IO_Self(i).iJoinTp&IOJn_Direct)==IOJn_Direct; };
    bool           IO_Direct_Rmt(int i)                     { return (IO_Rmt(i).iJoinTp&IOJn_Direct)==IOJn_Direct; };
    int            IO_Join_Typ(int i)                       { return m_IOs[i].iJoinTp; };
    int            IO_Join_Id(int i)                        { return m_IOs[i].iJoinId; };
    int            IO_Cluster_Id(int i)                     { return m_IOs[i].iClusterId; }
    void           SetIO_Cluster_Id(int i, int c)           { m_IOs[i].iClusterId=c;      }
    CJoinCluster * IO_Cluster_Self(int i)                   { int j=m_IOs[i].iClusterId; return j>=0 ? &m_Clusters[j] : NULL; }
    CJoinCluster * IO_Cluster_Rmt(int i)                    { int j=IO_Rmt(i).iClusterId; return j>=0 ? &Nd_Rmt(i)->m_Clusters[j] : NULL; }

    virtual eEvalSeqStart IOEvalSeqStart_Self(int i)        { return IO_Self(i).m_eEvalSeqStart; }
    eEvalSeqStart  IOEvalSeqStart_Rmt(int i)                { return Nd_Rmt(i)->IOEvalSeqStart_Self(IOIONo_Rmt(i)); }

    bool           IO_EvalSeqStartOK(int i)
                      {
                      if (IOEvalSeqStart_Self(i)==ESS_Allowed && IOEvalSeqStart_Rmt(i)==ESS_Allowed)
                        return true;
                      if (IOEvalSeqStart_Self(i)==ESS_Required || IOEvalSeqStart_Rmt(i)==ESS_Required)
                        return true;
                      return false;
                      };
    bool           IO_EvalSeqStartDenied(int i)
                      {
                      if (IOEvalSeqStart_Self(i)==ESS_Denied || IOEvalSeqStart_Rmt(i)==ESS_Denied)
                        return true;
                      return false;
                      };

    void           BuildJoinList();
    void           ConnectJoinList();
    void           PostConnectJoinList();
    flag           ConnectionsOK();
    void           CleanupConnections();
    virtual flag   MoveTearFlange(int iTearIO, FlwNode * &pNewTearNd, int &iNewTearIO);

    dword          IOType_Self(int i)           { return IOAreas[m_IOs[i].m_iIODescSelf].m_dwType; };
    int            IOId_Self(int i)             { return IOAreas[m_IOs[i].m_iIODescSelf].m_Id;};
    IDMask         IOIdMask_Self(int i)         { return IDMask(1) << IOId_Self(i); };
    //int            NIOsWithId_Self(int  RqdId);
#if KWIKCONNECT
    int            IOWithId_Self(int  RqdId);
    int            NIOsWithId_Self(int  RqdId);
#else
    int            IOWithId_Self(int  RqdId)    { for (int i = 0; i < NoFlwIOs(); i++) if (IOId_Self(i)==RqdId) return i; return -1;};
    int            NIOsWithId_Self(int  RqdId)  { for (int i=0, j=0; i < NoFlwIOs(); i++) if (IOId_Self(i)==RqdId) j++; return j;};
#endif
    int            IOId_Rmt(int i)              { return m_IOs[i].Rmt->IOAreas[m_IOs[i].m_iIODescRmt].m_Id; };
    IDMask         IOIdMask_Rmt(int i)          { return IDMask(1) << IOId_Rmt(i); };
    //int            IOWithId_Rmt(int  RqdId)     { for (int i = 0; i < NoFlwIOs(); i++) if (IOId_Rmt(i)==RqdId) return i; return -1;};
    //int            NIOsWithId_Rmt(int  RqdId)   { for (int i = 0, j=0; i < NoFlwIOs(); i++) if (IOId_Rmt(i)==RqdId) j++; return j;};

    IOAreaRec     *IODesc_SelfIndexed(int i)    { IOAreaRec * p=&IOAreas[i]; return (p && p->pName ? p : NULL); };
    IOAreaRec     *IODesc_SelfId(int iId)       { IOAreaRec * p=IOAreas; while (p && p->pName && p->m_Id!=iId) p++; return (p && p->pName ? p : NULL); };

    int            IOIONo_Rmt(int i)            { return (m_IOs.GetCount() ? m_IOs[i].m_iIONoRmt : 0);};

    virtual double LnkFlowDefined()             { return !GetActiveHold() ? PBQmEst : 0.0; };
    virtual bool   LnkFlowDefinedIn(int IONo)
      {
      if (GetActiveHold())
        return false;
      if ((IODesc_Self(IONo)->Dirn & LIO_InOut)==LIO_In)
        return true;
      if ((IODesc_Rmt(IONo)->Dirn & LIO_InOut)==LIO_Out)
        return true;

      return false;
      //return !GetActiveHold() && NoProcessIOs()==2 && (Nd_Rmt(1)==p);
      };
    virtual bool   LnkFlowDefinedOut(int IONo)
      {
      if (GetActiveHold())
        return false;
      if ((IODesc_Self(IONo)->Dirn & LIO_InOut)==LIO_Out)
        return true;
      if ((IODesc_Rmt(IONo)->Dirn & LIO_InOut)==LIO_In)
        return true;
      return false;
      //return !GetActiveHold() && (Nd_Rmt(0)==p);
      };

    int             NIOPBs(int i)                { return IO_Self(i).PBs().GetSize();};
    int             NIOPBs_Rmt(int i)            { return IO_Rmt(i).PBs().GetSize();};
    CPressBlkArray &IOPBs(int i)                 { return IO_Self(i).PBs(); };
    CPressBlk &     IOPB_Self(int i)             { return IO_Self(i).PBs().LastElement(); };
    CPressBlk &     IOPB_Flng(int i)             { return IOFlange(i)->PB; };
    CPressBlk &     IOPB_Rmt(int i)              { return IO_Rmt(i).PBs().LastElement(); };
    CPressBlk &     IOPB(int i, int pb)          { return IO_Self(i).PBs()[pb]; };
    //FBS CPressBlk &     IOFBPB_Flng(int i, int fe)          { return IO_Self(i).FBs()[fe].FlngPB(); };
    //FBS CPressBlk &     IOFBPB_Node(int i, int fe)          { return IO_Self(i).FBs()[fe].NodePB(); };
//    FlwBlk        *IOFB(int i, int fe)    {return &(*IO_Self(i).pFBs)[fe];};

    double         ContainerMeanPress(IOP_RhoH_Info & Info, double POffset);
    void           Set_IOP_RhoH_Self(int i, double P, IOP_RhoH_Info & Info);
    void           Set_IOP_RhoH_Self(int i, double P, double SLDensity, double Head);
    void           Clr_RhoH_Self(int i);
    double         IOP_RhoH_Self(int i);
    double         IOP_RhoH_Rmt(int i);

    void           Set_IOP_Self(int i, double p)
      {
      //dbgpln("Set_IOP_Self %12.2f %12.2f %s[%i]>%s", IOPB_Self(i).P, p, m_sTag(), i, Nd_Rmt(i)->FullObjTag());
      IOPB_Self(i).P=p;
      };
    void           Set_IOP_Flng(int i, double p)
      {
      //dbgpln("Set_IOP_Flng %12.2f %12.2f %s[%i]>%s", IOPB_Flng(i).P, p, m_sTag(), i, Nd_Rmt(i)->FullObjTag());
      IOPB_Flng(i).P=p;
      };
    void           Set_IOP_Rmt(int i, double p)
      {
      //dbgpln("Set_IOP_Rmt  %12.2f %12.2f %s[%i]>%s", IOPB_Rmt(i).P, p, m_sTag(), i, Nd_Rmt(i)->FullObjTag());
      IOPB_Rmt(i).P=p;
      };

#if DoPressTestAndRange
    double         IOP_Self(int i)                    { return PressTest_(IOPB_Self(i).P); };
    double         IOP_Term_Self(int i)               { return PressTest_(IOPB_Self(i).P+IOP_RhoH_Self(i)); };
    double         IOP_Flng(int i)                    { return PressTest_(IOPB_Flng(i).P); };
    double         IOP_Rmt(int i)                     { return PressTest_(IOPB_Rmt(i).P); };
    double         IOP_Term_Rmt(int i)                { return PressTest_(IOPB_Rmt(i).P+IOP_RhoH_Rmt(i)); };
#else
    double         IOP_Self(int i)                    { return IOPB_Self(i).P; };
    double         IOP_Term_Self(int i)               { return IOPB_Self(i).P+IOP_RhoH_Self(i); };
    double         IOP_Flng(int i)                    { return IOPB_Flng(i).P; };
    double         IOP_Rmt(int i)                     { return IOPB_Rmt(i).P; };
    double         IOP_Term_Rmt(int i)                { return IOPB_Rmt(i).P+IOP_RhoH_Rmt(i); };
#endif

//    void           Set_IOP_Mem_Self(int i, double P)  { IOPB_Self(i).PMem=(float)P; };
//    void           Set_IOP_Mem_Flng(int i, double P)  { IOPB_Flng(i).PMem=(float)P; };
//    double         IOP_Mem_Self(int i)                { return IOPB_Self(i).PMem; };
//    double         IOP_Mem_Flng(int i)                { return IOPB_Flng(i).PMem; };

    void           Set_IOP_Good_Self(int i, double P) { IOPB_Self(i).PGood=P; };
    void           Set_IOP_Good_Flng(int i, double P) { IOPB_Flng(i).PGood=P; };
    double         IOP_Good_Self(int i)               { return IOPB_Self(i).PGood; };
    double         IOP_Good_Flng(int i)               { return IOPB_Flng(i).PGood; };

//    void           Set_IOP_Ref_Self(int i, double P)   { IOPB_Self(i).PRef=(float)P; };
//    void           Set_IOP_Ref_Flng(int i, double P)   { IOPB_Flng(i).PRef=(float)P; };
//    void           Set_IOP_Ref_Rmt(int i, double P)    { IOPB_Rmt(i).PRef=(float)P; };
//    double         IOP_Ref_Self(int i)                { return IOPB_Self(i).PRef; };
//    double         IOP_Ref_Flng(int i)                { return IOPB_Flng(i).PRef; };
//    double         IOP_Ref_Rmt(int i)                 { return IOPB_Rmt(i).PRef; };

    void           Set_IOP_Est_Self(int i, double P)   { IOPB_Self(i).PEst=P; };
    void           Set_IOP_Est_Flng(int i, double P)   { IOPB_Flng(i).PEst=P; };
    void           Set_IOP_Est_Rmt(int i, double P)    { IOPB_Rmt(i).PEst=P; };
    double         IOP_Est_Self(int i)                { return IOPB_Self(i).PEst; };
    double         IOP_Est_Flng(int i)                { return IOPB_Flng(i).PEst; };
    double         IOP_Est_Rmt(int i)                 { return IOPB_Rmt(i).PEst; };

    double         IOP_Max_Self(int i)                { return IOPB_Self(i).PMax; };
    double         IOP_Max_Flng(int i)                { return IOPB_Flng(i).PMax; };
    double         IOP_Max_Rmt(int i)                 { return IOPB_Rmt(i).PMax; };

    double         IOP_MaxSet_Self(int i)             { return IOPB_Self(i).PMaxLim; };
    double         IOP_MaxSet_Flng(int i)             { return IOPB_Flng(i).PMaxLim; };
    double         IOP_MaxSet_Rmt(int i)              { return IOPB_Rmt(i).PMaxLim; };

    double         IODatum_Flng(int i)                { return IOFlange(i)->m_Datum; };
    double         IODatum_Term(int i);
    double         IODatum_Calc(int i)                { ASSERT(0); return 0.0; };

    sint          &IOHgtOrd(int i)                    { return m_IOs[i].iHgtOrd; };
    flag           IOSameHgt(int i)                   { return m_IOs[i].fSameHgt; };
    void           SetIOSameHgt(int i, flag Same)     { m_IOs[i].fSameHgt=Same; };
    double         IOAbsFracHgt_Flng(int i, double ShapeHeight, double ShapeDatumPt);
    double         IOAbsFracHgt_Flng(int i, SpContainer & C) { return IOAbsFracHgt_Flng(i, C.ShapeHeight(), C.ShapeDatumPt()); };
    double         IOAbsFracHgt_Term(int i, double ShapeHeight, double ShapeDatumPt);
    double         IOAbsFracHgt_Term(int i, SpContainer & C) { return IOAbsFracHgt_Term(i, C.ShapeHeight(), C.ShapeDatumPt()); };
    double         IORelFracHgt_Flng(int i, flag Ranged=true) { double F=m_IOs[i].dFracHgt; if (Ranged) F=Range(MinFracHgt[SolveInlineMethod()?1:0], F, MaxFracHgt[SolveInlineMethod()?1:0]); return F; };
    void           SetIORelFracHgt_Flng(int i, double h)      { m_IOs[i].dFracHgt=h; };

    double         IOAperture(int i, flag Ranged=true) { double A=m_IOs[i].dAperture; if (Ranged) A=Range(MinAperture[SolveInlineMethod()?1:0], A, 0.5); return A;};
    void           SetIOAperture(int i, double d)      { m_IOs[i].dAperture=d; };
    double         IOApertureAngle(int i)              { return m_IOs[i].dApertureAngle; };
    void           SetIOApertureAngle(int i, double d) { m_IOs[i].dApertureAngle=d; };

    double         IOEntrainRateA(int i)               { return m_IOs[i].dEntrainRateA; };
    double         IOEntrainRateB(int i)               { return m_IOs[i].dEntrainRateB; };
    void           SetIOEntrainRateA(int i, double d)  { m_IOs[i].dEntrainRateA=d; };
    void           SetIOEntrainRateB(int i, double d)  { m_IOs[i].dEntrainRateB=d; };

    double         IOEstV2LOnEntry(int i)              { return m_IOs[i].dEstV2LOnEntry; };
    double         IOEstL2VOnEntry(int i)              { return m_IOs[i].dEstL2VOnEntry; };
    void           SetIOEstV2LOnEntry(int i, double d) { m_IOs[i].dEstV2LOnEntry=d; };
    void           SetIOEstL2VOnEntry(int i, double d) { m_IOs[i].dEstL2VOnEntry=d; };

    byte           IOExitRule(int i)                   { return m_IOs[i].iExitRule; };
    SpVectorObj  * IOExitImage(int i)                  { return m_IOs[i].m_pImg; };
    void           SetIOExitRule(int i, byte Rule)
      {
      CIORec &io=m_IOs[i];
      io.iExitRule=Rule;
      if (Rule&IOER_Img)
        {
        if (!io.m_pImg)
          {
          io.m_pImg=new SpVectorObj("Img", this, TOA_Embedded);
          for (int s=0; s<SVSpcCount(); s++)
            io.m_pImg->M.SetVValue(s, SetNANFlag(NF_Ignore));
          io.m_pImg->SetView(SVV_AsRawFrac);
          };
        }
      else
        {
        delete io.m_pImg;
        io.m_pImg=NULL;
        };
      };

    void           SetIOQm_In(int i, double Qm, bool DoRmt=true)     { for (int fe=0; fe<NIOFBs(i); fe++) IOFB(i, fe)->SetQm(Qm);     if (DoRmt) for (fe=0; fe<NIOFBs_Rmt(i); fe++) IOFB_Rmt(i, fe)->SetQm(-Qm);   };
    void           SetIOQm_Out(int i, double Qm, bool DoRmt=true)    { for (int fe=0; fe<NIOFBs(i); fe++) IOFB(i, fe)->SetQm(-Qm);    if (DoRmt) for (fe=0; fe<NIOFBs_Rmt(i); fe++) IOFB_Rmt(i, fe)->SetQm(Qm);    };
    void           SetIOQmEst_In(int i, double Qm, bool DoRmt=true)  { for (int fe=0; fe<NIOFBs(i); fe++) IOFB(i, fe)->SetQmEst(Qm);  if (DoRmt) for (fe=0; fe<NIOFBs_Rmt(i); fe++) IOFB_Rmt(i, fe)->SetQmEst(-Qm);};
    void           SetIOQmEst_Out(int i, double Qm, bool DoRmt=true) { for (int fe=0; fe<NIOFBs(i); fe++) IOFB(i, fe)->SetQmEst(-Qm); if (DoRmt) for (fe=0; fe<NIOFBs_Rmt(i); fe++) IOFB_Rmt(i, fe)->SetQmEst(Qm); };

    double         IOQm_In(int i)                      { return IOFB(i,0)->GetQm(); };
    double         IOQm_Out(int i)                     { return -IOFB(i,0)->GetQm(); };
    double         IOQmEst_In(int i)                   { return IOFB(i,0)->GetQmEst(); };
    double         IOQmEst_Out(int i)                  { return -IOFB(i,0)->GetQmEst(); };
    double         IOQmPrvPB_In(int i)                 { return IOFB(i,0)->GetQmPrvPB(); };
    double         IOQmPrvPB_Out(int i)                { return -IOFB(i,0)->GetQmPrvPB(); };
    bool           IO_In(int i)                        { return FlwPos(IOQmEst_In(i)); };
    bool           IO_InGEZ(int i)                     { return FlwGEZ(IOQmEst_In(i)); };
    bool           IO_Out(int i)                       { return FlwPos(IOQmEst_Out(i)); };
    bool           IO_OutGEZ(int i)                    { return FlwGEZ(IOQmEst_Out(i)); };
    bool           IO_Zero(int i)                      { return FlwZero(IOQmEst_In(i)); };
    int            IO_Dirn(int i)                      { double q=IOQmEst_In(i); return (FlwZero(q) ? FD_Zero : (q>0.0 ? FD_In : FD_Out)); };
    int            IOSign(int i)                       { return IO_Dirn(i); };
    //int            TwoIOInIndex(bool Reverse=false)    { return SolveBufferedMethod() ? (!IO_Out(0) ? 0 : 1) : (!Reverse ? 0 : 1); }
    int            TwoIOInIndex(bool Reverse=false)    { return NetDynamicMethod() ? (!IO_Out(0) ? 0 : 1) : (!Reverse ? 0 : 1); }
    bool           IOEvalSeq_In(int i)                 { return m_IOs[i].fEvalSeqIn!=0; };
    bool           IOEvalSeq_Out(int i)                { return m_IOs[i].fEvalSeqOut!=0; };
    void           SetIOEvalSeq_In(int i, bool On)     { m_IOs[i].fEvalSeqIn=On; };
    void           SetIOEvalSeq_Out(int i, bool On)    { m_IOs[i].fEvalSeqOut=On; };
    bool           IOEvalSeq_WasIn(int i)              { return m_IOs[i].fEvalSeqWasIn!=0; };
    bool           IOEvalSeq_WasOut(int i)             { return m_IOs[i].fEvalSeqWasOut!=0; };
    void           SetIOEvalSeq_WasIn(int i, bool On)  { m_IOs[i].fEvalSeqWasIn=On; };
    void           SetIOEvalSeq_WasOut(int i, bool On) { m_IOs[i].fEvalSeqWasOut=On; };

    void           SetIOQmAvail_Self(int i, double A)  { for (int fe=0; fe<NIOFBs(i); fe++) IOFB(i,fe)->SetQmAvail(A); };
    void           SetIOQmSpace_Self(int i, double A)  { for (int fe=0; fe<NIOFBs(i); fe++) IOFB(i,fe)->SetQmSpace(A); };
    double         IOQmAvail_Self(int i)               { return IOFB(i,0)->GetQmAvail(); };
    double         IOQmSpace_Self(int i)               { return IOFB(i,0)->GetQmSpace(); };
    double         IOQmAvail_Rmt(int i)                { return IOFB_Rmt(i,0)->GetQmAvail(); };
    double         IOQmSpace_Rmt(int i)                { return IOFB_Rmt(i,0)->GetQmSpace(); };

//    void           SetIOQmAvail(int i, double A)           { IOFB(i)->SetQmAvail(A); };
//    void           SetIOQmSpace(int i, double A)           { IOFB(i)->SetQmSpace(A); };
//    double         IOQmAvail(int i)                        { return IOFB(i)->GetQmAvail(); };
//    double         IOQmSpace(int i)                        { return IOFB(i)->GetQmSpace(); };

//    flag           IOGasCanEscape(int i)                   { return IOFB(i)->GetGasCanEscape(); };
//    double         IOFullLiqFlow(int i)                    { return IOFB(i)->GetFullLiqFlowFrac(); };
//    flag           IOLiqAtBase(int i)                      { return IOFB(i)->GetLiqAtBase(); };
//    void           SetIOGasCanEscape(int i, flag On)       { IOFB(i)->SetGasCanEscape(On); };
//    void           SetIOFullLiqFlow(int i, double F)       { IOFB(i)->SetFullLiqFlowFrac(F); };
//    void           SetIOLiqAtBase(int i, flag On)          { IOFB(i)->SetLiqAtBase(On); };

    FlwNode      * GetQmEstSrc2IO();
    LPCTSTR        GetQmEstType2IO();
    void           SetQmEst2IO(double Rqd, eSQ2IO DoWhat=SQ2IO_All);
    double         GetQmEst2IO(double *Rqd);
    void           SetQm2IO(double Rqd, eSQ2IO DoWhat=SQ2IO_All);
    double         GetQm2IO(double *Rqd);

    CPressBlk  &   JoinPBlk(int iJoinId);
    void           Set_JoinP(int iJoinId, double P)           { JoinPBlk(iJoinId).P=P; };
    void           Set_JoinP_Est(int iJoinId, double P)       { JoinPBlk(iJoinId).PEst=P; };
    void           Set_JoinP_Good(int iJoinId, double P)      { JoinPBlk(iJoinId).PGood=P; };
    //void           Set_JoinP_Ref(int iJoinId, double P)       { JoinPBlk(iJoinId).PRef=(float)P; };
    void           Set_JoinP_Max(int iJoinId, double P)       { JoinPBlk(iJoinId).PMax=(float)P; };
    void           Set_JoinP_MaxSet(int iJoinId, double P)    { JoinPBlk(iJoinId).PMaxLim=(float)P; };
    double         JoinP(int iJoinId)                         { return JoinPBlk(iJoinId).P; };
    double         JoinP_Est(int iJoinId)                     { return JoinPBlk(iJoinId).PEst; };
    double         JoinP_Good(int iJoinId)                    { return JoinPBlk(iJoinId).PGood; };
    //double         JoinP_Ref(int iJoinId)                     { return JoinPBlk(iJoinId).PRef; };
    double         JoinP_Max(int iJoinId)                     { return JoinPBlk(iJoinId).PMax; };
    double         JoinP_MaxSet(int iJoinId)                  { return JoinPBlk(iJoinId).PMaxLim; };

    void           Set_IO_PSensV(int i, double P)            { m_IOs[i].dPSensV=P; };
    void           Set_IO_PSensL(int i, double P)            { m_IOs[i].dPSensL=P; };
    void           Set_IO_PSensVL(int i, double V, double L) { m_IOs[i].dPSensV=V; m_IOs[i].dPSensL=L; };
    double         IO_PSensV(int i)                          { return m_IOs[i].dPSensV; };
    double         IO_PSensL(int i)                          { return m_IOs[i].dPSensL; };

    void           Set_IO_RhoHSensV(int i, double P)            { m_IOs[i].dRhoHSensV=P; };
    void           Set_IO_RhoHSensL(int i, double P)            { m_IOs[i].dRhoHSensL=P; };
    void           Set_IO_RhoHSensVL(int i, double V, double L) { m_IOs[i].dRhoHSensV=V; m_IOs[i].dRhoHSensL=L; };
    double         IO_RhoHSensV(int i)                          { return m_IOs[i].dRhoHSensV; };
    double         IO_RhoHSensL(int i)                          { return m_IOs[i].dRhoHSensL; };

    void           Set_IO_V2LQmFact(int i, double FV, double FL);//         { m_IOs[i].dV2LFactor=F; };
    double         IO_V2LQmVFact(int i)                      { return m_IOs[i].dV2LQmVFact; };
    double         IO_V2LQmLFact(int i)                      { return m_IOs[i].dV2LQmLFact; };
    void           SetJoinV2LFlow(int j, double Flow);//, double VFact, double LFact);
    double         JoinV2LFlow(int j)                        { return Joins[j].dV2LQmReqd; };
    void           SetJoinPressEquil(int j, double P);//         { m_IOs[i].dV2LFactor=F; };
    double         JoinPressEquil(int j)                     { return Joins[j].dPressEquil; };

    void           RestoreContents(SpContainer &C, ContStartInfo & Start);
    void           ZeroVLSensitivities(int JoinId);
    void           SetVLSensitivities(int JoinId, SpContainer & C, ContStartInfo & Start, bool DoConvergeStates);
    void           SetStaticHeadSensitivities(int JoinId, SpContainer & C, ContStartInfo & Start);
    void           SetEquilibratingV2L(int JoinId, SpContainer & C, ContStartInfo & Start);

    void           Set_ProdFrac(int JoinNo, int i, double Frac);

    virtual long   NodeFlwTask(NodeFlwTasks Task);

    FlwNode*       CreateFlwNode(char * Class, char * SubClass, char * TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    char *          FindFlwNodeIdIndexed(char * pGroup, int iIndex);
    char *          FindFlwNodeDescIndexed(char * pGroup, int iIndex);
    char *          FindFlwNodeShortDescIndexed(char * pGroup, int iIndex);

    virtual flag   PreStartCheck();
    void           MakeAreaTag();
    void           SetAreaTag(LPCTSTR Tag);
    LPCTSTR        GetAreaTagDisp();

    virtual void   GlobalConnect();
    virtual void   PostGlobalConnect();
    virtual void   GlobalDisConnect();
    virtual void   PreGlobalDisConnect();
    virtual flag   InitialiseSolution() { return true; };
    virtual flag   TerminateSolution()  { return true; };
    virtual void   SetNodeTypeFlags()   { };
    virtual flag   IsValidAuditNode()   { return false; };

    virtual void   StartSolution() { };
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn) { };
    virtual flag   Set_Sizes() { return true; };
    virtual void   SetDatumsDone();// { };

    void           SetQRatio(int iSolnIndex, FlwBlk* pSI, double Ratio, FlwBlk* pMI);


    //long            m_lModesNear;

    //void            SetHasFullFlow(flag On);
    //flag            HasFullFlow();
    //void            SetHasModeNear(int iJoinNo, long Mode);
    //void            AddHasModeNear(int iJoinNo, long Mode);
    flag           HasModeNear(int iJoinNo, long Mode);


    double         MeasureJoinPressure(int iJoinNo, CRqdPressCtrl *pPC=NULL);
    void           SetJoinPressure(int iJoinNo, CRqdPressCtrl *pPC=NULL);
    void           SetJoinPressure(int iJoinNo, double P, bool DoInputs=false, bool DoOutputs=true);
    void           EvalJoinPressure(int iJoin, CRqdPressCtrl *pPC=NULL, SpModel/*Owner*/ * pMdl=NULL);

    virtual void   EvalJoinPressures(long JoinMask);
    virtual void   EvalJoinFlows(int JoinNo) {};
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo) { return false; };
    //virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All) {};
    //virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All) {};
    virtual void   EvalState() {};

    virtual CFlwRegBlk * GetFlwRegulator() { return NULL; };

    virtual void   SetState(eScdMdlStateActs RqdState);

    virtual flag   GetModelAction(CMdlActionArray & Acts)   { return false; };
    virtual flag   SetModelAction(CMdlAction & Act)         { return false; };

    virtual flag   GetModelGraphic(CMdlGraphicArray & Grfs) { return false; };
    virtual flag   OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf) { return false; };

    virtual void   PreConfigureJoins();
    virtual void   ConfigureJoins()                         {};
    virtual void   PostConfigureJoins();
    virtual void   StartStep()        {};
    virtual void   EvalSteadyState()  {};

    virtual void   InitFlowInfo();
    virtual void   StepFlowInfo();
    virtual void   EvalFlowInfoStart();
    virtual void   EvalFlowInfoEnd();
    virtual flag   TestFlowInfo();
    virtual flag   EvalPressureSens() { return true; };
//    virtual void   CompletePressureSens();
    virtual void   ApplyDerivs(double dTime, flag DoDbg) {};

    virtual double GetMaxTimeInc()     { return 1e10; };
    virtual void   EvalProductsInit(EvalProductsInitTasks Task) {};
    virtual bool   EvalProductClusters(int Index, long & JoinMask)   { JoinMask=-1; return Index==0; };
    virtual void   EvalProductsSurge(CNodeEvalIndex & NEI) {};
    virtual void   EvalProducts(CNodeEvalIndex & NEI)      {};
    virtual void   EvalDerivs(CNodeEvalIndex & NEI)        {};
    //virtual void   PostEvalDerivs()    {};
    virtual void   EvalIntegral(CNodeEvalIndex & NEI)      { fIntegralDone=false; };
    virtual void   ODEOperate(CODEDataBlock & ODB);

    // Helper usually called by ODEOperate
    virtual void   ConvergeStates(CConvergeStateBlk & CSB)    {};
    bool           IntegralDone()             { return fIntegralDone; };
    void           SetIntegralDone(bool Done) { fIntegralDone=Done; };
    //bool           InlineIntegral()           { return !SolveBufferedMethod(); }

    int            UpdateElectrics();
    virtual int    AutoWiring(AutoWireJob AWJ)            { return 0; };
    virtual int    GetNodeWiring(CNodeWiring &NodeWiring);// { return 0; };
    virtual int    GetLinkWiring(CLinkWiring &LinkWiring) { return -1; };
    virtual int    SetLinkWiring(CLinkWiring &LinkWiring) { return -1; };
    virtual int    GetWireCount()                         { return -1; }
    virtual CEWire * GetWire(int Index)                   { return NULL; }
    virtual void   ClearWires()                           {};
    virtual bool   SetWire(int iWire, LPCTSTR sTag,
                           LPCTSTR sSrcTermStripName, LPCTSTR sSrcTerminalName,
                           LPCTSTR sDstTermStripName, LPCTSTR sDstTerminalName) { return false; };

    virtual void   EvalDiscrete() {};
    virtual void   EvalAudit(int Pass);
    virtual void   GetBalanceReport(CNodeAuditInfo & Audit);
    //virtual void   CheckXRefs();
    virtual void   EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All) {};
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All) {};
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All) {};
    virtual void   EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All) {};
    virtual void   EvalPowerRequired() {};
    virtual void   EvalStatistics(eScdCtrlTasks Tasks=CO_All) {};

#if WithIOChgMonitor
    virtual void   MonitorPreEvalProducts();
    virtual void   MonitorPostEvalProducts();
    virtual void   MonitorPostTestTears();
#endif

    void           ResetMakeUpCnt();
    void           BumpMakeUpCnt();
    virtual void   EvalPBMakeUpReqd(long JoinMask)   { };
    virtual void   EvalPBMakeUpAvail(long JoinMask)  { };

    virtual void   SteadyStateInit(eScdSSTests ssTest);
    virtual bool   SteadyStateTest(eScdSSTests ssTest, CToleranceBlock & PTol, CToleranceBlock & QmTol,
                   double & MaxError, Strng & MaxTag);

    virtual void   ClosureInfo();
    void           GetClosureInfo(bool DoFlows, bool DoClear);
    double         GetBalanceValue(eScdBalanceValues BV, int Join);
      //{ return (BV==eBV_RCTFeedTemp || BV==eBV_RCTProdTemp) ? dNAN : 0.0; };

    virtual void   DumpStates();
    virtual void   DumpDerivs();

    static void    RegisterMacroMdlNode(CMacroMdlIOArray &MMIOs, const type_info * pNdType, int ioid, flag fModel, const type_info * pNdVarTypeId)
      { CMacroMdl::RegisterNode(MMIOs, pNdType, ioid, fModel, pNdVarTypeId);};
    //static void    RegisterMacroMdlNode(CMacroMdlIOArray &MMIOs, TagObjClass* pClass, int ioid, flag fModel, const type_info * pNdVarTypeId)
    //  { CMacroMdl::RegisterNode(MMIOs, pClass, ioid, fModel, pNdVarTypeId);};
    //static flag    RegisterMacroMdlNode(CMacroMdlIOArray &MMIOs, char * pClassId, int ioid, flag fModel, const type_info * pNdVarTypeId)
    //  { return CMacroMdl::RegisterNode(MMIOs, pClassId, ioid, fModel, pNdVarTypeId);};
    virtual void   MacroMdlEvaluate(eScdMacroMdlEvals Eval) { };
    virtual flag   MacroMdlValidNd(int iIONo)   { return true; };
    virtual void   MacroMdlAddNd(int iIONo)     { };
    virtual void   MacroMdlRemoveNd(int iIONo)  { };
    virtual CMacroMdlBase* MacroMdlActivate()     { return NULL; };
    virtual void   MacroMdlDeactivate()         { };

    virtual flag   QuerySubsReqd(CXM_DataRequest &Rqst, long &MdlIndex)    { return CNodeXRefMngr::QuerySubsReqd(Rqst, MdlIndex); };
    virtual flag   QuerySubsAvail(CXM_DataRequest &Rqst, long &MdlIndex)   { return CNodeXRefMngr::QuerySubsAvail(Rqst, MdlIndex); };
    virtual flag   ReadSubsData(CXM_ObjectData &ObjData, long &MdlIndex)   { return CNodeXRefMngr::ReadSubsData(ObjData, MdlIndex); };
    virtual flag   WriteSubsData(CXM_ObjectData &ObjData, long &MdlIndex)  { return CNodeXRefMngr::WriteSubsData(ObjData, MdlIndex); };

    virtual flag   OnLoad(char * pConfigName);
    virtual flag   OnSave(char * pConfigName);
    virtual void   OnAppActivate(BOOL bActive);
    virtual int    FilesUsed(CFilesUsedArray & Files);
//    virtual int    FilesUsed(CStringArray& Files);
    virtual int    ChangeTag(char * pOldTag, char * pNewTag) { return EOCT_DONE; };
    virtual int    DeleteTag(char * pDelTag) { return EODT_DONE; };
    virtual int    GetConnectionTags(Strng_List & TagLst);

    virtual void   InitChangeTrace();
    virtual void   TraceChanges();
    virtual CTraceArray * GetChangeTrace(long i, dword Flags);

    //CNodeXRefMngr Overrides
    virtual int    ResolveNearXRef(CXRefItem * XRef, flag MustBeParm);
    virtual int    ResolveFarXRef(CXRefItem * XRef);
    virtual int    ResolveIORef(int IOId, CTagRef * & pRef);
    virtual LPSTR  GetOwnerTag();
    virtual bool   GetOwnerDbgBrk();
    virtual long   GetCIONo(long Id, Strng & ID, Strng & Tg, bool & IsIn, bool &IsOut)
      {
      int i=CIOWithId_Self(Id);
      if (i>=0)
        {
        Ctrl_GetConnIDStr(i, ID, Tg);
        IsIn=Ctrl_In(i)!=0;
        IsOut=Ctrl_Out(i)!=0;
        }
      return i;
      };
    virtual double GetCIOValue(long Id)           { long i = CIOWithId_Self(Id); return i>=0 ? CIO_Value(i) : dNAN; };
    virtual void   SetCIOValue(long Id, double D) { long i = CIOWithId_Self(Id); if (i>=0)  SetCIO_Value(i, D); };


    long           AuditOption();
    LPCSTR         GetAuditOptionStr();
    long           SetAuditOptionStr(LPCSTR Str);
    void           GetAuditOptionStrLst(DDBValueLstMem & Lst, bool ForNode);

    // ConditionBlk Override
    DEFINE_CI(FlwNode, CTNode, 36);

    virtual void   SetXRefCI(long Id, bool On, LPCTSTR Text=NULL)
      {
      if (Text)
        {
        if (On)
          SetCI(Id+26, "%s", Text);
        else
          ClrCI(Id+26);
        }
      else
        SetCI(Id+26, On);
      };

#if NDSTOPWATCHES
    // Stop Watches
    void           AddSW()
      {
      if (pSWs==NULL)
        {
        pSWs=new FNdSWs;
        pSWs->SetSize(FSW_Max);
        }
      else
        {
        for (int i=0;i<FSW_Max; i++)
          (*pSWs)[i].Clear();
        }
      };
    void           ClrSW()        { delete pSWs; pSWs=NULL; }
    void           StartSW(int i) { if (pSWs) (*pSWs)[i].Start(); }
    void           StopSW(int i)  { if (pSWs) (*pSWs)[i].Stop(); }
    CStopWatch&    SW(int i)      { return (*pSWs)[i]; }
#endif

    void SetTopologyChanged()
      { if (pNetTopoChg) { *pNetTopoChg=1; *pSubNetTopoChg=1; };};

#if DOPBSMOOTHING
    CAMsAGDatas    MsAGDatas;
    CAMSJoins      MsJoins;
    virtual void   ClearMsIndices();
    virtual void   EvalMsAGDatas();
    virtual void   MergeMsAGDatas();
    virtual void   MarkMsIndices();
    virtual void   SetMsIndices(CMSIndexInit &Index);
    virtual void   DumpMsAGDatas();
    virtual void   AddMsEqns(SparseSolver & SS, int & Eqn);
    virtual void   AdjustMsFlows(SparseSolver & SS);
    virtual void   UpdateMsSoln(SparseSolver & SS);
#endif

    virtual dword  ModelStatus();
    static bool    GetStatusAsStopped() { return gs_Exec.Stopped() && ((m_dwMdlStatusOptions&MSO_ShowRunInEdit)==0); };
    static bool    GetStatus() { return ((m_dwMdlStatusOptions&MSO_ShowStatus)!=0); };

    SpConduit *    GetWrkConduit(char *Tag, char *IdStr);
    SpContainer *  GetWrkContainer(char *Tag, char *IdStr);

    static flag    CollectChgStats() { return fCollectChgStats; };

   FlwNode   & getArea()                { return *m_pArea; };
    _declspec(property(get=getArea))    FlwNode   & Area;

  public:// protected:
    static flag    fInConfigureJoins;
    static flag    fCollectChgStats;
    static dword   m_dwMdlStatusOptions; //for status displays
    static double  m_dStatusVapTestFrac; //for status displays
    static double  m_dStatusSolTestFrac; //for status displays

    bool           fSolutionBusy,
                   fHasFiles,
                   fIntegralDone, // Set if EvalIntegral has done its Job - Just zero derivs
                   fValidateDataComplete, // A Check
                   fActiveInMacroMdl, // A Check
                   fEvalImpCalled, // A Check
                   fEvalProductsSurgeCalled, // A Check
                   fEvalProductsCalled, // A Check
                   fEvalPressSensReqd, // Must Call EvalPressSens if set
                   fSolutionForRoot,
                   fElecConnsDone;

    int            m_nJoins, m_nRqdJoins;
    int            m_nLinks, m_nRqdLinks;
    int            m_nProcessJoins;
    int            m_nMkupJoins;
    int            m_nAuditJoins;

    int            m_nIOs;
    int            m_nFIOs;
    int            m_nProcLnkIOs;
    int            m_nProcessIOs;

    int            m_nProcDirectIOs;
    int            m_nMkupDirectIOs;

    int            m_iCIO1, m_nCIOs;
    int            m_iEIO1, m_nEIOs;
    int            m_iAIO1, m_nAIOs;

    long           NoIOs()                { return m_nIOs; };
    long           NoFlwIOs()             { return m_nFIOs; };
    long           NoProcessIOs()         { return m_nProcessIOs; };
    long           NoProcLnkIOs()         { return m_nProcLnkIOs; };

    long           NoAttachedDirectIOs()  { return m_AttachedDirectIOs.GetCount(); };
    long           NoEnabledDirectIOs()   { return m_EnabledDirectIOs.GetCount(); };

    long           NoProcDirectIOs()      { return m_nProcDirectIOs; };
    long           NoRealDirectIOs()      { return m_nProcDirectIOs+m_nMkupDirectIOs; };
    long           NoAuditDirectIOs()     { return m_EnabledDirectIOs.GetCount()-m_nProcDirectIOs-m_nMkupDirectIOs; };

    long           FirstProcDirect()      { return 0; };
    long           FirstMkupDirect()      { return m_nProcDirectIOs; };
    long           LastMkupDirect()       { return m_nProcDirectIOs+m_nMkupDirectIOs-1; };
    long           FirstAuditDirect()     { return m_nProcDirectIOs+m_nMkupDirectIOs; };
    long           LastAuditDirect()      { return m_EnabledDirectIOs.GetCount()-1; };

    long           NoCIOs()       { return m_nCIOs;};
    long           NoEIOs()       { return m_nEIOs;};
    long           NoAIOs()       { return m_nAIOs;};
    long           CIO1()         { return m_iCIO1;};
    long           CION()         { return m_iCIO1+m_nCIOs;};
    long           EIO1()         { return m_iEIO1;};
    long           EION()         { return m_iEIO1+m_nEIOs;};
    long           AIO1()         { return m_iAIO1;};
    long           AION()         { return m_iAIO1+m_nAIOs;};

    long           NoCtrls()      { return m_nCIOs+m_nEIOs+m_nAIOs;};
    long           Ctrl1()        { return m_iCIO1;};
    long           CtrlN()        { return m_iAIO1+m_nAIOs;};

    long           NoLinks()      { return m_Links.GetCount();};
    CLinkRec     * Link(int i)    { return &m_Links[i];};
    FlwBlk       * LinkFB(int i, int fe)  { return &m_Links[i].m_FBs[fe]; };
    CSpPropInfo  * LinkFwdFlwProps(int i, int fe) { return &LinkFB(i,fe)->m_FlwProps[0]; };
    CSpPropInfo  * LinkRevFlwProps(int i, int fe) { return &LinkFB(i,fe)->m_FlwProps[1]; };

    CIORecArray    m_IOs; // to allow for range checks
    CLinkArray     m_Links;
    IOAreaRec    * m_pIOAreas;
    IOAreaRec    * m_pIOAreasOwned;
#if KWIKCONNECT
    CArray<CIOAreaLcl, CIOAreaLcl&> m_IOAreaLcl;
#endif

    Strng          m_sAreaTag;
    Strng          m_sAreaDisp;
    CTODirectPtr<FlwNode> m_pArea;
    CList<FlwNode*, FlwNode*> m_NdsInArea;
    CArray<CDirectFlwIO*, CDirectFlwIO*> m_AttachedDirectIOs;
    CArray<CDirectFlwIO*, CDirectFlwIO*> m_EnabledDirectIOs;
    CList<CDirectFlwIO*, CDirectFlwIO*> m_DirectIOsConnected;
    POSITION       m_AreaNdListPos;

    JoinArray      Joins;

    pflag          pNetTopoChg;
    pflag          pSubNetTopoChg;
    int            NetNo;

    struct
      {
      double       m_X;
      double       m_Y;
      double       m_Z;
      }          m_Origin;

    double         m_Datum, m_DatumRqd;
    Z_States       m_ZState;

#if WITHSPECSHEETS
    Strng          sSpecSheet;
#endif
    Strng          sCndStrng;

    //long           m_nFlwIOsIn, m_nFlwIOsOut;     // No of FlwIOs in & out of each node
    long           m_nCtrlIOsIn, m_nCtrlIOsOut;   // No of CtrlIOs in & out of each node
    long           m_nSeqCtrlIOsIn, m_nSeqCtrlIOsOut;  // No of CtrlIOs in & out of each node
                                                  // connecting nodes actually in the CtrlSeq
    long           m_nXRefsIn, m_nXRefsOut;       // No of XRefs in & out of each node
    long           m_nSeqXRefsIn, m_nSeqXRefsOut; // No of XRefs in & out of each node
                                                  // connecting nodes actually in the CtrlSeq
    long           m_nElecIOsIn, m_nElecIOsOut;     // No of ElecIOs in & out of each node
    long           m_nCAirIOsIn, m_nCAirIOsOut;     // No of ElecIOs in & out of each node

    //long           m_nFlwIOsInReqd, m_nFlwIOsInDone;
    long           m_nCtrlsInReqd, m_nCtrlsInDone;
    long           m_nPwrIOsInReqd, m_nPwrIOsInDone;

    flag           m_InPwrCtrlSeq;
    //flag           m_InPwrSeq;
    //long           m_lPrevXRefUpdates;

    //CIArray        m_FlwDependentIOs;
    CIArray        m_CtrlDependentIOs;
    CIArray        m_PwrDependentIOs;

    CJoinClusterArray m_Clusters;

    long           m_AuditOption;
    long           m_ForceAuditOption;

    CExternAuditCalc m_ExtAudit;

#if NDSTOPWATCHES
    FNdSWs         *pSWs;
#endif

    dword          dwPrevStatus;

    byte           m_iEvalPosn;
    Strng          m_sEvalOrder;
    //long           m_iProcOrder;
    long           m_iCtrlOrder;
    long           iAllIndex;

    flag           fMarked;
    flag           fNegPress;


    CClosureInfoArray m_Closure;

    CArray<CPwrUser*, CPwrUser*> m_PwrUsers;

    CMap<CString, LPCTSTR, SpConduit*, SpConduit*> WrkConduits;
    CMap<CString, LPCTSTR, SpContainer*, SpContainer*> WrkContainers;

    CRITICAL_SECTION m_LockSect;


  };

// ---------------------------------------------------------------------------

class StkSpConduit
  {
  protected:
    SpConduit * m_pConduit;
  public:
    StkSpConduit(SpConduit * pConduit) { m_pConduit=pConduit; };
    StkSpConduit(char * Tag, char *IdStr, FlwNode * pNd) { m_pConduit=pNd->GetWrkConduit(Tag, IdStr); };
    SpConduit & operator()() { return *m_pConduit; };
    SpConduit * operator->() { return m_pConduit; };
    //operator SpConduit&() { return *m_pConduit; };
  };

class StkSpContainer
  {
  protected:
    SpContainer * m_pContainer;
  public:
    StkSpContainer(SpContainer * pContainer) { m_pContainer=pContainer; };
    StkSpContainer(char * Tag, char *IdStr, FlwNode * pNd) { m_pContainer=pNd->GetWrkContainer(Tag, IdStr); };
    SpContainer & operator()() { return *m_pContainer; };
    SpContainer *  operator->() { return m_pContainer; };
  };

#define chLINEID() __FILE__ "." chSTR2(__COUNTER__)

//---------------------------------------------------------------------------

XID xidSysCADSolutionOld          = FlwXID(1);
XID xidNetMethod                  = FlwXID(2);
XID xidSolveMethod                = FlwXID(3);
XID xidHeatMethod                 = FlwXID(4);
XID xidFlowMode                   = FlwXID(5);
XID xidFlowModeRqd                = FlwXID(7);
XID xidFlowModeSet                = FlwXID(8);
XID xidAuditReqd                  = FlwXID(9);
XID xidActiveMode                 = FlwXID(10);
XID xidHoldMode                   = FlwXID(11);
XID xidSolveState                 = FlwXID(12);
XID xidSysCADActive               = FlwXID(13);
XID xidSpecSheet                  = FlwXID(14);
XID xidEvalOrder                  = FlwXID(15);
XID xidNdCondition                = FlwXID(16);
XID xidNdConditionCount           = FlwXID(17);
XID xidSetState                   = FlwXID(18);
XID xidNdNJoins                   = FlwXID(19);
XID xidNdNIOs                     = FlwXID(20);
XID xidMarked                     = FlwXID(21);
XID xidFlwNdRqdArea               = FlwXID(22);

XID xidSrcIOTag                   = FlwXID(40);
XID xidDstIOTag                   = FlwXID(41);

XID xidSetStateZeroFlows          = FlwXID(50);
XID xidSetStateEmpty              = FlwXID(51);
XID xidSetStatePreSet             = FlwXID(52);
XID xidSetStateSteadyState        = FlwXID(53);
XID xidSetStatePBInit             = FlwXID(54);
XID xidSetStateDynStatsRunInit    = FlwXID(55);
XID xidSetStateEmptySpillTargets  = FlwXID(56);

XID xidFBMeanTemp                 = FlwXID(30000);  // User Info passes Additional Info
XID xidFBMeanPress                = FlwXID(30001);  // User Info passes Additional Info
XID xidFBMeanRho                  = FlwXID(30002);  // User Info passes Additional Info
XID xidFBRise                     = FlwXID(30003);  // User Info passes Additional Info
XID xidFBActLen                   = FlwXID(30004);  // User Info passes Additional Info
XID xidFBFitLen                   = FlwXID(30005);  // User Info passes Additional Info
XID xidFBRqdLen                   = FlwXID(30006);  // User Info passes Additional Info

XID xidFBFirst                    = FlwXID(30000);  
XID xidFBLast                     = FlwXID(30009);  

XID xidIOSeqStartOK               = FlwXID(30010);
XID xidIORmtSeqStartOK            = FlwXID(30011);
XID xidIOSeqStart                 = FlwXID(30012);

XID xidIOSeqFirst                 = FlwXID(30010);  
XID xidIOSeqLast                  = FlwXID(30019);  

XID xidElevation                  = FlwXID(30020);

XID xidJoinNetInfo                = FlwXID(30100);  // User Info passes Additional Info
XID xidFRBMeasRho                 = FlwXID(30200);
XID xidFRBMeasRhoStep             = FlwXID(30201);

//===========================================================================

class DllImportExport NodeDrawDef
  {
  public:
    NodeDrawDef();
    ~NodeDrawDef();
    NodeGrfInfo*   FindGrfInfo();
    long           AppendDrawing(char* ClassName, char * DrwGroup, double * Drawing);

  protected:
    NodeGrfInfo*   pNdDrw;
    long           Len;
  };

//---------------------------------------------------------------------------

class DllImportExport NodeDrawEntry
  {
  public:
    NodeDrawEntry(char* pClassName, char* pVersion, char * pDrwGroup, double * pDrawing);
    ~NodeDrawEntry();
    NodeDrawEntry * Next()        { return pNext; };
    char          * ClassName()   { return m_ClassName(); };
    char          * DrwGroup()    { return m_DrwGroup(); };
    double        * Drawing()     { return m_pDrawing; };

  protected:
    NodeDrawEntry * pNext;
    double        * m_pDrawing;
    Strng           m_ClassName;
    Strng           m_DrwGroup;

  public:
    static NodeDrawEntry* pFirst;
  };

//===========================================================================

#define BOX(x,y,w,h) x,y, x,y+h, x+w,y+h, x+w,y, x,y
#define DEFINE_NODE_DRAWING(Class,Name, Drawing)

//===========================================================================

inline FlwNode*       CJoinRec::Nd()                     { return pNd; };
inline double         CJoinRec::Datum()                  { return pNd->m_Datum; };
inline flag           CJoinRec::InFixedNet()             { return fInFixedNet; };
inline flag           CJoinRec::InXferNet()              { return fInXferNet; };
inline flag           CJoinRec::IsXferLnk()              { ASSERT(m_nConns==2); return IOFB(0,0)->IsXfer()||IOFB(1,0)->IsXfer(); };
inline flag           CJoinRec::IsFixedLnk()             { ASSERT(m_nConns==2); return IOFB(0,0)->IsFixed()||IOFB(1,0)->IsFixed(); };

inline int            CJoinRec::NConns()                 { return m_nConns; };
inline CJoinRec*       CJoinRec::Join_Rmt(int c)          { CJConnectRec & C=m_Conns[c]; return C.m_iRmtJoin>=0 ? &C.m_pRmtNd->Joins[C.m_iRmtJoin] : NULL; };
inline flag           CJoinRec::Conn_Open(int c)         { return m_Conns[c].m_iRmtJoin<0; };
inline flag           CJoinRec::Connected(int c)         { return m_Conns[c].m_iRmtJoin>=0; };
inline flag           CJoinRec::Conn_ToLink(int c)       { return m_Conns[c].m_iRmtJoin>=0 && Join_Rmt(c)->fIs2IO; };
inline flag           CJoinRec::Conn_ToTie(int c)        { return m_Conns[c].m_iRmtJoin>=0 && Join_Rmt(c)->fIsTie; };
inline CJConnectRec & CJoinRec::Conn(int c)              { return m_Conns[c]; };
inline CJConnectRec & CJoinRec::Conn_Rmt(int c)          { return Join_Rmt(c)->m_Conns[m_Conns[c].m_iRmtConn]; };
inline int            CJoinRec::ConnNo_Rmt(int c)        { return m_Conns[c].m_iRmtConn; };

inline FlwNode      * CJoinRec::Nd_Rmt(int c)            { return m_Conns[c].m_pRmtNd; };
inline flag           CJoinRec::IsIO(int c)              { return m_Conns[c].m_iIONo>=0; };
inline int            CJoinRec::IONo(int c)              { return m_Conns[c].m_iIONo; };
inline int            CJoinRec::IONo_Rmt(int c)          { return pNd->IOIONo_Rmt(m_Conns[c].m_iIONo); };
inline int            CJoinRec::QSgn(int c)              { return m_Conns[c].m_iQSgn; };

inline CFlange      * CJoinRec::Flng(int c)              { return m_Conns[c].m_pFlng; };
inline int            CJoinRec::NIOFBs(int c)            { return m_Conns[c].m_pFBs->GetSize(); };
inline int            CJoinRec::NIOFBs_Rmt(int c)        { return Conn_Rmt(c).m_pFBs->GetSize(); };
inline int            CJoinRec::IOFBJoinIndex(int c)     { return NIOFBs(c)-1;};
inline int            CJoinRec::IOFBFlngIndex(int c)     { return 0;};
inline FlwBlk       * CJoinRec::IOFB(int c, int fe)      { return &(*m_Conns[c].m_pFBs)[fe]; };
inline FlwBlk       * CJoinRec::IOFBJoin(int c)          { return &(*m_Conns[c].m_pFBs)[NIOFBs(c)-1]; };
inline FlwBlk       * CJoinRec::IOFBFlng(int c)          { return &(*m_Conns[c].m_pFBs)[0]; };
inline FlwBlk       * CJoinRec::IOFB_Rmt(int c, int fe)  { return Nd_Rmt(c)->IOFB(IONo_Rmt(c), fe); };
inline FlwBlk       * CJoinRec::IOFBJoin_Rmt(int c)      { return Nd_Rmt(c)->IOFB(IONo_Rmt(c), NIOFBs_Rmt(c)); };
inline FlwBlk       * CJoinRec::IOFBFlng_Rmt(int c)      { return Nd_Rmt(c)->IOFB(IONo_Rmt(c), 0); };
inline CIORec        * CJoinRec::IO_Self(int c)           { return &(pNd->m_IOs[m_Conns[c].m_iIONo]); };
inline CIORec        * CJoinRec::IO_Rmt(int c)            { return &Nd_Rmt(c)->m_IOs[IONo_Rmt(c)];};

inline int            CJoinRec::NIOPBs(int c)            { return m_Conns[c].m_pPBs->GetSize(); };
inline int            CJoinRec::NIOPBs_Rmt(int c)        { return Conn_Rmt(c).m_pPBs->GetSize(); };
inline CPressBlk    * CJoinRec::IOPB(int c, int fe)      { return &(*m_Conns[c].m_pPBs)[fe]; };
inline CPressBlk    * CJoinRec::IOPB_Self(int c)         { return &(*m_Conns[c].m_pPBs)[NIOPBs(c)-1]; };
inline CPressBlk    * CJoinRec::IOPB_Flng(int c)         { return &m_Conns[c].m_pFlng->PB; };
inline CPressBlk    * CJoinRec::IOPB_Rmt(int c, int fe)  { return &Nd_Rmt(c)->IOPB(IONo_Rmt(c), fe); };
//inline CPressBlk    * CJoinRec::IOPBJoin_Rmt(int c)      { return &Nd_Rmt(c)->IOPB(IONo_Rmt(c), NIOPBs_Rmt(c)); };
//inline CPressBlk    * CJoinRec::IOPB_Flng_Rmt(int c)      { return &Nd_Rmt(c)->IOPB(IONo_Rmt(c), 0); };

inline flag           CJoinRec::IsLink(int c)            { return m_Conns[c].m_iLnkNo>=0; };
inline int            CJoinRec::LinkNo(int c)            { return m_Conns[c].m_iLnkNo; };
inline FlwBlk       * CJoinRec::LinkFB(int c)            { return &(*m_Conns[c].m_pFBs)[0]; };
inline CPressBlk    * CJoinRec::LinkPB(int c, int i)     { return &(*m_Conns[c].m_pPBs)[i]; };
inline CLinkRec     * CJoinRec::Link(int c)              { return &pNd->m_Links[m_Conns[c].m_iLnkNo]; };
inline CPressBlk    * CJoinRec::LinkPB_Self(int c)       { CLinkRec & L=Nd()->m_Links[LinkNo(c)]; return &L.PB(L.m_iJoinId0==m_iJoinId ? 0 : 1); };
inline CPressBlk    * CJoinRec::LinkPB_Rmt(int c)        { CLinkRec & L=Nd()->m_Links[LinkNo(c)]; return &L.PB(L.m_iJoinId0==m_iJoinId ? 1 : 0); };

inline int            CJoinRec::NFBs(int c)              { return m_Conns[c].m_pFBs->GetSize(); };
inline FlwBlk       * CJoinRec::FB(int c, int fe)        { return &(*m_Conns[c].m_pFBs)[fe]; };

inline FlwBlk       * CJoinRec::FB_Self(int c)           { return IsIO(c) ? IOFBJoin(c) : LinkFB(c); }
inline int            CJoinRec::FB_SelfSgn(int c)        { return IsIO(c) ? 1 : (Nd()->m_Links[LinkNo(c)].m_iJoinId0==m_iJoinId ? -1:1); }
inline double         CJoinRec::FBQm_In(int c)           { return IsIO(c) ? IOFBJoin(c)->m_Qm : LinkFB(c)->m_Qm*(Nd()->m_Links[LinkNo(c)].m_iJoinId0==m_iJoinId ? -1:1); }

inline CPressBlk    * CJoinRec::PB_Self(int c)           { return IsIO(c) ? IOPB(c, NIOPBs(c)-1) : LinkPB_Self(c); }
inline CPressBlk    * CJoinRec::PB_Rmt(int c)            { return IsIO(c) ? IOPB_Rmt(c, NIOPBs(c)-1) : LinkPB_Rmt(c); }

#if defined(WITH_GFB)
inline CGrpFlwBlk   * CJoinRec::GFB(int c)               { return (*m_Conns[c].m_pFBs)[0].m_pGFb; };
inline CGrpFlwBlk   * CJoinRec::GFB_Rmt(int c)           { return (*Conn_Rmt(c).m_pFBs)[0].m_pGFb; };
#else
inline void         * CJoinRec::GFB(int c)               { return (*m_Conns[c].m_pFBs)[0].m_pGFb; };
inline void         * CJoinRec::GFB_Rmt(int c)           { return (*Conn_Rmt(c).m_pFBs)[0].m_pGFb; };
#endif
inline flag           CJoinRec::Placed(int c)            { return (*m_Conns[c].m_pFBs)[0].m_bPlaced; };
inline flag           CJoinRec::Placed_Rmt(int c)        { return (*Conn_Rmt(c).m_pFBs)[0].m_bPlaced; };

inline double         CJoinRec::Pressure()               { return m_PB.P;};
inline void           CJoinRec::SetPressure(double P_)   { m_PB.P=P_; };
inline CPressBlk    * CJoinRec::PB()                     { return &m_PB;};

inline void           CJoinRec::SetQm_PressRes(double Press, double Res) { pQmAux->SetQm_PressRes(Press, Res); };
inline void           CJoinRec::SetQm_FixedQm (double RqdQm)             { pQmAux->SetQm_FixedQm (RqdQm); };
inline void           CJoinRec::SetQm_QmRatio (double Ratio)             { pQmAux->SetQm_QmRatio (Ratio); };
inline void           CJoinRec::Set_IORatio (int IONo, double Ratio)     { pQmAux->Set_IORatio (IONo, Ratio); };

inline void           CJoinRec::Set_IOP_Self(int c, double p)     { Nd()->IOPB_Self(IONo(c)).P=p; };
inline void           CJoinRec::Set_IOP_Flng(int c, double p)     { Nd()->IOPB_Flng(IONo(c)).P=p; };
inline void           CJoinRec::Set_IOP_Rmt(int c, double p)      { Nd()->IOPB_Rmt(IONo(c)).P=p; };
inline double         CJoinRec::IOP_Self(int c)                   { return Nd()->IOPB_Self(IONo(c)).P; };
inline double         CJoinRec::IOP_Flng(int c)                   { return Nd()->IOPB_Flng(IONo(c)).P; };
inline double         CJoinRec::IOP_Rmt(int c)                    { return Nd()->IOPB_Rmt(IONo(c)).P; };

//inline void           CJoinRec::Set_IOP_Ref_Self(int c, double P) { Nd()->IOPB_Self(IONo(c)).PRef=(float)P; };
//inline void           CJoinRec::Set_IOP_Ref_Flng(int c, double P) { Nd()->IOPB_Flng(IONo(c)).PRef=(float)P; };
//inline void           CJoinRec::Set_IOP_Ref_Rmt(int c, double P)  { Nd()->IOPB_Rmt(IONo(c)).PRef=(float)P; };
//inline double         CJoinRec::IOP_Ref_Self(int c)               { return Nd()->IOPB_Self(IONo(c)).PRef; };
//inline double         CJoinRec::IOP_Ref_Flng(int c)               { return Nd()->IOPB_Flng(IONo(c)).PRef; };
//inline double         CJoinRec::IOP_Ref_Rmt(int c)                { return Nd()->IOPB_Rmt(IONo(c)).PRef; };

inline void           CJoinRec::Set_IOP_Est_Self(int c, double P) { Nd()->IOPB_Self(IONo(c)).PEst=P; };
inline void           CJoinRec::Set_IOP_Est_Flng(int c, double P) { Nd()->IOPB_Flng(IONo(c)).PEst=P; };
inline void           CJoinRec::Set_IOP_Est_Rmt(int c, double P)  { Nd()->IOPB_Rmt(IONo(c)).PEst=P; };
inline double         CJoinRec::IOP_Est_Self(int c)               { return Nd()->IOPB_Self(IONo(c)).PEst; };
inline double         CJoinRec::IOP_Est_Flng(int c)               { return Nd()->IOPB_Flng(IONo(c)).PEst; };
inline double         CJoinRec::IOP_Est_Rmt(int c)                { return Nd()->IOPB_Rmt(IONo(c)).PEst; };

inline double         CJoinRec::IOP_Max_Self(int c)               { return Nd()->IOPB_Self(IONo(c)).PMax; };
inline double         CJoinRec::IOP_Max_Flng(int c)               { return Nd()->IOPB_Flng(IONo(c)).PMax; };
inline double         CJoinRec::IOP_Max_Rmt(int c)                { return Nd()->IOPB_Rmt(IONo(c)).PMax; };

inline double         CJoinRec::IOP_MaxSet_Self(int c)               { return Nd()->IOPB_Self(IONo(c)).PMaxLim; };
inline double         CJoinRec::IOP_MaxSet_Flng(int c)               { return Nd()->IOPB_Flng(IONo(c)).PMaxLim; };
inline double         CJoinRec::IOP_MaxSet_Rmt(int c)                { return Nd()->IOPB_Rmt(IONo(c)).PMaxLim; };


// ===========================================================================
//
//
//
//===========================================================================
#if DOPBSMOOTHING

//inline int CMSIO::Index() { return m_pMS->m_iIndex; };
inline CMSIndex * CMSIO::MSIndex() { return m_pNd->IOMSIndex(m_iIONo);};//pMS; }
inline FlwNode * CMSIO::Nd() { return m_pNd; };
inline int CMSIO::SpIndex(int SpNo) { return MSIndex()->SpIndex()[SpNo]; };
inline void CMSIO::SetSpIndex(int SpNo, int Inx) { MSIndex()->SpIndex()[SpNo]=Inx; };
inline void CMSIO::SetSpIndexRqd(int SpNo, flag Mrgd) { int &i=MSIndex()->SpIndex()[SpNo]; i=Max(i,Mrgd?-1:0); };
inline FlwNode * CMSIO::Nd_Rmt() { return m_pNd->Nd_Rmt(m_iIONo); };

//inline void CMSIO::Switch(CMSIO &MSIO)  { pFlng->MS.Switch(pThis=&This; };
#endif

// ===========================================================================
//
//
//
//===========================================================================

#define MAKEIOTG4DDB(Tg, i, fe)                                              \
  {                                                                          \
  if (PrjFileVerNo()<69)                                                     \
    {                                                                        \
    if (fe>0)                                                                \
      Tg.Set("%s.%i", Nd_Rmt(i)->Tag(), fe);                                \
    else                                                                     \
      Tg=Nd_Rmt(i)->Tag();                                                   \
    }                                                                        \
  else                                                                       \
    {                                                                        \
    if (fe>0)                                                                \
      Tg.Set("%s.%s.%i", Nd_Rmt(i)->Tag(), IODesc_Rmt(i)->pName, fe);       \
    else                                                                     \
      Tg.Set("%s.%s", Nd_Rmt(i)->Tag(), IODesc_Rmt(i)->pName);               \
    }                                                                        \
  }
#define MAKELNKTG4DDB(Tg, i, fe)                                             \
  {                                                                          \
  Tg.Set("Lnk.%i.%i", i, fe);                                               \
  }

// ===========================================================================
//
//
//
//===========================================================================

inline SpConduit & CDirectFlwIO::getConduit()       { return *m_pFlng->Conduit(false); };

inline FlwNode & CNodeEvalIndex::getNd()            { return *m_pNd; };
inline CJoinCluster & CNodeEvalIndex::getCluster()  { return m_pNd->m_Clusters[m_iCluster]; };

// ===========================================================================
//
//
//
//===========================================================================

//inline void FlwBlk::SetXferMode()
//  {
//  if (!m_fFixed || !m_fXfer)
//    {
//    //SetTopologyChanged();
//    //DBGTOPOCHGFB("SetXferMode", m_iNo);
//    }
//  m_QmRqd           = dNAN;
//  m_QmReg           = 1.0;
//  m_QmXfCap         = m_iDirnRel2Connect*MaxXfCap;
//  m_fFixed          = true;
//  m_fXfer           = true;
//  m_fIsDegFree      = true;
//  #if (0||dbgTrackQmModes)
//  dbgpln("%-20s fb%04i", "SetXferMode", m_iNo);
//  #endif
//  };
//
//inline void FlwBlk::SetXferCapacity(double QmCap)
//  {
//  m_QmRqd           = dNAN;
//  m_QmReg           = 1.0;
//  m_QmXfCap         = QmCap;
//  };
//
//inline void FlwBlk::SetQmReqd(double QmReqd, flag IsFree, float MaxNode, float MaxFlng)
//  {
//  if (!m_fFixed || m_fXfer)
//    {
//    SetTopologyChanged();
//    DBGTOPOCHGFB("SetQmReqd", m_iNo);
//    }
//  m_QmRqd           = QmReqd;
//  m_QmReg           = 1.0;
//  m_QmXfCap         = dNAN;
//  m_fFixedVlv       = false;
//  m_fFixed          = true;
//  m_fXfer           = false;
//  m_fIsDegFree      = IsFree;
//  NodePB().PMaxLim  = MaxNode;
//  FlngPB().PMaxLim  = MaxFlng;
//  #if (0||dbgTrackQmModes)
//  dbgpln("%-20s fb%04i", "SetQmReqd", m_iNo);
//  #endif
//  };
//
//inline void FlwBlk::SetQmReqdVlv(double QmReqd, double Reg, flag IsFree, float MaxNode, float MaxFlng)
//  {
//  if (!m_fFixed || m_fXfer)
//    {
//    SetTopologyChanged();
//    DBGTOPOCHGFB("SetQmReqdVlv", m_iNo);
//    }
//  m_QmRqd           = QmReqd;//*Reg;
//  m_QmReg           = Reg;
//  m_fFixedVlv       = true;
//  m_fFixed          = true;
//  m_fXfer           = false;
//  m_fIsDegFree      = IsFree;
//  NodePB().PMaxLim  = MaxNode;
//  FlngPB().PMaxLim  = MaxFlng;
//  #if (0||dbgTrackQmModes)
//  dbgpln("%-20s fb%04i", "SetQmReqdVlv", m_iNo);
//  #endif
//  };
//
//inline void FlwBlk::SetQmFree()
//  {
//  if (m_fFixed || m_fXfer)
//    {
//    SetTopologyChanged();
//    DBGTOPOCHGFB("SetQmFree", m_iNo);
//    }
//  m_QmRqd           = dNAN;
//  m_QmReg           = 1.0;                                          
//  m_QmXfCap         = dNAN;
//  m_fFixed          = false;
//  m_fXfer           = false;
//  m_fIsDegFree      = false;
//  NodePB().PMaxLim  = fNAN;
//  FlngPB().PMaxLim  = fNAN;
//  #if (0||dbgTrackQmModes)
//  dbgpln("%-20s fb%04i", "SetQmFree", m_iNo);
//  #endif
//  };
//
////-------------------------------------------------------------------------
//
//inline void FlwBlk::SetBiDirectionalFlow()
//  {
//  ASSERT(FlwNode::fInConfigureJoins);
//  UDF.OpenP=1.0;
//  UDF.CloseP=0.0;
//  UDF.DirnAllowed=UDFD_Both;
//  UDF.ActionCount=0;
//  UDF.Restrict=0.0;
//  UDF.Action=UDFA_None;
//  };
//
////-------------------------------------------------------------------------
//
//inline void FlwBlk::SetUniDirectionalFlow(flag Fwd, double OpenPress, double ClosePress)
//  {
//  ASSERT(FlwNode::fInConfigureJoins);
//  UDF.OpenP=OpenPress;
//  UDF.CloseP=ClosePress;
//  UDF.DirnAllowed=Fwd ? UDFD_Fwd : UDFD_Rev;
//  };
//
////-------------------------------------------------------------------------

inline double FlwBlk::RequiredQm()
  {
  return m_fFixed && m_fXfer ? m_QmXfCap : m_QmRqd;
  };

// ===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif


