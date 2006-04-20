//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#define  __CTRL_IO_CPP
#include "ctrl_io.h"

//===========================================================================

//===========================================================================
//
//
//
//===========================================================================

//void CIO_Logic::DDBAdd_OnOff(DataDefnBlk & DDB, pchar pTag, pflag pOn, flag bOffisBlank)
//  {
//  DDB.Bool("",    pTag,  DC_,    "",     pOn,     this, isParm);
//  DDB.IndexedStrValue((int)False, bOffisBlank ? "" : "Off");
//  DDB.IndexedStrValue((int)True, "On");
//  }
//
////---------------------------------------------------------------------------
//
//void CIO_Logic::DDBAdd_YesNo(DataDefnBlk & DDB, pchar pTag, pflag pYes, flag bNoisBlank)
//  {
//  DDB.Bool("",    pTag,  DC_,    "",     pYes,     this, isParm);
//  DDB.IndexedStrValue((int)False, bNoisBlank ? "" : "No");
//  DDB.IndexedStrValue((int)True, "Yes");
//  }

//---------------------------------------------------------------------------

pchar CIO_Logic::LogicName()
  {
  static Strng S;
  Class()->GetShortDesc(S);
  return S();
  }

//===========================================================================
//
//
//
//===========================================================================

CIO_Blk::CIO_Blk(pchar Group, pchar Name, pTaggedObject pAttach, CPwrUser * pPwr)
  {
  pGroup=Group;
  m_pPwr=pPwr;
  pLogic=CreateLogic(Name, pAttach);
  };

//--------------------------------------------------------------------------

CIO_Blk::~CIO_Blk()
  {
  if (pLogic)
    delete pLogic;
  };

//--------------------------------------------------------------------------
   
CIO_Logic * CIO_Blk::CreateLogic(pchar Name_, pTaggedObject pAttach)
  { 
  CIO_Logic * pLog=NULL;
  if (Name_ && strlen(Name_)>0)
    pLog=(CIO_Logic *)TagObjClass::Construct(pGroup, Name_, NULL, "Logic", pAttach, pAttach ? TOA_Embedded : TOA_Free);
  if (pLog)
    pLog->m_pPwr=m_pPwr;
  return pLog;
  };

// -------------------------------------------------------------------------

void CIO_Blk::ChangeLogic(pchar Name_)
  { 
  ASSERT(pLogic);
  if (Name_ && (_stricmp(Name_, pLogic->ClassId())!=0))
    {
    ASSERT(pLogic->pAttachedTo);
    pLogic->StructureChanged(pLogic->pAttachedTo);
    CIO_Logic * pLog=CreateLogic(Name_, pLogic->pAttachedTo);
    if (pLog)
      {
      pLog->CopyFrom(pLogic);
      if (pLogic)
        delete pLogic;
      pLogic=pLog;
      }
    }
  };

//--------------------------------------------------------------------------

