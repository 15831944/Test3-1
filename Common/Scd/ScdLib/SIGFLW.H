//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SIGFLW_H
#define  __SIGFLW_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __VECTORS_H
  #include "vectors.h"
#endif  
#ifndef __SCDTEMPL_H
  #include "scdtempl.h"
#endif  
#ifndef __EQNSOLVE_H
  #include "eqnsolve.h"
#endif  

#if defined(__SIGFLW_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================
//
//
//
// =========================================================================

_FWDDEF(CSignalFlowGraph);
_FWDDEF(CSFGNodeInfo);
_FWDDEF(CSFGNode);
_FWDDEF(CSFGEdge);

class CSFGEdge
  {
  friend class CSignalFlowGraph;
  friend class CSFGNode;
  friend class CSFGEdgeList;
  protected:
    CSFGNode       *pSrcNode, *pDstNode;
  public:
    CSFGEdge()  { pSrcNode=pDstNode=NULL; };
    ~CSFGEdge() { };

  };

//-------------------------------------------------------------------------

class CSFGEdgeList    : public CSCDList<CSFGEdge*, CSFGEdge*> { };
typedef CSCDPtrListIter<CSFGEdgeList, CSFGEdge*> CSFGEdgeIter;
typedef CSCDPtrListIterWithPos<CSFGEdgeList, CSFGEdge*> CSFGEdgeIterPos;

//-------------------------------------------------------------------------

class DllImportExport CSFGNodeInfo
  {
  friend class CSignalFlowGraph;
  protected:
    CSFGNode*  m_pNode;

  public:
    CSFGNodeInfo() { m_pNode=NULL;};
    
    virtual LPCTSTR SFGNodeTag(TaggedObject * pRefTo=NULL)=0;
    virtual CSFGNodeInfo * SFGNodePtr()=0;
  };

class CSFGNodeInfoList    : public CSCDList<CSFGNodeInfo*, CSFGNodeInfo*> { };
typedef CSCDPtrListIter<CSFGNodeInfoList, CSFGNodeInfo*> CSFGNodeInfoIter;
typedef CSCDPtrListIterWithPos<CSFGNodeInfoList, CSFGNodeInfo*> CSFGNodeInfoIterPos;

//-------------------------------------------------------------------------

class CSFGNode
  {
  friend class CSignalFlowGraph;
  friend class CSFGEdge;
  friend class CSFGEdgeList;
  protected:
    CSFGEdgeList    EdgesIn, EdgesOut;
    CSFGNodeInfo    *pInfo;
    byte           iPriority;
  public:
    CSFGNode()                   { pInfo=NULL; };
    CSFGNode(CSFGNodeInfo * Info) { pInfo=Info; };
    virtual ~CSFGNode()                { };  // turn this into a polymorphic class for TearVarPosBlk
  };

//-------------------------------------------------------------------------

class CSFGNodeList    : public CSCDList<CSFGNode*, CSFGNode*> { };
typedef CSCDPtrListIter<CSFGNodeList, CSFGNode*> CSFGNodeIter;
typedef CSCDPtrListIterWithPos<CSFGNodeList, CSFGNode*> CSFGNodeIterPos;

//-------------------------------------------------------------------------

class DllImportExport CSignalFlowGraph
  {
  friend class CSFGNodeInfo;
  friend class CSFGNode;
  friend class CSFGEdge;
  protected:
    
    CSFGNodeList   Nodes[TP_Last+1], FreeNodes;
    CSFGEdgeList   Edges, FreeEdges;

    //CFlangeList    TornNodes;
    CSFGNodeInfoList TornNodes;

  public:
    CSignalFlowGraph();
    ~CSignalFlowGraph();

    void          AddNode(CSFGNodeInfo* pTObj, byte iPriority);
    void          AddEdge(CSFGNodeInfo* pSrcObj, CSFGNodeInfo* pDstObj, char*pViaTag);

    void          Clear();
    int           TearDirect(); // returns the Numbers of Tear 
    CSFGNodeInfoList & TearNodeList() { return TornNodes; };
    int           TearCount() { return TornNodes.GetCount();};

  protected:
    CSFGNode*      AllocNode();
    CSFGEdge*      AllocEdge();
    void          FreeNode(CSFGNode* pNode);
    void          FreeEdge(CSFGEdge* pEdge);
    void          DelNode(CSFGNode* pNode);
    void          DelEdge(CSFGEdge* pEdge);

    void          AddEdge(CSFGNode* pSrcNd, CSFGNode* pDstNd, char*pViaTag);

    void          CountEdges(CSFGNode* pNode, int &NIn, int &NOut);
    flag          SelfLoop(CSFGNode* pNode);
    
    void          ReduceEssen(CSFGNode* pNode);
    void          ReduceNonEssen(CSFGNode* pNode);


  };

// =========================================================================
#undef DllImportExport 

#endif
