//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3APRIV.H *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef C3APRIV
#define C3APRIV
#include <c3defs.h>
#include <c2pt2.h>
#include <c2hpt2.h>

extern  void    c3a_get_pt __(( PT3 DUMMY0 )) ;
extern  void    c3a_print_pt __(( FILE* DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3a_conv_pt_3d2d __(( PT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , PT2* DUMMY5 )) ;
extern  void    c3a_conv_pt_2d3d __(( PT2* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , PT3* DUMMY5 )) ;
#ifdef  SPLINE
extern  BOOLEAN c3a_non_rational __(( HPT3* DUMMY0 , INT DUMMY1 )) ;
extern  C3_BOX  c3a_box_hpoly __(( HPT3* DUMMY0 , INT DUMMY1 , 
            C3_BOX DUMMY2 )) ;
extern  C3_BOX  c3a_box_init_hpt __(( C3_BOX DUMMY0 , HPT3 DUMMY1 )) ;
extern  C3_BOX  c3a_box_append_hpt __(( C3_BOX DUMMY0 , HPT3 DUMMY1 )) ;
extern  void    c3a_print_hpt __(( FILE* DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3a_sub __(( HPT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            HPT3* DUMMY3 )) ;
extern  void    c3a_pt_hpt __(( PT3* DUMMY0 , INT DUMMY1 , HPT3* DUMMY2 )) ;
extern  BOOLEAN c3a_hpt_pt __(( HPT3* DUMMY0 , INT DUMMY1 , PT3* DUMMY2 )) ;
extern  REAL    c3a_hpoly_flat __(( HPT3* DUMMY0 , INT DUMMY1 )) ;
extern  REAL    c3a_poly_flat __(( PT3* DUMMY0 , INT DUMMY1 )) ;
extern  void    c3a_rot_cs_hpt __(( HPT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , HPT3* DUMMY6 )) ;
extern  void    c3a_rot_cs_pt __(( PT3* DUMMY0 , INT DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , PT3* DUMMY6 )) ;
extern  void    c3a_conv_hpt_2d3d __(( HPT2* DUMMY0 , INT DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , HPT3* DUMMY5 )) ;
extern  void    c3a_conv_hpt_3d2d __(( HPT3* DUMMY0 , INT DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , HPT2* DUMMY5 )) ;
extern  BOOLEAN c3a_project_hpt_oblique __(( HPT3* DUMMY0 , INT DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , PT3 DUMMY4 , HPT3* DUMMY5 )) ;
extern  void    c3a_transform_hpt __(( HPT3* DUMMY0 , INT DUMMY1 ,
            C3_TRANSFORM DUMMY2 , HPT3* DUMMY3 )) ;
#endif  /* SPLINE */
#endif  /* C3APRIV */