void CIO_Blk::BuildDataDefn(DataDefnBlk & DDB, pTaggedObject pOwnObj, pchar pName, dword dwUserInfo, pchar pComment, int HandleOffset)
  {
  ASSERT(pGroup);
  if (DDB.BeginStruct(pOwnObj, pName, pComment, DDB_NoPage, dwUserInfo))
    {
    DDBValueLstMem DDB0;
    TagObjClass::GetSDescValueLst(pGroup, DDB0);
    DDB.String  ("Type",      "",       DC_    , "",      xidLogicNm+HandleOffset  , pOwnObj  ,isParm|SetOnChange, DDB0());
    if (pLogic)
      pLogic->BuildDataDefn(DDB);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CIO_Blk::DataXchg(DataChangeBlk & DCB, int HandleOffset)
  {
  if (pLogic && pLogic->DataXchg(DCB))
    return True;

  switch (DCB.lHandle-HandleOffset)
    {
    case xidLogicNm:
      {
      if (DCB.rpC)
        {
        Strng S;
        for (int i=0; ;i++)
          {
          pTagObjClass pC=TagObjClass::FindClassIndexed(pGroup, i);
          if (pC==NULL)
            break;
          else 
            {
            pC->GetShortDesc(S);
            if (S.XStrICmp(DCB.rpC)==0)
              {
              ChangeLogic(pC->ClassId());
              break;
              }
            }
          }
        }
      DCB.pC=LogicName();
      return True;
      }
    }
  return False;
  };

//===========================================================================
//
//
//
//===========================================================================

CIO_DLogic::CIO_DLogic() 
  {
  pbSet=NULL;
  };

//---------------------------------------------------------------------------

void CIO_DLogic::AttachValue(pflag pSet) 
  {
  pbSet=pSet;
  };

//---------------------------------------------------------------------------

void CIO_DLogic::Set(flag Set) 
  {
  *pbSet=Set;
  };

//---------------------------------------------------------------------------

void CIO_DLogic::BuildDataDefn(pTaggedObject pObj, pchar pTag, DataDefnBlk & DDB)
  {
  DDB.Bool("",    "On",  DC_,    "",     pbSet,   pObj, isParm, DDBOnOff);
  };

//---------------------------------------------------------------------------

flag CIO_DLogic::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  };

//---------------------------------------------------------------------------

void CIO_DLogic::EvalDiscrete(FlwNode* pNd)
  {
  };

void CIO_DLogic::EvalCtrlActions(FlwNode* pNd)
  {
  };

void CIO_DLogic::EvalCtrlStrategy(FlwNode* pNd)
  {
  };

//===========================================================================

CIO_Digital::CIO_Digital()
  {
  bSet=1;
  pLogic=NULL;
  };

//---------------------------------------------------------------------------

void CIO_Digital::AttachLogic(pCIO_DLogic Logic)
  {
  pLogic=Logic; 
  if (pLogic)
    pLogic->AttachValue(&bSet);
  };

//---------------------------------------------------------------------------

pCIO_DLogic CIO_Digital::DetachLogic()
  {
  pCIO_DLogic RetLogic=pLogic;
  pLogic=NULL;
  return RetLogic;
  };

//---------------------------------------------------------------------------

flag CIO_Digital::Set(flag SetIt) 
  {
  if (pLogic)
    pLogic->Set(SetIt);
  else
    bSet=(flag)SetIt; 
  return bSet;
  };
  
//---------------------------------------------------------------------------

void CIO_Digital::BuildDataDefn(pTaggedObject pObj, pchar pTag, DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(pObj, pTag, NULL, DDB_NoPage))
    {
    if (pLogic)
      pLogic->BuildDataDefn(pObj, pTag, DDB);
    else
      {
      DDB.CheckBoxBtn("",      "On",     DC_,    "",     &bSet,  pObj, isParm, DDBOnOff);
      //DDB.Bool("",    "On",  DC_,    "",     &bSet,          pObj, isParm, DDBOnOff);
      } 
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CIO_Digital::DataXchg(DataChangeBlk & DCB)
  {
  if (pLogic)
    return pLogic->DataXchg(DCB);
  return 0;
  };

//---------------------------------------------------------------------------

void CIO_Digital::EvalDiscrete(FlwNode* pNd)
  {
  };

void CIO_Digital::EvalCtrlActions(FlwNode* pNd)
  {
  };

void CIO_Digital::EvalCtrlStrategy(FlwNode* pNd)
  {
  };

//===========================================================================

CIO_ALogic::CIO_ALogic() 
  {
  pdVal=NULL;
  };

//---------------------------------------------------------------------------

void CIO_ALogic::AttachValue(pdouble pVal) 
  {
  pdVal=pVal;
  };

//---------------------------------------------------------------------------

void CIO_ALogic::Set(double Val) 
  {
  *pdVal=Val;
  };

//---------------------------------------------------------------------------

void CIO_ALogic::BuildDataDefn(pTaggedObject pObj, pchar pTag, DataDefnBlk & DDB)
  {
  DDB.Double("Signal",    "Y",  DC_Frac,    "%",     pdVal,          pObj, isParm);
  };

//---------------------------------------------------------------------------

flag CIO_ALogic::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  };

//---------------------------------------------------------------------------

void CIO_ALogic::EvalDiscrete(FlwNode* pNd)
  {
  };

void CIO_ALogic::EvalCtrlActions(FlwNode* pNd)
  {
  };

void CIO_ALogic::EvalCtrlStrategy(FlwNode* pNd)
  {
  };

//===========================================================================

CIO_Analog::CIO_Analog()
  {
  dVal=1.0;
  pLogic=NULL;
  };

//---------------------------------------------------------------------------

void CIO_Analog::AttachLogic(pCIO_ALogic Logic)
  {
  pLogic=Logic; 
  if (pLogic)
    pLogic->AttachValue(&dVal);
  };

//---------------------------------------------------------------------------

pCIO_ALogic CIO_Analog::DetachLogic()
  {
  pCIO_ALogic RetLogic=pLogic;
  pLogic=NULL;
  return RetLogic;
  };

//---------------------------------------------------------------------------

double CIO_Analog::Set(double Val) 
  {
  if (pLogic)
    pLogic->Set(Val);
  else
    dVal=Val; 
  return dVal;
  };
  
//---------------------------------------------------------------------------

void CIO_Analog::BuildDataDefn(pTaggedObject pObj, pchar pTag, DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(pObj, pTag, NULL, DDB_NoPage))
    {
    if (pLogic)
      pLogic->BuildDataDefn(pObj, pTag, DDB);
    else
      {
      DDB.Double("Signal",    "Y",  DC_Frac,    "%",     &dVal,          pObj, isParm);
      } 
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CIO_Analog::DataXchg(DataChangeBlk & DCB)
  {
  if (pLogic)
    return pLogic->DataXchg(DCB);
  return 0;
  };

//---------------------------------------------------------------------------

void CIO_Analog::EvalDiscrete(FlwNode* pNd)
  {
  };

void CIO_Analog::EvalCtrlActions(FlwNode* pNd)
  {
  };

void CIO_Analog::EvalCtrlStrategy(FlwNode* pNd)
  {
  };

//===========================================================================
//
//
//
//===========================================================================
