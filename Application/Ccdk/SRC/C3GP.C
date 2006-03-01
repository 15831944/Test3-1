//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3GP.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3rdefs.h>
#include <c3apriv.h>
#include <qglpriv.h>
#include <c3vmcrs.h>
#include <c3gmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3g_info ( arc, t0, t1, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
FILE *outfile ;
{
    PT3 ctr, pt ;
    REAL rad ;

    if ( C3V_IS_SMALL ( c3r_get_d_vec ( C3_ARC_SEG(arc) ) ) ) {
        qgl_print_string ( outfile, "The arc is degenerate" ) ;
    }
    else {
        c3r_ctr ( C3_ARC_SEG(arc), ctr ) ;
        rad = c3r_rad ( C3_ARC_SEG(arc) ) ;
        qgl_print_string ( outfile, "Center" ) ;
        qgl_print_tabs ( outfile, 5 ) ;
        c3a_print_pt ( outfile, ctr ) ;
        qgl_print_new_line ( outfile ) ;
        qgl_print_string ( outfile, "Radius" ) ;
        qgl_print_tabs ( outfile, 5 ) ;
        qgl_print_real ( outfile, rad ) ;
    }

    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Start point" ) ;
    qgl_print_tabs ( outfile, 4 ) ;
    c3g_pt_tan ( arc, t0, pt, NULL ) ;
    c3a_print_pt ( outfile, pt ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "End point" ) ;
    qgl_print_tabs ( outfile, 4 ) ;
    c3g_pt_tan ( arc, t1, pt, NULL ) ;
    c3a_print_pt ( outfile, pt ) ;
}

