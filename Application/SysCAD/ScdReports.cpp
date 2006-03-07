// ScdReports.cpp : Implementation of CScdReports
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdReports.h"
#include "..\..\common\com\scdif\Scdcomcmds.h"

#include "execlib.h"
#include "mainfrm.h"

/////////////////////////////////////////////////////////////////////////////
// CScdReports

STDMETHODIMP CScdReports::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] =
    {
    &IID_IScdReports
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdReports::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_Report_GetValues: 
    case ComCmd_Report_Trend: 
    case ComCmd_Report_SetValues: 
    case ComCmd_Report_SaveAndClose: 
    case ComCmd_Report_ExecuteMacro: 
      Fire_OnReportProcessed(Data); break;
    };
  };

STDMETHODIMP CScdReports::Report(VARIANT WhichOne, IScdReport ** pItem)
  {
  appSCD_COMENTRY(eScdReportsCodes)
    {
    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;

        //CDocComInfo DCI;
        //DCI.m_DoWhat=CDocComInfo::DCI_GetCount;
        //if (MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
        //  {
        //  return Scd.ReturnH(E_FAIL);
        //  }
        //long N=DCI.m_lCount;
        //if (i>0 && i<=N)
        //  {
        //  DCI.m_DoWhat=CDocComInfo::DCI_GetTitle;
        //  DCI.m_lIndex=i;
        //  if (MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
        //    {
        //    return Scd.ReturnH(E_FAIL);
        //    }
        //  CComPtr<IScdGraphic> pGrf;
        //  SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
        //  SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
        //  SCD_ROF(pGrf.CopyTo(pItem));
        //  return S_OK;
        //  }
        //return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        return Scd.ReturnH(E_NOTIMPL);
        }
      case VT_BSTR:
        {
        //SCD_LclStr(Title, WhichOne.bstrVal);
        //
        //CDocComInfo DCI;
        //DCI.m_DoWhat=CDocComInfo::DCI_FindTitle;
        //DCI.m_sTitle=Title;
        //if (MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
        //  {
        //  return Scd.ReturnH(E_FAIL);
        //  }
        //long N=DCI.m_lCount;
        //if (DCI.m_lIndex>=0)
        //  {
        //  CComPtr<IScdGraphic> pGrf;
        //  SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
        //  SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
        //  SCD_ROF(pGrf.CopyTo(pItem));
        //  return S_OK;
        //  }
        //return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        return Scd.ReturnH(E_NOTIMPL);
        }
      default:
        return Scd.ReturnH(E_INVALIDARG);
      }
    *pItem=NULL;
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdReports::get_Count(LONG * pVal)
  {
  appSCD_COMENTRYGET(eScdReportsCodes, pVal)
    {
    *pVal=0;
    return Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdReports::Add(BSTR bsIDStr, IScdReport ** pItem)
  {
  appSCD_COMENTRY(eScdReportsCodes)
    {
    //SCD_LclStr(Name, bsName);
    //
    //CDocComInfo DCI;
    //DCI.m_DoWhat=CDocComInfo::DCI_Add;
    //DCI.m_sTitle=Name;
    //if (MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)==0)
    //  {
    //  CComPtr<IScdGraphic> pGrf;
    //  SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
    //  SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
    //  SCD_ROF(pGrf.CopyTo(pItem));
    //  return S_OK;
    //  }
    return Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdReports::Remove(VARIANT WhichOne)
  {
  appSCD_COMENTRY(eScdReportsCodes)
    {
    Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdReports::get__NewEnum(IUnknown * * pVal)
  {
  appSCD_COMENTRYGET(eScdReportsCodes, pVal)
    {
    //CDocComInfo DCI;
    //DCI.m_DoWhat=CDocComInfo::DCI_GetCount;
    //if (MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
    //  {
    //  return Scd.ReturnH(E_FAIL);
    //  }
    //long N=DCI.m_lCount;
    //
    //VARIANT* pVar = new VARIANT[N];
    //for(int i = 0; i < N; i++)
    //  {
    //  DCI.m_DoWhat=CDocComInfo::DCI_GetTitle;
    //  DCI.m_lIndex=i;
    //  if (MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_DocCmd, (long)&DCI)!=0)
    //    {
    //    return Scd.ReturnH(E_FAIL);
    //    }
    //  CComPtr<IScdGraphic> pGrf;
    //  SCD_ROF(pGrf.CoCreateInstance(CLSID_ScdGraphic));
    //  SCD_ROF(pGrf->put_Name(_bstr_t(DCI.m_sTitle)));
    //  pVar[i].vt = VT_DISPATCH;
    //  pVar[i].pdispVal = pGrf;
    //  pVar[i].pdispVal->AddRef();
    //  }
    //// Now make the enum.
    //typedef CComObject< CComEnum< IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy< VARIANT > > > enumVar;
    //enumVar* pEnumVar = new enumVar;
    //pEnumVar->Init(&pVar[0], &pVar[N], NULL, AtlFlagCopy);
    //delete[] pVar;
    //// return the enum.
    //return pEnumVar->QueryInterface(IID_IUnknown, (void**)pVal);
    return Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }

#define SCD_INITSYNC(cmd)                                   \
  long TheCmd=cmd;                                        \
  InitCOCmdStuff(TheCmd);

#define SCD_EXECSYNC                                        \
  switch (ExecCOCmdStuff(TheCmd))                         \
  {                                                     \
    case CScdCOCmdBase::SS_CallTimeout:                   \
    Scd.ReturnH(HRESULT_ERR(2), "Async Call Timeout");  \
    break;                                              \
      case CScdCOCmdBase::SS_CallReturned:                  \
      case CScdCOCmdBase::SS_CallASync:                     \
      break;                                              \
  }

STDMETHODIMP CScdReports::Process(eScdReportTypes Type, BSTR bsFilename, BSTR bsReportname, VARIANT_BOOL SaveAndClose)
  {
  appSCD_COMENTRY(eScdReportsCodes)
    {
    short Opt=SUB_REPORT_GETVALUES;
    long Cmd=ComCmd_Report_GetValues;
    switch (Type)
      {
      case eScdRT_GetValues:
        Opt=SUB_REPORT_GETVALUES; 
        Cmd=ComCmd_Report_GetValues;
        break;
      case eScdRT_SetValues: 
        Opt=SUB_REPORT_SETVALUES; 
        Cmd=ComCmd_Report_SetValues;
        break;
      case eScdRT_Trend:   
        Opt=SUB_REPORT_TREND; 
        Cmd=ComCmd_Report_Trend;
        break;
        //Opt=SUB_REPORT_TREND
        //Opt=SUB_REPORT_SETVALUES
        //Opt=SUB_REPORT_SAVEANDCLOSE
        //Opt=SUB_REPORT_MACRO
      }

    SCD_INITSYNC(Cmd);

    SCD_LclStr(Filename, bsFilename);
    SCD_LclStr(Reportname, bsReportname);

    CScdCOCmdBlk *pC=new CScdCOCmdBlk(-1, 0);
    CXM_OleExcelReport* pXB = new CXM_OleExcelReport(pC/*NULL*/, (LPTSTR)Filename, (LPTSTR)Reportname, Opt);
    MainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);

    SCD_EXECSYNC;
    if (SaveAndClose)
      {
      SCD_INITSYNC(ComCmd_Report_SaveAndClose);
      int Opt=SUB_REPORT_SAVEANDCLOSE;
      CScdCOCmdBlk *pC=new CScdCOCmdBlk(-1, 0);
      CXM_OleExcelReport* pXB = new CXM_OleExcelReport(pC, (LPTSTR)Filename, "", Opt);
      MainWnd()->PostMessage(WMU_CMD, SUB_CMD_GENERATEOLEREPORT, (LPARAM)pXB);
      SCD_EXECSYNC;
      }
    }
  SCD_COMEXIT
  }
