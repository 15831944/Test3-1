//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CA.C *********************************/ 
/***************** Three-dimensional arcs *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alrdefs.h>
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3pdefs.h>
#include <c3ndefs.h>
#include <c3cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_arc_data ( curve, ctr, rad_ptr, normal, sweep_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 ctr, normal ;
REAL *rad_ptr, *sweep_ptr ;
{
    if ( ctr != NULL ) {
        if ( !c3c_get_arc_center ( curve, ctr ) )
            RETURN ( FALSE ) ;
    }
    if ( rad_ptr != NULL ) {
        if ( !c3c_get_arc_radius ( curve, rad_ptr ) )
            RETURN ( FALSE ) ;
    }
    if ( sweep_ptr != NULL ) {
        if ( !c3c_get_arc_sweep ( curve, sweep_ptr ) )
            RETURN ( FALSE ) ;
    }
    if ( normal != NULL ) {
        if ( !c3c_get_arc_normal ( curve, normal ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_arc_radius ( curve, rad_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
REAL *rad_ptr ;
{
    if ( !C3_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    *rad_ptr = c3g_rad ( C3_CURVE_ARC(curve) ) ;
    RETURN ( *rad_ptr > 0.0 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_arc_center ( curve, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 ctr ;
{
    if ( !C3_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    RETURN ( c3g_ctr ( C3_CURVE_ARC(curve), ctr ) != NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_arc_normal ( curve, normal ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 normal ;
{
    if ( !C3_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    RETURN ( c3g_normal ( C3_CURVE_ARC(curve), normal ) != NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_arc_sweep ( curve, sweep_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
REAL *sweep_ptr ;
{
    REAL d ;

    if ( !C3_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;

    d = c3g_d_parm ( C3_CURVE_ARC(curve) ) ;

    if ( IS_SMALL(C3_CURVE_T0(curve)) && IS_SMALL(C3_CURVE_T1(curve)-1.0) )
        *sweep_ptr = alr_sweep ( d ) ;
    else if ( IS_SMALL(C3_CURVE_T0(curve)) && IS_SMALL(C3_CURVE_T1(curve)-2.0) )
        *sweep_ptr = TWO_PI ;
    else if ( C3_CURVE_T1(curve) <= 1.0 )
        *sweep_ptr = alr_sweep ( alr_trim_d ( d, 
            C3_CURVE_T0(curve), C3_CURVE_T1(curve) ) ) ;
    else if ( C3_CURVE_T0(curve) >= 1.0 )
        *sweep_ptr = alr_sweep ( alr_trim_d ( d, 
            C3_CURVE_T0(curve)-1.0, C3_CURVE_T1(curve)-1.0 ) ) ;
    else 
        *sweep_ptr = 
            alr_sweep ( alr_trim_d ( d, C3_CURVE_T0(curve), 1.0 ) ) + 
            alr_sweep ( alr_trim_d ( d, 0.0, C3_CURVE_T1(curve)-1.0 ) ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_get_arc_pts_d ( curve, pt0, pt1, d_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 pt0, pt1, d_vec ;
{
    if ( C3_CURVE_IS_LINE(curve) ) 
        RETURN ( c3l_get_pts_d ( C3_CURVE_LINE(curve), 
            C3_CURVE_T0(curve), C3_CURVE_T1(curve), pt0, pt1, d_vec ) ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        RETURN ( c3g_get_pts_d ( C3_CURVE_ARC(curve), 
            C3_CURVE_T0(curve), C3_CURVE_T1(curve), pt0, pt1, d_vec ) ) ;
    else 
        RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_arc_angle ( curve, parm, angle_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm ;
REAL *angle_ptr ;
{
    if ( !C3_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        *angle_ptr = c3g_t_to_angle ( C3_CURVE_ARC(curve), PARM_T(parm) ) ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_arc_parm ( curve, angle, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
REAL angle ;
PARM parm ;
{
    if ( !C3_CURVE_IS_ARC(curve) ) 
        RETURN ( FALSE ) ;
    else {
        PARM_T(parm) = c3g_angle_to_t ( C3_CURVE_ARC(curve), angle ) ;
        alr_parm_set ( parm ) ;
        RETURN ( TRUE ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_pcurve_data ( curve, i, is_arc, ctr, ept1, 
            rad_ptr, normal, sweep_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
INT i ;
BOOLEAN *is_arc ;
PT3 ctr, ept1, normal ;
REAL *rad_ptr, *sweep_ptr ;
{
    INT n ;

    if ( !C3_CURVE_IS_PCURVE(curve) )
        RETURN ( FALSE ) ;
    n = c3p_n ( C3_CURVE_PCURVE(curve) ) ;
    if ( i < 0 || i >= n - 1 )
        RETURN ( FALSE ) ;
    if ( is_arc != NULL )
        *is_arc = c3p_get_data ( C3_CURVE_PCURVE(curve), i, 
            ctr, ept1, rad_ptr, normal, sweep_ptr ) ;
    else
        (void) c3p_get_data ( C3_CURVE_PCURVE(curve), i, 
            ctr, ept1, rad_ptr, normal, sweep_ptr ) ;
    RETURN ( TRUE ) ;
}


#ifdef SPLINE
/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_get_ellipse_data ( curve, ctr, major_axis, minor_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 ctr ;
REAL *major_axis, *minor_axis ;
{
    RETURN ( C3_CURVE_IS_ELLIPSE(curve) && 
        c3n_get_ellipse_data ( C3_CURVE_NURB(curve), 
            ctr, major_axis, minor_axis ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_ellipse_to_nurb ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    if ( !C3_CURVE_IS_ELLIPSE(curve) ) 
        RETURN ( FALSE ) ;
    else {
        C3_CURVE_TYPE(curve) = C3_NURB_TYPE ;
        RETURN ( TRUE ) ;
    }
}
#endif /*SPLINE*/


