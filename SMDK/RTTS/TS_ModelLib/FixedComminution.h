//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __FixedComminution_H
#define  __FixedComminution_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "comminution.h"
#include "Mill.h"

class CDischargePartition
{
public:
	// Name of PSD
    CString m_Name;

	// Operational CSS
	double m_dCSS;
	// CSS for test data
	double m_dTestCSS;
	// BYpass Selection for Feed < CSS
	bool m_bBypassOn;

    // PSD of Discharge at Test CSS
	CArray <double, double&> m_dDischarge;
};

//---------------------------------------------------------------------------

class CComminution_Fixed : public CComminution
{
public:
    // Constructor
	CComminution_Fixed();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MBaseMethod &M,
					  MStream &Feed ,
	                  MStream &Product,
					  bool bInit);

protected:
#ifdef OLD
	double m_dCSS; // Operational CSS
	double m_dTestCSS; // CSS for test data

	// PSD of Discharge at Test CSS
	CArray <double, double&> m_dDischarge;
#endif

	// Discharge selection list
	MDDValueLst *m_DDPSDDischarge;

	// Selected Discharge for Display
	long m_lDischargeSel;

	// Array of Discharge partitions
	CArray <CDischargePartition, CDischargePartition&> m_CDischarge;

};

#endif