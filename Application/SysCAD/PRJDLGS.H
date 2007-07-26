//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __PRJDLGS_H
#define __PRJDLGS_H

#include "executiv.h"
#include "resource.h"

//===========================================================================

class CProjectSettingsDlg : public CPropertySheet
  {
  DECLARE_DYNAMIC(CProjectSettingsDlg)
  public:
    //CProjectSettingsDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CProjectSettingsDlg(bool AsModal, CProject *Prj, CProjectSettings *Set, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CProjectSettingsDlg();

    bool                m_bAsModal;
    CProject          * m_pPrj;
    CProjectSettings  * m_pSet;
    CPropertyPage     * m_pModesPage;
    CPropertyPage     * m_pGrfTagsPage;
    CPropertyPage     * m_pCommsPage;
    CPropertyPage     * m_pArchivePage;
    CPropertyPage     * m_pHistoryPage;
#if WITHNETSERVER
    CPropertyPage* pNetworkPage;
#endif

    //{{AFX_VIRTUAL(CProjectSettingsDlg)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

  protected:
    void StoreCurPageNo();
    //{{AFX_MSG(CProjectSettingsDlg)
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnNcDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CMergeDlg : public CDialog
  {
  public:
    CMergeDlg(CMergeProjectsInfo* MPI, CWnd* pParent = NULL);
    ~CMergeDlg();
    //{{AFX_DATA(CMergeDlg)
    enum { IDD = IDD_MERGEDLG };
    CString m_MergeReportFile;
    CString m_ModelCfg;
    CString m_SlavePrj;
    int m_MasterRenameType;
    int m_SlaveRenameType;
    CString m_MasterPrefixSuffix;
    CString m_SlavePrefixSuffix;
    int m_MasterChangeType;
    int m_SlaveChangeType;
    CString m_TagAppendChars;
#if WithNumericTags
	  BOOL m_MasterNumTagsOK;
	  BOOL m_SlaveNumTagsOK;
#endif
	  BOOL m_MasterStNumTagsOK;
    BOOL m_SlaveStNumTagsOK;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CMergeDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;
    CMergeProjectsInfo* pMPI;
    BOOL bTagNamingChanged;
    void PopulateDialog();
    void UpdateSlaveNumTags();
    void UpdateSlaveNumTagsOK();
    //{{AFX_MSG(CMergeDlg)
    afx_msg void OnBrowse();
    virtual BOOL OnInitDialog();
    afx_msg void OnMerge();
    afx_msg void OnChangeSlaveprj();
    afx_msg void OnMstrstnumtags();
    afx_msg void OnMstrnumtags();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnMasterPrefixSuffix();
    afx_msg void OnSlavePrefixSuffix();
    afx_msg void OnChangeType();
    afx_msg void OnUpdateMasterPrefixSuffix(CCmdUI* pCmdUi);
    afx_msg void OnUpdateSlavePrefixSuffix(CCmdUI* pCmdUi);
    afx_msg void OnUpdateMasterStNumTags(CCmdUI* pCmdUi);
    afx_msg void OnUpdateSlaveNumTags(CCmdUI* pCmdUi);
    afx_msg void OnUpdateSlaveStNumTags(CCmdUI* pCmdUi);

  DECLARE_MESSAGE_MAP()
  };

class CMergeReportDlg : public CDialog
  {
  public:
    byte bMergeAllowed:1;
    Strng sMessage;

    CMergeReportDlg(CMergeProjectsInfo* MPI, CWnd* pParent = NULL);
    //{{AFX_DATA(CMergeReportDlg)
    enum { IDD = IDD_MERGEREPDLG };
    CHeaderListBox m_List;
    CTabCtrl m_Tab;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CMergeReportDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    int iPrevTab;
    CMergeProjectsInfo* pMPI;
    void PopulateTabList();
    //{{AFX_MSG(CMergeReportDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeReportTab(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    afx_msg void OnUpdateOK(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

#endif
