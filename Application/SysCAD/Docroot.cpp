//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#define  __DOCROOT_CPP

#include "sc_defs.h"
#include "syscad.h"
#include "fixedit.h"
#include "project.h"
#include "gpfuncs.h"
#include "gpwfuncs.h"
#include "drwhdr.h"
#include "grf3drw.h"
#include "cmdmenu.h"
#include "docroot.h"
#include "errorlog.h"
#include "mainfrm.h"
#include "scd_wm.h"
#include "wndslct.h"
#include "explorescd.h"
#include "basecmds.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

IMPLEMENT_SERIAL(DocRoot, CDocument, 0 /* schema number*/ )

DocRoot::DocRoot()// :
  //DocCB(10)
  //GCB(0), FCB(0), ICB(0)
  {
  bIsOpen = False;
  //pCurrentCB=NULL;//&GCB;
  //EnvCmds=NULL;
  gs_pCmd->VNTChanged();
  lVNTChkSum=0;
  pNextDoc=NULL;
  AddDoc();
#if USESCDEXPLORER
  CExploreScd::RefreshIt(true);
#else
  CWndSlctWnd::RefreshIt();
#endif
  }

//---------------------------------------------------------------------------

DocRoot::~DocRoot()
  {
  RemoveDoc();
  //gs_pCmd->VNTInvalid()
//  gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
#if USESCDEXPLORER
  CExploreScd::RefreshIt(true);
#else
  CWndSlctWnd::RefreshIt();
#endif
  }

//---------------------------------------------------------------------------

pDocRoot DocRoot::pFirstDoc=NULL; 

//---------------------------------------------------------------------------

void DocRoot::SetDocFirst()
  {
  RemoveDoc();
  AddDoc();
  }

//---------------------------------------------------------------------------

void DocRoot::AddDoc()
  {
  pNextDoc=pFirstDoc;
  pFirstDoc=this;
  }

//---------------------------------------------------------------------------

void DocRoot::RemoveDoc()
  {
  if (pFirstDoc==this)
    pFirstDoc=pFirstDoc->pNextDoc;
  else
    {
    pDocRoot p=pFirstDoc;
    while (p && p->pNextDoc!=this)
      p = p->pNextDoc;
    ASSERT(p);
    p->pNextDoc=pNextDoc;
    }
  }

//---------------------------------------------------------------------------

void DocRoot::OnAttachDoc()
  {
  }

//---------------------------------------------------------------------------

void DocRoot::OnActivate(flag bActivate)
  {
  gs_pCmd->VNTChanged();
  if (bActivate)
    {
    gs_pCmd->SetEnable(true);
    SetDocFirst();
    //gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
    gs_pCmd->SetFocus();
    }
  }

//---------------------------------------------------------------------------

void DocRoot::OpenVNT()
  {
  gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
  }

//---------------------------------------------------------------------------

void DocRoot::SetTitle(LPCTSTR lpszTitle)
  {
  CString s(lpszTitle);
  CDocTemplate* pTempl = GetDocTemplate();
  if (pTempl)
    {
    CString se;
    pTempl->GetDocString(se, CDocTemplate::filterExt);
    bool Grf = (se==".scg");
    int No = 5;
    flag Found = True;
    while (Found)
      {
      Found = False;
      POSITION Pos = pTempl->GetFirstDocPosition();
      while (Pos && !Found)
        {
        CDocument* pDoc = pTempl->GetNextDoc(Pos);
        if (pDoc!=this)
          {
          CString Title = pDoc->GetTitle();
          if (Title.GetLength()==s.GetLength() && _stricmp((const char*)s, (const char*)Title)==0)
            Found = True;
          }
        }
      if (Found)
        {
        CString Ext = "";
        int DotPos = s.ReverseFind('.');
        if (DotPos>=0)
          {
          Ext = s.Mid(DotPos, 256);
          s = s.Left(DotPos);
          }
        if (Grf)
          {
          s.Format("%02d%s", No, "_Flowsheet");
          No += 5;
          }
        else
          {
          int _Pos = s.ReverseFind('_');
          if (_Pos>=0)
            {
            CString ss = s.Mid(_Pos+1, 256);
            s = s.Left(_Pos+1);
            if (ss.GetLength()>0)
              {
              char Buff[32];
              sprintf(Buff, "%d", atoi((const char*)ss) + 1);
              s += Buff;
              }
            else
              s += "1";
            }
          else
            s += "_1";
          }
        s += Ext;
        }
      }
    }
  CDocument::SetTitle((const char*)s);
  }

