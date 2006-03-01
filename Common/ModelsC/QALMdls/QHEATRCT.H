//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __QHEATRCT_H
#define  __QHEATRCT_H

#include "models.h"

#define DllImportExport 

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport QGibbsiteHOR : public CR_EqnHeat

  {
  public:
    QGibbsiteHOR(CR_EqnHeatFactory * Factory, pCR_Eqn pEqn) : CR_EqnHeat(Factory, pEqn) { };
    virtual ~QGibbsiteHOR() { };

    virtual double totDHf(SpModel *Mdl, double T, double P);

  };

DEFINE_CLASSBUILDER1(QGibbsiteHOR, CR_EqnHeat, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport QBoehmiteHOR : public CR_EqnHeat

  {
  public:

    QBoehmiteHOR(CR_EqnHeatFactory * Factory, pCR_Eqn pEqn) : CR_EqnHeat(Factory, pEqn) { };
    virtual ~QBoehmiteHOR() { };

    virtual double totDHf(SpModel *Mdl, double T, double P);

  };

DEFINE_CLASSBUILDER1(QBoehmiteHOR, CR_EqnHeat, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================
/*
class DllImportExport CEC_GibbACEq : public CR_EqnControl
  {
  protected:
    double         dRqdExtent;
    double         dACEq;
  
  public:
    CEC_GibbACEq(pCR_Eqn pEqn) : CR_EqnControl(pEqn) { Clear(); };
    virtual ~CEC_GibbACEq() { };
    virtual eScdReactionBasis Basis() { return RctEquilibrium;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   VerifyData();

    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
  };

DEFINE_CLASSBUILDER1(CEC_GibbACEq, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_BoehmACEq: public CR_EqnControl
  {
  protected:
    double         dRqdExtent;
    double         dACEq;

    double         dK0;
    double         dPercM45;

    double         dVLiq;    // kL/h
    double         dSSA;     // m^2/g
    double         dTSA;     // m^2/L
    double         dMSolids; // m^2/L
    double         dResTime; // Secs

    double         dACOut;
    double         dTRct;

  public:
    CEC_BoehmACEq(pCR_Eqn pEqn) : CR_EqnControl(pEqn) { Clear(); };
    virtual ~CEC_BoehmACEq() { };
    virtual eScdReactionBasis Basis() { return RctEquilibrium;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   VerifyData();

    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
  };

DEFINE_CLASSBUILDER1(CEC_BoehmACEq, CR_EqnControl, pCR_Eqn)
*/
//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport 

#endif

