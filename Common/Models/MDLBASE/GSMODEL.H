//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __GSMODEL_H
#define  __GSMODEL_H

#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __M_HXBASE_H
  #include "m_hxbase.h"
#endif
#ifndef __SP_REACT_H
  #include "sp_react.h"
#endif

#if defined(__GSMODEL_CPP)
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================
// Forward Declaration
class MdlNode;

// =========================================================================

_FWDDEF(GSModel);
_FWDDEF(GSAlphas)
_FWDDEF(GSGammas)

//============================================================================
//
// Alpha Gamma Model Stuff
//
//============================================================================

// Operational Modes
const byte GSO_General0         = 0;  // Orig GenModel
const byte GSO_V5               = 1;  // Version 5 Implementation
const byte GSO_MassFracSplit    = 10; // Individual special cases
const byte GSO_MassFlowSplit    = 11;
const byte GSO_PhMassFracSplit  = 12;
const byte GSO_PhMassFlowSplit  = 13;
const byte GSO_SpMassFracSplit  = 14;
const byte GSO_SpMassFlowSplit  = 15;
const byte GSO_PhaseRecovery    = 16;
const byte GSO_VolumeFlowSplit    = 17;
const byte GSO_PhVolumeFlowSplit  = 18;
const byte GSO_SpVolumeFlowSplit  = 19;

// Flow Modes
const byte GSA_QmDefault        = 0;
const byte GSA_QmDemand         = 1; // Low Priority
const byte GSA_QmPercSplit      = 2; // Med Priority
const byte GSA_QmMassFlow       = 3; // High Priority
const byte GSA_QmVolumeFlow     = 4; // High Priority
const byte GSA_QmInValid        = 255;

// Makeup Modes;
const byte GSA_MkDefault        = 0;
const byte GSA_MkMixture        = 1;
const byte GSA_MkDensity        = 2;
const byte GSA_MkLSRatio        = 3;
const byte GSA_MkPhaseComp      = 4;
const byte GSA_MkSpecieFrac     = 5;
const byte GSA_MkInValid        = 255;

// Phase Split Methods;
const byte GSPM_Total           = 0x01;
const byte GSPM_Individual      = 0x02;
const byte GSPM_Both            = 0x03;

typedef CArray<double, double> GSAlphaArray;

class DllImportExport GSIOAlpha //: public TaggedObject
  {
  friend class  GSModel;
  public:
    byte           IONo;
    byte           iQmMode;
    byte           iMkMode;
    byte           iOrder;
    byte           iPriority;
    Strng          Desc;
    double          Qm;
    double          QmErr;
    double          Qv;
    double          QvErr;
    double         m_FracSplit;
    double          DmndSplit;
    double          LSRatio;
    double          Density;
    double          SFrac;
    double          LFrac;
    double          VFrac;
    //double          PhFrac[MaxPhases];

    union
      {
      struct 
        {
        double          m_SSplt;
        double          m_LSplt;
        double          m_VSplt;
        };
      double          m_Splt[3];
      };
    double          m_PhSplt[MaxPhases];

    double          m_SQmRqd;
    double          m_LQmRqd;
    double          m_VQmRqd;
    double          m_PhQmRqd[MaxPhases];
    double          m_SQvRqd;
    double          m_LQvRqd;
    double          m_VQvRqd;
    double          m_PhQvRqd[MaxPhases];

    bool           m_bPhSpltHasValid[BOT_Last];
    bool           m_bPhQmRqdHasValid[BOT_Last];
    bool           m_bPhQvRqdHasValid[BOT_Last];

    GSAlphaArray  &Alphas;
    GSAlphaArray  &Splits;
    GSAlphaArray  &QmRqds;
    GSAlphaArray  &QvRqds;

    GSIOAlpha();
    ~GSIOAlpha();

    void           BuildDataDefnOrdPri(DataDefnBlk &DDB, rGSModel Mdl);
    void           BuildDataDefn(DataDefnBlk & DDB, rGSModel Mdl, flag Visible, int IONo, flag LastOne);
    GSIOAlpha & operator=(GSIOAlpha & A);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB, byte PhMethSpec);

    byte           QmMode(rGSModel Mdl);
    byte           MkMode(rGSModel Mdl);

  };

typedef CArray   <GSIOAlpha, GSIOAlpha&> GSIOAlphaArray;

// ---------------------------------------------------------------------------

const byte AG_SpecieMult  = 0;
const byte AG_EnergyMult  = 0;
const byte AG_InputScale  = 1;

class DllImportExport GSGamma
  {
  public:
    double         Val;
    byte           Is, Was;
  };

typedef CArray   <GSGamma, GSGamma&> GSGammaArray;

// ---------------------------------------------------------------------------

const byte GSG_Off        = 0;

// ---------------------------------------------------------------------------

typedef CArray   <byte, byte> GSIOIndexArray;

