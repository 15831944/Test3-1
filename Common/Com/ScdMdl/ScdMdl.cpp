// ScdMdl.cpp : Implementation of DLL Exports.
// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f ScdMdlps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>

#define __SCDMDL_CPP

#include "ScdMdl.h"

#include "ScdMdl_i.c"

//#include "ScdSpModel.h"
//#include "ScdSpecieDB.h"                  
#include "ScdLicenseMdl.h"
//#include "ScdSpecieDefn.h"
#include "ScdSpecieDefns.h"
#include "ScdSpecieDefn.h"
//#include "ScdSpVectorDefn.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	//OBJECT_ENTRY(CLSID_ScdSpModel, CScdSpModel)
	//OBJECT_ENTRY(CLSID_ScdSpecieDB, CScdSpecieDB)
	OBJECT_ENTRY(CLSID_ScdLicenseMdl, CScdLicenseMdl)
	//OBJECT_ENTRY(CLSID_ScdSpecieDefn, CScdSpecieDefn)
	OBJECT_ENTRY(CLSID_ScdSpecieDefns, CScdSpecieDefns)
	OBJECT_ENTRY(CLSID_ScdSpecieDefn, CScdSpecieDefn)
	//OBJECT_ENTRY(CLSID_ScdSpVectorDefn, CScdSpVectorDefn)
END_OBJECT_MAP()

class CScdMdlApp : public CWinApp
	{
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CScdMdlApp)
	public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();
		//}}AFX_VIRTUAL

		//{{AFX_MSG(CScdMdlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

BEGIN_MESSAGE_MAP(CScdMdlApp, CWinApp)
	//{{AFX_MSG_MAP(CScdMdlApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CScdMdlApp theApp;

BOOL CScdMdlApp::InitInstance()
	{
	_Module.Init(ObjectMap, m_hInstance, &LIBID_ScdMdl);
	return CWinApp::InitInstance();
	}

int CScdMdlApp::ExitInstance()
	{
	TRACE0("ScdMdl : ExitInstance\n");
	_Module.Term();
	return CWinApp::ExitInstance();
	}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE0("ScdMdl : Calling AfxDllCanUnloadNow\n");
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
 	return _Module.RegisterServer(TRUE);
	}             

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
	{
	return _Module.UnregisterServer(TRUE);
	}

//===========================================================================
//
//
//
//===========================================================================

#define DLLEXPORT  __declspec(dllexport)

//---------------------------------------------------------------------------

extern "C" DLLEXPORT BOOL IsFlowLibDLL()
	{
	return TRUE;
	}

//---------------------------------------------------------------------------

extern "C" DLLEXPORT BOOL GetDLLInfo(MDLLInfo* pDLLInfo)
	{
	pDLLInfo->iPriority = 5;
	pDLLInfo->sName = "ScdMdl";
	return TRUE;
	}

//---------------------------------------------------------------------------

extern "C" DLLEXPORT CMdlCfgBase* GetCfgPropPg(int iPage, CMdlCfgSheet * pSheet)
	{
	return NULL;
	};

//===========================================================================
//
//
//
//===========================================================================

void ForceLoad_ScdMdl()
	{
	// Dummy Function to allow other libraries to force load this one
	}

//===========================================================================
//
//
//
//===========================================================================
//#include "ScdMessages.h"
