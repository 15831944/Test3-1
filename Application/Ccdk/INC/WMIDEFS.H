//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* WMIDEFS.H *********************************/
/*********************** Window manager prototypes *************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef WMIDEFS
#define WMIDEFS

#include <wmdefs.h>

extern  WM_DISPLAY  wmi_init __(( GR_COLOR DUMMY0 )) ;
//extern  BOOLEAN     wmi_init __(( GR_COLOR DUMMY0 )) ;
extern  WM_DISPLAY     wmi_init_gmode __(( int DUMMY0 , GR_COLOR DUMMY1 )) ;
//extern  BOOLEAN     wmi_init_gmode __(( int DUMMY0 , GR_COLOR DUMMY1 )) ;
extern  void        wmi_clear_screen __(( void )) ;
extern  void        wmi_exit __((WM_DISPLAY  wmi)) ;
//extern  void        wmi_exit __(( void )) ;
extern  void        wmi_set_context __((WM_DISPLAY  wmi, HDC hdc)) ;
extern  void        wmi_set_screen_color __(( GR_COLOR DUMMY0 )) ;
extern  GR_COLOR    wmi_get_screen_color __(( void )) ;
extern  BOOLEAN     wmi_get_display_status __(( void )) ;
extern  void        wmi_set_display_all __(( void )) ;
extern  void        wmi_set_display_current __(( void )) ;
extern  VP_VIEWPORT wmi_set_current __(( VP_VIEWPORT DUMMY0 )) ;
extern  VP_VIEWPORT wmi_get_current __(( void )) ;
extern  void        wmi_show __(( void )) ;
extern  void        wmi_fill __(( void )) ;
extern  void        wmi_border __(( void )) ;
extern  void        wmi_unshow __(( void )) ;
extern  BOOLEAN     wmi_close __(( VP_VIEWPORT DUMMY0 )) ;
extern  VP_VIEWPORT wmi_register __(( VP_VIEWPORT DUMMY0 )) ;
extern  VP_VIEWPORT wmi_select __(( PT2 DUMMY0 )) ;
extern  INT         wmi_n_vpts __(( void )) ;
#endif /* WMIDEFS */

