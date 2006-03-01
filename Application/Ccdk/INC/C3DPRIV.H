//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** C3DPRIV.h **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C3DPRIV
#define     C3DPRIV
#include    <c3defs.h>

extern  C3_CURVE    c3d_curve __(( void )) ;
extern  C3_CURVE    c3d_polygon_knots __(( PT3* DUMMY0 , INT DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  C3_CURVE    c3d_pcurve_frame __(( INT DUMMY0 , INT DUMMY1 )) ;
#ifdef SPLINE
extern  C3_CURVE    c3d_spline_frame __(( INT DUMMY0 , INT DUMMY1 ,
            C3_KNOT_OPTIONS DUMMY2 )) ;
#endif /* SPLINE */
#endif /* C3DPRIV */

