#include "stdafx.h"
#include "grfdoc.h"
#include "msgwnd.h"

#include "flwnode.h"
#include "SvcConn.h"
#include "SvcConnCLR.h"

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

#define DO_EXIT(Method)                                                                                            \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i", Method, "", m_lRequestIdRet);                                                    \
  };

#define DO_EXIT_G(Method, Guid)                                                                                    \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7s %7I64i %s", Method, "", m_lRequestIdRet, Guid);                                           \
  };

#else

#define DO_ENTRY_GT(Method, Guid, Tag)             /* Do Nothing */
#define DO_ENTRY_GTP(Method, Guid, Tag, Path)      /* Do Nothing */
#define DO_EXIT(Method)                            /* Do Nothing */
#define DO_EXIT_G(Method, Guid)                    /* Do Nothing */

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
  //m_lRequestId = 2000; // temporary - should be zeros
  m_lEventId   = 0;

  //m_bExportBusy = false;
  }
CSvcConnect::~CSvcConnect()
  {
  }

bool CSvcConnect::Startup(LPCSTR projectPath, LPCSTR configPath)
  {
  m_GrfGrpsNames.InitHashTable(101);
  m_GrfGrpsGuids.InitHashTable(101);

  m_pCLR = new CSvcConnectCLR;
  return m_pCLR->Startup(this, projectPath, configPath);

  };

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


//bool CSvcConnect::RequestItemDefinitions(LPSTR Group, CFlwNodeDefinitionList & Defns)
//  {
//  gs_pPrj->RequestNodeDefinitions(Group, Defns);
//
//  if (1)
//    {
//    POSITION Pos=Defns.GetHeadPosition();
//    if (Pos)
//      dbgpln("=======================================================================");
//    while (Pos)
//      {
//      CFlwNodeDefinition &D=Defns.GetNext(Pos);
//      dbgpln("ClassNV     : %s", D.m_sClassNV());
//      dbgpln("Class       : %s", D.m_sClass());
//      dbgpln("Description : %s", D.m_sDescription());
//      dbgpln("ShortDesc   : %s", D.m_sShortDesc());
//      dbgpln("Category    : 0x%08x", D.m_Category);
//      dbgpln("IOs         : %i", D.m_IOs.GetCount());
//      for (int io=0; io<D.m_IOs.GetCount(); io++)
//        {
//        CRequestModelIOInfoRec & IO = D.m_IOs[io];
//        dbgpln("  %-9s : %4i %3i %3i %3i %2x %2i %s%s %s %s", 
//          IO.Name(), IO.iId, IO.nIORqd, IO.nIOMax, IO.Dirn, 
//          IO.m_dwType, IO.iGrp, IO.Input?"I":" ", IO.Output?"O":" ", IO.m_Hidden?"Hide":"    ", IO.Desc());
//      
//        }
//      dbgpln("=======================================================================");
//      }
//    }
//
//  return Defns.GetCount()>0;
//  }

