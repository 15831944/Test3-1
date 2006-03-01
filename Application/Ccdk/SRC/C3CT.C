//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CT.C *********************************/ 
/**************** Two-dimensional curves ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3hdefs.h>
#include <c3ldefs.h>
#include <c3ndefs.h>
#include <c3pdefs.h>
#include <c3tdefs.h>
#include <c3adefs.h>
#include <c3cdefs.h>
#include <c3vdefs.h>
#include <c3vmcrs.h>
#include <c3mem.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_translate ( curve, shift ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 shift ;
{
    if ( C3_CURVE_IS_LINE(curve) ) {
        c3l_translate ( C3_CURVE_LINE(curve), shift, C3_CURVE_LINE(curve) ) ;
        c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        c3g_translate ( C3_CURVE_ARC(curve), shift ) ;
        c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
            C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        c3p_translate ( C3_CURVE_PCURVE(curve), shift ) ;
        c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#ifdef  SPLINE
    else {
        c3n_translate ( C3_CURVE_NURB(curve), shift ) ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#else
    else
        RETURN ( NULL ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_scale ( curve, origin, factor ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 origin ;
REAL factor ;
{
    PT3 a ;
    REAL t = 1.0 - factor ;

    C3V_SCALE ( origin, t, a ) ;

    if ( C3_CURVE_IS_LINE(curve) ) {
        c3l_scale ( C3_CURVE_LINE(curve), a, factor, C3_CURVE_LINE(curve) ) ;
        c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        c3g_scale ( C3_CURVE_ARC(curve), origin, factor ) ;
        c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
            C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        c3p_scale ( C3_CURVE_PCURVE(curve), origin, factor ) ;
        c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#ifdef  SPLINE
    else {
        c3n_scale ( C3_CURVE_NURB(curve), a, factor ) ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#else
    else
        RETURN ( NULL ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_mirror ( curve, n, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 n ;
REAL d ;
{
    if ( C3_CURVE_IS_LINE(curve) ) {
        c3l_mirror ( C3_CURVE_LINE(curve), n, d, C3_CURVE_LINE(curve) ) ;
        c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        c3g_mirror ( C3_CURVE_ARC(curve), n, d ) ;
        c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
            C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        c3p_mirror ( C3_CURVE_PCURVE(curve), n, d ) ;
        c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#ifdef  SPLINE
    else {
        c3n_mirror ( C3_CURVE_NURB(curve), n, d ) ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#else
    else
        RETURN ( NULL ) ;
#endif  /* SPLINE */
}

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_rotate ( curve, origin, axis, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 origin, axis ;
REAL angle ;
{
    REAL c, s ;

    c = cos ( angle ) ;
    s = sin ( angle ) ;
    RETURN ( c3c_rotate_cs ( curve, origin, axis, c, s ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_rotate_cs ( curve, origin, axis, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 origin, axis ;
REAL c, s ;
{
    PT3 nrmd_axis ;

    c3v_normalize ( axis, nrmd_axis ) ;
    if ( C3_CURVE_IS_LINE(curve) ) {
        c3l_rotate_cs ( C3_CURVE_LINE(curve), origin, nrmd_axis, 
            c, s, C3_CURVE_LINE(curve) ) ;
        c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        c3g_rotate_cs ( C3_CURVE_ARC(curve), origin, nrmd_axis, c, s ) ;
        c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
            C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        c3p_rotate_cs ( C3_CURVE_PCURVE(curve), origin, nrmd_axis, c, s ) ;
        c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#ifdef  SPLINE
    else {
        c3n_rotate_cs ( C3_CURVE_NURB(curve), origin, axis, c, s ) ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
#else
    else
        RETURN ( NULL ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_project_normal ( curve, normal, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 normal ;
REAL dist ;
{
    RETURN ( c3c_project_oblique ( curve, normal, dist, normal ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_project_oblique ( curve, normal, dist, direction ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 normal, direction ;
REAL dist ;
{
#ifdef  SPLINE
    C3_NURB nurb ;
#endif /*SPLINE*/
    if ( C3_CURVE_IS_LINE(curve) ) {
        if ( !c3l_project_oblique ( C3_CURVE_LINE(curve), 
            normal, dist, direction, C3_CURVE_LINE(curve) ) ) 
            RETURN ( NULL ) ;
        c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        if ( c3g_project_oblique ( C3_CURVE_ARC(curve), 
                normal, dist, direction ) ) {
            c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
                C3_CURVE_BOX(curve) ) ;
            RETURN ( curve ) ;
        }
#ifdef SPLINE
        else {
            nurb = c3n_arc ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve) ) ;
            if ( nurb == NULL ) 
                RETURN ( NULL ) ;
            if ( !c3n_project_oblique ( nurb, normal, 
                dist, direction ) ) {
                    c3n_free_nurb ( nurb ) ;
                    RETURN ( NULL ) ;
            }
            C3_FREE_ARC ( C3_CURVE_ARC(curve) ) ;
            C3_CURVE_NURB(curve) = nurb ;
            C3_CURVE_TYPE(curve) = C3_ELLIPSE_TYPE ;
            c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
            RETURN ( curve ) ;
        }
#else
        else
            RETURN ( NULL ) ;
#endif /*SPLINE*/
    }

    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        if ( c3p_project_oblique ( C3_CURVE_PCURVE(curve), normal, dist, 
            direction ) ) {
            c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
            RETURN ( curve ) ;
        }
#ifdef SPLINE
        else {
            nurb = c3n_pcurve ( C3_CURVE_PCURVE(curve) ) ;
            if ( nurb == NULL ) 
                RETURN ( NULL ) ;
            if ( !c3n_project_oblique ( nurb, normal, 
                dist, direction ) ) {
                    c3n_free_nurb ( nurb ) ;
                    RETURN ( NULL ) ;
            }
            c3p_free ( C3_CURVE_PCURVE(curve) ) ;
            C3_CURVE_TYPE(curve) = C3_ELLIPSE_TYPE ;
            C3_CURVE_NURB(curve) = nurb ;
            c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
            RETURN ( curve ) ;
        }
#else
        else
            RETURN ( NULL ) ;
#endif /*SPLINE*/
    }
#ifdef  SPLINE
    else {
        if ( !c3n_project_oblique ( C3_CURVE_NURB(curve), 
            normal, dist, direction ) )
                RETURN ( NULL ) ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
    }
#else
    else 
        RETURN ( NULL ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3c_transform ( curve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
C3_TRANSFORM t ;
{
#ifdef  SPLINE
    C3_NURB nurb ;
#endif /*SPLINE*/
    if ( C3_CURVE_IS_LINE(curve) ) 
      {
      c3l_transform ( C3_CURVE_LINE(curve), t, C3_CURVE_LINE(curve) ) ;
      c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
                C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
      RETURN ( curve ) ;
      }
    else if ( C3_CURVE_IS_ARC(curve) ) 
      {
      if ( c3g_transform ( C3_CURVE_ARC(curve), t ) ) 
        {
        c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
        }
#ifdef SPLINE
      else 
        {
        nurb = c3n_arc ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve) ) ;
        if ( nurb == NULL ) 
          RETURN ( NULL ) ;
        c3n_transform ( nurb, t ) ;
        C3_FREE_ARC ( C3_CURVE_ARC(curve) ) ;
        C3_CURVE_NURB(curve) = nurb ;
        C3_CURVE_TYPE(curve) = C3_ELLIPSE_TYPE ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
        C3_CURVE_J0(curve) = 2 ;
        C3_CURVE_J1(curve) = c3n_get_n ( C3_CURVE_NURB(curve) ) - 1 ;
        RETURN ( curve ) ;
        }
#else
      else 
        RETURN ( NULL ) ;
#endif /*SPLINE*/ 
      }
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
      {
      if ( c3p_transform ( C3_CURVE_PCURVE(curve), t ) ) 
        {
        c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
        }
#ifdef SPLINE
      else 
        {
        nurb = c3n_pcurve ( C3_CURVE_PCURVE(curve) ) ;
        if ( nurb == NULL ) 
          RETURN ( NULL ) ;
        c3n_transform ( nurb, t ) ;
        c3p_free ( C3_CURVE_PCURVE(curve) ) ;
        C3_CURVE_NURB(curve) = nurb ;
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
        RETURN ( curve ) ;
        }
#else
      else
        RETURN ( NULL ) ;
#endif /*SPLINE*/
      }
#ifdef  SPLINE
    else 
      {
      c3n_transform ( C3_CURVE_NURB(curve), t ) ;
      c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
      RETURN ( curve ) ;
      }
#else
    else 
      RETURN ( NULL ) ;
#endif  /* SPLINE */
}
