//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXAPPM.H ***********************************/ 
#ifndef     DXAPPM 
#define     DXAPPM 
#ifdef __ACAD11__

#define     DXF_APPID_NAME_PTR(E)        (&(DX_TE_APPID(E)->appid_name))
#define     DXF_APPID_NAME_GET(E)        (DX_TE_APPID(E)->appid_name.p)
#define     DXF_APPID_NAME_SET(E,value)  (dx0_strass(&DX_TE_APPID(E)->appid_name,value))


#define     DXF_APPID_FLAGS(E)       (DX_TE_APPID(E)->appid_flags)
#endif
#endif

