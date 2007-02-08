//-- OSM_hprc.cpp -----------------------------------------------------------
//
// Classes Implemented:
//
//  OSM_HPRC
//
//  31/01/2000  Initial version.  
//   David T.
//
//  08/02/2000  Changed class HPRC to OSM_HPRC for consistency
//   David T.   with other classes in the OSM library.
//
//  09/02/2000  Many fixes to match the CSIRO regression equations
//   David T.   between tph/P/xg/kW to the supplied feed (and tph)
//


//-- External Definitions ---------------------------------------------------

#include "stdafx.h"
//#include <iostream.h>
#include <stdlib.h>
#include <math.h>

#include "OSM.h"
//#pragma optimize("", off)

//---------------------------------------------------------------------------
//
// OSM_HPRC IMPLEMENTATION
//
//   High-Pressure-Roll-Crusher Model. Largely hardwired
//   for single-component argyle material and for the
//   argyle studded roll machine.
//
// Members Defined:
//
//   OSM_HPRC( OSM_Type& )
//   ~OSM_HPRC( )
//
//   int calculate( )
//
//   ostream& operator<<( ostream&, OSM_HPRC& )    (non-member func)
//

//-- Initialize OSM_HPRC Model ----------------------------------------------

OSM_HPRC::OSM_HPRC( OSM_Type& sType )

  // Dimension HRPC component models/objects with sType

  : OSM_Crusher(sType),                // this crusher model
    pCrush(sType),                     // pre-crush model
    eCrush(sType),                     // edge-crush model
    cCrush(sType),                     // center-crush model
    bypass(sType),                     // bypass Stream
    pFeed(sType),                      // pre-crush feed Stream
    eFeed(sType),                      // edge-crush feed Stream
    cFeed(sType)                       // center-crush feed Stream
{
    //-- Machine Parameters -------------------------------------------------

    // (hardwired for argyle studded HPRC)

    D           = 2200;                // Roll diameter        [mm]
    W           = 1000;                // Roll effective width [mm]
    P           = 39;                  // Roll pressure        [bar]
    P_set       = 51;                  // Roll pressure        [bar]
    rollSpeed   = 2;                   // Roll speed           [m/s]
    kWnoLoad    = 200;                 // Unloaded power draw  [kW]
    alphaC      = 24;                  // rock draw angle      [deg]
    RollModel   = 0;                   // roll type (smooth)   {0,1,2}

    splitB      =  0.100;              // split to bypass      [%]
    splitE      =  0.200;              // split to edge-effect [%]
    MaxBypassSize = 63.0;              // Max bypass size      [mm]

    forceFactor = 115.5;               // specific force const [kN/bar]

    useCalculatedGap = 0;              // Use calc or specified xg     {0,1}
    xg_Set      = 20.0;                // User specified working gap   [mm]

    //-- Feed material characteristics --------------------------------------

    oreSG       = 2.70;                // Ore specific gravity [t/m^3]
    rhoc        = 2.30;                // Ore cake density     [t/m^3]

    dropA       = 85.8;                // Drop weight 'A' and 'b'
    dropB       = 0.440;               //   rock hardness parameters

    packedA     = 55.762325084229;     // Packed bed 'A' and 'b' hardness
    packedB     =  0.940284991594;     //   parameters (for lamphorite)

    //-- Connect component model objects ------------------------------------

    pCrush.setFeed( pFeed );           // Connect pre-crusher to its feed
    eCrush.setFeed( eFeed );           //   edge-crusher
    cCrush.setFeed( cFeed );           //   center-crusher
}

//-- Deallocate OSM_HPRC ----------------------------------------------------

OSM_HPRC::~OSM_HPRC( )
{
    /* void */
}

//-- Calculate OSM_HPRC model product() Stream ------------------------------

