//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SysCADBrunoScreen_H
#define  __SysCADBrunoScreen_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif


#include "BrunoScreen.h"

#ifdef __SysCADBrunoScreen_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(UserMethods)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

DEFINE_TRANSFERMETHOD(SysCADBrunoScreen)

class SysCADBrunoScreen : public MBaseMethod
  {
  public:
    SysCADBrunoScreen();
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool PreStartCheck() { bInit = true; return true; }; 

  protected:

	// The Bruno Screen Object
	TS_MinProc::BrunoScreen1  Screen;

	// System persistent feed data
	TS_MinProc::PFlowStream1   FeedStream;
	TS_MinProc::PSizingInfo1   SizInfo;
	TS_MinProc::PStreamInfo1   MatInfo;
 

	// Flag to tell EvalProducts to initialise
	bool bInit;

	// Initialisation of TS_ModelLib2 takes a vector of parameter data
	double		  dParams[10];
 
  };

#endif