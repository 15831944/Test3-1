//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/*********************************** C2BS.C ********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3bdefs.h>
#include <fnbdefs.h>
#include <c3hmcrs.h>
#include <c3vmcrs.h>

STATIC BOOLEAN c3b_select_w __(( HPT3*, INT, REAL, PT3, PT3, PT3, REAL, 
            REAL, REAL, REAL*, REAL* )) ;
STATIC BOOLEAN c3b_select_init __(( HPT3*, INT, REAL, PT3, 
            PT3, PT3, REAL* )) ; 
STATIC BOOLEAN c3b_select_iter __(( HPT3*, INT, REAL, PT3, 
            PT3, PT3, REAL* )) ; 

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_select ( b, d, w, normal, pt, view_plane_normal, tol, 
            t0, t1, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset */
PT3     normal, pt, view_plane_normal ;
REAL    tol ;
REAL    t0, t1 ;
REAL    *t_ptr ;
REAL    *dist_ptr ;
{
    HPT3 *a ;
    HREAL *c ;
    BOOLEAN status ;
    INT i ;
    PT3 x ;

    if ( !IS_SMALL(w) ) 
        RETURN ( c3b_select_w ( b, d, w, normal, pt, view_plane_normal, tol, 
            t0, t1, t_ptr, dist_ptr ) ) ;

    a = CREATE ( d, HPT3 ) ;
    c = CREATE ( 2*d-1, HREAL ) ;
    if ( a==NULL || c==NULL ) 
        RETURN ( FALSE ) ;

    for ( i=0 ; i<d ; i++ ) {
        C3H_SUB ( b[i], pt, x ) ;
        C3V_CROSS ( x, view_plane_normal, a[i] ) ;
        a[i][3] = b[i][3] ;
    }
    c3b_dot ( a, d, a, d, c ) ;
    status = fnb_select ( c, 2*d-1, 
        tol*tol * C3V_NORM_SQUARED(view_plane_normal), 
        t0, t1, t_ptr, dist_ptr ) ;
    if ( dist_ptr != NULL ) 
        *dist_ptr = *dist_ptr > 0.0 ? sqrt ( *dist_ptr ) : 0.0 ;
    KILL ( a ) ;
    KILL ( c ) ;
    RETURN ( status ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_select_w ( b, d, w, normal, pt, view_plane_normal, tol, 
        t0, t1, t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset */
PT3     normal, pt, view_plane_normal ;
REAL    tol ;
REAL    t0, t1 ;
REAL    *t_ptr ;
REAL    *dist_ptr ;
{
    REAL t, dist ;
    PT3 p, cross ;

    if ( c3b_select_iter ( b, d, w, normal, pt, view_plane_normal, &t ) && 
        t >= - BBS_ZERO && t <= 1.0 + BBS_ZERO ) {
        c3b_eval_pt ( b, d, w, normal, t, p ) ;
        C3V_SUB ( p, pt, p ) ;
        C3V_CROSS ( p, view_plane_normal, cross ) ;
        dist = C3V_NORM(cross) ;
        if ( dist <= tol ) {
            if ( t_ptr != NULL ) 
                *t_ptr = t0 * ( 1.0 - t ) + t1 * t ;
            if ( dist_ptr != NULL ) 
                *dist_ptr = dist ;
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_select_init ( b, d, w, normal, pt, 
            view_plane_normal, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;
INT     d ;
REAL    w ;
PT3     normal, pt, view_plane_normal ;
REAL    *t_ptr ;
{
    PT3 p, tan, c0, c1 ;
    REAL u, f0, f1, a0, a1 ;

    if ( !c3b_ept_tan0 ( b, d, w, normal, p, tan ) )
        RETURN ( FALSE ) ;
    C3V_SUB ( p, pt, p ) ;
    C3V_CROSS ( p, view_plane_normal, c0 ) ;
    C3V_CROSS ( tan, view_plane_normal, c1 ) ;
    f0 = C3V_DOT ( c0, c1 ) ;

    if ( !c3b_ept_tan1 ( b, d, w, normal, p, tan ) )
        RETURN ( FALSE ) ;
    C3V_SUB ( p, pt, p ) ;
    C3V_CROSS ( p, view_plane_normal, c0 ) ;
    C3V_CROSS ( tan, view_plane_normal, c1 ) ;
    f1 = C3V_DOT ( c0, c1 ) ;
    if ( IS_SMALL(f0-f1) ) 
        u = 0.5 ;
    else
        u = f0 / ( f0 - f1 ) ;
    if ( u < 0.0 ) 
        u = 0.0 ;
    else if ( u > 1.0 ) 
        u = 1.0 ;
    a0 = b[0][3] * ( 1.0 - u ) ;
    a1 = b[d-1][3] * u ;
    *t_ptr = a1 / ( a0 + a1 ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_select_iter ( b, d, w, normal, pt, 
            view_plane_normal, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;
INT     d ;
REAL    w ;
PT3     normal, pt, view_plane_normal ;
REAL    *t_ptr ;
{
    INT count ;
    REAL delta, r, r0, r1 ;
    PT3 c0, c1, c2, v, x[3]  ;

    if ( !c3b_select_init ( b, d, w, normal, pt, view_plane_normal, t_ptr ) )
        RETURN ( FALSE ) ;

    for ( count=0 ; count<10 ; count++ ) {
        if ( !c3b_eval ( b, d, w, normal, *t_ptr, 2, x ) )
            RETURN ( FALSE ) ;
        C3V_SUB ( x[0], pt, v ) ;
        C3V_CROSS ( v, view_plane_normal, c0 ) ;
        C3V_CROSS ( x[1], view_plane_normal, c1 ) ;
        C3V_CROSS ( x[2], view_plane_normal, c2 ) ;
        r0 = C3V_DOT ( c1, c1 ) ;
        r1 = C3V_DOT ( v, c2 ) ;
        r = r0 + r1 ;
        if ( IS_SMALL(r) ) {
            r = r0 ;
            if ( IS_SMALL(r) ) 
                RETURN ( FALSE ) ;
        }
        delta = - C3V_DOT ( c0, c1 ) / r ;
        *t_ptr += delta ;
        if ( *t_ptr > 1.0 ) {
            if ( count>0 && ( *t_ptr > 1.2 ) ) 
                RETURN ( FALSE ) ;
            *t_ptr = 1.0 ;
        }
        if ( *t_ptr < 0.0 ) {
            if ( count>0 && ( *t_ptr < -0.2 ) ) 
                RETURN ( FALSE ) ;
            *t_ptr = 0.0 ;
        }
        if ( IS_ZERO(delta) ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}

#endif /*SPLINE*/


