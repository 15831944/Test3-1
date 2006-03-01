//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/***************************** C3BDEFS.H ***********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifndef C3BDEFS
#define C3BDEFS
#include <c3defs.h>
#include <albdefs.h>

#define C3B_BRK0(A,D,T,B)   alb_brk0((REAL*)A,D,4,T,(REAL*)B)
#define C3B_BRKL(A,D,T,B)   alb_brkl((REAL*)A,D,4,T,(REAL*)B)
#define C3B_BRKR(A,D,T,B)   alb_brkr((REAL*)A,D,4,T,(REAL*)B)
#define C3B_SBDV0(A,D,B)    alb_sbdv0((REAL*)A,D,4,(REAL*)B)

extern  BOOLEAN c3b_eval __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , INT DUMMY5 , PT3* DUMMY6 )) ;
extern  BOOLEAN c3b_eval_pt __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , PT3 DUMMY5 )) ;
extern  void    c3b_dot __(( HPT3* DUMMY0 , INT DUMMY1 , HPT3* DUMMY2 ,
            INT DUMMY3 , HREAL* DUMMY4 )) ;
extern  BOOLEAN c3b_project __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , REAL* DUMMY5 , PT3 DUMMY6 , 
            REAL* DUMMY7 )) ;
extern  BOOLEAN c3b_ept0 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 , 
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN c3b_etan0 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN c3b_enormal0 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN c3b_ept_tan0 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 )) ;
extern  BOOLEAN c3b_ept1 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN c3b_etan1 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN c3b_enormal1 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 )) ;
extern  BOOLEAN c3b_ept_tan1 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 )) ;
extern  void    c3b_brk0 __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            HPT3* DUMMY3 )) ;
extern  void    c3b_brkl __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            HPT3* DUMMY3 )) ;
extern  void    c3b_brkr __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            HPT3* DUMMY3 )) ;
extern  void    c3b_sbdv0 __(( HPT3* DUMMY0 , INT DUMMY1 , HPT3* DUMMY2 )) ;
extern  BOOLEAN c3b_project_init __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , REAL* DUMMY5 )) ;
extern  BOOLEAN c3b_project_iter __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , REAL* DUMMY5 )) ;
extern  BOOLEAN c3b_approx __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , INT DUMMY6 ,
            REAL DUMMY7 , C3_BOX DUMMY8 , BOOLEAN DUMMY9 , BOOLEAN DUMMY10 ,
            BOOLEAN DUMMY11 , PT3* DUMMY12 , PARM DUMMY13 , INT DUMMY14 , 
            INT* DUMMY15 , REAL* DUMMY16 )) ;
extern  void    c3b_mass_prop __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , INT DUMMY6 , 
            INT DUMMY7 , REAL DUMMY8 , PF_PROP3 DUMMY9 , REAL* DUMMY10 )) ;
extern  BOOLEAN c3b_select __(( HPT3* DUMMY0 , INT DUMMY1 , REAL DUMMY2 ,
            PT3 DUMMY3 , PT3 DUMMY4 , PT3 DUMMY5 , REAL DUMMY6 , 
            REAL DUMMY7 , REAL DUMMY8 , REAL* DUMMY9 , REAL* DUMMY10 )) ;



#endif /* C3BDEFS */


