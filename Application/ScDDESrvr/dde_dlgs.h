//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __DDEDLGS_H
#define __DDEDLGS_H

//===========================================================================
class CScdDDEManager;

class CDDEStatsDlg : public CDialog
  {
  public:
    CDDEStatsDlg(CScdDDEManager* pDDEMngr, CWnd* pParent = NULL);   // standard constructor
    ~CDDEStatsDlg();
    CScdDDEManager* pMngr;
    WORD  bDidInit:1,
          bSubsChanged:1;

    //{{AFX_DATA(CDDEStatsDlg)
    enum { IDD = IDD_DDESTATSDLG };
    CListBox m_StatsList;
    int m_ShowStats;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CDDEStatsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CDDEStatsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnForce();
    afx_msg void OnShowStats();
    afx_msg void OnRefresh();
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateStats(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateForce(CCmdUI* pCmdUi);
    afx_msg void OnUpdateRefresh(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

#endif
