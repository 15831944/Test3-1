//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3D2.C *********************************/
/********* Three-dimensional curve construction routines ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2ndefs.h>
#include <c2pdefs.h>
#include <c2adefs.h>
#include <c2ddefs.h>
#include <c2dpriv.h>
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3ndefs.h>
#include <c3pdefs.h>
#include <c3adefs.h>
#include <c3dpriv.h>
#include <c3ddefs.h>
#include <c3vmcrs.h>

STATIC C2_CURVE_TYPE_DEF c3d_type_3d2d __(( C3_CURVE_TYPE_DEF type )) ;
STATIC C3_CURVE_TYPE_DEF c3d_type_2d3d __(( C2_CURVE_TYPE_DEF type )) ;

/*----------------------------------------------------------------------*/
STATIC C2_CURVE_TYPE_DEF c3d_type_3d2d ( type ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE_TYPE_DEF type ;
{
    if ( type == C3_LINE_TYPE )
        RETURN ( C2_LINE_TYPE ) ;
    else if ( type == C3_ARC_TYPE )
        RETURN ( C2_ARC_TYPE ) ;
    else if ( type == C3_PCURVE_TYPE )
        RETURN ( C2_PCURVE_TYPE ) ;
    else if ( type == C3_ELLIPSE_TYPE )
        RETURN ( C2_ELLIPSE_TYPE ) ;
    else if ( type == C3_NURB_TYPE )
        RETURN ( C2_NURB_TYPE ) ;
    else if ( type == C3_BEZIER_TYPE )
        RETURN ( C2_BEZIER_TYPE ) ;
    else 
        RETURN ( C2_UNKNOWN_TYPE ) ;
}

/*----------------------------------------------------------------------*/
STATIC C3_CURVE_TYPE_DEF c3d_type_2d3d ( type ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE_TYPE_DEF type ;
{
    if ( type == C2_LINE_TYPE )
        RETURN ( C3_LINE_TYPE ) ;
    else if ( type == C2_ARC_TYPE )
        RETURN ( C3_ARC_TYPE ) ;
    else if ( type == C2_PCURVE_TYPE )
        RETURN ( C3_PCURVE_TYPE ) ;
    else if ( type == C2_ELLIPSE_TYPE )
        RETURN ( C3_ELLIPSE_TYPE ) ;
    else if ( type == C2_NURB_TYPE )
        RETURN ( C3_NURB_TYPE ) ;
    else if ( type == C2_BEZIER_TYPE )
        RETURN ( C3_BEZIER_TYPE ) ;
    else 
        RETURN ( C3_UNKNOWN_TYPE ) ;
}

/*----------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE c3d_convert_3d2d ( curve_3d, origin, x_axis, y_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve_3d ;
PT3 origin, x_axis, y_axis ;
{
    C2_CURVE curve_2d ;

    curve_2d = c2d_curve () ;
    if ( curve_2d == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(curve_2d) = c3d_type_3d2d ( C3_CURVE_TYPE(curve_3d) ) ;
    C2_CURVE_T0(curve_2d) = C3_CURVE_T0(curve_3d) ;
    C2_CURVE_J0(curve_2d) = C3_CURVE_J0(curve_3d) ;
    C2_CURVE_T1(curve_2d) = C3_CURVE_T1(curve_3d) ;
    C2_CURVE_J1(curve_2d) = C3_CURVE_J1(curve_3d) ;

    if ( C3_CURVE_IS_LINE(curve_3d) ) {
        C2_CURVE_LINE(curve_2d) = c3l_conv_3d2d ( C3_CURVE_LINE(curve_3d), 
            origin, x_axis, y_axis, NULL ) ;
        if ( C2_CURVE_LINE(curve_2d) != NULL ) 
            c2l_box ( C2_CURVE_LINE(curve_2d), C2_CURVE_T0(curve_3d),
                C2_CURVE_T1(curve_3d), C2_CURVE_BOX(curve_2d) ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve_3d) ) {
        C2_CURVE_ARC(curve_2d) = c3g_conv_3d2d ( C3_CURVE_ARC(curve_3d), 
            origin, x_axis, y_axis, NULL ) ;
        if ( C2_CURVE_ARC(curve_2d) != NULL ) 
            c2g_box ( C2_CURVE_ARC(curve_2d), C2_CURVE_T0(curve_3d),
                C2_CURVE_T1(curve_3d), C2_CURVE_BOX(curve_2d) ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve_3d) ) {
        C2_CURVE_PCURVE(curve_2d) = c3p_conv_3d2d ( C3_CURVE_PCURVE(curve_3d), 
            origin, x_axis, y_axis, NULL ) ;
        if ( C2_CURVE_PCURVE(curve_2d) != NULL ) 
            c2p_box ( C2_CURVE_PCURVE(curve_2d), C2_CURVE_BOX(curve_2d) ) ;
    }
#ifdef  SPLINE
    else if ( C3_CURVE_IS_SPLINE(curve_3d) || C3_CURVE_IS_ELLIPSE(curve_3d) 
        || C3_CURVE_IS_BEZIER(curve_3d) ) {
        C2_CURVE_NURB(curve_2d) = c3n_convert_3d2d ( C3_CURVE_NURB(curve_3d), 
            origin, x_axis, y_axis ) ;
        if ( C2_CURVE_NURB(curve_2d) != NULL ) 
            c2n_box ( C2_CURVE_NURB(curve_2d), C2_CURVE_BOX(curve_2d) ) ;
    }
#endif  /* SPLINE */
    else
        curve_2d = NULL ;
    if ( curve_2d != NULL && C2_CURVE_LINE(curve_2d) == NULL ) {
        c2d_free_curve ( curve_2d ) ;
        curve_2d = NULL ;
    }
    RETURN ( curve_2d ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_convert_2d3d ( curve_2d, origin, x_axis, y_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve_2d ;
PT3 origin, x_axis, y_axis ;
{
    C3_CURVE curve_3d ;

    curve_3d = c3d_curve () ;
    if ( curve_3d == NULL ) 
        RETURN ( NULL ) ;
    C2_CURVE_TYPE(curve_3d) = c3d_type_2d3d ( C3_CURVE_TYPE(curve_2d) ) ;
    C3_CURVE_T0(curve_3d) = C2_CURVE_T0(curve_2d) ;
    C3_CURVE_J0(curve_3d) = C2_CURVE_J0(curve_2d) ;
    C3_CURVE_T1(curve_3d) = C2_CURVE_T1(curve_2d) ;
    C3_CURVE_J1(curve_3d) = C2_CURVE_J1(curve_2d) ;

    if ( C2_CURVE_IS_LINE(curve_2d) ) {
        C3_CURVE_LINE(curve_3d) = c3l_conv_2d3d ( C2_CURVE_LINE(curve_2d), 
            origin, x_axis, y_axis, NULL ) ;
        if ( C3_CURVE_LINE(curve_3d) != NULL )
            c3l_box ( C3_CURVE_LINE(curve_3d), C2_CURVE_T0(curve_3d),
                C2_CURVE_T1(curve_3d), C3_CURVE_BOX(curve_3d) ) ;
    }
    else if ( C2_CURVE_IS_ARC(curve_2d) ) {
        C3_CURVE_ARC(curve_3d) = c3g_conv_2d3d ( C2_CURVE_ARC(curve_2d), 
            origin, x_axis, y_axis, NULL ) ;
        if ( C3_CURVE_ARC(curve_3d) != NULL ) 
            c3g_box ( C3_CURVE_ARC(curve_3d), C3_CURVE_IS_EXT_ARC(curve_3d),
                C3_CURVE_BOX(curve_3d) ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve_2d) ) {
        C3_CURVE_PCURVE(curve_3d) = c3p_conv_2d3d ( C2_CURVE_PCURVE(curve_2d), 
            origin, x_axis, y_axis, NULL ) ;
        if ( C3_CURVE_PCURVE(curve_3d) != NULL ) 
            c3p_box ( C3_CURVE_PCURVE(curve_3d), C3_CURVE_BOX(curve_3d) ) ;
    }
#ifdef  SPLINE
    else if ( C2_CURVE_IS_SPLINE(curve_2d) || C2_CURVE_IS_ELLIPSE(curve_2d) 
        || C2_CURVE_IS_BEZIER(curve_2d) ) {
        C3_CURVE_NURB(curve_3d) = c3n_convert_2d3d ( C2_CURVE_NURB(curve_2d), 
            origin, x_axis, y_axis ) ;
        if ( C3_CURVE_NURB(curve_3d) != NULL ) 
            c3n_box ( C3_CURVE_NURB(curve_3d), C3_CURVE_BOX(curve_3d) ) ;
    }
#endif  /* SPLINE */
    else
        curve_3d = NULL ;
    if ( curve_3d != NULL && C3_CURVE_LINE(curve_3d) == NULL ) {
        c3d_free_curve ( curve_3d ) ;
        curve_3d = NULL ;
    }
    RETURN ( curve_3d ) ;
}

