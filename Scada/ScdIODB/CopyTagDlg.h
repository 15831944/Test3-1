//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  CopyTagDlg.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  The dialog that prompts to rename/overwrite/skip a
//                copy operation that runs into a duplicate tag.
//
//**************************************************************************

#ifndef __COPYTAGDLG_H__               // Only Include Once
#define __COPYTAGDLG_H__

class CCopyTagDlg : public CDialog
{
// Construction
public:
   CCopyTagDlg(OModbusTag* pNewTag,
               OModbusTag* pCurrentTag,
               CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CCopyTagDlg)
   enum { IDD = IDD_COPY_TAG };
   CString  m_name;
   CString  m_title;
   CString  m_current;
   CString  m_new;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CCopyTagDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CCopyTagDlg)
   afx_msg void OnRename();
   afx_msg void OnOverwrite();
   afx_msg void OnSkip();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif