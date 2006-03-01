//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CN.C *********************************/
/**************** Three-dimensional curves and planes *******************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <almdefs.h>
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3pdefs.h>
#include <c3ndefs.h>
#include <c3cdefs.h>
#include <c3vmcrs.h>

STATIC INT  c3c_curves_plane_def __(( DML_LIST, PT3, REAL*, PT3[3] )) ;
STATIC void c3c_curve_plane_def __(( C3_CURVE, PT3, REAL*, PT3[3], INT* )) ;

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_on_plane ( curve, normal, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 normal ;
REAL dist ;
{
    if ( C3_CURVE_IS_LINE(curve) ) 
        RETURN ( c3l_on_plane ( C3_CURVE_LINE(curve), normal, dist ) ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        RETURN ( c3g_on_plane ( C3_CURVE_ARC(curve), normal, dist ) ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( c3p_on_plane ( C3_CURVE_PCURVE(curve), normal, dist ) ) ;
    else 
#ifdef  SPLINE
        RETURN ( c3n_on_plane ( C3_CURVE_NURB(curve), normal, dist ) ) ;
#else
    RETURN ( FALSE ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_plane ( curve, normal, dist_ptr, status_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 normal ;
REAL *dist_ptr ;
C3_PLANE_STATUS *status_ptr ;
{
    C3_PLANE_STATUS status ;
    if ( C3_CURVE_IS_LINE(curve) ) 
        status = C3_PLANE_UNDER_DET ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        status = c3g_plane ( C3_CURVE_ARC(curve), normal, dist_ptr ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        status = c3p_plane ( C3_CURVE_PCURVE(curve), normal, dist_ptr ) ;
    else 
#ifdef  SPLINE
        status = c3n_plane ( C3_CURVE_NURB(curve), normal, dist_ptr ) ;
#else
    status = C3_PLANE_UNDER_DET ;
#endif  /* SPLINE */
    if ( status_ptr != NULL ) 
        *status_ptr = status ;
    RETURN ( status == C3_PLANE_DET ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_curves_plane ( curve_list, normal, dist_ptr, 
        status_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curve_list ;
PT3 normal ;
REAL *dist_ptr ;
C3_PLANE_STATUS *status_ptr ;
{
    INT deg ;
    PT3 p[2] ;

    deg = c3c_curves_plane_def ( curve_list, normal, dist_ptr, p ) ;
    if ( status_ptr != NULL ) {
        if ( deg <= 2 ) 
            *status_ptr = C3_PLANE_UNDER_DET ;
        else if ( deg >= 4 ) 
            *status_ptr = C3_PLANE_OVER_DET ;
        else 
            *status_ptr = C3_PLANE_DET ;
    }
    RETURN ( deg==3 ) ;
}


/*----------------------------------------------------------------------*/
STATIC INT c3c_curves_plane_def ( curve_list, normal, dist_ptr, p ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curve_list ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
{
    INT deg = 0 ;
    DML_ITEM item ;

    DML_WALK_LIST ( curve_list, item ) {
        c3c_curve_plane_def ( DML_RECORD(item), normal, dist_ptr, p, &deg ) ;
        if ( deg >= 4 ) 
            RETURN ( deg ) ;
    }
    RETURN ( deg ) ;
 }


/*----------------------------------------------------------------------*/
STATIC void c3c_curve_plane_def ( curve, normal, dist_ptr, p, deg_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 normal ;
REAL *dist_ptr ;
PT3 p[2] ;
INT *deg_ptr ;
{
    if ( C3_CURVE_IS_LINE(curve) ) 
        c3l_plane_def ( C3_CURVE_LINE(curve), normal, dist_ptr, p, deg_ptr ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        c3g_plane_def ( C3_CURVE_ARC(curve), normal, dist_ptr, p, deg_ptr ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        c3p_plane_def ( C3_CURVE_PCURVE(curve), normal, dist_ptr, 
            p, deg_ptr ) ;
#ifdef  SPLINE
    else 
        c3n_plane_def ( C3_CURVE_NURB(curve), normal, dist_ptr, p, deg_ptr ) ;
#endif  /* SPLINE */
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL c3c_pnts_plane_fit ( pt, n, normal, dist_ptr ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *pt ;
INT n ;
PT3 normal ;
REAL *dist_ptr ;
{
    PT3 q ;
    INT i ;
    REAL u, v, devn, *mtx ;

    C3V_SET_ZERO ( q ) ;
    for ( i = 0 ; i < n ; i++ ) {
        v = 1.0 / ( (REAL)i + 1 ) ;
        u = 1.0 - v ;
        C3V_ADDW ( q, u, pt[i], v, q ) ;
    }
    mtx = CREATE ( 3*n, REAL ) ;

    for ( i = 0 ; i < n ; i++ ) {
        C3V_SUB ( pt[i], q, mtx+3*i ) ;
    }
    if ( !alm_min_quad_form ( mtx, n, 3, normal, NULL ) ) 
        devn = -1.0 ;
    else {
        KILL ( mtx ) ;
        *dist_ptr = C3V_DOT ( q, normal ) ;
        devn = 0.0 ;
        for ( i = 0 ; i < n ; i++ ) {
            v = fabs ( C3V_DOT ( pt[i], normal ) - *dist_ptr ) ;
            if ( v > devn )
                devn = v ;
        }
    }
    RETURN ( sqrt ( devn ) ) ;
}

