//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  FindServersDlg.h
//  $Author    :  Jim Hansen
//
//  Description:  The OPC actions are mostly handled by this Doc.
//                class Item represents items in a group
//                class CAdviseSink is the interface that gets advise callbacks.
//
//**************************************************************************
#if !defined(AFX_FINDSERVERSDLG_H__658EECA1_81F1_11D1_A115_00609724420C__INCLUDED_)
#define AFX_FINDSERVERSDLG_H__658EECA1_81F1_11D1_A115_00609724420C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FindServersDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindServersDlg dialog

class CFindServersDlg : public CDialog
{
// Construction
public:
    CFindServersDlg(CStringList* pList, CWnd* pParent = NULL);
   CStringList*    m_pOPCServers;

// Dialog Data
    //{{AFX_DATA(CFindServersDlg)
    enum { IDD = IDD_FIND_OPCSERVERS };
    CAnimateCtrl    m_animation;
    CString m_node;
    CString m_server;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFindServersDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
   HKEY           hk;
   int            index;

    // Generated message map functions
    //{{AFX_MSG(CFindServersDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDSERVERSDLG_H__658EECA1_81F1_11D1_A115_00609724420C__INCLUDED_)
