//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* DX3G3.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3pdefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx3defs.h>
#include <dx3priv.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE dx3_pline_to_pcurve ( pline ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
{
    BOOLEAN closed ;
    C3_CURVE curve ;
    C3_PCURVE pcurve ;
    DML_ITEM item ;
    INT n ;
    DXF_ENTITY last_vtx ;
    C3_PC_BUFFER buffer ;
    PT3 x, y ;

    if ( !(DXF_PLINE_FLAGS(pline)&DXF_PLINE_3D) ) 
        dx3_arb_axis ( DXF_ENTITY_EXTRUSION(pline), x, y ) ;

    closed = ( DXF_PLINE_FLAGS(pline) & 1 ) ;
    n = DML_LENGTH ( DXF_PLINE_VLIST(pline) ) ; 
    if ( closed ) {
        n++ ;
        last_vtx = (DXF_ENTITY)dml_first_record ( DXF_PLINE_VLIST(pline) ) ;
    }
    else
        last_vtx = (DXF_ENTITY)dml_last_record ( DXF_PLINE_VLIST(pline) ) ;

    curve = c3d_pcurve_frame ( 0, 0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_T0(curve) = 0.0 ;
    C3_CURVE_J0(curve) = 0 ;
    C3_CURVE_T1(curve) = (REAL)(n-1) ;
    C3_CURVE_J1(curve) = n-2 ;
    pcurve = C3_CURVE_PCURVE(curve) ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    item = DML_FIRST ( DXF_PLINE_VLIST(pline) ) ;
    n-- ;
    dx3_pline_to_pc_buffer ( pline, &item, &n, x, y, 
        C3_PCURVE_BUFFER(pcurve), last_vtx ) ;

    while ( item != NULL && n > 0 ) {
        buffer = dx3_pline_to_pc_buffer ( pline, &item, &n, x, y, 
            NULL, last_vtx ) ;
        if ( buffer != NULL ) {
            if ( C3_PCURVE_BLIST(pcurve) == NULL ) 
                C3_PCURVE_BLIST(pcurve) = dml_create_list () ;
            dml_append_data ( C3_PCURVE_BLIST(pcurve), buffer ) ;
        }
    }
    c3p_box ( pcurve, C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


