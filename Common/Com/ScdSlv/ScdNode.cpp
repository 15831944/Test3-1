// ScdNode.cpp : Implementation of CScdNode
#include "stdafx.h"
#include "ScdSlv.h"
#include "ScdNode.h"
#include "ScdNodes.h"
//#include "sfe_srvr.h"
#include "flwsolve.h"
#include "scdslvwrap.h"
#include ".\scdnode.h"

/////////////////////////////////////////////////////////////////////////////
// CScdNode

STDMETHODIMP CScdNode::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] = 
    {
    &IID_IScdNode
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }               

STDMETHODIMP CScdNode::get_Tag(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    SCD_Copy2BSTR(pVal, m_pNode->Tag());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::put_Tag(BSTR newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_TOPO_OK;

    SCD_LclStr(Tag, newVal);
    m_pNode->Tag((LPTSTR)Tag);
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::get_Type(eScdNodeTypes *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    *pVal=(eScdNodeTypes)m_pNode->m_dwType;;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::get_TypeString(BSTR *pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;
    Strng T;
    SCD_Copy2BSTR(pVal, m_pNode->TypeString(T));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::get_Class(IScdNodeClass **pClass)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pClass)
    {
    CHECK_TOPO_OK;

    IScdNodeClass * p;
    ::CoCreateInstance(CLSID_ScdNodeClass, NULL, CLSCTX_ALL, IID_IScdNodeClass, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode->Class());
    *pClass=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::SetObjectInfo(DWORD newVal)
  {
  dllSCD_COMENTRY(eScdNodeCodes)
    {
    CHECK_SFE_OK;
    m_pNode=(FlwNode*)newVal;
    m_lTopoCount=gs_pSfeSrvr->FE_GetTopologyCount();
    }		
  SCD_COMEXIT
  }


STDMETHODIMP CScdNode::get_Connects(eScdDirections Direction, eScdNodeTypes Types, IScdConnects **pConnects)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pConnects)
    {
    CHECK_TOPO_OK;

    IScdConnects * p;
    ::CoCreateInstance(CLSID_ScdConnects, NULL, CLSCTX_ALL, IID_IScdConnects, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode);
    *pConnects=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::get_ConnectPoints(eScdDirections Direction, eScdNodeTypes Types, IScdConnectPoints **pConnectPoints)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pConnectPoints)
    {
    CHECK_TOPO_OK;

    IScdConnectPoints * p;
    ::CoCreateInstance(CLSID_ScdConnectPoints, NULL, CLSCTX_ALL, IID_IScdConnectPoints, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode);
    *pConnectPoints=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::get_SrcNode(IScdNode **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    FlwNode * pRmt=NULL;

    DWORD Type=m_pNode->m_dwType;
    if (Type&(nc_MSrcSnk) && m_pNode->NoFlwIOs()==1 && m_pNode->IODesc_Self(0)->Dirn&LIO_In) 
      pRmt=m_pNode->Nd_Rmt(0);
    else if (Type&(nc_MSrcSnk) && m_pNode->NoFlwIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(0);
    else if (Type&(nc_MLnk|nc_MBst) && m_pNode->NoFlwIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(0);
    else if (Type&(nc_CLnk) && m_pNode->NoCIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(m_pNode->CIO1()+0);
    else if (Type&(nc_ELnk) && m_pNode->NoEIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(m_pNode->EIO1()+0);
    else if (Type&(nc_ALnk) && m_pNode->NoAIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(m_pNode->AIO1()+0);
    if (pRmt)
      {
      IScdNode * p;
      ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
      p->SetObjectInfo((DWORD)pRmt);
      *pVal=p; // User must release
      }
    return S_OK;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdNode::get_DstNode(IScdNode **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    FlwNode * pRmt=NULL;

    DWORD Type=m_pNode->m_dwType;
    if (Type&(nc_MSrcSnk) && m_pNode->NoFlwIOs()==1 && m_pNode->IODesc_Self(0)->Dirn&LIO_Out) 
      pRmt=m_pNode->Nd_Rmt(0);
    else if (Type&(nc_MSrcSnk) && m_pNode->NoFlwIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(1);
    else if (Type&(nc_MLnk|nc_MBst) && m_pNode->NoFlwIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(1);
    else if (Type&(nc_CLnk) && m_pNode->NoCIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(m_pNode->CIO1()+1);
    else if (Type&(nc_ELnk) && m_pNode->NoEIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(m_pNode->EIO1()+1);
    else if (Type&(nc_ALnk) && m_pNode->NoAIOs()==2) 
      pRmt=m_pNode->Nd_Rmt(m_pNode->AIO1()+1);
    if (pRmt)
      {
      IScdNode * p;
      ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
      p->SetObjectInfo((DWORD)pRmt);
      *pVal=p; // User must release
      }
    return S_OK;
    }
  SCD_COMEXIT
  }

//STDMETHODIMP CScdNode::get_RmtNode(long Index, eScdDirections eDirn, IScdNode **pVal)
//  {
//  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
//    {
//    if (!m_pNode || !gs_pSfeSrvr || m_lTopoCount!=gs_pSfeSrvr->GetTopologyCount())
//      return Scd.ReturnH(E_FAIL);
//
//    FlwNode * pRmt=NULL;
//
//    if (Index<m_pNode->m_nIOs)
//      {
//      DWORD Type=m_pNode->m_dwType;
//      
//      if (Type&(nc_Process|nc_MSrcSnk|nc_MXfer|nc_MBst|nc_Control|nc_Elec))
//        {
//        FlwNode * p=m_pNode->Nd_Rmt(Index);
//        if (p)
//          {
//          bool DoTest=false;
//          if (p->m_dwType&(nc_MLnk))
//            DoTest=(p->NoFlwIOs()==2);
//          else if (p->m_dwType&(nc_CLnk))
//            DoTest=(p->NoCIOs()==2);
//          else if (p->m_dwType&(nc_ELnk))
//            DoTest=(p->NoEIOs()==2);
//          else if (p->m_dwType&(nc_ALnk))
//            DoTest=(p->NoAIOs()==2);
//          if (DoTest)
//            {
//            long IRmt=m_pNode->IOIONo_Rmt(Index);
//            if ((IRmt==1 && eDirn&eScdDirection_In) || 
//                (IRmt==0 && eDirn&eScdDirection_Out))
//              pRmt=p;
//            }
//          }
//        }
//      else if (Type&(nc_MLnk|nc_CLnk|nc_ELnk|nc_ALnk) && m_pNode->NoFlwIOs()==2) 
//        {
//        if ((Index==0 && (eDirn&eScdDirection_In)) || 
//            (Index==1 && (eDirn&eScdDirection_Out)))
//          pRmt=m_pNode->Nd_Rmt(Index);
//        }
//      if (pRmt)
//        {
//        IScdNode * p;
//        ::CoCreateInstance(CLSID_ScdNode, NULL, CLSCTX_ALL, IID_IScdNode, (void**)&p);
//        p->SetObjectInfo((DWORD)pRmt);
//        *pVal=p; // User must release
//        }
//      }
//    return S_OK;
//    }
//  SCD_COMEXIT
//  }
//
//STDMETHODIMP CScdNode::get_RmtNodeBound(long *pVal)
//  {
//  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
//    {
//    if (!m_pNode || !gs_pSfeSrvr || m_lTopoCount!=gs_pSfeSrvr->GetTopologyCount())
//      return Scd.ReturnH(E_FAIL);
//    *pVal=m_pNode->m_IOs-1;
//    return S_OK;
//    }
//  SCD_COMEXIT
//  }

STDMETHODIMP CScdNode::get_RmtNodes(eScdDirections Dirns, eScdNodeTypes Types, IScdNodes **pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    IScdNodes * p;
    ::CoCreateInstance(CLSID_ScdNodes, NULL, CLSCTX_ALL, IID_IScdNodes, (void**)&p);
    p->SetObjectInfo(eWhatNodes_RmtNodes, (DWORD)m_pNode, Types, Dirns);
    *pVal=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdNode::get_Electrics(IScdElectrics** pVal)
  {
  dllSCD_COMENTRYGET(eScdNodeCodes, pVal)
    {
    CHECK_TOPO_OK;

    IScdElectrics * p;
    ::CoCreateInstance(CLSID_ScdElectrics, NULL, CLSCTX_ALL, IID_IScdElectrics, (void**)&p);
    p->SetObjectInfo((DWORD)m_pNode);
    *pVal=p; // User must release
    return S_OK;
    }
  SCD_COMEXIT
  }
