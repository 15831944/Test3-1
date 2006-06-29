//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GlobalPartitionSeperator_H
#define  __GlobalPartitionSeperator_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "seperator.h"

//---------------------------------------------------------------------------

class CSeperator_GlobalPartition : public CSeperator
{
public:
    // Constructor
	CSeperator_GlobalPartition();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit);
protected:
	// Seperates the specified Feed stream into 2 products based on the
	// Efficiency Curve
	void Seperate(CArray <double, double&> &in_Eu,
			 double  in_WaterSplitToUS,
			 MStream &in_Feed,
		     MStream &out_ProductA ,
		     MStream &out_ProductB);

	// Partition selection list
	MDDValueLst *m_DDPSDPartition;

	// Selected Partition to for Display
	long m_lPartitionSel;

	// Array of partitions
	CArray <CPartition, CPartition&> m_CTopPartition;
	CArray <CPartition, CPartition&> m_CBottomPartition;

	// Water Splits
	double m_dTopWaterSplitToUS;
	double m_dBottomWaterSplitToUS;
};


#endif