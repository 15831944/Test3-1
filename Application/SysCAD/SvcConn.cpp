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
  m_lRequestId = 2000; // temporary - should be zeros
  m_lEventId   = 1000;

  //m_bExportBusy = false;
  }
CSvcConnect::~CSvcConnect()
  {
  }

bool CSvcConnect::Startup(LPCSTR projectPath, LPCSTR configPath, bool ImportScd9)
  {
  m_GrfGrpsNames.InitHashTable(101);
  m_GrfGrpsGuids.InitHashTable(101);

  m_pCLR = new CSvcConnectCLR;
  return m_pCLR->Startup(this, projectPath, configPath, ImportScd9);

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
      }
#endif

    // Simple Layout
    int NAcross=Max(1,int(Sqrt((double)GI.PageCount())+0.5));
    float XOffSet=(GI.PageNo()%NAcross)*310.0f*1.414f;
    float YOffSet=(GI.PageNo()/NAcross)*310.0f;

    if (PrevPage!=GI.PageNo())
      {
      PrevPage=GI.PageNo();

      CString GrpGuid = TaggedObject::CreateGUIDStr(); // where should this come from

      m_lRequestId++;

#if dbgSvcConn
      if (dbgConnect())
        {
        dbgpln("DoCreateGroupE >>> %7s %7I64i %s  %-20s  %s", "", m_lRequestId, GrpGuid , GI.PageName(), MakePath(projectPath));
        dbgindent(2);
        }
#endif

      m_pCLR->DoCreateGroup(m_lRequestIdRet, GrpGuid, GI.PageName(), MakePath(projectPath), 
        CRectangleF(GI.PageRct().Left()+XOffSet,GI.PageRct().Bottom()+YOffSet, GI.PageRct().Width(), GI.PageRct().Height()));

#if dbgSvcConn
      if (dbgConnect())
        {
        dbgindent(-2);
        dbgpln("DoCreateGroupE <<< %7s %7I64i %s  ", "", m_lRequestIdRet, GrpGuid);
        }
      }
#endif

    switch (GI.Type())
      {
      case CGetExistingItems::eIsNode:
        {

        CString ItemGuid;// = CreateGUIDStr(); // where should this come from
        CString Shape    = ExtractShape(I.m_sSymbol());
        CString Model    = I.m_sClass();

        m_lRequestId++;

#if dbgSvcConn
        if (dbgConnect())
          {
          dbgpln("DoCreateItemE >>> %7s %7I64i %s  %-20s  %s", "", m_lRequestId, "", I.m_sTag(), MakePath(projectPath, GI.PageName()));
          dbgindent(2);
          }
#endif

        m_pCLR->DoCreateItem(m_lRequestIdRet, ItemGuid, I.m_sTag(),
          MakePath(projectPath, GI.PageName()), Model, Shape,
          CRectangleF(I.m_LoBnd.m_X+XOffSet, GI.PageRct().Height()-I.m_HiBnd.m_Y+YOffSet, I.m_HiBnd.m_X-I.m_LoBnd.m_X, I.m_HiBnd.m_Y-I.m_LoBnd.m_Y),
          0.0,  CRectangleF(0.0, 0.0, 0.0, 0.0), // !!! textArea not used!
		  0, false, false);

#if dbgSvcConn
        if (dbgConnect())
          {
          dbgindent(-2);
          dbgpln("DoCreateItemE <<< %7s %7I64i %s ", "", m_lRequestIdRet, ItemGuid);
          }
#endif
        break;
        }

      case CGetExistingItems::eIsLink:
        {
        break;
        }
      }
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

#define DO_ENTRY_GT(Method, Guid, Tag)                                                                             \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7s %7I64i %s %s", Method, "", m_lRequestId, Guid, Tag);                                      \
  dbgindent(2);                                                                                                    \
  }

#define DO_ENTRY_GTP(Method, Guid, Tag, Path)                                                                      \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7s %7I64i %s  %-20s  %s", Method, "", m_lRequestId, Guid, Tag, Path);                        \
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

//========================================================================

#define ON_ENTRY_GT(Method, Guid, Tag)                                                                             \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7I64i %7I64i %s %s", Method, eventId, m_lRequestId, Guid, Tag);                              \
  dbgindent(2);                                                                                                    \
  }

#define ON_ENTRY_GTP(Method, Guid, Tag, Path)                                                                      \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgpln("%-30s  >>> %7I64i %7I64i %s  %-20s  %s", Method, eventId, m_lRequestId, Guid, Tag, Path);                \
  dbgindent(2);                                                                                                    \
  }

