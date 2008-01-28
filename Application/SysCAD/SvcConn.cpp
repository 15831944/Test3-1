#include "stdafx.h"
#include "grfdoc.h"
#include "msgwnd.h"

#include "flwnode.h"
#include "SvcConn.h"
#include "SvcConnCLR.h"

//#include "optoff.h"

#define NETSERVERNAME "SvcConnect"

//========================================================================

#if SYSCAD10

#define dbgSvcConn  1

// temporary
#if dbgSvcConn
static bool dbgConnect() { return 1; }
#endif


//========================================================================
//
//
//
//========================================================================

#define WITHDBGLINES  01

#if WITHDBGLINES

#define DO_ENTRY_GT(Method, Guid, Tag)                                                                             \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7s %7s %s %s", Method, "", "", Guid, Tag);                                                   \
  dbgindent(2);                                                                                                    \
  }

#define DO_ENTRY_GTP(Method, Guid, Tag, Path)                                                                      \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7s %7s %s  %-20s  %s", Method, "", "", Guid, Tag, Path);                                     \
  dbgindent(2);                                                                                                    \
  }

#define DO_ENTRY_TP(Method, Tag, Path)                                                                             \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7s %7s %-20s  %s", Method, "", "", Tag, Path);                                               \
  dbgindent(2);                                                                                                    \
  }

#define DO_ENTRY_GTPSM(Method, Guid, Tag, Path, Shape, Model)                                                      \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7s %7s %s  %-20s  %s  %s  %s", Method, "", "", Guid, Tag, Path, Shape, Model);               \
  dbgindent(2);                                                                                                    \
  }

#define DO_EXIT(Method)                                                                                            \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i", Method, "", m_lRequestIdRet);                                                    \
  };

#define DO_EXIT_T(Method, Tag)                                                                                     \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i %s", Method, "", m_lRequestIdRet, Tag);                                            \
  };

#define DO_EXIT_G(Method, Guid)                                                                                    \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i %s", Method, "", m_lRequestIdRet, Guid);                                           \
  };

#define DO_EXIT_GT(Method, Guid, Tag)                                                                              \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i %s %s", Method, "", m_lRequestIdRet, Guid, Tag);                                   \
  };

#define DO_EXIT_GG(Method, Guid1, Guid2)                                                                           \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i %s %s", Method, "", m_lRequestIdRet, Guid1, Guid2);                                \
  };

#else

#define DO_ENTRY_GT(Method, Guid, Tag)             /* Do Nothing */
#define DO_ENTRY_TP(Method, Tag, Path)             /* Do Nothing */
#define DO_ENTRY_GTP(Method, Guid, Tag, Path)      /* Do Nothing */
#define DO_EXIT(Method)                            /* Do Nothing */
#define DO_EXIT_T(Method, Guid)                    /* Do Nothing */
#define DO_EXIT_G(Method, Guid)                    /* Do Nothing */
#define DO_EXIT_GG(Method, Guid1, Guid2)           /* Do Nothing */

#endif

//========================================================================

#if WITHDBGLINES

#define ON_ENTRY_GT(Method, Guid, Tag)                                                                             \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7I64i %7I64i %s %s", Method, eventId, requestId, Guid, Tag);                                 \
  dbgindent(4);                                                                                                    \
  }

#define ON_ENTRY_GTP(Method, Guid, Tag, Path)                                                                      \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7I64i %7I64i %s  %-20s  %s", Method, eventId, requestId, Guid, Tag, Path);                   \
  dbgindent(4);                                                                                                    \
  }

#define ON_ENTRY_GTM(Method, Guid, Tag, Model)                                                                      \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7I64i %7I64i %s  %-20s %s", Method, eventId, requestId, Guid, Tag, Model);                   \
  dbgindent(4);                                                                                                    \
  }

#define ON_ENTRY_GTPSM(Method, Guid, Tag, Path, Shape, Model)                                                      \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7I64i %7I64i %s  %-20s  %s  %s  %s", Method, eventId, requestId, Guid, Tag, Path, Shape, Model); \
  dbgindent(4);                                                                                                    \
  }

#define ON_EXIT(Method)                                                                                            \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-4);                                                                                                   \
  dbgpln("%-30s  <<< %7I64i %7I64i", Method, eventId, m_lRequestIdRet);                                            \
  };

#define ON_EXIT_G(Method, Guid)                                                                                    \
  if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-4);                                                                                                   \
  dbgpln("%-30s  <<< %7I64i %7I64i %s", Method, eventId, m_lRequestIdRet, Guid);                                   \
  };

#else

#define ON_ENTRY_GT(Method, Guid, Tag)             /* Do Nothing */            
#define ON_ENTRY_GTP(Method, Guid, Tag, Path)      /* Do Nothing */            
#define ON_EXIT(Method)                            /* Do Nothing */            
#define ON_EXIT_G(Method, Guid)                    /* Do Nothing */            

#endif

//========================================================================
//
// Common
//
//========================================================================

static int CheckTagExists(LPCSTR Tag)
  {
  if (gs_pSfeSrvr)
    {
    if (!(gs_pSfeSrvr->FE_TestModelTagUnique((char*)Tag)))
      return 0;
    else
      return -1;
    }
  return -2;
  }

static CDocument* GetGrfDoc(int index, LPCSTR name)
  {
  bool Done=false;
  CDocument* pDoc=NULL;
  int GrfDocCnt=-1;
  CDocTemplate &Templ = ScdApp()->GraphTemplate();
  POSITION PosD = Templ.GetFirstDocPosition();
  if (name==NULL)
    {
    return Templ.GetNextDoc(PosD);
    }
  else
    {
    while (!Done && PosD)
      {
      pDoc = Templ.GetNextDoc(PosD);
      POSITION pos = pDoc->GetFirstViewPosition();
      if (pos)
        {
        CView* pFirstView = pDoc->GetNextView(pos);
        CWnd* w = pFirstView->GetParent();
        //      if (pDoc->IsKindOf(RUNTIME_CLASS(CGrfDoc)))
        if (dynamic_cast<CGrfDoc*>(pDoc))
          {
          //ASSERT(dynamic_cast<CGrfFrameWnd*>(w));
          GrfDocCnt++;
          if (index<0 && pDoc->GetTitle().CompareNoCase(name)==0)
            Done = true;
          else if (GrfDocCnt==index)
            Done = true;
          }
        }
      }
    if (!Done)
      return NULL;
    }
  return pDoc;
  }


