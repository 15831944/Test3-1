//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DRV_DLGS_H
#define  __DRV_DLGS_H

#if WITHDRVMAN

//------------------------------------------------------------------------

_FWDDEF(CDriverSheet)
class CDriverSheet : public CPropertySheet
  {
  DECLARE_DYNAMIC(CDriverSheet)
  public:
    CDriverSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CDriverSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CDriverSheet();
    void UpdateStats();
    void Stop();
    void DisplayTag(char* pGotoTag);
    //{{AFX_VIRTUAL(CDriverSheet)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL
  protected:
    void DeletePages();
    //{{AFX_MSG(CDriverSheet)
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnNcDestroy();
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateStats(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    flag bInited;
  };

//------------------------------------------------------------------------

class CDriverPage : public CPropertyPage
  {
  DECLARE_DYNCREATE(CDriverPage)
  public:
    flag bDidInit:1,
         bChanged:1;
    CDriverPage();
    ~CDriverPage();
    //{{AFX_DATA(CDriverPage)
    enum { IDD = IDD_DRIVER_PAGE };
    CComboBox	m_StartRefresh;
    BOOL m_ForcesOn;
    BOOL m_IgnoresOn;
    int m_NoDrivers;
    int m_NoSlots;
    BOOL	m_NoiseOn;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CDriverPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
  protected:
    void Change(flag chg = True);
    virtual void OnOK();
    //{{AFX_MSG(CDriverPage)
    afx_msg void OnApplybtn();
    afx_msg void OnDrvforceson();
    afx_msg void OnDrvignoreson();
    virtual BOOL OnInitDialog();
    afx_msg void OnForceall();
    afx_msg void OnForcenone();
    afx_msg void OnReloadbtn();
    afx_msg void OnDrvnoiseon();
    afx_msg void OnSelchangeDrvstartrefresh();
    afx_msg void OnDrvslotsRefreshIO();
    afx_msg void OnDrvslotsRefreshIO_In();
    afx_msg void OnDrvslotsRefreshIO_Out();
    afx_msg void OnDrvslotsRefreshConns();
    //}}AFX_MSG
    afx_msg void OnUpdateApplyBtn(CCmdUI* pCmdUi);
    afx_msg LRESULT OnUpdateBtns(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };

//------------------------------------------------------------------------

class CSlotsPage : public CPropertyPage
  {
  DECLARE_DYNCREATE(CSlotsPage)
  public:
    CSlotsPage();
    ~CSlotsPage();
    flag bDidInit:1,
         bChanged:1;
    int CurSlotNo;
    //{{AFX_DATA(CSlotsPage)
	enum { IDD = IDD_DRIVERSLOTS_PAGE };
    CComboBox	m_SlotList;
    CString	m_DrvDesc;
    CString	m_SlotDesc;
    CString	m_Desc;
    CString	m_IO;
    BOOL	m_IgnoreRead;
    BOOL	m_IgnoreWrite;
    double	m_ForceValue;
    BOOL	m_EnableForces;
    CString m_RawValue;
	CString	m_Connects;
	CString	m_Value;
	//}}AFX_DATA
    //{{AFX_VIRTUAL(CSlotsPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void Change(flag chg = True);
    void RefreshSlotList();
    virtual void OnOK();
    virtual void OnCancel();
    //{{AFX_MSG(CSlotsPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeSlotcombo();
    afx_msg void OnApplybtn();
    afx_msg void OnIgnoreread();
    afx_msg void OnIgnorewrite();
    afx_msg void OnEnableforces();
    afx_msg void OnChangeForcevalue();
    afx_msg void OnDrvslotsRefreshIO();
    afx_msg void OnDrvslotsRefreshConns();
    //}}AFX_MSG
    afx_msg LRESULT OnUpdateStats(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateApplyBtn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateIgnoreRead(CCmdUI* pCmdUi);
    afx_msg void OnUpdateIgnoreWrite(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//------------------------------------------------------------------------

extern CDriverSheet* pDrvSheet;   //pointer to driver tabbed dialog options box

//------------------------------------------------------------------------
#endif

#endif

