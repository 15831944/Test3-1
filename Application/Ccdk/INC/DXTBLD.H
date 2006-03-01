//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXTBLD.H ***********************************/ 
#ifndef     DXTBLD 
#define     DXTBLD 

typedef     INT     DXF_TABLE_ENTRY_ID ;

#define     DXF_LTYPE         1
#define     DXF_LAYER         2
#define     DXF_STYLE         3
#define     DXF_VIEW          4
#define     DXF_UCS           5
#define     DXF_VPORT         6
#ifdef __ACAD11__
#define     DXF_DIMSTYLE      7
#define     DXF_APPID         8
#endif

typedef     struct  dx_te_struct {
            DXF_ENAME       te_ename ;
            DXF_TABLE_ENTRY_ID  te_id ;
            union {
                STRING      te_anytype ;
#ifdef DX_TBL_ON
                DX_LTYPE_S  te_ltype ;
#ifdef __ACAD11__
                DX_APPID_S  te_appid ;
                DX_DIMSTYLE_S  te_dimstyle ;
#endif
                DX_LAYER_S  te_layer ;
                DX_STYLE_S  te_style ;
                DX_VIEW_S   te_view ;
                DX_UCS_S    te_ucs ;
                DX_VPORT_S  te_vport ;
#endif  /* DX_TBL_ON */
            }   te_data ;
            } DXF_TABLE_ENTRY_S ;
typedef     DXF_TABLE_ENTRY_S *DXF_TABLE_ENTRY ;
#endif

