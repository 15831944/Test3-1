//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-13 11:09:09 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
// $Nokeywords: $
//===========================================================================

#define __TUBE_DIGESTER_CPP
#include "stdafx.h"
#include "TubeDigester.h"
#include <math.h>
//#pragma optimize("", off)

enum {	idCoolI = 0,
		idCoolO,
		idHotI,
		idHotO,
		idVent };

enum {	LC_SolverIndex = 0, 
		LC_FlashVapIndex,
		LC_BadInputs };

double LMTD(double THI, double THO, double TCI, double TCO)
{
	double r = (THI - TCO) / NZ(THO - TCI);
	double n = (THI - TCO) - (THO - TCI);
	if (r < ZeroLimit)
		return 0;	//We should be returning a NAN here. We are asking for the LMTD when the two temperatures cross.

	if (abs(n) < ZeroLimit)
		return THO - TCI;

	return (n) / log(r);
}

static MInOutDefStruct s_IODefs[]=
{
	//  Desc;		Name;		PortId; Rqd; Max; CnId, FracHgt;  Options;
	{ "Cool In",	"CoolI",	idCoolI,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
	{ "Cool Out",	"CoolO",	idCoolO,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Hot In",		"HotI",		idHotI,  1,  10,    1,    1.0f,  MIO_In |MIO_Material },
	{ "Hot Out",	"HotO",		idHotO,  1,   1,    1,    1.0f,  MIO_Out|MIO_Material },
	{ "Vent",		"Vent",		idVent,   0,   1,    1,    1.0f,  MIO_Out|MIO_Material },
	{ NULL },
};

double Drw_CKWATubeDigester[] = 
  { 
  MDrw_Poly, -10,1, 10,1,
  MDrw_Poly, -10,-1, 10,-1,

  MDrw_Poly, -9,3, -9,1,
  MDrw_Poly, -9,-1, -9,-2, 8,-2, 8,-3,

  MDrw_Poly, 9,-3, 9,-1,
  MDrw_Poly, 9,1, 9,2, -8,2, -8,3,

  MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CKWATubeDigester, "TubeDigester", DLL_GroupName)

void CKWATubeDigester_UnitDef::GetOptions()
{
	SetDefaultTag("TR");
	SetDrawing("HeatExchange", Drw_CKWATubeDigester);
	SetTreeDescription("Heat Transfer:Shell & Tube(2)");
	SetDescription("TODO: Flash Train Digester");
	SetModelSolveMode(MSolveMode_Probal);
	SetModelGroup(MGroup_Energy);
	//SetModelLicense(MLicense_HeatExchange);
};

//---------------------------------------------------------------------------

CKWATubeDigester::CKWATubeDigester(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
	MBaseMethod(pUnitDef, pNd),
	m_HotRB(this, false, "RB_Hot"), m_ColdRB(this, false, "RB_Cold"),
	m_HotVLE(this, VLEF_QPFlash, "VLE_Hot"), m_ColdVLE(this, VLEF_QPFlash, "VLE_Cold"),
	m_FTC(this),
	m_HotEHX(this, 0, "EHX_Hot"), m_ColdEHX(this, 0, "EHX_Cold")
{
	m_bOn = true;
	m_eOperationMode = OM_Simple;
	m_dHTC = 1.00;
	m_dActualArea = 10.0;
	m_dLMTDFact = 1.0;
	m_dReqQmVent = 0.0;

	m_dUA = m_dHTC * m_dActualArea;
	m_dLMTD = dNAN;
	m_dUALMTD = dNAN;
	m_dDuty = dNAN;
	m_dTheorArea = dNAN;

	m_dP = 0.0;
	m_bIgnoreAreaLimit = false;
}


  //---------------------------------------------------------------------------

  CKWATubeDigester::~CKWATubeDigester()
    {
    }

  //---------------------------------------------------------------------------

  void CKWATubeDigester::Init()
    {
    SetIODefinition(s_IODefs);
    RegisterFlashTrainNode(idHotI, true, m_FTC.TypeInfo());
    }

  //---------------------------------------------------------------------------

static MDDValueLst gs_OpModes[] = {
		{ OM_Simple, "Simple" },
		{ OM_Condensing, "Condensing" },
		{ OM_Evaporating, "Evaporating" },
		{ OM_CondEvap, "Condensing / Evaporating" },
		{ 0 } };

void CKWATubeDigester::BuildDataFields()
{

	DD.Page("HX");
	DD.CheckBox("On", "", &m_bOn, MF_PARAMETER);
	DD.Long("Operation_Mode", "OpMode", (long*)&m_eOperationMode, MF_PARAMETER, gs_OpModes);
	DD.Double("HTC", "", &m_dHTC, MF_PARAMETER, MC_HTC);
	DD.Double("Area", "", &m_dActualArea, MF_PARAMETER, MC_Area);
	DD.Show(m_eOperationMode != OM_Simple && !m_FTC.Active);
	DD.CheckBox("Ignore_Area_Limit", "", &m_bIgnoreAreaLimit, MF_PARAMETER);
	DD.Show();
	DD.Double("U*A", "", &m_dUA, MF_RESULT, MC_UA);
	DD.Double("UALMTD", "", &m_dUALMTD, MF_RESULT, MC_Pwr);
	DD.Double("Duty", "", &m_dDuty, MF_RESULT, MC_Pwr);
	DD.Double("TheorArea", "", &m_dTheorArea, MF_RESULT, MC_Area);
	DD.Double("LMTDFact", "", &m_dLMTDFact, MF_PARAMETER, MC_Frac);
	DD.Double("QmVentRqd", "", &m_dReqQmVent, MF_PARAMETER, MC_Qm);

	DD.Page("..");
	DD.StructBegin("Hot_Side");
	m_HotSide.BuildDataFields(DD);
	DD.StructEnd();
	DD.StructBegin("Cold_Side");
	m_ColdSide.BuildDataFields(DD);
	DD.StructEnd();
	
	m_HotEHX.BuildDataFields();	m_ColdEHX.BuildDataFields();
	m_HotRB.BuildDataFields();	m_ColdRB.BuildDataFields();
	m_HotVLE.BuildDataFields();	m_ColdVLE.BuildDataFields();
	
	DD.Show(m_FTC.Active);
	m_FTC.BuildDataFields();
}

//---------------------------------------------------------------------------

bool CKWATubeDigester::ValidateDataFields()
{
	//Eliminate zeros in case of database corruption or some such.
	m_dHTC = GTZ(m_dHTC);
	m_dActualArea = GTZ(m_dActualArea);
	m_dLMTDFact = Range(ZeroLimit, m_dLMTDFact, 1.0);

	return m_dHTC > ZeroLimit
		&& m_dActualArea > ZeroLimit
		&& m_dLMTDFact > ZeroLimit;
}

bool CKWATubeDigester::PreStartCheck()
{
	if (FlwIOs.Count[FlwIOs.First[idVent]] == 0 && m_dReqQmVent > 0)
		return false;

	return true;
}

//---------------------------------------------------------------------------

bool CKWATubeDigester::ExchangeDataFields()
{
	return false;
}

//---------------------------------------------------------------------------

// NOTE: if the tears in a flash train are moved to the slurry lines connecting the heaters the resulting convergence
// is much improved. This code snippet will set this up automatically
bool CKWATubeDigester::ConfigureJoins()
{ 
	if (MBaseMethod::ConfigureJoins())
	{
		// If a tear is needed then tear the slurry lines first
		for (int i=0; i<FlwIOs.Count; i++)
		{
			MFlow F=FlwIOs[i];
			if (F.Id>=idCoolI && F.Id<=idCoolO)
				F.TearPriority=MTP_First;
		}

		return true;
	}
	return false;
}

  //---------------------------------------------------------------------------

bool CKWATubeDigester::EvalJoinPressures()
{
	if (1)
	{//set pressures at each join (pipes connected to unit)
	for (int j=0; j<Joins.Count; j++)
	{
	double Pj=Joins[j].GetProbalPIn();

	if (j==1 && m_FTC.Active)
	{
		m_FTC.SuctionP=Pj;

		Joins[j].SetProbalP(Pj, true, true);
	} else 
	{
		Joins[j].SetProbalP(Pj, false, true);
	}


	if (j==0)
	{
	double pOut;
	if (Pj < m_dP)    // Pressure drop greater than inlet pressure
	pOut = 101.325;   // Just set outlet to atmospheric for now...
	else
	pOut = Pj-m_dP;
	Joins[j].SetProbalP(pOut, false, true);
	}
	}
	return true;
	}
	else
	{
	//INCOMPLETECODE(__FILE__, __LINE__) 
	}
	return false;
}

void CKWATubeDigester::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
      CI.AddPowerIn(0, -m_dDuty);
    }
}

void CKWATubeDigester::EvalProducts()
{
	m_dUA = m_dHTC * m_dActualArea;

	MStreamI HotI, CoolI;
	FlwIOs.AddMixtureIn_Id(HotI, idHotI);
	FlwIOs.AddMixtureIn_Id(CoolI, idCoolI);
	m_HotSide.ReadInStream(HotI);
	m_ColdSide.ReadInStream(CoolI);

	m_HotEHX.EvalProducts(HotI);	m_ColdEHX.EvalProducts(CoolI);
	//Vent as required:
	DoVent(HotI);

	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;

	if (HotI.T > CoolI.T)
	{
		switch (m_eOperationMode)
		{
		case OM_Simple:
			DoSimpleHeater(HotI, CoolI);
			break;
		case OM_Condensing:
			DoCondensingHeater(HotI, CoolI);
			break;
		case OM_Evaporating:
			DoEvaporatingHeater(HotI, CoolI);
			break;
		case OM_CondEvap:
			DoCondEvapHeater(HotI, CoolI);
			break;
		}

		m_dDuty = HotI.totHz() - HotO.totHz();
		m_dLMTD = LMTD(HotI.T, HotO.T, CoolI.T, CoolO.T);
		m_dUALMTD = m_dLMTD * m_dUA;
	}
	else
	{
		m_dDuty = 0;
		m_dLMTD = dNAN;
		m_dUALMTD = dNAN;
		m_dTheorArea = dNAN;
		Log.SetCondition(true, LC_BadInputs, MMsg_Warning, "Hot Stream is not Hotter than Cold Stream. Heat Exchanger disabled.");
	}

	m_HotRB.EvalProducts(HotO);		m_ColdRB.EvalProducts(CoolO);
	m_HotSide.ReadOutStream(HotO);	m_ColdSide.ReadOutStream(CoolO);
}

  //---------------------------------------------------------------------------
//After creating an instance of class, no changes may be made to the streams before calling Solve;
class HXSolver : public MRootFinder
{
protected:
	MStream m_HotI, m_CoolI;
	MStream& m_HotO, &m_CoolO;

	CKWATubeDigester& m_TD;

	static MToleranceBlock s_Tol;

public:
	HXSolver (CKWATubeDigester* TD, MStream& HotO, MStream& CoolO, MStream HotI, MStream CoolI):
	MRootFinder("A Desc", s_Tol),
	m_HotO(HotO),
	m_CoolO(CoolO),
	m_HotI(HotI),
	m_CoolI(CoolI),
	m_TD(*TD)
	{
		m_HotO = m_HotI;
		m_CoolO = m_CoolI;
		double dbg = m_CoolO.Mass();
	};
};

MToleranceBlock HXSolver::s_Tol(TBF_Both, "CKWATubeDigester:HXSolver", 0.0, 1.0e-12);
  //---------------------------------------------------------------------------
//Iteration parameter is cool side output temperature.
class SimpleHXSolver : public HXSolver
{
public:
	SimpleHXSolver(CKWATubeDigester *TD, MStream& HotO, MStream& CoolO, MStream HotI, MStream CoolI) : HXSolver(TD, HotO, CoolO, HotI, CoolI) {}

	double Function(double TCO)
	{
		m_CoolO.T = TCO;
		//m_CoolO.SetTP(TCO, m_CoolO.P);
		m_TD.m_dDuty = m_CoolO.totHz() - m_CoolI.totHz();
		m_HotO.Set_totHz(m_HotI.totHz() - m_TD.m_dDuty);

		m_TD.m_dLMTD = LMTD(m_HotI.T, m_HotO.T, m_CoolI.T, m_CoolO.T);
		m_TD.m_dUALMTD = m_TD.m_dLMTD * m_TD.m_dUA;
		
		return m_TD.m_dLMTDFact * m_TD.m_dUALMTD - m_TD.m_dDuty;
	}
};

void CKWATubeDigester::DoSimpleHeater(MStream HotI, MStream CoolI)
{
	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;
	SimpleHXSolver solver(this, HotO, CoolO, HotI, CoolI);
	solver.FindRoot(0, CoolI.T, HotI.T);
	m_dTheorArea = m_dActualArea;
	m_HotSide.m_eOpMode = m_ColdSide.m_eOpMode = OM_Simple;
}

  //---------------------------------------------------------------------------
//Iteration parameter is condensation fraction.
class CondensingHXSolver : public HXSolver
{
protected:
	double m_dTSat, m_dPSat;
	double m_dCoolingQ;

	MVLEBlk& m_VLE;

public:
	const static double s_dTnTooHigh;

	CondensingHXSolver(CKWATubeDigester* TD, MStream& HotO, MStream& ColdO, MStream HotI, MStream ColdI) : HXSolver(TD, HotO, ColdO, HotI, ColdI), m_VLE(TD->m_HotVLE)
	{
		m_dTSat = m_HotI.SaturationT();
		m_dPSat = m_HotI.SaturationTotalP();

		m_HotO.SetTP(m_dTSat, m_dPSat);
		m_dCoolingQ = m_HotI.totHz() - m_HotO.totHz();
	}
	
	double Function(double CondensingFrac)
	{
		//First: Do the heating caused by condensing the input steam
		m_HotO = m_HotI;
		m_HotO.SetTP(m_dTSat, m_dPSat);

		double HotHzPreCond = m_HotO.totHz();
		m_VLE.SetFlashVapFrac(m_HotO, m_dTSat, m_dPSat, 1 - CondensingFrac, VLEF_Null);
		m_HotO.SetTP(m_dTSat, m_dPSat);
		double CondensingQ = HotHzPreCond - m_HotO.totHz();
		m_CoolO.Set_totHz(m_CoolI.totHz() + CondensingQ);

		//If the cond frac is too high, the output will be heated above TSat.
		//In this case, we should return a large number to force the solver to search for another solution.
		double Tn = m_CoolO.T;
		if (Tn > m_dTSat)
			return s_dTnTooHigh;

		//Calculate the LMTD associated with this part of the process...
		double CondLMTD = LMTD(m_dTSat, m_dTSat, m_CoolI.T, m_CoolO.T);
		double CondTheorUA = CondensingQ / NZ(CondLMTD);

		//Now: We do the heating caused by cooling the input steam.

		m_CoolO.Set_totHz(m_CoolO.totHz() + m_dCoolingQ);
		
		double CoolLMTD = LMTD(m_HotI.T, m_dTSat, Tn, m_CoolO.T);
		double CoolTheorUA = m_dCoolingQ / NZ(CoolLMTD);

		m_TD.m_dTheorArea = (CondTheorUA + CoolTheorUA) / (m_TD.m_dHTC * m_TD.m_dLMTDFact);
		
		return m_TD.m_dTheorArea - m_TD.m_dActualArea;
	}

	void SetHotIMass(double HotIMass)
	{
		m_HotI.SetM(m_HotI, MP_All, HotIMass);
	}
};

const double CondensingHXSolver::s_dTnTooHigh = 1E6;
class VapReqSolver : public MRootFinder
{
protected:
	CKWATubeDigester& m_TD;
	CondensingHXSolver& m_CDHXS;
	static MToleranceBlock s_Tol;
public:
	VapReqSolver(CKWATubeDigester* TD, CondensingHXSolver & CDHXS) :
		MRootFinder("Vapour Req Solver", s_Tol),
		m_TD(*TD),
		m_CDHXS(CDHXS)
	{}

	double Function(double HotMass)
	{
		m_CDHXS.SetHotIMass(HotMass);
		return m_CDHXS.Function(1);
	}
};

MToleranceBlock VapReqSolver::s_Tol(TBF_Both, "CKWATubeDigester:VapReq", 0.0, 1.0e-12);
void CKWATubeDigester::DoCondensingHeater(MStream HotI, MStream CoolI)
{
	//First check if the steam is so superheated that this acts as only a simple heater:

	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;

	HotO = HotI;
	CoolO = CoolI;

	//double HotQm = HotI.Mass(MP_All); double HotGasQm = HotI.Mass(MP_Gas);
	//double HotT = HotI.T; double HotP = HotI.P;
	//double ColdQm = CoolI.Mass(MP_All);

	double TSat = HotI.SaturationT();
	double PSat = HotI.SaturationTotalP();
	HotO.SetTP(TSat, PSat);
	double deltaHz = HotI.totHz() - HotO.totHz();
	CoolO.Set_totHz(CoolI.totHz() + deltaHz);
	double TCI = CoolO.T;
	if (TCI > TSat || m_HotVLE.FlashVapFrac(HotI) < ZeroLimit)
	{
		DoSimpleHeater(HotI, CoolI);
		//What to do with FTC here? Leave it for now.
		Log.SetCondition(true, LC_FlashVapIndex, MMsg_Warning, "HX Operating in simple heat exchanger mode");
		return;
	}
	
	CondensingHXSolver solver(this, HotO, CoolO, HotI, CoolI);
	if (m_bIgnoreAreaLimit && !m_FTC.Active)
	{
		double ret = solver.Function(1);
		//If we condense all fluid, we may increase TCO above TSat.
		if (ret >= CondensingHXSolver::s_dTnTooHigh)
		{
			Log.SetCondition(true, LC_FlashVapIndex, MMsg_Warning, "Vapour present in hot output");
			//If in this block, heat is only from condensing - it is a fair assumption that QCond is directly proportional to condensing frac.
			double condensingQ = CoolO.totHz() - CoolI.totHz();
			CoolO.SetTP(TSat, PSat);
			double properCondensingQ = CoolO.totHz() - CoolI.totHz();
			solver.Function(properCondensingQ / NZ(condensingQ) * .998); //Factor of .998 for certainty...
		}
	}
	else
	{
		double HotICondFrac = 1 - m_HotVLE.FlashVapFrac(HotI);
		int solveResult = solver.FindRoot(0, HotICondFrac, 1);
		if (m_FTC.Active)
		{
			double res = solver.Result();
			VapReqSolver VRSolver(this, solver);
			int VR_Result = VRSolver.FindRoot(0, CoolI.Mass(MP_All) * 1E-4, CoolI.Mass(MP_All) * 100);
			if (VR_Result != RF_OK)
				Log.Message(MMsg_Error, "Vapour Req Solver: %s", VRSolver.ResultString(VR_Result));
			m_FTC.VapourFlowReqd = VRSolver.Result();// * m_VLE.FlashVapFrac(HotI); //TODO: Find out more about this.
			HotO = HotI;
			solver.Function(res);	//Restore the streams to their proper values.
		}
		switch (solveResult)
		{
		case RF_OK:
			break;
		case RF_LoLimit:
			Log.SetCondition(true, LC_SolverIndex, MMsg_Warning, "No vapour condensed.");
			break;
		case RF_HiLimit:
			break;
		default:
			Log.SetCondition(true, LC_SolverIndex, MMsg_Error, "%s%s", "Solve Result: ", solver.ResultString(solveResult));
		}
		if (m_HotVLE.FlashVapFrac(HotO) > .0005)
			Log.SetCondition(true, LC_FlashVapIndex, MMsg_Warning, "Vapour present in hot output");
	}

	m_HotSide.m_eOpMode = OM_Condensing;
	m_ColdSide.m_eOpMode = OM_Simple;
}


  //---------------------------------------------------------------------------
class EvaporatingHXSolver : public HXSolver
{
protected:
	double m_dTSat, m_dPSat;
	double m_dWarmQ;
	MVLEBlk &m_VLE;

public:
	EvaporatingHXSolver(CKWATubeDigester* TD, MStream& HotO, MStream& CoolO, MStream HotI, MStream CoolI) : HXSolver(TD, HotO, CoolO, HotI, CoolI), m_VLE(TD->m_ColdVLE)
	{
		m_dTSat = m_CoolI.SaturationT();
		m_dPSat = m_CoolI.SaturationTotalP();

		m_CoolO.SetTP(m_dTSat, m_dPSat);
		m_dWarmQ = m_CoolO.totHz() - m_CoolI.totHz();
	}

	double Function(double EvaporatingFrac)
	{
		//Basically the same as a Condensing Heater, in reverse.
		//Evaporation:
		m_CoolO = m_CoolI;
		m_CoolO.SetTP(m_dTSat, m_dPSat);

		double CoolHzPreEvap = m_CoolO.totHz();
		m_VLE.SetFlashVapFrac(m_CoolO, m_dTSat, m_dPSat, EvaporatingFrac, VLEF_Null);
		m_CoolO.SetTP(m_dTSat, m_dPSat);
		double EvaporationQ = m_CoolO.totHz() - CoolHzPreEvap;
		m_HotO.Set_totHz(m_HotO.totHz() - EvaporationQ);

		double Tn = m_HotO.T;
		if (Tn > m_dTSat)
			return CondensingHXSolver::s_dTnTooHigh;

		double EvapLMTD = LMTD(m_HotI.T, m_HotO.T, m_dTSat, m_dTSat);
		double EvapTheorUA = EvaporationQ / NZ(EvapLMTD);

		//Temp Change-based Q:
		m_HotO.Set_totHz(m_HotO.totHz() - m_dWarmQ);

		double WarmLMTD = LMTD(Tn, m_HotO.T, m_CoolI.T, m_dTSat);
		double WarmTheorUA = m_dWarmQ / NZ(WarmLMTD);

		m_TD.m_dTheorArea = (EvapTheorUA + WarmTheorUA) / (m_TD.m_dHTC * m_TD.m_dLMTDFact);

		return m_TD.m_dTheorArea - m_TD.m_dActualArea;
	}
};

void CKWATubeDigester::DoEvaporatingHeater(MStream HotI, MStream CoolI)
{
	//First check if the input is so cold that this acts as only a simple heater:
	
	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;

	double TSat = HotI.SaturationT();
	double PSat = HotI.SaturationTotalP();
	CoolO.SetTP(TSat, PSat);
	double deltaHz = CoolO.totHz() - CoolI.totHz();
	HotO.Set_totHz(HotI.totHz() - deltaHz);
	if (HotO.T < TSat)
	{
		DoSimpleHeater(HotI, CoolI);
		Log.SetCondition(true, LC_FlashVapIndex, MMsg_Warning, "HX Operating in simple heat exchanger mode");
		return;
	}
	
	EvaporatingHXSolver solver(this, HotO, CoolO, HotI, CoolI);
	if (m_bIgnoreAreaLimit)
	{
		double ret = solver.Function(1);
		//If we condense all fluid, we may increase TCO above TSat.
		if (ret >= CondensingHXSolver::s_dTnTooHigh)
		{
			//If in this block, heat is only from evaporating - it is a fair assumption that QCond is directly proportional to condensing frac.
			double evaporatingQ = HotI.totHz() - HotO.totHz();
			HotO.SetTP(TSat, PSat);
			double properEvaporatingQ = HotI.totHz() - HotO.totHz();
			solver.Function(properEvaporatingQ / NZ(evaporatingQ) * .995); //Factor of .995 for certainty...
		}
	}
	else
	{
		double ColdIVapFrac = m_ColdVLE.FlashVapFrac(CoolI);
		int solveResult = solver.FindRoot(0, ColdIVapFrac, 1);
		switch (solveResult)
		{
		case RF_OK:
			break;
		case RF_LoLimit:
			Log.SetCondition(true, LC_SolverIndex, MMsg_Warning, "No liquid evaporated.");
			break;
		case RF_HiLimit:
			break;
		default:
			Log.SetCondition(true, LC_SolverIndex, MMsg_Error, "%s%s", "Solve Result: ", solver.ResultString(solveResult));
		}
		if (m_HotVLE.FlashVapFrac(CoolO) < .9995)
			Log.SetCondition(true, LC_FlashVapIndex, MMsg_Warning, "Liquid present in cold output");
	}
	m_HotSide.m_eOpMode = OM_Simple;
	m_ColdSide.m_eOpMode = OM_Evaporating;
}

class CondEvapHXSolver : public HXSolver
{
protected:
	MVLEBlk &m_HotVLE, &m_ColdVLE;
	double m_dQCooling, m_dQWarmingI;
	double m_dHTSat, m_dCTSat;
	double m_dHPSat, m_dCPSat;
	double m_dUA_Cond_SensibleI;
	double m_dLMTD_Cond_Evap;
public:
	CondEvapHXSolver(CKWATubeDigester* TD, MVLEBlk& HotVLE, MVLEBlk& ColdVLE,
		MStream& HotO, MStream& ColdO, MStream HotI, MStream ColdI) : 
		HXSolver(TD, HotO, ColdO, HotI, ColdI),
		m_HotVLE(HotVLE), m_ColdVLE(ColdVLE)
	{
		m_dHTSat = m_HotI.SaturationT();
		m_dCTSat = m_CoolI.SaturationT();
		m_dHPSat = m_HotI.SaturationTotalP();
		m_dCPSat = m_CoolI.SaturationTotalP();

		m_HotO.SetTP(m_dHTSat, m_dHPSat);
		m_CoolO.SetTP(m_dCTSat, m_dCPSat);

		m_dQCooling = m_HotI.totHz() - m_HotO.totHz();
		m_dQWarmingI = m_CoolO.totHz() - m_CoolI.totHz();

		double LMTDWarmingI = LMTD(m_dHTSat, m_dHTSat, m_CoolI.T, m_dCTSat);
		m_dUA_Cond_SensibleI = m_dQWarmingI / NZ(LMTDWarmingI);
		m_dLMTD_Cond_Evap = NZ(LMTD(m_dHTSat, m_dHTSat, m_dCTSat, m_dCTSat));
	}

	double Function(double CondFrac)
	{
		//TODO: Case where no simultaneous evap / condensation can occur. Handle boundary cases:
		// * THSat < TCSat
		MStream TestHotO = m_HotI;
		m_HotO = m_HotI; m_CoolO = m_CoolI; //Restore streams...
		double UA_Cond_Evap = 0, UA_Cond_Sense = 0, UA_Sense_Evap = 0, UA_Sense_Sense = 0;
		double UA_Cond_SenseI = m_dUA_Cond_SensibleI;
		//Condense the incomming cold fluid, and calculate QCond:
		m_HotO.SetTP(m_dHTSat, m_dHPSat);
		double initialHz = m_HotO.totHz();
		m_HotVLE.SetFlashVapFrac(m_HotO, m_dHTSat, m_dHPSat, 1 - CondFrac, VLEF_Null);
		m_HotO.SetTP(m_dHTSat, m_dHPSat);
		double QCond = initialHz - m_HotO.totHz();

		//Determine if the Q from condensing is enough to fully vaporize the incomming cold stream:
		m_ColdVLE.PFlash(m_CoolO, m_dCPSat, QCond, VLEF_Null);

		if (m_ColdVLE.FlashVapFrac(m_CoolO) < 1) //Simple case where condensation is not enough to fully evaporate.
		{
			UA_Cond_Evap = (QCond - m_dQWarmingI) / m_dLMTD_Cond_Evap; //We have NZ'd this elsewhere.

			//Now determine what happens as we cool the incomming stream to TSat, PSat.
			m_ColdVLE.PFlash(m_CoolO, m_dCPSat, m_dQCooling);
			if (m_ColdVLE.FlashVapFrac(m_CoolO) < 1) //Simple case where there is condensate in the cold output
			{
				double LMTD_Sense_Evap = LMTD(m_HotI.T, m_dHTSat, m_dCTSat, m_dCTSat);
				UA_Sense_Evap = m_dQCooling / NZ(LMTD_Sense_Evap);
			}
			else									//Cold stream is completely evaporated, and slightly superheated by the sensible cooling of the input stream.
			{
				//First do the rest of the evaporation...
				m_CoolO = m_CoolI;
				m_ColdVLE.SetFlashVapFrac(m_CoolO, m_dCTSat, m_dCPSat, 1, VLEF_Null);
				m_CoolO.SetTP(m_dCTSat, m_dCPSat);
				double QEvap = m_CoolO.totHz() - m_CoolI.totHz() - m_dQWarmingI;
				double QSense_Evap = QEvap - (QCond - m_dQWarmingI);
				TestHotO.Set_totHz(m_HotI.totHz() - m_dQCooling + QSense_Evap);
				double Tn = TestHotO.T;
				double LMTD_Sense_Evap = LMTD(Tn, m_dHTSat, m_dCTSat, m_dCTSat);
				UA_Sense_Evap = QSense_Evap / NZ(LMTD_Sense_Evap);
				//Then superheat the steam:
				m_CoolO.Set_totHz(m_CoolI.totHz() + QCond + m_dQCooling);
				double LMTD_Sense_Sense = LMTD(m_HotI.T, Tn, m_dCTSat, m_CoolO.T);
				double QSense_Sense = m_dQCooling - QSense_Evap;
				UA_Sense_Sense = QSense_Sense / NZ(LMTD_Sense_Sense);
			}
		}
		else	//The cold side is fully evaporated before we even consider the cooling of the hotside:
		{
			double Tn = m_CoolO.T;
			m_CoolO = m_CoolI;
			m_ColdVLE.SetFlashVapFrac(m_CoolO, m_dCTSat, m_dCPSat, 1, VLEF_Null);
			m_CoolO.SetTP(m_dCTSat, m_dCPSat);
			double QEvap = m_CoolO.totHz() - m_CoolI.totHz() - m_dQWarmingI;
			UA_Cond_Evap = QEvap / m_dLMTD_Cond_Evap;
			//Well, fortunately sysCAD has done much of the work for us. Currently output stream is at Tn.
			m_CoolO.T = Tn;
			double QCond_Sense = QCond - m_dQWarmingI - QEvap;
			double LMTD_Cond_Sense = LMTD(m_dHTSat, m_dHTSat, m_dCTSat, Tn);
			UA_Cond_Sense = QCond_Sense / NZ(LMTD_Cond_Sense);
			m_CoolO.Set_totHz(m_CoolO.totHz() + m_dQCooling);
			double LMTD_Sense_Sense = LMTD(m_HotI.T, m_dHTSat, Tn, m_CoolO.T);
			UA_Sense_Sense = m_dQCooling / NZ(LMTD_Sense_Sense);
		}
		m_TD.m_dTheorArea = (UA_Cond_SenseI + UA_Cond_Evap + UA_Cond_Sense + UA_Sense_Evap + UA_Sense_Sense) / (m_TD.m_dHTC * m_TD.m_dLMTDFact);
		return m_TD.m_dTheorArea - m_TD.m_dActualArea;
	}
};

void CKWATubeDigester::DoCondEvapHeater(MStream HotI, MStream CoolI)
{
	if (HotI.SaturationT() < CoolI.SaturationT())
	{
		Log.SetCondition(true, LC_FlashVapIndex, MMsg_Warning, "HX Operating in simple heat exchanger mode");
		DoSimpleHeater(HotI, CoolI);
		return;
	}
	//TODO: Make this as functional as straight Condensing or straight evaporating heater. [What warnings do we want? Do we want a steam demand?]
	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;
	CondEvapHXSolver solver(this, m_HotVLE, m_ColdVLE, HotO, CoolO, HotI, CoolI);
	double HotICondFrac = 1 - m_HotVLE.FlashVapFrac(HotI);
	solver.FindRoot(0, HotICondFrac, 1);
	m_HotSide.m_eOpMode = OM_Condensing;
	m_ColdSide.m_eOpMode = OM_Evaporating;
}

void CKWATubeDigester::DoVent(MStream& HotI)
{
	if (FlwIOs.Count[idVent] < 1) return;	//If no vent is connected, don't vent.
	MStream& Vent = FlwIOs[FlwIOs.First[idVent]].Stream;
	Vent = HotI;
	Vent.ZeroMass();
	double VentMass = m_dReqQmVent;
	Vent.SetM(HotI, MP_Gas, VentMass);
	HotI.SetM(HotI, MP_Gas, HotI.Mass(MP_Gas) - VentMass);
}

HXSideVars::HXSideVars()
{
	m_eOpMode = OM_Simple;
	m_dQm = m_dCp = m_dTi = m_dTo = m_dPi = m_dPo = m_ddT = m_dSatT = m_dSatP = m_dSatPP = dNAN;
	m_ddHz = dNAN;
}

void HXSideVars::ReadInStream(const MStream &InStream)
{
	m_dQm = InStream.Mass();
	m_dCp = InStream.totCp();
	m_dTi = InStream.T;
	m_dPi = InStream.P;
	m_dSatT = InStream.SaturationT();
	m_dSatP = InStream.SaturationTotalP();
	m_dSatPP = InStream.SaturationP();
	m_dInHz = InStream.totHz();
}

void HXSideVars::ReadOutStream(const MStream &OutStream)
{
	m_dTo = OutStream.T;
	m_dPo = OutStream.P;
	m_ddT = m_dTo - m_dTi;
	m_ddHz = OutStream.totHz() - m_dInHz;
}

void HXSideVars::BuildDataFields(MDataDefn DD)
{
	DD.Long("Operation_Mode", "", (long*)&m_eOpMode, MF_RESULT, gs_OpModes);
	DD.Double("Qm", "", &m_dQm, MF_RESULT, MC_Qm);
	DD.Double("Cp", "", &m_dCp, MF_RESULT, MC_CpMs);
	DD.Double("Ti", "", &m_dTi, MF_RESULT, MC_T);
	DD.Double("To", "", &m_dTo, MF_RESULT, MC_T);
	DD.Double("dT", "", &m_ddT, MF_RESULT, MC_dT);
	DD.Double("Pi", "", &m_dPi, MF_RESULT, MC_P);
	DD.Double("Po", "", &m_dPo, MF_RESULT, MC_P);
	DD.Double("SatT", "", &m_dSatT, MF_RESULT, MC_T);
	DD.Double("SatP", "", &m_dSatP, MF_RESULT, MC_P);
	DD.Double("SatPP", "", &m_dSatPP, MF_RESULT, MC_P); //Should this be MC_DP?
	DD.Double("dHz", "", &m_ddHz, MF_RESULT, MC_E);
}