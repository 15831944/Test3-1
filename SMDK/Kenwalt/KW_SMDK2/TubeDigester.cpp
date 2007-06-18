
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

double Drw_CCARTubeDigester[] = 
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
DEFINE_TRANSFER_UNIT(CCARTubeDigester, "TubeDigester", DLL_GroupName)

void CCARTubeDigester_UnitDef::GetOptions()
{
	SetDefaultTag("TR");
	SetDrawing("HeatExchange", Drw_CCARTubeDigester);
	SetTreeDescription("Heat Transfer:Shell & Tube(2)");
	SetDescription("TODO: Flash Train Digester");
	SetModelSolveMode(MSolveMode_Probal);
	SetModelGroup(MGroup_Energy);
	//SetModelLicense(MLicense_HeatExchange);
};

//---------------------------------------------------------------------------

CCARTubeDigester::CCARTubeDigester(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
	MBaseMethod(pUnitDef, pNd),
	m_RB(this, false, "RB"),
	m_VLE(this, VLEF_QPFlash, "VLE"),
	m_FTC(this),
	m_EHX(this, 0, "EHX")
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
}


  //---------------------------------------------------------------------------

  CCARTubeDigester::~CCARTubeDigester()
    {
    }

  //---------------------------------------------------------------------------

  void CCARTubeDigester::Init()
    {
    SetIODefinition(s_IODefs);
    RegisterFlashTrainNode(idHotI, true, m_FTC.TypeInfo());
    }

  //---------------------------------------------------------------------------


void CCARTubeDigester::BuildDataFields()
{
	static MDDValueLst MDD0[] = {
		{ OM_Simple, "Simple" },
		{ OM_Condensing, "Condensing" },
		{ OM_Evaporating, "Evaporating" },
		{ 0 } };

	DD.Page("HX");
	DD.CheckBox("On", "", &m_bOn, MF_PARAMETER);
	DD.Long("Operation_Mode", "OpMode", (long*)&m_eOperationMode, MF_PARAMETER, MDD0);
	DD.Double("HTC", "", &m_dHTC, MF_PARAMETER, MC_HTC);
	DD.Double("Area", "", &m_dActualArea, MF_PARAMETER, MC_Area);
	DD.Double("U*A", "", &m_dUA, MF_RESULT, MC_UA);
	DD.Double("UALMTD", "", &m_dUALMTD, MF_RESULT, MC_Pwr);
	DD.Double("Duty", "", &m_dDuty, MF_RESULT, MC_Pwr);
	DD.Double("TheorArea", "", &m_dTheorArea, MF_RESULT, MC_Area);
	DD.Double("LMTDFact", "", &m_dLMTDFact, MF_RESULT, MC_Frac);
	DD.Double("QmVentRqd", "", &m_dReqQmVent, MF_PARAMETER, MC_Qm);
	
	m_EHX.BuildDataFields();
	m_RB.BuildDataFields();
	m_VLE.BuildDataFields();
	m_FTC.BuildDataFields();
}

//---------------------------------------------------------------------------

bool CCARTubeDigester::ValidateDataFields()
{
	//Eliminate zeros in case of database corruption or some such.
	m_dHTC = GTZ(m_dHTC);
	m_dActualArea = GTZ(m_dActualArea);
	m_dLMTDFact = Range(ZeroLimit, m_dLMTDFact, 1.0);

	return m_dHTC > ZeroLimit
		&& m_dActualArea > ZeroLimit
		&& m_dLMTDFact > ZeroLimit;
}

bool CCARTubeDigester::PreStartCheck()
{
	if (FlwIOs.Count[FlwIOs.First[idVent]] == 0 && m_dReqQmVent > 0)
		return false;

	return true;
}

//---------------------------------------------------------------------------

bool CCARTubeDigester::ExchangeDataFields()
{
	return false;
}

//---------------------------------------------------------------------------

// NOTE: if the tears in a flash train are moved to the slurry lines connecting the heaters the resulting convergence
// is much improved. This code snippet will set this up automatically
bool CCARTubeDigester::ConfigureJoins()
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

bool CCARTubeDigester::EvalJoinPressures()
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

void CCARTubeDigester::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
      CI.AddPowerIn(0, -m_dDuty);
    }
}

