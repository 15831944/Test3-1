#include "stdafx.h"

#define __EVAPBLOCK_CPP
#include "EvapBlock.h"

//=========================================================================
//
//
//
//=========================================================================

XID xidEvapEnable = EvapXID(1);
XID xidEvapEqnNm  = EvapXID(2);

//============================================================================
//
//
//
//============================================================================

const pchar CEvapBlock::GroupName="EvapBlock";

IMPLEMENT_EVAP(CEvapBlock, "EvapNone", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "None",  " ");


CEvapBlock::CEvapBlock(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  //dHeatFlow=0.0;
  //TempKFeed=Std_T;
  //TempKProd=Std_T;
  }

//--------------------------------------------------------------------------

CEvapBlock::~CEvapBlock()
  {
  };

//--------------------------------------------------------------------------

void CEvapBlock::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "Evap", NULL, DDB_NoPage))
    {
    //DDB.Double ("",     "HeatFlow",   DC_Pwr,  "kW",    &dHeatFlow,  this, isResult|0);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CEvapBlock::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
//  dHeatFlow=0.0;
//  //TempKFeed=Qf.Temp();
//  //TempKProd=TempKFeed;
  };

void CEvapBlock::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
//  dHeatFlow=0.0;
//  //TempKFeed=Qf.Temp();
//  //TempKProd=TempKFeed;
  };

//============================================================================
//
//
//
//============================================================================

IMPLEMENT_EVAP(CEvBlk_Percentage, "EvapPerc", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Percentage",  " ");

CEvBlk_Percentage::CEvBlk_Percentage(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEvapBlock(pClass_, Tag_, pAttach, eAttach)
  {
  //dHperQm = 0.0;
  }

//--------------------------------------------------------------------------

CEvBlk_Percentage::~CEvBlk_Percentage()
  {
  }

//--------------------------------------------------------------------------

void CEvBlk_Percentage::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "Evap", NULL, DDB_NoPage))
    {
    //DDB.Double ("",     "LossPerQm",  DC_HMs, "kJ/kg", &dHperQm,    this, isParm);
    //DDB.Double ("",     "HeatFlow",   DC_Pwr,  "kW",    &dHeatFlow,  this, isResult|0);
    //DDB.Double ("TempFeed",     "",   DC_T,    "C",     &TempKFeed,  this, isResult);//|noFileAtAll);
    //DDB.Double ("TempProd",     "",   DC_T,    "C",     &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CEvBlk_Percentage::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEvBlk_Percentage::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
//  dHeatFlow=0.0;
//  //TempKFeed=Qf.Temp();
//  //TempKProd=TempKFeed;
  };

void CEvBlk_Percentage::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
//  dHeatFlow=0.0;
//  //TempKFeed=Qf.Temp();
//  //TempKProd=TempKFeed;
  };

//============================================================================
//
//
//
//============================================================================

//CEvapBase::CEvapBase(void)
//  {
//  }
//
//CEvapBase::~CEvapBase(void)
//  {
//  }

flag CEvapBase::Open(TagObjClass * pEvapClass, flag Fixed)
  {
  m_fEnabled=True;

  if (m_pEvapB && (pEvapClass==NULL || m_pEvapB->Class()==pEvapClass))
    return True;

  delete m_pEvapB;
  m_fFixed=Fixed;

  if (pEvapClass)
    m_pEvapB=(CEvapBlock*)pEvapClass->Construct(NULL, "Evap", m_pNd, TOA_Embedded);//pNd);
  else
    m_pEvapB=(CEvapBlock*)CEvapBlockClass.Construct(NULL, "Evap", m_pNd, TOA_Embedded);//pNd);
  m_pNd->StructureChanged(NULL);

  m_fEnabled=m_pEvapB!=NULL;
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void CEvapBase::Close()
  {
  if (m_fFixed)
    return;
  m_fEnabled=False;
  //if (pEvapClass==NULL)
  //  return;
  ////pGSM->StructureChanged(NULL);
  ////pGSM->Gammas.SetSize(0);
  ////pGSM->IOAlpha.SetSize(0);
  //delete pEvapClass;
  //pEvapClass=NULL;
  };
//--------------------------------------------------------------------------

void CEvapBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
#if WITHBLOCKEVALUATOR
  DDB.Byte("",     "Evaporator",  DC_,    "",     xidEvapEnable,  m_pNd, Flags, GetOnOffValLst());
#else
  DDB.CheckBoxBtn("",     "Evaporator",  DC_,    "",     xidEvapEnable,  m_pNd, Flags, DDBOnOff);
#endif
  }

//--------------------------------------------------------------------------

void CEvapBase::BuildDataDefn(DataDefnBlk &DDB, char* pTag, char* pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (pTag==NULL)
    pTag="Eval";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(SHM_All, m_fEnabled);

  if (Enabled())//pHL)
    {
    if (DDB.BeginStruct(m_pNd, pTag, pTagComment, PageIs))
      {
      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CEvapBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidEvapEqnNm  , m_pNd,m_fFixed ? 0 : isParm|SetOnChange, DDB0());

      if (m_pEvapB)
        {
        DDB.SuppressNextBlock();
        m_pEvapB->BuildDataDefn(DDB);
        }
      }
    DDB.EndStruct();
    }
  DDB.SetVisibility(Old);
  };

//--------------------------------------------------------------------------

flag CEvapBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidEvapEnable:
      if (DCB.rB)
        {
        if (*DCB.rB)
          Open(*DCB.rB);
        else
          Close();
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidEvapEqnNm:
      if (DCB.rpC && !m_fFixed)
        {
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CEvapBlockClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          Open(pC);
        else
          Close();
        }
      DCB.pC = m_pEvapB ? m_pEvapB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//=========================================================================
//
//
//
//=========================================================================
