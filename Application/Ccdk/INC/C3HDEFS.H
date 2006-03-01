//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3HDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef SPLINE
#ifndef C3HDEFS
#define C3HDEFS
#include <c3defs.h>
#include <c2defs.h>
extern  void        c3h_set_zero __(( HPT3 DUMMY0 )) ;
extern  void        c3h_set __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 , 
            REAL DUMMY3 , HPT3 DUMMY4 )) ;
extern  void        c3h_copy __(( HPT3 DUMMY0 , HPT3 DUMMY1 )) ;
extern  void        c3h_diff __(( HPT3 DUMMY0 , HPT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  void        c3h_sub __(( HPT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  void        c3h_addv __(( HPT3 DUMMY0 , PT3 DUMMY1 , HPT3 DUMMY2 )) ;
extern  void        c3h_mirror __(( HPT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            HPT3 DUMMY3 )) ;
extern  BOOLEAN     c3h_hpt_pt __(( HPT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3h_pt_hpt __(( PT3 DUMMY0 , HPT3 DUMMY1 )) ;
extern  BOOLEAN     c3h_project_oblique __(( HPT3 DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 , HPT3 DUMMY4 )) ;
extern  void        c3h_rotate __(( HPT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , PT3 DUMMY4 )) ;
extern  void        c3h_rotate_cs __(( HPT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 )) ;
extern  BOOLEAN     c3h_on_plane __(( HPT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  void        c3h_plane_def __(( HPT3 DUMMY0 , PT3 DUMMY1 [2],
            PT3 DUMMY2 , REAL* DUMMY3 , INT* DUMMY4 )) ;
extern  void        c3h_conv_3d2d __(( HPT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , HPT2 DUMMY4 )) ;
extern  void        c3h_conv_2d3d __(( HPT2 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 , HPT3 DUMMY4 )) ;
#endif /*C3HDEFS*/

#endif /*SPLINE*/

