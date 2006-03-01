//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ServerStatus.h
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to display OPC server status.
//                A timer updates the status while the dialog is alive.
//
//**************************************************************************

class CServerStatus : public CDialog
{
// Construction
public:
   CServerStatus(OPCServer& newServer, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CServerStatus)
   enum { IDD = IDD_STATUS };
   CString  m_current;
   DWORD m_groups;
   DWORD m_bandwidth;
   CString  m_last;
   CString  m_start;
   CString  m_status;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CServerStatus)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void Update();
   OPCServer opcServer;

   // Generated message map functions
   //{{AFX_MSG(CServerStatus)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnTimer(UINT nIDEvent);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
