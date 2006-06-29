//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SysCADComminution_H
#define  __SysCADComminution_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif


#ifdef __SysCADComminution_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(UserMethods)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

#include "FixedComminution.h"
#include "WhitenComminution.h"
#include "HPRCComminution.h"
#include "Model400Comminution.h"

//---------------------------------------------------------------------------

class SysCADComminution : public MBaseMethod
  {
  public:

    //SysCADComminution(TaggedObject * pNd);
    SysCADComminution(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool PreStartCheck() { bInit = true; return true; }; 

  protected:
 
	// Seperator Method
	CComminution::eMethod				m_Method;
  bool  m_bOrePropsExist; //true if OreProperties database found

	// Persistent Storage for each Seperation Type
	CComminution_Fixed			m_CComminution_Individual_Fixed;
	CComminution_Whiten	        m_CComminution_Whiten;
	CComminution_HPRC	        m_CComminution_HPRC;
	CComminution_Model400       m_CComminution_Model400;

	// The Selected Method
	CComminution          *m_pCComminution;

	// Flag to tell EvalProducts to initialise
	bool bInit;

	// System persistent feed data
	//TS_MinProc::PFlowStream1   FeedStream;
	//TS_MinProc::PSizingInfo1   SizInfo;
	//TS_MinProc::PStreamInfo1   MatInfo;
 
  };

#endif