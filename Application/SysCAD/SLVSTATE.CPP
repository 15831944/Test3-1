//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "resource.h"
#include "SlvState.h"
#include "scd_wm.h"
#include "project.h"
#include "accnode.h"
#include "mdlrunmngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline flag NotBusy() { return gs_pPrj && gs_pPrj->pPrjDoc && /*pExec &&*/ !gs_Exec.Busy(); };

const int MAXCOLUMNS=4;

/////////////////////////////////////////////////////////////////////////////
// CSolveState dialog

CSolveState::CSolveState(CSolveTool * SolveTool, CWnd* pParent /*=NULL*/)
  : CDialog(CSolveState::IDD, pParent)
  {
  pSolveTool=SolveTool;

  //{{AFX_DATA_INIT(CSolveState)
  //}}AFX_DATA_INIT
  if (!Create(CSolveState::IDD, pParent))//, WS_VISIBLE | WS_OVERLAPPEDWINDOW))
    {
    TRACE("Failed to create SolveState\n");
    }

  CRect WinRect, wr1, wr2, mwr;
  GetWindowRect(&WinRect);
  GetDlgItem(IDB_SLV_ADV)->GetWindowRect(&wr1);
  GetDlgItem(IDC_WORSTERRS)->GetWindowRect(&wr2);
  WndRightShift=wr2.right-wr1.right;

  AfxGetMainWnd()->GetWindowRect(&mwr);
  int dx=((mwr.right-mwr.left)-(WinRect.right-WinRect.left-WndRightShift))/2-WinRect.left;
  int dy=((mwr.bottom-mwr.top)-(WinRect.bottom-WinRect.top))/2-WinRect.top;
  //int dx=0, dy=0;
  int x=gs_pPrj->m_Solver.m_iStateX;
  int y=gs_pPrj->m_Solver.m_iStateY;
  if (x>-10000)
    {
    int xw=WinRect.right-WinRect.left;
    int yw=WinRect.bottom-WinRect.top;

    WinRect.right =mwr.left+x+xw;
    WinRect.left  =mwr.left+x;
    WinRect.bottom=mwr.top+y+yw;
    WinRect.top   =mwr.top+y;
    }
  else
    {
    WinRect.right+=dx;
    WinRect.left+=dx;
    WinRect.top+=dy;
    WinRect.bottom+=dy;
    }

  RECT CR;
  AfxGetMainWnd()->GetTopWindow()->GetClientRect(&CR);
  int MaxX=CR.right-(2*GetSystemMetrics(SM_CXSIZE)+GetSystemMetrics(SM_CXSIZEFRAME));
  int MaxY=CR.bottom-(GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYSIZEFRAME));
  
  if (WinRect.left>MaxX)
    {
    int dx=MaxX-WinRect.left;
    WinRect.right+=dx;
    WinRect.left+=dx;
    }
  if (WinRect.top>MaxY)
    {
    int dy=MaxY-WinRect.top;
    WinRect.top+=dy;
    WinRect.bottom+=dy;
    }

  MoveWindow(&WinRect, False);

  //WndRightShift*=-1;
  ToggleWindowWidth(False); // Always make it Narrow

  if (gs_pPrj->m_Solver.m_fStateErrors) // Expand if neccessary
    ToggleWindowWidth(False);

  //ShowWindow(SW_SHOW);  
  CenterWindow();
  }

// --------------------------------------------------------------------------

CSolveState::~CSolveState()
  {
  //ASSERT(pState==NULL || pState==this);
  //pState = NULL;
  }

// --------------------------------------------------------------------------

void CSolveState::ToggleWindowWidth(flag RePaint)
  {
  CRect WinRect;
  GetWindowRect(&WinRect);
  WndRightShift *= -1;
  WinRect.right += WndRightShift;
  MoveWindow(&WinRect, RePaint);

  SetDlgItemText(IDB_SLV_ADV, WndRightShift < 0 ? "Errors >>" : "<< Basic");
  gs_pPrj->m_Solver.m_fStateErrors=Sign(WndRightShift)>0;
  gs_pPrj->m_Solver.m_iStateX=WinRect.left;
  gs_pPrj->m_Solver.m_iStateY=WinRect.top;
  iExtraInfoStyle = (WndRightShift<0 ? 0 : 1);
  PBEqnCB().SetCollectWorst(iExtraInfoStyle);
  }

// --------------------------------------------------------------------------

void CSolveState::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CSolveState)
	DDX_Control(pDX, IDC_WORSTERRS, m_ErrList);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
    {
    int xxx=0;
    }
  else
    {
    ((CButton*)GetDlgItem(IDC_SLV_PINNED))->SetCheck(gs_pPrj->m_Solver.m_fStatePinned);
    ((CButton*)GetDlgItem(IDC_SLV_PINNED))->SetBitmap(gs_pPrj->m_Solver.m_fStatePinned ? (HBITMAP)PinnedDownBitmap : (HBITMAP)PinnedUpBitmap);
    
    char Buff[80];
    CString sBuff;
    CProgressCtrl * pProg1 = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
    CProgressCtrl * pProg2 = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS2);
    if (1)
      {
      const int nTears    = PBEqnCB().NUnknowns();
      const int NBadError = PBEqnCB().NBadError();
      const int NBadLimit = PBEqnCB().NBadLimit();
      const int NIters    = PBEqnCB().NIters();
      //const int Cnvrgd    = PBEqnCB().fConverged;
      const int Decades = (int)(0.99+100.0*fabs(log10(Range(1.0e-9, PBEqnCB().Cfg.dEps_Rel, 0.1))));
      const int ErrXfrm = (int)(0.99+100.0*fabs(log10(Range(1.0e-9, PBEqnCB().WorstErr(), 0.9))));
      const int rel=(int)(0.99+fabs(log10(Range(1.0e-9, PBEqnCB().Cfg.dEps_Rel, 0.1))));
      XSW().m_swRun.MinSecDesc(sBuff);

      if (NIters>0)
        {
        if (XStopping())
          sprintf(Buff, "%i Stopping", NIters);
        else
          sprintf(Buff, "%i", NIters);
        GetDlgItem(IDC_ITERATIONS)->SetWindowText(Buff);
        GetDlgItem(IDC_SOLVETIME)->SetWindowText(XSW().m_swRun.MinSecDesc(sBuff));
        }
      else if (XStarting() || nTears==0)
        {
        GetDlgItem(IDC_ITERATIONS)->SetWindowText("Starting");
        GetDlgItem(IDC_SOLVETIME)->SetWindowText("0:00");
        }
      else
        {
        GetDlgItem(IDC_ITERATIONS)->SetWindowText("0");
        GetDlgItem(IDC_SOLVETIME)->SetWindowText(sBuff);
        }

      pProg1->SetPos(ErrXfrm);
      pProg1->SetRange(0, Decades);
      if (NBadLimit>0)
        sprintf(Buff, NIters>0 ? "Limit" : "");
      else if (NBadError>0)
        {
        if (NIters>0)
          sprintf(Buff, "%*.*f %%", rel+2, rel-1, PBEqnCB().WorstErr()*100.0);
        else
          Buff[0] = 0;
        }
      else 
        sprintf(Buff, NIters>1 ? "Solved" : "");
      XStrTrim(Buff);
      GetDlgItem(IDC_MAXERROR)->SetWindowText(Buff);
      pStatusBar->UpdateIndicator(2, Buff, TRUE); 
      sprintf(Buff, "%i  %s", NIters, sBuff);
      pStatusBar->UpdateIndicator(1, Buff, TRUE); 
      
      if (nTears>0)
        {
        pProg2->SetRange(0, nTears);
        if (NIters>0)
          pProg2->SetPos(Max(0, (int)(nTears-(NBadError+NBadLimit))));
        else
          pProg2->SetPos(0);
        }
      else
        {
        pProg2->SetRange(0, 1);
        pProg2->SetPos(0);
        }
      if (NIters>0)
        sprintf(Buff, "%i of %i", Max(0, (int)(nTears-(NBadError+NBadLimit))), nTears);
      else if (XStarting() || nTears==0)
        Buff[0] = 0;
      else
        sprintf(Buff, "0 of %i", nTears);
      GetDlgItem(IDC_CONVERGED)->SetWindowText(Buff);
      pStatusBar->UpdateIndicator(3, Buff, TRUE); 
      }
    
    
    if (iExtraInfoStyle)
      {
      CCustomListCtrl * pErrLst=(CCustomListCtrl *)GetDlgItem(IDC_WORSTERRS);
      
      //COLORREF BKCOLOR=0x00c0c0c0;
      //pErrLst->SetBkColor(BKCOLOR);
      for (int i=0; i<Min(PBEqnCB().NBadError()+PBEqnCB().NBadLimit(), (long)MAX_EQNSLV_WORST); i++)
        {
        if (i>=pErrLst->GetItemCount())
          pErrLst->InsertItem(i, " ");
        char Buff[1024];
        strcpy(Buff, PBEqnCB().Worst[i].cStr);
        char *p=Buff;
        for (int j=0; j<MAXCOLUMNS; j++)
          {
          char *q=strchr(p, '\t');
          if (q)
            *q=0;
          pErrLst->SetItemText(i, j, p);
          if (!q)
            break;
          p=q+1;
          }
        }
      for (  ; i<pErrLst->GetItemCount(); i++)
        for (int j=0; j<MAXCOLUMNS; j++)
          pErrLst->SetItemText(i, j, " ");

      if (PBEqnCB().NBadError()+PBEqnCB().NBadLimit()==0)
        {
        //pErrLst->SetItemText(0, 0, ".....solved.....");
        //pErrLst->SetItemText(0, 3, "Solved");
        pErrLst->SetItemText(0, 0, ".....");
        //pErrLst->SetItemText(2, 0, ".....");
        i=1;
        char Buff[1024];
        strcpy(Buff, PBEqnCB().WorstOther.cStr);
        char *p=Buff;
        for (int j=0; j<MAXCOLUMNS; j++)
          {
          char *q=strchr(p, '\t');
          if (q)
            *q=0;
          pErrLst->SetItemText(i, j, p);
          if (!q)
            break;
          p=q+1;
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSolveState, CDialog)
  //{{AFX_MSG_MAP(CSolveState)
  ON_BN_CLICKED(IDB_SLV_ADV, OnSlvAdv)
  ON_BN_CLICKED(IDC_SLV_PINNED, OnSlvPinned)
	ON_NOTIFY(NM_DBLCLK, IDC_WORSTERRS, OnDblclkWorsterrs)
	ON_NOTIFY(NM_RCLICK, IDC_WORSTERRS, OnRclickWorsterrs)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_PB_UPDATEDATA, OnUpdateDataByMsg)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

LRESULT CSolveState::OnUpdateDataByMsg(WPARAM wParam, LPARAM lParam)
  {
  switch (wParam)
    {
    case 0:
      ShowWindow(SW_RESTORE);
      break;
    case 1:
    case 3:
      if (wParam==3 || !gs_pPrj->m_Solver.m_fStatePinned)
        {
        ShowWindow(SW_HIDE);
        ScdMainWnd()->PostMessage(WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0); //get main window thread to top
        }
      break;
    case 2:
      UpdateData(False);
      //RedrawWindow();
      break;
    }
  return 0;
  }

//---------------------------------------------------------------------------

CSolveState* CSolveState::pState=NULL;

void CSolveState::Open(CSolveTool * SolveTool)
  {
  if (pState==NULL)
    pState=new CSolveState(SolveTool, AfxGetMainWnd());
  //pState->SendMessage(WMU_PB_UPDATEDATA, 1, 0);
  }

void CSolveState::Close()
  {
  if (pState)
    {
    pState->DestroyWindow();
    delete pState;
    pState = NULL;
    }
  }

void CSolveState::Show()
  {
  if (pState)
    {
    if (gs_TheRunMngr.AutomationBusy())
      pState->PostMessage(WMU_PB_UPDATEDATA, 0, 0);
    else
      pState->SendMessage(WMU_PB_UPDATEDATA, 0, 0);
    }
  };

void CSolveState::Hide(int ForceHide)
  {
  if (pState)
    {
    if (gs_TheRunMngr.AutomationBusy())
      pState->PostMessage(WMU_PB_UPDATEDATA, ForceHide ? 3 : 1, 0);
    else
      pState->SendMessage(WMU_PB_UPDATEDATA, ForceHide ? 3 : 1, 0);
    }
  };

void CSolveState::Update()
  {
  if (pState)
    {
    if (gs_TheRunMngr.AutomationBusy())
      pState->PostMessage(WMU_PB_UPDATEDATA, 2, 0);
    else
      pState->SendMessage(WMU_PB_UPDATEDATA, 2, 0);
    }
  };

//---------------------------------------------------------------------------

BOOL CSolveState::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  PinnedUpBitmap.LoadMappedBitmap(IDB_PINNEDUP);
  PinnedDownBitmap.LoadMappedBitmap(IDB_PINNEDDOWN);
    
  CCustomListCtrl* pErrLst=(CCustomListCtrl *)GetDlgItem(IDC_WORSTERRS);
  //COLORREF BKCOLOR=0x00c0c0c0;
  //pErrLst->SetBkColor(BKCOLOR);
  RECT Rect;
  pErrLst->GetWindowRect(&Rect);
  const int Width = Rect.right - Rect.left - 1;
  pErrLst->InsertColumn(0, "Tolerance", LVCFMT_RIGHT, (int)(Width*0.13));
  pErrLst->InsertColumn(1, "Value",     LVCFMT_RIGHT, (int)(Width*0.14));
  pErrLst->InsertColumn(2, "Damping",   LVCFMT_RIGHT, (int)(Width*0.13));
  pErrLst->InsertColumn(3, "Tag",       LVCFMT_LEFT, (int)(Width*0.60));
  ASSERT(MAXCOLUMNS==4);
  //CImageList ImgList;
  //CBitmap BM;
  //BM.LoadBitmap(IDB_ACTPAGEIMGS);
  //BOOL b=ImgList.Create(10, 10, FALSE, 0, 10);
  //ImgList.Add(&BM, (CBitmap*)NULL);
  
  //m_ActPageList.SetImageList(&ImgList , LVSIL_NORMAL);
  //m_ActPageList.SetImageList(&ImgList , LVSIL_SMALL);
  //m_ActPageList.SetImageList(&ImgList , LVSIL_STATE);

  return TRUE;
  }

//---------------------------------------------------------------------------

void CSolveState::OnSlvAdv() 
  {
  ToggleWindowWidth();
  }

//---------------------------------------------------------------------------

void CSolveState::OnSlvPinned() 
  {
  gs_pPrj->m_Solver.m_fStatePinned=!gs_pPrj->m_Solver.m_fStatePinned;
  ((CButton*)GetDlgItem(IDC_SLV_PINNED))->SetBitmap(gs_pPrj->m_Solver.m_fStatePinned ? (HBITMAP)PinnedDownBitmap : (HBITMAP)PinnedUpBitmap);
  }

//---------------------------------------------------------------------------

void CSolveState::OnCancel()
  {
  if (NotBusy())
    CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CSolveState::OnDblclkWorsterrs(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CCustomListCtrl* pErrLst=(CCustomListCtrl *)GetDlgItem(IDC_WORSTERRS);
  CString sLastTag;
  if (GetHitTag(*pErrLst, sLastTag)>=0)
    {
    if (AccessableTag(sLastTag)>=0)
      gs_AccessWnds.AccessNode(-1, (char*)(const char*)sLastTag);
    }
	*pResult = 0;
  }

//---------------------------------------------------------------------------

void CSolveState::OnRclickWorsterrs(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  CCustomListCtrl* pErrLst=(CCustomListCtrl *)GetDlgItem(IDC_WORSTERRS);
  CString sLastTag;
  if (GetHitTag(*pErrLst, sLastTag)>=0)
    {
    CRect Rect;
    pErrLst->GetWindowRect(&Rect);
    PopupMenuForTag(sLastTag, Rect.left + pErrLst->PrevDownPoint.x, Rect.top + pErrLst->PrevDownPoint.y);
    }
	*pResult = 0;
  }

//---------------------------------------------------------------------------

int CSolveState::GetHitTag(CCustomListCtrl& Ctrl, CString& sTag)
  {
  sTag = "";
  LV_HITTESTINFO HTI;
  HTI.pt.y = Ctrl.PrevDownPoint.y;
  HTI.pt.x = 10;
  int iLastIndex = Ctrl.HitTest(&HTI);
  if (gs_pPrj && gs_pPrj->pPrjDoc && iLastIndex>=0 && (HTI.flags & LVHT_ONITEM))
    {
    sTag = Ctrl.GetItemText(iLastIndex, 3);
    if (sTag.GetLength()>0)
      return iLastIndex;
    }
  return -1;
  }

//---------------------------------------------------------------------------

BOOL CSolveState::AccessableTag(CString& sTag)
  {
  if (TaggedObject::TestValidTag((char*)(const char*)sTag)==0)
    {
    const int i = sTag.Find('.');
    if (i>=0)
      sTag = sTag.Left(i);
    CXM_ObjectTag ObjTag((char*)(const char*)sTag, TABOpt_Exists);
    CXM_ObjectData ObjData;
    CXM_Route Route;
    if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
      return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CSolveState::PopupMenuForTag(CString& sTag, int x, int y)
  {
  CString sObjTag;
  sObjTag = sTag;
  BOOL TagOK = AccessableTag(sObjTag);
  if (TagOK)
    {
    const int len = sObjTag.GetLength();
    CString LeftTag,RightTag;
    int index = sObjTag.Find("//", 1);
    BOOL IsTearTag = (index>=0 && sObjTag[len-1]=='#');
    if (IsTearTag)
      {
      LeftTag = sObjTag.Left(index);
      IsTearTag = AccessableTag(LeftTag);
      if (IsTearTag)
        {
        RightTag = sObjTag.Mid(index+2, len-index-3);
        }
      }
    BOOL IsFlashTrnTag = (!IsTearTag && len>4 && sObjTag.Find("_FT#", 0)==len-4);
    if (IsFlashTrnTag)
      {
      LeftTag = sObjTag.Left(len-4);
      }

    CMenu Menu;
    Menu.CreatePopupMenu();
    Strng s;
    s.Set("&Access %s...", (const char*)sObjTag);
    Menu.AppendMenu(MF_STRING, IDM_SLV_ACCESS, s());
    Menu.AppendMenu(MF_STRING|(IsTearTag||IsFlashTrnTag?MF_GRAYED:0), IDM_SLV_FIND, "&Find...");
    Menu.AppendMenu(MF_STRING, IDM_SLV_COPY, "&Copy tag");
    Menu.AppendMenu(MF_STRING|(IsTearTag||IsFlashTrnTag?0:MF_GRAYED), IDM_SLV_COPYEXTRA, "Copy tag &Extra");
    //Menu.AppendMenu(MF_STRING|MF_GRAYED, IDM_SLV_COPYALL, "Copy all &tags");
    if (LeftTag.GetLength() || RightTag.GetLength())
      {
      Menu.AppendMenu(MF_SEPARATOR);
      if (LeftTag.GetLength())
        {
        s.Set("Access %s...", (const char*)LeftTag);
        Menu.AppendMenu(MF_STRING, IDM_SLV_ACCESSLEFT, s());
        s.Set("Find %s...", (const char*)LeftTag);
        Menu.AppendMenu(MF_STRING, IDM_SLV_FINDLEFT, s());
        }
      if (RightTag.GetLength())
        {
        s.Set("Access %s...", (const char*)RightTag);
        Menu.AppendMenu(MF_STRING, IDM_SLV_ACCESSRIGHT, s());
        s.Set("Find %s...", (const char*)RightTag);
        Menu.AppendMenu(MF_STRING, IDM_SLV_FINDRIGHT, s());
        }
      }

    int RetCd = Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, x, y, this);//Rect.left+Ctrl.PrevDownPoint.x, Rect.top+Ctrl.PrevDownPoint.y, pWnd);
    Menu.DestroyMenu();
    switch (RetCd)
      {
      case IDM_SLV_ACCESS:
        gs_AccessWnds.AccessNode(-1, (char*)(const char*)sObjTag);
        break;
      case IDM_SLV_FIND:
        gs_pPrj->FindTag((char*)(const char*)sObjTag);
        break;
      case IDM_SLV_ACCESSLEFT:
        gs_AccessWnds.AccessNode(-1, (char*)(const char*)LeftTag);
        break;
      case IDM_SLV_FINDLEFT:
        gs_pPrj->FindTag((char*)(const char*)LeftTag);
        break;
      case IDM_SLV_ACCESSRIGHT:
        gs_AccessWnds.AccessNode(-1, (char*)(const char*)RightTag);
        break;
      case IDM_SLV_FINDRIGHT:
        gs_pPrj->FindTag((char*)(const char*)RightTag);
        break;
      case IDM_SLV_COPY:
      case IDM_SLV_COPYEXTRA:
        {
        Strng s;
        if (IsTearTag || IsFlashTrnTag)
          {
          int index = sTag.Find(".", 1);
          if (index>=0)
            {
            CString RTag = sTag.Mid(index+1);
            if (RetCd==IDM_SLV_COPYEXTRA)
              s.Set("%s.V.[%s].Meas\r\n%s.V.[%s].Error", (const char*)sObjTag, (const char*)RTag, (const char*)sObjTag, (const char*)RTag);//todo:add the cnvs....???
            else
              s.Set("%s.V.[%s].Meas", (const char*)sObjTag, (const char*)RTag);//todo:add the cnvs....???
            }
          else
            s = (const char*)sTag;
          }
        else// if (IsControlTag)
          {
          s.Set("%s.Cfg.[???].Meas", (const char*)sObjTag);
          //todo: for a PID control tag, we need to get to the required actual tag......
          }
        CopyTextToClipboard(this, s());
        break;
        }
      case IDM_SLV_COPYALL:
        {
        Strng s;
        for (int i=0; i<MAX_EQNSLV_WORST; i++)
          {
          //todo
          }
        CopyTextToClipboard(this, s());
        break;
        }
      }
    }
  }

//---------------------------------------------------------------------------
