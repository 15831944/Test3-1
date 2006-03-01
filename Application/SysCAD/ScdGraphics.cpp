// ScdGraphics.cpp : Implementation of CScdGraphics
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdGraphics.h"
#include "licbase.h"

#include "cmd_mngr.h"
#include "mainfrm.h"
//#include "optoff.h"

//#include "..\..\scapp1\docroot.h"
//#include "..\..\scapp1\project.h"

/////////////////////////////////////////////////////////////////////////////
// CScdGraphics

STDMETHODIMP CScdGraphics::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] =
    {
    &IID_IScdGraphics
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }
void CScdGraphics::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

CScdGraphics::CScdGraphics() : \
CScdCOCmdBase(WMU_COM_APP)
  {
  //m_pUnkMarshaler = NULL;
  }

HRESULT CScdGraphics::FinalConstruct()
  {
  FC_TESTLICENSE(gs_License.AllowCOMApp());
  return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
  }

void CScdGraphics::FinalRelease()
  {
  //m_pUnkMarshaler.Release();
  }

STDMETHODIMP CScdGraphics::Page(VARIANT WhichOne, IScdGraphic ** pItem)
  {
  appSCD_COMENTRY(eScdGraphicsCodes)
    {
    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;

        CDocComInfo DCI;
        DCI.m_DoWhat=CDocComInfo::DCI_GetCount;
        if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
          {
          return Scd.ReturnH(E_FAIL);
          }
        long N=DCI.m_lCount;
        if (i>0 && i<=N)
          {
          DCI.m_DoWhat=CDocComInfo::DCI_GetTitle;
          DCI.m_lIndex=i-1;
          if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
            {
            return Scd.ReturnH(E_FAIL);
            }
          CComPtr<IScdGraphic> pGrf;
          SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
          SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
          SCD_ROF(pGrf.CopyTo(pItem));
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Title, WhichOne.bstrVal);

        CDocComInfo DCI;
        DCI.m_DoWhat=CDocComInfo::DCI_FindTitle;
        DCI.m_sTitle=Title;
        if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
          {
          return Scd.ReturnH(E_FAIL);
          }
        long N=DCI.m_lCount;
        if (DCI.m_lIndex>=0)
          {
          CComPtr<IScdGraphic> pGrf;
          SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
          SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
          SCD_ROF(pGrf.CopyTo(pItem));
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      default:
        return Scd.ReturnH(E_INVALIDARG);
      }
    *pItem=NULL;
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdGraphics::get_Count(LONG * pVal)
  {
  appSCD_COMENTRYGET(eScdGraphicsCodes, pVal)
    {
    CDocComInfo DCI;
    DCI.m_DoWhat=CDocComInfo::DCI_GetCount;
    if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
      {
      return Scd.ReturnH(E_FAIL);
      }
    *pVal=DCI.m_lCount;
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdGraphics::Add(BSTR bsName, IScdGraphic ** pItem)
  {
  appSCD_COMENTRY(eScdGraphicsCodes)
    {
    SCD_LclStr(Name, bsName);

    CDocComInfo DCI;
    DCI.m_DoWhat=CDocComInfo::DCI_Add;
    DCI.m_sTitle=Name;
    if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)==0)
      {
      CComPtr<IScdGraphic> pGrf;
      SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
      SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
      SCD_ROF(pGrf.CopyTo(pItem));
      return S_OK;
      }
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdGraphics::Remove(VARIANT WhichOne)
  {
  appSCD_COMENTRY(eScdGraphicsCodes)
    {
    Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdGraphics::get__NewEnum(IUnknown * * pVal)
  {
  appSCD_COMENTRYGET(eScdGraphicsCodes, pVal)
    {               
    CDocComInfo DCI;
    DCI.m_DoWhat=CDocComInfo::DCI_GetCount;
    if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
      {
      return Scd.ReturnH(E_FAIL);
      }
    long N=DCI.m_lCount;

    VARIANT* pVar = new VARIANT[N];
    for(int i = 0; i < N; i++)
      {
      DCI.m_DoWhat=CDocComInfo::DCI_GetTitle;
      DCI.m_lIndex=i;
      if (/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
        {
        return Scd.ReturnH(E_FAIL);
        }
      //CComPtr<IScdGraphic> pGrf;
      //SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
      //SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
      //pVar[i].vt = VT_DISPATCH;
      //pVar[i].pdispVal = pGrf;
      //pVar[i].pdispVal->AddRef();

      IScdGraphic * p;
      SCD_ROF(::CoCreateInstance(CLSID_ScdGraphic, NULL, CLSCTX_ALL, IID_IScdGraphic, (void**)&p));
      SCD_ROF(p->put_Name(_bstr_t(DCI.m_sTitle)));
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdGraphic")
        pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = pDisp;
      p->Release();

      }
    // Now make the enum.
    typedef CComObject< CComEnum< IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy< VARIANT > > > enumVar;
    enumVar* pEnumVar = new enumVar;
    pEnumVar->Init(&pVar[0], &pVar[N], NULL, AtlFlagCopy);
    delete[] pVar;
    // return the enum.
    return pEnumVar->QueryInterface(IID_IUnknown, (void**)pVal);
    }
  SCD_COMEXIT
  }

