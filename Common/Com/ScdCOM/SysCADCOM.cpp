// Helper.cpp: Implementation of CHelper
#include "stdafx.h"
#define __SYSCADCOM_CPP
#include <atlstr.H>
#include "ScdCOM.h"
#include "SysCADCOM.h"
//#include "scdcomtmpl.h"


//The Lines sample defines an exception handler that fills the EXCEPINFO structure and signals IDispatch to return DISP_E_EXCEPTION (App.cpp):
/*
//STDMETHODIMP 
HRESULT RaiseException (LPCTSTR szError, GUID & rguid)
{
USES_CONVERSION;
   extern HRESULT g_scodes[];
   //TCHAR szError[128];//STR_LEN];
   ICreateErrorInfo *pcerrinfo;
   IErrorInfo *perrinfo;
   HRESULT hr;
   BSTR bstrDescription = NULL;

//   if (LoadString(g_pApplication->m_hinst, nID, szError,    sizeof(szError)));
   bstrDescription =   SysAllocString(T2OLE(szError));

   // Set ErrInfo object so that VTBL binding controllers can get
   // rich error information. If the controller is using IDispatch to    // access properties or methods, DispInvoke will fill the EXCEPINFO    // structure using the values specified in the ErrorInfo object and    // DispInvoke will return DISP_e_EXCEPTION. The property or method    // must return a failure return value for DispInvoke to do this.
      hr = CreateErrorInfo(&pcerrinfo);
      if (SUCCEEDED(hr))
   {
      pcerrinfo->SetGUID(rguid);
      pcerrinfo->SetSource(g_pApplication->m_bstrProgID);
      if (bstrDescription)
         pcerrinfo->SetDescription(bstrDescription);
      hr = pcerrinfo->QueryInterface(IID_IerrorInfo, (LPVOID *) &perrinfo);
      if (SUCCEEDED(hr))
{
         SetErrorInfo(0, perrinfo);
         perrinfo->Release();
   }
   if (bstrDescription)
      SysFreeString(bstrDescription);
   return ResultFromScode(g_scodes[nID-1001]);
}
 
*/

/////////////////////////////////////////////////////////////////////////////
// CSysCADCmd

HRESULT CSysCADCmd::FinalConstruct()
  {
	//FC_TESTLICENSE(1);

  return S_OK;
  };
void CSysCADCmd::FinalRelease()
  {
  int xxx=0;
  };

CScdCOMBase	*CSysCADCmd::sm_pCallback = 0;

STDMETHODIMP CSysCADCmd::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ISysCADCmd
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CSysCADCmd::Register(ULONG i_dwCastedCallback)
{

//ASDFG xxx;

	sm_pCallback = reinterpret_cast<CScdCOMBase*>(i_dwCastedCallback);
  sm_pCallback->Initialise(this);
	//sm_pSysCADCmd=this;
	CLSID&	koos = sm_pCallback->clsidGetCLSID();
	return S_OK;
}

STDMETHODIMP CSysCADCmd::GetResourceHInstance(ULONG *hResInst)
  {
  *hResInst=(ULONG)_Module.m_hInstResource;
	return S_OK;
  }

STDMETHODIMP CSysCADCmd::GetResourceCBRegID(ULONG *ID)
  {
  *ID=(ULONG)IDR_CALLBACKCLASS;
	return S_OK;
  }

void CSysCADCmd::InitCOCmdStuff(DWORD EvId)
  {
  if (!m_bSyncCalls)
    return;
  _ASSERTE(EvId<MaxComEvents);
  if (m_hEventRcvd[EvId]==INVALID_HANDLE_VALUE)
    m_hEventRcvd[EvId]=CreateEvent(NULL, FALSE, FALSE, NULL);        
  }

UINT CSysCADCmd::ExecCOCmdStuff(DWORD EvId)
  {
  if (!m_bSyncCalls)
    return SCS_CallASync;
  _ASSERTE(EvId<MaxComEvents);
  if (WaitForSingleObject(m_hEventRcvd[EvId], m_dwSyncTimeOut)==WAIT_TIMEOUT)
    return SCS_CallTimeout;
  return SCS_CallReturned;
  }

void CSysCADCmd::InitCOCmdStuff(DWORD *EvIds, DWORD nEvts)
  {
  if (!m_bSyncCalls)
    return;
  _ASSERTE(nEvts<MaxComEvents);
  for (UINT i=0; i<nEvts; i++)
    {
    if (m_hEventRcvd[EvIds[i]]==INVALID_HANDLE_VALUE)
      m_hEventRcvd[EvIds[i]]=CreateEvent(NULL, FALSE, FALSE, NULL);       
    }
  };

