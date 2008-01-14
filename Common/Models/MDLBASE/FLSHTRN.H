//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLSHTRN_H
#define  __FLSHTRN_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif

#ifdef __FLSHTRN_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

const double MinFlshTrainP=1.0;

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CFT_Flash : public CMacroMdlBase
  {
  friend class CMMFlashTrain;
  friend class MFT_Flash; // SMDK
  public:
    CFT_Flash(FlwNode * pNd);

    void        BuildDataDefn(DataDefnBlk & DDB);
    flag        DataXchg(DataChangeBlk & DCB);
    void        SetState(eScdMdlStateActs RqdState);

  public:
    FlwNode *   m_pNd;

    double      m_dQVap;
    double      m_dFeedP;
    double      m_dFeedVapP;
    double      m_dPRqd;
    double      m_dPRqdCalc;
    double      m_dPEval;
    
    double      m_dQVapRqd;
    double      m_dQVapEval;
    double      m_dQVapFrac;
    double      m_dQFrac;
    double      m_dQCool;
    double      m_dQRqdTotal;
    
    double      m_dDQvDp;
    flag        m_fDQvDpGood;

    double      m_dFlashP;
    double      m_dFlashT;

    double      m_dSuctionP;    // Copy of Suction Pess in Condensors

    byte        m_iMMEqpList;

    #if DOPBSMOOTHING  
    //CFlwNodeArray CoolLnks;
    CArray <CMSIO, CMSIO&> m_CoolLnks;
    double      m_dVapLoss;
    #endif

  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CFT_Condenser : public CMacroMdlBase
  {
  friend class CMMFlashTrain;
  friend class MFT_Condenser; // SMDK
  public:
    CFT_Condenser(FlwNode * pNd);
    void        BuildDataDefn(DataDefnBlk & DDB);
    flag        DataXchg(DataChangeBlk & DCB);
    void        SetState(eScdMdlStateActs RqdState);
    double      SuctionP()                            { return m_dSuctionP; };
    void        SetSuctionP(double P)                 { m_dSuctionP=P; };

  public:
    FlwNode *   m_pNd;

    double      m_dQCool;
    double      m_dQCond;
    double      m_dQRqd;
    double      m_dSuctionP;
    double      m_dMinSatPress;

    byte        m_iMMEqpList;

    #if DOPBSMOOTHING  
    //CFlwNodeArray CoolLnks;
    CArray <CMSIO, CMSIO&> CoolLnks;
    double      m_dVapLoss;
    #endif

  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CFT_Mixer : public CMacroMdlBase
  {
  public:
    CFT_Mixer(FlwNode * pNd);

    void        BuildDataDefn(DataDefnBlk & DDB);
    flag        DataXchg(DataChangeBlk & DCB);

    void        Activate(CMacroMdl* pMMNd, pFlwNode pNd);
    void        Deactivate();

  public:
    FlwNode *   m_pNd;
    
    flag        m_bPartOfFlashTrain;
  };

//=========================================================================== 
//
//
//
//===========================================================================

template<class TYPE, class ARG_TYPE>
class CArraySrch : public CArray <TYPE, ARG_TYPE>
  {
  public:
    flag Exists(ARG_TYPE p) 
      { 
      for (int j=0; j<GetSize(); j++) 
        if (ElementAt(j)==p) 
          return True; 
      return False; 
      };
  };

typedef CArraySrch <CFT_Condenser*, CFT_Condenser*> CArrayCond;
typedef CArraySrch <CFT_Flash*, CFT_Flash*> CArrayFlsh;

//=========================================================================== 
//
//
//
//=========================================================================== 

#if WithTearVarBlkEdit
DEFINE_TAGOBJEDTDERIVED(FlshTrnTearVarBlk, TearVarBlkClassDef);
#else
DEFINE_TAGOBJDERIVED(FlshTrnTearVarBlk, TearVarBlkClassDef);
#endif

class DllImportExport FlshTrnTearVarBlk : public TearVarBlk
  {
  protected:
    CnvAttribute m_PCnv;
    FmtAttribute m_PFmt;
  public:
    FlshTrnTearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    FlshTrnTearVarBlk(pchar Tag_, pTaggedObject pAttach);
    virtual ~FlshTrnTearVarBlk();

    void DoConstruct();

    virtual void   OnEditConstruct();
    virtual void   OnEditDestroy();

  };

//=========================================================================== 
//
//
//
//=========================================================================== 


DEFINE_TAGOBJ(CMMFlashTrain);

class DllImportExport CMMFlashTrain : public CMacroMdl, public TearPosBlk
  {
  public:
    int            iIter;
    double         PMean;

    CArrayCond Cond;
    CArrayFlsh Flsh;

    static CMacroMdlIOArray MMIOs;

    CMMFlashTrain(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CMMFlashTrain();

    virtual CMacroMdlIO * ValidNd(flag fIsModel, pFlwNode pNd, int iIONo);
    virtual flag   ValidModel();

    virtual flag   PBInitialise();
    virtual void   PBTerminate();
    virtual void   PBPreJoinPressures();
    virtual void   PBPreEvalProducts();
    virtual void   PBPostEvalProducts();

    virtual TearPosBlk * GetTearPos() { return (TearPosBlk*)this; };

    virtual TearVarBlk* CreateVarBlk(char * Tag, pTaggedObject pAttach);

    virtual byte   TearVarType();
//    virtual flag   TearAdvanceReqd();
//    virtual flag   TearCnvTestReqd();
    virtual void   TearInitialiseOutputs(double EstimatePortion);
    virtual void   TearInputs2Outputs();
    virtual char * TearGetTag();
    virtual void   TearGetInputs(TearVarArray & TV);
    virtual void   TearGetOutputs(TearVarArray & TV);
    virtual void   TearSetOutputs(TearVarArray & TV);
  };

//==========================================================================
//
//
//
// ==========================================================================

#undef DllImportExport

#endif
