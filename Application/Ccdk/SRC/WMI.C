//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 */
/********************************* WMI.C ***********************************/
/******************************* Window Manager ****************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#include <dmldefs.h>
#include <grrdefs.h>
#include <vpidefs.h>
#include <wmidefs.h>
#include <wmmcrs.h>

EXTERN  HDC   win_HDC;
GLOBAL  WM_DISPLAY  WM_INFO=NULL ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC WM_DISPLAY  /*BOOLEAN cnm*/ wmi_init ( screen_color )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_COLOR screen_color ;
{
    RETURN ( wmi_init_gmode ( GR_DETECT, screen_color ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC WM_DISPLAY  /*BOOLEAN cnm*/ wmi_init_gmode ( gmode, screen_color )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int gmode ;
GR_COLOR screen_color ;
{
    if ( WM_INFO==NULL ) 
        WM_INFO = MALLOC ( 1, WM_DISPLAY_S ) ;

    if ( WM_INFO != NULL ) {
        WMI_VIEWPORT_LIST = dml_create_list();
        WMI_SCREEN_COLOR = screen_color ;
        WMI_DISPLAY_ALL = TRUE ;
    }

    if (WM_INFO && grr_init ( gmode, screen_color, 1) ) 
      RETURN ( WM_INFO);
    else
      RETURN NULL;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_clear_screen __(( void )) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    grr_clear_screen ( WMI_SCREEN_COLOR ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_exit __((WM_DISPLAY  wmi))  /*cnm*/
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    WM_INFO = wmi;
    if ( WM_INFO!=NULL ) 
        dml_destroy_list ( WMI_VIEWPORT_LIST, vpi_free ) ;
    FREE ( WM_INFO ) ;
    WM_INFO = NULL ;
    grr_exit ();
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_set_context (wmi, hdc)  /*cnm*/
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
WM_DISPLAY  wmi;
HDC hdc;
{
    WM_INFO = wmi;
    win_HDC = hdc;
    grr_init(GR_DETECT, 0, 0);
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_set_screen_color ( color ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_COLOR color ;
{
    WMI_SCREEN_COLOR = color ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC GR_COLOR wmi_get_screen_color () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    RETURN ( WMI_SCREEN_COLOR ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN wmi_get_display_status () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    RETURN ( WMI_DISPLAY_ALL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_set_display_all () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    WMI_DISPLAY_ALL = TRUE ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_set_display_current () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    WMI_DISPLAY_ALL = FALSE ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wmi_set_current ( viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT viewport ;
{
    DML_ITEM item ;
    item = dml_find_data ( WMI_VIEWPORT_LIST, viewport ) ;
    if ( item == NULL ) 
        RETURN ( NULL ) ;
    dml_make_last ( WMI_VIEWPORT_LIST, item ) ;
    RETURN ( viewport ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wmi_get_current ( ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    RETURN ( WMI_CURR_VIEWPORT ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_show ( ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    if ( WMI_DISPLAY_ALL )
        dml_apply ( WMI_VIEWPORT_LIST, vpi_show ) ;
    else
        vpi_show ( WMI_CURR_VIEWPORT ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_fill ( ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    if ( WMI_DISPLAY_ALL )
        dml_apply ( WMI_VIEWPORT_LIST, vpi_fill ) ;
    else
        vpi_fill ( WMI_CURR_VIEWPORT ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_border ( ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    if ( WMI_DISPLAY_ALL )
        dml_apply ( WMI_VIEWPORT_LIST, vpi_border ) ;
    else
        vpi_border ( WMI_CURR_VIEWPORT ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void wmi_unshow () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    DML_ITEM item ;

    if ( WMI_DISPLAY_ALL )
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) 
            vpi_unshow ( DML_RECORD(item), WMI_SCREEN_COLOR ) ;
    else
        vpi_unshow ( WMI_CURR_VIEWPORT, WMI_SCREEN_COLOR ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC BOOLEAN wmi_close ( viewport ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT viewport ;
{
    if ( viewport == NULL ) 
        wmi_close ( WMI_CURR_VIEWPORT ) ;
    RETURN ( dml_kill_data ( WMI_VIEWPORT_LIST, viewport, vpi_free ) ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wmi_register ( viewport )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
VP_VIEWPORT viewport ;
{
    dml_append_data ( WMI_VIEWPORT_LIST, viewport ) ;
    RETURN ( viewport ) ;
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT wmi_select ( p ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT2 p ;
{
    DML_ITEM item ;
    DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) 
        if ( vpi_pt_inside ( DML_RECORD(item), p ) )
            RETURN ( (VP_VIEWPORT)DML_RECORD(item) ) ;
    RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC INT wmi_n_vpts __(( void )) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    RETURN ( dml_length ( WMI_VIEWPORT_LIST ) ) ;
}

