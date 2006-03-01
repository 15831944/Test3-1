// SlotCommon.cpp: implementation of the CSlotCommon class.
//
////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SysCADMarshal.h"
#include "SlotCommon.h"
#include "SlotMngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// =======================================================================
//
//
//

// =======================================================================

LPCSTR SrcDstString(eConnSrcDst e)
  {
  switch(e)
    {
    case eCSD_Null:       return "Null";
    case eCSD_Slot:       return "Slot";
    case eCSD_Link:       return "Link";
    case eCSD_Device:     return "Device";
    case eCSD_Manual:     return "Manual";
    case eCSD_File:       return "File";
    case eCSD_Simulator:  return "Simulator";
    //case eCSD_Connect:    return "Connect";
    case eCSD_CdBlk:      return "CdBlk";
    default:              return "??";
    }

  return "";
  }

// -----------------------------------------------------------------------

LPCSTR TypeToString(WORD vt)
  {
  switch(vt)
    {
    case VT_NULL:   return "Null";
    case VT_EMPTY:  return "-";
    case VT_BOOL:   return "Bool";
    case VT_I1:     return "I1";
    case VT_I2:     return "I2";
    case VT_I4:     return "I4";
    case VT_UI1:    return "U1";
    case VT_UI2:    return "U2";
    case VT_UI4:    return "U4";
    case VT_R4:     return "R4";
    case VT_R8:     return "R8";
    default:        return "??"; ASSERT(FALSE);
    }

  return "";
  }

// -----------------------------------------------------------------------

LPCSTR ActionToString(WORD A)
  {
  switch(A)
    {
    case Scd_XIO_InOut: return "InOut";
    case Scd_XIO_In:    return "In";
    case Scd_XIO_Out:   return "Out";
    case Scd_XIO_Null:  return "Null";
    }
  return "???";
  }
// -----------------------------------------------------------------------
bool VariantsEqual(const VARIANT *pDst, const VARIANT *pSrc)
  {
  if(pDst == pSrc)
    {
    return true;
    }

  //
  // Variants not equal if types don't match
  //
  if(V_VT(pDst) != V_VT(pSrc))
    {
    return false;
    }

  //
  // Check type specific values
  //
  switch(V_VT(pDst))
  {
  case VT_EMPTY:
  case VT_NULL:
    return true;

  case VT_I1:
    return V_I1(pDst) == V_I1(pSrc);

  case VT_I2:
    return V_I2(pDst) == V_I2(pSrc);

  case VT_I4:
    return V_I4(pDst) == V_I4(pSrc);

  case VT_R4:
    return V_R4(pDst) == V_R4(pSrc);

  case VT_R8:
    return V_R8(pDst) == V_R8(pSrc);

  case VT_CY:
    return memcmp(&(V_CY(pDst)), &(V_CY(pSrc)), sizeof(CY)) == 0;

  case VT_DATE:
    return V_DATE(pDst) == V_DATE(pSrc);

  case VT_BSTR:
    return(::SysStringByteLen(V_BSTR(pDst)) == ::SysStringByteLen(V_BSTR(pSrc)))
  && (memcmp(V_BSTR(pDst), V_BSTR(pSrc), ::SysStringByteLen(V_BSTR(pDst))) == 0);

  case VT_DISPATCH:
    return V_DISPATCH(pDst) == V_DISPATCH(pSrc);

  case VT_ERROR:
    return V_ERROR(pDst) == V_ERROR(pSrc);

  case VT_BOOL:
    return V_BOOL(pDst) == V_BOOL(pSrc);

  case VT_UNKNOWN:
    return V_UNKNOWN(pDst) == V_UNKNOWN(pSrc);

  case VT_DECIMAL:
    return memcmp(&(V_DECIMAL(pDst)), &(V_DECIMAL(pSrc)), sizeof(DECIMAL)) == 0;

  case VT_UI1:
    return V_UI1(pDst) == V_UI1(pSrc);

  case VT_UI2:
    return V_UI2(pDst) == V_UI2(pSrc);

  case VT_UI4:
    return V_UI4(pDst) == V_UI4(pSrc);

  default:
    _com_issue_error(E_INVALIDARG);

    // fall through
  }

  return false;
  }

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

bool gs_bAsHex=false;

