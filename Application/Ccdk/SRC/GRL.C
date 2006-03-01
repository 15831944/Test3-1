//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */      
/******************************** GRL.S ************************************/
/********************** Lowest Level Graphic Routines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef  _CCDK_WIN
#ifdef  __TURBOC__
#define Do__TURBOC__
#endif
#ifdef  __MCRSFTC__
#define Do__MCRSFTC__
#endif
#ifdef  __MCRGRAPH__
#define Do__MCRGRAPH__
#endif
#endif

#ifdef  Do__TURBOC__
#include <dos.h>
#include <stdlib.h>
#endif  /*Do__TURBOC__*/

#include <grldefs.h>
#include "lpoint.h"
#include <limits.h>
#include <windows.h>
#include <stdio.h>

//#include "optoff.h"

#define xdbggrl

#ifdef dbggrl
extern FILE * dbgf;
#endif

#ifdef  _CCDK_WIN
#include <assert.h>
#include <stdio.h>
#define _LocalClip
int curr_color;
GLOBAL  HDC   win_HDC;
GLOBAL  BOOLEAN win_printing;
static COLORREF win_ColorMap[GR_MAXCOLORS] = 
  {

    0x00000000,         /*  GR_BLACK          */
    0x00c00000,         /*  GR_BLUE           */
    0x0000c000,         /*  GR_GREEN          */
    0x00c0c000,         /*  GR_CYAN           */
    0x000000c0,         /*  GR_RED            */
    0x00c000c0,         /*  GR_MAGENTA        */
    0x0000c0c0,         /*  GR_BROWN          */
    0x00c0c0c0,         /*  GR_LIGHTGRAY      */
    0x00808080,         /*  GR_DARKGRAY       */
    0x00ff0000,         /*  GR_LIGHTBLUE      */
    0x0000ff00,         /*  GR_LIGHTGREEN     */
    0x00ffff00,         /*  GR_LIGHTCYAN      */
    0x000000ff,         /*  GR_LIGHTRED       */
    0x00ff00ff,         /*  GR_LIGHTMAGENT    */
    0x0000ffff,         /*  GR_LIGHTYELLOW    */
    GRREF_NEARWHITE,    /*  GR_WHITE          */ /* Problem with printing*/
                        /*  Black to the end  */
  
  };
static HPEN  win_Pen[GR_MAXCOLORS][GR_MAXSTYLES][GR_MAXTHICKNESS] = { NULL, /*...*/ } ;
static int  gs_ThickWidth[GR_MAXTHICKNESS] = {1, 4, 6};
#endif

GLOBAL  REAL    GR_SCREEN_RATIO = 1.3333333333 ;
#ifdef  __GFX__
#define _LocalClip
GLOBAL  GR_PATT         GRL_PATT = GR_SOLID_PATT ;
GLOBAL  int             GRL_VIEW_HANDLE = 0 ;
#endif
    
#ifdef  _LocalClip
STATIC void grl_box_corners __(( GR_PIX_COORD* DUMMY0, GR_PIX_COORD* DUMMY1, 
    GR_PIX_COORD* DUMMY2, GR_PIX_COORD* DUMMY3 )) ;
STATIC void grl_clip_line __(( GR_PIX_COORD DUMMY0, GR_PIX_COORD DUMMY1, 
    GR_PIX_COORD DUMMY2, GR_PIX_COORD DUMMY3 )) ;
GLOBAL  GR_PIX_COORD    GRL_X = 0 ;
GLOBAL  GR_PIX_COORD    GRL_Y = 0 ;
GLOBAL  REAL            FGRL_X = 0 ;
GLOBAL  REAL            FGRL_Y = 0 ;
GLOBAL  int             GRL_X0, GRL_Y0, GRL_X1, GRL_Y1 ;
GLOBAL  REAL            FGRL_X0, FGRL_Y0, FGRL_X1, FGRL_Y1 ;
GLOBAL  GR_COLOR        GRL_COLOR = GR_BLACK ;
GLOBAL  int             GRL_OVERLAY = 0;

