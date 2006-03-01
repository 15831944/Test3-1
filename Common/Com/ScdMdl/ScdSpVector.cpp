// ScdSpVector.cpp : Implementation of CScdSpVector

#include "stdafx.h"
#include "ScdSpVector.h"
#include "sp_model.h"
#include ".\scdspvector.h"

//===========================================================================
//
//
//
//===========================================================================

#define DllImportExport 
DEFINE_TAGOBJ(CScdSpVectorOwn);
//DEFINE_SPARES(CScdSpVector, 100);

class CScdSpVectorOwn : public SpModelOwner
  {
  public:
    CScdSpVectorOwn(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
      SpModelOwner(pClass_, pTag_, pAttach, eAttach, false)
        {
        pModel=(pSpModel)SpModelClass.ConstructGroup(DefaultModel(), NULL, SpModelDefTag, this, TOA_Embedded|TOA_Hidden);
        //pModel->pGlblView=&GlblView;
        //SetView(SVV_AsMassFlow);
        //SetUsage(SPMU_Flow);
        };
      virtual ~CScdSpVectorOwn() {};

  public:
    // Satisfy abstract classes 
    void BuildDataDefn(DataDefnBlk & DDB) {};
    virtual void   SetState(eScdMdlStateActs RqdState) {};
  };

//IMPLEMENT_SPARES(CScdSpVector, 100);
IMPLEMENT_TAGOBJ(CScdSpVectorOwn, "ScdSpVector", "ScdSpVector", "", "", "", TOC_SYSTEM,
                 "Scd Sp Vector", "Scd Specie Vector");

#undef DllImportExport 


//===========================================================================
//
//
//
//===========================================================================


STDMETHODIMP CScdSpVector::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdSpVector
    };

  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

//---------------------------------------------------------------------------

CScdSpVector::CScdSpVector()// : SpModelOwner(&CScdSpVectorClass, "SpVector", NULL, TOA_Free, false)
  {
  }
//---------------------------------------------------------------------------

CScdSpVector::~CScdSpVector() 
  {
  }
//---------------------------------------------------------------------------

HRESULT CScdSpVector::FinalConstruct()
  {
  m_pOwn = new CScdSpVectorOwn(&CScdSpVectorOwnClass, "SpVector", NULL, TOA_Free);
  //m_pMdl=;
  return S_OK;
  }

//---------------------------------------------------------------------------

void CScdSpVector::FinalRelease() 
  {
  delete m_pOwn;
  }

//---------------------------------------------------------------------------
//     Helpers
double CScdSpVector::TFromT(VARIANT & T) 
  {
  switch (T.vt)
    {
    case VT_R8    : return T.dblVal;
    case VT_R4    : return T.fltVal;
    case VT_I2    : return T.iVal;
    case VT_I4    : return T.lVal;
    case VT_ERROR : return m_pOwn->Model()->Temp();
    default: SCD_THROW(E_INVALIDARG, "Bad Type in TFromT");
    };
  return 0;
  }

//---------------------------------------------------------------------------

double CScdSpVector::PFromP(VARIANT & P) 
  {
  switch (P.vt)
    {
    case VT_R8    : return P.dblVal;
    case VT_R4    : return P.fltVal;
    case VT_I2    : return P.iVal;
    case VT_I4    : return P.lVal;
    case VT_ERROR : return m_pOwn->Model()->Press();
    default: SCD_THROW(E_INVALIDARG, "Bad Type in PFromP");
    };
  return 0;
  }

//---------------------------------------------------------------------------

