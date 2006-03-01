//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3C8.C *********************************/
/**************** Three-dimensional curves ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alsdefs.h>
#include <c3cdefs.h>
#include <c3ldefs.h>
#include <c3gdefs.h>
#include <c3ndefs.h>
#include <c3vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_trim0 ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm ;
{
    COPY_PARM ( parm, C3_CURVE_PARM0(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_trim1 ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PARM parm ;
{
    COPY_PARM ( parm, C3_CURVE_PARM1(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_trim ( curve, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PARM parm0, parm1 ;
{
    if ( parm0 != NULL ) 
        COPY_PARM ( parm0, C3_CURVE_PARM0(curve) ) ;
    if ( parm1 != NULL ) 
        COPY_PARM ( parm1, C3_CURVE_PARM1(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_trim_t0 ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
REAL t ;
{
    C3_CURVE_T0(curve) = t ;
    c3c_parm_adjust ( curve, C3_CURVE_PARM0(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_trim_t1 ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
REAL t ;
{
    C3_CURVE_T1(curve) = t ;
    c3c_parm_adjust ( curve, C3_CURVE_PARM1(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_trim_t ( curve, t0, t1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
REAL t0, t1 ;
{
    c3c_trim_t0 ( curve, t0 ) ;
    c3c_trim_t1 ( curve, t1 ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_parm_adjust ( curve, parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm ;
{
    if ( C3_CURVE_IS_LINE(curve) ) 
        PARM_J(parm) = 1 ;
    if ( C3_CURVE_IS_ARC(curve) || C3_CURVE_IS_PCURVE(curve) ) 
        PARM_J(parm) = (INT)PARM_T(parm) ;
#ifdef SPLINE
    if ( C3_CURVE_IS_ELLIPSE(curve) || C3_CURVE_IS_SPLINE(curve) ) /* Added CNM */
/* CNM     else  */
        c3n_parm_adjust ( C3_CURVE_NURB(curve), parm ) ;
#endif /*SPLINE*/
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_reverse ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    REAL t ;

    if ( C3_CURVE_IS_LINE(curve) ) {
        c3l_reverse ( C3_CURVE_LINE(curve), C3_CURVE_LINE(curve) ) ;
        t = C3_CURVE_T0(curve) ;
        C3_CURVE_T0(curve) = 1.0 - C3_CURVE_T1(curve) ;
        C3_CURVE_T1(curve) = 1.0 - t ;
    }

    else if ( C3_CURVE_IS_ARC(curve) ) {
        c3g_reverse ( C3_CURVE_ARC(curve), C3_CURVE_ARC(curve) ) ;
        t = C3_CURVE_T0(curve) ;
        C3_CURVE_T0(curve) = 1.0 - C3_CURVE_T1(curve) ;
        C3_CURVE_T1(curve) = 1.0 - t ;
    }

#ifdef SPLINE
    else {
        c3n_reverse ( C3_CURVE_NURB(curve), 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
    }
#endif /*SPLINE*/
    RETURN ( curve ) ;
}

