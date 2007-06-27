//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __BBBASE_CPP
#include "BBBase.h"
//#include "optoff.h"

//====================================================================================

CBBBase::CBBBase(MdlNode* pObj)
  {
  m_pNd = pObj;

  m_pMethod = NULL;

  InitGeneral();
  }

//--------------------------------------------------------------------------

CBBBase::~CBBBase()
  {
  if (m_pMethod)
    {
    delete m_pMethod;
    m_pMethod = NULL;
    //m_iMethodIndex = 0;
    }
  }

//--------------------------------------------------------------------------

void CBBBase::InitGeneral()
  {
  //perhaps move this???...
  MInitialise();
  }

//---------------------------------------------------------------------------

void CBBBase::SetSubClass(LPTSTR MdlName)
  {
  };
  
//--------------------------------------------------------------------------

void CBBBase::DoBuildDataDefn(DataDefnBlk & DDB)
  {
  if (m_pMethod)
    {
    Method.m_pDDB=&DDB;
    if (Method.m_pUnitDef->m_AddUsrPrefix)
      DDB.BeginStruct(m_pNd, "Usr", NULL, DDB_NoPage);
    Method.BuildDataFields();
    if (Method.m_pUnitDef->m_AddUsrPrefix)
      DDB.EndStruct();

    Method.m_pDDB=NULL;
    }
  }

//--------------------------------------------------------------------------

flag CBBBase::DoDataXchg(DataChangeBlk & DCB)
  {
  if (!m_pMethod)
    return false;
  Method.m_pDCB=&DCB;

  if (Method.ExchangeDataFields())
    goto Done;

  MUtilArray &A=Method.m_Utils;
  for (int i=0; i<A.GetSize(); i++)
    if (A[i]->ExchangeDataFields())
      goto Done;

  return false;

Done:
  Method.m_pDCB=NULL;
  return true;
  }

//--------------------------------------------------------------------------

flag CBBBase::DoValidateData(ValidateDataBlk & VDB)
  {
  if (!m_pMethod)
    return true;
  Method.m_pVDB=&VDB;

  flag OK=Method.ValidateDataFields();
  MUtilArray &A=Method.m_Utils;
  for (int i=0; i<A.GetSize(); i++)
    if (!A[i]->ValidateDataFields())
      OK=false;

  Method.m_pVDB=NULL;
  return OK;
  }

//--------------------------------------------------------------------------

flag CBBBase::DoPreStartCheck()
  {
  if (m_pMethod)
    {
    /*if (!(MethodImpl.m_dwMethOptions & MO_Probal) && !m_pNd->NetDynamicMethod())
      {
      LogError(m_pNd->FullObjTag(), 0, "PreStart error : Method not supported in ProBal mode.");
      return false;
      }
    if (!(MethodImpl.m_dwMethOptions & MO_Dynamic) && m_pNd->NetDynamicMethod())
      {
      LogError(m_pNd->FullObjTag(), 0, "PreStart error : Method not supported in Dynamic mode.");
      return false;
      }*/
    Method.m_sErrorMsg="Unknown";
    if (!Method.PreStartCheck())
      {
      LogError(m_pNd->FullObjTag(), 0, "PreStart error : %s", (const char*)Method.m_sErrorMsg);
      return false;
      }
    Method.m_sErrorMsg="";
    }

  return true;
  }

//--------------------------------------------------------------------------

void CBBBase::DoPostConnect(long IONo)
  {
  if (!m_pMethod)
    return;
  Method.PostConnect(IONo); 
  };
void CBBBase::DoPreDisConnect(long IONo)
  {
  if (!m_pMethod)
    return;
  Method.PreDisConnect(IONo); 
  };
//--------------------------------------------------------------------------
void CBBBase::DoSetDatums(CSetDatumsData * pSDD, long nSDD)
  {
  if (!m_pMethod)
    return;
#ifndef _RELEASE   //kga temp
//INCOMPLETECODE(__FILE__, __LINE__);
#endif
  //Method.FlwIOs.Surge.m_pSDD=pSDD;
  //Method.FlwIOs.Surge.m_nSrg=nSDD;
  //Method.SetDatums(); 
  //Method.FlwIOs.Surge.m_pSDD=NULL;
  };
void CBBBase::DoSetSizes()
  {
  if (!m_pMethod)
    return;
  Method.SetSizes(); 
  };
void CBBBase::DoInitialiseSolution()
  {
  if (!m_pMethod)
    return;
  Method.InitialiseSolution(); 
  };

