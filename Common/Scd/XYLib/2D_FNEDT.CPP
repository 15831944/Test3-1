//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#include "stdafx.h"
#define __2D_FNEDT_CPP
#include "sc_defs.h"
#include "limits.h"
#include "gpfuncs.h"
#include "vectors.h"
#include "2d_fn.h"
#include "fixedit.h"
#include "2d_fnedt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int C2DFnEdt::XPix=10000;
const int C2DFnEdt::YPix=10000;
const int C2DFnEdt::CrvPts=128;

//------------------------------------------------------------------------------

struct MappingSave
  {
  POINT    VOrg;
  SIZE     VExt;
  POINT    WOrg;
  SIZE     WExt;
  };

//---------------------------------------------------------------------------

void PushScaling(rGDIBlk GB, RECT &GraphR, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_ANISOTROPIC);
  SetViewportOrgEx(GB.DC().m_hDC, GraphR.left, GraphR.top, &MapSv.VOrg);
  SetViewportExtEx(GB.DC().m_hDC, GraphR.right-GraphR.left, GraphR.bottom-GraphR.top, &MapSv.VExt);
  SetWindowOrgEx(GB.DC().m_hDC, 0, C2DFnEdt::YPix, &MapSv.WOrg);
  SetWindowExtEx(GB.DC().m_hDC, C2DFnEdt::XPix, -C2DFnEdt::YPix, &MapSv.WExt);
  }

//---------------------------------------------------------------------------

void PopScaling(rGDIBlk GB, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_TEXT);
  SetViewportOrgEx(GB.DC().m_hDC, MapSv.VOrg.x,  MapSv.VOrg.y, NULL);
  SetViewportExtEx(GB.DC().m_hDC, MapSv.VExt.cx, MapSv.VExt.cy, NULL);
  SetWindowOrgEx(GB.DC().m_hDC, MapSv.WOrg.x,  MapSv.WOrg.y, NULL);
  SetWindowExtEx(GB.DC().m_hDC, MapSv.WExt.cx, MapSv.WExt.cy, NULL);
  }

//---------------------------------------------------------------------------

C2DFnEdt::C2DFnEdt(pFxdEdtView pView_, pC2DFn pFn_) :
  FxdEdtBookRef(pView_)
  {
  pFn = pFn_;
  iDragPtNo = -1;
  pLabels = new CLabelWnd;
  pLabels->CreateIt(pView_);
  pLabels->SetFont(pView_->ScrGB.pFont[0]);
  pLabels->SetLineCnt(2);
  }

//---------------------------------------------------------------------------

C2DFnEdt::~C2DFnEdt()
  {
  pLabels->DestroyWindow();
  //cnmdelete pLabels;
  }

//---------------------------------------------------------------------------

void C2DFnEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void C2DFnEdt::PutDataDone()
  {
  if (pFn->YCumulative())
    {
    pFn->ToCumulative();
    }
  }

//---------------------------------------------------------------------------

void C2DFnEdt::GetDataStart()
  {
  if (pFn->YCumulative())
    {
    pFn->ToFractional();
    }
  }

//---------------------------------------------------------------------------

void C2DFnEdt::GetDataDone()
  {
  if (pFn->YCumulative())
    {
    pFn->ToCumulative();
    }
  }

//---------------------------------------------------------------------------

void C2DFnEdt::StartBuild()
  {
  pFn->SetView(pView);
  pFn->StartBuild();
  }

//---------------------------------------------------------------------------

void C2DFnEdt::Build()
  {
  pFn->SetView(pView);
  pFn->Build();
  }

//---------------------------------------------------------------------------

void C2DFnEdt::ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols)
  {
  Pg.MoveRows(-32000);
  Pg.MoveRows(pFn->GraphHeight());
  }

//---------------------------------------------------------------------------

void C2DFnEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  pFn->Load(EI, Str);
  }

//---------------------------------------------------------------------------

long C2DFnEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  switch (pFn->Parse(EI, Str))
    {
    case 1 : Wnd(pView).InvalidateRect(&GraphR); break;
    }
  return 0;
  }

//---------------------------------------------------------------------------

