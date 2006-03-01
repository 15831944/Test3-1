//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3SP.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <alsdefs.h>
#include <c3bdefs.h>
#include <c3sdefs.h>
#include <c3apriv.h>

STATIC BOOLEAN spl3_approx __(( HPT3*, INT, INT, REAL*, REAL, PT3, PARM, PARM, 
        REAL, C3_BOX, BOOLEAN, BOOLEAN, PT3*, PARM, INT, INT*, PARM )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_approx ( a, n, d, knot, offset, normal, parm0, parm1, 
        gran, dir, pt_buffer, parm_buffer, buf_size, index, end_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
INT n, d ;
REAL *knot ;
REAL offset ;
PT3 normal ;
PARM parm0, parm1 ;
REAL gran ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM end_parm ;
{

    BOOLEAN non_rational ;

    non_rational = c3a_non_rational ( a, n ) ;

    RETURN ( spl3_approx ( a, n, d, knot, offset, normal, parm0, parm1, 
        gran, NULL, dir, non_rational, pt_buffer, parm_buffer, buf_size, 
        index, end_parm ) ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_approx_zoomed ( a, n, d, knot, offset, normal, 
            parm0, parm1, gran, view_ctr_pt, w, 
            pt_buffer, parm_buffer, buf_size, index, end_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
INT n, d ;
REAL *knot ;
REAL offset ;
PT3 normal ;
PARM parm0, parm1 ;
REAL gran ;
PT3 view_ctr_pt ;
REAL w ; 
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM end_parm ;
{
    C3_BOX_S box ;

    C3_MIN_X(&box) = view_ctr_pt[0] - w ;
    C3_MIN_Y(&box) = view_ctr_pt[1] - w ;
    C3_MIN_Z(&box) = view_ctr_pt[2] - w ;
    C3_MAX_X(&box) = view_ctr_pt[0] + w ;
    C3_MAX_Y(&box) = view_ctr_pt[1] + w ;
    C3_MAX_Z(&box) = view_ctr_pt[2] + w ;

    RETURN ( spl3_approx ( a, n, d, knot, offset, normal, parm0, parm1, 
        gran, &box, TRUE, FALSE, pt_buffer, parm_buffer, 
        buf_size, index, end_parm ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN spl3_approx ( a, n, d, knot, offset, normal, parm0, parm1, 
        gran, box, dir, non_rational, pt_buffer, parm_buffer, 
        buf_size, index, end_parm )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
INT n, d ;
REAL *knot ;
REAL offset ;
PT3 normal ;
PARM parm0, parm1 ;
REAL gran ;
C3_BOX box ;
BOOLEAN dir ;
BOOLEAN non_rational ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM end_parm ;
{
    BOOLEAN status ;
    INT j0, j ;    
    HPT3 *b ;
    REAL t ;

    b = CREATE ( d, HPT3 ) ;

    if ( dir ) {
        j0 = PARM_J(parm0) ;
        while ( j0 < PARM_J(parm1) && PARM_T(parm0) >= knot[j0+1] - BBS_ZERO ) 
            j0++ ;
        C3S_CONV_BEZ ( a, d, knot, j0, b ) ;
        if ( PARM_T(parm0) > knot[j0] + BBS_ZERO ) 
            c3b_brkr ( b, d, ( PARM_T(parm0) - knot[j0] ) / 
                ( knot[j0+1] - knot[j0] ), b ) ;

        if ( PARM_J(parm1) == j0 ) {
            if ( knot[j0+1] > PARM_T(parm0) + BBS_ZERO ) 
                c3b_brkl ( b, d, ( PARM_T(parm1) - PARM_T(parm0) ) / 
                    ( knot[j0+1] - PARM_T(parm0) ), b ) ;

            status = c3b_approx ( b, d, offset, normal, PARM_T(parm0), 
                PARM_T(parm1), j0, gran, box, dir, non_rational, TRUE, 
                pt_buffer, parm_buffer, buf_size, index, &t ) ;

            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
        }

        else {
            status = c3b_approx ( b, d, offset, normal, PARM_T(parm0), 
                knot[j0+1], j0, gran, box, dir, non_rational, TRUE, 
                pt_buffer, parm_buffer, buf_size, index, &t ) ;
            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
      
            for ( j=j0+1 ; j<PARM_J(parm1) && status ; j++ ) {
                if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                    C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                    status = c3b_approx ( b, d, offset, normal, knot[j], 
                        knot[j+1], j, gran, box, dir, non_rational, (BOOLEAN)(box!=NULL), 
                        pt_buffer, parm_buffer, buf_size, index, &t ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
    
            j = PARM_J(parm1) ;
            if ( status && knot[j+1]-knot[j] > BBS_ZERO ) {
                C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) {
                    if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) 
                        c3b_brkl ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                            ( knot[j+1] - knot[j] ), b ) ;
                    status = c3b_approx ( b, d, offset, normal, knot[j], 
                        PARM_T(parm1), j, gran, box, dir, non_rational, 
                        (BOOLEAN)(box!=NULL), pt_buffer, parm_buffer, buf_size, 
                        index, &t ) || ( t >= PARM_T(parm1) ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
        }
    }

    else {
        j0 = PARM_J(parm0) ;
        while ( PARM_T(parm0) <= knot[j0] + BBS_ZERO ) 
            j0-- ;
        C3S_CONV_BEZ ( a, d, knot, j0, b ) ;
        if ( PARM_T(parm0) < knot[j0+1] - BBS_ZERO ) 
            c3b_brkl ( b, d, ( PARM_T(parm0) - knot[j0] ) / 
                ( knot[j0+1] - knot[j0] ), b ) ;

        if ( PARM_J(parm1) == j0 ) {
            c3b_brkr ( b, d, ( PARM_T(parm1) - knot[j0] ) / 
                ( PARM_T(parm0) - knot[j0] ), b ) ;

            status = c3b_approx ( b, d, offset, normal, PARM_T(parm0), 
                PARM_T(parm1), j0, gran, box, dir, non_rational, TRUE, 
                pt_buffer, parm_buffer, buf_size, index, &t ) ;

            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
        }

        else {
            status = c3b_approx ( b, d, offset, normal, PARM_T(parm0), 
                knot[j0], j0, gran, box, dir, non_rational, TRUE, 
                pt_buffer, parm_buffer, buf_size, index, &t ) ;
            if ( !status ) 
                SET_PARMJ ( t, j0, end_parm ) ;
      
            for ( j=j0-1 ; j>PARM_J(parm1) && status ; j-- ) {
                if ( knot[j+1]-knot[j] > BBS_ZERO ) {
                    C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                    status = c3b_approx ( b, d, offset, normal, knot[j+1], 
                        knot[j], j, gran, box, dir, non_rational, (BOOLEAN)(box!=NULL), 
                        pt_buffer, parm_buffer, buf_size, index, &t ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
    
            j = PARM_J(parm1) ;
            if ( status && knot[j+1]-knot[j] > BBS_ZERO ) {
                C3S_CONV_BEZ ( a, d, knot, j, b ) ;
                if ( PARM_T(parm1) < knot[j+1] - BBS_ZERO ) {
                    if ( PARM_T(parm1) > knot[j] + BBS_ZERO ) 
                        c3b_brkr ( b, d, ( PARM_T(parm1) - knot[j] ) / 
                            ( knot[j+1] - knot[j] ), b ) ;
                    status = c3b_approx ( b, d, offset, normal, knot[j+1], 
                        PARM_T(parm1), j, gran, box, dir, non_rational, 
                        (BOOLEAN)(box!=NULL), pt_buffer, parm_buffer, buf_size, 
                        index, &t ) || ( t <= PARM_T(parm1) ) ;
                    if ( !status ) 
                        SET_PARMJ ( t, j, end_parm ) ;
                }
            }
        }
    }

    KILL ( b ) ;
    RETURN ( status ) ;
}
#endif  /*SPLINE*/

