//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3CW.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef __NDP__
#include <string.h>
#include <alwdefs.h>
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3pdefs.h>
#include <c3ndefs.h>
#include <c3cdefs.h>
#include <c3cpriv.h>
#include <c3ddefs.h>
#include <c3dpriv.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST c3c_read_curves ( filename, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST curves_list ;
{
    FILE *file ;

    file = fopen ( filename, "r" ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    curves_list = c3c_get_curves ( file, curves_list ) ;
    fclose ( file ) ;
    RETURN ( curves_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST c3c_get_curves ( file, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
DML_LIST curves_list ;
{
    C3_CURVE curve ;

    while ( TRUE ) {
        curve = c3c_get_curve ( file ) ; 
        if ( curve == NULL ) 
            RETURN ( curves_list ) ;
        if ( curves_list == NULL ) 
            curves_list = dml_create_list () ;
        dml_append_data ( curves_list, curve ) ;
    }
#ifndef __UNIX__
    RETURN ( NULL ) ;
#endif
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_CURVE c3c_get_curve ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
{
    char name[12] ;
    C3_CURVE curve ;

    if ( !alw_get_string ( file, name, 12 ) )
        RETURN ( NULL ) ;
    curve = c3d_curve () ;
    if ( strcmpi ( name, "line" ) == 0 ) {
        C3_CURVE_LINE(curve) = c3l_get ( file, 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
        C3_CURVE_TYPE(curve) = C3_LINE_TYPE ;
    }
    else if ( strcmpi ( name, "arc" ) == 0 ) {
        C3_CURVE_ARC(curve) = c3g_get ( file, 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
        C3_CURVE_TYPE(curve) = C3_ARC_TYPE ;
    }
    else if ( strcmpi ( name, "pcurve" ) == 0 ) {
        C3_CURVE_PCURVE(curve) = c3p_get ( file, 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
        C3_CURVE_TYPE(curve) = C3_PCURVE_TYPE ;
    }
#ifdef SPLINE
    else if ( strcmpi ( name, "spline" ) == 0 ||
              strcmpi ( name, "nurb" ) == 0 ) {
        C3_CURVE_NURB(curve) = c3n_get ( file, 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
        C3_CURVE_TYPE(curve) = C3_NURB_TYPE ;
    }
    else if ( strcmpi ( name, "ellipse" ) == 0 ) {
        C3_CURVE_NURB(curve) = c3n_get ( file, 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
        C3_CURVE_TYPE(curve) = C3_ELLIPSE_TYPE ;
    }
    else if ( strcmpi ( name, "ellipse" ) == 0 ) {
        C3_CURVE_NURB(curve) = c3n_get ( file, 
            C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
        C3_CURVE_TYPE(curve) = C3_BEZIER_TYPE ;
    }
#endif
    else
        C3_CURVE_LINE(curve) = NULL ;
    if ( C3_CURVE_LINE(curve) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    c3c_box ( curve ) ;
    RETURN ( curve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_write_curves ( filename, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST curves_list ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = c3c_put_curves ( file, curves_list ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_write_curve ( filename, curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
C3_CURVE curve ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = c3c_put_curve ( file, curve ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_put_curves ( file, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
DML_LIST curves_list ;
{
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    DML_WALK_LIST ( curves_list, item ) {
        status = c3c_put_curve ( file, DML_RECORD(item) ) && status ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_put_curve ( file, curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
C3_CURVE curve ;
{
    BOOLEAN status ;

    if ( C3_CURVE_IS_LINE(curve) ) 
        status = alw_put_string ( file, "line", NULL ) && 
            c3l_put ( file, C3_CURVE_LINE(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        status = alw_put_string ( file, "arc", NULL ) && 
            c3g_put ( file, C3_CURVE_ARC(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        status = alw_put_string ( file, "pcurve", NULL ) && 
            c3p_put ( file, C3_CURVE_PCURVE(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
#ifdef SPLINE 
    else if ( C3_CURVE_IS_ELLIPSE(curve) ) 
        status = alw_put_string ( file, "ellipse", NULL ) && 
            c3n_put ( file, C3_CURVE_NURB(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
    else if ( C3_CURVE_IS_SPLINE(curve) ) 
        status = alw_put_string ( file, "spline", NULL ) && 
            c3n_put ( file, C3_CURVE_NURB(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
    else if ( C3_CURVE_IS_BEZIER(curve) ) 
        status = alw_put_string ( file, "bezier", NULL ) && 
            c3n_put ( file, C3_CURVE_NURB(curve), 
                C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve) ) ;
#endif
    else
        status = FALSE ;
    RETURN ( status ) ;
}
#endif

