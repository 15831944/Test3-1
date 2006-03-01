//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXLTPM.H ***********************************/ 
#ifndef     DXLTPM 
#define     DXLTPM 
#include    "tststr.h"



#define     DXF_LTYPE_NAME_PTR(E)         (&(DX_TE_LTYPE(E)->ltype_name))
#define     DXF_LTYPE_NAME_GET(E)         STRTEST(DX_TE_LTYPE(E)->ltype_name.p)
#define     DXF_LTYPE_NAME_SET(E,value)   (dx0_strass(&DX_TE_LTYPE(E)->ltype_name,value))

#define     DXF_LTYPE_FLAGS(E)      (DX_TE_LTYPE(E)->ltype_flags)

#define     DXF_LTYPE_TEXT_PTR(E)         (&(DX_TE_LTYPE(E)->ltype_text))
#define     DXF_LTYPE_TEXT_GET(E)         STRTEST(DX_TE_LTYPE(E)->ltype_text.p)
#define     DXF_LTYPE_TEXT_SET(E,value)   (dx0_strass(&DX_TE_LTYPE(E)->ltype_text,value))

#define     DXF_LTYPE_ALGN(E)       (DX_TE_LTYPE(E)->ltype_algn)
#define     DXF_LTYPE_CNT(E)        (DX_TE_LTYPE(E)->ltype_count)
#define     DXF_LTYPE_TOT_LEN(E)    (DX_TE_LTYPE(E)->ltype_tot_len)
#define     DXF_LTYPE_LEN(E)        (DX_TE_LTYPE(E)->ltype_len)
#endif

