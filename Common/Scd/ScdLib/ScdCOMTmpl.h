
#ifndef __SCDCOMTMPL_H
#define __SCDCOMTMPL_H
//#pragma once

#include "..\..\..\SMDK\Include\MsgLog.h"

class CScdComException 
  {
  public:
    HRESULT m_hResult;
    CString m_Desc;
    CString m_FnName;
    CScdComException(HRESULT hr, LPCTSTR Desc, LPCTSTR FnName)
      {
      m_hResult=hr;
      m_Desc=Desc;
      m_FnName=FnName;
      }
    CScdComException(HRESULT hr, LPCTSTR Desc)
      {
      m_hResult=hr;
      m_Desc=Desc;
      }
  };

#undef appSCD_COMENTRY       
#undef dllSCD_COMENTRY       
#undef SCD_COMEXIT       

#define HRESULT_OK(Code)    MAKE_HRESULT(0, FACILITY_ITF, Code)
#define HRESULT_INFO(Code)  MAKE_HRESULT(1, FACILITY_ITF, Code)
#define HRESULT_WARN(Code)  MAKE_HRESULT(2, FACILITY_ITF, Code)
#define HRESULT_ERR(Code)   MAKE_HRESULT(3, FACILITY_ITF, Code)


class CScdCallEntryExitData
  {
  public:
    HRESULT   m_hResult;                                                               
    CString * m_pCatchStr;                                                             
    long      m_LastRetCode;                                                           
  public:
    CScdCallEntryExitData()
      {
      m_hResult = S_OK;                                                                
      m_pCatchStr = NULL;                                                              
      m_LastRetCode = 0;                                                               
      };
    ~CScdCallEntryExitData() 
      { 
      };
  };

template <class T> 
class CScdCallEntryExit
  {
  protected:
    const CLSID & m_clsid;
    CScdCallEntryExitData & m_Data;
    LPCTSTR m_MyFnName;
  public:
    CScdCallEntryExit(LPCTSTR MyFnName, CScdCallEntryExitData & Data, const CLSID & clsid) :  m_clsid(clsid), m_Data(Data)
      { 
      gs_MsgLog.ComCallStart(); 
      m_MyFnName=MyFnName;
      m_Data.m_hResult = S_OK;                                                                 
      m_Data.m_pCatchStr = NULL;                                                               
      m_Data.m_LastRetCode = 0;                                                                
      if (gs_MsgLog.TestOption(LogOption_COMCallsEntry) && gs_MsgLog.GetComCallLevel()==0)                      
        LogNote("Com", 0, "CallEntry:%s", m_MyFnName);                                           
      };
    ~CScdCallEntryExit() 
      { 
      if (FAILED(m_Data.m_hResult))                                                                     
        {
        if (m_Data.m_pCatchStr)                                                                                
          {                                                                                           
          //Error(*pCatchStr, GUID_NULL, hResult);                                                      
          if (gs_MsgLog.TestOption(LogOption_COMCallsExit))                                                  
            if (m_Data.m_hResult==E_UNEXPECTED)
              LogNote("Com", 0, "CallErrorExit:%s %s",m_MyFnName, "Unknown Exception");    
            else
              LogNote("Com", 0, "CallErrorExit(%08x):%s %s", m_Data.m_hResult, m_MyFnName);               
          delete m_Data.m_pCatchStr;
          m_Data.m_pCatchStr=NULL;
          }                                                                                           
        else 
          {                                                                                           
          if (gs_MsgLog.TestOption(LogOption_COMCallsExit) && gs_MsgLog.GetComCallLevel()==0)                          
            LogNote("Com", 0, "CallExit(%08x):%s", m_Data.m_hResult,m_MyFnName);                                 
          //Error(m_MyFnName, GUID_NULL/*CLSID_ScdTest*/, m_Data.m_hResult);                                       
          }                                                                                           
        }
      gs_MsgLog.ComCallEnd(); 
      };
    BOOL HResultOK() { return SUCCEEDED(m_Data.m_hResult); };
    HRESULT ReturnH(HRESULT RetCode)                                  
      {                                                                                          
      if (SUCCEEDED(RetCode))
        return S_OK;
      m_Data.m_LastRetCode = -1;                                                           
      m_Data.m_hResult = (HRESULT)RetCode;

      char S[MsgBuffLen];                                                                        
      sprintf(S, "Error %08x",m_Data.m_hResult);
      ComLogError(m_MyFnName, 0, S);                                                                  
      return AtlReportError(m_clsid, S, GUID_NULL, m_Data.m_hResult );        
      }                                                                                          
    HRESULT ReturnH(HRESULT RetCode, LPCTSTR fmt, ...)                                  
      {                    
      if (SUCCEEDED(RetCode))
        return S_OK;
      m_Data.m_LastRetCode = -1;                                                           
      m_Data.m_hResult = (HRESULT)RetCode;
      char S[MsgBuffLen];                                                                        
      va_list argptr;                                                                            
      va_start(argptr,fmt);                                                                      
      vsprintf(S, fmt, argptr);                                                                  
      va_end(argptr);                                                                            
      ComLogError(m_MyFnName, 0, S);                                                                  
      return AtlReportError(m_clsid, S, GUID_NULL, m_Data.m_hResult );        
      }                                                                                          
    HRESULT Return(T RetCode, LPCTSTR fmt, ...)                                  
      {                    
      if ((long)RetCode==0)
        return S_OK;
      if (HRESULT_FACILITY(m_Data.m_LastRetCode)==FACILITY_NULL)
        {
        m_Data.m_LastRetCode = (long)RetCode;                                                             
        m_Data.m_hResult = HRESULT_ERR((long)RetCode);
        }
      else
        {
        m_Data.m_LastRetCode = -1;                                                           
        m_Data.m_hResult = (HRESULT)RetCode;
        }
      char S[MsgBuffLen];                                                                        
      va_list argptr;                                                                            
      va_start(argptr,fmt);                                                                      
      vsprintf(S, fmt, argptr);                                                                  
      va_end(argptr);                                                                            
      ComLogError(m_MyFnName, 0, S);                                                                  
      return AtlReportError(m_clsid, S, GUID_NULL, m_Data.m_hResult );        
      }                                                                                          
  };

