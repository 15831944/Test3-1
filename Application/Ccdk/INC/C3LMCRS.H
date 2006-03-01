//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/******************************* C3LMCRS.H *********************************/
#ifndef C3LMCRS
#define C3LMCRS

#define     C3_LINE_PT0(L)      ((L)->pt[0])
#define     C3_LINE_PT1(L)      ((L)->pt[1])
#define     C3_LINE_DIR_VEC(L,DV) C3V_SUB(C3_LINE_PT1(L),C3_LINE_PT0(L),DV)
#endif  /* C3LMCRS */

