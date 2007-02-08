//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __HPRCComminution_CPP
#include "HPRCComminution.h"
//#pragma optimize("", off)

//====================================================================================
//             HPRC Comminution
//====================================================================================

CComminution_HPRC::CComminution_HPRC()
{
  feedData = NULL;
  sizeSeries = NULL;
  PrevNumSizes = 0;

  bUseDesignFeed = 1;            
  tphDesignFeed = 1200.0 * 1000.0 / 3600;    // design feed rate to HPRC     [1000 tph --> kg/s]

  //-- Machine Parameters... (defaults for Argyle Crusher)
  P_set       = 3900.0;              // Roll pressure      [kPa] = 39bar
  D           = 2.2;                 // Roll diameter        [m]
  W           = 1.0;                 // Roll effective width [m]
  rollSpeed   = 2.0;                 // Roll speed           [m/s]
  kWnoLoad    = 200.0;               // Unloaded power draw  [kW]
  //alphaC      = 24;                  // rock draw angle      [deg]
  RollModel   = RM_Smooth;           // HPRC roll model type         {0-smooth,1-studded method1,2-studded method2}
  splitB      =  0.1;                // split to bypass      [frac] = 10%
  splitE      =  0.2;                // split to edge-effect [frac] = 20%
  MaxBypassSize = 0.063;             // Max bypass size      [m] = 63mm
  //forceFactor = 115.5;               // specific force const [kN/bar]
  //-- Feed material characteristics...
  oreSG       = 2700.0;              // Ore specific gravity [kg/m^3]
  rhoc        = 2300.0;              // Ore cake density     [kg/m^3]
  dropA       = 85.8;                // Drop weight 'A' and 'b'
  dropB       = 0.440;               //   rock hardness parameters

  packedA     = 55.762325084229;     // Packed bed 'A' and 'b' hardness
  packedB     =  0.940284991594;     //   parameters (for lamphorite)

  P                = 3900.0;         // Roll pressure      [kPa] = 39bar
  tphFeed          = 0.0;
  tphRoll          = 0.0;
  tphPredictedRoll = 0.0;
  tphMaxPredicted  = 2000.0;
  tphBypass        = 0.0;
  tphPreCrush      = 0.0;
  tphEdgeCrush     = 0.0;
  tphCenterCrush   = 0.0;
  tphProduct       = 0.0;
  FeedRatio        = 0.0;
  passing118       = 0.0;
  Fsp              = 0.0;
  bUseCalculatedGap= 1;
  xg_Set           = 0.02; //User specified working gap   [mm] = 20mm
  xg_Calculated    = 0.0;
  kWtotal          = 0.0;
}

//====================================================================================

