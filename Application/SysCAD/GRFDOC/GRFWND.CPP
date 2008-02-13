//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define  __GRFWND_CPP
#include "sc_defs.h"
#include "syscad.h"
#include "grfdoc.h"
#include "grfwnd.h"
#include "scd_wm.h"
#include "mainfrm.h"
#include "project.h"
#include "wndslct.h"
#include "explorescd.h"
#include "svcconn.h"
#include "multiprintselect.h"

//#include "optoff.h"

//===========================================================================

CGrfSubs::CGrfSubs(char* pTag)
  {
  sTag = pTag;
  pDS = NULL;
  }

//===========================================================================

CGrfWnd* pTheLastGrfWnd = NULL;

IMPLEMENT_DYNCREATE(CGrfWnd, CView)

BEGIN_MESSAGE_MAP(CGrfWnd, CView)
  //{{AFX_MSG_MAP(CGrfWnd)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_SIZE()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDBLCLK()
  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_MULTIPLE, OnFilePrintMultiple)
  ON_WM_SETFOCUS()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DOGRFCMD, OnDoGrfCmd)
  ON_MESSAGE(WMU_DOSETFOCUS, OnDoSetFocus)
  ON_MESSAGE(WMU_GRFWNDDSPUPDATE, OnDoDspUpdate)
  ON_MESSAGE(WMU_GRFWNDWRITESUBS, OnDoWriteSubs)
  //  ON_COMMAND_RANGE(IDM_SRCIO_0, IDM_UNITIO_MAX, OnUnitIOMenu)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CGrfWnd::CGrfWnd() :
Dsp(this)
  {
  pDoc=NULL;
  EO_Register(pExecName_GrfWnd, EODisplayUpdate|EOWrite_Msg, /*Pri*/0,/*Stack*/0);
  BaseCursor=NULL;
  OptionCursor=NULL;
  BaseCursor=ScdApp()->LoadCursor(IDC_GRAPHICS1);
  bDonePaint=0;
  bPretendPrinting=0;
  PrevNumStatusColourChgs=-1;
  }

//---------------------------------------------------------------------------

CGrfWnd::~CGrfWnd()
  {
  EO_DeRegister();
  }

//---------------------------------------------------------------------------

