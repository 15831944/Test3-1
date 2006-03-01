//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* GRLDEFS.H *********************************/
/****************** Lowest level Graphics prototypes ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!(C) Copyright 1989, 1990, 1991 Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef GRLDEFS
#define GRLDEFS

#include <grdefs.h>

extern void mhm_lineto(REAL DUMMY0, REAL DUMMY0);
extern void mhm_moveto(REAL DUMMY0, REAL DUMMY0);
            
extern  void grl_moveto __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 )) ;
extern  void grl_lineto __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 )) ;
extern  void grl_line __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 , 
            GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  void grl_rectangle __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
            GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  void grl_fill_rectangle __(( GR_PIX_COORD DUMMY0 , 
            GR_PIX_COORD DUMMY1 , GR_PIX_COORD DUMMY2 , 
            GR_PIX_COORD DUMMY3 )) ;
extern  void grl_outtext __(( STRING DUMMY0 )) ;
extern  void grl_outtext_xy __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
            STRING DUMMY2 )) ;
extern  BOOLEAN grl_set_videomode __(( INT DUMMY0 )) ;
extern  INT     grl_videomode __(( void )) ;
extern  void    grl_exit_videomode __(( void )) ;
extern  void    grl_set_color __(( GR_COLOR DUMMY0, INT DUMMY1 )) ;
extern  void    grl_set_thickness __(( REAL THICKNESS )) ;
extern  COLORREF  grl_get_win_color ( GR_COLOR color ) ; //KGA 22/1/99
extern  void    grl_set_win_color ( GR_COLOR color, COLORREF colorref ) ; //CNM 08/07/1/99
extern  GR_COLOR   grl_get_color __(( void )) ;
extern  void    grl_set_textposition __(( GR_PIX_COORD DUMMY0 , 
            GR_PIX_COORD DUMMY1 )) ;
extern  void    grl_clip __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
            GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  void    grl_unclip __(( void )) ;
extern  void    grl_set_patt __(( GR_PATT DUMMY0 )) ;
extern  void    grl_set_solid_patt __(( void )) ;
extern  GR_PATT    grl_get_patt __(( void )) ;
extern  void    grl_clear_screen __(( void )) ;
extern  GR_PIX_COORD grl_get_max_x __(( void )) ;
extern  GR_PIX_COORD grl_get_max_y __(( void )) ;
extern  GR_PIX_COORD grl_get_min_x __(( void )) ;
extern  GR_PIX_COORD grl_get_min_y __(( void )) ;
extern  GR_PIX_COORD grl_get_numtextcols __(( void )) ;
extern  GR_PIX_COORD grl_get_numtextrows __(( void )) ;
extern  void    grl_floodfill __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
            GR_COLOR DUMMY2 )) ;
extern  void    grl_putimage __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
            ANY DUMMY2 )) ;
extern  ANY     grl_getimage __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
             GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  REAL    grl_get_screen_ratio __(( void )) ;
extern  void    grl_set_screen_ratio __(( REAL DUMMY0 )) ;

#endif /* GRLDEFS */

