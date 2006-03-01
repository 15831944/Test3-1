//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/************************************* C3AB.C ******************************/
/***************** Arrays of two-dimensional homogeneous points ************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                     All rights reserved                  !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3adefs.h>
#include <c3apriv.h>
#include <c3vdefs.h>
#include <c3vmcrs.h>

STATIC  BOOLEAN c3ab_box_overlap __(( C3_BOX, C3_BOX, REAL )) ;
/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box ( min_pt, max_pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 min_pt, max_pt ;
{
    C3_BOX box ;

    box = MALLOC ( 1, C3_BOX_S ) ;
    if ( box == NULL ) 
        RETURN ( NULL ) ;
    if ( min_pt != NULL )
        C3V_COPY ( min_pt, C3_MIN_PT(box) ) ;
    if ( max_pt != NULL )
        C3V_COPY ( max_pt, C3_MAX_PT(box) ) ;
    RETURN ( box ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void c3a_free_box ( box ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
{
    dmm_free ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3a_box_overlap ( box1, box2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_BOX box1, box2 ;
{
    RETURN ( c3ab_box_overlap ( box1, box2, BBS_TOL ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL c3a_box_size ( box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
{
    RETURN ( box == NULL ? 0.0 : 
        C3V_DISTL1 ( C3_MIN_PT(box), C3_MAX_PT(box) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3ab_box_overlap ( box1, box2, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_BOX box1, box2 ;
REAL tol ;
{
    if ( C3_MAX_X(box1) < C3_MIN_X(box2) - tol ) 
        RETURN ( FALSE ) ;
    if ( C3_MAX_X(box2) < C3_MIN_X(box1) - tol ) 
        RETURN ( FALSE ) ;
    if ( C3_MAX_Y(box1) < C3_MIN_Y(box2) - tol ) 
        RETURN ( FALSE ) ;
    if ( C3_MAX_Y(box2) < C3_MIN_Y(box1) - tol ) 
        RETURN ( FALSE ) ;
    if ( C3_MAX_Z(box1) < C3_MIN_Z(box2) - tol ) 
        RETURN ( FALSE ) ;
    if ( C3_MAX_Z(box2) < C3_MIN_Z(box1) - tol ) 
        RETURN ( FALSE ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX  c3a_box_poly ( a, d, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a  ;          /* Input array */
