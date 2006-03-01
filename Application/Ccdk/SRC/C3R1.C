//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3R.C **********************************/
/******************* Routines for processing arc geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <alrdefs.h>
#include <c3rdefs.h>
#include <c3rmcrs.h>
#include <c3vmcrs.h>
STATIC BOOLEAN c3r_approx1 __(( C3_ASEG, REAL, REAL, INT, REAL, C3_BOX, REAL, 
        REAL, PT3*, PARM, INT, INT*, REAL* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_approx ( arc, t0, t1, j, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
REAL t0, t1 ;
REAL gran ;
INT j ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    INT index0, i, n ;
    REAL d, u, v, sweep, angle, c, s ;
    PT3 ctr, vec, x_vec, y_vec, x_vec1, y_vec1 ;

    if ( *index >= buf_size ) {
        *end_t = t0 + (REAL)j ;
        RETURN ( FALSE ) ;
    }

    d = c3r_d_parm ( arc ) ;

    if ( !IS_SMALL(t0) || !IS_SMALL(t1-1.0) ) {
        C3_ASEG_S trim_arc ;
        BOOLEAN status ;

        index0 = *index ;
        status = c3r_approx ( c3r_trim ( arc, t0, t1, &trim_arc ), 0.0, 1.0, 
            j, gran, dir, pt_buffer, parm_buffer, buf_size, index, end_t ) ;
        *end_t = alr_trim_parm ( d, *end_t - (REAL)j, t0, t1 ) + (REAL)j ;
        if ( parm_buffer != NULL ) {
            for ( i=index0 ; i<*index ; i++ ) {
                PARM_T(parm_buffer+i) = alr_trim_parm ( d, 
                    PARM_T(parm_buffer+i) - (REAL)j, t0, t1 ) + (REAL)j ;
                PARM_J(parm_buffer+i) = j ;
            }
        }
        RETURN ( status ) ;
    }

    sweep = alr_sweep ( d ) ;
    u = C3V_DIST ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) ;
    if ( IS_SMALL(u) ) 
        n = 1 ;
    else if ( fabs(d) * u <= 0.5 * gran ) 
        n = (INT) ( sqrt ( 0.5 * fabs(sweep) * 
            ( 1.0 + d*d ) * u / gran ) + 1.0 ) ;
    else {
        u = 2.0 * fabs(d) * gran / ( u * ( 1.0 + d * d ) ) ;
        u = sqrt ( u ) ;
        u = u > 1.0 ? HALF_PI : asin ( u ) ;
        n = (INT) ( 0.5 * fabs(sweep) / u + 1.0 ) ;
    }
    if ( dir ) {
        C3V_COPY ( C3_ASEG_PT0(arc), pt_buffer[*index] ) ;
    }
    else {
        C3V_COPY ( C3_ASEG_PT1(arc), pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = dir ? (REAL)j : (REAL)(j+1) ;
        PARM_J(parm_buffer+(*index)) = j ;
    }
    (*index)++ ;

    if ( *index >= buf_size ) {
        *end_t = dir ? (REAL)j : (REAL)(j+1) ;
        RETURN ( FALSE ) ;
    }
    
    if ( n > 1 ) {
        (void)c3r_ctr_rad ( arc, ctr ) ;
        if ( dir ) {
            C3V_SUB ( C3_ASEG_PT0(arc), ctr, x_vec ) ;
        }
        else {
            C3V_SUB ( C3_ASEG_PT1(arc), ctr, x_vec ) ;
        }
        C3V_SUB ( C3_ASEG_PT1(arc), C3_ASEG_PT0(arc), vec ) ;
        if ( !dir ) {
            C3V_NEGATE ( vec, vec ) ;
        }
        v = C3V_DOT ( vec, vec ) ;
        u = C3V_DOT ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) * v ;
        c = v - u ;
        s = 2.0 * v ;
        C3V_ADDW ( vec, c, C3_ASEG_D_VEC(arc), sqrt ( v ) * s, y_vec ) ;
        c = 0.25 / sqrt ( u * v ) ;
        C3V_SCALE ( y_vec, c, y_vec ) ;
        if ( !dir ) {
            C3V_NEGATE ( y_vec, y_vec ) ;
        }
        angle = sweep / ( (REAL)n ) ; 
        c = cos ( angle ) ;
        s = sin ( angle ) ;
        if ( !dir ) 
            s = -s ;
        for ( i=1 ; i<n ; i++ ) {

            C3V_COPY ( x_vec, x_vec1 ) ;
            C3V_COPY ( y_vec, y_vec1 ) ;
            C3V_ADDW ( x_vec1, c, y_vec1, s, x_vec ) ;
            C3V_ADDW ( x_vec1, -s, y_vec1, c, y_vec ) ;
            C3V_ADD ( x_vec, ctr, pt_buffer[*index] ) ;
            if ( parm_buffer != NULL ) {
                PARM_T(parm_buffer+(*index)) = 
                    (REAL)j + alr_parm ( (REAL)i * angle, d ) ;
                PARM_J(parm_buffer+(*index)) = j ;
            }
            (*index)++ ;
            if ( *index == buf_size ) {
                *end_t = (REAL)j + ( dir ? 
                    alr_parm ( (REAL)i*angle, d ) :
                    alr_parm ( sweep-(REAL)i*angle, d ) ) ;
                RETURN ( FALSE ) ;
            }
        }
    }

    if ( dir ) {
        C3V_COPY ( C3_ASEG_PT1(arc), pt_buffer[*index] ) ;
    }
    else {
        C3V_COPY ( C3_ASEG_PT0(arc), pt_buffer[*index] ) ;
    }
    if ( parm_buffer != NULL ) {
        PARM_T(parm_buffer+(*index)) = dir ? (REAL)(j+1) : (REAL)j ;
        PARM_J(parm_buffer+(*index)) = j ; 
    }
    (*index)++ ;
    *end_t = dir ? 1.0 + (REAL)j : (REAL)j ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_approx_zoomed ( arc, t0, t1, j, gran, box, w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ; 
REAL t0, t1 ;
REAL gran ;
INT j ;
C3_BOX box ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    REAL flatness ;
    BOOLEAN status ;

    if ( !IS_SMALL(t0) || !IS_SMALL(t1-1.0) ) {
        C3_ASEG_S trim_arc ;
        INT i, index0 ;
 
        index0 = *index ;
        status = c3r_approx_zoomed ( c3r_trim ( arc, t0, t1, &trim_arc ), 
            0.0, 1.0, j, gran, box, w, pt_buffer, parm_buffer, buf_size, 
            index, end_t ) ;
        *end_t = alr_trim_parm ( c3r_d_parm(arc), *end_t - (REAL)j, t0, t1 ) 
            + (REAL)j ;

        if ( parm_buffer != NULL ) {
            for ( i=index0 ; i<*index ; i++ ) {
                PARM_T(parm_buffer+i) = alr_trim_parm ( c3r_d_parm(arc), 
                    PARM_T(parm_buffer+i) - (REAL)j, t0, t1 ) + (REAL)j ;
                PARM_J(parm_buffer+i) = j ;
            }
        }
        RETURN ( status ) ;
    }

    flatness = c3r_flatness ( arc ) ;
    status = c3r_approx1 ( arc, t0, t1, j, gran, box, flatness, 10.0*w, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) ;
    *end_t += (REAL)j ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3r_approx1 ( arc, t0, t1, j, gran, box, flatness, size, 
        pt_buffer, parm_buffer, buf_size, index, end_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
REAL t0, t1 ;
INT j ;
REAL gran ;
C3_BOX box ;
REAL flatness, size ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
REAL *end_t ;
{
    C3_ASEG_S arc0, arc1 ;
    REAL t, fl ;

    if ( ( C3_ASEG_PT0(arc)[0] < C3_MIN_X(box) - flatness && 
           C3_ASEG_PT1(arc)[0] < C3_MIN_X(box) - flatness ) || 
         ( C3_ASEG_PT0(arc)[0] > C3_MAX_X(box) + flatness && 
           C3_ASEG_PT1(arc)[0] > C3_MAX_X(box) + flatness ) || 
         ( C3_ASEG_PT0(arc)[1] < C3_MIN_Y(box) - flatness && 
           C3_ASEG_PT1(arc)[1] < C3_MIN_Y(box) - flatness ) || 
         ( C3_ASEG_PT0(arc)[1] > C3_MAX_Y(box) + flatness && 
           C3_ASEG_PT1(arc)[1] > C3_MAX_Y(box) + flatness ) ||
         ( C3_ASEG_PT0(arc)[2] < C3_MIN_Z(box) - flatness && 
           C3_ASEG_PT1(arc)[2] < C3_MIN_Z(box) - flatness ) || 
         ( C3_ASEG_PT0(arc)[2] > C3_MAX_Z(box) + flatness && 
           C3_ASEG_PT1(arc)[2] > C3_MAX_Z(box) + flatness ) ) {
        *end_t = t1 ;
        RETURN ( TRUE ) ;
    }

    if ( flatness <= gran && 
        C3V_DISTL1 ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) <= size ) {

        C3V_COPY ( C3_ASEG_PT1(arc), pt_buffer[*index] ) ;
        if ( parm_buffer != NULL ) 
            SET_PARMJ ( t1, j, parm_buffer +(*index) ) ;
        (*index)++ ;
        if ( *index >= buf_size ) {
            *end_t = t1 ;
            RETURN ( FALSE ) ;
        }
        RETURN ( TRUE ) ;
    }

    c3r_sbdv ( arc, &arc0, &arc1 ) ;
#ifdef DEBUG
{
    PT3 ctr, ctr0, ctr1 ;
    REAL rad, rad0, rad1 ;
    rad = c3r_ctr_rad ( arc, ctr ) ;
    rad0 = c3r_ctr_rad ( &arc0, ctr0 ) ;
    rad1 = c3r_ctr_rad ( &arc1, ctr1 ) ;
}
#endif
    t = 0.5 * ( t0 + t1 ) ;
    fl = c3r_flatness ( &arc0 ) ;
    RETURN ( c3r_approx1 ( &arc0, t0, t, j, gran, box, fl, size, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) &&
             c3r_approx1 ( &arc1, t, t1, j, gran, box, fl, size, 
                pt_buffer, parm_buffer, buf_size, index, end_t ) ) ;
}

