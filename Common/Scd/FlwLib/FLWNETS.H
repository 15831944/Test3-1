//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLWNETS_H
#define  __FLWNETS_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif

#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __FSTHREAD_H
  #include "fsthread.h"
#endif

#ifdef __FLWNETS_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================
//
//
//
//============================================================================

#define DISTERRORINTRAVERSE  01

extern DllImportExport const double MinFxdQmDiff;
extern DllImportExport const double ArrayReAnalyseRatio;
extern DllImportExport const double MinimumResistance;
extern DllImportExport const double NetMaxResRatioInit;

extern DllImportExport const int    MaxBuildMatrixPasses;
extern DllImportExport const int    MaxNetSolvePasses;

extern DllImportExport const double MaxdQMult;
extern DllImportExport const int    ConvergeModulo;
extern DllImportExport const double SignificanceRatio;

//============================================================================

struct GrpFlwBlkHiImp
  {
  int               fLimit;
  //double            Scale;
  //CGrpFlwBlk *        xpLoGrp;
  };

//----------------------------------------------------------------------------

class CGrpFlwBlkSD
  {
  friend class CGrpFlwBlk;
  public:

    flag            IsIO()        { return m_iIONo>=0; };
    flag            Joined()      { return m_pJoin!=NULL; };
    CJoinRec       * Join()        { return m_pJoin; };
    int             PInx()        { return m_pJoin->TieInx.PInx(); };
    FlwNode       * Nd()          { return m_pNd; };
    int             NIOFBs()      { return m_pNd->NIOFBs(m_iIONo); };
    FlwBlk        * IOFB(int FE)  { return m_pNd->IOFB(m_iIONo, FE); };
    FlwBlk        * IOFBJoin()    { return m_pNd->IOFBJoin(m_iIONo); };
    FlwBlk        * IOFBFlng()    { return m_pNd->IOFBFlng(m_iIONo); };
    double          Datum()       { return m_pNd->IODatum_Term(m_iIONo); };
    int             NIOPBs()      { return m_pNd->NIOPBs(m_iIONo); };
    CPressBlk     * IOPB(int FE)  { return &m_pNd->IOPB(m_iIONo, FE); };
    CPressBlk     * IOPBFlng()    { return &m_pNd->IOPB_Flng(m_iIONo); };

    FlwBlk        * LinkFB()      { return &m_pNd->m_Links[m_iLnkNo].m_FBs[0]; };
    CLinkRec      & Link()        { return m_pNd->m_Links[m_iLnkNo]; };

  public:
    FlwNode       * m_pNd;
    int             m_iIONo;
    int             m_iLnkNo;
    FlwBlk        * m_pFBJn;
    CJoinRec      * m_pJoin;
  };

//----------------------------------------------------------------------------

enum eGrpFlwIndex { GFI_Null, GFI_IO, GFI_Lnk, GFI_Join};

class CGrpFlwIndex : public CFlwBlkIndex
  {
  friend class CGrpFlwBlk;
  friend class CGrpFlwBlkSeq;
  public:

    CGrpFlwIndex()
      {
      m_eType     = GFI_Null;
      m_pJoin     = NULL;
      m_pFB       = NULL;
      m_pSrcPB    = NULL;
      m_pDstPB    = NULL;
      m_bAtStart  = false;
      m_bAtEnd    = false;
      };

    CGrpFlwIndex(eGrpFlwIndex eType, FlwNode * pNd, int iSgn, int iIONo, int iFENo, int iLnkNo,
                 CJoinRec * pJoin, FlwBlk * pFB, CPressBlk * pSrcPB, CPressBlk * pDstPB) :
     CFlwBlkIndex(pNd, iSgn, iIONo, iFENo, iLnkNo)
      {
      m_eType     = eType;
      m_pJoin     = pJoin;
      m_pFB       = pFB;
      m_pSrcPB    = pSrcPB;
      m_pDstPB    = pDstPB;
      m_bAtStart  = false;
      m_bAtEnd    = false;
      };

    FlwNode       & getNd()               { return *m_pNd; };
    FlwBlk        & getFB()               { return *m_pFB; };
    CJoinRec       & getJoin()             { return *m_pJoin; };
    CPressBlk     & SrcPB(flag GoFwd)     { return GoFwd ? *m_pSrcPB : *m_pDstPB; };
    CPressBlk     & DstPB(flag GoFwd)     { return !GoFwd ? *m_pSrcPB : *m_pDstPB; };
    CLinkRec      & getLink()             { return m_pNd->m_Links[m_iLnkNo]; };
    bool          & AtStart(flag GoFwd)   { return GoFwd ? m_bAtStart : m_bAtEnd; };
    bool          & AtEnd(flag GoFwd)     { return !GoFwd ? m_bAtStart : m_bAtEnd; };

    __declspec(property(get=getNd))       FlwNode   & Nd; 
    __declspec(property(get=getFB))       FlwBlk    & FB; 
    __declspec(property(get=getJoin))     CJoinRec   & Join; 
    __declspec(property(get=getLink))     CLinkRec  & Link; 

  public:
    eGrpFlwIndex    m_eType;
    CJoinRec      * m_pJoin;
    FlwBlk        * m_pFB;
    CPressBlk     * m_pSrcPB;
    CPressBlk     * m_pDstPB;
    bool            m_bAtStart;
    bool            m_bAtEnd;

  DEFINE_SPARES(CGrpFlwIndex);
  };

