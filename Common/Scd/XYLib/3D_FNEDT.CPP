//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#include "stdafx.h"
#define __3D_FNEDT_CPP
#include "sc_defs.h"
#include "limits.h"
#include "gpfuncs.h"
#include "vectors.h"
#include "3d_fn.h"
#include "fixedit.h"
#include "3d_fnedt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int C3DFnEdt::XPix=10000;
const int C3DFnEdt::YPix=10000;
const int C3DFnEdt::CrvPts=128;

//------------------------------------------------------------------------------

struct MappingSave
  {
  POINT    VOrg;
  SIZE     VExt;
  POINT    WOrg;
  SIZE     WExt;
  };

//---------------------------------------------------------------------------

void ZPushScaling(rGDIBlk GB, RECT &GraphR, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_ANISOTROPIC);
  SetViewportOrgEx(GB.DC().m_hDC, GraphR.left, GraphR.top, &MapSv.VOrg);
  SetViewportExtEx(GB.DC().m_hDC, GraphR.right-GraphR.left, GraphR.bottom-GraphR.top, &MapSv.VExt);
  SetWindowOrgEx(GB.DC().m_hDC, 0, C3DFnEdt::YPix, &MapSv.WOrg);
  SetWindowExtEx(GB.DC().m_hDC, C3DFnEdt::XPix, -C3DFnEdt::YPix, &MapSv.WExt);
  }

//---------------------------------------------------------------------------

void ZPopScaling(rGDIBlk GB, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_TEXT);
  SetViewportOrgEx(GB.DC().m_hDC, MapSv.VOrg.x,  MapSv.VOrg.y, NULL);
  SetViewportExtEx(GB.DC().m_hDC, MapSv.VExt.cx, MapSv.VExt.cy, NULL);
  SetWindowOrgEx(GB.DC().m_hDC, MapSv.WOrg.x,  MapSv.WOrg.y, NULL);
  SetWindowExtEx(GB.DC().m_hDC, MapSv.WExt.cx, MapSv.WExt.cy, NULL);
  }

//---------------------------------------------------------------------------

C3DFnEdt::C3DFnEdt(pFxdEdtView pView_, pC3DFn pFn_) :
  FxdEdtBookRef(pView_)
  {
  pFn=pFn_;
  //pFn->Clear();
  bBusy = False;
  GrfRows=10;
  GrfCols=10;
  CrsRows=10;
  CrsCols=10;
  iDragPtNo = -1;
  pLabels = new CLabelWnd;
  pLabels->CreateIt(pView_);
  pLabels->SetFont(pView_->ScrGB.pFont[0]);
  pLabels->SetLineCnt(3);
  }

//---------------------------------------------------------------------------

C3DFnEdt::~C3DFnEdt()
  {
  delete pLabels;
  }

//---------------------------------------------------------------------------

void C3DFnEdt::StartBuild()
  {
  pFn->SetView(pView);
  pFn->StartBuild();
  }

//---------------------------------------------------------------------------

void C3DFnEdt::Build()
  {
  pFn->SetView(pView);
  pFn->Build();
  }

//---------------------------------------------------------------------------

void C3DFnEdt::ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols)
  {
  int i=0;
  GrfCols=Min(30, (int)TotalCols);
  GrfRows=Min(20, (TotalCols*2)/3);
  Pg.MoveRows(-32000);
  Pg.MoveRows(GrfRows);
  }

//---------------------------------------------------------------------------

void C3DFnEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  pFn->Load(EI, Str);
  }

//---------------------------------------------------------------------------

long C3DFnEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  switch (pFn->Parse(EI, Str))
    {
    case 1 : 
      Wnd(pView).InvalidateRect(&GraphR); 
      Wnd(pView).InvalidateRect(&CrsR); 
      break;
    }
  return 0;
  }

//---------------------------------------------------------------------------

long C3DFnEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  switch (pFn->ButtonPushed(EI, Str))
    {
    case 1 : Wnd(pView).InvalidateRect(&GraphR); break;
    }
  return 0;
  }

//---------------------------------------------------------------------------

