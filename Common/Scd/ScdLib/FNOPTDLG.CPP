// FnOptDlg.cpp : implementation file
//
                
#include "stdafx.h"
//#include "resource.h"
#include "FnOptDlg.h"
#include "gpfuncs.h"
#include "errorlog.h"
//#include "fndlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
                             
/////////////////////////////////////////////////////////////////////////////
// CFnOptDlg dialog
   
int CFnOptDlg::m_AllOK=false;  
int CFnOptDlg::m_SkipDlg=false;

CFnOptDlg::CFnOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFnOptDlg::IDD, pParent)
  {
	
  m_FullFileRmt = _T("");
	m_FullFileLcl = _T("");

  OkRmt=false;
  OkLcl=false;
  OldRmt=false;
  OldLcl=false;

  //{{AFX_DATA_INIT(CFnOptDlg)
	m_FileRmt = _T("");
	m_FileLcl = _T("");
	m_TimeRmt = _T("");
	m_TimeLcl = _T("");
	m_Note = _T("");
	//}}AFX_DATA_INIT

  m_UseRmt = -1;
	m_UseLclDefault = 1;
	m_UseRmtDefault = 0;
  }


void CFnOptDlg::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
  if (!pDX->m_bSaveAndValidate)
    {
    Strng t;
    t.FnContract((char*)(const char*)m_FileRmt);
    m_FileRmt=t();
    t.FnContract((char*)(const char*)m_FileLcl);
    m_FileLcl=t();

    }
	//{{AFX_DATA_MAP(CFnOptDlg)
	DDX_Text(pDX, IDC_FILE1, m_FileRmt);
	DDX_Text(pDX, IDC_FILE2, m_FileLcl);
	DDX_Text(pDX, IDC_TIME1, m_TimeRmt);
	DDX_Text(pDX, IDC_TIME2, m_TimeLcl);
	DDX_Text(pDX, IDC_NOTE, m_Note);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CFnOptDlg, CDialog)
	//{{AFX_MSG_MAP(CFnOptDlg)
	ON_BN_CLICKED(IDOKALL, OnOkall)
	ON_BN_CLICKED(IDC_USELCL, OnUselcl)
	ON_BN_CLICKED(IDC_USENEWEST, OnUsenewest)
	ON_BN_CLICKED(IDC_REASON1, OnReason1)
	ON_BN_CLICKED(IDC_REASON2, OnReason2)
	ON_BN_CLICKED(IDC_USERMT, OnUsermt)
	ON_BN_CLICKED(IDC_BROWSEFILE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFnOptDlg message handlers

void CFnOptDlg::OnOK() 
  {
	// TODO: Add extra validation here

  SetFileUse(m_UseRmt);

  ScdPFUser.WrInt("LoadOptions", "UseLclDefault", m_UseLclDefault);
  ScdPFUser.WrInt("LoadOptions", "UseRmtDefault", m_UseRmtDefault);

	CDialog::OnOK();
  }

void CFnOptDlg::OnOkall() 
  {
	// TODO: Add your control notification handler code here
  m_AllOK=true;
	
	OnOK();
  }

void CFnOptDlg::OnCancel() 
  {
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
  }

BOOL CFnOptDlg::OnInitDialog() 
  {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

  m_HasNote=(m_TheNote.GetLength()>0);

  m_UseLclDefault=ScdPFUser.RdInt("LoadOptions", "UseLclDefault", 1);
  m_UseRmtDefault=ScdPFUser.RdInt("LoadOptions", "UseRmtDefault", 0);

  if (UpdateInfo() && !m_HasNote)
    {
    SetFileUse(true);
    EndDialog(IDOK);
    return FALSE;
    }

  SetReason();
  UpdateData(FALSE);
  UpdateRadios();
  
  if (m_SkipDlg || (m_AllOK && !m_HasNote))
    {
    SetFileUse(m_UseRmt);
    EndDialog(IDOK);
    return FALSE;
    }

  GetDlgItem(IDC_SEENOTE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_BROWSEFILE)->EnableWindow(!OkRmt);
  GetDlgItem(IDC_BROWSEFILE)->ShowWindow(!OkRmt ? SW_SHOW : SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

void CFnOptDlg::UpdateRadios() 
  {
  CheckRadioButton(IDC_REASON1, IDC_REASON2, m_UseRmt ? IDC_REASON1 : IDC_REASON2);
  CheckRadioButton(IDC_USELCL, IDC_USERMT, 
    m_UseLclDefault ? IDC_USELCL : m_UseRmtDefault ? IDC_USERMT : IDC_USENEWEST);
  }

void CFnOptDlg::SetFileUse(int Rmt) 
  {                       
  CString *p=NULL;
  if (Rmt)
    {
	  m_FullFileUse=m_FullFileRmt;
    p=&m_FileRmt;
    }
  else
    {
    m_FullFileUse=m_FullFileLcl;
    p=&m_FileLcl;
    }

  LogNote("SysCAD", 0, "Opened %s", (char*)(const char*)*p);
  }

void CFnOptDlg::SetReason() 
  {
  if (OkRmt && OkLcl)
    {
    flag Same= !OldRmt && !OldLcl;
    m_UseRmt= m_UseRmtDefault || !m_UseLclDefault && (Same || !OldRmt && OldLcl);
    }
  else
    {
    m_UseRmt=OkRmt;
    }
  }

void CFnOptDlg::OnUselcl() 
  {
	// TODO: Add your control notification handler code here
  m_UseLclDefault=1;
  m_UseRmtDefault=0;
  SetReason();
  UpdateRadios();
  }

void CFnOptDlg::OnUsenewest() 
  {
	// TODO: Add your control notification handler code here
  m_UseLclDefault=0;
  m_UseRmtDefault=0;
  SetReason();
  UpdateRadios();
  }

void CFnOptDlg::OnUsermt() 
  {
	// TODO: Add your control notification handler code here
  m_UseLclDefault=0;
  m_UseRmtDefault=1;
  SetReason();
  UpdateRadios();
  }

void CFnOptDlg::OnReason1() 
  {
	// TODO: Add your control notification handler code here
  m_UseRmt=1;
  UpdateRadios();
  }

void CFnOptDlg::OnReason2() 
  {
	// TODO: Add your control notification handler code here
  m_UseRmt=0;
  UpdateRadios();
  }

void CFnOptDlg::OnBrowse() 
  {
	// TODO: Add your control notification handler code here
  Strng s, Path, Ext, Extn("*"), X;
  Path.FnDrivePath((char*)(const char*)m_FullFileRmt);
  Ext.FnExt((char*)(const char*)m_FullFileRmt);
  Extn.Set("*%s", Ext());
  X.Set("(*%s)|*%s|All Files (*.*)|*.*||", Ext(), Ext());
  CSCDFileDialog Dlg(true, NULL, Extn(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, X());
  Dlg.m_ofn.lpstrInitialDir = Path();
  Dlg.m_ofn.lpstrTitle = "Browse";

  if (Dlg.DoModal()==IDOK)
    m_FullFileRmt=Dlg.GetPathName();
  m_FileRmt=m_FullFileRmt;
 
  UpdateInfo();
  SetReason();
  UpdateData(FALSE);
  UpdateRadios();
  }


bool CFnOptDlg::UpdateInfo() 
  {
  FILETIME FtRmt, FtLcl;
  if ((OkRmt=FnModifyTime((char*)(const char*)m_FullFileRmt, FtRmt)))
    {
    CTime time(FtRmt);
    m_TimeRmt=time.Format("%#c");
    }
  if ((OkLcl=FnModifyTime((char*)(const char*)m_FullFileLcl, FtLcl)))
    {
    CTime time(FtLcl);
    m_TimeLcl=time.Format("%#c");
    }

  m_Note="";
  bool Same=false;
  Strng TxtRmt, TxtLcl;
  if (OkRmt && OkLcl)
    {
    OldRmt=(CompareFileTime(&FtRmt, &FtLcl)<0);
    OldLcl=(CompareFileTime(&FtLcl, &FtRmt)<0);
    Same = !OldRmt && !OldLcl;
    TxtRmt=Same ? "Same" : OldRmt ? "Old" : "New";
    TxtLcl=Same ? "Same" : OldLcl ? "Old" : "New";
    if (Same)
      m_Note+="Files the same";
    else if (OldLcl)
      m_Note+="Original file has changed";
    else 
      m_Note+="Original file is old";
    }
  else
    {
    TxtRmt=OkRmt ? "Exists" : "Missing";
    TxtLcl=OkLcl ? "Exists" : "Missing";
    if (!OkRmt)
      m_Note+="Original file Missing ";
    if (!OkLcl)
      m_Note+="Backup file Missing";
    }

  if (m_HasNote)
    {
    m_Note += "  (Note:";
    m_Note += m_TheNote;
    m_Note += ")";
    }

  GetDlgItem(IDC_REASON1)->SetWindowText(TxtRmt());
  GetDlgItem(IDC_REASON2)->SetWindowText(TxtLcl());

  GetDlgItem(IDC_REASON1)->SetWindowText(TxtRmt());
  GetDlgItem(IDC_REASON2)->SetWindowText(TxtLcl());

  return Same;
  }