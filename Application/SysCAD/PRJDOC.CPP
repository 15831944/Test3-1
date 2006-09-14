//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#define __PRJDOC_CPP
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "prjdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CPrjDoc, CDocument)

CPrjDoc::CPrjDoc()
  {
  bAllowClose = 0;
  bOnCloseBusy = 0;
  }

//---------------------------------------------------------------------------

CPrjDoc::~CPrjDoc()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPrjDoc, CDocument)
  //{{AFX_MSG_MAP(CPrjDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

#ifdef _DEBUG
void CPrjDoc::AssertValid() const
  {
  CDocument::AssertValid();
  }

void CPrjDoc::Dump(CDumpContext& dc) const
  {
  CDocument::Dump(dc);
  }
#endif //_DEBUG

//---------------------------------------------------------------------------

BOOL CPrjDoc::OnNewDocument()
  {
  if (!CDocument::OnNewDocument())
    {
    bAllowClose = 1;
    return FALSE;
    }
  if (gs_pPrj->pPrjDoc)
    gs_pPrj->bDocChanged = True;

  gs_pPrj->OnNew(); //if this fails, gs_pPrj->bCmdSuccess is set to false
  
  if (gs_pPrj->bCmdSuccess)
    {
    gs_pPrj->pPrjDoc = (CDocument*)this;
    Strng T = PrjFile();
    SetPathName(T());
    Strng T2;
    T.FnName(PrjFile());
    T2.FnExt(PrjFile());
    T += T2;
    SetTitle(T());

    CAutoIncDec AID(gs_FileNewFlag);
    if (gs_pPrj->m_fNewTrend)
      ScdApp()->TrendTemplate().OpenDocumentFile(NULL); 
    if (gs_pPrj->m_fNewGraph)
      ScdApp()->GraphTemplate().OpenDocumentFile(NULL); 
    
    //gs_pPrj->OnClose();
    // This will do actual save
    gs_pPrj->DoSave(false, NULL, false);
    if (gs_pPrj->bCmdSuccess)
      {
      ScdPFUser.WrStr("General", "RecentProject", PrjFile());
      ScdApp()->AddToRecentFileList(PrjFile());
      ScdApp()->StdProfileChanged();
      }
    }
  else
    bAllowClose = 1;
  return gs_pPrj->bCmdSuccess;
  }

//---------------------------------------------------------------------------

BOOL CPrjDoc::OnOpenDocument(LPCTSTR lpszPathName) 
  {
  Strng Ext;
  Ext.FnExt((char*)lpszPathName);
  if (Ext.XStrICmp(".spj")!=0)
    {
    LogError("SysCAD", 0, "Unknown file type '%s'", lpszPathName);
    bAllowClose = 1;
    return FALSE;
    }

  if (!CDocument::OnOpenDocument(lpszPathName))
    {
    bAllowClose = 1;
    return FALSE;
    }

  if (gs_pPrj->pPrjDoc)
    gs_pPrj->bDocChanged = True;
  gs_pPrj->OnOpen((char*)lpszPathName); //if this fails, gs_pPrj->bCmdSuccess is set to false
  if (gs_pPrj->bCmdSuccess)
    gs_pPrj->pPrjDoc = (CDocument*)this;
  else
    {
    //gs_pPrj->pPrjDoc = NULL;
    bAllowClose = 1;
    }
  return gs_pPrj->bCmdSuccess;
  }

//---------------------------------------------------------------------------

BOOL CPrjDoc::OnSaveDocument(LPCTSTR lpszPathName) 
  {
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }

  if (lpszPathName && !gs_pPrj->bForceSaveAsNV && (!gs_pPrj->bDoingSaveAs || gs_pPrj->bDoingSaveAsWithName ))
    {
    Strng Label;
    gs_pPrj->OnSave((char*)lpszPathName);//, Label());
    }
  else
    gs_pPrj->OnSave(NULL);//, NULL);
  return gs_pPrj->bCmdSuccess;
  }

//---------------------------------------------------------------------------

void CPrjDoc::OnCloseDocument() 
  {
  if (gs_pPrj->pPrjDoc && gs_pPrj->pPrjDoc==this)
    {
    bOnCloseBusy = 1;
    CDocument* pOldDoc = gs_pPrj->pPrjDoc;
    if (gs_pPrj->OnClose())
      {
      gs_pPrj->bDocChanged = True;
      bOnCloseBusy = 0;
      CDocument::OnCloseDocument();
      gs_pPrj->pPrjDoc = NULL;
      }
    else
      {
      if (gs_pPrj->pPrjDoc==NULL)
        gs_pPrj->pPrjDoc = pOldDoc;
      bOnCloseBusy = 0;
      }
    }
  else if (bAllowClose)
    {
    if (gs_pPrj->pPrjDoc==NULL)
      gs_pPrj->CloseForFailedOpen();
    CDocument::OnCloseDocument();
    }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
