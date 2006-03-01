//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/*********************************** C3E.C *********************************/
/*********************** 3-d curve evaluation routines *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <c3edefs.h>
#include <c3vdefs.h>
#include <c3vmcrs.h>

#ifdef SPLINE
STATIC BOOLEAN c3e_norm_deriv __(( PT3*, REAL*, INT, REAL* )) ;
STATIC BOOLEAN c3e_unit_vec_deriv __(( PT3*, INT, PT3* )) ;
STATIC BOOLEAN c3e_ratio_2 __(( PT3*, REAL*, REAL*, INT, PT3* )) ;
#endif
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3e_ratio ( a, p, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *a  ;   /* a[p+1] - Array of hom. coordinates of curve and its 
                    derivatives of order ó p */
INT     p ;     /* Order of the derivatives */
PT3     *b  ;   /* b[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
{
    INT i, j, q=p+1 ;
    REAL *c, w, *c_ptr ;

    if ( HPT3_INF ( a[0] ) )
        RETURN ( FALSE ) ;
    c = CREATE ( q*q, REAL ) ;
    if ( c== NULL ) 
        RETURN ( FALSE ) ;
    ala_binom ( q, c ) ;
    for ( i=0, c_ptr=c ; i<=p ; i++, c_ptr+=q ) {
        b[i][0] = a[i][0] ;
        b[i][1] = a[i][1] ;
        b[i][2] = a[i][2] ;
        for ( j=0 ; j<i ; j++ ) {
            w = c_ptr[j] * a[i-j][3] ;
            b[i][0] -= w * b[j][0] ;
            b[i][1] -= w * b[j][1] ;
            b[i][2] -= w * b[j][2] ;
        }
        b[i][0] /= (a[0][3]) ;
        b[i][1] /= (a[0][3]) ;
        b[i][2] /= (a[0][3]) ;
    }
    KILL ( c ) ;
    RETURN ( TRUE ) ;
}        


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3e_curvature ( x, crv, ctr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3    *x  ;    /* x[3] - derivatives of order ó 2 */
REAL   *crv ;  /* Curvature */   
PT3    ctr ;   /* ctr - center of the curvature */
{
    PT3 vec ;
    REAL w, n1_sq, n1 ;

    C3V_CROSS ( x[1], x[2], vec ) ;
    n1_sq = C3V_DOT ( x[1], x[1] ) ;
    n1 = sqrt ( n1_sq ) ;
    if ( IS_SMALL(n1) ) 
        RETURN ( 0 ) ;
    *crv = C3V_NORM ( vec ) / ( n1_sq * n1 ) ;

    if ( IS_SMALL(*crv) ) 
        RETURN ( -1 ) ;

    w = - C3V_DOT ( x[1], x[2] ) / n1_sq ;
    C3V_ADDT ( x[2], x[1], w, vec ) ;
    if ( !c3v_normalize ( vec, vec ) )
        RETURN ( -1 ) ;
    w = 1.0 / *crv ;
    C3V_ADDT ( x[0], vec, w, ctr ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3e_polynomial2 ( b, t, p, f ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 b[3], *f ;
REAL t ;
INT p ; /* p>= 2 */
{
    INT i, j ;
    for ( j=0 ; j<4 ; j++ ) {
        f[0][j] = ( b[2][j] * t + b[1][j] ) * t + b[0][j] ;
        f[1][j] = 2.0 * b[2][j] * t + b[1][j] ;
        f[2][j] = 2.0 * b[2][j] ;

        for ( i=3 ; i<=p ; i++ )
            f[i][j] = 0.0 ;
    }
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3e_offset ( x, w, normal, p, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3    *x  ;    /* x[p+2] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p+1 */
REAL    w ;     /* Offset distance */
PT3     normal ;   /* normal to the plane */
INT     p ;     /* Order of the derivatives */
PT3     *y  ;   /* y[p+1] - Array of eucl. coordinates of offset curve 
                    and its derivatives of order ó p */
{
/* y = x + w * peprendicular */

    INT i ;

    if ( !c3e_normal ( x, normal, p, y ) ) 
        RETURN ( FALSE ) ;
    for ( i=0 ; i <= p ; i++ ) {
        C3V_ADDT ( x[i], y[i], -w, y[i] ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3e_normal ( x, normal, p, n )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3    *x  ;    /* x[p+2] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p+1 */
PT3     normal ;    /* normal to the plane */
INT     p ;     /* Order of the derivatives */
PT3     *n  ;   /* n[p+1] - Array of eucl. coordinates of offset curve 
                    and its derivatives of order ó p */
{
/* n = cross ( x', normal ) normalized */
    PT3 *z ;
    INT i ;
    BOOLEAN status ;

    z = CREATE ( p+2, PT3 ) ;

    for ( i = 0 ; i <= p ; i++ ) {
        C3V_CROSS ( normal, x[i+1], z[i] ) ;
    }

    status = c3e_unit_vec_deriv ( z, p, n ) ;  /* v = x'/norm(x') */
    KILL ( z ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3e_norm_deriv ( x, c, p, z )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3    *x  ;    /* x[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
REAL    *c  ;   
INT     p ;     /* Order of the derivatives */
REAL    *z  ;   /* z[p+1] - Array of norm(a) and 
                    its derivatives of order ó p */
{
    INT i, k, m, q=p+1 ;
    REAL r ;
    BOOLEAN even_k ;
    
    r = C3V_NORM ( x[0] ) ;
    if ( IS_ZERO(r) )       /* The first derivative is zero */
         RETURN ( FALSE ) ;
    
    z[0] = r ;   /* norm(a') */
    r = 1.0 / r ;

    for ( k=1, m=1, even_k = FALSE ; k<=p ; k++ ) {
        z[k] = C3V_DOT ( x[0], x[k] ) ;
        for (i=1 ; i<m ; i++ )
            z[k] += ( c[k*q+i] * ( C3V_DOT ( x[i], x[k-i] ) - z[i]*z[k-i] ) ) ;
    
        if ( even_k ) {
            z[k] += ( 0.5 * c[k*q+m] * 
                ( C3V_DOT ( x[m], x[m] ) - z[m] * z[m] ) ) ;
            m++ ;
            even_k = FALSE ;
        }
        else 
            even_k = TRUE ;
        z[k] *= r ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3e_unit_vec_deriv ( x, p, u )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3     *x  ;    /* x[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
INT     p ;     /* Order of the derivatives */
PT3     *u  ;   /* u[p+1] - Array of x/norm(x) and 
                    its derivatives of order ó p */
{
    REAL    *c  ;   /* c[(p+1)*(p+1)] - binomial coefficients */
    REAL *z ;
    BOOLEAN status ;

    c = CREATE ( (p+1)*(p+1), REAL ) ;
    z = CREATE ( p+1, REAL ) ;      /* z[p] = (norm(x)) -p-th derivative */
    ala_binom ( p+1, c ) ;
    status = c3e_norm_deriv ( x, c, p, z ) && c3e_ratio_2 ( x, z, c, p, u ) ;
    KILL ( z ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3e_ratio_2 ( x, y, c, p, z )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3     *x  ;   /* x[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
REAL    *y  ;   /* y[p+1] - Array of denominator values and its 
                    derivatives of order ó p */
REAL    *c ;     /* c[(p+1)*(p+1)] - binomial coefficients */
INT     p ;     /* Order of the derivatives */
PT3     *z  ;   /* z[p+1] - Array of eucl. coordinates of curve and its 
                    derivatives of order ó p */
{
    INT i, j, q=p+1 ;
    REAL w, *c_ptr ;

    if ( IS_ZERO(y[0]) )
        RETURN ( FALSE ) ;
    for ( i=0, c_ptr=c ; i<=p ; i++, c_ptr+=q ) {
        C3V_COPY ( x[i], z[i] ) ;
        for ( j=0 ; j<i ; j++ ) {
            w = c_ptr[j] * y[i-j] ;
            z[i][0] -= w * z[j][0] ;
            z[i][1] -= w * z[j][1] ;
            z[i][2] -= w * z[j][2] ;
        }
        z[i][0] /= y[0] ;
        z[i][1] /= y[0] ;
        z[i][2] /= y[0] ;
    }
    RETURN ( TRUE ) ;
}
#endif  /*SPLINE*/
