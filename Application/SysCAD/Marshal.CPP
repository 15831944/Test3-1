//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
//#include "drv_dlgs.h"
//#include "drv_mngr.h"
#include "marshal.h"
#include "project.h"
#include "dlgbusy.h"
#include "dbgmngr.h"
#include "TagDB.h"
#include "dbhelper.h"
#include "scd_wm.h"
#include "..\..\..\Scada\SysCADMarshal\SysCADMarshal_i.c"
#include "..\..\..\SMDK\Include\msglog.h"
//#include "d:\temp\atlsvr\atlsvr_i.c"

//#include "optoff.h"

#define dbgMarshalMan     1

inline bool IsNumDataVT(VARTYPE VT) { return VT==VT_R4 || VT==VT_R8 || VT==VT_I1 || VT==VT_I2 || VT==VT_I4 || VT==VT_UI1 || VT==VT_UI2 || VT==VT_UI4; }
inline bool IsBooleanDataVT(VARTYPE VT) { return VT==VT_BOOL; }
inline bool IsFloatDataVT(VARTYPE VT) { return VT==VT_R4 || VT==VT_R8; }
inline bool IsSignedDataVT(VARTYPE VT) { return VT==VT_I1 || VT==VT_I2 || VT==VT_I4; }
inline bool IsUnsignedDataVT(VARTYPE VT) { return VT==VT_UI1 || VT==VT_UI2 || VT==VT_UI4; }

#if dbgMarshalMan
static CDbgMngr dbgTagSubs    ("MarshalManager", "TagSubs");
static CDbgMngr dbgGetValues  ("MarshalManager", "GetValues");
static CDbgMngr dbgPutValues  ("MarshalManager", "PutValues");
static CDbgMngr dbgChgValues  ("MarshalManager", "ChgValues");
static CDbgMngr dbgSubsQueue  ("MarshalManager", "SubsQueue");
#endif

// -----------------------------------------------------------------------

LPCSTR FltFmt(double D)
  {
  double d=fabs(D);
  if (d<1.0e10 && d>=1.0e-1) return "%.3f";
  if (d>1.0e10) return "%.6g";
  if (d>1.0e-20) return "%.g";
  return "0.0";
  }

// -----------------------------------------------------------------------

void VariantToString(VARIANT value, CString &strText, bool AsHex)
  {
  switch(value.vt)
    {
    case VT_NULL:
      strText = "NULL";
      break;
    case VT_EMPTY:
      strText = "-";
      break;
    case VT_BOOL:
      strText = value.boolVal ? _T("1") : _T("0");
      break;
    case VT_UI1:
      strText.Format(AsHex ? _T("0x%hx") : _T("%hd"), (USHORT) value.bVal);
      break;
    case VT_UI2:
      strText.Format(AsHex ? _T("0x%hx") : _T("%hd"), value.uiVal);
      break;
    case VT_UI4:
      strText.Format(AsHex ? _T("0x%hx") : _T("%hd"), value.ulVal);
      break;
    case VT_I1:
      strText.Format(AsHex ? _T("0x%hx") : _T("%hd"), value.cVal);
      break;
    case VT_I2:
      strText.Format(AsHex ? _T("0x%hx") : _T("%hd"), value.iVal);
      break;
    case VT_I4:
      strText.Format(AsHex ? _T("0x%x") : _T("%d"), value.lVal);
      break;
    case VT_R4:
      strText.Format(FltFmt(value.fltVal), value.fltVal);
      break;
    case VT_R8:
      strText.Format(FltFmt(value.dblVal), value.dblVal);
      break;
    case VT_BSTR:
      strText = value.bstrVal;
      break;
    default:  // Arrays of simple types
        {
        if((value.vt & VT_ARRAY) == VT_ARRAY)
          {
          CString   temp;
          SAFEARRAY *pArray = value.parray;
          LONG      lBound = 0, uBound = 0;
          SafeArrayGetLBound(pArray, 1, &lBound);
          SafeArrayGetUBound(pArray, 1, &uBound);
          for(long element = lBound; element <= uBound; element++)
            {
            if(!strText.IsEmpty()) strText += _T(", ");
            switch(value.vt &~VT_ARRAY)
            {
            case VT_BOOL:
                {
                VARIANT_BOOL  b = 0;
                SafeArrayGetElement(pArray, &element, &b);
                temp = b ? _T("1") : _T("0");
                }
              break;
            case VT_UI1:
                {
                BYTE  b = 0;
                SafeArrayGetElement(pArray, &element, &b);
                temp.Format(AsHex ? _T("0x%hx") : _T("%hd"), b);
                }
              break;
            case VT_UI2:
            case VT_I2:
                {
                short b = 0;
                SafeArrayGetElement(pArray, &element, &b);
                temp.Format(AsHex ? _T("0x%hx") : _T("%hd"), b);
                }
              break;
            case VT_I4:
                {
                long  b = 0;
                SafeArrayGetElement(pArray, &element, &b);
                temp.Format(AsHex ? _T("0x%x") : _T("%d"), b);
                }
              break;
            case VT_R4:
                {
                float d = 0;
                SafeArrayGetElement(pArray, &element, &d);
                temp.Format(_T("%g"), d);
                }
              break;
            case VT_R8:
                {
                double  d = 0;
                SafeArrayGetElement(pArray, &element, &d);
                temp.Format(_T("%g"), d);
                }
              break;
            case VT_BSTR:
                {
                BSTR  b;
                SafeArrayGetElement(pArray, &element, &b);
                temp = b;
                }
              break;
            }

            strText += temp;
            }
          }
        else
          strText = _T("?");
        }
    }
  }

// -----------------------------------------------------------------------

static LPCTSTR VTAsString(long vt)
  {
  // NotGood
  static CString  X;
  switch(vt & VT_TYPEMASK)
  {
  case VT_EMPTY:            X = "VT_EMPTY"; break;
  case VT_NULL:             X = "VT_NULL"; break;
  case VT_I2:               X = "VT_I2"; break;
  case VT_I4:               X = "VT_I4"; break;
  case VT_R4:               X = "VT_R4"; break;
  case VT_R8:               X = "VT_R8"; break;
  case VT_CY:               X = "VT_CY"; break;
  case VT_DATE:             X = "VT_DATE"; break;
  case VT_BSTR:             X = "VT_BSTR"; break;
  case VT_DISPATCH:         X = "VT_DISPATCH"; break;
  case VT_ERROR:            X = "VT_ERROR"; break;
  case VT_BOOL:             X = "VT_BOOL"; break;
  case VT_VARIANT:          X = "VT_VARIANT"; break;
  case VT_UNKNOWN:          X = "VT_UNKNOWN"; break;
  case VT_DECIMAL:          X = "VT_DECIMAL"; break;
  case VT_I1:               X = "VT_I1"; break;
  case VT_UI1:              X = "VT_UI1"; break;
  case VT_UI2:              X = "VT_UI2"; break;
  case VT_UI4:              X = "VT_UI4"; break;
  case VT_I8:               X = "VT_I8"; break;
  case VT_UI8:              X = "VT_UI8"; break;
  case VT_INT:              X = "VT_INT"; break;
  case VT_UINT:             X = "VT_UINT"; break;
  case VT_VOID:             X = "VT_VOID"; break;
  case VT_HRESULT:          X = "VT_HRESULT"; break;
  case VT_PTR:              X = "VT_PTR"; break;
  case VT_SAFEARRAY:        X = "VT_SAFEARRAY"; break;
  case VT_CARRAY:           X = "VT_CARRAY"; break;
  case VT_USERDEFINED:      X = "VT_USERDEFINED"; break;
  case VT_LPSTR:            X = "VT_LPSTR"; break;
  case VT_LPWSTR:           X = "VT_LPWSTR"; break;
  case VT_RECORD:           X = "VT_RECORD"; break;
  case VT_FILETIME:         X = "VT_FILETIME"; break;
  case VT_BLOB:             X = "VT_BLOB"; break;
  case VT_STREAM:           X = "VT_STREAM"; break;
  case VT_STORAGE:          X = "VT_STORAGE"; break;
  case VT_STREAMED_OBJECT:  X = "VT_STREAMED_OBJECT"; break;
  case VT_STORED_OBJECT:    X = "VT_STORED_OBJECT"; break;
  case VT_BLOB_OBJECT:      X = "VT_BLOB_OBJECT"; break;
  case VT_CF:               X = "VT_CF"; break;
  case VT_CLSID:            X = "VT_CLSID"; break;
  case VT_BSTR_BLOB:        X = "VT_BSTR_BLOB"; break;
  default:                  X = "VT_????"; break;
  }

  if(vt & VT_VECTOR) X += "VT_VECTOR";
  if(vt & VT_ARRAY) X += "VT_ARRAY";
  if(vt & VT_BYREF) X += "VT_BYREF";
  if(vt & VT_RESERVED) X += "VT_RESERVED";

  //  VT_BSTR_BLOB  = 0xfff,
  //  VT_VECTOR = 0x1000,
  //  VT_ARRAY  = 0x2000,
  //  VT_BYREF  = 0x4000,
  //  VT_RESERVED = 0x8000,
  //  VT_ILLEGAL  = 0xffff,
  //  VT_ILLEGALMASKED  = 0xfff,
  //  VT_TYPEMASK = 0xfff
  return X;
  }

