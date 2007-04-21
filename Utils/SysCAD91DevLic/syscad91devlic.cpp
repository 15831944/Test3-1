//============== Copyright Kenwalt (Pty) Ltd (1997, ...) ====================
// $Log:  $
// 
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "syscad91devlic.h"
#include "kwaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CSysCADLicApp, CWinApp)
  //{{AFX_MSG_MAP(CSysCADLicApp)
  //}}AFX_MSG
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CSysCADLicApp::CSysCADLicApp()
{
}

CSysCADLicApp theApp;

BOOL CSysCADLicApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#elif (_MSC_VER < 1400)
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CKwaDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
