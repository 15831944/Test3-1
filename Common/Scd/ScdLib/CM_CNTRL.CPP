//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>

#define __CM_CNTRL_CPP

#include "cm_cntrl.h"

#define dbgEst

//==========================================================================
//
//
//
//==========================================================================

CMFControls::CMFControls(CCMinFinder *pFn_, double Tol/*=1.0e-6*/, int ItMax_/*=100*/)
  {
  pFn=pFn_;
  dTol=Tol;
  iItMax=ItMax_;
  };

// -------------------------------------------------------------------------
// ...
int CMFControls::Solve()//CDVector &DeltaParms, double &Sa) 
  {
  //CRegStuff R;
  InitParameters();

  #ifdef dbgEst
  dbgpln("------------** Controls **------------");
  #endif
  return True;
  }

//==========================================================================
