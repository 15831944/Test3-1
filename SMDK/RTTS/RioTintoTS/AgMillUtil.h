//-- AgMillUtil.h.h: Definitions for AG Mill model helper classes -----------

#if !defined(AGMILLUTIL_H_413801E2_DE23_4A74_89D4_2F98AAAA0B55)
#define      AGMILLUTIL_H_413801E2_DE23_4A74_89D4_2F98AAAA0B55

#if _MSC_VER > 1000
#pragma once
#endif

using namespace RioTintoTS;

//namespace TS_MinProc
//{

// forward declaration to classes defined in this file
class AgMillConfig1;
class AgMillConfig2;
class AgMillParameters1;
class BallSizing1;
class BallSizing2;
class Appearance1;
class MaterialData1;
                   
// smart object pointers to each of the supported interfaces

typedef ObjectPtr< AgMillConfig1 > PAgMillConfig1;

typedef ObjectPtr< AgMillConfig2>
    PAgMillConfig2;

typedef ObjectPtr< AgMillParameters1 >
    PAgMillParameters1;

typedef ObjectPtr< BallSizing1 >
    PBallSizing1;

typedef ObjectPtr< BallSizing2 >
    PBallSizing2;

typedef ObjectPtr< Appearance1>
    PAppearance1;

typedef ObjectPtr< MaterialData1>
    PMaterialData1;

    
/****************************************************************************
*
*   class AgMillConfig1
*
*   Class to specify the physical dimensions and operating conditions
*   of an AG/SAG mill model for use by the VariableRatesAG model.
*
****************************************************************************/
class AgMillConfig1 //: public ModelBase
{

public:

    //-- Lifecycle ----------------------------------------------------------

    AgMillConfig1( ) //: ModelBase( ID_AgMillConfig1, this )
      { /* void */ }

    ~AgMillConfig1( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );


    //-- Implementation -----------------------------------------------------

    double SAG_NumMills;                // UNIT(11) ???
    double SAG_Diameter;                // UNIT(12)
    double SAG_Length;                  // UNIT(13)
    double SAG_Trunion_D;               // UNIT(58)
    double SAG_Feed_Cone_Angle;         // UNIT(59)
    double SAG_Disch_Cone_Angle;        // UNIT(60)
    double SAG_Grate_Size;              // UNIT(14)
    double SAG_Fine_Size;               // UNIT(15)
    double SAG_Grate_OAF;               // UNIT(61)
    double SAG_PP_OAF;                  // UNIT(16)
    double SAG_PP_Size;                 // UNIT(17)
    double SAG_Mean_Radial_Position;    // UNIT(62)
    double SAG_Fr_CriticalSpeed;        // UNIT(210)
    double SAG_Ball_Load_by_Volume;     // UNIT(18)
    double SAG_No_Load_Power;           // UNIT(216)
    double SAG_Feed_Solids_Density;     // Added DMT 26/1/03
};




/****************************************************************************
*
*   class AgMillConfig2
*
*   Class to specify the physical dimensions and operating conditions
*   of an AG/SAG mill model for use by the VariableRatesAG model.
*
****************************************************************************/
class AgMillConfig2 //: public ModelBase
{

public:

    //-- Lifecycle ----------------------------------------------------------

    AgMillConfig2( ) //: ModelBase( ID_AgMillConfig2, this )
      { /* void */ }

    ~AgMillConfig2( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );


    //-- Implementation -----------------------------------------------------

    double SAG_NumMills;
    double SAG_Diameter;
    double SAG_Belly_Length;
    double SAG_Trunion1_D;
    double SAG_Trunion2_D;
    double SAG_Feed_Cone_Angle;
    double SAG_Disch_Cone_Angle;
    double SAG_Grate_Size;
    double SAG_Fine_Size;
    double SAG_Grate_OAF;
    double SAG_Mean_Radial_Position;
    double SAG_PP_Size;
    double SAG_PP_OAF;
    double SAG_Fr_CriticalSpeed;
    double SAG_Ball_Load_by_Volume;
    double SAG_No_Load_Power;
    double SAG_Feed_Solids_Density;

    double SAG_Disch_Coeff;
    double SAG_Param_M1;
    double SAG_Param_M2;
    double SAG_Coarse_Factor;
    double SAG_Charge_Porosity;
    double SAG_Design_Load;
    double SAG_Net_Power_Factor;
    double SAG_CMS;
    double SAG_MMS;

    long   SAG_No_of_Knots;
    Vector SAG_Spline_Size_Knots;
    Vector SAG_Breakage_Rates_K;
};




/****************************************************************************
*
*   class AgMillParameters1
*
*   Class to hold model specific parameters used by the VariableRatesAG
*   mill model. Parameters are used to fit the predictions of the mill
*   discharge model, breakage rate model, and the power model.
*
****************************************************************************/
class AgMillParameters1 //: public ModelBase
{

public:

    //-- Lifecycle ----------------------------------------------------------

    AgMillParameters1( ) //: ModelBase( ID_AgMillParameters1, this )
      { /* void */ }

    ~AgMillParameters1( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );


    //-- Implementation -----------------------------------------------------

    long   SAG_No_of_Knots;                 // UNIT(31)

    Vector SAG_Spline_Size_Knots;  // UNIT(32) - UNIT(36)
    Vector SAG_Breakage_Rates;     // UNIT(37) - UNIT(41)

    double SAG_Disch_Coeff;                 // UNIT(46) ???
    double SAG_Param_M1;                    // UNIT(42)
    double SAG_Param_M2;                    // UNIT(43)
    double SAG_Coarse_Factor;               // UNIT(48) ???
    /* design load ??? */
    double SAG_Charge_Porosity;             // UNIT(52)
    double SAG_Net_Power_Factor;            // UNIT(217)
};




/****************************************************************************
*
*   class BallSizing1
*
*   Class to specify the physical properties and size distrubtion
*   of the ball charge for use by the VariableRatesAG mill model. 
*
****************************************************************************/
class BallSizing1 //: public ModelBase
{

public:

    //-- Lifecycle ----------------------------------------------------------

    BallSizing1( ) //: ModelBase( ID_BallSizing1, this )
      { /* void */ }

    ~BallSizing1( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );

    int nSize() { return nSize_; }

    //-- Implementation -----------------------------------------------------


    double Ball_SG;                 
    Vector Ball_Size_Distr;

private:

    int nSize_;

};



/****************************************************************************
*
*   class BallSizing2
*
*   Class to specify the physical properties and size distrubtion
*   of the ball charge for use by the VariableRatesAG mill model. 
*
****************************************************************************/
class BallSizing2 //: public ModelBase
{

public:

    //-- Lifecycle ----------------------------------------------------------

    BallSizing2( ) //: ModelBase( ID_BallSizing2, this )
      { /* void */ }

    ~BallSizing2( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );

    int nSize() { return nSize_; }

    //-- Implementation -----------------------------------------------------


    double Ball_SG;                 
    Vector Sieves;
    Vector Retained;

private:

    int nSize_;

};



/****************************************************************************
*
*   class Appearance1
*
*   Class to specify the high enery breakage appearance
*   of a mineral
*
****************************************************************************/
class Appearance1 //: public ModelBase
{

public:

    //-- Lifecycle ----------------------------------------------------------

    Appearance1( ) //: ModelBase( ID_Appearance1, this )
      { /* void */ }

    ~Appearance1( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );


    //-- Implementation -----------------------------------------------------

    Matrix TValues;
};




/****************************************************************************
*
*   class MaterialData1
*
*   Class to specify the physical properties of a mineral material.
*
****************************************************************************/
class MaterialData1 //: public ModelBase
{

public:


    //-- Lifecycle ----------------------------------------------------------

    MaterialData1( ) //: ModelBase( ID_MaterialData1, this )
      { /* void */ }

    ~MaterialData1( )
      { /* void */ }


    //-- Public interfaces --------------------------------------------------

    // Initializable: initialize the object
    //virtual INIT_STATUS Initialize( VARIANT vArgs );


    //-- Implementation -----------------------------------------------------

    double SG;                  // material specific gravity
    double A;                   // t10-Ecs relationship parameter
    double b;                   // t10-Ecs relationship parameter
    double ta;                  // tumble test abrasion parameter
    double alpha;               // rock fracture parameter
    double beta;                // rock fracture parameter

    PAppearance1 AppFunc;       // material appearance function

};




//}      // namespace TS_MinProc

#endif // AGMILLUTIL_H_413801E2_DE23_4A74_89D4_2F98AAAA0B55
