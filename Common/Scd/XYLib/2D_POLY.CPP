//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define __2D_POLY_CPP
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_poly.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DPoly, "2D_Poly", "", TOC_SYSTEM, "Polynomial", "n'th order polynomial");

// =========================================================================

pchar C2DPoly::ParmDescs[C2DPolyMaxOrder+1] = 
  {"a0","a1","a2","a3","a4","a5","a6","a7","a8","a9"};

C2DPoly::C2DPoly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DPoly::~C2DPoly()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DPoly::Clear()
  {
  CBaseMdl::Clear();
  SetOrder(1);
  }

// -------------------------------------------------------------------------

void C2DPoly::CopyModel(pC2DPoly pMd)
  {
  if (pMd == this)
    return;
  Order = pMd->Order;
  CBaseMdl::CopyModel(pMd);
  }

// -------------------------------------------------------------------------

XID xidPolyOrder   = XyXID(100);

void C2DPoly::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Long("Order",    "", DC_, "", xidPolyOrder   , this, isParm);
  CBaseMdl::BuildDataDefn(DDB);
  }

// -------------------------------------------------------------------------

flag C2DPoly::DataXchg(DataChangeBlk &DCB) 
  {
  switch (DCB.lHandle)
    {
    case xidPolyOrder:
      if (DCB.rL)
        SetOrder(*(DCB.rL));
      DCB.L = Order;
      return 1;
    }
  return False; 
  }

// -------------------------------------------------------------------------

flag C2DPoly::InitParms(flag DoIt)
  {
  if (DoIt)
    {
    Parms[0] = Yx(0.0);
    }
  return True;
  }

// -------------------------------------------------------------------------
 
double C2DPoly::Yx(double Xi)
  {
  double d = Parms[0];
  double x = 1.0;
  for (int i=1; i<=Order; i++)
    {
    x *= Xi;
    d += (Parms[i] * x);
    }
  return d;
  }
 
// -------------------------------------------------------------------------

flag C2DPoly::ReFit()
  {
  if (NPts()>Order)
    {
    SetOrder(Order);
    CDVector DP;
    if (!pFn->LBEst(DP, dSa, iIter, iMaxIter))
      iIter = -1;   //singularity
    }
  else
    iIter = -2;  //insufficient points
  return True;
  }

// -------------------------------------------------------------------------

void C2DPoly::SetOrder(long nOrder)
  {
  nOrder = Min(Max(nOrder,1L), C2DPolyMaxOrder);
  SetNParms(nOrder+1);
  if (pFn && nOrder!=Order)
    pFn->StructureChanged(pFn);
  Order = nOrder;
  }

// -------------------------------------------------------------------------

const int Id_Order=1;

void C2DPoly::Build(flag bShowEqn)
  {
  StartBlk(bShowEqn ? 1 : 0, 0, NULL);
  if (bShowEqn)
    {
    int L=0;
    SetDParm(L,"Order",    9, "", Id_Order   ,  5, 2, "");
    }
  BuildParmsBlk(bShowEqn);
  }

// -------------------------------------------------------------------------

void C2DPoly::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_Order:    Str.Set("%i", Order); break;
      }
    }
  LoadParmsBlk(EI, Str);
  }

// -------------------------------------------------------------------------

long C2DPoly::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    bObjModified=1;
    switch (EI.FieldId)
      {
      case Id_Order: 
        {
        int OldOrder = Order;
        SetOrder(Str.SafeAtoL());
        if (Order!=OldOrder)
          View().DoRebuild();
        break;
        }
      }
    }
  ParseParmsBlk(EI, Str, bObjModified);
  return 1;
  }

//---------------------------------------------------------------------------

long C2DPoly::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return ButtonPushedParmsBlk(EI, Str);
  }

//---------------------------------------------------------------------------
// =========================================================================