//========================================================================
//
// Messaging
//
//========================================================================

CSvcConnect::CSvcConnect()
  {
  m_lEventId   = 0;
  }
CSvcConnect::~CSvcConnect()
  {
  }

void CSvcConnect::Startup()
  {
  m_GrfGrpsNames.InitHashTable(101);
  m_GrfGrpsGuids.InitHashTable(101);

  m_pCLR = new CSvcConnectCLR;
  m_pCLR->Startup(this);
  };

bool CSvcConnect::ConfigSetup()
{
  return m_pCLR->ConfigSetup(this);
}

void CSvcConnect::Shutdown()
  {
  if (m_pCLR)
    {
    m_pCLR->Shutdown();
    delete m_pCLR;
    m_pCLR=NULL;
    }

  POSITION Pos=m_GrfGrpsNames.GetStartPosition();
  while (Pos)
    {
    LPCSTR Key;
    CsGrfGroup *pG;
    m_GrfGrpsNames.GetNextAssoc(Pos, Key, pG);
    delete pG;
    }

  m_GrfGrpsNames.RemoveAll();
  m_GrfGrpsGuids.RemoveAll();
  };


void CSvcConnect::Export2Scd10(LPCSTR projectPath, LPCSTR configPath)
  {
  m_Ctrl.m_bExportBusy = true;
  //m_pCLR->Export(projectPath, configPath);


  if (m_pCLR->PrepareForExport(projectPath, configPath))
    {
    CGetExistingItems GI;

    int PrevPage=-1;
    CString Path;

    POSITION Pos=GI.m_Groups.GetHeadPosition();
    while (Pos)
      {
      CGetExistingItems::CGroup & Grp=*GI.m_Groups.GetNext(Pos);

      CString GrpGuid = TaggedObject::CreateGuidStr(); // where should this come from

      DO_ENTRY_GTP("DoCreateGroupE", GrpGuid, Grp.m_sTitle, MakePath(projectPath));

      m_pCLR->AddCreateGroup(m_lRequestIdRet, GrpGuid, Grp.m_sTitle, MakePath(projectPath), 
        CRectangleF(Grp.m_PageRct.Left()+Grp.m_XOff,Grp.m_PageRct.Bottom()+Grp.m_YOff, Grp.m_PageRct.Width(), Grp.m_PageRct.Height()));

      DO_EXIT_GT("DoCreateGroupE", GrpGuid, Grp.m_sTitle);

      }
    //    static __int64 RqID=0;

    Pos=GI.m_Items.GetHeadPosition();
    while (Pos)
      {
      CGetExistingItems::CItem & I = *GI.m_Items.GetNext(Pos);
      if (I.m_sTag.CompareNoCase("PlantArea")==0)
        continue;

      CNodeListItem & N = *I.m_pNLItem;

      CGetExistingItems::CGroupIndex Inx;
      if (!GI.m_TagMap.Lookup(I.m_sTag, Inx))
        {
        LogError("Export2Scd10", 0, "Tag Not in Graphics %s", I.m_sTag);
        continue;
        };

      CGetExistingItems::CGroup & Grp = *Inx.m_pGrp;
      CGrfTagInfo               & GTI =Grp.m_GTIA[Inx.m_iGTIA];

      CString Symbol = GTI.m_sSymbol();

#if dbgSvcConn
      if (dbgConnect())
        {
        dbgpln("Export %s %-20s %-20s %-20s", N.m_bIsLnk?"Link":"Node", N.m_sTag, Symbol, N.m_sClass);
        dbgindent(4);
        }
#endif

      if (!N.m_bIsLnk)
        {
        CString GraphicGuid = GTI.m_sGuid();
        CString Shape    = ExtractShape(Symbol);
        CString Model    = N.m_sClass;

        FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag, NULL);

        CString ModelGuid = pNode->Guid();

        DO_ENTRY_GTPSM("DoCreateNodeE", ModelGuid, I.m_sTag, MakePath(projectPath, Grp.m_sTitle), Shape, Model);

        float boxW = float(int(GTI.m_HiBnd.m_X-GTI.m_LoBnd.m_X));
        float boxH = float(int(GTI.m_HiBnd.m_Y-GTI.m_LoBnd.m_Y));
        float boxX = float(int(GTI.m_LoBnd.m_X + Grp.m_XOff) + 0.5); // needs to be x.5mm to meet grid in 10.
        float boxY = float(int(Grp.m_PageRct.Height() - GTI.m_HiBnd.m_Y + Grp.m_YOff) + 0.5); // needs to be x.5mm to meet grid in 10.

        float textBoxW = float(int(GTI.m_Tag.m_XScale * 3.0 * GTI.m_sTag.GetLength()));
        float textBoxH = float(int(GTI.m_Tag.m_YScale * 5.0));
        float textBoxX = float(int(GTI.m_Tag.m_X + Grp.m_XOff - textBoxW / 2.0) + 0.5); // needs to be x.5mm to meet grid in 10.
        float textBoxY = float(int(Grp.m_PageRct.Height() - GTI.m_Tag.m_Y + Grp.m_YOff - textBoxH) + 0.5); // needs to be x.5mm to meet grid in 10.

        m_pCLR->AddCreateNode(m_lRequestIdRet, ModelGuid, GraphicGuid, I.m_sTag,
          MakePath(projectPath, Grp.m_sTitle), Model, Shape,
          CRectangleF(boxX, boxY, boxW, boxH),
          0.0, 
          CRectangleF(textBoxX, textBoxY, textBoxW, textBoxH),
          -GTI.m_Tag.m_Rotation, GTI.m_Tag.m_Visible,
          0, false, false);

        DO_EXIT_GG("DoCreateNodeE", ModelGuid, GraphicGuid);
        }
      else
        {
        CString GraphicGuid = GTI.m_sGuid();
        CString Shape    = ExtractShape(Symbol);
        CString Model    = N.m_sClass;

        DO_ENTRY_GTP("DoCreateLinkE", "", I.m_sTag, MakePath(projectPath, Grp.m_sTitle));

        CLinePointsArray  LPA;

        LPA.SetSize(0);
        Grp.m_pDoc->GCB.pDrw->CollectLinkInfo(GTI, LPA);

        FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag, NULL);

        CString ModelGuid  = pNode->Guid();
        CString SrcMdlGuid = pNode->Nd_Rmt(0)->Guid();
        CString SrcGrfGuid = "Grf GUID Fetch Failed";
        CString SrcPort    = pNode->IODesc_Rmt(0)->IOName();
        CString DstMdlGuid = pNode->Nd_Rmt(1)->Guid();
        CString DstGrfGuid = "Grf GUID Fetch Failed";
        CString DstPort    = pNode->IODesc_Rmt(1)->IOName();

        CGetExistingItems::CGroupIndex SInx;
        if (GI.m_TagMap.Lookup(pNode->Nd_Rmt(0)->Tag(), SInx))
          SrcGrfGuid = Grp.m_GTIA[SInx.m_iGTIA].m_sGuid();
        else
          LogError("Export2Scd10", 0, "Tag Not in Graphics %s", pNode->Nd_Rmt(0)->Tag());

        CGetExistingItems::CGroupIndex DInx;
        if (GI.m_TagMap.Lookup(pNode->Nd_Rmt(1)->Tag(), DInx))
          DstGrfGuid = Grp.m_GTIA[DInx.m_iGTIA].m_sGuid();
        else
          LogError("Export2Scd10", 0, "Tag Not in Graphics %s", pNode->Nd_Rmt(1)->Tag());

        if (dbgConnect())                                                              
          {                                                                            
          dbgpln("%-30s   Src Mdl:%s Grf:%s %s", "", SrcMdlGuid, SrcGrfGuid, SrcPort);
          dbgpln("%-30s   Dst Mdl:%s Grf:%s %s", "", DstMdlGuid, DstGrfGuid, DstPort);
          }


        CPointFList CtrlPts;
        for (int i=0; i<LPA.GetCount(); i++)
          {
          float PtX = float(int(LPA[i].x+Grp.m_XOff) + 0.5); // needs to be x.5mm to meet grid in 10.
          float PtY = float(int(Grp.m_PageRct.Height()-LPA[i].y+Grp.m_YOff) + 0.5); // needs to be x.5mm to meet grid in 10.
          CtrlPts.AddTail(CPointF(PtX, PtY));
          }

        float textBoxW = float(int(GTI.m_Tag.m_XScale * 3.0 * GTI.m_sTag.GetLength()));
        float textBoxH = float(int(GTI.m_Tag.m_YScale * 5.0));
        float textBoxX = float(int(GTI.m_Tag.m_X + Grp.m_XOff - textBoxW / 2.0) + 0.5); // needs to be x.5mm to meet grid in 10.
        float textBoxY = float(int(Grp.m_PageRct.Height() - GTI.m_Tag.m_Y + Grp.m_YOff - textBoxH) + 0.5); // needs to be x.5mm to meet grid in 10.

        m_pCLR->AddCreateLink(m_lRequestIdRet, ModelGuid, GraphicGuid, I.m_sTag, 
          MakePath(projectPath, Grp.m_sSymbol), 
          Model, 
          SrcMdlGuid, DstMdlGuid, 
          SrcGrfGuid, DstGrfGuid, 
          SrcPort, DstPort, 
          CtrlPts,
          CRectangleF(textBoxX, textBoxY, textBoxW, textBoxH), true,
          -GTI.m_Tag.m_Rotation);


        DO_EXIT_G("DoCreateLinkE", ModelGuid);
        }

      // Remove Original Symbol
      if (GTI.e)
        {
        Grp.m_pDoc->GCB.pDsp->Draw(GTI.e, GrfHelper.GR_BACKGROUND);
        Grp.m_pDoc->GCB.pDrw->Delete(GTI.e);
        }

#if dbgSvcConn
      if (dbgConnect())
        {
        dbgindent(-4);
        dbgpln("");
        }
#endif
      }

    if (!m_pCLR->ProcessChangeLists(m_lRequestIdRet))
      {
      _asm int 3;  // Some Error occurred
      }
    }


  m_Ctrl.m_bExportBusy = false;
  };