//---------------------------------------------------------------------------

void DocRoot::SetPathName(LPCTSTR pszPathName, BOOL bAddToMRU/* = TRUE*/)
  {
  Strng T;
  T.FnExt((pchar)pszPathName);
  CDocument::SetPathName(pszPathName, (_stricmp(T() ? T():"", ".spj")==0));
  }

//---------------------------------------------------------------------------

//NBNB: KGA: I am overiding this undocumented function; 
//      3 May 1996 from doccore.cpp; check with future versions of MFC
//      Last checked Feb 1997 with MSVC++/MFC version 4.2
BOOL DocRoot::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
#ifndef _MAC
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
#else
			int iBad = newName.FindOneOf(_T(":"));
#endif
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

#ifndef _MAC
			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
#endif
		}

		if (!ScdApp()->DoPromptFileName(newName,
		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate, false))
			return FALSE;       // don't even attempt to save
	}

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
    /* KGA, DO NOT DELETE !!!
		if (0 lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(newName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
				DELETE_EXCEPTION(e);
			}
			END_CATCH_ALL
		}*/
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);

	return TRUE;        // success
}

//---------------------------------------------------------------------------

BOOL DocRoot::OnNewDocument()
  {
  if (!CDocument::OnNewDocument())
    return False;
  
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }

  if (gs_pPrj->pPrjDoc==NULL && !gs_pPrj->bDoingLoad)//sPrjFile.Length() > 0)
    {
    LogError("SysCAD", 0, "A project must be opened or a new project created.");
    return False;
    }

  /*CString s = GetTitle();
  Strng T = PrjFiles();
  T += (const char*)s;
  GetDocTemplate()->GetDocString(s, CDocTemplate::filterExt);
  T += (const char*)s;
  SetPathName(T());
  Strng T1,T2;
  T1.FnName(T());
  T2.FnExt(T());
  T = T1;
  T += T2;
  SetTitle(T());*/

  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  CDocTemplate* pTemplate = GetDocTemplate();
  CString s;
  bool GrfChangeName = false;
  if (1)
    {
    pTemplate->GetDocString(s, CDocTemplate::filterExt);
    if (s==".scg")
      {
      s = "05_Flowsheet.scg";
      SetTitle(s); //this may alter title;
      GrfChangeName = true;
      //ChooseTitle(this, s);
      //SetTitle(s);
      }

    }
  if (!GrfChangeName)
    {
    s = GetTitle();
    Strng T = (const char*)s;
    pTemplate->GetDocString(s, CDocTemplate::filterExt);
    T += (const char*)s;
    SetTitle(T()); //this may alter title;
    }
  Strng PathN = PrjFiles();
  s = GetTitle();
  PathN += (const char*)s;
  SetPathName(PathN()); //do this AFTER SetTitle

  OnAttachDoc();
  OnActivate(True);

  //gs_pCmd->SetDocForCmds(this);
  //gs_pCmd->ProcessAStr("CLEAR DOCUMENT \r");
  //gs_pCmd->SetDocForCmds(NULL);
  bIsOpen = True;
  return True;
  }

//---------------------------------------------------------------------------

BOOL DocRoot::OnOpenDocument(const char* pszPathName)
  {
  if (gs_pPrj->pPrjDoc==NULL && !gs_pPrj->bDoingLoad)//sPrjFile.Length() > 0)
    {
    LogError("SysCAD", 0, "A project must be opened or a new project created.");
    bIsOpen = True; //ensures document/window is closed properly
    return False;
    }

  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  OnAttachDoc(); // to connect and attach document to cmdblks

  //Strng Cmd;
  //Cmd.Set("LOAD DOCUMENT %s\r",pszPathName);
  //gs_pCmd->ProcessAStr(Cmd());
  OpenDocument(pszPathName);
  bIsOpen = True;

  //OnActivate(True); // to connect and attach document to cmdblks

  //gs_pCmd->SetDocForCmds(NULL);
  return True;
  }

//---------------------------------------------------------------------------

