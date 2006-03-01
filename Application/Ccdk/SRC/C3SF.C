//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3SF.C *********************************/
/**************** Printing two-dimensional splines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef SPLINE
#include <c3sdefs.h>
#include <c3apriv.h>
#include <qglpriv.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3s_info ( a, n, d, knot, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
INT n, d ;
REAL *knot ;
FILE *outfile ;
{
    INT i ;
    qgl_print_string ( outfile, "The number of control points is\t\t" ) ;
    qgl_print_int ( outfile, n ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "The order of the spline is\t\t" ) ;
    qgl_print_int ( outfile, d ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_tab ( outfile ) ;
    qgl_print_string ( outfile, "Control points:\t\t" ) ;
    qgl_print_new_line ( outfile ) ;
    for ( i=0 ; i<n ; i++ ) {
        c3a_print_hpt ( outfile, a[i] ) ;
        qgl_print_new_line ( outfile ) ;
    }
    qgl_print_tab ( outfile ) ;
    qgl_print_string ( outfile, "Knots:\t\t" ) ;
    qgl_print_new_line ( outfile ) ;
    for ( i=0 ; i<n+d ; i++ ) {
        qgl_print_real ( outfile, knot[i] ) ;
        qgl_print_new_line ( outfile ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3s_info_ellipse ( a, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a ;
FILE *outfile ;
{
    PT3 major_axis, minor_axis, ctr ;

    if ( !c3s_get_ellipse_data ( a, ctr, major_axis, minor_axis ) ) {
        qgl_print_string ( outfile, "Computation failed\n" ) ;
        RETURN ;
    }
    qgl_print_string ( outfile, "Center" ) ;
    qgl_print_tabs ( outfile, 3 ) ;
    c3a_print_pt ( outfile, ctr ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Major axis" ) ;
    qgl_print_tabs ( outfile, 4 ) ;
    c3a_print_pt ( outfile, major_axis ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Minor axis" ) ;
    qgl_print_tabs ( outfile, 4 ) ;
    c3a_print_pt ( outfile, minor_axis ) ;
    qgl_print_new_line ( outfile ) ;
}
#endif /*SPLINE*/


