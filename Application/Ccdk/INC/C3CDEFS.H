//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3CDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3CDEFS
#define     C3CDEFS
#include    <dmdefs.h>
#include    <c3defs.h>
#include    "tststr.h"

extern  BOOLEAN     c3c_eval __(( C3_CURVE DUMMY0 , PARM DUMMY1 , INT DUMMY2 ,
            PT3* DUMMY3 )) ;
extern  BOOLEAN     c3c_eval_pt __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  BOOLEAN     c3c_eval_tan __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  BOOLEAN     c3c_eval_pt_tan __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN     c3c_ept0 __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3c_etan0 __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3c_ept_tan0 __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  BOOLEAN     c3c_ept1 __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3c_etan1 __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3c_ept_tan1 __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  BOOLEAN     c3c_closed __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3c_copy __(( C3_CURVE DUMMY0 , C3_CURVE DUMMY1 )) ;
extern  BOOLEAN     c3c_project __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PARM DUMMY2 , PT3 DUMMY3 )) ;
extern  void        c3c_info_curve __(( C3_CURVE DUMMY0 , FILE* DUMMY1 )) ;
extern  C3_CURVE    c3c_translate __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_CURVE    c3c_scale __(( C3_CURVE DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3c_mirror __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3c_rotate __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 )) ;
extern  C3_CURVE    c3c_rotate_cs __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN     c3c_approx __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , REAL DUMMY3 , BOOLEAN DUMMY4 , PT3* DUMMY5 ,
            PARM DUMMY6 , INT DUMMY7 , INT* DUMMY8 )) ;
extern  void        c3c_approx_init __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , BOOLEAN DUMMY3 )) ;
extern  C3_CURVE    c3c_trim0 __(( C3_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C3_CURVE    c3c_trim1 __(( C3_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  C3_CURVE    c3c_trim __(( C3_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  C3_CURVE    c3c_trim_t0 __(( C3_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C3_CURVE    c3c_trim_t1 __(( C3_CURVE DUMMY0 , REAL DUMMY1 )) ;
extern  C3_CURVE    c3c_trim_t __(( C3_CURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3c_parm_adjust __(( C3_CURVE DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN     c3c_select __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , PARM DUMMY4 , REAL* DUMMY5 )) ;
extern  REAL        c3c_length __(( C3_CURVE DUMMY0 )) ;
extern  C3_CURVE    c3c_project_normal __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_CURVE    c3c_project_oblique __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_CURVE    c3c_transform __(( C3_CURVE DUMMY0 , 
            C3_TRANSFORM DUMMY1 )) ;
extern  void        c3c_display __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , REAL DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 ,
             PF_DISPLAY DUMMY6 , ANY DUMMY7 )) ;
extern  BOOLEAN     c3c_get_arc_data __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 , PT3 DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN     c3c_get_arc_center __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3c_get_arc_radius __(( C3_CURVE DUMMY0 , REAL* DUMMY1 )) ;
extern  BOOLEAN     c3c_get_arc_normal __(( C3_CURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3c_get_arc_sweep __(( C3_CURVE DUMMY0 , REAL* DUMMY1 )) ;
extern  BOOLEAN     c3c_arc_angle __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  BOOLEAN     c3c_arc_parm __(( C3_CURVE DUMMY0 , REAL DUMMY1 ,
            PARM DUMMY2 )) ;
extern  BOOLEAN     c3c_get_arc_pts_d __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN     c3c_get_pcurve_data __(( C3_CURVE DUMMY0 , INT DUMMY1 ,
            BOOLEAN* DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 ,  REAL* DUMMY5 ,
            PT3 DUMMY6 , REAL* DUMMY7 )) ;
extern  BOOLEAN     c3c_on_plane __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN     c3c_plane __(( C3_CURVE DUMMY0 , PT3 DUMMY1 , REAL* DUMMY2 ,
            C3_PLANE_STATUS* DUMMY3 )) ;
extern  BOOLEAN     c3c_curves_plane __(( DML_LIST DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 ,  C3_PLANE_STATUS* DUMMY3 )) ;
extern  BOOLEAN     c3c_eval_equi_parm __(( C3_CURVE DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , INT DUMMY3 , PT3* DUMMY4 , PT3 DUMMY5 , 
            PT3 DUMMY6 )) ; 
extern  DML_LIST c3c_read_curves __(( STRING DUMMY0 , DML_LIST DUMMY1 )) ;
extern  DML_LIST c3c_get_curves __(( FILE* DUMMY0 , DML_LIST DUMMY1 )) ;
extern  C3_CURVE c3c_get_curve __(( FILE* DUMMY0 )) ;
extern  BOOLEAN  c3c_write_curves __(( STRING DUMMY0 , DML_LIST DUMMY1 )) ;
extern  BOOLEAN  c3c_write_curve __(( STRING DUMMY0 , C3_CURVE DUMMY1 )) ;
extern  BOOLEAN  c3c_put_curves __(( FILE* DUMMY0 , DML_LIST DUMMY1 )) ;
extern  BOOLEAN  c3c_put_curve __(( FILE* DUMMY0 , C3_CURVE DUMMY1 )) ;
extern  BOOLEAN  c3c_put __(( STRING DUMMY0 , C3_CURVE DUMMY1 )) ;
#ifdef  SPLINE
extern  BOOLEAN     c3c_get_ellipse_data __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN c3c_ellipse_to_nurb __(( C3_CURVE DUMMY0 )) ;
#endif /*SPLINE*/
extern  BOOLEAN c3c_select_face __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 , BOOLEAN DUMMY6 , 
            REAL* DUMMY7 )) ;
extern  BOOLEAN c3c_select_triangle __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , PT3 DUMMY4 , REAL* DUMMY5 )) ;
extern  C3_CURVE c3c_reverse __(( C3_CURVE DUMMY0 )) ;
extern  REAL c3c_pnts_plane_fit __(( PT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 , 
            REAL* DUMMY3 )) ;
extern  DML_LIST c3c_sort_curve_list __(( DML_LIST DUMMY0 , PF_EVAL3 DUMMY1 , 
            PF_EVAL3 DUMMY2 , BOOLEAN DUMMY3 , DML_LIST DUMMY4 )) ;


// mhmnew

#define C3_CURVE_LAYER_GET(c)           STRTEST(c->layer.p)
#define C3_CURVE_LAYER_SET(c,value)     (dx0_strass(&c->layer,value))


// mhmnew




#endif /* C3CDEFS*/

