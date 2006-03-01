//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3PD.C *********************************/
/**************** Routines for processing polycurve geometry ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3pdefs.h>
#include <c3qdefs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_create ( s, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT s, n ;
{
    C3_PCURVE pcurve ;

    pcurve = MALLOC ( 1, C3_PCURVE_S ) ;
    if ( pcurve==NULL ) 
        RETURN ( NULL ) ;
    c3q_initialize ( C3_PCURVE_BUFFER(pcurve), s, n ) ; 
    C3_PCURVE_BLIST(pcurve) = NULL ;
    RETURN ( pcurve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3p_free ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    if ( pcurve==NULL ) 
        RETURN ;
    c3q_clear ( C3_PCURVE_BUFFER(pcurve) ) ;
    dml_destroy_list ( C3_PCURVE_BLIST(pcurve), c3q_free ) ;
    FREE ( pcurve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_create_copy ( pcurve0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve0 ;
{
    C3_PCURVE pcurve ;
    C3_PC_BUFFER buffer0 ;

    buffer0 = C3_PCURVE_BUFFER(pcurve0) ;
    pcurve = c3p_create ( c3q_size ( buffer0 ) , c3q_n ( buffer0 ) ) ;
    if ( pcurve == NULL ) 
        RETURN ( NULL ) ;
    c3q_copy ( buffer0, C3_PCURVE_BUFFER(pcurve) ) ;
    C3_PCURVE_BLIST(pcurve) = 
        dml_recreate ( C3_PCURVE_BLIST(pcurve0), (PF_ANY1)c3q_create_copy ) ; 
    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_add_arc_2pts ( pcurve, a0, a1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 a0, a1 ;
{
    RETURN ( c3q_add_arc_2pts ( c3p_last_buffer(pcurve), a0, a1 ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_add_arc_ctr_pt ( pcurve, ctr, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 ctr, a ;
{
    RETURN ( c3q_add_arc_ctr_pt ( c3p_last_buffer(pcurve), ctr, a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_add_arc_tan ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 a ;
{
    RETURN ( c3q_add_arc_tan ( c3p_last_buffer(pcurve), a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_add_line ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 a ;
{
    RETURN ( c3q_add_line ( c3p_last_buffer(pcurve), a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_add_line_tan ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 a ;
{
    RETURN ( c3q_add_line_tan ( c3p_last_buffer(pcurve), a ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_remove_last ( pcurve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    RETURN ( c3q_remove_last ( c3p_last_buffer(pcurve) ) == NULL ? 
        NULL : pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_close ( pcurve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    PT3 p0 ;
    c3p_ept0 ( pcurve, p0 ) ;
    RETURN ( c3p_add_line ( pcurve, p0 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_trim ( pcurve0, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve0 ;
REAL t0, t1 ;
{
    DML_ITEM item ;
    C3_PCURVE pcurve1 = c3p_create ( 0, 0 ) ;
    C3_PC_BUFFER buffer0, buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer0 = C3_PCURVE_BUFFER(pcurve0) ;

    c3q_trim ( buffer0, t0, t1, j0, C3_PCURVE_BUFFER(pcurve1) ) ; 
    j0 += ( c3q_n ( buffer0 ) - 1 ) ;

    C3_WALK_PCURVE ( pcurve0, item ) {
        buffer0 = (C3_PC_BUFFER)DML_RECORD(item) ;
        buffer = c3q_trim ( buffer0, t0, t1, j0, NULL ) ; 
        j0 += ( c3q_n ( buffer0 ) - 1 ) ;
        if ( buffer != NULL ) {
            if ( C3_PCURVE_BLIST(pcurve1) == NULL ) 
                C3_PCURVE_BLIST(pcurve1) = dml_create_list () ;
            dml_append_data ( C3_PCURVE_BLIST(pcurve1), buffer ) ; 
        }
    }
    RETURN ( pcurve1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PCURVE c3p_through ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
{
    C3_PCURVE pcurve ;

    pcurve = c3p_create ( n, n ) ;
    if ( pcurve == NULL ) 
        RETURN ( NULL ) ;
    c3q_through ( a, n, C3_PCURVE_BUFFER(pcurve) ) ;
    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3p_init ( pcurve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 a ;
{
    C3_PC_BUFFER buffer ;
    buffer = c3p_first_buffer ( pcurve ) ;
    if ( buffer == NULL ) 
        RETURN ;
    c3q_init ( buffer, a ) ;
}


