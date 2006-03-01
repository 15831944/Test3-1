//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 -x */
/******************************* DXX.C **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dxents.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxcode.h>
#include <dxpos.h>

STATIC void dxf_explode_pt __(( REAL[3], REAL[3], REAL[3], REAL, REAL, REAL, 
                                REAL, INT, INT, REAL, REAL, REAL[3] )) ;
STATIC BOOLEAN dxf_explode_template __(( char*, INT*, REAL[3], DXF_ENTITY,
                        INT, INT, char* )) ;
#ifdef __ACAD11__
STATIC DML_LIST dxf_explode_pline_vtcs __(( DXF_FILE, DML_LIST, REAL[3], 
                        DXF_ENTITY, INT, INT )) ;
STATIC DML_LIST dxf_explode_insert_attrs __(( DXF_FILE, DML_LIST, REAL[3], 
                        DXF_ENTITY, INT, INT )) ;
#else
STATIC DML_LIST dxf_explode_pline_vtcs __(( DML_LIST, REAL[3], 
                        DXF_ENTITY, INT, INT )) ;
STATIC DML_LIST dxf_explode_insert_attrs __(( DML_LIST, REAL[3], 
                        DXF_ENTITY, INT, INT )) ;
#endif
/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_block_base_pt ( file, block_name, base_pt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* It has been positioned at the beginning of the block */
DXF_FILE file ;
STRING block_name ;
REAL base_pt[3] ;
{ 
    fpos_t pos ;
    INT template[4] ;

    template[0] = 10 ;
    template[1] = 20 ;
    template[2] = 30 ;
    template[3] = DXF_END_CODE ;

    if ( !dx0_getpos ( file, &pos ) )
        RETURN ( FALSE ) ;
    if ( block_name != NULL && !dx0_setpos_block ( file, block_name ) ) {
        (void)dx0_setpos ( file, &pos ) ;
        RETURN ( FALSE ) ;
    }

    base_pt[0] = 0.0 ;
    base_pt[1] = 0.0 ;
    base_pt[2] = 0.0 ;
    if ( dx0_get_object ( file, base_pt, template, NULL, NULL ) == 
        DXF_BAD_CODE ) {
        (void)dx0_setpos ( file, &pos ) ;
        RETURN ( FALSE ) ;
    }
    else
        RETURN ( dx0_setpos ( file, &pos ) ) ;
}

