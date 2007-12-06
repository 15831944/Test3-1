//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "grf3drw.h"
#include "sfe_clnt.h"
#include "project.h"
#include "..\flwlib\sfedll.h"
#include "dbgmngr.h"
#include "apprunmngr.h"
#include "explorescd.h"
#include "scdver.h"

//#include "optoff.h"


#define dbgCmd        1
#define dbgReply      0

#define dbgClnt       01
#if dbgClnt
static CDbgMngr dbgDumpTagClasses("SFE_Clnt", "TagObjClasses");
#endif

//===========================================================================

//---------------------------------------------------------------------------
//===========================================================================

SFEFlwLib       * gs_pTheSFELib = NULL;
CAppRunManager  * gs_pAppRunMngr =  NULL;

//---------------------------------------------------------------------------

SFEFlwLib::SFEFlwLib()
  {
#if WITHSFEWND
  pSFEFrame=NULL;
  pSFEWnd=NULL;
#endif
  pSfe=NULL;
  SFENodeGrfInfo=NULL;
  FPP_State=0;

  m_Status.m_LoadReqd=true;
  m_Status.m_nLoadSeqNo=0;

  m_AssocGrfs.m_LoadReqd=true;
  m_AssocGrfs.m_nLoadSeqNo=0;

  ASSERT(gs_pTheSFELib==NULL);
  gs_pTheSFELib=this;
  }

//---------------------------------------------------------------------------

SFEFlwLib::~SFEFlwLib()
  {
  gs_pTheSFELib=NULL;
  }

//---------------------------------------------------------------------------

void SFEFlwLib::Enter()
  {
  CExecObj::LockXO();
  FPP_State=_controlfp(0,0);                                    // Get Old fpControl
  _clearfp();                                                   // clear any outstanding exceptions
  _controlfp(_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);	//Set New fpControl
  }

//---------------------------------------------------------------------------

void SFEFlwLib::Leave()
  {
  FPP_RestoreExceptions(FPP_State);
  CExecObj::UnlockXO();
  }

//---------------------------------------------------------------------------

flag SFEFlwLib::Initialise(pchar ModelCfg, pchar FlwLibTag, pchar EO_LocationTag)
  {
  char Buff[1024];
  //ModuleFileName= (GetModuleFileName(hSFELib, Buff, sizeof(Buff)) > 0) ? Buff : "";
  //sMdlDLLPath=ModuleFileName;

  sFlwLibTag=FlwLibTag;
  sMdlDLLPath=(GetModuleFileName(hSFELib, Buff, sizeof(Buff)) > 0) ? Buff : "";

#if dbgClnt
  if (dbgDumpTagClasses())
    {
    dbgpln("TagClasses - After Load");
    TagObjClass::DumpAll();
    }
#endif

  SFENodeGrfInfo=NULL;
  fn_AddSfeSrvr AddSfeSrvr=(fn_AddSfeSrvr)GetProcAddress(hSFELib, "AddSfeSrvr");
  if (AddSfeSrvr)
    {
    //nLoads++;
    pSfe=AddSfeSrvr();
    ASSERT(pSfe);
    if (pSfe)
      {
      gs_pAppRunMngr=new CAppRunManager;
      gs_TheRunMngr.m_pAppMngr=(CBaseRunManager*)gs_pAppRunMngr;
      }

    if (!pSfe->Init(ModelCfg, FlwLibTag, AfxGetInstanceHandle(), this, 1, 0, NULL, &m_Status.m_LoadReqd, &m_AssocGrfs.m_LoadReqd))
      {
      SFENodeGrfInfo=NULL;
      LogError("SysCAD", 0, "Error initializing DLL %s  (Check preceding errors!)", sMdlDLLName());
      UnLoad();
      delete gs_pAppRunMngr;
      gs_pAppRunMngr=NULL;
      gs_TheRunMngr.m_pAppMngr=NULL;
      pSfe=NULL;
      }
    else
      {
#if WITHSFEWND
      ASSERT(pSFEFrame==NULL);
      CCreateContext CC;
      CC.m_pCurrentDoc=NULL;
      CC.m_pNewDocTemplate=NULL;
      CC.m_pLastView=NULL;
      CC.m_pCurrentFrame=NULL;
      CC.m_pNewViewClass=RUNTIME_CLASS(EngStateWnd);
      pSFEFrame = new CEngStateFrm;
      HICON hIcon=ScdApp()->LoadIcon(IDI_SFESTATE);
      CString ClassName(AfxRegisterWndClass(CS_DBLCLKS|CS_NOCLOSE, 0, 0, hIcon));
      if (pSFEFrame->Create(ClassName.GetBuffer(0),
        FlwLibTag, WS_CHILD | WS_VISIBLE |
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | // seems to be required
        WS_THICKFRAME | WS_MINIMIZEBOX | //WS_MAXIMIZEBOX |
        WS_MINIMIZE,
        pSFEFrame->rectDefault, NULL, &CC))
        {
        pSFEWnd = (pEngStateWnd)(pSFEFrame)->GetTopWindow();
        pSFEWnd->SetFlwLibName(FlwLibTag);
        pSFEWnd->OnInitialUpdate();
        gs_pPrj->RestoreOneWindow(FlwLibTag, pSFEWnd, false);
        //        EO_Register(EO_LocationTag, EOWrite_Thread | EORead_Thread | EOExec_Thread, THREAD_PRIORITY_BELOW_NORMAL, 4*PkDataListLength, 101);

        }
#endif
      SYSTEM_INFO si;
      GetSystemInfo(&si);
      int Prio = si.dwNumberOfProcessors>1 ? THREAD_PRIORITY_ABOVE_NORMAL : THREAD_PRIORITY_BELOW_NORMAL;
      EO_Register(EO_LocationTag, EOWrite_Thread | EORead_Thread | EOExec_Thread, Prio, 20000/*4*PkDataListLength*/, 101);

      SFENodeGrfInfo=pSfe->Drawings();
      }
    }
  else
    {
    char txt[512];
    sprintf(txt, "Bad DLL %s", sMdlDLLName());
    AfxMessageBox(txt, MB_ICONEXCLAMATION);
    }

  return Loaded();
  }

