//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $                            
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "WndSlct.h"
#include "scd_wm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
// CWndArrangeDlg dialog

//===========================================================================

CWndArrangeDlg::CWndArrangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWndArrangeDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CWndArrangeDlg)
	m_MaximiseMain  = ScdPFUser.RdInt("ArrangeWindows", "MaximiseMain", 1)!=0;
	m_GrfMinimise   = ScdPFUser.RdInt("ArrangeWindows", "GrfMinimise", 1)!=0;
	m_OthMinimise   = ScdPFUser.RdInt("ArrangeWindows", "OthMinimise", 1)!=0;
	m_TrnMinimise   = ScdPFUser.RdInt("ArrangeWindows", "TrnMinimise", 1)!=0;
	m_GrfStyle      = ScdPFUser.RdInt("ArrangeWindows", "GrfStyle", 1);
	m_OthStyle      = ScdPFUser.RdInt("ArrangeWindows", "OthStyle", 1);
	m_TrnStyle      = ScdPFUser.RdInt("ArrangeWindows", "TrnStyle", 1);

  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CWndArrangeDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWndArrangeDlg)
	DDX_Check(pDX, IDC_AW_MAXMAIN, m_MaximiseMain);
	DDX_Check(pDX, IDC_AW_GRFMINIMISE, m_GrfMinimise);
	DDX_Check(pDX, IDC_AW_OTHMINIMISE, m_OthMinimise);
	DDX_Check(pDX, IDC_AW_TRNMINIMISE, m_TrnMinimise);
	DDX_Radio(pDX, IDC_AW_GRFPOSNONE, m_GrfStyle);
	DDX_Radio(pDX, IDC_AW_OTHPOSNONE, m_OthStyle);
	DDX_Radio(pDX, IDC_AW_TRNPOSNONE, m_TrnStyle);
	//}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWndArrangeDlg, CDialog)
  //{{AFX_MSG_MAP(CWndArrangeDlg)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CWndArrangeDlg::OnBnClickedOk()
  {
  OnOK();

  // TODO: Add your control notification handler code here
	ScdPFUser.WrInt("ArrangeWindows", "MaximiseMain",m_MaximiseMain?1:0);
	ScdPFUser.WrInt("ArrangeWindows", "GrfMinimise", m_GrfMinimise?1:0);
	ScdPFUser.WrInt("ArrangeWindows", "OthMinimise", m_OthMinimise?1:0);
  ScdPFUser.WrInt("ArrangeWindows", "TrnMinimise", m_TrnMinimise?1:0);
	ScdPFUser.WrInt("ArrangeWindows", "GrfStyle",    m_GrfStyle);
	ScdPFUser.WrInt("ArrangeWindows", "OthStyle",    m_OthStyle);
	ScdPFUser.WrInt("ArrangeWindows", "TrnStyle",    m_TrnStyle);
  }

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//===========================================================================
//===========================================================================
// CSelectWindowDlg dialog

