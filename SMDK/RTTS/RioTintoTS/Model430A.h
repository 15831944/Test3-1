
//-- Model430.h: Definitions for Model430 class -----------------------------

#pragma once
#ifndef Model430_H_
#define Model430_H_

#include "TS.h"
#include "ObjectPtr.h"
#include "StreamInfo1.h"
#include "FlowStream1.h"
#include "AgMillUtil.h"

#define WithFortranModel430 1

#if WithFortranModel430

namespace RioTintoTS
{

// Forward declarations.
class Model430A;

//! Smart pointer typedef for class \c TestStream.
typedef ObjectPtr<Model430A> PModel430A;

/****************************************************************************
*                       
*   class Model430A;
*
*   UnitModel that for the JK style AS/AG model.
*
****************************************************************************/
class TS_API Model430A
{

public:

    // Construct default
    Model430A( );

    // Destroy resources
    ~Model430A( );

	// Initialize model with configuration and parameters
	bool Initialize( PStreamInfo1 inConfig, const VectorView& paramVec );

    // Cause WhitenCrusher1 to calculate itself
    virtual bool CalculateModel( PFlowStream1 FeedStream );

	// Model calculated values
	//Vector ModelOutput;

    // Object pointers to configuration information
    PAgMillConfig1      Config;
    PAgMillParameters1  Parameters;
    PBallSizing1        BallCharge;

    // Object pointers to product flowstreams

    PFlowStream1        Discharge;
    PFlowStream1        Load;


//private:

    //-- Implementation -----------------------------------------------------

   // Implementation

    int nType_;					// Number of size fractions
    int nSize_;					// Number of material types
    PStreamInfo1 config_;		// Stream information structure

    // Values output by the model (accessible by client)

    Vector      out_ModelOutput;
    Vector      out_D;
    Vector      out_R;
    double      out_Iterations;
    double      out_Error;
    double      out_WaterAddition;
    std::string out_Status;
    double      in_FeedSolidsDensity;

    // Model dimensions

    int nKnot;                  // Number of knots

    // Interface to FORTRAN code

	double UNIT[501];           // UNIT parameter vector
    double SIZE[50];            // GSIM sizing vector
    double SIZE_MAPPED[50];     // GSIM sizing vector mapped to represent size range of feed
    long   NTYPE;               // Number of ore types
    double ORESG[10];           // Specific gravity of each mineral
    double Param_A[10];         // T10-Ecs parameter A
    double Param_b[10];         // T10-Ecs parameter b
    double Param_ta[10];        // Abrasion parmeter ta
    double BSIZING[50];         // Ball size distribution
    long   NSPR;                // Number of spline knots
    double RSIZE[20];           // Spline knot - size
    double RSP[20];             // Spline knot - breakage rate
    double AMFEED[10][50];      // Mill feed sizing
    double AMDISC[10][50];      // Mill product sizing
    double AMFEED_MAPPED[10][50];// Mill feed sizing (mapped to size distribution that covers to top size in feed)
    double AMDISC_MAPPED[10][50];// Mill product sizing
    double AMLOAD[10][50];      // Mill load sizing
    double BLOAD[50];           // Ball load
    double TAIL[10][50];        // ??
    double D[50];               // Discharge rate x size
    double R[50];               // Breakage rate x size
    double AMDATA[10][50];      // Appearance function output
    double OUTPUT[20];
 


};

}
#endif

#endif

//-- Model430.h: Definitions for Model430 class -----------------------------

#ifdef NEVER

#if !defined(MODEL430A_H__NEED_A_GUID)
#define      MODEL430A_H__NEED_A_GUID

#if _MSC_VER > 1000
#pragma once
#endif

#include "TS_ObjectLib.h"
#include "TS_ModelLib1.h"
#include "TS_ModelUtility.h"
#include "VariantMap.h"
#include "AgMillUtil.h"
#include "MinProc.h"


