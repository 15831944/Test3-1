//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  WriteItemDlg.h
//  $Author    :  Jim Hansen
//
//  Description:  Dialog for writing values to an item.
//
//
//**************************************************************************

class WriteItemDlg : public CDialog
{
// Construction
public:
   WriteItemDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(WriteItemDlg)
   enum { IDD = IDD_WRITE_ITEM };
   BOOL  m_async;
   CString  m_value;
   //}}AFX_DATA
   CString  m_item;


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(WriteItemDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(WriteItemDlg)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
