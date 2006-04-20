//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  FSServer.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  FS OPC Server DLL
//
//  Description:  DLL functions, message loop thread, and callback class
//
//  If MFC_EXTENSION_DLL is defined, this is an MFC extension DLL (used with
//  MFC applications). If it is not defined, it statically links.
//**************************************************************************


#include "stdafx.h"
#include "OPCServer.h"
//#include <atlimpl.cpp>
//#include <statreg.cpp>
#include "initguid.h"

#include "cathelp.h"
#define __GUID_DEFINED__
#include "opcda_cats.c"

#include "OPCDa_i.c"      // OPC COM interface IDs
#include "OPCComn_i.c"
                       
#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif
    
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
//*******************************************************************
// {00000000-0000-0000-0000-000000000000}
//  StartFSServer will set a real CLSID in this placeholder
CLSID CLSID_OPCServer =
{ 0x00000000, 0x0000, 0x0000, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } };

CComModule _Module;     // Global ATL module

BEGIN_OBJECT_MAP(ObjectMap)
   OBJECT_ENTRY(CLSID_OPCServer, OPCServer)
END_OBJECT_MAP()

UINT OPCSTMFORMATDATA;
UINT OPCSTMFORMATDATATIME;
UINT OPCSTMFORMATWRITECOMPLETE;
FILETIME     serverStartTime;
COPCCallback* pCallback=0;
BOOL running;
extern CRITICAL_SECTION g_cs;  // Added 2.02

//*******************************************************************
// This thread function runs the message loop for OPC COM communications
DWORD OPCMainThread(DWORD hInstance)
{
   HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   if( FAILED( hRes ) )
      return hRes;

   _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);

   // wait until we get a WM_QUIT
   MSG msg;
   while (GetMessage(&msg, 0, 0, 0))
      DispatchMessage(&msg);
   _Module.RevokeClassObjects(); // Revoke class factories for ATL

   // Notify clients that the server is exiting
   {  // protect this loop
      CSLock wait( &g_cs );
      POSITION pos = servers.GetHeadPosition();
      while( pos )
      {
         OPCServerBase* pServer = servers.GetNext( pos );
         pServer->ServerShutdown( _T("Exiting Server"));
      }
   }

   // Let clients disconnect
   Sleep( 500 );  // was 2000

   // disconnect remaining clients
   {  // protect this loop
      CSLock wait( &g_cs );
      POSITION pos = servers.GetHeadPosition();
      while( pos )
      {
         OPCServerBase* pServer = servers.GetNext( pos );
         IUnknown* pUnk=0;
         HRESULT hr = ((CComObject<OPCServer>*)pServer)->QueryInterface( IID_IUnknown, (LPVOID*)&pUnk );
         if( SUCCEEDED(hr) )
            CoDisconnectObject( pUnk, 0 );  // disconnect from any remaining clients
      }
   }
     
   // Moved down by CNM on JV advice
   //CoUninitialize();
   Sleep( 500 );


   // Delete any remaining servers
   {  // protect this loop
      CSLock wait( &g_cs );
      POSITION pos = servers.GetHeadPosition();
      while( pos )
      {
         OPCServerBase* pServer = servers.GetNext( pos );
         delete pServer;
      }
   }

   CoUninitialize();
   //Sleep( 500 );

   return S_OK;
}


#ifdef MFC_EXTENSION_DLL
#include "afxdllx.h"    // standard MFC Extension DLL routines

static AFX_EXTENSION_MODULE NEAR extensionDLL = { NULL, NULL };
#endif

HANDLE hOPCThread = NULL;
DWORD OPCThreadId = 0;

/////////////////////////////////////////////////////////////////////////////
// find all languages in the resources
BOOL CALLBACK EnumResLangProc(
                    HMODULE hModule, // resource-module handle 
                    LPCTSTR lpszType, // address of resource type 
                    LPCTSTR lpszName, // address of resource name 
                    LANGID wIDLanguage, // resource language identifier
                    LPARAM lParam) // application-defined 
{
    LCIDs.Add(wIDLanguage);
    return TRUE;
}

