//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
//#include "qrydlg.h"
#include "tagvdoc.h"
#include "tagvtrnd.h"
#include "tagvdlgs.h"
#include "dbgmngr.h"
#include "scd_wm.h"
#include "syscad.h"

//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgGrfDraw   0
#define dbgTrends        01

#if dbgTrends
static CDbgMngr dbgReBuild        ("Trends", "ReBuild");
static CDbgMngr dbgReBuildPts     ("Trends", "ReBuildPts");
static CDbgMngr dbgDraw           ("Trends", "Draw");
static CDbgMngr dbgThread         ("Trends", "Thread");
static CDbgMngr dbgTimeBase       ("Trends", "TimeBase");
static CDbgMngr dbgQueryHistory   ("Trends", "QueryHistory");
static CDbgMngr dbgDrawTrendTicks ("Trends", "DrawTrendTicks");

inline double dbgTimeRound(double Tim) {return Tim-3600.0*floor(Tim/3600.0);};
#endif

const int LegendFont=1;
const int LegendChars=5;

const long OTU_FromHead = 0x2;

IMPLEMENT_DYNCREATE(CTagVwTrend, CView)

#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

//------------------------------------------------------------------------------

IMPLEMENT_MEMSTATS(CLabelWnd)

//------------------------------------------------------------------------------

struct MappingSave
  {
  POINT    VOrg;
  SIZE     VExt;
  POINT    WOrg;
  SIZE     WExt;
  };

//------------------------------------------------------------------------------

void PushMapping(CDC & dc, CRect TrendR, MappingSave & Sv)
  {
  SetMapMode(dc.m_hDC, MM_ANISOTROPIC);
  SetViewportOrgEx(dc.m_hDC, TrendR.left, TrendR.top, &Sv.VOrg);
  SetViewportExtEx(dc.m_hDC, TrendR.right-TrendR.left, TrendR.bottom-TrendR.top, &Sv.VExt);
  SetWindowOrgEx(dc.m_hDC, 0, TrendYMax+1, &Sv.WOrg);
  SetWindowExtEx(dc.m_hDC, (TrendXMax+1), -(TrendYMax+1), &Sv.WExt);
  }

//------------------------------------------------------------------------------

void PopMapping(CDC & dc, MappingSave & Sv)
  {
  SetMapMode(dc.m_hDC, MM_TEXT);
  SetViewportOrgEx(dc.m_hDC, Sv.VOrg.x,  Sv.VOrg.y, NULL);
  SetViewportExtEx(dc.m_hDC, Sv.VExt.cx, Sv.VExt.cy, NULL);
  SetWindowOrgEx(dc.m_hDC, Sv.WOrg.x,  Sv.WOrg.y, NULL);
  SetWindowExtEx(dc.m_hDC, Sv.WExt.cx, Sv.WExt.cy, NULL);
  }

//---------------------------------------------------------------------------  

void PushPen(CDC & dc, flag PrintFlag, pCTagVwSlot Slt, TrendPenRec &PenMem)
  {
  if (PrintFlag)
    {
    LOGBRUSH Brush = {PS_SOLID, BLACK_PEN, 0};
    //KGA 10/97 PS_USERSTYLE is not supported on Windows 95 !!!
    //PenMem.hPen=::ExtCreatePen(PS_GEOMETRIC|PS_USERSTYLE, Slt->dwWidth, &Brush, Slt->Style.Cnt, Slt->Style.Pat);
//    PenMem.hPen=::ExtCreatePen(PS_GEOMETRIC|PS_SOLID, Slt->dwWidth, &Brush, 0, NULL);
    PenMem.hPen=::CreatePen(Slt->lPen.lopnStyle, Slt->dwWidth, Slt->lPen.lopnColor);
    }
  else
    PenMem.hPen=::CreatePen(Slt->lPen.lopnStyle, Slt->dwWidth, Slt->lPen.lopnColor);
//    PenMem.hPen=::ExtCreatePen(PS_GEOMETRIC|PS_SOLID, Slt->dwWidth, &Slt->Brush, 0, NULL);
    
  PenMem.hOldPen=(HPEN)::SelectObject(dc.m_hDC, PenMem.hPen);
  }

//---------------------------------------------------------------------------  

void PopPen(CDC & dc, TrendPenRec &PenMem)
  {
  ::SelectObject(dc.m_hDC, PenMem.hOldPen);
  ::DeleteObject(PenMem.hPen);
  }

//===========================================================================

CTagVwTrend::CTagVwTrend() :
  STWnd(this), ETWnd(this) ,DurWnd(this)
  {
  m_lNChanges=0;
  DrwStartTime=CTimeValue(0.0);
  DrwCurrentTime=CTimeValue(0.0);

  for (int f=0; f<3; f++) 
    Font[f]=NULL;
  
  CurrentPosR.SetRect(0,0,0,0);
  bCurrentPrinting=1;
  bPrinting=0;
  DblClks=0;
  DblClkTm=0;
  MouseFlags=0;
  bDispCleared=0;
  
  LastTrndNo=0;
  LastTimeCursPos=-1;
  }

//---------------------------------------------------------------------------

CTagVwTrend::~CTagVwTrend()
  {
  for (int f=0; f<3; f++) 
    if (Font[f]) 
      delete Font[f];
  }

//---------------------------------------------------------------------------

BOOL CTagVwTrend::PreCreateWindow(CREATESTRUCT &cs)
  {
  cs.lpszClass=AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_CLASSDC | CS_BYTEALIGNCLIENT,
                                   ScdApp()->LoadCursor(IDC_GRAPHICS1), 
                                   (HBRUSH)GetStockObject(BLACK_BRUSH/*LTGRAY_BRUSH*/), 0);
  cs.style |= WS_CLIPCHILDREN;
  return CView::PreCreateWindow(cs);
  }

//---------------------------------------------------------------------------

int CTagVwTrend::OnCreate(LPCREATESTRUCT lpCreateStruct) 
  {
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
  RECT Rect;
  GetClientRect(&Rect);
  Rect.bottom = Rect.top;
  Rect.right = Rect.bottom;
  STWnd.Create(NULL, "", /*WS_VISIBLE | */WS_CHILD | WS_CLIPSIBLINGS, Rect, this, (UINT)-1);
  ETWnd.Create(NULL, "", /*WS_VISIBLE | */WS_CHILD | WS_CLIPSIBLINGS, Rect, this, (UINT)-1);
  DurWnd.Create(NULL, "", /*WS_VISIBLE | */WS_CHILD | WS_CLIPSIBLINGS, Rect, this, (UINT)-1);
	return 0;
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
  {
  CStopWatch SW;
  SW.Start();
  pCTagVwDoc pDoc=(pCTagVwDoc)GetDocument();
  if (pDoc)
    {
    ((pCTagVwDoc)pDoc)->OnActivate(bActivate);
    }

  CView::OnActivateView(bActivate, pActivateView, pDeactiveView );
  //dbgpln("OnActivate Tnd : %10.3f", SW.Secs());
  }

//---------------------------------------------------------------------------

void CTagVwTrend::ShowLabels()
  {
  CString Buff1,Buff2;
  int Len;
  pCTagVwDoc pDoc = Doc();
  TrndTimeBase TB = pDoc->GetCurrentTimeBase();
  if ((pDoc->ST || pDoc->SD) && !gs_Exec.SolvingPB())
    {
    STWnd.ShowWindow(SW_SHOW);
    if (pDoc->ST)
      {
      Buff1=TB.StartTime.Format(TD_Time);
      if (!pDoc->HS)
        {
        //*strchr(Buff1, '.') = 0;
        Len=Buff1.Find('.');
        if (Len>=0)
          Buff1=Buff1.Left(Len);
        }
      if (pDoc->SD)
        Buff2=TB.StartTime.Format(TD_DateLeft);
      }
    else
      Buff1=TB.StartTime.Format(TD_DateLeft);
    STWnd.SetTextToFit(Buff1.GetBuffer(0), Buff2.GetBuffer(0), 0, 0);
    }
  else
    STWnd.ShowWindow(SW_HIDE);
  if ((pDoc->ET || pDoc->ED) && !gs_Exec.SolvingPB())
    {
    ETWnd.ShowWindow(SW_SHOW);
    if (pDoc->ET)
      {
      Buff1=TB.EndTime.Format(TD_Time);
      if (!pDoc->HS)
        {
        //*strchr(Buff1, '.') = 0;
        Len=Buff1.Find('.');
        if (Len>=0)
          Buff1=Buff1.Left(Len);
        }
      if (pDoc->ED)
        Buff2=TB.EndTime.Format(TD_DateLeft);
      }
    else
      Buff1=TB.EndTime.Format(TD_DateLeft);
    ETWnd.SetTextToFit(Buff1.GetBuffer(0), Buff2.GetBuffer(0), 2, 0);
    }
  else
    ETWnd.ShowWindow(SW_HIDE);
  if ((pDoc->Dur) && !gs_Exec.SolvingPB())
    {
    DurWnd.ShowWindow(SW_SHOW);
    Buff1.Format("<%s>", (TB.EndTime-TB.StartTime).Format(TD_Days|TD_Time));
    if (!pDoc->HS && (TB.EndTime-TB.StartTime>CTimeValue(10.0)))
      {
      Len=Buff1.Find('.');
      if (Len>=0)
        Buff1=Buff1.Left(Len);
      Buff1+=">";
      }
    DurWnd.SetTextToFit(Buff1.GetBuffer(0), "", 1, 2);
    }
  else
    DurWnd.ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnInitialUpdate()
  {
  }

//---------------------------------------------------------------------------

void CTagVwTrend::ClearDisplay()
  {
  LastTimeCursPos=-1;
  Invalidate();//FALSE);
  STWnd.Invalidate();
  ETWnd.Invalidate();
  DurWnd.Invalidate();
  bDispCleared=1;
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint)
  {
  if (lHint==0)
    CView::OnUpdate(pSender, 0, pHint); 

  if (lHint & (TGU_ClearTrendDisplay|TGU_Colour))
    ClearDisplay();

  if (lHint & TGU_Start)
    TryScrollWindow();

  if (lHint & TGU_Stop)
    {
    }

  if (lHint & (TGU_UpdateAll))
    {
    pCTagVwDoc pDoc=Doc();
    UpdateTimeCurs(TimeNAN, pDoc->BackGroundColour, pDoc->CTimeColour); // Clear
    for (int i=0; i<NoTrends(); i++)
      UpdateTrend(i, OTU_FromHead);
    UpdateTimeCurs(gs_Exec.TheTime, pDoc->BackGroundColour, pDoc->CTimeColour); // Redraw 
    }
  }

//---------------------------------------------------------------------------

void CTagVwTrend::SetTrendPosition(CDC* pDC, CRect *CRReqd)
  {
  CRect CRPos;
  if (CRReqd)
    {
    CRPos=*CRReqd;
    ++CRPos.left;
    --CRPos.right;
    ++CRPos.top;
    --CRPos.bottom;
    }
  else
    GetClientRect(CRPos);
  
  if ((CurrentPosR!=CRPos) || (bCurrentPrinting!=bPrinting))
    {
    CurrentPosR=CRPos;
    bCurrentPrinting=bPrinting;
    CDC DC;
    if (pDC==NULL)
      {
      DC.CreateCompatibleDC(NULL);
      pDC=&DC;
      }
    int ChH[3];
    CDCResChk ResChk(DC);
    ChH[0] = pDC->GetDeviceCaps(LOGPIXELSY)/10;//8;
    ChH[1] = pDC->GetDeviceCaps(LOGPIXELSY)/32;//8;//6;
    ChH[2] = pDC->GetDeviceCaps(LOGPIXELSY)/6;//4;
    //int ChW = pDC->GetDeviceCaps(LOGPIXELSX)/10;
    CFont *pOldFont=pDC->GetCurrentFont();
    for (int f=0; f<3; f++)
      {
      if (Font[f]) 
        delete Font[f];
      Font[f] = new CFont;
      Font[f]->CreateFont(ChH[f],              // int nHeight, 
                      0,                       // int nWidth, 
                      0,                       // int nEscapement, 
                      0,                       // int nOrientation, 
                      FW_REGULAR,              // int nWeight, 
                      0,                       // BYTE bItalic, 
                      0,                       // BYTE bUnderline, 
                      0,                       // BYTE cStrikeOut, 
                      1,                       // BYTE nCharSet, 
                      //OUT_DEFAULT_PRECIS,      // BYTE nOutPrecision, 
                      OUT_TT_PRECIS,      // BYTE nOutPrecision, 
                      CLIP_DEFAULT_PRECIS,     // BYTE nClipPrecision, 
                      DEFAULT_QUALITY,         // BYTE nQuality, 
                      FIXED_PITCH|FF_MODERN,   // BYTE nPitchAndFamily, 
                      "Courier"                // LPCSTR lpszFacename 
                     );

      pDC->SelectObject(Font[f]);
      TEXTMETRIC tm;
      pDC->GetTextMetrics(&tm);
      CharSize[f].cx=tm.tmMaxCharWidth;  
      CharSize[f].cy=tm.tmHeight;
      }

    pDC->SelectObject(pOldFont);

    CLabelWnd::pFont = Font[0];
    CLabelWnd::CharX = CharSize[0].cx;
    CLabelWnd::CharY = CharSize[0].cy;

    int Left=CRPos.left;
    int Right=CRPos.right;
    int Top=CRPos.top;
    int Bottom=CRPos.bottom;
    CrSz.cx=CRPos.right-CRPos.left;
    CrSz.cy=CRPos.bottom-CRPos.top;
    int cyh=0;
    int cyf=0;
    if (bPrinting)
      {
      cyh=1*CharSize[LegendFont].cy+1;
      cyf=CharSize[LegendFont].cy+1;
      pCTagVwDoc pDoc=Doc();
      for (int i=0; i<NoTrends(); i++)
        if (pDoc->Trend[i].pSlt)//&& line is on
          cyf+=CharSize[LegendFont].cy;
      }

    TrendR.SetRect(Left, Top+cyh,  Right, Bottom-cyf-1);
    FootR.SetRect (Left, Bottom-cyf, Right, Bottom);
     
    BordR.SetRect(TrendR.left-1, TrendR.top-1, TrendR.right+1, TrendR.bottom+1);
    }

  DrawR.SetRect(CRPos.left-1, CRPos.top-1, CRPos.right+1, CRPos.bottom+1);
  ReDrawR=DrawR;
  }

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTagVwTrend, CView)
  //{{AFX_MSG_MAP(CTagVwTrend)
  ON_WM_MOVE()
  ON_WM_SHOWWINDOW()
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_MESSAGE(WMU_ONUPDATE, OnUpdateByMsg)
  ON_MESSAGE(WMU_SCROLL,   OnScrollByMsg)
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

void CTagVwTrend::OnDraw(CDC* pDC)
  {
  CDCResChk ResChk(pDC);

  bDispCleared=0;

  pCTagVwDoc pDoc=Doc();
  TrndTimeBase TB = pDoc->GetCurrentTimeBase();
  CTimeValue TimeWindow=TB.EndTime-TB.StartTime;
  CTimeValue TrndStartTime=TB.StartTime+CTimeValue((double)((long)CTimeValue(TimeWindow*(((double)(ReDrawR.left-DrawR.left))/
                                                            (DrawR.right-DrawR.left))).Seconds));
  CRect ClpBox;
  pDC->GetClipBox(&ClpBox);

  flag DoingAll=TRUE;
  int    i;
  CPoint Pt1, Pt2;
  CRgn   ClipRgn;
  ClipRgn.CreateRectRgnIndirect(ClpBox); //ReDrawR);

  COLORREF BackGndColour, BorderColour, GridColour, TimeColour, TextColourBusy, TextColourNotBusy;
  if (bPrinting)
    {
    BackGndColour     = RGB(0xFF, 0xFF,0xFF);
    BorderColour      = RGB(0x00, 0x00,0x00);
    GridColour        = RGB(0x00, 0x00,0x00);
    TimeColour        = RGB(0x00, 0x00,0x00);
    TextColourBusy    = RGB(0x00, 0x00,0x00);
    TextColourNotBusy = RGB(0x00, 0x00,0x00);
    }
  else
    {
    BackGndColour     = pDoc->BackGroundColour;
    BorderColour      = pDoc->BorderColour;
    GridColour        = pDoc->GridColour;
    TimeColour        = pDoc->CTimeColour;
    TextColourBusy    = pDoc->TextColourBusy;
    TextColourNotBusy = pDoc->TextColourNotBusy;
    }

  CBrush BackGndBrush(BackGndColour);
  //CPen   BackPen(PS_SOLID,1,BackGndColour);
  CPen   BorderPen(PS_SOLID, bPrinting ? 0 : 1, pDC->GetNearestColor(BorderColour));
  CPen   GridPen(pDoc->GridStyle, bPrinting ? 0 : 1, pDC->GetNearestColor(GridColour));

  CPen* OldPen=pDC->SelectObject(&BorderPen);
  //CGdiObject* OldBrush=(CGdiObject*)pDC->SelectObject(&BackGndBrush);
  COLORREF OldBk=pDC->SetBkColor(BackGndColour);
  pDC->FillSolidRect(&ClpBox, BackGndColour);
  
  int OldBkMode = pDC->SetBkMode(TRANSPARENT);

  if (!MouseFlags)
    SetRangeValues(False);

  pDC->SelectClipRgn(NULL);
  if (!bPrinting)
    UpdateTimeCurs(DrwCurrentTime, BackGndColour, TimeColour);

  MappingSave MapSv;
  /*if (bPrinting)
    {
    CRect R(&TrendR);
    R.left-=10;
    R.right+=10;
    PushMapping(*pDC, R, MapSv);
    }
  else*/
    PushMapping(*pDC, TrendR, MapSv);

  for (i=0; i<NoTrends(); i++)
    if (pDoc->Trend[i].pSlt && (!bPrinting || pDoc->Trend[i].pSlt->nTrendNo>=0))
      {
      pCTagVwSlot Slt=pDoc->Trend[i].pSlt;

      TrendPenRec PenMem;
      PushPen(*pDC, bPrinting && !bUseColorPrint, Slt, PenMem);

      DrawTrend(i, pDC, DoingAll, False);
      if (bPrinting)
        {
        //TODO Get trend printing to draw legends with labels...
        Pt1.x=FootR.left;    
        Pt1.y=FootR.top+(i+1)*CharSize[LegendFont].cy+CharSize[LegendFont].cy/2;
        Pt2.x=FootR.left+(LegendChars-1)*CharSize[LegendFont].cx;
        Pt2.y=Pt1.y;
        pDC->DPtoLP(&Pt1);
        pDC->DPtoLP(&Pt2);
        pDC->MoveTo(Pt1);
        pDC->LineTo(Pt2);
        }

      PopPen(*pDC, PenMem);
      }

  pDoc->iSltHiLite=-1;
  pDoc->fSltHiLiteOn=false;
  pDC->SelectObject(&GridPen);
  DrawGrid(pDC);
  if (bPrinting)
    {
    pDC->SelectObject(&BorderPen);
    pDC->MoveTo(0, 0);
    pDC->LineTo(0, TrendYMax);
    pDC->LineTo(TrendXMax, TrendYMax);
    pDC->LineTo(TrendXMax, 0);
    pDC->LineTo(0, 0);

    Strng Title,DtTm;
    Title.Set("%s    [SysCAD  %s]", Doc()->GetTitle(), CurDateTime(DtTm));//, FullCopyrightNotice());
    CFont* pOldFont=pDC->SelectObject(Font[LegendFont]);
    COLORREF OldTxt=pDC->SetTextColor(TextColourNotBusy);
    pDC->TextOut(6, TrendYMax, Title(), Title.Length());
    pDC->SetTextColor(OldTxt);
    pDC->SelectObject(pOldFont);
    }

  PopMapping(*pDC, MapSv);
  
  if (!bPrinting)
    {
    DrwStartTime=TB.StartTime;
    DrwEndTime=TB.EndTime;

    ReDrawR.left=TrendR.left/2;
    ReDrawR.right=TrendR.left/2;
    ReDrawR.top=TrendR.top;
    ReDrawR.bottom=TrendR.bottom;

    if (LastTimeCursPos>=0)
      {
      CPen TimePen(pDoc->CTimeStyle, 1, pDC->GetNearestColor(TimeColour));
      pDC->SelectObject(&TimePen);
      Pt1.x=LastTimeCursPos;    
      Pt1.y=TrendR.top;    
      Pt2.x=LastTimeCursPos;
      Pt2.y=TrendR.bottom;
      pDC->MoveTo(Pt1);
      pDC->LineTo(Pt2);
      }
    }

  LockDoc();
  ShowLabels();
  FreeDoc();

  pDC->SelectClipRgn(NULL);
  pDC->SelectObject(OldPen);
  //pDC->SelectObject(OldBrush);
  pDC->SetBkMode(OldBkMode);

  if (TrkTrend.No>=0)
    ToggleDigCursor(TrkTrend.XY);
  }

//---------------------------------------------------------------------------  

#if dbgTrends
void DrawTrendTicks(CDC* pDC, POINT *Pt, int np)
  {
  for (int j=0; j<np; j++)
    {
    POINT TPts[2];
    TPts[0]=Pt[j];
    TPts[1]=Pt[j];
    TPts[0].y+=5;
    TPts[1].y-=5;
    pDC->Polyline(TPts, 2);
    }
  }            
#endif

//---------------------------------------------------------------------------  

void CTagVwTrend::DrawTrend(int iNo, CDC* pDC, flag DoingAll, flag OnlyNew)
  {
  LockDoc();
  pCTagVwDoc pDoc=Doc();
  rCTrendSlot Trend = pDoc->Trend[iNo];
  rCTrendPtBlkList PtBlks=Trend.PtBlks;

  POSITION Pos=Trend.PtBlks.GetHeadPosition();
  int nall=0;

  #if dbgTrends
  const int DrawTrendTicksOn=dbgDrawTrendTicks();
  #endif

  while (Pos)
    {
    pCTrendPtBlk p = PtBlks.GetNext(Pos);
    int n = (p ? p->NPts() : 0);

    if (n>0)
      {
      if (OnlyNew)
        {
        int np=p->NewPoints();
        if (np>0)
          {
          np=Min(np+1, n);
          if (np>=2)
            {
            int i=Max(0, n-np-1);
            pDC->Polyline(&p->Pt[i], np);
            #if dbgTrends
            if (DrawTrendTicksOn)
              DrawTrendTicks(pDC, &p->Pt[i], np);
            #endif
            }
          }
        p->ClearNewPoints();
        }
      else if (DoingAll || pDC->PtVisible(p->Pt[0]) || pDC->PtVisible(p->Pt[n-1]))
        {
        if (n>=2)
          {
          pDC->Polyline(&p->Pt[0], n);
          #if dbgTrends
          if (DrawTrendTicksOn)
            DrawTrendTicks(pDC, &p->Pt[0], n);
          #endif
          }
        nall+=n;
        }
      }
    }

  FreeDoc();
  }  

//---------------------------------------------------------------------------  

double TimeDivSize(TrndTimeBase TB, int nDivs)
  {
  static double AllowableDivs[] = 
    { 1., 5., 10., 30.,                                            // Secs
      60., 2*60., 5*60., 10*60., 20*60.,                           // Mins
      3600., 2*3600., 4*3600., 6*3600., 8*3600.,12*3600.,          // Hours
      86400., 2*86400., 4*86400., 7*86400., 14*86400., 28*86400.   // Days
    };
  CTimeValue TimeWindow=TB.EndTime-TB.StartTime;
  double TDiv=ceil(TimeWindow.Seconds/nDivs);
  
  for (int i=0; i<sizeof(AllowableDivs)/sizeof(AllowableDivs[0]); i++)
    if (AllowableDivs[i]>=TDiv)
      return AllowableDivs[i];
  
  return TDiv;
  }

//---------------------------------------------------------------------------  

void CTagVwTrend::DrawGrid(CDC* pDC)
  {
  LockDoc();
  pCTagVwDoc pDoc=Doc();
  
  int x,y,i;
  if (pDoc->iNXGridDivs>0)
    {
    TrndTimeBase TB = pDoc->GetCurrentTimeBase();
    double dT=TimeDivSize(TB, pDoc->iNXGridDivs);
    double T=floor(TB.StartTime.Seconds/dT)*dT;

    while (1)//i=0; i<pDoc->iNXGridDivs+1; i++)
      {
      x=TimePixel(T, pDoc->TB); 
      if (x>TrendXMax)
        break;
      pDC->MoveTo(x, 0);
      pDC->LineTo(x, TrendYMax);
      T+=dT;
      }
    }

  if (pDoc->iNYGridDivs>0)
    for (i=0; i<pDoc->iNYGridDivs+1; i++)
      {
      y=(TrendYMax*i)/pDoc->iNYGridDivs;
      pDC->MoveTo(0, y);
      pDC->LineTo(TrendXMax, y);
      }

  FreeDoc();
  }  

//---------------------------------------------------------------------------  

void CTagVwTrend::DrawInfo(CDC* pDC)
  {
  LockDoc();

  pCTagVwDoc pDoc = Doc();
  if (bPrinting)
    {
    /*Strng Title,DtTm;
    Title.Set("%s    [SysCAD  %s]", Doc()->GetTitle(), CurDateTime(DtTm));//, FullCopyrightNotice());
    pDC->TextOut(?Tp.x, ?Tp.y, Title(), Title.Length());*/
    }
  else
    ShowLabels();
 
  FreeDoc();
  }  

//---------------------------------------------------------------------------

void CTagVwTrend::UpdateTrend(int iTrnd, long Flags)
  {
  if (bDispCleared)
    return ;

  LockDoc();
  pCTagVwDoc pDoc=Doc();
  rCTrendSlot Trend = pDoc->Trend[iTrnd];

  pCTagVwSlot pSlt=Trend.pSlt;
  if (pSlt)
    if (!MouseFlags)
      {
      CClientDC dc(this);
      CDCResChk ResChk(dc);

      dc.SelectClipRgn(NULL); // Draw 
      MappingSave MapSv;
      PushMapping(dc, TrendR, MapSv);
      TrendPenRec PenMem;
      PushPen(dc, bPrinting && !bUseColorPrint, pSlt, PenMem);
      DrawTrend(iTrnd, &dc, False, True);
      PopPen(dc, PenMem);
      PopMapping(dc, MapSv);
      }
  FreeDoc();
  };

//---------------------------------------------------------------------------

void CTagVwTrend::HiLiteTrend(int iTrnd, flag On)
  {
  if (iTrnd<0 || iTrnd>=NoTrends())
    return;
  if (bDispCleared)
    return ;

  //dbgpln("Trend HiLite %3i %s", iTrnd, On?"ON":"OFF");
  LockDoc();
  pCTagVwDoc pDoc=Doc();
  UpdateTimeCurs(dNAN, pDoc->BackGroundColour, pDoc->CTimeColour); // Clear

  rCTrendSlot Trend = pDoc->Trend[iTrnd];

  pCTagVwSlot pSlt=Trend.pSlt;
  if (pSlt)
    if (!MouseFlags)
      {
      CClientDC dc(this);
      CDCResChk ResChk(dc);

      dc.SelectClipRgn(NULL); // Draw 
      MappingSave MapSv;
      PushMapping(dc, TrendR, MapSv);
      TrendPenRec PenMem;
      //int WMem=pSlt->dwWidth;
      //pSlt->dwWidth*=2;
//      pSlt->Brush.lbColor=iSlot<0 ? RGB(0xff,0xff,0xff) : CTagVwDoc::PenColours[iSlot % ColourArrayLen];
      int OldStyle=pSlt->lPen.lopnStyle;
      pSlt->lPen.lopnStyle=PS_DOT;

      PushPen(dc, bPrinting && !bUseColorPrint, pSlt, PenMem);
      int OldMode=dc.SetROP2(R2_XORPEN);
      int OldBkMode=dc.SetBkMode(TRANSPARENT);
      DrawTrend(iTrnd, &dc, True, False);
      dc.SetBkMode(OldBkMode);
      pSlt->lPen.lopnStyle=OldStyle;
      //pSlt->dwWidth=WMem;
      dc.SetROP2(OldMode);
      PopPen(dc, PenMem);
      PopMapping(dc, MapSv);
      }
  FreeDoc();
  UpdateTimeCurs(gs_Exec.TheTime, pDoc->BackGroundColour, pDoc->CTimeColour); // Redraw 
  }

//---------------------------------------------------------------------------

void CTagVwTrend::TryScrollWindow()
  {
  };

//---------------------------------------------------------------------------

void CTagVwTrend::PointtoLP(POINT &Pt)
  {
  CClientDC dc(this);
  CDCResChk ResChk(dc);
  
  MappingSave MapSv;
  PushMapping(dc, TrendR, MapSv);
  dc.DPtoLP(&Pt);
  PopMapping(dc, MapSv);
  }

//---------------------------------------------------------------------------

void CTagVwTrend::ToggleDigCursor(POINT &Pt)
  {
  if (Pt.x>=0)
    {
    CClientDC dc(this);
    CDCResChk ResChk(dc);

    MappingSave MapSv;
    PushMapping(dc, TrendR, MapSv);
    CPen APen(Doc()->DigStyle,1,dc.GetNearestColor(Doc()->DigColour));
    CGdiObject* OldPen=(CGdiObject*)dc.SelectObject(&APen);
    int OldMode=dc.SetROP2(R2_XORPEN);
    dc.MoveTo(Pt.x, 0);
    dc.LineTo(Pt.x, TrendYMax);
    dc.MoveTo(0, Pt.y);
    dc.LineTo(TrendXMax, Pt.y);
    dc.SetROP2(OldMode);
    dc.SelectObject(OldPen);
    PopMapping(dc, MapSv);
    }
  }

//---------------------------------------------------------------------------

void CTagVwTrend::UpdateTimeCurs(CTimeValue Time, COLORREF BackGndColour, COLORREF TimeColour)
  {
  if (bDispCleared)
    {
    return;
    }

  if (!MouseFlags)
    {
    DrwCurrentTime=Time;

    CClientDC dc(this);
    CDCResChk ResChk(dc);

    POINT Pt1, Pt2;
    //CPen   BlackPen(PS_SOLID, 1, RGB(0,0,0));
    //CPen   WhitePen(PS_SOLID, 1, RGB(255,255,255));
    CPen   BackPen(PS_SOLID, 1, BackGndColour);
    CPen   TimePen(PS_SOLID, 1, TimeColour);
    //CPen   XTimePen(PS_SOLID, 1, 
    //  RGB(Min(255,GetRValue(TimeColour)+GetRValue(BackGndColour)),
    //      Min(255,GetGValue(TimeColour)+GetGValue(BackGndColour)),
    //      Min(255,GetBValue(TimeColour)+GetBValue(BackGndColour))));
    //CPen   XTimePen(PS_SOLID, 1, 
    //  RGB(Min(255,GetRValue(TimeColour)+GetRValue(BackGndColour)),
    //      Min(255,GetGValue(TimeColour)+GetGValue(BackGndColour)),
    //      Min(255,GetBValue(TimeColour)+GetBValue(BackGndColour))));
    CGdiObject *OldPen=dc.SelectObject(&BackPen);
    int OldMode=dc.SetROP2(R2_XORPEN);

    int NewTimeCursPos=-1;
    if (ValidTime(Time))
      {
      pCTagVwDoc pDoc=Doc();
      TrndTimeBase TB = pDoc->GetCurrentTimeBase();

      double TPos=(Time-TB.StartTime).Seconds/GTZ((TB.EndTime-TB.StartTime).Seconds);
      if (TPos >=0.0 && TPos <=1.0)
        NewTimeCursPos=TrendR.left+(long)((TrendR.right-TrendR.left)*TPos+1.5);    
      }
    if (NewTimeCursPos!=LastTimeCursPos)
      {
      if (LastTimeCursPos>=0)
        {
        // Undo drawing 
        Pt1.x=LastTimeCursPos;
        Pt1.y=TrendR.top;    
        Pt2.x=Pt1.x;
        Pt2.y=TrendR.bottom;
        dc.SelectObject(&TimePen);//BackPen);
        dc.MoveTo(Pt1);
        dc.LineTo(Pt2);
        dc.SelectObject(&BackPen);//BackPen);
        dc.MoveTo(Pt1);
        dc.LineTo(Pt2);
        }
      if (NewTimeCursPos>=0)
        {
        Pt1.x=NewTimeCursPos;    
        Pt1.y=TrendR.top;    
        Pt2.x=NewTimeCursPos;
        Pt2.y=TrendR.bottom;
        dc.SelectObject(&BackPen);
        dc.MoveTo(Pt1);
        dc.LineTo(Pt2);
        dc.SelectObject(&TimePen);
        dc.MoveTo(Pt1);
        dc.LineTo(Pt2);
        }
      LastTimeCursPos=NewTimeCursPos;
      }
    dc.SetROP2(OldMode);
    dc.SelectObject(OldPen);

    }
  }

//---------------------------------------------------------------------------

void CTagVwTrend::SetRangeValues(flag FixIndicator)
  {
  pCTagVwDoc pDoc=Doc();
  TrndTimeBase TB = pDoc->GetCurrentTimeBase();

  CTimeValue TimeWindow=TB.EndTime-TB.StartTime;
  }

//---------------------------------------------------------------------------

void CTagVwTrend::SetDigValues(TrackTrend &WrkTrnd)
  {
  if (WrkTrnd.No>=0)
    {
    LockDoc();
    pCTagVwDoc pDoc = Doc();
    rCTrendSlot Trend = pDoc->Trend[WrkTrnd.No];
    rCTagVwSlot Slot= *Trend.pSlt;
    const char* Tag = Slot.sTag();
    const char* Desc = Slot.sDesc();
    const char* Date;
    //char DateBuff[256];
    TrndTimeBase TB = pDoc->GetCurrentTimeBase();
    CTimeValue XMn = TB.StartTime;
    CTimeValue XMx = TB.EndTime; 
    long t = (long)(((XMx-XMn).Seconds*WrkTrnd.XY.x)/TrendXMax);
    Date=(XMn + CTimeValue((double)t)).Format(TD_Time|(gs_Exec.SyncWithClock()?TD_DateRight:0));
    double YMn = Trend.Scl.YMn;
    double YMx = Trend.Scl.YMx;
    double Val = YMn+((YMx-YMn)*WrkTrnd.XY.y)/TrendYMax;
    FreeDoc();

    char InfoDate[512], /*InfoTag[512],*/ InfoVal[512], InfoRange[512];
    sprintf(InfoDate, "%s     (%dsecs)", Date, t);              //sprintf(InfoTag, Desc ? "%s[%s]" : "%s", Tag, Desc);
    //sprintf(InfoDesc, "%s", Tag, Desc);
    //Strng SVal, SMn, SMx;
    Slot.Fmt.FormatFloat(Val, InfoVal, sizeof(InfoVal));

    Slot.Fmt.FormatFloat(YMn, InfoRange, sizeof(InfoRange));
    strcat(InfoRange, " .. ");
    int l=strlen(InfoRange);
    Slot.Fmt.FormatFloat(YMx, &InfoRange[l], sizeof(InfoRange)-l);
    //if (0 && Slot.Cnv.Text())
    //  sprintf(InfoVal, "%s (%s)  [%s >> %s]", SVal(), Slot.Cnv.Text(), SMn(), SMx());
    //else
    //  sprintf(InfoVal, "%s  [%s >> %s]", SVal(), SMn(), SMx());
//      sprintf(InfoVal, "%.4g", Val);
    CTrndInfo::SetCurrentData(InfoDate, Tag, Desc, InfoVal, InfoRange);
    }
  else
    {
    CTrndInfo::SetCurrentData("", "", "", "", "");
    }
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnMove(int x, int y)
{
  CView::OnMove(x, y);
  // TODO: Add your message handler code here
  SetTrendPosition(NULL, NULL);
}

//---------------------------------------------------------------------------

void CTagVwTrend::OnShowWindow(BOOL bShow, UINT nStatus)
{
  CView::OnShowWindow(bShow, nStatus);
  // TODO: Add your message handler code here
  SetTrendPosition(NULL, NULL);
}

//---------------------------------------------------------------------------

void CTagVwTrend::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize(nType, cx, cy);
  // TODO: Add your message handler code here
  SetTrendPosition(NULL, NULL);
}

//---------------------------------------------------------------------------

void CTagVwTrend::OnLButtonDown(UINT nFlags, CPoint point)
  {
  // TODO: Add your message handler code here and/or call default

  MDIActivateThis(this);

  if (MouseFlags==0)
    {
    //if (nFlags & MK_SHIFT)
    //  {
    //  MouseFlags=nFlags;
    //
    //  // point must be in device co-ords for this call
    //  CTrndInfo::Open(this, point, (const char*)Doc()->GetTitle());
    //
    //  MouseFlags=nFlags;
    //  PointtoLP(point);
    //  SetCapture();
    //
    //  TrackTrend ClsTrnd;
    //  pCTagVwDoc pDoc=Doc();
    //  if (pDoc->FindClosestTrend(FCT_Vertical, point, ClsTrnd, LastTrndNo))
    //    {
    //    TrkTrend=ClsTrnd;
    //    ToggleDigCursor(TrkTrend.XY);
    //    }
    //  SetDigValues(TrkTrend);
    //
    //  LastMousePos.x=-1;
    //  gs_pCmd->SetFocus();
    //
    //  }
    //else
      {
      MouseFlags=nFlags;
      // point must be in device co-ords for this call
      CTrndInfo::Open(this, point, (const char*)Doc()->GetTitle());

      PointtoLP(point);
      SetCapture();
  
      TrackTrend ClsTrnd;
      pCTagVwDoc pDoc=Doc();
      if (pDoc->FindClosestTrend(FCT_Vertical, point, ClsTrnd, LastTrndNo))
        {
        TrkTrend=ClsTrnd;
        ToggleDigCursor(TrkTrend.XY);
        }
      SetDigValues(TrkTrend);
  
      LastMousePos.x=-1;
      gs_pCmd->SetFocus();
      }
    }
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnLButtonUp(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (MouseFlags & MK_LBUTTON)
    {
    PointtoLP(point);
    ToggleDigCursor(TrkTrend.XY);
    TrkTrend.Clear();
    ReleaseCapture();
    LastMousePos.x=-1;
    MouseFlags=0;
    }
  CTrndInfo::Close();
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnRButtonDown(UINT nFlags, CPoint point)
  {
  // TODO: Add your message handler code here and/or call default

  MDIActivateThis(this);

  if (1)//nFlags & MK_SHIFT)
    {
    POINT pointlp=point;
    PointtoLP(pointlp);
    TrackTrend ClsTrnd;
    pCTagVwDoc pDoc=Doc();
    pCTagVwSlot pSlot = NULL;

    CMenu Menu;
    Menu.CreatePopupMenu();
    CRect WRect;
    GetWindowRect(&WRect);
    CPoint  RBPoint;
    RBPoint.x = WRect.left+point.x;
    RBPoint.y = WRect.top+point.y;
    double Val=0.0;
    flag DoingScales=false;
   // pCTagVwDoc pDoc = Doc();
    //pCTagVwSlot pSlot = NULL;

    if (pDoc->FindClosestTrend(FCT_Vertical, pointlp, ClsTrnd, LastTrndNo))
      {
      flag OK=false;

      rCTrendSlot Trend = pDoc->Trend[ClsTrnd.No];    
      pSlot = Trend.pSlt;
      if (pSlot && Valid(pSlot->dValue))// && (nChar==VK_F11))
        {
        DoingScales=true;

//        char* Tag = Trend.pSlt->sTag();
//        char* Desc = Trend.pSlt->sDesc();
        double YMn = Trend.Scl.YMn;
        double YMx = Trend.Scl.YMx;
        Val = YMn+((YMx-YMn)*ClsTrnd.XY.y)/TrendYMax;


        Menu.AppendMenu(MF_STRING, IDM_TRNDSCL_TAG, pSlot->sTag());
        Menu.AppendMenu(MF_SEPARATOR);
        pDoc->CreateScaleMenu(Menu, pSlot);
        Menu.AppendMenu(MF_SEPARATOR);
        }
      }

    CMenu ColourMenu;
    ColourMenu.CreatePopupMenu();
    ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURBACK       , "BackGround");
    //ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURBORDER     , "Border");
    ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURGRID       , "Grid");
    ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURCTIME      , "TimeCursor");
    //ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURDIG        , "Dig");
    //ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURTEXTBUSY   , "TextBusy");
    ColourMenu.AppendMenu(MF_STRING, IDM_TREND_COLOURTEXTNOTBUSY, "Text");//"TextNotBusy");

    Menu.AppendMenu(MF_POPUP, (unsigned int)ColourMenu.m_hMenu, "&Colours");

    int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, RBPoint.x, RBPoint.y, this);//&View());
    Menu.DestroyMenu();                                           

    COLORREF * pColour=NULL;
    switch (RetCd)
      {
      case IDM_TREND_COLOURBACK:        pColour=&(Doc()->BackGroundColour); break;
      case IDM_TREND_COLOURBORDER:      pColour=&(Doc()->BorderColour); break;
      case IDM_TREND_COLOURGRID:        pColour=&(Doc()->GridColour); break;
      case IDM_TREND_COLOURCTIME:       pColour=&(Doc()->CTimeColour); break;
      case IDM_TREND_COLOURDIG:         pColour=&(Doc()->DigColour); break;
      case IDM_TREND_COLOURTEXTBUSY:    pColour=&(Doc()->TextColourBusy); break;
      case IDM_TREND_COLOURTEXTNOTBUSY: pColour=&(Doc()->TextColourNotBusy); break;
      }

    if (pColour)
      {
      CColorDialog ColDlg(*pColour, CC_RGBINIT | CC_SHOWHELP, this);
      ColDlg.m_cc.lpCustColors = gs_CustomColours;
      if (ColDlg.DoModal()==IDOK)
        {
        *pColour = ColDlg.GetColor();
        pDoc->UpdateAllViews(NULL, TGU_Colour, NULL);
        }
      }
    else if (DoingScales && pDoc->ProcessScaleMenu(RetCd, pSlot, Val))
      {
      pDoc->UpdateAllViews(NULL, TGU_ClearTrendDisplay|TGU_NowActivated, NULL);
      pDoc->FixTimeBase();
      }
    }
  else if (MouseFlags==0)
    {
    /*
    MouseFlags=nFlags;

    // point must be in device co-ords for this call
    CTrndInfo::Open(this, point, (const char*)Doc()->GetTitle());

    MouseFlags=nFlags;
    PointtoLP(point);
    SetCapture();

    TrackTrend ClsTrnd;
    pCTagVwDoc pDoc=Doc();
    if (pDoc->FindClosestTrend(FCT_Vertical, point, ClsTrnd, LastTrndNo))
      {
      TrkTrend=ClsTrnd;
      ToggleDigCursor(TrkTrend.XY);
      }
    SetDigValues(TrkTrend);

    LastMousePos.x=-1;
    gs_pCmd->SetFocus();
    */
    }
}

