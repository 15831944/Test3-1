//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __WhitenComminution_H
#define  __WhitenComminution_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "WhitenCrusher1.h"
#include "comminution.h"

//---------------------------------------------------------------------------

class CComminution_Whiten : public CComminution
{
public:
    // Constructor
	CComminution_Whiten();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MBaseMethod &M,
					  MStream	  &Feed ,
	                  MStream     &Product,
					  bool        bInit);

protected:

	//
	// Parameters
	// 

    double m_dCSS;                  
    double m_dLLen;                 
    double m_dET;                   
    double m_dLHr;                  
    double m_dMotorPower;           
    double m_dNoLoadPower;          
    double m_dA0;                   
    double m_dA1;                   
    double m_dA2;                   
    double m_dA3;                   
    double m_dA4;                   
    double m_dB0;                   
    double m_dB1;                   
    double m_dB2;                   
    double m_dB3;                   
    double m_dB4;                   
    double m_dB5;                   
    double m_dC0;                   
    double m_dD0;                   
    double m_dD1;                   
    double m_dD2;                   
    double m_dE0;                   
    double m_dE1;                   
    double m_dF0;                   
    double m_dF1;                   
    double m_dF2;                   

	//
	// Outputs
	//

	double m_dTPH;
	double m_dCapacity;
	double m_dUtilisation;
	double m_dF80;
	double m_dP80;
	double m_dk1;
	double m_dk2;
	double m_dk3;
	double m_dEcs;
	double m_dNetPower;
	double m_dGrossPower;


	// The TS Whiten Crusher
	RioTintoTS::WhitenCrusher1 Crusher;
	// System persistent feed data
	RioTintoTS::PFlowStream1   FeedStream;
	RioTintoTS::PStreamInfo1   MatInfo;

	// Mapping area for SysCAD to TS params
 	double m_Params[26];

};

#endif