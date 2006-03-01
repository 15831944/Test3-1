//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3D.C **********************************/
/********* Three-dimensional curve construction routines ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c3ndefs.h>
#include <c3pdefs.h>
#include <c3cdefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>
#include <c3mem.h>
#include <c3vmcrs.h>
#include <dx0defs.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE c3d_curve () 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    C3_CURVE curve ;

    curve = C3_ALLOC_CURVE ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_LINE(curve) = NULL ;
    C3_CURVE_TYPE(curve) = C3_NURB_TYPE ;

    C3_CURVE_LAYER_SET(curve,NULL); // mhm added

    C3_CURVE_X_MIN(curve) = 0.0; // kga added
    C3_CURVE_X_MAX(curve) = 0.0;
    C3_CURVE_Y_MIN(curve) = 0.0;
    C3_CURVE_Y_MAX(curve) = 0.0;
    C3_CURVE_Z_MIN(curve) = 0.0;
    C3_CURVE_Z_MAX(curve) = 0.0;

    C3_CURVE_T0(curve) = 0;
    C3_CURVE_J0(curve) = 0;
    C3_CURVE_T1(curve) = 0;
    C3_CURVE_J1(curve) = 0;

    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void c3d_free_curve ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    if ( curve == NULL ) 
        RETURN ;
    else if ( C3_CURVE_IS_LINE(curve) ) {
        C3_FREE_LINE ( C3_CURVE_LINE(curve) ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        C3_FREE_ARC ( C3_CURVE_ARC(curve) ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        c3p_free ( C3_CURVE_PCURVE(curve) ) ;
    }
#ifdef  SPLINE
    else 
        c3n_free_nurb ( C3_CURVE_NURB(curve) ) ;
#endif  /* SPLINE */

    // mhm free the layer on the curve
    dx0_strfree(&curve->layer);

    C3_FREE_CURVE ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC PT3 *c3d_point ( x, y, z ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z ;
{
    REAL *pt ;
     
    pt = MALLOC ( 3, REAL ) ;
    if ( pt != NULL )
        C3V_SET ( x, y, z, pt ) ;
    RETURN ( (PT3*)pt ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void c3d_free_point ( pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *pt ;
{
    FREE ( *pt ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_copy ( curve0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve0 ;
{
    C3_CURVE curve ;
    curve = C3_ALLOC_CURVE ;

    C3_CURVE_LAYER_SET(curve,NULL); // mhm added



    if ( curve != NULL ) 
        c3c_copy ( curve0, curve ) ;
    if ( C3_CURVE_LINE(curve) == NULL ) {
        c3d_free_curve ( curve ) ;
        curve = NULL ;
    }
    RETURN ( curve ) ;
}