void DocRoot::OnCloseDocument()
  {
  if (bIsOpen)
    {
    gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
    gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!
  
    //KGA : 7/5/96 : document sometimes not active ??? causes cmd to go to wrong document/window!!!
    if (pFirstDoc!=this)
      OnActivate(True); 
    gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!
    //if (gs_pPrj->pPrjDoc)
    //  AfxMessageBox("Are you sure you want to remove the document from the project?");
    //gs_pCmd->ProcessAStr("CLOSE DOCUMENT \r");
    CloseDocument();
    }
  }

//---------------------------------------------------------------------------

BOOL DocRoot::OnSaveDocument(const char* pszPathName)
  {
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  //KGA : 7/5/96 : document sometimes not active ??? causes cmd to go to wrong document/window!!!
  if (pFirstDoc!=this)
    OnActivate(True); 
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  //Check if a SaveAs is using a filename that is allready open in this project...
  const int len = strlen(pszPathName);
  CDocTemplate* pTempl = GetDocTemplate();
  if (pTempl)
    {
    POSITION Pos = pTempl->GetFirstDocPosition();
    while (Pos)
      {
      CDocument* pDoc = pTempl->GetNextDoc(Pos);
      if (pDoc!=this)
        {
        CString PathName = pDoc->GetPathName();
        if (PathName.GetLength()==len && _stricmp((const char*)PathName, pszPathName)==0)
          {
          LogError("SysCAD", LF_Exclamation, "Save As '%s' failed. Document/window allready open!", pszPathName);
          return False;
          }
        }
      }
    }

  //Strng Cmd;
  //Cmd.Set("SAVE DOCUMENT %s\r",pszPathName);
  //gs_pCmd->ProcessAStr(Cmd());
  SaveDocument(pszPathName);//dn.GetBuffer(0));
  return True;
  }

//---------------------------------------------------------------------------

void DocRoot::DeleteContents()
  {
  CDocument::DeleteContents();
  /*
  if (gs_Exec.Busy())
    {
    ErrBox(MB_OK, "Must not be running");
    return False;
    }
  */
  SetModifiedFlag(FALSE);
  gs_pPrj->bDocChanged=1;
 
//  CDocument::OnCloseDocument();      // will close doc down
//  return True;
  }

//---------------------------------------------------------------------------

BOOL DocRoot::OpenDocument(const char* pszPathName)
  {
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }

  SetModifiedFlag(FALSE);
  gs_pPrj->bDocChanged=0;
  
  char Fn[512];
  if (strpbrk(pszPathName, ":\\")==NULL)
    {
    strcpy(Fn, PrjFiles());
    strcat(Fn, pszPathName);
    }
  else
    strcpy(Fn, pszPathName);
  CString Ext;
  VERIFY(GetDocTemplate()->GetDocString(Ext, CDocTemplate::filterExt));
  //GetDocTemplate()->GetDocString(Ext, CDocTemplate::filterExt);
  pchar ext=Ext.GetBuffer(0);
  const int l=strlen(Fn);
  const int el=strlen(ext);
  if (l<=el || (_stricmp(&Fn[l-el], ext)!=0 && Fn[l-el]!='.'))
    strcat(Fn, ext);

  FILE* pFile= fopen(Fn, "rt");
  flag b=(pFile!=NULL);
   
  if (b && !feof(pFile))
    b=ReadDocument(Fn, pFile);

  if (pFile) 
    fclose(pFile);
  if (b)
    {
    SetPathName(Fn);
    //LogNote("Document", 0, "Loaded : %s", Fn); Who really wants to know?
    }
  else
    {
    LogError("Document", 0, "NOT Loaded : %s", Fn);
    }
  return b;
  }

//---------------------------------------------------------------------------

BOOL DocRoot::CloseDocument()
  {
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }
  gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);

  SetModifiedFlag(FALSE);
  gs_pPrj->bDocChanged=1;
 
  CDocument::OnCloseDocument();      // will close doc down
  return True;
  }

//---------------------------------------------------------------------------

