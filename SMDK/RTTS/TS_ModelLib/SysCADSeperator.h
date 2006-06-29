//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SysCADSeperator_H
#define  __SysCADSeperator_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "EfficiencySeperator.h"
#include "LoadBasedSeperator.h"
#include "PartitionSeperator.h"
#include "GlobalPartitionSeperator.h"

#ifdef __SysCADSeperator_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(UserMethods)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


//---------------------------------------------------------------------------

class SysCADSeperator : public MBaseMethod
  {
  public:

    //SysCADSeperator(TaggedObject * pNd);
	SysCADSeperator(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool PreStartCheck() { bInit = true; return true; }; 

  protected:

	// Seperator Method
	CSeperator::eMethod				m_Method;

	// Persistent Storage for each Seperation Type
	CSeperator_LoadBased			m_CSeperator_LoadBased;
	CSeperator_EfficiencyCurve		m_CSeperator_EfficiencyCurve;
	CSeperator_GlobalPartition		m_CSeperator_GlobalPartition;
	CSeperator_ComponentPartition	m_CSeperator_ComponentPartition;

	// The Selected Method
	CSeperator          *m_pCSeperator;

	// Flag to tell EvalProducts to initialise
	bool bInit;

	// Parameter for specifying 1 or 2 decks
	bool m_bTwoDecks;

  };

#endif