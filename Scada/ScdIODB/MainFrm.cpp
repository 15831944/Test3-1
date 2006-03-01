//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  MainFrm.cpp
//  $Author    :  Jim Hansen
//
//  Description:
//
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"

#include "TreeView.h"
#include "ListView.h"
#include "Splash.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
   ON_COMMAND_EX(ID_VIEW_STYLEBAR, OnBarCheck)
   ON_UPDATE_COMMAND_UI(ID_VIEW_STYLEBAR, OnUpdateControlBarMenu)
   ON_COMMAND(ID_UPDATEALLRQST, OnUpdateallRqst)
   //{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
	ON_WM_TIMER()
   ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
   ID_SEPARATOR,           // status line indicator
   ID_SUMMARY,
};

//**************************************************************************
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
   m_UpdRqstCount=0;
   m_UpdRqstCountLast=0;
}

CMainFrame::~CMainFrame()
{
}

//**************************************************************************
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   CRect rect;
   GetClientRect( &rect );
   CSize size( rect.Size() );
   if (size.cx>600)
     {
     SetWindowPos(NULL, 0, 0, 600, size.cy, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);
     }

   if (!m_wndToolBar.Create(this) ||
      !m_wndToolBar.LoadToolBar(IDR_TOOLBAR))
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }

   if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }

   CSplashWnd::ShowSplashScreen(this);

   m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
      CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);

   if( ((CScdTgSrcApp*)AfxGetApp())->m_bAutoClose )
      SetTimer( WM_POLLCLOSE, 1000, NULL );

   SetTimer( WM_POLLUPDATE, 500, NULL );

   return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
   CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
   CFrameWnd::Dump(dc);
}

#endif //_DEBUG

//**************************************************************************
// CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
   if (!m_wndSplitter.CreateStatic(this, 1, 2))
      {
      TRACE0("Failed to create split bar ");
      return FALSE;    // failed to create
      }

   CRect rect;
   GetClientRect( &rect );
   CSize size( rect.Size() );
   CSize size1( size );
   size.cx /= 4;
   if (size.cx>150)
     size.cx=150;

   if( !m_wndSplitter.CreateView(0, 0,
                                 RUNTIME_CLASS(CDeviceView),
                                 size,
                                 pContext))
      {
      TRACE0("Failed to create device pane");
      return FALSE;    // failed to create
      }

   size.cx = size1.cx-size.cx;
   if( !m_wndSplitter.CreateView(0, 1,
                                 RUNTIME_CLASS(CTagView),
                                 size,
                                 pContext))
      {
      TRACE0("Failed to create device pane");
      return FALSE;    // failed to create
      }

   return TRUE;
}

//**************************************************************************
// Don't let the app close while clients are connected
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
   UINT nItemID = LOWORD(wParam);
   if ((nItemID==0xe141) && _Module.GetLockCount())
   {
      if (MessageBox(_T("OPC Clients are currently connected to this server, Exit anyway?"),
                     _T("ScdIODB"), MB_YESNO|MB_ICONWARNING ) != IDYES )
         return TRUE;

      ((CScdTgSrcApp*)AfxGetApp())->ForceExit();
   }

   // route as normal command
   return CFrameWnd::OnCommand(wParam, lParam);
}

//**************************************************************************
// Don't let the app close while clients are connected
void CMainFrame::OnSysCommand(UINT nID, LONG lParam)
{
   UINT nItemID = (nID & 0xFFF0);
   if ((nItemID==SC_CLOSE) && _Module.GetLockCount())
   {
      if (MessageBox(_T("OPC Clients are currently connected to this server, Exit anyway?"),
                     _T("ScdIODB"), MB_YESNO|MB_ICONSTOP ) != IDYES )
         return;
      ((CScdTgSrcApp*)AfxGetApp())->ForceExit();
   }

   // call default functionality
   CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
   if((nIDEvent==WM_POLLCLOSE) && (_Module.GetLockCount()==0) )
   {
      KillTimer(WM_POLLCLOSE);
TRACE0("WM_POLLCLOSE\n");
      CFrameWnd::OnCommand(0xe141, 0);
      return;
   }

   if (nIDEvent==WM_POLLUPDATE)
     {
     if (m_UpdRqstCount>0 && m_UpdRqstCount==m_UpdRqstCountLast)
       {
TRACE0("WM_POLLUPDATE\n");
       theApp.m_pMainWnd/*AfxGetMainWnd()*/->PostMessage(WM_COMMAND, ID_UPDATEALL, 0);
       m_UpdRqstCount=0;
       }

     m_UpdRqstCountLast=m_UpdRqstCount;
     }

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateallRqst()
  {
  m_UpdRqstCount++;
  }

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDocument* pDocument = GetActiveDocument();
	if (pDocument)
    pDocument->SetModifiedFlag(FALSE);
	
	CFrameWnd::OnClose();
}
