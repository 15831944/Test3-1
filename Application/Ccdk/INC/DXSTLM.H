//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXSTLM.H ***********************************/ 
#ifndef     DXSTLM 
#define     DXSTLM 
#include    "tststr.h"

#define     DXF_STYLE_MIRRORED_X    2
#define     DXF_STYLE_MIRRORED_Y    4

#define     DXF_STYLE_NAME_PTR(E)       (&(DX_TE_STYLE(E)->style_name))
#define     DXF_STYLE_NAME_GET(E)       STRTEST(DX_TE_STYLE(E)->style_name.p)
#define     DXF_STYLE_NAME_SET(E,value)       (dx0_strass(&DX_TE_STYLE(E)->style_name,value))

#define     DXF_STYLE_FLAGS(E)      (DX_TE_STYLE(E)->style_flags)
#define     DXF_STYLE_HEIGHT(E)     (DX_TE_STYLE(E)->style_height)
#define     DXF_STYLE_WIDTH(E)      (DX_TE_STYLE(E)->style_width)
#define     DXF_STYLE_OBL_ANG(E)    (DX_TE_STYLE(E)->style_obl_ang)
#define     DXF_STYLE_GFLAGS(E)     (DX_TE_STYLE(E)->style_gflags)
#define     DXF_STYLE_LAST_HT(E)    (DX_TE_STYLE(E)->style_last_ht)

#define     DXF_STYLE_FONT_PTR(E)       (&(DX_TE_STYLE(E)->style_font))
#define     DXF_STYLE_FONT_GET(E)       STRTEST(DX_TE_STYLE(E)->style_font.p)
#define     DXF_STYLE_BIGFONT_PTR(E)    (&(DX_TE_STYLE(E)->style_bigfont))
#define     DXF_STYLE_BIGFONT_GET(E)    STRTEST(DX_TE_STYLE(E)->style_bigfont.p)

#define     DXF_STYLE_FONT_SET(E,value)       ( dx0_strass(&DX_TE_STYLE(E)->style_font,value    ))
#define     DXF_STYLE_BIGFONT_SET(E,value)    ( dx0_strass(&DX_TE_STYLE(E)->style_bigfont,value ))


#endif

