//================== Copyright Hatch Associates =======================================================================
// $Nokeywords: $
//=====================================================================================================================

#include "stdafx.h"
#include "hatchheater.h"
#pragma optimize("", off) //include this when stepping through code

//=====================================================================================================================

/*---------------------------------------------------------------------------------------------------------------------
Heat Exchanger Model:

The model has two operating modes:
-	Liquor / Liquor (including Slurries)
-	Fully condensing

The incidents stream identified as "Slurry.." is a slurry stream in both cases. For the Liquor /
Liquor case, the "Steam / Condensate" pair is the second liquor stream.

Note that:

-	Reactions (if present) operate on the "Slurry" stream;
-	Heat losses (if specified) are from the "Condensate" stream.

---------------------------------------------------------------------------------------------------------------------*/
const long idSlurIn = 0;
const long idSlurOur = 1;
const long idSteamIn = 2;
const long idCondOut = 3;

// Note: Set "CnId" to match the inputs and outputs
static MInOutDefStruct s_IODefs[]=
{
  //  Desc;             Name;           PortId;		Rqd;	Max; CnId, FracHgt;  Options;
	{ "Slurry In",      "SlurryIn",		idSlurIn,   1,		10,  0,    1.0f,  MIO_In |MIO_Material },
    { "Slurry Out",     "SlurryOut",	idSlurOur,  1,		1,   0,    1.0f,  MIO_Out|MIO_Material },
    { "Steam",          "Steam",		idSteamIn,  1,		10,  1,    1.0f,  MIO_In |MIO_Material },
    { "Condensate",     "Condensate",   idCondOut,  1,		1,   1,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
};


static double Drw_HatchHeater[] = 
{ 
  MDrw_Arc, 0, 0, 5,
  MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
  MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
  MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
  MDrw_End
};

//---------------------------------------------------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(HatchHeater, "HatchHeater", DLL_GroupName)
void HatchHeater_UnitDef::GetOptions()
  {
  SetDefaultTag("SH");
  SetDrawing("HeatExchange", Drw_HatchHeater);
  SetTreeDescription("Hatch: Heater");
  SetDescription("Shell & Tube Heat Exchager with Internal Reaction(s)");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------------------------------------------------
const int idxCondensing=0;
const int idxLiq_Liq=1;

HatchHeater::HatchHeater(MUnitDefBase * pUnitDef, TaggedObject * pNd) : 
	MBaseMethod(pUnitDef, pNd), 
	m_RB(this, false, "RB"),
	m_VLE(this, VLEF_QPFlash, "VLE"),
	m_FTC(this)
{
	//default values...
	bOnLine = true;
	CalcMode=idxCondensing;
	dActualDuty = 0.0;
	dA=1000;
	dU=1;
	dDamping=0.5;
	SlurInT=0;
	SlurOutT=0;
	SlurHeatGain=0;
	ReactHeat=0;
	EndoHeat=0;

	// Steam / Liquor Side
	SteamInT=0;
	CondOutT=0;
	StmHeatDrop=0;
	HeatLoss=0;

	DoPrint=0;
  DoPrint2=0;
  CpTestTol=1e-11;
  EqualCp=0;
}

//---------------------------------------------------------------------------------------------------------------------
HatchHeater::~HatchHeater()
{
}

//---------------------------------------------------------------------------------------------------------------------
void HatchHeater::Init()
{
	SetIODefinition(s_IODefs);
	// Register as potential part of flashtrain, set connection as the steam input
	RegisterFlashTrainNode(idSteamIn, true, m_FTC.TypeInfo()); 
}

//---------------------------------------------------------------------------------------------------------------------

const int idDX_HeaterSize = 1;

void HatchHeater::BuildDataFields()
{
	static MDDValueLst DDInputMethods[]=
    {
	    {0, "Fully Condensing"},
		{1, "Liquor / Liquor"},
		{0}
	};
	static MDDValueLst DDpr[]=
    {
	    {0, "Off"},
		{1, "On"},
		{0}
	};
	m_RB.OnOffCheckBox();

	DD.Page("HX");
	DD.StructBegin("HX");
	DD.Text("");
	DD.Text("Requirements...");
	DD.Long("Check","",(long*)&DoPrint, MF_PARAMETER|MF_SET_ON_CHANGE, DDpr);
	DD.Long("Check2","",(long*)&DoPrint2, MF_PARAMETER|MF_SET_ON_CHANGE, DDpr);
	DD.Double("Damping", "", &dDamping, MF_PARAMETER, MC_);

	DD.Double("CpTestTol", "", &CpTestTol, MF_PARAMETER, MC_);
	DD.Long("EqualCp", "", &EqualCp, MF_RESULT|MF_NO_FILING);

	DD.CheckBox("On", "",  &bOnLine, MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
	DD.Text("");
	DD.Long("CalcMethod","",(long*)&CalcMode, MF_PARAMETER|MF_SET_ON_CHANGE, DDInputMethods);
	DD.Double("Area", "", &dA, MF_PARAMETER, MC_Area("m^2"));
	DD.Double("HTC", "", &dU, MF_PARAMETER, MC_HTC("kW/m^2.K"));
	DD.Double("HeatLossReqd", "", &HeatLoss, MF_PARAMETER, MC_Pwr("kJ/s"));

	DD.Text("");
	DD.Text("Slurry...");
	DD.Double("FeedT", "", &SlurInT, MF_RESULT|MF_NO_FILING, MC_T("C"));
	DD.Double("ProdT", "", &SlurOutT, MF_RESULT|MF_NO_FILING, MC_T("C"));
	DD.Double("SensibleHeatGain", "", &SlurHeatGain, MF_RESULT|MF_NO_FILING, MC_Pwr("kJ/s"));
	DD.Double("HeatOfReaction", "", &EndoHeat, MF_RESULT|MF_NO_FILING, MC_Pwr("kJ/s"));
	DD.Text(" HOR displayed positive if the reaction absorbs heat");

	DD.Text("");
	DD.Text("Heat Exchanger...");
	DD.Double("LMTD", "", &dLMTD, MF_RESULT, MC_T("C"));
	DD.Text(" Heat Transfer=U*A*LMTD if and ONLY if there are NO Reactions or Heat Losses");
	DD.String("HeaterSize", "", idDX_HeaterSize, MF_RESULT|MF_NO_FILING);
	DD.Double("HeatLossCalc", "", &HeatLoss, MF_RESULT|MF_NO_FILING, MC_Pwr("kJ/s"));

	DD.Text("");
	DD.Text("Steam / Liquor...");
	DD.Double("SteamT", "", &SteamInT, MF_RESULT|MF_NO_FILING, MC_T("C"));
	DD.Double("CondT", "", &CondOutT, MF_RESULT|MF_NO_FILING, MC_T("C"));
	DD.Double("SensibleHeatDrop", "", &StmHeatDrop, MF_RESULT|MF_NO_FILING, MC_Pwr("kJ/s"));
	DD.StructEnd();

	DD.Text("");
	m_FTC.BuildDataFields();

	if (m_RB.Enabled())
    {
		DD.Page("RB", false); //when reaction is on, always force this on to a new "RB" tab page
    }
	m_RB.BuildDataFields();

}

//---------------------------------------------------------------------------------------------------------------------
bool HatchHeater::ExchangeDataFields()
{
  if (m_FTC.ExchangeDataFields())
    return true;
	switch (DX.Handle)
    {
		case idDX_HeaterSize: 
			DX.String=(LPCSTR) HeaterSize;
			return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------------------

// NOTE: if the tears in a flash train are moved to the slurry lines connecting the heaters the resulting convergence
// is much improved. This code snippet will set this up automatically
bool HatchHeater::ConfigureJoins()
  {
  if (MBaseMethod::ConfigureJoins())
    {
    // If a tear is needed then tear the slurry lines first
    for (int i=0; i<FlwIOs.Count; i++)
      {
      MFlow F=FlwIOs[i];
      if (F.Id>=idSlurIn && F.Id<=idSlurOur)
        F.TearPriority=MTP_First;
      }
  
    return true;
    }
  return false;
  }
//CNM //////////////////////
//---------------------------------------------------------------------------------------------------------------------
bool HatchHeater::EvalJoinPressures()
{
	if (1)//IsProbal)
    {//set pressures at each join (pipes connected to unit)
		for (int j=0; j<Joins.Count; j++)
		{
			double Pj=Joins[j].GetProbalPIn();

			//	Shell side and flashtrain is active
			if (j==1 && m_FTC.Active) m_FTC.SuctionP=Pj; 

			Joins[j].SetProbalP(Pj, true, true);
		}
		return true;
	}
	else
    {
    //INCOMPLETECODE(__FILE__, __LINE__) 
    }
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
// Average Cp for liquid / liquid calculations: CpAvg=(CpOut*Tout-CpIn*Tin)/(Tout-Tin)
double	HatchHeater::CalcAvgCp(int opt,MStream& FlowI, MStream& FlowO)
{
	double CpAvg,CpIn,CpOut,Tin,Tout,FlowRate;
	
	try
	{
		// Flow Rate
		FlowRate=FlowI.Mass(MP_All);

		// Get the inlet temperature and handle Low Temperature.
		Tin=K_2_C(FlowI.T);
		if(Tin<=ZeroLimit) return(3.15);

		// Calculate the inlet Cp and handle No Outlet Calculated
		CpIn=FlowI.totHz()/(Tin*GTZ(FlowRate));
		if(opt==0) return(CpIn);

		// Get the outlet temperature and handle Low Temperature.
		Tout=K_2_C(FlowO.T);
		if(Tout<=ZeroLimit) return(3.15);

		// Calculate the outlet Cp
		CpOut=FlowO.totHz()/(Tout*GTZ(FlowRate));

		// If the temperatures are the same, return the Cp calculated from the outlet stream
		if(fabs(Tout-Tin)<ZeroLimit) return(CpOut);

		// Calculate the average Cp
		CpAvg=(CpOut*Tout-CpIn*Tin)/(Tout-Tin);

		// Warn on -ve CpAvg and return default
		if(CpAvg<0) 
		{
			Log.Message(MMsg_Error,"HatchHeater::CalcCp, -ve CpAvg=%f",CpAvg);
			return(CpOut);
		}
		return(CpAvg);
	}
	catch (MMdlException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcAvgCp - MMdlException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcAvgCp - MFPPException -----------");
		e.ClearFPP();
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcAvgCp - MSysException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcAvgCp - ... Exception -----------");
		Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
	return(3.15);
}

//---------------------------------------------------------------------------------------------------------------------
// Exit Cp for liquid / liquid calculations
double	HatchHeater::CalcExitCp(int opt,MStream& FlowI, MStream& FlowO)
{
	double CpIn,CpOut,Tin,Tout,FlowRate;
	
	try
	{
		// Flow Rate
		FlowRate=FlowI.Mass(MP_All);

		if(opt==0)
		{
			// Get the inlet temperature and handle Low Temperature.
			Tin=K_2_C(FlowI.T);
			if(Tin<=ZeroLimit) return(3.15);

			// Calculate the inlet Cp and handle No Outlet Calculated
			CpIn=FlowI.totHz()/(Tin*GTZ(FlowRate));
			return(CpIn);
		}

		// Get the outlet temperature and handle Low Temperature.
		Tout=K_2_C(FlowO.T);
		if(Tout<=ZeroLimit) return(3.15);

		// Calculate the outlet Cp and the average value
		CpOut=FlowO.totHz()/(Tout*GTZ(FlowRate));
		return(CpOut);
	}
	catch (MMdlException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcExitCp - MMdlException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcExitCp - MFPPException -----------");
		e.ClearFPP();
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcExitCp - MSysException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::CalcExitCp - ... Exception -----------");
		Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
	return(3.15);
}

//---------------------------------------------------------------------------------------------------------------------
void HatchHeater::EvalProducts()
{
	try
    {
		MStreamI QI_Slur;								//initialise local empty copy of a stream
		MStreamI QI_Stm;									//initialise local empty copy of a stream
		FlwIOs.AddMixtureIn_Id(QI_Slur, idSlurIn);		//sum of all feed streams
		FlwIOs.AddMixtureIn_Id(QI_Stm, idSteamIn);		//sum of all feed streams
		const long index1 = FlwIOs.First[idSlurOur];
		MStream & QO_Slur = FlwIOs[index1].Stream;		//get reference to the actual output stream
		const long index2 = FlwIOs.First[idCondOut];
		MStream & QO_Cond = FlwIOs[index2].Stream;		//get reference to the actual output stream

		HeatLost=-HeatLoss;
		HeaterSize="OK";

		// Set outputs=inputs
		QO_Slur = QI_Slur;
		QO_Cond = QI_Stm;

		Log.ClearCondition(2);

	    // Get mass flow rates.
		double	Slurry_Mass=QI_Slur.Mass(MP_All);
		double	StmCond_Mass=QI_Stm.Mass(MP_All);
		double	SlurOut_T,Cond_T,PrevSlurOut_T;
		double	Slurry_Cp,StmCond_Cp;
		double	phi,chi;
		int		Conv=0;

		// For anything to happen, the heater must be on line and that both the heat transfer area and the 
		// heat transfer coefficient must have positive values.
		if (bOnLine && (dU>0) && (dA>0))
		{
			// Call the reaction code on the slurry stream. NOTE that the reaction (if present) will be 
			// evaluated even if the other stream is not present
			m_RB.EvalProducts(QO_Slur);

			// Stop if there is zero flow rate in one of the streams
			if((Slurry_Mass<=0) || (StmCond_Mass<=0)) return;

			// Get the energy released by the reaction
			ReactHeat=QO_Slur.totHz()-QI_Slur.totHz();

			// ==== Liquor / Liquor Mode ==========================================================
			if(CalcMode==idxLiq_Liq)
			{
				// This loop is temporary; needs better convergence control
				SlurOut_T=1e4;
				for(int i=0;(i<20)&&(!Conv);i++)
				{
					PrevSlurOut_T=SlurOut_T;

					// Calculate average values of Cp and hence components of the Temperature Equation
					Slurry_Cp=CalcAvgCp(i,QI_Slur,QO_Slur);
					StmCond_Cp=CalcAvgCp(i,QI_Stm,QO_Cond);
					double	gamma=StmCond_Mass*StmCond_Cp;
					double	delta=Slurry_Mass*Slurry_Cp;
					phi=(ReactHeat/delta)+(HeatLost/gamma);
					chi=-((1./gamma)-(1./delta));

					// Calculate exit values of Cp for the overall Energy Balance
					Slurry_Cp=CalcExitCp(i,QI_Slur,QO_Slur);
					StmCond_Cp=CalcExitCp(i,QI_Stm,QO_Cond);
					gamma=StmCond_Mass*StmCond_Cp;
					delta=Slurry_Mass*Slurry_Cp;

					// Solve the Temperature and Energy Equations for the Exit Temperatures
					double	alpha=phi/(chi*dU*dA);
					double	beta=exp(-chi*dU*dA);
					double	X=-K_2_C(QI_Stm.T)-K_2_C(QI_Slur.T)*beta+alpha*beta-alpha;
					double	Hin=QI_Slur.totHz()+QI_Stm.totHz()+ReactHeat+HeatLost;
					Cond_T=(delta*X+Hin)/NZ(gamma-beta*delta);
					SlurOut_T=(Hin-gamma*Cond_T)/delta;

					if(C_2_K(Cond_T)<QI_Slur.T) SlurOut_T-=.5;
					QO_Slur.T=C_2_K(SlurOut_T);
					QO_Cond.T=C_2_K(Cond_T);
					QO_Cond.Set_totHz(QI_Slur.totHz()+QI_Stm.totHz()+ReactHeat+HeatLost-QO_Slur.totHz());

					if(fabs(PrevSlurOut_T-K_2_C(QO_Slur.T))<.001) Conv=1;
				}

				if(QO_Cond.T<QI_Slur.T)
				{
					Log.Message(MMsg_Error,"HatchHeater::EvalProducts: Mismatch Calculating L/L:");
					Log.Message(MMsg_Error,"H Slurry In=%.3f (C), Cond Out=%.3f (C)",K_2_C(QI_Slur.T),K_2_C(QO_Cond.T));
				}

			}

			// ==== Fully Condensing Mode ==============================================================================
			else if(CalcMode==idxCondensing)
			{
				// If part of Flash Train, compute steam requirement.
				if(m_FTC.Active) 
			    {
			         DoCondensingHeater(QI_Stm,QI_Slur,QO_Cond,QO_Slur);
		        }

				// When operating under live steam conditions calculate the maximum temperature which the slurry can
				// attain: this is the smaller of the saturation temperature of the steam and the temperature calculated
				// using the heater UA and the steam and slurry properties. Note that when the heater operates as part
				// of a flash train, the maximum slurry temperature associated with the current steam conditions.
				if(!m_FTC.Active)
				{
					double	Tmax1=QI_Stm.SaturationT(QI_Stm.P);
					double	Tmax2=CalcTco(QI_Stm,QI_Slur,QO_Cond,QO_Slur);
					QO_Slur.T=(Tmax1>Tmax2) ? Tmax2 : Tmax1;
					QO_Slur.T=CalcTco(QI_Stm,QI_Slur,QO_Cond,QO_Slur);

					// Calculate the MAXIMUM energy which can be absorbed by the slurry stream.
					double	HeatMax=QO_Slur.totHz()-QI_Slur.totHz();

					// Heat available by washing off superheat
					QO_Cond=QI_Stm;
					double	SteamHeat=QO_Cond.totHz();				// Heat in Feed Steam
					QO_Cond.T=QI_Stm.SaturationT(QI_Stm.P);			// Set saturated
					double	SatHeat=QO_Cond.totHz();				// Heat in Saturated Steam
					double	SuperHeat=SteamHeat-SatHeat;			// Superheat "washed off"

					long	H2OVap=gs_MVDefn.LookupNX("H2O(g)");
					long	H2OLiq=gs_MVDefn.LookupNX("H2O(l)");

					// First, assume that ALL of the steam supplied is condensed
					QO_Cond.M[H2OVap]=0;
					QO_Cond.M[H2OLiq]=StmCond_Mass;
					QO_Cond.T=QI_Stm.SaturationT(QI_Stm.P);

					// Heat released by condensing all steam from the SATURATED condition
					double	HeatAllCond=SatHeat-QO_Cond.totHz();
					// Heat available to increase slurry temperature: latent heat + superheat + reaction heat (if any)
					double	HeatAvbl=HeatAllCond+SuperHeat+ReactHeat+HeatLost;

					// If more is available than can be absorbed, only part of the (now) SATURATED steam is condensed
					//if((!m_FTC.Active) && (.999888*HeatAvbl)>HeatMax)
					if((.999888*HeatAvbl)>HeatMax)
					{
						// Heat to be released by condensing steam; fraction steam to be condensed
						double	HeatCond=HeatMax-(SuperHeat+ReactHeat);
						double	Frac=HeatCond/HeatAllCond;
						QO_Cond.M[H2OLiq]=StmCond_Mass*Frac;
						QO_Cond.M[H2OVap]=StmCond_Mass-QO_Cond.M[H2OLiq];
						HeatAvbl=Frac*HeatAllCond+SuperHeat+ReactHeat+HeatLost;
						HeaterSize="Undersized for steam load";
						Log.SetCondition(1,MMsg_Error,"Heater Area undersized for steam load");
					}
					QO_Slur.Set_totHz(QI_Slur.totHz()+HeatAvbl);
if (DoPrint2) 
Log.Message(MMsg_Error,"HeatAvbl:%.3f   Qm:%.3f  T:%.3f  P:%.3f", HeatAvbl, StmCond_Mass, K_2_C(QO_Cond.T), QI_Stm.P);
				}
				else Log.ClearCondition(1);

			}
		}

		//get display values...
		SlurInT=QI_Slur.T;
		SlurOutT=QO_Slur.T;
		SteamInT=QI_Stm.T;
		CondOutT=QO_Cond.T;
		SlurHeatGain=QO_Slur.totHz()-QI_Slur.totHz();
		StmHeatDrop=QI_Stm.totHz()-QO_Cond.totHz();
		EndoHeat=-ReactHeat;

		double	dTi,dTo,HR;

		if(CalcMode==idxLiq_Liq) 
		{
			HR=(phi/chi)/(dU*dA);
			dTi=CondOutT-SlurInT;
			dTo=SteamInT-SlurOutT;
		}
		else
		{
			dTi=CondOutT-SlurInT;
			dTo=CondOutT-SlurOutT;
		}

		if(dTi*dTo<=0)
		{
			Log.Message(MMsg_Error,"HatchHeater::EvalProducts: Error Calculating LMTD:");
			Log.Message(MMsg_Error,"Slurry In=%.3f (C), Slurry Out=%.3f (C)",K_2_C(SlurInT),K_2_C(SlurOutT));
			Log.Message(MMsg_Error,"Steam In=%.3f (C), Cond Out=%.3f (C)",K_2_C(SteamInT),K_2_C(CondOutT));
			dLMTD=C_2_K(0);
		}
		else if(fabs(dTi-dTo)<1e-11)
		{
			dLMTD=C_2_K(1);
		}
		else
		{
			dLMTD=(dTo-dTi)/log(dTo/dTi);
			dLMTD=C_2_K(dLMTD); // Convert to K for display
		}

	}
	catch (MMdlException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::EvalProducts - MMdlException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::EvalProducts - MFPPException -----------");
		e.ClearFPP();
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::EvalProducts - MSysException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::EvalProducts - ... Exception -----------");
		Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
}

//---------------------------------------------------------------------------------------------------------------------
// Utility function to calculate the outlet temperature of the slurry on the basis of its area and heat transfer 
// coefficient and taking account of the effects of reactions, heat losses and changes in the slurry specific heat 
// between inlet and outlet.
double	HatchHeater::CalcTco(MStream& ShellI, MStream& TubeI, MStream& ShellO, MStream& TubeO)
{
	double UA = dU*dA;
	double	Tci=TubeI.T;
	double	Slurry_Mass=TubeI.Mass(MP_All);

	// For anything to happen in the heater, require that there is heater transfer area, a heat transfer
	// coefficient and a slurry flow.
	if(UA<1.0e-6) return(Tci);


	// Slurry Specific Heat, such that H=WCpT, T measured above 0C
	double	Tci_C=K_2_C(TubeI.T);
	double	Tco_C=K_2_C(TubeO.T);
	double	SlurIn_Cp;
	double	SlurOut_Cp;
	if((Tci_C<=ZeroLimit)||(Tco_C<=ZeroLimit)) SlurIn_Cp=SlurOut_Cp=3.15;	// Just in case!!
	else
	{
		SlurIn_Cp=TubeI.totHz()/(Tci_C*Slurry_Mass);
		SlurOut_Cp=TubeO.totHz()/(Tco_C*Slurry_Mass);
	}
	double	f1,f2,mc;
	if(fabs(SlurIn_Cp-SlurOut_Cp)<1e-6)//this can cause a discontinuity!?!?!?
	{
		mc=0;
		f1=1/(SlurIn_Cp*Slurry_Mass);
		f2=1;
	}
	else
	{
		mc=Slurry_Mass*(SlurOut_Cp-SlurIn_Cp);
		f1=log(SlurOut_Cp/SlurIn_Cp)/mc;
		f2=1+mc/UA;
	}
	double	FTemp=ShellI.SaturationT();
	double	HR=ReactHeat/UA;
	// Calculate slurry outlet temperaure. For no Heat of Reaction and no change in Slurry Cp, this 
	// reduces to the standard form: To=Ts-(Tstm-Ti)*exp(-UA/MCp)
	double	Tco=((FTemp+HR)-(FTemp+HR-Tci*f2)*exp(-(UA+mc)*f1))/f2;
	return(Tco);
}

//---------------------------------------------------------------------------------------------------------------------
void HatchHeater::DoCondensingHeater(MStream& ShellI, MStream& TubeI, MStream& ShellO, MStream& TubeO)
{
	double	LastFlow=0;

	try
	{
		if (TubeI.MassFlow()>0 && ShellI.MassFlow()>0)
		{
			m_FTC.VapourFlow=ShellI.MassFlow();
			double MaxVapFlow=150.0;//???
			CST_CondensateFinder Fn(this, TubeI, ShellI, TubeO, ShellO);

			// Now Solve and set the flash train demand
			if(DoPrint) Log.Message(MMsg_Error,"----- First -----");
			int RetCode = Fn.FindRoot(0, 0, MaxVapFlow);
			switch (RetCode)
			{
				case RF_OK: 
					LastFlow=m_FTC.VapourFlowReqd;
					m_FTC.VapourFlowReqd = Fn.Result();
					break;
				case RF_LoLimit: 
					Log.Message(MMsg_Error, "Condensate Flow at Low Limit - RootFinder:%s %f",
						Fn.ResultString(Fn.Error()),Fn.Result()); 
					m_FTC.VapourFlowReqd = MaxVapFlow*1e-6;   
					break;
				case RF_HiLimit: 
					Log.Message(MMsg_Error, "Condensate Flow at High Limit - RootFinder:%s %f",
						Fn.ResultString(Fn.Error()),Fn.Result()); 
					m_FTC.VapourFlowReqd = MaxVapFlow;        
					break;
				default: 
					Log.Message(MMsg_Error, "Condensate Flow not found - RootFinder:%s", Fn.ResultString(Fn.Error())); 
				break;
			}

			// Operating as part of a flash train macro model, must set "makeup" required...
			if (m_FTC.Active)
			{
				const double SatP = TubeI.SaturationP(TubeI.T);
				//m_FTC.MinSaturationP = SatP; //set the minimum allowable pressure for the steam source flash
        m_FTC.putMinSaturationP(SatP);
				if(DoPrint)
          Log.Message(MMsg_Error,"----- Last -----");

				Fn.Function2(m_FTC.VapourFlow);

				if (1)
				{
					// Make sure Damping is OK
					if(dDamping<0) dDamping=0;
					else if(dDamping>1) dDamping=1;

					// Only allow one steam feed connection
					for (int i=0; i<FlwIOs.Count; i++)
					{
						MFlow F=FlwIOs[i];
						if (F.Id==idSteamIn && F.IsFlwIn && F.IsMakeUpAvail)
              {
							F.MakeUpReqd=(1-dDamping)*m_FTC.VapourFlowReqd+dDamping*LastFlow;
              }
					}
				}
				else
				{
					// Loop through all the connections and sum the "available makeup". 
					// There may be more than one steam source directly connected!
					double Reqd=m_FTC.VapourFlowReqd;
					double Avail=0.0;
					for (int i=0; i<FlwIOs.Count; i++)
					{
						MFlow F=FlwIOs[i];
						if (F.Id==idSteamIn && F.IsFlwIn && F.IsMakeUpAvail) Avail+=F.MakeUpAvail;
					}
					Avail=GTZ(Avail);

					// Distribute according to MakeUpAvail
					for (int i=0; i<FlwIOs.Count; i++)
					{
						MFlow F=FlwIOs[i];
						if (F.Id==idSteamIn && F.IsFlwIn && F.IsMakeUpAvail)
              {
							F.MakeUpReqd=Reqd*Max(1.0e-9, F.MakeUpAvail/Avail);
              }
					}
				}
        if (fabs(m_FTC.VapourFlowReqd-m_FTC.VapourFlow)>1.0e-4)
  				Log.SetCondition(2,MMsg_Warning,"Flash train steam demand calculation not met");
			}
		}
	}
	catch (MMdlException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::DoCondensing - MMdlException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::DoCondensing - MFPPException -----------");
		e.ClearFPP();
		Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::DoCondensing - MSysException -----------");
		Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		Log.Message(MMsg_Error,"-------- HatchHeater::DoCondensing - ... Exception -----------");
		Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
}

//---------------------------------------------------------------------------------------------------------------------
void HatchHeater::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, dActualDuty); //ensure heat balance
    }
  }

//=====================================================================================================================

MToleranceBlock CST_CondensateFinder::s_Tol(TBF_Both, "HatchUnits:CondensFinder", 0.0, 1.0e-12);

//---------------------------------------------------------------------------------------------------------------------
CST_CondensateFinder::CST_CondensateFinder(HatchHeater* pUnit, MStream& TubeI, MStream& ShellI, MStream& TubeO, 
										   MStream& ShellO) : 
	MRootFinder("A Desc" ,s_Tol),
	m_Unit(*pUnit), 
	m_TubeI(TubeI),
	m_ShellI(ShellI),
	m_TubeO(TubeO),
	m_ShellO(ShellO),
	m_VLE(pUnit->m_VLE)
{
	m_TubeO.SuppressErrors=true;
	m_ShellO.SuppressErrors=true;
}

//---------------------------------------------------------------------------------------------------------------------
CST_CondensateFinder::~CST_CondensateFinder()
{
	m_TubeO.SuppressErrors=false;
	m_ShellO.SuppressErrors=false;
}

//---------------------------------------------------------------------------------------------------------------------
// This function returns the difference between:
// 1.	The sensible heat given up by the condensing steeam
// 2.	The sensible gained by the slurry, calculated on the basis of its area and heat transfer coefficient and
//		taking account of the effects of reactions, heat losses and changes in the slurry specific heat between
//		inlet and outlet.
double CST_CondensateFinder::Function(double Amount)
{
	try
	{
		double	HR,Tci,Tco,f1,f2,mc,Slurry_Mass,TDuty,UA;
		double	SlurIn_Cp;
		double	SlurOut_Cp;

		// Set the condensate stream equal to the steam feed stream and save the total heat
		m_ShellO.SetM(m_ShellI, MP_All, Amount, m_ShellI.P);
		double ShtotHz=m_ShellO.totHz();

		// Condense the vapour at its saturation pressure and calculate the energy given up by the condensing steam
		double FTemp=m_ShellI.SaturationT();
		double FPress=m_ShellI.SaturationP();
		//double FPress=m_ShellI.SaturationP(FTemp);
		m_VLE.SetFlashVapFrac(m_ShellO, FTemp, FPress, 0.0, VLEF_Null);
		m_ShellO.SetTP(FTemp, FPress);
		// Sensible heat available to heat the slurry
		m_Unit.m_dDuty=-(m_ShellO.totHz()-ShtotHz);

		// Make an estimate of the slurry outlet temperature for the purpose of estimating the Cp at the outlet;
		// the change in composition due to reactions is alreay accounted for.
		m_TubeO.Set_totHf(m_TubeI.totHf()+m_Unit.m_dDuty);
		// Variable for sensible heat absorbed in heating the slurry, in reactions, and in heat losses.
		 TDuty=0;
		// Heat Transfer Coefficient x Heater Area
		UA = m_Unit.dU*m_Unit.dA;
		// Slurry Mass
		Slurry_Mass=m_TubeI.Mass(MP_All);

		// For a Heat Requirement to exist, it must be case that the heater has an Area and a Heat Transfer 
		// Coeffcient; it must also have slurry flowing through it.
		if((UA>1e-6)&&(Slurry_Mass>1e-6))
	    {
			// Slurry Specific Heat, such that H=WCpT, T measured above 0C
			double	Tci_C=K_2_C(m_TubeI.T);
			double	Tco_C=K_2_C(m_TubeO.T);
			if((Tci_C<=ZeroLimit)||(Tco_C<=ZeroLimit)) SlurIn_Cp=SlurOut_Cp=3.15; 	// Just in case!!
			else
			{
				SlurIn_Cp=m_TubeI.totHz()/(Tci_C*Slurry_Mass);
				SlurOut_Cp=m_TubeO.totHz()/(Tco_C*Slurry_Mass);
			}

			// No change in Slurry Cp through the heater
		     const double ToleranceTest = m_Unit.CpTestTol;
			if(fabs(SlurIn_Cp-SlurOut_Cp)<ToleranceTest)//this can cause a discontinuity!?!?!?
			{
if(m_Unit.DoPrint) m_Unit.Log.Message(MMsg_Error,">>>>>>>>>>>>>>>>> Case 1 %f %f %f",SlurIn_Cp,SlurOut_Cp,Slurry_Mass);
				mc=Slurry_Mass*(SlurOut_Cp-SlurIn_Cp);
				f1=1/(SlurIn_Cp*Slurry_Mass);
				f2=1;
if(m_Unit.DoPrint) m_Unit.Log.Message(MMsg_Error,">>>>>>>>>>>>>>>>> Case 1 %f %f %f",mc,f1,f2);
        m_Unit.EqualCp = true;
			}
			// Allow for effects of change in Slurry Cp through the heater
			else
			{
if(m_Unit.DoPrint) m_Unit.Log.Message(MMsg_Error,"<<<<<<<<<<<<<<<<< Case 2 %f %f %f",SlurIn_Cp,SlurOut_Cp,Slurry_Mass);
				mc=Slurry_Mass*(SlurOut_Cp-SlurIn_Cp);
				f1=SlurOut_Cp/GTZ(SlurIn_Cp);
				f1=log(f1)/(Slurry_Mass*NZ(SlurOut_Cp-SlurIn_Cp));
				f2=1+mc/UA;
if(m_Unit.DoPrint) m_Unit.Log.Message(MMsg_Error,">>>>>>>>>>>>>>>>> Case 2 %f %f %f",mc,f1,f2);
        m_Unit.EqualCp = false;
			}
			// Heat of Reaction
			HR=m_Unit.ReactHeat/UA;
			// Get Slurry inlet temperature
			Tci=m_TubeI.T;
			// Calculate slurry outlet temperaure. For no Heat of Reaction and no change in Slurry Cp, this 
			// reduces to the standard form: To=Ts-(Tstm-Ti)*exp(-UA/MCp)
			Tco=((FTemp+HR)-(FTemp+HR-Tci*f2)*exp(-UA*f1*f2))/f2;
			// Set the Slurry outlet temperature
			m_TubeO.T=Tco;

			// --- Calculate the Heater Duty ---
			// This is the difference between the Sensible Heat in the Outlet Slurry and the sum of:
			// - Sensible Heat in the Inlet Slurry
			// - Heat of Reaction (Exothermic Reactions with -ve HOR will increase the duty)
			// - Heat Losse (-ve, so they increase the duty)
			// ??? Is "m_TubeO.totHz()" calculated as part of the calculation? Appears to be
			TDuty=m_TubeO.totHz()-(m_TubeI.totHz()+m_Unit.ReactHeat+m_Unit.HeatLost);
		}
		else
		{
			TDuty = 0.0;
	  }

		// Difference between sensible heat given up by the condensing steam and the net sensible heat absorbed 
		// by the slurry
		double Diff = m_Unit.m_dDuty-TDuty;

if(m_Unit.DoPrint) 
{
	m_Unit.Log.Message(MMsg_Error,"Diff=%.1f  Duty(Slurry)=%.1f   Duty(Steam)=%f",Diff,m_Unit.m_dDuty,TDuty);
	m_Unit.Log.Message(MMsg_Error,"  Steam=%.2f (kg/s)  Tci=%.2f (C)  Tco=%.2f (C)  UA=%.2f (kJ/s.K)  f1=%.4f (kgK/kJ)   f2=%.4f (-)",Amount,Tci,Tco);
	m_Unit.Log.Message(MMsg_Error,"  Tsat(Steam)=%.2f (C)  HR=%.2f (C)  UA=%.2f (kJ/s.K)  f1=%.4f (kgK/kJ)   f2=%.4f (-)",FTemp,HR,UA,f1,f2);
	m_Unit.Log.Message(MMsg_Error,"  Slurry Flow=%.1f (kJ/s)  CpIn=%.4f (kJ/kgK)  CpOut=%.4f (kJ/kgK)  mc=%.4f (-)",Slurry_Mass,SlurIn_Cp,SlurOut_Cp,mc);
}

		return Diff;
	}
	catch (MMdlException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function - MMdlException -----------");
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function - MFPPException -----------");
		e.ClearFPP();
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function - MSysException -----------");
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function - ... Exception -----------");
		m_Unit.Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
	return(0);
}

//---------------------------------------------------------------------------------------------------------------------
// "Last throw of the dice."
double CST_CondensateFinder::Function2(double Amount)
{

if(m_Unit.DoPrint) 
	m_Unit.Log.Message(MMsg_Error,"The amount of steam actually supplied is: %.3f kg/s, at %.3f C",Amount,K_2_C(m_ShellO.T));

	int	opt=0;

	try
	{
		/*
		// Use ALL steam supplied to heat the slurry: this leads to disaster!
		if(opt==1)
		{
			m_ShellO=m_ShellI;
			double	SteamHeat=m_ShellO.totHz();				// Heat in Feed Steam
			m_ShellO.T=m_ShellI.SaturationT(m_ShellI.P);	// Set saturated

			long	H2OVap=gs_MVDefn.LookupNX("H2O(g)");
			long	H2OLiq=gs_MVDefn.LookupNX("H2O(l)");

			m_ShellO.M[H2OVap]=0;
			m_ShellO.M[H2OLiq]=Amount;
			m_ShellO.T=m_ShellI.SaturationT(m_ShellI.P);

			// Heat released by condensing all steam
			double	HeatAllCond=SteamHeat-m_ShellO.totHz();
			// Heat available to increase slurry temperature: latent heat + superheat + reaction heat (if any)
			double	HeatAvbl=HeatAllCond+m_Unit.ReactHeat+m_Unit.HeatLost;
			if(m_Unit.DoPrint) 
			{
				m_Unit.Log.Message(MMsg_Error,"Heat in the superheated steam:               %.3f (kg/s)",SteamHeat);
				m_Unit.Log.Message(MMsg_Error,"Heat released by condensing:                 %.3f (kg/s)",HeatAllCond);
				m_Unit.Log.Message(MMsg_Error,"Heat of Reaction:                            %.3f (kg/s)",m_Unit.ReactHeat);
				m_Unit.Log.Message(MMsg_Error,"Heat Losses:                                 %.3f (kg/s)",m_Unit.ReactHeat);
				m_Unit.Log.Message(MMsg_Error,"Heat AVAILABLE (Complete condensation):      %.3f (kg/s)",HeatAvbl);
			}
			m_TubeO.Set_totHz(m_TubeI.totHz()+HeatAvbl);

			// Calculate the difference between heat given up by COMPLETE condensation of the steam, and the net
			// sensible heat absorbed by the slurry
			m_Unit.m_dDuty=HeatAllCond;
			double	TDuty=m_TubeO.totHz()-(m_TubeI.totHz()+m_Unit.ReactHeat+m_Unit.HeatLost);
			double Diff = m_Unit.m_dDuty-TDuty;

			if(m_Unit.DoPrint) 
			{
				m_Unit.Log.Message(MMsg_Error,"Heat USED:                                   %.3f (kg/s)",HeatAvbl);
				m_Unit.Log.Message(MMsg_Error,"TDuty:                                       %.3f (kg/s)",TDuty);
				m_Unit.Log.Message(MMsg_Error,"m_Unit.m_dDuty:                              %.3f (kg/s)",m_Unit.m_dDuty);
				m_Unit.Log.Message(MMsg_Error,"Slurry exit temperature is:                  %.3f",K_2_C(m_TubeO.T));
				m_Unit.Log.Message(MMsg_Error,"Heat in the feed slurry:                     %.3f (kg/s)",m_TubeI.totHz());
				m_Unit.Log.Message(MMsg_Error,"Heat in the product slurry:                  %.3f (kg/s)",m_TubeO.totHz());
				m_Unit.Log.Message(MMsg_Error,"DIff:                                        %.3f (kg/s)",Diff);
			}
			
		return(Diff);
		}
		*/


		// Slurry outlet temperature cannot exceed:  (1) steam saturation temperature; (2) LTMD value.
		double	Tmax1=m_ShellI.SaturationT(m_ShellI.P);
		double	Tmax2=m_Unit.CalcTco(m_ShellI,m_TubeI,m_ShellO,m_TubeO);
		m_TubeO.T=(Tmax1>Tmax2) ? Tmax2 : Tmax1;
		m_TubeO.T=m_Unit.CalcTco(m_ShellI,m_TubeI,m_ShellO,m_TubeO);

		// Calculate the MAXIMUM energy which can be absorbed by the slurry stream.
		double	HeatMax=m_TubeO.totHz()-m_TubeI.totHz();

if(m_Unit.DoPrint) 
{
	m_Unit.Log.Message(MMsg_Error,"The slurry inlet temperature is:             %.3f",K_2_C(m_TubeI.T));
	m_Unit.Log.Message(MMsg_Error,"The saturation temperature of the steam is:  %.3f",K_2_C(Tmax1));
	m_Unit.Log.Message(MMsg_Error,"Max slurry exit temperature (LMTD) is:       %.3f",K_2_C(Tmax2));
	m_Unit.Log.Message(MMsg_Error,"Max slurry exit temperature is:              %.3f",K_2_C(m_TubeO.T));
	m_Unit.Log.Message(MMsg_Error,"Heat REQUIRED:                               %.3f (kg/s)",HeatMax);
}

		// Heat available by washing off superheat
		m_ShellO=m_ShellI;
		double	SteamHeat=m_ShellO.totHz();				// Heat in Feed Steam
		m_ShellO.T=m_ShellI.SaturationT(m_ShellI.P);	// Set saturated
		double	SatHeat=m_ShellO.totHz();				// Heat in Saturated Steam
		double	SuperHeat=SteamHeat-SatHeat;			// Superheat "washed off"

		long	H2OVap=gs_MVDefn.LookupNX("H2O(g)");
		long	H2OLiq=gs_MVDefn.LookupNX("H2O(l)");

		// First, assume that ALL of the steam supplied is condensed
		m_ShellO.M[H2OVap]=0;
		m_ShellO.M[H2OLiq]=Amount;
		m_ShellO.T=m_ShellI.SaturationT(m_ShellI.P);

		// Heat released by condensing all steam from the SATURATED condition
		double	HeatAllCond=SatHeat-m_ShellO.totHz();
		// Heat available to increase slurry temperature: latent heat + superheat + reaction heat (if any)
		double	HeatAvbl=HeatAllCond+SuperHeat+m_Unit.ReactHeat+m_Unit.HeatLost;

if(m_Unit.DoPrint) 
{
	m_Unit.Log.Message(MMsg_Error,"Heat in the superheated steam:               %.3f (kg/s)",SteamHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat in the saturated steam:                 %.3f (kg/s)",SatHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat released by washing off the superheat:  %.3f (kg/s)",SuperHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat released by condensing sat steam:       %.3f (kg/s)",HeatAllCond);
	m_Unit.Log.Message(MMsg_Error,"Heat of Reaction:                            %.3f (kg/s)",m_Unit.ReactHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat Losses:                                 %.3f (kg/s)",m_Unit.ReactHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat AVAILABLE (Complete condensation):      %.3f (kg/s)",HeatAvbl);
}

		// If more is available than can be absorbed, only part of the (now) SATURATED steam is condensed
		double	Frac=1;
		if((.999888*HeatAvbl)>HeatMax)
		{
			// Heat to be released by condensing steam; fraction steam to be condensed
			//double	HeatCond=HeatMax-(SuperHeat+m_Unit.ReactHeat);
			//Frac=HeatCond/HeatAllCond;
			//m_ShellO.M[H2OLiq]=Amount*Frac;
			//m_ShellO.M[H2OVap]=Amount-m_ShellO.M[H2OLiq];
			//HeatAvbl=Frac*HeatAllCond+SuperHeat+m_Unit.ReactHeat+m_Unit.HeatLost;
			m_Unit.HeaterSize="Undersized for steam load";
			m_Unit.Log.SetCondition(1,MMsg_Error,"Heater Area undersized for steam load");
if(m_Unit.DoPrint2) 
m_Unit.Log.Message(MMsg_Error,"HeatAvbl:%.3f   Qm:%.3f  T:%.3f  P:%.3f", HeatAvbl, Amount, K_2_C(m_ShellO.T), m_ShellI.P);
		}

		m_TubeO.Set_totHz(m_TubeI.totHz()+HeatAvbl);

if(m_Unit.DoPrint2) 
m_Unit.Log.Message(MMsg_Error,"HeatAvbl:%.3f   Qm:%.3f  T:%.3f  P:%.3f", HeatAvbl, Amount, K_2_C(m_ShellO.T), m_ShellI.P);
		// Calculate the difference between heat given up by COMPLETE condensation of the steam, and the net
		// sensible heat absorbed by the slurry
		m_Unit.m_dDuty=HeatAllCond;
		double	TDuty=m_TubeO.totHz()-(m_TubeI.totHz()+m_Unit.ReactHeat+m_Unit.HeatLost);
		double Diff = m_Unit.m_dDuty-TDuty;

if(m_Unit.DoPrint) 
{
	m_Unit.Log.Message(MMsg_Error,"Heat USED:                                   %.3f (kg/s)",HeatAvbl);
	m_Unit.Log.Message(MMsg_Error,"TDuty:                                       %.3f (kg/s)",TDuty);
	m_Unit.Log.Message(MMsg_Error,"m_Unit.m_dDuty:                              %.3f (kg/s)",m_Unit.m_dDuty);
	m_Unit.Log.Message(MMsg_Error,"Slurry exit temperature is:                  %.3f",K_2_C(m_TubeO.T));
	m_Unit.Log.Message(MMsg_Error,"Heat in the feed slurry:                     %.3f (kg/s)",m_TubeI.totHz());
	m_Unit.Log.Message(MMsg_Error,"Heat in the product slurry:                  %.3f (kg/s)",m_TubeO.totHz());
	m_Unit.Log.Message(MMsg_Error,"Fraction of steam condensed:                 %.3f (kg/s)",Frac);
	m_Unit.Log.Message(MMsg_Error,"DIff:                                        %.3f (kg/s)",Diff);
}

		return(Diff);
	}
	catch (MMdlException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - MMdlException -----------");
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - MFPPException -----------");
		e.ClearFPP();
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - MSysException -----------");
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - ... Exception -----------");
		m_Unit.Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
	return(0);
}

double CST_CondensateFinder::Function3(double Amount)
{

if(m_Unit.DoPrint) 
	m_Unit.Log.Message(MMsg_Error,"The amount of steam actually supplied is: %.3f kg/s, at %.3f C",Amount,K_2_C(m_ShellO.T));

	int	opt=0;

	try
	{
  //shell===stm  ; tube===slur
		// Slurry outlet temperature cannot exceed:  (1) steam saturation temperature; (2) LTMD value.
		double	Tmax1=m_ShellI.SaturationT(m_ShellI.P);
		double	Tmax2=m_Unit.CalcTco(m_ShellI,m_TubeI,m_ShellO,m_TubeO);
		m_TubeO.T=(Tmax1>Tmax2) ? Tmax2 : Tmax1;
		m_TubeO.T=m_Unit.CalcTco(m_ShellI,m_TubeI,m_ShellO,m_TubeO);

		// Calculate the MAXIMUM energy which can be absorbed by the slurry stream.
		double	HeatMax=m_TubeO.totHz()-m_TubeI.totHz();

if(m_Unit.DoPrint) 
{
	m_Unit.Log.Message(MMsg_Error,"The slurry inlet temperature is:             %.3f",K_2_C(m_TubeI.T));
	m_Unit.Log.Message(MMsg_Error,"The saturation temperature of the steam is:  %.3f",K_2_C(Tmax1));
	m_Unit.Log.Message(MMsg_Error,"Max slurry exit temperature (LMTD) is:       %.3f",K_2_C(Tmax2));
	m_Unit.Log.Message(MMsg_Error,"Max slurry exit temperature is:              %.3f",K_2_C(m_TubeO.T));
	m_Unit.Log.Message(MMsg_Error,"Heat REQUIRED:                               %.3f (kg/s)",HeatMax);
}

		// Heat available by washing off superheat
		m_ShellO=m_ShellI;
		double	SteamHeat=m_ShellO.totHz();				// Heat in Feed Steam
		m_ShellO.T=m_ShellI.SaturationT(m_ShellI.P);	// Set saturated
		double	SatHeat=m_ShellO.totHz();				// Heat in Saturated Steam
		double	SuperHeat=SteamHeat-SatHeat;			// Superheat "washed off"

		long	H2OVap=gs_MVDefn.LookupNX("H2O(g)");
		long	H2OLiq=gs_MVDefn.LookupNX("H2O(l)");

		// First, assume that ALL of the steam supplied is condensed
		m_ShellO.M[H2OVap]=0;
		m_ShellO.M[H2OLiq]=Amount;
		m_ShellO.T=m_ShellI.SaturationT(m_ShellI.P);

		// Heat released by condensing all steam from the SATURATED condition
		double	HeatAllCond=SatHeat-m_ShellO.totHz();
		// Heat available to increase slurry temperature: latent heat + superheat + reaction heat (if any)
		double	HeatAvbl=HeatAllCond+SuperHeat+m_Unit.ReactHeat+m_Unit.HeatLost;

if(m_Unit.DoPrint) 
{
	m_Unit.Log.Message(MMsg_Error,"Heat in the superheated steam:               %.3f (kg/s)",SteamHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat in the saturated steam:                 %.3f (kg/s)",SatHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat released by washing off the superheat:  %.3f (kg/s)",SuperHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat released by condensing sat steam:       %.3f (kg/s)",HeatAllCond);
	m_Unit.Log.Message(MMsg_Error,"Heat of Reaction:                            %.3f (kg/s)",m_Unit.ReactHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat Losses:                                 %.3f (kg/s)",m_Unit.ReactHeat);
	m_Unit.Log.Message(MMsg_Error,"Heat AVAILABLE (Complete condensation):      %.3f (kg/s)",HeatAvbl);
}

		// If more is available than can be absorbed, only part of the (now) SATURATED steam is condensed
		double	Frac=1;
		if((.999888*HeatAvbl)>HeatMax)
		{
			// Heat to be released by condensing steam; fraction steam to be condensed
			double	HeatCond=HeatMax-(SuperHeat+m_Unit.ReactHeat);
			Frac=HeatCond/HeatAllCond;
			m_ShellO.M[H2OLiq]=Amount*Frac;
			m_ShellO.M[H2OVap]=Amount-m_ShellO.M[H2OLiq];
			HeatAvbl=Frac*HeatAllCond+SuperHeat+m_Unit.ReactHeat+m_Unit.HeatLost;
			m_Unit.HeaterSize="Undersized for steam load";
			m_Unit.Log.SetCondition(1,MMsg_Error,"Heater Area undersized for steam load");
		}

		m_TubeO.Set_totHz(m_TubeI.totHz()+HeatAvbl);

		// Calculate the difference between heat given up by COMPLETE condensation of the steam, and the net
		// sensible heat absorbed by the slurry
		m_Unit.m_dDuty=HeatAllCond;
		double	TDuty=m_TubeO.totHz()-(m_TubeI.totHz()+m_Unit.ReactHeat+m_Unit.HeatLost);
		double Diff = m_Unit.m_dDuty-TDuty;
    
		return(Diff);
	}
	catch (MMdlException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - MMdlException -----------");
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	catch (MFPPException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - MFPPException -----------");
		e.ClearFPP();
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	catch (MSysException &e)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - MSysException -----------");
		m_Unit.Log.Message(MMsg_Error, e.Description);
    }
	  catch (...)
    {
		m_Unit.Log.Message(MMsg_Error,"-------- CST_CondensateFinder::Function2 - ... Exception -----------");
		m_Unit.Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
	return(0);
}
