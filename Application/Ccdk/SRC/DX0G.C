//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 -x */
/******************************* DX0G.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#ifndef __NDP__
#ifndef __UNIX__
#include <stdlib.h>
#endif
#endif
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxcode.h>
#include <dxpos.h>


void StripCode(STRING string, INT *Len) // CNM
  {
  // if string starts %% then assume it is a code (formatting) 
  // and strip it out

  if (*Len>3 && string[0]=='%' && string[1]=='%')
    {
    memmove(&string[0], &string[3], *Len+1); // Include NULL
    *Len-=3;
    }
  }

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_string ( file, string )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
STRING string ;
{ 
    INT i, n ;
    DX_BUFFER buffer ;

    if ( string==NULL ) 
        string = buffer ;

    if ( DX_FILE_ASCII(file) ) {
		{
		//int   a  = DX_BUFFER_SIZE;
		//char *p  = string;
		//FILE *fp = DX_FILE_HANDLE(file);
		//fpos_t      aaa;

		//if( fgetpos(fp,&aaa))
        //    RETURN ( FALSE ) ;
        //(void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
        if ( fgets ( string, DX_BUFFER_SIZE, DX_FILE_HANDLE(file) ) == NULL )
            RETURN ( FALSE ) ; // mhm change - EOF condition is found
		}
        for ( i=0 ; i<DX_BUFFER_SIZE ; i++ ) {
            if ( string[i] == '\r' || string[i] == '\n' 
                || string[i] == '\0' ) {
                string[i] = '\0' ; 
                StripCode(string, &i); // CNM
                RETURN ( TRUE ) ;
              
            }
        }
        RETURN ( FALSE ) ;
    }
    else if ( DX_FILE_BINARY(file) ) {
        for ( i=0 ; i<DX_BUFFER_SIZE ; i++ ) {
            n = fread ( string+i, 1, 1, DX_FILE_HANDLE(file) ) ;
            if ( n <= 0 ) 
                RETURN ( FALSE ) ;
            if ( string[i] == '\r' || string[i] == '\n' 
                || string[i] == '\0' ) {
                string[i] = '\0' ; 
                StripCode(string, &i); // CNM
                RETURN ( TRUE ) ;
            }
        }
        RETURN ( FALSE ) ;
    }
    else 
        RETURN ( FALSE ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_dxstring ( file, string )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
DX_STRING* string ;
{ 
    INT i;
    DX_BUFFER buffer;
    i = 0;


    //if ( string==NULL ) 
    //    string = buffer ;

    if ( DX_FILE_ASCII(file) ) 
      {
      if ( fgets ( buffer, DX_BUFFER_SIZE, DX_FILE_HANDLE(file) ) == NULL )
        {
        RETURN ( FALSE ) ;
    	  }
      for ( i=0 ; i<DX_BUFFER_SIZE ; i++ ) 
        {
        if ( buffer[i] == '\r' || buffer[i] == '\n' || buffer[i] == '\0' ) 
          {
          buffer[i] = '\0' ; 
          break;
          }
        }
      StripCode(buffer, &i); // CNM
      if( i == DX_BUFFER_SIZE )
          {
          dx0_strass(string,NULL);
          RETURN ( FALSE ) ;
          }
        dx0_strass(string,buffer);
        RETURN ( TRUE ) ;
      }
    else
      { 
      StripCode(buffer, &i); // CNM
      dx0_strass(string,buffer);
      RETURN ( FALSE ) ;
      }
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_get_code ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{
    INT code = DXF_BAD_CODE ;
    STRING endptr ; 
    DX_BUFFER string ;
    unsigned char c ;                          

    if ( DX_FILE_ASCII(file) ) {
        //(void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
        if ( fgets ( string, DX_BUFFER_SIZE, DX_FILE_HANDLE(file) ) == NULL )
            RETURN ( DXF_BAD_CODE ) ;
        code = (INT) strtol ( string, &endptr, 10 ) ;
    }

    else if ( DX_FILE_BINARY(file) ) {
        if ( !fread ( &c, 1, 1, DX_FILE_HANDLE(file) ) ) 
            RETURN ( DXF_BAD_CODE ) ;
        code = (INT) c ;
    }
    RETURN ( code ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_int ( file, int_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT *int_ptr ; 
{
    STRING endptr ; 
    DX_BUFFER string ;

    if ( DX_FILE_ASCII(file) ) {
        //(void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
        if ( fgets ( string, DX_BUFFER_SIZE, DX_FILE_HANDLE(file) ) == NULL )
            RETURN ( FALSE ) ;
        if ( int_ptr != NULL ) 
            *int_ptr = (INT) strtol ( string, &endptr, 10 ) ;
    }

    else if ( DX_FILE_BINARY(file) ) {
        if ( int_ptr == NULL ) 
            int_ptr = (INT*)string ;
        if ( !fread ( int_ptr, 2, 1, DX_FILE_HANDLE(file) ) ) 
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_real ( file, real_ptr )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
REAL *real_ptr ;
{
    STRING endptr ; 
    DX_BUFFER string ;

    if ( DX_FILE_ASCII(file) ) {
        //(void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
        if ( fgets ( string, DX_BUFFER_SIZE, DX_FILE_HANDLE(file) ) == NULL )
            RETURN ( FALSE ) ;
        if ( real_ptr != NULL ) 
            *real_ptr = (REAL) strtod ( string, &endptr ) ;
    }

    else if ( DX_FILE_BINARY(file) ) {
        if ( real_ptr == NULL ) 
            real_ptr = (REAL*)string ;
        if ( !fread ( real_ptr, 8, 1, DX_FILE_HANDLE(file) ) )
            RETURN ( FALSE ) ;
    }
    RETURN ( TRUE ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_void ( file, code ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
{ 
    INT i ;
    REAL r ;
    DX_BUFFER string ;

    if ( DX_CODE_STRING(code) ) 
        RETURN ( dx0_get_string ( file, string ) ) ;
    else if ( DX_CODE_INT(code) ) 
        RETURN ( dx0_get_int ( file, &i ) ) ;
    else if ( DX_CODE_REAL(code) ) 
        RETURN ( dx0_get_real ( file, &r ) ) ;
    else if ( DX_FILE_BINARY(file) ) 
        RETURN ( FALSE ) ;
    else 
        RETURN ( dx0_get_string ( file, string ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_get_any ( file, code, r ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT code ;
ANY r ;
{ 
    if ( r==NULL ) 
        RETURN ( dx0_get_void ( file, code ) ) ;
    else if ( DX_CODE_STRING(code) ) 
        RETURN ( dx0_get_dxstring ( file, (DX_STRING *)r ) ) ;
    else if ( DX_CODE_INT(code) ) 
        RETURN ( dx0_get_int ( file, (INT*)r ) ) ;
    else if ( DX_CODE_REAL(code) ) 
        RETURN ( dx0_get_real ( file, (REAL*)r ) ) ;
    else 
        RETURN ( dx0_get_void ( file, code ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_get_object ( file, object, template, mod_6, mod_62 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
ANY object ;         
INT* template ;
BOOLEAN *mod_6, *mod_62 ;
{ 
    INT *t, code, array_count=0 ;
    fpos_t pos ;
    STRING data, array=NULL ;
    INT mode = DXF_NODEF_CODE, mult = 0, i, j, m ;
    BOOLEAN end_loop ;
    INT *mult_code = NULL, *mult_count = NULL ;
#ifdef __ACAD11__
// mhm    DX_STRING buffer ;
    DX_BUFFER buffer ;
//    DX_STRING *strin;
#endif

    if ( mod_6 != NULL ) 
        *mod_6 = FALSE ;
    if ( mod_62 != NULL ) 
        *mod_62 = FALSE ;

#ifdef __ACAD11__
    if ( template != NULL && *template == DXF_MULT_CODE ) {
        mult = template[1] ;
        mult_code = template + 2 ;
        mult_count = CREATE ( mult, INT ) ;
        for ( i = 0 ; i < mult ; i++ ) 
            mult_count[i] = 0 ;
        template += ( mult+2 ) ;
    }
#endif
    while ( TRUE ) {

        if ( !dx0_getpos ( file, &pos ) ) {
            KILL ( mult_count ) ;
            //(void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
            RETURN ( DXF_BAD_CODE ) ;
        }
        code = dx0_get_code ( file ) ;
        if ( code == 6 && mod_6 != NULL ) 
            *mod_6 = TRUE ;
        if ( code == 62 && mod_62 != NULL ) 
            *mod_62 = TRUE ;

        if ( DX_CODE_NEW_ITEM(code) ) {
            KILL ( mult_count ) ;
            if( dx0_setpos ( file, &pos ) )
              {
              RETURN(code);
              }
            else
              {
//              (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
              RETURN(DXF_BAD_CODE);
              }
              // mhm change RETURN ( dx0_setpos ( file, &pos ) ? code : DXF_BAD_CODE ) ;
        }

        m = 0 ;
        j = -1 ;
        if ( mult_code != NULL ) {
            for ( i = 0 ; i < mult && j == -1 ; i++ ) {
                if ( mult_code[i] == code ) {
                    j = i ;
                    m = mult_count[i] ;
                }
            }
        }

        end_loop = FALSE ;
        i = 0 ;
#ifdef __ACAD11__
        if ( mode == DXF_LIST_CODE ) 
            mode = DXF_NODEF_CODE ;
#endif
        for ( t = template, data = (STRING)object ; !end_loop ; t++ ) {
            if ( ( t == NULL ) || ( *t == code ) ) {
                if ( mode == DXF_NODEF_CODE || mode == DXF_DEF0_CODE || 
                    mode == DXF_PUT0_CODE || mode == DXF_STANDARD_CODE ||
                    mode == DXF_DEF1_CODE ) {
                    if ( i == m ) {
                        if ( !dx0_get_any ( file, code, data ) ) {
                            KILL ( mult_count ) ;
//                            (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                            RETURN ( DXF_BAD_CODE ) ;
                        }

                        end_loop = TRUE ;
                        if ( j >= 0 ) 
                            (mult_count[j])++ ;
                    }
                    else {
                        data += dx0_sizeof ( *t ) ;
                        i++ ;
                    }
                }
                else if ( mode == DXF_ARRAY_COUNT_CODE ) { 
                    if ( !dx0_get_any ( file, code, data ) ) {
                        KILL ( mult_count ) ;
//                        (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                        RETURN ( DXF_BAD_CODE ) ;
                    }
                    array_count = *(INT*)data ;
                    end_loop = TRUE ;
                }
                else if ( mode == DXF_ARRAY_CODE ) { 
                    if ( array == NULL ) {
                        array = MALLOC ( array_count * dx0_sizeof(*t), char ) ;
                        if ( array == NULL ) {
                            KILL ( mult_count ) ;
//                            (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                            RETURN ( DXF_BAD_CODE ) ;
                        }
                        *(STRING*)data = array ;
                    }
                    if ( !dx0_get_any ( file, code, array ) ) {
                        KILL ( mult_count ) ;
//                        (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                        RETURN ( DXF_BAD_CODE ) ;
                    }
                    array += dx0_sizeof ( *t ) ;
                    end_loop = TRUE ;
                }
#ifdef __ACAD11__
                else if ( mode == DXF_LIST_CODE ) { 
                    if ( *(DML_LIST*)data == NULL ) {
                        *(DML_LIST*)data = dml_create_list () ;
                        if ( *(DML_LIST*)data == NULL ) {
                            KILL ( mult_count ) ;
//                            (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                            RETURN ( DXF_BAD_CODE ) ;
                        }
                    }
                    if ( !dx0_get_any ( file, code, buffer ) ) {
                        KILL ( mult_count ) ;
//                        (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                        RETURN ( DXF_BAD_CODE ) ;
                    }
                    dml_append_data ( *(DML_LIST*)data, strdup(buffer) ) ;
                    end_loop = TRUE ;
                }
#endif
            }

            else if ( *t == DXF_END_CODE ) {
                if ( !dx0_get_any ( file, code, NULL ) ) {
                    KILL ( mult_count ) ;
//                    (void)fgetpos(DX_FILE_HANDLE(file),&file->file_last_seek);
                    RETURN ( DXF_BAD_CODE ) ;
                }
                end_loop = TRUE ;
            }
#ifdef __ACAD11__
            else if ( *t == DXF_ARRAY_COUNT_CODE || *t == DXF_ARRAY_CODE ||
                *t == DXF_DEF0_CODE || *t == DXF_DEF1_CODE || 
                *t == DXF_PUT0_CODE || *t == DXF_STANDARD_CODE || 
                *t == DXF_NODEF_CODE || *t == DXF_LIST_CODE ) {
                if ( mode == DXF_LIST_CODE && *t != DXF_LIST_CODE ) 
                    data += dx0_sizeof ( mode ) ;
                mode = *t ;
            }
            else if ( mode != DXF_ARRAY_CODE && mode != DXF_LIST_CODE ) 
                data += dx0_sizeof ( *t ) ;
#else
            else if ( *t == DXF_ARRAY_COUNT_CODE || *t == DXF_ARRAY_CODE ||
                *t == DXF_DEF0_CODE || *t == DXF_DEF1_CODE || 
                *t == DXF_PUT0_CODE || *t == DXF_STANDARD_CODE || 
                *t == DXF_NODEF_CODE ) {
                mode = *t ;
            }
            else if ( mode != DXF_ARRAY_CODE ) 
                data += dx0_sizeof ( *t ) ;
#endif
        }
    }
#ifndef __UNIX__ 
    KILL ( mult_count ) ;
    RETURN ( code ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_find ( file, code, string, curr_pos )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
INT code ;
STRING string ;
fpos_t *curr_pos ;
{ 
    INT i, curr_code ;
    DX_BUFFER curr_string ;
    REAL r ;
    fpos_t pos0 ;

    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;

    while ( TRUE ) {
        if ( curr_pos != NULL && !dx0_getpos ( file, curr_pos ) )
            RETURN ( FALSE ) ;
        curr_code = dx0_get_code ( file ) ; 

        if ( curr_code < 0 ) {
            (void)dx0_setpos ( file, &pos0 ) ;
            RETURN ( FALSE ) ;
        }
        if ( DX_CODE_STRING(curr_code) ) {
            if ( string == NULL && curr_code == code ) 
                RETURN ( TRUE ) ;
            else { 
                dx0_get_string ( file, curr_string ) ;
                if ( curr_code == code && !strcmp ( string, curr_string ) ) 
                    RETURN ( TRUE ) ;
                if ( ( DX_CODE_NEW_ITEM(curr_code) ) && 
                    !strcmp ( "EOF", curr_string ) ) {
                    file->file_status |= DXF_EOFFOUND;// mhm addition
                    (void)dx0_setpos ( file, &pos0 ) ;
                    RETURN ( FALSE ) ;
                }
            }
        }
        else if ( DX_CODE_REAL(curr_code) ) {
            dx0_get_real ( file, &r ) ;
            if ( curr_code == code ) 
                RETURN ( TRUE ) ;
        }
        else if ( DX_CODE_INT(curr_code) ) {
            dx0_get_int ( file, &i ) ;
            if ( curr_code == code ) 
                RETURN ( TRUE ) ;
        }
/*  To be able to read data of unknown type. Should not work in binary */
        else { 
            if ( DX_FILE_BINARY(file) ) 
                RETURN ( FALSE ) ;
            dx0_get_string ( file, curr_string ) ;
            if ( curr_code == code ) 
                RETURN ( TRUE ) ;
        }
    }
#ifndef __UNIX__ 
   (void)dx0_setpos ( file, &pos0 ) ;
    RETURN ( FALSE ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN dx0_find_after ( file, code0, string0, code, string, 
            curr_pos )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
INT code0 ;
STRING string0 ;
INT code ;
STRING string ;
fpos_t *curr_pos ;
{ 
    INT i, curr_code ;
    DX_BUFFER curr_string ;
    REAL r ;
    fpos_t pos0 ;

    BOOLEAN status ;

    status = FALSE ;
    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;

    while ( TRUE ) {
        if ( curr_pos != NULL && !dx0_getpos ( file, curr_pos ) )
            RETURN ( FALSE ) ;
        curr_code = dx0_get_code ( file ) ; 

        if ( curr_code < 0 ) {
            (void)dx0_setpos ( file, &pos0 ) ;
            RETURN ( FALSE ) ;
        }

        if ( DX_CODE_STRING(curr_code) ) {
            dx0_get_string ( file, curr_string ) ;
            if ( string == NULL && curr_code == code ) {
                if ( status ) 
                    RETURN ( TRUE ) ;
            }
            else { 
                if ( curr_code == code && !strcmp ( string, curr_string ) ) {
                    if ( status )
                        RETURN ( TRUE ) ;
                }
            }

            if ( string0 == NULL ) 
                status = ( curr_code == code0 ) ;
            else { 
                status = ( ( curr_code == code0 ) && 
                    !strcmp ( string0, curr_string ) ) ;
                if ( ( DX_CODE_NEW_ITEM(curr_code) ) && 
                    !strcmp ( "EOF", curr_string ) ) {
                    file->file_status |= DXF_EOFFOUND;// mhm addition
                    (void)dx0_setpos ( file, &pos0 ) ;
                    RETURN ( FALSE ) ;
                }
            }
        }

        else if ( DX_CODE_REAL(curr_code) ) {
            dx0_get_real ( file, &r ) ;
            if ( curr_code == code ) {
                if ( status ) 
                    RETURN ( TRUE ) ;
            status = ( curr_code == code0 ) ;
            }
        }

        else if ( DX_CODE_INT(curr_code) ) {
            dx0_get_int ( file, &i ) ;
            if ( curr_code == code ) {
                if ( status ) 
                    RETURN ( TRUE ) ;
            }
            status = ( curr_code == code0 ) ;
        }
/* To be able to read data of unknown type. Does not work in binary */
        else { 
            if ( DX_FILE_BINARY(file) ) 
                RETURN ( FALSE ) ;
            dx0_get_string ( file, curr_string ) ;
            if ( curr_code == code ) {
                if ( status ) 
                    RETURN ( TRUE ) ;
            }
            status = ( curr_code == code0 ) ;
        }
    }
#ifndef __UNIX__ 
   (void)dx0_setpos ( file, &pos0 ) ;
    RETURN ( FALSE ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_count ( file, code, string )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ; 
INT code ;
STRING string ;
{ 
    INT count ;
    fpos_t pos0 ;

    if ( !dx0_getpos ( file, &pos0 ) )
        RETURN ( FALSE ) ;

    count = 0 ;
    while ( TRUE ) {
        if ( dx0_find ( file, code, string, NULL ) )
            count++ ;
        else {
            if ( dx0_setpos ( file, &pos0 ) )
                RETURN ( count ) ;
            else
                RETURN ( -1 ) ;
        }
    }
#ifndef __UNIX__
    if ( dx0_setpos ( file, &pos0 ) )
        RETURN ( count ) ;
    else
        RETURN ( -1 ) ;
#endif
}