long C2DFnEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  switch (pFn->ButtonPushed(EI, Str))
    {
    case 1 : Wnd(pView).InvalidateRect(&GraphR); break;
    }
  return 0;
  }

//---------------------------------------------------------------------------

void C2DFnEdt::SetRanges(CDVector& YPts, CDVector& XPts)
  {
	const int len = pFn->Length();
  const flag IsXLog = pFn->XLog();
  const flag IsYLog = pFn->YLog();
  if (pFn->AutoScale())
    {
    if (len==0)
      {
      pFn->SetXDispMin(pFn->XLoLimit());
      pFn->SetXDispMax(pFn->XUpLimit());
      pFn->SetYDispMin(pFn->YLoLimit());
      pFn->SetYDispMax(pFn->YUpLimit());
      }
    else
      {
      pFn->SetXDispMin(pFn->ScanXMin());
      pFn->SetXDispMax(pFn->ScanXMax());
      if (!IsXLog)
        {
        double xBorder = (pFn->XDispMax()-pFn->XDispMin())/40.0;
        pFn->SetXDispMin(pFn->XDispMin()-xBorder);
        pFn->SetXDispMax(pFn->XDispMax()+xBorder);
        }
      }
    }
  if (IsXLog)
    {//set ranges to nearest decade
    pFn->SetXDispMin(Max(pFn->XDispMin(), MinLogVal));
    pFn->SetXDispMin(pow(10.0, floor(Log10(pFn->XDispMin()))));
    pFn->SetXDispMax(pow(10.0, ceil(Log10(Max(pFn->XDispMax(), pFn->XDispMin()*10.0)))));
    }
  else
    pFn->SetXDispMax(Max(pFn->XDispMax(), pFn->XDispMin()+1.0e-6));

  if (IsXLog || IsYLog)
    {
    int XDecades = IsXLog ? ((int)(Log10(pFn->XDispMax()) - Log10(pFn->XDispMin()))) : 1;
    int YDecades = IsYLog ? ((int)(Log10(pFn->YDispMax()) - Log10(pFn->YDispMin()))) : 1;
    int Decades = Max(XDecades, YDecades);
    YPts.SetSize(Decades*18+1);
    XPts.SetSize(Decades*18+1);
    double Xv = pFn->XDispMin();
    int i,Cnt = 0;
    for (int j=0; j<Decades; j++)
      {
      for (int i=2; i<20; i++)
        {
        XPts[Cnt] = Xv*((double)i/2.0);
        YPts[Cnt] = pFn->Yx(XPts[Cnt]);
        Cnt++;
        }
      Xv *= 10.0;
      }
    YPts[YPts.GetLen()-1] = pFn->Yx(pFn->XDispMax());
    XPts[XPts.GetLen()-1] = pFn->XDispMax();
    i = 18;
    while (YPts.GetLen()>CrvPts)
      {//too many points, remove some...
      Cnt = 0;
      for (j=0; j<=(Decades*i); j++)
        {
        if (j%i != i-1)
          {
          YPts[Cnt] = YPts[j];
          XPts[Cnt++] = XPts[j];
          }
        }
      YPts.SetSize(Cnt);
      XPts.SetSize(Cnt);
      i -= 2;
      }
    }
  else
    {
    YPts.SetSize(CrvPts);
    XPts.SetSize(0);
    double dCrvX = (pFn->XDispMax() - pFn->XDispMin())/(CrvPts-1);
    double Xv = pFn->XDispMin();
    for (int j=0; j<CrvPts; j++,Xv+=dCrvX)
      YPts[j] = pFn->Yx(Xv);
    }
  if (pFn->AutoScale() && len>0)
    {
    pFn->SetYDispMin(YPts.ScanMin());
    pFn->SetYDispMax(YPts.ScanMax());
    if (!IsYLog)
      {
      double yBorder = (pFn->YDispMax()-pFn->YDispMin())/40.0;
      pFn->SetYDispMin(pFn->YDispMin()-yBorder);
      pFn->SetYDispMax(pFn->YDispMax()+yBorder);
      }
    }
  if (IsYLog)
    {
    pFn->SetYDispMin(pow(10.0, floor(Log10(Max(pFn->YDispMin(), MinLogVal)))));
    pFn->SetYDispMax(pow(10.0, ceil(Log10(Max(pFn->YDispMax(), pFn->YDispMin()*10.0)))));
    }
  else
    pFn->SetYDispMax(Max(pFn->YDispMax(), pFn->YDispMin()+1.0e-6));
  }