void CSvcConnect::Attach2Scd10()
  {
  m_pCLR->Attach2Scd10();
  };

//========================================================================

void CSvcConnect::Load()
  {
  m_pCLR->Load();
  };

void CSvcConnect::Save()
  {
  m_pCLR->Save();
  };

//========================================================================

void CSvcConnect::LogMessage(DWORD Type, LPCSTR Msg)
  {
  m_pCLR->LogMessage(Type, Msg);
  };

void CSvcConnect::dbgPrintLn(LPCSTR Fmt, LPCSTR S0, LPCSTR S1, LPCSTR S2, LPCSTR S3, LPCSTR S4)
  {
  dbgpln((LPSTR)Fmt, S0, S1, S2, S3, S4);
  };

void CSvcConnect::DoOpenProject(LPCSTR tag, LPCSTR path)
  {
  CString Fn(path);
  Fn+="\\Project.spj";
  gs_pPrj->DoOpen((LPSTR)(LPCSTR)Fn, "",  PLT_Normal);
  int xxx=0;
  };

void CSvcConnect::Remove9Graphics()
  {
  gs_pPrj->Remove9Graphics();
  }

//========================================================================

void CSvcConnect::OnCreateGroup(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, 
                                const CRectangleF & boundingRect)
  {
  CsGrfGroup * pG = new CsGrfGroup;
  pG->m_Guid=guid;
  pG->m_Name=tag;
  pG->m_Path=path;
  pG->m_Bounds=boundingRect;

  m_GrfGrpsNames.SetAt(pG->m_Name, pG);
  m_GrfGrpsGuids.SetAt(pG->m_Guid, pG);
  };

