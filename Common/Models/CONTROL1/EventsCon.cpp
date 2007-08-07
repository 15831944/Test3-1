//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "eventscon.h"
//#include "optoff.h"

#if !SKIPIT 

//==========================================================================
//
//
//
//==========================================================================

const byte ReportToMsg = 0x01;
const byte ReportToEventLog = 0x02;

//==========================================================================

EventRowInfo::EventRowInfo()
  {
  m_dTime = 0.0;
  m_dOutputVal = 0.0;
  }

EventRowInfo::EventRowInfo(double Time, double Output, char* OutputTag)
  {
  m_dTime = Time;
  m_dOutputVal = Output;
  m_sOutputTag = OutputTag;
  }

typedef EventRowInfo* pEventRowInfo;

//==========================================================================

EventConInfo::EventConInfo(CEventsCon* Parent)
  {
  pParent = Parent;
  bValid = 0;
  bReloadRqd = 1;
  bWrapArround = 1;
  eLogSetTags = 0;
  sPath = PrjFiles();
  hProcess = NULL;
  dwProcessId = 0;
  RowData = NULL;
  iRowCnt = 0;
  //SetRowCnt(?);
  dStartTime = 0.0;
  dTimePassed = 0.0;
  iCurIndex = 0;
  }

//--------------------------------------------------------------------------

EventConInfo::~EventConInfo()
  {
  SetRowCnt(0);
  }

//--------------------------------------------------------------------------

long EventConInfo::SetRowCnt(long NewSize)
  {
  NewSize = Range(0L, NewSize, 8192L);
  if (NewSize!=iRowCnt)
    {
    EventRowInfo ** NewRowData = NULL;
    if (NewSize<iRowCnt)
      {
      for (int i=NewSize; i<iRowCnt; i++)
        delete RowData[i];
      }
    if (NewSize>0)
      {
      NewRowData = new pEventRowInfo[NewSize];
      for (int i=0; (i<NewSize && i<iRowCnt); i++)
        NewRowData[i] = RowData[i];
      for (i=iRowCnt; i<NewSize; i++)
        {
        NewRowData[i] = new EventRowInfo;
        }
      }
    if (RowData)
      delete []RowData;
    RowData = NewRowData;
    iRowCnt = NewSize;
    }
  return iRowCnt;
  }

//--------------------------------------------------------------------------

