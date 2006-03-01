//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXUCSM.H ***********************************/ 
/*****************  Include file for DXF file manipulation  ****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#ifndef     DXUCSM 
#define     DXUCSM 
#include    "tststr.h"

#define     DXF_UCS_NAME_PTR(E)         (&(DX_TE_UCS(E)->ucs_name))
#define     DXF_UCS_NAME_GET(E)         STRTEST(DX_TE_UCS(E)->ucs_name.p)
#define     DXF_UCS_NAME_SET(E,value)    (dx0_strass(&DX_TE_UCS(E)->ucs_name,value))


#define     DXF_UCS_FLAGS(E)        (DX_TE_UCS(E)->ucs_flags)
#define     DXF_UCS_ORIGIN(E)       (DX_TE_UCS(E)->ucs_origin)
#define     DXF_UCS_X_AXIS(E)       (DX_TE_UCS(E)->ucs_x_axis)
#define     DXF_UCS_Y_AXIS(E)       (DX_TE_UCS(E)->ucs_y_axis)
#endif
