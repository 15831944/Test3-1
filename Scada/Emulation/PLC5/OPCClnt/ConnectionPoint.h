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
extern CComModule _Module;
#include <atlcom.h>

class COpcC_Callback :
   public IOPCDataCallback,
   public CComObjectRootEx<CComMultiThreadModel>
{
public:
   COpcC_Callback() {;}
   virtual  ~COpcC_Callback(){;}

BEGIN_COM_MAP(COpcC_Callback)
   COM_INTERFACE_ENTRY(IOPCDataCallback)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(COpcC_Callback)

   // The OPC Callback functions
   STDMETHODIMP OnDataChange(
    DWORD       Transid,
    OPCHANDLE   grphandle,
    HRESULT     masterquality,
    HRESULT     mastererror,
    DWORD       count,
    OPCHANDLE * clienthandles,
    VARIANT   * values,
    WORD      * quality,
    FILETIME  * time,
    HRESULT   * errors);

   STDMETHODIMP OnReadComplete(
    DWORD       Transid,
    OPCHANDLE   grphandle,
    HRESULT     masterquality,
    HRESULT     mastererror,
    DWORD       count,
    OPCHANDLE * clienthandles,
    VARIANT   * values,
    WORD      * quality,
    FILETIME  * time,
    HRESULT   * errors);

   STDMETHODIMP OnWriteComplete(
    DWORD       Transid,
    OPCHANDLE   grphandle,
    HRESULT     mastererr,
    DWORD       count,
    OPCHANDLE * clienthandles,
    HRESULT   * errors);

   STDMETHODIMP OnCancelComplete(
    DWORD       transid,
    OPCHANDLE   grphandle);
};

typedef CComObject<COpcC_Callback> OPCCallbackObject;

//**************************************************************************
class COpcC_Shutdown :
   public IOPCShutdown,
   public CComObjectRootEx<CComMultiThreadModel>
{
public:
   COpcC_Shutdown() {;}
   virtual  ~COpcC_Shutdown(){;}

BEGIN_COM_MAP(COpcC_Shutdown)
   COM_INTERFACE_ENTRY(IOPCShutdown)
END_COM_MAP()

DECLARE_NOT_AGGREGATABLE(COpcC_Shutdown)

   // The OPC Callback function
   STDMETHODIMP ShutdownRequest(LPCWSTR szReason);

   CString reason;
};

typedef CComObject<COpcC_Shutdown> OPCShutdownObject;
