//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3CPRIV.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     C3CPRIV
#define     C3CPRIV
#include    <c3defs.h>
extern  BOOLEAN     c3c_midpt_dir __(( C3_CURVE DUMMY0 , INT DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  void        c3c_box __(( C3_CURVE )) ;
extern  BOOLEAN     c3c_approx_zoomed __(( C3_CURVE DUMMY0 , PARM DUMMY1 ,
            PARM DUMMY2 , REAL DUMMY3 , PT3 DUMMY4 ,  REAL DUMMY5 ,
            PT3* DUMMY6 , PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 )) ;
#endif /* C3CPRIV*/