void C2DFnEdt::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  ASSERT(pFn->GetNIV()==1);
  CDC &DC = GB.DC();
  CDCResChk ResChk(DC);

  SetPosition(GB); //calulate size of GraphR
  flag DoGraph = DC.RectVisible(&GraphR);

  CGdiObject* OldBrush = (CGdiObject*)DC.SelectObject(GB.pBrushGrfBack);

  if (DoGraph)
    {//draw the curve...
    int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);

    COLORREF OldBkColor = DC.SetBkColor(GB.crGrfBack);
    CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);
    DC.Rectangle(&GraphR);

    MappingSave MapSv;
    PushScaling(GB, GraphR, MapSv);

    CRgn ClipRgnGrf;
    ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
    Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);

    CPoint Org = DC.GetWindowOrg();
    CSize Ext = DC.GetWindowExt();

  	const int len = pFn->Length();
    const flag IsXLog = pFn->XLog();
    const flag IsYLog = pFn->YLog();

    CDVector YPts;
    CDVector XPts;
    SetRanges(YPts, XPts);

    //get normalised values...
    if (IsXLog)
      {
      XMin = Log10(pFn->XDispMin());
      XMax = Log10(pFn->XDispMax());
      }
    else
      {
      XMin = pFn->GetNormX(pFn->XDispMin());
      XMax = pFn->GetNormX(pFn->XDispMax());
      }
    if (IsYLog)
      {
      YMin = Log10(pFn->YDispMin());
      YMax = Log10(pFn->YDispMax());
      }
    else
      {
      YMin = pFn->GetNormY(pFn->YDispMin());
      YMax = pFn->GetNormY(pFn->YDispMax());
      }
    const double dX = GTZ(XMax-XMin);
    const double dY = GTZ(YMax-YMin);
    XFactor = XPix/dX;
    YFactor = YPix/dY;

    //draw axis...
    ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
    int x1,y1;
    if (!IsXLog)
      {
      x1 = (int)Range((double)-INT_MAX, (pFn->GetNormX(0.0)-XMin)*XFactor, (double)INT_MAX);
      DC.MoveTo(x1,Org.y);
      DC.LineTo(x1,Org.y+Ext.cy);
      }
    if (!IsYLog)
      {
      y1 = (int)Range((double)-INT_MAX, (pFn->GetNormY(0.0)-YMin)*YFactor, (double)INT_MAX);
      DC.MoveTo(Org.x,y1);
      DC.LineTo(Org.x+Ext.cx,y1);
      }
    //draw log decades...
    if (IsXLog)
      {
      int Decades = (int)(Log10(pFn->XDispMax()) - Log10(pFn->XDispMin()));
      double Xv = pFn->XDispMin();
      for (int j=0; j<Decades; j++)
        {
        ::SelectObject(DC.m_hDC, GB.hLinePenDim[1]);
        for (int i=1; i<10; i++)
          {
          x1 = (int)Range((double)-INT_MAX, (Log10(Xv*i)-XMin)*XFactor, (double)INT_MAX);
          DC.MoveTo(x1,Org.y);
          DC.LineTo(x1,Org.y+Ext.cy);
          if (i==1)
            ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
          if (Decades>4)
            i++;
          }
        Xv *= 10.0;
        }
      }
    if (IsYLog)
      {
      int Decades = (int)(Log10(pFn->YDispMax()) - Log10(pFn->YDispMin()));
      double Yv = pFn->YDispMin();
      for (int j=0; j<Decades; j++)
        {
        ::SelectObject(DC.m_hDC, GB.hLinePenDim[1]);
        for (int i=1; i<10; i++)
          {
          y1 = (int)Range((double)-INT_MAX, (Log10(Yv*i)-YMin)*YFactor, (double)INT_MAX);
          DC.MoveTo(Org.x,y1);
          DC.LineTo(Org.x+Ext.cx,y1);
          if (i==1)
            ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
          if (Decades>4)
            i++;
          }
        Yv *= 10.0;
        }
      }

    ::SelectObject(DC.m_hDC, GB.hLinePen[0]);
    if (len>0)
      {//draw crosses at each point...
      if (!IsXLog)
        pFn->GetNormX(pFn->Xs());
      if (!IsYLog)
        pFn->GetNormY(pFn->Ys());
      const int dx = XPix/150;
      const int dy = YPix/150;
      for (int j=0; j<len; j++)
        {
        if (IsXLog)
          x1 = (int)Range((double)-INT_MAX, (Log10(GTZ(pFn->Xp(j)))-XMin)*XFactor, (double)INT_MAX);
        else
          x1 = (int)Range((double)-INT_MAX, (pFn->Xp(j)-XMin)*XFactor, (double)INT_MAX);
        if (IsYLog)
          y1 = (int)Range((double)-INT_MAX, (Log10(GTZ(pFn->Yp(j)))-YMin)*YFactor, (double)INT_MAX);
        else
          y1 = (int)Range((double)-INT_MAX, (pFn->Yp(j)-YMin)*YFactor, (double)INT_MAX);
        DC.MoveTo(x1-dx,y1);
        DC.LineTo(x1+dx,y1);
        DC.MoveTo(x1,y1-dy);
        DC.LineTo(x1,y1+dy);
        }
      if (!IsXLog)
        pFn->GetActualX(pFn->Xs());
      if (!IsYLog)
        pFn->GetActualY(pFn->Ys());
      }

    //draw the curve...
    POINT XY[CrvPts];
    if (IsXLog && IsYLog)
      {
      // cnm
      for (int j=0; j<YPts.GetLen(); j++)
        {
        XY[j].x = (int)Range((double)-XPix, (Log10(GTZ(XPts[j]))-XMin)*XFactor, (double)XPix);
        XY[j].y = (int)Range((double)-YPix, (Log10(GTZ(YPts[j]))-YMin)*YFactor, (double)YPix);
        //dbgpln("%3i x:%7i y:%7i", j, XY[j].x, XY[j].y);
        }
      //ASSERT(FALSE); //LOG Y scale graph not implemented
      }
    else if (IsXLog)
      {
      for (int j=0; j<YPts.GetLen(); j++)
        {
        XY[j].x = (int)Range((double)-XPix, (Log10(GTZ(XPts[j]))-XMin)*XFactor, (double)XPix);
        XY[j].y = (int)Range((double)-YPix, (pFn->GetNormY(YPts[j])-YMin)*YFactor, (double)YPix);
        //dbgpln("%3i x:%7i y:%7i", j, XY[j].x, XY[j].y);
        }
      }
    else if (IsYLog)
      {
      // cnm
      for (int j=0; j<YPts.GetLen(); j++)
        {
        XY[j].x = (int)Range((double)-XPix, (pFn->GetNormX(XPts[j])-XMin)*XFactor, (double)XPix);
        XY[j].y = (int)Range((double)-YPix, (Log10(YPts[j])-YMin)*YFactor, (double)YPix);
        //dbgpln("%3i x:%7i y:%7i", j, XY[j].x, XY[j].y);
        }
      //ASSERT(FALSE); //LOG Y scale graph not implemented
      }
    else
      {
      double dCrvX = dX/(YPts.GetLen()-1);
      double Xv = XMin;
      for (int j=0; j<YPts.GetLen(); j++,Xv+=dCrvX)
        {
        XY[j].x = (int)Range((double)-INT_MAX, (Xv-XMin)*XFactor, (double)INT_MAX);
        XY[j].y = (int)Range((double)-INT_MAX, (pFn->GetNormY(YPts[j])-YMin)*YFactor, (double)INT_MAX);
        }
      }
    DC.Polyline(XY, YPts.GetLen());
    PopScaling(GB, MapSv);
    DC.SelectObject(OldPen);
    DC.SetBkColor(OldBkColor);
    }
  DC.SelectObject(OldBrush);
  }

