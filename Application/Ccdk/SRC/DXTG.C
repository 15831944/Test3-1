//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 -x */
/******************************* DXTG.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_get_table_entry ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_NAME name ;
    DXF_TABLE_ENTRY table_entry ;
    DXF_TABLE_ENTRY_ID id ;
    INT *template ;

    if ( !dx0_find ( file, 0, NULL, NULL ) || !dx0_get_string ( file, name ) )
        RETURN ( NULL ) ;
    id = dx0_table_entry_id ( name ) ;
#ifdef __ACAD11__
    table_entry = dx0_alloc_table_entry ( id, file ) ;
#else
    table_entry = dx0_alloc_table_entry ( id ) ;
#endif
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;
#ifdef __ACAD11__
    template = dx0_table_entry_template ( id, dx0_file_release ( file ) ) ;
#else
    template = dx0_table_entry_template ( id ) ;
#endif
    if ( template == NULL ) 
        RETURN ( NULL ) ;

    if ( dx0_get_object ( file, DX_TE_ANY(table_entry), template, NULL, NULL ) 
        == DXF_BAD_CODE ) {
        dxf_free_table_entry ( table_entry ) ;
        RETURN ( NULL ) ;
    }
    else
        RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_get_table_entries ( file, table_entry_id, te_list )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_TABLE_ENTRY_ID table_entry_id ;
DML_LIST te_list ;
{ 
    DXF_TABLE_ENTRY table_entry ;

    if ( !dxf_set_filepos ( file, dx0_table_entry_name ( table_entry_id ), 
        DXF_FIRST_TABLE_ENTRY ) )
        RETURN ( NULL ) ;
    if ( te_list == NULL ) 
        te_list = dml_create_list ();
    if ( te_list == NULL ) 
        RETURN ( NULL ) ;

    for ( table_entry = dxf_get_table_entry ( file ) ; table_entry != NULL ;
        table_entry = dxf_get_table_entry ( file ) ) {
        if ( dml_append_data ( te_list, table_entry ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( te_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_read_table_entries ( filename, table_entry_id, 
            te_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DXF_TABLE_ENTRY_ID table_entry_id ;
DML_LIST te_list ;
{
    DXF_FILE file ;

    file = dxf_fopen ( filename, DXF_READ ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    te_list = dxf_get_table_entries ( file, table_entry_id, te_list ) ;
    dxf_fclose ( file ) ;
    RETURN ( te_list ) ;
}

