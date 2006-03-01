//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 -x */
/******************************* DXT0.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dxtbls.h>
#include <dx0defs.h>
#include <dxttmpls.h>

#ifdef __ACAD11__

GLOBAL INT DXF_LTYPE_TEMPLATE_10[]    = { DX_LTYPE_TEMPLATE_10 } ;
GLOBAL INT DXF_LAYER_TEMPLATE_10[]    = { DX_LAYER_TEMPLATE_10 } ; 
GLOBAL INT DXF_STYLE_TEMPLATE_10[]    = { DX_STYLE_TEMPLATE_10 } ;
GLOBAL INT DXF_VIEW_TEMPLATE_10[]     = { DX_VIEW_TEMPLATE_10 } ;
GLOBAL INT DXF_UCS_TEMPLATE_10[]      = { DX_UCS_TEMPLATE_10 } ;
GLOBAL INT DXF_VPORT_TEMPLATE_10[]    = { DX_VPORT_TEMPLATE_10 } ;

GLOBAL INT DXF_LTYPE_TEMPLATE_11[]    = { DX_LTYPE_TEMPLATE_11 } ;
GLOBAL INT DXF_LAYER_TEMPLATE_11[]    = { DX_LAYER_TEMPLATE_11 } ; 
GLOBAL INT DXF_STYLE_TEMPLATE_11[]    = { DX_STYLE_TEMPLATE_11 } ;
GLOBAL INT DXF_VIEW_TEMPLATE_11[]     = { DX_VIEW_TEMPLATE_11 } ;
GLOBAL INT DXF_UCS_TEMPLATE_11[]      = { DX_UCS_TEMPLATE_11 } ;
GLOBAL INT DXF_VPORT_TEMPLATE_11[]    = { DX_VPORT_TEMPLATE_11 } ;
GLOBAL INT DXF_APPID_TEMPLATE_11[]    = { DX_VPORT_TEMPLATE_11 } ;
GLOBAL INT DXF_DIMSTYLE_TEMPLATE_11[] = { DX_VPORT_TEMPLATE_11 } ;

#else

GLOBAL INT DXF_LTYPE_TEMPLATE[]    = { DX_LTYPE_TEMPLATE } ;
GLOBAL INT DXF_LAYER_TEMPLATE[]    = { DX_LAYER_TEMPLATE } ; 
GLOBAL INT DXF_STYLE_TEMPLATE[]    = { DX_STYLE_TEMPLATE } ;
GLOBAL INT DXF_VIEW_TEMPLATE[]     = { DX_VIEW_TEMPLATE } ;
GLOBAL INT DXF_UCS_TEMPLATE[]      = { DX_UCS_TEMPLATE } ;
GLOBAL INT DXF_VPORT_TEMPLATE[]    = { DX_VPORT_TEMPLATE } ;

#endif

GLOBAL char    DXF_LTYPE_WORD[]    = { "LTYPE" } ;
GLOBAL char    DXF_LAYER_WORD[]    = { "LAYER" } ;
GLOBAL char    DXF_STYLE_WORD[]    = { "STYLE" } ;
GLOBAL char    DXF_VIEW_WORD[]     = { "VIEW" } ;
GLOBAL char    DXF_UCS_WORD[]      = { "UCS" } ;
GLOBAL char    DXF_VPORT_WORD[]    = { "VPORT" } ;

#ifdef __ACAD11__
GLOBAL char    DXF_APPID_WORD[]    = { "APPID" } ;
GLOBAL char    DXF_DIMSTYLE_WORD[] = { "DIMSTYLE" } ;
#endif

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_TABLE_ENTRY dx0_alloc_table_entry ( table_entry_id, file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_TABLE_ENTRY_ID table_entry_id ;
DXF_FILE file ;
{ 
    DXF_TABLE_ENTRY table_entry ;
    INT s, *template, release ;

    release = dx0_file_release ( file ) ;
    template = dx0_table_entry_template ( table_entry_id, release ) ;

    if ( template == NULL ) {
        dx0_set_file_status ( file, DXF_TABLE_ENTRY_UNKNOWN ) ;
        RETURN ( NULL ) ;
    }
    s = sizeof ( DXF_TABLE_ENTRY_ID ) + dx0_sizeof_template ( template ) +
        sizeof ( DXF_ENAME ) ;
    table_entry = (DXF_TABLE_ENTRY) MALLOC ( s, char ) ;
    if ( table_entry == NULL ) {
        dx0_set_file_status ( file, DXF_MALLOC_FAILURE ) ;
        RETURN ( NULL ) ;
    }
    DXF_ENAME_INIT(DX_TE_ENAME(table_entry)) ;
    DX_TE_ID(table_entry) = table_entry_id ;
    dx0_init_template ( DX_TE_ANY(table_entry), template ) ;
    RETURN ( table_entry ) ;
}
#else

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_TABLE_ENTRY dx0_alloc_table_entry ( table_entry_id ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_TABLE_ENTRY_ID table_entry_id ;
{ 
    DXF_TABLE_ENTRY table_entry ;
    INT s, *template = dx0_table_entry_template ( table_entry_id ) ;

    if ( template == NULL ) 
        RETURN ( NULL ) ;
    s = sizeof ( DXF_TABLE_ENTRY_ID ) + dx0_sizeof_template ( template ) +
        sizeof ( DXF_ENAME ) ;
    table_entry = (DXF_TABLE_ENTRY) MALLOC ( s, char ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;
    DXF_ENAME_INIT(DX_TE_ENAME(table_entry)) ;
    DX_TE_ID(table_entry) = table_entry_id ;
    dx0_init_template ( DX_TE_ANY(table_entry), template ) ;
    RETURN ( table_entry ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_free_table_entry ( table_entry ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_TABLE_ENTRY table_entry ;
{ 
    if ( DX_TE_ID(table_entry) == DXF_LTYPE ) 
        FREE ( DXF_LTYPE_LEN(table_entry) ) ;

    switch( DX_TE_ID(table_entry) )
      {
      case  DXF_LTYPE     :
        dx0_strfree(DXF_LTYPE_NAME_PTR(table_entry));
        dx0_strfree(DXF_LTYPE_TEXT_PTR(table_entry));
        break;
      case  DXF_LAYER     :
        dx0_strfree(DXF_LAYER_NAME_PTR(table_entry));     
        dx0_strfree(DXF_LAYER_LTYPE_PTR(table_entry));    
        break;
      case  DXF_STYLE     :
        dx0_strfree(DXF_STYLE_NAME_PTR(table_entry));    
        dx0_strfree(DXF_STYLE_FONT_PTR(table_entry));    
        dx0_strfree(DXF_STYLE_BIGFONT_PTR(table_entry)); 
        break;
      case  DXF_VIEW      :
        dx0_strfree(DXF_VIEW_NAME_PTR(table_entry));
        break;
      case  DXF_UCS       :
        dx0_strfree(DXF_UCS_NAME_PTR(table_entry));
        break;
      case  DXF_VPORT     :
        dx0_strfree(DXF_VP_NAME_PTR(table_entry));
        break;
      case  DXF_DIMSTYLE  :
        dx0_strfree(DXF_DIMSTYLE_NAME_PTR(table_entry));     
        dx0_strfree(DXF_DIMSTYLE_DIMPOST_PTR(table_entry));  
        dx0_strfree(DXF_DIMSTYLE_DIMAPOST_PTR(table_entry));
        dx0_strfree(DXF_DIMSTYLE_DIMBLK1_PTR(table_entry));  
        dx0_strfree(DXF_DIMSTYLE_DIMBLK2_PTR(table_entry));  
        dx0_strfree(DXF_DIMSTYLE_DIMBLK_PTR(table_entry));  
        break;
      case  DXF_APPID     :
        dx0_strfree(DXF_APPID_NAME_PTR(table_entry));
        break;
      default:
        break;
      }


    FREE ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_TABLE_ENTRY_ID dx0_table_entry_id ( name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name ;
{ 
    if ( strcmp ( name, DXF_LTYPE_WORD ) == 0 ) 
        RETURN ( DXF_LTYPE ) ;
    else if ( strcmp ( name, DXF_LAYER_WORD ) == 0 ) 
        RETURN ( DXF_LAYER ) ;
    else if ( strcmp ( name, DXF_STYLE_WORD ) == 0 ) 
        RETURN ( DXF_STYLE ) ;
    else if ( strcmp ( name, DXF_VIEW_WORD ) == 0 ) 
        RETURN ( DXF_VIEW ) ;
    else if ( strcmp ( name, DXF_UCS_WORD ) == 0 ) 
        RETURN ( DXF_UCS ) ;
    else if ( strcmp ( name, DXF_VPORT_WORD ) == 0 ) 
        RETURN ( DXF_VPORT ) ;
#ifdef __ACAD11__
    else if ( strcmp ( name, DXF_APPID_WORD ) == 0 ) 
        RETURN ( DXF_APPID ) ;
    else if ( strcmp ( name, DXF_DIMSTYLE_WORD ) == 0 ) 
        RETURN ( DXF_DIMSTYLE ) ;
#endif
    else 
        RETURN ( -1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE STRING dx0_table_entry_name ( table_entry_id ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_TABLE_ENTRY_ID table_entry_id ;
{ 
    if ( table_entry_id == DXF_LTYPE ) 
        RETURN ( DXF_LTYPE_WORD ) ;
    else if ( table_entry_id == DXF_LAYER ) 
        RETURN ( DXF_LAYER_WORD ) ;
    else if ( table_entry_id == DXF_STYLE ) 
        RETURN ( DXF_STYLE_WORD ) ;
    else if ( table_entry_id == DXF_VIEW ) 
        RETURN ( DXF_VIEW_WORD ) ;
    else if ( table_entry_id == DXF_UCS ) 
        RETURN ( DXF_UCS_WORD ) ;
    else if ( table_entry_id == DXF_VPORT ) 
        RETURN ( DXF_VPORT_WORD ) ;
#ifdef __ACAD11__
    else if ( table_entry_id == DXF_APPID ) 
        RETURN ( DXF_APPID_WORD ) ;
    else if ( table_entry_id == DXF_DIMSTYLE ) 
        RETURN ( DXF_DIMSTYLE_WORD ) ;
#endif
    else
        RETURN ( NULL ) ;
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT *dx0_table_entry_template ( table_entry_id, release ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_TABLE_ENTRY_ID table_entry_id ;
INT release ;
{ 
    if ( table_entry_id == DXF_LTYPE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_LTYPE_TEMPLATE_11 : DXF_LTYPE_TEMPLATE_10 ) ;
    else if ( table_entry_id == DXF_LAYER ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_LAYER_TEMPLATE_11 : DXF_LAYER_TEMPLATE_10 ) ;
    else if ( table_entry_id == DXF_STYLE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_STYLE_TEMPLATE_11 : DXF_STYLE_TEMPLATE_10 ) ;
    else if ( table_entry_id == DXF_VIEW ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_VIEW_TEMPLATE_11 : DXF_VIEW_TEMPLATE_10 ) ;
    else if ( table_entry_id == DXF_UCS ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_UCS_TEMPLATE_11 : DXF_UCS_TEMPLATE_10 ) ;
    else if ( table_entry_id == DXF_VPORT ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_VPORT_TEMPLATE_11 : DXF_VPORT_TEMPLATE_10 ) ;
    else if ( table_entry_id == DXF_APPID ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_APPID_TEMPLATE_11 : NULL ) ;
    else if ( table_entry_id == DXF_DIMSTYLE ) 
        RETURN ( ( release == DXF_ACAD_RELEASE ) ? 
            DXF_DIMSTYLE_TEMPLATE_11 : NULL ) ;
    else
        RETURN ( NULL ) ;
}
#else
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT *dx0_table_entry_template ( table_entry_id ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_TABLE_ENTRY_ID table_entry_id ;
{ 
    if ( table_entry_id == DXF_LTYPE ) 
        RETURN ( DXF_LTYPE_TEMPLATE ) ;
    else if ( table_entry_id == DXF_LAYER ) 
        RETURN ( DXF_LAYER_TEMPLATE ) ;
    else if ( table_entry_id == DXF_STYLE ) 
        RETURN ( DXF_STYLE_TEMPLATE ) ;
    else if ( table_entry_id == DXF_VIEW ) 
        RETURN ( DXF_VIEW_TEMPLATE ) ;
    else if ( table_entry_id == DXF_UCS ) 
        RETURN ( DXF_UCS_TEMPLATE ) ;
    else if ( table_entry_id == DXF_VPORT ) 
        RETURN ( DXF_VPORT_TEMPLATE ) ;
    else
        RETURN ( NULL ) ;
}
#endif
