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
//
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


static bool AddUnitGrf(bool DoingExport, LPCSTR Tag, LPCTSTR Path, double X, double Y, double Z, LPCSTR Symbol, LPCSTR ClassId, double XScale, double YScale, double Rotation)
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
      CB.Pt.World.X=X;
      CB.Pt.World.Y=Y;
      CB.Pt.World.Z=Z;
      CB.NdScl.X=XScale;
      CB.NdScl.Y=YScale;
      CB.NdScl.Z=YScale;
      CB.Rotate=(float)Rotation;
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
      LogError(NETSERVERNAME, 0, "AddUnit '%s' failed!", Tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }
    return !Error;
    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception while Adding Unit '%s'", Tag);
    }
  return false;  
  }

//========================================================================
//
//
//
//========================================================================


void SS_CreateItem(bool DoingExport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                   LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect,
                   double angle, COLORREF fillColor, 
                   bool mirrorX, bool mirrorY)
  {
  //296 = hgt of a3 sheat
  if (!AddUnitGrf(DoingExport, tag, path, boundingRect.MidX(), 296.0-boundingRect.MidY(), 0.0, shape, model, mirrorX ? -1.0:1.0/*XScale*/, mirrorY ? -1.0:1.0/*YScale*/, angle))
    {
    }
  

  int xxx=0;
  }

//========================================================================
//
//
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

bool CGetExistingItems::GetOne()
  {
  for (;;)
    {
    m_iInArray++;

    if (m_iInArray>=m_nInArray)
      {// Get Next Page
      if (!m_GrfDocPos)
        return false; // no pore pages
      
      bool DoAllInserts=true;
      m_pDoc=dynamic_cast<CGrfDoc*>(m_GrfTemplate.GetNextDoc(m_GrfDocPos));
      m_iPage++;
      m_sPage=m_pDoc->GetTitle();
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
