//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3VDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef C3VDEFS
#define C3VDEFS
#include <c3defs.h>
#include <c2defs.h>

extern  BOOLEAN     c3v_is_small __(( PT3 DUMMY0 )) ;
extern  BOOLEAN     c3v_ident_pts __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL*       c3v_set_zero __(( PT3 DUMMY0 )) ;
extern  REAL*       c3v_set __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL*       c3v_copy __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL*       c3v_add __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  REAL*       c3v_addt __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL*       c3v_addw __(( PT3 DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , PT3 DUMMY4 )) ;
extern  REAL*       c3v_addc __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 )) ;
extern  REAL*       c3v_addu __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL*       c3v_sub __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  REAL*       c3v_negate __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL*       c3v_mid_pt __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  REAL*       c3v_scale __(( PT3 DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 )) ;
extern  REAL        c3v_dot __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL*       c3v_cross __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  BOOLEAN     c3v_unit_normal __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  REAL        c3v_triple_prod __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  REAL        c3v_norm __(( PT3 DUMMY0 )) ;
extern  REAL        c3v_norm_squared __(( PT3 DUMMY0 )) ;
extern  BOOLEAN     c3v_normalize __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3v_normalize_l1 __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_norml1 __(( PT3 DUMMY0 )) ;
extern  REAL        c3v_dist __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_dist_squared __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_distl1 __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3v_vecs_parallel __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_vecs_sin __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_vecs_cos __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_vecs_angle __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3v_vec_angle_z __(( PT3 DUMMY0 )) ;
extern  REAL        c3v_vec_angle_xy __(( PT3 DUMMY0 )) ;
extern  void        c3v_spherical __(( PT3 DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 , REAL* DUMMY3 )) ;
extern  REAL*       c3v_set_spherical __(( REAL DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  REAL        c3v_project_line __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  REAL*       c3v_project_pt_normal __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN     c3v_project_pt_oblique __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  REAL*       c3v_project_vec_normal __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  BOOLEAN     c3v_project_vec_oblique __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  REAL*       c3v_mirror_pt __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL*       c3v_mirror_vec __(( PT3 DUMMY0 , PT3 DUMMY1 , 
            PT3 DUMMY2 )) ;
extern  BOOLEAN     c3v_basis __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  BOOLEAN     c3v_basis_orthogonal __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3v_coord_sys __(( PT3 DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  REAL*       c3v_rotate_pt __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , PT3 DUMMY4 )) ;
extern  REAL*       c3v_rotate_pt_cs __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 )) ;
extern  REAL*       c3v_rotate_vec __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  REAL*       c3v_rotate_vec_cs __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN     c3v_plane_3pts __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN     c3v_pt_on_plane __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN     c3v_vec_on_plane __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL*       c3v_conv_pt_3d2d __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , PT2 DUMMY4 )) ;
extern  REAL*       c3v_conv_vec_3d2d __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT2 DUMMY3 )) ;
extern  REAL*       c3v_conv_pt_2d3d __(( PT2 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  REAL*       c3v_conv_vec_2d3d __(( PT2 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
#endif /*C3VDEFS*/

