
// ScdSlvTags.h : Declaration of the CScdTags

#ifndef __SCDTAGS_H_
#define __SCDTAGS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdSlvTags
class ATL_NO_VTABLE CScdSlvTags :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdSlvTags, &CLSID_ScdSlvTags>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdSlvTags>,
  public IDispatchImpl<IScdSlvTags, &IID_IScdSlvTags, &LIBID_ScdSlv>
  {
  public:
    CScdSlvTags() 
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDSLVTAGS)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdSlvTags)
      COM_INTERFACE_ENTRY(IScdSlvTags)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdSlvTags)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(eScdSlvTagsCodes);

    HRESULT FinalConstruct()
      {
      return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
      }

    void FinalRelease()
      {
      //m_pUnkMarshaler.Release();
      }

    //CComPtr<IUnknown> m_pUnkMarshaler;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdSlvTags
  public:
    STDMETHOD(get_TagType)(/*[in]*/ BSTR TheTag, /*[out, retval]*/ eScdSlvTagType *pVal);
    STDMETHOD(get_TagValue)(/*[in]*/ BSTR TheTag, /*[out, retval]*/ VARIANT *pVal);
    STDMETHOD(put_TagValue)(/*[in]*/ BSTR TheTag, /*[in]*/ VARIANT newVal);
    STDMETHOD(Tag)(VARIANT WhichOne, IScdSlvTag ** pItem);
    STDMETHOD(get_Count)(LONG * pVal);
    STDMETHOD(Add)(BSTR Tag);
    STDMETHOD(Remove)(VARIANT WhichOne);
    STDMETHOD(get__NewEnum)(IUnknown * * pVal);

  };

#endif //__SCDTAGS_H_