//**************************************************************************
//**************************************************************************
//**************************************************************************
//
// Function Name : DllMain
// Description   : This routine is the entry point for this DLL.
//
// Convention    : DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
//                 Where:
//                    hInstance  = Instance handle for this
//                    dwReason   = Entry point reason
//                    lpReserved = Reserved.
//
// Return Value  : int
//
//**************************************************************************

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
   if (dwReason == DLL_PROCESS_ATTACH)
   {
    	TRACE1("ScdOPCSrv.DLL Initializing %08x!\n", hInstance);
#ifdef MFC_EXTENSION_DLL
      // Extension DLL one-time initialization - do not allocate memory here,
      // use the TRACE or ASSERT macros or call MessageBox
      if (!AfxInitExtensionModule(extensionDLL, hInstance))
         return 0;

      // create a new CDynLinkLibrary for this app
      new CDynLinkLibrary(extensionDLL);
#endif
    UINT idRsrcBlk = IDS_OPC_E_INVALIDHANDLE / 16 + 1;
    
    // Fill the list of supported languages for an error message
    EnumResourceLanguages(hInstance,  // Look in this module.
                          RT_STRING, // given resource type
                          MAKEINTRESOURCE(idRsrcBlk), // given resource name
                          (ENUMRESLANGPROC)EnumResLangProc, // callback function
                          0); // application-defined 

   }
   else if (dwReason == DLL_PROCESS_DETACH)
   {
      StopFSServer();
#ifdef MFC_EXTENSION_DLL
    	TRACE0("ScdOPCSrv.DLL Terminating!\n");
      AfxTermExtensionModule(extensionDLL);
#endif
   }

   return 1;   // ok
}

//*******************************************************************
// InitFSServer - initializes using the app's clsid and resources
// creates the message loop thread

const bool WITHOPCSERVER=true;
BOOL StartFSServer(HINSTANCE hInstance, CLSID* pCLSID, bool WithThread)
  {
  if (WITHOPCSERVER)
    {
    ObjectMap[0].pclsid = pCLSID; // fix up the correct CLSID for this app

    pCallback = new COPCCallback;
    OPCSTMFORMATDATA = RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
    OPCSTMFORMATDATATIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
    OPCSTMFORMATWRITECOMPLETE = RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));
    CoFileTimeNow( &serverStartTime );

    _Module.Init(ObjectMap, hInstance);

    if (WithThread)
      {
      SECURITY_ATTRIBUTES sa;
      sa.nLength = sizeof(sa);
      sa.lpSecurityDescriptor = NULL;
      hOPCThread = ::CreateThread( &sa, 0,
        (LPTHREAD_START_ROUTINE)&OPCMainThread,
        hInstance,
        0,
        &OPCThreadId);
      if( !hOPCThread )
        return FALSE;
      running = TRUE;
      }
    else
      hOPCThread = NULL;
    return TRUE;
    }
  return TRUE;
  }

//*******************************************************************
BOOL StopFSServer()
  {
  if (WITHOPCSERVER)
    {
    running = FALSE;
    if( hOPCThread )
      {
      ::PostThreadMessage(OPCThreadId, WM_QUIT, 0, 0);
      if( WAIT_FAILED == WaitForSingleObject( hOPCThread, 5000) )
        {
        TRACE(_T("Wait failed\n"));
        return FALSE;
        }
      CloseHandle( hOPCThread );
      hOPCThread = NULL;

      Sleep(10);
      try
        {
        delete pCallback;
        }
      catch (...) // access violation at this delete on shut 
        {
        int xxx=0;
        }
      }
    _Module.Term();
    return TRUE;
    }
  return TRUE;
  }

//*******************************************************************
// RegisterServer - Adds entries to the system registry

static HRESULT WINAPI MyUpdateRegistryFromResourceS(LPCTSTR EXEName, CAtlModule & Module, UINT nResID, BOOL bRegister,
	struct _ATL_REGMAP_ENTRY* pMapEntries /*= NULL*/) throw()
{
	CRegObject ro;
  ro.FinalConstruct();

	if (pMapEntries != NULL)
	{
		while (pMapEntries->szKey != NULL)
		{
			ATLASSERT(NULL != pMapEntries->szData);
			ro.AddReplacement(pMapEntries->szKey, pMapEntries->szData);
			pMapEntries++;
		}
	}

	HRESULT hr = Module.AddCommonRGSReplacements(&ro);
	if (FAILED(hr))
		return hr;

	USES_CONVERSION_EX;
	TCHAR szDllModule[MAX_PATH], szSrvModule[MAX_PATH];
	HINSTANCE hInst = _AtlBaseModule.GetModuleInstance();
	DWORD dwFLen = GetModuleFileName(hInst, szDllModule, MAX_PATH);
	if( dwFLen == 0 )
		return AtlHresultFromLastError();
	else if( dwFLen == MAX_PATH )
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

  if (EXEName)
    strcpy(szSrvModule, EXEName);
  else
    strcpy(szSrvModule, szDllModule);
  
