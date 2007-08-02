//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

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
//#include "optoff.h"

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

  m_sDocGUID = TaggedObject::CreateGUIDStr();
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
      if (_stricmp(gs_pPrj->m_sGrfFrameName()+DelimitPrefixLen, Scls[i].Nm)==0)
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

BOOL CGrfDoc::FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr)
  {
  return GCB.pDrw->FindTag(Tag, pLastMatchingInsert, pLastMatchingAttr);
  }

//---------------------------------------------------------------------------

void CGrfDoc::FindTagComplete(LPSTR Tag, DXF_ENTITY pTagInsert, DXF_ENTITY pTagAttr, dword Options)
  {



  //if (!pGWnd->bDonePaint)
  //  {//drawing has never been painted ie display list has not been built, force it...
  //  pGWnd->Dsp.Open(NULL);
  //  CRect ClpBox;
  //  pGWnd->GetClientRect(&ClpBox);
  //  pGWnd->Dsp.Paint(ClpBox);
  //  pGWnd->Dsp.Close();
  //  pGWnd->bDonePaint = 1;
  //  }


  //pDrw->SelectInsertsOnAttrCombo(NULL, a999, b999);
  //if (1)//(pDsp->Vp1)->FirstSelectedEntity())
  //  {
  //gs_pCmd->SetRetCode(1);
  //gs_pCmd->Print("Goto '%s' succeeded\n", b999[0]);
  //ActivateGWnd();

  POSITION pos = GetFirstViewPosition();
  CView* pFirstView = GetNextView( pos );
  CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;

  if (pGWnd->GetFocus()!=pGWnd)
    MDIActivateThis(pGWnd); //bring the window to the front
  if (pGWnd->GetParent()->IsIconic())
    pGWnd->GetParent()->ShowWindow(SW_RESTORE);
 
  GCB.pDrw->pDsp->Open();

  //if (Options & FTO_MoveCursor)
  //  SetCursor(pTagInsert, pTagAttr);

  //if (0)//gs_pPrj && (gs_pPrj->m_bGrfPromptForZoom || gs_pPrj->m_bGrfMoveCursor))
  //  {
  //  //PanAndSetCursor();
  //  }
  //else
  //  {
  //  //CString DocTitle = pGWnd->GetDocument()->GetTitle();
  //  LogNote("GrfCmds", 0, "%s has been found on %s.", Tag, (const char*)GetTitle());//DocTitle);
  //  }
  char * TagTagList[] =   { TagAttribStr, (pchar)NULL  };
  char * ValueTagList[] = { Tag, (pchar)NULL     };

  GCB.pDrw->pDsp->Vp1->ClearAllEntity();
  GCB.pDrw->pDsp->Vp1->ClrSelectionAllList();
  GCB.pDrw->pDsp->Vp1->AddSelectionEntityList(DXF_INSERT);
  GCB.pDrw->pDsp->Vp1->AddSelectionAttribList(TagAttribStr);


  IRegExpPtr *pRE=NULL;//(gs_pPrj->m_bGrfRegExpOn) ? new IRegExpPtr("VBScript.RegExp") : NULL;
  GCB.pDrw->SelectInsertsOnAttrCombo(NULL, TagTagList, ValueTagList, pRE);

  GCB.pDrw->EntityInvalidate(pTagAttr, NULL);
  //pDsp->Vp1->ClearAllEntity();
  //GCB.pDrw->pDsp->Draw(e1, -1);

  CRect ClpBox;
  pGWnd->GetClientRect(&ClpBox);
  pGWnd->Dsp.Paint(ClpBox);

  //pGWnd->EnInvalidate(false);

  Pan(pTagInsert, pTagAttr, Options );

  if (Options & (FTO_MoveCursor|FTO_HighliteSlow|FTO_HighliteKwik))
    SetCursor(pTagInsert, pTagAttr, Options );


  if (Options & FTO_DoAccess)// gs_pPrj->bDoGrfAutoAccess)
    {
    char* pTxt = new char[strlen(Tag)+1];
    strcpy(pTxt, Tag);
    ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCESS, (LPARAM)pTxt);
    }
  else if (Options & FTO_DoAccessNoUpd)// gs_pPrj->bDoGrfAutoAccess)
    {
    char* pTxt = new char[strlen(Tag)+1];
    strcpy(pTxt, Tag);
    ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCNOUPDHIST, (LPARAM)pTxt);
    }

  //else
  //  {
  //  gs_pCmd->Print("Goto '%s' failed\n", b999[0]);
  //  //AfxMessageBox("Goto has not found the requested Tag.\nTry another drawing.");//, MB_ICONQUESTION);
  //  if (GotoFromToolbar)
  //    {
  //    char* pTxt = new char[strlen(b999[0])+1];
  //    strcpy(pTxt, b999[0]);
  //    ScdMainWnd()->PostMessage(WMU_TAGACTION, gs_pPrj->bDoGrfAutoAccess ? SUB_TAG_FINDANDACCESS : SUB_TAG_FIND, (LPARAM)pTxt);
  //    }
  //  }
  //gs_pPrj->bDoGrfAutoAccess = 0;
  //delete pRE;
  //......

  GCB.pDrw->pDsp->Close();

  };

