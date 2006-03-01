//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3SDEFS.C ********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef      SPLINE
#ifndef     C3SDEFS
#define     C3SDEFS
#include    <c3defs.h>
#include    <alsdefs.h>

#define C3S_CONV_BEZ(A,D,KNOT,J,B) \
als_conv_bez((REAL*)((A)+(J)-(D)+1),(D),(KNOT)+(J)-(D)+1,4,(REAL*)(B))

extern  BOOLEAN c3s_interp_knots __(( PT3* DUMMY0 , INT DUMMY1 , 
            C3_KNOT_OPTIONS DUMMY2 , REAL* DUMMY3 )) ;
extern  BOOLEAN c3s_clsd_interp_knots __(( PT3* DUMMY0 , INT DUMMY1 ,
            C3_KNOT_OPTIONS DUMMY2 , REAL* DUMMY3 )) ;
extern  BOOLEAN c3s_interp_tan __(( PT3* DUMMY0 , INT DUMMY1 , 
            C3_KNOT_OPTIONS DUMMY2 , PT3 DUMMY3 , C3_TAN_OPTIONS DUMMY4 , 
            PT3 DUMMY5 , C3_TAN_OPTIONS DUMMY6 , PT3* DUMMY7 , REAL* DUMMY8 )) ;
extern  BOOLEAN c3s_interp_clsd __(( PT3* DUMMY0 , INT DUMMY1 , 
            C3_KNOT_OPTIONS DUMMY2 , PT3* DUMMY3 , REAL* DUMMY4 )) ;
extern  BOOLEAN c3s_eval __(( HPT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 , PARM DUMMY6 , 
            INT DUMMY7 , PT3* DUMMY8 )) ;
extern  BOOLEAN c3s_eval_tan __(( HPT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , PARM DUMMY4 , PT3 DUMMY5 )) ;
extern  BOOLEAN c3s_project __(( HPT3* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PT3 DUMMY4 , PARM DUMMY5 , PARM DUMMY6 , 
            PT3 DUMMY7 , PARM DUMMY8 , PT3 DUMMY9 )) ;
extern  BOOLEAN c3s_approx __(( HPT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 , PARM DUMMY6 ,
            PARM DUMMY7 , REAL DUMMY8 , BOOLEAN DUMMY9 , PT3* DUMMY10 ,
            PARM DUMMY11 , INT DUMMY12 , INT* DUMMY13 , PARM DUMMY14 )) ;
extern  BOOLEAN c3s_approx_zoomed __(( HPT3* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 , 
            PARM DUMMY6 , PARM DUMMY7 , REAL DUMMY8 , PT3 DUMMY9 , 
            REAL DUMMY10 , PT3* DUMMY11 , PARM DUMMY12 , INT DUMMY13 , 
            INT* DUMMY14 , PARM DUMMY15 )) ;
extern  void    c3s_info __(( HPT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , FILE* DUMMY4 )) ;
extern  void    c3s_info_ellipse __(( HPT3* DUMMY0 , FILE* DUMMY1 )) ;
extern  BOOLEAN c3s_select __(( HPT3* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PT3 DUMMY4 , PARM DUMMY5 , PARM DUMMY6 , 
            PT3 DUMMY7 , PT3 DUMMY8 , REAL DUMMY9 , PARM DUMMY10 , 
            REAL* DUMMY11 )) ;
extern  void    c3s_mass_prop __(( HPT3* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PT3 DUMMY4 , PARM DUMMY5 , PARM DUMMY6 ,
            INT DUMMY7 ,  INT DUMMY8 , REAL DUMMY9 , 
            PF_PROP3 DUMMY10 , REAL* DUMMY11 )) ;
extern  BOOLEAN c3s_get_ellipse_data __(( HPT3* DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN c3s_on_plane __(( HPT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 )) ;
extern  C3_PLANE_STATUS c3s_plane __(( HPT3* DUMMY0 , INT DUMMY1 ,
            PT3 DUMMY2 , REAL* DUMMY3 )) ;
extern  void    c3s_plane_def __(( HPT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            REAL* DUMMY3 , PT3 DUMMY4 [2], INT* DUMMY5 )) ;
extern  void    c3s_pcurve __(( C3_PCURVE DUMMY0 , HPT3* DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  BOOLEAN  c3s_get __(( FILE* DUMMY0 , HPT3* DUMMY1 , INT DUMMY2 , 
            INT DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 , PT3 DUMMY6 )) ;
extern  BOOLEAN  c3s_put __(( FILE* DUMMY0 , HPT3* DUMMY1 , INT DUMMY2 , 
            INT DUMMY3 , REAL* DUMMY4 , REAL DUMMY5 , PT3 DUMMY6 , 
            PARM DUMMY7 , PARM DUMMY8 )) ;
extern  void c3s_ellipse __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            HPT3* DUMMY3 , REAL* DUMMY4 )) ;
extern  void    c3s_reverse __(( HPT3* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , PARM DUMMY4 , PARM DUMMY5 )) ;
#endif  /* C3SDEFS */
#endif  /* SPLINE */

