//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3PS.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3pdefs.h>
#include <c3qdefs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_select ( pcurve, t0, t1, pt, normal, tol, 
        t_ptr, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
PT3 pt, normal ;
REAL tol ;
REAL *t_ptr ;
REAL *dist_ptr ;
{
    DML_ITEM item ;
    C3_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;

    if ( c3q_select ( buffer, t0, t1, j0, pt, normal, tol, t_ptr, dist_ptr ) )
        RETURN ( TRUE ) ;
    j0 += ( c3q_n ( buffer ) - 1 ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
        if ( c3q_select ( buffer, t0, t1, j0, pt, normal, tol, 
            t_ptr, dist_ptr ) )
            RETURN ( TRUE ) ;
        j0 += ( c3q_n ( buffer ) - 1 ) ;
    }
    RETURN ( FALSE ) ;
}