//---------------------------------------------------------------------------

flag SFEFlwLib::Terminate()
  {
  Enter();

  if (pSfe)
    {
    pSfe->Term();
    pSfe=NULL;
    }

  if (SFENodeGrfInfo)
    {
    delete SFENodeGrfInfo;
    SFENodeGrfInfo=NULL;
    }

  EO_DeRegister();
#if WITHSFEWND
  if (pSFEFrame)
    {
    pSFEFrame->DestroyWindow();
    pSFEFrame=NULL;
    pSFEWnd=NULL;
    }
#endif

  //  dbgpln("TagClasses - After UnLoad");
  //  TagObjClass::DumpAll();

  UnlockXO();
  return true;
  }

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

//pSFEMdlLib SFEMdlLib::Attach()
//  {
//  if (pTheDLL && pTheDLL->Loaded())
//    {
//    pTheDLL->iUsage++;
//    return pTheDLL;
//    }
//  LogError("SFEClient", 0, "Model DLL Not Loaded");
//  return NULL;
//  }
////---------------------------------------------------------------------------
//
//flag SFEMdlLib::Detach()
//  {
//  if (pTheDLL && pTheDLL->Loaded())
//    {
//    pTheDLL->iUsage--;
//    return true;
//    }
//  return false;
//  }

//---------------------------------------------------------------------------

int SFEFlwLib::AddAttachedToMenu(CMenu& Menu, char* pTag)
  {
  if (pSfe==NULL)
    return 0;

  Enter();

  Strng s;
  RequestModelIOConnRec ConnInfo;
  int j = 0;
  while (pSfe->RequestModelIOConn(pTag, j++, ConnInfo) && j<IDM_PRJ_ACCESSLAST-IDM_PRJ_ACCESSFIRST)
    {
    Strng Tg;
    CRequestModelIOInfoRec IOInfo;
    if (pSfe->RequestModelIOInfoById(pTag, ConnInfo.iId, IOInfo))
      Tg.Set("%s : %s", IOInfo.Name(), ConnInfo.Tag());
    else
      Tg=ConnInfo.Tag();

    if (j==1)
      Menu.AppendMenu(MF_SEPARATOR);
    s.Set("Access %s...", Tg());//ConnInfo.Tag());
    Menu.AppendMenu(MF_STRING, IDM_PRJ_ACCESSFIRST+j-1, s());
    }
  Leave();
  return j;
  }

//---------------------------------------------------------------------------

#define VENTER        if (pSfe==NULL) return ; Enter();  try {
#define VLEAVE        Leave();                } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define BENTER(x)     if (pSfe==NULL) return x; Enter(); try { byte bVal=
#define BLEAVE        Leave(); return bVal;   } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define FENTER(x)     if (pSfe==NULL) return x; Enter(); try { flag fVal=
#define FLEAVE        Leave(); return fVal;   } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define IENTER(x)     if (pSfe==NULL) return x; Enter(); try { int iVal=
#define ILEAVE        Leave(); return iVal;   } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define DENTER(x)     if (pSfe==NULL) return x; Enter(); try { double dVal=
#define DLEAVE        Leave(); return dVal;   } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define PCENTER(x)    if (pSfe==NULL) return x; Enter(); try { char * pcVal=
#define PCLEAVE       Leave(); return pcVal;  } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define DWENTER(x)    if (pSfe==NULL) return x; Enter(); try { dword dwVal=
#define DWLEAVE       Leave(); return dwVal;  } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }
#define SENTER(x)     if (pSfe==NULL) return x; Enter(); try { Strng sVal=
#define SLEAVE        Leave(); return sVal;   } catch (MSysException e) { FPP_RestoreExceptions(FPP_State); CExecObj::UnlockXO(); throw; }

//---------------------------------------------------------------------------

int SFEFlwLib::FE_DoInsert(char * Class, char *SubClass, char *PrimaryCfgIn, char *NTag, char *SrcTag, char *DstTag, FlwNode **pNd, LPSTR pGuidStr)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(-1)
    pSfe->FE_DoInsert(Class, SubClass, PrimaryCfgIn, NTag, SrcTag, DstTag, pNd, pGuidStr);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

int SFEFlwLib::FE_DoInsertRoot(char * RootName)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(-1)
    pSfe->FE_DoInsertRoot(RootName);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };
int SFEFlwLib::FE_DoDelete(char * Tag)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(-1)
    pSfe->FE_DoDelete(Tag);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

flag SFEFlwLib::FE_GetNodeGuid(pchar Tag, Strng & Guid)
  {
  //SetLoadAllModelStatus();
  //SetLoadAllModelAssocGrfs();
  FENTER(NULL)
    pSfe->FE_GetNodeGuid(Tag, Guid);
  //CExploreScd::RefreshIt(false);
  FLEAVE
  };

int SFEFlwLib::FE_ChangeNodeGuid(pchar Tag, pchar Guid)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(-1)
    pSfe->FE_ChangeNodeGuid(Tag, Guid);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

flag SFEFlwLib::FE_FindNodeInfoFromGuid(pchar Guid, Strng &Tag, flag & IsLink)
  {
  //SetLoadAllModelStatus();
  //SetLoadAllModelAssocGrfs();
  FENTER(0)
    pSfe->FE_FindNodeInfoFromGuid(Guid, Tag, IsLink);
  //CExploreScd::RefreshIt(false);
  FLEAVE
  };

int SFEFlwLib::FE_DoConnect(char * SrcTag, char *DstTag)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(-1)
    pSfe->FE_DoConnect(SrcTag, DstTag);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };
int SFEFlwLib::FE_DoDisConnect(char * SrcTag, char *DstTag)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(-1)
    pSfe->FE_DoDisConnect(SrcTag, DstTag);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

