//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/*********************************** C3B.C *********************************/
/*********************** Two-dimensional Bezier curves *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <albdefs.h>
#include <c3apriv.h>
#include <c3bdefs.h>
#include <c3hmcrs.h>
#include <c3vmcrs.h>
STATIC  BOOLEAN c3b_approx1 __(( HPT3*, INT, REAL, PT3, REAL, REAL, INT, REAL, 
            C3_BOX, BOOLEAN, PT3*, PARM, INT, INT*, REAL* )) ;
STATIC  BOOLEAN c3b_approx_non_rat __(( HPT3*, INT, REAL, REAL, INT, REAL, 
            BOOLEAN, BOOLEAN, PT3*, PARM, INT, INT*, REAL* )) ;
STATIC BOOLEAN c3b_approx1_non_rat __(( PT3*, INT, REAL, REAL, INT, REAL, 
            BOOLEAN, PT3*, PARM, INT, INT*, REAL* )) ;
STATIC BOOLEAN c3b_approx2_non_rat __(( PT3*, INT, REAL, REAL, INT, REAL, 
            BOOLEAN, PT3*, PARM, INT, INT*, REAL* )) ;
STATIC BOOLEAN c3b_approx3_non_rat __(( PT3*, REAL, REAL, INT, REAL, BOOLEAN, 
            PT3*, PARM, INT, INT*, REAL* )) ;
STATIC BOOLEAN c3b_approx4_non_rat __(( PT3, PT3, PT3, PT3, REAL, REAL, INT, 
            REAL, BOOLEAN, PT3*, PARM, INT, INT*, REAL* )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3b_approx ( b, d, w, normal, t0, t1, j, gran, box, dir, 
    non_rational, ept_bit, pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b ;  /* Control points of the segment */
