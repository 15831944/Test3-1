//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3QD.C *********************************/
/**************** Routines for processing polycurve geometry ***************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <aladefs.h>
#include <c3qdefs.h>
#include <c3rdefs.h>
#include <c3vmcrs.h>
#include <c3qmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_create ( buffer, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
INT s ;
{
    if ( buffer == NULL ) 
        buffer = MALLOC ( 1, C3_PC_BUFFER_S ) ;
    if ( buffer==NULL ) 
        RETURN ( NULL ) ;
    c3q_initialize ( buffer, s, 0 ) ;
    RETURN ( buffer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_initialize ( buffer, s, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
INT s, n ;
{
    C3_PC_BUFFER_SIZE(buffer) = s ;
    C3_PC_BUFFER_N(buffer) = n ;
    C3_PC_BUFFER_A(buffer) = ( s > 0 ) ? MALLOC ( 2*s-1, PT3 ) : NULL ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_free ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    c3q_clear ( buffer ) ;
    FREE ( buffer ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_clear ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    if ( buffer == NULL ) 
        RETURN ;
    FREE ( C3_PC_BUFFER_A(buffer) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3q_copy ( buffer0, buffer1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer0, buffer1 ;
{
    C3_PC_BUFFER_N(buffer1) = C3_PC_BUFFER_N(buffer0) ;
    ala_copy ( (REAL*)C3_PC_BUFFER_A(buffer0), 3*(2*C3_PC_BUFFER_N(buffer0)-1), 
        (REAL*)C3_PC_BUFFER_A(buffer1) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_create_copy ( buffer0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer0 ;
{
    C3_PC_BUFFER buffer1 = c3q_create ( NULL, C3_PC_BUFFER_SIZE(buffer0) ) ;
    if ( buffer1 != NULL ) 
        c3q_copy ( buffer0, buffer1 ) ;
    RETURN ( buffer1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_check_size ( buffer, add_size ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
INT add_size ;
{
    if ( C3_PC_BUFFER_N(buffer) + add_size <= C3_PC_BUFFER_SIZE(buffer) )
        RETURN ( buffer ) ;
    C3_PC_BUFFER_SIZE(buffer) = C3_PC_BUFFER_N(buffer) + add_size + 10 ;
    REALLOC ( C3_PC_BUFFER_A(buffer), 2*C3_PC_BUFFER_SIZE(buffer)+1, PT3 ) ;
    if ( C3_PC_BUFFER_A(buffer)==NULL ) {
        FREE ( buffer ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_add_arc_2pts ( buffer, a0, a1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 a0, a1 ;
{
    if ( c3q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    c3r_3pts ( C3_PC_BUFFER_PT1(buffer), a0, a1, 
        C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-1) ) ;
    C3_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_add_arc_ctr_pt ( buffer, ctr, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 ctr, pt ;
{
    if ( c3q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    c3r_2pts_ctr ( C3_PC_BUFFER_PT1(buffer), pt, ctr, 
        C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-1) ) ;
    C3_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_add_arc_tan ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 a ;
{
    PT3 tan ;

    if ( c3q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    c3r_tan1 ( C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-2), tan ) ;
    c3r_2pts_tan ( C3_PC_BUFFER_PT1(buffer), tan, a, 
        C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-1) ) ;
    C3_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_add_line ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 a ;
{
    if ( c3q_check_size ( buffer, 1 ) == NULL )
        RETURN ( NULL ) ;
    c3r_line ( C3_PC_BUFFER_PT1(buffer), a, 
        C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-1) ) ;
    C3_PC_BUFFER_N(buffer) += 1 ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_add_line_tan ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 a ;
{
    PT3 p, vec, tan ;
    REAL t ;

    c3r_tan1 ( C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-2), tan ) ;
    C3V_SUB ( a, C3_PC_BUFFER_PT1(buffer), vec ) ;
    t = C3V_DOT ( vec, tan ) / C3V_DOT ( tan, tan ) ;
    C3V_ADDT ( C3_PC_BUFFER_PT1(buffer), tan, t, p ) ;
    RETURN ( c3q_add_line ( buffer, p ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_remove_last ( buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    C3_PC_BUFFER_N(buffer)-- ;
    RETURN ( C3_PC_BUFFER_N(buffer) > 0 ? buffer : NULL ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_close ( buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    RETURN ( c3q_add_line ( buffer, C3_PC_BUFFER_PT0(buffer) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_trim ( buffer0, t0, t1, j0, buffer1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer0, buffer1 ;
REAL t0, t1 ;
INT j0 ;
{
    INT i0, i1, n, i ;

    if ( buffer0 == NULL || C3_PC_BUFFER_N(buffer0) <= 0 ) 
        RETURN ( buffer1 ) ;
    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;
    if ( t0 <= 0.0 ) 
        t0 = 0.0 ;
    if ( t1 > (REAL) ( C3_PC_BUFFER_N(buffer0) - 1 ) )
        t1 = (REAL) ( C3_PC_BUFFER_N(buffer0) - 1 ) ;
    if ( t0 >= (REAL) ( C3_PC_BUFFER_N(buffer0) - 1 ) || t1 <= 0.0 )
        RETURN ( NULL ) ;

    i0 = (INT)(t0+BBS_ZERO) ;
    i1 = (INT)(t1-BBS_ZERO) ;
    t0 -= (REAL)i0 ;
    t1 -= (REAL)i1 ;
    n = i1 - i0 + 2 ;
    buffer1 = c3q_create ( buffer1, n ) ;
    C3_PC_BUFFER_N(buffer1) = n ;
    if ( i0 == i1 ) 
        c3r_trim ( C3_PC_BUFFER_ARC(buffer0,i0), t0, t1, 
            C3_PC_BUFFER_ARC(buffer1,0) ) ;
    else {
        c3r_trim ( C3_PC_BUFFER_ARC(buffer0,i0), t0, 1.0, 
            C3_PC_BUFFER_ARC0(buffer1) ) ;
        for ( i=1 ; i<n-2 ; i++ ) {
            C3V_COPY ( C3_PC_BUFFER_PT(buffer0,i0+i), 
                C3_PC_BUFFER_PT(buffer1,i) ) ;
            C3V_COPY ( C3_PC_BUFFER_D_VEC(buffer0,i0+i), 
                C3_PC_BUFFER_D_VEC(buffer1,i) ) ;
        }
        c3r_trim ( C3_PC_BUFFER_ARC(buffer0,i1), 0.0, t1, 
            C3_PC_BUFFER_ARC1(buffer1) ) ;
    }
    RETURN ( buffer1 ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3q_through ( a, n, buffer ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
C3_PC_BUFFER buffer ;
{
    INT i ;

    for ( i=0 ; i<n-1 ; i++ ) {
        C3V_COPY ( a[i], C3_PC_BUFFER_PT(buffer,i) ) ;
        C3V_SET_ZERO ( C3_PC_BUFFER_D_VEC(buffer,i) ) ;
    }
    C3V_COPY ( a[n-1], C3_PC_BUFFER_PT(buffer,n-1) ) ;
    RETURN ( buffer ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3q_init ( buffer, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 a ;
{
    C3V_COPY ( a, C3_PC_BUFFER_PT0(buffer) ) ;
}

