#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
#include <map>
using namespace std;
//---------------------------------------------------------------------------

class Stacker : public MBaseMethod
  {
  public:
    Stacker(MUnitDefBase * pUnitDef, TaggedObject * pNd);
	virtual ~Stacker();
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool ValidateDataFields();  
    virtual void StartSolution(); // each Start of run
	virtual bool PreStartCheck(); 
    virtual bool    ExchangeDataFields();       // virtual method for manipulating data

	double m_dSlewRate,m_dTravelRate,m_dLuftRate;
	double m_dSlewDir,m_dTravelDir,m_dLuftDir;
	double m_dInitSlew,m_dInitTravel,m_dInitLuft;
	double m_dSlewPos,m_dTravelPos,m_dLuftPos;

  protected:
    CStacker      S;
    bool          bOnLine;
	bool          bLoaded;
  public:
	  void UpdateStacker(void);
};

//---------------------------------------------------------------------------
