//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/******************************* C2CP.C *********************************/
/**************** Printing two-dimensional curves ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2apriv.h>
#include <c2cdefs.h>
#include <qglpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC void c2c_info_curve ( curve, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_CURVE curve ;
FILE *outfile ;
{
    qgl_print_new_line ( outfile ) ;
    if ( C2_CURVE_IS_LINE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 2d line\n" ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) {
        if ( C2_CURVE_T0(curve) <= BBS_ZERO && 
            C2_CURVE_T1(curve) >= 2.0 - BBS_ZERO )
            qgl_print_string ( outfile, "The curve is a 2d circle\n" ) ;
        else
            qgl_print_string ( outfile, "The curve is a 2d arc\n" ) ;
    }
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 2d pcurve\n" ) ;
#ifdef  SPLINE
    else if ( C2_CURVE_IS_SPLINE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 2d spline\n" ) ;
    else if ( C2_CURVE_IS_BEZIER(curve) ) 
        qgl_print_string ( outfile, "The curve is a 2d Bezier spline\n" ) ;
    else if ( C2_CURVE_IS_ELLIPSE(curve) ) 
        qgl_print_string ( outfile, "The curve is a 2d ellipse\n" ) ;
    else 
        qgl_print_string ( outfile, "The curve's type is unknown\n" ) ;
#endif  /*SPLINE*/

    qgl_print_string ( outfile, "The start parameter of the curve is" ) ;
    qgl_print_tab ( outfile ) ;
    qgl_print_real ( outfile, C2_CURVE_T0(curve) ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "The  end  parameter of the curve is" ) ;
    qgl_print_tab ( outfile ) ;
    qgl_print_real ( outfile, C2_CURVE_T1(curve) ) ;
    qgl_print_new_line ( outfile ) ;
    if ( C2_CURVE_IS_LINE(curve) ) 
        c2l_info ( C2_CURVE_LINE(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), outfile ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        c2g_info ( C2_CURVE_ARC(curve), C2_CURVE_T0(curve), 
            C2_CURVE_T1(curve), outfile ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        c2p_info ( C2_CURVE_PCURVE(curve), outfile ) ;
#ifdef  SPLINE
    else if ( C2_CURVE_IS_ELLIPSE(curve) ) 
        c2n_info_ellipse ( C2_CURVE_NURB(curve), outfile ) ;
    else if ( C2_CURVE_IS_SPLINE(curve) || C2_CURVE_IS_BEZIER(curve) ) 
        c2n_info ( C2_CURVE_NURB(curve), outfile ) ;
#endif  /*SPLINE*/
    qgl_print_new_line ( outfile ) ;
}    

/*----------------------------------------------------------------------*/
BBS_PUBLIC void c2c_info_int_rec ( int_rec, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C2_INT_REC int_rec ;
FILE *outfile ;
{
    qgl_print_new_line ( outfile ) ;
    if ( C2_INT_REC_TYPE(int_rec) < 0 ) 
        qgl_print_string ( outfile, "Coincident segments\n\n" ) ;
    else {
        qgl_print_string ( outfile, "t1 = " ) ;
        qgl_print_real ( outfile, C2_INT_REC_T1(int_rec) ) ;
        qgl_print_tab ( outfile ) ;
        qgl_print_string ( outfile, "j1 = " ) ;
        qgl_print_int ( outfile, C2_INT_REC_J1(int_rec) ) ;
        qgl_print_tab ( outfile ) ;
        qgl_print_string ( outfile, "t2 = " ) ;
        qgl_print_real ( outfile, C2_INT_REC_T2(int_rec) ) ;
        qgl_print_tab ( outfile ) ;
        qgl_print_string ( outfile, "j2 = " ) ;
        qgl_print_int ( outfile, C2_INT_REC_J2(int_rec) ) ;
        qgl_print_new_line ( outfile ) ;
        qgl_print_string ( outfile, "pt\t" ) ;
        c2a_print_pt ( outfile, C2_INT_REC_PT(int_rec) ) ;
        qgl_print_string ( outfile, "\t, dist = \t" ) ;
        qgl_print_real ( outfile, C2_INT_REC_DIST(int_rec) ) ;

        qgl_print_new_line ( outfile ) ;
        if ( C2_INT_REC_TRANS(int_rec) )
            qgl_print_string ( outfile, "Transversal intersection\n" ) ;
        if ( C2_INT_REC_TANGENT(int_rec) )
            qgl_print_string ( outfile, "Tangential intersection\n" ) ;
    }
}    


