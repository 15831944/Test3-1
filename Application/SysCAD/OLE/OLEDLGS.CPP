//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __OLEDLGS_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "ole_base.h"
#include "..\resource.h"
#include "oledlgs.h"
#include "gpwfuncs.h"
#include "scd_wm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

COleInfoWnd::COleInfoWnd(CWnd* pParent /*=NULL*/)
  : CDialog(COleInfoWnd::IDD, pParent)
  {
  //{{AFX_DATA_INIT(COleInfoWnd)
  //}}AFX_DATA_INIT
  m_Count = 0;
  m_Desc = "";
  m_Name = "";
  Create(COleInfoWnd::IDD, pParent); // NonModal Create
  ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

void COleInfoWnd::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COleInfoWnd)
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COleInfoWnd, CDialog)
  //{{AFX_MSG_MAP(COleInfoWnd)
  ON_MESSAGE(WMU_OLEINFOUPDATE, OnUpdateData)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

LRESULT COleInfoWnd::OnUpdateData(WPARAM wParam, LPARAM lParam)
  {
  COleInfoMsg* pMsg = (COleInfoMsg*)lParam;
  if ((pMsg->Flags & OleInfoCountFlag) && pMsg->iCount<1)
    ShowWindow(SW_HIDE);
  else
    {
    if (!IsWindowVisible())
      {
      WINDOWPLACEMENT WP;
      WP.length = sizeof(WP);
      if (GetWindowPlacement(&WP))
        {
        const int Offset = 100; //width of query info window IDD_QUERYINFO
        RECT R;
        GetParent()->GetWindowRect(&R);
        WP.rcNormalPosition.right = WP.rcNormalPosition.right - WP.rcNormalPosition.left + R.left + 3 + Offset;
        WP.rcNormalPosition.left = R.left + 3 + Offset;
        int bottom = R.bottom - 2;
        if ((pStatusBar->GetStyle() & WS_VISIBLE) != 0)
          bottom -= 18; //height of statusbar
        WP.rcNormalPosition.top = bottom - (WP.rcNormalPosition.bottom - WP.rcNormalPosition.top);
        WP.rcNormalPosition.bottom = bottom;
        WP.showCmd = SW_SHOWNOACTIVATE;
        WP.length = sizeof(WP);
        SetWindowPlacement(&WP);
        }
      }
    if (pMsg->Flags & OleInfoCountFlag)
      m_Count = pMsg->iCount;
    if (pMsg->Flags & OleInfoNameFlag)
      m_Name = pMsg->sName();
    if (pMsg->Flags & OleInfoTxtFlag)
      m_Desc = pMsg->sTxt();
    char Buff[512];
    if (m_Count>1)
      sprintf(Buff, "%d  %s  %s", m_Count, (const char*)m_Name, (const char*)m_Desc);
    else
      sprintf(Buff, " %s  %s", (const char*)m_Name, (const char*)m_Desc);
    SetDlgItemText(IDC_TXTMESSAGE, Buff);
    UpdateWindow();
    }
  delete pMsg;
  return True;
  }

//===========================================================================
