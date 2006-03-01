//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************* W3D.C ***********************************/
/******* Display of 3-dim geometry in the window manager environment *******/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <v3ddefs.h>
#include <v3dpriv.h>
#include <vpidefs.h>
#include <w3ddefs.h>
#include <w3dpriv.h>
#include <wmmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_set_textposition ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
{
    vpi_clip ( WMI_CURR_VIEWPORT ) ;
    v3d_set_textposition ( pt, WMI_CURR_VIEWPORT ) ;
    vpi_unclip ( WMI_CURR_VIEWPORT ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_moveto ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_moveto ( pt, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_moveto ( pt, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_lineto ( pt ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_lineto ( pt, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_lineto ( pt, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_curve ( curve, parm0, parm1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PARM parm0, parm1 ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_curve ( curve, parm0, parm1, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_curve ( curve, parm0, parm1, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_point ( pt, size ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt ;
REAL size ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_point ( pt, size, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_point ( pt, size, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_poly ( a, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 *a  ;
INT n ;
{
    DML_ITEM item ;

    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_poly ( a, n, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_poly ( a, n, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_curve_dir ( curve, dir ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
INT dir ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_curve_dir ( curve, dir, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_curve_dir ( curve, dir, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


#ifdef SPLINE

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void w3d_hpoly ( a, n ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
HPT3 *a  ;
INT n ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_hpoly ( a, n, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_hpoly ( a, n, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}            


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void w3d_ctl_poly ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_ctl_poly ( curve, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_ctl_poly ( curve, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}
#endif /*SPLINE*/

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_curve_box ( curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
{
    w3d_box ( C3_CURVE_BOX(curve) ) ; 
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_box ( box ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_BOX box ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_box ( box, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_box ( box, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC void w3d_arrow ( pt, vec, a, b, h ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
PT3 pt, vec ;
REAL a, b, h ;
{
    DML_ITEM item ;
    if ( WMI_DISPLAY_ALL ) 
        DML_WALK_LIST ( WMI_VIEWPORT_LIST, item ) {
            vpi_clip ( DML_RECORD(item) ) ;
            v3d_arrow ( pt, vec, a, b, h, DML_RECORD(item) ) ;
            vpi_unclip ( DML_RECORD(item) ) ;
        }
    else {
        vpi_clip ( WMI_CURR_VIEWPORT ) ;
        v3d_arrow ( pt, vec, a, b, h, WMI_CURR_VIEWPORT ) ;
        vpi_unclip ( WMI_CURR_VIEWPORT ) ;
    }
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC VP_VIEWPORT w3d_select ( curve, pt, sel_parm, dist_ptr ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                   All rights reserved                    !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_CURVE curve ;
PT3 pt ;
PARM sel_parm ;
REAL *dist_ptr ;
{
    DML_ITEM item ;
    for ( item=DML_LAST(WMI_VIEWPORT_LIST) ; item!=NULL ; 
        item=DML_PREV(item) ) 
        if ( v3d_select ( curve, pt, DML_RECORD(item), sel_parm, dist_ptr ) )
            RETURN ( (VP_VIEWPORT)DML_RECORD(item) ) ;
    RETURN ( NULL ) ;
}



