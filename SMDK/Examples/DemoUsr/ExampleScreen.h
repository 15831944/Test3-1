//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __EXAMPLESCREEN_H
#define  __EXAMPLESCREEN_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __EXAMPLESCREEN_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class WhitenScreen : public MBaseMethod
  {
  public:
    WhitenScreen(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();

  protected:
    double        dD50;
    double        dAlpha;
    double        dBeta;
    double        dFeedQm;
    double        dProdQm0;
    double        dProdQm1;
    CArray <double, double&> Eu;
  };

#endif