//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#define  __VLVLOGIC_CPP
#include "vlvlogic.h"
#include "power.h"

//===========================================================================
//
//
//
//===========================================================================

VL_Base::VL_Base(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CIO_Logic(pClass_, pTag, pAttach, eAttach)
  {
  m_bOn=1; // power on
  dPosition=1.0;
  dReqdPosition=1.0;
  dManualPosition=dNAN;
  dMinOpening=0;
  dMaxOpening=1;
  dLeakOpening=0;
  dPhysPosn=0.0;
  bReverseAction=false;
  };

//---------------------------------------------------------------------------

VL_Base::~VL_Base()
  {
  };

//---------------------------------------------------------------------------

flag VL_Base::CopyFrom(CIO_Logic * pOther)
  {
  VL_Base* pO=dynamic_cast<VL_Basic*>(pOther);
  if (pO)
    {
    m_bOn            = pO->m_bOn;
    dPosition        = pO->dPosition;
    dReqdPosition    = pO->dReqdPosition;
    dManualPosition  = pO->dManualPosition;
    dMinOpening      = pO->dMinOpening;
    dMaxOpening      = pO->dMaxOpening;    
    dLeakOpening     = pO->dLeakOpening;   
    return true;              
    }
  return false;
  };

//--------------------------------------------------------------------------

void VL_Base::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  ID=pAttachedTo->FullObjTag();
  ID+=".Posn.Reqd";
  Tg=pAttachedTo->FullObjTag();
  Tg+=".Posn.Reqd";
  };

//===========================================================================
//
//
//
//===========================================================================
/*#D:#T:Position valve logic
#X:#h<General Description>#nThis valve position logic equation allows the user 
to set the actual position of the actuator within the valve. This strategy does 
not distinguish between the actual position of the valve's actuator and the required 
position, ie there is no time delay between the required and actual.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : On or off. This indicates whether the valve is operational or not.#n
#i<N/O> : Yes or no. This determines whether or not the valve remains open when not 
operational.#n
#i<Reqd> : The required position (0-100%).#n
#n#n
#h<Associated variables>#n
#i<Actual> : The current actual position of the actuator of the valve (0-100%).#n
#i<Open> : Is the valve currently fully open (Actual position > 99.9%).#n
#i<Closed> : Is the valve currently fully closed (Actual position < 0.1%).#n
#n
#n#h<Other>#n
Short name:VL_Basic#n
Model type:Valve Position Logic#n
#G:Valve Position Logic
*/

IMPLEMENT_VLVPOSLOGIC(VL_Basic, "VL_Basic", "", TOC_SYSTEM, "Position", "Position Valve Logic") 