#define DECLARE_SCD(T)                                                                          \
  CScdCallEntryExitData m_ScdData;                                                              \
  STDMETHOD(get_ReturnCode)(T * pVal) { *pVal= (T)m_ScdData.m_LastRetCode; m_ScdData.m_LastRetCode=0; return S_OK; };  


#define dllSCD_COMENTRY(T)                                                                  \
  AFX_MANAGE_STATE(AfxGetStaticModuleState())                                               \
  static LPCTSTR MyFnName=__FUNCTION__;                                                              \
  CScdCallEntryExit<T> Scd(MyFnName, m_ScdData, GetObjectCLSID());                              \
  try                                                                                           

#define appSCD_COMENTRY(T)                                                                  \
  static LPCTSTR MyFnName=__FUNCTION__;                                                              \
  CScdCallEntryExit<T> Scd(MyFnName, m_ScdData, GetObjectCLSID());                              \
  try                                                                                           

#define dllSCD_COMENTRYGET(T, pVal)                                                         \
  AFX_MANAGE_STATE(AfxGetStaticModuleState())                                               \
  static LPCTSTR MyFnName=__FUNCTION__;                                                              \
  CScdCallEntryExit<T> Scd(MyFnName, m_ScdData, GetObjectCLSID());                              \
  if (pVal==NULL)                                                                               \
    return Scd.ReturnH(E_POINTER);                                                              \
  try                   

#define appSCD_COMENTRYGET(T, pVal)                                                         \
  static LPCTSTR MyFnName=__FUNCTION__;                                                              \
  CScdCallEntryExit<T> Scd(MyFnName, m_ScdData, GetObjectCLSID());                              \
  if (pVal==NULL)                                                                               \
    return Scd.ReturnH(E_POINTER);                                                              \
  try                   

#define SCD_COMEXIT                                                                             \
  catch (CScdComException e)                                                                    \
    {                                                                                           \
    m_ScdData.m_pCatchStr =new CString(e.m_Desc);                                               \
    if (e.m_FnName.GetLength()>0)                                                               \
      {                                                                                         \
      (*m_ScdData.m_pCatchStr)+="\nIn Function:";                                               \
      (*m_ScdData.m_pCatchStr)+=e.m_FnName;                                                     \
      }                                                                                         \
    (*m_ScdData.m_pCatchStr)+="\nCalled From:";                                                 \
    (*m_ScdData.m_pCatchStr)+=MyFnName;                                                         \
    m_ScdData.m_hResult=e.m_hResult;                                                            \
    }                                                                                           \
  catch (...)                                                                                   \
    {                                                                                           \
    m_ScdData.m_pCatchStr =new CString("Unknown Exception\nIn:");                               \
    (*m_ScdData.m_pCatchStr)+=MyFnName;                                                         \
    m_ScdData.m_hResult=E_UNEXPECTED;                                                           \
    }                                                                                           \
  return m_ScdData.m_hResult;

#define SCD_THROW(hr, Desc)                                                                \
  {                                                                                             \
  HRESULT LclHR=(hr);                                                                           \
  if (gs_MsgLog.TestOption(LogOption_COMCallsInternal))                                                \
    LogNote("Com", 0, "SCD_THROW HRESULT(Throw)=%08x %s", LclHR, Desc);                      \
  AtlTrace("SCD_THROW %08x", LclHR);                                                            \
  throw CScdComException (LclHR, Desc);                                                         \
  }                                                                                             
                                                                                                
