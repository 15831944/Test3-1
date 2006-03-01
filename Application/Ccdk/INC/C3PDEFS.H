//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************** C3PDEFS.H ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef     C3PDEFS
#define     C3PDEFS
#include <c3defs.h>
#include <c2defs.h>
#include <dmdefs.h>

extern  BOOLEAN     c3p_eval __(( C3_PCURVE DUMMY0 , REAL DUMMY1 , INT DUMMY2 ,
            PT3* DUMMY3 )) ;
extern  BOOLEAN     c3p_pt_tan __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 )) ;
extern  INT         c3p_n __(( C3_PCURVE DUMMY0 )) ; 
extern  C3_ASEG     c3p_segment __(( C3_PCURVE DUMMY0, INT DUMMY1 )) ; 
extern  BOOLEAN     c3p_segment_epts_d __(( C3_PCURVE DUMMY0, INT DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , PT3 DUMMY4 )) ; 
extern  C3_ASEG     c3p_first_segm __(( C3_PCURVE DUMMY0 )) ; 
extern  C3_ASEG     c3p_last_segm __(( C3_PCURVE DUMMY0 )) ; 
extern  C3_PC_BUFFER c3p_first_buffer __(( C3_PCURVE DUMMY0 )) ; 
extern  C3_PC_BUFFER c3p_last_buffer __(( C3_PCURVE DUMMY0 )) ; 
extern  void        c3p_box __(( C3_PCURVE DUMMY0 , C3_BOX DUMMY1 )) ;
extern  void        c3p_box_append __(( C3_PCURVE DUMMY0 , C3_BOX DUMMY1 )) ;
extern  BOOLEAN     c3p_ept0 __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  BOOLEAN     c3p_ept1 __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3p_init __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  REAL        c3p_length __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN     c3p_select __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 ,  PT3 DUMMY3 , PT3 DUMMY4 , REAL DUMMY5 ,
            REAL* DUMMY6 , REAL* DUMMY7 )) ;
extern  BOOLEAN     c3p_project __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 , REAL* DUMMY4 , PT3 DUMMY5 )) ;
extern  BOOLEAN     c3p_approx __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , BOOLEAN DUMMY4 ,  PT3* DUMMY5 ,
            PARM DUMMY6 , INT DUMMY7 , INT* DUMMY8 , REAL* DUMMY9 )) ;
extern  BOOLEAN     c3p_approx_zoomed __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , REAL DUMMY3 , PT3 DUMMY4 ,  REAL DUMMY5 ,
            PT3* DUMMY6 , PARM DUMMY7 , INT DUMMY8 , INT* DUMMY9 ,
            REAL* DUMMY10 )) ;
extern  C3_PCURVE   c3p_create __(( INT DUMMY0 , INT DUMMY1 )) ;
extern  C3_PCURVE   c3p_trim __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 )) ;
extern  void        c3p_free __(( C3_PCURVE DUMMY0 )) ;
extern  C3_PCURVE   c3p_create_copy __(( C3_PCURVE DUMMY0 )) ;
extern  C3_PCURVE   c3p_add_arc_2pts __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  C3_PCURVE   c3p_add_arc_ctr_pt __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 )) ;
extern  C3_PCURVE   c3p_add_arc_tan __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_PCURVE   c3p_add_line __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_PCURVE   c3p_add_line_tan __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  C3_PCURVE   c3p_add __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 , 
            PT3 DUMMY3 , REAL DUMMY4 )) ;
extern  C3_PCURVE   c3p_through __(( PT3* DUMMY0 , INT DUMMY1 )) ;
extern  C3_PCURVE   c3p_remove_last __(( C3_PCURVE DUMMY0 )) ;
extern  C3_PCURVE   c3p_close __(( C3_PCURVE DUMMY0 )) ;
extern  REAL        c3p_curvature __(( C3_PCURVE DUMMY0 , REAL DUMMY1 )) ;
extern  void        c3p_rotate_cs __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN     c3p_transform __(( C3_PCURVE DUMMY0 , 
            C3_TRANSFORM DUMMY1 )) ;
extern  BOOLEAN     c3p_closed __(( C3_PCURVE DUMMY0 )) ;
extern  void        c3p_translate __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 )) ;
extern  void        c3p_scale __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  void        c3p_mirror __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  BOOLEAN     c3p_on_plane __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 )) ;
extern  C3_PLANE_STATUS c3p_plane __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 )) ;
extern  void        c3p_plane_def __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            REAL* DUMMY2 , PT3 DUMMY3 [2], INT* DUMMY4 )) ;
extern  BOOLEAN     c3p_project_oblique __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            REAL DUMMY2 , PT3 DUMMY3 )) ;
extern  C2_PCURVE   c3p_conv_3d2d __(( C3_PCURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , C2_PCURVE DUMMY4 )) ;
extern  C3_PCURVE   c3p_conv_2d3d __(( C2_PCURVE DUMMY0 , PT3 DUMMY1 ,
            PT3 DUMMY2 , PT3 DUMMY3 , C3_PCURVE DUMMY4 )) ;
extern  INT         c3p_coord_extrs __(( C3_PCURVE DUMMY0 , REAL DUMMY1 ,
            REAL DUMMY2 , INT DUMMY3 , DML_LIST DUMMY4 )) ;
extern  void        c3p_info __(( C3_PCURVE DUMMY0 , FILE* DUMMY1 )) ;
extern  C3_PCURVE   c3p_get __(( FILE* DUMMY0 , PARM DUMMY1 , PARM DUMMY2 )) ;
extern  BOOLEAN     c3p_put __(( FILE* DUMMY0 , C3_PCURVE DUMMY1 , 
            PARM DUMMY2 , PARM DUMMY3 )) ;
extern  BOOLEAN     c3p_get_data __(( C3_PCURVE DUMMY0 , INT DUMMY1 , 
            PT3 DUMMY2 , PT3 DUMMY3 , REAL* DUMMY4 , PT3 DUMMY5 , 
            REAL* DUMMY6 )) ;
#endif /* C3PDEFS*/