VL_Basic::VL_Basic(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  VL_Base(pClass_, pTag, pAttach, eAttach)
  {
  bNormClosed=1;
  bOpenCmd=0;
  bCloseCmd=0;
  bLatchCmds=0;
  iLatchState=0;
  bOpened=0;
  bClosed=0;
  };

//---------------------------------------------------------------------------

VL_Basic::~VL_Basic()
  {
  };

//---------------------------------------------------------------------------

void VL_Basic::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.CheckBoxBtn("State",         "On",     DC_,     "",  &m_bOn,           this, isParm, DDBOnOff);
  DDB.CheckBoxBtn("N/C",           "",       DC_,     "",  &bNormClosed,     this, isParm, DDBYesNo);
  DDB.Double     ("Actual",        "Actual", DC_Frac, "%", &dPosition,       this, isResult|0);
  DDB.Double     ("Required",      "Reqd",   DC_Frac, "%", &dReqdPosition,   this, isParm);
  DDB.CheckBoxBtn("OpenCmd",       "",       DC_,     "",  &bOpenCmd,        this, isParm|0,      DDBOnOff);
  DDB.CheckBoxBtn("CloseCmd",      "",       DC_,     "",  &bCloseCmd,       this, isParm|0,      DDBOnOff);
  DDB.CheckBoxBtn("LatchCmds",     "",       DC_,     "",  &bLatchCmds,      this, isParm|0,      DDBOnOff);
  DDB.Long       ("LatchState",    "",       DC_,     "",  &iLatchState,     this, isParm|InitHidden);
  DDB.CheckBoxBtn("Open",          "",       DC_,     "",  &bOpened,           this, isResult|0,      DDBOnOff);
  DDB.CheckBoxBtn("Closed",        "",       DC_,     "",  &bClosed,         this, isResult|0,      DDBOnOff);
  DDB.Double     ("ManualOvrRide", "Manual", DC_Frac, "%", &dManualPosition, this, isParm|NAN_OK);
  DDB.Float      ("MinOpening",    "",       DC_Frac, "%", &dMinOpening,     this, isParm|InitHidden);
  DDB.Float      ("MaxOpening",    "",       DC_Frac, "%", &dMaxOpening,     this, isParm|InitHidden);
  DDB.Float      ("LeakOpening",   "",       DC_Frac, "%", &dLeakOpening,    this, isParm|InitHidden);
  DDB.Double     ("Physical",      "",       DC_Frac, "%", &dPhysPosn,      this, isResult|0);
  DDB.CheckBoxBtn("ReverseAction", "",       DC_,     "",  &bReverseAction,  this, isParm|0,      DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  }

//---------------------------------------------------------------------------

flag VL_Basic::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  }

//---------------------------------------------------------------------------

void VL_Basic::EvalCtrlActions(FlwNode* pFNode)
  {
  if (bLatchCmds)
    {
    if (bCloseCmd)
      iLatchState=-1;
    else if (bOpenCmd)
      iLatchState=1;
    }
  else
    iLatchState=0;

  flag LclOpen=bOpenCmd;
  flag LclClose=bCloseCmd;

  if (!LclOpen && !LclClose)
    {
    if (iLatchState>0)
      LclOpen=1;
    else if (iLatchState<0)
      LclClose=1;
    }
  
  if (!(LclOpen && LclClose) && (LclOpen || LclClose))
    {
    if (LclOpen)
      dReqdPosition=1;
    else 
      dReqdPosition=0;
    }
  
  if (!On())// || !m_Pwr.On())
    dPosition=(bNormClosed ? 0.0 : 1.0);
  else 
    dPosition=Valid(dManualPosition) ? dManualPosition : dReqdPosition;

  dMinOpening=Range(0.0f, dMinOpening, 0.99f);
  dMaxOpening=Range(dMinOpening, dMaxOpening, 1.0f);
  dLeakOpening  = Range(0.0f, dLeakOpening, 0.9f);   

  dPosition=Range(double(dMinOpening), dPosition, double(dMaxOpening));
  
  bOpened=(dPosition>=dMaxOpening-0.001);
  bClosed=(dPosition<=dMinOpening+0.001);

  if (bOpened)
    bOpenCmd=0;
  if (bClosed)
    bCloseCmd=0;

  if (bOpened || bClosed)
    iLatchState=0;
  };

//---------------------------------------------------------------------------

double VL_Basic::ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  if (pFRB && pFRB->What()!=FBReg_Off)
    dPosition=pFRB->Ctrl();
  else
    dPosition=Range(0.0, dPosition, 1.0);

  dPhysPosn = dLeakOpening+(bReverseAction?(1.0-dPosition):dPosition)/(1.0-dLeakOpening);
  return dPhysPosn;   
  //return dLeakOpening+dPosition/(1.0-dLeakOpening);   
  };

//---------------------------------------------------------------------------

void VL_Basic::SetActualPosition(double Posn)
  {
  if (On())
    dPosition=(Posn-dLeakOpening)*(1.0-dLeakOpening);
  };

//---------------------------------------------------------------------------

flag VL_Basic::On()
  {
  return m_bOn && PwrOn();
  };

//---------------------------------------------------------------------------

