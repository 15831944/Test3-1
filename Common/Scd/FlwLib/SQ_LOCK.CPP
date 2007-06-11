//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>               

#define  __SQ_LOCK_CPP

#include "sc_defs.h"
#include "sp_db.h"
#include "sq_lock.h"
#include "executiv.h"

#define  dbgFlash   WITHDEBUG

#include "dbgmngr.h"

#define dbgLockUp  WITHDEBUG

#if dbgLockUp
static CDbgMngr dbgLockup      ("Lockup",  "Build");
#endif


// ==========================================================================
//                        Specie Quality - Size Distribution
// ==========================================================================


IMPLEMENT_SPARES(SQLockup, 100);
IMPLEMENT_TAGOBJ(SQLockup, "SpQuality", "LockUp", "", "", "LKUP", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, 
                 "Specie Lockup",
                 "Specie Lockup to describe the fraction of species unavali to chemical attack");
SQLockup::SQLockup(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpQuality(pClass_, Tag_, pAttach, eAttach)
  {
  Lockup.SetSize(SDB.Count(), 0.0);
  };

// --------------------------------------------------------------------------

SQLockup::~SQLockup()
  {
  };

// --------------------------------------------------------------------------

void SQLockup::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, "Lockup", NULL, DDB_RqdPage))
    {
    for (int i=0; i<SDB.Count(); i++)
      DDB.Double(SDB[i].Tag(),SDB[i].Sym(),DC_Frac,"%", &Lockup[i],this,isParm);
  
    //DDB.Object(&SizeFn, this, DDB_RqdPage);
    DDB.Text("");
    }
  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag SQLockup::ValidateData(ValidateDataBlk & VDB)
  {
  for (int s=0; s<SDB.Count(); s++)
    Lockup[s]=Range(0.0, Lockup[s], 1.0);
  return True;
  };

// --------------------------------------------------------------------------

void SQLockup::ExchangeSpecies(SpQuality * Other)
  {
  //dbgpln("Exch ??? %s ???", Tag());
  };

// --------------------------------------------------------------------------

void SQLockup::ZeroMass()
  {
  Lockup.Mult(0.0);
  };

// --------------------------------------------------------------------------

void SQLockup::ZeroDeriv()
  {
  };

// --------------------------------------------------------------------------

void SQLockup::ScaleMass(PhMask PhaseM, double Mult)
  {
  };

// --------------------------------------------------------------------------

void SQLockup::ScaleMass(CIArray & SpIds, double Mult)
  {
  };

// --------------------------------------------------------------------------

void SQLockup::ScaleMass(CSysVector  &Scl, double Mult)
  {
  };

// --------------------------------------------------------------------------

void SQLockup::SetMassF(CSysVector &M1, SpQuality * pQual2)//pSpModel pMdl2, SpQuality * pQual2, CSysVector &Filter, double Frac)
  {
  Lockup=((pSQLockup)pQual2)->Lockup;
  //SizeFn.CopyFunction(((pSQLockup)pQual2)->SizeFn);
  };

// --------------------------------------------------------------------------

void SQLockup::AddMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2)
  {
  pSQLockup pQ2=(pSQLockup)pQual2;
  if (pQ2)
    for (int i=0; i<SDB.Count(); i++)
      Lockup[i]=(M1[i]*Lockup[i] + M2[i]*pQ2->Lockup[i])/GTZ(M1[i]+M2[i]);
  };

// --------------------------------------------------------------------------

void SQLockup::SubMassF(CSysVector &M1, SpQuality * pQual2, CSysVector &M2)
  {
  };

// --------------------------------------------------------------------------

void SQLockup::Copy(pSpModel pMdl2, SpQuality * pQual2)
  {
  Lockup=((pSQLockup)pQual2)->Lockup;
  };

// --------------------------------------------------------------------------

void SQLockup::AddDeriv(pSpModel pMdl2, SpQuality * pQual2, double Sgn_)
  {
  // There should not be any code here 
  // Qualities are added in Discrete
  };

// --------------------------------------------------------------------------

void SQLockup::AddDiscrete(pSpModel pMdl2, SpQuality * pQual2, double Sgn_)
  {
  pSQLockup pQ2=(pSQLockup)pQual2;
  if (pQ2 && Sgn_>=0.0)
    for (int i=0; i<SDB.Count(); i++)
      {
      double m1=pModel->m_M[i];
      double m2=pMdl2->m_M[i]*ICGetTimeInc();
      Lockup[i]=(m1*Lockup[i] + m2*pQ2->Lockup[i])/GTZ(m1+m2);
      }
  };

// --------------------------------------------------------------------------

void SQLockup::ODEOperate(CODEDataBlock & ODB)
  {
  };

//===========================================================================
