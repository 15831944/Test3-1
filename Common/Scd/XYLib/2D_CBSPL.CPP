//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"

#define __2D_CBSPL_CPP
#include "sc_defs.h"
#include "2d_fn.h"

#include "2d_cbspl.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DCubicSpline, "2D_CbSpl", "", TOC_SYSTEM, "Cubic-Spline", "Cubic Spline Fit");

// =========================================================================

 
C2DCubicSpline::C2DCubicSpline(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach),
  CSpline()
  {
  //TRACE("XYFf::C2DCubicSpline");
  Clear();
  }
 
// -------------------------------------------------------------------------
/*
C2DCubicSpline::C2DCubicSpline(const XYModel& Fn) :
  XYModel()
  {
  }
*/
// -------------------------------------------------------------------------
 
C2DCubicSpline::~C2DCubicSpline()
  {
  Clear();
  };
 
// -------------------------------------------------------------------------

void C2DCubicSpline::CopyModel(pC2DCubicSpline pMd)
  {
  if (pMd == this)
    return;
  Yderiv = pMd->Yderiv;
  Xderiv = pMd->Xderiv;
  CSpline = pMd->CSpline;
  }

// -------------------------------------------------------------------------

void C2DCubicSpline::BuildDataDefn(DataDefnBlk & DDB)
  {
  CBaseMdl::BuildDataDefn(DDB);
  }

//---------------------------------------------------------------------------
 
void C2DCubicSpline::Clear()
  {
  Yderiv.SetSize(0);
  Xderiv.SetSize(0);
  /*CSpline.Clear(); */
  };

// -------------------------------------------------------------------------
 
double C2DCubicSpline::Xy(double Yi)
  {
  CDVector & X =  pFn->Xs();
  CDVector & Y =  pFn->Ys();

  if ((Y.GetLen()>3)&&(X.ScanMin()>0.0)&&(Y.ScanMin()>0.0))
    {
    if (Xderiv.GetLen() <= 0)
      {
      CSpline.Spline(Y, X, Xderiv);
      }
    double xv, yv;

    yv=Range(Y.ScanMin(), Yi, Y.ScanMax());  // can't extrapolate too well

    if (CSpline.Splint(Y, X, Xderiv, yv, &xv)==0)
      return xv;
    else
      return 0.0;
    }
  else
    return 0.0;
  //return CSpline.Xy(Yi);
  }

// -------------------------------------------------------------------------
 
double C2DCubicSpline::Yx(double Xi)
  {

  CDVector & X =  pFn->Xs();
  CDVector & Y =  pFn->Ys();

  if ((X.GetLen()>3)&&(X.ScanMin()>0.0)&&(Y.ScanMin()>0.0))
    {
    if (Yderiv.GetLen() <= 0)
      {
      CSpline.Spline(X, Y, Yderiv);
      }

    double yv, xv;

    xv=Range(X.ScanMin(), Xi, X.ScanMax());  // can't extrapolate too well

    if (CSpline.Splint(X, Y, Yderiv, xv, &yv)==0)
      return yv;
    else
      return 0.0;
    }
  else
    return 0.0;
  //return CSpline.Yx(Xi);
  }
 
// -------------------------------------------------------------------------
/* 
double C2DCubicSpline::dydx(double Xi, long & CurSeg)
  {
  return (Yx(Xi, CurSeg) - Yx(Xi + 0.001, CurSeg)) / 0.001;
  }
*/


// -------------------------------------------------------------------------
/*
flag C2DCubicSpline::DoFit()
  {

  return ReFit();
  } */

// -------------------------------------------------------------------------

flag C2DCubicSpline::ReFit()
  {
  if (NPts()>0)
    {
    CDVector & X =  pFn->Xs();
    CDVector & Y =  pFn->Ys();
    CSpline.Spline(X, Y, Yderiv);
    CSpline.Spline(Y, X, Xderiv);
    }
  
  return True;
  }

// -------------------------------------------------------------------------


void C2DCubicSpline::Build()
  {
  }

// -------------------------------------------------------------------------

void C2DCubicSpline::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  }

// -------------------------------------------------------------------------

long C2DCubicSpline::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    Fn().ReFit();
    }
  return 0;
  }

//---------------------------------------------------------------------------

long C2DCubicSpline::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return 0;
  }

//---------------------------------------------------------------------------
/*
flag C2DCubicSpline::Changed(FxdEdtInfo &EI)
  {
  if (CurrentBlk(EI))
    {
    }
  return false;
  };

//---------------------------------------------------------------------------

void C2DCubicSpline::ClearChanged(FxdEdtInfo &EI)
  {
  if (CurrentBlk(EI))
    {
    }
  };
*/

// =========================================================================