BOOL DocRoot::SaveDocument(const char* pszPathName)
  {
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }
  char Fn[512];
  CString TmpFn;
  if (pszPathName==NULL)
    {
    TmpFn=GetTitle();
    pszPathName = TmpFn.GetBuffer(0);
    }
  
  if (strpbrk(pszPathName, ":\\")==NULL)
    {
    strcpy(Fn, PrjFiles());
    strcat(Fn, pszPathName);
    }
  else
    strcpy(Fn, pszPathName);

  CString Ext;
  VERIFY(GetDocTemplate()->GetDocString(Ext, CDocTemplate::filterExt));
  pchar ext=Ext.GetBuffer(0);
  const int l=strlen(Fn);
  const int el=strlen(ext);
  if (l<=el)
    strcat(Fn, ext);
  else if (_stricmp(&Fn[l-el], ext)!=0)
    {
    if (Fn[l-el]=='.')
      Fn[l-el]=0; //"old" or "incorect" extension needs to be replaced
    strcat(Fn, ext);
    }

  FILE* pFile= fopen(Fn, "wt");
  flag b=(pFile!=NULL);

  if (b)
    b=WriteDocument(Fn, pFile);

  if (pFile) 
    fclose(pFile);
  if (b)
    {
    SetPathName(Fn);
    SetModifiedFlag(FALSE);
    gs_pCmd->Print("%s - Saved\n", Fn);
    }
  else
    {
    gs_pCmd->Print("%s - NOT SAVED\n", Fn);
    }
  return b;
  }

//---------------------------------------------------------------------------

BOOL DocRoot::ReadDocument(const char* pszPathName, FILE* pFile)
  {
  return True;
  }

//---------------------------------------------------------------------------

BOOL DocRoot::WriteDocument(const char* pszPathName, FILE* pFile)
  {
  return True;
  }

//---------------------------------------------------------------------------

