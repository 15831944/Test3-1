//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SR_CTRL_H
#define  __SR_CTRL_H

#ifndef __SR_EQN_H
  #include "sr_eqn.h"
#endif

#ifdef __SR_CTRL_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEqnSpRef
  {
  public:
    Strng          m_Name;
    int            m_SpcId;
    int            m_ProdTerm;
    int            m_ReactTerm;

    CEqnSpRef(LPCTSTR pName) { Clear(); m_Name=pName; };
    CEqnSpRef() { Clear(); };
    void Clear() { m_SpcId=m_ProdTerm=m_ReactTerm=-1; m_Name="";};
  
    flag SetUp(CReactionBlock & RB, CR_Eqn & Eq);
  };


typedef  CArray <CEqnSpRef, CEqnSpRef&> CEqnSpRefArray; 

//===========================================================================
//
//                           Stoichiometry Equations
//
//===========================================================================

class DllImportExport CEC_Equilibrium : public CR_EqnControl
  {
  public:
    DualDbl        m_ddKEq;//
    double         m_dK_Rt;

    CEC_Equilibrium(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn) { Clear(); };

    virtual eScdReactionBasis Basis() { return RctEquilibrium; };
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);

  };

DEFINE_CLASSBUILDER1(CEC_Equilibrium, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_EquilTXPoly : public CR_EqnControl
  {
  public:
    double         dK_Eq;
    double         dK_Act;
    double         dT_Off;
    double         dFnA;
    double         dFnB;
    double         dFnC;
    double         dFnD;
    double         dFnE;

    CEC_EquilTXPoly(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn) { Clear(); };

    virtual eScdReactionBasis Basis() { return RctEquilibrium; };
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);

  };

//DEFINE_CLASSBUILDER1(CEC_EquilTXPoly, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_Ratio : public CR_EqnControl
  {
  public:

    CEqnSpRefArray & m_Sp;
    CDVector       m_RatioEq;
    CDVector       m_RatioRt;

    DualDbl        m_ddKEq;
    double         m_dKRt;

    CEC_Ratio(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn), m_Sp(*(new CEqnSpRefArray)) { Clear(); };
    virtual ~CEC_Ratio() { delete &m_Sp; };
    virtual eScdReactionBasis Basis() { return RctEquilibrium;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);
  };

DEFINE_CLASSBUILDER1(CEC_Ratio, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_FracExt : public CR_EqnControl
  {
  public:
    CEC_FracExt(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn) { Clear(); };
    virtual ~CEC_FracExt() { Clear(); };
    virtual eScdReactionBasis Basis() { return RctExtent;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual void   BuildDataDefnEnd(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual bool   ExtentITOInitConds()      { return true; };
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);

  public:
    CEqnSpRef      m_Spc;
    double         m_dRqdExtentLcl;

    double         m_dRqdExtDT;
    double         m_dRefMoles;
  };

DEFINE_CLASSBUILDER1(CEC_FracExt, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_FinalConc : public CR_EqnControl
  {
  public:
    CEC_FinalConc(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn) { Clear(); };
    virtual ~CEC_FinalConc() { Clear(); };
    virtual eScdReactionBasis Basis() { return RctEquilibrium;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);

  public:
    CEqnSpRef      m_Spc;
    DualDbl        m_ddRqdConc; // g/L = kg/m^3
    double         m_dRqdTemp;  // K
    int            m_iFReactTerm;
    int            m_iFProdTerm;
  };

DEFINE_CLASSBUILDER1(CEC_FinalConc, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_MSFinalFrac : public CR_EqnControl
  {
  public:
    CEC_MSFinalFrac(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn) { Clear(); };
    virtual ~CEC_MSFinalFrac() { Clear(); };
    virtual eScdReactionBasis Basis() { return RctEquilibrium;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);

  public:
    CEqnSpRef      m_Spc;
    DualDbl        m_ddRqdFrac; // % mass frac in total
    int            m_iFReactTerm;
    int            m_iFProdTerm;
    flag           m_AsTotal;
  };

DEFINE_CLASSBUILDER1(CEC_MSFinalFrac, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CEC_MLFinalFrac : public CR_EqnControl
  {
  public:
    CEC_MLFinalFrac(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) : CR_EqnControl(Factory, pEqn) { Clear(); };
    virtual ~CEC_MLFinalFrac() { Clear(); };
    virtual eScdReactionBasis Basis() { return RctEquilibrium;};
    virtual void   Clear();
    virtual void   Parse(CRCTTokenFile &TF);
    virtual void   SetUp();
    virtual LPCTSTR GetRctString();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    // Methods Required by "Default ReactingMoles())"
    virtual void   CalcKs(double ProdMoles, double &EqK, double &K);
    virtual double ExtentReqd();
    virtual double ExtentActual(double ProdMoles=0.0);

  public:
    CEqnSpRef      m_Spc;
    DualDbl        m_ddRqdFrac; // % mass frac in phase
    int            m_iFReactTerm;
    int            m_iFProdTerm;
    flag           m_AsTotal;
  };

DEFINE_CLASSBUILDER1(CEC_MLFinalFrac, CR_EqnControl, pCR_Eqn)

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif

