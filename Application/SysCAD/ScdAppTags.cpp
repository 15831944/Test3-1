// ScdAppTags.cpp : Implementation of CScdAppTags
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdAppTags.h"

#include "project.h"

/////////////////////////////////////////////////////////////////////////////
// CScdAppTags

STDMETHODIMP CScdAppTags::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] =
  {
    &IID_IScdAppTags
  };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}

void CScdAppTags::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

STDMETHODIMP CScdAppTags::Tag(VARIANT WhichOne, IScdAppTag ** pItem)
  {
  appSCD_COMENTRY(eScdAppTagsCodes)
    {
    Scd.ReturnH(E_NOTIMPL);

    //if (Index<0 || Index>=m_Msgs.GetSize())
    //  return E_INVALIDARG;
//    *pItem=m_Msgs[Index];
//    return (*pItem)->AddRef();
    *pItem=NULL;
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdAppTags::get_Count(LONG * pVal)
  {
  appSCD_COMENTRYGET(eScdAppTagsCodes, pVal)
    {
    *pVal=0;
    Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdAppTags::Add(BSTR bsTag)
  {
  appSCD_COMENTRY(eScdAppTagsCodes)
    {
//    _bstr_t bTag(bsTag);
//    LPTSTR Tag=bTag;
//    bool DataErr = (CheckTag(Tag)!=0);
//    if (DataErr)
//      return Scd.Return(eScdAppTagsCode_TagMissing, "Tag missing : '%s'", Tag);
//
//    RequestModelInfoRec MInfo;
//    DataErr = !gs_pFlwLib->RequestModelInfoByClassId(Class, MInfo);
//    if (DataErr)
//      return Scd.Return(eScdAppTagsCode_UnknownClass, "Unknown Class Type : AddTag '%s' of type '%s' failed!", Tag, Class);
//
//    //DWORD LicCat = gs_License.LicCatagories();
//    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
//    //if (DataErr || stricmp(MInfo.Group(), FlwTagGrp)!=0)
//    //  return Scd.Return(eScdAppTagsCode_BadLicense, "Class Type NOT allowed: AddTag '%s' of type '%s' failed!", Tag, Class);
//
//#pragma chFIXIT(this errcode needs to be sorted out)
//    int err = gs_pFlwLib->FE_DoInsert(Class, Tag, NULL, NULL);
//    if (err)
//      return Scd.Return(eScdAppTagsCode_Failed, "Error %i : AddTag '%s' of type '%s' failed!", err, Tag, Class);
//    //gs_LicenseCnt.CalcTags();
    Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdAppTags::Remove(VARIANT WhichOne)
  {
  appSCD_COMENTRY(eScdAppTagsCodes)
    {
    Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }
STDMETHODIMP CScdAppTags::get__NewEnum(IUnknown * * pVal)
  {
  appSCD_COMENTRYGET(eScdAppTagsCodes, pVal)
    {
    // This block of code allows VB-like iteration.
    //
    // Make a temp array of VARIANTS and fill with the
    // current CoSquiggles.

    //int size = 0;
    //for(int i = 0; i < m_Msgs.GetSize(); i++)
    //  {
    //  eScdMsg_Flags Flags;
    //  HRESULT hr=m_Msgs[i]->get_Flags(&Flags);
    //  if (m_iSelection & Flags)
    //    size++;
    //  }
    //VARIANT* pVar = new VARIANT[size];
    //
    //for(i = 0; i < size; i++)
    //  {
    //  eScdMsg_Flags Flags;
    //  HRESULT hr=m_Msgs[i]->get_Flags(&Flags);
    //  if (m_iSelection & Flags)
    //    {
    //    pVar[i].vt = VT_DISPATCH;
    //    pVar[i].pdispVal = m_Msgs[i];
    //    }
    //  }
    //
    //// Now make the enum.
    //typedef CComObject< CComEnum< IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy< VARIANT > > > enumVar;
    //enumVar* pEnumVar = new enumVar;
    //pEnumVar->Init(&pVar[0], &pVar[size], NULL, AtlFlagCopy);
    //delete[] pVar;
    //
    //// return the enum.
    //return pEnumVar->QueryInterface(IID_IUnknown, (void**)pVal);
    Scd.ReturnH(E_NOTIMPL);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdAppTags::get_TagValue(BSTR bsTheTag, VARIANT *pVal)
  {
  appSCD_COMENTRYGET(eScdAppTagsCodes, pVal)
    {
    SCD_LclStr(Tag, bsTheTag);
    CScdCmdTag T;
    T.m_pTag=Tag;
    T.m_pvtValue=pVal;
    LRESULT lRet=SendComCmd(ComCmd_GetTag, (LPARAM)&T);
    //gs_Events.Send(ComCmd_GetTag, (CScdCOCmdBlk*)this, lRet);
    return Scd.ReturnH(lRet==0 ? S_OK : E_INVALIDARG);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdAppTags::put_TagValue(BSTR bsTheTag, VARIANT newVal)
  {
  appSCD_COMENTRY(eScdAppTagsCodes)
    {
    SCD_LclStr(Tag, bsTheTag);
    #if dbgScdCmdIF
    if (dbgComCmds())
      dbgpln("### SetTag %s", Tag);
    #endif
    CScdCmdTag T;
    T.m_pTag=Tag;
    T.m_pvtValue=&newVal;
    LRESULT lRet=SendComCmd(ComCmd_SetTag, (LPARAM)&T);
    return Scd.ReturnH(lRet==0 ? S_OK : E_FAIL);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdAppTags::get_TagType(BSTR bsTheTag, eScdAppTagType *pVal)
  {
  appSCD_COMENTRYGET(eScdAppTagsCodes, pVal)
    {
    SCD_LclStr(Tag, bsTheTag);

    #if dbgScdCmdIF
    if (dbgComCmds())
      dbgpln("### TagType %s", Tag);
    #endif
    if (Tag==NULL)
      {
      *pVal = eScdATT_NullTag;
      return S_OK;
      }
    if (Tag[0]==0)
      {
      *pVal = eScdATT_ZeroTag;
      return S_OK;
      }
    if (gs_pPrj==NULL)
      {
      *pVal = eScdATT_NoPrj;
      return S_OK;
      }

    VARIANT Value;
    CScdCmdTag T;
    T.m_pTag=Tag;
    T.m_pvtValue=&Value;
    LRESULT lRet=SendComCmd(ComCmd_get_TagType, (long)&T);
    //gs_Events.Send(ComCmd_get_TagType, pC, lRet);
    if (lRet==0)
      {
      *pVal = (eScdAppTagType)Value.lVal;
      return S_OK;
      }
    *pVal = eScdATT_Error;
    return Scd.ReturnH(E_FAIL);
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdAppTags::get_TagRecording(BSTR bsTheTag, eScdTagRecState *pVal)
  {
  appSCD_COMENTRYGET(eScdAppTagsCodes, pVal)
    {
    SCD_LclStr(Tag, bsTheTag);
    CScdCmdTag T;
    T.m_pTag=Tag;
    T.m_pLong=(long*)pVal;
    LRESULT lRet=SendComCmd(ComCmd_get_TagRecording, (LPARAM)&T);
    //gs_Events.Send(ComCmd_GetTag, (CScdCOCmdBlk*)this, lRet);
    return Scd.ReturnH(lRet==0 ? S_OK : E_FAIL);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdAppTags::put_TagRecording(BSTR bsTheTag, eScdTagRecState newVal)
  {
  appSCD_COMENTRY(eScdAppTagsCodes)
    {
    SCD_LclStr(Tag, bsTheTag);
    #if dbgScdCmdIF
    if (dbgComCmds())
      dbgpln("### SetTag %s", Tag);
    #endif
    CScdCmdTag T;
    T.m_pTag=Tag;
    T.m_lLong=newVal;
    LRESULT lRet=SendComCmd(ComCmd_put_TagRecording, (LPARAM)&T);
    return Scd.ReturnH(lRet==0 ? S_OK : E_FAIL);
    }
  SCD_COMEXIT
  }
