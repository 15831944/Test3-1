//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3LP.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3ldefs.h>
#include <c3apriv.h>
#include <qglpriv.h>

/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c3l_info ( line, t0, t1, outfile ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_LINE line ; 
REAL t0, t1 ;
FILE *outfile ; 
{ 
    PT3 p ;
    qgl_print_string ( outfile, "First endpoint" ) ;
    qgl_print_tab ( outfile ) ;
    c3l_eval ( line, t0, 0, (PT3*)p ) ; 
    c3a_print_pt ( outfile, p ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Second endpoint" ) ;
    qgl_print_tab ( outfile ) ;
    c3l_eval ( line, t1, 0, (PT3*)p ) ; 
    c3a_print_pt ( outfile, p ) ;
    qgl_print_new_line ( outfile ) ;
}

