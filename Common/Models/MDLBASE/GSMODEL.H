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

#define WITHTHERMAL 01

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

// Flow Modes
const byte GST_None             = 0;
const byte GST_TReqd            = 1;
const byte GST_TRise            = 2;
const byte GST_TDrop            = 3;

// ---------------------------------------------------------------------------

typedef CArray<double, double> GSAlphaArray;

class DllImportExport GSIOAlpha
  {
  friend class  GSModel;
  public:
    GSIOAlpha();
    ~GSIOAlpha();

    void            BuildDataDefnOrdPri(DataDefnBlk &DDB, GSModel &  Mdl);
    void            BuildDataDefn(DataDefnBlk & DDB, GSModel &  Mdl, flag Visible, int IONo, flag LastOne);
    GSIOAlpha & operator=(GSIOAlpha & A);
    flag            DataXchg(DataChangeBlk & DCB);
    flag            ValidateData(ValidateDataBlk & VDB, byte PhMethSpec);

    byte            QmMode(GSModel &  Mdl);
    byte            MkMode(GSModel &  Mdl);

  public:
    byte            m_IONo;
    byte            m_iQmMode;
    byte            m_iMkMode;
    byte            m_iOrder;
    byte            m_iPriority;
    Strng           m_Desc;
    double          m_Qm;
    double          m_QmErr;
    double          m_Qv;
    double          m_QvErr;
    double          m_FracSplit;
    double          m_DmndSplit;
    double          m_LSRatio;
    double          m_Density;
    double          m_SFrac;
    double          m_LFrac;
    double          m_VFrac;

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
                    
    bool            m_bPhSpltHasValid[BOT_Last];
    bool            m_bPhQmRqdHasValid[BOT_Last];
    bool            m_bPhQvRqdHasValid[BOT_Last];

    GSAlphaArray  & m_Alphas;
    GSAlphaArray  & m_Splits;
    GSAlphaArray  & m_QmRqds;
    GSAlphaArray  & m_QvRqds;

  };

typedef CArray   <GSIOAlpha, GSIOAlpha&> GSIOAlphaArray;

// ---------------------------------------------------------------------------

class DllImportExport GSIOThermal
  {
  friend class  GSModel;
  public:
    GSIOThermal();
    ~GSIOThermal();

    void            BuildDataDefnOrdPri(DataDefnBlk &DDB, GSModel &  Mdl);
    GSIOThermal &   operator=(GSIOThermal & A);

    byte            Mode(GSModel * Mdl);

  public:
    byte            m_IONo;
    byte            m_iMode;
    byte            m_iOrder;
    byte            m_iPriority;
    Strng           m_Desc;
    double          m_TempReqd;
    double          m_TempRise;
    double          m_TempDrop;
    double          m_TempOut;
    bool            m_bLast;

  };

typedef CArray   <GSIOThermal, GSIOThermal&> GSIOThermalArray;

// ---------------------------------------------------------------------------

const byte AG_SpecieMult  = 0;
const byte AG_EnergyMult  = 0;
const byte AG_InputScale  = 1;

class DllImportExport GSGamma
  {
  public:
    double         m_Val;
    byte           m_Is, m_Was;
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
  friend class GSIOThermal;
  friend class GSModelBase;
  friend class CTNoneFinder;

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
    void           EvalProductsOutGen0(long SolveMethod, double Press, SpConduit &Sd);

    //
    void           BuildDataDefnV5(DataDefnBlk &DDB);
    flag           DataXchgV5(DataChangeBlk & DCB);
    flag           ValidateDataV5(ValidateDataBlk & VDB);
    int            FindIOEvalPriorityV5(int *Priority, byte *QmMd, byte *MkMd);
    void           EvalProductsOutV5(long SolveMethod, double Press, SpConduit &Sd);

    //
    void           BuildDataDefnSplit(DataDefnBlk &DDB);
    flag           DataXchgSplit(DataChangeBlk & DCB);
    flag           ValidateDataSplit(ValidateDataBlk & VDB);
    int            FindIOEvalPrioritySplit(int *I, GSIOAlpha**A);//, byte *QmMd, byte *MkMd);
    void           EvalProductsOutSplit(long SolveMethod, double Press, SpConduit &Sd);
    //
    void           PrepareDefinedData();
    void           BuildDataDefn(DataDefnBlk &DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);
    void           PostConnect(int IONo);
    void           PreDisConnect(int IONo);
    void           EvalProducts(long SolveMethod, double Press, SpConduit *pQf, CReactionBlock *pRB, double Flash_Press, CEnvironHX * pEHX);
    void           EvalProductsOut(long SolveMethod, double Press, SpConduit &Sd);
    void           MakeUpNodeTransferAvail(int JoinId);
    void           MakeUpNodeTransferReqd(int JoinId);

#if WITHTHERMAL 
    void           BuildDataDefnThermal(DataDefnBlk &DDB);
    flag           DataXchgThermal(DataChangeBlk & DCB);
    flag           ValidateDataThermal(ValidateDataBlk & VDB);
    int            FindIOEvalPriorityThermal(int *I, GSIOThermal**A);
    void           EvalProductsThermal();
#endif

  protected:
    void           CheckConfig();

    DEFINE_CI(GSModel, TaggedObject, 16);
 
  protected:
    MdlNode           & m_Nd;
    GSIOIndexArray    & m_IOs;
    
    int                 m_IOId0;
    int                 m_IOIdN;
    int                 m_NProds;

    byte                m_iOperation;
    byte                m_iPhMethSpec;
    byte                m_iGMode;
    byte                m_iQmMode;
    byte                m_iMkMode;
    GSGammaArray      & m_Gammas;
    GSIOAlphaArray    & m_IOAlpha;
    GSIOThermalArray  & m_IOThermal;

    byte                m_iRecPh;
    byte                m_iRecStrm;
    double              m_dRecFrac;
    double              m_dRecComp;

    flag                m_bIOChanges;
    flag                m_bTrackStatus;
    SpConduit         * m_pWrkCd;

#if WITHTHERMAL 
    bool                m_bThermal;
    bool                m_bAllowHX;
    double              m_TempIn;
    double              m_PressIn;
    double              m_PowerIn;
    double              m_FinalTempMin;
    double              m_FinalTempMax;
    bool                m_ChangeTst;
    double              m_ChangeNeg;
    double              m_ChangePos;
#endif

   };