LPCTSTR VariantToString(VARIANT value, CString &strText, bool AsHex)
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
  return strText;
  }

//---------------------------------------------------------------------------

void StringToVariant(VARTYPE VT, CString &strText, COleVariant &V, bool AsHex)
  {
  //COleVariant &V=Value;
  double f=0;
  strText.TrimLeft();
  strText.TrimRight();
  if (strText.GetLength()>2 && strText[0]=='0' && (strText[1]=='x' || strText[1]=='X'))
    {
    ReportError("List", 0, "Incomplete - Cannot yet parse Hex Value %s", strText);
    V.ChangeType(VT_R8);
    f=V.dblVal;
    }
  else
    f = atof(strText);

  V.vt=VT;
  switch( VT)
    {
    case VT_EMPTY:
    case VT_NULL:
      V.vt = VT_R8;
      V.dblVal = 0.0;
      break;
    case VT_BOOL:
      V.boolVal = f!=0.0;
      break;
    case VT_UI1: // uchar
      V.bVal = (unsigned char)f;
      break;
    case VT_UI2: // uchar
      V.uiVal = (unsigned short)f;
      break;
    case VT_UI4: // uchar
      V.ulVal = (unsigned long)f;
      break;
    case VT_I1 :  // long
      V.cVal = (char)f;
      break;
    case VT_I2 :   // short
      V.iVal = (short)f;
      break;
    case VT_I4 :  // long
      V.lVal = (long)f;
      break;
    case VT_R4 :
      V.fltVal = (float)f;
      break;
    case VT_R8 :
      V.dblVal = f;
      break;
    default: ASSERT( FALSE );
    }
  }

// -----------------------------------------------------------------------

LPCSTR TimeStampToString(FILETIME Ft, CString &strText, FILETIME *pRef)
  {
  //FILETIME FtL;
  //FileTimeToLocalFileTime(&Ft, &FtL);
  if(Ft.dwLowDateTime || Ft.dwHighDateTime)
    {
    if(pRef)
      {
      LARGE_INTEGER LRef, LFt, LDiff;
      memcpy(&LRef, pRef, sizeof(LRef));
      memcpy(&LFt, &Ft, sizeof(Ft));

      LDiff.QuadPart = LFt.QuadPart - LRef.QuadPart;

      double  D = (double)LDiff.QuadPart;

      strText.Format("%.3f", D * 1.0e-7);
      }
    else
      {
      COleDateTime  DT(Ft);
      strText = DT.Format(VAR_TIMEVALUEONLY);
      }
    }
  else
    strText = "";
  return strText;
  }

// -----------------------------------------------------------------------
LPCSTR VTAsString(long vt)
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

// -----------------------------------------------------------------------

