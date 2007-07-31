//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "dde_mngr.h"
#include "dde_exec.h"
#include "resource.h"
#include "dde_dlgs.h"
#include "scd_wm.h"

#define dbgDde       0
   
#if dbgDde
#include "dbgmngr.h"
static CDbgMngr dbgDDE("General", "DDE");
#endif


//==========================================================================

CScdDDEManager* CScdDDEManager::pTheMngr = NULL;

CScdDDEManager::CScdDDEManager()
  {
  pTheDDEStatsDlg = NULL;
  pXDde = new CDdeExec();
  }

//--------------------------------------------------------------------------

CScdDDEManager::~CScdDDEManager()
  {
  delete pXDde;
  }

//--------------------------------------------------------------------------

CCommsServerManagerBase* CScdDDEManager::Init()
  {
  ASSERT(pTheMngr==NULL);
  pTheMngr = new CScdDDEManager;
  return pTheMngr;
  }

//--------------------------------------------------------------------------

bool CScdDDEManager::Stop()
  {
  pTheMngr->CloseStatusWnd();
  delete pTheMngr;
  pTheMngr = NULL;
  return true;
  }

//--------------------------------------------------------------------------

char* CScdDDEManager::StatusStr(Strng & Status)
  {
  /*if (pTheCB->iWriteFailCnt)
    Status.Set("Tags:%d  W:%d R:%d  (WriteErrors:%d)", pTheCB->iTagCnt, pTheCB->iWriteOKCnt, pTheCB->iReadOKCnt, pTheCB->iWriteFailCnt);
  else
    Status.Set("Tags:%d  W:%d R:%d", pTheCB->iTagCnt, pTheCB->iWriteOKCnt, pTheCB->iReadOKCnt);*/
  return Status();
  }

//--------------------------------------------------------------------------

bool CScdDDEManager::OpenStatusWnd()
  {
  if (pTheDDEStatsDlg==NULL)
    pTheDDEStatsDlg = new CDDEStatsDlg(this);
  return true;
  }

//--------------------------------------------------------------------------

bool CScdDDEManager::CloseStatusWnd()
  {
  if (pTheDDEStatsDlg)
    {
    pTheDDEStatsDlg->DestroyWindow();
    pTheDDEStatsDlg = NULL;
    }
  return true;
  }

//--------------------------------------------------------------------------

void CScdDDEManager::UpdateStatusWnd(WPARAM wParam/*=0*/, LPARAM lParam/*=0*/)
  {
  if (pTheDDEStatsDlg)
    {
    if (wParam & SUB_UPDATE_SUBSCHANGED)
      {
      pTheDDEStatsDlg->bSubsChanged = 1;
      return;
      }

    if (wParam & SUB_UPDATE_SENDMSG)
      pTheDDEStatsDlg->SendMessage(WMU_DDEUPDATESTATS, wParam, lParam);
    else
      pTheDDEStatsDlg->PostMessage(WMU_DDEUPDATESTATS, wParam, lParam);
    }
  }

//--------------------------------------------------------------------------
//==========================================================================
