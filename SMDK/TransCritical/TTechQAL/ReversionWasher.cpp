//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//           Reversion Washer Model 2004 - Transcritical Technologies 
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __REVERSIONWASHER_CPP
#include "reversionwasher.h"


//====================================================================================




enum ThermalHeatLossMethods { THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_HeatFlowFrac };



class SESolverFn : public MRootFinderBase
{
	ReversionWasher & q;
	static MToleranceBlock s_Tol;

public: 
	double Function(double x) {return q.scandrettBalance(x);}
	SESolverFn(ReversionWasher & q_): MRootFinderBase("Main", s_Tol), q(q_) {}
};


MToleranceBlock SESolverFn::s_Tol(TBF_Both, "ReversionWasher:Scandrett", 0.00005, 0, 100);


static MInOutDefStruct s_IODefs[]=
{
	//  Desc;             Name;         Id; Rqd; Max; CnId, FracHgt;  Options;
	{ "WashWater",          "Feed1",  0,   1,  5,    0,    1.0f,  MIO_In |MIO_Material },
	{ "Mud Feed",          "Feed2" ,  1,   1,  5,    0,    1.0f,  MIO_In |MIO_Material }, 
	{ "Overflow",       "Prod1",   2,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Underflow",       "Prod2",  3,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Side Stream",     "Feed3", 4,  0, 5, 0, 1.0f, MIO_In |MIO_Material },
	{ NULL },
};
// NOTE!!!!!!!! Side stream not incorporated yet - contents do not involve in scandrett mixing.



static MDDValueLst  iODERtn[] = {
	{0, "Timestep", 0},
	{1, "RungeKutta", 0},
	{2, "Adaptive RK", 0},
	NULL
};  

static MDDValueLst  iScandSpc[] = {
	{0, "Aluminate", 0},
	{1, "Caustic", 0}, 
	NULL
};

static MDDValueLst DDB3[]={
	{0,          "None" },
	{1,      "TempDrop"},
	{2, "FixedLoss"},
	{3,  "LossCoeff"},
	{0}
};

static char *labels[] = {
	"Intermed 0",
		"Intermed 1", 
		"Intermed 2", 
		"Intermed 3", 
		"Intermed 4", 
		"Intermed 5", 
		"Intermed 6", 
		"Intermed 7",
		"Intermed 8",
		"Intermed 9",
		"Intermed A"


}; 


static double ReversionWasherDraw[] = {
	MDrw_Poly,  -20, 20, 20, 20, 20, -12, 0, -20, -20, -12, -20, 20, 
		MDrw_Poly,  -20, 0, 20, 0,
		MDrw_Poly,  -16, 10, 16, 10, -16, -10, 16, -10, 13, -8, 13, -12, 16, -10,  
		MDrw_End
};

DEFINE_TRANSFER_UNIT(ReversionWasher, "ReversionWasher", DLL_GroupName)

void ReversionWasher_UnitDef::GetOptions()
  {
  SetDefaultTag("RW");
  SetDrawing("WasherDecanter", ReversionWasherDraw);
  SetTreeDescription("QAL:Reversion Washer");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
  };

//---------------------------------------------------------------------------


// IMPLEMENT_TRANSFERMETHOD(ReversionWasher, 
// 			 "ReversionWasher", 
// 			 "Demo:ReversionWasher", 
// 			 "Data", ReversionWasherDraw, DLL_GroupName);