void VL_Basic::SetOn(int On)
  {
  m_bOn=On!=0;
  };

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Pos & Stroke valve logic
#X:#h<General Description>#nThis valve logic equation incorporates the lag that 
exists between the required position of the valve and the actual position of the 
valve. The stroke time of the valve is the maximum lag time taken by the actuator 
to move the valve from the open position to the closed position.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : On or off. This indicates whether the valve is operational or not.#n
#i<N/C> : Yes or no. This determines whether or not the valve remains closed when 
not operational.#n
#i<Reqd> : The required position (0-100%).#n
#i<StrkTime> : The time taken for the actuator of the valve to move from a fully open 
position to the fully closed position.#n
#n#n
#h<Associated variables>#n
#i<Actual> : The current actual position of the actuator of the valve (0-100%).#n
#i<Open> : Is the valve currently fully open (Actual position > 99.9%).#n
#i<Closed> : Is the valve currently fully closed (Actual position < 0.1%).#n
#n
#n#h<Other>#n
Short name:VL_Strok#n
Model type:Valve Position Logic#n
#G:Valve Position Logic
*/

IMPLEMENT_VLVPOSLOGIC(VL_Stroke, "VL_Strok", "", TOC_SYSTEM, "Pos & Stroke", 
                      "Position Valve Logic including Stroke Time") 

VL_Stroke::VL_Stroke(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  VL_Basic(pClass_, pTag, pAttach, eAttach)
  {
  dStrokeTime=10; // 10 seconds
  };

//---------------------------------------------------------------------------

VL_Stroke::~VL_Stroke()
  {
  };

//---------------------------------------------------------------------------

void VL_Stroke::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.CheckBoxBtn("State", "On",  DC_, "", &m_bOn, this, isParm, DDBOnOff);
  DDB.CheckBoxBtn("N/C",   "",    DC_, "", &bNormClosed, this, isParm, DDBYesNo);
  DDB.Double("Actual",     "Actual",    DC_Frac, "%",    &dPosition,     this, isResult|0);
  DDB.Double("Required",   "Reqd",      DC_Frac, "%",    &dReqdPosition, this, isParm);
  DDB.Double("StrokeTime", "StrkTime",  DC_Time, "s",    &dStrokeTime,   this, isParm);
  DDB.CheckBoxBtn("", "OpenCmd",  DC_, "", &bOpenCmd,        this, isParm|0,      DDBOnOff);
  DDB.CheckBoxBtn("", "CloseCmd", DC_, "", &bCloseCmd,       this, isParm|0,      DDBOnOff);
  DDB.CheckBoxBtn("LatchCmds",     "",       DC_,     "",  &bLatchCmds,      this, isParm|0,      DDBOnOff);
  DDB.Long       ("LatchState",    "",       DC_,     "",  &iLatchState,     this, isParm|InitHidden);
  DDB.CheckBoxBtn("", "Open",     DC_, "", &bOpened, this, isResult|0, DDBOnOff);
  DDB.CheckBoxBtn("", "Closed",   DC_, "", &bClosed, this, isResult|0, DDBOnOff);
  DDB.Double("ManualOvrRide", "Manual",    DC_Frac, "%",    &dManualPosition, this, isParm|NAN_OK);
  DDB.Float ("MinOpening",    "",       DC_Frac, "%", &dMinOpening,     this, isParm|InitHidden);
  DDB.Float ("MaxOpening",    "",       DC_Frac, "%", &dMaxOpening,     this, isParm|InitHidden);
  DDB.Float ("LeakOpening",   "",       DC_Frac, "%", &dLeakOpening,    this, isParm|InitHidden);
  DDB.Double("Physical",      "",          DC_Frac, "%", &dPhysPosn,      this, isResult|0);
  DDB.CheckBoxBtn("ReverseAction", "",     DC_,     "",  &bReverseAction,  this, isParm|0,      DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  };

//---------------------------------------------------------------------------

flag VL_Stroke::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  };

