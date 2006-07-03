//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __CSKOGasSep_H
#define  __CSKOGasSep_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#define BASIC1
#include "models.h"


#ifdef __CSKOGasSep_CPP
  #define DllImportExport DllExport
#elif !defined(SKO)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(CSKOGasSep);
class CSKOGasSep : public MN_Xfer
  {
  public:
    bool           m_bDataOK;

    static CArray<long, long&>     sm_GasId;
    static CArray<double, double&> sm_S;
    static CArray<double, double&> sm_Wt;

    double         m_dAbundFeed;
    double         m_dAbundRich;
    double         m_dAbundStrip;

    double         m_dK_In;
    double         m_dK_Rec;
    double         m_dK_L;
    double         m_dK_H;

    double         m_dTheta;
    double         m_dPhi;
    double         m_dAlpha;
    double         m_dBeta;
    double         m_dGamma;

    enum  eBetaEqns { BE_Name0, BE_Name1 }; // KCG to change
    eBetaEqns      m_eBetaEqn;
    double         m_dExponent;
    double         m_dMultiplier;

    double         m_dPRatio;

    double         m_ThetaLight;
    double         m_ThetaHeavy;

    CArray<double, double&> m_Beta;
    CArray<double, double&> m_J;        // Intermediate
    CArray<double, double&> m_Y;        // Calcd Mole Fraction In Lite Stream
    CArray<double, double&> m_RichFrac; // Measured Mass Fraction In Lite Stream

    double         AbundanceRatio(SpConduit & Q)
      {
      return -1;
      //double M1=Q.VMole[m_lSpCarrier];
      //double M2=0;
      //for (int i=0; i<m_lSpIsotope.GetSize(); i++)
      //  M2+=Q.VMole[m_lSpIsotope[i]];
      //return M1/GTZ(M2);
      }

  public:
    CSKOGasSep(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CSKOGasSep();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   ChangeMode(dword NewMode, dword OldMode);
    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual dword  ModelStatus();

    DEFINE_CI(CSKOGasSep, MN_Xfer, 4);


  };

//===========================================================================

#undef DllImportExport

#endif
