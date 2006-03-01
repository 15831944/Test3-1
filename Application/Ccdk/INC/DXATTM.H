//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXATTM.H ***********************************/
#ifndef     DXATTM
#define     DXATTM 
#define     DXF_ATTRIB_INVIS        DXF_ATTDEF_INVIS
#define     DXF_ATTRIB_IS_CONST     DXF_ATTDEF_IS_CONST
#define     DXF_ATTRIB_VER_REQ      DXF_ATTDEF_VER_REQ
#define     DXF_ATTRIB_IS_PRESET    DXF_ATTDEF_IS_PRESET
#define     DXF_ATTRIB_PT(E)        (DX_ENTITY_ATTRIB(E)->attrib_pt)
#define     DXF_ATTRIB_HEIGHT(E)    (DX_ENTITY_ATTRIB(E)->attrib_height)

#define     DXF_ATTRIB_VALUE_PTR(E)           (&(DX_ENTITY_ATTRIB(E)->attrib_value))
#define     DXF_ATTRIB_VALUE_GET(E)           STRTEST(DX_ENTITY_ATTRIB(E)->attrib_value.p)
#define     DXF_ATTRIB_VALUE_SET(E,value)     (dx0_strass(&DX_ENTITY_ATTRIB(E)->attrib_value,value))

#define     DXF_ATTRIB_TAG_PTR(E)       (&(DX_ENTITY_ATTRIB(E)->attrib_tag))
#define     DXF_ATTRIB_TAG_GET(E)       STRTEST(DX_ENTITY_ATTRIB(E)->attrib_tag.p)
#define     DXF_ATTRIB_TAG_SET(E,value) (dx0_strass(&DX_ENTITY_ATTRIB(E)->attrib_tag,value))


#define     DXF_ATTRIB_AFLAGS(E)    (DX_ENTITY_ATTRIB(E)->attrib_aflags)
#define     DXF_ATTRIB_FLD_LEN(E)   (DX_ENTITY_ATTRIB(E)->attrib_fld_len)
#define     DXF_ATTRIB_ROT_ANG(E)   (DX_ENTITY_ATTRIB(E)->attrib_rot_ang)
#define     DXF_ATTRIB_X_SCALE(E)   (DX_ENTITY_ATTRIB(E)->attrib_x_scale)
#define     DXF_ATTRIB_OBL_ANG(E)   (DX_ENTITY_ATTRIB(E)->attrib_obl_ang)

#define     DXF_ATTRIB_STYLE_PTR(E)        (&(DX_ENTITY_ATTRIB(E)->attrib_style))
#define     DXF_ATTRIB_STYLE_GET(E)        STRTEST(DX_ENTITY_ATTRIB(E)->attrib_style.p)
#define     DXF_ATTRIB_STYLE_SET(E,value)  (dx0_strass(&DX_ENTITY_ATTRIB(E)->attrib_style,value))

#define     DXF_ATTRIB_GFLAGS(E)    (DX_ENTITY_ATTRIB(E)->attrib_gflags)
#define     DXF_ATTRIB_JUST(E)      (DX_ENTITY_ATTRIB(E)->attrib_just)
#define     DXF_ATTRIB_AL_PT(E)     (DX_ENTITY_ATTRIB(E)->attrib_al_pt)
#endif

