//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/******************************* C3D.C **********************************/
/********* Three-dimensional curve construction routines ****************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c3cdefs.h>
#include <c3ddefs.h>
#include <c3vdefs.h>

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_translate ( curve0, shift ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
PT3 shift ;
{
    C3_CURVE curve ;

    curve = c3d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c3c_translate ( curve, shift ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_scale ( curve0, origin, factor ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
REAL factor ;
PT3 origin ;
{
    C3_CURVE curve ;
    curve = c3d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c3c_scale ( curve, origin, factor ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_mirror ( curve0, n, d ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
PT3 n ;
REAL d ;
{
    C3_CURVE curve ;
    curve = c3d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c3c_mirror ( curve, n, d ) ;
    RETURN ( curve ) ;
}

/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_rotate ( curve0, origin, axis, angle ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
PT3 origin, axis ;
REAL angle ;
{
    C3_CURVE curve ;
    curve = c3d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c3c_rotate ( curve, origin, axis, angle ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_rotate_cs ( curve0, origin, axis, c, s ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
PT3 origin, axis ;
REAL c, s ;
{
    C3_CURVE curve ;
    curve = c3d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    c3c_rotate_cs ( curve, origin, axis, c, s ) ;
    RETURN ( curve ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_project_oblique ( curve0, normal, dist, direction ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
PT3 normal ;
REAL dist ;
PT3 direction ;
{
    C3_CURVE curve ;
    curve = c3d_copy ( curve0 ) ;
    if ( curve == NULL ) 
        RETURN ( NULL ) ;
    if ( c3c_project_oblique ( curve, normal, dist, direction ) != NULL )
        RETURN ( curve ) ;
    else {
        c3d_free_curve ( curve ) ;
        RETURN ( curve ) ;
    }
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_project_normal ( curve0, normal, dist ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
PT3 normal ;
REAL dist ;
{
    RETURN ( c3d_project_oblique ( curve0, normal, dist, normal ) ) ;
}


/*----------------------------------------------------------------------*/
BBS_PUBLIC C3_CURVE c3d_transform ( curve0, t ) 
/*----------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve0 ;
C3_TRANSFORM t ;
{
    C3_CURVE curve ;
    curve = c3d_copy ( curve0 ) ;
    if ( c3c_transform ( curve, t ) == NULL ) {
        c3d_free_curve ( curve ) ;
        RETURN ( NULL ) ;
    }
    else 
        RETURN ( curve ) ;
}

