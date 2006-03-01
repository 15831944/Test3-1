//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CS.C *********************************/
/*********** Three-dimensional curves - select **************************/
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
#include <c3vmcrs.h>
STATIC BOOLEAN c3c_pts_on_one_side __(( PT3, PT3, PT3, PT3 )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_select ( curve, pt, view_plane_normal, tol, 
            sel_parm, dist_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 pt, view_plane_normal ;
REAL tol ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    PARM_S temp_parm ;
    if ( sel_parm == NULL ) 
        sel_parm = &temp_parm ;

    if ( C3_CURVE_IS_LINE(curve) ) {
        PARM_J(sel_parm) = 1 ;
        RETURN ( c3l_select ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
                C3_CURVE_T1(curve), pt, view_plane_normal, tol, 
                &(PARM_T(sel_parm)), dist_ptr ) ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        if ( c3g_select ( C3_CURVE_ARC(curve), C3_CURVE_T0(curve), 
                C3_CURVE_T1(curve), pt, view_plane_normal, tol, 
                &(PARM_T(sel_parm)), dist_ptr ) ) {
            alr_parm_set ( sel_parm ) ;
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }
    if ( C3_CURVE_IS_PCURVE(curve) ) {
        if ( sel_parm != NULL ) {
            if ( c3p_select ( C3_CURVE_PCURVE(curve), 
                C3_CURVE_T0(curve), C3_CURVE_T1(curve), pt, view_plane_normal, 
                tol, &(PARM_T(sel_parm)), dist_ptr ) ) {
                alr_parm_set ( sel_parm ) ;
                RETURN ( TRUE ) ;
            }
            else 
                RETURN ( FALSE ) ;
        }
        else 
            RETURN ( c3p_select ( C3_CURVE_PCURVE(curve), C3_CURVE_T0(curve), 
                C3_CURVE_T1(curve), pt, view_plane_normal, tol, 
                NULL, dist_ptr ) ) ;
    }
    else 
#ifdef  SPLINE
        RETURN ( c3n_select ( C3_CURVE_NURB(curve), 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve), 
            pt, view_plane_normal, tol, sel_parm, dist_ptr ) ) ;
#else  /*SPLINE*/
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_select_face ( p0, p1, p2, p3, a, dirvec, cnvx, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 p0, p1, p2, p3, a, dirvec ;
BOOLEAN cnvx ;
REAL* dist ;
{
    BOOLEAN status ;
    if ( dist != NULL ) 
        *dist = -1.0 ;

    if ( p3 == NULL || C3V_IDENT_PTS ( p2, p3 ) )
        status = c3c_select_triangle ( p0, p1, p2, a, dirvec, dist ) ;
    else if ( cnvx )
        status = c3c_select_triangle ( p0, p1, p2, a, dirvec, dist ) ||
                 c3c_select_triangle ( p1, p2, p3, a, dirvec, dist ) ;
    else if ( c3c_pts_on_one_side ( p0, p1, p2, p3 ) ) 
        status = c3c_select_triangle ( p0, p1, p2, a, dirvec, dist ) ||
                 c3c_select_triangle ( p1, p2, p3, a, dirvec, dist ) ;
    else if ( c3c_pts_on_one_side ( p1, p2, p3, p0 ) ) 
        status = c3c_select_triangle ( p1, p2, p3, a, dirvec, dist ) ||
                 c3c_select_triangle ( p2, p3, p0, a, dirvec, dist ) ;
    else if ( c3c_pts_on_one_side ( p2, p3, p0, p1 ) ) 
        status = c3c_select_triangle ( p2, p3, p0, a, dirvec, dist ) ||
                 c3c_select_triangle ( p3, p0, p1, a, dirvec, dist ) ;
    else if ( c3c_pts_on_one_side ( p3, p0, p1, p2 ) ) 
        status = c3c_select_triangle ( p3, p0, p1, a, dirvec, dist ) ||
                 c3c_select_triangle ( p0, p1, p2, a, dirvec, dist ) ;
    else
        status = FALSE ;
    if ( !status && dist != NULL ) 
        *dist = 0.0 ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
STATIC BOOLEAN c3c_pts_on_one_side ( p0, p1, p2, p3 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 p0, p1, p2, p3 ;
{
    PT3 x, y, z, vec ;
    REAL v2, v3 ;

    C3V_SUB ( p1, p0, x ) ;
    C3V_SUB ( p3, p2, y ) ;
    C3V_CROSS ( x, y, z ) ;
    C3V_CROSS ( z, x, y ) ;
    C3V_SUB ( p2, p0, vec ) ;
    v2 = C3V_DOT ( vec, y ) ;
    C3V_SUB ( p3, p0, vec ) ;
    v3 = C3V_DOT ( vec, y ) ;
    RETURN ( ( v2 > 0.0 ) == ( v3 > 0.0 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_select_triangle ( p0, p1, p2, a, dirvec, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 p0, p1, p2, a, dirvec ;
REAL* dist ;
{
    PT3 v0, v1, v2 ;
    REAL t, t0, u1, u2 ;


    C3V_SUB ( p1, p0, v1 ) ;
    C3V_SUB ( p2, p0, v2 ) ;
    C3V_SUB ( a, p0, v0 ) ;

    t = C3V_TRIPLE_PROD ( v1, v2, dirvec ) ;
    if ( IS_SMALL ( t ) ) 
        RETURN ( FALSE ) ;
    u1 = C3V_TRIPLE_PROD ( v0, v2, dirvec ) / t ;
    if ( u1 <= - BBS_ZERO || u1 >= 1.0 + BBS_ZERO )
        RETURN ( FALSE ) ;
    u2 = C3V_TRIPLE_PROD ( v1, v0, dirvec ) / t ;
    if ( u2 <= - BBS_ZERO || u2 >= 1.0 + BBS_ZERO )
        RETURN ( FALSE ) ;
    if ( u1 + u2 >= 1.0 + BBS_ZERO )
        RETURN ( FALSE ) ;
    t0 = - C3V_TRIPLE_PROD ( v1, v2, v0 ) / t ;
    if ( t0 < 0.0 ) 
        RETURN ( FALSE ) ;
    if ( dist != NULL ) {
        if ( IS_ZERO ( *dist + 1.0 ) ) 
            *dist = C3V_NORM ( dirvec ) * t0 ;
        else {
            t = C3V_NORM ( dirvec ) * t0 ;
            if ( t < *dist )
                *dist = t ;
        }
    }
    RETURN ( TRUE ) ;
}
