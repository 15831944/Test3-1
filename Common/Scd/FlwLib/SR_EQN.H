//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SR_EQN_H
#define  __SR_EQN_H

#ifndef __SP_REACT_H
  #include "sp_react.h"
#endif

#ifndef __NRECIPE_H
  #include "nrecipe.h"
#endif

#ifdef __SR_EQN_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

XID xid_HORRqdDH          = RctXID(1000);
//XID xid_HORDefT           = RctXID(1001);
XID xid_totHORUsrDef      = RctXID(1002);
XID xid_mlHORUsrDef       = RctXID(1003);
XID xid_msHORUsrDef       = RctXID(1004);
XID xid_msHfRef           = RctXID(1005);
XID xid_totHORRctPr       = RctXID(1006);
XID xid_mlHORRctPr        = RctXID(1007);
XID xid_msHORRctPr        = RctXID(1008);
XID xid_totHORRctFd       = RctXID(1009);
XID xid_mlHORRctFd        = RctXID(1010);
XID xid_msHORRctFd        = RctXID(1011);
XID xid_msHfRct           = RctXID(1012);
XID xid_RCTExtent         = RctXID(1013);
XID xid_Reaction          = RctXID(1014);
XID xid_RctSeq            = RctXID(1015);
XID xid_RctState          = RctXID(1016);
XID xid_ResTime           = RctXID(1017);
XID xid_RTRef             = RctXID(1018);
XID xid_dMlXfer           = RctXID(1019);
XID xid_dMsXfer           = RctXID(1020);
XID xid_dMlXferInt        = RctXID(1021);
XID xid_ExtentError       = RctXID(1022);
XID xid_RCTFracRate       = RctXID(1023);
XID xid_RCTKEquilibrium   = RctXID(1024);
XID xid_RCTFinalConcT     = RctXID(1025);

//===========================================================================
// Stoichometry classes
class CR_EqnControl;
class CR_EqnHeat;
class CR_EqnTerm;
class CR_Eqn;

//===========================================================================
//
//
//
//===========================================================================

class CR_EqnControlFactory;
class DllImportExport CR_EqnControl
  {
  public:
    CR_EqnControl(CR_EqnControlFactory * Factory, CR_Eqn* pEqn);
    virtual ~CR_EqnControl() { Clear(); };

    virtual eScdReactionBasis Basis() { return RctEquilibrium; };
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF) {};
    void           ParseExtent(CRCTTokenFile &TF);
    void           ParseDynamic(CRCTTokenFile &TF);
    virtual void   SetUp() {};
    virtual LPCTSTR GetRctString() { m_AsString="CR_EqnControl"; return m_AsString; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB) {};
    virtual void   BuildDataDefnEnd(DataDefnBlk & DDB) {};
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    flag           SetReactProdInx(int SpcId, int &XReactTerm, int &XProdTerm);
    flag           SetExtent(int SpcId, double Frac);
    double         MolarKRatio(double dProdMoles);

    bool           DynamicRate() { return RB.Nd() && RB.Nd()->SolveDynamicMethod() && RB.m_RctBase->IsSurgeType(); }

    // Methods Required by "Default ReactingMoles())"
    virtual double LimitingReactant();
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual void   ApplyChanges(double dProdMoles, SpMArray & M);
    virtual double MeanResidenceTimeReact();
    virtual double MeanResidenceTimeProd();
    virtual double ExtentDynamicScale(double K);

    virtual void   StartStep();
    virtual void   EvalDiscrete();
  
    virtual bool   ExtentITOInitConds()               { return false; };
    virtual double ExtentReqd()                       { return m_ddExtentRqd(); };
    virtual double ExtentActual(double ProdMoles=0.0) { return dNAN; };
    virtual double ExtentError();
    virtual flag   ExtentType();
    virtual double FwdRctExtent();
    virtual double RevRctExtent();

  public:
    CR_EqnControlFactory * m_pFactory;
    CR_Eqn         &Eq;
    CReactionBlock &RB;

    int            m_iXSpcId;
    int            m_iXReactTerm; 
    int            m_iXProdTerm;
    DualDbl        m_ddExtentRqd;

    bool           m_bDoRate;
    bool           m_bStablize;
    DualDbl        m_ddFracRate;
    double         m_dKStart;
    double         m_dKEOStep;
    double         m_dKFinal;

    bool           m_bDoResTime;
    DualDbl        m_ddDefinedResTime;
    DualDbl        m_ddActualResTime;
    double         m_dAlpha;
    double         m_dRelRate;

    Strng          m_sExtentSpc;
    byte           m_bExtentType;
    CString        m_AsString;
  };

DEFINE_CLASSFACTORY1(CR_EqnControl, CR_Eqn*)

//===========================================================================
//
//
//
//===========================================================================

