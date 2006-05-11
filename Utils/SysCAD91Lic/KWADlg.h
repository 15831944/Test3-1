//============== Copyright Kenwalt (Pty) Ltd (1997, ...) ====================
// $Log:  $
// 
// $Nokeywords: $
//===========================================================================

class CKwaDlg : public CDialog
  {
  public:
    CKwaDlg(CWnd* pParent = NULL);// standard constructor
    //{{AFX_DATA(CKwaDlg)
    enum { IDD = IDD_KWA_DIALOG };
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CKwaDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void PopupMenu(CPoint point);
    void UpdateInfo();
    void SaveWindowPos();

    HICON m_hIcon;
    //{{AFX_MSG(CKwaDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnAbout();
    afx_msg void OnAuthorize();
    afx_msg void OnInformation();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMore();
    afx_msg void OnClose();
    virtual void OnOK();
    virtual void OnCancel();
    //}}AFX_MSG
    afx_msg void OnLicense(UINT nID);
    afx_msg void OnUpdateLicense(CCmdUI* pCmdUI);
    afx_msg LRESULT OnInitLicenseMsg(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };
