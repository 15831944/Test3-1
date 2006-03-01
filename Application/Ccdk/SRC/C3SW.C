//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3SW.C *********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef __NDP__
#ifdef  SPLINE
#include <alwdefs.h>
#include <c3ndefs.h>
#include <c3sdefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_get ( file, a, n, d, knot, w_ptr, normal ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
HPT3* a ;
INT n, d ;
REAL *knot, *w_ptr ;
PT3 normal ;
{
    RETURN ( alw_get_array ( file, (REAL*)a, 4*n ) && 
        alw_get_array ( file, knot, n+d ) &&
        alw_get_real ( file, w_ptr ) && alw_get_array ( file, normal, 3 ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c3s_put ( file, a, n, d, knot, w, normal, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
HPT3 *a ;
INT n, d ;
REAL *knot, w ;
PT3 normal ;
PARM parm0, parm1 ;
{
    RETURN ( alw_put_int ( file, n, NULL ) && alw_put_int ( file, d, NULL ) &&
             alw_put_array ( file, (REAL*)a, 4*n, NULL  ) &&
             alw_put_array ( file, knot, n+d, NULL  ) &&
             alw_put_real ( file, w, NULL ) &&
             alw_put_array ( file, normal, 3, NULL  ) &&
             alw_put_parm ( file, parm0, NULL ) && 
             alw_put_parm ( file, parm1, NULL ) ) ;
}
#endif  /*SPLINE*/
#endif  /* NDP */

