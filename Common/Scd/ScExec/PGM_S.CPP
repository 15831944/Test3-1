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
  Idf_SDB_SpCount = 1,
  Idf_SDB_PhaseCount,
  Idf_SDB_IPhaseCount,
  Idf_SDB_SolidPhase, 
  Idf_SDB_LiquidPhase,
  Idf_SDB_GasPhase,   
  Idf_SDB_PhaseTag,
  Idf_SDB_IPhaseTag,
  Idf_SDB_PhaseSymbol,
  Idf_SDB_IPhaseSymbol,
  Idf_SDB_FindSpecie,
  Idf_SDB_FindPhase,
  Idf_SDB_FindIPhase,
  Idf_SDB_Symbol,
  Idf_SDB_Tag,
  Idf_SDB_SpPhaseNo,
  Idf_SDB_SpIPhaseNo,
  Idf_SDB_SpPhaseTag,
  Idf_SDB_SpIPhaseTag,
  Idf_SDB_SpPhaseSymbol,
  Idf_SDB_SpIPhaseSymbol,
  Idf_SDB_IsSolid,
  Idf_SDB_IsLiquid,
  Idf_SDB_IsGas,
  Idf_SDB_MolecularWt,
  Idf_SDB_Density,
  Idf_SDB_mlHf,
  Idf_SDB_msHf,
  Idf_SDB_mlHz,
  Idf_SDB_msHz,
  Idf_SDB_mlHs,
  Idf_SDB_msHs,
  Idf_SDB_VapourP,
  Idf_SDB_VapourT,

  };

//---------------------------------------------------------------------------

