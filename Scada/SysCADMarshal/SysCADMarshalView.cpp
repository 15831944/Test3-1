// SysCADMarshalView.cpp : implementation of the CSysCADMarshalView class
//

#include "stdafx.h"
#include "SysCADMarshal.h"

#include "SysCADMarshalDoc.h"
#include "SysCADMarshalView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalView

IMPLEMENT_DYNCREATE(CSysCADMarshalView, CView)

BEGIN_MESSAGE_MAP(CSysCADMarshalView, CView)
  //{{AFX_MSG_MAP(CSysCADMarshalView)
  //}}AFX_MSG_MAP
  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalView construction/destruction

CSysCADMarshalView::CSysCADMarshalView()
{
  // TODO: add construction code here

}

CSysCADMarshalView::~CSysCADMarshalView()
{
}

BOOL CSysCADMarshalView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalView drawing

void CSysCADMarshalView::OnDraw(CDC* pDC)
{
  CSysCADMarshalDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalView printing

BOOL CSysCADMarshalView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CSysCADMarshalView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add extra initialization before printing
}

void CSysCADMarshalView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalView diagnostics

#ifdef _DEBUG
void CSysCADMarshalView::AssertValid() const
{
  CView::AssertValid();
}

void CSysCADMarshalView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CSysCADMarshalDoc* CSysCADMarshalView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSysCADMarshalDoc)));
  return (CSysCADMarshalDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSysCADMarshalView message handlers

