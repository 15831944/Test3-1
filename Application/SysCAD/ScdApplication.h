
// ScdApplication.h : Declaration of the CScdApplication

#ifndef __SCDAPPLICATION_H_
#define __SCDAPPLICATION_H_

#include "resource.h"       // main symbols
#include "..\..\common\com\scdif\scdif.h"       // main symbols
#include "..\..\common\com\scdslv\scdatl.h"       // main symbols
#include "ScdAppCP.h"
#include "..\..\common\com\scdif\scdcomevts.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdApplicationFactory

#include <map>
typedef std::map<std::string,IUnknown*> CScdAppmapObjects;

class CScdApplicationFactory : 
  public CComClassFactory, 
  public IOleItemContainer
  //, public ITheObjectsTable  
  {
  public:
    CScdApplicationFactory() {};
    BEGIN_COM_MAP(CScdApplicationFactory)
      COM_INTERFACE_ENTRY(IClassFactory)
      COM_INTERFACE_ENTRY2(IParseDisplayName, IOleItemContainer)
      COM_INTERFACE_ENTRY(IOleItemContainer)
      //COM_INTERFACE_ENTRY(ITheObjectsTable)
    END_COM_MAP()
    //
    // IParseDisplayName
    STDMETHOD(ParseDisplayName)(
    IBindCtx *pbc,     //Pointer to bind context
    LPOLESTR pszDisplayName, //Pointer to string containing display name
    ULONG *pchEaten,   //Pointer to length, in characters, of display name
    IMoniker **ppmkOut //Address of output variable that receives the 
    //resulting IMoniker interface pointer
    )
      {
      HRESULT hr=::CreateItemMoniker(OLESTR("!"),
        pszDisplayName+1,
        ppmkOut);
      if(SUCCEEDED(hr))
        {
        *pchEaten = wcslen(pszDisplayName);
        }else
        {
        *pchEaten = 0;
        }
      return hr;
      }
    //IOleItemContainer
    STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown **ppenum)
      {
      return E_NOTIMPL;
      }
    STDMETHOD(LockContainer)(BOOL fLock)
      {
      return E_NOTIMPL;
      }
    STDMETHOD(GetObject)(LPOLESTR pszItem,     //Pointer to name of the object requested
      DWORD dwSpeedNeeded,  //Speed requirements on binding
      IBindCtx *pbc,        //Pointer to bind context object to be used
      REFIID riid,          //Reference to the identifier of the 
      // interface pointer desired
      void **ppvObject      //Address of output variable that receives 
      // the interface pointer requested in riid
      );

    STDMETHOD(GetObjectStorage)(LPOLESTR pszItem,IBindCtx *pbc,REFIID riid,void **ppvStorage)
      {
      return E_NOTIMPL;
      }
    STDMETHOD(IsRunning)(LPOLESTR pszItem)	
      {
      return E_NOTIMPL;
      }

    ////	ITheObjectsTable
    //STDMETHOD(RemoveObject)(BSTR bstrObjectName);
  
  protected:
    static CScdAppmapObjects sm_mapObjects;
  };

/////////////////////////////////////////////////////////////////////////////
// CScdApplication

//class CScdApplicationClassFactory : public CComClassFactory {};

class ATL_NO_VTABLE CScdApplication :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdApplication, &CLSID_ScdApplication>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdApplication>,
  public IDispatchImpl<IScdApplication, &IID_IScdApplication, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>,
  public CProxy_IScdApplicationEvents< CScdApplication >
{
public:
  CScdApplication();

DECLARE_REGISTRY_RESOURCEID(IDR_SCDAPPLICATION)
//DECLARE_REGISTRY_RESOURCEID_SCDEXE_AS_SERVER(IDR_SCDAPPLICATION)
DECLARE_GET_CONTROLLING_UNKNOWN()

//DECLARE_CLASSFACTORY_SINGLETON(CScdApplication);
//DECLARE_CLASSFACTORY_EX(CScdApplicationFactory);
DECLARE_PROTECT_FINAL_CONSTRUCT()

// This breaks the registration
//DECLARE_OBJECT_DESCRIPTION("SysCAD Application Class");

BEGIN_COM_MAP(CScdApplication)
  COM_INTERFACE_ENTRY(IScdApplication)
//DEL   COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(ISupportErrorInfo)
  COM_INTERFACE_ENTRY(IConnectionPointContainer)
  //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
  COM_INTERFACE_ENTRY2(IDispatch, IScdApplication)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
  COM_INTERFACE_ENTRY(IScdASyncEvents)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CScdApplication)
CONNECTION_POINT_ENTRY(DIID__IScdApplicationEvents)
END_CONNECTION_POINT_MAP()

DECLARE_SCD(long)

#ifdef _DEBUG
BEGIN_CATEGORY_MAP(CScdApplication)
  IMPLEMENTED_CATEGORY(CATID_SysCADAppObject)
END_CATEGORY_MAP()
#endif

//DECLARE_CLASSFACTORY_EX(CScdApplicationClassFactory)

  HRESULT FinalConstruct();
  void FinalRelease();

  //CComPtr<IUnknown> m_pUnkMarshaler;
  CComPtr<IScdSolver  > m_pSolver;
  CComPtr<IScdProject > m_pProject;
  CComPtr<IScdOptions > m_pOptions;
  CComPtr<IScdLicenseApp > m_pLicense;

// ISupportsErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScdApplication
public:
  STDMETHOD(CloseProject)(/*[in,defaultvalue(FALSE)]*/ VARIANT_BOOL SaveIt);
  STDMETHOD(get_Options)(/*[out, retval]*/ IScdOptions ** pOptions);
  STDMETHOD(get_License)(/*[out, retval]*/ IScdLicenseApp ** pLicense);
  STDMETHOD(CreateProject)(/*[in]*/ BSTR CfgFolder, /*[in]*/ BSTR PrjFolder, VARIANT_BOOL WithVersion, VARIANT_BOOL RemoveOld, /*[out, retval]*/ IScdProject ** pProject);
  STDMETHOD(OpenProject)(/*[in]*/ BSTR PrjFolder, /*[out, retval]*/ IScdProject ** pProject);
  STDMETHOD(get_Messages)(/*[out, retval]*/ IScdMessages ** pMessages);
  STDMETHOD(get_Test)(/*[out, retval]*/ IScdTest ** pTest);
  STDMETHOD(get_Debug)(/*[out, retval]*/ IScdDebug ** pDebug);


// IScdASyncEvents
  STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
    {
    CScdCOCmdBase::DoEventMsg(Evt, Data);
    return S_OK;
    }
  virtual void FireTheEvent(long Evt, long Data);

  STDMETHOD(SetWindowState)(eScdWndStateCmds ReqdState);
  STDMETHOD(get_Project)(IScdProject** pVal);
};

#endif //__SCDAPPLICATION_H_
