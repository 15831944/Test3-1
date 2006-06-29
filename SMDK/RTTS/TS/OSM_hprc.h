#ifndef DMT_HPRC_H
#define DMT_HPRC_H

//-- hprc.h -----------------------------------------------------------------
//
// Classes defined:
//
//   OSM_HPRC
//


//HPRC methods...
const int RM_Smooth       = 0;
const int RM_StudMethod1  = 1;
const int RM_StudMethod2  = 2;

//---------------------------------------------------------------------------
//
// Class OSM_HPRC DEFINITION
//

class TS_API OSM_HPRC : public OSM_Crusher
{
  //friend class HPRCHelper;
  friend class CComminution_HPRC;
  public:

    // Initialize according to StreamType
    OSM_HPRC( OSM_Type& sType );

    // Deallocate
    ~OSM_HPRC( );

  //-- Access ---------------------------------------------------------------

    // Calculate the discharge sizing of the HPRC
    int calculate( );

    // Set HPRC model type
    void setRollModel(int newRollModel )
      { RollModel = newRollModel; }

    // Output text synopsis of HPRC model to ostream (debug)
    friend ostream& operator<<( ostream& out, OSM_HPRC& H );

  private:
  
  //-- Implementation -------------------------------------------------------

    // Machine Parameters

    double P_set;                   // Roll pressure        [bar]
    double D;                       // Roll diameter                [mm]
    double W;                       // Roll width                   [mm]
    double rollSpeed;               // Roll speed                   [mm]
    int    RollModel;               // HPRC roll model type         {0-smooth,1-studded method1,2-studded method2}
    int    useCalculatedGap;        // Use calc or specified xg     {0,1}
    double xg_Set;                  // User specified working gap   [mm]
    double oreSG;                   // ore specific gravity         [t/m^3] 
    double rhoc;                    // cake density                 [t/m^3]
    double kWnoLoad;                // no-load power draw           [kW]
    double kWtotal;                 // loaded power draw            [kW]
    double kWnet;                   // net power draw               [kW]
    double alphaC;                  // rock slip or draw angle      [deg]
    double forceFactor;             // specific grind. force const  [kN/bar]

    double splitB;                  // split to bypass              [0..1]
    double splitE;                  // split to edge-effect crusher [0..1]
    double MaxBypassSize;           // Max bypass size              [mm]

    //calculated values...
    double P;                       // Roll pressure                [bar]
    double tphFeed;                 // feed rate to HPRC            [tph]
    double tphRoll;                 // feed thru rolls              [tph]
    double tphBypass;               // feed thru internal bypass    [tph]
    double tphPredictedRoll;        // predicted feed thru rolls    [tph]
    double tphMaxPredicted;         // max predicted feed thru rolls[tph]
    double FeedRatio;               // feed ratio: %pass90 / %pass5
    double passing118;              // %feed passing 1.18mm
    double Fsp;                     // specific grinding force      [N/mm^2]
    double logFsp;                     //  = log10(Fsp)
    double xg;                      // Working gap                  [mm]
    double xg_Calculated;           // Working gap calculated       [mm]
    double m0;                         // dimensionless tph parameter
    double m;                          // dimensionless tph

    // Feed parameters

    double dropA;                   // drop weight rock hardness A  [?]
    double dropB;                   // drop weight rock hardness b  [?]
    double packedA;                 // packed bed rock hardness A   [?]
    double packedB;                 // packed bed rock hardness b   [?]

    // HPRC component models / objects

    OSM_Stream bypass;              // hprc bypass stream
    OSM_Stream pFeed;               // feed to pre-crusher
    OSM_Stream eFeed;               // feed to edge-crusher
    OSM_Stream cFeed;               // feed to center crusher

    OSM_Model400 pCrush;            // pre-crush model
    OSM_Model400 eCrush;            // edge-crush model
    OSM_Model400 cCrush;            // center-crush model
};


//-- end hprc.h -------------------------------------------------------------
#endif

