// ScdSpVector.h : Declaration of the CScdSpVector

#pragma once
#include "resource.h"       // main symbols

#include "ScdMdl.h"
#include "sp_model.h"
#include "tagobj.h"

// CScdSpVector
//class SpModel;
class CScdSpVectorOwn;

class ATL_NO_VTABLE CScdSpVector : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public CComCoClass<CScdSpVector, &CLSID_ScdSpVector>,
  public ISupportErrorInfo,
  public IDispatchImpl<IScdSpVector, &IID_IScdSpVector, &LIBID_ScdMdl, /*wMajor =*/ 1, /*wMinor =*/ 0>
  {
  public:
    CScdSpVector();
    virtual ~CScdSpVector();

    DECLARE_REGISTRY_RESOURCEID(IDR_SCDSPVECTOR)


    BEGIN_COM_MAP(CScdSpVector)
      COM_INTERFACE_ENTRY(IScdSpVector)
      COM_INTERFACE_ENTRY2(IDispatch, IScdSpVector)
      COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    DECLARE_SCD(long);

    // ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();
    void FinalRelease(); 

  public:
    // Satisfy abstract classes 
    //void BuildDataDefn(DataDefnBlk & DDB) {};
    //virtual void   SetState(eScdMdlStateActs RqdState) {};
     //SpModel * m_pMdl;
     //SpModelOwner * m_pMdlOwn;
    CScdSpVectorOwn * m_pOwn;
    // Helpers
    double TFromT(VARIANT & T);
    double PFromP(VARIANT & P);
    double HFromH(VARIANT & H);

  public:
    // IScdSpVector Methods
    STDMETHOD(get_Vector)(/*[in]*/ VARIANT_BOOL EnthalpyBasis, /*[out, retval]*/ SAFEARRAY **pVal);
    STDMETHOD(put_Vector)(/*[in]*/ VARIANT_BOOL EnthalpyBasis, /*[in]*/ VARIANT pVal);//SAFEARRAY *pVal);
    STDMETHOD(get_Enthalpy)(/*[out, retval]*/ double *pVal);
    STDMETHOD(put_Enthalpy)(/*[in]*/ double newVal);
    STDMETHOD(get_Pressure)(/*[out, retval]*/ double *pVal);
    STDMETHOD(put_Pressure)(/*[in]*/ double newVal);
    STDMETHOD(get_Temperature)(/*[out, retval]*/ double *pVal);
    STDMETHOD(put_Temperature)(/*[in]*/ double newVal);
    STDMETHOD(get_SpecieMass)(/*[in]*/ LONG SpecieIndex, /*[out, retval]*/ double *pVal);
    STDMETHOD(put_SpecieMass)(/*[in]*/ LONG SpecieIndex, /*[in]*/ double newVal);
    STDMETHOD(get_Masses)(/*[out, retval]*/ SAFEARRAY **pVal);
    STDMETHOD(put_Masses)(/*[in]*/ VARIANT pVal);//SAFEARRAY *pVal);
    STDMETHOD(get_Mass)(/*[in]*/ LONG Phases, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_Density)(/*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_Volume)(/*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_SaturationP)(/*[in]*/ VARIANT TdK, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_SaturationT)(/*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_Concentration)(/*[in]*/ LONG SpecieIndex, /*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_msEnthalpy)(/*[in]*/ double RefT, /*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_msEntropy)(/*[in]*/ double RefT, /*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_msCp)(/*[in]*/ double RefT, /*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    //STDMETHOD(get_mlEnthalpy)(/*[in]*/ double RefT, /*[in]*/ LONG Phases, /*[in]*/ VARIANT TdK, /*[in]*/ VARIANT PkPa, /*[out, retval]*/ double *pVal);
    STDMETHOD(get_Properties)(IScdProperties ** pVal);
    STDMETHOD(get_Model)(BSTR* pVal);
    STDMETHOD(put_Model)(BSTR newVal);
    STDMETHOD(get_SpecieDefns)(IScdSpecieDefns ** pVal);
    STDMETHOD(get_Defn)(IScdSpVectorDefn ** pVal);
  };

#undef DllImportExport 

OBJECT_ENTRY_AUTO(__uuidof(ScdSpVector), CScdSpVector)
