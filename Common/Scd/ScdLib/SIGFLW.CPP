//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
//#include "stdafx.h"
//#include <string.h>
//#include <math.h>
//#include <stdio.h>

#include "stdafx.h"
#include "sc_defs.h"
#define  __SIGFLW_CPP
#include "sigflw.h"
#include "dbgmngr.h"

// ========================================================================

#define dbgSFG  01

#if dbgSFG 
static CDbgMngr dbgSFGBuild      ("SigFlwGrf",  "Build");
static CDbgMngr dbgSFGTearProcess("SigFlwGrf",  "TearProcess");
static CDbgMngr dbgSFGShowTear   ("SigFlwGrf",  "ShowTear");
#endif

// ========================================================================
//
//
//
// ========================================================================

CSignalFlowGraph::CSignalFlowGraph() 
  {
  };

// ------------------------------------------------------------------------

CSignalFlowGraph::~CSignalFlowGraph() 
  {
  Clear();
  while (!FreeNodes.IsEmpty())
    delete FreeNodes.RemoveHead();

  while (!FreeEdges.IsEmpty())
    delete FreeEdges.RemoveHead();
  }

// ------------------------------------------------------------------------

CSFGNode* CSignalFlowGraph::AllocNode()
  {
  CSFGNode* pNd = FreeNodes.GetCount()>0 ? FreeNodes.RemoveHead() : new CSFGNode;
  pNd->pInfo = NULL;
  return pNd;
  };

// ------------------------------------------------------------------------

CSFGEdge* CSignalFlowGraph::AllocEdge()
  {
  CSFGEdge* pEd = FreeEdges.GetCount()>0 ? FreeEdges.RemoveHead() : new CSFGEdge;

  pEd->pSrcNode = NULL;
  pEd->pDstNode = NULL;
  return pEd;
  };

// ------------------------------------------------------------------------

void CSignalFlowGraph::FreeNode(CSFGNode* pNode) { FreeNodes.AddHead(pNode); };
void CSignalFlowGraph::FreeEdge(CSFGEdge* pEdge) 
  { 
  FreeEdges.AddHead(pEdge); 
  };

// ------------------------------------------------------------------------

void CSignalFlowGraph::AddNode(CSFGNodeInfo* pInfo, byte iPriority)
  {

  if (pInfo)
    {
    CSFGNode* pNode=AllocNode();
    pNode->pInfo = pInfo;
    pNode->iPriority = iPriority;
    pInfo->m_pNode=/*(PVOID)*/pNode;

    Nodes[pNode->iPriority].AddTail(pNode);
    }
  else
    {
    }
  #if dbgSFG 
  if (dbgSFGBuild())
    dbgpln("Add Node   [%2i]   [%2i][%2i] %s",iPriority,
    pInfo->m_pNode->EdgesIn.GetCount(),
    pInfo->m_pNode->EdgesOut.GetCount(),
    pInfo?pInfo->SFGNodeTag() : "?????????");
  #endif
  }

// ------------------------------------------------------------------------

void CSignalFlowGraph::AddEdge(CSFGNodeInfo*  pSrcObj, CSFGNodeInfo*  pDstObj, char* pViaTag)
  {
  CSFGNode* pSrcNd=(CSFGNode*)pSrcObj->m_pNode;
  CSFGNode* pDstNd=(CSFGNode*)pDstObj->m_pNode;
  if (pSrcNd && pDstNd)
    AddEdge(pSrcNd, pDstNd, pViaTag);
  }

// ------------------------------------------------------------------------

void CSignalFlowGraph::AddEdge(CSFGNode* pSrcNd, CSFGNode* pDstNd, char* pViaTag)
  {
  ASSERT(pSrcNd);
  ASSERT(pDstNd);
  flag Found = False;//(pSrcNd->EdgesOut.FindEdgeWithDstNode(pDstNd) != NULL);
  CSFGEdgeIter EO(pSrcNd->EdgesOut);
  for (CSFGEdge* p=EO.First(); (p); p=EO.Next())
    if (p->pDstNode==pDstNd)
      {
      Found = True;
      break;
      }

  if (!Found)
    {
    CSFGEdge* pEdge=AllocEdge();
    pEdge->pSrcNode=pSrcNd;
    pEdge->pDstNode=pDstNd;

    pDstNd->EdgesIn.AddTail(pEdge);
    pSrcNd->EdgesOut.AddTail(pEdge);
    // no add one      
    #if dbgSFG 
    if (dbgSFGBuild())
      {
      //dbgp("Add Edge      %s -> %s",pSrcNd->pInfo->pNd1->Tag(),pDstNd->pInfo->pNd1->Tag());
      dbgp("Add Edge      %s[%i] -> %s[%i]",
        pSrcNd->pInfo->SFGNodeTag(),
        pSrcNd->EdgesOut.GetCount(),
        pDstNd->pInfo->SFGNodeTag(),
        pDstNd->EdgesIn.GetCount());
      if (pViaTag)
        dbgp(" [%s]",pViaTag);
      dbgpln("");
      }
    #endif
    }
  else
    {
    #if dbgSFG 
    if (dbgSFGBuild())
      //dbgpln("Edge Exists   %s -> %s",pSrcNd->pInfo->pNd1->Tag(),pDstNd->pInfo->pNd1->Tag());
      dbgpln("Edge Exists   %s[%i] -> %s[%i]",
        pSrcNd->pInfo->SFGNodeTag(),
        pSrcNd->EdgesOut.GetCount(),
        pDstNd->pInfo->SFGNodeTag(),
        pDstNd->EdgesIn.GetCount());
    #endif
    }
  }

// ------------------------------------------------------------------------

void CSignalFlowGraph::DelNode(CSFGNode* pNode)
  {
  CSFGNodeIterPos Nd(Nodes[pNode->iPriority]);
  for (CSFGNode* p=Nd.First(); (p); p=Nd.Next())
    if (p==pNode)
      break;
  ASSERT(p);
  ASSERT(Nd.Pos());
  //POSITION pos=Nodes.Find(pNode);
  Nodes[pNode->iPriority].RemoveAt(Nd.Pos());
  FreeNode(pNode);
  }

// ------------------------------------------------------------------------

void CSignalFlowGraph::DelEdge(CSFGEdge* pEdge)
  {
  CSFGNode* pSrcNode=pEdge->pSrcNode;
  CSFGNode* pDstNode=pEdge->pDstNode; 
  
  // .. disconnect pSrcNode         
  if (pSrcNode)
    {
    CSFGEdgeIterPos EO(pSrcNode->EdgesOut);
    for (CSFGEdge* p=EO.First(); (p); p=EO.Next())
      if (p==pEdge)
        break;
    ASSERT(p);
    ASSERT(EO.Pos());
    pSrcNode->EdgesOut.RemoveAt(EO.Pos());
    }
                                  
  // .. disconnect pDstNode
  if (pDstNode)
    {
    CSFGEdgeIterPos EI(pDstNode->EdgesIn);
    for (CSFGEdge* p=EI.First(); (p); p=EI.Next())
      if (p==pEdge)
        break;
    ASSERT(p);
    ASSERT(EI.Pos());
    pDstNode->EdgesIn.RemoveAt(EI.Pos());

//    POSITION pos=pDstNode->EdgesIn.Find(pEdge);
//    if (pos)
//      pDstNode->EdgesIn.RemoveAt(pos);
    }
  
  FreeEdge(pEdge);
  }
        
// ------------------------------------------------------------------------

void CSignalFlowGraph::Clear()
  {

  if (dbgSFGBuild())
    {
    dbgpln("CSignalFlowGraph Clear");
    }
  
  for (int i=0; i<=TP_Last; i++)
    while (!Nodes[i].IsEmpty())
      {
      CSFGNode *pNd=Nodes[i].RemoveHead();
      pNd->EdgesIn.RemoveAllToFreeList();
      pNd->EdgesOut.RemoveAllToFreeList();
      FreeNodes.AddHead(pNd);
      }

  while (!Edges.IsEmpty())
    FreeEdges.AddHead(Edges.RemoveHead());

  }

// ------------------------------------------------------------------------

void CSignalFlowGraph::CountEdges(CSFGNode* pNode, int &NIn, int &NOut)
  {
  NIn = pNode->EdgesIn.GetCount();
  NOut = pNode->EdgesOut.GetCount();
  }

// ------------------------------------------------------------------------
      
flag CSignalFlowGraph::SelfLoop(CSFGNode* pNode)
  {    
  CSFGEdgeIter It(pNode->EdgesOut);
  for (CSFGEdge* p=It.First(); (p); p=It.Next())
    if (p->pDstNode==pNode)
      return True;
  return False;
  }   
      
// ------------------------------------------------------------------------
      
void CSignalFlowGraph::ReduceEssen(CSFGNode* pNode)
  {
  #if dbgSFG 
  if (dbgSFGTearProcess())
    //dbgpln("Reduce     Essential %s ",pNode->pInfo->pNd1->Tag());
    dbgpln("Reduce     Essential %s ",pNode->pInfo->SFGNodeTag());
  #endif

  CSFGEdgeList & EdgesIn = pNode->EdgesIn;
  while (!EdgesIn.IsEmpty())
    DelEdge(EdgesIn.GetHead());
  CSFGEdgeList & EdgesOut = pNode->EdgesOut;
  while (!EdgesOut.IsEmpty())
    DelEdge(EdgesOut.GetHead());

  DelNode(pNode);
  }
      
// ------------------------------------------------------------------------
      
void CSignalFlowGraph::ReduceNonEssen(CSFGNode* pNode)
  {    
  #if dbgSFG 
  if (dbgSFGTearProcess())
    dbgpln("Reduce Non Essential %s ",pNode->pInfo->SFGNodeTag());
  #endif

  CSFGEdgeIter I(pNode->EdgesIn);
  for (CSFGEdge* pIEdge=I.First(); (pIEdge); pIEdge=I.Next())
    {
    CSFGEdgeIter O(pNode->EdgesOut);
    for (CSFGEdge* pOEdge=O.First(); (pOEdge); pOEdge=O.Next())
      AddEdge(pIEdge->pSrcNode, pOEdge->pDstNode, NULL);
    }
  
  ReduceEssen(pNode);
  }  
      
