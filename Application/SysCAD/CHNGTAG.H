//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __SLCTWND_H
  #include "selctwnd.h"
#endif

class CChangeTag : public CDialog
  {
  public:
    CChangeTag(CWnd* pParent = NULL);   // standard constructor
    CChangeTag(char* InitTag, int CallAccess, CWnd* pParent = NULL);
    ~CChangeTag();
    int fCallAccess;
    //{{AFX_DATA(CChangeTag)
    enum { IDD = IDD_CHANGETAG };
    CString m_NewTag;
    CString m_OldTag;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CChangeTag)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;
    //{{AFX_MSG(CChangeTag)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    afx_msg void OnChangeTag();
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateOK(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CFindTagDlg : public CDialog
  {
  public:
    CFindTagDlg(CWnd* pParent = NULL);   // standard constructor
    ~CFindTagDlg();
    CString sFindTag;  //Tag selected to be found
    BOOL bTrendFind;   //search trends
    static BOOL bPrevAutoAccess;
    //{{AFX_DATA(CFindTagDlg)
	enum { IDD = IDD_FINDTAGDLG };
    CComboBox m_Tag2Combo;
    CComboBox m_TagCombo;
    BOOL m_AutoAccess;
    BOOL m_ZoomPrompt;
    BOOL m_MoveCursor;
	BOOL	m_RegExpOn;
	//}}AFX_DATA
    //{{AFX_VIRTUAL(CFindTagDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CCustomToolTipCtrl* pTTC;
    //{{AFX_MSG(CFindTagDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnFind();
    afx_msg void OnFind2();
    virtual void OnOK();
	afx_msg void OnSetfocusFindtagcombo();
	afx_msg void OnSetfocusFindtagcombo2();
	//}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CCopyTagBlkDlg : public CDialog
  {
  public:
    CCopyTagBlkDlg(CExecObj* EO, char* ModelClass, char* Tag=NULL, CWnd* pParent = NULL);
    virtual ~CCopyTagBlkDlg();
    //{{AFX_DATA(CCopyTagBlkDlg)
    enum { IDD = IDD_COPYBLKDLG };
    CListBox PreviewList;
    CComboBox m_UnitsCombo;
    CComboBox m_ModelCombo;
    CCheckListBox LBlksList;
    CCheckListBox RBlksList;
    CListBox OtherUnitsList;
    CSelectTreeCtrl m_CopyTree;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCopyTagBlkDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    static CTagTrees TagTrees;
    Strng      sModelClass;     //model class/type for transfer
    Strng      sTag;            //selected tag
    CExecObj*  pEO;             //Executive object to use for get/set tags
    //AccNodeWnd& AW;
    WORD       bShowDefaults:1, //
               bShowMore:1;     //show wider window with more details
    int        iWinMoreWidth;   //amount by which to make window wider/narrower
    int        iSrc;            //
    int        iDst;            //
    CSVector   NewBlocks;       //list of new blocks
    CWordArray NewBlocksFlags;  //flags for list of new blocks
    CImageList TagDataImgList;  //
    CCustomToolTipCtrl* pTTC;

    flag BuildTree();
    void BldTreeSelList(HTREEITEM h, char* TagPrefix, int& Cnt, CSVector& List);
    int BuildTreeSelList(CSVector& List, char* pRootTag = NULL);
    void BuildUnitsList();
    void BuildOtherUnitsList();
    void BuildBlockList();
    void BuildPreviewList();
    flag UpdateDescriptors();
    //AccDataBlk* CopyFromNode();
    //void CopyToNode(AccDataBlk * pData);

    //{{AFX_MSG(CCopyTagBlkDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnCpySelectedSrc();
    afx_msg void OnCpyBlocksSrc();
    afx_msg void OnCpyBlocksDst();
    afx_msg void OnCpyDatabaseSrc();
    afx_msg void OnCpyOtherDst();
    virtual void OnOK();
    afx_msg void OnNewBlock();
    afx_msg void OnDeleteBlock();
    afx_msg void OnMakeDefaultBlock();
    afx_msg void OnTransfer();
    afx_msg void OnSelchangeCpymodel();
    afx_msg void OnSelchangeCpyunit();
    afx_msg void OnSelchangeCpyrblkslist();
    afx_msg void OnSelchangeCpyotherunitslist();
    afx_msg void OnSelchangeCpylblkslist();
    afx_msg void OnMoreLess();
    afx_msg void OnSelectAll();
    //}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTreeSelectedChanged();
    afx_msg void OnCheckChangeRBlkList();
    afx_msg void OnUpdateCpyDelete(CCmdUI* pCmdUi);
    afx_msg void OnUpdateTransfer(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

