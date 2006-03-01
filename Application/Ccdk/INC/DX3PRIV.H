//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** DX3PRIV.H ***********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX3PRIV
#define     DX3PRIV

extern  BOOLEAN     dx3_pc_buffer_planar __(( C3_PC_BUFFER DUMMY0 )) ;
extern  C3_PC_BUFFER dx3_pline_to_pc_buffer __(( DXF_ENTITY, DML_ITEM*, 
            INT*, PT3, PT3, C3_PC_BUFFER, DXF_ENTITY )) ;
extern  C3_CURVE dx3_pline_to_pcurve __(( DXF_ENTITY )) ;
extern  void     dx3_pcurve_pt __(( DXF_ENTITY, DXF_ENTITY, PT3, PT3, PT3 )) ;
#ifdef __ACAD11__
extern  DXF_ENTITY   dx3_pcurve_to_entity __(( DXF_FILE DUMMY0 , 
            C3_PCURVE DUMMY1 , REAL DUMMY2 , REAL DUMMY3 )) ;
extern  DXF_ENTITY   dx3_pc_buffer_to_entity __(( DXF_FILE DUMMY0 , 
            C3_PC_BUFFER DUMMY1 , DXF_ENTITY DUMMY2 , BOOLEAN DUMMY3 , 
            PT3 DUMMY4 , PT3 DUMMY5 , PT3 DUMMY6 )) ;
#ifdef SPLINE
extern  DXF_ENTITY   dx3_spline_to_entity __(( DXF_FILE DUMMY0 , 
            C3_NURB DUMMY1 , PARM DUMMY2 , PARM DUMMY3 )) ;
#endif
extern  DXF_ENTITY   dx3_vertex_to_entity __(( DXF_FILE DUMMY0 , PT3 DUMMY1 , 
            REAL DUMMY2 , INT DUMMY3 , BOOLEAN DUMMY4 , PT3 DUMMY5 , 
            PT3 DUMMY6 , PT3 DUMMY7 )) ;
#else
extern  DXF_ENTITY  dx3_pcurve_to_entity __(( C3_PCURVE DUMMY0 , 
            REAL DUMMY1 , REAL DUMMY2 )) ;
extern  DXF_ENTITY   dx3_pc_buffer_to_entity __(( C3_PC_BUFFER DUMMY0 , 
            DXF_ENTITY DUMMY1 , BOOLEAN DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 , 
            PT3 DUMMY5 )) ;
#ifdef SPLINE
extern  DXF_ENTITY  dx3_spline_to_entity __(( C3_NURB DUMMY0 , PARM DUMMY1 , 
            PARM  DUMMY2 )) ;
#endif
extern  DXF_ENTITY   dx3_vertex_to_entity __(( PT3 DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 , BOOLEAN DUMMY3 , PT3 DUMMY4 , PT3 DUMMY4 , 
            PT3 DUMMY5 )) ;
#endif
extern  void        dx3_wcs_to_ecs __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  void        dx3_arb_axis __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_CURVE    dx3_pline_to_curve __(( DXF_ENTITY DUMMY0 )) ;
extern  REAL*       dx3_ecs_to_wcs __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
#endif

