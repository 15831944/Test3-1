// PLC5Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PLC5.h"
#include "PLC5Dlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

inline double SafeAtoF(const char * Str, double Default=0.0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    //if (*Str=='*')
    //  return dNAN();
    if (isdigit(*Str) || *Str=='-' || *Str=='+' || *Str=='.')
      return atof(Str);
    }
  return Default;
  }

// -----------------------------------------------------------

inline long SafeAtoL(const char * Str, long Default=0)
  {
  if (Str)
    {
    while (*Str!=0 && isspace(*Str))
      Str++;
    if (isdigit(*Str) || *Str=='-' || *Str=='+')
      return atol(Str);
    }
  return Default;
  }

//===========================================================================
//
//
//
//===========================================================================
// Also put this GUID into the OPCServer.rgs file
// {15A44DE7-91FA-48d2-8D96-0DDA57670EE5}
CLSID CLSID_OPCServer =
{ 0x15a44de7, 0x91fa, 0x48d2, { 0x8d, 0x96, 0xd, 0xda, 0x57, 0x67, 0xe, 0xe5 } };

//===========================================================================
//
//
//
//===========================================================================

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//===========================================================================
//
//
//
//===========================================================================
/////////////////////////////////////////////////////////////////////////////
// CPLC5Dlg dialog

CPLC5Dlg::CPLC5Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPLC5Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPLC5Dlg)
	m_State = _T("");
	m_DisplayFmt = 0;
	m_StatusMsg = _T("");
	m_sGoto = _T("");
	m_Remote = FALSE;
	m_Reset = FALSE;
	m_RunMult = 1.0f;
	m_StepMS = 1000;
	m_IterMS = 100;
	m_UpdateMS = 50;
	m_ExecTime = _T("");
	m_UpdTime = _T("");
	m_ChgMonitorOn = FALSE;
	//}}AFX_DATA_INIT
//	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  //pOPCCallBack = OPCCallBack;
  //m_pWkRung = NULL;
  //m_pWkLadder = NULL;
  m_iPcIndex=-1;
  m_iTbIndex=-1;
  m_iLdIndex=-1;
  m_iWdIndex=-1;
  m_iBtIndex=-1;

  m_TimeTillUpdate=0;
  m_DispLst.m_ActLen=0;

  m_EvalTime=0;
  m_DispTime=0;
  m_nStepsReqd=0;
  m_nItsPerStep=1;
  m_nActualIterMS=1;
  m_nStepNo=0;
  m_nIterNo=0;
  m_RunningSlow=false;

  m_lSequenceNo=0;

  m_HistLine1=0;

  m_MyOPCCallBack=NULL;
  m_OPCServerStarted=false;
//  m_iIteration=0;
//  m_UpNAbout=FALSE;


}

CPLC5Dlg::~CPLC5Dlg()
  {
  OPCStop();//Close();
  }

void CPLC5Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPLC5Dlg)
	DDX_Control(pDX, IDC_TBHEADER, m_TbHeader);
	DDX_Control(pDX, IDC_LDHEADER, m_LdHeader);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Control(pDX, IDC_LDRSTATIC, m_LdStatic);
	DDX_Control(pDX, IDC_DATASTATIC, m_TbStatic);
	DDX_Control(pDX, IDC_DATASCROLLBAR, m_TbScroll);
	DDX_Control(pDX, IDC_LDRSCROLLBAR, m_LdScroll);
	DDX_Text(pDX, IDC_STATE, m_State);
	DDX_Radio(pDX, IDC_RADIODEC, m_DisplayFmt);
	DDX_Text(pDX, IDC_STATUSMSG, m_StatusMsg);
	DDX_Text(pDX, IDC_EDITGOTO, m_sGoto);
	DDX_Check(pDX, IDC_REMOTE, m_Remote);
	DDX_Check(pDX, IDC_RESET, m_Reset);
	DDX_Text(pDX, IDC_EDITRUNMULT, m_RunMult);
	DDV_MinMaxFloat(pDX, m_RunMult, 0.1f, 1000.f);
	DDX_Text(pDX, IDC_EDITSTEP, m_StepMS);
	DDV_MinMaxUInt(pDX, m_StepMS, 10, 10000);
	DDX_Text(pDX, IDC_EDITITER, m_IterMS);
	DDV_MinMaxUInt(pDX, m_IterMS, 10, 1000);
	DDX_Text(pDX, IDC_EDITUPDATE, m_UpdateMS);
	DDV_MinMaxUInt(pDX, m_UpdateMS, 10, 1000);
	DDX_Text(pDX, IDC_EXECTIME, m_ExecTime);
	DDX_Text(pDX, IDC_UPDTIME, m_UpdTime);
	DDX_Check(pDX, IDC_CHANGE_MONITOR, m_ChgMonitorOn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPLC5Dlg, CDialog)
	//{{AFX_MSG_MAP(CPLC5Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_VSCROLL()
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_BN_CLICKED(IDSTEP, OnStep)
	ON_BN_CLICKED(IDITER, OnIter)
	ON_BN_CLICKED(IDRUN, OnRun)
	ON_BN_CLICKED(IDSTOP, OnStop)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RADIODEC, OnRadiodec)
	ON_BN_CLICKED(IDC_RADIOHEX, OnRadiohex)
	ON_WM_RBUTTONDOWN()
	ON_EN_CHANGE(IDC_EDITGOTO, OnChangeEditgoto)
	ON_EN_SETFOCUS(IDC_EDITGOTO, OnSetfocusEditgoto)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_FINDINLADDER, OnFindinladder)
	ON_WM_RBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_GOTORUN, OnGotorun)
	ON_BN_CLICKED(IDC_GOTOSTOP, OnGotostop)
	ON_BN_CLICKED(IDC_GOTOIDLE, OnGotoidle)
	ON_COMMAND(ID_LOADSNAP, OnLoadsnap)
	ON_COMMAND(ID_SAVESNAP, OnSavesnap)
	ON_COMMAND(ID_LOADBACK, OnLoadback)
	ON_COMMAND(ID_SAVEBACK, OnSaveback)
	ON_BN_CLICKED(IDC_CHANGE_MONITOR, OnChangeMonitor)
	//}}AFX_MSG_MAP
  ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnGetToolTipNotify)
//  ON_MESSAGE( WM_OPCCMD, OnOpcCmd)
  ON_MESSAGE( WM_COMCMD, OnCOMCmd)
  ON_MESSAGE( WM_INITLOAD, OnInitLoad)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPLC5Dlg message handlers

BOOL CPLC5Dlg::OnInitDialog()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

  LOGFONT LogFont;
  LogFont.lfHeight         = 9;
  LogFont.lfWidth          = 0;
  LogFont.lfEscapement     = 0;
  LogFont.lfOrientation    = 0;
  LogFont.lfWeight         = FW_REGULAR;
  LogFont.lfItalic         = 0;// (i>=MaxGDIFonts/2) ? TRUE : FALSE;
  LogFont.lfUnderline      = 0;
  LogFont.lfStrikeOut      = 0;
  LogFont.lfCharSet        = DEFAULT_CHARSET;
  LogFont.lfOutPrecision   = OUT_DEVICE_PRECIS;//OUT_DEFAULT_PRECIS;// OUT_RASTER_PRECIS;//
  LogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
  LogFont.lfQuality        = PROOF_QUALITY;//DEFAULT_QUALITY;
  LogFont.lfPitchAndFamily = DEFAULT_PITCH/*FIXED_PITCH*/|FF_DONTCARE;//FF_MODERN;
  strcpy(LogFont.lfFaceName, "Lucida Console");
  //strcpy(LogFont.lfFaceName, "Lucida Sans Unicode");

  //strcpy(LogFont.lfFaceName, "Book Antiqua");
  //strcpy(LogFont.lfFaceName, "Century Gothic");
  //strcpy(LogFont.lfFaceName, "Courier");
  //strcpy(LogFont.lfFaceName, "Letter Gothic");
  ////strcpy(LogFont.lfFaceName, "Lucida Console");
  //strcpy(LogFont.lfFaceName, "Trebuchet MS");

  //strcpy(LogFont.lfFaceName, "Tahoma");
  //strcpy(LogFont.lfFaceName, "Univers");
  //strcpy(LogFont.lfFaceName, "Verdana");
  //strcpy(LogFont.lfFaceName, "Courier New Bold");
  //strcpy(LogFont.lfFaceName, "Comic Sans MS");

  m_TheFontL.CreateFontIndirect(&LogFont);
  
  LogFont.lfHeight         = 10;
  m_TheFontT.CreateFontIndirect(&LogFont);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  SetUpDlg();

  m_ToolTip.Create(this);
  CRect Rc;
  GetDlgItem(IDC_DATASTATIC)->GetWindowRect(&Rc);
  ScreenToClient(&Rc);
  //m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK, &Rc, IDC_DATASTATIC); 
  m_ToolTip.AddTool(this, "B", &Rc, IDC_DATASTATIC); 

  GetDlgItem(IDC_LDRSTATIC)->GetWindowRect(&Rc);
  ScreenToClient(&Rc);
  m_ToolTip.AddTool(this, "A", &Rc, IDC_LDRSTATIC); 
  //m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK, &Rc, IDC_LDRSTATIC); 

  m_ToolTip.SetDelayTime(200); 

  m_StoppedBitmap.LoadMappedBitmap(IDB_STOP);
  m_RunningBitmap.LoadMappedBitmap(IDB_RUN);
  m_StepBitmap.LoadMappedBitmap(IDB_STEP);
  ((CButton*)GetDlgItem(IDSTOP))->SetBitmap((HBITMAP)m_StoppedBitmap);
  ((CButton*)GetDlgItem(IDRUN))->SetBitmap((HBITMAP)m_RunningBitmap);
  ((CButton*)GetDlgItem(IDSTEP))->SetBitmap((HBITMAP)m_StepBitmap);

	return TRUE;  // return TRUE  unless you set the focus to a control
  }

