//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Model400Comminution_H
#define  __Model400Comminution_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "WhitenCrusher1.h"
#include "comminution.h"

//---------------------------------------------------------------------------

class CComminution_Model400 : public CComminution
{
public:
    // Constructor
	CComminution_Model400();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MBaseMethod &M,
		              MStream &Feed ,
	                  MStream &Product,
					  bool bInit);

protected:

    // Model 400 Parameters
	double m_dK1;
	double m_dK2;
	double m_dK3;
    double m_dEcs;

	//
	// Whiten Parameters
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
	// Whiten Outputs
	//

	double mo_dTPH;
	double mo_dCapacity;
	double mo_dUtilisation;
	double mo_dF80;
	double mo_dP80;
	double mo_dk1;
	double mo_dk2;
	double mo_dk3;
	double mo_dEcs;
	double mo_dNetPower;
	double mo_dGrossPower;


	// We use the WhitenCrusher to implement Model400
	RioTintoTS::WhitenCrusher1 Crusher;
	// System persistent feed data
	RioTintoTS::PFlowStream1   FeedStream;
	RioTintoTS::PStreamInfo1   MatInfo;

	// Mapping area for SysCAD to TS params
 	double m_Params[26];

};

#endif