	LPOLESTR pszSrvModule;
	pszSrvModule = T2OLE_EX(szSrvModule, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
	LPOLESTR pszDllModule;
	pszDllModule = T2OLE_EX(szDllModule, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
#ifndef _UNICODE
	if(pszSrvModule == NULL) 
		return E_OUTOFMEMORY;
	if(pszDllModule == NULL) 
		return E_OUTOFMEMORY;
#endif

	OLECHAR pszSrvModuleUnquoted[_MAX_PATH * 2], pszDllModuleUnquoted[_MAX_PATH * 2];
#if (_MSC_VER >= 1400) 
  Module.EscapeSingleQuote(pszSrvModuleUnquoted, _MAX_PATH * 2, pszSrvModule);
	Module.EscapeSingleQuote(pszDllModuleUnquoted, _MAX_PATH * 2, pszDllModule);
#else
	Module.EscapeSingleQuote(pszSrvModuleUnquoted, pszSrvModule);
	Module.EscapeSingleQuote(pszDllModuleUnquoted, pszDllModule);
#endif
	HRESULT hRes;
	if (EXEName || (hInst == NULL) || (hInst == GetModuleHandle(NULL))) // register as EXE
	{
		// If Registering as an EXE, then we quote the resultant path.
		// We don't do it for a DLL, because LoadLibrary fails if the path is
		// quoted
		OLECHAR pszSrvModuleQuote[(_MAX_PATH + _ATL_QUOTES_SPACE)*2]; 		
		pszSrvModuleQuote[0] = OLESTR('\"');
#if (_MSC_VER >= 1400)
		ocscpy_s(pszSrvModuleQuote + 1, (_MAX_PATH + _ATL_QUOTES_SPACE)*2, pszSrvModuleUnquoted);
#else
		ocscpy(pszSrvModuleQuote + 1, pszSrvModuleUnquoted);
#endif
		int nLen = ocslen(pszSrvModuleQuote);
		pszSrvModuleQuote[nLen] = OLESTR('\"');
		pszSrvModuleQuote[nLen + 1] = 0;

		hRes = ro.AddReplacement(OLESTR("Module"), pszSrvModuleQuote);
	}
	else
	{
		hRes = ro.AddReplacement(OLESTR("Module"), pszSrvModuleUnquoted);
	}	

	if(FAILED(hRes))
		return hRes;
		
	hRes = ro.AddReplacement(OLESTR("Module_Raw"), pszSrvModuleUnquoted);
	if(FAILED(hRes))
		return hRes;

	LPCOLESTR szType = OLESTR("REGISTRY");
	hr = (bRegister) ? ro.ResourceRegister(pszDllModule, nResID, szType) :
		ro.ResourceUnregister(pszDllModule, nResID, szType);
	return hr;
}
static HRESULT MyRegisterServer(LPCTSTR EXEName, CComModule & Module, BOOL bRegTypeLib = FALSE, const CLSID* pCLSID = NULL) throw()
{
	HRESULT hr = S_OK;
	_ATL_OBJMAP_ENTRY* pEntry = Module.m_pObjMap;
	if (pEntry != NULL)
	{
		for (;pEntry->pclsid != NULL; pEntry++)
		{
			if (pCLSID != NULL)
			{
				if (!IsEqualGUID(*pCLSID, *pEntry->pclsid))
					continue;
			}
			hr = pEntry->pfnUpdateRegistry(TRUE);

      bool bRegister=TRUE;
			ATL::_ATL_REGMAP_ENTRY regMapEntries[2]; 
			memset(&regMapEntries[1], 0, sizeof(ATL::_ATL_REGMAP_ENTRY)); 
      regMapEntries[0].szKey = L"OLEMISC"; 
			TCHAR szOleMisc[32]; 
			wsprintf(szOleMisc, _T("%d"), 0);//Module._GetMiscStatus()); 
			USES_CONVERSION_EX; 
			regMapEntries[0].szData = T2OLE_EX(szOleMisc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD); 
			if (regMapEntries[0].szData == NULL) 
				return E_OUTOFMEMORY;

	    // Hack by Mark West to make PLC5.EXE register with correct information
		#ifdef PLC5
   			HRESULT hr=MyUpdateRegistryFromResourceS(EXEName, Module, IDR_PLC5OPCSERVER, bRegister, regMapEntries);
		#else
   			HRESULT hr=MyUpdateRegistryFromResourceS(EXEName, Module, IDR_OPCSERVER, bRegister, regMapEntries);
		#endif


			if (FAILED(hr))
				break;
			hr = AtlRegisterClassCategoriesHelper( *pEntry->pclsid,
				pEntry->pfnGetCategoryMap(), TRUE );
			if (FAILED(hr))
				break;
		}
	}
	if (SUCCEEDED(hr))
		hr = Module.CAtlModuleT<CComModule>::RegisterServer(bRegTypeLib, pCLSID);
	return hr;
}

STDAPI RegisterServer(LPCTSTR EXEName)
{
//   HRESULT hr = _Module.RegisterServer();
   HRESULT hr = MyRegisterServer(EXEName, _Module);//.RegisterServer();

   if( SUCCEEDED(hr) )
   {  // Register this server in both OPC DA 1.0 and 2.0 categories
      hr = CreateComponentCategory( CATID_OPCDAServer10, L"OPC Data Access Servers Version 1.0");
      if( SUCCEEDED(hr) )
         hr = RegisterCLSIDInCategory( *ObjectMap[0].pclsid, CATID_OPCDAServer10 );
      hr = CreateComponentCategory( CATID_OPCDAServer20, L"OPC Data Access Servers Version 2.0");
      if( SUCCEEDED(hr) )
         hr = RegisterCLSIDInCategory( *ObjectMap[0].pclsid, CATID_OPCDAServer20 );
   }
   return hr;
}

//*******************************************************************
// UnregisterServer - Removes entries from the system registry

STDAPI UnregisterServer(void)
{
   UnRegisterCLSIDInCategory( *ObjectMap[0].pclsid, CATID_OPCDAServer10 );
   UnRegisterCLSIDInCategory( *ObjectMap[0].pclsid, CATID_OPCDAServer20 );
   HRESULT hr = _Module.UnregisterServer();
   return hr;
}

BOOL FSServerInUse()
{
   return (_Module.GetLockCount( )? TRUE : FALSE );
}

//*******************************************************************
// class CTag
//*******************************************************************
CTag::CTag()
{
   VariantInit( &m_value );
   m_nativeType = VT_R8;
   m_quality = 0;
   CoFileTimeNow( &m_timestamp );
   m_accessRights = OPC_READABLE|OPC_WRITEABLE;
   m_refCount = 0;
}

CTag::~CTag()
{
   VariantClear( &m_value );
}

//*******************************************************************
HRESULT CTag::SetValue( VARIANT& newValue )
{
   return VariantCopy( &m_value, &newValue );
}

//*******************************************************************
// App registers its object through this function
void SetCallbackObject(COPCCallback* pNewCallback )
{
   delete pCallback;
   ASSERT( pNewCallback != NULL );
   pCallback = pNewCallback;
}

//*******************************************************************
// class COPCCallback

COPCCallback::COPCCallback()
{
   InitializeCriticalSection( &m_CS );
}

COPCCallback::~COPCCallback()
{
   DeleteCriticalSection( &m_CS );
}

//*******************************************************************
LPWSTR COPCCallback::AllocString(LPCTSTR source)
{
   USES_CONVERSION;
   int length = _tcslen(source)+1;
   LPWSTR destination = (LPWSTR)CoTaskMemAlloc(2*length);
   wcscpy( destination, T2OLE((LPTSTR)source) );
   return destination;
}

//*******************************************************************
HRESULT COPCCallback::QueryNumProperties(
                              LPCTSTR     name,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid)
{
   return OPC_E_UNKNOWNITEMID;
}

//*******************************************************************
HRESULT COPCCallback::QueryAvailableProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pDataTypes)
{
   return OPC_E_UNKNOWNITEMID;
}

//*******************************************************************
HRESULT COPCCallback::GetItemProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors)
{
   return OPC_E_UNKNOWNITEMID;
}

