//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SP_REACT_H
#define  __SP_REACT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifndef __SP_DB_H
  #include "sp_db.h"
#endif

#ifndef __SP_CONT_H
  #include "sp_cont.h"
#endif

#ifndef __TKNPARSE_H
  #include "tknparse.h"       
#endif

#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif

#include "blockevalbase.h"

#ifdef __SP_REACT_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

XID xidRCTEnable         = RctXID(0);
XID xidRCTName           = RctXID(1);
XID xidRCTPath           = RctXID(2);
XID xidRCTOn             = RctXID(3);
XID xidEditRCTBtnTxt     = RctXID(4);
XID xidEditRCTBtnDlg     = RctXID(5);
XID xidReloadRCTBtn      = RctXID(6);
XID xidBrowseRCTBtn      = RctXID(7);
XID xidRCTLdState        = RctXID(8);
XID xidRCTLdError        = RctXID(9);
XID xidRCTRunState       = RctXID(10);
XID xidRCTFdDuty         = RctXID(11);
XID xidRCTFdDutyRct      = RctXID(12);
XID xidRCTFdDutySrc      = RctXID(13);
XID xidRCTFdDutySnk      = RctXID(14);
XID xidRCTFdDutySrcSnk   = RctXID(15);
XID xidRCTFdHfErr        = RctXID(16);
XID xidRCTPrDuty         = RctXID(17);
XID xidRCTPrDutyRct      = RctXID(18);
XID xidRCTPrDutySrc      = RctXID(19);
XID xidRCTPrDutySnk      = RctXID(21);
XID xidRCTPrDutySrcSnk   = RctXID(22);
XID xidRCTPrHfErr        = RctXID(23);
XID xidRCTDutyPwr        = RctXID(24);
XID xidRCTDutyHX         = RctXID(25);
XID xidRCTDutyHXPwr      = RctXID(26);
XID xidRCTRefDuty        = RctXID(27);
XID xidRCTRefDutyRct     = RctXID(28);

XID xidRCTZeroDuty       = RctXID(30);
XID xidRCTZeroDutyRct    = RctXID(31);

XID xidRCTdRctTol        = RctXID(32);
XID xidRCTiMaxIter       = RctXID(33);
XID xidRCTdRctTTol       = RctXID(34);
XID xidRCTiMaxTIter      = RctXID(35);
XID xidRCTNumOfRct       = RctXID(36);

const char Comma=',';

//===========================================================================

const char SPR_ErrBuild        = 'B';
const char SPR_ErrMathRuntime  = 'M';
const char SPR_ErrOtherRuntime = 'R';

//=========================== 'Exception handler' ===========================

class DllImportExport RBException
 {
 public :
   enum RCTErrors {noError,Error,noFile, Build};

   int m_Reason;
   RBException(RCTErrors reason=noError) {m_Reason = reason;};
 };

//===========================================================================

//===========================================================================
// base Classes / Forward Defs
class CReaction;
class CRCTTokenFile;
DEFINE_TAGOBJ(CReactionBlock)

//===========================================================================
//
//
//
//===========================================================================

#define DEFINE_REACTION(SS) DEFINE_TAGOBJ(SS)

#define IMPLEMENT_REACTION(Obj, ReactionId, Version, Cat, SDesc, LDesc) \
        IMPLEMENT_TAGOBJ(Obj, CReaction::GroupName, ReactionId, Version, "", "", Cat, SDesc, LDesc)

//---------------------------------------------------------------------------

