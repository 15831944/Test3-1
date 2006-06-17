//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HYPRODCLASSIF_H
#define  __HYPRODCLASSIF_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#ifndef __HYPRODCOMMON_H
#include "HyprodCommon.h"
#endif

#ifdef __HYPRODCLASSIF_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(AlcanU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

//DEFINE_SURGEMETHOD(Classifier)

enum eClassifModel
  {
    eMdl_Gravity,
    eMdl_Cyclone,
  };
enum eInputMethod
  {
    eIM_CutSize,
    eIM_Recovery,
    eIM_Underflow,
    eIM_Overflow,
    eIM_Model,
  };
enum eSharpnessEquation
  {
    eSE_Lynch,
    eSE_Plitt,
  };

class Classifier : public MBaseMethod, public HyprodPrecipHelper
  {
  friend class CRecoveryFn;
  public:
    Classifier(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
	virtual bool ExchangeDataFields();
    virtual bool ValidateDataFields();
    virtual void EvalProducts();
    virtual void ClosureInfo(MClosureInfo & CI);
    virtual bool PreStartCheck();

    bool CalcPrecipition(MVector & Feed, MVector & Prod);
    bool ComputeEfficiency(MVector & Feed, MVector & OF, MVector & UF, double Beta);
    bool CalcSeparation(MVector & Feed, MVector & OF, MVector & UF);
    bool CalcSeparationNoPSD(MVector & Feed, MVector & OF, MVector & UF);

  protected:
    bool          m_bOn;
    bool          m_bPrecipOn;
	bool		  m_bBrentSolverSelected;
    eClassifModel m_eModel;
    eInputMethod  m_eInputMethod;
    eSharpnessEquation m_eSharpEqn;
    double m_dD50;
    double m_dSharpness;
    double m_dRqdUFSolidsConc;
    double m_dRqdRecoveryFrac;
    double m_dRqdUFVolFlow;
    double m_dRqdOFSolidsTons;
    double m_dDiameter;
    double m_dHindSettlFac;
    MStream         m_QProd;

    double m_dRf;
    double m_dUFSolidsConc;
    double m_dUFVolFlow;
    double m_dOFSolidsConc;
    double m_dOFVolFlow;
    double m_dOFSolidsTons;
    double m_dCalcRecoveryFrac;
    double m_dRqdRecoveryFracUsed;
    double m_dD50Used;

    CArray <double, double&> Eu;
  };

#endif