BOOL CPLC5Dlg::OnGetToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
  {
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
  UINT nID =pTTTStruct->idFrom;
  
 // if(nID)
 //   {
 //   
 //   return(TRUE);
 //   }
  return FALSE;
  };


void CPLC5Dlg::OnSysCommand(UINT nID, LPARAM lParam)
  {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	  {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	  }
	else
	  {
		CDialog::OnSysCommand(nID, lParam);
	  }
  }

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPLC5Dlg::OnPaint()
  {
  if (IsIconic())
	  {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	  }
  else
    {
		CPaintDC dc(this); // device context for painting
    PaintDynamics(dc);

    if (m_sInitFile.GetLength())
      ::AfxGetMainWnd()->PostMessage(WM_INITLOAD, 0);
    }
  }

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPLC5Dlg::OnQueryDragIcon()
  {
	return (HCURSOR) m_hIcon;
  }


void CPLC5Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

  int Y=pScrollBar->GetScrollPos();

  SCROLLINFO Info;
  Info.cbSize=sizeof(Info);
  Info.fMask=SIF_ALL;
  pScrollBar->GetScrollInfo(&Info);

  BOOL Skip=FALSE;
  switch (nSBCode)
    {
    case SB_BOTTOM        : Info.nPos=Info.nMax-Info.nPage; break;
    case SB_LINEDOWN      : Info.nPos+=1; break;
    case SB_LINEUP        : Info.nPos-=1; break;
    case SB_PAGEDOWN      : Info.nPos+=Info.nPage; break;
    case SB_PAGEUP        : Info.nPos-=Info.nPage; break;
    case SB_THUMBPOSITION : Info.nPage=nPos; // Scroll to the absolute position. The current position is provided in nPos.
    case SB_THUMBTRACK    : Info.nPos=(nPos); break; // Drag scroll box to specified position. The current position is provided in nPos.
    case SB_TOP           : Info.nPos=0; break;
    case SB_ENDSCROLL     :
    default:
      Skip=TRUE;
      break;
    }

  Info.fMask=SIF_POS;
  pScrollBar->SetScrollInfo(&Info);

  int NewY=pScrollBar->GetScrollPos();

  if (!Skip && (NewY!=Y))
    RedrawTheDisplay(pScrollBar==&m_LdScroll ? Vw_Ladder : Vw_Table);

  }

//===========================================================================

void CPLC5Dlg::SetUpDlg()
  {
  m_Plc.InitTree(m_Tree);

  //pOPCCallBack->InitAPLCTag("I", "00"); 

  m_iPcIndex=0;
  m_iLdIndex=-1;
  m_iTbIndex=-1;
  FixScrollBar();

  m_TimerID=0;
  m_iState=State_Stopped;
  m_iReqdState=-1;
  m_iStepEndState=-1;

  //m_iIteration=0;

  SetRunState();
  };

//===========================================================================

void CPLC5Dlg::FixScrollBar()
  {
  SCROLLINFO Info;

  if (m_ChgMonitorOn)
    {
    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_ALL;
    m_TbScroll.GetScrollInfo(&Info);


    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE;
    Info.nMin=0;

    Info.nMax=m_History.Length()-5;
    m_HistLine1=Min(m_HistLine1,long(Info.nMax-1));
    Info.nPos=m_HistLine1;
    Info.nPage=10;

    m_TbScroll.SetScrollInfo(&Info);
    }
  else if (m_iPcIndex>=0 && m_iTbIndex>=0 && (m_Plc[m_iPcIndex].m_Table[m_iTbIndex]))
    {
    CTable&T=*m_Plc[m_iPcIndex].m_Table[m_iTbIndex];
    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_ALL;
    m_TbScroll.GetScrollInfo(&Info);


    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE;
    Info.nMin=0;

    Info.nMax=(T.NVals()-1)/T.ValsPerColumn()+1;
    T.m_Line1=Min(T.m_Line1,Info.nMax-1);
    Info.nPos=T.m_Line1;
    Info.nPage=10;

    m_TbScroll.SetScrollInfo(&Info);
    }

  if (m_iPcIndex>=0 && m_iLdIndex>=0)
    {
    CLadder&L=m_Plc[m_iPcIndex].m_Ladder[m_iLdIndex];
    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_ALL;
    m_LdScroll.GetScrollInfo(&Info);


    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE;
    Info.nMin=0;

    Info.nMax=L.m_Rungs.GetSize()-1;
    L.m_Line1=Min(L.m_Line1, Info.nMax);
    Info.nPos=Min(L.m_Line1,Info.nMax-1);
    Info.nPage=1;//30/m_HeightMax;

    m_LdScroll.SetScrollInfo(&Info);
    }
  }

//===========================================================================

void CPLC5Dlg::SetRunState(int iState)
  {
  if (m_Plc.GetSize()<=0)
    return;

  int OldState=m_iState;
  if (iState>=0)
    m_iState=iState;

  m_State.Format("%s: Step:%i  Iter:%i", States[m_iState], 
                 m_nStepNo, m_Plc[Max(0,m_iPcIndex)].m_IterNo);

  m_StatusMsg=(m_Reset ? "RESET: ":"");
  CString Xtra;
  if (m_iPcIndex>0)
    {
    if (m_Plc[m_iPcIndex].m_nUnImpOpCd>0)
      {
      CString S;
      S.Format("UnImplemented OpCodes:%i ",m_Plc[m_iPcIndex].m_nUnImpOpCd);
      Xtra+=S;
      }
    if (m_Plc[m_iPcIndex].m_nInCompOpCd>0)
      {
      CString S;
      S.Format("Incomplete OpCodes:%i ",m_Plc[m_iPcIndex].m_nInCompOpCd);
      Xtra+=S;
      }
    if (m_RunningSlow)
      {
  #ifdef _RELEASE
      Xtra+="SLOW ";
  #else
      Xtra+="SLOW[DEBUG] ";
  #endif
      }
    }
  if (Xtra.GetLength()==0)
    Xtra="OK";
  m_StatusMsg+=Xtra;

  m_ExecTime.Format("Exec:%4.1f",m_EvalTime);
  m_UpdTime.Format("Disp:%4.1f",m_DispTime);
  UpdateData(FALSE);

  GetDlgItem(IDC_EDITITER)->EnableWindow(m_iState<=State_Stopped ? 1 : 0);
  GetDlgItem(IDC_EDITSTEP)->EnableWindow(m_iState<=State_Stopped ? 1 : 0);
  GetDlgItem(IDC_EDITRUNMULT)->EnableWindow(m_iState<=State_Stopped ? 1 : 0);
  //GetDlgItem(IDSTEP)->EnableWindow(m_iState!=State_Running? 1 : 0);

  ((CButton*)GetDlgItem(IDSTOP))->SetCheck(m_iState==State_Stopped ? 1:0);
  ((CButton*)GetDlgItem(IDRUN))->SetCheck(m_iState==State_Running? 1:0);

  //SetFocus();
  //GotoDlgCtrl(GetDlgItem(IDC_LDRSTATIC));
//  if (OldState!=m_iState)
//    {
//    switch (m_iState)
//      {
//      case State_Stopped:
//        theApp.m_ScdCmdIF.DoEventMsg(ComState_Stop,0);
//        break;
//      case State_Idling:
//        theApp.m_ScdCmdIF.DoEventMsg(ComState_Idle,0);
//        break;
//      }    
//    }
  };

//===========================================================================

void CPLC5Dlg::ActivateInterface(bool On)
  {
  GetDlgItem(IDSTOP)->EnableWindow(On ? 1 : 0);
  GetDlgItem(IDRUN)->EnableWindow(On ? 1 : 0);
  GetDlgItem(IDSTEP)->EnableWindow(On ? 1 : 0);
  };

//===========================================================================

void CPLC5Dlg::RedrawTheDisplay(byte What, UINT Flags)
  {
  CRect DlgRct, StaticRct;
  if (What&Vw_Table)
    {
    if (m_iTbIndex>=0)
      {
      CString S;
      S.Format("Data File : %s.%s", 
               m_Plc[m_iPcIndex].m_sPlcName, 
               m_Plc[m_iPcIndex].Table(m_iTbIndex).m_sName);
      m_TbHeader.SetWindowText(S);
      }

    GetClientRect(&DlgRct);
    ClientToScreen(&DlgRct);
    GetDlgItem(IDC_DATASTATIC)->GetClientRect(&StaticRct);
    GetDlgItem(IDC_DATASTATIC)->ClientToScreen(&StaticRct);
    StaticRct-=DlgRct.TopLeft();
    RedrawWindow(&StaticRct, NULL, Flags);
    }
  if (What&Vw_Ladder)
    {
    if (m_iPcIndex>=0 && m_iLdIndex>=0)
      {
      CString S;
      S.Format("Ladder File %i: %s.%s", m_iLdIndex, m_Plc[m_iPcIndex].m_sPlcName, m_Plc[m_iPcIndex].m_Ladder[m_iLdIndex].m_sName);
      m_LdHeader.SetWindowText(S);
      }

    GetClientRect(&DlgRct);
    ClientToScreen(&DlgRct);
    GetDlgItem(IDC_LDRSTATIC)->GetClientRect(&StaticRct);
    GetDlgItem(IDC_LDRSTATIC)->ClientToScreen(&StaticRct);
    StaticRct-=DlgRct.TopLeft();
    RedrawWindow(&StaticRct, NULL, Flags);
    }
  }

//===========================================================================