void CCARTubeDigester::EvalProducts()
{
	m_dUA = m_dHTC * m_dActualArea;

	MStream HotI, CoolI;
	FlwIOs.AddMixtureIn_Id(HotI, idHotI);
	FlwIOs.AddMixtureIn_Id(CoolI, idCoolI);

	m_EHX.EvalProducts(HotI);
	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;

	switch (m_eOperationMode)
	{
	case OM_Simple:
		DoSimpleHeater(HotI);
		break;
	case OM_Condensing:
		DoCondensingHeater(HotI);
		break;
	case OM_Evaporating:
		DoEvaporatingHeater(HotI);
		break;
	}

	m_dDuty = HotI.totHz() - HotO.totHz();
	m_dLMTD = LMTD(HotI.T, HotO.T, CoolI.T, CoolO.T);
	m_dUALMTD = m_dLMTD * m_dUA;

	m_RB.EvalProducts(HotO);
}

//After creating an instance of class, no changes may be made to the streams before calling Solve;
class HXSolver : public MRootFinder
{
protected:
	MStream m_HotI, m_CoolI;
	MStream& m_HotO, m_CoolO;

	CCARTubeDigester& m_TD;
	MVLEBlk& m_VLE;

	static MToleranceBlock s_Tol;

public:
	HXSolver (CCARTubeDigester* TD, MStream& HotO, MStream& CoolO, MStream HotI):
	MRootFinder("A Desc", s_Tol),
	m_HotO(HotO),
	m_CoolO(CoolO),
	m_HotI(HotI),
	m_TD(*TD),
	m_VLE(TD->m_VLE)
	{
		m_TD.FlwIOs.AddMixtureIn_Id(m_CoolI, idCoolI);

		m_HotO = m_HotI;
		m_CoolO = m_CoolI;
		double dbg = m_CoolO.Mass();
	};
};

MToleranceBlock HXSolver::s_Tol(TBF_Both, "CCARTubeDigester:HXSolver", 0.0, 1.0e-12);

//Iteration parameter is cool side output temperature.
class SimpleHXSolver : public HXSolver
{
public:
	SimpleHXSolver(CCARTubeDigester *TD, MStream& HotO, MStream& CoolO, MStream HotI) : HXSolver(TD, HotO, CoolO, HotI) {}

	double Function(double TCO)
	{
		//m_CoolO.T = TCO;
		m_CoolO.SetTP(TCO, m_CoolO.P);
		m_TD.m_dDuty = m_CoolO.totHz() - m_CoolI.totHz();
		m_HotO.Set_totHz(m_HotI.totHz() - m_TD.m_dDuty);

		m_TD.m_dLMTD = LMTD(m_HotI.T, m_HotO.T, m_CoolI.T, m_CoolO.T);
		m_TD.m_dUALMTD = m_TD.m_dLMTD * m_TD.m_dUA;
		
		return m_TD.m_dUALMTD - m_TD.m_dDuty;
	}
};

void CCARTubeDigester::DoSimpleHeater(MStream HotI)
{
	MStream CoolI;
	FlwIOs.AddMixtureIn_Id(CoolI, idCoolI);
	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;
	SimpleHXSolver solver(this, HotO, CoolO, HotI);
	solver.FindRoot(0, CoolI.T, HotI.T);
	m_dTheorArea = m_dActualArea;
}

//Iteration parameter is condensation fraction.
class CondensingHXSolver : public HXSolver
{
protected:
	double m_dTSat, m_dPSat;
	double m_dCoolingQ;

public:
	CondensingHXSolver(CCARTubeDigester* TD, MStream& HotO, MStream& ColdO, MStream HotI) : HXSolver(TD, HotO, ColdO, HotI)
	{
		m_dTSat = m_HotI.SaturationT();
		m_dPSat = m_HotI.SaturationP();

		m_HotO.SetTP(m_dTSat, m_dPSat);
		m_dCoolingQ = m_HotI.totHz() - m_HotO.totHz();
	}
	
	double Function(double CondensingFrac)
	{
		//TODO: Prevent overshooting during condensation phase.
		//	- This would cause a log of a negative number, bad news.

		//First: Do the heating caused by condensing the input steam
		m_HotO = m_HotI;
		m_HotO.SetTP(m_dTSat, m_dPSat);

		double HotHzPreCond = m_HotO.totHz();
		double HotIGasCont = m_HotO.Mass(MP_Gas);
		m_VLE.SetFlashVapFrac(m_HotO, m_dTSat, m_dPSat, 1 - CondensingFrac, VLEF_Null);
		m_HotO.SetTP(m_dTSat, m_dPSat);
		double HotOGasCont = m_HotO.Mass(MP_Gas);
		double CondensingQ = HotHzPreCond - m_HotO.totHz();
		m_CoolO.Set_totHz(m_CoolI.totHz() + CondensingQ);

		//If the cond frac is too high, the output will be heated above TSat.
		//In this case, we should return a large number to force the solver to search for another solution.
		double Tn = m_CoolO.T;
		if (Tn > m_dTSat)
			return 1E6;

		//Calculate the LMTD associated with this part of the process...
		double CondLMTD = LMTD(m_dTSat, m_dTSat, m_CoolI.T, m_CoolO.T);
		double CondTheorUA = CondensingQ / NZ(CondLMTD);

		//Now: We do the heating caused by cooling the input steam.

		m_CoolO.Set_totHz(m_CoolO.totHz() + m_dCoolingQ);
		
		double CoolLMTD = LMTD(m_HotI.T, m_dTSat, Tn, m_CoolO.T);
		double CoolTheorUA = m_dCoolingQ / NZ(CoolLMTD);

		m_TD.m_dTheorArea = (CondTheorUA + CoolTheorUA) / m_TD.m_dUA;
		
		return m_TD.m_dTheorArea - m_TD.m_dActualArea;
	}
};

void CCARTubeDigester::DoCondensingHeater(MStream HotI)
{
	//First check if the steam is so superheated that this acts as only a simple heater:
	MStream CoolI;

	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;

	FlwIOs.AddMixtureIn_Id(CoolI, idCoolI);

	double HotQm = HotI.Mass(MP_All); double HotGasQm = HotI.Mass(MP_Gas);
	double HotT = HotI.T; double HotP = HotI.P;
	double ColdQm = CoolI.Mass(MP_All);

	double TSat = HotI.SaturationT();
	double PSat = HotI.SaturationP();
	double initialHz = HotI.totHz();
	HotI.SetTP(TSat, PSat);
	double deltaHz = initialHz - HotI.totHz();
	CoolI.Set_totHz(CoolI.totHz() + deltaHz);
	double TCI = CoolI.T;
	if (TCI > TSat || m_VLE.FlashVapFrac(HotI) < ZeroLimit)
	{
		DoSimpleHeater(HotI);
		//What to do with FTC here? Leave it for now.
		return;
	}
	
	CondensingHXSolver solver(this, HotO, CoolO, HotI);
	solver.FindRoot(0, 0, 1);
}

class EvaporatingHXSolver : public HXSolver
{
protected:
	double m_dTSat, m_dPSat;
	double m_dWarmQ;

public:
	EvaporatingHXSolver(CCARTubeDigester* TD, MStream& HotO, MStream& CoolO, MStream HotI) : HXSolver(TD, HotO, CoolO, HotI)
	{
		m_dTSat = m_CoolI.SaturationT();
		m_dPSat = m_CoolI.SaturationP();

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
		double EvaporationQ = m_CoolO.totHz() - CoolHzPreEvap;
		m_HotO.Set_totHz(m_HotO.totHz() - EvaporationQ);

		double EvapLMTD = LMTD(m_HotI.T, m_HotO.T, m_dTSat, m_dTSat);
		double EvapTheorUA = EvaporationQ / NZ(EvapLMTD);

		//Temp Change-based Q:
		double Tn = m_HotO.T;
		m_HotO.Set_totHz(m_HotO.totHz() - m_dWarmQ);

		double WarmLMTD = LMTD(Tn, m_HotO.T, m_CoolI.T, m_dTSat);
		double WarmTheorUA = m_dWarmQ / NZ(WarmLMTD);

		m_TD.m_dTheorArea = (EvapTheorUA + WarmTheorUA) / m_TD.m_dUA;

		return m_TD.m_dTheorArea - m_TD.m_dActualArea;
	}
};

void CCARTubeDigester::DoEvaporatingHeater(MStream HotI)
{
	//First check if the input is so cold that this acts as only a simple heater:
	MStream CoolI;
	
	MStream& HotO = FlwIOs[FlwIOs.First[idHotO]].Stream;
	MStream& CoolO = FlwIOs[FlwIOs.First[idCoolO]].Stream;

	FlwIOs.AddMixtureIn_Id(CoolI, idCoolI);
	double TSat = HotI.SaturationT();
	double PSat = HotI.SaturationP();
	double initialHz = CoolI.totHz();
	CoolI.SetTP(TSat, PSat);
	double deltaHz = CoolI.totHz() - initialHz;
	HotI.Set_totHz(HotI.totHz() - deltaHz);
	if (HotI.T < TSat)
	{
		DoSimpleHeater(HotI);
		return;
	}
	
	EvaporatingHXSolver solver(this, HotO, CoolO, HotI);
	solver.FindRoot(0, 0, 1);
}

