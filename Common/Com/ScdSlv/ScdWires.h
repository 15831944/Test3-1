// ScdWires.h : Declaration of the CScdWires

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdWires

class ATL_NO_VTABLE CScdWires : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdWires, &CLSID_ScdWires>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdWires, &IID_IScdWires, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdWires()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDWIRES)

    DECLARE_NOT_AGGREGATABLE(CScdWires)

    BEGIN_COM_MAP(CScdWires)
      COM_INTERFACE_ENTRY(IScdWires)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      m_lTopoCount          = -1;
      m_lElectricsTopoCount = -1;
      m_pNode               = NULL;
      m_eTypes              = eScdElecType_All;
      m_eDirns              = eScdDirection_All;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    CArray <LPDISPATCH, LPDISPATCH>       m_IWires;
    CArray <CEWire*, CEWire*>             m_Wires;
    long                m_lTopoCount;
    long                m_lElectricsTopoCount;

    //eWhatNodes          m_eWhat; 
    FlwNode           * m_pNode;
    //CETermStrip       * m_pTermStrip;
    eScdElecTypes       m_eTypes;
    eScdDirections      m_eDirns;

    void                BuildData();

  public:
    // IScdWires Methods
    STDMETHOD(Wire)( VARIANT  WhichOne,  IScdWire * *  pItem);
    STDMETHOD(get__NewEnum)( LPUNKNOWN *  pVal);
    STDMETHOD(get_Count)( long *  pVal);
    STDMETHOD(SetObjectInfo)(DWORD  Node,  DWORD  Types,  DWORD  Dirn);
    STDMETHOD(Select)( eScdElecTypes  Types, eScdDirections Direction);
    STDMETHOD(SetWire)(LONG No, BSTR Tag, BSTR SrcTermStrip, BSTR SrcTerminal, BSTR DstTermStrip, BSTR DstTerminal);
    STDMETHOD(AutoWire)(BSTR SrcTermStrip, BSTR DstTermStrip);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdWires), CScdWires)
