//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "archmngr.h"
//#include "drv_dlgs.h"
//#include "drv_mngr.h"
#include "project.h"
#include "dlgbusy.h"
#include "dbgmngr.h"
#include "..\schist\hstmsg.h"
#include "TagDB.h"
#include "dbhelper.h"
#include "sfe_clnt.h"
#include "scd_wm.h"
#include "archdlgs.h"
#include "..\..\..\SMDK\Include\msglog.h"
#include "optoff.h"

#define dbgArchiver       WITHDEBUG

#if dbgArchiver
static CDbgMngr dbgArchiverWrites("ArchiveManager", "Writes");
static CDbgMngr dbgTime("ArchiveManager", "Time");
static CDbgMngr dbgConfig("ArchiveManager", "Config");
static CDbgMngr dbgReadCfg("ArchiveManager", "ReadCfg");
static CDbgMngr dbgDBState("ArchiveManager", "DbState");
static CDbgMngr dbgExecute("ArchiveManager", "Execute");
#endif

const LPTSTR ArcName = "Archive";

LPCTSTR ADBFnStr(int iFn)
  {
  switch (iFn)
    {
    case ADBFn_Instant:      return "Instant";
    case ADBFn_Minimum:      return "Minimum";
    case ADBFn_Maximum:      return "Maximum";
    case ADBFn_PeriodAvg:    return "Average";
    case ADBFn_RunningAvg:   return "RunningAvg";
    case ADBFn_Count:        return "Count";
    }
  return "?";
  };

LPCTSTR ADBMeasStr(int iMeas)
  {
  switch (iMeas)
    {
    case ADBMeas_StartPt:      return "StartPt";
    case ADBMeas_MidPt:        return "MidPt";
    case ADBMeas_EndPt:        return "EndPt";
    }
  return "?";
  };

LPCTSTR ADBFirstStr(int iMeas)
  {
  switch (iMeas)
    {
    case ADBFirst_Use:         return "Use";
    case ADBFirst_Ignore:      return "Ignore";
    }
  return "?";
  };

static LPCTSTR s_sArchiveContent = 
";=============================================================\n"
"; SysCAD Archive configuration file\n"
";=============================================================\n"
"\n"
"[Options]\n"
"DBFile     = $Prj\\Archive.MDB\n"
"\n"
";=============================================================\n"
"\n"
"[Columns]\n"
"DateTime   = 0\n"
"Date       = 1\n"
"Time       = 1\n"
"ElapsedHMS = 0\n"
"ElapsedDbl = s\n"
"Command    = 0\n"
"Type       = 1\n"
"Tag        = 1\n"
"Source     = 1\n"
"IDNo       = 0\n"
"IterNo     = 0\n"
"CallNo     = 0\n"
"SeqNo      = 0\n"
"Message    = 1\n"
"\n"
";=============================================================\n"
"\n"
"[MessageLog] ; System\n"
"On         = 1\n"
"Format     = DB\n"
"\n"
";=============================================================\n"
"\n"
"[EventLog] ; System\n"
"On         = 1\n"
"Format     = DB\n"
"\n"
";=============================================================\n"
"; \n"
"; Syntax\n"
"; -------\n"
"; \n"
"; [Table1] \n"
"; Name       = ATableName\n"
"; On         = 1\n"
"; Format     = DB\n"
"; Path       = $Prj\\Archive\n"
"; SigDigits  = 20\n"
"; Period     = hh:mm:ss\n"
"; Offset     = hh:mm:ss\n"
"; FieldNN    = \"FieldName\",   SysCADTag,              Function, MeasurePoint, FirstPoint\n"
"; TriggerNN  = \"Description\", SysCADTag,              TriggerTest\n"
"; Field1     = \"Steps\",       PlantModel.Stats.Steps, Current\n"
"; Trigger1   = \"Monthly\",     $Dyn.DateRec.Month,     Delta=0.1\n"
"; \n"
"; \n"
"; Options\n"
"; -------\n"
"; Format     = DB / CSV / TXT\n"
"; On         = 0/1 (off/on)\n" 
"; Path       = A Folder Name for CSV & TXT files. (Usually with the project) $Prj=current project folder\n"
"; SigDigits  = 1 .. 20 Number of significant digits written to CSV & TXT files\n"
"; Period     = Time frequency for reporting fields, where time zero is at start.\n"
"; Offset     = Time offset within the specified period. Optional. Zero is the default.\n"
"; FieldName  = Report column or field name. Should not contain characters: '.,[]\n"
"; SysCADTag  = Any valid SysCAD tag. May have conversions.\n"
"; Function   = Current / Minimum / Maximum / Average / Count\n"
";         OR = Cur     / Min     / Max     / Avg     / Cnt  \n"
"; MeasurePt  = StartPt / EndPt / MidPt  - EndPt is the default\n"
"; FirstPoint = IgnoreFirst / UseFirst  - IgnoreFirst is the default\n"
"; TriggerTest = Rise [= Value]/ Fall [= Value]/ Delta [= Value]\n"
";   [= Value] = Optional Test Value (default = 1.0)\n"
"; TriggerTest = StepRise [= Value]/ StepFall [= Value]/ StepDelta [= Value]\n"
";   [= Value] = Optional Test Value (default = 1.0)\n"
"; \n"
"; Notes:\n"
"; 1) Use Period for reporting at fixed time intervals, OR use Triggers for reporting\n"
";    at irregular time intervals (ie report specified fields based on trigger logic).\n"
"; 2) For the trigger test use the StepXXXX functions to compare results from previous\n"
";    iteration to decide if fields should be saved, or use the Rise/Fall/Delta to compare\n"
";    results from previous occurance of the trigger.\n"
"; 3) There can be any number of triggers. If the conditions of any of the triggers are met,\n"
";    then the fields will be reported.\n"
"; 4) All table entries are optional except the Name\n"
"; 5) If no Fields are entered then the table will still be created.\n"
"; 6) If one or more Triggers are used, a period & offset is not relevant or required.\n" 
"; 7) If no triggers or period are specified then the table will not accumulate data.\n"
"; \n"
";=============================================================\n"
"; Examples:\n"
"; [Table1]\n"
"; Name       = Daily\n"
"; On         = 1\n"
"; Format     = DB\n"
"; Period     = 24:00:00\n"
"; Field1     = \"Steps\",                PlantModel.Stats.Steps, Current\n"
"; Field2     = \"Tank1 Level (%)\",      TNK_1.Lvl (%), Current\n"
"; Field3     = \"Tank1 Max Level (%)\",  TNK_1.Lvl (%), Maximum\n"
"; Field4     = \"Tank1 Ave Level (%)\",  TNK_1.Lvl (%), Average\n"
"; \n"
"; [Table2]\n"
"; Name       = Monthly\n"
"; On         = 1\n"
"; Format     = CSV\n"
"; Path       = $Prj\\Archive\n"
"; Trigger1   = \"Monthly\",              $Dyn.DateRec.Month,     Delta=0.1\n"
"; Field1     = \"Steps\",                PlantModel.Stats.Steps, Current\n"
"; Field2     = \"Tank1 Level (%)\",      TNK_1.Lvl (%),          Current\n"
"; Field3     = \"Tank1 Max Level (%)\",  TNK_1.Lvl (%),          Maximum\n"
"; Field4     = \"Tank1 Ave Level (%)\",  TNK_1.Lvl (%),          Average\n"
"; \n"
";=============================================================\n"
"\n";

const LPCTSTR InvalidFldNmChars = ".'[]";
const LPCTSTR InvalidFldNmCharsM = "[]";
const LPCTSTR InvalidFldNmCharsD = ".'";

static LPCTSTR sMessageLogStr    = "MessageLog";
static LPCTSTR sEventLogStr      = "EventLog";
static LPCTSTR sRecordStr        = "RecordNo";
static LPCTSTR sDateTimeStr      = "DateTime";
static LPCTSTR sDateStr          = "Date";
static LPCTSTR sTimeStr          = "Time";
static LPCTSTR sElapsedHMSStr    = "ElapsedHMS";
static LPCTSTR sElapsedDaysStr   = "ElapsedDays";
static LPCTSTR sElapsedHrsStr    = "ElapsedHrs";
static LPCTSTR sElapsedMinsStr   = "ElapsedMins";
static LPCTSTR sElapsedSecsStr   = "ElapsedSecs";
static LPCTSTR sSourceStr        = "Source";
static LPCTSTR sTagStr           = "Tag";
static LPCTSTR sMessageStr       = "Message";
static LPCTSTR sCommandStr       = "Command";
static LPCTSTR sTypeStr          = "Type";
static LPCTSTR sIDNoStr          = "IDNo";
static LPCTSTR sIterNoStr        = "IterNo";
static LPCTSTR sCallNoStr        = "CallNo";
static LPCTSTR sSeqNoStr         = "SeqNo";

static const struct CSystemTable 
  { 
  LPCTSTR   m_Table; 
  LPCTSTR   m_FldName; 
  byte      m_Type; 
  int       m_Size; 
  UINT      m_Mask; 
  } 
s_SystemTables[] =
  {
    {NULL,           sDateTimeStr,   tt_Generic,0,   LogItem_Time         }, // tt_Generic to map to DateTime
    {NULL,           sDateStr,       tt_Generic,0,   LogItem_TimeDate     }, // tt_Generic to map to DateTime
    {NULL,           sTimeStr,       tt_Generic,0,   LogItem_TimeTime     }, // tt_Generic to map to DateTime
    {NULL,           sElapsedHMSStr, tt_Strng,  30,  LogItem_ElapsedHMS   },
    {NULL,           sElapsedDaysStr,tt_Double, 0,   LogItem_ElapsedDays  },
    {NULL,           sElapsedHrsStr, tt_Double, 0,   LogItem_ElapsedHrs   },
    {NULL,           sElapsedMinsStr,tt_Double, 0,   LogItem_ElapsedMins  },
    {NULL,           sElapsedSecsStr,tt_Double, 0,   LogItem_ElapsedSecs  },
    
    {sMessageLogStr, sCommandStr,    tt_Strng,  30,  LogItem_Command},
    {sMessageLogStr, sTypeStr,       tt_Strng,  20,  LogItem_Type   },
    {sMessageLogStr, sSourceStr,     tt_Strng,  50,  LogItem_Source },
    {sMessageLogStr, sIDNoStr,       tt_Long,   0,   LogItem_IDNo   },
    {sMessageLogStr, sIterNoStr,     tt_Long,   0,   LogItem_IterNo },
    {sMessageLogStr, sCallNoStr,     tt_Long,   0,   LogItem_CallNo },
    {sMessageLogStr, sSeqNoStr,      tt_Long,   0,   LogItem_SeqNo  },
    {sMessageLogStr, sMessageStr,    tt_Strng,  255, LogItem_Message},

    {sEventLogStr,   sTagStr,        tt_Strng,  100, LogItem_Tag    },
    {sEventLogStr,   sMessageStr,    tt_Strng,  255, LogItem_Message},

  };

inline LPTSTR C2T(LPCTSTR X) { return (LPTSTR)X; };

//===========================================================================
//
//
//
//===========================================================================

void CArcAddress::SetIndex(CArchiveManager & AM) 
  { 
  m_lInputIndex=AM.FindInputIndex(m_sTag); 
  if (m_lInputIndex<0)
    {
    m_lFieldIndex=AM.FindFieldIndex(m_sTag); 
    if (m_lFieldIndex<0)
      m_lInputIndex=AM.AddInput(m_sTag); 
    }
  else
    m_lFieldIndex=-1; 

#if dbgArchiver
  if (dbgReadCfg())
    dbgpln("SetIndex:Inp/Fld:[%4i/%4i] %s", m_lInputIndex, m_lFieldIndex, (LPCTSTR)m_sTag);
#endif
  };

double CArcAddress::GetValue(CArchiveManager & AM, LPTSTR CnvTxt)
  {
  if (m_lInputIndex>=0)
    return AM.GetInput(m_lInputIndex).GetValue(CnvTxt);
  if (m_lFieldIndex>=0)
    return AM.GetField(m_lFieldIndex).GetValue();
  return 0;
  }

CCnvIndex CArcAddress::GetCnvIndex(CArchiveManager & AM)
  {
  if (m_lInputIndex>=0)
    return AM.GetInput(m_lInputIndex).m_iCnv;
  if (m_lFieldIndex>=0)
    {
    //return AM.GetField(m_lFieldIndex).GetValue();
    _asm int 3;
    }
  return 0;
  }
//===========================================================================
//
//
//
//===========================================================================

CArchiveInput::CArchiveInput()
  {
  //m_dPreviousValue=dNAN;
  m_dValue=dNAN;
  };

bool CArchiveInput::PutValue(CPkDataItem &PItem)
  {
  m_dValue=PItem.Value()->GetDouble();
  return true;
  };

bool CArchiveInput::PutValue(double Value)
  {
  m_dValue=Value;
  return true;
  };

//===========================================================================
//
//
//
//===========================================================================

CArchiveDBField::CArchiveDBField(CArchiveManager *pAM) : m_rAM(*pAM)
  {
  m_iFn               = ADBFn_Null;
  m_iMeas             = ADBMeas_Null;
  m_lTableIndex       = -1;
  m_bResetReqd        = true;
  m_dCalculatedValue  = 0.0;
  m_dInputValue       = 0.0;

  m_bSystem           = false;
  };

//---------------------------------------------------------------------------

CArchiveDBField::~CArchiveDBField()
  {
  };

//---------------------------------------------------------------------------

bool CArchiveDBField::GetValue(CPkDataList &List, /*CPkDataItem * &pPItem,*/ bool Complete)
  {
  bool GotIt = False;
  PkDataUnion PData;

  PData.SetTypeDouble(tt_Double, m_dCalculatedValue);  

  if (Complete)
    {
    //supply ranges in SI units !!!
    DataUnion uMin;
    DataUnion uMax;
    CCnvIndex iCnv=m_Src.GetCnvIndex(m_rAM);
    uMin.Set(Cnvs[(iCnv)]->Normal(0.0, m_sCnvTxt()));
    uMax.Set(Cnvs[(iCnv)]->Normal(1.0, m_sCnvTxt()));
    List.SetDataValueAll(/*pPItem,*/ (char*)(LPCTSTR)m_sScdMTag, "", PData, isArchiveTag,
      uMin, uMax, iCnv, m_sCnvTxt()?m_sCnvTxt():"", 
      ""/*sCnvFam()*/, NULL, False, C2T(m_sDescription));
    }
  else
    List.SetDataValue(/*pPItem,*/ NULL, PData);
  return True;
  }

//---------------------------------------------------------------------------

