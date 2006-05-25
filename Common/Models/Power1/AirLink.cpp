//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __AIRLINK_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "airlink.h"

#include "scd_wm.h"

//==========================================================================
//
//
//
//==========================================================================

static IOAreaRec TwoEIOAreaList[] =
  {{"Input",         "In" , ElecIOId(0), LIO_In , nc_ELnk, 1,50, IOGRP(1)},
   {"Input",         "I"  , ElecIOId(0), LIO_In , nc_ELnk, 1,50, IOGRP(1)},
   {"",              "Src", ElecIOId(0), LIO_In , nc_ELnk, 1,50, IOGRP(1)},
   {"",              "S"  , ElecIOId(0), LIO_In , nc_ELnk, 1,50, IOGRP(1)},
   {"Output",        "Out", ElecIOId(1), LIO_Out, nc_ELnk, 1,50, IOGRP(1)},
   {"Output",        "O"  , ElecIOId(1), LIO_Out, nc_ELnk, 1,50, IOGRP(1)},
   {"",              "Dst", ElecIOId(1), LIO_Out, nc_ELnk, 1,50, IOGRP(1)},
   {"",              "D"  , ElecIOId(1), LIO_Out, nc_ELnk, 1,50, IOGRP(1)},
   {NULL}};


IMPLEMENT_AIRLINK(CAirLink, "AirLink", "", "A", TOC_ALL|TOC_GRP_POWERDIST|TOC_POWERDIST,
                  "Air:Link",
                  "Air:Link.")

CAirLink::CAirLink(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_ELnk, TwoEIOAreaList);
  fActiveHoldOK_Dyn=true;
  m_bOn = 1;
  //m_dValue = 0;
  }

//--------------------------------------------------------------------------

CAirLink::~CAirLink()
  {
  }

//--------------------------------------------------------------------------

void CAirLink::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CAirLink::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "A_Link", NULL, DDB_NoPage);
  DDB.Text("");
  //DDB.CheckBoxBtn("On",     "", DC_,   "", &m_bOn,        this, isParm);
  //DDB.Double     ("Value",  "", DC_,   "", &m_dValue,     this, isResult);
  DDB.String     ("SrcIO",  "", DC_,   "", xidSrcIOTag,   this, isResult|noFile|noSnap);
  DDB.String     ("DstIO",  "", DC_,   "", xidDstIOTag,   this, isResult|noFile|noSnap);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CAirLink::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  return False;
  }

//--------------------------------------------------------------------------

flag CAirLink::ValidateData(ValidateDataBlk & VDB)
  {
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CAirLink::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  }

//--------------------------------------------------------------------------

void CAirLink::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  }

//--------------------------------------------------------------------------

void CAirLink::EvalDiscrete()
  {
  }

//--------------------------------------------------------------------------

flag CAirLink::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  //MdlAction M0 = {0, MAT_State, 0/*!PSB.On()*/, "On", 1};
  //Acts.SetSize(0);
  //Acts.SetAtGrow(0, M0);
  return true;
  };

//--------------------------------------------------------------------------

flag CAirLink::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
      //PSB.SetOn(Act.iValue);
      break;
    }
  return true;
  };

//--------------------------------------------------------------------------

dword CAirLink::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=(m_bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CAirLink::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tBad external tag references"; return 1;
    case  2: pS="E\tErrors in evaluating functions"; return 1;
    case  3: pS="E\tEngineering units invalid"; return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================
