//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3P1.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3pdefs.h>
#include <c3qdefs.h>
#include <dmldefs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_approx ( pcurve, t0, t1, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
REAL gran ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    DML_ITEM item ;
    INT j, n ;
    C3_PC_BUFFER buffer ;

    if ( dir ) {
        j = 0 ;
        buffer = C3_PCURVE_BUFFER(pcurve) ;
        if ( !c3q_approx ( buffer, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
        j += ( c3q_n ( buffer ) - 1 ) ;

        C3_WALK_PCURVE ( pcurve, item ) {
            buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
            if ( !c3q_approx ( buffer, t0, t1, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
            j += ( c3q_n ( buffer ) - 1 ) ;
        }
    }
    else {
        n = c3p_n ( pcurve ) ;
        t0 -= ( (REAL)n - 1.0 ) ;
        t1 -= ( (REAL)n - 1.0 ) ;
        j = n ;

        for ( item = dml_last ( C3_PCURVE_BLIST(pcurve) ) ; 
              item != NULL ; item = dml_prev ( item ) ) {
            buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
            j -= ( c3q_n ( buffer ) - 1 ) ;
            if ( !c3q_approx ( DML_RECORD(item), t0, t1, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) )
                RETURN ( FALSE ) ;
        }
        if ( !c3q_approx ( C3_PCURVE_BUFFER(pcurve), t0, t1, 0, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_approx_zoomed ( pcurve, t0, t1, gran, view_ctr_pt, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
REAL gran ;
PT3 view_ctr_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    DML_ITEM item ;
    INT j ;
    C3_PC_BUFFER buffer ;

    j = 0 ;
    buffer = C3_PCURVE_BUFFER(pcurve) ; 

    if ( !c3q_approx_zoomed ( buffer, t0, t1, j, gran, 
        view_ctr_pt, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
        RETURN ( FALSE ) ;
    j += ( c3q_n ( buffer ) - 1 ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
        if ( !c3q_approx_zoomed ( DML_RECORD(item), t0, t1, j, gran, 
            view_ctr_pt, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
        j += ( c3q_n ( buffer ) - 1 ) ;
    }
    RETURN ( TRUE ) ;
}

