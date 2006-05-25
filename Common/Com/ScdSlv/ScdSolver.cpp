// ScdSolver.cpp : Implementation of CScdSolver
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdSolver.h"
#include "ScdNodes.h"
#include "sfe_srvr.h"
#include ".\scdsolver.h"
#include "licbase.h"
#include "scdslvwrap.h"

//#include "sfe_clnt.h"

/////////////////////////////////////////////////////////////////////////////
// CScdSolver

HRESULT CScdSolver::FinalConstruct()
  {
	FC_TESTLICENSE(gs_License.AllowCOMSlv());
  //FC_ROF(CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p));
  //FC_ROF(m_pMsgs.CoCreateInstance(CLSID_ScdMessages));
  FC_ROF(m_pSpecieDefns.CoCreateInstance(CLSID_ScdSpecieDefns));
  FC_ROF(m_pDebug.CoCreateInstance(CLSID_ScdDebug));
  FC_ROF(m_pTest.CoCreateInstance(CLSID_ScdTest));
  FC_ROF(m_pMTags.CoCreateInstance(CLSID_ScdSlvTags));
  //FC_ROF(m_pNodes.CoCreateInstance(CLSID_ScdNodes));
  //FC_ROF(m_pUnits.CoCreateInstance(CLSID_ScdUnits));
  //FC_ROF(m_pLinks.CoCreateInstance(CLSID_ScdLinks));
  FC_ROF(m_pProbal.CoCreateInstance(CLSID_ScdProbal));
  FC_ROF(m_pDynamic.CoCreateInstance(CLSID_ScdDynamic));

  //AddChildEvents(m_pNodes.p);
  //AddChildEvents(m_pUnits.p);
  //AddChildEvents(m_pLinks.p);
  AddChildEvents(m_pProbal.p);
  AddChildEvents(m_pDynamic.p);

  return m_SlvWrap.FinalConstruct();
  }

void CScdSolver::FinalRelease()
  {
  m_pTest.Release();
  m_pDebug.Release();
  //m_pSolver.Release();
  m_pSpecieDefns.Release();
  //m_pMsgs.Release();
  m_SlvWrap.FinalRelease();
  //m_pUnkMarshaler.Release();
  m_pMTags.Release();
  //m_pNodes.Release();
  //m_pUnits.Release();
  //m_pLinks.Release();
  m_pProbal.Release();
  m_pDynamic.Release();
  }

STDMETHODIMP CScdSolver::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] =
    {
      &IID_IScdSolver,
      &IID_IScdTest,
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
      if (InlineIsEqualGUID(*arr[i],riid))
        return S_OK;
    }
  return S_FALSE;
  }

void CScdSolver::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

STDMETHODIMP CScdSolver::InitialiseCfg(BSTR CfgFileName)
  {
  dllSCD_COMENTRY(long)
    {
    if (!m_SlvWrap.InitialiseCfg(CfgFileName))
      return E_FAIL;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSolver::InitialiseDB(BSTR DBFileName, SAFEARRAY *parrSpecies)
  {
  dllSCD_COMENTRY(long)
    {
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSolver::get_Messages(/*[out, retval]*/ IScdMessages**pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    //Scd.ReturnH(m_pMsgs.CopyTo(pVal));
    IScdMessages * p;
    ::CoCreateInstance(CLSID_ScdMessages, NULL, CLSCTX_ALL, IID_IScdMessages, (void**)&p);
    //p->SetObjectInfo(eWhatNodes_All, (DWORD)NULL, eScdNodeType_All, eScdDirection_All);
    *pVal=p;
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_SpecieDefns(/*[out, retval]*/ IScdSpecieDefns**pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pSpecieDefns.CopyTo(pVal));
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Conversions(IScdConversion ** pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    IScdConversion * p;
    ::CoCreateInstance(CLSID_ScdConversions, NULL, CLSCTX_ALL, IID_IScdConversions, (void**)&p);
    //p->SetObjectInfo(eWhatNodes_All, (DWORD)NULL, eScdNodeType_All, eScdDirection_All);
    *pVal=p;
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Tags(/*[out, retval]*/ IScdSlvTags**pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pMTags.CopyTo(pVal));
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Debug(/*[out, retval]*/ IScdDebug**pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pDebug.CopyTo(pVal));
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Test(/*[out, retval]*/ IScdTest**pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pTest.CopyTo(pVal));
    }
  SCD_COMEXIT
  };

//STDMETHODIMP CScdSolver::get_Units(IScdUnits **pUnits)
//  {
//  dllSCD_COMENTRYGET(long, pUnits)
//    {
//    Scd.ReturnH(m_pUnits.CopyTo(pUnits));
//    }
//  SCD_COMEXIT
//  };
//
//STDMETHODIMP CScdSolver::get_Links(IScdLinks **pLinks)
//  {
//  dllSCD_COMENTRYGET(long, pLinks)
//    {
//    Scd.ReturnH(m_pLinks.CopyTo(pLinks));
//    }
//  SCD_COMEXIT
//  };

STDMETHODIMP CScdSolver::get_Nodes(eScdNodeTypes eType, IScdNodes **pNodes)
  {
  dllSCD_COMENTRYGET(long, pNodes)
    {
    //Scd.ReturnH(m_pNodes.CopyTo(pNodes));
    IScdNodes * p;
    ::CoCreateInstance(CLSID_ScdNodes, NULL, CLSCTX_ALL, IID_IScdNodes, (void**)&p);
    p->SetObjectInfo(eWhatNodes_All, (DWORD)NULL, eType, eScdDirection_All);
    *pNodes=p;
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Probal(IScdProbal **pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pProbal.CopyTo(pVal));
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Dynamic(IScdDynamic **pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pDynamic.CopyTo(pVal));
    }
  SCD_COMEXIT
  };


STDMETHODIMP CScdSolver::CreateSpModel(BSTR ProgID, IScdTaggedObject **pModel)
  {
  dllSCD_COMENTRYGET(long, pModel)
    {
    CLSID clsid;
    if (SUCCEEDED(CLSIDFromProgID(ProgID, &clsid)))
      {
      return Scd.ReturnH(::CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IScdTaggedObject, (void**)pModel));
      };
    return Scd.ReturnH(E_FAIL);
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::CollectEvalSequenceInfo(eScdEvalSeq_Options eReqdSeq, long * ItemCount)
  {
  dllSCD_COMENTRY(long)
    {
    if (!gs_pSfeSrvr)
      return Scd.ReturnH(E_FAIL);
      
    m_SeqOptions=eReqdSeq;
    *ItemCount=gs_pSfeSrvr->FE_GetEvalOrder( (eReqdSeq&eScdEvalSeq_Control)!=0, 
                                          (eReqdSeq&eScdEvalSeq_FullDesc)!=0, 
                                          (eReqdSeq&eScdEvalSeq_FullDesc)!=0, 
                                           TV_None, m_SeqInfo);
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::GetEvalSequenceItem(long Index, BSTR *Tag, BSTR *Class, long *IOCOunt)
  {
  dllSCD_COMENTRY(long)
    {
    if (Index<1 || Index>m_SeqInfo.GetSize())
      return Scd.ReturnH(E_INVALIDARG);
    CEvalOrderItem & Itm=m_SeqInfo[Index-1];
    
    _bstr_t s(Itm.m_sTag());
    *Tag=s.copy();

    Strng ClassId;
    flag b = gs_pSfeSrvr->RequestModelClassId(Itm.m_sTag(), ClassId);
    _bstr_t s1(ClassId());
    *Class=s1.copy();

    *IOCOunt=Itm.m_FIOs.GetSize()+Itm.m_CIOs.GetSize()+Itm.m_XIOs.GetSize();
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::GetEvalSequenceIOItem(long Index, long IOItem, eScdEvalSeqIO_Class *eClass, eScdEvalSeqIO_Flags *eFlags, long * RemoteIndex, BSTR * RemoteDesc)
  {
  dllSCD_COMENTRY(long)
    {
    if (Index<1 || Index>m_SeqInfo.GetSize())
      return Scd.ReturnH(E_INVALIDARG);
    CEvalOrderItem & Itm=m_SeqInfo[Index-1];

    IOItem--;
    if (IOItem<0)
      Scd.ReturnH(E_INVALIDARG);
    CEvalOrderIOItem *pIOItm=NULL;
    if (IOItem<Itm.m_FIOs.GetSize())
      {
      pIOItm=&Itm.m_FIOs[IOItem];
      }
    else
      {
      IOItem-=Itm.m_FIOs.GetSize();
      if (IOItem<Itm.m_CIOs.GetSize())
        {
        pIOItm=&Itm.m_CIOs[IOItem];
        }
      else
        {
        IOItem-=Itm.m_CIOs.GetSize();
        if (IOItem<Itm.m_XIOs.GetSize())
          {
          pIOItm=&Itm.m_XIOs[IOItem];
          }
        else
          return Scd.ReturnH(E_INVALIDARG);
        }
      }

    dword Flgs=0;
		if (pIOItm->m_bIn)    Flgs|= eScdEvalSeqIOFlags_In;
		if (pIOItm->m_bOut)   Flgs|= eScdEvalSeqIOFlags_Out;
		if (pIOItm->m_bTear)  Flgs|= eScdEvalSeqIOFlags_Tear;
		if (pIOItm->m_bOwner) Flgs|= eScdEvalSeqIOFlags_Owner;
    *eFlags = (eScdEvalSeqIO_Flags)Flgs;

    *eClass = (eScdEvalSeqIO_Class)pIOItm->m_dwType;

    //eScdEvalSeqIO_Flags eFlags;
    *RemoteIndex=m_SeqOptions&eScdEvalSeq_Control ? pIOItm->m_lRmtCtrlOrd : pIOItm->m_lRmtProcOrd;

    _bstr_t s(pIOItm->m_sRmtTag());
    *RemoteDesc=s.copy();
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_RunMode(eScdMode *pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    *pVal=(eScdMode)gs_Exec.GlblRunModes();
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::put_RunMode(eScdMode newVal)
  {
  dllSCD_COMENTRY(long)
    {
    gs_Exec.SetGlblRunModes(newVal, SHM_All);
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdSolver::get_Time(eScdTimeFormat Fmt, VARIANT * pVal)
  {
  dllSCD_COMENTRYGET(long, pVal)
    {
    switch (Fmt)
      {
      case eScdTimeFmt_Seconds: 
      case eScdTimeFmt_Secs1970: 
        pVal->vt=VT_R8; 
        pVal->dblVal=gs_Exec.TheTime.Seconds; 
        break;
      case eScdTimeFmt_Date1900: 
        SecsToDate1900Var(gs_Exec.TheTime/*.Seconds*/, pVal);
        break;
      default:                  
        DoBreak();
        break;
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdSolver::put_Time(eScdTimeFormat Fmt, VARIANT newVal)
  {
  dllSCD_COMENTRY(long)
    {
    VARIANT T;
    VariantInit(&T);
    switch (Fmt)
      {
      case eScdTimeFmt_Seconds: 
      case eScdTimeFmt_Secs1970: 
        {
        HRESULT hr=VariantChangeType(&T, &newVal, 0, VT_R8);
        if (SUCCEEDED(hr))
          gs_Exec.SetTheTime(T.dblVal, "ScdSolver", true);
        else
          Scd.ReturnH(hr);
        break;
        }
      case eScdTimeFmt_Date1900: 
        {
        HRESULT hr=VariantChangeType(&T, &newVal, 0, VT_DATE);
        if (SUCCEEDED(hr))
          {
          CTimeValue RqdTime;
          if (Date1900VarToSecs(T, &RqdTime))
            {
            gs_Exec.SetTheTime(RqdTime, "ScdSolver", true);
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
  SCD_COMEXIT
  }
