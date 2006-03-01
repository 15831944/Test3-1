// ScdTerminals.h : Declaration of the CScdTerminals

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdTerminals

class ATL_NO_VTABLE CScdTerminals : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTerminals, &CLSID_ScdTerminals>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdTerminals, &__uuidof(IScdTerminals), &LIBID_ScdSlv, /* wMajor = */ 1, /* wMinor = */ 0>
  {
  public:
    CScdTerminals()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDTERMINALS)

    DECLARE_NOT_AGGREGATABLE(CScdTerminals)

    BEGIN_COM_MAP(CScdTerminals)
      COM_INTERFACE_ENTRY(IScdTerminals)
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

    CArray <LPDISPATCH, LPDISPATCH>       m_ITerminals;
    CArray <CETerminal*, CETerminal*>     m_Terminals;
    long                m_lTopoCount;
    long                m_lElectricsTopoCount;

    //eWhatNodes                        m_eWhat; 
    FlwNode           * m_pNode;
    //CETermStrip          * m_pTermStrip;
    eScdElecTypes       m_eTypes;
    eScdDirections      m_eDirns;

    void                BuildData();

  public:

    // IScdTerminals Methods
    STDMETHOD(Terminal)( VARIANT  WhichOne,  IScdTerminal * *  pItem);
    STDMETHOD(get__NewEnum)( LPUNKNOWN *  pVal);
    STDMETHOD(get_Count)( long *  pVal);
    STDMETHOD(SetObjectInfo)(DWORD  Node,  DWORD  Types,  DWORD  Dirn);
    STDMETHOD(Select)( eScdElecTypes  Types, eScdDirections  Direction);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdTerminals), CScdTerminals)
