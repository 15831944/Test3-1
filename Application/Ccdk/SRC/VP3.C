//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************* VP3.C ***********************************/
/********************** Display viewports management ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3vdefs.h>
#include <grrdefs.h>
#include <vp3defs.h>
#include <v3ddefs.h>
#include <vpidefs.h>
#include <c3vmcrs.h>
#include <vpmcrs.h>

#define xdbgvpi

extern FILE *dbgf;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_top ( x, y, z, w, h, px0, py0, px1, py1, 
            bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    PT3 view_ctr_pt, view_x_vec, view_y_vec ;

    RETURN ( vpi_create ( c3v_set ( x, y, z, view_ctr_pt ) , 
        c3v_set ( 0.5*w, 0.0, 0.0, view_x_vec ),
        c3v_set ( 0.0, 0.5*h, 0.0, view_y_vec ), 
        px0, py0, px1, py1, bd_color, bk_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_front ( x, y, z, w, h, px0, py0, px1, py1, 
        bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    PT3 view_ctr_pt, view_x_vec, view_y_vec ;

    RETURN ( vpi_create ( c3v_set ( x, y, z, view_ctr_pt ) , 
        c3v_set ( 0.5*w, 0.0, 0.0, view_x_vec ),
        c3v_set ( 0.0, 0.0, 0.5*h, view_y_vec ), 
        px0, py0, px1, py1, bd_color, bk_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_right ( x, y, z, w, h, px0, py0, px1, py1, 
        bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    PT3 view_ctr_pt, view_x_vec, view_y_vec ;

    RETURN ( vpi_create ( c3v_set ( x, y, z, view_ctr_pt ) , 
        c3v_set ( 0.0, 0.5*w, 0.0, view_x_vec ),
        c3v_set ( 0.0, 0.0, 0.5*h, view_y_vec ), 
        px0, py0, px1, py1, bd_color, bk_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_isometric ( x, y, z, w, h, px0, py0, px1, py1, 
    bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    PT3 view_ctr_pt, view_x_vec, view_y_vec ;

    w *= (0.5/SQRT_2) ;
    h *= (0.5/sqrt(6.0)) ;
    RETURN ( vpi_create ( c3v_set ( x, y, z, view_ctr_pt ) , 
        c3v_set ( w, w, 0.0, view_x_vec ), 
        c3v_set ( -h, h, 2.0*h, view_y_vec ), 
        px0, py0, px1, py1, bd_color, bk_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_create ( view_ctr_pt, eye_pt, view_up, w, h, 
    px0, py0, px1, py1, bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 view_ctr_pt, eye_pt, view_up ;
REAL w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    PT3 view_x_vec, view_y_vec, view_z_vec ;
    REAL t ;

    C3V_SUB ( eye_pt, view_ctr_pt, view_z_vec ) ;
    if ( C3V_IS_SMALL(view_z_vec) )
        RETURN ( NULL ) ;

    t = - C3V_DOT ( view_up, view_z_vec ) / 
        C3V_DOT ( view_z_vec, view_z_vec ) ;
    C3V_ADDT ( view_up, view_z_vec, t, view_y_vec ) ;
    if ( !c3v_normalize ( view_y_vec, view_y_vec ) ) 
        RETURN ( NULL ) ;
    C3V_CROSS ( view_y_vec, view_z_vec, view_x_vec ) ;
    if ( !c3v_normalize ( view_x_vec, view_x_vec ) ) 
        RETURN ( NULL ) ;
    w *= 0.5 ;
    h *= 0.5 ;
    C3V_SCALE ( view_x_vec, w, view_x_vec ) ;
    C3V_SCALE ( view_y_vec, h, view_y_vec ) ;
    RETURN ( vpi_create ( view_ctr_pt, view_x_vec, view_y_vec, 
        px0, py0, px1, py1, bd_color, bk_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_set_view_box ( vp, view_box )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C3_BOX view_box ;
{
    RETURN ( vp3_fit ( vp, view_box, 1.0 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_fit ( vp, box, border_factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C3_BOX box ;
REAL border_factor ;
{
    REAL xx, xy, yy, a, w0, w1, w ;
    INT i ;

    C3V_MID_PT ( C3_MIN_PT(box), C3_MAX_PT(box), 
        VP_VIEWPORT_VIEW_CTR_PT(vp) ) ;
    xx = C3V_DOT ( VP_VIEWPORT_VIEW_X_VEC(vp), VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    xy = C3V_DOT ( VP_VIEWPORT_VIEW_X_VEC(vp), VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;
    yy = C3V_DOT ( VP_VIEWPORT_VIEW_Y_VEC(vp), VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;

    w0 = w1 = 0.0 ;
    for ( i=0 ; i<3 ; i++ ) {
        a = fabs ( C3_MAX_PT(box)[i] - C3_MIN_PT(box)[i] ) ;
        w0 += ( a * fabs ( VP_VIEWPORT_VIEW_X_VEC(vp)[i] * yy - 
            VP_VIEWPORT_VIEW_Y_VEC(vp)[i] * xy ) ) ;
        w1 += ( a * fabs ( VP_VIEWPORT_VIEW_Y_VEC(vp)[2] * xx - 
            VP_VIEWPORT_VIEW_X_VEC(vp)[0] * xy ) ) ; 
    }

    w = w0 < w1 ? w1 : w0 ;
    w *= ( 0.5 * border_factor / ( xx * yy - xy * xy ) ) ;

    C3V_SCALE ( VP_VIEWPORT_VIEW_X_VEC(vp), w, VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    C3V_SCALE ( VP_VIEWPORT_VIEW_Y_VEC(vp), w, VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;
    RETURN ( vpi_setup ( vp ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL* vp3_get_ctr ( vp, ctr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT3 ctr ;
{
    C3V_COPY ( VP_VIEWPORT_VIEW_CTR_PT(vp), ctr ) ;
    RETURN ( ctr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_set_ctr ( vp, ctr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT3 ctr ;
{
#ifdef dbgvpi
  fprintf(dbgf, "vpi_set_ctr_1\n");
#endif

    C3V_COPY ( ctr, VP_VIEWPORT_VIEW_CTR_PT(vp) ) ;

#ifdef dbgvpi
  fprintf(dbgf, "vpi_set_ctr_2\n");
#endif
    RETURN ( vpi_setup ( vp ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT* vp3_point_to_pixel ( pt, vp, pxl ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
VP_VIEWPORT vp ;
INT pxl[2] ;
{
    PT2 a ;
    V3D_PT_TRANSFORM ( pt, vp, a ) ;
    RETURN ( grr_point_to_pixel ( a, pxl ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL* vp3_pixel_to_point ( pxl, vp, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT pxl[2] ;
VP_VIEWPORT vp ;
PT3 pt ;
{
    PT2 a ;

    grr_pixel_to_point ( pxl, a ) ;
    pt[0] = VP_VIEWPORT_INVERSE(vp)[0][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[0][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[0][2] ; 
    pt[1] = VP_VIEWPORT_INVERSE(vp)[1][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[1][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[1][2] ;
    pt[2] = VP_VIEWPORT_INVERSE(vp)[2][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[2][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[2][2] ;
    /*    fprintf(dbgf, "vp3_pixel_to_point %i %i --> %g %g\n",pxl[0],pxl[1],pt[0],pt[1]); */
    RETURN ( pt ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL* vp3_pixel_to_point_real( pxl, vp, pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL pxl[2] ;
VP_VIEWPORT vp ;
PT3 pt ;
{
    PT2 a ;

    grr_pixel_to_point_real ( pxl, a ) ;
    pt[0] = VP_VIEWPORT_INVERSE(vp)[0][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[0][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[0][2] ; 
    pt[1] = VP_VIEWPORT_INVERSE(vp)[1][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[1][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[1][2] ;
    pt[2] = VP_VIEWPORT_INVERSE(vp)[2][0] * a[0] + 
        VP_VIEWPORT_INVERSE(vp)[2][1] * a[1] + VP_VIEWPORT_INVERSE(vp)[2][2] ;
    /*    fprintf(dbgf, "vp3_pixel_to_point %i %i --> %g %g\n",pxl[0],pxl[1],pt[0],pt[1]); */
    RETURN ( pt ) ;
}



/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vp3_get_view ( vp, ctr, x_vec, y_vec )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT3 ctr, x_vec, y_vec ;
{
    if ( ctr != NULL ) 
        C3V_COPY ( VP_VIEWPORT_VIEW_CTR_PT(vp), ctr ) ;
    if ( x_vec != NULL ) 
        C3V_COPY ( VP_VIEWPORT_VIEW_X_VEC(vp), x_vec ) ;
    if ( y_vec != NULL ) 
        C3V_COPY ( VP_VIEWPORT_VIEW_Y_VEC(vp), y_vec ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_set_view ( vp, ctr, x_vec, y_vec )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
PT3 ctr, x_vec, y_vec ;
{
    if ( ctr != NULL ) 
        C3V_COPY ( ctr, VP_VIEWPORT_VIEW_CTR_PT(vp) ) ;
    if ( x_vec != NULL ) 
        C3V_COPY ( x_vec, VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    if ( y_vec != NULL ) 
        C3V_COPY ( y_vec, VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;
    RETURN ( vpi_setup ( vp ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void vp3_get_view_dir ( vp, theta_ptr, phi_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
REAL *theta_ptr, *phi_ptr ;
{
    PT3 view_dir ;

    C3V_CROSS ( VP_VIEWPORT_VIEW_X_VEC(vp), VP_VIEWPORT_VIEW_Y_VEC(vp), 
        view_dir ) ;
/*
    C3V_NEGATE ( view_dir, view_dir ) ;
*/
    c3v_spherical ( view_dir, NULL, theta_ptr, phi_ptr ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_set_view_dir ( vp, theta, phi ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
REAL theta, phi ;
{
    PT3 view_dir ;
    REAL t, nx, ny ;

    c3v_set_spherical ( 1.0, theta, phi, view_dir ) ;
/*
    C3V_NEGATE ( view_dir, view_dir ) ;
*/
    nx = C3V_NORM ( VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    ny = C3V_NORM ( VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;

    t = - C3V_DOT ( VP_VIEWPORT_VIEW_Y_VEC(vp), view_dir ) ;
    C3V_ADDT ( VP_VIEWPORT_VIEW_Y_VEC(vp), view_dir, t, 
        VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;
    if ( !c3v_normalize ( VP_VIEWPORT_VIEW_Y_VEC(vp), 
        VP_VIEWPORT_VIEW_Y_VEC(vp) ) ) 
        RETURN ( NULL ) ;
    C3V_CROSS ( VP_VIEWPORT_VIEW_Y_VEC(vp), view_dir, 
        VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    if ( !c3v_normalize ( VP_VIEWPORT_VIEW_X_VEC(vp), 
        VP_VIEWPORT_VIEW_X_VEC(vp) ) ) 
        RETURN ( NULL ) ;
    C3V_SCALE ( VP_VIEWPORT_VIEW_X_VEC(vp), nx, VP_VIEWPORT_VIEW_X_VEC(vp) ) ;
    C3V_SCALE ( VP_VIEWPORT_VIEW_Y_VEC(vp), ny, VP_VIEWPORT_VIEW_Y_VEC(vp) ) ;
    vpi_setup ( vp ) ;
    RETURN ( vp ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT vp3_append_transform ( vp, t ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT vp ;
C3_TRANSFORM t ;
{
    INT i, j ;
    VP_TRANSFORM vt ;
    vpi_get_transform ( vp, vt ) ;

    for ( i=0 ; i<2 ; i++ ) {
        for ( j=0 ; j<4 ; j++ ) 
            VP_VIEWPORT_TRANSFORM(vp)[i][j] = 
                vt[i][0]*t[0][j] + vt[i][1]*t[1][j] + vt[i][2]*t[2][j] ;
        VP_VIEWPORT_TRANSFORM(vp)[i][3] += vt[i][3] ;
    }
    RETURN ( vp ) ;
}

