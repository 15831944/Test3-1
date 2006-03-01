// ScdFileSystem.h : Declaration of the CScdFileSystem

#ifndef __SCDFILESYSTEM_H_
#define __SCDFILESYSTEM_H_

#include "resource.h"       // main symbols
#include "ScdVBCP.h"

/////////////////////////////////////////////////////////////////////////////
// CScdFileSystem
class ATL_NO_VTABLE CScdFileSystem : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScdFileSystem, &CLSID_ScdFileSystem>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CScdFileSystem>,
	public IDispatchImpl<IScdFileSystem, &IID_IScdFileSystem, &LIBID_ScdVBLib>,
	public CProxy_IScdFileSystemEvents< CScdFileSystem >
{
public:
	CScdFileSystem()
	{
  m_hFileFind=INVALID_HANDLE_VALUE;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCDFILESYSTEM)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScdFileSystem)
	COM_INTERFACE_ENTRY(IScdFileSystem)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdFileSystem)
CONNECTION_POINT_ENTRY(DIID__ScdFileSystemEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdFileSystem
public:
	STDMETHOD(get_FindNextFile)(/*[out]*/ BSTR * pFileName, /*[out]*/ ScdFileAttributes *pAttr, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_FindFirstFile)(/*[in]*/ BSTR MaskScd, /*[out]*/ BSTR * pFileName, /*[out]*/ ScdFileAttributes *pAttr, /*[out, retval]*/ long *pVal);
private:
	WIN32_FIND_DATA m_FindData;
	HANDLE m_hFileFind;
};

#endif //__SCDFILESYSTEM_H_
