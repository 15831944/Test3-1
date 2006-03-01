// ImpExpSelect.cpp : implementation file
//

#include "stdafx.h"
#include "SysCAD.h"
#include "ImpExpSelect.h"
#include ".\impexpselect.h"


// CSelectImport dialog

IMPLEMENT_DYNAMIC(CSelectImport, CDialog)
CSelectImport::CSelectImport(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectImport::IDD, pParent)
  , m_lImportWhat(-1)
  {
}

CSelectImport::~CSelectImport()
{
}

void CSelectImport::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_IMPORTWHAT, m_List);
}


BEGIN_MESSAGE_MAP(CSelectImport, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_WM_CREATE()
  ON_CBN_SELCHANGE(IDC_IMPORTWHAT, OnCbnSelchangeImportwhat)
END_MESSAGE_MAP()

// CSelectImport message handlers

void CSelectImport::OnBnClickedOk()
  {
  // TODO: Add your control notification handler code here
  UpdateData();
  m_lImportWhat=m_List.GetCurSel();
  OnOK();
  }

BOOL CSelectImport::OnInitDialog(void)
  {
  BOOL OK=CDialog::OnInitDialog();
  m_List.ResetContent();
  m_List.AddString("Neutral DB");
  m_List.AddString("SysCAD Flowsheet");
  m_List.AddString("ELMS Electrical DB");
  m_List.SetCurSel(0);

  return 0;
  }


void CSelectImport::OnCbnSelchangeImportwhat()
  {
  }

// CSelectExport dialog

IMPLEMENT_DYNAMIC(CSelectExport, CDialog)
CSelectExport::CSelectExport(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectExport::IDD, pParent)
  , m_lExportWhat(-1)
  {
}

CSelectExport::~CSelectExport()
{
}

void CSelectExport::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_IMPORTWHAT, m_List);
}


BEGIN_MESSAGE_MAP(CSelectExport, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_WM_CREATE()
  ON_CBN_SELCHANGE(IDC_IMPORTWHAT, OnCbnSelchangeExportwhat)
END_MESSAGE_MAP()

// CSelectExport message handlers

void CSelectExport::OnBnClickedOk()
  {
  // TODO: Add your control notification handler code here
  UpdateData();
  m_lExportWhat=m_List.GetCurSel();
  OnOK();
  }

BOOL CSelectExport::OnInitDialog(void)
  {
  BOOL OK=CDialog::OnInitDialog();
  m_List.ResetContent();
  m_List.AddString("Neutral DB");
  m_List.AddString("SysCAD Flowsheet");
  m_List.AddString("ELMS Electrical DB");
  m_List.SetCurSel(0);

  return 0;
  }


void CSelectExport::OnCbnSelchangeExportwhat()
  {
  }

