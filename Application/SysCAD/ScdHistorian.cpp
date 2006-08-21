// ScdHistorian.cpp : Implementation of CScdHistorian
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdHistorian.h"
#include "project.h"
#include "hstmain.h"
#include ".\scdhistorian.h"

/////////////////////////////////////////////////////////////////////////////
// CScdHistorian

STDMETHODIMP CScdHistorian::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdHistorian
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdHistorian::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

STDMETHODIMP CScdHistorian::Delete(void)
  {
  appSCD_COMENTRY(long)
    {
    if (gs_HstMngr.HstOpen())
      gs_HstMngr.DoClose();
    gs_HstMngr.DoDeleteHistorian();
    gs_HstMngr.DoOpenHistorian();
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdHistorian::RecordTags(BSTR bsTagFileName)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(TagFileName, bsTagFileName);
    if (!gs_HstMngr.KeepHistoryFile((LPSTR)TagFileName))
      Scd.ReturnH(E_FAIL);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdHistorian::DeleteData(void)
  {
  appSCD_COMENTRY(long)
    {
    if (gs_HstMngr.HstOpen())
      gs_HstMngr.DoClose();
    gs_HstMngr.DoDeleteDataFiles();
    gs_HstMngr.DoOpenHistorian();
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdHistorian::ExtractData(BSTR bsFileName, eScdTimeFormat Fmt, VARIANT StartTime, DOUBLE Duration, DOUBLE Interval, eScdHistDataExtractOptions Options)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(FileName, bsFileName);
    if (0)//FileName)//!((Points3D.vt&VT_R8) && (Points3D.vt&VT_ARRAY)))
      Scd.ReturnH(DISP_E_TYPEMISMATCH);
    else
      {   
      double StartTm=0;
      VARIANT T;
      VariantInit(&T);
      switch (Fmt)
        {
        case eScdTimeFmt_Seconds: 
        case eScdTimeFmt_Secs1970: 
          {
          HRESULT hr=VariantChangeType(&T, &StartTime, 0, VT_R8);
          if (SUCCEEDED(hr))
            {
            StartTm=T.dblVal;
            gs_HstMngr.QueryToFile((LPSTR)FileName, StartTm, Duration, Interval);
            }
          else
            Scd.ReturnH(hr);
          break;
          }
        case eScdTimeFmt_Date1900: 
          {
          HRESULT hr=VariantChangeType(&T, &StartTime, 0, VT_DATE);
          if (SUCCEEDED(hr))
            {
            if (Date1900VarToSecs(T, &StartTm))
              {
              gs_HstMngr.QueryToFile((LPSTR)FileName, StartTm, Duration, Interval);
              }
            //else
            //  Scd.ReturnH(?hr);
            }
          else
            Scd.ReturnH(hr);
          break;
          }
        default: 
          DoBreak();    
          break;
        }
      VariantClear(&T);
      }
    }
  SCD_COMEXIT
  }
