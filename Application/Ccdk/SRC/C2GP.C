//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/********************************** C2GP.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2gdefs.h>
#include <c2rdefs.h>
#include <c2apriv.h>
#include <c2gmcrs.h>
#include <qglpriv.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2g_info ( arc, t0, t1, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc ;
FILE *outfile ;
REAL t0, t1 ;
{
    PT2 ctr, pt ;
    REAL rad ;

    if ( IS_ZERO ( c2r_get_d ( C2_ARC_SEG(arc) ) ) ) {
        qgl_print_string ( outfile, "The arc is degenerate" ) ;
    }
    else {
        c2r_ctr ( C2_ARC_SEG(arc), ctr ) ;
        rad = C2_ARC_ZERO_RAD(arc) ? 0.0 : c2r_rad ( C2_ARC_SEG(arc) ) ;
        qgl_print_string ( outfile, "Center" ) ;
        qgl_print_tabs ( outfile, 3 ) ;
        c2a_print_pt ( outfile, ctr ) ;
        qgl_print_new_line ( outfile ) ;
        qgl_print_string ( outfile, "Radius" ) ;
        qgl_print_tabs ( outfile, 3 ) ;
        qgl_print_real ( outfile, rad ) ;
    }

    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Start point" ) ;
    qgl_print_tabs ( outfile, 2 ) ;
    c2g_pt_tan ( arc, t0, pt, NULL ) ;
    c2a_print_pt ( outfile, pt ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "End point" ) ;
    qgl_print_tabs ( outfile, 2 ) ;
    c2g_pt_tan ( arc, t1, pt, NULL ) ;
    c2a_print_pt ( outfile, pt ) ;
}

