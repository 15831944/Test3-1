// DLLLicenseDialog.cpp : implementation file
//

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include "scribble.h"
#include "LinkSMDK.h"
#include "DLLLicenseDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//---------------------------------------------------------------------------

CDLLLicenseDialog::CDLLLicenseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDLLLicenseDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLLLicenseDialog)
	m_DllName = _T("");
	m_DllStatus = _T("");
	//}}AFX_DATA_INIT
}

//---------------------------------------------------------------------------

void CDLLLicenseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLLLicenseDialog)
	DDX_Text(pDX, IDC_EDIT_DLL_NAME, m_DllName);
	DDX_Text(pDX, IDC_DLL_STATUS, m_DllStatus);
	//}}AFX_DATA_MAP
}

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDLLLicenseDialog, CDialog)
	//{{AFX_MSG_MAP(CDLLLicenseDialog)
	ON_BN_CLICKED(IDC_BUTTON_DLL_BROWSE, OnButtonDllBrowse)
	ON_BN_CLICKED(IDC_BUTTON_DLL_LICENSE, OnButtonDllLicense)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CDLLLicenseDialog::OnButtonDllBrowse() 
  {
  char Buff[128];
  sprintf(Buff, "DLL files (*.dll)|*.dll|All Files (*.*)|*.*||");
  CFileDialog Dlg(TRUE, NULL, "*.dll", OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
  Dlg.m_ofn.lpstrInitialDir = "C:\\SysCAD90\\bin";
  sprintf(Buff, "Find file or DLL to License");
  Dlg.m_ofn.lpstrTitle = Buff;

  if (Dlg.DoModal()==IDOK)
    {
    m_DllName = Dlg.GetPathName();
    // Check to see if the DLL is already licensed
    // and update the information on whether it is licensed or not
    if ( ReadScribble(m_DllName) )
      m_DllStatus = "File or DLL is Licensed";
    else
      m_DllStatus = "File or DLL is not Licensed";
    UpdateData(false);
    }
  }

//---------------------------------------------------------------------------

void CDLLLicenseDialog::OnButtonDllLicense() 
{
  // Here we add the scribble to the specified DLL
  // If it is not already licensed

  if (m_DllName == "" ) return;

  if ( ReadScribble(m_DllName) )
  {
    // Replace License
    if ( AddScribble(m_DllName,true) )
    {
      m_DllStatus = "File or DLL license replaced";
      MessageBox( m_DllName , "File or DLL license replaced", MB_ICONINFORMATION );
    }
    else
    {
      m_DllStatus = "File or DLL is not Licensed";
      MessageBox( m_DllName , "Already Licensed but failed to replace", MB_ICONINFORMATION );
    }
  }
  else
  {
    // Add new license
    if ( AddScribble(m_DllName,false) )
      m_DllStatus = "File or DLL is Licensed";
    else
      m_DllStatus = "File or DLL is not Licensed";

  }

  UpdateData(false);
}

//---------------------------------------------------------------------------

void CDLLLicenseDialog::OnOK() 
{
  OnButtonDllLicense();

	CDialog::OnOK();
}

//---------------------------------------------------------------------------

inline BOOL Valid(float a) { return !_isnan(a); };
inline BOOL Valid(double a) { return !_isnan(a); };

//===========================================================================

//===========================================================================
//===========================================================================

CProfINIFile::CProfINIFile(char* pFilename)
  {
  sFilename = pFilename;
  }

//---------------------------------------------------------------------------

CProfINIFile::CProfINIFile(char* pDir, char* pFilename)
  {
  sFilename = pDir;
  sFilename += pFilename;
  }

//---------------------------------------------------------------------------

CProfINIFile::~CProfINIFile()
  {
  }

//---------------------------------------------------------------------------

double CProfINIFile::RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault)
  {
  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), (char*)(const char*)sFilename);
  if (Len==0)
    return dDefault;
  //return SafeAtoF(Buff, dDefault);
  return atof(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue)
  {
  if (Valid(dValue))
    {
    char Buff[256];
    sprintf(Buff, "%.20g", dValue);
    WritePrivateProfileString(lpszSection, lpszEntry, Buff, (char*)(const char*)sFilename);
    }
  else
    WritePrivateProfileString(lpszSection, lpszEntry, "*", (char*)(const char*)sFilename);
  }

//---------------------------------------------------------------------------

long CProfINIFile::RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault)
  {
  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), (char*)(const char*)sFilename);
  if (Len==0)
    return nDefault;
  return atol(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue)
  {
  char Buff[256];
  sprintf(Buff, "%li", nValue);
  WritePrivateProfileString(lpszSection, lpszEntry, Buff, (char*)(const char*)sFilename);
  }

