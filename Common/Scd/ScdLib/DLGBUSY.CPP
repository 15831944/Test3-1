//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define __DLGBUSY_CPP
#include "scd_wm.h"
#include "resource.h"
#include "errorlog.h"
#include "dlgbusy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern AFX_EXTENSION_MODULE SCDLibDLL;

//===========================================================================

const WPARAM MSG_OPEN_IT      = 1;
const WPARAM MSG_OPEN_IT_CAN  = 2;
const WPARAM MSG_CLOSE_IT     = 3;

const int MaxMsgDepth = 20;

class CDlgBusyWin : public CDialog
  {
  friend class CDlgBusy;
  protected:
    CDlgBusyWin(pchar Msg, CWnd* pParent = NULL); // standard constructor
    virtual ~CDlgBusyWin();

    //{{AFX_DATA(CDlgBusyWin)
    enum { IDD = IDD_BUSYDLG };
    CBitmapButton m_Logo;
    CBitmapButton m_Icon;
    //}}AFX_DATA
    int Cnt;
    CString m_Msg[MaxMsgDepth];
    flag m_Canned[MaxMsgDepth];
    flag m_DoCan[MaxMsgDepth];

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
    virtual void PostNcDestroy();

    //{{AFX_MSG(CDlgBusyWin)
    virtual BOOL OnInitDialog();
    afx_msg void OnActivateApp( BOOL bActive, HTASK hTask);
    afx_msg void OnCancelBusy();
    //}}AFX_MSG
    LRESULT OnDlgBusyOpenClose(WPARAM wParam, LPARAM lParam);
    LRESULT OnDlgBusySet(WPARAM wParam, LPARAM lParam);
    LRESULT OnDlgBusyTestCan(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

static CDlgBusyWin* BusyWnd = NULL;

//---------------------------------------------------------------------------

CDlgBusyWin::CDlgBusyWin(pchar Msg, CWnd* pParent /*=NULL*/)
  : CDialog() // CDlgBusy::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CDlgBusyWin)
  //}}AFX_DATA_INIT
  Cnt=-1;
  //m_Msg[0] = Msg;
  m_Icon.LoadBitmaps(IDB_BIGICON);
  m_Logo.LoadBitmaps(IDB_LOGO);

  Create(CDlgBusyWin::IDD, pParent);
  
  for (int i=0; i<MaxMsgDepth; i++)
    {
    m_Canned[i]=false;
    m_DoCan[i]=false;
    }

//  m_Canned=false;
  }

//---------------------------------------------------------------------------

CDlgBusyWin::~CDlgBusyWin()
  {
  BusyWnd = NULL;
  }

//---------------------------------------------------------------------------

void CDlgBusyWin::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDlgBusyWin)
	//}}AFX_DATA_MAP

  CStatic* p=(CStatic*)GetDlgItem(IDC_TEXT);
  if (!pDX->m_bSaveAndValidate && (Cnt>=0))
    p->SetWindowText(m_Msg[Cnt]);
  else
    p->SetWindowText("");
  }

//---------------------------------------------------------------------------

void CDlgBusyWin::PostNcDestroy()
  {
  delete this;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgBusyWin, CDialog)
  //{{AFX_MSG_MAP(CDlgBusyWin)
    ON_WM_SIZE()
//	ON_BN_CLICKED(IDC_CANCELBUSY, OnCancelBusy)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DLGBUSYOPENCLOSE,  OnDlgBusyOpenClose)
  ON_MESSAGE(WMU_DLGBUSYSET,        OnDlgBusySet)
  ON_MESSAGE(WMU_DLGBUSYTESTCAN,    OnDlgBusyTestCan)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDlgBusyWin::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  m_Icon.SubclassDlgItem(IDC_APPICON, this);
  m_Icon.SizeToContent();
  m_Logo.SubclassDlgItem(IDC_LOGO, this);
  m_Logo.SizeToContent();
  SetDlgItemText(IDC_FULLVERSION, FullVersion());
  SetDlgItemText(IDC_UPDATE, UpdateString());
  SetDlgItemText(IDC_COPYRIGHT, CopyrightNotice());
  SetDlgItemText(IDC_COMPANY, FullCompany());
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//===========================================================================

