//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3QS.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3qdefs.h>
#include <c3rdefs.h>
#include <c3qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_select ( buffer, t0, t1, j0, pt, normal, tol, 
            t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
PT3 pt, normal ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    INT i ;
    REAL dist, t ;
    BOOLEAN status = FALSE ;

    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) {
        if ( c3r_select ( C3_PC_BUFFER_ARC(buffer,i), 0.0, 1.0, 
            pt, normal, tol, &t, &dist ) ) {
            t += (REAL) ( i + j0 ) ;
            if ( ( t >= t0 - BBS_ZERO ) && ( t <= t1 + BBS_ZERO ) ) {
                if ( dist_ptr == NULL ) {
                    if ( t_ptr != NULL ) 
                        *t_ptr = t ;
                    RETURN ( TRUE ) ;
                }
                if ( !status || dist < *dist_ptr ) {
                    status = TRUE ;
                    if ( t_ptr != NULL ) 
                        *t_ptr = t ;
                    *dist_ptr = dist ;
                }
            }
        }
    }
    RETURN ( dist_ptr != NULL && *dist_ptr <= tol ) ;
}