class CGrpFlwBlkSeq : public CArray<CGrpFlwIndex*, CGrpFlwIndex*> {};
//typedef CSCDPtrListIterBD<CGrpFlwBlkSeq, CGrpFlwIndex*> CGrpFlwSeqIter;

//----------------------------------------------------------------------------

const long MaxRegHistory=5;
class CGrpFlwRegBlk
  {
  friend class CFlwRegBlk;
  friend class FlwBlk;
  friend class CGrpFlwBlk;
  friend class CRegSolver;
  friend class CSubNetBlk;
  friend class CNetBlk;

  public:
    void SetFlwRegulator(double R);
    byte           What() { return m_iWhat; }; 

    double GetMeasValue();

  protected:
    byte           m_iWhat; 
    FlwNode       *m_pNd;
    FlwBlk        *m_pFB;
    CFlwRegBlk    *m_pFRB;
    double         m_dMeasValue;
    double         m_dCtrlCur;
    flag           m_fSetCI18;

    long           m_nHistory;
    double         m_daError[MaxRegHistory];
    double         m_daCtrl[MaxRegHistory];
    double         m_daGain[MaxRegHistory];

  };

//----------------------------------------------------------------------------

class CGrpFlwBlk
  {
  DEFINE_SPARES(CGrpFlwBlk)

  public:

    CGrpFlwBlk();
    virtual ~CGrpFlwBlk();

    char *         FwdTag();
    char *         RevTag();
    char *         Tag(double Qm) { return Qm>=0 ? FwdTag() : RevTag(); };

    void           AttachFB(FlwBlk *pFB, CNetBlk & NI, CSubNetBlk & SNI);
    void           SetNet(CNetBlk *pNet, CSubNetBlk * pSubNet);
    void           SetSrc(CJoinRec * pJ, int ConnNo);
    void           SetDst(CJoinRec * pJ, int ConnNo);
    void           Reverse();

    FlwNode      * RootNd()       { return m_Lnks.GetCount()>0 ? m_Lnks.GetHead()->pNd : m_Src.m_pNd; };
    flag           RootIsLnk()    { return m_Lnks.GetCount()>0; };

    flag           VFlowAllowed(bool FullMode)     { return FullMode ? (m_iFlwPh_Act&FPh_Gasses)!=0 : false; };
    flag           LFlowAllowed(bool FullMode)     { return FullMode ? (m_iFlwPh_Act&FPh_Liquid)!=0 : true; };
    //void           TestWhichFlows(CFlwBlkBase & FB);
    void           InitFlowPhases();
    bool           TestSetFlowPhases(bool FullFlow);
    void           SetFlowPhases(byte New);


    int          & EqnOne()       { return m_LnkInx.iEqnOne; };
    int          & EqnCnt()       { return m_LnkInx.nEqnCnt; };

    int            PSrcInx()      { return m_Src.m_pJoin->PInx(); };
    int            PDstInx()      { return m_Dst.m_pJoin->PInx(); };
    int            PFndInx()      { return m_iPFndInx; };

    int          & LQVInx()       { return m_LnkInx.iQVInInx; }
    int          & LQLInx()       { return m_LnkInx.iQLInInx; }
    int          & LQInx(int Lp)  { return Lp==0 ? m_LnkInx.iQVInInx : m_LnkInx.iQLInInx; }

    double         VResAdj()      { return m_dVResAdj; };
    double         LResAdj()      { return m_dLResAdj; };
    double         ResAdj(int Lp) { return Lp==0 ? m_dVResAdj : m_dLResAdj; };

    double         VPrevQm()      { return m_dVPrevQm; };
    double         LPrevQm()      { return m_dLPrevQm; };
    double         PrevQm(int Lp) { return Lp==0 ? m_dVPrevQm : m_dLPrevQm; };
    void           SetPrevQm(int Lp, double Q) { if (Lp==0) m_dVPrevQm=Q; else m_dLPrevQm=Q; };

    void           Clear(CNetBlk* pNet_, CFullNetBlk *pFNB_);
    void           AddJoin(CJoinRec * pJ, flag AtHead=False);

    void           InitFBSequence();
    void           Traverse(pchar dbgwhere);
    void           TraverseGetMode(flag GoFwd, CGrpFlwBlkSD & Src, CGrpFlwBlkSD & Dst, CSpPropInfo *pNetProps, pchar dbgwhere);
    //void           TraverseSetQm(double Qm, flag GoFwd, CGrpFlwBlkSD & Src, CGrpFlwBlkSD & Dst, CSpPropInfo *pNetProps, pchar dbgwhere);
    void           TraversePress(eScdFlwEqnTasks Task, double Qm, flag GoFwd, CGrpFlwBlkSD & Src, CGrpFlwBlkSD & Dst, CSpPropInfo *pNetProps, pchar dbgwhere);
    void           TraverseDbgHeader(CSpPropInfo *pNetProps);
    void           DbgDumpPBlks(flag GoFwd, bool WithPress);
    void           SetLinkQmEstAndQm(double Qm_);
    void           SetLinkQmEst(double Qm_);
    double         GetLinkQm();
    double         GetLinkQmEst();

    void           ConvergeChokePt(int MaxLoops, flag LastOuterLoop);
    void           ConvergeQm(int MaxLoops, flag FirstOuterLoop, flag LastOuterLoop);
    void           ConvergeRegulator(int MaxLoops, flag FirstOuterLoop, flag LastOuterLoop);
    double         RequiredQm() { return m_fFixedXfer ? m_QmXfCap : m_QmRqd;};

    //void           Set_VPhaseFrac(double F) { m_dVPhaseFrac=F; };
    //double         VPhaseFrac() { return m_dVPhaseFrac; };
    flag           TestAndSolvePartialStaticHead(int StartDirn);


  public:

    CSubNetBlk*    m_pSubNet;
    CNetBlk*       m_pNet;
    CFullNetBlk*   m_pFNB;

    CGrpFlwBlkSD   m_Src, m_Dst;
    CJoinPtrList   m_Lnks;
    CGrpFlwBlkSeq  m_FBSeq;

    Strng          m_FTag, m_RTag;

    byte           m_iPropsSelected;

    sint           m_PassDirn;
    double         m_QmIn;
    double         m_QmEstIn;
    double         m_QmEstInMem;
    double         m_QmGood;
    double         m_Src_P;
    double         m_Dst_P;
    double         m_End_P;
    double         m_P_Applied;
    double         m_P_Mean;
    double         m_P_Here;
    double         m_P_HereLnk;
    double         m_P_HereSafe;
    double         m_P_Lowest;
    double         m_P_Err;
    double         m_Sum_dP;
    double         m_Sum_adPqdQ;
    double         m_Sum_dPbdQ;
//    double         Sum_ResQ;
    double         m_Sum_ResT;
    double         m_Sum_B;
    double         m_Sum_ROp;
    double         m_Sum_BOp;

    double         m_Sum_RPrev[2];
    double         m_Sum_BPrev[2];

    double         m_Sum_BSmth;
    double         m_Sum_RSmth;
    double         m_Sum_ROpSmth;
    double         m_Sum_BOpSmth;
    double         m_BstDamping;
    

    struct CXBst
      {
      FlwBlk *     m_pFB;
      int          m_dFB;
      }            m_XBst;

    CArray <FlwBlk*, FlwBlk*> m_FBs;

    CChokeAtRec    m_ChokeAt;
    CChokeInfoRec  m_ChokeInfo;

    double         m_CnvFilter;

    byte           m_nFixedVlv;
    byte           m_nFixedRqd;
    
    flag           m_fGFTolsOK,
                   m_fGFNetTolsOK,
                   m_fGFRegTolOK,
                   m_fGFHoldRegCvg,
                   m_fDerivsBad,
                   m_fFunctOfPress,
                   m_fEvalFwdOnly,
                   m_fEvalRevOnly,
                   m_fMomentumValid,
                   m_fHasDegFree,
                   m_fFixed,
                   m_fFixedXfer,
                   m_fFixedExternal,
                   m_fInXferNet,
                   m_fUnDefined,
                   m_fTopologyChanged,
                   m_fChoked,
                   m_fSelfLoop,
                   m_fUDFShut,
                   m_fTraverseFwd,
                   //m_fVFlowAllowed,
                   //m_fLFlowAllowed,
                   m_fIsBad,
                   m_fSrcIsEdge,
                   m_fDstIsEdge,
                   m_fFastConvergeOK,
                   m_fHoldFastConverge,
                   m_fInGrpList,
                   m_fHasDbgBrk;

    byte           m_iFlwPh_Act;

    flag           m_fSetCI5:1,
                   m_fSetCI6:1,
                   m_fSetCI7:1,
                   m_fSetCI8:1,
                   m_fSetCI13:1,
                   m_fSetCI14:1,
//                   fSetCI18:1,
                   m_fSetCI21:1,
                   m_fSetCI22:1;

    CGrpFlwRegBlk  m_Regulator;

//                   fSetVolFrac:1;
 
//    double         dVVolFrac;
//    double         dVVolFracBal;
    CSpPropInfo  * m_pBalProps; 

    double         m_QmRqd;
    double         m_QmReg;
    double         m_QmXfCap;

    GQmRatioRec    m_QmRatio;

    int            m_PressDir;
    int            m_iInitialFlwDirn;
    flag           m_fFindStaticHead;

    GrpFlwBlkHiImp m_HiImp;

    SpConduit *    m_pTwoPhCd;
    flag           m_fTwoPhase;

    LnkInxBlk      m_LnkInx;
    int            m_iPFndInx;
    int            m_NdInx;
    int            m_SigPVInEqn;
    int            m_SigPLInEqn;

    //double         m_dVPhaseFrac;
    double         m_dVResAdj, m_dLResAdj;
    double         m_dVPrevQm, m_dLPrevQm;
    double         m_dVResMtx, m_dLResMtx;

    #if DISTERRORINTRAVERSE
    CArray<double, double> m_WrkDP;
    #endif
    
    #ifdef _DEBUG
    dword          m_dwIterNo;
    long           m_nTraverse;
    long           m_nMaxTraverse;
    float          m_nAvgTraverse;
    #endif
  };