void ReportError(LPCSTR Tag, HRESULT hr, LPCSTR Fmt, ...)
  {
  char    Buff[2048];
  va_list argptr;
  va_start(argptr, Fmt);
  vsprintf(Buff, Fmt, argptr);
  va_end(argptr);
  if(FAILED(hr))
    {
    char  msg[2048];
    char * pStr=NULL;
    switch (hr)
      {
      case OPC_E_INVALIDHANDLE:     pStr="The value of the handle is invalid."; break;
      case OPC_E_BADTYPE:           pStr="The server cannot convert the data between the  requested data type and the canonical data type."; break;
      case OPC_E_PUBLIC:            pStr="The requested operation cannot be done on a public group."; break;
      case OPC_E_BADRIGHTS:         pStr="The Items AccessRights do not allow the operation."; break;
      case OPC_E_UNKNOWNITEMID:     pStr="The item is no longer available in the server address space"; break;
      case OPC_E_INVALIDITEMID:     pStr="The item definition doesn't conform to the server's syntax."; break;
      case OPC_E_INVALIDFILTER:     pStr="The filter string was not valid"; break;
      case OPC_E_UNKNOWNPATH:       pStr="The item's access path is not known to the server."; break;
      case OPC_E_RANGE:             pStr="The value was out of range."; break;
      case OPC_E_DUPLICATENAME:     pStr="Duplicate name not allowed."; break;
      case OPC_S_UNSUPPORTEDRATE:   pStr="The server does not support the requested data rate but will use the closest available rate."; break;
      case OPC_S_CLAMP:             pStr="A value passed to WRITE was accepted but the output was clamped."; break;
      case OPC_S_INUSE:             pStr="The operation cannot be completed because the object still has references that exist."; break;
      case OPC_E_INVALIDCONFIGFILE: pStr="The server's configuration file is an invalid format."; break;
      case OPC_E_NOTFOUND:          pStr="The server could not locate the requested object."; break;
      case OPC_E_INVALID_PID:       pStr="The server does not recognise the passed property ID."; break;
      }
    if (!pStr)
      {
      DWORD Len=FormatMessage(
        FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        msg,
        MAX_PATH,
        NULL);
      if (Len==0)
        Len=FormatMessage(
          FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
          NULL,
          GetLastError(),
          MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
          msg,
          MAX_PATH,
          NULL);
      if (Len)
        {
        pStr=msg;
        //strip cr/lf
        char *p;
        while (p=strchr(msg, 0x0a))
          *p=' ';
        while (p=strchr(msg, 0x0d))
          *p=' ';

        }
      }
    if (pStr)
      {
      strcat(Buff, ":");
      strcat(Buff, pStr);
      }
    else
      {
      strcat(Buff, ":????");
      }
    }

  FILETIME  Ts;
  CoFileTimeNow(&Ts);

  static long ErrorNumber = 0;
  CErrorItem  *pErr = new CErrorItem(Tag ? Tag : "", ++ErrorNumber, Ts, Buff);

  //dbgpln("----------------->>> %s  <<<----------------------------------------------------",Buff);
  theApp.PostThreadMessage(WMU_ADDERROR, 0, (LPARAM) pErr);

  //gs_SlotMngr.AppendErrorMessage(Buff);
  }

// -----------------------------------------------------------------------

void ReportErrorBox(HRESULT hr, LPCSTR Fmt, ...)
  {
  char    Buff[2048];
  va_list argptr;
  va_start(argptr, Fmt);
  vsprintf(Buff, Fmt, argptr);
  va_end(argptr);

  ReportError(NULL, hr, "%s", Buff);
  }

// =======================================================================
//
//
//
// =======================================================================

CDelayBlock::CDelayBlock(void)
  {
  //m_bDelayLock=false;
  m_dwTime1 = 0;
  m_dwTime2 = 0;
  m_bUseTime2 = false;
  m_bEdge = false;
  }

void CDelayBlock::Advance(DWORD DT)
  {
  m_dwTime1 = Max(DWORD(0), m_dwTime1 - DT);
  if(m_bUseTime2) m_dwTime2 = Max(DWORD(0), m_bUseTime2 - DT);
  }

// =======================================================================
//
//
//
// =======================================================================

DWORD CChangeItem::sm_dwNumber=0; 

CChangeItem::CChangeItem(void)
  {
  m_eSrc = eCSD_Null;
  m_eDst = eCSD_Null;

  //memset(&m_vValue, 0, sizeof(m_vValue));
  //m_wQuality=0;
  //CoFileTimeNow(&m_TimeStamp);
  m_lSrcInx = -1;
  m_lDstInx = -1;
  m_hServer = -1;
  m_pNext = NULL;
  m_dwNumber=sm_dwNumber++; 
  m_bRefresh = false;
  };

CChangeItem::CChangeItem(eConnSrcDst Src, long SrcInx, eConnSrcDst Dst, long DstInx, OPCHANDLE hServer, DWORD TransID, CFullValue &FullValue, bool OverrideHold, bool Refresh)
  {
  m_eSrc = Src;
  m_eDst = Dst;
  m_lSrcInx = SrcInx;
  m_lDstInx = DstInx;
  m_hServer = hServer;
  m_dwTransactionID = TransID;
  *((CFullValue*)this)= FullValue;
  m_dwNumber=sm_dwNumber++; 
  m_bOverrideHold=OverrideHold;
  m_pNext = NULL;
  m_bRefresh = Refresh;
  }

// =======================================================================
//
//
//
// =======================================================================

CErrorItem::CErrorItem(void)
  {
  m_pNext = NULL;
  };

// -----------------------------------------------------------------------
CErrorItem::CErrorItem(LPCSTR Tag, long No, FILETIME TimeStamp, LPCSTR Str) :
  m_sTag(Tag),
  m_sError(Str)
  {
  m_ftTimeStamp = TimeStamp;
  m_lNo = No;
  m_pNext = NULL;
  };