INT     d ;    /* Order of the segment */
REAL    w ;    
PT3     normal ;
REAL    t0, t1 ;    
INT     j ;    
REAL    gran ;
C3_BOX  box ;
BOOLEAN dir, non_rational, ept_bit ;
PT3     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    if ( box == NULL && non_rational && IS_SMALL(w) ) 
        RETURN ( c3b_approx_non_rat ( b, d, t0, t1, j, gran, dir, ept_bit, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;

    if ( ept_bit && *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }

    if ( ept_bit && !HPT3_INF(b[0]) ) {
        if ( dir ) 
            c3b_ept0 ( b, d, w, normal, pt_buffer[*index] ) ;
        else 
            c3b_ept1 ( b, d, w, normal, pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t0, j, parm_buffer +(*index) ) ;
        (*index)++ ;
    }
    if ( *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }
    RETURN ( c3b_approx1 ( b, d, w, normal, t0, t1, j, gran, box, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_approx1 ( b, d, w, normal, t0, t1, j, gran, box, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *b ;
INT d ;
REAL w ;
PT3 normal ;
REAL t0, t1 ;
INT j ;
REAL gran ;
C3_BOX box ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *t_ptr ;
{
    REAL flatness ;
    HPT3 *bl, *br ;
    REAL mid_t ;
    BOOLEAN status ; 
    INT i ;

    flatness = c3a_hpoly_flat ( b, d ) ;

    if ( !IS_SMALL(w) && flatness <= gran ) {
        PT3 x0, x1, n0, n1, x, n, cross ;

        if ( c3b_ept0 ( b, d, w, normal, x0 ) && 
            c3b_ept1 ( b, d, w, normal, x1 ) && 
            c3b_enormal0 ( b, d, w, normal, n0 ) && 
            c3b_enormal1 ( b, d, w, normal, n1 ) ) {
            C3V_SUB ( x1, x0, x ) ;
            C3V_SUB ( n1, n0, n ) ;
            C3V_CROSS ( x, n, cross ) ;
            flatness += 0.5 * fabs(w) * C3V_NORML1 ( cross ) ;
        }
    }

    if ( box != NULL && flatness >= 0.0 ) {
        for ( i=0 ; i<3 ; i++ ) {
            if ( ( b[0][i]/b[0][3] < C3_MIN_PT(box)[i] - flatness && 
                b[d-1][i]/b[d-1][3] < C3_MIN_PT(box)[i] - flatness ) ) {
                *t_ptr = t1 ;
                RETURN ( FALSE ) ;
            }
            if ( ( b[0][i]/b[0][3] > C3_MAX_PT(box)[i] + flatness && 
                b[d-1][i]/b[d-1][3] > C3_MAX_PT(box)[i] + flatness ) ) {
                *t_ptr = t1 ;
                RETURN ( FALSE ) ;
            }
        }
    }

    if ( flatness >= 0.0 && flatness <= gran ) {
        if ( dir ? c3b_ept1 ( b, d, w, normal, pt_buffer[*index] ) :
            c3b_ept0 ( b, d, w, normal, pt_buffer[*index] ) ) {
            if ( parm_buffer != NULL ) 
                SET_PARMJ ( t0, j, parm_buffer +(*index) ) ;
            (*index)++ ;
            if ( *index >= buf_size ) {
                *t_ptr = t1 ;
                RETURN ( FALSE ) ;
            }
        }
        RETURN ( TRUE ) ;
    }

    bl = CREATE ( 2*d-1, HPT3 ) ;
    br = bl + d - 1 ;
    c3b_sbdv0 ( b, d, bl ) ;
    mid_t = 0.5 * ( t0 + t1 ) ;
    if ( dir ) 
        status = c3b_approx1 ( bl, d, w, normal, t0, mid_t, j, gran, box, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            c3b_approx1 ( br, d, w, normal, mid_t, t1, j, gran, box, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ;
    else
        status = c3b_approx1 ( br, d, w, normal, t0, mid_t, j, gran, box, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            c3b_approx1 ( bl, d, w, normal, mid_t, t1, j, gran, box, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ;

    KILL ( bl ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_approx_non_rat ( b, d, t0, t1, j, gran, dir, ept_bit, 
    pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3    *b ;  /* Control points of the segment ; w=0*/
INT     d ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir, ept_bit ;
PT3     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT3 *c ;
    INT i ;
    BOOLEAN status ;

    if ( *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }

    c = CREATE ( d, PT3 ) ;
    for ( i=0 ; i<d ; i++ ) 
        C3V_COPY ( b[i], c[i] ) ;

    if ( ept_bit ) {
        if ( dir ) {
            C3V_COPY ( c[0], pt_buffer[*index] ) ;
        }
        else {
            C3V_COPY ( c[d-1], pt_buffer[*index] ) ;
        }
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t0, j, parm_buffer + (*index) ) ;
        (*index)++ ;
    }

    if ( *index >= buf_size ) {
        *t_ptr = t0 ;
        RETURN ( FALSE ) ;
    }
    status = c3b_approx1_non_rat ( c, d, t0, t1, j, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, t_ptr ) ;
    KILL ( c ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_approx1_non_rat ( b, d, t0, t1, j, gran, dir, 
    pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3     *b ;
INT     d ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT3     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    if ( d==4 ) 
        RETURN ( c3b_approx3_non_rat ( b, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;

    else 
        RETURN ( c3b_approx2_non_rat ( b, d, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_approx2_non_rat ( b, d, t0, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3     *b ;
INT     d ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT3     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    REAL    flatness ;
    PT3 *bl, *br ;
    REAL mid_t ;
    BOOLEAN status ;

    flatness = c3a_poly_flat ( b, d ) ;
    if ( flatness <= gran ) {
        if ( dir ) {
            C3V_COPY ( b[d-1], pt_buffer[*index] ) ;
        }
        else {
            C3V_COPY ( b[0], pt_buffer[*index] ) ;
        }
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer + (*index) ) ;
        (*index)++ ;
        if ( *index >= buf_size ) {
            *t_ptr = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
    }

    bl = CREATE ( 2*d-1, PT3 ) ;
    br = bl + d - 1 ;
    alb_sbdv0 ( (REAL*)b, d, 3, (REAL*)bl ) ;
    mid_t = 0.5 * ( t0 + t1 ) ;
    if ( dir ) 
        status = c3b_approx2_non_rat ( bl, d, t0, mid_t, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            c3b_approx2_non_rat ( br, d, mid_t, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ;
    else
        status = c3b_approx2_non_rat ( br, d, t0, mid_t, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
            c3b_approx2_non_rat ( bl, d, mid_t, t1, j, gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, t_ptr ) ;

    KILL ( bl ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_approx3_non_rat ( b, t0, t1, j, gran, dir, 
    pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

PT3    *b ;  /* Control points of the segment ; w=0*/
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT3     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT3 c0, c1, c2, c3 ;

    C3V_COPY ( b[0], c0 ) ;
    C3V_COPY ( b[3], c1 ) ;
    c2[0] = b[2][0] - 2.0*b[1][0] + b[0][0] ;
    c2[1] = b[2][1] - 2.0*b[1][1] + b[0][1] ;
    c2[2] = b[2][2] - 2.0*b[1][2] + b[0][2] ;
    c3[0] = b[3][0] - 2.0*b[2][0] + b[1][0] ;
    c3[1] = b[3][1] - 2.0*b[2][1] + b[1][1] ;
    c3[2] = b[3][2] - 2.0*b[2][2] + b[1][2] ;

    RETURN ( c3b_approx4_non_rat ( c0, c1, c2, c3, t0, t1, j, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3b_approx4_non_rat ( b0, b1, c0, c1, t0, t1, j, gran, dir, 
    pt_buffer, parm_buffer, buf_size, index, t_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3     b0, b1, c0, c1 ;
REAL    t0, t1 ;
INT     j ;
REAL    gran ;
BOOLEAN dir ;
PT3     *pt_buffer ;
PARM    parm_buffer ;
INT     buf_size, *index ;
REAL    *t_ptr ;
{
    PT3 bm, cl, cm, cr ;
    REAL mid_t ;

    if ( fabs(c0[0]) <= gran && fabs(c0[1]) <= gran && fabs(c0[2]) <= gran && 
         fabs(c1[0]) <= gran && fabs(c1[1]) <= gran && fabs(c1[2]) <= gran ) {

        if ( dir ) {
            C3V_COPY ( b1, pt_buffer[*index] ) ;
        }
        else {
            C3V_COPY ( b0, pt_buffer[*index] ) ;
        }
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer + (*index) ) ;
        (*index)++ ;
        if ( *index >= buf_size ) {
            *t_ptr = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
    }

    C3V_SCALE ( c0, 0.25, cl ) ;
    C3V_SCALE ( c1, 0.25, cr ) ;
    C3V_MID_PT ( cl, cr, cm ) ;
    C3V_MID_PT ( b0, b1, bm ) ;
    C3V_ADDT ( bm, cm, -3.0, bm ) ;
    mid_t = 0.5 * ( t0 + t1 ) ;

    if ( dir ) 
        RETURN ( c3b_approx4_non_rat ( b0, bm, cl, cm, t0, mid_t, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
             c3b_approx4_non_rat ( bm, b1, cm, cr, mid_t, t1, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
    else
        RETURN ( c3b_approx4_non_rat ( bm, b1, cm, cr, t0, mid_t, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, t_ptr ) &&
             c3b_approx4_non_rat ( b0, bm, cl, cm, mid_t, t1, j, gran, dir, 
                pt_buffer, parm_buffer, buf_size, index, t_ptr ) ) ;
}

#endif  /*SPLINE*/