typedef CGrpFlwBlk *CGrpFlwBlkPtr;

// ===========================================================================

class CGrpFlwList : public CSCDList<CGrpFlwBlk *, CGrpFlwBlk *> {};
typedef CSCDPtrListIter<CGrpFlwList, CGrpFlwBlk *> GrpFlwIter;
typedef CLockedListIter<CGrpFlwList, CGrpFlwBlkPtr&> CLkGrpFlwIter;

// ===========================================================================

struct CJoinGrpPtr
  {
  public:
    flag      IsGrp()  { return pG!=NULL; };
    flag      IsJoin() { return pJ!=NULL; };
    CGrpFlwBlk *Grp()   { return pG; };
    CJoinRec   *Join()  { return pJ; };

  public:
    CGrpFlwBlk * pG;
    CJoinRec * pJ;
  };

class CJoinGrpList : public CSCDList<CJoinGrpPtr , CJoinGrpPtr & >
  {
  public :
    void AddJoin(CJoinRec * pJ) { CJoinGrpPtr JG; JG.pG=NULL; JG.pJ=pJ; AddTail(JG);};
    void AddGrp(CGrpFlwBlk * pG) { CJoinGrpPtr JG; JG.pJ=NULL; JG.pG=pG; AddTail(JG);};
  };

