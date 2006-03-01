// ScdTermStrips.h : Declaration of the CScdTermStrips

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"
#include "power.h"


// CScdTermStrips

class ATL_NO_VTABLE CScdTermStrips : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTermStrips, &CLSID_ScdTermStrips>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdTermStrips, &IID_IScdTermStrips, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  friend class CScdTerminal;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  public:
    CScdTermStrips()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDTERMSTRIPS)

    DECLARE_NOT_AGGREGATABLE(CScdTermStrips)

    BEGIN_COM_MAP(CScdTermStrips)
      COM_INTERFACE_ENTRY(IScdTermStrips)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    DECLARE_SCD(long);

    HRESULT FinalConstruct()
      {
      m_pNode=NULL;
      m_eTypes=eScdElecType_All;
      m_eDirns=eScdDirection_All;
      m_lTopoCount=-1;
      m_lElectricsTopoCount=-1;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    CArray <LPDISPATCH, LPDISPATCH>       m_ITermStrips;
    CArray <CETermStrip*, CETermStrip*>   m_TermStrips;
    long              m_lTopoCount;
    long              m_lElectricsTopoCount;

    //eWhatNodes                        m_eWhat; 
    FlwNode         * m_pNode;
    eScdDirections    m_eDirns;
    eScdElecTypes     m_eTypes;

    void              BuildData();

  public:
    // IScdTermStrips Methods
    STDMETHOD(TermStrip)( VARIANT  WhichOne,  IScdTermStrip * *  pItem);
    STDMETHOD(get__NewEnum)( LPUNKNOWN *  pVal);
    STDMETHOD(get_Count)( long *  pVal);
    STDMETHOD(SetObjectInfo)( DWORD  Node,  DWORD  Types,  DWORD  Dirn);
    STDMETHOD(Select)( eScdElecTypes  Types, eScdDirections  Direction);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdTermStrips), CScdTermStrips)
