//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __GRFDOC_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "grf3drw.h"
#include "grfdoc.h"
#include "grfwnd.h"
#include "project.h"
#include "licbase.h"
#include "DBHelper.h"
#include "grfcmds.h"
#include "flwcmds.h"
#include "neutraldlgs.h"
#include "neutralgrf.h"
#include "neutralmdl.h"

// for GUID's
#ifndef __RPC_H__
  #include <rpc.h>
#endif
#ifndef __RPCDCE_H__
  #include <rpcdce.h>
#endif

#pragma comment (lib, "rpcrt4.lib")


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//===========================================================================

IMPLEMENT_SERIAL(CGrfDoc, DocRoot, 0 /* schema number*/ )

CGrfDoc::CGrfDoc() :
  GCB(0, 32), FCB(0,0)//, ICB(0,0)
  {
  pCurrentCB = NULL;
  bModelsActive = true;

  // Generate DefaultGuid
  UCHAR*psDocGUID=NULL;
  UUID DocGUID;

  ASSERT_ALWAYS(UuidCreate(&DocGUID)==RPC_S_OK, "Guid Not Created");
  ASSERT_ALWAYS(UuidToString(&DocGUID, &psDocGUID)==RPC_S_OK, "Guid Not Converted");

  m_sDocGUID.Set("{%s}", psDocGUID);
  
  RpcStringFree(&psDocGUID);
  }

//---------------------------------------------------------------------------

CGrfDoc::~CGrfDoc()
  {
  //gs_pCmd->CloseCommandBlksTo(gs_pBaseCmds);
  }

//---------------------------------------------------------------------------

void CGrfDoc::OnAttachDoc()
  {
  DocRoot::OnAttachDoc();

  GCB.AttachDoc(this, &GCB, pCurrentCB);
  FCB.AttachDoc(this, &GCB, &FCB);
//  ICB.AttachDoc(this, &GCB, &ICB);
  }

//---------------------------------------------------------------------------

void CGrfDoc::OnActivate(flag bActivate)
  {
  OnAttachDoc();
  DocRoot::OnActivate(bActivate);
  if (bActivate)
    {
    if (pCurrentCB==&FCB)
      {
      FCB.OnActivateDoc(bActivate); 
      }
    else
      {
      GCB.OnActivateDoc(bActivate);
      }
    }
  }

//---------------------------------------------------------------------------

void CGrfDoc::OpenVNT()
  {
  DocRoot::OpenVNT();
  gs_pCmd->OpenCommandBlk(&GCB);
  if (pCurrentCB)
    gs_pCmd->OpenCommandBlk(pCurrentCB);
  }

//---------------------------------------------------------------------------

BOOL CGrfDoc::ReadDocument(const char* pszPathName, FILE* pFile)
  {
  GCB.AttachDoc(this, &GCB, pCurrentCB);
  FCB.AttachDoc(this, &GCB, &FCB);
//  ICB.AttachDoc(this, &GCB, &ICB);

//  SetModifiedFlag(FALSE);
//  Prj.bDocChanged=0;
  if (!feof(pFile))
    {
    char c=fgetc(pFile);
    while (c=='d')
      {
      char what[512], buff[512];
      fscanf(pFile, " %s %s ", what, buff);
      if (strcmp(what, "a")==0)
        if (strcmp(buff, "FLW")==0)
          pCurrentCB=&FCB;
//        else if (strcmp(buff, "INS")==0)
//          pCurrentCB=&ICB;
        else
          pCurrentCB=NULL;

      c=fgetc(pFile);
      }
    while (c=='g')
      {
      char buff[512];
      fscanf(pFile, " %s ", buff);
      m_sDocGUID=buff;
      c=fgetc(pFile);
      }
    if (c!=EOF)
      ungetc(c, pFile);
    }

  flag b=1;
  if (b) b=GCB.DoReadDocument(pszPathName, pFile);
  if (b) b=FCB.DoReadDocument(pszPathName, pFile);
//  if (b) b=ICB.DoReadDocument(pszPathName, pFile);
  return b;
  }

//---------------------------------------------------------------------------

BOOL CGrfDoc::WriteDocument(const char* pszPathName, FILE* pFile)
  {
  char buff[256];
  strcpy(buff, "GRF");
  if (pCurrentCB==&FCB)
    strcpy(buff, "FLW");
//  else if (pCurrentCB==&ICB)
//    strcpy(buff, "INS");
  fprintf(pFile, "d a %s\n", buff);
  fprintf(pFile, "g %s\n", m_sDocGUID());
  
  flag b = 1;
  if (b) b=GCB.DoWriteDocument(pszPathName, pFile);
  if (b) b=FCB.DoWriteDocument(pszPathName, pFile);
//  if (b) b=ICB.DoWriteDocument(pszPathName, pFile);
  return b;
  }

