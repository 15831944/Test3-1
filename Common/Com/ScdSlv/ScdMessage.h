
// ScdMessage.h : Declaration of the CScdMessage

#ifndef __SCDMESSAGE_H
#define __SCDMESSAGE_H

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdMessage
class ATL_NO_VTABLE CScdMessage :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdMessage, &CLSID_ScdMessage>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdMessage>,
  public IDispatchImpl<IScdMessage, &IID_IScdMessage, &LIBID_ScdSlv>
  {
  public:
    CScdMessage()
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDMESSAGE)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdMessage)
      COM_INTERFACE_ENTRY(IScdMessage)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdMessage)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      m_iFlags=eScdMsg_NULL;
      m_bsSource="Unknown";
      m_bsDescription="Unknown";
      return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
      }

    void FinalRelease()
      {
      //m_pUnkMarshaler.Release();
      }

    //CComPtr<IUnknown> m_pUnkMarshaler;

    eScdMsg_Flags  m_iFlags;
    long           m_lIDNo;
    long           m_lIterNo;
    long           m_lSeqNo;
    long           m_lCallNo;
    CComBSTR       m_bsSource;
    CComBSTR       m_bsDescription;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdMessage
  public:
    // IScdMessage
    STDMETHOD(get_Flags)(eScdMsg_Flags* pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        *pVal=m_iFlags;
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_Flags)(eScdMsg_Flags newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_iFlags=newVal;
        }
      SCD_COMEXIT
      }
    STDMETHOD(get_Description)(BSTR * pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        m_bsDescription.CopyTo(pVal);
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_Description)(BSTR newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_bsDescription=newVal;
        }
      SCD_COMEXIT
      }
    STDMETHOD(get_Source)(BSTR * pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        m_bsSource.CopyTo(pVal);
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_Source)(BSTR newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_bsSource=newVal;
        }
      SCD_COMEXIT
      }
    // IScdMessage
    STDMETHOD(get_IDNo)(LONG * pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        *pVal=m_lIDNo;
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_IDNo)(LONG newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_lIDNo=newVal;
        }
      SCD_COMEXIT
      }
    STDMETHOD(get_IterationNo)(LONG * pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        *pVal=m_lIterNo;
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_IterationNo)(LONG newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_lIterNo=newVal;
        }
      SCD_COMEXIT
      }
    STDMETHOD(get_SequenceNo)(LONG * pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        *pVal=m_lSeqNo;
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_SequenceNo)(LONG newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_lSeqNo=newVal;
        }
      SCD_COMEXIT
      }
    STDMETHOD(get_CallNo)(LONG * pVal)
      {
      dllSCD_COMENTRYGET(long, pVal)
        {
        *pVal=m_lCallNo;
        }
      SCD_COMEXIT
      }
    STDMETHOD(put_CallNo)(LONG newVal)
      {
      dllSCD_COMENTRY(long)
        {
        m_lCallNo=newVal;
        }
      SCD_COMEXIT
      }

  };

#endif //__SCDMESSAGE_H_
