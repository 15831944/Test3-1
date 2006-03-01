//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/****************************** DX3P11Q.C *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3rdefs.h>
#include <dxents.h>
#include <dx3defs.h>
#ifdef __ACAD11__
#include <dx3priv.h>
#include <dmldefs.h>
#include <c3qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_ENTITY dx3_pc_buffer_to_entity ( file, buffer, pline, 
        planar, x, y, extrusion ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C3_PC_BUFFER buffer ;
DXF_ENTITY pline ;
BOOLEAN planar ;
PT3 x, y, extrusion ;
{ 
    INT i ;
    DXF_ENTITY vertex ;
    REAL d ;

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) {
        d = c3r_d_parm ( C3_PC_BUFFER_ARC(buffer,i) ) ;
        if ( !IS_SMALL(d) )
            DXF_PLINE_FLAGS(pline) |= DXF_PLINE_FIT ;
        vertex = dx3_vertex_to_entity ( file, C3_PC_BUFFER_PT(buffer,i), 
            d, 0, planar, x, y, extrusion ) ;
        if ( vertex == NULL ||
            dml_append_data ( DXF_PLINE_VLIST(pline), vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( pline ) ;
}
#endif /*__ACAD11__*/

