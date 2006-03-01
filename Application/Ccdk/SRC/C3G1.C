//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3G1.C *********************************/
/******************* Routines for displaying arc geometry ******************/
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
BBS_PRIVATE BOOLEAN c3g_approx ( arc, t0, t1, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
REAL gran ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C3_ASEG_S arc1 ;

    if ( dir ) {
        if ( t1 <= 1.0 + BBS_ZERO ) 
            RETURN ( c3r_approx ( C3_ARC_SEG(arc), t0, t1, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
        else if ( t0 >= 1.0 - BBS_ZERO ) 
            RETURN ( c3r_approx ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                t0-1.0, t1-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) ) ;
        else 
            RETURN ( c3r_approx ( C3_ARC_SEG(arc), t0, 1.0, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) && 
                c3r_approx ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                    0.0, t1-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                    buf_size, index, end_t ) ) ;
    }
    else {
        if ( t0 <= 1.0 + BBS_ZERO ) 
            RETURN ( c3r_approx ( C3_ARC_SEG(arc), t1, t0, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
        else if ( t1 >= 1.0 - BBS_ZERO ) 
            RETURN ( c3r_approx ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                t1-1.0, t0-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) ) ;
        else 
            RETURN ( c3r_approx ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                0.0, t0-1.0, 1, gran, dir, pt_buffer, parm_buffer, 
                buf_size, index, end_t ) && 
                c3r_approx ( C3_ARC_SEG(arc), t1, 1.0, 0, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3g_approx_zoomed ( arc, t0, t1, gran, view_ctr_pt, w, 
            pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
REAL gran ;
PT3 view_ctr_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C3_BOX_S box ;
    C3_ASEG_S arc1 ;

    C3_MIN_X(&box) = view_ctr_pt[0] - w ;
    C3_MAX_X(&box) = view_ctr_pt[0] + w ;
    C3_MIN_Y(&box) = view_ctr_pt[1] - w ;
    C3_MAX_Y(&box) = view_ctr_pt[1] + w ;
    C3_MIN_Z(&box) = view_ctr_pt[2] - w ;
    C3_MAX_Z(&box) = view_ctr_pt[2] + w ;

    if ( C3_ARC_ZERO_RAD(arc) ) {
        PT3 ctr ;
        if ( *index >= buf_size ) {
            *end_t = t0 ;
            RETURN ( FALSE ) ;
        }
        
        C3V_COPY ( c3r_ctr ( C3_ARC_SEG(arc), ctr ), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t0 ;
            PARM_J(parm_buffer+(*index)) = 0 ;
        }
        (*index)++ ;
        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }
        C3V_COPY ( ctr, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) {
            PARM_T(parm_buffer+(*index)) = t1 ;
            PARM_J(parm_buffer+(*index)) = t1<=1.0 ? 0 : 1 ;
        }
        (*index)++ ;
        RETURN ( TRUE ) ;
    }

    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( c3r_approx_zoomed ( C3_ARC_SEG(arc), t0, t1, 0, gran, &box, 
            w, pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) 
        RETURN ( c3r_approx_zoomed ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
            t0-1.0, t1-1.0, 1, gran, &box, w, pt_buffer, parm_buffer, 
            buf_size, index, end_t ) ) ;
    else 
        RETURN ( c3r_approx_zoomed ( C3_ARC_SEG(arc), t0, 1.0, 0, gran, &box, 
            w, pt_buffer, parm_buffer, buf_size, index, end_t ) && 
            c3r_approx_zoomed ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
            0.0, t1-1.0, 1, gran, &box, w, pt_buffer, parm_buffer, 
            buf_size, index, end_t ) ) ;
}

