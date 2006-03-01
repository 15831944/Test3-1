//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXATDM.H ***********************************/
#ifndef     DXATDM 
#define     DXATDM 
#include    "tststr.h"

#define     DXF_ATTDEF_INVIS            1
#define     DXF_ATTDEF_IS_CONST         2
#define     DXF_ATTDEF_VER_REQ          4
#define     DXF_ATTDEF_IS_PRESET        8
#define     DXF_ATTDEF_PT(E)        (DX_ENTITY_ATTDEF(E)->attdef_pt)
#define     DXF_ATTDEF_HEIGHT(E)    (DX_ENTITY_ATTDEF(E)->attdef_height)

#define     DXF_ATTDEF_VALUE_PTR(E)           (&(DX_ENTITY_ATTDEF(E)->attdef_value))
#define     DXF_ATTDEF_VALUE_GET(E)           STRTEST(DX_ENTITY_ATTDEF(E)->attdef_value.p)
#define     DXF_ATTDEF_VALUE_SET(E,value)     (dx0_strass(&DX_ENTITY_ATTDEF(E)->attdef_value,value))


#define     DXF_ATTDEF_PROMPT_PTR(E)        (&(DX_ENTITY_ATTDEF(E)->attdef_prompt))
#define     DXF_ATTDEF_PROMPT_GET(E)        STRTEST(DX_ENTITY_ATTDEF(E)->attdef_prompt.p)
#define     DXF_ATTDEF_PROMPT_SET(E,value)  (dx0_strass(&DX_ENTITY_ATTDEF(E)->attdef_prompt,value))


#define     DXF_ATTDEF_TAG_PTR(E)       (&(DX_ENTITY_ATTDEF(E)->attdef_tag))
#define     DXF_ATTDEF_TAG_GET(E)       STRTEST(DX_ENTITY_ATTDEF(E)->attdef_tag.p)
#define     DXF_ATTDEF_TAG_SET(E,value) (dx0_strass(&DX_ENTITY_ATTDEF(E)->attdef_tag,value))


#define     DXF_ATTDEF_AFLAGS(E)    (DX_ENTITY_ATTDEF(E)->attdef_aflags)
#define     DXF_ATTDEF_FLD_LEN(E)   (DX_ENTITY_ATTDEF(E)->attdef_fld_len)
#define     DXF_ATTDEF_ROT_ANG(E)   (DX_ENTITY_ATTDEF(E)->attdef_rot_ang)
#define     DXF_ATTDEF_X_SCALE(E)   (DX_ENTITY_ATTDEF(E)->attdef_x_scale)
#define     DXF_ATTDEF_OBL_ANG(E)   (DX_ENTITY_ATTDEF(E)->attdef_obl_ang)

#define     DXF_ATTDEF_STYLE_PTR(E)        (&(DX_ENTITY_ATTDEF(E)->attdef_style))
#define     DXF_ATTDEF_STYLE_GET(E)        STRTEST(DX_ENTITY_ATTDEF(E)->attdef_style.p)
#define     DXF_ATTDEF_STYLE_SET(E,value)  (dx0_strass(&DX_ENTITY_ATTDEF(E)->attdef_style,value))

#define     DXF_ATTDEF_GFLAGS(E)    (DX_ENTITY_ATTDEF(E)->attdef_gflags)
#define     DXF_ATTDEF_JUST(E)      (DX_ENTITY_ATTDEF(E)->attdef_just)
#define     DXF_ATTDEF_AL_PT(E)     (DX_ENTITY_ATTDEF(E)->attdef_al_pt)
#endif