IMPLEMENT_SPARES(CChangeItem, 10000);
IMPLEMENT_SPARES(CChangeBlock, 10);
IMPLEMENT_SPARES(CErrorItem, 1000);

// =======================================================================
//
//
//
// =======================================================================

static struct { long ErrNo; LPCSTR Str;} const ErrStrs[]=
  {
    {SErr_NoError             ,""                       },
    {SErr_ASyncIOWrite        ,"ASyncIOWrite"           },
    {SErr_AddItem             ,"AddItem"                },
    {SErr_CdBlkTagMissing     ,"CdBlk Tag Missing"      },
    {SErr_CdBlkTagBadType     ,"CdBlk Tag BadType"      },
    {SErr_Clamp               ,"Clamp"                  },
    {SErr_CmpOp               ,"CmpOp"                  },
    {SErr_ConnectionFn        ,"ConnectionFn"           },
    {SErr_ConnectionOp        ,"ConnectionOp"           },
    {SErr_FilterOp            ,"FilterOp"               },
    {SErr_GetAbsFn            ,"GetAbsFn"               },
    {SErr_GetCmpFn            ,"GetCmpFn"               },
    {SErr_GetFn               ,"GetFn"                  },
    {SErr_GetProfileFn        ,"GetProfileFn"           },
    {SErr_GetRevFn            ,"GetRevFn"               },
    {SErr_GetSqrFn            ,"GetSqrFn"               },
    {SErr_GetSqrtFn           ,"GetSqrtFn"              },
    {SErr_InvertOp            ,"InvertOp"               },
    {SErr_MissingOPCTag       ,"Missing OPCTag"         },
    {SErr_Modifier            ,"Modifier"               },
    {SErr_NoiseOp             ,"NoiseOp"                },
    {SErr_OscillatorOp        ,"OscillatorOp"           },
    {SErr_ProfileOp           ,"ProfileOp"              },
    {SErr_Range               ,"Range"                  },
    {SErr_RangeOp             ,"RangeOp"                },
    {SErr_RemoveItem          ,"RemoveItem"             },
    {SErr_ScaleOp             ,"ScaleOp"                },
    {SErr_SetClientHandles    ,"SetClientHandles"       },
    {SErr_SetFn               ,"SetFn"                  },
    {SErr_SetRevFn            ,"SetRevFn"               },
    {SErr_SlotExcluded        ,"Slot Excluded"          },
    {SErr_Span                ,"Span"                   },
    {SErr_SqrOp               ,"SqrOp"                  },
    {SErr_SqrtOp              ,"SqrtOp"                 },
    {SErr_TooManyConnectFns   ,"Too Many Connect Fns"   },
    {SErr_TooManyConnects     ,"Too Many Connects"      },
    {SErr_Unknown             ,"Unknown"                },
    {SErr_NotLoaded           ,"Not Loaded"             },
    {SErr_MissingSimTag       ,"Missing Simulator Tag"  },
    {SErr_ReadSubs            ,"Reads Subs"             },
    {SErr_ProfNotDone         ,"Prof Not Done"          },
    {SErr_InvalidFilename     ,"Invalid Filename"       },
    {SErr_SetValue            ,"Set Value"              },
    {SErr_SetValueDevice      ,"Set Value Device"       },
    {SErr_ApplySpanInComing   ,"ApplySpanInComing"      },
    {SErr_ApplySpanOutGoing   ,"ApplySpanOutGoing"      },
    {SErr_ApplyRangeLink2Slot ,"ApplyRangeLink2Slot"    },
    {SErr_PotentialRecursion  ,"Potential Recursion"    },
  {0},
  };

LPCSTR SErr_String(long i)
  {
  ASSERT(i>=0 && i<sizeof(ErrStrs)/sizeof(ErrStrs[0]) && ErrStrs[i].ErrNo==i);
  return ErrStrs[i].Str;
  }
  
