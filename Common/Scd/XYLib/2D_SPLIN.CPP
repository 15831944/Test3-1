//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_SPLIN_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_splin.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DSpline, "2D_Splin", "", TOC_SYSTEM, "Spline-Tension", "Spline Under Tension");

// =========================================================================

const double C2DSpline::SlopeFree=1.2345e-300;
const double C2DSpline::SlopeOfSegment=2.3456e-300;
 
C2DSpline::C2DSpline(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DSpline::~C2DSpline()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DSpline::Clear()
  {
  CBaseMdl::Clear();
  CurSeg = 0;
  Sigma =1.0;
  SlpLo =0.0;
  Slp1 =0.0;
  SlpN =0.0;
  SlpHi =0.0;
  Yp.SetSize(0);
  }

// -------------------------------------------------------------------------

void C2DSpline::CopyModel(pC2DSpline pMd)
  {
  if (pMd == this)
    return;
  SetNParms(0);
  CurSeg = pMd->CurSeg;
  Sigma  = pMd->Sigma;
  SlpLo  = pMd->SlpLo;
  Slp1   = pMd->Slp1 ;
  SlpN   = pMd->SlpN ;
  SlpHi  = pMd->SlpHi;
  Yp     = pMd->Yp;
  CBaseMdl::CopyModel(pMd);
  }

// -------------------------------------------------------------------------

void C2DSpline::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("Sigma",  "", DC_, "", &Sigma  , this, isParm);
  DDB.Double("dYdXLo", "", DC_, "", &SlpLo  , this, isParm);
  DDB.Double("dYdX1",  "", DC_, "", &Slp1   , this, isParm);
  DDB.Double("dYdXN",  "", DC_, "", &SlpN   , this, isParm);
  DDB.Double("dYdXHi", "", DC_, "", &SlpHi  , this, isParm);
  CBaseMdl::BuildDataDefn(DDB);
  }

// -------------------------------------------------------------------------
 
double C2DSpline::Yx(double Xi)
  {
  if (NPts()>0)
    {
    rCDVector X = pFn->Xs();
    rCDVector Y = pFn->Ys();
  	Yp.SetSize(NPts());
    long i;
    long last = NPts() - 1;
    double s, Sigmap, del1, del2, dels, exps1, sinhd1, exps, sinhd2, sinhs, yr;
 
    s = X[last] - X[0];
    Sigmap = fabs(Sigma) * last / GTZ(s);
 
    // try the old CurSeg value first
    if (Xi < X[0])
      {
      CurSeg = 1;
      return Y[0] + SlpLo * (Xi - X[0]);
      }
    if (Xi > X[last])
      {
      CurSeg = last;
      return Y[last] + SlpHi * (Xi - X[last]);
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
    yr = (Yp[i] * sinhd1 + Yp[i - 1] * sinhd2) / GTZ(sinhs) +
    ((Y[i] - Yp[i]) * del1 + (Y[i - 1] - Yp[i - 1]) * del2) / GTZ(dels);
 
  //  dbgpln("Spline::Yx %i x%#11.5g y%#11.5g",i,Xi,yr);
  //  dbgwin(1,20,"XD_6i %i x%#11.5g y%#11.5g",i,Xi,yr);
    return yr;
    }
  return 0.0;
  }
 
// -------------------------------------------------------------------------

flag C2DSpline::ReFit()
  {
  if (NPts()>0)
    {
    rCDVector X = pFn->Xs();
    rCDVector Y = pFn->Ys();
  	Yp.SetSize(NPts());
 
    long i;
    double Slpp1, Slppn, Sigmap, dels, exps, sinhs, sinhin, diagin, spdiag,
           delx1, dx1, delx2, dx2, diag1, diag2, delx12, c1, c2, c3, deln, delnm1, delnn;
 
    long last = NPts() - 1;
    delx1 = X[1] - X[0];
    dx1 = (Y[1] - Y[0]) / delx1;
 
    if (Sigma < 0.0 || NPts() == 2)
      {
      Yp[0] = 0.0;
      Yp[1] = 0.0;
      return True;
      }
    
    CDVector temp(NPts());

    if (Sigma < 0.0 || Slp1 == SlopeFree)
      {
      delx2 = X[2] - X[1];
      delx12 = X[2] - X[0];
      c1 = - ((delx12 + delx1) / delx12 / delx1);
      c2 = delx12 / delx1 / delx2;
      c3 = - delx1 / delx12 / delx2;
      Slpp1 = c1 * Y[0] + c2 * Y[1] + c3 * Y[2];
      }
    else if (SlpN == SlopeOfSegment)
      Slpp1 = (Y[1] - Y[0]) / (X[1] - X[0]);
    else
      Slpp1 = Slp1;
 
    if (Sigma < 0.0 || SlpN == SlopeFree)
      {
      deln = X[last] - X[last - 1];
      delnm1 = X[last - 1] - X[last - 2];
      delnn = X[last] - X[last - 2];
      c1 = (delnn + deln) / delnn / deln;
      c2 = - delnn / deln / delnm1;
      c3 = deln / delnn / delnm1;
      Slppn = c3 * Y[last - 2] + c2 * Y[last - 1] + c1 * Y[last];
      }
    else if (SlpN == SlopeOfSegment)
      Slppn = (Y[last] - Y[last - 1]) / (X[last] - X[last - 1]);
    else
      Slppn = SlpN;
 
    if (SlpLo == SlopeFree)
      SlpLo = Slpp1;
    else if (SlpLo == SlopeOfSegment)
      SlpLo = (Y[1] - Y[0]) / (X[1] - X[0]);
    //else
    //  SlpLo = SlpLo_;
 
    if (SlpHi == SlopeFree)
      SlpHi = Slppn;
    else if (SlpHi == SlopeOfSegment)
      SlpHi = (Y[last] - Y[last - 1]) / (X[last] - X[last - 1]);
    //else
    //  SlpHi = SlpHi_;
 
    Sigmap = fabs(Sigma) * last / (X[last] - X[0]);
    dels = Sigmap * delx1;
    exps = exp(dels);
    sinhs = 0.5 * (exps - 1.0 / exps);
    sinhin = 1.0 / (delx1 * sinhs);
    diag1 = sinhin * (dels * 0.5 * (exps + 1.0 / exps) - sinhs);
    diagin = 1.0 / diag1;
    Yp[0] = diagin * (dx1 - Slpp1);
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
        Yp[i] = diagin * (dx2 - dx1 - spdiag * Yp[i - 1]);
        spdiag = sinhin * (sinhs - dels);
        temp[i] = diagin * spdiag;
        dx1 = dx2;
        diag1 = diag2;
        }
    diagin = 1.0 / (diag1 - spdiag * temp[last - 1]);
    Yp[last] = diagin * (Slppn - dx2 - spdiag * Yp[last - 1]);
 
    for (i = last - 1; i>0; i--)
      Yp[i] = Yp[i] - temp[i] * Yp[i + 1];
    }
  return True;
  }

