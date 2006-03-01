//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3N2.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <aladefs.h>
#include <c3apriv.h>
#include <c2ndefs.h>
#include <c3ndefs.h>
#include <c2nmcrs.h>
#include <c3nmcrs.h>
#include <c3vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PRIVATE C2_NURB c3n_convert_3d2d ( nurb_3d, origin, x_axis, y_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb_3d ;
PT3 origin, x_axis, y_axis ;
{
    C2_NURB nurb_2d ;

    nurb_2d = c2n_create_nurb ( C3_NURB_N(nurb_3d), C3_NURB_D(nurb_3d) ) ;
    if ( nurb_2d != NULL ) {
        ala_copy ( C3_NURB_KNOT(nurb_3d), C3_NURB_N(nurb_3d) + 
            C3_NURB_D(nurb_3d), C2_NURB_KNOT(nurb_2d) ) ;
        c3a_conv_hpt_3d2d ( C3_NURB_CTPT(nurb_3d), C3_NURB_N(nurb_3d),
            origin, x_axis, y_axis, C2_NURB_CTPT(nurb_2d) ) ;
        C2_NURB_W(nurb_2d) = C3_NURB_W(nurb_3d) ;
    }
    RETURN ( nurb_2d ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_convert_2d3d ( nurb_2d, origin, x_axis, y_axis ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_NURB nurb_2d ;
PT3 origin, x_axis, y_axis ;
{
    C3_NURB nurb_3d ;

    nurb_3d = c3n_create_nurb ( C2_NURB_N(nurb_2d), C2_NURB_D(nurb_2d) ) ;
    if ( nurb_3d != NULL ) {
        ala_copy ( C2_NURB_KNOT(nurb_2d), C2_NURB_N(nurb_2d) + 
            C2_NURB_D(nurb_2d), C3_NURB_KNOT(nurb_3d) ) ;
        c3a_conv_hpt_2d3d ( C2_NURB_CTPT(nurb_2d), C2_NURB_N(nurb_2d),
            origin, x_axis, y_axis, C3_NURB_CTPT(nurb_3d) ) ;
        C3_NURB_W(nurb_3d) = C2_NURB_W(nurb_2d) ;
        C3V_CROSS ( x_axis, y_axis, C3_NURB_NORMAL(nurb_3d) ) ;
    }
    RETURN ( nurb_3d ) ;
}
#endif  /*SPLINE*/

