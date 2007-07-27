#include "stdafx.h"
#include "grfdoc.h"
#include "msgwnd.h"

#include "NetServerU.h"
#include "flwnode.h"

#define NETSERVERNAME "NetServer"

//========================================================================

#if SYSCAD10

//========================================================================
//
// Common
//
//========================================================================

static int CheckTagExists(LPCTSTR Tag)
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

static CDocument* GetGrfDoc(int index, LPCTSTR name)
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

class CSSGrfGroup
  {
  public:
    CString       m_Guid;
    CString       m_Name;
    CString       m_Path;

    CRectangleF   m_Bounds;
  };

class CSSGrfGroupMap : public CMap <LPCSTR, LPCSTR, CSSGrfGroup*, CSSGrfGroup*> {};

CSSGrfGroupMap gs_GrfGrpsNames;
CSSGrfGroupMap gs_GrfGrpsGuids;

//========================================================================

void SS_Initialise()
  {
  gs_GrfGrpsNames.InitHashTable(101);
  gs_GrfGrpsGuids.InitHashTable(101);

  };

//========================================================================

void SS_Terminate()
  {
  POSITION Pos=gs_GrfGrpsNames.GetStartPosition();
  while (Pos)
    {
    LPCSTR Key;
    CSSGrfGroup *pG;
    gs_GrfGrpsNames.GetNextAssoc(Pos, Key, pG);
    delete pG;
    }

  gs_GrfGrpsNames.RemoveAll();
  gs_GrfGrpsGuids.RemoveAll();
  };

//========================================================================

void SS_CreateGroup(bool DoingExport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                    const CRectangleF & boundingRect)
  {


  CSSGrfGroup * pG = new CSSGrfGroup;
  pG->m_Guid=guid;
  pG->m_Name=tag;
  pG->m_Path=path;
  pG->m_Bounds=boundingRect;

  gs_GrfGrpsNames.SetAt(pG->m_Name, pG);
  gs_GrfGrpsGuids.SetAt(pG->m_Guid, pG);

  };

//========================================================================

void SS_CreateItem(bool DoingExport, __int64 eventId, __int64 requestId, LPCTSTR Guid, LPCTSTR Tag, LPCTSTR Path, 
                   LPCTSTR ClassId, LPCTSTR Symbol, const CRectangleF & boundingRect,
                   double Angle, COLORREF FillColor, 
                   bool MirrorX, bool MirrorY)
  {

  try
    {
    bool Error = false;
    if (!DoingExport)
      {
      Error = (CheckTagExists(Tag)==0);
      if (Error)
        {
        LogError(NETSERVERNAME, 0, "Tag not found : AddUnit '%s' failed!", Tag);
        // return Scd.Return(eScdGraphicCode_TagNotFound, "Tag not found : AddUnit '%s' failed!", Tag);
        }

      }

    CRectangleF PageRct(0.0, 420, 0, 297);
    CDocument* pDoc=NULL;
    if (!Error)
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

      pDoc=GetGrfDoc(-1, Pg.GetLength()>0?Pg.GetBuffer():NULL);//m_sName);
      Error = (pDoc==NULL);
      if (!Error)
        {
        CSSGrfGroup *pG;
        if (gs_GrfGrpsNames.Lookup(Pg, pG))
          {
          PageRct = pG->m_Bounds;
          }
        }
      }


    if (!Error)
      {
      //RequestModelInfoRec MInfo;
      //Strng ClassId;
      //flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
      //ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
      //b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
      //ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);

      CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
      CInsertBlk CB; 

      CB.ATag=Tag;
      CB.AClass=ClassId;//();
      CB.Pt.World.X=boundingRect.MidX()-PageRct.Left(); // not inversion
      CB.Pt.World.Y=PageRct.Bottom()-boundingRect.MidY();  // this must be inverted
      CB.Pt.World.Z=0;
      CB.NdScl.X=MirrorX ? -1:1;//XScale;
      CB.NdScl.Y=MirrorY ? -1:1;
      CB.NdScl.Z=1;
      CB.Rotate=(float)Angle;
      CB.ATagBase="XX_";//MInfo.TagInitialID();
      //if (Symbol && strchr(Symbol, ':'))
      CB.ASymbol=Symbol;
      //else
      //  {
      //  CB.ASymbol="XX";//MInfo.DrwGroup();
      //  CB.ASymbol+=':';
      //  CB.ASymbol+=Symbol;
      //  }
      //CB.ASymbol+=".dxf";
        
      //CB.m_BoundRct=

      if (DoingExport)
        pGDoc->GCB.DoInsertNodeGrf(&CB);
      else
        pGDoc->GCB.DoInsertNode(&CB);
        
      }

    if (Error)
      {
      LogError(NETSERVERNAME, 0, "CreateUnit '%s' failed!", Tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }
    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception in CreateUnit '%s'", Tag);
    }
  }

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
  
static struct CPageSizeInfo {LPCTSTR Nm; float Scl;} s_PgInfo[]=
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

            DoneOne = true;
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