flag EventConInfo::DoLoad()
  {
  bValid = 0;
  bReloadRqd = 0;
  Strng Fn(sPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sFile;
  if (FileExists(Fn()))
    {
    bool FileError = false;
    char Buff[1024];
    int ValidRowCnt = 0;
    FILE* f=fopen(Fn(), "rt");
    if (f)
      {
      //estimate number of rows...
      fgets(Buff, sizeof(Buff)-1, f);// Skip Title Line
      while (!feof(f))
        {
        if (fgets(Buff, sizeof(Buff)-1, f))
          ValidRowCnt++;
        }
      fclose(f);
      f = NULL;
      }
    else
      {
      FileError = true;
      sStatus = "Cannot open specified file!";
      }

    if (ValidRowCnt>0)
      {
      FILE* f=fopen(Fn(), "rt");
      if (f)
        {
        SetRowCnt(ValidRowCnt);
        ValidRowCnt = 0;

        int line_number = 0;
        char* c[MaxCSVCols];
        fgets(Buff, sizeof(Buff)-1, f);// Skip Title Line
        line_number++;
        while (!feof(f))
          {
          if (fgets(Buff, sizeof(Buff)-1, f))
            {
            line_number++;
            int Quote;
            int nFlds = ParseCSVTokens(Buff, c, Quote);
            if (Quote)
              {
              LogError(pParent->Tag(), 0, "Line %u: Quotes mismatched", line_number);
              break;
              }
            if (nFlds<3)
              {
              LogError(pParent->Tag(), 0, "Line %u: Expect Time, Tag and Value", line_number);
              break;
              }
            if (c[0] && c[1] && c[2] && strlen(c[0])>0 && strlen(c[1])>0/* && strlen(c[2])>0*/)
              {
              RowData[ValidRowCnt]->m_dTime = atof(c[0]);
              RowData[ValidRowCnt]->m_sOutputTag = c[1];
              RowData[ValidRowCnt]->m_dOutputVal = strlen(c[2])==0 ? 0.0 : (c[2][0]=='*' ? dNAN : atof(c[2]));
              ValidRowCnt++;
              }
            }
          }
        fclose(f);
        if (iRowCnt!=ValidRowCnt)
          SetRowCnt(ValidRowCnt);
        bValid = 1;
        Sort();
        const double tm = RowData[iRowCnt-1]->m_dTime;
        if (tm>86400.0)
          sStatus.Set("OK (%d rows; %.0fhours)", iRowCnt, tm/3600.0);
        else if (tm>3600.0)
          sStatus.Set("OK (%d rows; %.0fminutes)", iRowCnt, tm/60.0);
        else
          sStatus.Set("OK (%d rows; %.0fseconds)", iRowCnt, tm);
        return true;
        }
      else
        {
        FileError = true;
        sStatus = "Cannot open specified file!";
        }
      }
    else
      {
      if (!FileError)
        sStatus = "No rows of Time, Tag and Value have been specified!";
      }
    }
  else
    sStatus = "Specified file not found!";
  return false;
  }

//--------------------------------------------------------------------------

void EventConInfo::Sort()
  {
  if (bValid && iRowCnt>0)
    {
    EventRowInfo * Dummy;
    for (int i=0; i<iRowCnt-1; i++)
      {
      for (int j=0; j<iRowCnt-i-1; j++)
        {
        if (RowData[j+1]->m_dTime<RowData[j]->m_dTime)
          {
          Dummy = RowData[j];
          RowData[j] = RowData[j+1];
          RowData[j+1] = Dummy;
          }
        }
      }
//for (int i=0; i<iRowCnt; i++)
//{
//LogNote(pParent->Tag(), 0, "%2d:%s at %g to %g", i, RowData[i]->m_sOutputTag(), RowData[i]->m_dTime, RowData[i]->m_dOutputVal);
//}
    }
  }

//--------------------------------------------------------------------------

void EventConInfo::LoadAndAdvance(double ICTime, double TimePassed)
  {
  DoLoad();
  dTimePassed = TimePassed;
  dStartTime = ICTime - dTimePassed;
  }

//--------------------------------------------------------------------------

void EventConInfo::Start(double ICTime)
  {
  dStartTime = ICTime;
  dTimePassed = 0.0;
  iCurIndex = 0;
  }

//--------------------------------------------------------------------------

void EventConInfo::ExecIns(double ICTime)
  {
  if (bReloadRqd)
    DoLoad();
  if (bValid)
    {
    dTimePassed = ICTime - dStartTime;
    if (iCurIndex>=iRowCnt)
      return;
//LogNote(pParent->Tag(), 0, "Time:%g   TimePassed:%g", ICTime, dTimePassed);
    while (iCurIndex<iRowCnt && RowData[iCurIndex]->m_dTime<=dTimePassed)
      {
      /*if (1)
        {
        Strng WrkTag,WrkCnv;
        TaggedObject::SplitTagCnv(RowData[iCurIndex]->m_sOutputTag(), WrkTag, WrkCnv);
        CXM_ObjectTag ObjTag(WrkTag(), 0);//TABOpt_AllInfoOnce);
        CXM_ObjectData ObjData;
        CXM_Route Route;
        bool Ok = (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route)!=0);
    
        if (Ok)
          {
          CPkDataItem *pPItem=ObjData.FirstItem();
          if ((pPItem->Flags() & isParm)!=0)
            {
            PkDataUnion DU;
            byte Type=pPItem->Type();
            bool IsNum = IsNumData(Type);
            bool IsStr = IsStrng(Type);
            DU.SetTypeDouble(Type, RowData[iCurIndex]->m_dOutputVal, pPItem->CnvIndex(), WrkCnv());
            CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
        //Ok = (pDoc->XWriteTaggedItem(ObjData, Route)==TOData_OK);
            Ok = (gs_Exec.XWriteTaggedItem(NULL, ObjData, Route)!=TOData_NotFound);//???
            //if (Ok)
            //  gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Trend, sTagOnly(), Str, Cnv.Text());
            }
          else
            Ok = 0;
          }
        }
      */
      int Ret = TryWriteTag(pParent, pParent->FamilyHead(), RowData[iCurIndex]->m_sOutputTag(), RowData[iCurIndex]->m_dOutputVal);
      if (Ret!=FXR_Found)
        {
        Strng S;
        if (Ret & FXR_NotFound) S = "Tag not found";
        else if (Ret & FXR_NotParam) S = "Cannot set this type of tag";
        else if (Ret & FXR_BadValue) S = "Tag is not a number";// or string";
        else if (Ret & FXR_ParamStopped) S = "Tag cannot be set while solving";
        LogWarning(pParent->Tag(), 0, "Failed to set tag '%s' (%s)!", RowData[iCurIndex]->m_sOutputTag(), S());
        }
      else
        {
        if (eLogSetTags)
          {
          char Msg[1024];
          sprintf(Msg, "Set '%s' to %g", RowData[iCurIndex]->m_sOutputTag(), RowData[iCurIndex]->m_dOutputVal);
          if (eLogSetTags & ReportToMsg)
            LogNote(pParent->Tag(), 0, Msg);
          if (eLogSetTags & ReportToEventLog)
            gs_EventLog.LogEvent(pParent->Tag(), Msg);
          }
        }
      iCurIndex++;
      }
    }
  }

//==========================================================================

static double Drw_EventsCon[] =
       {
       DD_Poly, -3.2,-3.2, -3.2,3.2, 3.2,3.2, 3.2,-3.2, -3.2,-3.2, 
       DD_Poly, 1.6,2.4, -1.6,2.4, -1.6,-2.4, 1.6,-2.4,
       DD_Poly, -1.6,0, 0.8,0,
       DD_TagPos, 0, -6.7,
       DD_End
       };

IMPLEMENT_MODELUNIT(CEventsCon, "Events", "", Drw_EventsCon, "Control", "EV", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Events",
                   "Events Control model.")

//--------------------------------------------------------------------------

CEventsCon::CEventsCon(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  ECI(this)
  {
  fHasFiles=1;
  AttachClassInfo(nc_Control, NULL, NULL, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  bOn = 1;
  bAboutToStart = 0;
  }

//--------------------------------------------------------------------------

CEventsCon::~CEventsCon()
  {
  }

//--------------------------------------------------------------------------

void CEventsCon::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmCheckBtn    = 1000;
const word idmPath        = 1001;
const word idmFile        = 1002;
const word idmEditBtn     = 1003;
const word idmBrowseBtn   = 1004;
const word idmReloadBtn   = 1005;
const word idmRestartBtn  = 1006;
const word idmTimePassed  = 1007;
const word idmEventCnt    = 1008;
const word idmLastEvnTime = 1009;
const word idmNextTime    = 1010;
const word idmNextTag     = 1011;
const word idmNextOutput  = 1012;

//--------------------------------------------------------------------------

void CEventsCon::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDBLogSetTags[]={
    {0, "None"},
    {ReportToMsg, "Messages"},
    {ReportToEventLog, "EventLog"},
    {ReportToMsg|ReportToEventLog,   "Messages and EventLog"},
    {0}};

  DDB.BeginStruct(this);
  //DDB.Text("");
  //DDB.CheckBoxBtn("ShowCnv",      "",         DC_, "", &bWithCnvComment, this, isParmStopped, DDBYesNo);
  ////DDB.Text("");
  //DDB.Button("Check_tags"/* and functions"*/, "", DC_, "", idmCheckBtn, this, isParmStopped);
  DDB.Text("");

  Strng Tag;
  DDB.CheckBoxBtn("On",          "",          DC_,     "",      &bOn, this, isParm, DDBYesNo);
  DDB.String("Status",           "",          DC_,     "",      &(ECI.sStatus), this, isResult|noFileAtAll);
  DDB.String("Folder",           "",          DC_,     "",      idmPath, this, isResult);//isParm);
  DDB.String("File",             "",          DC_,     "",      idmFile, this, isParm);
  DDB.Button("Edit",             "",          DC_,     "",      idmEditBtn, this, isParm);
  DDB.Button("Browse",           "",          DC_,     "",      idmBrowseBtn, this, isParm);
  DDB.Byte("LogTagSets",         "",          DC_,      "",     &ECI.eLogSetTags, this, isParm, DDBLogSetTags);
  DDB.Button("Reload_Restart",   "",          DC_,     "",      idmReloadBtn, this, isParm);
  DDB.Button("Restart",          "",          DC_,     "",      idmRestartBtn, this, isParm);
  //DDB.CheckBoxBtn("WrapArround", "",          DC_,     "",      &ECI.bWrapArround, this, isParm, DDBYesNo);
  DDB.Double("TimePassed",       "",          DC_Time, "s",     idmTimePassed, this, isResult);
  DDB.Long  ("EventIndex",       "",          DC_,     "",      &ECI.iCurIndex, this, isResult);
  DDB.Long  ("EventCount",       "",          DC_,     "",      idmEventCnt, this, isResult|noFileAtAll);
  DDB.Double("LastEventTime",    "",          DC_Time, "s",     idmLastEvnTime, this, isResult|noFileAtAll);
  DDB.Text("");
  DDB.Double("NextTime",         "",          DC_Time, "s",     idmNextTime, this, isResult|noFileAtAll|NAN_OK);
  DDB.String("NextTag",          "",          DC_,     "",      idmNextTag, this, isResult|noFileAtAll);
  DDB.Double("NextOutput",       "",          DC_,     "",      idmNextOutput, this, isResult|noFileAtAll|NAN_OK);
  DDB.Text("");
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CEventsCon::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmCheckBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        //AccNodeWnd::RefreshData(TRUE); ???
        //TODO: need to accept current data before processing button push!
        if (PreStartCheck())
          LogNote(Tag(), 0, "No bad external tag references or function errors");
        bAboutToStart = 0;
        }
      DCB.B=0;
      return 1;
    case idmPath:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot ???
        ECI.sPath = DCB.rpC;
        ECI.sPath.FnContract();
        }
      if (DCB.ForFileSnpScn())
        {
        ECI.sPath.FnContract();//ensure it is always in contract form!
        DCB.pC = ECI.sPath();
        }
      else
        {
        ECI.sPathForView = ECI.sPath;
        if (SymbolicPaths())
          ECI.sPathForView.FnContract();
        else
          ECI.sPathForView.FnExpand();
        DCB.pC = ECI.sPathForView();
        }
      return 1;
    case idmFile:
      if (DCB.rpC)
        {
        Strng Prev(ECI.sFile);
        ECI.sFile = DCB.rpC;
        if (Prev!=ECI.sFile)
          ECI.bReloadRqd = 1;
        }
      DCB.pC = ECI.sFile();
      return 1;
    case idmEditBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng FullFilename;
        int RetCode = DCB.DoEditBtn(ECI.hProcess, ECI.dwProcessId, ECI.sPath(), ECI.sFile(), NULL, &FullFilename, true);
        if (RetCode==-1)
          {
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            char Buff[2048];
            strcpy(Buff, "Time,Tag,Value\n");
            fwrite(Buff, sizeof(char), strlen(Buff), f);
            fclose(f);
            }
          RetCode = DCB.DoEditBtn(ECI.hProcess, ECI.dwProcessId, ECI.sPath(), ECI.sFile(), NULL);
          }
        }
      DCB.B=0;
      return 1;
    case idmBrowseBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Prev(ECI.sPath);
        Prev += ECI.sFile;
        DCB.DoBrowseBtn(NULL/*"csv"*/, "SysCAD Events", ECI.sPath, ECI.sFile);
        Strng Next(ECI.sPath);
        Next += ECI.sFile;
        if (Prev!=Next)
          ECI.bReloadRqd = 1;
        }
      DCB.B=0;
      return 1;
    case idmReloadBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        ECI.DoLoad();
        ECI.Start(ICGetTime());
        }
      DCB.B=0;
      return 1;
    case idmRestartBtn:
      if (DCB.rB && (*DCB.rB!=0))
        ECI.Start(ICGetTime());
      DCB.B=0;
      return 1;
    case idmTimePassed:
      if (DCB.rD)
        {
        ECI.LoadAndAdvance(ICGetTime(), *DCB.rD);
        }
      DCB.D=ECI.dTimePassed;
      return 1;
    case idmEventCnt:
      DCB.L=ECI.iRowCnt;
      return 1;
    case idmLastEvnTime:
      DCB.D=ECI.iRowCnt>0 ? ECI.RowData[ECI.iRowCnt-1]->m_dTime : 0.0;
      return 1;
    case idmNextTime:
      DCB.D=ECI.iCurIndex<ECI.iRowCnt ? ECI.RowData[ECI.iCurIndex]->m_dTime : dNAN;
      return 1;
    case idmNextTag:
      DCB.pC=ECI.iCurIndex<ECI.iRowCnt ? ECI.RowData[ECI.iCurIndex]->m_sOutputTag() : "";
      return 1;
    case idmNextOutput:
      DCB.D=ECI.iCurIndex<ECI.iRowCnt ? ECI.RowData[ECI.iCurIndex]->m_dOutputVal : dNAN;
      return 1;
    }//end switch
  return False;
  }

