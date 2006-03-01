//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#define __3D_DATA_CPP
#include "sc_defs.h"
#include "3d_fn.h"
#include "3d_data.h"

// =========================================================================

IMPLEMENT_3D_MODEL(C3DDataOnly, "3D_Data", "", TOC_SYSTEM, "DataOnly For Surface", "DataOnly Surface");
IMPLEMENT_SPARES(C3DDataOnly, 100);

// =========================================================================
 
C3DDataOnly::C3DDataOnly(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  C3DModel(pClass_, pTag, pAttach, eAttach)
  {
  bDoYReFit=0;
  Clear();
  }
 
// -------------------------------------------------------------------------
 
C3DDataOnly::~C3DDataOnly()
  {
  Clear();
  }
 
// -------------------------------------------------------------------------

void C3DDataOnly::CopyModel(pC3DDataOnly pMd)
  {
  if (pMd == this)
    return;
  AllContours = pMd->AllContours;
  m_TheLock.Lock();
  pFn->SetIsBuilt(False); //ReFit(); //forces trinet to be built
  m_TheLock.Unlock();
  }

// -------------------------------------------------------------------------

void C3DDataOnly::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Bool  ("AllContours", "", DC_,         "",     &AllContours, this, isParm);
  //save trinet ???
  }

//---------------------------------------------------------------------------
 
void C3DDataOnly::Clear()
  {
  AllContours = 0;
  bDoYReFit=1;
  }

// -------------------------------------------------------------------------
 /*
short C3DDataOnly::DataQuadrant(double x, double y)
  {
  if( y < 0.0 )
    {
    if( x < 0.0 )
      {
      if( fabs(x) < fabs(y) )  return 32;
      else                     return 16;
      }
    else
      {
      if( fabs(x) < fabs(y) )  return 64;
      else                     return 128;
      }
    }
  else
    {
    if( x < 0.0 )
      {
      if( fabs(x) < fabs(y) )  return 4;
      else                     return 8;
      }
    else
      {
      if( fabs(x) < fabs(y) )  return 2;
      else                     return 1;
      }
    }
  ASSERT(0);
  }
*/
// -------------------------------------------------------------------------
 
double C3DDataOnly::Zxy(double x, double y)
  {
  m_TheLock.Lock();
  if (!pFn->IsBuilt())
    ReFit();
  //dbgp("Refit->Zxy");
  double z;
  if (!pFn->Trinet.XYZonplane(x,y,z))
    z=dNAN;
  //dbgpln("=%14.6g", z);
  m_TheLock.Unlock();
  return z;
  }

// -------------------------------------------------------------------------
 
double C3DDataOnly::Yxz(double x, double z)
  {
  m_TheLock.Lock();
  if (!pFn->IsBuilt())
    ReFit();
  if (bDoYReFit)
    {
    YTrinet.Clear();
    YTrinet.InitTriNet(pFn->ScanXMin(), pFn->ScanZMin(), pFn->ScanXMax(), pFn->ScanZMax());
    for (int i=0; i<pFn->Zs().GetLen(); i++ )
      YTrinet.AddVertex(pFn->Xp(i), pFn->Zp(i), pFn->Yp(i), 0);

    //dbgpln("YTriNet");
    i=0;
    double x1,y1,z1, x2,y2,z2, x3,y3,z3;
    for (flag ok=YTrinet.first_triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3); ok ; ok=YTrinet.next_triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3))
      dbgpln("%3i  %10g,%10g  %10g,%10g  %10g,%10g",++i, x1,y1, x2,y2, x3,y3);
    }
  bDoYReFit=0;
  //dbgp("Refit->Yxz");
  double y;
  if (!YTrinet.XYZonplane(x,z,y))
    y=dNAN;
  //dbgpln("=%14.6g", y);
  m_TheLock.Unlock();
  return dNAN;
  }

// -------------------------------------------------------------------------

flag C3DDataOnly::ReFit()
  {
  m_TheLock.Lock();
  bDoYReFit=1;
  if (!pFn->IsBuilt())
    {
    //dbgpln("Refit->entry");
    pFn->ReFit();                   
    //dbgpln("Refit->exit");
    }
  m_TheLock.Unlock();
  return True;
  }

// -------------------------------------------------------------------------

const short Id_AllContours = 1;

void C3DDataOnly::Build()
  {
  StartBlk(1, 0, NULL);
  short L=0;
  SetDParm(L,"All Contours", 13, "", Id_AllContours, 1, 0, "");
  }

// -------------------------------------------------------------------------

void C3DDataOnly::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_AllContours: Str.Set("%i", AllContours); break;
      }
    }
  }

// -------------------------------------------------------------------------

long C3DDataOnly::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix = 0;
  if (CurrentBlk(EI))
    {
    switch (EI.FieldId)
      {
      case Id_AllContours:
        AllContours=(byte)Str.SafeAtoL();
        Fix=1;
        break; 
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long C3DDataOnly::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return 0;
  }

//---------------------------------------------------------------------------
// =========================================================================
