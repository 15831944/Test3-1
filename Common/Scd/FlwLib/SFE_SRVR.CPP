//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define __SFE_SRVR_CPP
#include "flwsolve.h"
#include "sfe_srvr.h"
#include "tknparse.h"
#include "datacnvs.h"
#include "dbgmngr.h"
#include "comstuff.h"

#define dbgSFE_Srvr 1

#if dbgSFE_Srvr 
#include <crtdbg.h>
static CDbgMngr dbgMemStats("SFE_Srvr", "MemStats");
#endif

//===========================================================================

#if dbgSFE_Srvr 
const int MaxNoDumpCnt=99;
static int NoDumpCnt=0x7fffffff;
static MEMORYSTATUS PrvMS;
//#ifdef _DEBUG
//static _CrtMemState checkPt1;
//#endif
#endif

//===========================================================================

// This creates COM unload problems - moved to SFE_BASE.CPP
// CSfeSrvr * gs_pSfeSrvr=NULL;

//CList <SfeInitClass *, SfeInitClass *> SfeInitClass::List;
//===========================================================================

flag CSfeSrvr::Init(pchar ModelCfg, pchar FlwLibTag, HINSTANCE hInst, CExecObj *pObj, flag DbgOn_, flag FlushDbg_, fn_CallBack CallBack,
                    flag *pStatusLoadReqd, flag * pAssocGrfsLoadReqd)
  {
  ASSERT(gs_pSfeSrvr==NULL);
  gs_pSfeSrvr=this;
  SfeCallBack=CallBack;
  SfeFS=NULL;

  CCOMObjManagerRoot::gs_Manager=new CCOMObjManager;

  flag SfeInitOK=false;
  SfeFS=(CFlwSolver *)TagObjClass::Construct("FlwSolve", "FlwSolve", NULL, FlwLibTag, NULL, TOA_Free);
  
  if (SfeFS)
    {
    SfeInitOK=SfeFS->Init(pStatusLoadReqd, pAssocGrfsLoadReqd);
    if (!SfeInitOK)
      {
      Term();
      return false;
      }

    SfeFS->AttachMeToObj(NULL, TOA_Free);
    SfeFS->FlushDbg=FlushDbg_;
    if (SfeFS->FlushDbg)
      dbgfileflush(1);

    }
 
  CDbgMngr Dbg16("General", "Flt16:Test");
  if (Dbg16())
    flt16::Test();

  FlwNode::SetExecObj(pObj);//NULL;


  return SfeFS!=NULL;
  }

//---------------------------------------------------------------------------

#if dbgSFE_Srvr 
#endif

//---------------------------------------------------------------------------

flag CSfeSrvr::Term()
  {
  // Spares Usage
  #if dbgSFE_Srvr && HEAP_SPARES
  if (dbgMemStats() || 1)
    CSparesList::DumpUsage("Term", True);
  #endif
  
  if (SfeFS)
    {
    SfeFS->Term();
    CFlwSolver * FS=(CFlwSolver *)SfeFS;
    if (FS->FlushDbg)
      dbgfileflush(0);
    
    FS->Destroy();
    SfeFS=NULL;
    }
  
  TagObjClass::ClearAll();
  CCOMObjManagerRoot::gs_Manager=NULL;

  FlwNode::SetExecObj(NULL);
  
  ASSERT(gs_pSfeSrvr==this);
  delete this;
  gs_pSfeSrvr=NULL;

  return True;
  }

//---------------------------------------------------------------------------

pNodeGrfInfo CSfeSrvr::Drawings()
  {
  NodeDrawDef NDD;
  return NDD.FindGrfInfo();
  }

//---------------------------------------------------------------------------

int CSfeSrvr::FE_DoInsert(char * Class, char *SubClass, char *PrimaryCfgIn, char *NTag, char *SrcTag, char *DstTag, FlwNode **pNd, LPSTR pGuidStr)
  {
  return SfeFS->FE_DoInsert(Class, SubClass, PrimaryCfgIn, NTag, SrcTag, DstTag, pNd, pGuidStr);
  };
int CSfeSrvr::FE_DoInsertRoot(char * RootName)
  {
  return SfeFS->FE_DoInsertRoot(RootName);
  };