//---------------------------------------------------------------------------

int CProfINIFile::RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
  {
  char Buff[256];
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), (char*)(const char*)sFilename);
  if (Len==0)
    return nDefault;
  return atoi(Buff);
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
  {
  char Buff[256];
  sprintf(Buff, "%i", nValue);
  WritePrivateProfileString(lpszSection, lpszEntry, Buff, (char*)(const char*)sFilename);
  }

//---------------------------------------------------------------------------

CString &CProfINIFile::RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pDefault)
  {
  char Buff[1024];
  //Strng s;
  DWORD Len = GetPrivateProfileString(lpszSection, lpszEntry, "", Buff, sizeof(Buff), (char*)(const char*)sFilename);
  if (Len==0)
    sTmpRdStr = pDefault;
  else
    sTmpRdStr = Buff;
  return sTmpRdStr;
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pValue)
  {
  WritePrivateProfileString(lpszSection, lpszEntry, pValue, (char*)(const char*)sFilename);
  }

//---------------------------------------------------------------------------

DWORD CProfINIFile::RdSection(LPCTSTR lpszSection, char* pBuff, int BuffLen)
  {
  DWORD Len = GetPrivateProfileSection(lpszSection, pBuff, BuffLen, (char*)(const char*)sFilename);
  return Len;
  }

//---------------------------------------------------------------------------

void CProfINIFile::WrSection(LPCTSTR lpszSection, char* pBuff)
  {
  WritePrivateProfileSection(lpszSection, pBuff, (char*)(const char*)sFilename);
  }

//---------------------------------------------------------------------------

DWORD CProfINIFile::RdSectionNames(char* pBuff, int BuffLen)
  {
  DWORD Len = GetPrivateProfileSectionNames(pBuff, BuffLen, (char*)(const char*)sFilename);
  return Len;
  }

//---------------------------------------------------------------------------

void CProfINIFile::Flush()
  {
  }

//---------------------------------------------------------------------------

void CProfINIFile::GetProfItem(char* &pBuff, char* &pName, char* &pExp)
  {
  pName=pBuff;
  pExp=strchr(pBuff, '=');
  pBuff+=strlen(pBuff)+1;
  if(pExp)
    {
    *pExp=0;
    pExp++;
    }
  else
    pExp="";
  }

//---------------------------------------------------------------------------
//===========================================================================

CProfINIFile AppPF;

void DoInit()
  {
  AppPF.SetProfFilename("DevLic.ini");
  }

//===========================================================================
//===========================================================================
//---------------------------------------------------------------------------

CLicenseLocationDlg::CLicenseLocationDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CLicenseLocationDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CLicenseLocationDlg)
  m_AppPath = _T("");
  m_LocationOpt = 0;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLicenseLocationDlg)
  DDX_Text(pDX, IDC_CK_APPPATH, m_AppPath);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLicenseLocationDlg, CDialog)
  //{{AFX_MSG_MAP(CLicenseLocationDlg)
  ON_BN_CLICKED(IDC_CK_BROWSE, OnBrowse)
  ON_BN_CLICKED(IDC_CK_LOCATIONOPTION, OnLocationOption)
  ON_BN_CLICKED(IDC_CK_LOCATIONOPTION2, OnLocationOption)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_CK_BROWSE, OnUpdateLocation)
  ON_UPDATE_COMMAND_UI(IDC_CK_APPPATH, OnUpdateLocation)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CLicenseLocationDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnUpdateLocation(CCmdUI* pCmdUi)
  {
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnLocationOption() 
  {
  UpdateData(TRUE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnBrowse() 
  {
  UpdateData(TRUE);
  char Buff[128];
  sprintf(Buff, "%s (%s)|%s||", CK_AppName, CK_KeyFile, CK_KeyFile);

  //CSCDFileDialog Dlg(TRUE, NULL, CK_KeyFile, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
  CFileDialog Dlg(TRUE, NULL, CK_KeyFile, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, Buff);
  Dlg.m_ofn.lpstrInitialDir = (const char*)m_AppPath;
  sprintf(Buff, "Find %s with license", CK_AppName);
  Dlg.m_ofn.lpstrTitle = Buff;
  if (Dlg.DoModal()==IDOK)
    {
    CString NewPath = Dlg.GetPathName();
    char Drv[_MAX_DRIVE];
    char Dir[_MAX_DIR];
    _splitpath((const char*)NewPath, Drv, Dir, NULL, NULL);
    m_AppPath = Drv;
    m_AppPath += Dir;
    UpdateData(FALSE);
    }

  }

//---------------------------------------------------------------------------

void CLicenseLocationDlg::OnOK() 
  {
  UpdateData(TRUE);

  CDialog::OnOK();
  }

//===========================================================================

