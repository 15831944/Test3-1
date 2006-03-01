//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* DX3G2.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3adefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>
#include <c3ndefs.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx3defs.h>
#include <dx3priv.h>
#include <c3nmcrs.h>
#include <c3vmcrs.h>

#ifdef SPLINE
STATIC C3_CURVE dx3_pline_to_spline __(( DXF_ENTITY )) ;
STATIC void dx3_pline_to_nurb __(( DXF_ENTITY, C3_NURB, BOOLEAN )) ;
#endif /*SPLINE*/
/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE dx3_pline_to_curve ( pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
{
    if ( DXF_PLINE_TYPE(pline) & DXF_PLINE_3D_MESH )
        RETURN ( NULL ) ;
    if ( DXF_PLINE_TYPE(pline) == DXF_PLINE_NSMOOTH ) 
        RETURN ( dx3_pline_to_pcurve ( pline ) ) ;
    else
#ifdef SPLINE
        RETURN ( dx3_pline_to_spline ( pline ) ) ;
#else
        RETURN ( NULL ) ;
#endif /*SPLINE*/
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx3_pcurve_pt ( pline, vtx, x, y, pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline, vtx ;
PT3 x, y, pt ;
{
    if ( DXF_PLINE_FLAGS(pline)&DXF_PLINE_3D ) {
        C3V_COPY ( DXF_VERTEX_PT(vtx), pt ) ;
    }
    else 
        dx3_ecs_to_wcs ( DXF_VERTEX_PT(vtx), x, y, 
            DXF_ENTITY_EXTRUSION(pline), pt ) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
STATIC void dx3_pline_to_nurb ( pline, nurb, closed ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
C3_NURB nurb ;
BOOLEAN closed ;
{
    DML_ITEM item ;
    INT n ;
    DXF_ENTITY vertex ;
    PT3 x, y ;

    n = 0 ;
    DML_WALK_LIST ( DXF_PLINE_VLIST(pline), item ) {
        vertex = (DXF_ENTITY)DML_RECORD(item) ;
        if ( DXF_VERTEX_FLAGS(vertex) & DXF_VERTEX_SPL_FR ) {
            C3_NURB_CTPT(nurb)[n][3] = 1.0 ;
            dx3_pcurve_pt ( pline, vertex, x, y, C3_NURB_CTPT(nurb)[n] ) ;
                n++ ;
        }
    }

    if ( closed ) {
        dx3_pcurve_pt ( pline, vertex, x, y, C3_NURB_CTPT(nurb)[n] ) ;
        C3_NURB_CTPT(nurb)[n][3] = 1.0 ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_pline_to_spline ( pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
{
    BOOLEAN closed ;
    C3_CURVE spline ;
    DML_ITEM item ;
    INT d, n ;
    DXF_ENTITY vertex ;
    PT3 x, y ;

    if ( !(DXF_PLINE_FLAGS(pline)&DXF_PLINE_3D) ) 
        dx3_arb_axis ( DXF_ENTITY_EXTRUSION(pline), x, y ) ;

    closed = ( DXF_PLINE_FLAGS(pline) & 1 ) ;
    if ( DXF_PLINE_TYPE(pline) == DXF_PLINE_QUADR ) 
        d = 3 ;
    else if ( DXF_PLINE_TYPE(pline) == DXF_PLINE_CUBIC ) 
        d = 4 ;
    else 
        RETURN ( NULL ) ;

    n = closed ? 1 : 0 ;
    DML_WALK_LIST ( DXF_PLINE_VLIST(pline), item ) {
        vertex = (DXF_ENTITY)DML_RECORD(item) ;
        if ( DXF_VERTEX_FLAGS(vertex) & DXF_VERTEX_SPL_FR ) 
            n++ ;
    }

    //KGA 28/4/97 : call to c3d_spline_frame() fails if n==0 !!!
    if (n==0)
      RETURN NULL;
    //end of KGA change

    spline = c3d_spline_frame ( n, d, C3_CLSC_UNI ) ;
    if ( spline == NULL ) 
        RETURN ( NULL ) ;

    dx3_pline_to_nurb ( pline, C3_CURVE_NURB(spline), closed ) ;
    c3n_box ( C3_CURVE_NURB(spline), C3_CURVE_BOX(spline) ) ;
    RETURN ( spline ) ;
}
#endif /*SPLINE*/

