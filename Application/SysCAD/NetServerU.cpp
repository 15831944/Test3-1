#include "stdafx.h"
#include "grfdoc.h"
#include "msgwnd.h"

#include "NetServerU.h"
#include "flwnode.h"

//========================================================================
//
//
//
//========================================================================

CNSGrfItem::CNSGrfItem() 
  {
  m_InsertX   = 0;
  m_InsertY   = 0;
  m_ScaleX    = 1;
  m_ScaleY    = 1;
  m_Rotation  = 0;
  };

CNSGrfItem::CNSGrfItem(LPCTSTR Page, CGrfTagInfo & Info)
  {
  m_Page      = Page;
  m_InsertX   = (float)Info.m_Node.m_X;
  m_InsertY   = (float)Info.m_Node.m_Y;
  m_ScaleX    = (float)Info.m_Node.m_XScale;
  m_ScaleY    = (float)Info.m_Node.m_YScale;
  m_Rotation  = (float)Info.m_Node.m_Rotation;
  };

CNSGrfItem::~CNSGrfItem()
  {
  };

//========================================================================

CNSGrfNode::CNSGrfNode() 
  {
  m_pMdl      = NULL;
  };

CNSGrfNode::CNSGrfNode(LPCTSTR Page, CNSMdlNode * pMdl, CGrfTagInfo & Info) : \
CNSGrfItem(Page, Info),
m_pMdl(pMdl)
  {
  };

CNSGrfNode::~CNSGrfNode()
  {
  };

//========================================================================

CNSGrfLink::CNSGrfLink() 
  {
  m_pMdl=NULL;
  };

CNSGrfLink::CNSGrfLink(LPCTSTR Page, CNSMdlLink * pMdl, CGrfTagInfo & Info, CLinePointsArray & LPA) : \
CNSGrfItem(Page, Info),
m_pMdl(pMdl)
  {
  //m_IsLink=true;
  }

CNSGrfLink::~CNSGrfLink()
  {
  }

//========================================================================
//
//
//
//========================================================================

CNSMdlNode::CNSMdlNode(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID) : \
CNSGuidItem(Tag, Guid, ClassID)
  { 
  //m_IsMdl   = true;
  m_pNd   = NULL;
  //m_Guid  = "{00000000-0000-0000-0000-000000000000}";
  //m_pGrfs   = NULL;
  };

CNSMdlNode::~CNSMdlNode()   {};


CNSMdlLink::CNSMdlLink(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID, LPCTSTR SrcGuid, LPCTSTR DstGuid, LPCTSTR SrcPort, LPCTSTR DstPort) : \
CNSGuidItem(Tag, Guid, ClassID)
  { 
  m_IsLink  = true; 
  m_pGrf    = NULL;
  m_SrcGuid = SrcGuid;
  m_DstGuid = DstGuid;
  m_SrcPort = SrcPort;
  m_DstPort = DstPort;
  };

CNSMdlLink::~CNSMdlLink() { };

//========================================================================
//
//
//
//========================================================================

CNETServerU::CNETServerU()    
  {
  };

CNETServerU::~CNETServerU()
  {
  };

void CNETServerU::Add(CNSGuidItem * pGuid)
  {
  m_Guids.AddTail(pGuid);
  };

void CNETServerU::LoadItems()
  {
  m_PrjName = PrjName(); //gs_pPrj->

  CDocTemplate & Template = ScdApp()->GraphTemplate();
  POSITION Pos = Template.GetFirstDocPosition();
  while (Pos)
    {
    CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

    CGrfTagInfoArray GTIA;

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
              pNode->IOArea_Rmt(0).pName, pNode->IOArea_Rmt(1).pName);
            CNSGrfLink * pGrf = new CNSGrfLink(Page(), pMdl, I, LPA);
            for (int i=0; i<LPA.GetSize(); i++)
              pGrf->m_Pts.Add(CNSGrfLink::CPt(LPA[i].x, LPA[i].y));
            pMdl->m_pGrf = pGrf;
            m_Guids.AddTail(pMdl);
            }
          else if (TagTyp==1)// || TagTyp==0)
            {
            DoneOne = true;
            CNSMdlNode * pMdl=new CNSMdlNode(I.m_sTag(), Guid(), I.m_sClass());
            CNSGrfNode * pGrf=new CNSGrfNode(Page(), pMdl, I);
            pMdl->m_pGrfs.Add(pGrf);
            m_Guids.AddTail(pMdl);
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

void CNETServerU::UpdateItems()
  {
  };

void CNETServerU::Startup()
  {

  LoadItems();
  //CList <CNSGrfNode*, CNSGrfNode*> m_Items;
  //CMap  <CNSGrfNode*, CNSGrfNode*, LPCTSTR*, LPCTSTR*> m_TagMap;
  //CMap  <CNSGrfNode*, CNSGrfNode*, LPCTSTR*, LPCTSTR*> m_GuidMap;
  //LogNote("CNETServerThread", 0, "Startup");

  //m_Config = gcnew SysCAD::Interface::ConfigData;
  //m_StencilPath = gcnew String(BaseCfgFiles());
  //m_StencilPath = m_StencilPath + "..\\Stencils\\";

  //Console::WriteLine("Startup");
  //CreateConnects();
  //GetStencils();
  //MarshalGraphics();
  //MarshalConfig();
  };

void CNETServerU::Shutdown()
  {
  //delete m_Config;

  //LogNote("CNETServerThread", 0, "Shutdown");
  };

//========================================================================
//
//
//
//========================================================================
