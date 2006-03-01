//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __FILEDLGS_CPP
#include "sc_defs.h"
#include "syscad.h"
//#include "resource.h"
//#include "project.h"
//#include "mainfrm.h"
//#include "accnode.h"
//#include "tagvdoc.h"
//#include "tagvdlgs.h"
//#include "grfdoc.h"
//#include "toolbars.h"
//#include "helpinfo.h"
#include "filedlgs.h"
//#include "cmd_mngr.h"
//#include "scd_wm.h"
//#include "dbgmngr.h"
//#include "msgwnd.h"
//#include "scdver.h"

//extern "C" 
//  {
//  #include "grldefs.h"
//  }

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//===========================================================================
// CSCDFileNewDlg dialog

CSCDFileNewDlg::CSCDFileNewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCDFileNewDlg::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CSCDFileNewDlg)
	//}}AFX_DATA_INIT
  m_pReqdTemplate=NULL;
  }


void CSCDFileNewDlg::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCDFileNewDlg)
	DDX_Control(pDX, IDC_NEWCTRLTREND, m_NewCtrlTrend);
	DDX_Control(pDX, IDC_NEWTREND, m_NewTrend);
	DDX_Control(pDX, IDC_NEWVISIO, m_NewVisio);
	DDX_Control(pDX, IDC_NEWGRAPHICS, m_NewGraphics);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSCDFileNewDlg, CDialog)
	//{{AFX_MSG_MAP(CSCDFileNewDlg)
	ON_BN_CLICKED(IDC_NEWGRAPHICS, OnNewgraphics)
	ON_BN_CLICKED(IDC_NEWTREND, OnNewtrend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCDFileNewDlg message handlers

void CSCDFileNewDlg::OnNewgraphics() 
  {
	// TODO: Add your control notification handler code here
  SetEnables();
	
  }

void CSCDFileNewDlg::OnNewtrend() 
  {
	// TODO: Add your control notification handler code here
  SetEnables();
	
  }

BOOL CSCDFileNewDlg::OnInitDialog() 
  {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
#if (!WITHVISIODOC)
  m_NewVisio.ShowWindow(SW_HIDE);
#endif

  CProfINIFile PF(PrjFile());
  m_NewGraphics.SetCheck(PF.RdInt("FileDlgs", "NewGraph", 1));
  m_NewTrend.SetCheck(m_NewGraphics.GetCheck() ? 0 : 1);
  m_NewVisio.SetCheck(PF.RdInt("FileDlgs", "NewVisio", 1));
  m_NewCtrlTrend.SetCheck(0);
  SetEnables();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

void CSCDFileNewDlg::SetEnables()
  {
  m_NewVisio.EnableWindow(m_NewGraphics.GetCheck()!=0);
  m_NewCtrlTrend.EnableWindow(m_NewTrend.GetCheck()!=0);
  };

void CSCDFileNewDlg::OnOK() 
  {
	// TODO: Add extra validation here

  if (m_NewGraphics.GetCheck())
    {
#if WITHVISIODOC
    m_pReqdTemplate=m_NewVisio.GetCheck() ? &ScdApp()->VisioTemplate() : &ScdApp()->GraphTemplate();
#else
    m_pReqdTemplate=&ScdApp()->GraphTemplate();
#endif
    }
  else if (m_NewTrend.GetCheck())
    {
    m_pReqdTemplate=m_NewCtrlTrend.GetCheck() ? &ScdApp()->ControlTemplate() : &ScdApp()->TrendTemplate();
    }

  CProfINIFile PF(PrjFile());
  PF.WrInt("FileDlgs", "NewGraph", m_NewGraphics.GetCheck());
  PF.WrInt("FileDlgs", "NewVisio", m_NewVisio.GetCheck());
	
	CDialog::OnOK();
  }

//===========================================================================
// CNewGrfDocOptions dialog

CNewGrfDocOptions::CNewGrfDocOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CNewGrfDocOptions::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CNewGrfDocOptions)
	m_ExistingFile = FALSE;
	//}}AFX_DATA_INIT
    
  PF.SetProfFilename(PrjFile());
  }


void CNewGrfDocOptions::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewGrfDocOptions)
	DDX_Control(pDX, IDC_EDITNEWFILE, m_NewFileCtrl);
	DDX_Control(pDX, IDC_EDITINITFILE, m_InitFileCtrl);
	DDX_Check(pDX, IDC_EXISTINGFILE, m_ExistingFile);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
    {
    CString S;
    m_NewFileCtrl.GetWindowText(S);
    m_NewFile=S;
    m_NewFile.FnExpand();
    m_InitFileCtrl.GetWindowText(S);
    m_InitFile=S;
    m_InitFile.FnExpand();
    }
  else
    {
    Strng S;
    S.FnContract(m_NewFile());
    m_NewFileCtrl.SetWindowText(S());
    S.FnContract(m_InitFile());
    m_InitFileCtrl.SetWindowText(S());
    }
  }


