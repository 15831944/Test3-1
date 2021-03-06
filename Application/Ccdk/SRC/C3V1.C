//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3V1.C *********************************/
/*************************** 3d vector geometry ****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <c3vdefs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3v_is_small ( a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
{
    RETURN ( IS_SMALL(a[0]) && IS_SMALL(a[1]) && IS_SMALL(a[2]) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN     c3v_ident_pts(a,b) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    RETURN ( IS_SMALL(a[0]-b[0]) && IS_SMALL(a[1]-b[1]) && 
        IS_SMALL(a[2]-b[2]) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_dot ( a, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    RETURN ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] ) ;
}




/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL    c3v_triple_prod ( a, b, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
{
    RETURN ( c[0] * ( a[1] * b[2] - a[2] * b[1] ) + 
             c[1] * ( a[2] * b[0] - a[0] * b[2] ) + 
             c[2] * ( a[0] * b[1] - a[1] * b[0] ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_norm ( a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
{
    RETURN ( sqrt ( a[0]*a[0] + a[1]*a[1] + a[2]*a[2] ) ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_norm_squared ( a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
{
    RETURN ( a[0]*a[0] + a[1]*a[1] + a[2]*a[2] ) ;
}    


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_norml1 ( a ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a ;
{
    RETURN ( fabs(a[0]) + fabs(a[1]) + fabs(a[2]) ) ;
}    



/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_dist ( a, b )   
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    PT3 c ;
    C3V_SUB ( a, b, c ) ;
    RETURN ( C3V_NORM ( c ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_dist_squared ( a, b )   
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    PT3 c ;
    C3V_SUB ( b, a, c ) ;
    RETURN ( c[0]*c[0] + c[1]*c[1] + c[2]*c[2] ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL        c3v_distl1 ( a, b )   
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    RETURN ( fabs(a[0]-b[0]) + fabs(a[1]-b[1]) + fabs(a[2]-b[2]) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_add ( a, b, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
{
    c[0] = a[0] + b[0] ;
    c[1] = a[1] + b[1] ;
    c[2] = a[2] + b[2] ;
    RETURN ( c ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_addt ( a, b, t, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
REAL t ;
{
    c[0] = a[0] + t * b[0] ;
    c[1] = a[1] + t * b[1] ;
    c[2] = a[2] + t * b[2] ;
    RETURN ( c ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_addw ( a, p, b, q, c ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
REAL p, q ;
{
    c[0] = p * a[0] + q * b[0] ;
    c[1] = p * a[1] + q * b[1] ;
    c[2] = p * a[2] + q * b[2] ;
    RETURN ( c ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_addc ( c, a, p, b, q, x ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c, x ;
REAL p, q ;
{
    x[0] = c[0] + p * a[0] + q * b[0] ;
    x[1] = c[1] + p * a[1] + q * b[1] ;
    x[2] = c[2] + p * a[2] + q * b[2] ;
    RETURN ( x ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_addu ( a, b, u, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
REAL u ;
{
    c[0] = (1.0-u) * a[0] + u * b[0] ;
    c[1] = (1.0-u) * a[1] + u * b[1] ;
    c[2] = (1.0-u) * a[2] + u * b[2] ;
    RETURN ( c ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_sub ( a, b, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
{
    c[0] = a[0] - b[0] ;
    c[1] = a[1] - b[1] ;
    c[2] = a[2] - b[2] ;
    RETURN ( c ) ;
}
 

/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_negate ( a, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
{
    b[0] = - a[0] ;
    b[1] = - a[1] ;
    b[2] = - a[2] ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_mid_pt ( a, b, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
{
    c[0] = 0.5 * ( a[0] + b[0] ) ;
    c[1] = 0.5 * ( a[1] + b[1] ) ;
    c[2] = 0.5 * ( a[2] + b[2] ) ;
    RETURN ( c ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_scale ( a, t, b )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b ;
REAL t ;
{
    b[0] = t * a[0] ;
    b[1] = t * a[1] ;
    b[2] = t * a[2] ;
    RETURN ( b ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC REAL*       c3v_cross ( a, b, c )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 a, b, c ;
{
    c[0] = a[1] * b[2] - a[2] * b[1] ;
    c[1] = a[2] * b[0] - a[0] * b[2] ;
    c[2] = a[0] * b[1] - a[1] * b[0] ;
    RETURN ( c ) ;
}

