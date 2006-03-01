//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*************************** C3NDEFS.h **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef      SPLINE
#ifndef     C3NDEFS
#define     C3NDEFS
#include    <c3defs.h>
#include    <c2defs.h>

extern  BOOLEAN     c3n_project __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , PT3 DUMMY3 , PARM DUMMY4 , PT3 DUMMY5 )) ;
extern  C3_NURB     c3n_create_nurb __(( INT DUMMY0 , INT DUMMY1 )) ;
extern  void        c3n_set_ctpts __(( PT3* DUMMY0 , C3_NURB DUMMY1 )) ; 
extern  void        c3n_set_cthpts __(( HPT3* DUMMY0 , C3_NURB DUMMY1 )) ; 
extern  void        c3n_set_knots __(( REAL* DUMMY0 , C3_NURB DUMMY1 )) ; 
extern  void        c3n_set_uniform_knots __(( C3_NURB DUMMY0 )) ; 
extern  void        c3n_set_clsc_uni_knots __(( C3_NURB DUMMY0 )) ; 
extern  C3_NURB     c3n_create_copy_nurb __(( C3_NURB DUMMY0 )) ;
extern  C3_NURB     c3n_trim __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  void        c3n_free_nurb __(( C3_NURB DUMMY0 )) ;
extern  BOOLEAN     c3n_closed __(( C3_NURB DUMMY0 )) ;
extern  C3_NURB     c3n_line __(( C3_LINE DUMMY0 )) ;
extern  C3_NURB     c3n_arc __(( C3_ARC DUMMY0 , BOOLEAN DUMMY1 )) ;
extern  C3_NURB     c3n_pcurve __(( C3_PCURVE DUMMY0 )) ;
extern  void        c3n_reverse __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  BOOLEAN     c3n_parm_adjust __(( C3_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  void        c3n_box __(( C3_NURB DUMMY0 , C3_BOX DUMMY1 )) ;
extern  void        c3n_translate __(( C3_NURB DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3n_scale __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  void        c3n_mirror __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 )) ;
extern  void        c3n_rotate_cs __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            PT3 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN     c3n_project_oblique __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  void    c3n_transform  __(( C3_NURB DUMMY0 , 
            C3_TRANSFORM DUMMY1 )) ;
extern  HPT3*   c3n_get_ctpt __(( C3_NURB DUMMY0 )) ;
extern  INT     c3n_get_n __(( C3_NURB DUMMY0 )) ;
extern  REAL*   c3n_get_knot __(( C3_NURB DUMMY0 )) ;
extern  INT     c3n_get_d __(( C3_NURB DUMMY0 )) ;
extern  REAL    c3n_get_w __(( C3_NURB DUMMY0 )) ;
extern  void    c3n_set_w __(( C3_NURB DUMMY0 , REAL DUMMY1 )) ;
extern  REAL*   c3n_get_normal __(( C3_NURB DUMMY0 )) ;
extern  void    c3n_set_normal __(( C3_NURB DUMMY0 , PT3 DUMMY1 )) ;
extern  C2_NURB c3n_convert_3d2d __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_NURB c3n_convert_2d3d __(( C2_NURB DUMMY0 , PT3 DUMMY1 , 
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  void c3n_init_parms __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  BOOLEAN c3n_trimmed __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 )) ;
extern  BOOLEAN c3n_trimmed0 __(( C3_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN c3n_trimmed1 __(( C3_NURB DUMMY0 , PARM DUMMY1 )) ;
extern  BOOLEAN c3n_interp_tan __(( PT3* DUMMY0 , INT DUMMY1 , 
            C3_KNOT_OPTIONS DUMMY2 , PT3 DUMMY3 , C3_TAN_OPTIONS DUMMY4 , 
            PT3 DUMMY5 , C3_TAN_OPTIONS DUMMY6 , C3_NURB DUMMY7 )) ;
extern  BOOLEAN c3n_interp_clsd __(( PT3* DUMMY0 , INT DUMMY1 , 
            C3_KNOT_OPTIONS DUMMY2 , C3_NURB DUMMY3 )) ;
extern  C3_NURB c3n_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN c3n_put __(( FILE* DUMMY0 , C3_NURB DUMMY1 , PARM DUMMY2 , 
            PARM DUMMY3 )) ;
extern  void    c3n_mass_prop __(( C3_NURB DUMMY0 , PARM DUMMY1 , PARM DUMMY2 ,
            INT DUMMY3 ,  INT DUMMY4 , REAL DUMMY5 , PF_PROP3 DUMMY6 , 
            REAL* DUMMY7 )) ;
extern  BOOLEAN c3n_get_ellipse_data __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN c3n_approx __(( C3_NURB DUMMY0 , PARM DUMMY1 , PARM DUMMY2 ,
            PARM DUMMY3 , PARM DUMMY4 , REAL DUMMY5 , BOOLEAN DUMMY6 , 
            PT3* DUMMY7 , PARM DUMMY8 , INT DUMMY9 , INT* DUMMY10 , 
            PARM DUMMY11 )) ;
extern  BOOLEAN c3n_approx_zoomed __(( C3_NURB DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 ,  PARM DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 , 
            REAL DUMMY6 , PT3* DUMMY7 , PARM DUMMY8 , 
            INT DUMMY9 , INT* DUMMY10 , PARM DUMMY11 )) ;
extern  BOOLEAN c3n_eval __(( C3_NURB DUMMY0 , PARM DUMMY1 , INT DUMMY2 , 
            PT3* DUMMY3 )) ;
extern  BOOLEAN c3n_on_plane __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 )) ; 
extern  C3_PLANE_STATUS c3n_plane __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  void c3n_plane_def __(( C3_NURB DUMMY0 , PT3 DUMMY1 , 
            REAL* DUMMY2 , PT3[2] DUMMY3 , INT* DUMMY3 )) ;
extern  void c3n_info __(( C3_NURB DUMMY0 , FILE* DUMMY1 )) ;
extern  void c3n_info_ellipse __(( C3_NURB DUMMY0 , FILE* DUMMY1 )) ;
extern  BOOLEAN c3n_select __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 , 
            PARM DUMMY6 , REAL* DUMMY7 )) ;
extern  void c3n_ellipse __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            C3_NURB DUMMY3 )) ;
#endif /* C3NDEFS*/ 
#endif /* SPLINE */

