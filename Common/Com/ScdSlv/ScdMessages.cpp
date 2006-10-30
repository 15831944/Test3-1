// ScdMessages.cpp : Implementation of CScdMessages
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdMessages.h"
#include "..\..\..\SMDK\Include\MsgLog.h"

/////////////////////////////////////////////////////////////////////////////
// CScdMessages

STDMETHODIMP CScdMessages::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdMessages
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }


HRESULT CScdMessages::FinalConstruct()
  {
  gs_MsgLog.IncrementComIFCnt();

  m_bSnapshotReqd=true;
  m_iViewLimit=1000;
  m_iSelection=eScdMsg_All;
  BuildSnapshot();
  return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
  }

void CScdMessages::FinalRelease()
  {
  for (int i = 0; i<m_Msgs.GetSize(); i++)
    m_Msgs[i]->Release();

  gs_MsgLog.DecrementComIFCnt();
  //m_pUnkMarshaler.Release();
  }
void CScdMessages::BuildSnapshot()
  {
  m_bSnapshotReqd=false;
  gs_MsgLog.Lock();
  long n=gs_MsgLog.ComQMsgCount();
  long i=Max(0L, (long)(gs_MsgLog.ComQMsgCount()-m_iViewLimit));
  m_Msgs.SetSize(n);
  for (  ; i<n; i++)
    {
    CMsgLogItem &It=gs_MsgLog.GetComQMsg(i);
    //if ((It.m_dwFlags&m_iOptions)!=0)
      {
      CComPtr<IScdMessage> p;
      p.CoCreateInstance(CLSID_ScdMessage);
      p.CopyTo(&m_Msgs[i]);

      p->put_Flags(eScdMsg_Flags(It.m_dwFlags));
      p->put_IDNo(It.m_lIDNo);
      p->put_IterationNo(It.m_lIterNo);
      p->put_SequenceNo(It.m_lSeqNo);
      p->put_CallNo(It.m_lCallNo);
      CComBSTR s(It.Source());
      p->put_Source(s);
      CComBSTR d(It.Description());
      p->put_Description(d);
      }
    }
  gs_MsgLog.Unlock();
  }

STDMETHODIMP CScdMessages::Item(long Index, IScdMessage ** pItem)
  {
  dllSCD_COMENTRY(long)
    {
    if (m_bSnapshotReqd)
      BuildSnapshot();
    if (Index<0 || Index>=m_Msgs.GetSize())
      return E_INVALIDARG;
    *pItem=m_Msgs[Index];
    return (*pItem)->AddRef();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdMessages::get_Count(LONG * pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    gs_MsgLog.Lock();
    *pVal=gs_MsgLog.ComQMsgCount();
    gs_MsgLog.Unlock();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdMessages::get_ViewLimit(LONG * pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal = m_iViewLimit;
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdMessages::put_ViewLimit(LONG pVal)
  {
  dllSCD_COMENTRY(long)
    {
    m_iViewLimit=pVal;
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdMessages::get__NewEnum(IUnknown * * pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    int size = 0;
    for(int i = 0; i < m_Msgs.GetSize(); i++)
      {
      eScdMsg_Flags Flags; 
      HRESULT hr=m_Msgs[i]->get_Flags(&Flags);
      if (m_iSelection & Flags)
        size++;
      }
    VARIANT* pVar = new VARIANT[size];

    for(i = 0; i < size; i++)
      {
      eScdMsg_Flags Flags; 
      HRESULT hr=m_Msgs[i]->get_Flags(&Flags);
      if (m_iSelection & Flags)
        {
        pVar[i].vt = VT_DISPATCH;
        pVar[i].pdispVal = m_Msgs[i];
        }
      }

    // Now make the enum.
    typedef CComObject< CComEnum< IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy< VARIANT > > > enumVar;
    enumVar* pEnumVar = new enumVar;
    pEnumVar->Init(&pVar[0], &pVar[size], NULL, AtlFlagCopy);
    delete[] pVar;

    // return the enum.
    Scd.ReturnH(pEnumVar->QueryInterface(IID_IUnknown, (void**)pVal));
    }		
  SCD_COMEXIT
  }


STDMETHODIMP CScdMessages::Refresh()
  {
  dllSCD_COMENTRY(long)
    {
    for (int i = 0; i<m_Msgs.GetSize(); i++)
      {
      m_Msgs[i]->Release();
      m_Msgs[i]=NULL;
      }
    BuildSnapshot();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdMessages::Flush(long NoToKeep)
  {
  dllSCD_COMENTRY(long)
    {
    for (int i = 0; i<m_Msgs.GetSize(); i++)
      {
      m_Msgs[i]->Release();
      m_Msgs[i]=NULL;
      }
    gs_MsgLog.FlushComQ(NoToKeep);
    BuildSnapshot();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdMessages::get_Options(eScdMsg_Options * pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal = (eScdMsg_Options)gs_MsgLog.GetOptions();
    }		

  SCD_COMEXIT
  }
STDMETHODIMP CScdMessages::put_Options(eScdMsg_Options pVal)
  {
  dllSCD_COMENTRY(long)
    {
    gs_MsgLog.SetOptions(pVal);
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdMessages::Select(eScdMsg_Flags Selection)
  {
  dllSCD_COMENTRY(long)
    {
    m_iSelection=(eScdMsg_Flags)Selection;
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdMessages::LogFileOpen(BSTR FileName)
  {
  USES_CONVERSION;
  dllSCD_COMENTRY(long)
    {
    Scd.Return(gs_MsgLog.LogFileOpen(OLE2T(FileName))?S_OK:S_FALSE, "Log FIle Not Opened");
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdMessages::LogFileClose()
  {
  dllSCD_COMENTRY(long)
    {
    gs_MsgLog.LogFileClose();
    }		
  SCD_COMEXIT
  }

