//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __QALCYCLONE_H
#define  __QALCYCLONE_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __QALCYCLONE_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class CSeparPSD
  {
  public:
    virtual void BuildDataFieldsConfig(MBaseMethod & Mdl) = 0;
    virtual void BuildDataFieldsResult(MBaseMethod & Mdl) = 0;
    virtual int CalcSeparation(MBaseMethod & Mdl, MStream & QI, MStream & Qof, MStream & Quf) = 0;
  };

class CSeparWhitenPSD : public CSeparPSD
  {
  public:
    CSeparWhitenPSD();
    virtual void BuildDataFieldsConfig(MBaseMethod & Mdl);
    virtual void BuildDataFieldsResult(MBaseMethod & Mdl);
    virtual int CalcSeparation(MBaseMethod & Mdl, MStream & QI, MStream & Qof, MStream & Quf);
  protected:
    double        dD50;
    double        dAlpha;
    //double        dBeta;
    CArray <double, double&> Eu;
  };

class CSeparPlittPSD : public CSeparPSD
  {
  public:
    CSeparPlittPSD();
    virtual void BuildDataFieldsConfig(MBaseMethod & Mdl);
    virtual void BuildDataFieldsResult(MBaseMethod & Mdl);
    virtual int CalcSeparation(MBaseMethod & Mdl, MStream & QI, MStream & Qof, MStream & Quf);
  protected:
    double CalcDP(double Qvc, double Phi);

    //input:
    double         m_NoOfCyclones;
    double         m_SIDi;
    double         m_SIDo;
    double         m_SIDu;
    double         m_SIDc;
    double         m_SIh;
    double         m_RefTemp;
    double         m_Factor1_d50;
    double         m_Factor2_Sharp;
    double         m_Factor3_DP;
    double         m_Factor4_S;
    double         m_Factor5_OFLiq;
    long           m_ViscOption;
    double         m_UserVisc;

    //internal:
    double         m_Di; //cm
    double         m_Do;
    double         m_Du;
    double         m_Dc;
    double         m_h;
    double         m_RfMem;   // This is a value which needs to be converged

    //results:
    double         m_Phi;
    double         m_Qvc;
    double         m_d50;
    double         m_S;
    double         m_H;
    double         m_m;
    double         m_Alpha;
    double         m_DP;
    double         m_Rv;
    double         m_Rs;
    //double         m_Scale;
    double         m_Rf; // This is a value which needs to be converged
    double         m_FdLiqVisc;


    CArray <double, double&> Eu;
  };

class QALCyclone : public MBaseMethod
  {
  public:
    QALCyclone(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();

  protected:
    CSeparWhitenPSD Whiten;
    CSeparPlittPSD Plitt;
    double        dNoOfCyclones;
    double        dFeedQm;
    double        dProdUfQm;
    double        dProdOfQm;
  };

#endif