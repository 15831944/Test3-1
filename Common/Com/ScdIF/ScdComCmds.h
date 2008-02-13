// ComCom.h: interface for the CComCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCDCOMCMDS_H__00BA0271_074E_4644_97D6_0F13D81677E9__INCLUDED_)
#define AFX_SCDCOMCMDS_H__00BA0271_074E_4644_97D6_0F13D81677E9__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#if defined(__SCDCOMCMDS_CPP)
  #define DllImportExport __declspec(dllexport)
#elif !defined(SCDIF)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

#include <atlbase.H>
#include "scdif.h"

class DllImportExport CScdCmdTag
  {
  public:
    CScdCmdTag()
      {
      m_pTag=NULL;
      m_pvtValue=NULL;
      m_pLong=NULL;
      m_lLong=0;
      }

    LPCTSTR   m_pTag;
    VARIANT * m_pvtValue;
    long    * m_pLong;
    long      m_lLong;
  
  };

//inline GetCS(DWORD d) { return (d >> 16) & 0xffff; }
//inline GetCC(DWORD d) { return d & 0xffff; }
//
//const long  CS_Invalid                        =  0x00000000;
//const long  CS_Kbd                            =  0x00010000;
//const long  CS_Cmd                            =  0x00020000;
//const long  CS_App                            =  0x00040000;
//const long  CS_Mdl                            =  0x00080000;
//const long  CS_Script                         =  0x00100000;

const long  ComCmd_NULL                           =  0;
const long  ComCmd_get_UpNAbout                   =  1;
const long  ComCmd_WaitUpNAbout                   =  2;
const long  ComCmd_get_SyncCallsOn                =  3;
const long  ComCmd_put_SyncCallsOn                =  4;
const long  ComCmd_get_SyncCallsTimeOut           =  5;
const long  ComCmd_put_SyncCallsTimeOut           =  6;

const long  ComCmd_CreateProject                  = 10;
const long  ComCmd_LoadProject                    = 11;
const long  ComCmd_SaveProject                    = 12;
const long  ComCmd_SaveProjectNV                  = 13;
const long  ComCmd_ExportNeutralDB                = 14;
const long  ComCmd_ImportNeutralDB                = 15;
const long  ComCmd_LoadSnapshot                   = 16;
const long  ComCmd_SaveSnapshot                   = 17;
const long  ComCmd_LoadScenario                   = 18;
const long  ComCmd_SaveScenario                   = 19;
const long  ComCmd_LoadBacktrack                  = 20;
const long  ComCmd_SaveBacktrack                  = 21;
const long  ComCmd_CloseProject                   = 22;
const long  ComCmd_SetTag                         = 23;
const long  ComCmd_GetTag                         = 24;
const long  ComCmd_StartDynamicSolve              = 25;
const long  ComCmd_StartProbalSolve               = 26;
const long  ComCmd_Start                          = 27;
const long  ComCmd_Step                           = 28;
const long  ComCmd_Stop                           = 29;
const long  ComCmd_Idle                           = 30;
const long  ComCmd_RunToSteadyState               = 31;
const long  ComCmd_Wait                           = 32;
const long  ComCmd_WaitCount                      = 33;
const long  ComCmd_WaitTillSteadyState            = 34;
const long  ComCmd_WaitTillStop                   = 35;
const long  ComCmd_ResetWait                      = 36;
const long  ComCmd_Report_GetValues               = 37;
const long  ComCmd_Report_GetInfo                 = 38;
const long  ComCmd_Report_Trend                   = 39;
const long  ComCmd_Report_SetValues               = 40;
const long  ComCmd_Report_SaveAndClose            = 41;
const long  ComCmd_Report_ExecuteMacro            = 42;
const long  ComCmd_Report_Process                 = 43;

const long  ComCmd_DocCmd                         = 45;

const long  ComCmd_get_CmpFilename                = 50;
const long  ComCmd_put_CmpFilename                = 51;
const long  ComCmd_get_CmpSort                    = 52;
const long  ComCmd_put_CmpSort                    = 53;
const long  ComCmd_get_CmpMaxCount                = 54;
const long  ComCmd_put_CmpMaxCount                = 55;
const long  ComCmd_get_CmpRelativeTolerance       = 56;
const long  ComCmd_put_CmpRelativeTolerance       = 57;
const long  ComCmd_get_CmpAbsoluteTolerance       = 58;
const long  ComCmd_put_CmpAbsoluteTolerance       = 59;
const long  ComCmd_get_CmpShowDifferentStrings    = 60;
const long  ComCmd_put_CmpShowDifferentStrings    = 61;
const long  ComCmd_get_CmpShowMissingTags         = 62;
const long  ComCmd_put_CmpShowMissingTags         = 63; // Specifically for Scripting
const long  ComCmd_CompareOptions                 = 64; // Specifically for Scripting
const long  ComCmd_CompareOptions2                = 65;
const long  ComCmd_CompareReset                   = 66;
const long  ComCmd_CompareScenarios               = 67;
const long  ComCmd_CompareScenarioToCurrent       = 68;
const long  ComCmd_TestScenario                   = 69;
const long  ComCmd_RestartHistorian               = 70;
const long  ComCmd_Sleep                          = 71;
const long  ComCmd_Exit                           = 72;
const long  ComCmd_get_EventsOn                   = 73;
const long  ComCmd_put_EventsOn                   = 74;
const long  ComCmd_get_IsStopped                  = 75;
const long  ComCmd_get_IsIdling                   = 76;
const long  ComCmd_get_IsRunning                  = 77;
const long  ComCmd_get_StepSize                   = 78;
const long  ComCmd_put_StepSize                   = 79;
const long  ComCmd_get_RealTimeOn                 = 80;
const long  ComCmd_put_RealTimeOn                 = 81;
const long  ComCmd_get_RealTimeMultiplier         = 82;
const long  ComCmd_put_RealTimeMultiplier         = 93;
const long  ComCmd_get_SequenceNo                 = 94;
const long  ComCmd_put_SequenceNo                 = 95;
const long  ComCmd_SetAppWndState                 = 96;
const long  ComCmd_OnVariableSet                  = 97;
const long  ComCmd_VariableSetReplayItem          = 98;
const long  ComCmd_VariableSetReplayCtrl          = 99;
const long  ComCmd_get_TagValue                   = 100;
const long  ComCmd_put_TagValue                   = 101;
const long  ComCmd_EOC                            = 102; // Specifically for Scripting

const long  ComState_Idle                         = 110;
const long  ComState_Stop                         = 111;
const long  ComState_StepProbal                   = 112;
const long  ComState_StepDynamic                  = 113;

const long  ComCmd_get_TagType                    = 114;
const long  ComCmd_get_TagRecording               = 115;
const long  ComCmd_put_TagRecording               = 116;

const long  ComCmd_HistorianDelete                = 117;

// ADJUST WHEN NECCESSARY
const long  MaxComEvents                          = 120;
                                                  
const long  ComCmd_ExecNextCmd                    =  0; // Specifically for Scripting

extern DllImportExport LPCTSTR ScdCOMCmd2Str(long Cmd);

//=========================================================================

#undef DllImportExport


#endif // !defined(AFX_SCDCOMCMDS_H__00BA0271_074E_4644_97D6_0F13D81677E9__INCLUDED_)
