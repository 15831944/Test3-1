//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#define __3D_POLY_CPP
#include "sc_defs.h"
#include "3d_fn.h"
#include "3d_poly.h"

// =========================================================================

IMPLEMENT_3D_MODEL(C3DPoly, "3D_Poly", "", TOC_SYSTEM, "Polynomial Surface", "2nd Order Polynomial Surface");
IMPLEMENT_SPARES(C3DPoly, 100);

pchar C3DPoly::ParmDescs[C3DPolyNoOfParms] =
  {"a0","a1","a2","a3","a4","a5","a6","a7","a8" };

// =========================================================================
 
C3DPoly::C3DPoly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C3DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C3DPoly::~C3DPoly()
  {
  }
 
//---------------------------------------------------------------------------
 
void C3DPoly::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(C3DPolyNoOfParms);
  }

// -------------------------------------------------------------------------

double C3DPoly::Zxy(double Xi, double Yi)
  {
  double x2 = Xi * Xi;
  double y2 = Yi * Yi;
  return (Parms[0] * x2 * y2) +
         (Parms[1] * x2 * Yi) +
         (Parms[2] * x2) +
         (Parms[3] * Xi * y2) +
         (Parms[4] * Xi * Yi) +
         (Parms[5] * Xi) +
         (Parms[6] * y2) +
         (Parms[7] * Yi) +
         (Parms[8]);
  }

// -------------------------------------------------------------------------

double C3DPoly::Yxz(double Xi, double Zi)
  {
  ASSERT(0);
  return 0.0;
  }

// -------------------------------------------------------------------------

flag C3DPoly::ReFit()
  {
  if (NPts()>=C3DPolyNoOfParms)
    {
    CDVector DP;
    flag b = False;//pFn->LBEst(DP, dSa, iIter, iMaxIter);
    if (!b)
      iIter = -1;   //singularity
    }
  else
    iIter = -2;  //insufficient points
  return True;
  }

// -------------------------------------------------------------------------

// =========================================================================
