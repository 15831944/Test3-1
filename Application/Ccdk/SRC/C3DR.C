//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3DR.C *********************************/
/********** Three-dimensional polygon construction routines *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef SPLINE
#include <aladefs.h>
#include <c3ndefs.h>
#include <c3adefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_polygon ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
{
    RETURN ( c3d_polygon_knots ( a, n, NULL ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE c3d_polygon_knots ( a, n, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
REAL *knot  ;
{
    C3_CURVE curve ;

    curve = c3d_curve() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;

    C3_CURVE_NURB(curve) = c3n_create_nurb ( n, 2 ) ;
    if ( C3_CURVE_NURB(curve) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }

    C3_CURVE_TYPE(curve) = C3_NURB_TYPE ;
    c3n_set_ctpts ( a, C3_CURVE_NURB(curve) ) ;
    c3n_set_knots ( knot, C3_CURVE_NURB(curve) ) ;
    c3n_init_parms ( C3_CURVE_NURB(curve), C3_CURVE_PARM0(curve), 
        C3_CURVE_PARM1(curve) ) ;
    c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;

}
#endif /* SPLINE */

