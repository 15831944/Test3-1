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
#include <dmldefs.h>
#include <dx3defs.h>
#include <dx0defs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_put_point ( file, point, descriptor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
PT3 point ;
DXF_ENTITY_DESC descriptor ;
{ 
    BOOLEAN status ;
#ifdef __ACAD11__
    DXF_ENTITY point_entity = dx3_point_to_entity ( file, point ) ;
#else
    DXF_ENTITY point_entity = dx3_point_to_entity ( point ) ;
#endif
    if ( point_entity == NULL )
        RETURN ( FALSE ) ;
#ifdef __ACAD11__
    dx0_set_entity_descriptor ( file, point_entity, descriptor ) ;
#else
    dx0_set_entity_descriptor ( point_entity, descriptor ) ;
#endif
    status = dxf_put_entity ( file, point_entity ) ;
    dxf_free_entity ( point_entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_put_points ( file, points, descriptor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST points ;
DXF_ENTITY_DESC descriptor ;
{ 
    BOOLEAN status = TRUE ;
    DML_ITEM item ;

    if ( points == NULL )
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( points, item ) 
        status = dx3_put_point ( file, DML_RECORD(item), descriptor ) &&
            status ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_write_points ( filename, file_type, 
        points, descriptor )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DXF_FILE_MODE file_type ;
DML_LIST points ;
DXF_ENTITY_DESC descriptor ;
{ 
    DXF_FILE file = dxf_fopen ( filename, DXF_WRITE | file_type ) ;

    if ( file == NULL )
        RETURN ( FALSE ) ;

    dxf_set_precision ( file, (INT) ( 1.0 - log10 ( BBS_TOL ) ) ) ;
    RETURN ( dxf_open_entities ( file ) &&
        dx3_put_points ( file, points, descriptor ) && 
        dxf_close_entities ( file ) && dxf_fclose ( file ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_put_curve ( file, curve, descriptor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
C3_CURVE curve ;
DXF_ENTITY_DESC descriptor ;
{ 
    BOOLEAN status ;
#ifdef __ACAD11__
    DXF_ENTITY curve_entity = dx3_curve_to_entity ( file, curve ) ;
#else
    DXF_ENTITY curve_entity = dx3_curve_to_entity ( curve ) ;
#endif

    if ( curve_entity == NULL )
        RETURN ( FALSE ) ;
#ifdef __ACAD11__
    dx0_set_entity_descriptor ( file, curve_entity, descriptor ) ;
#else
    dx0_set_entity_descriptor ( curve_entity, descriptor ) ;
#endif
    status = dxf_put_entity ( file, curve_entity ) ;
    dxf_free_entity ( curve_entity ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_put_curves ( file, curves, descriptor )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST curves ;
DXF_ENTITY_DESC descriptor ;
{
    BOOLEAN status = TRUE ;
    DML_ITEM item ;

    if ( curves == NULL )
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( curves, item )
        status = dx3_put_curve ( file, DML_RECORD(item), descriptor ) &&
            status ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_write_curve ( filename, file_type, curve, descriptor )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DXF_FILE_MODE file_type ;
C3_CURVE curve ;
DXF_ENTITY_DESC descriptor ;
{
    DXF_FILE file = dxf_fopen ( filename, DXF_WRITE | file_type ) ;

    if ( file == NULL )
        RETURN ( FALSE ) ;

    dxf_set_precision ( file, (INT) ( 1.0 - log10 ( BBS_TOL ) ) ) ;
    RETURN ( dxf_open_entities ( file ) && 
        dx3_put_curve ( file, curve, descriptor ) &&
        dxf_close_entities ( file ) && dxf_fclose ( file ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dx3_write_curves ( filename, file_type, curves, descriptor )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DXF_FILE_MODE file_type ;
DML_LIST curves ;
DXF_ENTITY_DESC descriptor ;
{
    DXF_FILE file = dxf_fopen ( filename, DXF_WRITE | file_type ) ;

    if ( file == NULL )
        RETURN ( FALSE ) ;

    dxf_set_precision ( file, (INT) ( 1.0 - log10 ( BBS_TOL ) ) ) ;
    RETURN ( dxf_open_entities ( file ) && 
        dx3_put_curves ( file, curves, descriptor ) &&
        dxf_close_entities ( file ) && dxf_fclose ( file ) ) ;
}

