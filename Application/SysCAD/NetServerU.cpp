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


static bool AddUnitGrf(LPCSTR Tag, double X, double Y, double Z, LPCSTR Symbol, LPCSTR ClassId, double XScale, double YScale, double Rotation)
  {
  try
    {
    bool DataErr = (CheckTagExists(Tag)==0);
    if (DataErr)
      {
      LogError(NETSERVERNAME, 0, "Tag not found : AddUnit '%s' failed!", Tag);
      // return Scd.Return(eScdGraphicCode_TagNotFound, "Tag not found : AddUnit '%s' failed!", Tag);
      }
    CDocument* pDoc=NULL;
    if (!DataErr)
      {
      pDoc=GetGrfDoc(-1, NULL);//m_sName);
      DataErr = (pDoc==NULL);
      }

    if (!DataErr)
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

      pGDoc->GCB.DoInsertNode(&CB);
      }

    if (DataErr)
      {
      LogError(NETSERVERNAME, 0, "AddUnit '%s' failed!", Tag);
      //return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
      }
    return !DataErr;
    }
  catch(...)
    {
    LogError(NETSERVERNAME, 0, "Exception while Adding Unit '%s'", Tag);
    }
  return false;  
  }



void SS_CreateItem(__int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                   LPCTSTR model, LPCTSTR shape, const CRectangleF & /*RectangleF */boundingRect,
                   //Model^ model, Shape^ shape, RectangleF boundingRect, 
                   double angle, COLORREF /*System::Drawing::Color*/ fillColor, 
                   bool mirrorX, bool mirrorY)
  {
  

  if (!AddUnitGrf(tag, boundingRect.MidX(), boundingRect.MidY(), 0.0, shape, model, mirrorX ? -1.0:1.0/*XScale*/, mirrorY ? -1.0:1.0/*YScale*/, angle))
    {
    }
  

  int xxx=0;
  }

//========================================================================
//
//
//
//========================================================================

#endif

//========================================================================
