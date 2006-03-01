//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3LDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3LDEFS
#define     C3LDEFS
#include <c3defs.h>
#include <c2defs.h>

extern  C3_LINE c3l_create_copy __(( C3_LINE DUMMY0 )) ;
extern  void    c3l_copy __(( C3_LINE DUMMY0 , C3_LINE DUMMY1 )) ;
extern  void    c3l_2pts __(( PT3 DUMMY0 , PT3 DUMMY1 , C3_LINE DUMMY2 )) ;
extern  C3_LINE c3l_create_2pts __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3l_eval __(( C3_LINE DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT3* DUMMY3 )) ;
extern  REAL    c3l_project __(( C3_LINE DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  void    c3l_project_plane __(( C3_LINE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , C3_LINE DUMMY3 )) ;
extern  void    c3l_info __(( C3_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            FILE* DUMMY3 )) ;
extern  void    c3l_box __(( C3_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            C3_BOX DUMMY3 )) ;
extern  REAL    c3l_length __(( C3_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  BOOLEAN c3l_approx __(( C3_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
             PT3* DUMMY3 , PARM DUMMY4 , INT DUMMY5 , INT* DUMMY6 ,
            REAL* DUMMY7 )) ;
extern  BOOLEAN c3l_approx_zoomed __(( C3_LINE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 , REAL DUMMY4 ,  PT3* DUMMY5 ,
            PARM DUMMY6 , INT DUMMY7 , INT* DUMMY8 , REAL* DUMMY9 )) ;
extern  BOOLEAN c3l_select __(( C3_LINE DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 ,  REAL* DUMMY6 ,
            REAL* DUMMY7 )) ;
extern  C2_LINE c3l_conv_3d2d __(( C3_LINE DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , C2_LINE DUMMY4 )) ;
extern  C3_LINE c3l_conv_2d3d __(( C2_LINE DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , C3_LINE DUMMY4 )) ;
extern  BOOLEAN c3l_on_plane __(( C3_LINE DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  BOOLEAN c3l_plane __(( C3_LINE DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  void    c3l_plane_def __(( C3_LINE DUMMY0 , PT3 DUMMY1 , REAL* DUMMY2 ,
            PT3 DUMMY3 [2], INT* DUMMY4 )) ;
extern  C3_LINE c3l_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN c3l_put __(( FILE* DUMMY0 , C3_LINE DUMMY1 , PARM DUMMY2 , 
            PARM DUMMY3 )) ;
extern  C3_LINE c3l_reverse __(( C3_LINE DUMMY0 , C3_LINE DUMMY1 )) ;
extern  void    c3l_translate __(( C3_LINE DUMMY0 , PT3 DUMMY1 , 
            C3_LINE DUMMY2 )) ;
extern  void c3l_scale __(( C3_LINE DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 , C3_LINE DUMMY3 )) ;
extern  void c3l_mirror __(( C3_LINE DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 , C3_LINE DUMMY3 )) ;
extern  void c3l_rotate_cs __(( C3_LINE DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            REAL DUMMY3 , REAL DUMMY4 , C3_LINE DUMMY5 )) ;
extern  BOOLEAN c3l_project_oblique __(( C3_LINE DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 , PT3 DUMMY3 , C3_LINE DUMMY4 )) ;
extern  void c3l_transform __(( C3_LINE DUMMY0 , C3_TRANSFORM DUMMY1 , 
            C3_LINE DUMMY2 )) ;
extern  BOOLEAN  c3l_get_pts_d __(( C3_LINE DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 )) ;
#endif /* C3LDEFS*/

