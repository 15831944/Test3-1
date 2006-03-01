//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3GS.C *********************************/
/******************** Routines for selecting arcs **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c3gdefs.h>
#include <c3rdefs.h>
#include <c3gmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3g_select ( arc, t0, t1, pt, normal, tol, 
            t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
PT3 pt, normal ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    C3_ASEG_S arc1 ;
    BOOLEAN status ;

    if ( C3_ARC_ZERO_RAD(arc) ) {
        PT3 ctr ;
        *t_ptr = 0.5 ;
        if ( C3V_DISTL1 ( c3r_ctr ( C3_ARC_SEG(arc), ctr ), pt ) > tol ) 
            RETURN ( FALSE ) ;
        if ( dist_ptr != NULL ) 
            *dist_ptr = C3V_DIST ( c3r_ctr ( C3_ARC_SEG(arc), ctr ), pt ) ;
        RETURN ( TRUE ) ;
    }

    if ( t1 <= 1.0 + BBS_ZERO ) 
        status = c3r_select ( C3_ARC_SEG(arc), t0, t1, pt, normal, 
            tol, t_ptr, dist_ptr ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) {
        status = c3r_select ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
            t0-1.0, t1-1.0, pt, normal, tol, t_ptr, dist_ptr ) ;
        if ( t_ptr != NULL ) 
            *t_ptr += 1.0 ;
    }
    else {
        status = c3r_select ( C3_ARC_SEG(arc), t0, 1.0, pt, normal, 
            tol, t_ptr, dist_ptr ) ;
        if ( !status ) {
            status = c3r_select ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                0.0, t1-1.0, pt, normal, tol, t_ptr, dist_ptr ) ;
            if ( t_ptr != NULL ) 
                *t_ptr += 1.0 ;
        }
    }
    RETURN ( status ) ;
}

