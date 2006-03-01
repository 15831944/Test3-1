//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXFILD.H ***********************************/ 
#ifndef DXFILD
#define DXFILD
typedef     unsigned    DXF_FILE_STATUS ;
#define     DXF_FILE_INIT               0
#define     DXF_ENTITY_UNKNOWN          1
#define     DXF_TABLE_ENTRY_UNKNOWN     2
#define     DXF_MALLOC_FAILURE          4
#define     DXF_NO_OBJECTS_FOUND        8
#define     DXF_PLINENOVERTS            16 /* mhm addition */
#define     DXF_EOFFOUND                32 /* mhm addition */





typedef     unsigned        DXF_FILE_MODE ;
#define     DXF_ASCII       1
#define     DXF_BINARY      2
#define     DXF_READ        4
#define     DXF_WRITE       8

typedef     struct  dx_file_struct {
            FILE                *fp ;
            DXF_FILE_MODE       file_type ; 
            INT                 file_prec ; 
#ifdef  __ACAD11__
            INT                 file_release ; 
#endif
            DXF_FILE_STATUS     file_status ; 
            fpos_t              file_header_pos ;
            fpos_t              file_tables_pos ;
            fpos_t              file_blocks_pos ;
            fpos_t              file_entities_pos ;
            DML_LIST            file_blockpos_list ;
            DML_LIST            file_unrec_list ;
            unsigned            file_flags ;
            long                file_length ;
            INT                 file_splinesegs ;
  
            DX_STRING           layer ;// mhm
            int                 status_line; // mhm

            } DXF_FILE_S ;
typedef     DXF_FILE_S  *DXF_FILE ;

#endif

