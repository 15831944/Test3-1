//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __NEUTRALDLGS_H
#define __NEUTRALDLGS_H

#include "gpwfuncs.h"
#include "scdver.h"
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "neutralcommon.h"
#include "grf3drw.h"

//===========================================================================

class CNeutralExportDBDlg : public CDialog
  {
  public:
    CNeutralExportDBDlg(CWnd* pParent = NULL);   // standard constructor
    enum { IDD = IDD_NEUTRALEXPORTDLG };
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  protected:
    virtual BOOL OnInitDialog();
  	virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedSeparatedbs();
    afx_msg void OnBnClickedWithconfig();
    afx_msg void OnEnChangeGrfdatabase();
    afx_msg void OnBnClickedBrowseneutralpath();
    CEdit m_GrfDatabase;
    CEdit m_MdlDatabase;
    CButton m_SeparateDBs;
    CButton m_WithGraphics;
    CButton m_WithModels;
    CButton m_WithConfig;
    CButton m_WithParameters;

    // Outputs
    CString m_sGrfDatabase;
    CString m_sMdlDatabase;
    bool    m_bWithModels;
    bool    m_bWithGraphics;
    bool    m_bWithConfig;
    bool    m_bWithParameters;
    bool    m_bWithState;
    bool    m_bCreateOnly;

    eScdNDBOptions Options();
    CButton m_CreateOnly;
  };

//===========================================================================

class CNeutralImportDBDlg : public CDialog
  {
  public:
    CNeutralImportDBDlg(CWnd* pParent = NULL);   // standard constructor
    enum { IDD = IDD_NEUTRALIMPORTDLG };
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  protected:
    virtual BOOL OnInitDialog();
  	virtual void OnOK();
    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedSeparatedbs();
    afx_msg void OnEnChangeGrfdatabase();
    afx_msg void OnBnClickedBrowseneutralpath();
    CEdit m_GrfDatabase;
    CEdit m_MdlDatabase;
    CButton m_SeparateDBs;
    CButton m_WithModels;
    CButton m_WithGraphics;
    CButton m_WithConfig;

    // Outputs
    CString m_sGrfDatabase;
    CString m_sMdlDatabase;
    bool    m_bWithModels;
    bool    m_bWithGraphics;
    bool    m_bWithConfig;

    eScdNDBOptions Options();

  };

//===========================================================================

class CNeutralSaveGroupDlg : public CDialog
  {
  public:
    CNeutralSaveGroupDlg(Grf3D_Display *pDsp, CWnd * pParent = NULL);   // standard constructor
    enum { IDD = IDD_NEUTRALSAVEGROUP };
    void            AddTag(LPCTSTR Tag);

    CString         GetDatabaseName();
    CString         GetGroup();
    bool            IsGroupValid();

  protected:
    virtual BOOL    OnInitDialog();
  	virtual void    OnOK();
    virtual void    DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
    void            UpdateGroupList();
    bool            FindGroup(LPCTSTR Name);
    void            DeleteGroup(LPCTSTR Name);
    //void            GetDBName();
    //void            PutDBName();
    void            SetDBNameFull(LPCTSTR Fn);
    void            SetDBNameSymb(LPCTSTR Fn);
    CString         Value2Text(double M);
    double          Text2Value(LPCTSTR S, double Mn=0, double Mx=100);
    double          Text2Value(CEdit & E, double Mn=0, double Mx=100);
    void            ReDrawSymbols();
    void            UpdateCalcdPts(bool UpdateOrgText);    
    void            DoChecks();


  public:
    afx_msg void    OnBnClickedWithconfig();
    afx_msg void    OnBnClickedWithstate();
    afx_msg void    OnEnChangeDatabase();
    afx_msg void    OnBnClickedBrowsedatabase();
    afx_msg void    OnBnClickedCancel();
    afx_msg void    OnEnChangeLmargin();
    afx_msg void    OnEnChangeRmargin();
    afx_msg void    OnEnChangeTmargin();
    afx_msg void    OnEnChangeBmargin();
    afx_msg void    OnCbnSelchangeXorgopts();
    afx_msg void    OnCbnSelchangeYorgopts();
    afx_msg void    OnEnChangeXorigin();
    afx_msg void    OnEnChangeYorigin();
    afx_msg void    OnBnClickedFindoptions();
    afx_msg void    OnBnClickedReplaceoptions();
    afx_msg void    OnBnClickedDoreplace();
    afx_msg void    OnBnClickedResettags();
    afx_msg void    OnBnClickedOk();
    afx_msg void    OnCbnEditupdateGrouplist();
    afx_msg void    OnEnChangeGrpdesc();
    afx_msg void    OnNMDblclkTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnLvnBeginlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnLvnEndlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnLvnItemchangedTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnCbnSelchangeGrouplist();
    afx_msg void    OnBnClickedDeletegrp();
    void            OnPaint();

    CButton         m_WithConfig;
    CButton         m_WithState;
    CEdit           m_editDatabase;
    CString         m_sDatabaseFull; 
    Strng           m_sDatabaseSymb;
    CString         m_sGroupName;
    CString         m_sGroupDesc;
    bool            m_bAllOK;
    
    CDblTriPt       m_Lo;
    CDblTriPt       m_Hi;
    CDblTriPt       m_AlignLo;
    CDblTriPt       m_AlignHi;
    CDblTriPt       m_OrigDig;
    CDblTriPt       m_OrigMan;
    CDblTriPt       m_Origin;


    CStringArray    m_OldTags;
    CStringArray    m_NewTags;
    CComboBox       m_GroupList;
    CEdit           m_GroupDesc;
    CEdit           m_LMargin;
    CEdit           m_RMargin;
    CEdit           m_TMargin;
    CEdit           m_BMargin;
    CComboBox       m_XOrgOpts;
    CComboBox       m_YOrgOpts;
    CStatic         m_SymbolFrame;
    CRect           m_SymRectClient;
    CStatic         m_SizeInfo;
    CEdit           m_XOrigin;
    CEdit           m_YOrigin;

    int             m_iUpdateBusy;
    Grf3D_Display * m_pDsp; 
    CListCtrl       m_TagList;
    CEdit           m_String1;
    CEdit           m_String2;
    CButton         m_OK;
    CStatic         m_Message;
    CButton         m_DeleteGrp;
  };

