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

class CSelectWindowDlg : public CDialog
  {
  public:
    CSelectWindowDlg(CWnd* pParent = NULL);   // standard constructor

    CWindowLists WL;
    int iPrevList;
    CWnd * pToActivate;
    //{{AFX_DATA(CSelectWindowDlg)
    enum { IDD = IDD_SELECTWINDOWDLG };
    CListBox m_Trends;
    CListBox m_Graphics;
    CListBox m_General;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSelectWindowDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    void ActivateWnd(CWnd* pWnd);
    //{{AFX_MSG(CSelectWindowDlg)
    afx_msg void OnDblclkGeneralwndlist();
    afx_msg void OnDblclkGraphicswndlist();
    afx_msg void OnDblclkTrendswndlist();
    afx_msg void OnNewwindow();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnSelchangeGeneralwndlist();
    afx_msg void OnSelchangeGraphicswndlist();
    afx_msg void OnSelchangeTrendswndlist();
    //}}AFX_MSG
    afx_msg void OnUpdateOK(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CWndSlctWnd : public CDialog
  {
  public:
    CWndSlctWnd(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CWndSlctWnd)
    enum { IDD = IDD_WNDSELECT };
    CListBox	m_List;
    //}}AFX_DATA

    static void OpenIt();
    static void CloseIt();
    static void RefreshIt();
    static void ChkRefreshIt();
    static BOOL UseSelectWndList() { return bUseSelectWndList; };
    static void SetUseSelectWndList(BOOL Use) { bUseSelectWndList = Use; };

    static void ArrangeWindows(int RqdLayoutStyle = -1);

    //{{AFX_VIRTUAL(CWndSlctWnd)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    static CWndSlctWnd * pTheWnd;
    static BOOL bDoRefresh;
    static BOOL bUseSelectWndList;
    bool Inited;
    void DoTheRefresh();
    void SavePos();
    void RestorePos();

    //{{AFX_MSG(CWndSlctWnd)
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
    afx_msg void OnSelchangeList2();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

