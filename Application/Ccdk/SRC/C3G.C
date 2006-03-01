//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3G.C **********************************/
/******************* Routines for processing arc geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c3gdefs.h>
#include <c3rdefs.h>
#include <c3adefs.h>
#include <c3vdefs.h>
#include <c3gmcrs.h>
#include <c3vmcrs.h>
#include <c3mem.h>
#ifdef SPLINE
#include <c3bdefs.h>
#endif

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ARC c3g_create_copy ( arc0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc0 ;
{
    C3_ARC arc ;
    arc = C3_ALLOC_ARC ;
    if ( arc != NULL ) {
        c3r_copy ( C3_ARC_SEG(arc0), C3_ARC_SEG(arc) ) ;
        C3_ARC_ZERO_RAD(arc) = C3_ARC_ZERO_RAD(arc0) ;
    }
    RETURN ( arc ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3g_eval ( arc, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t ;
INT p ;
PT3 *x ;
{
    if ( C3_ARC_ZERO_RAD(arc) ) {
        INT i ;
        c3r_ctr ( C3_ARC_SEG(arc), x[0] ) ;
        for ( i=1 ; i<=p ; i++ ) 
            C3V_SET_ZERO ( x[i] ) ;
    }

    else {
        if ( t <= 0.0 ) 
            t = 0.0 ;
        if ( t <= 1.0 ) 
            c3r_eval ( C3_ARC_SEG(arc), t, p, x ) ;
        else {
            C3_ASEG_S arc1 ;

            if ( t >= 2.0 ) 
                t = 2.0 ;
            c3r_eval ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                t - 1.0, p, x ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3g_pt_tan ( arc, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t ;
PT3 pt, tan_vec ;
{
    if ( t <= 0.0 ) 
        t = 0.0 ;
    if ( t <= 1.0 ) 
        c3r_pt_tan ( C3_ARC_SEG(arc), t, pt, tan_vec ) ;
    else {
        C3_ASEG_S arc1 ;

        if ( t >= 2.0 ) 
            t = 2.0 ;
        c3r_pt_tan ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
            t - 1.0, pt, tan_vec ) ;
    }
    if ( C3_ARC_ZERO_RAD(arc) && pt != NULL ) 
        c3r_ctr ( C3_ARC_SEG(arc), pt ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3g_length ( arc, t0, t1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
{
    C3_ASEG_S arc1 ;
    if ( IS_SMALL(t0) && IS_SMALL(t1-1.0) ) 
        RETURN ( c3r_length ( C3_ARC_SEG(arc) ) ) ;
    else if ( IS_SMALL(t0) && IS_SMALL(t1-2.0) ) {
        
        RETURN ( c3r_length ( C3_ARC_SEG(arc) ) + 
            c3r_length ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ) ) ) ;
    }
    else 
        RETURN ( c3r_length ( c3r_trim ( C3_ARC_SEG(arc), t0, t1, &arc1 ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3g_project ( arc, t0, t1, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
PT3 pt ;
REAL *t_ptr ;
PT3 proj_pt ;
{
    C3_ASEG_S arc1 ;
    REAL u0, u1 ;
    PT3 p0, p1 ; 
    BOOLEAN status ;

    if ( t1 <= 1.0 + BBS_ZERO ) 
        RETURN ( c3r_project ( C3_ARC_SEG(arc), pt, t_ptr, proj_pt ) ) ;
    else if ( t0 >= 1.0 - BBS_ZERO ) {
        status = c3r_project ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
            pt, t_ptr, proj_pt ) ;
        *t_ptr += 1.0 ;
        RETURN ( status ) ;
    }
    else { 
        if ( !c3r_project ( C3_ARC_SEG(arc), pt, &u0, p0 ) ) {
            status = c3r_project ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
                pt, t_ptr, proj_pt ) ;
            *t_ptr += 1.0 ;
            RETURN ( status ) ;
        }
        else if ( !c3r_project ( c3r_complement ( C3_ARC_SEG(arc), &arc1 ), 
            pt, &u1, p1 ) ) {
            if ( proj_pt != NULL ) 
                C3V_COPY ( p0, proj_pt ) ;
            *t_ptr = u0 ;
            RETURN ( TRUE ) ;
        }
        else {
            if ( C3V_DIST ( p0, pt ) < C3V_DIST ( p1, pt ) ) {
                *t_ptr = u0 ;
                if ( proj_pt != NULL ) 
                    C3V_COPY ( p0, proj_pt ) ;
            }
            else {
                *t_ptr = u1 + 1.0 ;
                if ( proj_pt != NULL ) 
                    C3V_COPY ( p1, proj_pt ) ;
            }
            RETURN ( TRUE ) ;
        }
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3g_box ( arc, ext_arc, box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
BOOLEAN ext_arc ;
C3_BOX box ;
{
    PT3 ctr, normal, p ;
    REAL rad ;

    if ( ext_arc ) {
        rad = c3r_rad ( C3_ARC_SEG(arc) ) ;
        if ( rad < 0.0 ) {
            c3g_pt_tan ( arc, 0.0, p, NULL ) ;
            c3a_box_init_pt ( box, p ) ;
            c3g_pt_tan ( arc, 1.0, p, NULL ) ;
            c3a_box_append_pt ( box, p ) ;
        }
        else 
            c3a_box_circle ( box, c3r_ctr ( C3_ARC_SEG(arc), ctr), rad, 
                c3r_normal ( C3_ARC_SEG(arc), normal ) ) ;
    }
    else
        c3r_box ( C3_ARC_SEG(arc), box ) ;
}


#ifdef SPLINE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3g_bezs ( arc, ext_arc, bez ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
BOOLEAN ext_arc ;
HPT3 *bez ;
{
    C3_ASEG_S arcc ;
    HPT3 bez0[3], *bez1 ;

    if ( ext_arc ) {
        c3r_bez ( C3_ARC_SEG(arc), bez0 ) ;
        C3B_SBDV0 ( bez0, 3, bez ) ;
        c3r_bez ( c3r_complement ( C3_ARC_SEG(arc), &arcc ), bez0 ) ;
        bez1 = bez + 4 ;
        C3B_SBDV0 ( bez0, 3, bez1 ) ;
        RETURN ( 9 ) ;
    }

    else if ( fabs ( c3r_d_parm ( C3_ARC_SEG(arc) ) ) <= 1.0 - BBS_ZERO ) {
        c3r_bez ( C3_ARC_SEG(arc), bez ) ;
        RETURN ( 3 ) ;
    }
    else {
        c3r_bez ( C3_ARC_SEG(arc), bez0 ) ;
        C3B_SBDV0 ( bez0, 3, bez ) ;
        RETURN ( 5 ) ;
    }
}
#endif /*SPLINE*/

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PLANE_STATUS c3g_plane ( arc, normal, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
PT3 normal ;
REAL *dist_ptr ;
{
    RETURN ( c3r_plane ( C3_ARC_SEG(arc), normal, dist_ptr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3g_on_plane ( arc, normal, dist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
PT3 normal ;
REAL dist ;
{
    PT3 c ;

    RETURN ( C3_ARC_ZERO_RAD(arc) ? 
        c3v_pt_on_plane ( c3r_ctr ( C3_ARC_SEG(arc), c ), normal, dist ) :
        c3r_on_plane ( C3_ARC_SEG(arc), normal, dist ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3g_plane_def ( arc, normal, dist_ptr, p, deg_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software !!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
INT *deg_ptr ;
{
    if ( !C3_ARC_ZERO_RAD(arc) )
        c3r_plane_def ( C3_ARC_SEG(arc), normal, dist_ptr, p, deg_ptr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ARC c3g_reverse ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc0, arc1 ;
{
    if ( c3r_reverse ( C3_ARC_SEG(arc0), C3_ARC_SEG(arc1) ) == NULL ) 
        RETURN ( NULL ) ;
    C3_ARC_ZERO_RAD(arc1) = C3_ARC_ZERO_RAD(arc0) ;
    RETURN ( arc1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3g_rad ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
{
    RETURN ( C3_ARC_ZERO_RAD(arc) ? 0.0 : c3r_rad ( C3_ARC_SEG(arc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c3g_ctr ( arc, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
PT3 ctr ;
{
    RETURN ( c3r_ctr ( C3_ARC_SEG(arc), ctr ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c3g_normal ( arc, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
PT3 normal ;
{
    RETURN ( C3_ARC_ZERO_RAD(arc) ? NULL : 
        c3r_normal ( C3_ARC_SEG(arc), normal ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3g_t_to_angle ( arc, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t ;
{
    RETURN ( C3_ARC_ZERO_RAD(arc) ? 0.0 : 
        c3r_t_to_angle ( C3_ARC_SEG(arc), t ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3g_angle_to_t ( arc, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL angle ;
{
    RETURN ( C3_ARC_ZERO_RAD(arc) ? 0.0 : 
        c3r_angle_to_t ( C3_ARC_SEG(arc), angle ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3g_d_parm ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
{
    RETURN ( C3_ARC_ZERO_RAD(arc) ? 0.0 : c3r_d_parm ( C3_ARC_SEG(arc) ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3g_get_pts_d ( arc, t0, t1, pt0, pt1, d_vec )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
REAL t0, t1 ;
PT3 pt0, pt1 ;
REAL* d_vec ;
{
    RETURN ( !C3_ARC_ZERO_RAD(arc) && 
        c3r_get_pts_d ( C3_ARC_SEG(arc), t0, t1, pt0, pt1, d_vec ) ) ;
}