class DllImportExport CRctSolveBlk
  {
  public:
    flag       fCpsOK;
    flag       fHfsOK;
    //double     xdHs;
    double     m_HOR;
    double     m_HORDiff;
    double     m_Hf_FdT;
    double     m_Hf_PrT;
    double     m_Hs_FdT;
    double     m_Hs_PrT;
    double     m_Hf_RefT;
    double     m_Hf_ZeroT;
    double     m_dHfZeroTAtmP;
    SpVector   dMass;

    CRctSolveBlk() { Clear();};
    void Clear()
      {
      fCpsOK=true;
      fHfsOK=true;
      //xdHs=0;
      m_HOR=0.0;
      m_HORDiff=0.0;
      m_Hf_FdT=0.0;
      m_Hf_PrT=0.0;
      m_Hs_FdT=0.0;
      m_Hs_PrT=0.0;
      m_Hf_RefT=0.0;
      m_Hf_ZeroT=0.0;
      m_dHfZeroTAtmP=0.0;
      dMass.Zero();
      };
    void Add(CRctSolveBlk & ESB)
      {
      fCpsOK&=ESB.fCpsOK;
      fHfsOK&=ESB.fHfsOK;
      //xdHs+=ESB.xdHs;
      m_HOR+=ESB.m_HOR;
      m_HORDiff+=ESB.m_HORDiff;
      m_Hf_FdT+=ESB.m_Hf_FdT;
      m_Hf_PrT+=ESB.m_Hf_PrT;
      m_Hs_FdT+=ESB.m_Hs_FdT;
      m_Hs_PrT+=ESB.m_Hs_PrT;
      m_Hf_RefT+=ESB.m_Hf_RefT;
      m_Hf_ZeroT+=ESB.m_Hf_ZeroT;
      m_dHfZeroTAtmP+=ESB.m_dHfZeroTAtmP;
      dMass+=ESB.dMass;
      };
    CRctSolveBlk & operator+=(CRctSolveBlk & ESB)
      {
      fCpsOK&=ESB.fCpsOK;
      fHfsOK&=ESB.fHfsOK;
      //xdHs+=ESB.xdHs;
      m_HOR+=ESB.m_HOR;
      m_HORDiff+=ESB.m_HORDiff;
      m_Hf_FdT+=ESB.m_Hf_FdT;
      m_Hf_PrT+=ESB.m_Hf_PrT;
      m_Hs_FdT+=ESB.m_Hs_FdT;
      m_Hs_PrT+=ESB.m_Hs_PrT;
      m_Hf_RefT+=ESB.m_Hf_RefT;
      m_Hf_ZeroT+=ESB.m_Hf_ZeroT;
      m_dHfZeroTAtmP+=ESB.m_dHfZeroTAtmP;
      dMass+=ESB.dMass;
      return *this;
      };
  };

//---------------------------------------------------------------------------

const dword CRS_StopExec  = 0x10000000;
const dword CRS_NoBalance = 0x10000001;
const dword CRS_BadCps    = 0x00000002;
const dword CRS_BadHfs    = 0x00000004;
const dword CRS_NoExtent  = 0x10000008;
const dword CSR_NotAvail  = 0x10000010;

const dword EAS_Rct       = 0x00000001;
const dword EAS_Extent    = 0x00000002;
const dword EAS_HOR       = 0x00000004;
const dword EAS_All       = 0x0000ffff;

