//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* WM3DEFS.H *********************************/
/********************* 3d Window Management definitions ********************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef WM3DEFS
#define WM3DEFS

#include <c3defs.h>
#include <wmdefs.h>

extern VP_VIEWPORT wm3_top __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;

extern VP_VIEWPORT wm3_front __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;

extern VP_VIEWPORT wm3_right __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;

extern VP_VIEWPORT wm3_isometric __(( REAL DUMMY0 , REAL DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;

extern VP_VIEWPORT wm3_create __(( PT3 DUMMY0 , PT3 DUMMY1 , PT3 DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , REAL DUMMY6 ,
             REAL DUMMY7 , REAL DUMMY8 , GR_COLOR DUMMY9 , GR_COLOR DUMMY10 )) ;



#endif /* WM2DEFS */



