//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#if WITHSPECSHEETS

class CSpecSheet : public CDialog
  {
  public:
    CSpecSheet(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CSpecSheet)
    enum { IDD = IDD_SPECSHEET };
    CComboBox m_Sheets;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSpecSheet)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CByteArray ReadOnlyList;
    //{{AFX_MSG(CSpecSheet)
    afx_msg void OnDelete();
    afx_msg void OnLoad();
    afx_msg void OnSave();
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeSheets();
    afx_msg void OnEditchangeSheets();
    //}}AFX_MSG
    afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
#endif
