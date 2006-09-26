// ScdMarshal.cpp : Implementation of CScdMarshal
#include "stdafx.h"
#include "SysCADMarshal.h"
#include "ScdMarshal.h"
#include "SlotMngr.h"


/////////////////////////////////////////////////////////////////////////////
// CScdMarshal

STDMETHODIMP CScdMarshal::InterfaceSupportsErrorInfo(REFIID riid)
  {
	static const IID* arr[] = 
	  {
		&IID_IScdMarshal
	  };
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	  {
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	  }
	return S_FALSE;
  }

// ------------------------------------------------------------------------

#define ALLOCSTRING(sz,T)                        \
  {                                              \
  /*sz = (LPWSTR)CoTaskMemAlloc(2+strlen(T)*2); */   \
  sz = (LPSTR)CoTaskMemAlloc(1+strlen(T)*1);    \
  if( sz == NULL )                               \
    {                                            \
    return E_OUTOFMEMORY;                        \
    }                                            \
  /*wcscpy( sz, T2OLE(T)); */                        \
  strcpy( sz, T);                                \
  }

#define FREESTRING(sz)  { CoTaskMemFree(sz); sz=NULL; }                

// ========================================================================
//
//
//
// ========================================================================

CScdMarshal::CScdMarshal()
	{
	m_pUnkMarshaler = NULL;
  m_SubsItems.SetSize(0, 256);
  m_nSubsItems=0;
	}

// ========================================================================
//
//
//
// ========================================================================

STDMETHODIMP CScdMarshal::Open(LPCWSTR FileName, long *Return)
  {
  USES_CONVERSION;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
  theApp.PostThreadMessage(WMU_OPENDOC, (WPARAM)this, (long)new CString(W2T(FileName)));

  COpenCloseWait Wait;
  // Open Done
  //gs_SlotMngr.AttachSimulator(this);
  *Return=0;
  return S_OK ;//: E_FAIL;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::Close(VARIANT_BOOL Save, long *Return)
  {
  USES_CONVERSION;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
  theApp.PostThreadMessage(WMU_CLOSEDOC, 0, 0);

  COpenCloseWait Wait;
  *Return=0;
  return S_OK ;//: E_FAIL;
  }

// ========================================================================
//
//
//
// ========================================================================

STDMETHODIMP CScdMarshal::Start(SCD_EXECUTIONCONTROL *pTmCB, DWORD *pdwReturn)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

//dbgpln("Start");

  if (gs_SlotMngr.m_bDoingFullCfg)
    {
    *pdwReturn =3;
    return S_OK;
    }

  CMsgLoopLock Lk("Start");
  gs_SlotMngr.ScdRunning=true;

  //Sleep(1000);

  long N=gs_SlotMngr.NLinks;
  if (N>0)
    {
    //DWORD dwCountMax = gs_SlotMngr.NLinks;
    DWORD dwCount    = N;
    SCD_TAGITEM *pTags=(SCD_TAGITEM*)CoTaskMemAlloc(N*sizeof(SCD_TAGITEM));
    SCD_DATAITEM *pData=(SCD_DATAITEM*)CoTaskMemAlloc(N*sizeof(SCD_DATAITEM));
    DWORD        *pRetCode=(DWORD*)CoTaskMemAlloc(N*sizeof(DWORD));

  //dbgpln("Start");
    for (int i=0; i<N; i++)
      {
      SCD_TAGITEM &Tag=pTags[i];
      CLink &Link=*gs_SlotMngr.Links[i];
      ASSERT(i==Link.m_lLink);
      Tag.m_dwItemIndex=-1;
      Tag.m_dwHandle=MakeIOMHandle(IOML_Link, 0, i, IOMSI_Value);
      Tag.m_dwOptions=0;
      Tag.m_dwHandle=0;
      if (Link.m_bGet && Link.m_bSet)
        Tag.m_dwAction=Scd_XIO_InOut;
      else if (Link.m_bGet)
        Tag.m_dwAction=Scd_XIO_In;
      else if (Link.m_bSet)
        Tag.m_dwAction=Scd_XIO_Out;
      else
        Tag.m_dwAction=Scd_XIO_Null;
      Tag.m_vtType=Link.Type();
      ALLOCSTRING(Tag.m_szTag,  Link.m_sSimTag);
      ALLOCSTRING(Tag.m_szCnv,  Link.m_sCnvTxt);
      memset(&pData[i], 0, sizeof(pData[i]));
      pRetCode[i]=0;
      }
    if (1)
      {
  //dbgpln("Start");
      HRESULT hr=Fire_ReadTaggedItems(dwCount, &pTags, &pData, &pRetCode);
  //dbgpln("Start");
  
      for (i=0; i<N; i++)
        {
        SCD_TAGITEM &Tag=pTags[i];
        SCD_DATAITEM &Data=pData[i];
        CLink &Link=*gs_SlotMngr.Links[i];

        Link.m_State.m_bSimTagValid=pRetCode[i]!=0;
        Link.m_State.m_bSimTagIsParm=(pRetCode[i]&SCDTAG_IsParm)!=0;

        if (!Link.m_State.m_bSimTagValid)
          {
          Link.SetError(SErr_MissingSimTag);
          Link.SendStateValue();
          }
    
        FREESTRING(Tag.m_szTag);
        FREESTRING(Tag.m_szCnv);
        VariantClear(&Data.m_vValue);
        }
      }
    else
      {
      for (i=0; i<N; i++)
        {
        CLink &Link=*gs_SlotMngr.Links[i];
        Link.m_State.m_bSimTagValid=true;
        Link.m_State.m_bSimTagIsParm=true;
        }
      }
    
    CoTaskMemFree((void*)pTags);
    CoTaskMemFree((void*)pData);
    CoTaskMemFree((void*)pRetCode);
    }

  *pdwReturn =0;
//dbgpln("Start-Done");
 
	return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::Execute(SCD_EXECUTIONCONTROL * pTmCB, SCD_EXECRETURN * pEORet, DWORD * pdwReturn)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  *pdwReturn =0;
  //dbgpln("Stop-Done");
 
	return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::Stop(SCD_EXECUTIONCONTROL *pTmCB, DWORD *pdwReturn)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());


  gs_SlotMngr.ScdRunning=false;
  *pdwReturn =0;

  return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::QueryTime(SCD_EXECUTIONCONTROL *pTmCB, double * pdTimeReqd, double * pdDeltaTReqd)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  CMsgLoopLock Lk("QueryTime");

	return S_OK;
  }

