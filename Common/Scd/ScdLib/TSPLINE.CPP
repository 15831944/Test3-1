//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include "sc_defs.h"
#define __TSPLINE_CPP

#include "tspline.h"

// =========================================================================

const byte TSpline::iSlopeFree=0;
const byte TSpline::iSlopeOfSegment=1;
const byte TSpline::iSlopeSpecd=2;
 
TSpline::TSpline() //:
  //Pt(*(new TSplinePts))
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
TSpline::~TSpline()
  {
  //delete &Pt;
  }
 
//---------------------------------------------------------------------------
 
void TSpline::Clear()
  {
  CurSeg = 0;
  dSigma = 1.0;
  dSlpLo = 0.0;
  dSlp1  = 0.0;
  dSlpN  = 0.0;
  dSlpHi = 0.0;
  iSlpLo = iSlopeFree;
  iSlp1  = iSlopeFree;
  iSlpN  = iSlopeFree;
  iSlpHi = iSlopeFree;
  X.SetSize(0);
  Y.SetSize(0);
  T.SetSize(0);
  }

// -------------------------------------------------------------------------

TSpline& TSpline::operator=(TSpline * pSp)
//void TSpline::operator=(pTSpline pMd)
  {
  if (pSp == this)
    return *this;
  CurSeg = pSp->CurSeg;
  dSigma  = pSp->dSigma;
  iSlpLo  = pSp->iSlpLo;
  iSlp1   = pSp->iSlp1 ;
  iSlpN   = pSp->iSlpN ;
  iSlpHi  = pSp->iSlpHi;
  dSlpLo  = pSp->dSlpLo;
  dSlp1   = pSp->dSlp1 ;
  dSlpN   = pSp->dSlpN ;
  dSlpHi  = pSp->dSlpHi;
  X=pSp->X;
  Y=pSp->Y;
  T=pSp->T;
  //Pt.Copy(pSp->Pt);
  return *this;
  }

// -------------------------------------------------------------------------
 
void TSpline::AddPt(double Xi, double Yi)
  {
  int i=X.GetLen();
  X.SetSize(i+1);
  Y.SetSize(i+1);
  T.SetSize(i+1);
  X[i]=Xi;
  Y[i]=Yi;
  T[i]=0.0;
  }

// -------------------------------------------------------------------------
 
double TSpline::Yx(double Xi)
  {
  int NPts=X.GetLen();
  if (NPts>0)
    {
  	T.SetSize(NPts);
    long i;
    long last = NPts - 1;
    double s, Sigmap, del1, del2, dels, exps1, sinhd1, exps, sinhd2, sinhs, yr;
 
    s = X[last] - X[0];
    Sigmap = fabs(dSigma) * last / s;
 
    // try the old CurSeg value first
    if (Xi < X[0])
      {
      CurSeg = 1;
      return Y[0] + dSlpLo * (Xi - X[0]);
      }
    if (Xi > X[last])
      {
      CurSeg = last;
      return Y[last] + dSlpHi * (Xi - X[last]);
      }
    CurSeg = Range(1L, CurSeg, last);
 
    while (CurSeg > 1 && X[CurSeg - 1] > Xi) CurSeg--;
    while (CurSeg < last && X[CurSeg] <= Xi) CurSeg++;
 
    i = CurSeg;
    del1 = Xi - X[i - 1];
    del2 = X[i] - Xi;
    dels = X[i] - X[i - 1];
    exps1 = exp(Sigmap * del1);
    sinhd1 = 0.5 * (exps1 - 1.0 / exps1);
    exps = exp(Sigmap * del2);
    sinhd2 = 0.5 * (exps - 1.0 / exps);
    exps = exps1 * exps;
    sinhs = 0.5 * (exps - 1.0 / exps);
    CurSeg = i;
    yr = (T[i] * sinhd1 + T[i - 1] * sinhd2) / sinhs +
    ((Y[i] - T[i]) * del1 + (Y[i - 1] - T[i - 1]) * del2) / dels;
 
  //  dbgpln("Spline::Yx %i x%#11.5g y%#11.5g",i,Xi,yr);
  //  dbgwin(1,20,"XD_6i %i x%#11.5g y%#11.5g",i,Xi,yr);
    return yr;
    }
  return 0.0;
  }
 
// -------------------------------------------------------------------------