//*******************************************************************
HRESULT COPCCallback::LookupItemIDs(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pszNewItemIDs,
                              HRESULT   * pErrors)
{
   return OPC_E_UNKNOWNITEMID;
}

//*******************************************************************
DWORD COPCCallback::SetUpdateRate( DWORD newUpdateRate )
{
   if( newUpdateRate > 10 )   // limit max rate to 10mSec
      return newUpdateRate;
   else
      return 10;
}

//*******************************************************************
//
CTag* COPCCallback::AddTag(
                           LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType)
{
   CSLock wait( &m_CS );
   CTag* pTag = new CTag;
   // Initialize the tag's type, etc.
   // should store in a list
   return pTag;
}

//*******************************************************************
HRESULT COPCCallback::ValidateTag(
                           CTag* pTag,
                           LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType)
{
   return E_NOTIMPL;
}

//*******************************************************************
//
HRESULT COPCCallback::Remove(
    DWORD           dwNumItems,
    CTag         ** ppTags)
{
   return S_OK;
}

//*******************************************************************
LPCTSTR COPCCallback::GetTagAccessPath( CTag* pTag )
{
   return _T("");
}

//*******************************************************************
LPCTSTR COPCCallback::GetTagName( CTag* pTag )
{
   return _T("");
}

