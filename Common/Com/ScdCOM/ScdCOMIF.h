// interface for the CScdCOMBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDCALLBACK_H__3AD5C06F_5A66_434A_934C_1173E6316E20__INCLUDED_)
#define AFX_COMMANDCALLBACK_H__3AD5C06F_5A66_434A_934C_1173E6316E20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>
#include "..\ScdIF\scdcomcmds.h"
#include "..\ScdIF\scdcomevts.h"

#if defined(__SCDCOMCALLBACK_CPP) || defined(__SCDCOMWRAPPER_CPP) || defined(__SYSCADCOM_CPP)
  #define DllImportExport __declspec(dllexport)
#elif !defined(SCDCOM)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//#include <ScdCOMWrapper.h>

class DllImportExport CSysCADCmdHookBack
  {
  public:
  // Call to Client
  virtual void DoEventMsg(UINT e, LPARAM l)=0;
  virtual HRESULT ReportError(LPCTSTR szDesc, UINT Code)=0;
  };


//=================

const UINT  SCS_CallReturned   = 1;
const UINT  SCS_CallTimeout    = 2;
const UINT  SCS_CallASync      = 3;

//=================
class DllImportExport CScdCOMBase; // forward

class DllImportExport CScdCOMWork
{
protected:
  static DWORD WINAPI StaticThreadProc( LPVOID lpv );
  void Register( CScdCOMBase *i_pCallback, DWORD ComEvtMsg);
  CScdCOMWork();
  virtual ~CScdCOMWork();

public:
  void UnRegister();
  HRESULT UpdateRegistry(HINSTANCE hInst, BOOL bRegister);//, LPWSTR pwszAppID, LPWSTR pwszTypeLib);
  HRESULT ReportError(LPCTSTR szDesc, UINT Code);

private:
  //inter-thread comms
  CScdCOMBase*  m_pCallback;
  HINSTANCE         m_hResInst;
  ULONG             m_lResCBRegID;
  HANDLE            m_hThreadProc;
  HANDLE            m_evGoDead;
  DWORD             m_dwRegClsObj;
  DWORD             m_dwRegActObj;
  DWORD             m_dwThreadId;
  DWORD             m_dwEvtMsgId;
  CScdCOEventSender      m_EventSender;

  void DoStartupStuff();
  void DoShutdownStuff();
  DWORD WINAPI ThreadProc();
  static  IID   smIID_IHelper;
  static  IID   smIID_IBackdoor;
  static  CLSID smCLSID_Helper;
};

//=================

class DllImportExport CScdCOMBase : public CScdCOMWork
{
protected:
  CSysCADCmdHookBack * m_pHook;

public:
  void Register(DWORD ComEvtMsg) { CScdCOMWork::Register(this, ComEvtMsg);};
  virtual CLSID& clsidGetCLSID()=0;
  virtual LPWSTR pwszGetProgID()=0;
  virtual LPWSTR pwszGetVIProgID()=0;
  virtual LPWSTR pwszGetClassDesc()=0;