//---------------------------------------------------------------------------

void VL_Stroke::EvalCtrlActions(FlwNode* pFNode)
  {
  if (bLatchCmds)
    {
    if (bCloseCmd)
      iLatchState=-1;
    else if (bOpenCmd)
      iLatchState=1;
    }
  else
    iLatchState=0;

  flag LclOpen=bOpenCmd;
  flag LclClose=bCloseCmd;

  if (!LclOpen && !LclClose)
    {
    if (iLatchState>0)
      LclOpen=1;
    else if (iLatchState<0)
      LclClose=1;
    }

  double dReqdPos, dDiffPos;

  if (!(LclOpen && LclClose) && (LclOpen || LclClose))
    {
    double dCmdPos;
    if (LclOpen)
      dCmdPos=1;
    else 
      dCmdPos=0;

    dDiffPos = dCmdPos-dReqdPosition;
    // limit the diff by the stroketime
    dDiffPos = Sign(dDiffPos)*Min(fabs(dDiffPos),(ICGetTimeInc()/GTZ(dStrokeTime)));
    // apply it;
    dReqdPosition+=dDiffPos;
    }
  
  // Decide the required position
  if (!On())//m_bOn || !m_Pwr.On())
    dReqdPos=(bNormClosed ? 0.0 : 1.0);
  else 
    dReqdPos=Valid(dManualPosition) ? dManualPosition : dReqdPosition;
      
  // the difference
  dDiffPos = dReqdPos-dPosition;
  // limit the diff by the stroketime
  dDiffPos = Sign(dDiffPos)*Min(fabs(dDiffPos),(ICGetTimeInc()/GTZ(dStrokeTime)));
  // apply it;
  dPosition+=dDiffPos;

  dMinOpening=Range(0.0f, dMinOpening, 0.99f);
  dMaxOpening=Range(dMinOpening, dMaxOpening, 1.0f);
  dLeakOpening  = Range(0.0f, dLeakOpening, 0.9f);   

  dPosition=Range(double(dMinOpening), dPosition, double(dMaxOpening));
  
  bOpened=(dPosition>=dMaxOpening-0.001);
  bClosed=(dPosition<=dMinOpening+0.001);

  if (bOpened)
    bOpenCmd=0;
  if (bClosed)
    bCloseCmd=0;

  if (bOpened || bClosed)
    iLatchState=0;
  };

//---------------------------------------------------------------------------

double VL_Stroke::ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  return VL_Basic::ActualPosition(pFNode, pFRB);
  };

//===========================================================================
//
//
//
//===========================================================================

//const byte PLSS_FwdOnly           = 0;
//const byte PLSS_FwdRevLogic       = 1;
//const byte PLSS_FwdRevRegulation  = 2;
//
//static DDBValueLst DDBFwdRevMode[]={
//  {PLSS_FwdOnly,          "FwdOnly"},
//  {PLSS_FwdRevLogic,      "Logic"},
//  {PLSS_FwdRevRegulation, "Regulation"},
//  {0}};
  
IMPLEMENT_VLVPOSLOGIC(VL_StrokeOC, "VL_StrokeOC", "", TOC_SYSTEM, 
                      "Stroke Opn/Cls", "Stroke Open/Close") 

