//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXDESD.H ***********************************/ 
#ifndef     DXDESD 
#define     DXDESD 
typedef     struct  dx_entity_desc {
            DX_STRING   layer ;  
//            char        handle[DX_HANDLE_SIZE] ;
            DX_STRING   handle ;  
            INT         ltype_status ;
            DX_STRING   ltype ;  
            REAL        elevation ;
            REAL        thickness ;
            INT         color_status ;
            INT         color ;
            REAL        extrusion[3] ; 
            } DXF_ENTITY_DESC_S ;
typedef     DXF_ENTITY_DESC_S *DXF_ENTITY_DESC ;
#endif

