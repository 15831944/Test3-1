// CommandWrapper.cpp: implementation of the CScdCOMWork class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#define __SCDCOMWRAPPER_CPP
#include "ScdCOMIF.h"
#include "atlbase.h"

//========================================================================

static LPCTSTR GetErrorString(LPCTSTR description, HRESULT error)
{
   static TCHAR temp[2048];
   strcpy(temp, description);
   strcat(temp, _T(" "));
   TCHAR msg[MAX_PATH*5];
   DWORD ok = FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                  msg, MAX_PATH, NULL );
   if( ok )
   {
//FIX      TRACE(msg);
//FIX      TRACE(_T("\n"));
      strcat(temp, msg);
      return temp;
   }

   // else try the server
   strcat(temp, _T("Unknown Error."));
   return temp;
}

//========================================================================

static void LclReportError(LPCTSTR description, HRESULT error)
  {
  if (SUCCEEDED(error))
    return;

  LPCTSTR msg=GetErrorString(description,error);
  //LPCTSTR appName((LPCSTR)AFX_IDS_APP_TITLE);
  ::MessageBox(0, msg, "ScdCOM", MB_OK);
  //OutputLine("                                    %s", msg);
  };
//========================================================================

// This neesd top MATCH UP WITH ScdCOM
class IBackdoor : public IDispatch
  {
  public:
      virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Register(
          /* [in] */ ULONG i_dwCastedCallback) = 0;

      virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResourceHInstance (
          /* [ref][out] */ ULONG *hResInst) = 0;
      virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResourceCBRegID (
          /* [ref][out] */ ULONG *ID) = 0;

  };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IID		CScdCOMWork::smIID_IHelper = {0xF0A8E28B,0xC450,0x4F11,{0xB9,0x6B,0x7A,0xA6,0x2B,0x44,0x5D,0x36}};
IID		CScdCOMWork::smIID_IBackdoor = {0x7837875f,0x63e2,0x4642,{0xaf,0x7c,0xdf,0x1b,0xa7,0xff,0x10,0xa4}};
CLSID	CScdCOMWork::smCLSID_Helper = {0x1E7E6A3F,0x9107,0x40E7,{0x96,0x16,0x65,0xEE,0xE2,0xCC,0xCE,0x06}};

CScdCOMWork::CScdCOMWork()
{

}

CScdCOMWork::~CScdCOMWork()
{
}

void CScdCOMWork::Register(CScdCOMBase *i_pCallback, DWORD ComEvtMsg)
{
	//m_evGoDead = ::CreateEvent( 0 , TRUE , FALSE , _T("") );
  
	m_pCallback = i_pCallback;

	IClassFactory		*pClassFactory;
	IBackdoor			*pBackdoor;

  m_dwEvtMsgId = ComEvtMsg;

  // Get Class Factory for Helper COM Object
	HRESULT hr = ::CoGetClassObject( smCLSID_Helper , CLSCTX_ALL , 0, IID_IClassFactory, (LPVOID*)&pClassFactory);
  LclReportError("CoGetClassObject", hr);
  _ASSERTE(SUCCEEDED(hr));
  // Create one
	hr = pClassFactory->CreateInstance( 0 , smIID_IBackdoor , (LPVOID*)&pBackdoor);
  LclReportError("CreateInstance", hr);
  _ASSERTE(SUCCEEDED(hr));

  // Set Static Member to m_pCallback
  //	hr = pBackdoor->Register( reinterpret_cast<ULONG>( m_pCallback ) );

  // For Registry stuff
	hr = pBackdoor->GetResourceHInstance( reinterpret_cast<ULONG*>(&m_hResInst));
  LclReportError("GetResourceHInstance", hr);
  _ASSERTE(SUCCEEDED(hr));
	hr = pBackdoor->GetResourceCBRegID( reinterpret_cast<ULONG*>(&m_lResCBRegID));
  LclReportError("GetResourceCBRegID", hr);
  _ASSERTE(SUCCEEDED(hr));
  // release
	pBackdoor->Release();

  // If this class gets used a lot we might want to use the non-leaky version
	m_hThreadProc = ::CreateThread(0 , 0 , StaticThreadProc , this , 0 , &m_dwThreadId);
  m_EventSender.Init(m_dwThreadId, m_dwEvtMsgId);

  //::SetThreadPriority(m_hThreadProc, THREAD_PRIORITY_ABOVE_NORMAL);
  }

DWORD WINAPI CScdCOMWork::StaticThreadProc(LPVOID lpv)
{
	return static_cast<CScdCOMWork*>(lpv)->ThreadProc();
}

