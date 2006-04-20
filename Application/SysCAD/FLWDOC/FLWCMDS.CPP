//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#define  __FLWCMDS_CPP

#include "sc_defs.h"
#include "syscad.h"
#include "gpfuncs.h"
#include "dlgbusy.h"
#include "fixedit.h"
#include "executiv.h"
#include "tagdb.h"
#include "drwhdr.h"
#include "grf3drw.h"
#include "cmdmenu.h"
#include "sfe_clnt.h"
#include "grfcmds.h"
#include "flwcmds.h"
#include "errorlog.h"
#include "project.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

inline SFEFlwLib * FE() { return gs_pPrj->FlwLib(); };
//===========================================================================

FlwCmdBlk::FlwCmdBlk(int ExtraCmds, int ExtraCIS):
  GrfComCmdBlk(60+ExtraCmds, 10+ExtraCIS)
  {
  //EngineName="";
  DataFmt = "APPEND TAG CNVS ALL ";
  //FE()=NULL;
  }

//---------------------------------------------------------------------------

FlwCmdBlk::~FlwCmdBlk()
  {
  }

//---------------------------------------------------------------------------

void FlwCmdBlk::InitForCmd()
  {
  }
  
//---------------------------------------------------------------------------

void FlwCmdBlk::OnActivateDoc(flag bActivate)
  {
  GrfComCmdBlk::OnActivateDoc(bActivate);
  pGrf->OnActivateDoc(bActivate);
  }

//---------------------------------------------------------------------------

void FlwCmdBlk::BuildVNT()
  {
  SETVNT("HELP",     "MODEL",      N_A,  -1, (CmdFn)&FlwCmdBlk::DoLoadUserDocs, EF_ERS);

  SETCIS(ID_HELP_UserDocs, "HELP MODEL\r");

  GrfComCmdBlk::BuildVNT();
  }

//---------------------------------------------------------------------------

flag FlwCmdBlk::EngineOpen()
  {
  if (FE())
    return True;
  gs_pCmd->Print("No Flow Engine Open\n");
  return False;
  }

//---------------------------------------------------------------------------

void FlwCmdBlk::DoSolve()
  {
  if (!EngineOpen()) 
    return;
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC:
      {
      CWaitCursor Wait;
//      FE()->FE_DoSolveNet();
      break;
      }
    case EX_RESULTS:
      DumpErrors();
      break;
    default:;  
    }  
  pDsp->Close();
  };

//---------------------------------------------------------------------------

void FlwCmdBlk::DoLoadUserDocs()
  {
  if (!EngineOpen()) 
    return;
  switch (When)
    {
    case EX_EXEC:
      {
      if (pMdl && gs_pPrj)
        gs_pPrj->ModelHelp();
      LogWarning("FlwCmds", 0, "No Documentation Found");
      break;  
      }
    }
  }


//---------------------------------------------------------------------------

void FlwCmdBlk::DumpReplies()
  {
  }

//---------------------------------------------------------------------------

void FlwCmdBlk::DumpErrors()
  {
  }

//---------------------------------------------------------------------------

flag FlwCmdBlk::TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs)
  {
  if (pTag==NULL || strlen(pTag)==0)
    return false;
  pDXF_Drawing pDrw=((pGrfCmdBlk)pGrf)->pDrw;
  Entity_List_Info InsInfo;
  Entity_List_Info EntInfo;
  DXF_ENTITY Ins = pDrw->First_Insert(InsInfo);
  while (Ins)
    {
    DXF_ENTITY TAtt = First_Ent(Attr_List(Ins), EntInfo);
    if (TAtt && Attr_Is(TAtt, "Tag"))
      {
      pchar pAttr=Attr_Value(TAtt);
      if (_stricmp(pTag, pAttr) == 0)
        return false; //found tag
      }
    Ins = pDrw->Next_Insert(InsInfo);
    }
  return true;
  }

//---------------------------------------------------------------------------

flag FlwCmdBlk::TestSymbolNameUnique(Strng &Sym, pchar pHdr, char SepChar, Strng &UniqueSym)
  {
  pDXF_Drawing   pDrw=((pGrfCmdBlk)pGrf)->pDrw;
  //Strng KernalName(pHdr);
  //KernalName+=SepChar;
  int unum = pDrw->Blocks.FindUnique(pHdr, -1);
  if (unum>=0)
    UniqueSym.Set("%s%c%d",pHdr,SepChar,unum);
  else
    UniqueSym.Set("%s%c1",pHdr,SepChar);
  return False;
  }

////---------------------------------------------------------------------------
//
//int FlwCmdBlk::AddNodeModel(pchar ModelType, pchar Tag)
//  {
//  if (!EngineOpen()) 
//    return False;
//  return FE()->FE_DoInsert(ModelType, Tag, NULL, NULL);
//  }
//
////---------------------------------------------------------------------------
//
//int FlwCmdBlk::DeleteNodeModel(pchar Tag)
//  {
//  VERIFY(FALSE); //OLD CODE 12/97
//  if (!EngineOpen()) 
//    return False;
//  return FE()->FE_DoDelete(Tag);
//  }
//
////---------------------------------------------------------------------------
//
//int FlwCmdBlk::AddNodeConnect(pchar Type, pchar Tag, pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
//  {
//  if (!EngineOpen()) 
//    return False;
//  Strng Src, Dst;
//  Src.Set("%s.%s", SrcTag, SrcOut);
//  Dst.Set("%s.%s", DstTag, DstIn);
//  return FE()->FE_DoInsert(Type, Tag, Src(), Dst());
//  }
//
////---------------------------------------------------------------------------
//
//int FlwCmdBlk::AddConnect(pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn)
//  {
//  if (!EngineOpen()) 
//    return False;
//  Strng Src, Dst;
//  Src.Set("%s.%s", SrcTag, SrcOut);
//  Dst.Set("%s.%s", DstTag, DstIn);
//  return FE()->FE_DoConnect(Src(), Dst());
//  }
//
//---------------------------------------------------------------------------

BOOL FlwCmdBlk::DoReadDocument(const char *pszPathName, FILE* pFile)
  {
  if (!feof(pFile))
    {
    char c=fgetc(pFile);
    while (c=='f')
      {
      //pGrfDoc pGDoc=(pGrfDoc)pDoc;
      char what[512], buff[512];
      fscanf(pFile, " %s %s ", what, buff);
      //FE()=SFEClientDLL::Attach();//EngineName());
      //if (strcmp(what, "e")==0)
      //  {
      //  //EngineName=strcmp(buff , ".")!=0 ? buff : "";
      //  //SFEClientDLL::Detach();//FE());
      //  FE()=SFEClientDLL::Attach();//EngineName());
      //  //if (FE()==NULL)
      //  //  EngineName="";
      //  }

      c=fgetc(pFile);
      }
    if (c!=EOF)
      ungetc(c, pFile);
    }
  return 1;
  };

//---------------------------------------------------------------------------

BOOL FlwCmdBlk::DoWriteDocument(const char *pszPathName, FILE* pFile)
  {
  fprintf(pFile, "f \n" );//, EngineName.Length()>0 ? EngineName() : ".");
  return 1;
  };

//===========================================================================
