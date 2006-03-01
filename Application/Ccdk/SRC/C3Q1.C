//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3Q1.C *********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3qdefs.h>
#include <c3rdefs.h>
#include <c3vmcrs.h>
#include <c3qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_approx ( buffer, t0, t1, j0, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
REAL gran ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    INT i0, i1, i ;
    REAL u0, u1 ;

    if ( C3_PC_BUFFER_N(buffer) <= 0 ) 
        RETURN ( TRUE ) ;
    if ( t0 >= (REAL)(j0+C3_PC_BUFFER_N(buffer)-1) || t1 <= (REAL)j0 )
        RETURN ( TRUE ) ;
    u0 = ( t0 > (REAL)j0 ) ? ( t0 - (REAL)j0 ) : 0.0 ;
    u1 = ( t1 < (REAL)(j0+C3_PC_BUFFER_N(buffer)-1) ) ? 
        ( t1 - (REAL)j0 ) : (REAL)(C3_PC_BUFFER_N(buffer)-1) ;
    i0 = (INT)u0 ;
    i1 = (INT)u1 ;

    if ( C3_PC_BUFFER_N(buffer) == 1 ) {
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        C3V_COPY ( C3_PC_BUFFER_PT(buffer,0), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            PARM_SETJ ( t0, i0+j0, parm_buffer+(*index) ) ;
        (*index)++ ;
        *end_t = t0 ;
        RETURN ( TRUE ) ;
    }

    u0 -= (REAL)i0 ;
    u1 -= (REAL)i1 ;

    if ( i0==i1 ) 
        RETURN ( c3r_approx ( C3_PC_BUFFER_ARC(buffer,i0), u0, u1, i0+j0, 
            gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;

    if ( !c3r_approx ( C3_PC_BUFFER_ARC(buffer,i0), u0, 1.0, i0+j0, gran, 
        dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
        RETURN ( FALSE ) ;

    for ( i=i0+1 ; i<i1 ; i++ ) {
        (*index)-- ;
        if ( !c3r_approx ( C3_PC_BUFFER_ARC(buffer,i), 0.0, 1.0, i+j0, gran, 
            dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }

    if ( u1 > BBS_ZERO ) {
        (*index)-- ;
        if ( !c3r_approx ( C3_PC_BUFFER_ARC(buffer,i1), 0.0, u1, i1+j0, gran, 
            dir, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }

    RETURN ( TRUE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_approx_zoomed ( buffer, t0, t1, j0, gran, 
        view_ctr_pt, w, pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
REAL gran ;
PT3 view_ctr_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C3_BOX_S box ;
    INT i0, i1, i ;
    REAL u0, u1 ;

    if ( C3_PC_BUFFER_N(buffer) <= 0 ) 
        RETURN ( TRUE ) ;
    if ( t0 >= (REAL)(j0+C3_PC_BUFFER_N(buffer)-1) || t1 <= (REAL)j0 )
        RETURN ( TRUE ) ;
    u0 = ( t0 > (REAL)j0 ) ? ( t0 - (REAL)j0 ) : 0.0 ;
    u1 = ( t1 < (REAL)(j0+C3_PC_BUFFER_N(buffer)-1) ) ? 
        ( t1 - (REAL)j0 ) : (REAL)(C3_PC_BUFFER_N(buffer)-1) ;
    i0 = (INT)u0 ;
    i1 = (INT)u1 ;

    C3_MIN_X(&box) = view_ctr_pt[0] - w ;
    C3_MAX_X(&box) = view_ctr_pt[0] + w ;
    C3_MIN_Y(&box) = view_ctr_pt[1] - w ;
    C3_MAX_Y(&box) = view_ctr_pt[1] + w ;
    C3_MIN_Z(&box) = view_ctr_pt[2] - w ;
    C3_MAX_Z(&box) = view_ctr_pt[2] + w ;

    if ( C3_PC_BUFFER_N(buffer) == 1 ) {
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        C3V_COPY ( C3_PC_BUFFER_PT(buffer,0), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            PARM_SETJ ( t0, i0+j0, parm_buffer+(*index) ) ;
        (*index)++ ;
        *end_t = t0 ;
        RETURN ( TRUE ) ;
    }

    u0 -= (REAL)i0 ;
    u1 -= (REAL)i1 ;

    if ( i0==i1 ) 
        RETURN ( c3r_approx_zoomed ( C3_PC_BUFFER_ARC(buffer,i0), u0, u1, i0, 
            gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;

    if ( !c3r_approx_zoomed ( C3_PC_BUFFER_ARC(buffer,i0), u0, 1.0, i0, 
        gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
        RETURN ( FALSE ) ;

    for ( i=i0+1 ; i<i1 ; i++ ) {
        if ( !c3r_approx_zoomed ( C3_PC_BUFFER_ARC(buffer,i), 0.0, 1.0, i, 
            gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }

    if ( u1 > BBS_ZERO ) {
        if ( !c3r_approx_zoomed ( C3_PC_BUFFER_ARC(buffer,i1), 0.0, u1, i1, 
            gran, &box, w, pt_buffer, parm_buffer, buf_size, index, end_t ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