//---------------------------------------------------------------------------

void CGrfDoc::SetPathName( const char* pszPathName )
  {
  DocRoot::SetPathName(pszPathName);
  char T[256], t[256];
  strcpy(T, pszPathName);
  char *p=strrchr(T, '.');
  if (p) *p=0;
  
  POSITION pos = GetFirstViewPosition();
  CView* pFirstView = GetNextView( pos );
  CGrfWnd* pWnd=(CGrfWnd*)pFirstView;

  strcpy(t, T);
  strcat(t, ".WND");
  pWnd->EO_ChangeName(t);
  strcpy(t, T);
  strcat(t, ".GRF");
  GCB.EO_ChangeName(t);
  strcpy(t, T);
  strcat(t, ".FLW");
  FCB.EO_ChangeName(t);
//  strcpy(t, T);
//  strcat(t, ".INS");
//  ICB.EO_ChangeName(t);
  }

//---------------------------------------------------------------------------

BOOL CGrfDoc::OnNewDocument()
  {
  if (!DocRoot::OnNewDocument())
    return False;
  if (gs_pPrj && !gs_pPrj->bDoingLoad)
    {
    if (gs_License.GraphicWindowsAllowed()!=CK_InfiniteGrfs && CWindowLists::GetGrfWndCount()>gs_License.GraphicWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Cannot open new graphics window, license allows maximum of %d", gs_License.GraphicWindowsAllowed());
      return False;
      }
    }

  //KGA 6/6/96: Assume that flowsheet cmds and ONE model engine is required.
  //In the future, when this changes a dialog box can be placed here to
  //let the user choose which cmds must be opened and model engine/library dll
  //must be loaded.
  Strng Cmd("ACTIVATE FLWSHEET_CMDS \r");
  gs_pCmd->ProcessAStr(Cmd());
  if (CfgFile())
    {
    Cmd.Set("OPEN ENGINE %s \r", CfgFile());
    gs_pCmd->ProcessAStr(Cmd());
    }

#if WITHGRFDOCFRAME
  const int DelimitPrefixLen=8;
  if (gs_pPrj->m_sGrfFrameName.XStrICmp("None")==0)
    {
    }
  else if (gs_pPrj->m_sGrfFrameName.XStrNICmp("Extents-", DelimitPrefixLen)==0)
    {
    struct CPt {int XSgn, YSgn, XPt, YPt;} Pts[]=
      {
        {+1, +1,   0,   0},
        {+1, -1,   0, 297},
        {-1, -1, 419, 297},
        {-1, +1, 419,   0},
      };
    struct CScl {LPCTSTR Nm; float Scl;} Scls[]=
      {
        {"A5", 0.5f},
        {"A4", 0.7071f},
        {"A3", 1.0f},
        {"A2", 1.4142f},
        {"A1", 2.0f},
        {0}
      };
    int Len=10;
    float Scl=1;
    for (int i=0; Scls[i].Nm; i++)
      if (stricmp(gs_pPrj->m_sGrfFrameName()+DelimitPrefixLen, Scls[i].Nm)==0)
        {
        Scl=Scls[i].Scl;
        break;
        }

    for (int i=0; i<4; i++)
      {
      CPt &P=Pts[i];
      C3_CURVE TheCurve = NULL;
      PT3 Pos;
      Pos[0]=P.XPt*Scl;
      Pos[1]=P.YPt*Scl+P.YSgn*Len;
      Pos[2]=0;
      TheCurve = GCB.pDrw->Add_PLine_Start(Pos);
      Pos[0]=P.XPt*Scl;
      Pos[1]=P.YPt*Scl;
      Pos[2]=0;
      GCB.pDrw->Add_PLine_Vertex(TheCurve, Pos);
      Pos[0]=P.XPt*Scl+P.XSgn*Len;
      Pos[1]=P.YPt*Scl;
      Pos[2]=0;
      GCB.pDrw->Add_PLine_Vertex(TheCurve, Pos);
      }

    gs_pCmd->ProcessAStr("zoom view all\r");
    }
  else
    {
    Strng GrpLib(gs_pPrj->m_sDefGrpLib);
    GrpLib.FnExpand();

    CNeutralImportExport NImport;
    NImport.SetImportOrigin(CDblTriPt(0,0,0), CDblTriPt(1,1,1));
    NImport.SetImportFilter(gs_pPrj->m_sGrfFrameName());
    NImport.SetImportTagFixups(eFixup_ReplaceTag, "Frame", "", NULL, NULL);
    bool OK=NImport.DoImportGroup(eNDB_Graphics, this, GrpLib(), GrpLib());
    if (OK)
      gs_pCmd->ProcessAStr("zoom view all\r");
    else
      LogWarning("Graphics Doc", 0, "Frame %s not Inserted", gs_pPrj->m_sGrfFrameName()); 
    }
#endif
  return True;
  }