UINT CSysCADCmd::ExecCOCmdStuff(DWORD *EvIds, DWORD nEvts)
  {
  if (!m_bSyncCalls)
    return SCS_CallASync;
  HANDLE h[MaxComEvents];
  _ASSERTE(nEvts<MaxComEvents);
  for (UINT i=0; i<nEvts; i++)
    {
    _ASSERTE(EvIds[i]<MaxComEvents);
    h[i]=m_hEventRcvd[EvIds[i]];
    }
  if (WaitForMultipleObjects(nEvts, h, FALSE, m_dwSyncTimeOut)==WAIT_TIMEOUT)
    {
    return SCS_CallTimeout;
    }
  return SCS_CallReturned;
  };

void CSysCADCmd::DoEventMsg(UINT e, LPARAM l)
  {
  //e=GetCC(e);
  if (m_bSyncCalls)
    {
    _ASSERTE(e<MaxComEvents);
    if (m_hEventRcvd[e]!=INVALID_HANDLE_VALUE)
      {
      m_lEventLParam[e]=l;
      ::SetEvent(m_hEventRcvd[e]);
      }
    switch (e)
      {
      case ComState_StepProbal: 
        if (m_bEventsOn)
          Fire_OnStepProbal(l); 
        break;
      case ComState_StepDynamic: 
        if (m_bEventsOn)
          Fire_OnStepDynamic(l); 
        break;
      case ComState_Stop: 
        if (m_bEventsOn)
          Fire_OnStop(l); 
        break;
      case ComState_Idle: 
        if (m_bEventsOn)
          Fire_OnIdle(l); 
        break;

      case ComCmd_LoadProject: 
        if (m_bEventsOn)
          Fire_OnProjectLoaded(l); 
        break;
      case ComCmd_SaveProject: 
      case ComCmd_SaveProjectNV: 
        if (m_bEventsOn)
          Fire_OnProjectSaved(l); 
        break;
      case ComCmd_CloseProject: 
        if (m_bEventsOn)
          Fire_OnProjectClosed(l); 
        break;
      case ComCmd_ExportNeutralDB: 
      case ComCmd_ImportNeutralDB: 
        //if (m_bEventsOn)
        //  Fire_OnProjectSaved(l); 
        break;
      case ComCmd_put_TagValue: 
      case ComCmd_SetTag: 
        if (m_bEventsOn)
          Fire_OnSetTag(l); 
        break;
      case ComCmd_get_TagValue: 
      case ComCmd_GetTag: 
        if (m_bEventsOn)
          Fire_OnGetTag(l); 
        break;

      case ComCmd_LoadSnapshot: 
        if (m_bEventsOn)
          Fire_OnSnapshotLoaded(l); 
        break;
      case ComCmd_SaveSnapshot: 
        if (m_bEventsOn)
          Fire_OnSnapshotSaved(l); 
        break;
      case ComCmd_LoadScenario: 
        if (m_bEventsOn)
          Fire_OnScenarioLoaded(l); 
        break;
      case ComCmd_SaveScenario: 
        if (m_bEventsOn)
          Fire_OnScenarioSaved(l); 
        break;
      case ComCmd_LoadBacktrack: 
        if (m_bEventsOn)
          Fire_OnBacktrackLoaded(l); 
        break;
      case ComCmd_SaveBacktrack: 
        if (m_bEventsOn)
          Fire_OnBacktrackSaved(l); 
        break;

      case ComCmd_OnVariableSet:
        {
        EVM_VariableSet *pMVS=(EVM_VariableSet*)l;
        if (m_bEventsOn)
          Fire_OnVariableSet(pMVS->m_lSource, pMVS->m_lSequenceNo, pMVS->m_bsTag, pMVS->m_vValue, pMVS->m_bsCnvTxt); 
        CoTaskMemFree(pMVS);
        break;
        }
      default:
        ReportError("Unexpected DoEventMsg", e);
        break;
      }
    }
  else
    {
    switch (e)
      {
      //case ComCmd_get_UpNAbout: Fire_OnUpNAbout(w); break;
      case ComCmd_StartProbalSolve: 
      case ComCmd_StartDynamicSolve: 
      case ComCmd_Start: 
      case ComCmd_Step:
      case ComCmd_Stop: 
      case ComCmd_Idle: 
      case ComCmd_RunToSteadyState: 
        ReportError("Unexpected DoEventMsg", e);
        break;
      case ComCmd_Exit:
        /*Fire_OnExit(l);???*/ 
        break;
      case ComCmd_LoadProject: 
        if (m_bEventsOn)
          Fire_OnProjectLoaded(l); 
        break;
      case ComCmd_SaveProject: 
      case ComCmd_SaveProjectNV: 
        if (m_bEventsOn)
          Fire_OnProjectSaved(l); 
        break;
      case ComCmd_CloseProject: 
        if (m_bEventsOn)
          Fire_OnProjectClosed(l); 
        break;
      case ComCmd_ExportNeutralDB: 
      case ComCmd_ImportNeutralDB: 
        //if (m_bEventsOn)
        //  Fire_OnProjectSaved(l); 
        break;
      case ComCmd_put_TagValue: 
      case ComCmd_SetTag: 
        if (m_bEventsOn)
          Fire_OnSetTag(l); 
        break;
      case ComCmd_get_TagValue: 
      case ComCmd_GetTag: 
        if (m_bEventsOn)
          Fire_OnGetTag(l); 
        break;
      case ComState_StepProbal: 
        if (m_bEventsOn)
          Fire_OnStepProbal(l); 
        break;
      case ComState_StepDynamic: 
        if (m_bEventsOn)
          {
          //AtlTrace("Fire_OnStepDynamic %i\n", l);  
          Fire_OnStepDynamic(l);
          //AtlTrace("Done !\n");  
          }
        break;
      case ComState_Stop: 
        if (m_bEventsOn)
          Fire_OnStop(l); 
        break;
      case ComState_Idle: 
        if (m_bEventsOn)
          Fire_OnIdle(l); 
        break;
      case ComCmd_LoadSnapshot: 
        if (m_bEventsOn)
          Fire_OnSnapshotLoaded(l); 
        break;
      case ComCmd_SaveSnapshot: 
        if (m_bEventsOn)
          Fire_OnSnapshotSaved(l); 
        break;
      case ComCmd_LoadScenario: 
        if (m_bEventsOn)
          Fire_OnScenarioLoaded(l); 
        break;
      case ComCmd_SaveScenario: 
        if (m_bEventsOn)
          Fire_OnScenarioSaved(l); 
        break;
      case ComCmd_LoadBacktrack: 
        if (m_bEventsOn)
          Fire_OnBacktrackLoaded(l); 
        break;
      case ComCmd_SaveBacktrack: 
        if (m_bEventsOn)
          Fire_OnBacktrackSaved(l); 
        break;

      case ComCmd_OnVariableSet:
        {
        EVM_VariableSet *pMVS=(EVM_VariableSet*)l;
        if (m_bEventsOn)
          Fire_OnVariableSet(pMVS->m_lSource, pMVS->m_lSequenceNo, pMVS->m_bsTag, pMVS->m_vValue, pMVS->m_bsCnvTxt);
        CoTaskMemFree(pMVS);
        break;
        }
      default:
        ReportError("Unexpected DoEventMsg", e);
        break;
      }
    }
  }

