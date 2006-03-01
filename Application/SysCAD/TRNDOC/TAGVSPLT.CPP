//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "tagvdoc.h"
#include "fixedit.h"
#include "tagvtrnd.h"
#include "tagvtext.h"
#include "tagvsplt.h"
#include "tagvdlgs.h"
#include "scd_wm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CTagVwSplit, CMDIChildWnd)

CTagVwSplit::CTagVwSplit()
  {
  pDoc = NULL;
  }

//---------------------------------------------------------------------------

CTagVwSplit::~CTagVwSplit()
  {
  }

//---------------------------------------------------------------------------

BOOL CTagVwSplit::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
  {
  if (!m_wndSplitter.CreateStatic(this, 2, 1)) 
    return FALSE;
  if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTagVwTrend), CSize(lpcs->cx, (lpcs->cy*5)/10), pContext))
    return FALSE;
  if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CTagVwText), CSize(lpcs->cx, (lpcs->cy*5)/10), pContext))
    return FALSE;

  ASSERT(pContext->m_pCurrentDoc->IsKindOf(RUNTIME_CLASS(CTagVwDoc)));
  ASSERT(m_wndSplitter.GetPane(1,0)->IsKindOf(RUNTIME_CLASS(CTagVwText)));
  ASSERT(m_wndSplitter.GetPane(0,0)->IsKindOf(RUNTIME_CLASS(CTagVwTrend)));
  pDoc = (CTagVwDoc*)pContext->m_pCurrentDoc;
  pDoc->pTrnd = (CView*)m_wndSplitter.GetPane(0,0);
  pDoc->pText = (CView*)m_wndSplitter.GetPane(1,0);
  return TRUE;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTagVwSplit, CMDIChildWnd)
  //{{AFX_MSG_MAP(CTagVwSplit)
  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
  //ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
  ON_WM_SETFOCUS()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CTagVwSplit::OnFilePrint()
  {
  CTrendPrintDlg Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    if (Dlg.m_PrintTrendLine)
      ((CTagVwTrend*)m_wndSplitter.GetPane(0,0))->DoOnFilePrint(Dlg.m_TrueColors);
    if (Dlg.m_PrintTrendText)
      ((CTagVwText*)m_wndSplitter.GetPane(1,0))->DoOnFilePrint(Dlg.m_TrueColors);
    }
  }

//---------------------------------------------------------------------------

BOOL CTagVwSplit::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  if (message==WM_CLOSE)
    {//intercept close message...
    if (gs_Exec.Busy())
      {
      LogError("SysCAD", 0, "Must not be running");
      return True;
      }
    if (/*TaggedObject::ProbalMode() && */AfxMessageBox("Are you sure you want to close the trend window?", MB_YESNO)==IDNO) //always prompt before close in probal mode
      return True;
    if (GetActiveDocument())
      GetActiveDocument()->SetModifiedFlag(FALSE);
    }
  return CMDIChildWnd::OnWndMsg(message, wParam, lParam, pResult);
  }

//---------------------------------------------------------------------------

void CTagVwSplit::OnSetFocus(CWnd* pOldWnd) 
  {
  if (gs_pCmd->BusyDocument())
    {
    POSITION pos = gs_pCmd->BusyDocument()->GetFirstViewPosition();
    ASSERT(pos);
    CView* pView = gs_pCmd->BusyDocument()->GetNextView(pos);
    if (GetActiveView()!=pView)
      pView->PostMessage(WMU_DOSETFOCUS, 0 , (LPARAM)pView);
    }
  else
    CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CTagVwSplit::OnSize(UINT nType, int cx, int cy) 
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(TRUE);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(FALSE);
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CTagVwSplitC, CTagVwSplit)

CTagVwSplitC::CTagVwSplitC()
  {
  //pDoc = NULL;
  }

//---------------------------------------------------------------------------

CTagVwSplitC::~CTagVwSplitC()
  {
  }

//---------------------------------------------------------------------------

BOOL CTagVwSplitC::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
  {
  if (!m_wndSplitter.CreateStatic(this, 2, 1)) 
    return FALSE;
  if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTagVwTrend), CSize(lpcs->cx, (lpcs->cy*5)/10), pContext))
    return FALSE;
  if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CTagVwTextC), CSize(lpcs->cx, (lpcs->cy*5)/10), pContext))
    return FALSE;

  ASSERT(pContext->m_pCurrentDoc->IsKindOf(RUNTIME_CLASS(CTagVwDocC)));
  ASSERT(m_wndSplitter.GetPane(1,0)->IsKindOf(RUNTIME_CLASS(CTagVwTextC)));
  ASSERT(m_wndSplitter.GetPane(0,0)->IsKindOf(RUNTIME_CLASS(CTagVwTrend)));
  pDoc = (CTagVwDoc*)pContext->m_pCurrentDoc;
  pDoc->pTrnd = (CView*)m_wndSplitter.GetPane(0,0);
  pDoc->pText = (CView*)m_wndSplitter.GetPane(1,0);
  pDocC = (CTagVwDocC*)pContext->m_pCurrentDoc;
  return TRUE;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTagVwSplitC, CTagVwSplit)
  //{{AFX_MSG_MAP(CTagVwSplit)
//  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
//  //ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
//  ON_WM_SETFOCUS()
//  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
/*
void CTagVwSplit::OnFilePrint()
  {
  CTrendPrintDlg Dlg;
  if (Dlg.DoModal()==IDOK)
    {
    if (Dlg.m_PrintTrendLine)
      ((CTagVwTrend*)m_wndSplitter.GetPane(0,0))->DoOnFilePrint(Dlg.m_TrueColors);
    if (Dlg.m_PrintTrendText)
      ((CTagVwTextC*)m_wndSplitter.GetPane(1,0))->DoOnFilePrint(Dlg.m_TrueColors);
    }
  }

//---------------------------------------------------------------------------

BOOL CTagVwSplit::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  if (message==WM_CLOSE)
    {//intercept close message...
    if (gs_Exec.Busy())
      {
      LogError("SysCAD", 0, "Must not be running");
      return True;
      }
    if (GetActiveDocument())
      GetActiveDocument()->SetModifiedFlag(FALSE);
    }
  return CMDIChildWnd::OnWndMsg(message, wParam, lParam, pResult);
  }

//---------------------------------------------------------------------------

void CTagVwSplit::OnSetFocus(CWnd* pOldWnd) 
  {
  if (gs_pCmd->BusyDocument())
    {
    POSITION pos = gs_pCmd->BusyDocument()->GetFirstViewPosition();
    ASSERT(pos);
    CView* pView = gs_pCmd->BusyDocument()->GetNextView(pos);
    if (GetActiveView()!=pView)
      pView->PostMessage(WMU_DOSETFOCUS, 0 , (LPARAM)pView);
    }
  else
    CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CTagVwSplit::OnSize(UINT nType, int cx, int cy) 
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(TRUE);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(FALSE);
  }
*/
//---------------------------------------------------------------------------