//---------------------------------------------------------------------------

BOOL CGrfDoc::OnOpenDocument(const char* pszPathName)
  {
  if (gs_pPrj && !gs_pPrj->bDoingLoad)
    {
    if (gs_License.GraphicWindowsAllowed()!=CK_InfiniteGrfs && CWindowLists::GetGrfWndCount()>gs_License.GraphicWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Cannot open graphics window, license allows maximum of %d", gs_License.GraphicWindowsAllowed());
      bIsOpen = True; //ensures document/window is closed properly
      return False;
      }
    }
  return DocRoot::OnOpenDocument(pszPathName);
  }

//---------------------------------------------------------------------------

int CGrfDoc::GetTagList(Strng_List & Lst) 
  { 
  Lst.Clear();
  GCB.pDrw->CollectTags(Lst);
  return Lst.Length();
  }

//---------------------------------------------------------------------------

int CGrfDoc::GetTagListInfo(bool AllInserts, CGrfTagInfoArray & GTIA) 
  {
  GTIA.SetSize(0, 64);
  GCB.pDrw->CollectTagInfo(AllInserts, GTIA);
  return GTIA.GetSize();
  }

//---------------------------------------------------------------------------

//void CGrfDoc::ExportGraphicsData(CNeutralGrfImportExport & EGH) 
//  {
//  CGrfTagInfoArray GTIA;
//  const int Cnt = GetTagListInfo(GTIA);
//  if (Cnt>0)
//    {
//    Strng Page = GetTitle();
//    if (Page.Len()>4)
//      Page = Page.Left(Page.Len()-4);//remove .scg
//    POSITION pos = GetFirstViewPosition();
//    CView* pFirstView = GetNextView( pos );
//    CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
//    for (int i=0; i<Cnt; i++)
//      {
//      CGrfTagInfo & I = GTIA[i];
//      CXM_ObjectTag ObjTag(I.sTag(), TABOpt_Parms);
//      CXM_ObjectData ObjData;
//      CXM_Route Route;
//      int TagTyp = 0; //error
//      if (pGWnd->XReadTaggedItem(ObjTag, ObjData, Route))
//        {
//        I.sClass = ObjData.FirstItem()->Class();
//        pTagObjClass pTagObjC = TagObjClass::FindClassId(I.sClass());
//        if (pTagObjC==NULL)
//          pTagObjC = TagObjClass::FindClassName(I.sClass());
//        if (pTagObjC && (stricmp(FlwLinkGrp, pTagObjC->Group())==0 || stricmp(CtrlLinkGrp, pTagObjC->Group())==0))
//          TagTyp = 2; //link
//        else
//          TagTyp = 1; //unit/node
//        }
//      else
//        {
//        I.sClass = "";
//        }
//
//      if (TagTyp==2)
//        {
//        LinePointsArray LPA;
//        GCB.pDrw->CollectLinkInfo(I, LPA);
//        EGH.WriteLink(I, LPA, Page());
//        }
//      else if (TagTyp==1 || TagTyp==0)
//        {
//        EGH.WriteUnit(I, Page());
//        }
//      }
//    }
//  }

//---------------------------------------------------------------------------

int CGrfDoc::ClearSelectedTags()
  {
  return 0;
  };

//---------------------------------------------------------------------------

int CGrfDoc::GetSelectedTags(Strng_List & Lst)
  {
  CEntInView* p = GCB.pDsp->Vp1->FirstSelectedEntity();
  while (p)
    {
    DXF_ENTITY e = p->EntityPtr();
    if (e)
      {
      pchar pTag = Find_Attr_Value(e, "Tag");
      if (pTag)
        Lst.Append(pTag);
      }
    p = GCB.pDsp->Vp1->NextSelectedEntity();
    }
  return 0;
  };

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGrfDoc, DocRoot)
  //{{AFX_MSG_MAP(CGrfDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