//--------------------------------------------------------------------------

flag CEventsCon::ValidateData(ValidateDataBlk & VDB)
  {
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CEventsCon::PreStartCheck()
  {
  bAboutToStart = 1;
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

void CEventsCon::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (bOn && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {
    if (ECI.bValid)
      {
      ECI.ExecIns(ICGetTime());
      }
    }
  }

//--------------------------------------------------------------------------

void CEventsCon::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  flag DoReset = false;
  switch (RqdState)
    {
    case MSA_PBInit   : 
    case MSA_ZeroFlows: 
    case MSA_Empty    : 
    case MSA_PreSet   : break;
    case MSA_DynStatsRunInit: DoReset = true; break;
    default: break;
    }
  if (DoReset && bOn)
    {
    ECI.Start(ICGetTime());
    }
  }

//--------------------------------------------------------------------------

void CEventsCon::EvalProducts(CNodeEvalIndex & NEI)
  {
  }

//--------------------------------------------------------------------------

int CEventsCon::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  if (ECI.sFile.Len()>0)
    {
    Strng Fn(ECI.sPath);
    Fn.FnExpand();
    Fn += ECI.sFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
    Cnt++;
    }
  FlwNode::FilesUsed(Files);
  return Files.GetCount();
  //return Cnt;
  }

//--------------------------------------------------------------------------

int CEventsCon::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  for (int j=0; j<ECI.iRowCnt; j++)
    {
    if (CTgFnIoVar::ContainsTag(ECI.RowData[j]->m_sOutputTag, pOldTag))
      LogWarning(Tag(), 0, "Change tag '%s' affects Events Controller model line %d.", pOldTag, j+2);
    }
  return FlwNode::ChangeTag(pOldTag, pNewTag);
  }

//--------------------------------------------------------------------------

int CEventsCon::DeleteTag(pchar pDelTag)
  {
  for (int j=0; j<ECI.iRowCnt; j++)
    {
    if (CTgFnIoVar::ContainsTag(ECI.RowData[j]->m_sOutputTag, pDelTag))
      LogWarning(Tag(), 0, "Delete tag '%s' affects Events Controller model line %d.", pDelTag, j+2);
    }
  return FlwNode::DeleteTag(pDelTag);
  }

//--------------------------------------------------------------------------
                                          
dword CEventsCon::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=(bOn ? FNS_On : FNS_Off);
  return Status;
  }

//--------------------------------------------------------------------------

flag CEventsCon::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\t?????????"; return 1;
    default:                                               
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================

#endif