//========================================================================
//
//
//
//========================================================================

CString CSvcConnect::MakePath(LPCSTR Part1, LPCSTR Part2, LPCSTR Part3)
  {
  CString S("/");
  if (Part1) { S+=Part1; S+="/"; }
  if (Part2) { S+=Part2; S+="/"; }
  if (Part3) { S+=Part3; S+="/"; }
  return S;
  };

CString CSvcConnect::ExtractPageName(LPCSTR Path)
  {
  CString Pg(Path);
  // find endof Path
  int n=0; 
  for (int i=0; i<Pg.GetLength(); i++)
    if (Pg[i]=='/')
      n++;

  if (n>=2)
    {
    // Assume Page is last delimited string
    for (i=0; i<n-1; i++)
      Pg.Delete(0, Pg.Find('/')+1);

    Pg.Delete(Pg.Find('/'), 1000);
    }
  // Error Checking ?????????
  return Pg;
  }

CGrfDoc * CSvcConnect::FindGrfDoc(LPCSTR PageName)
  {
  CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(GetGrfDoc(-1, PageName && strlen(PageName)>0?PageName:NULL));
  // Error Checking ?????????
  return pDoc;
  }

CGrfWnd * CSvcConnect::FindGrfWnd(LPCSTR PageName)
  {
  CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(GetGrfDoc(-1, PageName && strlen(PageName)>0?PageName:NULL));
  if (pDoc)
    {
    POSITION pos = pDoc->GetFirstViewPosition();
    CView* pFirstView = pDoc->GetNextView( pos );
    return (CGrfWnd*)pFirstView;
    }
  return NULL;
  }

CRectangleF CSvcConnect::GetPageRect(LPCSTR PageName) 
  {
  CRectangleF PageRct(0.0, 420, 0, 297);
  CsGrfGroup *pG;
  if (PageName&& m_GrfGrpsNames.Lookup(PageName, pG))
    {
    PageRct = pG->m_Bounds;
    }
  return PageRct;
  }

CRectangleF CSvcConnect::GetContainingPageRect(const CRectangleF & rct)
  {
  POSITION Pos=m_GrfGrpsNames.GetStartPosition();
  while (Pos)
    {
    LPCSTR Key;
    CsGrfGroup *pG;
    m_GrfGrpsNames.GetNextAssoc(Pos, Key, pG);
    if (pG->m_Bounds.Contains(rct, true))
      return pG->m_Bounds;
    }
  return CRectangleF(0.0, 0, 0, 0);
  }

CsGrfGroup * CSvcConnect::GetContainingGroup(const CRectangleF & rct)
  {
  POSITION Pos=m_GrfGrpsNames.GetStartPosition();
  while (Pos)
    {
    LPCSTR Key;
    CsGrfGroup *pG;
    m_GrfGrpsNames.GetNextAssoc(Pos, Key, pG);
    if (pG->m_Bounds.Contains(rct, true))
      return pG;
    }
  return NULL;
  };

CString CSvcConnect::ExtractShape(LPCSTR Symbol)
  {
  CString Shape(Symbol);
  int iSep=Shape.FindOneOf("$:");
  if (iSep>=0)
    Shape.Delete(0, iSep+1);
  return Shape;
  }

//========================================================================
//
//
//
//========================================================================

void CSvcConnect::GCBCreateGroup(LPCSTR Guid, LPCSTR Prj, LPCSTR Page, const CRectangleF & boundingRect)
  {
  DO_ENTRY_GTP("GCBCreateGroup", Guid, "--", MakePath(Prj, Page));

  CDocTemplate & GrfTemplate = ScdApp()->GraphTemplate();

  //ASSERT(pTemplate_ != NULL);

  gs_pPrj->bDoingLoad=true;
  CGrfDoc * pDoc = (CGrfDoc *)GrfTemplate.OpenDocumentFile(NULL);
  gs_pPrj->bDoingLoad=false;

  if (pDoc)
    {
    pDoc->SetTitle(Page);
    OnCreateGroup(-1/*eventId*/, -1/*requestId*/, Guid, Page, Prj, boundingRect); 
    //pDoc->m
    }

  DO_EXIT("GCBCreateGroup");
  }

//========================================================================
//
//
//
//========================================================================

void CSvcConnect::GCBCreateNode(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR Symbol, LPCSTR ClassId, Pt_3f Pt, Pt_3f Scl, float Angle)
  {
  double Width=20;
  double Height=20;

  CRectangleF PageRct = GetPageRect(Page);
  CRectangleF boundingRect(Pt.x()-0.5*Width,PageRct.Top()-Pt.y()-0.5*Height,Width, Height);
  // TO Fix
  //CString Shape = ExtractShape(ClassId);//Symbol);
  CString Shape = ExtractShape(Symbol);

  DO_ENTRY_GTP("GCBCreateNode", "NULL-Guid", Tag, MakePath(Prj, Page));

  CString ModelGuid = TaggedObject::CreateGuidStr();
  CString GraphicGuid = TaggedObject::CreateGuidStr();

  m_pCLR->AddCreateNode(m_lRequestIdRet, ModelGuid, GraphicGuid, Tag, MakePath(Prj, Page), ClassId, Shape, boundingRect, Angle, CRectangleF(0.0, 0.0, 0.0, 0.0), 0.0, true, 0, false, false); // !!! tagArea not used.

  DO_EXIT_GG("GCBCreateNode", ModelGuid, GraphicGuid);
  };