//--------------------------------------------------------------------------

void CBBBase::DoStartSolution()
  {
  //MethodImpl.m_bMixtureInInited=false;
  //MethodImpl.m_bIOStreamsValid=false;
  //MethodImpl.m_NoFlwsIn=-1;
  //MethodImpl.m_NoFlwsOut=-1;

  if (!m_pMethod)
    return;
  Method.StartSolution(); 
  };

//--------------------------------------------------------------------------

void CBBBase::DoStartStep() 
  { 
  //MethodImpl.m_bMixtureInInited=false;
  //MethodImpl.m_NoFlwsIn=-1;
  //MethodImpl.m_NoFlwsOut=-1;

  if (!m_pMethod)
    return;
  Method.StartStep(); 
  };

//--------------------------------------------------------------------------

bool CBBBase::DoPropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!m_pMethod)
    return false;
  return Method.PropagateNetInfo(Ctrl, IONo);
  };

//--------------------------------------------------------------------------

bool CBBBase::DoConfigureJoins() 
  {
  if (!m_pMethod)
    return false;
  return Method.ConfigureJoins(); 
  };
bool CBBBase::DoEvalJoinPressures(long JoinMask)
  { 
  if (!m_pMethod)
    return false;
  Method.m_lJoinMask = JoinMask;
  return Method.EvalJoinPressures(); 
  };
bool CBBBase::DoEvalJoinFlows(long JoinNo)
  { 
  if (!m_pMethod)
    return false;
  return Method.EvalJoinFlows(JoinNo); 
  };
bool CBBBase::DoEvalFlowEquations(eScdFlwEqnTasks Task,/*CSpPropInfo *pProps,*/ long IONo, long FE, long LnkNo) 
  {
  if (!m_pMethod)
    return false;
  return Method.EvalFlowEquations(Task, IONo, FE, LnkNo); 
  };

//--------------------------------------------------------------------------

void CBBBase::DoEvalPBMakeUpReqd(long JoinMask) 
  {
  if (!m_pMethod)
    return;
  Method.m_lJoinMask = JoinMask;
  return Method.EvalMakeUpReqd(); 
  };
void CBBBase::DoEvalPBMakeUpAvail(long JoinMask) 
  {
  if (!m_pMethod)
    return;
  Method.m_lJoinMask = JoinMask;
  return Method.EvalMakeUpAvail(); 
  };


//void CBBBase::DoEvalProductsInit(EvalProductsInitTasks Task) { Method.EvalProductsInit(); };
//void CBBBase::DoEvalProductsSurge() { Method.EvalProductsSurge(); };
//void CBBBase::DoInitFlowInfo() { Method.InitFlowInfo(); };
//void CBBBase::DoStepFlowInfo() { Method.StepFlowInfo(); };
//void CBBBase::DoEvalFlowInfoStart() { Method.EvalFlowInfoStart(); };
//void CBBBase::DoEvalFlowInfoEnd() { Method.EvalFlowInfoEnd(); };
//flag CBBBase::DoTestFlowInfo() { Method.TestFlowInfo(); };
//flag CBBBase::DoEvalPressureSens() { Method.EvalPressureSens(); };
//void CBBBase::DoCompletePressureSens() { Method.CompletePressureSens(); };
//void CBBBase::DoApplyDerivs(double dTime, flagBBBase::DoDbg) { Method.ApplyDerivs(); };
  
//void CBBBase::DoEvalProductsInit(EvalProductsInitTasks Task) { Method.EvalProductsInit(); };
//void CBBBase::DoEvalProductsSurge() { Method.EvalProductsSurge(); };
//--------------------------------------------------------------------------

//void CBBBase::DoEvalProductsSurge(CNodeEvalIndex & NEI)
//  { 
//  if (!m_pMethod)
//    return;
//  Method.m_lJoinMask = JoinMask;
//  Method.EvalProductsSurge(); 
//  };

void CBBBase::DoEvalProducts(CNodeEvalIndex & NEI)
  { 
  if (!m_pMethod)
    return;
  Method.m_lJoinMask = NEI.JoinMask;
  Method.m_lJoinMask = NEI.JoinMask;
  Method.EvalProducts(); 
  };
//void CBBBase::DoEvalDerivs(double StepSize) { Method.EvalDerivs(); };
void CBBBase::DoEvalIntegral(CNodeEvalIndex & NEI) 
  { 
  if (!m_pMethod)
    return;
  Method.m_lJoinMask = NEI.JoinMask;
  Method.EvalIntegral(ICGetTimeInc()); 
  };