int OSM_HPRC::calculate( )
{
    //- feed characterization locals ----------------------------------------

    double feedP5;                     // size passing 5% of feed
    double feedP10;                    // size passing 10% of feed
    double feedP50;                    // size passing 50% of feed 
    double feedP80;                    // size passing 80% of feed
    double feedP90;                    // size passing 90% of feed
    //double passing118;                 // %feed passing 1.18 mm

    const double mb_k0  = 49.49261359; // Mt Barrow A.b correction
    const double mb_k1  =  0.02520741; //   regression
    const double mb_k2  = -6.10097343; //   parameters
    const double mb_sf  = 15.64387376; // Mt Barrow scale factor sum

    const double aPwr   = 0.50;        // A correction parameter
    const double bPwr   = 0.50;        // b correction parameter

    OSM_Vector abFactors( nSize() );   // A.b correction function

    double ab_sf;                      // A.b correction factor wtd. sum

    double dropA1;                     // corrected drop weight
    double dropB1;                     //   A,b hardness parameters

    double packedA1;                   // corrected packed bed
    double packedB1;                   //   A,b hardness parameters

    //- gap calculation locals ----------------------------------------------

    const double x0_k0  =  0.02352474; // dimensionless gap x0 parameter
    const double x0_k1  =  0.00047377; //   regression from passing118

    const double xg_k0  =  1.17441623; // roll speed gap correction ratio
    const double xg_k1  = -0.14534686; //   regression from passing118

    const double sg_k0  =  0.757578;   // smooth roll gap correction ratio
    const double sg_k1  = -0.004452;   //   regression from passing118
    
    const double q      = -0.42821425; // dimensionless gap constant

    //double Fsp;                        // specific grinding force
    //double logFsp;                     //  = log10(Fsp)

    double x0_D;                       // dimensionless gap parameter
    double xg_D;                       // dimensionless gap = xg/D

    //- roll throughput calculation locals ----------------------------------

    const double m0_k0  =  0.01977044; // dimensionless tph m0 parmater
    const double m0_k1  =  0.00042668; //   regression from passing118
    
    const double sm_k0  =  0.631313;   // smooth roll tph correction
    const double sm_k1  = -0.004608;   //   regression parameters

    const double s      = -0.17363167; // dimensionless tph constant
    
    double tphCSIRO;                   // CSIRO tph, corrected for roll type
    //double m0;                         // dimensionless tph parameter
    //double m;                          // dimensionless tph

    //- roll net kW calculation locals --------------------------------------

    const double g      =  0.07322;    // beta regression constants
    const double d      = -0.0067;     //   regression of Fsp value

    double beta;                       // torque displacement angle

    //- crushing energy calculations ----------------------------------------

    const double preA   =  0.54739238;
    const double preB1  =  1.77402203; // Ecs portioning regression
    const double preB2  = -2.83541474; //  parameters
    const double preB3  =  1.66095879;
    const double preD   =  800;

    double netEcs;                     // HPRC Ecs, based on kWnet
    double fracP, EcsP;                // pre crush Ecs portion and value
    double fracC, EcsC;                // edge crush Ecs portion and value
    double fracE, EcsE;                // center crush Ecs portion and value

    // Pre-Crusher Parameters

    double k1p;                        // pre crusher k1          [mm]
    double k2p;                        // pre crusher k2          [mm]
    double k3p;                        // pre crusher k3          [?]
    double t10p;                       // pre crusher t10         [?]

    // Edge-Effect Crusher Parameters

    double k1e;                        // edge crusher k1         [mm]
    double k2e;                        // edge crusher k2         [mm]
    double k3e;                        // edge crusher k3         [?]
    double t10e;                       // edge crusher t10        [?]

    // Center-Effect Crusher Parameters

    double k1c;                        // center crusher k1       [mm]
    double k2c;                        // center crusher k2       [mm]
    double k3c;                        // center crusher k3       [?]
    double t10c;                       // center crusher t10      [?]

  //-- 1. Test for material present in feed stream --------------------------

    // Obtain tphFeed if feed available
    if( hasFeed() )
        tphFeed = feed().sum();
    else
        tphFeed = 0.0;

    // Exit early if no material in feedStream
    if( tphFeed<=0.0 )
    {
        // Set empty product stream and exit
        product().clear();
        return 1;
    }

  //-- 2. Characterize HPRC feed --------------------------------------------

    // Create a cumulative passing size distribution of feed stream
    OSM_cSizeDist cDist( feed() );

    // Determine size passing percentages of feed stream
    feedP5  = cDist.sizePassing( 5.0 );
    feedP10 = cDist.sizePassing( 10.0 );
    feedP50 = cDist.sizePassing( 50.0 );
    feedP80 = cDist.sizePassing( 80.0 );
    feedP90 = cDist.sizePassing( 90.0 );

    // Determine feed ratio: size at 90% passes divided by  / size at 5% passes
    FeedRatio = feedP90 / feedP5;

    // Determine percentage of feed passing 1.18mm
    passing118 = cDist.percentPassing( 1.18 );

// Testing
//	passing118 = 19.41850;


  //-- 3. Determine correction factor for parameter A.b (hardness) ----------
  //      (correction based on size distribution in feed)

    // Calculate abFactors[] for each size fraction
    for( int i=0; i<nSize(); i++ )
        abFactors[i] = mb_k0 * ( 1-exp( -mb_k1 * (nominal()[i]-mb_k2) ) );

    // Get sum of abFactor[] weighted by feed in each size
    ab_sf = 0;
    for( int j=0; j<nComp(); j++ )
        ab_sf += dot( feed()[j], abFactors );
    ab_sf /= feed().sum();

    // Scale abScaleFactor to Mt. Barrow weighted sum
    ab_sf /= mb_sf;

    // Calculate corrected drop weight parameters
    dropA1 = dropA * pow( ab_sf, aPwr );
    dropB1 = dropB * pow( ab_sf, bPwr );

    // Calculate corrected pack bed A,b parameters
    packedA1 = packedA * pow( ab_sf, aPwr );
    packedB1 = packedB * pow( ab_sf, bPwr );
    
  //-- 4. Calculate Fsp --------------------------------

    // calculate tphRoll from feed and %bypass
    tphRoll = tphFeed * (1.0-splitB);
    tphBypass = tphFeed - tphRoll;

    //-- Calculate Fsp from tphRoll (CSIRO) --------------------------------

    // calculate tphCSIRO, feed to CSIRO hprc model
    if( RollModel==RM_Smooth )
        // correct tphRoll for smooth roll
        tphCSIRO = tphRoll * ( sm_k0 - sm_k1 * passing118 );
    else
        // CSIRO model is same as studded
        tphCSIRO = tphRoll;

    // Calculate dimensionless tphCSIRO (m)
    m0 = m0_k0 + m0_k1 * passing118;
    m = tphCSIRO / ( oreSG * (rollSpeed*3600) * (W/1000) * (D/1000) );

    switch (RollModel)
      {
      case RM_StudMethod1:
        // calculate logFsp, Fsp
        logFsp = ((m/m0)-1)/s;
        Fsp = pow( 10.0, logFsp );
        // Calculate P from Fsp
        P = ( Fsp * D * W ) / ( forceFactor * 1000.0 );
        break;
      case RM_Smooth:
      case RM_StudMethod2:
        Fsp = P_set * (forceFactor * 1000.0) / (D * W);
        logFsp = log10(Fsp);
        P = P_set;
        break;
      }
    
  //-- 5. Calculate working gap, predicted tph, and power (CSIRO) -----------

    //** (i) working gap prediction (xg) **

    // calculate dimensionless gap (corrected for roll speed)
    x0_D = x0_k0 + x0_k1 * passing118;
    xg_D = x0_D * ( 1 + q * logFsp ) * ( xg_k0 + xg_k1 * rollSpeed );
    
    if( RollModel==RM_Smooth )
        // correct CSIRO model gap for smooth roll 
        xg = ( sg_k0 + sg_k1 * passing118 ) * xg_D * D;
    else
        // gap in CSIRO model is studded gap
        xg = xg_D * D;
    xg_Calculated = xg;
    if (!useCalculatedGap)
      xg = xg_Set;

    // predicted throughput based on dimensions, etc
    tphPredictedRoll = xg * W * rhoc * rollSpeed *0.0036;
    if ( RollModel==RM_Smooth )
      {
      tphMaxPredicted = tphPredictedRoll;
      tphMaxPredicted += tphBypass; //take bypass into affect
      }
    else
      {
      // calculate MAXIMUM predicted throughput based on dimensions, etc
      //const double _K   =  76.630;
      double _K, M, BD;
      _K = 89.988 - (0.282 * P);
      BD = 0.0604 * log(FeedRatio) + 1.3908;
      M = 366.09 * log(BD) + _K;
      tphMaxPredicted = M * (D/1000.0) * (W/1000) * rollSpeed;
      tphMaxPredicted += tphBypass; //take bypass into affect
      }
    
    //** (ii) net kW prediction (kWnetStuds,kWnetSmooth) **
        
    beta = g + d * Fsp;
    kWnet = beta * 2 * (D/1000) * (W/1000) * rollSpeed * Fsp * 1000;
	// Line below removed as requested by Ted Bearman April 2004
    //kWnet *= 2.0; //for TWO motors
    if( kWnet<0 ) kWnet=0;
    
  //-- 6. Calculate crushing energy in each component crusher model ---------

    //-- Old version (a BUG!), worked well sometimes
    //  fracP = preA * ( 1-exp( -preB1*(feedP10*preD/D)
    //                          -preB2*(feedP10*preD/D)
    //                          -preB3*(feedP10*preD/D) ) );     

    fracP = preA * ( 1-exp( -preB1*(feedP10*preD/D)
                            -preB2*(feedP50*preD/D)
                            -preB3*(feedP80*preD/D) ) );     

    if( fracP < 0 ) fracP = 0;
   
    fracC = 1 - fracP;
    fracE = 0.80;

    kWtotal = kWnoLoad + kWnet;
    netEcs = kWnet / tphRoll;

    EcsP = fracP * netEcs;
    EcsC = fracC * netEcs;
    EcsE = fracE * EcsC;
    
  //-- 7. Calculate component crusher model parameters ----------------------

    // pre-crusher parameters
    k1p  = xg;
    k2p  = xg + D * ( 1 - cos((3.14159/180) * (alphaC/2)) );
    k3p  = 2.50;
    t10p = dropA1 * ( 1 - exp( -dropB1 * EcsP ) );

    // Use drop weight appearance data for preCrush
    pCrush.setAppearance( 1.4936, 0.9543, -0.3856, 23.72 );
    pCrush.setParams( k1p, k2p, k3p );
    pCrush.setT10( max(t10p, 1.0e-9) );

    // edge-crusher parameters
    k1e  = 0.64 * ( 1.20 * xg );
    k2e  = 1.20 * xg;
    k3e  = 2.30;
    t10e = dropA1 * ( 1 - exp( -dropB1 * EcsE ) );

    // Use drop weight appearance data for edgeCrush
    eCrush.setAppearance( 1.4936, 0.9543, -0.3856, 23.72 );
    eCrush.setParams( k1e, k2e, k3e );
    eCrush.setT10( max(t10e, 1.0e-9) );

    // center crusher parameters
    k1c  = 0.00;
    k2c  = xg;
    k3c  = 1.80;
    t10c = packedA1 * ( 1 - exp( -packedB1 * EcsC ) );

    // Use packed bed appearance data for centerCrush
    cCrush.setAppearance( 0.4919, 0.9984, -0.7347, 34.65 );
    cCrush.setParams( k1c, k2c, k3c );
    cCrush.setT10( max(t10c, 1.0e-9) );
    
  //-- 8. Execute HPRC component crusher models in order --------------------
  
    // Split HPRC feed() to bypass and precrush feed
    bypass.loadProduct( feed(), splitB );
    pFeed.loadProduct( feed(), (1-splitB) );

    // Adjust bypass to exclude sizes above MaxBypassSize
    double passingMaxBypass = cDist.percentPassing( MaxBypassSize );
    double tphBypOversize = tphBypass * (100.0 - passingMaxBypass) / 100.0;

	// 9/11/4 - Avoid Divide by Zero MRW
	double percentBypOversize = 0.0;
	if (tphBypass > 0.0)
		percentBypOversize = tphBypOversize / tphBypass;

    if ((tphBypOversize>0.001) && (bypass.nSize()>0))
      {// To maintain mass balance in bypass, 
       // "redistribute" proportianally the oversize in bypass to remaining smaller sizes...
      for( int comp=0; comp<nComp(); comp++ )
        {
        OSM_Vector& x = bypass[comp]; // Aliases to component vector
        double tphBypassSum = 0.0;
        for (int i=0; i<bypass.nSize()-1; i++) //pkh
          {
          double xi = x[i] / (tphBypass - tphBypOversize);
          if (bypass.sieve()[i]<MaxBypassSize)
            x[i] += (tphBypOversize*xi);
          else 
            x[i] = 0.0;
          tphBypassSum += x[i];
          }
        x[bypass.nSize()-1] = tphBypass - tphBypassSum;
        }
      }


    // Calculate precrush model
    pCrush.calculate( );

    // Split precrush product to edge and center crush feeds
    eFeed.loadProduct( pCrush.product(), splitE );
    cFeed.loadProduct( pCrush.product(), (1-splitE) );

    // Calculate edge and center crush models
    eCrush.calculate();
    cCrush.calculate();

    //temp for debug...
    //double d0 = pCrush.product().sum();
    //double d1 = eCrush.product().sum();
    //double d2 = cCrush.product().sum();
    //double d3 = bypass.sum();
    //double d4 = eFeed.sum();
    //double d5 = cFeed.sum();
    //double d6 = bypass.sum();

    // Combine bypass, edge and center crush products into HPRC product()
    product().load( eCrush.product() );
    product().add( cCrush.product() );
    product().add( bypass );
    
    // Return success
    return 1;
}

