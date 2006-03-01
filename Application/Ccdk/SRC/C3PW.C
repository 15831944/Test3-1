//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3PW.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef __NDP__
#include <alwdefs.h>
#include <c3pdefs.h>
#include <c3qdefs.h>
#include <dmldefs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_get ( file, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
PARM parm0, parm1 ;
{
    INT n ;
    C3_PCURVE pcurve ;

    if ( !alw_get_int ( file, &n ) )
        RETURN ( NULL ) ;
    pcurve = c3p_create ( n, n ) ;
    if ( !c3q_get ( file, C3_PCURVE_BUFFER(pcurve), n ) ||
        !alw_get_parm ( file, parm0 ) || !alw_get_parm ( file, parm1 ) ) {
        c3p_free ( pcurve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( pcurve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_put ( file, pcurve, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
C3_PCURVE pcurve ;
PARM parm0, parm1 ;
{
    INT n ;
    DML_ITEM item ;
    C3_PC_BUFFER buffer ;
    BOOLEAN status ;

    n = c3p_n ( pcurve ) ;

    if ( !alw_put_int ( file, n, NULL ) ) 
        RETURN ( FALSE ) ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    status = ( c3q_n ( buffer ) > 0 ) ;
    if ( !c3q_put ( file, buffer, FALSE ) ) 
        RETURN ( FALSE ) ;

    C3_WALK_PCURVE ( pcurve, item ) {
        buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
        if ( !c3q_put ( file, buffer, status ) ) 
            RETURN ( FALSE ) ;
        if ( !status ) 
            status = ( c3q_n ( buffer ) > 0 ) ;
    }
    RETURN ( alw_put_parm ( file, parm0, NULL ) && 
        alw_put_parm ( file, parm1, NULL ) ) ;
}
#endif

