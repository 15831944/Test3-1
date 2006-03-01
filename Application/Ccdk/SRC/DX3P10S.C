//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* DX3P.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alsdefs.h>
#include <c3ndefs.h>
#include <c3sdefs.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx3defs.h>
#include <dx3priv.h>
#include <dx0defs.h>
#include <c3nmcrs.h>
#include <c3vmcrs.h>
#ifndef __ACAD11__

#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx3_spline_to_entity ( spline, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB spline ;
PARM parm0, parm1 ;
{
    DXF_ENTITY pline ;
    INT i, m, n ;
    DXF_ENTITY vertex ;
    BOOLEAN planar ;
    PT3 pt, x, y, extrusion ;
    REAL *knot, h, elevation ;
    PARM_S parm ;

    if ( ( parm0 != NULL && 
        PARM_T(parm0) > C3_NURB_KNOT(spline)[C3_NURB_D(spline)-1]+BBS_ZERO ) 
        || ( parm1 != NULL && 
        PARM_T(parm1) < C3_NURB_KNOT(spline)[C3_NURB_N(spline)]-BBS_ZERO ) ) {

        C3_NURB tspline = c3n_trim ( spline, parm0, parm1 ) ;
        pline = dx3_spline_to_entity ( tspline, NULL, NULL ) ;
        c3n_free_nurb ( tspline ) ;
        RETURN ( pline ) ;
    }

    planar = ( c3s_plane ( C3_NURB_CTPT(spline), C3_NURB_N(spline), 
        extrusion, &elevation ) == C3_PLANE_DET ) ;
    if ( planar ) 
        dx3_arb_axis ( extrusion, x, y ) ;

    pline = dx0_alloc_entity ( DXF_POLYLINE ) ;
    if ( pline == NULL ) 
        RETURN ( NULL ) ;

    if ( C3_NURB_D(spline) == 3 ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_SPL_FIT ;
        DXF_PLINE_TYPE(pline) = DXF_PLINE_QUADR ;
    }
    else if ( C3_NURB_D(spline) == 4 ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_SPL_FIT ;
        DXF_PLINE_TYPE(pline) = DXF_PLINE_CUBIC ;
    }
    else if ( C3_NURB_D(spline) != 2 ) 
        RETURN ( NULL ) ;

    n = C3_NURB_N(spline) ;
    knot = MALLOC ( n + C3_NURB_D(spline), REAL) ;
    als_clsc_uni_knots ( n, C3_NURB_D(spline), knot ) ;

    if ( c3n_closed ( spline ) ) {
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_CLSD ;
        n-- ;
    }

    if ( planar ) {
        C3V_COPY ( extrusion, DXF_ENTITY_EXTRUSION(pline) ) ;
        DXF_PLINE_ORIGIN(pline)[2] = elevation ;
    }
    else
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_3D ;

    DXF_PLINE_VLIST(pline) = dml_create_list();
    if ( DXF_PLINE_VLIST(pline) == NULL )
        RETURN ( NULL ) ;

    vertex = dx3_vertex_to_entity ( C3_NURB_CTPT(spline)[0], 0.0, 
        DXF_VERTEX_SPL_FR, planar, x, y, extrusion ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
        RETURN ( NULL ) ;

    h = 1.0 / (REAL)DXF_SPLINESEGS ;
    m = DXF_SPLINESEGS * ( n - C3_NURB_D(spline) + 1 ) ;
    PARM_J(&parm) = 0 ;

    for ( i=0, PARM_T(&parm) = 0.0 ; i <= m ; i++, PARM_T(&parm) += h ) {
        c3s_eval ( C3_NURB_CTPT(spline), n, C3_NURB_D(spline), knot, 
            C3_NURB_W(spline), C3_NURB_NORMAL(spline), &parm, 0, (PT3*)pt ) ;
        vertex = dx3_vertex_to_entity ( pt, 0.0, DXF_VERTEX_SPL_FIT, 
            planar, x, y, extrusion ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }

    for ( i=1 ; i<n ; i++ ) {
        vertex = dx3_vertex_to_entity ( C3_NURB_CTPT(spline)[i], 0.0, 
            DXF_VERTEX_SPL_FR, planar, x, y, extrusion ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    FREE ( knot ) ;
    RETURN ( pline ) ;
}
#endif /*SPLINE*/

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx3_vertex_to_entity ( pt, bulge, flags, 
    planar, x, y, extrusion ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
REAL bulge ;
INT flags ;
BOOLEAN planar ;
PT3 x, y, extrusion ;
{ 
    DXF_ENTITY vertex ;

    vertex = dx0_alloc_entity ( DXF_VERTEX ) ;
    if ( vertex == NULL ) 
        RETURN ( NULL ) ;
    DXF_VERTEX_FLAGS(vertex) = flags ;
    if ( planar ) {
        dx3_wcs_to_ecs ( pt, x, y, extrusion, DXF_VERTEX_PT(vertex) ) ;
        DXF_VERTEX_BULGE(vertex) = bulge ;
    }
    else {
        C3V_COPY ( pt, DXF_VERTEX_PT(vertex) ) ;
        DXF_VERTEX_FLAGS(vertex) |= DXF_VERTEX_3D_PLINE ;
    }
    RETURN ( vertex ) ;
}
#endif

