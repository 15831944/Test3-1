//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3Q.C **********************************/
/*************** Routines for processing polycurve geometry ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

//#include <alpdefs.h>
//#include <alrdefs.h>
//#include <c3rdefs.h>
//#include <c3adefs.h>

#include <c3qdefs.h>
#include <c3rdefs.h>
#include <c3adefs.h>
#include <c3vdefs.h>
#include <c3vpriv.h>
#include <dmldefs.h>
#include <c3qmcrs.h>
#include <c3vmcrs.h>
#include <c3rmcrs.h>
STATIC void c3q_box0 __(( C3_PC_BUFFER, C3_BOX )) ;
STATIC BOOLEAN c3q_ti __(( C3_PC_BUFFER, REAL*, INT* )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_eval ( buffer, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t ;
INT p ;
PT3 *x ;
{
    INT i ;

    if ( C3_PC_BUFFER_N(buffer) == 1 ) {
        C3V_COPY ( C3_PC_BUFFER_PT0(buffer), x[0] ) ;
        for ( i = 1 ; i <= p ; i++ ) {
            C3V_SET_ZERO ( x[i] ) ;
        }
        RETURN ( TRUE ) ;
    }
    RETURN ( c3q_ti ( buffer, &t, &i ) && 
        c3r_eval ( C3_PC_BUFFER_ARC(buffer,i), t-(REAL)i, p, x ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3q_n ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    RETURN ( C3_PC_BUFFER_N(buffer) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3q_size ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    RETURN ( C3_PC_BUFFER_SIZE(buffer) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_pt_tan ( buffer, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t ;
PT3 pt, tan_vec ;
{
    INT i ;
    RETURN ( c3q_ti ( buffer, &t, &i ) && 
        c3r_pt_tan ( C3_PC_BUFFER_ARC(buffer,i), t-(REAL)i, 
            pt, tan_vec ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3q_segment ( buffer, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
INT *i ;
{
    if ( buffer == NULL || C3_PC_BUFFER_N(buffer) <= 0 )
        RETURN ( NULL ) ;
    if ( *i < 0 )
        RETURN ( NULL ) ;
    else if ( *i < (C3_PC_BUFFER_N(buffer)-1) )
        RETURN ( C3_PC_BUFFER_ARC(buffer,*i) ) ;
    else {
        *i -= ( C3_PC_BUFFER_N(buffer) - 1 ) ;
        RETURN ( NULL ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3q_first_segm ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    if ( buffer == NULL || C3_PC_BUFFER_N(buffer) <= 0 )
        RETURN ( NULL ) ;
    else 
        RETURN ( C3_PC_BUFFER_ARC(buffer,0) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3q_last_segm ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    if ( buffer == NULL || C3_PC_BUFFER_N(buffer) <= 1 )
        RETURN ( NULL ) ;
    else 
        RETURN ( C3_PC_BUFFER_ARC(buffer,C3_PC_BUFFER_N(buffer)-2) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_curvature ( buffer, t, crv ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t, *crv ;
{
    INT i ;
    if ( c3q_ti ( buffer, &t, &i ) ) {
        *crv = c3r_curvature ( C3_PC_BUFFER_ARC(buffer,i) ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN c3q_box ( buffer, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_PC_BUFFER buffer ;
C3_BOX box ;
{
    if ( C3_PC_BUFFER_N(buffer) <= 0 ) 
        RETURN ( FALSE ) ;
    /* CNM Start */
    if ( C3_PC_BUFFER_N(buffer) <= 1 ) 
      {
      c3a_box_init_pt ( box, C3_ASEG_PT0(C3_PC_BUFFER_ARC(buffer,0)) ) ;
      RETURN ( TRUE );
      }
	/* CNM End */

    c3r_box ( C3_PC_BUFFER_ARC(buffer,0), box ) ;
    c3q_box0 ( buffer, box ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE BOOLEAN c3q_box_append ( buffer, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_PC_BUFFER buffer ;
C3_BOX box ;
{
    if ( C3_PC_BUFFER_N(buffer) <= 0 ) 
        RETURN ( FALSE ) ;
    c3r_box_append ( C3_PC_BUFFER_ARC(buffer,0), box ) ;
    c3q_box0 ( buffer, box ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC void c3q_box0 ( buffer, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_PC_BUFFER buffer ;
C3_BOX box ;
{
    INT i ;
    for ( i=1 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) 
        c3r_box_append ( C3_PC_BUFFER_ARC(buffer,i), box ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3q_coord_extrs ( buffer, t0, t1, j0, coord, extr_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0, coord ;
DML_LIST extr_list ;
{
    REAL t[2], u ;
    INT i, j, m, n ;
    C3_EXTR_REC extr_rec ;
    PT3 pt ;

    n = 0 ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) {
        m = c3r_coord_extrs ( C3_PC_BUFFER_ARC(buffer,i), coord, t ) ;
        for ( j=0 ; j<m ; j++ ) {
            u = t[j] + (REAL)j0 + (REAL)i ;
            if ( u >= t0-BBS_ZERO && t[j]<=t1+BBS_ZERO && 
                ( m==0 || !IS_SMALL(t[j] - 
                C3_EXTR_REC_T((C3_EXTR_REC)DML_LAST_RECORD(extr_list)) ) ) ) {
                extr_rec = MALLOC ( 1, C3_EXTR_REC_S ) ;
                if ( extr_rec == NULL )
                    RETURN (-1) ;
                C3_EXTR_REC_T(extr_rec) = u ; 
                C3_EXTR_REC_J(extr_rec) = i + j0 ; 
                c3r_pt_tan ( C3_PC_BUFFER_ARC(buffer,i), t[j], pt, NULL ) ;
                C3_EXTR_REC_F(extr_rec) = pt[coord] ;
                C3_EXTR_REC_TYPE(extr_rec) = 1 ;
                dml_append_data ( extr_list, extr_rec ) ; 
                n++ ;
            }
        }
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3q_length ( buffer, t0, t1, j0 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
{
    INT i0, i1, i ;
    REAL length ;
    C3_ASEG_S arc1 ;

    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;
    i0 = (INT)t0 ;
    i1 = (INT)t1 ;
    t0 -= (REAL)i0 ;
    t1 -= (REAL)i1 ;
    length = 0.0 ;

    if ( i0==i1 ) {
        c3r_trim ( C3_PC_BUFFER_ARC(buffer,i0), t0, t1, &arc1 ) ;
        RETURN ( c3r_length ( &arc1 ) ) ;
    }

    c3r_trim ( C3_PC_BUFFER_ARC(buffer,i0), t0, 1.0, &arc1 ) ;
    length += c3r_length ( &arc1 ) ;
    for ( i=i0+1 ; i<i1 ; i++ )
        length += c3r_length ( C3_PC_BUFFER_ARC(buffer,i) ) ;
    if ( t1 > BBS_ZERO ) {
        c3r_trim ( C3_PC_BUFFER_ARC(buffer,i1), 0.0, t1, &arc1 ) ;
        length += c3r_length ( &arc1 ) ;
    }
    RETURN ( length ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_project ( buffer, t0, t1, j0, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL t0, t1 ;
INT j0 ;
PT3 pt ;
REAL *t_ptr ;
PT3 proj_pt ;
{
    INT i ;
    BOOLEAN status=FALSE ;
    REAL dist, dist_min=0.0, t ;
    PT3 p ;

    t0 -= (REAL)j0 ;
    t1 -= (REAL)j0 ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) {
        if ( c3r_project ( C3_PC_BUFFER_ARC(buffer,i), pt, &t, p ) ) {
            t += (REAL)i ;
            if ( t0-BBS_ZERO <= t && t <= t1+BBS_ZERO ) {
                dist = C3V_DIST ( p, pt ) ;
                if ( !status || dist < dist_min ) {
                    dist_min = dist ;
                    *t_ptr = t + (REAL)j0 ;
                    if ( proj_pt != NULL ) 
                        C3V_COPY ( p, proj_pt ) ;
                    status = TRUE ;
                }
            }
        }
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_closed ( buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    RETURN ( C3V_IDENT_PTS ( C3_PC_BUFFER_PT0(buffer), 
        C3_PC_BUFFER_PT1(buffer) ) ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PLANE_STATUS c3q_plane ( buffer, normal, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 normal ;
REAL *dist_ptr ;
{
    INT deg=0 ;
    PT3 p[2] ;

    c3q_plane_def ( buffer, normal, dist_ptr, p, &deg ) ;
    if ( deg < 3 ) 
        RETURN ( C3_PLANE_UNDER_DET ) ;
    else if ( deg > 3 ) 
        RETURN ( C3_PLANE_OVER_DET ) ;
    else
        RETURN ( C3_PLANE_DET ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_on_plane ( buffer, normal, dist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 normal ;
REAL dist ;
{
    PT3 c ;
    INT i ;

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) {
        if ( !c3v_pt_on_plane ( C3_PC_BUFFER_PT(buffer,i), normal, dist ) )
            RETURN ( FALSE ) ;
    }
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 ; i++ ) {
        if ( !c3v_pt_on_plane ( c3r_mid_pt ( C3_PC_BUFFER_ARC(buffer,i), c ), 
            normal, dist ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3q_plane_def ( buffer, normal, dist_ptr, p, deg_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
INT *deg_ptr ;
{
    PT3 c ;
    INT i ;

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) && *deg_ptr < 4 ; i++ ) 
        c3v_plane_def ( C3_PC_BUFFER_PT(buffer,i), p, normal, 
            dist_ptr, deg_ptr ) ;
    for ( i=0 ; i<C3_PC_BUFFER_N(buffer)-1 && *deg_ptr < 4 ; i++ ) 
        c3v_plane_def ( c3r_mid_pt ( C3_PC_BUFFER_ARC(buffer,i), c ), 
            p, normal, dist_ptr, deg_ptr ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c3q_ti ( buffer, t, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
REAL *t ;
INT *i ;
{
    *i = (INT)(*t-BBS_ZERO) ;
    if ( *i < 0 ) {
        *i = 0 ;
        *t = 0.0 ;
        RETURN ( TRUE ) ;
    }
    else if ( *i >= C3_PC_BUFFER_N(buffer) - 1 ) {
        *i = C3_PC_BUFFER_N(buffer) - 2 ;
        *t -= ( (REAL) C3_PC_BUFFER_N(buffer) - 1.0 ) ;
        RETURN ( *t <= 1.0 + BBS_ZERO ) ;
    }
    else 
        RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_ept0 ( buffer, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 p ;
{
    if ( buffer != NULL && C3_PC_BUFFER_N(buffer) > 0 ) {
        C3V_COPY ( C3_PC_BUFFER_PT0(buffer), p ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3q_ept1 ( buffer, p )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
PT3 p ;
{
    if ( buffer != NULL && C3_PC_BUFFER_N(buffer) > 0 ) {
        C3V_COPY ( C3_PC_BUFFER_PT1(buffer), p ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}