/*-------------------------------------------------------------------------*/
STATIC void dxf_explode_pt ( p0, base_pt, insert_pt, x_scale, y_scale, 
        z_scale, ang, i, j, hx, hy, p1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL p0[3], base_pt[3], insert_pt[3], x_scale, y_scale, z_scale, 
        ang, hx, hy, p1[3] ;
INT i, j ;
{ 
    REAL c, s, x, y ;

    x = x_scale * ( p0[0] - base_pt[0] ) ;
    y = y_scale * ( p0[1] - base_pt[1] ) ;
    ang *= PI_OVER_180 ;
    c = cos ( ang ) ;
    s = sin ( ang ) ;
    p1[0] = insert_pt[0] + (REAL)i * hx + x * c - y * s ;
    p1[1] = insert_pt[1] + (REAL)j * hy + x * s + y * c ;
    p1[2] = insert_pt[2] + z_scale * ( p0[2] - base_pt[2] ) ;
}


/*-------------------------------------------------------------------------*/
STATIC BOOLEAN dxf_explode_template ( object0, template, base_pt, 
    insert, i, j, object1 )           
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING object0, object1 ;
INT *template ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{ 
    STRING data0, data1 ;
    INT *t, step, s ;

    for ( t=template, data0=object0,  data1=object1 ; 
        *t != DXF_END_CODE ; t+=step ) {

        step = 1 ;

        if ( DX_CODE_STRING(*t) ) {
            strcpy ( data1, data0 ) ;
            s = dx0_sizeof ( *t ) ;
        }

        else if ( DX_CODE_INT(*t) ) {
            *(INT*)data1 = *(INT*)data0 ;
            s = dx0_sizeof ( *t ) ;
        }

        else if ( *t == 40 ) {
            *(REAL*)data1 = fabs(DXF_INSERT_X_SCALE(insert)) * *(REAL*)data0 ;
            s = sizeof ( REAL ) ;
        }
#ifdef __ACAD11__
        else if ( ( *t > 40 && *t < 49 ) || ( *t > 1040 && *t <= 1042 ) ) {
#else
        else if ( *t > 40 && *t < 49 ) {
#endif
            *(REAL*)data1 = DXF_INSERT_X_SCALE(insert) * *(REAL*)data0 ;
            s = sizeof ( REAL ) ;
        }
        else if ( *t >= 50 && *t < 58 && 
            ( *t != 51 || *(t-1) != DXF_DEF0_CODE ) ) {
            *(REAL*)data1 = *(REAL*)data0 + DXF_INSERT_ROT_ANG(insert) ;
            if ( DXF_INSERT_X_SCALE(insert) < 0.0 ) 
                *(REAL*)data1 += 180.0 ;
            if ( *(REAL*)data1 >= 360.0 ) 
                *(REAL*)data1 -= 360.0 ;
            s = sizeof ( REAL ) ;
        }
#ifdef __ACAD11__
        else if ( ( *t >= 10 && *t <= 18 ) || ( *t >= 1010 && *t <= 1013 ) ) {
#else
        else if ( *t >= 10 && *t <= 18 ) {
#endif
            dxf_explode_pt ( (REAL*)data0, base_pt, DXF_INSERT_PT(insert), 
                DXF_INSERT_X_SCALE(insert), DXF_INSERT_Y_SCALE(insert), 
                DXF_INSERT_Z_SCALE(insert), DXF_INSERT_ROT_ANG(insert), 
                i, j, DXF_INSERT_COL_SPC(insert), DXF_INSERT_ROW_SPC(insert), 
                (REAL*)data1 ) ;
            s = 3 * sizeof ( REAL ) ;
            step = 3 ;
        }
        else if ( *t == 210 ) {
            s = 3 * sizeof ( REAL ) ;
            step = 3 ;
        }
        else 
            s = dx0_sizeof ( *t ) ;
        data0 += s ;
        data1 += s ;
    }
    RETURN ( TRUE ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_explode_entity ( file, entity, base_pt, 
            insert, i, j )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{
    DXF_ENTITY new ;
    INT *template ;

    template = dx0_entity_template ( DX_ENTITY_ID(entity), 
        dx0_file_release ( file ) ) ;
    if ( template == NULL )
        RETURN ( NULL ) ;
    new = dx0_alloc_entity ( DX_ENTITY_ID(entity), file ) ;
    if ( new == NULL ) 
        RETURN ( NULL ) ;
    if ( !dxf_explode_template ( (char*)DX_ENTITY_ANY(entity), template, 
        base_pt, insert, i, j, (char*)DX_ENTITY_ANY(new) ) ) {
        dxf_free_entity ( new ) ;
        RETURN ( NULL ) ;
    }
    if ( DXF_ENTITY_IS_PLINE(entity) ) {
        DXF_PLINE_VLIST(new) = dxf_explode_pline_vtcs ( file, 
            DXF_PLINE_VLIST(entity), base_pt, insert, i, j ) ; 
        if ( DXF_PLINE_VLIST(new) == NULL ) {
            dxf_free_entity ( new ) ;
            RETURN ( FALSE ) ;
        }
    }
    if ( DXF_ENTITY_IS_INSERT(entity) ) {
        DXF_INSERT_ATTRS(new) = 
            dxf_explode_insert_attrs ( file, DXF_INSERT_ATTRS(entity), 
                base_pt, insert, i, j ) ; 
    }
    RETURN ( new ) ;
}

#else
/*-------------------------------------------------------------------------*/
BBS_PUBLIC DXF_ENTITY dxf_explode_entity ( entity, base_pt, insert, i, j )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{
    DXF_ENTITY new ;
    INT *template ;

    template = dx0_entity_template ( DX_ENTITY_ID(entity) ) ;
    if ( template == NULL )
        RETURN ( NULL ) ;
    new = dx0_alloc_entity ( DX_ENTITY_ID(entity) ) ;
    if ( new == NULL ) 
        RETURN ( NULL ) ;
    if ( !dxf_explode_template ( (char*)DX_ENTITY_ANY(entity), template, 
        base_pt, insert, i, j, (char*)DX_ENTITY_ANY(new) ) ) {
        dxf_free_entity ( new ) ;
        RETURN ( NULL ) ;
    }
    if ( DXF_ENTITY_IS_PLINE(entity) ) {
        DXF_PLINE_VLIST(new) = dxf_explode_pline_vtcs ( 
            DXF_PLINE_VLIST(entity), base_pt, insert, i, j ) ; 
        if ( DXF_PLINE_VLIST(new) == NULL ) {
            dxf_free_entity ( new ) ;
            RETURN ( FALSE ) ;
        }
    }
    if ( DXF_ENTITY_IS_INSERT(entity) ) {
        DXF_INSERT_ATTRS(new) = 
            dxf_explode_insert_attrs ( DXF_INSERT_ATTRS(entity), 
                base_pt, insert, i, j ) ; 
    }
    RETURN ( new ) ;
}
#endif
#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
STATIC DML_LIST dxf_explode_pline_vtcs ( file, old_list, base_pt, 
        insert, i, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST old_list ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{ 
    DXF_ENTITY new_vertex ;
    DML_LIST new_list ;
    DML_ITEM item ;

    if ( old_list == NULL ) 
        RETURN ( NULL ) ;
    new_list = dml_create_list();
    if ( new_list == NULL ) 
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( old_list, item ) {
        new_vertex = dxf_explode_entity ( file, DML_RECORD(item), 
            base_pt, insert, i, j ) ;
        if ( new_vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( new_list, new_vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( new_list ) ;
}

#else
/*-------------------------------------------------------------------------*/
STATIC DML_LIST dxf_explode_pline_vtcs ( old_list, base_pt, insert, i, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST old_list ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{ 
    DXF_ENTITY new_vertex ;
    DML_LIST new_list ;
    DML_ITEM item ;

    if ( old_list == NULL ) 
        RETURN ( NULL ) ;
    new_list = dml_create_list();
    if ( new_list == NULL ) 
        RETURN ( FALSE ) ;
    DML_WALK_LIST ( old_list, item ) {
        new_vertex = dxf_explode_entity ( DML_RECORD(item), 
            base_pt, insert, i, j ) ;
        if ( new_vertex == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( new_list, new_vertex ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( new_list ) ;
}
#endif

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
STATIC DML_LIST dxf_explode_insert_attrs ( file, old_list, base_pt, 
        insert, i, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DML_LIST old_list ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{ 
    DXF_ENTITY new_attr ;
    DML_LIST new_list ;
    DML_ITEM item ;

    if ( old_list == NULL ) 
        RETURN ( NULL ) ;
    new_list = dml_create_list();
    if ( new_list == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( old_list, item ) {
        new_attr = dxf_explode_entity ( file, DML_RECORD(item), 
            base_pt, insert, i, j ) ;
        if ( new_attr == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( new_list, new_attr ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( new_list ) ;
}

#else
/*-------------------------------------------------------------------------*/
STATIC DML_LIST dxf_explode_insert_attrs ( old_list, base_pt, insert, i, j ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DML_LIST old_list ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
INT i, j ;
{ 
    DXF_ENTITY new_attr ;
    DML_LIST new_list ;
    DML_ITEM item ;

    if ( old_list == NULL ) 
        RETURN ( NULL ) ;
    new_list = dml_create_list();
    if ( new_list == NULL ) 
        RETURN ( NULL ) ;
    DML_WALK_LIST ( old_list, item ) {
        new_attr = dxf_explode_entity ( DML_RECORD(item), 
            base_pt, insert, i, j ) ;
        if ( new_attr == NULL ) 
            RETURN ( NULL ) ;
        if ( dml_append_data ( new_list, new_attr ) == NULL )
            RETURN ( NULL ) ;
    }
    RETURN ( new_list ) ;
}
#endif

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_explode_entities ( file, entity, base_pt, 
            insert, entitylist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY entity ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
DML_LIST entitylist ;
{
    DXF_ENTITY new ;
    INT i, j ;

    for ( i=0 ; i<DXF_INSERT_COLS_CNT(insert) ; i++ )
        for ( j=0 ; j<DXF_INSERT_ROWS_CNT(insert) ; j++ ) {
            new = dxf_explode_entity ( file, entity, base_pt, insert, i, j ) ;
            if ( new == NULL ) 
                RETURN ( FALSE ) ;
            if ( dml_append_data ( entitylist, new ) == NULL ) 
                RETURN ( FALSE ) ;
        }
    RETURN ( TRUE ) ;
}
#else
/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN dxf_explode_entities ( entity, base_pt, insert, 
            entitylist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
REAL base_pt[3] ;
DXF_ENTITY insert ;
DML_LIST entitylist ;
{
    DXF_ENTITY new ;
    INT i, j ;

    for ( i=0 ; i<DXF_INSERT_COLS_CNT(insert) ; i++ )
        for ( j=0 ; j<DXF_INSERT_ROWS_CNT(insert) ; j++ ) {
            new = dxf_explode_entity ( entity, base_pt, insert, i, j ) ;
            if ( new == NULL ) 
                RETURN ( FALSE ) ;
            if ( dml_append_data ( entitylist, new ) == NULL ) 
                RETURN ( FALSE ) ;
        }
    RETURN ( TRUE ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC DML_LIST dxf_explode ( file, insert, filter, entitylist )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
DXF_ENTITY insert ;
DXF_FILTER filter ;
DML_LIST entitylist ;
{
    fpos_t pos ;
    REAL base_pt[3] ;
    DXF_ENTITY entity ;

    if ( !DXF_ENTITY_IS_INSERT(insert) ) 
        RETURN ( NULL ) ;

    if ( !dx0_getpos ( file, &pos ) )
        RETURN ( NULL ) ;
    if ( !dx0_setpos_block ( file, DXF_INSERT_BLOCK_GET(insert) ) ||
         !dxf_block_base_pt ( file, NULL, base_pt ) ) {
        (void)dx0_setpos ( file, &pos ) ;
        RETURN ( NULL ) ;
    }
    if ( entitylist == NULL ) 
        entitylist = dml_create_list ();
    if ( entitylist == NULL ) {
        (void)dx0_setpos ( file, &pos ) ;
        RETURN ( NULL ) ;
    }

    for ( entity = dxf_get_entity ( file, filter ) ; entity != NULL ;
        entity = dxf_get_entity ( file, filter ) ) {
#ifdef __ACAD11__
        if ( !dxf_explode_entities ( file, entity, base_pt, insert, 
            entitylist ) ) {
#else
        if ( !dxf_explode_entities ( entity, base_pt, insert, entitylist ) ) {
#endif
            (void)dx0_setpos ( file, &pos ) ;
            RETURN ( NULL ) ;
        }
        dxf_free_entity ( entity ) ;
    }
    (void)dx0_setpos ( file, &pos ) ;
    RETURN ( entitylist ) ;
}

