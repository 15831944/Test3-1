//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3C.C **********************************/
/**************** Three-dimensional curves ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3gdefs.h>
#include <c3ldefs.h>
#include <c3ndefs.h>
#include <c3pdefs.h>
#include <c3rdefs.h>
#include <c3sdefs.h>
#include <c3adefs.h>
#include <c3cdefs.h>
#include <c3cpriv.h>
#include <c3vdefs.h>
#include <c3vmcrs.h>

#ifdef  SPLINE
STATIC void length3_eval __(( PT3*, REAL* )) ;
#endif  /*SPLINE*/
/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_closed ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    PT3 ept0, ept1 ;
    if ( IS_ZERO ( C3_CURVE_T1(curve) - C3_CURVE_T0(curve) ) ) 
        RETURN ( TRUE ) ;   /* degenerate curve */
    else if ( C3_CURVE_IS_LINE(curve) )
        RETURN ( FALSE ) ;
    else if ( C3_CURVE_IS_ARC(curve) )
        RETURN ( C3_CURVE_T0(curve) <= BBS_ZERO &&
            C3_CURVE_T1(curve) >= 2.0 - BBS_ZERO ) ;
    else 
        RETURN ( c3c_ept0 ( curve, ept0 ) && c3c_ept1 ( curve, ept1 ) 
            && C3V_IDENT_PTS ( ept0, ept1 ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_project ( curve, pt, parm, proj_pt )
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 pt ;
PARM parm ;
PT3 proj_pt ;
{
    if ( C3_CURVE_IS_LINE(curve) ) {
        PARM_T(parm) = c3l_project ( C3_CURVE_LINE(curve), pt, proj_pt ) ;
        PARM_J(parm) = 1 ;
        RETURN ( PARM_T(parm) >= C3_CURVE_T0(curve) && 
                 PARM_T(parm) <= C3_CURVE_T1(curve) ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        if ( !c3g_project ( C3_CURVE_ARC(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), pt, &PARM_T(parm), proj_pt ) )
            RETURN ( FALSE ) ;
        PARM_J(parm) = (INT)PARM_T(parm) ;
        RETURN ( TRUE ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        if ( !c3p_project ( C3_CURVE_PCURVE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), pt, &PARM_T(parm), proj_pt ) )
            RETURN ( FALSE ) ;
        PARM_J(parm) = (INT)PARM_T(parm) ;
        RETURN ( TRUE ) ;
    }
    else 
#ifdef  SPLINE
        RETURN ( c3n_project ( C3_CURVE_NURB(curve), C3_CURVE_PARM0(curve), 
            C3_CURVE_PARM1(curve), pt, parm, proj_pt ) ) ;
#else
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC REAL c3c_length ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
{
#ifdef  SPLINE
    REAL length ;
#endif  /*SPLINE*/
    if ( C3_CURVE_IS_LINE(curve) )
        RETURN ( c3l_length ( C3_CURVE_LINE(curve), 
            C3_CURVE_T0(curve), C3_CURVE_T1(curve) ) ) ;
    else if ( C3_CURVE_IS_ARC(curve) )
        RETURN ( c3g_length ( C3_CURVE_ARC(curve), 
            C3_CURVE_T0(curve), C3_CURVE_T1(curve) ) ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) )
        RETURN ( c3p_length ( C3_CURVE_PCURVE(curve), 
            C3_CURVE_T0(curve), C3_CURVE_T1(curve) ) ) ;
    else 
#ifdef  SPLINE
        c3n_mass_prop ( C3_CURVE_NURB(curve), C3_CURVE_PARM0(curve), 
            C3_CURVE_PARM1(curve), 
            1, 1, LENGTH_TOL, length3_eval, &length ) ;
        RETURN ( length ) ;
#else  /*SPLINE*/
        RETURN ( 0.0 ) ;
#endif  /*SPLINE*/
}


#ifdef  SPLINE
/*----------------------------------------------------------------------*/
STATIC void length3_eval ( x, result ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *x ;
REAL *result ;
{
    *result = C3V_NORM ( x[1] ) ;
}
#endif  /*SPLINE*/


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3c_midpt_dir ( curve, dir, pt, tan_vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 pt, tan_vec ;
INT dir ;
{
    PARM_S parm ;

    PARM_T(&parm) = 0.5 * ( C3_CURVE_T0(curve) + C3_CURVE_T1(curve) ) ;
    PARM_J(&parm) = (INT) ( 0.5 * ( (REAL)C3_CURVE_J0(curve) + 
        (REAL)C3_CURVE_J1(curve) ) ) ;
    if ( !c3c_eval_pt_tan ( curve, &parm, pt, tan_vec ) )
        RETURN ( FALSE ) ;
    if ( dir == -1 ) 
        C3V_NEGATE ( tan_vec, tan_vec ) ;
    RETURN ( TRUE ) ;
}

