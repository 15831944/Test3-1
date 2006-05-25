// ScdDynamic.cpp : Implementation of CScdDynamic
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdDynamic.h"

/////////////////////////////////////////////////////////////////////////////
// CScdDynamic

STDMETHODIMP CScdDynamic::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdDynamic
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdDynamic::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

STDMETHODIMP CScdDynamic::get_IsStopped(VARIANT_BOOL *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_TheRunMngr.Stopped();  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::get_IsIdling(VARIANT_BOOL *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_TheRunMngr.Idling();  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::get_IsRunning(VARIANT_BOOL *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_TheRunMngr.Running();  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::get_StepSize(double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_Exec.StepSizeMax.Seconds;  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::put_StepSize(double newVal)
  {
  dllSCD_COMENTRY(long)
    {
    gs_Exec.StepSizeMax=newVal;  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::get_RealTime(VARIANT_BOOL *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_Exec.RealTime();  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::put_RealTime(VARIANT_BOOL newVal)
  {
  dllSCD_COMENTRY(long)
    {
    gs_Exec.SetRealTime(newVal!=0);  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::get_RealTimeMultiplier(double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_Exec.RealTimeMult();  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::put_RealTimeMultiplier(double newVal)
  {
  dllSCD_COMENTRY(long)
    {
    gs_Exec.SetRealTimeMult(newVal);  
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::Start()
  {
  dllSCD_COMENTRY(long)
    {
    long  Evts[]={ComState_Stop, ComState_StepDynamic}; 
    switch (PostComCmd(ComCmd_StartDynamicSolve, Evts, sizeof(Evts)/sizeof(Evts[0]), 0))//(long)Info.lParam))
      {                                                             
      case CScdCOCmdBase::SS_CallTimeout:                                         
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:                                        
      case CScdCOCmdBase::SS_CallASync:                                           
        break;                                                
      }                                                             
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::Step(long IterMark, double WaitForNext)
  {
  dllSCD_COMENTRY(long)
    {
    gs_TheRunMngr.SetDynamicMode();
    CDoOneStepInfo Info;
    Info.bHoldAdv=false;
    Info.lWaitForNextMS=(long)(WaitForNext*1000.0);

    long  Evts[]={ComState_Idle, ComState_Stop, ComState_StepProbal, ComState_StepDynamic}; 
    switch (PostComCmd(ComCmd_Step, Evts, sizeof(Evts)/sizeof(Evts[0]), (long)Info.lParam))
      {                                                             
      case CScdCOCmdBase::SS_CallTimeout:                                         
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:                                        
      case CScdCOCmdBase::SS_CallASync:                                           
        break;                                                
      }                                                             
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::Stop()
  {
  dllSCD_COMENTRY(long)
    {
    long  Evts[]={ComState_Stop}; 
    switch (PostComCmd(ComCmd_Stop, Evts, sizeof(Evts)/sizeof(Evts[0]), 0))
      {                                                             
      case CScdCOCmdBase::SS_CallTimeout:                                         
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:                                        
      case CScdCOCmdBase::SS_CallASync:                                           
        break;                                                
      }                                                             
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::Idle()
  {
  dllSCD_COMENTRY(long)
    {
    long  Evts[]={ComState_Idle, ComState_Stop, ComState_StepProbal, ComState_StepDynamic}; 
    switch (PostComCmd(ComCmd_Idle, Evts, sizeof(Evts)/sizeof(Evts[0]), 0))
      {                                                             
      case CScdCOCmdBase::SS_CallTimeout:                                         
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:                                        
      case CScdCOCmdBase::SS_CallASync:                                           
        break;                                                
      }                                                             
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdDynamic::RunToSteadyState()
  {
  dllSCD_COMENTRY(long)
    {
    long  Evts[]={ComState_Stop, ComState_StepDynamic}; 
    switch (PostComCmd(ComCmd_RunToSteadyState, Evts, sizeof(Evts)/sizeof(Evts[0]), 0))//(long)Info.lParam))
      {                                                             
      case CScdCOCmdBase::SS_CallTimeout:                                         
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:                                        
      case CScdCOCmdBase::SS_CallASync:                                           
        break;                                                
      }                                                             
    }
  SCD_COMEXIT
  };