void CSvcConnect::Export(LPCSTR projectPath, LPCSTR configPath)
  {
  m_Ctrl.m_bExportBusy = true;
  //m_pCLR->Export(projectPath, configPath);

  CGetExistingItems GI;

  int PrevPage=-1;
  CString Path;

  //    static __int64 RqID=0;

  while (GI.GetOne())
    {
    CGrfTagInfo & I = GI.Item();

#if dbgSvcConn
    if (dbgConnect())
      {
      dbgpln("Export Item/Link %i %-20s %-20s %-20s", GI.Type(), I.m_sTag(), I.m_sSymbol(), I.m_sClass());
      dbgindent(4);
      }
#endif

    // Simple Layout
    int NAcross=Max(1,int(Sqrt((double)GI.PageCount())+0.5));
    float XOffSet=(GI.PageNo()%NAcross)*310.0f*1.414f;
    float YOffSet=(GI.PageNo()/NAcross)*310.0f;

    if (PrevPage!=GI.PageNo())
      {
      PrevPage=GI.PageNo();

      CString GrpGuid = TaggedObject::CreateGuidStr(); // where should this come from

      DO_ENTRY_GTP("DoCreateGroupE", GrpGuid , GI.PageName(), MakePath(projectPath));

      m_pCLR->DoCreateGroup(m_lRequestIdRet, GrpGuid, GI.PageName(), MakePath(projectPath), 
        CRectangleF(GI.PageRct().Left()+XOffSet,GI.PageRct().Bottom()+YOffSet, GI.PageRct().Width(), GI.PageRct().Height()));

      DO_EXIT_G("DoCreateGroupE", GrpGuid);
      }

    switch (GI.Type())
      {
      case CGetExistingItems::eIsNode:
        {
        CString ItemGuid;
        CString Shape    = ExtractShape(I.m_sSymbol());
        CString Model    = I.m_sClass();

        DO_ENTRY_GTP("DoCreateItemE", "", I.m_sTag(), MakePath(projectPath, GI.PageName()));

        m_pCLR->DoCreateItem(m_lRequestIdRet, ItemGuid, I.m_sTag(),
          MakePath(projectPath, GI.PageName()), Model, Shape,
          CRectangleF(I.m_LoBnd.m_X+XOffSet, GI.PageRct().Height()-I.m_HiBnd.m_Y+YOffSet, I.m_HiBnd.m_X-I.m_LoBnd.m_X, I.m_HiBnd.m_Y-I.m_LoBnd.m_Y),
          0.0,  CRectangleF(0.0, 0.0, 0.0, 0.0), // !!! textArea not used!
          0, false, false);

        DO_EXIT_G("DoCreateItemE", ItemGuid);
        break;
        }

      case CGetExistingItems::eIsLink:
        {
        CString ItemGuid;
        CString Shape    = ExtractShape(I.m_sSymbol());
        CString Model    = I.m_sClass();

        DO_ENTRY_GTP("DoCreateLinkE", "", I.m_sTag(), MakePath(projectPath, GI.PageName()));

        CPointFList CtrlPts;
        for (int i=0; i<GI.m_LPA.GetCount(); i++)
          CtrlPts.AddHead(CPointF(GI.m_LPA[i].x+XOffSet, GI.PageRct().Height()-GI.m_LPA[i].y+YOffSet));

        m_pCLR->DoCreateLink(m_lRequestIdRet, ItemGuid, I.m_sTag(), 
          MakePath(projectPath, GI.PageName()), 
          Model, 
          GI.m_SrcGuid, GI.m_DstGuid, 
          GI.m_SrcPort, GI.m_DstPort, CtrlPts);


        DO_EXIT_G("DoCreateLinkE", ItemGuid);
        break;
        }
      }
#if dbgSvcConn
    if (dbgConnect())
      {
      dbgindent(-4);
      dbgpln("");
      }
#endif
    }

  m_Ctrl.m_bExportBusy = false;
  };

void CSvcConnect::Load()
  {
  m_pCLR->Load();
  };
void CSvcConnect::Save()
  {
  m_pCLR->Save();
  };

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

void CSvcConnect::GCBCreateItem(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR Symbol, LPCSTR ClassId, Pt_3f Pt, Pt_3f Scl, float Angle)
  {
  double Width=20;
  double Height=20;

  CRectangleF PageRct = GetPageRect(Page);
  CRectangleF boundingRect(Pt.x()-0.5*Width,PageRct.Top()-Pt.y()-0.5*Height,Width, Height);
  // TO Fix
  CString Shape = ExtractShape(ClassId);//Symbol);

  DO_ENTRY_GTP("GCBCreateItem", "NULL-Guid", Tag, MakePath(Prj, Page));

  CString GuidRet;

  m_pCLR->DoCreateItem(m_lRequestIdRet, GuidRet, Tag, MakePath(Prj, Page), ClassId, Shape, boundingRect, Angle, CRectangleF(0.0, 0.0, 0.0, 0.0), 0, false, false); // !!! textArea not used.

  DO_EXIT_G("GCBCreateItem", GuidRet);
  };

//------------------------------------------------------------------------

void CSvcConnect::OnCreateItem(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, LPCSTR path, 
                               LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect,
                               float angle, const CRectangleF & textArea, COLORREF FillColor, 
                               bool mirrorX, bool mirrorY)
  {

// !!! textArea not handled as yet.

  ON_ENTRY_GTP("OnCreateItem", Guid, tag, path);

  try
    {

    CString PageName=CSvcConnect::ExtractPageName(path);

    //CGrfWnd * pWnd=FindGrfWnd(PageName);
    //PageRct = GetPageRect(PageName);

    m_Ctrl.SetXObjArray(gs_pTheSFELib);
    m_Ctrl.AddXObjArray(FindGrfWnd(PageName));

    int RetCode = gs_Exec.SCInsertItem(m_Ctrl, tag, Guid, path, model, shape, boundingRect, angle, textArea, FillColor, mirrorX, mirrorY);


    if (RetCode!=EOSC_DONE)
      {
      LogError(NETSERVERNAME, 0, "CreateUnit '%s' failed!", tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }
    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception in CreateUnit '%s'", tag);
    }

  ON_EXIT_G("OnCreateItem", Guid);
  }

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

void CSvcConnect::GCBDeleteItem(DXF_ENTITY eEntity, LPCSTR Tag)
  {

  Strng Guid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, Guid))
    {
    DO_ENTRY_GT("GCBDeleteItem", Guid(), Tag);

    m_pCLR->DoDeleteItem(m_lRequestIdRet, Guid());

    DO_EXIT("GCBDeleteItem");
    }
  else
    {
    }
  }

