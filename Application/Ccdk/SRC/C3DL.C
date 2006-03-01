//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3DL.C *********************************/
/********* Three-dimensional curve construction routines ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3ldefs.h>
#include <c3ddefs.h>
#include <c3dpriv.h>
#include <qgldefs.h>
#include <c3vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_line ( ept0, ept1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 ept0, ept1 ;
{
    C3_CURVE curve ;

    curve = c3d_curve() ;
    if ( curve != NULL ) {
        C3_CURVE_TYPE(curve) = C3_LINE_TYPE ;
        C3_CURVE_T0(curve) = 0.0 ;
        C3_CURVE_J0(curve) = 1 ;
        C3_CURVE_T1(curve) = 1.0 ;
        C3_CURVE_J1(curve) = 2 ;

        C3_CURVE_LINE(curve) = c3l_create_2pts ( ept0, ept1 ) ;
        c3l_box ( C3_CURVE_LINE(curve), 0.0, 1.0, C3_CURVE_BOX(curve) ) ;
    }
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_line_dir ( ept0, dir_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

PT3 ept0, dir_vec ;
{
    PT3 ept1 ;

    C3V_ADD ( ept0, dir_vec, ept1 ) ;
    RETURN ( c3d_line ( ept0, ept1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_ray ( origin, theta, phi ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 origin ;
REAL theta, phi ;
{
    PT3 dir_vec ;
    REAL w_size = qgl_get_world_size (), c = cos ( phi ) * w_size ;

    dir_vec[0] = cos ( theta ) * c ;
    dir_vec[1] = sin ( theta ) * c ;
    dir_vec[1] = sin ( phi ) * w_size ;
    RETURN ( c3d_line_dir ( origin, dir_vec ) ) ;
}

