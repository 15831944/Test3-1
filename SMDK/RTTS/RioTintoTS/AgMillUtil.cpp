//-- AgMillUtil.cpp -------------------------------------------------------

//#include "StdAfx.h"
//#include "TS_ModelUtility.h"
//#include "VariantMap.h"
#include "AgMillUtil.h"

using namespace TS_ObjectLib;
using namespace TS_MinProc;
using namespace TS_Math;

using Initializable::INIT_STATUS;
using ValueRef::VALUE_TYPE;




namespace TS_ModelLib3
{
    extern "C"
    {
        // ID_AgMillConfig1
        // f5636808-cf11-4bb2-95ad-a15421833a39
        extern const InterfaceID ID_AgMillConfig1 =
        {
            0xf5636808, 0xcf11, 0x4bb2,
            {0x95, 0xad, 0xa1, 0x54, 0x21, 0x83, 0x3a, 0x39}
        };

        // ID_AgMillConfig2
        // 76e08387-1dbe-443e-9e7f-b192f1a74347
        extern const InterfaceID ID_AgMillConfig2 =
        {
            0x76e08387, 0x1dbe, 0x443e,
            {0x9e, 0x7f, 0xb1, 0x92, 0xf1, 0xa7, 0x43, 0x47}
        };

        // ID_AgMillParameters1
        // 69c4adf3-7bf9-4ca0-92df-3967ddd36bd1
        extern const InterfaceID ID_AgMillParameters1 = 
        {
            0x69c4adf3, 0x7bf9, 0x4ca0,
            {0x92, 0xdf, 0x39, 0x67, 0xdd, 0xd3, 0x6b, 0xd1}
        };

        // ID_BallSizing1
        // d5c171d0-473c-49cd-9dcf-35f04b92ab0c
        extern const InterfaceID ID_BallSizing1 = 
        {
            0xd5c171d0, 0x473c, 0x49cd,
            {0x9d, 0xcf, 0x35, 0xf0, 0x4b, 0x92, 0xab, 0x0c}
        };

        // ID_BallSizing2
        // 6372fee9-7746-4edd-98cc-03055ee3b4a7
        extern const InterfaceID ID_BallSizing2 = 
        {
            0x6372fee9, 0x7746, 0x4edd, 
            {0x98, 0xcc, 0x03, 0x05, 0x5e, 0xe3, 0xb4, 0xa7}
        
        };

        // ID_Appearance1
        // a47acc1c-8b46-43e5-8f2e-5c0a70b98c06
        extern const InterfaceID ID_Appearance1 = 
        {
            0xa47acc1c, 0x8b46, 0x43e5,
            {0x8f, 0x2e, 0x5c, 0x0a, 0x70, 0xb9, 0x8c, 0x06}
        };

        // ID_MaterialData1
        // 2d775764-4dc1-4857-ad49-483a19a30d06
        extern const InterfaceID ID_MaterialData1 = 
        {
            0x2d775764, 0x4dc1, 0x4857,
            {0xad, 0x49, 0x48, 0x3a, 0x19, 0xa3, 0x0d, 0x06}
        };
    }
}




/****************************************************************************
*
*   BallSizing1::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a BallSizing1 object
*
****************************************************************************/
INIT_STATUS BallSizing1::Initialize( VARIANT vArgs )
{
    INIT_STATUS RetVal;
    VariantMap  ArgMap;                 // Mapping of vArgs to ArgVec

    double      tBallSG;
    Vector      tSizing;

    ArgMap.SetSize( 2 );                // Setup ArgMap to point to
    ArgMap[0] = tBallSG;                //  temporary copies of 
    ArgMap[1] = tSizing;                //  BallCharge1 members    

    RetVal = INIT_FAIL;                 // Default is to have failed

    if( ArgMap.GetValues(vArgs) )       // Attempt to copy vArgs to ArgVec
    {
        if( tSizing.size() > 1 )        // Ensure correct number of values
        {
            Ball_SG = tBallSG;          // Commit new values
            Ball_Size_Distr = tSizing;  //  member data
            nSize_ = tSizing.size();

            RetVal = INIT_OK;	        // Succeeded
        }
    }
    return RetVal;
}




