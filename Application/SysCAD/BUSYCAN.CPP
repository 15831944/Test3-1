//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// busycan.cpp : implementation file
//

#include "stdafx.h"
#include "syscad.h"
#include "busycan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBusyCan dialog


CBusyCan::CBusyCan(CWnd* pParent /*=NULL*/)
	: CDialog(CBusyCan::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBusyCan)
	m_Msg = _T("");
	//}}AFX_DATA_INIT
  m_Cancelled=FALSE;

  Create(CBusyCan::IDD, pParent); // NonModal Create
}


void CBusyCan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBusyCan)
	DDX_Text(pDX, IDC_MSG, m_Msg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBusyCan, CDialog)
	//{{AFX_MSG_MAP(CBusyCan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBusyCan message handlers

void CBusyCan::OnCancel() 
{
	// TODO: Add extra cleanup here
  m_Cancelled=TRUE;
	
  // CDialog::OnCancel();
}
