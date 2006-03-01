//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXDESM.H ***********************************/ 
#ifndef     DXDESM
#define     DXDESM 
#include    "tststr.h"

#define     DXF_DESC_LAYER_PTR(D)       (&((D)->layer))
#define     DXF_DESC_LAYER_GET(D)       STRTEST((D)->layer.p)
#define     DXF_DESC_LAYER_SET(D,value) (dx0_strass(&(D)->layer,value))


#define     DXF_DESC_HANDLE_PTR(D)      (&((D)->handle))
#define     DXF_DESC_HANDLE_GET(D)      STRTEST((D)->handle.p)
#define     DXF_DESC_HANDLE_SET(D,v)    (dx0_strass(&(D)->handle,v))



#define     DXF_DESC_LTYPE_STATUS(D)    ((D)->ltype_status)

#define     DXF_DESC_LTYPE_PTR(D)       (&((D)->ltype))
#define     DXF_DESC_LTYPE_GET(D)       STRTEST((D)->ltype.p)
#define     DXF_DESC_LTYPE_SET(D,value) (dx0_strass(&(D)->ltype,value))

#define     DXF_DESC_ELEVATION(D)     ((D)->elevation) 
#define     DXF_DESC_THICKNESS(D)     ((D)->thickness) 
#define     DXF_DESC_COLOR_STATUS(D)  ((D)->color_status) 
#define     DXF_DESC_COLOR(D)         ((D)->color) 
#define     DXF_DESC_EXTRUSION(D)     ((D)->extrusion) 
#define     DX_CREATE_DESC            (MALLOC(1,DXF_ENTITY_DESC_S))
#define     DX_FREE_DESC(D)           FREE(D)
#endif