/****************************************************************************
*
*   BallSizing2::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a BallSizing2 object
*
****************************************************************************/
INIT_STATUS BallSizing2::Initialize( VARIANT vArgs )
{
    Vector  paramVec1;                  // Mill configuration values
    Vector  paramVec2;                  // Model parameter values
    Vector  vecSizes;                   // Spline vector - sizes
    Vector  vecRates;                   // Spline vector - breakage rates
    bool    argTypeOk = false;          // Result of argument tests

    VariantArray argArray( vArgs );     // use VariantArray to probe vArgs

    //-- Test - Number and type of arguments --------------------------------

    argTypeOk  = ValueRef(Ball_SG).get(   argArray[0] );
    argTypeOk &= ValueRef(Sieves).get(    argArray[1] );
    argTypeOk &= ValueRef(Retained).get(  argArray[2] );

    if( false == argTypeOk )
        goto initFailed;

    //-- Test arguments -----------------------------------------------------

    // Sieves and Retained should match in size
    nSize_ = Sieves.size();
    if( nSize_ != Retained.size() )
        goto initFailed;

    // Should be at least two size fractions
    if( nSize_ < 2 )
        goto initFailed;

    // Ok. At this point the object should be
    // configured as specified, and ready for
    // use by other objects.

    return INIT_OK;

initFailed:

    // Some error ocurred in the initialization of
    // this object. Indiate that the object should
    // be discarded.

    return INIT_FAIL;
}




/****************************************************************************
*
*   AgMillConfig1::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a AgMillConfig1 object
*
****************************************************************************/
INIT_STATUS AgMillConfig1::Initialize( VARIANT vArgs )
{
    INIT_STATUS RetVal;                 // Initialize() status value
    VariantMap  ArgMap;                 // Mapping of vArgs to ArgVec
    Vector      ArgVec;                 // Values extracted from vArgs

    ArgMap.SetSize( 1 );                // Setup ArgMap to point to
    ArgMap[0] = ArgVec;                 //  the values in ArgVec

    RetVal = INIT_FAIL;                 // Default is to have failed

    // Attempt to copy vArgs[] values into local variables
    if( ArgMap.GetValues(vArgs) )
    {
         // Ensure argument vector is current sized
        if( ArgVec.size()==16 || ArgVec.size()==15 )
        {
            // Can commit temporary values into member
            //  values of the AgMillConfig1 object

            SAG_NumMills             = ArgVec[0];
            SAG_Diameter             = ArgVec[1];
            SAG_Length               = ArgVec[2];
            SAG_Trunion_D            = ArgVec[3];
            SAG_Feed_Cone_Angle      = ArgVec[4];
            SAG_Disch_Cone_Angle     = ArgVec[5];
            SAG_Grate_Size           = ArgVec[6];
            SAG_Fine_Size            = ArgVec[7];
            SAG_Grate_OAF            = ArgVec[8];
            SAG_PP_OAF               = ArgVec[9];
            SAG_PP_Size              = ArgVec[10];
            SAG_Mean_Radial_Position = ArgVec[11];
            SAG_Fr_CriticalSpeed     = ArgVec[12];
            SAG_Ball_Load_by_Volume  = ArgVec[13];
            SAG_No_Load_Power        = ArgVec[14];

            if( ArgVec.size()==16 )
            {
                // Use supplied value of feed solids density
                SAG_Feed_Solids_Density = ArgVec[15];
            }
            else
            {
                // Default - do no modify feed solids density
                SAG_Feed_Solids_Density = 0.0;
            }

            // Initialization has succeeded
            RetVal = INIT_OK;
        }
    }
    return RetVal;
}




