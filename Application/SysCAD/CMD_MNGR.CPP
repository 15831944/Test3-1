//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "cmd_mngr.h"
#include "gpwfuncs.h"
#include "syscad.h"
#include "mainfrm.h"
#include "project.h"
#include "scd_wm.h"
#include "mdlrunmngr.h"
#include "resource.h"
#include "scdcmdif.h"
//#include "scdappif.h"
//#include "mainfrm.h"
#include "dbgmngr.h"
#include "..\..\Com\ScdCOM\ScdCOMIF.h"
#include <atlconv.h>
#include "neutralmdl.h"
//#include "optoff.h"

#define DOMEMCHECKS 0
//(defined(_DEBUG))

#if DOMEMCHECKS
#define MEMCHECKS() {dbgpln("CrtCheck"); if (!_CrtCheckMemory()) DoBreak();};
#else
#define MEMCHECKS()
#endif

#define dbgScdCmdIF  01

#if dbgScdCmdIF
static CDbgMngr dbgDoComCmd("ScdCOM", "DoComCmd");
#endif

//===========================================================================

const DWORD CmdFlagStopped       = 0x0001;  //must SysCAD be stopped to execute command
const DWORD CmdFlagRunning       = 0x0002;  //must SysCAD be running to execute command
const DWORD CmdFlagStopIdle      = 0x0004;  //must SysCAD be stopped or idling to execute command
const DWORD CmdFlagStopRun       = 0x0008;  //must SysCAD be stopped or running to execute command
const DWORD CmdFlagIdleRun       = 0x0010;  //must SysCAD be idling or running to execute command
const DWORD CmdPrjLoaded         = 0x0020;  //must a project be loaded to execute command
const DWORD CmdFlagExecuteWait   = 0x0040;  //must this command only be executed after certain conditions are met in call to EO_Execute
const DWORD CmdFlagStopWait      = 0x0080;  //must this command only be executed after certain conditions are met in call to EO_Stopping
const DWORD CmdFlagDoNotExecNext = 0x0100;  //must wait for previous command to be completed
const DWORD CmdFlagDoNotPrompt   = 0x0200;  //do not let user interface give prompt for current command
const DWORD CmdFlagSaveOnExit    = 0x0400;  //Save On Exit for this command
const DWORD CmdFlagNoSaveOnExit  = 0x0800;  //NoSave On Exit for this command


/*const int NoOfCommands = ;
struct CmdInfo
  {
  char* pName;  //command name
  DWORD iParms; //number of parameters expected
  } ;*/


struct tCmdNameList { DWORD Cmd; LPCTSTR Name; } CmdNameList[] =
  {
    {ComCmd_NULL,                         "NULL"                        },
    {ComCmd_get_UpNAbout,                 "get_UpNAbout"                },
    {ComCmd_WaitUpNAbout,                 "WaitUpNAbout"                },
    {ComCmd_get_SyncCallsOn,              "get_SyncCallsOn"             },
    {ComCmd_put_SyncCallsOn,              "put_SyncCallsOn"             },
    {ComCmd_get_SyncCallsTimeOut,         "get_SyncCallsTimeOut"        },
    {ComCmd_put_SyncCallsTimeOut,         "put_SyncCallsTimeOut"        },

    {ComCmd_CreateProject,                "CreateProject"               },
    {ComCmd_LoadProject,                  "LoadProject"                 },
    {ComCmd_SaveProject,                  "SaveProject"                 },
    {ComCmd_SaveProjectNV,                "SaveProjectNV"               },
    {ComCmd_ExportNeutralDB,              "ExportNeutralDB"             },
    {ComCmd_ImportNeutralDB,              "ImportNeutralDB"             },
    {ComCmd_LoadSnapshot,                 "LoadSnapshot"                },
    {ComCmd_SaveSnapshot,                 "SaveSnapshot"                },
    {ComCmd_LoadScenario,                 "LoadScenario"                },
    {ComCmd_SaveScenario,                 "SaveScenario"                },
    {ComCmd_LoadBacktrack,                "LoadBacktrack"               },
    {ComCmd_SaveBacktrack,                "SaveBacktrack"               },
    {ComCmd_CloseProject,                 "CloseProject"                },
    {ComCmd_SetTag,                       "SetTag"                      },
    {ComCmd_GetTag,                       "GetTag"                      },
    {ComCmd_StartDynamicSolve,            "StartDynamicSolve"           },
    {ComCmd_StartProbalSolve,             "StartProbalSolve"            },
    {ComCmd_Start,                        "Start"                       },
    {ComCmd_Step,                         "Step"                        },
    {ComCmd_Stop,                         "Stop"                        },
    {ComCmd_Idle,                         "Idle"                        },
    {ComCmd_RunToSteadyState,             "RunToSteadyState"            },
    {ComCmd_Wait,                         "Wait"                        },
    {ComCmd_WaitCount,                    "WaitCount"                   },
    {ComCmd_WaitTillSteadyState,          "WaitTillSteadyState"         },
    {ComCmd_WaitTillStop,                 "WaitTillStop"                },
    {ComCmd_ResetWait,                    "ResetWait"                   },
    {ComCmd_Report_GetValues,             "GenerateTagReport"           },
    {ComCmd_Report_Trend,                 "GenerateTrendReport"         },
    {ComCmd_Report_SetValues,             "ProcessSetTags"              },
    {ComCmd_Report_SaveAndClose,          "SaveAndCloseReport"          },
    {ComCmd_Report_ExecuteMacro,          "ExecuteMacro"                },
    {ComCmd_Report_Process,               "ProcessReport"               },

    {ComCmd_DocCmd,                       "DocCmd"                      },

    {ComCmd_get_CmpFilename,              "get_CmpFilename"             },
    {ComCmd_put_CmpFilename,              "put_CmpFilename"             },
    {ComCmd_get_CmpSort,                  "get_CmpSort"                 },
    {ComCmd_put_CmpSort,                  "put_CmpSort"                 },
    {ComCmd_get_CmpMaxCount,              "get_CmpMaxCount"             },
    {ComCmd_put_CmpMaxCount,              "put_CmpMaxCount"             },
    {ComCmd_get_CmpRelativeTolerance,     "get_CmpRelativeTolerance"    },
    {ComCmd_put_CmpRelativeTolerance,     "put_CmpRelativeTolerance"    },
    {ComCmd_get_CmpAbsoluteTolerance,     "get_CmpAbsoluteTolerance"    },
    {ComCmd_put_CmpAbsoluteTolerance,     "put_CmpAbsoluteTolerance"    },
    {ComCmd_get_CmpShowDifferentStrings,  "get_CmpShowDifferentStrings" },
    {ComCmd_put_CmpShowDifferentStrings,  "put_CmpShowDifferentStrings" },
    {ComCmd_get_CmpShowMissingTags,       "get_CmpShowMissingTags"      },
    {ComCmd_put_CmpShowMissingTags,       "put_CmpShowMissingTags"      },
    {ComCmd_CompareOptions,               "CompareOptions"              },
    {ComCmd_CompareOptions2,              "CompareOptions2"             },
    {ComCmd_CompareReset,                 "CompareReset"                },
    {ComCmd_CompareScenarios,             "CompareScenarios"            },
    {ComCmd_CompareScenarioToCurrent,     "CompareScenarioToCurrent"    },
    {ComCmd_TestScenario,                 "TestScenario"                },
    {ComCmd_RestartHistorian,             "RestartHistorian"            },
    {ComCmd_Sleep,                        "Sleep"                       },
    {ComCmd_Exit,                         "Exit"                        },
    {ComCmd_get_EventsOn,                 "get_EventsOn"                },
    {ComCmd_put_EventsOn,                 "put_EventsOn"                },
    {ComCmd_get_IsStopped,                "get_IsStopped"               },
    {ComCmd_get_IsIdling,                 "get_IsIdling"                },
    {ComCmd_get_IsRunning,                "get_IsRunning"               },
    {ComCmd_get_StepSize,                 "get_StepSize"                },
    {ComCmd_put_StepSize,                 "put_StepSize"                },
    {ComCmd_get_RealTimeOn,               "get_RealTimeOn"              },
    {ComCmd_put_RealTimeOn,               "put_RealTimeOn"              },
    {ComCmd_get_RealTimeMultiplier,       "get_RealTimeMultiplier"      },
    {ComCmd_put_RealTimeMultiplier,       "put_RealTimeMultiplier"      },
    {ComCmd_get_SequenceNo,               "get_SequenceNo"              },
    {ComCmd_put_SequenceNo,               "put_SequenceNo"              },
    {ComCmd_SetAppWndState,               "SetAppWndState"              },
    {ComCmd_OnVariableSet,                "OnVariableSet"               },
    {ComCmd_VariableSetReplayItem,        "VariableSetReplayItem"       },
    {ComCmd_VariableSetReplayCtrl,        "VariableSetReplayCtrl"       },
    {ComCmd_get_TagValue,                 "get_TagValue"                },
    {ComCmd_put_TagValue,                 "put_TagValue"                },
    {ComCmd_EOC,                          "EOC"                         },

    {ComState_Idle,                       "ComState_Idle"               },
    {ComState_Stop,                       "ComState_Stop"               },
    {ComState_StepProbal,                 "ComState_StepProbal"         },
    {ComState_StepDynamic,                "ComState_StepDynamic"        },

    {ComCmd_get_TagType,                  "get_TagType"                 },
    {ComCmd_get_TagRecording,             "get_TagRecording"            },
    {ComCmd_put_TagRecording,             "put_TagRecording"            },

    {ComCmd_HistorianDelete,              "HistorianDelete"             },

    {0,                                   NULL                          },
  };

LPCTSTR CmdNames(DWORD Cmd)
  {
  for (int i=0; CmdNameList[i].Name; i++)
    if (Cmd==CmdNameList[i].Cmd)
      return CmdNameList[i].Name;
  return "???";
  }


