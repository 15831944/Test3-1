//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLWSOLVE_H
#define  __FLWSOLVE_H

#ifndef _AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __STDIO_H
  #include <stdio.h>
#endif
#ifndef __FLWNETS_H
  #include "FlwNets.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif
#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __SFE_BASE_H
  #include "sfe_base.h"
#endif
#ifndef __SIGFLW_H
  #include <sigflw.h>
#endif
#ifndef __EQNSOLVE_H
  #include "eqnsolve.h"
#endif
#ifndef __TKNPARSE_H
  #include "tknparse.h"
#endif
#ifndef __FLWSLVOB_H
  #include "flwslvob.h"
#endif
#ifndef __ODESOLVE_H
  #include "odesolve.h"
#endif

#ifdef __FLWSOLVE_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
// Command Identifiers

#define MAXTRACES 512

// ==========================================================================
//
//
//
// ==========================================================================

DEFINE_TAGOBJ(CFlwSolver)
DEFINE_TAGOBJ(FlwSlvPB)
DEFINE_TAGOBJ(FlwSlvDyn)

// ==========================================================================
//
//
//
// ==========================================================================

typedef CLockedListIter<CFlwNodeList, FlwNode*&> CLkFlwNodeIter;
typedef CLockedListIter<CNodeEvalIndexList, CNodeEvalIndex&> CLkNodeEvalIter;

class CFlwNodeListArray : public CArray <CFlwNodeList, CFlwNodeList&> { };
class CFlwNodeListODE   
  { 
  public: 
    CFlwNodeListODE(CFlwNodeList & List, CODEDataBlock & ODB) :
    m_List(List),  
    m_ODB (ODB)
      {
      };
    CFlwNodeList & m_List;  
    CODEDataBlock & m_ODB; 
  };

const byte FTS_DynMode = 0x01;
const byte FTS_ThrdSeq = 0x02;
const byte FTS_Order   = 0x04;

const byte TOSL_Proc   = 0x01;
const byte TOSL_Ctrl   = 0x02;
const byte TOSL_All    = 0x03;


