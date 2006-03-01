//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/********************************** C2LP.C *********************************/
/******************** Routines for processing lines ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ldefs.h>
#include <c2apriv.h>
#include <qglpriv.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c2l_info ( line, t0, t1, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_LINE line ;
FILE *outfile ;
REAL t0, t1 ;
{
    PT2 p ;
    qgl_print_string ( outfile, "First endpoint\t\t" ) ;
    c2l_pt_tan ( line, t0, p, NULL ) ;
    c2a_print_pt ( outfile, p ) ;
    qgl_print_new_line ( outfile ) ;
    qgl_print_string ( outfile, "Second endpoint\t\t" ) ;
    c2l_pt_tan ( line, t1, p, NULL ) ;
    c2a_print_pt ( outfile, p ) ;
}

