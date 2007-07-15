//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
    

#include "stdafx.h"
#define  __GRF3DSP_CPP
#include "sc_defs.h"
#include "drwhdr.h"
#include "grfwnd.h"
//#include "optoff.h"

#define  dbgButtonUpDn   0
#define  dbgGrfDsp       0
#define  dbgGrfDspOC     0
#define  dbgGrfZoom      0
#define  dbgdxfmem       0

//===========================================================================
    
Viewport::Viewport(ViewDirections VwDir, double ulx, double uly, double lrx, double lry, pDXF_Drawing DrwIn)
  {
  SelectOnEntity = 0;
  Prv=NULL;
  Nxt=NULL;
  Drw=DrwIn;
  PT3 p;
  c3v_set_zero(p);
  c3a_box_init_pt(&ZoomBox, p);
  ZoomType = Zoom_Page;
  disp_list = NULL;
  dynm_list = NULL;
  zoom_xscale  = 1.0;
  zoom_yscale  = 1.0;
  zoom_xoffset = 0.0;
  zoom_yoffset = 0.0;
  Curseg_level = -1;

  m_bUpdateOnly=false;
  m_bColoursChgd=false;

  switch (VwDir)
    {
    case Vw_Top: vp=vp3_top(0.,0.,0., 1.,1., ulx, uly, lrx, lry, GrfHelper.GR_BACKGROUND /*WHITE*/, GrfHelper.GR_BACKGROUND); break;
    case Vw_Front: vp=vp3_front(0.,0.,0., 1.,1., ulx, uly, lrx, lry, GrfHelper.GR_BACKGROUND /*WHITE*/, GrfHelper.GR_BACKGROUND); break;
    default : vp=NULL;
    }
  }

//---------------------------------------------------------------------------
    
Viewport::~Viewport()
  {
  if (disp_list)
    { 
    CEntInView* pcur = disp_list, *pdel;
    while( pcur )
      {
      pdel = pcur;
      pcur = pcur->GetNextValue();
      DelDisplaySegment(pdel); // this shuld do the dynm_list as well
      }
    }
  //if (dynm_list)
  //  {
  //  TRACE0("Viewport dynamic segments not deleted\n");
  //  }
  //
  //if( disp_list ){
  //
  //   CEntInView* p = disp_list;
  //   CEntInView* tmp;
  //
  //
  //   do{
  //      tmp = p;
  //      p = p->GetNextValue();
  //      delete tmp;
  //
  //   }while(p);
  //}
  if (vp)
    vpi_free(vp);
  }

//---------------------------------------------------------------------------

void Viewport::Paint(RECT &Rect, PAINTSTRUCT& PaintInfo)
  {
  if (Drw)
    {
    #if dbgGrfDsp
    dbgp("Viewport::Paint");
    #endif
    const float w = (float)(Rect.right - Rect.left);
    const float h = (float)(Rect.bottom - Rect.top);
    if (w>0.0 && h>0.0)
      {
      C3_BOX_S SubRgn;
      Pt_3f p((PaintInfo.rcPaint.left - Rect.left)/w, 1.0-((PaintInfo.rcPaint.bottom - Rect.top)/h), 0.0);
      c3a_box_init_pt(&SubRgn, p.p());
      p.Set((PaintInfo.rcPaint.right - Rect.left)/w, 1.0-((PaintInfo.rcPaint.top - Rect.top)/h), 1.0);
      c3a_box_append_pt(&SubRgn, p.p());

      if (vp)
        Drw->Draw(this, &SubRgn, NULL);
      }

    #if dbgGrfDsp
    dbgpln(".");
    #endif
    }
  }

//---------------------------------------------------------------------------

void Viewport::ApplyDeltaMatrix()
  {
  CEntInView* p = disp_list;

  if( !p )
    return;
  do
    {
    p->Qtransform(zoom_xscale,zoom_yscale,zoom_xoffset,zoom_yoffset);
    }
  while((p = p->GetNextValue()));
  //PrintDisplayList();
  //PrintDynamicList();
  }

//---------------------------------------------------------------------------

void Viewport::DeSelectEntity(CEntInView* pEnt)
  {
  if (pEnt)
    {
    pEnt->ClrMarkBit();
    pEnt->Qdisplay();
    }
  }

//---------------------------------------------------------------------------

CEntInView* Viewport::SelectEntity(CEntInView* best)
  {
  if (!disp_list)
    return NULL;
  MarkCount++;
  if (best)
    {
    best->SetMarkBit();
    best->Qhighlight();
    }
  return best;
  }
    
//---------------------------------------------------------------------------

CEntInView* Viewport::FindEntInView4Entity(DXF_ENTITY e)
  {
  if (disp_list==NULL)
    return NULL;

  DWORD CW=FPP_DisableExceptions();

  CEntInView* p = disp_list;
  do
    {
    if( p->EntityPtr()==e)
      break;
    }
  while((p = p->GetNextValue()));
  
  FPP_RestoreExceptions(CW);
  return p;
  }

//---------------------------------------------------------------------------

CEntInView* Viewport::SelectClosestEntity(Pt_SLW pt, BOOL& AlreadySelected, double MaxDist/*=1.0e30*/)
  {
  AlreadySelected = false;
  if (!disp_list)
    return NULL;
  double mdis = 1.0e30;
  double dis;
  long closest = 1000000;
  int xpos = pt.Logical[0];
  int ypos = pt.Logical[1];
  CEntInView* p = disp_list;
  CEntInView* best = NULL;
  flag InBox=false;

  DWORD CW=FPP_DisableExceptions();//IGF to prevent Line_2f::PerpParm crashing due to infinite "len"
  do
    {
    if( EntityIsSelectable(p->EntityPtr(), p->CurvePtr()))
      {
      dis = (p->SegmentProximityTest(xpos, ypos, InBox));
      if (dis < mdis)
        {
        if (p->AllowBoxSelect() && InBox)
          {
          best = p;
          mdis = Min(mdis, dis);
          }
        else if (dis <= MaxDist)
          {
          best = p;
          mdis = dis;
          }
        }
      }
    }
  while((p = p->GetNextValue()));

  MarkCount++;
  if( best )
    {
    if (best->IfMarkBit())
      AlreadySelected = true;
    best->SetMarkBit();
    best->Qhighlight();
    }
  FPP_RestoreExceptions(CW);
  return best;
  }

//---------------------------------------------------------------------------

CEntInView* Viewport::SelectClosestEntity(Pt_SLW pt, double MaxDist/*=1.0e30*/)
  {
  BOOL AlreadySelected;
  return SelectClosestEntity(pt, AlreadySelected, MaxDist);
  }
    
//---------------------------------------------------------------------------

int Viewport::SelectClosestEntities(Pt_SLW pt, CEntInView** pEntLst, double* DistList, int MaxCnt, double MaxDist/*=1.0e30*/)
  {
  if (!disp_list || !pEntLst || MaxCnt<1)
    return NULL;

  //this routine can be made neater/more efficient......
  double mdis = 1.0e30;
  double dis;
  long closest = 1000000;
  int xpos = pt.Logical[0];
  int ypos = pt.Logical[1];
  CEntInView* p = disp_list;
  CEntInView* best = NULL;
  int Cnt = 0;
  flag InBox;

  DWORD CW=FPP_DisableExceptions();//IGF to prevent Line_2f::PerpParm crashing due to infinite "len"
  do
    {
    if( EntityIsSelectable(p->EntityPtr(), p->CurvePtr()))
      {
      dis = (p->SegmentProximityTest(xpos, ypos, InBox));
      flag AddIt = false;
      if (dis < mdis)
        {
        if (p->AllowBoxSelect() && InBox)
          {
          AddIt = true;
          }
        else if (dis <= MaxDist)
          {
          AddIt = true;
          }
        }
      if (AddIt)
        {
        if (Cnt<MaxCnt)
          {
          pEntLst[Cnt] = p;
          DistList[Cnt] = dis;
          Cnt++;
          }
        else
          {
          double mmdis = DistList[0];
          int mmpos = 0;
          for (int j=1; j<Cnt; j++)
            {
            if (DistList[j]>mmdis)
              {
              mmdis = DistList[j];
              mmpos = j;
              }
            }
          pEntLst[mmpos] = p;
          DistList[mmpos] = dis;

          mmdis = DistList[0];
          mmpos = 0;
          for (j=1; j<Cnt; j++)
            {
            if (DistList[j]>mmdis)
              {
              mmdis = DistList[j];
              mmpos = j;
              }
            }
          mdis = DistList[mmpos];
          }
        }
      }
    }
  while ((p = p->GetNextValue()));
  FPP_RestoreExceptions(CW);

  if (Cnt>1)
    {//sort list...
    for (int i=0; i<Cnt; i++)
      {
      int MinPos = -1;
      double MinVal = DistList[i];
      for (int j=i+1; j<Cnt; j++)
        {
        if (DistList[j]<MinVal)
          {
          MinPos = j;
          MinVal = DistList[j];
          }
        }
      if (MinPos>=0)
        {
        CEntInView* p = pEntLst[i];
        dis = DistList[i];
        pEntLst[i] = pEntLst[MinPos];
        DistList[i] = DistList[MinPos];
        pEntLst[MinPos] = p;
        DistList[MinPos] = dis;
        }
      }
    }

  /*MarkCount++;
  if( best )
    {
    best->SetMarkBit();
    best->Qhighlight();
    }*/
  return Cnt;
  }

