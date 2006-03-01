//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3BE.C *********************************/
/**************** Three-dimensional Bezier curves evaluations **************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <albdefs.h>
#include <c3bdefs.h>
#include <c3edefs.h>
#include <c3hmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_eval ( b, d, w, normal, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;  /* b[d] array of control points */
INT d ;     /* order */
REAL w ;
PT3 normal ;
REAL t ;    /* evaluation parameter */
INT p ;     /* order of the highest derivative */
PT3 *x  ;   /* output array of derivatives */
{
    HPT3 *a ;
    PT3 *a1 ;
    BOOLEAN status ;

    if ( IS_SMALL(w) ) {
        a = CREATE ( p+1, HPT3 ) ;
        if ( a==NULL ) 
            RETURN ( FALSE ) ;
        alb_eval ( (REAL*)b, d, 4, t, p, (REAL*)a ) ;
        status = c3e_ratio ( a, p, x ) ;
        KILL ( a ) ;
    }
    else {
        a = CREATE ( p+2, HPT3 ) ;
        a1 = CREATE ( p+2, PT3 ) ;
        alb_eval ( (REAL*)b, d, 4, t, p+1, (REAL*)a ) ;
        status = c3e_ratio ( a, p+1, a1 ) && 
            c3e_offset ( a1, w, normal, p, x ) ;
        KILL ( a ) ;
        KILL ( a1 ) ;
    }   
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_eval_pt ( b, d, w, normal, t, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;  /* b[d] array of control points */
INT d ;     /* order */
REAL w ;    /* offset distance */
PT3 normal ;
REAL t ;    /* evaluation parameter */
PT3 x ;   /* output array of derivatives */
{
    RETURN ( c3b_eval ( b, d, w, normal, t, 0, (PT3*)x ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_ept0 ( b, d, w, normal, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 pt ;
{
    if ( HPT3_INF(b[0]) ) 
        RETURN ( FALSE ) ;
    if ( IS_ZERO(w) ) {
        C3H_HPT_PT ( b[0], pt ) ;
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( c3b_eval_pt ( b, d, w, normal, 0.0, pt ) ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_etan0 ( b, d, w, normal, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 tan_vec ;
{
    INT i ;

    if ( IS_SMALL(w) ) {
        for ( i=1 ; i<d ; i++ ) { 
            C3H_DIFF ( b[i], b[0], tan_vec ) ; 
            if ( !C3V_IS_SMALL(tan_vec) ) 
                RETURN ( TRUE ) ; 
        } 
    }
    else {
        PT3 x[2] ;
        if ( c3b_eval ( b, d, w, normal, 0.0, 1, x ) ) {
            C3V_COPY ( x[1], tan_vec ) ;
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_enormal0 ( b, d, w, normal, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 n ;
{
    PT3 x[2] ;
    RETURN ( c3b_eval ( b, d, w, normal, 0.0, 1, x ) && 
        c3e_normal ( x, normal, 0, (PT3*)n ) ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_ept_tan0 ( b, d, w, normal, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT  d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 pt ;
PT3 tan_vec ;
{
    if ( IS_SMALL(w) ) 
        RETURN ( c3b_ept0 ( b, d, w, normal, pt ) && 
            c3b_etan0 ( b, d, w, normal, tan_vec ) ) ;
    else {
        PT3 x[2] ;
        if ( c3b_eval ( b, d, w, normal, 0.0, 1, x ) ) {
            if ( pt != NULL ) {
                C3V_COPY ( x[0], pt ) ;
            }
            if ( tan_vec != NULL ) {
                C3V_COPY ( x[1], tan_vec ) ;
            }
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_ept1 ( b, d, w, normal, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 pt ;
{
    if ( HPT3_INF(b[d-1]) ) 
        RETURN ( FALSE ) ;
    if ( IS_ZERO(w) ) {
        C3H_HPT_PT ( b[d-1], pt ) ;
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( c3b_eval_pt ( b, d, w, normal, 1.0, pt ) ) ;
}   


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_etan1 ( b, d, w, normal, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 tan_vec ;
{
    INT i ;

    if ( IS_SMALL(w) ) {
        for ( i=d-2 ; i>=0 ; i-- ) { 
            C3H_DIFF ( b[d-1], b[i], tan_vec ) ; 
            if ( !C3V_IS_SMALL(tan_vec) ) 
                RETURN ( TRUE ) ; 
        } 
    }
    else {
        PT3 x[2] ;
        if ( c3b_eval ( b, d, w, normal, 1.0, 1, x ) ) {
            C3V_COPY ( x[1], tan_vec ) ;
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_enormal1 ( b, d, w, normal, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 n ;
{
    PT3 x[2] ;
    RETURN ( c3b_eval ( b, d, w, normal, 1.0, 1, x ) && 
        c3e_normal ( x, normal, 0, (PT3*)n ) ) ;
} 


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN     c3b_ept_tan1 ( b, d, w, normal, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b  ;      /* Control points */
INT d ;         /* Order */
REAL w ;
PT3 normal ;
PT3 pt ;
PT3 tan_vec ;
{
    if ( IS_SMALL(w) ) 
        RETURN ( c3b_ept1 ( b, d, w, normal, pt ) && 
            c3b_etan1 ( b, d, w, normal, tan_vec ) ) ;
    else {
        PT3 x[2] ;
        if ( c3b_eval ( b, d, w, normal, 1.0, 1, x ) ) {
            if ( pt != NULL ) {
                C3V_COPY ( x[0], pt ) ;
            }
            if ( tan_vec != NULL ) {
                C3V_COPY ( x[1], tan_vec ) ;
            }
            RETURN ( TRUE ) ;
        }
    }
    RETURN ( FALSE ) ;
}
#endif  /*SPLINE*/


