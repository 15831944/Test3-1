//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Seperator_H
#define  __Seperator_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class CPartition
{
public:
	// Name of PSD
    CString m_Name;

	// Water Split
	//double m_dWaterSplitToUS;

    // Partition Curve for Seperator
	CArray <double, double&> m_EU;
};

//---------------------------------------------------------------------------

class CPartitionArray
{
public:


    // Array of Partition Curves for Seperator
	// There is one curve for each possible PSD
	CArray <CPartition, CPartition&> m_Partition;
};

//===========================================================================

class CSeperator
{
public:

    // Available Seperator Methods
	enum eMethod { eMethod_LoadBased , eMethod_EfficiencyCurve ,
                   eMethod_GlobalPartition , eMethod_ComponentPartition };

    // Constructor
	CSeperator() { };

  // Data Entry Field Method
  virtual void BuildDataFields(MDataDefn &DB) = 0;

  // Seperator Method Execution
  virtual void EvalProducts( MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit) = 0;


};

//===========================================================================

#endif