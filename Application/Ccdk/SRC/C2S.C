//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/********************************** C2S.C **********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef  SPLINE
#include <alsdefs.h>
#include <c2sdefs.h>
#include <c2edefs.h>
#include <c2vmcrs.h>
#include <c2hmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_eval ( c, n, d, knot, w, parm, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c  ;
INT n ;
INT d ;
REAL *knot  ;
REAL w ;
PARM parm ;
INT p ;
PT2 *x  ;
{
    HPT2 *a ;
    PT2 *a1 ;
    BOOLEAN status ;

    if ( !als_parm_adjust ( n, d, knot, parm ) )
        RETURN ( FALSE ) ;
    if ( IS_ZERO(w) ) { /* no offset */
        a = CREATE ( p+1, HPT2 ) ;
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d,
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), p, (REAL*)a ) ;
        status = c2e_ratio ( a, p, x ) ;
        KILL ( a ) ;
    }
    else {  /* offset */
        a = CREATE ( p+2, HPT2 ) ;
        a1 = CREATE ( p+2, PT2 ) ;
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d,
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), p+1, (REAL*)a ) ;
        status = c2e_ratio ( a, p+1, a1 ) && c2e_offset ( a1, w, p, x ) ;
        KILL ( a ) ;
        KILL ( a1 ) ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_pt_tan ( c, n, d, knot, w, parm, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c  ;
INT n ;
INT d ;
REAL *knot  ;
REAL w ;
PARM parm ;
PT2 pt, tan_vec ;
{
    HPT2 x1, x[2] ;
    REAL r ;

    if ( !als_parm_adjust ( n, d, knot, parm ) )
        RETURN ( FALSE ) ;
    if ( tan_vec == NULL && IS_SMALL(w) ) 
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d, 
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), 0, (REAL*)x ) ;
    else 
        als_eval ( (REAL*)(c+PARM_J(parm)-d+1), d, 
            knot+PARM_J(parm)-d+1, 3, PARM_T(parm), 1, (REAL*)x ) ;
    if ( tan_vec != NULL ) 
        C2H_DIFF ( x[1], x[0], x1 ) ;
    if ( pt != NULL ) {
        if ( HPT2_INF(x[0]) ) 
            RETURN ( FALSE ) ;
        pt[0] = x[0][0] / x[0][2] ;
        pt[1] = x[0][1] / x[0][2] ;

        if ( !IS_SMALL(w) ) {
            r = C2V_NORM ( x1 ) ;
            if ( IS_SMALL(r) ) 
                RETURN ( FALSE ) ;
            r = w / r ;
            C2V_OFFSET ( pt, x1, r, pt ) ;
        }
    }
    if ( tan_vec != NULL ) 
        C2V_COPY ( x1, tan_vec ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2s_coinc ( c1, n1, d1, knot1, w1, c2, n2, d2, knot2, w2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *c1, *c2 ;
INT n1, n2 ;
INT d1, d2 ;
REAL *knot1, *knot2  ;
REAL w1, w2 ;
{
    INT i ;

    if ( n1 != n2 || d1 != d2 )
        RETURN ( FALSE ) ;
    if ( !IS_SMALL ( w1 - w2 ) )
        RETURN ( FALSE ) ;

    for ( i = 0 ; i < n1+d1 ; i++ ) {
        if ( !IS_SMALL ( knot1[i] - knot2[i] ) )
            RETURN ( FALSE ) ;
    }

    for ( i = 0 ; i < n1 ; i++ ) {
        if ( !IS_SMALL ( c1[i][0] - c2[i][0] ) )
            RETURN ( FALSE ) ;
        if ( !IS_SMALL ( c1[i][1] - c2[i][1] ) )
            RETURN ( FALSE ) ;
        if ( !IS_SMALL ( c1[i][2] - c2[i][2] ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}
#endif  /*SPLINE*/