//------------------------------------------------------------------------

//void CSvcConnect::OnCreateItem(__int64 eventId, __int64 requestId, List<Guid> created, )
//  {
//  }

void CSvcConnect::OnCreateNodeG(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, LPCSTR path, 
                                LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect,
                                float angle, const CRectangleF & tagArea, float tagAngle, COLORREF FillColor, 
                                bool mirrorX, bool mirrorY)
  {

  // !!! tagArea not handled as yet.

  ON_ENTRY_GTPSM("OnCreateNodeG", Guid, tag, path, shape, model);

  try
    {

    CString PageName=CSvcConnect::ExtractPageName(path);

    //CGrfWnd * pWnd=FindGrfWnd(PageName);
    //PageRct = GetPageRect(PageName);

    //m_Ctrl.SetXObjArray(gs_pTheSFELib);
    //m_Ctrl.AddXObjArray(FindGrfWnd(PageName));

    m_Ctrl.SetXObjArray(FindGrfWnd(PageName));

    int RetCode = gs_Exec.SCInsertNodeG(m_Ctrl, tag, Guid, path, model, shape, boundingRect, angle, tagArea, tagAngle, FillColor, mirrorX, mirrorY);


    if (RetCode!=EOSC_DONE)
      {
      LogError(NETSERVERNAME, 0, "CreateNode '%s' failed!", tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }
    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception in CreateNode '%s'", tag);
    }

  ON_EXIT_G("OnCreateNodeG", Guid);
  }

//------------------------------------------------------------------------

//void CSvcConnect::OnCreateItem(__int64 eventId, __int64 requestId, List<Guid> created, )
//  {
//  }

void CSvcConnect::OnCreateNodeM(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, LPCSTR model)
  {

  // !!! tagArea not handled as yet.

  ON_ENTRY_GTM("OnCreateNodeM", Guid, tag, model);

  try
    {

    //CString PageName=CSvcConnect::ExtractPageName(path);

    //CGrfWnd * pWnd=FindGrfWnd(PageName);
    //PageRct = GetPageRect(PageName);

    m_Ctrl.SetXObjArray(gs_pTheSFELib);
    //m_Ctrl.AddXObjArray(FindGrfWnd(PageName));

    int RetCode = gs_Exec.SCInsertNodeM(m_Ctrl, tag, Guid, model);


    if (RetCode!=EOSC_DONE)
      {
      LogError(NETSERVERNAME, 0, "CreateNode '%s' failed!", tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }
    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception in CreateNode '%s'", tag);
    }

  ON_EXIT_G("OnCreateNodeM", Guid);
  }

void CSvcConnect::OnDeleteNodeM(__int64 eventId, __int64 requestId, LPCSTR Guid)
  {
  Strng Tag;
  flag IsLink;
  if (gs_pPrj->FindNodeInfoFromGuid((LPSTR)Guid, Tag, IsLink))
    {

    ON_ENTRY_GT("OnDeleteNodeM", Guid, Tag());

    try
      {
      m_Ctrl.SetXObjArray(gs_pTheSFELib);

      int RetCode = gs_Exec.SCDeleteNodeM(m_Ctrl, Tag(), Guid);


      if (RetCode!=EOSC_DONE)
        {
        LogError(NETSERVERNAME, 0, "DeleteNode '%s' failed!", Tag());
        //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
        }
      }
    catch(...)
      {
      LogError(NETSERVERNAME, 0, "Exception in CreateNode '%s'", Tag());
      }

    ON_EXIT_G("OnDeleteNodeM", Guid);
    }
  }

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

void CSvcConnect::GCBDeleteNode(DXF_ENTITY eEntity, LPCSTR Tag)
  {

  Strng ModelGuid, GraphicGuid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, ModelGuid))
    {
    DO_ENTRY_GT("GCBDeleteNode", ModelGuid(), Tag);

    __asm int 3; // what about GraphicGuid ???
    m_pCLR->DoDeleteNode(m_lRequestIdRet, ModelGuid(), GraphicGuid());

    DO_EXIT("GCBDeleteNode");
    }
  else
    {
    }
  }

//------------------------------------------------------------------------

void CSvcConnect::OnDeleteNodeG(__int64 eventId, __int64 requestId, LPCSTR Guid)
  {
  ON_ENTRY_GT("OnDeleteItem", Guid, "");

  Strng Tag;
  flag IsLink;
  Strng_List Pages;
  if (gs_pPrj->FindNodeInfoFromGuid((LPSTR)Guid, Tag, IsLink))
    {
    if (!IsLink)
      {
      m_Ctrl.ClrXObjArray();

      int RetCode = gs_Exec.SCDeleteNodeG(m_Ctrl, Tag(), Guid, "???");
      if (RetCode!=EOSC_DONE)
        {
        LogError(Tag(), 0, "Model not deleted");
        //DeletesFailedCnt++;
        }

      }
    else
      {
      // Links should be deleted by OnDeleteLink
      }
    }
  else
    {
    // ... Error
    }

  ON_EXIT("OnDeleteItem");
  };

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

void CSvcConnect::GCBModifyNodePosition(CGrfDoc *pDoc, DXF_ENTITY eEntity, LPCSTR Tag, Pt_3f Delta)
  {
  //LPSTR Tag="?Tag?";

  Strng Guid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, Guid))
    {
    DO_ENTRY_GT("GCBModifyItem", Guid(), Tag);

    CRectangleF boundingRect;
    bool MirrorX=false;
    bool MirrorY=false;
    Delta.Y=-Delta.Y; // Y is inverted

    //m_pCLR->DoModifyItemPosition(m_lRequestIdRet, Guid(), Delta);

    DO_EXIT("GCBModifyItem");
    }
  else
    {
    }
  };

