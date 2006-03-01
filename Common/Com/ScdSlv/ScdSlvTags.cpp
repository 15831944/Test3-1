// ScdSlvTags.cpp : Implementation of CScdTags
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdSlvTags.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSlvTags

STDMETHODIMP CScdSlvTags::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdSlvTags
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdSlvTags::Tag(VARIANT WhichOne, IScdSlvTag ** pItem)
  {
  dllSCD_COMENTRY(eScdSlvTagsCodes)
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
STDMETHODIMP CScdSlvTags::get_Count(LONG * pVal)
  {
  dllSCD_COMENTRYGET(eScdSlvTagsCodes, pVal)
    {
    *pVal=0;
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdSlvTags::Add(BSTR bsTag)
  {
  dllSCD_COMENTRY(eScdSlvTagsCodes)
    {
    //    _bstr_t bClass(bsClass);
    //    _bstr_t bTag(bsTag);
    //    LPTSTR Class=bClass;
    //    LPTSTR Tag=bTag;
    //    bool DataErr = (CheckTag(Tag)!=0);
    //    if (DataErr)
    //      return Scd.Return(eScdSlvTagsCode_TagExists, "Tag exists : AddTag '%s' of type '%s' failed!", Tag, Class);
    //
    //    RequestModelInfoRec MInfo;
    //    DataErr = !gs_pFlwLib->RequestModelInfoByClassId(Class, MInfo);
    //    if (DataErr)
    //      return Scd.Return(eScdSlvTagsCode_UnknownClass, "Unknown Class Type : AddTag '%s' of type '%s' failed!", Tag, Class);
    //
    //    //DWORD LicCat = gs_License.LicCatagories();
    //    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
    //    //if (DataErr || stricmp(MInfo.Group(), FlwTagGrp)!=0)
    //    //  return Scd.Return(eScdSlvTagsCode_BadLicense, "Class Type NOT allowed: AddTag '%s' of type '%s' failed!", Tag, Class);
    //
    //#pragma chFIXIT(this errcode needs to be sorted out)
    //    int err = gs_pFlwLib->FE_DoInsert(Class, Tag, NULL, NULL);
    //    if (err)
    //      return Scd.Return(eScdSlvTagsCode_Failed, "Error %i : AddTag '%s' of type '%s' failed!", err, Tag, Class);
    //    //gs_LicenseCnt.CalcTags();
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdSlvTags::Remove(VARIANT WhichOne)
  {
  dllSCD_COMENTRY(eScdSlvTagsCodes)
    {
    Scd.ReturnH(E_NOTIMPL);
    }		
  SCD_COMEXIT
  }
STDMETHODIMP CScdSlvTags::get__NewEnum(IUnknown * * pVal)
  {
  dllSCD_COMENTRYGET(eScdSlvTagsCodes, pVal)
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
    //	  {
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


/**
HRESULT CScdCmdIF::put_TagValue(LPCTSTR Tag, VARIANT Value)
{
#if dbgScdCmdIF  
if (dbgComCmds())
dbgpln("### SetTag %s", Tag);
#endif
CScdCmdTag T;
T.m_pTag=Tag;
T.m_pvtValue=&Value;
LRESULT lRet=::AfxGetMainWnd()->SendMessage(WMU_COMCMD, ComCmd_SetTag, (LPARAM)&T);
gs_ScdCmdIF.SendEventMsg(ComCmd_SetTag,lRet);
//  gs_pScdAppIF->SendEventMsg(ComCmd_SetTag,lRet);
return lRet==0 ? S_OK : E_FAIL;
};

HRESULT CScdCmdIF::get_TagValue(LPCTSTR Tag, VARIANT * Value)
{
#if dbgScdCmdIF  
if (dbgComCmds())
dbgpln("### GetTag %s", Tag);
#endif
CScdCmdTag T;
T.m_pTag=Tag;
T.m_pvtValue=Value;
LRESULT lRet=::AfxGetMainWnd()->SendMessage(WMU_COMCMD, ComCmd_GetTag, (LPARAM)&T);
gs_ScdCmdIF.SendEventMsg(ComCmd_GetTag,lRet);
//  gs_pScdAppIF->SendEventMsg(ComCmd_GetTag,lRet);
return lRet==0 ? S_OK : E_FAIL;
};

HRESULT CScdCmdIF::get_TagType(LPCTSTR Tag, long *pVal)
{
#if dbgScdCmdIF  
if (dbgComCmds())
dbgpln("### TagType %s", Tag);
#endif
if (Tag==NULL)
{
*pVal = -2;
return S_OK;
}
if (Tag[0]==0)
{
*pVal = -3;
return S_OK;
}
if (gs_pPrj==NULL)
{
*pVal = -4;
return S_OK;
}

VARIANT Value;
CScdCmdTag T;
T.m_pTag=Tag;
T.m_pvtValue=&Value;
LRESULT lRet=::AfxGetMainWnd()->SendMessage(WMU_COMCMD, ComCmd_get_TagType, (LPARAM)&T);
gs_ScdCmdIF.SendEventMsg(ComCmd_get_TagType,lRet);
//  gs_pScdAppIF->SendEventMsg(ComCmd_get_TagType,lRet);
if (lRet==0)
{
*pVal = Value.lVal;
return S_OK;
}
*pVal = -5;
return E_FAIL;
};
**/

STDMETHODIMP CScdSlvTags::get_TagValue(BSTR TheTag, VARIANT *pVal)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState())

    // TODO: Add your implementation code here

    return S_OK;
  }

STDMETHODIMP CScdSlvTags::put_TagValue(BSTR TheTag, VARIANT newVal)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState())

    // TODO: Add your implementation code here

    return S_OK;
  }

STDMETHODIMP CScdSlvTags::get_TagType(BSTR bsTheTag, eScdSlvTagType *pVal)
  {
  dllSCD_COMENTRYGET(eScdSlvTagsCodes, pVal)
    {
    SCD_LclStr(Tag, bsTheTag);
#if dbgScdCmdIF  
    if (dbgComCmds())
      dbgpln("### TagType %s", Tag);
#endif
    if (Tag==NULL)
      {
      *pVal = eScdSTT_NullTag;
      return S_OK;
      }
    if (Tag[0]==0)
      {
      *pVal = eScdSTT_ZeroTag;
      return S_OK;
      }
    return Scd.ReturnH(E_NOTIMPL);
    //    if (gs_pPrj==NULL)
    //      {
    //      *pVal = eScdSTT_NoPrj;
    //      return S_OK;
    //      }
    //
    //  VARIANT Value;
    //  CScdCmdTag T;
    //  T.m_pTag=Tag;
    //  T.m_pvtValue=&Value;
    //  LRESULT lRet=::AfxGetMainWnd()->SendMessage(WMU_COMCMD, ComCmd_get_TagType, (LPARAM)&T);
    //  gs_ScdCmdIF.SendEventMsg(ComCmd_get_TagType,lRet);
    ////  gs_pScdAppIF->SendEventMsg(ComCmd_get_TagType,lRet);
    //  if (lRet==0)
    //    {
    //    *pVal = Value.lVal;
    //    return S_OK;
    //    }
    //  *pVal = -5;
    //  return E_FAIL;
    //
    }		
  SCD_COMEXIT
  }

