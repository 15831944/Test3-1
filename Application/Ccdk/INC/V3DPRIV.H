//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* V3DPRIV.H *********************************/
/*********************** Display of 3-dim curves ***************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef V3DPRIV
#define V3DPRIV

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

extern  void    v3d_buffer __(( PT3* DUMMY0 , INT DUMMY1 , 
        VP_VIEWPORT DUMMY2 )) ;
#ifdef SPLINE
extern  void    v3d_hpoly __(( HPT3* DUMMY0 , INT DUMMY1 , 
            VP_VIEWPORT DUMMY2 )) ;
#endif
extern  void    v3d_ctl_poly __(( C3_CURVE DUMMY0 , VP_VIEWPORT DUMMY1 )) ;
#endif /* V3DDEFS */