BOOL CGrfWnd::PreCreateWindow(CREATESTRUCT& cs)
  {
  if (GrfHelper.pBrush==NULL)
    GrfHelper.pBrush = new CBrush(GrfHelper.BackGroundRGB);
  /*
  UINT Style=WS_CHILD | WS_VISIBLE | // WS_CAPTION |
  WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
  WS_BORDER | WS_THICKFRAME;
  cs.lpszClass = AfxRegisterWndClass(Style,
  //ScdApp()->LoadStandardCursor(IDC_CROSS),
  0,//ScdApp()->LoadCursor(IDC_GRAPHICS1), //DYNAMICCROSS),
  (HBRUSH)GetStockObject(BLACK_BRUSH), 0);*/

  const UINT Style = CS_DBLCLKS;
  cs.lpszClass = AfxRegisterWndClass(Style, 0, (HBRUSH)(*GrfHelper.pBrush), 0);
  return CView::PreCreateWindow(cs);
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnInitialUpdate()
  {
  CString Title = GetDocument()->GetTitle();
  gs_pPrj->RestoreOneWindow((char*)(const char*)Title, this, false);
  CView::OnInitialUpdate();
  }

//---------------------------------------------------------------------------

BOOL CGrfWnd::Terminate()
  {
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CGrfWnd::Open(pchar FlwSheetName_)
  {
  Invalidate();
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CGrfWnd::Close()
  {
  Invalidate();
  return TRUE;
  }

//---------------------------------------------------------------------------

void CGrfWnd::SendCmdToAll(char* pCmdStr)
  {
  gs_pCmd->ProcessAStr("\x1b"); //Ensure there are no half complete commands
  POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
  while (Pos)
    {
    CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
    pGrfDoc->OnActivate(True);  // direct cmds to this document
    gs_pCmd->ProcessAStr(pCmdStr);
    }
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
  {
  pDoc = dynamic_cast<CGrfDoc*>(GetDocument());
  if (pDoc)
    ((CGrfDoc*)pDoc)->OnActivate(bActivate);

  //  CWndSlctWnd::RefreshIt();
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
  {
  if (lHint==0)
    Invalidate();
  }

//---------------------------------------------------------------------------

LRESULT CGrfWnd::OnDoSetFocus(WPARAM wParam, LPARAM lParam)
  {
  if (lParam)
    {
    CWnd* pWnd = (CWnd*)lParam;
    pWnd = pWnd->GetParent();
    pWnd->BringWindowToTop();
    //pWnd->SetFocus();
    }
  return True;
  }

//---------------------------------------------------------------------------

//void CGrfWnd::OnUnitIOMenu(UINT id)
//  {
//  if (id>=IDM_DSTIO_0)
//    Dsp.iDstIOIndex=id-IDM_DSTIO_0;
//  else
//    Dsp.iSrcIOIndex=id-IDM_SRCIO_0;
////  Dsp.iDstIOIndex=id-IDM_UNITIO_0;
//  };

//---------------------------------------------------------------------------

#define NewGrfUpdate 10

bool CGrfWnd::UpdateForPaint(bool ForceUpd, bool ColoursChgd)
  {
#if NewGrfUpdate 
  //CStopWatch SW;
  //SW.Start();
  //dbgpln("OnDraw   A  %10.3fms", SW.Lap()*1e3); 
  flag DoIt=false;
  if (gs_pTheSFELib->TestLoadModelStatus()>0)
    {
    gs_pTheSFELib->SetLoadAllModelStatus();
    DoIt=true;
    }

  if (gs_pTheSFELib->TestLoadModelAssocGrfs())
    {
    gs_pTheSFELib->SetLoadAllModelAssocGrfs();
    DoIt=true;
    }

  //dword NumStatusColourChgs=gs_pTheSFELib->GetNumStatusColourChgs();
  if (ForceUpd || ColoursChgd || DoIt /*|| NumStatusColourChgs!=PrevNumStatusColourChgs*/)
    {
    //PrevNumStatusColourChgs=NumStatusColourChgs;

    return true;
    }
  return false;
#else
  return true;
#endif
  };

void CGrfWnd::OnDraw(CDC* pDC)
  {
  Dsp.Open(pDC);

  flag HasUpdate=GetUpdateRect(NULL, FALSE);

  if (pDC->IsPrinting() || bPretendPrinting)
    {
    if (bPretendPrinting)
      pDC->FillSolidRect(0,0,9999,9999,RGB(255,255,255));
    //heading...
    //TODO put some code to select a better font
    //pOldFont = pDC->SelectObject(gs_pAccWnd->ScrGB.pFont[0]);
    CString Name = GetDocument()->GetTitle();
    Strng Title,DtTm;
    Title.Set(" %s    [SysCAD  %s]", (const char*)Name, CurDateTime(DtTm));//, FullCopyrightNotice());
    pDC->TextOut(0, 0, Title(), Title.Length());
    }

  CRect ClpBox;
  int i=::GetClipBox(Dsp.HDC_List[Dsp.Opens], &ClpBox);

  if (i!=ERROR && i!=NULLREGION)
    {
    //if (UpdateForPaint(false, false))
    Dsp.Paint(ClpBox);
    }

  Dsp.Close();

  bDonePaint = 1;
  }

//---------------------------------------------------------------------------

void CGrfWnd::DelDynamicSegment(pDynamicSegment pDS)
  {
  CSingleLock Lock(&m_XDataLock, true);

  const int Size = m_XData.GetSize();
  for (int i=0; i<Size; i++)
    {
    if (m_XData[i]->pDS==pDS)
      {
      m_XData[i]->pDS = NULL;
      m_XData[i]->iFindCnt = 0;
      return;
      }
    }
  }

//---------------------------------------------------------------------------

LRESULT CGrfWnd::OnDoGrfCmd(WPARAM wParam, LPARAM lParam)
  {
  switch (wParam)
    {
    case ID_GRF_RefreshAnnotation:
      (dynamic_cast<CGrfDoc*>(GetDocument()))->OnActivate(True);  // direct cmds to this document
      gs_pCmd->ProcessAStr("refresh annotation\r");
      break;
    }
  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  switch (Rqst.RQ_Type)
    {
    case RQ_Files:
      if (Info.Count()<2)
        {
        Info.sData = (const char*)(GetDocument()->GetPathName());
        if (Info.Count()==1)
          {
          Info.sData.FnDrivePathName(Info.sData());
          Info.sData += ".dxf";
          }
        // Will always return the current Prj
        Info.sData.FnContract();
        return True;
        }
      break;
    }
  return False;
  }

//---------------------------------------------------------------------------

int CGrfWnd::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CGrfWnd::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  //change tag for annotation blocks...
  if (Dsp.Vp1->Drw->ChangeTag_NewAnnotation_Block(pOldTag, pNewTag))
    {
    PostMessage(WMU_DOGRFCMD, ID_GRF_RefreshAnnotation, 0);
    }
  //change tag for models...
  if (Dsp.ChangeTag(pOldTag, pNewTag))
    {
    Dsp.Open();
    GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;
    GCB.pDrw->EntityInvalidate();
    Invalidate(True);
    Dsp.Close();
    }

  return EOCT_DONE;
  }

//---------------------------------------------------------------------------

int CGrfWnd::EO_ChangeTagDone(pchar pOldTag, pchar pNewTag)
  {
  ////change tag for annotation blocks...
  //if (Dsp.Vp1->Drw->ChangeTag_NewAnnotation_Block(pOldTag, pNewTag))
  //  {
  //  PostMessage(WMU_DOGRFCMD, ID_GRF_RefreshAnnotation, 0);
  //  }
  ////change tag for models...
  //if (Dsp.ChangeTag(pOldTag, pNewTag))
  //  {
  //  Dsp.Open();
  //  GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;
  //  GCB.pDrw->EntityInvalidate();
  //  Invalidate(True);
  //  Dsp.Close();
  //  }

  return EOCT_DONE;
  }

//---------------------------------------------------------------------------

int CGrfWnd::EO_DeleteTags(Strng_List & Tags) 
  {//a tag has been deleted, ...
  int RetCode = EODT_NOTFOUND;
#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    for (Strng * pDelTag=Tags.First(); pDelTag; pDelTag=Tags.Next())
      {
      DXF_ENTITY eInsert, eAttrib;
      if (Dsp.FindTag(pDelTag->Str(), eInsert, eAttrib))
        {
        Dsp.Open();

        if (eInsert)
          {
          GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;

          Dsp.Draw(eInsert, GrfHelper.GR_BACKGROUND);
          GCB.pDrw->Delete(eInsert);
          }

        Dsp.Close();
        }
      }
    }
#endif
  return RetCode;
  }

//---------------------------------------------------------------------------

int CGrfWnd::EO_SCInsertNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                            LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                            float angle, const CSvcTagBlk & TagBlk, COLORREF Colour, 
                            bool mirrorX, bool mirrorY) 
  {
#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    CRectangleF PageRct(0.0, 420, 0, 297);
    CString PageName=CSvcConnect::ExtractPageName(path);

    if (PageName.CompareNoCase(pDoc->GetTitle())!=0)
      return EOSC_NOTFOUND;

    //CGrfDoc * pDoc=CSvcCOnnect::FindGrfDoc(PageName);
    PageRct = PrjSvc().GetPageRect(PageName);

    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    CInsertBlk CB; 

    CB.ATag=pTag;
    CB.AClass=model;//();
    CB.Pt.World.X=boundingRect.MidX()-PageRct.Left(); // not inversion
    CB.Pt.World.Y=PageRct.Top()-boundingRect.MidY();  // this must be inverted
    CB.Pt.World.Z=0;
    CB.NdScl.X=mirrorX ? -1:1;//XScale;
    CB.NdScl.Y=mirrorY ? -1:1;
    CB.NdScl.Z=1;
    CB.Rotate=(float)angle;
    CB.ATagBase="XX_";//MInfo.TagInitialID();
    CB.ASymbol=shape;
    CB.m_sGuid=ItemGuid;
    CB.m_Rect=boundingRect;
    CB.m_TagPt.X=TagBlk.m_Area.MidX();
    CB.m_TagPt.Y=PageRct.Top()-TagBlk.m_Area.Top();
    CB.m_TagVisible=TagBlk.m_Visible;

    //if (m_bExportBusy)
    //  {
    pGDoc->GCB.DoInsertNodeGrf(&CB);

    CB.m_Rect=CRectangleF(); // clear it
    //pGDoc->GCB.DoChangeNodeGuid(CB.ATag(), CB.m_sGuid());
    //  }
    //else
    //  pGDoc->GCB.DoInsertNode(&CB);
    }
#endif

  return EOSC_DONE;
  };

int CGrfWnd::EO_SCDeleteNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR GrfGuid, LPCSTR path)
  {
  int RetCode = EOSC_NOTFOUND;

#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    DXF_ENTITY eInsert, eAttrib;
    if ((pTag && Dsp.FindTag((LPSTR)pTag, eInsert, eAttrib)) ||
        (GrfGuid && Dsp.FindGuid((LPSTR)GrfGuid, eInsert, eAttrib)))
      {
      Dsp.Open();

      if (eInsert)
        {
        GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;
        Dsp.Draw(eInsert, GrfHelper.GR_BACKGROUND);
        GCB.pDrw->Delete(eInsert);
        RetCode = EOSC_DONE;
        }

      Dsp.Close();
      }
    }
