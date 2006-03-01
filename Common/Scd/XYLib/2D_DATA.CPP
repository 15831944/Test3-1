//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"

#define __2D_DATA_CPP

//#include "vectors.h"
#include "sc_defs.h"
#include "2d_fn.h"
#include "2d_data.h"

//#include "optoff.h"

// =========================================================================

IMPLEMENT_2D_MODEL(C2DDataOnly, "2D_Data", "", TOC_SYSTEM, "Data Only XYFunction", "Data Only XYFunction");
IMPLEMENT_SPARES(C2DDataOnly, 100);

// =========================================================================
 
C2DDataOnly::C2DDataOnly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C2DModel(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C2DDataOnly::~C2DDataOnly()
  {
  }
 
//---------------------------------------------------------------------------
 
void C2DDataOnly::Clear()
  {
  CurSeg=0;
  }

// -------------------------------------------------------------------------

void C2DDataOnly::CopyModel(pC2DDataOnly pMd)
  {
  if (pMd == this)
    return;
  CurSeg = pMd->CurSeg;
  }

// -------------------------------------------------------------------------

void C2DDataOnly::BuildDataDefn(DataDefnBlk & DDB)
  {
  }

// -------------------------------------------------------------------------
 
double C2DDataOnly::Xy(double Yi)
  {
  double Xi;//, d0, d1;//, mn, mx, y;
  //y=Y[0];
  switch (pFn->Length())
    {
    case 0 :
      CurSeg = 0;
      return 0.0;
    case 1 :
      CurSeg = 0;
      return pFn->Xp(0);
    default :
      {
      CDVector & X = pFn->Xs();
      CDVector & Y = pFn->Ys();
      long i;
      int DerivConstSign=1;
      int slope=(int)Sign(Y[1]-Y[0]);
      for (i=1; i<pFn->Length(); i++)
        {
        if (slope!=Sign(Y[i]-Y[i-1]))
          DerivConstSign=0;
        if ((Yi-Y[i-1])*(Yi-Y[i])<=0.0)
          break;
        }
      if (i<pFn->Length())
        {
        // keep i
        }
      else 
        {
        if (DerivConstSign)
          {
          if (slope>0)
            {
            if (Yi<Y[0])
              i=1;
            else
              i=pFn->Length()-1;
            }
          else if (slope<0)
            {
            if (Yi>Y[0])
              i=1;
            else
              i=pFn->Length()-1;
            }
          else
            i=-1;
          }
        else
          i=-1;
        }
      
      if (i>0)
        {
        if (fabs(Y[i] - Y[i-1]) > 1.0e-15)
          Xi = X[i-1]+(X[i] - X[i-1]) * (Yi - Y[i-1]) / (Y[i] - Y[i-1]);
        else
          Xi = X[i-1];
        }
      else
        Xi=0;
        
      CurSeg = i;
      //dbgpln("  Ch2D.Xy : Yi %#11.5g --> Xi %#11.5g", Yi, Xi);
      return Xi;
      }
    }
  }
 
// -------------------------------------------------------------------------
 
double C2DDataOnly::Yx(double Xi)
  {
  const long len = pFn->Length();
  switch (len)
    {
    case 0 : return 0.0;
    case 1 : return pFn->Yp(0);
    default :
      {
      CDVector & X = pFn->Xs();
      CDVector & Y = pFn->Ys();
      for (long i=0; (i<len-2) && (Xi>=X[i+1]); i++) 
        { };
        //if (fabs(X[i+1] - X[i]) > 1.0e-12)
        //  break;
      int xx=0;
      return Y[i]+(Y[i+1] - Y[i]) * (Xi - X[i]) / GTZ(X[i+1] - X[i]);
      //return pFn->Yp(i);
      }
    }
  }
 
// -------------------------------------------------------------------------

flag C2DDataOnly::ReFit()
  {
  return True;
  }

// -------------------------------------------------------------------------

const int Id_CurSeg=1;

void C2DDataOnly::Build()
  {
  StartBlk(0, 0, NULL);
  /*
  int L=0;
  SetSpace(L, 2);
  SetDParm(L,"CurSeg", 3, "", Id_CurSeg  ,  6, 0, "");
  */
  }

// -------------------------------------------------------------------------

void C2DDataOnly::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_CurSeg:  Str.Set("%i", CurSeg); break;
      }
    }
  }

// -------------------------------------------------------------------------

long C2DDataOnly::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return 0;
  }

//---------------------------------------------------------------------------

long C2DDataOnly::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return 0;
  }

//---------------------------------------------------------------------------
// =========================================================================
