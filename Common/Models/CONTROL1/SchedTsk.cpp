//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "schedtsk.h"
#include "scd_wm.h"
//#include "optoff.h"

//==========================================================================

const byte MT_Macro     = SUB_REPORT_MACRO;
const byte MT_TagRep    = SUB_REPORT_GETVALUES;
const byte MT_TrendRep  = SUB_REPORT_TREND;
const byte MT_SetTags   = SUB_REPORT_SETVALUES;

const byte STT_None     = 0;
const byte STT_Count    = 1;
const byte STT_Time     = 2;

const int MaxRepTags = 10;

const word idmScrFolder      = 1050;
const word idmScrEditBtn     = 1051;
const word idmScrBrowseBtn   = 1052;
const word idmScrExecBtn     = 1053;
const word idmMcrFolder      = 1060;
const word idmMcrEditBtn     = 1061;
const word idmMcrBrowseBtn   = 1062;
const word idmMcrExecBtn     = 1063;
const word idmRepFolder      = 1070;
const word idmRepEditBtn     = 1071;
const word idmRepBrowseBtn   = 1072;
const word idmRepExecBtn     = 1073;
const word idmRepEndTime     = 1074;
const word idmRepTagCnt      = 1075;
const word idmRepTag         = 1076; //leave MaxRepTags free after this


ScheduleTask::ScheduleTask()
  {
  bScrOn      = 0;
  sScrPath    = PrjFiles();
  iScrSchedType = STT_Count;
  iScrExecCnt = 100;
  iScrIterCnt = 0;
  dScrExecTime = 3600.0;
  dScrIterTime = 0.0;
  hScrProcess = NULL;

  bMcrOn      = 0;
  sMcrPath    = PrjFiles();
  iMcrSchedType = STT_None;
  iMcrExecCnt = 100;
  iMcrIterCnt = 0;
  dMcrExecTime = 3600.0;
  dMcrIterTime = 0.0;
  iMcrType    = MT_Macro;
  sMcrName    = "*";

  bRepOn      = 0;
  sRepPath    = PrjFiles();
  iRepSchedType = STT_None;
  iRepExecCnt = 100;
  iRepIterCnt = 0;
  dRepExecTime = 3600.0;
  dRepIterTime = 0.0;
  sRepTblName = "History";
  iRepTagCnt  = 4;
  RepTags.SetSize(iRepTagCnt);
  iRepPtsCnt  = 61;
  dRepDuration= 3600.0;
  sRepEndTime = "*";
  }

//--------------------------------------------------------------------------

