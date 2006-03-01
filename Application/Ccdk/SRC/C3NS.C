//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3NS.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3apriv.h>
#include <c3adefs.h>
#include <c3hdefs.h>
#include <c3ndefs.h>
#include <c3sdefs.h>
#include <c3hmcrs.h>
#include <c3nmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_project ( nurb, parm0, parm1, pt, parm, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm0, parm1 ;
PT3 pt ;
PARM parm ;
PT3 proj_pt ;
{
    RETURN ( c3s_project ( C3_NURB_CTPT(nurb), C3_NURB_D(nurb), 
            C3_NURB_KNOT(nurb), C3_NURB_W(nurb), C3_NURB_NORMAL(nurb), 
            parm0, parm1, pt, parm, proj_pt ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_reverse ( nurb, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm0, parm1 ;
{
    c3s_reverse ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
        C3_NURB_D(nurb), C3_NURB_KNOT(nurb), parm0, parm1 ) ;
    C3_NURB_W(nurb) = - C3_NURB_W(nurb) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_parm_adjust ( nurb, parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm ;
{
    RETURN ( als_parm_adjust ( C3_NURB_N(nurb), C3_NURB_D(nurb), 
        C3_NURB_KNOT(nurb), parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_box ( nurb, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
C3_BOX box ;
{
    c3a_box_hpoly ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), box ) ;
    c3a_box_inflate ( box, fabs(C3_NURB_W(nurb)) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_translate ( nurb, shift ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 shift ;
{
    INT i ;
    for ( i=0 ; i<C3_NURB_N(nurb) ; i++ ) 
        c3h_addv ( C3_NURB_CTPT(nurb)[i], shift, C3_NURB_CTPT(nurb)[i] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_scale ( nurb, a, factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 a ;
REAL factor ;
{
    INT i ;
    for ( i=0 ; i<C3_NURB_N(nurb) ; i++ ) {
        C3V_ADDW ( a, C3_NURB_CTPT(nurb)[i][3], C3_NURB_CTPT(nurb)[i], 
            factor, C3_NURB_CTPT(nurb)[i] ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_mirror ( nurb, n, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 n ;
REAL d ;
{
    INT i ;
    for ( i=0 ; i<C3_NURB_N(nurb) ; i++ ) 
        c3h_mirror ( C3_NURB_CTPT(nurb)[i], n, d, C3_NURB_CTPT(nurb)[i] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_rotate_cs ( nurb, origin, axis, c, s ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 origin, axis ;
REAL c, s ;
{
    c3a_rot_cs_hpt ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
        origin, axis, c, s, C3_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_project_oblique ( nurb, normal, dist, direction ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 normal, direction ;
REAL dist ;
{
    RETURN ( c3a_project_hpt_oblique ( C3_NURB_CTPT(nurb), 
            C3_NURB_N(nurb), normal, dist, direction, C3_NURB_CTPT(nurb) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_transform ( nurb, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
C3_TRANSFORM t ;
{
    c3a_transform_hpt ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), t, 
        C3_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE HPT3* c3n_get_ctpt ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3_NURB_CTPT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3n_get_n ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3_NURB_N(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c3n_get_knot ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3_NURB_KNOT(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3n_get_d ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3_NURB_D(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3n_get_w ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3_NURB_W(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_w ( nurb, w ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
REAL w ;
{
    C3_NURB_W(nurb) = w ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c3n_get_normal ( nurb ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
{
    RETURN ( C3_NURB_NORMAL(nurb) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3n_set_normal ( nurb, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 normal ;
{
    C3V_COPY ( normal, C3_NURB_NORMAL(nurb) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_init_parms ( nurb, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm0, parm1 ;
{
    INT n, d ;

    n = C3_NURB_N(nurb) ;
    d = C3_NURB_D(nurb) ;
    if ( parm0 != NULL ) {
        PARM_T(parm0) = C3_NURB_KNOT(nurb)[d-1] ;
        PARM_J(parm0) = d-1 ;
    }
    if ( parm1 != NULL ) {
        PARM_T(parm1) = C3_NURB_KNOT(nurb)[n] ;
        PARM_J(parm1) = n-1 ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_trimmed ( nurb, parm0, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm0, parm1 ;
{
    RETURN ( c3n_trimmed0 ( nurb, parm0 ) && c3n_trimmed1 ( nurb, parm1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_trimmed0 ( nurb, parm0 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm0 ;
{
    RETURN ( PARM_T(parm0) > 
        C3_NURB_KNOT(nurb)[C3_NURB_D(nurb)-1] + BBS_ZERO ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_trimmed1 ( nurb, parm1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm1 ;
{
    RETURN ( PARM_T(parm1) < 
            C3_NURB_KNOT(nurb)[C3_NURB_N(nurb)] - BBS_ZERO ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_interp_tan ( a, n, knot_options, 
                tan0, tan0_options, tan1, tan1_options, nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-2] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
PT3 tan0    ;
C3_TAN_OPTIONS tan0_options ;
PT3 tan1    ;
C3_TAN_OPTIONS tan1_options ;
C3_NURB nurb ;
{
    PT3 *b ;

    b = CREATE ( n+2, PT3 ) ;
    if ( b == NULL ) 
        RETURN ( FALSE ) ;

    if ( !c3s_interp_tan ( a, n+2, knot_options, tan0, tan0_options, 
            tan1, tan1_options, b, C3_NURB_KNOT(nurb) ) ) 
        RETURN ( FALSE ) ;
    c3n_set_ctpts ( b, nurb ) ;
    KILL ( b ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_interp_clsd ( a, n, knot_options, nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n-3] */
INT n ;
C3_KNOT_OPTIONS knot_options ;
C3_NURB nurb ;
{
    PT3 *b ;

    b = CREATE ( n+3, PT3 ) ;
    if ( b == NULL ) 
        RETURN ( FALSE ) ;

    if ( !c3s_interp_clsd ( a, n+3, knot_options, b, 
        C3_NURB_KNOT(nurb) ) ) 
        RETURN ( FALSE ) ;
    c3n_set_ctpts ( b, nurb ) ;
    KILL ( b ) ;
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_mass_prop ( nurb, parm0, parm1, p, dim, tol, 
        prop_function, result ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM    parm0, parm1 ;
INT     p ;
INT     dim ;
REAL    tol ;
PF_PROP3 prop_function ;
REAL    *result ;    /* result[dim] */
{
    c3s_mass_prop ( C3_NURB_CTPT(nurb), C3_NURB_D(nurb), 
        C3_NURB_KNOT(nurb), C3_NURB_W(nurb), C3_NURB_NORMAL(nurb), 
        parm0, parm1, p, dim, tol, prop_function, result ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_get_ellipse_data ( nurb, ctr, major_axis, minor_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 ctr ;
REAL *major_axis, *minor_axis ;
{
    RETURN ( c3s_get_ellipse_data ( C3_NURB_CTPT(nurb), 
        ctr, major_axis, minor_axis ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_eval ( nurb, parm, p, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm ;
INT p ;
PT3 *x ;
{
    RETURN ( c3s_eval ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb),
        C3_NURB_D(nurb), C3_NURB_KNOT(nurb), C3_NURB_W(nurb), 
        C3_NURB_NORMAL(nurb), parm, p, x ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_on_plane ( nurb, normal, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 normal ;
REAL dist ;
{
    RETURN ( c3s_on_plane ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
        normal, dist ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_PLANE_STATUS c3n_plane ( nurb, normal, dist_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 normal ;
REAL *dist_ptr ;
{
    RETURN ( c3s_plane ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
        normal, dist_ptr ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_plane_def ( nurb, normal, dist_ptr, p, deg_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
INT *deg_ptr ;
{
    c3s_plane_def ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
        normal, dist_ptr, p, deg_ptr ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_select ( nurb, parm0, parm1, pt, view_plane_normal, 
            tol, sel_parm, dist_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM parm0, parm1 ;
PT3 pt, view_plane_normal ;
REAL tol ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    RETURN ( c3s_select ( C3_NURB_CTPT(nurb), C3_NURB_D(nurb), 
        C3_NURB_KNOT(nurb), C3_NURB_W(nurb), C3_NURB_NORMAL(nurb), 
        parm0, parm1, pt, view_plane_normal, tol, sel_parm, dist_ptr ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3n_ellipse ( ctr, major_axis, minor_axis, nurb ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 ctr, major_axis, minor_axis ;
C3_NURB nurb ;
{
    INT i ;

    C3V_COPY ( major_axis, C3_NURB_CTPT(nurb)[0] ) ;
    C3_NURB_CTPT(nurb)[0][3] = 1.0 ;

    C3V_ADD ( major_axis, minor_axis, C3_NURB_CTPT(nurb)[1] ) ;
    C3V_SCALE ( C3_NURB_CTPT(nurb)[1], 1.0/SQRT_2, C3_NURB_CTPT(nurb)[1] ) ;
    C3_NURB_CTPT(nurb)[1][3] = 1.0/SQRT_2 ;

    C3V_COPY ( minor_axis, C3_NURB_CTPT(nurb)[2] ) ;
    C3_NURB_CTPT(nurb)[2][3] = 1.0 ;

    C3V_SUB ( minor_axis, major_axis, C3_NURB_CTPT(nurb)[3] ) ;
    C3V_SCALE ( C3_NURB_CTPT(nurb)[3], 1.0/SQRT_2, C3_NURB_CTPT(nurb)[3] ) ;
    C3_NURB_CTPT(nurb)[3][3] = 1.0/SQRT_2 ;
    
    for ( i=4 ; i<9 ; i++ ) {
        C3V_NEGATE ( C3_NURB_CTPT(nurb)[i-4], C3_NURB_CTPT(nurb)[i] ) ;
        C3_NURB_CTPT(nurb)[i][3] = C3_NURB_CTPT(nurb)[i-4][3] ;
    }

    for ( i=0 ; i<9 ; i++ ) 
        C3H_ADDV ( C3_NURB_CTPT(nurb)[i], ctr, C3_NURB_CTPT(nurb)[i] ) ;
    
    C3_NURB_KNOT(nurb)[0] = C3_NURB_KNOT(nurb)[1] = 
        C3_NURB_KNOT(nurb)[2] = 0.0 ;
    C3_NURB_KNOT(nurb)[3] = C3_NURB_KNOT(nurb)[4] = 0.5 ;
    C3_NURB_KNOT(nurb)[5] = C3_NURB_KNOT(nurb)[6] = 1.0 ;
    C3_NURB_KNOT(nurb)[7] = C3_NURB_KNOT(nurb)[8] = 1.5 ;
    C3_NURB_KNOT(nurb)[9] = C3_NURB_KNOT(nurb)[10] = 
        C3_NURB_KNOT(nurb)[11] = 2.0 ;
} 
#endif  /*SPLINE*/

