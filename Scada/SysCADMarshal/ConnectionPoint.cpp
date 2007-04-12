//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ConnectionPoint.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Callback class for OPC Data Access 2.0
//
//
//**************************************************************************
#include "stdafx.h"
//#include "connectionpoint.h"
#include "slotmngr.h"

#define dbgOPC  1

//**************************************************************************

// Called by the server at the update rate when data changes
STDMETHODIMP COPC_Callback::OnDataChange
                            (
                              DWORD     Transid,
                              OPCHANDLE grphandle,
                              HRESULT   masterquality,
                              HRESULT   mastererror,
                              DWORD     count,
                              OPCHANDLE *clienthandles,
                              VARIANT   *values,
                              WORD      *quality,
                              FILETIME  *time,
                              HRESULT   *errors
                            )
  {
  for(DWORD index = 0; index < count; index++)
    {
    if(SUCCEEDED(errors[index]))
      {
      long hClient=clienthandles[index];
      
      switch (values[index].vt)
        {
        case VT_R8:
          if (!Valid(values[index].dblVal))
            {
            values[index].dblVal=0;
            LogError("Marshal",0,"Bad Double Value received from OPC");
            }
          break;
        case VT_R4:
          if (!Valid(values[index].fltVal))
            {
            values[index].fltVal=0;
            LogError("Marshal",0,"Bad Single Value received from OPC");
            }
          break;
        }

      //?MAKE_INVALIDFLTOPCRASH();

      CFullValue FV(values[index], quality[index], time[index]);
      gs_SlotMngr.AppendChange(eCSD_Device, -1, eCSD_Slot, hClient, Transid, FV, NULL);
      gs_SlotMngr.m_Stats.m_nDeviceChgsIn++;
      }
    }
  if (count)
    gs_SlotMngr.RequestQueueFlush();
  return S_OK;
  }

//**************************************************************************

// Called by the server to complete an ASyncIO2::Read
STDMETHODIMP COPC_Callback::OnReadComplete
                            (
                              DWORD     Transid,
                              OPCHANDLE grphandle,
                              HRESULT   masterquality,
                              HRESULT   mastererror,
                              DWORD     count,
                              OPCHANDLE *clienthandles,
                              VARIANT   *values,
                              WORD      *quality,
                              FILETIME  *time,
                              HRESULT   *errors
                            )
  {
  for(DWORD index = 0; index < count; index++)
    {
    if(SUCCEEDED(errors[index]))
      {
      long hClient=clienthandles[index];
      CFullValue FV(values[index], quality[index], time[index]);
      gs_SlotMngr.AppendChange(eCSD_Device, -1, eCSD_Slot, hClient, Transid, FV, NULL);
      gs_SlotMngr.m_Stats.m_nDeviceChgsIn++;
      }
    }
  if (count)
    gs_SlotMngr.RequestQueueFlush();
  return S_OK;
  }

//**************************************************************************

// Called by the server to complete an ASyncIO2::Write
STDMETHODIMP COPC_Callback::OnWriteComplete
                            (
                              DWORD     Transid,
                              OPCHANDLE grphandle,
                              HRESULT   mastererr,
                              DWORD     count,
                              OPCHANDLE *clienthandles,
                              HRESULT   *errors
                            )
  {
  //ASSERT(Transid == 2); // test only
  for(DWORD index = 0; index < count; index++)
    {
    if(FAILED(errors[index]))
      {
//      COPC_Item *pItem = (COPC_Item *) clienthandles[index];
//
//      //         CSLock lock(&pItem->m_pAccess->m_CSect);
//      //         PostMessage(theDoc->hView, WM_OPCWRITE, errors[index], 0);
      }
    }

  return S_OK;
  }

//**************************************************************************

// Called by the server to complete an ASyncIO2::Cancel
STDMETHODIMP COPC_Callback::OnCancelComplete(DWORD transid, OPCHANDLE grphandle)
  {
  return S_OK;
  }

//**************************************************************************

// Called by 2.0 servers when shutting down.
STDMETHODIMP COPC_Shutdown::ShutdownRequest(LPCWSTR szReason)
  {
  reason = szReason;

  //   PostMessage(theDoc->hView, WM_OPCSHUTDOWN, (WPARAM)reason.GetBuffer(0), 0);
  return S_OK;
  }
