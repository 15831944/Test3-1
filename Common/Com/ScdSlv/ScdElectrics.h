// ScdElectrics.h : Declaration of the CScdElectrics

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"
//#include "_IScdElectricsEvents_CP.h"


// CScdElectrics

class ATL_NO_VTABLE CScdElectrics : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdElectrics, &CLSID_ScdElectrics>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdElectrics>,
  //public CProxy_IScdElectricsEvents<CScdElectrics>, 
  public IDispatchImpl<IScdElectrics, &IID_IScdElectrics, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>

  {
  public:
    CScdElectrics()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDELECTRICS)

    DECLARE_NOT_AGGREGATABLE(CScdElectrics)

    BEGIN_COM_MAP(CScdElectrics)
      COM_INTERFACE_ENTRY(IScdElectrics)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()

    //BEGIN_CONNECTION_POINT_MAP(CScdElectrics)
    //  CONNECTION_POINT_ENTRY(__uuidof(_IScdElectricsEvents))
    //END_CONNECTION_POINT_MAP()
    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    DECLARE_SCD(eScdNodeCodes);

    FlwNode * m_pNode;
    long      m_lTopoCount;

    HRESULT FinalConstruct()
      {
      m_pNode=NULL;
      m_lTopoCount=-1;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

  public:
    STDMETHOD(get_TermStrips)(eScdElecTypes Types, eScdDirections Direction, IScdTermStrips** pVal);
    STDMETHOD(get_Wires)(eScdElecTypes Types, IScdWires** pVal);
    STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD newVal);

    STDMETHOD(get_Terminals)(eScdElecTypes Types, IScdTerminals** pVal);
    STDMETHOD(get_Components)(eScdElecTypes Types, IScdComponents** pVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdElectrics), CScdElectrics)