#endif

  return RetCode;
  };

int CGrfWnd::EO_SCModifyNodeG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid, LPCSTR path,
                            LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
                            float angle, const CSvcTagBlk & TagBlk, COLORREF Colour, 
                            bool mirrorX, bool mirrorY)     
  { 
#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    CRectangleF PageRct(0.0, 420, 0, 297);
    CString PageName=CSvcConnect::ExtractPageName(path);

    if (PageName.CompareNoCase(pDoc->GetTitle())!=0)
      return EOSC_NOTFOUND;

    DXF_ENTITY eInsert, eAttrib;
    if (Dsp.FindTag((LPSTR)pTag, eInsert, eAttrib))
      {
      Dsp.Open();

      if (eInsert)
        {
        GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;

        PageRct = PrjSvc().GetPageRect(PageName);

        C3_BOX_S CurBounds;

        if (1) // change the shape
          {
          CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
          GrfCmdBlk& GCB = pGDoc->GCB;
          Dsp.Draw(eInsert, GrfHelper.GR_BACKGROUND);
          GCB.pDrw->Delete(eInsert);

          CInsertBlk CB; 

          CB.ATag=pTag;
          CB.AClass=model;//();
          CB.Pt.World.X=boundingRect.MidX()-PageRct.Left(); // not inversion
          CB.Pt.World.Y=PageRct.Top()-boundingRect.MidY();  // this must be inverted
          CB.Pt.World.Z=0;
          CB.NdScl.X=mirrorX ? -1:1;//XScale;
          CB.NdScl.Y=mirrorY ? -1:1;
          CB.NdScl.Z=1;
          CB.Rotate=(float)angle;
          CB.ATagBase="XX_";//MInfo.TagInitialID();
          CB.ASymbol=shape;
          CB.m_sGuid=ItemGuid;
          CB.m_TagPt.X=TagBlk.m_Area.MidX();
          CB.m_TagPt.Y=PageRct.Top()-TagBlk.m_Area.Top();
          CB.m_TagVisible=TagBlk.m_Visible;
          //CB.m_Rect=boundingRect;
          //if (m_bExportBusy)
          //  {

          // A Kludge - get around a bug ????????????????????????????????????????
          int iDash = CB.ASymbol.Find('-');
          if (iDash>0)
            CB.ASymbol=CB.ASymbol.Left(iDash);
          // ????????????????????????????????????????????????????????????????????

          if (pGDoc->GCB.DoInsertNodeGrf(&CB))
            eInsert=CB.e;

          //CB.m_Rect=CRectangleF(); // clear it

          }
        //else
        //  {
        DXF_INSERT_ROT_ANG(eInsert) = 0; // do this to allow GetBounds to get the correct size

        DML_LIST CurveLst=dml_create_list();
        DML_LIST InsertLst=dml_create_list();
        DML_LIST OtherLst=dml_create_list();
        dml_append_data(InsertLst,eInsert);
        GCB.pDrw->GetBounds(&CurBounds, CurveLst, InsertLst, OtherLst);  
        dml_clear_list(InsertLst);
        dml_destroy_list(CurveLst, dmm_free);
        dml_destroy_list(InsertLst, dmm_free);
        dml_destroy_list(OtherLst, dmm_free);

        DXF_INSERT_PT(eInsert)[0]   = boundingRect.MidX()-PageRct.Left();
        DXF_INSERT_PT(eInsert)[1]   = PageRct.Top()-boundingRect.MidY();
        DXF_INSERT_PT(eInsert)[2]   = 0;
        DXF_INSERT_X_SCALE(eInsert)  = DXF_INSERT_X_SCALE(eInsert)  * boundingRect.Width()/Max(1.0, (C3_MAX_X(&CurBounds)-C3_MIN_X(&CurBounds)));
        DXF_INSERT_Y_SCALE(eInsert)  = DXF_INSERT_Y_SCALE(eInsert)  * boundingRect.Height()/Max(1.0, (C3_MAX_Y(&CurBounds)-C3_MIN_Y(&CurBounds)));

        DXF_INSERT_ROT_ANG(eInsert) = -angle;
        //  }

        GCB.pDrw->EntityInvalidate(eInsert);
        Invalidate(true);

        }

      Dsp.Close();
      return EOSC_DONE; 
      }
    }
#endif
  return EOSC_NOTFOUND; 
  };

//---------------------------------------------------------------------------

int CGrfWnd::EO_SCInsertLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR GraphicGuid,
                               LPCSTR model, 
                               const CSvcLnkGBlk & LnkGBlk,
                               CPointFList & ControlPoints, 
                               const CSvcTagBlk & TagBlk)
  {
#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    CRectangleF PageRct(0.0, 420, 0, 297);
    CString PageName=pDoc->GetTitle(); // Assume that this is THE page required
    PageRct = PrjSvc().GetPageRect(PageName);

    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;


    //Strng SrcTag, DstTag;
    //flag SrcLnk, DstLnk;

    //if(!gs_pSfeSrvr->FE_FindNodeInfoFromGuid((LPSTR)OriginGuid, SrcTag, SrcLnk) ||
    //   !gs_pSfeSrvr->FE_FindNodeInfoFromGuid((LPSTR)DestinationGuid, DstTag, DstLnk))
    //  {
    //  return EOSC_DONE;
    //  }

    DXF_ENTITY eInsert, eAttrib;
    if (!Dsp.FindGuid((LPSTR)LnkGBlk.m_OriginGuid, eInsert, eAttrib) && !Dsp.FindGuid((LPSTR)LnkGBlk.m_DestinationGuid, eInsert, eAttrib))
      {
      return EOSC_DONE;
      }

    CLineDrawHelper LDH;
    LDH.SetGuid(GraphicGuid);
    LDH.SetLineMethod(LDM_Direct); //todo user option
    LDH.SetArrowRule(ARM_EndDirnAndLast); //todo user option
    LDH.SetArrowScale(2.5); //todo user option
    LDH.SetShowTag(TagBlk.m_Visible); //todo user option

    LDH.SetTag(pTag, TagBlk.m_Area.MidX(), PageRct.Top()-TagBlk.m_Area.Top(), 0.0);
    LDH.SetTagProps(1.0, 0.0);//m_pLk->m_TagScale.m_X, m_pLk->m_TagRotation);
    LDH.InitPoints();

    CRectangleF boundingRect;
    POSITION Pos=ControlPoints.GetHeadPosition();
    while (Pos)
      {
      CPointF &pt=ControlPoints.GetNext(Pos);
      Pt_3f Pt3(pt.X()-PageRct.Left(), PageRct.Top()-pt.Y(), 0.0);

      LDH.AddPoint(Pt3, LPT_Line);
      }

    if (pGDoc!=NULL)
      {
      //todo check if parms are legal!!!
      pGDoc->GCB.DoInsertLinkGrf(LDH, (LPSTR)LnkGBlk.m_OriginTag, (LPSTR)LnkGBlk.m_DestinationTag, true);
      }
    }
