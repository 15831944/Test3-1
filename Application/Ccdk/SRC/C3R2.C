//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -3 */
/********************************** C3G2.C *********************************/
/******************** Routines for processing arcs ************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2rdefs.h>
#include <c3rdefs.h>
#include <c3vdefs.h>
#include <c3rmcrs.h>
#include <c2rmcrs.h>
#include <c3vmcrs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_ASEG c3r_conv_3d2d ( arc_3d, c, u, v, arc_2d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C3_ASEG arc_3d ;
PT3 c, u, v ;
C2_ASEG arc_2d ;
{
    PT3 n1, n2 ;

    if ( !c3v_basis_orthogonal ( u, v ) )
        RETURN ( NULL ) ;
    c3v_conv_pt_3d2d ( C3_ASEG_PT0(arc_3d), c, u, v, C3_ASEG_PT0(arc_2d) ) ;
    c3v_conv_pt_3d2d ( C3_ASEG_PT1(arc_3d), c, u, v, C3_ASEG_PT1(arc_2d) ) ;
    C2_ASEG_D(arc_2d) = c3r_d_parm ( arc_3d ) ;
    C3V_CROSS ( u, v, n1 ) ; 

    // mhmchange - guess
    // this crashes id c3r_normal returns NULL
    //old code  if ( C3V_DOT ( n1, c3r_normal ( arc_3d, n2 ) ) < 0.0 ) 
    //old code      C2_ASEG_D(arc_2d) = - C2_ASEG_D(arc_2d) ;

    if( c3r_normal ( arc_3d, n2 ) )
      {
      if ( C3V_DOT ( n1, c3r_normal ( arc_3d, n2 ) ) < 0.0 ) 
          C2_ASEG_D(arc_2d) = - C2_ASEG_D(arc_2d) ;
      }
    RETURN ( arc_2d ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C3_ASEG c3r_conv_2d3d ( arc_2d, c, u, v, arc_3d ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
C2_ASEG arc_2d ;
PT3 c, u, v ;
C3_ASEG arc_3d ;
{
    PT2 d_vec ;

    if ( !c3v_basis_orthogonal ( u, v ) )
        RETURN ( NULL ) ;
    c3v_conv_pt_2d3d ( C2_ASEG_PT0(arc_2d), c, u, v, C2_ASEG_PT0(arc_3d) );
    c3v_conv_pt_2d3d ( C2_ASEG_PT1(arc_2d), c, u, v, C2_ASEG_PT1(arc_3d) );
    c3v_conv_vec_2d3d ( c2r_d_vec ( arc_2d, d_vec ), u, v, 
        C3_ASEG_D_VEC(arc_3d) ) ;
    RETURN ( arc_3d ) ;
}

