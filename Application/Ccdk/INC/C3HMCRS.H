//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3HMCRS.H *********************************/
/*************** Include file for 3d definitions and macros ****************/
/***************** Homogeneous vector manipulation macros ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3HMCRS
#define     C3HMCRS
#ifdef      QGLMACROS
#ifdef      SPLINE

#define C3H_SET_ZERO(A) {(A)[0]=0.0;(A)[1]=0.0;(A)[2]=0.0;(A)[3]=1.0;}
#define C3H_SET(X,Y,Z,W,A) {(A)[0]=(X);(A)[1]=(Y);(A)[2]=(Z);(A)[3]=(W);}
#define C3H_COPY(A,B)   \
{(B)[0]=(A)[0];(B)[1]=(A)[1];(B)[2]=(A)[2];(B)[3]=(A)[3];}
#define C3H_DIFF(A,B,C) {(C)[0]=(A)[0]*(B)[3]-(B)[0]*(A)[3];\
(C)[1]=(A)[1]*(B)[3]-(B)[1]*(A)[3];(C)[2]=(A)[2]*(B)[3]-(B)[2]*(A)[3];} 
#define C3H_SUB(A,V,B)  {(B)[0]=(A)[0]-V[0]*(A)[3];\
(B)[1]=(A)[1]-V[1]*(A)[3];(B)[2]=(A)[2]-V[2]*(A)[3];}
#define C3H_HPT_PT(A,B)  \
{(B)[0]=(A)[0]/(A)[3];(B)[1]=(A)[1]/(A)[3];(B)[2]=(A)[2]/(A)[3];}
#define C3H_PT_HPT(A,B)  \
{(B)[0]=(A)[0];(B)[1]=(A)[1];(B)[2]=(A)[2];(B)[3]=1.0;}
#define C3H_ADDV(A,V,B)  {(B)[0]=(A)[0]+(V)[0]*(A)[3];\
(B)[1]=(A)[1]+(V)[1]*(A)[3];(B)[2]=(A)[2]+(V)[2]*(A)[3];(B)[3]=(A)[3];}

#else       /* !QGLMACROS */
#include    <c3hdefs.h>

#define     C3H_SET_ZERO    c3h_set_zero        
#define     C3H_COPY        c3h_copy        
#define     C3H_DIFF        c3h_diff        
#define     C3H_SUB         c3h_sub     
#define     C3H_HPT_PT      c3h_hpt_pt

#endif  /* SPLINE */
#endif  /* QGLMACROS*/

#endif  /* C3HMCRS*/


