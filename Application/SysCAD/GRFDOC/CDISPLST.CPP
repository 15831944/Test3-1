//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "dxfstuff.h"
#include "grf3drw.h"
#include "dynmseg.h"
#include "traceent.h"

//#include "optoff.h"
    
CEntInView::CEntInView(DML_ITEM item_in, DXF_ENTITY e_in, C3_CURVE c_in, pViewport vpin, int BoxSlct, int ColorRqd, int OvrColorRqd, int OvrPenRqd, double Thick, char * Tag)
  {
  //dbgpln("CEntInView %08x", this);
  if(fix_and_test_buffer())
    {
    instr=new LONGPOINT[size_buffer_points()];
    ni=new int[size_buffer_entries()];
    type=new short[size_buffer_entries()];
    nni=size_buffer_entries();
    ninstr=size_buffer_points();
    segflag=0;
    vp=vpin;
    if(!vpin)
      {
      TRACE0(" CEntInView points to null viewport\n");
      }
    memcpy(instr, addr_buffer_points(), size_buffer_points()*sizeof(LONGPOINT));
    memcpy(type, addr_buffer_type(), size_buffer_entries()*sizeof(short));
    memcpy(ni, addr_buffer_entries(), size_buffer_entries()*sizeof(int));
    }
  else
    {
    instr=(LONGPOINT*)NULL;
    ni=(int*)NULL;
    type=NULL;

    // cnm
    nni=0;
    ninstr=0;
    }
  e=e_in;
  c=c_in;
  item=item_in;
  fBoxSlct=BoxSlct;
  m_iColorRqd=ColorRqd;
  m_iOvrColorRqd=OvrColorRqd;
  m_iOvrPenRqd=OvrPenRqd;
  m_iColorDrawn=-2;
  m_iOvrColorDrawn=-2;
  #if WithLineThickness
  m_ThicknessRqd=Thick;
  #else
  m_ThicknessRqd=1;
  #endif
  rgbFillColorRqd=RGB(255,0,0);//GR_LIGHTRED;
  pTag=Tag;
  }

CEntInView::~CEntInView()
  {
  //dbgpln("~CEntInView %08x", this);
  if(instr)
    delete[]instr;
  if(ni)
    delete[]ni;
  if(type)
    delete[]type;
  if(item)
    item->pKenwalta=NULL;
  }

extern "C"
{
  mypolypolyline(int xoff, int yoff, LONGPOINT *p, short *t, DWORD *x, DWORD cxx, double level, double level2, int dytype, int colorrqd, int *colordrwn, int ovrcolorrqd, int ovrpenrqd, int *ovrcolordrwn, HBRUSH hFillBrush, double thickness);
};


//<! CEntInView*
CEntInView* // this in fact is a delete 
CEntInView::EntityInvalidate()
  {
  //<!    //vp->DelDynamicSegment(this); // removes it from the dynamic list

  m_iColorDrawn=-2;
  vp->DelDisplaySegment(this);

  // removes it from the display list
  return(CEntInView*)NULL;
  }

static CMap<COLORREF, COLORREF, HBRUSH, HBRUSH> s_FillBrushes;
static bool s_FillBrushesInited=false;

HBRUSH GetFillBrushHandle(COLORREF rgb)
  {
  if (!s_FillBrushesInited)
    {
    s_FillBrushes.InitHashTable(500);
    s_FillBrushesInited=true;
    }
  HBRUSH h;
  if (!s_FillBrushes.Lookup(rgb, h))
    {
    h=CreateSolidBrush(rgb);
    s_FillBrushes.SetAt(rgb, h);
    }
  return h;
  };

void CEntInView::Qdisplay(int segment, double level, double level2, int dytype)
  {
  int     flag;
  DWORD     one=1;

  LONGPOINT*p=instr;

  int     i;

  if(ninstr==0)
    return;
  for(i=0; i<segment; i++)
    {
    p+=ni[i];
    }
  flag=dytype;
  if(IfMarkBit())
    flag=dytype|SEG_HIGH;
  mypolypolyline(0, 0, p, type +i, (DWORD *)(ni +i), one, level, level2, dytype, m_iColorRqd, &m_iColorDrawn, m_iOvrColorRqd, m_iOvrPenRqd, &m_iOvrColorDrawn, GetFillBrushHandle(rgbFillColorRqd), m_ThicknessRqd);
  Qdisplay();
  }