void CBBBase::DoEvalDiscrete() 
  {
  if (!m_pMethod)
    return;
  Method.EvalDiscrete(); 
  };
void CBBBase::DoEvalCtrlInitialise(eScdCtrlTasks Tasks) 
  {
  if (!m_pMethod)
    return;
  Method.EvalCtrlInitialise(Tasks); 
  };
void CBBBase::DoEvalCtrlActions(eScdCtrlTasks Tasks) 
  {
  if (!m_pMethod)
    return;
  Method.EvalCtrlActions(Tasks); 
  };
void CBBBase::DoEvalCtrlStrategy(eScdCtrlTasks Tasks) 
  {
  if (!m_pMethod)
    return;
  Method.EvalCtrlStrategy(Tasks); 
  };
void CBBBase::DoEvalCtrlTerminate(eScdCtrlTasks Tasks) 
  {
  if (!m_pMethod)
    return;
  Method.EvalCtrlTerminate(Tasks); 
  };
void CBBBase::DoEvalStatistics(eScdCtrlTasks Tasks) 
  {
  if (!m_pMethod)
    return;
  Method.EvalStatistics(Tasks); 
  };

void CBBBase::DoEvalPowerAvailable() 
  {
  if (!m_pMethod)
    return;
  Method.EvalPowerAvailable(); 
  };

void CBBBase::DoEvalPowerRequired() 
  {
  if (!m_pMethod)
    return;
  Method.EvalPowerRequired(); 
  };

void CBBBase::DoEvalState() 
  {
  if (!m_pMethod)
    return;
  Method.EvalState(); 
  };

void CBBBase::DoOnAppActivate(BOOL bActive)
  {
  if (!m_pMethod)
    return;
  MUtilArray &A=Method.m_Utils;
  for (int i=0; i<A.GetSize(); i++)
    A[i]->OnAppActivate(bActive!=0);
  };

int CBBBase::DoFilesUsed(CFilesUsedArray & Files)
  {
  int n=0;
  if (!m_pMethod)
    return n;

  MUtilArray &A=Method.m_Utils;
  for (int i=0; i<A.GetSize(); i++)
    n+=A[i]->FilesUsed(Files);

  return n;
  };

void CBBBase::DoClosureInfo(MClosureInfo & CI)
  {
  Method.ClosureInfo(CI);
  };

void CBBBase::DoSetState(eScdMdlStateActs RqdState)
  {
  if (!m_pMethod)
    return;
  Method.SetState(MStatesToSet(RqdState)); 
  }

flag CBBBase::DoGetModelAction(CMdlActionArray & Acts)
  {
  if (!m_pMethod)
    return false;
  return Method.GetModelAction(Acts);
  };

flag CBBBase::DoSetModelAction(CMdlAction & Act)
  {
  if (!m_pMethod)
    return false;
  return Method.SetModelAction(Act);
  };
              
flag CBBBase::DoGetModelGraphic(CMdlGraphicArray & Grfs)
  {
  if (!m_pMethod)
    return false;
  return Method.GetModelGraphic(Grfs);
  };

flag CBBBase::DoOperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
  {
  if (!m_pMethod)
    return false;
  return Method.OperateModelGraphic(Wnd, Grf);
  };


// Flash Train - MacroMdls
void CBBBase::DoMacroMdlEvaluate(eScdMacroMdlEvals Eval)  
  {
  if (!m_pMethod)
    return;
  Method.MacroMdlEvaluate(Eval);
  };
flag CBBBase::DoMacroMdlValidNd(int iIONo)    
  {
  if (!m_pMethod)
    return true;
  return Method.MacroMdlValidNd(iIONo);
  };
void CBBBase::DoMacroMdlAddNd(int iIONo)      
  {
  if (!m_pMethod)
    return;
  Method.MacroMdlAddNd(iIONo);
  };
void CBBBase::DoMacroMdlRemoveNd(int iIONo)   
  {
  if (!m_pMethod)
    return;
  Method.MacroMdlRemoveNd(iIONo);
  };
CMacroMdlBase* CBBBase::DoMacroMdlActivate()     
  {
  if (!m_pMethod)
    return NULL;
  return Method.MacroMdlActivate();
  };
void CBBBase::DoMacroMdlDeactivate()          
  {
  if (!m_pMethod)
    return;
  Method.MacroMdlDeactivate();
  };

//===========================================================================

