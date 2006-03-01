//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3N.C **********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <aladefs.h>
#include <alsdefs.h>
#include <c3apriv.h>
#include <c3ndefs.h>
#include <c3mem.h>
#include <c3nmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_create_nurb ( n, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT n, d ;
{
    C3_NURB nurb ;

    nurb = C3_ALLOC_NURB ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    C3_NURB_N(nurb) = n ;
    C3_NURB_D(nurb) = d ;
    C3_NURB_CTPT_PTR(nurb) = MALLOC ( n, HPT3 ) ;
    C3_NURB_KNOT_PTR(nurb) = MALLOC ( n+d, REAL ) ;
    if ( C3_NURB_CTPT(nurb) == NULL || C3_NURB_KNOT(nurb) == NULL ) {
        c3n_free_nurb ( nurb ) ;
        nurb = NULL ;
    }
    C3_NURB_W(nurb) = 0.0 ;
    C3V_SET ( 0.0, 0.0, 1.0, C3_NURB_NORMAL(nurb) ) ;
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_ctpts ( a, nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a ;
C3_NURB nurb ;
{
    c3a_pt_hpt ( a, C3_NURB_N(nurb), C3_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_cthpts ( a, nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
C3_NURB nurb ;
{
    ala_copy ( (REAL*)a, 4*C3_NURB_N(nurb), (REAL*)C3_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_knots ( knot, nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL *knot ;
C3_NURB nurb ;
{
    if ( knot == NULL ) 
        c3n_set_uniform_knots ( nurb ) ;
    else
        ala_copy ( knot, C3_NURB_N(nurb) + C3_NURB_D(nurb), 
            C3_NURB_KNOT(nurb) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_uniform_knots ( nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    als_uniform_knots ( C3_NURB_N(nurb), C3_NURB_D(nurb), 
        C3_NURB_KNOT(nurb) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_clsc_uni_knots ( nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    als_clsc_uni_knots ( C3_NURB_N(nurb), C3_NURB_D(nurb), 
        C3_NURB_KNOT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_create_copy_nurb ( nurb0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb0 ;
{
    C3_NURB nurb ;

    nurb = c3n_create_nurb ( C3_NURB_N(nurb0), C3_NURB_D(nurb0) ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    ala_copy ( (REAL*)C3_NURB_CTPT(nurb0), 4*C3_NURB_N(nurb), 
        (REAL*)C3_NURB_CTPT(nurb) ) ;
    ala_copy ( C3_NURB_KNOT(nurb0), C3_NURB_N(nurb)+C3_NURB_D(nurb), 
        C3_NURB_KNOT(nurb) ) ;
    C3_NURB_W(nurb) = C3_NURB_W(nurb0) ;
    C3V_COPY ( C3_NURB_NORMAL(nurb0), C3_NURB_NORMAL(nurb) ) ;
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_free_nurb ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
    {
    FREE ( C3_NURB_CTPT(nurb) ) ;
    FREE ( C3_NURB_KNOT(nurb) ) ;
    C3_FREE_NURB ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_closed ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3V_IDENT_PTS ( C3_NURB_CTPT(nurb)[0], 
        C3_NURB_CTPT(nurb)[C3_NURB_N(nurb)-1] ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_trim ( nurb0, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb0 ;
PARM parm0, parm1 ;
{
    INT j0, j1, n, d=C3_NURB_D(nurb0) ;
    C3_NURB nurb ;

    if ( PARM_T(parm1) <= PARM_T(parm0) + BBS_ZERO )
        RETURN ( NULL ) ;
    j0 = ( parm0 == NULL ) ? d-1 : PARM_J(parm0) ;
    j1 = ( parm1 == NULL ) ? C3_NURB_N(nurb0)-1 : PARM_J(parm1) ;
    if ( IS_ZERO ( C3_NURB_KNOT(nurb0)[j1] - PARM_T(parm1) ) )
        j1-- ;
    n = j1 - j0 + d ;
    nurb = c3n_create_nurb ( n, d ) ;
    if ( nurb == NULL ) 
        RETURN ( nurb ) ;
    als_trim ( (REAL*)C3_NURB_CTPT(nurb0), d, C3_NURB_KNOT(nurb0), 4, 
        parm0, parm1, (REAL*)C3_NURB_CTPT(nurb), n, C3_NURB_KNOT(nurb) ) ;
    RETURN ( nurb ) ;
}
#endif  /*SPLINE*/

