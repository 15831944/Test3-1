
// ScdGraphic.h : Declaration of the CScdGraphic

#ifndef __SCDGRAPHIC_H_
#define __SCDGRAPHIC_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CScdGraphic
class ATL_NO_VTABLE CScdGraphic :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdGraphic, &CLSID_ScdGraphic>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdGraphic>,
  public IDispatchImpl<IScdGraphic, &IID_IScdGraphic, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
  {
  public:
    CScdGraphic() : CScdCOCmdBase(WMU_COM_APP)
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDGRAPHIC)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdGraphic)
      COM_INTERFACE_ENTRY(IScdGraphic)
      //COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
      //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
      COM_INTERFACE_ENTRY2(IDispatch, IScdGraphic)
      COM_INTERFACE_ENTRY(IScdASyncEvents)
    END_COM_MAP()
    BEGIN_CONNECTION_POINT_MAP(CScdGraphic)
    END_CONNECTION_POINT_MAP()

    DECLARE_SCD(eScdGraphicCodes);

#ifdef _DEBUG
    BEGIN_CATEGORY_MAP(CScdGraphic)
      IMPLEMENTED_CATEGORY(CATID_SysCADAppObject)
    END_CATEGORY_MAP()
#endif

    CString  m_sName;

    HRESULT FinalConstruct()
      {
      return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
      }

    void FinalRelease()
      {
      //m_pUnkMarshaler.Release();
      }

    //CComPtr<IUnknown> m_pUnkMarshaler;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdGraphic
  public:
    STDMETHOD(AddLinkM)(/*[in]*/ BSTR Tag, /*[in]*/ eScdLinkDrawMethods Method);
    STDMETHOD(AddLinkA)(/*[in]*/ BSTR Tag, /*[in]*/ VARIANT Points3D);
    STDMETHOD(Zoom)(/*[in]*/ eScdZooms zoom);
    STDMETHOD(AddUnit)(/*[in]*/ BSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ BSTR Symbol, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation);
    STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
    STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);

    // IScdASyncEvents
    STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
      {
      CScdCOCmdBase::DoEventMsg(Evt, Data);
      return S_OK;
      }
    virtual void FireTheEvent(long Evt, long Data);

    STDMETHOD(InsertGroup)(BSTR Library, BSTR Group, DOUBLE X, DOUBLE Y, DOUBLE Z, DOUBLE XScale, DOUBLE YScale, DOUBLE ZScale, IScdTagFixup * TagFixups, eScdNDBOptions Options);
  };

#endif //__SCDGRAPHIC_H_
