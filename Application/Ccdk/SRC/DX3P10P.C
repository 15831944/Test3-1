//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/****************************** DX3P10P.C *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dxents.h>
#include <dx3defs.h>
#include <dx3priv.h>
#ifndef __ACAD11__
#include <c3pdefs.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx3_pcurve_to_entity ( pcurve, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
{ 
    DXF_ENTITY pline ;
    BOOLEAN planar, closed ;
    DXF_ENTITY vertex ;
    PT3 p, x, y, extrusion ;
    REAL elevation ;
    DML_ITEM item ;

    planar = ( c3p_plane ( pcurve, extrusion, &elevation ) == C3_PLANE_DET ) ;
    if ( !planar ) {
        if ( !dx3_pc_buffer_planar ( C3_PCURVE_BUFFER(pcurve) ) )
            RETURN ( NULL ) ;
        C3_WALK_PCURVE ( pcurve, item ) {
            if ( !dx3_pc_buffer_planar ( DML_RECORD(item) ) )
                RETURN ( NULL ) ;
        }
    }
    else 
        dx3_arb_axis ( extrusion, x, y ) ;

    if ( t0 > BBS_ZERO || t1 < (REAL) c3p_n(pcurve) - 1.0 - BBS_ZERO ) {
        C3_PCURVE tcurve = c3p_trim ( pcurve, t0, t1 ) ;
        pline = dx3_pcurve_to_entity ( tcurve, 
            0.0, (REAL) c3p_n(tcurve) - 1.0 ) ;
        c3p_free ( tcurve ) ;
        RETURN ( pline ) ;
    }
    pline = dx0_alloc_entity ( DXF_POLYLINE ) ;
    if ( pline == NULL ) 
        RETURN ( NULL ) ;

    closed = c3p_closed ( pcurve ) ;
    if ( closed ) 
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_CLSD ;

    if ( planar ) {
        DXF_ENTITY_EXTRUSION(pline)[0] = extrusion[0] ; 
        DXF_ENTITY_EXTRUSION(pline)[1] = extrusion[1] ; 
        DXF_ENTITY_EXTRUSION(pline)[2] = extrusion[2] ; 
        DXF_PLINE_ORIGIN(pline)[2] = elevation ;
    }
    else
        DXF_PLINE_FLAGS(pline) |= DXF_PLINE_3D ;

    DXF_PLINE_VLIST(pline) = dml_create_list();
    if ( DXF_PLINE_VLIST(pline) == NULL )
        RETURN ( NULL ) ;
    dx3_pc_buffer_to_entity ( C3_PCURVE_BUFFER(pcurve), pline, planar, 
        x, y, extrusion ) ;
    C3_WALK_PCURVE ( pcurve, item ) 
        dx3_pc_buffer_to_entity ( DML_RECORD(item), pline, planar, 
            x, y, extrusion ) ;

    if ( !closed && c3p_ept1 ( pcurve, p ) ) {
        vertex = dx3_vertex_to_entity ( p, 0.0, 0, planar, x, y, extrusion ) ;
        if ( vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( pline ) ;
}
#endif

