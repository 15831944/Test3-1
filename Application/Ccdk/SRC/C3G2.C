//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3G2.C *********************************/
/******************** Routines for processing arcs ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2rdefs.h>
#include <c3gdefs.h>
#include <c3rdefs.h>
#include <c3vdefs.h>
#include <c2gmcrs.h>
#include <c3gmcrs.h>
#include <c3vmcrs.h>
#include <c2mem.h>
#include <c3mem.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ARC c3g_conv_3d2d ( arc_3d, c, u, v, arc_2d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc_3d ;
PT3 c, u, v ;
C2_ARC arc_2d ;
{
    if ( arc_2d == NULL ) 
       arc_2d = C2_ALLOC_ARC ;
    C2_ARC_ZERO_RAD(arc_2d) = C3_ARC_ZERO_RAD(arc_3d) ;
    if ( c3r_conv_3d2d ( C3_ARC_SEG(arc_3d), c, u, v, 
        C3_ARC_SEG(arc_2d) ) == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( arc_2d ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ARC c3g_conv_2d3d ( arc_2d, c, u, v, arc_3d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ARC arc_2d ;
PT3 c, u, v ;
C3_ARC arc_3d ;
{
    if ( arc_3d == NULL ) 
        arc_3d = C3_ALLOC_ARC ;
    C3_ARC_ZERO_RAD(arc_3d) = C2_ARC_ZERO_RAD(arc_2d) ;
    if ( c3r_conv_2d3d ( C2_ARC_SEG(arc_2d), c, u, v, 
        C2_ARC_SEG(arc_3d) ) == NULL ) 
        RETURN ( NULL ) ;
    RETURN ( arc_3d ) ;
}

