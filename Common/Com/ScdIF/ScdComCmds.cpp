// ComCom.cpp: implementation of the CComCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#define __SCDCOMCMDS_CPP
#include "resource.h"
#include "scdcomcmds.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//=========================================================================
//
//
//
//=========================================================================

LPCTSTR ScdCOMCmd2Str(long Cmd)
  {
  switch (Cmd)
    {

    //case ComCmd_NULL                          : return "ComCmd_NULL";
    case ComCmd_get_UpNAbout                  : return "ComCmd_get_UpNAbout";
    case ComCmd_WaitUpNAbout                  : return "ComCmd_WaitUpNAbout";
    case ComCmd_get_SyncCallsOn               : return "ComCmd_get_SyncCallsOn";
    case ComCmd_put_SyncCallsOn               : return "ComCmd_put_SyncCallsOn";
    case ComCmd_get_SyncCallsTimeOut          : return "ComCmd_get_SyncCallsTimeOut";
    case ComCmd_put_SyncCallsTimeOut          : return "ComCmd_put_SyncCallsTimeOut";
    case ComCmd_LoadProject                   : return "ComCmd_LoadProject";
    case ComCmd_SaveProject                   : return "ComCmd_SaveProject";
    case ComCmd_SaveProjectNV                 : return "ComCmd_SaveProjectNV";
    case ComCmd_ExportNeutralDB               : return "ComCmd_ExportNeutralDB"; 
    case ComCmd_ImportNeutralDB               : return "ComCmd_ImportNeutralDB"; 
    case ComCmd_LoadSnapshot                  : return "ComCmd_LoadSnapshot";
    case ComCmd_SaveSnapshot                  : return "ComCmd_SaveSnapshot";
    case ComCmd_LoadScenario                  : return "ComCmd_LoadScenario";
    case ComCmd_SaveScenario                  : return "ComCmd_SaveScenario";
    case ComCmd_LoadBacktrack                 : return "ComCmd_LoadBacktrack";
    case ComCmd_SaveBacktrack                 : return "ComCmd_SaveBacktrack";
    case ComCmd_CloseProject                  : return "ComCmd_CloseProject";
    case ComCmd_SetTag                        : return "ComCmd_SetTag";
    case ComCmd_GetTag                        : return "ComCmd_GetTag";
    case ComCmd_get_TagValue                  : return "ComCmd_get_TagValue";
    case ComCmd_put_TagValue                  : return "ComCmd_put_TagValue";
    case ComCmd_StartDynamicSolve             : return "ComCmd_StartDynamicSolve";
    case ComCmd_StartProbalSolve              : return "ComCmd_StartProbalSolve";
    case ComCmd_Start                         : return "ComCmd_Start";
    case ComCmd_Step                          : return "ComCmd_Step";
    case ComCmd_Stop                          : return "ComCmd_Stop";
    case ComCmd_Idle                          : return "ComCmd_Idle";
    case ComCmd_Wait                          : return "ComCmd_Wait";
    case ComCmd_WaitCount                     : return "ComCmd_WaitCount";
    case ComCmd_WaitTillSteadyState           : return "ComCmd_WaitTillSteadyState";
    case ComCmd_WaitTillStop                  : return "ComCmd_WaitTillStop";
    case ComCmd_ResetWait                     : return "ComCmd_ResetWait";
    case ComCmd_Report_GetValues              : return "ComCmd_Report_GetValues";
    case ComCmd_Report_Trend                  : return "ComCmd_Report_Trend";
    case ComCmd_Report_SetValues              : return "ComCmd_Report_SetValues";
    case ComCmd_Report_SaveAndClose           : return "ComCmd_Report_SaveAndClose";
    case ComCmd_Report_ExecuteMacro           : return "ComCmd_Report_ExecuteMacro";
    case ComCmd_get_CmpFilename               : return "ComCmd_get_CmpFilename";
    case ComCmd_put_CmpFilename               : return "ComCmd_put_CmpFilename";
    case ComCmd_get_CmpSort                   : return "ComCmd_get_CmpSort";
    case ComCmd_put_CmpSort                   : return "ComCmd_put_CmpSort";
    case ComCmd_get_CmpMaxCount               : return "ComCmd_get_CmpMaxCount";
    case ComCmd_put_CmpMaxCount               : return "ComCmd_put_CmpMaxCount";
    case ComCmd_get_CmpRelativeTolerance      : return "ComCmd_get_CmpRelativeTolerance";
    case ComCmd_put_CmpRelativeTolerance      : return "ComCmd_put_CmpRelativeTolerance";
    case ComCmd_get_CmpAbsoluteTolerance      : return "ComCmd_get_CmpAbsoluteTolerance";
    case ComCmd_put_CmpAbsoluteTolerance      : return "ComCmd_put_CmpAbsoluteTolerance";
    case ComCmd_get_CmpShowDifferentStrings   : return "ComCmd_get_CmpShowDifferentStrings";
    case ComCmd_put_CmpShowDifferentStrings   : return "ComCmd_put_CmpShowDifferentStrings";
    case ComCmd_get_CmpShowMissingTags        : return "ComCmd_get_CmpShowMissingTags";
    case ComCmd_put_CmpShowMissingTags        : return "ComCmd_put_CmpShowMissingTags";
    case ComCmd_CompareOptions                : return "ComCmd_CompareOptions";
    case ComCmd_CompareOptions2               : return "ComCmd_CompareOptions2";
    case ComCmd_CompareReset                  : return "ComCmd_CompareReset";
    case ComCmd_CompareScenarios              : return "ComCmd_CompareScenarios";
    case ComCmd_CompareScenarioToCurrent      : return "ComCmd_CompareScenarioToCurrent";
    case ComCmd_TestScenario                  : return "ComCmd_TestScenario";
    case ComCmd_RestartHistorian              : return "ComCmd_RestartHistorian";
    case ComCmd_Sleep                         : return "ComCmd_Sleep";
    case ComCmd_Exit                          : return "ComCmd_Exit";
    case ComCmd_get_EventsOn                  : return "ComCmd_get_EventsOn";
    case ComCmd_put_EventsOn                  : return "ComCmd_put_EventsOn";
    case ComCmd_get_IsStopped                 : return "ComCmd_get_IsStopped";
    case ComCmd_get_IsIdling                  : return "ComCmd_get_IsIdling";
    case ComCmd_get_IsRunning                 : return "ComCmd_get_IsRunning";
    case ComCmd_get_StepSize                  : return "ComCmd_get_StepSize";
    case ComCmd_put_StepSize                  : return "ComCmd_put_StepSize";
    case ComCmd_get_RealTimeOn                : return "ComCmd_get_RealTimeOn";
    case ComCmd_put_RealTimeOn                : return "ComCmd_put_RealTimeOn";
    case ComCmd_get_RealTimeMultiplier        : return "ComCmd_get_RealTimeMultiplier";
    case ComCmd_put_RealTimeMultiplier        : return "ComCmd_put_RealTimeMultiplier";
    case ComCmd_get_SequenceNo                : return "ComCmd_get_SequenceNo";
    case ComCmd_put_SequenceNo                : return "ComCmd_put_SequenceNo";
    case ComCmd_SetAppWndState                : return "ComCmd_SetAppWndState";
    case ComCmd_OnVariableSet                 : return "ComCmd_OnVariableSet";
    case ComCmd_VariableSetReplayItem         : return "ComCmd_VariableSetReplayItem";
    case ComCmd_VariableSetReplayCtrl         : return "ComCmd_VariableSetReplayCtrl";
                                                 
    case ComCmd_EOC                           : return "ComCmd_EOC";

    case ComState_Idle                        : return "ComState_Idle";
    case ComState_Stop                        : return "ComState_Stop";
    case ComState_StepProbal                  : return "ComState_StepProbal";
    case ComState_StepDynamic                 : return "ComState_StepDynamic";
    case ComCmd_get_TagType                   : return "ComCmd_get_TagType";
    case ComCmd_get_TagRecording              : return "ComCmd_get_TagRecording";
    case ComCmd_put_TagRecording              : return "ComCmd_put_TagRecording";

    case MaxComEvents                         : return "MaxComEvents";
                                                 
    case ComCmd_ExecNextCmd                   : return "NULL/ComCmd_ExecNextCmd";
    default: return "???";
    }
  }

//=========================================================================

//=========================================================================
//
//
//
//=========================================================================
