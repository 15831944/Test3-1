//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3DP.C *********************************/
/********** Three-dimensional polycurve construction routines ***********/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3cdefs.h>
#include <c3pdefs.h>
#include <c3rdefs.h>
#include <c3adefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>
#include <c3vmcrs.h>
STATIC C3_CURVE c3d_pcurve_add __(( C3_CURVE, PT3, PT3, PT3, REAL )) ;

/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE c3d_pcurve_frame ( s, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT s, n ;
{
    C3_CURVE curve ;

    curve = c3d_curve ();
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_TYPE(curve) = C3_PCURVE_TYPE ;
    C3_CURVE_T0(curve) = 0.0 ;
    C3_CURVE_J0(curve) = 0 ;
    C3_CURVE_T1(curve) = (REAL)(n-1) ;
    C3_CURVE_J1(curve) = n-2 ;
    C3_CURVE_PCURVE(curve) = c3p_create ( s, n ) ;
    if ( C3_CURVE_PCURVE(curve) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_through ( a, n ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a      ;   /* a[n] */
INT n ;
{
    C3_CURVE curve ;

    curve = c3d_curve ();
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    C3_CURVE_TYPE(curve) = C3_PCURVE_TYPE ;
    C3_CURVE_T0(curve) = 0.0 ;
    C3_CURVE_J0(curve) = 0 ;
    C3_CURVE_T1(curve) = (REAL)(n-1) ;
    C3_CURVE_J1(curve) = n-2 ;
    C3_CURVE_PCURVE(curve) = c3p_through ( a, n ) ;
    if ( C3_CURVE_PCURVE(curve) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_init_size ( a, size ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
INT size ;
{
    C3_CURVE curve ;

    curve = c3d_pcurve_frame ( size, 1 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c3p_init ( C3_CURVE_PCURVE(curve), a ) ;
    c3a_box_init_pt ( C3_CURVE_BOX(curve), a ) ;

    curve->color=0;        /* mhm/cnm addition */


//    memset(curve->layer, 0, sizeof(curve->layer));     /* mhm/cnm addition */
    C3_CURVE_LAYER_SET(curve,NULL);


    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_init ( a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
{
    RETURN ( c3d_pcurve_init_size ( a, 10 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_add_arc_2pts ( curve, a0, a1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 a0, a1 ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    if ( c3p_add_arc_2pts ( C3_CURVE_PCURVE(curve), a0, a1 ) == NULL )
        RETURN ( NULL ) ;
    c3p_box_append ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_add_arc_ctr_pt ( curve, ctr, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 ctr, a ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    if ( c3p_add_arc_ctr_pt ( C3_CURVE_PCURVE(curve), ctr, a ) == NULL )
        RETURN ( NULL ) ;
    c3p_box_append ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_add_arc_tan ( curve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 a ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    if ( c3p_add_arc_tan ( C3_CURVE_PCURVE(curve), a ) == NULL )
        RETURN ( NULL ) ;
    c3p_box_append ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_add_line ( curve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 a ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( c3p_add_line ( C3_CURVE_PCURVE(curve), a ) == NULL )
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    c3a_box_append_pt ( C3_CURVE_BOX(curve), a ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_add_line_tan ( curve, a ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 a ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( c3p_add_line_tan ( C3_CURVE_PCURVE(curve), a ) == NULL )
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    c3p_box_append ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_remove_last ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) -= 1.0 ;
    C3_CURVE_J1(curve) -= 1 ;
    RETURN ( c3p_remove_last ( C3_CURVE_PCURVE(curve) ) == NULL ? 
        NULL : curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_close ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    c3p_close ( C3_CURVE_PCURVE(curve) ) ;
    RETURN ( curve ) ;
}

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_segment ( curve, i ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
INT i ;
{
    PT3 pt0, pt1, d_vec ;

    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( !c3p_segment_epts_d ( C3_CURVE_PCURVE(curve), i, pt0, pt1, d_vec ) )
        RETURN ( NULL ) ;
    RETURN ( C3V_IS_ZERO ( d_vec ) ? 
        c3d_line ( pt0, pt1 ) : c3d_arc ( pt0, pt1, d_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST c3d_pcurve_smash ( curve, curve_list ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
DML_LIST curve_list ;
{
    C3_CURVE crv ;
    INT i ;

    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( curve_list == NULL ) 
        curve_list = dml_create_list () ;

    for ( i = 0 ; ; i++ ) {
        crv = c3d_pcurve_segment ( curve, i ) ;
        if ( crv == NULL ) 
            RETURN ( curve_list ) ;
        dml_append_data ( curve_list, crv ) ;
    }
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*----------------------- TEMP -----CNM---------------------------------*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

BBS_PUBLIC DML_LIST c3c_sort_curve_list(a , b, c, d, e)
DML_LIST a;
PF_EVAL3 b; 
PF_EVAL3 c; 
BOOLEAN  d; 
DML_LIST e;
  {
  RETURN (a);
  };


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_curves ( curve_list, tol ) 
/*----------------------------------------------------------------------*/
/***** The curves on the curve_list are deleted *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curve_list ;
REAL tol ;
{
    C3_CURVE curve, pcurve ;
    DML_ITEM item ;
    PT3 p0 ;

    c3c_sort_curve_list ( curve_list, (PF_EVAL3)c3c_ept0, (PF_EVAL3)c3c_ept1, 
        FALSE, curve_list ) ;

    curve = (C3_CURVE) dml_first_record ( curve_list ) ;
    c3c_ept0 ( curve, p0 ) ;
    pcurve = c3d_pcurve_init ( p0 ) ;

    DML_WALK_LIST ( curve_list, item ) {
        curve = (C3_CURVE) DML_RECORD(item) ;
        if ( c3d_pcurve_add_curve ( pcurve, curve, tol ) == NULL ) 
            RETURN ( pcurve ) ;
    }
    RETURN ( pcurve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST c3d_pcurve_list ( curve_list, tol, pcurve_list ) 
/*----------------------------------------------------------------------*/
/***** The curves on the curve_list are deleted *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST curve_list ;
REAL tol ;
DML_LIST pcurve_list ;
{
    C3_CURVE curve, pcurve ;
    DML_ITEM item ;
    PT3 p0 ;

    c3c_sort_curve_list ( curve_list, (PF_EVAL3)c3c_ept0, 
        (PF_EVAL3)c3c_ept1, FALSE, curve_list ) ;
    if ( pcurve_list == NULL ) 
        pcurve_list = dml_create_list () ;

    curve = (C3_CURVE) dml_first_record ( curve_list ) ;
    c3c_ept0 ( curve, p0 ) ;
    pcurve = c3d_pcurve_init ( p0 ) ;

    DML_WALK_LIST ( curve_list, item ) {
        curve = (C3_CURVE) DML_RECORD(item) ;
        if ( c3d_pcurve_add_curve ( pcurve, curve, tol ) == NULL ) {
            dml_append_data ( pcurve_list, pcurve ) ;
            c3c_ept0 ( curve, p0 ) ;
            pcurve = c3d_pcurve_init ( p0 ) ;
            c3d_pcurve_add_curve ( pcurve, curve, tol ) ;
        }
    }
    RETURN ( pcurve_list ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_pcurve_add_curve ( pcurve, curve, tol ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE pcurve, curve ;
REAL tol ;
{
    PT3 pt0, pt1, d_vec ;

    c3c_get_arc_pts_d ( curve, pt0, pt1, d_vec ) ;
    RETURN ( c3d_pcurve_add ( pcurve, pt0, pt1, d_vec, tol ) ) ;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*------------------------TEMP -------CNM-------------------------------*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

BBS_PRIVATE C3_PCURVE c3p_add ( curve, pt0, pt1, d_vec, tol)
C3_PCURVE curve ;
PT3 pt0, pt1,d_vec ;
REAL tol ;
  {
    RETURN ( curve ) ;
  }

/*----------------------------------------------------------------------*/
STATIC C3_CURVE c3d_pcurve_add ( curve, pt0, pt1, d_vec, tol ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 pt0, pt1, d_vec ;
REAL tol ;
{
    if ( !C3_CURVE_IS_PCURVE(curve) ) 
        RETURN ( NULL ) ;
    if ( c3p_add ( C3_CURVE_PCURVE(curve), pt0, pt1, d_vec, tol ) == NULL )
        RETURN ( NULL ) ;
    C3_CURVE_T1(curve) += 1.0 ;
    C3_CURVE_J1(curve) += 1 ;
    c3p_box_append ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
    RETURN ( curve ) ;
}