void ScheduleTask::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj)
  {
  //DDB.Page("SchedTask", DDB_RqdPage);
  static DDBValueLst DDBSchedType[]=
    {
      {STT_None,              "None"         }, 
      {STT_Count,             "Iter Count"   },
      {STT_Time,              "Sim Time"     },
      {0}
    };
  if (DDB.BeginStruct(pObj, "Scr", NULL, DDB_NoPage))
    {
    DDB.Text("");
    DDB.Text("Schedule SysCAD Script...");
    DDB.CheckBoxBtn("On",          "",    DC_,     "", &bScrOn,         pObj, isParm, DDBYesNo);
    DDB.String("Status",           "",    DC_,     "", &sScrStatus,     pObj, isResult|noFileAtAll);
    DDB.String("Folder",           "",    DC_,     "", idmScrFolder,    pObj, isResult);
    DDB.String("File",             "",    DC_,     "", &sScrFile,       pObj, isParm);
    DDB.Button("Edit_ssc",         "",    DC_,     "", idmScrEditBtn,   pObj, isParm);
    DDB.Button("Browse_ssc",       "",    DC_,     "", idmScrBrowseBtn, pObj, isParm);
    DDB.Button("Execute_Now",      "",    DC_,     "", idmScrExecBtn,   pObj, isParm);
    DDB.Byte  ("SchedType",        "",    DC_,     "", &iScrSchedType,  pObj, isParm, DDBSchedType);
    DDB.Visibility(NSHM_All, iScrSchedType==STT_Count);
    DDB.Long  ("ExecCnt",          "",    DC_,     "", &iScrExecCnt,    pObj, isParm);
    DDB.Long  ("IterCnt",          "",    DC_,     "", &iScrIterCnt,    pObj, isParm);
    DDB.Visibility(NSHM_All, iScrSchedType==STT_Time);
    DDB.Double("ExecTime",         "",    DC_Time, "s", &dScrExecTime,  pObj, isParm);
    DDB.Double("SimTime",          "",    DC_Time, "s", &dScrIterTime,  pObj, isParm);
    DDB.Visibility();
    DDB.Text("");
    }
  DDB.EndStruct();

  static DDBValueLst DDBMcrType[]=
    {
      {MT_Macro,              "Macro"         }, 
      {MT_TagRep,             "Tag Report"    },
      {MT_TrendRep,           "Trend Report"  },
      {MT_SetTags,            "Set Tags"      },
      {0}
    };
  if (DDB.BeginStruct(pObj, "Mcr", NULL, DDB_NoPage))
    {
    DDB.Text("");
    DDB.Text("Schedule Excel Report or Macro...");
    DDB.CheckBoxBtn("On",          "",    DC_,     "", &bMcrOn,         pObj, isParm, DDBYesNo);
    DDB.String("Status",           "",    DC_,     "", &sMcrStatus,     pObj, isResult|noFileAtAll);
    DDB.String("Folder",           "",    DC_,     "", idmMcrFolder,    pObj, isResult);
    DDB.String("File",             "",    DC_,     "", &sMcrFile,       pObj, isParm);
    DDB.Byte  ("Type",             "",    DC_,     "", &iMcrType,       pObj, isParm, DDBMcrType);
    DDB.String("Name",             "",    DC_,     "", &sMcrName,       pObj, isParm);
    DDB.Button("Edit_xls",         "",    DC_,     "", idmMcrEditBtn,   pObj, isParm);
    DDB.Button("Browse_xls",       "",    DC_,     "", idmMcrBrowseBtn, pObj, isParm);
    DDB.Button("Execute_Now",      "",    DC_,     "", idmMcrExecBtn,   pObj, isParm);
    DDB.Byte  ("SchedType",        "",    DC_,     "", &iMcrSchedType,  pObj, isParm, DDBSchedType);
    DDB.Visibility(NSHM_All, iMcrSchedType==STT_Count);
    DDB.Long  ("ExecCnt",          "",    DC_,     "", &iMcrExecCnt,    pObj, isParm);
    DDB.Long  ("IterCnt",          "",    DC_,     "", &iMcrIterCnt,    pObj, isParm);
    DDB.Visibility(NSHM_All, iMcrSchedType==STT_Time);
    DDB.Double("ExecTime",         "",    DC_Time, "s", &dMcrExecTime,  pObj, isParm);
    DDB.Double("SimTime",          "",    DC_Time, "s", &dMcrIterTime,  pObj, isParm);
    DDB.Visibility();
    DDB.Text("");
    }
  DDB.EndStruct();

  DDB.Page("MDB_Trend", DDB_RqdPage);
  if (DDB.BeginStruct(pObj, "Rep", NULL, DDB_NoPage))
    {
    DDB.Text("");
    DDB.Text("Trend MDB Report...");
    DDB.CheckBoxBtn("On",          "",    DC_,     "", &bRepOn,         pObj, isParmStopped, DDBYesNo);
    DDB.String("Status",           "",    DC_,     "", &sRepStatus,     pObj, isResult|noFileAtAll);
    DDB.String("Folder",           "",    DC_,     "", idmRepFolder,    pObj, isResult);
    DDB.String("File",             "",    DC_,     "", &sRepFile,       pObj, isParm);
    DDB.String("TableName",        "",    DC_,     "", &sRepTblName,    pObj, isParm);
    DDB.Long  ("NoOfPoints",       "",    DC_,     "", &iRepPtsCnt,     pObj, isParm);
    DDB.Double("Duration",         "",    DC_Time, "s", &dRepDuration,  pObj, isParm);
    DDB.String("EndTime",          "",    DC_,     "", idmRepEndTime,   pObj, isParm);
    DDB.Long  ("NoOfTags",         "",    DC_,     "", idmRepTagCnt,    pObj, isParm);
    char Buff[16];
    for (int i=0; i<iRepTagCnt; i++)
      {
      sprintf(Buff, "Tag%d", i);
      DDB.String(Buff,             "",    DC_,     "", idmRepTag+i,     pObj, isParm|isTag);
      }
    DDB.Button("Browse_mdb",       "",    DC_,     "", idmRepBrowseBtn, pObj, isParm);
    DDB.Button("Execute_Now",      "",    DC_,     "", idmRepExecBtn,   pObj, isParm);
    DDB.Byte  ("SchedType",        "",    DC_,     "", &iRepSchedType,  pObj, isParm, DDBSchedType);
    DDB.Visibility(NSHM_All, iRepSchedType==STT_Count);
    DDB.Long  ("ExecCnt",          "",    DC_,     "", &iRepExecCnt,    pObj, isParm);
    DDB.Long  ("IterCnt",          "",    DC_,     "", &iRepIterCnt,    pObj, isParm);
    DDB.Visibility(NSHM_All, iRepSchedType==STT_Time);
    DDB.Double("ExecTime",         "",    DC_Time, "s", &dRepExecTime,  pObj, isParm);
    DDB.Double("SimTime",          "",    DC_Time, "s", &dRepIterTime,  pObj, isParm);
    DDB.Visibility();
    DDB.Text("");
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag ScheduleTask::DataXchg(DataChangeBlk & DCB, TaggedObject* pObj)
  {
  switch (DCB.lHandle)
    {
    case idmScrFolder:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot ???
        sScrPath = DCB.rpC;
        sScrPath.FnContract();
        }
      if (DCB.ForView())
        {
        if (SymbolicPaths())
          sScrPath.FnContract();
        else
          sScrPath.FnExpand();
        DCB.pC = sScrPath();
        }
      else
        {
        sScrPath.FnContract();
        DCB.pC = sScrPath();
        }
      return 1;
    case idmScrEditBtn:
      if (DCB.rB && (*DCB.rB!=0))
        DCB.DoEditBtn(hScrProcess, dwScrProcessId, sScrPath(), sScrFile(), "ssc");
      DCB.B=0;
      return 1;
    case idmScrBrowseBtn:
      if (DCB.rB && (*DCB.rB!=0))
        DCB.DoBrowseBtn("ssc", "SysCAD Script", sScrPath, sScrFile);
      DCB.B=0;
      return 1;
    case idmScrExecBtn:
      if (DCB.rB && (*DCB.rB!=0))
        ExecScr();
      DCB.B=0;
      return 1;

    case idmMcrFolder:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot ???
        sMcrPath = DCB.rpC;
        sMcrPath.FnContract();
        }
      if (DCB.ForView())
        {
        if (SymbolicPaths())
          sMcrPath.FnContract();
        else
          sMcrPath.FnExpand();
        DCB.pC = sMcrPath();
        }
      else
        {
        sMcrPath.FnContract();
        DCB.pC = sMcrPath();
        }
      return 1;
    case idmMcrEditBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        if (sMcrFile.Len()>0)
          {
          Strng Fn(sMcrPath);
          Fn.FnExpand();
          Fn.FnCheckEndBSlash();
          Fn += sMcrFile;
          CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, Fn(), "", SUB_REPORT_OPEN);
          AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
          }
        }
      DCB.B=0;
      return 1;
    case idmMcrBrowseBtn:
      if (DCB.rB && (*DCB.rB!=0))
        DCB.DoBrowseBtn("xls", "Excel", sMcrPath, sMcrFile);
      DCB.B=0;
      return 1;
    case idmMcrExecBtn:
      if (DCB.rB && (*DCB.rB!=0))
        ExecMcr();
      DCB.B=0;
      return 1;

    case idmRepFolder:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot ???
        sRepPath = DCB.rpC;
        sRepPath.FnContract();
        }
      if (DCB.ForView())
        {
        if (SymbolicPaths())
          sRepPath.FnContract();
        else
          sRepPath.FnExpand();
        DCB.pC = sRepPath();
        }
      else
        {
        sRepPath.FnContract();
        DCB.pC = sRepPath();
        }
      return 1;
    case idmRepEndTime:
      if (DCB.rpC)
        {
        flag EndTimeErr = false;
        sRepEndTime = DCB.rpC;
        if (sRepEndTime.Len()==0)
          sRepEndTime = "*";
        if (_stricmp(sRepEndTime(), "*")!=0)
          {
          CTimeValue EndTime;
          if (!EndTime.Parse(sRepEndTime()))
            {
            EndTimeErr = true;
            }
          else
            {
            sRepEndTime = EndTime.Format(TD_TimeDate);
            if (_stricmp(sRepEndTime(), "?")==0)
              EndTimeErr = true;
            }
          }
        //pObj->SetCI(1, EndTimeErr);
        }
      DCB.pC = sRepEndTime();
      return 1;
    case idmRepEditBtn:
      //if (DCB.rB)
      //  ???        
      DCB.B=0;
      return 1;
    case idmRepBrowseBtn:
      if (DCB.rB && (*DCB.rB!=0))
        DCB.DoBrowseBtn("mdb", "Access", sRepPath, sRepFile);
      DCB.B=0;
      return 1;
    case idmRepExecBtn:
      if (DCB.rB && (*DCB.rB!=0))
        ExecRep();
      DCB.B=0;
      return 1;
    case idmRepTagCnt:
      if (DCB.rL)
        {
        flag Chng = (*DCB.rL!=iRepTagCnt);
        iRepTagCnt = Range(0L, *DCB.rL, (long)MaxRepTags);
        RepTags.SetSize(iRepTagCnt);
        if (Chng)
          pObj->StructureChanged(pObj);
        }
      DCB.L = iRepTagCnt;
      return 1;
    default:
      if (DCB.lHandle>=idmRepTag)// && DCB.lHandle<idmRepTag+MaxRepTags)
        {
        ASSERT(DCB.lHandle<idmRepTag+MaxRepTags);
        const int index = DCB.lHandle-idmRepTag;
        /*if (index>=iRepTagCnt)
          {//for safety...
          iRepTagCnt = index+1;
          RepTags.SetSize(iRepTagCnt);
          }*/
        if (DCB.rpC)
          RepTags[index] = DCB.rpC;
        DCB.pC = RepTags[index]();
        return 1;
        }
    }
  return false;
  }