//------------------------------------------------------------------------

void CSvcConnect::OnModifyNodeG(__int64 eventId, __int64 requestId, LPCSTR ItemGuid, LPCSTR tag, LPCSTR path, 
                                LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                                float angle, const CRectangleF & tagArea, float tagAngle, COLORREF Colour, 
                                bool mirrorX, bool mirrorY)
  {
  ON_ENTRY_GT("OnModifyNodeG", ItemGuid, tag);

  CString PageName=CSvcConnect::ExtractPageName(path);
  m_Ctrl.SetXObjArray(FindGrfWnd(PageName));

  int RetCode = gs_Exec.SCModifyNodeG(m_Ctrl, tag, ItemGuid, path, model, shape, boundingRect, angle, tagArea, tagAngle, Colour, mirrorX, mirrorY);
  if (RetCode!=EOSC_DONE)
    {
    LogError(tag, 0, "Model not modified");
    //DeletesFailedCnt++;
    }
  else
    {
    }

  ON_EXIT("OnModifyNodeG");
  };

//------------------------------------------------------------------------

void CSvcConnect::OnModifyNodeM(__int64 eventId, __int64 requestId, LPCSTR ItemGuid, LPCSTR tag, LPCSTR model)
  {
  ON_ENTRY_GT("OnModifyNodeM", ItemGuid, tag);

  int RetCode = gs_Exec.SCModifyNodeM(m_Ctrl, tag, ItemGuid, model);
  if (RetCode!=EOSC_DONE)
    {
    LogError(tag, 0, "Model not modified");
    //DeletesFailedCnt++;
    }
  else
    {
    }

  ON_EXIT("OnModifyNodeM");
  };

//========================================================================
//
//
//
//========================================================================

void CSvcConnect::GCBCreateLink(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR ClassId, 
                                LPCSTR SrcTag, LPCSTR DstTag, LPCSTR SrcPort, LPCSTR DstPort, 
                                CPointFList & ControlPoints)//, const CRectangleF & tagArea)
  {
  double Width=20;
  double Height=20;

  CRectangleF PageRct = GetPageRect(Page);
  //CRectangleF boundingRect(Pt.x()-0.5*Width,PageRct.Top()-Pt.y()-0.5*Height,Width, Height);
  // TO Fix
  CString Shape = ExtractShape(ClassId);//Symbol);

  //FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(SrcTag, NULL);
  //FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(DstTag, NULL);

  DO_ENTRY_GTP("GCBCreateLink", "NULL-Guid", Tag, MakePath(Prj, Page));

  CString GuidRet;

  POSITION Pos=ControlPoints.GetHeadPosition();
  while (Pos)
    {
    CPointF & Pt=ControlPoints.GetNext(Pos);
    Pt.Set(PageRct.Left()+Pt.X(),PageRct.Top()-Pt.Y());
    }

  //float textBoxW = int(GTI.m_Tag.m_XScale * 3.0 * GTI.m_sTag.GetLength());
  //float textBoxH = int(GTI.m_Tag.m_YScale * 5.0);
  //float textBoxX = int(GTI.m_Tag.m_X + Grp.m_XOff - textBoxW / 2.0) + 0.5; // needs to be x.5mm to meet grid in 10.
  //float textBoxY = int(Grp.m_PageRct.Height() - GTI.m_Tag.m_Y + Grp.m_YOff - textBoxH) + 0.5; // needs to be x.5mm to meet grid in 10.
  float textBoxW = 0.0;
  float textBoxH = 0.0;
  float textBoxX = 0.0;
  float textBoxY = 0.0;


  //m_pCLR->DoCreateLink(m_lRequestIdRet, GuidRet, Tag, MakePath(Prj, Page), ClassId, 
  //  pSrc?pSrc->Guid():"?", pDst?pDst->Guid():"?", SrcPort, DstPort, ControlPoints,
  //  CRectangleF(textBoxX, textBoxY, textBoxW, textBoxH),
  //0.0);

  DO_EXIT_G("GCBCreateLink", GuidRet);
  };

//------------------------------------------------------------------------

void CSvcConnect::OnCreateLinkG(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, /*LPCSTR path,*/ 
                                LPCSTR ClassId, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                CPointFList & ControlPoints,
                                const CRectangleF & tagArea, float tagAngle)
  {
  // !!! tagArea not handled as yet.

  ON_ENTRY_GT("OnCreateLinkG", Guid, tag);

#if WITHDBGLINES
  if (dbgConnect())
    {
    dbgpln(" Src:%s", OriginGuid);                                 
    dbgpln(" Dst:%s", DestinationGuid);                                 
    }
#endif

  try
    {

    FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(NULL, OriginGuid);
    FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(NULL, DestinationGuid);

    CRectangleF boundingRect;
    POSITION Pos=ControlPoints.GetHeadPosition();
    if (Pos)
      {
      boundingRect.Set(ControlPoints.GetNext(Pos));
      while (Pos)
        boundingRect.Include(ControlPoints.GetNext(Pos), false);
      }

    CsGrfGroup * pGrp=CSvcConnect::GetContainingGroup(boundingRect);

    m_Ctrl.ClrXObjArray();
    if (pGrp)
      {
      CString GroupName=pGrp->m_Name;
      m_Ctrl.SetXObjArray(FindGrfWnd(GroupName));
      }

    int RetCode = gs_Exec.SCInsertLinkG(m_Ctrl, tag, Guid, /*path,*/ ClassId, 
      OriginGuid, DestinationGuid, 
      pSrc?pSrc->Tag():"", pDst?pDst->Tag():"",
      ControlPoints, tagArea, tagAngle);

    if (RetCode!=EOSC_DONE)
      {
      LogError(NETSERVERNAME, 0, "CreateLink '%s' failed!", tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }

    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception in CreateUnit '%s'", tag);
    }

  ON_EXIT_G("OnCreateLinkG", Guid);
  };

//------------------------------------------------------------------------

void CSvcConnect::OnCreateLinkM(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, /*LPCSTR path,*/ 
                                LPCSTR ClassId, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                LPCSTR OriginPort, LPCSTR DestinationPort)
  {
  // !!! tagArea not handled as yet.

  ON_ENTRY_GT("OnCreateLinkM", Guid, tag);

  try
    {

    FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(NULL, OriginGuid);
    FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(NULL, DestinationGuid);


    m_Ctrl.SetXObjArray(gs_pTheSFELib);

    int RetCode = gs_Exec.SCInsertLinkM(m_Ctrl, tag, Guid, /*path,*/ ClassId, 
      OriginGuid, DestinationGuid, 
      pSrc?pSrc->Tag():"", pDst?pDst->Tag():"",
      OriginPort, DestinationPort);

    if (RetCode!=EOSC_DONE)
      {
      LogError(NETSERVERNAME, 0, "CreateLink '%s' failed!", tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }

    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception in CreateUnit '%s'", tag);
    }

  ON_EXIT_G("OnCreateLinkM", Guid);
  };

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

void CSvcConnect::GCBDeleteLink(DXF_ENTITY eEntity, LPCSTR Tag)
  {

  Strng Guid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, Guid))
    {
    DO_ENTRY_GT("GCBDeleteLink", Guid(), Tag);

    //m_pCLR->DoDeleteLink(m_lRequestIdRet, Guid());

    DO_EXIT("GCBDeleteLink");
    }
  else
    {
    }
  }