BEGIN_MESSAGE_MAP(CNewGrfDocOptions, CDialog)
	//{{AFX_MSG_MAP(CNewGrfDocOptions)
	ON_BN_CLICKED(IDB_BROWSEINIFILE, OnBrowseinifile)
	ON_BN_CLICKED(IDB_BROWSENEWFILE, OnBrowsenewfile)
	ON_EN_CHANGE(IDC_EDITINITFILE, OnChangeEditinitfile)
	ON_EN_CHANGE(IDC_EDITNEWFILE, OnChangeEditnewfile)
	ON_BN_CLICKED(IDC_EXISTINGFILE, OnExistingfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewGrfDocOptions message handlers

void CNewGrfDocOptions::OnOK() 
  {
	// TODO: Add extra validation here
  UpdateData(TRUE);

  if (m_ExistingFile)
    {
	  if (!FileExists(m_NewFile()))
      {
      Strng F;
      F.Set("File not found:\n%s",m_NewFile());
      AfxMessageBox(F(), MB_ICONEXCLAMATION|MB_OK);
      return;
      }
    }
  else
    {
	  if (FileExists(m_NewFile()))
      {
      Strng F;
      F.Set("Overwrite %s", m_NewFile());
      if (IDNO==AfxMessageBox(F(), MB_ICONQUESTION|MB_YESNO))
        return;
      }
	  if (!FileExists(m_InitFile()))
      {
      Strng F;
      F.Set("Default file missing:\n%s", m_InitFile());
      AfxMessageBox(F(), MB_ICONEXCLAMATION|MB_OK);
      return;
      }

    if (!CopyFile(m_InitFile(), m_NewFile(), FALSE))
      {
      Strng E;
      E.GetSystemErrorStr(GetLastError());
      Strng F;
      F.Set("File not copied :\n%s\n%s\n%s",m_InitFile(),m_NewFile(),E());
      AfxMessageBox(F(), MB_ICONEXCLAMATION|MB_OK);
      return;
      }
    }

  PF.WrStr("FileDlgs", "VisioDefault", m_InitFile());

  CDialog::OnOK();
  }

BOOL CNewGrfDocOptions::OnInitDialog() 
  {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  m_InitFile=PF.RdStr("FileDlgs", "VisioDefault", "");
  if (m_InitFile.Length()<1)
    {
    m_InitFile=BaseCfgFiles();
    m_InitFile+="Visio\\Default Page.vsd";
    }

  UpdateData(FALSE);
  UpdateEnables();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

void CNewGrfDocOptions::OnBrowseinifile() 
  {
  Strng Path, X;

  Path=PF.RdStr("FileDlgs", "VisioDefaultPath", "");
  if (Path.Length()<=0)
    Path.FnDrivePath(m_InitFile());

  X.Set("(*.vsd)|*.vsd|Visio Drawings (*.vsd)|*.vsd||");
  CSCDFileDialog Dlg(true, "*.vsd", m_InitFile(), OFN_HIDEREADONLY | OFN_SHAREAWARE, X());
  Dlg.m_ofn.lpstrInitialDir = Path();
  Dlg.m_ofn.lpstrTitle = "Browse";

  if (Dlg.DoModal()==IDOK)
    {
    m_InitFile=Dlg.GetPathName();
    Path.FnDrivePath(m_InitFile());
    PF.WrStr("FileDlgs", "VisioDefaultPath", Path());
    }

  UpdateData(FALSE);
  }

void CNewGrfDocOptions::OnBrowsenewfile() 
  {
  Strng Path, X;

  Path=PF.RdStr("FileDlgs", "VisioNewPath", "");
  if (Path.Length()<=0)
    Path.FnDrivePath(m_NewFile());

  X.Set("(*.vsd)|*.vsd|Visio Drawings (*.vsd)|*.vsd||");
  CSCDFileDialog Dlg(true, "*.vsd", m_NewFile(), OFN_HIDEREADONLY | OFN_SHAREAWARE, X());
  Dlg.m_ofn.lpstrInitialDir = Path();
  Dlg.m_ofn.lpstrTitle = "Browse";

  if (Dlg.DoModal()==IDOK)
    {
    m_NewFile=Dlg.GetPathName();
    Path.FnDrivePath(m_NewFile());
    PF.WrStr("FileDlgs", "VisioNewPath", Path());
    }

  UpdateData(FALSE);
  UpdateEnables();
  }

void CNewGrfDocOptions::OnChangeEditinitfile() 
  {
  }

void CNewGrfDocOptions::OnChangeEditnewfile() 
  {
  UpdateEnables();
  }

void CNewGrfDocOptions::UpdateEnables() 
  {
  UpdateData(TRUE);
//	BOOL Exists=FileExists((char*)(LPCTSTR)m_NewFile);
	GetDlgItem(IDC_EDITINITFILE)->EnableWindow(!m_ExistingFile);
	GetDlgItem(IDB_BROWSEINIFILE)->EnableWindow(!m_ExistingFile);
  }

void CNewGrfDocOptions::OnExistingfile() 
  {
	UpdateEnables();
  }

//===========================================================================