//--------------------------------------------------------------------------

void ScheduleTask::EvalDiscrete()
  {
  const double dT = ICGetTimeInc();
  //script...
  iScrExecCnt = max(iScrExecCnt, 10L);
  dScrExecTime = max(dScrExecTime, 5.0);
  if (bScrOn)
    {
    if ((iScrSchedType==STT_Count) && iScrIterCnt>0)
      {
      sScrStatus = "Execute";
      ldiv_t div_result;
      div_result = ldiv(iScrIterCnt, iScrExecCnt);
      if (div_result.rem==0)
        ExecScr();
      }
    else if ((iScrSchedType==STT_Time) && dScrIterTime>0.0)
      {
      sScrStatus = "Execute";
      const double d = dScrIterTime/dScrExecTime;
      if (d-(dScrIterTime*floor(d))<dT)
        ExecScr();
      }
    else
      sScrStatus = "On";
    }
  else
    sScrStatus = "Off";
  iScrIterCnt++;
  dScrIterTime += dT;

  //excel macro...
  iMcrExecCnt = max(iMcrExecCnt, 10L);
  dMcrExecTime = max(dMcrExecTime, 5.0);
  if (bMcrOn)
    {
    if ((iMcrSchedType==STT_Count) && iMcrIterCnt>0)
      {
      sMcrStatus = "Execute";
      ldiv_t div_result;
      div_result = ldiv(iMcrIterCnt, iMcrExecCnt);
      if (div_result.rem==0)
        ExecMcr();
      }
    else if ((iMcrSchedType==STT_Time) && dMcrIterTime>0.0)
      {
      sMcrStatus = "Execute";
      const double d = dMcrIterTime/dMcrExecTime;
      if (d-(dMcrIterTime*floor(d))<dT)
        ExecMcr();
      }
    else
      sMcrStatus = "On";
    }
  else
    sMcrStatus = "Off";
  iMcrIterCnt++;
  dMcrIterTime += dT;

  //Report...
  iRepExecCnt = max(iRepExecCnt, 10L);
  dRepExecTime = max(dRepExecTime, 5.0);
  if (bRepOn)
    {
    if ((iRepSchedType==STT_Count) && iRepIterCnt>0)
      {
      sRepStatus = "Execute";
      ldiv_t div_result;
      div_result = ldiv(iRepIterCnt, iRepExecCnt);
      if (div_result.rem==0)
        ExecRep();
      }
    else if ((iRepSchedType==STT_Time) && dRepIterTime>0.0)
      {
      sRepStatus = "Execute";
      const double d = dRepIterTime/dRepExecTime;
      if (d-(dRepIterTime*floor(d))<dT)
        ExecRep();
      }
    else
      sRepStatus = "On";
    }
  else
    sRepStatus = "Off";
  iRepIterCnt++;
  dRepIterTime += dT;
  }

