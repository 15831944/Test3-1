// SysCADView.cpp : implementation of the CSysCADView class
//

#include "stdafx.h"
#include "SysCAD.h"

#include "SysCADDoc.h"
#include "SysCADView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysCADView

IMPLEMENT_DYNCREATE(CSysCADView, CView)

BEGIN_MESSAGE_MAP(CSysCADView, CView)
	//{{AFX_MSG_MAP(CSysCADView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysCADView construction/destruction

CSysCADView::CSysCADView()
{
	// TODO: add construction code here

}

CSysCADView::~CSysCADView()
{
}

BOOL CSysCADView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADView drawing

void CSysCADView::OnDraw(CDC* pDC)
{
	CSysCADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADView printing

BOOL CSysCADView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSysCADView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSysCADView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADView diagnostics

#ifdef _DEBUG
void CSysCADView::AssertValid() const
{
	CView::AssertValid();
}

void CSysCADView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSysCADDoc* CSysCADView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSysCADDoc)));
	return (CSysCADDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSysCADView message handlers
