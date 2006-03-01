//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3SD.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <alsdefs.h>
#include <c3bdefs.h>
#include <c3pdefs.h>
#include <c3rdefs.h>
#include <c3sdefs.h>
#include <c3vmcrs.h>
#include <c3hmcrs.h>

STATIC  BOOLEAN c3sd_tan_vec_parab __(( PT3, PT3, PT3, REAL, REAL, 
            REAL, PT3 )) ;
STATIC  void    c3sd_tan_vec_magnitude __(( PT3, PT3, PT3, PT3 )) ;
STATIC  BOOLEAN c3sd_start_tangent __(( PT3*, INT, REAL*, PT3, 
            C3_TAN_OPTIONS, PT3 )) ;
STATIC  BOOLEAN c3sd_end_tangent __(( PT3*, INT, REAL*, PT3, 
            C3_TAN_OPTIONS, PT3 )) ;
STATIC  INT     c3s_arc __(( C3_ASEG, HPT3* )) ;

/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3sd_tan_vec_parab ( pt0, pt1, pt2, t0, t1, t2, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, pt2 ; /* interpolating points */
REAL t0, t1, t2 ;   /* knots */
PT3 tan_vec    ;    /* computed tangent vector */
{
    REAL h10, h21, h20, u, v ;
    
    h10 = t1 - t0 ;
    h21 = t2 - t1 ;
    if ( ( IS_ZERO(h10) ) || ( IS_ZERO(h21) ) )
        RETURN ( FALSE ) ;
    h20 = t2 - t0 ;
    u = h20 / ( h10 * h21 ) ;
    v = h10 / ( h20 * h21 ) ;
    tan_vec[0] = ( pt1[0] - pt0[0] ) * u - ( pt2[0] - pt0[0] ) * v ;
    tan_vec[1] = ( pt1[1] - pt0[1] ) * u - ( pt2[1] - pt0[1] ) * v ;
    tan_vec[2] = ( pt1[2] - pt0[2] ) * u - ( pt2[2] - pt0[2] ) * v ;
    RETURN ( TRUE ) ;
}    


/*-------------------------------------------------------------------------*/
STATIC void c3sd_tan_vec_magnitude ( pt0, pt1, dir_vec, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1 ; /* interpolating points */
PT3 dir_vec    ;    /* input tangent direction vector */
PT3 tan_vec    ;    /* output tangent direction vector */
{
    REAL t ;
    PT3 diff ;

    C3V_SUB ( pt1, pt0, diff ) ;
    t = C3V_DOT ( diff, dir_vec ) / C3V_DOT ( dir_vec, dir_vec ) ;
    C3V_SCALE ( dir_vec, t, tan_vec ) ;
}    


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3sd_start_tangent ( a, n, knot, tan0, tan0_options, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-2] */
INT n ;
REAL *knot  ;   /* knot[n+4] */
PT3 tan0    ;   /* input (if any) atngent vector */
C3_TAN_OPTIONS tan0_options ;
PT3 tan_vec ;
{
    if ( tan0_options == C3_NDEF_TAN ) {
        if ( n==4 ) {
            C3V_SUB ( a[1], a[0], tan_vec ) ;
            RETURN ( TRUE ) ;
        }
        else 
            RETURN ( c3sd_tan_vec_parab ( a[0], a[1], a[2], 
                    knot[3], knot[4], knot[5], tan_vec ) ) ;
    }
    else if ( tan0_options == C3_DEF_TAN ) {
        c3sd_tan_vec_magnitude ( a[0], a[1], tan0, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( tan0_options == C3_DEF_TAN_MAGN ) {
        C3V_COPY ( tan0, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3sd_end_tangent ( a, n, knot, tan1, tan1_options, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-2] */
INT n ;
REAL *knot  ;   /* knot[n+4] */
PT3 tan1    ;   /* input (if any) atngent vector */
C3_TAN_OPTIONS tan1_options ;
PT3 tan_vec ;
{
    if ( tan1_options == C3_NDEF_TAN ) {
        if ( n==4 ) {
            C3V_SUB ( a[1], a[0], tan_vec ) ;
            RETURN ( TRUE ) ;
        }
        else 
            RETURN ( c3sd_tan_vec_parab ( a[n-3], a[n-4], a[n-5], 
                    knot[n], knot[n-1], knot[n-2], tan_vec ) ) ;
    }
    else if ( tan1_options == C3_DEF_TAN ) {
        c3sd_tan_vec_magnitude ( a[n-4], a[n-3], tan1, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else if ( tan1_options == C3_DEF_TAN_MAGN ) {
        C3V_COPY ( tan1, tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_interp_knots ( a, n, knot_options, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-2] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
REAL *knot  ;   /* knot[n+4] */
{

    REAL c ;
    INT i ;
    PT3 vec ;

    if ( knot_options == C3_DEFAULT ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) {
            C3V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C3V_NORM ( vec ) ;
            if ( c <= BBS_ZERO ) 
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + sqrt ( c ) ;
        }
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C3_UNIFORM ) {
        knot[0] = -3.0 ;
        for ( i=1 ; i<n+4 ; i++ ) 
            knot[i] = knot[i-1] + 1.0 ;
        RETURN ( TRUE ) ;
    }   
    else if ( knot_options == C3_CLSC_UNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] = knot[i-1] + 1.0 ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }   
    else if ( knot_options == C3_NONUNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) {
            C3V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C3V_NORM ( vec ) ;
            if ( c <= BBS_ZERO ) 
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + c ;
        }
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }   
    else 
        RETURN ( FALSE ) ;
}       

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_clsd_interp_knots ( a, n, knot_options, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-3] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
REAL *knot  ;   /* knot[n+4] */
{

    REAL c ;
    INT i ;
    PT3 vec ;

    if ( knot_options == C3_DEFAULT ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<n ; i++ ) {
            C3V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C3V_NORM ( vec ) ;
            if ( c <= BBS_ZERO ) 
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + sqrt ( c ) ;
        }
        C3V_SUB ( a[0], a[n-4], vec ) ;
        c = C3V_NORM ( vec ) ;
        if ( c <= BBS_ZERO ) 
            RETURN ( FALSE ) ;
        knot[n] = knot[n-1] + sqrt ( c ) ;
        
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }
    else if ( knot_options == C3_UNIFORM ) {
        knot[0] = -3.0 ;
        for ( i=1 ; i<n+4 ; i++ ) 
            knot[i] = knot[i-1] + 1.0 ;
        RETURN ( TRUE ) ;
    }   
    else if ( knot_options == C3_CLSC_UNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] = knot[i-1] + 1.0 ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }   
    else if ( knot_options == C3_NONUNI ) {
        knot[0] = knot[1] = knot[2] = knot[3] = 0.0 ;
        for ( i=4 ; i<n ; i++ ) {
            C3V_SUB ( a[i-3], a[i-4], vec ) ;
            c = C3V_NORM ( vec ) ;
            if ( c <= BBS_ZERO ) 
                RETURN ( FALSE ) ;
            knot[i] = knot[i-1] + c ;
        }
        C3V_SUB ( a[0], a[n-4], vec ) ;
        c = C3V_NORM ( vec ) ;
        if ( c <= BBS_ZERO ) 
            RETURN ( FALSE ) ;
        knot[n] = knot[n-1] + c ;
        
        c = (REAL)(n-3) / knot[n] ;
        for ( i=4 ; i<=n ; i++ ) 
            knot[i] *= c ;
        knot[n+1] = knot[n+2] = knot[n+3] = knot[n] ;
        RETURN ( TRUE ) ;
    }   
    else 
        RETURN ( FALSE ) ;
}       


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_interp_tan ( a, n, knot_options,
        tan0, tan0_options, tan1, tan1_options, c, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-2] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
PT3 tan0    ;
C3_TAN_OPTIONS tan0_options ;
PT3 tan1    ;
C3_TAN_OPTIONS tan1_options ;
PT3 *c     ;   /* c[n]  */
REAL *knot  ;   /* knot[n+4] */

{
    PT3 tan0_vec, tan1_vec ;

    RETURN ( c3s_interp_knots ( a, n, knot_options, knot ) &&
             c3sd_start_tangent ( a, n, knot, tan0, tan0_options, tan0_vec ) &&
             c3sd_end_tangent ( a, n, knot, tan1, tan1_options, tan1_vec ) &&
             als_interp_tan ( (REAL*)a, n, 3, knot, 
                    tan0_vec, tan1_vec, (REAL*)c ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_interp_clsd ( a, n, knot_options, c, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-2] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
PT3 *c     ;    /* c[n]  */
REAL *knot  ;   /* knot[n+4] */
{
    RETURN ( c3s_clsd_interp_knots ( a, n, knot_options, knot ) &&
             als_interp_clsd ( (REAL*)a, n, 3, knot, (REAL*)c ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3s_pcurve ( pcurve, a, knot ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
HPT3 *a      ;   /* a[2*n+1] */
REAL *knot  ;   /* knot[2*n+4] */
{
    INT i, j, k ;
    C3_ASEG arc ;

    knot[0] = 0.0 ;

    for ( i=0, j=0, arc = c3p_segment ( pcurve, 0 ) ; arc != NULL ; 
        i++, arc = c3p_segment ( pcurve, i ) ) {
        k = c3s_arc ( arc, a+j ) ;
        if ( k == 3 ) {
            knot[j+1] = knot[j+2] = (REAL)i ;
            j += 3 ;
        }
        else {
            knot[j+1] = knot[j+2] = (REAL)i ;
            knot[j+3] = knot[j+4] = (REAL)i+0.5 ;
            j += 4 ;
        }
    }
    knot[j+1] = knot[j+2] = knot[j+3] = (REAL)i ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c3s_arc ( arc, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
HPT3 *a      ;   /* a[2*n+1] */
{
    HPT3 bez0[3] ;

    if ( fabs ( c3r_d_parm ( arc ) ) <= 1.0 - BBS_ZERO ) {
        c3r_bez ( arc, a ) ;
        RETURN ( 3 ) ;
    }
    else {
        c3r_bez ( arc, bez0 ) ;
        C3B_SBDV0 ( bez0, 3, a ) ;
        RETURN ( 5 ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3s_ellipse ( ctr, major_axis, minor_axis, a, knot ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 ctr ;
PT3 major_axis, minor_axis ;
HPT3 *a ;
REAL *knot ;
{
    HPT3 b[3] ;
    INT i ;

    C3H_PT_HPT ( major_axis, b[0] ) ;
    C3H_PT_HPT ( minor_axis, b[1] ) ;
    b[1][3] = 0.0 ;
    C3H_PT_HPT ( major_axis, b[2] ) ;
    C3V_NEGATE ( b[2], b[2] ) ;
    C3B_SBDV0 ( b, 3, a ) ;
    for ( i=4 ; i<9 ; i++ ) {
        C3V_NEGATE ( a[i-4], a[i] ) ;
        a[i][3] = a[i-4][3] ;
    }

    for ( i=0 ; i<9 ; i++ ) {
        C3H_ADDV ( a[i], ctr, a[i] ) ;
    }
    als_ellipse_knots ( 12, knot ) ;
}
#endif  /*SPLINE*/