typedef CSCDListIter<CJoinGrpList, CJoinGrpPtr *> CJoinGrpIter;

// ===========================================================================

inline flag ConnHasGFB(CJoinRec * pJ, int c) { return pJ->GFB(c) && pJ->GFB_Rmt(c); };
inline flag ConnPlaced(CJoinRec * pJ, int c) { return pJ->Placed(c) && pJ->Placed_Rmt(c); };
inline flag ConnIsXfer(CJoinRec * pJ, int c) { return pJ->IsIO(c) ? pJ->IOFB(c,0)->GetXfer() || pJ->IOFB_Rmt(c,0)->GetXfer() : pJ->LinkFB(c)->IsXfer(); };
inline flag ConnIsFixed(CJoinRec * pJ, int c) { return pJ->IsIO(c) ? pJ->IOFB(c,0)->GetFixed() || pJ->IOFB_Rmt(c,0)->GetFixed() : pJ->LinkFB(c)->IsFixed(); };
inline flag ConnSameSubNet(CJoinRec * pJ, int c) { return (pJ->GFB(c)->m_pSubNet == pJ->SubNet()); };
inline flag FBIsXfer(CJoinRec * pJ, int c) { return pJ->IsIO(c) ? pJ->IOFB(c,0)->GetXfer() : pJ->LinkFB(c)->GetXfer(); };
inline flag FBIsFixed(CJoinRec * pJ, int c) { return pJ->IsIO(c) ? pJ->IOFB(c,0)->GetFixed() : pJ->LinkFB(c)->GetFixed(); };

// ===========================================================================

class CJoinGFBPtr
  {
  public:
    flag            IsJoin()          { return m_pJoin!=NULL; };
    flag            IsFB()            { return m_pFB!=NULL;   };
    flag            IsGFB()           { return m_pGFB!=NULL;  };

    FlwNode       * Nd()              { return m_pNd;         };
    CJoinRec      * Join()            { return m_pJoin;       };
    FlwBlk        * FB()              { return m_pFB;         };
    CGrpFlwBlk    * GFB()             { return m_pGFB;        };

    flag            IsFixed()         { return m_pFB ? m_pFB->GetFixed() : false; };
    flag            Forward()         { return m_fForward; };
    flag            StartTree()       { return m_fStartTree; };
    
    char *          Tag(Strng &S)
      {
      if (IsJoin())
        {
        S="Jn:";
        S+=Nd()->FullObjTag();
        }
      else if (IsGFB())
        S=m_fForward ? GFB()->FwdTag() : GFB()->RevTag();
      else if (m_pNd)
        {
        S="Nd:";
        S+=m_pNd->FullObjTag();
        }
      else
        S="FB ???";
      return S();
      };

  public:
    FlwNode     * m_pNd;
    CJoinRec    * m_pJoin;
    FlwBlk      * m_pFB;
    CGrpFlwBlk  * m_pGFB;
    int           m_iFxd;
    flag          m_fForward, m_fStartTree;
  };

class CJoinGFBList : public CSCDList<CJoinGFBPtr , CJoinGFBPtr & >
  {
  public :
    void AddFB(FlwBlk * pFB, FlwNode * pNd, flag Forward, flag StartTree, int iSrcPBOrder, int iFBOrder, int iDstPBOrder);
    void AddGFB(CGrpFlwBlk * pGFB, flag Forward, flag StartTree);
    void AddJoin(CJoinRec * pJ, int iPBOrder);
  };

typedef CSCDListIter<CJoinGFBList, CJoinGFBPtr *> CJoinGFBIter;

// ===========================================================================

typedef int BldNetInx[JTyp_NoOf];

// ===========================================================================

struct ZListItem
  {
  CJoinRec     * pJn;
  FlwNode     * pNd;
  int           iIo;
  int           iCn;
  double        dZ;
  double        dZRmt;
  };

// ===========================================================================

class CSubNetBlk
  {
  friend class CNetBlk;

  DEFINE_SPARES(CSubNetBlk)

  public:
    CSubNetBlk();
    ~CSubNetBlk();
    void           Clear(int SubNetNo_, CNetBlk* pNet_, CFullNetBlk *pFNB_);

    void           FindFxdFBPEstRqd(CJoinGFBPtr & JG);
    void           FindFxdJoinPEstRqd(CJoinGFBPtr & JG);
    void           SolveFxdFBP(CJoinGFBPtr & JG);
    void           SolveFxdJoinP(CJoinGFBPtr & JG);
    void           SolveFxdPs();
    void           DumpFxdConnPs();

    void           LimitFxdGFBQ(CJoinGFBPtr & JG, long & Changes);
    void           LimitFxdJoinQ(CJoinGFBPtr & JG, long & Changes);
    void           SolveFxdGFBQ(CJoinGFBPtr & JG);
    void           SolveFxdJoinQ(CJoinGFBPtr & JG);
    void           SumFxdLnkQ(CJoinGFBPtr & JG);
    void           SolveFxdQs();
    
    void           SolveXferGFBQ(CJoinGFBPtr & JG);
    void           SolveXferQs();

    void           SolveTiePressures();

//    flag           LoadTiePress();
    flag           Measure(int NetCnvLoop);
    void           Traverse();
    flag           Converge(int NetCnvLoop);
    flag           SolveUnDefined();

    char *         TIndicesStr(char * Buff, CJoinRec * pJ);
    char *         LIndicesStr(char * Buff, CGrpFlwBlk * pG);

    void           SetFullFlow(flag On) { m_bFullFlow=On!=0; m_Lp0=On?0:1; m_LpN=2; }

    flag           DoJoinFlow(CJoinRec & J, int i)    
      { 
      if (m_bFullFlow)
        return (i==0) && J.VFlowAllowed(m_bFullFlow) || (i==1) && J.LFlowAllowed(m_bFullFlow); 
      else
        return (i==1);
      };
    flag           DoJoinFlow(CGrpFlwBlk & G, int i)  
      { 
      if (m_bFullFlow)
        return (i==0) && G.VFlowAllowed(m_bFullFlow) || (i==1) && G.LFlowAllowed(m_bFullFlow); 
      else
        return (i==1);
      };
    flag           DoJoinOneFlow(CGrpFlwBlk & G)      
      {
      if (m_bFullFlow)
        return !G.VFlowAllowed(m_bFullFlow) || !G.LFlowAllowed(m_bFullFlow); 
      else
        return true;
      };


    void           SolveIsoNode();

    void           CalcIndicesFull1(CJoinRec * pSrc, CJoinRec * p, CGrpFlwBlk * pG);
    void           CalcIndicesFull2(int &NdInx, CJoinRec * pSrc,
                                    CJoinRec * p, CGrpFlwBlk * pG,
                                    BldNetInx &PQInx, int &EqnOne);

    void           CalcIndicesFull();
    flag           BuildMatrixFull(long NetCnvLoop);
    flag           SolveMatrixFull(long NetCnvLoop);
    flag           ExtractMatrixFull(long NetCnvLoop);

    //void           CalcIndicesFlow1(CJoinRec * pSrc, CJoinRec * p, CGrpFlwBlk * pG);
    //void           CalcIndicesFlow2(int &NdInx, CJoinRec * pSrc,
    //                                CJoinRec * p, CGrpFlwBlk * pG,
    //                                BldNetInx &PQInx, int &EqnOne);

    //void           CalcIndicesFlow();
    //flag           BuildMatrixFlow(long NetCnvLoop);
    //flag           SolveMatrixFlow(long NetCnvLoop);
    //flag           ExtractMatrixFlow(long NetCnvLoop);
 
    void           CheckJoinFlows(CJoinRec * pTie);
    flag           SolveNetFlows();
    flag           Solve();
    flag           SubNetTopologyOK() {return !fTopologyChanged;};

    void           DoAddToZSeq(FlwNode  *Nd, int IONo, dword Flags);
    void           DoAddToZSeq(CJoinRec  *Jn, dword Flags);
    void           DoAddToZSeq(FlwBlk   *FB, FlwNode  *Nd, int IONo, int FENo, dword Flags);
    void           AddToZSeq(CJoinRec *pJn, int iCn, FlwNode *pNd, int iIo);
    void           AddToZList(ZListItem & ZI);
    flag           FindZSeq();
    flag           DoZSeq();

//    void           DoAddToFlwSeq(FlwNode  *Nd, int IONo);
//    void           DoAddToFlwSeq(FlwBlk   *FB, FlwNode  *Nd, int IONo, int FENo);
//    void           AddToFlwSeq(CJoinRec *p, CJoinRec *pFrom, FlwNode *pSrc, int iIOSrc);
//    flag           FindFlwSeq();
//    flag           DoFlwSeq();

    void           InitUDFs();
    int            TestUDFsChange();
    flag           CheckForErrors();
    void           SetDBGFlags();

    void           FITGrpMeasure(CLkGrpFlwIter & Iter);
    void           FITGrpTraverse(CLkGrpFlwIter & Iter);
    void           FITGrpConverge(CLkGrpFlwIter & Iter);

  protected :
    void           ShowSubNet(int dbgShowConns);
    void           ShowSubNetsT(CJoinRec * p, int dbgShowConns);
    void           ShowSubNetsJ(CJoinRec * p, int dbgShowConns);
    void           ShowSubNetsL(CGrpFlwBlk * g, int dbgShowConns);

  public:
    CJoinPtrList    Joins;
    CJoinPtrList    Ties;
    CGrpFlwList     m_Grps;
    CJoinGFBList    m_FxdPList;
    CJoinGFBList    m_FxdQList;
    CJoinGrpList    m_CalcIndexSeq;

    CGrpFlwList     m_RegGrps;
    
    FlwBlkList     m_UDFFBs;

    CJoinRec *     m_pFirstJoin;

    int            m_SubNetNo;
    int            iNetCnvLoop;
    int            m_Lp0, m_LpN;

    bool           m_bFullFlow; 

    flag           fSolved,
                   fNetTolsOK,
                   fMustAnalyse,
                   m_fLonely,
                   m_fUnDefined,
                   m_fIsolatedNet,
                   m_fIsolatedNode,
                   m_fFixed,
                   m_fXfer,
                   m_fXferTies,
                   m_fNetWrk,
                   fAnalyseOK,
                   fMomentumValid,
                   fHasDbgBrk;

    flag           fIndexCalculationReqd;
    flag           fTopologyChanged;

    CNetBlk*        pNet;
    CFullNetBlk*    pFNB;
    CSparseSolver  m_SS;
    flag           fClearSS;
    int            iMtxOrder;
  };