//---------------------------------------------------------------------------

void C2DFnEdt::SetPosition(rGDIBlk GB)
  {
  int CW = GB.ColWdt();
  int RH = GB.RowHgt();
  GraphR = GB.LuDataRect();
  GraphR.top -= 1;
  GraphR.right = GraphR.left+pFn->GraphWidth()/*GrfCols*/*CW;
  GraphR.bottom = GraphR.top+pFn->GraphHeight()/*GrfRows*/*RH;

  GraphR.top+=View().CPg->FixedRows*RH;
  GraphR.bottom+=View().CPg->FixedRows*RH;

  iNameWidth = Max(pFn->XNameLen(), pFn->YNameLen());
  pLabels->SetPosition(GraphR.left, GraphR.bottom+1, iNameWidth + 18);
  }

//---------------------------------------------------------------------------

void C2DFnEdt::PointtoLP(POINT &Pt)
  {
  CDC* pDC = &(View().ScrGB.DC());
  CDCResChk ResChk(pDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  pDC->DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

void C2DFnEdt::PointtoWP(POINT Pt, double& x, double& y)
  {
  rGDIBlk rGB = View().ScrGB;
  CDC& rDC = rGB.DC();
  CDCResChk ResChk(rDC);
  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  rDC.DPtoLP(&Pt);
  PopScaling(View().ScrGB, MapSv);
  if (pFn->XLog())
    x = pow(10.0, XMin+(Pt.x/XFactor));
  else
    x = pFn->GetActualX(XMin+(Pt.x/XFactor));
  if (pFn->YLog())
    y = pow(10.0, YMin+(Pt.y/YFactor));
  else
    y = pFn->GetActualY(YMin+(Pt.y/YFactor));
  }

//---------------------------------------------------------------------------

int C2DFnEdt::ClosestPt(CPoint point)
  {
  double x,y;
  PointtoWP(point, x, y);
  int PtNo = -1;
  double dist = DBL_MAX;
  for (int j=0; j<pFn->Length(); j++)
    {
    double d = Sqr(x - pFn->Xp(j)) + Sqr(y - pFn->Yp(j));
    if (d<dist)
      {
      dist = d;
      PtNo = j;
      }
    }
  return PtNo;
  }

//---------------------------------------------------------------------------

void C2DFnEdt::ToggleDigCursor(POINT &Pt)
  {
  CClientDC dc(&Wnd(pView));
  CDCResChk ResChk(dc);

  MappingSave MapSv;
  PushScaling(View().ScrGB, GraphR, MapSv);
  CPen APen(PS_SOLID,1,RGB(0xFF, 0xFF, 0xFF));//dc.GetNearestColor(Doc().DigColour));
  CGdiObject* OldPen=(CGdiObject*)dc.SelectObject(&APen);
  int OldMode=dc.SetROP2(R2_XORPEN);
  dc.MoveTo(Pt.x,0);
  dc.LineTo(Pt.x,YPix);
  dc.MoveTo(0, Pt.y);
  dc.LineTo(XPix, Pt.y);
  dc.SetROP2(OldMode);
  PopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    if (nFlags & MK_SHIFT)
      iDragPtNo = ClosestPt(point);
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    if (iDragPtNo>=0)
      {
      View().DoReload();
      iDragPtNo = -1;
      }
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag C2DFnEdt::DoMouseMove(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    double x,y;
    PointtoWP(point, x, y);
    char Buff[256];
    sprintf(Buff, "%*s(X):%#14.5g", iNameWidth, pFn->XName(), x);
    pLabels->SetText(0, Buff);
    sprintf(Buff, "%*s(Y):%14.5g", iNameWidth, pFn->YName(), y);
    pLabels->SetText(1, Buff);
    pLabels->Show();
    pLabels->Invalidate();
    if (nFlags & MK_LBUTTON)
      {
      if (nFlags & MK_SHIFT)
        {
        if (iDragPtNo>=0)
          {
          pFn->MovePt(iDragPtNo, x, y);
          pFn->bObjModified = 1;
          }
        }
      else
        iDragPtNo = -1;
      Wnd(pView).InvalidateRect(&GraphR);
      }
    return 1;
    }
  else
    {
    pLabels->Hide();
    }
  return 0;
  }

//---------------------------------------------------------------------------
