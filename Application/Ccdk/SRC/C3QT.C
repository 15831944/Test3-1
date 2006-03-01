//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3QT.C *********************************/
/***************** Routines for processing polycurve geometry **************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3qdefs.h>
#include <c3rdefs.h>
#include <c3tdefs.h>
#include <c3vdefs.h>
#include <c3vmcrs.h>
#include <c3qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_translate ( buffer, shift ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 shift ;
{
    INT i ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) {
        C3V_ADD ( C3_PC_BUFFER_PT(buffer,i), shift, 
            C3_PC_BUFFER_PT(buffer,i) ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_scale ( buffer, origin, factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 origin ;
REAL factor ;
{
    INT i ;
    PT3 a ;
    REAL t = 1.0 - factor ;

    C3V_SCALE ( origin, t, a ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) 
        C3V_ADDT ( a, C3_PC_BUFFER_PT(buffer,i), 
            factor, C3_PC_BUFFER_PT(buffer,i) ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        C3V_SCALE ( C3_PC_BUFFER_D_VEC(buffer,i), 
            factor, C3_PC_BUFFER_D_VEC(buffer,i) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_mirror ( buffer, n, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 n ;
REAL d ;
{
    INT i ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) 
        c3v_mirror_pt ( C3_PC_BUFFER_PT(buffer,i), 
            n, d, C3_PC_BUFFER_PT(buffer,i) ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        c3v_mirror_vec ( C3_PC_BUFFER_D_VEC(buffer,i), 
            n, C3_PC_BUFFER_D_VEC(buffer,i) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3q_rotate_cs ( buffer, origin, axis, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 origin, axis ;
REAL c, s ;
{
    INT i ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) 
        c3v_rotate_pt_cs ( C3_PC_BUFFER_PT(buffer,i), 
            origin, axis, c, s, C3_PC_BUFFER_PT(buffer,i) ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        c3v_rotate_vec_cs ( C3_PC_BUFFER_D_VEC(buffer,i), 
            axis, c, s, C3_PC_BUFFER_D_VEC(buffer,i) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_project_oblique ( buffer, normal, dist, direction ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 normal, direction ;
REAL dist ;
{
    INT i ;
    if ( !c3v_vecs_parallel ( normal, direction ) ) {
        for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
            if ( !C3V_IS_SMALL ( C3_PC_BUFFER_D_VEC(buffer,i) ) ) 
                RETURN ( FALSE ) ;
    }

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) 
        if ( !c3v_project_pt_oblique ( C3_PC_BUFFER_PT(buffer,i), 
            normal, dist, direction, C3_PC_BUFFER_PT(buffer,i) ) )
            RETURN ( FALSE ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        if ( !c3v_project_vec_oblique ( C3_PC_BUFFER_D_VEC(buffer,i), 
            normal, direction, C3_PC_BUFFER_D_VEC(buffer,i) ) )
            RETURN ( FALSE ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_transform ( buffer, t, t_orth ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
C3_TRANSFORM t ;
BOOLEAN t_orth ;
{
    INT i ;
    if ( !t_orth ) 
        for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        if ( !C3V_IS_SMALL(C3_PC_BUFFER_D_VEC(buffer,i)) )
            RETURN ( FALSE ) ;

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) 
        c3t_eval_pt ( C3_PC_BUFFER_PT(buffer,i), t, 
            C3_PC_BUFFER_PT(buffer,i) ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        c3t_eval_vec ( C3_PC_BUFFER_D_VEC(buffer,i), 
            t, C3_PC_BUFFER_D_VEC(buffer,i) ) ;
    RETURN ( TRUE ) ;
}


