//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* W3DDEFS.H *********************************/
/*************************** View prototypes *******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef W3DDEFS
#define W3DDEFS

#include <c3defs.h>
#include <wmdefs.h>

extern  void    w3d_set_textposition __(( PT3 DUMMY0 )) ;
extern  void    w3d_moveto __(( PT3 DUMMY0 )) ;
extern  void    w3d_lineto __(( PT3 DUMMY0 )) ;
extern  void    w3d_curve __(( C3_CURVE DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  void    w3d_point __(( PT3 DUMMY0 , REAL DUMMY1 )) ;
extern  VP_VIEWPORT w3d_select __(( C3_CURVE DUMMY0 , PT3 DUMMY1 ,
            PARM DUMMY2 , REAL* DUMMY3 )) ;
extern  void    w3d_poly __(( PT3* DUMMY0 , INT DUMMY1 )) ;
extern  void    w3d_curve_dir __(( C3_CURVE DUMMY0 , INT DUMMY1 )) ;
extern  void    w3d_curve_box __(( C3_CURVE DUMMY0 )) ;
extern  void    w3d_box __(( C3_BOX DUMMY0 )) ;
extern  void    w3d_arrow __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 )) ;
#endif /* W3DDEFS */



