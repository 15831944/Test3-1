//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3EDEFS.H *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef C3EDEFS
#define C3EDEFS
#include <c3defs.h>

extern  BOOLEAN c3e_ratio __(( HPT3* DUMMY0 , INT DUMMY1 , PT3* DUMMY2 )) ;
extern  INT     c3e_curvature __(( PT3 DUMMY0 [3], REAL* DUMMY1 , 
            PT3 DUMMY2 )) ;
extern  void    c3e_polynomial2 __(( HPT3 DUMMY0 [3], REAL DUMMY1 ,
            INT DUMMY2 , HPT3* DUMMY3 )) ;
#ifdef SPLINE
extern  BOOLEAN c3e_offset __(( PT3* DUMMY0 , REAL DUMMY1 , PT3 DUMMY2 , 
            INT DUMMY3 , PT3* DUMMY4 )) ;
extern  BOOLEAN c3e_normal __(( PT3* DUMMY0 , PT3 DUMMY2 , INT DUMMY1 , 
            PT3* DUMMY2 )) ;
#endif  /* SPLINE */
#endif /* C3EDEFS */