//---------------------------------------------------------------------------

void Viewport::SelectAssocEntities()
  {
  CEntInView*  p = FirstSelectedEntity();
  while (p)
    {
    DXF_ENTITY e=p->EntityPtr();
    // Only Move AssocTag rel to Tag - NOT the reverse
    //char * pAssocTag=Find_Attr_Value(e, AssocTagAttribStr);
    char * pAssocTag=NULL;

    if (pAssocTag)
      {
      for (CEntInView* p1 = disp_list; p1; p1=p1->GetNextValue())
        {
        DXF_ENTITY e1=p1->EntityPtr();
        char * pTag = e1!=NULL ? Find_Attr_Value(e1, "Tag") : NULL;

        if (pTag && stricmp(pAssocTag, pTag)==0)
          SelectEntity(p1);
        }
      }
    else 
      {
      char * pTag=Find_Attr_Value(e, "Tag");
      if (pTag)
        {
        for (CEntInView* p1 = disp_list; p1; p1=p1->GetNextValue())
          {
          DXF_ENTITY e1=p1->EntityPtr();
          char * pAssocTag = e1!=NULL ? Find_Attr_Value(e1, AssocTagAttribStr) : NULL;

          if (pAssocTag && stricmp(pAssocTag, pTag)==0)
            SelectEntity(p1);
          }
        }
      }
    p=NextSelectedEntity();
    }
  };

//---------------------------------------------------------------------------

CEntInView* Viewport::SelectLocOnEntity(Pt_SLW pt,PT3 pos)
  {
  double      mdis = 1.0e30;
  double      dis;
  long        closest = 1000000;
  int         xpos    = pt.Logical[0];
  int         ypos    = pt.Logical[1];
  CEntInView*  p       = disp_list;
  CEntInView*  best;
  flag InBox;

  if( !p )
    return NULL;
  do
    {
    if( EntityIsSelectable(p->EntityPtr(),p->CurvePtr()))
      {
      dis = (p->SegmentProximityTest(xpos,ypos, InBox));
      if (dis < mdis)
        {
        best = p;
        mdis = dis;
        }
      }
    }
  while((p = p->GetNextValue()));
  MarkCount++;
  //best->SetMarkBit();
  //best->Qhighlight();

  // Get the closest point in screen coordinates
  // This stuff from here only works in 2D
  REAL xp,yp;
  if( best->SegmentClosest(xpos,ypos,xp,yp) < 1000.0 )
    {
    PT2 a;
    a[0] = xp;
    a[1] = yp;
    vp3_pixel_to_point_real(a,vp,pos);
    Pt_SLW pdig;
    pdig.Logical.X = (int)xp;
    pdig.Logical.Y = (int)yp;
    Drw->pDsp->Add_Dig_Point_List(pdig);
    return best;
    }
  return NULL;
  }
    
//---------------------------------------------------------------------------
    
CEntInView* Viewport::LastSelectedEntity()
  {
  if (!disp_list)
    return ((CEntInView*)NULL);
  CEntInView* xxx;
  cur = disp_list;
  while (cur = cur->GetNextValue())
    xxx = cur;
  while (xxx && xxx->IfMarkBit())
    xxx = xxx->GetPrevValue();
  cur = xxx;
  curprv = cur->GetPrevValue();
  curnxt = cur->GetNextValue();
  return cur;
  }
    
//---------------------------------------------------------------------------

CEntInView* Viewport::FirstSelectedEntity()
  {
  if( !disp_list )
    return((CEntInView*)NULL);
  cur = disp_list;
  curnxt = disp_list->GetNextValue();
  curprv = disp_list->GetPrevValue();

  if( cur->IfMarkBit() )
     return cur;

  return NextSelectedEntity();
  }
    
//---------------------------------------------------------------------------

CEntInView* Viewport::NextSelectedEntity()
  {
  cur = curnxt;
  if( !cur )
    return(cur);
  do
    {
    if( cur->IfMarkBit() )
      break;
    }
  while( cur = cur->GetNextValue());
  if (cur)
    {
    curprv = cur->GetPrevValue();
    curnxt = cur->GetNextValue();
    }
  return cur;
  }
    
//---------------------------------------------------------------------------

CEntInView* Viewport::PrevSelectedEntity()
  {
  cur = curprv;
  if( !cur )
    return cur;
  do
    {
    if( cur->IfMarkBit() )
      break;
    }
  while( cur = cur->GetPrevValue());
  if (cur)
    {
    curprv = cur->GetPrevValue();
    curnxt = cur->GetNextValue();
    }
  return cur;
  }
    
//---------------------------------------------------------------------------

void Viewport::ClearAllEntity()
  {
  MarkCount = 0;
  CEntInView* p       = disp_list;

  if( !p )
    return;
  do
    {
    if (p->IfMarkBit())
      {
      MarkCount--;
      p->ClrMarkBit();
      p->Qdisplay();
      }
    }
  while((p = p->GetNextValue()));

  return;
  }
    
//---------------------------------------------------------------------------

void Viewport::SelectEntitiesInBox(Pt_SLW start,Pt_SLW end)
  {
  int          x1    = start.Logical[0];
  int          y1    = start.Logical[1];
  int          x2    = end.Logical[0];
  int          y2    = end.Logical[1];
  CEntInView* p       = disp_list;


  if (x1 > x2)
    {
    int tmp = x1; x1 = x2; x2 = tmp;
    }
  if (y1 > y2)
    {
    int tmp = y1; y1 = y2; y2 = tmp;
    }
  if( !p )
    return;
  do
    {
    if( EntityIsSelectable(p->EntityPtr(),p->CurvePtr()))
      {
      if (p->SegmentInBox(x1, y1, x2, y2))
        {
        MarkCount++;
        p->SetMarkBit();
        p->Qhighlight();
        }
      }
    }
  while((p = p->GetNextValue()));
  return;
  }
    
//---------------------------------------------------------------------------

void Viewport::SelectEntitiesInCrossBox(Pt_SLW start,Pt_SLW end)
  {
  int          x1    = start.Logical[0];
  int          y1    = start.Logical[1];
  int          x2    = end.Logical[0];
  int          y2    = end.Logical[1];
  CEntInView* p       = disp_list;
  //CEntInView* best;

  if (x1 > x2)
    {
    int tmp = x1; x1 = x2; x2 = tmp;
    }
  if (y1 > y2)
    {
    int tmp = y1; y1 = y2; y2 = tmp;
    }
  if( !p )
    return;
  do
    {
    if( EntityIsSelectable(p->EntityPtr(),p->CurvePtr()))
      {
      if (p->SegmentInCrossBox(x1, y1, x2, y2))
        {
        MarkCount++;
        p->SetMarkBit();
        p->Qhighlight();
        }
      }
    }
  while((p = p->GetNextValue()));
  return;
  }
    
//---------------------------------------------------------------------------

void Viewport::RepaintDisplayList()
  {
  if (!dynm_list)
    return;
  DynamicSegment* pd = dynm_list;
  do
    {
    if (pd)
      {
      //pd->display_last = -1.0;
      DrawDynamicSegment(pd);
      }
    }
  while((pd = pd->GetNext()));
  }
    
//---------------------------------------------------------------------------
    
void Viewport::PrintDisplayList(FILE *fp)
  {
  CEntInView* p = disp_list;

  if( !p )
    return;

  do
    {
    (void)fprintf(fp,"Dlist addr:%x, n:%x, p:%x, c:%x, e:%x, num:%d, mark:%x\n",
      p,
      p->GetNextValue(),
      p->GetPrevValue(),
      p->CurvePtr(),
      p->EntityPtr(),
      p->NumPoint(),
      p->MarkBit()
    );

    }
  while( p = p->GetNextValue());
  }
    