#endif
  return EOSC_DONE;
  };

int CGrfWnd::EO_SCDeleteLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR GrfGuid )
  {
  int RetCode = EOSC_NOTFOUND;

#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    DXF_ENTITY eInsert, eAttrib;
    if ((pTag && Dsp.FindTag((LPSTR)pTag, eInsert, eAttrib)) ||
        (GrfGuid && Dsp.FindGuid((LPSTR)GrfGuid, eInsert, eAttrib)))
      {
      Dsp.Open();

      if (eInsert)
        {
        GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;
        Dsp.Draw(eInsert, GrfHelper.GR_BACKGROUND);
        GCB.pDrw->Delete(eInsert);
        RetCode = EOSC_DONE;
        }

      Dsp.Close();
      }
    }
#endif

  return RetCode;
  };

int CGrfWnd::EO_SCModifyLinkG(CSvcExecCtrl & Ctrl, LPCSTR pTag, LPCSTR ItemGuid,
                               LPCSTR model, 
                               const CSvcGuidPair & Guids, 
                               CPointFList & ControlPoints, const CSvcTagBlk & TagBlk)
  {
#if SYSCAD10         
  if (gs_pPrj->SvcActive)
    {
    CRectangleF PageRct(0.0, 420, 0, 297);
    CString PageName=pDoc->GetTitle(); // Assume that this is THE page required
    PageRct = PrjSvc().GetPageRect(PageName);

    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;

    //FlwNode * pSrc = gs_pSfeSrvr->FE_FindNode(NULL, OriginGuid);
    //FlwNode * pDst = gs_pSfeSrvr->FE_FindNode(NULL, DestinationGuid);

    DXF_ENTITY eSrc, eAttSrc;
    DXF_ENTITY eDst, eAttDst;
    if (Dsp.FindGuid((LPSTR)Guids.m_OriginGuid, eSrc, eAttSrc) && Dsp.FindGuid((LPSTR)Guids.m_DestinationGuid, eDst, eAttDst))
      {
      LPSTR SrcTag = Find_Attr_Value(eSrc, TagAttribStr);
      LPSTR DstTag = Find_Attr_Value(eDst, TagAttribStr);

      DXF_ENTITY eInsert, eAttrib;
      if (Dsp.FindTag((LPSTR)pTag, eInsert, eAttrib))
        {
        Dsp.Open();

        if (eInsert)
          {
          GrfCmdBlk& GCB = ((CGrfDoc*)pDoc)->GCB;

          LPSTR BlockName = DXF_INSERT_BLOCK_GET(eInsert);

          Dsp.Draw(eInsert, GrfHelper.GR_BACKGROUND);
          GCB.pDrw->Delete(eInsert);

          CLineDrawHelper LDH;
          LDH.SetGuid(ItemGuid);
          LDH.SetLineMethod(LDM_Direct); //todo user option
          LDH.SetArrowRule(ARM_EndDirnAndLast); //todo user option
          LDH.SetArrowScale(2.5); //todo user option
          //LDH.SetShowTag(false); //todo user option

          LDH.SetTag(pTag, TagBlk.m_Area.MidX(), PageRct.Top()-TagBlk.m_Area.Top(), 0.0);
          LDH.SetShowTag(TagBlk.m_Visible);

          LDH.SetTagProps(1.0, 0.0);//m_pLk->m_TagScale.m_X, m_pLk->m_TagRotation);
          LDH.InitPoints();

          CRectangleF boundingRect;
          POSITION Pos=ControlPoints.GetHeadPosition();
          while (Pos)
            {
            CPointF &pt=ControlPoints.GetNext(Pos);
            Pt_3f Pt3(pt.X()-PageRct.Left(), PageRct.Top()-pt.Y(), 0.0);

            LDH.AddPoint(Pt3, LPT_Line);
            }

          if (pGDoc!=NULL)
            {
            //todo check if parms are legal!!!
            pGDoc->GCB.DoInsertLinkGrf(LDH, SrcTag, DstTag, true);
            }
          }

        Dsp.Close();
        return EOSC_DONE; 
        }
      }
    }
#endif
  return EOSC_NOTFOUND; 
  };

//---------------------------------------------------------------------------

flag CGrfWnd::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Start(CXM_TimeControl &CB)
  {
  return True;
  }

//---------------------------------------------------------------------------

void CGrfWnd::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  if (gs_pPrj->m_bGrfAnimationOn)
    {
    CXM_ReadIndexedData * pMsg = (CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
    flag ReadAll = pMsg->ReadAll;
    long DataIndex = pMsg->Start ? 0 : pMsg->LastIndex+1;
    flag IsStart = pMsg->Start;
    XM.Clear();

    CSingleLock Lock(&m_XDataLock, true);

    DynamicSegment* pDS = Dsp.Vp1->dynm_list;
    if (IsStart)
      {
      while (pDS)
        {
        DispInfo* pI = new DispInfo(pDS);
        m_XData.Add(pI);
        pDS = pDS->GetNext();
        }
      }
    Strng tmpStrng;
    for ( ; DataIndex<m_XData.GetSize(); DataIndex++)
      {
      pDS = m_XData[DataIndex]->pDS;
      tmpStrng.Set("%s.%s", pDS->GetTag(), pDS->GetVar());
      CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex, tmpStrng(), TABOpt_AllInfoOnce, XIO_In);
      if (!XM.PackMsg(DRqst)) //if it cannot fit - EXIT
        {
        delete DRqst;
        break;
        }
      }
    }
  else
    XM.Clear();
  }

//---------------------------------------------------------------------------

