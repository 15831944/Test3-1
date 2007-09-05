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


void CSvcConnect::Export2Scd10(LPCSTR projectPath, LPCSTR configPath)
  {
  m_Ctrl.m_bExportBusy = true;
  //m_pCLR->Export(projectPath, configPath);

  CGetExistingItems GI;

  int PrevPage=-1;
  CString Path;

  POSITION Pos=GI.m_Groups.GetHeadPosition();
  while (Pos)
    {
    CGetExistingItems::CGroup & Grp=*GI.m_Groups.GetNext(Pos);

    CString GrpGuid = TaggedObject::CreateGuidStr(); // where should this come from

    DO_ENTRY_GTP("DoCreateGroupE", GrpGuid , Grp.m_sTitle, MakePath(projectPath));

    m_pCLR->DoCreateGroup(m_lRequestIdRet, GrpGuid, Grp.m_sTitle, MakePath(projectPath), 
      CRectangleF(Grp.m_PageRct.Left()+Grp.m_XOff,Grp.m_PageRct.Bottom()+Grp.m_YOff, Grp.m_PageRct.Width(), Grp.m_PageRct.Height()));

    DO_EXIT_G("DoCreateGroupE", GrpGuid);
    
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
      CString ItemGuid;
      CString Shape    = ExtractShape(Symbol);
      CString Model    = N.m_sClass;

      DO_ENTRY_GTPSM("DoCreateItemE", "", I.m_sTag, MakePath(projectPath, Grp.m_sTitle), Shape, Model);

      float textBoxW = GTI.m_Tag.m_XScale * 3.0 * GTI.m_sTag.GetLength();
      float textBoxH = GTI.m_Tag.m_YScale * 5.0;
      float textBoxX = GTI.m_Tag.m_X + Grp.m_XOff - textBoxW / 2.0;
      float textBoxY = Grp.m_PageRct.Height() - GTI.m_Tag.m_Y + Grp.m_YOff - textBoxH;

      m_pCLR->DoCreateItem(m_lRequestIdRet, ItemGuid, I.m_sTag,
        MakePath(projectPath, Grp.m_sTitle), Model, Shape,
        CRectangleF(GTI.m_LoBnd.m_X+Grp.m_XOff, Grp.m_PageRct.Height()-GTI.m_HiBnd.m_Y+Grp.m_YOff, GTI.m_HiBnd.m_X-GTI.m_LoBnd.m_X, GTI.m_HiBnd.m_Y-GTI.m_LoBnd.m_Y),
        0.0, 
				CRectangleF(textBoxX, textBoxY, textBoxW, textBoxH),
				-GTI.m_Tag.m_Rotation,
        0, false, false);

      DO_EXIT_G("DoCreateItemE", ItemGuid);
      }
    else
      {
      CString ItemGuid;
      CString Shape    = ExtractShape(Symbol);
      CString Model    = N.m_sClass;

      DO_ENTRY_GTP("DoCreateLinkE", "", I.m_sTag, MakePath(projectPath, Grp.m_sTitle));

      CLinePointsArray  LPA;

      LPA.SetSize(0);
      Grp.m_pDoc->GCB.pDrw->CollectLinkInfo(GTI, LPA);

      FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag, NULL);

      CString SrcGuid = pNode->Nd_Rmt(0)->Guid();
      CString SrcPort = pNode->IODesc_Rmt(0)->IOName();
      CString DstGuid = pNode->Nd_Rmt(1)->Guid();
      CString DstPort = pNode->IODesc_Rmt(1)->IOName();
      if (dbgConnect())                                                              
        {                                                                            
        dbgpln("%-30s   Src %s.%s", "", SrcGuid, SrcPort);
        dbgpln("%-30s   Dst %s.%s", "", DstGuid, DstPort);
        }


      CPointFList CtrlPts;
      for (int i=0; i<LPA.GetCount(); i++)
        CtrlPts.AddTail(CPointF(LPA[i].x+Grp.m_XOff, Grp.m_PageRct.Height()-LPA[i].y+Grp.m_YOff));

      m_pCLR->DoCreateLink(m_lRequestIdRet, ItemGuid, I.m_sTag, 
        MakePath(projectPath, Grp.m_sSymbol), 
        Model, 
        SrcGuid, DstGuid, SrcPort, DstPort, CtrlPts);


      DO_EXIT_G("DoCreateLinkE", ItemGuid);
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
  //CString Shape = ExtractShape(ClassId);//Symbol);
  CString Shape = ExtractShape(Symbol);

  DO_ENTRY_GTP("GCBCreateItem", "NULL-Guid", Tag, MakePath(Prj, Page));

  CString GuidRet;

  m_pCLR->DoCreateItem(m_lRequestIdRet, GuidRet, Tag, MakePath(Prj, Page), ClassId, Shape, boundingRect, Angle, CRectangleF(0.0, 0.0, 0.0, 0.0), 0.0, 0, false, false); // !!! textArea not used.

  DO_EXIT_G("GCBCreateItem", GuidRet);
  };

