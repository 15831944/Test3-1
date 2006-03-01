//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************* WM3.C ***********************************/
/***************** 3d windows in the window manager environment ************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>
#include <vp3defs.h>
#include <wm3defs.h>
#include <wmmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm3_top ( x, y, z, w, h, px0, py0, px1, py1, 
            bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    VP_VIEWPORT viewport ; 
    viewport = vp3_top ( x, y, z, w, h, px0, py0, px1, py1, 
        bd_color, bk_color ) ;
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm3_front ( x, y, z, w, h, px0, py0, px1, py1, 
            bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    VP_VIEWPORT viewport ; 
    viewport = vp3_front ( x, y, z, w, h, px0, py0, px1, py1, 
        bd_color, bk_color ) ;
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm3_right ( x, y, z, w, h, px0, py0, px1, py1, 
            bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    VP_VIEWPORT viewport ; 
    viewport = vp3_right ( x, y, z, w, h, px0, py0, px1, py1, 
        bd_color, bk_color ) ;
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm3_create ( view_ctr_pt, eye_pt, view_up, w, h, 
    px0, py0, px1, py1, bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 view_ctr_pt, eye_pt, view_up ;
REAL w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    VP_VIEWPORT viewport ; 
    viewport = vp3_create ( view_ctr_pt, eye_pt, view_up, w, h, 
    px0, py0, px1, py1, bd_color, bk_color ) ;
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wm3_isometric ( x, y, z, w, h, px0, py0, px1, py1, 
    bd_color, bk_color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL x, y, z, w, h, px0, py0, px1, py1 ;
GR_COLOR bd_color, bk_color ;
{
    VP_VIEWPORT viewport ; 
    viewport = vp3_isometric ( x, y, z, w, h, px0, py0, px1, py1, 
            bd_color, bk_color ) ;
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}

