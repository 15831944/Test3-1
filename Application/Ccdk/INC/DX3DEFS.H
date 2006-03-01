//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** DX3DEFS.H ***********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX3DEFS
#define     DX3DEFS

#include <dxfdefs.h>
#include <dmdefs.h>
#include <c3defs.h>

extern  PT3*        dx3_entity_to_point __(( DXF_ENTITY DUMMY0 )) ;
extern  C3_CURVE    dx3_entity_to_curve __(( DXF_ENTITY DUMMY0 )) ;
#ifdef __ACAD11__
extern  DXF_ENTITY  dx3_point_to_entity __(( DXF_FILE DUMMY0 , PT3 DUMMY1 )) ;
extern  DXF_ENTITY  dx3_curve_to_entity __(( DXF_FILE DUMMY0 , 
            C3_CURVE DUMMY1 )) ;
#else
extern  DXF_ENTITY  dx3_point_to_entity __(( PT3 DUMMY0 )) ;
extern  DXF_ENTITY  dx3_curve_to_entity __(( C3_CURVE DUMMY0 )) ;
#endif
extern  PT3*        dx3_get_point __(( DXF_FILE DUMMY0 , DXF_FILTER DUMMY1 )) ;
extern  DML_LIST    dx3_get_points __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  DML_LIST    dx3_read_points __(( STRING DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  C3_CURVE    dx3_get_curve __(( DXF_FILE DUMMY0 , DXF_FILTER DUMMY1 )) ;
extern  DML_LIST    dx3_get_curves __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  DML_LIST    dx3_read_curves __(( STRING DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  BOOLEAN     dx3_put_point __(( DXF_FILE DUMMY0 , PT3 DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx3_put_points __(( DXF_FILE DUMMY0 , DML_LIST DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx3_write_points __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 ,
            DML_LIST DUMMY2 ,  DXF_ENTITY_DESC DUMMY3 )) ;
extern  BOOLEAN     dx3_put_curve __(( DXF_FILE DUMMY0 , C3_CURVE DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx3_put_curves __(( DXF_FILE DUMMY0 , DML_LIST DUMMY1 ,
            DXF_ENTITY_DESC DUMMY2 )) ;
extern  BOOLEAN     dx3_write_curve __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 ,
            C3_CURVE DUMMY2 ,  DXF_ENTITY_DESC DUMMY3 )) ;
extern  BOOLEAN     dx3_write_curves __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 ,
            DML_LIST DUMMY2 ,  DXF_ENTITY_DESC DUMMY3 )) ;
#endif     /*DX3DEFS*/


