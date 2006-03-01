//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** DMMDEFS.H ***********************************/
/**************** Include file for memory management component *************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef DMMDEFS
#define DMMDEFS
#include <qgldefs.h>

/****** Declarations of routines *********/
extern  void*   dmm_malloc __(( size_t DUMMY0 ,char *name));
extern  void    dmm_free __(( void* DUMMY0 ));

#endif  /* DMMDEFS */ 