void C3DFnEdt::DoTriangle(CDC &DC,double *a,double *b,double *c,double dz)
  {  
  if (_isnan(a[2]) || _isnan(b[2]) || _isnan(c[2]) || (a[2]==b[2] && a[2]==c[2]))
    return;

  int tst[3];
  tst[0] = 0;
  tst[1] = 0;
  tst[2] = 0;
  double *mn,*mx,*md,x1,x2,y1,y2;

  if (fmod(a[2], dz) < 0.0001)
    a[2] += 0.0001*dz;

  if (fmod(b[2], dz) < 0.0001)
    b[2] += 0.0001*dz;

  if (fmod(c[2], dz) < 0.0001)
    c[2] += 0.0001*dz;

  if (a[2]>=b[2] && a[2]>=c[2])
    { mx = a; tst[0] = 1; }
  else if (b[2]>=c[2] && b[2]>=a[2])
    { mx = b; tst[1] = 1; }
  else                                   
    { mx = c; tst[2] = 1; }

  if (a[2]<b[2] && a[2]<c[2])
    { mn = a; tst[0] = 1; }
  else if (b[2]<c[2] && b[2]<a[2])  
    { mn = b; tst[1] = 1; }
  else
    { mn = c; tst[2] = 1; }

  if (tst[0]==0)
    { md = a; }
  else if (tst[1]==0)
    { md = b; }
  else
    { md = c; }

  //dz = pFn->GetActualZ(dz);
  double zmn = mn[2];//pFn->GetActualZ(mn[2]);
  double zmx = mx[2];//pFn->GetActualZ(mx[2]);
  double zmd = md[2];//pFn->GetActualZ(md[2]);
  if ((zmx-zmn)/dz>MaxContourIntervals)
    dz = (zmx-zmn)/MaxContourIntervals; //prevent too many contour lines between two points
  double start = dz*ceil(zmn/dz);
  double end   = dz*floor(zmx/dz);
  
  for (double z = start; z<end+(dz/2.0); z += dz)
    {
    //TRACE("a: %10.3f %10.3f %10.3f\n",  a[0], a[1], a[2]);
    //TRACE("b: %10.3f %10.3f %10.3f\n",  b[0], b[1], b[2]);
    //TRACE("c: %10.3f %10.3f %10.3f\n",  c[0], c[1], c[2]);
    //TRACE("mn: %10.3f %10.3f %10.3f\n", mn[0], mn[1], mn[2]);
    //TRACE("md: %10.3f %10.3f %10.3f\n", md[0], md[1], md[2]);
    //TRACE("mx: %10.3f %10.3f %10.3f\n", mx[0], mx[1], mx[2]);
    //TRACE("z : %10.3f\n", z );
    double delta = (z - zmn)/(zmx - zmn);
    x1 = mn[0] + (mx[0] - mn[0])*delta;
    y1 = mn[1] + (mx[1] - mn[1])*delta;
    DC.MoveTo((int)((x1-XMin)*XFactor),(int)((y1-YMin)*YFactor));
    if (z<=zmd)
      {
      delta = (z - zmn)/(zmd - zmn);
      x2 = mn[0] + (md[0] - mn[0])*delta;
      y2 = mn[1] + (md[1] - mn[1])*delta;
      DC.LineTo((int)((x2-XMin)*XFactor),(int)((y2-YMin)*YFactor));
      }
    else
      {
      delta = (z - zmd)/(zmx - zmd);
      x2 = md[0] + (mx[0] - md[0])*delta;
      y2 = md[1] + (mx[1] - md[1])*delta;
      DC.LineTo((int)((x2-XMin)*XFactor),(int)((y2-YMin)*YFactor));
      }
    //TRACE("x2: %10.3f   y2:%10.3f\n", x2, y2);
    //TRACE("delta: %10.3f\n", delta);
    //TRACE("Slope_x: %10.3f  Slope_y:%10.3f   Tot = %10.3f\n\n",x2-x1,y2-y1,(x2-x1)/(y2-y1));
    }
  //pFn->GetNormY(dz);
  //pFn->GetNormY(mn[2]);
  //pFn->GetNormY(mx[2]);
  //pFn->GetNormY(md[2]);
  }

//---------------------------------------------------------------------------