void CSlotValueSet::SetValueSet(LPCTSTR Values)
  {
  LPCTSTR p=Values,pc;
  while ((pc=strchr(p, ','))!=0)
    {
    int l=pc-p;
    char buff[4096];
    strncpy(buff, p, l);
    buff[l]=0;
    m_Values.Add(buff);
    p=pc+1;
    }

  if (strlen(p)>0)
    m_Values.Add(p);

  for (int i=0; i<m_Values.GetSize(); i++)
    {
    int j;
    while ((j=m_Values[i].Find(" "))>=0)
      m_Values[i].Delete(j,1);
    }
  };

void CSlotValueSet::SetValueID(LPCTSTR ValueID, COleVariant & vValue, eConnSrcDst Dst, int Index)
  {    
  int L=strlen(ValueID);
  for (int i=0; i<m_Values.GetSize(); i++)
    if (strnicmp(m_Values[i], ValueID, L)==0 && m_Values[i][L]=='=')
      break;
      
  if (i<m_Values.GetSize())
    {
    int xxx=0;
    CString T=m_Values[i].Mid(L+1);

    CFullValue FV(OPC_QUALITY_GOOD);
    COleVariant &V=FV;
    V=vValue;

    double f=0;
    T.TrimLeft();
    T.TrimRight();
    if (T.GetLength()>2 && T[0]=='0' && (T[1]=='x' || T[1]=='X'))
      {
      ReportError("List", 0, "Incomplete - Cannot yet parse Hex Value %s", T);
      V.ChangeType(VT_R8);
      f=V.dblVal;
      }
    else
      f = atof(T);

    switch( vValue.vt)
      {
      case VT_EMPTY:
      case VT_NULL:
        V.vt = VT_R8;
        V.dblVal = 0.0;
        break;
      case VT_BOOL:
        V.boolVal = f!=0.0;
        break;
      case VT_UI1: // uchar
        V.bVal = (unsigned char)f;
        break;
      case VT_UI2: // uchar
        V.uiVal = (unsigned short)f;
        break;
      case VT_UI4: // uchar
        V.ulVal = (unsigned long)f;
        break;
      case VT_I1 :  // long
        V.cVal = (char)f;
        break;
      case VT_I2 :   // short
        V.iVal = (short)f;
        break;
      case VT_I4 :  // long
        V.lVal = (long)f;
        break;
      case VT_R4 :
        V.fltVal = (float)f;
        break;
      case VT_R8 :
        V.dblVal = f;
        break;
      default: ASSERT( FALSE );
      }

    gs_SlotMngr.AppendChange(eCSD_Manual, -1, Dst, Index, gs_SlotMngr.GetTransactionID(), FV, NULL, true);
    gs_SlotMngr.RequestQueueFlush();

    }
  //X.Format(",%s=", ValueID);
  };

// =======================================================================
//
//
//
// =======================================================================

CSelection::CSelection()
  {
  m_bOn         = true;
  m_bCaseSens   = false;
  m_bAndClause  = true;
  m_bErrorsOnly = false;
  m_bHoldsOnly  = false;
  for (int s=0; s<eMaxSelectStrings; s++)
    m_pRE[s] = NULL;
  SetHow(eFilterWild);
  };

CSelection::CSelection(const CSelection & Other)
  {
  *this=Other;
  };
CSelection::~CSelection()
  {
  };

CSelection & CSelection::operator=(const CSelection & Other)
  {
  m_bOn = Other.m_bOn;
  m_bCaseSens =  Other.m_bCaseSens;
  m_bAndClause = Other.m_bAndClause;
  m_bErrorsOnly = Other.m_bErrorsOnly;
  m_bHoldsOnly = Other.m_bHoldsOnly;
  for (int s=0; s<eMaxSelectStrings; s++)
    {
    m_sStr[s] = Other.m_sStr[s];
    m_sStrL[s] = m_sStr[s].MakeLower();
    }
  SetHow(Other.m_eHow);
  return *this;
  };