//------------------------------------------------------------------------

void CSvcConnect::OnDeleteLinkM(__int64 eventId, __int64 requestId, LPCSTR Guid)
  {
  ON_ENTRY_GT("OnDeleteLink", Guid, "");

  Strng Tag;
  flag IsLink;
  Strng_List Pages;
  if (gs_pPrj->FindNodeInfoFromGuid((LPSTR)Guid, Tag, IsLink))
    {
    if (IsLink)
      {
      m_Ctrl.ClrXObjArray();

      int RetCode = gs_Exec.SCDeleteLinkM(m_Ctrl, Tag(), Guid);
      if (RetCode!=EOSC_DONE)
        {
        LogError(Tag(), 0, "Link not deleted");
        //DeletesFailedCnt++;
        }

      }
    else
      {
      // Links should be deleted by OnDeleteLink
      }
    }
  else
    {
    // ... Error
    }

  ON_EXIT("OnDeleteLink");
  };

//------------------------------------------------------------------------

void CSvcConnect::OnDeleteLinkG(__int64 eventId, __int64 requestId, LPCSTR Guid)
  {
  ON_ENTRY_GT("OnDeleteLink", Guid, "");

  Strng Tag;
  flag IsLink;
  Strng_List Pages;
  if (gs_pPrj->FindNodeInfoFromGuid((LPSTR)Guid, Tag, IsLink))
    {
    if (IsLink)
      {
      m_Ctrl.ClrXObjArray();

      int RetCode = gs_Exec.SCDeleteLinkG(m_Ctrl, Tag(), Guid);
      if (RetCode!=EOSC_DONE)
        {
        LogError(Tag(), 0, "Link not deleted");
        //DeletesFailedCnt++;
        }

      }
    else
      {
      // Links should be deleted by OnDeleteLink
      }
    }
  else
    {
    // ... Error
    }

  ON_EXIT("OnDeleteLink");
  };


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

void CSvcConnect::GCBModifyLinkPts(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, /*LPCSTR ClassId, 
                                                                                       LPCSTR SrcTag, LPCSTR DstTag, LPCSTR SrcPort, LPCSTR DstPort,*/ 
                                                                                       CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle)
  {

  Strng Guid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, Guid))
    {
    DO_ENTRY_GT("GCBModifyLinkPts", Guid(), Tag);

    CRectangleF PageRct = GetPageRect(Page);

    FlwNode * pLink = gs_pSfeSrvr->FE_FindNode(Tag, NULL);

    LPSTR     pClass  = pLink->ClassId();
    FlwNode * pSrcNd  = pLink->Nd_Rmt(0);
    LPSTR     pSrcIO  = pLink->IOArea_Rmt(0).IOName();
    FlwNode * pDstNd  = pLink->Nd_Rmt(1);
    LPSTR     pDstIO  = pLink->IOArea_Rmt(1).IOName();

    POSITION Pos=ControlPoints.GetHeadPosition();
    while (Pos)
      {
      CPointF &Pt=ControlPoints.GetNext(Pos);
      Pt.Set(PageRct.Left()+Pt.X(),PageRct.Top()-Pt.Y());
      }

    //m_pCLR->DoModifyLink(m_lRequestIdRet, Guid(), Tag, "?Path?", pClass, pSrcNd->Guid(), pDstNd->Guid(), pSrcIO, pDstIO, ControlPoints, tagArea, tagAngle);

    DO_EXIT("GCBModifyLinkPts");
    }
  else
    {
    }
  }

//------------------------------------------------------------------------

void CSvcConnect::OnModifyLinkG(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                                LPCSTR ClassId, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle)
  {
  ON_ENTRY_GT("OnModifyLinkG", LinkGuid, Tag);


  FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(NULL, OriginGuid);
  FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(NULL, DestinationGuid);

  CRectangleF boundingRect;
  POSITION Pos=ControlPoints.GetHeadPosition();
  if (Pos)
    {
    boundingRect.Set(ControlPoints.GetNext(Pos));
    while (Pos)
      boundingRect.Include(ControlPoints.GetNext(Pos), false);
    }

  CsGrfGroup * pGrp=CSvcConnect::GetContainingGroup(boundingRect);

  m_Ctrl.ClrXObjArray();
  //m_Ctrl.SetXObjArray(gs_pTheSFELib);
  if (pGrp)
    {
    CString GroupName=pGrp->m_Name;
    m_Ctrl.SetXObjArray(FindGrfWnd(GroupName));
    }

  int RetCode = gs_Exec.SCModifyLinkG(m_Ctrl, Tag, LinkGuid, ClassId, OriginGuid, DestinationGuid, pSrc->Tag(), pDst->Tag(), ControlPoints, tagArea, tagAngle);
  if (RetCode!=EOSC_DONE)
    {
    LogError(Tag, 0, "Link not modified");
    //DeletesFailedCnt++;
    }
  else
    {
    }

  ON_EXIT("OnModifyLinkG");
  };