void CComminution_HPRC::BuildDataFields(MDataDefn &DB)
{ 
  DB.ObjectBegin("TS_HPRC", "HPRC");

  DB.Page("HPRC");
  DB.Text("Requirements...");
  DB.Long("RollModel",  "", &RollModel, MF_PARAMETER|MF_SET_ON_CHANGE);
  if (RollModel==RM_Smooth)
    DB.Text("Using Smooth model");
  else if (RollModel==RM_StudMethod1)
    DB.Text("Using StuddedMethod1 model");
  else if (RollModel==RM_StudMethod2)
    DB.Text("Using StuddedMethod2 model");
  else
    DB.Text("Using Unknown model");
  DB.Double("RollPressure",    "",  &P_set,            MF_PARAMETER|((RollModel==RM_StudMethod2 || RollModel==RM_Smooth) ? 0 : MF_NO_VIEW),MC_P("kPa"));
  DB.CheckBox("UseDesignFeed", "",  &bUseDesignFeed,   MF_PARAMETER);
  DB.Double("DesignFeed",      "",  &tphDesignFeed,    MF_PARAMETER|(bUseDesignFeed ? 0 : MF_NO_VIEW),MC_Qm("t/h"));
  DB.Double("RollDiameter",    "",  &D,                MF_PARAMETER,MC_L("mm"));
  DB.Double("RollWidth",       "",  &W,                MF_PARAMETER,MC_L("mm"));
  DB.Double("RollSpeed",       "",  &rollSpeed,        MF_PARAMETER,MC_Ldt("m/s"));
  DB.Double("UnloadedPower",   "",  &kWnoLoad,         MF_PARAMETER|MF_INIT_HIDDEN,MC_Pwr("kW"));
  DB.Double("BypassSplit",     "",  &splitB,           MF_PARAMETER,MC_Frac("%"));
  DB.Double("MaxBypassSize",   "",  &MaxBypassSize,    MF_PARAMETER|MF_INIT_HIDDEN,MC_L("mm"));
  DB.Double("EdgeSplit",       "",  &splitE,           MF_PARAMETER,MC_Frac("%"));
  DB.CheckBox("UseCalculatedGap", "", &bUseCalculatedGap, MF_PARAMETER);
  DB.Show(bUseCalculatedGap==0);
  DB.Double("WorkingGap",      "",  &xg_Set,           MF_PARAMETER,MC_L("mm"));
  DB.Show();
  DB.Double("DropWeight_A",    "",  &dropA,            MF_PARAMETER|MF_INIT_HIDDEN,MC_None);
  DB.Double("DropWeight_B",    "",  &dropB,            MF_PARAMETER|MF_INIT_HIDDEN,MC_None);
  DB.Double("PackedBed_A",     "",  &packedA,          MF_PARAMETER|MF_INIT_HIDDEN,MC_None);
  DB.Double("PackedBed_B",     "",  &packedB,          MF_PARAMETER|MF_INIT_HIDDEN,MC_None);
  DB.Double("OreDensity",      "",  &oreSG,            MF_PARAMETER,MC_Rho("t/m^3"));
  DB.Double("CakeDensity",     "",  &rhoc,             MF_PARAMETER,MC_Rho("t/m^3"));
  DB.Text("");
  DB.Text("Results...");
  DB.Double("MaximumCapacity", "",  &tphMaxPredicted,  MF_RESULT,MC_Qm("t/h"));
  DB.Double("Feed",            "",  &tphFeed,          MF_RESULT|MF_INIT_HIDDEN,MC_Qm("t/h"));
  DB.Double("RollsFeed",       "",  &tphRoll,          MF_RESULT,MC_Qm("t/h"));
  DB.Double("Bypass",          "",  &tphBypass,        MF_RESULT|MF_INIT_HIDDEN,MC_Qm("t/h"));
  DB.Double("PreCrush",        "",  &tphPreCrush,      MF_RESULT|MF_INIT_HIDDEN,MC_Qm("t/h"));
  DB.Double("EdgeCrush",       "",  &tphEdgeCrush,     MF_RESULT|MF_INIT_HIDDEN,MC_Qm("t/h"));
  DB.Double("CenterCrush",     "",  &tphCenterCrush,   MF_RESULT|MF_INIT_HIDDEN,MC_Qm("t/h"));
  DB.Double("Product",         "",  &tphProduct,       MF_RESULT|MF_INIT_HIDDEN,MC_Qm("t/h"));
  DB.Double("Fsp",             "",  &Fsp,              MF_RESULT|MF_INIT_HIDDEN,MC_None);
  DB.Double("FeedRatio",       "",  &FeedRatio,        MF_RESULT|MF_INIT_HIDDEN,MC_None);
  DB.Double("FeedPassing118",  "",  &passing118,       MF_RESULT,MC_Frac("%"));
  DB.Double("CalcRollPressure","",  &P,                MF_RESULT,MC_P("kPa"));
  DB.Double("LoadedPower",     "",  &kWtotal,          MF_RESULT,MC_Pwr("kW"));
  DB.Double("CalcWorkingGap",  "",  &xg_Calculated,    MF_RESULT,MC_L("mm"));

  DB.ObjectEnd();


	}

//====================================================================================

void CComminution_HPRC::EvalProducts(MBaseMethod &M, MStream &Feed , MStream &Product , bool bInit )
	
{ 
	bool ModelErr=true;

	MIPSD & PSDin  = *Feed.FindIF<MIPSD>();
	MIPSD & PSDout = *Product.FindIF<MIPSD>();

	if ( IsNothing(PSDin)==false )
	{
		// Input Stream does have a PSD
		// which implies that some of the input stream species have associated size data

		long NumSizes = PSDin.getSizeCount();
		long NumComps = PSDin.getPSDVectorCount();


		//get copy of vector of size intervals in mm (smallest to largest)
		double* pSieveData = new double[NumSizes+1]; 
		PSDin.ExtractSizes(pSieveData, 1000.0 ); // SysCAD Stores sizes in SI units

		// Create some memory for the input and output PSD mass data
		double* pDataI = new double[NumSizes+1];
		double* pDataO = new double[NumSizes+1];
		double* pDataRevO = new double[NumSizes+1];

		if (feedData==NULL || sizeSeries==NULL || PrevNumSizes!=NumSizes)
		{
			if (feedData)
			{
				delete []feedData;
				feedData = NULL;
			}
			if (sizeSeries)
			{
				delete []sizeSeries;
				sizeSeries = NULL;
			}
			PrevNumSizes = NumSizes;
			if (PrevNumSizes>0)
			{
				feedData = new double[PrevNumSizes+1];
				sizeSeries = new double[PrevNumSizes+1];
			}
		}

		//Get sieve sieries data in required format
		//for (int i=0; i<(NumSizes-1); i++)	
		//	sizeSeries[i] = pSieveData[NumSizes-2-i];
		//sizeSeries[NumSizes-1] = 0.0; //ensure "smallest" size is zero
		long OSM_NumSizes = NumSizes+1; // OSM requires 1 more size for the zero
		for (int i=0; i<(NumSizes); i++)	
			sizeSeries[i] = pSieveData[NumSizes-1-i];
		sizeSeries[OSM_NumSizes-1] = 0.0; //ensure "smallest" size is zero

		//bUseCalculatedGap = 1; //force this...
		if (RollModel!=RM_Smooth && RollModel!=RM_StudMethod2  && RollModel!=RM_StudMethod1 )
			RollModel = RM_Smooth; //force to be one of these two models...

		for (int c=0; c<NumComps; c++)
		{

			// Populate the input mass vector
			// The underlying implementation only stores
			// fractions. Hence we cannot get access to the individual
			// masses for each size interval directly.
			// This method works out the actual mass
			// for each size interval and populates the specified vector
			// Mass is in kg/s
			//for (i=0; i<NumSizes; i++)	
			//	pDataI[i] = PSDin.getMass(c,NumSizes-1-i);
			double FdQmComp = 0.0;
			for (int i=0; i<NumSizes; i++)	 //pkh
			{
				pDataI[i+1] = PSDin.getMass(c,NumSizes-1-i);
				FdQmComp += pDataI[i+1];
			}
			pDataI[0] = 0.0;
			pDataI[OSM_NumSizes-1] = 0.0;

			double FdQmCompBug = Feed.getM(c); // This is not working for some reason?

			//if (FdQmComp>1.0e-9)
			{
				//convert to required format, t/h from largest to smallest sizes...
				for (int i=0; i<OSM_NumSizes; i++) //pkh
					feedData[i] = pDataI[i]/1000.0*3600.0;

				//-- Setup the flowsheet stream material type ---------------------------
				OSM_Type feedType;
				feedType.setSizes(OSM_NumSizes, sizeSeries);
				feedType.setComponents(1);

				//-- Setup a feed stream ------------------------------------------------
				OSM_Stream feed(feedType, feedData);  // nominate its type and data

				//-- Setup a HPRC crusher -----------------------------------------------
				OSM_HPRC hprc1(feedType);             // nominate its type
				hprc1.setRollModel(RollModel);
				hprc1.useCalculatedGap = bUseCalculatedGap;
				hprc1.xg_Set = xg_Set * 1000.0; //convert from m to mm
				hprc1.P_set = P_set / 100.0; //convert from kPa to bar
				hprc1.D = D * 1000.0; //convert from m to mm
				hprc1.W = W * 1000.0; //convert from m to mm
				hprc1.rollSpeed = rollSpeed;
				hprc1.kWnoLoad = kWnoLoad;
				hprc1.splitB = splitB;
				hprc1.splitE = splitE;
				hprc1.MaxBypassSize = MaxBypassSize * 1000.0; //convert from m to mm
				hprc1.dropA = dropA;
				hprc1.dropB = dropB;
				hprc1.packedA = packedA;
				hprc1.packedB = packedB;
				hprc1.oreSG = oreSG / 1000.0; //convert from kg/m^3 to t/m^3
				hprc1.rhoc = rhoc / 1000.0; //convert from kg/m^3 to t/m^3

				hprc1.setFeed(feed);              // set its feed stream

				//-- Calculate the hprc and output results ------------------------------
				hprc1.calculate();
				const double ProductTtl = hprc1.product().sum();

				tphFeed = hprc1.tphFeed * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphRoll = hprc1.tphRoll * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphPredictedRoll = hprc1.tphPredictedRoll * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphMaxPredicted = hprc1.tphMaxPredicted * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphBypass = hprc1.bypass.sum() * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphPreCrush = hprc1.pFeed.sum() * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphEdgeCrush = hprc1.eFeed.sum() * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphCenterCrush = hprc1.cFeed.sum() * 1000.0 / 3600.0; //converted from t/h to kg/s
				tphProduct = ProductTtl * 1000.0 / 3600.0; //converted from t/h to kg/s
				FeedRatio = hprc1.FeedRatio / 100.0;
				passing118 = hprc1.passing118 / 100.0;
				Fsp = hprc1.Fsp;
				P = hprc1.P * 100.0; //converted from bar to kPa
				xg_Calculated = hprc1.xg_Calculated / 1000.0;  //converted from mm to m
				kWtotal = hprc1.kWtotal;

				//--- get product info into SysCAD format--------------------------------
				const OSM_Stream& product = hprc1.product();   // access product Stream
				ModelErr = (fabs(tphProduct-tphFeed)>1.0e-12);

				if (tphProduct>1.0e-3)
				{
					//convert product size data from t/h to fractions and from smallest to largest sizes...
					double sum = 0.0;
					for (int i=0; i<OSM_NumSizes-1; i++) //pkh
					{
						pDataO[i] = (hprc1.product()[0][i] * 1000.0 / 3600.0);
						sum += pDataO[i];
					}
					pDataO[OSM_NumSizes-1] = max(0.0, FdQmComp - sum); //fix rounding err
				}
				else
				{//pass thru
					for (int i=0; i<OSM_NumSizes; i++) //pkh
						pDataO[i] = pDataI[i];
				}
			}
			//else
			//{//pass thru
			//	for (i=0; i<OSM_NumSizes; i++)
			//		pDataO[i] = pDataI[i];
			//}

			for (int i=0; i<NumSizes; i++)	//pkh
				//pDataRevO[i] = pDataO[NumSizes-1-i];
				pDataRevO[i] = pDataO[OSM_NumSizes-1-i];
			PSDout.ReplaceMassVector(pDataRevO,c);


		} // for

		// Cleanup
		delete []pSieveData;
		delete []pDataI;
		delete []pDataO;
		delete []pDataRevO;
	}

}

//====================================================================================