_FWDDEF(FlwSlvBlk)
class FlwSlvBlk  
  {
  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    flag           CreateSolutionBlks(int NRqdThreads);

    int            ClearSolutionBlks(flag ClearAll);

    virtual flag   PreStartCheckAll();
    virtual flag   InitialiseSolutionAll();
    virtual flag   TerminateSolutionAll();
    virtual void   StartSolutionAll();
    virtual void   ConfigureJoinsAll();

    flag           BuildMacroMdls();
    void           DeleteMacroMdls();

    flag           BuildSignalFlowGraph(flag DoingDynMode, byte Which);
    void           SetUpTearLinks(CSFGNodeInfoList & TearNodeList);
    void           SetUpTearCLinks(CSFGNodeInfoList & TearNodeList);
    void           AddToEvalOrder(pFlwNode p, pFlwNode pSrc, sint ReqdDirn, int NdStackIndex);

    virtual bool   BuildCtrlNdListReqd();
    virtual void   BuildCtrlNdList();

    flag           FindTOSequenceStart(byte Which);
    void           BuildTOSequence(byte Which);
    void           SummariseTOSequence(byte Which);
    flag           SolveTOSequenceThrd(int What, byte Which, LPVOID OtherData=NULL);

    void           FindTOSequence(byte Flags, byte Which);
    void           DumpEvalOrder(byte Which);

    void           InitTearConvergence(EqnSlvCtrlBlk & EqnCB);
    flag           TestTearConvergence(long ConvergeLoopCnt, EqnSlvCtrlBlk & EqnCB, Strng_List &BadTearTags, Strng_List & BadTearInfo);
    void           TransferAllTears(EqnSlvCtrlBlk & EqnCB);
    void           InitialiseAllTears(EqnSlvCtrlBlk & EqnCB);
    void           RestartAllTears(EqnSlvCtrlBlk & EqnCB);

    double         TestBalance(const EqnSlvCtrlBlk & TCtrl, CNodeEvalIndexList &Nds, Strng_List * pTBReport=NULL);

    void           ShowFlows(CNodeEvalIndexList &Nds);


    void           FITConfigureJoins(CFlwNodeList & List);//CLkFlwNodeIter & Iter);

    void           EvalAllElectrics(bool ConvergeIt);
    void           EvalAllPowerRequired();

    void           InitClosure();
    void           TestClosure(CNodeEvalIndexList &TOSequence);
    void           MarkClosureDataAvail(CNodeEvalIndexList &List);
    void           MarkClosureDataInvalid(CFlwNodeList &List);

    void           FixAllNdsLists();

  public:
    CFlwSolver      &FlwSlvNd;

    flag                m_SolutionBlksOk;
    CFlwNodeList        m_AllNds;
    CFlwNodeList        m_ExcNds;
    CFlwNodeList        m_ProcessNds;
    CFlwNodeList        m_PwrCtrlNds;
    CFlwNodeList        m_StandAloneNds;
    CFlwNodeListArray   m_ProcessNdsLA;
    CFlwNodeListArray   m_AllNdsLA;

    flag                m_fProcTOSeqValid;
    flag                m_fCtrlTOSeqValid;
    int                 m_iProcTOSequence;
    CNodeEvalIndexList  m_ProcTOSequence[2];
    CNodeEvalIndexList  m_CtrlTOSequence;
    CNodeEvalIndexList  m_AllTOSequence;
    CFlangeList         m_Flanges,   m_TearFlanges;
    CTerminalList       m_Terminals, m_TearTerminals;
    CXRefPtrList        m_TearXRefs;

    void                ClearProcTOSequence();
    void                SwapAndClearProcTOSequence();
    CNodeEvalIndexList & getProcTOSequence()      { return m_ProcTOSequence[m_iProcTOSequence]; };
    CNodeEvalIndexList & getProcTOSequencePrev()  { return m_ProcTOSequence[(m_iProcTOSequence+1)%2]; };
    _declspec(property(get=getProcTOSequence))     CNodeEvalIndexList & ProcTOSequence;
    _declspec(property(get=getProcTOSequencePrev)) CNodeEvalIndexList & ProcTOSequencePrev;

    MacroMdlList        MacroMdls;

    CSignalFlowGraph    m_ProcSigFlwGrf;
    CSignalFlowGraph    m_CtrlSigFlwGrf;
    CNodeEvalIndexList  m_TOList[4], m_TOListWork[4];
    
    CNodeEvalIndexList  & m_ProcTOList1, & m_ProcTOList2;
    CNodeEvalIndexList  & m_CtrlTOList1, & m_CtrlTOList2;

    CNodeElectricsArray m_TSCA;

    CLkNodeEvalIter     m_TOListIter[4];

    FlwSlvBlk(CFlwSolver * pFlwSlvNd);
    virtual ~FlwSlvBlk();

    Strng_List     TBReport;
  };

// ==========================================================================
//
//
//
// ==========================================================================

const int NDS_LapTime       = 0;
const int NDS_LapTimeFrac   = 1;
const int NDS_LapCount      = 2;
const int NDS_TotalTime     = 3;
const int NDS_TotalTimeFrac = 4;
const int NDS_TotalCounts   = 5;

