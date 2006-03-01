//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3P.C **********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3pdefs.h>
#include <c3rdefs.h>
#include <c3qdefs.h>
#include <c3pmcrs.h>
#include <c3vmcrs.h>
#include <dmldefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_eval ( pcurve, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t ;
INT p ;
PT3 *x ;
{
    BOOLEAN status = TRUE ;
    DML_ITEM item ;

    if ( t < 0.0 ) {
        t = 0.0 ;
        status = FALSE ;
    }
    if ( c3q_eval ( C3_PCURVE_BUFFER(pcurve), t, p, x ) )
        RETURN ( status ) ;
    C3_WALK_PCURVE ( pcurve, item ) { 
        if ( c3q_eval ( DML_RECORD(item), t, p, x ) )
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3p_n ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    INT n ;
    DML_ITEM item ;

    n = c3q_n ( C3_PCURVE_BUFFER(pcurve) ) - 1 ;
    C3_WALK_PCURVE ( pcurve, item ) 
        n += ( c3q_n ( DML_RECORD(item) ) - 1 ) ;
    n++ ;
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3p_segment ( pcurve, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
INT i ;
{
    DML_ITEM item ;
    C3_ASEG arc ;

    if ( i < 0 ) 
        RETURN ( NULL ) ;
    arc = c3q_segment ( C3_PCURVE_BUFFER(pcurve), &i ) ;
    if ( arc != NULL ) 
        RETURN ( arc ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        arc = c3q_segment ( DML_RECORD(item), &i ) ;
        if ( arc != NULL ) 
            RETURN ( arc ) ;
    }
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_segment_epts_d ( pcurve, i, ept0, ept1, d_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
INT i ;
PT3 ept0, ept1, d_vec ;
{
    C3_ASEG arc = c3p_segment ( pcurve, i ) ;
    if ( arc == NULL ) 
        RETURN ( FALSE ) ;
    else {
        c3r_epts_d ( ept0, ept1, d_vec, arc ) ;
        RETURN ( TRUE ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3p_first_segm ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    C3_ASEG arc ;

    arc = c3q_first_segm ( C3_PCURVE_BUFFER(pcurve) ) ;
    RETURN ( arc != NULL ? arc :
        c3q_first_segm ( dml_first_record ( C3_PCURVE_BLIST(pcurve) ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3p_last_segm ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    C3_ASEG arc ;
    arc = c3q_last_segm ( dml_last_record ( C3_PCURVE_BLIST(pcurve) ) ) ;
    RETURN ( arc != NULL ? arc : c3q_last_segm ( C3_PCURVE_BUFFER(pcurve) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3p_first_buffer ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    C3_PC_BUFFER buffer ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    if ( c3q_n ( buffer ) > 0 ) 
        RETURN ( buffer ) ;
    else 
        RETURN ( (C3_PC_BUFFER)dml_first_record ( C3_PCURVE_BLIST(pcurve) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PC_BUFFER c3p_last_buffer ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    C3_PC_BUFFER buffer ;
    buffer = (C3_PC_BUFFER)dml_last_record ( C3_PCURVE_BLIST(pcurve) ) ;
    if ( buffer != NULL && c3q_n ( buffer ) > 0 ) 
        RETURN ( buffer ) ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    RETURN ( c3q_n ( buffer ) > 0 ? buffer : NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_pt_tan ( pcurve, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t ;
PT3 pt, tan_vec ;
{
    BOOLEAN status = TRUE ;
    DML_ITEM item ;

    if ( t < 0.0 ) {
        t = 0.0 ;
        status = FALSE ;
    }
    if ( c3q_pt_tan ( C3_PCURVE_BUFFER(pcurve), t, pt, tan_vec ) )
        RETURN ( status ) ;
    C3_WALK_PCURVE ( pcurve, item ) { 
        if ( c3q_pt_tan ( DML_RECORD(item), t, pt, tan_vec ) )
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3p_curvature ( pcurve, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t ;
{
    DML_ITEM item ;
    REAL crv ;

    if ( t < 0.0 ) 
        t = 0.0 ;
    if ( c3q_curvature ( C3_PCURVE_BUFFER(pcurve), t, &crv ) )
        RETURN ( crv ) ;
    C3_WALK_PCURVE ( pcurve, item ) { 
        if ( c3q_curvature ( DML_RECORD(item), t, &crv ) )
            RETURN ( crv ) ;
    }
    RETURN ( 0.0 ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c3p_box ( pcurve, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_PCURVE pcurve ;
C3_BOX box ;
{
    BOOLEAN status ;
    DML_ITEM item ;

    status = c3q_box ( C3_PCURVE_BUFFER(pcurve), box ) ;
    C3_WALK_PCURVE ( pcurve, item ) { 
        if ( status ) 
            c3q_box_append ( DML_RECORD(item), box ) ;
        else
            status = c3q_box ( DML_RECORD(item), box ) ;
    }
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c3p_box_append ( pcurve, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_PCURVE pcurve ;
C3_BOX box ;
{
    C3_ASEG arc ;
    arc = c3p_last_segm ( pcurve ) ;
    c3r_box_append ( arc, box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3p_coord_extrs ( pcurve, t0, t1, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
INT     coord ;
DML_LIST extr_list ;
{
    DML_ITEM item ;
    INT m ;
    C3_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    m = c3q_coord_extrs ( buffer, t0, t1, j0, coord, extr_list ) ;
    j0 += ( c3q_n ( buffer ) - 1 ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
        m += c3q_coord_extrs ( buffer, t0, t1, j0, coord, extr_list ) ;
        j0 += ( c3q_n ( buffer ) - 1 ) ;
    }
    RETURN ( m ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3p_length ( pcurve, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
{
    REAL length ;
    DML_ITEM item ;
    C3_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    length = c3q_length ( buffer, t0, t1, j0 ) ;
    j0 += ( c3q_n ( buffer ) - 1 ) ;

    C3_WALK_PCURVE ( pcurve, item ) {
        buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
        length += c3q_length ( buffer, t0, t1, j0 ) ;
        j0 += ( c3q_n ( buffer ) - 1 ) ;
    }
    RETURN ( length ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_project ( pcurve, t0, t1, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
REAL t0, t1 ;
PT3 pt ;
REAL *t_ptr ;
PT3 proj_pt ;
{
    DML_ITEM item ;
    C3_PC_BUFFER buffer ;
    INT j0 ;

    j0 = 0 ;
    buffer = C3_PCURVE_BUFFER(pcurve) ;
    if ( c3q_project ( buffer, t0, t1, j0, pt, t_ptr, proj_pt ) )
        RETURN ( TRUE ) ;
    j0 += ( c3q_n ( buffer ) - 1 ) ;

    C3_WALK_PCURVE ( pcurve, item ) {
        buffer = (C3_PC_BUFFER)DML_RECORD(item) ;
        if ( c3q_project ( buffer, t0, t1, j0, pt, t_ptr, proj_pt ) )
            RETURN ( TRUE ) ;
        j0 += ( c3q_n ( buffer ) - 1 ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_closed ( pcurve )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    PT3 p0, p1 ;

    RETURN ( c3p_ept0 ( pcurve, p0 ) && c3p_ept1 ( pcurve, p1 ) && 
        C3V_IDENT_PTS ( p0, p1 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PLANE_STATUS c3p_plane ( pcurve, normal, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 normal ;
REAL *dist_ptr ;
{
    INT deg=0 ;
    PT3 p[2] ;

    c3p_plane_def ( pcurve, normal, dist_ptr, p, &deg ) ;
    if ( deg < 3 ) 
        RETURN ( C3_PLANE_UNDER_DET ) ;
    else if ( deg > 3 ) 
        RETURN ( C3_PLANE_OVER_DET ) ;
    else
        RETURN ( C3_PLANE_DET ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_on_plane ( pcurve, normal, dist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 normal ;
REAL dist ;
{
    DML_ITEM item ;

    if ( !c3q_on_plane ( C3_PCURVE_BUFFER(pcurve), normal, dist ) ) 
        RETURN ( FALSE ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        if ( !c3q_on_plane ( DML_RECORD(item), normal, dist ) ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3p_plane_def ( pcurve, normal, dist_ptr, p, deg_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
INT *deg_ptr ;
{
    DML_ITEM item ;

    c3q_plane_def ( C3_PCURVE_BUFFER(pcurve), normal, dist_ptr, p, deg_ptr ) ;
    C3_WALK_PCURVE ( pcurve, item ) 
        c3q_plane_def ( DML_RECORD(item), normal, dist_ptr, p, deg_ptr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_ept0 ( pcurve, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT2 p ;
{
    DML_ITEM item ;

    if ( c3q_ept0 ( C3_PCURVE_BUFFER(pcurve), p ) ) 
        RETURN ( TRUE ) ;
    C3_WALK_PCURVE ( pcurve, item ) { 
        if ( c3q_ept0 ( DML_RECORD(item), p ) ) 
            RETURN ( TRUE ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_ept1 ( pcurve, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT2 p ;
{
    DML_ITEM item ;

    for ( item = dml_last ( C3_PCURVE_BLIST(pcurve) ) ; 
        item != NULL ; item = dml_prev ( item ) ) {
        if ( c3q_ept1 ( DML_RECORD(item), p ) ) 
            RETURN ( TRUE ) ;
    }
    if ( c3q_ept1 ( C3_PCURVE_BUFFER(pcurve), p ) ) 
        RETURN ( TRUE ) ;
    RETURN ( FALSE ) ;
}

/*--------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_get_data ( pcurve, i, ctr, ept1, rad_ptr, 
            normal, sweep_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
INT i ;
PT3 ctr, ept1 ;
REAL *rad_ptr ;
PT3 normal ;
REAL *sweep_ptr ;
{
    RETURN ( c3r_get_data ( c3p_segment ( pcurve, i ), 
            ctr, ept1, rad_ptr, normal, sweep_ptr ) ) ;
}

