//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** DXTTMPLS.H **********************************/ 
/*****************  Include file for DXF file manipulation  ****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#ifndef     DXTTMPLS 
#define     DXTTMPLS 

#define DX_LTYPE_TEMPLATE 2, 70, 3, 72, DXF_ARRAY_COUNT_CODE, 73, \
DXF_NODEF_CODE, 40, DXF_ARRAY_CODE, 49, DXF_END_CODE
#ifdef  __ACAD11__
#define DX_LTYPE_TEMPLATE_10    DX_LTYPE_TEMPLATE
#define DX_LTYPE_TEMPLATE_11    DX_LTYPE_TEMPLATE
#endif 

#ifdef __ACAD11__
#define DX_APPID_TEMPLATE 2, 70, DXF_END_CODE
#define DX_APPID_TEMPLATE_11    DX_APPID_TEMPLATE

#define DX_DIMSTYLE_TEMPLATE 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47,\
48, 67, 68, 69, 71, 72, 73, 74, 75, 76, 77, 78, 140, 141, 142, 143, 144, 145,\
170, 171, 172, 173, 174, 175, DXF_END_CODE
#define DX_DIMSTYLE_TEMPLATE_11    DX_DIMSTYLE_TEMPLATE
#endif

#define DX_LAYER_TEMPLATE 2, 70, 62, 6, DXF_END_CODE
#ifdef  __ACAD11__
#define DX_LAYER_TEMPLATE_10    DX_LAYER_TEMPLATE
#define DX_LAYER_TEMPLATE_11    DX_LAYER_TEMPLATE
#endif 

#define DX_STYLE_TEMPLATE 2, 70, 40, 41, 50, 71, 42, 3, 4, DXF_END_CODE
#ifdef  __ACAD11__
#define DX_STYLE_TEMPLATE_10    DX_STYLE_TEMPLATE
#define DX_STYLE_TEMPLATE_11    DX_STYLE_TEMPLATE
#endif 

#define DX_VIEW_TEMPLATE \
2, 70, 40, 41, 10, 20, 11, 21, 31, 12, 22, 32, 42, 43, 44, 50, 71, DXF_END_CODE
#ifdef  __ACAD11__
#define DX_VIEW_TEMPLATE_10    DX_VIEW_TEMPLATE
#define DX_VIEW_TEMPLATE_11    DX_VIEW_TEMPLATE
#endif 

#define DX_UCS_TEMPLATE 2, 70, 10, 20, 30, 11, 21, 31, 12, 22, 32, DXF_END_CODE
#ifdef  __ACAD11__
#define DX_UCS_TEMPLATE_10    DX_UCS_TEMPLATE
#define DX_UCS_TEMPLATE_11    DX_UCS_TEMPLATE
#endif 

#define DX_VPORT_TEMPLATE 2, 70, 10, 20, 11, 21, 12, 22, 13, 23, 14, 24, \
15, 25, 16, 26, 36, 17, 27, 37, 40, 41, 42, 43, 44, \
50, 51, 71, 72, 73, 74, 75, 76, 77, 78, DXF_END_CODE
#ifdef  __ACAD11__
#define DX_VPORT_TEMPLATE_10    DX_VPORT_TEMPLATE
#define DX_VPORT_TEMPLATE_11 2, 70, 10, 20, 11, 21, 12, 22, 13, 23, 14, 24, \
15, 25, 16, 26, 36, 17, 27, 37, 40, 41, 42, 43, 44, \
50, 51, 68, 69, 71, 72, 73, 74, 75, 76, 77, 78, DXF_END_CODE
#endif 

#endif
