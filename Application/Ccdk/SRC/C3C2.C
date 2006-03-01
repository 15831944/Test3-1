//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3C2.C *********************************/
/**************** Three-dimensional curves ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3ndefs.h>
#include <c3pdefs.h>
#include <c3adefs.h>
#include <c3cdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_copy ( curve0, curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0, curve ;
{
    C3_CURVE_TYPE(curve) = C3_CURVE_TYPE(curve0) ;
    C3_CURVE_T0(curve) = C3_CURVE_T0(curve0) ;
    C3_CURVE_J0(curve) = C3_CURVE_J0(curve0) ;
    C3_CURVE_T1(curve) = C3_CURVE_T1(curve0) ;
    C3_CURVE_J1(curve) = C3_CURVE_J1(curve0) ;

    if ( C3_CURVE_IS_LINE(curve) ) 
        C3_CURVE_LINE(curve) = c3l_create_copy ( C3_CURVE_LINE(curve0) ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        C3_CURVE_ARC(curve) = c3g_create_copy ( C3_CURVE_ARC(curve0) ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        C3_CURVE_PCURVE(curve) = c3p_create_copy ( C3_CURVE_PCURVE(curve0) ) ;
#ifdef  SPLINE
    else 
        C3_CURVE_NURB(curve) = c3n_create_copy_nurb ( C3_CURVE_NURB(curve0) ) ;
#endif  /* SPLINE */
    c3a_box_copy ( C3_CURVE_BOX(curve0), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}

