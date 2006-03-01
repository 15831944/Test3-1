//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3GDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3GDEFS
#define     C3GDEFS
#include <c3defs.h>
#include <c2defs.h>
extern  C3_ARC  c3g_create_copy __(( C3_ARC DUMMY0 )) ;
extern  void    c3g_eval __(( C3_ARC DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT3* DUMMY3 )) ;
extern  void    c3g_pt_tan __(( C3_ARC DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL    c3g_length __(( C3_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  BOOLEAN c3g_select __(( C3_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 ,  REAL* DUMMY6 ,
            REAL* DUMMY7 )) ;
extern  BOOLEAN c3g_project __(( C3_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , REAL* DUMMY4 , PT3 DUMMY5 )) ;
extern  BOOLEAN c3g_approx __(( C3_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , BOOLEAN DUMMY4 ,  PT3* DUMMY5 , PARM DUMMY6 ,
            INT DUMMY7 , INT* DUMMY8 , REAL* DUMMY9 )) ;
extern  BOOLEAN c3g_approx_zoomed __(( C3_ARC DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 ,
             PT3* DUMMY6 , PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 ,
            REAL* DUMMY10 )) ;
extern  void    c3g_box __(( C3_ARC DUMMY0 , BOOLEAN DUMMY1 , C3_BOX DUMMY2 )) ;
extern  C2_ARC  c3g_conv_3d2d __(( C3_ARC DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , C2_ARC DUMMY4 )) ;
extern  C3_ARC  c3g_conv_2d3d __(( C2_ARC DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , C3_ARC DUMMY4 )) ;
extern  INT     c3g_bezs __(( C3_ARC DUMMY0 , BOOLEAN DUMMY1 , HPT3* DUMMY2 )) ;
extern  void    c3g_info __(( C3_ARC DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            FILE* DUMMY3 )) ;
extern  BOOLEAN c3g_on_plane __(( C3_ARC DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  C3_PLANE_STATUS c3g_plane __(( C3_ARC DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  void    c3g_plane_def __(( C3_ARC DUMMY0 , PT3 DUMMY1 , REAL* DUMMY2 ,
            PT3 DUMMY3 [2], INT* DUMMY4 )) ;
extern  C3_ARC c3g_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN c3g_put __(( FILE* DUMMY0 , C3_ARC DUMMY1 , PARM DUMMY2 , 
            PARM DUMMY3 )) ;
extern  C3_ARC c3g_reverse __(( C3_ARC DUMMY0 , C3_ARC DUMMY1 )) ;
extern  void   c3g_translate __(( C3_ARC DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3g_scale __(( C3_ARC DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  void    c3g_mirror __(( C3_ARC DUMMY0 , PT3 DUMMY1, REAL DUMMY2 )) ;
extern  void    c3g_rotate_cs __(( C3_ARC DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN c3g_project_oblique __(( C3_ARC DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN c3g_transform __(( C3_ARC DUMMY0 , C3_TRANSFORM DUMMY1 )) ;
extern  void    c3g_arc_epts_d __(( C3_ARC DUMMY0 , PT3 DUMMY1 , 
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_ARC  c3g_epts_d __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            C3_ARC DUMMY3 )) ;
extern  C3_ARC  c3g_3pts __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            C3_ARC DUMMY3 )) ;
extern  C3_ARC  c3g_2pts_tan __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            C3_ARC DUMMY3 )) ;
extern  C3_ARC  c3g_2pts_ctr __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            C3_ARC DUMMY3 )) ;
extern  C3_ARC c3g_circle __(( PT3 DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 , 
            C3_ARC DUMMY3 )) ;
extern  REAL *c3g_ctr __(( C3_ARC DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL *c3g_normal __(( C3_ARC DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL c3g_rad __(( C3_ARC DUMMY0 )) ;
extern  REAL c3g_t_to_angle __(( C3_ARC DUMMY0 , REAL DUMMY1 )) ;
extern  REAL c3g_angle_to_t __(( C3_ARC DUMMY0 , REAL DUMMY1 )) ;
extern  REAL c3g_d_parm __(( C3_ARC DUMMY0 )) ;
extern  BOOLEAN  c3g_get_pts_d __(( C3_ARC DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 )) ;
#endif /* C3GDEFS*/

