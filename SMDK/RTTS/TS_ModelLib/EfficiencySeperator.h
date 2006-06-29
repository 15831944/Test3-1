//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __EfficiencySeperator_H
#define  __EfficiencySeperator_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "seperator.h"

//---------------------------------------------------------------------------

class CSeperator_EfficiencyCurve : public CSeperator
{
public:
    // Constructor
	CSeperator_EfficiencyCurve();

    // Data Entry Field Method
	virtual void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	virtual void EvalProducts(MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit=false);

protected:

	// Calculate Efficiency Curve based on parameters
    void CalculateEfficiencyCurve( MIPSD &in_PSD , double in_D50 , double in_Alpha, double in_Beta, double in_C,
								   double &inout_BetaStar,
		                           CArray <double, double&> &out_Eu);


	// Iteratively find BetaStar
	double CalcBetaStar(double in_Alpha, double in_Beta );

	// Seperates the specified Feed stream into 2 products based on the
	// Efficiency Curve
	void CSeperator_EfficiencyCurve::Seperate(CArray <double, double&> &in_Eu,
											  double  in_WaterSplit,
											  MStream &in_Feed,
		                                      MStream &out_ProductA ,
		                                      MStream &out_ProductB);

	// Parameters
	double m_dTopAlpha;
	double m_dTopBeta;
	double m_dTopBetaStar; // Calculated
	double m_dTopD50;
	double m_dTopC;
	double m_dTopWaterSplitToUS;

	double m_dBottomAlpha;
	double m_dBottomBeta;
	double m_dBottomBetaStar; // Calculated
	double m_dBottomD50;
	double m_dBottomC;
	double m_dBottomWaterSplitToUS;

	// Calculated Efficiency Curves
	CArray <double, double&> m_TopEu;
	CArray <double, double&> m_BottomEu;
};

#endif