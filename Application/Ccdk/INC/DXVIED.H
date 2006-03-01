//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXVIED.H ***********************************/ 
#ifndef     DXVIED 
#define     DXVIED 
typedef     struct  dx_view_struct {
            DX_STRING  view_name ;
            INT     view_flags ;
            REAL    view_height ;
            REAL    view_width ;
            REAL    view_ctr[2] ;
            REAL    view_dir[3] ;
            REAL    view_tgt_pt[3] ;
            REAL    view_lens_len ;
            REAL    view_fr_clip ;
            REAL    view_bk_clip ;
            REAL    view_twist ;
            INT     view_mode ;
            } DX_VIEW_S ;
typedef     DX_VIEW_S  *DX_VIEW ;
#endif