void CEntInView::Qdisplay(int xoff, int yoff)
  {
  int     flag;

  if(ninstr==0)
    return;
  flag=0;
  if(IfMarkBit())
    flag=SEG_HIGH;
  mypolypolyline(xoff, yoff, instr, type, (DWORD *)ni, (DWORD)nni, 0.0, 0.0, flag, m_iColorRqd, &m_iColorDrawn, m_iOvrColorRqd, m_iOvrPenRqd, &m_iOvrColorDrawn, GetFillBrushHandle(rgbFillColorRqd), m_ThicknessRqd);
  }


void CEntInView::Qhot(REAL temp)
  {
  static REAL ttt=0.0;

  // remember temp not used
  set_color_temperature(ttt);
  mypolypolyline(0, 0, instr, type, (DWORD *)ni, (DWORD)nni, 0.0, 0.0, 0, -1, &m_iColorDrawn, -1, -1, &m_iOvrColorDrawn, HBRUSH(0), m_ThicknessRqd);//fillcolor);
  ttt+=0.1;
  }

void CEntInView::SetMarkBit()
  {
  segflag|=SEGMENT_MARKED;
  Qdisplay();
  }

void CEntInView::ClrMarkBit()
  {
  segflag&= ~SEGMENT_MARKED;
  Qdisplay();
  }

void CEntInView::Qhighlight()
  {
  if(ninstr==0)
    return;
  grr_set_color(GR_LIGHTYELLOW);
  mypolypolyline(0, 0, instr, type, (DWORD *)ni, (DWORD)nni, 0.0, 0.0, SEG_HIGH, -1, &m_iColorDrawn, -1, -1, &m_iOvrColorDrawn, HBRUSH(0), m_ThicknessRqd);//fillcolor);
  }

void CEntInView::Qprint(FILE *fp)
  {
  if(ninstr==0)
    return;

  int     i, j, k;
  (void)fprintf(fp, "                      CEntInView:%x Prv:%x Nxt:%x Vp:%x C:%x E:%x Segflag: %d Numseg:%d\n", this, prv, nxt, vp, c, e, segflag, nni);
  for(i=0, k=0; i<nni; i++)
    {
    int     col=((0xff00&type[i])>>8);
    (void)fprintf(fp, "                            Seg:%d  Col:%d ", i, col);
    for(j=0; j<ni[i]; j++, k++)
      {
      double     xp=instr[k].x;
      double     yp=instr[k].y;
      (void)fprintf(fp, "(%.2f,%.2f) ", xp, yp);
      }
    (void)fprintf(fp, "\n");
    }
  }
  
    
void CEntInView::Qtransform(double xscl, double yscl, double xoff, double yoff)
  {
  if(ninstr==0)
    return;

  int     i, j, k;

  for(i=0, k=0; i<nni; i++)
    {
    for(j=0; j<ni[i]; j++, k++)
      {
      instr[k].x=(float)((instr[k].x*xscl)+xoff);
      instr[k].y=(float)((instr[k].y*yscl)+yoff);
      }
    }
  }

void CEntInView::SetNextValue(CEntInView* ptr)
  {
  nxt=ptr;
  }

CEntInView* CEntInView::GetNextValue()
  {
  return nxt;
  }

void CEntInView::SetPrevValue(CEntInView* ptr)
  {
  prv=ptr;
  }

CEntInView* CEntInView::GetPrevValue()
  {
  return prv;
  }

REAL CEntInView::SegmentProximityTest(int x, int y, flag &InBox)
  {
  REAL parm;
  REAL distance;
  REAL dtest=1.0e30;
  REAL ptest;
  Pt_2f p;

  // CNM

  float     xmin=1.0e30f;
  float     xmax= -1.0e30f;
  float     ymin=1.0e30f;
  float     ymax= -1.0e30f;
  p.Set((REAL)x, REAL(y));
  if(ninstr==0)
    return 1.0e300;
  int     i, j, k;

  for(i=0, k=0; i<nni; i++)
    {
    for(j=0; j<ni[i]; j++, k++)
      {
      if(k>0)
        {
        xmin=Min(xmin, instr[k].x);
        ymin=Min(ymin, instr[k].y);
        xmax=Max(xmax, instr[k].x);
        ymax=Max(ymax, instr[k].y);
        }
      if(j>0)
        {
        Line_2f l;
        l.Set(instr[k -1].x, instr[k -1].y, instr[k].x, instr[k].y);
        parm=l.PerpParm(p, distance);
        if(parm<0.0)
          // CNM
          distance=_hypot(instr[k -1].x -p.X, instr[k -1].y -p.Y);
        // dist to start of segment
        else if(parm>1.0)
          distance=_hypot(instr[k].x -p.X, instr[k].y -p.Y);
        // dist to end of segment
        if(dtest>distance /* CNM && parm >= 0.0 && parm <= 1.0 */ )
          {
          dtest=distance;
          ptest=parm;
          }
        }
      }
    }
  InBox=InRange(xmin, (float)x, xmax)&&InRange(ymin, (float)y, ymax);
  return dtest;
  }

