//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 -x */
/******************************* DXTP.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dx0defs.h>
#include <dxfilm.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_open_table ( file, table_entry_id, n )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_TABLE_ENTRY_ID table_entry_id ;
INT n ;
{
    if ( !DX_FILE_TABLES_OPEN(file) && !dxf_open_tables ( file ) ) 
        RETURN ( FALSE ) ;
    if ( DX_FILE_TABLES_CLOSED(file) )
        RETURN ( FALSE ) ;
    if ( DX_FILE_LAST_OPEN(file) && !dxf_close_table(file) )
        RETURN ( FALSE ) ;
    DX_FILE_SET_LAST_OPEN(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "TABLE" ) && 
        dx0_put_string_group ( file, 2, 
        dx0_table_entry_name ( table_entry_id ) ) && 
        ( n < 0 || dx0_put_int_group ( file, 70, n ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_table_entry ( file, table_entry ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_TABLE_ENTRY table_entry ;
{ 
    INT *template ;
    if ( !DX_FILE_TABLES_OPEN(file) )
        RETURN ( FALSE ) ;
#ifdef __ACAD11__  
    template = dx0_table_entry_template ( DX_TE_ID(table_entry), 
        dx0_file_release ( file ) ) ;
#else
    template = dx0_table_entry_template ( DX_TE_ID(table_entry) ) ;
#endif
    if ( template == NULL ) 
        RETURN ( FALSE ) ;
    if ( !dx0_put_string_group ( file, 0, 
        dx0_table_entry_name ( DX_TE_ID(table_entry) ) ) )
        RETURN ( FALSE ) ;
    if ( !dx0_put_object ( file, DX_TE_ANY(table_entry), 
        template, TRUE, TRUE ) )
        RETURN ( FALSE ) ;
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_table_entries ( file, table_entries_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST table_entries_list ;
{ 
    DML_ITEM item ;

    DML_WALK_LIST ( table_entries_list, item ) {
        if ( !dxf_put_table_entry ( file, DML_RECORD(item) ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_layer ( file, name, 
            color, frozen, ltypename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name, ltypename ;
INT color ;
BOOLEAN frozen ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
#ifdef __ACAD11__
    table_entry = dxf_create_layer ( file, name, color, frozen, ltypename ) ;
#else
    table_entry = dxf_create_layer ( name, color, frozen, ltypename ) ;
#endif
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_ltype ( file, name, text, algn, 
        tot_len, count, len ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name, text ;
INT algn, count ;
REAL tot_len, *len ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
#ifdef __ACAD11__
    table_entry = dxf_create_ltype ( file, name, text, algn, 
        tot_len, count, len ) ;
#else
    table_entry = dxf_create_ltype ( name, text, algn, 
        tot_len, count, len ) ;
#endif
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_style ( file, name, flags, height, width, 
    angle, text_flags, last_height, font_filename, bigfont_filename )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
INT flags ; /* code 70 */
REAL height, width, angle ;
INT text_flags ; /* code 71 */
REAL last_height ;
STRING font_filename, bigfont_filename ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
#ifdef __ACAD11__
    table_entry = dxf_create_style ( file, name, flags, height, width, 
        angle, text_flags, last_height, font_filename, bigfont_filename ) ;
#else
    table_entry = dxf_create_style ( name, flags, height, width, 
        angle, text_flags, last_height, font_filename, bigfont_filename ) ;
#endif
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_view ( file, name, h, w, ctr, dir, tgt_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
REAL h, w, ctr[3], dir[3], tgt_pt[3] ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
#ifdef __ACAD11__
    table_entry = dxf_create_view ( file, name, h, w, ctr, dir, tgt_pt ) ;
#else
    table_entry = dxf_create_view ( name, h, w, ctr, dir, tgt_pt ) ;
#endif
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_ucs ( file, name, origin, x_axis, y_axis )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
REAL origin[3], x_axis[3], y_axis[3] ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
#ifdef __ACAD11__
    table_entry = dxf_create_ucs ( file, name, origin, x_axis, y_axis ) ;
#else
    table_entry = dxf_create_ucs ( name, origin, x_axis, y_axis ) ;
#endif
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_vport ( file, name, x0, y0, x1, y1, 
        ctr, dir, tgt_pt, height, ar ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
REAL x0, y0, x1, y1, ctr[2], dir[3], tgt_pt[3], height, ar ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
#ifdef __ACAD11__
    table_entry = dxf_create_vport ( file, name, x0, y0, x1, y1, 
        ctr, dir, tgt_pt, height, ar ) ;
#else
    table_entry = dxf_create_vport ( name, x0, y0, x1, y1, 
        ctr, dir, tgt_pt, height, ar ) ;
#endif
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_appid ( file, name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
    table_entry = dxf_create_appid ( file, name ) ;
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_dimstyle ( file, name, 
    dimpost, dimapost, dimblk, dimblk1, dimblk2, scale ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name, dimpost, dimapost, dimblk, dimblk1, dimblk2 ;
REAL scale ;
{
    DXF_TABLE_ENTRY table_entry ;
    BOOLEAN status ;
    table_entry = dxf_create_dimstyle ( file, name, dimpost, dimapost, 
        dimblk, dimblk1, dimblk2, scale ) ;
    status = ( table_entry != NULL ) && 
        dxf_put_table_entry ( file, table_entry ) ;
    dxf_free_table_entry ( table_entry ) ;
    RETURN ( status ) ;
}
#endif

