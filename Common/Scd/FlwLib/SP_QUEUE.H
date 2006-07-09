//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SP_QUEUE_H
#define  __SP_QUEUE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif  
#ifndef __FLWEQN_H
  #include "flweqn.h"
#endif  
#ifndef __2D_FN_H
  #include "..\xylib\2d_fn.h"
#endif  

#ifdef __SP_QUEUE_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define ADDHEAD   0
#define ADDTAIL   1
#define ADDBEFORE 2
#define ADDAFTER  3

// ===========================================================================

//_FWDDEF(SpQSection)
//_FWDDEF(SpQIO)
DEFINE_TAGOBJ(SpQueue)
DEFINE_TAGOBJ(CSpPlugFlow)
class SpQIO;

// ===========================================================================
//
//                              Specie Section
//
// ===========================================================================

class DllImportExport SpQSection
  {
  friend class SpQueue;
  friend class CSpPlugFlow;
  friend class SpQSectionList;
  friend class CSpPFSectionList;
    
  public:
    SpQSection();
    ~SpQSection();

    flag          GetOtherData(FilingControlBlock &FCB);
    flag          PutOtherData(FilingControlBlock &FCB);
                  
    double        Start() { return m_dPosition; };
    double        End()   { return m_dPosition + m_dLength; };
    double        Length() { return m_dLength; };
    double        Mass() { return m_pMdl->Mass(som_ALL); };
    double        totHf() { return m_pMdl->totHf(som_ALL); };
    double        totHs() { return m_pMdl->totHs(som_ALL); };
    double        totHz() { return m_pMdl->totHz(som_ALL); };
    double        Temp() { return m_pMdl->Temp(); };
    double        Loading() { return Mass()/GTZ(Length()); };
    void          SetStart(double St) { m_dPosition=St; };
    void          SetLength(double L) { m_dLength=L; };
    void          SetMakeup(SpQueue * pQ, double Frac, double QmRqd, SpConduit * pProd);//pSpillArea pSpill);
    //static void   SetMakeup(SpQIO & Feed, SpQueue * pQ, double Frac, double QmRqd, SpConduit * pProd);
    void          Load(SpQueue * pQ, double Frac, SpConduit * pFeed, CDirectFlwIO * pSpill);//pSpillArea pSpill);
    void          UnLoad(SpQueue * pQ, double Frac, double QmRqd, SpConduit * pProd, CDirectFlwIO * pSpill);//pSpillArea pSpill);
    void          ScaleMass(double Scl) { m_pMdl->ScaleMass(som_ALL, Scl);};
    void          CopyMass(SpQSection * pSct, double Scl);
    void          Spill(SpQueue * pQ, double Frac, CDirectFlwIO *pSpill);
    void          Spill(CSpPlugFlow* pQ, double Frac);

    void          Load(CSpPlugFlow * pPF, double Frac, SpConduit * pFeed);
    void          UnLoad(CSpPlugFlow * pPF, double Frac, double QmRqd, SpConduit * pProd);
    void          Fill(CSpPlugFlow * pPF, double Frac, SpConduit * pFeed, double PressS, double PressE, double VelS, double VelE);

  protected:
    bool           m_bTentative;
    double         m_dLength;
    double         m_dPosition;
    SpModel *      m_pMdl;
    // Added for PlugFlow
    double         m_dDeltaPosS;
    double         m_dDeltaLen;
    //double         dPositionStart;
    double         m_dPressS;
    double         m_dPressE;
    double         m_dVelS;
    double         m_dVelE;
    double         m_dMassScale;
    UINT           m_ID;
    static UINT    m_IDCurrent;
    POSITION       m_ListPos;
  };

class SpQSectionList : public CSCDList <SpQSection *, SpQSection *> 
  {
  public:
    SpQSectionList (SpQueue *pQ) { m_pQ=pQ; };

    SpQSection * Add(double Ss, double Se, byte Where, POSITION AtPos, POSITION &RetPos, SpQSectionList & m_Spares);
    bool         Break(double P, POSITION AtPos, POSITION &LastPos, SpQSectionList & m_Spares);

  protected:
    SpQueue *  m_pQ;
  
  };
