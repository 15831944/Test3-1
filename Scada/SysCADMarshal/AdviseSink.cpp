// AdviseSink.cpp: implementation of the AdviseSink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "AdviseSink.h"
#include "OPCStuff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//extern COPC_Client* theDoc;

//
// * COPC_AdviseSink::COPC_AdviseSink
// * COPC_AdviseSink::~COPC_AdviseSink
// *
// * Parameters (Constructor):
// *  pApp            PAPP to the application
// *
//
COPC_AdviseSink::COPC_AdviseSink(void)
  {
  m_cRef = 0;
  return;
  }

COPC_AdviseSink::~COPC_AdviseSink(void)
  {
  return;
  }

//
// * COPC_AdviseSink::QueryInterface
// * COPC_AdviseSink::AddRef
// * COPC_AdviseSink::Release
// *
// * Purpose:
// *  IUnknown members for COPC_AdviseSink object.
//
STDMETHODIMP COPC_AdviseSink::QueryInterface(REFIID riid, void **ppv)
  {
  *ppv = NULL;

  if(IID_IUnknown == riid || IID_IAdviseSink == riid) *ppv = this;

  if(NULL != *ppv)
    {
    ((LPUNKNOWN) * ppv)->AddRef();
    return NOERROR;
    }

  return ResultFromScode(E_NOINTERFACE);
  }

STDMETHODIMP_(ULONG)
COPC_AdviseSink::AddRef(void)
  {
  return ++m_cRef;
  }

STDMETHODIMP_(ULONG)
COPC_AdviseSink::Release(void)
  {
  if(0 != --m_cRef) return m_cRef;

  delete this;
  return 0;
  }

//
// * COPC_AdviseSink::OnDataChange
// *
// * Purpose:
// *  Notifes the advise sink that data changed in a data object.
// *  On this message you may request a new data rendering and update
// *  your displays as necessary.  Any data sent to this function is
// *  owned by the caller, not by this advise sink.
// *
// *  All Advise Sink methods are asynchronous and therefore we
// *  should attempt no synchronous calls from within them to an EXE
// *  object.  If we do, we'll get RPC_E_CALLREJECTED.
// *
// * Parameters:
// *  pFEIn           LPFORMATETC describing format that changed
// *  pSTM            LPSTGMEDIUM providing the medium in which the
// *                  data is provided.
// *
// * Return Value:
// *  None
//
STDMETHODIMP_ (void)
COPC_AdviseSink::OnDataChange(LPFORMATETC pFE, LPSTGMEDIUM pSTM)
{
  // Verify the format follows the OPC spec
  if(TYMED_HGLOBAL != pFE->tymed) return;
  if(pSTM->hGlobal == 0) return;
#ifdef DATATIMEFORMAT
  if(OPCSTMFORMATDATATIME != pFE->cfFormat)
#else
    if(OPCSTMFORMATDATA != pFE->cfFormat)
#endif // DATATIMEFORMAT
      {
      OnWriteComplete(pFE, pSTM);
      return;
      }

  // It must be a data advise
  const BYTE  *buffer = (BYTE *) GlobalLock(pSTM->hGlobal);
  if(!buffer) return;

  const OPCGROUPHEADER  *pHeader = (OPCGROUPHEADER *) buffer;

  // check pHeader->hClientGroup
  // pHeader->dwTransactionID
  // pHeader->hrStatus
  int                   offset = sizeof(OPCGROUPHEADER);

  /**
  // for each item in the data stream, get the value and quality
#ifdef DATATIMEFORMAT
  for(DWORD index = 0; index < pHeader->dwItemCount;
        index++, offset += sizeof(OPCITEMHEADER1))
    {
    const OPCITEMHEADER1  *pItemHeader = (OPCITEMHEADER1 *) &buffer[offset];
    COPC_Item             *pItem = (COPC_Item *) pItemHeader->hClient;


    //      CSLock lock(&pItem->m_pAccess->m_CSect);
    pItem->timestamp = pItemHeader->ftTimeStampItem;
#else
  for(DWORD index = 0; index < pHeader->dwItemCount;
          index++, offset += sizeof(OPCITEMHEADER2))
    {
    const OPCITEMHEADER2  *pItemHeader = (OPCITEMHEADER2 *) &buffer[offset];
    COPC_Item             *pItem = (COPC_Item *) pItemHeader->hClient;
#endif // DATATIMEFORMAT
    VARIANT               *pValue =
      (VARIANT *) &buffer[pItemHeader->dwValueOffset];

    // Strings and arrays are packed in the stream
    // requiring unpacking
    if(pValue->vt == VT_BSTR)
      {
      pValue->bstrVal = (BSTR) ((BYTE *) pValue + sizeof(VARIANT) + sizeof(DWORD));
      }
    else if((pValue->vt & VT_ARRAY) == VT_ARRAY)
      {
      pValue->parray = (SAFEARRAY *) ((BYTE *) pValue + sizeof(VARIANT));
      pValue->parray->pvData = ((BYTE *) pValue->parray + sizeof(SAFEARRAY));
      }
    pItem->m_value = pValue;
    pItem->quality = pItemHeader->wQuality;
    }
**/
  GlobalUnlock(pSTM->hGlobal);

  //   PostMessage(theDoc->hView, WM_OPCDATA, 0, 0);
  return;
  }
STDMETHODIMP_ (void)
COPC_AdviseSink::OnWriteComplete(LPFORMATETC pFE, LPSTGMEDIUM pSTM)
  {
  const BYTE  *buffer = (BYTE *) GlobalLock(pSTM->hGlobal);
  if(!buffer) return;

  const OPCGROUPHEADERWRITE *pHeader = (OPCGROUPHEADERWRITE *) buffer;
  if(FAILED(pHeader->hrStatus))
    {
    INCOMPLETECODE(__FILE__, __LINE__);  //
    //     PostMessage(theDoc->hView, WM_OPCWRITE, pHeader->hrStatus, 0);
    }
  else
    {
    int offset = sizeof(OPCGROUPHEADERWRITE);

    // for each item in the data stream, access its data
    for(DWORD index = 0; index < pHeader->dwItemCount;
              index++, offset += sizeof(OPCITEMHEADERWRITE))
      {
      const OPCITEMHEADERWRITE  *pItemHeader =
        (OPCITEMHEADERWRITE *) &buffer[offset];
      if(FAILED(pItemHeader->dwError))
        {
        //            PostMessage(theDoc->hView, WM_OPCWRITE, pItemHeader->dwError, 0);
        }
      }
    }
  GlobalUnlock(pSTM->hGlobal);

  return;
  }

//
// * COPC_AdviseSink::OnViewChange
// * COPC_AdviseSink::OnRename
// * COPC_AdviseSink::OnSave
// * COPC_AdviseSink::OnClose
// *
// * Unimplemented members
//
STDMETHODIMP_ (void)
COPC_AdviseSink::OnViewChange(DWORD dwAspect, LONG lindex)
  {
  return;
  }
STDMETHODIMP_ (void)
COPC_AdviseSink::OnRename(LPMONIKER pmk)
  {
  return;
  }
STDMETHODIMP_ (void)
COPC_AdviseSink::OnSave (void)
  {
  return;
  }
STDMETHODIMP_ (void)
COPC_AdviseSink::OnClose (void)
  {
  return;
  }