//---------------------------------------------------------------------------

void CTagVwTrend::OnRButtonUp(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
//  if (MouseFlags & MK_RBUTTON)
//    {
//    PointtoLP(point);
//    ToggleDigCursor(TrkTrend.XY);
//    TrkTrend.Clear();
//    ReleaseCapture();
//    LastMousePos.x=-1;
//    MouseFlags=0;
//    }
//  CTrndInfo::Close();
}

//---------------------------------------------------------------------------

void CTagVwTrend::OnMouseMove(UINT nFlags, CPoint point)
  {
  if (MouseFlags)
    {
    if (LastMousePos != point)
      {
      LastMousePos = point;
      PointtoLP(point);
      if (TrkTrend.No>=0)
        {
        ToggleDigCursor(TrkTrend.XY);
        int TrndNo=TrkTrend.No;
        flag Found=0;
        TrackTrend WrkTrnd;
        pCTagVwDoc pDoc=Doc();
        Found=pDoc->FindClosestTrend(FCT_Vertical, point, WrkTrnd, 
                                     LastTrndNo, 
                                     (nFlags & MK_SHIFT) ? TrndNo : -1);
//                    LastTrndNo, (MouseFlags & MK_LBUTTON) ? -1 : TrndNo);

        if (Found)// && ((MouseFlags&MK_SHIFT)==0))
          TrkTrend=WrkTrnd;
        ToggleDigCursor(TrkTrend.XY);
        SetDigValues(TrkTrend);
        }
      }
    }
  else
    SetRangeValues(True);
  CView::OnMouseMove(nFlags, point);
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  PointtoLP(point);
  if (GetTickCount()-DblClkTm > 5000L)
    DblClks=0;
  DblClkTm=GetTickCount();
  DblPts[DblClks]=point;
  ++DblClks;
  if (DblClks==2)
    {
    if (abs(DblPts[0].x-DblPts[1].x) > TrendXMax*0.01)
      {
      CTimeValue St=DrwStartTime+double(DrwEndTime-DrwStartTime)*Min(DblPts[0].x, DblPts[1].x)/TrendXMax;
      CTimeValue En=DrwStartTime+double(DrwEndTime-DrwStartTime)*Max(DblPts[0].x, DblPts[1].x)/TrendXMax;
      pCTagVwDoc pDoc=Doc();
      pDoc->SetTimebase(St, Max(0.01, En-St), true);
      }
    else
      {
      CTimeValue D=DrwEndTime-DrwStartTime;
      CTimeValue St=DrwStartTime-D*0.5;
      CTimeValue En=DrwEndTime+D*0.5;
      pCTagVwDoc pDoc=Doc();
      pDoc->SetTimebase(St, Max(0.01, En-St), true);
      }
    DblClks=0;
    }
  
  CView::OnLButtonDblClk(nFlags, point);
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  gs_pCmd->SendMessage(WM_CHAR, nChar, MAKELONG(nRepCnt, nFlags));
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  gs_pCmd->SendMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  gs_pCmd->SendMessage(WM_KEYUP, nChar, MAKELONG(nRepCnt, nFlags));
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

void CTagVwTrend::DoOnFilePrint(BOOL UseColors)
  {
  bUseColorPrint = UseColors;
  OnFilePrint();
  }

//---------------------------------------------------------------------------

void CTagVwTrend::DoOnFilePrintPreview()
  {
  OnFilePrintPreview();
  }

//---------------------------------------------------------------------------

BOOL CTagVwTrend::OnPreparePrinting( CPrintInfo* pInfo )
  {
  return DoPreparePrinting(pInfo);
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
  {
  CView::OnBeginPrinting(pDC, pInfo);

  pInfo->m_nCurPage = 1;
  pInfo->SetMinPage(1);
  pInfo->SetMaxPage(1);
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
  {
  CView::OnEndPrinting(pDC, pInfo);
  bPrinting=0;
  SetTrendPosition(NULL, NULL);
  //qqqDoc()->HoldUpdateAllViews(0);
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView )
  {
  CView::OnEndPrintPreview(pDC, pInfo, point, pView);
  //bPreviewing=0;
  }

//---------------------------------------------------------------------------

void CTagVwTrend::OnPrint(CDC *pDC, CPrintInfo* pInfo)
  {
  bPrinting=1;
  SetTrendPosition(pDC, &pInfo->m_rectDraw);
  CView::OnPrint(pDC, pInfo);
  }

//---------------------------------------------------------------------------

LRESULT CTagVwTrend::OnUpdateByMsg(WPARAM wParam, LPARAM lParam)
  {
  OnUpdate(NULL, wParam, (CObject*)lParam);
//  OnUpdate(NULL, 0, NULL);
  return True;
  }

//---------------------------------------------------------------------------

LRESULT CTagVwTrend::OnScrollByMsg(WPARAM wParam, LPARAM lParam)
  {
  pCTagVwDoc pDoc = Doc();
  if (pDoc->Scroll)
    {
    CFloatLParamUnion FL;
    FL.L=lParam;
    double d = FL.f;

    UpdateTimeCurs(dNAN, pDoc->BackGroundColour, pDoc->CTimeColour); // remove TimeMarker

    LastTimeCursPos=-1;
    RECT ClntRct;
    GetClientRect(&ClntRct);
    TrndTimeBase &TB = pDoc->GetCurrentTimeBase();
    long dx=(long)(d/Max(1e-3, (TB.EndTime-TB.StartTime).Seconds)*(ClntRct.right-ClntRct.left));
    if (pDoc->ET || pDoc->ED)
      ETWnd.ShowWindow(SW_HIDE);
    if (pDoc->Dur)
      DurWnd.ShowWindow(SW_HIDE);
    ScrollWindow(-dx, 0, NULL, NULL);
    if (pDoc->ET || pDoc->ED)
      ETWnd.ShowWindow(SW_SHOW);
    if (pDoc->Dur)
      DurWnd.ShowWindow(SW_SHOW);
    }
  else
    ClearDisplay();
  return True;
  }

//===========================================================================

//COLORREF CLabelWnd::BkCol = RGB(0, 0, 0);
//COLORREF CLabelWnd::TxCol = RGB(255, 255, 255);
CFont* CLabelWnd::pFont = NULL;
int CLabelWnd::CharX = 8;
int CLabelWnd::CharY = 14;

//---------------------------------------------------------------------------

CLabelWnd::CLabelWnd(CTagVwTrend * Trnd)
  {
  pTrnd = Trnd;
  Txt1 = "";
  Txt2 = "";
  bMultiLine = 0;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLabelWnd, CWnd)
	//{{AFX_MSG_MAP(CLabelWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CLabelWnd::SetTextToFit(pchar p1, pchar p2, byte xpos, byte ypos)
  {
  Txt1 = p1;
  Txt2 = p2;
  bMultiLine = (Txt2.Length()>0);
  RECT Rect;
  GetParent()->GetClientRect(&Rect);
  int width = CharX * Max(Txt1.Length(), Txt2.Length());
  if (xpos==1)
    Rect.left = ((Rect.right - Rect.left)/2) - (width/2);
  else if (xpos==2)
    Rect.left = Rect.right - width - 1;
  Rect.right = Rect.left + width;
  int height = CharY;
  if (bMultiLine)
    height += CharY;
  if (ypos==1)
    Rect.top = ((Rect.bottom - Rect.top)/2) - (height/2);
  else if (ypos==2)
    Rect.top = Rect.bottom - height;
  Rect.bottom = Rect.top + height;
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (GetWindowPlacement(&wp))
    {
    wp.rcNormalPosition.left = Rect.left;
    wp.rcNormalPosition.right = Rect.right;
    wp.rcNormalPosition.bottom = Rect.bottom;
    wp.rcNormalPosition.top = Rect.top;
    wp.length = sizeof(wp);
    SetWindowPlacement(&wp);
    }
  //Invalidate();
  }

//---------------------------------------------------------------------------

void CLabelWnd::OnPaint() 
  {
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  CFont * pOldFont=dc.GetCurrentFont();
  if (pFont)
    dc.SelectObject(pFont);
  dc.SetBkColor(pTrnd->Doc()->BackGroundColour);
  dc.SetTextColor(pTrnd->Doc()->TextColourNotBusy);
  RECT Rect;
  GetClientRect(&Rect);
  //int OldBkMode = dc.SetBkMode(TRANSPARENT);
  //CBrush Brush(BkCol);
  //dc.FillRect(&Rect, &Brush);
  if (bMultiLine)
    {
    Rect.bottom = CharY;
    dc.DrawText(Txt1(), Txt1.Length(), &Rect, DT_LEFT);
    Rect.top += CharY;
    Rect.bottom += CharY;
    dc.DrawText(Txt2(), Txt2.Length(), &Rect, DT_LEFT);
    }
  else
    dc.DrawText(Txt1(), Txt1.Length(), &Rect, DT_LEFT);
  // Do not call CStatic::OnPaint() for painting messages
  //dc.SetBkMode(OldBkMode);
  dc.SelectObject(pOldFont);
  }

//===========================================================================

