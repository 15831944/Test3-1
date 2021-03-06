//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/************************************ C2AG.C *******************************/
/***************** Arrays of two-dimensional homogeneous points ************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                     All rights reserved                  !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef   SPLINE
#include <c2apriv.h>
#include <c2vdefs.h>
#include <c2vmcrs.h>

STATIC  BOOLEAN c2ag_pt_on_side __(( PT2 DUMMY0 , PT2 DUMMY1 , PT2 DUMMY2 , 
            PT2 DUMMY3 )) ;
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_pt_in_polygon ( c, n, a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 *c ;
INT n ;
PT2 a ;
{
    INT i ;

    for ( i=2 ; i<n ; i++ )
        if ( c2a_pt_in_triangle ( c[0], c[i-1], c[i], a ) ) 
            RETURN ( TRUE ) ;
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2a_pt_in_triangle ( a0, a1, a2, b ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2     a0, a1, a2 ;    /* Vertices of triangle */
PT2     b ;    /* Point */
{
    RETURN ( c2ag_pt_on_side ( a0, a1, a2, b ) && 
             c2ag_pt_on_side ( a2, a0, a1, b ) && 
             c2ag_pt_on_side ( a1, a2, a0, b ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN c2ag_pt_on_side ( a0, a1, b1, b2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2     a0, a1 ;    /* End points of a line */
PT2     b1, b2 ;    /* Points */
{
    PT2 v ;
    INT side1, side2 ;

    C2V_SUB ( a1, a0, v ) ;
    side1 = c2v_side ( b1, a0, v ) ;
    side2 = c2v_side ( b2, a0, v ) ;
    RETURN ( side1==0 || side2==0 || side1==side2 ) ;
}

#ifdef NEW_CODE
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT c2a_pt_on_side ( a, n, b, vec ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2     *a ;    
INT     n ;
PT2     b, vec ;    /* Point and direction */
{
    INT i, side, side0 ;

    side = 2 ;
    for ( i=0 ; i<n ; i++ ) {
        side0 = c2v_side ( a[i], b, vec ) ;
        if ( side==2 ) 
            side = side0 ;
        else if ( side0 != side ) 
            RETURN ( 0 ) ;
    }
    RETURN ( side ) ;
}
#endif /*NEW_CODE*/
#endif   /*SPLINE*/