void CSelection::SetHow(eFilterHow How)
  {
  m_eHow = How;

  for (int s=0; s<eMaxSelectStrings; s++)
    {
    if (m_sStr[s].GetLength()>0)
      {
      if (!m_pRE[s])
        m_pRE[s] = new IRegExpPtr("VBScript.RegExp");
      (*m_pRE[s])->Global     = _variant_t(true);//' Set do all occurs
      (*m_pRE[s])->IgnoreCase = _variant_t(!m_bCaseSens);//' Set case insensitivity.

      switch (How)
        {
        case eFilterContains:
          {
          CString S;
          S.Format(".*%s.*", m_sStr[s]);
          (*m_pRE[s])->Pattern = _bstr_t(S);
          break;
          };
        case eFilterWild:
          {
          CString S("^");
          for (int i=0; i<m_sStr[s].GetLength(); i++)
            {
            if (m_sStr[s][i]=='*')
              S+=".*";
            else if (m_sStr[s][i]=='?')
              S+=".";
            else if (m_sStr[s][i]!='^' && m_sStr[s][i]!='$')
              S+=m_sStr[s][i];
            }
          S+="$";
          (*m_pRE[s])->Pattern = _bstr_t(S);
          break;
          }
        case eFilterRegExp:
          {
          CString S("^");
          for (int i=0; i<m_sStr[s].GetLength(); i++)
            {
            if (m_sStr[s][i]!='^' && m_sStr[s][i]!='$')
              S+=m_sStr[s][i];
            }
          S+="$";
          (*m_pRE[s])->Pattern = _bstr_t(S);
          break;
          }
        default:
          (*m_pRE[s])->Pattern = _bstr_t(m_sStr[s]);
          break;
        }
      }
    else
      {
      delete m_pRE[s];
      m_pRE[s]=NULL;
      }
    }
  }

bool CSelection::InFilter(LPCTSTR TheTag, LPCTSTR SimTag, LPCTSTR OPCTag, LPCTSTR Type, bool HasError, bool HasHold)
  {
  //dbgpln("%-20s %-20s %-20s", TheTag?TheTag:"", SimTag?SimTag:"", OPCTag?OPCTag:"");
  if (!m_bOn)
    return true;
  bool Match[eMaxSelectStrings]={true,true,true,true};
  if (m_eHow==eFilterContains)
    {
    if (m_bCaseSens)
      {
      Match[eFilterTheTag]=TheTag && strstr(TheTag, m_sStr[eFilterTheTag])!=NULL;
      Match[eFilterSimTag]=SimTag && strstr(SimTag, m_sStr[eFilterSimTag])!=NULL;
      Match[eFilterOPCTag]=OPCTag && strstr(OPCTag, m_sStr[eFilterOPCTag])!=NULL;
      }
    else
      {
      Match[eFilterTheTag]=TheTag && strstr(CString(TheTag).MakeLower(), m_sStrL[eFilterTheTag])!=NULL;
      Match[eFilterSimTag]=SimTag && strstr(CString(SimTag).MakeLower(), m_sStrL[eFilterSimTag])!=NULL;
      Match[eFilterOPCTag]=OPCTag && strstr(CString(OPCTag).MakeLower(), m_sStrL[eFilterOPCTag])!=NULL;
      }
    }
  else
    {
    try
      {
      Match[eFilterTheTag]= (TheTag && m_pRE[eFilterTheTag] && ((*m_pRE[eFilterTheTag])->Test(_bstr_t(TheTag))==VARIANT_TRUE));
      Match[eFilterSimTag]= (SimTag && m_pRE[eFilterSimTag] && ((*m_pRE[eFilterSimTag])->Test(_bstr_t(SimTag))==VARIANT_TRUE));
      Match[eFilterOPCTag]= (OPCTag && m_pRE[eFilterOPCTag] && ((*m_pRE[eFilterOPCTag])->Test(_bstr_t(OPCTag))==VARIANT_TRUE));
      }
    catch(...)
      {
      ReportError("InFilter", 0, "Failure in Regular expressions");
      for (int i=0; i<eMaxSelectStrings; i++)
        Match[i]=true;
      }
    }
  bool Ret;
  if (m_bAndClause)
    {
    Ret=true;
    for (int i=0; i<eMaxSelectStrings; i++)
      Ret=Ret && (Match[i] || m_sStrL[i].GetLength()==0);
    }
  else
    {
    Ret=false;
    for (int i=0; i<eMaxSelectStrings; i++)
      Ret=Ret || (Match[i] && m_sStrL[i].GetLength()>0);
    }
  return Ret && (HasError || !m_bErrorsOnly) && (HasHold || !m_bHoldsOnly);
  }

// =======================================================================
//
//
//
// =======================================================================
