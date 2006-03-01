//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include <string.h>
#include <dmldefs.h>
#include <dx0defs.h>
#include <dxetmpls.h>


/* kenwalt mhm to creaet a new dxf block */

BBS_PUBLIC DXF_BLOCK dxf_create_block_kw (DXF_FILE file,STRING blockname,STRING blck2 ,int flags,REAL *base)
{ 
    INT *block_template ;
    DXF_BLOCK block ;

#ifdef __ACAD11__
    block = dx0_alloc_block ( dx0_file_release ( file ) );
#else
    block = dx0_alloc_block ();
#endif
    if ( block == NULL ) 
        RETURN ( NULL ) ;
#ifdef __ACAD11__
    block_template = ( dx0_file_release ( file ) == DXF_ACAD_RELEASE ) ? 
        DXF_BLOCK_TEMPLATE_11 : DXF_BLOCK_TEMPLATE_10 ;
#else
    block_template = DXF_BLOCK_TEMPLATE ;
#endif

//    (void)strcpy(DXF_BLOCK_NAME(block),blockname);
    DXF_BLOCK_NAME_SET(block,blockname);
    DXF_BLOCK_FLAGS(block) = flags;
    DXF_BLOCK_BASE_PT(block)[0] = base[0];
    DXF_BLOCK_BASE_PT(block)[1] = base[1];
    DXF_BLOCK_BASE_PT(block)[2] = base[2];
    RETURN ( block ) ;
}

