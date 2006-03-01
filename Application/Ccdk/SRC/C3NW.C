//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3NW.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef __NDP__
#ifdef  SPLINE
#include <alwdefs.h>
#include <c3ndefs.h>
#include <c3sdefs.h>
#include <c3nmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_get ( file, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
PARM parm0, parm1 ;
{
    INT n, d ;
    C3_NURB nurb ;

    if ( !alw_get_int ( file, &n ) || !alw_get_int ( file, &d ) )
        RETURN ( NULL ) ;
    nurb = c3n_create_nurb ( n, d ) ;
    if ( !c3s_get ( file, C3_NURB_CTPT(nurb), n, d, C3_NURB_KNOT(nurb), 
        &C3_NURB_W(nurb), C3_NURB_NORMAL(nurb) ) ||
        !alw_get_parm ( file, parm0 ) || !alw_get_parm ( file, parm1 ) ) {
        c3n_free_nurb ( nurb ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_put ( file, nurb, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
C3_NURB nurb ;
PARM parm0, parm1 ;
{
    RETURN ( c3s_put ( file, C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
            C3_NURB_D(nurb), C3_NURB_KNOT(nurb), C3_NURB_W(nurb), 
            C3_NURB_NORMAL(nurb), parm0, parm1 ) ) ;
}
#endif  /*SPLINE*/
#endif  /* NDP */