// ========================================================================

#define TRANSFERPROP(Name, Param)                                        \
	try                                                                    \
	{                                                                      \
		if (SUCCEEDED(sm_pCallback->Name(Param)))                            \
      return S_OK;                                                       \
    else                                                                 \
      return E_FAIL;                                                     \
	}                                                                      \
	catch(...)                                                             \
	{                                                                      \
		return E_FAIL;                                                       \
	}                                                                      \
                                                                         \
	return S_OK;

#define TRANSFER0(Name, Cmd)                                             \
	try                                                                    \
	{                                                                      \
    InitCOCmdStuff(Cmd);                                                 \
		if (SUCCEEDED(sm_pCallback->Name()))                                 \
      {                                                                  \
      switch (ExecCOCmdStuff(Cmd))                                       \
        {                                                                \
        case SCS_CallReturned:                                           \
          return S_OK;                                                   \
        case SCS_CallTimeout:                                            \
          return ReportError("Synchronous Call Timeout", 2);             \
        case SCS_CallASync:                                              \
          return S_OK;                                                   \
        }                                                                \
      }                                                                  \
    else                                                                 \
      {                                                                  \
      AtlTrace("ScdCOM Callback0 Failed %i\n", Cmd);                     \
      return E_FAIL;                                                     \
      }                                                                  \
                                                                         \
	}                                                                      \
	catch(...)                                                             \
	{                                                                      \
    AtlTrace("ScdCOM Callback0 Failed - CATCH %i\n", Cmd);               \
		return E_FAIL;                                                       \
	}                                                                      \
	return S_OK;