// ---------------------------------------------------------------------------

class CSPACSubNetBlk : public CSmartPtrAllocate<CSubNetBlk> {};
class SubNetArray : public CArray<CSPACSubNetBlk, CSPACSubNetBlk&> {};

// ===========================================================================

class DllImportExport TstFxdLnkQRec
  {
  public:
    int            nNonZeroFixed;
    int            nFixed;
    int            nXfer;
    flag           fFxdJoin;
    flag           fXferJoin;
    int            iDepth;

    TstFxdLnkQRec()
      {
      nNonZeroFixed=0;
      nFixed=0;
      nXfer=0;
      fFxdJoin=0;
      fXferJoin=0;
      iDepth=0;
      };
  };

// ===========================================================================
//-------------------------------------------------------------------------

enum Job_Cmds {
  Job_FixNet,
  Job_SolveFlwsFxd,
  Job_SolveFlwsFxdDone,
  Job_SolveFlws,
  Job_SolveFlwsDone,
  Job_SolvePs,
  Job_CheckDone,
//  Job_Done,
  Job_CmdLast
  };

extern char* Job_CmdStr[Job_CmdLast+1];

struct CJobItem
  {
  int iWhat, iNetNo, iSubNetNo;
  CJobItem()
    {
    iWhat    =0;
    iNetNo   =0;
    iSubNetNo=0;
    }
  CJobItem(int What, int NetNo, int SubNetNo)
    {
    iWhat    =What;
    iNetNo   =NetNo;
    iSubNetNo=SubNetNo;
    }
  };

