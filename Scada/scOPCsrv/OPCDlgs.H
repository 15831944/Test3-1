/*
class COPCStatsPage : public CPropertyPage
  {
  DECLARE_DYNCREATE(COPCStatsPage)
  public:
    COPCStatsPage();
    COPCStatsPage(pCOPCDriver Drv);
    ~COPCStatsPage();

    flag bDidInit;
    COPCDriver *pDrv;
    //{{AFX_DATA(COPCStatsPage)
	  enum { IDD = IDD_OPCSTATSPAGE };
	  CListBox	m_StatsList;
	  double	m_IterTime;
	  //}}AFX_DATA
    CString m_Coms[MaxOPCClnts];
    double m_Times[MaxOPCClnts];
    double m_Procs[MaxOPCClnts];
    //{{AFX_VIRTUAL(COPCStatsPage)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
	  CStopWatch SW;
    double PrevIterTime;
    //{{AFX_MSG(COPCStatsPage)
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
    afx_msg LRESULT OnUpdateStats(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
  };
*/

//===========================================================================

class COPCHoldPage : public CPropertyPage
  {
	DECLARE_DYNCREATE(COPCHoldPage)
  public:
	  COPCHoldPage();
    COPCHoldPage(pCOPCDriver Drv);
  	~COPCHoldPage();

    flag bDidInit;
    CDrvSlot *pCurSlot;
    COPCDriver *pDrv;
	  //{{AFX_DATA(COPCHoldPage)
	  enum { IDD = IDD_OPCHOLDPAGE };
	  CComboBox	m_TagCombo;
	  CListBox	m_OpList;
	  DWORD	m_DelayTime;
	  CString	m_FullAddr;
	  BOOL	m_UseDelay2;
	  DWORD	m_DelayTime2;
	  CString	m_Type;
	  CString	m_IO;
	  //}}AFX_DATA
	  //{{AFX_VIRTUAL(COPCHoldPage)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
    void RefreshDrvTagList();
    virtual void OnOK();
    virtual void OnCancel();
	  //{{AFX_MSG(COPCHoldPage)
	  virtual BOOL OnInitDialog();
    afx_msg void OnHold();
	  afx_msg void OnSelchangeTagcombo();
	  afx_msg void OnSelchangeOplist();
  	//}}AFX_MSG
    afx_msg void OnUpdateHoldBtn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateDelay(CCmdUI* pCmdUi);
    afx_msg void OnUpdateDelay2(CCmdUI* pCmdUi);
    afx_msg void OnUpdateUseDelay2(CCmdUI* pCmdUi);
	  DECLARE_MESSAGE_MAP()
  };

//===========================================================================