#define TRANSFER1(Name, Cmd, Param1)                                     \
	try                                                                    \
	{                                                                      \
    InitCOCmdStuff(Cmd);                                                 \
		if (SUCCEEDED(sm_pCallback->Name(Param1)))                           \
      {                                                                  \
      switch (ExecCOCmdStuff(Cmd))                                       \
        {                                                                \
        case SCS_CallReturned:                                           \
          return S_OK;                                                   \
        case SCS_CallTimeout:                                            \
          return ReportError("Synchronous Call Timeout", 2);             \
        case SCS_CallASync:                                              \
          return S_OK;                                                   \
        }                                                                \
      }                                                                  \
    else                                                                 \
      {                                                                  \
      AtlTrace("ScdCOM Callback1 Failed %i\n", Cmd);                     \
      return E_FAIL;                                                     \
      }                                                                  \
	}                                                                      \
	catch(...)                                                             \
	{                                                                      \
    AtlTrace("ScdCOM Callback1 Failed - CATCH %i\n", Cmd);               \
		return E_FAIL;                                                       \
	}                                                                      \
	return S_OK;

#define TRANSFER2(Name, Cmd, Param1, Param2)                             \
	try                                                                    \
	{                                                                      \
    InitCOCmdStuff(Cmd);                                                 \
		if (SUCCEEDED(sm_pCallback->Name(Param1, Param2)))                   \
      {                                                                  \
      switch (ExecCOCmdStuff(Cmd))                                       \
        {                                                                \
        case SCS_CallReturned:                                           \
          return S_OK;                                                   \
        case SCS_CallTimeout:                                            \
          return ReportError("Synchronous Call Timeout", 2);             \
        case SCS_CallASync:                                              \
          return S_OK;                                                   \
        }                                                                \
      }                                                                  \
    else                                                                 \
      {                                                                  \
      AtlTrace("ScdCOM Callback2 Failed %i\n", Cmd);                     \
      return E_FAIL;                                                     \
      }                                                                  \
	}                                                                      \
	catch(...)                                                             \
	{                                                                      \
    AtlTrace("ScdCOM Callback2 Failed - CATCH %i\n", Cmd);               \
		return E_FAIL;                                                       \
	}                                                                      \
	return S_OK;

#define TRANSFER3(Name, Cmd, Param1, Param2, Param3)                     \
	try                                                                    \
	{                                                                      \
    InitCOCmdStuff(Cmd);                                                 \
		if (SUCCEEDED(sm_pCallback->Name(Param1, Param2, Param3)))           \
      {                                                                  \
      switch (ExecCOCmdStuff(Cmd))                                       \
        {                                                                \
        case SCS_CallReturned:                                           \
          return S_OK;                                                   \
        case SCS_CallTimeout:                                            \
          return ReportError("Synchronous Call Timeout", 2);             \
        case SCS_CallASync:                                              \
          return S_OK;                                                   \
        }                                                                \
      }                                                                  \
    else                                                                 \
      {                                                                  \
      AtlTrace("ScdCOM Callback3 Failed %i\n", Cmd);                     \
      return E_FAIL;                                                     \
      }                                                                  \
	}                                                                      \
	catch(...)                                                             \
	{                                                                      \
    AtlTrace("ScdCOM Callback3 Failed - CATCH %i\n", Cmd);               \
		return E_FAIL;                                                       \
	}                                                                      \
	return S_OK;

#define TRANSFER4(Name, Cmd, Param1, Param2, Param3, Param4)             \
	try                                                                    \
	{                                                                      \
    InitCOCmdStuff(Cmd);                                                 \
		if (SUCCEEDED(sm_pCallback->Name(Param1, Param2, Param3, Param4)))   \
      {                                                                  \
      switch (ExecCOCmdStuff(Cmd))                                       \
        {                                                                \
        case SCS_CallReturned:                                           \
          return S_OK;                                                   \
        case SCS_CallTimeout:                                            \
          return ReportError("Synchronous Call Timeout", 2);             \
        case SCS_CallASync:                                              \
          return S_OK;                                                   \
        }                                                                \
      }                                                                  \
    else                                                                 \
      {                                                                  \
      AtlTrace("ScdCOM Callback4 Failed %i\n", Cmd);                     \
      return E_FAIL;                                                     \
      }                                                                  \
	}                                                                      \
	catch(...)                                                             \
	{                                                                      \
    AtlTrace("ScdCOM Callback4 Failed - CATCH %i\n", Cmd);               \
		return E_FAIL;                                                       \
	}                                                                      \
	return S_OK;

// ========================================================================

