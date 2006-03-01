//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/********************************  QGLMEM.H ********************************/
/**************** include file for QGL memory checking *********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     QGLMEM
#define     QGLMEM
#include    <qgldefs.h>

extern  ANY     qgl_malloc __(( INT DUMMY0 )) ;
extern  void    qgl_free __(( ANY DUMMY0 )) ;
extern  ANY     qgl_create __(( INT DUMMY0 )) ;
extern  void    qgl_kill __(( ANY DUMMY0 )) ;
extern  void    qgl_print_malloc __(( void )) ;
extern  void    qgl_print_create __(( void )) ;
#endif /* QGLMEM*/


