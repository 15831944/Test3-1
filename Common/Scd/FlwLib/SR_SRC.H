//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SR_SRC_H
#define  __SR_SRC_H

#ifndef __SP_REACT_H
  #include "sp_react.h"
#endif

#ifdef __SR_SRC_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


XID xidRCTSrcHf       = RctXID(100);
XID xidRCTSrcHs       = RctXID(101);
XID xidRCTSrcReaction = RctXID(102);
XID xidRCTSnkHf       = RctXID(103);
XID xidRCTSnkHs       = RctXID(104);
XID xidRCTSnkReaction = RctXID(105);
XID xidRCTHXAmbTemp   = RctXID(110);
XID xidRCTHXTFrac     = RctXID(111);
XID xidRCTHXTargetT   = RctXID(112);
XID xidRCTHXPwr       = RctXID(113);
XID xidRCTHXRqdFinalT = RctXID(114);
XID xidRCTHXElectEff  = RctXID(115);
XID xidRCTSrcMs       = RctXID(200);
XID xidRCTSnkMs       = RctXID(300);

//===========================================================================
//
//                          Source Reaction
//
//===========================================================================

_FWDDEF(CR_SrcTerm);
_FWDDEF(CR_Src);


class DllImportExport CR_SrcTerm
  {
  public:
    CR_SrcTerm()
      {
      m_SpcName="";
      m_Demand=0.0;
      m_SpcId=-1;
      }
    CR_SrcTerm(char * Name, double StMoles, int Id)
      {
      m_SpcName=Name;
      m_Demand=StMoles;
      m_SpcId=Id;
      if (m_SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      };

    CR_SrcTerm(char * Name, double Val)
      {
      m_SpcName=Name;
      m_Demand=Val;
      m_SpcId=SDB.Find(Name);
      if (m_SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      };
    void Zero() 
      {
      m_Demand=0;
      };

  public:
    Strng        m_SpcName;
    double       m_Demand;
    int          m_SpcId;
 };

class CR_SrcTermArray : public CArray <CR_SrcTerm, CR_SrcTerm&> 
  {
  public:
    void AddTerm(char * Name, double StMoles, int Id)
      {
      CR_SrcTerm T(Name, StMoles, Id);
      Add(T);
      };
    void AddTerm(char * Name, double StMoles)
      {
      CR_SrcTerm T(Name, StMoles);
      Add(T);
      };
  };

//---------------------------------------------------------------------------

DEFINE_REACTION(CR_Src);
class DllImportExport CR_Src : public CReaction
  {
  public:

    int            NSpcs() { return Spcs.GetSize(); };

    CR_Src(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CR_Src();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual eScdReactionBasis Basis();
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual dword  Check();
    virtual dword  GetState();
    virtual void   Zero();
    virtual LPCTSTR GetRctString(dword Options=EAS_All);
    virtual void   dbgDump(pchar pTitle);

    flag           ParseTerm(CR_SrcTerm &Term);

    virtual flag   CpsOK();
    virtual flag   HfsOK();
    virtual double totDHf(SpModel *Mdl, double T, double P);

    virtual void   SrcSnkStart();
    virtual void   SrcSnkEnd(CRctSolveBlk & RSB);//SpMArray &dMass, double &dHeat);
    virtual void   SrcSnkAdjust(SpMArray & M);
    
#if DOPBSMOOTHING  
    virtual void   GetMsAGData(CMSJoin & AG);
#endif
  public:
    CR_SrcTermArray & Spcs;
    double         AddMoles;

    CString        m_AsString;
    Strng          m_DispEqnStr;
  };

//===========================================================================
//
//
//
//===========================================================================
//===========================================================================
//
//                          Source Reaction
//
//===========================================================================

_FWDDEF(CR_SnkTerm);
_FWDDEF(CR_Snk);


class DllImportExport CR_SnkTerm
  {
  public:
    CR_SnkTerm()
      {
      m_SpcName="";
      m_Excess=0.0;
      m_SpcId=-1;
      }
    CR_SnkTerm(char * Name, double StMoles, int Id)
      {
      m_SpcName=Name;
      m_Excess=StMoles;
      m_SpcId=Id;
      if (m_SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      };

    CR_SnkTerm(char * Name, double Val)
      {
      m_SpcName=Name;
      m_Excess=Val;
      m_SpcId=SDB.Find(Name);
      if (m_SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      };
    void   Zero()
      {
      m_Excess=0;
      };

  public:
    Strng        m_SpcName;
    double       m_Excess;
    int          m_SpcId;
  };

class CR_SnkTermArray : public CArray <CR_SnkTerm, CR_SnkTerm&> 
  {
  public:
    void AddTerm(char * Name, double StMoles, int Id)
      {
      CR_SnkTerm T(Name, StMoles, Id);
      Add(T);
      };
    void AddTerm(char * Name, double StMoles)
      {
      CR_SnkTerm T(Name, StMoles);
      Add(T);
      };
  };

//---------------------------------------------------------------------------

DEFINE_REACTION(CR_Snk);
class DllImportExport CR_Snk : public CReaction
  {
  public:
    int            NSpcs() { return Spcs.GetSize(); };

    CR_Snk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CR_Snk();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual eScdReactionBasis Basis();
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual dword  Check();
    virtual dword  GetState();
    virtual void   Zero();
    virtual LPCTSTR GetRctString(dword Options=EAS_All);
    virtual void   dbgDump(pchar pTitle);

    flag           ParseTerm(CR_SnkTerm &Term);

    virtual flag   CpsOK();
    virtual flag   HfsOK();
    virtual double totDHf(SpModel *Mdl, double T, double P);

    virtual void   SrcSnkStart();
    virtual void   SrcSnkEnd(CRctSolveBlk & RSB);
    virtual void   SrcSnkAdjust(SpMArray & M);
    
#if DOPBSMOOTHING  
    virtual void   GetMsAGData(CMSJoin & AG);
#endif

  public:
    double         AddMoles;
    CString        m_AsString;
    Strng          m_DispEqnStr;

    CR_SnkTermArray & Spcs;
  };

//===========================================================================
//
//
//
//===========================================================================

const byte CR_HX_Pwr          = 1;
// HX2 Types
const byte CR_HX_UA           = 2;
const byte CR_HX_ApproachAmb  = 3;
// HX1 types                  
const byte CR_HX_ApproachT    = 4;
const byte CR_HX_Electrolysis = 5;
const byte CR_HX_FinalT       = 6;

DEFINE_REACTION(CR_HX);
class DllImportExport CR_HX : public CReaction
  {
  public:

    CR_HX(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CR_HX();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual eScdReactionBasis Basis();
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual dword  Check();
    virtual void   Zero();
    virtual dword  GetState();
    virtual LPCTSTR GetRctString(dword Options=EAS_All);
    virtual void   dbgDump(pchar pTitle);
    virtual void   StartStep();
    virtual void   EvalProducts(){};
    virtual void   EvalDerivs(){};
    virtual void   ConvergeStates(CConvergeStateBlk &CSB){};
    virtual void   EvalDiscrete(){};

    flag           ParseTerm(CR_SnkTerm &Term);

//    virtual double ReactingMoles();

    virtual void   ProductFromExtent(double RMols, double dTime){};
    virtual void   ProductFromRates(double RMols, double dTime){};
    virtual void   DerivsFromRates(double RMols, double dTime){};
    virtual void   ConvergeExtent(double RMols){};

    virtual flag   CpsOK() { return true; };
    virtual flag   HfsOK() { return true; };;
    virtual double totDHf(SpModel *Mdl, double T, double P) { return 0.0; };

    virtual double HeatExchange(pSpModel M, double DHfTot, double TStart, double TEst, flag Advancing, flag CnvrgStart);

  public:
    byte           m_iWhat;
    flag           m_bMaintFeedT;
    flag           m_bLocalAmbT;
    double         m_dHs;
    double         m_dPwr;
    DualDbl        m_ddHtXfer;
    DualDbl        m_ddAmbTemp;
    DualDbl        m_ddPwr;
    DualDbl        m_ddUA;
    DualDbl        m_ddTFrac;
    DualDbl        m_ddRqdFinalT;
    DualDbl        m_ddTargetT;
    DualDbl        m_ddElectEff;
    
    eScdReactionBasis  m_theBasis;

    CString        m_AsString;
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif

