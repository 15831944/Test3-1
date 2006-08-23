#if !defined(AFX_MDLVALUE_H__43155910_ADD8_11D1_BA40_0000E82A5623__INCLUDED_)
#define AFX_MDLVALUE_H__43155910_ADD8_11D1_BA40_0000E82A5623__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MdlValue.h : header file
//

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "MdlValue.h"
#include "executiv.h"
#include "gpwfuncs.h"
#include "resource.h"

class CVarTagItem
  {
  public:
    Strng        m_sLabel;
    Strng        m_sTag;
    FmtAttribute m_Fmt;
    CnvAttribute m_Cnv;
  };
typedef CArray <CVarTagItem, CVarTagItem&> CVarTagArray;

/////////////////////////////////////////////////////////////////////////////
// CMdlValueSet dialog

class CMdlValueSet : public CDialog
{
protected:
  MdlAction Action;
  Strng sTag;
  int iX, iY;
// Construction
public:
//	CMdlValueSet(CWnd* pParent = NULL);   // standard constructor
  CMdlValueSet(char * pTag, MdlAction & Act, int X, int Y, CWnd* pParent =NULL);

// Dialog Data
	//{{AFX_DATA(CMdlValueSet)
	enum { IDD = IDD_MDLVALUESET };
	CSliderCtrl	m_Slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMdlValueSet)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMdlValueSet)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck1();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  static void Add(char *pTag, MdlAction & Act, int X, int Y, CWnd* pParent =NULL);
  static void Clear();
  
  static void StartUp();
  static void ShutDown();
};

/////////////////////////////////////////////////////////////////////////////
// CMdlValueShow dialog

const int MdlValueShowMaxTags  = 20;
const int MdlValueShowMaxItems = 200;
const int MdlValueShowMaxStatusBlks=10;
class CMdlValueShow : public CDialog
{

  // Construction
public:
//	CMdlValueShow(CWnd* pParent = NULL);   // standard constructor
  CMdlValueShow(char * pTag, /*MdlAction & Act,*/ int X, int Y, bool Pinned, CWnd* pParent =NULL);

  // Dialog Data
	//{{AFX_DATA(CMdlValueShow)
	enum { IDD = IDD_MDLVALUESHW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMdlValueShow)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMdlValueShow)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	//}}AFX_MSG
  afx_msg LRESULT OnUpdateWnd(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
  void AtStart(char * pTag, int X, int Y, bool Pinned);
  void DoTheUpdate(); 
  void DoTheUpdateByMsg(); 
  bool AcceptNewTag(char * pTag, char * pObjClassId);
  void SetReloadTagsList() { m_bMustLoadTags=true; };
  static void Add(char *pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent =NULL);
  static void TryAdd(char *pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent =NULL);
  static bool Clear(bool All=true);
  static void TryClear(bool All=true);
  static void UpdateAll();
  static void DeleteItem(char * pObjClassId, int Index);
  //static void AddItem(char * pObjClassId, int Index, char * pRefTag, CnvAttribute &Cnv, FmtAttribute &Fmt);
  static void AddItem(TagInfoBlk & IB);//, int Index);
  static flag FindItem(char * pObjClassId, int Index, CVarTagItem & TagItem);
    //Strng & RefTag, CnvAttribute &Cnv, FmtAttribute &Fmt);
//        Item.Set("V%i",AddIndex);
  static void StartUp();
  static void ShutDown();

protected:
  long           m_nTags;
  Strng          m_sObjClassId;
  Strng          m_sTag[MdlValueShowMaxTags];
  bool           m_bMustLoadTags;
  CVarTagArray   m_TagItems;
  int            m_iSequence;

  int            m_iX, m_iY;
  Strng          m_TagStr;
  Strng          m_CnvStr;
  Strng          m_DataStr[MdlValueShowMaxTags];
  Strng          m_CondStr[MdlValueShowMaxTags];

  bool           m_StateShw[MdlValueShowMaxStatusBlks];
  char           m_StateCh[MdlValueShowMaxTags][MdlValueShowMaxStatusBlks];
  long           m_StateCol[MdlValueShowMaxTags][MdlValueShowMaxStatusBlks];

  int            m_nLines;
  bool           m_bPinned; 

  long           m_iChartItem;
  bool           m_ItemChartOn[MdlValueShowMaxItems];
  long           m_ItemChartColourIndex;
  byte           m_ItemChartColour[MdlValueShowMaxItems];
  bool           m_ItemChartWithZero[MdlValueShowMaxItems];
  long           m_ItemClientRow[MdlValueShowMaxItems+1];
  long           m_iTagSelected;
  long           m_TagClientCol[MdlValueShowMaxTags+1];
  
  static Strng   m_sDragTag;
  static CMdlValueShow * m_pStartWnd;

  long           m_TWide;
  long           m_DWide;
  long           m_CWide;
  long           m_TotWide;
  long           m_TotDeep;

  bool           m_bAllowMove;
};

