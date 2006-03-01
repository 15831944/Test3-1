//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#define __MDLRUNMNGR_CPP
#include "sc_defs.h"
#include "mdlrunmngr.h"
//#include "project.h"
//#include "msgwnd.h"
//#include "licbase.h"
//#include "slvtool.h"
#include "errcodes.h"
#include "dbgmngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgMdlRunMngr 01

#if dbgMdlRunMngr
static CDbgMngr dbgActions  ("RunManager", "Actions");
#endif

//===========================================================================

CMdlRunManager gs_TheRunMngr;

//===========================================================================

CMdlRunManager::CMdlRunManager()
  {
  m_pAppMngr=NULL;
  m_ComUIActive=true;
  }

//---------------------------------------------------------------------------

CMdlRunManager::~CMdlRunManager()
  {
  }

//---------------------------------------------------------------------------

flag CMdlRunManager::Stopped() { return XStopped(); }
flag CMdlRunManager::Idling()  { return XRunning() && XWaiting(); }
flag CMdlRunManager::Running() { return XRunning() && !XWaiting(); }
flag CMdlRunManager::IdlingDsp()  { return XRunning() && XPaused(); }
flag CMdlRunManager::RunningDsp() { return XRunning() && !XPaused(); }

void CMdlRunManager::GotoIdleMode()
  {
  #if dbgMdlRunMngr
	if (dbgActions())
		dbgpln("------ RunMngr ------> GotoIdleMode");
  #endif
  LogSeparator("Run",0);
  if (m_pAppMngr)
    m_pAppMngr->GotoIdleMode(0);

  //gs_pPrj->CheckLicense(TRUE);
  XGotoIdleMode();
  if (m_pAppMngr)
    m_pAppMngr->GotoIdleMode(1);
  }

void CMdlRunManager::GotoEditMode()
  {
  #if dbgMdlRunMngr
	if (dbgActions())
		dbgpln("------ RunMngr ------> GotoEditMode");
  #endif
  if (m_pAppMngr)
    m_pAppMngr->GotoEditMode(0);
  XGotoEditMode();
  if (m_pAppMngr)
    m_pAppMngr->GotoEditMode(1);
  }
void CMdlRunManager::GotoRunMode(CExecutive::RqdRunMode Md)
  {
  #if dbgMdlRunMngr
	if (dbgActions())
		dbgpln("------ RunMngr ------> GotoRunMode");
  #endif
  LogSeparator("Run",0);
  switch (Md)
    {
    case CExecutive::RRM_PBMd:
      LogSeparator("Set Probal Mode",0);
      SetProbalMode();
      break;
    case CExecutive::RRM_DynMd:
      LogSeparator("Set Dynamic Mode",0);
      SetDynamicMode();
      break;
    case CExecutive::RRM_DynMdFlow:
      LogSeparator("Set Dynamic Mode (Flow)",0);
      SetDynFlowMode();//OrFull(true);
      break;
    case CExecutive::RRM_DynMdFull:
      LogSeparator("Set Dynamic Mode (Full)",0);
      SetDynFullMode();//FlowOrFull(false);
      break;
    case CExecutive::RRM_Current:
      break;
    }


  if (m_pAppMngr)
    m_pAppMngr->GotoRunMode(0, Md);
  //gs_pPrj->CheckLicense(TRUE);
  XGotoRunMode(Md);
  if (m_pAppMngr)
    m_pAppMngr->GotoRunMode(1, Md);
  }

void CMdlRunManager::DoStep(CDoOneStepInfo Info)
  {
  #if dbgMdlRunMngr
	if (dbgActions())
		dbgpln("------ RunMngr ------> DoStep");
  #endif
  LogSeparator("Step",0);
  if (m_pAppMngr)
    m_pAppMngr->DoStep(0, Info);
  XDoOneStep(Info);
  if (m_pAppMngr)
    m_pAppMngr->DoStep(1, Info);
  }

void CMdlRunManager::SetDynamicMode()
  {
  if (m_pAppMngr)
    m_pAppMngr->SetDynamicMode(1);
  //gs_Exec.SetSolveMode(DYNMODE);
  //gs_Exec.SetDynMode(DFlow ? DYNFLOWMODE : DYNFULLMODE);
  if (gs_Exec.GlblProbalMode())
    {
    long SM=TaggedObject::GetPermissableGlblRunModes(SM_DynBoth);
    if (SM&SM_DynXfer)
      gs_Exec.SetGlblRunModes(SM_DynXfer, SM_All);
    else if (SM&SM_DynFull)
      gs_Exec.SetGlblRunModes(SM_DynFull, SM_All);
    else
      {
      ASSERT_ALWAYS(false, "No Permissable Dynamic Modes");
      }
    }
  else
    gs_Exec.SetGlblRunModes(gs_Exec.GlblRunModes(), SM_All);
  if (m_pAppMngr)
    m_pAppMngr->SetDynamicMode(0);
  }

void CMdlRunManager::SetDynFlowMode()
  {
  if (m_pAppMngr)
    m_pAppMngr->SetDynamicMode(1);
  ////gs_Exec.SetSolveMode(DYNMODE);
  //gs_Exec.SetDynFlowMode();//(DFlow ? DYNFLOWMODE : DYNFULLMODE);
  gs_Exec.SetGlblRunModes(SM_DynXfer, SM_All);

  if (m_pAppMngr)
    m_pAppMngr->SetDynamicMode(0);
  }

void CMdlRunManager::SetDynFullMode()
  {
  if (m_pAppMngr)
    m_pAppMngr->SetDynamicMode(1);
  ////gs_Exec.SetSolveMode(DYNMODE);
  //gs_Exec.SetDynFullMode();//(DFlow ? DYNFLOWMODE : DYNFULLMODE);
  gs_Exec.SetGlblRunModes(SM_DynFull, SM_All);
  if (m_pAppMngr)
    m_pAppMngr->SetDynamicMode(0);
  }

void CMdlRunManager::SetProbalMode()
  {
  if (m_pAppMngr)
    m_pAppMngr->SetProbalMode(0);
  gs_Exec.SetGlblRunModes(SM_Probal, SM_All);
  if (m_pAppMngr)
    m_pAppMngr->SetProbalMode(1);
  }

//---------------------------------------------------------------------------

void CMdlRunManager::DoEO_Starting()
  {
  if (m_pAppMngr)
    m_pAppMngr->DoEO_Starting();
  }

//---------------------------------------------------------------------------

void CMdlRunManager::DoEO_Execute()
  {
  if (m_pAppMngr)
    m_pAppMngr->DoEO_Execute();
  }

//===========================================================================
//
//
//
//===========================================================================