#define SCD_TOFFN(hr, Desc)                                                         \
  {                                                                                             \
  HRESULT LclHR=(hr);                                                                           \
  if (FAILED(LclHR))                                                                            \
    {                                                                                           \
    if (gs_MsgLog.TestOption(LogOption_COMCallsInternal))                                              \
      LogNote("Com", 0, "SCD_TOFFN HRESULT(Throw)=%08x %s [%s]", LclHR, Desc, FnName);       \
    AtlTrace("SCD_TOFFN %08x", LclHR);                                                          \
    throw CScdComException (hr, Desc, __FUNCTION__);                                                 \
    }                                                                                           \
  }                                                                                             
                                                                                                
#define SCD_TOF(hr, Desc)                                                                   \
  {                                                                                             \
  HRESULT LclHR=(hr);                                                                           \
  if (FAILED(LclHR))                                                                            \
    {                                                                                           \
    /*XDoBreak(); */                                                                                \
    if (gs_MsgLog.TestOption(LogOption_COMCallsInternal))                                              \
      LogNote("Com", 0, "SCD_TOF HRESULT(Throw)=%08x %s", LclHR, Desc);                    \
    AtlTrace("SCD_TOF %08x", LclHR);                                                            \
    throw CScdComException (LclHR, Desc);                                                       \
    }                                                                                           \
  }                                                                                             

extern __declspec(dllimport) HRESULT DoFC_ROF(HRESULT hr);

#define FC_ROF(hr)                                                                              \
  {                                                                                             \
  HRESULT LclHR=(hr);                                                                           \
  if (FAILED(LclHR))																																						\
		return DoFC_ROF(LclHR); 																																		\
  }

#define FC_TESTLICENSE(IsLic){ FC_ROF((IsLic) ? S_OK:E_ACCESSDENIED); };                                                              

#define SCD_ROF(hr)                \
  {                                \
  HRESULT LclHR=(hr);              \
  if (FAILED(LclHR))               \
    return Scd.ReturnH(LclHR);     \
  }

// ======================================================================
//
//
//
// ======================================================================
// compliments of Don Box in ESSENTIAL COM


class CGlobalInterfaceTable
  {
  //GlobalInterfacePointer 
    IGlobalInterfaceTable *m_pGIT;
  public:

    CGlobalInterfaceTable()
      {
      m_pGIT=NULL;
      };
    ~CGlobalInterfaceTable()
      {
      Term();
      };

    void Init()
      {
      HRESULT hr;
      hr=CoCreateInstance(CLSID_StdGlobalInterfaceTable,
                       NULL,
                       CLSCTX_INPROC_SERVER,
                       IID_IGlobalInterfaceTable,
                       (void **)&m_pGIT);
      if (FAILED(hr))
        {
        DoBreak();
        }
      };
    void Term()
      {
      if (m_pGIT)
        m_pGIT->Release();
      m_pGIT=NULL;
      };
    IGlobalInterfaceTable * operator->() 
      {
      if (m_pGIT==NULL)
        Init();
      return m_pGIT;
      };
    BOOL IsOK()
      {
      if (m_pGIT==NULL)
        Init();
      return (m_pGIT!=NULL);
      };

  };

extern CGlobalInterfaceTable g_GIT;

// ======================================================================

//class GIScdTaggedObject;
//extern IGlobalInterfaceTable *g_GIT.m_pGIT;

