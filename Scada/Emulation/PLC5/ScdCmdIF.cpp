// CmdCallback.cpp: implementation of the CScdCmdIF class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScdCmdIF.h"
#include "PLC5.h"
#include "PLC5Dlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TheDlg (*((CPLC5Dlg*)AfxGetMainWnd()))

CScdCmdIF::CScdCmdIF()
{

}

CScdCmdIF::~CScdCmdIF()
{

}

CLSID CLSID_SysCADCmd = {0x09087B23,0xB280,0x4A17,{0x9A,0x7C,0xB5,0x45,0x2E,0x2C,0x3E,0xA7}};

CLSID& CScdCmdIF::clsidGetCLSID()
{
	return CLSID_SysCADCmd;
}


LPWSTR CScdCmdIF::pwszGetProgID()
{
	return L"PLC5.SysCADCmd.1";
}

LPWSTR CScdCmdIF::pwszGetVIProgID()
{
	return L"PLC5.SysCADCmd";
}

LPWSTR CScdCmdIF::pwszGetClassDesc()
{
	return L"PLC5 SysCADCmd";
}


HRESULT CScdCmdIF::get_UpNAbout(VARIANT_BOOL * pVal)
  {
  * pVal=theApp.m_UpNAbout;
	return S_OK;//theApp.m_UpNAbout;
  }

HRESULT CScdCmdIF::WaitUpNAbout(double TimeOut)
  {
  // Time out in Seconds
  while (!theApp.m_UpNAbout && (TimeOut>0))
    {
    ::Sleep(100);
    TimeOut-=0.100;
    }

  if (theApp.m_UpNAbout)
    return S_OK;
  else
    return theApp.m_ScdCmdIF.ReportError("WaitUpNAbout Timeout", 4);
  }

HRESULT CScdCmdIF::LoadProject( LPCTSTR pszProjectName )
  {
  CString *p=new CString(pszProjectName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_LoadProject, (LPARAM)p);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveProject( LPCTSTR pszProjectName )
  {
  CString *p=new CString(pszProjectName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_SaveProject, (LPARAM)p);
	return S_OK;
  };

HRESULT CScdCmdIF::LoadSnapshot( LPCTSTR pszSnapshotName )
  {
  CString *p=new CString(pszSnapshotName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_LoadSnapshot, (LPARAM)p);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveSnapshot( LPCTSTR pszSnapshotName )
  {
  CString *p=new CString(pszSnapshotName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_SaveSnapshot, (LPARAM)p);
	return S_OK;
  };

HRESULT CScdCmdIF::LoadScenario( LPCTSTR pszScenarioName )
  {
  CString *p=new CString(pszScenarioName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_LoadScenario);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveScenario( LPCTSTR pszScenarioName )
  {
  CString *p=new CString(pszScenarioName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_SaveScenario);
	return S_OK;
  };

HRESULT CScdCmdIF::LoadBacktrack( LPCTSTR pszBacktrackName )
  {
  CString *p=new CString(pszBacktrackName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_LoadBacktrack, (LPARAM)p);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveBacktrack( LPCTSTR pszBacktrackName )
  {
  CString *p=new CString(pszBacktrackName);
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_SaveBacktrack, (LPARAM)p);
	return S_OK;
  };

HRESULT CScdCmdIF::CloseProject( )
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_CloseProject);
	return S_OK;
  };


HRESULT CScdCmdIF::SetTag(LPCTSTR Tag, VARIANT * Value)
  {
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=Value;
  ::AfxGetMainWnd()->SendMessage(WM_COMCMD, ComCmd_SetTag, (LPARAM)&T);
	return S_OK;
  };

HRESULT CScdCmdIF::GetTag(LPCTSTR Tag, VARIANT * Value)
  {
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=Value;
  ::AfxGetMainWnd()->SendMessage(WM_COMCMD, ComCmd_GetTag, (LPARAM)&T);
	return S_OK;
  };

HRESULT CScdCmdIF::put_TagValue(LPCTSTR Tag, VARIANT Value)
  {
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=&Value;
  ::AfxGetMainWnd()->SendMessage(WM_COMCMD, ComCmd_SetTag, (LPARAM)&T);
	return S_OK;
  };

HRESULT CScdCmdIF::get_TagValue(LPCTSTR Tag, VARIANT * Value)
  {
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=Value;
  ::AfxGetMainWnd()->SendMessage(WM_COMCMD, ComCmd_GetTag, (LPARAM)&T);
	return S_OK;
  };

HRESULT CScdCmdIF::StartDynamicSolve()
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_StartDynamicSolve);
  return S_OK;
  }

