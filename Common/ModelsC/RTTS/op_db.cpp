//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __OP_DB_CPP
#include "sp_dbextra.h"
#include "op_db.h"
#include "..\..\..\SMDK\Include\md_base.h"
#include "..\..\..\SMDK\Include\md_defn.h"
#include "oreprops.h"
//#include "optoff.h"

//===========================================================================

OrePropDataBase ODB; //global Ore Properties Database

//===========================================================================

/*OreProp::OreProp() : SpecieProp()
  {
  }*/

//---------------------------------------------------------------------------

/*char* OreProp::Name()
  {
  return SDB[iSId].Tag();
  }*/

//---------------------------------------------------------------------------
//===========================================================================

OrePropDataBase::OrePropDataBase() : CSpeciePropDataBase()
  {
  pCfgItems = NULL;
  }

//---------------------------------------------------------------------------

OrePropDataBase::~OrePropDataBase()
  {
  if (pCfgItems)
    delete []pCfgItems;
  pCfgItems = NULL;
  /*todo: check DLL unload sequence...
  RT_OreProps* pOP = (RT_OreProps*)GetGlobalOther("RioTintoOreProps");
  delete pOP;
  SetGlobalOther("RioTintoOreProps", NULL);
  */
  }

//---------------------------------------------------------------------------

/*enum ePropNames { enMaterial, enDescription, enLump, enGroup, enDensity, enDI, 
                  enTheorFe, enTheorSi, enTheorAl, enTheorP, enTheorMn, enTheorLOI, 
                  enK1C, enA, enb, enAb, enRockA, enUTS, enCritUTS, enPPV, enb80, enUCS };*/
/*char* ColNames[] = { "Material", "Description", "Lump", "Group", "Density", "DI", 
                     "TheorFe", "TheorSi", "TheorAl", "TheorP", "TheorMn", "TheorLOI", 
                     "K1C", "A", "b", "Ab", "Rock A", "UTS", "Crit UTS", "PPV", "b80", "UCS", NULL };*/
class CfgItemInitData
  {
  public:
    char*       pName;
    short       iWidth;
    byte        bString:1,
                bDisplay:1;
  };

CfgItemInitData RqdPropDetails[] = { 
  { "Material",        8,  true,  true },
  { "Description",    24,  true,  true },
/*  { "Lump",            5, false,  true },
  { "Group",           5,  true,  true },
  { "Density",         6, false,  true },
  { "DI",              5, false,  true },
  { "K1C",             6, false,  true },
  { "A",               6, false,  true },
  { "b",               6, false,  true },
  { "Ab",              6, false,  true },
  { "Rock A",          6, false,  true },
  { "UTS",             6, false,  true },
  { "Crit UTS",        6, false,  true },
  { "PPV",             6, false,  true },
  { "b80",             6, false,  true },
  { "UCS",             6, false,  true },*/
  };

//---------------------------------------------------------------------------

void OrePropDataBase::CopyToSMDK()
  {
  RT_OreProps* pOP = (RT_OreProps*)GetGlobalOther("RioTintoOreProps");
  if (pOP)
    delete pOP;
  pOP = new RT_OreProps;
  pOP->SetOrePropsCount(ODB.GetCount());
  RT_OreProp* pOPs = pOP->GetOreProps(); //a copy of the database for SMDK purposes

  int OptCnt = 0;
  for (int i=0; i<PropCfg.PropCount(); i++)
    {
    if (PropCfg.Optional(i) && !PropCfg.String(i))
      OptCnt++;
    }
  pOP->SetOtherPropsCount(OptCnt);
  if (OptCnt>0)
    {
    OptCnt = 0;
    for (int i=0; i<PropCfg.PropCount(); i++)
      {
      if (PropCfg.Optional(i) && !PropCfg.String(i))
        pOP->SetOtherPropName(OptCnt++, PropCfg.Name(i));
      }
    }

  for (int i=0; i<ODB.GetCount(); i++)
    {
    if (ODB[i].OK())
      {
      pOPs[i].bOK=true;
      strncpy(pOPs[i].sMaterial,    ODB[i].Material(),    sizeof(pOPs[i].sMaterial));
      if (ODB[i].Description())
        strncpy(pOPs[i].sDescription, ODB[i].Description(), sizeof(pOPs[i].sDescription));
      else
        pOPs[i].sDescription[0]=0;
      /*strncpy(pOPs[i].sGroup,       ODB[i].Group(),       sizeof(pOPs[i].sGroup));
      pOPs[i].dLump     = ODB[i].Lump   ();
      pOPs[i].dDensity  = ODB[i].Density();
      pOPs[i].dDI       = ODB[i].DI     ();
      pOPs[i].dK1C      = ODB[i].K1C    ();
      pOPs[i].dA        = ODB[i].A      ();
      pOPs[i].db        = ODB[i].b      ();
      pOPs[i].dAb       = ODB[i].Ab     ();
      pOPs[i].dRockA    = ODB[i].RockA  ();
      pOPs[i].dUTS      = ODB[i].UTS    ();
      pOPs[i].dCritUTS  = ODB[i].CritUTS();
      pOPs[i].dPPV      = ODB[i].PPV    ();
      pOPs[i].db80      = ODB[i].b80    ();
      pOPs[i].dUCS      = ODB[i].UCS    ();*/
      
      if (OptCnt>0)
        {
        OptCnt = 0;
        for (int j=0; j<PropCfg.PropCount(); j++)
          {
          if (PropCfg.Optional(j) && !PropCfg.String(j))
            {
            pOPs[i].pOtherProps[OptCnt] = ODB[i].dProp[j];
            OptCnt++;
            }
          }
        }
      }
    }

  SetGlobalOther("RioTintoOreProps", (void*)pOP);
  }

