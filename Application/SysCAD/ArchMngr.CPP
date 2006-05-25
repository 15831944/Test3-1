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

//#include "optoff.h"

#define dbgArchiver       1

#if dbgArchiver
static CDbgMngr dbgArchiverWrites("ArchiveManager", "Writes");
static CDbgMngr dbgTime("ArchiveManager", "Time");
static CDbgMngr dbgConfig("ArchiveManager", "Config");
static CDbgMngr dbgReadCfg("ArchiveManager", "ReadCfg");
static CDbgMngr dbgDBState("ArchiveManager", "DbState");
#endif

const LPTSTR ArcName = "Archive";

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
      m_lInputIndex=AM.AddInput(m_sTag);//=AM.FindItemIndex(m_sTag); 
    }
  else
    m_lFieldIndex=-1; 

#if dbgArchiver
  if (dbgReadCfg())
    dbgpln("SetIndex:Inp/Fld:[%4i/%4i] %s", m_lInputIndex, m_lFieldIndex, (LPCTSTR)m_sTag);
#endif
  };

double CArcAddress::GetValue(CArchiveManager & AM)
  {
  if (m_lInputIndex>=0)
    return AM.GetInput(m_lInputIndex).GetValue();
  if (m_lFieldIndex>=0)
    return AM.GetField(m_lFieldIndex).GetValue();
  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================

flag CArchiveInput::PutValue(CPkDataItem &PItem)
  {
  m_dValue=PItem.Value()->GetDouble();
  return true;
  };

//===========================================================================
//
//
//
//===========================================================================

CArchiveDBField::CArchiveDBField()
  {
  m_bWhat=ADBFn_Null;
  //m_lInputIndex=-1;
  m_lTableIndex=-1;
  m_bReset=true;
  m_dValue=0.0;
  m_dbType=ADOX::adDouble;
  m_dbAttr=ADOX::adColNullable;//ADOX::ColumnAttributesEnum(0);
  };

//---------------------------------------------------------------------------

CArchiveDBField::~CArchiveDBField()
  {
  };

//---------------------------------------------------------------------------

flag CArchiveDBField::GetValue(CPkDataList &List, /*CPkDataItem * &pPItem,*/ flag Complete)
  {
  flag GotIt = False;
  PkDataUnion PData;

  PData.SetTypeDouble(tt_Double, m_dValue);  

  if (Complete)
    {
    //supply ranges in SI units !!!
    DataUnion uMin;
    DataUnion uMax;
    uMin.Set(Cnvs[(m_iCnv)]->Normal(0.0, m_sCnvTxt()));
    uMax.Set(Cnvs[(m_iCnv)]->Normal(1.0, m_sCnvTxt()));
    List.SetDataValueAll(/*pPItem,*/ (char*)(LPCTSTR)m_sScdMTag, "", PData, isArchiveTag,
      uMin, uMax, m_iCnv, m_sCnvTxt(), 
      ""/*sCnvFam()*/, NULL, False, (LPTSTR)(LPCTSTR)m_sDescription);
    }
  else
    List.SetDataValue(/*pPItem,*/ NULL, PData);
  return True;
  }

//---------------------------------------------------------------------------

flag CArchiveDBField::Execute(CXM_TimeControl &CB, CArchiveManager & ArcMan)
  {
  double CurVal=m_Src.GetValue(ArcMan);//0;//ArcMan.Slots[m_lSlotIndex]->GetRawValueDble();

  bool WasReset=m_bReset;
  if (m_bReset)
    {
    m_bReset=false;
    switch (m_bWhat)
      {
      case ADBFn_Instant:
      case ADBFn_Minimum:
      case ADBFn_Maximum:
      case ADBFn_PeriodAvg:
      case ADBFn_RunningAvg:
        m_dValue=CurVal;
        break;
      case ADBFn_Count:
        m_dValue=0;
        break;
      }
    m_nSamples=0;
    }

  double PrevVal=m_dValue;
  switch (m_bWhat)
    {
    case ADBFn_Instant:
      m_dValue=CurVal;
      break;
    case ADBFn_Minimum:
      m_dValue=Min(CurVal, m_dValue);
      break;
    case ADBFn_Maximum:
      m_dValue=Max(CurVal, m_dValue);
      break;
    case ADBFn_PeriodAvg:
      m_dValue=(CurVal+m_nSamples*m_dValue)/(m_nSamples+1);
      m_nSamples++;
      break;
    case ADBFn_RunningAvg:
      m_dValue=CurVal;
      break;
    case ADBFn_Count:
      m_dValue+=1;
      break;
    };
  m_bChanged=WasReset||(fabs(m_dValue-PrevVal)>1.0e-3);
  return true;
  };

//===========================================================================

CArchiveTrigger::CArchiveTrigger()
  {
  m_bInitReqd=true;
  m_bRise=false;
  m_bFall=false;
  m_bAny=false;
  }

//---------------------------------------------------------------------------

CArchiveTrigger::~CArchiveTrigger()
  {
  };

//---------------------------------------------------------------------------

bool CArchiveTrigger::Parse(LPCTSTR Cfg)
  {
  if (Cfg==NULL || strlen(Cfg)==0)
    return false;

  char Buff[4096];
  CSVColArray c;
  int Quote;
  strncpy(Buff, Cfg, sizeof(Buff));
  int nFlds = ParseCSVTokens(Buff, c, Quote);
  if (Quote || nFlds<1)
    goto Bad;
  else
    {
    m_Src.SetTag(c[0]);
    m_dPrevAny=0.0;
    m_dPrevEdge=0.0;
    for (int i=1; c[i] && strlen(c[i])>0; i++)
      {
      char *p;
      while ((p=strchr(c[i],' '))!=NULL)
        memmove(p, p+1, strlen(p+1));
      if (_strnicmp(c[i], "delta=", 6)==0)
        {
        m_bAny=true;
        m_dDeltaAny=fabs(SafeAtoF(c[1]+6,1));
        }
      else if (_strnicmp(c[i], "rise=", 5)==0)
        {
        m_bRise=true;
        m_dDeltaRise=fabs(SafeAtoF(c[1]+5,1));
        }
      else if (_strnicmp(c[i], "fall=", 5)==0)
        {
        m_bFall=true;
        m_dDeltaFall=fabs(SafeAtoF(c[1]+5,1));
        }
      else if (_stricmp(c[i], "delta")==0)
        {
        m_bAny=true;
        m_dDeltaAny=1;
        }
      else if (_stricmp(c[i], "rise")==0)
        {
        m_bRise=true;
        m_dDeltaRise=1;
        }
      else if (_stricmp(c[i], "fall")==0)
        {
        m_bFall=true;
        m_dDeltaFall=1;
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

bool CArchiveTrigger::TestTrigger(CArchiveManager  & ArcMan)
  {
  double CurValue=m_Src.GetValue(ArcMan);

  bool Triggered=false;
  if (m_bInitReqd)
    {
    m_dPrevAny=CurValue;
    m_dPrevEdge=CurValue;
    m_bInitReqd=false;
    }      
  if (m_bRise && (CurValue-m_dPrevEdge)>m_dDeltaRise)
    Triggered=true;
  else if (m_bFall && (CurValue-m_dPrevEdge)<-m_dDeltaFall)
    Triggered=true;
  else if (m_bAny && fabs(CurValue-m_dPrevAny)>m_dDeltaAny)
    {
    m_dPrevAny=CurValue;
    Triggered=true;
    }
  m_dPrevEdge=CurValue;

  return Triggered;
  }

//===========================================================================

CArchiveDBTable::CArchiveDBTable(CArchiveManager *pADB) : m_rAM(*pADB)
  {
  m_bInitReqd=true;
  m_dNextSaveTime=CTimeValue(0.0);
  m_bPeriodOn=false;
  //m_pDef=NULL;
  //m_pRst=NULL;
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
  m_lPeriod=Max(0L,Period);
  m_lOffset=Range(0L, Offset, SecondsInDay);
  m_bPeriodOn=m_lPeriod>0;
  m_bInitReqd=true;
  if (m_bPeriodOn && ((SecondsInDay % m_lPeriod)!=0))
    {
    LogWarning(ArcName, 0, "Period for %s not Modulo 24 Hrs", (LPCTSTR)m_sName);
    }
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::AddTrigger(LPCTSTR Cfg)
  {
  CArchiveTrigger * pTr=new CArchiveTrigger;
  if (!pTr->Parse(Cfg))
    {
    delete pTr;
    return false;
    }
  else
    m_Triggers.Add(pTr);
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::Execute(CXM_TimeControl &CB, CArchiveManager & ArcMan)
  {
  bool DoSavePer=false;
  bool DoSaveTrg=false;

  CTimeValue Period((double)m_lPeriod);
  CTimeValue Offset((double)m_lOffset);

  if (m_bInitReqd)
    {
    if (m_bPeriodOn)
      {
      __time64_t tt=(__time64_t)CB.m_TheTime.Seconds;
      tm* t=_localtime64(&tt);

      CTimeValue CurTimeInDay((double)(t->tm_sec+t->tm_min*SecondsInMinute+t->tm_hour*SecondsInHour));
      CTimeValue SaveTimeInDay(Offset);

      //simple - could be more efficient  
      while (SaveTimeInDay-Period>CurTimeInDay)
        SaveTimeInDay-=Period;
      while (SaveTimeInDay<CurTimeInDay)
        SaveTimeInDay+=Period;

      m_dNextSaveTime=CB.m_TheTime-CurTimeInDay+SaveTimeInDay;
      }

    m_bInitReqd=false;
    }

  if (m_bPeriodOn && (CB.m_TheTime>=m_dNextSaveTime))
    {
    DoSavePer=true;
    m_dNextSaveTime+=Period;
    }

  for (int tr=0; tr<m_Triggers.GetSize(); tr++)
    DoSaveTrg=m_Triggers[tr]->TestTrigger(ArcMan);

  if (DoSavePer || DoSaveTrg)
    {
#if dbgArchiver
    if (dbgArchiverWrites())
      LogNote(ArcName,0,"%s %s %s", CB.m_TheTime.Format(TD_Time), DoSavePer ? "Period ":"Trigger",(LPCTSTR)m_sName);
#endif

    if (ArcMan.m_bDBOK)
      AddDBRecord(CB);
    else
      LogWarning(ArcName,0,"Record not written : %s %s %s", CB.m_TheTime.Format(TD_Time), DoSavePer ? "Period ":"Trigger",(LPCTSTR)m_sName);

    for (long fd=0; fd<m_Fields.GetSize(); fd++)
      m_Fields[fd]->m_bReset=true;
    };

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBTable()
  {
  ADOX::_TablePtr pTbl;
  try
    {
    //    m_Def
    try
      {
      pTbl=m_rAM.m_pCat->Tables->GetItem((LPCTSTR)m_sName);
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

  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBFields(ADOX::_TablePtr & pTbl)
  {
  //_asm int 3; // MUST FIX dbAutoIncrField
  CreateDBField(pTbl, true, "RecNo", ADOX::adInteger, ADOX::ColumnAttributesEnum (0), true/*dbAutoIncrField*/);
  CreateDBField(pTbl, true, "Time", ADOX::adDate, ADOX::ColumnAttributesEnum (0));

  for (int f=0; f<m_Fields.GetSize(); f++)  
    {
    CArchiveDBField &It=*m_Fields[f];
    CreateDBField(pTbl, false, It.m_sFldTag, It.m_dbType, It.m_dbAttr);
    }

  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CreateDBField(ADOX::_TablePtr & pTbl, bool SysFld, LPCTSTR FldName, ADOX::DataTypeEnum FldType, ADOX::ColumnAttributesEnum FldAttr, bool MakeAutoInc)
  {

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
        pCol->DefinedSize=(FldType==ADOX::adInteger)?sizeof(long):0;//Size;
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
  return true;
  };

//---------------------------------------------------------------------------

bool CArchiveDBTable::AddDBRecord(CXM_TimeControl &CB)
  {
  if (m_pRst==NULL)
    return false;

  try
    {
    __time64_t tt=(__time64_t)CB.m_TheTime.Seconds;
    tm* t=_localtime64(&tt);

    COleDateTime DT;
    DT.SetDateTime(t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    m_pRst->AddNew();

    COleVariant dv(DT);
    m_pRst->Fields->GetItem("Time")->Value=dv;

    for (long fd=0; fd<m_Fields.GetSize(); fd++)
      {
      COleVariant v((double)fd);
      m_pRst->Fields->GetItem((LPCTSTR)m_Fields[fd]->m_sFldTag)->Value=m_Fields[fd]->m_dValue;
      }

    m_pRst->Update();
    }
  catch (_com_error & e)
    {
    m_rAM.DisplayException(e, LF_Exclamation, "Adding Record ", (LPCTSTR)m_sName);
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CArchiveDBTable::CloseDBTable()
  {
  try
    {
    if (m_pRst)
      m_pRst->Close();
    m_pRst.Release();

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
  return true;
  }

//===========================================================================

CArchiveManager::CArchiveManager() :
  m_evDone(FALSE)
    {
    //m_pDB=NULL;
    m_lChgSrchStart=0;
    m_bIsOpen=false;
    m_bDBOK=false;
    m_bRegistered=false;
    m_bOpenDBOnRun=false;

    //m_bUseArchive=false;
    m_bDBExclusive=false;
    m_bDBReadOnly=false;
    m_iLoadSaveCnt=0;

    m_lInputOffset=0;
    m_lBusyChanges=0;
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
    long n=m_Tables.GetSize();
    for (long i=0; i<n; i++)
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
    m_bIsOpen=false;
    m_bDBOK=false;

    m_bDBExclusive=false;
    m_bDBReadOnly=false;
    m_iLoadSaveCnt=0;
    }

  // --------------------------------------------------------------------------

  int CArchiveManager::Configure(const char * CfgFile, bool OpenDBOnRun)
    {
    m_sCfgFile= CfgFile;
    m_bOpenDBOnRun=OpenDBOnRun;

    return 0;
    }

  //---------------------------------------------------------------------------

  inline LPTSTR castLPTSTR(LPCTSTR X) { return (LPTSTR)X; };

  bool CArchiveManager::ReadCfg()
    {
    Strng DataFn, DataPath;

    Initialise();

    if (!FileExists(castLPTSTR(m_sCfgFile)))
      return false;


    CProfINIFile PF(castLPTSTR(m_sCfgFile));

    m_sDataFile="";
    DataFn = PF.RdStr("Options", "DBFile", "ScdArchive");
    DataFn.FnSearchExpand();
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

    for (int i=0; ; i++)
      {
      CString TbSect, TbName;
      TbSect.Format("Table%i", i+1);
      TbName=PF.RdStr(TbSect, "Name", "");
      if (TbName.GetLength()<=0)
        break;
      CArchiveDBTable * pTb=new CArchiveDBTable(this) ;
      m_Tables.Add(pTb);

      // NBNBNB No Checking
      CTimeValue P,O;
      pTb->m_sName=TbName;
      pTb->m_sCfgSection=TbSect;

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
        AddField(pTb->m_sName, FldOpt);
        }
      };

    return true;
    }

  //---------------------------------------------------------------------------

  bool CArchiveManager::CompleteOpen()
    {
    ASSERT_ALWAYS(!XBusy(), "CompleteOpen - SysCAD Running");

    bool OK=true;

    for (int s=0; s<m_Inputs.GetSize(); s++)
      delete m_Inputs[s];
    m_Inputs.SetSize(0);

    long nt=m_Tables.GetSize();
    for (long t=0; t<nt; t++)
      {
      CArchiveDBTable & Tb=*m_Tables[t];
      long ntt=Tb.m_Triggers.GetSize();
      for (long tt=0; tt<ntt; tt++)
        {
        CArchiveTrigger &Tr=*Tb.m_Triggers[tt];
        Tr.m_Src.SetIndex(*this);
        //)
        //if (!Tr.m_Src.SetIndex(*this))
        //  {
        //  LogError(ArcName,0,"Trigger %s not found for Table %s", (LPCTSTR)Tr.m_Src.m_sTag, (LPCTSTR)Tb.m_sName);
        //  OK=false;
        //  }
        }
      Tb.m_Fields.SetSize(0,64);
      }

    long nf=m_Fields.GetSize();
    for (long f=0; f<nf; f++)
      {
      CArchiveDBField *pFld=m_Fields[f];
      pFld->m_Src.SetIndex(*this);
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
      if (!IsNumData(pFld->m_iType))
        {
        LogError(ArcName,0,"Item data is not numeric %s", (LPCTSTR)pFld->m_Src.m_sTag);
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

    m_nFieldsOpn=0;
    m_nTablesOpn=0;

    OpenOK = CompleteOpen();
    OpenOK = OpenOK && OpenDB();
    OpenOK = OpenOK && CreateDBTables();
    OpenOK = OpenOK && CreateDBRecordsets();

    if (OpenOK)
      LogNote(ArcName, 0, "%i Fields in %i Tables", m_nFieldsOpn, m_nTablesOpn);
    else
      LogNote(ArcName, 0, "Database not opened");

    return OpenOK;
    }
  //---------------------------------------------------------------------------

  bool CArchiveManager::Start()
    {
    if (!m_bIsOpen)
      return true;

    if (m_bIsOpen && m_bOpenDBOnRun)
      m_bDBOK = OpenTheDB();

    return m_bDBOK;
    }

  //---------------------------------------------------------------------------

  bool CArchiveManager::Stop()
    {
    if (!m_bIsOpen)
      return true;

    if (m_bIsOpen && m_bOpenDBOnRun)
      CloseDB();

    return true;
    }

  // --------------------------------------------------------------------------

  int CArchiveManager::Open()
    {
    CDlgBusy::Open("\nOpening Archive Manager");
    int Err = 0;
    dbgpln("DRV CArchiveManager::Open()");
    Initialise();
    m_bIsOpen = false;
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

      m_bIsOpen=OK;

      if (m_bIsOpen && !m_bOpenDBOnRun)
        m_bDBOK = OpenTheDB();

      if (!m_bIsOpen)
        {
        CloseDB();
        Initialise();
        }

      if (!m_bIsOpen)
        LogError(ArcName, 0, "'%s' not opened", (LPCTSTR)m_sCfgFile);
#if dbgArchiverMan
      if (dbgDumpConfig())
        ConfigurationDump();
#endif
      }

    if (m_bIsOpen)
      {
      //?EO_Register(pExecName_ArcMngr, EOWrite_Thread | EORead_Thread | EOExec_Thread,/*Pri*/THREAD_PRIORITY_ABOVE_NORMAL,/*Stack*/50000, /*HashSize*/ 101);

      //setup slot map...
      //    SlotMap.RemoveAll();
      //    UINT PrimeNo = FindNextPrimeNumber((UINT)(UsedSlots*1.18));
      //    SlotMap.InitHashTable(PrimeNo);
      //    ConSlots.SetSize(UsedConns, 256);
      //    for (long i=0; i<Slots.GetSize(); i++)
      //      SlotMap.SetAt(Slots[i]->sTag(), i);
      //
      //    //setup connection array
      //    int SlotConnCnt = 0;
      //    UsedConns = 0;
      //    int UsedSubs = 0;
      //    for (i=0; i<Slots.GetSize(); i++)
      //      {
      //      CDrvSlot& S = *(Slots[i]);
      //      CDrvSubsConn *pCon = S.pConSlot;
      //      if (pCon)
      //        SlotConnCnt++;
      //      while (pCon)
      //        {
      //        CDrvSubsConn *DelCon = NULL;
      //        int SlotNo = FindChanForTag(pCon->sTag());
      //        if (SlotNo>=0 && Slots[SlotNo]->pDrv==S.pDrv && pCon->m_bGet)
      //          {
      //          LogError(ArcName, 0, "Connection unable to get Archive tag %s for slot %s", pCon->sTag(), Slots[i]->sTag());
      //          DelCon = pCon;
      //          }
      //        else
      //          {
      //          if (SlotNo>=0 && Slots[SlotNo]->pDrv==S.pDrv)
      //            {
      //            pCon->bInternal = 1;
      //            pCon->pSlot = Slots[SlotNo];
      //            pCon->Typ = Slots[SlotNo]->iTyp;
      //            Slots[SlotNo]->bHasFarCon = 1;
      //            }
      //          if (!pCon->m_bGet)
      //            S.bHasSet = 1;
      //          pCon->pParentSlot = Slots[i];
      //          if (!pCon->bInternal)
      //            {
      //            int SubsIndex = FindInput(pCon->sTag());
      //            if (SubsIndex>=0)
      //              Inputs[SubsIndex]->AddConn(pCon);
      //            else
      //              {
      //              Inputs.Add(new CInput(pCon));
      //              SubsIndex = UsedSubs;
      //              UsedSubs++;
      //              }
      //            pCon->iSubsIndex = SubsIndex;
      //            }
      //          ConSlots.SetAt(UsedConns, pCon);
      //          UsedConns++;
      //          }
      //        pCon = pCon->pNxt;
      //        if (DelCon)
      //          {//invalid connection, delete it...
      //          if (DelCon==S.pConSlot)
      //            S.pConSlot = pCon;
      //          else
      //            {
      //            CDrvSubsConn *pTempCon = S.pConSlot;
      //            while (pTempCon->pNxt!=DelCon)
      //              pTempCon = pTempCon->pNxt;
      //            pTempCon->pNxt = pCon;
      //            }
      //          delete DelCon;
      //          }
      //        }
      //      if ((i%20)==0)
      //        CDlgBusy::SetLine(3, "Slot connects:%i of %i", i, UsedSlots);
      //      }
      //    CDlgBusy::SetLine(3, "Slot connects:%i of %i", UsedSlots, UsedSlots);
      //    ConSlots.SetSize(UsedConns, 256);
      //    LogNote(ArcName, 0, "%u slots have %u connections", SlotConnCnt, UsedConns);


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
              {
              LoadTags(FCB);
              //for (long j=0; j<Slots.GetSize(); j++)
              //  {
              //  CDrvSlot& S = *(Slots[j]);
              //  if (S.bUseInitVal && (!bIgnoresOn || !S.bIgnoreWrites))
              //    {
              //    PkDataUnion Data(S.dInitVal);
              //    S.PutTagValue(Data);
              //    }
              //  }
              }
            CloseHandle(H);
            }
          }
        }

      //is it safe to assume the historian EO is opened before the Archive manager EO ???
      //RecordSlots();
      }
    else
      Err = 2;
    XBuildDataLists(); //otherwise if connect column tags are removed, and the Archive is reloaded, SysCAD crashes
    CDlgBusy::Close();

    return Err;
    }

  // --------------------------------------------------------------------------

  flag CArchiveManager::Close(BOOL SaveState/*=true*/)
    {
    if (!m_bIsOpen)
      return true;

    if (m_bIsOpen)
      CDlgBusy::Open("\n\nClosing Archive Manager");

    if (!m_bOpenDBOnRun)
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
    if (m_bIsOpen)
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

  const LPCTSTR InvalidFldNmChars = ".'[]";
  const LPCTSTR InvalidFldNmCharsM = "[]";
  const LPCTSTR InvalidFldNmCharsD = ".'";

  long CArchiveManager::AddField(LPCTSTR Table, LPCTSTR CfgString)
    {
    if (CfgString==NULL || strlen(CfgString)==0)
      return 0;

    CSVColArray f;
    int Quote,nParms;
    CString ADBCfg=CfgString;
    nParms = ParseCSVTokens((LPTSTR)CfgString, f, Quote) - 1;
    if (Quote || f[0]==NULL || nParms<1)
      {
      LogError(ArcName, 0, "Invalid Archive Spec: %s", (LPCTSTR)ADBCfg);
      return -1;
      }

    LPCTSTR Tag=f[0];
    for (int i=1; i<=nParms; i++)
      {
      byte iFn=ADBFn_Null;
      if      (_stricmp(f[i], ADBFuncTags[ADBFn_Instant])==0)      iFn=ADBFn_Instant;
      else if (_stricmp(f[i], ADBFuncTags[ADBFn_Minimum])==0)      iFn=ADBFn_Minimum;
      else if (_stricmp(f[i], ADBFuncTags[ADBFn_Maximum])==0)      iFn=ADBFn_Maximum;
      else if (_stricmp(f[i], ADBFuncTags[ADBFn_PeriodAvg])==0)    iFn=ADBFn_PeriodAvg;
      //else if (_stricmp(f[i], ADBFuncTags[ADBFn_RunningAvg])==0) iFn=ADBFn_RunningAvg
      else if (_stricmp(f[i], ADBFuncTags[ADBFn_Count])==0)        iFn=ADBFn_Count;
      else
        {
        LogError(ArcName, 0, "%s has Invalid Function Spec: %s", Tag, f[i]);
        return -1;
        }

      CXM_ObjectTag ObjTag((LPTSTR)Tag, TABOpt_AllInfoOnce);
      CXM_ObjectData ObjData;
      CXM_Route Route;
      if (XReadTaggedItem(ObjTag, ObjData, Route))
        {
        CPkDataItem & Item =*ObjData.FirstItem();
        if (!IsNumData(Item.Type()))
          LogWarning(ArcName, 0, "Tag '%s' not numeric.", Tag);

        return AddField(Tag, Table, iFn, Item.Type(),Item.CnvIndex(), Item.CnvTxt(),"");
        }
      }

    return -1;
    };

  long CArchiveManager::AddField(LPCTSTR Tag, LPCTSTR Table, byte iFn, byte iType, CCnvIndex iCnv, LPCTSTR CnvTxt, LPCTSTR Description)
    {
    long Index=-1;
    if (iFn!=ADBFn_Null)
      {
      CArchiveDBField *pFld=new CArchiveDBField;
      pFld->m_sTable=Table;
      pFld->m_bWhat=iFn;
      if (iFn==ADBFn_Count)
        {
        pFld->m_iType=tt_Long;
        pFld->m_iCnv=0;
        pFld->m_sCnvTxt="";
        }
      else
        {
        pFld->m_iType=iType;
        pFld->m_iCnv=iCnv;
        pFld->m_sCnvTxt=CnvTxt;
        }
      pFld->m_sDescription=Description;
      pFld->m_Src.m_sTag=Tag;
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
        dbgpln("AddField [%3i]%-10s %-20s %-20s %-30s %-40s %s", 
          Index,
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

  long CArchiveManager::AppendField(LPCTSTR Tag, LPCTSTR Table, byte iFn, byte iType, CCnvIndex iCnv, LPCTSTR CnvTxt, LPCTSTR Description)
    {
    long Index=-1;
    int iTb=FindTable(Table);
    if (iTb>=0)
      {
      Index=AddField(Tag, Table, iFn, iType, iCnv, CnvTxt, Description);

      if (Index>=0)
        {
        // Reopen Table / RecordSet
        CArchiveDBTable & Tb=*m_Tables[iTb];
        bool OK;
        if (m_bOpenDBOnRun)
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

          CProfINIFile PF(castLPTSTR(m_sCfgFile));
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
    ASSERT_ALWAYS(!XBusy(), "AddInput - SysCAD Running");

    CXM_ObjectTag ObjTag((LPTSTR)Tag, TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    CXM_Route Route;
    //      CXMsgLst XM;

    long Index=-1;
    if (XReadTaggedItem(ObjTag, ObjData, Route))
      {
      CPkDataItem & Item =*ObjData.FirstItem();
      CArchiveInput * pSubs=new CArchiveInput;
      pSubs->m_sScdMTag=Tag;
      pSubs->m_iType=Item.Type();
      pSubs->m_iCnv=Item.CnvIndex();
      pSubs->m_sCnvTxt=Item.CnvTxt();

      if (IsNumData(pSubs->m_iType))
        {
        if (IsFloatData(pSubs->m_iType))
          pSubs->m_dValue=Item.Value()->GetDouble();
        else
          pSubs->m_dValue=Item.Value()->GetLong();
        Index=m_Inputs.Add(pSubs);
#if dbgArchiver
        if (dbgConfig())
          dbgpln("AddInput %-20s %-10s ", (LPCTSTR)pSubs->m_sScdMTag, tt_TypeString(pSubs->m_iType));
#endif
        }
      else
        LogWarning(ArcName, 0, "Tag '%s' not numeric.", Tag);
      }
    else
      {
      LogWarning(ArcName, 0, "Tag '%s' not found.", Tag);
      }

    return Index;
    };

  //---------------------------------------------------------------------------

  flag CArchiveManager::Execute(CXM_TimeControl &CB)//, CArchiveManager & ArcMan)
    {
    if (!m_bIsOpen)
      return 0;

    int NI=m_Fields.GetSize();
    for (int i=0; i<NI; i++)
      m_Fields[i]->Execute(CB, *this);

    int NT=m_Tables.GetSize();
    for (int t=0; t<NT; t++)
      {
      m_Tables[t]->Execute(CB, *this);
      }
    return true;
    };

  //---------------------------------------------------------------------------

  long CArchiveManager::NoOfChanges()
    {
    if (!m_bIsOpen)
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
    if (!m_bIsOpen)
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
    if (!FileExists((LPTSTR)(LPCTSTR)m_sDataFile, fd))
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
    if (FileExists((LPTSTR)(LPCTSTR)m_sDataFile, fd))
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
    for (int t=0; t<m_Tables.GetSize(); t++)
      {
      if (!m_Tables[t]->CreateDBTable())
        OK=false;
      }
#if dbgArchiver
    if (dbgDBState())
      dbgpln("CreateDBTables %s",OK?"OK":"BAD");
#endif
    return OK;
    };

  //---------------------------------------------------------------------------

  bool CArchiveManager::CreateDBRecordsets()
    {
    bool OK=true;
    for (int t=0; t<m_Tables.GetSize(); t++)
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
    if (m_pCnn!=NULL)
      {
      try
        {
        for (int t=0; t<m_Tables.GetSize(); t++)
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

    LogError(ArcName, LogOpts, (LPTSTR)(LPCTSTR)message);
    }

  //---------------------------------------------------------------------------

  bool CArchiveManager::SendStart()
    {
    AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_ARCHIVESTART, 0);
    return m_bIsOpen;
    }

  //---------------------------------------------------------------------------

  bool CArchiveManager::SendExec()
    {
    AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_ARCHIVEEXEC, 0);
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
          AddField(pX->Tag(), pX->TableName(), pX->iFn, pX->cType, pX->iCnv, pX->CnvTxt(), pX->Description());
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
      CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex+m_lInputOffset, (LPTSTR)(LPCTSTR)pInput->m_sScdMTag, TABOpt_AllInfoOnce, XIO_In);//pInput->m_iAction);
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
    SendExec();
    return false;
    }

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
    if (m_bIsOpen)
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
    //      ASSERT_ALWAYS(Len<' ', "Bad Driver Tag Len");
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
    if (m_bIsOpen)
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
    if (m_bIsOpen)
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
      d=C.m_dValue;
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
          m_Fields[ci]->m_dValue = d;
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
    if (!m_bIsOpen)
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