void CPLC5Dlg::PaintTable(CPaintDC & dc, CRect & TheCRct, CRect & TheRct, CPoint &TheOff)
  {
  char Buff[4096];
  Buff[0]=0;

  if (m_iPcIndex>=0 && m_iTbIndex>=0)
    {
    int LastY=TheCRct.bottom+TheOff.y;

    CTable & Tb=m_Plc[m_iPcIndex].Table(m_iTbIndex);
    int iLnNo=0;
    Tb.m_Line1=m_TbScroll.GetScrollPos();

    COLORREF bkcolor=dc.GetBkColor();
    COLORREF txtcolor=dc.GetTextColor();

    int nLines=((Tb.NVals()-1)/Tb.ValsPerColumn())+1;
    for (int l=Tb.m_Line1; ; l++)
      {
      long iLnNo=l+1;
      int Y0=TheOff.y+m_CharSz.y*(l-Tb.m_Line1);//-m_CharSz.y/2;
      int Y1=Y0+m_CharSz.y;
      int X0=TheOff.x-m_CharSz.x/2;
      int X1=X0+TheCRct.Width()+m_CharSz.x;

      if (l<nLines)
        {
        int i=l*Tb.ValsPerColumn();
        sprintf(Buff, Tb.OctalAdd()?" %4o:":" %4i:",i);

        CRect rc(X0,Y0,X1,Y1), rc1;
        rc1.IntersectRect(&rc, &TheRct);
        dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,Buff, strlen(Buff),NULL);

        for (int j=0; i+j<Tb.NVals() && j<Tb.ValsPerColumn(); j++)
          {
          for (int k=0; k<Tb.NStructVals(); k++)
            {
            bool chg=false;
            bool frc=false;
            if (Tb.IsFlt())
              {
              sprintf(Buff, "%*s%*g",Tb.ColumnWidth()-12,"",12, Tb.FValue(i+j,k));
              chg=Tb.FValue(i+j,k)!=Tb.FValuePrev(i+j,k);
              }
            else
              {
              bool DoHex;
              if (Tb.ReqdFmt(k)=='x')
                DoHex=true;
              else if (Tb.ReqdFmt(k)=='i')
                DoHex=false;
              else
                DoHex=Tb.m_HexDispReqd;

              if (DoHex)
                sprintf(Buff, "%*s0x%04x",Tb.ColumnWidth()-7,"", Tb.UIValue(i+j,k));
              else
                sprintf(Buff, "%*i",Tb.ColumnWidth()-1, Tb.IValue(i+j,k));
              chg=Tb.IValue(i+j,k)!=Tb.IValuePrev(i+j,k);
              frc=Tb.IsIValueFrc(i+j,k);
              }
            int X0=TheOff.x+m_CharSz.x/2+m_CharSz.x*(6+(j+k)*Tb.ColumnWidth());
            int X1=X0+m_CharSz.x*strlen(Buff);//+10000;
            CRect rc(X0,Y0,X1,Y1), rc1;
            rc1.IntersectRect(&rc, &TheRct);
            //COLORREF color=dc.SetBkColor(chg ? RGB(0xcc,0xcc,0xcc) : bkcolor);
            bool Slct=m_iWdIndex==(i+j) && (m_iBtIndex<0 || m_iBtIndex==k);
            DWORD Brgb=bkcolor;
            DWORD Trgb=txtcolor;
            if (chg)
              Brgb=RGB(0xcc,0xcc,0xcc);
            else if (Slct)
              Brgb=RGB(0x00,0xff,0xff);
            //else if (frc)
            //  Brgb=RGB(0xff,0x00,0x00);
            //if (frc)
            //  Trgb=RGB(0xff,0x00,0x00);
            dc.SetBkColor(Brgb);
            dc.SetTextColor(Trgb);

            dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,Buff, strlen(Buff),NULL);
            if (frc)
              {
              Brgb=RGB(0xff,0x00,0x00);
              dc.SetBkColor(Brgb);
              int X0=TheOff.x+m_CharSz.x/2+m_CharSz.x*(6+(j+k)*Tb.ColumnWidth()+Tb.ColumnWidth()-1);
              int X1=X0+m_CharSz.x*1;//+10000;

              CRect rc(X0,Y0,X1,Y1), rc1;
              rc1.IntersectRect(&rc, &TheRct);

              dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,"*", 1,NULL);
              }
            
            dc.SetBkColor(bkcolor);
            dc.SetTextColor(txtcolor);
            }
          }
        }
      else
        {
        int nc=(TheCRct.Width()*11)/(m_CharSz.x*10);
        memset(Buff, ' ', nc);
        Buff[nc]=0;
        CRect rc(X0,Y0,X1,Y1), rc1;
        rc1.IntersectRect(&rc, &TheRct);
        dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,Buff, strlen(Buff),NULL);
        }
      iLnNo++;
      if (Y1>=LastY)//TheCRct.bottom)
        break;
      };
    dc.SetBkColor(bkcolor);
    }
  };

//===========================================================================

void CPLC5Dlg::PaintLadder(CPaintDC & dc, CRect & TheCRct, CRect & TheRct, CPoint &TheOff)
  {
  char Buff[4096];
  Buff[0]=0;
  if (m_iPcIndex>=0 && m_iLdIndex>=0)
    {
    int LastY=TheCRct.bottom+TheOff.y;

    CRungImage Img;
    m_LdShft=CPoint(m_CharSz.x*5+m_CharSz.x/2, 0);

    CLadder & Ld=m_Plc[m_iPcIndex].m_Ladder[m_iLdIndex];
    Ld.m_Line1=m_LdScroll.GetScrollPos();
    long iLnNo=0;
    byte Done=false;
    m_DispLst.m_ActLen=0;

    int DX[512];
    for (int i=0; i<512; i++)
      DX[i]=m_CharSz.x;
//            DX[i]=TheCRct.Width()/Ld.m_MaxWidth;
    
    for (int l=Ld.m_Line1; !Done ; l++)
      {
      int X0=TheOff.x-m_CharSz.x/2;
      int X1=X0+TheCRct.Width()+m_CharSz.x;
      if (l<Ld.m_Rungs.GetSize())
        {
        CRung &R=*Ld.m_Rungs[l];
        Img.Clear();
//              if (dbgf) fprintf(dbgf,"--------------\n");
        R.BuildTxtImage(Img, m_DispLst, iLnNo);
//              if (dbgf) fprintf(dbgf,"--------------\n");

        for (int h=0; !Done && h<Img.m_Height; h++)
          {
          if (h==1)
            sprintf(Buff, " %4i%s",l,&Img.m_C[h][0]);
          else
            sprintf(Buff, "     %s",&Img.m_C[h][0]);
          //sprintf(Buff, " %s",&Img.m_C[h][0]);
          int Y0=TheOff.y+m_CharSz.y*(iLnNo+h)+1;//m_CharSz.y;
          int Y1=Y0+m_CharSz.y;
          CRect rc(X0,Y0,X1,Y1), rc1;
          rc1.IntersectRect(&rc, &TheRct);
          //rc1.InflateRect(0,1,0,0

          DWORD oldcolour;
          if (Img.m_Colour[h]>0)
            {
            COLORREF rgb=RGB(0xff,0x00,0x00);
            oldcolour=dc.SetTextColor(rgb);
            }

          dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,Buff, strlen(Buff), DX);
          if (Img.m_Colour[h]>0)
            dc.SetTextColor(oldcolour);
            
          if (Y1>=LastY)
            Done=true;
          }
        iLnNo+=Img.m_Height;
        }
      else
        {
        int nc=(TheCRct.Width()*11)/(m_CharSz.x*10);
        memset(Buff, ' ', nc);
        Buff[nc]=0;
        int Y0=TheOff.y+m_CharSz.y*(iLnNo)+1;
        int Y1=Y0+m_CharSz.y;
        CRect rc(X0,Y0,X1,Y1), rc1;
        rc1.IntersectRect(&rc, &TheRct);
        dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,Buff, strlen(Buff),NULL);
        if (Y1>=LastY)
          Done=true;
        iLnNo++;
        }
      }
    int nFullRungs=l-Ld.m_Line1-1;

    SCROLLINFO Info;
    Info.cbSize=sizeof(Info);
    Info.fMask=SIF_PAGE;
    Info.nPage=Max(1, nFullRungs);

    m_LdScroll.SetScrollInfo(&Info);
    }
  };

//===========================================================================

void CPLC5Dlg::PaintChangeHistory(CPaintDC & dc, CRect & TheCRct, CRect & TheRct, CPoint &TheOff)
  {
  char Buff[4096];
  Buff[0]=0;
  int LastY=TheCRct.bottom+TheOff.y;

  COLORREF bkcolor=dc.GetBkColor();
  COLORREF txtcolor=dc.GetTextColor();

  int iLnNo=0;
  m_HistLine1=m_TbScroll.GetScrollPos();

  int nLines=m_History.Length();
  for (int l=m_HistLine1; ; l++)
    {
    long iLnNo=l+1;
    int Y0=TheOff.y+m_CharSz.y*(l-m_HistLine1);//-m_CharSz.y/2;
    int Y1=Y0+m_CharSz.y;
    int X0=TheOff.x+1;//+m_CharSz.x/2;
    int X1=X0+TheCRct.Width()+m_CharSz.x;

    if (l<nLines)
      {
      LPCTSTR pStr=m_History.Value(l);

      CRect rc(X0,Y0,X1,Y1), rc1;
      rc1.IntersectRect(&rc, &TheRct);
      dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,pStr, strlen(pStr),NULL);
      }
    else
      {
      int nc=(TheCRct.Width()*11)/(m_CharSz.x*10);
      memset(Buff, ' ', nc);
      Buff[nc]=0;
      CRect rc(X0,Y0,X1,Y1), rc1;
      rc1.IntersectRect(&rc, &TheRct);
      dc.ExtTextOut(X0,Y0,ETO_CLIPPED|ETO_OPAQUE,&rc1,Buff, strlen(Buff),NULL);
      }
    iLnNo++;
    if (Y1>=LastY)//TheCRct.bottom)
      break;
    };
  
  dc.SetBkColor(bkcolor);
  };

