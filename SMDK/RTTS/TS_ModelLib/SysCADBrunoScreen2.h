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

#define NumScreenParams 11

class SysCADBrunoScreen : public MBaseMethod
  {
  public:
    SysCADBrunoScreen(TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool PreStartCheck() { bInit = true; return true; }; 

  protected:

	// The Bruno Screen Objects
	TS_MinProc::BrunoScreen1  TopDeckScreen;
	TS_MinProc::BrunoScreen1  BottomDeckScreen;

	// System persistent feed data
	TS_MinProc::PFlowStream1   FeedStream;
	TS_MinProc::PSizingInfo1   SizInfo;
	TS_MinProc::PStreamInfo1   MatInfo;
 

	// Flag to tell EvalProducts to initialise
	bool bInit;

	// Parameter for specifying 1 or 2 decks
	bool bTwoDecks;

	// Storage for SysCAD Build Definition Data
	double TopDeckDataDefnParams[NumScreenParams];
	double BottomDeckDataDefnParams[NumScreenParams];

	// Double Array for TS_ModelLib calls to Initialize
	// This is populated by
	// calling SysCADParams::ConvertParamsToDoubles(struct Parameter* p, int n,double* d)
	double dParams[NumScreenParams];
 
  };

#endif