//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "tagvdoc.h"
#include "fixedit.h"
#include "tagvtext.h"
#include "tagvdlgs.h"
#include "project.h"
#include "mdlrunmngr.h"
#include "apprunmngr.h"
#include "selctwnd.h"
#include "scd_wm.h"
#include "syscad.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#include "optoff.h"

//===========================================================================
#define WithTagFldWidth 0

const int TV_TagFldWidthId  = 0;
const int TV_DummyId        = 1;
const int TV_ScrollToEnd    = 2;
const int TV_Reconect       = 3;

const int TV_FirstTrndFldId = 10; //------------------------------

const int TV_TrndFldId      = 10;
const int TV_RowDescFldId   = 11;
const int TV_TagFldId       = 12;
const int TV_DirnFldId      = 13;
const int TV_MeasFldId      = 14;
const int TV_MinFldId       = 15;
const int TV_MaxFldId       = 16;
const int TV_DescFldId      = 17;

const int TV_LastTrndFldId  = 19; //------------------------------

const int TV_LTrndFld1Id    = 20;
const int TV_LTrndFld2Id    = 21;
const int TV_LTrndFld3Id    = 22;
const int TV_LCtrlFldId     = 23;
const int TV_LMeasFldId     = 24;
const int TV_LSptFldId      = 25;
const int TV_LDiffFldId     = 26;
const int TV_LOutFldId      = 27;
const int TV_LDescFldId     = 28;

const int TV_LastLCtrlFldId = 29; //------------------------------

const int TV_AutoTune       = 30;
const int TV_TuneWithHold   = 31;
const int TV_HoldAll        = 32;
                            
const int TV_PlantGain      = 33;
const int TV_PlantDelay     = 34;
const int TV_PlantPeriod    = 35;
                            
const int TV_PIDGain        = 36;
const int TV_PIDIc          = 37;
const int TV_PIDDc          = 38;

const int TV_SettleMin      = 39;
const int TV_SettleMax      = 40;
const int TV_TestDelta      = 41;
const int TV_PBDeRate       = 42;
                            
const int TV_LastHCtrlFldId = 49; //------------------------------last header control field

//---------------------------------------------------------------------------

const UCHAR IDDspWdt      = 6;
const UCHAR TagDspWdt     = 30;
const UCHAR TypeDspWdt    = 2;
const UCHAR DirnDspWdt    = 2;
const UCHAR EngDspWdt     = 8;
const UCHAR MinDspWdt     = 6;
const UCHAR MaxDspWdt     = 6; 
const UCHAR MeasDspWdt    = 12;
const UCHAR DeltaDspWdt   = 8; 
const UCHAR FmtDspWdt     = 1;
const UCHAR OffsetDspWdt  = 5;
const UCHAR SpanDspWdt    = 5;
const UCHAR CollectDspWdt = 2;

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CTagVwText, FxdEdtView)

HWND CTagVwText::m_hCurrentTrend=NULL;

//---------------------------------------------------------------------------
const int MaxTagCmds=200; // NBNB See separ in resource.h

CTagVwText::CTagVwText()
  {
  NUsed=0;
  pUpDC=NULL;
  pMenuSlt = NULL;
  iMarginWidth = 1;
  m_uTimerId=0;
  m_uTimerCnt=0;
  m_iDragX=0;
  m_iDragI=-1;
  m_AddTagSlt=-1;
  m_sTag4Access.SetSize(MaxTagCmds);
  }

//---------------------------------------------------------------------------

CTagVwText::~CTagVwText()
  {
  m_hCurrentTrend=NULL;
  }

//---------------------------------------------------------------------------

BOOL CTagVwText::PreCreateWindow(CREATESTRUCT &cs)
  {
  cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_CLASSDC,
                                   NULL/*ScdApp()->LoadStandardCursor(IDC_ARROW)*/,
                                   (HBRUSH)GetStockObject(LTGRAY_BRUSH), 0);
  return CView::PreCreateWindow(cs);
  //return FxdEdtView::PreCreateWindow(cs); do NOT call FxdEdtView
  }

//---------------------------------------------------------------------------

void CTagVwText::OnInitialUpdate()
  {
  CString Title = GetDocument()->GetTitle();
  gs_pPrj->RestoreOneWindow((char*)(const char*)Title, GetParent(), false);
  DriverBitmap.LoadBitmap(IDB_TRND_DRIVERTAG);
  ArchiveBitmap.LoadBitmap(IDB_TRND_ARCHIVETAG);
  FindBitmap.LoadBitmap(IDB_TRND_FINDTAG);
  FxdEdtView::OnInitialUpdate();
  HCURSOR h=ScdApp()->LoadStandardCursor(IDC_ARROW);
  //SetCursor(h);
  //SetRqdCursor(NULL);
  SetRqdCursor(h);
  }

//---------------------------------------------------------------------------

int CTagVwText::SlotIndex(FxdEdtInfo &EI) { return EI.BlkRowNo-1; }
int CTagVwText::SlotIndex(pFxdEdtFld pFld) { return pFld->pRow->RowNo-1; }
int CTagVwText::SlotIndex(int Row, int Id) { return Row-1; };

//---------------------------------------------------------------------------

void CTagVwText::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
  {
  //CStopWatch SW;
  //SW.Start();
  FxdEdtView::OnActivateView(bActivate, pActivateView, pDeactiveView );
  //dbgpln("OnActivate Txt : %10.3f", SW.Secs());
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTagVwText, FxdEdtView)
  //{{AFX_MSG_MAP(CTagVwText)
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONUP()
  ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_ONUPDATE, OnUpdateByMsg)
  ON_COMMAND_RANGE(IDM_TRND_EDIT, IDM_TRND_LAST, OnMenu)
  ON_COMMAND_RANGE(IDM_TRNDSCL_FIRST, IDM_TRNDSCL_LAST, OnMenu)
  ON_COMMAND_RANGE(IDM_ACC_CNV1, IDM_ACC_CNVN, OnCnv)
  ON_COMMAND_RANGE(IDM_ACC_FMT1, IDM_ACC_FMTN, OnFormat)
  ON_MESSAGE(WMU_ADDTAG2TREND, OnAddTag2Trend)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CTagVwText::DoOnFilePrint(BOOL UseColors)
  {
  bUseColorPrint = UseColors;
  if (CPg)
    strcpy(CPg->TabName, Doc()->GetTitle()); //sets print title
  OnFilePrint();
  }

//---------------------------------------------------------------------------

void CTagVwText::OnDrawBack(rGDIBlk GB, int PgNo, CRgn &ClipRgn)
  {
  FxdEdtView::OnDrawBack(GB, PgNo, ClipRgn);
  }

//---------------------------------------------------------------------------

void CTagVwText::OnDrawLine(rGDIBlk GB, int PgNo, int RowNo)
  {
  if (CRow(RowNo)->Visible(GB) || GB.DC().m_bPrinting==1)
    {
    CRow(RowNo)->Write(GB);
    FxdEdtInfo EI;
    if (LocateFromCR(0,RowNo, EI))
      {
      //dbgpln("Draw Trace %i.3",RowNo);
      CTagVwSlot * Slt = DataSlot(EI.BlkRowNo+EI.Index);
      FxdEdtFld * Fld = CRow(EI.PgRowNo)->FldFromId(TV_TrndFldId);
      if (Slt && Fld)
        {
        const int y1=Fld->Ys(GB);
        const int x2=Fld->Xe(GB);
        CDC &DC = GB.DC();
        CDCResChk ResChk(DC);

        CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);
        CBrush* OldBrush = DC.GetCurrentBrush();
        COLORREF OldBk = DC.SetBkColor(GB.crGrfBack);
        int Recx = x2+iMarginWidth;
        int Recy = y1+4;
        if (Slt->bDriverTag)
          {
          if (iMarginWidth==1)
            DoRebuild();

          CDC memdc;
          CDCResChk ResChk(DC);
          memdc.CreateCompatibleDC(&DC);
          CDCResChk ResChkMem(memdc);
          CBitmap* pOldBmp = memdc.SelectObject(&DriverBitmap);
          DC.BitBlt(x2+1, y1+1, 14, 11, &memdc, 0, 0, SRCCOPY);
          memdc.SelectObject(pOldBmp);
          Recx = x2+9;
          Recy = y1+7;
          }
        else if (Slt->bArchiveTag)
          {
          if (iMarginWidth==1)
            DoRebuild();

          CDC memdc;
          CDCResChk ResChk(DC);
          memdc.CreateCompatibleDC(&DC);
          CDCResChk ResChkMem(memdc);
          CBitmap* pOldBmp = memdc.SelectObject(&ArchiveBitmap);
          DC.BitBlt(x2+1, y1+1, 14, 11, &memdc, 0, 0, SRCCOPY);
          memdc.SelectObject(pOldBmp);
          Recx = x2+9;
          Recy = y1+7;
          }
#define ShowFindBitmap 0
#if ShowFindBitmap
        else if (!GDIBlk::LowRes() && Slt->TagValid())
          {
          CDC memdc;
          CDCResChk ResChk(DC);
          memdc.CreateCompatibleDC(&DC);
          CDCResChk ResChkMem(memdc);
          CBitmap* pOldBmp = pOldBmp = memdc.SelectObject(&FindBitmap);
          DC.BitBlt(x2+1, y1+(Slt->bRecorded ? 1 : 3), 9, 8, &memdc, 0, 0, SRCCOPY);
          memdc.SelectObject(pOldBmp);
          if (Slt->bRecorded)
            {
            Recx = x2+2;
            Recy = y1+6;
            }
          }
#endif

        if (Slt->bRecorded)
          {
          const int ElipSz = (GB.DC().m_bPrinting ? 28 : 7);
          CRect TrndR(Recx, Recy, Recx+ElipSz, Recy+ElipSz);
          CBrush Brush(RGB(255, 255, 0));
          DC.SelectObject(&Brush);
          DC.Ellipse(TrndR);
          }

        if (Slt->TagValid() || Slt->HasFunct())
          {
          const int tx1=Fld->Xs(GB);
          const int tx2=Fld->Xe(GB);
          const int ty1=Fld->Ys(GB);
          const int ty2=Fld->Ye(GB);
          const int tym=(ty2+ty1)/2;

          const flag TrndOn = (Slt->nTrendNo>=0);
          if (!GB.DC().m_bPrinting)
            {
            //draw trend 'push' button...
            CRect TrndR1(tx1, ty1, tx2, ty2);
            CBrush TrndBackBrush1(DC.GetNearestColor(Doc()->BackGroundColour));
            CBrush* pRqdPrush1 = TrndOn ? &TrndBackBrush1 : GB.pBrushTxtBack;
            DC.SelectObject(pRqdPrush1);
            DC.Rectangle(TrndR1);
            }

          if (!GB.DC().m_bPrinting || TrndOn)
            {
            COLORREF Colr = (TrndOn && (!GB.DC().m_bPrinting || bUseColorPrint)) ? Slt->lPen.lopnColor : RGB(128,128,128);
            CPen Pen(PS_SOLID, Slt->dwWidth, Colr);
            CPen* pOldPen1 = DC.SelectObject(&Pen);
            DC.MoveTo(tx1+1, ty2-2);    
            DC.LineTo((2*tx1+(tx2-1))/3, ty1+2);
            DC.LineTo((tx1+(tx2-1))/2+1, tym);    
            DC.LineTo(tx2-1, tym);
            if (TrndOn && Slt->bAutoScaleOn)
              {
              DC.MoveTo(tx2-3, ty1+1);    
              DC.LineTo(tx2-2, ty1+1);    
              DC.LineTo(tx2-2, ty2-2);    
              if (0)
                {
                DC.LineTo(tx2-3, ty2-2);
                DC.LineTo(tx2-3, ty1+1);
                }
              else
                DC.LineTo(tx2-4, ty2-2);
              }            
            DC.SelectObject(pOldPen1);
            }
          }

        DC.SelectObject(OldPen);
        DC.SelectObject(OldBrush);
        DC.SetBkColor(OldBk);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwText::OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint)
  {
  if (lHint & TGU_NowActivated)
    {
    m_hCurrentTrend=GetSafeHwnd();
    }
  if (lHint & TGU_NowDeactivated)
    {
    }
  if (lHint & (TGU_Start|TGU_Stop))
    lHint |= TGU_LoadDataComplete|TGU_UpdateAll;

  if (lHint & (TGU_LoadComplete))
    {
    DoRebuild();
    LoadStrings();
    FixScrollSizes(True);
    FxdEdtView::OnUpdate(pSender, 0, pHint);
    }
  if (lHint & (TGU_Toggle | TGU_TagView))
    {
    DoRebuild();
    LoadStrings();
    }
  if (lHint & (TGU_Toggle))
    {
    RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW); 
    }
  if (lHint & (TGU_LoadDataComplete))
    {
    DoReload();
    LoadStrings();
    }
  if (lHint & TGU_UpdateAll)
    {
    CClientDC UpDC(this);
    CDCResChk ResChk(UpDC);

    InitDC(&UpDC);
    ScrGB.Attach(&UpDC, NULL);

    int i = Doc()->iFirstChgdFld;
    while (i>=0)
      {
      CTagVwSlot * pSlot = Doc()->Slot(i);
      ValFlds[i]->SetMarks(pSlot->m_XRefStatus);
      pchar p = pSlot->sValue(); 
      if (ValFlds[i]->FieldId==TV_LCtrlFldId)
        lHint |= TGU_Repaint;//TGU_Colour;
      else
        CPg->SetFieldValue(ScrGB, ValFlds[i], p ? p : "");
      //NBNB: THIS MUST BE FIXED PROPERLY
      //NBNB: sometimes i == pSlot->iNextChgdFld causing an endless loop. This is almost certainly caused by dual CPU access to iFirstChgdFld, etc list!!!
      ASSERT_RDB(pSlot->iNextChgdFld!=i, "Going into permanent loop!");
      if (pSlot->iNextChgdFld==i)
        {//BUG
        i = -1; //jump out for now
        }
      else
        i = pSlot->iNextChgdFld;
      }

    ScrGB.Detach();
    ClrFldsChanged();
    }
  if (lHint & TGU_UpdateMinMax)
    {
    CClientDC UpDC(this);
    CDCResChk ResChk(UpDC);

    InitDC(&UpDC);
    ScrGB.Attach(&UpDC, NULL);

    for (int i=0; i<nValFlds; i++)
      if (MinFlds[i])
        {
        CTagVwSlot * pSlot = Doc()->Slot(i);
        pchar pn = pSlot->sMinVal(); 
        pchar px = pSlot->sMaxVal(); 
        CPg->SetFieldValue(ScrGB, MinFlds[i], pn ? pn : "");
        CPg->SetFieldValue(ScrGB, MaxFlds[i], px ? px : "");
        }

    ScrGB.Detach();
    ClrFldsChanged();
    }
  if (lHint & (0x1000))
    {
    FxdEdtView::OnUpdate(pSender, 0, pHint);
    LoadStrings();
    }
  if (lHint & (TGU_Colour))
    {
    FxdEdtView::OnUpdate(pSender, 0, pHint);
    }
  if (lHint & (TGU_Repaint))
    RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); 
  if (lHint==0)
    {
    FxdEdtView::OnUpdate(pSender, lHint, pHint);
    ClrFldsChanged();
    }
  }

//---------------------------------------------------------------------------

// by default only use first 7 colours (the bright ones)

const int MaxTrndColours = 7;
COLORREF DefTrndColours[MaxTrndColours] =
{ RGB(255,255,255),
  RGB(255,  0,  0),
  RGB(255,255,  0),
  RGB(  0,  0,255),
  RGB(  0,255,255),
  RGB(  0,255,  0),
  RGB(255,  0,255),
  /*  
  RGB(128,128,  0),
  RGB(  0,128,128),
  RGB(  0,  0,128),
  RGB(255,128,  0),
  RGB(128,  0,128),
  RGB(255,  0,128),
  RGB(  0,128, 64),
  RGB(192,192,192),
  RGB(128,128,128),
  RGB(  0,  0,160),
  RGB(128,128, 64),
  RGB(255,128,  0),
  RGB(  0,128,255),
  RGB(128,  0,  0),
  RGB(128,255,  0),
  RGB(255,128,128),
  RGB(255,128,255),
  RGB(128, 64,  0),
  RGB(128,128,255),
  RGB(255,128, 64),
  RGB(128,  0,255),
  RGB(128,128,192),
  RGB( 64,  0, 64),
  RGB( 64,128,128),
  RGB(  0,255,128) 
  */
  };

//---------------------------------------------------------------------------

void CTagVwText::StartBuild()
  {
  if (Doc()->ColumnWidths[0]==0)
    {
    Doc()->ColumnWidths[0]=TagDspWdt;
    Doc()->ColumnWidths[1]=MeasDspWdt;
    Doc()->ColumnWidths[2]=MinDspWdt;
    Doc()->ColumnWidths[3]=MaxDspWdt; 
    }
  
  NUsed=0;
  FxdEdtView::StartBuild();
  }

//---------------------------------------------------------------------------

void CTagVwText::Build(int NRows, int NCols, int ColIndent, pvoid BuildData)
  {
  if (NRows<=0)
    NRows = NoSlots()+1;
  ValFlds.SetSize(0,128);
  MinFlds.SetSize(0,128);
  MaxFlds.SetSize(0,128);
  nValFlds=0;
  //MnMxFlds.SetSize(0,128);
  //nMnMxFlds=0;
  
  CXM_Route DRoute;
  const flag DrvOn = (Doc()->XFindObject(pExecName_DrvMngr, DRoute));
//const flag DrvOn=1;//kga!
  iMarginWidth = (DrvOn ? 2 : 1);
  int MeasWdt=MeasDspWdtFn();
  int MinWdt=MinDspWdtFn();
  int MaxWdt=MaxDspWdtFn();
  int r;
  /*
  for (r=1; r<=NRows-1; r++)
    if (!IsStrng(Doc()->DS[r-1].cType))
      {
      MeasWdt=Max(MeasWdt, Doc()->DS[r-1].sValue.Length());
      MinWdt=Max(MinWdt, Doc()->DS[r-1].sMinVal.Length());
      MaxWdt=Max(MaxWdt, Doc()->DS[r-1].sMaxVal.Length());
      }
  */
  
  StartBlk(NRows, 0L+NUsed-1, NULL);
  NUsed+=(NRows-1);
  SetDesc(0, "", -1, 2, 0, iMarginWidth==1 ? " " : "  ");
  SetDesc(0, "", -1, ColIndent, 0, "");
  #if WithTagFldWidth
  SetDesc(0, "Tag", -1, TagDspWdtFn()-4, 0, " ");
  SetParm(0, "", TV_TagFldWidthId, 3, 0, " ");
  #else
  SetDesc(0, "Tag", -1, TagDspWdtFn()-6, 0, " ");
  if (GDIBlk::LowRes())
    {
    SetBitmapButton1(0, IDB_TRNDREFRESH_LOWRES, TV_Reconect, 2, 0, " ");
    SetBitmapButton1(0, IDB_TRNDSCROLLEND_LOWRES, TV_ScrollToEnd, 2, 0, " ");
    }
  else
    {
    SetBitmapButton2(0, IDB_TRNDREFRESH, TV_Reconect, 2, 0, " ");
    SetBitmapButton2(0, IDB_TRNDSCROLLEND, TV_ScrollToEnd, 2, 0, " ");
    }
  #endif
  SetDesc(0, "Meas", -1, MeasWdt, 2, " ");
  SetDesc(0, "Min", -1, MinWdt, 2, " ");
  SetDesc(0, "Max", -1, MaxWdt, 2, " ");
  SetDesc(0, "Description", -1, 60, 0, "");
  for (r=1; r<=NRows-1; r++)
    {
    CTagVwSlot &DS=Doc()->DS[r-1];
    char* RowDesc="";
    if (BuildData)
      RowDesc=((pchar*)(BuildData))[r-1];
    SetDesc(r, "", TV_TrndFldId, 2, 0, iMarginWidth==1 ? " " : "  ");
    SetDesc(r, RowDesc, -1 , ColIndent, 0, "");
    SetParm(r, "", TV_TagFldId, TagDspWdtFn(), 0, " ");
    if (DS.sTag.Length()>=TagDspWdtFn() || (DS.sTag.Length()>0 && DS.sTag[0]=='='))
      SetBigStrLength((DS.sTag.Length()>0 && DS.sTag[0]=='=') ? 2048 : 128);
    ASSERT_ALWAYS((r-1)==nValFlds, "UpdFld Index Mismatch");
    if (IsStrng(DS.cType))
      {
      byte Just= IsTimeCnv(DS.Cnv.Index()) ?2:0;
      ValFlds.SetAtGrow(nValFlds, SetData(r,"", TV_MeasFldId, MeasWdt+MinWdt+MaxWdt+2, Just, " "));
      MinFlds.SetAtGrow(nValFlds, NULL);
      MaxFlds.SetAtGrow(nValFlds, NULL);
      }
    else
      {
      byte Just=/*DS.sStrList.Length()>0 ? 0 :*/ 2;
      ValFlds.SetAtGrow(nValFlds, SetData(r,"", TV_MeasFldId, MeasWdt, Just, " "));
      MinFlds.SetAtGrow(nValFlds, SetParm(r, "", TV_MinFldId, MinWdt, Just, " "));
      MaxFlds.SetAtGrow(nValFlds, SetParm(r, "", TV_MaxFldId, MaxWdt, Just, " "));
      }
    nValFlds++;
    SetParm(r, "", TV_DescFldId, 60, 0, "");
    }
  
  FxdEdtView::Build(0, 0, 0);
  }

//---------------------------------------------------------------------------

void CTagVwText::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    //TRACE1("Load DataSlot %3i", RowId+Index1-1);
    CTagVwSlot * Slt=DataSlot(EI.BlkRowNo+EI.Index);
    char* p="";
    //dbgpln("Load DataSlot %3i %s", EI.BlkRowNo+EI.Index-1, Slt ? (Slt->TagValid() ? "Valid":"Invalid") : "-");

    #if WithTagFldWidth
    if (EI.FieldId==TV_TagFldWidthId)
      {
      char TFWBuff[32];
      sprintf(TFWBuff, "%i", TagDspWdtFn()); 
      p=TFWBuff; 
      }
    else 
    #endif
    if (Slt)
      {
      if (Slt->TagValid() || Slt->HasFunct())
        {
        switch (EI.FieldId)
          {
          case TV_TagFldId     : 
            p=Slt->Tag(); 
            break;
          #if WithTagFldWidth
          case TV_TagFldWidthId : 
            {
            char TFWBuff[32];
            sprintf(TFWBuff, "%i", TagDspWdtFn()); 
            p=TFWBuff; 
            break;
            }
          #endif
          //case TV_DirnFldId: p=Slt->sDirn(); break;
          //case TV_RawFldId: p=Slt->sRaw(); break;
          case TV_MeasFldId    : 
            p=Slt->sValue(); 
            if (XStopped())
              EI.Fld->fEditable=Slt->m_bEditableStopped; 
            else
              EI.Fld->fEditable=Slt->m_bEditableRunning; 
            EI.Fld->fIsResult=Slt->bIsResult; 
            EI.Fld->SetMarks(Slt->m_XRefStatus);
            break;
          case TV_MinFldId     : 
            p=Slt->sMinVal();
            EI.Fld->fEditable=!IsStrng(Slt->cType);
            EI.Fld->fIsResult=false;
            break;
          case TV_MaxFldId     : 
            p=Slt->sMaxVal();
            EI.Fld->fEditable=!IsStrng(Slt->cType);
            EI.Fld->fIsResult=false;
            break;
          case TV_DescFldId    : 
            p=Slt->sDesc();
            EI.Fld->fEditable=!Slt->bDriverTag && !Slt->bArchiveTag;
            EI.Fld->fIsResult=false;
            break;
          default:;
          }
        }
      else
        {
        switch (EI.FieldId)
          {
          case TV_TagFldId : 
            p=Slt->Tag(); 
            break;
          case TV_MeasFldId :
            EI.Fld->SetMarks(0);
          case TV_MinFldId  :
          case TV_MaxFldId  : 
            p=""; 
            EI.Fld->fEditable=!Slt->IsComment(); 
            break;
          case TV_DescFldId : 
            p=Slt->sDesc(); 
            EI.Fld->fEditable=!Slt->IsComment(); 
            break;
          default: p=""; break;
          }
        }
      }
    Str=p ? p : "";
    //Str[Len]=0;
    }
  };

//---------------------------------------------------------------------------

long CTagVwText::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    CTagVwSlot * Slt=DataSlot(EI.BlkRowNo+EI.Index);
    #if WithTagFldWidth
    if (EI.FieldId==TV_TagFldWidthId)
      {
      TagDspWdtFn()=Max(Str.SafeAtoL(), 8L);
      DoRebuild();
      }
    else 
    #endif
    if (Slt)
      {
      if (EI.FieldId==TV_TagFldId)
        {
        dbgpln("Tag Change : %s", Str());
        Slt->ChangeTag(Str(), Doc(), DefTrndColours[(EI.BlkRowNo+EI.Index) % MaxTrndColours]);

        if (!EI.fPasteBusy)
          Doc()->UpdateAllViews(NULL, TGU_Toggle, NULL);
        DoRebuild();
        CRow(EI.PgRowNo)->Invalidate(ScrGB);
        ClrFldsChanged();
        }
      else if (Slt->TagValid() || Slt->HasFunct())
        {
        flag UpdateRqd=(!gs_Exec.Busy() || gs_Exec.Waiting());
        switch (EI.FieldId)
          {
          case TV_DirnFldId: break;
          case TV_MeasFldId:
            if (Slt->TagValid())
              {
              CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
              if (!Slt->SetOutput(pDoc, Str(), TRUE, TRUE)) 
                {
                char buff[256];
                sprintf(buff, "%s could not be changed", Slt->Tag());
                AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
                UpdateRqd=0;
                //CRow(EI.PgRowNo)->Invalidate(ScrGB);
                }
              else if (UpdateRqd  && !EI.fPasteBusy)
                Doc()->LoadData(Slt, TGU_LoadDataComplete);
              break;
              }
            break;
          case TV_MaxFldId: 
            if (Slt->bTagValid && IsNumData(Slt->cType) && Str.Find(' ')>0)
              Slt->sMaxVal = Str.Left(Str.Find(' '));
            else
              Slt->sMaxVal = Str;
            if (Slt->nTrendNo>=0 && !EI.fPasteBusy)
              {
              Doc()->UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
              Doc()->FixTimeBase();
              }
            break;
          case TV_MinFldId: 
            if (Slt->bTagValid && IsNumData(Slt->cType) && Str.Find(' ')>0)
              Slt->sMinVal = Str.Left(Str.Find(' '));
            else
              Slt->sMinVal = Str;
            if (Slt->nTrendNo>=0 && !EI.fPasteBusy)
              {
              Doc()->UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
              Doc()->FixTimeBase();
              }
            break;
          case TV_DescFldId: 
            Slt->sDesc=Str; 
            break;
          default:;
          }
        if (UpdateRqd)
          {
          DoReload();
          }
        if (!EI.fPasteBusy)
          Doc()->UpdateAllViewsByMsg(NULL, TGU_Toggle, NULL);
        ClrFldsChanged();
        }
      }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CTagVwText::Changed(FxdEdtInfo &EI)
  {
  if (CurrentBlk(EI))
    {
    CTagVwSlot * Slt=DataSlot(EI.BlkRowNo+EI.Index);
    if (Slt && (Slt->TagValid() || Slt->HasFunct()))
      switch(EI.FieldId)
        {
        //case TV_RawFldId:
        //  return Slt->RawChanged();
        case TV_MeasFldId:
          return Slt->MeasChanged();
        default:
          return TRUE;
        }
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

void CTagVwText::ClearChanged(FxdEdtInfo &EI)
  {
  if (CurrentBlk(EI))
    {
    CTagVwSlot * Slt=DataSlot(EI.BlkRowNo+EI.Index);
    if (Slt && (Slt->TagValid() || Slt->HasFunct()))
      switch(EI.FieldId)
        {
        //case TV_RawFldId:
        //  Slt->ClearRawChanged();
        //  return;
        case TV_MeasFldId:
          Slt->ClearChanged();
          return;
        default:;
        }
    }
  }

//---------------------------------------------------------------------------

void CTagVwText::StartFldUpdate()
  {
  pUpDC = new CClientDC(this);
  pResChk= new CDCResChk(pUpDC);
  InitDC(pUpDC);
  ScrGB.Attach(pUpDC, NULL);
  }

//---------------------------------------------------------------------------

void CTagVwText::UpdateFld(int i, pchar p)
  {
  CPg->SetFieldValue(ScrGB, ValFlds[i], p); 
  }

//---------------------------------------------------------------------------

void CTagVwText::EndFldUpdate()
  {
  ScrGB.Detach();
  delete pResChk;
  delete pUpDC;
  }

//---------------------------------------------------------------------------

void CTagVwText::SelectTrendTag(int i)
  {
  MoveCursorTo(0, i+1, 3, true);
  }

//---------------------------------------------------------------------------

int CTagVwText::CalcRowRange(int &R1, int &R2)
  {
  R1=-1;
  R2=-1;
  for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
    {
    if (R1>=0)
      {
      R1=Min(R1, (int)(pFld->pRow->RowNo));
      R2=Max(R2, (int)(pFld->pRow->RowNo));
      }
    else
      {
      R1=pFld->pRow->RowNo;
      R2=pFld->pRow->RowNo;
      }
    }
  return ((R1==-1 || R2==-1) ? 0 : R2-R1);
  }

//---------------------------------------------------------------------------

void CTagVwText::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  FxdEdtView::OnLButtonDblClk(nFlags, point);
  FxdEdtInfo EI;
  if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
    {
    int xxx=0;
    //if (!EI.Fld->Editing)
    //  {
    //  pFxdEdtFld Fld=CRow((int)ChEditPos.y)->FldFromId(TV_TrndFldId);
    //  if (Fld)
    //    {
    //    int iSltNo=EI.BlkRowNo+EI.Index;
    //    CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
    //    if (pDoc->DS[iSltNo].TagValid())
    //      pDoc->ToggleTrending(iSltNo);
    //    }
    //  }
    }
  }

//---------------------------------------------------------------------------

void CTagVwText::OnLButtonDown(UINT nFlags, CPoint point) 
  {
	FxdEdtView::OnLButtonDown(nFlags, point);
  FxdEdtInfo EI;
  if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
    {
    if (EI.Fld->FieldId==-1)
      {
      //Look at fld to right
      FxdEdtInfo EI1;
      if (LocateFromCR((int)ChEditPos.x+1, (int)ChEditPos.y, EI1))
        {
        m_iDragI=-1;
        switch (EI1.Fld->FieldId)
          {
          case TV_MeasFldId:
            {
            m_iDragX=point.x;
            m_iDragI=0;
            break;
            }
          case TV_MinFldId:
            {
            m_iDragX=point.x;
            m_iDragI=1;
            break;
            }
          case TV_MaxFldId:
            {
            m_iDragX=point.x;
            m_iDragI=2;
            break;
            }
          case TV_DescFldId:
            {
            m_iDragX=point.x;
            m_iDragI=3;
            break;
            }
          }
        if (m_iDragI>=0)
          {
          HCURSOR h=ScdApp()->LoadCursor(MAKEINTRESOURCE(IDC_ADJUSTEW));
          //HCURSOR h=ScdApp()->LoadStandardCursor(IDC_SIZEWE);
          //SetCursor(h);
          SetRqdCursor(h);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------
flag WdtChanged(int & Wdt, int DX, int Mn)
  {
  int Prev = Wdt;
  Wdt = Range(Mn, Wdt+DX, 124);
  return (Prev!=Wdt);
  }

void CTagVwText::OnLButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView::OnLButtonUp(nFlags, point);
  FxdEdtInfo EI;
  if (m_iDragI>=0)
    {
    const int DX=(point.x-m_iDragX)/ScrGB.ColWdt();
    flag Changed = false;
    switch (m_iDragI)
      {
      case 0: Changed = WdtChanged(TagDspWdtFn(), DX, 9); break;
      case 1: Changed = WdtChanged(MeasDspWdtFn(), DX, 3); break;
      case 2: Changed = WdtChanged(MinDspWdtFn(), DX, 2); break;
      case 3: Changed = WdtChanged(MaxDspWdtFn(), DX, 2); break;
      }
    if (Changed)
      {
      DoRebuild();
      Doc()->UpdateAllViews(NULL, TGU_Repaint, NULL);
      }
    m_iDragI=-1;
    HCURSOR h=ScdApp()->LoadStandardCursor(IDC_ARROW);
    //SetCursor(h);
    //SetRqdCursor(NULL);
    SetRqdCursor(h);
    }
  else if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
    {
    if (EI.Fld->FieldId==TV_TrndFldId)
      {
      const int iSltNo = EI.BlkRowNo+EI.Index;
      CTagVwDoc* pDoc = (CTagVwDoc*)GetDocument();
      if (pDoc->DS[iSltNo].TagValid() || pDoc->DS[iSltNo].HasFunct())
        pDoc->ToggleTrending(iSltNo);
      }
    else if (EI.Fld->FieldId==-1 && ChEditPos.x<=4)
      {
      CTagVwSlot * pSlt = DataSlot(EI.BlkRowNo+EI.Index);
      if (pSlt)
        {
        if (pSlt->bDriverTag)
          pSlt->ShowDriverTagInfo(Doc(), 0x0001);
        else if (pSlt->bArchiveTag)
          pSlt->ShowArchiveTagInfo(Doc(), 0x0001);
        else if (pSlt->TagValid())
          {
          m_sTag4Access[0]=pSlt->sTag();
          const int DotPos = m_sTag4Access[0].Find('.');
          if (DotPos>=0)
            m_sTag4Access[0]= m_sTag4Access[0].Left(DotPos);
          PostMessage(WM_COMMAND, IDM_TRND_FINDTAG_0, 0);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwText::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  FxdEdtView::OnRButtonUp(nFlags, point);
  CTagVwText::DoOnRButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

flag CTagVwText::DoOnRButtonUp(UINT nFlags, CPoint point) 
  {
  FxdEdtView::OnRButtonUp(nFlags, point);
  CPoint RP;
  Point2RowCols(ScrGB, point, RP);
  const int x = Range((int)0, (int)(RP.x), (int)(CPg->MaxCols-1));
  const int y = Range((int)0, (int)(RP.y), (int)(CPg->Rows-1));
  iMenuLine = y;

  int R1,R2;
  const int RowRange = CalcRowRange(R1, R2);
  flag OneSlot = (RowRange<1);
  flag DoMenu = True;
  pMenuSlt = NULL;
  CTagVwSlot * pSlt = NULL;
  CTagVwSlot * pCSlt = NULL;
  flag IsLCtrlFld = false;
  flag IsHCtrlFld = false;
  FxdEdtInfo EI;
  if (LocateFromCR(x, y, EI))
    {
    pSlt = DataSlot(SlotIndex(EI));//.BlkRowNo+EI.Index);
    IsLCtrlFld = (EI.FieldId>TV_LastTrndFldId && EI.FieldId<=TV_LastLCtrlFldId);
    IsHCtrlFld = (EI.FieldId>TV_LastLCtrlFldId && EI.FieldId<=TV_LastHCtrlFldId);
    FxdEdtInfo TmpEI=EI;
    TmpEI.FieldId=TV_LCtrlFldId;
    pCSlt = DataSlot(SlotIndex(TmpEI));//.BlkRowNo+EI.Index);
    }
  flag IsSlt = (pSlt!=NULL);
  flag IsTrnd = (pSlt && (pSlt->nTrendNo>=0));
  flag IsDriverTag = (pSlt && pSlt->bDriverTag);
  flag IsArchiveTag = (pSlt && pSlt->bArchiveTag);
  flag IsText = (pSlt && pSlt->sTag.Len()>0);
  flag IsComment = (pSlt && pSlt->sTag.Len()>0 && pSlt->sTag[0]=='\'');
  flag IsFunct = (pSlt && pSlt->sTag.Len()>0 && pSlt->sTag[0]=='=');
  flag IsFirstRow = (y==0);
  flag IsCtrlFld = (IsLCtrlFld || IsHCtrlFld);

  if (IsSlt && bShiftDown)
    {
    if (IsDriverTag)
      pSlt->ShowDriverTagInfo(Doc(), 0x0001);
    else if (IsArchiveTag)
      pSlt->ShowArchiveTagInfo(Doc(), 0x0001);
    //else
    //  pStatusBar->SetMsg1(""); //place temporary message on status bar
    DoMenu = False;
    }
  if (DoMenu && IsTrnd)
    {
    FxdEdtFld * Fld = CRow(EI.PgRowNo)->FldFromId(TV_TrndFldId);
    if (Fld && x>=Fld->Col1 && x<Fld->ColN)
      {
      DoMenu = False;
      pMenuSlt = pSlt;
      OnMenu(IDM_TRND_COLOR);
      }
    }
  if (DoMenu && IsText && !IsComment)
    pMenuSlt = pSlt;

  if (DoMenu)
    {
    CMenu Menu;
    CMenu FormatMenu;
    CMenu CnvMenu;
    CMenu SclMenu;
    CMenu RecordMenu;
    CMenu ShowHideMenu;
    CMenu OtherMenu;

    SclMenu.CreatePopupMenu();
    Menu.CreatePopupMenu();
    if (!IsSlt)//IsFirstRow || )
      {
      Menu.AppendMenu(MF_STRING, IDM_TRND_SCROLLEND, "Scroll to &End\tCtrl+E");
      Menu.AppendMenu(MF_STRING, IDM_TRND_RECONNECT, "&Reconnect tags");
      Menu.AppendMenu(MF_STRING, IDM_TRND_OPTIONS, "&Options...");
      Menu.AppendMenu(MF_STRING, IDM_TRND_QUERY, "&Query...");
      Menu.AppendMenu(MF_STRING, IDM_TRND_GLOBALTIMEBASE, "&Global timebase");
      Menu.AppendMenu(MF_STRING, IDM_TRND_FLASHIND, "&Trend Indicator");

      CMenu FitMenu;
      FitMenu.CreatePopupMenu();
      CTagVwDoc::CreateFitMenu(FitMenu, true, true);
      Menu.AppendMenu(MF_POPUP, (UINT)FitMenu.m_hMenu, "&Fit all trends");

      //      ?      Menu.AppendMenu(MF_STRING, IDM_TRND_FITALL, "&Fit all trends");
      if (CTagVwDoc::TimeBaseGlobal())
        Menu.CheckMenuItem(IDM_TRND_GLOBALTIMEBASE, MF_BYCOMMAND|MF_CHECKED);
      if (gs_pPrj->bFlashTrend)
        Menu.CheckMenuItem(IDM_TRND_FLASHIND, MF_BYCOMMAND|MF_CHECKED);
      }
    else
      {
      RecordMenu.CreatePopupMenu();
      RecordMenu.AppendMenu(MF_STRING, IDM_TRND_RECORDTAGON, "&On");
      RecordMenu.AppendMenu(MF_STRING, IDM_TRND_RECORDTAGOFF, "O&ff");
      ShowHideMenu.CreatePopupMenu();
      ShowHideMenu.AppendMenu(MF_STRING, IDM_TRND_TOGGLEON,  "&Show Trend Lines");
      ShowHideMenu.AppendMenu(MF_STRING, IDM_TRND_TOGGLEOFF, "&Hide Trend Lines");
      if (pMenuSlt)
        {
        CnvAttribute::SetCmdIndex(IDM_ACC_CNV1);
        FmtAttribute::SetCmdIndex(IDM_ACC_FMT1);
        Strng s;

        FormatMenu.CreatePopupMenu();
        pMenuSlt->Fmt.AddToMenu(FormatMenu);

        CnvMenu.CreatePopupMenu();
        pMenuSlt->Cnv.AddToMenu(CnvMenu);

        Menu.AppendMenu(MF_POPUP, (UINT)CnvMenu.m_hMenu, "&Conversions");
        Menu.AppendMenu(MF_POPUP, (UINT)FormatMenu.m_hMenu, "&Format");
        Menu.AppendMenu(MF_SEPARATOR);
        //Menu.AppendMenu(MF_STRING, IDM_TRND_EDIT, "&Edit...");

        //Strng sLastSelected;
        switch (EI.FieldId)
          {
          case TV_LSptFldId:
          case TV_LMeasFldId:
          case TV_LDiffFldId:
          case TV_LOutFldId:
          case TV_LDescFldId:
            m_sTag4Access[0]=pCSlt->sDesc();
            //sLastSelected=EI.Fld->sTag;
            break;
          default:
            m_sTag4Access[0]=pMenuSlt->sTag();
            break;
          }
        if (!IsFunct)
          {
          const int DotPos = m_sTag4Access[0].Find('.');
          if (DotPos>=0)
            m_sTag4Access[0]= m_sTag4Access[0].Left(DotPos);

          if (IsDriverTag)
            {
            s.Set("Driver info for %s", m_sTag4Access[0]);
            Menu.AppendMenu(MF_STRING, IDM_TRND_DRIVERINFO, s());
            Menu.AppendMenu(MF_STRING, IDM_TRND_DRIVERDLGINFO, "Dri&ver info dialog");
            Menu.AppendMenu(MF_STRING, IDM_TRND_DRIVERCOPTTAG, "Copy Driver Address/Tag");
            }
          if (IsArchiveTag)
            {
            //s.Set("Archive info for %s", m_sTag4Access());
            //Menu.AppendMenu(MF_STRING, IDM_TRND_ARCHIVEINFO, s());
            Menu.AppendMenu(MF_STRING, IDM_TRND_ARCHIVEDLGINFO, "Archive info dialog");
            }
          }
        if (!IsHCtrlFld && EI.FieldId!=TV_LDescFldId)
          {
          if (OneSlot)
            {
            Menu.AppendMenu(MF_STRING, IDM_TRND_RECORDTAG, "Record in &historian");
            if (pMenuSlt->bRecorded)
              Menu.CheckMenuItem(IDM_TRND_RECORDTAG, MF_BYCOMMAND|MF_CHECKED);
            if (IsDriverTag || /*IsArchiveTag ||*/ IsFunct)
              Menu.EnableMenuItem(IDM_TRND_RECORDTAG, MF_BYCOMMAND|MF_GRAYED);
            }
          else
            {
            Menu.AppendMenu(MF_POPUP, (UINT)RecordMenu.m_hMenu, "Record in &historian");
            }

          if (OneSlot)
            {
            Menu.AppendMenu(MF_STRING, IDM_TRND_TOGGLE,  "Show &Trend");
            if (IsTrnd)
              Menu.CheckMenuItem(IDM_TRND_TOGGLE, MF_BYCOMMAND|MF_CHECKED);
            }
          else
            {
            Menu.AppendMenu(MF_POPUP, (UINT)ShowHideMenu.m_hMenu, "Show/Hide &Trend");
            }

          if (OneSlot)
            {
            s.Set("Co&lour %s...", OneSlot ? "" : m_sTag4Access[0]);
            Menu.AppendMenu(MF_STRING, IDM_TRND_COLOR, s());
            }
          }
        if (!IsHCtrlFld)
          Menu.AppendMenu(MF_SEPARATOR);
        if ((pMenuSlt->Cnv.Index())==0)
          Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED); // Conversions
        if (!IsFloatData(pMenuSlt->cType))
          Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED); // Format
        }

      if (pMenuSlt==NULL)
        {
        if (!OneSlot)
          {
          Menu.AppendMenu(MF_POPUP, (UINT)ShowHideMenu.m_hMenu, "Show/Hide &Trend");
          Menu.AppendMenu(MF_POPUP, (UINT)RecordMenu.m_hMenu, "Record in &historian");
          Menu.AppendMenu(MF_SEPARATOR);
          }
        }
      if ((pMenuSlt || !OneSlot) && !IsHCtrlFld && EI.FieldId!=TV_LDescFldId)
        {
        Doc()->CreateScaleMenu(SclMenu, OneSlot ? pMenuSlt : NULL);
        Menu.AppendMenu(MF_POPUP, (UINT)SclMenu.m_hMenu, OneSlot ? "&Scale Trend" : "&Scale Trends");
        }
      OtherMenu.CreatePopupMenu();
      OtherMenu.AppendMenu(MF_STRING, IDM_TRND_SCROLLEND, "Scroll to &End\tCtrl+E");
      if (OneSlot)
        OtherMenu.AppendMenu(MF_STRING, IDM_TRND_SELECTTAG, "&Select Tag...");
      OtherMenu.AppendMenu(MF_STRING, IDM_TRND_RECONNECT, "&Reconnect tags");
      OtherMenu.AppendMenu(MF_STRING, IDM_TRND_OPTIONS, "&Options...");
      OtherMenu.AppendMenu(MF_STRING, IDM_TRND_QUERY, "&Query...");
      OtherMenu.AppendMenu(MF_STRING, IDM_TRND_GLOBALTIMEBASE, "&Global timebase");

      CMenu FitMenu;
      FitMenu.CreatePopupMenu();
      CTagVwDoc::CreateFitMenu(FitMenu, true, true);
      OtherMenu.AppendMenu(MF_POPUP, (UINT)FitMenu.m_hMenu, "&Fit all trends");

      //OtherMenu.AppendMenu(MF_STRING, IDM_TRND_FITALL, "&Fit all trends");

      if (CTagVwDoc::TimeBaseGlobal())
        OtherMenu.CheckMenuItem(IDM_TRND_GLOBALTIMEBASE, MF_BYCOMMAND|MF_CHECKED);
      if (gs_pPrj->bFlashTrend)
        OtherMenu.CheckMenuItem(IDM_TRND_FLASHIND, MF_BYCOMMAND|MF_CHECKED);
      Menu.AppendMenu(MF_POPUP, (UINT)OtherMenu.m_hMenu, "&Other");

      if (!IsCtrlFld)
        {
        Menu.AppendMenu(MF_SEPARATOR);
        Menu.AppendMenu(MF_STRING, IDM_TRND_CLEAR, "Clear");
        Menu.AppendMenu(MF_STRING, IDM_TRND_DELETE, "&Delete");
        Menu.AppendMenu(MF_STRING, IDM_TRND_INSERT, "&Insert");
        if (gs_Exec.Busy())
          {
          Menu.EnableMenuItem(IDM_TRND_DELETE, MF_BYCOMMAND|MF_GRAYED);
          Menu.EnableMenuItem(IDM_TRND_INSERT, MF_BYCOMMAND|MF_GRAYED);
          }
        else if (NoSlots()==0)
          Menu.EnableMenuItem(IDM_TRND_DELETE, MF_BYCOMMAND|MF_GRAYED);
        //else if (Doc()->DS[NoSlots()-1].sTag() || iMenuLine==NoSlots()-1)
        //  Menu.EnableMenuItem(IDM_TRND_INSERT, MF_BYCOMMAND|MF_GRAYED);
        }

      Menu.AppendMenu(MF_SEPARATOR);
      Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
      Menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");

      if (!IsFunct && pMenuSlt)
        {
#if WITHXREFUPDATES
        CXRefInfoArray Refs;
        CString S(pSlt->sTagOnly());
        if (S.GetLength()>0)
          {
          S.MakeLower();
          gs_pTheSFELib->RequestTagRefInfo(S, Refs);
          bool FirstOne=true;
          if (Refs.GetSize())
            {
            //Menu.AppendMenu(MF_SEPARATOR);
            bool HasConns=0;
            CMenu CnMenu;
            CnMenu.CreatePopupMenu();
            long iTag=1;
            for (int iPass=0; iPass<2; iPass++)
              {
              for (int ii=0; ii<Refs.GetSize(); ii++)
                {
                CTagRefInfoItem  *Ref = Refs[ii];
                CString s;
                if (Ref->m_fIsSetRef && iPass==0)
                  {
                  m_sTag4Access[iTag] = Ref->m_sRmtTag();
                  s.Format(Ref->m_iSetCnt ? "%s -->(Set)":"%s -->()", Ref->m_sRmtID());
                  }
                if (Ref->m_fIsGetRef && iPass==1)
                  {
                  m_sTag4Access[iTag] = Ref->m_sRmtTag();
                  s.Format(Ref->m_iGetCnt ? "(Get)--> %s":"()--> %s", Ref->m_sRmtID());
                  }

                if (s.GetLength()>0)
                  {
                  HasConns=1;

                  CMenu XMenu;
                  XMenu.CreatePopupMenu();
                  XMenu.AppendMenu(MF_STRING|MF_GRAYED, 0, m_sTag4Access[iTag]);
                  XMenu.AppendMenu(MF_SEPARATOR);
                  XMenu.AppendMenu(MF_STRING, IDM_TRND_ACCESSTAG_0+iTag, "&Access");
                  XMenu.AppendMenu(MF_STRING, IDM_TRND_FINDTAG_0+iTag,   "&Find");
                  Menu.AppendMenu(MF_POPUP|(FirstOne?MF_MENUBARBREAK:0), (unsigned int)XMenu.m_hMenu, s);
                  FirstOne=false;
                  iTag++;
                  }
                }
              }
            }
          }
          //Menu.AppendMenu(MF_POPUP|MF_MENUBARBREAK|(HasConns?0:MF_GRAYED), (unsigned int)CnMenu.m_hMenu, "Connects");
        #endif
        
        const int DotPos = m_sTag4Access[0].Find('.');
        if (DotPos>=0)
          m_sTag4Access[0]= m_sTag4Access[0].Left(DotPos);
        if (!IsDriverTag && !IsArchiveTag)
          {
          Strng s;
          if (OneSlot)
            {
            //if (FirstOne)
            Menu.AppendMenu(MF_SEPARATOR);

            CMenu XMenu;
            s=m_sTag4Access[0];
            XMenu.CreatePopupMenu();
            XMenu.AppendMenu(MF_STRING, IDM_TRND_ACCESSTAG_0, "Access\t...");
            XMenu.AppendMenu(MF_STRING, IDM_TRND_FINDTAG_0,   "Find\t...");
            Menu.AppendMenu(MF_POPUP, (unsigned int)XMenu.m_hMenu, s());
          //  s.Set("&Access %s...", m_sTag4Access[0]);
          //  Menu.AppendMenu(MF_STRING, IDM_TRND_ACCESSTAG_0, s());
          //  if (!IsHCtrlFld)
          //    {
          //    s.Set("Fi&nd %s...", m_sTag4Access[0]);
          //    Menu.AppendMenu(MF_STRING, IDM_TRND_FINDTAG_0, s());
          //    }
            }
          }
        }
      }

    CRect Rect;
    GetWindowRect(&Rect);
    MenuPt.x = Rect.left+point.x;
    MenuPt.y = Rect.top+point.y;
    Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, MenuPt.x, MenuPt.y, this);

    //FormatMenu.DestroyMenu(); // Built into Destructor
    Menu.DestroyMenu();
    }

  return true;
  }

//---------------------------------------------------------------------------

long CTagVwText::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (CurrentBlk(EI))
    {
    if (EI.FieldId==TV_ScrollToEnd)
      {
      PostMessage(WM_COMMAND, IDM_TRND_SCROLLEND, 0); //NB: Must POST !
      //OnMenu(IDM_TRND_SCROLLEND);
      }
    else if (EI.FieldId==TV_Reconect)
      {
      PostMessage(WM_COMMAND, IDM_TRND_RECONNECT, 0); //NB: Must POST !
      //OnMenu(IDM_TRND_RECONNECT);
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

void CTagVwText::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {                                                               
  flag OK=false;
  if (CPg)
    {
//    pFxdEdtFld Fld=EdtFld();
//    StartCharEdit(Fld);
//    flag Reposition=true;
    int PrevRow=-1;
    for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
      {
      if (pFld->pRow->RowNo>PrevRow)
        {
        int R=pFld->pRow->RowNo;
        CTagVwSlot * pSlot = DataSlot(R-1);
        if (pSlot && Valid(pSlot->dValue) && (nChar==VK_F11))
          {
          OK=true;
          double Val=pSlot->dValue;
          double Mn=atof(pSlot->sMinVal() ? pSlot->sMinVal() : "0");
          double Mx=atof(pSlot->sMaxVal() ? pSlot->sMaxVal() : "1");
          double Mid=0.5*(Mx+Mn);
          double Diff=0.5*(Mx-Mn);
          if (bControlDown)
            Mid=Val;
          else
            Diff*=(bShiftDown ? 2.0 : 0.5);
          Mn=Mid-Diff;
          Mx=Mid+Diff;

          int i;
          Strng S;
          S.Set("%.8g",Mn);
          i=S.Length()-1;
          while (i>1 && S[i]=='0')
            S[i--]=' ';
          S.LRTrim();
          pSlot->sMinVal=S;
          S.Set("%.8g",Mx);
          i=S.Length()-1;
          while (i>1 && S[i]=='0')
            S[i--]=' ';
          S.LRTrim();
          pSlot->sMaxVal=S;
          }
        PrevRow=R;
        }
      }

    }

  FxdEdtView::OnKeyDown(nChar, nRepCnt, nFlags);
  if (OK)
    {
    Doc()->UpdateAllViews(NULL, TGU_ClearTrendDisplay|TGU_NowActivated, NULL);
    Doc()->FixTimeBase();
    }
  }

//---------------------------------------------------------------------------
/*
LRESULT CTagVwText::OnDocUpdate(WPARAM wParam, LPARAM lParam)
  {
  CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
  pDoc->UpdateAllViews(NULL, wParam, NULL);
  return True;
  };
*/
//---------------------------------------------------------------------------

void CTagVwText::OnMenu(UINT id)
  {
  switch (id)
    {
    /*case IDM_TRND_EDIT:
      Doc()->TrendSlotsDlg(pMenuSlt->sTag());
      break;*/
    case IDM_TRND_CNV:
      {
      CnvAttribute::SetCmdIndex(IDM_ACC_CNV1);
      CMenu Menu;
      Menu.CreatePopupMenu();
      pMenuSlt->Cnv.AddToMenu(Menu);
      Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, MenuPt.x, MenuPt.y, this);
      Menu.DestroyMenu();
      break;
      }
    case IDM_TRND_FORMAT:
      {
      FmtAttribute::SetCmdIndex(IDM_ACC_FMT1);
      CMenu Menu;
      Menu.CreatePopupMenu();
      pMenuSlt->Fmt.AddToMenu(Menu);
      Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, MenuPt.x, MenuPt.y, this);
      Menu.DestroyMenu();
      break;
      }
    case IDM_TRND_CLEAR:
      {
      int R1,R2;
      CalcRowRange(R1, R2);
      Doc()->ClearLine(SlotIndex(R1, TV_TrndFldId), R2-R1+1);
      }
      break;
    case IDM_TRND_DELETE:
      {
      int R1,R2;
      CalcRowRange(R1, R2);
      Doc()->DeleteLine(SlotIndex(R1, TV_TrndFldId), R2-R1+1);
      Doc()->DS.ChangeSize(-(R2-R1+1));
      Doc()->DS.CheckSize(MinTrendSlots);
      DoRebuild();
      }
      break;
    case IDM_TRND_INSERT:
      {
      int R1,R2;
      CalcRowRange(R1, R2);
      Doc()->DS.ChangeSize((R2-R1+1));
      Doc()->InsertLine(SlotIndex(R1, TV_TrndFldId), R2-R1+1);
      Doc()->DS.CheckSize(MinTrendSlots);
      DoRebuild();
      }
      break;
    case IDM_TRND_COLOR:
      {
      CColorDialog ColDlg(pMenuSlt->lPen.lopnColor, CC_RGBINIT | CC_SHOWHELP, this);
      ColDlg.m_cc.lpCustColors = CustomColours;
      if (ColDlg.DoModal()==IDOK)
        {
        pMenuSlt->lPen.lopnColor = ColDlg.GetColor();
        Doc()->UpdateAllViews(NULL, TGU_Colour, NULL);
        }
      break;
      }
    case IDM_TRND_OPTIONS:
      {
      Doc()->TrendOptionsDlg();
      break;
      }
    case IDM_TRND_QUERY:
      {
      Doc()->TrendQueryDlg();
      break;
      }
    case IDM_TRND_GLOBALTIMEBASE:
      {
      CTagVwDoc::DoSetTimeGlobal(!CTagVwDoc::TimeBaseGlobal());
      break;
      }
    case IDM_TRND_SCROLLEND:
      {
      Doc()->AdjustTimebase( 1.0e38, 1.0, 1, 1);
      break;
      }
    case IDM_TRND_FLASHIND:
      {
      gs_pPrj->bFlashTrend=!gs_pPrj->bFlashTrend;
      break;
      }
    //case IDM_TRND_FITALL:
    case IDM_TRNDSCLALL_FITMIN:
    case IDM_TRNDSCLALL_FIT1:
    case IDM_TRNDSCLALL_FIT10:
    case IDM_TRNDSCLALL_FIT50:
    case IDM_TRNDSCLALL_FITZMIN:
    case IDM_TRNDSCLALL_FITZ1:
    case IDM_TRNDSCLALL_FITZ10:
    case IDM_TRNDSCLALL_FITZ50:
    case IDM_TRNDSCLALL_FITDEFAULT:
    case IDM_TRNDSCLALL_FITAUTOON:
    case IDM_TRNDSCLALL_FITAUTOOFF:
      {
      Doc()->OnTrendFitAll(id);
      break;
      }
    case IDM_TRND_RECONNECT:
      {
      Doc()->Rebuild();
      break;
      }
    case IDM_TRND_SELECTTAG:
      {
      CSelectTagDlg Dlg(pMenuSlt ? pMenuSlt->sTag() : "", SelFull, 1, this);
      if (Dlg.DoModal()==IDOK && Dlg.sTag.GetLength()>0)
        {
        if (pMenuSlt)
          {
          if (pMenuSlt->ChangeTag((char*)(const char*)Dlg.sTag, Doc(), pMenuSlt->lPen.lopnColor))
            {
            Doc()->UpdateAllViews(NULL, TGU_Toggle, NULL);
            DoRebuild();
            }
          }
        else
          {
          CTagVwSlot * Slt = DataSlot(iMenuLine-1);
          if (Slt && Slt->ChangeTag((char*)(const char*)Dlg.sTag, Doc(), DefTrndColours[(iMenuLine-1) % MaxTrndColours]))
            {
            Doc()->UpdateAllViews(NULL, TGU_Toggle, NULL);
            DoRebuild();
            }
          }
        }
      break;
      }

    case IDM_TRND_RECORDTAG:
    case IDM_TRND_RECORDTAGON:
    case IDM_TRND_RECORDTAGOFF:
      {
      int OK=false;
      int S1=-1;
      for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
        {
        const int iSltNo = SlotIndex(pFld);
        if (iSltNo!=S1)
          {
          S1=iSltNo;
          if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
            {
            CTagVwSlot * pSlt = DataSlot(iSltNo);
            if (id==IDM_TRND_RECORDTAG || (id==IDM_TRND_RECORDTAGON == pSlt->bRecorded==0))
              if (pSlt->RecordTag(Doc(), !pSlt->bRecorded))
                OK=true;
            }
          }
        }
      if (OK)
        Doc()->UpdateAllViews(NULL, TGU_TagView, NULL);
      }
      break;
    case IDM_TRND_DRIVERINFO:
      pMenuSlt->ShowDriverTagInfo(Doc(), 0x0001);
      break;
    case IDM_TRND_DRIVERDLGINFO:
      pMenuSlt->ShowDriverTagInfo(Doc(), 0x0001 | 0x0004);
      break;
    case IDM_TRND_DRIVERCOPTTAG:
      pMenuSlt->ShowDriverTagInfo(Doc(), 0x0001 | 0x0008);
      break;
    case IDM_TRND_ARCHIVEINFO:
      pMenuSlt->ShowArchiveTagInfo(Doc(), 0x0001);
      break;
    case IDM_TRND_ARCHIVEDLGINFO:
      pMenuSlt->ShowArchiveTagInfo(Doc(), 0x0001 | 0x0004);
      break;


    case IDM_TRND_TOGGLE:
    case IDM_TRND_TOGGLEON:
    case IDM_TRND_TOGGLEOFF:
      {
//      CArray<int, int> SltNoLst;
      CIArray SltNoLst;
      SltNoLst.SetSize(0,128);
      int OK=false;
      //int S1=-1;
      for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
        {
        int iSltNo = SlotIndex(pFld);
          if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
            {
            CTagVwSlot * pSlt = DataSlot(iSltNo);
            if (id==IDM_TRND_TOGGLE || (id==IDM_TRND_TOGGLEON == pSlt->nTrendNo<0))
              if (pSlt->TagValid() || pSlt->HasFunct())
                SltNoLst.Add(iSltNo);
            }
        }
      if (id==IDM_TRND_TOGGLE)
        ((CTagVwDoc*)GetDocument())->ToggleTrending(SltNoLst);
      else
        ((CTagVwDoc*)GetDocument())->SetTrending(SltNoLst, id==IDM_TRND_TOGGLEON);
      }
      break;
    default:
      {
      if (id>=IDM_TRND_ACCESSTAG_0 && id<IDM_TRND_FINDTAG_0+MaxTagCmds)
        {
        int idx=(id % MaxTagCmds);
        if (m_sTag4Access[idx].GetLength()>0)
          {
          char* pTxt = new char[m_sTag4Access[idx].GetLength()+1];
          strcpy(pTxt, m_sTag4Access[idx]);
          AfxGetMainWnd()->PostMessage(WMU_TAGACTION, (id<IDM_TRND_FINDTAG_0) ? SUB_TAGACTION_ACCESS : SUB_TAGACTION_FIND, (LPARAM)pTxt);
          }
        }
      else
        {
        int OK=false;
        int S1=-1;
        if (id>=IDM_TRNDSCL_FIRST && id<=IDM_TRNDSCL_LAST)
          {
          for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
            {
            const int iSltNo = SlotIndex(pFld);
            if (iSltNo!=S1)
              {
              S1=iSltNo;
              if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
                {
                CTagVwSlot * pSlt = DataSlot(iSltNo);
                if (Doc()->ProcessScaleMenu(id, pSlt, pSlt->dValue))
                  OK=true;
                }
              }
            }
          if (OK)
            {
            Doc()->UpdateAllViews(NULL, TGU_ClearTrendDisplay|TGU_NowActivated, NULL);
            Doc()->FixTimeBase();
            }
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwText::OnCnv(UINT id)
  {
  ASSERT(pMenuSlt && (pMenuSlt->Cnv.Index())!=0);
  pCDataCnv pC = Cnvs[(pMenuSlt->Cnv.Index())];
  for (int i=id-IDM_ACC_CNV1; i>0; i--)
    pC = pC->Next();
  if (pC && strcmp(pC->Txt(), pMenuSlt->Cnv.Text())!=0)
    {//conversion changed...
    int S1=-1;
    for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
      {
      int iSltNo=SlotIndex(pFld);
      if (iSltNo!=S1)
        {
        S1=iSltNo;
        if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
          {
          CTagVwSlot * pSlt = DataSlot(iSltNo);
          pSlt->ChangeCnv(pC->Txt());
          }
        }
      }
    Doc()->UpdateAllViews(NULL, TGU_TagView, NULL);
    }
  }

//---------------------------------------------------------------------------

void CTagVwText::OnFormat(UINT id)
  {
  ASSERT(pMenuSlt);
  for (UINT i=0; DefinedFmts.GetSize(); i++) 
    if (i==id-IDM_ACC_FMT1) 
      break;

  if (DefinedFmts[i].Name() && _stricmp(DefinedFmts[i].Name(), pMenuSlt->Fmt.Name())!=0)
    {//format has changed...
    int S1=-1;
    for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
      {
      int iSltNo=SlotIndex(pFld);
      if (iSltNo!=S1)
        {
        S1=iSltNo;
        if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
          {
          CTagVwSlot * pSlt = DataSlot(iSltNo);
          pSlt->Fmt = DefinedFmts[i];
          if (IsNumData(pSlt->cType))
            {
            Strng s = pSlt->sValue;
            s.Trim();
            if (s.Len()>0 && _stricmp("*", s())!=0)
              pSlt->Fmt.FormatFloat(atof(s()), pSlt->sValue, pSlt->sStrList.Length()>0 ? &pSlt->sStrList:NULL);
            }
          }
        }
      }
//    pMenuSlt->Fmt = DefinedFmts[i];
//    if (IsNumData(pMenuSlt->cType))
//      {
//      Strng s = pMenuSlt->sValue;
//      s.Trim();
//      if (s.Length()>0 && _stricmp("*", s())!=0)
//        {
//        double d = atof(s());
//        pMenuSlt->Fmt.FormatFloat(d, pMenuSlt->sValue);
//        }
//      }
    Doc()->UpdateAllViews(NULL, TGU_TagView, NULL);
    }
  }

//---------------------------------------------------------------------------

LRESULT CTagVwText::OnUpdateByMsg(WPARAM wParam, LPARAM lParam)
  {
  //CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
  //pDoc->UpdateAllViews(NULL, wParam, NULL);
  OnUpdate(NULL, wParam, (CObject*)lParam);
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CTagVwText::OnAddTag2Trend(WPARAM wParam, LPARAM lParam)
  {
  LPTSTR pStr=(LPTSTR)lParam;

  dbgpln("Add Tag : %s", pStr);

  int GotOne=false;
  int iSltNo=m_AddTagSlt;
  for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
    {
    iSltNo=SlotIndex(pFld);
    if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
      {
      if (DataSlot(iSltNo)->Tag()==NULL)
        {
        GotOne=true;
        break;
        }
      }
    }

  if (!GotOne)
    {
    for (iSltNo++; iSltNo<NoSlots(); iSltNo++)
      {
      if (iSltNo>=0 && iSltNo<NoSlots())//How or Why is this sometimes wrong !!!!!
        {
        if (DataSlot(iSltNo)->Tag()==NULL)
          {
          GotOne=true;
          break;
          }
        }
      }
    }

  if (GotOne)
    {
    m_AddTagSlt=iSltNo;
    CTagVwSlot * pSlt = DataSlot(iSltNo);
    pSlt->ChangeTag(pStr, Doc(), DefTrndColours[(iSltNo) % MaxTrndColours]);

    //if (!EI.fPasteBusy)
    Doc()->UpdateAllViews(NULL, TGU_Toggle, NULL);
    DoRebuild();
    //CRow(EI.PgRowNo)->Invalidate(ScrGB);
    ClrFldsChanged();
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

const int iInActiveImg=0;
const int iActiveImg=1;
const int iManualImg=2;
const int iHoldImg=3;
const int iTuneRqstOffImg=4;
const int iTuneRqstImg=5;
const int iTuneBusy1Img=7;
const int iTuneBusy2Img=6;
const int iViewOffImg=10;
const int iViewOnImg=11;

const int iImgW=12;
const int iImgH=12;

IMPLEMENT_DYNCREATE(CTagVwTextC, CTagVwText)

//---------------------------------------------------------------------------

CTagVwTextC::CTagVwTextC()
  {
  iMarginWidth = 4;
  m_iMenuSlot=-1;
  //m_NLoopRows=0;
  m_TrndRow1=0;
  m_LoopRow1=0;
  //iViewControl=-1;
  }

//---------------------------------------------------------------------------

CTagVwTextC::~CTagVwTextC()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTagVwTextC, CTagVwText)
  //{{AFX_MSG_MAP(CTagVwTextC)
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONUP()
  ON_WM_KEYDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
//  ON_MESSAGE(WMU_ONUPDATE, OnUpdateByMsg)
//  ON_COMMAND_RANGE(IDM_TRND_EDIT, IDM_TRND_SCROLLEND, OnMenu)
  ON_COMMAND_RANGE(IDM_CTRL_VIEW, IDM_CTRL_TUNE, OnMenuTune)
//  ON_COMMAND_RANGE(IDM_TRNDSCL_FIRST, IDM_TRNDSCL_LAST, OnMenu)
//  ON_COMMAND_RANGE(IDM_ACC_CNV1, IDM_ACC_CNVN, OnCnv)
//  ON_COMMAND_RANGE(IDM_ACC_FMT1, IDM_ACC_FMTN, OnFormat)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CTagVwTextC::OnInitialUpdate()
  {
  CString Title = GetDocument()->GetTitle();
  gs_pPrj->RestoreOneWindow((char*)(const char*)Title, GetParent(), false);
  CtrlBitmap.LoadBitmap(IDB_CTRLSTATES);
  MaskBitmap.LoadBitmap(IDB_CTRLMASKS);
  FxdEdtView::OnInitialUpdate();
  HCURSOR h=ScdApp()->LoadStandardCursor(IDC_ARROW);
  SetRqdCursor(h);
  }

//---------------------------------------------------------------------------

int CTagVwTextC::SlotIndex(int Row, int Id)
  {
  int Index=-1;
  if (Id<TV_FirstTrndFldId)
    {
    }
  else if (Id<TV_LastTrndFldId)
    {
    Index=(Row-m_TrndRow1)+LoopSlotOne()+LoopSlotCount();
    }
  else if (Id<TV_LastLCtrlFldId)
    {
    int Off=-1;
    switch (Id)
      {
      case TV_LCtrlFldId:
      case TV_LDescFldId:
        Off=0;
        break;
      case TV_LSptFldId:
      case TV_LTrndFld1Id:
        Off=1;
        break;
      case TV_LMeasFldId:
      case TV_LTrndFld2Id:
        Off=2;
        break;
      case TV_LOutFldId:
      case TV_LTrndFld3Id:
        Off=3; 
        break;
      //default:
      //  Index=-1;
      }
    if (Off>=0)
      Index=(Row-m_LoopRow1)*SlotsPerCtrl+Off+LoopSlotOne();
    }
  else // PID Stuff
    {
    Index=Id-TV_AutoTune;
    }

  return Index;
  }

//---------------------------------------------------------------------------

int CTagVwTextC::SlotIndex(FxdEdtInfo &EI) { return SlotIndex(EI.BlkRowNo, EI.FieldId); }
int CTagVwTextC::SlotIndex(pFxdEdtFld pFld) { return SlotIndex(pFld->pRow->RowNo, pFld->FieldId); }

//---------------------------------------------------------------------------

void CTagVwTextC::OnDrawLine(rGDIBlk GB, int PgNo, int RowNo)
  {
  if (CRow(RowNo)->Visible(GB) || GB.DC().m_bPrinting==1)
    {
    CRow(RowNo)->Write(GB);
    FxdEdtInfo EI;
    if (LocateFromCR(0,RowNo, EI))
      {
      //dbgpln("Draw Trace %i.3",RowNo);
      pFxdEdtFld CFld = CRow(EI.PgRowNo)->FldFromId(TV_LCtrlFldId);
      
      CDC &DC = GB.DC();
      CDCResChk ResChk(DC);
      CDC memdc;
      memdc.CreateCompatibleDC(&DC);
      CDCResChk ResChkMem(memdc);
      
      CPen* OldPen = NULL;
      CBrush* OldBrush = NULL;
      COLORREF OldBk;
      CBitmap* poldbmp = NULL;
      
      if (CFld)// && (Slt->bRecorded || Slt->nTrendNo>=0))
        {
        const int cx1=CFld->Xs(GB);
        const int cx2=CFld->Xe(GB);
        const int cxm=(cx1+cx2)/2;
        const int cy1=CFld->Ys(GB);
        const int cy2=CFld->Ye(GB)+1;

        OldPen = DC.SelectObject(GB.pPenTxtBord);
        OldBk = DC.SetBkColor(GB.crGrfBack);

        //EI.FieldId=TV_TIds[it];
        int iCSlt = SlotIndex(EI.BlkRowNo, TV_LCtrlFldId);
        CTagVwSlot * CSlt = DataSlot(iCSlt);
        if (CSlt)
          {
          //clear background
          CRect Rct(cx1, cy1, cx2, cy2);
          DC.FillRect(Rct, GB.pBrushTxtBack);
          //if (!GB.DC().m_bPrinting && (EI.BlkRowNo==(Doc()->ViewControl()+1)))
          //  DC.DrawEdge(Rct, EDGE_ETCHED, BF_RECT);

          int iStateImg=0;
          dword State=(dword)CSlt->sValue.SafeAtoL();//.atol()
          //if ((State & (CSW_On|CSW_AutoMan))!=(CSW_On|CSW_AutoMan))
          if (!(State & CSW_On))
            iStateImg=-1;    // Off
          else if (!(State & CSW_AutoMan))
            iStateImg=iManualImg;    //Inactive
          else if (!(State & CSW_Active))
            iStateImg=iInActiveImg;    //Inactive
          else
            iStateImg=iActiveImg;    //Active - in control

#define MASKBLT(x, ImgNo) \
          DC.MaskBlt(cx1+x, cy1+1, iImgW, iImgH, &memdc, iImgW*ImgNo, 0, \
                     MaskBitmap,  iImgW*ImgNo, 0, MAKEROP4(SRCCOPY, SRCAND))

          if (iStateImg>=0)
            {
            poldbmp = memdc.SelectObject(&CtrlBitmap);
            if (EI.BlkRowNo==(Doc()->ViewControl()+m_LoopRow1))
              MASKBLT(0, iViewOnImg);
            else 
              MASKBLT(0, iViewOffImg);

            MASKBLT(9, iStateImg);
          
            if (iStateImg>=0 && iStateImg!=iManualImg && (State & CSW_Hold))
              MASKBLT(17, iHoldImg);
          
            if (State & CSW_TuneBusy1)
              MASKBLT(25, iTuneBusy1Img);
            else if (State & CSW_TuneBusy2)
              MASKBLT(25, iTuneBusy2Img);
            else if (State & CSW_TuneRqst)
              MASKBLT(25, iTuneRqstImg);
            else if (iStateImg==iActiveImg)
              MASKBLT(25, iTuneRqstOffImg);
            }

#undef MASKBLT
          }
        }

      for (int it=0; it<4; it++)
        {
        static int TIds[]={TV_TrndFldId, TV_LTrndFld1Id, TV_LTrndFld2Id, TV_LTrndFld3Id};
        pFxdEdtFld TFld = CRow(EI.PgRowNo)->FldFromId(TIds[it]);
        if (!TFld)
          continue;
        EI.FieldId=TIds[it];
        CTagVwSlot * Slt = DataSlot(SlotIndex(EI));
        if (Slt && (Slt->TagValid() || Slt->HasFunct()))
          {
          const int tx1=TFld->Xs(GB);
          const int tx2=TFld->Xe(GB);
          const int ty1=TFld->Ys(GB);
          const int ty2=TFld->Ye(GB);
          const int tym=(ty2+ty1)/2;

          const flag TrndOn = (Slt->nTrendNo>=0);
          //CDC &DC = GB.DC();
          //CDCResChk ResChk(DC);

          if (!GB.DC().m_bPrinting)
            {
            //draw trend 'push' button...
            CRect TrndR(tx1, ty1, tx2, ty2);
            CBrush TrndBackBrush(DC.GetNearestColor(Doc()->BackGroundColour));
            CBrush * pRqdPrush= TrndOn ? &TrndBackBrush : GB.pBrushTxtBack;
            if (OldBrush)
              DC.SelectObject(pRqdPrush);
            else
              OldBrush = DC.SelectObject(pRqdPrush);
            DC.Rectangle(TrndR);
            }

          if (!GB.DC().m_bPrinting || TrndOn)
            {
            COLORREF Colr = (TrndOn && (!GB.DC().m_bPrinting || bUseColorPrint)) ? Slt->lPen.lopnColor : RGB(128,128,128);
            CPen Pen(PS_SOLID, Slt->dwWidth, Colr);
            CPen* pOldPen1 = DC.SelectObject(&Pen);
            DC.MoveTo(tx1+1, ty2-2);    
            DC.LineTo((2*tx1+(tx2-1))/3, ty1+2);
            DC.LineTo((tx1+(tx2-1))/2+1, tym);    
            DC.LineTo(tx2-1, tym);
            if (TrndOn && Slt->bAutoScaleOn)
              {
              DC.MoveTo(tx2-3, ty1+1);    
              DC.LineTo(tx2-2, ty1+1);    
              DC.LineTo(tx2-2, ty2-2);    
              if (0)
                {
                DC.LineTo(tx2-3, ty2-2);
                DC.LineTo(tx2-3, ty1+1);
                }
              else
                DC.LineTo(tx2-4, ty2-2);
              }            
            DC.SelectObject(pOldPen1);
            }

          if (Slt->bRecorded)
            {
            const int ElipSz = (GB.DC().m_bPrinting ? 28 : 7);
            CRect TrndR(tx2, ty1+4, tx2+ElipSz, ty1+ElipSz+4);
            CBrush Brush(RGB(255, 255, 0));
            if (OldBrush)
              DC.SelectObject(&Brush);
            else
              OldBrush = DC.SelectObject(&Brush);
            DC.Ellipse(TrndR);
            }
          }
        }

      if (OldPen)
        {
        DC.SelectObject(OldPen);
        DC.SetBkColor(OldBk);
        }
      if (OldBrush)
        DC.SelectObject(OldBrush);
      if (poldbmp)
        memdc.SelectObject(poldbmp);
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwTextC::Build(int NRows, int NCols, int ColIndent, pvoid BuildData)
  {
  if (NRows<=0)
    NRows = NoSlots()+1;

  nValFlds=0;

  CXM_Route DRoute;

  iMarginWidth = 6;
  Strng MrgStr;
  MrgStr.Set("%*s", iMarginWidth, "");
  int MeasWdt=MeasDspWdtFn();
  int MinWdt=MinDspWdtFn();
  int MaxWdt=MaxDspWdtFn();
  int r;
  /*
  for (r=1; r<=NRows-1; r++)
    if (!IsStrng(Doc()->DS[r-1].cType))
      {
      MeasWdt=Max(MeasWdt, Doc()->DS[r-1].sValue.Length());
      MinWdt=Max(MinWdt, Doc()->DS[r-1].sMinVal.Length());
      MaxWdt=Max(MaxWdt, Doc()->DS[r-1].sMaxVal.Length());
      }
  */
  
  int ComWide=23;
  StartBlk(NRows, 0L+NUsed-1, NULL);
  NUsed+=(NRows-1);
  
  r=0;
  ComWide=16;
  
  int iSlt=0;
  SetSpace(r, ComWide);
  r++;
  SetDesc(r, "AutoTune",   -1, 12, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetCheckBox(r, "",   TV_AutoTune, 2, 0, " "));
  SetSpace(r, ComWide-(12+1+2+1));
  r++;
  SetDesc(r, "TuneWithHold",   -1, 12, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetCheckBox(r, "",   TV_TuneWithHold, 2, 0, " "));
  SetSpace(r, ComWide-(12+1+2+1));
  r++;
  SetDesc(r, "HoldAll",   -1, 12, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetCheckBox(r, "",   TV_HoldAll, 2, 0, " "));
  SetSpace(r, ComWide-(12+1+2+1));
  r++;
  SetSpace(r, ComWide);
  r++;

  r=0;
  ComWide=17;
//  SetSpace(r, ComWide);
//  r++;
  SetDesc(r, "Plant:", -1, 7, 0, " ");
  SetSpace(r, ComWide-(7+1));
  r++;
  SetSpace(r, 1);
  SetDesc(r, "Gain", -1, 6, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PlantGain, 8, 2, " "));
  SetSpace(r, ComWide-(1+6+1+8+1));
  r++;
  SetSpace(r, 1);
  SetDesc(r, "Delay", -1, 6, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PlantDelay, 8, 2, " "));
  SetSpace(r, ComWide-(1+6+1+8+1));
  r++;
  SetSpace(r, 1);
  SetDesc(r, "Period", -1, 6, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PlantPeriod, 8, 2, " "));
  SetSpace(r, ComWide-(1+6+1+8+1));
  r++;
  SetSpace(r, ComWide);
  r++;

  r=0;
  ComWide=17;
  SetDesc(r, "PID:", -1, 7, 0, " ");
  SetSpace(r, ComWide-(7+1));
  r++;
  SetSpace(r, 1);
  SetDesc(r, "Gain", -1, 6, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PIDGain, 8, 2, " "));
  SetSpace(r, ComWide-(1+6+1+8+1));
  r++;
  SetSpace(r, 1);
  SetDesc(r, "Ic", -1, 6, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PIDIc, 8, 2, " "));
  SetSpace(r, ComWide-(1+6+1+8+1));
  r++;
  SetSpace(r, 1);
  SetDesc(r, "Dc", -1, 6, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PIDDc, 8, 2, " "));
  SetSpace(r, ComWide-(1+6+1+8+1));
  r++;
  SetSpace(r, ComWide);
  r++;

  r=0;
  ComWide=23;
  SetSpace(r, ComWide);
  r++;
  SetDesc(r, "Settle Min", -1, 15, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_SettleMin, 5, 2, " "));
  SetSpace(r, ComWide-(15+1+5+1));
  r++;
  SetDesc(r, "Settle Max", -1, 15, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_SettleMax, 5, 2, " "));
  SetSpace(r, ComWide-(15+1+5+1));
  r++;
  SetDesc(r, "TestDelta", -1, 12, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_TestDelta, 8, 2, " "));
  SetSpace(r, ComWide-(12+1+8+1));
  r++;
  SetDesc(r, "PBDeRate", -1, 12, 0, " ");
  ValFlds.SetAtGrow(nValFlds++, SetParm(r,"", TV_PBDeRate, 8, 2, " "));
  SetSpace(r, ComWide-(12+1+8+1));
  r++;
//  SetDesc(r, "RetuneOutDelta", -1, 12, 0, " ");
//  SetParm(r,"", TV_TagFldWidthId, 8, 2, " ");
//  SetSpace(r, ComWide-(12+1+8+1));
//  r++;

  r=6;
  ComWide=0;
  SetDesc(r,"", -1, ComWide, 0, "");
  
  SetDesc(r,"State", -1, 5, 0, "");
  SetDesc(r,"", -1, 2, 0, " ");
  SetDesc(r,"Setpoint", -1, MeasWdt, 0, " ");
  SetDesc(r,"", -1, 2, 0, " ");
  SetDesc(r,"Measure", -1, MeasWdt, 0, " ");
  SetDesc(r,"", -1, 2, 0, " ");
  SetDesc(r,"Output", -1, MeasWdt, 0, " ");
  SetDesc(r,"Loop", -1, 4, 0, " ");
  r++;

  int MinLoops=1; //15;
  int NLoops=LoopSlotCount()/SlotsPerCtrl;
  int NLoopRows=Max(MinLoops, NLoops);

//  for (int l=MinLoops; l<NLoopRows; l++)
//    SetSpace(r++, ComWide);
  m_LoopRow1=r;
  //for (l=0; l<NLoops; l++)
  for (int l=0; l<NLoops; l++)
    {
    ValFlds.SetAtGrow(nValFlds++, SetDesc(r,"", TV_LCtrlFldId, 5, 0, ""));
    SetDesc(r,"", TV_LTrndFld1Id, 2, 0, " ");
    ValFlds.SetAtGrow(nValFlds++, SetData(r,"", TV_LSptFldId, MeasWdt, 2, " "));
    SetDesc(r,"", TV_LTrndFld2Id, 2, 0, " ");
    ValFlds.SetAtGrow(nValFlds++, SetData(r,"", TV_LMeasFldId, MeasWdt, 2, " "));
    SetDesc(r,"", TV_LTrndFld3Id, 2, 0, " ");
    ValFlds.SetAtGrow(nValFlds++, SetData(r,"", TV_LOutFldId, MeasWdt, 2, " "));
    SetParm(r,"", TV_LDescFldId, 40, 0, "");
    r++;
    }

  for ( ; l<NLoopRows; l++)
    {
    SetDesc(r,"", TV_DummyId, 5, 0, "");
    SetDesc(r,"", TV_DummyId, 2, 0, " ");
    SetData(r,"", TV_DummyId, MeasWdt, 2, " ");
    SetDesc(r,"", TV_DummyId, 2, 0, " ");
    SetData(r,"", TV_DummyId, MeasWdt, 2, " ");
    SetDesc(r,"", TV_DummyId, 2, 0, " ");
    SetData(r,"", TV_DummyId, MeasWdt, 2, " ");
    SetParm(r,"", TV_DummyId, 40, 0, "");
    r++;
    }
  
  SetSpace(r++);
  SetDesc(r, "", -1, 2, 0, iMarginWidth==1 ? " " : "  ");
  SetDesc(r, "", -1, ColIndent, 0, "");
  #if WithTagFldWidth
  SetDesc(r, "Tag", -1, TagDspWdtFn()-4, 0, " ");
  SetParm(r, "", TV_TagFldWidthId, 3, 0, " ");
  #else
  SetDesc(r, "Tag", -1, TagDspWdtFn()-6, 0, " ");
  if (GDIBlk::LowRes())
    {
    SetBitmapButton1(r, IDB_TRNDREFRESH_LOWRES, TV_Reconect, 2, 0, " ");
    SetBitmapButton1(r, IDB_TRNDSCROLLEND_LOWRES, TV_ScrollToEnd, 2, 0, " ");
    }
  else
    {
    SetBitmapButton2(r, IDB_TRNDREFRESH, TV_Reconect, 2, 0, " ");
    SetBitmapButton2(r, IDB_TRNDSCROLLEND, TV_ScrollToEnd, 2, 0, " ");
    }
  #endif
  SetDesc(r, "Meas", -1, MeasWdt, 2, " ");
  SetDesc(r, "Min", -1, MinWdt, 2, " ");
  SetDesc(r, "Max", -1, MaxWdt, 2, " ");
  SetDesc(r, "Description", -1, 60, 0, "");
  r++;

  m_TrndRow1=r;

  for (int i=0; i<nValFlds; i++)
    {
    MinFlds.SetAtGrow(i, NULL);
    MaxFlds.SetAtGrow(i, NULL);
    }

  for (int s=LoopSlotOne()+LoopSlotCount(); s<NRows-1; s++)
    {
    pchar RowDesc="";
    if (BuildData)
      RowDesc=((pchar*)(BuildData))[s];
    CTagVwSlot * Slt=DataSlot(s);
       
    SetDesc(r,"", TV_TrndFldId, 2, 0, iMarginWidth==1 ? " " : "  ");
    SetDesc(r,RowDesc, -1 , ColIndent, 0, "");
    SetParm(r,"", TV_TagFldId, TagDspWdtFn(), 0, " ");
    if (Slt->sTag.Length()>=TagDspWdtFn() || (Slt->sTag.Length()>0 && Slt->sTag[0]=='='))
      SetBigStrLength((Slt->sTag.Length()>0 && Slt->sTag[0]=='=') ? 2048 : 128);
    if (IsStrng(Slt->cType))
      {
      ValFlds.SetAtGrow(nValFlds, SetData(r,"", TV_MeasFldId, MeasWdt+MinWdt+MaxWdt+2, 0, " "));
      MinFlds.SetAtGrow(nValFlds, NULL);
      MaxFlds.SetAtGrow(nValFlds, NULL);
      }
    else
      {
      byte Just=/*Slt->sStrList.Length()>0 ? 0 :*/ 2;
      ValFlds.SetAtGrow(nValFlds, SetData(r,"", TV_MeasFldId, MeasWdt, Just, " "));
      MinFlds.SetAtGrow(nValFlds, SetParm(r,"", TV_MinFldId, MinWdt, Just, " "));
      MaxFlds.SetAtGrow(nValFlds, SetParm(r,"", TV_MaxFldId, MaxWdt, Just, " "));
      }
    nValFlds++;
    SetParm(r,"", TV_DescFldId, 60, 0, "");
    r++;
    }
  
  FxdEdtView::Build(0, 0, 0);
  }

//---------------------------------------------------------------------------

void CTagVwTextC::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    char TFWBuff[256];
    //TRACE1("Load DataSlot %3i", RowId+Index1-1);
    int iSlt=SlotIndex(EI);

    CTagVwSlot * Slt=DataSlot(iSlt);
    int iFLS=Doc()->LoopSlotOne();
    int iSlt0=SlotsPerCtrl*((iSlt-iFLS)/SlotsPerCtrl)+iFLS;
    CTagVwSlot * Slt0=iSlt0>=0 ? DataSlot(iSlt0) : NULL;
    pchar p="";

    if (EI.FieldId==TV_DummyId)
      {
      EI.Fld->fEditable=false;
      }
    else if (EI.FieldId==TV_TagFldWidthId)
      {
      sprintf(TFWBuff, "%i", TagDspWdtFn()); 
      p=TFWBuff; 
      }
    else if (Slt)
      {
      
      if (Slt->TagValid() || Slt->HasFunct())
        {
        switch (EI.FieldId)
          {
          case TV_TagFldId: p=Slt->Tag(); break; //EI.Fld->fEditable=false; break;
          case TV_TagFldWidthId: 
            {
            sprintf(TFWBuff, "%i", TagDspWdtFn()); 
            p=TFWBuff; 
            break;
            }
          //case TV_DirnFldId: p=Slt->sDirn(); break;
          //case TV_RawFldId: p=Slt->sRaw(); break;
          case TV_MeasFldId: 
            EI.Fld->SetMarks(Slt->m_XRefStatus);
          case TV_LSptFldId: 
          case TV_LOutFldId:
            p=Slt->sValue();
            if (XStopped())
              EI.Fld->fEditable=Slt->m_bEditableStopped; 
            else
              EI.Fld->fEditable=Slt->m_bEditableRunning; 
            EI.Fld->fIsResult=Slt->bIsResult;
            break;
          case TV_LMeasFldId: 
            p=Slt->sValue();
            EI.Fld->fEditable=false;
            EI.Fld->fIsResult=Slt->bIsResult;
            break;
          case TV_MinFldId:
            p=Slt->sMinVal();
            EI.Fld->fEditable=!IsStrng(Slt->cType);
            EI.Fld->fIsResult=false;
            break;
          case TV_MaxFldId:
            p=Slt->sMaxVal();
            EI.Fld->fEditable=!IsStrng(Slt->cType);
            EI.Fld->fIsResult=false;
            break;
          case TV_LDescFldId: 
            p=Slt0->sDesc();
            EI.Fld->fEditable=false/*!Slt->bDriverTag*/;
            EI.Fld->fIsResult=false;
            break;
          case TV_AutoTune: 
          case TV_TuneWithHold:
          case TV_HoldAll:
          case TV_SettleMin: 
          case TV_SettleMax: 
          case TV_TestDelta: 
          case TV_PBDeRate: 
          case TV_PlantGain: 
          case TV_PlantDelay: 
          case TV_PlantPeriod: 
          case TV_PIDGain: 
          case TV_PIDIc: 
          case TV_PIDDc: 
            p=Slt->sValue(); 
            if (XStopped())
              EI.Fld->fEditable=Slt->m_bEditableStopped; 
            else
              EI.Fld->fEditable=Slt->m_bEditableRunning; 
            EI.Fld->fIsResult=Slt->bIsResult;
            break;
          case TV_DescFldId:
            p=Slt->sDesc();
            EI.Fld->fEditable=!Slt->IsComment(); 
            break;
          default:;
          }
        }
      else
        {
        switch (EI.FieldId)
          {
          case TV_TagFldId:
            p=Slt->Tag();
            break;
          case TV_MeasFldId:
          case TV_LSptFldId:
          case TV_LMeasFldId:
          case TV_LOutFldId:
          case TV_MinFldId:
          case TV_MaxFldId:
            p="";
            EI.Fld->fEditable=!Slt->IsComment();
            break;
          case TV_DescFldId:
            p=Slt->sDesc();
            EI.Fld->fEditable=!Slt->IsComment();
            break;
          case TV_DummyId:
            p="";
            EI.Fld->fEditable=false;
            break;

          default: p=""; break;
          }
        }
      }
    Str=p ? p : "";
    }
  };

//---------------------------------------------------------------------------

long CTagVwTextC::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    CTagVwSlot * Slt=DataSlot(SlotIndex(EI));//EI.BlkRowNo+EI.Index);
    if (EI.FieldId==TV_TagFldWidthId)
      {
      TagDspWdtFn()=Max(Str.SafeAtoL(), 8L);
      DoRebuild();
      }
    else if (Slt)
      if (EI.FieldId==TV_TagFldId)
        {
        dbgpln("Tag Change : %s", Str());
        Slt->ChangeTag(Str(), Doc(), DefTrndColours[(EI.BlkRowNo+EI.Index) % MaxTrndColours]);

        if (!EI.fPasteBusy)
          Doc()->UpdateAllViews(NULL, TGU_Toggle, NULL);
        DoRebuild();
        CRow(EI.PgRowNo)->Invalidate(ScrGB);
        ClrFldsChanged();
        }
      else if (Slt->TagValid() || Slt->HasFunct())
        {
        flag UpdateRqd=(!gs_Exec.Busy() || gs_Exec.Waiting());
        switch (EI.FieldId)
          {
          case TV_DirnFldId:
            break;
          case TV_AutoTune: 
          case TV_TuneWithHold:
          case TV_HoldAll:
          case TV_SettleMin: 
          case TV_SettleMax: 
          case TV_TestDelta: 
          case TV_PBDeRate: 
          case TV_PlantGain: 
          case TV_PlantDelay: 
          case TV_PlantPeriod: 
          case TV_PIDGain: 
          case TV_PIDIc: 
          case TV_PIDDc: 
          case TV_LSptFldId: 
          case TV_LMeasFldId: 
          case TV_LOutFldId: 
            if (Slt->TagValid())
              {
              CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
              if (!Slt->SetOutput(pDoc, Str(), TRUE, TRUE)) 
                {
                char buff[256];
                sprintf(buff, "%s could not be changed", Slt->Tag());
                AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
                UpdateRqd=0;
                //CRow(EI.PgRowNo)->Invalidate(ScrGB);
                }
              else if (UpdateRqd  && !EI.fPasteBusy)
                Doc()->LoadData(Slt, TGU_LoadDataComplete);
              break;
              }
            break;
          case TV_MeasFldId:
            if (Slt->TagValid())
              {
              CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
              if (!Slt->SetOutput(pDoc, Str(), TRUE, TRUE)) 
                {
                char buff[256];
                sprintf(buff, "%s could not be changed", Slt->Tag());
                AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
                UpdateRqd=0;
                //CRow(EI.PgRowNo)->Invalidate(ScrGB);
                }
              else if (UpdateRqd  && !EI.fPasteBusy)
                Doc()->LoadData(Slt, TGU_LoadDataComplete);
              break;
              }
            break;
          case TV_MaxFldId: 
            if (Slt->bTagValid && IsNumData(Slt->cType) && Str.Find(' ')>0)
              Slt->sMaxVal = Str.Left(Str.Find(' '));
            else
              Slt->sMaxVal = Str;
            if (Slt->nTrendNo>=0 && !EI.fPasteBusy)
              {
              Doc()->UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
              Doc()->FixTimeBase();
              }
            break;
          case TV_MinFldId: 
            if (Slt->bTagValid && IsNumData(Slt->cType) && Str.Find(' ')>0)
              Slt->sMinVal = Str.Left(Str.Find(' '));
            else
              Slt->sMinVal = Str;
            if (Slt->nTrendNo>=0 && !EI.fPasteBusy)
              {
              Doc()->UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
              Doc()->FixTimeBase();
              }
            break;
          case TV_DescFldId: 
            Slt->sDesc=Str; 
            break;
          default:;
          }
        if (UpdateRqd)
          {
          DoReload();
          }
        if (!EI.fPasteBusy)
          Doc()->UpdateAllViews(NULL, TGU_Toggle, NULL);
        ClrFldsChanged();
        }
    }
  return 0;
  }

//---------------------------------------------------------------------------

long CTagVwTextC::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (CurrentBlk(EI))
    {
    flag UpdateRqd=(!gs_Exec.Busy() || gs_Exec.Waiting());
    switch (EI.FieldId)
      {
      case TV_AutoTune:
      case TV_TuneWithHold:
      case TV_HoldAll:
        {
        CTagVwSlot * Slt=DataSlot(SlotIndex(EI));//EI.BlkRowNo+EI.Index);
        if (Slt && (Slt->TagValid() || Slt->HasFunct()))
          {
          if (!Slt->SetOutput(Doc(), Str(), TRUE, TRUE)) 
            {
            char buff[256];
            sprintf(buff, "%s could not be changed", Slt->Tag());
            AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
            UpdateRqd=0;
            //CRow(EI.PgRowNo)->Invalidate(ScrGB);
            }
          else if (UpdateRqd  && !EI.fPasteBusy)
            {
            Doc()->LoadData(NULL, TGU_LoadDataComplete|TGU_Toggle);
            }
          }
        break;
        }
      case TV_ScrollToEnd:
        {
        PostMessage(WM_COMMAND, IDM_TRND_SCROLLEND, 0); //NB: Must POST !
        //OnMenu(IDM_TRND_SCROLLEND);
        break;
        }
      case TV_Reconect:
        {
        PostMessage(WM_COMMAND, IDM_TRND_RECONNECT, 0); //NB: Must POST !
        //OnMenu(IDM_TRND_RECONNECT);
        break;
        }
      }
    }
  return Fix;
  }

//--------------------------------------------------------------------------

flag CTagVwTextC::Changed(FxdEdtInfo &EI)
  {
  if (CurrentBlk(EI))
    {
    CTagVwSlot * Slt=DataSlot(SlotIndex(EI));//EI.BlkRowNo+EI.Index);
    if (Slt && (Slt->TagValid() || Slt->HasFunct()))
      switch(EI.FieldId)
        {
        //case TV_RawFldId:
        //  return Slt->RawChanged();
        case TV_AutoTune:
        case TV_TuneWithHold:
        case TV_HoldAll:
        case TV_SettleMin:
        case TV_SettleMax:
        case TV_TestDelta:
        case TV_PBDeRate:
        case TV_PlantGain:
        case TV_PlantDelay:
        case TV_PlantPeriod:
        case TV_PIDGain:
        case TV_PIDIc:
        case TV_PIDDc:
        case TV_LCtrlFldId:
        case TV_LSptFldId:
        case TV_LMeasFldId:
        case TV_LOutFldId:
          return Slt->MeasChanged();
        default:
          return TRUE;
        }
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

void CTagVwTextC::ClearChanged(FxdEdtInfo &EI)
  {
  if (CurrentBlk(EI))
    {
    CTagVwSlot * Slt=DataSlot(SlotIndex(EI));//EI.BlkRowNo+EI.Index);
    if (Slt && (Slt->TagValid() || Slt->HasFunct()))
      switch(EI.FieldId)
        {
        //case TV_RawFldId:
        //  Slt->ClearRawChanged();
        //  return;
        case TV_AutoTune:
        case TV_TuneWithHold:
        case TV_HoldAll:
        case TV_SettleMin:
        case TV_SettleMax:
        case TV_TestDelta:
        case TV_PBDeRate:
        case TV_PlantGain:
        case TV_PlantDelay:
        case TV_PlantPeriod:
        case TV_PIDGain:
        case TV_PIDIc:
        case TV_PIDDc:
        case TV_LCtrlFldId:
        case TV_LSptFldId:
        case TV_LMeasFldId:
        case TV_LOutFldId:
          Slt->ClearChanged();
          return;
        default:;
        }
    }
  }

//---------------------------------------------------------------------------

int PosInCtrlFld(FxdEdtInfo &EI, GDIBlk & ScrGB, int X)
  {
  int xs=EI.Fld->Xs(ScrGB);
  int xe=EI.Fld->Xe(ScrGB);
  return ((X-xs)*4)/(xe-xs);
  }

void CTagVwTextC::OnLButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView::OnLButtonUp(nFlags, point);
  FxdEdtInfo EI;
  if (m_iDragI>=0)
    {
    const int DX=(point.x-m_iDragX)/ScrGB.ColWdt();
    flag Changed = false;
    switch (m_iDragI)
      {
      case 0: Changed = WdtChanged(TagDspWdtFn(), DX, 9); break;
      case 1: Changed = WdtChanged(MeasDspWdtFn(), DX, 3); break;
      case 2: Changed = WdtChanged(MinDspWdtFn(), DX, 2); break;
      case 3: Changed = WdtChanged(MaxDspWdtFn(), DX, 2); break;
      }
    if (Changed)
      {
      DoRebuild();
      Doc()->UpdateAllViews(NULL, TGU_Repaint, NULL);
      }
    m_iDragI=-1;
    HCURSOR h=ScdApp()->LoadStandardCursor(IDC_ARROW);
    //SetCursor(h);
    //SetRqdCursor(NULL);
    SetRqdCursor(h);
    }
  else if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
    {
    if (EI.Fld->FieldId==TV_TrndFldId ||
        EI.Fld->FieldId==TV_LTrndFld1Id ||
        EI.Fld->FieldId==TV_LTrndFld2Id ||
        EI.Fld->FieldId==TV_LTrndFld3Id)
      {
      const int iSltNo = SlotIndex(EI);
      CTagVwDoc* pDoc = (CTagVwDoc*)GetDocument();
      if (pDoc->DS[iSltNo].TagValid() || pDoc->DS[iSltNo].HasFunct())
        pDoc->ToggleTrending(iSltNo);
      }
    else if (EI.Fld->FieldId==TV_LCtrlFldId)
      {
      const int iSltNo = SlotIndex(EI);
      switch (PosInCtrlFld(EI, ScrGB, point.x))
        {
        case 0: SelectSlotForView(iSltNo); break;
        case 1: ToggleAutoManual(iSltNo); break;
        case 2: 
        case 3: ToggleTuneRqst(iSltNo); break;
        }
      }
    else if (EI.Fld->FieldId==-1 && ChEditPos.x<=4)
      {
      CTagVwSlot * pSlt = DataSlot(SlotIndex(EI));//EI.BlkRowNo+EI.Index);
      if (pSlt)
        {
        if (pSlt->bDriverTag)
          pSlt->ShowDriverTagInfo(Doc(), 0x0001);
        else if (pSlt->bArchiveTag)
          pSlt->ShowArchiveTagInfo(Doc(), 0x0001);
        else if (pSlt->TagValid())
          {
          m_sTag4Access[0]=pSlt->sTag();
          const int DotPos = m_sTag4Access[0].Find('.');
          if (DotPos>=0)
            m_sTag4Access[0]= m_sTag4Access[0].Left(DotPos);
          PostMessage(WM_COMMAND, IDM_TRND_FINDTAG_0, 0);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwTextC::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  FxdEdtView::OnRButtonUp(nFlags, point);
  if (!CTagVwTextC::DoOnRButtonUp(nFlags, point))
    CTagVwText::DoOnRButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

flag CTagVwTextC::DoOnRButtonUp(UINT nFlags, CPoint point) 
  {
  FxdEdtInfo EI;
  if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
    {
    if (EI.Fld->FieldId==TV_LCtrlFldId)
      {
      const int iSltNo = SlotIndex(EI);//EI.BlkRowNo+EI.Index;
      CTagVwSlot * Slt = DataSlot(SlotIndex(EI));

      if (Slt->TagValid() || Slt->HasFunct())
        {
        dword State=(dword)Slt->sValue.SafeAtoL();//.atol()

        CMenu Menu;
        Menu.CreatePopupMenu();

        Menu.AppendMenu(MF_STRING, IDM_CTRL_VIEW, "&View\tCtrl+V");
        Menu.AppendMenu(MF_STRING, IDM_CTRL_TUNE, "&Tune\tCtrl+T");
        if (State & CSW_TuneRqst)
          Menu.CheckMenuItem(IDM_CTRL_TUNE, MF_BYCOMMAND|MF_CHECKED);

        Menu.AppendMenu(MF_SEPARATOR);

        CMenu FitMenu;
        FitMenu.CreatePopupMenu();
        CTagVwDoc::CreateFitMenu(FitMenu, true, true);
        Menu.AppendMenu(MF_POPUP, (UINT)FitMenu.m_hMenu, "&Fit all trends");

        m_iMenuSlot=iSltNo;

        CRect Rect;
        GetWindowRect(&Rect);
        MenuPt.x = Rect.left+point.x;
        MenuPt.y = Rect.top+point.y;
        Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, MenuPt.x, MenuPt.y, this);

        Menu.DestroyMenu();
        }
      return true;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void CTagVwTextC::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  FxdEdtView::OnLButtonDblClk(nFlags, point);
  /*KGA 10/7/2000 : is any of this needed ???
  FxdEdtInfo EI;
  if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
    {
    if (EI.Fld->FieldId==TV_LCtrlFldId)
      {
      const int iSltNo = SlotIndex(EI);
      switch (PosInCtrlFld(EI, ScrGB, point.x))
        {
        case 0: SelectSlotForView(iSltNo); break;
        case 1: ToggleAutoManual(iSltNo); break;
        case 2: 
        case 3: ToggleTuneRqst(iSltNo); break;
        }
      }
    }*/
  }

//---------------------------------------------------------------------------

void CTagVwTextC::SelectSlotForView(int iSltNo)
  {
  if (iSltNo>=0)
    Doc()->SetViewControl((iSltNo-Doc()->LoopSlotOne())/SlotsPerCtrl);
  else
    Doc()->SetViewControl(0);
  Doc()->Rebuild();
  }

//---------------------------------------------------------------------------

void CTagVwTextC::ToggleTuneRqst(int iSltNo)
  {
  if (iSltNo>=0)
    {
    CTagVwSlot * Slt = DataSlot(iSltNo);
    dword State=(dword)Slt->sValue.SafeAtoL();
    if ((State & CSW_AutoMan)==0 || (State & CSW_Active)==0)
      return;
    State = (State & ~CSW_TuneRqst) | (~State & CSW_TuneRqst); // Invert TuneRqst
    Slt->sValue.Set("%i", State);
    State = State | (CSW_TuneRqst<<16); // Mask Values
    Strng Str;
    Str.Set("%i", State);
    CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
    if (!Slt->SetOutput(pDoc, Str(), TRUE, TRUE)) 
      {
      char buff[256];
      sprintf(buff, "%s could not be changed", Slt->Tag());
      AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
      }
    Doc()->UpdateAllViewsByMsg(NULL, TGU_Repaint, NULL);
    }
  }

//---------------------------------------------------------------------------

void CTagVwTextC::ToggleAutoManual(int iSltNo)
  {
  if (iSltNo>=0)
    {
    CTagVwSlot * Slt = DataSlot(iSltNo);
    dword State=(dword)Slt->sValue.SafeAtoL();
    State = (State & ~CSW_AutoMan) | (~State & CSW_AutoMan); // Invert CSW_AutoMan
    Slt->sValue.Set("%i", State);
    State = State | (CSW_AutoMan<<16); // Mask Values
    Strng Str;
    Str.Set("%i", State);
    CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
    if (!Slt->SetOutput(pDoc, Str(), TRUE, TRUE)) 
      {
      char buff[256];
      sprintf(buff, "%s could not be changed", Slt->Tag());
      AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
      }
    Doc()->UpdateAllViewsByMsg(NULL, TGU_Repaint, NULL);
    }
  }

//---------------------------------------------------------------------------

flag CTagVwText::DoMouseMove(UINT nFlags, CPoint point)
  {
  //if (m_hArrow==NULL)
  //  m_hArrow=ScdApp()->LoadCursor(IDC_MYHAND);
  //if (m_hHand==NULL)
  //  m_hHand=ScdApp()->LoadCursor(IDC_MYHAND);
  CRect rct;
  GetClientRect(&rct);

  flag InRct=(SendMessage(WM_NCHITTEST)==HTCLIENT);

  point.x=Range(rct.left, point.x, rct.right);
  point.y=Range(rct.top, point.y, rct.bottom);

  CPoint RP;
  Point2RowCols(ScrGB, point, RP);

  HCURSOR h=NULL;
  int Slt2HiLite=-1;
  FxdEdtInfo EI;
  if (LocateFromCR((int)RP.x, (int)RP.y, EI))
    {
    switch (EI.Fld->FieldId)
      {
      case TV_TrndFldId:
      case TV_LTrndFld1Id:
      case TV_LTrndFld2Id:
      case TV_LTrndFld3Id:
        h=ScdApp()->LoadCursor(MAKEINTRESOURCE(IDC_MYHANDTRND));
        Slt2HiLite = SlotIndex(EI);
        break;
      case TV_TagFldId:
      case TV_MeasFldId:
      case TV_MinFldId:
      case TV_MaxFldId:
      case TV_DescFldId:
      case TV_LMeasFldId:
      case TV_LSptFldId:
      case TV_LDiffFldId:
      case TV_LOutFldId:
      case TV_LDescFldId:
        Slt2HiLite = SlotIndex(EI);
        break;
      case TV_LCtrlFldId:
        {
        switch (PosInCtrlFld(EI, ScrGB, point.x))
          {
          case 0: h=ScdApp()->LoadCursor(MAKEINTRESOURCE(IDC_MYHANDVIEW)); break;
          case 1: h=ScdApp()->LoadCursor(MAKEINTRESOURCE(IDC_MYHANDAUTO)); break;
          case 2: 
          case 3: h=ScdApp()->LoadCursor(MAKEINTRESOURCE(IDC_MYHANDTUNE)); break;
          }
        break;
        }
      }
    }

  //CTagVwText *pCapWnd=dynamic_cast<CTagVwText *>(GetCapture());
  //if ((pCapWnd==this) && !InRct)
  //  {
  //  ReleaseCapture();
  //  Slt2HiLite=-1;
  //  }
  //else if (pCapWnd==NULL)
  //  {
  //  //SetCapture();
  //  if (h==NULL)
  //    h=ScdApp()->LoadStandardCursor(IDC_ARROW);
  //  SetCursor(h);
  //  }

  if (Slt2HiLite>=0 && m_uTimerId==0)
    {
    m_uTimerId=SetTimer(IDT_BLINK, 250, NULL);
    m_uTimerCnt=0;
    //Doc()->HiLiteTrend(Slt2HiLite);
    }
  else if ((Slt2HiLite<0) && m_uTimerId && KillTimer(IDT_BLINK))
    m_uTimerId=0;
  //else
  Doc()->HiLiteTrend(Slt2HiLite);

  return 0;
  }

//---------------------------------------------------------------------------

void CTagVwText::OnTimer(UINT nIDEvent) 
  {
	// TODO: Add your message handler code here and/or call default
  //dbgpln("Timer: %3i %6i", m_uTimerId, GetTickCount());

  Doc()->BlinkTrend();
  //CTagVwText::OnTimer(nIDEvent);
  if ((++m_uTimerCnt>8) && m_uTimerId && KillTimer(IDT_BLINK))
    {
    m_uTimerId=0;
    m_uTimerCnt=0;
    Doc()->HiLiteTrend(-1);
    }
  }

//---------------------------------------------------------------------------

void CTagVwTextC::OnMenuTune(UINT id)
  {
  switch (id)
    {
    case IDM_CTRL_VIEW:
      if (m_iMenuSlot>=0)
        Doc()->SetViewControl((m_iMenuSlot-Doc()->LoopSlotOne())/SlotsPerCtrl);
      else
        Doc()->SetViewControl(0);
      Doc()->Rebuild();
      //Doc()->LoadData();
      //Doc()->UpdateAllViewsByMsg(NULL, TGU_Repaint, NULL);
      //DoRebuild();
      //LoadStrings();
      break;
    case IDM_CTRL_TUNE:
      if (m_iMenuSlot>=0)
        {
        CTagVwSlot * Slt = DataSlot(m_iMenuSlot);
        dword State=(dword)Slt->sValue.SafeAtoL();//.atol()
        //if (State & CSW_TuneRqst)
        //  State 
        State = (State & ~CSW_TuneRqst) | (~State & CSW_TuneRqst); // Invert TuneRqst
        //Strng Str;
        Slt->sValue.Set("%i", State);
        State = State | (CSW_TuneRqst<<16); // Mask Values
        Strng Str;
        Str.Set("%i", State);
        CTagVwDoc* pDoc=(CTagVwDoc*)GetDocument();
        if (!Slt->SetOutput(pDoc, Str(), TRUE, TRUE)) 
          {
          char buff[256];
          sprintf(buff, "%s could not be changed", Slt->Tag());
          AfxMessageBox(buff, MB_ICONSTOP|MB_OK);
          }
        Doc()->UpdateAllViewsByMsg(NULL, TGU_Repaint, NULL);
        }
      break;
    }
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