class FlwSlvPB : public FlwSlvBlk, public CTNode
  {
  public:
    FlwSlvPB(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    FlwSlvPB(CFlwSolver * pFlwSlvNd);
    virtual ~FlwSlvPB();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   BuildDataDefnOld(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };

    flag           CreateSolutionBlks(int NRqdThreads);//flag fProBal_);

    int            ClearSolutionBlks(flag ClearAll);

    void           DoSolnInit(bool DoReInit);
    int            InitSolution(const EqnSlvCtrlBlk & EQCtrl, CFlwNodeList &AllNdsIn);
    int            StepSolution(const EqnSlvCtrlBlk & EQCtrl);
    int            TermSolution(const EqnSlvCtrlBlk & EQCtrl);
    int            GetSolutionError(const EqnSlvCtrlBlk & EQCtrl, Strng & RptLine);

    //void           TestClosure(CFlwNodeList &TOSequence);

    void           FITPBEvalMakeUpAvail(CLkNodeEvalIter & Iter);
    void           FITPBEvalMakeUpReqd(CLkNodeEvalIter & Iter);
    void           FITPBEvalPressures(CLkNodeEvalIter & Iter);
    void           FITPBEvalProducts(CLkNodeEvalIter & Iter);
    void           FITPBEvalDiscrete(CFlwNodeList & List);//CLkFlwNodeIter & Iter);
    void           FITPBEvalCtrlActions(CFlwNodeList & List);//CLkFlwNodeIter & Iter);
    void           FITPBEvalCtrlStrategy(CFlwNodeList & List);//CLkFlwNodeIter & Iter);

    void           EvalAllDiscrete();
    void           EvalAllBalance();
    void           EvalAllCtrlInitialise(eScdCtrlTasks Tasks);
    void           EvalAllCtrlStrategy(eScdCtrlTasks Tasks);
    void           EvalAllCtrlActions(eScdCtrlTasks Tasks);
    void           EvalAllCtrlTerminate(eScdCtrlTasks Tasks);
    //bool           EvalAllElectrics(long What);
//    virtual void   EvalAllPowerAvailable();
//    virtual void   EvalAllPowerRequired();

#if DOPBSMOOTHING  
    SparseSolver * pSSMS;
    int            m_nMtxOrder;
    void           SolvePBSmoothingInitial(const EqnSlvCtrlBlk & EQCtrl);
    void           SolvePBSmoothingEstimate(const EqnSlvCtrlBlk & EQCtrl);
    void           SolvePBSmoothingEvaluate(const EqnSlvCtrlBlk & EQCtrl);
#endif

    void           DumpNdStats(int What, double TotTime);

    flag           fLogClearWasOn;

    CTimeStateBlock IntCB;
    
    CFlwNodeList    LnkNds;
  };

// ==========================================================================
//
//
//
// ==========================================================================

// Control Evaluation
const byte ECA_Start=0;
const byte ECA_End=1;

_FWDDEF(FlwSlvBlk)
class FlwSlvDyn : public FlwSlvBlk , public ODESolverBase, public CTNode
  {
  public:
    FlwSlvDyn(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    FlwSlvDyn(CFlwSolver * pFlwSlvNd);
    virtual ~FlwSlvDyn();
    void Construct();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   BuildDataDefnOld(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  TagOfParent() { return PlantModelTag; };

    flag           CreateSolutionBlks(int NRqdThreads);//flag fProBal_);

    int            Dynamic_Setup(double Time_Stop_, double Time_Target_, double Time_Step_, flag ToSteadyState_, flag SolveOn);

    int            ClearSolutionBlks(flag ClearAll);

    virtual double StartTime(void) {return ODE().GetTimeStepStart();};
    virtual double Time(void) {return IntCB.GetTime();};
    virtual double dTime(void) {return IntCB.GetTimeInc();};
    virtual void   EvalAllStates();

    // ODEBase Overides
    virtual void   ODEStartStep();
    virtual void   ODEOperate(CODEDataBlock & ODB);
    virtual void   ODEDerivs();
    
    virtual void   EvalAllDiscrete();
    virtual void   EvalAllBalance();
    virtual void   EvalAllCtrlInitialise(eScdCtrlTasks Tasks);
    virtual void   EvalAllCtrlActions(eScdCtrlTasks Tasks);
    virtual void   EvalAllCtrlStrategy(eScdCtrlTasks Tasks);
    void           EvalAllCtrlTerminate(eScdCtrlTasks Tasks);
    virtual void   EvalAllStatistics(eScdCtrlTasks Tasks);
    //bool           EvalAllElectrics(long What);
    //virtual void   EvalAllPowerAvailable();
    //virtual void   EvalAllPowerRequired();

    flag           QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    flag           Start(CXM_TimeControl &CB, CFlwNodeList &AllNdsIn);
    flag           ReStart();

    flag           Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    void           CheckStopConditions(CXM_TimeControl &CB);
    flag           CheckStopRequired(CXM_TimeControl &CB, bool Starting, bool ShowMsgs);
    flag           Stop(CXM_TimeControl &CB);
    flag           PartialStop();
    //flag           FlowsSteady() {return ;};

    void           DumpRunStats(flag Total);
    void           DumpNdStats(int What);

//    void           FITConfigureJoins(CLkFlwNodeIter & Iter);
    void           FITPowers(CLkFlwNodeIter & Iter);
    void           FITStates(CFlwNodeList & List);
    void           FITStartStep(CFlwNodeList & List);
    void           FITFlowInfo(CFlwNodeList & List);
    void           FITEvalProductsSurge(CLkNodeEvalIter & Iter);
    void           FITEvalProducts(CLkNodeEvalIter & Iter);
    void           FITEvalDerivs(CLkNodeEvalIter & Iter);
    void           FITEvalIntegral(CLkNodeEvalIter & Iter);
    void           FITODEOperate(CFlwNodeList & List, CODEDataBlock & ODE);
    void           FITDiscrete(CFlwNodeList & List);
    void           FITEvalCtrlActions(CFlwNodeList & List);
    void           FITEvalCtrlStrategy(CLkFlwNodeIter & Iter);//CFlwNodeList & List);
    void           FITEvalStatistics(CFlwNodeList & List);
    void           FITEvalAudit(CLkNodeEvalIter & Iter);

    CFullNetBlk     FlwNets;
    word            NNetNds;
    word            NNets;

    //double          StopTime;
    //bool            m_StopRequired;

    eScdSSTests     m_SSTest;
    long            m_SSStopCount;
    long            m_SSMaxIters;
    long            m_SSIters;
    long            m_SSSteady;
    double          m_SSMaxChange;
    Strng           m_SSMaxTag;

    CToleranceBlock m_SSPressTol;
    CToleranceBlock m_SSQmTol;


    CTimeStateBlock IntCB;
    CODESolver      m_ODE;
    byte            m_EvalCtrlAt;

    dword           dwIterNo;

    Strng           m_sDispHistTagListPath;
  };

// ==========================================================================
//
//
//
// ==========================================================================

struct FSubsDataItem
  {
  XIOAction        Action;
  pTagAccessBlk    pTAB;
  pFlwNode         pNd;
  long             lMdlIndex;
  };// FSubsDataItem;

//============================================================================

DEFINE_TAGOBJ(CADObject);

class DllImportExport CADObject : public TaggedObject
  {
  DEFINE_MEMSTATS(CADObject)

  public:
    CSArray        m_SelectedTags;
    CFlwSolver     *pFS, *pSrcFS;

  public:
    CADObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CADObject();

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};

    virtual int    SetSelectedTags(Strng_List &SelectedTags);
    virtual int    GetTagTree(CFlwTree & Tree);
    virtual int    GetStatus(Strng & Status);
    virtual flag   ValidModel() { return true; };
  };

typedef CSCDList <pCADObject, pCADObject> CADObjectList;
typedef CSCDPtrListIter<CADObjectList, pCADObject> CADObjectIter;
typedef CSCDPtrListIterWithPos<CADObjectList, pCADObject> CADObjectIterPos;

//===========================================================================

#define ADObjectGrp "ADObject"
#define IMPLEMENT_ADOBJECT(ClassName, Name, Version, DrwGroup, DefTag, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, ADObjectGrp, Name, Version, DrwGroup, DefTag, Cat, ShortDesc, Desc); 

// ==========================================================================

class CADGroup
  {
  public:
    TagObjClass *pClass;
    Strng       Class;
    Strng       Name;
   
    CArray <CADObject*, CADObject*> Obj;
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CFlwSolver : public FlwNode, public CTagRefStatusFnHook, public CGlblXRefMngr
  {
  public:
    CFlwSolver(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CFlwSolver();

    static  bool   ParseSpecieDef1(int nToks, CSVColArray & C, CDBInitRec &CDBI, int CfgFilePrjFileVerNo, int InsertIndex=100000);
    static  bool   ParseSpecieDef2(int nToks, CSVColArray & C, CDBInitRec &CDBI, int CfgFilePrjFileVerNo);
    static  bool   LoadSpecieData(LPCTSTR CfgFile, LPCTSTR PrjSDBName, bool WithMsgs);

    flag           Init();
    flag           Term();

    virtual TaggedObject * FindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen);

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   BuildDataDefnOld(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual int    FilesUsed(CFilesUsedArray & Files);

    virtual void   Set_All_Zs();
    virtual void   Set_All_Sizes();

    CTimeStateBlock &IntCB() { return XDoingPB() ? PB.IntCB : Dyn.IntCB; };

    void           DoGlobalLinks(int DoingStart);
    void           TryDoGlobalLinks() { DoGlobalLinks(False); };
    void           UnDoGlobalLinks();

    long           MarkFlwNodesPressNetwork(FlwNode * pNd);
    long           MarkFlwNodesFlowNetwork(FlwNode * pNd);

    int            FE_DoInsert(char *ClassIn, char *SubClassIn, char *PrimaryCfgIn, char *NTag, char *SrcTag, char *DstTag, FlwNode **pNd=NULL);
    int            FE_DoInsertRoot(char * RootName);
    int            FE_DoDelete(char * Tag);
    int            FE_DoConnect(char * SrcTag, char *DstTag);
    int            FE_DoDisConnect(char * SrcTag, char *DstTag);
    int            FE_SetCommon(char * Tag, char * EqpDesc, char * EqpMemo, char * EqpIdStr, char * EqpLocation, char * PlantArea, char * EqpGUID);
    int            FE_GetCommon(char * Tag, Strng & EqpDesc, Strng & EqpMemo, Strng & EqpIdStr, Strng & EqpLocation, Strng & PlantArea, Strng & EqpGUID);
//    int            FE_DoSolveNet();
    int            FE_TagOperation(FE_TagOps Act, Strng_List & TagList);
    int            FE_SetHoldGlobalLinks(bool On);

    byte           FE_FlwMomentumOn();
    void           FE_SetFlwMomentumOn(byte On);


    byte           FE_ODE_Method()                      { return ODE().Method(); };
    byte           FE_ODE_StepSizeControl()             { return ODE().StepSizeControl(); };
    void           FE_ODE_SetMethod(byte Method_)       { ODE().SetMethod(Method_); };
    void           FE_ODE_SetStepSizeControl(byte StepSizeControl_) { ODE().SetStepSizeControl(StepSizeControl_); };
    double         FE_ODE_GetMagEPSAbs()                { return ODE().GetMagEPSAbs(); };
    double         FE_ODE_GetMagEPSRel()                { return ODE().GetMagEPSRel(); };
    void           FE_ODE_SetMagEPSAbs(double Eps)      { ODE().SetMagEPSAbs(Eps); };
    void           FE_ODE_SetMagEPSRel(double Eps)      { ODE().SetMagEPSRel(Eps); };
    double         FE_ODE_GetTimeInc()                  { return ODE().GetTimeInc(); };
    void           FE_ODE_SetTimeInc(CTimeValue T)      { ODE().SetTimeInc(T); };
    double         FE_ODE_GetTimeIncMxRqd()             { return ODE().GetTimeIncMxRqd(); };
    void           FE_ODE_SetTimeIncMxRqd(CTimeValue T) { ODE().SetTimeIncMxRqd(T); };
    double         FE_ODE_GetTimeIncMnRqd()             { return ODE().GetTimeIncMnRqd(); };
    void           FE_ODE_SetTimeIncMnRqd(CTimeValue T) { ODE().SetTimeIncMnRqd(T); };

    double         FE_Net_GetLnkEPSAbs()             { return Dyn.FlwNets.m_LnkEPS.GetAbs();};
    double         FE_Net_GetLnkEPSRel()             { return Dyn.FlwNets.m_LnkEPS.GetRel();};
    void           FE_Net_SetLnkEPSAbs(double Eps)   { Dyn.FlwNets.m_LnkEPS.SetAbs(Eps);};
    void           FE_Net_SetLnkEPSRel(double Eps)   { Dyn.FlwNets.m_LnkEPS.SetRel(Eps);};
    double         FE_Net_GetNetDeRate()             { return Dyn.FlwNets.m_NetDeRating;};
    void           FE_Net_SetNetDeRate(double Mult)  { Dyn.FlwNets.m_NetDeRating=Mult;};
    LPCTSTR        FE_Net_GetSpillArea()             { return gs_DirectFlwIOTagDefault();};
    void           FE_Net_SetSpillArea(LPCTSTR Spill) { gs_DirectFlwIOTagDefault=Spill;};
    
    void           DoTraceChanges(dword Flags);

    flag           RequestModelIOInfoByClassId(pchar pClass, RequestModelIOInfoArray & Info);

    flag           RequestModelIOInfoByIndex(pchar pTag, int iIndex, RequestModelIOInfoRec & Info);
    flag           RequestModelIOInfoById(pchar pTag, int iId, RequestModelIOInfoRec & Info);
    flag           RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info);
    int            RequestModelStatus(CModelStatusCriterion &Criteria, CModelStatusArray &Status);
    int            RequestModelStatusChgCnt(CModelStatusCriterion &Criteria, int MaxTest);
    int            RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs);
    //int            RequestXRefInfoChg();
    flag           RequestModelClassId(pchar pTag, Strng & ClassId);
    flag           RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info);

    flag           FE_TestModelTagUnique(pchar pTag);
    
    int            FE_GetRctInfo(CRctBlkInformation & RctInfo);
    int            FE_SetRctInfo(LPCTSTR NodeTag);

    int            FE_GetNodeWiring(CNodeWiring &NodeWiring);
    int            FE_GetLinkWiring(CLinkWiring &LinkWiring);
    int            FE_SetLinkWiring(CLinkWiring &LinkWiring);

    long           FE_GetTopologyCount();
    long           FE_GetNodes(DWORD What, CFlwNodeArray & Nodes);
    FlwNode       *FE_FindNode(LPCTSTR Tag);
    //void           FE_ElecComponentsChanged();
    void           FE_UnDoGlobalLinks();
    long           FE_SpModelFixupStrategy();
    void           FE_SetSpModelFixupStrategy(long Strategy);

    void           EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags);
    flag           EO_GetModelAction(Strng & Tag, MdlActionArray & Acts);
    flag           EO_SetModelAction(Strng & Tag, MdlAction & Act);

    // Executiv Overrides
    void           EO_OnAppActivate(BOOL bActive);
    flag           EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
    int            EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    int            EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    int            EO_QueryDeleteTag(pchar pDelTag);
    int            EO_DeleteTag(pchar pDelTag);
    int            EO_CanClose(Strng_List & Problems);

    int            EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine);

    void           EO_SetSolveMode();
    flag           EO_Starting(flag fBeginStarting);
    flag           EO_SetTime(CTimeValue TimeRqd, bool Rewind);
    flag           EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    flag           EO_GotoRunStart(CXM_TimeControl &CB);
    flag           EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged);
    flag           EO_PreStart(CXM_TimeControl &CB);
    flag           EO_Start(CXM_TimeControl &CB);
    void           EO_QuerySubsReqd(CXMsgLst &XM);
    void           EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    flag           EO_ReadSubsData(CXMsgLst &XM);
    flag           EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);
    DWORD          EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route);
    int            EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route);
    flag           EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    flag           EO_StepDone(CXM_TimeControl &CB, flag StepExecuted);
    flag           EO_QueryStop(CXM_TimeControl &CB, bool Starting);
    flag           EO_Stop(CXM_TimeControl &CB);
    flag           EO_Stopping(flag BeginStopping);
    flag           EO_BeginPause(CXM_TimeControl &CB);
    flag           EO_EndPause(CXM_TimeControl &CB);

    flag           EO_BeginSave(FilingControlBlock &FCB);
    flag           EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM);
    flag           EO_SaveOtherData(FilingControlBlock &FCB);
    flag           EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM);
    flag           EO_EndSave(FilingControlBlock &FCB);
    flag           EO_BeginLoad(FilingControlBlock &FCB);
    flag           EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM);
    flag           EO_LoadOtherData(FilingControlBlock &FCB);
    flag           EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM);
    flag           EO_EndLoad(FilingControlBlock &FCB);

    CArray <FSubsDataItem, FSubsDataItem& > XData;
    int            LastNotPacked; // Used for testing in EO_ReadSubsData

    void           XDataClear();
    byte           iSaveWhat;
    pFlwNode       pSaveNode;
    POSITION       pSaveTear;
    int            iSaveIONo;
    Strng          Label;

    // Multi Processor Stuff
    long           nProcessors;
    long           NProcessors() { return nProcessors; };

    long           nRqdThreads;
    long           NRqdThreads();

    // Analysis Design
    flag           fADInited;
    long           m_ADIndexGrp;
    long           m_ADIndexObj;

    CArray <CADGroup, CADGroup&> ADGrps;

    void           ADInit();
    int            DuplicateNetwork(CSArray & SelectedTags, FlwNode * pRoot);
    int            AnalyseNetwork(CFlwTree & Tree);

    int            FE_AD_GetID      (int Index, Strng & Class, Strng & Name);
    int            FE_AD_Select     (int Index, char * Tag);
    int            FE_AD_SetSelectedTags(Strng_List &SelectedTags);
    int            FE_AD_GetTagTree (CFlwTree & Tree);
    int            FE_AD_GetStatus  (Strng & Status);
    int            FE_AD_Go         ();
    
    int            FE_IOPnt_Add     (LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs);
    int            FE_IOPnt_Remove  (LPCTSTR Tag);

    
    int            FE_GetEvalOrder(bool GetCtrl, bool GetFullDescI, bool GetFullDescO, eTraceValue TraceWhat, CEvalOrderArray &Info);
    int            FE_GetEvalStats(CEvalStatsArray &Info);
    int            FE_GetNodeList(CNodeList&List);
    int            FE_GetNodeConfiguration(BOOL AllParms, BOOL AllState, LPCTSTR NdTag, CNodeConfigList&List);
    int            FE_GetNodeBalanceInfo(CNodeBalanceInfo & Balance, CNodeAuditInfo & Audit);

    DEFINE_CI(CFlwSolver, FlwNode, 16);

    // CTagRefStatusFnHook override
    CTagRefStatusWord GetTagRefStatus(LPCTSTR Tag);

  public:
    SDBObject      SDBObj;
    TearObject     TearObj;
