#include "stdafx.h"
#include "grfdoc.h"
#include "msgwnd.h"

//#include "NetServerU.h"
#include "flwnode.h"

//========================================================================

#if SYSCAD10

//========================================================================
//
//
//
//========================================================================

CNSGrfItem::CNSGrfItem() 
  {
  };

CNSGrfItem::CNSGrfItem(LPCTSTR Page)
  {
  m_Page      = Page;
  };

CNSGrfItem::~CNSGrfItem()
  {
  };

//========================================================================

CNSGrfNode::CNSGrfNode() 
  {
  m_pMdl      = NULL;
  m_Left = 0.0;
  m_Top = 0.0;
  m_Width = 0.0;
  m_Height = 0.0;
  m_Rotation  = 0;
  };

CNSGrfNode::CNSGrfNode(LPCTSTR Page, CNSMdlNode * pMdl, CGrfTagInfo & Info) : \
CNSGrfItem(Page),
m_pMdl(pMdl)
  {
  m_Left = (float)Info.m_LoBnd.m_X;
  m_Top = -(float)Info.m_HiBnd.m_Y;
  m_Width = (float)Info.m_HiBnd.m_X - (float)Info.m_LoBnd.m_X;
  m_Height = (float)Info.m_HiBnd.m_Y - (float)Info.m_LoBnd.m_Y;

  m_Rotation  = (float)Info.m_Node.m_Rotation;
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
CNSGrfItem(Page),
m_pMdl(pMdl)
  {
  //m_IsLink=true;
  }

CNSGrfLink::~CNSGrfLink()
  {
  }

//========================================================================

CNSGrfThing::CNSGrfThing() 
  {
  m_Left = 0.0;
  m_Top = 0.0;
  m_Width = 0.0;
  m_Height = 0.0;
  m_Rotation  = 0;
  };

CNSGrfThing::CNSGrfThing(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR Page, CGrfTagInfo & Info) : \
      m_Tag(Tag),
      m_Guid(Guid),
      CNSGrfItem(Page)
  {
  m_Left = (float)Info.m_LoBnd.m_X;
  m_Top = -(float)Info.m_HiBnd.m_Y;
  m_Width = (float)Info.m_HiBnd.m_X - (float)Info.m_LoBnd.m_X;
  m_Height = (float)Info.m_HiBnd.m_Y - (float)Info.m_LoBnd.m_Y;

  m_Rotation  = (float)Info.m_Node.m_Rotation;
  };

CNSGrfThing::~CNSGrfThing()
  {
  };

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
  CWaitMsgCursor Wait("9->10");
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

//---------------------------------------------------------------------------


  void CNETServerU::SaveItems(CGrfDoc * pDoc)
  {

  CWaitMsgCursor Wait("10->9");

  try
    { 
    POSITION pos = m_Guids.GetHeadPosition();
    for (int i=0; i<m_Guids.GetCount(); i++)
    {
      CNSGuidItem* pGuid = m_Guids.GetAt(pos);
      if (!pGuid->m_IsLink)
      {
        CNSMdlNode* pMdlNode = dynamic_cast<CNSMdlNode *>(pGuid);
        CNSGrfNode* pGrfNode = pMdlNode->m_pGrfs[0];

        if (pDoc)
          {
          //todo check if parms are legal!!!
          CInsertBlk CB; 
          CB.ATag=AdjustTag(pMdlNode->m_Tag);
          CB.AClass="Inserts";//m_pIn->m_sClassID;
          CB.Pt.World.X=pGrfNode->m_Left;
          CB.Pt.World.Y=pGrfNode->m_Top;
          CB.Pt.World.Z=0.0;
          CB.GrpScl.X=pGrfNode->m_Width;
          CB.GrpScl.Y=pGrfNode->m_Height;
          CB.GrpScl.Z=0.0;
          CB.Rotate=pGrfNode->m_Rotation;
          //CB.ATagBase=MInfo.DefTag();

          // Unit Symbols
          CB.ASymbol=FixSymbol(pMdlNode->m_ClassID, pMdlNode->m_ClassID);
          //ImportSymbol(pMdlNode->m_ClassID, false);

          Attr_Settings &rASet=pDoc->GCB.Tag_Attr_Set;
          Attr_Settings ASet=rASet;
          

          if (!Valid(pGrfNode->m_TagLeft))
            {
            pGrfNode->m_TagLeft=CB.Pt.World.X;
            pGrfNode->m_TagTop=CB.Pt.World.Y;
            }

          pDoc->GCB.Tag_InsertPt.X=pGrfNode->m_TagLeft;
          pDoc->GCB.Tag_InsertPt.Y=pGrfNode->m_TagTop;
          pDoc->GCB.Tag_InsertPt.Z=0.0;
          pDoc->GCB.HideTag=false;
          rASet.XScl=pGrfNode->m_TagWidth;
          rASet.Rot=pGrfNode->m_Rotation;

          //ImportSymbol(CB.ASymbol(), false);
          pDoc->GCB.DoInsertNodeGrf(&CB, true);
          rASet=ASet;
          }
      }
      }
    }
  catch(_com_error & e)
    {
    LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query while reading GraphicsLinks\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
    }

  ////(b) links...
  ////SQL = "SELECT * FROM GraphicsLinkLines ORDER BY Page, Tag, SequenceNo";
  //try
  //  { 
  //  SQL.Format("SELECT * FROM [GraphicsLinkLines] WHERE [Group] = '%s' ORDER BY [Tag], [SequenceNo]", m_sGroup);

  //  m_pLl->OpenQuery(NULL, SQL, 0/*dbReadOnly*/);
  //  ADODB::_RecordsetPtr llrs = m_pLl->Recordset();
  //  if (!llrs->adEOF)
  //    llrs->MoveFirst();
  //  Strng PrevTag;

  //  int PrevSeqNo;
  //  Pt_3f Pt;
  //  CLineDrawHelper LDH;
  //  LDH.SetLineMethod(LDM_Direct); //todo user option
  //  LDH.SetArrowRule(ARM_EndDirnAndLast); //todo user option
  //  LDH.SetArrowScale(2.5*(m_Scale.m_X+m_Scale.m_Y+m_Scale.m_Z)/3); //todo user option
  //  LDH.SetShowTag(false); //todo user option
  //  while (!llrs->adEOF)
  //    {
  //    m_pLl->Read();
  //    if (PrevTag.GetLength()>0 && _stricmp(m_pLl->m_sTag, PrevTag())!=0)
  //      {
  //      pDoc->GCB.DoInsertLinkGrf(LDH, NULL, NULL, true);
  //      PrevTag="";
  //      }
  //    if (PrevTag.GetLength()==0)
  //      {
  //      PrevSeqNo = m_pLl->m_lSeqNo;
  //      LDH.InitPoints();
  //      PrevTag = m_pLl->m_sTag;
  //      CString Tg = AdjustTag(m_pLl->m_sTag);
  //      //LDH.SetTag((char*)(LPCSTR)Tg);

  //      CString LSQL;                                                           
  //      LSQL.Format("SELECT * FROM [GraphicsLinks] WHERE [Tag] = '%s' AND [Group] = '%s'", m_pLl->m_sTag, m_sGroup);
  //      m_pLk->OpenQuery(NULL, LSQL, 0/*dbReadOnly*/);
  //      ADODB::_RecordsetPtr llks = m_pLk->Recordset();
  //      if (!llks->adEOF)
  //        llks->MoveFirst();
  //      if (!llks->adEOF)
  //        {
  //        m_pLk->Read();

  //        if (!Valid(m_pLk->m_TagPt.m_X))
  //          {
  //          m_pLk->m_TagPt.m_X=m_pLk->m_Insert.m_X;
  //          m_pLk->m_TagPt.m_Y=m_pLk->m_Insert.m_Y;
  //          m_pLk->m_TagPt.m_Z=m_pLk->m_Insert.m_Z;
  //          }

  //        LDH.SetShowTag(m_pLk->m_TagVisible!=0);
  //        LDH.SetTag(Tg, m_pLk->m_TagPt.m_X, m_pLk->m_TagPt.m_Y, m_pLk->m_TagPt.m_Z);
  //        LDH.SetTagProps(m_pLk->m_TagScale.m_X, m_pLk->m_TagRotation);
  //        }
  //      else
  //        {
  //        LDH.SetShowTag(FALSE);
  //        LDH.SetTag(Tg);
  //        }
  //      }

  //    Pt.Set(m_pLl->m_Vertex.m_X*m_Scale.m_X+m_Move.m_X, 
  //           m_pLl->m_Vertex.m_Y*m_Scale.m_Y+m_Move.m_Y, 
  //           m_pLl->m_Vertex.m_Z*m_Scale.m_Z+m_Move.m_Z);
  //    LDH.AddPoint(Pt, m_pLl->m_lSeqNo-PrevSeqNo>1 ? LPT_Break : LPT_Line);
  //    PrevSeqNo = m_pLl->m_lSeqNo;
  //    llrs->MoveNext();
  //    }
  //  pDoc->GCB.DoInsertLinkGrf(LDH, NULL, NULL, true);

  //  Close();
  //  }
  //catch(_com_error & e)
  //  {
  //  LogError("GraphicslNDB", LF_Exclamation, "Cannot open and read query while reading GraphicsLinks\n%ld:%s : %s", e.Error(), (LPCTSTR)e.Source(), (LPCTSTR)e.Description());
  //  Close();
  //  return 0;
  //  }
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

#endif

//========================================================================