//char* CmdNames[] =
//  { "", //NULL
//    "", //"get_UpNAbout",
//    "WaitUpNAbout",
//    "", //"get_SyncCallsOn",
//    "", //"put_SyncCallsOn",
//    "", //"get_SyncCallsTimeOut",
//    "", //"put_SyncCallsTimeOut",
//    "LoadProject",
//    "SaveProject",
//    "LoadSnapshot",
//    "SaveSnapshot",
//    "LoadScenario",
//    "SaveScenario",
//    "LoadBacktrack",
//    "SaveBacktrack",
//    "CloseProject",
//    "SetTag",
//    "GetTag",
//    "StartDynamicSolve",
//    "StartProbalSolve",
//    "", // "Start", Taken out - not in original Scripts only added for COM - problem waiting for ComState_StepProbal OR ComState_StepDynamic
//    "Step",
//    "Stop",
//    "Idle",
//    "Wait",
//    "WaitCount",
//    "WaitTillSteadyState",
//    "WaitTillStop",
//    "ResetWait",
//    "GenerateTagReport",
//    "GenerateTrendReport",
//    "ProcessSetTags",
//    "SaveAndCloseReport",
//    "ExecuteMacro",
//    "", //"get_CmpFilename",
//    "", //"put_CmpFilename",
//    "", //"get_CmpSort",
//    "", //"put_CmpSort",
//    "", //"get_CmpMaxCount",
//    "", //"put_CmpMaxCount",
//    "", //"get_CmpRelativeTolerance",
//    "", //"put_CmpRelativeTolerance",
//    "", //"get_CmpAbsoluteTolerance",
//    "", //"put_CmpAbsoluteTolerance",
//    "", //"get_CmpShowDifferentStrings",
//    "", //"put_CmpShowDifferentStrings",
//    "", //"get_CmpShowMissingTags",
//    "", //"put_CmpShowMissingTags",
//    "CompareOptions",
//    "CompareOptions2",
//    "CompareReset",
//    "CompareScenarios",
//    "CompareScenarioToCurrent",
//    "TestScenario",
//    "RestartHistorian",
//    "Sleep",
//    "Exit",
//    "", // get_EventsOn
//    "", // put_EventsOn
//    "", // get_IsStopped
//    "", // get_IsIdling
//    "", // get_IsRunning
//    "", // get_StepSize
//    "", // put_StepSize
//    "", // get_RealTimeOn
//    "", // put_RealTimeOn
//    "", // get_RealTimeMultiplier
//    "", // put_RealTimeMultiplier
//    "", // get_SequenceNo
//    "", // put_SequenceNo
//    "SetAppWndState",
//    "OnVariableSet",
//    "VariableSetReplayItem",
//    "VariableSetReplayCtrl",
//    "", // get_TagValue
//    "", // put_TagValue
//    "EOC",
//  };

//===========================================================================

static inline void LclPostMessage(UINT Msg, WPARAM w, LPARAM l=0)
  {
  while (!::ScdMainWnd()->PostMessage(Msg, w, l))
    {
#if dbgScdCmdIF
if (1 || dbgDoComCmd())
  dbgpln("@@@@@@ REPOST[%i] %2i:%2i",GetLastError(),Msg, w);
#endif
    Sleep(10);
    }
  }

//===========================================================================

class CScriptInfo : public CDialog
  {
  public:
    CScriptInfo(CWnd* pParent = NULL);   // standard constructor
    ~CScriptInfo();
    void SetInfo(int CmdCount, char* pCurCmdTxt, char* pNxtCmdTxt);
    //{{AFX_DATA(CScriptInfo)
    enum { IDD = IDD_SCRIPTINFO };
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CScriptInfo)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CScriptInfo)
    virtual BOOL OnInitDialog();
    afx_msg void OnAbort();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

CScriptInfo* pInfoWnd = NULL;     //pointer to window showing script progress

//--------------------------------------------------------------------------

CScriptInfo::CScriptInfo(CWnd* pParent /*=NULL*/)
  : CDialog(CScriptInfo::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CScriptInfo)
  //}}AFX_DATA_INIT
  Create(CScriptInfo::IDD, pParent); //create modeless dialog
  }

//--------------------------------------------------------------------------

CScriptInfo::~CScriptInfo()
  {
  pInfoWnd = NULL;
  }

//--------------------------------------------------------------------------

void CScriptInfo::PostNcDestroy()
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//--------------------------------------------------------------------------

