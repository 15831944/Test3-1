
// ScdSolver.h : Declaration of the CScdSolver

#ifndef __SCDSOLVER_H_
#define __SCDSOLVER_H_

#include "resource.h"       // main symbols
#include "ScdSlvWrap.h"
#include "..\scdif\scdcomevts.h"
#include "scd_wm.h"
#include "sfe_base.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSolver
class ATL_NO_VTABLE CScdSolver :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdSolver, &CLSID_ScdSolver>,
  public ISupportErrorInfo,
  //public IConnectionPointContainerImpl<CScdSolver>,
  public IDispatchImpl<IScdSolver, &IID_IScdSolver, &LIBID_ScdSlv>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
  {
  public:
    CScdSolver() :
        CScdCOCmdBase(WMU_COM_SLV)
          {
          //m_pUnkMarshaler = NULL;
          }

        DECLARE_REGISTRY_RESOURCEID(IDR_SCDSOLVER)
        DECLARE_GET_CONTROLLING_UNKNOWN()

        DECLARE_PROTECT_FINAL_CONSTRUCT()
        //DECLARE_CLASSFACTORY_SINGLETON(CScdSolver)

        BEGIN_COM_MAP(CScdSolver)
          COM_INTERFACE_ENTRY(IScdSolver)
          //DEL   COM_INTERFACE_ENTRY(IDispatch)
          COM_INTERFACE_ENTRY(ISupportErrorInfo)
          //COM_INTERFACE_ENTRY(IConnectionPointContainer)
          //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
          //COM_INTERFACE_ENTRY_AGGREGATE(IID_IScdMessages, m_pMsgs.p)
          COM_INTERFACE_ENTRY_AGGREGATE(IID_IScdSpecieDefns, m_pSpecieDefns.p)
          COM_INTERFACE_ENTRY_AGGREGATE(IID_IScdDebug, m_pDebug.p)
          COM_INTERFACE_ENTRY_AGGREGATE(IID_IScdTest, m_pTest.p)
          COM_INTERFACE_ENTRY2(IDispatch, IScdSolver)
          COM_INTERFACE_ENTRY(IScdASyncEvents)
        END_COM_MAP()
        //BEGIN_CONNECTION_POINT_MAP(CScdSolver)
        //END_CONNECTION_POINT_MAP()

        DECLARE_SCD(long);

        //#ifdef _DEBUG
        BEGIN_CATEGORY_MAP(CScdSolver)
          IMPLEMENTED_CATEGORY(CATID_SysCADModelObject)
        END_CATEGORY_MAP()
        //#endif

        HRESULT FinalConstruct();
        void FinalRelease();

        CScdSlvWrap m_SlvWrap;

        // Aggregated
        //CComPtr<IUnknown> m_pUnkMarshaler;

        // Local
        //CComPtr<IScdMessages  > m_pMsgs;
        CComPtr<IScdSpecieDefns > m_pSpecieDefns;
        CComPtr<IScdDebug       > m_pDebug;
        CComPtr<IScdTest      	> m_pTest;
        CComPtr<IScdSlvTags   	> m_pMTags;
        //CComPtr<IScdNodes   	  > m_pNodes;
        //CComPtr<IScdUnits     	> m_pUnits;
        //CComPtr<IScdLinks     	> m_pLinks;
        CComPtr<IScdProbal    	> m_pProbal;
        CComPtr<IScdDynamic   	> m_pDynamic;

        eScdEvalSeq_Options   m_SeqOptions;
        CEvalOrderArray       m_SeqInfo;

        // ISupportsErrorInfo
        STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

        // IScdSolver
  public:
    STDMETHOD(get_RunMode)(/*[out, retval]*/ eScdMode *pVal);
    STDMETHOD(put_RunMode)(/*[in]*/ eScdMode newVal);
    STDMETHOD(GetEvalSequenceIOItem)(/*[in]*/ long Index, /*[in]*/ long IOItem, /*[out]*/ eScdEvalSeqIO_Class * eClass, /*[out]*/ eScdEvalSeqIO_Flags * eFlags, /*[out]*/ long * RemoteIndex, /*[out]*/ BSTR * RemoteDesc);
    STDMETHOD(GetEvalSequenceItem)(/*[in]*/ long Index, /*[out]*/ BSTR * Tag, /*[out]*/ BSTR * Class, /*[out]*/ long * IOCount);
    STDMETHOD(CollectEvalSequenceInfo)(/*[in]*/ eScdEvalSeq_Options eReqdSeq, /*[out]*/ long * ItemCount);
    STDMETHOD(CreateSpModel)(/*[in]*/ BSTR ProgID, /*[out, retval]*/ IScdTaggedObject ** pModel);
    STDMETHOD(get_Dynamic)(/*[out, retval]*/ IScdDynamic * *pVal);
    STDMETHOD(get_Probal)(/*[out, retval]*/ IScdProbal * *pVal);
    STDMETHOD(get_Nodes)(eScdNodeTypes eType, /*[out, retval]*/ IScdNodes ** pNodes);
    //STDMETHOD(get_Links)(/*[out, retval]*/ IScdLinks ** pLinks);
    //STDMETHOD(get_Units)(/*[out, retval]*/ IScdUnits ** pUnits);
    STDMETHOD(get_Debug)(/*[out, retval]*/ IScdDebug ** pDebug);
    STDMETHOD(get_Test)(/*[out, retval]*/ IScdTest ** pTest);
    STDMETHOD(get_Tags)(/*[out, retval]*/ IScdSlvTags ** pTags);
    STDMETHOD(InitialiseCfg)(/*[in]*/ BSTR CfgFileName);
    STDMETHOD(InitialiseDB)(/*[in]*/ BSTR DBFileName, SAFEARRAY *parrSpecies);
    STDMETHOD(get_Messages)(/*[out, retval]*/ IScdMessages**pVal);
    STDMETHOD(get_SpecieDefns)(/*[out, retval]*/ IScdSpecieDefns**pVal);
    STDMETHOD(get_Conversions)(/*[out, retval]*/IScdConversion ** pVal);
    // IScdUnits

    // IScdASyncEvents
    STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
      {
      CScdCOCmdBase::DoEventMsg(Evt, Data);
      return S_OK;
      }
    virtual void FireTheEvent(long Evt, long Data);
    STDMETHOD(get_Time)(eScdTimeFormat Fmt, VARIANT * pVal);
    STDMETHOD(put_Time)(eScdTimeFormat Fmt, VARIANT newVal);
  };

#endif //__SCDSOLVER_H_