class CR_EqnHeatFactory;
class DllImportExport CR_EqnHeat
  {
  public:
    CR_EqnHeat(CR_EqnHeatFactory * Factory, CR_Eqn* pEqn);

    virtual void   Clear() 
      { 
      m_bProdTerm=false;
      m_iTermNo=0; 
      m_iCnvBasisRqd=DC_HMs; 
      m_iCnvBasisAct=DC_HMs; 
      };
    virtual void   Parse( CRCTTokenFile &TF)
      {
      m_bProdTerm=false;
      m_iTermNo=0; 
      };
    virtual void   SetUp() { };
    virtual LPCTSTR GetRctString() { return ""; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB){return True;};

    virtual double msDHf(SpModel *Mdl, double T, double P);
    virtual double mlDHf(SpModel *Mdl, double T, double P);
    virtual double totDHf(SpModel *Mdl, double T, double P);

  public:
    CR_Eqn         &m_Eq;
    CReactionBlock &m_RB;
    bool           m_bProdTerm;
    int            m_iTermNo;
    CCnvIndex      m_iCnvBasisRqd;
    CCnvIndex      m_iCnvBasisAct;
    CString        m_AsString;
    CR_EqnHeatFactory * m_pFactory;
  };


DEFINE_CLASSFACTORY1(CR_EqnHeat, CR_Eqn*)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CR_EqnHeatFixedMs : public CR_EqnHeat
  {
  public:
    CR_EqnHeatFixedMs(CR_EqnHeatFactory * Factory, CR_Eqn* pEqn);
    CR_EqnHeatFixedMs(CR_EqnHeatFactory * Factory, CR_Eqn* pEqn, double msdH);

    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    virtual double totDHf(SpModel *Mdl, double T, double P);

  public:
    DualDbl        m_ddRqddH;
    double         m_dDefT;
    double         m_dDefP;
  };

DEFINE_CLASSBUILDER1(CR_EqnHeatFixedMs, CR_EqnHeat, CR_Eqn*)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CR_EqnHeatFixedMl : public CR_EqnHeatFixedMs
  {
  public:
    CR_EqnHeatFixedMl(CR_EqnHeatFactory * Factory, CR_Eqn* pEqn);
    CR_EqnHeatFixedMl(CR_EqnHeatFactory * Factory, CR_Eqn* pEqn, double mldH);

    virtual void   Parse(CRCTTokenFile &TF);
    virtual double totDHf(SpModel *Mdl, double T, double P);
    virtual LPCTSTR GetRctString();
  };

DEFINE_CLASSBUILDER1(CR_EqnHeatFixedMl, CR_EqnHeat, CR_Eqn*)

//===========================================================================
//
//                           Stoichiometry Equations
//
//===========================================================================

