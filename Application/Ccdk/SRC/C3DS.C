//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3D.C **********************************/
/********* Three-dimensional curve construction routines ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3ndefs.h>
#include <c3adefs.h>
#include <c3cdefs.h>
#include <c3dpriv.h>
#include <c3ddefs.h>
#include <c3vmcrs.h>

STATIC  BOOLEAN     c3d_spline_header0 __(( C3_CURVE, INT, INT )) ;
STATIC  void        c3d_spline_header1 __(( C3_CURVE )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_spline ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
{
    RETURN ( c3d_spline_knots ( a, n, C3_DEFAULT ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_spline_knots ( a, n, knot_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
{
    RETURN ( c3d_spline_tan ( a, n, knot_options, 
        NULL, C3_NDEF_TAN, NULL, C3_NDEF_TAN ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_spline_tan ( a, n, knot_options, 
                tan0, tan0_options, tan1, tan1_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
PT3 tan0    ;
C3_TAN_OPTIONS tan0_options ;
PT3 tan1    ;
C3_TAN_OPTIONS tan1_options ;
{
    C3_CURVE curve ;

    curve = c3d_curve() ;
    if ( curve != NULL ) {
        if ( !c3d_spline_header0 ( curve, n+2, 4 ) ) {
            c3d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        if ( !c3n_interp_tan ( a, n, knot_options, tan0, tan0_options, 
            tan1, tan1_options, C3_CURVE_NURB(curve) ) ) {
            c3d_free_curve ( curve ) ;
            curve = NULL ;
        }
        else
            c3d_spline_header1 ( curve ) ;
    }   
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_spline_clsd ( a, n, knot_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
{
    C3_CURVE curve ;

    curve = c3d_curve() ;
    if ( curve != NULL ) {
        if ( !c3d_spline_header0 ( curve, n+3, 4 ) ) {
            c3d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        if ( !c3n_interp_clsd ( a, n, knot_options, C3_CURVE_NURB(curve) ) ) {
            c3d_free_curve ( curve ) ;
            curve = NULL ;
        }
        else
            c3d_spline_header1 ( curve ) ;
    }   
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN c3d_spline_header0 ( curve, n, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
INT n ;    
INT d ;
{
    C3_CURVE_NURB(curve) = c3n_create_nurb ( n, d ) ;
    if ( C3_CURVE_NURB(curve) == NULL ) 
        RETURN ( FALSE ) ;
    C3_CURVE_TYPE(curve) = C3_NURB_TYPE ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_nurb ( a, n, d, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a      ;   /* a[n] */
INT n, d ;
REAL *knot ;
{
    C3_CURVE curve = c3d_curve () ;

    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( !c3d_spline_header0 ( curve, n, d ) ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c3n_set_cthpts ( a, C3_CURVE_NURB(curve) ) ;
    c3n_set_knots ( knot, C3_CURVE_NURB(curve) ) ;
    c3d_spline_header1 ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_nu_bspline ( a, n, d, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n, d ;
REAL *knot ;
{
    C3_CURVE curve = c3d_curve () ;

    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( !c3d_spline_header0 ( curve, n, d ) ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c3n_set_knots ( knot, C3_CURVE_NURB(curve) ) ;
    c3n_set_ctpts ( a, C3_CURVE_NURB(curve) ) ;
    c3d_spline_header1 ( curve ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
STATIC void c3d_spline_header1 ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    c3n_init_parms ( C3_CURVE_NURB(curve), C3_CURVE_PARM0(curve), 
        C3_CURVE_PARM1(curve) ) ;
    c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
}   


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_curve_to_spline ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    C3_CURVE spline ;
    C3_NURB nurb ;

    if ( C3_CURVE_IS_SPLINE(curve) || C3_CURVE_IS_ELLIPSE(curve) ||
        C3_CURVE_IS_BEZIER(curve) ) {

        if ( c3n_trimmed ( C3_CURVE_NURB(curve), 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ) {
            nurb = c3n_trim ( C3_CURVE_NURB(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
            spline = c3d_curve() ;
            if ( spline == NULL ) 
                RETURN ( NULL ) ;
            c3n_set_w ( nurb, c3n_get_w ( C3_CURVE_NURB(curve) ) ) ;
            c3n_set_normal ( nurb, c3n_get_normal ( C3_CURVE_NURB(curve) ) ) ;
            C3_CURVE_NURB(spline) = nurb ;
            C3_CURVE_TYPE(spline) = C3_NURB_TYPE ;
            C3_CURVE_T1(spline) = C3_CURVE_T1(curve) ;
            C3_CURVE_J1(spline) = C3_CURVE_J1(curve) - C3_CURVE_J0(curve) 
                + c3n_get_d ( C3_CURVE_NURB(curve) ) - 1 ;
            C3_CURVE_T0(spline) = C3_CURVE_T0(curve) ;
            C3_CURVE_J0(spline) = c3n_get_d ( C3_CURVE_NURB(curve) ) - 1 ;
            c3n_box ( C3_CURVE_NURB(spline), C3_CURVE_BOX(spline) ) ;
            RETURN ( spline ) ;
        }
        else
            RETURN ( c3d_copy ( curve ) ) ;
    }

    spline = c3d_curve() ;
    if ( spline == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_TYPE(spline) = C3_NURB_TYPE ;
    C3_CURVE_T0(spline) = C3_CURVE_T0(curve) ;
    C3_CURVE_T1(spline) = C3_CURVE_T1(curve) ;
    c3a_box_copy ( C3_CURVE_BOX(curve), C3_CURVE_BOX(spline) ) ;

    if (C3_CURVE_IS_LINE(curve) ) {
        C3_CURVE_NURB(spline) = c3n_line ( C3_CURVE_LINE(curve) ) ;
        C3_CURVE_J0(spline) = 1 ;
        C3_CURVE_J1(spline) = 2 ;
    }
    else if (C3_CURVE_IS_ARC(curve) ) {
        C3_CURVE_TYPE(spline) = C3_ELLIPSE_TYPE ;
        C3_CURVE_NURB(spline) = c3n_arc ( C3_CURVE_ARC(curve), 
            C3_CURVE_IS_EXT_ARC(curve) ) ;
        C3_CURVE_J0(spline) = C3_CURVE_J0(curve) ;
        C3_CURVE_J1(spline) = C3_CURVE_J1(curve) ;
    }
    else if (C3_CURVE_IS_PCURVE(curve) ) {
        C3_CURVE_NURB(spline) = c3n_pcurve ( C3_CURVE_PCURVE(curve) ) ;
        C3_CURVE_J0(spline) = C3_CURVE_J0(curve) + 2 ;
        C3_CURVE_J1(spline) = c3n_get_n ( C3_CURVE_NURB(spline) ) - 1 ;
    }

    RETURN ( C3_CURVE_NURB(spline) == NULL ? NULL : spline ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE c3d_spline_frame ( n, d, knot_options ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n, d ;
C3_KNOT_OPTIONS knot_options ;
{
    C3_CURVE curve ;

    curve = c3d_curve() ;
    if ( curve != NULL ) {
        if ( !c3d_spline_header0 ( curve, n, d ) ) {
            c3d_free_curve ( curve ) ;
            RETURN ( NULL ) ;
        }
        if ( knot_options == C3_UNIFORM ) 
            c3n_set_uniform_knots ( C3_CURVE_NURB(curve) ) ;
        else if ( knot_options == C3_CLSC_UNI ) 
            c3n_set_clsc_uni_knots ( C3_CURVE_NURB(curve) ) ;
        c3n_init_parms ( C3_CURVE_NURB(curve), C3_CURVE_PARM0(curve), 
            C3_CURVE_PARM1(curve) ) ;
    }   
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE c3d_spline_approx ( curve, parm0, parm1, m ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm0, parm1 ;
INT m ;
{
    PT3 *x, tan0, tan1 ;
    REAL scale ;
    C3_CURVE spline ;

    if ( m < 4 ) 
        RETURN ( NULL ) ;

    x = CREATE ( m-2, PT3 ) ;
    if ( !c3c_eval_equi_parm ( curve, parm0, parm1, m-2, x, tan0, tan1 ) ) {
        KILL ( x ) ;
        RETURN ( NULL ) ;
    }

    scale = ( ( parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1) ) -
        ( parm0 == NULL ? C3_CURVE_T0(curve) : PARM_T(parm0) ) ) / 
        (REAL)(m-3) ;
    C3V_SCALE ( tan0, scale, tan0 ) ;
    C3V_SCALE ( tan1, scale, tan1 ) ;
    spline = c3d_spline_tan ( x, m-2, C3_CLSC_UNI, tan0, C3_DEF_TAN, tan1, 
        C3_DEF_TAN ) ;
    KILL ( x ) ;
    RETURN ( spline ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_ellipse_to_nurb ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    C3_CURVE copy ;
    if ( !C3_CURVE_IS_ELLIPSE(curve) ) 
        RETURN ( NULL ) ;
    else {
        copy = c3d_copy ( curve ) ;
        if ( c3c_ellipse_to_nurb ( copy ) ) 
            RETURN ( copy ) ;
        else {
            c3d_free_curve ( copy ) ;
            RETURN ( NULL ) ;
        }
    }
}

#endif  /* SPLINE */

