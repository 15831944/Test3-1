// ScdTerminal.h : Declaration of the CScdTerminal

#pragma once
#include "resource.h"       // main symbols

#include "ScdSlv.h"


// CScdTerminal

class ATL_NO_VTABLE CScdTerminal : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdTerminal, &CLSID_ScdTerminal>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdTerminal, &IID_IScdTerminal, &LIBID_ScdSlv, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  friend class CScdTerminal;
  friend class CScdTermStrip;
  friend class CScdTermStrips;
  public:
    CScdTerminal() 
      {
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDTERMINAL)

    DECLARE_NOT_AGGREGATABLE(CScdTerminal)

    BEGIN_COM_MAP(CScdTerminal)
      COM_INTERFACE_ENTRY(IScdTerminal)
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
      m_pTerminal=NULL;
      return S_OK;
      }

    void FinalRelease() 
      {
      }

    long          m_lTopoCount;
    long          m_lElectricsTopoCount;

    FlwNode     * m_pNode;
    CETerminal  * m_pTerminal;

    void          BuildData();

  public:

    STDMETHOD(SetObjectInfo)(DWORD  Terminal);
    STDMETHOD(get_Tag)(BSTR* pVal);
    STDMETHOD(get_TermStrip)(IScdTermStrip** pVal);
    STDMETHOD(get_Voltage)(IScdComplex** pVal);
    //STDMETHOD(get_Current)(IScdComplex** pVal);
    STDMETHOD(get_Node)(IScdNode** pVal);
  };

//OBJECT_ENTRY_AUTO(__uuidof(ScdTerminal), CScdTerminal)
