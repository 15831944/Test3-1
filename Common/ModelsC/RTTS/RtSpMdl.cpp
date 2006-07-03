//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __RTSPMDL_CPP
#include "sp_db.h"
#include "rtspmdl.h"
#define SIZEDST1
#include "models.h"
#include "op_db.h"

// ==========================================================================

#define dbgModels 01
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgSpecies ("BASIC", "Species");
#endif


// ==========================================================================
//
//                      Rio Tinto Specie Model
//
// ==========================================================================
  
// must be in Range 0x0001 to 0x8000
const dword SMVF_LiqConcOK   = 0x00000001;

const word xidComp0 = 30010;

IMPLEMENT_SPMODEL(CRtSpMdl, "RioTinto", "", TOC_ALL|TOC_RTTS, "RioTinto", "Rio Tinto Size Specie Model");
IMPLEMENT_SPARES(CRtSpMdl, 100);

CRtSpMdl::CRtSpMdl(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SpModel(pClass_, Tag_, pAttach, eAttach)
  {
  }

// --------------------------------------------------------------------------

CRtSpMdl::~CRtSpMdl()
  {
  }

// --------------------------------------------------------------------------

void CRtSpMdl::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {
  SpModel::BuildDataDefn_Vars(DDB);

  //Strng Tg(DDB.BlockTag());
  //Tg+="..";
  //DDB.Page(Tg(), DDB_OptPage);
  if (ODB.AssayCompCount()>0)
    {
    DDB.BeginObject(this, "SD", "SizeData", NULL, DDB_OptPage);
    DDB.Text("Extra Size Data");
    for (int i=0; i<ODB.AssayCompCount(); i++)
      DDB.Double((char*)ODB.AssayName(i), "", DC_Qm, "kg/s", xidComp0+i, this, isResult|noFile|noSnap|NAN_OK);
    DDB.EndObject();
    }
  }

// --------------------------------------------------------------------------

flag CRtSpMdl::DataXchg(DataChangeBlk & DCB)
  {
  if (SpModel::DataXchg(DCB))
    return true;

  if (DCB.lHandle>=xidComp0 && DCB.lHandle<xidComp0+ODB.AssayCompCount())
    {
    const int Index = DCB.lHandle-xidComp0;
    const int EnIndex = ODB.AssayEnIndex(Index);
    CSD_DistDefn &DD = *SD_Defn.GetDist(0);
    double d = 0.0;
    for (int s=0; s<DD.NPriIds(); s++)
      {
      int SpId = DD.PriSzId(s);
      ASSERT(SpId>=0);
      if (ODB[SpId].OK())
        d += (VMass[SpId]*ODB[SpId].CompPercentage(EnIndex)/100.0);
      }
    DCB.D=d;
    return 1;
    }

    /*SpModel * pMdl = this;// Model();
    SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, false);
    if (pSz==NULL || !pSz->DistributionsExist())
      {
      DCB.D=dNAN; 
      }
    else
      {
      double d = 0.0;
      int EnIndex = DCB.lHandle-xidComp0;
      CSD_Distribution &D=pSz->Dist(0);
      for (int s=0; s<D.NPriIds(); s++)
        {
        int SpId = D.PriSzId(s);
        ASSERT(SpId>=0);
        if (ODB[SpId].OK())
          {
          double ddd=ODB[SpId].CompPercentage(EnIndex)/100.0;
          double dd=SpMass(SpId);
          d += (dd*ddd);
          }
        }
      DCB.D=d;
      return 1;
      }
    }*/
  return 0;
  }

//---------------------------------------------------------------------------

flag CRtSpMdl::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=SpModel::ValidateData(VDB);
  
  return OK;
  }

//--------------------------------------------------------------------------

flag CRtSpMdl::CIStrng(int No, pchar & pS)
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

