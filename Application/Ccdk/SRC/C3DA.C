//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3DA.C *********************************/
/********** Three-dimensional curve construction routines ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3adefs.h>
#include <c3dpriv.h>
#include <c3ddefs.h>
#include <c3vmcrs.h>
#include <c3mem.h>

STATIC C3_CURVE c3d_arc0 __(( void )) ;
STATIC C3_CURVE c3d_arc1 __(( C3_CURVE )) ;
/*----------------------------------------------------------------------*/
STATIC C3_CURVE c3d_arc0 __(( void )) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    C3_CURVE curve = c3d_curve() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_TYPE(curve) = C3_ARC_TYPE ;
    C3_CURVE_ARC(curve) = C3_ALLOC_ARC ;
    if ( C3_CURVE_ARC(curve) == NULL ) {
        C3_FREE_CURVE ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
STATIC C3_CURVE c3d_arc1 ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    C3_CURVE_T0(curve) = 0.0 ;
    C3_CURVE_T1(curve) = 1.0 ;
    C3_CURVE_J0(curve) = 0 ;
    C3_CURVE_J1(curve) = 0 ;
    c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
        C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_arc ( pt0, pt1, d_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, d_vec ;
{
    C3_CURVE curve = c3d_arc0() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( c3g_epts_d ( pt0, pt1, d_vec, C3_CURVE_ARC(curve) ) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( c3d_arc1 ( curve ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_arc_3pts ( pt1, pt2, pt3 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt1, pt2, pt3 ;
{
    C3_CURVE curve = c3d_arc0() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( c3g_3pts ( pt1, pt2, pt3, C3_CURVE_ARC(curve) ) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( c3d_arc1 ( curve ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_arc_2pts_tan ( pt0, tan0, pt1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, tan0, pt1 ;
{
    C3_CURVE curve = c3d_arc0() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( c3g_2pts_tan ( pt0, tan0, pt1, C3_CURVE_ARC(curve) ) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( c3d_arc1 ( curve ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_arc_2pts_ctr ( pt0, pt1, ctr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, ctr ;
{
    C3_CURVE curve = c3d_arc0() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( c3g_2pts_ctr ( pt0, pt1, ctr, C3_CURVE_ARC(curve) ) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( c3d_arc1 ( curve ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_circle ( ctr, rad, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 ctr, normal ;
REAL rad ;
{
    C3_CURVE curve = c3d_arc0() ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( c3g_circle ( ctr, rad, normal, C3_CURVE_ARC(curve) ) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    C3_CURVE_T0(curve) = 0.0 ;
    C3_CURVE_J0(curve) = 0 ;
    C3_CURVE_T1(curve) = 2.0 ;
    C3_CURVE_J1(curve) = 1 ;
    c3a_box_circle ( C3_CURVE_BOX(curve), ctr, rad, normal ) ;
    RETURN ( curve ) ;
}