int SFEFlwLib::FE_SetCommon(char * Tag, char * EqpDesc, char * EqpMemo, char * EqpIdStr, char * EqpLocation, char * PlantArea, char * EqpGUID)
  {
  IENTER(-1)
    pSfe->FE_SetCommon(Tag, EqpDesc, EqpMemo, EqpIdStr, EqpLocation, PlantArea, EqpGUID);
  ILEAVE
  };
int SFEFlwLib::FE_GetCommon(char * Tag, Strng & EqpDesc, Strng & EqpMemo, Strng & EqpIdStr, Strng & EqpLocation, Strng & PlantArea, Strng & EqpGUID)
  {
  IENTER(-1)
    pSfe->FE_GetCommon(Tag, EqpDesc, EqpMemo, EqpIdStr, EqpLocation, PlantArea, EqpGUID);
  ILEAVE
  };

//int SFEFlwLib::FE_DoSolveNet()
//  { IENTER(-1) pSfe->FE_DoSolveNet(); ILEAVE };
int  SFEFlwLib::FE_TagOperation(FE_TagOps Act, Strng_List & TagList)
  {
  //static int XXX=0;
  //dbgpln("XXXXXXXX %i", XXX++);
  IENTER(-1) pSfe->FE_TagOperation(Act, TagList); ILEAVE
  };
int  SFEFlwLib::FE_SetHoldGlobalLinks(bool On)
  { IENTER(-1) pSfe->FE_SetHoldGlobalLinks(On); ILEAVE };

byte SFEFlwLib::FE_FlwMomentumOn()
  { BENTER(0) pSfe->FE_FlwMomentumOn(); BLEAVE};
void SFEFlwLib::FE_SetFlwMomentumOn(byte On)
  { VENTER pSfe->FE_SetFlwMomentumOn(On); VLEAVE };

byte SFEFlwLib::FE_ODE_Method()
  { BENTER(0) pSfe->FE_ODE_Method(); BLEAVE};
byte SFEFlwLib::FE_ODE_StepSizeControl()
  { BENTER(0) pSfe->FE_ODE_StepSizeControl(); BLEAVE};
void SFEFlwLib::FE_ODE_SetMethod(byte Method_)
  { VENTER pSfe->FE_ODE_SetMethod(Method_); VLEAVE};
void SFEFlwLib::FE_ODE_SetStepSizeControl(byte StepSizeControl_)
  { VENTER pSfe->FE_ODE_SetStepSizeControl(StepSizeControl_); VLEAVE};
double SFEFlwLib::FE_ODE_GetMagEPSAbs()
  { DENTER(0.0) pSfe->FE_ODE_GetMagEPSAbs(); DLEAVE};
double SFEFlwLib::FE_ODE_GetMagEPSRel()
  { DENTER(0.0) pSfe->FE_ODE_GetMagEPSRel(); DLEAVE};
void SFEFlwLib::FE_ODE_SetMagEPSAbs(double Eps)
  { VENTER pSfe->FE_ODE_SetMagEPSAbs(Eps); VLEAVE};
void SFEFlwLib::FE_ODE_SetMagEPSRel(double Eps)
  { VENTER pSfe->FE_ODE_SetMagEPSRel(Eps); VLEAVE};
double SFEFlwLib::FE_ODE_GetTimeInc()
  { DENTER(0.0) pSfe->FE_ODE_GetTimeInc(); DLEAVE};
void SFEFlwLib::FE_ODE_SetTimeInc(double T)
  { VENTER pSfe->FE_ODE_SetTimeInc(T); VLEAVE};
double SFEFlwLib::FE_ODE_GetTimeIncMxRqd()
  { DENTER(0.0) pSfe->FE_ODE_GetTimeIncMxRqd(); DLEAVE};
void SFEFlwLib::FE_ODE_SetTimeIncMxRqd(double T)
  { VENTER pSfe->FE_ODE_SetTimeIncMxRqd(T); VLEAVE};
double SFEFlwLib::FE_ODE_GetTimeIncMnRqd()
  { return pSfe==NULL ? 0 : pSfe->FE_ODE_GetTimeIncMnRqd(); };
void SFEFlwLib::FE_ODE_SetTimeIncMnRqd(double T)
  { VENTER pSfe->FE_ODE_SetTimeIncMnRqd(T); VLEAVE};

double SFEFlwLib::FE_Net_GetLnkEPSAbs()
  { DENTER(0.0) pSfe->FE_Net_GetLnkEPSAbs(); DLEAVE};
double SFEFlwLib::FE_Net_GetLnkEPSRel()
  { DENTER(0.0) pSfe->FE_Net_GetLnkEPSRel(); DLEAVE};
void SFEFlwLib::FE_Net_SetLnkEPSAbs(double Eps)
  { VENTER pSfe->FE_Net_SetLnkEPSAbs(Eps); VLEAVE};
void SFEFlwLib::FE_Net_SetLnkEPSRel(double Eps)
  { VENTER pSfe->FE_Net_SetLnkEPSRel(Eps); VLEAVE};
double SFEFlwLib::FE_Net_GetNetDeRate()
  { DENTER(0.0) pSfe->FE_Net_GetNetDeRate(); DLEAVE};
void SFEFlwLib::FE_Net_SetNetDeRate(double Mult)
  { VENTER pSfe->FE_Net_SetNetDeRate(Mult); VLEAVE};
pchar SFEFlwLib::FE_Net_GetSpillArea()
  { PCENTER(NULL) pSfe->FE_Net_GetSpillArea(); PCLEAVE};
void SFEFlwLib::FE_Net_SetSpillArea(pchar Spill)
  { VENTER pSfe->FE_Net_SetSpillArea(Spill); VLEAVE}

int SFEFlwLib::FE_AD_GetID(int Index, Strng & Class, Strng & Name)
  { IENTER(-1) pSfe->FE_AD_GetID(Index, Class, Name); ILEAVE};
