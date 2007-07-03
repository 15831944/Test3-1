//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-07-04 09:53:44 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"

#endif
#include <sstream>
#include <iomanip>
//---------------------------------------------------------------------------





// Worlsely Model

const static int nClasses=25;
const static int nKernels=5;
const static double dBayerRefTemp=273.15+25;

double Viscosity_cp(MStream & s);




class CClassifier: public MBaseMethod
  {
  public:
    CClassifier(MUnitDefBase *pUnitDef,TaggedObject * pNd);
    virtual ~CClassifier(void);

    void Init();
    void BuildDataFields();
    bool ValidateDataFields();
    void EvalProducts();
    void ClosureInfo(MClosureInfo & CI);

    void DoClassifier(MStream &, MStream &);

  protected:
    bool bOnline;
    long iClassMethod;
    double dTankVol;
    double dViscosity;
    double tTerm;
    bool bCalcCut_Size;

    double m_dByPass;  
    double m_dSharp_Index;  
    double m_dCut_Size;     
    double m_dSlurry_split; 
    double m_dOver_pass;    
    double m_dL0;

    double m_dxS;
    double m_dEff;

    double L[nClasses+1];
    double Lav[nClasses+1];

    MStreamI Feed;

    long iPSD;
    void displayStream(MStream &ms, int scrn);
    void displayPSD(MStream &ms, int scrn);  double dd[8][26];

  };

