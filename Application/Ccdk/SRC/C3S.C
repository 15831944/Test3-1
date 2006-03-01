//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3S.C **********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef  SPLINE
#include <aladefs.h>
#include <alsdefs.h>
#include <c3bdefs.h>
#include <c3edefs.h>
#include <c3sdefs.h>
#include <c3vmcrs.h>
#include <c3hmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_eval ( c, n, d, knot, w, normal, parm, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *c  ;
INT n ;
INT d ;
REAL *knot  ;
REAL w ;
PT3 normal ;
PARM parm ;
INT p ;
PT3 *x  ;
{
    HPT3 *a ;
    PT3 *a1 ;
    BOOLEAN status ;

    if ( !als_parm_adjust ( n, d, knot, parm ) )
        RETURN ( FALSE ) ;
    if ( IS_ZERO(w) ) {
        a = CREATE ( p+1, HPT3 ) ;
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d, 
            knot+PARM_J(parm)-d+1, 4, PARM_T(parm), p, (REAL*)a ) ;
        status = c3e_ratio ( a, p, x ) ;
        KILL ( a ) ;
        RETURN ( status ) ;
    }
    else {
        a = CREATE ( p+2, HPT3 ) ;
        a1 = CREATE ( p+2, PT3 ) ;
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d,
            knot+PARM_J(parm)-d+1, 4, PARM_T(parm), p+1, (REAL*)a ) ;
        status = c3e_ratio ( a, p+1, a1 ) && 
            c3e_offset ( a1, w, normal, p, x ) ;
        KILL ( a ) ;
        KILL ( a1 ) ;
        RETURN ( status ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_eval_tan ( c, n, d, knot, parm, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *c  ;
INT n ;
INT d ;
REAL *knot  ;
PARM parm ;
PT3 x ;
{
    HPT3 a[2] ;

    if ( !als_parm_adjust ( n, d, knot, parm ) )
        RETURN ( FALSE ) ;
    als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d, 
        knot+PARM_J(parm)-d+1, 4, PARM_T(parm), 1, (REAL*)a ) ;
    C3H_DIFF ( a[1], a[0], x ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_project ( a, d, knot, w, normal, parm0, parm1, pt, 
        pr_parm, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *a  ;
INT     d ;
REAL    *knot  ;
REAL    w ;
PT3 normal ;
PARM parm0, parm1 ;
PT3     pt ;
PARM pr_parm ;
PT3     proj_pt ;
{
    BOOLEAN status ;
    INT j ;    
    HPT3 *b ;
    HPT3 curr_pt ;
    REAL dist0, dist, t ;    
    PT3     ppt ;

    status = FALSE ;
    b = CREATE ( d, HPT3 ) ;
    j = PARM_J(parm0) ;
    C3S_CONV_BEZ ( a, d, knot, j, b ) ;
    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        c3b_brkr ( b, d, ( PARM_T(parm0) - knot[j] ) / ( knot[j+1] - knot[j] ), 
            b ) ;

    if ( PARM_J(parm1) == j ) {
        c3b_brkl ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        status = c3b_project ( b, d, w, normal, pt, &t, ppt, &dist ) ;
        if ( status ) {
            PARM_T(pr_parm) = PARM_T(parm0) + 
                ( PARM_T(parm1) - PARM_T(parm0) ) * t ;
            PARM_J(pr_parm) = j ;
            if ( proj_pt != NULL ) 
                C3V_COPY ( ppt, proj_pt ) ;
        }
    }

    else {
        status = c3b_project ( b, d, w, normal, pt, &t, ppt, &dist ) ;
        if ( status ) {
            PARM_T(pr_parm) = PARM_T(parm0) + 
                ( knot[j+1] - PARM_T(parm0) ) * t ;
            PARM_J(pr_parm) = j ;
            if ( proj_pt != NULL ) 
                C3V_COPY ( ppt, proj_pt ) ;
        }
      
        for ( j=PARM_J(parm0) + 1 ; j < PARM_J(parm1) ; j++ ) {
            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                if ( !status ) {
                    status = c3b_project ( b, d, w, normal, pt, 
                        &t, ppt, &dist ) ;
                    if ( status ) {
                        PARM_T(pr_parm) = knot[j] + 
                            ( knot[j+1] - knot[j] ) * t ;
                        PARM_J(pr_parm) = j ;
                        if ( proj_pt != NULL ) 
                            C3V_COPY ( ppt, proj_pt ) ;
                    }
                }
                else if ( c3b_project ( b, d, w, normal, pt, 
                    &t, curr_pt, &dist0 ) && dist0 < dist ) {
                    C3V_COPY ( curr_pt, ppt ) ;
                    dist = dist0 ;
                    PARM_T(pr_parm) = knot[j] + 
                        ( knot[j+1] - knot[j] ) * t ;
                    PARM_J(pr_parm) = j ;
                    if ( proj_pt != NULL ) 
                        C3V_COPY ( ppt, proj_pt ) ;
                }
            }
        }
    
        j = PARM_J(parm1) ;
        if ( knot[j+1]-knot[j] > BBS_ZERO ) {
            C3S_CONV_BEZ ( a, d, knot, j, b ) ;
            if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) {
                if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) 
                    c3b_brkl ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                        ( knot[j+1] - knot[j] ), b ) ;
                if ( !status ) {
                    status = c3b_project ( b, d, w, normal, pt, 
                        &t, ppt, &dist ) ;
                    if ( status ) {
                        PARM_T(pr_parm) = knot[j] + 
                            ( PARM_T(parm1) - knot[j] ) * t ;
                        PARM_J(pr_parm) = j ;
                        if ( proj_pt != NULL ) 
                            C3V_COPY ( ppt, proj_pt ) ;
                    }
                }
                else if ( c3b_project ( b, d, w, normal, pt, &t, 
                    curr_pt, &dist0 ) && dist0 < dist ) {
                    C3V_COPY ( curr_pt, ppt ) ;
                    dist = dist0 ;
                    PARM_T(pr_parm) = knot[j] + 
                        ( PARM_T(parm1) - knot[j] ) * t ;
                    PARM_J(pr_parm) = j ;
                    if ( proj_pt != NULL ) 
                        C3V_COPY ( ppt, proj_pt ) ;
                }
            }
        }
    }
    KILL ( b ) ;
    RETURN ( status ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3s_mass_prop ( a, d, knot, w, normal, parm0, parm1, p, 
        dim, tol, prop_function, result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *a  ;
INT     d ;
REAL    w ;
PT3     normal ;
REAL    *knot  ;
PARM    parm0, parm1 ;
INT     p ;
INT     dim ;
REAL    tol ;
PF_PROP3 prop_function ;
REAL    *result ;    /* result[dim] */
{
    REAL *bez_result ;
    INT j ;    
    HPT3 *b ;

    ala_set_zero ( result, dim ) ;
    bez_result = CREATE ( dim, REAL ) ;
    b = CREATE ( d, HPT3 ) ;
    j = PARM_J(parm0) ;
    C3S_CONV_BEZ ( a, d, knot, j, b ) ;

    if ( PARM_J(parm1) == j ) 
        c3b_mass_prop ( b, d, w, normal, PARM_T(parm0), PARM_T(parm1), 
            p, dim, tol, prop_function, result ) ;

    else {
        c3b_mass_prop ( b, d, w, normal, PARM_T(parm0), knot[j+1], 
            p, dim, tol, prop_function, result ) ;
      
        for ( j=PARM_J(parm0)+1 ; j<PARM_J(parm1) ; j++ ) {
            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                c3b_mass_prop ( b, d, w, normal, knot[j], knot[j+1], 
                    p, dim, tol, prop_function, bez_result ) ;
                ala_add ( result, bez_result, dim, result ) ;
            }
        }
    
        if ( PARM_T(parm1) - knot[j] > BBS_ZERO ) {
            C3S_CONV_BEZ ( a, d, knot, j, b ) ;
            c3b_mass_prop ( b, d, w, normal, knot[j], PARM_T(parm1), 
                p, dim, tol, prop_function, bez_result ) ;
            ala_add ( result, bez_result, dim, result ) ;
        }
    }
    KILL ( b ) ;
    KILL ( bez_result ) ;
}    
#endif  /*SPLINE*/