int SFEFlwLib::FE_AD_Select(int Index, char * Tag)
  { IENTER(-1) pSfe->FE_AD_Select(Index, Tag); ILEAVE};
int SFEFlwLib::FE_AD_SetSelectedTags(Strng_List &SelectedTags)
  { IENTER(-1) pSfe->FE_AD_SetSelectedTags(SelectedTags); ILEAVE};
int SFEFlwLib::FE_AD_GetTagTree(CFlwTree & Tree)
  { IENTER(-1) pSfe->FE_AD_GetTagTree(Tree); ILEAVE};
int SFEFlwLib::FE_AD_GetStatus(Strng & Status)
  { IENTER(-1) pSfe->FE_AD_GetStatus(Status); ILEAVE};
int SFEFlwLib::FE_AD_Go()
  { IENTER(-1) pSfe->FE_AD_Go(); ILEAVE};

int SFEFlwLib::FE_IOPnt_Add(LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs)
  { IENTER(-1) pSfe->FE_IOPnt_Add(Tag, Input, Desc, Type, ValueStr, ValueCnvs); ILEAVE};
int SFEFlwLib::FE_IOPnt_Remove(LPCTSTR Tag)
  { IENTER(-1) pSfe->FE_IOPnt_Remove(Tag); ILEAVE};

int SFEFlwLib::FE_GetEvalOrder(bool GetCtrl, bool GetFullDescI, bool GetFullDescO, eTraceValue TraceWhat, CEvalOrderArray &Info)
  { IENTER(-1) pSfe->FE_GetEvalOrder(GetCtrl, GetFullDescI, GetFullDescO, TraceWhat, Info); ILEAVE};
int SFEFlwLib::FE_GetEvalStats(CEvalStatsArray &Info)
  { IENTER(-1) pSfe->FE_GetEvalStats(Info); ILEAVE};
int SFEFlwLib::FE_GetNodeList(CNodeList&List)
  { IENTER(-1) pSfe->FE_GetNodeList(List); ILEAVE};
int SFEFlwLib::FE_GetNodeConfiguration(BOOL AllParms, BOOL AllState, LPCTSTR NdTag, CNodeConfigList&List)
  { IENTER(-1) pSfe->FE_GetNodeConfiguration(AllParms, AllState, NdTag, List); ILEAVE};
int SFEFlwLib::FE_GetNodeBalanceInfo(CNodeBalanceInfo & Balance, CNodeAuditInfo & Audit)
  { IENTER(-1) pSfe->FE_GetNodeBalanceInfo(Balance, Audit); ILEAVE};

int SFEFlwLib::FE_SpModelFixupStrategy()
  { IENTER(-1) pSfe->FE_SpModelFixupStrategy(); ILEAVE}
void SFEFlwLib::FE_SetSpModelFixupStrategy(long Strategy)
  { VENTER; pSfe->FE_SetSpModelFixupStrategy(Strategy); VLEAVE }

pchar SFEFlwLib::RequestModelDLLPath()
  { return sMdlDLLPath(); };
pchar SFEFlwLib::RequestModelDLLTag()
  { return sFlwLibTag(); };
flag SFEFlwLib::RequestModelInfoByGroupIndex(pchar pGroup, int iIndex, RequestModelInfoRec &Info)
  { FENTER(false) pSfe->RequestModelInfoByGroupIndex(pGroup, iIndex, Info); FLEAVE };

flag SFEFlwLib::RequestModelInfoByClassId(pchar pClass, RequestModelInfoRec & Info)
  { FENTER(false) pSfe->RequestModelInfoByClassId(pClass, Info); FLEAVE};
flag SFEFlwLib::RequestModelClassId(pchar pTag, Strng & ClassId)
  { FENTER(false) pSfe->RequestModelClassId(pTag, ClassId); FLEAVE};
flag SFEFlwLib::RequestModelConnInfo(pchar pTag, RequestConnModelInfoRec & Info)
  { FENTER(false) pSfe->RequestModelConnInfo(pTag, Info); FLEAVE};
//flag SFEFlwLib::RequestNodeDefinitions(LPSTR Group, CFlwNodeDefinitionList & Defns)
//  { FENTER(false) pSfe->RequestNodeDefinitions(Group, Defns); FLEAVE};
flag SFEFlwLib::RequestModelIOInfoByIndex(pchar pTag, int iIndex, CRequestModelIOInfoRec & Info)
  { FENTER(false) pSfe->RequestModelIOInfoByIndex(pTag, iIndex, Info); FLEAVE};
flag SFEFlwLib::RequestModelIOInfoById(pchar pTag, int iId, CRequestModelIOInfoRec & Info)
  { FENTER(false) pSfe->RequestModelIOInfoById(pTag, iId, Info); FLEAVE};
flag SFEFlwLib::RequestModelIOConn(pchar pTag, int iNo, RequestModelIOConnRec & Info)
  { FENTER(false) pSfe->RequestModelIOConn(pTag, iNo, Info); FLEAVE};

int  SFEFlwLib::RequestModelStatus(CModelStatusArray &Status, flag & TagsChanged)
  {
  if (m_Status.m_LoadReqd)
    {
    m_Status.m_LoadReqd=false;
    IENTER(false) pSfe->RequestModelStatus(m_Status.m_dwOptions, Status, TagsChanged);
    ILEAVE
    }
  else
    {
    return 0;
    }
  };

flag SFEFlwLib::TestLoadModelStatus()
  {
  return m_Status.m_LoadReqd;
  };

flag SFEFlwLib::RequestModelAssocGrfs(CMdlAssocGrfArray & Grf)
  { 
  if (m_AssocGrfs.m_LoadReqd)
    {
    m_AssocGrfs.m_LoadReqd=false;
    FENTER(true) pSfe->RequestModelAssocGrfs(Grf); 
    FLEAVE
    }
  else
    {
    return 0;
    }
  };
