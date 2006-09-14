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
#include "mainfrm.h"
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
  m_CreateDone=false;
  m_PrevClientX=0;
  m_PrevClientY=0;
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
  m_CreateDone=true;
  return TRUE;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTagVwSplit, CMDIChildWnd)
  //{{AFX_MSG_MAP(CTagVwSplit)
  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
  //ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
  ON_WM_SETFOCUS()
  ON_WM_SIZE()
  ON_WM_MOVE()
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
    if (/*TaggedObject::NetProbalMethod() && */AfxMessageBox("Are you sure you want to close the trend window?", MB_YESNO)==IDNO) //always prompt before close in probal mode
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

//void CTagVwSplit::OnSize(UINT nType, int cx, int cy) 
//  {
//  CMDIChildWnd::OnSize(nType, cx, cy);
//  if (nType==SIZE_MAXIMIZED)
//    CWindowLists::SetWndMaxMode(TRUE);
//  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
//    CWindowLists::SetWndMaxMode(FALSE);
//
//  if (m_CreateDone && gs_pPrj->m_TrndBehaviour && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
//    {
//    CIncDecFlag IDF(gs_BlockSizeAdjustment);
//    CWindowLists WL;
//    if (WL.BuildSingleList()<0)
//      return;
//
//    if (gs_FileNewFlag)
//      {
//      for (int i=0; i<WL.TrndWnds.GetCount(); i++)
//        {
//        if (WL.TrndWnds[i]!=this)
//          {
//          WINDOWPLACEMENT wp;
//          wp.length = sizeof(wp);
//          WL.TrndWnds[i]->GetWindowPlacement(&wp);
//          int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
//          int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;
//
//          // set new one
//          wp.length = sizeof(wp);
//          GetWindowPlacement(&wp);
//          wp.rcNormalPosition.right=wp.rcNormalPosition.left+W;
//          wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+H;
//          wp.length = sizeof(wp);
//          SetWindowPlacement(&wp);
//          break; // done
//          }
//        }
//      }
//    else
//      {
//      WINDOWPLACEMENT wp;
//      wp.length = sizeof(wp);
//      GetWindowPlacement(&wp);
//      int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
//      int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;
//
//      for (int i=0; i<WL.TrndWnds.GetCount(); i++)
//        {
//        if (WL.TrndWnds[i]!=this)
//          {
//
//          WINDOWPLACEMENT wp;
//          wp.length = sizeof(wp);
//          WL.TrndWnds[i]->GetWindowPlacement(&wp);
//          wp.rcNormalPosition.right=wp.rcNormalPosition.left+W;
//          wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+H;
//          wp.length = sizeof(wp);
//          WL.TrndWnds[i]->SetWindowPlacement(&wp);
//          }
//        }
//      }
//    }
//  }

//---------------------------------------------------------------------------