REAL CEntInView::SegmentClosest(int x, int y, REAL &xp, REAL &yp)
  {
  int     boxtestsize=50;

  REAL parm;
  REAL distance;
  REAL dtest=1.0e30;
  REAL ptest;
  Pt_2f p;
  p.Set((REAL)x, REAL(y));
  if(ninstr==0)
    return 1.0e300;

  int     i, j, k, kv= -1;

  for(i=0, k=0; i<nni; i++)
    {
    for(j=0; j<ni[i]; j++, k++)
      {
      if(j>0)
        {
        if((x+boxtestsize)<Min(instr[k -1].x, instr[k].x))
          continue;
        if((y+boxtestsize)<Min(instr[k -1].y, instr[k].y))
          continue;
        if((x-boxtestsize)>Max(instr[k -1].x, instr[k].x))
          continue;
        if((y-boxtestsize)>Max(instr[k -1].y, instr[k].y))
          continue;
        Line_2f l;
        l.Set(instr[k -1].x, instr[k -1].y, instr[k].x, instr[k].y);
        parm=l.PerpParm(p, distance);
        if(parm<0.0)
          {
          // CNM
          distance=_hypot(instr[k -1].x -p.X, instr[k -1].y -p.Y);
          // dist to start of segment
          parm=0.0;
          }
        else if(parm>1.0)
          {
          distance=_hypot(instr[k].x -p.X, instr[k].y -p.Y);
          // dist to end of segment
          parm=1.0;
          }
        if(dtest>distance /* CNM && parm >= 0.0 && parm <= 1.0*/ )
          {
          dtest=distance;
          ptest=parm;
          kv=k;
          }
        }
      }
    }
  if(kv!= -1&&dtest<1.0e29)
    {
    xp=instr[kv -1].x +ptest *(instr[kv].x -instr[kv -1].x);
    yp=instr[kv -1].y +ptest *(instr[kv].y -instr[kv -1].y);
    }
  return dtest;
  }

// assumes x1 < x2 y1 < y2
int CEntInView::SegmentInBox(int x1, int y1, int x2, int y2)
  {
  if(ninstr==0)
    return 0;

  int     i, j, k;

  for(i=0, k=0; i<nni; i++)
    {
    for(j=0; j<ni[i]; j++, k++)
      {
      if(instr[k].x<x1)
        return(0);
      if(instr[k].x>x2)
        return(0);
      if(instr[k].y<y1)
        return(0);
      if(instr[k].y>y2)
        return(0);
      }
    }
  return 1;
  }

int CEntInView::SegmentInCrossBox(int x1, int y1, int x2, int y2)
  {
  if(ninstr==0)
    return 0;
  if(SegmentInBox(x1, y1, x2, y2))
    return 1;
  return(SegmentCrossBox(x1, y1, x2, y2));
  }
    
// assumes x1 < x2 y1 < y2
int CEntInView::SegmentCrossBox(int x1, int y1, int x2, int y2)
  {
  if(ninstr==0)
    return 0;

  int     i, j, k;

  for(i=0, k=0; i<nni; i++)
    {
    for(j=0; j<ni[i]; j++, k++)
      {
      if(j)
        {
        REAL p1, p2;
        Line_2f b1, b2, b3, b4;
        b1.Set(x1, y1, x1, y2);
        b2.Set(x1, y2, x2, y2);
        b3.Set(x2, y2, x2, y1);
        b4.Set(x2, y1, x1, y1);
        Line_2f l;
        l.Set(instr[k -1].x, instr[k -1].y, instr[k].x, instr[k].y);
        if(l.LineSegmentCross(l, b1, p1, p2)||l.LineSegmentCross(l, b2, p1, p2)||l.LineSegmentCross(l, b3, p1, p2)||l.LineSegmentCross(l, b4, p1, p2))
          {
          return 1;
          }
        }
      }
    }
  return 0;
  }
