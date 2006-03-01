//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 -x */
/******************************* DX0.C **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxfilm.h>
#include <dxcode.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_sizeof ( code ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT code ;
{ 
    if ( code == 5 ) 
        RETURN ( sizeof (DX_STRING) ) ;    
//        RETURN ( DX_HANDLE_SIZE ) ;
    else if ( code == 1 ) 
        RETURN ( DX_TXT_STRING_SIZE ) ;    
#ifdef __ACAD11__
    else if ( code == 1005 ) 
        RETURN ( sizeof (DX_STRING) ) ;    
//        RETURN ( DX_HANDLE_SIZE ) ;     
    else if ( code == 1004 ) 
        RETURN ( DX_TXT_STRING_SIZE ) ;    
    else if ( code == 1000 ) 
        RETURN ( DX_1000_SIZE ) ;          
    else if ( code == 1071 ) 
        RETURN ( sizeof(long) ) ;
#endif
    else if ( DX_CODE_STRING(code) ) 
        RETURN ( sizeof (DX_STRING) ) ;    
    else if ( DX_CODE_REAL(code) ) 
        RETURN ( sizeof ( REAL) ) ;
    else if ( DX_CODE_INT(code) ) 
        RETURN ( sizeof ( INT) ) ;
    else if ( code == DXF_ANY_CODE ) 
        RETURN ( sizeof ( ANY ) ) ;
    else if ( code == DXF_ENAME_CODE ) 
        RETURN ( sizeof ( DXF_ENAME ) ) ;
#ifdef __ACAD11__
    else if ( code == DXF_LIST_CODE ) 
        RETURN ( sizeof ( DML_LIST ) ) ;
#endif
    else 
        RETURN ( 0 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL *dx0_copy_point ( p0, p1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL p0[3], p1[3] ;
{ 
    if ( p1 != NULL ) {
        if ( p0 == NULL ) {
            p1[0] = p1[1] = p1[2] = 0.0 ;
        }
        else {
            p1[0] = p0[0] ;
            p1[1] = p0[1] ;
            p1[2] = p0[2] ;
        }
    }
    RETURN ( p1 ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_rewind ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    char sentinel[22];     /* Binary DXF file sentinel buffer */

    rewind ( DX_FILE_HANDLE(file) ) ;
    if ( DX_FILE_BINARY(file) ) 
        fread ( sentinel, 1, 22, DX_FILE_HANDLE(file) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE STRING dx0_strcpy ( string1, string2 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING string1, string2 ;
{ 
    RETURN ( strcpy ( string1, string2 == NULL ? "\0" : string2 ) ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE STRING dx0_strass ( DX_STRING* string1, STRING string2 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
//STRING *string1, string2 ;
{ 
    int len;

    if( string2 && (len = strlen(string2)) )
      {
      if( string1 && string1->p )
        {
        if( len+1 <= string1->size )
          {
          strcpy(string1->p,string2);
          }
        else
          {
          dx0_strfree(string1);
          string1->p = MALLOC(len+1,char);
          string1->size = len+1;
          strcpy(string1->p,string2);
          }
        }
      else
        {
        string1->p = MALLOC(len+1,char);
        string1->size = len+1;
        strcpy(string1->p,string2);
        }
      }
    else
      {
      //kga 11/4/01 Code below causes memory leaks! Why is the size set to 0?
      if (string1->p && string1->size==0)
        {
        dx0_strfree(string1); //free old one character null string
        }
      string1->p = MALLOC(1,char);
      string1->p[0] = '\0';
      //string1->p = NULL;
      //string1->p[0] = '\0';
      string1->size = 0;


      /*
      // CNM 12/9/00 I restore this to fix the problem with a block with no name
      // I do NOT know why it had been modified in the first place
      string1->p = MALLOC(1,char);
      string1->p[0] = '\0';
      //string1->p = NULL; //MALLOC(1,char);
      //string1->p[0] = '\0';
      string1->size = 0;
      */
      }
    RETURN (string1->p) ;
}




BBS_PRIVATE void dx0_strfree ( DX_STRING* string)
{ 
if( string && string->p )
  {
  dmm_free(string->p);
  string->p    = NULL;
  string->size = 0;

  }
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_unrec ( file, pos )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
fpos_t pos ;
{ 
    fpos_t *pos1 ;
    if ( file == NULL ) 
        RETURN ;
    if ( DX_FILE_UNREC_LIST(file) == NULL ) 
        DX_FILE_UNREC_LIST(file) = dml_create_list () ;
    pos1 = MALLOC ( 1, fpos_t ) ;
    *pos1 = pos ;
    dml_append_data ( DX_FILE_UNREC_LIST(file), pos1 ) ; 
}


#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT dx0_file_release ( file )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
{ 
    RETURN ( ( file == NULL ) ? DXF_ACAD_RELEASE : DX_FILE_RELEASE(file) ) ;
}




/*

static bad_line = -1;



int
dxf_current_line(FILE *fp)
  {
  int cnt = 0;
  char b[4096];
  fpos_t pos,p;
  if( fgetpos(fp,&pos) )
    {
    return -1;
    }
  rewind(fp);
  while( fgets(b,4096,fp) && !fgetpos(fp,&p) && p < pos )
    {
    cnt++;
    }
  fsetpos(fp,&pos);
  return cnt;
  }
 */


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_FILE_STATUS dx0_set_file_status ( file, s )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_FILE file ;
INT s ;
{ 
    if ( file == NULL ) 
        RETURN ( 0 ) ;
    DX_FILE_STATUS(file) = DX_FILE_STATUS(file) | s ;

    file->status_line = -1;
   // if(( s & DXF_ENTITY_UNKNOWN      ) || ( s & DXF_TABLE_ENTRY_UNKNOWN ) ||
   //    ( s & DXF_MALLOC_FAILURE      ) || ( s & DXF_NO_OBJECTS_FOUND    ) ||
   //    ( s & DXF_PLINENOVERTS        ))
   //      {
   //      file->status_line = dxf_current_line(DX_FILE_HANDLE(file));
   //      }


    RETURN ( DX_FILE_STATUS(file) ) ;
}
#endif

