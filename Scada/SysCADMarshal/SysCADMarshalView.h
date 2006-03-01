// SysCADMarshalView.h : interface of the CSysCADMarshalView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSCADMARSHALVIEW_H__4E5472AC_B358_48BD_85D7_09527AAE6600__INCLUDED_)
#define AFX_SYSCADMARSHALVIEW_H__4E5472AC_B358_48BD_85D7_09527AAE6600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSysCADMarshalView : public CView
  {
  protected: // create from serialization only
    CSysCADMarshalView();
    DECLARE_DYNCREATE(CSysCADMarshalView)

  // Attributes
  public:
    CSysCADMarshalDoc* GetDocument();

  // Operations
  public:

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSysCADMarshalView)
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
    virtual ~CSysCADMarshalView();
  #ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
  #endif

  protected:

  // Generated message map functions
  protected:
    //{{AFX_MSG(CSysCADMarshalView)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

#ifndef _DEBUG  // debug version in SysCADMarshalView.cpp
inline CSysCADMarshalDoc* CSysCADMarshalView::GetDocument()
   { return (CSysCADMarshalDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSCADMARSHALVIEW_H__4E5472AC_B358_48BD_85D7_09527AAE6600__INCLUDED_)