CSelectWindowDlg::CSelectWindowDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CSelectWindowDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CSelectWindowDlg)
  //}}AFX_DATA_INIT
  iPrevList = -1;
  pToActivate=NULL;
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectWindowDlg)
  DDX_Control(pDX, IDC_TRENDSWNDLIST, m_Trends);
  DDX_Control(pDX, IDC_GRAPHICSWNDLIST, m_Graphics);
  DDX_Control(pDX, IDC_GENERALWNDLIST, m_General);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSelectWindowDlg, CDialog)
  //{{AFX_MSG_MAP(CSelectWindowDlg)
  ON_LBN_DBLCLK(IDC_GENERALWNDLIST, OnDblclkGeneralwndlist)
  ON_LBN_DBLCLK(IDC_GRAPHICSWNDLIST, OnDblclkGraphicswndlist)
  ON_LBN_DBLCLK(IDC_TRENDSWNDLIST, OnDblclkTrendswndlist)
  ON_BN_CLICKED(IDC_NEWWINDOW, OnNewwindow)
  ON_LBN_SELCHANGE(IDC_GENERALWNDLIST, OnSelchangeGeneralwndlist)
  ON_LBN_SELCHANGE(IDC_GRAPHICSWNDLIST, OnSelchangeGraphicswndlist)
  ON_LBN_SELCHANGE(IDC_TRENDSWNDLIST, OnSelchangeTrendswndlist)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CSelectWindowDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  const int Count = WL.BuildSingleList();
  for (int i=0; i<Count; i++)
    {
    switch (WL.Wnds[i].iType)
      {
      case 0: m_General.AddString((const char*)WL.Wnds[i].m_sName); break;
      case 1: m_Graphics.AddString((const char*)WL.Wnds[i].m_sName); break;
      case 2: m_Trends.AddString((const char*)WL.Wnds[i].m_sName); break;
      }
    }
  if (m_Trends.GetCount()==0)
    {//change window size to hide trend windows list box...
    CRect WinRect;
    GetWindowRect(&WinRect);
    //SetWindowPos(this, 0, 0, ((WinRect.right - WinRect.left) * 2 / 3) + 4, WinRect.bottom - WinRect.top, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
    SetWindowPos(this, 0, 0, 270, WinRect.bottom - WinRect.top, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
    GetDlgItem(IDC_TRENDSWNDLIST)->EnableWindow(FALSE);
    }

  CWnd* pWnd = CWindowLists::GetCurrentTopWindow();
  if (pWnd)
    {
    CString Txt;
    for (int a=0; a<NAccessWnds; a++)
      if (pWnd==WL.pAccessWnd[a])
        {
        Txt = CWindowLists::AccessWndTitle(a);
        goto Next;
        }
    pWnd->GetWindowText(Txt);
Next:
    int i = m_General.FindStringExact(-1, (const char*)Txt);
    if (i>=0)
      {
      m_General.SetCurSel(i);
      iPrevList = 0;
      }
    else
      {
      i = m_Graphics.FindStringExact(-1, (const char*)Txt);
      if (i>=0)
        {
        m_Graphics.SetCurSel(i);
        iPrevList = 1;
        }
      else
        {
        i = m_Trends.FindStringExact(-1, (const char*)Txt);
        if (i>=0)
          {
          m_Trends.SetCurSel(i);
          iPrevList = 2;
          }
        }
      }
    }
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::ActivateWnd(CWnd* pWnd)
  {
  if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
    {
    if (pWnd->GetFocus()!=pWnd)
      ((CMDIChildWnd*)pWnd)->MDIActivate(); //bring the window to the front
    if (pWnd->IsIconic())
      pWnd->ShowWindow(SW_RESTORE);
    }
  else
    ASSERT(FALSE); //What window is this ???
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnDblclkGeneralwndlist() 
  {
  UpdateData(TRUE);
  int i = m_General.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_General.GetText(i, Txt);
    const int Index = WL.Find(0, (char*)(const char*)Txt);
    ASSERT(Index>=0);
    //EndDialog(IDCANCEL);
    pToActivate=WL.Wnds[Index].pWnd;
    //ActivateWnd(WL.Wnds[Index].pWnd);
    CDialog::OnOK();
    }
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnDblclkGraphicswndlist()
  {
  UpdateData(TRUE);
  int i = m_Graphics.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_Graphics.GetText(i, Txt);
    const int Index = WL.Find(1, (char*)(const char*)Txt);
    ASSERT(Index>=0);
    //EndDialog(IDCANCEL);
    pToActivate=WL.Wnds[Index].pWnd;
    //ActivateWnd(WL.Wnds[Index].pWnd);
    CDialog::OnOK();
    }
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnDblclkTrendswndlist() 
  {
  UpdateData(TRUE);
  int i = m_Trends.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_Trends.GetText(i, Txt);
    const int Index = WL.Find(2, (char*)(const char*)Txt);
    ASSERT(Index>=0);
    //EndDialog(IDCANCEL);
    pToActivate=WL.Wnds[Index].pWnd;
    //ActivateWnd(WL.Wnds[Index].pWnd);
    CDialog::OnOK();
    }
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnSelchangeGeneralwndlist() 
  {
  iPrevList = 0;
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnSelchangeGraphicswndlist() 
  {
  iPrevList = 1;
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnSelchangeTrendswndlist() 
  {
  iPrevList = 2;
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnOK() 
  {
  switch (iPrevList)
    {
    case 0: OnDblclkGeneralwndlist(); return;
    case 1: OnDblclkGraphicswndlist(); return;
    case 2: OnDblclkTrendswndlist(); return;
    }
  CDialog::OnOK();
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::PostNcDestroy() 
  {
  // TODO: Add your specialized code here and/or call the base class
  if (pToActivate)
    ActivateWnd(pToActivate);
  CDialog::PostNcDestroy();
  }
//--------------------------------------------------------------------------

void CSelectWindowDlg::OnNewwindow() 
  {
  iPrevList = -1;
  ScdMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW);
  EndDialog(IDCANCEL);
  }

//--------------------------------------------------------------------------

void CSelectWindowDlg::OnUpdateOK(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(iPrevList!=-1);
  }

//--------------------------------------------------------------------------
//===========================================================================
// CWndSlctWnd dialog

CWndSlctWnd * CWndSlctWnd::pTheWnd=NULL;
BOOL CWndSlctWnd::bDoRefresh=false;
BOOL CWndSlctWnd::bUseSelectWndList=true;

CWndSlctWnd::CWndSlctWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CWndSlctWnd::IDD, pParent)
  {
  Inited=false;
	//{{AFX_DATA_INIT(CWndSlctWnd)
	//}}AFX_DATA_INIT
  Create(CWndSlctWnd::IDD, pParent);
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWndSlctWnd)
  DDX_Control(pDX, IDC_LISTWNDSLCT, m_List);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWndSlctWnd, CDialog)
  //{{AFX_MSG_MAP(CWndSlctWnd)
  ON_WM_SIZE()
  ON_WM_MOVE()
  ON_WM_ACTIVATE()
  ON_LBN_SELCHANGE(IDC_LISTWNDSLCT, OnSelchangeList2)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CWndSlctWnd::OnInitDialog() 
  {
	CDialog::OnInitDialog();
  Inited=true;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

//--------------------------------------------------------------------------

static LPCTSTR Section="WindowSelector";

void CWndSlctWnd::SavePos()
  {
  CProfINIFile PF(PrjFile());
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (pTheWnd->GetWindowPlacement(&wp))
    {
    PF.WrInt(Section, "Left", wp.rcNormalPosition.left);
    PF.WrInt(Section, "Top", wp.rcNormalPosition.top);
    PF.WrInt(Section, "Right", wp.rcNormalPosition.right);
    PF.WrInt(Section, "Bottom", wp.rcNormalPosition.bottom);
    }
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::RestorePos()
  {
  CProfINIFile PF(PrjFile());
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (pTheWnd->GetWindowPlacement(&wp))
    {
    wp.rcNormalPosition.left  =PF.RdInt(Section, "Left",   wp.rcNormalPosition.left  );
    wp.rcNormalPosition.top   =PF.RdInt(Section, "Top",    wp.rcNormalPosition.top   );
    wp.rcNormalPosition.right =PF.RdInt(Section, "Right",  wp.rcNormalPosition.right );
    wp.rcNormalPosition.bottom=PF.RdInt(Section, "Bottom", wp.rcNormalPosition.bottom);
    pTheWnd->SetWindowPlacement(&wp);
    //TODO!!! if window is not visible because it is "off" the screen & not on dual monitor, then reposition it!!!
    }
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::OpenIt()
  {
  bDoRefresh=false;
  if (pTheWnd==NULL)
    pTheWnd=new CWndSlctWnd(AfxGetMainWnd());
  RefreshIt();

  CProfINIFile PF(PrjFile());
  PF.WrInt(Section, "Open", 1);
  
  pTheWnd->RestorePos();  
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::CloseIt()
  {
  if (pTheWnd && PrjFile())
    {
    pTheWnd->SavePos();

    pTheWnd->DestroyWindow();
    }
  delete pTheWnd;
  pTheWnd=NULL;
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::RefreshIt()
  {
  bDoRefresh=true;
  }

void CWndSlctWnd::ChkRefreshIt()
  {
  if (pTheWnd && bDoRefresh && pTheWnd->m_hWnd)
    {
    bDoRefresh=false;
    pTheWnd->DoTheRefresh();
    }
  }

void CWndSlctWnd::DoTheRefresh()
  {
  LockWindowUpdate( );
  
  m_List.ResetContent();

  CWindowLists WL;

  const int Count = WL.BuildSingleList();
  for (int iPass=1; iPass<=3; iPass++)
    {
    char * Separs[] =
      {
      "----- Graphics",
      "----- Trends",
      "----- Other"
      };
    CWnd * pActWindow=CWnd::GetActiveWindow();
    int iAct=-1;
    int iStart=m_List.AddString(Separs[iPass-1]);
    for (int i=0; i<Count; i++)
      {
      LPCTSTR p=WL.Wnds[i].m_sName; 
      if (iPass==3)
        { // Filter out some windows
        if (_stricmp(p, "Command")==0 ||
            _stricmp(p, "Order of Evaluation")==0 ||
            _strnicmp(p, "SysCAD ", 7)==0)
          continue;
        }

      if (WL.Wnds[i].iType==iPass || 
          WL.Wnds[i].iType==0 && iPass==3)
        {
        for (int j=iStart+1; j<m_List.GetCount(); j++)
          {
          CString T;
          m_List.GetText(j, T);
          if (T.CompareNoCase(p)>0)
            break;
          }
  
        //WINDOWPLACEMENT WP;
        //WP.length=sizeof(WP);
        //WL.Wnds[i].pWnd->GetWindowPlacement(&WP);
        //if (WP.showCmd==-1)
        //  iAct=m_List.InsertString(j, p); 
        //else
        m_List.InsertString(j, p); 
        }
      }
    }
  
  UnlockWindowUpdate();
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::OnOK() 
  {
	CDialog::OnOK();
  CloseIt();
  }

void CWndSlctWnd::OnCancel() 
  {
  CloseIt();
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::OnSize(UINT nType, int cx, int cy) 
  {
	CDialog::OnSize(nType, cx, cy);
	if (Inited)
    {
    WINDOWPLACEMENT WP;
    WP.length=sizeof(WP);
    m_List.GetWindowPlacement(&WP);
    WP.rcNormalPosition.right=cx;
    WP.rcNormalPosition.bottom=cy;
    m_List.SetWindowPlacement(&WP);
    SavePos();
    }
  }

//--------------------------------------------------------------------------

void CWndSlctWnd::OnMove(int x, int y)
  {
	CDialog::OnMove(x, y);
	if (Inited)
    {
    //WINDOWPLACEMENT WP;
    //WP.length=sizeof(WP);
    //m_List.GetWindowPlacement(&WP);
    //WP.rcNormalPosition.right=cx;
    //WP.rcNormalPosition.bottom=cy;
    //m_List.SetWindowPlacement(&WP);
    SavePos();
    }
  };

//--------------------------------------------------------------------------

void CWndSlctWnd::OnSelchangeList2() 
  {
	// TODO: Add your control notification handler code here
  int iSel=m_List.GetCurSel();
  if (iSel>=0)
    {
    CString Txt;
    m_List.GetText(iSel, Txt);

    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    const int Index = WL.Find(255, (char*)(const char*)Txt);
    if (Index<0) 
      return;
  
    ASSERT(Index>=0);
    CWnd *pToActivate=WL.Wnds[Index].pWnd;
    if (pToActivate)
      {
      if (pToActivate->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
        {
        if (pToActivate->GetFocus()!=pToActivate)
          ((CMDIChildWnd*)pToActivate)->MDIActivate(); //bring the window to the front
        if (pToActivate->IsIconic())
          //pToActivate->ShowWindow(SW_RESTORE);
          pToActivate->ShowWindow(SW_SHOWNOACTIVATE);
        else
          {
          /*CWnd *w=pToActivate;
          while (w && !(w->IsKindOf(RUNTIME_CLASS(CFrameWnd))))
            w = w->GetParent();
          if (w && (w->IsKindOf(RUNTIME_CLASS(CFrameWnd))))
            {
            w->BringWindowToTop();
            }*/
          //pToActivate->BringWindowToTop();
          //pToActivate->SetActiveWindow();
          pToActivate->SetFocus();
          }
        }
      else
        ASSERT(FALSE); //What window is this ???
      }
    //m_List.SetCurSel(-1);
    }
  ShowWindow(SW_RESTORE);//SW_SHOWNORMAL);
  }

//--------------------------------------------------------------------------

static void CalcTileSizes(int Cnt, int WidthAvail, int HeightAvail, double WndRatio, int & Width, int & Height)
  {
  if (Cnt>0)
    {
    Height = (int)(WndRatio*WidthAvail);
    double Ratio = (double)HeightAvail/Height;
    double cols = sqrt((double)Cnt/Ratio);
    int Cols = Max(1, (int)(cols+0.5));
    int Rows = (int)((double)Cnt/Cols+0.5);
    Width = WidthAvail/Cols;
    Height = (int)(WndRatio*Width);
    if (Rows*Height>HeightAvail+1)
      {
      Cols++;
      Width = WidthAvail/Cols;
      Height = (int)(WndRatio*Width);
      }
    }
  }

void CWndSlctWnd::ArrangeWindows(int RqdLayoutStyle/*=-1*/) 
  {
  CWndArrangeDlg Dlg;
  if (RqdLayoutStyle<0)
    if (Dlg.DoModal()!=IDOK)
      return;

  CWaitCursor Wait;
  const flag LockUpdate = 0;
  CWindowLists WL;
  const int Count = WL.BuildSingleList(true);
  int MainWndIndex = -1;
  for (int i=0; MainWndIndex<0 && i<Count; i++)
    if (WL.Wnds[i].pWnd==WL.pMainWnd)
      MainWndIndex=i;
  CWnd* pCurTopWnd = CWindowLists::GetCurrentTopWindow();
  if (Dlg.m_MaximiseMain)
    WL.pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
  else
    WL.pMainWnd->ShowWindow(SW_SHOWNOACTIVATE);
  if (LockUpdate)
    WL.pMainWnd->LockWindowUpdate();
  CWnd* pMDIWnd = WL.GetMDIClientWnd();
  CRect R;
  pMDIWnd->GetWindowRect(R);
  const int MainWidth = R.Width();
  const int MainHeight = R.Height();
  const double WndRatio = (double)MainHeight/MainWidth;
  const int AccWidth = Max(460, (int)(MainWidth/3));
  const int cycaption = GetSystemMetrics(SM_CYCAPTION);

  const int TrnWndCnt = WL.GetTrendWndCount();
  const int TrnDelta = cycaption-1;
  int TrnTop = -1;
  int TrnLeft = -1;
  int TrnWidth;
  int TrnHeight;
  switch (Dlg.m_TrnStyle)
    {
    case 0: break;
    case 1:
      TrnTop = cycaption;
      TrnWidth = Max(380, (int)(MainWidth*0.4));
      TrnHeight = (int)(MainHeight*0.7);
      break;
    case 2:
      TrnLeft = MainWidth/3;
      TrnWidth = MainWidth/2;
      TrnHeight = (int)(MainHeight*0.7);
      break;
    case 3:
      if (TrnWndCnt>0)
        CalcTileSizes(TrnWndCnt, MainWidth, MainHeight, WndRatio, TrnWidth, TrnHeight);
      break;
    case 4:
      // Overlay
      break;
    }

  const int GrfWndCnt = WL.GetGrfWndCount();
  const int GrfDelta = cycaption-1;
  const int GrfWidthSpace = MainWidth-AccWidth+2;
  int GrfTop = -1;
  int GrfLeft = -1;
  int GrfWidth;
  int GrfHeight;
  switch (Dlg.m_GrfStyle)
    {
    case 0: break;
    case 1:
      GrfWidth = Range((int)(MainWidth*0.4), (int)((MainWidth-AccWidth)-((GrfWndCnt-1)*GrfDelta)), GrfWidthSpace);
      GrfHeight = (int)(MainHeight*GrfWidth/MainWidth);
      break;
    case 2:
      if (GrfWndCnt>0)
        CalcTileSizes(GrfWndCnt, GrfWidthSpace, MainHeight, WndRatio, GrfWidth, GrfHeight);
      break;
    case 3:
      // Overlay
      break;
    }

  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  for (i=0; i<Count; i++)
    if (i!=MainWndIndex)
      {
      if (WL.Wnds[i].iType==1)
        {
        if (Dlg.m_GrfStyle==0)
          {
          WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
          }
        else if (Dlg.m_GrfStyle==3 && GrfTop<0)
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          GrfLeft   = wp.rcNormalPosition.left;
          GrfWidth  = wp.rcNormalPosition.left - wp.rcNormalPosition.right;
          GrfTop    = wp.rcNormalPosition.top;
          GrfHeight = wp.rcNormalPosition.top - wp.rcNormalPosition.bottom;
          }
        else
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = GrfLeft;
          wp.rcNormalPosition.right = wp.rcNormalPosition.left+GrfWidth;
          wp.rcNormalPosition.top = GrfTop;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+GrfHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          if (Dlg.m_GrfStyle==3)
            {
            }
          else if (Dlg.m_GrfStyle==1)
            {
            GrfTop += GrfDelta;
            GrfLeft += GrfDelta;
            if (GrfTop>MainHeight-GrfHeight)
              {
              GrfTop = (int)(GrfDelta*2.5);
              GrfLeft = -1;
              }
            }
          else
            {
            GrfLeft += GrfWidth;
            if (GrfLeft>GrfWidthSpace-10)
              {
              GrfTop += GrfHeight;
              GrfLeft = -1;
              }
            }
          }
        }
      else if (WL.Wnds[i].iType==2)
        {
        if (Dlg.m_TrnStyle==0)
          {
          WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
          }
        else
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = TrnLeft;
          wp.rcNormalPosition.right = wp.rcNormalPosition.left+TrnWidth;
          wp.rcNormalPosition.top = TrnTop;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+TrnHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          if (Dlg.m_TrnStyle==1)
            {
            if (TrnWndCnt>1)
              TrnLeft += ((MainWidth-TrnWidth)/(TrnWndCnt-1));
            }
          else if (Dlg.m_TrnStyle==2)
            {
            TrnTop += TrnDelta;
            TrnLeft += TrnDelta;
            if (TrnLeft>MainWidth-TrnWidth || TrnTop>MainHeight-TrnHeight)
              {
              TrnTop = -1;
              TrnLeft = -1;
              }
            }
          else
            {
            TrnLeft += TrnWidth;
            if (TrnLeft>MainWidth-10)
              {
              TrnTop += TrnHeight;
              TrnLeft = -1;
              }
            }
          }
        }
      else
        {//other windows...
        ASSERT(NAccessWnds==2);
        if (Dlg.m_OthStyle==0)
          {
          WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
          }
        else if (WL.Wnds[i].pWnd==WL.pAccessWnd[0])
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = MainWidth-AccWidth;
          wp.rcNormalPosition.right = MainWidth;
          wp.rcNormalPosition.top = -2;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+MainHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          }
        else if (WL.Wnds[i].pWnd==WL.pAccessWnd[1])
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = MainWidth-AccWidth;
          wp.rcNormalPosition.right = MainWidth;
          wp.rcNormalPosition.top = -2;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top+MainHeight;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          }
        else if (WL.Wnds[i].pWnd==WL.pPrjWnd)
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          const int PrjWidth = Min(MainWidth-2,930);
          wp.rcNormalPosition.left = (MainWidth-PrjWidth-2)/2;
          wp.rcNormalPosition.right = wp.rcNormalPosition.left+PrjWidth;
          wp.rcNormalPosition.top = cycaption;
          wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (int)(MainHeight*0.8);
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          }
        else if (WL.Wnds[i].pWnd==WL.pMsgWnd)
          {
          WL.Wnds[i].pWnd->GetWindowPlacement(&wp);
          wp.length = sizeof(wp);
          wp.showCmd = SW_SHOWNOACTIVATE;
          wp.rcNormalPosition.left = 0;
          wp.rcNormalPosition.right = Max(400L, long(wp.rcNormalPosition.left+MainWidth-AccWidth));
          wp.rcNormalPosition.top = (int)(MainHeight/2)-cycaption;
          wp.rcNormalPosition.bottom = MainHeight-cycaption;
          WL.Wnds[i].pWnd->SetWindowPlacement(&wp);
          //todo: centre message window splitter
          }
        else
          WL.Wnds[i].pWnd->ShowWindow(SW_SHOWNOACTIVATE);
        }
      }

  //minimise windows in required sequence...
  int Pass[3] = {1,2,0};
  if (!Dlg.m_GrfMinimise)
    Pass[0]=-1;
  if (!Dlg.m_TrnMinimise)
    Pass[1]=-1;
  if (!Dlg.m_OthMinimise)
    Pass[2]=-1;
  for (int j=0; j<3; j++)
    if (Pass[j]!=-1)
      {
      for (i=0; i<Count; i++)
        {
        if (WL.Wnds[i].iType==Pass[j] && i!=MainWndIndex)
          WL.Wnds[i].pWnd->ShowWindow(WL.Wnds[i].pWnd==pCurTopWnd ? SW_SHOWMINIMIZED : SW_SHOWMINNOACTIVE);
        }
      }
    
  if (LockUpdate)
    WL.pMainWnd->UnlockWindowUpdate();
  if (pCurTopWnd)
    pCurTopWnd->ShowWindow(pCurTopWnd->IsIconic() ? SW_SHOWMINIMIZED : SW_SHOWNORMAL);
  }

//--------------------------------------------------------------------------
//int CntCnt = 0;
void CWndSlctWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
  {
  /*if (Inited && (nState==WA_CLICKACTIVE || nState==WA_ACTIVE))
    {
    LogNote("Wnd", 0, "Expand %d", CntCnt++);
    }
  else
    {
    LogNote("Wnd", 0, "Shrink %d", CntCnt++);
    }*/
  CDialog::OnActivate(nState, pWndOther, bMinimized);
  }


