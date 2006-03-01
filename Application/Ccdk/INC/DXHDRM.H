//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXHDRM.H ***********************************/ 
#ifndef     DXHDRM
#define     DXHDRM 
#include    "tststr.h"


#define     DXF_HEADER_LAYER_GET(H)         STRTEST((H)->layer.p)
#define     DXF_HEADER_LAYER_SET(H,value)   (dx0_strass(&(H)->layer,value))


#define     DXF_HEADER_HANDLE_GET(H)        STRTEST((H)->handle)
#define     DXF_HEADER_HANDLE_SET(H,v)      (dx0_strass(&(H)->handle,v))


#define     DXF_HEADER_LTYPE(H)       ((H)->ltype)
#define     DXF_HEADER_THICKNESS(H)   ((H)->thickness) 
#define     DXF_HEADER_COLOR(H)       ((H)->color) 
#ifdef __ACAD11__
#define     DXF_HEADER_ENT_SPACE(H)   ((H)->ent_space) 
#endif
#define     DXF_HEADER_EXTRUSION(H)   ((H)->extrusion) 
#define     DX_CREATE_HEADER          (MALLOC(1,DXF_ENTITY_HEADER_S))
#define     DX_FREE_HEADER(H)         FREE(H)
#endif