template <class Itf, const IID* piid>
class GlobalInterfacePointer //: public GlobalInterfacePointer // : public CComPtr<Itf> 
  {
    DWORD m_dwCookie; // the GIT cookie
    BOOL  m_IsClone;
    // prevent misuse
    GlobalInterfacePointer(const GlobalInterfacePointer&);
    void operator =(const GlobalInterfacePointer&);
  public:
    CComPtr<Itf> m_pInitialComPtr;

    // start as invalid cookie
    GlobalInterfacePointer(void) : m_dwCookie(0), m_IsClone(FALSE) { }

    // start with auto-globalized local pointer
    GlobalInterfacePointer(Itf *pItf, HRESULT& hr) 
      : m_dwCookie(0) 
      { 
      m_IsClone = FALSE;
      hr = Globalize(pItf); 
      }

    // auto-unglobalize
    ~GlobalInterfacePointer(void) 
      {
      if (m_dwCookie)
        Unglobalize(); 
      }

    CComPtr<Itf> & InitialComPtr() { return m_pInitialComPtr; };
    Itf *operator ->(void) { return m_pInitialComPtr.p; }
    
    HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL )
      {
      HRESULT hr=m_pInitialComPtr.CoCreateInstance(rclsid, pUnkOuter, dwClsContext);
      if (FAILED(hr))
        return hr;
      return Globalize(m_pInitialComPtr.p);
      };


    // register an interface pointer in GIT
    HRESULT Globalize(Itf *pItf=NULL) 
      {
      if (pItf==NULL)
        pItf=m_pInitialComPtr.p;
      //if (!(g_GIT.IsOK() && (m_dwCookie == 0)))
      //  {
      //  _asm int 3;
      //  }
      ASSERT(g_GIT.IsOK() && (m_dwCookie == 0));
      return g_GIT->RegisterInterfaceInGlobal(pItf, *piid, &m_dwCookie);
      }

    // revoke an interface pointer in GIT
    HRESULT Unglobalize(void) 
      {
      if (!m_IsClone)
        {
        ASSERT(g_GIT.IsOK() && (m_dwCookie != 0));
        HRESULT hr=g_GIT->RevokeInterfaceFromGlobal(m_dwCookie);
        m_dwCookie = 0;
        return hr;
        }
      m_dwCookie = 0;
      return S_OK;
      }
    HRESULT TryUnglobalize(void) 
      {
      if (!m_IsClone && m_dwCookie)
        {
        ASSERT(g_GIT.IsOK() && (m_dwCookie != 0));
        HRESULT hr=g_GIT->RevokeInterfaceFromGlobal(m_dwCookie);
        m_dwCookie = 0;
        return hr;
        }
      return S_OK;
      }
    // get a local interface pointer from GIT
    HRESULT Localize(Itf **ppItf) const 
      {
      ASSERT(g_GIT.IsOK() && (m_dwCookie != 0));
      return g_GIT->GetInterfaceFromGlobal(m_dwCookie, 
         *piid,(void**)ppItf);
      }

    void Clone(DWORD Cookie) // the GIT cookie
      {
      ASSERT(m_dwCookie==0);
      m_IsClone=TRUE;
      m_dwCookie=Cookie; // the GIT cookie
      };
    // convenience methods
    bool IsOK(void) const { return m_dwCookie != 0; }
    DWORD GetCookie(void) const { return m_dwCookie; }
  };

#define GIP(Itf) GlobalInterfacePointer<Itf, &IID_##Itf>

// ======================================================================
//
//
//
// ======================================================================

template <class Itf, const IID* piid>
class LocalInterfacePointer 
  {
    CComPtr<Itf> m_pItf; // temp imported pointer
  // prevent misuse
    LocalInterfacePointer(const LocalInterfacePointer&);
    LocalInterfacePointer & operator = (const LocalInterfacePointer&);
  public:
    LocalInterfacePointer(const GlobalInterfacePointer<Itf, 
      piid>& rhs)//,HRESULT& hr)
      {
      //TRACE1("LocalInterfacePointer1 %08x\n", rhs.GetCookie());
      HRESULT hr = rhs.Localize(&m_pItf.p);
      ASSERT(SUCCEEDED(hr));
      }

    LocalInterfacePointer(DWORD dwCookie)//, HRESULT& hr) 
      {
      //TRACE1("LocalInterfacePointer2 %08x\n", dwCookie);
      ASSERT(g_GIT.IsOK());
      HRESULT hr = g_GIT->GetInterfaceFromGlobal(dwCookie, *piid, 
        (void**)&m_pItf.p);
      ASSERT(SUCCEEDED(hr));
      }
    ~LocalInterfacePointer(void)
      {
      //TRACE("~LocalInterfacePointer\n");
      if (m_pItf.p) 
        m_pItf.Release(); 
      }

    class SafeItf : public Itf 
      {
      STDMETHOD_(ULONG, AddRef)(void) = 0;  // hide
      STDMETHOD_(ULONG, Release)(void) = 0; // hide
      };

    SafeItf *GetInterface(void) const  
      { 
      return (SafeItf*)m_pItf.p; 
      }

    SafeItf *operator ->(void) const 
      { 
      ASSERT(m_pItf.p != 0);
      return GetInterface();
      }
  };

#define LIP(Itf) LocalInterfacePointer<Itf, &IID_##Itf>

#define DefineGLIP(Itf) typedef GlobalInterfacePointer<Itf, &IID_##Itf>  G##Itf; \
                        typedef LocalInterfacePointer<Itf, &IID_##Itf>  L##Itf; 


// ======================================================================
//
//
//
// ======================================================================

DefineGLIP(IScdTaggedObject);

#define SCD_LclStr(A,B) _bstr_t lbs##A(B); LPCTSTR A=lbs##A;
#define SCD_Copy2BSTR(Dst, Src) { _bstr_t s(Src); *Dst=s.copy();}

// ======================================================================
//
//
//
// ======================================================================
#endif