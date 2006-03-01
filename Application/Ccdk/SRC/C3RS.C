//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3RS.C *********************************/
/******************** Routines for selecting arcs **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <alpdefs.h>
#include <c3ldefs.h>
#include <c3rdefs.h>
#include <c3vdefs.h>
#include <c3lmcrs.h>
#include <c3rmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_select ( arc, t0, t1, pt, normal, tol, 
            t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
REAL t0, t1 ;
PT3 pt, normal ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    REAL a, b, r, u, dot, z0, z1, z2, t[2] ;
    PT3 ctr, arc_normal, pt_vec, p, b0, b1 ;
    INT i, n, mult[2] ;

    if ( t_ptr == NULL ) 
        t_ptr = t ;

    if ( c3r_normal ( arc, arc_normal ) == NULL ) {
        C3_LINE_S line ;
        C3V_COPY ( C3_ASEG_PT0(arc), C3_LINE_PT0(&line) ) ;
        C3V_COPY ( C3_ASEG_PT1(arc), C3_LINE_PT1(&line) ) ;
        RETURN ( c3l_select ( &line, t0, t1, pt, normal, tol, 
            t_ptr, dist_ptr ) ) ;
    }

    r = c3r_ctr_rad ( arc, ctr ) ;
    C3V_SUB ( ctr, pt, pt_vec ) ;
    u = C3V_DOT ( pt_vec, arc_normal ) ;
    dot = C3V_DOT ( normal, arc_normal ) ;
    t0 -= ( tol/r ) ;
    t1 += ( tol/r ) ;
    if ( fabs(dot) >= BBS_ZERO * C3V_NORML1(normal) ) {
        u /= dot ;
        C3V_ADDT ( pt, normal, u, p ) ;
        if ( fabs ( C3V_DIST ( p, ctr ) - r ) > tol ) 
            RETURN ( FALSE ) ;
        if ( !c3r_project ( arc, p, t_ptr, NULL ) )
            RETURN ( FALSE ) ;
        if ( *t_ptr >= t0 && *t_ptr <= t1 ) {
            if ( dist_ptr != NULL ) 
                *dist_ptr = C3V_DIST ( p, ctr ) - r ;
            RETURN ( TRUE ) ;
        }
        else
            RETURN ( FALSE ) ;
    }

    if ( fabs ( C3V_DOT ( pt_vec, arc_normal ) ) >= 
        BBS_ZERO * C3V_NORML1 ( pt_vec ) )
            RETURN ( FALSE ) ;

    C3V_CROSS ( normal, arc_normal, p ) ;
    C3V_SUB ( C3_ASEG_PT0(arc), pt, b0 ) ;
    C3V_SUB ( C3_ASEG_PT1(arc), pt, b1 ) ;
    c3r_ab ( arc, &a, &b ) ;
    z0 = C3V_DOT ( b0, p ) ;
    z2 = C3V_DOT ( b1, p ) ;
    z1 = a * ( z0 + z2 ) + b * C3V_DIST ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) 
        * C3V_DOT ( C3_ASEG_D_VEC(arc), p ) ;
    n = alp_solve_bez3 ( z0, 0.5*z1, z2, t, mult ) ;
    for ( i=0 ; i<n ; i++ ) {
        if ( t[i] >= t0 && t[i] <= t1 ) {
            if ( t_ptr != NULL ) 
                *t_ptr = t[i] ;
            if ( dist_ptr != NULL ) 
                *dist_ptr = 0.0 ;
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}

