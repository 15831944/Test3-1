//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXLTPD.H ***********************************/ 
#ifndef     DXLTPD 
#define     DXLTPD 
typedef     struct      dx_ltype_struct {
            DX_STRING   ltype_name ;
            INT         ltype_flags ;
            DX_STRING   ltype_text ;
            INT         ltype_algn ;
            INT         ltype_count ;
            REAL        ltype_tot_len ;
            REAL*       ltype_len ;
            } DX_LTYPE_S ;
typedef     DX_LTYPE_S  *DX_LTYPE ;
#endif 

