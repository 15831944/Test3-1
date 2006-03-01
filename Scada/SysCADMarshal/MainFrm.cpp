// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "SysCADMarshal.h"

#include "MainFrm.h"
#include "ListView.h"
#include "TreeView.h"
#include "ErrorView.h"
#include "slotmngr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(CMainFrame)
  ON_WM_CREATE()
  ON_COMMAND(ID_CONFIGURATION_LOADDB, OnConfigurationLoaddb)
  ON_UPDATE_COMMAND_UI(ID_CONFIGURATION_LOADDB, OnUpdateConfigurationLoaddb)
  ON_COMMAND(ID_CONFIGURATION_ADDDB, OnConfigurationAdddb)
  ON_UPDATE_COMMAND_UI(ID_CONFIGURATION_ADDDB, OnUpdateConfigurationAdddb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
static UINT indicators[] =
  {
  ID_SEPARATOR, // status line indicator
  ID_INDICATOR_SLOTS,   
  ID_INDICATOR_LINKS,   
  ID_INDICATOR_HOLDS,   
  ID_INDICATOR_QUEUES, 
  ID_INDICATOR_CHANGES,
  //ID_INDICATOR_CAPS,
  //ID_INDICATOR_NUM,
  //ID_INDICATOR_SCRL,
  };

/////////////////////////////////////////////////////////////////////////////

// CMainFrame construction/destruction
CMainFrame::CMainFrame(void)
  {
  // TODO: add member initialization code here
  }

CMainFrame::~CMainFrame(void)
  {
  }

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if(CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
       CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
    TRACE0("Failed to create toolbar\n");
    return -1;  // fail to create
    }

  if(!m_wndStatusBar.Create(this)
     || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
    {
    TRACE0("Failed to create status bar\n");
    return -1;  // fail to create
    }

  // TODO: Delete these three lines if you don't want the toolbar to
  //  be dockable
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  return 0;
  }

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
  {
  if(!CFrameWnd::PreCreateWindow(cs)) return FALSE;

  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  return TRUE;
  }

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid(void) const
  {
  CFrameWnd::AssertValid();
  }

void CMainFrame::Dump(CDumpContext &dc) const
  {
  CFrameWnd::Dump(dc);
  }
#endif //_DEBUG
  ///
  ///////////////////////////////////////////////////////////////////////////////

// CMainFrame message handlers
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
  {
  if(!m_wndSplitter.CreateStatic(this, 1, 2))
    {
    TRACE0("Failed to create split bar ");
    return FALSE; // failed to create
    }

  CRect rect;
  GetClientRect(&rect);

  CSize size(rect.Size());
  CSize size1(size);
  size.cx /= 4;
  if(size.cx > 120)
    size.cx = 120;

  if(!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CDeviceView), size, pContext))
    {
    TRACE0("Failed to create device pane");
    return FALSE; // failed to create
    }

  if(!m_wndSplitter2.CreateStatic(&m_wndSplitter, 2, 1,
    WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
    m_wndSplitter.IdFromRowCol(0, 1)
    ))
    {
    TRACE0("Failed to create split bar ");
    return FALSE; // failed to create
    }

  size.cx = size1.cx - size.cx;
  size.cy = (size1.cy*3)/4;


  if(!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CTagView), size, pContext))
    {
    TRACE0("Failed to create device pane");
    return FALSE; // failed to create
    }

  if(!m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CErrorView), size, pContext))
    {
    TRACE0("Failed to create device pane");
    return FALSE; // failed to create
    }

  return TRUE;

  //return CFrameWnd::OnCreateClient(lpcs, pContext);
  }

void CMainFrame::OnConfigurationLoaddb()
  {
  // TODO: Add your command handler code here

  }

void CMainFrame::OnUpdateConfigurationLoaddb(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(!gs_SlotMngr.ScdRunning);
  }

void CMainFrame::OnConfigurationAdddb()
  {
  // TODO: Add your command handler code here

  }

void CMainFrame::OnUpdateConfigurationAdddb(CCmdUI* pCmdUI)
  {
  // TODO: Add your command update UI handler code here

  }