class DllImportExport CR_EqnTerm
  {
  public:
    Strng        SpcName;
    double       NoOfMoles;
    int          SpcId;
    double       dSpUsed;
    double       dSpAvailToEqn;
    double       dMlXfer;
    double       m_dMsXfer;

    CR_EqnTerm()
      {
      SpcName="";
      NoOfMoles=0.0;
      SpcId=-1;
      dSpUsed=0.0;
      dSpAvailToEqn=0.0;
      dMlXfer=0.0;
      m_dMsXfer=0.0;
      }
    CR_EqnTerm(char * Name, double StMoles, int Id)
      {
      SpcName=Name;
      NoOfMoles=StMoles;
      SpcId=Id;
      if (SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      dSpUsed=0.0;
      dSpAvailToEqn=0.0;
      dMlXfer=0.0;
      m_dMsXfer=0.0;
      };

    CR_EqnTerm(char * Name, double StMoles)
      {
      SpcName=Name;
      NoOfMoles=StMoles;
      SpcId=SDB.Find(Name);
      if (SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      dSpUsed=0.0;
      dSpAvailToEqn=0.0;
      dMlXfer=0.0;
      m_dMsXfer=0.0;
      };
    CR_EqnTerm(int Id, double StMoles)
      {
      NoOfMoles=StMoles;
      SpcId=Id;
      if (SpcId<0)
        CReactionBlock::Error(NULL, 'B', 11);
      else
        SpcName=SDB[Id].SymOrTag();
      dSpUsed=0.0;
      dSpAvailToEqn=0.0;
      dMlXfer=0.0;
      m_dMsXfer=0.0;
      };
    void Clear()
      {
      SpcName="";
      NoOfMoles=0.0;
      SpcId=-1;
      dSpUsed=0.0;
      dSpAvailToEqn=0.0;
      dMlXfer=0.0;
      m_dMsXfer=0.0;
      }
    void   Zero()
      {
      dSpUsed=0;
      dSpAvailToEqn=0;
      dMlXfer=0.0;
      m_dMsXfer=0.0;
      };
  };

class CR_EqnTermArray : public CArray <CR_EqnTerm, CR_EqnTerm&> 
  {
  public:
    void AddTerm(char * Name, double StMoles, int Id)
      {
      CR_EqnTerm T(Name, StMoles, Id);
      Add(T);
      };
    void AddTerm(char * Name, double StMoles)
      {
      CR_EqnTerm T(Name, StMoles);
      Add(T);
      };
    void AddTerm(int Id, double StMoles)
      {
      CR_EqnTerm T(Id, StMoles);
      Add(T);
      };
  };

//---------------------------------------------------------------------------

class DllImportExport CR_EqnBlk
  {
  public:
    CIArray     SpId2L;
    CIArray     L2SpId;
    CNRSimplex  SPlex;
    CDVector    SpcReqd;
    CDVector    SpcTotRate;
    CDVector    Time2Exhaust;
    CIArray     IsEquil;
    CDVector    EXfrReqd;
    CDVector    EXfrRate;
    CDVector    EXfrReqdTarget;
    CDVector    EXfrReqdTot;
    CDVector    EXfrReqdStep;
  };

//---------------------------------------------------------------------------

DEFINE_REACTION(CR_Eqn);
class DllImportExport CR_Eqn : public CReaction
  {
  public:
    CR_Eqn(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CR_Eqn();


    CR_EqnControl& Control() { ASSERT(pControl); return *pControl; };
    flag           HasControl() { return pControl!=NULL; };
    flag           FwdRct() { return fFwdRct; };
    flag           RevRct() { return fRevRct; };
    flag           RctOn() { return fRctOn; };
    flag           DoFwdRctExtent() { return pControl && (pControl->m_iXReactTerm>=0); };
    flag           DoRevRctExtent() { return pControl && (pControl->m_iXProdTerm>=0); };

    int            NReactants() { return Reactant.GetSize(); };
    int            NProducts() { return Product.GetSize(); };
    
    CR_EqnHeat&    Heat() { ASSERT(pHeat); return *pHeat; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual eScdReactionBasis Basis();
    virtual void   GetEqnAsString(Strng & Es, dword Options);
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);

    int            SpecieReactantIndex(Strng &SpcName);
    int            SpecieProductIndex(Strng &SpcName);
    int            SpecieReactantIndex(int SpcId);
    int            SpecieProductIndex(int SpcId);
    virtual void   SetUp();
    virtual dword  Check();
    virtual void   Zero();
    virtual dword  GetState();
    virtual LPCTSTR GetRctString(dword Options=EAS_All) { GetEqnAsString(m_AsString, Options); return (LPCTSTR)m_AsString(); };
    virtual void   dbgDump(pchar pTitle);

    flag           ParseTerm(CR_EqnTerm &Term);

    virtual double LimitReactantMoles();
    virtual double LimitProductMoles();
    virtual double ReactingMoles();
    virtual flag   SetReactProdInx(int SpcId, int &XReactTerm, int &XProdTerm);
    virtual flag   SetExtent(int SpcId, double Frac);

    virtual flag   CpsOK();
    virtual flag   HfsOK();
    virtual double totDHf(SpModel *Mdl, double T, double P)     { return pHeat->totDHf(Mdl, T, P); };

    virtual void   Set_mlDHf(int SpcNo, double mlHeatOfReaction);
    virtual void   Set_msDHf(int SpcNo, double msHeatOfReaction);

    static long    SolveBlock1(CReactionBlock &RB, CR_Eqn**Eqns, int NEqns, CRctSolveBlk & RSB);
    static long    SolveBlock2(CReactionBlock &RB, CR_Eqn**Eqns, int NEqns, CRctSolveBlk & RSB);
    static long    SolveBlock3(CReactionBlock &RB, CR_Eqn**Eqns, int NEqns, CRctSolveBlk & RSB);

    virtual long   SolveBlock(int Version, CReactionBlock &RB, CReaction **Eqns, int NEqns, CRctSolveBlk & RSB);
    virtual void   Finalise(CReactionBlock &RB);
    
    void           AdjustModel(double dProdMoles, double T, double P);

#if DOPBSMOOTHING  
    virtual void   GetMsAGData(CMSJoin & AG);
#endif

    DEFINE_CI(CR_Eqn, CReaction, 8);

  public:
    static CToleranceBlock sm_ExtentTol;


    CR_EqnTermArray & Reactant;
    CR_EqnTermArray & Product;
    CR_EqnControl* pControl;
    CR_EqnHeat*    pHeat;

    flag           fFwdRct;
    flag           fRevRct;
    flag           fRctOn;

    flag           fAutoBalance;
    
    flag           fDone;
    flag           fCnvrgd;

    int            iLimitSpc;
    double         dMlXfer;
    //double         dMlXferTot;
    double         dRctScl;
    double         m_dRelRate;
    int            iDirn;

    Strng          m_DispEqnStr;
    Strng          m_AsString;
    
    CR_EqnBlk      *pBlk;

  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif

