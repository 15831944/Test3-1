//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __CTRLLINK_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "ctrllink.h"

#include "scd_wm.h"

#define MYASSERT(x) /*ASSERT(x)*/

//==========================================================================
//
//
//
//==========================================================================

IOAreaRec TwoCIOAreaList[] =
  {{"Input",         "In" , CtrlIOId(0), LIO_In0 , nc_CLnk, 1,1, IOGRP(1)},
   {"Input",         "I"  , CtrlIOId(0), LIO_In  , nc_CLnk, 1,1, IOGRP(1)},
   {"",              "Src", CtrlIOId(0), LIO_In  , nc_CLnk, 1,1, IOGRP(1)},
   {"",              "S"  , CtrlIOId(0), LIO_In  , nc_CLnk, 1,1, IOGRP(1)},
   {"Output",        "Out", CtrlIOId(1), LIO_Out0, nc_CLnk, 1,1, IOGRP(1)},
   {"Output",        "O"  , CtrlIOId(1), LIO_Out , nc_CLnk, 1,1, IOGRP(1)},
   {"",              "Dst", CtrlIOId(1), LIO_Out , nc_CLnk, 1,1, IOGRP(1)},
   {"",              "D"  , CtrlIOId(1), LIO_Out , nc_CLnk, 1,1, IOGRP(1)},
   {NULL}};


IMPLEMENT_CONTROLLINK(CCtrlLink, "CtrlLink", "", "C", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                   "IO - CtrlLink",
                   "IO - CtrlLink.")

CCtrlLink::CCtrlLink(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_CLnk, TwoCIOAreaList);
  fActiveHoldOK_Dyn=true;
  m_bOn = 1;
  m_dValue = 0;
  }

//--------------------------------------------------------------------------

CCtrlLink::~CCtrlLink()
  {
  }

//--------------------------------------------------------------------------

void CCtrlLink::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CCtrlLink::PostConnect(int IONo)
  {
  FlwNode::PostConnect(IONo);
  }

//--------------------------------------------------------------------------

const word idmCount     = 1000;
const word idmCheckBtn  = 1001;
const word idmCfgTags   = 1100;

const word NoOfCfgTags  = 10;

//--------------------------------------------------------------------------

void CCtrlLink::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CCtrlLink", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",     "", DC_,   "", &m_bOn,        this, isParmStopped, DDBYesNo);
  DDB.Double     ("Value",  "", DC_,   "", &m_dValue,     this, isResult);
  DDB.String     ("SrcIO",  "", DC_,   "", xidSrcIOTag,   this, isResult|noFile|noSnap);
  DDB.String     ("DstIO",  "", DC_,   "", xidDstIOTag,   this, isResult|noFile|noSnap);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CCtrlLink::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  return False;
  }

//--------------------------------------------------------------------------

flag CCtrlLink::ValidateData(ValidateDataBlk & VDB)
  {
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

void CCtrlLink::EvalCtrlInitialise()
  {
  if (NoCIOs()>=2 && m_bOn && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {
    }
  }

//--------------------------------------------------------------------------

void CCtrlLink::EvalCtrlStrategy()
  {
  if (fDoDbgBrk)
    { int xxx=0;}
  if (NoCIOs()>=2 && m_bOn && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {
    m_dValue=CIO_Value(0);
    MYASSERT(_CrtCheckMemory());
    SetCIO_Value(1, m_dValue);
    MYASSERT(_CrtCheckMemory());
    int i0=IOIONo_Rmt(0);
    CTagRef * p0=i0<Nd_Rmt(0)->m_IOTagRefs.GetSize() ? Nd_Rmt(0)->m_IOTagRefs[i0] : NULL;

    //dbgpln("CCtrlLink: CTagRef Src %08x  %s", p0, sTag());

    MYASSERT(_CrtCheckMemory());
    if (p0)
      p0->IncGets(); 
    MYASSERT(_CrtCheckMemory());
    int i1=IOIONo_Rmt(1);
    CTagRef * p1=i1<Nd_Rmt(1)->m_IOTagRefs.GetSize() ? Nd_Rmt(1)->m_IOTagRefs[i1] : NULL;

    //dbgpln("CCtrlLink: CTagRef Dst %08x  %s", p1, sTag());
             
    MYASSERT(_CrtCheckMemory());
    if (p1)
      p1->IncSets(); 
    MYASSERT(_CrtCheckMemory());
    }                                  
  else
    {
    m_dValue=0;
    SetCIO_Value(1, m_dValue);
    MYASSERT(_CrtCheckMemory());
    }
  }

//--------------------------------------------------------------------------

void CCtrlLink::EvalDiscrete()
  {
  }

//--------------------------------------------------------------------------

dword CCtrlLink::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=/*FNS_IsCLnk|*/(m_bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CCtrlLink::CIStrng(int No, pchar & pS)
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
