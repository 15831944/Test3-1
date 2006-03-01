//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __EXAMPLE_H
#define  __EXAMPLE_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __EXAMPLE_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

DEFINE_MODEL_METHOD(Example)

class Example : public MBaseMethod
  {
  public:
    Example();
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();

  protected:
    double        dFeedQm;
    double        dRqdTemp;
    double        dFeedT;
    double        dDoubleParm1;
    double        dDoubleParm2;
    double        dDoubleParm3;
    double        dDoubleParm4;
    double        dDoubleParm5;
    long          iLongParm1;
    short         iShortParm1;
    bool          bBoolParm1;

    double        dMixT;
    double        dMixP;
    double        dMixQm;
    double        dMixQv;
    double        dMixRho;
    double        dMixCp;
  };

#endif