//------------------------------------------------------------------------

void CSvcConnect::OnDeleteItem(__int64 eventId, __int64 requestId, LPCSTR Guid)
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
 
      int RetCode = gs_Exec.SCDeleteItem(m_Ctrl, Tag(), Guid, "???");
      if (RetCode!=EODT_DONE)
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

void CSvcConnect::GCBModifyItemPosition(CGrfDoc *pDoc, DXF_ENTITY eEntity, LPCSTR Tag, Pt_3f Delta)
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

    m_pCLR->DoModifyItemPosition(m_lRequestIdRet, Guid(), Delta);

    DO_EXIT("GCBModifyItem");
    }
  else
    {
    }
  };

//------------------------------------------------------------------------

void CSvcConnect::OnModifyItem(__int64 eventId, __int64 requestId, LPCSTR ItemGuid, LPCSTR tag, LPCSTR path, 
                               LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                               float angle, const CRectangleF & textArea, COLORREF Colour, 
                               bool mirrorX, bool mirrorY)
  {
  ON_ENTRY_GT("OnModifyItem", ItemGuid, tag);

  CString PageName=CSvcConnect::ExtractPageName(path);
  m_Ctrl.SetXObjArray(FindGrfWnd(PageName));

  int RetCode = gs_Exec.SCModifyItem(m_Ctrl, tag, ItemGuid, path, model, shape, boundingRect, angle, textArea, Colour, mirrorX, mirrorY);
  if (RetCode!=EODT_DONE)
    {
    LogError(tag, 0, "Model not modified");
    //DeletesFailedCnt++;
    }
  else
    {
    }

  ON_EXIT("OnModifyItem");
  };

//========================================================================
//
//
//
//========================================================================

void CSvcConnect::GCBCreateLink(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR ClassId, 
                      LPCSTR SrcTag, LPCSTR DstTag, LPCSTR SrcPort, LPCSTR DstPort, 
                      CPointFList & ControlPoints)//, const CRectangleF & textArea)
  {
  double Width=20;
  double Height=20;

  CRectangleF PageRct = GetPageRect(Page);
  //CRectangleF boundingRect(Pt.x()-0.5*Width,PageRct.Top()-Pt.y()-0.5*Height,Width, Height);
  // TO Fix
  CString Shape = ExtractShape(ClassId);//Symbol);

  FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(SrcTag, NULL);
  FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(DstTag, NULL);

  DO_ENTRY_GTP("GCBCreateLink", "NULL-Guid", Tag, MakePath(Prj, Page));

  CString GuidRet;

  POSITION Pos=ControlPoints.GetHeadPosition();
  while (Pos)
    {
    CPointF & Pt=ControlPoints.GetNext(Pos);
    Pt.Set(PageRct.Left()+Pt.X(),PageRct.Top()-Pt.Y());
    }

  m_pCLR->DoCreateLink(m_lRequestIdRet, GuidRet, Tag, MakePath(Prj, Page), ClassId, 
    pSrc?pSrc->Guid():"?", pDst?pDst->Guid():"?", SrcPort, DstPort, ControlPoints);

  DO_EXIT_G("GCBCreateLink", GuidRet);
  };

//------------------------------------------------------------------------

void CSvcConnect::OnCreateLink(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, /*LPCSTR path,*/ 
                                LPCSTR ClassId, 
                                LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                LPCSTR OriginPort, LPCSTR DestinationPort, 
                                CPointFList & ControlPoints, const CRectangleF & textArea)
  {
// !!! textArea not handled as yet.

  ON_ENTRY_GT("OnCreateLink", Guid, tag);

  try
    {

    FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(NULL, OriginGuid);
    FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(NULL, DestinationGuid);

    pSrc->Tag();

    CRectangleF boundingRect;
    POSITION Pos=ControlPoints.GetHeadPosition();
    if (Pos)
      {
      boundingRect.Set(ControlPoints.GetNext(Pos));
      while (Pos)
        boundingRect.Include(ControlPoints.GetNext(Pos), false);
      }

    CsGrfGroup * pGrp=CSvcConnect::GetContainingGroup(boundingRect);

    m_Ctrl.SetXObjArray(gs_pTheSFELib);
    if (pGrp)
      {
      CString GroupName=pGrp->m_Name;
      m_Ctrl.AddXObjArray(FindGrfWnd(GroupName));
      }

    int RetCode = gs_Exec.SCInsertLink(m_Ctrl, tag, Guid, /*path,*/ ClassId, 
                    OriginGuid, DestinationGuid, 
                    pSrc?pSrc->Tag():"", pDst?pDst->Tag():"",
                    OriginPort, DestinationPort, 
                    ControlPoints, textArea);

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

  ON_EXIT_G("OnCreateLink", Guid);
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

    m_pCLR->DoDeleteLink(m_lRequestIdRet, Guid());

    DO_EXIT("GCBDeleteLink");
    }
  else
    {
    }
  }