//===========================================================================

void CPLC5Dlg::PaintDynamics(CPaintDC &dc)
  {
//  FILE * olddbgf=dbgf;
//  dbgf=fopen("\\MosRef\\ESD PLC\\dbgpaint.txt", "wt");//"a+t");

  //int Line1=m_LadScroll.GetScrollPos();
  for (int iVw=Vw_Table; iVw<=Vw_Ladder; iVw++)
    {
    CFont *OldFont=dc.SelectObject(iVw==Vw_Table ? &m_TheFontT : &m_TheFontL);

    CRect TheCRct;
    CPoint DTheOff(0,0);
    CPoint TheOff(0,0);
    //CPoint &TheOff = iVw==Vw_Table ? m_TbOff : m_LdOff;//(0,0);

    //TheOff=CPoint(0,0);

    GetDlgItem(iVw==Vw_Table ? IDC_DATASTATIC : IDC_LDRSTATIC)->GetClientRect(&TheCRct);
    GetDlgItem(iVw==Vw_Table ? IDC_DATASTATIC : IDC_LDRSTATIC)->ClientToScreen(&TheOff);
    ClientToScreen(&DTheOff);
    TheOff-=DTheOff;

    CRect TheRct=TheCRct;
    TheRct.OffsetRect(TheOff);

    TEXTMETRIC TM;
    dc.GetTextMetrics(&TM);
    m_CharSz.x=TM.tmAveCharWidth;
    m_CharSz.y=TM.tmHeight;

    dc.SetBkColor(COLORREF(RGB(255,255,255)));
    //int LastY=TheCRct.Height()+TheOff.y;

    switch (iVw)
      {
      case Vw_NULL:
        break;
      case Vw_Table:
        {
        if (m_ChgMonitorOn)
          PaintChangeHistory(dc, TheCRct, TheRct, TheOff);
        else
          PaintTable(dc, TheCRct, TheRct, TheOff);
        };
        break;
      case Vw_Ladder:
        {
        PaintLadder(dc, TheCRct, TheRct, TheOff);
        };
        break;
      default:;
      }
    dc.SelectObject(OldFont);
    }

//  fclose(dbgf);
//  dbgf=olddbgf;
  }

//===========================================================================

void CPLC5Dlg::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
  {
	
	*pResult = 0;
  }

void CPLC5Dlg::SetDisplayTable(int iPc, int iTb, int iWd, int iBt)
  {
  if (iPc>=0 && iPc<m_Plc.GetSize() && iTb>=0 && iTb<m_Plc[iPc].m_nTables && m_Plc[iPc].m_Table[iTb])
  //if (iTb>=0 && iTb<m_nTables)
    {
    m_iPcIndex=iPc;
    m_iTbIndex=iTb;
    m_iWdIndex=iWd;
    m_iBtIndex=iBt;
    CTable & T=m_Plc[m_iPcIndex].Table(m_iTbIndex);
    m_DisplayFmt=T.m_HexDispReqd ? 1:0;

    GetDlgItem(IDC_RADIODEC)->EnableWindow(T.ReqdFmt(0)==' ');
    GetDlgItem(IDC_RADIOHEX)->EnableWindow(T.ReqdFmt(0)==' ');
    }
  else
    {
    //m_iPcIndex=-1;
    m_iTbIndex=-1;
    m_iWdIndex=-1;
    m_iBtIndex=-1;
    GetDlgItem(IDC_RADIODEC)->EnableWindow(FALSE);
    GetDlgItem(IDC_RADIOHEX)->EnableWindow(FALSE);
    }

  UpdateData(FALSE);
  FixScrollBar();
  }

void CPLC5Dlg::SetDisplayLadder(int iPc, int iLd)
  {
  if (iPc>=0 && iLd>=0 && iLd<m_Plc[iPc].m_nLadders)
    {
    m_iPcIndex=iPc;
    m_iLdIndex=iLd;
    }
  else
    {
    //m_iPcIndex=-1;
    m_iLdIndex=-1;
    }
  UpdateData(FALSE);
  FixScrollBar();
  }

void CPLC5Dlg::SetDisplaySymbol(int iPc, int iSm)
  {
  if (iPc>=0 && iPc<NPlcs() && iSm>=0 && iSm<m_Plc[iPc].m_AddInfo.GetSize())
    {
    CAddInfo * pInfo=m_Plc[iPc].m_AddInfo[iSm];
    SetDisplayTable(iPc, pInfo->m_pAddress->FileNo(), pInfo->m_pAddress->WordNo(false), -1);//pInfo->m_pAddress->BitNo(false));
    }
  else
    SetDisplayTable(m_iPcIndex, m_iTbIndex, -1,-1);

  UpdateData(FALSE);
  FixScrollBar();
  }

void CPLC5Dlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
  {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
  HTREEITEM h=pNMTreeView->itemNew.hItem;
  DWORD D=m_Tree.GetItemData(h);

  int iVw=ExtractItemVw(D);
  if (iVw==Vw_Table)
    SetDisplayTable(ExtractItemPc(D), ExtractItemData(D));
  else if (iVw==Vw_Ladder)
    SetDisplayLadder(ExtractItemPc(D), ExtractItemData(D));
  else if (iVw==Vw_Symbol)
    SetDisplaySymbol(ExtractItemPc(D), ExtractItemData(D));
  else
    {
    //SetDisplayTable(-1, -1);
    //SetDisplayLadder(-1, -1);
    //SetDisplaySymbol(-1, -1);
    }
  FixScrollBar();
  RedrawTheDisplay(iVw);

  *pResult = 0;
  }

void CPLC5Dlg::OnIter()
  {
	UpdateData(TRUE);
  CStopWatch SW;

  int OldReqdState=m_iReqdState;
 
  if (m_iReqdState>=0)
    {
    if (m_iReqdState==m_iState)
      m_iReqdState=-1;

    switch (m_iState)
      {
      case State_Stopped:
        {
        m_nStepNo=0;
        m_nIterNo=0;
        for (int p=0; p<NPlcs(); p++)
          m_Plc[p].m_IterNo=0;
        }
      case State_Idling:
        {
        SetRunState(m_iReqdState);
        m_iReqdState=-1;
        break;
        }
      case State_Running:
        {
        int xxx=0;
        break;
        }
      }
    }

  switch (m_iState)
    {
    case State_Stopped:
      {
      //_asm int 3;
      break;
      }
    case State_Running:
      {
      SW.ReStart();

      
      for (int p=0; p<NPlcs(); p++)
        {
        m_Plc[p].SaveData();
        m_Plc[p].Evaluate(m_nActualIterMS, m_Reset!=FALSE, m_Remote!=FALSE);
        }

      for (int x=0; x<m_Xfer.GetSize(); x++)
        m_Xfer[x]->DoTransfer();
      for (x=0; x<m_Force.GetSize(); x++)
        m_Force[x]->DoTransfer();

      m_nIterNo++;
      m_History.m_nIterNo=m_nIterNo;
      if (m_nIterNo%m_nItsPerStep==0)
        {
        m_lSequenceNo++;
        m_nStepNo++;
        //theApp.m_ScdCmdIF.DoEventMsg(ComState_StepDynamic,0);
        theApp.m_ScdCmdIF.DoEventMsg(ComState_StepDynamic,0);
		//theApp.m_ScdCmdIF.m_EventSender.Send(ComState_StepDynamic,0);



        if (m_iReqdState>=0)
          {
          SetRunState(m_iReqdState);
          m_iReqdState=-1;
          }
        else if (m_nStepsReqd>0)
          {
          m_nStepsReqd--;
          if (m_nStepsReqd==0)
            {
            if (m_iStepEndState==State_Stopped)
              {
              StopTheTimer();
              SetRunState(State_Stopped);
              }
            else
              SetRunState(State_Idling);
            }
          }
        }
      m_EvalTime=SW.Secs()*1000;

//      m_History.Add("-------------------");
//
//      if (dbgf)
//        fprintf(dbgf, "-------------------\n");

      }
      // Fall thru
    case State_Idling:
      {
      FixScrollBar();
      
      m_TimeTillUpdate-=m_IterMS;
      if (m_TimeTillUpdate<=0)
        {
        SW.ReStart();
        m_TimeTillUpdate=m_UpdateMS;
        RedrawTheDisplay(0xff, RDW_INVALIDATE | RDW_UPDATENOW);
        m_DispTime=SW.Secs()*1000;
        SW.ReStart();
        }

      if (dbgf) 
        fflush(dbgf);
      UpdateData(FALSE);
      break;
      }
    }
  SetRunState();
  
  if (m_iReqdState<0 && OldReqdState>=0)
    {
    switch (m_iState)
      {
      case State_Stopped:
        theApp.m_ScdCmdIF.DoEventMsg(ComState_Stop,0);
        break;
      case State_Idling:
        theApp.m_ScdCmdIF.DoEventMsg(ComState_Idle,0);
        break;
      }    
    }

  }

void CPLC5Dlg::OnStep()
  {
  switch (m_iState)
    {
    case State_Running:
      if (m_nStepsReqd>=0)
        m_nStepsReqd+=1;
      break;
    case State_Idling:
      m_iReqdState=State_Running;
      m_iStepEndState=m_iState;
      m_nStepsReqd=Max(1L,m_nStepsReqd+1L);
      break;
    case State_Stopped:
      m_iReqdState=State_Running;
      m_iStepEndState=m_iState;
      m_nStepsReqd=1;
      StartTheTimer();
      break;
    }
  }

