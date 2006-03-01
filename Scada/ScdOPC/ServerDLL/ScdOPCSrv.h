//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-2000, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  FSServer.h
//  Author     :  Jim Hansen
//
//  Subsystem  :  FactorySoft OPC Server DLL interface
//
//  Description:
//
//**************************************************************************

#if !defined(_FS_SERVERDLL_INCLUDED_)
#define _FS_SERVERDLL_INCLUDED_

#pragma once
#pragma pack( push, serverDLL, 2 )  // use 2-byte packing for these classes

//**************************************************************************
#ifdef _SCDOPCSRVIMPL    // only defined in the DLL
    #define CLASS_DECL_SERVERDLL  __declspec(dllexport)
#else
    #define CLASS_DECL_SERVERDLL  __declspec(dllimport)

//    #ifdef _UNICODE
//        #ifdef _DEBUG
//            #pragma comment(lib, "FSServerud.lib")
//        #else
//            #pragma comment(lib, "FSServeru.lib")
//        #endif
//    #else
//        #ifdef _DEBUG
//            #pragma comment(lib, "FSServerd.lib")
//        #else
//            #pragma comment(lib, "FSServer.lib")
//        #endif
//    #endif
// moved into end use file
//    #ifdef _UNICODE
//        #ifdef _DEBUG
//            #pragma comment(lib, "ScdOPCSrv.lib")
//        #else
//            #pragma comment(lib, "ScdOPCSrv.lib")
//        #endif
//    #else
//        #ifdef _DEBUG
//            #pragma comment(lib, "ScdOPCSrv.lib")
//        #else
//            #pragma comment(lib, "ScdOPCSrv.lib")
//        #endif
//    #endif
#endif

// DLL Exports
class COPCCallback;
class COPCBrowser;

BOOL StartFSServer(HINSTANCE hInstance, CLSID* pCLSID, bool WithThread);
BOOL StopFSServer();
STDAPI RegisterServer(LPCTSTR EXEName);
STDAPI UnregisterServer(void);
void SetCallbackObject(COPCCallback* pNewCallback );
BOOL FSServerInUse();
BOOL MatchPattern( LPCTSTR String, LPCTSTR Pattern, BOOL bCaseSensitive );

// Make a US English LCID
#define  ENG_LCID  MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),SORT_DEFAULT)
//**************************************************************************
// Class CTag represents a bit of data. Subclass CTag to create a structure
// that contains additional information for your application.

class CLASS_DECL_SERVERDLL CTag
{
public:
                  CTag();
   virtual        ~CTag();
   HRESULT        SetValue( VARIANT& newValue );
   virtual void   AddRef()   {InterlockedIncrement(&m_refCount);}
   virtual void   Release()  {InterlockedDecrement(&m_refCount);}
   BOOL  InUse()  { return m_refCount>0 ? TRUE : FALSE; }

   VARTYPE        m_nativeType;
   VARIANT        m_value;
   FILETIME       m_timestamp;
   WORD           m_quality;
   SHORT          m_accessRights;  // OPC_READABLE or'd with OPC_WRITEABLE

private:
   LONG           m_refCount;   // reference counting
};

//**************************************************************************
//

class CLASS_DECL_SERVERDLL COPCCallback
{
public:
   COPCCallback();
   virtual ~COPCCallback();

   // New 2.0 Item Properties
   LPWSTR AllocString(LPCTSTR source);
   virtual HRESULT QueryNumProperties(
                              LPCTSTR     name,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid);
   virtual HRESULT QueryAvailableProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pDataTypes);
   virtual HRESULT GetItemProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors);
   virtual HRESULT LookupItemIDs(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pszNewItemIDs,
                              HRESULT   * pErrors);

   virtual OPCSERVERSTATE GetServerState() {return OPC_STATUS_RUNNING;}
   virtual COPCBrowser* CreateBrowser() {return NULL;}
   virtual DWORD SetUpdateRate( DWORD newUpdateRate );
   virtual CTag* AddTag(   LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType);
   virtual HRESULT ValidateTag(
                           CTag* pTag,
                           LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType);
   virtual HRESULT Remove(
                           DWORD       dwNumItems,
                           CTag     ** ppTags);
   virtual LPCTSTR GetTagAccessPath( CTag * pTag );
   virtual LPCTSTR GetTagName( CTag * pTag );
   virtual BOOL    GetTagLimits(  CTag * pTag, double *pHigh, double *pLow );
   virtual HRESULT Scan(
                           DWORD       dwNumItems,
                           CTag     ** ppTags,
                           HRESULT   * pErrors);
   virtual HRESULT Read(
                           DWORD       dwNumItems,
                           CTag     ** ppTags,
                           HRESULT   * pErrors);
   virtual HRESULT ReadTag(CTag      * pTag);
   virtual HRESULT Write(
                           DWORD       dwNumItems,
                           CTag     ** ppTags,
                           VARIANT   * pItemValues,
                           HRESULT   * pErrors);
   virtual HRESULT WriteTag(
                           CTag      * pTag,
                           VARIANT   & value);
   virtual LPCTSTR GetErrorString(
                              HRESULT     dwError,
                              LCID        dwLocale);
   virtual LPCTSTR GetVendorString();

   virtual void ConnectNotify(bool Connecting) {}; //new method added by kga

   CRITICAL_SECTION     m_CS;   // protect access to tags created by this class
};

//*******************************************************************
class CLASS_DECL_SERVERDLL COPCBrowser
{
public:
   virtual ~COPCBrowser();
   virtual OPCNAMESPACETYPE QueryOrganization();
   virtual BOOL MoveUp();
   virtual BOOL MoveDown(LPCTSTR branch);
   virtual HRESULT GetNames(  OPCBROWSETYPE type,
                              LPCTSTR stringFilter,
                              VARTYPE dataTypeFilter,
                              DWORD accessFilter );
   virtual LPCTSTR GetItemID( LPCTSTR name );
   virtual LPCTSTR Next();
   virtual void Reset();
   virtual HRESULT GetAccessPaths( LPCTSTR name );
   virtual LPCTSTR NextAccessPath();
};

//*******************************************************************
// Use to Initialize a critical section and automatically delete it
// when the object goes out of scope (including exceptions, etc).
class CSObject
{
public:
   CSObject(CRITICAL_SECTION* lock)
      : pLock(lock) {InitializeCriticalSection(pLock);}
   ~CSObject()
      {DeleteCriticalSection(pLock);}
private:
   CRITICAL_SECTION* pLock;
};

//*******************************************************************
// Use to enter a critical section and automatically leave it
// when the object goes out of scope (including exceptions, etc).
class CSLock
{
public:
   CSLock(CRITICAL_SECTION* lock)
      : pLock(lock) {EnterCriticalSection(pLock);}
   ~CSLock()
      {LeaveCriticalSection(pLock);}
private:
   CRITICAL_SECTION* pLock;
};

#pragma pack( pop, serverDLL )

#endif
