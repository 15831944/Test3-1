//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CD.C *********************************/
/************* Three-dimensional curves - polygonalization **************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3pdefs.h>
#include <c3ndefs.h>
#include <c3adefs.h>
#include <c3cdefs.h>
#include <c3cpriv.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_approx ( curve, parm1, draw_parm, gran, dir, 
        pt_buffer, parm_buffer, buf_size, index ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm1 ;
PARM draw_parm ;
REAL gran ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
{
    BOOLEAN end_bit = TRUE ;
    PARM_S start_parm ;

    *index = 0 ;
    COPY_PARM ( draw_parm, &start_parm ) ;

    if ( C3_CURVE_IS_LINE(curve) ) {
        end_bit = c3l_approx ( C3_CURVE_LINE(curve), PARM_T(&start_parm), 
            parm1 == NULL ? ( dir ? C3_CURVE_T1(curve) : C3_CURVE_T0(curve) ) 
            : PARM_T(parm1), 
            pt_buffer, parm_buffer, buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 0 ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        end_bit = c3g_approx ( C3_CURVE_ARC(curve), PARM_T(&start_parm), 
            parm1 == NULL ? ( dir ? C3_CURVE_T1(curve) : C3_CURVE_T0(curve) ) 
            : PARM_T(parm1), gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 0 ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        end_bit = c3p_approx ( C3_CURVE_PCURVE(curve), PARM_T(&start_parm), 
            parm1 == NULL ? ( dir ? C3_CURVE_T1(curve) : C3_CURVE_T0(curve) ) 
            : PARM_T(parm1), gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 0 ;
    }

#ifdef  SPLINE
    else
        end_bit = c3n_approx ( C3_CURVE_NURB(curve), &start_parm, 
            parm1, C3_CURVE_PARM0(curve), C3_CURVE_PARM1(curve), 
            gran, dir, pt_buffer, parm_buffer, buf_size, index, draw_parm ) ;
#endif  /*SPLINE*/


    if ( !end_bit ) {
        if ( dir ) 
            end_bit = ( PARM_T(draw_parm) >= 
                ( parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1) ) - 
                    BBS_ZERO ) ;
        else
            end_bit = ( PARM_T(draw_parm) <= 
            ( parm1 == NULL ? C3_CURVE_T0(curve) : PARM_T(parm1) ) + 
                BBS_ZERO ) ;
    }
    RETURN ( end_bit ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_approx_zoomed ( curve, parm1, draw_parm, gran, 
            view_ctr_pt, w, pt_buffer, parm_buffer, buf_size, index ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm1 ;
PARM draw_parm ;
REAL gran ;
PT3 view_ctr_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
{
    BOOLEAN end_bit = TRUE ;
    PARM_S start_parm ;

    *index = 0 ;
    COPY_PARM ( draw_parm, &start_parm ) ;

    if ( C3_CURVE_IS_LINE(curve) ) {
        end_bit = c3l_approx_zoomed ( C3_CURVE_LINE(curve), 
            PARM_T(&start_parm), 
            parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1), 
            view_ctr_pt, w, pt_buffer, parm_buffer, 
            buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 1 ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        end_bit = c3g_approx_zoomed ( C3_CURVE_ARC(curve), 
            PARM_T(&start_parm), 
            parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1), gran, 
            view_ctr_pt, w, pt_buffer, parm_buffer, 
            buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 1 ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        end_bit = c3p_approx_zoomed ( C3_CURVE_PCURVE(curve), 
            PARM_T(&start_parm), 
            parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1), gran, 
            view_ctr_pt, w, pt_buffer, parm_buffer, 
            buf_size, index, &PARM_T(draw_parm) ) ;
        PARM_J(draw_parm) = 1 ;
    }

#ifdef  SPLINE
    else
        end_bit = c3n_approx_zoomed ( C3_CURVE_NURB(curve), &start_parm, 
            parm1, C3_CURVE_PARM1(curve), gran, view_ctr_pt, w, pt_buffer, 
            parm_buffer, buf_size, index, draw_parm ) ;
#endif  /*SPLINE*/

    if ( !end_bit && PARM_T(draw_parm) >=
        ( parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1) ) - BBS_ZERO )
        end_bit = TRUE ;
    RETURN ( end_bit ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC void c3c_approx_init ( curve, parm0, draw_parm, dir ) 
/*----------------------------------------------------------------------*/
C3_CURVE curve ;
PARM parm0 ;
PARM draw_parm ;
BOOLEAN dir ;
{
    COPY_PARM ( parm0 != NULL ? parm0 : 
        ( dir ? C3_CURVE_PARM0(curve) : C3_CURVE_PARM1(curve) ), draw_parm ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void c3c_display ( curve, parm0, parm1, gran, view_ctr_pt, w, 
    display, display_data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm0, parm1 ;
PT3 view_ctr_pt ;
REAL gran, w ;
PF_DISPLAY display ;
ANY display_data ;
{
    PT3 pt_buffer[C3_DISPLAY_BUF_SIZE] ;
    PARM_S curve_parm ;
    INT index ;
    BOOLEAN zoomed, end_bit ;
    REAL boxsiz ;

    boxsiz = c3a_box_size ( C3_CURVE_BOX(curve) ) ;
    zoomed = w < 0.1 * boxsiz ;
    c3c_approx_init ( curve, parm0, &curve_parm, TRUE ) ;

    if (fabs(boxsiz)>1.0e30) //kga 19/11/02
      {//likely a problem!!!
      //temporary work around to prevent crash/lockup
      return; 
      }

    do {
        end_bit = zoomed ? 
            c3c_approx_zoomed ( curve, parm1, &curve_parm, gran, view_ctr_pt, 
                w, pt_buffer, NULL, C3_DISPLAY_BUF_SIZE, &index ) :
            c3c_approx ( curve, parm1, &curve_parm, gran, TRUE, 
                pt_buffer, NULL, C3_DISPLAY_BUF_SIZE, &index ) ;
            if ( display != PF_NULL ) 
                (*display) ( pt_buffer, index, display_data ) ;
    } while ( !end_bit ) ;
}

