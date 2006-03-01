//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TAGVDLGS_H
#define __TAGVDLGS_H

#include "tagvdoc.h"
#include "..\resource.h"

#define UseTrendInfoDlg 0 //dialog no longer used (June 1999)
#define UseNewTrendDlg 0 //dialog no longer used (June 1999)

#if UseTrendInfoDlg

class CTrndInfoDlg : public CDialog
  {
  public:
    CTrndInfoDlg(pCTagVwDoc pDoc_, char* pFirstTag = "", CWnd* pParent = NULL);
    //{{AFX_DATA(CTrndInfoDlg)
    enum { IDD = IDD_TRNDSLOTDLG };
    CString	m_Desc;
    CString	m_Type;
    CString	m_Eng;
    double	m_DispMin;
    double	m_DispMax;
    CString	m_Recording;
    CString	m_TrendTitle;
    CString	m_Tag;
    //}}AFX_DATA
    CComboCombo	m_TagList;
    pCTagVwDoc pDoc;
    CString sFirstTag;
    int CurSel;
    flag bChanged:1,
         bHistOn:1,
         bHistExists:1,
         bHistLocal:1, // CNM
         bDidChange:1,
         bDidTagChange:1;
    //{{AFX_VIRTUAL(CTrndInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void BuildList();
    void Changed(flag HasChanged = True);
    //{{AFX_MSG(CTrndInfoDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTag();
    afx_msg void OnApply();
    afx_msg void OnEdit();
    virtual void OnOK();
    afx_msg void OnChangeTag();
    //}}AFX_MSG
    afx_msg LRESULT OnComboBoxMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUi);
    afx_msg void OnUpdateApply(CCmdUI* pCmdUi);
    afx_msg void OnChange(UINT nID);
    DECLARE_MESSAGE_MAP()
  };
#endif
//===========================================================================

class CTrndDlg : public CDialog
  {
  public:
    CTrndDlg(char* pFirstOption = "", CWnd* pParent = NULL);   // standard constructor
    ~CTrndDlg();

    CString       sFirstOption;
    int           iPrevSel;
    int           iWndCnt;
    CWnd**        TrendWnds;
    flag          bChanged:1;
    CComboCombo   m_TrendList;
    //{{AFX_DATA(CTrndDlg)
    enum { IDD = IDD_TRNDDLG };
    int		m_XDivs;
    int		m_YDivs;
    BOOL	m_ST;
    BOOL	m_SD;
    BOOL	m_ET;
    BOOL	m_ED;
    BOOL	m_Dur;
    BOOL	m_HS;
    int		m_ScrollOpt;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CTrndDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void StoreWndList(CWnd* w);
    void GetTrendWndList();
    void SavePrev();
    void Changed(flag HasChanged = True);
    //{{AFX_MSG(CTrndDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTrndslist();
    afx_msg void OnApplyall();
    virtual void OnOK();
    afx_msg void OnApply();
    afx_msg void OnQuery();
    //}}AFX_MSG
    afx_msg LRESULT OnComboBoxMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateApplyAll(CCmdUI* pCmdUi);
    afx_msg void OnUpdateApply(CCmdUI* pCmdUi);
    afx_msg void OnUpdateBtn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateFlags(CCmdUI* pCmdUi);
    afx_msg void OnChange(UINT nID);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
#if UseNewTrendDlg

class CNewTrndSlotDlg : public CDialog
  {
  public:
    CNewTrndSlotDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CNewTrndSlotDlg)
    enum { IDD = IDD_TRNDNEWSLOTDLG };
    CString	m_Tag;
    CString	m_Desc;
    CString	m_Type;
    CString	m_Eng;
    double	m_DispMin;
    double	m_DispMax;
    BOOL	m_RecordIt;
    //}}AFX_DATA
    flag bHistOn;
    //{{AFX_VIRTUAL(CNewTrndSlotDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CNewTrndSlotDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    afx_msg void OnUpdateRecordIt(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

#endif
//===========================================================================

class CTBaseSetDlg : public CDialog
  {
  public:
    CTBaseSetDlg(pCTagVwDoc pDoc_, CWnd* pParent = NULL);
    ~CTBaseSetDlg();  //xIGF
    //{{AFX_DATA(CTBaseSetDlg)
    enum { IDD = IDD_TIMEBASE };
    CString m_Duration;
    CString m_Start;
    CString	m_TrendTitle;
    //}}AFX_DATA
    pCTagVwDoc pDoc;
    //{{AFX_VIRTUAL(CTBaseSetDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;

    //{{AFX_MSG(CTBaseSetDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnOptions();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);//xIGF
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CTrndInfo : public CDialog
  {
  public:
    CTrndInfo(CWnd* pParent = NULL);
    //{{AFX_DATA(CTrndInfo)
	enum { IDD = IDD_TRNDINFO };
    CString m_Tag;
    CString m_Time;
    CString m_Value;
	CString	m_Range;
	CString	m_Desc;
	//}}AFX_DATA

    static CTrndInfo* pTheTrndInfo;
    static void      Open(CWnd* pParent, POINT pt, const char * pTrendName);
    static void      Close(UINT CloseDelay=0);
    static void      ShutDown();
    static void      SetCurrentData(const char * pTime, const char * pTag, const char* pDesc, const char* pValue, const char* pRange);

    //{{AFX_VIRTUAL(CTrndInfo)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CTrndInfo)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CTrendPrintDlg : public CDialog
  {
  public:
    CTrendPrintDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CTrendPrintDlg)
    enum { IDD = IDD_TRNDPRINTDLG };
    BOOL m_PrintTrendLine;
    BOOL m_PrintTrendText;
    BOOL m_TrueColors;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CTrendPrintDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CTrendPrintDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
#endif


