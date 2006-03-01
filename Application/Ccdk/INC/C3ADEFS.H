//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3ADEFS.H *****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef C3ADEFS
#define C3ADEFS
#include <c3defs.h>

extern  C3_BOX  c3a_box __(( PT3 DUMMY0 , PT3 DUMMY1 )) ;
extern  void    c3a_free_box __(( C3_BOX DUMMY0 )) ;
extern  C3_BOX  c3a_box_poly __(( PT3* DUMMY0 , INT DUMMY1 , C3_BOX DUMMY2 )) ;
extern  REAL    c3a_box_size __(( C3_BOX DUMMY0 )) ;
extern  BOOLEAN c3a_pt_inside __(( C3_BOX DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  C3_BOX  c3a_box_init_pt __(( C3_BOX DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_BOX  c3a_box_append_pt __(( C3_BOX DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN c3a_box_overlap __(( C3_BOX DUMMY0 , C3_BOX DUMMY1 )) ;
extern  C3_BOX  c3a_box_parlm __(( C3_BOX DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  C3_BOX  c3a_box_plus_minus __(( C3_BOX DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  C3_BOX  c3a_box_ctr __(( C3_BOX DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 )) ;
extern  C3_BOX  c3a_box_append __(( C3_BOX DUMMY0 , C3_BOX DUMMY1 )) ;
extern  C3_BOX  c3a_box_copy __(( C3_BOX DUMMY0 , C3_BOX DUMMY1 )) ;
extern  C3_BOX  c3a_box_inflate __(( C3_BOX DUMMY0 , REAL DUMMY1 )) ;
extern  C3_BOX  c3a_box_circle __(( C3_BOX DUMMY0 , PT3 DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 )) ;
extern  C3_BOX  c3a_box_union __(( C3_BOX DUMMY0 , C3_BOX DUMMY1 ,
            C3_BOX DUMMY2 )) ;
extern  BOOLEAN c3a_box_w_overlap __(( C3_BOX DUMMY0 , C3_BOX DUMMY1 ,
            REAL DUMMY2 )) ;
#endif  /* C3ADEFS */

