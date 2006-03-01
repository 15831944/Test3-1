//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3VMCRS.H *********************************/
/******************Euclidean 3d vector manipulation macros *****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3VMCRS
#define     C3VMCRS
#ifdef      QGLMACROS

#define     C3V_IS_ZERO(A)  (IS_ZERO((A)[0])&&IS_ZERO((A)[1])&&IS_ZERO((A)[2]))
#define     C3V_IS_SMALL(A)  \
(IS_SMALL((A)[0])&&IS_SMALL((A)[1])&&IS_SMALL((A)[2]))
#define     C3V_IDENT_PTS(A,B)  \
(IS_SMALL((A)[0]-(B)[0])&&IS_SMALL((A)[1]-(B)[1])&&IS_SMALL((A)[2]-(B)[2]))
#define     C3V_SET_ZERO(A) {(A)[0]=0.0;(A)[1]=0.0;(A)[2]=0.0;}
#define     C3V_SET(X,Y,Z,A) {(A)[0]=(X);(A)[1]=(Y);(A)[2]=Z;}
#define     C3V_COPY(A,B)   {(B)[0]=(A)[0];(B)[1]=(A)[1];(B)[2]=(A)[2];}
#define     C3V_ADD(A,B,C)  \
{(C)[0]=(A)[0]+(B)[0];(C)[1]=(A)[1]+(B)[1];(C)[2]=(A)[2]+(B)[2];}
#define     C3V_ADDT(A,B,T,C)  \
{(C)[0]=(A)[0]+(T)*(B)[0];(C)[1]=(A)[1]+(T)*(B)[1];(C)[2]=(A)[2]+(T)*(B)[2];}
#define     C3V_ADDW(A,P,B,Q,C) {(C)[0]=(P)*(A)[0]+(Q)*(B)[0];\
(C)[1]=(P)*(A)[1]+(Q)*(B)[1];(C)[2]=(P)*(A)[2]+(Q)*(B)[2];}
#define     C3V_ADDC(C,A,P,B,Q,X) {(X)[0]=(C)[0]+(P)*(A)[0]+(Q)*(B)[0];\
(X)[1]=(C)[1]+(P)*(A)[1]+(Q)*(B)[1];(X)[2]=(C)[2]+(P)*(A)[2]+(Q)*(B)[2];}
#define     C3V_ADDU(A,B,U,C) {(C)[0]=(1.0-(U))*(A)[0]+(U)*(B)[0];\
(C)[1]=(1.0-(U))*(A)[1]+(U)*(B)[1];(C)[2]=(1.0-(U))*(A)[2]+(U)*(B)[2];}
#define     C3V_SUB(A,B,C)  \
{(C)[0]=(A)[0]-(B)[0];(C)[1]=(A)[1]-(B)[1];(C)[2]=(A)[2]-(B)[2];}
#define     C3V_NEGATE(A,B)  {(B)[0]= -(A)[0];(B)[1]= -(A)[1];(B)[2]= -(A)[2];}
#define     C3V_MID_PT(A,B,C) {(C)[0]=0.5*((A)[0]+(B)[0]);\
(C)[1]=0.5*((A)[1]+(B)[1]);(C)[2]=0.5*((A)[2]+(B)[2]);}
#define     C3V_SCALE(A,T,B) \
{(B)[0]=(T)*(A)[0];(B)[1]=(T)*(A)[1];(B)[2]=(T)*(A)[2];}
#define     C3V_DOT(A,B)    ((A)[0]*(B)[0]+(A)[1]*(B)[1]+(A)[2]*(B)[2])
#define     C3V_TRIPLE_PROD(A,B,C) ((C)[0]*((A)[1]*(B)[2]-(A)[2]*(B)[1])+\
(C)[1]*((A)[2]*(B)[0]-(A)[0]*(B)[2])+(C)[2]*((A)[0]*(B)[1]-(A)[1]*(B)[0]))
#define     C3V_CROSS(A,B,C)  {(C)[0]=(A)[1]*(B)[2]-(A)[2]*(B)[1];\
(C)[1]=(A)[2]*(B)[0]-(A)[0]*(B)[2];(C)[2]=(A)[0]*(B)[1]-(A)[1]*(B)[0];}
#define     C3V_NORM(A)     (sqrt((A)[0]*(A)[0]+(A)[1]*(A)[1]+(A)[2]*(A)[2]))
#define     C3V_NORM_SQUARED(A) ((A)[0]*(A)[0]+(A)[1]*(A)[1]+(A)[2]*(A)[2])
#define     C3V_NORML1(A)   (fabs((A)[0])+fabs((A)[1])+fabs((A)[2]))
#define     C3V_DIST(A,B)   (sqrt(((A)[0]-(B)[0])*((A)[0]-(B)[0])+\
((A)[1]-(B)[1])*((A)[1]-(B)[1])+((A)[2]-(B)[2])*((A)[2]-(B)[2])))
#define     C3V_DISTL1(A,B) \
(fabs((A)[0]-(B)[0])+fabs((A)[1]-(B)[1])+fabs((A)[2]-(B)[2]))

#else       /* !QGLMACROS */

#include    <c3vdefs.h>

#define     C3V_IS_ZERO     c3v_is_zero 
#define     C3V_IS_SMALL    c3v_is_small    
#define     C3V_IDENT_PTS   c3v_ident_pts   
#define     C3V_SET_ZERO    c3v_set_zero    
#define     C3V_SET         c3v_set         
#define     C3V_COPY        c3v_copy        
#define     C3V_ADD         c3v_add         
#define     C3V_ADDT        c3v_addt        
#define     C3V_ADDW        c3v_addw        
#define     C3V_ADDU        c3v_addu        
#define     C3V_SUB         c3v_sub     
#define     C3V_NEGATE      c3v_negate      
#define     C3V_MID_PT      c3v_mid_pt      
#define     C3V_SCALE       c3v_scale       
#define     C3V_DOT         c3v_dot     
#define     C3V_TRIPLE_PROD c3v_triple_prod
#define     C3V_CROSS       c3v_cross       
#define     C3V_NORM        c3v_norm        
#define     C3V_NORM_SQUARED c3v_norm_squared        
#define     C3V_NORML1      c3v_norml1      
#define     C3V_DIST        c3v_dist        
#define     C3V_DISTL1      c3v_distl1      

#endif /* QGLMACROS*/

#endif /* C3VMCRS*/