flag SFEFlwLib::TestLoadModelAssocGrfs()
  {
  return m_AssocGrfs.m_LoadReqd;
  };

flag SFEFlwLib::RequestModelAssocGrfsMasks(bool SetClass, bool SetNode,LPCSTR NdTag, CModelAssocGrfMasks & Msks)
  { FENTER(true) pSfe->RequestModelAssocGrfsMasks(SetClass, SetNode, NdTag, Msks); FLEAVE};

int SFEFlwLib::RequestTagRefInfo(LPCTSTR Tag, CXRefInfoArray &Refs)
  { IENTER(false) pSfe->RequestTagRefInfo(Tag, Refs); ILEAVE};

//int SFEFlwLib::RequestXRefInfoChg()
//   { IENTER(false) pSfe->RequestXRefInfoChg(); ILEAVE};
flag SFEFlwLib::FE_TestModelTagUnique(pchar pTag)
  { FENTER(true) pSfe->FE_TestModelTagUnique(pTag); FLEAVE};

int SFEFlwLib::FE_GetRctInfo(CRctBlkInformation & RctInfo)
  { IENTER(0) pSfe->FE_GetRctInfo(RctInfo); ILEAVE};
int SFEFlwLib::FE_SetRctInfo(LPCTSTR NodeTag)
  { IENTER(0) pSfe->FE_SetRctInfo(NodeTag); ILEAVE};

int  SFEFlwLib::FE_GetNodeWiring(CNodeWiring &NodeWiring)
  { IENTER(false) pSfe->FE_GetNodeWiring(NodeWiring); ILEAVE};
int  SFEFlwLib::FE_GetLinkWiring(CLinkWiring &LinkWiring)
  { IENTER(false) pSfe->FE_GetLinkWiring(LinkWiring); ILEAVE};
int  SFEFlwLib::FE_SetLinkWiring(CLinkWiring &LinkWiring)
  { IENTER(false) pSfe->FE_SetLinkWiring(LinkWiring); ILEAVE};

void SFEFlwLib::EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags)
  { VENTER pSfe->EO_SetModelState(RqdState, RqdTags); VLEAVE};
flag SFEFlwLib::EO_GetModelAction(LPCTSTR Tag, CMdlActionArray & Acts)
  { FENTER(false) pSfe->EO_GetModelAction(Tag, Acts); FLEAVE};
flag SFEFlwLib::EO_SetModelAction(LPCTSTR Tag, CMdlAction & Act)
  { FENTER(false) pSfe->EO_SetModelAction(Tag, Act); FLEAVE};

flag SFEFlwLib::EO_GetModelGraphic(LPCTSTR Tag, CMdlGraphicArray & Grfs)
  { FENTER(false) pSfe->EO_GetModelGraphic(Tag, Grfs); FLEAVE};
flag SFEFlwLib::EO_OperateModelGraphic(LPCTSTR Tag, CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
  { FENTER(false) pSfe->EO_OperateModelGraphic(Tag, Wnd, Grf); FLEAVE};


//flag SFEFlwLib::EO_SolutionReqd()
//   { return pSfe==NULL ? false : pSfe->EO_SolutionReqd();  };

//int SFEFlwLib::EO_InitSolution(EqnSlvCtrlBlk & EQCtrl)
//   { return pSfe==NULL ? -1 : pSfe->EO_InitSolution(EQCtrl); };
//int SFEFlwLib::EO_StepSolution(EqnSlvCtrlBlk & EQCtrl)
//   { return pSfe==NULL ? -1 : pSfe->EO_StepSolution(EQCtrl); };
//int SFEFlwLib::EO_TermSolution(EqnSlvCtrlBlk & EQCtrl)
//   { return pSfe==NULL ? -1 : pSfe->EO_TermSolution(EQCtrl); };
int SFEFlwLib::EO_GetSolutionError(EqnSlvCtrlBlk & EQCtrl, Strng & RptLine)
  { IENTER(-1) pSfe->EO_GetSolutionError(EQCtrl, RptLine); ILEAVE };

// CExecObj Overrides
void SFEFlwLib::EO_OnAppActivate(BOOL bActive)
  { VENTER pSfe->EO_OnAppActivate(bActive);  VLEAVE};
flag SFEFlwLib::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  { FENTER(false) pSfe->EO_RequestTagInfo(Rqst, Info);  FLEAVE};
int  SFEFlwLib::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  { IENTER(false) pSfe->EO_QueryChangeTag(pOldTag, pNewTag); ILEAVE };
int  SFEFlwLib::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_ChangeTag(pOldTag, pNewTag);
  ILEAVE
  };
int  SFEFlwLib::EO_ChangeTagDone(pchar pOldTag, pchar pNewTag)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_ChangeTagDone(pOldTag, pNewTag);
  ILEAVE
  };
int  SFEFlwLib::EO_QueryDeleteTags(Strng_List & Tags) 
  { IENTER(false) pSfe->EO_QueryDeleteTags(Tags); ILEAVE };
int  SFEFlwLib::EO_DeleteTags(Strng_List & Tags) 
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_DeleteTags(Tags);
  //CExploreScd::RefreshIt(false);
  ILEAVE
  };
int  SFEFlwLib::EO_DeleteTagsDone(Strng_List & Tags) 
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_DeleteTagsDone(Tags);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

//----------------------------------------------------------------------------------------------------------------

int SFEFlwLib::EO_SCInsertNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                              LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                              float angle, const CRectangleF & tagArea, float tagAngle, COLORREF Colour, 
                              bool mirrorX, bool mirrorY)
  {
  IENTER(false) pSfe->EO_SCInsertNodeG(Ctrl, pTag, ItemGuid, path, model, shape, boundingRect, angle, tagArea, tagAngle, Colour, mirrorX, mirrorY);
  ILEAVE
  };