void DocRoot::UpdateAllViewsByMsg(CView* pSender, LPARAM lHint, CObject* pHint, BOOL PostMsg/*=FALSE*/)
  {// walk through all views
  ASSERT(pSender == NULL || !m_viewList.IsEmpty());
  // must have views if sent by one of them

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
    {
    CView* pView = GetNextView(pos);
    if (pView != pSender)
      {
      if (PostMsg)
        pView->PostMessage(WMU_ONUPDATE, lHint, (LPARAM)pHint);
      else
        pView->SendMessage(WMU_ONUPDATE, lHint, (LPARAM)pHint);
      }
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(DocRoot, CDocument)
  //{{AFX_MSG_MAP(DocRoot)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//===========================================================================

DocCmdBlk::DocCmdBlk(int ExtraCmds, int ExtraCIS) :
    CommandBlk(10+ExtraCmds,0+ExtraCIS)
  {
  pDoc=NULL;
  //pDsp=NULL;
  pWnd=NULL;
  //pGrf=NULL;
  //pMdl=NULL;
  //EO_Register("RootCmds", EOExec_None, 0, 0);
  }

//---------------------------------------------------------------------------

DocCmdBlk::~DocCmdBlk()
  {
  //EO_DeRegister();
  }

//---------------------------------------------------------------------------

void DocCmdBlk::InitForCmd()
  {
  }

//---------------------------------------------------------------------------

void DocCmdBlk::OnActivateDoc(flag bActivate)
  {
  }

//---------------------------------------------------------------------------

void DocCmdBlk::BuildVNT()
  {
  //TRACE("Open Root VNT\n");
  //SETVNT("ACTivate",   "Flwsheet_Cmds",      N_A,                1, (CmdFn)&DocCmdBlk::DoActivate,     EF_ERS);
  //SETVNT("ACTivate",   "Instrument_Cmds",    N_A,                1, (CmdFn)&DocCmdBlk::DoActivate,     EF_ERS);
  SETVNT("NEw",        "DOcument",           N_A,                1, (CmdFn)&DocCmdBlk::DoNew,          EF_ERS);
  SETVNT("LOad",       "DOcument",           AName,              1, (CmdFn)&DocCmdBlk::DoOpen,         EF_ERS);
  SETVNT("SAve",       "DOcument",           AName,              1, (CmdFn)&DocCmdBlk::DoSave,         EF_ERS);
  SETVNT("CLOse",      "DOcument",           N_A,                1, (CmdFn)&DocCmdBlk::DoClose,        EF_ERS);
  //SETVNT("CLEar",      "DOcument",           N_A,                1, (CmdFn)&DocCmdBlk::DoClear,        EF_ERS);

  //VERIFY(NCmds<MaxCmds);
  }

//---------------------------------------------------------------------------
/*
void DocCmdBlk::DoActivate()
  {
  if (When==EX_EXEC)
    {
    CFrameWnd *pWndFrame= (CFrameWnd *)pWnd->GetParent();
    ASSERT(pWndFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
    SysCAD.m_pMainWnd->PostMessage(WM_USER_ACTCMD, 0, (LONG)pWndFrame);
    }
  }
*/
//---------------------------------------------------------------------------

void DocCmdBlk::DoNew()
  {
  //pDocRoot pDoc=(pDocRoot)pDoc;
  if (When==EX_EXEC)
    {
    LogError("SysCAD", 0, "Not Yet Implemented");
    ASSERT(0);
    //pDoc->DoNewDocument(); 
    }
  }

//---------------------------------------------------------------------------

void DocCmdBlk::DoOpen()
  {
  if (When==EX_EXEC)
    pDoc->OpenDocument(gs_pCmd->Token(2)); 
  }

//---------------------------------------------------------------------------

void DocCmdBlk::DoSave()
  {
  if (When==EX_EXEC)
    pDoc->SaveDocument(gs_pCmd->Token(2));
  }

//---------------------------------------------------------------------------

void DocCmdBlk::DoClose()
  {
  if (When==EX_EXEC)
    {
    pDoc->CloseDocument();
    //gs_pCmd->CmdBlkInvalid();
    }
  }

//---------------------------------------------------------------------------

/**
void DocCmdBlk::DoClear()
  {
  if (When==EX_EXEC)
    pDoc->ClearDocument();
  }
**/
//---------------------------------------------------------------------------

BOOL DocCmdBlk::OpenDocument(const char *pszPathName, FILE* pFile)
  {
  if (!feof(pFile))
    {
    char c=fgetc(pFile);
    while (c=='r')
      {
      //pDocRoot pDoc=(pDocRoot)pDoc;
      char what[512], buff[512];
      fscanf(pFile, " %s %s ", what, buff);
      if (strcmp(what, "a")==0)
        {
        }

      c=fgetc(pFile);
      }
    if (c!=EOF)
      ungetc(c, pFile);
    }
  return True;
  }

//---------------------------------------------------------------------------

BOOL DocCmdBlk::SaveDocument(const char *pszPathName, FILE* pFile)
  {
  return True;
  }

//---------------------------------------------------------------------------

void DocCmdBlk::AttachDoc(pDocRoot pDoc_)
  {
  pDoc=pDoc_;
  POSITION pos = pDoc->GetFirstViewPosition();
  CView* pFirstView = pDoc->GetNextView( pos );
  pWnd=pFirstView;
  }

//===========================================================================

//===========================================================================

IMPLEMENT_SERIAL(OleDocRoot, COleLinkingDoc, 0 /* schema number*/ )

OleDocRoot::OleDocRoot()// :
  //DocCB(10)
  //GCB(0), FCB(0), ICB(0)
  {
  bIsOpen = False;
  //pCurrentCB=NULL;//&GCB;
  //EnvCmds=NULL;
  //gs_pCmd->VNTChanged();
  //lVNTChkSum=0;
  //pNextDoc=NULL;
  //AddDoc();
  }

//---------------------------------------------------------------------------

OleDocRoot::~OleDocRoot()
  {
  //RemoveDoc();
  //gs_pCmd->VNTInvalid()
//  gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
  }

//---------------------------------------------------------------------------

//pDocRoot OleDocRoot::pFirstDoc=NULL; 
//
////---------------------------------------------------------------------------
//
//void OleDocRoot::SetDocFirst()
//  {
//  RemoveDoc();
//  AddDoc();
//  }
//
////---------------------------------------------------------------------------
//
//void OleDocRoot::AddDoc()
//  {
//  pNextDoc=pFirstDoc;
//  pFirstDoc=this;
//  }
//
////---------------------------------------------------------------------------
//
//void OleDocRoot::RemoveDoc()
//  {
//  if (pFirstDoc==this)
//    pFirstDoc=pFirstDoc->pNextDoc;
//  else
//    {
//    pDocRoot p=pFirstDoc;
//    while (p && p->pNextDoc!=this)
//      p = p->pNextDoc;
//    ASSERT(p);
//    p->pNextDoc=pNextDoc;
//    }
//  }
//
////---------------------------------------------------------------------------
//
//void OleDocRoot::OnAttachDoc()
//  {
//  }
//
////---------------------------------------------------------------------------
//
void OleDocRoot::OnActivate(flag bActivate)
  {
//  gs_pCmd->VNTChanged();
  if (bActivate)
    {
    gs_pCmd->SetEnable(false);
//    SetDocFirst();
//    //gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
//    gs_pCmd->SetFocus();
    }
  }
//
////---------------------------------------------------------------------------
//
//void OleDocRoot::OpenVNT()
//  {
//  gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
//  }
//
//---------------------------------------------------------------------------

void OleDocRoot::SetTitle(LPCTSTR lpszTitle)
  {
  CString s(lpszTitle);
  CDocTemplate* pTempl = GetDocTemplate();
  if (pTempl)
    {
    flag Found = True;
    while (Found)
      {
      Found = False;
      POSITION Pos = pTempl->GetFirstDocPosition();
      while (Pos && !Found)
        {
        CDocument * pDoc = pTempl->GetNextDoc(Pos);
        if (pDoc!=this)
          {
          CString Title = pDoc->GetTitle();
          if (Title.GetLength()==s.GetLength() && _stricmp((const char*)s, (const char*)Title)==0)
            Found = True;
          }
        }
      if (Found)
        {
        CString Ext = "";
        int DotPos = s.ReverseFind('.');
        if (DotPos>=0)
          {
          Ext = s.Mid(DotPos, 256);
          s = s.Left(DotPos);
          }
        int _Pos = s.ReverseFind('_');
        if (_Pos>=0)
          {
          CString ss = s.Mid(_Pos+1, 256);
          s = s.Left(_Pos+1);
          if (ss.GetLength()>0)
            {
            char Buff[32];
            sprintf(Buff, "%d", atoi((const char*)ss) + 1);
            s += Buff;
            }
          else
            s += "1";
          }
        else
          s += "_1";
        s += Ext;
        }
      }
    }
  COleLinkingDoc::SetTitle((const char*)s);
  }

//---------------------------------------------------------------------------

void OleDocRoot::SetPathName(LPCTSTR pszPathName, BOOL bAddToMRU/* = TRUE*/)
  {
  Strng T;
  T.FnExt((pchar)pszPathName);
  COleLinkingDoc::SetPathName(pszPathName, (_stricmp(T(), ".spj")==0));
  }

//---------------------------------------------------------------------------
/*
//NBNB: KGA: I am overiding this undocumented function; 
//      3 May 1996 from doccore.cpp; check with future versions of MFC
//      Last checked Feb 1997 with MSVC++/MFC version 4.2
BOOL OleDocRoot::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
#ifndef _MAC
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
#else
			int iBad = newName.FindOneOf(_T(":"));
#endif
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

#ifndef _MAC
			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
#endif
		}

		if (!ScdApp()->DoPromptFileName(newName,
		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
			return FALSE;       // don't even attempt to save
	}

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
    // KGA, DO NOT DELETE !!!
		//if (0 lpszPathName == NULL)
		//{
		//	// be sure to delete the file
		//	TRY
		//	{
		//		CFile::Remove(newName);
		//	}
		//	CATCH_ALL(e)
		//	{
		//		TRACE0("Warning: failed to delete file after failed SaveAs.\n");
		//		DELETE_EXCEPTION(e);
		//	}
		//	END_CATCH_ALL
		//}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);

	return TRUE;        // success
}
*/
//---------------------------------------------------------------------------

BOOL OleDocRoot::OnNewDocument()
  {
  if (!COleLinkingDoc::OnNewDocument())
    return False;
  
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }

  if (gs_pPrj->pPrjDoc==NULL && !gs_pPrj->bDoingLoad)//sPrjFile.Length() > 0)
    {
    LogError("SysCAD", 0, "A project must be opened or a new project created.");
    return False;
    }

  /*CString s = GetTitle();
  Strng T = PrjFiles();
  T += (const char*)s;
  GetDocTemplate()->GetDocString(s, CDocTemplate::filterExt);
  T += (const char*)s;
  SetPathName(T());
  Strng T1,T2;
  T1.FnName(T());
  T2.FnExt(T());
  T = T1;
  T += T2;
  SetTitle(T());*/

  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  CString s;
  GetDocTemplate()->GetDocString(s, CDocTemplate::filterExt);
  m_strReqdPathName = PrjFiles();
  m_strReqdPathName += (LPCTSTR)GetTitle();
  m_strReqdPathName += (LPCTSTR)s;

//  OnAttachDoc();
//  OnActivate(True);

  //gs_pCmd->SetDocForCmds(this);
  //gs_pCmd->ProcessAStr("CLEAR DOCUMENT \r");
  //gs_pCmd->SetDocForCmds(NULL);
  bIsOpen = True;
  return True;
  }

//---------------------------------------------------------------------------

BOOL OleDocRoot::OnOpenDocument(const char* pszPathName)
  {
  if (gs_pPrj->pPrjDoc==NULL && !gs_pPrj->bDoingLoad)//sPrjFile.Length() > 0)
    {
    LogError("SysCAD", 0, "A project must be opened or a new project created.");
    bIsOpen = True; //ensures document/window is closed properly
    return False;
    }

  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

//  OnAttachDoc(); // to connect and attach document to cmdblks

  //Strng Cmd;
  //Cmd.Set("LOAD DOCUMENT %s\r",pszPathName);
  //gs_pCmd->ProcessAStr(Cmd());
  //OpenDocument(pszPathName);
  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }
  BOOL Ret=COleLinkingDoc::OnOpenDocument(pszPathName);
  bIsOpen = Ret;

  //OnActivate(True); // to connect and attach document to cmdblks

  //gs_pCmd->SetDocForCmds(NULL);
  return Ret;
  }