flag TSpline::Fit()
  {
  int NPts=Y.GetLen();
  if (NPts>0)
    {
  	T.SetSize(NPts);
 
    long i;
    double Slpp1, Slppn, Sigmap, dels, exps, sinhs, sinhin, diagin, spdiag,
           delx1, dx1, delx2, dx2, diag1, diag2, delx12, c1, c2, c3, deln, delnm1, delnn;
 
    long last = NPts - 1;
    delx1 = X[1] - X[0];
    dx1 = (Y[1] - Y[0]) / delx1;
 
    if (dSigma < 0.0 || NPts == 2)
      {
      T[0] = 0.0;
      T[1] = 0.0;
      return True;
      }
    
    CDVector temp(NPts);

    if (dSigma < 0.0 || iSlp1 == iSlopeFree)
      {
      delx2 = X[2] - X[1];
      delx12 = X[2] - X[0];
      c1 = - ((delx12 + delx1) / delx12 / delx1);
      c2 = delx12 / delx1 / delx2;
      c3 = - delx1 / delx12 / delx2;
      Slpp1 = c1 * Y[0] + c2 * Y[1] + c3 * Y[2];
      }
    else if (iSlp1 == iSlopeOfSegment)
      Slpp1 = (Y[1] - Y[0]) / (X[1] - X[0]);
    else
      Slpp1 = dSlp1;
 
    if (dSigma < 0.0 || iSlpN == iSlopeFree)
      {
      deln = X[last] - X[last - 1];
      delnm1 = X[last - 1] - X[last - 2];
      delnn = X[last] - X[last - 2];
      c1 = (delnn + deln) / delnn / deln;
      c2 = - delnn / deln / delnm1;
      c3 = deln / delnn / delnm1;
      Slppn = c3 * Y[last - 2] + c2 * Y[last - 1] + c1 * Y[last];
      }
    else if (iSlpN == iSlopeOfSegment)
      Slppn = (Y[last] - Y[last - 1]) / (X[last] - X[last - 1]);
    else
      Slppn = dSlpN;
 
    if (iSlpLo == iSlopeFree)
      dSlpLo = Slpp1;
    else if (iSlpLo == iSlopeOfSegment)
      dSlpLo = (Y[1] - Y[0]) / (X[1] - X[0]);
    //else
    //  SlpLo = SlpLo_;
 
    if (iSlpHi == iSlopeFree)
      dSlpHi = Slppn;
    else if (iSlpHi == iSlopeOfSegment)
      dSlpHi = (Y[last] - Y[last - 1]) / (X[last] - X[last - 1]);
    //else
    //  SlpHi = SlpHi_;
 
    Sigmap = fabs(dSigma) * last / (X[last] - X[0]);
    dels = Sigmap * delx1;
    exps = exp(dels);
    sinhs = 0.5 * (exps - 1.0 / exps);
    sinhin = 1.0 / (delx1 * sinhs);
    diag1 = sinhin * (dels * 0.5 * (exps + 1.0 / exps) - sinhs);
    diagin = 1.0 / diag1;
    T[0] = diagin * (dx1 - Slpp1);
    spdiag = sinhin * (sinhs - dels);
    temp[0] = diagin * spdiag;
    if (last != 1)
      for (i = 1; i < last; i++)
        {
        delx2 = X[i + 1] - X[i];
        dx2 = (Y[i + 1] - Y[i]) / delx2;
        dels = Sigmap * delx2;
        exps = exp(dels);
        sinhs = 0.5 * (exps - 1.0 / exps);
        sinhin = 1.0 / (delx2 * sinhs);
        diag2 = sinhin * (dels * (0.5 * (exps + 1.0 / exps)) - sinhs);
        diagin = 1.0 / (diag1 + diag2 - spdiag * temp[i - 1]);
        T[i] = diagin * (dx2 - dx1 - spdiag * T[i - 1]);
        spdiag = sinhin * (sinhs - dels);
        temp[i] = diagin * spdiag;
        dx1 = dx2;
        diag1 = diag2;
        }
    diagin = 1.0 / (diag1 - spdiag * temp[last - 1]);
    T[last] = diagin * (Slppn - dx2 - spdiag * T[last - 1]);
 
    for (i = last - 1; i>0; i--)
      T[i] = T[i] - temp[i] * T[i + 1];
    }
  return True;
  }

// =========================================================================
