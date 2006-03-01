//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** DX0DEFS.H ***********************************/
/*******************  Prototypes for dxf_get routines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX0DEFS
#define     DX0DEFS

#include <dxfdefs.h>

extern  BOOLEAN     dx0_file_box __(( DXF_FILE DUMMY0 , DXF_FILTER DUMMY1 ,
            REAL DUMMY2 [3], REAL DUMMY3 [3])) ;
extern  void        dx0_free_entity __(( DXF_ENTITY DUMMY0 )) ;
extern  DXF_BLOCKPOS    dx0_alloc_blockpos __(( void )) ;
extern  void            dx0_free_blockpos __(( DXF_BLOCKPOS DUMMY0 )) ;
extern  INT         dx0_sizeof __(( INT DUMMY0 )) ;
extern  void        dx0_rewind __(( DXF_FILE DUMMY0 )) ;
extern  REAL*       dx0_copy_point __(( REAL DUMMY0 [3], REAL DUMMY1 [3])) ;
extern  BOOLEAN     dx0_get_string __(( DXF_FILE DUMMY0 , STRING DUMMY1 )) ;
extern  BOOLEAN     dx0_get_dxstring __(( DXF_FILE DUMMY0 , DX_STRING* DUMMY1 )) ;
extern  BOOLEAN     dx0_get_alloc_string __(( DXF_FILE DUMMY0 , 
            STRING* DUMMY1 )) ;
extern  INT         dx0_get_code __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_get_int __(( DXF_FILE DUMMY0 , INT* DUMMY1 )) ;
extern  BOOLEAN     dx0_get_real __(( DXF_FILE DUMMY0 , REAL* DUMMY1 )) ;
extern  BOOLEAN     dx0_get_void __(( DXF_FILE DUMMY0 , INT DUMMY1 )) ;
extern  BOOLEAN     dx0_get_any __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            ANY DUMMY2 )) ;
extern  INT         dx0_get_object __(( DXF_FILE DUMMY0 , ANY DUMMY1 ,
            INT* DUMMY2 , BOOLEAN* DUMMY3 , BOOLEAN* DUMMY4 )) ;
extern  BOOLEAN     dx0_find __(( DXF_FILE DUMMY0 , INT DUMMY1 , 
            STRING DUMMY2 , fpos_t* DUMMY3 )) ;
extern  BOOLEAN     dx0_find_after __(( DXF_FILE DUMMY0 , INT DUMMY1 , 
            STRING DUMMY2 , INT DUMMY3 , STRING DUMMY4 , fpos_t* DUMMY5 )) ;
extern  INT         dx0_count __(( DXF_FILE DUMMY0 , INT DUMMY1 , 
            STRING DUMMY2 )) ;
extern  BOOLEAN     dx0_setpos_header __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_setpos_tables __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_setpos_blocks __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_setpos_block __(( DXF_FILE DUMMY0 , STRING DUMMY1 )) ;
extern  BOOLEAN     dx0_block_list __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_setpos_entities __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_setpos_next_entity __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_put_seqend __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN     dx0_put_code __(( DXF_FILE DUMMY0 , INT DUMMY1 )) ;
extern  BOOLEAN     dx0_put_int __(( DXF_FILE DUMMY0 , INT DUMMY1 )) ;
extern  BOOLEAN     dx0_put_real __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 )) ;

extern  BOOLEAN     dx0_put_string __(( DXF_FILE DUMMY0 ,   STRING DUMMY1 )) ;
extern  BOOLEAN     dx0_put_dxstring __(( DXF_FILE DUMMY0 , DX_STRING *DUMMY1 )) ;


extern  BOOLEAN     dx0_put_int_group __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 )) ;
extern  BOOLEAN     dx0_put_real_group __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            REAL DUMMY2 )) ;


extern  BOOLEAN     dx0_put_string_group __(( DXF_FILE DUMMY0 , INT DUMMY1 , STRING DUMMY2 )) ;
extern  BOOLEAN     dx0_put_dxstring_group __(( DXF_FILE DUMMY0 , INT DUMMY1 , DX_STRING *DUMMY2 )) ;


extern  BOOLEAN     dx0_put_any_group __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            ANY DUMMY2 )) ;
extern  BOOLEAN     dx0_put_int_mode __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , INT DUMMY3 )) ;
extern  BOOLEAN     dx0_put_real_mode __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL DUMMY3 )) ;


extern  BOOLEAN     dx0_put_string_mode __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , STRING DUMMY3 )) ;
extern  BOOLEAN     dx0_put_dxstring_mode __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , DX_STRING *DUMMY3 )) ;


extern  BOOLEAN     dx0_put_any_mode __(( DXF_FILE DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , ANY DUMMY3 )) ;
extern  BOOLEAN     dx0_put_object __(( DXF_FILE DUMMY0 , ANY DUMMY1 ,
            INT* DUMMY2 , BOOLEAN DUMMY3 , BOOLEAN DUMMY4 )) ;
extern  void        dx0_clear_any __(( INT DUMMY0 , STRING DUMMY1 )) ;
extern  void        dx0_set1_any __(( INT DUMMY0 , ANY DUMMY1 )) ;
extern  void        dx0_setneg1_any __(( INT DUMMY0 , ANY DUMMY1 )) ;
extern  void        dx0_init_template __(( ANY DUMMY0 , INT* DUMMY1 )) ;
extern  BOOLEAN     dx0_set_template __(( ANY DUMMY0 , INT* DUMMY1 ,
            INT DUMMY2 , ANY DUMMY3 )) ;
extern  ANY         dx0_get_template __(( ANY DUMMY0 , INT* DUMMY1 ,
            INT DUMMY2 )) ;
extern  DXF_ENTITY_ID   dx0_entity_id __(( STRING DUMMY0 )) ;
extern  DXF_TABLE_ENTRY_ID dx0_table_entry_id __(( STRING DUMMY0 )) ;
extern  STRING      dx0_table_entry_name __(( DXF_TABLE_ENTRY_ID DUMMY0 )) ;
extern  STRING      dx0_entity_name __(( DXF_ENTITY_ID DUMMY0 )) ;
extern  BOOLEAN     dx0_entity_filter __(( DXF_ENTITY DUMMY0 , 
            DXF_FILTER DUMMY1 )) ;
extern  BOOLEAN     dx0_entity_id_filter __(( DXF_ENTITY_ID DUMMY0 ,
            DXF_FILTER DUMMY1 )) ;
extern  BOOLEAN     dx0_layer_filter __(( STRING DUMMY0 , DXF_FILTER DUMMY1 )) ;
extern  BOOLEAN     dx0_color_filter __(( INT DUMMY0 , DXF_FILTER DUMMY1 )) ;
extern  STRING      dx0_strcpy __(( STRING    DUMMY0 , STRING DUMMY1 )) ;

extern  STRING      dx0_strass __(( DX_STRING* DUMMY0 , STRING DUMMY1 )) ;
extern  void        dx0_strfree __(( DX_STRING* DUMMY0  )) ;

extern  void        dx0_unrec __(( DXF_FILE DUMMY0 , fpos_t DUMMY1 )) ;
#include <dx0hdr.h>
#ifdef __ACAD11__
#include <dx0defs1.h>
#else
#include <dx0defs0.h>
#endif     /*__ACAD11__*/
#endif     /*DX0DEFS*/

