// BadLicLocation.cpp : implementation file
//

#include "stdafx.h"
#include "BadLicLocation.h"
#include "licbase.h"


// CBadLicLocation dialog

IMPLEMENT_DYNAMIC(CBadLicLocation, CDialog)

CBadLicLocation::CBadLicLocation(LPCTSTR Location, CWnd* pParent /*=NULL*/)
: CDialog(CBadLicLocation::IDD, pParent)
  {
  m_Location = Location;
  m_Return = LicInit_ExitReqd;
  }

CBadLicLocation::~CBadLicLocation()
  {
  }

BOOL CBadLicLocation::OnInitDialog()
  {
  CDialog::OnInitDialog();
  m_CurLocation.SetWindowText(m_Location);
  return TRUE;
  };

void CBadLicLocation::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_STATICCURLOC, m_CurLocation);
  }


BEGIN_MESSAGE_MAP(CBadLicLocation, CDialog)
  ON_BN_CLICKED(ID_CHANGELOCATION, &CBadLicLocation::OnBnClickedChangelocation)
  ON_BN_CLICKED(ID_CONTINUEINDEMO, &CBadLicLocation::OnBnClickedContinueindemo)
  ON_BN_CLICKED(IDCANCEL, &CBadLicLocation::OnBnClickedCancel)
END_MESSAGE_MAP()


// CBadLicLocation message handlers

void CBadLicLocation::OnBnClickedChangelocation()
  {
  m_Return = LicInit_ChgLoc;
  OnOK();
  }

void CBadLicLocation::OnBnClickedContinueindemo()
  {
  m_Return = LicInit_GoDemo;
  OnOK();
  }

void CBadLicLocation::OnBnClickedCancel()
  {
  m_Return = LicInit_ExitReqd;
  OnCancel();
  }
