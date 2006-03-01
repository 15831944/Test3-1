//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/*********************************** C3BM.C ********************************/
/********************** Three-dimensional Bezier curves ********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef  SPLINE
#include <aladefs.h>
#include <c3bdefs.h>
#include <qglgauss.h>
#include <c3vmcrs.h>
/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3b_mass_prop ( b, d, w, normal, t0, t1, p, dim, tol, 
            prop_function, result ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

HPT3    *b  ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    /* Offset */
PT3     normal ; /* curve normal */
REAL    t0, t1 ;    /* Parameters */
INT     p ;
INT     dim ;
REAL    tol ;
PF_PROP3 prop_function ;
REAL    *result ;    /* result[dim] */
{

    INT i, m, k, j ;
    BOOLEAN success ;
    REAL h, u, t, *result_new, *result_temp ;
    PT3 *x ;
    HPT3 *bl, *br ;

    success = FALSE ;
    result_new = CREATE ( dim, REAL ) ;
    if ( result_new==NULL )
        RETURN ;
    result_temp = CREATE ( dim, REAL ) ;
    if ( result_temp==NULL )
        RETURN ;
    x = CREATE ( p+1, PT3 ) ;
    if ( x==NULL )
        RETURN ;
    h = t1 - t0 ;

    for ( m=0, k=0 ; m<8 && !success ; m++ ) {

        ala_set_zero ( result_new, dim ) ;

        for ( i=0 ; i<=m ; i++, k++ ) {

            t = 0.5 * ( 1.0 + NODE[k] ) ;
            if ( c3b_eval ( b, d, w, normal, t, p, x ) ) {
                u = 1.0 ;
                for ( j=1 ; j<=p ; j++ ) {
                    u /= h ;
                    C3V_SCALE ( x[j], u, x[j] ) ;
                }
                (*prop_function) ( x, result_temp ) ;
                ala_addt ( result_new, result_temp, WEIGHT[k], 
                    dim, result_new ) ;
            }        

            t = 0.5 * ( 1.0 - NODE[k] ) ;
            if ( c3b_eval ( b, d, w, normal, t, p, x ) ) {
                (*prop_function) ( x, result_temp ) ;
                u = 1.0 ;
                for ( j=1 ; j<=p ; j++ ) {
                    u /= h ;
                    C3V_SCALE ( x[j], u, x[j] ) ;
                }
                (*prop_function) ( x, result_temp ) ;
                ala_addt ( result_new, result_temp, WEIGHT[k], 
                    dim, result_new ) ;
            }        
        } 

        success = m && ala_diff_small ( result_new, result, dim, tol ) ;
        ala_copy ( result_new, dim, result ) ;
    }

    if ( !success ) {
        t = 0.5 * ( t0 + t1 ) ;
        bl = CREATE ( 2*d-1, HPT3 ) ;
        if ( bl==NULL )
            RETURN ;
        br = bl + (d-1) ;
        c3b_sbdv0 ( b, d, bl ) ;
        c3b_mass_prop ( bl, d, w, normal, t0, t, p, dim, 
            tol, prop_function, result ) ;
        c3b_mass_prop ( br, d, w, normal, t, t1, p, dim, 
            tol, prop_function, result_new ) ;
        ala_add ( result, result_new, dim, result ) ;
        KILL ( bl ) ;
    }
    else 
        ala_scale ( result, 0.5*h, dim, result ) ;
    KILL ( result_new ) ;
    KILL ( result_temp ) ;
    KILL ( x ) ;
}
#endif  /*SPLINE*/