// ---------------------------------------------------------------------------

class DllImportExport GSModelBase
  {
  friend class GSModel;

  public:
 
    GSModelBase(MdlNode *pAttach, int IOId0=0, int IOIdN=SpillIOId0-1) 
      { 
      m_pGSM=NULL; 
      m_pNd=pAttach; 
      m_fEnabled=False;
      m_MdlIOId0=IOId0;
      m_MdlIOIdN=IOIdN;      
      };
    ~GSModelBase() 
      { 
      delete m_pGSM; 
      };

    flag           Open();
    void           Close();
    void           SetIOIdRange(int IOId0=0, int IOIdN=SpillIOId0-1)
      { 
      m_MdlIOId0=IOId0;
      m_MdlIOIdN=IOIdN;
      if (m_pGSM)
        {
        m_pGSM->m_IOId0=IOId0;
        m_pGSM->m_IOIdN=IOIdN;
        }
      };

    pGSModel       operator() () { return m_fEnabled ? m_pGSM : NULL; };
    flag           Enabled() { return m_fEnabled && (m_pGSM!=NULL); };
    byte           QmMode(int iIONo=-1);
    byte           MkMode(int iIONo=-1);
    GSGammaArray   &Gammas();
    GSIOAlphaArray &IOAlpha();
    
    void           Add_OnOff(DataDefnBlk &DDB, dword Flags=(isParmStopped | SetOnChange));
    void           BuildDataDefn(DataDefnBlk &DDB, char* pTag=NULL, char* pTagComment=NULL, DDBPages PageIs=DDB_RqdPage, dword UserInfo=0);
    flag           DataXchg(DataChangeBlk & DCB);
    
    flag           ValidateData(ValidateDataBlk & VDB)        { return Enabled() ? m_pGSM->ValidateData(VDB) : 0; };
    void           PostConnect(int IONo)                      { if (m_pGSM) m_pGSM->PostConnect(IONo); };
    void           PreDisConnect(int IONo)                    { if (m_pGSM) m_pGSM->PreDisConnect(IONo); };
    void           EvalProducts(long  SolveMethod, double Press, SpConduit *pQf, CReactionBlock *pRB, double Flash_Press, CEnvironHX * pEHX)  
      { if (Enabled()) m_pGSM->EvalProducts(SolveMethod, Press, pQf, pRB, Flash_Press, pEHX); };
    void           EvalProductsOut(long SolveMethod, double Press, SpConduit &Sd) 
      { if (Enabled()) m_pGSM->EvalProductsOut(SolveMethod, Press, Sd); };
    void           MakeUpNodeTransferAvail(int JoinId)        { if (Enabled()) m_pGSM->MakeUpNodeTransferAvail(JoinId); };
    void           MakeUpNodeTransferReqd(int JoinId)         { if (Enabled()) m_pGSM->MakeUpNodeTransferReqd(JoinId); };

  protected:
    flag            m_fEnabled;
    pGSModel        m_pGSM;
    MdlNode       * m_pNd;
    int             m_MdlIOId0;
    int             m_MdlIOIdN;

  };

// ---------------------------------------------------------------------------

inline byte GSIOAlpha::QmMode(GSModel &  Mdl) { return (m_iQmMode==GSA_QmDefault) ? Mdl.m_iQmMode: m_iQmMode; };
inline byte GSIOAlpha::MkMode(GSModel &  Mdl) { return (m_iMkMode==GSA_MkDefault) ? Mdl.m_iMkMode: m_iMkMode; };

// ---------------------------------------------------------------------------

inline byte GSIOThermal::Mode(GSModel * Mdl) { return m_bLast && !Mdl->m_bAllowHX ? GST_None : m_iMode; };

// =========================================================================

#undef DllImportExport

#endif
