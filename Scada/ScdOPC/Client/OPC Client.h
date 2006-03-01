//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPC Client.h
//  $Author    :  Jim Hansen
//
//  Description:  Standard MFC application
//
//
//**************************************************************************

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// OPCClientApp:
// See OPC Client.cpp for the implementation of this class
//

class OPCClientApp : public CWinApp
{
public:
   OPCClientApp();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OPCClientApp)
   public:
   virtual BOOL InitInstance();
   //}}AFX_VIRTUAL

// Implementation

   //{{AFX_MSG(OPCClientApp)
   afx_msg void OnAppAbout();
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
