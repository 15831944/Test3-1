//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXFLTM.H ***********************************/
#ifndef     DXFLTM 
#define     DXFLTM 

#include    "tststr.h"


#define     DX_FILTER_TYPE(F)       ((F)->filter_entity_type)

#define     DX_FILTER_LAYER_PTR(F)            (&((F)->filter_layer))
#define     DX_FILTER_LAYER_GET(F)            STRTEST((F)->filter_layer.p)
#define     DX_FILTER_LAYER_SET(F,value)      (dx0_strass(&((F)->filter_layer),value))

#define     DX_FILTER_COLOR_STATUS(F)      ((F)->filter_color_status)
#define     DX_FILTER_COLOR(F)      ((F)->filter_color)
#define     DX_CREATE_FILTER        (MALLOC(1,DXF_FILTER_S))
#define     DX_FREE_FILTER(F)       FREE(F)
#endif

