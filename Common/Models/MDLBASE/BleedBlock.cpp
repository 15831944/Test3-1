#include "stdafx.h"

#define __XBLOCKBLOCK_CPP
#include "BleedBlock.h"

#if WITHBLOCKEVALUATOR

//=========================================================================
//
//
//
//=========================================================================

XID xidAdjustEnable = AdjustXID(1);
XID xidAdjustMdlNm  = AdjustXID(2);

//============================================================================
//
//
//
//============================================================================

const pchar CBleedBlock::GroupName="AdjustBlock";

IMPLEMENT_BLEEDBLOCK(CBleedBlock, "BB_None", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "None",  " ");


CBleedBlock::CBleedBlock(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  m_pAdjustBase = NULL;
  }

//--------------------------------------------------------------------------

CBleedBlock::~CBleedBlock()
  {
  };

//--------------------------------------------------------------------------

void CBleedBlock::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, NULL/*"Evap"*/, NULL, DDB_NoPage))
    {
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CBleedBlock::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
  };

void CBleedBlock::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  };

//============================================================================
//
//
//
//============================================================================

//#define VER1 01
//
//const byte Dest_Mix     = -2;
//const byte Dest_Discard = -1;
//const byte Dest_IO1     = 0; //????????????????
//
//============================================================================
//
//
//
//============================================================================

