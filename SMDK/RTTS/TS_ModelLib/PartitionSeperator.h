//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PartitionSeperator_H
#define  __PartitionSeperator_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "seperator.h"

//---------------------------------------------------------------------------

class CSeperator_ComponentPartition : public CSeperator
{
public:
    // Constructor
	CSeperator_ComponentPartition();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit);

	void Seperate(CArray <CPartitionArray, CPartitionArray&> *pCPartition,
				  long in_PSD,
				  double  in_WaterSplitToUS,
		          MStream &in_Feed,
		          MStream &out_ProductA ,
		          MStream &out_ProductB);

protected:

	// Partition based on PSD selection list
	MDDValueLst *m_DDPSDPartition;

	// Selected PSD Partitions to for Display
	long m_lPartitionSel;

	// Species selection list
	MDDValueLst *m_DDSpeciesPartition;

	// Selected Species to Display
	long m_lSpeciesSel;

	// Number of Species with Size Data
	long m_NSpeciesWithSizeData;

	// Array of partitions
	CArray <CPartitionArray, CPartitionArray&> m_CTopPartition;
	CArray <CPartitionArray, CPartitionArray&> m_CBottomPartition;

	// Water Split
	double m_dTopWaterSplitToUS;
	double m_dBottomWaterSplitToUS;

};

#endif