//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXLAYM.H ***********************************/ 
#ifndef     DXLAYM 
#define     DXLAYM 
#include    "tststr.h"


#define     DXF_LAYER_ON_THAWED     0
#define     DXF_LAYER_FROZEN        1
#define     DXF_LAYER_NOT_USED      2
#define     DXF_LAYER_HAS_VP_EXCL   3

#define     DXF_LAYER_NAME_PTR(E)          (&(DX_TE_LAYER(E)->layer_name))
#define     DXF_LAYER_NAME_GET(E)          STRTEST(DX_TE_LAYER(E)->layer_name.p)
#define     DXF_LAYER_NAME_SET(E,value)    (dx0_strass(&DX_TE_LAYER(E)->layer_name,value))
#define     DXF_LAYER_NAME_FREE(E)         (dx0_strfree(&(DX_TE_LAYER(E)->layer_name)))

#define     DXF_LAYER_FLAGS(E)      (DX_TE_LAYER(E)->layer_flags)
#define     DXF_LAYER_COLOR(E)      (DX_TE_LAYER(E)->layer_color)

#define     DXF_LAYER_LTYPE_PTR(E)        (&(DX_TE_LAYER(E)->layer_ltype))
#define     DXF_LAYER_LTYPE_GET(E)        STRTEST(DX_TE_LAYER(E)->layer_ltype.p)
#define     DXF_LAYER_LTYPE_SET(E,value)  (dx0_strass(&DX_TE_LAYER(E)->layer_ltype,value))


#endif
