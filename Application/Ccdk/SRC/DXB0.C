//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 -x */
/******************************* DXB0.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dx0defs.h>
#include <dxetmpls.h>

#ifdef  __ACAD11__
INT DXF_BLOCK_TEMPLATE_10[] = { DX_BLOCK_TEMPLATE_10 } ;
INT DXF_BLOCK_TEMPLATE_11[] = { DX_BLOCK_TEMPLATE_11 } ;
#else
INT DXF_BLOCK_TEMPLATE[]    = { DX_BLOCK_TEMPLATE } ;
#endif

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_BLOCK dx0_alloc_block ( release )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT release ;
{ 
    DXF_BLOCK block = DX_CREATE_BLOCK ;
    if ( block == NULL )
        RETURN ( NULL ) ;
    DXF_ENAME_INIT ( DXF_BLOCK_ENAME(block) ) ;
    dx0_init_template ( (char *)block + sizeof(DXF_ENAME), ( release == 10 ) ? 
        DXF_BLOCK_TEMPLATE_10 : DXF_BLOCK_TEMPLATE_11 ) ;
    RETURN ( block ) ;
}
#else

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_BLOCK dx0_alloc_block ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
    DXF_BLOCK block = DX_CREATE_BLOCK ;
    if ( block == NULL )
        RETURN ( NULL ) ;
    DXF_ENAME_INIT ( DXF_BLOCK_ENAME(block) ) ;
    dx0_init_template ( (char *)block + sizeof(DXF_ENAME), DXF_BLOCK_TEMPLATE ) ;
    RETURN ( block ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_free_block ( block ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_BLOCK block ;
{ 
    dx0_strfree(DXF_BLOCK_XREF_PATHNAME_PTR(block));
    dx0_strfree(DXF_BLOCK_LAYER_PTR(block));
    dx0_strfree(DXF_BLOCK_HANDLE_PTR(block));
    dx0_strfree(DXF_BLOCK_LTYPE_PTR(block)); //kga 10/4/2001
    dx0_strfree(DXF_BLOCK_NAME_PTR(block));
    dx0_strfree(DXF_BLOCK_NAME3_PTR(block));
    DX_FREE_BLOCK ( block ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_BLOCKPOS dx0_alloc_blockpos ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
    DXF_BLOCKPOS blockpos = DX_CREATE_BLOCKPOS ;
    if ( blockpos == NULL )
        RETURN ( NULL ) ;
    
    DX_BLOCKPOS_NAME_SET(blockpos, NULL );
//    dx0_strcpy ( DX_BLOCKPOS_NAME_GET(blockpos), NULL ) ;
    DX_BLOCKPOS_NAME_SET(blockpos, NULL ) ;
    RETURN ( blockpos ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_free_blockpos ( blockpos ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_BLOCKPOS blockpos ;
{ 

    dx0_strfree(DX_BLOCKPOS_NAME_PTR( blockpos ));
    DX_FREE_BLOCKPOS ( blockpos ) ;
}