#if WITHCTRLOBJ
    CtrlObject     CtrlObj;
#endif
#if WITHCHANGEOBJ
    ChgObject      ChgObj;
#endif
    TolObject      TolObj;
#if WITHORDEROBJ
    OrderObject    OrderObj;
#endif
    PwrObject      PwrObj;
#if WITHIOOBJ
    IOObject       IOObj;
#endif             

    FlwNode      * m_pRoot;
    FlwNode      * m_pPlantArea;

    flag           FlushDbg;
    int            iLclResultsValid;

    FlwSlvPB       PB;
    FlwSlvDyn      Dyn;
    
    FILE           *Rpt;
    flag           fStopOnError;
    flag           fGlblLinksDone;
    flag           fKeepGlblLinks;
    flag           m_fHoldDoGlobalLinks;
    flag           m_bMustCreateObjsFromFile;

    flag           m_bThreadStats;

    int            m_RQ_Files_Cnt;

    double         NetMaxResRatio;
    double         m_RandomSeed;
     
    long           m_lUnDoGlobalLinksCount;

    CToleranceBlock m_MCloseTol;
    CToleranceBlock m_HCloseTol;
    CToleranceBlock m_HCloseTol2;

     

  protected:
    CString        SecsBuff;
    Strng          m_PrjCnvsDBName;
    Strng          m_PrjSDBName;

    CArray<FlwNode*> m_TempNdPtrs;
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif



