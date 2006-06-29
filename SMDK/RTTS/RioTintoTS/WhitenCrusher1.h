#pragma once
#ifndef WHITENCRUSHER1_H_
#define WHITENCRUSHER1_H_

#include "TS.h"
#include "ObjectPtr.h"
#include "StreamInfo1.h"
#include "FlowStream1.h"

namespace RioTintoTS
{

// Forward declarations.
class WhitenCrusher1;

//! Smart pointer typedef for class \c TestStream.
typedef ObjectPtr<WhitenCrusher1> PWhitenCrusher1;


/****************************************************************************
*                       
*   class WhitenCrusher1;
*
*   UnitModel that for the JK style crusher model.
*
****************************************************************************/
class TS_API WhitenCrusher1
{

public:

    // Construct default
    WhitenCrusher1( );

    // Destroy resources
    ~WhitenCrusher1( );

	// Initialize model with configuration and parameters
	bool Initialize( PStreamInfo1 Config, const VectorView& paramVec );

    // Cause WhitenCrusher1 to calculate itself
    virtual bool CalculateModel( PFlowStream1 FeedStream );

	// Product streams
    PFlowStream1 Discharge;

	// Model calculated values
	Vector ModelOutput;
	Vector T10;

private:

    // Implementation

    int nType_;					// Number of size fractions
    int nSize_;					// Number of material types

    Vector C_;					// Classification function for each size
    Vector nomSize_;			// Nominal size of each size fraction

    PStreamInfo1 config_;		// Stream information structure
    Vector Content_;			// Machine contents
    Vector BreakFeed_;			// Material selected for breakage
    Vector BreakProd_;			// Breakage fragments of BreakFeed_;

    double k1;					// Classification function k1 param
    double k2;					// Classification function k2 param
    double k3;					// Classification function k3 param
    double Ecs;					// Specific energy of breakage

	double LLen;				// Liner length
	double LHr;					// Liner hours
	double ET;					// Eccentric throw
	double CSS;					// closed side setting

	double TPH;					// solids feed to crusher
	double F80;					// 80% passing size of feed
	double P80;					// 80% passing size of product

	double MotorPower;			// Installed motor power
	double NoLoadPower;			// Power when idling

	double GrossPower;			// Gross crushing power
	double NetPower;			// Net crushing power

	double Capacity;            // Calculated capacity (tph)
	double Utilization;         // Utilization wrt capacity

	double a0,a1,a2,a3,a4;		// k1 regression
	double b0,b1,b2,b3,b4,b5;	// k2 regression
	double c0;					// k3 regression
	double d0,d1,d2;			// Ecs regression
	double e0,e1;				// Capacity regression
	double f0,f1,f2;			// Reserved for future power model
};

}

#endif
