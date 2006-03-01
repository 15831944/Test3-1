// SysCADDoc.cpp : implementation of the CSysCADDoc class
//

#include "stdafx.h"
#include "SysCAD.h"

#include "SysCADDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysCADDoc

IMPLEMENT_DYNCREATE(CSysCADDoc, CDocument)

BEGIN_MESSAGE_MAP(CSysCADDoc, CDocument)
	//{{AFX_MSG_MAP(CSysCADDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSysCADDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CSysCADDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//      DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISysCAD to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {16C1B041-BA8C-48D2-93F9-BF155BF0E9D9}
static const IID IID_ISysCAD =
{ 0x16c1b041, 0xba8c, 0x48d2, { 0x93, 0xf9, 0xbf, 0x15, 0x5b, 0xf0, 0xe9, 0xd9 } };

BEGIN_INTERFACE_MAP(CSysCADDoc, CDocument)
	INTERFACE_PART(CSysCADDoc, IID_ISysCAD, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysCADDoc construction/destruction

CSysCADDoc::CSysCADDoc()
{
	// TODO: add one-time construction code here

	EnableAutomation();

	AfxOleLockApp();
}

CSysCADDoc::~CSysCADDoc()
{
	AfxOleUnlockApp();
}

BOOL CSysCADDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSysCADDoc serialization

void CSysCADDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSysCADDoc diagnostics

#ifdef _DEBUG
void CSysCADDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSysCADDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSysCADDoc commands