//#define VER1 01
//
//const byte Dest_Mix     = -2;
//const byte Dest_Discard = -1;
//const byte Dest_IO1     = 0; //????????????????
//
IMPLEMENT_BLEEDBLOCK(CXBlk_Bleed, "BB_Simple", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Simple",  " ");

CXBlk_Bleed::CXBlk_Bleed(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CBleedBlock(pClass_, Tag_, pAttach, eAttach)
  {
//#if VER1
//  m_Components.SetSize(gs_CDB.DDBCompListVapLiq.Length());
//  for (int i=0; i<m_Components.GetSize(); i++)
//    {
//    m_Components[i].m_CIndex   = gs_CDB.DDBCompListVapLiq[i].m_lVal;
//    m_Components[i].m_Dest     = 0;
//    m_Components[i].m_Fraction = 0.0;
//    }
//#endif
  m_Type=Type_TotFrac;
  m_Frac=0;
  m_QmBleed=0;
  }

//--------------------------------------------------------------------------

CXBlk_Bleed::~CXBlk_Bleed()
  {
  }

//--------------------------------------------------------------------------

//static DDBValueLst DDBDestinations[] =
//  {
//    { Dest_Mix      , "Mixture"},
//    { Dest_Discard  , "Discard"},
//    { 0 }
//  };

void CXBlk_Bleed::BuildDataDefn(DataDefnBlk& DDB)
  {

  if (DDB.BeginStruct(this, "AdjustQm", NULL, DDB_NoPage))
    {
    static DDBValueLst DDBType[] =
      {
        {Type_TotFrac, "Frac"},
        {Type_TotQm,   "Qm"},
        {}
      };
    DDB.Text(" ");
    DDB.Long  ("", "Type",        DC_,     "%",     (long*)&m_Type, this, isParm, DDBType);
    DDB.Double("", "Fraction",    DC_Frac, "%",     &m_Frac,        this, isParm);
    DDB.Double("", "Qm",          DC_Qm,   "kg/s",  &m_QmBleed,     this, isParm);
//#if VER1
//    if (DDB.BeginArray(this, "Comp", "EVB_Comps", m_Components.GetSize()))
//      {
//      for (int i=0; i<m_Components.GetSize(); i++)
//        {
//        LPTSTR Tg=gs_CDB[m_Components[i].m_CIndex].SymOrTag();
//        if (DDB.BeginElement(this, Tg, NULL, i))
//          {
//          DDB.Byte  ("", "Destination", DC_,     "",  &m_Components[i].m_Dest,      this, isParm, DDBDestinations);
//          DDB.Double("", "Fraction",    DC_Frac, "%", &m_Components[i].m_Fraction,  this, isParm);
//          }
//        }
//      }
//    DDB.EndArray();
//#else
//    DDB.Int("CompCount",     "",   DC_, "", xidCompCount,    this, isParm);
//    if (DDB.BeginArray(this, "Comp", "EVB_Comps", m_Components.GetSize()))
//      {
//      for (int i=0; i<m_Components.GetSize(); i++)
//        {
//        if (DDB.BeginElement(this, i, NULL, i))
//          {
//          DDB.Long  ("", "Component", DC_,     "",  &m_Components[i].m_CIndex,    this, isParmStopped, &gs_CDB.DDBCompListDashVapLiq);
//          DDB.Double("", "Fraction",  DC_Frac, "%", &m_Components[i].m_Fraction,  this, isParm);
//          }
//        }
//      }
//    DDB.EndArray();
//#endif
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CXBlk_Bleed::DataXchg(DataChangeBlk & DCB)
  {
//#if VER1
//#else
//  switch (DCB.lHandle)
//    {
//    case xidCompCount:
//      if (DCB.rL)
//        {
//        int Old=m_Components.GetSize();
//        m_Components.SetSize(*DCB.rL);
//        for (int i=Old; i<m_Components.GetSize(); i++)
//          {
//          m_Components[i].m_CIndex=-1;
//          m_Components[i].m_Fraction=0.0;
//          }
//        }
//      DCB.L=m_Components.GetSize();
//      return 1;
//    }
//#endif
  return 0;
  }

//--------------------------------------------------------------------------

flag CXBlk_Bleed::ValidateData(ValidateDataBlk & VDB)
  {
//#if VER1
//#else
//  for (int i=0; i<m_Components.GetSize(); i++)
//    {
//    int iComp=m_Components[i].m_CIndex;
//    if (iComp>=0)
//      {
//      for (int j=i+1; j<m_Components.GetSize(); j++)
//        {
//        if (iComp==m_Components[j].m_CIndex)
//          {
//          m_Components.RemoveAt(j);
//          j--;
//          }
//        }
//      }
//    }
//#endif
//
//  for (int i=0; i<m_Components.GetSize(); i++)
//    m_Components[i].m_Fraction=Range(0.0, m_Components[i].m_Fraction, 1.0);

  return CBleedBlock::ValidateData(VDB); 
  }

//--------------------------------------------------------------------------

void CXBlk_Bleed::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
  //SpConduit & Discard = m_pAdjustBase->DiscardCd();
  //Discard.QZero();
  //SpMArray OldVap;

  //Qf.SetPress(Po);
  //double H=Qf.totHf();
  //for (int i=0; i<m_Components.GetSize(); i++)
  //  {
  //  CAdjustComp  & EC = m_Components[i];
  //  CComponent & C  = gs_CDB[EC.m_CIndex];
  //  int iLiq=C.LiqPhInx();
  //  int iVap=C.VapPhInx();
  //  double Liq=Qf.VMass[iLiq];
  //  double Vap=Qf.VMass[iVap];
  //  double D=Liq*EC.m_Fraction;

  //  Qf.SetVValue(iLiq, Liq-D);
  //  Qf.SetVValue(iVap, Vap+D);

  //  Discard.SetVValue(iVap, D);
  //  OldVap.SetVValue(iVap, Vap);
  //  }
  //Qf.Set_totHf(H);

  //double T=Qf.Temp();
  //double P=Qf.Press();
  //for (int i=0; i<m_Components.GetSize(); i++)
  //  {
  //  CAdjustComp  & EC = m_Components[i];
  //  int iVap = gs_CDB[EC.m_CIndex].VapPhInx();
  //  switch (EC.m_Dest)
  //    {
  //    case Dest_Mix:
  //      // Qf is at correct conditions
  //      Discard.SetVValue(iVap, 0.0);
  //      break;
  //    case Dest_Discard:
  //      Qf.SetVValue(iVap, OldVap[iVap]); // reset to original vapour mass
  //      break;
  //    }
  //  }
  //Qf.SetTempPress(T, P);
  //Discard.SetTempPress(T, P);
  };

void CXBlk_Bleed::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(Qf, Po, FinalTEst);
  };

//============================================================================
//
//
//
//============================================================================

CBleedBase::CBleedBase(TaggedObject * pAttach, int Index) : CBlockEvalBase(BEId_Adj)
  { 
  m_pAdjustB=NULL; 
  m_pNd=pAttach; 
  m_fEnabled=false;
  m_fFixed=false; 
  m_Index=Index; 
  };
CBleedBase::~CBleedBase()  
  { 
  delete m_pAdjustB; 
  };

Strng CBleedBase::Name()
  {
  Strng S;
  S.Set("B%i", m_Index+1);
  return S;
  }

flag CBleedBase::Open(TagObjClass * pAdjustClass, flag Fixed)
  {
  m_fEnabled=True;

  if (m_pAdjustB && (pAdjustClass==NULL || m_pAdjustB->Class()==pAdjustClass))
    return True;

  delete m_pAdjustB;
  m_fFixed=Fixed;

  if (pAdjustClass)
    m_pAdjustB=(CBleedBlock*)pAdjustClass->Construct(NULL, "Bleed", m_pNd, TOA_Embedded);//pNd);
  else
    m_pAdjustB=(CBleedBlock*)CBleedBlockClass.Construct(NULL, "Bleed", m_pNd, TOA_Embedded);//pNd);
  m_pNd->StructureChanged(NULL);
  m_pAdjustB->m_pAdjustBase=this;

  m_fEnabled=m_pAdjustB!=NULL;
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void CBleedBase::Close()
  {
  if (m_fFixed)
    return;
  m_fEnabled=False;
  };
//--------------------------------------------------------------------------

void CBleedBase::Add_OnOff(DataDefnBlk &DDB, dword Flags, int UserInfo)
  {
  Strng S;
  S.Set("Bleed%i", m_Index+1);
  DDB.PushUserInfo(UserInfo);
  DDB.Byte(S(), "",  DC_,    "",     xidAdjustEnable,  m_pNd, Flags, GetOnOffValLst());
  DDB.PopUserInfo();
  }

//--------------------------------------------------------------------------

void CBleedBase::BuildDataDefn(DataDefnBlk &DDB, char* pTag, char* pTagComment, DDBPages PageIs, dword UserInfo)
  {
  DDB.PushUserInfo(UserInfo);
  if (pTag==NULL)
    pTag="Xx";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(SHM_All, m_fEnabled);
  if (Enabled())//pHL)
    {
    if (DDB.BeginObject(m_pNd, Name()(), "EB_Bleed", pTagComment, PageIs))
      {
      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CBleedBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidAdjustMdlNm  , m_pNd,m_fFixed ? 0 : isParm|SetOnChange, DDB0());

      if (m_pAdjustB)
        {
        DDB.SuppressNextBlock();
        m_pAdjustB->BuildDataDefn(DDB);
        }
      }
    DDB.EndObject();
    }
  DDB.SetVisibility(Old);
  DDB.PopUserInfo();
  };

//--------------------------------------------------------------------------

flag CBleedBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidAdjustEnable:
      if (DCB.rB)
        {
        if (*DCB.rB)
          Open(*DCB.rB);
        else
          Close();
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidAdjustMdlNm:
      if (DCB.rpC && !m_fFixed)
        {
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CBleedBlockClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          Open(pC);
        else
          Close();
        }
      DCB.pC = m_pAdjustB ? m_pAdjustB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//=========================================================================
//
//
//
//=========================================================================

#endif