//===========================================================================
//
//
//
//===========================================================================

#define ALLOCSTRING(sz,T)                        \
  {                                              \
  sz = (LPSTR)CoTaskMemAlloc(1+strlen(T)*1);    \
  if( sz == NULL )                               \
    {                                            \
    /*return E_OUTOFMEMORY; */                       \
    }                                            \
  /*wcscpy( sz, T2OLE(T)); */                    \
  strcpy( sz, T);                                \
  }

#define FREESTRING(sz)  { CoTaskMemFree(sz); sz=NULL; }                
//===========================================================================
//
//
//
//===========================================================================

CIOMarshal* gs_pIOMarshal = NULL;

//===========================================================================

CIOMarshal::CIOMarshal()
  {
  m_dwMarshalCookie=0;
  m_dwExecObjectCookie=0;
  
  m_dwMaxItemCount=0;
  m_dwOldMaxItemCount=0;
  m_dwItemCount=0;
  m_pTags=NULL;
  m_pDatas=NULL;
  m_pDWords=NULL;
  m_pOldTags=NULL;
  m_pOldDatas=NULL;
  m_pOldDWords=NULL;


  m_Items.SetSize(0,256);

  //AllocItemMemory(130);

  }

// --------------------------------------------------------------------------

CIOMarshal::~CIOMarshal()
  {
  //for (int i=0; i<m_dwMaxItemCount; i++)
  //  {
  //  FREESTRING(m_pTags[i].m_szTag);
  //  FREESTRING(m_pTags[i].m_szCnv);
  //  }
  CoTaskMemFree(m_pTags);
  CoTaskMemFree(m_pDatas);
  CoTaskMemFree(m_pDWords);
  }

// --------------------------------------------------------------------------

void CIOMarshal::Initialise()
  {
  };

// --------------------------------------------------------------------------

int CIOMarshal::Configure(LPCSTR File, LPCSTR Node)//, flag IgnoresOn, flag ForcesOn, flag ReadAll, flag NoiseOn, byte DrvStartFBKCheck)
  {
  m_FileName=File ? File : "";
  m_FileName.FnExpand();
  m_Node=Node?Node:"";
  return 0;
  };

// --------------------------------------------------------------------------

void CIOMarshal::Options(char* pGotoTag)
  {
  };

// --------------------------------------------------------------------------

void CIOMarshal::CloseOptions()
  {
  };

// --------------------------------------------------------------------------

long CIOMarshal::Open()
  {
  USES_CONVERSION;
  long Ret=DoOpenAttach();
  if (Ret==0)
    {
    LPCWSTR F=T2W(m_FileName());
    HRESULT hr=m_Marshal->Open(F, &Ret);
    if (SUCCEEDED(hr) && Ret==0)
      {
      EO_Register(pExecName_IOMarshal, EOWrite_Thread | EORead_Thread | EOExec_Thread,/*Pri*/THREAD_PRIORITY_ABOVE_NORMAL,/*Stack*/50000, /*HashSize*/ 101);
      return 0;
      }
    }
  return Ret;
  }

// --------------------------------------------------------------------------

long CIOMarshal::Close(BOOL SaveState)
  {
  long Ret=-1;
  EO_DeRegister();

  HRESULT hr=m_Marshal->Close(SaveState, &Ret);
  if (SUCCEEDED(hr) && Ret==0)
    return DoCloseDetach();
  return Ret;
  };

// --------------------------------------------------------------------------

long CIOMarshal::DoOpenAttach()
  {
  USES_CONVERSION;

  if (m_Marshal.IsOk())
    return 0;

  HRESULT hr = S_OK;
  // Create a running object from the class ID
  LPUNKNOWN pUnkn = NULL;

  DWORD ClsCtx=CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER;
  if (1)
    {
    COSERVERINFO si={0};
    MULTI_QI  qi;

    si.dwReserved1 = 0;
    si.pwszName = NULL;
    if (m_Node.Length()>0)
      si.pwszName = T2OLE(m_Node());
    si.pAuthInfo = NULL;
    si.dwReserved2 = 0;

    qi.pIID = &IID_IUnknown;//IID_IScdMarshal;
    qi.pItf = NULL;
    qi.hr = 0;
                             
    hr = CoCreateInstanceEx(CLSID_ScdMarshal, NULL, ClsCtx, &si, 1, &qi);
//    hr = CoCreateInstanceEx(CLSID_CXYZXYZ, NULL, ClsCtx, &si, 1, &qi);
    if (FAILED(hr))
      {
      LogError("Marshal", 0, "Error connecting to SysCADMarshal %08x", hr);
      return 1;
      }
    if (FAILED(qi.hr))
      {
      LogError("Marshal", 0, "Error connecting to SysCADMarshal %08x", qi.hr);
      return 2;
      }
    pUnkn = qi.pItf;
    }

  hr=m_Marshal.Attach(pUnkn );
  if( FAILED(hr) )
    {
    LogError("Marshal", 0, "Attaching to IScdMarshal %08x", hr);
    return 3;
    }
  hr=m_ExecObject.Attach(pUnkn );
  if( FAILED(hr) )
    {
    LogError("Marshal", 0, "Attaching to IScdExecObject %08x", hr);
    return 4;
    }

  pUnkn->Release();  // Don't need this anymore.
  pUnkn = NULL;

  // IScdExecObject callback  
  if (1)
    {
    IConnectionPointContainer *pCPC = 0;
    hr = m_ExecObject.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if( FAILED(hr) )
      {
      LogError("Marshal", 0, _T("ExecObject.QueryInterface IID_IConnectionPointContainer %08x"), hr);
      return 8;
      }
    IConnectionPoint *pCallbackCP = 0;
    hr = pCPC->FindConnectionPoint(IID__IScdExecObjectEvents, &pCallbackCP);
    pCPC->Release();
    if( FAILED(hr) )
      {
      LogError("Marshal", 0, _T("Find ConnectionPoint IID__IScdExecObjectEvents %08x"), hr);
      return 9;
      }

    m_pExecObjectEvents = new CScdExecObjectEvents;
    m_pExecObjectEvents->AddRef();
    
    hr = pCallbackCP->Advise(m_pExecObjectEvents, &m_dwExecObjectCookie);
    pCallbackCP->Release();
    if( FAILED(hr) )
      {
      LogError("Marshal", 0, _T("Advise ExecObject Events %08x"), hr);
      return 10;
      }
    }

  if (1)
    {
    IConnectionPointContainer *pCPC = 0;
    // IScdMarshal callback
    hr = m_Marshal.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if( FAILED(hr) )
      {
      LogError("Marshal", 0, _T("Marshal.QueryInterface IID_IConnectionPointContainer %08x"), hr);
      return 5;
      }
    IConnectionPoint *pCallbackCP = 0;
    hr = pCPC->FindConnectionPoint(IID__IScdMarshalEvents, &pCallbackCP);
    pCPC->Release();
    if( FAILED(hr) )
      {
      LogError("Marshal", 0, _T("Find ConnectionPoint IID__IScdMarshalEvents %08x"), hr);
      return 6;
      }

    m_pMarshalEvents = new CScdMarshalEvents;
    m_pMarshalEvents->AddRef();
    
    hr = pCallbackCP->Advise(m_pMarshalEvents, &m_dwMarshalCookie);
    pCallbackCP->Release();
    if( FAILED(hr) )
      {
      LogError("Marshal", 0, _T("Advise Marshal Events %08x"), hr);
      return 7;
      }
    }

  return 0;
  };

