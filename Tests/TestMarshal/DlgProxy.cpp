// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "TestMarshal.h"
#include "DlgProxy.h"
#include "TestMarshalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestMarshalDlgAutoProxy

IMPLEMENT_DYNCREATE(CTestMarshalDlgAutoProxy, CCmdTarget)

CTestMarshalDlgAutoProxy::CTestMarshalDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CTestMarshalDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CTestMarshalDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CTestMarshalDlgAutoProxy::~CTestMarshalDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CTestMarshalDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CTestMarshalDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CTestMarshalDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTestMarshalDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CTestMarshalDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ITestMarshal to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2C971345-9D63-4000-AF61-FC31A1A64F0F}
static const IID IID_ITestMarshal =
{ 0x2c971345, 0x9d63, 0x4000, { 0xaf, 0x61, 0xfc, 0x31, 0xa1, 0xa6, 0x4f, 0xf } };

BEGIN_INTERFACE_MAP(CTestMarshalDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CTestMarshalDlgAutoProxy, IID_ITestMarshal, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {7035768B-1B4C-4A35-900F-1A360495CC61}
IMPLEMENT_OLECREATE2(CTestMarshalDlgAutoProxy, "TestMarshal.Application", 0x7035768b, 0x1b4c, 0x4a35, 0x90, 0xf, 0x1a, 0x36, 0x4, 0x95, 0xcc, 0x61)

/////////////////////////////////////////////////////////////////////////////
// CTestMarshalDlgAutoProxy message handlers
