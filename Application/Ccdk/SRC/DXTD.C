//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 -x */
/******************************* DXTD.C *********************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!                                                      !!!!!!!!*/ 
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#include <string.h> 
#include <dxtbls.h>
#include <dx0defs.h>

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_layer ( file, name, 
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
    DXF_TABLE_ENTRY table_entry = 
        dx0_alloc_table_entry ( DXF_LAYER, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;
//      dx0_strcpy ( DXF_LAYER_NAME(table_entry), name ) ;
    DXF_LAYER_NAME_SET(table_entry, name ) ;
    DXF_LAYER_FLAGS(table_entry) = frozen ? 1 : 0 ;
    DXF_LAYER_COLOR(table_entry) = color ;

//    dx0_strcpy ( DXF_LAYER_LTYPE(table_entry), ltypename ) ;
    DXF_LAYER_LTYPE_SET(table_entry, ltypename ) ;


    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_ltype ( file, name, text, algn, 
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
    INT i ;
    DXF_TABLE_ENTRY table_entry = 
        dx0_alloc_table_entry ( DXF_LTYPE, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

//    dx0_strcpy ( DXF_LTYPE_NAME(table_entry), name ) ;
    DXF_LTYPE_NAME_SET(table_entry, name ) ;
//    dx0_strcpy ( DXF_LTYPE_TEXT(table_entry), text ) ;
    DXF_LTYPE_TEXT_SET(table_entry, text ) ;
    DXF_LTYPE_ALGN(table_entry) = algn ;
    DXF_LTYPE_TOT_LEN(table_entry) = tot_len ;
    DXF_LTYPE_CNT(table_entry) = count ;
    if ( count > 0 ) {
        DXF_LTYPE_LEN(table_entry) = MALLOC ( count, REAL ) ;
        if ( DXF_LTYPE_LEN(table_entry) == NULL ) {
            dx0_set_file_status ( file, DXF_MALLOC_FAILURE ) ;
            RETURN ( NULL ) ;
        }
        for ( i=0 ; i<count ; i++ ) 
            DXF_LTYPE_LEN(table_entry)[i] = len[i] ;
    }
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_style ( file, name, flags, height, width, 
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
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_STYLE, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;
//    dx0_strcpy ( DXF_STYLE_NAME(table_entry), name ) ;
    DXF_STYLE_NAME_SET(table_entry, name ) ;
    DXF_STYLE_FLAGS(table_entry) = flags ;
    DXF_STYLE_HEIGHT(table_entry) = height ;
    DXF_STYLE_WIDTH(table_entry) = width ;
    DXF_STYLE_OBL_ANG(table_entry) = angle ;
    DXF_STYLE_GFLAGS(table_entry) = text_flags ;
    DXF_STYLE_LAST_HT(table_entry) = last_height ;
//    dx0_strcpy ( DXF_STYLE_FONT(table_entry), font_filename ) ;
//    dx0_strcpy ( DXF_STYLE_BIGFONT(table_entry), bigfont_filename ) ;

    DXF_STYLE_FONT_SET(table_entry, font_filename ) ;
    DXF_STYLE_BIGFONT_SET(table_entry, bigfont_filename ) ;


    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_view ( file, name, h, w, ctr, 
        dir, tgt_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
REAL h, w, ctr[3], dir[3], tgt_pt[3] ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_VIEW, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

    DXF_VIEW_HEIGHT(table_entry) = h ;
    DXF_VIEW_WIDTH(table_entry) = w ;
//    dx0_strcpy ( DXF_VIEW_NAME(table_entry), name ) ;
    DXF_VIEW_NAME_SET(table_entry, name ) ;
    dx0_copy_point ( ctr, DXF_VIEW_CTR(table_entry) ) ;
    dx0_copy_point ( dir, DXF_VIEW_DIR(table_entry) ) ;
    dx0_copy_point ( tgt_pt, DXF_VIEW_TGT_PT(table_entry) ) ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_ucs ( file, name, 
        origin, x_axis, y_axis )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
REAL origin[3], x_axis[3], y_axis[3] ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_UCS, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;


//    dx0_strcpy ( DXF_UCS_NAME(table_entry), name ) ;
    DXF_UCS_NAME_SET(table_entry, name ) ;
    dx0_copy_point ( origin, DXF_UCS_ORIGIN(table_entry) ) ;
    dx0_copy_point ( x_axis, DXF_UCS_X_AXIS(table_entry) ) ;
    dx0_copy_point ( y_axis, DXF_UCS_Y_AXIS(table_entry) ) ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_vport ( file, name, x0, y0, x1, y1, 
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
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_VPORT, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

    DXF_VP_NAME_SET(table_entry, name ) ;
    DXF_VP_LL(table_entry)[0] = x0 ;
    DXF_VP_LL(table_entry)[1] = y0 ;
    DXF_VP_UR(table_entry)[0] = x1 ;
    DXF_VP_UR(table_entry)[1] = y1 ;
    DXF_VP_CTR(table_entry)[0] = ctr[0] ;
    DXF_VP_CTR(table_entry)[1] = ctr[1] ;
    dx0_copy_point ( dir, DXF_VP_DIR(table_entry) ) ;
    dx0_copy_point ( tgt_pt, DXF_VP_TGT_PT(table_entry) ) ;
    DXF_VP_HEIGHT(table_entry) = height ;
    DXF_VP_AR(table_entry) = ar ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_appid ( file, name ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING name ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_APPID, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

//    dx0_strcpy ( DXF_APPID_NAME(table_entry), name ) ;
    DXF_APPID_NAME_SET(table_entry, name ) ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_dimstyle ( file, name, 
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
    DXF_TABLE_ENTRY table_entry = 
        dx0_alloc_table_entry ( DXF_DIMSTYLE, file ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

//    dx0_strcpy ( DXF_DIMSTYLE_NAME(table_entry), name ) ;
//    dx0_strcpy ( DXF_DIMSTYLE_DIMPOST(table_entry), dimpost ) ; 
//    dx0_strcpy ( DXF_DIMSTYLE_DIMAPOST(table_entry), dimapost ) ; 
//    dx0_strcpy ( DXF_DIMSTYLE_DIMBLK1(table_entry), dimblk1 ) ;
//    dx0_strcpy ( DXF_DIMSTYLE_DIMBLK2(table_entry), dimblk2 ) ;

    DXF_DIMSTYLE_NAME_SET(table_entry, name ) ;
    DXF_DIMSTYLE_DIMPOST_SET(table_entry, dimpost ) ; 
    DXF_DIMSTYLE_DIMAPOST_SET(table_entry, dimapost ) ; 
    DXF_DIMSTYLE_DIMBLK1_SET(table_entry, dimblk1 ) ;
    DXF_DIMSTYLE_DIMBLK2_SET(table_entry, dimblk2 ) ;
    DXF_DIMSTYLE_DIMBLK_SET(table_entry, dimblk ) ;// handle size
    
    DXF_DIMSTYLE_DIMSCALE(table_entry) = scale ;
    RETURN ( table_entry ) ;
}

#else
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_layer ( name, color, frozen, ltypename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name, ltypename ;
INT color ;
BOOLEAN frozen ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_LAYER ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;
//    dx0_strcpy ( DXF_LAYER_NAME(table_entry), name ) ;
    DXF_LAYER_NAME_SET(table_entry, name ) ;
    DXF_LAYER_FLAGS(table_entry) = frozen ? 1 : 0 ;
    DXF_LAYER_COLOR(table_entry) = color ;


//    dx0_strcpy ( DXF_LAYER_LTYPE(table_entry), ltypename ) ;
    DXF_LAYER_LTYPE_SET(table_entry, ltypename ) ;


    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_ltype ( name, text, algn, 
        tot_len, count, len ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name, text ;
INT algn, count ;
REAL tot_len, *len ;
{
    INT i ;
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_LTYPE ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

//    dx0_strcpy ( DXF_LTYPE_NAME(table_entry), name ) ;
    DXF_LTYPE_NAME_SET(table_entry, name ) ;
//    dx0_strcpy ( DXF_LTYPE_TEXT(table_entry), text ) ;
    DXF_LTYPE_TEXT_SET(table_entry, text ) ;
    DXF_LTYPE_ALGN(table_entry) = algn ;
    DXF_LTYPE_TOT_LEN(table_entry) = tot_len ;
    DXF_LTYPE_CNT(table_entry) = count ;
    if ( count > 0 ) {
        DXF_LTYPE_LEN(table_entry) = 
            MALLOC ( count, REAL ) ;
        if ( DXF_LTYPE_LEN(table_entry) == NULL )
            RETURN ( NULL ) ;
        for ( i=0 ; i<count ; i++ ) 
            DXF_LTYPE_LEN(table_entry)[i] = len[i] ;
    }
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_style ( name, flags, height, width, 
    angle, text_flags, last_height, font_filename, bigfont_filename )  
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name ;
INT flags ; /* code 70 */
REAL height, width, angle ;
INT text_flags ; /* code 71 */
REAL last_height ;
STRING font_filename, bigfont_filename ;
{
    DXF_TABLE_ENTRY table_entry = 
        dx0_alloc_table_entry ( DXF_STYLE ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;
//    dx0_strcpy ( DXF_STYLE_NAME(table_entry), name ) ;
    DXF_STYLE_NAME_SET(table_entry, name ) ;
    DXF_STYLE_FLAGS(table_entry) = flags ;
    DXF_STYLE_HEIGHT(table_entry) = height ;
    DXF_STYLE_WIDTH(table_entry) = width ;
    DXF_STYLE_OBL_ANG(table_entry) = angle ;
    DXF_STYLE_GFLAGS(table_entry) = text_flags ;
    DXF_STYLE_LAST_HT(table_entry) = last_height ;
//    dx0_strcpy ( DXF_STYLE_FONT(table_entry), font_filename ) ;
//    dx0_strcpy ( DXF_STYLE_BIGFONT(table_entry), bigfont_filename ) ;
    DXF_STYLE_FONT_SET(table_entry, font_filename ) ;
    DXF_STYLE_BIGFONT_SET(table_entry, bigfont_filename ) ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_view ( name, h, w, ctr, dir, tgt_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name ;
REAL h, w, ctr[3], dir[3], tgt_pt[3] ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_VIEW ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

    DXF_VIEW_HEIGHT(table_entry) = h ;
    DXF_VIEW_WIDTH(table_entry) = w ;
//    dx0_strcpy ( DXF_VIEW_NAME(table_entry), name ) ;
    DXF_VIEW_NAME_SET(table_entry, name ) ;
    dx0_copy_point ( ctr, DXF_VIEW_CTR(table_entry) ) ;
    dx0_copy_point ( dir, DXF_VIEW_DIR(table_entry) ) ;
    dx0_copy_point ( tgt_pt, DXF_VIEW_TGT_PT(table_entry) ) ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_ucs ( name, origin, x_axis, y_axis )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name ;
REAL origin[3], x_axis[3], y_axis[3] ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_UCS ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

    DXF_UCS_NAME_SET(table_entry, name ) ;
    dx0_copy_point ( origin, DXF_UCS_ORIGIN(table_entry) ) ;
    dx0_copy_point ( x_axis, DXF_UCS_X_AXIS(table_entry) ) ;
    dx0_copy_point ( y_axis, DXF_UCS_Y_AXIS(table_entry) ) ;
    RETURN ( table_entry ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_TABLE_ENTRY dxf_create_vport ( name, x0, y0, x1, y1, ctr, dir, 
        tgt_pt, height, ar ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING name ;
REAL x0, y0, x1, y1, ctr[2], dir[3], tgt_pt[3], 
            height, ar ;
{
    DXF_TABLE_ENTRY table_entry = dx0_alloc_table_entry ( DXF_VPORT ) ;
    if ( table_entry == NULL ) 
        RETURN ( NULL ) ;

    DXF_VP_NAME_SET(table_entry, name ) ;
    DXF_VP_LL(table_entry)[0] = x0 ;
    DXF_VP_LL(table_entry)[1] = y0 ;
    DXF_VP_UR(table_entry)[0] = x1 ;
    DXF_VP_UR(table_entry)[1] = y1 ;
    DXF_VP_CTR(table_entry)[0] = ctr[0] ;
    DXF_VP_CTR(table_entry)[1] = ctr[1] ;
    dx0_copy_point ( dir, DXF_VP_DIR(table_entry) ) ;
    dx0_copy_point ( tgt_pt, DXF_VP_TGT_PT(table_entry) ) ;
    DXF_VP_HEIGHT(table_entry) = height ;
    DXF_VP_AR(table_entry) = ar ;
    RETURN ( table_entry ) ;
}
#endif
