//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3QP.C *********************************/
/****************** Routines for processing arc geometry *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3qdefs.h>
#include <c3apriv.h>
#include <qglpriv.h>
#include <c3qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_info ( buffer, outfile ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
FILE *outfile ;
{
    INT i ;

    qgl_print_tab ( outfile ) ;
    qgl_print_string ( outfile, "Points:\t\t\t\tBulge:" ) ;
    qgl_print_new_line ( outfile ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) {
        c3a_print_pt ( outfile, C3_PC_BUFFER_PT(buffer,i) ) ;
        qgl_print_new_line ( outfile ) ;
        qgl_print_tab ( outfile ) ;
        c3a_print_pt ( outfile, C3_PC_BUFFER_D_VEC(buffer,i) ) ;
        qgl_print_new_line ( outfile ) ;
    }
    c3a_print_pt ( outfile, C3_PC_BUFFER_PT1(buffer) ) ;
    qgl_print_tabs ( outfile, 3 ) ;
}


