//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __GENDLGS_H
#define __GENDLGS_H

#include "gpwfuncs.h"
#include "scdver.h"
#include "resource.h"
#include "afxwin.h"

extern BOOL bDebugOptionsVisible;

//===========================================================================

class CGenSheet : public CPropertySheet
  {
  DECLARE_DYNAMIC(CGenSheet)
  public:
    CGenSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CGenSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CGenSheet();

    CPropertyPage* pToolBarPage;         
    CPropertyPage* pGlobalPage;
    CPropertyPage* pOtherPage;   
    CPropertyPage* pMousePage;   
    CPropertyPage* pColourPage;             
    CPropertyPage* pDebugPage;             
    CPropertyPage* pStatsPage;             
    CPropertyPage* pTrendsPage;           
    CPropertyPage* pSnapShotPage;
#if WITHSCRCYCLES
    CPropertyPage* pScreenSavePage;   
#endif
    
    flag IsStatsPage();
    //{{AFX_VIRTUAL(CGenSheet)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

  protected:
    void StoreCurPageNo();
    //{{AFX_MSG(CGenSheet)
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnNcDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CReportsProfileLineInfo
  {
  public:
    Strng Name;
    Strng Path;
    Strng FileOnly;
    Strng FullFilename;
    Strng Type;
  };

typedef CReportsProfileLineInfo* pCReportsProfileLineInfo;
typedef CArray <CReportsProfileLineInfo*, pCReportsProfileLineInfo&> CRPFLIArray; 

//--------------------------------------------------------------------------

class CReportsDlg : public CDialog
  {
  public:
    CReportsDlg(CWnd* pParent = NULL);   // standard constructor
    ~CReportsDlg();
    static void BuildList(CRPFLIArray& List, int& RepSelected, int& SetSelected, int& MacroSelected);
    //{{AFX_DATA(CReportsDlg)
    enum { IDD = IDD_REPORTSDLG };
    CTabCtrl m_Tab;
    CHeaderListBox m_RepList;
    CHeaderListBox m_SetList;
    CHeaderListBox m_MacroList;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CReportsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    int iPrevTab;
    CHeaderListBox* pList;
    void ChangeTab();
    void GenerateOne(int index);
    void ParseLine(int index, CString& RepName, CString& RepFilename, CString& RepType);
    //{{AFX_MSG(CReportsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnAdd();
    afx_msg void OnRemove();
    afx_msg void OnOptions();
    afx_msg void OnEdit();
    afx_msg void OnGenerate();
    afx_msg void OnSelchangeReportstab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnGenerateall();
    afx_msg void OnSelchangeList();
    afx_msg void OnDblclkList();
    afx_msg void OnEditExcel();
    //}}AFX_MSG
    afx_msg void OnUpdateSelection(CCmdUI* pCmdUi);
    afx_msg void OnUpdateGenerateAll(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CScriptsProfileLineInfo
  {
  public:
    Strng Path;
    Strng FileOnly;
    Strng FullFilename;
  };

typedef CScriptsProfileLineInfo* pCScriptsProfileLineInfo;
typedef CArray <CScriptsProfileLineInfo*, pCScriptsProfileLineInfo&> CSPFLIArray; 

//--------------------------------------------------------------------------

class CScriptsDlg : public CDialog
  {
  public:
    CScriptsDlg(CWnd* pParent = NULL);   // standard constructor
    ~CScriptsDlg();
    static int BuildList(CSPFLIArray& List);
    //{{AFX_DATA(CScriptsDlg)
    enum { IDD = IDD_SCRIPTSDLG };
    CHeaderListBox m_List;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CScriptsDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);//xIGF
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    HANDLE       hProcess;      //process handle for the editor
    DWORD        dwProcessId;   //process ID for the editor
    CCustomToolTipCtrl* pTTC;//xIGF

    //{{AFX_MSG(CScriptsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnAdd();
    afx_msg void OnRemove();
    afx_msg void OnEdit();
    afx_msg void OnSelchangeFilelist();
    afx_msg void OnExecScript();
    afx_msg void OnDblclkFilelist();
    afx_msg void OnEditfile();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);//xIGF
    afx_msg void OnUpdateSelection(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CCompareLine
  {
  public:
    Strng sFilename1;
    Strng sFilename2;
    Strng sOutFilename;
    flag bCurrent;
    flag bWindow;

    char* MakeLine(Strng& Line);
  };

typedef CArray <CCompareLine, CCompareLine&> CCompareLineArray; 

class CCompareDlg : public CDialog
  {
  public:
    CCompareDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CCompareDlg();//xIGF
    //{{AFX_DATA(CCompareDlg)
    enum { IDD = IDD_COMPAREDLG };
    CHeaderListBox m_List;
    BOOL m_ResetOutFile;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCompareDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);//xIGF
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCompareLineArray Lines;
    CCompareInfo CompInfo;
    CCustomToolTipCtrl* pTTC;//xIGF

   //{{AFX_MSG(CCompareDlg)
    virtual BOOL OnInitDialog(); //xIGF
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnAdd();
    afx_msg void OnRemove();
    afx_msg void OnEdit();
    afx_msg void OnSelchangeFilelist();
    afx_msg void OnCompare();
    afx_msg void OnDblclkFilelist();
    afx_msg void OnOptions();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);//xIGF
    afx_msg void OnUpdateSelection(CCmdUI* pCmdUi);
    afx_msg void OnUpdateResetOutFile(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

class CCompareResultsDlg : public CDialog
  {
  public:
    int        RetVal;            //return value from Executive CompareScenarios
    CString    sLastTag;          //last tag selected
    int        iLastIndex;        //index of last selection
    CCompareResultsDlg(CCompareInfo* Info, CWnd* pParent = NULL);
    ~CCompareResultsDlg();
    //{{AFX_DATA(CCompareResultsDlg)
    enum { IDD = IDD_COMPARERESULTSDLG };
    CCustomListCtrl m_List;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCompareResultsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    CCompareInfo* pInfo;
    int AccessableTag(CString& sTag);
    //{{AFX_MSG(CCompareResultsDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void OnOK();
    //}}AFX_MSG
    afx_msg void OnMenu(UINT id);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

//class CNeutralImportDBDlg : public CDialog
//  {
//  public:
//    CNeutralImportDBDlg(CWnd* pParent = NULL);   // standard constructor
//    enum { IDD = IDD_NEUTRALIMPORTDLG };
//    protected:
//    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//  protected:
//    virtual BOOL OnInitDialog();
//  	virtual void OnOK();
//    DECLARE_MESSAGE_MAP()
//  public:
//    afx_msg void OnBnClickedSeparatedbs();
//    afx_msg void OnEnChangeGrfdatabase();
//    afx_msg void OnBnClickedBrowseneutralpath();
//    CEdit m_GrfDatabase;
//    CEdit m_MdlDatabase;
//    CButton m_SeparateDBs;
//    CString m_sGrfDatabase;
//    CString m_sMdlDatabase;
//  };
//
////===========================================================================
//
//class CNeutralExportDBDlg : public CDialog
//  {
//  public:
//    CNeutralExportDBDlg(CWnd* pParent = NULL);   // standard constructor
//    enum { IDD = IDD_NEUTRALEXPORTDLG };
//    protected:
//    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//  protected:
//    virtual BOOL OnInitDialog();
//  	virtual void OnOK();
//    DECLARE_MESSAGE_MAP()
//  public:
//    afx_msg void OnBnClickedSeparatedbs();
//    afx_msg void OnBnClickedWithconfig();
//    afx_msg void OnEnChangeGrfdatabase();
//    afx_msg void OnBnClickedBrowseneutralpath();
//    CEdit m_GrfDatabase;
//    CEdit m_MdlDatabase;
//    CButton m_SeparateDBs;
//    CButton m_WithConfig;
//    CString m_sGrfDatabase;
//    CString m_sMdlDatabase;
//  };
//
//===========================================================================

#endif