typedef CSCDPtrListIterWithPos<SpQSectionList, SpQSection *> SpQSectionIter;

class CSpPFSectionList : public CSCDList <SpQSection *, SpQSection *> 
  {
  public:
    CSpPFSectionList (CSpPlugFlow*pQ) { m_pPF=pQ; };

    SpQSection * Add(double Ss, double Se, byte Where, POSITION AtPos, POSITION &RetPos, CSpPFSectionList & m_Spares);
    bool         Break(double P, POSITION AtPos, POSITION &LastPos, CSpPFSectionList & m_Spares);

  protected:
    CSpPlugFlow * m_pPF;
  };
typedef CSCDPtrListIterWithPos<CSpPFSectionList, SpQSection *> SpPFSectionIter;

// ===========================================================================
//
//                              Specie QIO
//
// ===========================================================================

enum eQIOTypes { QIO_Fixed, QIO_FixedEnd, QIO_Moving, QIO_Distributed };

class DllImportExport SpQIO
  {
  friend class SpQueue;
  friend class CSpPlugFlow;

  public:
    SpQIO();
    ~SpQIO();

    double          Position()                    { return m_dPosition;};
    double          Removal()                     { return m_dRemoval;};
    double          QmEst()                       { return m_dQmEst;};
    double          QmAct()                       { return m_dQmAct;};
    double          Loss()                        { return m_dLoss;};
    flag            FeedLimited()                 { return m_fFeedLimited;};
    void            SetFeedLimited(flag On)       { m_fFeedLimited=On;};
    double          FeedCapFrac()                 { return m_dFeedCapFrac;};
    void            SetFeedCapFrac(double Cap)    { m_dFeedCapFrac=Cap;};
    eQIOTypes       Type()                        { return m_eType; };
    void            SetType(eQIOTypes T)          { m_eType=T; };
    long            Index()                       { return m_lIndex; };
    void            SetIndex(long T)              { m_lIndex=T; };
    double          BinWidth()                    { return m_dBinWidth; };
    void            SetBinWidth(double W)         { m_dBinWidth=W; };
    double          SpillMargin()                 { return m_dSpillMargin; };
    void            SetSpillMargin(double W)      { m_dSpillMargin=W; };
    void            SetPosition(double Position);  //{ if (m_dPosition!=Position) {pQ->bPosChg=1; m_dPosition=Position;}; };
    void            SetSpill(char * SpillTag);     //{ SpillTarget()->SetTag(SpillTag); };
    void            SetRemoval(double Removal)    { m_dRemoval=Range(0.0, Removal, 1.0); };

    void            SetIOMoving(bool On, double Spd=0, double MinPosn=0, double MaxPosn=0);
    void            SetUseBinSizes(bool On);

    CDirectFlwIO  & getSpill()                    { return *m_pSpill; };
    _declspec(property(get=getSpill))             CDirectFlwIO & Spill;

  protected:
    double         m_dPosition;
    double         m_dRemoval;
    double         m_dQmEst;
    double         m_dQmAct;
    double         m_dLoss;
    flag           m_fFeedLimited;
    double         m_dFeedCapFrac;

    bool           m_bIOMoving;
    double         m_dPosMin;
    double         m_dPosMax;
    double         m_dIOSpd;
    double         m_dBeltStart;
    double         m_dBeltEnd;
    
    bool           m_bUseBinSizes;
    double         m_dBinWidth;
    double         m_dSpillMargin;
    // info for conveyor
    eQIOTypes      m_eType;
    long           m_lIndex;

    // this must be a pointer/reference to an instance because CArray is used to 
    // keep a an array of SpQIO's and this does move elements around
    CDirectFlwIO * m_pSpill;//pSpillArea     pSpill;
    SpConduit *    m_pCond;
    SpQueue *      m_pQ;
    //bool           m_bOwnSpill;


  };

