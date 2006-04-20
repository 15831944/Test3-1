//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 -x */
/******************************* DXHP.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dx0defs.h>
#include <dxfilm.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_put_hdr_var ( file, var_name, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING var_name ;
ANY data ;
{ 
    INT *template = dx0_hdr_var_template ( var_name ) ;

    if ( template == NULL ) 
        RETURN ( FALSE ) ;
    if ( !DX_FILE_HEADER_OPEN(file) )
        dxf_open_header ( file ) ;
    if ( DX_FILE_HEADER_CLOSED(file) )
        RETURN ( FALSE ) ;
    if ( _stricmp ( var_name, "$HANDLING" ) == 0 && *(INT*)data )
        DX_FILE_SET_ENT_HNDL_ENABLED(file) ;
    RETURN ( dx0_put_string_group ( file, 9, var_name ) && 
        dx0_put_object ( file, (char*)data, template, TRUE, TRUE ) ) ;
}