//---------------------------------------------------------------------------

void OleDocRoot::OnCloseDocument()
  {
  if (bIsOpen)
    {
    gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
    gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!
  
    //KGA : 7/5/96 : document sometimes not active ??? causes cmd to go to wrong document/window!!!
    //if (pFirstDoc!=this)
    //  OnActivate(True); 
    gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!
    //if (gs_pPrj->pPrjDoc)
    //  AfxMessageBox("Are you sure you want to remove the document from the project?");
    //gs_pCmd->ProcessAStr("CLOSE DOCUMENT \r");
    if (gs_Exec.Busy())
      {
      LogError("SysCAD", 0, "Must not be running");
      }
    SetModifiedFlag(FALSE);
    gs_pPrj->bDocChanged=1;
    COleLinkingDoc::OnCloseDocument();

    }
  }

//---------------------------------------------------------------------------

BOOL OleDocRoot::OnSaveDocument(const char* pszPathName)
  {
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  if (gs_Exec.Busy())
    {
    LogError("SysCAD", 0, "Must not be running");
    return False;
    }
  //KGA : 7/5/96 : document sometimes not active ??? causes cmd to go to wrong document/window!!!
  //if (pFirstDoc!=this)
  //  OnActivate(True); 
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands, call more than once!

  //Check if a SaveAs is using a filename that is allready open in this project...
//  //TODO RESTORE

  /****/
  const int len = strlen(pszPathName);
  CDocTemplate* pTempl = GetDocTemplate();
  if (pTempl)
    {
    POSITION Pos = pTempl->GetFirstDocPosition();
    while (Pos)
      {
      CDocument * pDoc = pTempl->GetNextDoc(Pos);
      if (pDoc!=this)
        {
        CString PathName = pDoc->GetPathName();
        if (PathName.GetLength()==len && _stricmp((const char*)PathName, pszPathName)==0)
          {
          LogError("SysCAD", LF_Exclamation, "Save As '%s' failed. Document/window allready open!", pszPathName);
          return False;
          }
        }
      }
    }
  /***/
  //Strng Cmd;
  //Cmd.Set("SAVE DOCUMENT %s\r",pszPathName);
  //gs_pCmd->ProcessAStr(Cmd());

  return COleLinkingDoc::OnSaveDocument(pszPathName);//dn.GetBuffer(0));
  }

