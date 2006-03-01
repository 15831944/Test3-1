//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3TDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C3TDEFS
#define     C3TDEFS
#include    <c3defs.h>
#include    <c2defs.h> 

extern  REAL*       c3t_eval_2d __(( PT2 DUMMY0 , C3_TRANSFORM DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  REAL*       c3t_eval_pt __(( PT3 DUMMY0 , C3_TRANSFORM DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  REAL*       c3t_eval_vec __(( PT3 DUMMY0 , C3_TRANSFORM DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  REAL* c3t_copy __(( C3_TRANSFORM DUMMY0, C3_TRANSFORM DUMMY1 )) ;
extern  C3_TRANSFORM* c3t_create __(( void )) ;
extern  REAL*       c3t_lcs __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            C3_TRANSFORM DUMMY3 )) ;
extern  void        c3t_free __(( C3_TRANSFORM DUMMY0 )) ;
extern  REAL*       c3t_mult __(( C3_TRANSFORM DUMMY0 , C3_TRANSFORM DUMMY1 ,
            C3_TRANSFORM DUMMY2 )) ;
extern  BOOLEAN     c3t_orthogonal __(( C3_TRANSFORM DUMMY0 )) ;
extern  REAL*       c3t_translate __(( PT3 DUMMY0 , C3_TRANSFORM DUMMY1 )) ;
extern  REAL*       c3t_rotate __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            C3_TRANSFORM DUMMY3 )) ;
extern  REAL*       c3t_rotate_cs __(( PT3 DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , C3_TRANSFORM DUMMY4 )) ;
extern  REAL*       c3t_scale __(( PT3 DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , C3_TRANSFORM DUMMY4 )) ;
extern  REAL*       c3t_mirror __(( PT3 DUMMY0 , REAL DUMMY1 , 
            C3_TRANSFORM DUMMY2 )) ;
extern  REAL*       c3t_inverse __(( C3_TRANSFORM DUMMY0 , 
            C3_TRANSFORM DUMMY1 )) ;
extern  BOOLEAN c3t_positive __(( C3_TRANSFORM DUMMY1 )) ;
extern  REAL c3t_det __(( C3_TRANSFORM DUMMY1 )) ;
#endif /* C3TDEFS*/