void CPLC5Dlg::StartTheTimer()
  {
  // must reset timer
  m_nItsPerStep=Max(1U, m_StepMS/m_IterMS);
  m_nActualIterMS=m_StepMS/m_nItsPerStep;
  m_TimerID=SetTimer(1, Max((UINT)10, (UINT)(m_IterMS/m_RunMult)), NULL);
  // Do First Iter immediately
  SendMessage(WM_COMMAND, IDITER);
  }
void CPLC5Dlg::StopTheTimer()
  {
  if (m_TimerID!=0)
    KillTimer(1);
  m_TimerID=0;
  }

void CPLC5Dlg::GotoRun()
  {
  if (m_iState!=State_Running)
    {
    UpdateData(TRUE);
    
    m_nStepsReqd=-1;
    m_RunningSlow=false;
    m_iReqdState=State_Running;
    StartTheTimer();

    m_RunningTicks=GetTickCount();
    }
  }

void CPLC5Dlg::GotoIdle()
  {
  if (m_iState!=State_Idling)
    {
    m_iReqdState=State_Idling;
    if (!TimerRunning())
      StartTheTimer();
    }
  else
    theApp.m_ScdCmdIF.DoEventMsg(ComState_Idle,0);
  }

void CPLC5Dlg::GotoStop()
  {
  if (m_iState!=State_Stopped)
    m_iReqdState=State_Stopped;
  else
    theApp.m_ScdCmdIF.DoEventMsg(ComState_Stop,0);
  }

void CPLC5Dlg::OnRun()
  {
  if (m_iState!=State_Running)
    GotoRun();
  else
    GotoIdle();
  }

void CPLC5Dlg::OnStop()
  {
  if (m_iState!=State_Stopped)
    GotoStop();
  else
    GotoIdle();
  }

void CPLC5Dlg::OnTimer(UINT nIDEvent)
  {
	if (nIDEvent==1)
    SendMessage(WM_COMMAND, IDITER);

	CDialog::OnTimer(nIDEvent);
  }

void CPLC5Dlg::OnOK()
  {
	//CDialog::OnOK();
  }

void CPLC5Dlg::OnCancel()
  {
	CDialog::OnCancel();
  }

int CPLC5Dlg::PointInClient(CPoint &point)
  {
  int ret=0;
  CPoint pt(point);
  ClientToScreen(&pt);

  CRect rc;
  m_LdStatic.GetClientRect(&rc);
  m_LdStatic.ClientToScreen(&rc);
  if (rc.PtInRect(pt))
    {
    m_LdStatic.ScreenToClient(&pt);
    point=pt;
    return 1;
    }

  m_TbStatic.GetClientRect(&rc);
  m_TbStatic.ClientToScreen(&rc);
  if (rc.PtInRect(pt))
    {
    m_TbStatic.ScreenToClient(&pt);
    point=pt;
    return 2;
    }
  return 0;
  }

CPoint PtToChar(CPoint Pt, CPoint Shft, CPoint Char)
  {
  Pt.x=(Pt.x-Shft.x)/Char.x;
  Pt.y=(Pt.y-Shft.y)/Char.y;
  return Pt;
  }

CMtxElement * TargetMtxElement(CLadderDisplayList &m_DispLst, CPoint Pt, CPoint CPt, CPoint &RPt)
  {
  for (int i=0; i<m_DispLst.m_ActLen; i++)
    if (m_DispLst[i].m_Pos.PtInRect(CPt))
      {
      RPt=CPt;
      RPt-=m_DispLst[i].m_Pos.TopLeft();
      return m_DispLst[i].m_pE;
      }
  return NULL;
  }

CAddress * TargetAddress(CLadderDisplayList &m_DispLst, CPoint Pt, CPoint CPt)
  {
  for (int i=0; i<m_DispLst.m_ActLen; i++)
    if (m_DispLst[i].m_Pos.PtInRect(CPt))
      if (m_DispLst[i].m_pE)
        return m_DispLst[i].m_pE->TargetAddress(Pt, CPt);
  return NULL;
  }

void CPLC5Dlg::OnLButtonDown(UINT nFlags, CPoint point)
  {
  switch (PointInClient(point))
    {
    case 1:
      {
      m_LdScroll.SetFocus();

      CPoint CPt=PtToChar(point, m_LdShft, m_CharSz);
      CAddress * pA=TargetAddress(m_DispLst, point, CPt);
      if (pA)
        {
        if (nFlags & MK_SHIFT)
          {
          SetDisplayTable(m_iPcIndex, pA->Table()->m_iIndex);
          RedrawTheDisplay(Vw_Table);
          }
        else
          {
          if (pA->IsBit())
            {
            }
          else if (pA->IsFlt())
            {
            }
          else
            {
            }
          }
        }
      }
      break;
    case 2:
      m_TbScroll.SetFocus();
      break;
    }
	
  CDialog::OnLButtonDown(nFlags, point);
  }

void CPLC5Dlg::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  //int r,c;
  switch (PointInClient(point))
    {
    case 1:
      {
      CPoint CPt=PtToChar(point, m_LdShft, m_CharSz);
      CAddress * pA=TargetAddress(m_DispLst, point, CPt);
      if (pA)
        {
        if (pA->IsBValueFrc())
          pA->SetBValueFrc(!pA->BValue());
        else
          pA->SetBValue(!pA->BValue());
        RedrawTheDisplay(Vw_Table);
        }
      }
      break;
    case 2:
      {
      }
      break;
    }

	CDialog::OnLButtonDblClk(nFlags, point);
  }

void CPLC5Dlg::OnLButtonUp(UINT nFlags, CPoint point)
  {
	CDialog::OnLButtonUp(nFlags, point);
  }

void CPLC5Dlg::OnRButtonDown(UINT nFlags, CPoint point)
  {
  //TRACE("RB %i,%i\n",point.x,point.y);
  switch (PointInClient(point))
    {
    case 1:
      {
      m_LdScroll.SetFocus();
      CPoint CPt=PtToChar(point, m_LdShft, m_CharSz);
      CPoint RPt;
      CMtxElement * pE=TargetMtxElement(m_DispLst, point, CPt, RPt);
      if (pE)
        {
        //CPt.x-=pE->m_X;
        //CPt.y-=pE->MyRung();
        CAddress * pA=pE->TargetAddress(point, RPt);
        CLadder * pL=pE->TargetLadder(point, RPt);
        byte What=0;
        if (pL)
          {
          SetDisplayLadder(m_iPcIndex, pL->m_iIndex);
          What|=Vw_Ladder;
          }
        if (pA)
          {
          SetDisplayTable(m_iPcIndex, pA->Table()->m_iIndex);
          What|=Vw_Table;
          }
        if (What)
          RedrawTheDisplay(What);
        }
      }
      break;
    case 2:
      m_TbScroll.SetFocus();
      break;
    }
	
	CDialog::OnRButtonDown(nFlags, point);
  }

void CPLC5Dlg::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
  switch (PointInClient(point))
    {
    case 1:
      {
      CPoint CPt=PtToChar(point, m_LdShft, m_CharSz);
      CAddress * pA=TargetAddress(m_DispLst, point, CPt);
      if (pA)
        {
        if (pA->IsBValueFrc())
          pA->ClrBValueFrc();
        else
          pA->SetBValueFrc(!pA->BValue());
        RedrawTheDisplay(Vw_Table);
        }
      }
      break;
    case 2:
      {
      }
      break;
    }

	CDialog::OnRButtonDblClk(nFlags, point);
}

void CPLC5Dlg::OnRadiodec()
  {
  UpdateData(TRUE);
  if (m_iTbIndex>=0)
    {
    m_Plc[m_iPcIndex].Table(m_iTbIndex).m_HexDispReqd=false;
    RedrawTheDisplay(Vw_Table);
    }
  }

void CPLC5Dlg::OnRadiohex()
  {
  UpdateData(TRUE);	
  if (m_iTbIndex>=0)
    {
    m_Plc[m_iPcIndex].Table(m_iTbIndex).m_HexDispReqd=true;
    RedrawTheDisplay(Vw_Table);
    }
  }

void CPLC5Dlg::OnChangeEditgoto()
  {
  UpdateData(TRUE);
  if (m_sGoto.GetLength()>0)
    {
    bool IsNum=true;
    for (int i=0; i<m_sGoto.GetLength(); i++)
      if (!isdigit(m_sGoto[i]))
        IsNum=false;
    if (m_iPcIndex>=0)
      {
      if (m_iLdIndex>=0 && IsNum)
        {
        CLadder & Ld=m_Plc[m_iPcIndex].m_Ladder[m_iLdIndex];
        Ld.m_Line1=SafeAtoL(m_sGoto);
        FixScrollBar();
        RedrawTheDisplay(Vw_Ladder);

  //    if (m_sGoto.GetLength()>0 && (m_sGoto[0]=='\'' ||m_sGoto[0]=='\"'))
  //      IsNum=false;
    
        }
      else
        {
        LPCTSTR p=(m_sGoto[0]=='\'' || m_sGoto[0]=='\"') ? &m_sGoto.GetBuffer(0)[1] : m_sGoto;
        CAddInfo *pAdd=m_Plc[m_iPcIndex].FindSymbol(p);
        if (!pAdd)
          pAdd=m_Plc[m_iPcIndex].FindAddress(p);
        if (pAdd)
          SetDisplaySymbol(m_iPcIndex, pAdd ? pAdd->m_AddInfoInx : -1);
        else
          {
          CString t=p;
          if (t[t.GetLength()-1]==':' || t[t.GetLength()-1]=='/')
            t=t.Left(t.GetLength()-1);
          for (int i=0; i<m_Plc[m_iPcIndex].m_nTables; i++)
            if (m_Plc[m_iPcIndex].m_Table[i] && 
                m_Plc[m_iPcIndex].m_Table[i]->m_sName.CompareNoCase(t)==0)
              break;
          if (i<m_Plc[m_iPcIndex].m_nTables)
            SetDisplayTable(m_iPcIndex, i);
          else
            SetDisplayTable(m_iPcIndex, -1);
          }
        RedrawTheDisplay(Vw_Table);
        }
      }
    }
  }

