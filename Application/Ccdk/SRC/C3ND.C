//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3ND.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3ndefs.h>
#include <c3sdefs.h>
#include <c3nmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3n_approx ( nurb, start_parm, end_parm, parm0, parm1, 
        gran, dir, pt_buffer, parm_buffer, buf_size, index, draw_parm ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM start_parm, end_parm, parm0, parm1 ;
REAL gran ;
BOOLEAN dir ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM draw_parm ;
{
    RETURN ( c3s_approx ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb),
            C3_NURB_D(nurb), C3_NURB_KNOT(nurb), C3_NURB_W(nurb), 
            C3_NURB_NORMAL(nurb), start_parm, end_parm != NULL ? end_parm : 
            ( dir ? parm1 : parm0 ), gran, dir, 
            pt_buffer, parm_buffer, buf_size, index, draw_parm ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3n_approx_zoomed ( nurb, start_parm, end_parm, parm1, 
gran, view_ctr_pt, w, pt_buffer, parm_buffer, buf_size, index, draw_parm ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_NURB nurb ;
PARM start_parm, end_parm, parm1 ;
REAL gran ;
PT3 view_ctr_pt ;
REAL w ;
PT3 *pt_buffer ;
PARM parm_buffer ;
INT buf_size, *index ;
PARM draw_parm ;
{
    RETURN ( c3s_approx_zoomed ( C3_NURB_CTPT(nurb), C3_NURB_N(nurb), 
        C3_NURB_D(nurb), C3_NURB_KNOT(nurb), C3_NURB_W(nurb), 
        C3_NURB_NORMAL(nurb), start_parm, end_parm != NULL ? end_parm : parm1, 
        gran, view_ctr_pt, w, pt_buffer, parm_buffer, 
        buf_size, index, draw_parm ) ) ;
}
#endif  /*SPLINE*/

