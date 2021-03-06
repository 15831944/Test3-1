//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXVIEM.H ***********************************/ 
#ifndef     DXVIEM 
#define     DXVIEM 
#include    "tststr.h"

#define     DXF_VIEW_NAME_PTR(E)        (&(DX_TE_VIEW(E)->view_name))
#define     DXF_VIEW_NAME_GET(E)        STRTEST(DX_TE_VIEW(E)->view_name.p)
#define     DXF_VIEW_NAME_SET(E,value)  (dx0_strass(&DX_TE_VIEW(E)->view_name,value))



#define     DXF_VIEW_HEIGHT(E)      (DX_TE_VIEW(E)->view_height)
#define     DXF_VIEW_WIDTH(E)       (DX_TE_VIEW(E)->view_width)
#define     DXF_VIEW_CTR(E)         (DX_TE_VIEW(E)->view_ctr)
#define     DXF_VIEW_DIR(E)         (DX_TE_VIEW(E)->view_dir)
#define     DXF_VIEW_TGT_PT(E)      (DX_TE_VIEW(E)->view_tgt_pt)
#define     DXF_VIEW_LENS_LEN(E)    (DX_TE_VIEW(E)->view_lens_len)
#define     DXF_VIEW_FR_CLIP(E)     (DX_TE_VIEW(E)->view_fr_clip)
#define     DXF_VIEW_BK_CLIP(E)     (DX_TE_VIEW(E)->view_bk_clip)
#define     DXF_VIEW_TWIST(E)       (DX_TE_VIEW(E)->view_twist)
#define     DXF_VIEW_MODE(E)        (DX_TE_VIEW(E)->view_mode)
#endif
