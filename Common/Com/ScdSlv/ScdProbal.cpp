// ScdProbal.cpp : Implementation of CScdProbal
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdProbal.h"

/////////////////////////////////////////////////////////////////////////////
// CScdProbal

STDMETHODIMP CScdProbal::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdProbal
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdProbal::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };


STDMETHODIMP CScdProbal::get_IsStopped( VARIANT_BOOL *  pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_TheRunMngr.Stopped();  
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdProbal::get_IsIdling( VARIANT_BOOL *  pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_TheRunMngr.Idling();  
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdProbal::get_IsRunning( VARIANT_BOOL *  pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    * pVal=gs_TheRunMngr.Running();  
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdProbal::Start()
  {
  dllSCD_COMENTRY(long)
    {
    long  Evts[]={ComState_Stop, ComState_StepProbal}; 
    switch (PostComCmd(ComCmd_StartProbalSolve, Evts, sizeof(Evts)/sizeof(Evts[0]), 0))
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
  }
STDMETHODIMP CScdProbal::Step( long  IterMark,  double  WaitForNext)
  {
  dllSCD_COMENTRY(long)
    {
    gs_TheRunMngr.SetProbalMode();
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
  }
STDMETHODIMP CScdProbal::Stop()
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
  }
STDMETHODIMP CScdProbal::Idle()
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
  }