int SFEFlwLib::EO_SCDeleteNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path)
  {
  IENTER(false) pSfe->EO_SCDeleteNodeG(Ctrl, pTag, ItemGuid, path);
  ILEAVE
  };
int SFEFlwLib::EO_SCModifyNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                              LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                              float angle, const CRectangleF & tagArea, float tagAngle, COLORREF Colour, 
                              bool mirrorX, bool mirrorY)
  {
  IENTER(false) pSfe->EO_SCModifyNodeG(Ctrl, pTag, ItemGuid, path, model, shape, boundingRect, angle, tagArea, tagAngle, Colour, mirrorX, mirrorY);
  ILEAVE
  };

int SFEFlwLib::EO_SCInsertNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR model)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCInsertNodeM(Ctrl, pTag, ItemGuid, model);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };
int SFEFlwLib::EO_SCDeleteNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCDeleteNodeM(Ctrl, pTag, ItemGuid);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };
int SFEFlwLib::EO_SCModifyNodeM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR model)
  {
  IENTER(false) pSfe->EO_SCModifyNodeM(Ctrl, pTag, ItemGuid, model);
  ILEAVE
  };

//----------------------------------------------------------------------------------------------------------------

int SFEFlwLib::EO_SCInsertLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                               LPCSTR model, 
                               LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                               LPCSTR OriginTag, LPCSTR DestinationTag, 
                               CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle) 
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCInsertLinkG(Ctrl, pTag, ItemGuid, /*path,*/
                                   model, 
                                   OriginGuid, DestinationGuid, 
                                   OriginTag, DestinationTag, 
                                   ControlPoints, tagArea, tagAngle) ;
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

int SFEFlwLib::EO_SCDeleteLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCDeleteLinkG(Ctrl, pTag, ItemGuid/*, path*/);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

int SFEFlwLib::EO_SCModifyLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                               LPCSTR model, 
                               LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                               LPCSTR OriginTag, LPCSTR DestinationTag, 
                               CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle) 
  {
  //SetLoadAllModelStatus();
  //SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCModifyLinkG(Ctrl, pTag, ItemGuid, /*path,*/
                                   model, 
                                   OriginGuid, DestinationGuid, 
                                   OriginTag, DestinationTag, 
                                   ControlPoints, tagArea, tagAngle) ;
  //CExploreScd::RefreshIt(false);
  ILEAVE
  };
int SFEFlwLib::EO_SCInsertLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                               LPCSTR model, 
                               LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                               LPCSTR OriginTag, LPCSTR DestinationTag, 
                               LPCSTR OriginPort, LPCSTR DestinationPort) 
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCInsertLinkM(Ctrl, pTag, ItemGuid, /*path,*/
                                   model, 
                                   OriginGuid, DestinationGuid, 
                                   OriginTag, DestinationTag, 
                                   OriginPort, DestinationPort) ;
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

int SFEFlwLib::EO_SCDeleteLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCDeleteLinkM(Ctrl, pTag, ItemGuid/*, path*/);
  CExploreScd::RefreshIt(false);
  ILEAVE
  };

int SFEFlwLib::EO_SCModifyLinkM(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid/*, LPCSTR path*/,
                               LPCSTR model, 
                               LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                               LPCSTR OriginTag, LPCSTR DestinationTag, 
                               LPCSTR OriginPort, LPCSTR DestinationPort) 
  {
  //SetLoadAllModelStatus();
  //SetLoadAllModelAssocGrfs();
  IENTER(false) pSfe->EO_SCModifyLinkM(Ctrl, pTag, ItemGuid, /*path,*/
                                   model, 
                                   OriginGuid, DestinationGuid, 
                                   OriginTag, DestinationTag, 
                                   OriginPort, DestinationPort) ;
  //CExploreScd::RefreshIt(false);
  ILEAVE
  };

//----------------------------------------------------------------------------------------------------------------

int  SFEFlwLib::EO_CanClose(Strng_List & Problems)
  { IENTER(EO_CanClose_Yes)  pSfe->EO_CanClose(Problems); ILEAVE };
void SFEFlwLib::EO_SetSolveMode()
  { VENTER pSfe->EO_SetSolveMode(); VLEAVE};
flag SFEFlwLib::EO_Starting(flag fBeginStarting)
  { FENTER(false) pSfe->EO_Starting(fBeginStarting);  FLEAVE};
flag SFEFlwLib::EO_SetTime(CTimeValue TimeRqd, bool Rewind)
  { FENTER(false) pSfe->EO_SetTime(TimeRqd, Rewind); FLEAVE };
flag SFEFlwLib::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  { FENTER(false) pSfe->EO_QueryTime(CB, TimeRqd, dTimeRqd); FLEAVE };
flag SFEFlwLib::EO_GotoRunStart(CXM_TimeControl &CB)
  { FENTER(false) pSfe->EO_GotoRunStart(CB); FLEAVE};
flag SFEFlwLib::EO_GotoRunEnd(CXM_TimeControl &CB, bool TimeChanged)
  { FENTER(false) pSfe->EO_GotoRunEnd(CB, TimeChanged); FLEAVE};
flag SFEFlwLib::EO_PreStart(CXM_TimeControl &CB)
  { FENTER(false) pSfe->EO_PreStart(CB); FLEAVE};
flag SFEFlwLib::EO_Start(CXM_TimeControl &CB)
  {
  FENTER(false) pSfe->EO_Start(CB);
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  FLEAVE
  };
void SFEFlwLib::EO_QuerySubsReqd(CXMsgLst &XM)
  { VENTER pSfe->EO_QuerySubsReqd(XM); VLEAVE};
void SFEFlwLib::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  { VENTER pSfe->EO_QuerySubsAvail(XM, XMRet); VLEAVE};
flag SFEFlwLib::EO_ReadSubsData(CXMsgLst &XM)
  { FENTER(false) pSfe->EO_ReadSubsData(XM); FLEAVE};