//---------------------------------------------------------------------------

int OrePropDataBase::InitAndLoad()
  {
  CArray<CString, CString&> OptionalNames;
  CString S;
  CProfINIFile PF(CfgFile());
  Strng Entry;
  Strng s("x");
  int OptCnt = 0;
  while (strcmp(s(), "xXx")!=0)
    {
    Entry.Set("Assay%02d", OptCnt);
    s = PF.RdStr("AssayColumns", Entry(), "xXx");
    if (strcmp(s(), "xXx")!=0)
      {
      S = s();
      OptionalNames.Add(S);
      OptCnt++;
      }
    }
  int FldCnt = 0;
  s = "x";
  while (strcmp(s(), "xXx")!=0)
    {
    Entry.Set("FieldName%02d", FldCnt++);
    s = PF.RdStr("ExtraSpecieProperties", Entry(), "xXx");
    if (strcmp(s(), "xXx")!=0)
      {
      S = s();
      OptionalNames.Add(S);
      OptCnt++;
      }
    }

  int i;
  int RqdCnt = sizeof(RqdPropDetails)/sizeof(RqdPropDetails[0]);
  const int TtlCnt = RqdCnt+OptCnt;

  pCfgItems = new SpeciePropCfgItem[TtlCnt];
  for (i=0; i<RqdCnt; i++)
    pCfgItems[i].Set(RqdPropDetails[i].pName, RqdPropDetails[i].iWidth, RqdPropDetails[i].bString, RqdPropDetails[i].bDisplay);
  for (i=RqdCnt; i<TtlCnt; i++)
    pCfgItems[i].Set((char*)(const char*)(OptionalNames[i-RqdCnt]), 8, 0, 1, 1);

  int RetCode = Init(pCfgItems, TtlCnt);
  if (RetCode==0)
    {
    CString mdbName;
    s = PF.RdStr("ExtraSpecieProperties", "Database", "xXx");
    if (strcmp(s(), "xXx")==0)
      {
      s = CfgDBFileName();
      PF.WrStr("ExtraSpecieProperties", "Database", s());
      }
    if (s.Find('\\')>=0)
      mdbName = s();
    else
      {
      mdbName = CfgFiles();
      mdbName += s();
      }
    Strng TableName;
    TableName = PF.RdStr("ExtraSpecieProperties", "TableName", "xXx");
    if (strcmp(TableName(), "xXx")==0)
      {
      TableName = "OreProperties";
      PF.WrStr("ExtraSpecieProperties", "TableName", TableName());
      }

    RetCode = Load((char*)(const char*)mdbName, TableName());
    
    if (RetCode==0)
      {
      int AssayCnt = 0;
      for (i=RqdCnt; i<TtlCnt; i++)
        {
        if (pCfgItems[i].bFound)
          AssayCnt++;
        else
          LogWarning("SpeciePropDB", 0, "Field %s not found in table OreProperties!", pCfgItems[i].Name());
        }
      AssayCompNames.SetSize(AssayCnt);
      AssayCompEnIndex.SetSize(AssayCnt);
      AssayCnt = 0;
      for (i=RqdCnt; i<TtlCnt; i++)
        {
        if (pCfgItems[i].bFound)
          {
          AssayCompNames[AssayCnt] = pCfgItems[i].Name();
          AssayCompEnIndex[AssayCnt] = i;
          AssayCnt++;
          }
        }

      for (i=0; i<ODB.GetCount(); i++)
        {
        OreProp & OP = ODB[i];
        if (OP.OK())
          LogNote("SpeciePropDB", 0, "Loaded %s (%s)", OP.Tag(), OP.Description());
        }
      SDB.SetExtraProps(this); //assume SDB is valid

      CopyToSMDK();
      }

    }
  return RetCode;
  }

//---------------------------------------------------------------------------
//===========================================================================


