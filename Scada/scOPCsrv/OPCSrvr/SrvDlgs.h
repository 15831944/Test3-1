//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __SRVDLGS_H
#define __SRVDLGS_H

#ifndef  __SCDVER_H
#include "scdver.h"
#endif
#if WITHOPC

//===========================================================================
class CScdOPCManager;

const int MaxOPCStatsHistory = 5;

class COPCSrvrStatsDlg : public CDialog
  {
  public:
    COPCSrvrStatsDlg(CScdOPCManager* pOPCMngr, CWnd* pParent = NULL);   // standard constructor
    ~COPCSrvrStatsDlg();
    CScdOPCManager* pMngr;
    Strng Hist[MaxOPCStatsHistory];
    int iHistIndex;
    WORD  bDidInit:1,
          bSubsChanged:1;

    //{{AFX_DATA(COPCSrvrStatsDlg)
    enum { IDD = IDD_OPCSRVRSTATSDLG };
    CListBox m_StatsList;
    int m_ShowStats;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(COPCSrvrStatsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(COPCSrvrStatsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnForce();
    afx_msg void OnShowStats();
    afx_msg void OnRefresh();
    afx_msg void OnOptions();
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateStats(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateForce(CCmdUI* pCmdUi);
    afx_msg void OnUpdateRefresh(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

#endif
#endif
