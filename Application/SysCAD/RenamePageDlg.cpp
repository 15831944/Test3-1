// RenamePageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SysCAD.h"
#include "RenamePageDlg.h"


// CRenamePageDlg dialog

IMPLEMENT_DYNAMIC(CRenamePageDlg, CDialog)

CRenamePageDlg::CRenamePageDlg(LPCTSTR OldName, CWnd* pParent /*=NULL*/)
: CDialog(CRenamePageDlg::IDD, pParent)
, m_OldName(OldName)
, m_NewName(OldName)
  {

  }

CRenamePageDlg::~CRenamePageDlg()
  {
  }

void CRenamePageDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_OLDNAME2, m_NewName);
  DDX_Text(pDX, IDC_OLDNAME, m_OldName);
  }


BEGIN_MESSAGE_MAP(CRenamePageDlg, CDialog)
END_MESSAGE_MAP()


// CRenamePageDlg message handlers
