//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3SS.C *********************************/
/***************************** Splines select ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef  SPLINE
#include <c3bdefs.h>
#include <c3sdefs.h>
#include <c3adefs.h>
#include <c3vdefs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_select ( a, d, knot, w, normal, parm0, parm1, pt, 
        view_plane_normal, tol, sel_parm, dist_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
INT d ;
REAL *knot ;
REAL w ;
PT3 normal ;
PARM parm0, parm1 ;
PT3 pt, view_plane_normal ;
REAL tol ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    BOOLEAN status, st_init ;
    INT j ;    
    HPT3 *b ;
    PARM_S parm ;
    REAL t, dist ;

    if ( sel_parm == NULL ) 
        sel_parm = &parm ;
    status = FALSE ;
    st_init = FALSE ;
    b = CREATE ( d, HPT3 ) ;
    j = PARM_J(parm0) ;
    C3S_CONV_BEZ ( a, d, knot, j, b ) ;

    if ( PARM_T(parm0) > knot[j] + BBS_ZERO ) 
        C3B_BRKR ( b, d, ( PARM_T(parm0) - knot[j] ) / 
            ( knot[j+1] - knot[j] ), b ) ;

    if ( PARM_J(parm1) == j ) {
        C3B_BRKL ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
            ( knot[j+1] - PARM_T(parm0) ), b ) ;
        status = c3b_select ( b, d, w, normal, pt, view_plane_normal, tol, 
            PARM_T(parm0), PARM_T(parm1), &(PARM_T(sel_parm)), dist_ptr ) ;
        if ( status )
            PARM_J(sel_parm) = j ;
    }

    else {
        status = c3b_select ( b, d, w, normal, pt, view_plane_normal, tol, 
            PARM_T(parm0), knot[j+1], &(PARM_T(sel_parm)), dist_ptr ) ;
        if ( status ) 
            PARM_J(sel_parm) = j ;
      
        for ( j=PARM_J(parm0)+1 ; 
            j<PARM_J(parm1) && ( !status || dist_ptr != NULL ) ; j++ ) {
            if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                status = c3b_select ( b, d, w, normal, pt, view_plane_normal, 
                    tol, knot[j], knot[j+1], &t, &dist ) ;
                if ( status ) {
                    if ( dist_ptr == NULL ) {
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                    }
                    else if ( !st_init || dist < *dist_ptr ) {
                        st_init = TRUE ;
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                        *dist_ptr = dist ;
                    }
                }
            }
        }
    
        if ( !status ) {
            j = PARM_J(parm1) ;
            if ( PARM_T(parm1) - knot[j] > BBS_ZERO ) {
                C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                C3B_BRKL ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                    ( knot[j+1] - knot[j] ), b ) ;
                status = c3b_select ( b, d, w, normal, pt, view_plane_normal, 
                    tol, knot[j], PARM_T(parm1), 
                    &(PARM_T(sel_parm)), dist_ptr ) ;
                if ( status ) {
                    if ( dist_ptr == NULL ) {
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                    }
                    else if ( !st_init || dist < *dist_ptr ) {
                        st_init = TRUE ;
                        PARM_T(sel_parm) = t ;
                        PARM_J(sel_parm) = j ;
                        *dist_ptr = dist ;
                    }
                }
            }
        }
    }
    KILL ( b ) ;
    RETURN ( status ) ;
}
#endif  /*SPLINE*/

