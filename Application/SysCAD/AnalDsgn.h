#if !defined(AFX_ANALDSGN_H__2E7A94E7_A52D_11D2_BCBE_0000E82A5623__INCLUDED_)
#define AFX_ANALDSGN_H__2E7A94E7_A52D_11D2_BCBE_0000E82A5623__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnalDsgn.h : header file
//

#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __SLCTWND_H
  #include "selctwnd.h"
#endif
#ifndef __HELPINFO_H
  #include "helpinfo.h"
#endif
/////////////////////////////////////////////////////////////////////////////
// CAnalyseDesign dialog

class CAnalyseDesign : public CDialog
{
// Construction
public:
	CAnalyseDesign(CWnd* pParent = NULL);   // standard constructor
	~CAnalyseDesign();

  CImageList FlwTreeImgList;
// Dialog Data
	//{{AFX_DATA(CAnalyseDesign)
	enum { IDD = IDD_ANALYSEDESIGN };
	CTreeCtrl	m_NodeTree;
	CTreeCtrl	m_TypeTree;
	CString	m_Status;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalyseDesign)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CCustomToolTipCtrl* pTTC;
  int                 m_ADIndex;
//  CArray <Strng, Strng&> SelectedTags;
  Strng_List          SelectedTags;

  BOOL  BuildTypesTree();
  int   GetSelectList();
  int   SendSlctList2ADObj();
  int   GetADObjStatus();
  int   GetADObjTree();
  void  LoadSelectedNodes();
  void  DoTheWork();
  
  // Generated message map functions
	//{{AFX_MSG(CAnalyseDesign)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusTypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandedTypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefresh();
	//}}AFX_MSG
  afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
//  afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );

//  virtual void OnActivateFrame( UINT nState, CFrameWnd* pFrameWnd );

  
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALDSGN_H__2E7A94E7_A52D_11D2_BCBE_0000E82A5623__INCLUDED_)
