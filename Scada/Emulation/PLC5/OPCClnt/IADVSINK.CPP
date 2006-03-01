/*
 * IADVSINK.CPP
 * Data Object User Chapter 10
 *
 * Implementation of an object with IAdviseSink.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Microsoft
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */

#include "stdafx.h"
#include "OPCClient.h"
#include <afxadv.h>

//extern COpcC_Client* theDoc;

/*
 * COpcC_AdviseSink::COpcC_AdviseSink
 * COpcC_AdviseSink::~COpcC_AdviseSink
 *
 * Parameters (Constructor):
 *  pApp            PAPP to the application
 *
 */

COpcC_AdviseSink::COpcC_AdviseSink()
    {
    m_cRef=0;
    return;
    }

COpcC_AdviseSink::~COpcC_AdviseSink(void)
    {
    return;
    }

/*
 * COpcC_AdviseSink::QueryInterface
 * COpcC_AdviseSink::AddRef
 * COpcC_AdviseSink::Release
 *
 * Purpose:
 *  IUnknown members for COpcC_AdviseSink object.
 */

STDMETHODIMP COpcC_AdviseSink::QueryInterface(REFIID riid, void** ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid || IID_IAdviseSink==riid)
        *ppv=this;

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) COpcC_AdviseSink::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) COpcC_AdviseSink::Release(void)
    {
    if (0!=--m_cRef)
        return m_cRef;

    delete this;
    return 0;
    }



/*
 * COpcC_AdviseSink::OnDataChange
 *
 * Purpose:
 *  Notifes the advise sink that data changed in a data object.
 *  On this message you may request a new data rendering and update
 *  your displays as necessary.  Any data sent to this function is
 *  owned by the caller, not by this advise sink.
 *
 *  All Advise Sink methods are asynchronous and therefore we
 *  should attempt no synchronous calls from within them to an EXE
 *  object.  If we do, we'll get RPC_E_CALLREJECTED.
 *
 * Parameters:
 *  pFEIn           LPFORMATETC describing format that changed
 *  pSTM            LPSTGMEDIUM providing the medium in which the
 *                  data is provided.
 *
 * Return Value:
 *  None
 */


STDMETHODIMP_(void) COpcC_AdviseSink::OnDataChange(LPFORMATETC pFE
    , LPSTGMEDIUM pSTM)
   {
   // Verify the format follows the OPC spec
   if( TYMED_HGLOBAL != pFE->tymed )
      return;
   if( pSTM->hGlobal == 0 )
      return;
#ifdef DATATIMEFORMAT
   if( OPCSTMFORMATDATATIME != pFE->cfFormat )
#else
   if( OPCSTMFORMATDATA != pFE->cfFormat )
#endif // DATATIMEFORMAT
   {
      OnWriteComplete(pFE, pSTM);
      return;
   }

   // It must be a data advise
   const BYTE* buffer = (BYTE*)GlobalLock( pSTM->hGlobal );
   if( !buffer )
      return;

   const OPCGROUPHEADER* pHeader = (OPCGROUPHEADER*)buffer;
   // check pHeader->hClientGroup
   // pHeader->dwTransactionID
   // pHeader->hrStatus
   int offset = sizeof(OPCGROUPHEADER);
   // for each item in the data stream, get the value and quality
#ifdef DATATIMEFORMAT
   for( DWORD index=0; index<pHeader->dwItemCount; index++, offset += sizeof(OPCITEMHEADER1) )
      {
      const OPCITEMHEADER1* pItemHeader = (OPCITEMHEADER1*)&buffer[offset];
      COpcC_Item* pItem = (COpcC_Item*)pItemHeader->hClient;
      pItem->timestamp = pItemHeader->ftTimeStampItem;
#else
   for( DWORD index=0; index<pHeader->dwItemCount; index++, offset += sizeof(OPCITEMHEADER2) )
      {
      const OPCITEMHEADER2* pItemHeader = (OPCITEMHEADER2*)&buffer[offset];
      COpcC_Item* pItem = (COpcC_Item*)pItemHeader->hClient;
#endif // DATATIMEFORMAT

      VARIANT* pValue = (VARIANT*)&buffer[pItemHeader->dwValueOffset];
      // Strings and arrays are packed in the stream
      // requiring unpacking
      if( pValue->vt == VT_BSTR )
        {
        pValue->bstrVal = (BSTR)((BYTE*)pValue + sizeof(VARIANT) + sizeof(DWORD));
        }
      else if( (pValue->vt & VT_ARRAY) == VT_ARRAY )
        {
        pValue->parray = (SAFEARRAY*)((BYTE*)pValue + sizeof(VARIANT));
        pValue->parray->pvData = ((BYTE*)pValue->parray + sizeof(SAFEARRAY));
        }
      pItem->value = pValue;
      pItem->quality = pItemHeader->wQuality;
      }

   GlobalUnlock( pSTM->hGlobal );

//   PostMessage(theDoc->hView, WM_OPCDATA, 0, 0);

   return;
   }



STDMETHODIMP_(void) COpcC_AdviseSink::OnWriteComplete(LPFORMATETC pFE
    , LPSTGMEDIUM pSTM)
   {
   const BYTE* buffer = (BYTE*)GlobalLock( pSTM->hGlobal );
   if( !buffer )
      return;

   const OPCGROUPHEADERWRITE* pHeader = (OPCGROUPHEADERWRITE*)buffer;
   if( FAILED(pHeader->hrStatus) )
     {
//     PostMessage(theDoc->hView, WM_OPCWRITE, pHeader->hrStatus, 0);
     }
   else
   {
      int offset = sizeof(OPCGROUPHEADERWRITE);
      // for each item in the data stream, access its data
      for( DWORD index=0; index<pHeader->dwItemCount; index++, offset += sizeof(OPCITEMHEADERWRITE) )
      {
         const OPCITEMHEADERWRITE* pItemHeader = (OPCITEMHEADERWRITE*)&buffer[offset];
         if( FAILED(pItemHeader->dwError) )
         {
//            PostMessage(theDoc->hView, WM_OPCWRITE, pItemHeader->dwError, 0);
         }
      }
   }
   GlobalUnlock( pSTM->hGlobal );

   return;
   }


/*
 * COpcC_AdviseSink::OnViewChange
 * COpcC_AdviseSink::OnRename
 * COpcC_AdviseSink::OnSave
 * COpcC_AdviseSink::OnClose
 *
 * Unimplemented members
 */

STDMETHODIMP_(void) COpcC_AdviseSink::OnViewChange(DWORD dwAspect
    , LONG lindex)
    {
    return;
    }

STDMETHODIMP_(void) COpcC_AdviseSink::OnRename(LPMONIKER pmk)
    {
    return;
    }

STDMETHODIMP_(void) COpcC_AdviseSink::OnSave(void)
    {
    return;
    }

STDMETHODIMP_(void) COpcC_AdviseSink::OnClose(void)
    {
    return;
    }
