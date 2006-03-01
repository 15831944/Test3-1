//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3D.C **********************************/
/********** Three-dimensional ellipse construction routines *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3ndefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>
#include <c3hmcrs.h>
#include <c3vmcrs.h>

STATIC void c3d_ellipse_nurb __(( PT3, PT3, PT3, HPT3*, REAL* )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_ellipse ( ctr, major_axis, minor_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 ctr, major_axis, minor_axis ;
{
    C3_CURVE ellipse ;

    ellipse = c3d_curve() ;
    if ( ellipse == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_TYPE(ellipse) = C3_ELLIPSE_TYPE ;
    C3_CURVE_NURB(ellipse) = c3n_create_nurb ( 9, 3 ) ;
    c3n_ellipse ( ctr, major_axis, minor_axis, C3_CURVE_NURB(ellipse) ) ;
    c3n_init_parms ( C3_CURVE_NURB(ellipse), C3_CURVE_PARM0(ellipse), 
        C3_CURVE_PARM1(ellipse) ) ;
    c3n_box ( C3_CURVE_NURB(ellipse), C3_CURVE_BOX(ellipse) ) ; 
    RETURN ( ellipse ) ;
}
#endif  /* SPLINE */