//---------------------------------------------------------------------------

void OleDocRoot::DeleteContents()
  {
  COleLinkingDoc::DeleteContents();
  /*
  if (gs_Exec.Busy())
    {
    ErrBox(MB_OK, "Must not be running");
    return False;
    }
  */
  SetModifiedFlag(FALSE);
  gs_pPrj->bDocChanged=1;
 
//  COleLinkingDoc::OnCloseDocument();      // will close doc down
//  return True;
  }

//---------------------------------------------------------------------------
//
//BOOL OleDocRoot::OpenDocument(const char* pszPathName)
//  {
//  if (gs_Exec.Busy())
//    {
//    LogError("SysCAD", 0, "Must not be running");
//    return False;
//    }
//
//  SetModifiedFlag(FALSE);
//  gs_pPrj->bDocChanged=0;
//  
//  char Fn[512];
//  if (strpbrk(pszPathName, ":\\")==NULL)
//    {
//    strcpy(Fn, PrjFiles());
//    strcat(Fn, pszPathName);
//    }
//  else
//    strcpy(Fn, pszPathName);
//  CString Ext;
//  VERIFY(GetDocTemplate()->GetDocString(Ext, CDocTemplate::filterExt));
//  //GetDocTemplate()->GetDocString(Ext, CDocTemplate::filterExt);
//  pchar ext=Ext.GetBuffer(0);
//  int l=strlen(Fn);
//  int el=strlen(ext);
//  if (l<=el || _stricmp(&Fn[l-el], ext)!=0)
//    strcat(Fn, ext);
//
//  FILE* pFile= fopen(Fn, "rt");
//  flag b=(pFile!=NULL);
//   
//  if (b && !feof(pFile))
//    b=ReadDocument(Fn, pFile);
//
//  if (pFile) 
//    fclose(pFile);
//  if (b)
//    {
//    SetPathName(Fn);
//    //LogNote("Document", 0, "Loaded : %s", Fn); Who really wants to know?
//    }
//  else
//    {
//    LogError("Document", 0, "NOT Loaded : %s", Fn);
//    }
//  return b;
//  }