  virtual HRESULT get_UpNAbout(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT WaitUpNAbout(double TimeOut)=0;
  //virtual HRESULT get_SyncCallsOn(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;         // Handled in Wrapper
  //virtual HRESULT put_SyncCallsOn(/*[in]*/ VARIANT_BOOL newVal)=0;                 // Handled in Wrapper
  //virtual HRESULT get_SyncCallsTimeOut(/*[out, retval]*/ double *pVal)=0;  // Handled in Wrapper
  //virtual HRESULT put_SyncCallsTimeOut(/*[in]*/ double newVal)=0;          // Handled in Wrapper
  virtual HRESULT LoadProject(/*[in]*/ LPCTSTR bsProjectName)=0;
  virtual HRESULT SaveProject(/*[in]*/ LPCTSTR bsProjectName)=0;
  virtual HRESULT LoadSnapshot(/*[in]*/ LPCTSTR bsSnapshotName)=0;
  virtual HRESULT SaveSnapshot(/*[in]*/ LPCTSTR bsSnapshotName)=0;
  virtual HRESULT LoadScenario(/*[in]*/ LPCTSTR bsScenarioName)=0;
  virtual HRESULT SaveScenario(/*[in]*/ LPCTSTR bsScenarioName)=0;
  virtual HRESULT LoadBacktrack(/*[in]*/ LPCTSTR bsBacktrackName)=0;
  virtual HRESULT SaveBacktrack(/*[in]*/ LPCTSTR bsBacktrackName)=0;
  virtual HRESULT CloseProject()=0;
  virtual HRESULT SetTag(/*[in]*/ LPCTSTR Tag, /*[in]*/ VARIANT *Value)=0;
  virtual HRESULT GetTag(/*[in]*/ LPCTSTR Tag, /*[out]*/ VARIANT *Value)=0;
  virtual HRESULT StartDynamicSolve()=0;
  virtual HRESULT StartProbalSolve()=0;
  virtual HRESULT Start()=0;
  virtual HRESULT Step(long IterMark, double WaitForNext)=0;
  virtual HRESULT Stop()=0;
  virtual HRESULT Idle()=0;
  virtual HRESULT Wait(/*[in]*/ double Seconds)=0;
  virtual HRESULT WaitCount(/*[in]*/ long Iterations)=0;
  virtual HRESULT WaitTillSteadyState()=0;
  virtual HRESULT WaitTillStop()=0;
  virtual HRESULT ResetWait()=0;
  virtual HRESULT GenerateTagReport(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname)=0;
  virtual HRESULT GenerateTrendReport(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname)=0;
  virtual HRESULT ProcessSetTags(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname)=0;
  virtual HRESULT SaveAndCloseReport(/*[in]*/ LPCTSTR Filename)=0;
  virtual HRESULT ExecuteMacro(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname)=0;
  virtual HRESULT get_CmpFilename(/*[out, retval]*/ BSTR *pVal)=0;
  virtual HRESULT put_CmpFilename(/*[in]*/ BSTR newVal)=0;
  virtual HRESULT get_CmpSort(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT put_CmpSort(/*[in]*/ VARIANT_BOOL newVal)=0;
  virtual HRESULT get_CmpMaxCount(/*[out, retval]*/ long *pVal)=0;
  virtual HRESULT put_CmpMaxCount(/*[in]*/ long newVal)=0;
  virtual HRESULT get_CmpRelativeTolerance(/*[out, retval]*/ double *pVal)=0;
  virtual HRESULT put_CmpRelativeTolerance(/*[in]*/ double newVal)=0;
  virtual HRESULT get_CmpAbsoluteTolerance(/*[out, retval]*/ double *pVal)=0;
  virtual HRESULT put_CmpAbsoluteTolerance(/*[in]*/ double newVal)=0;
  virtual HRESULT get_CmpShowDifferentStrings(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT put_CmpShowDifferentStrings(/*[in]*/ VARIANT_BOOL newVal)=0;
  virtual HRESULT get_CmpShowMissingTags(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT put_CmpShowMissingTags(/*[in]*/ VARIANT_BOOL newVal)=0;
  virtual HRESULT CompareReset()=0;
  virtual HRESULT CompareScenarios(/*[in]*/ LPCTSTR Filename1, /*[in]*/ LPCTSTR Filename2)=0;
  virtual HRESULT CompareScenarioToCurrent(/*[in]*/ LPCTSTR Filename)=0;
  virtual HRESULT TestScenario(/*[in]*/ LPCTSTR Filename)=0;
  virtual HRESULT RestartHistorian()=0;
  virtual HRESULT Sleep(double Seconds)=0;
  virtual HRESULT Exit()=0;
  //virtual HRESULT get_EventsOn(/*[out, retval]*/ VARIANT_BOOL *pVal)=0; // Handled in Wrapper
  //virtual HRESULT put_EventsOn(/*[in]*/ VARIANT_BOOL newVal)=0;         // Handled in Wrapper
  virtual HRESULT get_IsStopped(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT get_IsIdling(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT get_IsRunning(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT get_StepSize(/*[out, retval]*/ double *pVal)=0;
  virtual HRESULT put_StepSize(/*[in]*/ double newVal)=0;
  virtual HRESULT get_RealTimeOn(/*[out, retval]*/ VARIANT_BOOL *pVal)=0;
  virtual HRESULT put_RealTimeOn(/*[in]*/ VARIANT_BOOL newVal)=0;
  virtual HRESULT get_RealTimeMultiplier(/*[out, retval]*/ double *pVal)=0;
  virtual HRESULT put_RealTimeMultiplier(/*[in]*/ double newVal)=0;
  virtual HRESULT get_SequenceNo(/*[out, retval]*/ long *pVal)=0;
  virtual HRESULT put_SequenceNo(/*[in]*/ long newVal)=0;
  virtual HRESULT SetAppWndState(/*[out, retval]*/ long ReqdState)=0;
  virtual HRESULT VariableSetReplayItem(/*[in]*/ long SequenceNo, /*[in]*/ LPCTSTR Tag, /*[in]*/ VARIANT Value, /*[in]*/ LPCTSTR CnvTxt)=0;
  virtual HRESULT VariableSetReplayCtrl(/*[in]*/ long Command, /* [in] */ long Param)=0;
  virtual HRESULT get_TagValue(LPCTSTR Tag, VARIANT *pVal)=0;
  virtual HRESULT put_TagValue(LPCTSTR Tag, VARIANT newVal)=0;
  virtual HRESULT get_TagType(LPCTSTR Tag, long *pVal)=0;

  virtual HRESULT InsertNode(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation )=0;
  virtual HRESULT InsertLink(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag, /*[in]*/ LPCTSTR SrcTag, /*[in]*/ LPCTSTR SrcIO, /*[in]*/ LPCTSTR DstTag, /*[in]*/ LPCTSTR DstIO, /*[in]*/ double Xs, /*[in]*/ double Ys, /*[in]*/ double Zs, /*[in]*/ double Xd, /*[in]*/ double Yd, /*[in]*/ double Zd)=0;
  virtual HRESULT get_GrfWndIndex(/*[out, retval]*/ long *pVal)=0;
  virtual HRESULT put_GrfWndIndex(/*[in]*/ long newVal)=0;
  virtual HRESULT ZoomWindowFull()=0;
  virtual HRESULT CreateUnit(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag )=0;
  virtual HRESULT CreateUnitGrf(/*[in]*/ LPCTSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ LPCTSTR Symbol, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation )=0;
  virtual HRESULT CreateLink(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag, /*[in]*/ LPCTSTR SrcTag, /*[in]*/ LPCTSTR SrcIO, /*[in]*/ LPCTSTR DstTag, /*[in]*/ LPCTSTR DstIO)=0;
  virtual HRESULT CreateLinkGrf(/*[in]*/ LPCTSTR Tag, /*[in]*/ long DrawLineMethod)=0;
  virtual HRESULT CreateLinkGrfLine(/*[in]*/ LPCTSTR Tag, /*[in]*/ double Xs, /*[in]*/ double Ys, /*[in]*/ double Zs, /*[in]*/ double Xd, /*[in]*/ double Yd, /*[in]*/ double Zd)=0;

  CScdCOMBase();
  virtual ~CScdCOMBase();
  // Callback to Client
  virtual void DoEventMsg(UINT e, LPARAM l);
  virtual HRESULT ReportError(LPCTSTR szDesc, UINT Code);
  void Initialise(CSysCADCmdHookBack* Hook);

};


class DllImportExport EVM_VariableSet
  {
  public:
    long            m_lSource;
    long            m_lSequenceNo;
    BSTR            m_bsTag;
    VARIANT         m_vValue;
    BSTR            m_bsCnvTxt;
  };


#undef DllImportExport

#endif // !defined(AFX_COMMANDCALLBACK_H__3AD5C06F_5A66_434A_934C_1173E6316E20__INCLUDED_)
