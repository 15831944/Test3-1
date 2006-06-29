//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HPRCComminution_H
#define  __HPRCComminution_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "comminution.h"
#include "OSM.h"

//---------------------------------------------------------------------------

class CComminution_HPRC : public CComminution
{
public:
    // Constructor
	CComminution_HPRC();

    // Data Entry Field Method
	void BuildDataFields(MDataDefn &DB);

    // Seperator Method Execution
	void EvalProducts(MBaseMethod &M,
		              MStream &Feed ,
	                  MStream &Product,
					  bool bInit);

  private:
    double* feedData;
    double* sizeSeries;
    int PrevNumSizes;

    bool   bUseDesignFeed;            
    double tphDesignFeed;           // design feed rate to HPRC     [tph]

    //see HP Rolls crusher model (osm_hprc.h)...
    // Machine Parameters
    double P_set;                   // Roll pressure        [bar]
    double D;                       // Roll diameter                [m]
    double W;                       // Roll width                   [mm]
    double rollSpeed;               // Roll speed                   [mm]
    long   RollModel;               // HPRC roll model type         {0-smooth,1-studded method1,2-studded method2}
    double oreSG;                   // ore specific gravity         [t/m^3] 
    double rhoc;                    // cake density                 [t/m^3]
    double kWnoLoad;                // no-load power draw           [kW]
    double kWtotal;                 // loaded power draw            [kW]
    //double kWnet;                   // net power draw               [kW]
    //double alphaC;                  // rock slip or draw angle      [deg]
    double forceFactor;             // specific grind. force const  [kN/bar]

    double splitB;                  // split to bypass              [0..1]
    double splitE;                  // split to edge-effect crusher [0..1]
    double MaxBypassSize;           // Max bypass size              [mm]

    //calculated parameters...
    double P;                       // Roll pressure                [bar]
    double tphFeed;                 // feed rate to HPRC            [tph]
    double tphRoll;                 // feed thru rolls              [tph]
    double tphPredictedRoll;        // predicted feed thru rolls    [tph]
    double tphMaxPredicted;         // max predicted feed thru rolls[tph]
    double tphBypass;
    double tphPreCrush;
    double tphEdgeCrush;
    double tphCenterCrush;
    double tphProduct;
    double FeedRatio;               // feed ratio: %pass90 / %pass5
    double passing118;              // %feed passing 1.18mm
    double Fsp;                     // specific grinding force
    bool   bUseCalculatedGap;       // Use calc or specified xg     {0,1}
    double xg_Set;                  // User specified working gap   [mm]
    double xg_Calculated;           // Working gap calculated       [mm]

    double dropA;                   // drop weight rock hardness A  [?]
    double dropB;                   // drop weight rock hardness b  [?]
    double packedA;                 // packed bed rock hardness A   [?]
    double packedB;                 // packed bed rock hardness b   [?]

};

#endif