void CTagVwSplit::OnSize(UINT nType, int cx, int cy)
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(TRUE);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(FALSE);

  if (m_CreateDone && gs_pPrj->m_TrndBehaviour && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CAutoIncDec AID(gs_BlockSizeAdjustment);
    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    if (gs_FileNewFlag)
      {
      if (WL.TrndWnds.GetCount()==1) // First
        {
        CRect IRect;
        CWnd * p=ScdMainWnd();
        if (dynamic_cast<CMainFrame*>(p)->GetInitRect(2, IRect))
          {
          WINDOWPLACEMENT wp;
          wp.length = sizeof(wp);
          GetWindowPlacement(&wp);
          wp.rcNormalPosition=IRect;
          SetWindowPlacement(&wp);
          }

        //WINDOWPLACEMENT wp;
        //wp.length = sizeof(wp);
        //GetWindowPlacement(&wp);
        //int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
        //int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;
        //W=700;
        //H=450;
        //wp.rcNormalPosition.right=wp.rcNormalPosition.left+W;
        //wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+H;

        //SetWindowPlacement(&wp);
        }
      }
    else
      {
      WINDOWPLACEMENT wp;
      wp.length = sizeof(wp);
      GetWindowPlacement(&wp);
      int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
      int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

      for (int i=0; i<WL.TrndWnds.GetCount(); i++)
        {
        if (WL.TrndWnds[i]!=this)
          {
          WINDOWPLACEMENT wpt;
          wpt.length = sizeof(wpt);
          WL.TrndWnds[i]->GetWindowPlacement(&wpt);
          if (gs_pPrj->m_TrndBehaviour & WB_Coincident)
            {
            wpt.rcNormalPosition=wp.rcNormalPosition;
            }
          else if (gs_pPrj->m_TrndBehaviour & WB_SizeTogether)
            {
            wpt.rcNormalPosition.right=wpt.rcNormalPosition.left+W;
            wpt.rcNormalPosition.bottom=wpt.rcNormalPosition.top+H;
            }
          wpt.length = sizeof(wpt);
          WL.TrndWnds[i]->SetWindowPlacement(&wpt);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwSplit::OnMove(int x, int y)
  {
  CMDIChildWnd::OnMove(x, y);

  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  GetWindowPlacement(&wp);
  int DX=wp.rcNormalPosition.left-m_PrevClientX;
  int DY=wp.rcNormalPosition.top-m_PrevClientY;

  if (m_CreateDone && (gs_pPrj->m_TrndBehaviour || gs_FileNewFlag) && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CAutoIncDec AID(gs_BlockSizeAdjustment);
    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    if (gs_FileNewFlag)
      {
      CWnd * pTopWnd=NULL;
      int Z=10000;
      WL.BuildZOrder(true);
      for (int i=0; i<WL.Wnds.GetCount(); i++)
        {
        CTopWindowInfo & WI=WL.Wnds[i];
        CWnd * pWnd=WI.pWnd;
        if (pWnd->IsKindOf(RUNTIME_CLASS(CTagVwSplit)) && pWnd!=this)
          {
          if (WI.iZOrd<Z)
            {
            Z=WI.iZOrd;
            pTopWnd=WI.pWnd;
            }
          }
        }
      if (pTopWnd)
        {
        wp.length = sizeof(wp);
        pTopWnd->GetWindowPlacement(&wp);
        wp.length = sizeof(wp);
        SetWindowPlacement(&wp);
        }
      }
    else
      {
      //WINDOWPLACEMENT wp;
      wp.length = sizeof(wp);
      GetWindowPlacement(&wp);
      int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
      int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

      for (int i=0; i<WL.TrndWnds.GetCount(); i++)
        {
        if (WL.TrndWnds[i]!=this)
          {
          WINDOWPLACEMENT wpt;
          wpt.length = sizeof(wpt);
          WL.TrndWnds[i]->GetWindowPlacement(&wpt);
          if (gs_pPrj->m_TrndBehaviour & WB_Coincident)
            {
            wpt.rcNormalPosition=wp.rcNormalPosition;
            }
          else if (gs_pPrj->m_TrndBehaviour & WB_MoveTogether)
            {
            wpt.rcNormalPosition.left+=DX;
            wpt.rcNormalPosition.right+=DX;
            wpt.rcNormalPosition.top+=DY;
            wpt.rcNormalPosition.bottom+=DY;
            }
          wpt.length = sizeof(wpt);
          WL.TrndWnds[i]->SetWindowPlacement(&wpt);
          }
        }
      }
    }

  wp.length = sizeof(wp);
  GetWindowPlacement(&wp);
  m_PrevClientX=wp.rcNormalPosition.left;
  m_PrevClientY=wp.rcNormalPosition.top;
  }

//---------------------------------------------------------------------------

void CTagVwSplit::DoBehaviourChange()
  {
  if (gs_pPrj->m_TrndBehaviour && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CAutoIncDec AID(gs_BlockSizeAdjustment);
    WINDOWPLACEMENT wp;
    wp.length = sizeof(wp);

    if (gs_FileNewFlag)
      {
      CWindowLists WL;
      CWnd * pTopWnd=NULL;
      CWnd * pTopWndBut1=NULL;
      int Z=10000;
      WL.BuildZOrder(true);
      for (int i=0; i<WL.Wnds.GetCount(); i++)
        {
        CTopWindowInfo & WI=WL.Wnds[i];
        CWnd * pWnd=WI.pWnd;
        if (pWnd->IsKindOf(RUNTIME_CLASS(CTagVwSplit)))// && pWnd!=this)
          {
          if (WI.iZOrd<Z)
            {
            Z=WI.iZOrd;
            pTopWndBut1=pTopWnd;
            pTopWnd=WI.pWnd;
            }
          }
        }
      if (pTopWndBut1)
        {
        wp.length = sizeof(wp);
        pTopWndBut1->GetWindowPlacement(&wp);
        wp.length = sizeof(wp);
        pTopWnd->SetWindowPlacement(&wp);
        }
      }
    else
      {

      CWindowLists WL;
      if (WL.BuildZOrder()<0)
        return;

      if (WL.TrndWnds.GetCount()<1)
        return;

      WINDOWPLACEMENT wp;
      wp.length = sizeof(wp);

      int iTop=-1;
      int iZ=10000;
      for (int i=0; i<WL.Wnds.GetCount(); i++)
        {
        if (WL.Wnds[i].pWnd->IsKindOf(RUNTIME_CLASS(CTagVwSplit)) && WL.Wnds[i].iZOrd<iZ)
          {
          iTop=i;
          iZ=WL.Wnds[i].iZOrd;
          }
        }

      if (iTop>=0)
        {
        WL.Wnds[iTop].pWnd->GetWindowPlacement(&wp);

        for (int i=0; i<WL.TrndWnds.GetCount(); i++)
          {

          WINDOWPLACEMENT wpt;
          wpt.length = sizeof(wpt);
          WL.TrndWnds[i]->GetWindowPlacement(&wpt);
          if (gs_pPrj->m_TrndBehaviour & WB_Coincident)
            {
            wpt.rcNormalPosition=wp.rcNormalPosition;
            }
          else if (gs_pPrj->m_TrndBehaviour & WB_MoveTogether)
            {
            _asm int 3;
            //wpt.rcNormalPosition.left+=DX;
            //wpt.rcNormalPosition.right+=DX;
            //wpt.rcNormalPosition.top+=DY;
            //wpt.rcNormalPosition.bottom+=DY;
            }
          wpt.length = sizeof(wpt);
          WL.TrndWnds[i]->SetWindowPlacement(&wpt);

          }
        }
      }
    }
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
  m_CreateDone=true;
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