//------------------------------------------------------------------------

void CSvcConnect::OnModifyLinkM(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                                LPCSTR ClassId, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                LPCSTR OriginPort, LPCSTR DestinationPort)
  {
  ON_ENTRY_GT("OnModifyLinkM", LinkGuid, Tag);


  FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(NULL, OriginGuid);
  FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(NULL, DestinationGuid);


  m_Ctrl.SetXObjArray(gs_pTheSFELib);

  int RetCode = gs_Exec.SCModifyLinkM(m_Ctrl, Tag, LinkGuid, ClassId, OriginGuid, DestinationGuid, pSrc->Tag(), pDst->Tag(), OriginPort, DestinationPort);
  if (RetCode!=EOSC_DONE)
    {
    LogError(Tag, 0, "Link not modified");
    //DeletesFailedCnt++;
    }
  else
    {
    }

  ON_EXIT("OnModifyLinkM");
  };

//========================================================================
//
// Export Stuff
//
//========================================================================

static struct CPageSizeInfo {LPCSTR Nm; float Scl;} s_PgInfo[]=
  {
    {"A5", 0.5f},
    {"A4", 0.7071f},
    {"A3", 1.0f},
    {"A2", 1.4142f},
    {"A1", 2.0f},
    {0}
  };


CGetExistingItems::CGetExistingItems()
  {
  m_nPages=0;

  CDocTemplate & GrfTemplate(ScdApp()->GraphTemplate());

  m_PageMap.InitHashTable(201);
  m_TagMap.InitHashTable(20001);

  POSITION Pos = GrfTemplate.GetFirstDocPosition();

  float MaxPageW=0;
  float MaxPageH=0;
  while (Pos)
    {
    CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(GrfTemplate.GetNextDoc(Pos));

    m_Groups.AddTail(new CGroup(pDoc->GetTitle(), m_nPages++, pDoc));
    CGroup &Grp=*m_Groups.GetTail();

    double PageX = 0;
    double PageY = 0;
    double PageW = 420;
    double PageH = 297;

    if (Grp.m_pDoc->GCB.pDrw->GetBounds())
      {
      double DrwX = C3_MIN_X(&Grp.m_pDoc->GCB.pDrw->m_Bounds);
      double DrwY = C3_MIN_Y(&Grp.m_pDoc->GCB.pDrw->m_Bounds);
      double DrwW = C3_MAX_X(&Grp.m_pDoc->GCB.pDrw->m_Bounds) - DrwX;
      double DrwH = C3_MAX_Y(&Grp.m_pDoc->GCB.pDrw->m_Bounds) - DrwY;
      bool FoundPageSz=false;
      for (int i=0; s_PgInfo[i].Nm; i++)
        {
        double Scl=s_PgInfo[i].Scl;
        double PgW=s_PgInfo[i].Scl*420;
        double PgH=s_PgInfo[i].Scl*297;
        if (DrwW<=PgW*1.02 && DrwH<=PgH*1.02)
          {
          PageW=PgW;
          PageH=PgH;
          FoundPageSz=true;
          break;
          }
        }
      if (!FoundPageSz)
        {
        PageX += (PageW-DrwW)*0.5;
        PageY += (PageH-DrwH)*0.5;
        }
      }
    else
      {
      }

    Grp.m_PageRct.Set(PageX, PageY, PageW, PageH);
    MaxPageW=float(Max(MaxPageW, PageW));
    MaxPageH=float(Max(MaxPageH, PageH));
    }

  Pos=m_Groups.GetHeadPosition();
  while (Pos)
    {
    CGroup & Grp=*m_Groups.GetNext(Pos);


    int NAcross=Max(1,int(Sqrt((double)m_nPages)+0.5));
    Grp.m_XOff=(Grp.m_No%NAcross)*MaxPageW*1.05f;
    Grp.m_YOff=(Grp.m_No/NAcross)*MaxPageH*1.05f;

    m_PageMap.SetAt(Grp.m_sTitle, &Grp);

    bool DoAllInserts=true;
    long nInArray = Grp.m_pDoc->GetTagListInfo(DoAllInserts, Grp.m_GTIA);
    for (long i=0; i<nInArray; i++)
      {
      CGroupIndex Inx;
      if (!m_TagMap.Lookup(Grp.m_GTIA[i].m_sTag(), Inx))
        {
        Inx.m_pGrp=&Grp;
        Inx.m_iGTIA=i;
        m_TagMap.SetAt(Grp.m_GTIA[i].m_sTag(), Inx);
        dbgpln("TagMap %s", Grp.m_GTIA[i].m_sTag());
        }
      }
    }


  long iRet=gs_pPrj->m_pFlwLib->FE_GetNodeList(m_Nodes);

  Pos=m_Nodes.GetHeadPosition();
  while (Pos)
    {
    CNodeListItem &I=m_Nodes.GetNext(Pos);
    m_Items.AddTail(new CItem(I.m_sTag, &I));

    dbgpln("Item %s", I.m_sTag);

    }
  }

//------------------------------------------------------------------------

CGetExistingItems::~CGetExistingItems()
  {
  while (m_Groups.GetCount())
    delete m_Groups.RemoveTail();
  while (m_Items.GetCount())
    delete m_Items.RemoveTail();
  }

//========================================================================
//
//
//
//========================================================================

#endif

//========================================================================