//--------------------------------------------------------------------------

void ScheduleTask::ExecScr()
  {
  Strng Fn(sScrPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sScrFile;
  if (FileExists(Fn()))
    {
    sScrStatus = "Ok";
    char* pTxt = new char[Fn.Len()+1];
    strcpy(pTxt, Fn());
    //AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_RUNSCRIPT, (LPARAM)pTxt);
    ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_RUNSCRIPT, (LPARAM)pTxt);
    }
  else
    sScrStatus = "Specified file not found!";
  }

//--------------------------------------------------------------------------

void ScheduleTask::ExecMcr()
  {
  Strng Fn(sMcrPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sMcrFile;
  if (FileExists(Fn()))
    {
    sMcrStatus = "Ok";
    gs_Exec.AddReportRequest(new CXM_OleExcelReport(NULL, Fn(), sMcrName(), iMcrType, TRUE));
    //CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, Fn(), sMcrName(), iMcrType);
    ////AfxGetMainWnd()->SendMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
    //ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
    }
  else
    sMcrStatus = "Specified file not found!";
  }

//--------------------------------------------------------------------------

void ScheduleTask::ExecRep()
  {
  Strng Fn(sRepPath);
  Fn.FnExpand();
  Fn.FnCheckEndBSlash();
  Fn += sRepFile;
  flag OK = true;
  if (!FileExists(Fn()))
    {
    OK = false;
    sRepStatus = "Specified file not found!";
    }
  if (sRepTblName.Len()==0)
    {
    OK = false;
    sRepStatus = "Table Name not specified!";
    }
  flag GotOne = false;
  for (int i=0; i<iRepTagCnt && !GotOne; i++)
    {
    if (RepTags[i].Len()==0)
      GotOne = true;
    }
  if (!GotOne)
    {
    OK = false;
    sRepStatus = "No tags specified!";
    }
  CTimeValue EndTime;
  if (sRepEndTime.Len()==0 || _stricmp(sRepEndTime(), "*")==0)
    EndTime = gs_Exec.TheTime;
  else
    {
    if (!EndTime.Parse(sRepEndTime()))
      {
      OK = false;
      sRepStatus = "Invalid end time!";
      }
    else
      {
      sRepEndTime = EndTime.Format(TD_TimeDate);
      if (_stricmp(sRepEndTime(), "?")==0)
        {
        OK = false;
        sRepStatus = "Invalid end time!";
        }
      }
    }
  if (OK)
    {
    sRepStatus = "Ok";
    dRepDuration = Max(dRepDuration, 1.0);
    iRepPtsCnt = Max(iRepPtsCnt, 3L);
    CXM_RepTrendDB* pXB = new CXM_RepTrendDB(Fn(), sRepTblName(), EndTime, dRepDuration, iRepPtsCnt);
    for (int i=0; i<iRepTagCnt; i++)
      {
      RepTags[i].LRTrim(" \t'\"\r\n");
      if (RepTags[i].Len())
        pXB->xAddTag(RepTags[i]());
      }
    //AfxGetMainWnd()->SendMessage(WMU_REPORT, SUB_REPACTION_GENERATE, (LPARAM)pXB);
    ScdMainWnd()->PostMessage(WMU_REPORT, SUB_REPACTION_GENERATE, (LPARAM)pXB);
    }
  else
    sRepStatus = "Specified file not found!";
  }

//--------------------------------------------------------------------------

int ScheduleTask::FilesUsed(CFilesUsedArray & Files)
  {
  int Cnt = 0;
  Strng Fn;
  if (sScrFile.Len()>0)
    {
    Fn = sScrPath;
    Fn.FnExpand();
    Fn += sScrFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditTxt);
    Cnt++;
    }
  if (sMcrFile.Len()>0)
    {
    Fn = sMcrPath;
    Fn.FnExpand();
    Fn += sMcrFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditExcel);
    Cnt++;
    }
  if (sRepFile.Len()>0)
    {
    Fn = sRepPath;
    Fn.FnExpand();
    Fn += sRepFile;
    Files.AddFile(Fn(), FU_CopyFile|FU_EditMDB);
    Cnt++;
    }
  return Cnt;
  }