class CJobList : public CList<CJobItem, CJobItem&> {};
class CJobListIter : public CLockedListIter<CJobList, CJobItem&> 
  {
  public:
  void AddJob(int What, int Net, int SubNet)
    {
    CJobItem A(What, Net, SubNet);
    AddTail(A);
    }
  };


//-------------------------------------------------------------------------
// ===========================================================================

enum GFBlkPos   { GFB_Start, GFB_Cont };
enum LFxdPos    { LFP_FixedHere, LFP_Elsewhere };
enum ACPosition { ACP_Head, ACP_Tail };

class CNetBlk
  {
  DEFINE_SPARES(CNetBlk)

  public:
    CFlwNodeList    Nds;
    CJoinPtrList    Joins;

    bool           bFixBusy;

    int            NetNo;
    CFullNetBlk*    pFNB;
    flag           fSolved;
    flag           fTopologyChanged;
    flag           dbgSubNetStruct;
    int            NSubNets;
    SubNetArray    SubNet;

    int            nOverSpecChgs;
    int            iSolvePass;
    long           nJobsToDo;

    CNetBlk();
    ~CNetBlk();
    void           Clear(int NetNo, CFullNetBlk *pFNB_);
    void           PrepareSubNet(int SubNetNo);

    CGrpFlwBlk *    AllocGFB();
    void           FreeGFB(CGrpFlwBlk * p);

    flag           AddConn2GFB(ACPosition Pos, JoinTypes Typ, CGrpFlwBlkPtr & pGFB, CJoinRec * pSrcJoin, int SrcConnNo);
    flag           FindConnGFB(JoinTypes Typ, CGrpFlwBlkPtr & pGFB, CJoinRec * pSrcJoin, int SrcConnNo, bool ReverseIfReqd=false);
    flag           MassConserveTie(CJoinRec *p) { return p->fIsTie && !p->fIsImpNd && !p->fIsAccLnk; };


    void           FindFixedConnsFromJoin(flag ForXfer, bool Fwd, CSubNetBlk &SNI, CJoinRec * p, int ConnNo, int &FxdOrder, CGrpFlwBlk *pGFB);
    void           FindFixedConns(flag ForXfer, bool Fwd, CSubNetBlk &SNI, CJoinRec * pSrc, int SrcConnNo, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB, LFxdPos Where, int &FxdOrder);
    void           FindFxdQs(flag ForXfer, CSubNetBlk &SNI, CJoinRec * p, int Conn, int &FxdOrder);
    flag           FindXferTies(CSubNetBlk &SNI, CJoinRec * & pJ);
    void           FindMatrixConns(CSubNetBlk &SNI, CJoinRec * pSrc, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB);

    flag           FindNetLinks(CSubNetBlk &SNI, CJoinRec * &pJ);
    void           FindLonelyConns(ACPosition Pos, CSubNetBlk &SNI, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB);
    flag           FindLonelyConns(CSubNetBlk &SNI, CJoinRec * p, int ConnNo);
    //void           FindUnDefConns(ACPosition Pos, CSubNetBlk &SNI, CJoinRec * p, int ConnNo, CGrpFlwBlk * pGFB);
    //flag           FindUnDefConns(CSubNetBlk &SNI, CJoinRec * p, int ConnNo);

    void           BuildGFB(CGrpFlwList & Grps, CJoinRec * pStart, int ConnNo);
    void           BuildGFBs(CGrpFlwList & Grps);
    void           DumpGFBSummary(CGrpFlwList & Grps);
    void           SetUpGFBs(CGrpFlwList & Grps);
    void           ConfigureGFBs(CSubNetBlk & SNI);
    void           ConfigureTies(CSubNetBlk & SNI);
    void           BldSubNets();
    void           UnBldSubNets();
    void           FixNetTopology();
    void           SetDBGFlags();
    flag           NetTopologyOK() {return !fTopologyChanged;};
    void           SetQmEstAndQm();
    void           SaveQmEst();
    void           RestoreQmEst();
    void           SaveQmGood();
    void           SavePGood();
    void           RestorePGood();
    void           DumpJoinList();

    void           Solve();
    flag           CheckForErrors();

    void           SolveStart();
    void           JobFixNet();
    void           JobSolveFlwsFxd(int Sub);
    void           JobSolveFlwsFxdDone();
    void           JobSolveFlws(int Sub);
    void           JobSolveFlwsDone();
    void           JobSolvePs(int Sub);
    void           JobCheckDone();
    CJobListIter   & JobListIter();

  protected:
    void           ShowSubNets(int N=-1, int dbgShowConns=false);

  };

// ---------------------------------------------------------------------------

typedef CArray<CNetBlk, CNetBlk&> NetArray;

// ===========================================================================

const int TLO_Tree          = 0x0001;
const int TLO_Alpha         = 0x0002;

enum CnvMethod {CnvM_Only};