int CSfeSrvr::FE_DoDelete(char * Tag)
  {
  return SfeFS->FE_DoDelete(Tag);
  };
flag CSfeSrvr::FE_GetNodeGuid(pchar Tag, Strng & Guid)
  {
  return SfeFS->FE_GetNodeGuid(Tag, Guid);
  };
int CSfeSrvr::FE_ChangeNodeGuid(pchar Tag, pchar Guid)
  {
  return SfeFS->FE_ChangeNodeGuid(Tag, Guid);
  };

flag CSfeSrvr::FE_FindNodeInfoFromGuid(pchar Guid, Strng &Tag, flag & IsLink)
  {
  return SfeFS->FE_FindNodeInfoFromGuid(Guid, Tag, IsLink);
  }

int CSfeSrvr::FE_DoConnect(char * SrcTag, char *DstTag)
  {
  return SfeFS->FE_DoConnect(SrcTag, DstTag);
  };
int CSfeSrvr::FE_DoDisConnect(char * SrcTag, char *DstTag)
  {
  return SfeFS->FE_DoDisConnect(SrcTag, DstTag);
  };
//int CSfeSrvr::FE_DoSolveNet()
//  {
//  return SfeFS->FE_DoSolveNet();
//  };
int CSfeSrvr::FE_SetCommon(char * Tag, char * EqpDesc, char * EqpMemo, char * EqpIdStr, char * EqpLocation, char * PlantArea, char * EqpGUID)
  {
  return SfeFS->FE_SetCommon(Tag, EqpDesc, EqpMemo, EqpIdStr, EqpLocation, PlantArea, EqpGUID);
  };
int CSfeSrvr::FE_GetCommon(char * Tag, Strng & EqpDesc, Strng & EqpMemo, Strng & EqpIdStr, Strng & EqpLocation, Strng & PlantArea, Strng & EqpGUID)
  {
  return SfeFS->FE_GetCommon(Tag, EqpDesc, EqpMemo, EqpIdStr, EqpLocation, PlantArea, EqpGUID);
  };

int CSfeSrvr::FE_TagOperation(FE_TagOps Act, Strng_List & TagList)
  {
  return SfeFS->FE_TagOperation(Act, TagList);
  };
int CSfeSrvr::FE_SetHoldGlobalLinks(bool On)
  {
  return SfeFS->FE_SetHoldGlobalLinks(On);
  };

byte CSfeSrvr::FE_FlwMomentumOn()
  {
  return SfeFS->FE_FlwMomentumOn();
  };
void CSfeSrvr::FE_SetFlwMomentumOn(byte On)
  {
  SfeFS->FE_SetFlwMomentumOn(On);
  };

byte CSfeSrvr::FE_ODE_Method()
  {
  return SfeFS->FE_ODE_Method();
  }
byte CSfeSrvr::FE_ODE_StepSizeControl()
  {
  return SfeFS->FE_ODE_StepSizeControl();
  }
void CSfeSrvr::FE_ODE_SetMethod(byte Method_)
  {
  SfeFS->FE_ODE_SetMethod(Method_);
  }
void CSfeSrvr::FE_ODE_SetStepSizeControl(byte StepSizeControl_)
  {
  SfeFS->FE_ODE_SetStepSizeControl(StepSizeControl_);
  }
double CSfeSrvr::FE_ODE_GetMagEPSAbs()
  {
  return SfeFS->FE_ODE_GetMagEPSAbs();
  }
double CSfeSrvr::FE_ODE_GetMagEPSRel()
  {
  return SfeFS->FE_ODE_GetMagEPSRel();
  }
void CSfeSrvr::FE_ODE_SetMagEPSAbs(double Eps)
  {
  SfeFS->FE_ODE_SetMagEPSAbs(Eps);
  }
void CSfeSrvr::FE_ODE_SetMagEPSRel(double Eps)
  {
  SfeFS->FE_ODE_SetMagEPSRel(Eps);
  }
double CSfeSrvr::FE_ODE_GetTimeInc()
  {
  return SfeFS->FE_ODE_GetTimeInc();
  }
void CSfeSrvr::FE_ODE_SetTimeInc(double T)
  {
  SfeFS->FE_ODE_SetTimeInc(T);
  }