GCSDB::GCSDB(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, &IB.m_VarMap, "GlblSDB", VarClassDefn)
  {
  AddVar  (IB, "Fidelity",      &GCLong);        
  
  AddFunct(IB, "SpecieCount",     0, false, Idf_SDB_SpCount);               // SpecieCount    ()
  AddFunct(IB, "PhaseCount",      0, false, Idf_SDB_PhaseCount);            // PhaseCount     ()
  AddFunct(IB, "IPhaseCount",     0, false, Idf_SDB_IPhaseCount);           // IPhaseCount    ()

  AddFunct(IB, "SolidPhase",      0, false, Idf_SDB_SolidPhase);            // SolidPhase     ()
  AddFunct(IB, "LiquidPhase",     0, false, Idf_SDB_LiquidPhase);           // LiquidPhase     ()
  AddFunct(IB, "GasPhase",        0, false, Idf_SDB_GasPhase);              // GasPhase     ()

  AddFunct(IB, "PhaseTag",        1, false, Idf_SDB_PhaseTag,     true);    // PhaseTag       (PhIndex)
  AddFunct(IB, "IPhaseTag",       1, false, Idf_SDB_IPhaseTag,    true);    // IPhaseTag      (IPhIndex)
  AddFunct(IB, "PhaseSymbol",     1, false, Idf_SDB_PhaseSymbol,  true);    // PhaseSymbol    (PhIndex)
  AddFunct(IB, "IPhaseSymbol",    1, false, Idf_SDB_IPhaseSymbol, true);    // IPhaseSymbol   (IPhIndex)
                                                                              
  AddFunct(IB, "FindSpecie",      1, true,  Idf_SDB_FindSpecie);            // FindSpecie     (Symbol/Tag)
  AddFunct(IB, "FindPhase",       1, true,  Idf_SDB_FindPhase);             // FindPhase      (PhaseName)
  AddFunct(IB, "FindIPhase",      1, true,  Idf_SDB_FindIPhase);            // FindIPhase     (IPhaseName)

  AddFunct(IB, "SpSymbol",        1, false, Idf_SDB_Symbol,       true);    // Symbol         (SpIndex)
  AddFunct(IB, "SpTag",           1, false, Idf_SDB_Tag,          true);    // Tag            (SpIndex)

  AddFunct(IB, "SpPhaseNo",       1, false, Idf_SDB_SpPhaseNo);             // SpPhaseNo      (SpIndex)
  AddFunct(IB, "SpIPhaseNo",      1, false, Idf_SDB_SpIPhaseNo);            // SpIPhaseNo     (SpIndex)
  AddFunct(IB, "SpPhaseTag",      1, false, Idf_SDB_SpPhaseTag,     true);  // SpPhaseTag     (SpIndex)
  AddFunct(IB, "SpIPhaseTag",     1, false, Idf_SDB_SpIPhaseTag,    true);  // SpIPhaseTag    (SpIndex)
  AddFunct(IB, "SpPhaseSymbol",   1, false, Idf_SDB_SpPhaseSymbol,  true);  // SpPhaseSymbol  (SpIndex)
  AddFunct(IB, "SpIPhaseSymbol",  1, false, Idf_SDB_SpIPhaseSymbol, true);  // SpIPhaseSymbol (SpIndex)
                                                                               
  AddFunct(IB, "SpIsSolid",       1, false, Idf_SDB_IsSolid);               // SpIsSolid      (SpIndex)
  AddFunct(IB, "SpIsLiquid",      1, false, Idf_SDB_IsLiquid);              // SpIsLiquid     (SpIndex)
  AddFunct(IB, "SpIsGas",         1, false, Idf_SDB_IsGas);                 // SpIsGas        (SpIndex)

  AddFunct(IB, "SpMolecularWt",   1, false, Idf_SDB_MolecularWt);           // MolecularWt    (SpIndex)
  AddFunct(IB, "SpDensity",       3, false, Idf_SDB_Density);               // Density        (SpIndex, temp, press)
  AddFunct(IB, "SpmlHf",          3, false, Idf_SDB_mlHf);                  // mlHf           (SpIndex, temp, press)
  AddFunct(IB, "SpmsHf",          3, false, Idf_SDB_msHf);                  // msHf           (SpIndex, temp, press)
  AddFunct(IB, "SpmlHz",          3, false, Idf_SDB_mlHz);                  // mlHz           (SpIndex, temp, press)
  AddFunct(IB, "SpmsHz",          3, false, Idf_SDB_msHz);                  // msHz           (SpIndex, temp, press)
  AddFunct(IB, "SpmlHs",          3, false, Idf_SDB_mlHs);                  // mlHs           (SpIndex, temp, press)
  AddFunct(IB, "SpmsHs",          3, false, Idf_SDB_msHs);                  // msHs           (SpIndex, temp, press)
  AddFunct(IB, "SpVapourP",       2, false, Idf_SDB_VapourP);               // VapourP        (SpIndex, temp)
  AddFunct(IB, "SpVapourT",       2, false, Idf_SDB_VapourT);               // VapourT        (SpIndex, temp)
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

const int OccCount=3;
static struct CPhNames {LPSTR m_Tag; LPSTR m_Sym; int m_BOT; } s_PhNames[OccCount]=
  {
    {"Solids", "S", BOT_Solid},
    {"Liquids", "L", BOT_Liquid},
    {"Gas", "G", BOT_Gas}
  };

double GCSDB::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CLclSDB * pLclSDB = (CLclSDB*)pSubClass;
  //long Index = Range(0L, IB.GetLParm(), SDB.Count()-1L);
  switch (FunctId)
    {
    case Idf_SDB_SpCount:       return SDB.Count();
    case Idf_SDB_PhaseCount:    return OccCount;
    case Idf_SDB_IPhaseCount:   return CDB.PhaseCount();

    case Idf_SDB_SolidPhase:    return BOT_Solid;
    case Idf_SDB_LiquidPhase:   return BOT_Liquid;
    case Idf_SDB_GasPhase:      return BOT_Gas;

    case Idf_SDB_FindSpecie:    return SDB.Find(IB.GetSParm());
    case Idf_SDB_FindPhase:     
      {
      Strng S(IB.GetSParm());
      for (int i=0; i<OccCount; i++)
        {
        if (S.XStrICmp(s_PhNames[i].m_Sym)==0 || S.XStrICmp(s_PhNames[i].m_Tag)==0)
          {
          ASSERT(i==s_PhNames[i].m_BOT);
          return i;
          }
        }
      return -1;
      }
    case Idf_SDB_FindIPhase:    
      {
      int i0=CDB.PhaseFirst();
      int iN=CDB.PhaseLast();
      Strng S(IB.GetSParm());
      for (int i=i0; i<iN; i++)
        {
        CPhaseInfo &I=CDB.m_PhInfo[i];
        if (S.XStrICmp(I.m_Sym())==0 || S.XStrICmp(I.m_Tag())==0)
          return i;
        }
      return -1;
      }
    case Idf_SDB_PhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<OccCount)
        {
        IB.PushStringValue(s_PhNames[i].m_Sym);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_PhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<OccCount)
        {
        IB.PushStringValue(s_PhNames[i].m_Tag);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_IPhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<CDB.PhaseCount())
        {
        IB.PushStringValue(CDB.m_PhInfo[i].m_Sym());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_IPhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<CDB.PhaseCount())
        {
        IB.PushStringValue(CDB.m_PhInfo[i].m_Tag());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpPhaseNo:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].m_eOcc;
      return -1;
      }
    case Idf_SDB_SpIPhaseNo:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].m_PhInx;
      return -1;
      }

    case Idf_SDB_SpPhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(s_PhNames[SDB[i].m_eOcc].m_Sym);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpPhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(s_PhNames[SDB[i].m_eOcc].m_Tag);
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpIPhaseSymbol: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(CDB.m_PhInfo[SDB[i].m_PhInx].m_Sym());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_SpIPhaseTag: 
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(CDB.m_PhInfo[SDB[i].m_PhInx].m_Tag());
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }

    case Idf_SDB_IsSolid:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].IsSol();
      return 0;
      }
    case Idf_SDB_IsLiquid:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].IsLiq();
      return 0;
      }
    case Idf_SDB_IsGas:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].IsGas();
      return 0;
      }

    case Idf_SDB_Symbol:        
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(SDB.Sym(i));      
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }
    case Idf_SDB_Tag:
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        {
        IB.PushStringValue(SDB.Tag(i));      
        return 1;
        }
      IB.PushStringValue("?");
      return 0;
      }

    case Idf_SDB_MolecularWt:   
      {
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].MoleWt();
      return 0;
      }
    case Idf_SDB_Density: 
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].Density(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }

    case Idf_SDB_mlHf:    
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].mlHf(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_msHf:    
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].msHf(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_mlHz:      
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].mlHz(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_msHz:   
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].msHz(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_mlHs:     
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].mlHs(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }
    case Idf_SDB_msHs:    
      {
      double P=IB.GetDParm(); 
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].msHs(pLclSDB->m_iFidelity, T, P, NULL, NULL);
      return 0;
      }

    case Idf_SDB_VapourP:   
      {
      double T=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].VapourP(pLclSDB->m_iFidelity, T);
      return 0;
      }
    case Idf_SDB_VapourT:  
      {
      double P=IB.GetDParm(); 
      int i=IB.GetLParm();
      if (i>=0 && i<SDB.Count())
        return SDB[i].VapourT(pLclSDB->m_iFidelity, P);
      return 0;
      }

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