// -------------------------------------------------------------------------

const int Id_Sigma=1;
const int Id_SlpLo=2;
const int Id_Slp1 =3;
const int Id_SlpN =4;
const int Id_SlpHi=5;

void C2DSpline::Build()
  {
  StartBlk(5, 0, NULL);
  int L=0;
  SetDParm(L,"Sigma",  6, "", Id_Sigma  ,  10, 2, "");
  L++;
  SetDParm(L,"dYdXLo", 6, "", Id_SlpLo  ,  10, 2, "");
  L++;
  SetDParm(L,"dYdX0 ", 6, "", Id_Slp1   ,  10, 2, "");
  L++;
  SetDParm(L,"dYdXN ", 6, "", Id_SlpN   ,  10, 2, "");
  L++;
  SetDParm(L,"dYdXHi", 6, "", Id_SlpHi  ,  10, 2, "");
  }

// -------------------------------------------------------------------------

void C2DSpline::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_Sigma:   Str.Set("%g", Sigma); break;
      case Id_SlpLo:   Str.Set("%g", SlpLo); break;
      case Id_Slp1 :   Str.Set("%g", Slp1 ); break;
      case Id_SlpN :   Str.Set("%g", SlpN ); break;
      case Id_SlpHi:   Str.Set("%g", SlpHi); break;
      }
    }
  }

// -------------------------------------------------------------------------

long C2DSpline::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    //DataChanged=1;
    switch (EI.FieldId)
      {
      case Id_Sigma:  Sigma=Str.SafeAtoF(); Fn().ReFit(); break;
      case Id_SlpLo:  SlpLo=Str.SafeAtoF(); Fn().ReFit(); break;
      case Id_Slp1 :  Slp1 =Str.SafeAtoF(); Fn().ReFit(); break;
      case Id_SlpN :  SlpN =Str.SafeAtoF(); Fn().ReFit(); break;
      case Id_SlpHi:  SlpHi=Str.SafeAtoF(); Fn().ReFit(); break;
      }
    }
  return 1;
  }

//---------------------------------------------------------------------------

long C2DSpline::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return 0;
  }

//---------------------------------------------------------------------------
// =========================================================================
