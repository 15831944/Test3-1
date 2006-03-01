//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXSHPM.H ***********************************/ 
#ifndef     DXSHPM 
#define     DXSHPM 
#include    "tststr.h"

#define     DXF_SHAPE_INSERT_PT(E)  (DX_ENTITY_SHAPE(E)->shape_insert_pt)
#define     DXF_SHAPE_SIZE(E)       (DX_ENTITY_SHAPE(E)->shape_size)

#define     DXF_SHAPE_NAME_GET(E)        STRTEST(DX_ENTITY_SHAPE(E)->shape_name.p)
#define     DXF_SHAPE_NAME_SET(E,value)  (dx0_strass(&DX_ENTITY_SHAPE(E)->shape_name,value))


#define     DXF_SHAPE_ROT_ANG(E)    (DX_ENTITY_SHAPE(E)->shape_rot_ang)
#define     DXF_SHAPE_X_SCALE(E)    (DX_ENTITY_SHAPE(E)->shape_x_scale)
#define     DXF_SHAPE_OBL_ANG(E)    (DX_ENTITY_SHAPE(E)->shape_obl_ang)
#endif

