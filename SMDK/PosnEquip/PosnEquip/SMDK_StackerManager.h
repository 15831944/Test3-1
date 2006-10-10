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

class StackerManager : public MBaseMethod
  {
  public:
    StackerManager(MUnitDefBase * pUnitDef, TaggedObject * pNd);
	virtual ~StackerManager();
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool ValidateDataFields();  
    virtual void StartSolution(); // each Start of run
	virtual bool PreStartCheck(); 
    virtual bool    ExchangeDataFields();       // virtual method for manipulating data

	CString m_szText;

  protected:
 //   CStacker      S;
    bool          bOnLine;
	bool	m_bLoading;
  public:
};

//---------------------------------------------------------------------------
