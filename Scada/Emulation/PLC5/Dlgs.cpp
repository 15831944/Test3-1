// Dlgs.cpp : implementation file
//

#include "stdafx.h"
#include "plc5.h"
#include "Dlgs.h"
#include "support.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog


COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenDlg)
	m_PrjPath = _T("");
	m_DbgOn = FALSE;
	//}}AFX_DATA_INIT
}


void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenDlg)
	DDX_Text(pDX, IDC_PRJFOLDER, m_PrjPath);
	DDX_Check(pDX, IDC_OPENDBGFILE, m_DbgOn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
	//{{AFX_MSG_MAP(COpenDlg)
	ON_BN_CLICKED(IDBROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDlg message handlers

void COpenDlg::OnBrowse() 
  {

  char Drv[_MAX_DRIVE];
  char Path[_MAX_PATH];
  char File[_MAX_FNAME];
  char Ext[_MAX_EXT];
  _splitpath(m_PrjPath, Drv, Path, File, Ext);
  CString sFNRoot=Drv;
  sFNRoot+=Path;
  CString sFNFile=File;
  sFNFile+=Ext;

  CSCDFileDialog Dlg(true, NULL, sFNFile,//m_PrjPath,//"*.PPJ;*.PC5",//m_PrjFolder,//Ext(), 
                     OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, 
                     "PLC Project (*.PPJ)|*.PPJ|PLC Ladder (*.PC5)|*.PC5||");
//  Dlg.m_ofn.lpstrInitialDir = sFNRoot;
  Dlg.m_ofn.lpstrTitle = "PLC5 Project";
  HWND H=::GetActiveWindow();
  if (Dlg.DoModal()==IDOK)
    {
    m_PrjPath=Dlg.GetPathName();
    UpdateData(FALSE);
    }
  }
