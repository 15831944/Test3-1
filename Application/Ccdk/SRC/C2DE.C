//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3*/
/******************************* C2DE.C *********************************/
/********** Two-dimensional curve construction routines *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c2ndefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c2vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c2d_ellipse ( ctr, major_axis, minor_axis, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 ctr ;
REAL major_axis, minor_axis, angle ;
{
    C2_CURVE ellipse = c2d_curve() ;
    if ( ellipse == NULL ) 
        RETURN ( NULL ) ;

    C2_CURVE_TYPE(ellipse) = C2_ELLIPSE_TYPE ;
    C2_CURVE_NURB(ellipse) = 
        c2n_ellipse ( ctr, major_axis, minor_axis, angle ) ;
    c2n_box ( C2_CURVE_NURB(ellipse), C2_CURVE_BOX(ellipse) ) ;
    C2_CURVE_T0(ellipse) = 0.0 ;
    C2_CURVE_J0(ellipse) = 2 ;
    C2_CURVE_T1(ellipse) = 2.0 ;
    C2_CURVE_J1(ellipse) = 8 ;
    RETURN ( ellipse ) ;
}
#endif  /* SPLINE */

