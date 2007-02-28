//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __PGM_S_CPP
#include "sc_defs.h"
#include "pgm_e.h"
#include "pgm_s.h"
#include "sp_db.h"
 
//===========================================================================

class CLclSDB
  {
  public:
    CLclSDB()
      {
      m_iFidelity=1;
      }

  public:
    long m_iFidelity;
  };

//===========================================================================

enum 
  {
  Idf_SDB_Count = 1,
  Idf_SDB_Find,
  Idf_SDB_SolidPhases,
  Idf_SDB_LiquidPhases,
  Idf_SDB_GasPhases,
  Idf_SDB_Symbol,
  Idf_SDB_Tag,
  Idf_SDB_Phase,
  Idf_SDB_MolecularWt,
  Idf_SDB_Density,
  Idf_SDB_mlHf,
  Idf_SDB_msHf,
  Idf_SDB_mlHz,
  Idf_SDB_msHz,
  Idf_SDB_mlHs,
  Idf_SDB_msHs,
  Idf_SDB_VapourP,
  Idf_SDB_VapourT                        
  };

//---------------------------------------------------------------------------

GCSDB::GCSDB(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "GlblSDB", VarClassDefn)
  {
  AddVar  (IB, "Fidelity",      &GCLong);        
  
  AddFunct(IB, "Count",         0, false, Idf_SDB_Count);                 // Count          ()
  AddFunct(IB, "Find",          1, true,  Idf_SDB_Find);                  // Find           (Symbol/Tag)

  AddFunct(IB, "SolidPhases",   0, false, Idf_SDB_SolidPhases);           // SolidPhases    ()
  AddFunct(IB, "LiquidPhases",  0, false, Idf_SDB_LiquidPhases);          // LiquidPhases   ()
  AddFunct(IB, "GasPhases",     0, false, Idf_SDB_GasPhases);             // GasPhases      ()
  
  AddFunct(IB, "Symbol",        1, false, Idf_SDB_Symbol,       true);    // Symbol         (index)
  AddFunct(IB, "Tag",           1, false, Idf_SDB_Tag,          true);    // Tag            (index)
  AddFunct(IB, "Phase",         1, false, Idf_SDB_Phase);                 // Phase          (index)
  AddFunct(IB, "MolecularWt",   1, false, Idf_SDB_MolecularWt);           // MolecularWt    (index, temp, press)
  AddFunct(IB, "Density",       3, false, Idf_SDB_Density);               // Density        (index, temp, press)
  AddFunct(IB, "mlHf",          3, false, Idf_SDB_mlHf);                  // mlHf           (index, temp, press)
  AddFunct(IB, "msHf",          3, false, Idf_SDB_msHf);                  // msHf           (index, temp, press)
  AddFunct(IB, "mlHz",          3, false, Idf_SDB_mlHz);                  // mlHz           (index, temp, press)
  AddFunct(IB, "msHz",          3, false, Idf_SDB_msHz);                  // msHz           (index, temp, press)
  AddFunct(IB, "mlHs",          3, false, Idf_SDB_mlHs);                  // mlHs           (index, temp, press)
  AddFunct(IB, "msHs",          3, false, Idf_SDB_msHs);                  // msHs           (index, temp, press)
  AddFunct(IB, "VapourP",       2, false, Idf_SDB_VapourP);               // VapourP        (index, temp)
  AddFunct(IB, "VapourT",       2, false, Idf_SDB_VapourT);               // VapourT        (index, temp)
  }

//---------------------------------------------------------------------------

void GCSDB::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new CLclSDB;
  }
  
//---------------------------------------------------------------------------

void GCSDB::Done(pGCClassVar pClassVar)
  {
  delete (CLclSDB*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCSDB::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CLclSDB * pLclSDB = (CLclSDB*)pSubClass;
  //long Index = Range(0L, IB.GetLParm(), SDB.Count()-1L);
  switch (FunctId)
    {
    case Idf_SDB_Count:         return SDB.Count();
    case Idf_SDB_Find:          return SDB.Find(IB.GetSParm());

    case Idf_SDB_SolidPhases:   return som_Sol;
    case Idf_SDB_LiquidPhases:  return som_Liq;
    case Idf_SDB_GasPhases:     return som_Gas;

    case Idf_SDB_Symbol:        IB.PushStringValue(SDB.Sym(Range(0L, IB.GetLParm(), SDB.Count()-1L)));      return 0.0;
    case Idf_SDB_Tag:           IB.PushStringValue(SDB.Tag(Range(0L, IB.GetLParm(), SDB.Count()-1L)));      return 0.0;
    
    case Idf_SDB_Phase:         return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].m_PhMsk;
    case Idf_SDB_MolecularWt:   return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].MoleWt();
    case Idf_SDB_Density:       return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].Density(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);

    case Idf_SDB_mlHf:          return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].mlHf(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);
    case Idf_SDB_msHf:          return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].msHf(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);
    case Idf_SDB_mlHz:          return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].mlHz(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);
    case Idf_SDB_msHz:          return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].msHz(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);
    case Idf_SDB_mlHs:          return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].mlHs(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);
    case Idf_SDB_msHs:          return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].msHs(pLclSDB->m_iFidelity, IB.GetDParm(), IB.GetDParm(), NULL, NULL);

    case Idf_SDB_VapourP:       return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].VapourP(pLclSDB->m_iFidelity, IB.GetDParm());
    case Idf_SDB_VapourT:       return SDB[Range(0L, IB.GetLParm(), SDB.Count()-1L)].VapourT(pLclSDB->m_iFidelity, IB.GetDParm());

    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCSDB::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  CLclSDB * pLcl = (CLclSDB*)pSubClass;
  GCFCBAppendRec(FCB, '{', "GlblSDB", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Fidelity", pLcl->m_iFidelity);
  GCFCBAppendRec(FCB, '}', "GlblSDB", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCSDB::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  CLclSDB * pLcl = (CLclSDB*)pSubClass;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        {
        long iF;
        if (Buff.Try("Fidelity", iF)) 
          {
          pLcl->m_iFidelity=iF;
          break;
          }
        break;
        }
      }
    }
  return True;
  }
  
//===========================================================================