DWORD WINAPI CScdCOMWork::ThreadProc()
{
	try
	{
		::CoInitializeEx( 0 , COINIT_MULTITHREADED );

		DoStartupStuff();
    gs_Events.Add(&m_EventSender);

    MSG msg;
		while (GetMessage(&msg,NULL,0,0)>0)
      {
      if (msg.message==m_dwEvtMsgId)
        {
  //AtlTrace(">>CScdCOMWork::DoEventMsg\n");
        m_pCallback->DoEventMsg(msg.wParam, msg.lParam);
  //AtlTrace("<<CScdCOMWork::DoEventMsg\n");
        }
      else
        Sleep(1);
      }

    gs_Events.Remove(&m_EventSender);
    DoShutdownStuff();

		::CoUninitialize( );
		return 0;
	}
	catch(...)
	{
		return -1;
	}
}

HRESULT CScdCOMWork::ReportError(LPCTSTR szDesc, UINT Code)
  {
  return m_pCallback->ReportError(szDesc, Code);
  }

void CScdCOMWork::UnRegister()
{
	//::SetEvent( m_evGoDead );
  PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
	if ( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThreadProc , 10000 ))
    {
		::TerminateThread( m_hThreadProc , 0 );
    }
//	::CloseHandle( m_hThreadProc );
	//::CloseHandle( m_evGoDead );
}

void CScdCOMWork::DoStartupStuff()
{
	IClassFactory		*pClassFactory;
	IBackdoor			*pBackdoor;
  // Get Class Factory for Helper COM Object
	HRESULT hr = ::CoGetClassObject( smCLSID_Helper , CLSCTX_ALL , 0, IID_IClassFactory, (LPVOID*)&pClassFactory);
  LclReportError("CoGetClassObject", hr);
	_ASSERTE(SUCCEEDED(hr));
  // Create one
	hr = pClassFactory->CreateInstance( 0 , smIID_IBackdoor , (LPVOID*)&pBackdoor);
  LclReportError("CreateInstance", hr);
	_ASSERTE(SUCCEEDED(hr));
  // Set Statis Member to m_pCallback
	hr = pBackdoor->Register( reinterpret_cast<ULONG>( m_pCallback ) );
  LclReportError("Register", hr);
	_ASSERTE(SUCCEEDED(hr));

  // release
	pBackdoor->Release();

	//Now tel the SCM we're doing this object
	LPUNKNOWN	pUnk;
  // get IUknown of class factory
	hr = pClassFactory->QueryInterface( IID_IUnknown , (LPVOID*)&pUnk );
  LclReportError("QueryInterface", hr);
	_ASSERTE(SUCCEEDED(hr));
	//DWORD dwRegister;
  // tell COM that WE syscad/PLC5/H&B now serves the specd class ID
	hr = ::CoRegisterClassObject( m_pCallback->clsidGetCLSID() , pUnk ,CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &m_dwRegClsObj);
  LclReportError("CoRegisterClassObject", hr);
	_ASSERTE(SUCCEEDED(hr));

//  hr=::CoInitializeSecurity(NULL, 0, NULL, NULL , 0, 0, NULL, 0, NULL);
//  LclReportError("CoInitializeSecurity", hr);
//	_ASSERTE(SUCCEEDED(hr));

	//DWORD dwRegisterO;
  hr = ::RegisterActiveObject( pUnk ,m_pCallback->clsidGetCLSID(), ACTIVEOBJECT_WEAK, &m_dwRegActObj);
  LclReportError("RegisterActiveObject", hr);
	_ASSERTE(SUCCEEDED(hr));

}

void CScdCOMWork::DoShutdownStuff()
{
  HRESULT hr;
  // Revoke Registrations
	hr = ::CoRevokeClassObject(m_dwRegClsObj);
	_ASSERTE(SUCCEEDED(hr));
  hr = ::RevokeActiveObject(m_dwRegActObj, NULL);
	_ASSERTE(SUCCEEDED(hr));

}

static void ReplaceSingleQuote(LPOLESTR lpDest, LPCOLESTR lp)
	{
		while (*lp)
		{
			*lpDest++ = *lp;
			if (*lp == OLESTR('\''))
				*lpDest++ = *lp;
			lp++;
		}
		*lpDest = NULL;
	}

