//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3RT.C *********************************/
/******************* Routines for processing arc geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3rdefs.h>
#include <c3tdefs.h>
#include <c3vdefs.h>
#include <c3rmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_translate ( arc, shift ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 shift ;
{
    C3V_ADD ( C3_ASEG_PT0(arc), shift, C3_ASEG_PT0(arc) ) ;
    C3V_ADD ( C3_ASEG_PT1(arc), shift, C3_ASEG_PT1(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_scale ( arc, origin, factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 origin ;
REAL factor ;
{
    PT3 a ;
    REAL t = 1.0 - factor ;

    C3V_SCALE ( origin, t, a ) ;

    C3V_ADDT ( a, C3_ASEG_PT0(arc), factor, C3_ASEG_PT0(arc) ) ;
    C3V_ADDT ( a, C3_ASEG_PT1(arc), factor, C3_ASEG_PT1(arc) ) ;
    if ( factor < 0.0 ) 
        C3V_NEGATE ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_mirror ( arc, n, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 n ;
REAL d ;
{
    REAL norm ;
    c3v_mirror_pt ( C3_ASEG_PT0(arc), n, d, C3_ASEG_PT0(arc) ) ;
    c3v_mirror_pt ( C3_ASEG_PT1(arc), n, d, C3_ASEG_PT1(arc) ) ;
    norm = C3V_NORM ( C3_ASEG_D_VEC(arc) ) ;
    c3v_mirror_vec ( C3_ASEG_D_VEC(arc), n, C3_ASEG_D_VEC(arc) ) ;
    c3v_normalize ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) ;
    C3V_SCALE ( C3_ASEG_D_VEC(arc), norm, C3_ASEG_D_VEC(arc) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3r_rotate_cs ( arc, origin, axis, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 origin, axis ;
REAL c, s ;
{
    PT3 nrmd_axis ;
    c3v_normalize ( axis, nrmd_axis ) ;
#ifdef DEBUG
{
    PT2 p ; 
    c3r_pt_tan ( arc, 0.5, p, NULL ) ;
    c3v_rotate_pt_cs ( p, origin, axis, c, s, p ) ;
}
#endif
    c3v_rotate_pt_cs ( C3_ASEG_PT0(arc), origin, axis, c, s, 
        C3_ASEG_PT0(arc) ) ;
    c3v_rotate_pt_cs ( C3_ASEG_PT1(arc), origin, axis, c, s, 
        C3_ASEG_PT1(arc) ) ;
    c3v_rotate_vec_cs ( C3_ASEG_D_VEC(arc), axis, c, s, C3_ASEG_D_VEC(arc) ) ;
#ifdef DEBUG
{
    PT2 p ; 
    c3r_pt_tan ( arc, 0.5, p, NULL ) ;
}
#endif
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_project_oblique ( arc, normal, dist, direction ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 normal, direction ;
REAL dist ;
{
    REAL norm ;

    if ( !c3r_is_normal ( arc, normal ) ) 
        RETURN ( FALSE ) ;

    if ( !c3v_project_pt_oblique ( C3_ASEG_PT0(arc), normal, dist, 
        direction, C3_ASEG_PT0(arc) ) )
        RETURN ( FALSE ) ;
    if ( !c3v_project_pt_oblique ( C3_ASEG_PT1(arc), normal, dist, 
        direction, C3_ASEG_PT1(arc) ) )
        RETURN ( FALSE ) ;
    norm = C3V_NORM ( C3_ASEG_D_VEC(arc) ) ;
    if ( !c3v_project_vec_oblique ( C3_ASEG_D_VEC(arc), normal, 
        direction, C3_ASEG_D_VEC(arc) ) ) 
        RETURN ( FALSE ) ;
    c3v_normalize ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) ;
    C3V_SCALE ( C3_ASEG_D_VEC(arc), norm, C3_ASEG_D_VEC(arc) ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_transform ( arc, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
C3_TRANSFORM t ;
{
    REAL norm ; 
    if ( !c3t_orthogonal ( t ) )
        RETURN ( FALSE ) ;
    c3t_eval_pt ( C3_ASEG_PT0(arc), t, C3_ASEG_PT0(arc) ) ;
    c3t_eval_pt ( C3_ASEG_PT1(arc), t, C3_ASEG_PT1(arc) ) ;
    norm = C3V_NORM ( C3_ASEG_D_VEC(arc) ) ;
    c3t_eval_vec ( C3_ASEG_D_VEC(arc), t, C3_ASEG_D_VEC(arc) ) ;
    c3v_normalize ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) ;
    if ( !c3t_positive ( t ) ) 
        norm = - norm ;
    C3V_SCALE ( C3_ASEG_D_VEC(arc), norm, C3_ASEG_D_VEC(arc) ) ;
    RETURN ( TRUE ) ;
}