double CSfeSrvr::FE_ODE_GetTimeIncMxRqd()
  {
  return SfeFS->FE_ODE_GetTimeIncMxRqd();
  }
void CSfeSrvr::FE_ODE_SetTimeIncMxRqd(double T)
  {
  SfeFS->FE_ODE_SetTimeIncMxRqd(T);
  }
double CSfeSrvr::FE_ODE_GetTimeIncMnRqd()
  {
  return SfeFS->FE_ODE_GetTimeIncMnRqd();
  }
void CSfeSrvr::FE_ODE_SetTimeIncMnRqd(double T)
  {
  SfeFS->FE_ODE_SetTimeIncMnRqd(T);
  }

double CSfeSrvr::FE_Net_GetLnkEPSAbs()
  {
  return SfeFS->FE_Net_GetLnkEPSAbs();
  }
double CSfeSrvr::FE_Net_GetLnkEPSRel()
  {
  return SfeFS->FE_Net_GetLnkEPSRel();
  }
void CSfeSrvr::FE_Net_SetLnkEPSAbs(double Eps)
  {
  SfeFS->FE_Net_SetLnkEPSAbs(Eps);
  }
void CSfeSrvr::FE_Net_SetLnkEPSRel(double Eps)
  {
  SfeFS->FE_Net_SetLnkEPSRel(Eps);
  }
double CSfeSrvr::FE_Net_GetNetDeRate()
  {
  return SfeFS->FE_Net_GetNetDeRate();
  }
void CSfeSrvr::FE_Net_SetNetDeRate(double Mult)
  {
  SfeFS->FE_Net_SetNetDeRate(Mult);
  }
pchar CSfeSrvr::FE_Net_GetSpillArea()
  {
  return (LPTSTR)(LPCTSTR)SfeFS->FE_Net_GetSpillArea();
  }
void CSfeSrvr::FE_Net_SetSpillArea(pchar  Spill)
  {
  SfeFS->FE_Net_SetSpillArea(Spill);
  }
  
int CSfeSrvr::FE_AD_GetID(int Index, Strng & Class, Strng & Name)
  {
  return SfeFS->FE_AD_GetID(Index, Class, Name);
  }
int CSfeSrvr::FE_AD_Select(int Index, char * Tag)
  {
  return SfeFS->FE_AD_Select(Index, Tag);
  }
int CSfeSrvr::FE_AD_SetSelectedTags(Strng_List &SelectedTags)
  {
  return SfeFS->FE_AD_SetSelectedTags(SelectedTags);
  }
int CSfeSrvr::FE_AD_GetTagTree(CFlwTree & Tree)
  {
  return SfeFS->FE_AD_GetTagTree(Tree);
  }
int CSfeSrvr::FE_AD_GetStatus(Strng & Status)
  {
  return SfeFS->FE_AD_GetStatus(Status);
  }
int CSfeSrvr::FE_AD_Go()
  {
  return SfeFS->FE_AD_Go();
  }

int CSfeSrvr::FE_IOPnt_Add(LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs)
  {
  return SfeFS->FE_IOPnt_Add(Tag, Input, Desc, Type, ValueStr, ValueCnvs);
  }
int CSfeSrvr::FE_IOPnt_Remove(LPCTSTR Tag)
  {
  return SfeFS->FE_IOPnt_Remove(Tag);
  }

int CSfeSrvr::FE_GetEvalOrder(bool GetCtrl, bool GetFullDescI, bool GetFullDescO, eTraceValue TraceWhat, CEvalOrderArray &Info)
  {
  return SfeFS->FE_GetEvalOrder(GetCtrl, GetFullDescI, GetFullDescO, TraceWhat, Info);
  }

int CSfeSrvr::FE_GetEvalStats(CEvalStatsArray &Info)
  {
  return SfeFS->FE_GetEvalStats(Info);
  }

int CSfeSrvr::FE_GetNodeList(CNodeList & List)
  {
  return SfeFS->FE_GetNodeList(List);
  }

int CSfeSrvr::FE_GetNodeConfiguration(BOOL AllParms, BOOL AllState, LPCTSTR NdTag, CNodeConfigList & List)
  {
  return SfeFS->FE_GetNodeConfiguration(AllParms, AllState, NdTag, List);
  }

int CSfeSrvr::FE_GetNodeBalanceInfo(CNodeBalanceInfo & Balance, CNodeAuditInfo & Audit)
  {
  return SfeFS->FE_GetNodeBalanceInfo(Balance, Audit);
  }

flag CSfeSrvr::RequestModelInfoByGroupIndex(pchar pGroup, int iNo, RequestModelInfoRec &Info) 
  {
  return CSfeSrvrBase::RequestModelInfoByGroupIndex(pGroup, iNo, Info); 
  };

flag CSfeSrvr::RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info)
  {
  return CSfeSrvrBase::RequestModelInfoByClassId(pClass, Info); 
  };

flag CSfeSrvr::RequestModelClassId(pchar pTag, Strng & ClassId)
  {
  return SfeFS->RequestModelClassId(pTag, ClassId); 
  };

flag CSfeSrvr::RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info)
  {
  return SfeFS->RequestModelConnInfo(pTag, Info); 
  };

//flag CSfeSrvr::RequestNodeDefinitions(LPSTR Group, CFlwNodeDefinitionList & Defns)
//  {
//  return SfeFS->RequestNodeDefinitions(Group, Defns); 
//  };
//
flag CSfeSrvr::RequestModelIOInfoByIndex(pchar pTag, int iIndex, CRequestModelIOInfoRec & Info) 
  {
  return SfeFS->RequestModelIOInfoByIndex(pTag, iIndex, Info); 
  };

flag CSfeSrvr::RequestModelIOInfoById(pchar pTag, int iId, CRequestModelIOInfoRec & Info) 
  {
  return SfeFS->RequestModelIOInfoById(pTag, iId, Info); 
  };

flag CSfeSrvr::RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info)
  {
  return SfeFS->RequestModelIOConn(pTag, iNo, Info); 
  };

int CSfeSrvr::RequestModelStatus(dword Options, CModelStatusArray &Status, flag & TagsChanged)
  {
  return SfeFS->RequestModelStatus(Options, Status, TagsChanged);
  };

flag CSfeSrvr::RequestModelAssocGrfs(CMdlAssocGrfArray & Grfs)
  { 
  return SfeFS->RequestModelAssocGrfs(Grfs); 
  };

flag CSfeSrvr::RequestModelAssocGrfsMasks(bool SetClass, bool SetNode, LPCSTR NdTag, CModelAssocGrfMasks & Msks)
  { 
  return SfeFS->RequestModelAssocGrfsMasks(SetClass, SetNode, NdTag, Msks);
  };

int CSfeSrvr::RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs) 
  { 
  return SfeFS->RequestTagRefInfo(Tag, Refs);
  };

int CSfeSrvr::FE_GetRctInfo(CRctBlkInformation & RctInfo)
  {
  return SfeFS->FE_GetRctInfo(RctInfo);
  };
int CSfeSrvr::FE_SetRctInfo(LPCTSTR NodeTag)
  {
  return SfeFS->FE_SetRctInfo(NodeTag);
  };

int CSfeSrvr::FE_GetNodeWiring(CNodeWiring &NodeWiring)
  {
  return SfeFS->FE_GetNodeWiring(NodeWiring);
  };
int CSfeSrvr::FE_GetLinkWiring(CLinkWiring &LinkWiring)
  {
  return SfeFS->FE_GetLinkWiring(LinkWiring);
  };
int CSfeSrvr::FE_SetLinkWiring(CLinkWiring &LinkWiring)
  {
  return SfeFS->FE_SetLinkWiring(LinkWiring);
  };

long CSfeSrvr::FE_GetTopologyCount()
  {
  return SfeFS->FE_GetTopologyCount();
  };

long CSfeSrvr::FE_GetNodes(DWORD What, CFlwNodeArray & Nodes)
  {
  return SfeFS->FE_GetNodes(What, Nodes);
  };

FlwNode *CSfeSrvr::FE_FindNode(LPCTSTR Tag, LPCTSTR Guid)
  {
  return SfeFS->FE_FindNode(Tag, Guid);
  };

flag CSfeSrvr::FE_TestModelTagUnique(pchar pTag)  
  {
  return SfeFS->FE_TestModelTagUnique(pTag); 
  };

//void CSfeSrvr::FE_ElecComponentsChanged()  
//  {
//  SfeFS->FE_ElecComponentsChanged(); 
//  };
void CSfeSrvr::FE_UnDoGlobalLinks()
  {
  SfeFS->FE_UnDoGlobalLinks();
  }

long CSfeSrvr::FE_SpModelFixupStrategy()  
  { 
  return SfeFS->FE_SpModelFixupStrategy();
  }
void CSfeSrvr::FE_SetSpModelFixupStrategy(long Strategy)  
  { 
  SfeFS->FE_SetSpModelFixupStrategy(Strategy);
  }

void CSfeSrvr::EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags)  
  {
  SfeFS->EO_SetModelState(RqdState, RqdTags); 
  };

flag CSfeSrvr::EO_GetModelAction(LPCTSTR Tag, CMdlActionArray & Acts) 
  { 
  return SfeFS->EO_GetModelAction(Tag, Acts); 
  };

flag CSfeSrvr::EO_SetModelAction(LPCTSTR Tag, CMdlAction & Act) 
  { 
  return SfeFS->EO_SetModelAction(Tag, Act); 
  };

flag CSfeSrvr::EO_GetModelGraphic(LPCTSTR Tag, CMdlGraphicArray & Grfs) 
  { 
  return SfeFS->EO_GetModelGraphic(Tag, Grfs); 
  };

flag CSfeSrvr::EO_OperateModelGraphic(LPCTSTR Tag, CMdlGraphicWnd & Wnd, CMdlGraphic & Grf) 
  { 
  return SfeFS->EO_OperateModelGraphic(Tag, Wnd, Grf); 
  };

void CSfeSrvr::EO_OnAppActivate(BOOL bActive)
  {
  if (SfeFS)
    SfeFS->EO_OnAppActivate(bActive); 
  };

flag CSfeSrvr::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  return SfeFS->EO_RequestTagInfo(Rqst, Info); 
  };

int CSfeSrvr::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {
  return SfeFS->EO_QueryChangeTag(pOldTag, pNewTag); 
  };

int CSfeSrvr::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  return SfeFS->EO_ChangeTag(pOldTag, pNewTag); 
  };

int CSfeSrvr::EO_ChangeTagDone(pchar pOldTag, pchar pNewTag)
  {
  return SfeFS->EO_ChangeTagDone(pOldTag, pNewTag); 
  };

int CSfeSrvr::EO_QueryDeleteTags(Strng_List & Tags) 
  {
  return SfeFS->EO_QueryDeleteTags(Tags); 
  };

int CSfeSrvr::EO_DeleteTags(Strng_List & Tags) 
  {
  return SfeFS->EO_DeleteTags(Tags); 
  };

int CSfeSrvr::EO_DeleteTagsDone(Strng_List & Tags) 
  {
  return SfeFS->EO_DeleteTagsDone(Tags); 
  };
int CSfeSrvr::EO_CanClose(Strng_List & Problems)
  {
  return SfeFS->EO_CanClose(Problems); 
  };


int CSfeSrvr::EO_SCInsertNodeG(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header, CSvcNdGBlk & NdGBlk, CSvcTagBlk & TagBlk)
  {
  return SfeFS->EO_SCInsertNodeG(Ctrl, Header, NdGBlk, TagBlk);
  };
int CSfeSrvr::EO_SCDeleteNodeG(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header)
  {
  return SfeFS->EO_SCDeleteNodeG(Ctrl, Header);
  };
int CSfeSrvr::EO_SCModifyNodeG(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header, CSvcNdGBlk & NdGBlk, CSvcTagBlk & TagBlk)
  {
  return SfeFS->EO_SCModifyNodeG(Ctrl, Header, NdGBlk, TagBlk);
  };


int CSfeSrvr::EO_SCInsertNodeM(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header)
  {
  return SfeFS->EO_SCInsertNodeM(Ctrl, Header);
  };
int CSfeSrvr::EO_SCDeleteNodeM(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header)
  {
  return SfeFS->EO_SCDeleteNodeM(Ctrl, Header);
  };
int CSfeSrvr::EO_SCModifyNodeM(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header)
  {
  return SfeFS->EO_SCModifyNodeM(Ctrl, Header);
  };


int CSfeSrvr::EO_SCInsertLinkG(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header, CSvcLnkGBlk & LnkGBlk, CPointFList & ControlPoints, CSvcTagBlk & TagBlk)
  {
  return SfeFS->EO_SCInsertLinkG(Ctrl, Header, LnkGBlk, ControlPoints, TagBlk);
  };
int CSfeSrvr::EO_SCDeleteLinkG(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header)
  {
  return SfeFS->EO_SCDeleteLinkG(Ctrl, Header);
  }
int CSfeSrvr::EO_SCModifyLinkG(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header, CSvcGuidPair & Guids, CPointFList & ControlPoints, CSvcTagBlk & TagBlk) 
  {
  return SfeFS->EO_SCModifyLinkG(Ctrl, Header, Guids, ControlPoints, TagBlk);
  };

int CSfeSrvr::EO_SCInsertLinkM(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header, CSvcLnkMBlk & LnkMBlk)
  {
  return SfeFS->EO_SCInsertLinkM(Ctrl, Header, LnkMBlk);
  };
int CSfeSrvr::EO_SCDeleteLinkM(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header)
  {
  return SfeFS->EO_SCDeleteLinkM(Ctrl, Header);
  }
int CSfeSrvr::EO_SCModifyLinkM(CSvcExecCtrl & Ctrl, CSvcHeaderBlk & Header, CSvcLnkMBlk & LnkMBlk) 
  {
  return SfeFS->EO_SCModifyLinkM(Ctrl, Header, LnkMBlk);
  };

int CSfeSrvr::EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine)
  {
  return SfeFS->EO_GetSolutionError(EQCtrl, RptLine);
  };

void CSfeSrvr::EO_SetSolveMode() 
  {
  SfeFS->EO_SetSolveMode();
  };

flag CSfeSrvr::EO_Starting(flag fBeginStarting)
  {
  return SfeFS->EO_Starting(fBeginStarting);
  };

flag CSfeSrvr::EO_SetTime(CTimeValue TimeRqd, bool Rewind) 
  {
  return SfeFS->EO_SetTime(TimeRqd, Rewind); 
  };

flag CSfeSrvr::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd) 
  {
  return SfeFS->EO_QueryTime(CB, TimeRqd, dTimeRqd); 
  };

flag CSfeSrvr::EO_GotoRunStart(CXM_TimeControl &CB)
  {
  return SfeFS->EO_GotoRunStart(CB);
  }

flag CSfeSrvr::EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged)
  {
  return SfeFS->EO_GotoRunEnd(CB, TimeChanged);
  }

flag CSfeSrvr::EO_PreStart(CXM_TimeControl &CB)
  {
  return SfeFS->EO_PreStart(CB);
  }

flag CSfeSrvr::EO_Start(CXM_TimeControl &CB)    
  {
  #if dbgSFE_Srvr && HEAP_SPARES
  if (dbgMemStats())
    CSparesList::DumpUsage("Start",True);
  #endif
  gs_TheRunMngr.DoEO_Starting();

  return SfeFS->EO_Start(CB); 
  };

void CSfeSrvr::EO_QuerySubsReqd(CXMsgLst &XM)      
  {
  SfeFS->EO_QuerySubsReqd(XM); 
  };

void CSfeSrvr::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)  
  {
  SfeFS->EO_QuerySubsAvail(XM, XMRet); 
  };

flag CSfeSrvr::EO_ReadSubsData(CXMsgLst &XM)       
  {
  return SfeFS->EO_ReadSubsData(XM); 
  };

flag CSfeSrvr::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock) 
  {
  return SfeFS->EO_WriteSubsData(XM, FirstBlock, LastBlock); 
  };

DWORD CSfeSrvr::EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  return SfeFS->EO_ReadTaggedItem(ObjTag, ObjData, Route); 
  };

int CSfeSrvr::EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  return SfeFS->EO_WriteTaggedItem(ObjData, Route); 
  };

flag CSfeSrvr::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet) 
  {
  flag b=SfeFS->EO_Execute(CB, EORet); 
  

  #if dbgSFE_Srvr 
  if (dbgMemStats())
    {
    MEMORYSTATUS MS;
    MS.dwLength=sizeof(MS);
    GlobalMemoryStatus(&MS);

    if (NoDumpCnt>=MaxNoDumpCnt || MS.dwAvailVirtual<PrvMS.dwAvailVirtual-1024)
      {
      NoDumpCnt=0;
      PrvMS=MS;
#if HEAP_SPARES
      Strng S;
      S.Set("Exec Iter %i", SfeFS->Dyn.dwIterNo);
      CSparesList::DumpUsage(S(),False);
#endif
      DumpOurHeapChanges();
      }
    else
      NoDumpCnt++;
    }
  #endif

  gs_TheRunMngr.DoEO_Execute();
  return b;
  };

flag CSfeSrvr::EO_StepDone(CXM_TimeControl &CB, flag StepExecuted)     
  {
  flag b=SfeFS->EO_StepDone(CB, StepExecuted); 
  
  #if dbgSFE_Srvr && HEAP_SPARES
  if (dbgMemStats())
    CSparesList::DumpUsage("Stop",True);
  #endif

  return b;
  };

flag CSfeSrvr::EO_QueryStop(CXM_TimeControl &CB, bool Starting)
  {
  return SfeFS->EO_QueryStop(CB, Starting); 
  };
;

flag CSfeSrvr::EO_Stop(CXM_TimeControl &CB)     
  {
  flag b=SfeFS->EO_Stop(CB); 
  
  #if dbgSFE_Srvr && HEAP_SPARES
  if (dbgMemStats())
    CSparesList::DumpUsage("Stop",True);
  #endif

  return b;
  };

flag CSfeSrvr::EO_BeginPause(CXM_TimeControl &CB)
  {
  return SfeFS->EO_BeginPause(CB); 
  };

flag CSfeSrvr::EO_EndPause(CXM_TimeControl &CB)
  {
  return SfeFS->EO_EndPause(CB); 
  };

flag CSfeSrvr::EO_BeginSave(FilingControlBlock &FCB)    
  {
  return SfeFS->EO_BeginSave(FCB); 
  };

flag CSfeSrvr::EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM)
  {
  return SfeFS->EO_SaveDefinedData(FCB, Tag, XM); 
  };

flag CSfeSrvr::EO_SaveOtherData(FilingControlBlock &FCB)
  {
  return SfeFS->EO_SaveOtherData(FCB); 
  };

flag CSfeSrvr::EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM)           
  {
  return SfeFS->EO_SaveConn(FCB, XM); 
  };

flag CSfeSrvr::EO_EndSave(FilingControlBlock &FCB)                         
  {
  return SfeFS->EO_EndSave(FCB);
  };

flag CSfeSrvr::EO_BeginLoad(FilingControlBlock &FCB)    
  {
  return SfeFS->EO_BeginLoad(FCB); 
  };

flag CSfeSrvr::EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM)            
  {
  return SfeFS->EO_LoadDefinedData(FCB, XM);
  };

flag CSfeSrvr::EO_LoadOtherData(FilingControlBlock &FCB)            
  {
  return SfeFS->EO_LoadOtherData(FCB);
  };

flag CSfeSrvr::EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM)           
  {
  return SfeFS->EO_LoadConn(FCB, XM); 
  };

flag CSfeSrvr::EO_EndLoad(FilingControlBlock &FCB)                         
  {
  return SfeFS->EO_EndLoad(FCB); 
  };
flag CSfeSrvr::EO_ConnectsDone(FilingControlBlock &FCB)                         
  {
  return SfeFS->EO_ConnectsDone(FCB); 
  };
flag CSfeSrvr::EO_DataLoaded(FilingControlBlock &FCB)                         
  {
  return SfeFS->EO_DataLoaded(FCB); 
  };

void CSfeSrvr::EO_BeginBulkChange()
  {
  SfeFS->FE_SetHoldGlobalLinks(true);
  };

void CSfeSrvr::EO_EndBulkChange()
  {
  SfeFS->FE_SetHoldGlobalLinks(false);
  };

//===========================================================================
//
//
//
//===========================================================================
