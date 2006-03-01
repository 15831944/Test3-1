//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* QGLGRAPH.H ********************************/
/************************* Display macros **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*************** These macros emulate C Graphics routines ******************/

#ifndef QGLGRAPH
#define QGLGRAPH

#ifdef  _CCDK_WIN
  #ifndef  __WINDOWS_H
    #include <windows.h>

    #include "Scd_SHLB.h"

  #endif
#else
#ifdef  __MCRGRAPH__
/*#include <graph.h> */
#endif
#ifdef  __TURBOC__
/*#include <graphics.h>*/
#endif
#endif
#ifdef  __GFX__
#ifdef  PI
#define QGL_PI  PI
#undef  PI
#endif
/*#include <gfx.h>*/
#ifndef __HIGHC24__
/*#include <gfxg_pro.h>*/
#endif
#ifdef  PI
#undef  PI
#define PI  QGL_PI
#endif
#endif  /*GFX*/

#endif  /*QGLGRAPH*/