HRESULT CScdCmdIF::StartProbalSolve()
  {
  //::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_StartDynamicSolve);
  return E_NOTIMPL;
  }

HRESULT CScdCmdIF::Start()
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_Start);
  return S_OK;
  }

HRESULT CScdCmdIF::Step(long IterMark, double WaitForNext)
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_Step);
  return S_OK;
  }

HRESULT CScdCmdIF::Stop()
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_Stop);
	return S_OK;
  }

HRESULT CScdCmdIF::Idle()
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_Idle);
	return S_OK;
  }


HRESULT CScdCmdIF::Wait(/*[in]*/ double Seconds) { return E_NOTIMPL; };
HRESULT CScdCmdIF::WaitCount(/*[in]*/ long Iterations) { return E_NOTIMPL; };
HRESULT CScdCmdIF::WaitTillSteadyState() { return E_NOTIMPL; };
HRESULT CScdCmdIF::WaitTillStop() { return E_NOTIMPL; };
HRESULT CScdCmdIF::ResetWait() { return E_NOTIMPL; };
HRESULT CScdCmdIF::GenerateTagReport(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_NOTIMPL; };
HRESULT CScdCmdIF::GenerateTrendReport(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_NOTIMPL; };
HRESULT CScdCmdIF::ProcessSetTags(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_NOTIMPL; };
HRESULT CScdCmdIF::SaveAndCloseReport(/*[in]*/ LPCTSTR Filename) { return E_NOTIMPL; };
HRESULT CScdCmdIF::ExecuteMacro(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpFilename(/*[out, retval]*/ BSTR *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpFilename(/*[in]*/ BSTR newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpSort(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpSort(/*[in]*/ VARIANT_BOOL newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpMaxCount(/*[out, retval]*/ long *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpMaxCount(/*[in]*/ long newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpRelativeTolerance(/*[out, retval]*/ double *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpRelativeTolerance(/*[in]*/ double newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpAbsoluteTolerance(/*[out, retval]*/ double *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpAbsoluteTolerance(/*[in]*/ double newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpShowDifferentStrings(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpShowDifferentStrings(/*[in]*/ VARIANT_BOOL newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_CmpShowMissingTags(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::put_CmpShowMissingTags(/*[in]*/ VARIANT_BOOL newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::CompareReset() { return E_NOTIMPL; };
HRESULT CScdCmdIF::CompareScenarios(/*[in]*/ LPCTSTR Filename1, /*[in]*/ LPCTSTR Filename2) { return E_NOTIMPL; };
HRESULT CScdCmdIF::CompareScenarioToCurrent(/*[in]*/ LPCTSTR Filename) { return E_NOTIMPL; };
HRESULT CScdCmdIF::TestScenario(/*[in]*/ LPCTSTR Filename) { return E_NOTIMPL; };
HRESULT CScdCmdIF::RestartHistorian() { return E_NOTIMPL; };
HRESULT CScdCmdIF::Sleep(double Seconds) { return E_NOTIMPL; };
HRESULT CScdCmdIF::Exit()
  {
  ::AfxGetMainWnd()->PostMessage(WM_COMCMD, ComCmd_Exit);
	return S_OK;
  }
//HRESULT CScdCmdIF::get_EventsOn(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_NOTIMPL; };
//HRESULT CScdCmdIF::put_EventsOn(/*[in]*/ VARIANT_BOOL newVal) { return E_NOTIMPL; };
HRESULT CScdCmdIF::get_IsStopped(/*[out, retval]*/ VARIANT_BOOL *pVal)
  { 
  *pVal=(TheDlg.m_iState==State_Stopped);
  return S_OK;
  };
HRESULT CScdCmdIF::get_IsIdling(/*[out, retval]*/ VARIANT_BOOL *pVal)
  { 
  *pVal=(TheDlg.m_iState==State_Idling);
  return S_OK;
  };
HRESULT CScdCmdIF::get_IsRunning(/*[out, retval]*/ VARIANT_BOOL *pVal) 
  { 
  *pVal=(TheDlg.m_iState==State_Running);
  return S_OK;
  };
HRESULT CScdCmdIF::get_StepSize(/*[out, retval]*/ double *pVal)
  {
  *pVal=TheDlg.m_StepMS*0.001;
  return S_OK;
  };
HRESULT CScdCmdIF::put_StepSize(/*[in]*/ double newVal)
  {
  TheDlg.m_StepMS=(long)(newVal*1000);
  return S_OK;
  };
HRESULT CScdCmdIF::get_RealTimeOn(/*[out, retval]*/ VARIANT_BOOL *pVal) 
  { 
  return E_NOTIMPL;
  };
HRESULT CScdCmdIF::put_RealTimeOn(/*[in]*/ VARIANT_BOOL newVal)
  { 
  return E_NOTIMPL;
  };
HRESULT CScdCmdIF::get_RealTimeMultiplier(/*[out, retval]*/ double *pVal) 
  { 
  *pVal=TheDlg.m_RunMult;
  return S_OK;
  };
HRESULT CScdCmdIF::put_RealTimeMultiplier(/*[in]*/ double newVal)
  { 
  TheDlg.m_RunMult=(float)newVal;
  return S_OK;
  };
HRESULT CScdCmdIF::get_SequenceNo(/*[out, retval]*/ long *pVal) 
  { 
  *pVal=TheDlg.m_lSequenceNo;
  return S_OK;
  };
HRESULT CScdCmdIF::put_SequenceNo(/*[in]*/ long newVal)
  { 
  TheDlg.m_lSequenceNo=newVal;
  return S_OK;
  };
HRESULT CScdCmdIF::SetAppWndState(/*[in]*/ long ReqdState)
  { 
  HRESULT ret=S_OK;
  switch (ReqdState)
    {
    case 1: // Foreground
      if (!AfxGetMainWnd()->SetForegroundWindow()) 
        ret=E_FAIL; 
      break;
    case 2: // Normal 
    case 3: // Maximise
    case 4: // Minimise
      {
      WINDOWPLACEMENT WP;
      AfxGetMainWnd()->GetWindowPlacement(&WP);
      switch (ReqdState)
        {
        case 2: WP.showCmd=SW_SHOWNORMAL; break; 
        case 3: WP.showCmd=SW_SHOWMAXIMIZED; break; 
        case 4: WP.showCmd=SW_SHOWMINIMIZED; break;
        }
      if (!AfxGetMainWnd()->SetWindowPlacement(&WP))
        ret=E_FAIL;
      break;
      }
    case 5: // Activate User Buttons
    case 6: // Deactivate User Buttons
      {
      TheDlg.ActivateInterface(ReqdState==5);
      break;
      }
    }
  return ret;
  };

HRESULT CScdCmdIF::VariableSetReplayItem(/* [in] */ long SequenceNo,/* [in] */ LPCTSTR Tag,/* [in] */ VARIANT Value,/* [in] */ LPCTSTR CnvTxt)
  {
  return S_OK;
  }
HRESULT CScdCmdIF::VariableSetReplayCtrl(/* [in] */ long Command, /* [in] */ long Param)
  {
  return S_OK;
  }
