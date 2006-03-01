// ScdTermStrip.h : Declaration of the CScdTermStrip

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdTermStrip

class ATL_NO_VTABLE CScdTermStrip : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTermStrip, &CLSID_ScdTermStrip>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdTermStrip, &IID_IScdTermStrip, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  friend class CScdTerminal;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  public:
    CScdTermStrip()
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDTERMSTRIP)

    DECLARE_NOT_AGGREGATABLE(CScdTermStrip)

    BEGIN_COM_MAP(CScdTermStrip)
      COM_INTERFACE_ENTRY(IScdTermStrip)
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
      m_pTermStrip=NULL;
      m_eDirns=eScdDirection_All;
      m_eTypes=eScdElecType_All;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    CArray <LPDISPATCH, LPDISPATCH>       m_ITerminals;
    CArray <CETerminal*, CETerminal*>     m_Terminals;
    long              m_lTopoCount;
    long              m_lElectricsTopoCount;

    //eWhatNodes                        m_eWhat; 
    FlwNode         * m_pNode;
    CETermStrip     * m_pTermStrip;
    eScdElecTypes     m_eTypes;
    eScdDirections    m_eDirns;

    void              BuildData();

  public:
    // IScdTermStrip Methods
    STDMETHOD(Terminal)( VARIANT  WhichOne,  IScdTerminal * *  pItem);
    STDMETHOD(get__NewEnum)( LPUNKNOWN *  pVal);
    STDMETHOD(get_Count)( long *  pVal);
    STDMETHOD(SetObjectInfo)(DWORD  TermStrip,  DWORD  Types,  DWORD  Dirn);
    STDMETHOD(Select)( eScdElecTypes  Types, eScdDirections  Direction);
    STDMETHOD(get_Tag)(BSTR* pVal);
    STDMETHOD(get_Node)(IScdNode** pVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdTermStrip), CScdTermStrip)