//-- Send text representation of OSM_HPRC to ostream& (for debugging) -------

ostream& operator<<( ostream& out, OSM_HPRC& H )
{
#ifdef NEVER
    out << "     diameter [mm] : " << H.D               << endl;
    out << "   eff. width [mm] : " << H.W               << endl;
    out << "       speed [m/s] : " << H.rollSpeed       << endl;
    out << "    pressure [bar] : " << H.P               << endl;
    out << "    kW noLoad [kW] : " << H.kWnoLoad        << endl;
    out << "    edge split [%] : " << H.splitE*100      << endl;
    out << "      roll type [] : "
        << ( H.RollModel==RM_Smooth ? "SMOOTH" : "STUDDED" )      << endl;
    out << endl;

    out << "    ore SG [t/m^3] : " << H.oreSG           << endl;
    out << "      rhoc [t/m^3] : " << H.rhoc            << endl;
    out << "      alphaC [deg] : " << H.alphaC          << endl;
    out << "   force factor [] : " << H.forceFactor     << endl;
    out << endl;

    out << "   HPRC feed [tph] : " << H.tphFeed         << endl;
    out << "   roll feed [tph] : " << H.tphRoll         << endl;
    out << "  bypass split [%] : " << H.splitB*100      << endl;
    out << "  working gap [mm] : " << H.xg              << endl;
    out << "        kWnet [kW] : " << H.kWnet           << endl;
    out << "      kWtotal [kW] : " << H.kWtotal         << endl;
    out << endl;    

    out << "        feed [tph] : " << H.tphFeed         << endl;
    out << "      bypass [tph] : " << H.bypass.sum()    << endl;
    out << "   pre-crush [tph] : " << H.pFeed.sum()     << endl;
    out << "  edge-crush [tph] : " << H.eFeed.sum()     << endl;
    out << "center-crush [tph] : " << H.cFeed.sum()     << endl;
    out << "     product [tph] : " << H.product().sum() << endl;
    out << endl;
#endif
    return out;
}


//-- end OSM_hprc.cpp -------------------------------------------------------

