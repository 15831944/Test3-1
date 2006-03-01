// CnvFormatDlg.cpp : implementation file
//
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define  __DATACNVS_CPP
#include "sc_defs.h"
#include "datacnvs.h"
#include "dataattr.h"
#include "dbhelper.h"
#include "errorlog.h"
#include "dbgmngr.h"
#include "cnvformatdlg.h"

#define dbgCnvs     1

#if dbgCnvs
//static CDbgMngr dbgCnvsList("General", "Cnvs");
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCnvFormatDlg dialog


CCnvFormatDlg::CCnvFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCnvFormatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCnvFormatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCnvFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCnvFormatDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCnvFormatDlg, CDialog)
	//{{AFX_MSG_MAP(CCnvFormatDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCnvFormatDlg message handlers
