// SysCADCOM.h : Declaration of the CHelper

#ifndef __SYSCADCMD_H_
#define __SYSCADCMD_H_

//extern const IID DIID__ISysCADCmdEvents;
#include "stdafx.h"

#include "resource.h"       // main symbols
#include "ScdCOMIF.h"
#include "ScdCOMCP.h"
/////////////////////////////////////////////////////////////////////////////
// CHelper
class ATL_NO_VTABLE CSysCADCmd :
  public CSysCADCmdHookBack,
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CSysCADCmd, &CLSID_SysCADCmd>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CSysCADCmd>,
	public IDispatchImpl<ISysCADCmd, &IID_ISysCADCmd, &LIBID_SysCADLib>,
	public IDispatchImpl<ISysCADEdit, &IID_ISysCADEdit, &LIBID_SysCADLib>,
	public IDispatchImpl<IBackdoor, &IID_IBackdoor, &LIBID_SysCADLib>,
  public IDispatchImpl<_ISysCADCmdEvents, &DIID__ISysCADCmdEvents, &LIBID_SysCADLib>,
	public CProxy_ISysCADCmdEvents< CSysCADCmd >,
  public IProvideClassInfo2Impl<&CLSID_SysCADCmd, &DIID__ISysCADCmdEvents, &LIBID_SysCADLib, 0,1>
    {
public:
	CSysCADCmd()
	{
  m_bSyncCalls=true; 
  m_bEventsOn=true; 
  for (UINT i=0; i<MaxComEvents; i++)
    {
    m_hEventRcvd[i]=INVALID_HANDLE_VALUE;
    m_lEventLParam[i]=0;
    }
  m_dwSyncTimeOut=1000000;
	}

	~CSysCADCmd()
	{
  for (UINT i=0; i<MaxComEvents; i++)
   if (m_hEventRcvd[i]!=INVALID_HANDLE_VALUE)
     CloseHandle(m_hEventRcvd[i]);
	}

HRESULT FinalConstruct();
void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_SYSCADCMD)

DECLARE_PROTECT_FINAL_CONSTRUCT()
//DECLARE_CLASSFACTORY_SINGLETON(CSysCADCmd)
DECLARE_CLASSFACTORY()

BEGIN_COM_MAP(CSysCADCmd)
	COM_INTERFACE_ENTRY(ISysCADCmd)
	COM_INTERFACE_ENTRY(ISysCADEdit)
//DEL 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY2(IDispatch, ISysCADCmd)
	COM_INTERFACE_ENTRY(IBackdoor)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(_ISysCADCmdEvents)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
//	COM_INTERFACE_ENTRY(ISysCADCmd)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CSysCADCmd)
CONNECTION_POINT_ENTRY(DIID__ISysCADCmdEvents)
END_CONNECTION_POINT_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
// ISysCADEdit
  STDMETHOD(get_GrfWndIndex)(/*[out, retval]*/ long *pVal);
  STDMETHOD(put_GrfWndIndex)(/*[in]*/ long newVal);
  STDMETHOD(ZoomWindowFull)();
  STDMETHOD(InsertNode)(/*[in]*/ BSTR Class, /*[in]*/ BSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation );
  STDMETHOD(InsertLink)(/*[in]*/ BSTR Class, /*[in]*/ BSTR Tag, /*[in]*/ BSTR SrcTag, /*[in]*/ BSTR SrcIO, /*[in]*/ BSTR DstTag, /*[in]*/ BSTR DstIO, /*[in]*/ double Xs, /*[in]*/ double Ys, /*[in]*/ double Zs, /*[in]*/ double Xd, /*[in]*/ double Yd, /*[in]*/ double Zd);
  STDMETHOD(CreateUnit)(/*[in]*/ BSTR Class, /*[in]*/ BSTR Tag );
  STDMETHOD(CreateUnitGrf)(/*[in]*/ BSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ BSTR Symbol, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation );
  STDMETHOD(CreateLink)(/*[in]*/ BSTR Class, /*[in]*/ BSTR Tag, /*[in]*/ BSTR SrcTag, /*[in]*/ BSTR SrcIO, /*[in]*/ BSTR DstTag, /*[in]*/ BSTR DstIO );
  STDMETHOD(CreateLinkGrf)(/*[in]*/ BSTR Tag, /*[in]*/ long DrawLineMethod);
  STDMETHOD(CreateLinkGrfLine)(/*[in]*/ BSTR Tag, /*[in]*/ double Xs, /*[in]*/ double Ys, /*[in]*/ double Zs, /*[in]*/ double Xd, /*[in]*/ double Yd, /*[in]*/ double Zd);