// ------------------------------------------------------------------------

//void __RPC_FAR * __RPC_USER midl_user_allocate(size_t cBytes)
//{
//    return((void __RPC_FAR *) malloc(cBytes));
//}

// ------------------------------------------------------------------------

long CScdMarshal::AddSubsItem(DWORD Handle, bool MustSend)
  {
  if (m_nSubsItems>=m_SubsItems.GetSize())
    m_SubsItems.SetSize(m_nSubsItems+1);
  m_SubsItems[m_nSubsItems].m_dwHandle=Handle;
  m_SubsItems[m_nSubsItems].m_bMustSend=MustSend;
  m_SubsItems[m_nSubsItems].m_lLastChange=-1;  // will force a 'Readall'
  return m_nSubsItems++;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::QuerySubscriptionTagsRequired(VARIANT_BOOL Start, DWORD *dwMaxCount, DWORD *dwCount, SCD_TAGITEM **pTags)
  {
  USES_CONVERSION;
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
//dbgpln("QuerySubscriptionTagsRequired");

  CMsgLoopLock Lk("QUerySubsTagsReqd");;
  //dbgpln("QuerySubscriptionTagsRequired --------------------");

  if (Start)
    {
    m_SubsItems.SetSize(0, 256);
    m_nSubsItems=0;
    }
//dbgpln("QuerySubscriptionTagsRequired");

	// TODO: Add your implementation code here
  UINT N=gs_SlotMngr.NLinks;
  if (*dwMaxCount<N)
    {
    CoTaskMemFree((void*)*pTags);
    *pTags=(SCD_TAGITEM*)CoTaskMemAlloc(N*sizeof(SCD_TAGITEM));
    *dwMaxCount=N;
    }
//dbgpln("QuerySubscriptionTagsRequired");

  *dwCount=N;
  for (UINT i=0; i<N; i++)
    {
    SCD_TAGITEM &Tag=(*pTags)[i];
    CLink &Link=*gs_SlotMngr.Links[i];
    ASSERT(i==Link.m_lLink);
    Tag.m_dwItemIndex=-1;
    Tag.m_dwHandle=MakeIOMHandle(IOML_Link, 0, i, IOMSI_Value);
    Tag.m_dwOptions=0;
    if (Link.m_bGet && Link.m_bSet)
      Tag.m_dwAction=Scd_XIO_InOut;
    else if (Link.m_bGet)
      Tag.m_dwAction=Scd_XIO_In;
    else if (Link.m_bSet)
      Tag.m_dwAction=Scd_XIO_Out;
    else
      Tag.m_dwAction=Scd_XIO_Null;
    Tag.m_vtType=Link.Type();
    ALLOCSTRING(Tag.m_szTag,  Link.m_sSimTag);
    ALLOCSTRING(Tag.m_szCnv,  Link.m_sCnvTxt);

    long xx=AddSubsItem(Tag.m_dwHandle, Link.m_bSet);
//dbgpln("  Reqd %4i %4i %08x %-5s %-10s %s", i, xx, Tag.m_dwHandle, ActionToString(Tag.m_dwAction), TypeToString(Tag.m_vtType), Tag.m_szTag);
    }
  
//dbgpln("QuerySubscriptionTagsRequired");

  return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::QuerySubscriptionTagsAvailable(DWORD *dwMaxCount, DWORD *dwCount, SCD_TAGITEM **pTags)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  CMsgLoopLock Lk("QuerySubsTagsAvail");
  //dbgpln("QuerySubscriptionTagsAvailable --------------------");

  long N=*dwCount;
  for (int i=0; i<N; i++)
    {
    SCD_TAGITEM &Tag=(*pTags)[i];

    DWORD dwHandle;
    if (gs_SlotMngr.FindTagHandle(Tag.m_szTag, dwHandle))
      {
      Tag.m_vtType=gs_SlotMngr.GetTypeFromHandle(dwHandle);
      Tag.m_dwHandle=dwHandle;
      Tag.m_dwItemIndex=AddSubsItem(dwHandle, (Tag.m_dwAction&Scd_XIO_Out)!=0);
//dbgpln("  Found %4i %4i %08x %-5s %-10s %s", i, Tag.m_dwItemIndex, Tag.m_dwHandle, ActionToString(Tag.m_dwAction), TypeToString(Tag.m_vtType), Tag.m_szTag);
      }
    else 
      {
      Tag.m_dwAction=Scd_XIO_Null;    
//dbgpln("  N/A   %4s %4s %8s %-10s %s", "","", "", "", Tag.m_szTag);
      }
    }

	return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::ReadSubscriptionData(VARIANT_BOOL Start, VARIANT_BOOL ReadAll, DWORD *dwMaxCount, DWORD *dwCount, SCD_DATAITEM **pItems)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  CMsgLoopLock Lk("ReadSubsData");
  //dbgpln("ReadSubscriptionData %s %s --------------------", Start?"Start":"", ReadAll?"ReadAll":"");
  
  UINT N=m_nSubsItems;//gs_SlotMngr.NLinks;
  if (*dwMaxCount<N)
    {
    CoTaskMemFree((void*)*pItems);
    *pItems=(SCD_DATAITEM*)CoTaskMemAlloc(N*sizeof(SCD_DATAITEM));
    for (UINT i=0; i<N; i++)
      VariantInit(&(*pItems)[i].m_vValue);
   *dwMaxCount=N;
    }

  *dwCount=0;
  for (UINT i=0; i<N; i++)
    {
    SCD_DATAITEM &DataItem=(*pItems)[*dwCount];
    CSubsItem & SubsItem=m_SubsItems[i];
    DWORD Index=ExtractIOMItem(SubsItem.m_dwHandle);
    switch (ExtractIOMList(SubsItem.m_dwHandle))
      {
      case IOML_Link:
        {
        CLink &Link=*gs_SlotMngr.Links[Index];
        //if (SubsItem.m_lLastChange!=Link.m_lValueChanges)
          {
          //SubsItem.m_lLastChange=Link.m_lValueChanges;
          VariantInit(&DataItem.m_vValue);
          if (gs_SlotMngr.GetValueFromHandle(SubsItem.m_dwHandle, DataItem.m_vValue, ReadAll!=0))
            {
            DataItem.m_dwItemIndex=i;
            DataItem.m_dwHandle=SubsItem.m_dwHandle;
            DataItem.m_dwOptions=0;
            (*dwCount)++;
            gs_SlotMngr.m_Stats.m_nScdLinkChgsOut++;
            }
          }
        }
        break;
      case IOML_Slot:
        {
        CSlot &Slot=*gs_SlotMngr.Slots[Index];
        //if (SubsItem.m_lLastChange!=Slot.m_lValueChanges)
          {
          //SubsItem.m_lLastChange=Slot.m_lValueChanges;
          VariantInit(&DataItem.m_vValue);
          if (gs_SlotMngr.GetValueFromHandle(SubsItem.m_dwHandle, DataItem.m_vValue, ReadAll!=0))
            {
            DataItem.m_dwItemIndex=i;
            DataItem.m_dwHandle=SubsItem.m_dwHandle;
            DataItem.m_dwOptions=0;
            (*dwCount)++;
            gs_SlotMngr.m_Stats.m_nScdSlotChgsOut++;
            }
          }
        }
        break;
      }
    }

	return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::WriteSubscriptionData(DWORD *dwMaxCount, DWORD *dwCount, SCD_DATAITEM **pItems)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  CMsgLoopLock Lk("WriteSubsData");
  //dbgpln("WriteSubscriptionData --------------------");

  long N=*dwCount;
  for (int i=0; i<N; i++)
    {
    SCD_DATAITEM &Data=(*pItems)[i];
    DWORD Index=ExtractIOMItem(Data.m_dwHandle);
    switch (ExtractIOMList(Data.m_dwHandle))
      {
      case IOML_Link:
        {
        CFullValue FV(OPC_QUALITY_GOOD);
        FV.m_vValue=Data.m_vValue;
        gs_SlotMngr.SendSimValue2Link(eCSD_Simulator, Index, 0, FV);
        //gs_SlotMngr.AppendChange(eCSD_Simulator, -1, eCSD_Link, Index, 0, FV, NULL);
        gs_SlotMngr.m_Stats.m_nScdLinkChgsIn++;
        }
        break;
      case IOML_Slot:
        {
        CFullValue FV(OPC_QUALITY_GOOD);
        FV.m_vValue=Data.m_vValue;
        gs_SlotMngr.AppendChange(eCSD_Simulator, -1, eCSD_Slot, Index, 0, FV, NULL);
        gs_SlotMngr.m_Stats.m_nScdSlotChgsIn++;
        }
        break;
      }
    }
  if (N)
    gs_SlotMngr.RequestQueueFlush();

	return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::ReadTaggedItems(DWORD dwCount, SCD_TAGITEM *pTags, SCD_DATAITEM *pData, DWORD *pReturn)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  CMsgLoopLock Lk("ReadTaggedItems");

  long N=dwCount;
  for (int i=0; i<N; i++)
    {
    SCD_TAGITEM &Tag=pTags[i];

    DWORD dwHandle;
    if (gs_SlotMngr.FindTagHandle(Tag.m_szTag, dwHandle))
      {
      pReturn[i]= (RC_RTI_Exists | RC_RTI_MarshalTag);
      VARTYPE vt=gs_SlotMngr.GetTypeFromHandle(dwHandle);
      if (IsNumDataVT(vt) || IsBooleanDataVT(vt))
        pReturn[i] |= RC_RTI_NumData;
      //else if (TAB.IsStrng())
      //  pReturn[i] |= RC_RTI_StrData;
      //if (TAB.Flags() & DDEF_PARAM)
      pReturn[i] |= RC_RTI_IsParm; // ALL AS PARAMS FOR NOW

      SCD_DATAITEM &Data=pData[i];
      Data.m_dwHandle=dwHandle;
      VariantInit(&Data.m_vValue);
      if (!gs_SlotMngr.GetValueFromHandle(dwHandle, Data.m_vValue, true))
        pReturn[i] = 0;
      }
    else
      pReturn[i] = 0;
    }

	return S_OK;
  }

// ------------------------------------------------------------------------

STDMETHODIMP CScdMarshal::WriteTaggedItems(DWORD dwCount, SCD_TAGITEM *pTags, SCD_DATAITEM *pData, DWORD *pReturn)
  {
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

  CMsgLoopLock Lk("WriteTaggedItems");

  long N=dwCount;
  for (int i=0; i<N; i++)
    {
    SCD_TAGITEM &Tag=pTags[i];

    DWORD dwHandle;
    if (gs_SlotMngr.FindTagHandle(Tag.m_szTag, dwHandle))
      {
      pReturn[i]= (RC_RTI_Exists | RC_RTI_MarshalTag);
      VARTYPE vt=gs_SlotMngr.GetTypeFromHandle(dwHandle);
      if (IsNumDataVT(vt) || IsBooleanDataVT(vt))
        pReturn[i] |= RC_RTI_NumData;
      //else if (TAB.IsStrng())
      //  pReturn[i] |= RC_RTI_StrData;
      //if (TAB.Flags() & DDEF_PARAM)
      pReturn[i] |= RC_RTI_IsParm; // ALL AS PARAMS FOR NOW

      SCD_DATAITEM &Data=pData[i];
      Data.m_dwHandle=dwHandle;
      if (!gs_SlotMngr.SetValueFromHandle(eCSD_Simulator, dwHandle, Data.m_vValue))
        pReturn[i] = 0;
      }
    else
      pReturn[i] = 0;
    }

	return S_OK;
  }

// ------------------------------------------------------------------------


// ------------------------------------------------------------------------


// ========================================================================
//
//
//
// ========================================================================