VL_StrokeOC::VL_StrokeOC(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  VL_Base(pClass_, pTag, pAttach, eAttach)
  {
  dReqdPosition=1.0; // 100%
  dOpenTime=10; // 10 seconds
  dCloseTime=10; // 10 seconds

  m_bActive=1;
  bActMemory=1;
  bNormClosed=1;//bNormOpen;
  bOpenCmd=0;
  bCloseCmd=0;
  //bLatchCmds;
  //iLatchState;
  bLoLimit=false;
  bHiLimit=false;

  iMode=0;
  bOpenInv=false;
  bCloseInv=false;
  dOpenTime=10;
  dCloseTime=10;
  dMapLo=0;
  dMapHi=1;
  dMapPosn=0;
  dLoLimit=0.01f;
  dHiLimit=0.99f;

  };

//---------------------------------------------------------------------------

VL_StrokeOC::~VL_StrokeOC()
  {
  };

//---------------------------------------------------------------------------

static DDBValueLst DDBVlvMode[]={
  {0,    "Active", MDD_Default },
  {1,    "Command(Latch)"},
  {2,    "Command(State)"},
  {0}};

void VL_StrokeOC::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Byte("Mode", "", DC_, "", &iMode, this, isParm, DDBVlvMode);
//  DDB.Byte("FwdRevMode", "", DC_, "", &iFwdRev, this, isParm, DDBFwdRevMode  );
  switch (iMode)
    {
    case 0:
      DDB.CheckBoxBtn("Open",         "",  DC_, "",  &m_bOn,   this, isParm, DDBOnOff);
      break;
    case 1:
    case 2:
      DDB.CheckBoxBtn("Permissive",   "",  DC_, "",  &m_bOn,     this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("OpenCmd",      "",  DC_, "",  &bOpenCmd,  this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("CloseCmd",     "",  DC_, "",  &bCloseCmd, this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("OpenInvert",   "",  DC_, "",  &bOpenInv,  this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("CloseInvert",  "",  DC_, "",  &bCloseInv, this, isParm, DDBOnOff);
      break;
    }

  DDB.CheckBoxBtn("Active",  "",  DC_, "",  &m_bActive,   this, 0, DDBOnOff);

  DDB.Double("Actual",        "Actual",    DC_Frac, "%", &dMapPosn,       this, isResult|0);
  DDB.Double("Required",      "Reqd",      DC_Frac, "%", &dReqdPosition,  this, isParm);
  DDB.Double("OpenTime",      "OpenTime",  DC_Time, "s", &dOpenTime,      this, isParm);
  DDB.Double("CloseTime",     "CloseTime", DC_Time, "s", &dCloseTime,     this, isParm);
  DDB.Double("Manual",        "",          DC_Frac, "%", &dManualPosition,this, isParm|NAN_OK);
  DDB.Float ("MinOpening",    "",          DC_Frac, "%", &dMinOpening,    this, isParm);
  DDB.Float ("MaxOpening",    "",          DC_Frac, "%", &dMaxOpening,    this, isParm);
  DDB.Float ("LeakOpening",   "",          DC_Frac, "%", &dLeakOpening,   this, isParm|InitHidden);
  DDB.Double("Physical",      "",          DC_Frac, "%", &dPhysPosn,      this, isResult|0);
  DDB.CheckBoxBtn("ReverseAction", "",     DC_,     "",  &bReverseAction,  this, isParm|0,      DDBOnOff);

  DDB.Bool  ("LoOpen",        "",          DC_,     "",  &bLoLimit,       this, isResult);
  DDB.Bool  ("HiOpen",        "",          DC_,     "",  &bHiLimit,       this, isResult);
  DDB.Float ("LoOpenLimit",   "",          DC_Frac, "%", &dLoLimit,       this, isParm|InitHidden);
  DDB.Float ("HiOpenLimit",   "",          DC_Frac, "%", &dHiLimit,       this, isParm|InitHidden);

  DDB.CheckBoxBtn("ActMemory",  "",        DC_,     "",  &bActMemory,     this, InitHidden, DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  };

//---------------------------------------------------------------------------

flag VL_StrokeOC::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  };

//---------------------------------------------------------------------------

flag VL_StrokeOC::On()
  {
  return m_bOn && PwrOn();
  };

//---------------------------------------------------------------------------

void VL_StrokeOC::SetOn(int On)
  {
  m_bOn=On!=0;
  };

//---------------------------------------------------------------------------

void VL_StrokeOC::EvalCtrlActions(FlwNode* pFNode)
  {
  flag ActReqd=m_bActive;
  switch (iMode)
    {
    case 0:
      ActReqd=bOpenInv ? !m_bOn : m_bOn;
      ClrCI(1);
      break;
    case 1:
      {
      flag Open = bOpenInv ? !bOpenCmd : bOpenCmd;
      flag Close  = bCloseInv  ? !bCloseCmd  : bCloseCmd;
      if (Open && !Close && m_bOn)
        ActReqd=true;
      else if (!m_bOn || (Close && !Open))
        ActReqd=false;
      SetCI(1, Close && Open);
      break;
      }
    case 2:
      {
      flag Open = bOpenInv ? !bOpenCmd : bOpenCmd;
      flag Close  = bCloseInv  ? !bCloseCmd  : bCloseCmd;
      if (Open && !Close && m_bOn)
        ActReqd=true;
      else 
        ActReqd=false;
      ClrCI(1);
      break;
      }
    }

//  if (ActReqd && (iFailMode & PLBFail_NoOpen)==0)
//    m_bActive=ActReqd;
//  else if (!ActReqd && (iFailMode & PLBFail_NoClose)==0)
  m_bActive=ActReqd;
  
  bActMemory=m_bActive;

  double dReqdOpn, dDiffOpn;

//  switch (iFwdRev)
//    {
//    case PLSS_FwdOnly:
  dReqdPosition=Range(0.0, dReqdPosition, 1.0);
  dReqdOpn=dReqdPosition;
  if (Valid(dManualPosition))
    dManualPosition=Range(0.0, dManualPosition, 1.0);
//      break;
//    case PLSS_FwdRevLogic:
//      dReqdPosition=Range(0.0, dReqdPosition, 1.0);
//      dReqdOpn=dReqdPosition*(bRunRev ? -1 : 1);
//      if (Valid(dManualPosition))
//        dManualPosition=Range(0.0, dManualPosition, 1.0);
//      break;
//    case PLSS_FwdRevRegulation:
//      dReqdPosition=Range(-1.0, dReqdPosition, 1.0);
//      dReqdOpn=dReqdPosition;
//      if (Valid(dManualPosition))
//        dManualPosition=Range(-1.0, dManualPosition, 1.0);
//      break;
//    }
  // Decide the required position
  if (!On())
    dReqdOpn=0.0;
      
  // the difference
  dDiffOpn = dReqdOpn-dPosition;
  // limit the diff by the stroketime
  if (fabs(dPosition+dDiffOpn*0.01)>fabs(dPosition))
    dDiffOpn = Sign(dDiffOpn)*Min(fabs(dDiffOpn), ICGetTimeInc()/GTZ(dOpenTime));
  else
    dDiffOpn = -Sign(dDiffOpn)*Max(-fabs(dDiffOpn),-ICGetTimeInc()/GTZ(dCloseTime));

  // apply it;
  dPosition+=dDiffOpn;

  dPosition=Range(-1.0, dPosition, 1.0);
  //if (!m_bActive)
  //  dMapPosn=0.0;
  //else 
  //  {
  if (Valid(dManualPosition))
    dMapPosn=dManualPosition;
  else
    dMapPosn=dPosition;
  dMapPosn=dMapLo+dMapPosn*(dMapHi-dMapLo);
  //  }
  
  bLoLimit = dPosition <= dLoLimit;   
  bHiLimit = dPosition >= dHiLimit;   
  };

//---------------------------------------------------------------------------

double VL_StrokeOC::ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  if (pFRB && pFRB->What()!=FBReg_Off)
    dMapPosn=pFRB->Ctrl();
  else
    dMapPosn=Range(0.0, dMapPosn, 1.0);

  dPhysPosn = dLeakOpening+(bReverseAction?(1.0-dMapPosn):dMapPosn)/(1.0-dLeakOpening);
  return dPhysPosn;   
  };

//===========================================================================
//===========================================================================
//===========================================================================
