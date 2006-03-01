
// ScdAppTags.h : Declaration of the CScdAppTags

#ifndef __SCDAPPTAGS_H_
#define __SCDAPPTAGS_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdAppTags
class ATL_NO_VTABLE CScdAppTags :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdAppTags, &CLSID_ScdAppTags>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdAppTags>,
  public IDispatchImpl<IScdAppTags, &IID_IScdAppTags, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
{
public:
  CScdAppTags() : CScdCOCmdBase(WMU_COM_APP)
  {
    //m_pUnkMarshaler = NULL;
  }

DECLARE_REGISTRY_RESOURCEID(IDR_SCDAPPTAGS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdAppTags)
  COM_INTERFACE_ENTRY(IScdAppTags)
  //COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY2(IDispatch, IScdAppTags)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdAppTags)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(eScdAppTagsCodes);

  HRESULT FinalConstruct()
  {
    return S_OK;//CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
  }

  void FinalRelease()
  {
    //m_pUnkMarshaler.Release();
  }

  //CComPtr<IUnknown> m_pUnkMarshaler;

// ISupportsErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdAppTags
public:
  STDMETHOD(get_TagRecording)(/*[in]*/ BSTR Tag, /*[out, retval]*/ eScdTagRecState *pVal);
  STDMETHOD(put_TagRecording)(/*[in]*/ BSTR Tag, /*[in]*/ eScdTagRecState newVal);
  STDMETHOD(get_TagType)(/*[in]*/ BSTR TheTag, /*[out, retval]*/ eScdAppTagType *pVal);
  STDMETHOD(get_TagValue)(/*[in]*/ BSTR TheTag, /*[out, retval]*/ VARIANT *pVal);
  STDMETHOD(put_TagValue)(/*[in]*/ BSTR TheTag, /*[in]*/ VARIANT newVal);
  STDMETHOD(Tag)(VARIANT WhichOne, IScdAppTag ** pItem);
  STDMETHOD(get_Count)(LONG * pVal);
  STDMETHOD(Add)(BSTR Tag);
  STDMETHOD(Remove)(VARIANT WhichOne);
  STDMETHOD(get__NewEnum)(IUnknown * * pVal);

// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

};

#endif //__SCDAPPTAGS_H_
