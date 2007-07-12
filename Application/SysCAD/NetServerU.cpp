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


static bool AddUnitGrf(bool DoingImport, LPCSTR Tag, double X, double Y, double Z, LPCSTR Symbol, LPCSTR ClassId, double XScale, double YScale, double Rotation)
  {
  try
    {
    bool Error = false;
    if (!DoingImport)
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
      pDoc=GetGrfDoc(-1, NULL);//m_sName);
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

      if (DoingImport)
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


void SS_CreateItem(bool DoingImport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                   LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect,
                   double angle, COLORREF fillColor, 
                   bool mirrorX, bool mirrorY)
  {
  

  //296 = hgt of a3 sheat
  if (!AddUnitGrf(DoingImport, tag, boundingRect.MidX(), 296.0-boundingRect.MidY(), 0.0, shape, model, mirrorX ? -1.0:1.0/*XScale*/, mirrorY ? -1.0:1.0/*YScale*/, angle))
    {
    }
  

  int xxx=0;
  }

//bool SS_GetExistingItem(FlwNode * & NdPtr, CString & tag, CString & page, 
//                   CString & model, CString & shape, CRectangleF & boundingRect,
//                   //Model^ model, Shape^ shape, RectangleF boundingRect, 
//                   double & angle, bool & mirrorX, bool & mirrorY)
//  {
//  if (NdPtr==NULL)
//    {
//    // Skip the FlwSolver
//    //gs_pSfeSrvr->Get
//    }
//
//  ////296 = hgt of a3 sheat
//  //if (!AddUnitGrf(tag, boundingRect.MidX(), 296.0-boundingRect.MidY(), 0.0, shape, model, mirrorX ? -1.0:1.0/*XScale*/, mirrorY ? -1.0:1.0/*YScale*/, angle))
//  //  {
//  //  }
//  //
//
//  return false;
//  }

CGetExistingItems::CGetExistingItems() : \
m_GrfTemplate(ScdApp()->GraphTemplate())
  {
  m_GrfDocPos = m_GrfTemplate.GetFirstDocPosition();
  m_nInArray  = 0;
  m_iInArray  = 0;
  m_pDoc      = NULL;
  }

bool CGetExistingItems::GetOne()
  {

  m_iInArray++;
  if (m_iInArray>=m_nInArray)
    {// Get Next Page
    if (!m_GrfDocPos)
      return false;
    
    bool DoAllInserts=true;
    m_pDoc=dynamic_cast<CGrfDoc*>(m_GrfTemplate.GetNextDoc(m_GrfDocPos));
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
          DoneOne = true;
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
          break;       
          }
        case eIsNode:
          {
          Strng tag = I.m_sTag();

          if (tag.Find("FLOWSHEET_")) // _NOT_ a FLOWSHEET_* non-unit -- There must be a better way to find/handle these?
            {
            DoneOne = true;
            //CNSMdlNode * pMdl=new CNSMdlNode(I.m_sTag(), Guid(), I.m_sClass());
            //CNSGrfNode * pGrf=new CNSGrfNode(Page(), pMdl, I);
            //pMdl->m_pGrfs.Add(pGrf);
            //m_Guids.AddTail(pMdl);
            }
          else
            {
            // This creates a dummy 'tank-1' thing annotation.
            DoneOne = true;
            //CNSGrfThing * pGrf=new CNSGrfThing(I.m_sTag(), Guid(), Page(), I);
            //m_Things.AddTail(pGrf);
            }
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

    return DoneOne;
    }

  return false;
  }


/*
 while (Pos)
    {
    CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));


    //if (OpenSymbolExport())
    //  pDoc->GCB.pDrw->SaveSymbols(m_SymbolExportFolder.GetBuffer(), pDoc->m_sDocGUID());

    bool DoAllInserts=true;
    const int Cnt = pDoc->GetTagListInfo(DoAllInserts, GTIA);
    if (Cnt>0)
      {
      long InsertCnt=0;
      bool DoneOne = false;
      Strng Page = pDoc->GetTitle();
      Strng Guid;
      if (Page.Len()>4)
        Page = Page.Left(Page.Len()-4);//remove .scg
      POSITION pos = pDoc->GetFirstViewPosition();
      CView* pFirstView = pDoc->GetNextView( pos );
      CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
      for (int i=0; i<Cnt; i++)
        {
        CGrfTagInfo & I = GTIA[i];
        //if (I.m_bHasTag && FindOldTag(I.m_sTag())<0)
        //  continue;

        int TagTyp = -1; //error
        if (I.m_bHasTag)
          {
          FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag());
          if (pNode)
            {
            TagObjClass * pTagObjC = pNode->Class();
            I.m_sClass = pNode->ClassId();
            Guid = pNode->EqpGUID();


          //CXM_ObjectTag ObjTag(I.m_sTag(), TABOpt_Parms);
          //CXM_ObjectData ObjData;
          //CXM_Route Route;
          //TagObjClass * pTagObjC = NULL;
          //if (pGWnd->XReadTaggedItem(ObjTag, ObjData, Route))
          //  {
          //  I.m_sClass = ObjData.FirstItem()->Class();
          //  pTagObjC = TagObjClass::FindClassId(I.m_sClass());
          //  if (pTagObjC==NULL)
          //    pTagObjC = TagObjClass::FindClassName(I.m_sClass());
            if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 || 
              _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 || 
              _stricmp(ElecLinkGrp, pTagObjC->Group())==0 || 
              _stricmp(AirLinkGrp, pTagObjC->Group())==0))
              TagTyp = 2; //link
            else
              TagTyp = 1; //unit/node
            if (pTagObjC)
              {
              I.m_sDrwGroup=pTagObjC->DrwGroup();
              }

            //Strng GuidTag(I.m_sTag());
            //GuidTag+=".EqpGUID";
            //CXM_ObjectTag GuidObjTag(GuidTag(), TABOpt_Parms);
            //CXM_ObjectData GuidObjData;
            //CXM_Route GuidRoute;
            //if (pGWnd->XReadTaggedItem(GuidObjTag, GuidObjData, GuidRoute))
            //  {
            //  Guid = GuidObjData.FirstItem()->Value()->GetString();
            //  }  
            }
          else
            {
            TagTyp=3;
            I.m_sClass = "";
            }

          if (TagTyp==2)
            {
            DoneOne = true;
            CLinePointsArray LPA;
            pDoc->GCB.pDrw->CollectLinkInfo(I, LPA);

            CNSMdlLink * pMdl = new CNSMdlLink(I.m_sTag(), Guid(), I.m_sClass(),
              pNode->Nd_Rmt(0)->EqpGUID(), pNode->Nd_Rmt(1)->EqpGUID(),
              pNode->IOArea_Rmt(0).m_pName, pNode->IOArea_Rmt(1).m_pName);
            CNSGrfLink * pGrf = new CNSGrfLink(Page(), pMdl, I, LPA);
            for (int i=0; i<LPA.GetSize(); i++)
              pGrf->m_Pts.Add(CNSGrfLink::CPt(LPA[i].x, LPA[i].y));
            pMdl->m_pGrf = pGrf;
            m_Guids.AddTail(pMdl);
            }
          else if (TagTyp==1)// || TagTyp==0)
            {
              Strng tag = I.m_sTag();

              if (tag.Find("FLOWSHEET_")) // _NOT_ a FLOWSHEET_* non-unit -- There must be a better way to find/handle these?
              {
                DoneOne = true;
                CNSMdlNode * pMdl=new CNSMdlNode(I.m_sTag(), Guid(), I.m_sClass());
                CNSGrfNode * pGrf=new CNSGrfNode(Page(), pMdl, I);
                pMdl->m_pGrfs.Add(pGrf);
                m_Guids.AddTail(pMdl);
              }
              else
              {
                // This creates a dummy 'tank-1' thing annotation.
                //DoneOne = true;
                //CNSGrfThing * pGrf=new CNSGrfThing(I.m_sTag(), Guid(), Page(), I);
                //m_Things.AddTail(pGrf);
              }
            }
          }
        //if (TagTyp==3)  
        //  {
        //  I.m_sTag.Set("$%i$", ++InsertCnt);
        //  I.m_sClass="Insert";
        //  I.m_sDrwGroup="Inserts";

        //  DoneOne=true;
        //  WriteInsert(m_sGroup, I, m_Origin, Page(), pDoc->m_sDocGUID());
        //  ExportSymbol(I, m_Origin, pDoc->m_sDocGUID());
        //  }
        }
      //if (m_bWithGroups && DoneOne)
      //  WriteGroupInfo(m_sGroup, m_sGroupDesc, m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
      }

    //CloseSymbolExport();


    //  RemotingServices::Marshal(graphic, filename);
    //  m_Config->projectList->Add(filename);
    //  LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
    }
  };


  static CString FixSymbol(LPCTSTR m_sSymbol, LPCTSTR m_sClassID)
  {
  // Unit Symbols
  CString Sym=m_sSymbol;
  Sym=Sym.Trim();
  if (Sym=="" || Sym.CompareNoCase("default")==0)
    Sym=m_sClassID;
  if ((Sym.GetLength()>0) && (Sym.Find(':')<0) && (strlen(m_sClassID)>0))
    {
    TagObjClass * pTagObjC = TagObjClass::FindClassId((LPTSTR)m_sClassID);
    if (pTagObjC==NULL)
      pTagObjC = TagObjClass::FindClassName((LPTSTR)m_sClassID);
    if (pTagObjC)
      {
      CString S(pTagObjC->DrwGroup());
      S+=":";
      Sym=S+Sym;
      }
    }
  return Sym;
  }



*/





//========================================================================
//
//
//
//========================================================================

#endif

//========================================================================
