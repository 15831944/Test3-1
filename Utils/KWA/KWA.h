//============== Copyright Kenwalt (Pty) Ltd (1997, ...) ====================
// $Log:  $
// 
// $Nokeywords: $
//===========================================================================

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

class CKwaApp : public CWinApp
  {
  public:
    CKwaApp();
    //{{AFX_VIRTUAL(CKwaApp)
    public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL
    //{{AFX_MSG(CKwaApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };


/////////////////////////////////////////////////////////////////////////////
