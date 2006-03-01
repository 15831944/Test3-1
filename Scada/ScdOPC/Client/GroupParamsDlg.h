//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  GroupParamsDlg.h
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to view and modify the group parameters.
//                (mainly the update rate and active flag)
//
//**************************************************************************

class GroupParamsDlg : public CDialog
{
// Construction
public:
   GroupParamsDlg(IUnknown* pGroup, CWnd* pParent = NULL);

// Dialog Data
   //{{AFX_DATA(GroupParamsDlg)
   enum { IDD = IDD_GROUP_PARAMS };
   BOOL  m_active;
   float m_deadband;
   DWORD m_LCID;
   CString  m_name;
   DWORD m_rate;
   long  m_timebias;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(GroupParamsDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   OPCGroupStateMgt opcGroup;
   OPCHANDLE client, server;

   // Generated message map functions
   //{{AFX_MSG(GroupParamsDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnApply();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