/*  REFERENCE
 *
 *  MAP of model UNIT() structure
 *
 *

    double SAG_NumMills;                // UNIT(11) ???
    double SAG_Diameter;                // UNIT(12)
    double SAG_Length;                  // UNIT(13)
    double SAG_Grate_Size;              // UNIT(14)
    double SAG_Fine_Size;               // UNIT(15)
    double SAG_PP_OAF;                  // UNIT(16)
    double SAG_PP_Size;                 // UNIT(17)
    double SAG_Ball_Load_by_Volume;     // UNIT(18)
    double SAG_Calc_Load;               // UNIT(19)
    double SAG_Ball_Size_Distr;         // UNIT(20) - UNIT(25)
    double SAG_Ball_Equivalence;        // UNIT(26) - UNIT(30)

    double SAG_No_of_Knots;             // UNIT(31)
    double SAG_Spline_Size_Knots;       // UNIT(32) - UNIT(36)
    double SAG_Breakge_Rates;           // UNIT(37) - UNIT(41)

    double SAG_Param_M1;                // UNIT(42)
    double SAG_Param_M2;                // UNIT(43)
    double SAG_Param_A;                 // UNIT(44)
    double SAG_Param_b;                 // UNIT(45)
    double SAG_Param_Ecs;               // UNIT(46)
//  double SAG_Disch_Coeff;             // UNIT(46) ???
    double SAG_Param_ta;                // UNIT(47)
//  double SAG_Coarse_Factor;           // UNIT(48) ???

//  double SAG_Fr_CriticalSpeed;        // UNIT(50)
//  double SAG_Ball_SG;                 // UNIT(51)
//  double SAG_Charge_Porosity;         // UNIT(52)
//  double SAG_New_Feed_F80;            // UNIT(53)
//  double SAG_New_Feed_Tph;            // UNIT(54)
//  double SAG_Recycle_Tph;             // UNIT(55)
//  double SAG_Recycle_Ratio;           // UNIT(56)
//  double SAG_Net_Power_Factor;        // UNIT(57)
//  double SAG_Trunion_D;               // UNIT(58)
//  double SAG_Feed_Cone_Angle;         // UNIT(59)
//  double SAG_Disch_Cone_Angle;        // UNIT(60)
//  double SAG_Grate_OAF;               // UNIT(61)
//  double SAG_Mean_Radial_Position;    // UNIT(62)

    double SAG_OREBLKVPC;               // UNIT(173)


    double SAG_Fr_CriticalSpeed;        // UNIT(210)
    double SAG_Ball_SG;                 // UNIT(211)
    double SAG_Charge_Porosity;         // UNIT(212)
    double SAG_Vol_Total_Load;          // UNIT(213)
    double SAG_Total_Chrage_Density;    // UNIT(214)
    double SAG_Void_Fill;               // UNIT(215)
    double SAG_No_Load_Power;           // UNIT(216)
    double SAG_Net_Power_Factor;        // UNIT(217)
    double SAG_Gross_Power;             // UNIT(218)

    double Exp_Vol_Total_Load;          // UNIT(219)
    double Cal_Vol_Total_Load;          // UNIT(220)
    double SDs_Vol_Total_Load;          // UNIT(221)
    double Err_Vol_Total_Load;          // UNIT(222)

*/




namespace TS_MinProc
{




// import objectlib identifiers
using TS_ObjectLib::InterfaceID;
using TS_ObjectLib::ModelBase;
using TS_ObjectLib::Initializable::INIT_STATUS;
using TS_ObjectLib::PObject;
using TS_ObjectLib::PDerived;

// import math classes
using TS_Math::Matrix;
using TS_Math::Vector;
using TS_Math::VectorView;

// forward declare MinProc interface classes 

class Model430A;
extern "C" 
{
    // interface tags
    extern const InterfaceID ID_Model430A;
}

// templated smart pointers 
typedef PDerived< Model430A, &ID_Model430A> PModel430A;




/****************************************************************************
*
*   class Model430A
*
*   Adaptor class to FORTRAN coded AG/SAG mill model.
*   Model is an adapation of Model430 but allows for 
*   multiple ore components in its feed.
*
****************************************************************************/
class Model430A : public UnitModel1
{

public:

    //-- Lifecycle ----------------------------------------------------------

    // Default un-initialized object
    Model430A( );

    // Release resources
    ~Model430A( );

    //-- Public interfaces --------------------------------------------------

    // Initializable interface: initialize the object
    virtual INIT_STATUS Initialize( VARIANT vArgs );

    // Calculate the model for a given feed
    bool Model430A::CalculateModel( PFlowStream1 FeedStream );
    
    //-- Operations ---------------------------------------------------------

    // Access the data vector of the Sag Mill model
    VARIANT AccessData( VARIANT vArgs );

    // Object pointers to configuration information

    PAgMillConfig1      Config;
    PAgMillParameters1  Parameters;
    PBallSizing1        BallCharge;

    // Object pointers to product flowstreams

    PFlowStream1        Discharge;
    PFlowStream1        Load;

private:

    //-- Implementation -----------------------------------------------------

    // Handles to objects used by Model430A

    long        hConfig;        // Handles to specified
    long        hParameters;    //  objects in the
    long        hBallCharge;    //   ObjectPool::
    long        hDischarge;
    long        hLoad;

    // Values output by the model (accessible by client)

    Vector      out_ModelOutput;
    Vector      out_D;
    Vector      out_R;
    double      out_Iterations;
    double      out_Error;
    double      out_WaterAddition;
    std::string out_Status;
    double      in_FeedSolidsDensity;

    // Model dimensions

    int nSize;                  // Number of sizes
    int nType;                  // Number of types
    int nKnot;                  // Number of knots

    // Interface to FORTRAN code

	double UNIT[501];           // UNIT parameter vector
    double SIZE[50];            // GSIM sizing vector
    long   NTYPE;               // Number of ore types
    double ORESG[10];           // Specific gravity of each mineral
    double Param_A[10];         // T10-Ecs parameter A
    double Param_b[10];         // T10-Ecs parameter b
    double Param_ta[10];        // Abrasion parmeter ta
    double BSIZING[50];         // Ball size distribution
    long   NSPR;                // Number of spline knots
    double RSIZE[20];           // Spline knot - size
    double RSP[20];             // Spline knot - breakage rate
    double AMFEED[10][50];      // Mill feed sizing
    double AMDISC[10][50];      // Mill product sizing
    double AMLOAD[10][50];      // Mill load sizing
    double BLOAD[50];           // Ball load
    double TAIL[10][50];        // ??
    double D[50];               // Discharge rate x size
    double R[50];               // Breakage rate x size
    double AMDATA[10][50];      // Appearance function output
    double OUTPUT[20];

};




}      // namespace TS_ObjectLib

#endif // MODEL430A_H__NEED_A_GUID
#endif