double CScdSpVector::HFromH(VARIANT & H) 
  {
  INCOMPLETECODE(); // sort out VT_NULL 
  switch (H.vt)
    {
    case VT_R8    : return H.dblVal;
    case VT_R4    : return H.fltVal;
    case VT_I2    : return H.iVal;
    case VT_I4    : return H.lVal;
    case VT_ERROR : return m_pOwn->Model()->Press();
    default: SCD_THROW(E_INVALIDARG, "Bad Type in HFromH");
    };
  return 0;
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Density(long Phases, VARIANT TdK, VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->Rho(PhMask(Phases),TFromT(TdK),PFromP(PkPa));
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Volume(long Phases, VARIANT TdK, VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->Volume(Phases,TFromT(TdK),PFromP(PkPa));
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_SaturationP(VARIANT TdK, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->SaturationP(TFromT(TdK));
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_SaturationT(VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->SaturationT(PFromP(PkPa));
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Concentration(long SpecieIndex, long Phases, VARIANT TdK, VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->m_M[SpecieIndex]/GTZ(m_pOwn->Model()->Volume(Phases,TFromT(TdK),PFromP(PkPa)));
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

//STDMETHODIMP CScdSpVector::get_C2S(double *pVal)
//  {
//  dllSCD_COMENTRYGET(long, pVal)
//    {
//    *pVal=456;
//    }
//  SCD_COMEXIT
//  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Mass(long Phases, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->Mass(Phases);
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Masses(/*[out, retval]*/ SAFEARRAY **pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    SAFEARRAY *pSA=NULL;
    SAFEARRAYBOUND bound[1]={SDB.Count(),0};
    pSA=SafeArrayCreate(VT_R8, 1, bound);

    long Cnt=bound[0].cElements;
    for (long i=0; i<Cnt; i++)
      {
      double d=m_pOwn->Model()->VMass[i];
      SafeArrayPutElement(pSA, &i, &d);
      }

    *pVal=pSA;
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::put_Masses(/*[in]*/ VARIANT newVal)
  {
  dllSCD_COMENTRY(long)
    {
    if ((newVal.vt&VT_R8) && (newVal.vt&VT_ARRAY))
      {    
      HRESULT hr=S_OK;
      SAFEARRAY *pSA=newVal.parray;
      //SAFEARRAY *pSA=pnewVal;

      double HUGEP *pDble;
      if (pSA->cDims!=1)
        return E_FAIL;

      // Get a pointer to the elements of the array.
      SCD_TOF(SafeArrayAccessData(pSA, (void HUGEP**)&pDble), "Accessing Data");

      long Cnt=pSA->rgsabound->cElements;
      long s=pSA->rgsabound->lLbound;

      for (long i=0; i<Cnt; i++)
        m_pOwn->Model()->VValue[i]=pDble[s++];

      SCD_TOF(SafeArrayUnaccessData(pSA), "UnAccessing Data");
      //CHECK NBNBNB Why is this not called as per example COM&ATL3.0 pg465
      // SafeArrayDestroy(pSA);

      return S_OK;
      }
    return DISP_E_TYPEMISMATCH;
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_SpecieMass(long SpecieIndex, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->VMass[SpecieIndex];
    //m_pOwn->Model()->dwValidFlags |= SMVF_ComOK; // assume all will be fetched
    }
  SCD_COMEXIT
  } 

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::put_SpecieMass(long SpecieIndex, double newVal)
  {
  dllSCD_COMENTRY(long)
    {
    m_pOwn->Model()->VValue[SpecieIndex]=newVal;
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

//STDMETHODIMP CScdSpVector::get_MassVersionNo(/*[out, retval]*/ LONG *pVal)
//  {
//  dllSCD_COMENTRYGET(long, pVal)
//    {
//    *pVal=m_pOwn->Model()->m_dwMassVersionNo;
//    }
//  SCD_COMEXIT
//  }
//
////---------------------------------------------------------------------------
//
//STDMETHODIMP CScdSpVector::AttachVirtualProperties(long gpCookieSpProps, /*[in]*/ BSTR TheGUID)
//  {
//  dllSCD_COMENTRY(long)
//    {
//    m_pOwn->Model()->m_gpVirtualProps.Clone(gpCookieSpProps);
//		CString S(TheGUID);
//    }
//  SCD_COMEXIT
//  }
//
////---------------------------------------------------------------------------
//
//STDMETHODIMP CScdSpVector::DetachVirtualProperties()
//  {
//  dllSCD_COMENTRY(long)
//    {
//    if (m_pOwn->Model()->m_gpVirtualProps.IsOK())
//      m_pOwn->Model()->m_gpVirtualProps.Unglobalize();
//    }
//  SCD_COMEXIT
//  }

//---------------------------------------------------------------------------

//STDMETHODIMP CScdSpVector::get_RawModelPtr(/*[out, retval]*/ long *pVal)
//  {
//  dllSCD_COMENTRYGET(long, pVal)
//    {
//    *pVal=(long)dynamic_cast<TaggedObject*>(m_pMdl);
//    }
//  SCD_COMEXIT
//  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Temperature(double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->Temp();
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::put_Temperature(double newVal)
  {
  dllSCD_COMENTRY(long)
    {
    double PkPa=m_pOwn->Model()->Press();
    m_pOwn->Model()->SetTempPress(newVal, PkPa);
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Pressure(double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->Press();
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::put_Pressure(double newVal)
  {
  dllSCD_COMENTRY(long)
    {
    double TdK=m_pOwn->Model()->Temp();
    m_pOwn->Model()->SetTempPress(TdK, newVal);
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Enthalpy(double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->totHz(som_ALL, m_pOwn->Model()->Temp(), m_pOwn->Model()->Press());
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::put_Enthalpy(double newVal)
  {
  dllSCD_COMENTRY(long)
    {
    m_pOwn->Model()->Set_totHz(newVal);
    }
  SCD_COMEXIT
  }

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_Vector(/*[in]*/ VARIANT_BOOL EnthalpyBasis, /*[out, retval]*/ SAFEARRAY **pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    SAFEARRAY *pSA=NULL;
    SAFEARRAYBOUND bound[1]={SDB.Count()+3,0};
    pSA=SafeArrayCreate(VT_R8, 1, bound);

    double d;
    long Cnt=bound[0].cElements;
    for (long i=0; i<SDB.Count(); i++)
      {
      d=m_pOwn->Model()->VMass[i];
      SafeArrayPutElement(pSA, &i, &d);
      }

    // NBNB EnthalpyBasis is ignored on "get"
    d=m_pOwn->Model()->totHz(som_ALL, m_pOwn->Model()->Temp(), m_pOwn->Model()->Press());
    SafeArrayPutElement(pSA, &i, &d);

    i++;
    d=m_pOwn->Model()->Temp();
    SafeArrayPutElement(pSA, &i, &d);

    i++;
    d=m_pOwn->Model()->Press();
    SafeArrayPutElement(pSA, &i, &d);

    *pVal=pSA;
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::put_Vector(/*[in]*/ VARIANT_BOOL EnthalpyBasis, /*[in]*/ VARIANT newVal)
  {
  dllSCD_COMENTRY(long)
    {
    if ((newVal.vt&VT_R8) && (newVal.vt&VT_ARRAY))
      {    
      HRESULT hr=S_OK;
      SAFEARRAY *pSA=newVal.parray;

      double HUGEP *pDble;
      if (pSA->cDims!=1)
        return E_FAIL;

      // Get a pointer to the elements of the array.
      SCD_TOF(SafeArrayAccessData(pSA, (void HUGEP**)&pDble), "Accessing Data");

      long Cnt=pSA->rgsabound->cElements;
      long s=pSA->rgsabound->lLbound;

      for (long i=0; i<SDB.Count(); i++)
        m_pOwn->Model()->VValue[i]=pDble[s++];

      double h=pDble[s++];
      double t=pDble[s++];
      double p=pDble[s++];
      if (EnthalpyBasis)
        {
        m_pOwn->Model()->SetPress(p);
        m_pOwn->Model()->Set_totHz(h);
        }
      else
        m_pOwn->Model()->SetTempPress(t,p);

      SCD_TOF(SafeArrayUnaccessData(pSA), "UnAccessing Data");
      //CHECK NBNBNB Why is this not called as per example COM&ATL3.0 pg465
      // SafeArrayDestroy(pSA);

      return S_OK;
      }
    return DISP_E_TYPEMISMATCH;
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_msEnthalpy(double RefT, LONG Phases, VARIANT TdK, VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->msHs(PhMask(Phases),TFromT(TdK),PFromP(PkPa));
    //if (RefT>=0)
    //  *pVal=-m_pOwn->Model()->msHf(PhMask(Phases),RefT,PFromP(PkPa));
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_msEntropy(double RefT, LONG Phases, VARIANT TdK, VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->msSf(PhMask(Phases),TFromT(TdK),PFromP(PkPa));
    if (RefT>=0)
      *pVal=-m_pOwn->Model()->msSf(PhMask(Phases),RefT,PFromP(PkPa));
    }
  SCD_COMEXIT
  };

//---------------------------------------------------------------------------

STDMETHODIMP CScdSpVector::get_msCp(double RefT, LONG Phases, VARIANT TdK, VARIANT PkPa, double *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=m_pOwn->Model()->msCp(PhMask(Phases),TFromT(TdK),PFromP(PkPa));
    //if (RefT>=0)
    //  *pVal=-m_pOwn->Model()->msCp(PhMask(Phases),RefT,PFromP(PkPa));
    }
  SCD_COMEXIT
  };


STDMETHODIMP CScdSpVector::get_Properties(IScdProperties ** pVal)
  {
  dllSCD_COMENTRY(long)
    {
    IScdProperties * p;
    ::CoCreateInstance(CLSID_ScdProperties, NULL, CLSCTX_ALL, IID_IScdProperties, (void**)&p);
    CPropertyBase *pB=(CPropertyBase *)m_pOwn/*->Model()*/;
    p->SetObjectInfo(1, (DWORD)pB);
    *pVal=p;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpVector::get_Model(BSTR* pVal)
  {
  dllSCD_COMENTRY(long)
    {
    _bstr_t X(m_pOwn->Model()->ClassId());
    *pVal=X.copy();
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpVector::put_Model(BSTR newVal)
  {
  dllSCD_COMENTRY(long)
    {
    CString S=newVal;
    m_pOwn->ChangeModel((LPTSTR)(LPCTSTR)S, true);
    if (S.CompareNoCase(m_pOwn->Model()->ClassId())!=0)
      Scd.ReturnH(E_FAIL); 
    if (0)
      {
      S += " --> ";
      S += m_pOwn->Model()->ClassId();
      AfxMessageBox(S);
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSpVector::get_SpecieDefns(IScdSpecieDefns ** pVal)
  {
  dllSCD_COMENTRY(long)
    {
		IScdSpecieDefns * p;
		::CoCreateInstance(CLSID_ScdSpecieDefns, NULL, CLSCTX_ALL, IID_IScdSpecieDefns, (void**)&p);
		//p->SetObjectInfo(m_pProps->PropertyMap()[m_iPropNo].m_iCnv, 0);
		*pVal=p;
		return S_OK;
    }		
	SCD_COMEXIT
  }

STDMETHODIMP CScdSpVector::get_Defn(IScdSpVectorDefn ** pVal)
  {
  dllSCD_COMENTRY(long)
    {
		IScdSpVectorDefn * p;
		::CoCreateInstance(CLSID_ScdSpVectorDefn, NULL, CLSCTX_ALL, IID_IScdSpVectorDefn, (void**)&p);
		//p->SetObjectInfo(m_pProps->PropertyMap()[m_iPropNo].m_iCnv, 0);
		*pVal=p;
		return S_OK;
    }		
	SCD_COMEXIT
  }
