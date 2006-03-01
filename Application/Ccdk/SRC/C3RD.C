//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3RD.C *********************************/
/***************** Routines for constructing arc geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3rdefs.h>
#include <c3vdefs.h>
#include <c3vpriv.h>
#include <c3rmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_epts_d ( pt0, pt1, d_vec, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, d_vec ;
C3_ASEG arc ;
{
// cnm
//    C3V_COPY ( pt0, C3_ASEG_PT0(arc) ) ;
//    C3V_COPY ( pt1, C3_ASEG_PT1(arc) ) ;
//    C3V_COPY ( d_vec, C3_ASEG_D_VEC(arc) ) ;
    C3V_COPY ( C3_ASEG_PT0(arc) , pt0) ;
    C3V_COPY ( C3_ASEG_PT1(arc) , pt1) ;
    C3V_COPY ( C3_ASEG_D_VEC(arc) , d_vec) ;
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_3pts ( pt0, mid_pt, pt1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, mid_pt ;
C3_ASEG arc ;
{
    REAL p, q, z, v, t ;
    PT3 c, vec ;

    C3V_COPY ( pt0, C3_ASEG_PT0(arc) ) ;
    C3V_COPY ( pt1, C3_ASEG_PT1(arc) ) ;
    p = c3v_coord ( pt0, pt1, mid_pt, c ) ;
    C3V_SUB ( pt1, pt0, vec ) ;
    v = C3V_DOT ( vec, vec ) ;
    if ( C3V_IS_SMALL ( c ) ) {
        C3V_SET_ZERO ( C3_ASEG_D_VEC(arc) ) ;
    }
    else {
        z = C3V_DOT ( c, c ) ;
        t = z - p * ( 1.0 - p ) * v ;
        q = sqrt ( v ) / ( sqrt ( t*t + v * z ) - t ) ;
        C3V_SCALE ( c, q, C3_ASEG_D_VEC(arc) ) ;
    }
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_2pts_tan ( pt0, tan0, pt1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, tan0 ;
C3_ASEG arc ;
{
    REAL a, b, c, z, p, q, a1, c1 ;
    PT3 vec ;
    C3V_SUB ( pt1, pt0, vec ) ;

    a = C3V_DOT ( vec, vec ) ;
    b = C3V_DOT ( vec, tan0 ) ;
    c = C3V_DOT ( tan0, tan0 ) ;
    a1 = sqrt ( a ) ;
    c1 = sqrt ( c ) ;
    z = a1 * c1 + b ;
    p = - b / ( a1 * z ) ;
    q = a1 / z ;
    C3V_COPY ( pt0, C3_ASEG_PT0(arc) ) ;
    C3V_COPY ( pt1, C3_ASEG_PT1(arc) ) ;
    C3V_ADDW ( vec, p, tan0, q, C3_ASEG_D_VEC(arc) ) ;
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_circle ( ctr, rad, normal, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 ctr, normal ;
REAL rad ;
C3_ASEG arc ;
{
    PT3 x ;
    (void) c3v_basis ( normal, x, C3_ASEG_D_VEC(arc) ) ;
    C3V_SCALE ( x, rad, x ) ;
    C3V_ADD ( ctr, x, C3_ASEG_PT0(arc) ) ;
    C3V_SUB ( ctr, x, C3_ASEG_PT1(arc) ) ;
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_2pts_ctr ( pt0, pt1, ctr, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1, ctr ;
C3_ASEG arc ;
{
    REAL z, v, p ;
    PT3 vec ;

    C3V_COPY ( pt0, C3_ASEG_PT0(arc) ) ;
    C3V_COPY ( pt1, C3_ASEG_PT1(arc) ) ;
    if ( !IS_SMALL ( c3v_coord ( pt0, pt1, ctr, C3_ASEG_D_VEC(arc) ) - 0.5 ) )
        RETURN ( NULL ) ;
    z = C3V_DOT ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) ;
    C3V_SUB ( pt1, pt0, vec ) ;
    v = C3V_DOT ( vec, vec ) ;
    p = - sqrt ( v ) / ( 2.0 * z + sqrt ( z * ( 4.0 * z + v ) ) ) ;
    C3V_SCALE ( C3_ASEG_D_VEC(arc), p, C3_ASEG_D_VEC(arc) ) ;
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_line ( pt0, pt1, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt0, pt1 ;
C3_ASEG arc ;
{
    C3V_COPY ( pt0, C3_ASEG_PT0(arc) ) ;
    C3V_COPY ( pt1, C3_ASEG_PT1(arc) ) ;
    C3V_SET_ZERO ( C3_ASEG_D_VEC(arc) ) ;
    RETURN ( arc ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_copy ( arc0, arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc0, arc ;
{
    if ( arc0 == NULL || arc == NULL ) 
        RETURN ( NULL ) ;
    C3V_COPY ( C3_ASEG_PT0(arc0), C3_ASEG_PT0(arc) ) ;
    C3V_COPY ( C3_ASEG_PT1(arc0), C3_ASEG_PT1(arc) ) ;
    C3V_COPY ( C3_ASEG_D_VEC(arc0), C3_ASEG_D_VEC(arc) ) ;
    RETURN ( arc ) ;
}