//------------------------------------------------------------------------

void CSvcConnect::OnDeleteLink(__int64 eventId, __int64 requestId, LPCSTR Guid)
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
 
      int RetCode = gs_Exec.SCDeleteLink(m_Ctrl, Tag(), Guid);
      if (RetCode!=EODT_DONE)
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

//========================================================================
//
// Export Stuff
//
//========================================================================

CGetExistingItems::CGetExistingItems() : \
m_GrfTemplate(ScdApp()->GraphTemplate())
  {
  m_nPages=0;
  m_GrfDocPos = m_GrfTemplate.GetFirstDocPosition();
  while (m_GrfDocPos)
    {
    m_nPages++;
    m_GrfTemplate.GetNextDoc(m_GrfDocPos);
    }

  m_GrfDocPos = m_GrfTemplate.GetFirstDocPosition();
  m_nInArray  = 0;
  m_iInArray  = 0;
  m_pDoc      = NULL;
  m_iPage     = -1;
  }

//------------------------------------------------------------------------

static struct CPageSizeInfo {LPCSTR Nm; float Scl;} s_PgInfo[]=
  {
    {"A5", 0.5f},
    {"A4", 0.7071f},
    {"A3", 1.0f},
    {"A2", 1.4142f},
    {"A1", 2.0f},
    {0}
  };

bool CGetExistingItems::GetOne()
  {
  for (;;)
    {
    m_iInArray++;

    if (m_iInArray>=m_nInArray)
      {// Get Next Page
      if (!m_GrfDocPos)
        return false; // no more pages

      bool DoAllInserts=true;
      m_pDoc=dynamic_cast<CGrfDoc*>(m_GrfTemplate.GetNextDoc(m_GrfDocPos));
      m_iPage++;
      m_sPage=m_pDoc->GetTitle();

      double PageX = 0;
      double PageY = 0;
      double PageW = 420;
      double PageH = 297;

      if (m_pDoc->GCB.pDrw->GetBounds())
        {
        double DrwX = C3_MIN_X(&m_pDoc->GCB.pDrw->Bounds);
        double DrwY = C3_MIN_Y(&m_pDoc->GCB.pDrw->Bounds);
        double DrwW = C3_MAX_X(&m_pDoc->GCB.pDrw->Bounds) - DrwX;
        double DrwH = C3_MAX_Y(&m_pDoc->GCB.pDrw->Bounds) - DrwY;
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

      m_PageRct.Set(PageX, PageY, PageW, PageH);

      m_nInArray = m_pDoc->GetTagListInfo(DoAllInserts, m_GTIA);
      m_iInArray=0;
      }

    if (m_iInArray<m_nInArray)
      {
      // Load Data
      CGrfTagInfo & I = m_GTIA[m_iInArray];

      bool DoneOne=false;
      m_Type = eIsError; //error
      if (I.m_bHasTag)
        {
        FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag(), NULL);
        if (pNode)
          {
          TagObjClass * pTagObjC = pNode->Class();
          I.m_sClass = pNode->ClassId();
          m_Guid = pNode->Guid();

          if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 || 
            _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 || 
            _stricmp(ElecLinkGrp, pTagObjC->Group())==0 || 
            _stricmp(AirLinkGrp, pTagObjC->Group())==0))
            m_Type = eIsLink; //link
          else
            m_Type = eIsNode; //unit/node
          if (pTagObjC)
            {
            I.m_sDrwGroup=pTagObjC->DrwGroup();
            }

          }
        else
          {
          m_Type = eIsNull;
          I.m_sClass = "";
          }

        switch (m_Type)
          {
          case eIsLink:
            {
            m_LPA.SetSize(0);
            m_pDoc->GCB.pDrw->CollectLinkInfo(I, m_LPA);
                        
            m_SrcGuid = pNode->Nd_Rmt(0)->Guid();
            m_SrcPort = pNode->IODesc_Rmt(0)->IOName();
            m_DstGuid = pNode->Nd_Rmt(1)->Guid();
            m_DstPort = pNode->IODesc_Rmt(1)->IOName();

            DoneOne = true;
            break;       
            }
          case eIsNode:
            {

            DoneOne = true;
            break;
            }
          }
        }
      if (DoneOne)
        {
        // Remove Original Symbol
        m_pDoc->GCB.pDsp->Draw(I.e, GrfHelper.GR_BACKGROUND);
        m_pDoc->GCB.pDrw->Delete(I.e);
        }

      if (DoneOne)
        return true;
      }
    }

  return false;
  }


//========================================================================
//
//
//
//========================================================================

#endif

//========================================================================