#define ON_EXIT(Method)                                                                                            \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7I64i %7I64i", Method, eventId, m_lRequestIdRet);                                            \
  };

#define ON_EXIT_G(Method, Guid)                                                                                    \
if (dbgConnect())                                                                                                  \
  {                                                                                                                \
  dbgindent(-2);                                                                                                   \
  dbgpln("%-30s  <<< %7I64i %7I64i %s", Method, eventId, m_lRequestIdRet, Guid);                                   \
  };

//========================================================================
//
//
//
//========================================================================

void CSvcConnect::DoCreateItem(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR Symbol, LPCSTR ClassId, Pt_3f Pt, Pt_3f Scl, float Angle)
  {
  double Width=20;
  double Height=20;

  CRectangleF PageRct = GetPageRect(Page);
  CRectangleF boundingRect(Pt.x()-0.5*Width,PageRct.Top()-Pt.y()-0.5*Height,Width, Height);
  // TO Fix
  CString Shape = ExtractShape(ClassId);//Symbol);

  m_lRequestId++;

  DO_ENTRY_GTP("DoCreateItem", "NULL-Guid", Tag, MakePath(Prj, Page));

  CString GuidRet;

  m_pCLR->DoCreateItem(m_lRequestIdRet, GuidRet, Tag, MakePath(Prj, Page), ClassId, Shape, boundingRect, Angle, CRectangleF(0.0, 0.0, 0.0, 0.0), 0, false, false); // !!! textArea not used.

  DO_EXIT_G("DoCreateItem", GuidRet);
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

    int RetCode = gs_Exec.SCInsertTag(m_Ctrl, tag, Guid, path, model, shape, boundingRect, angle, textArea, FillColor, mirrorX, mirrorY);


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

void CSvcConnect::DoDeleteItem(DXF_ENTITY eEntity, LPCSTR Tag)
  {

  m_lRequestId++;

  Strng Guid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, Guid))
    {
    DO_ENTRY_GT("DoDeleteItem", Guid(), Tag);

    m_pCLR->DoDeleteItem(m_lRequestIdRet, Guid());

    DO_EXIT("DoDeleteItem");
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
 
      int RetCode = gs_Exec.SCDeleteTag(m_Ctrl, Tag(), Guid, "???");
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

void CSvcConnect::DoModifyItemPosition(CGrfDoc *pDoc, DXF_ENTITY eEntity, LPCSTR Tag, Pt_3f Delta)
  {
  m_lRequestId++;
  //LPSTR Tag="?Tag?";

  Strng Guid;
  if (gs_pPrj->GetNodeGuid((LPSTR)Tag, Guid))
    {
    DO_ENTRY_GT("DoModifyItem", Guid(), Tag);

    CRectangleF boundingRect;
    bool MirrorX=false;
    bool MirrorY=false;
    Delta.Y=-Delta.Y; // Y is inverted

    m_pCLR->DoModifyItemPosition(m_lRequestId, Guid(), Delta);

    DO_EXIT("DoModifyItem");
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

  int RetCode = gs_Exec.SCModifyTag(m_Ctrl, tag, ItemGuid, path, model, shape, boundingRect, angle, textArea, Colour, mirrorX, mirrorY);
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
        FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag());
        if (pNode)
          {
          TagObjClass * pTagObjC = pNode->Class();
          I.m_sClass = pNode->ClassId();
          m_Guid = pNode->EqpGUID();

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

            CLinePointsArray LPA;
            m_pDoc->GCB.pDrw->CollectLinkInfo(I, LPA);

            //CNSMdlLink * pMdl = new CNSMdlLink(I.m_sTag(), Guid(), I.m_sClass(),
            //  pNode->Nd_Rmt(0)->EqpGUID(), pNode->Nd_Rmt(1)->EqpGUID(),
            //  pNode->IOArea_Rmt(0).m_pName, pNode->IOArea_Rmt(1).m_pName);
            //CNSGrfLink * pGrf = new CNSGrfLink(Page(), pMdl, I, LPA);
            //for (int i=0; i<LPA.GetSize(); i++)
            //  pGrf->m_Pts.Add(CNSGrfLink::CPt(LPA[i].x, LPA[i].y));
            //pMdl->m_pGrf = pGrf;
            //m_Guids.AddTail(pMdl);

            //DoneOne = true;
            break;       
            }
          case eIsNode:
            {
            Strng tag = I.m_sTag();

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
