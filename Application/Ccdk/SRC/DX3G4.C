//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* DX3G4.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3qdefs.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx3defs.h>
#include <dx3priv.h>
#include <c3vdefs.h>
#include <c3qmcrs.h>
#include <c3vmcrs.h>

STATIC void     dx3_pcurve_d_vec __(( DXF_ENTITY, DXF_ENTITY, PT3, 
            PT3, PT3 )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER dx3_pline_to_pc_buffer ( pline, item_ptr, n_ptr, 
            x, y, buffer, vtx0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY pline ;
DML_ITEM *item_ptr ;
INT *n_ptr ;
PT3 x, y ;
C3_PC_BUFFER buffer ;
DXF_ENTITY vtx0 ;
{
    DXF_ENTITY vertex, prev_vtx = NULL ;
    INT i, n ;

    if ( *n_ptr <= C3_PC_BUFFER_MAXSIZE ) {
        n = *n_ptr ;
        if ( vtx0 != NULL ) 
            n++ ;
    }
    else {
        n = C3_PC_BUFFER_MAXSIZE ;
        vtx0 = NULL ;
    }
    if ( n <= 0 ) 
        RETURN ( NULL ) ;
    buffer = c3q_create ( buffer, n ) ;

    for ( i=0 ; i < n-1 ; i++, *item_ptr = DML_NEXT(*item_ptr) ) {
        vertex = (DXF_ENTITY)DML_RECORD(*item_ptr) ;
        dx3_pcurve_pt ( pline, vertex, x, y, C3_PC_BUFFER_PT(buffer,i) ) ;
        if ( DXF_PLINE_FLAGS(pline)&DXF_PLINE_3D ) {
            C3V_SET_ZERO ( C3_PC_BUFFER_D_VEC(buffer,i) ) ;
            /* You cannot use this number in 3d anyway */
        }
        else {
            dx3_pcurve_d_vec ( prev_vtx, vertex, x, y, 
                C3_PC_BUFFER_D_VEC(buffer,i-1) ) ;
            prev_vtx = vertex ;
        }
        C3_PC_BUFFER_N(buffer)++ ;
        (*n_ptr)-- ;
    }
    if ( vtx0 == NULL ) {
        vertex = (DXF_ENTITY)DML_RECORD(*item_ptr) ;
        dx3_pcurve_pt ( pline, vertex, x, y, C3_PC_BUFFER_PT(buffer,i) ) ;
    }
    else {
        dx3_pcurve_pt ( pline, vtx0, x, y, C3_PC_BUFFER_PT(buffer,i) ) ;
/*
        if ( DXF_PLINE_FLAGS(pline)&DXF_PLINE_3D ) ;
        else 
            dx3_pcurve_d_vec ( prev_vtx, vtx0, x, y, 
                C3_PC_BUFFER_D_VEC(buffer,i-1) ) ;
*/
        if ( !( DXF_PLINE_FLAGS(pline)&DXF_PLINE_3D ) ) 
            dx3_pcurve_d_vec ( prev_vtx, vtx0, x, y, 
                C3_PC_BUFFER_D_VEC(buffer,i-1) ) ;
        *item_ptr = NULL ;
    }
    C3_PC_BUFFER_N(buffer)++ ;
    RETURN ( buffer ) ;
}




/*-------------------------------------------------------------------------*/
STATIC void dx3_pcurve_d_vec ( prev_vtx, vtx, x, y, d_vec )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY prev_vtx, vtx ;
PT3 x, y, d_vec ;
{
    REAL a0, a1, dist ;

    if ( prev_vtx == NULL ) 
        RETURN ;
    dist = c3v_dist ( DXF_VERTEX_PT(vtx), DXF_VERTEX_PT(prev_vtx) ) ;
    a0 = ( DXF_VERTEX_PT(vtx)[0] - DXF_VERTEX_PT(prev_vtx)[0] ) / dist ;
    a1 = ( DXF_VERTEX_PT(vtx)[1] - DXF_VERTEX_PT(prev_vtx)[1] ) / dist ;
    a0 *= DXF_VERTEX_BULGE(prev_vtx) ;
    a1 *= DXF_VERTEX_BULGE(prev_vtx) ;
    d_vec[0] = a1 * x[0] - a0 * y[0] ;
    d_vec[1] = a1 * x[1] - a0 * y[1] ;
    d_vec[2] = a1 * x[2] - a0 * y[2] ;
}