// --------------------------------------------------------------------------

long CIOMarshal::DoCloseDetach()
  {
  if(m_Marshal.IsOk() && m_ExecObject.IsOk())
    {
    if(m_dwExecObjectCookie)
      {
      IConnectionPointContainer *pCPC = 0;
      HRESULT hr = m_ExecObject.QueryInterface(IID_IConnectionPointContainer, (void **) &pCPC);
      if(SUCCEEDED(hr))
        {
        IConnectionPoint  *pCallbackCP = 0;
        hr = pCPC->FindConnectionPoint(IID__IScdExecObjectEvents, &pCallbackCP);
        if(SUCCEEDED(hr))
          {
          hr = pCallbackCP->Unadvise(m_dwExecObjectCookie);
          m_pExecObjectEvents->Release();
          pCallbackCP->Release();
          }
        pCPC->Release();
        }
      }

    if(m_dwMarshalCookie)
      {
      IConnectionPointContainer *pCPC = 0;
      HRESULT hr = m_Marshal.QueryInterface(IID_IConnectionPointContainer, (void **) &pCPC);
      if(SUCCEEDED(hr))
        {
        IConnectionPoint  *pCallbackCP = 0;
        hr = pCPC->FindConnectionPoint(IID__IScdMarshalEvents, &pCallbackCP);
        if(SUCCEEDED(hr))
          {
          hr = pCallbackCP->Unadvise(m_dwMarshalCookie);
          m_pMarshalEvents->Release();
          pCallbackCP->Release();
          }
        pCPC->Release();
        }
      }
    }
    
  m_Marshal.Detach();
  m_ExecObject.Detach();

  return 0;
  };

//---------------------------------------------------------------------------

void CopyTmCB(SCD_EXECUTIONCONTROL &TmCB, const CXM_TimeControl &CB)
  {
  TmCB.m_bRealTime          = CB.m_bRealTime;     
  TmCB.m_bSyncWithClock     = CB.m_bSyncWithClock;
  TmCB.m_bHoldAdv           = CB.m_bHoldAdv;      
  TmCB.m_dTime              = CB.m_TheTime.Seconds;          
  TmCB.m_dStepSize          = CB.m_StepSize.Seconds;      
  TmCB.m_dStepSizeMax       = CB.m_StepSizeMax.Seconds;   
  TmCB.m_dStepSizeNxt       = CB.m_StepSizeNxt.Seconds;   
  TmCB.m_dStopTime          = CB.m_TimeToStop.Seconds;      
  TmCB.m_dRealTimeMult      = CB.m_RealTimeMult;  
  TmCB.m_nStepCount         = CB.m_StepCount;        
  TmCB.m_nStepCountMax      = CB.m_StepCountMax;     
  
    //EqnSlvCtrlBlk    m_EqnCB;
      
  TmCB.m_ScnType            = (Scd_RunTerms)CB.m_ScnType;
  TmCB.m_TermAction         = (Scd_StopActions)CB.m_TermAction;
  TmCB.m_ScnStopTime        = CB.m_ScnStopTime.Seconds;
  TmCB.m_ScnDuration        = CB.m_ScnDuration.Seconds;
  }
  
// --------------------------------------------------------------------------

