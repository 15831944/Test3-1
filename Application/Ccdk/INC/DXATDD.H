//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXATDD.H ***********************************/
#ifndef     DXATDD 
#define     DXATDD 
typedef     struct  dx_attdef_struct {
            DXF_ENTITY_HEADER_S    attdef_entity_header ;
            REAL    attdef_pt[3] ;
            REAL    attdef_height ;
            DX_TXT_STRING attdef_value ;
            DX_STRING  attdef_prompt ;
            DX_STRING  attdef_tag ;
            INT     attdef_aflags ;
            INT     attdef_fld_len ;
            REAL    attdef_rot_ang ;
            REAL    attdef_x_scale ;
            REAL    attdef_obl_ang ;
            DX_STRING  attdef_style ;
            INT     attdef_gflags ;
            INT     attdef_just ;
            REAL    attdef_al_pt[3] ;
            } DX_ATTDEF_S ;
typedef     DX_ATTDEF_S *DX_ATTDEF ;
#endif

