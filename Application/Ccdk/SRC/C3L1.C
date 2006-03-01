//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3L.C **********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3ldefs.h>
#include <c3vmcrs.h>
#include <c3lmcrs.h>

STATIC BOOLEAN clip_line3 __(( PT3, PT3, REAL, REAL, PT3, PT3, REAL, 
        PT3*, PARM, INT, INT*, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3l_approx ( line, t0, t1, pt_buffer, parm_buffer, 
        buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line ;
REAL t0, t1 ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    if ( *index >= buf_size ) {
        *end_t = t0 ;
        RETURN ( FALSE ) ;
    }

    if ( IS_ZERO(t0) ) {
        C3V_COPY ( C3_LINE_PT0(line), pt_buffer[*index] ) ;
    }
    else {
        C3V_ADDU ( C3_LINE_PT0(line), C3_LINE_PT1(line), t0, 
        pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = t0 ;
        PARM_J(parm_buffer+(*index)) = 1 ;
    }
    (*index)++ ;

    if ( *index >= buf_size ) {
        *end_t = t1 ;
        RETURN ( FALSE ) ;
    }

    if ( IS_ZERO(t1-1.0) ) {
        C3V_COPY ( C3_LINE_PT1(line), pt_buffer[*index] ) ;
    }
    else {
        C3V_ADDU ( C3_LINE_PT0(line), C3_LINE_PT1(line), t1, 
        pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = t1 ;
        PARM_J(parm_buffer+(*index)) = 1 ;
    }
    (*index)++ ;

    *end_t = t1 ;
    RETURN ( TRUE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3l_approx_zoomed ( line, t0, t1, view_ctr_pt, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line ;
REAL t0, t1 ;
PT3 view_ctr_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    PT3 min_pt, max_pt, pt0, pt1 ;
    INT i ;

    if ( *index >= buf_size ) {
        *end_t = t0 ;
        RETURN ( FALSE ) ;
    }

    if ( IS_ZERO(t0) ) {
        C3V_COPY ( C3_LINE_PT0(line), pt0 ) ;
    }
    else {
        C3V_ADDU ( C3_LINE_PT0(line), C3_LINE_PT1(line), t0, pt0 ) ;
    }

    if ( IS_ZERO(t1-1.0) ) {
        C3V_COPY ( C3_LINE_PT1(line), pt1 ) ;
    }
    else {
        C3V_ADDU ( C3_LINE_PT1(line), C3_LINE_PT1(line), t1, pt1 ) ;
    }

    for ( i=0 ; i<3 ; i++ ) {
        min_pt[i] = view_ctr_pt[i] - w ;
        max_pt[i] = view_ctr_pt[i] + w ;
    }

    RETURN ( clip_line3 ( pt0, pt1, t0, t1, min_pt, max_pt, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;

}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN clip_line3 ( pt0, pt1, t0, t1, min_pt, max_pt, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1 ;
REAL t0, t1 ;
PT3 min_pt, max_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    PT3 pt ;
    REAL t ;
    INT i ;

    for ( i=0 ; i<3 ; i++ ) {
        if ( ( pt0[i] < min_pt[i] && pt1[i] < min_pt[i] ) || 
             ( pt0[i] > max_pt[i] && pt1[i] > max_pt[i] ) )
            RETURN ( TRUE ) ;
    }

    if ( C3V_DISTL1 ( pt0, pt1 ) <= w ) {

        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }

        C3V_COPY ( pt0, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t0 ;
            PARM_J(parm_buffer+(*index)) = 1 ;
        }
        (*index)++ ;

        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }

        C3V_COPY ( pt1, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t1 ;
            PARM_J(parm_buffer+(*index)) = 1 ;
        }
        (*index)++ ;
        *end_t = t1 ;
        RETURN ( TRUE ) ;
    }

    else {
        t = 0.5 * ( t0 + t1 ) ;
        C3V_MID_PT ( pt0, pt1, pt ) ;
        RETURN ( clip_line3 ( pt0, pt, t0, t, min_pt, max_pt, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) && 
            clip_line3 ( pt, pt1, t, t1, min_pt, max_pt, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
    }
}