ReversionWasher::ReversionWasher(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
  MBaseMethod(pUnitDef, pNd) , m_RB(this, false, "RB")
{
	//default values...
	bResTbyMud = false;       // Calculate residence time by mud volume or tank volume
	dTankVolume = 22084;      // Total tank volume
	dMudLevel = 0.3;        // Mud level as fraction of tank volume
	dMudReactionSolids = 300; // Mud solids at interface, eg 300gpl
	dUnderflowSolids = 500;   // Mud solids after squeezing out revert liquor, eg 500gpl 
	dDelta = .3;
	dK0 = 40;              // Kinetic rate constant, 490 in lab, 30 in plant!!!!
	iSteps = 50;
	dScandrettEff = .9;
	dWashBypass = 0.15; 
}

//---------------------------------------------------------------------------

void ReversionWasher::Init()
{
  //	SetRunModeOptions(MO_Probal);
	SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void ReversionWasher::BuildDataFields()
{
    DD.Text("Washer Model Data...");
    DD.CheckBox("Res.T.Mud", "",  &bResTbyMud, MF_PARAMETER|MF_SET_ON_CHANGE);
    DD.CheckBox("QAL Bayer", "", &bQALModel, MF_PARAMETER|MF_SET_ON_CHANGE);
    DD.CheckBox("ReactionB", "", &bRB, MF_PARAMETER|MF_SET_ON_CHANGE);
    DD.Double("Tank.Vol", "",      &dTankVolume, MF_PARAMETER, MC_Vol);
    DD.Double("Mud.Level", "",    &dMudLevel, MF_PARAMETER|(bResTbyMud ? 0 : MF_NO_VIEW), 
	      MC_Frac);
    DD.Double("Reactor.Solids", "",       &dMudReactionSolids, MF_PARAMETER, MC_Conc);
    DD.Double("Underflow.Solids", "",      &dUnderflowSolids, MF_PARAMETER, MC_Conc);
    DD.Double("Scandrett.Effic.", "",      &dScandrettEff, MF_PARAMETER, MC_Frac);
    DD.Double("Delta", "", &dDelta, MF_PARAMETER, MC_None);
    DD.Double("K0", "",       &dK0, MF_PARAMETER, MC_None);
    DD.Long("N Steps", "", &iSteps, MF_PARAMETER);
    DD.Long("ODE Solver", "", &iODE, MF_PARAMETER, iODERtn);
    DD.Long("ScandrettSpec", "", &iScand, MF_PARAMETER, iScandSpc);


    DD.Long  ("ThermalLossMethod", "",&iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
    DD.Show(iThermalLossMethod==THL_TempDrop);
    DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
    DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
    DD.Double("ThermalLoss", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
    DD.Show(iThermalLossMethod==THL_HeatFlowFrac);
    DD.Double("UFlow.Loss", "",  &dThermalLossU ,MF_PARAMETER, MC_UA);
    DD.Double("OFlow.Loss", "",  &dThermalLossO ,MF_PARAMETER, MC_UA);

    DD.Show();
    DD.Text("");
    DD.Text("Results...");

    DD.Double("ReversionTime", "",  &dReversionTime,     MF_RESULT|MF_NO_FILING, MC_Time);
    // DD.Double("Actual.ScandEff", "", &dAScandrettEff, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("VolumeFlow", "", &dVolume1, MF_RESULT|MF_NO_FILING, MC_Qv);
    DD.Double("InitA/C", "", &dInitAtoC,  MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("InitAConc A", "", &dInitAConc,  MF_RESULT|MF_NO_FILING, MC_None); 
    DD.Double("InitAlSat A*", "", &dInitAluminaSat, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("InitRevRt dA/dt", "", &dInitReversionRate, MF_RESULT|MF_NO_FILING,  MC_None);
    DD.Double("ReactionHt", "", &dReactionHeat, MF_RESULT|MF_NO_FILING,  MC_Pwr);
    DD.Double("Wash Bypass", "",      &dWashBypass, MF_RESULT, MC_Frac);
    DD.Page("Debug");
    for (int i=0; i<=10; i++)
	DD.Double(labels[i], "", dt+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Inlet Stream");
    const int NumSpecies = gs_MVDefn.Count();
    int ns = NumSpecies;
    if (ns>40) ns=40;
    for (int i=0; i<ns; i++)
	DD.Double(gs_MVDefn[i].Symbol(), "", dd[0]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Reactor Stream");
    for (int i=0; i<ns; i++) 
	DD.Double(gs_MVDefn[i].Symbol(), "", dd[1]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Revert Stream");
    for (int i=0; i<ns; i++) 
	DD.Double(gs_MVDefn[i].Symbol(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("..", bRB);
    m_RB.OnOffCheckBox();

    m_RB.BuildDataFields();
    DD.Show();
}

//---------------------------------------------------------------------------

void ReversionWasher::displayStream(MStream &ms, int scrn) {
	int ns = gs_MVDefn.Count();
	if (ns>40) ns = 40;       // Just in case there are more than 40 components
	for (int i=0; i<ns;  i++) dd[scrn][i] = ms.M[i];
}




void ReversionWasher::EvalProducts()
{
    try {
	FlwIOs.AddMixtureIn_Id(QI0, 0);
	FlwIOs.AddMixtureIn_Id(QI1, 1);
	MStream SideStream;
	FlwIOs.AddMixtureIn_Id(SideStream, 4);
	QI0.AddF(SideStream, MP_All, 1.0);

	MStream & QO0 = FlwIOs[FlwIOs.First[2]].Stream; // Product stream 1 - here the overflow
	MStream & QO1 = FlwIOs[FlwIOs.First[3]].Stream; // Product stream 2 - the mud underflow
	MStream QMud;    // Stream to reversion
	MStream QBypass;  // Bypass direct to overflow

	// Set the outlet streams to the inlet streams (pass through) that 
	// if unexpected errors happen
	// the model still balances.

	QO0.SetF(QI0, MP_All, 1.0);
	QO1.SetF(QI1, MP_All, 1.0);

	MIBayer & QI1B=QI1.IF<MIBayer>(false);    // Does the mud stream have Bayer properties
	if (IsNothing(QI1B)) {//Bayer properties interface not present
	    Log.Message(MMsg_Warning, "No Bayer Properties present... cannot perform reversion");
	    return;
	}
	dCMud = QI1B.CausticConc(bayerRefTemp);

	const double minMudLevel =1.0e-2;
	/// Verify that solids are present in the incoming stream
	if (QI1B.SolidsConc(bayerRefTemp) < minMudLevel) {
	    Log.Message(MMsg_Warning, "No mud present in incoming stream");
	    return;
	}
	dSolidsRho = QI1.Density(MP_Sol);

	double y = 0.0;  // Initially feed all the incoming to the
	QMud.SetF(QI1, MP_All, 1);
	QMud.AddF(QI0, MP_All, 1);
	MIBayer & QMudB = QMud.IF<MIBayer>(false);
	if (QMudB.SolidsConc(bayerRefTemp) > dMudReactionSolids) {   //Send all flow to reversion
	    QBypass.SetF(QI0, MP_All, 0.0);
	    Log.Message( MMsg_Warning, "Unable to achieve reqd mud level in plug reactor");
	} else {
	    if (dScandrettEff<1.0) {   // Scandrett eff is < 1 so solve for bypass split 

		SESolverFn s1fn(*this);
		s1fn.SetTarget(0);
		s1fn.Start(0.0, 1.0);
		s1fn.Solve_Brent();
		dWashBypass = s1fn.Result();
		double y = dWashBypass; 
		QBypass.SetF(QI0, MP_All, y);
		QMud.SetF(QI1, MP_All, 1);
		QMud.AddF(QI0, MP_All, 1-y);
	    }

	    double sol1 = QMudB.SolidsConc(bayerRefTemp);
	    double x = (dSolidsRho - dMudReactionSolids)/(dSolidsRho - sol1)*sol1/dMudReactionSolids;


	    MStream QM1;
	    QM1.SetF(QMud, MP_Sol, 1);
	    QM1.AddF(QMud, MP_Liq, x);
	    QBypass.AddF(QMud, MP_Liq, 1-x);
	    QMud.SetF(QM1, MP_All, 1);

	}
	if (bRB) {
	    MStream & QR = QMud;
	    m_RB.EvalProducts(QR);
	}


	displayStream(QMud, 0);
	double hfIn = QMud.totHf();

	double tmud = QMud.getT();

	/// Now do the actual reversion operation on the mud stream.
	/// Calculate reversion ti;e

	if (bResTbyMud)	
	    dReversionTime = dTankVolume*dMudLevel/QMud.VolumeFlow();
	else
	    dReversionTime = dTankVolume/(QI0.VolumeFlow()+QI1.VolumeFlow());
	dInitAluminaSat = QMudB.AluminaConcSat(tmud);


	ReversionStream QIR = ReversionStream(QMud, QMudB, dK0, dDelta, bQALModel);
	dInitReversionRate = QIR.dAdt();
	double h = dReversionTime/iSteps/3600;
	/// The actual reversion process... just timestep through...
	for (int i=0; i<iSteps; i++) {
	    QIR.revert2(h);
	}
		

	MStream QRevert;
	QRevert.SetF(QIR.getStream(), MP_All, 1.0);
	dReactionHeat = hfIn - QRevert.totHf();
	/// Now squeeze out liquor to provide the desired underflow mud level
	// Check to see if we can achieve desired mud underflow level
	MIBayer & QRevertB = QRevert.IF<MIBayer>(false);
	double sol1 = QRevertB.SolidsConc(bayerRefTemp);

	if (sol1 > dUnderflowSolids) {    // even adding all the bypass liquid wont get solids level right.
	    QO1.SetF(QRevert, MP_All, 1.0);  // Just send whole stream to underflow
	    QO1.AddF(QBypass, MP_All, 1.0);
	    QO0.SetF(QBypass, MP_All, 0.0);
	    Log.Message( MMsg_Warning, "Unable to achieve reqd mud level in underflow stream");
	} else {
	    double rhoSol = QRevert.Density(MP_Sol);  /// Determine underflow stream fraction
	    /// Could use dSolidsRho here, but there will be some minute change in density due
	    /// to reversion!
	    double y = (rhoSol - dUnderflowSolids)/(rhoSol - sol1)*sol1/dUnderflowSolids;
	    QO1.SetF(QRevert, MP_Sol, 1.0);  /// All the solids to underflow
	    QO1.AddF(QRevert, MP_Liq, y);    /// y of the liquids to underflow
	    QO0.SetF(QRevert, MP_Liq, 1-y);  /// the rest to overflow
	    QO0.AddF(QBypass, MP_All, 1.0);  /// and add back the bypass stream
	}

	double T1;
	double HO0 = QO0.totHf(MP_All);
	double HO1 = QO1.totHf(MP_All);

	switch (iThermalLossMethod)
	    {
	    case 0: 
		dThermalLoss = 0.0;
		dThermTempDrop = 0.0;
		break;
	    case 1: 
		QO0.putT(QO0.T-dTempDropRqd);
		QO1.putT(QO1.T-dTempDropRqd);
		dThermalLoss = HO0 + HO1 - QO0.totHf(MP_All) - QO1.totHf(MP_All);
		dThermTempDrop = dTempDropRqd;
		break;
	    case 2: 
		T1 = QO0.T; {
		    double a1 = QO0.MassFlow()/(QO0.MassFlow()+QO1.MassFlow());
		    QO0.Set_totHf(HO0-dThermalLossRqd*a1);
		    QO1.Set_totHf(HO1-dThermalLossRqd*(1-a1));
		    dThermalLoss = dThermalLossRqd;
		    dThermTempDrop = T1-QO0.T; } 
		break;
	    case 3:
		T1 = QO0.T;
		double dT=T1-AmbientTemp();
		double dH0 = dT*dThermalLossO;
		double dH1 = dT*dThermalLossU;
		QO0.Set_totHf(HO0-dH0);
		QO1.Set_totHf(HO1-dH1);
		dThermalLoss = dH0+dH1;
		dThermTempDrop = T1-QO0.T;
		break;			
	    }

	// Set the various displayed parameters.
	displayStream(QO0, 1);
	displayStream(QO1, 2);
	dVolume1 = QI0.VolumeFlow();
	dVolume2 = QI1.VolumeFlow();
	dMass1 = QMud.MassFlow();
	dInitAtoC = QMudB.AtoC();
	dInitAConc = QMudB.AluminaConc(bayerRefTemp);	

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

double ReversionWasher::scandrettBalance(double y) 
{
	MStream QM;
	MStream QB;
	MStream QMOut;

	QM.SetF(QI0, MP_All, 1-y);
	QB.SetF(QI0, MP_All, y);
	QM.AddF(QI1, MP_All, 1);
	MIBayer & qmb = QM.IF<MIBayer>(false);
	double sol1 = qmb.SolidsConc(bayerRefTemp);
	double x;		
	if (sol1 > dMudReactionSolids) {
		x = 1.0;
	} else {
		x = (dSolidsRho - dMudReactionSolids)/(dSolidsRho - sol1)*sol1/dMudReactionSolids;
	}
	QB.AddF(QM, MP_Liq, 1-x);
	QMOut.SetF(QM, MP_Sol, 1.0);
	QMOut.AddF(QM, MP_Liq, x);
	MIBayer & mbm = QMOut.IF<MIBayer>(false);
	MIBayer & mbb = QB.IF<MIBayer>(false);
	if (iScand==1) 
		return (dCMud-mbm.CausticConc(bayerRefTemp))/(dCMud-mbb.CausticConc(bayerRefTemp)) - dScandrettEff;
	else
		return (dCMud-mbm.AluminaConc(bayerRefTemp))/(dCMud-mbb.AluminaConc(bayerRefTemp)) - dScandrettEff;
};

void  ReversionWasher::ClosureInfo(MClosureInfo & CI)
{//ensure heat balance
  if (CI.DoFlows())
    {
	  CI.AddPowerIn(0, -dThermalLoss);

	  //if (iRctHeatMethod==RHM_Override1)
    CI.AddHeatFlwIn(0, -dReactionHeat, 0);
    }
}



double ReversionStream::dAdt() const
{
	double dMudTemp = MudStream.getT();
	double k = dK0*exp(-5470*(1./dMudTemp-1/t0));
	double i = MudB.IonicStrength();
	double aStar = MudB.AluminaConcSat(dMudTemp);
	double fc = MudB.CausticConc(dMudTemp)*(1-1.04*MudB.AtoC());   // Free Caustic, not in Bayer interface...
	double diff = MudB.AluminaConc(dMudTemp)-aStar;

	if (diff<0) 
		return 0.0;
	else
		return  -k*pow(diff, 2.28)*pow(fc, -4.21)*pow(.195+i, 5.)*exp(-.957*i);  
}

double ReversionStream::revert2(double deltaT) 
{
	double dX = dXALdt()*deltaT;
	double x = MudStream.M[ALUMINA]+dX;
	setX(x);
	return dX;
}

double ReversionStream::dAdMAL() const
{
	MStream QT;
	QT.SetF(MudStream, MP_All, 1);
	if (1) {
		QT.M[ALUMINA] += dDelta;
		QT.M[CAUSTIC] -= dDelta*(1.0-mwtRatio);
		QT.M[GIBBSITE] -= dDelta*mwtRatio;
	} else {
		QT.M[ALUMINA] += dDelta;
		QT.M[ALUMINAS] -= dDelta;
	}
	double t = QT.getT();
	MIBayer & QTB = QT.IF<MIBayer>(false);
	return (QTB.AluminaConc(298.15)-MudB.AluminaConc(298.15))/dDelta;
}

void ReversionStream::setX(double xAL) {
	if (1) {
		matmp = MudStream.M[ALUMINA];
		mctmp = MudStream.M[CAUSTIC];
		mgtmp = MudStream.M[GIBBSITE];
		MudStream.M[ALUMINA] = xAL;
		double delta = (matmp - xAL);
		MudStream.M[CAUSTIC]+=delta*(1.0-mwtRatio);
		MudStream.M[GIBBSITE]+=delta*mwtRatio;
	} else {
		matmp = MudStream.M[ALUMINA];
		mgtmp = MudStream.M[ALUMINAS];
		MudStream.M[ALUMINA] = xAL;
		MudStream.M[ALUMINAS] += (matmp-xAL);
	}
}


double ReversionStream::dXALdt(double xAL) {
	setX(xAL);
	double res =  dXALdt();
	restoreX();
	return res;
}

ReversionStream::ReversionStream(MStream & ms_, MIBayer & mb_, double k0_, double dD_, bool bQALModel):
  MudStream(ms_), MudB(mb_),  dK0(k0_), dDelta(dD_)  
{
	CAUSTIC = gs_MVDefn.LookupNX("NaOH(l)");
	GIBBSITE =  gs_MVDefn.LookupNX("Al[OH]3(s)");
	ALUMINA = gs_MVDefn.LookupNX("NaAl[OH]4(l)");
	mwtRatio = 0.661;
}
//====================================================================================
