//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3NP.C *********************************/
/********************************** Nurbs **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  SPLINE
#include <c3gdefs.h>
#include <c3ndefs.h>
#include <c3rdefs.h>
#include <c3sdefs.h>
#include <c3vmcrs.h>
#include <c3gmcrs.h>
#include <c3lmcrs.h>
#include <c3nmcrs.h>
#include <c3pmcrs.h>
#include <c3qmcrs.h>
#include <dmldefs.h>
STATIC INT c3n_pc_buf_dn __(( C3_PC_BUFFER )) ;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_pcurve ( pcurve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
{
    INT n ;
    C3_NURB nurb ;
    DML_ITEM item ;

    n = c3n_pc_buf_dn ( C3_PCURVE_BUFFER(pcurve) ) ;
    C3_WALK_PCURVE ( pcurve, item ) 
        n += c3n_pc_buf_dn ( DML_RECORD(item) ) ;

    n = 2 * n - 1 ;
    nurb = c3n_create_nurb ( n, 3 ) ;
    c3s_pcurve ( pcurve, C3_NURB_CTPT_PTR(nurb), C3_NURB_KNOT_PTR(nurb) ) ;
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
STATIC INT c3n_pc_buf_dn ( buffer ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PC_BUFFER buffer ;
{
    INT i, n = C3_PC_BUFFER_N(buffer) ;

    for ( i=0 ; i<C3_PC_BUFFER_N(buffer) ; i++ ) {
        if ( fabs(c3r_d_parm(C3_PC_BUFFER_ARC(buffer,i))) >= 1.0 - BBS_ZERO ) 
            n++ ;
    }
    RETURN ( n ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_line ( line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_LINE line ;
{
    C3_NURB nurb = c3n_create_nurb ( 2, 2 ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    C3V_COPY ( C3_LINE_PT0(line), C3_NURB_CTPT(nurb)[0] ) ;
    C3_NURB_CTPT(nurb)[0][2] = 1.0 ;
    C3V_COPY ( C3_LINE_PT1(line), C3_NURB_CTPT(nurb)[1] ) ;
    C3_NURB_CTPT(nurb)[1][2] = 1.0 ;
    C3_NURB_KNOT(nurb)[0] = C3_NURB_KNOT(nurb)[1] = 0.0 ; 
    C3_NURB_KNOT(nurb)[2] = C3_NURB_KNOT(nurb)[3] = 1.0 ; 
    RETURN ( nurb ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_NURB c3n_arc ( arc, full_circle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ARC arc ;
BOOLEAN full_circle ;
{
    INT n ;
    C3_NURB nurb ;

    n = full_circle ? 9 : 
        fabs ( c3r_d_parm ( C3_ARC_SEG(arc) ) ) < 1.0-BBS_ZERO ? 3 : 5 ;

    nurb = c3n_create_nurb ( n, 3 ) ;
    if ( nurb == NULL ) 
        RETURN ( NULL ) ;
    (void)c3g_bezs ( arc, full_circle, C3_NURB_CTPT(nurb) ) ;
    als_ellipse_knots ( n, C3_NURB_KNOT(nurb) ) ;
    RETURN ( nurb ) ;
}

#endif  /*SPLINE*/