class DllImportExport CReaction : public TaggedObject
  {
  public:
    static const pchar CReaction::GroupName;

    CReactionBlock &RB;
    int            SeqNo;
    int            EqnIDNo;
    Strng          m_SeqStr;
    Strng          m_sComment;

    Strng          sState;
    dword          dwState;

    virtual flag   CpsOK() = 0;
    virtual flag   HfsOK() = 0;
    virtual double totDHf(SpModel *Mdl, double T, double P) = 0;

    virtual void   Set_mldH(double mlHeatOfReaction) {};
    virtual eScdReactionBasis Basis()=0;
//    Strng          EqnError;

    CReaction(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CReaction();

    virtual void   BuildDataDefn(DataDefnBlk & DDB)=0;
    virtual flag   DataXchg(DataChangeBlk & DCB)=0;
    virtual flag   ValidateData(ValidateDataBlk & VDB) =0;

    virtual void   Clear()=0;
    virtual void   Parse(CRCTTokenFile &TF)=0;
    virtual void   SetUp()=0;
    virtual dword  Check()=0;
    virtual dword  GetState()=0;
    virtual void   Zero()=0;
    virtual LPCTSTR GetRctString(dword Options=EAS_All)=0;
    virtual void   dbgDump(pchar pTitle)=0;

    virtual long   SolveBlock(int Version, CReactionBlock &RB, CReaction **Eqns, int NEqns, CRctSolveBlk & RSB) { return 0;}
    virtual void   Finalise(CReactionBlock &RB) { }

    virtual void   SrcSnkStart() {};
    virtual void   SrcSnkEnd(CRctSolveBlk &RSB) { RSB.Clear(); };
    virtual void   SrcSnkAdjust(SpMArray &M) {};
    //virtual void   SrcSnkEnd(CSysVector &dMass, double &dHeat) { dMass.Zero(); dHeat=0.0; };
    virtual double HeatExchange(pSpModel M, double DHfTot, double TStart, double TEst, flag Advancing, flag CnvrgStart) { return 0.0; };
#if DOPBSMOOTHING
    virtual void   GetMsAGData(CMSJoin & AG) {};
#endif

    long Fidelity();

//    DEFINE_CI(CReaction, TaggedObject, 4);

  };

typedef CArray <CReaction*, CReaction*> CReactionArray;

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CRCTTokenFile : public CTokenFile
  {
  public:
    CReactionBlock &RB;

    CRCTTokenFile( CReactionBlock *pRB,
                   byte Places,
                   const char* nFileName,
                   flag nIgnoreComments = True,
                   flag nUseStringChar = True,
                   flag nUseIncludes = True) :
      RB(*pRB),
      CTokenFile(Places, nFileName, nIgnoreComments, nUseStringChar, nUseIncludes) {};

    flag           TokenIs(const char * pRqdToken);
    void           CheckToken(const char * pRqdToken);
    flag           TestToken(const char * pRqdToken) { return TokenIs(pRqdToken); };
    double         DoubleToken();
    int            IntToken();
  };

//===========================================================================
//
//
//
//===========================================================================

enum RBStates { RBS_Empty, RBS_Error, RBS_OK};

const byte RBXT_Strict      = 1;
const byte RBXT_Target      = 2;
const byte RBXT_Individual  = 3;

class DllImportExport CReactionBlock : public TaggedObject
  {
  friend class CReactionBase;
  friend class CReaction;
  friend class BalanceData;
  friend class CR_Eqn;
  friend class CR_VCS;
  friend class CR_Src;
  friend class CR_Snk;
  //friend class CR_Environ;
  friend class CRCTTokenFile;

  public:
    CReactionBlock(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CReactionBlock();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    dword          SetTheCIs();

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

  protected:
    void           Clear(flag fClearMsg, flag ClearAll);
    void           Parse(CRCTTokenFile &TF);

    // Solution
    void           GetSequenceIndex(CLMatrix &SeqEq, int &LastSeq, int &LastEq);
    void           Set_SpcMoles(SpModel & Mdl, CSysVector & M, CSysVector & SpcMoles, CSysVector & LockUp);
    void           Get_SpcMoles(SpConduit & C, CSysVector & Moles, CSysVector & Lockup);
    void           Get_SpcMoles(SpContainer & M, CSysVector & Moles, CSysVector & Lockup);
    void           Get_SpcMoles(SpModel & Mdl, CSysVector & Moles, CSysVector & Lockup);

    void           dbgDump();
    void           dbgDumpPostSpcData();
    void           dbgDumpPostCheckData();
    void           dbgDumpPostSolveData();

  public:
    // SetUp
    void           FileLoadAllowed(flag b) { fFileLoadAllowed=b; StructureChanged(this); };
    flag           Load(pchar pRCTName, flag KeepState);
    flag           SetRCTName(pchar pRCTName);
    CReaction     *AddReaction(TagObjClass *pEqnClass);
    CReaction     *AddReaction(CReaction *pRct);
    void           SetUpReactions();

    static void    Error(CReactionBlock *pRctnBlk, char Code, int errNo, pchar s=NULL);
    char          *RCTName() { return sRCTName(); };
    char          *RCTFile() { return sRCTFile(); };
    char          *RCTPath() { return sRCTPath(); };

    double         ConvergeHXs(CReactionArray &HXs, SpModel * M, double DHfTot, double TotHz, double TStart);
    void           AdjustForSrcSnks(SpMArray & M);
    void           EvalProducts(SpConduit & C, double EstFinalT=dNAN/*, bool FindFinalT=true*/);
    void           EvalDerivs(SpContainer & M, SpConduit & Fd, SpConduit & Pr);
    void           ConvergeStates(CConvergeStateBlk &CSB, SpModelOwner /*Container*/ & M, SpVector * MeanResTimes, /*SpConduit & Fd, SpConduit & Pr,*/ double EstFinalT=dNAN);
    void           EvalDiscrete(SpContainer & M, SpConduit & Fd, SpConduit & Pr);
#if DOPBSMOOTHING
    void           GetMsAGData(CMSJoin & AG);
#endif

    flag           OnAndOK()          { return fOn && (iReactionState==RBS_OK); };
    flag           InEvalProducts()   { return m_InWhat==eEvalProducts; };
    flag           InEvalDerivs()     { return m_InWhat==eEvalDerivs; };
    flag           InConvergeStates() { return m_InWhat==eConvergeStates; };
    flag           InDiscrete()       { return m_InWhat==eDiscrete; };
    bool           IsSurgeType();
    FlwNode      * Nd();

    flag           CpsOK();
    flag           HfsOK();

    void           AttachSpModel(SpModelOwner & ModelOwn);
    void           DetachSpModel();

    SpModel *      getModel()         { return m_pRefModel ? m_pRefModel : getOwnModel(); };
    SpModel *      getOwnModel()      { return m_pModelOwn ? m_pModelOwn->pModel : NULL; };
    SpPropOveride *Ovr()              { return m_pOvr; };  
    SpModelOwner & ModelOwner()       { return *m_pModelOwn; };
    double        *MassArray()        { return (m_pModelOwn!=NULL) ? m_pModelOwn->pModel->SVData() : NULL; };
    double         EstFinalT()        { return Valid(m_dEstFinalT) ? m_dEstFinalT : m_TempK; };
    CConvergeStateBlk * CSB()         { return m_pCSB; };
    double         TimeIncAtSolve()   { return m_TimeIncAtSolve; };


    _declspec(property(get=getModel))         SpModel * Model;
    _declspec(property(get=getOwnModel))      SpModel * OwnModel;

    byte           ExtentType()       { return m_ExtentType; };

    double         Temperature(bool Pr=false)   { return Pr ? m_TempKProd : m_TempKFeed; };
    double         Pressure(bool Pr=false)      { return m_Press; };
    double         HfSumTot(bool Pr=false)      { return Pr ? m_dHfSumTotPrT : m_dHfSumTotFdT; };
    double         HfErr(bool Pr=false)         { return Pr ? m_dHfErrPrT : m_dHfErrFdT; };
    double         HfErrZero()                  { return m_dHfErrZeroT; };

    double         HfSumRct(bool Pr=false)      { return Pr ? m_RctB.m_Hf_PrT : m_RctB.m_Hf_FdT; };
    double         HfSumRctZero()               { return m_RctB.m_Hf_ZeroT; };

    double         HfSumSrc(bool Pr=false)      { return Pr ? m_SrcB.m_Hf_PrT : m_SrcB.m_Hf_FdT; };
    double         HfSumSnk(bool Pr=false)      { return Pr ? m_SnkB.m_Hf_PrT : m_SnkB.m_Hf_FdT; };
    double         HfSumSrcSnk(bool Pr=false)   { return Pr ? m_SrcB.m_Hf_PrT+m_SnkB.m_Hf_PrT : m_SrcB.m_Hf_FdT+m_SnkB.m_Hf_FdT; };
    double         HsSumSrc(bool Pr=false)      { return Pr ? m_SrcB.m_Hs_PrT : m_SrcB.m_Hs_FdT; };
    double         HsSumSnk(bool Pr=false)      { return Pr ? m_SnkB.m_Hs_PrT : m_SnkB.m_Hs_FdT; };
    double         HsSumHX()                    { return m_dHsSumHX; };
    double         HsSumPwr()                   { return m_dHsSumPwr; };
    double         HsSumHXPwr()                 { return m_dHsSumHX+m_dHsSumPwr; };
    CSysVector  *  MsSumSrc()                   { return &m_SrcB.dMass; };
    CSysVector  *  MsSumSnk()                   { return &m_SnkB.dMass; };
    //CRctSolveBlk  &Src()          { return
    void           GetClosureInfo(CClosureInfo & CI);

    double         VolWithLockup(PhMask PhaseM, CSysVector & Moles, double T, double P);

    void           OnAppActivate(BOOL bActive);
    int            FilesUsed(CFilesUsedArray & Files);

    int            Version() { return iVersion; };

    long           Fidelity() { return SpModel::Fidelity(); }; // CNM this will have to be improved Model()->Fidelity(); };

    static void    GetSpecieIDs(CSVector &SpcNames, CLVector &SpcIDs);
    static void    FindElements (Strng &ElemList);
    static void    FindElements (CLVector &SpcIDs, Strng &ElemList);
    static void    SortElements (Strng &ElemList, CLVector &ElmsIDs, int &NumElems, int NumSpecies);
    static void    ElementComp (CDMatrix &Bm, Strng &ElemList, int NumElemnts);
    static void    ElementComp (CDMatrix &Bm, CLVector &SpcIDs, Strng &ElemList, int NumElemnts);
    static Strng   Element (int j, Strng &ElemList);
    static void    GetMolMass (CSysVector  &MolMass, CDMatrix &Bm, CLVector &ElemList);
    static void    GetQtyElems (CDVector &QtyElemns, CSysVector  &SpcMoles, CDMatrix &Bm, int NumElemnts);

  protected:

    DEFINE_CI(CReactionBlock, TaggedObject, 16);
  protected:
    flag           fOn;
    Strng          sRCTName;
    Strng          sRCTFile;
    Strng          sRCTPath;
    Strng          m_sComment;
    FILETIME       RCTTime;
    flag           bAutoReload;
    flag           bReloadReqd;
    CToleranceBlock m_RctTol;
    CToleranceBlock m_RctTTol;
    long           iVersion;

    CRCTTokenFile  *pTkns;
    HANDLE         hEditProcess;    //process handle for the editor
    DWORD          dwEditProcessId; //process ID for the editor
    FILETIME       EditTime;

    enum           eEvalPos { eNull, eEvalProducts, eEvalDerivs, eConvergeStates, eDiscrete} m_InWhat;      

    RBStates       iReactionState;  // if True then solving Valid reactions exits to solve
    Strng          m_LoadState;
    Strng          m_RunState;
    Strng          m_LoadError;

    CConvergeStateBlk *m_pCSB;

  public:

    CReactionArray &All;
    CReactionArray &Seq;
    //CReactionArray &Eqns;
    CReactionArray &Eqns;
    CReactionArray &Srcs;
    CReactionArray &Snks;
    CReactionArray &Pwrs;
    CReactionArray &HXs1;
    CReactionArray &HXs2;

    int            NAll()  { return All.GetSize(); };
    int            NSeq()  { return Seq.GetSize(); };
    int            NEqns() { return Eqns.GetSize(); };
    int            NSrcs() { return Srcs.GetSize(); };
    int            NSnks() { return Snks.GetSize(); };
    int            NPwrs() { return Pwrs.GetSize(); };
    int            NHXs1() { return HXs1.GetSize(); };
    int            NHXs2() { return HXs2.GetSize(); };

    int            nPermReacts;
    flag           fFileLoadAllowed;
    
    double         m_dHfIn;
    double         m_dHfOut;

    double         m_dHfSumTotFdT;
    double         m_dHfSumTotPrT;
    double         m_dHfSumTotRefT;
    double         m_dHfSumTotZeroT;
    double         m_dHfSumTotZeroTAtmP;

    double         m_dHfErrFdT;
    double         m_dHfErrPrT;
    double         m_dHfErrZeroT;
    double         m_dHsSumHX;
    double         m_dHsSumPwr;

    double         m_HfFdStd0;
    double         m_HfPrStd0;
    double         m_HfFdStdT;
    double         m_HfPrStdT;
    double         m_HfFdMdl0;
    double         m_HfPrMdl0;
    double         m_HfFdMdlT;
    double         m_HfPrMdlT;

    CRctSolveBlk   m_RctB, m_SrcB, m_SnkB;

    CLVector       ElemIds;      // individual specie names
    Strng          ElemList;     // list of elements names

    CReactionBase *m_RctBase;
    SpPropOveride *m_pOvr;
    SpModelOwner * m_pModelOwn;
    SpModel      * m_pRefModel;
    SpModel      * m_pStdRefModel;
    bool           m_UseStdRefModel;
    SpVector       Moles;        // moles of each specie
    SpVector       DMoles;       // currents est of change in moles of each specie
    SpVector       Moles_S;      // moles of each specie  @ Start
    SpVector       Moles_BS;     // moles of each specie  @ Start of each block
    //SpVector       Moles_DStrt;  // moles of each specie  @ Start of Eval Derivs

    SpVector       dMoles;       // rate of change of moles of each specie
    SpVector       dMass;        // rate of change of mass of each specie
    SpVector       Lockup;       // moles of each specie Lockedup

    SpVector     * m_pSpMRT;        // Mean Residence Time of each specie

    CDMatrix       Bm;           // species molecular formula vector
    CDVector       QtyElemns;    // elemental abundance input values
    SpVector       MolMass;      // molecular mass of each specie
    int            NumSpecies;
    int            NumElemnts;
    double         m_TempK;        // temperature
    double         m_TempKFeed;    // temperature @ Start
    double         m_TempKRct;     // temperature @ Rct
    double         m_TempKProd;    // temperature @ End
    double         m_UsrDefT;      
    double         m_UsrDefP;      
    double         m_dEstFinalT;   // Estimated final temperature
    double         m_Press;        // pressure

    bool           m_bFindFinalT;

    byte           m_ExtentType;

    double         m_TimeIncAtSolve;

    static byte    sm_AllVars;
    static byte    sm_ChgVars;

  };

const byte SRSX_UsrDefInfo  = 0x01;
const byte SRSX_FeedInfo = 0x02;
const byte SRSX_ProdInfo = 0x04;
const byte SRSX_All      = 0x07;

const byte SRSC_Mass  = 0x01;
const byte SRSC_Molar = 0x02;
const byte SRSC_All   = 0x03;

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CReactionBase : public CBlockEvalBase
  {
  public:
    CReactionBase(FlwNode * pAttach, bool IsSurgeType, LPTSTR Name="RB") : CBlockEvalBase(BEId_RB, 0, Name)
      { 
      //m_sRBTag=Name;
      pRB=NULL; 
      m_pNd=pAttach; 
      fEnabled=bOn=0; 
      m_bIsSurgeType=IsSurgeType; 
      m_dTimeInc=0;
      sm_AllRBs.AddTail(this);
      };
    ~CReactionBase() 
      { 
      POSITION Pos=sm_AllRBs.Find(this);
      if (Pos)
        sm_AllRBs.RemoveAt(Pos);
      delete pRB; 
      };

    CReactionBlock * operator() () { return bOn ? pRB : NULL; };

    flag           Open();
    flag           Open(byte OnOffSeq) { CBlockEvalBase::Open(OnOffSeq); return Open(); }
    byte           OpenStatus() { return CBlockEvalBase::OpenStatus(Enabled()); }
    flag           Load(char * pRCTFn) { return Open() ? pRB->Load(pRCTFn, false) : False; };
    char          *RBTag()   { return Name(); }//m_sRBTag(); };
    flag           SetRCTName(char * pRCTFn) { return Open() ? pRB->SetRCTName(pRCTFn) : False; };
    char          *RCTName() { return Open() ? pRB->RCTName() : NULL; };
    char          *Comment() { return Open() ? pRB->m_sComment() : NULL; };
    void           Close();
    void           Enable() { bOn=1; };
    void           Disable() { bOn=0; };
    bool           IsSurgeType() { return m_bIsSurgeType; };
    void           SetSurgeType(bool bIsSurgeType) { m_bIsSurgeType = bIsSurgeType; };
    FlwNode      * Nd()          { return m_pNd; };

    flag           Enabled() { return fEnabled && (pRB!=NULL) && bOn; };
    flag           OnAndOK() { return Enabled() && pRB->OnAndOK(); };
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=isParmStopped);
    void           BuildDataDefn(DataDefnBlk &DDB, pchar pTag_=NULL, pchar pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);

    flag           ValidateData(ValidateDataBlk & VDB) { return Enabled() ? pRB->ValidateData(VDB) : True; };

    double         Temperature(bool Pr=false) { return Enabled() ? pRB->Temperature(Pr) : StdT; };
    double         Pressure(bool Pr=false)    { return Enabled() ? pRB->Pressure(Pr) : StdP; };
    double         HfSumTot(bool Pr=false)    { return Enabled() ? pRB->HfSumTot(Pr) : 0.0; };
    double         HfSumSrc(bool Pr=false)    { return Enabled() ? pRB->HfSumSrc(Pr) : 0.0; };
    double         HfSumSnk(bool Pr=false)    { return Enabled() ? pRB->HfSumSnk(Pr) : 0.0; };
    double         HfSumRct(bool Pr=false)    { return Enabled() ? pRB->HfSumRct(Pr) : 0.0; };
    double         HfSumRctZero()             { return Enabled() ? pRB->HfSumRctZero() : 0.0; };
    double         HfErr(bool Pr=false)       { return Enabled() ? pRB->HfErr(Pr)    : 0.0; };
    double         HfErrZero()                { return Enabled() ? pRB->HfErrZero()  : 0.0; };
    double         HsSumHX()                  { return Enabled() ? pRB->HsSumHX()  : 0.0; };
    double         HsSumPwr()                 { return Enabled() ? pRB->HsSumPwr() : 0.0; };
    CSysVector    *MsSumSrc()                 { return Enabled() ? pRB->MsSumSrc() : NULL; };
    CSysVector    *MsSumSnk()                 { return Enabled() ? pRB->MsSumSnk() : NULL; };

    double         EstFinalT()                { return Enabled() ? pRB->EstFinalT() : Std_T; };

    void           EvalProducts(SpConduit & C, double EstFinalT=dNAN/*, bool FindFinalT=false*/) { if (Enabled()) pRB->EvalProducts(C, EstFinalT/*, FindFinalT*/); };
    void           EvalDerivs(SpContainer & M, SpConduit & Fd, SpConduit & Pr) { if (Enabled()) pRB->EvalDerivs(M, Fd, Pr); };
    void           ConvergeStates(CConvergeStateBlk &CSB, SpModelOwner /*Container*/ & M, SpVector * MeanResTimes/*, SpConduit & Fd, SpConduit & Pr*/) { if (Enabled()) pRB->ConvergeStates(CSB, M, MeanResTimes/*, Fd, Pr*/); };
    void           EvalDiscrete(SpContainer & M, SpConduit & Fd, SpConduit & Pr) { if (Enabled()) pRB->EvalDiscrete(M, Fd, Pr); };
#if DOPBSMOOTHING
    void           GetMsAGData(CMSJoin & AG)              { if (Enabled()) pRB->GetMsAGData(AG); };
#endif
    void           GetClosureInfo(CClosureInfo & CI)      { if (Enabled()) pRB->GetClosureInfo(CI); };

    void           OnAppActivate(BOOL bActive)            { if (Enabled()) pRB->OnAppActivate(bActive); };
    int            FilesUsed(CFilesUsedArray & Files)     { return pRB ? pRB->FilesUsed(Files) : 0; };
    flag           GetOtherData(FilingControlBlock &FCB)  { return pRB ? pRB->GetOtherData(FCB) : false; };
    flag           PutOtherData(FilingControlBlock &FCB)  { return pRB ? pRB->PutOtherData(FCB) : false; };

    int            GetRctCount();
    eScdReactionBasis GetRctBasis(int iRct);
    LPCTSTR        GetRctTag(int iRct);
    LPCTSTR        GetRctComment(int iRct);
    LPCTSTR        GetRctString(int iRct);
    long           GetRctSequence(int iRct);

  protected:
    flag           bOn;
    flag           fEnabled;
    CReactionBlock * pRB;
    FlwNode       *m_pNd;
    //Strng          m_sRBTag;
    bool           m_bIsSurgeType;
    double         m_dTimeInc;
  
  public:
    static CList<CReactionBase*, CReactionBase*> sm_AllRBs;
  };

//===========================================================================
//
//
//
//===========================================================================

inline long CReaction::Fidelity()         { return RB.OwnModel->Fidelity(); };
inline bool CReactionBlock::IsSurgeType() { return m_RctBase->IsSurgeType(); };
inline FlwNode * CReactionBlock::Nd()     { return m_RctBase->Nd(); };

//===========================================================================
//
//
//
//===========================================================================

extern LPCTSTR RctStrippedDouble(double D);

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif

