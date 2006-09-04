//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __WNDSLCT_H
#define __WNDSLCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

//===========================================================================

class CWndArrangeDlg : public CDialog
  {
  public:
    CWndArrangeDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CWndArrangeDlg)
    enum { IDD = IDD_WNDARRANGEDLG };
    BOOL	m_MaximiseMain;
    BOOL	m_GrfMinimise;
    BOOL	m_OthMinimise;
    BOOL	m_TrnMinimise;
    int		m_GrfStyle;
    int		m_OthStyle;
    int		m_TrnStyle;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CWndArrangeDlg)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CWndArrangeDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedOk();
  };

//===========================================================================

#endif