/****************************************************************************
*
*   AgMillConfig2::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a AgMillConfig2 object
*
****************************************************************************/
INIT_STATUS AgMillConfig2::Initialize( VARIANT vArgs )
{
    Vector  paramVec1;                  // Mill configuration values
    Vector  paramVec2;                  // Model parameter values
    Vector  vecSizes;                   // Spline vector - sizes
    Vector  vecRates;                   // Spline vector - breakage rates
                    
    int     nKnots;                     // Number of spline knots
    bool    argTypeOk = false;          // Result of argument tests

    VariantArray argArray( vArgs );     // use VariantArray to probe vArgs

    //-- Test - Number and type of arguments --------------------------------

    argTypeOk  = ValueRef(paramVec1).get(   argArray[0] );
    argTypeOk &= ValueRef(paramVec2).get(   argArray[1] );
    argTypeOk &= ValueRef(vecSizes).get(    argArray[2] );
    argTypeOk &= ValueRef(vecRates).get(    argArray[3] );

    if( false == argTypeOk )
        goto initFailed;

    //-- Test arguments -----------------------------------------------------

    // paramVec1 should have 17 elements
    if( paramVec1.size() < 17 )
        goto initFailed;

    // paramVec2 should have 9 elements
    if( paramVec2.size() < 9 )
        goto initFailed;

    // sizes and rates vectors should match
    nKnots = vecSizes.size();
    if( nKnots != vecRates.size() )
        goto initFailed;

    // number of knots should be at least 2
    if( nKnots<2 )
        goto initFailed;

    //-- Load values from arguments -----------------------------------------

    SAG_NumMills                = paramVec1[ 0];
    SAG_Diameter                = paramVec1[ 1];
    SAG_Belly_Length            = paramVec1[ 2];
    SAG_Trunion1_D              = paramVec1[ 3];
    SAG_Trunion2_D              = paramVec1[ 4];
    SAG_Feed_Cone_Angle         = paramVec1[ 5];
    SAG_Disch_Cone_Angle        = paramVec1[ 6];
    SAG_Grate_Size              = paramVec1[ 7];
    SAG_Fine_Size               = paramVec1[ 8];
    SAG_Grate_OAF               = paramVec1[ 9];
    SAG_Mean_Radial_Position    = paramVec1[10];
    SAG_PP_Size                 = paramVec1[11];
    SAG_PP_OAF                  = paramVec1[12];
    SAG_Fr_CriticalSpeed        = paramVec1[13];
    SAG_Ball_Load_by_Volume     = paramVec1[14];
    SAG_No_Load_Power           = paramVec1[15];
    SAG_Feed_Solids_Density     = paramVec1[16];

    SAG_Disch_Coeff             = paramVec2[0];
    SAG_Param_M1                = paramVec2[1];
    SAG_Param_M2                = paramVec2[2];
    SAG_Coarse_Factor           = paramVec2[3];
    SAG_Charge_Porosity         = paramVec2[4];
    SAG_Design_Load             = paramVec2[5];
    SAG_Net_Power_Factor        = paramVec2[6];
    SAG_CMS                     = paramVec2[7];
    SAG_MMS                     = paramVec2[8];

    SAG_No_of_Knots             = nKnots;
    SAG_Spline_Size_Knots       = vecSizes;
    SAG_Breakage_Rates_K        = vecRates;

    // Ok. At this point the object should be
    // configured as specified, and ready for
    // use by other objects.

    return INIT_OK;

initFailed:

    // Some error ocurred in the initialization of
    // this object. Indiate that the object should
    // be discarded.

    return INIT_FAIL;
}




/****************************************************************************
*
*   AgMillParameters1::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a AgMillParameters1 object
*
*   Arguments: vArgs = [ sizes, constants, mill parameters ] 
*
*   mill parameters is 7 element vector
*
****************************************************************************/
INIT_STATUS AgMillParameters1::Initialize( VARIANT vArgs )
{
    INIT_STATUS RetVal;                 // Initialize() status value
    VariantMap  ArgMap;                 // Mapping of vArgs to ArgVec

    int         NKnots1;                // Spline knots - from sizes[]
    int         NKnots2;                // Spline knots - from constants[]
    Vector      vecSizes;               // Spline vector - sizes
    Vector      vecRates;               // Spline vector - breakage rates
    Vector      vecParameters;          // Vector of other parameters

    ArgMap.SetSize( 3 );
    ArgMap[0] = vecSizes;               // Setup ArgMap to 
    ArgMap[1] = vecRates;               //   point to these 
    ArgMap[2] = vecParameters;          //     three vectors

    RetVal = INIT_FAIL;                 // Default is to have failed

    // Attempt to copy vArgs[] values into local variables
    if( ArgMap.GetValues(vArgs) )
    {
        // Count spline knots two different ways
        NKnots1 = vecSizes.size();
        NKnots2 = vecRates.size();

        // Ensure all vectors are correctly sized        
        if( NKnots1==NKnots2 && NKnots1>1 && vecParameters.size()==7 )
        {
            // Argument vectors are correctly sized
            //  Can commit these values into member
            //  values of the AgMillParameters1 object
    
            SAG_No_of_Knots       = NKnots1;
            SAG_Spline_Size_Knots = vecSizes;
            SAG_Breakage_Rates    = vecRates;

            SAG_Disch_Coeff      = vecParameters[0];
            SAG_Param_M1         = vecParameters[1];
            SAG_Param_M2         = vecParameters[2];
            SAG_Coarse_Factor    = vecParameters[3];
            /* DESIGN_LOAD ?     = vecParameters[4]; */
            SAG_Charge_Porosity  = vecParameters[5];
            SAG_Net_Power_Factor = vecParameters[6];

            // Initialization has succeeded
            RetVal = INIT_OK;
        }
    }
    return RetVal;
}




/****************************************************************************
*
*   Appearance1::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a Appearance1 object
*
****************************************************************************/
INIT_STATUS Appearance1::Initialize( VARIANT vArgs )
{
    INIT_STATUS RetVal;
    VariantMap  ArgMap;                 // Mapping of vArgs to ArgVec

    Matrix      tempMatrix;             // Temporary copy of TValues

    ArgMap.SetSize( 1 );                // Setup ArgMap to point to
    ArgMap[0] = tempMatrix;             //  temporary matrix of values

    RetVal = INIT_FAIL;                 // Default is to have failed

    // Attempt to copy vArgs[] values to temp locals
    if( ArgMap.GetValues(vArgs) )
    {
        // Ensure that the matrix retrieved from vArgs is 
        //   correctly sized for an appearance matrix

        if( tempMatrix.rowCount()==3 && tempMatrix.columnCount()==5 )
        {
            // Commit the temporary appearance matrix
            //  values into the Appearance1 object

            TValues = tempMatrix;

            // Initialization has succeeded
            RetVal = INIT_OK;
        }
    }
    return RetVal;
}




/****************************************************************************
*
*   MaterialData1::Initialize( )
*
*   Apply array of variant arguments to initialize the values
*   of a MaterialData1 object
*
****************************************************************************/
INIT_STATUS MaterialData1::Initialize( VARIANT vArgs )
{
    INIT_STATUS RetVal;
    VariantMap  ArgMap;                 // Mapping of vArgs to ArgVec

    double       tempSG;                // Temporary material SG
    double       tempA;                 // Temporary A parameter
    double       tempB;                 // Temporary B parameter
    double       tempTA;                // Temporary ta parameter
    long         objHandle;             // Handle to appearance object
    PAppearance1 tempAppFunc;           // Object resolved from objHandle
    
    ArgMap.SetSize( 5 );                // Setup ArgMap to point to
    ArgMap[0] = tempSG;                 //  temporary copies of the 
    ArgMap[1] = tempA;                  //  values to retrieve
    ArgMap[2] = tempB;                  //  from the vArgs array
    ArgMap[3] = tempTA;
    ArgMap[4] = objHandle;

    RetVal = INIT_FAIL;                 // Default is to have failed

    // Attempt to copy vArgs[] values to temp locals
    if( ArgMap.GetValues(vArgs) )
    {
        // Attempt to locate the object referred to by objHandle
        tempAppFunc = ObjectPool::Get( objHandle );

        // Ensure that an appearance function was found
        if( tempAppFunc.isValid() )
        {
            // Commit the temporary values for the material
            //  data parameters into the member variables 
            //   of the MaterialData1 object

            SG      = tempSG;
            A       = tempA;
            b       = tempB;
            ta      = tempTA;
            AppFunc = tempAppFunc;

            // Initialization has succeeded
            RetVal = INIT_OK;
        }
    }
    return RetVal;
}




//-- AgMillUtil.cpp ---------------------------------------------------------