//===========================================================================

class CNeutralInsertGroupDlg : public CDialog
  {
  public:
    CNeutralInsertGroupDlg(CWnd* pParent = NULL);   // standard constructor
    enum { IDD = IDD_NEUTRALINSERTGROUP };
    bool            CompleteForUse();
    void            ResetAfterImport();
    static void     Reset();

    bool            IsGroupSelected();

    eScdTagFixupRules GetFixupRule();
    CString         GetString1();
    CString         GetString2();
    CString         GetDatabaseName();
    CString         GetGroup();
    CStringArray  * GetOldTags();
    CStringArray  * GetNewTags();

  protected:
    virtual void    DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    bool            DoChecks();
    void            LoadTagList();
    void            FixTagRuleDisp();

  protected:
    virtual BOOL    OnInitDialog();
  	virtual void    OnOK();
    DECLARE_MESSAGE_MAP()

    void            UpdateGroupTree();
    void            SetDBNameFull(LPCTSTR Fn);
    void            SetDBNameSymb(LPCTSTR Fn);

  public:
    afx_msg void    OnEnChangedatabase();
    afx_msg void    OnBnClickedBrowsedatabase();
    afx_msg void    OnCbnSelchangeTagfixrules();
    afx_msg void    OnTvnSelchangedGrouptree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnBnClickedDoreplace();
    afx_msg void    OnBnClickedResettags();
    afx_msg void    OnBnClickedFindoptions();
    afx_msg void    OnBnClickedReplaceoptions();

    CEdit           m_editDatabase;
    CString         m_sDatabaseFull; 
    Strng           m_sDatabaseSymb;
    CStringArray    m_OldTags;
    CStringArray    m_NewTags;
    
    CNeutralGrpDescList m_GroupList;
    CTreeCtrl       m_GroupTree;
    CImageList      m_SlctImgList;
    CStatic         m_Desc;
    CListCtrl       m_TagList;
    CComboBox       m_FixupRule;
    CEdit           m_String1;
    CEdit           m_String2;
    CStatic         m_Message;
    CStatic m_StrNm1;
    CStatic m_StrNm2;
    afx_msg void OnNMDblclkTaglist(NMHDR *pNMHDR, LRESULT *pResult);
  };

//===========================================================================

// CNeutralEditTagDlg dialog

class CNeutralEditTagDlg : public CDialog
{
	DECLARE_DYNAMIC(CNeutralEditTagDlg)

public:
	CNeutralEditTagDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNeutralEditTagDlg();

// Dialog Data
	enum { IDD = IDD_NEUTRALEDITTAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CString m_OldTagName;
  CString m_NewTagName;
  CString m_OldTag;
  CString m_NewTag;
};


#endif