void C3DFnEdt::SimpleContour(CDC &DC)
  {
  int ix,iy;
  double x1,y1;
  double a[3],b[3],c[3];
  const long nx = pFn->XDivs();
  const long ny = pFn->YDivs();
  double dx = (XMax - XMin)/(nx-1);
  double dy = (YMax - YMin)/(ny-1);
  //double dz = (ZMax-ZMin)/pFn->ContourIntervals();
  double dz = 1.0/pFn->ContourIntervals();

  for (ix=0, x1=XMin; ix<nx; ix++,x1 += dx)
    {
    for (iy=0, y1=YMin; iy<ny; iy++,y1 += dy)
      {
      double x2 = x1 + dx;
      double y2 = y1 + dy;
      double xc = x1 + dx/2.0;
      double yc = y1 + dy/2.0;
      double X1 = pFn->GetActualX(x1);
      double X2 = pFn->GetActualX(x2);
      double Y1 = pFn->GetActualY(y1);
      double Y2 = pFn->GetActualY(y2);
      double z1,z2,z3,z4,z5;
      
      //if ((x2<XMax*1.05)&&(y2<YMax*1.05))//in surface area
        {
        z1 = pFn->Zxy(X1, Y1);
        if (_isnan(z1)) 
          continue;
        z2 = pFn->Zxy(X1, Y2);
        if (_isnan(z2)) 
          continue;
        z3 = pFn->Zxy(X2, Y2);
        if (_isnan(z3)) 
          continue;
        z4 = pFn->Zxy(X2, Y1);
        if (_isnan(z4)) 
          continue;
        z1 = pFn->GetNormZ(z1);
        z2 = pFn->GetNormZ(z2);
        z3 = pFn->GetNormZ(z3);
        z4 = pFn->GetNormZ(z4);
        z5 = (z1 + z2 + z3 + z4)/4.0; // to stop jagged contours

        //TRACE("dz:%.3f z1:%.3f z2:%.3f z3:%.3f z4:%.3f z5:%.3f\n",dz,z1,z2,z3,z4,z5);
      
        a[0] = x1; a[1] = y1; a[2] = z1;
        b[0] = x1; b[1] = y2; b[2] = z2;
        c[0] = xc; c[1] = yc; c[2] = z5;
        
        DoTriangle(DC,a,b,c,dz);
    
        a[0] = x2; a[1] = y2; a[2] = z3;
        DoTriangle(DC,a,b,c,dz);
    
        b[0] = x2; b[1] = y1; b[2] = z4;
        DoTriangle(DC,a,b,c,dz);
    
        a[0] = x1; a[1] = y1; a[2] = z1;
        DoTriangle(DC,a,b,c,dz);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void C3DFnEdt::OnDrawBack(rGDIBlk GB, int PgNo, CRgn &ClipRgn)
  {
  ASSERT(pFn->GetNIV()==2);
  const int len = pFn->Length();
  //if (pFn->Length()==0)
  //  return;
  CDC &DC = GB.DC();
  CDCResChk ResChk(DC);
  SetPosition(GB); // Temporary
  flag DoGraph = DC.RectVisible(&GraphR);
  flag DoCrs = DC.RectVisible(&CrsR);
  CGdiObject* OldBrush = (CGdiObject*)DC.SelectObject(GB.pBrushGrfBack);
  POINT XY[CrvPts*2];
  double dX,dY,dZ;
  COLORREF OldBkColor;
  CPen* OldPen;
  if (DoGraph || DoCrs)
    {
    //pFn->bIsBuilt = False;
    //pFn->ReFit();

    pFn->SetXDispMax(Max(pFn->XDispMax(), pFn->XDispMin()+1.0e-6));
    pFn->SetYDispMax(Max(pFn->YDispMax(), pFn->YDispMin()+1.0e-6));
    XMin = pFn->GetNormX(pFn->XDispMin());
    XMax = pFn->GetNormX(pFn->XDispMax());
    YMin = pFn->GetNormY(pFn->YDispMin());
    YMax = pFn->GetNormY(pFn->YDispMax());
    dX = GTZ(XMax - XMin);
    dY = GTZ(YMax - YMin);
	  XFactor = Range(-1.0e9, XPix/dX, 1.0e9);
	  YFactor = Range(-1.0e9, YPix/dY, 1.0e9);

    pFn->SetZDispMax(Max(pFn->ZDispMax(), pFn->ZDispMin()+1.0e-6));
    ZMin = pFn->GetNormZ(pFn->ZDispMin());
    ZMax = pFn->GetNormZ(pFn->ZDispMax());
    dZ = GTZ(ZMax - ZMin);
    ZFactor = Range(-1.0e9, YPix/dZ, 1.0e9);

    OldBkColor = DC.SetBkColor(GB.crGrfBack);
    OldPen = DC.SelectObject(GB.pPenTxtBord);
    DC.Rectangle(&GraphR);
    DC.Rectangle(&CrsR);
    }

  if (DoCrs)
    {//Draw cross section...
    MappingSave MapSv;
    ZPushScaling(GB, CrsR, MapSv);

    /*CRgn ClipRgnGrf;
    ClipRgnGrf.CreateRectRgnIndirect(&CrsR);
    int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);*/

    const long CrsPts = pFn->NoCrsPts();
    if (CrsPts>1)
      {
      //calculate the distances between points...
      CDVector DistPts(CrsPts-1);
      double CrsDist = 0.0;
      for (int j=0; j<CrsPts-1; j++)
        {
        double X1 = pFn->CrsSection[0][j];
        double X2 = pFn->CrsSection[0][j+1];
        double Y1 = pFn->CrsSection[1][j];
        double Y2 = pFn->CrsSection[1][j+1];
        DistPts[j] = Max(1.0e-10, sqrt(((X1-X2)*(X1-X2))+((Y1-Y2)*(Y1-Y2))));
        CrsDist += DistPts[j];
        }
      double DFactor = XPix/CrsDist;
      
      if (CrsPts>2)
        {//draw vertical lines at points (contour turning points)
        ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
        CPoint Org = DC.GetWindowOrg();
        CSize Ext = DC.GetWindowExt();
        double d = 0.0;
        for (j=0; j<CrsPts-2; j++)
          {
          d += DistPts[j];
          int x1 = (int)(d*DFactor);
          DC.MoveTo(x1,Org.y);
          DC.LineTo(x1,Org.y+Ext.cy);
          }
        }

      //draw the cross section...
      ::SelectObject(DC.m_hDC, GB.hLinePen[0]);
      CDVector Fst(2);
      CDVector Snd(2);
      long TmpCount = 0;
      Fst[0] = pFn->CrsSection[0][TmpCount];
      Fst[1] = pFn->CrsSection[1][TmpCount];
      Snd[0] = pFn->CrsSection[0][TmpCount+1];
      Snd[1] = pFn->CrsSection[1][TmpCount+1];
      double Dx = CrsDist/CrvPts;
      double X0 = Fst[0];
      double Y0 = Fst[1];
      j = 0;
      XY[j].x = 0;
      XY[j++].y = (int)((pFn->GetNormZ(pFn->Zxy(X0, Y0))-ZMin)*ZFactor);
      double TmpDist = 0.0;
      double TtlDist = 0.0;
      int k = 0;
      while (k<CrvPts)
        {
        TmpDist += Dx;
        TtlDist += Dx;
        while (TmpCount<CrsPts-1 && TmpDist>DistPts[TmpCount])
          {                                  
          Fst = Snd;
          Snd[0] = pFn->CrsSection[0][TmpCount+2];
          Snd[1] = pFn->CrsSection[1][TmpCount+2];
          X0 = Fst[0];
          Y0 = Fst[1];
          XY[j].x = (int)((TtlDist-TmpDist+DistPts[TmpCount])*DFactor);
          XY[j++].y = (int)((pFn->GetNormZ(pFn->Zxy(X0, Y0))-ZMin)*ZFactor);
          TmpDist -= DistPts[TmpCount];
          TmpCount++;
          }
        if (TmpCount<CrsPts-1 && j<CrvPts*2)
          {
          X0 = (TmpDist/DistPts[TmpCount]*(Snd[0]-Fst[0]))+Fst[0];
          Y0 = (TmpDist/DistPts[TmpCount]*(Snd[1]-Fst[1]))+Fst[1];
          XY[j].x = (int)(TtlDist*DFactor);
          XY[j++].y = (int)((pFn->GetNormZ(pFn->Zxy(X0, Y0))-ZMin)*ZFactor);
          }
        k++;
        }
      DC.Polyline(XY, j);
      }
    ZPopScaling(GB, MapSv);
    }

  if (DoGraph)
    {
    MappingSave MapSv;
    ZPushScaling(GB, GraphR, MapSv);

    CRgn ClipRgnGrf;
    ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
    int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);

    ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
    CPoint Org=DC.GetWindowOrg();
    CSize  Ext=DC.GetWindowExt();
    
    //draw axis...
    int x1 = (int)((pFn->GetNormX(0.0)-XMin)*XFactor);
    DC.MoveTo(x1,Org.y);
    DC.LineTo(x1,Org.y+Ext.cy);
    int y1 = (int)((pFn->GetNormY(0.0)-YMin)*YFactor);
    DC.MoveTo(Org.x,y1);
    DC.LineTo(Org.x+Ext.cx,y1);

    //draw crosses at each point...
    ::SelectObject(DC.m_hDC, GB.hLinePen[0]);
    pFn->GetNormX(pFn->Xs());
    pFn->GetNormY(pFn->Ys());
    const int dx = XPix/150;
    const int dy = YPix/150;
    for (int j=0; j<len; j++)
      {
      int x1 = ((int)((pFn->Xp(j)-XMin)*XFactor));
      int y1 = ((int)((pFn->Yp(j)-YMin)*YFactor));
      DC.MoveTo(x1-dx,y1);
      DC.LineTo(x1+dx,y1);
      DC.MoveTo(x1,y1-dy);
      DC.LineTo(x1,y1+dy);
      }
    pFn->GetActualX(pFn->Xs());
    pFn->GetActualY(pFn->Ys());

    //draw cross section on surface
    const long CrsPts = pFn->NoCrsPts();
    for (j=0; j<CrsPts; j++)
      {
      double X1 = pFn->GetNormX(pFn->CrsSection[0][j]);
      double Y1 = pFn->GetNormY(pFn->CrsSection[1][j]);
      XY[j].x = (int)((X1-XMin)*XFactor);
      XY[j].y = (int)((Y1-YMin)*YFactor);
      }
    DC.Polyline(XY, CrsPts);
    
    SimpleContour(DC);

    ZPopScaling(GB, MapSv);
    }

  if (DoGraph || DoCrs)
    {
    DC.SetBkColor(OldBkColor);
    DC.SelectObject(OldPen);
    }

  DC.SelectObject(OldBrush);
  }

//---------------------------------------------------------------------------

void C3DFnEdt::SetPosition(rGDIBlk GB)
  {
  int CW = GB.ColWdt();
  int RH = GB.RowHgt();
  GraphR = GB.LuDataRect();
  GraphR.top -= 1;
  GraphR.right = GraphR.left+GrfCols*CW;
  GraphR.bottom = GraphR.top+GrfRows*RH;

  GraphR.top+=View().CPg->FixedRows*RH;
  GraphR.bottom+=View().CPg->FixedRows*RH;

  CrsR = GraphR;
  CrsCols = GrfCols;
  CrsRows = GrfRows;
  CrsR.left = GraphR.right + 3;
  CrsR.right = CrsR.left + CrsCols*CW;
  CrsR.bottom = CrsR.top + CrsRows*RH;
  iNameWidth = Max(Max(pFn->XNameLen(), pFn->YNameLen()), pFn->ZNameLen());
  pLabels->SetPosition(GraphR.left, GraphR.bottom+1, iNameWidth + 18);
  }

//---------------------------------------------------------------------------

/*void C3DFnEdt::SetScaling(rGDIBlk GB)
  {
  GB.DC().SetMapMode(MM_ANISOTROPIC);
  GB.DC().SetViewportOrg(GraphR.left, GraphR.top);
  GB.DC().SetViewportExt((GraphR.right-(GrfCols+1)*GB.ColWdt())-GraphR.left, GraphR.bottom-GraphR.top);
  int Xb=XPix/20;
  int Yb=YPix/20;
  GB.DC().SetWindowOrg(-(0+Xb), YPix+Xb);
  GB.DC().SetWindowExt(XPix+2*Xb, -(YPix+2*Yb));
  }*/

//---------------------------------------------------------------------------

void C3DFnEdt::PointtoLP(POINT &Pt)
  {
  CDC* pDC = &(View().ScrGB.DC());
  CDCResChk ResChk(pDC);
  MappingSave MapSv;
  ZPushScaling(View().ScrGB, GraphR, MapSv);
  pDC->DPtoLP(&Pt);
  ZPopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------

void C3DFnEdt::PointtoWP(POINT Pt, double& x, double& y)
  {
  rGDIBlk rGB = View().ScrGB;
  CDC& rDC = rGB.DC();
  CDCResChk ResChk(rDC);
  MappingSave MapSv;
  ZPushScaling(View().ScrGB, GraphR, MapSv);
  rDC.DPtoLP(&Pt);
  ZPopScaling(View().ScrGB, MapSv);
  x = pFn->GetActualX(XMin+(Pt.x/NZ(XFactor)));
  y = pFn->GetActualY(YMin+(Pt.y/NZ(YFactor)));
  }

//---------------------------------------------------------------------------

void C3DFnEdt::PointtoCrsP(POINT Pt, double& d, double& z)
  {
  rGDIBlk rGB = View().ScrGB;
  CDC& rDC = rGB.DC();
  CDCResChk ResChk(rDC);
  MappingSave MapSv;
  ZPushScaling(View().ScrGB, GraphR, MapSv);
  rDC.DPtoLP(&Pt);
  ZPopScaling(View().ScrGB, MapSv);
  d = pFn->GetActualX(XMin+(Pt.x/NZ(XFactor)));
  z = pFn->GetActualZ(ZMin+(Pt.y/NZ(ZFactor)));
  }

//---------------------------------------------------------------------------

int C3DFnEdt::ClosestPt(CPoint point)
  {
  ASSERT(FALSE); //ToDo
  return -1;
  }

//---------------------------------------------------------------------------

void C3DFnEdt::ToggleDigCursor(POINT &Pt)
  {
  CClientDC dc(&Wnd(pView));
  CDCResChk ResChk(dc);

  MappingSave MapSv;
  ZPushScaling(View().ScrGB, GraphR, MapSv);
  CPen APen(PS_SOLID,1,RGB(0xFF, 0xFF, 0xFF));//dc.GetNearestColor(Doc().DigColour));
  CGdiObject* OldPen=(CGdiObject*)dc.SelectObject(&APen);
  int OldMode = dc.SetROP2(R2_XORPEN);
  dc.MoveTo(Pt.x,0);
  dc.LineTo(Pt.x,YPix);
  dc.MoveTo(0, Pt.y);
  dc.LineTo(XPix, Pt.y);
  dc.SetROP2(OldMode);
  ZPopScaling(View().ScrGB, MapSv);
  }

//---------------------------------------------------------------------------
/*bga
This if for creating a cross-section line to be displayed.
*/
//---------------------------------------------------------------------------

flag C3DFnEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  if (PtInRect(&GraphR, point))
    {
    if (nFlags & MK_SHIFT)
      {
      if (!bBusy) 
        pFn->iNoCrsPts = 0;
      bBusy = True;
      }
    else
      bBusy = False;
      
    if (bBusy)
      {
      double x,y;
      PointtoWP(point, x, y);
      pFn->CrsSection.SetSize(2,++pFn->iNoCrsPts);
      pFn->CrsSection[0][pFn->NoCrsPts()-1] = x;
      pFn->CrsSection[1][pFn->NoCrsPts()-1] = y;
      Wnd(pView).InvalidateRect(&GraphR); 
      Wnd(pView).InvalidateRect(&CrsR); 
      }
    return 1;
    }
  else
    bBusy = False;
  return 0;
  }

//---------------------------------------------------------------------------

flag C3DFnEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag C3DFnEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag C3DFnEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag C3DFnEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag C3DFnEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag C3DFnEdt::DoMouseMove(UINT nFlags, CPoint point)
  {
  if (!(nFlags & MK_SHIFT))
    bBusy=False;
  if (PtInRect(&GraphR, point))
    {
    double x,y;
    PointtoWP(point, x, y);
    char Buff[256];
    sprintf(Buff, "%*s(X):%14.3f", iNameWidth, pFn->XName(), x);
    pLabels->SetText(0, Buff);
    sprintf(Buff, "%*s(Y):%14.3f", iNameWidth, pFn->YName(), y);
    pLabels->SetText(1, Buff);
    double z=pFn->Zxy(x, y);
    if (Valid(z))
      sprintf(Buff, "%*s(Z):%14.3f", iNameWidth, pFn->ZName(), z);
    else
      sprintf(Buff, "%*s(Z):%14s", iNameWidth, pFn->ZName(), "*");
    pLabels->SetText(2, Buff);
    pLabels->Show();
    pLabels->Invalidate();
    return 1;
    }
  else if (PtInRect(&CrsR, point))
    {
    double x,z;
    PointtoCrsP(point, x, z);
    char Buff[256];
    sprintf(Buff, "%*s(X):%14.3f", iNameWidth, pFn->XName(), x);
    pLabels->SetText(0, Buff);
//    double y=dNAN;//pFn->Zxy(x, y);
    double y=pFn->Yxz(x, z);
    y=dNAN;
    if (Valid(z))
      sprintf(Buff, "%*s(Y):%14.3f", iNameWidth, pFn->YName(), z);
    else
      sprintf(Buff, "%*s(Y):%14s", iNameWidth, pFn->YName(), "*");
    pLabels->SetText(1, Buff);
    sprintf(Buff, "%*s(Z):%14.3f", iNameWidth, pFn->ZName(), z);
    pLabels->SetText(2, Buff);
    pLabels->Show();
    pLabels->Invalidate();
    return 1;
    }
  else
    pLabels->Hide();
  return 0;
  }

//---------------------------------------------------------------------------
