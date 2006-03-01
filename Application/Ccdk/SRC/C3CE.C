//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3CE.C *********************************/
/**************** Three-dimensional curves ******************************/
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
#include <c3vmcrs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_eval ( curve, parm, p, x ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm ;
INT p ;
PT3 *x ;
{
    if ( C3_CURVE_IS_LINE(curve) ) {
        c3l_eval ( C3_CURVE_LINE(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C3_CURVE_IS_ARC(curve) ) {
        c3g_eval ( C3_CURVE_ARC(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else if ( C3_CURVE_IS_PCURVE(curve) ) {
        c3p_eval ( C3_CURVE_PCURVE(curve), PARM_T(parm), p, x ) ;
        RETURN ( TRUE ) ;
    }
    else
#ifdef  SPLINE
        RETURN ( c3n_eval ( C3_CURVE_NURB(curve), parm, p, x ) ) ;
#else
        RETURN ( FALSE ) ;
#endif  /*SPLINE*/
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_eval_pt ( curve, parm, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PARM parm ;
PT3 pt ;
{
    RETURN ( c3c_eval ( curve, parm, 0, (PT3*)pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_eval_tan ( curve, parm, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PARM parm ;
PT3 tan_vec ;
{
    PT3 x[2] ;

    if ( c3c_eval ( curve, parm, 1, x ) ) {
        C3V_COPY ( x[1], tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_eval_pt_tan ( curve, parm, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PARM parm ;
PT3 pt ;
PT3 tan_vec ;
{
    PT3 x[2] ;
    
    if ( c3c_eval ( curve, parm, 1, x ) ) {
        if ( pt != NULL ) 
            C3V_COPY ( x[0], pt ) ;
        if ( tan_vec != NULL ) 
            C3V_COPY ( x[1], tan_vec ) ;
        RETURN ( TRUE ) ;
    }
    else
        RETURN ( FALSE ) ;
}   


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_ept0 ( curve, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PT3 pt ;
{
    RETURN ( c3c_eval_pt ( curve, C3_CURVE_PARM0(curve), pt ) ) ;
}

/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_etan0 ( curve, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PT3 tan_vec ;
{
    RETURN ( c3c_eval_tan ( curve, C3_CURVE_PARM0(curve), tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_ept_tan0 ( curve, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PT3 pt ;
PT3 tan_vec ;
{
    RETURN ( c3c_eval_pt_tan ( curve, C3_CURVE_PARM0(curve), pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_ept1 ( curve, pt ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PT3 pt ;
{
    RETURN ( c3c_eval_pt ( curve, C3_CURVE_PARM1(curve), pt ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_etan1 ( curve, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PT3 tan_vec ;
{
    RETURN ( c3c_eval_tan ( curve, C3_CURVE_PARM1(curve), tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3c_ept_tan1 ( curve, pt, tan_vec ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

C3_CURVE curve ;
PT3 pt ;
PT3 tan_vec ;
{
    RETURN ( c3c_eval_pt_tan ( curve, C3_CURVE_PARM1(curve), pt, tan_vec ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN c3c_eval_equi_parm ( curve, parm0, parm1, n, x, 
            tan0, tan1 ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm0, parm1 ;
INT n ;
PT3 *x, tan0, tan1 ;
{
    REAL step ;
    PARM_S parm ;
    BOOLEAN status ;
    INT i ;

    if ( n <= 1 ) 
        RETURN ( FALSE ) ;

    status = c3c_eval_pt_tan ( curve, 
        parm0 == NULL ? C3_CURVE_PARM0(curve) : parm0, x[0], tan0 ) ;

    step = ( ( parm1 == NULL ? C3_CURVE_T1(curve) : PARM_T(parm1) ) -
             ( parm0 == NULL ? C3_CURVE_T0(curve) : PARM_T(parm0) ) ) / 
             (REAL)(n-1) ; 
    PARM_COPY ( parm0 == NULL ? C3_CURVE_PARM0(curve) : parm0, &parm ) ;
    for ( i=1 ; i<n-1 ; i++ ) {
        PARM_T(&parm) += step ;
        status = status && c3c_eval_pt ( curve, &parm, x[i] ) ;
    }
    status = status && c3c_eval_pt_tan ( curve, 
        parm1 == NULL ? C3_CURVE_PARM1(curve) : parm1, x[n-1], tan1 ) ;
    RETURN ( status ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3c_box ( curve ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    if ( curve == NULL ) 
        RETURN ;
    else if ( C3_CURVE_IS_LINE(curve) ) 
        c3l_box ( C3_CURVE_LINE(curve), C3_CURVE_T0(curve), 
            C3_CURVE_T1(curve), C3_CURVE_BOX(curve) ) ;
    else if ( C3_CURVE_IS_ARC(curve) ) 
        c3g_box ( C3_CURVE_ARC(curve), C3_CURVE_IS_EXT_ARC(curve), 
            C3_CURVE_BOX(curve) ) ;
    else if ( C3_CURVE_IS_PCURVE(curve) ) 
        c3p_box ( C3_CURVE_PCURVE(curve), C3_CURVE_BOX(curve) ) ;
#ifdef  SPLINE
    else 
        c3n_box ( C3_CURVE_NURB(curve), C3_CURVE_BOX(curve) ) ;
#endif  /* SPLINE */
}