typedef CArray <SpQIO, SpQIO&> SpQIOArray;

// ===========================================================================
//
//                              Specie Queue
//
// ===========================================================================

enum eQProfTypes { QPT_None, QPT_FixedPts, QPT_AllSections, QPT_MinSections};

class DllImportExport SpQueue : public SpModelOwner
  {
  friend class SpQSection;
  friend class SpQIO;
  friend class SpQSectionList;

  DEFINE_SPARES(SpQueue);

  public:
    SpQueue(pTagObjClass pClass_, pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    SpQueue(pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach, CDirectFlwIO * pSpill, CDirectFlwIO * pVent, CDirectFlwIO * pAccIn, CDirectFlwIO * pAccOut);
    virtual ~SpQueue();

    void           SetState(eScdMdlStateActs RqdState);

    // Data Members ---------------------------------------------------------
    
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    // Shape Members --------------------------------------------------------
    void           SetLength(double L) { m_dLength=L; };
    double         Length() { return m_dLength;}
    void           SetMaxSpillLoading(double L) { m_dMaxSpillLoading=L; };
    double         MaxSpillLoading() { return m_dMaxSpillLoading; };
    void           SetMinLoading(double L) { m_dMinLoading=L; };
    double         MinLoading() { return m_dMinLoading; };
    double         MinSectLength() { return m_dLength*m_dMinFracSectLen; };

    // Model Members --------------------------------------------------------
    void           CopyModel(SpModel * RqdModel);
    void           ChangeModel(pchar ReqdModelDesc);
    void           ChangeModel(pTagObjClass pRqdModelClass_);
    flag           ModelIs(pTagObjClass pMC);
    
    // State Members --------------------------------------------------------

    int            NFeeds() { return m_Feed.GetSize(); };
    int            NProds() { return m_Prod.GetSize(); };

    void           SetNFeeds(int NFeeds);
    void           SetNProds(int NProds);
    //void           SetSpill(char * SpillTag) { SpillTarget()->SetTarget(SpillTag); };//pSpillArea Spill=NULL) {pSpill=Spill;};
    double         TotalSpiltMass() { return m_dTotSpiltM; };
    double         TotalSpiltHf() { return m_dTotSpiltHf; };
    double         TotalSpiltHs() { return m_dTotSpiltHs; };
    double         TotalSpiltHz() { return m_dTotSpiltHz; };
    void           SetTotalSpilt(double D, double Hf, double Hs, double Hz) 
      { 
      m_dTotSpiltM=D; 
      m_dTotSpiltHf=Hf; 
      m_dTotSpiltHs=Hs; 
      m_dTotSpiltHz=Hz; 
      };

    void           AddFeed()          { SetNFeeds(NFeeds()+1); m_Feed[NFeeds()-1].SetPosition(0.0); };
    void           AddProd()          { SetNProds(NProds()+1); m_Prod[NProds()-1].SetType(QIO_FixedEnd); m_Prod[NProds()-1].SetPosition(Length()); };
    void           DeleteFeed(int No) { m_Feed.RemoveAt(No); }
    void           DeleteProd(int No) { m_Prod.RemoveAt(No); }

    flag           FeedLimited(int FeedNo);
    double         FeedCapacity(int FeedNo);
    void           SetFeed(int FeedNo, double Qm, SpConduit * pFeed);
    void           SetProductQmEst(int ProdNo, double QmEst);//, SpConduit * pFeed);
    void           GetProduct(int ProdNo, /*double Qm,*/ SpConduit * pFeed);
    void           EvalProducts();
    void           SetStepInfo(double dTime, double MaxSpeed, double Speed, int MaxTurnDown);
    void           Advance(double dTime, flag Moved);
    void           Empty();

    void           SetTripper(bool On, double Speed=0.1, double Position=0.0, double PosnMin=0.0, double PosnMax=1e6);

    void           SetProfileLen(int Len=100) { m_fProfileOK=0; m_FnProfile.SetLength(Range(0, Len, 128)); };
    int            ProfileLen() { return m_FnProfile.GetLen(); };
    flag           UpdateProfile();
    double         ProfilePt(int iPrf) { UpdateProfile(); return m_FnProfile.Yp(Range(0, iPrf, (int)m_FnProfile.GetLen()-1)); };
    C2DFn       &  Profile() { return m_FnProfile; };
    void           SetShowProfileType(int ProfTyp) { m_eProfileType=(eQProfTypes)ProfTyp; };
    int            ShowProfileType() { return (int)m_eProfileType; };

    double         TotalMass();
    double         TotalHf();
    double         TotalHs();
    double         TotalHz();
    void           AddTotals(double &Mass, double &Hz, CSysVector * pSysVec=NULL);
    double         AverageLoading();
    double         Loading(double Dist);

    void           DumpProfile(int Where, LPCTSTR Comment, long Indent);

    int            NSections()  { return m_Sections.GetCount(); };
    int            NSpares()    { return m_Spares.GetCount(); };

    CDirectFlwIO & getSpill()   { return *m_pSpill; };
    CDirectFlwIO & getVent()    { return *m_pVent; };
    CDirectFlwIO & getAccIn()   { return *m_pAccIn; };
    CDirectFlwIO & getAccOut()  { return *m_pAccOut; };
    _declspec(property(get=getSpill))   CDirectFlwIO & Spill;
    _declspec(property(get=getVent))    CDirectFlwIO & Vent;
    _declspec(property(get=getAccIn))   CDirectFlwIO & AccIn;
    _declspec(property(get=getAccOut))  CDirectFlwIO & AccOut;

    DEFINE_CI(SpQueue, SpModelOwner, 4);

  public:
    SpQSectionList m_Sections, m_FeedSections, m_Spares;
    SpQIOArray     m_Feed, m_Prod;
  
  protected:
    void           Add2Sections(int FeedNo, SpQSectionList & Sections);
    bool           MergeSections(bool One2Two, SpQSection * pSctOne, SpQSection * pSct2Two);
    void           EstFeed(int FeedNo);
    void           EstProd(int ProdNo);//, int Feed0, int FeedN);
    void           LoadFeed(int FeedNo);
    void           UnLoadProd(int ProdNo);
    double         LenTol() { return 1.0e-6*m_dLength; };

  protected:
    double         m_dLength;
    double         m_dTimeSlice;
    double         m_dMaxSpeed;
    double         m_dSpeed;
    double         m_dMaxSpillLoading;
    int            m_nMaxTurnDown;
    double         m_dMinLoading;

    CDirectFlwIO * m_pSpill;
    CDirectFlwIO * m_pVent;
    CDirectFlwIO * m_pAccIn;
    CDirectFlwIO * m_pAccOut;
    //bool           m_bOwnSpill;
    double         m_dTotSpiltM;
    double         m_dTotSpiltHf;
    double         m_dTotSpiltHs;
    double         m_dTotSpiltHz;

    CLVector       m_FeedOrder, m_ProdOrder;
    flag           m_bPosChg;

    C2DFn          m_FnProfile;
    flag           m_fProfileOK;
    eQProfTypes    m_eProfileType;

    bool           m_TripperOn;
    double         m_dTripSpeed;
    double         m_dTripPosition;
    double         m_dTripPosMin;
    double         m_dTripPosMax;

  public:
    double         m_dMinFracSectLen;
    double         m_dTempTol;
    double         m_dLoadTol;
    double         m_dMFracTol;

  };

// ===========================================================================
//
//                              Specie PFIO
//
// ===========================================================================

//enum ePFIOTypes { PFIO_Fixed, PFIO_Moving, PFIO_Distributed };

class DllImportExport CSpPFIO
  {
  friend class CSpPlugFlow;

  public:
    CSpPFIO();
    ~CSpPFIO();

    double          Position()                    { return m_dPosition;};
    double          QmEst()                       { return m_dQmEst;};
    double          QmAct()                       { return m_dQmAct;};
    long            Index()                       { return m_lIndex; };
    void            SetIndex(long T)              { m_lIndex=T; };
    void            SetPosition(double Position);  //{ if (m_dPosition!=Position) {pQ->bPosChg=1; m_dPosition=Position;}; };

  protected:
    double         m_dPosition;
    //double         m_dPositionStart;
    double         m_dQmEst;
    double         m_dQmAct;
    long           m_lIndex;

    // this must be a pointer/reference to an instance because CArray is used to 
    // keep a an array of CSpPFIO's and this does move elements around
    SpConduit *    m_pCond;
    CSpPlugFlow *  m_pPF;
  };

typedef CArray <CSpPFIO, CSpPFIO&> SpPFIOArray;

// ===========================================================================
//
//                              Specie PlugFlow
//
// ===========================================================================

class DllImportExport CFlowSamplePt
  {
  friend class CSpPlugFlow;
  friend class MN_Lnk;
  friend class Pipe;
  friend class Duct;

  public:
    CFlowSamplePt()
      {
      //m_pLnk=NULL;
      m_pCnd=NULL;
      m_dFracPos=0;
      m_dAbsPos=0;
      m_dVel=0;
      }

  protected:
    double          m_dFracPos;
    double          m_dAbsPos;
    double          m_dVel;
    SpConduit     * m_pCnd;
    //MN_Lnk        * m_pLnk;
  };

class DllImportExport CSpPlugFlow : public SpModelOwner
  {
  friend class SpQSection;
  friend class CSpPFIO;

  DEFINE_SPARES(CSpPlugFlow);

  public:

    CSpPFSectionList m_Sections, m_Spares;
    CSpPFIO         m_IO[2];
  
    CSpPlugFlow(pTagObjClass pClass_, pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    CSpPlugFlow(pchar pTag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CSpPlugFlow();

    void           SetState(eScdMdlStateActs RqdState);

    // Data Members ---------------------------------------------------------
    
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    // Shape Members --------------------------------------------------------
    void           SetLength(double L) 
      { 
      m_dLength=L; 
      m_IO[0].m_dPosition=0;
      m_IO[1].m_dPosition=L;
      };
    double         Length() { return m_dLength;}
    void           SetVolume(double V) { m_dVolume=V; };
    double         Volume() { return m_dVolume;}
    void           Fill(SpConduit & C, PhMask PhaseM, double FillFrac, double PIn, double POut, double VelIn, double VelOut);

    // Model Members --------------------------------------------------------
    void           CopyModel(SpModel * RqdModel);
    void           ChangeModel(pchar ReqdModelDesc);
    void           ChangeModel(pTagObjClass pRqdModelClass_);
    flag           ModelIs(pTagObjClass pMC);

    // State Members --------------------------------------------------------
    void           GetInput(int No, double Qm, SpConduit * pFeed);
    void           SetOutput(int No, double Qm, SpConduit * pProd);
    void           EvalProducts();
    void           Advance(double dTime, double VelStart, double VelEnd, bool LinearPressProfile, double PStart, double PEnd, flag Moved);
    void           Empty();

    double         TotalMass();
    double         AverageLoading();
    double         Loading(double Dist);

    void           Sample(CFlowSamplePt & Pt);
    void           MeasureProperties(CSpPropInfoArray & Props, double StartPos, double EndPos);

    int            NSections() { return m_Sections.GetCount(); };
    int            NSpares() { return m_Spares.GetCount(); };

    DEFINE_CI(CSpPlugFlow, SpModelOwner, 4);

  protected:
    void           LoadIO(int No, double dTime, double VelStart, double VelEnd);
    void           UnLoadIO(int No, double dTime, double VelStart, double VelEnd, double PRqd);
    double         LenTol() { return 1.0e-6*m_dLength; };

  protected:
    double         m_dLength;
    double         m_dVolume;
    double         m_dTimeSlice;
  };

// ===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport 

#endif
