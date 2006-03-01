//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/*********************************** C3B.C *********************************/
/********************** Three-dimensional Bezier curves ********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3apriv.h>
#include <c3bdefs.h>
#include <c3vdefs.h>
#include <fnbdefs.h>
#include <c3vmcrs.h>
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_project ( b, d, w, normal, pt, t_ptr, ppt, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;
INT     d ;
REAL    w ;
PT3     normal ;
PT3     pt ;
REAL    *t_ptr, *dist_ptr ;
PT3     ppt ;
{
    HPT3 *a  ;
    PT3 x[3]  ;
    HREAL *c ;
    REAL z, dist0, dist=0.0 ;
    BOOLEAN status ;
    DML_LIST xlist ;
    DML_ITEM item ;
    C3_EXTR_REC xr ;

    a = CREATE ( d, HPT3 ) ;
    c3a_sub ( b, d, pt, a ) ;
    c = CREATE ( 2*d-1, HREAL ) ;
    c3b_dot ( a, d, a, d, c ) ;
    
    if ( IS_ZERO(w) ) {
        status = fnb_extr ( c, 2*d-1, FALSE /*min*/, t_ptr, &dist0 ) ;
        if ( status ) {
            if ( IS_ZERO(*t_ptr) && 
                c3b_ept_tan0 ( b, d, w, normal, x[0], x[1] ) ) {
                C3V_SUB ( x[0], pt, x[0] ) ;
                z = C3V_DOT ( x[0], x[1] ) ;
                status = ( fabs(z) <= BBS_ZERO * C3V_NORML1 ( x[1] ) ) ;
            }
            else if ( IS_ZERO(*t_ptr-1.0) && 
                c3b_ept_tan1 ( b, d, w, normal, x[0], x[1] ) ) {
                C3V_SUB ( x[0], pt, x[0] ) ;
                z = C3V_DOT ( x[0], x[1] ) ;
                status = ( fabs(z) <= BBS_ZERO * C3V_NORML1 ( x[1] ) ) ;
            }
        }
    }

    else {  /* Non-zero offset */

        xlist = dml_create_list () ;
        fnb_extrs ( c, 2*d-1, 0.0, 1.0, 0, xlist ) ;
        status = FALSE ;
        DML_WALK_LIST ( xlist, item ) {
            xr = (C3_EXTR_REC)DML_RECORD(item) ;
            c3b_eval ( b, d, w, normal, C3_EXTR_REC_T(xr), 2, (PT3*)x ) ;
            C3V_SUB ( x[0], pt, x[0] ) ;
            z = C3V_DOT ( x[0], x[2] ) + C3V_DOT ( x[1], x[1] ) ;
            if ( z > 0.0 ) {
                dist0 = C3V_DOT ( x[0], x[0] ) ;
                if ( !status || ( dist0 < dist ) ) {
                    dist = dist0 ;
                    status = TRUE ;
                    *t_ptr = C3_EXTR_REC_T(xr) ;
                }
            }
        }
        dml_destroy_list ( xlist, dmm_free ) ;
    }
    if ( status ) {
        if ( dist_ptr != NULL )
            *dist_ptr = ( dist0 > 0.0 ? sqrt ( dist0 ) : 0.0 ) ;
        if ( ppt != NULL ) { /* evaluate the point */
            status = c3b_eval_pt ( b, d, w, normal, *t_ptr, ppt ) ;
        }
    }
    KILL ( a ) ;
    KILL ( c ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_project_init ( b, d, w, normal, pt, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;
INT     d ;
REAL    w ;
PT3     normal ;
PT3     pt ;
REAL    *t_ptr ;
{
    PT3 pt0, pt1 ;
    REAL u, a0, a1 ;

    if ( !c3b_ept0 ( b, d, w, normal, pt0 ) )
        RETURN ( FALSE ) ;
    if ( !c3b_ept1 ( b, d, w, normal, pt1 ) )
        RETURN ( FALSE ) ;
    u = c3v_project_line ( pt0, pt1, pt, NULL ) ;
    a0 = b[0][3] * ( 1.0 - u ) ;
    a1 = b[d-1][3] * u ;
    *t_ptr = a1 / ( a0 + a1 ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_project_iter ( b, d, w, normal, pt, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b  ;
INT     d ;
REAL    w ;
PT3     normal ;
PT3     pt ;
REAL    *t_ptr ;
{
    INT count ;
    REAL delta, r, r0, r1 ;
    PT3 v, x[3]  ;

    if ( !c3b_project_init ( b, d, w, normal, pt, t_ptr ) )
        RETURN ( FALSE ) ;
    for ( count=0 ; count<10 ; count++ ) {
        if ( !c3b_eval ( b, d, w, normal, *t_ptr, 2, x ) )
            RETURN ( FALSE ) ;
        C3V_SUB ( x[0], pt, v ) ;
        r0 = C3V_DOT ( x[1], x[1] ) ;
        r1 = C3V_DOT ( v, x[2] ) ;
        r = r0 + r1 ;
        if ( IS_SMALL(r) ) {
            r = r0 ;
            if ( IS_SMALL(r) ) 
                RETURN ( FALSE ) ;
        }
        delta = - C3V_DOT ( v, x[1] ) / r ;
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
#endif  /*SPLINE*/


