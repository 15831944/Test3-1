//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SysCADSurge_H
#define  __SysCADSurge_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "PlugFlowSurge.h"
//#include "FullyMixedSurge.h"

#ifdef __SysCADSurge_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(UserMethods)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


//---------------------------------------------------------------------------

class SysCADSurge : public MBaseMethod
  {
  public:

    //SysCADSurge(TaggedObject * pNd);
	SysCADSurge(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool PreStartCheck() { bInit = true; return true; }; 

  protected:

	// Surge Method
	CSurge::eMethod				m_Method;

	// Persistent Storage for each Seperation Type
	//CSurge_PlugFlow			m_CSurge_PlugFlow;
	//CSurge_FullyMixed		m_CSurge_FullyMixed;

	// The Selected Method
	CSurge          *m_pCSurge;

	// Flag to tell EvalProducts to initialise
	bool bInit;

  };

#endif