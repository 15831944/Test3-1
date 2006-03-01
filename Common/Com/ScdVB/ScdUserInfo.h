// ScdUserInfo.h : Declaration of the CScdUserInfo

#ifndef __SCDUSERINFO_H_
#define __SCDUSERINFO_H_

#include "resource.h"       // main symbols
#include "ScdVBCP.h"

/////////////////////////////////////////////////////////////////////////////
// CScdUserInfo
class ATL_NO_VTABLE CScdUserInfo : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdUserInfo, &CLSID_ScdUserInfo>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdUserInfo>,
	public IDispatchImpl<IScdUserInfo, &IID_IScdUserInfo, &LIBID_ScdVBLib>,
	public CProxy_IScdUserInfoEvents< CScdUserInfo >
{
public:
	CScdUserInfo()
	{
  m_eUserType=ScdUser_None;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDUSERINFO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdUserInfo)
	COM_INTERFACE_ENTRY(IScdUserInfo)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdUserInfo)
CONNECTION_POINT_ENTRY(DIID__ScdUserInfoEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdUserInfo
public:
	STDMETHOD(get_UserType)(/*[out, retval]*/ ScdUserType *pVal);
	STDMETHOD(put_UserType)(/*[in]*/ ScdUserType newVal);
	STDMETHOD(get_AllUsersProfile)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_UserProfile)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_UserName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_IsSystemManager)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsInstructor)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsEngineer)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(get_IsTrainee)(/*[out, retval]*/ BOOL *pVal);
private:
	ScdUserType m_eUserType;
};

#endif //__SCDUSERINFO_H_
