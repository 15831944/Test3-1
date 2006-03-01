//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* DX3G1.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3ddefs.h>
#include <dxents.h>
#include <dx3defs.h>
#include <dx3priv.h>
#include <c3vmcrs.h>

STATIC C3_CURVE dx3_line_to_curve __(( DXF_ENTITY )) ;
STATIC C3_CURVE dx3_circle_to_curve __(( DXF_ENTITY )) ;
STATIC C3_CURVE dx3_arc_to_curve __(( DXF_ENTITY )) ;
STATIC C3_CURVE dx3_3dface_to_curve __(( DXF_ENTITY )) ;
STATIC C3_CURVE dx3_solid_to_curve __(( DXF_ENTITY )) ;
STATIC C3_CURVE dx3_trace_to_curve __(( DXF_ENTITY )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* dx3_ecs_to_wcs ( p, x, y, z, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 p, z, x, y, pt ;
{ 
    pt[0] = p[0] * x[0] + p[1] * y[0] + p[2] * z[0] ;
    pt[1] = p[0] * x[1] + p[1] * y[1] + p[2] * z[1] ;
    pt[2] = p[0] * x[2] + p[1] * y[2] + p[2] * z[2] ;
    RETURN ( pt ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC PT3* dx3_entity_to_point ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{ 
    RETURN ( entity != NULL && DXF_ENTITY_IS_POINT(entity) ? 
        c3d_point ( DXF_POINT_PT(entity)[0], DXF_POINT_PT(entity)[1], 
            DXF_POINT_PT(entity)[2] ) : 
        NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE dx3_entity_to_curve ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{ 
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    else if ( DXF_ENTITY_IS_LINE(entity) ) 
        RETURN ( dx3_line_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_CIRCLE(entity) ) 
        RETURN ( dx3_circle_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_ARC(entity) ) 
        RETURN ( dx3_arc_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) ) 
        RETURN ( dx3_pline_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_3DFACE(entity) ) 
        RETURN ( dx3_3dface_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_SOLID(entity) ) 
        RETURN ( dx3_solid_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_TRACE(entity) ) 
        RETURN ( dx3_trace_to_curve ( entity ) ) ;
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_line_to_curve ( line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY line ;
{
    RETURN ( c3d_line ( DXF_LINE_PT0(line), DXF_LINE_PT1(line) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_circle_to_curve ( circle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY circle ;
{
    PT3 ctr, x, y ;
    dx3_arb_axis ( DXF_ENTITY_EXTRUSION(circle), x, y ) ;
    RETURN ( c3d_circle ( 
        dx3_ecs_to_wcs ( DXF_CIRCLE_CTR(circle), x, y, 
            DXF_ENTITY_EXTRUSION(circle), ctr ),
        DXF_CIRCLE_RAD(circle), DXF_ENTITY_EXTRUSION(circle) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_arc_to_curve ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY arc ;
{
    PT3 x, y, pt0, pt1, pt2, p0, p1, p2 ;
    REAL a0, a1, a ;

    a0 = DXF_ARC_ANG0(arc) * PI_OVER_180 ;
    a1 = DXF_ARC_ANG1(arc) * PI_OVER_180 ;
    a = a1 > a0 ? 0.5 * ( a0 + a1 ) : 0.5 * ( a0 + a1 ) + PI ;
    p0[0] = DXF_ARC_CTR(arc)[0] + DXF_ARC_RAD(arc) * cos ( a0 ) ;
    p0[1] = DXF_ARC_CTR(arc)[1] + DXF_ARC_RAD(arc) * sin ( a0 ) ;
    p0[2] = DXF_ARC_CTR(arc)[2] ;
    p1[0] = DXF_ARC_CTR(arc)[0] + DXF_ARC_RAD(arc) * cos ( a ) ;
    p1[1] = DXF_ARC_CTR(arc)[1] + DXF_ARC_RAD(arc) * sin ( a ) ;
    p1[2] = DXF_ARC_CTR(arc)[2] ;
    p2[0] = DXF_ARC_CTR(arc)[0] + DXF_ARC_RAD(arc) * cos ( a1 ) ;
    p2[1] = DXF_ARC_CTR(arc)[1] + DXF_ARC_RAD(arc) * sin ( a1 ) ;
    p2[2] = DXF_ARC_CTR(arc)[2] ;

    dx3_arb_axis ( DXF_ENTITY_EXTRUSION(arc), x, y ) ;
    dx3_ecs_to_wcs ( p0, x, y, DXF_ENTITY_EXTRUSION(arc), pt0 ) ;
    dx3_ecs_to_wcs ( p1, x, y, DXF_ENTITY_EXTRUSION(arc), pt1 ) ;
    dx3_ecs_to_wcs ( p2, x, y, DXF_ENTITY_EXTRUSION(arc), pt2 ) ;
    RETURN ( c3d_arc_3pts ( pt0, pt1, pt2 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_3dface_to_curve ( face ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY face ;
{
    RETURN ( c3d_pcurve_through ( (PT3*)DXF_3DFACE_PT0(face), 
        C3V_IDENT_PTS ( DXF_3DFACE_PT2(face), DXF_3DFACE_PT3(face) ) ? 
        3 : 4 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_solid_to_curve ( solid ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY solid ;
{
    PT3 a[5] ;

    C3V_COPY ( DXF_SOLID_PT0(solid), a[0] ) ;
    C3V_COPY ( DXF_SOLID_PT1(solid), a[1] ) ;

    if ( C3V_IDENT_PTS ( DXF_SOLID_PT2(solid), DXF_SOLID_PT3(solid) ) ) { 
        C3V_COPY ( DXF_SOLID_PT2(solid), a[2] ) ;
        C3V_COPY ( DXF_SOLID_PT0(solid), a[3] ) ;
        RETURN ( c3d_pcurve_through ( a, 4 ) ) ;
    }

    else {
        C3V_COPY ( DXF_SOLID_PT2(solid), a[3] ) ;
        C3V_COPY ( DXF_SOLID_PT3(solid), a[2] ) ;
        C3V_COPY ( DXF_SOLID_PT0(solid), a[4] ) ;
        RETURN ( c3d_pcurve_through ( a, 5 ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC  C3_CURVE dx3_trace_to_curve ( trace ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY trace ;
{
    RETURN ( c3d_pcurve_through ( (PT3*)DXF_TRACE_PT0(trace), 4 ) ) ;
}

