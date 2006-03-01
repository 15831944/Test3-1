//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_PART_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_part.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DPart, "2D_Parti", "", TOC_SYSTEM, "S-Curve", "Partition curve (S-Curve)");

// =========================================================================

pchar C2DPart::ParmDescs[PartNoOfParms] =
  {"Alpha", "Xd50", "Skew" };

// =========================================================================
 
C2DPart::C2DPart(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DPart::~C2DPart()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DPart::Clear()
  {
  CBaseMdl::Clear();
  SetNParms(PartNoOfParms);
  Parms[0] = 10.0; //Alpha
  Parms[1] = 0.5; //Xd50
  Parms[2] = 0.0; //Skew
  bInvert = 0;
  }

// -------------------------------------------------------------------------

void C2DPart::CopyModel(pC2DPart pMd)
  {
  if (pMd == this)
    return;
  bInvert = pMd->bInvert;
  CBaseMdl::CopyModel(pMd);
  }

// -------------------------------------------------------------------------

void C2DPart::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Bool  ("Invert",  "", DC_, "", &bInvert , this, isParm);
  CBaseMdl::BuildDataDefn(DDB);
  }

// -------------------------------------------------------------------------
 
double C2DPart::Xy(double Yi)
  {
  Yi = Range(0.0001, Yi, 0.9999);
  Parms[0] = Max(1.0e-10, Parms[0]);//Alpha
  Parms[1] = Max(1.0e-10, Parms[1]);//Xd50
  double a = 2.0*Yi - 1.0 - Yi*exp(Parms[0])/(Yi-1.0);
  double b = log(GTZ(a));
  return Parms[1]*b/(Parms[0]);
  }

// -------------------------------------------------------------------------
 
double C2DPart::Yx(double Xi)
  {
  Parms[1] = Max(1.0e-10, Parms[1]);//Xd50
  double tAlpha = Max(Parms[0], 1.0e-12*Parms[1]/GTZ(Xi));
  double ex = exp(Range(-87.0, tAlpha*Xi/Parms[1], 87.0));
  double e = exp(Range(-87.0, tAlpha, 87.0));
  double y = (ex-1.0)/GTZ(ex+e-2.0);
  if (bInvert) 
    y = 1.0-y;
  return y;
  }

// -------------------------------------------------------------------------

flag C2DPart::ReFit()
  {
  if (NPts()>2)
    {
    CDVector DP;
    pFn->LBEst(DP, dSa, iIter, iMaxIter); //what about bInvert !!!
    }
  return True;
  }

// -------------------------------------------------------------------------

const int Id_Invert=1;

void C2DPart::Build(flag bShowEqn)
  {
  StartBlk(bShowEqn ? 1 : 0, 0, NULL);
  if (bShowEqn)
    {
    int L=0;
    SetDParm(L,"Invert", 9, "", Id_Invert  ,  1, 2, "");
    }
  BuildParmsBlk(bShowEqn);
  }

// -------------------------------------------------------------------------

void C2DPart::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_Invert: Str.Set("%i", bInvert); break;
      }
    }
  LoadParmsBlk(EI, Str);
  }

// -------------------------------------------------------------------------

long C2DPart::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    bObjModified=1;
    switch (EI.FieldId)
      {
      case Id_Invert: bInvert=atoi(Str()); break;
      }
    }
  ParseParmsBlk(EI, Str, bObjModified);
  return 1;
  }

//---------------------------------------------------------------------------

long C2DPart::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return ButtonPushedParmsBlk(EI, Str);
  }

//---------------------------------------------------------------------------
// =========================================================================
