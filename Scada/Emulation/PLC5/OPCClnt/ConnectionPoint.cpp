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
#include "OPCClient.h"

CComModule _Module;  // just needed to keep ATL happy
//extern OPCClientDoc* theDoc;

//**************************************************************************
// Called by the server at the update rate when data changes
STDMETHODIMP COpcC_Callback::OnDataChange(
                                    DWORD       Transid,
                                    OPCHANDLE   grphandle,
                                    HRESULT     masterquality,
                                    HRESULT     mastererror,
                                    DWORD       count,
                                    OPCHANDLE * clienthandles,
                                    VARIANT   * values,
                                    WORD      * quality,
                                    FILETIME  * time,
                                    HRESULT   * errors)
{
   // If Transid != 0, this is a refresh
   for( DWORD index=0; index<count; index++ )
   {
      if( SUCCEEDED(errors[index]) )
      {
         COpcC_Item* pItem = (COpcC_Item*)clienthandles[index];
         if( pItem )
         {
            pItem->value = values[index];
            pItem->quality = quality[index];
            pItem->timestamp = time[index];
         }
      }
   }
//   PostMessage(theDoc->hView, WM_OPCDATA, 0, 0);
   return S_OK;
}

//**************************************************************************
// Called by the server to complete an ASyncIO2::Read
STDMETHODIMP COpcC_Callback::OnReadComplete(
                                    DWORD       Transid,
                                    OPCHANDLE   grphandle,
                                    HRESULT     masterquality,
                                    HRESULT     mastererror,
                                    DWORD       count,
                                    OPCHANDLE * clienthandles,
                                    VARIANT   * values,
                                    WORD      * quality,
                                    FILETIME  * time,
                                    HRESULT   * errors)
{
   ASSERT( Transid == 3 ); // test only
   for( DWORD index=0; index<count; index++ )
   {
      if( SUCCEEDED(errors[index]) )
      {
         COpcC_Item* pItem = (COpcC_Item*)clienthandles[index];
         if( pItem )
         {
            pItem->value = values[index];
            pItem->quality = quality[index];
            pItem->timestamp = time[index];
         }
      }
   }
//   PostMessage(theDoc->hView, WM_OPCDATA, 0, 0);
   return S_OK;
}


//**************************************************************************
// Called by the server to complete an ASyncIO2::Write
STDMETHODIMP COpcC_Callback::OnWriteComplete(
                                    DWORD       Transid,
                                    OPCHANDLE   grphandle,
                                    HRESULT     mastererr,
                                    DWORD       count,
                                    OPCHANDLE * clienthandles,
                                    HRESULT   * errors)
{
   ASSERT( Transid == 2 ); // test only
   for( DWORD index=0; index<count; index++ )
   {
      if( FAILED(errors[index]) )
      {
         COpcC_Item* pItem = (COpcC_Item*)clienthandles[index];
//         PostMessage(theDoc->hView, WM_OPCWRITE, errors[index], 0);
      }
   }
   return S_OK;
}


//**************************************************************************
// Called by the server to complete an ASyncIO2::Cancel
STDMETHODIMP COpcC_Callback::OnCancelComplete(
                                    DWORD       transid,
                                    OPCHANDLE   grphandle)
{
   return S_OK;
}

//**************************************************************************
// Called by 2.0 servers when shutting down.
STDMETHODIMP COpcC_Shutdown::ShutdownRequest(LPCWSTR szReason)
{
   reason = szReason;
//   PostMessage(theDoc->hView, WM_OPCSHUTDOWN, (WPARAM)reason.GetBuffer(0), 0);
   return S_OK;
}
