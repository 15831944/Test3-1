//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3R.C **********************************/
/******************* Routines for processing arc geometry ******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <alpdefs.h>
#include <alrdefs.h>
#include <c3rdefs.h>
#include <c3adefs.h>
#include <c3edefs.h>
#include <c3vdefs.h>
#include <c3vpriv.h>
#include <c3rmcrs.h>
#include <c3vmcrs.h>

STATIC void c3r_eval_polynomial __(( C3_ASEG, REAL, INT, PT3* )) ;
STATIC void c3r_box0 __(( C3_ASEG, C3_BOX )) ;

#define C3R_UV(ARC,VEC,U,V) {C3V_SUB(C3_ASEG_PT1(ARC),C3_ASEG_PT0(ARC),VEC);\
V=C3V_DOT(VEC,VEC);U=C3V_DOT(C3_ASEG_D_VEC(ARC),C3_ASEG_D_VEC(ARC))*V;}
#define C3R_AB(U,V,A,B) {B=2.0*V/(V+U);A=B-1.0;}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_eval ( arc, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
REAL t ;
INT p ;
PT3 *x ;
{
    if ( p==0 ) 
        c3r_pt_tan ( arc, t, x[0], NULL ) ;
    else if ( p==1 ) 
        c3r_pt_tan ( arc, t, x[0], x[1] ) ;
    else if ( p >= 2 ) 
        c3r_eval_polynomial ( arc, t, p, x ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_pt_tan ( arc, t, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
REAL t ;
PT3 pt, tan_vec ;
{
    REAL t0, t1, t2, a, b, u, v, z, v1 ;
    PT3 vec ;

    C3R_UV ( arc, vec, u, v ) ;
    C3R_AB ( u, v, a, b ) ;
    v1 = sqrt ( v ) ;

    t0 = ( 1.0 - t ) * ( 1.0 - t ) ;
    t1 = t * ( 1.0 - t ) ;
    t2 = t * t ;

    z = t0 + 2.0 * a * t1 + t2 ;

    if ( pt != NULL ) {
        pt[0] = ( C3_ASEG_PT0(arc)[0] * t0 + C3_ASEG_PT1(arc)[0] * t2 + 
            ( a * ( C3_ASEG_PT0(arc)[0] + C3_ASEG_PT1(arc)[0] ) + 
            b * C3_ASEG_D_VEC(arc)[0] * v1 ) * t1 ) / z ;
        pt[1] = ( C3_ASEG_PT0(arc)[1] * t0 + C3_ASEG_PT1(arc)[1] * t2 + 
            ( a * ( C3_ASEG_PT0(arc)[1] + C3_ASEG_PT1(arc)[1] ) + 
            b * C3_ASEG_D_VEC(arc)[1] * v1 ) * t1 ) / z ;
        pt[2] = ( C3_ASEG_PT0(arc)[2] * t0 + C3_ASEG_PT1(arc)[2] * t2 + 
            ( a * ( C3_ASEG_PT0(arc)[2] + C3_ASEG_PT1(arc)[2] ) + 
            b * C3_ASEG_D_VEC(arc)[2] * v1 ) * t1 ) / z ;
    }

    if ( tan_vec != NULL ) {
        z *= z ;
        tan_vec[0] = ( ( a * vec[0] + b * C3_ASEG_D_VEC(arc)[0] * v1 ) * t0 + 
            2.0 * vec[0] * t1 +
            ( a * vec[0] - b * C3_ASEG_D_VEC(arc)[0] * v1 ) * t2 ) / z ;
        tan_vec[1] = ( ( a * vec[1] + b * C3_ASEG_D_VEC(arc)[1] * v1 ) * t0 + 
            2.0 * vec[1] * t1 +
            ( a * vec[1] - b * C3_ASEG_D_VEC(arc)[1] * v1 ) * t2 ) / z ;
        tan_vec[2] = ( ( a * vec[2] + b * C3_ASEG_D_VEC(arc)[2] * v1 ) * t0 + 
            2.0 * vec[2] * t1 +
            ( a * vec[2] - b * C3_ASEG_D_VEC(arc)[2] * v1 ) * t2 ) / z ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_tan0 ( arc, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 tan_vec ;
{
    REAL a, b, u, v, v1 ;
    PT3 vec ;

    C3R_UV ( arc, vec, u, v ) ;
    C3R_AB ( u, v, a, b ) ;
    v1 = sqrt ( v ) ;
    tan_vec[0] = a * vec[0] + b * C3_ASEG_D_VEC(arc)[0] * v1 ;
    tan_vec[1] = a * vec[1] + b * C3_ASEG_D_VEC(arc)[1] * v1 ;
    tan_vec[2] = a * vec[2] + b * C3_ASEG_D_VEC(arc)[2] * v1 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_tan1 ( arc, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 tan_vec ;
{
    REAL a, b, u, v, v1 ;
    PT3 vec ;

    C3R_UV ( arc, vec, u, v ) ;
    C3R_AB ( u, v, a, b ) ;
    v1 = sqrt ( v ) ;
    tan_vec[0] = a * vec[0] - b * C3_ASEG_D_VEC(arc)[0] * v1 ;
    tan_vec[1] = a * vec[1] - b * C3_ASEG_D_VEC(arc)[1] * v1 ;
    tan_vec[2] = a * vec[2] - b * C3_ASEG_D_VEC(arc)[2] * v1 ;
}


/*-------------------------------------------------------------------------*/
STATIC void c3r_eval_polynomial ( arc, t, p, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
REAL t ;
INT p ; /* p>= 2 */
PT3 *x ;
{
    HPT3 bez[3], b[3] ;
    HPT3 *f ;
    INT j ;

    f = CREATE ( p+1, HPT3 ) ;
    c3r_bez ( arc, bez ) ;
    for ( j=0 ; j<=3 ; j++ ) {
        b[0][j] = bez[0][j] ;
        b[1][j] = 2.0 * ( bez[1][j] - bez[0][j] ) ;
        b[2][j] = bez[2][j] - 2.0 * bez[1][j] + bez[0][j] ;
    }
    c3e_polynomial2 ( b, t, p, f ) ;
    (void) c3e_ratio ( f, p, x ) ;
    KILL ( f ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_bez ( arc, bez ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
HPT3 *bez ;
{
    C3V_COPY ( C3_ASEG_PT0(arc), bez[0] ) ;
    bez[0][3] = 1.0 ;
    c3r_bez_mid_pt ( arc, bez[1] ) ;
    C3V_COPY ( C3_ASEG_PT1(arc), bez[2] ) ;
    bez[2][3] = 1.0 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL*   c3r_mid_pt ( arc, p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 p ;
{
    PT3 d ;
    REAL v ;
    v = C3V_DIST ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) ;
    C3V_ADD ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc), p ) ;
    C3V_SCALE ( C3_ASEG_D_VEC(arc), v, d ) ;
    C3V_MID_PT ( p, d, p ) ;
    RETURN ( p ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_bez_mid_pt ( arc, bez ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
HPT3 bez ;
{
    REAL u, v ;

    u = C3V_DOT ( C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc) ) ;
    bez[3] = ( 1.0 - u ) / ( 1.0 + u ) ;
    v = C3V_DIST ( C3_ASEG_PT1(arc), C3_ASEG_PT0(arc) ) * ( 1.0 + bez[3] ) ;
    bez[0] = 0.5 * ( bez[3] * ( ( C3_ASEG_PT0(arc)[0] + C3_ASEG_PT1(arc)[0] ) )
        + v * C3_ASEG_D_VEC(arc)[0] ) ; 
    bez[1] = 0.5 * ( bez[3] * ( ( C3_ASEG_PT0(arc)[1] + C3_ASEG_PT1(arc)[1] ) )
        + v * C3_ASEG_D_VEC(arc)[1] ) ; 
    bez[2] = 0.5 * ( bez[3] * ( ( C3_ASEG_PT0(arc)[2] + C3_ASEG_PT1(arc)[2] ) )
        + v * C3_ASEG_D_VEC(arc)[2] ) ; 
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c3r_ab ( arc, a_ptr, b_ptr ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_ASEG arc ;
REAL *a_ptr, *b_ptr ;
{
    REAL u, v ;
    PT3 vec ;

    C3R_UV ( arc, vec, u, v ) ;
    C3R_AB ( u, v, (*a_ptr), (*b_ptr) ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c3r_box ( arc, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_ASEG arc ;
C3_BOX box ;
{
    c3a_box_init_pt ( box, C3_ASEG_PT0(arc) ) ;
    c3r_box0 ( arc, box ) ;
}


/*-------------------------------------------------------------------------*/ 
BBS_PRIVATE void c3r_box_append ( arc, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_ASEG arc ;
C3_BOX box ;
{
    c3a_box_append_pt ( box, C3_ASEG_PT0(arc) ) ;
    c3r_box0 ( arc, box ) ;
}


/*-------------------------------------------------------------------------*/ 
STATIC void c3r_box0 ( arc, box ) 
/*-------------------------------------------------------------------------*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                          !!!!!!!*/ 
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
C3_ASEG arc ;
C3_BOX box ;
  {
  PT3 p;
  INT i, j, n;
  REAL t[2];

  c3a_box_append_pt( box, C3_ASEG_PT1(arc) );

  for (j=0; j<3; j++)
    {
    n = c3r_coord_extrs( arc, j, t );
    for (i=0; i<n; i++) 
      {
      if (t[i] >= - BBS_ZERO && t[i] <= 1.0 + BBS_ZERO )
        {
        c3r_pt_tan( arc, t[i], p, NULL );
        c3a_box_append_pt( box, p );
        }
      }
    }
  }


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c3r_coord_extrs ( arc, coord, x_t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
INT     coord ;
REAL    *x_t ;
  {
  PT3 vec ;
  REAL z[3], t[2], a, b, u, v, v1 ;
  INT i, m, n, mult[2] ;


  //C3R_UV ( arc, vec, u, v ) ;
  //kga *** 5/1/98 *** changed the above line to those seen below,
  //previously it crashes if pt0 and pt1 in arc are the same!
  C3V_SUB(C3_ASEG_PT1(arc), C3_ASEG_PT0(arc), vec);
  if ( C3V_IS_SMALL(vec) )
    RETURN ( 0 );
  v = C3V_DOT(vec, vec);
  u = C3V_DOT(C3_ASEG_D_VEC(arc), C3_ASEG_D_VEC(arc))*v;


  if ( IS_SMALL(u+v) ) 
    RETURN ( 0 );
  C3R_AB( u, v, a, b );
  v1 = sqrt( v );
  z[0] = a * vec[coord] + b * C3_ASEG_D_VEC(arc)[coord] * v1;
  z[1] = vec[coord];
  z[2] = a * vec[coord] - b * C3_ASEG_D_VEC(arc)[coord] * v1;
  m = alp_solve_bez3( z[0], z[1], z[2], t, mult );
  n = 0;
  for (i=0; i<m; i++)
    {
    if ( - BBS_ZERO <= t[i] && t[i] <= 1.0 + BBS_ZERO ) 
      {
      x_t[n] = t[i];
      n++;
      }
    }
  RETURN ( n );
  }


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_length ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
{
    REAL d ;
    d = c3r_d_parm ( arc ) ;
    RETURN ( IS_ZERO(4.0*d) ? 
        C3V_DIST ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) : 
        C3V_DIST ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) * 
        ( 1.0 + d * d ) * atan ( d ) / d ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_project ( arc, pt, t_ptr, proj_pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 pt ;
REAL *t_ptr ;
PT3 proj_pt ;
{
    REAL u, v, a, b, v1, z0, z1, z2 ;
    PT3 vec, vec0, vec1 ;

    C3R_UV ( arc, vec, u, v ) ;
    C3R_AB ( u, v, a, b ) ;
    v1 = sqrt ( v ) ;
    C3V_SUB ( C3_ASEG_PT0(arc), pt, vec0 ) ;
    C3V_SUB ( C3_ASEG_PT1(arc), pt, vec1 ) ;
    z0 = C3V_DOT ( vec0, vec0 ) ;
    z1 = a * C3V_DOT ( vec0, vec1 ) + 0.5 * b * v1 * ( 
        C3V_DOT ( vec0, C3_ASEG_D_VEC(arc) ) + 
        C3V_DOT ( vec1, C3_ASEG_D_VEC(arc) ) ) ;
    z2 = C3V_DOT ( vec1, vec1 ) ;

    if ( !alp_ratio_min ( z0-2.0*z1+z2, 2.0*(z1-z0), z0, 
        u, -u, 0.25*(v+u), t_ptr ) ) 
        RETURN ( FALSE ) ;
    if ( proj_pt != NULL ) 
        c3r_pt_tan ( arc, *t_ptr, proj_pt, NULL ) ;
    RETURN ( TRUE ) ;
}


/*--------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_get_data ( arc, ctr, ept1, 
            rad_ptr, normal, sweep_ptr ) 
/*--------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 ctr, ept1, normal ;
REAL *rad_ptr, *sweep_ptr ;
{
    REAL r ;
    if ( rad_ptr == NULL ) 
        rad_ptr = &r ;

    if ( ept1 != NULL ) 
        C3V_COPY ( C3_ASEG_PT1(arc), ept1 ) ;
    if ( C3V_IS_ZERO ( C3_ASEG_D_VEC(arc) ) )
        RETURN ( FALSE ) ;
    *rad_ptr = c3r_ctr_rad ( arc, ctr ) ;
    if ( *rad_ptr < 0.0 ) 
        RETURN ( FALSE ) ;
    if ( normal != NULL ) {
        if ( c3r_normal ( arc, normal ) == NULL ) 
            RETURN ( FALSE ) ;
    }
    if ( sweep_ptr != NULL ) 
        *sweep_ptr = alr_sweep ( c3r_d_parm ( arc ) ) ;
    RETURN ( TRUE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_ctr_rad ( arc, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 ctr ;
{
    PT3 vec ;
    REAL u, v, p ;
    C3R_UV ( arc, vec, u, v ) ;
    if ( u <= BBS_ZERO * v ) 
        RETURN ( -1.0 ) ;
    if ( ctr != NULL ) {
        p = - 0.25 * ( v / u - 1.0 ) * sqrt ( v ) ;
        C3V_MID_PT ( C3_ASEG_PT1(arc), C3_ASEG_PT0(arc), ctr ) ;
        C3V_ADDT ( ctr, C3_ASEG_D_VEC(arc), p, ctr ) ;
    }
    RETURN ( 0.25 * ( v + u ) / sqrt ( u ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_rad ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
{
    RETURN ( c3r_ctr_rad ( arc, NULL ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL *c3r_ctr ( arc, ctr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 ctr ;
{
    RETURN ( c3r_ctr_rad ( arc, ctr ) < 0.0 ? NULL : ctr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL *c3r_normal ( arc, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 normal ;
{
    PT3 vec ;
    C3V_SUB ( C3_ASEG_PT1(arc), C3_ASEG_PT0(arc), vec ) ;
    C3V_CROSS ( C3_ASEG_D_VEC(arc), vec, normal ) ;
    RETURN ( c3v_normalize ( normal, normal ) ? normal : NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_PLANE_STATUS c3r_plane ( arc, normal, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 normal ;
REAL *dist_ptr ;
{
    if ( c3r_normal ( arc, normal ) == NULL ) 
        RETURN ( C3_PLANE_UNDER_DET ) ;
    *dist_ptr = C3V_DOT ( normal, C3_ASEG_PT0(arc) ) ;
    RETURN ( C3_PLANE_DET ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_on_plane ( arc, normal, dist ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 normal ;
REAL dist ;
{
    PT3 c ;

    RETURN ( c3v_pt_on_plane ( C3_ASEG_PT0(arc), normal, dist ) && 
             c3v_pt_on_plane ( C3_ASEG_PT1(arc), normal, dist ) && 
             c3v_pt_on_plane ( c3r_mid_pt ( arc, c ), normal, dist ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_is_normal ( arc, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 normal ;
{
    PT3 v ;
    if ( C3V_IS_SMALL ( C3_ASEG_D_VEC(arc) ) )
        RETURN ( TRUE ) ;
    C3V_SUB ( C3_ASEG_PT1(arc), C3_ASEG_PT0(arc), v ) ;
    RETURN ( IS_SMALL ( C3V_DOT ( v, normal ) ) &&
        IS_ZERO ( C3V_DOT ( C3_ASEG_D_VEC(arc), normal ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_curvature ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
{
    PT3 vec ;
    REAL u, v ;
    C3R_UV ( arc, vec, u, v ) ;
    RETURN ( 4.0 * sqrt ( u ) / ( v + u ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3r_sbdv ( arc, arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc, arc0, arc1 ;
{
    PT3 p0, p1, p2 ;

    c3r_pt_tan ( arc, 0.25, p0, NULL ) ;
    c3r_pt_tan ( arc, 0.5, p1, NULL ) ;
    c3r_pt_tan ( arc, 0.75, p2, NULL ) ;
    c3r_3pts ( C3_ASEG_PT0(arc), p0, p1, arc0 ) ;
    c3r_3pts ( p1, p2, C3_ASEG_PT1(arc), arc1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_trim ( arc0, t0, t1, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc0, arc1 ;
REAL t0, t1 ;
{
    PT3 a0, a1, a2 ;
    c3r_pt_tan ( arc0, t0, a0, NULL ) ;
    c3r_pt_tan ( arc0, 0.5*(t0+t1), a1, NULL ) ;
    c3r_pt_tan ( arc0, t1, a2, NULL ) ;
    c3r_3pts ( a0, a1, a2, arc1 ) ;
    RETURN ( arc1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_d_parm ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
{
    RETURN ( C3V_NORM ( C3_ASEG_D_VEC(arc) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL* c3r_get_d_vec ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
{
    RETURN ( C3_ASEG_D_VEC(arc) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_reverse ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc0, arc1 ;
{
    C3V_COPY ( C3_ASEG_PT0(arc0), C3_ASEG_PT1(arc1) ) ;
    C3V_COPY ( C3_ASEG_PT1(arc0), C3_ASEG_PT0(arc1) ) ;
    C3V_NEGATE ( C3_ASEG_D_VEC(arc0), C3_ASEG_D_VEC(arc1) ) ;
    RETURN ( arc1 ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_complement ( arc0, arc1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc0, arc1 ;
{
    REAL u, v, t ;
    PT3 vec ;

    C3R_UV ( arc0, vec, v, u ) ;
    t = - v / u ;
    C3V_COPY ( C3_ASEG_PT0(arc0), C3_ASEG_PT1(arc1) ) ;
    C3V_COPY ( C3_ASEG_PT1(arc0), C3_ASEG_PT0(arc1) ) ;
    C3V_SCALE ( C3_ASEG_D_VEC(arc0), t, C3_ASEG_D_VEC(arc1) ) ;
    RETURN ( arc1 ) ;
}    


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3r_plane_def ( arc, normal, dist_ptr, p, deg_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
INT *deg_ptr ;
{
    PT3 c ;

    c3v_plane_def ( C3_ASEG_PT0(arc), p, normal, dist_ptr, deg_ptr ) ;
    c3v_plane_def ( C3_ASEG_PT1(arc), p, normal, dist_ptr, deg_ptr ) ;
    c3v_plane_def ( c3r_mid_pt ( arc, c ), p, normal, dist_ptr, deg_ptr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_flatness ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ; 
{
    RETURN ( 0.5 * C3V_DIST ( C3_ASEG_PT0(arc), C3_ASEG_PT1(arc) ) * 
        fabs( c3r_d_parm ( arc ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_t_to_angle ( arc, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ; 
REAL t ;
{
    RETURN ( t <= 1.0 ? alr_angle ( t, c3r_d_parm ( arc ) ) :
        alr_angle ( t-1.0, 1.0 / c3r_d_parm ( arc ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL c3r_angle_to_t ( arc, angle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ; 
REAL angle ;
{
    REAL sweep ;
    sweep = alr_sweep ( c3r_d_parm ( arc ) ) ;
    RETURN ( angle <= sweep ? 
        alr_parm ( angle, c3r_d_parm ( arc ) ) : 
        alr_parm ( angle - sweep, 1.0 / c3r_d_parm ( arc ) ) + 1.0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3r_get_pts_d ( arc, t0, t1, pt0, pt1, d_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc ; 
REAL t0, t1 ;
PT3 pt0, pt1, d_vec ;
{
    if ( IS_SMALL(t0) && IS_SMALL(t1-1.0) ) {
        C3V_COPY ( C3_ASEG_PT0(arc), pt0 ) ;
        C3V_COPY ( C3_ASEG_PT1(arc), pt1 ) ;
        C3V_COPY ( C3_ASEG_D_VEC(arc), d_vec ) ;
        RETURN ( TRUE ) ;
    }
    else {
        C3_ASEG_S arc1 ;
        c3r_trim ( arc, t0, t1, &arc1 ) ;
        RETURN ( c3r_get_pts_d ( &arc1, 0.0, 1.0, pt0, pt1, d_vec ) ) ;
    }
}

