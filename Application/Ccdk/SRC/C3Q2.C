//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3Q2.C *********************************/
/******************** Routines for processing buffers **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2qdefs.h>
#include <c2rdefs.h>
#include <c3qdefs.h>
#include <c3rdefs.h>
#include <c3vdefs.h>
#include <c2qmcrs.h>
#include <c3qmcrs.h>
#include <c3vmcrs.h>
#include <c2mem.h>
#include <c3mem.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_PC_BUFFER c3q_conv_3d2d ( buffer_3d, c, u, v, buffer_2d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer_3d ;
PT3 c, u, v ;
C2_PC_BUFFER buffer_2d ;
{
    INT i ;

    buffer_2d = c2q_create ( buffer_2d, C3_PC_BUFFER_N(buffer_3d) ) ;
    C2_PC_BUFFER_N(buffer_2d) = C3_PC_BUFFER_N(buffer_3d) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer_3d)-1 ; i++ ) 
        c3r_conv_3d2d ( C3_PC_BUFFER_ARC(buffer_3d,i), c, u, v, 
            C2_PC_BUFFER_ARC(buffer_2d,i) ) ;
    RETURN ( buffer_2d ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_conv_2d3d ( buffer_2d, c, u, v, buffer_3d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_PC_BUFFER buffer_2d ;
PT3 c, u, v ;
C3_PC_BUFFER buffer_3d ;
{
    INT i ;

    if ( buffer_3d == NULL ) {
        buffer_3d = c3q_create ( buffer_3d, C2_PC_BUFFER_N(buffer_2d) ) ;
        if ( buffer_3d == NULL ) 
            RETURN ( NULL ) ;
    }
    else
        C3_PC_BUFFER_N(buffer_3d) = C2_PC_BUFFER_N(buffer_2d) ;

    for ( i=0 ; i<C2_PC_BUFFER_N(buffer_2d)-1 ; i++ ) 
        c3r_conv_2d3d ( C2_PC_BUFFER_ARC(buffer_2d,i), c, u, v, 
            C3_PC_BUFFER_ARC(buffer_3d,i) ) ;
    RETURN ( buffer_3d ) ;
}

