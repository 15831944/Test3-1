//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3LS.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3ldefs.h>
#include <c3vmcrs.h>
#include <c3lmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3l_select ( line, t0, t1, pt, normal, tol, 
            t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line ;
REAL t0, t1 ;
PT3 pt, normal ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    PT3 d, v, x, y ;
    REAL eps, delta, ny, t ;

    eps = tol * C3V_NORML1(normal) ;
    C3_LINE_DIR_VEC ( line, d ) ;
    C3V_SUB ( pt, C3_LINE_PT0(line), v ) ;
    C3V_CROSS ( v, normal, x ) ;
    C3V_CROSS ( d, normal, y ) ;
    ny = C3V_NORML1(y) ;
    if ( ny <= eps ) 
        t = 0.5 * ( t0 + t1 ) ;
    else {
        t = C3V_DOT ( x, y ) / C3V_DOT ( y, y ) ;
        C3V_ADDT ( x, y, -t, x ) ;
    }
    if ( t_ptr != NULL ) 
        *t_ptr = t ;
    delta = tol / C3V_NORML1 ( d ) ;
    if ( C3V_NORML1 ( x ) <= eps && t>=t0-delta && t<=t1+delta ) {
        if ( dist_ptr != NULL ) 
            *dist_ptr = C3V_NORM ( x ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}