GLOBAL  double          GRL_THICKNESS      = 0;
GLOBAL  int             GRL_THICKNESSINDEX = 0;
#endif
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_moveto ( x, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
{
#ifdef Do__MCRGRAPH__
    _moveto ( x, y ) ;
#endif
#ifdef Do__TURBOC__
    moveto ( x, y ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_MoveTo %i %i \n", x,y);
#endif
#endif
#ifdef _LocalClip
    GRL_X = x + GRL_X0 ;
    GRL_Y = y + GRL_Y0 ;
#endif
}
  /*
BBS_PRIVATE void mhm_lineto(x,y)
REAL x,y;
{
    x += GRL_X0 ;
    y += GRL_Y0 ;
    add_vector_buffer( GRL_X, GRL_Y, x, y ) ;
    GRL_X = x ;
    GRL_Y = y ;
}

BBS_PRIVATE void mhm_moveto(x,y)
REAL x,y;
{
    GRL_X = x + GRL_X0 ;
    GRL_Y = y + GRL_Y0 ;
}
    */
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_lineto ( x, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
{
#ifdef Do__MCRGRAPH__
    _lineto ( x, y ) ;
#endif
#ifdef Do__TURBOC__
    lineto ( x, y ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_LineTo %i %i \n", x,y);
#endif    
#endif    
#ifdef _LocalClip
    x += GRL_X0 ;
    y += GRL_Y0 ;
    grl_clip_line ( GRL_X, GRL_Y, x, y ) ;
    GRL_X = x ;
    GRL_Y = y ;
#endif
}

typedef struct    {
         short x,y;
      }scr_vector;

#define SIZE_VB    1000
static  int        *nb=NULL;    //[SIZE_VB];     //KGA does this memory get released ???
static  short       *type=NULL;  //[SIZE_VB];    //KGA does this memory get released ???
static  LONGPOINT  *vb=NULL;    //[SIZE_VB];     //KGA does this memory get released ???
static  int        nvb=0,evb=0,ivb=0,insflag=0,nnb=0,vbflag=0,curr_type = 1,clvl=0;
static  int        msize_nb = 0,msize_type = 0,msize_vb = 0;
#define SIZE_VB_INC 2048
int

ini_insert_vbflag(int lvl)
{
    clvl = lvl;
    vbflag++;
    return vbflag;
}

int
clr_insert_vbflag()
{
    vbflag--;
    return vbflag;
}

int
tst_insert_vbflag()
{
    return vbflag;
}


fix_and_test_buffer()
{
   if( nvb == 0 || nnb == 0 )
      return 0;

   if( nb[nnb-1] == 1 ){
      nnb--;
      nvb--;
   }
   if( nvb == 0 || nnb == 0 )
      return 0;
   return nvb;
}


int
ini_vector_buffer()
{
   if( tst_insert_vbflag())
      return 1;
   nvb = 0;
   nnb = 0;
   return 1;
}



LONGPOINT *
check_vb_size()
{
  if( msize_vb == 0 || nvb >= msize_vb-1){
    msize_vb += SIZE_VB_INC;
    if( vb ){
      vb = (LONGPOINT *)dmm_realloc((void *)vb,(size_t)sizeof(LONGPOINT)*msize_vb, "LONGPOINT_BUFFERvb");
    }
    else{
      vb = (LONGPOINT *)dmm_malloc((size_t)sizeof(LONGPOINT)*msize_vb,"LONGPOINT_BUFFERvb");
    }
  }
  assert(vb!=NULL);
  return vb;
}

int *
check_nb_size()
{
  if( msize_nb == 0 || nnb >= msize_nb-1){
    msize_nb += SIZE_VB_INC;
    if( nb ){
      nb = (int *)dmm_realloc((void *)nb,(size_t)sizeof(int)*msize_nb, "LONGPOINT_BUFFERnb");
    }
    else{
      nb = (int *)dmm_malloc((size_t)sizeof(int)*msize_nb,"LONGPOINT_BUFFERnb");
    }
  }
  assert(nb!=NULL);
  return nb;
}

short *
check_type_size()
{
  if( msize_type == 0 || nnb >= msize_type-1){
    msize_type += SIZE_VB_INC;
    if( type ){
      type = (short *)dmm_realloc((void *)type,(size_t)sizeof(short)*msize_type, "LONGPOINT_BUFFER");
    }
    else{
      type = (short *)dmm_malloc((size_t)sizeof(short)*msize_type,"LONGPOINT_BUFFER");
    }
  }
  assert(type!=NULL);
  return type;
}

void
free_buffer_points()
{
  if( vb ){
    dmm_free((void*)vb);
  }
}

void
free_buffer_entries()
{
  if( nb ){
    dmm_free((void*)nb);
  }
}

void
free_buffer_type()
{
  if( type ){
    dmm_free((void*)type);
  }
}

void            
add_vector_buffer(REAL xx0,REAL yy0,REAL xx1,REAL yy1)
{
   float x0,y0,x1,y1;

   //if( nvb == SIZE_VB-2 ){
   //    evb = -1;
   //    return;
   //}


   x0 = (float)xx0;
   y0 = (float)yy0; // mhm *8192 gone
   x1 = (float)xx1;
   y1 = (float)yy1;


   // move instruction
    if( nvb==0 || (nvb && ((vb[nvb-1].x - x0) || (vb[nvb-1].y - y0))))
      {
      // if old buffer has only one point overwrite it
      if( nvb && nb[nnb-1] == 1 )
        {
        nnb--;
        nvb--;
        }

      if( msize_vb == 0 || nvb == msize_vb)
        {
        msize_vb += SIZE_VB_INC;
        if( vb )
          {
          vb = (LONGPOINT *)dmm_realloc((void *)vb,(size_t)sizeof(LONGPOINT)*msize_vb, "LONGPOINT_BUFFER");
          }
        else
          {
          vb = (LONGPOINT *)dmm_malloc((size_t)sizeof(LONGPOINT)*msize_vb,"LONGPOINT_BUFFER");
          }
        }
      vb   = check_vb_size();
      nb   = check_nb_size();
      type = check_type_size();
      nb[nnb]   = 1;

      /*  ASSERT(curr_type  <   1);
      ASSERT(curr_color <   0);
      ASSERT(curr_color > 255); */
      type[nnb] = ((curr_color << 8)|(curr_type));

      vb[nvb].x = x0;
      vb[nvb].y = y0;
      nnb++;
      nvb++;
      }
   // draw instruction
   vb   = check_vb_size();
   vb[nvb].x  = x1;   
   vb[nvb].y  = y1;   
   nvb++;
   nb[nnb-1] += 1;
   
}

BBS_PRIVATE void mhm_lineto(x,y)
REAL x,y;
{
  //cnm POINT Pt;
    x += FGRL_X0 ;
    y += FGRL_Y0 ;
    add_vector_buffer( FGRL_X, FGRL_Y, x, y ) ;

    //MoveToEx (win_HDC, (int)FGRL_X, (int)FGRL_Y , &Pt);
    //LineTo (win_HDC, (int)x, (int)y );




    FGRL_X = x ;
    FGRL_Y = y ;
}

BBS_PRIVATE void mhm_moveto(x,y)
REAL x,y;
{
    FGRL_X = x + FGRL_X0 ;
    FGRL_Y = y + FGRL_Y0 ;
}


LONGPOINT *
addr_buffer_points()
{
   return vb;
}

int
size_buffer_points()
{
   return nvb;
}

int  *
addr_buffer_entries()
{
   return nb;
}

short *
addr_buffer_type()
{
   return type;
}

int
size_buffer_entries()
{
   return nnb;
}

int
color_buffer_entries()
{
   return curr_color;
}

set_buffer_type(int type)
{
   curr_type = type;
   return 1;
}

int 
get_buffer_offset()
{
  return nnb-1;
}


#include "dynmseg.h"

//#define VERIFY(h) if (h==NULL) MessageBox(NULL, "Display Not Open", "GRL.C", MB_OK|MB_ICONEXCLAMATION);
//#define VERIFY(h) /*{ if (h==NULL) fprintf(dbgf, "Display Not Open --- GRL.C"); }*/
#define VERIFY(h)

static POINT *mppb=NULL; /*cnm*/
static DWORD lmppb=0;      /*cnm*/

int mypolypolyline(int xoff,int yoff,LONGPOINT *p,short *t,DWORD *np, DWORD nnp,double lev,double lev2,int dytype, int colorrqd, int *colordrwn, int ovrcolorrqd, int ovrpenrqd, int *ovrcolordrwn, HBRUSH hFillBrush, double thickness)
  {

  double DEV_MAX=(double)((int)0x7ffffff);         /* +27Bits */
  double DEV_MIN=(double)(0-(int)0x7ffffff);  /* -27Bits */
  short col=colorrqd;
  short ovrcol=ovrcolorrqd;
  DWORD i,k,nxx=0;
  static HBRUSH hBlack;
  static HBRUSH hRed;
  static HBRUSH hGreen;
  //static HBRUSH hBrushes[sizeof(win_ColorMap)/sizeof(win_ColorMap[0])];
  static int BrushesOK=0;
  //static CMap<COLORREF, COLORREF, HBRUSH, HBRUSH> sm_hBrushes;//[sizeof(win_ColorMap)/sizeof(win_ColorMap[0])];

  #ifdef dbggrl
  fprintf(dbgf, "mypolypolyline %5i %10.2f %3i", GRL_COLOR, GRL_THICKNESS, GRL_THICKNESSINDEX);
  #endif


//const byte MSStyle_Off      = 0;
//const byte MSStyle_Solid    = 1;
//const byte MSStyle_LDash    = 2;
//const byte MSStyle_SDash    = 3;
//const byte MSStyle_Dots     = 4;
  // swap dots and dashes (ovrpenrqd=0 is solid)
  if (ovrcol>=0 && ovrpenrqd==3)
    {
    int t=col;
    col=ovrcol;
    ovrcol=t;
    ovrpenrqd=abs(ovrpenrqd);
    };
  
  if (!BrushesOK)
    {
    hBlack=CreateSolidBrush(RGB(0,0,0));
    hRed=CreateSolidBrush(RGB(255,0,0));
    hGreen=CreateSolidBrush(RGB(0,255,0));
    //memset(hBrushes, 0, sizeof(hBrushes));
    //sm_hBrushes.InitHashTable(512);
    BrushesOK=1;
    }
  for(i=0;i<nnp;i++)
    nxx+=np[i];
  if (nxx>lmppb)           /*cnm*/
    {
    dmm_free((void*)mppb);
    mppb = (POINT *)dmm_malloc((size_t)sizeof(POINT)*nxx,"POLYLINE_BUFFER");
    lmppb=nxx;
    }
  for(i=0; i < nxx; i++)
    {
    /* CNM
    float d=(p[i].x+0.5);
    mppb[i].x = (d < (float)INT_MIN ? (float)INT_MIN : (d > (float)INT_MAX ? (float)INT_MAX : d));
    d=(p[i].y+0.5);
    mppb[i].y = (d < (float)INT_MIN ? (float)INT_MIN : (d > (float)INT_MAX ? (float)INT_MAX : d));
    */
    double d=(p[i].x+0.5);
    mppb[i].x = (long)(d < DEV_MIN ? DEV_MIN : (d > DEV_MAX ? DEV_MAX : d));
    d=(p[i].y+0.5);
    mppb[i].y = (long)(d < DEV_MIN ? DEV_MIN : (d > DEV_MAX ? DEV_MAX : d));
    mppb[i].x -= xoff;
    mppb[i].y -= yoff;
    }

  
  for( i = 0, k = 0; i < nnp; i++,k += np[i-1])
    {
    //if (colorrqd==-1)
    //  col = (t[i]&0xff00) >> 8;
    //if( lev > 0.0 && dytype >= 2 && dytype <= 5)
    if( lev > 0.0 && SEG_IS_FILL(dytype) )
      {
      int xmin = 100000,xmax = -100000,ymin = 100000,ymax = -100000,l;
      HRGN preg,rreg;
      POINT rec[4];
   
      preg = CreatePolygonRgn(mppb+k,(int)np[i],WINDING);
   
      for( l = 0; l < (int)np[i]; l++ )
        {
        if( xmin > (mppb+k+l)->x) xmin = (mppb+k+l)->x;
        if( ymin > (mppb+k+l)->y) ymin = (mppb+k+l)->y;
        if( xmax < (mppb+k+l)->x) xmax = (mppb+k+l)->x;
        if( ymax < (mppb+k+l)->y) ymax = (mppb+k+l)->y;
        }    
      switch( dytype )
        {
        case SEG_TBFILL: // trim fill from top
           ymin = ymin + (int)((1.0-lev)*(ymax-ymin));
           break;
        case SEG_BTFILL: // trim fill from bottom
           ymax = ymin + (int)((lev)*(ymax-ymin));
           break;
        case SEG_LRFILL: // trim fill from left
           xmin = xmin + (int)((1.0-lev)*(xmax-xmin));
           break;
        case SEG_RLFILL: // trim fill from right
           xmax = xmin + (int)((lev)*(xmax-xmin));
           break;
        default:
           break;
        }
   
      VERIFY(win_HDC); // cnm is Display Open
      rec[0].x = xmin; rec[0].y = ymin;
      rec[1].x = xmin; rec[1].y = ymax;
      rec[2].x = xmax; rec[2].y = ymax;
      rec[3].x = xmax; rec[3].y = ymin;
      LPtoDP(win_HDC,rec,4);
      rreg = CreatePolygonRgn(rec,4,WINDING);
      FillRgn(win_HDC,preg,hBlack);
      SelectClipRgn(win_HDC,rreg);
      if (hFillBrush)//fillcolor>=0)
        {
        //if (hBrushes[fillcolor]==NULL)
        //  hBrushes[fillcolor]=CreateSolidBrush(win_ColorMap[fillcolor]);
        FillRgn(win_HDC,preg,hFillBrush);
        }
      else
        FillRgn(win_HDC,preg,hRed);
      SelectClipRgn(win_HDC,(HRGN)NULL);
      DeleteObject(rreg);
      DeleteObject(preg);
      }


    if( lev > 0.0 && SEG_IS_LR(dytype) )
      { // a left right/red green combination
      int xmin = 100000,xmax = -100000,ymin = 100000,ymax = -100000,l,xc,yc;
      HRGN preg,rreg;
      POINT reca[4],recb[4];

      preg = CreatePolygonRgn(mppb+k,(int)np[i],WINDING);
   
      for( l = 0; l < (int)np[i]; l++ )
        {
        if( xmin > (mppb+k+l)->x) xmin = (mppb+k+l)->x;
        if( ymin > (mppb+k+l)->y) ymin = (mppb+k+l)->y;
        if( xmax < (mppb+k+l)->x) xmax = (mppb+k+l)->x;
        if( ymax < (mppb+k+l)->y) ymax = (mppb+k+l)->y;
        }
      xc = xmin + (int)((xmax-xmin)*lev);
      yc = ymin + (int)((ymax-ymin)*lev);


      reca[0].x = xmin; reca[0].y = ymin;
      reca[1].x = xmin; reca[1].y = ymax;
      reca[2].x = xc;   reca[2].y = ymax;
      reca[3].x = xc;   reca[3].y = ymin;

      recb[0].x = xc;   recb[0].y = ymin;
      recb[1].x = xc;   recb[1].y = ymax;
      recb[2].x = xmax; recb[2].y = ymax;
      recb[3].x = xmax; recb[3].y = ymin;

      LPtoDP(win_HDC,reca,4);
      LPtoDP(win_HDC,recb,4);

      FillRgn(win_HDC,preg,hBlack);
      rreg = CreatePolygonRgn(reca,4,WINDING);
      SelectClipRgn(win_HDC,rreg);
      FillRgn(win_HDC,preg,hRed);
      rreg = CreatePolygonRgn(recb,4,WINDING);
      SelectClipRgn(win_HDC,rreg);
      FillRgn(win_HDC,preg,hGreen);
      SelectClipRgn(win_HDC,(HRGN)NULL);
      DeleteObject(rreg);
      DeleteObject(preg);
      }


    if( SEG_IS_TOGGLE(dytype) )
      { 
      HRGN preg;//,rreg;
      preg = CreatePolygonRgn(mppb+k,(int)np[i],WINDING);
      if( lev < 0.5 )
        FillRgn(win_HDC,preg,hRed);
      else
        FillRgn(win_HDC,preg,hGreen);
      DeleteObject(preg);
      }

    if( SEG_IS_BILEVEL(dytype) )
      { 
      HRGN preg;//,rreg;
      preg = CreatePolygonRgn(mppb+k,(int)np[i],WINDING);
      if( lev < 0.5 )
        FillRgn(win_HDC,preg,hRed);
      else
        FillRgn(win_HDC,preg,hGreen);
      DeleteObject(preg);
      }
    if (colorrqd==-1)
      col = (t[i]&0xff00) >> 8;
    if( SEG_IS_HIGH(dytype))
      {
      col = GR_HIGHLIGHT;
      ovrcol = -1; //kga 23/11/2001 : Do NOT use overlay colours for "highlight" graphics
      }

    if (ovrcol>=0 && ovrpenrqd>=0)
      {
      int md;
      int OverXLate[] = {0,0,1,1,2};
      grl_set_color(col,0);
      //grl_set_thickness(thickness); temp
      Polyline(win_HDC,mppb+k,(int)np[i]);
      *colordrwn=col;
      md=SetBkMode(win_HDC, TRANSPARENT);
      grl_set_color(ovrcol, OverXLate[ovrpenrqd]);
      //grl_set_thickness(thickness); temp
      Polyline(win_HDC,mppb+k,(int)np[i]);
      SetBkMode(win_HDC, md);
      }
    else
      {
      grl_set_color(col,0);
      //grl_set_thickness(thickness); temp
      Polyline(win_HDC,mppb+k,(int)np[i]);
      *colordrwn=col;
      }
    *ovrcolordrwn=ovrcol;

    }
//  DeleteObject(hBlack);
//  DeleteObject(hRed);
//  DeleteObject(hGreen);

  /* free((void *)mppb); */ /*cnm*/
  return col; 
  }

void free_mypolypoly_buffer()
{
  if (mppb)
    dmm_free((void*)mppb);
}




#ifdef _LocalClip
/*-------------------------------------------------------------------------*/
STATIC void grl_clip_line ( x0, y0, x1, y1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
    GR_PIX_COORD x, y ;
    int u0, v0, u1, v1 ;
    u0 = ( x0 >= GRL_X1 ) - ( x0 <= GRL_X0 ) ;
    v0 = ( y0 >= GRL_Y1 ) - ( y0 <= GRL_Y0 ) ;
    u1 = ( x1 >= GRL_X1 ) - ( x1 <= GRL_X0 ) ;
    v1 = ( y1 >= GRL_Y1 ) - ( y1 <= GRL_Y0 ) ;
    if ( u0 == 0 && u1 == 0 && v0 == 0 && v1 == 0 ) 
  {
#ifdef __GFX__
  LINE ( x0, y0, x1, y1, GRL_COLOR, DRAW_LINE, GRL_PATT ) ;
#else
  //POINT Pt;
  //MoveToEx (win_HDC, x0, y0 , &Pt);
  //LineTo (win_HDC, x1, y1 );

  // this is the vector to the screen
  //add_vector_buffer((int)x0,(int)y0,(int)x1,(int)y1);



#endif
  }
    else if ( u0 == u1 && ( u0 == 1 || u0 == -1 ) ) ;
    else if ( v0 == v1 && ( v0 == 1 || v0 == -1 ) ) ;
    else {
  if ( x1 - x0 <= 1 && x0 - x1 <= 1 && y1 - y0 <= 1 && y0 - y1 <= 1 ) 
      RETURN ;
  x = (GR_PIX_COORD)(0.5 * ( x0 + x1 ));
  y = (GR_PIX_COORD)(0.5 * ( y0 + y1 ));
  grl_clip_line ( x0, y0, x, y ) ;
  grl_clip_line ( x, y, x1, y1 ) ;
    }
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN grl_set_videomode ( mode )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT mode ;
{
#ifdef Do__TURBOC__
    int graphdriver, graphmode, result ;
    STRING bgi_directory=getenv("TC_BGI") ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_Set_Video\n");
#endif    
#endif
#ifdef __GFX__
    BOOLEAN status ;
#endif
    if ( mode == (INT)GR_DETECT ) 
    RETURN ( grl_set_videomode ( GR_VGA16 ) || 
      grl_set_videomode ( GR_EGA ) || grl_set_videomode ( GR_VGA2 ) || 
      grl_set_videomode ( GR_EGA0 ) || grl_set_videomode ( GR_HRES16 ) || 
      grl_set_videomode ( GR_HERC ) || grl_set_videomode ( GR_HRES0 ) ) ;
#ifdef Do__MCRGRAPH__
    else if ( mode == GR_VGA16 ) 
  RETURN ( _setvideomode ( _VRES16COLOR ) ) ;
    else if ( mode == GR_EGA ) 
  RETURN ( _setvideomode ( _ERESCOLOR ) ) ;
    else if ( mode == GR_VGA2 ) 
  RETURN ( _setvideomode ( _VRES2COLOR ) ) ;
    else if ( mode == GR_EGA0 ) 
  RETURN ( _setvideomode ( _ERESNOCOLOR ) ) ;
    else if ( mode == GR_HRES16 ) 
  RETURN ( _setvideomode ( _HRES16COLOR ) ) ;
    else if ( mode == GR_HERC ) 
  RETURN ( _setvideomode ( _HERCMONO ) ) ;
    else if ( mode == GR_HRES0 ) 
  RETURN ( _setvideomode ( _HRESBW ) ) ;
    else
  RETURN ( FALSE ) ;
#endif

#ifdef Do__TURBOC__
    else if ( mode == GR_VGA16 ) {
  graphdriver = VGA ;
  graphmode = VGAHI ;
    }
    else if ( mode == GR_EGA ) {
  graphdriver = EGA ;
  graphmode = EGAHI ;
    }
    else if ( mode == GR_VGA2 ) {
  RETURN ( FALSE ) ;
    }
    else if ( mode == GR_EGA0 ) {
  graphdriver = EGAMONO ;
  graphmode = EGAMONOHI ;
    }
    else if ( mode == GR_HRES16 ) {
  graphdriver = EGA ;
  graphmode = EGALO ;
    }
    else if ( mode == GR_HERC ) {
  graphdriver = HERCMONO ;
  graphmode = HERCMONOHI ;
    }
    else if ( mode == GR_HRES0 ) {
  graphdriver = CGA ;
  graphmode = CGAHI ;
    }
    else
  RETURN ( FALSE ) ;

    initgraph ( &graphdriver, &graphmode, bgi_directory ) ;
    result = graphresult() ;
    RETURN ( result >= 0 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_Set_VideoMode\n");
#endif    
    GRL_X0 = 0 ;
    GRL_Y0 = 0 ;
    GRL_X1 = grl_get_max_x() ;
    GRL_Y1 = grl_get_max_y() ;
    RETURN ( TRUE ) ;
#endif
#ifdef __GFX__
    else if ( mode == GR_VGA16 ) {
  _gfx.card_monitor = VGA_DISPLAY || VGA_CARD ;
  status = SCREEN ( FORCE_VGA_COLOR ) ;
    }
    else if ( mode == GR_EGA ) 
  status = SCREEN ( FORCE_EGA_COLOR ) ;
    else if ( mode == GR_VGA2 ) 
  status = SCREEN ( FORCE_VGA_MONO ) ;
    else if ( mode == GR_EGA0 ) 
  status = SCREEN ( FORCE_EGA_MONO ) ;
    else if ( mode == GR_HRES16 ) 
  status = SCREEN ( FORCE_EGA_HI_RES ) ;
    else if ( mode == GR_HERC ) {
  _gfx.card_monitor = MONO_DISPLAY || HERC_CARD ;
  status = SCREEN ( 2 ) ;   /* Auto select mode */
    }
    else if ( mode == GR_HRES0 ) 
  status = SCREEN ( HI_RES_BW ) ;
    else
  status = FALSE ;
    if ( status ) {
  GRL_X0 = 0 ;
  GRL_Y0 = 0 ;
  GRL_X1 = grl_get_max_x() ;
  GRL_Y1 = grl_get_max_y() ;
  GRL_VIEW_HANDLE = OPEN_VIEW ( 0, 0, GRL_X1, GRL_Y1, 0, 0 ) ;
    }
    RETURN ( status ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_exit_videomode ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    _setvideomode (_DEFAULTMODE ) ;
#endif
#ifdef Do__TURBOC__
    closegraph () ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_exit_videomode\n");
#endif    
#endif
#ifdef __GFX__

    CLOSE_VIEW ( GRL_VIEW_HANDLE, 1 ) ; 
    SCREEN(0) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT grl_videomode ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    if ( v_info.mode == _VRES16COLOR )
  RETURN ( GR_VGA16 ) ;
    else if ( v_info.mode == _ERESCOLOR )
  RETURN ( GR_EGA ) ;
    else if ( v_info.mode == _VRES2COLOR )
  RETURN ( GR_VGA2 ) ;
    else if ( v_info.mode == _ERESNOCOLOR )
  RETURN ( GR_EGA0 ) ;
    else if ( v_info.mode == _HRES16COLOR )
  RETURN ( GR_HRES16 ) ;
    else if ( v_info.mode == _HERCMONO )
  RETURN ( GR_HERC ) ;
    else if ( v_info.mode == _HRESBW )
  RETURN ( GR_HRES0 ) ;
    else
  RETURN ( GR_NOVMODE ) ;
#endif
#ifdef Do__TURBOC__
    int gmode = getgraphmode ();

    if ( gmode == VGAHI )
  RETURN ( GR_VGA16 ) ;
    else if ( gmode == EGAHI )
  RETURN ( GR_EGA ) ;
    else if ( gmode == EGAMONOHI )
  RETURN ( GR_EGA0 ) ;
    else if ( gmode == EGALO )
  RETURN ( GR_HRES16 ) ;
    else if ( gmode == HERCMONOHI )
  RETURN ( GR_HERC ) ;
    else if ( gmode == CGAHI )
  RETURN ( GR_HRES0 ) ;
    else
  RETURN ( GR_NOVMODE ) ;

#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_videomode\n");
#endif    
    RETURN ( GR_VGA16 ) ;
#endif
#ifdef __GFX__
    if ( _gfx.card_monitor & ( VGA_DISPLAY | VGA_CARD ) )
  RETURN ( GR_VGA16 ) ;
    else if ( _gfx.card_monitor & ( EGA_DISPLAY | EGA_CARD ) )
  RETURN ( GR_EGA ) ;
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | VGA_CARD ) )
  RETURN ( GR_VGA2 ) ;
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | EGA_CARD ) ) 
  RETURN ( GR_EGA0 ) ;
    else if ( _gfx.card_monitor & ( COLOR_DISPLAY | EGA_CARD ) ) 
  RETURN ( GR_HRES16 ) ;
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | HERC_CARD ) )
  RETURN ( GR_HERC ) ; 
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | CGA_CARD ) ) 
  RETURN ( GR_HRES0 ) ;
    else
  RETURN ( FALSE ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_color ( color , overlaypen)
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_COLOR color ;
{
    if (win_printing && color>0) //KGA 22/1/99
        color = GR_BLACK; //KGA 22/1/99

    curr_color = color;

#ifdef Do__MCRGRAPH__
    _setcolor ( color ) ;
#endif
#ifdef Do__TURBOC__
    setcolor ( color ) ;
    setfillstyle ( SOLID_FILL, color ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_Set_Color %i\n", color);
#endif
    if ((GRL_COLOR != color || GRL_OVERLAY != overlaypen) && color >= 0)
      {
      // NB windows def
      // #define PS_SOLID            0
      // #define PS_DASH             1       /* -------  */
      // #define PS_DOT              2       /* .......  */
      if (win_Pen[color][0][GRL_THICKNESSINDEX] == NULL)
        win_Pen[color][0][GRL_THICKNESSINDEX] = CreatePen(PS_SOLID, gs_ThickWidth[GRL_THICKNESSINDEX], win_ColorMap[color]);
      if (win_Pen[color][1][GRL_THICKNESSINDEX] == NULL)
        win_Pen[color][1][GRL_THICKNESSINDEX] = CreatePen(PS_DASH, gs_ThickWidth[GRL_THICKNESSINDEX], win_ColorMap[color]);
      if (win_Pen[color][2][GRL_THICKNESSINDEX] == NULL)
        win_Pen[color][2][GRL_THICKNESSINDEX] = CreatePen(PS_DOT, gs_ThickWidth[GRL_THICKNESSINDEX], win_ColorMap[color]);
      VERIFY(win_HDC); // cnm is Display Open
      SelectObject(win_HDC, win_Pen[color][overlaypen][GRL_THICKNESSINDEX]);
      }
#endif
#ifdef _LocalClip
    GRL_COLOR = color ;
    GRL_OVERLAY = overlaypen;
#endif
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_thickness ( thickness )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL thickness;
{
  int thickindex=GRL_THICKNESSINDEX;

  #ifdef dbggrl
  fprintf(dbgf, "grl_Set_Thickness %.3f\n", thickness);
  #endif

  if (GRL_THICKNESS != thickness)
    {
    thickindex=0;
    if (thickness>0)
      {
      if (thickness<1.01)
        thickindex=1;
      else
        thickindex=2;
      }

    // NB windows def
    // #define PS_SOLID            0
    // #define PS_DASH             1       /* -------  */
    // #define PS_DOT              2       /* .......  */
    if (win_Pen[GRL_COLOR][0][thickindex] == NULL)
      win_Pen[GRL_COLOR][0][thickindex] = CreatePen(PS_SOLID, gs_ThickWidth[thickindex], win_ColorMap[GRL_COLOR]);
    if (win_Pen[GRL_COLOR][1][thickindex] == NULL)
      win_Pen[GRL_COLOR][1][thickindex] = CreatePen(PS_DASH, gs_ThickWidth[thickindex], win_ColorMap[GRL_COLOR]);
    if (win_Pen[GRL_COLOR][2][thickindex] == NULL)
      win_Pen[GRL_COLOR][2][thickindex] = CreatePen(PS_DOT, gs_ThickWidth[thickindex], win_ColorMap[GRL_COLOR]);
    VERIFY(win_HDC); // cnm is Display Open
    SelectObject(win_HDC, win_Pen[GRL_COLOR][GRL_OVERLAY][thickindex]);
    }
  #ifdef _LocalClip
  GRL_THICKNESS = thickness;
  GRL_THICKNESSINDEX = thickindex;
  #endif
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

static HPEN tmp_pen;
set_color_temperature(REAL t)
{
    BYTE r = (BYTE)0;
    BYTE g = (BYTE)0;
    BYTE b = (BYTE)0;

   
    if(      t <= 0.3333333  ){
  r = 255;
  g = (BYTE)((t/0.3333333)*255);
  b = 0;
    }
    else if( t  > 0.6666666  ){
  r = 0;
  g = (BYTE)((1.0-((t-0.6666666)/0.3333333))*255);
  b = 255;
    }
    else{
  r = (BYTE)((1.0-((t-0.3333333)/0.3333333))*255);
  g = 255;
  b = (BYTE)(((t-0.3333333)/0.3333333)*255);
    }

   

    if( tmp_pen ){
  // do a destroy on the pen
    }

    tmp_pen = CreatePen(PS_SOLID, 1,RGB(r,g,b));
    VERIFY(win_HDC); // cnm is Display Open
    SelectObject(win_HDC,tmp_pen);
    return 1;   
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_textposition ( x, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
{
#ifdef Do__MCRGRAPH__
    _settextposition ( y+1, x+1 ) ;
#endif
#ifdef Do__TURBOC__
    union REGS regs ;
    regs.h.dh = y ;
    regs.h.dl = x ;
    regs.h.ah = 2 ;
    regs.h.bh = 0 ;
    int86 ( 0x10, &regs, &regs ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_Set_Text_Position\n");
#endif    
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
#endif
#ifdef __GFX__
    LOCATE ( y+1, x+1 ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE COLORREF grl_get_win_color ( color ) //KGA 22/1/99
GR_COLOR color;
  {
  return (color<0 ? win_ColorMap[0] : win_ColorMap[color]);  //KGA 22/1/99
  }

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_win_color ( color, colorref ) //CNM 08/07/99
GR_COLOR color;
COLORREF colorref;
  {
  if (color>=0 && color<=GR_LASTUSERCOLOR)
    {
    int thick, style;
    win_ColorMap[color]=colorref;
    for (style=0; style<GR_MAXSTYLES; style++)
      {
      for (thick=0; thick<GR_MAXTHICKNESS; thick++)
        {
        if (win_Pen[color][style][thick])
          DeleteObject(win_Pen[color][style][thick]);
        win_Pen[color][style][thick]=NULL;
        }
      }
    }
  }

/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_COLOR grl_get_color ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    RETURN ( _getcolor ( ) ) ;
#endif
#ifdef Do__TURBOC__
    RETURN ( getcolor ( ) ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_Get_Color\n");
#endif    
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
    RETURN ( GRL_COLOR ) ;
#else
#endif
#ifdef __GFX__
    RETURN ( GRL_COLOR ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_patt ( patt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PATT patt ;
{
#ifdef Do__MCRGRAPH__
    _setlinestyle ( patt ) ;
#endif
#ifdef Do__TURBOC__
    setlinestyle ( USERBIT_LINE, patt, 1 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_set_patt\n");
#endif    
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
#endif
#ifdef __GFX__
    GRL_PATT = patt ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_solid_patt ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    _setlinestyle ( GR_SOLID_PATT ) ;
#endif
#ifdef Do__TURBOC__
    setlinestyle ( SOLID_LINE, 0, 1 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_set_solid_patt\n");
#endif    
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
#endif
#ifdef __GFX__
    GRL_PATT = GR_SOLID_PATT ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PATT grl_get_patt ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    RETURN ( _getlinestyle () ) ;
#endif
#ifdef Do__TURBOC__
    struct linesettingstype ls_info ;
    getlinesettings(&ls_info) ;
    RETURN ( ls_info.upattern ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_get_patt\n");
#endif    
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
/* ???????????????????????????????*/
    RETURN ( 1 );
#endif
#ifdef __GFX__
    RETURN ( GRL_PATT ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_clear_screen ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   RECT r;
#ifdef Do__MCRGRAPH__
    _clearscreen ( _GCLEARSCREEN ) ;
#endif
#ifdef Do__TURBOC__
    cleardevice ( ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_get_clear_screen\n");
#endif    
   r.left=grl_get_min_x();
   r.top=grl_get_min_y();
   r.right=grl_get_max_x();
   r.bottom=grl_get_max_y();
//    InvalidateRect(win_HDC, &r, TRUE);  ?? CNM
#endif
#ifdef __GFX__
    CLS();
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_max_x ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numxpixels ) ;
#endif
#ifdef Do__TURBOC__
    RETURN ( getmaxx() ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
/*    fprintf(dbgf, "grl_Max X %i %i\n",LOWORD(GetWindowExt(win_HDC)),LOWORD(GetViewportExt(win_HDC)));*/
/*    fprintf(dbgf, "grl_Max X %p  %i %i\n",win_HDC,LOWORD(GetWindowExt(win_HDC)),LOWORD(GetViewportExt(win_HDC))); */
#endif
    SIZE Size;
    VERIFY(win_HDC); // cnm is Display Open
    /*kga 17/4/1996 this function sometimes returns 0, my attempt to fix it...*/
    if (GetWindowExtEx(win_HDC, &Size))
      RETURN (GR_PIX_COORD)Size.cx;
    else
      RETURN ( 1 );
    /*
    GetWindowExtEx(win_HDC,&Size); 
    RETURN (GR_PIX_COORD)Size.cx;
    */
#endif
#ifdef __GFX__
    RETURN ( _gfx.max_x ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_max_y ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numypixels ) ;
#endif
#ifdef Do__TURBOC__
    RETURN ( getmaxy() ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
/*     fprintf(dbgf, "grl_Max Y %i %i\n",HIWORD(GetWindowExt(win_HDC)),HIWORD(GetViewportExt(win_HDC))); */
/*    fprintf(dbgf, "grl_Max Y %p  %i %i\n",win_HDC,HIWORD(GetWindowExt(win_HDC)),HIWORD(GetViewportExt(win_HDC)));  */
#endif
    SIZE Size;
    VERIFY(win_HDC); // cnm is Display Open
    /*kga 17/4/1996 this function sometimes returns 0, my attempt to fix it...*/
    if (GetWindowExtEx(win_HDC,&Size))
      RETURN (GR_PIX_COORD)Size.cy;
    else
      RETURN ( 1 );
    /*
    GetWindowExtEx(win_HDC,&Size); 
    RETURN (GR_PIX_COORD)Size.cy;
    */
#endif
#ifdef __GFX__
    RETURN ( _gfx.max_y ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_min_x ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    RETURN ( 0 ) ;
#endif
#ifdef Do__TURBOC__
    RETURN ( 0 ) ;
#endif
#ifdef  _CCDK_WIN
    RETURN ( 0 ) ;
#endif
#ifdef __GFX__
    RETURN ( 0 ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_min_y ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    RETURN ( 0 ) ;
#endif
#ifdef Do__TURBOC__
    RETURN ( 0 ) ;
#endif
#ifdef  _CCDK_WIN
    RETURN ( 0 ) ;
#endif
#ifdef __GFX__
    RETURN ( 0 ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_numtextcols ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numtextcols ) ;
#else
    RETURN ( 80 ) ; /* temporarily */
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_numtextrows ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef Do__MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numtextrows ) ;
#else
    RETURN ( 30 ) ; /* temporarily */
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_rectangle ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef Do__MCRGRAPH__
    _rectangle ( _GBORDER, x0, y0, x1, y1 ) ;
#endif
#ifdef Do__TURBOC__
    rectangle ( x0, y0, x1, y1 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_Rect %i %i %i %i\n", x0, y0, x1, y1 );
#endif    
//    Rectangle (win_HDC, x0, y0, x1, y1 ) ;
#endif
#ifdef _LocalClip
    GRL_X = x0 ;
    GRL_Y = y0 ;
    grl_box_corners ( &x0, &y0, &x1, &y1 ) ;
#ifdef __GFX__
    LINE ( x0, y0, x1, y1, GRL_COLOR, EMPTY_BOX, GRL_PATT ) ;
#else
    Rectangle (win_HDC, x0, y0, x1, y1 ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_fill_rectangle ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef Do__MCRGRAPH__
    _rectangle ( _GFILLINTERIOR, x0, y0, x1, y1 ) ;
#endif
#ifdef Do__TURBOC__
    bar ( x0, y0, x1, y1 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_fill_rect\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef _LocalClip
    GRL_X = x0 ;
    GRL_Y = y0 ;
    grl_box_corners ( &x0, &y0, &x1, &y1 ) ;
#ifdef __GFX__
    LINE ( x0, y0, x1, y1, GRL_COLOR, EMPTY_BOX, GRL_PATT ) ;
#else
    Rectangle (win_HDC, x0, y0, x1, y1 ) ;
#endif
//    LINE ( x0, y0, x1, y1, GRL_COLOR, FILL_BOX, GRL_PATT ) ;
#endif
}


#ifdef _LocalClip
/*-------------------------------------------------------------------------*/
STATIC void grl_box_corners ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD *x0, *y0, *x1, *y1 ;
{
    *x0 += GRL_X0 ;
    *x1 += GRL_X0 ;
    *y0 += GRL_Y0 ;
    *y1 += GRL_Y0 ;
    if ( *x0 < GRL_X0 ) 
  *x0 = GRL_X0 ;
    else if ( *x0 > GRL_X1 ) 
  *x0 = GRL_X1 ;
    if ( *x1 < GRL_X0 ) 
  *x1 = GRL_X0 ;
    else if ( *x1 > GRL_X1 ) 
  *x1 = GRL_X1 ;
    if ( *y0 < GRL_Y0 ) 
  *y0 = GRL_Y0 ;
    else if ( *y0 > GRL_Y1 ) 
  *y0 = GRL_Y1 ;
    if ( *y1 < GRL_Y0 ) 
  *y1 = GRL_Y0 ;
    else if ( *y1 > GRL_Y1 ) 
  *y1 = GRL_Y1 ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_outtext ( buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING buffer ;
{
#ifdef Do__MCRGRAPH__
    _outtext ( buffer ) ;
#endif
#ifdef Do__TURBOC__
    outtext ( buffer ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_outtext\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef __GFX__
    printf ( "%s", buffer ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_outtext_xy ( x, y, buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
STRING buffer ;
{
#ifdef Do__MCRGRAPH__
    _settextposition ( y+1, x+1 ) ;
    _outtext ( buffer ) ;
#endif
#ifdef Do__TURBOC__
    struct viewporttype cur_view ;
    getviewsettings ( &cur_view ) ;
    setviewport ( 0, 0, getmaxx(), getmaxy(), 0 ) ;
    outtextxy ( y+1, x+1, buffer ) ;
    setviewport ( cur_view.left, cur_view.top, cur_view.right, 
  cur_view.bottom, 0 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_outtext_xy\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef __GFX__
    LOCATE ( y+1, x+1 ) ;
    printf ( "%s", buffer ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_clip ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef Do__MCRGRAPH__
    _setviewport ( x0, y0, x1, y1 ) ;
#endif
#ifdef Do__TURBOC__
    setviewport ( x0, y0, x1, y1, 1 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_clip\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef _LocalClip
    GRL_X0 = x0 ;
    GRL_X1 = x1 ;
    GRL_Y0 = y0 ;
    GRL_Y1 = y1 ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_unclip __(( void ))
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    GR_PIX_COORD x_max, y_max ;
    x_max = grl_get_max_x();
    y_max = grl_get_max_y();
#ifdef Do__MCRGRAPH__
    _setviewport ( 0, 0, x_max, y_max ) ;
#endif
#ifdef Do__TURBOC__
    setviewport ( 0, 0, x_max, y_max, 0 ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_unclip\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef _LocalClip
    GRL_X0 = 0 ;
    GRL_X1 = x_max ;
    GRL_Y0 = 0 ;
    GRL_Y1 = y_max ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_floodfill ( x, y, color )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
GR_COLOR color ;
{
#ifdef Do__MCRGRAPH__
    (void) _floodfill ( x, y, color ) ;
#endif
#ifdef Do__TURBOC__
    (void) floodfill ( x, y, color ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_floodfill\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef __GFX__
    PAINT ( x, y, GRL_COLOR, color ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_putimage ( x, y, buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
ANY buffer ;
{
#ifdef Do__MCRGRAPH__
    (void) _putimage ( x, y, buffer, _GPSET ) ;
#endif
#ifdef Do__TURBOC__
    (void) putimage ( x, y, buffer, COPY_PUT ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_putimage\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
#endif
#ifdef __GFX__
    PUT_PIC ( x, y, buffer, 'P' ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY grl_getimage ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef Do__MCRGRAPH__
    INT size = (INT)_imagesize ( x0, y0, x1, y1 ) ;
    ANY buffer = MALLOC ( size, char ) ;
    if ( buffer != NULL ) 
  (void) _getimage ( x0, y0, x1, y1, buffer ) ;
    RETURN ( buffer ) ;
#endif
#ifdef Do__TURBOC__
    INT size = (INT)imagesize ( x0, y0, x1, y1 ) ;
    ANY buffer = MALLOC ( size, char ) ;
    if ( buffer != NULL ) 
  (void) getimage ( x0, y0, x1, y1, buffer ) ;
    RETURN ( buffer ) ;
#endif
#ifdef  _CCDK_WIN
#ifdef dbggrl
    fprintf(dbgf, "grl_getimage\n");
#endif    
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
/* ??????????????????????? */
    RETURN ( NULL ) ;
#endif
#ifdef __GFX__
#ifdef __HIGHC24__
    RETURN ( _gfx_get_pic ( x0, y0, x1, y1 ) ) ;
#else
    RETURN ( GET_PIC ( x0, y0, x1, y1 ) ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL grl_get_screen_ratio __(( void ))
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __UNIX__
    RETURN ( GR_SCREEN_RATIO ) ;
#else
    RETURN ( GR_SCREEN_RATIO ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_screen_ratio ( screen_ratio )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL screen_ratio ;
{
    GR_SCREEN_RATIO = screen_ratio ;
}