//---------------------------------------------------------------------------

void Viewport::PrintDynamicList()
  {
  DynamicSegment* p = dynm_list;
  if( !p )
    return;
  do
    {
    TRACE1( "Dynamic Display List P %x ",p);
    TRACE1( "EntInViewP %x ",p->GetEntInView());
    TRACE1( "Tag %s ",p->GetTag());
    TRACE1( "Var %s ",p->GetVar());
    TRACE1( "Type %s ",p->GetType());
    TRACE0( "\n");
    }
  while( p = p->GetNext());
  }
    
//---------------------------------------------------------------------------
    
void Viewport::TestDynamicList()
  {
  DynamicSegment* p = dynm_list;
  if( !p )
    return;
  do
    {
    double level = (double)(rand()%1000)/1000.0;
    DrawDynamicSegment(p);
    }
  while( p = p->GetNext());
  }
    
//---------------------------------------------------------------------------
    
void Viewport::UpdateDynamicList(CEntInView *l)
  {
  DynamicSegment* p = dynm_list;
  if( !p )
    return;
  do
    {
    if( !p->GetEntInView())
      p->SetEntInView(l);
    }
  while( p = p->GetNext());
  }

//---------------------------------------------------------------------------
    
void Viewport::AddDisplaySegment(CEntInView* ptr)
  {
  ptr->SetPrevValue((CEntInView*)NULL);
  if( disp_list )
    disp_list->SetPrevValue(ptr);
  ptr->SetNextValue(disp_list);
  disp_list = ptr;
  }
    
//---------------------------------------------------------------------------

void Viewport::DelDisplaySegment(CEntInView* ptr)
  {
  // check and see if this is on the dynamic list
  DelDynamicSegment(ptr);
  CEntInView* p = ptr->GetPrevValue();
  CEntInView* n = ptr->GetNextValue();

  if (! p && n)
    {
    disp_list = n;
    n->SetPrevValue((CEntInView*)NULL);
    }
  else if (! n && p)
    {
    p->SetNextValue((CEntInView*)NULL);
    }
  else if (n && p)
    {
    p->SetNextValue(n);
    n->SetPrevValue(p);
    }
  else
    {
    disp_list = (CEntInView*)NULL;
    }
  delete ptr;
  }

//---------------------------------------------------------------------------

flag Viewport::EntityIsSelectable(DXF_ENTITY e,C3_CURVE c)
  {
  int type=0;
  pchar layr=0;
  ASSERT(e || c);

  if( c )
    {
    if( C3_CURVE_IS_LINE(c)   )type = DXF_LINE;       
    if( C3_CURVE_IS_ARC(c)    )type = DXF_ARC;       
    if( C3_CURVE_IS_PCURVE(c) )type = DXF_POLYLINE;       
    //if( C3_CURVE_IS_ELLIPSE(c))type = DXF_ELIPSE;       
    //if( C3_CURVE_IS_SPLINE(c) )type = DXF_SPLINE;  
    layr = C3_CURVE_LAYER_GET(c);     
    }
  else if( e )
    {
    type = DX_ENTITY_ID(e);
    layr = DXF_ENTITY_LAYER_GET(e);
    }
  //ASSERT(type);
  ASSERT(layr);

  //TRACE("Selecting type:%d layr:%s SelOnEnt:%d CGrfLayer.Length:%d Tag.Length:%d",
  //                         type,layr,SelectOnEntity,SelectOnLayer.Length(),SelectOnAttrTag.Length());
  // begin testing 
  if( SelectOnEntity == 0 && SelectOnLayer.Length() == 0 && SelectOnAttrTag.Length() == 0) 
    return(1);
  if( SelectOnEntity && !(SelectOnEntity & type))
    return(0);
  if( SelectOnLayer.Length() && !SelectOnLayer.Find(layr))
    return(0);
  if( SelectOnAttrTag.Length() && (type!=DXF_INSERT || e==NULL))
    return(0);
  if( SelectOnAttrTag.Length() )
    {
    pStrng p = SelectOnAttrTag.First();
    while( p )
      {
      if( (*p)() && Find_Attr(e,(*p)()) )
        break;
      p = SelectOnAttrTag.Next();
      }
      if( !p )
        return 0;
    }
  return(1);
  }
    
//---------------------------------------------------------------------------

void Viewport::ClrSelectionAllList()
  {
  ClrSelectionEntityList();
  ClrSelectionLayerList();
  ClrSelectionAttribList();
  }

//---------------------------------------------------------------------------

void Viewport::DrawDynamicSegment(DynamicSegment* ptr)
  {
  double v = ptr->GetHumanValue(ptr->GetSIValue());
  v = ptr->GetSclValue(v);
  if (ptr->UseFillColor())
    ptr->GetEntInView()->SetFillColorRqd(ptr->GetDispCol());
  ptr->GetEntInView()->Qdisplay(ptr->GetSeg(), v, /*ptr->display_dat*/0.0, atoi((const char *)(ptr->GetType())));
  ptr->SetLastSclValue(v);
  }

//---------------------------------------------------------------------------

void Viewport::DrawDynamicSegment(DynamicSegment* ptr, double val)
  {
  ptr->SetSIValue(val);
  double v = ptr->GetHumanValue(ptr->GetSIValue());
  v = ptr->GetSclValue(v);
  if (fabs( v - ptr->GetLastSclValue()) > 0.0005)
    {
    if (ptr->UseFillColor())
      ptr->GetEntInView()->SetFillColorRqd(ptr->GetDispCol());
    ptr->GetEntInView()->Qdisplay(ptr->GetSeg(), v, /*ptr->display_dat*/0.0, atoi((const char *)(ptr->GetType())));
    ptr->SetLastSclValue(v);
    }
  }

//---------------------------------------------------------------------------

void Viewport::AddDynamicSegment(DynamicSegment* ptr)
  {
  ptr->SetPrev(NULL);
  if (dynm_list)
    dynm_list->SetPrev(ptr);
  ptr->SetNext(dynm_list);
  dynm_list = ptr;
  }

//---------------------------------------------------------------------------