// ISysCADCmd
	STDMETHOD(VariableSetReplayCtrl)(/*[in]*/ long Command, /* [in] */ long Param);
	STDMETHOD(VariableSetReplayItem)(/*[in]*/ long SequenceNo, /*[in]*/ BSTR Tag, /*[in]*/ VARIANT Value,/* [in] */ BSTR CnvTxt);
	STDMETHOD(OnVariableSet)(/*[in]*/ long Source, /*[in]*/ long SequenceNo, /*[in]*/ BSTR Tag, /*[in]*/ VARIANT Value,/* [in] */ BSTR CnvTxt);
	STDMETHOD(OnBacktrackSaved)(/*[in]*/ long result);
	STDMETHOD(OnBacktrackLoaded)(/*[in]*/ long result);
	STDMETHOD(OnScenarioSaved)(/*[in]*/ long result);
	STDMETHOD(OnScenarioLoaded)(/*[in]*/ long result);
	STDMETHOD(OnSnapshotSaved)(/*[in]*/ long result);
	STDMETHOD(OnSnapshotLoaded)(/*[in]*/ long result);
	STDMETHOD(SaveBacktrack)(/*[in]*/ BSTR bsBacktrackName);
	STDMETHOD(LoadBacktrack)(/*[in]*/ BSTR bsBacktrackName);
	STDMETHOD(SetAppWndState)(long ReqdState);
	STDMETHOD(get_SequenceNo)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_SequenceNo)(/*[in]*/ long newVal);
	STDMETHOD(OnIdle)(long Flags);
	STDMETHOD(OnStop)(long Flags);
	STDMETHOD(OnStepDynamic)(long Flags);
	STDMETHOD(OnStepProbal)(long Flags);
	STDMETHOD(OnStart)(long result);

  STDMETHOD(get_UpNAbout)(/*[out, retval]*/ VARIANT_BOOL *pVal);
  STDMETHOD(WaitUpNAbout)(double TimeOut);
  STDMETHOD(get_SyncCallsOn)(/*[out, retval]*/ VARIANT_BOOL *pVal);
  STDMETHOD(put_SyncCallsOn)(/*[in]*/ VARIANT_BOOL newVal);
  STDMETHOD(get_SyncCallsTimeOut)(/*[out, retval]*/ double *pVal);
  STDMETHOD(put_SyncCallsTimeOut)(/*[in]*/ double newVal);
  STDMETHOD(LoadProject)(/*[in]*/ BSTR bsProjectName);
  STDMETHOD(SaveProject)(/*[in]*/ BSTR bsProjectName);
  STDMETHOD(LoadSnapshot)(/*[in]*/ BSTR bsSnapshotName);
  STDMETHOD(SaveSnapshot)(/*[in]*/ BSTR bsSnapshotName);
  STDMETHOD(LoadScenario)(/*[in]*/ BSTR bsScenarioName);
  STDMETHOD(SaveScenario)(/*[in]*/ BSTR bsScenarioName);
  STDMETHOD(CloseProject)();
  STDMETHOD(SetTag)(/*[in]*/ BSTR Tag, /*[in]*/ VARIANT *Value);
  STDMETHOD(GetTag)(/*[in]*/ BSTR Tag, /*[out]*/ VARIANT *Value);
  STDMETHOD(put_TagValue)(/*[in]*/ BSTR Tag, /*[in]*/ VARIANT Value);
  STDMETHOD(get_TagValue)(/*[in]*/ BSTR Tag, /*[out]*/ VARIANT *Value);
  STDMETHOD(get_TagType)(/*[in]*/ BSTR Tag, /*[out]*/ long *Value);
  STDMETHOD(StartDynamicSolve)();
  STDMETHOD(StartProbalSolve)();
  STDMETHOD(Start)();
  STDMETHOD(Step)(long IterMark, double WaitForNext);
  STDMETHOD(Stop)();
  STDMETHOD(Idle)();
  STDMETHOD(Wait)(/*[in]*/ double Seconds);
  STDMETHOD(WaitCount)(/*[in]*/ long Iterations);
  STDMETHOD(WaitTillSteadyState)();
  STDMETHOD(WaitTillStop)();
  STDMETHOD(ResetWait)();
  STDMETHOD(GenerateTagReport)(/*[in]*/ BSTR Filename, /*[in]*/ BSTR Reportname);
  STDMETHOD(GenerateTrendReport)(/*[in]*/ BSTR Filename, /*[in]*/ BSTR Reportname);
  STDMETHOD(ProcessSetTags)(/*[in]*/ BSTR Filename, /*[in]*/ BSTR Reportname);
  STDMETHOD(SaveAndCloseReport)(/*[in]*/ BSTR Filename);
  STDMETHOD(ExecuteMacro)(/*[in]*/ BSTR Filename, /*[in]*/ BSTR Reportname);
  STDMETHOD(get_CmpFilename)(/*[out, retval]*/ BSTR *pVal);
  STDMETHOD(put_CmpFilename)(/*[in]*/ BSTR newVal);
  STDMETHOD(get_CmpSort)(/*[out, retval]*/ VARIANT_BOOL *pVal);
  STDMETHOD(put_CmpSort)(/*[in]*/ VARIANT_BOOL newVal);
  STDMETHOD(get_CmpMaxCount)(/*[out, retval]*/ long *pVal);
  STDMETHOD(put_CmpMaxCount)(/*[in]*/ long newVal);
  STDMETHOD(get_CmpRelativeTolerance)(/*[out, retval]*/ double *pVal);
  STDMETHOD(put_CmpRelativeTolerance)(/*[in]*/ double newVal);
  STDMETHOD(get_CmpAbsoluteTolerance)(/*[out, retval]*/ double *pVal);
  STDMETHOD(put_CmpAbsoluteTolerance)(/*[in]*/ double newVal);
  STDMETHOD(get_CmpShowDifferentStrings)(/*[out, retval]*/ VARIANT_BOOL *pVal);
  STDMETHOD(put_CmpShowDifferentStrings)(/*[in]*/ VARIANT_BOOL newVal);
  STDMETHOD(get_CmpShowMissingTags)(/*[out, retval]*/ VARIANT_BOOL *pVal);
  STDMETHOD(put_CmpShowMissingTags)(/*[in]*/ VARIANT_BOOL newVal);
  STDMETHOD(CompareReset)();
  STDMETHOD(CompareScenarios)(/*[in]*/ BSTR Filename1, /*[in]*/ BSTR Filename2);
  STDMETHOD(CompareScenarioToCurrent)(/*[in]*/ BSTR Filename);
  STDMETHOD(TestScenario)(/*[in]*/ BSTR Filename);
  STDMETHOD(RestartHistorian)();
  STDMETHOD(Sleep)(double Seconds);
  STDMETHOD(Exit)();
	STDMETHOD(get_EventsOn)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_EventsOn)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_IsStopped)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_IsIdling)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_IsRunning)(/*[out, retval]*/ VARIANT_BOOL *pVal);
  STDMETHOD(get_StepSize)(/*[out, retval]*/ double *pVal);
  STDMETHOD(put_StepSize)(/*[in]*/ double newVal);
  STDMETHOD(get_RealTimeOn)(/*[out, retval]*/ VARIANT_BOOL *pVal); 
  STDMETHOD(put_RealTimeOn)(/*[in]*/ VARIANT_BOOL newVal);
  STDMETHOD(get_RealTimeMultiplier)(/*[out, retval]*/ double *pVal);
  STDMETHOD(put_RealTimeMultiplier)(/*[in]*/ double newVal);
  // IBackdoor
	STDMETHOD(Register)(ULONG i_dwCastedCallback);
	STDMETHOD(GetResourceHInstance)(/*[out, ref]*/ULONG *hResInst);
	STDMETHOD(GetResourceCBRegID)(/*[out,ref]*/ ULONG *ID);

private:
	static CScdCOMBase	*sm_pCallback;

  VARIANT_BOOL   m_bSyncCalls;    // Calls are syncronous if true
  VARIANT_BOOL   m_bEventsOn;     // Events are generated

  // these events are triggered when event received
  HANDLE m_hEventRcvd[MaxComEvents];
  LPARAM m_lEventLParam[MaxComEvents];
    
  DWORD  m_dwSyncTimeOut; // Timeout wait for sync calls; 
  
  // Utility Methods
protected:
  void DoEventMsg(UINT e, LPARAM l);
  void InitCOCmdStuff(DWORD EvId);
  UINT ExecCOCmdStuff(DWORD EvId);
  void InitCOCmdStuff(DWORD *EvIds, DWORD nEvts);
  UINT ExecCOCmdStuff(DWORD *EvIds, DWORD nEvts);

  HRESULT ReportError(LPCTSTR szDesc, UINT Code)
    {
    USES_CONVERSION;
    CLSID & rguid=sm_pCallback->clsidGetCLSID();
    HRESULT hRes=MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, Code);
    return AtlReportError(rguid, T2OLE(szDesc), IID_ISysCADCmd, hRes);
    };
};

#endif //__SYSCADCMD_H_