//------------------------------------------------------------------------

void CSvcConnect::OnCreateItem(__int64 eventId, __int64 requestId, LPCSTR Guid, LPCSTR tag, LPCSTR path, 
                               LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect,
                               float angle, const CRectangleF & textArea, float textAngle, COLORREF FillColor, 
                               bool mirrorX, bool mirrorY)
  {

// !!! textArea not handled as yet.

  ON_ENTRY_GTPSM("OnCreateItem", Guid, tag, path, shape, model);

  try
    {

    CString PageName=CSvcConnect::ExtractPageName(path);

    //CGrfWnd * pWnd=FindGrfWnd(PageName);
    //PageRct = GetPageRect(PageName);

    m_Ctrl.SetXObjArray(gs_pTheSFELib);
    m_Ctrl.AddXObjArray(FindGrfWnd(PageName));

    int RetCode = gs_Exec.SCInsertItem(m_Ctrl, tag, Guid, path, model, shape, boundingRect, angle, textArea, textAngle, FillColor, mirrorX, mirrorY);


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
                               float angle, const CRectangleF & textArea, float textAngle, COLORREF Colour, 
                               bool mirrorX, bool mirrorY)
  {
  ON_ENTRY_GT("OnModifyItem", ItemGuid, tag);

  CString PageName=CSvcConnect::ExtractPageName(path);
  m_Ctrl.SetXObjArray(FindGrfWnd(PageName));

  int RetCode = gs_Exec.SCModifyItem(m_Ctrl, tag, ItemGuid, path, model, shape, boundingRect, angle, textArea, textAngle, Colour, mirrorX, mirrorY);
  if (RetCode!=EOSC_DONE)
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
                                CPointFList & ControlPoints)//, const CRectangleF & textArea);
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
    m_pCLR->DoModifyLink(m_lRequestIdRet, Guid(), Tag, "?Path?", pClass, pSrcNd->Guid(), pDstNd->Guid(), pSrcIO, pDstIO, ControlPoints);

    DO_EXIT("GCBModifyLinkPts");
    }
  else
    {
    }
  }

//------------------------------------------------------------------------

void CSvcConnect::OnModifyLink(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                              LPCSTR ClassId, 
                              LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                              LPCSTR OriginPort, LPCSTR DestinationPort, 
                              CPointFList & ControlPoints, const CRectangleF & textArea)
  {
  ON_ENTRY_GT("OnModifyLink", LinkGuid, Tag);


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

  m_Ctrl.SetXObjArray(gs_pTheSFELib);
  if (pGrp)
    {
    CString GroupName=pGrp->m_Name;
    m_Ctrl.AddXObjArray(FindGrfWnd(GroupName));
    }

  //CString PageName=CSvcConnect::ExtractPageName(path);
  //m_Ctrl.SetXObjArray(FindGrfWnd(PageName));

  int RetCode = gs_Exec.SCModifyLink(m_Ctrl, Tag, LinkGuid, ClassId, OriginGuid, DestinationGuid, pSrc->Tag(), pDst->Tag(), OriginPort, DestinationPort, ControlPoints, textArea);
  if (RetCode!=EOSC_DONE)
    {
    LogError(Tag, 0, "Link not modified");
    //DeletesFailedCnt++;
    }
  else
    {
    }

  ON_EXIT("OnModifyLink");
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
      double DrwX = C3_MIN_X(&Grp.m_pDoc->GCB.pDrw->Bounds);
      double DrwY = C3_MIN_Y(&Grp.m_pDoc->GCB.pDrw->Bounds);
      double DrwW = C3_MAX_X(&Grp.m_pDoc->GCB.pDrw->Bounds) - DrwX;
      double DrwH = C3_MAX_Y(&Grp.m_pDoc->GCB.pDrw->Bounds) - DrwY;
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
    MaxPageW=Max(MaxPageW, PageW);
    MaxPageH=Max(MaxPageH, PageH);
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
