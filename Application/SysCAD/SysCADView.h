// SysCADView.h : interface of the CSysCADView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSCADVIEW_H__1F2FE57A_FD30_4733_AA4F_3F52A21D23CA__INCLUDED_)
#define AFX_SYSCADVIEW_H__1F2FE57A_FD30_4733_AA4F_3F52A21D23CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSysCADView : public CView
{
protected: // create from serialization only
	CSysCADView();
	DECLARE_DYNCREATE(CSysCADView)

// Attributes
public:
	CSysCADDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysCADView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSysCADView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSysCADView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SysCADView.cpp
inline CSysCADDoc* CSysCADView::GetDocument()
   { return (CSysCADDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSCADVIEW_H__1F2FE57A_FD30_4733_AA4F_3F52A21D23CA__INCLUDED_)