/////////////////////////////////////////////////////////////////////////////
// CMdlGraphicShow dialog

const int MdlGraphicShowMaxTags  = 10;
//const int MdlValueShowMaxItems = 200;
//const int MdlValueShowMaxStatusBlks=10;
class CMdlGraphicShow : public CDialog
{

  // Construction
public:
//	CMdlGraphicShow(CWnd* pParent = NULL);   // standard constructor
  CMdlGraphicShow(char * pTag, /*MdlAction & Act,*/ int X, int Y, bool Pinned, CWnd* pParent =NULL);

  // Dialog Data
	//{{AFX_DATA(CMdlGraphicShow)
	enum { IDD = IDD_MDLGRAPHICSHW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMdlGraphicShow)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMdlGraphicShow)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
  afx_msg LRESULT OnUpdateWnd(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
  void            AtStart(char * pTag, int X, int Y, bool Pinned);
  void            DoTheUpdate(); 
  void            DoTheUpdateByMsg(); 
  bool            AcceptNewTag(char * pTag, char * pObjClassId);
  void            SetReloadTagsList() { m_bMustLoadTags=true; };
  static void     Add(char *pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent =NULL);
  static void     TryAdd(char *pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent =NULL);
  static bool     Clear(bool All=true);
  static void     TryClear(bool All=true);
  static void     UpdateAll();
  static void     DeleteItem(char * pObjClassId, int Index);
  //static void     AddItem(char * pObjClassId, int Index, char * pRefTag, CnvAttribute &Cnv, FmtAttribute &Fmt);
  static void     AddItem(TagInfoBlk & IB);//, int Index);
  static flag     FindItem(char * pObjClassId, int Index, CVarTagItem & TagItem);
  static void     StartUp();
  static void     ShutDown();

protected:
  long            m_nTags;
  Strng           m_sObjClassId;
  Strng           m_sTag[MdlGraphicShowMaxTags];
  CMdlGraphic     m_Graphics[MdlGraphicShowMaxTags];
  bool            m_bMustLoadTags;
  int             m_iSequence;

  int             m_iX, m_iY;
  bool            m_bPinned; 

  CPoint          m_MousePt;
  UINT            m_MouseFlags;

  static Strng    m_sDragTag;
  static CMdlGraphicShow * m_pStartWnd;

  bool            m_bAllowMove;
};

/////////////////////////////////////////////////////////////////////////////
// CRptTagLists dialog

class CRptTagLists : public CDialog
  {
// Construction
public:
	CRptTagLists(CWnd* pParent = NULL);   // standard constructor
	~CRptTagLists();

  static void StartUp();
  static void ShutDown();
  static void Show(LPCSTR RqdClass=NULL, flag ForceShow=true);
  static void Hide();
  static void AddItem(TagInfoBlk & IB);//, int Index);
  static void LoadVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags);
  static void SaveVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags, CIArray * pSaveOrder = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRptTagLists)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  void LoadModelType();
  void LoadCtrls();
  void DoShow(LPCSTR RqdClass, LPCSTR Layout, flag ForceShow);
  void DoHide();
  void AddItem(LPCSTR pObjClassId, LPCSTR pRefTag, CnvAttribute &Cnv, FmtAttribute &Fmt);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRptTagLists)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnMovedown();
	afx_msg void OnMoveup();
	virtual void OnOK();
	afx_msg void OnQreport();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeModeltype();
	afx_msg void OnEndlabeleditVartaglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkVartaglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIncfg();
	afx_msg void OnRclickVartaglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusMaxvaluelen();
	afx_msg void OnKillfocusMaxTags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
// Dialog Data
	//{{AFX_DATA(CRptTagLists)
	enum { IDD = IDD_RPTTAGLISTS };
	CButton	m_InCfgCtrl;
	CListCtrl	m_VarTagList;
	CComboBox	m_ModelType;
	CString	m_UnitTagList;
	long	m_MaxValueLen;
	long	m_MaxTags;
	//}}AFX_DATA

  CVarTagArray  m_VarTags;
  int           m_nVarTags;
  CIArray       m_SaveOrder;
  bool          m_ModelTypesLoaded;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDLVALUE_H__43155910_ADD8_11D1_BA40_0000E82A5623__INCLUDED_)