DynamicSegment* Viewport::FindDynamicSegment(pchar Tag, pchar Var, byte DynTyp)
  {
  DynamicSegment* pDS = dynm_list;
  while (pDS)
    {
    if (DynTyp==pDS->GetDynTyp() && _stricmp(Tag, pDS->GetTag())==0 && _stricmp(Var, pDS->GetVar())==0)
      return pDS;
    pDS = pDS->GetNext();
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void Viewport::DelDynamicSegment(CEntInView* ptr)
  {
  if (!dynm_list)
    return;

  pDynamicSegment pfind = dynm_list;
  pDynamicSegment pnxt = pfind;
  while (pnxt)
    {
    pfind = pnxt;
    pnxt = pfind->GetNext();
    if (pfind->GetEntInView() == ptr)
      {
      DynamicSegment* p = pfind->GetPrev();
      DynamicSegment* n = pfind->GetNext();
      if (!p && n)
        {
        dynm_list = n;
        n->SetPrev(NULL);
        }
      else if (! n && p)
        {
        p->SetNext(NULL);
        }
      else if (n && p)
        {
        p->SetNext(n);
        n->SetPrev(p);
        }
      else
        {
        dynm_list = NULL;
        }
      if (Drw && Drw->pDsp && Drw->pDsp->TheWnd)
        {
        CGrfWnd *w = dynamic_cast<CGrfWnd*>(Drw->pDsp->TheWnd);
        if (w)
          w->DelDynamicSegment(pfind);
        }
      delete pfind;
      }
    }
  }

//---------------------------------------------------------------------------

void Viewport::Paint(RECT &Rect, CRect &UpdateRect)
  {
  if (Drw)
    {
    #if dbgGrfDsp
    dbgp("Viewport::Draw");
    #endif
    const float w = (float)(Rect.right - Rect.left);
    const float h = (float)(Rect.bottom - Rect.top);
    if (w>0.0 && h>0.0)
      {
      Pt_3f p((UpdateRect.left - Rect.left)/w, 1.0-((UpdateRect.bottom - Rect.top)/h), 0.0);
      C3_BOX_S SubRgn;
      c3a_box_init_pt(&SubRgn, p.p());
      p.Set((UpdateRect.right - Rect.left)/w, 1.0-((UpdateRect.top - Rect.top)/h), 1.0);
      c3a_box_append_pt(&SubRgn, p.p());

      #if dbgdxfmem
      DWORD s = FreeVirtualMemory();
      #endif

      if (vp)
        Drw->Draw(this, &SubRgn, NULL);

      //ApplyDeltaMatrix();
      RepaintDisplayList();

      #if dbgdxfmem
      DWORD e = FreeVirtualMemory();
      dbgpln("Memory delete for drawing display list %dk",(s-e)/1024);
      #endif
      }
    #if dbgGrfDsp
    dbgpln(".");
    #endif
    }
  }

//---------------------------------------------------------------------------

CEntInView* Viewport::Draw(DXF_ENTITY e, int Colour)
//void Viewport::Draw(DXF_ENTITY e, int Colour)
  {
  #if dbgGrfDsp
  dbgp("Viewport::Draw");
  #endif
  if (vp && e )
    {
    CGrfLayer * l = Drw->Find(DXF_ENTITY_LAYER_GET(e));
    DML_ITEM item = Drw->FindItem(e);
    ASSERT(l);
    Drw->Draw(e, this, &ZoomBox, Colour, 1, l, l, item);
    return (CEntInView*)(item->pKenwalta);
    }

  #if dbgGrfDsp
  dbgpln(".");
  #endif
  return NULL;
  }

//---------------------------------------------------------------------------

void Viewport::Draw(C3_CURVE c, int Colour)
  {
  #if dbgGrfDsp
  dbgp("Viewport::Draw");
  #endif
  if (vp)
    {
    CGrfLayer * l = Drw->Find(C3_CURVE_LAYER_GET(c));
    DML_ITEM item = Drw->FindItem(c);
    ASSERT(l);
    Drw->Draw(c, this, &ZoomBox, Colour,1,l,l,item);
    }
  #if dbgGrfDsp
  dbgpln(".");
  #endif
  }

//===========================================================================
    
Grf3D_Display::Grf3D_Display(int XAspect, int YAspect)
  {
  TheWnd=NULL;
  m_XAspect = XAspect;
  m_YAspect = YAspect;
  CursImgLen=0;
  Dig_Point_List.SetSize(0, 16);
  bPrinting = 0;
  CursEnt=NULL;
  m_XHairOn=0;
//      iSrcIOIndex = -1;
//      iDstIOIndex = -1;
  }

//---------------------------------------------------------------------------

Grf3D_Display::Grf3D_Display(CWnd* TheWndIn, int XAspect, int YAspect)
  {
  TheWnd = TheWndIn;
  m_XAspect = XAspect;
  m_YAspect = YAspect;

  CursImgLen=0;
  Vp1 = NULL;

  ButtonDown = 0;
  DoPrimaryInit=1;
  Opens  = 0;
  OwnsDC[Opens]=0;
  HDC_List[Opens]=NULL;
  HDC_Prev[Opens]=NULL;
  COLOR_Prev[Opens]=-1;

  Cursor = GC_NoCurs;
  CursEnt=NULL;

  my_HDC = NULL;

  m_XHairOn=0;
  Dig_Point_List.SetSize(0, 16);
  }

//---------------------------------------------------------------------------

Grf3D_Display::~Grf3D_Display()
  {
  while (Vp1)
    {
    pViewport p=Vp1;
    Vp1=Vp1->Nxt;
    delete p;
    }
  Dig_Point_List.RemoveAll();
  }
    
//---------------------------------------------------------------------------

flag Grf3D_Display::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  return Vp1->Drw->ChangeTag(pOldTag, pNewTag);
  }

//---------------------------------------------------------------------------

void Grf3D_Display::CollectTags(Strng_List & TagList)
  {
  Vp1->Drw->CollectTags(TagList);
  }

//---------------------------------------------------------------------------

flag Grf3D_Display::FindTag(LPSTR Tag, DXF_ENTITY &pLastMatchingInsert, DXF_ENTITY &pLastMatchingAttr)
  {
  return Vp1->Drw->FindTag(Tag, pLastMatchingInsert, pLastMatchingAttr);
  }

//---------------------------------------------------------------------------

void Grf3D_Display::Open(CDC* pDC, CRect * pDspRect)
  {
  Sect.Lock();

  Opens++;
  #if dbgGrfDspOC
  dbgpln("Grf3D_Display::Open  %4i %08p, %8i  %#10x %#10x %#10x", Opens, this, GetCurrentThreadId(), TheWnd, dynamic_cast<CGrfWnd*>(TheWnd), pDC);
  #endif

  grr_get_context(&HDC_Prev[Opens], &COLOR_Prev[Opens]);

  OwnsDC[Opens] = (pDC==NULL);
  HDC_List[Opens] = (OwnsDC[Opens] ? GetDC(TheWnd->m_hWnd) : pDC->m_hDC);
  my_HDC = HDC_List[Opens];

  CGrfWnd *pGrfWnd=dynamic_cast<CGrfWnd*>(TheWnd);
  bPrinting = ((pDC && pDC->IsPrinting()) || (pGrfWnd && pGrfWnd->bPretendPrinting));
  if (pDspRect)
    Rect = *pDspRect;
  else if (bPrinting  && pGrfWnd && !pGrfWnd->bPretendPrinting)
    Rect = PrintRect;
  else
    TheWnd->GetClientRect(&Rect);

  if (0)
    {
    double WndAspect=Rect.bottom/Max(1.0, (double)Rect.right); 
    double PgAspect=m_YAspect/(double)m_XAspect;
    double XScl=1, YScl=1;

    if (PgAspect<WndAspect)
      YScl=WndAspect/PgAspect;
    else
      XScl=PgAspect/WndAspect;

    SetMapMode(my_HDC, MM_ANISOTROPIC);
    SIZE Size;
    SetWindowExtEx(my_HDC, int(2*m_XAspect*XScl), int(2*m_YAspect*YScl), &Size);
    POINT Pt;
    SetViewportOrgEx(my_HDC, 0, 0, &Pt);
    SetViewportExtEx(my_HDC, Rect.right, Rect.bottom, &Size);
    }
  else
    {
    SetMapMode(my_HDC, MM_ISOTROPIC);
    SIZE Size;
    SetWindowExtEx(my_HDC, 2*m_XAspect, 2*m_YAspect, &Size);
    POINT Pt;
    SetViewportOrgEx(my_HDC, 0, 0, &Pt);
    SetViewportExtEx(my_HDC, Rect.right, Rect.bottom, &Size);
    }

  #if dbgGrfDspOC
  dbgpln("                     %4i %4i %8i %8i %10x", m_XAspect, m_YAspect, Rect.right, Rect.bottom, my_HDC);
  #endif

  if (bPrinting)
    {
    hOldPen   = (HPEN)SelectObject(my_HDC, GetStockObject(BLACK_PEN));
    hOldBrush = (HBRUSH)SelectObject(my_HDC, GetStockObject(WHITE_BRUSH));
    }
  else
    {
    hOldPen   = (HPEN)SelectObject(my_HDC, GetStockObject(WHITE_PEN));
    hOldBrush = (HBRUSH)SelectObject(my_HDC, (HBRUSH)(*GrfHelper.pBrush));
    }

//?  DP_to_Screen_LogicalPix(Msg, my_HDC, vp, CurrentPt.Screen, CurrentPt.Logical, &Cur_VP, CurrentPt.World);

  grr_set_context(my_HDC, -1);
  grr_printing(bPrinting);
  grr_init(GR_DETECT, GrfHelper.GR_BACKGROUND, DoPrimaryInit);
  DoPrimaryInit=0;

  if (CurrentView() == NULL)
    {
    AddView(Vw_Top, 0., 0., 1., 1., NULL);
    //AddView(Vw_Front, 0., 0., 1., 1., NULL);
    Show();
    }
  }

//---------------------------------------------------------------------------
    
void Grf3D_Display::Close()
  {
  #if dbgGrfDspOC
  dbgp("Grf3D_Display::Close %4i %08p, %8i", Opens, this, GetCurrentThreadId());
  #endif
  
  bPrinting = 0;
  SelectObject(my_HDC, hOldPen);
  SelectObject(my_HDC, hOldBrush);
  if (OwnsDC[Opens])
    ReleaseDC(TheWnd->m_hWnd, my_HDC);

  grr_set_context(HDC_Prev[Opens], COLOR_Prev[Opens]);

  #if dbgGrfDspOC
  dbgp(" %10x", HDC_Prev[Opens]);
  #endif

  Opens--;
  my_HDC = HDC_List[Opens];
  //grr_context(my_HDC);
  grr_printing(bPrinting);

  #if dbgGrfDspOC
  dbgpln(" %10x", my_HDC);
  #endif

  Dig_Point_List.SetSize(0, 16);
  
  Sect.Unlock();
  }
  
//---------------------------------------------------------------------------

void Grf3D_Display::Paint(PAINTSTRUCT& PaintInfo)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::Paint %p",this);
  #endif

  pViewport v = Vp1;
  while (v)
    {
    //if (!bPrinting || v!=Vp1)
      v->Show();
    v->Paint(Rect, PaintInfo);
    v=v->Nxt;
    }
  }

