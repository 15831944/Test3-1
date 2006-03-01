//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3PT.C *********************************/
/***************** Routines for processing polycurve geometry **************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c3pdefs.h>
#include <c3qdefs.h>
#include <c3tdefs.h>
#include <dmldefs.h>
#include <c3pmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3p_translate ( pcurve, shift ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 shift ;
{
    DML_ITEM item ;

    c3q_translate ( C3_PCURVE_BUFFER(pcurve), shift ) ;
    C3_WALK_PCURVE ( pcurve, item )  
        c3q_translate ( DML_RECORD(item), shift ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3p_scale ( pcurve, origin, factor ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 origin ;
REAL factor ;
{
    DML_ITEM item ;

    c3q_scale ( C3_PCURVE_BUFFER(pcurve), origin, factor ) ;
    C3_WALK_PCURVE ( pcurve, item )  
        c3q_scale ( DML_RECORD(item), origin, factor ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void c3p_mirror ( pcurve, n, d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 n ;
REAL d ;
{
    DML_ITEM item ;

    c3q_mirror ( C3_PCURVE_BUFFER(pcurve), n, d ) ;
    C3_WALK_PCURVE ( pcurve, item )  
        c3q_mirror ( DML_RECORD(item), n, d ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE void c3p_rotate_cs ( pcurve, origin, axis, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 origin, axis ;
REAL c, s ;
{
    DML_ITEM item ;

    c3q_rotate_cs ( C3_PCURVE_BUFFER(pcurve), origin, axis, c, s ) ;
    C3_WALK_PCURVE ( pcurve, item ) 
        c3q_rotate_cs ( DML_RECORD(item), origin, axis, c, s ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_project_oblique ( pcurve, normal, dist, direction ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
PT3 normal, direction ;
REAL dist ;
{
    DML_ITEM item ;

    if ( !c3q_project_oblique ( C3_PCURVE_BUFFER(pcurve), 
        normal, dist, direction ) )
        RETURN ( FALSE ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        if ( !c3q_project_oblique ( DML_RECORD(item), 
            normal, dist, direction ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*----------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3p_transform ( pcurve, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_PCURVE pcurve ;
C3_TRANSFORM t ;
{
    DML_ITEM item ;
    BOOLEAN t_orth = c3t_orthogonal ( t ) ;

    if ( !c3q_transform ( C3_PCURVE_BUFFER(pcurve), t, t_orth ) )
        RETURN ( FALSE ) ;
    C3_WALK_PCURVE ( pcurve, item ) {
        if ( !c3q_transform ( DML_RECORD(item), t, t_orth ) ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}

