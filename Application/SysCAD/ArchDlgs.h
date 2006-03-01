#if !defined(AFX_CAddToArchive_H__66B67656_D8BE_4357_8B35_53EE603A215C__INCLUDED_)
#define AFX_CAddToArchive_H__66B67656_D8BE_4357_8B35_53EE603A215C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAddToArchive.h : header file
//

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __ARCHMNGR_H
#include "archmngr.h"
#endif
#ifndef __SCDCTRLS_H
#include "scdctrls.h"
#endif


/////////////////////////////////////////////////////////////////////////////
// CAddToArchive dialog

class CAddToArchive : public CDialog
{
// Construction
public:
  CAddToArchive(TagInfoBlk & IB, CArchiveManager * pAM, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAddToArchive)
  enum { IDD = IDD_ARCH_ADD };
  CButton m_OnAdd;
  CListCtrl m_TableList;
  BOOL  m_bAverage;
  BOOL  m_bCount;
  BOOL  m_bCurrent;
  BOOL  m_bMaximum;
  BOOL  m_bMinimum;
  CString m_sTag;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAddToArchive)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  CArchiveManager  *m_pAM;
  byte              m_iType;
  CCnvIndex         m_iCnv;
  Strng             m_sCnvTxt;
 
  // Generated message map functions
  //{{AFX_MSG(CAddToArchive)
  virtual BOOL OnInitDialog();
  afx_msg void OnClickArctables(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnAdd();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


//------------------------------------------------------------------------

_FWDDEF(CArchiveSheet)
class CArchiveSheet : public CPropertySheet
  {
  DECLARE_DYNAMIC(CArchiveSheet)
  public:
    CArchiveSheet(CArchiveManager * pAM, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CArchiveSheet(CArchiveManager * pAM, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CArchiveSheet();
    void UpdateStats();
    void Stop();
    void DisplayTag(char* pGotoTag);
    //{{AFX_VIRTUAL(CArchiveSheet)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL
  protected:
    CArchiveManager * m_pAM;

    void DeletePages();
    //{{AFX_MSG(CArchiveSheet)
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnNcDestroy();
    //}}AFX_MSG
    //afx_msg LRESULT OnUpdateStats(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    flag bInited;
  };

//------------------------------------------------------------------------

class CArcTablesPage : public CPropertyPage
  {
  friend class CArchiveManager;

  DECLARE_DYNCREATE(CArcTablesPage)
  public:
    flag bDidInit:1,
         bTbTreeBuilt:1;
    CImageList m_TbImgList;

    CArcTablesPage();
    ~CArcTablesPage();

    //{{AFX_DATA(CArcTablesPage)
	enum { IDD = IDD_ARC_TABLES_PAGE };
    CCustomTreeCtrl m_TbTree;
	//}}AFX_DATA
    //{{AFX_VIRTUAL(CArcTablesPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL
  protected:
    CArchiveManager * m_pAM;

    void RebuildTbTree();
    virtual void OnOK();
    //{{AFX_MSG(CArcTablesPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnRclickTabtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeydownTabtree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateBtns(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };

//------------------------------------------------------------------------

class CArcInputPage : public CPropertyPage
  {
  friend class CArchiveManager;

  DECLARE_DYNCREATE(CArcInputPage)
  public:
    CArcInputPage();
    ~CArcInputPage();
    flag bDidInit:1,
         bSDTreeBuilt:1;
    CImageList m_SDImgList;

    //{{AFX_DATA(CArcInputPage)
    enum { IDD = IDD_ARC_INPUT_PAGE };
    CCustomTreeCtrl m_SDTree;
  //}}AFX_DATA
    //{{AFX_VIRTUAL(CArcInputPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CArchiveManager * m_pAM;
    
    void RebuildSDTree();
    virtual void OnOK();
    virtual void OnCancel();
    //{{AFX_MSG(CArcInputPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnAddinput();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//------------------------------------------------------------------------

extern CArchiveSheet* pArcSheet;   //pointer to driver tabbed dialog options box

//------------------------------------------------------------------------


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAddToArchive_H__66B67656_D8BE_4357_8B35_53EE603A215C__INCLUDED_)