DWORD CIOMarshal::EO_Message(CXMsgLst& XM, CXM_Route& Route)
  {
  DWORD RetCode = 0;
//  while (XM.MsgAvail())
//    switch (XM.MsgId())
//      {
//      case XM_DrvShowTagInfo :
//        {
//        pXB_DrvShowTagInfo pX = XM.DrvShowTagInfo();
//        const int SlotNo = FindChanForTag(pX->cTag);
//        if (SlotNo>=0)
//          {//tag found, show driver tag info as required...
//          ASSERT(pX->iDrvOptMask!=0);
//          RetCode = 1; //valid driver tag
//          CString s;
//          if (pX->iDrvOptMask & 0x0001)
//            {//show in status bar
//            Slots[SlotNo]->GetLongDrvDesc(s);
//            pStatusBar->SetMsg1((char*)(const char*)s); //place temporary message on status bar
//            }
//          if (pX->iDrvOptMask & 0x0002)
//            {//show in message window
//            if ((pX->iDrvOptMask & 0x0001)==0)
//              Slots[SlotNo]->GetLongDrvDesc(s);
//            LogNote("Driver", 0, (char*)(const char*)s);
//            }
//          if (pX->iDrvOptMask & 0x0004)
//            {//show in driver slot dialog
//            Options(pX->cTag);
//            }
//          if (pX->iDrvOptMask & 0x0008)
//            {//paste driver tag/address in clipboard
//            Slots[SlotNo]->GetTagOrAddrDesc(s);
//            CopyTextToClipboard(AfxGetMainWnd(), (char*)(const char*)s);
//            }
//          }
//        break;
//        }
//      default:
//        ASSERT(0);
//      }
//  XM.Clear();
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
dbgpln(">>EO_QueryTime");
  USES_CONVERSION;
  SCD_EXECUTIONCONTROL TmCB;
  CopyTmCB(TmCB, CB);

  double TimeReqd=dNAN;
  double DeltaTReqd=dNAN;
  HRESULT hr=m_ExecObject->QueryTime(&TmCB, &TimeReqd, &DeltaTReqd);
  if (FAILED(hr))// && Ret==0)
    {
    CString S;
    LogError("IOMarshal", 0, "QueryTime: %s", GetHResultString(S, hr));
    return false;
    }
dbgpln("  EO_QueryTime");
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_Start(CXM_TimeControl &CB)
  {
dbgpln(">>EO_Start");
  flag OK=true;
  SCD_EXECUTIONCONTROL TmCB;
  DWORD Ret=0;
  CopyTmCB(TmCB, CB);
dbgpln(" 1EO_Start");

  HRESULT hr=m_ExecObject->Start(&TmCB, &Ret);
  if (FAILED(hr))
    {
    CString S;
    LogError("IOMarshal", 0, "Start: %s", GetHResultString(S, hr));
    return false;
    }
dbgpln(" 2EO_Start");

  switch (Ret)
    {
    case 0:
      return true;
    case 1:
      LogNote("Marshal", 0, "Some Simulator tags in ScdMarshal not found");
      return false;
    case 2:
      LogError("Marshal", 0, "Errors in ScdMarshal - cannot start");
      return false;
    case 3:
      LogWarning("Marshal", 0, "ScdMarshal Busy configuring - cannot start");
      return false;
    default:;
    }
  LogWarning("Marshal", 0, "ScdMarshal ?????????? - cannot start");
dbgpln("  EO_Start");
  return false;
  }

//---------------------------------------------------------------------------

void CIOMarshal::EO_QueryHistoryCfg(CXMsgLst& XM)
  {
  }

//---------------------------------------------------------------------------

void CIOMarshal::EO_QueryArchiveCfg(CXMsgLst& XM)
  {
  }

// -----------------------------------------------------------------------

LPCSTR ActionToString(WORD A)
  {
  switch(A)
    {
    case XIO_InOut: return "InOut";
    case XIO_In:    return "In";
    case XIO_Out:   return "Out";
    case XIO_Null:  return "Null";
    }
  return "???";
  }

//---------------------------------------------------------------------------

void CIOMarshal::AllocItemMemory(DWORD N)
  {
  if (m_dwMaxItemCount<N)
    {
    dbgpln("AllocItemMemory %4i ===================================", N);
    m_dwMaxItemCount=N;
    CoTaskMemFree(m_pTags);    
    CoTaskMemFree(m_pDatas);   
    CoTaskMemFree(m_pDWords);  
    m_pTags=(SCD_TAGITEM*)CoTaskMemAlloc(N*sizeof(SCD_TAGITEM)); 
    m_pDatas=(SCD_DATAITEM*)CoTaskMemAlloc(N*sizeof(SCD_DATAITEM));   
    m_pDWords=(DWORD*)CoTaskMemAlloc(N*sizeof(DWORD));  
    memset(m_pTags, 0, N*sizeof(SCD_TAGITEM));
    memset(m_pDatas, 0, N*sizeof(SCD_DATAITEM));
    memset(m_pDWords, 0, N*sizeof(DWORD));
    }
  }

void CIOMarshal::MarkItemMemory()
  {
  //  dbgpln("MarkItemMemory      ===================================");
  m_dwOldMaxItemCount=m_dwMaxItemCount;
  m_pOldTags=m_pTags;    
  m_pOldDatas=m_pDatas;   
  m_pOldDWords=m_pDWords;  
  }
    
void CIOMarshal::FixItemMemory()
  {
  if (m_dwMaxItemCount>m_dwOldMaxItemCount)
    {
    dbgpln("FixItemMemory   %4i > %4i =============================", m_dwOldMaxItemCount, m_dwMaxItemCount);

    if (m_pOldTags==m_pTags)
      {
      CoTaskMemFree(m_pTags);    
      m_pTags=(SCD_TAGITEM*)CoTaskMemAlloc(m_dwMaxItemCount*sizeof(SCD_TAGITEM));    
      memset(m_pTags, 0, m_dwMaxItemCount*sizeof(SCD_TAGITEM));
      dbgpln("Tags Realloc %08x > %08x", m_pOldTags, m_pTags);
      }
    else
      {
      //CoTaskMemFree(m_pOldTags);    
      dbgpln("Tags Changed %08x > %08x", m_pOldTags, m_pTags);
      }

    if (m_pOldDatas==m_pDatas)
      {
      CoTaskMemFree(m_pDatas);    
      m_pDatas=(SCD_DATAITEM*)CoTaskMemAlloc(m_dwMaxItemCount*sizeof(SCD_DATAITEM));    
      memset(m_pDatas, 0, m_dwMaxItemCount*sizeof(SCD_DATAITEM));
      dbgpln("Data Realloc %08x > %08x", m_pOldDatas, m_pDatas);
      }
    else
      {
      //CoTaskMemFree(m_pOldDatas);   
      dbgpln("Data Changed %08x > %08x", m_pOldDatas, m_pDatas);
      }
  
    if (m_pOldDWords==m_pDWords)
      {
      CoTaskMemFree(m_pDWords);    
      m_pDWords=(DWORD*)CoTaskMemAlloc(m_dwMaxItemCount*sizeof(DWORD));    
      memset(m_pDWords, 0, m_dwMaxItemCount*sizeof(DWORD));
      dbgpln("DWrd Realloc %08x > %08x", m_pOldDWords, m_pDWords);
      }
    else
      {
      //CoTaskMemFree(m_pOldDWords);  
      dbgpln("DWrd Changed %08x > %08x", m_pOldDWords, m_pDWords);
      }

    }
  else
    {
    }
  }

// -----------------------------------------------------------------------

void CIOMarshal::EO_QuerySubsReqd(CXMsgLst& XM)
  {
  USES_CONVERSION;
dbgpln(">>EO_QuerySubsReqd");

  CXM_ReadIndexedData * pMsg = (CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll = pMsg->ReadAll;
  UINT DataIndex = pMsg->Start ? 0 : pMsg->LastIndex + 1;
  XM.Clear();
  #if dbgMarshalMan
  if (dbgTagSubs())
    dbgpln("CIOMarshal::EO_QuerySubsReqd %s %i", ReadAll?"ReadAll":"More", DataIndex);
  #endif
  
  if (pMsg->Start)
    {

    // CLaer m_Items
    for (int i=0; i<m_Items.GetSize(); i++)
      delete m_Items[i];
    m_Items.SetSize(0);

    m_dwItemCount=0;
    MarkItemMemory();
    HRESULT hr=m_ExecObject->QuerySubscriptionTagsRequired(pMsg->Start, &m_dwMaxItemCount, &m_dwItemCount, &m_pTags);
    if (FAILED(hr))
      {
      CString S;
      LogError("IOMarshal", 0, "QuerySubscriptionTagsRequired: %s", GetHResultString(S, hr));
      return ;
      }
    FixItemMemory();

    m_Items.SetSize(m_dwItemCount);
    for (UINT itm=0; itm<m_dwItemCount; itm++)
      {
      SCD_TAGITEM &Tag=m_pTags[itm];
      m_Items[itm]=new CIOMarshalItem;
      CIOMarshalItem &Item=*m_Items[itm];
      Item.m_sTag         = Tag.m_szTag;
      Item.m_sCnvTxt      = Tag.m_szCnv;
      Item.m_vtType       = Tag.m_vtType;
      Item.m_iCnv         = 0;
      Item.m_iTyp         = 0;
      Item.m_bOK          = false;
      Item.m_dwItemIndex  = itm;
      Item.m_dwHandle     = Tag.m_dwHandle;
      Item.m_dwOptions    = Tag.m_dwOptions;
      Item.m_dwAction     = Tag.m_dwAction;

      LPSTR strTag = (LPSTR)(LPCSTR)Item.m_sTag;
      CXM_ObjectTag  ObjTag(strTag, TABOpt_AllInfoOnce);
      CXM_ObjectData ObjData;
      CXM_Route      Route;
      if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route)!=0)
        {
        Item.m_bOK = true;
        CPkDataItem * pPItem=ObjData.FirstItem();
        //if (pCon->m_bGet || (pPItem->Flags() & isParm)!=0)
        //  {
        if (pPItem->Contains(PDI_Value))
          Item.m_iTyp = pPItem->Type();
        else
          {
          //LogError("DrvMngr", 0, "Unknown Type for connection tag %s for %s", pCon->m_sTag(), pCon->m_pParentSlot->sTag());
          }
        if (pPItem->Contains(PDI_CnvInfo))
          {
          Item.m_iCnv = pPItem->CnvIndex();
          long i=(Item.m_iCnv);
          if (i!=0 && Cnvs[i]->Find((LPSTR)(LPCSTR)Item.m_sCnvTxt)==NULL)
            LogError("IOMarshal", 0, "Cnv %s not found for connection tag %s for %s", 
                     Item.m_sCnvTxt, Item.m_sTag, Item.m_sTag);
          }
        }
      else
        {
        LogError("IOMarshal", 0, "Tag %s not found", strTag);
        }

      #if dbgMarshalMan
      if (dbgTagSubs())
        {
        LPSTR Actions[] = {"", "In", "Out", "InOut" };
        dbgpln("  SubsReqd %4i) %4i %08x %s %4i %2i %-5s %s", 
          i, Item.m_dwItemIndex, Item.m_dwHandle, Item.m_bOK?"   ":"BAD", Item.m_iTyp, Item.m_iCnv, 
          Actions[(long)Item.m_dwAction], Item.m_sTag);
        }
      #endif

      FREESTRING(Tag.m_szTag);
      FREESTRING(Tag.m_szCnv);
      }
    }

  for (  ; DataIndex<m_dwItemCount; DataIndex++)
    {
    CIOMarshalItem &Item=*m_Items[DataIndex];
    if (1)//Item.m_bOK)
      {
      LPSTR strTag=(LPSTR)(LPCSTR)Item.m_sTag;

      ASSERT(DataIndex==Item.m_dwItemIndex);
      CXM_DataRequest *DRqst=new CXM_DataRequest (Item.m_dwItemIndex, strTag, TABOpt_AllInfoOnce, (XIOAction)Item.m_dwAction);
      if (!XM.PackMsg(DRqst))
        {
        delete DRqst;
        break;
        }
      }
    }
dbgpln("  EO_QuerySubsReqd");
  }

