//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* VP3DEFS.H *********************************/
/*************************** View prototypes *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef VP3DEFS
#define VP3DEFS

#include <c2defs.h>
#include <c3defs.h>
#include <vpdefs.h>

extern  VP_VIEWPORT vp3_top __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
            REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;
extern  VP_VIEWPORT vp3_front __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;
extern  VP_VIEWPORT vp3_right __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;
extern  VP_VIEWPORT vp3_isometric __(( REAL DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,
            REAL DUMMY6 ,  REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 ,
            GR_COLOR DUMMY10 )) ;
extern  VP_VIEWPORT vp3_create __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 ,  REAL DUMMY5 , REAL DUMMY6 ,
            REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;
extern  VP_VIEWPORT vp3_set_view_box __(( VP_VIEWPORT DUMMY0 , 
            C3_BOX DUMMY1 )) ;
extern  VP_VIEWPORT vp3_fit __(( VP_VIEWPORT DUMMY0 , C3_BOX DUMMY1 ,
            REAL DUMMY2 )) ;
extern  REAL*       vp3_get_ctr __(( VP_VIEWPORT DUMMY0 , PT3 DUMMY1 )) ;
extern  VP_VIEWPORT vp3_set_ctr __(( VP_VIEWPORT DUMMY0 , PT3 DUMMY1 )) ;
extern  INT*        vp3_point_to_pixel __(( PT3 DUMMY0 , VP_VIEWPORT DUMMY1 ,
            INT DUMMY2 [2] )) ;
extern  REAL*       vp3_pixel_to_point __(( INT DUMMY0 [2], VP_VIEWPORT DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  REAL*       vp3_pixel_to_point_real __(( REAL DUMMY0 [2], VP_VIEWPORT DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  void        vp3_get_view __(( VP_VIEWPORT DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  VP_VIEWPORT vp3_set_view __(( VP_VIEWPORT DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  void        vp3_get_view_dir __(( VP_VIEWPORT DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  VP_VIEWPORT vp3_set_view_dir __(( VP_VIEWPORT DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  VP_VIEWPORT vp3_append_transform __(( VP_VIEWPORT DUMMY0 ,
            C3_TRANSFORM DUMMY1 )) ;
#endif /* VP3DEFS */


