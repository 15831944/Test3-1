//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3RDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C3RDEFS
#define     C3RDEFS
#include <c3defs.h>
#include <c2defs.h>

extern  BOOLEAN c3r_eval __(( C3_ASEG DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT3* DUMMY3 )) ;
extern  BOOLEAN c3r_pt_tan __(( C3_ASEG DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  void    c3r_tan0 __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3r_tan1 __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3r_bez __(( C3_ASEG DUMMY0 , HPT3* DUMMY1 )) ;
extern  REAL*   c3r_mid_pt __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3r_bez_mid_pt __(( C3_ASEG DUMMY0 , HPT3 DUMMY1 )) ;
extern  void    c3r_ab __(( C3_ASEG DUMMY0 , REAL* DUMMY1 , REAL* DUMMY2 )) ;
extern  void    c3r_box __(( C3_ASEG DUMMY0 , C3_BOX DUMMY1 )) ;
extern  void    c3r_box_append __(( C3_ASEG DUMMY0 , C3_BOX DUMMY1 )) ;
extern  INT     c3r_coord_extrs __(( C3_ASEG DUMMY0 , INT DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  REAL    c3r_length __(( C3_ASEG DUMMY0 )) ;
extern  BOOLEAN c3r_select __(( C3_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 ,  REAL* DUMMY6 ,
            REAL* DUMMY7 )) ;
extern  BOOLEAN c3r_project __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , REAL* DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL    c3r_ctr_rad __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL    *c3r_ctr __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL    c3r_rad __(( C3_ASEG DUMMY0 )) ;
extern  REAL    c3r_flatness __(( C3_ASEG DUMMY0 )) ;
extern  REAL    *c3r_normal __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN c3r_get_data __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL* DUMMY3 , PT3 DUMMY4 , REAL* DUMMY5 )) ;
extern  REAL    c3r_curvature __(( C3_ASEG DUMMY0 )) ;
extern  void    c3r_sbdv __(( C3_ASEG DUMMY0 , C3_ASEG DUMMY1 , 
            C3_ASEG DUMMY2 )) ;
extern  C3_ASEG c3r_trim __(( C3_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            C3_ASEG DUMMY3 )) ;
extern  REAL    c3r_d_parm __(( C3_ASEG DUMMY0 )) ;
extern  C3_ASEG c3r_reverse __(( C3_ASEG DUMMY0 , C3_ASEG DUMMY1 )) ;
extern  C3_ASEG c3r_complement __(( C3_ASEG DUMMY0 , C3_ASEG DUMMY1 )) ;
extern  BOOLEAN c3r_approx __(( C3_ASEG DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , BOOLEAN DUMMY5 ,  PT3* DUMMY6 ,
            PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 , REAL* DUMMY10 )) ;
extern  BOOLEAN c3r_approx_zoomed __(( C3_ASEG DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , C3_BOX DUMMY5 ,
             REAL DUMMY6 , PT3* DUMMY7 , PARM DUMMY8 , INT DUMMY9 ,
            INT* DUMMY10 , REAL* DUMMY11 )) ;
extern  C3_ASEG c3r_3pts __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            C3_ASEG DUMMY3 )) ;
extern  C3_ASEG c3r_2pts_tan __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            C3_ASEG DUMMY3 )) ;
extern  C3_ASEG c3r_circle __(( PT3 DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 ,
            C3_ASEG DUMMY3 )) ;
extern  C3_ASEG c3r_2pts_ctr __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            C3_ASEG DUMMY3 )) ;
extern  C3_ASEG c3r_line __(( PT3 DUMMY0 , PT3 DUMMY1 , C3_ASEG DUMMY2 )) ;
extern  C3_ASEG c3r_copy __(( C3_ASEG DUMMY0 , C3_ASEG DUMMY1 )) ;
extern  void    c3r_translate __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3r_scale __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  void    c3r_mirror __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  void    c3r_rotate_cs __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN c3r_on_plane __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  BOOLEAN c3r_is_normal __(( C3_ASEG DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_PLANE_STATUS c3r_plane __(( C3_ASEG DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  void    c3r_plane_def __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , REAL* DUMMY2 ,
            PT3 DUMMY3 [2], INT* DUMMY4 )) ;
extern  BOOLEAN c3r_project_oblique __(( C3_ASEG DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN c3r_transform __(( C3_ASEG DUMMY0 , C3_TRANSFORM DUMMY1 )) ;
extern  C3_ASEG c3r_epts_d __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , C3_ASEG DUMMY3 )) ;
extern  C2_ASEG c3r_conv_3d2d __(( C3_ASEG DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , C2_ASEG DUMMY0 )) ;
extern  C3_ASEG c3r_conv_2d3d __(( C2_ASEG DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , C3_ASEG DUMMY0 )) ;
extern  REAL*   c3r_get_d_vec __(( C3_ASEG DUMMY0 )) ;
extern  REAL    c3r_t_to_angle __(( C3_ASEG DUMMY0 , REAL DUMMY1 )) ;
extern  REAL c3r_angle_to_t __(( C3_ASEG DUMMY0 , REAL DUMMY1 )) ;
extern  BOOLEAN  c3r_get_pts_d __(( C3_ASEG DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 )) ;

#endif /* C3RDEFS*/

