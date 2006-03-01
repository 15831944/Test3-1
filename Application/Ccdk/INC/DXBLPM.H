//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/****************************** DXBLPM.H ***********************************/ 
#ifndef     DXBLPM
#define     DXBLPM 
#include    "tststr.h"


#define     DX_BLOCKPOS_NAME_PTR(B)         (&((B)->block_name ))
#define     DX_BLOCKPOS_NAME_GET(B)         STRTEST( (B)->block_name.p )
#define     DX_BLOCKPOS_NAME_SET(B,value)   ( dx0_strass(&(B)->block_name,value) )



#define     DX_BLOCKPOS_FPOS(B)         ((B)->block_position)
#define     DX_CREATE_BLOCKPOS          (MALLOC(1,DXF_BLOCKPOS_S))
#define     DX_FREE_BLOCKPOS(B)         FREE(B)
#endif

