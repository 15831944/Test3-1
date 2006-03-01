//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ConnectionPoint.h
//  $Author    :  Jim Hansen
//
//  Description:  Callback class for OPC Data Access 2.0
//
//
//**************************************************************************
#include <atlbase.h>
//extern CComModule _Module;
#include <atlcom.h>

class COPC_Callback :
  public IOPCDataCallback,
  public CComObjectRootEx <CComMultiThreadModel>
  {
  public:
    COPC_Callback(void)           { ; }
    virtual ~COPC_Callback(void)  { ; }
    BEGIN_COM_MAP(COPC_Callback)
    COM_INTERFACE_ENTRY(IOPCDataCallback)
    END_COM_MAP()
    DECLARE_NOT_AGGREGATABLE(COPC_Callback)
    // The OPC Callback functions
    STDMETHODIMP  OnDataChange
                  (
                    DWORD     Transid,
                    OPCHANDLE grphandle,
                    HRESULT   masterquality,
                    HRESULT   mastererror,
                    DWORD     count,
                    OPCHANDLE *clienthandles,
                    VARIANT   *values,
                    WORD      *quality,
                    FILETIME  *time,
                    HRESULT   *errors
                  );

    STDMETHODIMP  OnReadComplete
                  (
                    DWORD     Transid,
                    OPCHANDLE grphandle,
                    HRESULT   masterquality,
                    HRESULT   mastererror,
                    DWORD     count,
                    OPCHANDLE *clienthandles,
                    VARIANT   *values,
                    WORD      *quality,
                    FILETIME  *time,
                    HRESULT   *errors
                  );

    STDMETHODIMP  OnWriteComplete
                  (
                    DWORD     Transid,
                    OPCHANDLE grphandle,
                    HRESULT   mastererr,
                    DWORD     count,
                    OPCHANDLE *clienthandles,
                    HRESULT   *errors
                  );

    STDMETHODIMP  OnCancelComplete(DWORD transid, OPCHANDLE grphandle);
  };

typedef CComObject<COPC_Callback> OPCCallbackObject;

//**************************************************************************
class COPC_Shutdown :
  public IOPCShutdown,
  public CComObjectRootEx <CComMultiThreadModel>
  {
  public:
    COPC_Shutdown(void)           { ; }
    virtual ~COPC_Shutdown(void)  { ; }
    BEGIN_COM_MAP(COPC_Shutdown)
    COM_INTERFACE_ENTRY(IOPCShutdown)
    END_COM_MAP()
    DECLARE_NOT_AGGREGATABLE(COPC_Shutdown)
    // The OPC Callback function
    STDMETHODIMP  ShutdownRequest(LPCWSTR szReason);

    CString       reason;
  };

typedef CComObject<COPC_Shutdown> OPCShutdownObject;
