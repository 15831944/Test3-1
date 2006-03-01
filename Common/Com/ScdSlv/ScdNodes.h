
// ScdNodes.h : Declaration of the CScdNodes

#ifndef __SCDNODES_H_
#define __SCDNODES_H_

#include "resource.h"       // main symbols

enum eWhatNodes { eWhatNodes_All, eWhatNodes_RmtNodes };

/////////////////////////////////////////////////////////////////////////////
// CScdNodes
class ATL_NO_VTABLE CScdNodes :
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdNodes, &CLSID_ScdNodes>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdNodes>,
  public IDispatchImpl<IScdNodes, &IID_IScdNodes, &LIBID_ScdSlv>
  {
  public:
    CScdNodes()
      {
      //m_pUnkMarshaler = NULL;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDNODES)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdNodes)
      COM_INTERFACE_ENTRY(IScdNodes)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      //COM_INTERFACE_ENTRY(IConnectionPointContainer)
    END_COM_MAP()
    //BEGIN_CONNECTION_POINT_MAP(CScdNodes)
    //END_CONNECTION_POINT_MAP()

    DECLARE_SCD(eScdNodeCodes);

    HRESULT FinalConstruct()
      {
      m_lTopoCount=-1;
      m_eWhat   = eWhatNodes_All;
      m_pNode   = NULL;
      m_eDirns  = eScdDirection_All;
      m_eTypes  = eScdNodeType_All;
      // delay creation of m_Nodes until first call
      return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
      }

    void FinalRelease()
      {
      //m_pUnkMarshaler.Release();
      for (long i=0; i<m_INodes.GetSize(); i++)
        m_INodes[i]->Release();
      }

    //CComPtr<IUnknown> m_pUnkMarshaler;
    CArray <LPDISPATCH, LPDISPATCH> m_INodes;
    CFlwNodeArray    m_FlwNodes;
    long             m_lTopoCount;

    eWhatNodes       m_eWhat; 

    FlwNode        * m_pNode;
    eScdDirections   m_eDirns;
    eScdNodeTypes    m_eTypes;

    long             BuildArray();

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    // IScdNodes
  public:
    STDMETHOD(Select)(/*[in]*/eScdNodeTypes eType);
    STDMETHOD(SetObjectInfo)(/*[in]*/ DWORD What, /*[in]*/ DWORD Node, /*[in]*/ DWORD Types, /*[in]*/ DWORD Dirn);
    STDMETHOD(Node)(VARIANT WhichOne, IScdNode** pItem);
    STDMETHOD(get_Count)(LONG * pVal);
    STDMETHOD(get__NewEnum)(IUnknown * * pVal);
    STDMETHOD(AddUnit)(BSTR bsClass, BSTR bsTag, IScdNode ** pUnit);
    STDMETHOD(AddLink)(BSTR bsClass, BSTR bsTag, BSTR bsSrcTag, BSTR bsSrcIO, BSTR bsDstTag, BSTR bsDstIO, IScdNode ** pLink);
    STDMETHOD(AddCable)(BSTR bsClass, BSTR bsTag, BSTR bsSrcTag, BSTR bsSrcIO, BSTR bsSrcTS, BSTR bsDstTag, BSTR bsDstIO, BSTR bsDstTS, IScdNode ** pLink);
  };

#endif //__SCDNODES_H_
