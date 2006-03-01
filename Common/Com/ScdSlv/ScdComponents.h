// ScdComponents.h : Declaration of the CScdComponents

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdComponents

class ATL_NO_VTABLE CScdComponents : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdComponents, &CLSID_ScdComponents>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdComponents, &IID_IScdComponents, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdComponents()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDCOMPONENTS)

    DECLARE_NOT_AGGREGATABLE(CScdComponents)

    BEGIN_COM_MAP(CScdComponents)
      COM_INTERFACE_ENTRY(IScdComponents)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      m_lTopoCount=-1;
      m_lElectricsTopoCount=-1;

      m_pNode=NULL;
      m_eTypes=eScdElecType_All;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    CArray <LPDISPATCH, LPDISPATCH>         m_IComponents;
    CArray <CEComponent*, CEComponent*>     m_Components;
    long                m_lTopoCount;
    long                m_lElectricsTopoCount;

    //eWhatNodes                        m_eWhat; 
    FlwNode           * m_pNode;
    //CETermStrip          * m_pTermStrip;
    eScdElecTypes       m_eTypes;
    //eScdDirections      m_eDirns;

    void                BuildData();

    //CArray <LPDISPATCH, LPDISPATCH> m_IComponents;
    //FlwNode     * m_pNode;
    //long          m_lTopoCount;

    //void          BuildData();

  public:

    STDMETHOD(Component)( VARIANT  WhichOne,  IScdComponent * *  pItem);
    STDMETHOD(get__NewEnum)( LPUNKNOWN *  pVal);
    STDMETHOD(get_Count)( long *  pVal);
    STDMETHOD(SetObjectInfo)(DWORD  Node,  DWORD  Types);
    STDMETHOD(Select)( eScdElecTypes  Types);

  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdComponents), CScdComponents)
