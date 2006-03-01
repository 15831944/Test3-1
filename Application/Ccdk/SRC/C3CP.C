//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CP.C *********************************/
/**************** Three-dimensional curves ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3pdefs.h>
#include <c3ndefs.h>
#include <c3cdefs.h>
#include <qglpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC void c3c_info_curve ( curve, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
FILE *outfile ;
{
    qgl_print_new_line ( outfile ) ;
    if ( C3_CURVE_IS_LINE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 3d line\n" ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) {
        if ( C3_CURVE_T0(curve)<=BBS_ZERO && 
             C3_CURVE_T1(curve)>=2.0-BBS_ZERO )
            qgl_print_string ( outfile, "The curve is a 3d circle\n" ) ;
        else
            qgl_print_string ( outfile, "The curve is a 3d arc\n" ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 3d pcurve\n" ) ;
#ifdef  SPLINE
    else if ( C3_CURVE_IS_SPLINE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 3d spline\n" ) ;
    else if ( C3_CURVE_IS_BEZIER(curve) ) 
        qgl_print_string ( outfile, "The curve is a 3d Bezier spline\n" ) ;
    else if ( C3_CURVE_IS_ELLIPSE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 3d ellipse\n" ) ;
    else 
        qgl_print_string ( outfile, "The curve's type is unknown\n" ) ;
#endif  /*SPLINE*/

    qgl_print_string ( outfile, "The start parameter of the curve is" ) ;
    qgl_print_tab ( outfile ) ;
    qgl_print_real ( outfile, C3_CURVE_T0(curve) ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "The  end  parameter of the curve is" ) ;
    qgl_print_tab ( outfile ) ;
    qgl_print_real ( outfile, C3_CURVE_T1(curve) ) ;
    qgl_print_new_line ( outfile ) ;
    if ( C3_CURVE_IS_LINE(curve) ) 
        c3l_info ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), outfile ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        c3g_info ( C3_CURVE_ARC(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), outfile ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        c3p_info ( C3_CURVE_PCURVE(curve), outfile ) ;
#ifdef  SPLINE
    else if ( C3_CURVE_IS_ELLIPSE(curve) ) 
        c3n_info_ellipse ( C3_CURVE_NURB(curve), outfile ) ;
    else if ( C3_CURVE_IS_SPLINE(curve) || C3_CURVE_IS_BEZIER(curve) ) 
        c3n_info ( C3_CURVE_NURB(curve), outfile ) ;
#endif  /*SPLINE*/
    qgl_print_new_line ( outfile ) ;
}    

