//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 -x */
/******************************* DX0T.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#ifdef DEBUG
#include <dxents.h>
#endif
#include <dx0defs.h>
#include <dxcode.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_clear_any ( code, record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT code ;
STRING record ;
{ 
    long *name ;

//    if( DX_CODE_STRING(code) == 1 || DX_CODE_STRING(code) == 1004 )
//        dx0_strass ( &record, NULL ) ;
    if ( DX_CODE_STRING(code) ) 
        dx0_strass ((DX_STRING *)record, NULL ) ;
// mhmmem    if ( DX_CODE_STRING(code) ) 
// mhmmem       dx0_strcpy ( record, NULL ) ;
    else if ( DX_CODE_REAL(code) ) 
        *((REAL*)record) = 0.0 ;
    else if ( DX_CODE_COLOR(code) ) 
        *((INT*)record) = /* DX_COLOR_BYLAYER */ DXF_ANY_COLOR ;
    else if ( DX_CODE_INT(code) ) 
        *((INT*)record) = 0 ;
    else if ( code == DXF_ENAME_CODE ) {
        name = (long*)record ;
        name[0] = name[1] = -1 ;
    }
#ifdef __ACAD11__
    else if ( code == DXF_ANY_CODE || code == DXF_LIST_CODE ) 
        *((STRING*)record) = NULL ;
#else
    else if ( code == DXF_ANY_CODE ) 
        *((STRING*)record) = NULL ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_set1_any ( code, record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT code ;
ANY record ;
{ 
    if ( DX_CODE_REAL(code) ) 
        *((REAL*)record) = 1.0 ;
    else if ( DX_CODE_INT(code) ) 
        *((INT*)record) = 1 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_setneg1_any ( code, record ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT code ;
ANY record ;
{ 
    if ( DX_CODE_REAL(code) ) 
        *((REAL*)record) = -1.0 ;
    else if ( DX_CODE_INT(code) ) 
        *((INT*)record) = -1 ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_sizeof_template ( template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT* template ;
{ 
    INT s=0, i, mode=DXF_NODEF_CODE ;

    if ( template == NULL ) 
        RETURN ( -1 ) ;
#ifdef __ACAD11__
    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( i=0 ; template[i] != DXF_END_CODE ; i++ ) {
#ifdef __ACAD11__
        if ( mode != DXF_LIST_CODE )
            s += dx0_sizeof ( template[i] ) ; 
        if ( template[i] == DXF_LIST_CODE )
            mode = DXF_LIST_CODE ;
        else if ( template[i] < 0 )
            mode = DXF_NODEF_CODE ;
#else
        s += dx0_sizeof ( template[i] ) ;
#endif
    }
    RETURN ( s ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_init_template ( record, template ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
INT* template ;
{ 
    INT *t ;
    BOOLEAN zero_mode=TRUE, neg1_mode=FALSE, array_mode=FALSE, 
        list_mode=FALSE ;
    STRING data ;
#ifdef __ACAD11__
#ifdef DEBUG
    DX_VIEWPORT vp = (DX_VIEWPORT)record ;
#endif
    if ( template[0] == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t >= 0 ) { 
            if ( array_mode ) {
                dx0_clear_any ( DXF_ANY_CODE, data ) ;
                array_mode = FALSE ;
                data += dx0_sizeof ( DXF_ANY_CODE ) ;
            }
            else if ( !list_mode ) {
                if ( zero_mode ) 
                    dx0_clear_any ( *t, data ) ;
                else if ( neg1_mode ) 
                    dx0_setneg1_any ( *t, data ) ;
                else
                    dx0_set1_any ( *t, data ) ;
                data += dx0_sizeof ( *t ) ;
            }
        }
        else if ( *t == DXF_ARRAY_COUNT_CODE || *t == DXF_DEF0_CODE || 
            *t == DXF_NODEF_CODE ) {
            zero_mode = TRUE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_ARRAY_CODE ) {
            zero_mode = TRUE ;
            array_mode = TRUE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_DEF1_CODE ) {
            zero_mode = FALSE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_DEFNEG1_CODE ) {
            zero_mode = FALSE ;
            neg1_mode = TRUE ;
            list_mode = FALSE ;
        }
        else if ( *t == DXF_ENAME_CODE || *t == DXF_ANY_CODE ) { 
            dx0_clear_any ( *t, data ) ;
            data += dx0_sizeof ( *t ) ;
            list_mode = FALSE ;
        }
#ifdef __ACAD11__
        else if ( *t == DXF_LIST_CODE ) { 
            dx0_clear_any ( *t, data ) ;
            data += dx0_sizeof ( *t ) ;
            list_mode = TRUE ;
        }
#endif
    }
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_set_template ( record, template, code, value ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
INT* template ;
INT code ;
ANY value ;
{ 
    INT *t ;
    STRING data ;

    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t == code ) { 
            if ( DX_CODE_STRING(code) ) 
                strncpy ( data, value, dx0_sizeof(code) ) ;
            else if ( DX_CODE_REAL(code) ) 
                *((REAL*)data) = *((REAL*)value) ;
            else if ( DX_CODE_INT(code) ) 
                *((INT*)data) = *((INT*)value) ;
            RETURN ( TRUE ) ;
        }
        else
            data += dx0_sizeof ( *t ) ;
    }
    RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY dx0_get_template ( record, template, code ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
ANY record ;
INT* template ;
INT code ;
{ 
    INT *t ;
    STRING data ;

    for ( t=template, data=(STRING)record ; *t != DXF_END_CODE ; t++ ) {
        if ( *t == code ) 
            RETURN ( (ANY)data ) ;
        else
            data += dx0_sizeof ( *t ) ;
    }
    RETURN ( NULL ) ;
}