//--------------------------------------------------------------------------

int ScheduleTask::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  for (int i=0; i<iRepTagCnt; i++)
    CTgFnIoVar::ChangeTag(RepTags[i], pOldTag, pNewTag);
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int ScheduleTask::DeleteTag(pchar pDelTag, BOOL & FoundOne)
  {
  FoundOne = FALSE;
  for (int i=0; i<iRepTagCnt; i++)
    {
    if (CTgFnIoVar::ContainsTag(RepTags[i], pDelTag))
      FoundOne = TRUE;
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------
//==========================================================================
//
//  Schedule Task
//
//==========================================================================

double Drw_SchedTask[] = 
       { 
       DD_Arc, 0,0, 3.2,
       DD_Poly, 0.0,2.4, 0.0,0.0, 1.6,0.0,
       DD_TagPos, 0, -6.7,
       DD_End
       };

IMPLEMENT_MODELUNIT(CScheduleTask, "SchedTask", "", Drw_SchedTask, "Control", "TSK", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:Task",
                   "Schedule Task model.")

CScheduleTask::CScheduleTask(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
  {
  fHasFiles=1;
  AttachClassInfo(nc_Control, NULL, NULL, &NullFlwGroup);
  bAboutToStart = 0;
  }

//--------------------------------------------------------------------------

CScheduleTask::~CScheduleTask()
  {
  }

//--------------------------------------------------------------------------

void CScheduleTask::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CScheduleTask", NULL, DDB_NoPage);
  DDB.Text("");
  ST.BuildDataDefn(DDB, this);
  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CScheduleTask::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  if (ST.DataXchg(DCB, this))
    return 1;
  return False;
  }

//--------------------------------------------------------------------------

flag CScheduleTask::PreStartCheck()
  {
  bAboutToStart = 1;
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

void CScheduleTask::EvalDiscrete()
  {
  if (ICGetTimeInc()>0.0)
    ST.EvalDiscrete();
  bAboutToStart = 0;
  }

//--------------------------------------------------------------------------

int CScheduleTask::FilesUsed(CFilesUsedArray & Files)
  {
  ST.FilesUsed(Files);
  FlwNode::FilesUsed(Files);
  return Files.GetCount();
  }

//--------------------------------------------------------------------------

int CScheduleTask::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  ST.ChangeTag(pOldTag, pNewTag);
  return FlwNode::ChangeTag(pOldTag, pNewTag);
  }

//--------------------------------------------------------------------------

int CScheduleTask::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne;
  int RetCode = ST.DeleteTag(pDelTag, FoundOne);
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Schedule Task model '%s'.", pDelTag, Tag());
    }
  return FlwNode::DeleteTag(pDelTag);
  }

