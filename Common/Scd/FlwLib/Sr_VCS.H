//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SR_VCS_H
#define  __SR_VCS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifndef __SP_REACT_H
  #include "sp_react.h"
#endif

#ifndef __SR_VCS1_H
  #include "sr_vcs1.h"
#endif

#ifdef __SR_VCS_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// Free Energy Classes
_FWDDEF(CR_VCS)

//===========================================================================
//
//                          Free Energy Minimisation
//
//===========================================================================

_FWDDEF(CR_VCSSpc);
_FWDDEF(CR_VCS);

class DllImportExport CR_VCSSpc
  {
  public:
    Strng        SpcName;
    //double       NoOfMoles;
    int          SpcId;
    double       dSpUsed;
    double       dSpAvailToEqn;
    double       dUseFrac;
    double       dMlXfer;

    CR_VCSSpc()
      {
      SpcName="";
//      NoOfMoles=0.0;
      SpcId=-1;
      dSpUsed=dSpAvailToEqn=dUseFrac=dMlXfer=0.0;
      }
    CR_VCSSpc(char * Name, int Id)
      {
      SpcName=Name;
      SpcId=Id;
      if (SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      dSpUsed=dSpAvailToEqn=dUseFrac=dMlXfer=0.0;
      };

    CR_VCSSpc(char * Name)
      {
      SpcName=Name;
      SpcId=SDB.Find(Name);
      if (SpcId<0)
        CReactionBlock::Error(NULL, 'B', 0, Name);
      dSpUsed=dSpAvailToEqn=dUseFrac=dMlXfer=0.0;
      };
    void Clear()
      {
      SpcName="";
      SpcId=-1;
      dSpUsed=dSpAvailToEqn=dUseFrac=dMlXfer=0.0;
      }
    void   Zero()
      {
      dSpUsed=0;
      dSpAvailToEqn=0;
      dUseFrac=0;
      dMlXfer=0;
      };
  };

class CR_VCSSpcArray : public CArray <CR_VCSSpc, CR_VCSSpc&> 
  {
  public:
    void AddTerm(char * Name, int Id)
      {
      CR_VCSSpc T(Name, Id);
      Add(T);
      };
    void AddTerm(char * Name)
      {
      CR_VCSSpc T(Name);
      Add(T);
      };
  };

//---------------------------------------------------------------------------


DEFINE_REACTION(CR_VCS);
class DllImportExport CR_VCS : public CReaction
  {
  public:
    CR_VCS(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CR_VCS();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual eScdReactionBasis Basis();
    virtual void   GetEqnAsString(Strng & Es, dword Options);
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    int            SpecieIndex(Strng &SpcName);
    //int            SpecieProductIndex(Strng &SpcName);
    virtual void   SetUp();
    virtual dword  Check();
    virtual void   Zero();
    virtual dword  GetState();
    virtual LPCTSTR GetRctString(dword Options);
    virtual void   dbgDump(pchar pTitle);

    flag           ParseSpc(CR_VCSSpc &Term);

    virtual flag   CpsOK();
    virtual flag   HfsOK();
    virtual double totDHf(SpModel *Mdl, double T, double P) { return 0;};
    virtual long   SolveBlock(int Version, CReactionBlock &RB, CReaction **Eqns, int NEqns, CRctSolveBlk & RSB);

    DEFINE_CI(CR_VCS, CReaction, 8);

  public:
    CR_VCSSpcArray & m_Species;
    int            NSpecies() { return m_Species.GetSize(); };

    flag           fDone;
    flag           fCnvrgd;

    CVCSSolver     m_VCS;

    CString        m_AsString;
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif




