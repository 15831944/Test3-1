//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __FILTER_PRESS_CPP
#include "Filter Press.h"
//#include <math>;
#pragma optimize("", off)

//====================================================================================

enum {	idFeed = 0,
		idWash,
		idFiltrate,
		idCake,
		idWashings,
		idVent };

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;					Name;			Id;			Rqd; Max;	CnId, FracHgt;  Options;
    { "Filter Press Feed",  "Feed",			idFeed,		1,	20,		0,    1.0f,  MIO_In |MIO_Material },
	{ "Wash Water",			"WashWater",	idWash,		0,	5,		0,    1.0f,  MIO_In | MIO_Material },
	{ "Filtrate",			"Filtrate",		idFiltrate,	1,	1,		0,    0.98f,  MIO_Out | MIO_Material },
	{ "Cake",				"Cake",			idCake,		1,	1,		0,    1.0f,  MIO_Out | MIO_Material },
	{ "Washings",			"Washings",		idWashings,	0,	1,		0,    0.98f,  MIO_Out | MIO_Material },
	{ "",					"Vent",			idVent,		0,	1,		0,    1.0f,  MIO_Out | MIO_Material },
    { NULL }
  };

static double Drw_FltPrss[] = { MDrw_Poly, -10,-5, -10,5, -8,5, -8,-5, -6,-5, -6,5, -4,5,
                     -4,-5, -2,-5, -2,5, 0,5, 0,-5, 2,-5, 2,5, 4,5, 4,-5, 6,-5,
                     6,5, 8,5, 8,-5, 10,-5, 10,5, -10,5, -10,-5, 10,-5, 0,-7,
                     -10,-5, MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(FilterPress, "Filter Press", DLL_GroupName)
void FilterPress_UnitDef::GetOptions()
  {
  SetDefaultTag("FP");
  SetDrawing("Tank", Drw_FltPrss);
  SetTreeDescription("Demo:Filter Press");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

FilterPress::FilterPress(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	eWashMethod = WM_WashRatio;
	dWashRatio = 0.0;
	eFiltrateMethod  = FM_SolidsToFiltrateFrac;
	dReqFiltSolConc  = 1.0;
	dReqSolidsToFiltrate = 0.0001;
	dReqCakeMoisture    = 0.02;
	dReqWashEfficiency      = 0.9;
	dCakeSolConc  = 0.0;
	dFiltSolConc  = dReqFiltSolConc;
	dWashEfficiency      = dReqWashEfficiency;
	dSolidsToFiltUsed= dNAN;
	dCakeSolids   = 0.0;
	dFiltSolids   = 0.0;
	nWashCompSpecie = 0;
	dWashCompEff = 0.0;

	dPressureDrop        = 0.0;
	dMembraneRes          = 100;
	dViscosity         = 0.001;
	dSpecCakeResistance       = 2.0e11;
	dArea        = 10.0;

	bTrackStatus    = true;
}

//---------------------------------------------------------------------------

void FilterPress::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

enum {	idDX_Filt2Washing = 0 };

void FilterPress::BuildDataFields()
{
	
	static MDDValueLst DDB0[]={
		{FM_SolidsToFiltrateFrac, "SolidsToFiltrateFraction"},
		{FM_FiltrateConc, "FiltrateConcentration"},
		{0}};

	static MDDValueLst DDB1[] = {
		{WM_ConstantEfficiency, "Constant Efficiency"},
		{WM_WashRatio, "Based on Wash Ratio"},
		{0}};
	DD.Text    ("");

	DD.Text		("Requirements");
	DD.Long		("Filtrate_Method", "Method", (long*)&eFiltrateMethod, MF_PARAMETER, DDB0 );
	DD.Show		(eFiltrateMethod == FM_SolidsToFiltrateFrac );
	if (false)//PrjFileVerNo()<48) //TODO: Fix this.
		DD.Double("", "RqdFiltSolids", &dReqSolidsToFiltrate, MF_PARAMETER, MC_Frac("%"));
	else
		DD.Double ("Rqd_Solids_To_Filtrate", "RqdSolToFilt", &dReqSolidsToFiltrate, MF_PARAMETER, MC_Frac("%"));
	DD.Show		(eFiltrateMethod == FM_FiltrateConc);
	DD.Double	("Rqd_FiltrateSolidsConc25","RqdFiltSolConc25", &dReqFiltSolConc, MF_PARAMETER, MC_Conc("g/L"));
	DD.Show		();
	//DD.Double	("Solids_ToFiltrateUsed", "SolToFiltUsed", &dSolidsToFiltUsed, MF_RESULT | MF_NO_VIEW, MC_Frac("%"));
	DD.Double	("Rqd_Cake_Moisture", "RqdCakeMoist", &dReqCakeMoisture, MF_PARAMETER, MC_Frac("%"));
	DD.Long		("Wash_Efficiency_Method", "WashEffMeth", (long*)&eWashMethod, MF_PARAMETER, DDB1);
	DD.Show		(eWashMethod == WM_ConstantEfficiency);
	DD.Double	("Rqd_Wash_Efficiency", "RqdWashEff", &dReqWashEfficiency, MF_PARAMETER, MC_Frac("%"));
	DD.Show		(eWashMethod == WM_WashRatio);
	DD.Double	("Rqd_Single_Pass_Wash_Efficiecny", "RqdSP_WashEff", &dReqWashEfficiency, MF_PARAMETER, MC_Frac("%"));
	DD.Show		();

	DD.Text		("Results");
	DD.Double	("Wash_Ratio", "WashRatio", &dWashRatio, MF_RESULT, MC_Frac("%"));
	DD.Double	("Wash_Efficiency", "WashEff", &dWashEfficiency, MF_RESULT, MC_Frac("%"));
	DD.Double	("Cake_Solids",	"CakeSolids", &dCakeSolids, MF_RESULT, MC_Frac("%"));
	DD.Double	("Filtrate_Solids", "FiltSolids", &dFiltSolids, MF_RESULT, MC_Frac("%"));
	DD.Double	("CakeSolidsConc@25", "CakeSolConc25", &dCakeSolConc, MF_RESULT, MC_Conc("g/L"));
	DD.Double	("FiltrateSolidsConc@25", "FiltSolConc25", &dFiltSolConc, MF_RESULT, MC_Conc("g/L"));
	DD.Long		("", "WashComp", (long*)&nWashCompSpecie, MF_PARAMETER, gs_MVDefn.DDLiqSpList());
	DD.Double	("", "CompWashEff", &dWashCompEff, MF_RESULT, MC_Frac("%"));

	DD.Text		("");
	DD.CheckBox	("TrackStatus", "", &bTrackStatus, MF_PARAMETER);

	DD.Show		(IsSolveBuffered);
	DD.Text		("");
	DD.CheckBox	("", "Filtrate2Washing", &bFilt2Washing, MF_PARAMETER);
	DD.CheckBox	("", "CakeDisch", &bCakeDischarge, MF_PARAMETER);
	DD.Text		("");
	DD.Double	("", "Press_Drop", &dPressureDrop, MF_RESULT, MC_DP("kPa"));
	DD.Double	("", "MembraneRes", &dMembraneRes, MF_PARAMETER, MC_); //TODO: Find out how to get the resistance in SMDK
	DD.Double	("", "Viscosity", &dViscosity, MF_PARAMETER, MC_Visc);
	DD.Double	("", "SpecCakeResistance", &dSpecCakeResistance, MF_PARAMETER, MC_);	//TODO: Find out how to get the resistance in SMDK
	DD.Double	("", "Area", &dArea, MF_PARAMETER, MC_Area("m^2"));

	//TODO: Find out about the rest of this stuff
}

//---------------------------------------------------------------------------

bool FilterPress::ExchangeDataFields()
{
	return false;
}

//---------------------------------------------------------------------------

bool FilterPress::PreStartCheck()
{
	return FlwIOs.Count[idFeed] > 0
		&& FlwIOs.Count[idCake] > 0
		&& FlwIOs.Count[idFiltrate] > 0;
}

//---------------------------------------------------------------------------

bool FilterPress::ValidateDataFields()
{
	//Ranges copied from old FILTPRSS.CPP implementation.
	dReqSolidsToFiltrate = Range(0.0, dReqSolidsToFiltrate, 0.3);
	dReqFiltSolConc = Range(0.0, dReqFiltSolConc, 5000.0);
	dReqCakeMoisture = Range(0.0, dReqCakeMoisture, 0.99);
	dReqWashEfficiency = Range (0.0, dReqWashEfficiency, 1.0);
	if (dMembraneRes < 0)
		dMembraneRes = 0;
	if (dViscosity < 0)
		dViscosity = 0;
	if (dSpecCakeResistance < 0)
		dSpecCakeResistance = 0;
	if (dArea < 0)
		dArea = 0;
	return true;
}

//---------------------------------------------------------------------------

void FilterPress::EvalProducts()
{
	try {
		int idPressNote = 0, idWashNote = 1;
		MStream QFeed;
		FlwIOs.AddMixtureIn_Id(QFeed, idFeed);
		MStream QWashWater;
		bool bWashWaterConnected;
		if (bWashWaterConnected = (FlwIOs.getCount(idWash) > 0))
			FlwIOs.AddMixtureIn_Id(QWashWater, idWash);

		MStream QUnwashedCake = QFeed;
		QUnwashedCake.ZeroMass();

		MStream&	QFiltrate	= FlwIOs[FlwIOs.First[idFiltrate]].Stream;
					QFiltrate	= QFeed;
		MStream&	QCake		= FlwIOs[FlwIOs.First[idCake]].Stream;
					QCake		= QFeed;

		double dFeedSolidMass = QFeed.Mass(MP_Sol);
		double dFeedLiquidMass = QFeed.Mass(MP_Liq);

		bool bWashingsConnected = FlwIOs.Count[idWashings] > 0;

		//First off: The filtrate & UnwashedCake:

		/*Equations Solved using Mathematica 6.0, under the constrainsts of:
		 - Conservation of mass
		 - Cake Moisture Content
		 - Filtrate Solid Concentration [Either in % or g/L]
			- When using g/L, assume solid and liquid densities don't change [The (Aq) conc's don't]
		*/
		double dSolConcConstFac, dLiqConcConstFac;
		bool bTooWet;
		switch (eFiltrateMethod) {
		case FM_SolidsToFiltrateFrac:
			dSolConcConstFac = dReqSolidsToFiltrate;
			dLiqConcConstFac = dReqSolidsToFiltrate;
			bTooWet = dFeedSolidMass / NZ(dFeedSolidMass + dFeedLiquidMass) < dReqSolidsToFiltrate;
			break;
		case FM_FiltrateConc:
			dSolConcConstFac = dReqFiltSolConc / QFeed.Density(MP_Sol, C_2_K(25));
			dLiqConcConstFac= dReqFiltSolConc / QFeed.Density(MP_Liq, C_2_K(25));
			bTooWet = dFeedSolidMass / NZ(QFeed.Volume(MP_SL, C_2_K(25))) < dReqFiltSolConc;
			break;
		}
		/*Situations where variables can be out of range:
		 - Mass comes in too wet - required loss causes all mass to be sent to filtrate
		 - Mass comes in too dry - mass is drier than required moisture frac.
		*/
		double dFiltSolidFactor, dFiltLiquidFactor, dCakeSolidFactor, dCakeLiquidFactor;
		if (bTooWet)
		{
			SetNote(idPressNote, "Input feed too wet: All feed solids sent to filtrate");
			dFiltSolidFactor = dFiltLiquidFactor = 1;
			dCakeSolidFactor = dCakeLiquidFactor = 0;
		}
		else if (dFeedLiquidMass / NZ(dFeedSolidMass + dFeedLiquidMass) < dReqCakeMoisture)
		{
			SetNote(idPressNote, "Input feed too dry: All feed liquids sent to Cake");
			dFiltSolidFactor = dFiltLiquidFactor = 0;
			dCakeSolidFactor = dCakeLiquidFactor = 1;
		}
		else
		{
			dFiltSolidFactor = (((dFeedLiquidMass * (dReqCakeMoisture - 1) + dFeedSolidMass * dReqCakeMoisture) * dLiqConcConstFac)
									/ NZ(dReqCakeMoisture * dLiqConcConstFac - (dReqCakeMoisture - 1)*(dSolConcConstFac-1))) / dFeedSolidMass;
			dFiltLiquidFactor = - (((dFeedLiquidMass * (dReqCakeMoisture - 1) + dFeedSolidMass * dReqCakeMoisture) * (dSolConcConstFac - 1))
									/ NZ(dReqCakeMoisture * dLiqConcConstFac - (dReqCakeMoisture - 1)*(dSolConcConstFac - 1))) / dFeedLiquidMass;

			dCakeSolidFactor = 1 - dFiltSolidFactor;
			dCakeLiquidFactor = 1 - dFiltLiquidFactor;
		}

		for (int i = 0; i < gs_MVDefn.Count(); i++)
			if (gs_MVDefn[i].IsSolid())
			{
				QFiltrate.M[i] = QFeed.M[i] * dFiltSolidFactor;
				QUnwashedCake.M[i] = QFeed.M[i] * dCakeSolidFactor;
			}
			else if (gs_MVDefn[i].IsLiquid())
			{
				QFiltrate.M[i] = QFeed.M[i] * dFiltLiquidFactor;
				QUnwashedCake.M[i] = QFeed.M[i] * dCakeLiquidFactor;
			}

		//Now, wash it:
		//First, add the washwater solids, and record what mass of solids we add.
		//The equations associated with this are exceedingly ugly.
		if (FlwIOs.getCount(idWash) > 0) {
			//Short variables to create fewer errors translating the equations
			double WS = QWashWater.Mass(MP_Sol);
			double WL = QWashWater.Mass(MP_Liq);
			double CS = QUnwashedCake.Mass(MP_Sol);
			double CL = QUnwashedCake.Mass(MP_Liq);
			double a, rWS, rWL, rCS, rCL;
			double m = dReqCakeMoisture;
			double w;
			switch (eWashMethod)
			{
			case WM_ConstantEfficiency:
				w = dReqWashEfficiency;
			case WM_WashRatio:
				dWashRatio = WL / CL;
				w = 1 - pow(1-dReqWashEfficiency, dWashRatio);
			}
			switch (eFiltrateMethod) {
			case FM_SolidsToFiltrateFrac:
				a = dReqSolidsToFiltrate;
				rWS = rWL = rCS = rCL = 1;
				break;
			case FM_FiltrateConc:
				a = dReqFiltSolConc;
				rWS = QWashWater.Density(MP_Sol, C_2_K(25));
				rWL = QWashWater.Density(MP_Liq, C_2_K(25));
				rCS = QUnwashedCake.Density(MP_Sol, C_2_K(25));
				rCL = QUnwashedCake.Density(MP_Liq, C_2_K(25));
				break;
			}
			/*Possible situations where variables are out of range:
			 - A: Too much wash liquid - all solids are sent out in washings ['Fix' by sending everyhing to washings. Assume we will not end up with only washing solids]
			 - B: Not enough wash liquid - required wash efficiency impossible ['Fix' by putting all wash water in cake]
			 - C: Cake comes in dry [And very dry] - wash efficiency is higher than requested even if no cake moisture is displaced. 
				[Fix by sending everything].
			 - D: Wash water has FAAR too many solids - cake moisture is lower than required. [Fix by putting everything into cake]
			 - Cake should never come in too wet.
			 */
			//CO - Cake Out. WO - Washings out.
			double dCLtoCO, dCStoCO, dWStoCO;
			
			double dTemp = (WS*rCL*rCS*rWL*(a-rWS)+(a*WL*rCL*rCS+(CS*rCL*(a-rCS)+a*CL*rCS)*rWL)*rWS)
				/ NZ(CS*(m*a*(1-w)*rCS*rWL+rCL*((1-m)*a*rWL+rCS*(m*a*w-(1-m)*rWL)))*rWS + WS*rCL*rCS*((1-m)*rWL*(a-rWS)+m*a*rWS));

			double dWLtoCO = m * (w*CS + WS) * dTemp;
			if (dWLtoCO > WL)
			{ //Case B - not enough wash liquid
				SetNote(idWashNote, "Not enough wash water: All wash water sent to cake");
				//double dTemp1 = (WS*rCL*rCS*(a-rWS)+(CS*rCL*(a-rCS)+a*CL*rCS)*rWS)
				//			/ NZ(CS*((1-m)*rCL*(a-rCS)+m*a*rCS)*rWS + WS*rCS*((1-m)*rCL*(a-rWS) + m*a*rWS));
				double dTemp1 = 1 / (CS * ((1-m) * rCL * (a-rCS) + m*a*rCS) * rWS + WS * rCS * ((1-m) * rCL * (a-rWS) + m*a*rWS));
				double dTemp2 = WS * rCL * rCS * (a-rWS) + (CS * rCL * (a-rCS) + a*(CL+WL)*rCS)*rWS;
				
				dWLtoCO = WL;
				dCLtoCO = (m*CS*CS*rCL*(a-rCS)*rWS + CS*((-(1-m)*WL*rCL*(a-rCS) + m*a*CL*rCS)*rWS+m*WS*rCL*(rCS*(a-2*rWS)+a*rWS))
							+ WS*rCS*(-(1-m)*WL*rCL*(a-rWS) + m*(WS*rCL*(a-rWS) + a*CL*rWS))) * dTemp1;
				dCStoCO = (1-m) * CS * dTemp1 * dTemp2;
				dWStoCO = (1-m) * WS * dTemp1 * dTemp2;
			}
			else
			{
				//dWLtoCO already set.
				dCLtoCO = m * (1-w) * CS * dTemp;
				dCStoCO = (1-m) * CS * dTemp;
				dWStoCO = (1-m) * WS * dTemp;
			}

			if (dCLtoCO > CL) { //Case C - not enough cake moisture [Can only happen if cake comes in with moisture content v. low.]
				dCLtoCO = CL;	//Although this results in a lower than required moisture content, well, the cake is comming in drier than required.
				SetNote(idWashNote, "Cake too dry"); }
			
			//Simple Cons of Mass:
			double dWLtoWO = WL - dWLtoCO;
			double dCLtoWO = CL - dCLtoCO;
			double dWStoWO = WS - dWStoCO;
			double dCStoWO = CS - dCStoCO;

			double dWLtoCOFrac,dCLtoCOFrac,dWStoCOFrac,dCStoCOFrac,dWLtoWOFrac,dCLtoWOFrac,dWStoWOFrac,dCStoWOFrac;
			if (dCStoCO < 0) //Case A, Too wet: Everything sent with washings
			{
				SetNote(idWashNote, "Too much liquid in WashWater. Everything sent to washings");
				dWLtoCOFrac=dCLtoCOFrac=dWStoCOFrac=dCStoCOFrac=0;
				dWLtoWOFrac=dCLtoWOFrac=dWStoWOFrac=dCStoWOFrac=1;
			}
			else if ((CL+WL)/NZ(CS+WS+CL+WL) < m)	//Case D, Too Dry: Send everything to cake.
			{
				SetNote(idWashNote, "Not enough liquid in washing stage. Everything sent to cake");
				dWLtoCOFrac=dCLtoCOFrac=dWStoCOFrac=dCStoCOFrac=1;
				dWLtoWOFrac=dCLtoWOFrac=dWStoWOFrac=dCStoWOFrac=0;
			}
			else
			{
				//Here's where we handle if we have any other zeros.
				//Also handle if a variable manages to slip by the other checks [It is possible with outlandish input parameters.]
				dWLtoCOFrac = WL > 0 ? dWLtoCO / WL : 0;
				dCLtoCOFrac = CL > 0 ? dCLtoCO / CL : 0;
				dWStoCOFrac = WS > 0 ? dWStoCO / WS : 0;
				dCStoCOFrac = CS > 0 ? dCStoCO / CS : 0;
				dWLtoWOFrac = WL > 0 ? dWLtoWO / WL : 0;
				dCLtoWOFrac = CL > 0 ? dCLtoWO / CL : 0;
				dWStoWOFrac = WS > 0 ? dWStoWO / WS : 0;
				dCStoWOFrac = CS > 0 ? dCStoWO / CS : 0;
			}

			dWashEfficiency = CL > 0 ? 1 - dCLtoCOFrac : dNAN;

			MStream QWashingOutput = QCake;
			QWashingOutput.ZeroMass();

			for (int i = 0; i < gs_MVDefn.Count(); i++)
				if (gs_MVDefn[i].IsSolid())
				{
					QWashingOutput.M[i] = QUnwashedCake.M[i] * dCStoWOFrac + QWashWater.M[i] * dWStoWOFrac;
					QCake.M[i] = QUnwashedCake.M[i] * dCStoCOFrac + QWashWater.M[i] * dWStoCOFrac;
				}
				else if (gs_MVDefn[i].IsLiquid())
				{
					QWashingOutput.M[i] = QUnwashedCake.M[i] * dCLtoWOFrac + QWashWater.M[i] * dWLtoWOFrac;
					QCake.M[i] = QUnwashedCake.M[i] * dCLtoCOFrac + QWashWater.M[i] * dWLtoCOFrac;
				}

			double dInputHf;
			
			MStream* pQWashingOutput;
			if (bWashingsConnected)
			{
				dInputHf = QUnwashedCake.totHf() + QWashWater.totHf();
				pQWashingOutput = &QWashingOutput;
			}
			else
			{
				dInputHf = QUnwashedCake.totHf() + QWashWater.totHf() + QFiltrate.totHf();
				for (int i = 0; i < gs_MVDefn.Count(); i++)
					QFiltrate.M[i] += QWashingOutput.M[i];
				pQWashingOutput = &QFiltrate;	//For thermal property managing
			}

			bool converged = false;
			for (int i = 0; i < 10 && !converged; i++)
			{
				double dOutputHf = QCake.totHf() + pQWashingOutput->totHf();

				double deltaT = -(dOutputHf - dInputHf) / NZ(pQWashingOutput->totCp() + QCake.totCp());

				pQWashingOutput->T += deltaT;
				QCake.T = pQWashingOutput->T;

				converged = abs(dInputHf - dOutputHf) < 1; //TODO: Check what sort of convergence we require.
			}
			if (bWashingsConnected)
			{
				MStream& QWashings = FlwIOs[FlwIOs.First[idWashings]].Stream;
				QWashings = QWashingOutput;
			}
		}
		else
			QCake = QUnwashedCake;

		//Vent:
		if (FlwIOs.Count[idVent] > 0)
		{
			MStream& QVent = FlwIOs[FlwIOs.First[idVent]].Stream;
			QVent = QCake;
			QVent.ZeroMass();
			MStream QWashWater;
			if (FlwIOs.Count[idWash] > 0)
				QWashWater = FlwIOs[FlwIOs.First[idWash]].Stream;
			else
				QWashWater.ZeroMass();

			for (int i = 0; i < gs_MVDefn.Count(); i++)
				if (gs_MVDefn[i].IsGas())
					QVent.M[i] = QCake.M[i] + QWashWater.M[i];
		}
		
		//Update "Actual" values.
		dCakeSolids = QCake.MassFrac(MP_Sol);
		dFiltSolids = QFiltrate.MassFrac(MP_Sol);
		dCakeSolConc = QCake.Mass(MP_Sol) / NZ(QCake.Volume(MP_All, C_2_K(25.0)));
		dFiltSolConc = QFiltrate.Mass(MP_Sol) / NZ(QFiltrate.Volume(MP_All, C_2_K(25.0)));
		if (bWashWaterConnected)
		{
			double CFeed = QFeed.SpecieConc(MP_Liq, nWashCompSpecie, C_2_K(25.0));
			double CCake = QCake.SpecieConc(MP_Liq, nWashCompSpecie, C_2_K(25.0));
			double CWash = QWashWater.SpecieConc(MP_Liq, nWashCompSpecie, C_2_K(25.0));
			dWashCompEff = (CFeed - CCake) / NZ(CFeed - CWash);
		}
		else
		{
			dWashCompEff = 0;
			dWashEfficiency = 0;
		}
	}
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
}

bool FilterPress::EvalJoinPressures()
{
	return MBaseMethod::EvalJoinPressures();
	//TODO: Stuff
}


void FilterPress::SetNote(int id, LPCSTR str)
{
	if (bTrackStatus)
		Log.SetCondition(true, id, MMsg_Warning, "%s", str);
}