flag SFEFlwLib::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  { 
  FENTER(false) pSfe->EO_WriteSubsData(XM, FirstBlock, LastBlock); 
  FLEAVE
  };
DWORD SFEFlwLib::EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route)
  { DWENTER(0) pSfe->EO_ReadTaggedItem(ObjTag, ObjData, Route); DWLEAVE};
int  SFEFlwLib::EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route)
  { 
  IENTER(TOData_NotFound) pSfe->EO_WriteTaggedItem(ObjData, Route); 
  SetLoadAllModelStatus();
  
  if (XStopped())
    {
    SetLoadAllModelAssocGrfs();
    XUpdateDisplays(UDO_ForceUpd);
    }
  ILEAVE 
  };
flag SFEFlwLib::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  FENTER(false) pSfe->EO_Execute(CB, EORet);
  SetLoadAllModelStatus();
  //m_AssocGrfs.m_LoadReqd=true; // this will usually get set on-demand by the flwsolver
  FLEAVE
  };
flag SFEFlwLib::EO_StepDone(CXM_TimeControl &CB, flag StepExecuted)
  {
  FENTER(false) pSfe->EO_StepDone(CB, StepExecuted);
  FLEAVE
  };
flag SFEFlwLib::EO_QueryStop(CXM_TimeControl &CB, bool Starting)
  {
  FENTER(false) pSfe->EO_QueryStop(CB, Starting);
  FLEAVE
  };
flag SFEFlwLib::EO_Stop(CXM_TimeControl &CB)
  {
  FENTER(false) pSfe->EO_Stop(CB);
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  XUpdateDisplays(UDO_ForceUpd);
  FLEAVE
  };
flag SFEFlwLib::EO_Stopping(flag BeginStopping)
  {
  FENTER(false)
    pSfe->EO_Stopping(BeginStopping);
  if (!BeginStopping)
    {
    SetLoadAllModelStatus();
    SetLoadAllModelAssocGrfs();
    XUpdateDisplays(UDO_ForceUpd);
    }
  FLEAVE
  };
flag SFEFlwLib::EO_BeginPause(CXM_TimeControl &CB)
  {
  SetLoadAllModelStatus();
  SetLoadAllModelAssocGrfs();
  FENTER(false) pSfe->EO_BeginPause(CB);
  FLEAVE
  };
flag SFEFlwLib::EO_EndPause(CXM_TimeControl &CB)
  {
  FENTER(false) pSfe->EO_EndPause(CB);
  SetLoadAllModelStatus();
  //SetLoadAllModelAssocGrfs(); // this will usually get set on-demand by the flwsolver
  FLEAVE
  };

flag SFEFlwLib::EO_BeginSave(FilingControlBlock &FCB)
  { FENTER(false) pSfe->EO_BeginSave(FCB); FLEAVE};
flag SFEFlwLib::EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM)
  { FENTER(false) pSfe->EO_SaveDefinedData(FCB, Tag, XM); FLEAVE};
flag SFEFlwLib::EO_SaveOtherData(FilingControlBlock &FCB)
  { FENTER(false) pSfe->EO_SaveOtherData(FCB); FLEAVE};
flag SFEFlwLib::EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM)
  { FENTER(false) pSfe->EO_SaveConn(FCB, XM); FLEAVE};
flag SFEFlwLib::EO_EndSave(FilingControlBlock &FCB)
  { FENTER(false) pSfe->EO_EndSave(FCB); FLEAVE};
flag SFEFlwLib::EO_BeginLoad(FilingControlBlock &FCB)
  {
  LockStatus();
  m_Status.m_StatusMap.RemoveAll();
  m_Status.m_Status.SetSize(0);
  m_Status.m_LoadReqd=true;
  m_Status.m_nLoadSeqNo=0;
  m_AssocGrfs.m_LoadReqd=true;
  m_AssocGrfs.m_nLoadSeqNo=0;
  FreeStatus();
  FENTER(false) pSfe->EO_BeginLoad(FCB); FLEAVE
  };
flag SFEFlwLib::EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM)
  { FENTER(false) pSfe->EO_LoadDefinedData(FCB, XM); FLEAVE};
flag SFEFlwLib::EO_LoadOtherData(FilingControlBlock &FCB)
  { FENTER(false) pSfe->EO_LoadOtherData(FCB); FLEAVE};
flag SFEFlwLib::EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM)
  { FENTER(false) pSfe->EO_LoadConn(FCB, XM); FLEAVE};
flag SFEFlwLib::EO_EndConnect(FilingControlBlock &FCB)
  { FENTER(false) pSfe->EO_EndConnect(FCB); FLEAVE };
flag SFEFlwLib::EO_EndLoad(FilingControlBlock &FCB)
  {
  FENTER(false) pSfe->EO_EndLoad(FCB);
  CExploreScd::RefreshIt(true);
  FLEAVE
  };
void SFEFlwLib::EO_BeginBulkChange()
  { VENTER pSfe->EO_BeginBulkChange(); VLEAVE};
void SFEFlwLib::EO_EndBulkChange()
  { VENTER pSfe->EO_EndBulkChange(); VLEAVE};

//---------------------------------------------------------------------------

void SFEFlwLib::FE_SetModelStatusOptions(dword Options)
  {
  }

//---------------------------------------------------------------------------

#define dbgAssocGrfs      0