void CPLC5Dlg::OnSetfocusEditgoto()
  {
  UpdateData(TRUE);
//  if (m_iPcIndex>=0 && m_iLdIndex>=0)
//    {
//    CLadder & Ld=m_Plc.m_Ladder[m_iLdIndex];
//    Ld.m_Line1=atol(m_sGoto);
//    FixScrollBar();
//    RedrawTheDisplay(Vw_Ladder);
//    }
//  static DWORD X=0;
//  DWORD Y=GetTickCount();
//  if (abs(Y-X)>1000)
//    {
//    //m_sGoto="0";
//    //UpdateData(FALSE);
//    }
//	X=Y;
  }

void CPLC5Dlg::OnFindinladder() 
  {
  UpdateData(TRUE);
  if (m_iPcIndex >= 0 && (m_sGoto.GetLength()>0))
    {
    bool IsNum=true;
    for (int i=0; i<m_sGoto.GetLength(); i++)
      if (!isdigit(m_sGoto[i]))
        IsNum=false;
    if (!IsNum)
      {
      LPCTSTR p=(m_sGoto[0]=='\'' || m_sGoto[0]=='\"') ? &m_sGoto.GetBuffer(0)[1] : m_sGoto;
      CAddInfo *pAdd=m_Plc[m_iPcIndex].FindSymbol(p);
      if (!pAdd)
        pAdd=m_Plc[m_iPcIndex].FindAddress(p);
      if (pAdd)
        SetDisplaySymbol(m_iPcIndex, pAdd ? pAdd->m_AddInfoInx : -1);
      if (pAdd)
        {
        long l=m_iLdIndex;
        if (l>=0 && l<m_Plc[m_iPcIndex].m_nLadders)
          {
          CLadder &Ld=m_Plc[m_iPcIndex].m_Ladder[l];
          long l1=Ld.m_Line1;
          if (Ld.FindRungWithAddress(pAdd->m_sAddress)>=0)
            {
            m_LdScroll.SetScrollPos(Ld.m_FindRung);
            RedrawTheDisplay(Vw_Ladder);
            }
          }
        }
      }
    }
  }

void CPLC5Dlg::OnMouseMove(UINT nFlags, CPoint point) 
  {
	//TRACE("MM %i,%i\n",point.x,point.y);
  if (m_ToolTip.GetToolCount())
    {
    CToolInfo TI;
    if (m_ToolTip.HitTest(this, point, &TI))
      {
      CString S;
      switch (PointInClient(point))
        {
        case 1:
          if (TI.uId==IDC_LDRSTATIC)
            {
            CPoint CPt=PtToChar(point, m_LdShft, m_CharSz);
            CPoint RPt;
            CMtxElement * pE=TargetMtxElement(m_DispLst, point, CPt, RPt);
            S="";
            if (pE)
              {
              CAddress * pA=pE->TargetAddress(point, RPt);
              CLadder * pL=pE->TargetLadder(point, RPt);
              if (pA)
                {
                if (pA->IsFlt())
                  S.Format("%s=%g", pA->Text(), pA->FValue());
                else if (pA->IsBit())
                  S.Format("%s=%i", pA->Text(), pA->BValue());
                else
                  S.Format("%s=%i", pA->Text(), pA->IValue());
                }
              }
            m_ToolTip.UpdateTipText(S, this, IDC_LDRSTATIC);
            }
          break;
        case 2:
          {
          if (TI.uId==IDC_DATASTATIC)
            {
            CPoint CPt=PtToChar(point, m_LdShft, m_CharSz);
            CAddress * pA=TargetAddress(m_DispLst, point, CPt);
            if (pA)
              {
              if (pA->IsFlt())
                S.Format("%s=%g", pA->Text(), pA->FValue());
              else if (pA->IsBit())
                S.Format("%s=%i", pA->Text(), pA->BValue());
              else
                S.Format("%s=%i", pA->Text(), pA->IValue());
              }
            else
              S="";
            m_ToolTip.UpdateTipText(S, this, IDC_DATASTATIC);
            }
          break;
          }
        default:;
        }
//      Last=point;
      //m_ToopTip.UpdateTipText(
      m_ToolTip.Activate(TRUE);
      m_ToolTip.Update();
      }
    }
	CDialog::OnMouseMove(nFlags, point);
  }

BOOL CPLC5Dlg::PreTranslateMessage(MSG* pMsg) 
  {

  //In order for the tooltip control to be notified of important messages, such as WM_LBUTTONXXX messages, you must relay the messages to your tooltip control. The best method for this relay is to make a call to CToolTipCtrl::RelayEvent, in the PreTranslateMessage function of the owner window. The following example illustrates one possible method (assuming the tooltip control is called m_ToolTip):

  if(pMsg->message== WM_LBUTTONDOWN ||
          pMsg->message== WM_LBUTTONUP ||
          pMsg->message== WM_MOUSEMOVE)
    m_ToolTip.RelayEvent(pMsg);

	
	return CDialog::PreTranslateMessage(pMsg);
  }

afx_msg LRESULT CPLC5Dlg::OnInitLoad(WPARAM wParam, LPARAM lParam)
  {
  if (m_sInitFile.GetLength())
    Open(m_sInitFile);
  m_sInitFile="";
  return 0;
  }
//#include "..\MR_Common\OPCCmds.h"

//=========================================================================

int CPLC5Dlg::DoLoadSnapshot(LPCTSTR Fn)
  {
  CStopWatch SW;
  SW.Start();

  CString FnX(Fn);
  FnX.TrimRight("\\");
  long res=0;
  for (int i=0; res==0 && i<NPlcs(); i++)
    {
    CString FnY(FnX);
    FnY+="\\";
    FnY+=m_Plc[i].m_sPlcName;
    res=m_Plc[i].LoadSnapshot(FnY);
    }
  SW.Stop();
  double xx=SW.Secs();
  return res;
  }

//-------------------------------------------------------------------------

int CPLC5Dlg::DoSaveSnapshot(LPCTSTR Fn)
  {
  CStopWatch SW;
  SW.Start();

  CString FnX(Fn);
  FnX.TrimRight("\\");

  long res=FnCreatePath(FnX);
  if (res!=0)
    return res;

  for (int i=0; res==0 && i<NPlcs(); i++)
    {
    CString FnY(FnX);
    FnY+="\\";
    FnY+=m_Plc[i].m_sPlcName;
    res=m_Plc[i].SaveSnapshot(FnY);
    }
  SW.Stop();
  double xx=SW.Secs();
  return res;
  }

//-------------------------------------------------------------------------

int CPLC5Dlg::DoLoadBacktrack(LPCTSTR Fn)
  {
  CString FnX(Fn);
  FnX.TrimRight("\\");
  long res=0;
  for (int i=0; res==0 && i<NPlcs(); i++)
    {
    CString FnY(FnX);
    FnY+="\\";
    FnY+=m_Plc[i].m_sPlcName;
    res=m_Plc[i].LoadBacktrack(FnY);
    }
  return res;
  }

//-------------------------------------------------------------------------

int CPLC5Dlg::DoSaveBacktrack(LPCTSTR Fn)
  {
  CString FnX(Fn);
  FnX.TrimRight("\\");

  long res=FnCreatePath(FnX);
  if (res!=0)
    return res;

  for (int i=0; res==0 && i<NPlcs(); i++)
    {
    CString FnY(FnX);
    FnY+="\\";
    FnY+=m_Plc[i].m_sPlcName;
    res=m_Plc[i].SaveBacktrack(FnY);
    }
  return res;
  }

//=========================================================================