LRESULT CDlgBusyWin::OnDlgBusyOpenClose(WPARAM wParam, LPARAM lParam)
  {
  if (wParam==MSG_OPEN_IT || wParam==MSG_OPEN_IT_CAN)
    {
    gs_MsgLog.SetDefer(true);
    char* pMsg = (char*)lParam;
    char buff[1024];
    buff[0] = 0;
    ASSERT(pMsg && strlen(pMsg)<sizeof(buff)-16);
    char* p = buff;
    char* q;
    while ((q=strchr(pMsg, '\n'))!=NULL)
      {
      strncpy(p, pMsg, q-pMsg);
      p += q-pMsg;
      *p++ = '\r';
      *p++ = '\n';
      *p = 0;
      pMsg = q+1;
      }
    strcat(p, pMsg);
   
    if (Cnt<0)
      {
      Cnt++;
      m_Msg[Cnt] = buff;
      UpdateData(FALSE);
    
      BringWindowToTop();
      CenterWindow(AfxGetMainWnd());
      ShowWindow(SW_NORMAL);
      //SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
      }
    else if (Cnt<MaxMsgDepth-1)
      {
      Cnt++;
      m_Msg[Cnt] = buff;
      BringWindowToTop();
      CenterWindow(AfxGetMainWnd()); // In Case MainWnd Resized / Moved
      UpdateData(FALSE);
      }
    else
      {
      ASSERT(FALSE);
      }
    m_DoCan[Cnt]=(wParam==MSG_OPEN_IT_CAN);
    m_Canned[Cnt]=false;
    if (m_DoCan[Cnt])
      {
      }
    else
      {
      }
    AfxGetMainWnd()->UpdateWindow();
    }
  else if (wParam==MSG_CLOSE_IT)
    {
    Cnt--;
    if (m_DoCan[Cnt])
      {
      }
    else
      {
      }
    if (Cnt<0)
      {
      //SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
      ShowWindow(SW_HIDE);
      //MyPostMessage(AfxGetMainWnd(), WMU_UPDATEMAINWND, SUB_UPDMAIN_UPDATE, 0); //get main window thread to top
      AfxGetMainWnd()->UpdateWindow();
      }
    else
      {
      UpdateData(FALSE);

      CRect Rect(-10000,-10000,10000,10000);
      RedrawWindow(&Rect, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_FRAME | RDW_ALLCHILDREN);
      SendMessage(WM_NCACTIVATE, 1, 0);
      AfxGetMainWnd()->UpdateWindow();
      }
    gs_MsgLog.SetDefer(false); //must be balanced
    gs_MsgLog.FlushWndQ();
    }
  return 1;//0;
  }

//---------------------------------------------------------------------------

LRESULT CDlgBusyWin::OnDlgBusySet(WPARAM wParam, LPARAM lParam)
  {
  if (Cnt<0)
    return 1;

  const int MaxLines = 10;
  const int BuffSize = 512;
  int InsertLn = wParam;
  ASSERT(InsertLn<MaxLines);
  char* Txt = (char*)lParam;
  char L[MaxLines][BuffSize];
  for (int i=0; i<MaxLines; i++)
    L[i][0] = 0;
  char* p = m_Msg[Cnt].GetBuffer(0);  
  int Ln = 0;
  do 
    {
    char* q = strstr(p, "\r\n");
    if (q)
      {
      ASSERT((q-p)<BuffSize-1);
      strncpy(L[Ln], p, (q-p));
      L[Ln][q-p] = 0;
      p = q+2;
      }
    else
      {
      strncpy(L[Ln], p, BuffSize);
      p = NULL;
      }
    Ln++;
    }
  while (p && Ln<MaxLines);
  for (i=0; i<MaxLines; i++)
    L[i][BuffSize-1] = 0;
  
  p = Txt;
  while (p && InsertLn<MaxLines)
    {
    char* pn = strchr(p, '\n');
    if (pn)
      *pn = 0;
    strcpy(L[InsertLn++], p);
    p = pn ? pn+1 : NULL;
    }
  Ln = Max(InsertLn, Ln);

  char m[4096];
  int j = 0;
  for (i=0; i<Ln; i++)
    j += sprintf(&m[j], "%s\r\n", L[i]);
  ASSERT(j<sizeof(m));
  
  m_Msg[Cnt] = m;
  UpdateData(FALSE);

  //BringWindowToTop();
  ShowWindow(SW_SHOW);

  //todo, probably better to only update text instead of whole window ???
  CRect Rect(-10000, -10000, 10000, 10000);
  RedrawWindow(&Rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ALLCHILDREN);
  SendMessage(WM_NCACTIVATE, 1, 0);

  AfxGetMainWnd()->UpdateWindow();
  //return 0;
  return 1;
  }

