//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __EXAMPLETANKDYN_H
#define  __EXAMPLETANKDYN_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __EXAMPLETANKDYN_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class Tank : public MBaseMethod
  {
  public:
    Tank(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool PreStartCheck(CString & ErrMsg);
    virtual void EvalProducts();
    virtual void EvalDerivs(double StepSize);
    virtual void EvalIntegral(double StepSize); 

  protected:
    double        dFeedQm;
    double        dProdQm0;
    double        dProdQm1;
  };

#endif