//---------------------------------------------------------------------------

void Grf3D_Display::SetUpdateFlags(flag UpdOnly, flag ColoursChgd)
  {
  pViewport v = Vp1;
  while (v)
    {
    v->m_bUpdateOnly=UpdOnly;
    v->m_bColoursChgd=ColoursChgd;
    v=v->Nxt;
    }
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Paint(CRect &UpdateRect)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::Paint %p %i,%i  %i,%i",this,
         UpdateRect.left,UpdateRect.top,UpdateRect.right,UpdateRect.bottom);
  #endif

  pViewport v = Vp1;
  while (v)
    {
    if ((!bPrinting || v!=Vp1) && !v->m_bUpdateOnly)
      v->Show();
    v->Paint(Rect, UpdateRect);
    v=v->Nxt;
    }
  if (m_XHairOn)
    Show_Dig_CrossHair(m_XHairPt, true, m_XHairXOR);
  }

//---------------------------------------------------------------------------

void Grf3D_Display::Draw(DXF_ENTITY e, int Colour)
  {
  pViewport v=Vp1;
  while (v)
    {
    v->Draw(e, Colour);
    v=v->Nxt;
    }
  }

//---------------------------------------------------------------------------
    
void Grf3D_Display::Draw(C3_CURVE c, int Colour)
  {
  pViewport v=Vp1;
  while (v)
    {
    v->Draw(c, Colour);
    v=v->Nxt;
    }
  }

//---------------------------------------------------------------------------

void Grf3D_Display::ReDraw(Pt_SLW p1, Pt_SLW p2)
  {
  RECT r;
  r.left=Min(p1.Logical[0], p2.Logical[0]);
  r.top= Min(p1.Logical[1], p2.Logical[1]);
  r.right=Max(p1.Logical[0], p2.Logical[0]);
  r.bottom= Max(p1.Logical[1], p2.Logical[1]);
  InvalidateRect(TheWnd->m_hWnd, &r, TRUE);
  };

//-------------------------------------------------------------------------

void Grf3D_Display::XORCursor()
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::XOR_Cursor %i", Cursor);
  #endif
  if (Cursor)
    {
    OROPMode = SetROP2(my_HDC, R2_XORPEN);
    if (Cursor & GC_BigCurs)
      {
      Big_Curs(my_HDC, EndPt.Logical.X, EndPt.Logical.Y);
      if (Cursor & GC_RectCurs)
        {
        POINT Pt;
        MoveToEx(my_HDC, StartPt.Logical.X, EndPt.Logical.Y, &Pt);
        LineTo(my_HDC, StartPt.Logical.X, StartPt.Logical.Y);
        LineTo(my_HDC, EndPt.Logical.X, StartPt.Logical.Y);
        }
      }
    else if (Cursor & GC_RectCurs)
      Rect_Curs(my_HDC, StartPt.Logical.X, StartPt.Logical.Y, EndPt.Logical.X, EndPt.Logical.Y);
    if (Cursor & GC_RubberCurs)
      Rubber_Curs(my_HDC, StartPt.Logical.X, StartPt.Logical.Y, EndPt.Logical.X, EndPt.Logical.Y);
    if (Cursor & GC_ImageCurs )
      Image_Curs(my_HDC, StartPt.Logical/*CursImgBase*/, EndPt.Logical, CursImgLen, CursImgPts);
    if (Cursor & GC_DListCurs )
      DList_Curs(my_HDC, StartPt.Logical , EndPt.Logical, CursEnt );
    SetROP2(my_HDC, OROPMode);
    }
  }

//---------------------------------------------------------------------------

void Grf3D_Display::SetCursEntity(pDXF_Drawing pDrw,DXF_ENTITY e)
  {
  CursEnt = pDrw->FindEntInView(e);
  }

void Grf3D_Display::SetCursEntity(pDXF_Drawing pDrw,C3_CURVE c)
  {
  CursEnt = pDrw->FindEntInView(c);
  }

void Grf3D_Display::SetCursImgBase(Pt_3i Pt)
  {
  CursImgBase=Pt;
  dbgpln("Image_Base %5i %5i",Pt.X,Pt.Y);
  };

void Grf3D_Display::SetCursImgBase()
  {
  CursImgBase.Set(0,0,0);
  };

//---------------------------------------------------------------------------
    
void Grf3D_Display::SetCursImgPt(int N, Pt_3i Pt)
  {
  SetCursImgPt(N, Pt.X, Pt.Y);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::SetCursImgPt(int N, int X, int Y)
  {
  if( N >= MaxCursImgPts )
     return;
  CursImgLen = Min((N==0 ? N+1 : Max(CursImgLen, N+1)), MaxCursImgPts);
  CursImgPts[N].X=X;
  CursImgPts[N].Y=Y;
  CursImgPts[N].Z=0;
  dbgpln("Image_Pt %2i %5i %5i",N,X,Y);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::ButtonDownBegin(POINT point, byte Button, Cursor_Types CursNo)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::BDB");
  #endif
  Cursor = CursNo;
  SetCurrentPt(point);
  StartPt = CurrentPt;
  EndPt = CurrentPt;
  SetCapture(TheWnd->m_hWnd);
  }

//---------------------------------------------------------------------------

flag Grf3D_Display::FetchTaggedSIValue(pchar Tag, pchar Var, double& Val, CCnvIndex  & CnvInx, Strng &CnvTxt)
  {
  Strng TagBuff;
  TagBuff.Set("%s.%s", Tag, Var);
  CXM_Route ObjRoute; 
  CXM_ObjectTag ObjTag(TagBuff(), TABOpt_AllInfo);
  CXM_ObjectData ObjData;
  if (!((CGrfWnd*)TheWnd)->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
    return False;
  CPkDataItem* pPkDI = ObjData.FirstItem();
  if (!IsNumData(pPkDI->Type()))
    return False;
  if (CnvTxt()==NULL)
    CnvTxt=pPkDI->CnvTxt();
  CnvInx=pPkDI->CnvIndex();
  Val = pPkDI->Value()->GetDouble();//CnvInx, CnvTxt());
  return True;
  }

//---------------------------------------------------------------------------

flag Grf3D_Display::ToggleTaggedSIValue(pchar Tag, pchar Var, long& Val, CCnvIndex & CnvInx, Strng &CnvTxt)
  {
  Strng TagBuff;
  TagBuff.Set("%s.%s", Tag, Var);
  CXM_Route ObjRoute; 
  CXM_ObjectTag ObjTag(TagBuff(), TABOpt_AllInfo);
  CXM_ObjectData ObjData;
  if (!((CGrfWnd*)TheWnd)->XReadTaggedItem(ObjTag, ObjData, ObjRoute))
    return False;
  CPkDataItem* pPkDI = ObjData.FirstItem();
  if (!IsNumData(pPkDI->Type()))
    return False;
  Val = !pPkDI->Value()->GetLong();//pPkDI->CnvIndex(), pPkDI->CnvTxt()));
  PkDataUnion DU;
  DU.SetTypeLong(pPkDI->Value()->Type(), Val); //send toggle
  CXM_ObjectData WriteObjData(0, 0, TagBuff(), DU);
  if (((CGrfWnd*)TheWnd)->XWriteTaggedItem(WriteObjData, ObjRoute)==TOData_OK)
    return True;
  return False;
  }
    
//---------------------------------------------------------------------------

void Grf3D_Display::LeftButtonAction()
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::Left Button Action - Toggle a dynamic block");
  #endif
  Vp1->ClrSelectionAllList();
  Vp1->AddSelectionEntityList(DXF_INSERT);
  Vp1->AddSelectionAttribList("TOGGTAGS");
  Vp1->AddSelectionAttribList("TOGGVARS");
  Vp1->SelectClosestEntity(CurrentPt);
  CEntInView* p = Vp1->FirstSelectedEntity();
  if (p && p->EntityPtr())
    {
    pchar dynmtags = Find_Attr_Value(p->EntityPtr(), "TOGGTAGS");
    pchar dynmvars = Find_Attr_Value(p->EntityPtr(), "TOGGVARS");
    if (dynmtags && dynmvars)
      {
      Strng_List TagsList;
      TagsList.AppendTokString(dynmtags, " ");
      Strng_List VarsList;
      VarsList.AppendTokString(dynmvars, " ");
      Strng Var(dynmvars);
      long Value;
      pStrng ttmp = TagsList.First();
      pStrng vtmp = VarsList.First();
      while (ttmp && vtmp)
        {
        Strng WrkVar, WrkCnvTxt;
        TaggedObject::SplitTagCnv(vtmp->Str(), WrkVar, WrkCnvTxt);
        CCnvIndex CnvInx;
        if (ToggleTaggedSIValue(ttmp->Str(), vtmp->Str(), Value, CnvInx, WrkCnvTxt))
          {
          DynamicSegment* pd = Vp1->dynm_list;
          while (pd)
            {
            if (p==pd->GetEntInView())
              {
              if (Value)
                Value = 1;
              Vp1->DrawDynamicSegment(pd, (double)Value);
              }
            pd = pd->GetNext();
            }
          }
        ttmp = TagsList.Next();
        vtmp = VarsList.Next();
        }
      }
    }
  Vp1->ClrSelectionAllList();
  Vp1->ClearAllEntity();
  }

 //---------------------------------------------------------------------------
  
void Grf3D_Display::ButtonDownEnd(POINT point, byte Button)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::BDE");
  #endif
  XORCursor();
  ButtonDown=1;
  }

