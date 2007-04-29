//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __LoadBasedSeperator_H
#define  __LoadBasedSeperator_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "../RioTintoTS/LoadBasedScreen1.h"
#include "seperator.h"

//---------------------------------------------------------------------------

class CSeperator_LoadBased : public CSeperator
{
public:
    // Constructor
	CSeperator_LoadBased();

    // Data Entry Field Method
	virtual void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	virtual void EvalProducts(MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit);

protected:


	//
	// Screen Parameters
	//
	double m_dTopApperture;
	double m_dTopLength;
	double m_dTopAngle;
	double m_dTopWidth;
	double m_dTopOpenFraction;
	double m_dTopBulkDensity;
	bool   m_bTopWetScreening;
    long   m_lTopAppetureShape;
	long   m_lTopMediaType;
	long   m_lTopDeckLocation;
	bool   m_bTopGravel;
    double m_dTopCustomFactor;
	double m_dTopWaterSplitToUS;

	double m_dBottomApperture;
	double m_dBottomLength;
	double m_dBottomAngle;
	double m_dBottomWidth;
	double m_dBottomOpenFraction;
	double m_dBottomBulkDensity;
	bool   m_bBottomWetScreening;
    long   m_lBottomAppetureShape;
	long   m_lBottomMediaType;
	long   m_lBottomDeckLocation;
	bool   m_bBottomGravel;
    double m_dBottomCustomFactor;
	double m_dBottomWaterSplitToUS;

    //
	// Calculated Outputs
	//
	double m_dTopQF;		//  total tph of feed
	double m_dTopFeed_OS;	//  fraction of feed > apperture
	double m_dTopFeed_US;	//  fraction of feed < apperture
	double m_dTopFeed_HS;	//  fraction of feed < half-apperture
	double m_dTopQU;		//  flowrate of undersize stream
	double m_dTopQO;		//  flowrate of oversize stream
	double m_dTopS;			//  apperture size
	double m_dTopT;			//  screen efficiency (at S)
	double m_dTopAF;		//  effective screen area
	double m_dTopD50;		//  fitted d50 of separation
	double m_dTopA;			//  basic capacity factor
	double m_dTopB;			//  over-size factor
	double m_dTopC;			//  half-size factor
	double m_dTopD;			//  deck location factor
	double m_dTopE;			//  wet screening factor
	double m_dTopF;			//  feed bulk density factor
	double m_dTopG;			//  load vs efficiency factor
	double m_dTopH;			//  opening shape factor
	double m_dTopJ;			//  screen open area factor
	double m_dTopK;			//  rock/gravel factor
	double m_dTopL;			//  humidity factor
	double m_dTopX;			//  user custom factor
	double m_dTopV;			//  screen load %

	double m_dBottomQF;		//  total tph of feed
	double m_dBottomFeed_OS;//  fraction of feed > apperture
	double m_dBottomFeed_US;//  fraction of feed < apperture
	double m_dBottomFeed_HS;//  fraction of feed < half-apperture
	double m_dBottomQU;		//  flowrate of undersize stream
	double m_dBottomQO;		//  flowrate of oversize stream
	double m_dBottomS;		//  apperture size
	double m_dBottomT;		//  screen efficiency (at S)
	double m_dBottomAF;		//  effective screen area
	double m_dBottomD50;	//  fitted d50 of separation
	double m_dBottomA;		//  basic capacity factor
	double m_dBottomB;		//  over-size factor
	double m_dBottomC;		//  half-size factor
	double m_dBottomD;		//  deck location factor
	double m_dBottomE;		//  wet screening factor
	double m_dBottomF;		//  feed bulk density factor
	double m_dBottomG;		//  load vs efficiency factor
	double m_dBottomH;		//  opening shape factor
	double m_dBottomJ;		//  screen open area factor
	double m_dBottomK;		//  rock/gravel factor
	double m_dBottomL;		//  humidity factor
	double m_dBottomX;		//  user custom factor
	double m_dBottomV;		//  screen load %

	// The Bruno Screen Objects
	RioTintoTS::LoadBasedScreen1  TopDeckScreen;
	RioTintoTS::LoadBasedScreen1  BottomDeckScreen;

	// System persistent feed data
	RioTintoTS::PFlowStream1   FeedStream;
	RioTintoTS::PStreamInfo1   MatInfo;

	// Mapping area for SysCAD to TS params
 	double m_TopDeckParams[13];
	double m_BottomDeckParams[13];

    SysCADParamsTable m_A;

};

#endif