//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3QDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3QDEFS
#define     C3QDEFS
#include <c3defs.h>
#include <c2defs.h>
#include <dmdefs.h>

extern  BOOLEAN     c3q_eval __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 , 
            INT DUMMY2 , PT3* DUMMY3 )) ;
extern  BOOLEAN     c3q_pt_tan __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  BOOLEAN     c3q_box __(( C3_PC_BUFFER DUMMY0 , C3_BOX DUMMY1 )) ;
extern  BOOLEAN     c3q_box_append __(( C3_PC_BUFFER DUMMY0 , 
            C3_BOX DUMMY1 )) ;
extern  REAL        c3q_length __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 )) ;
extern  BOOLEAN     c3q_select __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 ,  INT DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 ,
            REAL DUMMY6 , REAL* DUMMY7 , REAL* DUMMY8 )) ;
extern  BOOLEAN     c3q_project __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , PT3 DUMMY4 , REAL* DUMMY5 , 
            PT3 DUMMY6 )) ;
extern  BOOLEAN     c3q_approx __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , BOOLEAN DUMMY5 ,
            PT3* DUMMY6 , PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 , 
            REAL* DUMMY10 )) ;
extern  BOOLEAN     c3q_approx_zoomed __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 ,
            REAL DUMMY6 , PT3* DUMMY7 , PARM DUMMY8 , INT DUMMY9 ,
            INT* DUMMY10 , REAL* DUMMY11 )) ;
extern  C3_PC_BUFFER   c3q_create __(( C3_PC_BUFFER DUMMY0 , INT DUMMY1 )) ;
extern  void       c3q_initialize __(( C3_PC_BUFFER DUMMY0 , INT DUMMY1 , 
            INT DUMMY2 )) ;
extern  C3_PC_BUFFER   c3q_trim __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , C3_PC_BUFFER DUMMY4 )) ;
extern  void        c3q_free __(( C3_PC_BUFFER DUMMY0 )) ;
extern  void        c3q_clear __(( C3_PC_BUFFER DUMMY0 )) ;
extern  C3_PC_BUFFER   c3q_create_copy __(( C3_PC_BUFFER DUMMY0 )) ;
extern  void           c3q_copy __(( C3_PC_BUFFER DUMMY0 , 
            C3_PC_BUFFER DUMMY1 )) ;
extern  C3_PC_BUFFER   c3q_check_size __(( C3_PC_BUFFER DUMMY0 , INT DUMMY1 )) ;
extern  C3_PC_BUFFER   c3q_add_arc_2pts __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  C3_PC_BUFFER   c3q_add_arc_ctr_pt __(( C3_PC_BUFFER DUMMY0 , 
            PT3 DUMMY1 , PT3 DUMMY2 )) ;
extern  C3_PC_BUFFER   c3q_add_arc_tan __(( C3_PC_BUFFER DUMMY0 , 
            PT3 DUMMY1 )) ;
extern  C3_PC_BUFFER   c3q_add_line __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_PC_BUFFER   c3q_add_line_tan __(( C3_PC_BUFFER DUMMY0 , 
            PT3 DUMMY1 )) ;
extern  C3_PC_BUFFER    c3q_through __(( PT3* DUMMY0 , INT DUMMY1 , 
            C3_PC_BUFFER DUMMY2 )) ;
extern  C3_PC_BUFFER   c3q_remove_last __(( C3_PC_BUFFER DUMMY0 )) ;
extern  C3_PC_BUFFER   c3q_close __(( C3_PC_BUFFER DUMMY0 )) ;
extern  BOOLEAN     c3q_curvature __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  INT         c3q_n __(( C3_PC_BUFFER DUMMY0  )) ; 
extern  INT         c3q_size __(( C3_PC_BUFFER DUMMY0  )) ; 
extern  C3_ASEG     c3q_segment __(( C3_PC_BUFFER DUMMY0 , INT* DUMMY1 )) ; 
extern  C3_ASEG     c3q_first_segm __(( C3_PC_BUFFER DUMMY0  )) ; 
extern  C3_ASEG     c3q_last_segm __(( C3_PC_BUFFER DUMMY0  )) ; 
extern  BOOLEAN     c3q_ept0 __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3q_ept1 __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3q_init __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3q_rotate_cs __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN     c3q_transform __(( C3_PC_BUFFER DUMMY0 , 
            C3_TRANSFORM DUMMY1 , BOOLEAN DUMMY2 )) ;
extern  BOOLEAN     c3q_closed __(( C3_PC_BUFFER DUMMY0 )) ;
extern  void        c3q_translate __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3q_scale __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  void        c3q_mirror __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN     c3q_on_plane __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_PLANE_STATUS c3q_plane __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  void        c3q_plane_def __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 , PT3 DUMMY3 [2], INT* DUMMY4 )) ;
extern  BOOLEAN     c3q_project_oblique __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  C2_PC_BUFFER   c3q_conv_3d2d __(( C3_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , C2_PC_BUFFER DUMMY4 )) ;
extern  C3_PC_BUFFER   c3q_conv_2d3d __(( C2_PC_BUFFER DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , C3_PC_BUFFER DUMMY4 )) ;
extern  INT         c3q_coord_extrs __(( C3_PC_BUFFER DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , INT DUMMY4 , DML_LIST DUMMY5 )) ;
extern  void        c3q_info __(( C3_PC_BUFFER DUMMY0 , FILE* DUMMY1 )) ;
extern  BOOLEAN     c3q_put __(( FILE* DUMMY0 , C3_PC_BUFFER DUMMY1 , 
            BOOLEAN DUMMY2 )) ;
extern  BOOLEAN     c3q_get __(( FILE* DUMMY0 , C3_PC_BUFFER DUMMY1 , 
            INT DUMMY2 )) ;

#endif /* C3QDEFS*/