afx_msg LRESULT CPLC5Dlg::OnCOMCmd(WPARAM wParam, LPARAM lParam)
  {
  //for (DWORD i=0; i<wParam; i++)
  //  {
  //  :: Beep(4000,200);
  //  :: Sleep(200);
  //  }
  CString *pS;
  switch (wParam)
    {
    case ComCmd_StartDynamicSolve:
      OnRun();
      theApp.m_ScdCmdIF.DoEventMsg(ComState_StepDynamic,0);
      break;
    case ComCmd_Start:
      GotoRun();
      //m_ScdCmdIF.DoEventMsg(ComCmd_Start,0);
      break;
    case ComCmd_Step:
      OnStep();
      //m_ScdCmdIF.DoEventMsg(ComCmd_Start,0);
      break;
    case ComCmd_Stop:
      GotoStop();
      //theApp.m_ScdCmdIF.DoEventMsg(ComState_Stop,0);
      break;
    case ComCmd_Idle:
      GotoIdle();
      //theApp.m_ScdCmdIF.DoEventMsg(ComState_Idle,0);
      break;
    case ComCmd_Exit:
      CDialog::OnCancel();
//      OnCancel();
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_Exit,0);
      break;
    case ComCmd_LoadProject:
      pS=(CString*)lParam;
      if (m_Plc.GetSize()>0)
        Close();
      Open(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_LoadProject,0);
      break;
    case ComCmd_SaveProject:
      pS=(CString*)lParam;
      //theApp.Open(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_SaveProject,0);
      break;
    case ComCmd_CloseProject:
      Close();
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_CloseProject,0);
      break;

    case ComCmd_LoadSnapshot:
      {
      pS=(CString*)lParam;
      long res=DoLoadSnapshot(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_LoadSnapshot,res);
      break;
      }
    case ComCmd_SaveSnapshot:
      {
      pS=(CString*)lParam;
      long res=DoSaveSnapshot(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_SaveSnapshot,res);
      break;
      }
    case ComCmd_LoadScenario:
      {
      pS=(CString*)lParam;
      long res=DoLoadSnapshot(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_LoadScenario,res);
      break;
      }
    case ComCmd_SaveScenario:
      {
      pS=(CString*)lParam;
      long res=DoSaveSnapshot(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_SaveScenario,res);
      break;
      }
    case ComCmd_LoadBacktrack:
      {
      pS=(CString*)lParam;
      long res=DoLoadBacktrack(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_LoadBacktrack,res);
      break;
      }
    case ComCmd_SaveBacktrack:
      {
      pS=(CString*)lParam;
      long res=DoSaveBacktrack(*pS);
      delete pS;
      theApp.m_ScdCmdIF.DoEventMsg(ComCmd_SaveBacktrack,res);
      break;
      }
    case ComCmd_SetTag:
      {
      CScdCmdTag *pT=(CScdCmdTag *) lParam;

      CAddress A(&m_Plc[m_iPcIndex]);
      VARIANT &V=*pT->m_pvtValue;
      

      CAddInfo *pAI=m_Plc[m_iPcIndex].FindSymbol((LPTSTR)pT->m_pTag);
      if (pAI)
        A.Parse((LPTSTR)pAI->Address());
      else
        A.Parse((LPTSTR)pT->m_pTag);

      if (A.ValidAddress())
        {
        VARIANT R;
        ::VariantInit(&R);
        if (A.IsFlt())
          {
          if (SUCCEEDED(::VariantChangeType(&R, pT->m_pvtValue, 0, VT_R4)))
            A.SetFValue(R.fltVal);
          }
        else if (A.IsBit())
          {
          if (SUCCEEDED(::VariantChangeType(&R, pT->m_pvtValue, 0, VT_BOOL)))
            A.SetBValue(R.boolVal!=false);
          }
        else
          {
          if (SUCCEEDED(::VariantChangeType(&R, pT->m_pvtValue, 0, VT_I2)))
            A.SetIValue(R.iVal);
          }
        theApp.m_ScdCmdIF.DoEventMsg(ComCmd_SetTag,0);
        }
      else
        theApp.m_ScdCmdIF.DoEventMsg(ComCmd_SetTag,1);
      break;
      }
    case ComCmd_GetTag:
      {
      CScdCmdTag *pT=(CScdCmdTag *) lParam;

      CAddress A(&m_Plc[m_iPcIndex]);
      VARIANT &V=*pT->m_pvtValue;
      CAddInfo *pAI=m_Plc[m_iPcIndex].FindSymbol((LPTSTR)pT->m_pTag);
      if (pAI)
        A.Parse((LPTSTR)pAI->Address());
      else
        A.Parse((LPTSTR)pT->m_pTag);

      if (A.ValidAddress())
        {
        //VARIANT R;
        ::VariantClear(&V);
        if (A.IsFlt())
          {
          V.vt=VT_R4;
          V.fltVal=A.FValue();
          }
        else if (A.IsBit())
          {
          V.vt=VT_BOOL;
          V.boolVal=A.BValue();
          }
        else
          {
          V.vt=VT_I2;
          V.iVal=A.IValue();
          }
        theApp.m_ScdCmdIF.DoEventMsg(ComCmd_GetTag,0);
        }
      else
        theApp.m_ScdCmdIF.DoEventMsg(ComCmd_GetTag,1);
      break;
      }
    }
  
  return 0;
  }

//=========================================================================

int CPLC5Dlg::AddAPlc(int N, LPCTSTR FNRoot, LPCTSTR FNFile, LPCTSTR FnOptProfile)
  {
  char cDrv[_MAX_DRIVE];
  char cPath[_MAX_PATH];
  char cFile[_MAX_FNAME];
  char cExt[_MAX_EXT];
  _splitpath(FNFile, cDrv, cPath, cFile, cExt);
  CString sFNCode(FNRoot), sFNSym(FNRoot), sFNSymO(FNRoot);
  sFNCode+=cFile;
  sFNCode+=".PC5";
  sFNSym+=cFile;
  sFNSym+=".SY5";
  sFNSymO+=cFile;
  sFNSymO+=".txt";
  return m_Plc[N].Load(cFile, sFNCode, sFNSym, sFNSymO, FnOptProfile);
  }

//=========================================================================

void CPLC5Dlg::BuildPrj(LPCTSTR FNRoot, LPCTSTR FNPrj)
  {
  CString Prj(FNRoot);
  Prj+=FNPrj;

  char Key[32], Ret[1024];
  for (int N=0; N<32; N++)
    {
    sprintf(Key, "PLC%i", N);
    if (GetPrivateProfileString("Hardware", Key, "", Ret, sizeof(Ret), Prj)==0)
      break;
    }
  
  // Must preallocate to prevent pointers moving
  m_Plc.SetSize(N);  
  for (int p=0; p<N; p++)
    {
    sprintf(Key, "PLC%i", p);
    if (GetPrivateProfileString("Hardware", Key, "", Ret, sizeof(Ret), Prj)>0)
      {
      if (!AddAPlc(p, FNRoot, Ret, Prj))
        break;
      }
    else
      break;
    }
  
  for (p=0; p<N; p++)
    {
    char BigBuff[100000];
    long Len=GetPrivateProfileSection(m_Plc[p].m_sPlcName, BigBuff, sizeof(BigBuff), Prj);
    if (Len>0)
      {
      char * Str=BigBuff;
      for ( ; strlen(Str)>0; Str+=strlen(Str)+1) 
        {
        char *Key=Str;
        char *Value=strchr(Str, '=');
        if (Value)
          {
          *Value=0; // Kill '='
          Value++;
          if (stricmp(Key, "Transfer")==0)
            {
            char *pDstA;
            char *pSrcP;
            char *pSrcA;
            char *pLen;
            pDstA=Value;
            pSrcP=strchr(pDstA, ',');
            if (pSrcP==NULL)
              continue;
            *pSrcP=0;
            pSrcP++;
            pSrcA=strchr(pSrcP, '.');
            if (pSrcA==NULL)
              continue;
            *pSrcA=0;
            pSrcA++;
            pLen=strchr(pSrcA,',');
            if (pLen==NULL)
              continue;
            *pLen=0;
            pLen++;
      
            long Len=SafeAtoL(pLen);

            AddCommsBlk(m_Plc[p].m_sPlcName, pDstA, pSrcP, pSrcA, Len);
            }
          else if (stricmp(Key, "Force")==0)
            {
            float Values[1000];
            float *pValues=Values;
            char *pDstA;
            char *pVal;
            char *pLen;
            pDstA=Value;
            pLen=strchr(pDstA,',');
            if (pLen==NULL)
              continue;
            *pLen=0;
            pLen++;
      
            long Len=SafeAtoL(pLen);
            pVal=strchr(pLen, ',');
            long nVal=0;
            while (pVal)
              {
              pVal++;
              Values[nVal++]=float(SafeAtoF(pVal,0.0));
              pVal=strchr(pVal, ',');
              };

            for ( ;nVal<Len; nVal++)
              Values[nVal]=0;

            AddForceBlk(m_Plc[p].m_sPlcName, pDstA, Len, Values);
            }
          else if (stricmp(Key, "ChgOff")==0)
            {
            m_Plc[p].DumpAdressOn(Value, 0);
            }
          else if (stricmp(Key, "ChgOn")==0)
            {
            m_Plc[p].DumpAdressOn(Value, 1);
            }
          }
        }
      }

    /*
    for (int i=0; ; i++)
      {
      sprintf(Key, "Transfer%i", i);
      if (GetPrivateProfileString(m_Plc[p].m_sPlcName, Key, "", Ret, sizeof(Ret), Prj)>0)
        {
        char *pDstA;
        char *pSrcP;
        char *pSrcA;
        char *pLen;
        pDstA=Ret;
        pSrcP=strchr(pDstA, ',');
        if (pSrcP==NULL)
          continue;
        *pSrcP=0;
        pSrcP++;
        pSrcA=strchr(pSrcP, '.');
        if (pSrcA==NULL)
          continue;
        *pSrcA=0;
        pSrcA++;
        pLen=strchr(pSrcA,',');
        if (pLen==NULL)
          continue;
        *pLen=0;
        pLen++;
      
        long Len=SafeAtoL(pLen);

        AddCommsBlk(m_Plc[p].m_sPlcName, pDstA, pSrcP, pSrcA, Len);
        }
      else
        break;
      }
    
    for (i=0; ; i++)
      {
      sprintf(Key, "Force%i", i);
      if (GetPrivateProfileString(m_Plc[p].m_sPlcName, Key, "", Ret, sizeof(Ret), Prj)>0)
        {
        float Values[1000];
        float *pValues=Values;
        char *pDstA;
        char *pVal;
        char *pLen;
        pDstA=Ret;
        pLen=strchr(pDstA,',');
        if (pLen==NULL)
          continue;
        *pLen=0;
        pLen++;
      
        long Len=SafeAtoL(pLen);
        pVal=strchr(pLen, ',');
        long nVal=0;
        while (pVal)
          {
          pVal++;
          Values[nVal++]=float(SafeAtoF(pVal,0.0));
          pVal=strchr(pVal, ',');
          };

        for ( ;nVal<Len; nVal++)
          Values[nVal]=0;

        AddForceBlk(m_Plc[p].m_sPlcName, pDstA, Len, Values);
        }
      else
        break;
      }
      */


    }
  }
  
//=========================================================================