//-------------------------------------------------------------------------

void Grf3D_Display::ButtonUpBegin(POINT point, byte Button)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::BUB");
  #endif
  SetCurrentPt(point);
  EndPt = CurrentPt;
  XORCursor();
  }

//-------------------------------------------------------------------------

void Grf3D_Display::ButtonUpEnd(POINT point, byte Button)
  {
  ReleaseCapture();
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::BUE");
  #endif
  ButtonDown=0;
  }

//---------------------------------------------------------------------------

void Grf3D_Display::MouseMoveBegin(POINT point)
  {
  SetCurrentPt(point);
  XORCursor();
  EndPt = CurrentPt;
  }


//---------------------------------------------------------------------------

void Grf3D_Display::MouseMoveEnd(POINT point)
  {
  XORCursor();
  }

//---------------------------------------------------------------------------

void Grf3D_Display::SetCPtWorld(Pt_3f world, Pt_SLW  &CPt)
  {
  POINT  pt;

  CPt.World.X = world.X;
  CPt.World.Y = world.Y;
  CPt.World.Z = world.Z;

  vp3_point_to_pixel(CPt.World.p(),CurrentView()->Vp(),CPt.Logical.p());
  pt.x = CPt.Logical.X;
  pt.y = CPt.Logical.Y;
  LPtoDP(my_HDC, &pt, 1);
  TheWnd->ClientToScreen(&pt);
  CPt.Screen.X = pt.x;   //This is screen relative to the window
  CPt.Screen.Y = pt.y;             
  }

//---------------------------------------------------------------------------

void Grf3D_Display::SetCurrentPtWorld(Pt_3f world)
  {
  POINT  pt;
  CurrentPt.World.X = world.X;
  CurrentPt.World.Y = world.Y;
  CurrentPt.World.Z = world.Z;

  vp3_point_to_pixel(CurrentPt.World.p(),CurrentView()->Vp(),CurrentPt.Logical.p());
  pt.x = CurrentPt.Logical.X;
  pt.y = CurrentPt.Logical.Y;
  LPtoDP(my_HDC, &pt, 1);
  TheWnd->ClientToScreen(&pt);
  CurrentPt.Screen.X = pt.x;   //This is screen relative to the window
  CurrentPt.Screen.Y = pt.y;             
  //POINT pnt;        
  //GetWindowOrgEx(my_HDC,&pnt);
  
  //CurrentPt.Screen.X += pt.x;   //This is screen relative to the window
  //CurrentPt.Screen.Y += pt.y;             
  }
  
//---------------------------------------------------------------------------
    
void Grf3D_Display::SetCurrentPt(POINT point)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::SetCurrentPt");
  #endif
// ??Cur VP

  POINT  pt;
  SIZE Size;
  int VPExtX = LOWORD(GetViewportExtEx(my_HDC, &Size));// ???? mhm
  int VPExtY = HIWORD(GetViewportExtEx(my_HDC, &Size));// ???? mhm

//TRACE("%d %d\n",Size.cx,Size.cy);


  pt.x = point.x;
  pt.y = point.y;
  
//<!   CurrentPt.Screen.X = ((double)pt.x)/VPExtX;
//<!   CurrentPt.Screen.Y = ((double)pt.y)/VPExtY;
CurrentPt.Screen.X = point.x;
CurrentPt.Screen.Y = point.y;

//CurrentPt.Screen.X = ((double)pt.x)/VPExtX;
//CurrentPt.Screen.Y = ((double)pt.y)/VPExtY;
  DPtoLP(my_HDC, &pt, 1);
  CurrentPt.Logical.X = pt.x;
  CurrentPt.Logical.Y = pt.y;

//??  *vpx = (vp ? wmi_select(Screen.p()) : NULL);
//??  if (*vpx)
  vp3_pixel_to_point(CurrentPt.Logical.p(), CurrentView()->Vp(), CurrentPt.World.p());
  }


//---------------------------------------------------------------------------
    
pViewport Grf3D_Display::AddView(ViewDirections VwDir, double ulx, double uly, double lrx, double lry, pDXF_Drawing DrwIn)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::AddView");
  #endif
  pViewport nvp = new Viewport(VwDir, ulx, uly, lrx, lry, DrwIn);
  nvp->Nxt=Vp1;
  Vp1=nvp;
  return nvp;
  };

//---------------------------------------------------------------------------

void Grf3D_Display::SetViewDrawing(pDXF_Drawing DrwIn, pViewport Vp)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::SetViewDrawing");
  #endif
  if (Vp==NULL)
    Vp=Vp1;
  Vp->Drw=DrwIn;
  };

//---------------------------------------------------------------------------

pDXF_Drawing Grf3D_Display::CurrentDrawing(pViewport Vp)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::CurrentDrawing");
  #endif
  if (Vp==NULL)
    Vp=Vp1;
  return Vp->Drw;
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Show()
  {
  Vp1->Show();
  };
    
//---------------------------------------------------------------------------

void Grf3D_Display::Show_Dig_Point(Pt_SLW &Pt, flag On, flag XOR)
  {
  const int size = 10;
  if (GrfHelper.pDigPen==NULL)
    GrfHelper.pDigPen = new CPen(PS_SOLID, 1, GrfHelper.DigPenRGB);

  int OldROPMode = SetROP2(my_HDC, (XOR ? R2_XORPEN : On ? R2_COPYPEN : R2_BLACK));
  HPEN hOldPen = (HPEN)SelectObject(my_HDC, (HPEN)(*GrfHelper.pDigPen));
  POINT LastPt;
  MoveToEx(my_HDC, Pt.Logical.X-size  ,Pt.Logical.Y-size, &LastPt);
  LineTo(my_HDC, Pt.Logical.X+size,Pt.Logical.Y+size);
  MoveToEx(my_HDC, Pt.Logical.X-size     ,Pt.Logical.Y+size, &LastPt);
  LineTo(my_HDC, Pt.Logical.X+size     ,Pt.Logical.Y-size);
  SelectObject(my_HDC, hOldPen);
  SetROP2(my_HDC, OldROPMode);
  };
    
/*
mhmlesson
AddaPt(Pt_SLW &p);

      Dig_Pts.Add(p);

ClearDigs
      Dig_pts.RemoveAll();
      Dig_pts.SetSize(0,16);

for (int i=0; Dpts.GetSize(); i_++
*/

//---------------------------------------------------------------------------

