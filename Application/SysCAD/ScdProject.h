
// ScdProject.h : Declaration of the CScdProject

#ifndef __SCDPROJECT_H_
#define __SCDPROJECT_H_

#include "resource.h"       // main symbols
#include "scdapplication.h"

/////////////////////////////////////////////////////////////////////////////
// CScdProject
class ATL_NO_VTABLE CScdProject :
  public CScdCOCmdBase,
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdProject, &CLSID_ScdProject>,
  public ISupportErrorInfo,
  public IConnectionPointContainerImpl<CScdProject>,
  public IDispatchImpl<IScdProject, &IID_IScdProject, &LIBID_ScdApp>,
  public IDispatchImpl<IScdASyncEvents, &IID_IScdASyncEvents, &LIBID_ScdIF>
  {
  public:
    CScdProject() : CScdCOCmdBase(WMU_COM_APP)
      {
      //m_pUnkMarshaler = NULL;
      m_iSeqNo=-1;
      }

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDPROJECT)
    DECLARE_GET_CONTROLLING_UNKNOWN()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CScdProject)
      COM_INTERFACE_ENTRY(IScdProject)
      //DEL   COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
      COM_INTERFACE_ENTRY(IConnectionPointContainer)
      //COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
      COM_INTERFACE_ENTRY2(IDispatch, IScdProject)
      COM_INTERFACE_ENTRY(IScdASyncEvents)
    END_COM_MAP()
    BEGIN_CONNECTION_POINT_MAP(CScdProject)
    END_CONNECTION_POINT_MAP()

    DECLARE_SCD(long);

#ifdef _DEBUG
    BEGIN_CATEGORY_MAP(CScdProject)
      IMPLEMENTED_CATEGORY(CATID_SysCADAppObject)
    END_CATEGORY_MAP()
#endif

    HRESULT FinalConstruct();
    void FinalRelease();

    //CComPtr<IUnknown> m_pUnkMarshaler;
    CComPtr<IScdGraphics  > m_pGraphics ;
    CComPtr<IScdTrends    > m_pTrends   ;
    CComPtr<IScdHistorian > m_pHistorian;
    CComPtr<IScdAppTags   > m_pAppTags  ;
    CComPtr<IScdSolver    > m_pSolver   ;
    CComPtr<IScdReports   > m_pReports  ;
    //CComPtr<IScdSnapshot  > m_pSnapshot ;
    CComPtr<IScdReplay    > m_pReplay   ;
    CComPtr<IScdOPCServer > m_pOPCServer;
    CComPtr<IScdIOMarshal > m_pIOMarshal;
    CComPtr<IScdDDEServer > m_pDDEServer;

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  public:
    // IScdASyncEvents
    STDMETHOD(DoEventMsg)(LONG Evt, LONG Data)
      {
      CScdCOCmdBase::DoEventMsg(Evt, Data);
      return S_OK;
      }
    virtual void FireTheEvent(long Evt, long Data);

    // IScdProject
    STDMETHOD(Save)(/*[in]*/ BSTR Filename, /*[in]*/ VARIANT_BOOL NewVersion);
    STDMETHOD(get_Tags)(/*[out, retval]*/ IScdAppTags ** pTags);
    STDMETHOD(get_SpecieDefns)(/*[out, retval]*/ IScdSpecieDefns ** pSpecieDefns);
    STDMETHOD(get_Solver)(/*[out, retval]*/ IScdSolver ** pSolver);
    //STDMETHOD(get_Snapshot)(/*[out, retval]*/ IScdSnapshot ** pSnapshot);
    STDMETHOD(get_Reports)(/*[out, retval]*/ IScdReports ** pReport);
    STDMETHOD(get_Historian)(/*[out, retval]*/ IScdHistorian ** pHistorian);
    STDMETHOD(get_Trends)(/*[out, retval]*/ IScdTrends ** pTrends);
    STDMETHOD(get_Graphics)(/*[out, retval]*/ IScdGraphics ** pGraphics);

    STDMETHOD(get_OPCServer)(IScdOPCServer ** pVal);
    STDMETHOD(get_IOMarshal)(IScdIOMarshal ** pVal);
    STDMETHOD(get_DDEServer)(IScdDDEServer** pVal);
    STDMETHOD(get_CfgFolder)(BSTR* pVal);
    STDMETHOD(get_PrjFolder)(BSTR* pVal);
    STDMETHOD(ExportNeutralDB)(eScdNDBOptions Options, BSTR GraphicsDatabase, BSTR ModelDatabase);
    STDMETHOD(ImportNeutralDB)(eScdNDBOptions Options, BSTR GraphicsDatabase, BSTR ModelDatabase, IScdTagFixup * TagFixups);
    STDMETHOD(SaveSnapshot)(BSTR FileName, long SeqStart);
    STDMETHOD(LoadSnapshot)(BSTR FileName, long NoInSeq);

    long    m_iSeqNo;
    CString m_sSnapName;

  };

#endif //__SCDPROJECT_H_
