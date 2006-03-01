//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** C3DDEFS.h **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C3DDEFS
#define     C3DDEFS
#include    <c3defs.h>
#include    <c2defs.h>

extern  void        c3d_free_curve __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3d_line __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_CURVE    c3d_line_dir __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_CURVE    c3d_ray __(( PT3 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  C3_CURVE    c3d_arc __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_arc_3pts __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_arc_2pts_tan __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_arc_2pts_ctr __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_circle __(( PT3 DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_copy __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3d_trim0 __(( C3_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C3_CURVE    c3d_trim1 __(( C3_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C3_CURVE    c3d_trim __(( C3_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  C3_CURVE    c3d_trim_t0 __(( C3_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C3_CURVE    c3d_trim_t1 __(( C3_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C3_CURVE    c3d_trim_t __(( C3_CURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3d_pcurve_through __(( PT3* DUMMY0 , INT DUMMY1 )) ;
extern  C3_CURVE    c3d_pcurve_init_size __(( PT3 DUMMY0 , INT DUMMY1 )) ;
extern  C3_CURVE    c3d_pcurve_init __(( PT3 DUMMY0 )) ;
extern  C3_CURVE    c3d_pcurve_add_arc_2pts __(( C3_CURVE DUMMY0 ,
            PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_pcurve_add_arc_ctr_pt __(( C3_CURVE DUMMY0 ,
            PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_CURVE    c3d_pcurve_add_arc_tan __(( C3_CURVE DUMMY0 , 
            PT3 DUMMY1 )) ;
extern  C3_CURVE    c3d_pcurve_add_line __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_CURVE    c3d_pcurve_add_line_tan __(( C3_CURVE DUMMY0 ,
            PT3 DUMMY1 )) ;
extern  C3_CURVE    c3d_pcurve_remove_last __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3d_pcurve_close __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3d_pcurve_segment __(( C3_CURVE DUMMY0 , INT DUMMY1 )) ;
extern  DML_LIST    c3d_pcurve_smash __(( C3_CURVE DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  C3_CURVE    c3d_pcurve_curves __(( DML_LIST DUMMY0 , REAL DUMMY1 )) ;
extern  DML_LIST    c3d_pcurve_list __(( DML_LIST DUMMY0 , REAL DUMMY1 , 
            DML_LIST DUMMY2 )) ;
extern  C3_CURVE    c3d_pcurve_add_curve __(( C3_CURVE DUMMY0 , 
            C3_CURVE DUMMY1 , REAL DUMMY2 )) ;
extern  C3_CURVE    c3d_ellipse_to_nurb __(( C3_CURVE DUMMY0 )) ;

#ifdef  SPLINE
extern  C3_CURVE    c3d_spline __(( PT3* DUMMY0 , INT DUMMY1 )) ;
extern  C3_CURVE    c3d_nurb __(( HPT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 )) ;
extern  C3_CURVE    c3d_nu_bspline __(( PT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 )) ;
extern  C3_CURVE    c3d_spline_knots __(( PT3* DUMMY0 , INT DUMMY1 ,
            C3_KNOT_OPTIONS DUMMY2 )) ;
extern  C3_CURVE    c3d_spline_tan __(( PT3* DUMMY0 , INT DUMMY1 ,
            C3_KNOT_OPTIONS DUMMY2 , PT3 DUMMY3 , C3_TAN_OPTIONS DUMMY4 ,
            PT3 DUMMY5 , C3_TAN_OPTIONS DUMMY6 )) ;
extern  C3_CURVE    c3d_spline_clsd __(( PT3* DUMMY0 , INT DUMMY1 ,
            C3_KNOT_OPTIONS DUMMY2 )) ;
extern  C3_CURVE    c3d_curve_to_spline __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3d_spline_approx __(( C3_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , INT DUMMY3 )) ;
extern  C3_CURVE    c3d_polygon __(( PT3* DUMMY0 , INT DUMMY1 )) ;
extern  C3_CURVE    c3d_bezier_ctl_pts __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_CURVE    c3d_bezier_2pts_2tans __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_CURVE    c3d_bezier_through __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_CURVE    c3d_bezier_through_opt __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 ,  C3_KNOT_OPTIONS DUMMY4 )) ;
extern  C3_CURVE    c3d_bezier_through_node __(( PT3 DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , REAL DUMMY4 ,  REAL DUMMY5 )) ;
extern  C3_CURVE    c3d_ellipse __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;

#endif  /* SPLINE */

extern  C3_CURVE    c3d_translate __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_CURVE    c3d_scale __(( C3_CURVE DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3d_mirror __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  PT3*        c3d_point __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 )) ;
extern  void        c3d_free_point __(( PT3* DUMMY0 )) ;
extern  C3_CURVE    c3d_rotate __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 )) ;
extern  C3_CURVE    c3d_rotate_cs __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  C3_CURVE    c3d_project_oblique __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_CURVE    c3d_project_normal __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3d_transform __(( C3_CURVE DUMMY0 , 
            C3_TRANSFORM DUMMY1 )) ;
extern  C2_CURVE    c3d_convert_3d2d __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_CURVE    c3d_convert_2d3d __(( C2_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;

#endif /* C3DDEFS */

