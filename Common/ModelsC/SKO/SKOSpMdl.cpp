//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SKOSPMDL_CPP
#include "sp_db.h"
#include "skospmdl.h"
#include "errorlog.h"

#include "gassep.h"

// ==========================================================================

#define dbgModels 01
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgSpecies ("BASIC", "Species");
#endif


// ==========================================================================
//
//                      SKO Specie Model
//
// ==========================================================================
  
// must be in Range 0x0001 to 0x8000
const dword SMVF_LiqConcOK   = 0x00000001;

const word xidAbundance = 30006;

IMPLEMENT_SPMODEL(CSKOSpMdl, "SKO", "", TOC_ALL|TOC_SYSTEM, "SKO", "SKO");
IMPLEMENT_SPARES(CSKOSpMdl, 100);

CSKOSpMdl::CSKOSpMdl(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SpModel(pClass_, Tag_, pAttach, eAttach)
  {
  }

// --------------------------------------------------------------------------

CSKOSpMdl::~CSKOSpMdl()
  {
  }

// --------------------------------------------------------------------------

void CSKOSpMdl::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  SpModel::BuildDataDefn_Vars(DDB);
  DDB.Text("SKO Props");
  DDB.Double("Abundance",   "",  DC_,  "",     xidAbundance, this,  isResult|noFile|noSnap|NAN_OK);
  }

// --------------------------------------------------------------------------

flag CSKOSpMdl::DataXchg(DataChangeBlk & DCB)
  {
  if (SpModel::DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xidAbundance:     
      //if (CSKOGasSep::m_lSpRich>=0 &&CSKOGasSep::m_lSpStrip>=0)
      //  {
      //  DCB.D=m_M[CSKOGasSep::m_lSpRich]/GTZ(m_M[CSKOGasSep::m_lSpStrip]); 
      //  }
      //else
        DCB.D=dNAN; 
      
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag CSKOSpMdl::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=SpModel::ValidateData(VDB);
  
  return OK;
  }

//--------------------------------------------------------------------------

flag CSKOSpMdl::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\t1 ???"; return 1;
    case 2: pS="W\t2 ???"; return 1;
    case 3: pS="W\t3 ???"; return 1;
    case 4: pS="W\t4 ???"; return 1;
    case 5: pS="W\t5 ???"; return 1;
    case 6: pS="W\t6 ???"; return 1;
    default:                                               
      return SpModel::CIStrng(No, pS);
    }
  }

// ==========================================================================
//
//                                  End
//
// ==========================================================================
//

