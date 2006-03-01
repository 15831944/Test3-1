//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  GroupDialog.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Group properties dialog
//
//
//**************************************************************************

#ifndef __GROUPDIALOG_H__              // Only Include Once
#define __GROUPDIALOG_H__

class GroupDialog : public CDialog
{
// Construction
public:
   GroupDialog(CWnd* pParent = NULL);   // standard constructor
   GroupDialog(OTagGroup* pGroup, CWnd* pParent = NULL);

// Dialog Data
   //{{AFX_DATA(GroupDialog)
   enum { IDD = IDD_GROUP };
   CString  m_name;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GroupDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
    OTagGroup* m_pGroup;
   // Generated message map functions
   //{{AFX_MSG(GroupDialog)
	virtual void OnOK();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif