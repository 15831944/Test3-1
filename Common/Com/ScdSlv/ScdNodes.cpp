// ScdNodes.cpp : Implementation of CScdNodes
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdNodes.h"
#include "flwsolve.h"
#include "scdslvwrap.h"
#include ".\scdnodes.h"

/////////////////////////////////////////////////////////////////////////////
// CScdNodes

static int CheckTag(LPCTSTR Tag)
  {
  if (gs_pSfeSrvr)
    {
    if (gs_pSfeSrvr->FE_TestModelTagUnique((char*)Tag))
      return 0;
    else
      return -1;
    }
  return -2;
  }

STDMETHODIMP CScdNodes::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdNodes
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

STDMETHODIMP CScdNodes::Node(VARIANT WhichOne, IScdNode ** pItem)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;

    switch (WhichOne.vt)
      {
      case VT_I2:
      case VT_I4:
        {
        long N=BuildArray();
        long i=(WhichOne.vt==VT_I2) ? WhichOne.iVal : WhichOne.lVal;
        if (i>=0 && i<N)
          {
          IScdNode * p;
          ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
          p->SetObjectInfo((DWORD)m_FlwNodes[i]);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Index");
        }
      case VT_BSTR:
        {
        SCD_LclStr(Name, WhichOne.bstrVal);
        FlwNode * pNd=gs_pSfeSrvr->FE_FindNode(Name);
        if (pNd)
          {
          IScdNode * p;
          ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
          p->SetObjectInfo((DWORD)pNd);
          *pItem=p; // User must release
          return S_OK;
          }
        return Scd.ReturnH(E_INVALIDARG, "Bad Tag");
        }
      default:
        return Scd.ReturnH(E_INVALIDARG);
      }
    *pItem=NULL;
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodes::get_Count(LONG * pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_SFE_OK;
    long size=BuildArray();
    *pVal=size;
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodes::get__NewEnum(IUnknown * * pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    if (m_eWhat==eWhatNodes_RmtNodes)
      CHECK_TOPO_OK
    else
      CHECK_SFE_OK;

    long size=BuildArray();

    VARIANT* pVar = new VARIANT[size];
    for(long i = 0; i < size; i++)
      {
      pVar[i].vt = VT_DISPATCH;
      pVar[i].pdispVal = m_INodes[i];
      }

    // Now make the enum.
    typedef CComObject< CComEnum< IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy< VARIANT > > > enumVar;
    enumVar* pEnumVar = new enumVar;
    pEnumVar->Init(&pVar[0], &pVar[size], NULL, AtlFlagCopy);
    delete[] pVar;
    // return the enum.
    return pEnumVar->QueryInterface(IID_IUnknown, (void**)pVal);
    //Scd.ReturnH(E_NOTIMPL);
    }		
  SCD_COMEXIT
  }


long CScdNodes::BuildArray()
  {
  long TopoCnt=gs_pSfeSrvr->FE_GetTopologyCount();
  if (m_lTopoCount!=TopoCnt)
    {
    UINT i;
    UINT size;
    m_lTopoCount=TopoCnt;

    switch (m_eWhat)
      {
      case eWhatNodes_All:
        size=gs_pSfeSrvr->FE_GetNodes(m_eTypes, m_FlwNodes);
        break;
      case eWhatNodes_RmtNodes:
        {
        size=0;
        FlwNode * TmpLst[MaxIOList];
        DWORD Type=m_pNode->m_dwType;
        if (Type&(nc_Process|nc_MSrcSnk|nc_MXfer|nc_MBst|nc_Control|nc_Elec))
          {
          for (int i=0; i<m_pNode->m_nIOs; i++)
            {
            FlwNode * p=m_pNode->Nd_Rmt(i);
            if (p && (p->m_dwType&m_eTypes))
              {
              bool DoTest=false;
              if (p->m_dwType&(nc_MLnk))
                DoTest=(p->NoFlwIOs()==2);
              else if (p->m_dwType&(nc_CLnk))
                DoTest=(p->NoCIOs()==2);
              else if (p->m_dwType&(nc_ELnk))
                DoTest=(p->NoEIOs()==2);
              else if (p->m_dwType&(nc_ALnk))
                DoTest=(p->NoAIOs()==2);
              if (DoTest)
                {
                long IRmt=m_pNode->IOIONo_Rmt(i);
                if ((IRmt==1 && m_eDirns&eScdDirection_In) || 
                  (IRmt==0 && m_eDirns&eScdDirection_Out))
                  TmpLst[size++]=p;
                }
              }
            }
          }
        else if (Type&(nc_MLnk|nc_CLnk|nc_ELnk|nc_ALnk) && m_pNode->NoFlwIOs()==2) 
          {
          for (int i=0; i<m_pNode->m_nIOs; i++)
            {
            FlwNode * p=m_pNode->Nd_Rmt(i);
            if (p && (p->m_dwType&m_eTypes))
              {
              if ((i==0 && (m_eDirns&eScdDirection_In)) || 
                (i==1 && (m_eDirns&eScdDirection_Out)))
                TmpLst[size++]=p;
              }
            }
          }
        m_FlwNodes.SetSize(size);
        for (DWORD i=0; i<size; i++)
          m_FlwNodes[i]=TmpLst[i];
        }
      break;
      }
    for (i = 0; i<(UINT)m_INodes.GetSize(); i++)
      m_INodes[i]->Release();

    m_INodes.SetSize(size);
    for (i = 0; i < size; i++)
      {
      IScdNode * p;
      ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
      p->SetObjectInfo((DWORD)m_FlwNodes[i]);
      LPDISPATCH pDisp;
      SCD_TOF(p->QueryInterface(IID_IDispatch, (void**)&pDisp), "IDispatch on ScdNode")
        m_INodes[i]=pDisp;
      p->Release();
      }
    }
  ASSERT(m_INodes.GetSize()==m_FlwNodes.GetSize());
  return m_INodes.GetSize();
  }

STDMETHODIMP CScdNodes::SetObjectInfo(DWORD What, DWORD Node, DWORD Types, DWORD Dirn)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;

    m_eWhat=(eWhatNodes)What;
    m_pNode=(FlwNode*)Node;
    m_eDirns=(eScdDirections)Dirn;
    m_eTypes=(eScdNodeTypes)Types;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount()-1; // force array build
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodes::Select(eScdNodeTypes eType)
  {
  dllSCD_COMENTRY(long)
    {
    IScdNodes * p;
    ::CoCreateInstance(CLSID_ScdNodes, NULL, CLSCTX_ALL, IID_IScdNodes, (void**)&p);
    p->SetObjectInfo(eType, (DWORD)NULL, eType, eScdDirection_All);
    //*pVal=p;
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdNodes::AddUnit(BSTR bsClass, BSTR bsTag, IScdNode ** pUnit)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    _bstr_t bClass(bsClass);
    _bstr_t bTag(bsTag);
    LPTSTR Class=bClass;
    LPTSTR Tag=bTag;

    if (!gs_pSfeSrvr)
      return Scd.Return(eScdUC_Failed, "FlwSolver Missing : AddUnit '%s' of type '%s' failed!", Tag, Class);

    bool DataErr = (CheckTag(Tag)!=0);
    if (DataErr)
      return Scd.Return(eScdUC_TagExists, "Tag exists : AddUnit '%s' of type '%s' failed!", Tag, Class);

    RequestModelInfoRec MInfo;
    DataErr = !gs_pSfeSrvr->RequestModelInfoByClassId(Class, MInfo);
    if (DataErr)
      return Scd.Return(eScdUC_UnknownClass, "Unknown Class Type : AddUnit '%s' of type '%s' failed!", Tag, Class);

    //DWORD LicCat = gs_License.LicCatagories();
    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
    //if (DataErr || _stricmp(MInfo.Group(), FlwUnitGrp)!=0)
    //  return Scd.Return(eScdUC_BadLicense, "Class Type NOT allowed: AddUnit '%s' of type '%s' failed!", Tag, Class);

    //TODO this errcode needs to be sorted out
    FlwNode * pNd=NULL;//gs_pSfeSrvr->FindNode(Name);
    int err = gs_pSfeSrvr->FE_DoInsert(Class, NULL, NULL, Tag, NULL, NULL, &pNd);
    if (err)
      return Scd.Return(eScdUC_Failed, "Error %i : AddUnit '%s' of type '%s' failed!", err, Tag, Class);
    //gs_LicenseCnt.CalcUnits();
    ASSERT(pNd!=NULL);
    if (pNd)
      {
      IScdNode * p;
      ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
      p->SetObjectInfo((DWORD)pNd);
      *pUnit=p; // User must release
      return S_OK;
      }
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodes::AddLink(BSTR bsClass, BSTR bsTag, BSTR bsSrcTag, BSTR bsSrcIO, BSTR bsDstTag, BSTR bsDstIO, IScdNode ** pLink)
  {
  dllSCD_COMENTRY(long)
    {
    SCD_LclStr(Class,  bsClass );
    SCD_LclStr(Tag,    bsTag   );
    SCD_LclStr(SrcTag, bsSrcTag);
    SCD_LclStr(DstTag, bsDstTag);
    SCD_LclStr(SrcIO,  bsSrcIO );
    SCD_LclStr(DstIO,  bsDstIO );

    if (!gs_pSfeSrvr)
      return Scd.Return(eScdUC_Failed, "FlwSolver Missing : CreateLink '%s' failed!", Tag);

    bool DataErr = (CheckTag(Tag)!=0);
    if (DataErr)
      return Scd.Return(1, "Tag exists : CreateLink '%s' failed!", Tag);

    DataErr = (CheckTag(SrcTag)==0);
    if (DataErr)
      return Scd.Return(3, "SrcTag '%s' not found : CreateLink '%s' failed!", SrcTag, Tag);

    DataErr = (CheckTag(DstTag)==0);
    if (DataErr)
      return Scd.Return(2, "DstTag '%s' not found : CreateLink '%s' failed!", DstTag, Tag);

    RequestModelInfoRec MInfo;
    DataErr = !gs_pSfeSrvr->RequestModelInfoByClassId((char*)Class, MInfo);
    if (DataErr)
      return Scd.Return(4, "Unknown Class Type : CreateLink '%s' of type '%s' failed!", Tag, Class);

    //DWORD LicCat = gs_License.LicCatagories();
    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
    //if (DataErr || _stricmp(MInfo.Group(), FlwLinkGrp)!=0)
    //  return ScdError(10, MyFnName, "Class Type NOT allowed: CreateLink '%s' of type '%s' failed!", Tag, Class);

    CString Src, Dst;
    Src.Format("%s.%s", SrcTag, SrcIO);
    Dst.Format("%s.%s", DstTag, DstIO);

    FlwNode * pNd=NULL;
    int err = gs_pSfeSrvr->FE_DoInsert((LPTSTR)Class, NULL, NULL, (LPTSTR)Tag, (LPTSTR)(LPCTSTR)Src, (LPTSTR)(LPCTSTR)Dst, &pNd);
    if (err)
      return Scd.Return(5, "Connection not Made:\n%s:%s\n%s.%s->%s.%s", Class, Tag, SrcTag, SrcIO, DstTag, DstIO);
    ASSERT(pNd!=NULL);
    if (pNd)
      {
      IScdNode * p;
      ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
      p->SetObjectInfo((DWORD)pNd);
      *pLink=p; // User must release
      return S_OK;
      }
    //gs_LicenseCnt.CalcUnits();

    //    bool DataErr = (CheckTag(Tag)!=0);
    //    if (DataErr)
    //      return ScdError(1, MyFnName, "Tag exists : AddUnit '%s' of type '%s' failed!", Tag, Class);
    //
    //    RequestModelInfoRec MInfo;
    //    DataErr = !gs_pSfeSrvr->RequestModelInfoByClassId(Class, MInfo);
    //    if (DataErr)
    //      return ScdError(2, MyFnName, "Unknown Class Type : AddUnit '%s' of type '%s' failed!", Tag, Class);
    //
    //    //DWORD LicCat = gs_License.LicCatagories();
    //    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
    //    //if (DataErr || _stricmp(MInfo.Group(), FlwUnitGrp)!=0)
    //    //  return ScdError(3, MyFnName, "Class Type NOT allowed: AddUnit '%s' of type '%s' failed!", Tag, Class);
    //
    //    int err = gs_pSfeSrvr->FE_DoInsert(Class, Tag, NULL, NULL);
    //    if (err)
    //      return ScdError(4, MyFnName, "Error %i : AddUnit '%s' of type '%s' failed!", err, Tag, Class);
    //gs_LicenseCnt.CalcLinks();
    }		
  SCD_COMEXIT
  }

STDMETHODIMP CScdNodes::AddCable(BSTR bsClass, BSTR bsTag, BSTR bsSrcTag, BSTR bsSrcIO, BSTR bsSrcTS, BSTR bsDstTag, BSTR bsDstIO, BSTR bsDstTS, IScdNode ** pLink)
  {
  dllSCD_COMENTRY(long)
    {
    SCD_LclStr(Class,  bsClass );
    SCD_LclStr(Tag,    bsTag   );
    SCD_LclStr(SrcTag, bsSrcTag);
    SCD_LclStr(DstTag, bsDstTag);
    SCD_LclStr(SrcIO,  bsSrcIO );
    SCD_LclStr(DstIO,  bsDstIO );
    SCD_LclStr(SrcTS,  bsSrcTS );
    SCD_LclStr(DstTS,  bsDstTS );

    if (!gs_pSfeSrvr)
      return Scd.Return(eScdUC_Failed, "FlwSolver Missing : CreateLink '%s' failed!", Tag);

    bool DataErr = (CheckTag(Tag)!=0);
    if (DataErr)
      return Scd.Return(1, "Tag exists : CreateLink '%s' failed!", Tag);

    DataErr = (CheckTag(SrcTag)==0);
    if (DataErr)
      return Scd.Return(3, "SrcTag '%s' not found : CreateLink '%s' failed!", SrcTag, Tag);

    DataErr = (CheckTag(DstTag)==0);
    if (DataErr)
      return Scd.Return(2, "DstTag '%s' not found : CreateLink '%s' failed!", DstTag, Tag);

    RequestModelInfoRec MInfo;
    DataErr = !gs_pSfeSrvr->RequestModelInfoByClassId((char*)Class, MInfo);
    if (DataErr)
      return Scd.Return(4, "Unknown Class Type : CreateLink '%s' of type '%s' failed!", Tag, Class);

    //DWORD LicCat = gs_License.LicCatagories();
    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
    //if (DataErr || _stricmp(MInfo.Group(), FlwLinkGrp)!=0)
    //  return ScdError(10, MyFnName, "Class Type NOT allowed: CreateLink '%s' of type '%s' failed!", Tag, Class);

    CString Src, Dst;
    Src.Format("%s.%s", SrcTag, SrcIO);
    Dst.Format("%s.%s", DstTag, DstIO);

    FlwNode * pNd=NULL;
    int err = gs_pSfeSrvr->FE_DoInsert((LPTSTR)Class, NULL, NULL, (LPTSTR)Tag, (LPTSTR)(LPCTSTR)Src, (LPTSTR)(LPCTSTR)Dst, &pNd);
    if (err)
      return Scd.Return(5, "Connection not Made:\n%s:%s\n%s.%s->%s.%s", Class, Tag, SrcTag, SrcIO, DstTag, DstIO);

    CLinkWiring LW;
    LW.m_bAutoWire=true;
    LW.m_LinkTag=pNd->FullObjTag();
    LW.m_SrcTS=SrcTS;
    LW.m_DstTS=DstTS;

    if (pNd->SetLinkWiring(LW)<0)
      return Scd.Return(5, "AutoWiring Failed:\n%s:%s\n%s.%s.%s->%s.%s.%s", Class, Tag, SrcTag, SrcIO, SrcTS, DstTag, DstIO, DstTS);

    ASSERT(pNd!=NULL);
    if (pNd)
      {
      IScdNode * p;
      ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
      p->SetObjectInfo((DWORD)pNd);
      *pLink=p; // User must release
      return S_OK;
      }
    //gs_LicenseCnt.CalcUnits();

    //    bool DataErr = (CheckTag(Tag)!=0);
    //    if (DataErr)
    //      return ScdError(1, MyFnName, "Tag exists : AddUnit '%s' of type '%s' failed!", Tag, Class);
    //
    //    RequestModelInfoRec MInfo;
    //    DataErr = !gs_pSfeSrvr->RequestModelInfoByClassId(Class, MInfo);
    //    if (DataErr)
    //      return ScdError(2, MyFnName, "Unknown Class Type : AddUnit '%s' of type '%s' failed!", Tag, Class);
    //
    //    //DWORD LicCat = gs_License.LicCatagories();
    //    //DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
    //    //if (DataErr || _stricmp(MInfo.Group(), FlwUnitGrp)!=0)
    //    //  return ScdError(3, MyFnName, "Class Type NOT allowed: AddUnit '%s' of type '%s' failed!", Tag, Class);
    //
    //    int err = gs_pSfeSrvr->FE_DoInsert(Class, Tag, NULL, NULL);
    //    if (err)
    //      return ScdError(4, MyFnName, "Error %i : AddUnit '%s' of type '%s' failed!", err, Tag, Class);
    //gs_LicenseCnt.CalcLinks();
    }		
  SCD_COMEXIT
  }