// ------------------------------------------------------------------------

#pragma warning (disable : 4102) // Unreferenced Labels
     
int CSignalFlowGraph::TearDirect()
  {
  // Direct Tearing Algorithm 
  //
  // From :
  //   Decomposition Algorithm for Chemical Process Simulation
  //   P.Ollero & C Amselem
  //   Checmical Eng Research & Design Vol 61 Sept 1983
  // 

  #if dbgSFG 
  if (dbgSFGTearProcess() || dbgSFGBuild())
    dbgpln("Tear direct           - START");
  #endif

  int NIn, NOut, ES, NodesExamined, iList;
  flag TearNodeFound;
  POSITION pos;
  CSFGNode* pNode;

  TornNodes.RemoveAllToFreeList();
  
  DTA_2:  ES=1;
          TearNodeFound = FALSE;
          NodesExamined = 0;
          pos=NULL;
          //iList=TP_Last; // Start with least preferred nodes

  DTA_3:  if (pos==NULL)
            {
            iList=TP_Last; // Start with least preferred nodes
            pos=Nodes[iList].GetHeadPosition();
            }
          while (pos==NULL && iList!=TP_First)
            {
            iList--;
            pos=Nodes[iList].GetHeadPosition();
            }
          if (pos==NULL)
            goto DTA_Stop;
          pNode=Nodes[iList].GetNext(pos);


  DTA_4:  NodesExamined++;
          CountEdges(pNode, NIn, NOut);
          #if dbgSFG 
          if (dbgSFGTearProcess())
            {
            dbgp("    : [%5i,%5i] [%3i,%3i]%s",ES,NodesExamined,NIn,NOut,pNode->pInfo->SFGNodeTag());
            CSFGEdgeIter It(pNode->EdgesOut);
            for (CSFGEdge* p=It.First(); (p); p=It.Next())
              dbgp(" %s",p->pDstNode->pInfo->SFGNodeTag());
            dbgpln("");
            }
          #endif

          if (NIn == 0 || NOut == 0)
            {
            ReduceEssen(pNode);
            NodesExamined=0;
            ES=1; // CNM
            goto DTA_7;
            }

  DTA_5:  if (SelfLoop(pNode))
            {
            TornNodes.AddTail(pNode->pInfo);
            TearNodeFound = True;
            #if dbgSFG 
            if (dbgSFGShowTear())
              dbgpln("Tear Found ------------------------- : %s",pNode->pInfo->SFGNodeTag());
            #endif
            ReduceEssen(pNode);
            NodesExamined=0;
            ES=1; // CNM
            goto DTA_7;
            }
      
  DTA_6:  if (ES == NIn || ES == NOut)
            {
            ReduceNonEssen(pNode);
            NodesExamined=0;
            goto DTA_7;
            }
          else
            goto DTA_8;

  DTA_7:  if (Nodes[TP_First].IsEmpty() &&
              Nodes[TP_Normal].IsEmpty() &&
              Nodes[TP_Last].IsEmpty())
            goto DTA_Stop;

  DTA_8:  if (NodesExamined>=(Nodes[TP_First].GetCount()+
                              Nodes[TP_Normal].GetCount()+
                              Nodes[TP_Last].GetCount()))
            goto DTA_9;
          goto DTA_3;

  DTA_9:  if (TearNodeFound)
            goto DTA_2;
          ES++;
          NodesExamined = 0;
          goto DTA_3;

  DTA_Stop:

  /*
      if (ES>10)
        {
        POSITION pos=Nodes.GetHeadPosition();
        while (pos)
          {
          CSFGNode* pNode=Nodes.GetNext(pos);
          CountEdges(pNode, NIn, NOut);
          dbgpln("SFG  :%s",pNode->pInfo->FullObjTag());
          POSITION pos1=pNode->EdgesIn.GetHeadPosition();
          dbgp("  In :");
          while (pos1)
            {
            CSFGEdge* pEdge=pNode->EdgesIn.GetNext(pos1);
            dbgp(" %s,",pEdge->pSrcNode==pNode ? pEdge->pDstNode->pInfo->FullObjTag() :  pEdge->pSrcNode->pInfo->FullObjTag());
            }
          dbgpln("");
          pos1=pNode->EdgesOut.GetHeadPosition();
          dbgp("  Out:");
          while (pos1)
            {
            CSFGEdge* pEdge=pNode->EdgesOut.GetNext(pos1);
            dbgp(" %s,",pEdge->pSrcNode==pNode ? pEdge->pDstNode->pInfo->FullObjTag() :  pEdge->pSrcNode->pInfo->FullObjTag());
            }
          dbgpln("");
          }
        }
      }
    }
  */

  #if dbgSFG 
  if (dbgSFGTearProcess() || dbgSFGBuild())
    dbgpln("Tear direct           - END");
  #endif
  return TornNodes.GetCount();
  }

// ========================================================================
//
//
//
//===========================================================================