//---------------------------------------------------------------------------

void CIOMarshal::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst& XMRet)
  {
  USES_CONVERSION;
  for (long N=0; XM.MsgAvail(); N++)
    XM.DataRequest();
  XM.RewindReadPosition();
  AllocItemMemory(N);
  #if dbgMarshalMan
  if (dbgTagSubs())
    dbgpln("CIOMarshal::EO_QuerySubsAvail");
  #endif

  //SCD_TAGITEM  *pTags=(SCD_TAGITEM *)CoTaskMemAlloc(N*sizeof(SCD_TAGITEM));
  //DWORD m_dwMaxItemCount=N;
  //DWORD dwTagCount=N;

  m_dwItemCount=0;
  for (long i=0; XM.MsgAvail(); i++)
    {
    CXM_DataRequest* Msg = XM.DataRequest();
    SCD_TAGITEM &Tag=m_pTags[i];
    memset(&Tag, 0, sizeof(Tag));
    ALLOCSTRING(Tag.m_szTag,  Msg->Tag);
    Tag.m_dwAction=(SCD_XIOAction)Msg->Action;
    m_dwItemCount++;
    }
  
  HRESULT hr=m_ExecObject->QuerySubscriptionTagsAvailable(&m_dwMaxItemCount, &m_dwItemCount, &m_pTags);
  if (FAILED(hr))
    {
    CString S;
    LogError("IOMarshal", 0, "QuerySubscriptionTagsAvailable: %s", GetHResultString(S, hr));
    return ;
    }

  for (i=0; i<N; i++)
    {
    SCD_TAGITEM &Tag=m_pTags[i];
    if (Tag.m_dwAction!=Scd_XIO_Null)
      {
      int j=m_Items.GetSize();
      m_Items.SetSize(j+1);
      m_Items[j]=new CIOMarshalItem;
      CIOMarshalItem &Item=*m_Items[j];
      Item.m_sTag         = Tag.m_szTag;
      Item.m_sCnvTxt      = Tag.m_szCnv;
      Item.m_iCnv         = 0;
      Item.m_vtType       = Tag.m_vtType;
      switch(Item.m_vtType)
        {
        case VT_BOOL: Item.m_iTyp = tt_Bool;    break;

        case VT_I1:   Item.m_iTyp = tt_Char;    break;
        case VT_I2:   Item.m_iTyp = tt_Short;   break;
        case VT_I4:   Item.m_iTyp = tt_Long;    break;
        case VT_UI1:  Item.m_iTyp = tt_Byte;    break;
        case VT_UI2:  Item.m_iTyp = tt_Word;    break;
        case VT_UI4:  Item.m_iTyp = tt_DWord;   break;
                      
        case VT_R4:   Item.m_iTyp = tt_Float;   break;
        case VT_R8:   Item.m_iTyp = tt_Double;  break;
        }
      Item.m_bOK          = false;
      Item.m_dwItemIndex  = i;
      Item.m_dwHandle     = Tag.m_dwHandle;
      Item.m_dwOptions    = Tag.m_dwOptions;
      Item.m_dwAction     = Tag.m_dwAction;
      #if dbgMarshalMan
      if (dbgTagSubs())
        {
        dbgpln("  SubsAvl  %4i) %4i %08x %s %-5s %4i %2i %s", 
          j, Item.m_dwItemIndex, Item.m_dwHandle, Item.m_bOK?"   ":"BAD", 
          ActionToString(Item.m_dwAction), Item.m_iTyp, Item.m_iCnv, Item.m_sTag);
        }
      #endif
      
      CXM_DataAvailable *DA=new CXM_DataAvailable (Tag.m_dwItemIndex, Tag.m_szTag, (XIOAction)Tag.m_dwAction);
      XMRet.PackMsg(DA);
      }
    FREESTRING(Tag.m_szTag);
    FREESTRING(Tag.m_szCnv);
    }
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_ReadSubsData(CXMsgLst& XM)
  {
  flag DataRead = 0;
  CXM_ReadIndexedData *pMsg = XM.ReadIndexedData();
  flag ReadAll = pMsg->ReadAll;
  flag Start = pMsg->Start;
  UINT DataIndex = pMsg->Start ? 0 : pMsg->LastIndex + 1;
  XM.Clear();
  #if dbgMarshalMan
  if (dbgTagSubs())
    dbgpln("CIOMarshal::EO_ReadSubsData %s %i", ReadAll?"ReadAll":"More", DataIndex);
  #endif
              
  if (Start)
    {
    m_dwItemCount=0;
    MarkItemMemory();
    HRESULT hr=m_ExecObject->ReadSubscriptionData(true, ReadAll, &m_dwMaxItemCount, &m_dwItemCount, &m_pDatas);
    if (FAILED(hr))
      {
      CString S;
      LogError("IOMarshal", 0, "ReadSubscriptionData: %s", GetHResultString(S, hr));
      return false;
      }
    FixItemMemory();
    }
    
  for (  ; DataIndex<m_dwItemCount; DataIndex++)
    {
    SCD_DATAITEM &DataItem=m_pDatas[DataIndex];
    VARIANT & V=DataItem.m_vValue;
    #if dbgMarshalMan
    if (dbgTagSubs())
      {
      CString S;
      VariantToString(V, S, false);
      dbgpln("  Read  %4i) %4i %08x %-10s %s", DataIndex, 
        DataItem.m_dwItemIndex, DataItem.m_dwHandle, VTAsString(V.vt), S);
      }
    #endif
    CXM_ObjectData *DataObj=new CXM_ObjectData (DataItem.m_dwItemIndex);
    //CPkDataItem * pPkItem = DataObj->List.FirstItem();
    PkDataUnion PData;
    //CPkDataItem * pPrevItem = pPkItem ;
    CIOMarshalItem & MItem=*m_Items[DataItem.m_dwItemIndex];
    if (MItem.m_bOK)
      {
      if (IsData(MItem.m_iTyp))
        {
        if (IsFloatData(MItem.m_iTyp))
          {
          HRESULT hr=VariantChangeType(&V, &V, 0, VT_R8);
          if (FAILED(hr))
            {
            CString S;
            LogError("IOMarshal", 0, "ReadSubsData %s: VariantChg: %s", MItem.m_sTag, GetHResultString(S, hr));
            }
          else
            {
            if (MItem.m_iCnv)
              PData.SetTypeDouble(MItem.m_iTyp, V.dblVal, MItem.m_iCnv, (LPSTR)(LPCSTR)MItem.m_sCnvTxt);
            else
              PData.SetTypeDouble(MItem.m_iTyp, V.dblVal);
            }
          }
        else
          {
          HRESULT hr=VariantChangeType(&V, &V, 0, VT_I4);
          if (FAILED(hr))
            {
            CString S;
            LogError("IOMarshal", 0, "ReadSubsData %s: VariantChg: %s", MItem.m_sTag, GetHResultString(S, hr));
            }
          else
            PData.SetTypeLong(MItem.m_iTyp, V.lVal, MItem.m_iCnv, (LPSTR)(LPCSTR)MItem.m_sCnvTxt); 
          }
        DataObj->List.SetDataValue(/*pPkItem,*/ NULL, PData);
        //DataObj->SetSize();
        if (XM.PackMsg(DataObj))
          DataRead=1;
        else
          {
          delete DataObj;
          break;
          }
        }
      else
        LogError("IOMarshal", 0, "Bad IO Type %s %s", tt_TypeString(MItem.m_iTyp), MItem.m_sTag);
      }
    }
  if (DataIndex>=m_dwItemCount)
    {
    for (UINT i=0; i<m_dwItemCount; i++)
      VariantClear(&m_pDatas[i].m_vValue);
    }

  return DataRead;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_WriteSubsData(CXMsgLst& XM, flag FirstBlock, flag LastBlock)
  {
  for (long N=0; XM.MsgAvail(); N++)
    XM.ObjectData();
  XM.RewindReadPosition();
  AllocItemMemory(N);

  #if dbgMarshalMan
  if (dbgTagSubs())
    dbgpln("CIOMarshal::EO_WriteSubsData");
  #endif

  m_dwItemCount=0;
  while (XM.MsgAvail())
    {
    CXM_ObjectData *pX = XM.ObjectData();
    CPkDataItem *pPItem = pX->FirstItem();

    CIOMarshalItem & MItem =*m_Items[pX->Index];
    SCD_DATAITEM & DataItem=m_pDatas[m_dwItemCount];
    DataItem.m_dwItemIndex=pX->Index;
    DataItem.m_dwHandle=MItem.m_dwHandle;
    DataItem.m_dwOptions=0;
    VARIANT & V=DataItem.m_vValue;
    VariantInit(&V);
    if (IsNumData(MItem.m_iTyp))
      {
      if (IsFloatData(MItem.m_iTyp))
        {
        V.vt=VT_R8;
        V.dblVal=pPItem->Value()->GetDouble(MItem.m_iCnv, (LPSTR)(LPCSTR)MItem.m_sCnvTxt);
        HRESULT hr=VariantChangeType(&V, &V, 0, MItem.m_vtType);
        if (SUCCEEDED(hr))
          {
          m_dwItemCount++;
          #if dbgMarshalMan
          if (dbgTagSubs())
            {
            CString S;
            VariantToString(V, S, false);
             dbgpln("  Write %4i) %4i %08x %-10s %s", pX->Index, DataItem.m_dwItemIndex, DataItem.m_dwHandle, VTAsString(V.vt), S);
            }
          #endif
          }
        else
          {
          CString S;
          LogError("IOMarshal", 0, "WriteSubsData %s: VariantChg: %s", MItem.m_sTag, GetHResultString(S, hr));
          }
        }
      else
        {
        V.vt=VT_I4;
        V.lVal=pPItem->Value()->GetLong(MItem.m_iCnv, (LPSTR)(LPCSTR)MItem.m_sCnvTxt);
        HRESULT hr=VariantChangeType(&V, &V, 0, MItem.m_vtType);
        if (SUCCEEDED(hr))
          {
          m_dwItemCount++;
          #if dbgMarshalMan
          if (dbgTagSubs())
            {
            CString S;
            VariantToString(V, S, false);
            dbgpln("  Write %4i) %4i %08x %-10s %s", pX->Index, DataItem.m_dwItemIndex, DataItem.m_dwHandle, VTAsString(V.vt), S);
            }
          #endif
          }
        else
          {
          CString S;
          LogError("IOMarshal", 0, "WriteSubsData %s: VariantChg: %s", MItem.m_sTag, GetHResultString(S, hr));
          }
        }
      }
    else
      {
      LogError("IOMarshal", 0, "WriteSubsData %s: Cannot Handle Non numeric data yet", MItem.m_sTag);
      }

    #if dbgMarshalMan
    //if (dbgPutValues())
    //  dbgpln("IOMarshal: Put   %-20s      = %g EO_WriteSubsData", 
    //    Slots[SlotNo]->sTag(), Slots[SlotNo]->GetRawValueDble());
    #endif
    }
 
  if (m_dwItemCount)
    {
    HRESULT hr=m_ExecObject->WriteSubscriptionData(&m_dwMaxItemCount, &m_dwItemCount, &m_pDatas);
    if (FAILED(hr))
      {
      CString S;
      LogError("IOMarshal", 0, "WriteSubscriptionData: %s", GetHResultString(S, hr));
      return false;
      }
    for (UINT i=0; i<m_dwItemCount; i++)
      VariantClear(&m_pDatas[i].m_vValue);
    }

  return true;
  }

//--------------------------------------------------------------------------

DWORD CIOMarshal::EO_ReadTaggedItem(CXM_ObjectTag& ObjTag, CXM_ObjectData& ObjData, CXM_Route& Route)
  {
  long N=1;
  DWORD dwCount = N;
  SCD_TAGITEM   Tag;
  SCD_DATAITEM  Data;
  DWORD RetCode = 0;
  
  memset(&Tag, 0, sizeof(Tag));
  ALLOCSTRING(Tag.m_szTag,  ObjTag.Tag);
  memset(&Data, 0, sizeof(Data));

  HRESULT hr=m_ExecObject->ReadTaggedItems(dwCount, &Tag, &Data, &RetCode);
  if (FAILED(hr))
    {
    CString S;
    LogError("IOMarshal", 0, "EO_ReadTaggedItem: %s", GetHResultString(S, hr));
    return false;
    }

  ObjData.List.Clear();
  //CPkDataItem * pPItem = ObjData.List.FirstItem();
  if (RetCode)
    {
    PkDataUnion PData;
    if (IsBooleanDataVT(Data.m_vValue .vt))
      {
      VariantChangeType(&Data.m_vValue, NULL, 0, VT_BOOL);
      PData.SetTypeLong(tt_Bool, Data.m_vValue.boolVal);
      }
    else if (IsFloatDataVT(Data.m_vValue .vt))
      {
      VariantChangeType(&Data.m_vValue, NULL, 0, VT_R8);
      PData.SetTypeDouble(tt_Double, Data.m_vValue.dblVal);
      }
    else if (IsSignedDataVT(Data.m_vValue .vt))
      {
      VariantChangeType(&Data.m_vValue, NULL, 0, VT_I4);
      PData.SetTypeLong(tt_Long, Data.m_vValue.lVal);
      }
    else if (IsUnsignedDataVT(Data.m_vValue .vt))
      {
      VariantChangeType(&Data.m_vValue, NULL, 0, VT_UI4);
      PData.SetTypeLong(tt_DWord, Data.m_vValue.ulVal);
      }
    else
      {
      LogError("IOMarshal", 0, "EO_ReadTaggedItem: Unknown Datatype %i", Data.m_vValue.vt);
      }

    if (PData.iType!=tt_NULL)
      {
      DataUnion uMin;
      DataUnion uMax;
      if (0)//pRange)
        {
        //uMin.Set(Cnvs[m_iCnv]->Normal(pRange->RngLo, m_sCnvTxt()));
        //uMax.Set(Cnvs[m_iCnv]->Normal(pRange->RngHi, m_sCnvTxt()));
        }
      else
        {
        uMin.Set(0);//Cnvs[m_iCnv]->Normal(0.0, m_sCnvTxt()));
        uMax.Set(1);//Cnvs[m_iCnv]->Normal(1.0, m_sCnvTxt()));
        }
      //ObkjData.List.SetDataValueAll(pPItem, sTag(), "", PData, isDriverTag | ((iAction & XIO_In) ? DDEF_PARAM : 0),
      //                             uMin, uMax, m_iCnv, m_sCnvTxt(), ""/*sCnvFam()*/, NULL, False, sDescription());
      ObjData.List.SetDataValueAll(/*pPItem,*/ ObjTag.Tag, "", PData, DDEF_PARAM,
                                   uMin, uMax, 0/*m_iCnv*/, ""/*m_sCnvTxt()*/, ""/*sCnvFam()*/, NULL, False, ""/*sDescription()*/);
      //ObjData.SetSize();
      }    
    }

  FREESTRING(Tag.m_szTag);
  FREESTRING(Tag.m_szCnv);
  VariantClear(&Data.m_vValue);

  return RetCode;
  }

//--------------------------------------------------------------------------

int CIOMarshal::EO_WriteTaggedItem(CXM_ObjectData& ObjData, CXM_Route& Route)
  {
  CPkDataItem * pItem = ObjData.FirstItem();

  //  int SlotNo = FindChanForTag(pItem->SymOrTag());
//  if (SlotNo>=0 && (!bIgnoresOn || !Slots[SlotNo]->bIgnoreWrites))
//    {
//    if (bForcesOn && Slots[SlotNo]->bEnableForces)
//      {
//      double d = Slots[SlotNo]->dForceVal;
//      PkDataUnion Data(d);
//      if (Slots[SlotNo]->PutTagValue(Data) && Slots[SlotNo]->m_bReqdByExec) //send to driver...
//        AddSltChange(SlotNo, d, Slots[SlotNo]); //send to SysCAD EO objects...
//      }
//    else
//      {
//      double d = pItem->Value()->GetDouble();
//      if (Slots[SlotNo]->PutTagValue(*(pItem->Value())) && Slots[SlotNo]->m_bReqdByExec) //send to driver...
//        AddSltChange(SlotNo, d, Slots[SlotNo]); //send to SysCAD EO objects...
//      }
//    #if dbgMarshalMan
//    if (dbgPutValues())
//      dbgpln("Drvr: Put   %-20s      = %g EO_WriteTaggedItem", Slots[SlotNo]->sTag(), Slots[SlotNo]->GetRawValueDble());
//    #endif
//    return TOData_OK;
//    }
  long N=1;
  DWORD dwCount = N;
  SCD_TAGITEM   Tag;
  SCD_DATAITEM  Data;
  DWORD RetCode = 0;

  memset(&Tag, 0, sizeof(Tag));
  ALLOCSTRING(Tag.m_szTag, pItem->SymOrTag());
  memset(&Data, 0, sizeof(Data));

  if (IsFloatData(pItem->Type()))
    {
    Data.m_vValue.vt=VT_R8;
    Data.m_vValue.dblVal=pItem->Value()->GetDouble();
    }
  else if (IsIntData(pItem->Type()))
    {
    Data.m_vValue.vt=VT_I4;
    Data.m_vValue.lVal=pItem->Value()->GetLong();
    }
  else if (IsUnSgnData(pItem->Type()))
    {
    Data.m_vValue.vt=VT_UI4;
    Data.m_vValue.ulVal=pItem->Value()->GetLong();
    }
  else
    return TOData_InvalidData;


  HRESULT hr=m_ExecObject->WriteTaggedItems(dwCount, &Tag, &Data, &RetCode);

  if (FAILED(hr))
    {
    CString S;
    LogError("IOMarshal", 0, "EO_ReadTaggedItem: %s", GetHResultString(S, hr));
    return TOData_CommsError;
    }

  FREESTRING(Tag.m_szTag);
  FREESTRING(Tag.m_szCnv);
  VariantClear(&Data.m_vValue);

  return RetCode != 0 ? TOData_OK : TOData_NotFound;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
dbgpln(">>EO_Execute");
  flag OK=true;
  SCD_EXECUTIONCONTROL TmCB;
  SCD_EXECRETURN EXRet;
  DWORD Ret=0;
  CopyTmCB(TmCB, CB);
  EXRet.m_Time=EORet.m_Time.Seconds;
  EXRet.m_StepSizeNxt=EORet.m_StepSizeNxt.Seconds;
  HRESULT hr=m_ExecObject->Execute(&TmCB, &EXRet, &Ret);
  if (FAILED(hr))
    {
    CString S;
    LogError("IOMarshal", 0, "Start: %s", GetHResultString(S, hr));
    return false;
    }
  EORet.m_Time=EXRet.m_Time;
  EORet.m_StepSizeNxt=EXRet.m_StepSizeNxt;

dbgpln("  EO_Execute");
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_Stop(CXM_TimeControl &CB)
  {
  dbgpln(">>EO_Stop");
  SCD_EXECUTIONCONTROL TmCB;
  DWORD Ret=0;
  CopyTmCB(TmCB, CB);
  HRESULT hr=m_ExecObject->Stop(&TmCB, &Ret);
  if (FAILED(hr))
    {
    CString S;
    LogError("IOMarshal", 0, "Start: %s", GetHResultString(S, hr));
    return false;
    }
  dbgpln("  EO_Stop");
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_TagsNotAvail(CXMsgLst& XM)
  {
//  while (XM.MsgAvail())
//    {
//    CXM_TagNotAvail * pX = XM.TagNotAvail();
//    LogError("IOMarshal", 0, "Tag not found %s", pX->Tag);
//    }
  XM.Clear();
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_BeginSave(FilingControlBlock &FCB)
  {
//  bDoneLocalLoadSave = 0;
//  if (bIsOpen)
//    {
//    for (int i=0; i<Drivers.InUse(); i++)
//      {
//      Drivers.At(i)->bDoneSave = 0;
//      Drivers.At(i)->BeginSave(FCB);
//      }
////    if (FCB.SaveAs())
////      FCB.CopyFile(sName());
//    }
//  iLoadSaveCnt = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst& XM)
  {
  return false;
//  while (1)
//    {
//    while (iLoadSaveCnt<Drivers.InUse() && !Drivers.At(iLoadSaveCnt)->HasLoadSave())
//      iLoadSaveCnt++;
//    if (iLoadSaveCnt>=Drivers.InUse())
//      {//finished save data for each driver
//      if (bDoneLocalLoadSave)
//        return false;
//      Tag = "IOMarshalData";
//      CXM_ObjectData ObjData(0);//indexed to the first Data Item which is all thats required
//      ObjData.SetSize();
//      XM.PackMsg(ObjData);
//      return true;
//      }
//    if (Drivers.At(iLoadSaveCnt)->SaveDefinedData(FCB, Tag, XM))
//      return true;
//    iLoadSaveCnt++;
//    }
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_SaveOtherData(FilingControlBlock &FCB)
  {
  return false;
//  DWORD NWrite;
//  char Buff[64]; // CNM Spares
//  if (iLoadSaveCnt<Drivers.InUse())
//    {
//    pCDriver pDrv = Drivers.At(iLoadSaveCnt);
//    if (!pDrv->bDoneSave)
//      {
//      byte Len=pDrv->sTag.Len();
//      ASSERT_ALWAYS(Len<' ', "Bad Driver Tag Len");
//      //ASSERT(pDrv->sTag.Len()<9);
//      strcpy(Buff, pDrv->sTag());
//      Buff[Len]=0;
//      FCB.WriteFile(&Len, sizeof(Len), &NWrite);
//      FCB.WriteFile((LPVOID)Buff, Len, &NWrite);
//      pDrv->bDoneSave = 1;
//      }
//    return pDrv->SaveOtherData(FCB);
//    }
//  if (bDoneLocalLoadSave)
//    return false;
//  bDoneLocalLoadSave = 1;
//  SaveTags(FCB, true); // CNM false);
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_BeginLoad(FilingControlBlock &FCB)
  {
//  bDoneLocalLoadSave = 0;
//  if (bIsOpen)
//    {
//    for (int i=0; i<Drivers.InUse(); i++)
//      Drivers.At(i)->BeginLoad(FCB);
//    }
//  iLoadSaveCnt = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst& XM)
  {
  return false;
//  for (long i=0; i<Drivers.InUse(); i++)
//    if (Drivers.At(i)->LoadDefinedData(FCB, XM))
//      return true;
//  //finished loading each driver
//  if (bDoneLocalLoadSave)
//    return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_LoadOtherData(FilingControlBlock &FCB)
  {
  return false;
//  if (FCB.Eof())
//    return false;
//  DWORD NRead;
//  char Buff[512];
////      Buff[Len]=0;
////      FCB.WriteFile(&Len, sizeof(Len), &NWrite);
////      FCB.WriteFile((LPVOID)Buff, Len, &NWrite);
//  // Read One Byte if > ' ' then old style 9 chars
//  FCB.ReadFile((LPVOID)Buff, 1, &NRead);
//  if (Buff[0]>' ')
//    FCB.ReadFile((LPVOID)&Buff[1], 8, &NRead);
//  else
//    {
//    //Buff[0] is Len Byte
//    byte Len=Buff[0];
//    FCB.ReadFile((LPVOID)Buff, Len, &NRead);
//    Buff[Len]=0;
//    }
//  pCDriver pDrv = Drivers.Find(Buff);
//  if (pDrv)
//    {
//    if (pDrv->LoadOtherData(FCB))
//      return true;
//    }
//  else if (strncmp("TAGS....", Buff, 8)==0)
//    {
//    bDoneLocalLoadSave = 1;
//    if (!FCB.Eof())
//      LoadTags(FCB);
//    return true;
//    }
  return false;
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_EndLoad(FilingControlBlock &FCB)
  {
//  for (long i=0; i<Slots.GetSize(); i++)
//    {
//    CDrvSlot& S = *(Slots[i]);
//    if (/*S.bLocal && CNM */ S.bUseInitVal && (!bIgnoresOn || !S.bIgnoreWrites))
//      {
//      PkDataUnion Data(S.dInitVal);
//      S.PutTagValue(Data);
//      }
//    }
  return true;
  }

//---------------------------------------------------------------------------

int CIOMarshal::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {//will I allow the tag to be changed...
//  /*int SlotNo = FindChanForTag(pOldTag);
//  if (SlotNo>=0)
//    return EOCT_NOTALLOWED;*/
//  if (gs_Exec.Busy())
//    {
//    //do not allow user to change a solver tag that is refered to by the driver
//    //connect column while syscad is running
//    const int len = strlen(pOldTag);
//    const int j = ConnSubsItems.GetSize();
//    for (int i=0; i<j; i++)
//      {
//      if (_strnicmp(pOldTag, ConnSubsItems[i]->m_pCon->m_sTag(), len)==0)
//        return EOCT_DRVNOTALLOWED;
//      }
//    }
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CIOMarshal::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {//a tag has been changed, ...
//  const int len = strlen(pOldTag);
//  const int j = ConnSubsItems.GetSize();
//  for (int i=0; i<j; i++)
//    {
//    pCConnSubsItem pSubsItem = ConnSubsItems[i];
//    if (_strnicmp(pOldTag, pSubsItem->m_pCon->m_sTag(), len)==0)
//      {
//      ASSERT(!gs_Exec.Busy());
//      LogWarning("IOMarshal", 0, "Changing tag '%s' affects a driver connection tag for '%s'", pOldTag, pSubsItem->m_pCon->m_pParentSlot->sTag());
//      }
//    }
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CIOMarshal::EO_QueryDeleteTag(pchar pDelTag)
  {//will I allow the tag to be deleted...
//  /*int SlotNo = FindChanForTag(pOldTag);
//  if (SlotNo>=0)
//    return EODT_NOTALLOWED;*/
  return EODT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CIOMarshal::EO_DeleteTag(pchar pDelTag)
  {//a tag has been deleted, ...
  return EODT_NOTFOUND;
  }

//---------------------------------------------------------------------------

void CIOMarshal::EO_OnAppActivate(BOOL bActive)
  {
  }

//---------------------------------------------------------------------------

flag CIOMarshal::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
//  if (bIsOpen)
//    {
//    switch (Rqst.RQ_Type)
//      {
//      case RQ_Files:
//        if (Rqst.bFirst)
//          {
//          iLoadSaveCnt = 0;
//          Info.sData = sPath;
//          Info.sData += sName;
//          Info.sData.FnContract();
//          Info.bForceCopy = true;
//          Info.bPreventCopy = false;
//          return true;
//          }
//
//        if (iLoadSaveCnt<Drivers.InUse())
//          {
//          Info.sData = Drivers.At(iLoadSaveCnt)->CfgFile();
//          Info.sData.FnContract();
//          Info.bForceCopy = true;
//          Info.bPreventCopy = false;
//          iLoadSaveCnt++;
//          return true;
//          }
//          //TODO ... ask individual drivers what files they use...
//          //Drivers.At(iLoadSaveCnt)->GetFilesUsed())
//          //Info.sData = ???;
//          //iLoadSaveCnt++;
//          //return true;
//        break;
//      case RQ_Tags: //return info on all tags based on request criteria...
//      case RQ_TagsConns: //return info on all tags based on request criteria...
//        if (Info.Count()<Slots.GetSize() && (Rqst.pGroup==NULL || _stricmp(Rqst.pGroup, "Driver")==0))
//          {
//          Info.sData = Slots[Info.Count()]->sTag();
//          return true;
//          }
//        break;
//      case RQ_TagCount:
//        if (Rqst.bFirst)
//          {
//          Info.dwData = Slots.GetSize();
//          Info.bDWDataValid = 1;
//          return true;
//          }
//        break;
//      }
//    }
  return false;
  }

//===========================================================================
//
//
//
//===========================================================================

HRESULT STDMETHODCALLTYPE CScdExecObjectEventsBase::ReadTaggedItems( 
        DWORD __RPC_FAR dwCount,
        SCD_TAGITEM __RPC_FAR **pTag,
        SCD_DATAITEM __RPC_FAR **pData,
        DWORD __RPC_FAR **pRetCds)
  {
dbgpln(">>CScdExecObjectEventsBase::ReadTaggedItems");
CStopWatch SW;
SW.Start();

  for (UINT i=0; i<dwCount; i++)
    {
//CStopWatch SW1;
//SW1.Start();

    CXM_ObjectTag Tg((*pTag)[i].m_szTag, TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    CXM_Route Route;

    VARIANT &V=(*pData)[i].m_vValue;
    VariantInit(&V);
    (*pRetCds)[i]=gs_Exec.XReadTaggedItem(gs_pIOMarshal, Tg, ObjData, Route);
    if ((*pRetCds)[i])
      {
      CPkDataItem* pItem = ObjData.FirstItem();
      int cType = pItem->Type();
      int CnvInx = pItem->CnvIndex();
      if ((CnvInx)>0 && (*pTag)[i].m_szCnv)
        {
        if (!gs_CnvsMngr.FindSecCnv((CnvInx), (*pTag)[i].m_szCnv))
          {
          (*pRetCds)[i]|=SCDTAG_BadCnvs;

          //m_IsValid=false;
          //m_sCnvTxt="???";
          }
        }
      //else
      //  m_sCnvTxt="";

      if (IsNumData(cType))
        {
        V.vt=VT_R8;
        V.dblVal=pItem->Value()->GetDouble(CnvInx, (*pTag)[i].m_szTag);
        ::VariantChangeType(&V, NULL, 0, (*pTag)[i].m_vtType);
        }
      //if (m_IsValid)
      //  {
      //  SYSTEMTIME ST;
      //  FILETIME FT;
      //  __time64_t t = (__time64_t)TimeCB().m_Time;
      //  struct tm *pT    = _localtime64(&t);
      //  ST.wSecond       = pT->tm_sec;
      //  ST.wMinute       = pT->tm_min;
      //  ST.wHour         = pT->tm_hour;
      //  ST.wDay          = pT->tm_mday;
      //  ST.wMonth        = pT->tm_mon;
      //  ST.wYear         = pT->tm_year+1900;
      //  ST.wMilliseconds = 0;
      //
      //  SystemTimeToFileTime(&ST, &FT);
      //
      //  CSLock wait( &pCB->m_CS );    // protect tag access
      //
      //  LoadLocal(pItem, FT);
      //  }
      }
//SW1.Stop();
//dbgpln("  %10.3fms %s", SW1.Secs()*1000, (*pTag)[i].m_szTag);
    }
SW.Stop();
dbgpln("  CScdExecObjectEventsBase::ReadTaggedItems %10.3fs", SW.Secs());
  return S_OK;
  };
//===========================================================================
//
//
//
//===========================================================================
