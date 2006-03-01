//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ScdIODB.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  MFC Application object
//
//
//**************************************************************************

#ifndef __ScdTgSrc_H__              // Only Include Once
#define __ScdTgSrc_H__


class CScdTgSrcApp : public CWinApp
{
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   CScdTgSrcApp();
   void  ForceExit();
   BOOL  m_bAutoClose;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CScdTgSrcApp)
   public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();
   //}}AFX_VIRTUAL

// Implementation

   //{{AFX_MSG(CScdTgSrcApp)
   afx_msg void OnAppAbout();
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   HANDLE hOPCThread;
   DWORD OPCThreadId;
};

extern CScdTgSrcApp theApp;

/////////////////////////////////////////////////////////////////////////////
#endif