DEFINE_TAGOBJ(GSModel);
class DllImportExport GSModel : public TaggedObject
  {
  friend class GSIOAlpha;
  friend class GSModelBase;

  protected:

    MdlNode        &Nd;
    GSIOIndexArray &IOs;
    
    byte           IOId0;
    byte           IOIdN;
    byte           NProds;

    byte           iOperation;
    byte           m_iPhMethSpec;
    byte           iGMode;
    byte           iQmMode;
    byte           iMkMode;
    GSGammaArray   &Gammas;
    GSIOAlphaArray &IOAlpha;

    byte           iRecPh;
    byte           iRecStrm;
    double          dRecFrac;
    double          dRecComp;

    flag           bIOChanges;
    flag           bTrackStatus;
  
  public:
    static pGSModel Open(pchar TagIn, pTaggedObject pAttach);
    static void    Close(pGSModel &pGSM);

    
    GSModel(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    ~GSModel();

    //
    void           BuildDataDefnGen0(DataDefnBlk &DDB);
    flag           DataXchgGen0(DataChangeBlk & DCB);
    flag           ValidateDataGen0(ValidateDataBlk & VDB);
    int            FindIOEvalPriorityGen0(int *Priority, byte *QmMd, byte *MkMd);
    void           EvalProductsOutGen0(long RunMds, double Press, SpConduit &Sd);

    //
    void           BuildDataDefnV5(DataDefnBlk &DDB);
    flag           DataXchgV5(DataChangeBlk & DCB);
    flag           ValidateDataV5(ValidateDataBlk & VDB);
    int            FindIOEvalPriorityV5(int *Priority, byte *QmMd, byte *MkMd);
    void           EvalProductsOutV5(long RunMds, double Press, SpConduit &Sd);

    //
    void           BuildDataDefnSplit(DataDefnBlk &DDB);
    flag           DataXchgSplit(DataChangeBlk & DCB);
    flag           ValidateDataSplit(ValidateDataBlk & VDB);
    int            FindIOEvalPrioritySplit(int *I, GSIOAlpha**A);//, byte *QmMd, byte *MkMd);
    void           EvalProductsOutSplit(long RunMds, double Press, SpConduit &Sd);
    //
    void           PrepareDefinedData();
    void           BuildDataDefn(DataDefnBlk &DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);
    void           PostConnect(int IONo);
    void           PreDisConnect(int IONo);
    void           EvalProducts(long RunMds, double Press, SpConduit *pQf, CReactionBlock *pRB, double Flash_Press, CEnvironHX * pEHX);
    void           EvalProductsOut(long RunMds, double Press, SpConduit &Sd);
    void           MakeUpNodeTransferAvail(int JoinId);
    void           MakeUpNodeTransferReqd(int JoinId);
    //flag           MakeUpNodeEvalProducts(int JoinId, GSModel *pGSM);

  protected:
    void           CheckConfig();

    DEFINE_CI(GSModel, TaggedObject, 16);
  };

// ---------------------------------------------------------------------------

class DllImportExport GSModelBase
  {
  friend class GSModel;

  protected:
    flag            fEnabled;
    pGSModel        pGSM;
    MdlNode        *pNd;
    byte           MdlIOId0;
    byte           MdlIOIdN;

  public:
 
    GSModelBase(MdlNode *pAttach, byte IOId0=0, byte IOIdN=255) 
      { 
      pGSM=NULL; 
      pNd=pAttach; 
      fEnabled=False;
      MdlIOId0=IOId0;
      MdlIOIdN=IOIdN;      
      };
    ~GSModelBase() 
      { 
      delete pGSM; 
      };

    flag           Open();
    void           Close();
    void           SetIOIdRange(byte IOId0=0, byte IOIdN=255)
      { 
      MdlIOId0=IOId0;
      MdlIOIdN=IOIdN;
      if (pGSM)
        {
        pGSM->IOId0=IOId0;
        pGSM->IOIdN=IOIdN;
        }
      };

    pGSModel       operator() () { return fEnabled ? pGSM : NULL; };
    flag           Enabled() { return fEnabled && (pGSM!=NULL); };
    //byte           GMode(int iIONo=-1);
    byte           QmMode(int iIONo=-1);
    byte           MkMode(int iIONo=-1);
    GSGammaArray   &Gammas();
    GSIOAlphaArray &IOAlpha();
    
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=(isParmStopped | SetOnChange));
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag=NULL, char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);
    
    flag           ValidateData(ValidateDataBlk & VDB) { return Enabled() ? pGSM->ValidateData(VDB) : 0; };
    void           PostConnect(int IONo) { if (pGSM/*Enabled()*/) pGSM->PostConnect(IONo); };
    void           PreDisConnect(int IONo) { if (pGSM/*Enabled()*/) pGSM->PreDisConnect(IONo); };
    void           EvalProducts(/*int JoinId,*/ long  RunMds, double Press, SpConduit *pQf, CReactionBlock *pRB, double Flash_Press, CEnvironHX * pEHX) 
      { if (Enabled()) pGSM->EvalProducts(/*JoinId,*/ RunMds, Press, pQf, pRB, Flash_Press, pEHX); };
    void           EvalProductsOut(/*int JoinId,*/ long RunMds, double Press, SpConduit &Sd) 
      { if (Enabled()) pGSM->EvalProductsOut(/*JoinId,*/ RunMds, Press, Sd); };
    void           MakeUpNodeTransferAvail(int JoinId)
      { if (Enabled()) pGSM->MakeUpNodeTransferAvail(JoinId); };
    void           MakeUpNodeTransferReqd(int JoinId)
      { if (Enabled()) pGSM->MakeUpNodeTransferReqd(JoinId); };

  protected:
  };

// ---------------------------------------------------------------------------

inline byte GSIOAlpha::QmMode(rGSModel Mdl) { return (iQmMode==GSA_QmDefault) ? Mdl.iQmMode: iQmMode; };
inline byte GSIOAlpha::MkMode(rGSModel Mdl) { return (iMkMode==GSA_MkDefault) ? Mdl.iMkMode: iMkMode; };

// =========================================================================

#undef DllImportExport

#endif