int SFEFlwLib::FE_LoadAllModelStatus()
  {

  LockStatus();
  flag TagsChanged=false;
  if (RequestModelStatus(m_Status.m_Status, TagsChanged))
    {
    dword iSize=FindNextPrimeNumber((m_Status.m_Status.GetSize()*120)/100);
    if (m_Status.m_StatusMap.GetHashTableSize()!=iSize)
      {
      m_Status.m_StatusMap.RemoveAll();
      m_Status.m_StatusMap.InitHashTable(iSize);
      }
    else if (TagsChanged)
      m_Status.m_StatusMap.RemoveAll();
    
    for (int i=0; i<m_Status.m_Status.GetSize(); i++)
      m_Status.m_StatusMap.SetAt(m_Status.m_Status[i].m_Tag(), &m_Status.m_Status[i]);

    m_Status.m_nLoadSeqNo++;
    long L=m_Status.m_Status.GetSize();
    FreeStatus();

#if dbgAssocGrfs
    dbgpln("FE_LoadAllModelStatus    Set Count %5i Seq %5i", L, m_Status.m_nLoadSeqNo);
#endif

    return L;
    }
  m_Status.m_StatusMap.RemoveAll();
  m_Status.m_Status.SetSize(0);
  FreeStatus();
#if dbgAssocGrfs
  dbgpln("FE_LoadAllModelStatus    %i", 0); 
#endif
  return 0;
  };


//---------------------------------------------------------------------------

dword SFEFlwLib::FE_GetModelStatus(pchar pTag)
  {
  //LockStatus();
  dword dwVal=0;

  LockStatus();
  if (m_Status.m_LoadReqd)
    {
    FreeStatus();
    CExecObj::LockXO();
    FE_LoadAllModelStatus();
    CExecObj::UnlockXO();
    LockStatus();

    //dbgpln("FE_GetModelStatus =======================");
    }

  CModelStatusItem* pItem;
  if (pTag && m_Status.m_StatusMap.Lookup(pTag, pItem))
    dwVal=pItem->m_dwFlags;
  else
    dwVal=0xFFFFFFFF;

  //dbgpln("Status  0x%08x %s", dwVal, pTag);
  FreeStatus();
  return dwVal;
  };

//---------------------------------------------------------------------------


int SFEFlwLib::FE_LoadAllModelAssocGrfs()
  {
  LockAssocGrfs();

#if (0 && dbgAssocGrfs)
  dbgpln("FE_LoadAllModelAssocGrfs====================");
#endif
  //CMdlAssocGrfArray Grfs;
  if (RequestModelAssocGrfs(m_AssocGrfs.m_Grfs))
    {
    m_AssocGrfs.m_Map.RemoveAll();

    dword ReqdSize=(Max(200, m_AssocGrfs.m_Map.GetCount())*120)/100;
    if (m_AssocGrfs.m_Map.GetHashTableSize()<=ReqdSize)
      m_AssocGrfs.m_Map.InitHashTable(ReqdSize);

    for (int ig=0; ig<m_AssocGrfs.m_Grfs.GetCount(); ig++)
      {
      CMdlAssocGrf * pG=m_AssocGrfs.m_Grfs[ig];
      m_AssocGrfs.m_Map.SetAt((LPTSTR)(LPCTSTR)pG->m_sNdTag, CMdlAssocGrfMapItem(pG,-1));
      for (int i=0; i<pG->m_Items.GetCount(); i++)
        m_AssocGrfs.m_Map.SetAt((LPTSTR)(LPCTSTR)pG->m_Items[i].m_sTag, CMdlAssocGrfMapItem(pG,i));

#if (0 && dbgAssocGrfs)
      dbgpln("  %s", pG->m_sNdTag);
      for (int i=0; i<pG->m_AssocGrfs.GetCount(); i++)
        dbgpln("    %2i %s", pG->m_AssocGrfs[i].m_eAGType, pG->m_AssocGrfs[i].m_sTag);
#endif
      }

    m_AssocGrfs.m_nLoadSeqNo++;
    long L=m_AssocGrfs.m_Grfs.GetSize();
    FreeAssocGrfs();
#if dbgAssocGrfs
    dbgpln("FE_LoadAllModelAssocGrfs Set Count %5i Seq %5i", L, m_AssocGrfs.m_nLoadSeqNo);
#endif
    return L;
    }
#if dbgAssocGrfs
  dbgpln("FE_LoadAllModelAssocGrfs  %i", 0);
#endif
  //m_AssocGrfs.m_Map.RemoveAll();
  //m_AssocGrfs.m_Grfs.RemoveAll();
  FreeAssocGrfs();
  return 0;
  };

//---------------------------------------------------------------------------

int SFEFlwLib::FE_PrepareAllModelAssocGrfs(dword & LoadSeqNo, CMdlAssocGrfArray *&pGrfs, CMdlAssocGrfMap *&pMap)
  {
  LockAssocGrfs();
#if dbgAssocGrfs
  dbgpln("FE_PrepareAllModelAssocGrfs >>>>>>>>>");
#endif
  if (m_AssocGrfs.m_LoadReqd)
    {
    FreeAssocGrfs();
    CExecObj::LockXO();
    FE_LoadAllModelAssocGrfs();
    CExecObj::UnlockXO();
    LockAssocGrfs();
    }

#if dbgAssocGrfs
  dbgpln("FE_PrepareAllModelAssocGrfs ========= %5i %5i %5i ", LoadSeqNo, m_AssocGrfs.m_nLoadSeqNo, m_AssocGrfs.m_Grfs.GetCount());
#endif

  if (LoadSeqNo!=m_AssocGrfs.m_nLoadSeqNo)
    {
    LoadSeqNo=m_AssocGrfs.m_nLoadSeqNo;

    int N=m_AssocGrfs.m_Grfs.GetCount();
    if (N>0)
      {
      pGrfs=&m_AssocGrfs.m_Grfs;
      pMap=&m_AssocGrfs.m_Map;
#if dbgAssocGrfs
      dbgpln("FE_PrepareAllModelAssocGrfs <<<<<<<<< NewLoad %i", N);
#endif
      return N;
      }

#if dbgAssocGrfs
    dbgpln("FE_PrepareAllModelAssocGrfs <<<<<<<<< NewLoad %i", 0);
#endif
    FreeAssocGrfs();
    return 0; 
    }

#if dbgAssocGrfs
  dbgpln("FE_PrepareAllModelAssocGrfs <<<<<<<<< ");
#endif
  FreeAssocGrfs();
  return 0;
  };

//---------------------------------------------------------------------------
