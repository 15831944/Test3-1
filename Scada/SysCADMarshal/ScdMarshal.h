	
// ScdMarshal.h : Declaration of the CScdMarshal

#ifndef __SCDMARSHAL_H_
#define __SCDMARSHAL_H_

#include "resource.h"       // main symbols
#include "SysCADMarshalCP.h"


// ========================================================================
//
//
//
// ========================================================================

class CSubsItem
  {
  public: 
    DWORD    m_dwHandle;
    DWORD    m_lLastChange;
    bool     m_bMustSend;

    CSubsItem()
      {
      m_dwHandle=0;
      m_lLastChange=-1;
      m_bMustSend=0;
      }
  };

// ========================================================================
//
//
//
// ========================================================================

/////////////////////////////////////////////////////////////////////////////
// CScdMarshal
class ATL_NO_VTABLE CScdMarshal : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdMarshal, &CLSID_ScdMarshal>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdMarshal>,
	public IScdMarshal,
	public IScdExecObject,
	//public _IScdExecObjectEvents,
	public CProxy_IScdMarshalEvents< CScdMarshal >,
	public CProxy_IScdExecObjectEvents< CScdMarshal >
{
public:
	CScdMarshal();

DECLARE_REGISTRY_RESOURCEID(IDR_SCDMARSHAL)
DECLARE_GET_CONTROLLING_UNKNOWN()

//DECLARE_CLASSFACTORY_SINGLETON(CScdMarshal)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdMarshal)
	COM_INTERFACE_ENTRY(IScdExecObject)
	COM_INTERFACE_ENTRY(IScdMarshal)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
	//COM_INTERFACE_ENTRY(_IScdExecObjectEvents)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdMarshal)
CONNECTION_POINT_ENTRY(IID__IScdMarshalEvents)
CONNECTION_POINT_ENTRY(IID__IScdExecObjectEvents)
END_CONNECTION_POINT_MAP()


	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(
			GetControllingUnknown(), &m_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		m_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> m_pUnkMarshaler;
  CArray<CSubsItem, CSubsItem&> m_SubsItems;
  long m_nSubsItems;

  long AddSubsItem(DWORD Handle, bool MustSend);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
	STDMETHOD(WriteTaggedItems)(/*[in]*/ DWORD dwCount, /*[in, size_is(dwCount)]*/ SCD_TAGITEM *pTags, /*[out, size_is(dwCount)]*/ SCD_DATAITEM *pData, /*[out, size_is(dwCount)]*/ DWORD *pReturn);
// IScdMarshal
	STDMETHOD(Open)(/*[in, string]*/ LPCWSTR Filename, /*[out]*/ long *Return);
	STDMETHOD(Close)(/*[in]*/ VARIANT_BOOL Save, /*[out]*/ long *Return);
// IScdExecObject
	STDMETHOD(Start)(SCD_EXECUTIONCONTROL * pTmCB, DWORD * pdwReturn);
	STDMETHOD(Execute)(SCD_EXECUTIONCONTROL * pTmCB, SCD_EXECRETURN * pEORet, DWORD * pdwReturn);
	STDMETHOD(Stop)(SCD_EXECUTIONCONTROL * pTmCB, DWORD * pdwReturn);
	STDMETHOD(QueryTime)(SCD_EXECUTIONCONTROL * pTmCB, double * pdTimeReqd, double * pdDeltaTReqd);
	STDMETHOD(QuerySubscriptionTagsRequired)(VARIANT_BOOL Start, DWORD *dwMaxCount, DWORD *dwCount, SCD_TAGITEM **pTags);
	STDMETHOD(QuerySubscriptionTagsAvailable)(DWORD *dwMaxCount, DWORD *dwCount, SCD_TAGITEM **pTags);
	STDMETHOD(ReadSubscriptionData)(VARIANT_BOOL Start, VARIANT_BOOL ReadAll, DWORD *dwMaxCount, DWORD *dwCount, SCD_DATAITEM **pItems);
	STDMETHOD(WriteSubscriptionData)(DWORD *dwMaxCount, DWORD *dwCount, SCD_DATAITEM **pItems);
	STDMETHOD(ReadTaggedItems)(DWORD dwCount, SCD_TAGITEM *pTags, SCD_DATAITEM *pData, DWORD *pReturn);
// _IScdExecObjectEvents
};

#endif //__SCDMARSHAL_H_
