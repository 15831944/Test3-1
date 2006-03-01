//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/******************************* C2SF.C *********************************/
/**************** Printing two-dimensional splines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef SPLINE
#include <c2sdefs.h>
#include <c2apriv.h>
#include <qglpriv.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE void c2s_info ( a, n, d, knot, w, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
INT n, d ;
REAL *knot ;
REAL w ;
FILE *outfile ;
{
    INT i ;
    qgl_print_string ( outfile, "The number of control points is\t\t" ) ;
    qgl_print_int ( outfile, n ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "The order of the spline is\t\t" ) ;
    qgl_print_int ( outfile, d ) ;
    qgl_print_new_line ( outfile ) ;
    if ( w > BBS_ZERO ) {
        qgl_print_string ( outfile, "Offset of the spline is\t\t" ) ;
        qgl_print_tabs ( outfile, 3 ) ;
        qgl_print_real ( outfile, w ) ;
        qgl_print_new_line ( outfile ) ;
    }
    qgl_print_tab ( outfile ) ;
    qgl_print_string ( outfile, "Control points:\t\t" ) ;
    qgl_print_new_line ( outfile ) ;
    for ( i=0 ; i<n ; i++ ) {
        c2a_print_hpt ( outfile, a[i] ) ;
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
BBS_PRIVATE void c2s_info_ellipse ( a, w, outfile ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT2 *a ;
REAL w ;
FILE *outfile ;
{
    REAL major_axis, minor_axis, angle ;
    PT2 ctr ;

    if ( !c2s_get_ellipse_data ( a, ctr, &major_axis, &minor_axis, &angle ) ) {
        qgl_print_string ( outfile, "Computation failed\n" ) ;
        RETURN ;
    }
    qgl_print_string ( outfile, "Center" ) ;
    qgl_print_tabs ( outfile, 3 ) ;
    c2a_print_pt ( outfile, ctr ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Major axis" ) ;
    qgl_print_tabs ( outfile, 4 ) ;
    qgl_print_real ( outfile, major_axis ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Minor axis" ) ;
    qgl_print_tabs ( outfile, 4 ) ;
    qgl_print_real ( outfile, minor_axis ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Angle" ) ;
    qgl_print_tabs ( outfile, 5 ) ;
    qgl_print_real ( outfile, angle ) ;
    if ( w > BBS_ZERO ) {
        qgl_print_string ( outfile, "Offset of the spline is\t\t" ) ;
        qgl_print_real ( outfile, w ) ;
        qgl_print_tabs ( outfile, 3 ) ;
        qgl_print_new_line ( outfile ) ;
    }
}
#endif /*SPLINE*/

