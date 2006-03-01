//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXSTLD.H ***********************************/ 
#ifndef     DXSTLD 
#define     DXSTLD 

typedef     struct  dx_style_struct {
            DX_STRING  style_name ;
            INT     style_flags ;
            REAL    style_height ;
            REAL    style_width ;
            REAL    style_obl_ang ;
            INT     style_gflags ;
            REAL    style_last_ht ;
            DX_STRING  style_font ;
            DX_STRING  style_bigfont ;
            } DX_STYLE_S ;
typedef     DX_STYLE_S  *DX_STYLE ;
#endif
