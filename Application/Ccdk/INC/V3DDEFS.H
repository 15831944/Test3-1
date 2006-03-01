//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* V3DDEFS.H *********************************/
/*********************** Display of 3-dim curves ***************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef V3DDEFS
#define V3DDEFS

#include <vpdefs.h>
#include <c3defs.h>

#define V3D_PT_TRANSFORM(A,VP,B)    {\
B[0]=VP_VIEWPORT_TRANSFORM(VP)[0][0]*A[0]+VP_VIEWPORT_TRANSFORM(VP)[0][1]*A[1]\
+VP_VIEWPORT_TRANSFORM(VP)[0][2]*A[2]+VP_VIEWPORT_TRANSFORM(VP)[0][3];\
B[1]=VP_VIEWPORT_TRANSFORM(VP)[1][0]*A[0]+VP_VIEWPORT_TRANSFORM(VP)[1][1]*A[1]\
+VP_VIEWPORT_TRANSFORM(VP)[1][2]*A[2]+VP_VIEWPORT_TRANSFORM(VP)[1][3];\
if ( !VP_VIEWPORT_CLIPPED(VP) ) {\
B[0] += VP_VIEWPORT_PX0(VP) ; B[1] += VP_VIEWPORT_PY0(VP);}\
VP_VIEWPORT_PREV_X(VP)=A[0];VP_VIEWPORT_PREV_Y(VP)=A[1];\
VP_VIEWPORT_PREV_Z(VP)=A[2];}

extern  void    v3d_set_textposition __(( PT3 DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
extern  void    v3d_moveto __(( PT3 DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
extern  void    v3d_lineto __(( PT3 DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
extern  void    v3d_curve __(( C3_CURVE DUMMY0 , PARM DUMMY1 , PARM DUMMY2 ,
            VP_VIEWPORT DUMMY3 )) ;
extern  void    v3d_point __(( PT3 DUMMY0 , REAL DUMMY1 , 
            VP_VIEWPORT DUMMY2 )) ;
extern  BOOLEAN v3d_select __(( C3_CURVE DUMMY0 , PT3 DUMMY1 , 
            VP_VIEWPORT DUMMY2 , PARM DUMMY3 , REAL* DUMMY4 )) ;
extern  void    v3d_poly __(( PT3* DUMMY0 , INT DUMMY1 , VP_VIEWPORT DUMMY2 )) ;
extern  void    v3d_curve_dir __(( C3_CURVE DUMMY0 , INT DUMMY1 ,
            VP_VIEWPORT DUMMY2 )) ;
extern  void    v3d_curve_box __(( C3_CURVE DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
extern  void    v3d_box __(( C3_BOX DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
extern  void    v3d_arrow __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , VP_VIEWPORT DUMMY5 )) ;
extern  BOOLEAN v3d_pt_inside __(( VP_VIEWPORT DUMMY0 , PT3 DUMMY1 )) ;

#endif /* V3DDEFS */


