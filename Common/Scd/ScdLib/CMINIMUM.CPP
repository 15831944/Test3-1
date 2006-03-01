//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>

#define __CMINIMUM_CPP

#include "cminimum.h"
#include "cm_cntrl.h"
#include "cm_lawbl.h"
#include "cm_flexi.h"

#define dbgEst

//==========================================================================

void CMFCommon::Normalise_Parms(CDVector &UParms, CDVector &NParms)
  {
  int J = 0;
  for (int I = 0; I < NumParms; I++)
    if (! ParmHolds[I])
      {
      if (DoParmsRange)
        NParms[J] = (UParms[I] - ParmMins[I]) / Max(1.0e-50,(ParmMaxs[I] - ParmMins[I]));
      else
        NParms[J] = UParms[I];
      J++;
      }
  }

//==========================================================================

void CMFCommon::DeNormalise_Parms(CDVector &UParms, CDVector &NParms)
  {
  int J = 0;
  for (int I = 0; I < NumParms; I++)
    if (! ParmHolds[I])
      {
      if (DoParmsRange)
        UParms[I] = ParmMins[I] + (ParmMaxs[I] - ParmMins[I]) * NParms[J];
      else
        UParms[I] = NParms[J];
      J++;
      }
  }

//==========================================================================

void CMFCommon::InitParameters()
  {
  NumParms = pFn->NParms();
  UserParms.SetSize(NumParms);
  ParmMins.SetSize(NumParms);
  ParmMaxs.SetSize(NumParms);
  ParmHolds.SetSize(NumParms);
  ParmTols.SetSize(NumParms);

  ParmHolds.SetAll(0);

  pFn->InitParms(UserParms, ParmHolds);
  DoParmsRange=pFn->GetParmRanges(ParmMins, ParmMaxs);
  
  NumParmsUsed = 0;
  for (int i = 0; i < NumParms; i++)
    if (! ParmHolds[i])
      NumParmsUsed++;

  NormParms.SetSize(NumParmsUsed);
  Normalise_Parms(UserParms, NormParms);
  }

//==========================================================================
//
//
//
//==========================================================================

int CCMinFinder::FindMinimum(byte Meth)
  {
  int Ret=-1;
  switch (Meth)
    {
    case CMM_None:; break;
    case CMM_Controls:
      {
      CMFControls *p=new CMFControls(this);
      Ret=p->Solve();
      delete p;
      //CMFControls Cn(this);
      //Ret=Cn.Solve();
      break;
      };
    case CMM_LawBailey:
      {
      CMFLawBailey LB(this);
      Ret=LB.Solve();
      break;
      };
    case CMM_FlexiPlex:
      {
      //CMFFlexiPlex *p=new CMFFlexiPlex(this);
      //Ret=p->Solve();
      //delete p;
      CMFFlexiPlex FP(this);
      Ret=FP.Solve();
      break;
      };
    };
  return Ret;
  };

//==========================================================================
//
//
//
//==========================================================================