static HRESULT GetModuleAsOLESTR(HINSTANCE hInst, LPOLESTR pszModuleQuote)
  {
	USES_CONVERSION;

  TCHAR szModule[_MAX_PATH];
	GetModuleFileName(hInst, szModule, _MAX_PATH);

	LPOLESTR pszModule;
	if ((hInst == NULL) || (hInst == GetModuleHandle(NULL))) // register as EXE
		{
		// Convert to short path to work around bug in NT4's CreateProcess
		TCHAR szModuleShort[_MAX_PATH];
		int cbShortName = GetShortPathName(szModule, szModuleShort, _MAX_PATH);

		if (cbShortName == _MAX_PATH)
			return E_OUTOFMEMORY;

		pszModule = (cbShortName == 0 || cbShortName == ERROR_INVALID_PARAMETER) ? T2OLE(szModule) : T2OLE(szModuleShort);
		}
	else
		pszModule = T2OLE(szModule);

	int nLen = ocslen(pszModule);
  ReplaceSingleQuote(pszModuleQuote, pszModule); // does the copy
  
  return S_OK;
  }

HRESULT CScdCOMWork::UpdateRegistry(HINSTANCE hInst,/*LPCOLESTR lpszRes,*/ BOOL bRegister)//, LPWSTR pwszAppID, LPWSTR pwszTypeLib)
  {
	USES_CONVERSION;



	//LPWSTR s1 = wszModule;
	//LPWSTR s2 = m_pCallback->pwszGetProgID();
    //LPWSTR s3 = m_pCallback->pwszGetVIProgID();
    //LPWSTR s4 = m_pCallback->pwszGetClassDesc();

	//return(S_FALSE);

	//ATLASSERT(pM != NULL);
	HRESULT hRes = S_OK;
	CComPtr<IRegistrar> p;
  hRes = CoCreateInstance(CLSID_Registrar, NULL, CLSCTX_INPROC_SERVER, IID_IRegistrar, (void**)&p);

	if (SUCCEEDED(hRes))
	  {
    OLECHAR wszModule[_MAX_PATH];
    hRes=GetModuleAsOLESTR(hInst, wszModule);
/*
	LPWSTR s1 = wszModule;
	LPWSTR s2 = m_pCallback->pwszGetProgID();
    LPWSTR s3 = m_pCallback->pwszGetVIProgID();
    LPWSTR s4 = m_pCallback->pwszGetClassDesc();
    LPOLESTR ppszCLSIDSTR1=NULL;  //Address of output variable that receives a
    StringFromCLSID(m_pCallback->clsidGetCLSID(),  &ppszCLSIDSTR1);

	return(S_FALSE);
*/

    if (FAILED(hRes))
      return hRes;

		p->AddReplacement(OLESTR("MODULE"),    wszModule);
		p->AddReplacement(OLESTR("PROGID"),    m_pCallback->pwszGetProgID());
		p->AddReplacement(OLESTR("VIPROGID"),  m_pCallback->pwszGetVIProgID());
		p->AddReplacement(OLESTR("CLASSDESC"), m_pCallback->pwszGetClassDesc());

    LPOLESTR ppszCLSIDSTR=NULL;  //Address of output variable that receives a
    StringFromCLSID(m_pCallback->clsidGetCLSID(),  &ppszCLSIDSTR);

    p->AddReplacement(OLESTR("CLASSID"),   ppszCLSIDSTR);
		p->AddReplacement(OLESTR("APPID"),     OLESTR("{EE3EC3B0-1866-4C37-9F37-E66D9BD8881F}"));//pwszAppID);
//    p->AddReplacement(OLESTR("TYPELIB"),   OLESTR("{700C6795-4F08-48D6-960A-88773A56E877}");


    OLECHAR wszResModule[_MAX_PATH];
    hRes=GetModuleAsOLESTR(m_hResInst, wszResModule);
    if (FAILED(hRes))
      return hRes;

    LPCOLESTR lpszRes=(LPCOLESTR )m_lResCBRegID;//

		LPCOLESTR szType = OLESTR("REGISTRY");
		if (HIWORD(lpszRes)==0)
		  {
			if (bRegister)
				hRes = p->ResourceRegister(wszResModule, ((UINT)LOWORD((DWORD)lpszRes)), szType);
			else
				hRes = p->ResourceUnregister(wszResModule, ((UINT)LOWORD((DWORD)lpszRes)), szType);
		  }
		else
		  {
			if (bRegister)
				hRes = p->ResourceRegisterSz(wszResModule, lpszRes, szType);
			else
				hRes = p->ResourceUnregisterSz(wszResModule, lpszRes, szType);
  		}

    CoTaskMemFree(ppszCLSIDSTR);
	  }

  return hRes;


  }
