//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/****************************** DX3P11.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dxents.h>
#include <dx3defs.h>
#ifdef __ACAD11__
#include <c3cdefs.h>
#include <dmldefs.h>
#include <dx3priv.h>
#include <dx0defs.h>
#include <c3vmcrs.h>

STATIC DXF_ENTITY   dx3_line_to_entity __(( DXF_FILE, C3_CURVE )) ;
STATIC DXF_ENTITY   dx3_arc_to_entity __(( DXF_FILE, C3_CURVE )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dx3_point_to_entity ( file, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
PT3 pt ;
{
    DXF_ENTITY entity ;
    entity = dx0_alloc_entity ( DXF_POINT, file ) ;
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    C3V_COPY ( pt, DXF_POINT_PT(entity) ) ;
    RETURN ( entity ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dx3_curve_to_entity ( file, curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C3_CURVE curve ;
{ 
    if ( C3_CURVE_IS_LINE(curve) )
        RETURN ( dx3_line_to_entity ( file, curve ) ) ;
    else if ( C3_CURVE_IS_ARC(curve) )
        RETURN ( dx3_arc_to_entity ( file, curve ) ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) )
        RETURN ( dx3_pcurve_to_entity ( file, C3_CURVE_PCURVE(curve),
            C3_CURVE_T0(curve), C3_CURVE_T1(curve) ) ) ;
#ifdef SPLINE
    else if ( C3_CURVE_IS_SPLINE(curve) || C3_CURVE_IS_BEZIER(curve) ||
        C3_CURVE_IS_ELLIPSE(curve) )
        RETURN ( dx3_spline_to_entity ( file, C3_CURVE_NURB(curve),
            C3_CURVE_PARM0(curve),  C3_CURVE_PARM1(curve) ) ) ;
#endif
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx3_line_to_entity ( file, line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C3_CURVE line ;
{ 
    DXF_ENTITY entity ;

    entity = dx0_alloc_entity ( DXF_LINE, file ) ;
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    c3c_ept0 ( line, DXF_LINE_PT0(entity) ) ;
    c3c_ept1 ( line, DXF_LINE_PT1(entity) ) ;
    RETURN ( entity ) ;
}

/*-------------------------------------------------------------------------*/
STATIC DXF_ENTITY dx3_arc_to_entity ( file, curve )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C3_CURVE curve ;
{ 
    DXF_ENTITY entity ;
    PT3 x, y, ctr, normal, p0, p1 ;
    REAL rad, sweep ;

    if ( !c3c_get_arc_data ( curve, ctr, &rad, normal, &sweep ) )
        RETURN ( NULL ) ;
    dx3_arb_axis ( normal, x, y ) ;
    if ( sweep <= TWO_PI - BBS_ZERO ) {
        entity = dx0_alloc_entity ( DXF_ARC, file ) ;
        if ( entity == NULL ) 
            RETURN ( NULL ) ;
        dx3_wcs_to_ecs ( ctr, x, y, normal, DXF_ARC_CTR(entity) ) ;
        DXF_ARC_RAD(entity) = rad ;
        c3c_ept0 ( curve, p0 ) ;
        dx3_wcs_to_ecs ( p0, x, y, normal, p1 ) ;
        DXF_ARC_ANG0(entity) = qgl_atan2 ( p1[1]-DXF_ARC_CTR(entity)[1], 
            p1[0]-DXF_ARC_CTR(entity)[0] ) / PI_OVER_180 ;
        DXF_ARC_ANG1(entity) = DXF_ARC_ANG0(entity) + sweep / PI_OVER_180 ;
    }
    else {
        entity = dx0_alloc_entity ( DXF_CIRCLE, file ) ;
        if ( entity == NULL ) 
            RETURN ( NULL ) ;
        dx3_wcs_to_ecs ( ctr, x, y, normal, DXF_CIRCLE_CTR(entity) ) ;
        DXF_CIRCLE_RAD(entity) = rad ;
    }
    C3V_COPY ( normal, DXF_ENTITY_EXTRUSION(entity) ) ;
    RETURN ( entity ) ;
}
#endif /*__ACAD11__*/