//---------------------------------------------------------------------------

LRESULT CDlgBusyWin::OnDlgBusyTestCan(WPARAM wParam, LPARAM lParam)
  {
  if (Cnt<0)
    return 0;

  return m_Canned[Cnt]!=0;
  }

//---------------------------------------------------------------------------

void CDlgBusyWin::OnActivateApp( BOOL bActive, HTASK hTask)
  {
  CDialog::OnActivateApp( bActive, (DWORD)hTask);
  UpdateWindow();
  }

//---------------------------------------------------------------------------

void CDlgBusyWin::OnCancelBusy() 
  {
  if (Cnt<0)
    return;

	// TODO: Add your control notification handler code here
  m_Canned[Cnt]=true;
  }

//===========================================================================
//
//
//
//===========================================================================

void CDlgBusy::Startup()
  {
//  CSaveRestResHandle SRH(SCDLibDLL.hModule);
  
  BusyWnd = new CDlgBusyWin("", AfxGetMainWnd());
  //BusyWnd->CenterWindow(AfxGetMainWnd());
  BusyWnd->Cnt = -1;
  BusyWnd->ShowWindow(SW_NORMAL);
  BusyWnd->ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

void CDlgBusy::Shutdown()
  {
//  CSaveRestResHandle SRH(SCDLibDLL.hModule);
  if (BusyWnd)
    {
    BusyWnd->ShowWindow(SW_HIDE);
    delete BusyWnd;//->DestroyWindow();
    }
  }

//---------------------------------------------------------------------------

void CDlgBusy::Open(pchar pMsg, flag CanIt)
  {
//  CSaveRestResHandle SRH(SCDLibDLL.hModule);
  if (!BusyWnd)
    return;
  BusyWnd->SendMessage(WMU_DLGBUSYOPENCLOSE, MSG_OPEN_IT, (long)pMsg);
  }

//---------------------------------------------------------------------------

void CDlgBusy::Close()
  {
  if (!BusyWnd)
    return;
//  CSaveRestResHandle SRH(SCDLibDLL.hModule);
  BusyWnd->SendMessage(WMU_DLGBUSYOPENCLOSE, MSG_CLOSE_IT, 0);
  }

//---------------------------------------------------------------------------

BOOL CDlgBusy::IsOpen()
  {
  return (BusyWnd!=NULL && BusyWnd->Cnt>=0);
  }
                                
//---------------------------------------------------------------------------

void CDlgBusy::SetLine(int Ln, pchar fmt, ...)
  {
//  CSaveRestResHandle SRH(SCDLibDLL.hModule);
  if (!BusyWnd)
    return;
	
  char Txt[256];
  Ln = Min(Ln, 9);
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(Txt, fmt, argptr);
  va_end(argptr);

  BusyWnd->SendMessage(WMU_DLGBUSYSET, Ln, (long)Txt);
  }
    
//---------------------------------------------------------------------------

BOOL CDlgBusy::IsCancelled()
  {
  if (!BusyWnd)
    return true;
  return BusyWnd->SendMessage(WMU_DLGBUSYTESTCAN, 0, 0);
  }
    
//---------------------------------------------------------------------------

BOOL CDlgBusy::ShowWindow(int CmdShow)
  {
  if (BusyWnd)
    return BusyWnd->ShowWindow(CmdShow);
  return FALSE;
  }

//===========================================================================
//
//
//
//===========================================================================