void Grf3D_Display::Add_Dig_Point_List(Pt_SLW &Pt)
  {
  Dig_Point_List.Add(Pt);
  Show_Dig_Point(Pt,True);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Show_Dig_Point_List()
  {
  for (int i=0; i < Dig_Point_List.GetSize(); i++ )
    {
    Show_Dig_Point(Dig_Point_List[i],True);
    }
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Clear_Last_Dig_Point_List()
  {
  if( Dig_Point_List.GetSize() == 0 )
    return;
  Show_Dig_Point(Dig_Point_List[Dig_Point_List.GetSize()-1],False);
  Dig_Point_List.RemoveAt(Dig_Point_List.GetSize()-1);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Clear_Dig_Point_List()
  {
   Hide_Dig_Point_List();
   Dig_Point_List.RemoveAll();
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Hide_Dig_Point_List()
  {
  for (int i=0; i < Dig_Point_List.GetSize(); i++ )
    {
    Show_Dig_Point(Dig_Point_List[i],False);
    }
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Hide_Dig_Point(Pt_SLW &Pt)
  {
  Show_Dig_Point(Pt, False);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Show_Dig_Line(Pt_SLW &Pt1, Pt_SLW &Pt2, flag On, flag XOR)
  {
  if (GrfHelper.pDigPen==NULL)
    GrfHelper.pDigPen = new CPen(PS_SOLID, 1, GrfHelper.DigPenRGB);

  int OldROPMode = SetROP2(my_HDC, (XOR ? R2_XORPEN : On ? R2_COPYPEN : R2_BLACK));
  HPEN hOldPen = (HPEN)SelectObject(my_HDC, (HPEN)(*GrfHelper.pDigPen));
  POINT LastPt;
  MoveToEx(my_HDC, Pt1.Logical.X,Pt1.Logical.Y, &LastPt);
  LineTo(my_HDC, Pt2.Logical.X,Pt2.Logical.Y);
  SelectObject(my_HDC, hOldPen);
  SetROP2(my_HDC, OldROPMode);
  };

//---------------------------------------------------------------------------
    
void Grf3D_Display::Hide_Dig_Line(Pt_SLW &Pt1, Pt_SLW &Pt2)
  {
  Show_Dig_Line(Pt1, Pt2, False);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Show_Dig_Rect(Pt_SLW &Pt1, Pt_SLW &Pt2, flag On)
  {
  if (GrfHelper.pDigPen==NULL)
    GrfHelper.pDigPen = new CPen(PS_SOLID, 1, GrfHelper.DigPenRGB);

  int OldROPMode = SetROP2(my_HDC, (On ? R2_COPYPEN : R2_BLACK));
  HPEN hOldPen = (HPEN)SelectObject(my_HDC, (HPEN)(*GrfHelper.pDigPen));
  POINT LastPt;
  MoveToEx(my_HDC, Pt1.Logical.X,Pt1.Logical.Y, &LastPt);
  LineTo(my_HDC, Pt1.Logical.X,Pt2.Logical.Y);
  LineTo(my_HDC, Pt2.Logical.X,Pt2.Logical.Y);
  LineTo(my_HDC, Pt2.Logical.X,Pt1.Logical.Y);
  LineTo(my_HDC, Pt1.Logical.X,Pt1.Logical.Y);
  SelectObject(my_HDC, hOldPen);
  SetROP2(my_HDC, OldROPMode);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Hide_Dig_Rect(Pt_SLW &Pt1, Pt_SLW &Pt2)
  {
  Show_Dig_Rect(Pt1, Pt2, False);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Show_Dig_CrossHair(Pt_SLW &Pt, flag On, flag XOR)
  {
  if (!On && !m_XHairOn)
    return;
  if (On && XOR)
    m_XHairPt=Pt;
  m_XHairOn=On;
  m_XHairXOR=XOR;

  Pt_SLW PtA, PtB;
  PtA.Logical.Set(-10000, m_XHairPt.Logical.Y, 0);
  PtB.Logical.Set( 10000, m_XHairPt.Logical.Y, 0);
  Show_Dig_Line(PtA, PtB, On, XOR);
  PtA.Logical.Set(m_XHairPt.Logical.X, -10000, 0);
  PtB.Logical.Set(m_XHairPt.Logical.X,  10000, 0);
  Show_Dig_Line(PtA, PtB, On, XOR);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::Move_Dig_CrossHair(Pt_SLW &Pt)
  {
  Show_Dig_CrossHair(m_XHairPt, false, true);
  Show_Dig_CrossHair(Pt, true, true);
  };

//---------------------------------------------------------------------------

static double gxscl,gyscl,gx0,gy0,xxscl,yyscl,xx0,yy0;

void vt_convert_init(VP_VIEWPORT vp)
  {
  PT3 pt0,pt1;
  INT ipt0[2],ipt1[2];
  pt0[0] =    0.0;  pt0[1] =    0.0;   pt0[2] = 0.0;
  pt1[0] = 1000.0;  pt1[1] = 1000.0;   pt1[2] = 0.0;
  ipt0[0] =   0; ipt0[1] =    0;
  ipt1[0] =   1; ipt1[1] =    1;
  vp3_pixel_to_point(ipt0,vp,pt0);
  vp3_pixel_to_point(ipt1,vp,pt1);
  gxscl = (pt1[0]-pt0[0]);             // units per pixel
  gyscl = (pt1[1]-pt0[1]);
  gx0   = pt0[0];                      // position of zero pixel
  gy0   = pt0[1];
  }
    
void vt_convert_save(VP_VIEWPORT vp)
  {
  PT3 bt0,bt1;
  INT ibt0[2],ibt1[2];

  bt0[0] =    0.0;  bt0[1] =    0.0;    bt0[2] = 0.0;
  bt1[0] = 1000.0;  bt1[1] = 1000.0;    bt1[2] = 0.0;

  ibt0[0] =   0;  ibt0[1] =   0;
  ibt1[0] =   1;  ibt1[1] =   1;

  vp3_pixel_to_point(ibt0,vp,bt0);
  vp3_pixel_to_point(ibt1,vp,bt1);

  xxscl = (bt1[0]-bt0[0]);
  yyscl = (bt1[1]-bt0[1]);
  xx0   = bt0[0];
  yy0   = bt0[1];
  }
    
inline double vt_convert_xoff() { return (gx0 - xx0)/NZ(xxscl); }
inline double vt_convert_yoff() { return (gy0 - yy0)/NZ(yyscl); }
inline double vt_convert_xscl() { return gxscl/NZ(xxscl); }
inline double vt_convert_yscl() { return gyscl/NZ(yyscl); }
    
//---------------------------------------------------------------------------

void Grf3D_Display::SetZoom(ZoomTypes ZoomTypeIn, Pt_SLW &Pt1, Pt_SLW &Pt2)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::SetZoom");
  #endif

  SetZoom(ZoomTypeIn,Pt1.World.X, Pt1.World.Y,Pt2.World.X,Pt2.World.Y);
  };

//---------------------------------------------------------------------------

void Grf3D_Display::SetZoom(ZoomTypes ZoomTypeIn, REAL wxl, REAL wyl, REAL wxh, REAL wyh)
  {
  #if dbgGrfDsp
  dbgpln("Grf3D_Display::SetZoom");
  #endif
  Viewport &vp=*CurrentView();
  Pt_3f p1, p2;
  PT3 v1;
  REAL w, h, dw, dh, xl, yl, zl;

  vt_convert_init(Vp1->vp);

  switch (ZoomTypeIn)
    {
    case Zoom_PanRel :
      w = fabs(C3_MAX_X(&vp.ZoomBox)-C3_MIN_X(&vp.ZoomBox));
      h = fabs(C3_MAX_Y(&vp.ZoomBox)-C3_MIN_Y(&vp.ZoomBox));
      if (w <1.0e-6 && vp.Drw->GetBounds())
        {
        p1.Set(C3_MIN_X(&vp.Drw->Bounds), C3_MIN_Y(&vp.Drw->Bounds));
        p2.Set(C3_MAX_X(&vp.Drw->Bounds), C3_MAX_Y(&vp.Drw->Bounds));
        c3a_box_init_pt(&vp.ZoomBox, p1.p());
        c3a_box_append_pt(&vp.ZoomBox, p2.p());
        w = fabs(C3_MAX_X(&vp.ZoomBox)-C3_MIN_X(&vp.ZoomBox));
        h = fabs(C3_MAX_Y(&vp.ZoomBox)-C3_MIN_Y(&vp.ZoomBox));
        }

      dw = w * (wxh-1.0);
      dh = h * (wyh-1.0);
      p1.Set(C3_MIN_X(&vp.ZoomBox)+w*wxl-0.5*dw,C3_MIN_Y(&vp.ZoomBox)+h*wyl-0.5*dh,0.0);
      p2.Set(C3_MAX_X(&vp.ZoomBox)+w*wxl+0.5*dw,C3_MAX_Y(&vp.ZoomBox)+h*wyl+0.5*dh,1.0);
      vp.ZoomType=Zoom_Win;
      break;
    case Zoom_Win :
      vp.ZoomType = ZoomTypeIn;
      p1.Set(wxl,wyl,0.0);
      p2.Set(wxh,wyh,1.0);
      break;
    case Zoom_PanAbs :
      ZoomTypeIn=Zoom_Win;
    default :
      vp.ZoomType = ZoomTypeIn;
      p1.Set(0.0,0.0,0.0);
      p2.Set(vp.Drw->PageWidth(),vp.Drw->PageHeight(),1.0);
      break;
    }

  c3a_box_init_pt(&vp.ZoomBox, p1.p());
  c3a_box_append_pt(&vp.ZoomBox, p2.p());

  float xc, yc, zc;

  switch (vp.ZoomType)
    {
    case Zoom_Win :
      w = fabs(C3_MAX_X(&vp.ZoomBox)-C3_MIN_X(&vp.ZoomBox));
      h = fabs(C3_MAX_Y(&vp.ZoomBox)-C3_MIN_Y(&vp.ZoomBox));
      if (w > 1.0e-6 && h > 1.0e-6)
        {
        vp3_set_view_dir(vp.vp, 0.0, 0.0);
        vpi_set_wh(vp.vp, w, h);
        c3v_set((float)0.5*(C3_MAX_X(&vp.ZoomBox)+C3_MIN_X(&vp.ZoomBox)),
          (float)0.5*(C3_MAX_Y(&vp.ZoomBox)+C3_MIN_Y(&vp.ZoomBox)),
          (float)0.5*(C3_MAX_Z(&vp.ZoomBox)+C3_MIN_Z(&vp.ZoomBox)), v1);
        vp3_set_ctr(vp.vp, v1);
        break;
        }
    case Zoom_All :
    case Zoom_Iso :
      if (vp.Drw->GetBounds())
        {
        //xc = (float)(0.5*(C3_MAX_X(&vp.Drw->Bounds)+C3_MIN_X(&vp.Drw->Bounds)));
        //yc = (float)(0.5*(C3_MAX_Y(&vp.Drw->Bounds)+C3_MIN_Y(&vp.Drw->Bounds)));
        //zc = (float)(0.5*(C3_MAX_Z(&vp.Drw->Bounds)+C3_MIN_Z(&vp.Drw->Bounds)));
        xc = (float)(0.5*Range((REAL)-1.0e38, C3_MAX_X(&vp.Drw->Bounds)+C3_MIN_X(&vp.Drw->Bounds), (REAL)1.0e38));
        yc = (float)(0.5*Range((REAL)-1.0e38, C3_MAX_Y(&vp.Drw->Bounds)+C3_MIN_Y(&vp.Drw->Bounds), (REAL)1.0e38));
        zc = (float)(0.5*Range((REAL)-1.0e38, C3_MAX_Z(&vp.Drw->Bounds)+C3_MIN_Z(&vp.Drw->Bounds), (REAL)1.0e38));
        w = Range((REAL)1.0, C3_MAX_X(&vp.Drw->Bounds)-C3_MIN_X(&vp.Drw->Bounds), (REAL)1.0e30);
        h = Range((REAL)1.0, C3_MAX_Y(&vp.Drw->Bounds)-C3_MIN_Y(&vp.Drw->Bounds), (REAL)1.0e30);
        const REAL ZoomBorderFactor = 1.05;
        w*=ZoomBorderFactor;
        h*=ZoomBorderFactor;

        vp3_set_view_dir(vp.vp, 0.0, 0.0);

        vpi_set_wh(vp.vp, w, h);
        c3v_set(xc,yc,zc, v1);
        vp3_set_ctr(vp.vp, v1);
        
        if (vp.ZoomType==Zoom_Iso)
          {
          static int ii=0;
          static double r0=0;//PI/12;
          static double r1=0;//-PI/2;//6;

          r0=0;//.5;//=PI/6;//PI/12;
          r1=0.5;//-PI/6;
          
#pragma chNOTE("NBNB EntityInvalidate()")
          //
          vp3_set_view_dir(vp.vp, r0, r1);//PI/6);
          vp3_fit(vp.vp, &vp.Drw->Bounds,2.1);

          xl = Max(C3_MAX_X(&vp.Drw->Bounds)-C3_MIN_X(&vp.Drw->Bounds), (REAL)1.0);
          yl = Max(C3_MAX_Y(&vp.Drw->Bounds)-C3_MIN_Y(&vp.Drw->Bounds), (REAL)1.0);
          zl = Max(C3_MAX_Z(&vp.Drw->Bounds)-C3_MIN_Z(&vp.Drw->Bounds), (REAL)1.0);

          PT3 P;
          P[0]=xc-xl/2.0;
          P[1]=yc-yl/2.0;
          P[2]=zc-zl/2.0;

          c3a_box_init_pt(&vp.ZoomBox, P);

          P[0]=xc+xl/2.0;
          P[1]=yc+yl/2.0;
          P[2]=zc+zl/2.0;
          c3a_box_append_pt(&vp.ZoomBox, P);
          }
        else
          {

          PT3 P;
          P[0]=xc-w/2.0;
          P[1]=yc-h/2.0;
          P[2]=0;
          c3a_box_init_pt(&vp.ZoomBox, P);

          P[0]=xc+w/2.0;
          P[1]=yc+h/2.0;
          P[2]=1;
          c3a_box_append_pt(&vp.ZoomBox, P);
          }
        break;
        }
    case Zoom_Page :

      vp3_set_view_dir(vp.vp, 0.0, 0.0);

      w = vp.Drw->PageWidth();
      h = vp.Drw->PageHeight();
      vpi_set_wh(vp.vp, w, h);
      c3v_set(0.5*w,0.5*h, 0.0, v1);
      vp3_set_ctr(vp.vp, v1);
      vp3_set_view_dir(vp.vp, 0.0, 0.0);
      p1.Set(0.0,0.0,0.0);
      p2.Set(vp.Drw->PageWidth(),vp.Drw->PageHeight(),1.0);
      break;
    }

  #if dbgGrfZoom
  switch (ZoomTypeIn)
    {
    case Zoom_PanRel : dbgpln("Zoom Pan Rel"); break;
    case Zoom_All    : dbgpln("Zoom All"); break;
    case Zoom_Page   : dbgpln("Zoom Page"); break;
    }
  #endif

  vt_convert_save(Vp1->vp);
  Vp1->zoom_xscale  = vt_convert_xscl();
  Vp1->zoom_yscale  = vt_convert_yscl();
  Vp1->zoom_xoffset = vt_convert_xoff();
  Vp1->zoom_yoffset = vt_convert_yoff();
  Vp1->ApplyDeltaMatrix();

  #if dbgGrfZoom
  dbgpln("BoundBox Max: %10.2f %10.2f %10.2f ",C3_MAX_X(&vp.Drw->Bounds),C3_MAX_Y(&vp.Drw->Bounds),C3_MAX_Z(&vp.Drw->Bounds));
  dbgpln("BoundBox Min: %10.2f %10.2f %10.2f ",C3_MIN_X(&vp.Drw->Bounds),C3_MIN_Y(&vp.Drw->Bounds),C3_MIN_Z(&vp.Drw->Bounds));
  dbgpln("ZoomBox Max : %10.2f %10.2f %10.2f ",C3_MAX_X(&vp.ZoomBox),C3_MAX_Y(&vp.ZoomBox),C3_MAX_Z(&vp.ZoomBox));
  dbgpln("ZoomBox Min : %10.2f %10.2f %10.2f ",C3_MIN_X(&vp.ZoomBox),C3_MIN_Y(&vp.ZoomBox),C3_MIN_Z(&vp.ZoomBox));
  vp3_get_ctr(vp.vp, v1);
  vpi_get_wh(vp.vp, &w, &h);
  dbgpln("Centre      : %10.2f %10.2f %10.2f",v1[0],v1[1],v1[2]);
  dbgpln("Height      : %10.2f %10.2f",w,h);
  dbgpln("Deltas      : %10g %10g %10g %10g",Vp1->zoom_xscale,Vp1->zoom_yscale,Vp1->zoom_xoffset,Vp1->zoom_yoffset);
  #endif
  };

//===========================================================================
    
