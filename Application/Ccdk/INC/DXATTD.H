//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXATTD.H ***********************************/
#ifndef     DXATTD 
#define     DXATTD 
typedef     struct  dx_attrib_struct {
            DXF_ENTITY_HEADER_S    attrib_entity_header ;
            REAL    attrib_pt[3] ;
            REAL    attrib_height ;
            DX_TXT_STRING attrib_value ;
            DX_STRING  attrib_tag ;
            INT     attrib_aflags ;
            INT     attrib_fld_len ;
            REAL    attrib_rot_ang ;
            REAL    attrib_x_scale ;
            REAL    attrib_obl_ang ;
            DX_STRING  attrib_style ;
            INT     attrib_gflags ;
            INT     attrib_just ;
            REAL    attrib_al_pt[3] ;
            } DX_ATTRIB_S ;
typedef     DX_ATTRIB_S *DX_ATTRIB ;
#endif