int CPLC5Dlg::Open(LPCTSTR FNRoot, LPCTSTR FNFile)
  {
  CWaitCursor Wait;
  char cExt[_MAX_EXT];
  _splitpath(FNFile, NULL, NULL, NULL, cExt);
 
  if (stricmp(cExt, ".ppj")==0)
    {
    BuildPrj(FNRoot, FNFile);
    }
  else
    {
    m_Plc.SetSize(1);
    AddAPlc(0, FNRoot, FNFile, NULL);
    }
  if (NPlcs()<=0)
    return false;

  for (int p=0; p<NPlcs(); p++)
    m_Plc[p].SaveData();

  SetUpDlg();
  OPCStart();

  return true;
  };

//=========================================================================

int CPLC5Dlg::Open(LPCTSTR FNFullFile)
  {
  char cDrv[_MAX_DRIVE];
  char cPath[_MAX_PATH];
  char cFile[_MAX_FNAME];
  char cExt[_MAX_EXT];
  _splitpath(FNFullFile, cDrv, cPath, cFile, cExt);

  CString FNRoot, FNFile;
  FNRoot=cDrv;
  FNRoot+=cPath;
  FNFile=cFile;
  FNFile+=cExt;

  return Open(FNRoot, FNFile);
  };

//=========================================================================

int CPLC5Dlg::Close()
  {
  OPCStop();

  m_Plc.SetSize(0);
  
  SetUpDlg();
  UpdateData(FALSE);

  return 0;
  };

//=========================================================================

void CPLC5Dlg::AddCommsBlk(LPCTSTR DstP, LPCTSTR DstA, LPCTSTR SrcP, LPCTSTR SrcA, long Len)
  {
  int N=m_Xfer.GetSize();
  m_Xfer.SetSize(N+1, 16);
  

  m_Xfer[N]=new CTransferBlk;
  CTransferBlk &I=*m_Xfer[N];
  
  I.m_sDstP=DstP;
  I.m_sDstA=DstA;
  I.m_sSrcP=SrcP;
  I.m_sSrcA=SrcA;
  I.m_nLen=Len;

  I.m_pSrc=NULL;
  I.m_pDst=NULL;

  I.m_bOK=false;

  for (int p=0; p<NPlcs(); p++)
    {
    if (I.m_sSrcP.CompareNoCase(m_Plc[p].m_sPlcName)==0)
      {
      CAddress *pA=new CAddress(&m_Plc[p], I.m_sSrcA);
      if (pA->m_IsValid)
        I.m_pSrc=pA;
      else
        delete pA;
      }
    if (I.m_sDstP.CompareNoCase(m_Plc[p].m_sPlcName)==0)
      {
      CAddress *pA=new CAddress(&m_Plc[p], I.m_sDstA);
      if (pA->m_IsValid)
        I.m_pDst=pA;
      else
        delete pA;
      }
    }
 
  if (I.m_pSrc && I.m_pDst)
    {
    if (I.m_pSrc->IsFlt() && I.m_pDst->IsFlt())
      {
      I.m_bOK=true;
      }
    else if (!I.m_pSrc->IsBit() && !I.m_pDst->IsBit())
      {
      I.m_bOK=true;
      }
    }
  } 

//=========================================================================

void CPLC5Dlg::AddForceBlk(LPCTSTR DstP, LPCTSTR DstA, long Len, float * Values)
  {
  int N=m_Force.GetSize();
  m_Force.SetSize(N+1, 16);
  
  m_Force[N]=new CForceBlk;
  CForceBlk &I=*m_Force[N];
  
  I.m_sDstP=DstP;
  I.m_sDstA=DstA;
  I.m_nLen=Len;

  I.m_pDst=NULL;
  
  I.m_bOK=false;

  for (int p=0; p<NPlcs(); p++)
    {
    if (I.m_sDstP.CompareNoCase(m_Plc[p].m_sPlcName)==0)
      {
      CAddress *pA=new CAddress(&m_Plc[p], I.m_sDstA);
      if (pA->m_IsValid)
        I.m_pDst=pA;
      else
        delete pA;
      }
    }

  I.m_Values.SetSize(Len);
  for (int i=0; i<Len; i++)
    I.m_Values[i]=Values[i];
 
  if (I.m_pDst)
    if (I.m_pDst->IsFlt() || !I.m_pDst->IsBit())
      I.m_bOK=true;
  } 

//=========================================================================

BOOL CPLC5Dlg::OPCStart()
  {
  if (m_OPCServerStarted)//FSServerInUse())
    return true;

//  if( !StartFSServer(theApp.m_hInstance, &CLSID_OPCServer) )
  if( !StartFSServer(theApp.m_hInstance, &CLSID_OPCServer,true) )
    return FALSE;
  
  if (m_MyOPCCallBack==NULL)
    m_MyOPCCallBack=new CPLC5OPCCallBack(&theApp, m_Plc);
  SetCallbackObject(m_MyOPCCallBack);

  m_OPCServerStarted=true;
  return TRUE;//StartFSServer(AfxGetInstanceHandle(), &CLSID_OPCServer);
  };
void CPLC5Dlg::OPCStop()
  {
  if (!m_OPCServerStarted)//FSServerInUse())
    return;
   
  m_OPCServerStarted=false;
    //SetCallbackObject(NULL);
  StopFSServer();
  //delete m_MyOPCCallBack;
  //m_MyOPCCallBack=NULL;
  };
BOOL CPLC5Dlg::OPCRegister()
  {
  //HRESULT hr=RegisterServer();
  HRESULT hr=RegisterServer("PLC5.exe");
  if (FAILED(hr) )
    AfxMessageBox(_T("OPC RegisterServer Failed"));
  return SUCCEEDED(hr);
  };
BOOL CPLC5Dlg::OPCUnregister()
  {
  HRESULT hr=UnregisterServer();
  if (FAILED(hr))
    AfxMessageBox(_T("OPC UnregisterServer Failed"));
  return SUCCEEDED(hr);
  };

//=========================================================================

void CPLC5Dlg::OnGotorun() 
  {
  GotoRun();	
  }

void CPLC5Dlg::OnGotostop() 
  {
	GotoStop();
  }

void CPLC5Dlg::OnGotoidle() 
  {
  GotoIdle();	
  }

void CPLC5Dlg::OnLoadsnap() 
  {
  CString Fn;
  Fn = "C:\\Documents and Settings\\Best\\Application Data\\BeST\\OTS\\SnapShots\\";
  Fn += "TheSnap\\ESD";
	DoLoadSnapshot(Fn);
  }

void CPLC5Dlg::OnSavesnap() 
  {
  CString Fn;
  Fn = "C:\\Documents and Settings\\Best\\Application Data\\BeST\\OTS\\SnapShots\\";
  Fn += "TheSnap\\ESD";
	DoSaveSnapshot(Fn);
  }

void CPLC5Dlg::OnLoadback() 
  {
  CString Fn;
  Fn = "C:\\Documents and Settings\\Best\\Application Data\\BeST\\OTS\\Backtracks\\";
  Fn += "TheBack\\ESD";
	DoLoadBacktrack(Fn);
  }

void CPLC5Dlg::OnSaveback() 
  {
  CString Fn;
  Fn = "C:\\Documents and Settings\\Best\\Application Data\\BeST\\OTS\\Backtracks\\";
  Fn += "TheBack\\ESD";
	DoSaveBacktrack(Fn);
  }

//=========================================================================

//---------------------------------------------------------------------------

CString FnDrivePath(LPCTSTR FilePath)
  {
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  _splitpath(FilePath, Drv, Dir, NULL, NULL);
  CString S;
  S.Format("%s%s", Drv, Dir);
  return S;
  }

//---------------------------------------------------------------------------

CString FnNameExt(LPCTSTR FilePath)
  {
  char Name[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(FilePath, NULL, NULL, Name, Ext);
  CString S;
  S.Format("%s%s", Name, Ext);
  return S;
  }

//---------------------------------------------------------------------------

CString FnDrivePathName(LPCTSTR FilePath)
  {
  char Drv[_MAX_DRIVE];
  char Dir[_MAX_DIR];
  char Name[_MAX_FNAME];
  _splitpath(FilePath, Drv, Dir, Name, NULL);
  CString S;
  S.Format("%s%s%s", Drv, Dir, Name);
  return S;
  }

//---------------------------------------------------------------------------

CString FnPathNameExt(LPCTSTR FilePath)
  {
  char Dir[_MAX_DIR];
  char Name[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(FilePath, NULL, Dir, Name, Ext);
  CString S;
  S.Format("%s%s%s", Dir, Name, Ext);
  return S;
  }


int FileExists(LPCTSTR FileName)
  {
  WIN32_FIND_DATA fd;
  HANDLE H;
  if ((H=FindFirstFile(FileName, &fd))!=INVALID_HANDLE_VALUE)
    {
    FindClose(H);
    return true;
    }
  return false;
  };


int FnCreatePath(LPCTSTR pPath)
  {
  CString Path(pPath), Tmp;
  CStringList Paths;
  Path.TrimRight("\\");
  while (Path.GetLength()>0 && !FileExists(Path))
    {
    Paths.AddTail(Path);
    Path=FnDrivePath(Path);
    Path.TrimRight("\\");
    }

  if (Path.GetLength()==0)
    return false;
  
  for(POSITION Pos = Paths.GetTailPosition(); Pos != NULL; )
    {
    CString S=Paths.GetPrev(Pos);
    if (!CreateDirectory(S, NULL))
      return 10;
    }
  return 0;
  }

//=========================================================================

void CPLC5Dlg::OnChangeMonitor() 
  {
  UpdateData(TRUE);
  FixScrollBar();

  GetDlgItem(IDC_RADIODEC)->EnableWindow(!m_ChgMonitorOn);
  GetDlgItem(IDC_RADIOHEX)->EnableWindow(!m_ChgMonitorOn);

  RedrawTheDisplay(Vw_Table, RDW_INVALIDATE | RDW_UPDATENOW);
	}