extern CGlblStopWatch gs_swTotalExecTicks;
extern CGlblStopWatch gs_swMeasureTicks;
extern CGlblStopWatch gs_swConvergeTicks;
extern CGlblStopWatch gs_swSolveTicks;
extern CGlblStopWatch gs_swLoadSurgeTicks;
extern CGlblStopWatch gs_swCalcIndicesTicks;
extern CGlblStopWatch gs_swMtxBuildTicks;
extern CGlblStopWatch gs_swMtxSolveTicks;
extern CGlblStopWatch gs_swMtxExtractTicks;
extern CGlblStopWatch gs_swChkJoinsTicks;
extern CGlblStopWatch gs_swEvalJoinQTicks;
extern CGlblStopWatch gs_swEvalJoinPTicks;
extern CGlblStopWatch gs_swSolveFxdFlwQTicks;
extern CGlblStopWatch gs_swSolveFxdFlwPTicks;
extern CGlblStopWatch gs_swFixTopoTicks;
extern CGlblStopWatch gs_swOverHeadTicks;
extern CGlblStopWatch gs_swTotalDerivative;

class CFullNetBlk
  {
  DEFINE_SPARES(CFullNetBlk)

  friend class CSubNetBlk;
  friend class CNetBlk;
  friend class CGrpFlwBlk;
  friend class FlwSlvDyn;
  friend class CFlwSolver;

  public:

  private:
    long            TotalSolveCnt, MaxSolveCnt;
                    
    CFlwNodeList&    AllNds;
    CFlwNodeList     Nds;
                    
    int             NNets;
    NetArray        Nets;
                    
    //flag            fStopReqd;
                    
    #ifdef _DEBUG   
    dword           dwIterNo;
    #endif
    
    double          m_dNetMaxResRatio;

  public:
    double          m_NetDeRating;
    long            m_lNetCnvCnt;
    long            m_lMaxUDFIters;
    flag            m_fAllowFastConverge;
    long            m_lResRunAvgStart;
    long            m_lResRunAvgCount;

    CToleranceBlock m_LnkEPS;
    CToleranceBlock m_QmFndEPS;
    CToleranceBlock m_VVolFndEPS;
    CToleranceBlock m_RegSolveEPS;

  public:
    CFullNetBlk(CFlwNodeList& AllNdListIn);
    virtual ~CFullNetBlk();

    flag           Initialise(/*CToleranceBlock & m_Tol, double Net_DeRating_*/);
    //flag           Initialise(double Lnk_Eps_A_, double Lnk_Eps_R_, double Net_DeRating_);

    void           CreateSolutionBlks(int NRqdThreads);
    void           ClearSolutionBlks();

    void           FollowIOs(FlwNode *p, int NetIn, FlwNode *&Start, FlwNode *&End);
    flag           MakeNetNodeList();

    flag           ProblemOK() {return Nets.GetSize()>0;};
    void           Solve(dword IterNo);
    int            CheckForErrors();
    //flag           StopReqd() { return fStopReqd; };

    void           SetQmEstAndQm();
    void           SaveQmEst();
    void           RestoreQmEst();
    void           SaveQmGood();
    void           AdjustQm();
    void           SavePGood();
    void           RestorePGood();

    void           ShowFlowTree(FlwNode * Root, pchar StartTag = NULL);
    void           SortTagList(FlwNode * Root, Strng_List &TagList, Strng_List &GrfList, int Ord, pchar StartTag = NULL);
    void           DumpSolveStats(FILE *f);
    void           DumpFlows(FILE *f, double Time_, double dTime_, pchar Desc_);

  private:

    flag           BuildProblem();
    void           ShowFlowTree1(int lvl, FlwNode *pSrc, FlwNode *p);
    void           SortTagTree(Strng_List &OldList, Strng_List &NewList, Strng_List &GrfList, FlwNode *pStart = NULL);


  private:
    CJobList       xJobList;
  public:
    CJobListIter   JobListIter;
    void           FSTFlwSolve();

  // Stats
    
    long GetNumNets();
    long GetNumSubNets(long iNet);
    long GetNumNodes(long iNet);
    long GetNumFxdLnks(long iNet);
    long GetNumLoneLnks(long iNet);
    long GetNumNetLnks(long iNet);
    //void ClearSWS();

    //double GetConvergeSecs();
    //double GetMeasureSecs();
    //double GetSolveSecs();
    //double GetLoadSurgeSecs();
    //double GetCalcIndicesSecs();
    //double GetMtxBuildSecs();
    //double GetMtxSolveSecs();
    //double GetMtxExtractSecs();
    //double GetChkJoinsSecs();
    //double GetEvalJoinQSecs();
    //double GetEvalJoinPSecs();
    //double GetSolveFxdFlwQSecs();
    //double GetSolveFxdFlwPSecs();

  };

// ===========================================================================

inline CJobListIter &  CNetBlk::JobListIter() { return pFNB->JobListIter; };

// ===========================================================================

#undef DllImportExport

#endif