void CGrfWnd::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  //XM.Clear();
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_TagsNotAvail(CXMsgLst &XM)
  {
  while (XM.MsgAvail())
    {
    CXM_TagNotAvail * pX = XM.TagNotAvail();
    LogError("Graphics", 0, "Tag '%s' in '%s' not found.", pX->Tag, (const char*)(GetDocument()->GetTitle()));
    }
  XM.Clear();
  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_ReadSubsData(CXMsgLst &XM)
  {
  flag DataRead=0;
  return DataRead;
  }

//---------------------------------------------------------------------------

LRESULT CGrfWnd::OnDoWriteSubs(WPARAM wParam, LPARAM lParam)
  {
  CSingleLock Lock(&m_XDataLock, true);

  bool MustClose=false;
  const int Size = m_XData.GetSize();
  for (int i=0; i<Size; i++)
    {
    DispInfo &I=*m_XData[i];
    if (I.bDrawRqd)
      {
      I.bDrawRqd=false;
      if ((I.pDS==NULL && I.iFindCnt<5))
        {//was deleted, try find new one...
        I.pDS = Dsp.Vp1->FindDynamicSegment(I.sTag(), I.sVar(), DynFill);
        I.iFindCnt++;
        }
      if (I.pDS)
        {
        if (!MustClose)
          {
          MustClose=true;
          Dsp.Open();
          }
        Dsp.Vp1->DrawDynamicSegment(I.pDS, I.dRqdValue);
        }
      }
    }
  if (MustClose)
    Dsp.Close();
  return 0;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  if (gs_pPrj->m_bGrfAnimationOn)
    {
    CSingleLock Lock(&m_XDataLock, true);

    flag DoIt=false;
    while (XM.MsgAvail())
      {
      CXM_ObjectData *pX = XM.ObjectData();
      CPkDataItem * pItem = pX->FirstItem();
      int iIndex = pX->Index;
      if (IsData(pItem->Type()) && iIndex<m_XData.GetSize())
        {
        DispInfo* pI = m_XData.GetAt(iIndex);
        pI->dRqdValue = pItem->Value()->GetDouble();
        pI->bDrawRqd = true;
        DoIt=true;
        }
      }
    if (DoIt)
      PostMessage(WMU_GRFWNDWRITESUBS);
    }

  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  return False;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Stopping(flag fBeginStopping)
  {
  if (!fBeginStopping)
    {
    if (gs_pPrj->m_bGrfUpdateAnnot)
      {
      //PostMessage(WM_COMMAND, ID_GRF_RefreshAnnotation, 0); Unfortunatly, this doesn't work!
      PostMessage(WMU_DOGRFCMD, ID_GRF_RefreshAnnotation, 0);
      }
    if (!gs_pPrj->m_bGrfAnimationOn)
      {//update all dynamic fills/bars
      //todo
      }
    }

  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Stop(CXM_TimeControl &CB)
  {
  CSingleLock Lock(&m_XDataLock, true);

  for (int i=0; i<m_XData.GetSize(); i++)
    delete m_XData[i];
  m_XData.RemoveAll();
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CGrfWnd::OnDoDspUpdate(WPARAM wParam, LPARAM lParam)
  {
  flag ForceUpd=(flag)wParam;
  flag ColoursChgd=(flag)lParam;

  Dsp.Open();
  CRect ClpBox;
  int i=::GetClipBox(Dsp.HDC_List[Dsp.Opens], &ClpBox);

  if (i!=ERROR && i!=NULLREGION)
    {
    Dsp.SetUpdateFlags(true, ColoursChgd);
    if (UpdateForPaint(ForceUpd!=0, ColoursChgd!=0))
      Dsp.Paint(ClpBox);
    Dsp.SetUpdateFlags(false, false);
    }
  Dsp.Close();
  return 0;
  }

//---------------------------------------------------------------------------

void CGrfWnd::EO_DisplayUpdate(byte Options, const CXM_TimeControl &CB)
  {
  //dbgpln("CGrfWnd::EO_DisplayUpdate");

  if (gs_pTheSFELib)
    {
#if (!NewGrfUpdate)
    flag DoIt=false;
    CModelStatusCriterion Criteria;
    if (gs_pTheSFELib->RequestModelStatusChgCnt(Criteria, 1)>0)
      {
      gs_pTheSFELib->SetLoadAllModelStatus();
      gs_pTheSFELib->SetLoadAllModelAssocGrfs();
      DoIt=true;
      }
    dword NumStatusColourChgs=gs_pTheSFELib->GetNumStatusColourChgs();
    if (ForceUpd || ColoursChgd || DoIt || NumStatusColourChgs!=PrevNumStatusColourChgs)
      {
      PrevNumStatusColourChgs=NumStatusColourChgs;

      PostMessage(WMU_GRFWNDDSPUPDATE, ForceUpd, ColoursChgd);
      }
#else
    PostMessage(WMU_GRFWNDDSPUPDATE, (Options & UDO_ForceUpd)!=0, (Options & UDO_ColoursChgd)!=0);
#endif
    }
  };

//---------------------------------------------------------------------------

void CGrfWnd::OnLButtonDown(UINT nFlags, CPoint point)
  {
  //dbgpln("CGrfWnd::OnLButtonDown >>>>>>>>>>>>>>>>");
  MDIActivateThis(this);

  flag DMR = gs_pCmd->BTNDefModifierReqd();
  if (DMR)
    {
    gs_pCmd->ProcessAStr(gs_pCmd->BTNDefModifier());
    gs_pCmd->ProcessAStr(" ");
    }

  if (gs_pCmd->BTNBlocked())
    {
    BringWindowToTop();

    if( !gs_pCmd->Busy() )
      {
      Dsp.Open();
      Dsp.ButtonDownBegin(point, 0, gs_pCmd->CursNo());
      gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
      gs_pCmd->ExecObjAcc(EX_MOUSE_LDOWN);
      gs_pCmd->SetShiftCtrl(0,0);
      Dsp.ButtonDownEnd(point, 0);

      Dsp.Close();
      }
    else
      {
      //dbgpln("OnLButtonDown BringToTop");
      //MessageBox(gs_pCmd->HWindow, "Unexpected Mouse Input", "COMMAND", MB_ICONEXCLAMATION|MB_OK);
#if dbgExecCmdLine
      dbgpln("Unexpected Mouse Input Down+");
#endif

      }
    }
  else
    {
    Dsp.Open();
    Dsp.ButtonDownBegin(point,0, gs_pCmd->CursNo());
    gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
    gs_pCmd->ExecCmdLine(EX_MOUSE_LDOWN);
    gs_pCmd->SetShiftCtrl(0,0);
    Dsp.ButtonDownEnd(point,0);
    Dsp.Close();
    }

  gs_pCmd->SetFocus();
  //dbgpln("CGrfWnd::OnLButtonDown <<<<<<<<<<<<<<<<");
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnLButtonUp(UINT nFlags, CPoint point)
  {
  //dbgpln("CGrfWnd::OnLButtonUp >>>>>>>>>>>>>>>>");
  //MDIActivateThis(this);
  if (Dsp.ButtonDown)
    {
    pDoc=dynamic_cast<CGrfDoc*>(GetDocument());
    if (pDoc)
      {
      GrfCmdBlk & GCB=((CGrfDoc*)pDoc)->GCB;
      if (GCB.bGrfLoadDefered)
        {
        flag LoadIt=1;
        if (!gs_pPrj->m_bGrfAutoLoad)
          {
          Strng S;
          S.Set("Load : %s", GCB.DrawingFile.GetBuffer(0));
          LoadIt=(AfxMessageBox(S(), MB_YESNO|MB_ICONQUESTION)==IDYES);
          }

        if (LoadIt)
          {
          CWaitMsgCursor Wait("Loading drawing");
          GCB.Load(GCB.DrawingFile.GetBuffer(0), False);
          GCB.bGrfLoadDefered=0;
          }
        }
      }

    if( !gs_pCmd->Busy() )
      {
      Dsp.Open();
      Dsp.ButtonUpBegin(point, 0);//gs_pCmd->CursNo());
      gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
      gs_pCmd->ExecObjAcc(EX_MOUSE_LUP);
      gs_pCmd->SetShiftCtrl(0,0);
      Dsp.ButtonUpEnd(point, 0);
      Dsp.Close();
      }
    else
      {
      Dsp.Open();
      Dsp.ButtonUpBegin(point,0);
      gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
      gs_pCmd->ExecCmdLine(EX_MOUSE_LUP);
      gs_pCmd->SetShiftCtrl(0,0);
      Dsp.ButtonUpEnd(point,0);
      Dsp.Close();
      }
    }
  else
    {
    CWnd *A=AfxGetMainWnd()->GetTopWindow();
    if (A==this)
      {
      int xxx=0;
      }

    dbgpln("Do MdlValue Show %08x %08x", this, A);
    }
  //dbgpln("CGrfWnd::OnLButtonUp <<<<<<<<<<<<<<<<");
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  //dbgpln("LButton  DBlClk");
  MDIActivateThis(this);
  if (!gs_pCmd->DblClkBlocked())
    {
    Dsp.Open();
    Dsp.ButtonDownBegin(point, 0, gs_pCmd->CursNo());
    gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
    gs_pCmd->ExecObjAcc(EX_MOUSE_LDCLK);
    gs_pCmd->SetShiftCtrl(0,0);
    Dsp.ButtonDownEnd(point, 0);
    Dsp.Close();
    }
  gs_pCmd->SetFocus();
  CView::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnRButtonDown(UINT nFlags, CPoint point)
  {
  MDIActivateThis(this);

  Dsp.Open();
  Dsp.ButtonDownBegin(point, 0, gs_pCmd->CursNo());
  gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
  gs_pCmd->ExecObjAcc(EX_MOUSE_RDOWN);
  gs_pCmd->SetShiftCtrl(0,0);
  Dsp.ButtonDownEnd(point,0);
  Dsp.Close();

  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnRButtonUp(UINT nFlags, CPoint point)
  {
  if (Dsp.ButtonDown)
    {
    pDoc=dynamic_cast<CGrfDoc*>(GetDocument());
    if (pDoc)
      {
      GrfCmdBlk & GCB=((CGrfDoc*)pDoc)->GCB;
      if (GCB.bGrfLoadDefered)
        {
        flag LoadIt=1;
        if (!gs_pPrj->m_bGrfAutoLoad)
          {
          Strng S;
          S.Set("Load : %s", GCB.DrawingFile.GetBuffer(0));
          LoadIt=(AfxMessageBox(S(), MB_YESNO|MB_ICONQUESTION)==IDYES);
          }

        if (LoadIt)
          {
          CWaitMsgCursor Wait("Loading drawing");
          GCB.Load(GCB.DrawingFile.GetBuffer(0), False);
          GCB.bGrfLoadDefered=0;
          }
        }
      }

    Dsp.Open();
    Dsp.ButtonUpBegin(point, 0);
    gs_pCmd->SetShiftCtrl((nFlags & MK_SHIFT)!=0, (nFlags & MK_CONTROL)!=0);
    gs_pCmd->ExecObjAcc(EX_MOUSE_RUP);
    gs_pCmd->SetShiftCtrl(0,0);
    Dsp.ButtonUpEnd(point, 0);
    Dsp.Close();
    }
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnMouseMove(UINT nFlags, CPoint point)
  {
  Dsp.Open();
  if (Dsp.ButtonDown)
    {
    Dsp.MouseMoveBegin(point);
    gs_pCmd->ExecCmdLine(EX_MOUSE_LMOVE);
    Dsp.MouseMoveEnd(point);
    }
  else
    {
    //Dsp.MouseMoveBegin(point);  // mhm comeback - may cause trouble
    Dsp.SetCurrentPt(point);
    //Dsp.MouseMoveEnd(point);    // mhm comeback - may cause trouble
    }

  if (!gs_Exec.Busy())
    {
    char Txt[512];
    if (1)
      sprintf(Txt, "%7.2f, %7.2f",Range(-9999999.0, Dsp.CurrentPt.World.X, 9999999.0), Range(-9999999.0, Dsp.CurrentPt.World.Y, 9999999.0));
    else
      sprintf(Txt, "%i,%i  %.2f,%.2f", point.x, point.y,Range(-9999999.0, Dsp.CurrentPt.World.X, 9999999.0), Range(-9999999.0, Dsp.CurrentPt.World.Y, 9999999.0));
    pStatusBar->UpdateIndicator(1, Txt, FALSE);
    }
  Dsp.Close();
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  gs_pCmd->SendMessage(WM_CHAR, nChar, MAKELONG(nRepCnt, nFlags));
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  gs_pCmd->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  gs_pCmd->SendMessage(WM_KEYUP, nChar, MAKELONG(nRepCnt, nFlags));
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnSize(UINT nType, int cx, int cy)
  {
  CView::OnSize(nType, cx, cy);
  // TODO: Add your message handler code here
  Invalidate();
  }

//---------------------------------------------------------------------------

BOOL CGrfWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
  {
  if (OptionCursor)
    ::SetCursor(OptionCursor);
  else
    ::SetCursor(BaseCursor);
  return CView ::OnSetCursor(pWnd, nHitTest, message);
  }

//---------------------------------------------------------------------------

BOOL CGrfWnd::SetCursor(UINT ReqdCursor)
  {
  if (ReqdCursor>0)
    OptionCursor=ScdApp()->LoadCursor(ReqdCursor);
  else
    OptionCursor=NULL;

  if (OptionCursor)
    ::SetCursor(OptionCursor);
  else
    ::SetCursor(BaseCursor);

  POINT Pt;
  if (GetCursorPos(&Pt)) // force cursor to update itself
    {
    SetCursorPos(Pt.x+1, Pt.y);
    SetCursorPos(Pt.x, Pt.y);
    }
  return True;
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnFilePrint()
  {
  CView::OnFilePrint();
  }

void CGrfWnd::OnFilePrintMultiple()
  {
  CMultiPrintSelect Dlg(this);

  POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
  while (Pos)
    {
    CGrfDoc* pGrfDoc = (CGrfDoc*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
    Dlg.AddDocument(pGrfDoc);
    }

  Dlg.AddStartDocument(this->GetDocument());

  if (Dlg.DoModal()==IDOK)
    {
    //Dlg.m_Documents // now contain doc to print
    POSITION Pos=Dlg.m_Documents.GetHeadPosition();
    while (Pos)
      {
      CDocument * pDoc=Dlg.m_Documents.GetNext(Pos);
      POSITION VPos=pDoc->GetFirstViewPosition();
      CGrfWnd * pWnd  = dynamic_cast<CGrfWnd*>(pDoc->GetNextView(VPos));
      if (pWnd)
        pWnd->OnFilePrint();

      // Change to Print Direct to stop the PrintDlg box
      // see http://www.codeguru.com/forum/archive/index.php/t-52473.html
      MSG& msg = AfxGetThreadState()->m_lastSentMsg;
      msg.wParam = ID_FILE_PRINT_DIRECT; 
      }
    };
  };

//---------------------------------------------------------------------------

BOOL CGrfWnd::OnPreparePrinting(CPrintInfo* pInfo)
  {
  pInfo->SetMaxPage(1);
  return DoPreparePrinting(pInfo);
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
  {
  CView::OnBeginPrinting(pDC, pInfo);
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
  {
  CView::OnEndPrinting(pDC, pInfo);
  }

//---------------------------------------------------------------------------

void CGrfWnd::OnPrint(CDC* pDC, CPrintInfo* pInfo)
  {
  Dsp.SetPrintInfo(pInfo);
  CView::OnPrint(pDC, pInfo);
  }

//---------------------------------------------------------------------------

void CGrfWnd::CopyBMPtoClipBoard(int Format)
  {
  if (!OpenClipboard())
    {
    //AfxMessageBox( "Cannot open the Clipboard" );
    return;
    }

  if (!EmptyClipboard())// Remove the current Clipboard contents
    {
    //AfxMessageBox( "Cannot empty the Clipboard" );
    return;
    }

  bool Failed=true;
  if (Format!=0)
    {
    bPretendPrinting=1;
    Invalidate(); //Invalidate entire client area
    UpdateWindow(); //forced redraw
    }

  RECT rect;
  GetWindowRect(&rect);
  int fwidth = max(2, rect.right - rect.left - 1);
  int fheight = max(2, rect.bottom - rect.top - 1);

  // Get the device context of window and allocate memory
  CDC memdc;
  memdc.CreateCompatibleDC(GetDC());
  CBitmap BM;
  BOOL B = BM.CreateCompatibleBitmap(&memdc, fwidth, fheight);
  if (B)
    {
    CBitmap* pOldBm = memdc.SelectObject(&BM);
    // Copy the bitmap to memory location based on coordinates.
    B = memdc.BitBlt(0, 0, fwidth, fheight, GetDC(), -1, -1, SRCCOPY);
    if (B)
      {
      HANDLE hData = (HBITMAP)BM;
      if (::SetClipboardData(CF_BITMAP, hData)!=NULL)
        Failed=false;
      //else
      //  AfxMessageBox( "Unable to set Clipboard data" );
      }
    memdc.SelectObject(pOldBm);
    }

  CloseClipboard();
  bPretendPrinting=0;
  if (Format!=0)
    {
    Invalidate(); //Invalidate entire client area
    UpdateWindow(); //forced redraw
    }
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CGrfFrameWnd, CMDIChildWnd)

CGrfFrameWnd::CGrfFrameWnd()
  {
  m_PrevClientX=0;
  m_PrevClientY=0;
  }

//---------------------------------------------------------------------------

CGrfFrameWnd::~CGrfFrameWnd()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGrfFrameWnd, CMDIChildWnd)
  //{{AFX_MSG_MAP(CGrfFrameWnd)
  ON_WM_SETFOCUS()
  ON_WM_SIZE()
  ON_WM_MOVE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CGrfFrameWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  if (message==WM_CLOSE)
    {//intercept close message...
    if (gs_Exec.Busy())
      {
      LogError("SysCAD", 0, "Must not be running");
      return True;
      }
    int Ret=AfxMessageBox("Are you sure you want to close the graphics window?", MB_ICONQUESTION|MB_YESNO);
    if (Ret==IDYES)
      {
      CGrfDoc * pDoc = dynamic_cast<CGrfDoc*>(GetActiveDocument());
      if (pDoc)
        {
        if (pDoc->DeleteTags(true)<0)
          return true;
        }
      //GetActiveDocument()->SetModifiedFlag(FALSE);
      if (GetActiveDocument())
        GetActiveDocument()->SetModifiedFlag(FALSE);
      return CMDIChildWnd::OnWndMsg(message, wParam, lParam, pResult);
      }
    return true;
    }
  return CMDIChildWnd::OnWndMsg(message, wParam, lParam, pResult);
  }

//---------------------------------------------------------------------------

void CGrfFrameWnd::OnSetFocus(CWnd* pOldWnd)
  {
  if (gs_pCmd->BusyDocument())
    {
    POSITION pos = gs_pCmd->BusyDocument()->GetFirstViewPosition();
    ASSERT(pos);
    CView* pView = gs_pCmd->BusyDocument()->GetNextView(pos);
    if (GetActiveView()!=pView)
      pView->PostMessage(WMU_DOSETFOCUS, 0, (LPARAM)pView);
    }
  else
    CMDIChildWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CGrfFrameWnd::OnSize(UINT nType, int cx, int cy)
  {
  CMDIChildWnd::OnSize(nType, cx, cy);
  if (nType==SIZE_MAXIMIZED)
    CWindowLists::SetWndMaxMode(TRUE);
  else if (nType==SIZE_MINIMIZED || nType==SIZE_RESTORED)
    CWindowLists::SetWndMaxMode(FALSE);

  if (gs_pPrj->m_GrfBehaviour && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CAutoIncDec AID(gs_BlockSizeAdjustment);
    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    if (gs_FileNewFlag)
      {
      if (WL.GrfWnds.GetCount()==1) // First
        {
        CRect IRect;
        CWnd * p=ScdMainWnd();
        if (dynamic_cast<CMainFrame*>(p)->GetInitRect(1, IRect))
          {
          WINDOWPLACEMENT wp;
          wp.length = sizeof(wp);
          GetWindowPlacement(&wp);
          wp.rcNormalPosition=IRect;
          SetWindowPlacement(&wp);
          }
        }
      }
    else
      {
      WINDOWPLACEMENT wp;
      wp.length = sizeof(wp);
      GetWindowPlacement(&wp);
      int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
      int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

      for (int i=0; i<WL.GrfWnds.GetCount(); i++)
        {
        if (WL.GrfWnds[i]!=this)
          {
          WINDOWPLACEMENT wpt;
          wpt.length = sizeof(wpt);
          WL.GrfWnds[i]->GetWindowPlacement(&wpt);
          if (gs_pPrj->m_GrfBehaviour & WB_Coincident)
            {
            wpt.rcNormalPosition=wp.rcNormalPosition;
            }
          else if (gs_pPrj->m_GrfBehaviour & WB_SizeTogether)
            {
            wpt.rcNormalPosition.right=wpt.rcNormalPosition.left+W;
            wpt.rcNormalPosition.bottom=wpt.rcNormalPosition.top+H;
            }
          wpt.length = sizeof(wpt);
          WL.GrfWnds[i]->SetWindowPlacement(&wpt);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CGrfFrameWnd::OnMove(int x, int y)
  {
  CMDIChildWnd::OnMove(x, y);

  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  GetWindowPlacement(&wp);
  int DX=wp.rcNormalPosition.left-m_PrevClientX;
  int DY=wp.rcNormalPosition.top-m_PrevClientY;

  if ((gs_pPrj->m_GrfBehaviour || gs_FileNewFlag) && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CAutoIncDec AID(gs_BlockSizeAdjustment);
    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    if (gs_FileNewFlag)
      {
      CWnd * pTopWnd=NULL;
      int Z=10000;
      WL.BuildZOrder(true);
      for (int i=0; i<WL.Wnds.GetCount(); i++)
        {
        CTopWindowInfo & WI=WL.Wnds[i];
        CWnd * pWnd=WI.pWnd;
        if (pWnd->IsKindOf(RUNTIME_CLASS(CGrfFrameWnd)) && pWnd!=this)
          {
          if (WI.iZOrd<Z)
            {
            Z=WI.iZOrd;
            pTopWnd=WI.pWnd;
            }
          }
        }
      if (pTopWnd)
        {
        wp.length = sizeof(wp);
        pTopWnd->GetWindowPlacement(&wp);
        wp.length = sizeof(wp);
        SetWindowPlacement(&wp);
        }
      }
    else
      {
      //WINDOWPLACEMENT wp;
      wp.length = sizeof(wp);
      GetWindowPlacement(&wp);
      int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
      int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

      for (int i=0; i<WL.GrfWnds.GetCount(); i++)
        {
        if (WL.GrfWnds[i]!=this)
          {
          WINDOWPLACEMENT wpt;
          wpt.length = sizeof(wpt);
          WL.GrfWnds[i]->GetWindowPlacement(&wpt);
          if (gs_pPrj->m_GrfBehaviour & WB_Coincident)
            {
            wpt.rcNormalPosition=wp.rcNormalPosition;
            }
          else if (gs_pPrj->m_GrfBehaviour & WB_MoveTogether)
            {
            wpt.rcNormalPosition.left+=DX;
            wpt.rcNormalPosition.right+=DX;
            wpt.rcNormalPosition.top+=DY;
            wpt.rcNormalPosition.bottom+=DY;
            }
          wpt.length = sizeof(wpt);
          WL.GrfWnds[i]->SetWindowPlacement(&wpt);
          }
        }
      }
    }

  wp.length = sizeof(wp);
  GetWindowPlacement(&wp);
  m_PrevClientX=wp.rcNormalPosition.left;
  m_PrevClientY=wp.rcNormalPosition.top;
  }

//---------------------------------------------------------------------------

void CGrfFrameWnd::DoBehaviourChange()
  {
  if (gs_pPrj->m_GrfBehaviour && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CAutoIncDec AID(gs_BlockSizeAdjustment);
    WINDOWPLACEMENT wp;
    wp.length = sizeof(wp);

    if (gs_FileNewFlag)
      {
      CWindowLists WL;
      CWnd * pTopWnd=NULL;
      CWnd * pTopWndBut1=NULL;
      int Z=10000;
      WL.BuildZOrder(true);
      for (int i=0; i<WL.Wnds.GetCount(); i++)
        {
        CTopWindowInfo & WI=WL.Wnds[i];
        CWnd * pWnd=WI.pWnd;
        if (pWnd->IsKindOf(RUNTIME_CLASS(CGrfFrameWnd)))// && pWnd!=this)
          {
          if (WI.iZOrd<Z)
            {
            Z=WI.iZOrd;
            pTopWndBut1=pTopWnd;
            pTopWnd=WI.pWnd;
            }
          }
        }
      if (pTopWndBut1)
        {
        wp.length = sizeof(wp);
        pTopWndBut1->GetWindowPlacement(&wp);
        wp.length = sizeof(wp);
        pTopWnd->SetWindowPlacement(&wp);
        }
      }
    else
      {
      CWindowLists WL;
      if (WL.BuildZOrder()<0)
        return;

      if (WL.GrfWnds.GetCount()<1)
        return;


      int iTop=-1;
      int iZ=10000;
      for (int i=0; i<WL.Wnds.GetCount(); i++)
        {
        if (WL.Wnds[i].pWnd->IsKindOf(RUNTIME_CLASS(CGrfFrameWnd)) && WL.Wnds[i].iZOrd<iZ)
          {
          iTop=i;
          iZ=WL.Wnds[i].iZOrd;
          }
        }

      if (iTop>=0)
        {
        WL.Wnds[iTop].pWnd->GetWindowPlacement(&wp);

        for (int i=0; i<WL.GrfWnds.GetCount(); i++)
          {

          WINDOWPLACEMENT wpt;
          wpt.length = sizeof(wpt);
          WL.GrfWnds[i]->GetWindowPlacement(&wpt);
          if (gs_pPrj->m_GrfBehaviour & WB_Coincident)
            {
            wpt.rcNormalPosition=wp.rcNormalPosition;
            }
          else if (gs_pPrj->m_GrfBehaviour & WB_MoveTogether)
            {
            _asm int 3;
            //wpt.rcNormalPosition.left+=DX;
            //wpt.rcNormalPosition.right+=DX;
            //wpt.rcNormalPosition.top+=DY;
            //wpt.rcNormalPosition.bottom+=DY;
            }
          wpt.length = sizeof(wpt);
          WL.GrfWnds[i]->SetWindowPlacement(&wpt);

          }                         
        }
      }
    }
  }

//---------------------------------------------------------------------------
