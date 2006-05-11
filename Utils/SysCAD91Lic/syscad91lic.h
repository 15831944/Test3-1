//============== Copyright Kenwalt (Pty) Ltd (1997, ...) ====================
// $Log:  $
// 
// $Nokeywords: $
//===========================================================================

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

class CSysCADLicApp : public CWinApp
  {
  public:
    CSysCADLicApp();
    //{{AFX_VIRTUAL(CSysCADLicApp)
    public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL
    //{{AFX_MSG(CSysCADLicApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };


/////////////////////////////////////////////////////////////////////////////