//--------------------------------------------------------------------------
                                          
dword CScheduleTask::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  //Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CScheduleTask::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tInvalid trend report end time"; return 1;
    default:                                               
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================

/*  byte data[16384];
  memset(data, 0, sizeof(data));
  CFileStatus Stat;
  BOOL b = CFile::GetStatus("c:\\print.dat", Stat);
  if (b)
    {
    DWORD dwCount=Stat.m_size;
    ASSERT(dwCount<sizeof(data));
    FILE* f = fopen("c:\\print.dat", "r");
    size_t ss = fread(data, 1, dwCount, f);
    b = RawDataToPrinter("\\\\PERPRINT\\PER0658", data, dwCount);
    fclose(f);
    }
BOOL RawDataToPrinter(LPSTR szPrinterName, LPBYTE lpData, DWORD dwCount)
   {
     HANDLE     hPrinter;
     DOC_INFO_1 DocInfo;
     DWORD      dwJob;
     DWORD      dwBytesWritten;

     // Need a handle to the printer.
     if( ! OpenPrinter( szPrinterName, &hPrinter, NULL ) )
       return FALSE;

     // Fill in the structure with info about this "document."
     DocInfo.pDocName = "My Document";
     DocInfo.pOutputFile = NULL;
     DocInfo.pDatatype = "RAW";
     // Inform the spooler the document is beginning.
     if( (dwJob = StartDocPrinter( hPrinter, 1, (/*LPSTR*//*LPBYTE)&DocInfo )) == 0 )
     {
       ClosePrinter( hPrinter );
       return FALSE;
     }
     // Start a page.
     if( ! StartPagePrinter( hPrinter ) )
     {
       EndDocPrinter( hPrinter );
       ClosePrinter( hPrinter );
       return FALSE;
     }
     // Send the data to the printer.
     if( ! WritePrinter( hPrinter, lpData, dwCount, &dwBytesWritten ) )
     {
       EndPagePrinter( hPrinter );
       EndDocPrinter( hPrinter );
       ClosePrinter( hPrinter );
       return FALSE;
     }
     // End the page.
     if( ! EndPagePrinter( hPrinter ) )
     {
       EndDocPrinter( hPrinter );
       ClosePrinter( hPrinter );
       return FALSE;
     }
     // Inform the spooler that the document is ending.
     if( ! EndDocPrinter( hPrinter ) )
     {
       ClosePrinter( hPrinter );
       return FALSE;
     }
     // Tidy up the printer handle.
     ClosePrinter( hPrinter );
     // Check to see if correct number of bytes were written.
     if( dwBytesWritten != dwCount )
       return FALSE;
     return TRUE;
   }
*/