bool CArchiveDBField::Execute(CXM_TimeControl &CB)
  {
  m_dInputValue = m_Src.GetValue(m_rAM, m_sCnvTxt()); // Value at the beginning of the step

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBField::StepDone(CXM_TimeControl &CB)
  {
  double InputAtStart = m_dInputValue;
  double InputAtEnd   = m_Src.GetValue(m_rAM, m_sCnvTxt());

  m_dInputValue       = InputAtEnd;

  if (m_rAM.Rewind())
    {
    if (m_iFirstOnRewind==ADBFirst_Ignore)
      InputAtStart = InputAtEnd;
    m_bResetReqd=true;
    }
  const bool ValidInputAtStart = Valid(InputAtStart);
  const bool ValidInputAtEnd = Valid(InputAtEnd);
  const bool BothValid = (ValidInputAtStart && ValidInputAtEnd);

  double InputMeas    = dNAN;
  switch (m_iMeas)
    {
    case ADBMeas_StartPt:  InputMeas = InputAtStart; break;
	case ADBMeas_MidPt:    InputMeas = (BothValid ? 0.5*(InputAtStart+InputAtEnd) : dNAN); break;
    case ADBMeas_EndPt:    InputMeas = InputAtEnd; break;
    }
  const bool ValidInputMeas = Valid(InputMeas);

#if dbgArchiver
  LPCTSTR SrchTag="";//"GC_1-z-AVG";
#endif
  if (m_bResetReqd)
    {
    switch (m_iFn)
      {
      case ADBFn_Instant:
        m_dCalculatedValue=InputMeas; 
        break;
      case ADBFn_Minimum:
	    if (BothValid)
          m_dCalculatedValue = Min(InputAtStart, InputAtEnd);
        break;
      case ADBFn_Maximum:
	    if (BothValid)
          m_dCalculatedValue = Max(InputAtStart, InputAtEnd);
        break;
      case ADBFn_PeriodAvg:
      case ADBFn_RunningAvg:
        switch (m_iMeas)
          {
          case ADBMeas_StartPt:  m_dCalculatedValue=InputAtStart; break;
          case ADBMeas_MidPt:    m_dCalculatedValue=(BothValid ? 0.5*(InputAtStart+InputAtEnd) : dNAN); break;
          case ADBMeas_EndPt:    m_dCalculatedValue=InputAtEnd; break;
          }
        break;
      case ADBFn_Count:
        m_dCalculatedValue=0;
        break;
      }
    m_nSamples=0;
    //#if dbgArchiver
    //    if (dbgExecute() && (strlen(SrchTag)==0 || stricmp(m_sFldTag, SrchTag)==0))
    //      dbgpln("%-10s Reset %5i %12s %12.4f = %12.4f %s", ADBFnStr(m_bWhat), m_nSamples, "", m_dInputValue, m_dCalculatedValue, m_sFldTag);
    //#endif
    }

  //TODO: If value is NAN, what is best result for min and max, etc!!!! (for now ignore?)

  double PrevFieldVal = m_dCalculatedValue;
  switch (m_iFn)
    {
    case ADBFn_Instant:
      m_dCalculatedValue = InputMeas; 
      break;
    case ADBFn_Minimum:
      if (BothValid)
        m_dCalculatedValue = Min(m_dCalculatedValue, Min(InputAtStart, InputAtEnd));
      break;               
    case ADBFn_Maximum:
      if (BothValid)
        m_dCalculatedValue = Max(m_dCalculatedValue, Max(InputAtStart, InputAtEnd));
      break;
    case ADBFn_PeriodAvg:
      if (ValidInputMeas)
		m_dCalculatedValue = (InputMeas + m_nSamples*m_dCalculatedValue)/(m_nSamples+1);
      m_nSamples++;
      break;
    case ADBFn_RunningAvg:
      m_dCalculatedValue = m_dInputValue;
      break;
    case ADBFn_Count:
      m_dCalculatedValue += 1.0;
      break;
    };

#if dbgArchiver
  if (dbgExecute() && (strlen(SrchTag)==0 || stricmp(m_sFldTag, SrchTag)==0))
    dbgpln("%-10s %-10s %5s %5i S:%12.4f E:%12.4f = I:%12.4f C:%12.4f %s", ADBFnStr(m_iFn), ADBMeasStr(m_iMeas), m_bResetReqd?"Reset":"", 
    m_nSamples, InputAtStart, InputAtEnd, m_dInputValue, m_dCalculatedValue, m_sFldTag);
#endif

  if (m_bResetReqd)
    m_bChanged = true;
  else
    {
    if (!Valid(m_dCalculatedValue) || !Valid(PrevFieldVal))
      m_bChanged = !(!Valid(m_dCalculatedValue) && !Valid(PrevFieldVal));
	else
      m_bChanged = (fabs(m_dCalculatedValue-PrevFieldVal)>1.0e-3);
	}
  m_bResetReqd = false;
  return true;
  };

//===========================================================================

ADOX::DataTypeEnum CArchiveDBField::dbType()
  {
  switch (m_iType)
    {
    case tt_Float : 
    case tt_Double: return ADOX::adDouble;
    case tt_Strng:  return ADOX::adVarWChar;
    case tt_Generic: return ADOX::adDate;
    }
  return ADOX::adInteger;
  };

//===========================================================================

ADOX::ColumnAttributesEnum CArchiveDBField::dbAttr()
  {
  return ADOX::adColNullable;
  };

//===========================================================================

CArchiveTrigger::CArchiveTrigger(CArchiveManager *pAM) : m_rAM(*pAM)
  {
  m_bInitReqd   = true;
  m_bRewindReqd = false;
  m_bStepTest   = false;
  m_bRise       = false;
  m_bFall       = false;
  m_bAny        = false;
  m_iTriggerFiredCount = 0;
  }

//---------------------------------------------------------------------------

CArchiveTrigger::~CArchiveTrigger()
  {
  };

//---------------------------------------------------------------------------

bool CArchiveTrigger::Parse(LPCTSTR Table, int No, LPCTSTR Cfg)
  {
  if (Cfg==NULL || strlen(Cfg)==0)
    return false;

  char Buff[4096];
  CSVColArray c;
  int Quote;
  strncpy(Buff, Cfg, sizeof(Buff));
  int nFlds = ParseCSVTokens(Buff, c, Quote, NULL, true);//, "=");

  if (Quote || nFlds<3)
    goto Bad;
  else
    {
    if (strlen(c[0])>0)
      m_sName = c[0];
    else
      m_sName.Format("%s.Trigger%i", Table, No);

    Strng WrkTag,WrkCnvTxt;
    TaggedObject::SplitTagCnv(c[1], WrkTag, WrkCnvTxt);

    if (WrkTag.Len()==0)
      {
      LogError(ArcName, 0, "Tag not specified for Trigger '%s'", Cfg);
      return false;
      }

    m_Src.SetTag(WrkTag());
    m_sCnvTxt = WrkCnvTxt();

    m_dPrevAny=0.0;
    m_dPrevEdge=0.0;
    for (int i=2; c[i] && strlen(c[i])>0; i++)
      {
      LPTSTR Tkn=c[i];
      char *p;
      while ((p=strchr(Tkn,' '))!=NULL)
        memmove(p, p+1, strlen(p+1)+1);
      if (_strnicmp(Tkn, "delta=", 6)==0)
        {
        m_bStepTest = false;
        m_bAny=true;
        m_dDeltaAny=fabs(SafeAtoF(Tkn+6,1));
        }
      else if (_strnicmp(Tkn, "rise=", 5)==0)
        {
        m_bStepTest = false;
        m_bRise=true;
        m_dDeltaRise=fabs(SafeAtoF(Tkn+5,1));
        }
      else if (_strnicmp(Tkn, "fall=", 5)==0)
        {
        m_bStepTest = false;
        m_bFall=true;
        m_dDeltaFall=fabs(SafeAtoF(Tkn+5,1));
        }
      else if (_stricmp(Tkn, "delta")==0)
        {
        m_bStepTest = false;
        m_bAny=true;
        m_dDeltaAny=1;
        }
      else if (_stricmp(Tkn, "rise")==0)
        {
        m_bStepTest = false;
        m_bRise=true;
        m_dDeltaRise=1;
        }
      else if (_stricmp(Tkn, "fall")==0)
        {
        m_bStepTest = false;
        m_bFall=true;
        m_dDeltaFall=1;
        }
      else if (_strnicmp(Tkn, "stepdelta=", 10)==0)
        {
        m_bStepTest = true;
        m_bAny=true;
        m_dDeltaAny=fabs(SafeAtoF(Tkn+10,1));
        }
      else if (_strnicmp(Tkn, "steprise=", 9)==0)
        {
        m_bStepTest = true;
        m_bRise=true;
        m_dDeltaRise=fabs(SafeAtoF(Tkn+9,1));
        }
      else if (_strnicmp(Tkn, "stepfall=", 9)==0)
        {
        m_bStepTest = true;
        m_bFall=true;
        m_dDeltaFall=fabs(SafeAtoF(Tkn+9,1));
        }
      else if (_stricmp(Tkn, "stepdelta")==0)
        {
        m_bStepTest = true;
        m_bAny=true;
        m_dDeltaAny=1;
        }
      else if (_stricmp(Tkn, "steprise")==0)
        {
        m_bStepTest = true;
        m_bRise=true;
        m_dDeltaRise=1;
        }
      else if (_stricmp(Tkn, "stepfall")==0)
        {
        m_bStepTest = true;
        m_bFall=true;
        m_dDeltaFall=1;
        }
      else if (_stricmp(Tkn, "passing")==0)
        {
        _asm int 3;

        //m_bFall=true;
        //m_dDeltaFall=1;
        }
      else
        goto Bad;
      }
    m_bInitReqd=true;
    }
  return true;

Bad:
  LogError(ArcName, 0, "Bad Trigger Configuration %s", Cfg);
  return false;
  };

//---------------------------------------------------------------------------

bool CArchiveTrigger::TestTrigger()
  {
  const double CurValue=m_Src.GetValue(m_rAM, m_sCnvTxt());

  bool Triggered=false;

  if (m_bInitReqd)
    {
    m_iTriggerFiredCount = 0;
    m_bInitReqd   = false;
    m_dPrevAny    = CurValue;
    m_dPrevEdge   = CurValue;
    m_bRewindReqd = true;
    }      

  if (m_bRewindReqd)
    {
    m_bRewindReqd = false;
    }      

  const bool ValidCur = Valid(CurValue);
  if (m_bRise || m_bFall)
    {
    const bool ValidEdge = Valid(m_dPrevEdge);
    if (ValidCur || ValidEdge)
      {
      if (ValidCur!=ValidEdge)
        {
        Triggered=true;
        }
      else if (m_bRise && (CurValue-m_dPrevEdge)>m_dDeltaRise)
        {
        Triggered=true;
        }
      else if (m_bFall && (CurValue-m_dPrevEdge)<-m_dDeltaFall)
        {
        Triggered=true;
        }
      }
    if (Triggered)
      m_dPrevEdge=CurValue;
    }
  else if (m_bAny)
    {
    const bool ValidAny = Valid(m_dPrevAny);
    if (ValidCur || ValidAny)
      {
      if (ValidCur!=ValidAny || fabs(CurValue-m_dPrevAny)>m_dDeltaAny)
        {
        m_dPrevAny=CurValue;
        Triggered=true;
        }
      }
    }
  if (m_bStepTest)
    {
    m_dPrevEdge=CurValue;
    m_dPrevAny=CurValue;
    }

  //if (Triggered && DoTriggerNote)
  //  LogNote(ArcName, 0, "%s: %s", (LPCTSTR)m_sName, How);

  if (Triggered)
    m_iTriggerFiredCount++;
  return Triggered;
  }

//===========================================================================

CArchiveDBTable::CArchiveDBTable(CArchiveManager *pAM, LPCTSTR TbName) : m_rAM(*pAM)
  {
  m_bInitReqd     = true;
  m_bRewindReqd   = false;
  m_dNextSaveTime = CTimeValue(0.0);
  m_bPeriodOn     = false;
  m_iFormat       = ADBTbFmt_Null;

  m_sName         = TbName;
  m_bOn           = true;

  m_hFile         = NULL;
  m_nSigDigits    = 20;
  m_nTxtRecNo     = -1;

  m_bClearOnRewind = true;

  };

//---------------------------------------------------------------------------

CArchiveDBTable::CArchiveDBTable(CArchiveManager *pAM,
                                 LPCTSTR TbName,
                                 byte    iTbFmt,
                                 LPCTSTR sPath,
                                 byte iSigDigits) : m_rAM(*pAM)
  {
  m_bInitReqd     = true;
  m_bRewindReqd   = false;
  m_dNextSaveTime = CTimeValue(0.0);
  m_bPeriodOn     = false;
  m_hFile         = NULL;
  m_nTxtRecNo     = -1;

  m_sName        = TbName;
  m_iFormat      = iTbFmt;
  m_sPath        = sPath;
  m_nSigDigits   = iSigDigits;

  m_bClearOnRewind = true;
  };

//---------------------------------------------------------------------------

CArchiveDBTable::~CArchiveDBTable()
  {
  for (int t=0; t<m_Triggers.GetSize(); t++)
    delete m_Triggers[t];
  m_Triggers.SetSize(0);
  m_Fields.SetSize(0);
  };

//---------------------------------------------------------------------------

const long SecondsInDay=24*60*60;
const long SecondsInHour=60*60;
const long SecondsInMinute=60;

bool CArchiveDBTable::AddPeriod(long Period, long Offset)
  {
  m_lPeriod     = Max(0L,Period);
  m_lOffset     = Range(0L, Offset, SecondsInDay);
  m_bPeriodOn   = m_lPeriod>0;
  m_bInitReqd   = true;
  m_bRewindReqd = false;

  if (m_bPeriodOn)
    {
    if (m_lPeriod<SecondsInDay && ((SecondsInDay % m_lPeriod)!=0))
      LogNote(ArcName, 0, "Period for %s not modulo 24 hours", (LPCTSTR)m_sName);
    if (m_lPeriod>SecondsInDay && ((m_lPeriod % SecondsInDay)!=0))
      LogNote(ArcName, 0, "Period for %s not modulo 24 hours", (LPCTSTR)m_sName);
    }
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::AddTrigger(LPCTSTR Cfg)
  {
  CArchiveTrigger * pTr=new CArchiveTrigger(&m_rAM);
  if (!pTr->Parse(m_sName, m_Triggers.GetCount()+1, Cfg))
    {
    delete pTr;
    return false;
    }
  else
    m_Triggers.Add(pTr);
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::Execute(CXM_TimeControl &CB)
  {
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::StepDone(CXM_TimeControl &CB)
  {
  if (!m_bOn)
    return true;

  bool DoSavePer=false;
  bool DoSaveTrg=false;

  CTimeValue Period((double)m_lPeriod);
  CTimeValue Offset((double)m_lOffset);

  if (m_bInitReqd)
    {
    m_bInitReqd   = false;
    m_bRewindReqd = true;
    }

  if (m_bRewindReqd)
    {
    if (m_bPeriodOn)
      {
      __time64_t tt=(__time64_t)CB.m_TheTime.Seconds;
      //tm* t=_localtime64(&tt);
      tm* t=_gmtime64(&tt);

      CTimeValue CurTimeInDay((double)(t->tm_sec+t->tm_min*SecondsInMinute+t->tm_hour*SecondsInHour));
      CTimeValue SaveTimeInDay(Offset);

      //simple - could be more efficient  
      while (SaveTimeInDay-Period>CurTimeInDay)
        SaveTimeInDay-=Period;
      while (SaveTimeInDay<CurTimeInDay)
        SaveTimeInDay+=Period;

      m_dNextSaveTime=CB.m_TheTime-CurTimeInDay+SaveTimeInDay;
      }

    if (0)
      {
      dbgpln("CArchiveDBTable::StepDone %s %14.3f %14.3f %8.3f %s",  
        "Rewind", 
        m_dNextSaveTime.Seconds, CB.m_TheTime.Seconds,
        m_bPeriodOn?(m_dNextSaveTime-CB.m_TheTime).Seconds:-1, m_sName);
      }

    m_bRewindReqd=false;
    }
  else
    {
    if (0)
      {
      dbgpln("CArchiveDBTable::StepDone %s %14.3f %14.3f %8.3f %s",  
        "      ", 
        m_dNextSaveTime.Seconds, CB.m_TheTime.Seconds,
        m_bPeriodOn?(m_dNextSaveTime-CB.m_TheTime).Seconds:-1, m_sName);
      }
    }

  bool DoTriggerNote=false; // ???????????????
#if dbgArchiver
  if (dbgArchiverWrites())
    DoTriggerNote=true;
#endif

  if (m_bPeriodOn && (CB.m_TheTime>=m_dNextSaveTime))
    {
    DoSavePer=true;
    m_dNextSaveTime+=Period;
    if (DoTriggerNote)
      LogNote(ArcName,0,"%s %s", "Periodic", (LPCTSTR)m_sName);
    //LogNote(ArcName,0,"%s %s %s", CB.m_TheTime.Format(TD_Time), DoSavePer ? "Period ":"Trigger",(LPCTSTR)m_sName);
    }

  for (int tr=0; tr<m_Triggers.GetSize(); tr++)
    DoSaveTrg = DoSaveTrg || m_Triggers[tr]->TestTrigger();//DoTriggerNote);


  if (DoSavePer || DoSaveTrg)
    {
    //if (DoTriggerNote)
    //  LogNote(ArcName,0,"%s %s %s", CB.m_TheTime.Format(TD_Time), DoSavePer ? "Period ":"Trigger",(LPCTSTR)m_sName);

    if (m_rAM.m_bDBOK)
      {
      AddDBRecord(CB);
      }
    else
      {
      LogWarning(ArcName,0,"Record not written : %s %s %s", CB.m_TheTime.Format(TD_Time), DoSavePer ? "Period ":"Trigger",(LPCTSTR)m_sName);
      }

    for (long fd=0; fd<m_Fields.GetSize(); fd++)
      m_Fields[fd]->m_bResetReqd=true;
    };

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBTable()
  {
  if (!m_bOn)
    return true;

  switch (m_iFormat)
    {
    case ADBTbFmt_DB:
      {
      ADOX::_TablePtr pTbl;
      try
        {
        //    m_Def
        try
          {
          pTbl=m_rAM.m_pCat->Tables->GetItem((LPCTSTR)m_sName);
          CreateDBFields(pTbl);
          }
        catch (_com_error & e)
          {
          if (SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
            //if (e->m_pErrorInfo->m_lErrorCode!=3265)
            {
            m_rAM.DisplayException(e, LF_Exclamation);
            return true;
            }
          else
            {
            try
              {
              pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));
              pTbl->Name=(LPCTSTR)m_sName;
              pTbl->ParentCatalog = m_rAM.m_pCat;
              CreateDBFields(pTbl);
              m_rAM.m_pCat->Tables->Append((IDispatch*)pTbl);
              m_rAM.m_nTablesOpn++;

              //m_pDef->Create(m_sName);
              //if (CreateDBFields())
              //  m_pDef->Append();
              //m_rAM.m_nTablesOpn++;
              }
            catch (_com_error & e)
              {
              m_rAM.DisplayException(e, LF_Exclamation, "Create Table", (LPCTSTR)m_sName);
              return false;
              }
            }
          }

        }
      catch (_com_error & e)
        {
        m_rAM.DisplayException(e, LF_Exclamation, "Create Table", (LPCTSTR)m_sName);
        //delete m_pDef;
        return false;
        }
      break;
      }
    case ADBTbFmt_CSV:
    case ADBTbFmt_TXT:
      {
      bool OK = ReOpenTxtTable(m_rAM.m_bRewind);
      if (OK)  
        m_rAM.m_nTablesOpn++;
      return OK;
      break;
      }
    }

  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::DeleteDBTable()
  {
  if (m_bOn)
    return false;

  switch (m_iFormat)
    {
    case ADBTbFmt_DB:
      {
      try
        {
        m_rAM.m_pCat->Tables->Delete((LPCTSTR)m_sName);
        return true;
        }
      catch (_com_error & e)
        {
        if (SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
          {
          m_rAM.DisplayException(e, LF_Exclamation, "Delete Table", (LPCTSTR)m_sName);
          return false;
          }
        return true;
        }
      break;
      }
    case ADBTbFmt_CSV:
    case ADBTbFmt_TXT:
      {
      remove(m_sFileName());
      break;
      }
    }

  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::ReOpenTxtTable(bool Rewind)
  {
  if (!m_bOn)
    return true;

  if (0)
    dbgpln("CArchiveDBTable::ReOpenTxtTable %s %s",  Rewind?"Rewind":"      ", m_sName);

  if (m_hFile)
    {
    if (fclose(m_hFile)==0)
      {
      m_hFile=NULL;
      }
    else
      {
      LogError(ArcName, 0, "Table File '%s' not Closed", m_sFileName);
      return false;
      }
    }

  m_nTxtRecNo=0;

  Strng Path(m_sPath);
  Path.FnExpand();
  if (Path.GetLength()<2)
    m_sFileName=PrjFiles();
  else if (Path[0]=='\\' || Path[1]==':')
    m_sFileName=Path;
  else
    {
    m_sFileName=PrjFiles();
    m_sFileName+=Path;
    }
  m_sFileName.FnCheckEndBSlash();

  Strng Err;
  if (FnCreatePath(m_sFileName(), Err))
    {
    m_sFileName+=m_sName;
    m_sFileName.FnCheckExtension(m_iFormat==ADBTbFmt_CSV?"CSV":"TXT");

    if (Rewind)
      {
      m_hFile=fopen(m_sFileName(), "w");
      if (m_hFile==NULL)
        {
        LogError(ArcName, 0, "Tablefile '%s' not opened", m_sFileName());
        return false;
        }
      }
    else
      {
      m_hFile=fopen(m_sFileName(), "a+");
      if (m_hFile==0)
        {
        LogError(ArcName, 0, "Tablefile '%s' not opened", m_sFileName());
        return false;
        }

      while (!feof(m_hFile))
        {
        char buff[8192];
        if (fgets(buff, sizeof(buff), m_hFile))
          m_nTxtRecNo=SafeAtoL(buff,m_nTxtRecNo);
        else
          break;
        }
      }

    CreateTxtFields();
    }
  else
    {
    LogError(ArcName, 0, "Table Path '%s' not created", m_sFileName());
    return false;
    }

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBFields(ADOX::_TablePtr & pTbl)
  {
  CreateDBField(pTbl, true, sRecordStr, ADOX::adInteger, 0, ADOX::ColumnAttributesEnum (0), true/*dbAutoIncrField*/);

  for (int f=0; f<m_Fields.GetSize(); f++)  
    {
    CArchiveDBField &It=*m_Fields[f];
    CreateDBField(pTbl, It.m_bSystem, It.m_sFldTag, It.dbType(), It.m_iSize, It.dbAttr());
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateTxtFields()
  {
  if (m_hFile)
    {
    char Separ = (m_iFormat==ADBTbFmt_CSV)?',':'\t';
    fprintf(m_hFile, "%s", sRecordStr);

    for (int f=0; f<m_Fields.GetSize(); f++)  
      {
      CArchiveDBField &It=*m_Fields[f];
      fprintf(m_hFile, "%c%s", Separ, It.m_sFldTag);
      m_rAM.m_nFieldsOpn++;
      }
    fprintf(m_hFile, "\n");
    fflush(m_hFile);
    }

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBField(ADOX::_TablePtr & pTbl, bool SysFld, LPCTSTR FldName, ADOX::DataTypeEnum FldType, int Size, ADOX::ColumnAttributesEnum FldAttr, bool MakeAutoInc)
  {
  if (!m_bOn)
    return true;

  ADOX::_ColumnPtr pCol;
  try
    {
    pCol=pTbl->Columns->GetItem(FldName);
    //m_pDef->GetFieldInfo(FldName, FI);
    if (!SysFld)
      m_rAM.m_nFieldsOpn++;
    }
  catch (_com_error & e)
    {
    if (SCODE_CODE(e.Error())!=ADODB::adErrItemNotFound)
      //if (e->m_pErrorInfo->m_lErrorCode!=3265)
      {
      m_rAM.DisplayException(e, LF_Exclamation);
      return false;
      }
    else
      {
      try
        {
        ADOX::_ColumnPtr pCol=ADOX::_ColumnPtr(__uuidof(ADOX::Column));
        pCol->Name=FldName;
        pCol->Type=FldType;
        switch (FldType)
          {
          case ADOX::adInteger:  pCol->DefinedSize=sizeof(long); break;
          case ADOX::adVarWChar: pCol->DefinedSize=Size;         break;
          default:               pCol->DefinedSize=0;            break;
          }
        if (FldType==ADOX::adBoolean) // it appears that adBooleans are allways nullable ???? CNM
          FldAttr=ADOX::ColumnAttributesEnum(FldAttr&~ADOX::adColNullable);
        pCol->Attributes=FldAttr;
        pTbl->Columns->Append((IDispatch*)pCol, FldType, 0);

        if (MakeAutoInc)
          {
          bool xxx=pCol->Properties->GetItem("AutoIncrement")->Value;
          pCol->Properties->GetItem("AutoIncrement")->Value=MakeAutoInc;
          }

        if (!SysFld)
          m_rAM.m_nFieldsOpn++;
        }
      catch (_com_error & e)
        {
        m_rAM.DisplayException(e, LF_Exclamation, "Create Field", FldName);
        return false;
        }
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBRecordset()
  {
  if (!m_bOn)
    return true;
  
  switch(m_iFormat)
    {
    case ADBTbFmt_DB:
      try
        {
        m_pRst=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
        m_pRst->Open((LPCTSTR)m_sName, _variant_t((IDispatch*)m_rAM.m_pCnn), ADODB::adOpenDynamic, ADODB::adLockOptimistic/*ReadOnly*/, ADODB::adCmdTableDirect);

        }
      catch (_com_error & e)
        {
        m_rAM.DisplayException(e, LF_Exclamation, "Open RS ", (LPCTSTR)m_sName);
        m_pRst.Release();
        return false;
        }
      break;
    case ADBTbFmt_CSV:
    case ADBTbFmt_TXT:
      break;
    }
  return true;
  };

//---------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable : 4101)

bool CArchiveDBTable::DeleteDBRecords()
  {
  if (!m_bOn)
    return true;
  
  int n=9000; // MaxLocksPerFile limit = 9500
  if (m_pRst!=0)
    {
    try 
      {
      try 
        {
        m_pRst->MoveFirst();
        }
      catch (...)
        {
        n=-1; // stop it;
        }
      while (!m_pRst->BOF && !m_pRst->adEOF && --n>=0)
        {
        //dbgpln("delete %5i %s", n, m_sName);
        m_pRst->Delete(ADODB::adAffectCurrent);
        try
          { 
          m_pRst->MoveFirst(); 
          }
        catch (...) 
          { 
          n=-1; // stop it;
          }
        }
      }
    catch (_com_error & e)
      {
      m_rAM.DisplayException(e, LF_Exclamation, "Deleting records from ", m_sName);
      return false;
      }
    }
  return true;
  }

#pragma warning(pop)

//---------------------------------------------------------------------------

void CArchiveDBTable::AddTimeFieldsDB(CXM_TimeControl &CB)
  {
  if (m_rAM.m_dwOptions & (LogItem_Time | LogItem_TimeDate | LogItem_TimeTime))
    {
    COleDateTime DT;
    __time64_t TheTime=(__time64_t)CB.m_TheTime.Seconds;
    //tm* t=_localtime64(&TheTime);
    tm* t=_gmtime64(&TheTime);
    DT.SetDateTime(t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    COleVariant dv1(DT);
    if (m_rAM.m_dwOptions & LogItem_Time)
      m_pRst->Fields->GetItem(sDateTimeStr)->Value=dv1;
    if (m_rAM.m_dwOptions & LogItem_TimeDate)
      m_pRst->Fields->GetItem(sDateStr)->Value=dv1;
    if (m_rAM.m_dwOptions & LogItem_TimeTime)
      m_pRst->Fields->GetItem(sTimeStr)->Value=dv1;
//todo
    }

  CTimeValue ET=(m_rAM.m_TimeAtStartValid ? CB.m_TheTime.Seconds-m_rAM.m_TimeAtStart.Seconds : 0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedHMS)
    {
    CString S;
    if (m_rAM.m_TimeAtStartValid)
      S=ET.Format(TD_DateLeft|TD_Time|TD_TrimTime);
    m_pRst->Fields->GetItem(sElapsedHMSStr)->Value=COleVariant(S);
    }

  if (m_rAM.m_dwOptions & LogItem_ElapsedDays)
    m_pRst->Fields->GetItem(sElapsedDaysStr)->Value=COleVariant(ET.Seconds/86400.0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedHrs)
    m_pRst->Fields->GetItem(sElapsedHrsStr)->Value=COleVariant(ET.Seconds/3600.0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedMins)
    m_pRst->Fields->GetItem(sElapsedMinsStr)->Value=COleVariant(ET.Seconds/60.0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedSecs)
    m_pRst->Fields->GetItem(sElapsedSecsStr)->Value=COleVariant(ET.Seconds/1.0);
  }

//---------------------------------------------------------------------------

void CArchiveDBTable::AddTimeFieldsTxt(CXM_TimeControl &CB, char Separ)
  {
  if (m_rAM.m_dwOptions & (LogItem_Time | LogItem_TimeDate | LogItem_TimeTime))
    {
    CString S;
    if (m_rAM.m_dwOptions & LogItem_Time)
      {
      S=CB.m_TheTime.Format(TD_DateLeft|TD_Time|TD_TrimTime);
      fprintf(m_hFile, "%c%s", Separ, S);
      }
    if (m_rAM.m_dwOptions & LogItem_TimeDate)
      {
      S=CB.m_TheTime.Format(TD_DateLeft);
      fprintf(m_hFile, "%c%s", Separ, S);
      }
    if (m_rAM.m_dwOptions & LogItem_TimeTime)
      {
      S=CB.m_TheTime.Format(TD_Time|TD_TrimTime);
      fprintf(m_hFile, "%c%s", Separ, S);
      }
    }

  CTimeValue ET=(m_rAM.m_TimeAtStartValid ? CB.m_TheTime.Seconds-m_rAM.m_TimeAtStart.Seconds : 0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedHMS)
    {
    CString S;
    if (m_rAM.m_TimeAtStartValid)
      S=ET.Format(TD_DateLeft|TD_Time|TD_TrimTime);
    fprintf(m_hFile, "%c%s", Separ, S);
    }

  if (m_rAM.m_dwOptions & LogItem_ElapsedDays)
    fprintf(m_hFile, "%c%.3f", Separ, ET.Seconds/86400.0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedHrs)
    fprintf(m_hFile, "%c%.3f", Separ, ET.Seconds/3600.0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedMins)
    fprintf(m_hFile, "%c%.3f", Separ, ET.Seconds/60.0);
  if (m_rAM.m_dwOptions & LogItem_ElapsedSecs)
    fprintf(m_hFile, "%c%.3f", Separ, ET.Seconds/1.0);

  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::AddDBRecord(CXM_TimeControl &CB)
  {
  if (!m_bOn)
    return true;

  if (m_iFormat==ADBTbFmt_DB)
    {
    if (m_pRst==NULL)
      return false;

    try
      {
      m_pRst->AddNew();

      AddTimeFieldsDB(CB);

      for (long fd=0; fd<m_Fields.GetSize(); fd++)
        {
        if (!m_Fields[fd]->m_bSystem)
          {
          COleVariant v((double)fd);
          m_pRst->Fields->GetItem((LPCTSTR)m_Fields[fd]->m_sFldTag)->Value=m_Fields[fd]->m_dCalculatedValue;
          }
        }

      m_pRst->Update();
      }
    catch (_com_error & e)
      {
      m_rAM.DisplayException(e, LF_Exclamation, "Adding Record ", (LPCTSTR)m_sName);
      return false;
      }
    }
  else
    {

    if (m_hFile==NULL)
      return false;

    //CTimeValue TFS;
    //TFS=(CB.m_TheTime-m_rAM.m_TimeAtStart);
    char Separ = (m_iFormat==ADBTbFmt_CSV)?',':'\t';
    fprintf(m_hFile, "%i", ++m_nTxtRecNo);

    AddTimeFieldsTxt(CB, Separ);


    m_nSigDigits=Range(2,(int)m_nSigDigits,20);
    for (long fd=0; fd<m_Fields.GetSize(); fd++)
      {
      if (!m_Fields[fd]->m_bSystem)
        fprintf(m_hFile, "%c%.*g", Separ, m_nSigDigits, m_Fields[fd]->m_dCalculatedValue);
      }

    fprintf(m_hFile, "\n");
    fflush(m_hFile);
    }

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::AddMsgRecord(CXM_TimeControl &CB, CMsgLogItem & Msg)
  {
  if (!m_bOn)
    return true;

  CAutoIncDec AID(m_rAM.m_iBusyLoggingMsg);

  CString Type;
  if (m_rAM.m_dwOptions & LogItem_Type) 
    {
    switch (Msg.m_dwFlags & (LogFlag_Fatal | LogFlag_Stop | LogFlag_Separator | 
      LogFlag_Error | LogFlag_Warning | LogFlag_Note | 
      LogFlag_Cmd | LogFlag_CondClear))
      {
      case LogFlag_Fatal: 
        Type= "Fatal";
        break;
      case LogFlag_Stop: 
        Type= "Stop";
        break;
      case LogFlag_Error: 
        Type= "Errot";
        break;
      case LogFlag_Warning: 
        Type="Warning";
        break;
      case LogFlag_Note:
        Type = "Note";
        break;
      }
    }

  if (m_iFormat==ADBTbFmt_DB)
    {
    if (m_pRst==NULL)
      return false;

    try
      {
      m_pRst->AddNew();

      AddTimeFieldsDB(CB);

      if (m_rAM.m_dwOptions & LogItem_Command) 
        m_pRst->Fields->GetItem(sCommandStr)->Value = 0;//Msg.m_lIDNo;
      if (m_rAM.m_dwOptions & LogItem_Type) 
        m_pRst->Fields->GetItem(sTypeStr)->Value    = (LPCTSTR)Type;
      if (m_rAM.m_dwOptions & LogItem_Source) 
        m_pRst->Fields->GetItem(sSourceStr)->Value  = Msg.Source();
      if (m_rAM.m_dwOptions & LogItem_IDNo) 
        m_pRst->Fields->GetItem(sIDNoStr)->Value    = Msg.m_lIDNo;
      if (m_rAM.m_dwOptions & LogItem_IterNo) 
        m_pRst->Fields->GetItem(sIterNoStr)->Value  = Msg.m_lIterNo;
      if (m_rAM.m_dwOptions & LogItem_CallNo) 
        m_pRst->Fields->GetItem(sCallNoStr)->Value  = Msg.m_lCallNo;
      if (m_rAM.m_dwOptions & LogItem_SeqNo) 
        m_pRst->Fields->GetItem(sSeqNoStr)->Value   = Msg.m_lSeqNo;
      if (m_rAM.m_dwOptions & LogItem_Message) 
        m_pRst->Fields->GetItem(sMessageStr)->Value = Msg.Description();

      for (long fd=0; fd<m_Fields.GetSize(); fd++)
        {
        if (!m_Fields[fd]->m_bSystem)
          {
          COleVariant v((double)fd);
          m_pRst->Fields->GetItem((LPCTSTR)m_Fields[fd]->m_sFldTag)->Value=m_Fields[fd]->m_dCalculatedValue;
          }
        }

      m_pRst->Update();
      }
    catch (_com_error & e)
      {
      m_rAM.DisplayException(e, LF_Exclamation, "Adding Record ", (LPCTSTR)m_sName);
      return false;
      }
    }
  else
    {
    if (m_hFile==NULL)
      return false;

    char Separ = (m_iFormat==ADBTbFmt_CSV)?',':'\t';
    fprintf(m_hFile, "%i", ++m_nTxtRecNo);

    AddTimeFieldsTxt(CB, Separ);

    if (m_rAM.m_dwOptions & LogItem_Command) 
      fprintf(m_hFile, "%c%i", Separ, 0);//Msg.m_lIDNo);
    if (m_rAM.m_dwOptions & LogItem_Type) 
      fprintf(m_hFile, "%c%s", Separ, (LPCTSTR)Type);
    if (m_rAM.m_dwOptions & LogItem_Source) 
      fprintf(m_hFile, "%c%s", Separ, Msg.Source());
    if (m_rAM.m_dwOptions & LogItem_IDNo) 
      fprintf(m_hFile, "%c%i", Separ, Msg.m_lIDNo);
    if (m_rAM.m_dwOptions & LogItem_IterNo) 
      fprintf(m_hFile, "%c%i", Separ, Msg.m_lIterNo);
    if (m_rAM.m_dwOptions & LogItem_CallNo) 
      fprintf(m_hFile, "%c%i", Separ, Msg.m_lCallNo);
    if (m_rAM.m_dwOptions & LogItem_SeqNo) 
      fprintf(m_hFile, "%c%i", Separ, Msg.m_lSeqNo);
    if (m_rAM.m_dwOptions & LogItem_Message) 
      fprintf(m_hFile, "%c%s", Separ, Msg.Description());

    m_nSigDigits=Range(2,(int)m_nSigDigits,20);
    for (long fd=0; fd<m_Fields.GetSize(); fd++)
      {
      if (!m_Fields[fd]->m_bSystem)
        fprintf(m_hFile, "%c%.*g", Separ, m_nSigDigits, m_Fields[fd]->m_dCalculatedValue);
      }

    fprintf(m_hFile, "\n");
    fflush(m_hFile);
    }
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::AddEventRecord(CXM_TimeControl &CB, LPCTSTR Tag, LPCTSTR Msg)
  {
  if (!m_bOn)
    return true;

  CAutoIncDec AID(m_rAM.m_iBusyLoggingEvent);

  if (m_iFormat==ADBTbFmt_DB)
    {
    if (m_pRst==NULL)
      return false;

    try
      {
      m_pRst->AddNew();
      //COleDateTime DT;

      AddTimeFieldsDB(CB);

      if (m_rAM.m_dwOptions & LogItem_Tag) 
        m_pRst->Fields->GetItem(sTagStr)->Value=Tag;
      if (m_rAM.m_dwOptions & LogItem_Message) 
        m_pRst->Fields->GetItem(sMessageStr)->Value=Msg;

      for (long fd=0; fd<m_Fields.GetSize(); fd++)
        {
        if (!m_Fields[fd]->m_bSystem)
          {
          COleVariant v((double)fd);
          m_pRst->Fields->GetItem((LPCTSTR)m_Fields[fd]->m_sFldTag)->Value=m_Fields[fd]->m_dCalculatedValue;
          }
        }

      m_pRst->Update();
      }
    catch (_com_error & e)
      {
      m_rAM.DisplayException(e, LF_Exclamation, "Adding Record ", (LPCTSTR)m_sName);
      return false;
      }
    }
  else
    {

    if (m_hFile==NULL)
      return false;

    char Separ = (m_iFormat==ADBTbFmt_CSV)?',':'\t';
    fprintf(m_hFile, "%i", ++m_nTxtRecNo);

    AddTimeFieldsTxt(CB, Separ);

    if (m_rAM.m_dwOptions & LogItem_Tag) 
      fprintf(m_hFile, "%c%s", Separ, Tag);
    if (m_rAM.m_dwOptions & LogItem_Message) 
      fprintf(m_hFile, "%c%s", Separ, Msg);


    m_nSigDigits=Range(2,(int)m_nSigDigits,20);
    for (long fd=0; fd<m_Fields.GetSize(); fd++)
      {
      if (!m_Fields[fd]->m_bSystem)
        fprintf(m_hFile, "%c%.*g", Separ, m_nSigDigits, m_Fields[fd]->m_dCalculatedValue);
      }

    fprintf(m_hFile, "\n");
    fflush(m_hFile);
    }
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::CloseDBTable()
  {
  if (!m_bOn)
    return true;
  
  switch (m_iFormat)
    {
    case ADBTbFmt_DB:
      {
      try
        {
        if (m_pRst)
          {
          m_pRst->Close();
          m_pRst.Release();
          }

        //if (m_pDef)
        //  m_pDef->Close();
        //delete m_pDef;
        //m_pDef=NULL;
        }
      catch (_com_error & e)
        {
        m_rAM.DisplayException(e, LF_Exclamation, "Closing Table ", (LPCTSTR)m_sName);
        return false;
        }
      break;
      }
    case ADBTbFmt_CSV:
    case ADBTbFmt_TXT:
      {
      if (m_hFile)
        {
        if (fclose(m_hFile)==0)
          {
          m_hFile=NULL;
          }
        else
          {
          LogError(ArcName, 0, "Table File '%s' not Closed", m_sName);
          return false;
          }
        }
      break;
      }
    }

  return true;
  }

//===========================================================================

CArchiveManager::CArchiveManager() : \
m_evDone(FALSE)
  {
  //m_pDB=NULL;
  m_lChgSrchStart=0;
  m_bOn=false;
  m_bDBOK=false;
  m_bRegistered=false;
  //m_bOpenDBOnRun=true;

  //m_bUseArchive=false;
  m_bDBExclusive=false;
  m_bDBReadOnly=false;
  m_iLoadSaveCnt=0;

  m_pMessageLog=NULL; 
  m_pEventLog=NULL; 
  m_iBusyLoggingMsg=0;
  m_iBusyLoggingEvent=0;

  m_lInputOffset=0;
  m_lBusyChanges=0;

  m_dwOptions = 0;

  m_bFirstClear =true;
  };

//---------------------------------------------------------------------------

CArchiveManager::~CArchiveManager()
  {
  for (int s=0; s<m_Inputs.GetSize(); s++)
    delete m_Inputs[s];
  m_Inputs.SetSize(0);
  for (int i=0; i<m_Fields.GetSize(); i++)
    delete m_Fields[i];
  m_Fields.SetSize(0);
  for (int t=0; t<m_Tables.GetSize(); t++)
    delete m_Tables[t];
  m_Tables.SetSize(0);
  m_FieldMap.RemoveAll();

  if (m_sPrevTempFile.GetLength())
    DeleteFile(m_sPrevTempFile);
  }

//---------------------------------------------------------------------------

long CArchiveManager::FindInputIndex(LPCTSTR Tag) 
  {
  long n=m_Inputs.GetSize();
  for (long i=0; i<n; i++)
    if (_stricmp(m_Inputs[i]->m_sScdMTag, Tag)==0)
      return i;
  return -1;
  };

//---------------------------------------------------------------------------

long CArchiveManager::FindFieldIndex(LPCTSTR Tag) 
  { 
  long n=m_Fields.GetSize();
  for (long i=0; i<n; i++)
    if (_stricmp(m_Fields[i]->m_sScdMTag, Tag)==0)
      return i;
  return -1;
  };

//---------------------------------------------------------------------------

long CArchiveManager::FindTable(LPCTSTR TbName)
  { 
  const int NT=m_Tables.GetSize();
  for (long i=0; i<NT; i++)
    if (m_Tables[i]->m_sName.CompareNoCase(TbName)==0)
      return i;
  return -1;
  };

//---------------------------------------------------------------------------

long CArchiveManager::FindField(LPCTSTR TbName, LPCTSTR FldName)
  { 
  long iTb=FindTable(TbName);
  if (iTb>=0)
    return FindField(iTb, FldName);
  return -1;
  };

//---------------------------------------------------------------------------

long CArchiveManager::FindField(long iTb, LPCTSTR FldName)
  { 
  if (iTb>=0)
    {
    CArchiveDBTable &Tb=*m_Tables[iTb];
    long n=Tb.m_Fields.GetSize();
    for (long i=0; i<n; i++)
      if (Tb.m_Fields[i]->m_sFldTag.CompareNoCase(FldName)==0)
        return i;
    }
  return -1;
  };

//---------------------------------------------------------------------------

void CArchiveManager::Initialise()
  {
  m_sDataFile = "";

  for (int s=0; s<m_Inputs.GetSize(); s++)
    delete m_Inputs[s];
  for (int i=0; i<m_Fields.GetSize(); i++)
    delete m_Fields[i];
  for (int t=0; t<m_Tables.GetSize(); t++)
    delete m_Tables[t];
  m_FieldMap.RemoveAll();

  m_FieldMap.InitHashTable(53);
  m_Fields.SetSize(0,64);
  m_Inputs.SetSize(0,64);
  m_Tables.SetSize(0,16);

  m_lChgSrchStart=0;
  m_bOn=false;
  m_bDBOK=false;

  m_bDBExclusive=false;
  m_bDBReadOnly=false;
  m_iLoadSaveCnt=0;

  m_bRewind = false;
  }

// --------------------------------------------------------------------------

//int CArchiveManager::Configure(const char * CfgFile, bool OpenDBOnRun)
//  {
//  m_sCfgFile= CfgFile;
//  //m_bOpenDBOnRun=OpenDBOnRun;
//
//  return 0;
//  }

//---------------------------------------------------------------------------

void CArchiveManager::AddTimeFields(LPCTSTR Table)
  {
  if (m_dwOptions & LogItem_Time         )  AddField(Table, sDateTimeStr   ,      "Reserved");
  if (m_dwOptions & LogItem_TimeDate     )  AddField(Table, sDateStr       ,      "Reserved");
  if (m_dwOptions & LogItem_TimeTime     )  AddField(Table, sTimeStr       ,      "Reserved");
  if (m_dwOptions & LogItem_ElapsedHMS   )  AddField(Table, sElapsedHMSStr ,      "Reserved");
  if (m_dwOptions & LogItem_ElapsedDays  )  AddField(Table, sElapsedDaysStr,      "Reserved");
  if (m_dwOptions & LogItem_ElapsedHrs   )  AddField(Table, sElapsedHrsStr ,      "Reserved");
  if (m_dwOptions & LogItem_ElapsedMins  )  AddField(Table, sElapsedMinsStr,      "Reserved");
  if (m_dwOptions & LogItem_ElapsedSecs  )  AddField(Table, sElapsedSecsStr,      "Reserved");
  }

//---------------------------------------------------------------------------

void CArchiveManager::AddReqdTables()
  {
  CArchiveDBTable * pMsgTb=new CArchiveDBTable(this, sMessageLogStr, ADBTbFmt_DB, "Archive", 5);
  CArchiveDBTable * pEvtTb=new CArchiveDBTable(this, sEventLogStr,   ADBTbFmt_DB, "Archive", 5);
  m_Tables.Add(pMsgTb);
  m_Tables.Add(pEvtTb);

  AddTimeFields(sMessageLogStr);
  if (m_dwOptions & LogItem_Command      )  AddField(sMessageLogStr, sCommandStr    ,      "Reserved");
  if (m_dwOptions & LogItem_Type         )  AddField(sMessageLogStr, sTypeStr       ,      "Reserved");
  if (m_dwOptions & LogItem_Source       )  AddField(sMessageLogStr, sSourceStr     ,      "Reserved");
  if (m_dwOptions & LogItem_IDNo         )  AddField(sMessageLogStr, sIDNoStr       ,      "Reserved");
  if (m_dwOptions & LogItem_IterNo       )  AddField(sMessageLogStr, sIterNoStr     ,      "Reserved");
  if (m_dwOptions & LogItem_CallNo       )  AddField(sMessageLogStr, sCallNoStr     ,      "Reserved");
  if (m_dwOptions & LogItem_SeqNo        )  AddField(sMessageLogStr, sSeqNoStr      ,      "Reserved");
  if (m_dwOptions & LogItem_Message	     )  AddField(sMessageLogStr, sMessageStr	  ,      "Reserved");

  AddTimeFields(sEventLogStr);
  if (m_dwOptions & LogItem_Tag          )  AddField(sEventLogStr, sTagStr          ,      "Reserved");
  if (m_dwOptions & LogItem_Message	     )  AddField(sEventLogStr, sMessageStr	    ,      "Reserved");

  };

//---------------------------------------------------------------------------

DWORD CArchiveManager::ReadOptions(LPCTSTR FileName)
  {
  Strng Fn(FileName);
  Fn.FnExpand();

  CheckCfg(Fn());

  CProfINIFile PF(Fn());

  DWORD Opts = 0;

  LPCTSTR sDblOpt=PF.RdStr("Columns", "ElapsedDbl ", "");

  //if (PF.RdInt("Columns", "Time"       , 0)!=0) Opts |= LogItem_Time       ;//for backward compatability
  if (PF.RdInt("Columns", "DateTime"   , 0)!=0) Opts |= LogItem_Time       ;
  if (PF.RdInt("Columns", "Date"       , 0)!=0) Opts |= LogItem_TimeDate   ;
  if (PF.RdInt("Columns", "Time"       , 0)!=0) Opts |= LogItem_TimeTime   ;
  if (PF.RdInt("Columns", "ElapsedHMS" , 0)!=0) Opts |= LogItem_ElapsedHMS ;
  if (sDblOpt && stricmp(sDblOpt," d")==0) Opts |= LogItem_ElapsedDays;
  if (sDblOpt && stricmp(sDblOpt," h")==0) Opts |= LogItem_ElapsedHrs ;
  if (sDblOpt && stricmp(sDblOpt," m")==0) Opts |= LogItem_ElapsedMins;
  if (sDblOpt && stricmp(sDblOpt," s")==0) Opts |= LogItem_ElapsedSecs;
  if (PF.RdInt("Columns", "Command"    , 0)!=0) Opts |= LogItem_Command    ;
  if (PF.RdInt("Columns", "Type"       , 1)!=0) Opts |= LogItem_Type       ;
  if (PF.RdInt("Columns", "Tag"        , 1)!=0) Opts |= LogItem_Tag        ;
  if (PF.RdInt("Columns", "Source"     , 1)!=0) Opts |= LogItem_Source     ;
  if (PF.RdInt("Columns", "IDNo"       , 0)!=0) Opts |= LogItem_IDNo       ;
  if (PF.RdInt("Columns", "IterNo"     , 0)!=0) Opts |= LogItem_IterNo     ;
  if (PF.RdInt("Columns", "CallNo"     , 0)!=0) Opts |= LogItem_CallNo     ;
  if (PF.RdInt("Columns", "SeqNo"      , 0)!=0) Opts |= LogItem_SeqNo      ;
  if (PF.RdInt("Columns", "Message"    , 1)!=0) Opts |= LogItem_Message    ;

  if (PF.RdInt("MessageLog","On"       , 1)!=0) Opts |= LogItem_MessageLogOn;
  if (PF.RdInt("EventLog","On"         , 1)!=0) Opts |= LogItem_EventLogOn ;

  //if ((Opts & LogItem_ElapsedDbl)==0) 
  //  Opts |= LogItem_ElapsedSecs;

  return Opts;
  };

void CArchiveManager::WriteOptions(LPCTSTR FileName, DWORD Opts)
  {
  Strng Fn(FileName);
  Fn.FnExpand();

  CheckCfg(Fn());

  CProfINIFile PF(Fn());

  PF.WrInt("Columns", "DateTime"   , (Opts & LogItem_Time       )!=0 ? 1:0);
  //PF.WrInt("Columns", "Date"       , (Opts & LogItem_TimeDate   )!=0 ? 1:0);
  //PF.WrInt("Columns", "Time"       , (Opts & LogItem_TimeTime   )!=0 ? 1:0);
  PF.WrInt("Columns", "ElapsedHMS" , (Opts & LogItem_ElapsedHMS )!=0 ? 1:0);
  if (Opts & LogItem_ElapsedDays) PF.WrStr("Columns", "ElapsedDbl ", "d") ;
  if (Opts & LogItem_ElapsedHrs ) PF.WrStr("Columns", "ElapsedDbl ", "h") ;
  if (Opts & LogItem_ElapsedMins) PF.WrStr("Columns", "ElapsedDbl ", "m") ;
  if (Opts & LogItem_ElapsedSecs) PF.WrStr("Columns", "ElapsedDbl ", "s") ;
  if ((Opts & LogItem_ElapsedDbl)==0) PF.WrStr("Columns", "ElapsedDbl ", "") ;
  PF.WrInt("Columns", "Command"    , (Opts & LogItem_Command    )!=0 ? 1:0);
  PF.WrInt("Columns", "Type"       , (Opts & LogItem_Type       )!=0 ? 1:0);
  PF.WrInt("Columns", "Tag"        , (Opts & LogItem_Tag        )!=0 ? 1:0);
  PF.WrInt("Columns", "Source"     , (Opts & LogItem_Source     )!=0 ? 1:0);
  PF.WrInt("Columns", "IDNo"       , (Opts & LogItem_IDNo       )!=0 ? 1:0);
  PF.WrInt("Columns", "IterNo"     , (Opts & LogItem_IterNo     )!=0 ? 1:0);
  PF.WrInt("Columns", "CallNo"     , (Opts & LogItem_CallNo     )!=0 ? 1:0);
  PF.WrInt("Columns", "SeqNo"      , (Opts & LogItem_SeqNo      )!=0 ? 1:0);
  PF.WrInt("Columns", "Message"    , (Opts & LogItem_Message    )!=0 ? 1:0);

  PF.WrInt("MessageLog","On"       , (Opts & LogItem_MessageLogOn)!=0 ? 1:0);
  PF.WrInt("EventLog","On"         , (Opts & LogItem_EventLogOn )!=0 ? 1:0);
  }; 

//---------------------------------------------------------------------------

bool CArchiveManager::ReadCfg()
  {
  Strng DataFn, DataPath;

  Initialise();

  //if (!FileExists(C2T(m_sCfgFile)))
  //  return false;

  CheckCfg(C2T(m_sCfgFile));

  CProfINIFile PF(C2T(m_sCfgFile));

  m_dwOptions = ReadOptions(C2T(m_sCfgFile)); 

  AddReqdTables();

  m_sDataFile="";
  DataFn = PF.RdStr("Options", "DBFile", "$Prj\\Archive.MDB");
  DataFn.FnExpand();
  DataPath.FnDrivePath(DataFn());
  if (DataPath.Length()==0)
    {
    Strng P((LPCTSTR)m_sCfgFile);
    P.FnDrivePath();
    m_sDataFile+=P();
    }
  m_sDataFile+=DataFn();

  m_sSeparator = PF.RdStr("Options", "Separator", "-");
  if (m_sSeparator.GetLength()>1)
    m_sSeparator=m_sSeparator.Left(1);
  else if (m_sSeparator.GetLength()<1)
    m_sSeparator="-";

  for (int iTb=0; ; iTb++)
    {
    CString TbSect, TbName, TbFmt;
    bool IsOn=true;
    switch (iTb)
      {
      case 0:
        TbSect="MessageLog";
        TbName="MessageLog";
        break;
      case 1:
        TbSect="EventLog";
        TbName="EventLog";
        break;
      default: // 2 & up
        TbSect.Format("Table%i", iTb-1);
        TbName=PF.RdStr(TbSect, "Name", "");
        break;
      }
    if (TbName.GetLength()<=0)
      break;

    IsOn=PF.RdInt(TbSect, "On", 1)!=0;
    TbFmt=PF.RdStr(TbSect, "Format", "DB");
    byte iTbFmt = ADBTbFmt_Null;
    if      (TbFmt.GetLength()==0)                                iTbFmt=ADBTbFmt_DB;
    else if (TbFmt.CompareNoCase(ADBTbFmtTags[ADBTbFmt_DB])==0)   iTbFmt=ADBTbFmt_DB;
    else if (TbFmt.CompareNoCase(ADBTbFmtTags[ADBTbFmt_CSV])==0)  iTbFmt=ADBTbFmt_CSV;
    else if (TbFmt.CompareNoCase(ADBTbFmtTags[ADBTbFmt_TXT])==0)  iTbFmt=ADBTbFmt_TXT;
    else
      {
      LogError(ArcName, 0, "%s Invalid Table Format: %s", TbName, TbFmt);
      break;
      }

    CArchiveDBTable * pTb=NULL;
    const int NT=m_Tables.GetSize();
    for (int iTb=0; iTb<NT; iTb++)
      {
      if (TbName.CompareNoCase(m_Tables[iTb]->m_sName)==0)
        {
        pTb=m_Tables[iTb];
        break;
        }
      }
    if (pTb==NULL)
      {
      pTb=new CArchiveDBTable(this, TbName) ;
      m_Tables.Add(pTb);
      AddTimeFields(TbName);
      }
    // NBNBNB No Checking
    CTimeValue P,O;
    pTb->m_sCfgSection  = TbSect;
    pTb->m_iFormat      = iTbFmt;
    pTb->m_bOn          = IsOn;

    pTb->m_sPath        = PF.RdStr(TbSect, "Path", "$Prj\\Archive");
    pTb->m_nSigDigits   = PF.RdInt(TbSect, "SigDigits", pTb->m_nSigDigits);
    CString ClrStr      = PF.RdStr(TbSect, "Clear", "Rewind");
    pTb->m_bClearOnRewind = ClrStr.Find("Rewind")>=0;
    //pTb->m_bClearOnRewind = ClrStr.Find("Rewind");

    //TbFmt=PF.RdStr(TbSect, "Name", "");

#if dbgArchiver
    if (dbgReadCfg())
      dbgpln("Tb:%-20s",(LPCTSTR)pTb->m_sName);
#endif

    P = P.Parse(PF.RdStr(TbSect, "Period", "")) ? P : -1.0;
    O = O.Parse(PF.RdStr(TbSect, "Offset", "")) ? O : 0.0;
    pTb->AddPeriod((long)P.Seconds, (long)O.Seconds);

    for (int j=0; ;j++)
      {
      CString TrigNm, TrigOpt;
      TrigNm.Format("Trigger%i", j+1);
      TrigOpt=PF.RdStr(TbSect, TrigNm, "");
      if (TrigOpt.GetLength()<=0)
        break;
      pTb->AddTrigger(TrigOpt);
      }

    for (int f=0; ;f++)
      {
      CString FldNm, FldOpt;
      FldNm.Format("Field%i", f+1);
      FldOpt=PF.RdStr(TbSect, FldNm, " ");
      if (FldOpt.GetLength()<=1)
        break;
      DoAddField(pTb->m_sName, FldNm, FldOpt);
      }
    };

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::CheckCfg(LPCTSTR CfgFile)
  {
  //if (CfgFile==NULL)
  //  CfgFile=m_sCfgFile;
  //Strng Fn(m_sCfgFile);
  if (!FileExists(C2T(CfgFile)))
    {
    FILE *h=fopen(C2T(CfgFile), "wt");
    if (h)
      {
      fprintf(h, "%s", s_sArchiveContent);
      fclose(h);
      }
    else
      {
      LogError("Archive", 0, "File %s not Opened", CfgFile);
      return false;
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::CompleteOpen()
  {
  //ASSERT_ALWAYS(!XBusy(), "CompleteOpen - SysCAD Running", __FILE__, __LINE__);
  bool OK=true;

  const int NT=m_Tables.GetSize();
  for (long t=0; t<NT; t++)
    {
    CArchiveDBTable & Tb=*m_Tables[t];
    Tb.m_Fields.SetSize(0,64);
    }

  long nf=m_Fields.GetSize();
  for (long f=0; f<nf; f++)
    {
    CArchiveDBField *pFld=m_Fields[f];
    pFld->m_lTableIndex=FindTable(pFld->m_sTable);
    if (pFld->m_lTableIndex>=0)
      {
      if (FindField(pFld->m_lTableIndex, "")<0)
        m_Tables[pFld->m_lTableIndex]->m_Fields.Add(pFld);
      else
        LogError(ArcName,0,"Field %s already exists in Table %s", (LPCTSTR)pFld->m_sFldTag, (LPCTSTR)pFld->m_sTable);
      }
    else
      {
      LogError(ArcName,0,"Table %s not found for Field %s", (LPCTSTR)pFld->m_sTable, (LPCTSTR)pFld->m_sFldTag);
      OK=false;
      }
    }

  m_lInputOffset=m_Fields.GetSize();

  return OK;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::OpenTheDB()
  {
  bool OpenOK=false;

  dbgpln("Manager::Open");

  m_nFieldsOpn=0;
  m_nTablesOpn=0;
  dbgpln("Manager::OpenTheDB");

  OpenOK = CompleteOpen();
  OpenOK = OpenOK && OpenDB();
  OpenOK = OpenOK && CreateDBTables();
  OpenOK = OpenOK && CreateDBRecordsets();

  m_pMessageLog=NULL;
  m_pEventLog=NULL;
  const int NT=m_Tables.GetSize();
  for (int i=0; i<NT; i++)
    {
    if (m_Tables[i]->m_sName.CompareNoCase(sMessageLogStr)==0)    
      m_pMessageLog=m_Tables[i];
    if (m_Tables[i]->m_sName.CompareNoCase(sEventLogStr)==0)    
      m_pEventLog=m_Tables[i]; 
    }
  m_iBusyLoggingMsg=0;
  m_iBusyLoggingEvent=0;

  m_pMessageLog->m_bClearOnRewind=false;

  if (m_pMessageLog==NULL)
    LogNote(ArcName, 0, "%s Table not Found", sMessageLogStr);
  if (m_pEventLog==NULL)
    LogNote(ArcName, 0, "%s Table not Found", sEventLogStr);

  if (OpenOK)
    LogNote(ArcName, 0, "Archive reporter: %i fields in %i tables", m_nFieldsOpn, m_nTablesOpn);
  else
    LogNote(ArcName, 0, "Database not opened");

  return OpenOK;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::Start()
  {
  if (!m_bOn)
    return true;

  dbgpln("Manager::Start");

  m_TimeAtStartValid = true;

  //if (m_bOn && m_bOpenDBOnRun)
  //  m_bDBOK = OpenTheDB();

  return m_bDBOK;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::Stop()
  {
  if (!m_bOn)
    return true;

  dbgpln("Manager::Stop");

  //if (m_bOn && m_bOpenDBOnRun)
  //  CloseDB();

  m_TimeAtStartValid = false;

  return true;
  }

// --------------------------------------------------------------------------

static void sfn_EventLogHook(LPCTSTR Tag, LPCTSTR Msg)
  {
  gs_pArcMan->LogAnEvent(Tag, Msg);
  };

int CArchiveManager::Open(LPCTSTR CfgFile, bool FirstClear)
  {
  CDlgBusy::Open("\nOpening Archive Manager");
  int Err = 0;
  dbgpln("DRV CArchiveManager::Open(%s)", CfgFile);

  m_sCfgFile= CfgFile;

  Initialise();
  m_bFirstClear = FirstClear;
  m_bOn = false;
  if (m_sCfgFile.GetLength()>0)
    {

    m_bSubsRead_Busy = 0;

    CDlgBusy::SetLine(1, "Opening Archive Manager : %s", (LPCTSTR)m_sCfgFile);

    if (!m_bRegistered)
      {
      EO_Register(pExecName_ArcMngr, EOWrite_Thread | EORead_Thread | EOExec_Thread,/*Pri*/THREAD_PRIORITY_ABOVE_NORMAL,/*Stack*/50000, /*HashSize*/ 101);
      m_bRegistered=true;
      }

    bool OK=ReadCfg();

    gs_Exec.ConfigureArchive();

    m_bOn=OK;

    if (m_bOn)// && !m_bOpenDBOnRun)
      m_bDBOK = OpenTheDB();

    if (!m_bOn)
      {
      CloseDB();
      Initialise();
      }

    if (!m_bOn)
      LogError(ArcName, 0, "'%s' not opened", (LPCTSTR)m_sCfgFile);
#if dbgArchiverMan
    if (dbgDumpConfig())
      ConfigurationDump();
#endif
    }

  if (m_bOn)
    {

    if (m_sPrevTempFile.GetLength())
      {//restore previous state...
      FilingControlBlock FCB(PrjFiles(), DefaultCfgLabel, TOF_DBase);
      HANDLE H = CreateFile((LPCTSTR)m_sPrevTempFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
      if (H!=INVALID_HANDLE_VALUE)
        {
        DWORD dwSize = GetFileSize(H, NULL);
        if (dwSize>0 && dwSize!=0xFFFFFFFF)
          {
          char Buff[9];
          FCB.InitialiseForRead(H, 0, dwSize);
          //SetFilePointer(H, 0, NULL, FILE_BEGIN);
          FCB.ReadFile((LPVOID)Buff, 9, &dwSize);
          ASSERT(strncmp("TAGS....", Buff, 8)==0);
          if (!FCB.Eof())
            LoadTags(FCB);
          CloseHandle(H);
          }
        }
      }

    //is it safe to assume the historian EO is opened before the Archive manager EO ???
    //RecordSlots();
    }
  else
    Err = 2;

  gs_EventLog.SetHook(sfn_EventLogHook);

  XBuildDataLists(); //otherwise if connect column tags are removed, and the Archive is reloaded, SysCAD crashes


  CDlgBusy::Close();

  return Err;
  }

// --------------------------------------------------------------------------

bool CArchiveManager::ConnectAll()
  {
  for (int s=0; s<m_Inputs.GetSize(); s++)
    delete m_Inputs[s];
  m_Inputs.SetSize(0);

  const int NT=m_Tables.GetSize();
  for (long t=0; t<NT; t++)
    {
    CArchiveDBTable & Tb=*m_Tables[t];
    const long ntt=Tb.m_Triggers.GetSize();
    for (long tt=0; tt<ntt; tt++)
      {
      CArchiveTrigger &Tr=*Tb.m_Triggers[tt];
      Tr.m_Src.SetIndex(*this);
      CCnvIndex iCnv=Tr.m_Src.GetCnvIndex(*this);
      if (iCnv && Cnvs[iCnv]->Find(Tr.m_sCnvTxt())==NULL)
        LogError(ArcName, 0, "Conversion units '%s' are invalid for %s", Tr.m_sCnvTxt(), Tr.m_Src.m_sTag);
      }
    }

  const long nf=m_Fields.GetSize();
  for (long f=0; f<nf; f++)
    {
    CArchiveDBField & Fld=*m_Fields[f];
    if (!Fld.m_bSystem)
      {
      Fld.m_Src.SetIndex(*this);
      CCnvIndex iCnv=Fld.m_Src.GetCnvIndex(*this);
      if (iCnv && Cnvs[iCnv]->Find(Fld.m_sCnvTxt())==NULL)
        LogError(ArcName, 0, "Conversion units '%s' are invalid for %s", Fld.m_sCnvTxt(), Fld.m_Src.m_sTag);
      }
    }

  return true;
  }

// --------------------------------------------------------------------------

bool CArchiveManager::DisConnectAll()
  {
  for (int s=0; s<m_Inputs.GetSize(); s++)
    delete m_Inputs[s];
  m_Inputs.SetSize(0);

  return true;
  }

// --------------------------------------------------------------------------

bool CArchiveManager::Close(BOOL SaveState/*=true*/)
  {
  if (!m_bOn)
    return true;

  gs_EventLog.SetHook(NULL);

  m_pMessageLog=NULL; 
  m_pEventLog=NULL; 
  m_iBusyLoggingMsg=0;
  m_iBusyLoggingEvent=0;

  if (m_bOn)
    CDlgBusy::Open("\n\nClosing Archive Manager");

  if (m_bOn)// && !m_bOpenDBOnRun)
    CloseDB();

  if (m_sPrevTempFile.GetLength())
    {
    DeleteFile((LPCTSTR)m_sPrevTempFile);
    m_sPrevTempFile = "";
    }

  if (SaveState)
    {
    if (m_sPrevTempFile.GetLength()==0)
      {
      char File[MAX_PATH];
      if (GetTempFileName(TemporaryFiles(), "ARC", 0, File))
        m_sPrevTempFile = File;
      else
        m_sPrevTempFile = "";
      }
    if (m_sPrevTempFile.GetLength())
      {
      FilingControlBlock FCB(PrjFiles(), DefaultCfgLabel, TOF_DBase);
      HANDLE H = CreateFile((LPCTSTR)m_sPrevTempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
      if (H!=INVALID_HANDLE_VALUE)
        {
        FCB.InitialiseForWrite(H, 0);
        SaveTags(FCB, true);
        CloseHandle(H);
        }
      }
    }

  dbgpln("DRV CArchiveManager::Close");
  if (pArcSheet)
    {
    pArcSheet->PressButton(PSBTN_CANCEL);
    delete pArcSheet;
    }
  if (m_bOn)
    CDlgBusy::Close();

  if (m_bRegistered)
    {
    EO_DeRegister();
    m_bRegistered=false;
    }

  Initialise();
  return true;
  }

//---------------------------------------------------------------------------

void CArchiveManager::Restart()
  {
  RestartDBTables();
  //Close();
  //Open();
  }

//---------------------------------------------------------------------------

long CArchiveManager::AddField(LPCTSTR Table, LPCTSTR Field, LPCTSTR CfgString)
  {
  CString S;
  S.Format("\"%s\", %s", Field, CfgString);
  return DoAddField(Table, Field, S);
  };

//---------------------------------------------------------------------------

long CArchiveManager::DoAddField(LPCTSTR Table, LPCTSTR Field, LPCTSTR CfgStringIn)
  {
  if (CfgStringIn==NULL || strlen(CfgStringIn)==0)
    return 0;

  CSVColArray f;
  int Quote;
  CString ADBCfg=CfgStringIn;

  CString CfgString=CfgStringIn;
  int nParms = ParseCSVTokens(C2T(CfgString), f, Quote, NULL, true);
  if (Quote || f[0]==NULL || nParms<1)
    {
    LogError(ArcName, 0, "Table %s, %s: Invalid archive field specification: %s", Table, Field, (LPCTSTR)ADBCfg);
    return -1;
    }

  const bool HasFldName = (ADBCfg.GetLength()>0 && ADBCfg[0]=='"');
  int iTagFld     = HasFldName?1:0;

  LPCTSTR FldName = HasFldName ? f[0]:NULL; 

  int iST=-1;
  if (HasFldName)
    {
    for (iST=sizeof(s_SystemTables)/sizeof(s_SystemTables[0])-1; iST>=0; iST--)
      {
      CSystemTable ST = s_SystemTables[iST];
      if ((ST.m_Table==NULL || stricmp(ST.m_Table, Table)==0) &&  (stricmp(ST.m_FldName, FldName)==0))
        break;
      }
    }

  CSystemTable ST = s_SystemTables[iST];

  LPCTSTR Tag     = f[iTagFld];     if (Tag  ==NULL) Tag  ="";
  LPCTSTR Fn      = f[iTagFld+1];   if (Fn   ==NULL) Fn   ="";
  LPCTSTR Meas    = f[iTagFld+2];   if (Meas ==NULL) Meas ="";
  LPCTSTR First   = f[iTagFld+3];   if (First==NULL) First="";
  
  if (Tag==NULL || strlen(Tag)==0)
    {
    LogError(ArcName, 0, "Table %s, %s: Tag not specified", Table, Field);
    return -1;
    }

  byte iFn=ADBFn_Null;
  if      (_stricmp(Fn, ADBFuncTags[ADBFn_Instant])==0    || _stricmp(Fn, ADBFuncSyms[ADBFn_Instant])==0     ) iFn=ADBFn_Instant;
  else if (_stricmp(Fn, ADBFuncTags[ADBFn_Minimum])==0    || _stricmp(Fn, ADBFuncSyms[ADBFn_Minimum])==0     ) iFn=ADBFn_Minimum;
  else if (_stricmp(Fn, ADBFuncTags[ADBFn_Maximum])==0    || _stricmp(Fn, ADBFuncSyms[ADBFn_Maximum])==0     ) iFn=ADBFn_Maximum;
  else if (_stricmp(Fn, ADBFuncTags[ADBFn_PeriodAvg])==0  || _stricmp(Fn, ADBFuncSyms[ADBFn_PeriodAvg])==0   ) iFn=ADBFn_PeriodAvg;
  //else if (_stricmp(Fn, ADBFuncTags[ADBFn_RunningAvg])==0 || _stricmp(Fn, ADBFuncSyms[ADBFn_RunningAvg])==0  ) iFn=ADBFn_RunningAvg
  else if (_stricmp(Fn, ADBFuncTags[ADBFn_Count])==0      || _stricmp(Fn, ADBFuncSyms[ADBFn_Count])==0       ) iFn=ADBFn_Count;
  else if (iST<0)
    {
    if (Fn==NULL || strlen(Fn)==0)
      LogError(ArcName, 0, "Table %s, %s: For tag '%s', a Function has not been specified", Table, Field, Tag);
    else
      LogError(ArcName, 0, "Table %s, %s: Tag '%s' has invalid Function (%s)", Table, Field, Tag, Fn);
    return -1;
    }

  byte iMeas=ADBMeas_Null;
  if      (Meas==NULL || strlen(Meas)==0)                    iMeas=ADBMeas_EndPt;
  else if (_stricmp(Meas, ADBMeasTags[ADBMeas_StartPt])==0)  iMeas=ADBMeas_StartPt;
  else if (_stricmp(Meas, ADBMeasTags[ADBMeas_MidPt])==0)    iMeas=ADBMeas_MidPt;
  else if (_stricmp(Meas, ADBMeasTags[ADBMeas_EndPt])==0)    iMeas=ADBMeas_EndPt;
  else if (iST<0)
    {
    LogError(ArcName, 0, "Table %s, %s: Tag '%s' has invalid Measurement (%s)", Table, Field, Tag, Meas);
    return -1;
    }

  byte iFirst=ADBFirst_Null;
  if      (First==NULL || strlen(First)==0)                     iFirst=ADBFirst_Use;
  else if (_stricmp(First, ADBFirstTags[ADBFirst_Use])==0)      iFirst=ADBFirst_Use;
  else if (_stricmp(First, ADBFirstTags[ADBFirst_Ignore])==0)   iFirst=ADBFirst_Ignore;
  else if (iST<0)
    {
    LogError(ArcName, 0, "Table %s, %s: Tag '%s' has invalid First Point Usage (%s)", Table, Tag, First);
    return -1;
    }

  byte iType=tt_Double; // Assume all field types are double except ...
  byte iSize=0;

  Strng WrkTag,WrkCnvTxt;
  TaggedObject::SplitTagCnv((LPTSTR)Tag, WrkTag, WrkCnvTxt);

  if (iST>=0)
    {
    iType   = ST.m_Type;
    iSize   = ST.m_Size;
    iFn     = ADBFn_System;
    iMeas   = ADBMeas_Null;
    iFirst  = ADBFirst_Null;
    }

  return AddField((iST>=0), WrkTag(), Table, FldName, iFn, iMeas, iFirst, iType, iSize, 0,  WrkCnvTxt() ? WrkCnvTxt():"","");
  };

//---------------------------------------------------------------------------

long CArchiveManager::AddField(bool System, LPCTSTR Tag, LPCTSTR Table, LPCTSTR FldName, byte iFn, byte iMeas, byte iFirst, byte iType, int iSize, CCnvIndex iCnv, LPCTSTR CnvTxt, LPCTSTR Description)
  {
  long Index=-1;
  if (iFn!=ADBFn_Null)
    {
    CArchiveDBField *pFld   = new CArchiveDBField(this);

    pFld->m_bSystem         = System;
    pFld->m_sTable          = Table;
    pFld->m_iFn             = iFn;
    pFld->m_iMeas           = iMeas;
    pFld->m_iFirstOnRewind  =  iFirst;
    if (iFn == ADBFn_Count)
      {
      pFld->m_iType         = tt_Long;
      pFld->m_iSize         = iSize;
      pFld->m_sCnvTxt       = "";
      }
    else
      {
      pFld->m_iType         = iType;
      pFld->m_iSize         = iSize;
      pFld->m_sCnvTxt       = CnvTxt;
      }
    pFld->m_sDescription    = Description;

    pFld->m_Src.m_sTag      = Tag;


    if (FldName)
      pFld->m_sFldTag=FldName;
    else
      pFld->m_sFldTag.Format("%s-%s", Tag, ADBFuncTags[iFn]);
    // fix FldTag - no special chars
    if (strpbrk(pFld->m_sFldTag, InvalidFldNmChars)!=NULL)
      {
      CString Old(pFld->m_sFldTag);
      char *p, *pT=pFld->m_sFldTag.GetBuffer(0);
      while ((p=strpbrk(pT, InvalidFldNmCharsM))!=NULL)
        memmove(p, p+1, strlen(p+1)+1);
      while ((p=strpbrk(pT, InvalidFldNmCharsD))!=NULL)
        *p=m_sSeparator[0];
      if (FldName)
        LogNote(ArcName, 0, "Tag %s changed to %s", (LPCTSTR)Old, (LPCTSTR)pFld->m_sFldTag);
      pFld->m_sFldTag.ReleaseBuffer();
      }
    if (1)
      pFld->m_sScdMTag.Format("%s.%s", (LPCTSTR)pFld->m_sTable, (LPCTSTR)pFld->m_sFldTag);
    else
      pFld->m_sScdMTag.Format("%s.%s.%s", (LPCTSTR)pFld->m_sTable, Tag, ADBFuncTags[iFn]);

    Index=m_Fields.Add(pFld);
    m_FieldMap.SetAt(pFld->m_sScdMTag, Index);

#if dbgArchiver
    if (dbgConfig())
      {
      dbgpln("AddField [%3i] %-6s %-10s %-20s %-20s %-30s %-40s %s", 
        Index,
        pFld->m_bSystem?"System":"",
        tt_TypeString(pFld->m_iType),
        (LPCTSTR)pFld->m_sTable,
        (LPCTSTR)pFld->m_Src.m_sTag,
        (LPCTSTR)pFld->m_sFldTag,
        (LPCTSTR)pFld->m_sScdMTag,
        (LPCTSTR)pFld->m_sDescription);
      }
#endif

    }

  return Index;
  };

//---------------------------------------------------------------------------

long CArchiveManager::AppendField(bool System, LPCTSTR Tag, LPCTSTR Table, LPCTSTR FldName, byte iFn, byte iMeas, byte iFirst, byte iType, int iSize, CCnvIndex iCnv, LPCTSTR CnvTxt, LPCTSTR Description)
  {
  long Index=-1;
  int iTb=FindTable(Table);
  if (iTb>=0)
    {
    Index=AddField(System, Tag, Table, FldName, iFn, iMeas, iFirst, iType, iSize, iCnv, CnvTxt, Description);

    if (Index>=0)
      {
      // Reopen Table / RecordSet
      CArchiveDBTable & Tb=*m_Tables[iTb];
      bool OK;
      if (0)//m_bOpenDBOnRun)
        {
        OK=CompleteOpen();
        }
      else
        {
        OK=Tb.CloseDBTable();
        OK=OK && CompleteOpen();
        OK=OK && Tb.CreateDBTable();
        OK=OK && Tb.CreateDBRecordset();
        }

      if (OK)
        {
        // Save in Config file 
        Strng S,F,X;
        S.Set("%s,%s", Tag, ADBFuncTags[iFn]);

        CProfINIFile PF(C2T(m_sCfgFile));
        for (int f=0; f<250; f++)
          {
          F.Set("Field%i", f+1);
          X=PF.RdStr(Tb.m_sCfgSection, F(), "?");
          if (X=="?")
            break;
          }
        if (f<250)
          PF.WrStr(Tb.m_sCfgSection, F(), S());
        else
          LogNote(ArcName, 0, "Too many fields in Table %s", Table);
        }
      else
        LogNote(ArcName, 0, "Table %s not ReOpened", Table);
      }
    }
  else
    LogNote(ArcName, 0, "Table %s not found", Table);

  return Index;
  }

//---------------------------------------------------------------------------

long CArchiveManager::AddInput(LPCTSTR Tag)
  {
  if (Tag==NULL || strlen(Tag)==0)
    return -1;

  CXM_ObjectTag ObjTag((LPTSTR)Tag, TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route Route;

  long Index=-1;
  if (XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem & Item =*ObjData.FirstItem();
    CArchiveInput * pInput=new CArchiveInput;
    pInput->m_sScdMTag=Tag;
    pInput->m_iType=Item.Type();
    pInput->m_iCnv=Item.CnvIndex();

    if (IsNumData(pInput->m_iType))
      {
      if (IsFloatData(pInput->m_iType))
        pInput->PutValue(Item.Value()->GetDouble());
      else
        pInput->PutValue(Item.Value()->GetLong());
      Index=m_Inputs.Add(pInput);
#if dbgArchiver
      if (dbgConfig())
        dbgpln("AddInput %-20s %-10s ", (LPCTSTR)pInput->m_sScdMTag, tt_TypeString(pInput->m_iType));
#endif
      }
    else
      LogError(ArcName, 0, "Tag '%s' not numeric.", Tag);
    }
  else
    {
    LogError(ArcName, 0, "Tag '%s' not found.", Tag);
    }

  return Index;
  };

//---------------------------------------------------------------------------

bool CArchiveManager::Execute(CXM_TimeControl &CB)
  {
  if (!m_bOn)
    return false;

  //dbgpln("CArchiveManager::Execute");

  const int NI=m_Fields.GetSize();
  for (int i=0; i<NI; i++)
    m_Fields[i]->Execute(CB);

  const int NT=m_Tables.GetSize();
  for (int t=0; t<NT; t++)
    {
    m_Tables[t]->Execute(CB);
    }
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveManager::StepDone(CXM_TimeControl &CB)
  {
  if (!m_bOn)
    return false;

  //dbgpln("CArchiveManager::StepDone");

  const int NI=m_Fields.GetSize();
  for (int i=0; i<NI; i++)
    m_Fields[i]->StepDone(CB);

  const int NT=m_Tables.GetSize();
  for (int t=0; t<NT; t++)
    {
    m_Tables[t]->StepDone(CB);
    }

  m_bRewind = false;
  return true;
  };

//---------------------------------------------------------------------------

long CArchiveManager::NoOfChanges()
  {
  if (!m_bOn)
    return 0;

  long N=0;
  for (long i=0; i<m_Fields.GetSize(); i++) 
    if (m_Fields[i]->m_bChanged) 
      N++; 
  return N;
  }

//---------------------------------------------------------------------------

long CArchiveManager::GetNextChange()
  {
  if (!m_bOn)
    return -1;

  long N=m_Fields.GetSize();
  int j=m_lChgSrchStart;
  for (long i=0; i<N; i++) 
    {
    if (j>=N)
      j=0;
    if (m_Fields[j]->m_bChanged) 
      {
      m_lChgSrchStart=j+1;
      return j;
      }
    j++;
    }
  return -1;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::OpenDB()
  {
#if dbgArchiver
  if (dbgDBState())
    dbgpln("OpenDB");
#endif

  m_bDBExclusive=false;
  m_bDBReadOnly=false;
  m_sDBConnect="";

  //AfxDaoInit();

  WIN32_FIND_DATA fd;
  ASSERT(m_pCat==NULL);
  ASSERT(m_pCnn==NULL);
  if (!FileExists(C2T(m_sDataFile), fd))
    {
    try
      {
      m_pCnn=ADODB::_ConnectionPtr(__uuidof(ADODB::Connection));
      m_pCat=ADOX::_CatalogPtr(__uuidof (ADOX::Catalog));

      CString S;
      S.Format("Provider='Microsoft.JET.OLEDB.4.0';"
        "Data source = %s;"
        "Jet OLEDB:Engine Type=5", (LPCTSTR)m_sDataFile);
      m_pCat->Create(_bstr_t(S));
      m_pCnn=m_pCat->GetActiveConnection();
      m_pCnn->Close();
      m_pCat.Release();
      m_pCnn.Release();
      }
    catch(_com_error & e)
      {
      DisplayException(e, LF_Exclamation, "Create Archive ", (LPCTSTR)m_sDataFile);
      m_pCat.Release();
      m_pCnn.Release();
      return false;
      }
    }

  // Close & then reopen to allow non exclusive access
  if (FileExists(C2T(m_sDataFile), fd))
    {
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError(ArcName, LF_Exclamation, "Cannot open read-only Database '%s'", (LPCTSTR)m_sDataFile);
      return false;
      }
    try 
      {
      m_pCnn=ADODB::_ConnectionPtr(__uuidof(ADODB::Connection));
      m_pCat=ADOX::_CatalogPtr(__uuidof (ADOX::Catalog));
      CString S;
      S.Format("Provider='Microsoft.JET.OLEDB.4.0';"
        "Data source = %s;", (LPCTSTR)m_sDataFile);
      //m_pCat->ActiveCollection(_bstr_t(S));
      m_pCnn->Open(_bstr_t(S), "", "", ADODB::adConnectUnspecified);
      m_pCat->PutActiveConnection(_variant_t((IDispatch*)m_pCnn));
      }
    catch (_com_error & e)
      {
      DisplayException(e, LF_Exclamation, "Open Archive ", (LPCTSTR)m_sDataFile);
      m_pCat.Release();
      m_pCnn.Release();
      //delete m_pDB;
      //m_pDB=NULL;
      return false;
      }
    }

#if dbgArchiver
  if (dbgDBState())
    dbgpln("OpenDB - done");
#endif
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveManager::CreateDBTables()
  {
  bool OK=true;
  const int NT=m_Tables.GetSize();
  for (int t=0; t<NT; t++)
    {
    CArchiveDBTable & Tb = *m_Tables[t];
    if (Tb.m_bOn)
      {
      if (!Tb.CreateDBTable())
        OK=false;
      }
    else
      {
      if (!Tb.DeleteDBTable())
        OK=false;
      }
    }
#if dbgArchiver
  if (dbgDBState())
    dbgpln("CreateDBTables %s",OK?"OK":"BAD");
#endif
  return OK;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::RestartDBTables()
  {
  bool OK=true;
  const int NT=m_Tables.GetSize();
  for (int t=0; t<NT; t++)
    {
    CArchiveDBTable & Tb = *m_Tables[t];
    if (Tb.m_bOn)
      {
      switch (Tb.m_iFormat)
        {
        case ADBTbFmt_DB:
          Tb.DeleteDBRecords();
          break;
        case ADBTbFmt_CSV:
        case ADBTbFmt_TXT:
          Tb.ReOpenTxtTable(true);
          break;
        }
      }
    }
  return OK;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::CreateDBRecordsets()
  {
  bool OK=true;
  const int NT=m_Tables.GetSize();
  for (int t=0; t<NT; t++)
    {
    if (!m_Tables[t]->CreateDBRecordset())
      OK=false;
    }
#if dbgArchiver
  if (dbgDBState())
    dbgpln("CreateDBRecordSets %s",OK?"OK":"BAD");
#endif
  return OK;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::CloseDB()
  {
  dbgpln("Manager::CloseDB");

  if (m_pCnn!=NULL)
    {
    try
      {
      const int NT=m_Tables.GetSize();
      for (int t=0; t<NT; t++)
        m_Tables[t]->CloseDBTable();
      if (m_pCnn!=NULL && m_pCnn->State!=ADODB::adStateClosed)
        m_pCnn->Close();
      m_pCnn.Release();
      m_pCat.Release();
      }
    catch (_com_error & e)
      {
      DisplayException(e, LF_Exclamation, "Close Database ", (LPCTSTR)m_sDataFile );
      m_pCnn.Release();
      m_pCat.Release();
      //delete m_pDB;
      //m_pDB=NULL;
      return false;
      }
#if dbgArchiver
    if (dbgDBState())
      dbgpln("CloseDB");
#endif
    }
  return true;
  };

//---------------------------------------------------------------------------

void CArchiveManager::DisplayException(_com_error & e, DWORD LogOpts, LPCTSTR strMsg1, LPCTSTR strMsg2)
  {
  CString message;
  if (1)//e)
    {
    message=strMsg1;
    message+=strMsg2;
    CString strErrCode;
    if (1)//e->m_pErrorInfo)
      {
      strErrCode.Format("\nException: Error Code = %ld\n", e.Error());
      message += strErrCode;
      message += (LPCTSTR)e.Source();
      message += _T(": ");
      message += (LPCTSTR)e.Description();
      }
    //for (long ie=0; ie<m_pcnn->errors->count; ie++)
    //  {
    //  message += "\n";
    //  adodb::errorptr pe=m_pcnn->errors->getitem(ie);
    //  message += (lpctstr)pe->description;
    //  }
    //else
    //  {
    //  strErrCode.Format("\nCDaoException: SCODE=%ld  AfxDaoError=%ld\n", e->m_scode, e->m_nAfxDaoError);
    //  message += strErrCode;
    //  }
    //AfxMessageBox(message);
    }
  else
    message="CDaoException e==NULL ????";
  //AfxMessageBox("CDaoException e==NULL ????");

  if (!m_iBusyLoggingMsg)
    LogError(ArcName, LogOpts, C2T(message));
  }

//---------------------------------------------------------------------------

bool CArchiveManager::SendStart()
  {
  AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_ARCHIVESTART, 0);
  return m_bOn;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::SendExecute()
  {
  AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_ARCHIVEEXECUTE, 0);
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::SendStepDone()
  {
  AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_ARCHIVESTEPDONE, 0);
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveManager::SendStop()
  {
  AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_ARCHIVESTOP, 0);
  return true;
  }

//===========================================================================
//
//
//
//===========================================================================

CArchiveManager* gs_pArcMan = NULL;

//---------------------------------------------------------------------------

DWORD CArchiveManager::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
#if dbgArchiver
  DWORD StartTime = GetTickCount();
#endif

  DWORD RetCode = 0;
  flag KeepingArchive=false;
  long LastIndex=-1;
  flag KeepingHistory=0;

  while (XM.MsgAvail())
    switch (XM.MsgId())
    {
      case XM_ArchiveExists:
        {
        CXM_ArchiveExists* pX = XM.ArchiveExists();
        char* pTag=pX->FirstTag();
        while (strlen(pTag))
          {
          long i = FindInputIndex(pTag);
          RetCode=0;
          if (i>=0)
            {
            RetCode |= RC_SM_ArcExists; //exists in historian
            //if (Slots[i]->Data.bRecordIt)
            //  RetCode |= RC_SM_ArcRecOn; //recording is on
            //if (Slots[i]->Data.bDriver)
            //  RetCode |= RC_SM_ArchiveTag; //archive tag
            }
          pX->SetFlags(pTag, RetCode);
          pTag=pX->NextTag(pTag);
          }
        XM.RewindReadPosition();//ClearBuff();
        return RetCode;
        }

      case XM_KeepArchive:
        {
        CXM_KeepArchive *pX = XM.KeepArchive();
        AddField(false, pX->Tag(), pX->TableName(), pX->FldName(), pX->m_iFn, pX->m_iMeas, pX->m_iFirst, pX->cType, 0, pX->iCnv, pX->CnvTxt(), pX->Description());
        KeepingArchive = 1;
        LastIndex = pX->lDataIndex;
        break;
        }
      case XM_ArcShowTagInfo :
        {
        CXM_ArcShowTagInfo *pX = XM.ArcShowTagInfo();
        LogNote(ArcName, 0, "Incomplete TagInfo %s", pX->cTag);
        /*
        const int SlotNo = FindChanForTag(pX->cTag);
        if (SlotNo>=0)
        {//tag found, show driver tag info as required...
        ASSERT(pX->iArcOptMask!=0);
        RetCode = 1; //valid driver tag
        CString s;
        if (pX->iArcOptMask & 0x0001)
        {//show in status bar
        Slots[SlotNo]->GetLongArcDesc(s);
        pStatusBar->SetMsg1((char*)(const char*)s); //place temporary message on status bar
        }
        if (pX->iArcOptMask & 0x0002)
        {//show in message window
        if ((pX->iArcOptMask & 0x0001)==0)
        Slots[SlotNo]->GetLongArcDesc(s);
        LogNote(ArcName, 0, (char*)(const char*)s);
        }
        */
        if (pX->iArcOptMask & 0x0004)
          {//show in archive slot dialog
          Options(pX->cTag);
          }
        break;
        }
      default: 
        ASSERT(0);
    }
  XM.Clear();

  if (KeepingArchive)
    {
    if (RetCode)
      {
#if dbgArchiver
      if (dbgTime())
        dbgpln("HT DoneArcCfgSlots:%d", GetTickCount() - StartTime);
#endif
      //Save();  // Save Configuration
#if dbgArchiver
      if (dbgTime())
        dbgpln("HT DoneSave:%d", GetTickCount() - StartTime);
#endif
      XBuildMyDataLists();
      }
    CXM_Long *xbLong=new CXM_Long (LastIndex);
    XM.PackMsg(xbLong);
    }
  return RetCode;
  }

//---------------------------------------------------------------------------

void CArchiveManager::EO_SetModelState(eScdMdlStateActs RqdState, Strng_List &RqdTags)
  {
  switch (RqdState)
    {
    case MSA_Null: 
    case MSA_ZeroFlows: 
    case MSA_Empty: 
    case MSA_PreSet: 
    case MSA_SteadyState: 
    case MSA_PBInit: 
    case MSA_DynStatsRunInit: 
    case MSA_EmptySpillTargets :    
      //for (int i=0; i<m_Inputs.GetCount(); i++)
      //  m_Inputs[i]->ClrPreviousValue();
      break;
    }
  };

//---------------------------------------------------------------------------

flag CArchiveManager::EO_SetTime(CTimeValue TimeRqd, bool Rewind)
  {
  if (Rewind)
    {
    m_TimeAtStart = TimeRqd;
    m_TimeAtStartValid = true;

    m_bRewind    = true;

    dbgpln("CArchiveManager::EO_SetTime %-10s %s", "Rewind", TimeRqd.Format(TD_Time|TD_Days));

    // must clear the tables data
    const int NT = m_Tables.GetSize();
    for (int t=0; t<NT; t++)
      {
      CArchiveDBTable &Tb=*m_Tables[t];
      Tb.m_bRewindReqd = true;
      if (m_bFirstClear || Rewind && Tb.m_bClearOnRewind)
        {
        switch (Tb.m_iFormat)
          {
          case ADBTbFmt_DB:
            Tb.DeleteDBRecords();
            break;
          case ADBTbFmt_CSV:
          case ADBTbFmt_TXT:
            Tb.ReOpenTxtTable(true);
            break;
          }
        }
      }
    m_bFirstClear = false;
    }

  return true;
  };

//---------------------------------------------------------------------------

flag CArchiveManager::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_Start(CXM_TimeControl &CB)
  {

  //  if (!bRecordHistDone)
  //    RecordSlots(); //ensure the historian knows what to record
  flag OK = 1;
  //  for (int i=0; i<Slots.GetSize(); i++)
  //    Slots[i]->bReqd = Slots[i]->bHasSet;
  //  for (i=0; i<Drivers.InUse(); i++)
  //    OK = OK && Drivers.At(i)->Start();

  //for (int i=0; i<m_Inputs.GetCount(); i++)
  //  m_Inputs[i]->PutValid(false);

  //int NI=m_Fields.GetSize();
  //for (int i=0; i<NI; i++)
  //  m_Fields[i]->Execute(CB, *this);

  //int NT=m_Tables.GetSize();
  //for (int t=0; t<NT; t++)
  //  {
  //  m_Tables[t]->Execute(CB, *this);
  //  }

  if (!SendStart())
    OK=0;

  return OK;
  }

//---------------------------------------------------------------------------

void CArchiveManager::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  CXM_ReadIndexedData * pMsg = (CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll = pMsg->ReadAll;
  long DataIndex = pMsg->Start ? 0 : pMsg->LastIndex-m_lInputOffset + 1;
  XM.Clear();
  for (  ; DataIndex<m_Inputs.GetSize(); DataIndex++)
    {
    CArchiveInput * pInput = m_Inputs[DataIndex];
    CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex+m_lInputOffset, C2T(pInput->m_sScdMTag), TABOpt_AllInfoOnce, XIO_In);//pInput->m_iAction);
    if (!XM.PackMsg(DRqst))
      {
      delete DRqst;
      break;
      }
    }
  }

//---------------------------------------------------------------------------

void CArchiveManager::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  while (XM.MsgAvail())
    {
    CXM_DataRequest * Msg = XM.DataRequest();
    long ItemIndex=FindFieldIndex(Msg->Tag);
    if (ItemIndex>=0)
      {//tag is in item list...
      CXM_DataAvailable *DA=new CXM_DataAvailable (ItemIndex, Msg->Tag, Msg->Action);
      XMRet.PackMsg(DA);
      }
    }
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_ReadSubsData(CXMsgLst &XM)
  {
  flag DataRead = 0;
  CXM_ReadIndexedData *pMsg = XM.ReadIndexedData();
  long DataIndex = pMsg->Start ? 0 : pMsg->LastIndex+1;
  if (pMsg->ReadAll)
    {//return all points...
    XM.Clear();

    while (DataIndex < m_Fields.GetSize())
      {
      CArchiveDBField & It = GetField(DataIndex);
      if (!It.m_bSystem)
        {
        CXM_ObjectData *Data=new CXM_ObjectData (DataIndex);
        CPkDataItem * pPItem = Data->List.FirstItem();
        //CPkDataItem * pPrevItem = pPItem;
        if (It.GetValue(Data->List, /*pPItem,*/ true))
          {
          //Data->SetSize();
          if (XM.PackMsg(Data))
            {
            DataRead = 1;
            It.ClearChanged();
            }
          else
            break; //message full
          }
        }
      DataIndex++;
      }
    }
  else
    {//return points that have changed (if any)...
    XM.Clear();
    if (!m_bSubsRead_Busy)
      {
      m_bSubsRead_Busy = 1;
      m_lBusyChanges = NoOfChanges();
      }
    while (1)
      {
      while (m_lBusyChanges)
        {
        long iIt= GetNextChange();
        m_lBusyChanges--;
        if (iIt>=0)
          {
          DataIndex = iIt;
          CXM_ObjectData *Data=new CXM_ObjectData (DataIndex);
          CPkDataItem * pPItem = Data->List.FirstItem();
          //CPkDataItem * pPrevItem = pPItem;
          if (GetCalcValue(iIt, Data->List, /*pPItem,*/ false))
            {
            //Data->SetSize();
            if (GetField(iIt).Changed())
              {
              if (XM.PackMsg(Data))
                {
                DataRead = 1;
                GetField(iIt).ClearChanged();
                }
              else
                {
                delete Data;
                //NOT good, change is not sent !!!
                break; //message full
                }
              }
            }
          }
        }
      m_bSubsRead_Busy = 0;
      break;
      }

    }
  //Free();
  return DataRead;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {

  while (XM.MsgAvail())
    {
    CXM_ObjectData *pX = XM.ObjectData();
    CPkDataItem *pPItem = pX->FirstItem();
    if (pX->Index>=m_lInputOffset)
      {
      CArchiveInput & It=GetInput(pX->Index-m_lInputOffset);
      It.PutValue(*pPItem);
#if dbgDriverMan
      if (dbgPutValues())
        dbgpln("Arc: Put %s  EO_WriteSubsData", (LPCTSTR)It.m_sScdMTag);
#endif
      }
    }

  return true;
  }

//--------------------------------------------------------------------------

DWORD CArchiveManager::EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  DWORD RetCode = 0;
  long ItemIndex=FindFieldIndex(ObjTag.Tag);
  if (ItemIndex>=0)
    {
    RetCode = (RC_RTI_Exists | RC_RTI_ArchiveTag);
    if (IsNumData(FieldType(ItemIndex)))
      RetCode |= RC_RTI_NumData;
    else if (IsStrng(FieldType(ItemIndex)))
      RetCode |= RC_RTI_StrData;
    ObjData.List.Clear();
    if ((ObjTag.Options & TABOpt_Exists)==0)
      {
      CPkDataItem * pPItem = ObjData.List.FirstItem();
      //CPkDataItem * pPrevItem = pPItem;
      GetCalcValue(ItemIndex, ObjData.List, /*pPItem,*/ true);
      //ObjData.SetSize();
      }
    }

  return RetCode;
  }

//--------------------------------------------------------------------------

int CArchiveManager::EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  return TOData_NotFound;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  SendExecute();
  return false;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_StepDone(CXM_TimeControl &CB, flag StepExecuted)
  {
  if (StepExecuted)
    SendStepDone();
  return false;
  };

//---------------------------------------------------------------------------

flag CArchiveManager::EO_Stop(CXM_TimeControl &CB)
  {
  SendStop();

  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_TagsNotAvail(CXMsgLst &XM)
  {
  while (XM.MsgAvail())
    {
    CXM_TagNotAvail * pX = XM.TagNotAvail();
    LogError(ArcName, 0, "Tag not found %s", pX->Tag);
    }
  XM.Clear();
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_BeginSave(FilingControlBlock &FCB)
  {
  m_bDoneLocalLoadSave = 0;
  if (m_bOn)
    {
    //    for (int i=0; i<Drivers.InUse(); i++)
    //      {
    //      Drivers.At(i)->bDoneSave = 0;
    //      Drivers.At(i)->BeginSave(FCB);
    //      }
    ////    if (FCB.SaveAs())
    ////      FCB.CopyFile(sName());
    }
  m_iLoadSaveCnt = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM)
  {
  //  while (1)
  //    {
  //    while (m_iLoadSaveCnt<Drivers.InUse() && !Drivers.At(m_iLoadSaveCnt)->HasLoadSave())
  //      m_iLoadSaveCnt++;
  //    if (m_iLoadSaveCnt>=Drivers.InUse())
  //      {//finished save data for each driver
  //      if (m_bDoneLocalLoadSave)
  //        return false;
  //      Tag = "ArchiveData";
  //      CXM_ObjectData ObjData(0);//indexed to the first Data Item which is all thats required
  //      ObjData.SetSize();
  //      XM.PackMsg(ObjData);
  //      return true;
  //      }
  //    if (Drivers.At(m_iLoadSaveCnt)->SaveDefinedData(FCB, Tag, XM))
  //      return true;
  //    m_iLoadSaveCnt++;
  //    }
  return false;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_SaveOtherData(FilingControlBlock &FCB)
  {
  //  DWORD NWrite;
  //  char Buff[64]; // CNM Spares
  //  if (m_iLoadSaveCnt<Drivers.InUse())
  //    {
  //    pCDriver pDrv = Drivers.At(m_iLoadSaveCnt);
  //    if (!pDrv->bDoneSave)
  //      {
  //      byte Len=pDrv->sTag.Len();
  //      ASSERT_ALWAYS(Len<' ', "Bad Driver Tag Len", __FILE__, __LINE__);
  //      //ASSERT(pDrv->sTag.Len()<9);
  //      strcpy(Buff, pDrv->sTag());
  //      Buff[Len]=0;
  //      FCB.WriteFile(&Len, sizeof(Len), &NWrite);
  //      FCB.WriteFile((LPVOID)Buff, Len, &NWrite);
  //      pDrv->bDoneSave = 1;
  //      }
  //    return pDrv->SaveOtherData(FCB);
  //    }
  //  if (m_bDoneLocalLoadSave)
  //    return false;
  //  m_bDoneLocalLoadSave = 1;
  //  SaveTags(FCB, false);
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_BeginLoad(FilingControlBlock &FCB)
  {
  m_bDoneLocalLoadSave = 0;
  if (m_bOn)
    {
    //    for (int i=0; i<Drivers.InUse(); i++)
    //      Drivers.At(i)->BeginLoad(FCB);
    }
  m_iLoadSaveCnt = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM)
  {
  //  for (long i=0; i<Drivers.InUse(); i++)
  //    if (Drivers.At(i)->LoadDefinedData(FCB, XM))
  //      return true;
  //finished loading each driver
  if (m_bDoneLocalLoadSave)
    return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_LoadOtherData(FilingControlBlock &FCB)
  {
  if (FCB.Eof())
    return false;
  //  DWORD NRead;
  //  char Buff[512];
  ////      Buff[Len]=0;
  ////      FCB.WriteFile(&Len, sizeof(Len), &NWrite);
  ////      FCB.WriteFile((LPVOID)Buff, Len, &NWrite);
  //  // Read One Byte if > ' ' then old style 9 chars
  //  FCB.ReadFile((LPVOID)Buff, 1, &NRead);
  //  if (Buff[0]>' ')
  //    FCB.ReadFile((LPVOID)&Buff[1], 8, &NRead);
  //  else
  //    {
  //    //Buff[0] is Len Byte
  //    byte Len=Buff[0];
  //    FCB.ReadFile((LPVOID)Buff, Len, &NRead);
  //    Buff[Len]=0;
  //    }
  //  pCDriver pDrv = Drivers.Find(Buff);
  //  if (pDrv)
  //    {
  //    if (pDrv->LoadOtherData(FCB))
  //      return true;
  //    }
  //  else if (strncmp("TAGS....", Buff, 8)==0)
  //    {
  //    m_bDoneLocalLoadSave = 1;
  //    if (!FCB.Eof())
  //      LoadTags(FCB);
  //    return true;
  //    }
  return false;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_EndConnect(FilingControlBlock &FCB)
  {
  return true;
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_EndLoad(FilingControlBlock &FCB)
  {
  //  for (long i=0; i<Slots.GetSize(); i++)
  //    {
  //    CDrvSlot& S = *(Slots[i]);
  //    if (S.bLocal && S.bUseInitVal && (!bIgnoresOn || !S.bIgnoreWrites))
  //      {
  //      PkDataUnion Data(S.dInitVal);
  //      S.PutTagValue(Data);
  //      }
  //    }
  return true;
  }

//---------------------------------------------------------------------------

int CArchiveManager::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {//will I allow the tag to be changed...
  /*int SlotNo = FindChanForTag(pOldTag);
  if (SlotNo>=0)
  return EOCT_NOTALLOWED;*/
  if (gs_Exec.Busy())
    {
    //do not allow user to change a solver tag that is refered to by the driver
    //connect column while syscad is running
    const int len = strlen(pOldTag);
    const int j = m_Inputs.GetSize();
    for (int i=0; i<j; i++)
      {
      if (_strnicmp(pOldTag, m_Inputs[i]->m_sScdMTag, len)==0)
        return EOCT_ARCNOTALLOWED;
      }
    }
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CArchiveManager::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {//a tag has been changed, ...
  const int len = strlen(pOldTag);
  const int j = m_Inputs.GetSize();
  for (int i=0; i<j; i++)
    {
    CArchiveInput * pInput = m_Inputs[i];
    if (_strnicmp(pOldTag, pInput->m_sScdMTag, len)==0)
      {
      ASSERT(!gs_Exec.Busy());
      LogWarning(ArcName, 0, "Changing tag '%s' affects archiver", pOldTag);
      }
    }
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CArchiveManager::EO_QueryDeleteTag(pchar pDelTag)
  {//will I allow the tag to be deleted...
  /*int SlotNo = FindChanForTag(pOldTag);
  if (SlotNo>=0)
  return EODT_NOTALLOWED;*/
  return EODT_NOTFOUND; 
  }

//---------------------------------------------------------------------------

int CArchiveManager::EO_DeleteTag(pchar pDelTag)
  {//a tag has been deleted, ...
  return EODT_NOTFOUND;
  }

//---------------------------------------------------------------------------

void CArchiveManager::EO_OnAppActivate(BOOL bActive)
  {
  }

//---------------------------------------------------------------------------

flag CArchiveManager::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  if (m_bOn)
    {
    switch (Rqst.RQ_Type)
      {
      case RQ_Files:
        if (Rqst.bFirst)
          {
          m_iLoadSaveCnt = 0;
          Info.sData = m_sCfgFile;
          Info.bForceCopy = true;
          Info.bPreventCopy = false;
          return true;
          }
        else if (m_iLoadSaveCnt==0)
          {
          m_iLoadSaveCnt++;
          Info.sData = m_sDataFile;
          Info.bForceCopy = false;
          Info.bPreventCopy = false;
          return true;
          }

        break;
      case RQ_Tags: //return info on all tags based on request criteria...
      case RQ_TagsConns: //return info on all tags based on request criteria...
        //if (Info.Count()<m_Items.GetSize() && (Rqst.pGroup==NULL || _stricmp(Rqst.pGroup, ArcName)==0))
        //  {
        //  Info.sData = m_Items[Info.Count()]->m_sTag();
        //  return true;
        //  }
        break;
      case RQ_TagCount:
        //if (Rqst.bFirst)
        //  {
        //  Info.dwData = m_Items.GetSize();
        //  Info.bDWDataValid = 1;
        //  return true;
        //  }
        break;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void CArchiveManager::SaveTags(FilingControlBlock &FCB, BOOL SaveAll)
  {
  DWORD NWrite;
  char Buff[9];
  long n;
  double d;
  strcpy(Buff, "TAGS....");
  FCB.WriteFile((LPVOID)Buff, 9, &NWrite);
  for (long i=0; i<m_Fields.GetSize(); i++)
    {
    CArchiveDBField & C = *m_Fields[i];
    long len = C.m_sScdMTag.GetLength();
    n=C.m_nSamples;
    d=C.m_dCalculatedValue;
    FCB.WriteFile((LPVOID)&len, sizeof(long), &NWrite);
    FCB.WriteFile((LPVOID)(LPCTSTR)C.m_sScdMTag, len+1, &NWrite);
    FCB.WriteFile((LPVOID)&n, sizeof(n), &NWrite);
    FCB.WriteFile((LPVOID)&d, sizeof(d), &NWrite);
    }
  }

//---------------------------------------------------------------------------

void CArchiveManager::LoadTags(FilingControlBlock &FCB)
  {
  DWORD NRead;
  char Buff[512];
  long len;
  long n;
  double d;
  while (!FCB.Eof())
    {
    FCB.ReadFile((LPVOID)&len, sizeof(long), &NRead);
    FCB.ReadFile((LPVOID)Buff, len+1, &NRead);
    FCB.ReadFile((LPVOID)&n, sizeof(n), &NRead);
    FCB.ReadFile((LPVOID)&d, sizeof(d), &NRead);
    int ci = FindFieldIndex(Buff);
    if (ci>=0)
      {//tag found...
      m_Fields[ci]->m_nSamples =n;
      if (Valid(d))
        m_Fields[ci]->m_dCalculatedValue = d;
      }
    }
  }

//---------------------------------------------------------------------------

void CArchiveManager::Options(char* pGotoTag/*=NULL*/)
  {
  if (pArcSheet==NULL)
    {
    pArcSheet = new CArchiveSheet(this, "Archive", AfxGetMainWnd(), pGotoTag==NULL ? 0 : 1);

    CArcTablesPage * pT=new CArcTablesPage();
    pT->m_pAM=this;
    pArcSheet->AddPage(pT);

    CArcInputPage *pS=new CArcInputPage();
    pS->m_pAM=this;
    pArcSheet->AddPage(pS);

    //CPropertyPage* Pages[16];
    //for (int i=0; i<Drivers.InUse(); i++)
    //  {
    //  pCDriver pDrv = Drivers.At(i);
    //  int Cnt = pDrv->AddPages(Pages);
    //  for (int j=0; j<Cnt; j++)
    //    pArcSheet->AddPage(Pages[j]);
    //  }

    if (pArcSheet->GetPageCount()>0)
      {
      if (!pArcSheet->Create(AfxGetMainWnd(),  WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE, WS_EX_DLGMODALFRAME))
        {
        TRACE("Failed to create Archive PropertySheet\n");
        delete pArcSheet;
        pArcSheet=NULL;
        }
      }
    else
      {
      delete pArcSheet;
      pArcSheet=NULL;
      }
    }
  if (pArcSheet && pGotoTag)
    {
    pArcSheet->DisplayTag(pGotoTag);
    }
  }

//---------------------------------------------------------------------------

void CArchiveManager::CloseOptions()
  {
  if (pArcSheet)
    {
    pArcSheet->PressButton(PSBTN_CANCEL);
    delete pArcSheet;
    }
  pArcSheet=NULL;
  }

//---------------------------------------------------------------------------

void CArchiveManager::AddFieldDlg(TagInfoBlk &IB)
  {
  if (!m_bOn)
    LogNote(ArcName,0,"Archive not enabled");
  else if (XBusy())
    LogNote(ArcName,0,"SysCAD must be stopped");
  else
    {
    CAddToArchive AA(IB, this, AfxGetMainWnd());
    AA.DoModal();
    }
  };

//===========================================================================

void CArchiveManager::LogAMessage(CMsgLogItem & Msg)
  {
  //dbgpln("CArchiveManager::LogAMessage %08x %-20s %s", Msg.m_dwFlags, Msg.Source, Msg.Description);
  if (m_pMessageLog)
    m_pMessageLog->AddMsgRecord(gs_Exec.TimeCB(), Msg);
  };

//===========================================================================

void CArchiveManager::LogAnEvent(LPCTSTR Tag, LPCTSTR Msg)
  {
  // WHAT ABOUT A LOCK/SEMA
  //dbgpln("CArchiveManager::LogAnEvent  %-20s %s", Tag, Msg);
  if (m_pEventLog)
    m_pEventLog->AddEventRecord(gs_Exec.TimeCB(), Tag, Msg);
  };

//===========================================================================
