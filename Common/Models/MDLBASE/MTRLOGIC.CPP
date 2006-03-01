//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#define  __MTRLOGIC_CPP
#include "mtrlogic.h"
#include "power.h"

//#include "optoff.h"

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Power & Speed pump logic
#X:#h<General Description>#nThis logic allows the user to change the pump state 
between ON and OFF, as well as allowing the pump speed to be ramped up or down.  This 
functionality is useful for pump control and when doing PLC testing using SysCAD.  Both 
variables can be changed while SysCAD is running.
#nIf the pump has been configured using the pump database, the pump speed will not exceed 
the maximum speed specified by the user.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : On or off. Is the pump operational or not.#n
#i<Y> : Required pump speed (0-100%).#n
#n
#n#h<Other>#n
Short name:PL_Basic#n
Model type:Pump Speed Logic#n
#G:Pump Speed Logic
*/

IMPLEMENT_PMPSPDLOGIC(PL_Basic, "PL_Basic", "", TOC_SYSTEM, "Power & Speed", "Power & Speed Motor Logic") 

PL_Basic::PL_Basic(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CIO_Logic(pClass_, pTag, pAttach, eAttach)
  {
  iMode=0;
  bOn=1;
  bStart=0;
  bStop=0;
  bStartInv=0;
  bStopInv=0;
  bStarted=0;
  bRunning=1;
  bTripped=0;

  m_dSpeedReg=0;
  m_dSpeed=0.0;
  m_dSpeedReqd=1.0;
  m_dManualSpeed=dNAN;
  bRunning=false;
  bTripped=false;

  m_bAutoRestart=true;
  iFailMode=PLBFail_None;
  bLoSpeed=1;
  bHiSpeed=0;
  dLoSpeedLimit=0.01f;
  dHiSpeedLimit=0.99f;
  };

//---------------------------------------------------------------------------

PL_Basic::~PL_Basic()
  {
  };

//---------------------------------------------------------------------------

static DDBValueLst DDBPmpStartMode[]={
  {0,    "OnOff",             MDD_Hidden},
  {0,    "On-Off",            MDD_Default|MDD_Hidden },
  {0,    "Simple",            MDD_Default },
  {1,    "Command(Latch)"     },
  {1,    "StartStop",         MDD_Hidden},
  {1,    "Start-Stop(Latch)", MDD_Hidden},
  {2,    "Command(State)"     },
  {2,    "Start-Stop(State)", MDD_Hidden},
  {0}};

static DDBValueLst DDBPmpFailMode[]={
  {PLBFail_None,                    "None"},
  {PLBFail_NoStart,                 "NoStart"},
  {PLBFail_NoStop,                  "NoStop"},
  {PLBFail_NoStart|PLBFail_NoStop,  "NoStartStop"},
  {0}};

void PL_Basic::BuildDataDefn(DataDefnBlk & DDB)
  {
  
  DDB.Byte("Mode", "", DC_, "", &iMode, this, isParm, DDBPmpStartMode);
  switch (iMode)
    {
    case 0:
      DDB.CheckBoxBtn("State",   "Run",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      break;
    case 1:
    case 2:
      DDB.CheckBoxBtn("Enable",     "",  DC_, "",  &bOn,      this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Start",      "",  DC_, "",  &bStart,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Stop",       "",  DC_, "",  &bStop,    this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StartInvert","",  DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StopInvert", "",  DC_, "",  &bStopInv, this, isParm, DDBOnOff);
      break;
    }
  DDB.CheckBoxBtn("Running",  "",  DC_, "",  &bRunning,   this, 0, DDBOnOff);
  DDB.CheckBoxBtn("Tripped",  "",  DC_, "",  &bTripped,   this, isParm, DDBOnOff);

  DDB.Double("LocalY", "Manual",    DC_Frac,    "%",     &m_dManualSpeed, this, isParm|NAN_OK);
  DDB.Double("Speed",  "Reqd",      DC_Frac,    "%",     &m_dSpeedReqd,   this, isParm);
  DDB.Double("",       "Actual",    DC_Frac,    "%",     &m_dSpeed,       this, isResult);
  DDB.Double("",       "Regulated", DC_Frac,    "%",     &m_dSpeedReg,    this, isResult);

  DDB.Bool  ("LoSpeed",      "",    DC_,        "",      &bLoSpeed,     this, isResult);
  DDB.Bool  ("HiSpeed",      "",    DC_,        "",      &bHiSpeed,     this, isResult);
  DDB.Float ("LoSpeedLimit", "",    DC_Frac,    "%",     &dLoSpeedLimit,this, isParm|InitHidden);
  DDB.Float ("HiSpeedLimit", "",    DC_Frac,    "%",     &dHiSpeedLimit,this, isParm|InitHidden);

  DDB.CheckBoxBtn("AutoReStart",  "",  DC_, "",  &m_bAutoRestart,   this, isParm, DDBOnOff);
  DDB.Byte("FailMode", "", DC_, "", &iFailMode, this, isParm, DDBPmpFailMode);
  DDB.CheckBoxBtn("RunMemory",  "",  DC_, "",  &bRunMemory,   this, InitHidden, DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  }


//---------------------------------------------------------------------------

flag PL_Basic::DataXchg(DataChangeBlk & DCB)
  {
  //if (m_pPwr && m_pPwr->DataXchg(DCB))
  //  return 1;
  return False;
  }

//---------------------------------------------------------------------------

flag PL_Basic::CopyFrom(CIO_Logic * pOther)
  {
  PL_Basic * pO=dynamic_cast<PL_Basic*>(pOther);
  if (pO)
    {
    iMode            = pO->iMode;
    bOn              = pO->bOn;
    bStart           = pO->bStart;
    bStop            = pO->bStop;
    bStartInv        = pO->bStartInv;
    bStopInv         = pO->bStopInv;
    bRunning         = pO->bRunning;
    bTripped         = pO->bTripped;
    m_dSpeed         = pO->m_dSpeed;
    m_dSpeedReg      = pO->m_dSpeedReg;
    m_dSpeedReqd     = pO->m_dSpeedReqd;
    m_dManualSpeed   = pO->m_dManualSpeed;
    m_bAutoRestart   = pO->m_bAutoRestart;
    iFailMode        = pO->iFailMode; 
    bLoSpeed         = pO->bLoSpeed;   
    bHiSpeed         = pO->bHiSpeed;
    dLoSpeedLimit    = pO->dLoSpeedLimit;  
    dHiSpeedLimit    = pO->dHiSpeedLimit;   
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

flag PL_Basic::DoRunning()
  {
  flag RunReqd=bRunning;
  if (!PwrOn() && !m_bAutoRestart)
    bOn=(bStartInv ? 1 : 0);
  if (bTripped)
    bOn=(bStartInv ? 1 : 0);
  switch (iMode)
    {
    case 0:
      RunReqd=bStartInv ? !bOn : bOn;
      ClrCI(1);
      break;
    case 1:
      {
      flag Start = bStartInv ? !bStart : bStart;
      flag Stop  = bStopInv  ? !bStop  : bStop;
      if (Start && !Stop && bOn)
        RunReqd=true;
      else if (!bOn || (Stop && !Start))
        RunReqd=false;
      SetCI(1, Stop && Start);
      break;
      }
    case 2:
      {
      flag Start = bStartInv ? !bStart : bStart;
      flag Stop  = bStopInv  ? !bStop  : bStop;
      if (Start && !Stop && bOn)
        RunReqd=true;
      else 
        RunReqd=false;
      ClrCI(1);
      break;
      }
    }

  if (bTripped || !PwrOn())
    bRunning=false;
  else if (RunReqd && (iFailMode & PLBFail_NoStart)==0)
    bRunning=RunReqd;
  else if (!RunReqd && (iFailMode & PLBFail_NoStop)==0)
    bRunning=RunReqd;
  
  bRunMemory=bRunning;
  return bRunning;
  }

//---------------------------------------------------------------------------

void PL_Basic::EvalCtrlActions(FlwNode* pFNode)
  {
  DoRunning();
 
  if (!bRunning)
    m_dSpeed=0;
  else if (Valid(m_dManualSpeed) && bRunning)
    m_dSpeed=m_dManualSpeed;
  else
    m_dSpeed=m_dSpeedReqd;
  m_dSpeed=Range(0.0, m_dSpeed, 1.0);

  }

//---------------------------------------------------------------------------

double PL_Basic::Speed(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  if (pFRB)
    m_dSpeedReg=m_dSpeed*pFRB->Ctrl();
  else
    m_dSpeedReg=m_dSpeed;
  bLoSpeed = m_dSpeedReg <= dLoSpeedLimit;   
  bHiSpeed = m_dSpeedReg >= dHiSpeedLimit;   
  return m_dSpeedReg;
  };

//--------------------------------------------------------------------------

void   PL_Basic::Start() 
  { 
  switch (iMode)
    {
    case 0:
      bOn = !bStartInv ? 1:0;
      break;
    default:
      bStart = !bStartInv ? 1:0;
      bStop  = !bStopInv ?  0:1;
    }
  };

void   PL_Basic::Stop()
  { 
  switch (iMode)
    {
    case 0:
      bOn    = !bStartInv ? 0:1;
      break;
    default:
      bStart = !bStartInv ? 0:1;
      bStop  = !bStopInv ?  1:0;
      break;
    }
  };

flag   PL_Basic::Running()              { return bRunning; };
flag   PL_Basic::Forward()              { return true; };
flag   PL_Basic::Reverse()              { return false; };
void   PL_Basic::SetDirection(flag Fwd) { ; };
flag   PL_Basic::Tripped()              { return bTripped; };
void   PL_Basic::SetTrip()              { bTripped=true; };
void   PL_Basic::ResetTrip()            { bTripped=false; };

//--------------------------------------------------------------------------

void PL_Basic::Ctrl_ConnIDStr(int i, Strng & ID, Strng &Tg)
  {
  ID=pAttachedTo->FullObjTag();
  ID+=".Speed.Reqd";
  Tg=pAttachedTo->FullObjTag();
  Tg+=".Speed.Reqd";
  };

//--------------------------------------------------------------------------

flag PL_Basic::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tStart Stop Mismatch";     return 1;
    case  2: pS="W\t??";                      return 1;
    case  3: pS="W\t??";                      return 1;
    default:
      return CIO_Logic::CIStrng(No, pS);
    }
  };

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Forward Reverse pump logic
#X:#h<General Description>#nThis logic allows the user to change the pump state 
between ON and OFF, the direction in which the pump operates, as well as allowing 
the pump speed to be ramped up or down.  This functionality is useful for pump control 
and when doing PLC testing using SysCAD.  All of the  variables can be changed while 
SysCAD is running.
#nIf the pump has been configured using the pump database, the pump speed will not exceed 
the maximum speed specified by the user.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : On or off. Is the pump operational or not.#n
#i<Forward> : Yes, the pump will operate in the normal direction, or no, the pump must 
reverse the direstion of flow..#n
#i<Y> : Required pump speed (0-100%).#n
#n
#n#h<Other>#n
Short name:PL_FwdRv#n
Model type:Pump Speed Logic#n
#G:Pump Speed Logic
*/

IMPLEMENT_PMPSPDLOGIC(PL_FwdRev, "PL_FwdRv", "", TOC_SYSTEM, "Fwd Rev", "Forward Reverse Motor Logic") 

PL_FwdRev::PL_FwdRev(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  PL_Basic(pClass_, pTag, pAttach, eAttach)
  {
  bFwd=1;
  };

//---------------------------------------------------------------------------

PL_FwdRev::~PL_FwdRev()
  {
  };

//---------------------------------------------------------------------------

void PL_FwdRev::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Byte("Mode", "", DC_, "", &iMode, this, isParm, DDBPmpStartMode);
  switch (iMode)
    {
    case 0:
      DDB.CheckBoxBtn("Enable",   "Run",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Invert",   "",     DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      break;
    case 1:
    case 2:
      DDB.CheckBoxBtn("Permissive", "",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Start",      "",  DC_, "",  &bStart,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Stop",       "",  DC_, "",  &bStop,    this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StartInvert","",  DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StopInvert", "",  DC_, "",  &bStopInv, this, isParm, DDBOnOff);
      break;
    }
  DDB.CheckBoxBtn("Running",  "",  DC_, "",  &bRunning,   this, 0, DDBOnOff);
  DDB.CheckBoxBtn("Tripped",  "",  DC_, "",  &bTripped,   this, isParm, DDBOnOff);

  //DDB.Add_OnOff("State", &bOn, False, this);
  //DDB.Add_YesNo("Forward", &bFwd, False, this);
  DDB.CheckBoxBtn("Forward", "",          DC_, "",  &bFwd,   this, isParm, DDBYesNo);
  DDB.Double("LocalY",       "Manual",    DC_Frac, "%", &m_dManualSpeed, this, isParm|NAN_OK);
  DDB.Double("Speed",        "Reqd",      DC_Frac, "%", &m_dSpeedReqd,   this, isParm);
  DDB.Double("",             "Actual",    DC_Frac, "%", &m_dSpeed, this, 0);
  DDB.Double("",             "Regulated", DC_Frac, "%", &m_dSpeedReg,    this, isResult);

  DDB.Bool  ("LoSpeed",      "",    DC_,        "",      &bLoSpeed,     this, isResult);
  DDB.Bool  ("HiSpeed",      "",    DC_,        "",      &bHiSpeed,     this, isResult);
  DDB.Float ("LoSpeedLimit", "",    DC_Frac,    "%",     &dLoSpeedLimit,this, isParm|InitHidden);
  DDB.Float ("HiSpeedLimit", "",    DC_Frac,    "%",     &dHiSpeedLimit,this, isParm|InitHidden);

  DDB.CheckBoxBtn("AutoReStart",  "",  DC_, "",  &m_bAutoRestart,   this, isParm, DDBOnOff);
  DDB.Byte("FailMode", "", DC_, "", &iFailMode, this, isParm, DDBPmpFailMode);
  DDB.CheckBoxBtn("RunMemory",  "",  DC_, "",  &bRunMemory,   this, InitHidden, DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  }

//---------------------------------------------------------------------------

flag PL_FwdRev::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  }

//---------------------------------------------------------------------------

//void PL_FwdRev::EvalDiscrete(FlwNode* pFNode)
//  {
//  };

//---------------------------------------------------------------------------

void PL_FwdRev::EvalCtrlActions(FlwNode* pFNode)
  {
  DoRunning();
  if (!bRunning)
    m_dSpeed=0;
  else if (Valid(m_dManualSpeed) && bRunning)
    m_dSpeed=m_dManualSpeed;
  else
    m_dSpeed=m_dSpeedReqd;
  m_dSpeed=Range(-1.0, m_dSpeed*(bFwd ? 1 : -1), 1.0);
  }

//---------------------------------------------------------------------------

double PL_FwdRev::Speed(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  if (pFRB)
    m_dSpeedReg=m_dSpeed*pFRB->Ctrl();
  else
    m_dSpeedReg=m_dSpeed;
  bLoSpeed = fabs(m_dSpeedReg) <= dLoSpeedLimit;   
  bHiSpeed = fabs(m_dSpeedReg) >= dHiSpeedLimit;   
  return m_dSpeedReg;
  };

//---------------------------------------------------------------------------

flag   PL_FwdRev::Forward()               { return bFwd; };
flag   PL_FwdRev::Reverse()               { return !bFwd; };
void   PL_FwdRev::SetDirection(flag Fwd)  { bFwd=Fwd; };

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Soft Start pump logic
#X:#h<General Description>#nThis logic allows the user to configure the starting method 
of the pump.  The soft start logic will slowly ramp up the speed of the pump to the 
operating point.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : On or off. Is the pump operational or not.#n
#i<Required> : Required pump speed (0-100%).#n
#i<StartTime> : The ramp up period of the pump..#n
#n#n
#h<Associated variables>#n
#i<Actual> : Current actual pump speed (0-100%)#n
#n
#n#h<Other>#n
Short name:PL_SftSt#n
Model type:Pump Speed Logic#n
#G:Pump Speed Logic
*/

IMPLEMENT_PMPSPDLOGIC(PL_SoftStart, "PL_SftSt", "", TOC_SYSTEM, "Soft Start", "Soft Start") 

PL_SoftStart::PL_SoftStart(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  PL_Basic(pClass_, pTag, pAttach, eAttach)
  {
  m_dSpeedReqd=1.0; // 100%
  dStartTime=10; // 10 seconds
  dMapLo=0.0;
  dMapHi=1.0;
  m_dMapSpeed=0.0;
  };

//---------------------------------------------------------------------------

PL_SoftStart::~PL_SoftStart()
  {
  };

//---------------------------------------------------------------------------

void PL_SoftStart::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Byte("Mode", "", DC_, "", &iMode, this, isParm, DDBPmpStartMode);
  switch (iMode)
    {
    case 0:
      DDB.CheckBoxBtn("Enable",   "Run",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Invert",   "",     DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      break;
    case 1:
    case 2:
      DDB.CheckBoxBtn("Permissive", "",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Start",      "",  DC_, "",  &bStart,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Stop",       "",  DC_, "",  &bStop,    this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StartInvert","",  DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StopInvert", "",  DC_, "",  &bStopInv, this, isParm, DDBOnOff);
      break;
    }
  DDB.CheckBoxBtn("Running",  "",  DC_, "",  &bRunning,   this, 0, DDBOnOff);
  DDB.CheckBoxBtn("Tripped",  "",  DC_, "",  &bTripped,   this, isParm, DDBOnOff);

  DDB.Double("StartTime",     "StrtTime", DC_Time, "s", &dStartTime,   this, isParm);
  DDB.Double("LocalY",        "Manual",   DC_Frac, "%", &m_dManualSpeed, this, isParm|NAN_OK);
  DDB.Double("Required",      "Reqd",     DC_Frac, "%", &m_dSpeedReqd,   this, isParm);
  DDB.Double("Actual",        "Actual",   DC_Frac, "%", &m_dSpeed,       this, isResult|0);
  DDB.Double("",              "Regulated",DC_Frac, "%", &m_dSpeedReg,    this, isResult);
  DDB.Double("MapLo",         "",         DC_Frac, "%", &dMapLo,       this, isParm);
  DDB.Double("MapHi",         "",         DC_Frac, "%", &dMapHi,       this, isParm);

  DDB.Bool  ("LoSpeed",      "",    DC_,        "",      &bLoSpeed,     this, isResult);
  DDB.Bool  ("HiSpeed",      "",    DC_,        "",      &bHiSpeed,     this, isResult);
  DDB.Float ("LoSpeedLimit", "",    DC_Frac,    "%",     &dLoSpeedLimit,this, isParm|InitHidden);
  DDB.Float ("HiSpeedLimit", "",    DC_Frac,    "%",     &dHiSpeedLimit,this, isParm|InitHidden);

  DDB.CheckBoxBtn("AutoReStart",  "",  DC_, "",  &m_bAutoRestart,   this, isParm, DDBOnOff);
  DDB.Byte("FailMode", "", DC_, "", &iFailMode, this, isParm, DDBPmpFailMode);
  DDB.CheckBoxBtn("RunMemory",  "",  DC_, "",  &bRunMemory,   this, InitHidden, DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  };

//---------------------------------------------------------------------------

flag PL_SoftStart::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  };

//---------------------------------------------------------------------------

void PL_SoftStart::EvalCtrlActions(FlwNode* pFNode)
  {
  DoRunning();
  
  double dReqdSpd, dDiffSpd;
  
  // Decide the required position
  if (bRunning)
    dReqdSpd=m_dSpeedReqd;
  else
    dReqdSpd=0.0;
      
  // the difference
  dDiffSpd = dReqdSpd-m_dSpeed;
  // limit the diff by the stroketime
  dDiffSpd = Sign(dDiffSpd)*Min(fabs(dDiffSpd),(ICGetTimeInc()/GTZ(dStartTime)));
  // apply it;
  m_dSpeed+=dDiffSpd;
  
  m_dSpeed=Range(0.0, m_dSpeed, 1.0);
  if (!bRunning)
    m_dMapSpeed=0.0;
  else 
    {
    if (Valid(m_dManualSpeed))
      m_dMapSpeed=Range(0.0, m_dManualSpeed, 1.0);
    else
      m_dMapSpeed=m_dSpeed;
    m_dMapSpeed=dMapLo+m_dMapSpeed*(dMapHi-dMapLo);
    }

  };

//---------------------------------------------------------------------------

double PL_SoftStart::Speed(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  if (pFRB)
    m_dSpeedReg=m_dMapSpeed*pFRB->Ctrl();
  else
    m_dSpeedReg=m_dMapSpeed;
  
  bLoSpeed = m_dSpeedReg <= dLoSpeedLimit;   
  bHiSpeed = m_dSpeedReg >= dHiSpeedLimit;   

  return m_dSpeedReg;
  };

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Soft Start/Stop pump logic
#X:#h<General Description>#nThis logic allows the user to configure the starting method 
of the pump.  The soft start/stop logic will slowly ramp up/down the speed of the pump to the 
operating point.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<State> : On or off. Is the pump operational or not.#n
#i<Required> : Required pump speed (0-100%).#n
#i<StartTime> : The ramp up period of the pump..#n
#i<StopTime> : The ramp down period of the pump..#n
#n#n
#h<Associated variables>#n
#i<Actual> : Current actual pump speed (0-100%)#n
#n
#n#h<Other>#n
Short name:PL_SftSt#n
Model type:Pump Speed Logic#n
#G:Pump Speed Logic
*/
const byte PLSS_FwdOnly           = 0;
const byte PLSS_FwdRevLogic       = 1;
const byte PLSS_FwdRevRegulation  = 2;

static DDBValueLst DDBFwdRevMode[]={
  {PLSS_FwdOnly,          "FwdOnly"},
  {PLSS_FwdRevLogic,      "Logic"},
  {PLSS_FwdRevRegulation, "Regulation"},
  {0}};
  
IMPLEMENT_PMPSPDLOGIC(PL_SoftStSp, "PL_SftStSp", "", TOC_SYSTEM, 
                      "Soft Start/Stop", "Soft Start/Stop") 

PL_SoftStSp::PL_SoftStSp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  PL_Basic(pClass_, pTag, pAttach, eAttach)
  {
  m_dSpeedReqd=1.0; // 100%
  dStartTime=10; // 10 seconds
  dStopTime=10; // 10 seconds
  dMapLo=0.0;
  dMapHi=1.0;
  m_dMapSpeed=0.0;
  iFwdRev=PLSS_FwdOnly;
  bRunRev=false;
  };

//---------------------------------------------------------------------------

PL_SoftStSp::~PL_SoftStSp()
  {
  };

//---------------------------------------------------------------------------

void PL_SoftStSp::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Byte("Mode", "", DC_, "", &iMode, this, isParm, DDBPmpStartMode);
  DDB.Byte("FwdRevMode", "", DC_, "", &iFwdRev, this, isParm, DDBFwdRevMode  );
  switch (iMode)
    {
    case 0:
      DDB.CheckBoxBtn("Enable",   "Run",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Invert",   "",     DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      break;
    case 1:
    case 2:
      DDB.CheckBoxBtn("Permissive", "",  DC_, "",  &bOn,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Start",      "",  DC_, "",  &bStart,   this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("Stop",       "",  DC_, "",  &bStop,    this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StartInvert","",  DC_, "",  &bStartInv,this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("StopInvert", "",  DC_, "",  &bStopInv, this, isParm, DDBOnOff);
      break;
    }

  if (iFwdRev==PLSS_FwdRevLogic)
    DDB.CheckBoxBtn("Reverse",  "",  DC_, "",  &bRunRev,   this, isParm, DDBOnOff);

  DDB.CheckBoxBtn("Running",  "",  DC_, "",  &bRunning,   this, 0, DDBOnOff);
  DDB.CheckBoxBtn("Tripped",  "",  DC_, "",  &bTripped,   this, isParm, DDBOnOff);

  DDB.Double("StartTime",     "StrtTime",  DC_Time, "s", &dStartTime,   this, isParm);
  DDB.Double("StopTime",      "StopTime",  DC_Time, "s", &dStopTime,    this, isParm);
  DDB.Double("LocalY",        "Manual",    DC_Frac, "%", &m_dManualSpeed, this, isParm|NAN_OK);
  DDB.Double("Required",      "Reqd",      DC_Frac, "%", &m_dSpeedReqd,   this, isParm);
  DDB.Double("Actual",        "Actual",    DC_Frac, "%", &m_dSpeed,       this, isResult|0);
  DDB.Double("",              "Regulated", DC_Frac, "%", &m_dSpeedReg,    this, isResult);
  DDB.Double("MapLo",         "",          DC_Frac, "%", &dMapLo,       this, isParm);
  DDB.Double("MapHi",         "",          DC_Frac, "%", &dMapHi,       this, isParm);
  DDB.Double("MapSpeed",      "",          DC_Frac, "%", &m_dMapSpeed,    this, isResult|0);

  DDB.Bool  ("LoSpeed",      "",    DC_,        "",      &bLoSpeed,     this, isResult);
  DDB.Bool  ("HiSpeed",      "",    DC_,        "",      &bHiSpeed,     this, isResult);
  DDB.Float ("LoSpeedLimit", "",    DC_Frac,    "%",     &dLoSpeedLimit,this, isParm|InitHidden);
  DDB.Float ("HiSpeedLimit", "",    DC_Frac,    "%",     &dHiSpeedLimit,this, isParm|InitHidden);

  DDB.CheckBoxBtn("AutoReStart",  "",  DC_, "",  &m_bAutoRestart,   this, isParm, DDBOnOff);
  DDB.Byte("FailMode", "", DC_, "", &iFailMode, this, isParm, DDBPmpFailMode);
  DDB.CheckBoxBtn("RunMemory",  "",  DC_, "",  &bRunMemory,   this, InitHidden, DDBOnOff);
  //if (m_pPwr)
  //  m_pPwr->BuildDataDefn(DDB);
  };

//---------------------------------------------------------------------------

flag PL_SoftStSp::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  };

//---------------------------------------------------------------------------

void PL_SoftStSp::EvalCtrlActions(FlwNode* pFNode)
  {
  DoRunning();

  double dReqdSpd, dDiffSpd;

  switch (iFwdRev)
    {
    case PLSS_FwdOnly:
      m_dSpeedReqd=Range(0.0, m_dSpeedReqd, 1.0);
      dReqdSpd=m_dSpeedReqd;
      if (Valid(m_dManualSpeed))
        m_dManualSpeed=Range(0.0, m_dManualSpeed, 1.0);
      break;
    case PLSS_FwdRevLogic:
      m_dSpeedReqd=Range(0.0, m_dSpeedReqd, 1.0);
      dReqdSpd=m_dSpeedReqd*(bRunRev ? -1 : 1);
      if (Valid(m_dManualSpeed))
        m_dManualSpeed=Range(0.0, m_dManualSpeed, 1.0);
      break;
    case PLSS_FwdRevRegulation:
      m_dSpeedReqd=Range(-1.0, m_dSpeedReqd, 1.0);
      dReqdSpd=m_dSpeedReqd;
      if (Valid(m_dManualSpeed))
        m_dManualSpeed=Range(-1.0, m_dManualSpeed, 1.0);
      break;
    }
  // Decide the required position
  if (!bRunning)
    dReqdSpd=0.0;
      
  // the difference
  dDiffSpd = dReqdSpd-m_dSpeed;
  // limit the diff by the stroketime
  if (fabs(m_dSpeed+dDiffSpd*0.01)>fabs(m_dSpeed))
    dDiffSpd = Sign(dDiffSpd)*Min(fabs(dDiffSpd), ICGetTimeInc()/GTZ(dStartTime));
  else
    dDiffSpd = -Sign(dDiffSpd)*Max(-fabs(dDiffSpd),-ICGetTimeInc()/GTZ(dStopTime));

  // apply it;
  m_dSpeed+=dDiffSpd;

  m_dSpeed=Range(-1.0, m_dSpeed, 1.0);
  if (!bRunning)
    m_dMapSpeed=0.0;
  else 
    {
    if (Valid(m_dManualSpeed))
      m_dMapSpeed=m_dManualSpeed;
    else
      m_dMapSpeed=m_dSpeed;
    m_dMapSpeed=dMapLo+m_dMapSpeed*(dMapHi-dMapLo);
    }
  
  };

//---------------------------------------------------------------------------

double PL_SoftStSp::Speed(FlwNode* pFNode, CFlwRegBlk * pFRB)
  {
  if (pFRB)
    m_dSpeedReg=m_dMapSpeed*pFRB->Ctrl();
  else
    m_dSpeedReg=m_dMapSpeed;

  bLoSpeed = m_dSpeedReg <= dLoSpeedLimit;   
  bHiSpeed = m_dSpeedReg >= dHiSpeedLimit;   
  
  return m_dSpeedReg;
  };

//---------------------------------------------------------------------------

flag   PL_SoftStSp::Forward()               { return !bRunRev; };
flag   PL_SoftStSp::Reverse()               { return bRunRev; };
void   PL_SoftStSp::SetDirection(flag Fwd)  { bRunRev=!Fwd; };

//===========================================================================
//===========================================================================
//===========================================================================