//---------------------------------------------------------------------------

flag CGrfDoc::Pan(DXF_ENTITY pTagInsert, DXF_ENTITY pTagAttr, dword Options)
  {
  //CEntInView* pEnt = ((pDsp->Vp1)->FirstSelectedEntity());
  DXF_ENTITY e = pTagInsert;

  DXF_Drawing   * pDrw = GCB.pDrw;
  Grf3D_Display * pDsp = GCB.pDrw->pDsp;

  if (1)//gs_pPrj->m_bGrfPromptForZoom)
    {

    C3_BOX_S Bounds;
    PT3 p;
    c3v_set(1.1e30, 1.1e30, 1.1e30, p);
    c3a_box_init_pt(&Bounds, p);
    C3_TRANSFORM c3t;

    c3t[0][0] = c3t[0][1] = c3t[0][2] = c3t[0][3] = 0.0;
    c3t[1][0] = c3t[1][1] = c3t[1][2] = c3t[1][3] = 0.0;
    c3t[2][0] = c3t[2][1] = c3t[2][2] = c3t[2][3] = 0.0;
    c3t[0][0] = c3t[1][1] = c3t[2][2] = 1.0;

    // Get bound on the block
    pBlock b = pDrw->Blocks.Find(DXF_INSERT_BLOCK_GET(e));
    pDrw->ConstructInsertMatrix(c3t, b, e);
    b->GetBounds(c3t, &Bounds);

    double ZBx0=GCB.pDrw->pDsp->Vp1->ZoomBox.minpt[0];
    double ZBy0=pDsp->Vp1->ZoomBox.minpt[1];
    double ZBx1=pDsp->Vp1->ZoomBox.maxpt[0];
    double ZBy1=pDsp->Vp1->ZoomBox.maxpt[1];

    double xl=ZBx1-ZBx0;
    double yl=ZBy1-ZBy0;

    bool DoPan=false;
    if (C3_MIN_X(&Bounds)<ZBx0)
      DoPan=true;
    else if (C3_MAX_X(&Bounds)>ZBx1)
      DoPan=true;
    else if (C3_MIN_Y(&Bounds)<ZBy0)
      DoPan=true;
    else if (C3_MAX_Y(&Bounds)>ZBy1)
      DoPan=true;

    if (DoPan)
      {
      xl=Max(xl, C3_MAX_X(&Bounds) - C3_MIN_X(&Bounds));
      yl=Max(yl, C3_MAX_Y(&Bounds) - C3_MIN_Y(&Bounds));

      double xc=(C3_MIN_X(&Bounds)+C3_MAX_X(&Bounds))/2.0;
      double yc=(C3_MIN_Y(&Bounds)+C3_MAX_Y(&Bounds))/2.0;

      double w = Max(C3_MAX_X(&pDrw->Bounds)-C3_MIN_X(&pDrw->Bounds), (REAL)1.0);
      double h = Max(C3_MAX_Y(&pDrw->Bounds)-C3_MIN_Y(&pDrw->Bounds), (REAL)1.0);

      double x0=pDrw->Bounds.minpt[0]-0.025*w;
      double y0=pDrw->Bounds.minpt[1]-0.025*h;
      double x1=pDrw->Bounds.maxpt[0]+0.025*w;
      double y1=pDrw->Bounds.maxpt[1]+0.025*h;

      double xlh=xl/2.0;
      double ylh=yl/2.0;
      if (xc-xlh<x0)
        xc-=xc-xlh-x0;
      if (xc+xlh>x1)
        xc-=xc+xlh-x1;
      if (yc-ylh<y0)
        yc-=yc-ylh-y0;
      if (yc+ylh>y1)
        yc-=yc+ylh-y1;

      pDsp->SetZoom(Zoom_Win, xc-xlh, yc-ylh, xc+xlh, yc+ylh);

      POSITION pos = GetFirstViewPosition();
      CView* pFirstView = GetNextView( pos );
      CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
      pGWnd->RedrawWindow();
      return true;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void CGrfDoc::SetCursor(DXF_ENTITY pTagInsert, DXF_ENTITY pTagAttr, dword Options)
  {
  Pt_3f   world;
  Pt_SLW  Pt1;
  world.X=DXF_INSERT_PT(pTagInsert)[0];//xc;
  world.Y=DXF_INSERT_PT(pTagInsert)[1];//yc;
  world.Z=0.0;
  GCB.pDrw->pDsp->SetCPtWorld(world, Pt1); 
  ///gs_pCmd->SetDigInfo(GC_BigCurs, 1);

  if (Options & FTO_MoveCursor)
    SetCursorPos((int)Pt1.Screen.X, (int)Pt1.Screen.Y);

  if (Options & (FTO_HighliteKwik|FTO_HighliteSlow))
    {
    Cursor_Types CursMem=GCB.pDrw->pDsp->Cursor;
    GCB.pDrw->pDsp->Cursor=GC_BigCurs;
    GCB.pDrw->pDsp->EndPt=Pt1;
    GCB.pDrw->pDsp->XORCursor();
    Sleep((Options & FTO_HighliteKwik) ? 200 : 400);
    GCB.pDrw->pDsp->XORCursor();
    GCB.pDrw->pDsp->Cursor=CursMem;
    }
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
//        if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 || _stricmp(CtrlLinkGrp, pTagObjC->Group())==0))
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
//        CLinePointsArray LPA;
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
      pchar pTag = Find_Attr_Value(e, TagAttribStr);
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

void CGrfDoc::OnCloseDocument()
  {
  //DeleteTags(false);
  DocRoot::OnCloseDocument();
  };

int CGrfDoc::DeleteTags(bool WithPrompts)
  {
  Strng_List MyTags;
  GetTagList(MyTags);
  int nTags=MyTags.Length();
  int nTagsMultPgs=0;
  if (nTags>0)
    {
    CMap<LPTSTR, LPTSTR, Strng*, Strng*> MyTagMap;
    MyTagMap.InitHashTable(FindNextPrimeNumber(MyTags.Length()));
    for (Strng *pTag=MyTags.First(); pTag; pTag=MyTags.Next())
      {
      pTag->SetIndex(0);
      MyTagMap.SetAt(pTag->Str(), pTag);
      }

    for (int iTmpl=iGraphTemplate; iTmpl<=iVisioTemplate; iTmpl++)
      {
      if (ScdApp()->TemplateExists(iTmpl))
        {
        POSITION Pos = ScdApp()->Template(iTmpl).GetFirstDocPosition();
        while (Pos)
          {
          CGrfDoc* pOtherDoc = (CGrfDoc*)(ScdApp()->Template(iTmpl).GetNextDoc(Pos));
          if (pOtherDoc!=this)
            {
            Strng *pTag;
            Strng_List OtherTags;
            int NGrfTags = pOtherDoc->GetTagList(OtherTags);
            for (Strng *pOtherTag=OtherTags.First(); pOtherTag; pOtherTag=OtherTags.Next())
              if (MyTagMap.Lookup(pOtherTag->Str(), pTag))
                pTag->SetIndex(pTag->Index()+1);
            }
          }
        }
      }

    for (Strng *pTag=MyTags.First(); pTag; pTag=MyTags.Next())
      {
      if (pTag->Index()>0)
        nTagsMultPgs++;
      }

    int Ret=IDYES;
    if (WithPrompts)
      {
      CString S;
      if (nTagsMultPgs)
        S.Format("Do you want Delete %i Units (%i Units have Multiple References)", nTags-nTagsMultPgs, nTagsMultPgs);
      else
        S.Format("Do you want Delete %i Units", nTags);
      Ret=AfxMessageBox(S, MB_ICONQUESTION|MB_YESNOCANCEL);
      }
    if (Ret==IDCANCEL)
      return -1;
    if (Ret==IDYES)
      {
      int nMdlDeletes=0;
      int nDeletesFailedCnt=0;
      for (Strng *pTag=MyTags.First(); pTag; pTag=MyTags.Next())
        {
        if (pTag->Index()==0)
          {
          int RetCode = gs_Exec.DeleteTag(pTag->Str());
          if (RetCode!=EODT_DONE)
            {
            //dbgpln("Mdl Not Deleted %s", pTag->Str());
            LogError(pTag->Str(), 0, "Model not deleted");
            nDeletesFailedCnt++;
            }
          else
            {
            //dbgpln("Mdl     Deleted %s", pTag->Str());
            nMdlDeletes++;
            }
          }
        }
      if (nDeletesFailedCnt)
        {
        CString S;
        S.Format("%i Units Failed to Delete", nDeletesFailedCnt);
        AfxMessageBox(S, MB_ICONEXCLAMATION|MB_OK);
        }
      }
    }
  return nTags;
  };

//---------------------------------------------------------------------------