//---------------------------------------------------------------------------

//BOOL OleDocRoot::CloseDocument()
//  {
//  if (gs_Exec.Busy())
//    {
//    LogError("SysCAD", 0, "Must not be running");
//    return False;
//    }
//  gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
//
//  SetModifiedFlag(FALSE);
//  gs_pPrj->bDocChanged=1;
// 
//  COleLinkingDoc::OnCloseDocument();      // will close doc down
//  return True;
//  }

//---------------------------------------------------------------------------

//BOOL OleDocRoot::SaveDocument(const char* pszPathName)
//  {
//  if (gs_Exec.Busy())
//    {
//    LogError("SysCAD", 0, "Must not be running");
//    return False;
//    }
//  char Fn[512];
//  CString TmpFn;
//  if (pszPathName==NULL)
//    {
//    TmpFn=GetTitle();
//    pszPathName = TmpFn.GetBuffer(0);
//    }
//  
//  if (strpbrk(pszPathName, ":\\")==NULL)
//    {
//    strcpy(Fn, PrjFiles());
//    strcat(Fn, pszPathName);
//    }
//  else
//    strcpy(Fn, pszPathName);
//
//  CString Ext;
//  VERIFY(GetDocTemplate()->GetDocString(Ext, CDocTemplate::filterExt));
//  pchar ext=Ext.GetBuffer(0);
//  int l=strlen(Fn);
//  int el=strlen(ext);
//  if (l<=el || _stricmp(&Fn[l-el], ext)!=0)
//    strcat(Fn, ext);
//
//  FILE* pFile= fopen(Fn, "wt");
//  flag b=(pFile!=NULL);
//
//  if (b)
//    b=WriteDocument(Fn, pFile);
//
//  if (pFile) 
//    fclose(pFile);
//  if (b)
//    {
//    SetPathName(Fn);
//    SetModifiedFlag(FALSE);
//    gs_pCmd->Print("%s - Saved\n", Fn);
//    }
//  else
//    {
//    gs_pCmd->Print("%s - NOT SAVED\n", Fn);
//    }
//  return b;
//  }

//---------------------------------------------------------------------------
//
//BOOL OleDocRoot::ReadDocument(const char* pszPathName, FILE* pFile)
//  {
//  return True;
//  }
//
////---------------------------------------------------------------------------
//
//BOOL OleDocRoot::WriteDocument(const char* pszPathName, FILE* pFile)
//  {
//  return True;
//  }
//
//---------------------------------------------------------------------------

void OleDocRoot::UpdateAllViewsByMsg(CView* pSender, LPARAM lHint, CObject* pHint, BOOL PostMsg/*=FALSE*/)
  {// walk through all views
  ASSERT(pSender == NULL || !m_viewList.IsEmpty());
  // must have views if sent by one of them

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
    {
    CView* pView = GetNextView(pos);
    if (pView != pSender)
      {
      if (PostMsg)
        pView->PostMessage(WMU_ONUPDATE, lHint, (LPARAM)pHint);
      else
        pView->SendMessage(WMU_ONUPDATE, lHint, (LPARAM)pHint);
      }
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(OleDocRoot, COleLinkingDoc)
  //{{AFX_MSG_MAP(OleDocRoot)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

