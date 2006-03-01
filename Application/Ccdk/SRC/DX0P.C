//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 -x*/
/******************************* DX0P.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxcode.h>

#include <stdio.h>

extern FILE * dbgf;

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_code ( file, code ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
{
    short c ;

    if ( DX_FILE_ASCII(file) ) {
        if ( code < 10 ) 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "  %d\n", code ) != 0 ) ;
        else if ( code < 100 ) 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), " %d\n", code ) != 0 ) ;
        else 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "%d\n", code ) != 0 ) ;
    }
    else if ( DX_FILE_BINARY(file) ) {
        c = (short) code ;
        RETURN ( fwrite ( &c, 1, 1, DX_FILE_HANDLE(file) ) ) ;
    }
    else 
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_int ( file, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT i ;
{
    if ( DX_FILE_ASCII(file) ) {
        if ( i >= 0 && i <= 9 )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "     %d\n", i ) != 0 ) ;
        else if ( ( i >= 10 && i <= 99 ) || ( i >= -9 && i <= -1 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "    %d\n", i ) != 0 ) ;
        else if ( ( i >= 100 && i <= 999 ) || ( i >= -99 && i <= -10 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "   %d\n", i ) != 0 ) ;
        else if ( ( i >= 1000 && i <= 9999 ) || ( i >= -999 && i <= -100 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "  %d\n", i ) != 0 ) ;
        else if ( ( i >= 10000 && (long)i <= 99999 ) || 
            ( i >= -9999 && i <= -1000 ) )
            RETURN ( fprintf ( DX_FILE_HANDLE(file), " %d\n", i ) != 0 ) ;
        else 
            RETURN ( fprintf ( DX_FILE_HANDLE(file), "%d\n", i ) != 0 ) ;
    }
/*
    if ( DX_FILE_ASCII(file) ) 
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%6.d\n", i ) != 0 ) ;
*/
    else if ( DX_FILE_BINARY(file) ) 
        RETURN ( fwrite ( &i, 2, 1, DX_FILE_HANDLE(file) ) ) ;
    else 
        RETURN ( FALSE ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_long ( file, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
long i ;
{
    if ( DX_FILE_ASCII(file) ) 
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%6.d\n", i ) != 0 ) ;
    else if ( DX_FILE_BINARY(file) ) 
        RETURN ( fwrite ( &i, 2, 1, DX_FILE_HANDLE(file) ) ) ;
    else 
        RETURN ( FALSE ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_real ( file, p, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT p ;
REAL r ;
{
    REAL a ;
    INT s ;
    //long c ;
    double c;
    if ( DX_FILE_ASCII(file) ) {
        if ( p <= 0 ) 
            p = DX_FILE_PRECISION(file) ;
        for ( s=1, a=10.0*r ; s<p ; s++, a*=10.0 ) {
            if( a >= 0.0 ) // CNM
              c = floor((double)a);
            else
              c = ceil((double)a);
            //c = (long)a ; 
            if ( fabs ( (REAL)c-a ) <= BBS_ZERO ) 
                RETURN ( fprintf ( DX_FILE_HANDLE(file), "%.*lf\n", 
                    s, r ) != 0 ) ;
        }
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%.*lf\n", p, r ) != 0 ) ;
    }
    else if ( DX_FILE_BINARY(file) ) 
        RETURN ( fwrite ( &r, 8, 1, DX_FILE_HANDLE(file) ) )  ;
    else 
        RETURN ( FALSE ) ;
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_string ( file, string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
STRING string ;
{
    INT i, n ;

    if ( DX_FILE_ASCII(file) ) 
        RETURN ( fprintf ( DX_FILE_HANDLE(file), "%s\n", string ) != 0 ) ;

    else if ( DX_FILE_BINARY(file) ) {
        for ( i=0 ; i<DX_BUFFER_SIZE ; i++ ) {
            n = fwrite ( string+i, 1, 1, DX_FILE_HANDLE(file) ) ;
            if ( n <= 0 ) 
                RETURN ( FALSE ) ;
            if ( string[i] == '\0' ) 
                RETURN ( TRUE ) ;
        }
        RETURN ( FALSE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}




/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_dxstring ( DXF_FILE file, DX_STRING *string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{

    if ( DX_FILE_ASCII(file) ) 
        if( string && string->p )
          RETURN ( fprintf ( DX_FILE_HANDLE(file), "%s\n", string->p ) != 0 );
        else 
          RETURN ( fprintf ( DX_FILE_HANDLE(file), "%s\n", " " ) != 0 );

    //else if ( DX_FILE_BINARY(file) ) {
    //    for ( i=0 ; i<DX_BUFFER_SIZE ; i++ ) {
    //        n = fwrite ( string+i, 1, 1, DX_FILE_HANDLE(file) ) ;
    //        if ( n <= 0 ) 
    //            RETURN ( FALSE ) ;
    //        if ( string[i] == '\0' ) 
    //            RETURN ( TRUE ) ;
    //    }
    //    RETURN ( FALSE ) ;
    //}
    else 
        RETURN ( FALSE ) ;
}













/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_int_group ( file, code, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
INT i ;
{
    RETURN ( dx0_put_code ( file, code ) && dx0_put_int ( file, i ) ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_long_group ( file, code, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
long i ;
{
    RETURN ( dx0_put_code ( file, code ) && dx0_put_long ( file, i ) ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_real_group ( file, code, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
REAL r ;
{
    RETURN ( dx0_put_code ( file, code ) && 
        dx0_put_real ( file, ( code > 200 && code < 1000 ) ? 16 : 0, r ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_dxstring_group ( DXF_FILE file, INT code, DX_STRING *string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    if ((!string || !string->p) && code==1) // DXF_ATTRIB_TEXT contents ?
      {
      RETURN ( dx0_put_code ( file, code ) && dx0_put_string ( file, " ") ) ;
      }
    else
      RETURN ( dx0_put_code ( file, code ) && dx0_put_dxstring ( file, string ) ) ;
}

BBS_PRIVATE BOOLEAN dx0_put_string_group ( DXF_FILE file, INT code, STRING string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
//DXF_FILE file ;
//INT code ;
//STRING string ;
{
    RETURN ( string == NULL || ( dx0_put_code ( file, code ) && 
        dx0_put_string ( file, string ) ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_any_group ( file, code, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
ANY data ;
{
    if ( DX_CODE_STRING(code) ) 
        RETURN ( dx0_put_dxstring_group ( file, code, (DX_STRING *)data ) ) ;
    if ( DX_CODE_REAL(code) ) 
        RETURN ( dx0_put_real_group ( file, code, *((REAL*)data) ) ) ;
#ifdef __ACAD11__
    if ( code == 1071 ) 
        RETURN ( dx0_put_long_group ( file, code, *((long*)data) ) ) ;
#endif
    if ( DX_CODE_INT(code) ) 
        RETURN ( dx0_put_int_group ( file, code, *((INT*)data) ) ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_int_mode ( file, code, mode, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code, mode ;
INT i ;
{
    RETURN ( ( mode == DXF_DEF0_CODE && i == 0 ) || 
             ( mode == DXF_DEF1_CODE && i == 1 ) || 
             ( mode == DXF_DEFNEG1_CODE && i == -1 ) || 
/* NEW_CODE 11-11-91 */
/*             ( DX_CODE_COLOR(code) && i == DXF_ANY_COLOR ) || CNM */
/* NEW_CODE 11-11-91 */
             ( dx0_put_code ( file, code ) && dx0_put_int ( file, i ) ) ) ;
}

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_long_mode ( file, code, mode, i ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code, mode ;
long i ;
{
    RETURN ( ( mode == DXF_DEF0_CODE && i == 0 ) || 
             ( mode == DXF_DEF1_CODE && i == 1 ) || 
             ( mode == DXF_DEFNEG1_CODE && i == -1 ) || 
             ( dx0_put_code ( file, code ) && dx0_put_long ( file, i ) ) ) ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_real_mode ( file, code, mode, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code, mode ;
REAL r ;
{
    RETURN ( ( mode == DXF_DEF0_CODE && IS_SMALL(r) ) || 
             ( mode == DXF_DEF1_CODE && IS_SMALL(r-1.0) ) || 
             ( mode == DXF_DEFNEG1_CODE && IS_SMALL(r+1.0) ) || 
             ( mode == DXF_ARRAY_CODE ) || ( dx0_put_code ( file, code ) && 
                dx0_put_real ( file, 
                    ( code > 200 && code < 1000 ) ? 16 : 0, r ) ) ) ;
}




/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_string_mode ( file, code, mode, string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code, mode ;
STRING string ;
{
    if ( ( code == 5 || code == 1005 ) && !DX_FILE_ENT_HNDL_ENABLED(file) )
        RETURN ( TRUE ) ;
    if ( ( string == NULL || string[0] == '\0') && mode != DXF_NODEF_CODE ) {
        if ( mode == DXF_PUT0_CODE ) 
            RETURN ( dx0_put_string_group ( file, code, "0" ) ) ;
        else if ( mode == DXF_STANDARD_CODE ) 
            RETURN ( dx0_put_string_group ( file, code, "STANDARD" ) ) ;
        else
            RETURN ( TRUE ) ;
    }
    else
        RETURN ( dx0_put_string_group ( file, code, string ) ) ;
}












/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_dxstring_mode ( DXF_FILE file, INT code, INT mode, DX_STRING *string ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{


    if ( ( code == 5 || code == 1005 ) && !DX_FILE_ENT_HNDL_ENABLED(file) )
        RETURN ( TRUE ) ;
    if ( (!string || !string->p || strlen(string->p)==0)  && mode != DXF_NODEF_CODE ) {
        if ( mode == DXF_PUT0_CODE )
          {
          RETURN(dx0_put_string_group ( file, code, "0" )) ;
          }
        else if ( mode == DXF_STANDARD_CODE ) 
          {
          RETURN(dx0_put_string_group ( file, code, "STANDARD" )) ;
          }
        else
          {
          RETURN ( TRUE ) ;
          }
    }
    else
        RETURN ( dx0_put_dxstring_group ( file, code, string ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_any_mode ( file, code, mode, data ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code, mode ;
ANY data ;
{
    if ( DX_CODE_STRING(code) ) 
        RETURN ( dx0_put_dxstring_mode ( file, code, mode, (DX_STRING *)data ) ) ;
    if ( DX_CODE_REAL(code) ) 
        RETURN ( dx0_put_real_mode ( file, code, mode, *((REAL*)data) ) ) ;
#ifdef __ACAD11__
    if ( code == 1071 ) 
        RETURN ( dx0_put_long_mode ( file, code, mode, *((long*)data) ) ) ;
#endif

    if ( DX_CODE_INT(code) && code == 68 )
       {
       if( (*((INT*)data))  )             // mhm change to stop null vals beeing written to views
          RETURN ( dx0_put_int_mode ( file, code, mode, *((INT*)data) ) ) ;
       else
          RETURN ( TRUE );
       }

    if ( DX_CODE_INT(code) && code == 69 )             // mhm change to stop null vals beeing written to views
       {
       if( (*((INT*)data))  )             // mhm change to stop null vals beeing written to views
          RETURN ( dx0_put_int_mode ( file, code, mode, *((INT*)data) ) ) ;
       else
          RETURN ( TRUE );
       }

    if ( DX_CODE_INT(code) ) 
        RETURN ( dx0_put_int_mode ( file, code, mode, *((INT*)data) ) ) ;
    else
        RETURN ( FALSE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_object ( file, object, template, put_6, put_62 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
ANY object ;         
INT* template ;
BOOLEAN put_6, put_62 ;
{ 
    INT *t ;
    STRING data, array ;
    INT i, count=0, mode = DXF_NODEF_CODE ;
    REAL *r ;
#ifdef __ACAD11__
    DML_ITEM item ;
#endif

#ifdef __ACAD11__
    if ( template != NULL && *template == DXF_MULT_CODE ) 
        template += ( template[1] + 2 ) ;
#endif
    for ( t=template, data=(STRING)object ; *t != DXF_END_CODE ; t++ ) {
        /*fprintf(dbgf, "put_object: %3i ",*t);*/
        if ( DX_CODE_MODE(*t) ) {
            if ( *t != DXF_PUT210_CODE ) 
                {
                mode = *t ;
                /*fprintf(dbgf, " mode %i",mode);*/
                }
        }
#ifdef __ACAD11__
        else if ( mode == DXF_LIST_CODE ) {
            if ( *(DML_LIST*)data != NULL ) {
                DML_WALK_LIST ( *(DML_LIST*)data, item ) {
                    if ( !dx0_put_any_mode ( file, *t, 
                        DXF_NODEF_CODE, DML_RECORD(item) ) )
                        RETURN ( FALSE ) ;
                }
            }
            data += dx0_sizeof ( mode ) ;
            /*fprintf(dbgf, "DXF_LISTCODE ");*/
        }
#endif
        else if ( mode == DXF_ARRAY_CODE ) {    /* CNM */
            array = *(STRING*)data ;
            for ( i=0 ; i<count ; i++ ) {
                if ( !dx0_put_any_group ( file, *t, array ) )
                    RETURN ( FALSE ) ;
                array += dx0_sizeof ( *t ) ;
            }
        }
        else {
            if ( mode == DXF_ARRAY_COUNT_CODE ) 
                count = *(INT*)data ;
            if ( *t==210 ) {
                r = (REAL*)data ;
                if ( !IS_SMALL(r[0]) || !IS_SMALL(r[1]) || !IS_SMALL(r[2]) )
                    mode = DXF_NODEF_CODE ;
            }

            /*fprintf(dbgf, "p %3i %3i %i%i d%s",*t, mode, put_6,put_62, data);*/
            if ( ( ( *t != 6 ) || put_6 ) && ( ( *t != 62 ) || put_62 ) 
                && !dx0_put_any_mode ( file, *t, mode, data ) )
/*
            if ( ( *t == 6 ) && !put_6 ) ;
            else if ( ( *t == 62 ) && !put_62 ) ;
            else if ( !dx0_put_any_mode ( file, *t, mode, data ) )
*/
                RETURN ( FALSE ) ;
            data += dx0_sizeof ( *t ) ;
        }
        /*fprintf(dbgf, "\n");*/
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_put_seqend ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    DX_FILE_SET_SEQ_CLOSED(file) ;
    RETURN ( dx0_put_string_group ( file, 0, "SEQEND" )
/*********  CNM  
    &&
        dx0_put_string_group ( file, 8, DX_FILE_ENT_LAYER(file) ) 
*********/        
        ) ;
}