STDMETHODIMP CSysCADCmd::get_UpNAbout(/*[out, retval]*/ VARIANT_BOOL *pVal)
{
	try
	{
		if (SUCCEEDED(sm_pCallback->get_UpNAbout(pVal)))
      return S_OK;
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CSysCADCmd::WaitUpNAbout(double TimeOut)
{
	try
	{
		if (SUCCEEDED(sm_pCallback->WaitUpNAbout(TimeOut)))
      return S_OK;
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CSysCADCmd::get_SyncCallsOn(VARIANT_BOOL *pVal)
{
  *pVal=m_bSyncCalls;
	return S_OK;
}

STDMETHODIMP CSysCADCmd::put_SyncCallsOn(VARIANT_BOOL newVal)
{
  m_bSyncCalls=newVal;
	return S_OK;
}

STDMETHODIMP CSysCADCmd::get_SyncCallsTimeOut(double *pVal)
{
  *pVal=m_dwSyncTimeOut*0.001;
	return S_OK;
}

STDMETHODIMP CSysCADCmd::put_SyncCallsTimeOut(double newVal)
{
  m_dwSyncTimeOut=DWORD(newVal*1000.0);
	return S_OK;
}

STDMETHODIMP CSysCADCmd::LoadProject(BSTR bsProjectName)
{
  AtlTrace("LoadProject %08x\n", GetCurrentThreadId());	
  TRANSFER1(LoadProject, ComCmd_LoadProject, _bstr_t( bsProjectName ))
}

STDMETHODIMP CSysCADCmd::SaveProject(BSTR bsProjectName)
{
  TRANSFER1(SaveProject, ComCmd_SaveProject, _bstr_t( bsProjectName ))
}

STDMETHODIMP CSysCADCmd::LoadSnapshot(BSTR bsSnapshotName)
{
  TRANSFER1(LoadSnapshot, ComCmd_LoadSnapshot, _bstr_t( bsSnapshotName ))
}

STDMETHODIMP CSysCADCmd::SaveSnapshot(BSTR bsSnapshotName)
{
	TRANSFER1(SaveSnapshot, ComCmd_SaveSnapshot, _bstr_t( bsSnapshotName ))
}

STDMETHODIMP CSysCADCmd::LoadScenario(BSTR bsScenarioName)
{
  TRANSFER1(LoadScenario, ComCmd_LoadScenario, _bstr_t( bsScenarioName ))	
}

STDMETHODIMP CSysCADCmd::SaveScenario(BSTR bsScenarioName)
{
	TRANSFER1(SaveScenario, ComCmd_SaveScenario, _bstr_t( bsScenarioName ))	
}

STDMETHODIMP CSysCADCmd::CloseProject()
{
	TRANSFER0(CloseProject, ComCmd_CloseProject)	
}


STDMETHODIMP CSysCADCmd::SetTag(BSTR Tag, VARIANT * Value)
{
  USES_CONVERSION;
	TRANSFER2(SetTag, ComCmd_SetTag, OLE2T(Tag), Value)
}

STDMETHODIMP CSysCADCmd::GetTag(BSTR Tag, VARIANT * Value)
{
  USES_CONVERSION;
	TRANSFER2(GetTag, ComCmd_GetTag, OLE2T(Tag), Value)
}

STDMETHODIMP CSysCADCmd::put_TagValue(BSTR Tag, VARIANT Value)
{
  USES_CONVERSION;
	TRANSFER2(SetTag, ComCmd_SetTag, OLE2T(Tag), &Value)
}

STDMETHODIMP CSysCADCmd::get_TagValue(BSTR Tag, VARIANT * Value)
{
  USES_CONVERSION;
	TRANSFER2(GetTag, ComCmd_GetTag, OLE2T(Tag), Value)
}

STDMETHODIMP CSysCADCmd::get_TagType(BSTR Tag, long * Value)
{
  USES_CONVERSION;
	try
	{
		if (SUCCEEDED(sm_pCallback->get_TagType(OLE2T(Tag), Value)))
      return S_OK;
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CSysCADCmd::StartDynamicSolve()
{
	TRANSFER0(StartDynamicSolve, ComState_StepDynamic)
}

STDMETHODIMP CSysCADCmd::StartProbalSolve()
{
  TRANSFER0(StartProbalSolve, ComState_StepProbal)	
}

STDMETHODIMP CSysCADCmd::Start()
{
	try
	{
    DWORD Sts[]={ComState_StepProbal, ComState_StepDynamic};
    InitCOCmdStuff(Sts, sizeof(Sts)/sizeof(Sts[0]));
		if (SUCCEEDED(sm_pCallback->Start()))
      {
      switch (ExecCOCmdStuff(Sts, sizeof(Sts)/sizeof(Sts[0])))
        {
        case SCS_CallReturned:
          // Must return m_dwLastWParam;
          return S_OK;
        case SCS_CallTimeout:
          return ReportError("Synchronous Call Timeout", 2);
        case SCS_CallASync:
          return S_OK;
        }
      }
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CSysCADCmd::Step(long IterMark, double WaitForNext)
{
	try
	{
//    LARGE_INTEGER Freq;
//    QueryPerformanceFrequency(&Freq);
//    LARGE_INTEGER Tmp; 
//    QueryPerformanceCounter(&Tmp); 
//    double Tm=((double)(Tmp.QuadPart))/Freq.QuadPart;
//
//    dbgpln("Time Step %18.6f", Tm);
    
    DWORD Sts[]={ComState_StepProbal, ComState_StepDynamic};
    InitCOCmdStuff(Sts, sizeof(Sts)/sizeof(Sts[0]));
		if (SUCCEEDED(sm_pCallback->Step(IterMark, WaitForNext)))
      {
      switch (ExecCOCmdStuff(Sts, sizeof(Sts)/sizeof(Sts[0])))
        {
        case SCS_CallReturned:
          // Must return m_dwLastWParam;
          return S_OK;
        case SCS_CallTimeout:
          return ReportError("Synchronous Call Timeout", 2);
        case SCS_CallASync:
          return S_OK;
        }
      }
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CSysCADCmd::Stop()
{
	TRANSFER0(Stop, ComState_Stop)	
}

STDMETHODIMP CSysCADCmd::Idle()
{
	TRANSFER0(Idle, ComState_Idle)	
}


STDMETHODIMP CSysCADCmd::Wait(double Seconds)
{
  TRANSFER1(Wait, ComCmd_Wait, Seconds)
}

STDMETHODIMP CSysCADCmd::WaitCount(long Iterations)
{
  TRANSFER1(WaitCount, ComCmd_WaitCount, Iterations)
}

STDMETHODIMP CSysCADCmd::WaitTillSteadyState()
{
  TRANSFER0(WaitTillSteadyState, ComCmd_WaitTillSteadyState)
}

STDMETHODIMP CSysCADCmd::WaitTillStop()
{
  TRANSFER0(WaitTillStop, ComCmd_WaitTillStop)
}

STDMETHODIMP CSysCADCmd::ResetWait()
{
  TRANSFER0(ResetWait, ComCmd_ResetWait)
}


STDMETHODIMP CSysCADCmd::GenerateTagReport(BSTR Filename, BSTR Reportname)
{
  USES_CONVERSION;  
  TRANSFER2(GenerateTagReport, ComCmd_Report_GetValues, OLE2T(Filename), OLE2T(Reportname))
}

STDMETHODIMP CSysCADCmd::GenerateTrendReport(BSTR Filename, BSTR Reportname)
{
  USES_CONVERSION;
  TRANSFER2(GenerateTrendReport, ComCmd_Report_Trend, OLE2T(Filename), OLE2T(Reportname))
}

STDMETHODIMP CSysCADCmd::ProcessSetTags(BSTR Filename, BSTR Reportname)
{
  USES_CONVERSION;
  TRANSFER2(ProcessSetTags, ComCmd_Report_SetValues, OLE2T(Filename), OLE2T(Reportname))
}

STDMETHODIMP CSysCADCmd::SaveAndCloseReport(BSTR Filename)
{
  USES_CONVERSION;
  TRANSFER1(SaveAndCloseReport, ComCmd_Report_SaveAndClose, OLE2T(Filename))
}

STDMETHODIMP CSysCADCmd::ExecuteMacro(BSTR Filename, BSTR Reportname)
{
  USES_CONVERSION;
  TRANSFER2(ExecuteMacro, ComCmd_Report_ExecuteMacro, OLE2T(Filename), OLE2T(Reportname))
}

STDMETHODIMP CSysCADCmd::get_CmpFilename(BSTR *pVal)
{
  TRANSFERPROP(get_CmpFilename, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpFilename(BSTR newVal)
{
  TRANSFERPROP(put_CmpFilename, newVal)
}

STDMETHODIMP CSysCADCmd::get_CmpSort(VARIANT_BOOL *pVal)
{
  TRANSFERPROP(get_CmpSort, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpSort(VARIANT_BOOL newVal)
{
  TRANSFERPROP(put_CmpSort, newVal)
}

STDMETHODIMP CSysCADCmd::get_CmpMaxCount(long *pVal)
{
  TRANSFERPROP(get_CmpMaxCount, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpMaxCount(long newVal)
{
  TRANSFERPROP(put_CmpMaxCount, newVal)
}

STDMETHODIMP CSysCADCmd::get_CmpRelativeTolerance(double *pVal)
{
  TRANSFERPROP(get_CmpRelativeTolerance, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpRelativeTolerance(double newVal)
{
  TRANSFERPROP(put_CmpRelativeTolerance, newVal)
}

STDMETHODIMP CSysCADCmd::get_CmpAbsoluteTolerance(double *pVal)
{
  TRANSFERPROP(get_CmpAbsoluteTolerance, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpAbsoluteTolerance(double newVal)
{
  TRANSFERPROP(put_CmpAbsoluteTolerance, newVal)
}

STDMETHODIMP CSysCADCmd::get_CmpShowDifferentStrings(VARIANT_BOOL *pVal)
{
  TRANSFERPROP(get_CmpShowDifferentStrings, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpShowDifferentStrings(VARIANT_BOOL newVal)
{
  TRANSFERPROP(put_CmpShowDifferentStrings, newVal)
}

STDMETHODIMP CSysCADCmd::get_CmpShowMissingTags(VARIANT_BOOL *pVal)
{
  TRANSFERPROP(get_CmpShowMissingTags, pVal)
}

STDMETHODIMP CSysCADCmd::put_CmpShowMissingTags(VARIANT_BOOL newVal)
{
  TRANSFERPROP(put_CmpShowMissingTags, newVal)
}

STDMETHODIMP CSysCADCmd::CompareReset()
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::CompareScenarios(BSTR Filename1, BSTR Filename2)
{
  USES_CONVERSION;
  TRANSFER2(CompareScenarios, ComCmd_CompareScenarios, OLE2T(Filename1), OLE2T(Filename2))
}

STDMETHODIMP CSysCADCmd::CompareScenarioToCurrent(BSTR Filename)
{
	USES_CONVERSION;
  TRANSFER1(CompareScenarioToCurrent, ComCmd_CompareScenarioToCurrent, OLE2T(Filename))
}

STDMETHODIMP CSysCADCmd::TestScenario(BSTR Filename)
{
	USES_CONVERSION;
  TRANSFER1(TestScenario, ComCmd_TestScenario, OLE2T(Filename))
}

STDMETHODIMP CSysCADCmd::RestartHistorian()
{
	TRANSFER0(RestartHistorian, ComCmd_RestartHistorian)
}

STDMETHODIMP CSysCADCmd::Sleep(double Seconds)
{
	TRANSFER1(Sleep, ComCmd_Sleep, Seconds)
}

STDMETHODIMP CSysCADCmd::Exit()
{
  m_bSyncCalls=false;
  TRANSFER0(Exit, ComCmd_Exit)
//return E_FAIL;
}

STDMETHODIMP CSysCADCmd::OnStart(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}


STDMETHODIMP CSysCADCmd::get_EventsOn(VARIANT_BOOL *pVal)
{
  *pVal=m_bEventsOn;
	return S_OK;
}

STDMETHODIMP CSysCADCmd::put_EventsOn(VARIANT_BOOL newVal)
{
  m_bEventsOn=newVal;
	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnStepProbal(long Flags)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnStepDynamic(long Flags)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnStop(long Flags)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnIdle(long Flags)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::get_IsStopped(VARIANT_BOOL *pVal)
{
	try
	{
		if (SUCCEEDED(sm_pCallback->get_IsStopped(pVal)))
      return S_OK;
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CSysCADCmd::get_IsIdling(VARIANT_BOOL *pVal)
{
	try
	{
		if (SUCCEEDED(sm_pCallback->get_IsIdling(pVal)))
      return S_OK;
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CSysCADCmd::get_IsRunning(VARIANT_BOOL *pVal)
{
	try
	{
		if (SUCCEEDED(sm_pCallback->get_IsRunning(pVal)))
      return S_OK;
    else
      return E_FAIL;
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CSysCADCmd::get_StepSize(double *pVal)
{
  TRANSFERPROP(get_StepSize, pVal)
}

STDMETHODIMP CSysCADCmd::put_StepSize(double newVal)
{
  TRANSFERPROP(put_StepSize, newVal)
}

STDMETHODIMP CSysCADCmd::get_RealTimeOn(VARIANT_BOOL *pVal)
{
	TRANSFERPROP(get_RealTimeOn, pVal)
}

STDMETHODIMP CSysCADCmd::put_RealTimeOn(VARIANT_BOOL newVal)
{
	TRANSFERPROP(put_RealTimeOn, newVal)
}

STDMETHODIMP CSysCADCmd::get_RealTimeMultiplier(double *pVal)
{
  TRANSFERPROP(get_RealTimeMultiplier, pVal)
}

STDMETHODIMP CSysCADCmd::put_RealTimeMultiplier(double newVal)
{
  TRANSFERPROP(put_RealTimeMultiplier, newVal)
}

STDMETHODIMP CSysCADCmd::get_SequenceNo(long *pVal)
{
  TRANSFERPROP(get_SequenceNo, pVal)
}

STDMETHODIMP CSysCADCmd::put_SequenceNo(long newVal)
{
  TRANSFERPROP(put_SequenceNo, newVal)
}

STDMETHODIMP CSysCADCmd::SetAppWndState(long ReqdState)
{
	try
  	{
		if (SUCCEEDED(sm_pCallback->SetAppWndState(ReqdState)))
      {
      return S_OK;
      }
	  }
	catch(...)
	  {
		return E_FAIL;
	  }
	return E_FAIL;
}

STDMETHODIMP CSysCADCmd::LoadBacktrack(BSTR bsBacktrackName)
{
  TRANSFER1(LoadBacktrack, ComCmd_LoadBacktrack, _bstr_t( bsBacktrackName ))
}

STDMETHODIMP CSysCADCmd::SaveBacktrack(BSTR bsBacktrackName)
{
  TRANSFER1(SaveBacktrack, ComCmd_SaveBacktrack, _bstr_t( bsBacktrackName ))
}

STDMETHODIMP CSysCADCmd::VariableSetReplayItem(long SequenceNo, BSTR Tag, VARIANT Value, BSTR CnvTxt)
  {
  USES_CONVERSION;
  TRANSFER4(VariableSetReplayItem, ComCmd_VariableSetReplayItem, SequenceNo, OLE2T(Tag), Value, OLE2T(CnvTxt))
	return S_OK;
  }

STDMETHODIMP CSysCADCmd::VariableSetReplayCtrl(long Command, long Param)
  {
  USES_CONVERSION;
  TRANSFER2(VariableSetReplayCtrl, ComCmd_VariableSetReplayCtrl, Command, Param);
	return S_OK;
  }

STDMETHODIMP CSysCADCmd::OnSnapshotLoaded(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnSnapshotSaved(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnScenarioLoaded(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnScenarioSaved(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnBacktrackLoaded(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnBacktrackSaved(long result)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSysCADCmd::OnVariableSet(long Source, long SequenceNo, BSTR Tag, VARIANT Value, BSTR CnvTxt)
{
	// TODO: Add your implementation code here

	return S_OK;
}


STDMETHODIMP CSysCADCmd::InsertNode(BSTR Class, BSTR Tag, double X, double Y, double Z, double XScale, double YScale, double Rotation)
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->InsertNode(_bstr_t(Class), _bstr_t(Tag), X, Y, Z, XScale, YScale, Rotation)))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }
STDMETHODIMP CSysCADCmd::InsertLink(BSTR Class, BSTR Tag, BSTR SrcTag, BSTR SrcIO, BSTR DstTag, BSTR DstIO, double Xs, double Ys, double Zs, double Xd, double Yd, double Zd)
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->InsertLink(_bstr_t(Class), _bstr_t(Tag), _bstr_t(SrcTag), _bstr_t(SrcIO), _bstr_t(DstTag), _bstr_t(DstIO), Xs, Ys, Zs, Xd, Yd, Zd)))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }

STDMETHODIMP CSysCADCmd::get_GrfWndIndex(long *pVal)
  {
  TRANSFERPROP(get_GrfWndIndex, pVal)
  }

STDMETHODIMP CSysCADCmd::put_GrfWndIndex(long newVal)
  {
  TRANSFERPROP(put_GrfWndIndex, newVal)
  }
STDMETHODIMP CSysCADCmd::ZoomWindowFull()
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->ZoomWindowFull()))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }

STDMETHODIMP CSysCADCmd::CreateUnit(BSTR Class, BSTR Tag )
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->CreateUnit(_bstr_t(Class), _bstr_t(Tag))))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }
STDMETHODIMP CSysCADCmd::CreateUnitGrf(BSTR Tag, double X, double Y, double Z, BSTR Symbol, double XScale, double YScale, double Rotation )
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->CreateUnitGrf(_bstr_t(Tag), X, Y, Z, _bstr_t(Symbol), XScale, YScale, Rotation)))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }
STDMETHODIMP CSysCADCmd::CreateLink(BSTR Class, BSTR Tag, BSTR SrcTag, BSTR SrcIO, BSTR DstTag, BSTR DstIO )
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->CreateLink(_bstr_t(Class), _bstr_t(Tag), _bstr_t(SrcTag), _bstr_t(SrcIO), _bstr_t(DstTag), _bstr_t(DstIO))))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }
STDMETHODIMP CSysCADCmd::CreateLinkGrf(BSTR Tag, long DrawLineMethod )
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->CreateLinkGrf(_bstr_t(Tag), DrawLineMethod)))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }
STDMETHODIMP CSysCADCmd::CreateLinkGrfLine(BSTR Tag, double Xs, double Ys, double Zs, double Xd, double Yd, double Zd )
  {
  try
    {
		if (SUCCEEDED(sm_pCallback->CreateLinkGrfLine(_bstr_t(Tag), Xs, Ys, Zs, Xd, Yd, Zd)))
      return S_OK;
    else
      return E_FAIL;
    }
  catch (...)
    {
    return E_FAIL;
    }
  }