INT  d ;            /* Number of points */
C3_BOX  box ;
{
    INT i ;

    C3_MIN_X(box) = C3_MAX_X(box) = a[0][0] ;
    C3_MIN_Y(box) = C3_MAX_Y(box) = a[0][1] ;
    C3_MIN_Z(box) = C3_MAX_Z(box) = a[0][2] ;
    
    for ( i=1 ; i<d ; i++ ) {
        if ( a[i][0] < C3_MIN_X(box) )
            C3_MIN_X(box) = a[i][0] ;
        else if ( a[i][0] > C3_MAX_X(box) )
            C3_MAX_X(box) = a[i][0] ;
        if ( a[i][1] < C3_MIN_Y(box) )
            C3_MIN_Y(box) = a[i][1] ;
        else if ( a[i][1] > C3_MAX_Y(box) )
            C3_MAX_Y(box) = a[i][1] ;
        if ( a[i][2] < C3_MIN_Z(box) )
            C3_MIN_Z(box) = a[i][2] ;
        else if ( a[i][2] > C3_MAX_Z(box) )
            C3_MAX_Z(box) = a[i][2] ;
    }
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3a_pt_inside ( box, pt, tol ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
C3_BOX box ;
REAL tol ;
{
    RETURN ( 
    ( C3_MIN_X(box) - tol <= pt[0] ) && ( C3_MAX_X(box) + tol >= pt[0] ) &&
    ( C3_MIN_Y(box) - tol <= pt[1] ) && ( C3_MAX_Y(box) + tol >= pt[1] ) &&
    ( C3_MIN_Z(box) - tol <= pt[2] ) && ( C3_MAX_Z(box) + tol >= pt[2] ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_init_pt ( box, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
PT3 pt ;
{
    C3_MIN_X(box) = C3_MAX_X(box) = PT3_X(pt) ;
    C3_MIN_Y(box) = C3_MAX_Y(box) = PT3_Y(pt) ;
    C3_MIN_Z(box) = C3_MAX_Z(box) = PT3_Z(pt) ;
    RETURN ( box ) ;
}                                       


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_append_pt ( box, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
PT3 pt ;
{
    if ( PT3_X(pt) < C3_MIN_X(box) ) 
        C3_MIN_X(box) = PT3_X(pt) ;
    else if ( PT3_X(pt) > C3_MAX_X(box) ) 
        C3_MAX_X(box) = PT3_X(pt) ;
    if ( PT3_Y(pt) < C3_MIN_Y(box) ) 
        C3_MIN_Y(box) = PT3_Y(pt) ;
    else if ( PT3_Y(pt) > C3_MAX_Y(box) ) 
        C3_MAX_Y(box) = PT3_Y(pt) ;
    if ( PT3_Z(pt) < C3_MIN_Z(box) ) 
        C3_MIN_Z(box) = PT3_Z(pt) ;
    else if ( PT3_Z(pt) > C3_MAX_Z(box) ) 
        C3_MAX_Z(box) = PT3_Z(pt) ;
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_append ( box, append_box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box, append_box ;
{
    if ( C3_MIN_X(append_box) < C3_MIN_X(box) ) 
        C3_MIN_X(box) = C3_MIN_X(append_box) ;
    if ( C3_MAX_X(append_box) > C3_MAX_X(box) ) 
        C3_MAX_X(box) = C3_MAX_X(append_box) ;
    if ( C3_MIN_Y(append_box) < C3_MIN_Y(box) ) 
        C3_MIN_Y(box) = C3_MIN_Y(append_box) ;
    if ( C3_MAX_Y(append_box) > C3_MAX_Y(box) ) 
        C3_MAX_Y(box) = C3_MAX_Y(append_box) ;
    if ( C3_MIN_Z(append_box) < C3_MIN_Z(box) ) 
        C3_MIN_Z(box) = C3_MIN_Z(append_box) ;
    if ( C3_MAX_Z(append_box) > C3_MAX_Z(box) ) 
        C3_MAX_Z(box) = C3_MAX_Z(append_box) ;
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_copy ( box0, box1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box0, box1 ;
{
    C3_MIN_X(box1) = C3_MIN_X(box0) ;
    C3_MAX_X(box1) = C3_MAX_X(box0) ;
    C3_MIN_Y(box1) = C3_MIN_Y(box0) ;
    C3_MAX_Y(box1) = C3_MAX_Y(box0) ;
    C3_MIN_Z(box1) = C3_MIN_Z(box0) ;
    C3_MAX_Z(box1) = C3_MAX_Z(box0) ;
    RETURN ( box1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_parlm ( box, origin, u_vec, v_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
PT3 origin, u_vec, v_vec ;
{
    PT3 pt ;

    C3V_COPY ( origin, pt ) ;
    c3a_box_init_pt ( box, pt ) ;
    C3V_ADD ( pt, u_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    C3V_ADD ( origin, v_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    C3V_ADD ( pt, u_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_plus_minus ( box, ctr, u_vec, v_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
PT3 ctr, u_vec, v_vec ;
{
    PT3 pt, pt1, pt2 ;

    C3V_ADD ( ctr, u_vec, pt1 ) ;
    C3V_SUB ( ctr, u_vec, pt2 ) ;
    C3V_ADD ( pt1, v_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    C3V_SUB ( pt1, v_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    C3V_ADD ( pt2, v_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    C3V_SUB ( pt2, v_vec, pt ) ;
    c3a_box_append_pt ( box, pt ) ;
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_ctr ( box, ctr, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
PT3 ctr ;
REAL r ;
{
    C3_MIN_X(box) = ctr[0] - r ;
    C3_MIN_Y(box) = ctr[1] - r ;
    C3_MIN_Z(box) = ctr[2] - r ;
    C3_MAX_X(box) = ctr[0] + r ;
    C3_MAX_Y(box) = ctr[1] + r ;
    C3_MAX_Z(box) = ctr[2] + r ;
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_circle ( box, ctr, r, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
PT3 ctr, normal ;
REAL r ;
{
    PT3 a, b, x, y ;

    c3v_basis ( normal, x, y ) ;
    C3V_SCALE ( x, r, x ) ;
    C3V_SCALE ( y, r, y ) ;
    C3V_ADD ( ctr, x, a ) ;
    C3V_ADD ( a, y, b ) ;
    c3a_box_init_pt ( box, b ) ;
    C3V_SUB ( a, y, b ) ;
    c3a_box_append_pt ( box, b ) ;
    C3V_SUB ( ctr, x, a ) ;
    C3V_ADD ( a, y, b ) ;
    c3a_box_append_pt ( box, b ) ;
    C3V_SUB ( a, y, b ) ;
    c3a_box_append_pt ( box, b ) ;
    RETURN ( box ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_union ( box1, box2, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_BOX box1, box2, box ;
{
    if ( C3_MIN_X(box1) < C3_MIN_X(box2) ) 
        C3_MIN_X(box) = C3_MIN_X(box1) ;
    else 
        C3_MIN_X(box) = C3_MIN_X(box2) ;
    if ( C3_MAX_X(box1) < C3_MAX_X(box2) ) 
        C3_MAX_X(box) = C3_MAX_X(box2) ;
    else 
        C3_MAX_X(box) = C3_MAX_X(box1) ;

    if ( C3_MIN_Y(box1) < C3_MIN_Y(box2) ) 
        C3_MIN_Y(box) = C3_MIN_Y(box1) ;
    else 
        C3_MIN_Y(box) = C3_MIN_Y(box2) ;
    if ( C3_MAX_Y(box1) < C3_MAX_Y(box2) ) 
        C3_MAX_Y(box) = C3_MAX_Y(box2) ;
    else 
        C3_MAX_Y(box) = C3_MAX_Y(box1) ;

    if ( C3_MIN_Z(box1) < C3_MIN_Z(box2) ) 
        C3_MIN_Z(box) = C3_MIN_Z(box1) ;
    else 
        C3_MIN_Z(box) = C3_MIN_Z(box2) ;
    if ( C3_MAX_Z(box1) < C3_MAX_Z(box2) ) 
        C3_MAX_Z(box) = C3_MAX_Z(box2) ;
    else 
        C3_MAX_Z(box) = C3_MAX_Z(box1) ;
    RETURN ( box ) ;
}



/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3a_box_w_overlap ( box1, box2, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_BOX box1, box2 ;
REAL w ;
{
    RETURN ( c3ab_box_overlap ( box1, box2, w+BBS_TOL ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_BOX c3a_box_inflate ( box, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
REAL w ;
{
    C3_MIN_X(box) -= w ;
    C3_MAX_X(box) += w ;
    C3_MIN_Y(box) -= w ;
    C3_MAX_Y(box) += w ;
    C3_MIN_Z(box) -= w ;
    C3_MAX_Z(box) += w ;
    RETURN ( box ) ;
}

#ifdef   SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_BOX c3a_box_hpoly ( a, d, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a  ;          /* Input array */
INT  d ;            /* Number of points */
C3_BOX  box ;
{
    REAL z ;
    INT i ;

    if ( HPT3_INF(a[0]) || HPT3_INF(a[d-1]) )
        RETURN ( NULL ) ;

    C3_MIN_X(box) = C3_MAX_X(box) = HPT3_X(a[0]) ;
    C3_MIN_Y(box) = C3_MAX_Y(box) = HPT3_Y(a[0]) ;
    C3_MIN_Z(box) = C3_MAX_Z(box) = HPT3_Z(a[0]) ;
    
    for ( i=1 ; i<d ; i++ ) {
        if ( HPT3_INF(a[i]) )
            RETURN ( FALSE ) ;
        z = HPT3_X(a[i]) ;
        if ( z < C3_MIN_X(box) )
            C3_MIN_X(box) = z ;
        else if ( z > C3_MAX_X(box) )
            C3_MAX_X(box) = z ;
        z = HPT3_Y(a[i]) ;
        if ( z < C3_MIN_Y(box) )
            C3_MIN_Y(box) = z ;
        else if ( z > C3_MAX_Y(box) )
            C3_MAX_Y(box) = z ;
        z = HPT3_Z(a[i]) ;
        if ( z < C3_MIN_Z(box) )
            C3_MIN_Z(box) = z ;
        else if ( z > C3_MAX_Z(box) )
            C3_MAX_Z(box) = z ;
    }
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_BOX c3a_box_init_hpt ( box, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
HPT3 pt ;
{
    REAL a ;

    if ( HPT3_INF(pt) ) 
        RETURN ( NULL ) ;
    a = HPT3_X(pt) ;
    C3_MIN_X(box) = C3_MAX_X(box) = a ;
    a = HPT3_Y(pt) ;
    C3_MIN_Y(box) = C3_MAX_Y(box) = a ;
    a = HPT3_Z(pt) ;
    C3_MIN_Z(box) = C3_MAX_Z(box) = a ;
    RETURN ( box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_BOX c3a_box_append_hpt ( box, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
HPT3 pt ;
{
    REAL a ;

    if ( HPT3_INF(pt) ) 
        RETURN ( NULL ) ;
    a = HPT3_X(pt) ;
    if ( a < C3_MIN_X(box) ) 
        C3_MIN_X(box) = a ;
    else if ( a > C3_MAX_X(box) ) 
        C3_MAX_X(box) = a ;
    a = HPT3_Y(pt) ;
    if ( a < C3_MIN_Y(box) ) 
        C3_MIN_Y(box) = a ;
    else if ( a > C3_MAX_Y(box) ) 
        C3_MAX_Y(box) = a ;
    a = HPT3_Z(pt) ;
    if ( a < C3_MIN_Z(box) ) 
        C3_MIN_Z(box) = a ;
    else if ( a > C3_MAX_Z(box) ) 
        C3_MAX_Z(box) = a ;
    RETURN ( box ) ;
}

#endif   /*SPLINE*/