void CScriptInfo::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScriptInfo)
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CScriptInfo, CDialog)
  //{{AFX_MSG_MAP(CScriptInfo)
  ON_BN_CLICKED(IDC_ABORT, OnAbort)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CScriptInfo::OnInitDialog()
  {
  CDialog::OnInitDialog();
  CRect R;
  GetWindowRect(&R);
  CRect CR;
  GetParent()->GetWindowRect(&CR);
  SetWindowPos(NULL, CR.right - R.Width() - 5, CR.bottom - R.Height() - 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
  return TRUE;
  }

//--------------------------------------------------------------------------

void CScriptInfo::SetInfo(int CmdCount, char* pCurCmdTxt, char* pNxtCmdTxt)
  {
  char Buff[512];
  sprintf(Buff, "Number of script commands to execute:%d", Max(0, CmdCount-1));
  SetDlgItemText(IDC_CMDCNT, Buff);
  SetDlgItemText(IDC_CURRENTCMD, pCurCmdTxt);
  SetDlgItemText(IDC_NEXTCMD, pNxtCmdTxt);
  UpdateWindow();
  }

//---------------------------------------------------------------------------

void CScriptInfo::OnAbort()
  {
  gs_pXCmd->AbortScripts();
  }

//===========================================================================

CCmdBase::CCmdBase(WORD ID, WORD DoneID/*=0*/)
  {
  CmdID = ID;
  CmdDoneID = (DoneID>0) ? DoneID : ID;
  //pNext = NULL;
  flags = CmdPrjLoaded;
  }

//---------------------------------------------------------------------------

CCmdBase::~CCmdBase()
  {
  }

//---------------------------------------------------------------------------

char* CCmdBase::Desc(char* Buff)
  {
  sprintf(Buff, "%s()", CmdNames(CmdID));
  return Buff;
  }

//===========================================================================

class CCmdDirect : public CCmdBase
  {
  public:
    WPARAM     CommandID;

    CCmdDirect(WORD ID, WORD DoneID, WPARAM CmdID, DWORD flgs) : CCmdBase(ID, DoneID) { CommandID = CmdID; flags |= flgs; };
    virtual ~CCmdDirect(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { ScdMainWnd()->PostMessage(WM_COMMAND, CommandID, 0); return 0; };
  };

//---------------------------------------------------------------------------

class CCmdSolvePB : public CCmdDirect
  {
  public:
    CCmdSolvePB(WORD ID, WORD DoneID, WPARAM CmdID, DWORD flgs) : CCmdDirect(ID, DoneID, CmdID, flgs) { };
    virtual ~CCmdSolvePB(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { return CCmdDirect::ExecCmd(pXCmd); };
  };

//---------------------------------------------------------------------------

class CCmdSolveDyn : public CCmdDirect
  {
  public:
    CCmdSolveDyn(WORD ID, WORD DoneID, WPARAM CmdID, DWORD flgs) : CCmdDirect(ID, DoneID, CmdID, flgs) { };
    virtual ~CCmdSolveDyn(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { return CCmdDirect::ExecCmd(pXCmd); };
  };

//---------------------------------------------------------------------------

class CCmdSolve : public CCmdDirect
  {
  public:
    CCmdSolve(WORD ID, WORD DoneID, WPARAM CmdID, DWORD flgs) : CCmdDirect(ID, DoneID, CmdID, flgs) { };
    virtual ~CCmdSolve(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { return CCmdDirect::ExecCmd(pXCmd); };
  };

//---------------------------------------------------------------------------

class CCmdLoadProject : public CCmdBase
  {
  public:
    Strng sPrjName;

    CCmdLoadProject(char* pPrjName) : CCmdBase(ComCmd_LoadProject) { sPrjName.FnSearchExpand(pPrjName, AF_All|AF_BackupFiles); flags |= CmdFlagStopped | CmdFlagDoNotPrompt | CmdFlagDoNotExecNext; flags &= (~CmdPrjLoaded); };
    virtual ~CCmdLoadProject() {};
    virtual int ExecCmd(CCmdExec* pXCmd);
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(\"%s\")", CmdNames(CmdID), sPrjName()); return Buff; };
  };

int CCmdLoadProject::ExecCmd(CCmdExec* pXCmd)
  {
  CDocument* pPrevPrjDoc = (gs_pPrj ? gs_pPrj->pPrjDoc : NULL);
  CDocument* pDoc = ScdApp()->OpenDocumentFile(sPrjName());
  if (pDoc)
    {
    if (pDoc==pPrevPrjDoc) //project was allready open
      ScdMainWnd()->PostMessage(WMU_CMDDONE, ComCmd_LoadProject, 0); //let script cmd mngr know cmd is complete
    return 0;
    }
  else
    {
    LogError("Command", 0, "Unable to open %s", sPrjName());
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

class CCmdCloseProject : public CCmdBase
  {
  public:
    int iTryCnt;

    CCmdCloseProject() : CCmdBase(ComCmd_CloseProject) { flags |= CmdFlagStopped | CmdFlagDoNotPrompt | CmdFlagDoNotExecNext; iTryCnt = 0; };
    virtual ~CCmdCloseProject() {};
    virtual int ExecCmd(CCmdExec* pXCmd);
  };

int CCmdCloseProject::ExecCmd(CCmdExec* pXCmd)
  {
  Strng_List Problems;
  switch (gs_Exec.CanClose(Problems))
    {
    case EO_CanClose_Yes:
      AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_CLOSE, 0);
      return 0;
    case EO_CanClose_Wait:
      if (iTryCnt++<6)
        {
        Sleep(250 + (iTryCnt*250)); //try give executive objects time to finish, so that project can be closed
        return -1;
        }
      break;
    case EO_CanClose_Save:
    case EO_CanClose_No:
      if (iTryCnt++<1)
        {
        Sleep(750); //try ONCE to give executive objects time to finish, so that project can be closed
        return -2;
        }
      break;
    }
  LogError("Command", 0, "Cannot close project");
  return 1;
  }

//---------------------------------------------------------------------------

class CCmdWait : public CCmdBase
  {
  public:
    double  dWaitTime;

    CCmdWait(double WaitTime) : CCmdBase(ComCmd_Wait) { dWaitTime = Max(0.0, WaitTime); flags |= CmdFlagExecuteWait | CmdFlagRunning; };
    virtual ~CCmdWait() {};
    virtual int ExecCmd(CCmdExec* pXCmd) { return 0; };
    virtual flag ExecNow(CCmdExec* pXCmd) { return (pXCmd->dCurTime - pXCmd->dStartTime >= dWaitTime); };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(%g)", CmdNames(CmdID), dWaitTime); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdWaitCount : public CCmdBase
  {
  public:
    DWORD  dwWaitCount;

    CCmdWaitCount(DWORD WaitCount) : CCmdBase(ComCmd_WaitCount) { dwWaitCount = WaitCount; flags |= CmdFlagExecuteWait | CmdFlagRunning; };
    virtual ~CCmdWaitCount() {};
    virtual int ExecCmd(CCmdExec* pXCmd) { return 0; };
    virtual flag ExecNow(CCmdExec* pXCmd) { return (pXCmd->dwIterCount >= dwWaitCount); };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(%d)", CmdNames(CmdID), dwWaitCount); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdWaitTillStop : public CCmdBase
  {
  public:
    CCmdWaitTillStop() : CCmdBase(ComCmd_WaitTillStop) { flags |= CmdFlagStopWait/* | CmdFlagRunning*/; };
    virtual ~CCmdWaitTillStop(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { return 0; };
    virtual flag ExecNow(CCmdExec* pXCmd) { return 1; };
  };

//---------------------------------------------------------------------------

class CCmdWaitTillSteadyState : public CCmdBase
  {
  public:
    CCmdWaitTillSteadyState() : CCmdBase(ComCmd_WaitTillStop) { flags |= CmdFlagExecuteWait | CmdFlagRunning; };
    virtual ~CCmdWaitTillSteadyState(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { return 0; };
    virtual flag ExecNow(CCmdExec* pXCmd);
  };

flag CCmdWaitTillSteadyState::ExecNow(CCmdExec* pXCmd)
  {
  //CNM_TODO
  //this is called after every iteration while the script is waiting for steady state,
  //return true when steady state is acheived.  How do we know this???
  return 1;
  }

//---------------------------------------------------------------------------

class CCmdResetWait : public CCmdBase
  {
  public:
    CCmdResetWait() : CCmdBase(ComCmd_ResetWait) {};
    virtual ~CCmdResetWait(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { pXCmd->dwIterCount = 0; pXCmd->dStartTime = pXCmd->dCurTime; return 0; };
  };

//---------------------------------------------------------------------------

class CCmdGenerateReport : public CCmdBase
  {
  public:
    short iReportType;
    Strng sRepFilename;
    Strng sRepName;

    CCmdGenerateReport(short ReportType, char* RepFilename, char* RepName);
    virtual ~CCmdGenerateReport() {};
    virtual int ExecCmd(CCmdExec* pXCmd);
    virtual char* Desc(char* Buff)
      {
      if (iReportType==SUB_REPORT_SAVEANDCLOSE)
        sprintf(Buff, "%s(\"%s\")", CmdNames(CmdID), sRepFilename());
      else
        sprintf(Buff, "%s(\"%s\", \"%s\")", CmdNames(CmdID), sRepFilename(), sRepName());
      return Buff;
      };
  };

inline int CmdIDFromRptType(int iRpt)
  {
  switch (iRpt)
    {
    case SUB_REPORT_GETVALUES: return ComCmd_Report_GetValues;
    case SUB_REPORT_TREND: return ComCmd_Report_Trend;
    case SUB_REPORT_SETVALUES: return ComCmd_Report_SetValues;
    case SUB_REPORT_SAVEANDCLOSE: return ComCmd_Report_SaveAndClose;
    case SUB_REPORT_MACRO: return ComCmd_Report_ExecuteMacro;
    default:
      DoBreak();
    }
  return -1;
  };

CCmdGenerateReport::CCmdGenerateReport(short ReportType, char* RepFilename, char* RepName) :
  CCmdBase(CmdIDFromRptType(ReportType))
  {
  iReportType = ReportType;
  sRepName = RepName;
  sRepFilename.FnSearchExpand(RepFilename, AF_All|AF_BackupFiles);
  CmdID=CmdIDFromRptType(ReportType);
  flags |= CmdFlagDoNotExecNext;
  if (CmdID!=ComCmd_Report_Trend)
    flags |= CmdFlagStopped;
  if (CmdID==ComCmd_Report_ExecuteMacro)
    flags &= (~CmdPrjLoaded);
  }

int CCmdGenerateReport::ExecCmd(CCmdExec* pXCmd)
  {
  CXM_OleExcelReport* pXB = new CXM_OleExcelReport(NULL, sRepFilename(), sRepName(), iReportType);
  MainWnd()->SendMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
  return 0;
  }

//---------------------------------------------------------------------------

class CCmdSetTag : public CCmdBase
  {
  public:
    double dValue;
    Strng sTag;

    CCmdSetTag(char* Tag, double Value) : CCmdBase(ComCmd_SetTag) { sTag = Tag; dValue = Value; };
    virtual ~CCmdSetTag(){};
    virtual int ExecCmd(CCmdExec* pXCmd);
    virtual char* Desc(char* Buff) { sprintf(Buff, Valid(dValue) ? "%s(\"%s\", %g)" : "%s(\"%s\", *)", CmdNames(CmdID), sTag(), dValue); return Buff; };
  };

int CCmdSetTag::ExecCmd(CCmdExec* pXCmd)
  {
  Strng WrkTag, WrkCnvTxt;
  TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
  flag UseCnv = (WrkCnvTxt.Length()>0);
  CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0));
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (pXCmd->XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem = ObjData.FirstItem();
    //flag Editable = ((pItem->Flags() & isParm)!=0);
    if (IsNumData(pItem->Type()))
      {
      if (UseCnv && Cnvs[pItem->CnvIndex()]->Find(WrkCnvTxt())==NULL)
        {
        UseCnv = 0;
        LogWarning("Command", 0, "SetTag: Valid conversion expected for tag %s", WrkTag());
        }
      }
    else
      {
      LogWarning("Command", 0, "SetTag: Valid tag expected (%s)", WrkTag());
      return 1;
      }

    PkDataUnion DU;
    if (UseCnv)
      DU.SetTypeDouble(pItem->Type(), dValue, pItem->CnvIndex(), WrkCnvTxt());
    else
      DU.SetTypeDouble(pItem->Type(), dValue);
    CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
    if (pXCmd->XWriteTaggedItem(ObjData, Route)!=TOData_OK)
      LogWarning("Command", 0, "SetTag: Write failed for %s", WrkTag());
    else
      if (UseCnv)
        gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Script, WrkTag(), dValue, WrkCnvTxt());
      else
        gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Script, WrkTag(), dValue);
    }
  else
    {
    LogWarning("Command", 0, "SetTag: Valid tag expected (%s)", WrkTag());
    return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

class CCmdLoadSnapshot : public CCmdBase
  {
  public:
    Strng sFilename;

    CCmdLoadSnapshot(char* pFilename) : CCmdBase(ComCmd_LoadSnapshot) { sFilename.FnSearchExpand(pFilename, AF_All|AF_BackupFiles); flags |= CmdFlagDoNotExecNext; };
    virtual ~CCmdLoadSnapshot(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { gs_pPrj->DoLoadSnapShotScenario(True, sFilename()); return 0; };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(\"%s\")", CmdNames(CmdID), sFilename()); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdSaveSnapshot : public CCmdBase
  {
  public:
    Strng sFilename;

    CCmdSaveSnapshot(char* pFilename) : CCmdBase(ComCmd_SaveSnapshot) { sFilename.FnSearchExpand(pFilename, AF_All|AF_BackupFiles); flags |= CmdFlagDoNotExecNext; };
    virtual ~CCmdSaveSnapshot(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { gs_pPrj->DoSaveSnapShotScenario(True, sFilename()); return 0; };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(\"%s\")", CmdNames(CmdID), sFilename()); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdLoadScenario : public CCmdBase
  {
  public:
    Strng sFilename;

    CCmdLoadScenario(char* pFilename) : CCmdBase(ComCmd_LoadScenario) { sFilename.FnSearchExpand(pFilename, AF_All|AF_BackupFiles); flags |= CmdFlagDoNotExecNext; };
    virtual ~CCmdLoadScenario(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { gs_pPrj->DoLoadSnapShotScenario(False, sFilename()); return 0; };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(\"%s\")", CmdNames(CmdID), sFilename()); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdSaveScenario : public CCmdBase
  {
  public:
    Strng sFilename;

    CCmdSaveScenario(char* pFilename) : CCmdBase(ComCmd_SaveScenario) { sFilename.FnSearchExpand(pFilename, AF_All|AF_BackupFiles); flags |= CmdFlagDoNotExecNext; };
    virtual ~CCmdSaveScenario(){};
    virtual int ExecCmd(CCmdExec* pXCmd) { gs_pPrj->DoSaveSnapShotScenario(False, sFilename()); return 0; };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(\"%s\")", CmdNames(CmdID), sFilename()); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdRestartHistorian : public CCmdBase
  {
  public:
    CCmdRestartHistorian() : CCmdBase(ComCmd_RestartHistorian) { flags |= CmdFlagStopped; };
    virtual ~CCmdRestartHistorian(){};
    virtual int ExecCmd(CCmdExec* pXCmd);
  };

int CCmdRestartHistorian::ExecCmd(CCmdExec* pXCmd)
  {
  if (gs_pPrj)
    {
    BOOL DidClose = gs_pPrj->IsHistorianOpen();
    if (DidClose)
      gs_pPrj->CloseHistorian();
    gs_pPrj->RestartHistorian();
    if (DidClose)
      gs_pPrj->OpenHistorian();
    }
  return 0;
  }

//---------------------------------------------------------------------------

class CCmdCompareOptions : public CCmdBase
  {
  public:
    Strng sFilename;
    flag bShowSortedFile;

    CCmdCompareOptions(char* pFilename, flag ShowSortedFile) : CCmdBase(ComCmd_CompareOptions)
      { sFilename.FnSearchExpand(pFilename, AF_All|AF_BackupFiles); bShowSortedFile = ShowSortedFile; flags &= (~CmdPrjLoaded); };
    virtual ~CCmdCompareOptions(){};
    virtual int ExecCmd(CCmdExec* pXCmd)
      { pXCmd->CompInfo.sOutFilename = sFilename; pXCmd->CompInfo.bShowSortedFile = bShowSortedFile; return 0; };
  };

//---------------------------------------------------------------------------

class CCmdCompareOptions2 : public CCmdBase
  {
  public:
    long iMaxCount;
    double dRelTolerance;
    double dAbsTolerance;
    flag bShowStrings;
    flag bShowAllMissing;

    CCmdCompareOptions2(long MaxCount, double RelTolerance, double AbsTolerance, flag ShowStrings, flag ShowAllMissing) : CCmdBase(ComCmd_CompareOptions2)
      { iMaxCount = MaxCount; dRelTolerance = RelTolerance; dAbsTolerance = AbsTolerance; bShowStrings = ShowStrings; bShowAllMissing = ShowAllMissing; flags &= (~CmdPrjLoaded); };
    virtual ~CCmdCompareOptions2(){};
    virtual int ExecCmd(CCmdExec* pXCmd)
      { pXCmd->CompInfo.iMaxCount = iMaxCount; pXCmd->CompInfo.dRelTolerance = dRelTolerance; pXCmd->CompInfo.dAbsTolerance = dAbsTolerance;
        pXCmd->CompInfo.bShowStrings = bShowStrings; pXCmd->CompInfo.bShowAllMissing = bShowAllMissing; return 0; };
  };

//---------------------------------------------------------------------------

class CCmdCompareReset : public CCmdBase
  {
  public:
    CCmdCompareReset() : CCmdBase(ComCmd_CompareReset) { flags &= (~CmdPrjLoaded); };
    virtual int ExecCmd(CCmdExec* pXCmd)
      { pXCmd->bResetNextCompOutFile = 1; return 0; };
  };

//---------------------------------------------------------------------------

class CCmdCompareScenarios : public CCmdBase
  {
  public:
    flag bCurrent;
    Strng sFilename1;
    Strng sFilename2;

    CCmdCompareScenarios(flag Current, char* pFilename1, char* pFilename2) : CCmdBase(ComCmd_CompareScenarios)
      { bCurrent = Current; sFilename1.FnSearchExpand(pFilename1, AF_All|AF_BackupFiles); sFilename2.FnSearchExpand(pFilename2, AF_All|AF_BackupFiles); if (bCurrent) CmdID = ComCmd_CompareScenarioToCurrent; flags |= CmdFlagStopped | CmdFlagDoNotExecNext; };
    virtual ~CCmdCompareScenarios(){};
    virtual int ExecCmd(CCmdExec* pXCmd);
  };

int CCmdCompareScenarios::ExecCmd(CCmdExec* pXCmd)
  {
  if (gs_pPrj)
    {
    pXCmd->CompInfo.pRoot = PrjFiles();
    pXCmd->CompInfo.sFilename1 = sFilename1;
    pXCmd->CompInfo.sFilename2 = sFilename2;
    pXCmd->CompInfo.bCurrent = bCurrent;
    pXCmd->CompInfo.bResetOutFile = pXCmd->bResetNextCompOutFile;
    pXCmd->bResetNextCompOutFile = 0;
    int RetVal = gs_Exec.CompareScenarios(pXCmd->CompInfo);
    }
  return 0;
  }

//---------------------------------------------------------------------------

class CCmdTestScenario : public CCmdBase
  {
  public:
    Strng sFilename;

    CCmdTestScenario(char* pFilename) : CCmdBase(ComCmd_TestScenario)
      { sFilename.FnSearchExpand(pFilename, AF_All|AF_BackupFiles); CmdID = ComCmd_TestScenario; flags |= CmdFlagStopped; };// | CmdFlagDoNotExecNext; };
    virtual ~CCmdTestScenario(){};
    virtual int ExecCmd(CCmdExec* pXCmd);
  };

int CCmdTestScenario::ExecCmd(CCmdExec* pXCmd)
  {
  if (gs_pPrj)
    {
    // Does File Exist ?
    Strng Fn, Path(sFilename());
    if (Path.Length()==0)
      Fn.Set("%s%s", PrjFiles(), sFilename());
    else
      Fn = sFilename();

    HANDLE H1 = CreateFile(Fn(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (H1!=INVALID_HANDLE_VALUE)
      {
      CloseHandle(H1);
      // File Exists .. do compare
      pXCmd->CompInfo.pRoot = PrjFiles();
      pXCmd->CompInfo.sFilename1 = sFilename;
      pXCmd->CompInfo.sFilename2 = "";
      pXCmd->CompInfo.bCurrent = True;
      pXCmd->CompInfo.bResetOutFile = pXCmd->bResetNextCompOutFile;
      pXCmd->bResetNextCompOutFile = 0;
      int RetVal = gs_Exec.CompareScenarios(pXCmd->CompInfo);
      }
    else
      {
      // File does not Exists .. do Save
      gs_pPrj->DoSaveSnapShotScenario(False, sFilename());
      }
    }
  return 0;
  }

//---------------------------------------------------------------------------

class CCmdSleep : public CCmdBase
  {
  public:
    long iSeconds;
    CCmdSleep(long Seconds) : CCmdBase(ComCmd_Sleep)
      {
      iSeconds = Max(0L, Seconds);
      flags |= CmdFlagStopped;// | CmdFlagDoNotExecNext | CmdFlagDoNotPrompt;
      flags &= (~CmdPrjLoaded);
      };
    virtual int ExecCmd(CCmdExec* pXCmd)
      {
      Sleep(iSeconds*1000); //perhaps should use timer or use sleep on another user-thread, so that main-thread isn't paused!?
      return 0;
      };
    virtual char* Desc(char* Buff) { sprintf(Buff, "%s(%d)", CmdNames(CmdID), iSeconds); return Buff; };
  };

//---------------------------------------------------------------------------

class CCmdExit : public CCmdBase
  {
  public:
    CCmdExit(int WithSave) : CCmdBase(ComCmd_Exit)
      {
      flags |= CmdFlagStopped | CmdFlagDoNotExecNext | CmdFlagDoNotPrompt;
      flags |= (WithSave ? CmdFlagSaveOnExit : CmdFlagNoSaveOnExit);
      flags &= (~CmdPrjLoaded);
      };
    virtual ~CCmdExit(){};
    virtual int ExecCmd(CCmdExec* pXCmd);
  };

int CCmdExit::ExecCmd(CCmdExec* pXCmd)
  {
  ScdMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
  return 0;
  }

//---------------------------------------------------------------------------
// End of cmds - primarly ensure that ScriptRunning returns correct Value
class CCmdEOC : public CCmdBase
  {
  public:
    CCmdEOC() : CCmdBase(ComCmd_EOC)
      {
      flags |= CmdFlagDoNotPrompt;
      };
    virtual ~CCmdEOC(){};
    virtual int ExecCmd(CCmdExec* pXCmd);
  };

int CCmdEOC::ExecCmd(CCmdExec* pXCmd)
  {
  // Do Nothing;
  return 0;
  }

//===========================================================================
//===========================================================================

CCmdExec* gs_pXCmd = NULL;

//===========================================================================

CCmdExec::CCmdExec()
  {
  //gs_XCmd = this;
  EO_Register(pExecName_ExecCmd, /*EOWrite_Msg|EORead_Msg|*/EOExec_Msg, /*Pri*/THREAD_PRIORITY_NORMAL, /*Stack*/10000);
  //pFirstCmd = NULL;
  //pLastCmd = NULL;
  //iCmdCount = 0;
  dStartTime = 0.0;
  dwIterCount = 0;
  iCmdAttempts = 0;
  pInfoWnd = NULL;
  bCOMCmdBusy=0;
  Reset();
  }

//---------------------------------------------------------------------------

CCmdExec::~CCmdExec()
  {
  if (pInfoWnd)
    pInfoWnd->DestroyWindow();
  EO_DeRegister();
  //gs_pXCmd = NULL;
  }

//---------------------------------------------------------------------------

void CCmdExec::Reset()
  {
  bIdling = 1; // Must get in At Start
  bResetNextCompOutFile = 0;
  bDoNotPrompt = 0;
  bSaveOnExit = 0;
  bNoSaveOnExit = 0;
  wCmdWaiting = 0;
  CompInfo.sOutFilename = "compare.txt";
  CompInfo.iMaxCount = 200;
  CompInfo.dRelTolerance = 0.0001;
  CompInfo.dAbsTolerance = 1.0e-6;
  CompInfo.bWindow = 0;
  CompInfo.bShowStrings = 1;
  CompInfo.bShowAllMissing = 0;
  CompInfo.bResetOutFile = 0;
  CompInfo.bShowAllTags = 0;
  CompInfo.bShowSortedFile = 0;
  }

//---------------------------------------------------------------------------

CCmdBase* CCmdExec::AddCmd(CCmdBase* pNewCmd)
  {
//  if (pLastCmd==NULL)
//    {
//    pFirstCmd = new CCmdEOC();
//    pLastCmd = pFirstCmd;
//    }
  //if (pFirstCmd)
  //  pLastCmd->pNext = pNewCmd;
  //else
  //  pFirstCmd = pNewCmd;
  //pLastCmd = pNewCmd;
  //iCmdCount++;

  if (!Cmds.IsEmpty() && Cmds.GetTail()->CmdID==ComCmd_EOC)
    {
    POSITION Tail=Cmds.GetTailPosition();
    Cmds.InsertBefore(Tail, pNewCmd);
    }
  else
    {
    Cmds.AddTail(pNewCmd);
    Cmds.AddTail(new CCmdEOC());
    }

  if (Cmds.GetCount()==2)
    {
    MainWnd()->UpdateStatusBar();
    }
  char Buff[16];
  sprintf(Buff, "%d:Cmds", Max(0, Cmds.GetCount()-1));
  pStatusBar->UpdateIndicator(StatusBarCmdIndicator, Buff, TRUE);

  return pNewCmd;
  }

//---------------------------------------------------------------------------

int CCmdExec::AddCmd(char* Txt)
  {
  if (Txt==NULL || strlen(Txt)==0)
    return 1;
  int Quote;
  CSVColArray f;
  int nParms = ParseCSVFunction(Txt, f, Quote) - 1;
  if (Quote)
    return 2;
  if (_stricmp(f[0], CmdNames(ComCmd_LoadProject))==0 && nParms==1)
    AddCmd(new CCmdLoadProject(StrQuotesTrim(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_SaveProject))==0 && nParms==0)
    AddCmd(new CCmdDirect(ComCmd_SaveProject, ComCmd_SaveProject, ID_PROJECT_SAVEALL, CmdFlagStopped | CmdFlagDoNotExecNext));
  else if (_stricmp(f[0], CmdNames(ComCmd_CloseProject))==0 && nParms==0)
    AddCmd(new CCmdCloseProject());
  else if (_stricmp(f[0], CmdNames(ComCmd_LoadSnapshot))==0 && nParms==1)
    AddCmd(new CCmdLoadSnapshot(StrQuotesTrim(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_SaveSnapshot))==0 && nParms==1)
    AddCmd(new CCmdSaveSnapshot(StrQuotesTrim(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_LoadScenario))==0 && nParms==1)
    AddCmd(new CCmdLoadScenario(StrQuotesTrim(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_SaveScenario))==0 && nParms==1)
    AddCmd(new CCmdSaveScenario(StrQuotesTrim(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_StartDynamicSolve))==0 && nParms==0)
    AddCmd(new CCmdSolveDyn(ComCmd_StartDynamicSolve, ComState_StepDynamic, ID_ACTIONS_RUNDYN, /*CmdFlagStopIdle |*/ CmdFlagDoNotExecNext));
  else if (_stricmp(f[0], CmdNames(ComCmd_RunToSteadyState))==0 && nParms==0)
    AddCmd(new CCmdSolveDyn(ComCmd_RunToSteadyState, ComState_StepDynamic, ID_ACTIONS_RUN_STEADY, /*CmdFlagStopIdle |*/ CmdFlagDoNotExecNext));
  else if (_stricmp(f[0], CmdNames(ComCmd_StartProbalSolve))==0 && nParms==0)
    AddCmd(new CCmdSolvePB(ComCmd_StartProbalSolve, ComState_StepProbal, ID_ACTIONS_RUNPB, /*CmdFlagStopIdle |*/ CmdFlagDoNotExecNext));
  //else if (_stricmp(f[0], CmdNames(ComCmd_Start))==0 && nParms==0)
  //  AddCmd(new CCmdSolve(ComCmd_Start, ComState_??, ID_ACTIONS_RUN, CmdFlagStopIdle | CmdFlagDoNotExecNext));
  else if (_stricmp(f[0], CmdNames(ComCmd_Idle))==0 && nParms==0)
    AddCmd(new CCmdDirect(ComCmd_Idle, ComState_Idle, ID_ACTIONS_IDLE, CmdFlagStopRun | CmdFlagDoNotExecNext));
  else if (_stricmp(f[0], CmdNames(ComCmd_Stop))==0 && nParms==0)
    AddCmd(new CCmdDirect(ComCmd_Stop, ComState_Stop, ID_ACTIONS_EDIT, CmdFlagIdleRun | CmdFlagDoNotExecNext));
  else if (_stricmp(f[0], CmdNames(ComCmd_Wait))==0 && nParms==1)
    AddCmd(new CCmdWait(atof(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_WaitCount))==0 && nParms==1)
    AddCmd(new CCmdWaitCount(atol(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_WaitTillStop))==0 && nParms==0)
    AddCmd(new CCmdWaitTillStop());
  else if (_stricmp(f[0], CmdNames(ComCmd_WaitTillSteadyState))==0 && nParms==0)
    AddCmd(new CCmdWaitTillSteadyState());
  else if (_stricmp(f[0], CmdNames(ComCmd_ResetWait))==0 && nParms==0)
    AddCmd(new CCmdResetWait());
  else if (_stricmp(f[0], CmdNames(ComCmd_Report_GetValues))==0 && nParms==2)
    AddCmd(new CCmdGenerateReport(SUB_REPORT_GETVALUES, StrQuotesTrim(f[1]), StrQuotesTrim(f[2])));
  else if (_stricmp(f[0], CmdNames(ComCmd_Report_Trend))==0 && nParms==2)
    AddCmd(new CCmdGenerateReport(SUB_REPORT_TREND, StrQuotesTrim(f[1]), StrQuotesTrim(f[2])));
  else if (_stricmp(f[0], CmdNames(ComCmd_Report_SetValues))==0 && nParms==2)
    AddCmd(new CCmdGenerateReport(SUB_REPORT_SETVALUES, StrQuotesTrim(f[1]), StrQuotesTrim(f[2])));
  else if (_stricmp(f[0], CmdNames(ComCmd_Report_SaveAndClose))==0 && nParms==1)
    AddCmd(new CCmdGenerateReport(SUB_REPORT_SAVEANDCLOSE, StrQuotesTrim(f[1]), ""));
  else if (_stricmp(f[0], CmdNames(ComCmd_Report_ExecuteMacro))==0 && nParms==2)
    AddCmd(new CCmdGenerateReport(SUB_REPORT_MACRO, StrQuotesTrim(f[1]), StrQuotesTrim(f[2])));
  else if (_stricmp(f[0], CmdNames(ComCmd_SetTag))==0 && nParms==2)
    AddCmd(new CCmdSetTag(StrQuotesTrim(f[1]), f[2][0]=='*' ? dNAN : atof(f[2])));
  else if (_stricmp(f[0], CmdNames(ComCmd_RestartHistorian))==0 && nParms==0)
    AddCmd(new CCmdRestartHistorian());
  else if (_stricmp(f[0], CmdNames(ComCmd_CompareOptions))==0 && nParms==2)
    AddCmd(new CCmdCompareOptions(StrQuotesTrim(f[1]), (atoi(f[2])!=0)));
  else if (_stricmp(f[0], CmdNames(ComCmd_CompareOptions2))==0 && nParms==5)
    AddCmd(new CCmdCompareOptions2(atol(f[1]), atof(f[2]), atof(f[3]), (atoi(f[4])!=0), (atoi(f[5])!=0)));
  else if (_stricmp(f[0], CmdNames(ComCmd_CompareReset))==0 && nParms==0)
    AddCmd(new CCmdCompareReset());
  else if (_stricmp(f[0], CmdNames(ComCmd_CompareScenarios))==0 && nParms==2)
    AddCmd(new CCmdCompareScenarios(False, StrQuotesTrim(f[1]), StrQuotesTrim(f[2])));
  else if (_stricmp(f[0], CmdNames(ComCmd_CompareScenarioToCurrent))==0 && nParms==1)
    AddCmd(new CCmdCompareScenarios(True, StrQuotesTrim(f[1]), ""));
  else if (_stricmp(f[0], CmdNames(ComCmd_TestScenario))==0 && nParms==1)
    AddCmd(new CCmdTestScenario(StrQuotesTrim(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_Sleep))==0 && nParms==1)
    AddCmd(new CCmdSleep(atol(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_Exit))==0 && nParms==0)
    AddCmd(new CCmdExit(0));
  else if (_stricmp(f[0], CmdNames(ComCmd_Exit))==0 && nParms==1)
    AddCmd(new CCmdExit(atol(f[1])));
  else if (_stricmp(f[0], CmdNames(ComCmd_EOC))==0 && nParms==0)
    AddCmd(new CCmdEOC());
  else
    return 3;
  return 0;
  }

//---------------------------------------------------------------------------

int CCmdExec::AddCmds(char* pFileName)
  {
  flag Err = 0;
  CStdioFile f;
  TRY
    {
    if (f.Open(pFileName, CFile::modeRead | CFile::typeText))
      {
      CString s;
      char Line[1024];
      int LineNo = 1;
      while (f.ReadString(s))
        {
        if (s.GetLength()>0)
          {
          strncpy(Line, (const char*)s, sizeof(Line));
          XStrTrim(Line, " \t\n\r");
          if (strlen(Line)>0 && Line[0]!=';' && AddCmd(Line)>0)
            {
            LogWarning("Command", 0, "Error on line %d for script file '%s'", LineNo, pFileName);
            LogWarning("Command", 0, "Check command or parameters for '%s'", (const char*)s);
            Err = 1;
            }
          }
        LineNo++;
        }
      //AddCmd("EOC");

      f.Close();
      }
    else
      {
      LogError("Command", LF_Exclamation, "Cannot open script command file '%s'", pFileName);
      return 1;
      }
    }
  CATCH (CFileException, e)
    {
    LogError("Command", LF_Exclamation, "File error %d reading script command file '%s'", e->m_cause, pFileName);
    return 2;
    }
  END_CATCH
  if (Err)
    {
    LogError("Command", LF_Exclamation, "Errors encountered while interpreting script file '%s'", pFileName);
    DelCmds();
    return 3;
    }

  return 0;
  }

//---------------------------------------------------------------------------

void CCmdExec::DelCmds()
  {
  while (Cmds.GetCount())
    delete Cmds.RemoveHead();
  if (pInfoWnd)
    pInfoWnd->DestroyWindow();
  MainWnd()->UpdateStatusBar();
  Reset();
  }

//---------------------------------------------------------------------------

void CCmdExec::AbortScripts()
  {
  DelCmds();
  LogNote("Command", 0, "Command script terminated by user");
  }

//---------------------------------------------------------------------------

int CCmdExec::ExecCmds()
  {
  MEMCHECKS()

  if (Cmds.IsEmpty())
    {
    //dbgpln("ExecCmds <EMPTY>");
    wCmdWaiting = 0;
    return -1; //no commands waiting to be executed
    }
  CCmdBase* pCmd = Cmds.IsEmpty() ? NULL : Cmds.GetHead();

  //dbgpln("ExecCmds <%2i>", pCmd->CmdID);

  if ((pCmd->flags & CmdFlagExecuteWait) || (pCmd->flags & CmdFlagStopWait))
    return -2; //command must not be executed until certain conditions are met
  if (wCmdWaiting!=0)
    return -3; //still waiting for previous command to be completed
  flag DoNotExecNext = 0;
  char Buff[1024];
  int err = 0;
  if ((gs_pPrj==NULL || gs_pPrj->pPrjDoc==NULL) && (pCmd->flags & CmdPrjLoaded))
    {
    LogError("Command", LF_Exclamation, "Command script terminated, %s cannot be exucuted when a project is not loaded", pCmd->Desc(Buff));
    DelCmds();
    }
  else if ((pCmd->flags & CmdFlagStopped) && gs_TheRunMngr.Running())
    {//Executive has not stopped yet - give it some time...
    //pStatusBar->SetMsg("Wait (%d) for %s", iCmdAttempts, pFirstCmd->Desc(Buff));
    if (iCmdAttempts>5)
      {
      LogError("Command", LF_Exclamation, "Command script terminated, command %s cannot be exucuted while SysCAD is running", pCmd->Desc(Buff));
      DelCmds();
      }
    else
      Sleep(++iCmdAttempts * 50);
    }
  else if ((pCmd->flags & CmdFlagRunning) && gs_TheRunMngr.Stopped())
    {
    //pStatusBar->SetMsg("Wait (%d) for %s", iCmdAttempts, pFirstCmd->Desc(Buff));
    if (iCmdAttempts>5)
      {
      LogError("Command", LF_Exclamation, "Command script terminated, command %s cannot be exucuted while SysCAD is stopped", pCmd->Desc(Buff));
      DelCmds();
      }
    else
      Sleep(++iCmdAttempts * 50);
    }
  else if ((pCmd->flags & CmdFlagStopIdle) && gs_TheRunMngr.Running())
    {
    //pStatusBar->SetMsg("Wait (%d) for %s", iCmdAttempts, pFirstCmd->Desc(Buff));
    if (iCmdAttempts>5)
      {
      LogError("Command", LF_Exclamation, "Command script terminated, command %s cannot be exucuted while SysCAD is stopped or idling", pCmd->Desc(Buff));
      DelCmds();
      }
    else
      Sleep(++iCmdAttempts * 50);
    }
  else if ((pCmd->flags & CmdFlagStopRun) && gs_TheRunMngr.Idling())
    {
    //pStatusBar->SetMsg("Wait (%d) for %s", iCmdAttempts, pFirstCmd->Desc(Buff));
    if (iCmdAttempts>5)
      {
      LogError("Command", LF_Exclamation, "Command script terminated, command %s cannot be exucuted while SysCAD is stopped or running", pCmd->Desc(Buff));
      DelCmds();
      }
    else
      Sleep(++iCmdAttempts * 50);
    }
  else if ((pCmd->flags & CmdFlagIdleRun) && gs_TheRunMngr.Stopped())
    {
    //pStatusBar->SetMsg("Wait (%d) for %s", iCmdAttempts, pFirstCmd->Desc(Buff));
    if (iCmdAttempts>5)
      {
      LogError("Command", LF_Exclamation, "Command script terminated, command %s cannot be exucuted while SysCAD is stopped or idling", pCmd->Desc(Buff));
      DelCmds();
      }
    else
      Sleep(++iCmdAttempts * 50);
    }
  else
    {//execute the command...
    MEMCHECKS()

    if (pInfoWnd==NULL)
      pInfoWnd = new CScriptInfo(AfxGetMainWnd());

    char Buff1[1024];
    Buff1[0]=0;

    pCmd->Desc(Buff);
    CCmdBase* pCmd1 = NULL;
    if (Cmds.GetCount()>=2)
      {
      POSITION pos=Cmds.GetHeadPosition();
      Cmds.GetNext(pos);
      pCmd1=Cmds.GetNext(pos);
      pCmd1->Desc(Buff1);
      }

    if (strcmp(Buff, "EOC()")==0)
      Buff[0]=0;
    if (strcmp(Buff1, "EOC()")==0)
      Buff1[0]=0;
    pInfoWnd->SetInfo(Cmds.GetCount(), Buff, Buff1);

//    if (pCmd->pNext)
//      {
//      char Buff1[1024];
//      pInfoWnd->SetInfo(Cmds.GetCount(), pCmd->Desc(Buff), pFirstCmd->pNext->Desc(Buff1));
//      }
//    else
//      pInfoWnd->SetInfo(Cmds.GetCount(), pCmd->Desc(Buff), "");

    pStatusBar->SetMsg("Execute script command %s", Buff);
    LogNote("Command", 0, "Execute %s", Buff);
    dbgpln("#### Execute Script command %s ####", Buff);
    DoNotExecNext = ((pCmd->flags & CmdFlagDoNotExecNext)!=0);
    bDoNotPrompt = ((pCmd->flags & CmdFlagDoNotPrompt)!=0);
    bSaveOnExit = ((pCmd->flags & CmdFlagSaveOnExit)!=0);
    bNoSaveOnExit = ((pCmd->flags & CmdFlagNoSaveOnExit)!=0);
    iCmdAttempts = 0;

    Sleep(200); // CNM  Seems to miss commands sometimes.

    MEMCHECKS()

    err = pCmd->ExecCmd(this);

    MEMCHECKS()

    if (err<0)
      {//want to try the same command again
      DoNotExecNext = 0;
      }
    else
      {
      if (DoNotExecNext)
        wCmdWaiting = pCmd->CmdDoneID; //CNM CmdID;
      if (err>0)
        LogError("Command", LF_Exclamation, "Command script terminated due to a critical error for %s", pCmd->Desc(Buff));

      if (!Cmds.IsEmpty())
        Cmds.RemoveHead();
      //CCmdBase* pDelCmd = pFirstCmd;
      //pFirstCmd = pFirstCmd->pNext;
      //delete pDelCmd;
      //iCmdCount--;
      if (err>0)
        DelCmds();
      }
    if (Cmds.GetCount()==0)
      {
      MainWnd()->UpdateStatusBar();
      if (pInfoWnd)
        pInfoWnd->DestroyWindow();
      }
    else
      {
      char Buff[16];
      sprintf(Buff, "%d:Cmds", Max(0,Cmds.GetCount()-1));
      pStatusBar->UpdateIndicator(StatusBarCmdIndicator, Buff, TRUE);
      }
    MEMCHECKS()
    }
  if (Cmds.GetCount())//pFirstCmd)
    {
    if (!DoNotExecNext)
      ScdMainWnd()->PostMessage(WMU_CMDNEXT, 0, 0); //post message to execute next command immediatly
    }
  else
    {
    bDoNotPrompt = 0;
    //pLastCmd = NULL;
    wCmdWaiting = 0;
    }
  MEMCHECKS()
  return err;
  }

//---------------------------------------------------------------------------

int CCmdExec::CmdComplete(WORD CmdID)
  {
  //dbgpln("CmdComplete <%2i %2i>", CmdID, wCmdWaiting);
  if (wCmdWaiting!=0 && CmdID==wCmdWaiting)
    {//command is complete, can execute next command
    wCmdWaiting = 0;
    bDoNotPrompt = 0;
    //dbgpln("CmdComplete <Done>");
    ScdMainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0); //post message to execute next command
    return 0;
    }
  return -1;
  }

//---------------------------------------------------------------------------

DWORD CCmdExec::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  DWORD RetCode = 0;
  while (XM.MsgAvail())
    switch (XM.MsgId())
      {
      case XM_QueryString:
        {
        /*pXB_QueryString p = XM.QueryString();
        if (p->cValue && strlen(p->cValue)==6 && _stricmp(p->cValue, "TheEnd")==0)
          {
          COleReportTrendMngr* pM = (COleReportTrendMngr*)(p->iSrcID);
          pM->Lock();
          pM->bQueryDone = 1;
          pM->Unlock();
          }*/
        RetCode = 1;
        break;
        }
      default:
        ASSERT(0);
      }
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_Start(CXM_TimeControl &CB)
  {
  dStartTime = CB.m_TheTime;
  dwIterCount = 0;
  return True;
  }

//---------------------------------------------------------------------------

void CCmdExec::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  XM.Clear();
  }

//---------------------------------------------------------------------------

void CCmdExec::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  //XM.Clear();
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_ReadSubsData(CXMsgLst &XM)
  {
  flag DataRead = 0;
  return DataRead;
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  while (XM.MsgAvail())
    {
    CXM_ObjectData *pX = XM.ObjectData();
    CPkDataItem * pPItem = pX->FirstItem();
    }
  return True;
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  dwIterCount++;
  dCurTime = CB.m_TheTime;
  CCmdBase* pCmd = Cmds.IsEmpty() ? NULL : Cmds.GetHead();
  if (pCmd && pCmd->flags & CmdFlagExecuteWait)
    {
    if (pCmd->ExecNow(this))
      {
      pCmd->flags &= (~CmdFlagExecuteWait);
      ScdMainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_Stop(CXM_TimeControl &CB)
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CCmdExec::EO_Stopping(flag fBeginStopping)
  {
  CCmdBase* pCmd = Cmds.IsEmpty() ? NULL : Cmds.GetHead();
  if ( !fBeginStopping && pCmd &&
       ((pCmd->flags & CmdFlagStopWait) || (pCmd->flags & CmdFlagExecuteWait)) )
    {
    if (pCmd->flags & CmdFlagExecuteWait)
      {
      char Buff[1024];
      LogError("Command", LF_Exclamation, "Command script terminated, %s cannot be exucuted when a project has just been stopped/solved.", pCmd->Desc(Buff));
      DelCmds();
      }
    if (pCmd && pCmd->ExecNow(this))
      {
      pCmd->flags &= (~CmdFlagStopWait);
      ScdMainWnd()->PostMessage(WMU_CMDNEXT, ComCmd_ExecNextCmd, 0);
      }
    }
  return True;
  }

//---------------------------------------------------------------------------

int CCmdExec::EO_CanClose(Strng_List & Problems)
  {
  /*if (pOleThread->IsBusy())
    {
    pOleThread->CancelQueueItems();
    //should these rather be added to the problems list???
    LogWarning("OLE", 0, "Extra OLE requests have been canceled.");
    LogError("OLE", 0, "OLE requests are still being serviced.");
    return EO_CanClose_Wait;
    }*/
  return EO_CanClose_Yes;
  }

//===========================================================================
//
//
//
//===========================================================================

class CFlagSetReset
  {
  protected:
    flag & m_Flag;
  public:
    CFlagSetReset(flag & Flag) : m_Flag(Flag) { m_Flag=true; gs_MsgLog.ComCallStart(); };
    ~CFlagSetReset() { m_Flag=false; gs_MsgLog.ComCallEnd(); };
  };

LRESULT CCmdExec::DoCOMCmd(long From, long Cmd, CScdCOCmdBlk * pCmdBlk, long Data)
  {
  CFlagSetReset Flg(bCOMCmdBusy);

  LRESULT lRet=0;
  DWORD ThisCmd=Cmd;
//#if WITHCOMCMD
//  ThisCmd=(Cmd& 0xff);
#if dbgScdCmdIF
if (dbgDoComCmd())
  dbgpln("@@@------------> Cmd %s", ScdCOMCmd2Str(ThisCmd));
#endif
  switch (ThisCmd)
    {
    //NOT MESSAGED case ComCmd_get_UpNAbout : break;
    //NOT MESSAGED case ComCmd_WaitUpNAbout : break;
    case ComCmd_CreateProject                   :
      {
      CScdCmdParms *pP=(CScdCmdParms *)Data;
      ASSERT(gs_pPrj);
      gs_pPrj->SetNewPrjCfg(pP->m_sParms[0]);
      gs_pPrj->SetNewPrjPath(pP->m_sParms[1]);
      gs_pPrj->SetNewPrjOptions(pP->m_bParms[0], pP->m_bParms[1]);
      //if ((EnableNoPrj() || (EnableNotBusy() && EnableNotAnalysing())) && EnableNotFiling() && !gs_License.Blocked())
      CDocument* pDoc = ScdApp()->ProjectTemplate().OpenDocumentFile(NULL);
      delete pCmdBlk;
      gs_Events.Send(ComCmd_CreateProject, /*pCmdBlk,*/ pDoc ? 0 :1);
      if (pDoc==NULL)
        LogError("COMCmd", 0, "Unable to Create %s", (LPCTSTR)pP->m_sParms[1]);
      delete pP;
      break;
      }
    case ComCmd_LoadProject:
      {
      LPTSTR pNm=(LPTSTR)Data;
      //CDocument* pPrevPrjDoc = (gs_pPrj ? gs_pPrj->pPrjDoc : NULL);
      CDocument* pDoc = ScdApp()->OpenDocumentFile(pNm);
      delete pCmdBlk;
      gs_Events.Send(ComCmd_LoadProject, /*pCmdBlk,*/ pDoc ? 0 :1);
      if (pDoc==NULL)
        LogError("COMCmd", 0, "Unable to open %s", pNm);
      delete[] pNm;
      break;
      }
    case ComCmd_SaveProject:
    case ComCmd_SaveProjectNV:
      {
      LPTSTR pNm=(LPTSTR)Data;
      gs_pPrj->bDoingSaveAsNV=ThisCmd==ComCmd_SaveProjectNV;
      if (pNm==NULL || strlen(pNm)==0)
        gs_pPrj->pPrjDoc->OnSaveDocument(PrjFile());
      else
        {
        gs_pPrj->bDoingSaveAs=true;
        gs_pPrj->bDoingSaveAsWithName = pNm && strlen(pNm)>0;
        gs_pPrj->pPrjDoc->OnSaveDocument(pNm);
        gs_pPrj->bDoingSaveAs=false;
        gs_pPrj->bDoingSaveAsWithName = false;
        }
      gs_pPrj->bDoingSaveAsNV=false;

      delete pCmdBlk;
      gs_Events.Send(ThisCmd, /*pCmdBlk,*/ 0);
      //if (pDoc==NULL)
      //  LogError("COMCmd", 0, "Unable to open %s", pNm);
      delete[] pNm;
      break;
      }
    case ComCmd_ExportNeutralDB:
      {
      CScdCmdImportExport *pCmd=(CScdCmdImportExport*)Data;

      CNeutralImportExport NExport;
      bool Failed=!NExport.DoExport(pCmd->m_iOptions, pCmd->m_pGraphicsDatabase, pCmd->m_pModelDatabase);

      gs_Events.Send(ComCmd_ExportNeutralDB, Failed? 1:0);

      if (Failed)
        LogError("COMCmd", 0, "Neutral Database import %s:%s failed!", pCmd->m_pGraphicsDatabase, pCmd->m_pModelDatabase);
      break;
      }

    case ComCmd_ImportNeutralDB:
      {
      CScdCmdImportExport *pCmd=(CScdCmdImportExport*)Data;

      CNeutralImportExport NImport;
      NImport.SetImportTagFixups(pCmd->m_iRule, pCmd->m_pPrefix, pCmd->m_pSuffix, NULL, NULL);
      bool Failed=!NImport.DoImportDB(pCmd->m_iOptions, pCmd->m_pGraphicsDatabase, pCmd->m_pModelDatabase);

      gs_Events.Send(ComCmd_ImportNeutralDB, Failed? 1:0);

      if (Failed)
        LogError("COMCmd", 0, "Neutral Database import %s:%s failed!", pCmd->m_pGraphicsDatabase, pCmd->m_pModelDatabase);
      break;
      }
    case ComCmd_LoadSnapshot :
    case ComCmd_LoadScenario:
      {
      LPTSTR pNm=(LPTSTR)Data;
      long Res=gs_pPrj->DoLoadSnapShotScenario(ThisCmd==ComCmd_LoadSnapshot, pNm)?0:1;
      delete pCmdBlk;
      gs_Events.Send(ThisCmd, /*pCmdBlk,*/ Res);
      };
      break;
    case ComCmd_SaveSnapshot:
    case ComCmd_SaveScenario:
      {
      LPTSTR pNm=(LPTSTR)Data;
      long Res=gs_pPrj->DoSaveSnapShotScenario(ThisCmd==ComCmd_SaveSnapshot, pNm)?0:1;
      delete pCmdBlk;
      gs_Events.Send(ThisCmd, /*pCmdBlk,*/ Res);
      };
      break;
    case ComCmd_LoadBacktrack :
      {
      LPTSTR pNm=(LPTSTR)Data;
      long Res=1;//gs_pPrj->DoLoadBacktrack(ThisCmd==ComCmd_LoadBacktrack, pNm)?0:1;
      delete pCmdBlk;
      gs_Events.Send(ThisCmd, /*pCmdBlk,*/ Res);
      };
      break;
    case ComCmd_SaveBacktrack:
      {
      LPTSTR pNm=(LPTSTR)Data;
      long Res=1;//gs_pPrj->DoSaveBacktrack(ThisCmd==ComCmd_SaveBacktrack, pNm)?0:1;
      delete pCmdBlk;
      gs_Events.Send(ThisCmd, /*pCmdBlk,*/ Res);
      };
      break;
    case ComCmd_CloseProject                  :
      {
      int iTryCnt=0;
      delete pCmdBlk;
      while (lRet==0)
        {
        Strng_List Problems;
        switch (gs_Exec.CanClose(Problems))
          {
          case EO_CanClose_Save:
            AfxGetMainWnd()->SendMessage(WM_COMMAND, Data ? ID_PROJECT_CLOSE:ID_PROJECT_CLOSE_NOSAVE, 0);
            gs_Events.Send(ComCmd_CloseProject, /*pCmdBlk,*/ 0);
            break;
          case EO_CanClose_Yes:
            AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_CLOSE, 0);
            gs_Events.Send(ComCmd_CloseProject, /*pCmdBlk,*/ 0);
            return 0;
          case EO_CanClose_Wait:
            if (iTryCnt++<6)
              Sleep(250 + (iTryCnt*250)); //try give executive objects time to finish, so that project can be closed
            else
              {
              gs_Events.Send(ComCmd_CloseProject, /*pCmdBlk,*/ 1);
              lRet=-1;
              }
            break;
          case EO_CanClose_No:
            if (iTryCnt++<1)
              Sleep(750); //try ONCE to give executive objects time to finish, so that project can be closed
            else
              {
              gs_Events.Send(ComCmd_CloseProject, /*pCmdBlk,*/ 2);
              lRet=-2;
              }
            break;
          }
        }

      LogError("Command", 0, "Cannot close project");
      break;
      }
    case ComCmd_SetTag :
      {
      lRet=ComCmdRet_OK;
      CScdCmdTag &T=*((CScdCmdTag*)Data);

      Strng WrkTag, WrkCnvTxt;
      TaggedObject::SplitTagCnv((char*)T.m_pTag, WrkTag, WrkCnvTxt);
      flag UseCnv = (WrkCnvTxt.Length()>0);
      CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0)|TABOpt_StrListOnce);
      CXM_ObjectData ObjData;
      CXM_Route Route;
      if (XReadTaggedItem(ObjTag, ObjData, Route))
        {
        CPkDataItem * pItem = ObjData.FirstItem();
        const byte cType = pItem->Type();
        if (IsNumData(cType) && UseCnv)
          {//check cnv
          if ((pItem->CnvIndex())==0 || Cnvs[pItem->CnvIndex()]->Find(WrkCnvTxt())==NULL)
            {
            UseCnv = false;
            LogWarning("COM Cmd", 0, "SetTag: Valid conversion expected %s", WrkTag());
            lRet=2;
            }
          }
        else
          {
          //LogWarning("Command", 0, "SetTag: Valid tag expected (%s)", WrkTag());
          //return 1;
          }

        PkDataUnion DU;
        VARIANT R;
        ::VariantInit(&R);
        if (IsFloatData(cType))
          {
          int PutIt=false;
          int ConvIt=true;
          if (T.m_pvtValue->vt==VT_BSTR)
            {
            USES_CONVERSION;
            LPCTSTR S=OLE2T(T.m_pvtValue->bstrVal);
            if (S[0]=='*')
              {
              R.vt=VT_R8;
              R.dblVal=dNAN;
              PutIt=true;
              ConvIt=false;
              UseCnv=false;
              }
            }
          if (ConvIt)
            PutIt=SUCCEEDED(::VariantChangeType(&R, T.m_pvtValue, 0, VT_R8));

          if (PutIt)
            {
            if (UseCnv)
              DU.SetTypeDouble(cType, R.dblVal, pItem->CnvIndex(), WrkCnvTxt());
            else
              DU.SetTypeDouble(cType, R.dblVal);
            }
          else
            {
            LogWarning("COM Cmd", 0, "SetTag: Type change failed (%s)", WrkTag());
            lRet=3;
            }
          }
        else if (IsNumData(cType))
          {
          if (!UseCnv && (T.m_pvtValue->vt==VT_BSTR))
            {
            USES_CONVERSION;
            LPCTSTR S=OLE2T(T.m_pvtValue->bstrVal);
            DU.SetTypeString(tt_Generic, (LPTSTR)S);
            }
          else if (SUCCEEDED(::VariantChangeType(&R, T.m_pvtValue, 0, VT_I4)))
            {
            if (UseCnv)
              DU.SetTypeLong(cType, R.lVal, pItem->CnvIndex(), WrkCnvTxt());
            else
              DU.SetTypeLong(cType, R.lVal);
            }
          else
            {
            LogWarning("COM Cmd", 0, "SetTag: Type change failed (%s)", WrkTag());
            lRet=4;
            }
          }
        else
          {
          USES_CONVERSION;
          if (SUCCEEDED(::VariantChangeType(&R, T.m_pvtValue, 0, VT_BSTR)))
            DU.SetTypeString(cType, OLE2T(R.bstrVal));
          else
            {
            LogWarning("COM Cmd", 0, "SetTag: Type change failed (%s)", WrkTag());
            lRet=5;
            }
          }

        CXM_ObjectData ObjData(0, 0, WrkTag(), 0, DU);
        if ((lRet==0) && (XWriteTaggedItem(ObjData, Route)!=TOData_OK))
          {
          LogWarning("COM Cmd", 0, "SetTag: Write failed %s", WrkTag());
          lRet=6;
          }
        else
          {
          if (IsFloatData(cType))
            gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_COM, WrkTag(), R.dblVal, UseCnv ? WrkCnvTxt() : "");
          else if (IsNumData(cType))
            gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_COM, WrkTag(), R.lVal, UseCnv ? WrkCnvTxt() : "");
          else
            {
            USES_CONVERSION;
            gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_COM, WrkTag(), OLE2T(R.bstrVal), "");
            }
          }
        }
      else
        {
        LogWarning("Command", 0, "SetTag: Valid tag expected (%s)", WrkTag());
        lRet=1;
        }

      delete pCmdBlk;
      gs_Events.Send(ComCmd_SetTag, /*pCmdBlk,*/ lRet);
      break;
      }
    case ComCmd_GetTag :
      {
      lRet=lRet;
      CScdCmdTag &T=*((CScdCmdTag*)Data);

      Strng WrkTag, WrkCnvTxt;
      TaggedObject::SplitTagCnv((char*)T.m_pTag, WrkTag, WrkCnvTxt);
      flag UseCnv = (WrkCnvTxt.Length()>0);
      CXM_ObjectTag ObjTag(WrkTag(), (UseCnv ? TABOpt_ValCnvsOnce : 0));
      CXM_ObjectData ObjData;
      CXM_Route Route;
      if (XReadTaggedItem(ObjTag, ObjData, Route))
        {
        CPkDataItem * pItem = ObjData.FirstItem();
        if (IsNumData(pItem->Type()))
          {
          if (UseCnv && Cnvs[pItem->CnvIndex()]->Find(WrkCnvTxt())==NULL)
            {
            UseCnv = 0;
            LogWarning("COM Cmd", 0, "GetTag: Valid conversion expected %s", WrkTag());
            lRet=ComCmdRet_InvalidConversion;
            }
          }
        else
          {
          //LogWarning("Command", 0, "GetTag: Valid tag expected (%s)", WrkTag());
          //return 1;
          }

        PkDataUnion &DU=*pItem->Value();
        VARIANT R;
        ::VariantInit(&R);
        if (IsFloatData(pItem->Type()))
          {
          T.m_pvtValue->vt=VT_R8;
          if (UseCnv)
            T.m_pvtValue->dblVal=DU.GetDouble(pItem->CnvIndex(), WrkCnvTxt());
          else
            T.m_pvtValue->dblVal=DU.GetDouble();
          }
        else if (IsNumData(pItem->Type()))
          {
          T.m_pvtValue->vt=VT_I4;
          if (UseCnv)
            T.m_pvtValue->lVal=DU.GetLong(pItem->CnvIndex(), WrkCnvTxt());
          else
            T.m_pvtValue->lVal=DU.GetLong();
          }
        else
          {
          T.m_pvtValue->vt=VT_BSTR;
          T.m_pvtValue->bstrVal=A2BSTR(DU.GetString());
          //T.Value->bstrVal=T2OLE(DU.GetString());
          }
        }
      else
        {
        LogWarning("Command", 0, "GetTag: Valid tag expected (%s)", WrkTag());
        lRet=ComCmdRet_InvalidTag;
        }

      delete pCmdBlk;
      gs_Events.Send(ComCmd_GetTag, /*pCmdBlk,*/ lRet);
      break;
      }
    case ComCmd_get_TagType :
      {
      lRet=ComCmdRet_OK;
      CScdCmdTag &T=*((CScdCmdTag*)Data);
      T.m_pvtValue->vt=VT_I4;

      Strng WrkTag, WrkCnvTxt;
      TaggedObject::SplitTagCnv((char*)T.m_pTag, WrkTag, WrkCnvTxt);
      //flag UseCnv = (WrkCnvTxt.Length()>0);
      CXM_ObjectTag ObjTag(WrkTag(), 0);//(UseCnv ? TABOpt_ValCnvsOnce : 0));
      CXM_ObjectData ObjData;
      CXM_Route Route;
      if (XReadTaggedItem(ObjTag, ObjData, Route))
        {
        CPkDataItem * pItem = ObjData.FirstItem();
        T.m_pvtValue->lVal=pItem->Type();
        }
      else
        {
        lRet=ComCmdRet_InvalidTag;
        T.m_pvtValue->lVal=-1; //unknown or illegal tag
        }

      delete pCmdBlk;
      gs_Events.Send(ComCmd_get_TagType, /*pCmdBlk,*/ lRet);
      break;
      }
    case ComCmd_get_TagRecording:
      {
      lRet=ComCmdRet_OK;
      CScdCmdTag &T=*((CScdCmdTag*)Data);
      *T.m_pLong=gs_pPrj->get_RecordTag(T.m_pTag);
      delete pCmdBlk;
      gs_Events.Send(ComCmd_get_TagRecording, /*pCmdBlk,*/ lRet);
      break;
      }
    case ComCmd_put_TagRecording:
      {
      lRet=ComCmdRet_OK;
      CScdCmdTag &T=*((CScdCmdTag*)Data);
      DWORD Ret=0;
      gs_pPrj->put_RecordTag(T.m_pTag, ((T.m_lLong&RC_SM_HistRecOn)!=0), Ret);
      delete pCmdBlk;
      gs_Events.Send(ComCmd_put_TagRecording, /*pCmdBlk,*/ lRet);
      break;
      }
    case ComCmd_StartDynamicSolve:
      gs_TheRunMngr.GotoRunMode(CExecutive::RRM_DynMd, -1);
      break;
    case ComCmd_RunToSteadyState:
      gs_TheRunMngr.GotoRunMode(CExecutive::RRM_DynMd, 1);
      break;
    case ComCmd_StartProbalSolve :
      gs_TheRunMngr.GotoRunMode(CExecutive::RRM_PBMd, -1);
      break;
    case ComCmd_Start:
      gs_TheRunMngr.GotoRunMode(CExecutive::RRM_Current, -1);
      break;
    case ComCmd_Step:
      CDoOneStepInfo Info;
      Info.lParam=Data;
      dbgpln("ComCmd_Step %i  %i  %i", Cmd, Info.bHoldAdv, Info.lWaitForNextMS);
      gs_TheRunMngr.DoStep(Info);
      break;
    case ComCmd_Exit :
      bSaveOnExit = false;
      bNoSaveOnExit = true;
      LclPostMessage(WM_COMMAND, ID_APP_EXIT, 0);
      break;
    case ComCmd_Stop :
      //LclPostMessage(WM_COMMAND, ID_ACTIONS_EDIT, 0);
      gs_TheRunMngr.GotoEditMode();
      break;
    case ComCmd_Idle :
      //LclPostMessage(WM_COMMAND, ID_ACTIONS_IDLE, 0);
      gs_TheRunMngr.GotoIdleMode();
      break;
    case ComCmd_SetAppWndState: 
      {
      switch (Data)
        {
        case 1: 
          if (!MainWnd()->SetForegroundWindow())
            lRet=ComCmdRet_Fail;
          break;
        case 2: 
        case 3: 
        case 4: 
          {
          WINDOWPLACEMENT WP;
          MainWnd()->GetWindowPlacement(&WP);
          switch (Data)
            {
            case 2: WP.showCmd=SW_SHOWNORMAL; break; 
            case 3: WP.showCmd=SW_SHOWMAXIMIZED; break; 
            case 4: WP.showCmd=SW_SHOWMINIMIZED; break;
            }
          if (!MainWnd()->SetWindowPlacement(&WP))
            lRet=ComCmdRet_Fail;
          break;
          }
        case 5: // Activate User Buttons
        case 6: // Deactivate User Buttons
          {
          gs_TheRunMngr.m_ComUIActive=(Data==5);
          break;
          }
        }
      gs_Events.Send(ComCmd_SetAppWndState, /*pCmdBlk,*/ lRet);
      }
      break;
    case ComCmd_Wait : break;
    case ComCmd_WaitCount : break;
    case ComCmd_WaitTillSteadyState : break;
    case ComCmd_WaitTillStop : break;
    case ComCmd_ResetWait : break;
    case ComCmd_Report_GetValues:
      {
//      CXM_OleExcelReport* pXB = new CXM_OleExcelReport(sRepFilename(), sRepName(), SUB_REPORT_GETVALUES);
//      ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);

//SUB_REPORT_GETVALUES
//SUB_REPORT_TREND
//SUB_REPORT_SETVALUES
//SUB_REPORT_SAVEANDCLOSE
//SUB_REPORT_MACRO
      }
      break;
    case ComCmd_Report_Trend: break;
    case ComCmd_Report_SaveAndClose: break;
    case ComCmd_Report_SetValues: break;
    case ComCmd_get_CmpFilename : break;
    case ComCmd_put_CmpFilename : break;
    case ComCmd_get_CmpSort : break;
    case ComCmd_put_CmpSort : break;
    case ComCmd_get_CmpMaxCount : break;
    case ComCmd_put_CmpMaxCount : break;
    case ComCmd_get_CmpRelativeTolerance : break;
    case ComCmd_put_CmpRelativeTolerance : break;
    case ComCmd_get_CmpAbsoluteTolerance : break;
    case ComCmd_put_CmpAbsoluteTolerance : break;
    case ComCmd_get_CmpShowDifferentStrings : break;
    case ComCmd_put_CmpShowDifferentStrings : break;
    case ComCmd_get_CmpShowMissingTags : break;
    case ComCmd_put_CmpShowMissingTags : break;
    case ComCmd_CompareReset : break;
    case ComCmd_CompareScenarios : break;
    case ComCmd_CompareScenarioToCurrent : break;
    case ComCmd_RestartHistorian : break;

    case ComCmd_DocCmd:
      {
      CDocComInfo *pDCI=(CDocComInfo*)Data;
      switch (pDCI->m_DoWhat)
        {
        case CDocComInfo::DCI_Add:
          {
          CDocument *pDoc=ScdApp()->GraphTemplate().OpenDocumentFile(NULL);
          //pDoc->SetTitle(pDCI->m_sTitle);
          if (pDoc)
            {
            FnCheckExtension(pDCI->m_sTitle, ".scg");
            pDoc->SetPathName(pDCI->m_sTitle);
            pDCI->m_sTitle=pDoc->GetTitle();
            }
          break;
          }
        case CDocComInfo::DCI_GetCount:
          {
          pDCI->m_lCount = 0;
          POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
          while (Pos)
            {
            pDCI->m_lCount++;
            (ScdApp()->GraphTemplate().GetNextDoc(Pos));
            }
          break;
          }
        case CDocComInfo::DCI_GetTitle:
          {
          pDCI->m_sTitle="Not Found";
          POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
          while (Pos)
            {
            CDocument*pDoc=(CDocument*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
            if (pDCI->m_lIndex==0)
              {
              pDCI->m_sTitle=pDoc->GetTitle();
              break;
              }
            pDCI->m_lIndex--;
            }
          if (pDCI->m_lIndex>0)
            {
            pDCI->m_lIndex=-1;//_asm int 3; // not found;
            }
          break;
          }
        case CDocComInfo::DCI_FindTitle:
          {
          //pDCI->m_sTitle="Not Found";
          FnCheckExtension(pDCI->m_sTitle, ".scg");
          POSITION Pos = ScdApp()->GraphTemplate().GetFirstDocPosition();
          pDCI->m_lIndex=-1;
          long l=0;
          while (Pos)
            {
            CDocument*pDoc=(CDocument*)(ScdApp()->GraphTemplate().GetNextDoc(Pos));
            if (pDCI->m_sTitle.CompareNoCase(pDoc->GetTitle())==0)
              {
              pDCI->m_sTitle=pDoc->GetTitle();
              pDCI->m_lIndex=l;
              break;
              }
            l++;
            }
          //pDCI->m_sTitle="Not Found";
          break;
          }
        }
      break;
      }
    case ComCmd_HistorianDelete:
      {
      gs_pPrj->DeleteHistorian();
      break;
      }
    default:
      DoBreak();
      break;
    }
//#endif
  return lRet;//ScdApp().DoFlushMsgQ(wParam, lParam);
  }

//---------------------------------------------------------------------------

LRESULT CCmdExec::DoCOMEvt(WPARAM wParam, LPARAM lParam)
  {
//#if WITHCOMCMD
  byte Evt=(wParam & 0xff);
  switch (Evt)
    {
    //case ComCmd_get_UpNAbout : break;
    //case ComCmd_WaitUpNAbout : break;
    case ComCmd_CreateProject :
    case ComCmd_LoadProject :
    case ComCmd_SaveProject :
    case ComCmd_SaveProjectNV :
    case ComCmd_ExportNeutralDB:
    case ComCmd_ImportNeutralDB:
    case ComCmd_CloseProject :
    case ComCmd_LoadSnapshot :
    case ComCmd_SaveSnapshot :
    case ComCmd_LoadScenario :
    case ComCmd_SaveScenario :
    case ComCmd_LoadBacktrack :
    case ComCmd_SaveBacktrack :
      // Ignore
      break;
    //case ComCmd_SetTag : break;
    //case ComCmd_GetTag : break;
    case ComCmd_StartDynamicSolve :
    case ComCmd_StartProbalSolve :
    case ComCmd_Start:
    case ComCmd_Step:
    case ComCmd_Stop :
    case ComCmd_Idle :
    case ComCmd_RunToSteadyState :
      DoBreak(); // These are not expected
      break;
    //case ComCmd_Exit : break;
    //case ComCmd_Wait : break;
    //case ComCmd_WaitCount : break;
    //case ComCmd_WaitTillSteadyState : break;
    //case ComCmd_WaitTillStop : break;
    //case ComCmd_ResetWait : break;
    case ComCmd_Report_GetValues:
    case ComCmd_Report_Trend:
    case ComCmd_Report_SetValues:
    case ComCmd_Report_ExecuteMacro:
    case ComCmd_Report_SaveAndClose:
      gs_Events.Send(Evt, /*NULL,*/ 0);
      break;
    //case ComCmd_get_CmpFilename : break;
    //case ComCmd_put_CmpFilename : break;
    //case ComCmd_get_CmpSort : break;
    //case ComCmd_put_CmpSort : break;
    //case ComCmd_get_CmpMaxCount : break;
    //case ComCmd_put_CmpMaxCount : break;
    //case ComCmd_get_CmpRelativeTolerance : break;
    //case ComCmd_put_CmpRelativeTolerance : break;
    //case ComCmd_get_CmpAbsoluteTolerance : break;
    //case ComCmd_put_CmpAbsoluteTolerance : break;
    //case ComCmd_get_CmpShowDifferentStrings : break;
    //case ComCmd_put_CmpShowDifferentStrings : break;
    //case ComCmd_get_CmpShowMissingTags : break;
    //case ComCmd_put_CmpShowMissingTags : break;
    //case ComCmd_CompareReset : break;
    //case ComCmd_CompareScenarios : break;
    //case ComCmd_CompareScenarioToCurrent : break;
    //case ComCmd_RestartHistorian : break;
    case ComState_StepProbal:
    case ComState_StepDynamic:
    case ComState_Stop :
    case ComState_Idle :
      gs_Events.Send(Evt, /*NULL,*/ 0);
      break;
    case ComCmd_CompareScenarios:
    case ComCmd_CompareScenarioToCurrent:
      // Ignore
      break;
    default:
      // Unexpected
      //DoBreak();
      LogError("ComCmd", 0, "DoCOMEvt(%d,%d)  Evt:%d", wParam, lParam, Evt);
      ASSERT_ALWAYS(FALSE, "Unexpected ComCmd!", __FILE__, __LINE__);
      break;
    }
//#endif
  return 0;
  }

//===========================================================================
//
//
//
//===========================================================================
