//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#define __APPRUNMNGR_CPP
#include "sc_defs.h"
#include "mdlrunmngr.h"
#include "apprunmngr.h"
#include "project.h"
#include "msgwnd.h"
#include "licbase.h"
#include "slvtool.h"
#include "errcodes.h"
#include "dbgmngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgPrjExec 01

//===========================================================================
//
//  CAppRunManager gs_PE;
//
//===========================================================================

//===========================================================================
//
//
//
//===========================================================================

CAppRunManager::CAppRunManager()
  {
//  m_ComUIActive=true;
  }

//---------------------------------------------------------------------------

CAppRunManager::~CAppRunManager()
  {
  }

//---------------------------------------------------------------------------

void CAppRunManager::GotoIdleMode(bool Done)
  {
  if (!Done)
    {
    if (XDoingPB())
      {
      SolveTool.Open();
      XSetSolveTool(&SolveTool);
      }
    else
      {
      SolveTool.Close();
      XSetSolveTool(NULL);
      }

    CMsgWindow::SetMsgsIncludeFlag(false);
    gs_pPrj->CheckLicense(TRUE);
    }
  //XGotoIdleMode();
  }

void CAppRunManager::GotoEditMode(bool Done)
  {
  }
void CAppRunManager::GotoRunMode(bool Done, CExecutive::RqdRunMode Md)
  {
  if (!Done)
    {
    if (XDoingPB())
      {
      SolveTool.Open();
      XSetSolveTool(&SolveTool);
      }
    else
      {
      SolveTool.Close();
      XSetSolveTool(NULL);
      }
    gs_pPrj->CheckLicense(TRUE);
    }
  }

void CAppRunManager::DoStep(bool Done, CDoOneStepInfo Info)
  {
  if (!Done)
    {
    if (XDoingPB())
      {
      SolveTool.Open();
      XSetSolveTool(&SolveTool);
      }
    else
      {
      SolveTool.Close();
      XSetSolveTool(NULL);
      }
    }
  }

void CAppRunManager::SetDynamicMode(bool Done)
  {
  if (!Done)
    {
    gs_pPrj->CheckLicense(TRUE);
    SolveTool.Close();
    }
  }

void CAppRunManager::SetProbalMode(bool Done)
  {
  if (!Done)
    gs_pPrj->CheckLicense(TRUE);
  gs_Exec.SetGlblRunModes(SM_Direct, SM_All);
  if (Done)
    SolveTool.Open();
  }

//---------------------------------------------------------------------------

void CAppRunManager::DoEO_Starting()
  {
  //RefsHelper.Starting();
  }

//---------------------------------------------------------------------------

void CAppRunManager::DoEO_Execute()
  {
  //RefsHelper.Execute();
  }

//---------------------------------------------------------------------------
//===========================================================================
//
//
//
//===========================================================================
