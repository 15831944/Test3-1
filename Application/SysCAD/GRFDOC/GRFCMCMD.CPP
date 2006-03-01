//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#define  __GRFCMCMD_CPP

#include "sc_defs.h"
#include "syscad.h"
#include "gpfuncs.h"
#include "gpwfuncs.h"
#include "dlgbusy.h"
#include "fixedit.h"
#include "executiv.h"

#include "drwhdr.h"
#include "grf3drw.h"
#include "cmdmenu.h"

#include "grfcmcmd.h"
#include "grfwnd.h"
#include "grfdoc.h"
#include "errorlog.h"

#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

GrfComCmdBlk::GrfComCmdBlk(int ExtraCmds, int ExtraCIS):
    DocCmdBlk(10+ExtraCmds, 0+ExtraCIS)
  {
  //pDoc=NULL;
  pDsp=NULL;
  //pWnd=NULL;
  pGrf=NULL;
  pMdl=NULL;
  EO_Register("RootCmds", EOExec_None, 0, 0);
  }

//---------------------------------------------------------------------------

GrfComCmdBlk::~GrfComCmdBlk()
  {
  EO_DeRegister();
  }

//---------------------------------------------------------------------------

void GrfComCmdBlk::InitForCmd()
  {
  }

//---------------------------------------------------------------------------

void GrfComCmdBlk::OnActivateDoc(flag bActivate)
  {
  };

//---------------------------------------------------------------------------
/*
void GrfComCmdBlk::OpenVNT()
  {
  };
*/
//---------------------------------------------------------------------------

void GrfComCmdBlk::BuildVNT()
  {
  SETVNT("ACTivate",   "Flwsheet_Cmds",      N_A,                1, (CmdFn)DoActivate,     EF_ERS);
  SETVNT("ACTivate",   "Instrument_Cmds",    N_A,                1, (CmdFn)DoActivate,     EF_ERS);
  //SETVNT("LOad",       "DOcument",           AName,              1, (CmdFn)DoLoad,         EF_ERS);
  //SETVNT("SAve",       "DOcument",           AName,              1, (CmdFn)DoSave,         EF_ERS);
  //SETVNT("CLear",      "DOcument",           N_A,                1, (CmdFn)DoClear,        EF_ERS);

  SETCIS(ID_GRF_FlowsheetCommands, "ACTIVATE FLWSHEET_CMDS\r");

  //VERIFY(NCmds<MaxCmds);
  //TRACE("Open Root VNT");
  DocCmdBlk::BuildVNT();
  }

//---------------------------------------------------------------------------

void GrfComCmdBlk::DoActivate()
  {
  switch (When)
    {
    case EX_EXEC:
      {
      CGrfDoc*pGDoc=(CGrfDoc*)pDoc;
      switch (Noun)
        {
        case 1:
          pGDoc->pCurrentCB=&pGDoc->FCB;
          break;  
        case 2:
//          pGDoc->pCurrentCB=&pGDoc->ICB;
          break;  
        }
      /**
      CFrameWnd *pWndFrame= (CFrameWnd *)pWnd->GetParent();
      ASSERT(pWndFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
      SysCAD.m_pMainWnd->PostMessage(WM_USER_ACTCMD, 0, (LONG)pWndFrame);
      **/
      gs_pCmd->VNTChanged();
      TRACE("=================REWRITE GrfComCmdBlk::DoActivate ! ================");
      }
      break;
    }   
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void GrfComCmdBlk::DumpErrors()
  {
  Strng Buff("");
  /*
  while (SFE.RequestError(Buff))
    gs_pCmd->Print("%s\n", Buff.Str());
  */
  }

//===========================================================================

void GrfComCmdBlk::AttachDoc(pDocRoot pDoc_, pGrfComCmdBlk pGrf_, pGrfComCmdBlk pMdl_)
  {
  DocCmdBlk::AttachDoc(pDoc_);
  pGWnd=(CGrfWnd*)pWnd;
  pDsp=&(pGWnd->Dsp);
  pDsp->Open();  // Create Viewports etc. if neccessary
  pDsp->Close();
  
  //dbgpln("GrfComCmdBlk::AttachDoc %#10x %#10x %#10x", pDoc_, pGrf_, pMdl_);

  pGrf=pGrf_;
  pMdl=pMdl_;
  };

//===========================================================================
//
//flag GrfComCmdBlk::ModelsAttached() { return FALSE;};
//
////---------------------------------------------------------------------------
//
//pNodeGrfInfo GrfComCmdBlk::GetNodeDrawings()
//  {
//  LogError("SysCAD", 0, "GetNodeDrawings NOT Implemented");
//  return NULL;
//  }
//
//---------------------------------------------------------------------------

flag GrfComCmdBlk::TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs)
  {
  return True;
  }

//---------------------------------------------------------------------------

flag GrfComCmdBlk::TestSymbolNameUnique(Strng &Sym, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueSym)
  {
  UniqueSym.Set("%s%c?",TRB.BaseTag(),TRB.SepChar());
  return False;
  };

//---------------------------------------------------------------------------

BOOL GrfComCmdBlk::DoReadDocument(const char *pszPathName, FILE* pFile)
  {
  if (!feof(pFile))
    {
    char c=fgetc(pFile);
    while (c=='r')
      {
      CGrfDoc* pGDoc=(CGrfDoc*)pDoc;
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
  };

//---------------------------------------------------------------------------

BOOL GrfComCmdBlk::DoWriteDocument(const char *pszPathName, FILE* pFile)
  {
  return True;
  };

//===========================================================================

