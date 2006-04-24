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
  if (1)//m_UpdateRequested)
    {
    flag DoIt=false;
    CModelStatusCriterion Criteria(gs_pPrj->m_dwMdlStatusOptions);
    if (gs_pTheSFELib->RequestModelStatusChgCnt(Criteria, 1)>0)
      {
      //dbgpln("OnDraw   B  %10.3fms", SW.Lap()*1e3); 
      gs_pTheSFELib->SetLoadAllModelStatus();
      //dbgpln("OnDraw   C  %10.3fms", SW.Lap()*1e3); 
      DoIt=true;
      }
    dword NumStatusColourChgs=gs_pTheSFELib->GetNumStatusColourChgs();
    if (ForceUpd || ColoursChgd || DoIt || 
      NumStatusColourChgs!=PrevNumStatusColourChgs)
      {
      PrevNumStatusColourChgs=NumStatusColourChgs;

      return true;

      //PostMessage(WMU_GRFWNDDSPUPDATE, ForceUpd, ColoursChgd);
      }
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

flag CGrfWnd::EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd)
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Start(const CXM_TimeControl &CB)
  {
  return True;
  }

//---------------------------------------------------------------------------

void CGrfWnd::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  if (gs_pPrj->bGrfAnimationOn)
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
  if (gs_pPrj->bGrfAnimationOn)
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

flag CGrfWnd::EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  return False;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Stopping(flag fBeginStopping)
  {
  if (!fBeginStopping)
    {
    if (gs_pPrj->bGrfUpdateAnnot)
      {
      //PostMessage(WM_COMMAND, ID_GRF_RefreshAnnotation, 0); Unfortunatly, this doesn't work!
      PostMessage(WMU_DOGRFCMD, ID_GRF_RefreshAnnotation, 0);
      }
    if (!gs_pPrj->bGrfAnimationOn)
      {//update all dynamic fills/bars
      //todo
      }
    }

  return True;
  }

//---------------------------------------------------------------------------

flag CGrfWnd::EO_Stop(const CXM_TimeControl &CB)
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

void CGrfWnd::EO_DisplayUpdate(flag ForceUpd, flag ColoursChgd, const CXM_TimeControl &CB)
  {
  if (gs_pTheSFELib)
    {
#if (!NewGrfUpdate)
    flag DoIt=false;
    CModelStatusCriterion Criteria;
    if (gs_pTheSFELib->RequestModelStatusChgCnt(Criteria, 1)>0)
      {
      gs_pTheSFELib->SetLoadAllModelStatus();
      DoIt=true;
      }
    dword NumStatusColourChgs=gs_pTheSFELib->GetNumStatusColourChgs();
    if (ForceUpd || ColoursChgd || DoIt || NumStatusColourChgs!=PrevNumStatusColourChgs)
      {
      PrevNumStatusColourChgs=NumStatusColourChgs;

      PostMessage(WMU_GRFWNDDSPUPDATE, ForceUpd, ColoursChgd);
      }
#else
    PostMessage(WMU_GRFWNDDSPUPDATE, ForceUpd, ColoursChgd);
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
        if (!gs_pPrj->bGrfAutoLoad)
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
        if (!gs_pPrj->bGrfAutoLoad)
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
    sprintf(Txt, "%7.2f, %7.2f",
            Range(-9999999.0, Dsp.CurrentPt.World.X, 9999999.0),
            Range(-9999999.0, Dsp.CurrentPt.World.Y, 9999999.0));
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

  if (gs_pPrj->m_KeepGrfSameSize && gs_ProjectOpenFlag && gs_BlockSizeAdjustment==0)
    {
    CIncDecFlag IDF(gs_BlockSizeAdjustment);
    CWindowLists WL;
    if (WL.BuildSingleList()<0)
      return;

    if (gs_FileNewFlag)
      {
      for (int i=0; i<WL.GrfWnds.GetCount(); i++)
        {
        if (WL.GrfWnds[i]!=this)
          {
          WINDOWPLACEMENT wp;
          wp.length = sizeof(wp);
          WL.GrfWnds[i]->GetWindowPlacement(&wp);
          int W=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
          int H=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;

          // set new one
          wp.length = sizeof(wp);
          GetWindowPlacement(&wp);
          wp.rcNormalPosition.right=wp.rcNormalPosition.left+W;
          wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+H;
          wp.length = sizeof(wp);
          SetWindowPlacement(&wp);
          break; // done
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

          WINDOWPLACEMENT wp;
          wp.length = sizeof(wp);
          WL.GrfWnds[i]->GetWindowPlacement(&wp);
          wp.rcNormalPosition.right=wp.rcNormalPosition.left+W;
          wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+H;
          wp.length = sizeof(wp);
          WL.GrfWnds[i]->SetWindowPlacement(&wp);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------