//*******************************************************************
BOOL COPCCallback::GetTagLimits(  CTag * pTag, double *pHigh, double *pLow )
{
   return FALSE;
}

//*******************************************************************
// Read each of these tags as a periodic scan
HRESULT COPCCallback::Scan(
    DWORD           dwNumItems,
    CTag         ** ppTags,
    HRESULT       * pErrors)
{
   return Read(dwNumItems, ppTags, pErrors);
}

//*******************************************************************
// Read each of these tags on demand
HRESULT COPCCallback::Read(
    DWORD           dwNumItems,
    CTag         ** ppTags,
    HRESULT       * pErrors)
{
   HRESULT hr = S_OK;
//   CSLock wait( &m_CS );
   for( DWORD index=0; index<dwNumItems; index++ )
   {
      // check if it's readable first
      if( (ppTags[index]->m_accessRights & OPC_READABLE) == 0 )
      {
         pErrors[index] = OPC_E_BADRIGHTS;
      }

      if( pErrors[index] == S_OK )
      {
         HRESULT hres = ReadTag( ppTags[index] );
         if( FAILED(hres) )
         {
            pErrors[index] = hres;
            hr = S_FALSE;
         }
      }
      else
         hr = S_FALSE;
   }
   return hr;
}

//*******************************************************************

HRESULT COPCCallback::ReadTag(
    CTag * pTag)
{
   CSLock wait( &m_CS );
   return S_OK;
}

//*******************************************************************
//  Write takes an array of tags and values
//  The errors array indicates if the tag pointer is valid.
//  Only use the tag pointers with a successful error code.
//
HRESULT COPCCallback::Write(
    DWORD        dwNumItems,
    CTag      ** ppTags,
    VARIANT    * pItemValues,
    HRESULT    * pErrors)
{
   HRESULT hr = S_OK;
//   CSLock wait( &m_CS );
   for( DWORD index=0; index<dwNumItems; index++ )
   {
      // check if it's writeable first
      if( ppTags[index] && (ppTags[index]->m_accessRights & OPC_WRITEABLE) == 0 )
      {
         pErrors[index] = OPC_E_BADRIGHTS;
      }

      if( pErrors[index] == S_OK )
      {
         HRESULT hres = WriteTag( ppTags[index], pItemValues[index] );
         if( FAILED(hres) )
         {
            pErrors[index] = hres;
            hr = S_FALSE;
         }
      }
      else
         hr = S_FALSE;
   }
   return hr;
}

//*******************************************************************
// Write the value to this tag
// If successful, return S_OK
// If unsuccessful, return E_FAIL or a server specific error code
// If the tag is not writable, return OPC_E_BADRIGHTS
//
HRESULT COPCCallback::WriteTag(
    CTag    * pTag,
    VARIANT & value)
{
   return S_OK;
}

//*******************************************************************
// return a string if the error code is recognized
LPCTSTR COPCCallback::GetErrorString(
                              HRESULT     dwError,
                              LCID        dwLocale)
{
   return NULL;
}

//*******************************************************************
// return the vendor string for this server
LPCTSTR COPCCallback::GetVendorString()
{
   return _T("FactorySoft, Inc");
}

//*******************************************************************
//  COPCBrowser class
//*******************************************************************
COPCBrowser::~COPCBrowser()
{
}

//*******************************************************************
OPCNAMESPACETYPE COPCBrowser::QueryOrganization()
{
   return OPC_NS_FLAT;
}

//*******************************************************************
BOOL COPCBrowser::MoveUp()
{
   return FALSE;
}

//*******************************************************************
BOOL COPCBrowser::MoveDown(LPCTSTR branch)
{
   return FALSE;
}

//*******************************************************************
HRESULT COPCBrowser::GetNames(  OPCBROWSETYPE type,
                   LPCTSTR stringFilter,
                   VARTYPE dataTypeFilter,
                   DWORD accessFilter )
{
   return S_OK;
}

//*******************************************************************
LPCTSTR COPCBrowser::GetItemID( LPCTSTR name )
{
   return _T(" ");
}

//*******************************************************************
LPCTSTR COPCBrowser::Next()
{
   return _T("");
}

//*******************************************************************
void COPCBrowser::Reset()
{
}

//*******************************************************************
HRESULT COPCBrowser::GetAccessPaths( LPCTSTR name )
{
   return S_OK;
}

//*******************************************************************
LPCTSTR COPCBrowser::NextAccessPath()
{
   return _T("");
}

