//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Inprocess.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  MFC application object, global definitions, OPC thread
//
//
//**************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include "cathelp.h"
#define __GUID_DEFINED__
#include "opcda_cats.c"

#include "OPCServer.h"

#define IID_DEFINED
#include "OPCDa_i.c"
#include "OPCComn_i.c"

// Run Guidgen to create a new CLSID for each server
// {4F5FB8C9-456E-11d0-9F7C-00609724420C}
CLSID CLSID_OPCServer =
{ 0x4f5fb8c9, 0x456e, 0x11d0, { 0x9f, 0x7c, 0x0, 0x60, 0x97, 0x24, 0x42, 0xc } };

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_OPCServer, OPCServer)
END_OBJECT_MAP()

UINT OPCSTMFORMATDATA;
UINT OPCSTMFORMATDATATIME;
UINT OPCSTMFORMATWRITECOMPLETE;
FILETIME     serverStartTime;

class CInProcessApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
};

CInProcessApp theApp;

BOOL CInProcessApp::InitInstance()
{
   OPCSTMFORMATDATA = RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
   OPCSTMFORMATDATATIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
   OPCSTMFORMATWRITECOMPLETE = RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));
   CoFileTimeNow( &serverStartTime );

    _Module.Init(ObjectMap, m_hInstance);
    return CWinApp::InitInstance();
}

int CInProcessApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
   // registers object, typelib and all interfaces in typelib
   HRESULT hr = _Module.RegisterServer(FALSE);
#if !defined(_WIN32_WCE)
   if( SUCCEEDED(hr) )
   {  // Register this server in both OPC DA 1.0 and 2.0 categories
      hr = CreateComponentCategory( CATID_OPCDAServer10, L"OPC Data Access Servers Version 1.0");
      if( SUCCEEDED(hr) )
         hr = RegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer10 );
      hr = CreateComponentCategory( CATID_OPCDAServer20, L"OPC Data Access Servers Version 2.0");
      if( SUCCEEDED(hr) )
         hr = RegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer20 );
   }
#endif
   return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    _Module.UnregisterServer();
#if !defined(_WIN32_WCE)
    UnRegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer10 );
    UnRegisterCLSIDInCategory( CLSID_OPCServer, CATID_OPCDAServer20 );
#endif
    return S_OK;
}

