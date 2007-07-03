//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//           QAL Classifier Model 2004 - Transcritical Technologies/ QAL 
//   Time-stamp: <2007-05-23 05:53:47 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __QALCLASSIFIER_CPP
#include "qalclassifier.h"



enum ThermalHeatLossMethods { THL_None, THL_TempDrop, THL_FixedHeatFlow, THL_HeatFlowFrac };
class QALClassifier;

double linMap(double x, double x1, double y1, double x2, double y2) 
{
  return y1 + (x-x1)*(y2-y1)/(x2-x1);
}


class CMaxLSplitFn: public MRootFinderBase
{
	MStream &Feed1;
	MStreamI QS;
	double solids;
	int OXALATE;

	static MToleranceBlock s_Tol;

public: 
	double Function(double x) {
		QS.SetF(Feed1, MP_Liq, x);
		QS.AddF(Feed1, MP_Sol, 1);
		QS.M[OXALATE] = x*Feed1.M[OXALATE];
		MIBayer & mb =  QS.IF<MIBayer>(false);
		return mb.SolidsConc(bayerRefTemp) - solids;
}		 
	CMaxLSplitFn(MStream &Feed1_, double solids_) : MRootFinderBase("LiquorSplit", s_Tol),
		Feed1(Feed1_), solids(solids_) { 
		OXALATE =  gs_MVDefn.LookupNX("Na2C2O4(s)");
	}
};

MToleranceBlock CMaxLSplitFn::s_Tol(TBF_Both, "QALClassifier:MaxSplit", 0.00005, 0, 100);




class CFUSolverFn : public MRootFinderBase
{
	MStream &Feed1;
	MStreamI QS;
	double solids;
	double y;
	int OXALATE;
	int iOSplit;

    static MToleranceBlock s_Tol;


public:
	double Function(double x) {
		QS.SetF(Feed1, MP_Liq, y);
		QS.AddF(Feed1, MP_Sol, x);
		if (iOSplit && OXALATE>=0) {    // special treatment of oxalate
			double oFlow = Feed1.M[OXALATE];
			QS.M[OXALATE]=y*oFlow;				//Oxalate split like liquid
		}
		MIBayer & mb =  QS.IF<MIBayer>(false);
		return mb.SolidsConc(bayerRefTemp) - solids;
	};

	CFUSolverFn(MStream &Feed1_, double solids_, double y_, int i_) : MRootFinderBase("Solids.Level", s_Tol),
	Feed1(Feed1_), solids(solids_), y(y_), iOSplit(i_) { 
		OXALATE =  gs_MVDefn.LookupNX("Na2C2O4(s)");
	}
};

MToleranceBlock CFUSolverFn::s_Tol(TBF_Both, "QALClassifier:Solids", 0.00005, 0, 100);



class PSDSolverFn : public MRootFinderBase
{
	QALClassifier & q;
    static MToleranceBlock s_Tol;

public: 
	double Function(double x) {return q.EvalBalance(x);}
	PSDSolverFn(QALClassifier & q_): MRootFinderBase("Main", s_Tol), q(q_) {}
};

MToleranceBlock PSDSolverFn::s_Tol(TBF_Both, "QALClassifier:Main", 0.00005, 0, 100);









static MInOutDefStruct s_IODefs[]=
{
	//  Desc;             Name;         Id; Rqd; Max; CnId, FracHgt;  Options;
	{ "Feed",          "Feed1",      0,   1,  5,    0,    1.0f,  MIO_In |MIO_Material },
	{ "Overflow",      "Overflow",   1,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Underflow",     "Underflow",  2,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ NULL },
};


static double QALClassifierDraw[] = {
	MDrw_Poly,  -20, 20, 20, 20, 20, -12, 0, -20, -20, -12, -20, 20, 
		MDrw_Poly,  -20, 0, 20, 0,
		MDrw_Poly,  -16, 10, 16, 10, -16, -10, 16, -10, 13, -8, 13, -12, 16, -10,  
		MDrw_End
};

static MDDValueLst  iPSDDisp[] = {
	{0, "Mass", 0},
	{1, "Frac", 0},
	{2, "Size", 0},
	NULL
};

static MDDValueLst  iControlMode[] = {
	{0, "UnderflowDensity", 0},
	{1, "Underflow.FlowRate", 0},
	{2, "Underflow.Recovery", 0},
	NULL
};


static MDDValueLst DDB3[]={
	{THL_None,          "None" },
	{THL_TempDrop,      "TempDrop"},
	{THL_FixedHeatFlow, "FixedLoss"},
	{THL_HeatFlowFrac,  "LossCoeff"},
	{0}
};



static MDDValueLst iOxSplitType[] = {
	{0, "Hydrate", 0},
	{1, "Liquid", 0},
//	{2, "SpecifyUF", 0},
	NULL
};

static MDDValueLst  iStdModeType[] = {
	{0, "UFSolRec", 0},
	{1, "OFConc", 0},
	{2, "UFVolFlow", 0},
	NULL
};

DEFINE_TRANSFER_UNIT(QALClassifier, "QALClassifier", DLL_GroupName)

void QALClassifier_UnitDef::GetOptions()
  {
  SetDefaultTag("CF");
  SetDrawing("SizeSeparation", QALClassifierDraw);
  SetTreeDescription("QAL:QAL Classifier");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina|MLicense_PSD);
  }



QALClassifier::QALClassifier(MUnitDefBase *pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
    //default values...
    bStdFn = false;
    iStdMode=0;
    dTempDropRqd = 0.5;
    dThermalLossRqd = 1500.0;
    dThermalLossU = 5;     //  Thermal losses in kW/K degrees above ambient
    dThermalLossO =10; 

    dS_x  = 180;       // See Header
    dTankDiameter = 22.3;
    dTankVolume = 5185;      // See Header
    dX_c = 0.4;        // See Header
    dS_u = 580; // See Header
    dS_cp = 1200;   // See Header 
    alpha = 2.9;
    dk_c = .05/3600;
    dCPPorosity = 0.5;
    dAFactor = 0.2;
    iPSD = 0;
    iOSplit = 0;
    dMaxAreaEfficiency = 1.0;
    iMode=0;
    dFuR = .05;
    dSR = .4;
    GIBBSITE =  gs_MVDefn.LookupNX("Al[OH]3(s)");
    if  (GIBBSITE==-1) Log.Message(MMsg_Error, "Can't find GIBBSITE in database");
    OXALATE =  gs_MVDefn.LookupNX("Na2C2O4(s)");
}

//---------------------------------------------------------------------------

void QALClassifier::Init()
{
  //	SetRunModeOptions(MO_Probal);
	SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void QALClassifier::BuildDataFields()
{
    DD.Text("Classifier Model Data...");
    DD.CheckBox("StdFunction", "",  &bStdFn, MF_PARAMETER|MF_SET_ON_CHANGE);
    
    DD.Show(bStdFn);
    DD.Long("Method", "", &iStdMode, MF_PARAMETER|MF_SET_ON_CHANGE, iStdModeType);
    DD.Double("RqdUFSolConc25", "",  &dS_u, MF_PARAMETER, MC_Conc);
    DD.Show(iStdMode==0 && bStdFn);
    DD.Double("RqdUFSolRec", "", &dSR, MF_PARAMETER, MC_Frac);
    DD.Show(iStdMode==1 && bStdFn);
    DD.Double("RqdOFSolConc25", "", &dS_o, MF_PARAMETER, MC_Conc);
    DD.Show(iStdMode==2 && bStdFn);
    DD.Double("RqdUFQv25", "", &dFuR, MF_PARAMETER, MC_Qv);

    DD.Show(!bStdFn);
    DD.Double("Classifier.Diameter", "", &dTankDiameter, MF_PARAMETER, MC_L);
    DD.Double("Classifier.Volume", "", &dTankVolume, MF_PARAMETER, MC_Vol);
    DD.Text("");
    DD.Double("Classifier.Cut.Sharpness", "",      &alpha, MF_PARAMETER, MC_None);
    DD.Double("Max.Area.Effic", "", &dMaxAreaEfficiency, MF_PARAMETER, MC_None);
    DD.Double("Compression.Point", "",     &dS_cp, MF_PARAMETER, MC_Conc);
    DD.Double("A.Factor", "", &dAFactor, MF_PARAMETER, MC_None);
    DD.Double("Compress.Point.Porosity", "",    &dCPPorosity, MF_PARAMETER, MC_None);
    DD.Double("Class.Zone.UF.Solids", "",      &dS_x, MF_PARAMETER, MC_Conc);
    DD.Double("Solids.Conc.Su", "",      &dS_u, MF_PARAMETER, MC_Conc);
    DD.Double("Compression.Rate", "",       &dk_c, MF_PARAMETER, MC_pS);
    DD.Double("Crit.Class.Vol.Ratio", "",    &dX_c, MF_PARAMETER, MC_Frac);
    DD.Long("Oxalate.Split", "", &iOSplit, MF_PARAMETER|MF_SET_ON_CHANGE, iOxSplitType);
    DD.Double("Ox.UFlow.Lvl", "",  &dOxUFlow, MF_PARAMETER|(iOSplit<2 ? MF_NO_VIEW : 0), MC_Frac);
    DD.Long("Control.Mode", "", &iMode, MF_PARAMETER|MF_SET_ON_CHANGE, iControlMode);
    DD.Show(iMode==0);
    DD.Show(iMode==1);
    DD.Double("ReqdUFlow", "", &dFuR,  MF_PARAMETER, MC_Qv);
    DD.Show(iMode==2);
    DD.Double("ReqdSolidsRecovery", "", &dSR,  MF_PARAMETER, MC_Frac);
    DD.Show();

    DD.Text("");
    DD.Long  ("ThermalLossMethod", "", &iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
    DD.Show(iThermalLossMethod==THL_TempDrop);
    DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
    DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
    DD.Double("ThermalLossRqd", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
    DD.Show(iThermalLossMethod==THL_HeatFlowFrac);
    DD.Double("UFlow.Loss", "",  &dThermalLossU ,MF_PARAMETER, MC_UA);
    DD.Double("OFlow.Loss", "",  &dThermalLossO ,MF_PARAMETER, MC_UA);

    DD.Show();
    DD.Page("Results...");

    DD.Double("Solids.Density", "",  &drhoSol, MF_RESULT|MF_NO_FILING, MC_Rho);

    DD.Double("Class.Area.Eff", "", & dClassEff, MF_RESULT|MF_NO_FILING, MC_None);				// Classifier Area Efficiency
    DD.Double("Overflow.Visc", "", & dOverflowViscosity, MF_RESULT|MF_NO_FILING, MC_Visc);
    DD.Double("Overflow.Liq.Visc", "", & dOverflowLiqVisc, MF_RESULT|MF_NO_FILING, MC_Visc);
    DD.Double("Overflow.Den", "", & dOverflowDensity, MF_RESULT|MF_NO_FILING, MC_Rho);
    DD.Double("Overflow.Sol.Den", "", & dOverflowSolidsDensity,  MF_RESULT|MF_NO_FILING, MC_Rho);
    DD.Double("Overflow.Kine.Visc", "", & dOverflowNu, MF_RESULT|MF_NO_FILING, MC_None);             // Overflow kinematic viscosity, for reynolds no calcs.
    DD.Double("Upflow.Velocity", "", & dUpflowVelocity,     MF_RESULT|MF_NO_FILING, MC_Ldt);
    DD.Double("Settling.Size", "", & d_50c,     MF_RESULT|MF_NO_FILING, MC_L);
    DD.Double("Particle.Re", "", &dRe,  MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("Rf", "", &dRf, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("Vcom", "", &dVcom, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("Solids%", "", &solidsToUnderflow,  MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("UFSolidsMassFlow", "", &dSolidsUnderflow, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("Underflow.Solids", "", &dUnderflowSolids,  MF_RESULT|MF_NO_FILING, MC_None);
    DD.Double("WtFr45", "", &dWtFr45 ,  MF_RESULT|MF_NO_FILING, MC_None);

    DD.Double("MaxLiqSplit", "", &dMaxLSplit, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Text("");
    DD.Double("ThermalLoss", "",      &dThermalLoss   ,MF_RESULT, MC_Pwr("kW"));
    DD.Double("ThermalTempDrop", "",  &dThermTempDrop ,MF_RESULT, MC_dT("C"));

#ifdef TTDEBUG
    DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER, iPSDDisp);
    DD.Page("Inlet");
    const int NumSpecies = gs_MVDefn.Count();
    int ns = NumSpecies;
    if (ns>40) ns=40;
    for (int i=0; i<ns; i++)
	DD.Double(gs_MVDefn[i].Symbol(), "", dd[0]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Overflow");
    for (int i=0; i<ns; i++) 
	DD.Double(gs_MVDefn[i].Symbol(), "", dd[1]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Underflow");
    for (int i=0; i<ns; i++) 
	DD.Double(gs_MVDefn[i].Symbol(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Size In");
    for (int i=0; i<30; i++) 
	DD.Double("Size...", "", dd[3]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Size OF");
    for (int i=0; i<30; i++) 
	DD.Double("Size...", "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Size UF");
    for (int i=0; i<30; i++) 
	DD.Double("Size...", "", dd[5]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("Split.uf.of");
    for (int i=0; i<30; i++) 
	DD.Double("Split...", "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
    DD.Page("bla");
    for (int i=0; i<30; i++) 
	DD.Double("Split...", "", dd[7]+i, MF_RESULT|MF_NO_FILING, MC_None);
#endif

}



void QALClassifier::EvalProducts() {

  try {
    FlwIOs.AddMixtureIn_Id(QI, 0);
    MStream & QO0 = FlwIOs[FlwIOs.First[1]].Stream;  // Product stream 1 -overflow
    MStream & QO1 = FlwIOs[FlwIOs.First[2]].Stream;  // Product stream 2 -mud underflow


    dTankArea = CircleArea(dTankDiameter);
    drhoSol = QI.Density(MP_Sol);
    dSolidsFlow = QI.MassFlow(MP_Sol);



    // Set the outlet streams to the inlet streams (pass through) 
    // so that if unexpected errors happen
    // the model still balances.
    // Check whether this incorporates properties as well...
    QO0 = QI;
    QO0.SetF(QI, MP_All, 0.5);
    QO1 = QI;
    QO1.SetF(QI, MP_All, 0.5);

    if (dSolidsFlow < 1.0e-6) {
      Log.Message(MMsg_Warning, "No solids in Feed");
      return;
    }

    if (dS_x > dS_u) 
      Log.Message(MMsg_Warning, "Compression Solids greater than UFlow Solids");


    MIBayer & QIB=QI.IF<MIBayer>(false);    // Does the mud stream have Bayer properties
    if (IsNothing(QIB)) {//Bayer properties interface not present
      Log.Message(MMsg_Warning, "No Bayer Properties present... cannot continue");
      return;
    }


    dS_i = QIB.SolidsConc(bayerRefTemp);    // need this for various stuff later.

    if (bStdFn) {   // Standard Functionality - doesnt need PSD
      double x, y;
      switch (iStdMode) {
      case 0: 
	y = dSR;
	x = (drhoSol-dS_u)/(drhoSol-dS_i)*dS_i/dS_u*y;
	if (x>1) x=1;
	break;
      case 1: 
	x = (dS_i-dS_o)*(drhoSol-dS_u)/((dS_u-dS_o)*(drhoSol-dS_i));
	y = (dS_i-dS_o)*dS_u/((dS_u-dS_o)*dS_i);
	break;
      case 2:
	y = dFuR/QI.VolumeFlow(MP_All,bayerRefTemp)*dS_u/dS_i;
	x = y*dS_i/dS_u*(drhoSol-dS_u)/(drhoSol-dS_i);
	break;
      }
      if (x<0.) x=0.;
      if (x>1.) x=1.;
      if (y<0.) y=0.;
      if (y>1.) y=1.;	      
      QO1.SetF(QI, MP_Liq, x);
      QO1.AddF(QI, MP_Sol, y);
      QO0.SetF(QI, MP_Liq, 1-x);
      QO0.AddF(QI, MP_Sol, 1-y);
  
    } else {   // PSD Model
      MIPSD & MudPSD = QI.IF<MIPSD>(false);   /// Does the mud stream have PSD...
      if (IsNothing(MudPSD)) {
	Log.Message(MMsg_Warning, "No PSD Properties present... cannot continue");
	return;
      }
      dWtFr45 = wtSub45(QI);	// Initialize to input value
      switch(iMode) {
      case 0:   /// Specified underflow solids - here use built in solver to find root...
	{
	  if (dS_i >= dS_u) { // All liquor will still give solids reqd in underflow
	    Log.Message(MMsg_Warning, "Inlet Solids greater than reqd underflow solids");
	    return;
	  } else {
	    if (!iOSplit) {
	      dMaxLSplit = (drhoSol-dS_u)/(drhoSol-dS_i)*dS_i/dS_u;
	    } else {
	      CMaxLSplitFn f(QI, dS_u);
	      f.SetTarget(0);
	      f.Start(0,1);
	      f.Solve_Brent();
	      dMaxLSplit = f.Result();
	    }
	  }
	    
	    
	  PSDSolverFn f(*this);
	  f.SetTarget(0);
	  f.Start(1.0e-6, dMaxLSplit-1.0e-6);
	  f.Solve_Brent();
	  double dfux = f.Result();
	  dUnderflowSolids = EvalBalance(dfux); 
	  break;
	}
	    
      case 1:   // Specified underflow flow  - in this case use iteration
	{
	      
	  double r = dFuR/QI.VolumeFlow();   // Start by dividing the flows 
	  QUnderflow.SetF(QI, MP_All, r);    // as required approximately
	  QOverflow.SetF(QI, MP_All, 1-r);
	  doSolidsUnderflow();
	  break;
	}
	    
      }
      QO0.SetF(QOverflow, MP_All, 1.0);
      QO1.SetF(QUnderflow, MP_All, 1.0);

    }

    ///  Now do the environmental heat loss:

    double T1;
    double HO0 = QO0.totHf(MP_All);
    double HO1 = QO1.totHf(MP_All);
    switch (iThermalLossMethod)
      {
      case THL_None: 
	dThermalLoss = 0.0;
	dThermTempDrop = 0.0;
	break;
      case THL_TempDrop: 
	QO0.putT(QO0.T-dTempDropRqd);
	QO1.putT(QO1.T-dTempDropRqd);
	dThermalLoss = HO0 + HO1 - QO0.totHf(MP_All) - QO1.totHf(MP_All);
	dThermTempDrop = dTempDropRqd;
	break;
      case THL_FixedHeatFlow: 
	T1 = QI.T; {
	  double a1 = QO0.MassFlow()/QI.MassFlow();
	  QO0.Set_totHf(HO0-dThermalLossRqd*a1);
	  QO1.Set_totHf(HO1-dThermalLossRqd*(1-a1));
	  dThermalLoss = dThermalLossRqd;
	  dThermTempDrop = T1-QO0.T; } 
	break;
      case THL_HeatFlowFrac:
	T1 = QI.T;
	double dT=T1-AmbientTemp();
	double dH0 = dT*dThermalLossO;
	double dH1 = dT*dThermalLossU;
	QO0.Set_totHf(HO0-dH0);
	QO1.Set_totHf(HO1-dH1);
	dThermalLoss = dH0+dH1;
	dThermTempDrop = T1-QO0.T;
	break;			

	
      }

#ifdef TTDEBUG
    displayPSD(QI,3);
    displayStream(QI, 0);
    displayStream(QO0, 1);
    displayStream(QO1, 2);

    displayPSD(QOverflow, 4);
    displayPSD(QUnderflow, 5);
#endif

    //get display values...
    //dProdQm1 = QO1.MassFlow();
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

// A single loop of the calcs, given a split of the incoming liquid 'x' to underflow.
// Return the error in solids to underflow


double QALClassifier::EvalBalance(double x) {
	double y;
	if (!iOSplit) 
		y = x/dMaxLSplit;
	else {
		CFUSolverFn f(QI, dS_u, x, iOSplit);
		f.SetTarget(0.);
		f.Start(0.0, 1.0);
		f.Solve_Brent();
		y = f.Result();
	}
		
	QUnderflow.SetF(QI, MP_Liq, x);    // Construct appropriate overflow and underflow streams.
	QUnderflow.AddF(QI, MP_Sol, y);
	QOverflow.SetF(QI, MP_Liq, 1-x);
	QOverflow.AddF(QI, MP_Sol, 1-y);
	if (iOSplit && OXALATE>=0) {    // special treatment of oxalate
		double oFlow = QI.M[OXALATE];
		QUnderflow.M[OXALATE]=x*oFlow;				//Oxalate split like liquid
		QOverflow.M[OXALATE]=(1-x)*oFlow;
	}




	dSolidsUnderflow = QUnderflow.MassFlow(MP_Sol);
	double totSol = QUnderflow.MassFlow(MP_Sol)+QOverflow.MassFlow(MP_Sol);

	dFu = QUnderflow.VolumeFlow(MP_All, bayerRefTemp);

	MIPSD & mpsd = QI.IF<MIPSD>(false);				// Construct PSD class references
	MIPSD & QOPSD = QOverflow.IF<MIPSD>(false);
	MIPSD & QUPSD = QUnderflow.IF<MIPSD>(false);
	MIBayer & QUB = QUnderflow.IF<MIBayer>(false);   // and Bayer props for underflow

	dRf = fRf();				// Determine  artificial stream split
	dVcom = fVcom();			// Determine size of compression zone
	double cvx = 1.0-dVcom/dTankVolume;   // Classification volume/total volume
	dClassEff = clAE(cvx);             // Compression Zone Efficiency
	if (dClassEff>dMaxAreaEfficiency) dClassEff = dMaxAreaEfficiency;
	calcOverflowProperties();			// Determine overflow properties from constructed stream
	dUpflowVelocity = upflowVelocity();  
	d_50c = dP();

	int ic = mpsd.getSizeCount();      // Now get the number of sizes in incoming stream
	double us = 0.0;
	double os = 0.0;
	for (int i=0; i<ic; i++) {
		double sp = mpsd.getSize(i);     // Size in microns, should use geom. mean?
		if (i) sp = sqrt(sp*mpsd.getSize(i-1));  // Nominal size
		double mp = mpsd.getMass(i);     // Mass passing of this size
		double mf = mpsd.getFrac(i);     // Mass passing of this size
		double z = fG(sp);                      // Partition fraction function
		if (z<0.0 || z>1.0) 
			Log.Message(MMsg_Error, "Fraction passing out of range!");
		us += z*mp; // Total mass
		os += (1-z)*mp;
	}  
	for (int i=0; i<ic; i++) {
		double sp = mpsd.getSize(i);     // Size in microns, should use geom. mean?
		if (i) sp = sqrt(sp*mpsd.getSize(i-1));  // Nominal size
		//		double mp = mpsd.getMass(i);     // Mass passing of this size
		double mf = mpsd.getMass(i);     // Mass passing of this size
		double z = fG(sp);                      // Partition fraction function
		if (z<0.0 || z>1.0) 
			Log.Message(MMsg_Error, "Fraction passing out of range!");
		double zz = fG(sp);                      // Partition fraction function
		

		QOPSD.putFrac(i, (1-zz)*mf/os);      // and divvy it up between the two streams.
		QUPSD.putFrac(i,  zz*mf/us);
	}  
	dWtFr45 = wtSub45(QOverflow);
	solidsToUnderflow = us;	// Fraction Solids to Underflow
	dPSDSolidsFlow = QUnderflow.M[GIBBSITE];

	return us  -dPSDSolidsFlow;
}


void QALClassifier::doSolidsUnderflow() {
	MIPSD & mpsd = QI.IF<MIPSD>(false);				// Construct PSD class references
	MIPSD & QOPSD = QOverflow.IF<MIPSD>(false);
	MIPSD & QUPSD = QUnderflow.IF<MIPSD>(false);
	MIBayer & QUB = QUnderflow.IF<MIBayer>(false);   // and Bayer props for underflow

	for (int k=0; k<100; k++) {
		const int NumSpecies = gs_MVDefn.Count();
		dFu = QUnderflow.VolumeFlow(MP_All, bayerRefTemp);
		dRf = fRf();				// Determine  artificial stream split
		dVcom = fVcom();			// Determine size of compression zone
		double cvx = 1.0-dVcom/dTankVolume;   // Classification volume/total volume
		dClassEff = clAE(cvx);             // Compression Zone Efficiency
		if (dClassEff>dMaxAreaEfficiency) dClassEff = dMaxAreaEfficiency;
		calcOverflowProperties();			// Determine overflow properties from constructed stream
		dUpflowVelocity = upflowVelocity();  
		d_50c = dP();
		double xz =QUnderflow.VolumeFlow();
		double xy =QI.M[GIBBSITE];
		double xx = QUnderflow.M[GIBBSITE];
		int ic = mpsd.getSizeCount();      // Now get the number of sizes in incoming stream
		double us = 0.0;
		double os = 0.0;
		for (int i=0; i<ic; i++) {
			double sp = mpsd.getSize(i);     // Size in microns, should use geom. mean?
			if (i) sp = sqrt(sp*mpsd.getSize(i-1));  // Nominal size
			double mp = mpsd.getMass(i);     // Mass passing of this size
			double z = fG(sp);                      // Partition fraction function
			if (z<0.0 || z>1.0) 
				Log.Message(MMsg_Error, "Fraction passing out of range!");
			us += z*mp; // Total mass
			os += (1-z)*mp;
			//QOPSD.putMassPassing(i, (1-z)*mp); 
			//QUPSD.putMassPassing(i,  z*mp);
			QOPSD.putFrac(i, (1-z));      // and divvy it up between the two streams.
			QUPSD.putFrac(i,  z);

		}  
		dWtFr45 = wtSub45(QOverflow);
		solidsToUnderflow = us;	// Fraction Solids to Underflow
		QUnderflow.M[GIBBSITE] = us;
		QOverflow.M[GIBBSITE] = QI.M[GIBBSITE]-us;
		double dFuA = QUnderflow.VolumeFlow();
		if (fabs(dFuA-dFuR)<1.0e-7 && k>3) break;
		double r = dFuA/dFuR;
		for (int j=0; j<NumSpecies; j++) {
			QUnderflow.M[j] /= r;
			QOverflow.M[j] = QI.M[j]-QUnderflow.M[j];
		}
		QUnderflow.MarkStateChanged();
		QOverflow.MarkStateChanged();
	}
}










double clAE(double x)   // Classifier Area Efficiency 
{
  if (x < clAExMin) return clAEyMin;
  if (x > clAExMax) return clAEyMax;
  return linMap(x, clAExMin, clAEyMin, clAExMax, clAEyMax);
}



double QALClassifier::fRf() 
{
	double fi = QI.VolumeFlow(MP_All, bayerRefTemp);
	double susx = dS_u/dS_x;
	if (susx>1.0) susx=1.0;
	return susx*(1.-dS_x/drhoSol)/(fi/dFu*(1-dS_i/drhoSol)+susx-1.);
}

double QALClassifier::fVcom()    // Volume of compression zone 
{
	double vol = dFu*dS_u/(dS_x*dk_c)*log((dS_cp-dS_x)/(dS_cp-dS_u));
	if (vol > dTankVolume) vol = dTankVolume;
	if (vol < 0.0) vol = 0.0;
	return vol;
}

double QALClassifier::fG(double x)  /// partition curve
{

  double expaxd = exp(-alpha*x/d_50c);
  return dRf+(1-dRf)*(1-expaxd)/(1+expaxd*(exp(alpha)-2));
}





void QALClassifier::calcOverflowProperties() {    // May need to modify this for QAL correlations.
	MIBayer & QOB=QOverflow.IF<MIBayer>(false);    // Does the mud stream have Bayer properties


	double oft = QOverflow.getT();
	dOverflowDensity = QOverflow.Density();
	dOverflowLiqVisc = QOverflow.DynamicViscosity(MP_Liq);
	

	double OFSolidsConc = QOB.SolidsConc(oft);
	double OFSolidsRho  = QOverflow.Density(MP_Sol);

	double viscCorrection = pow(1-(1-(1-OFSolidsConc/OFSolidsRho))/(1-dCPPorosity), dWtFr45*dAFactor*(dCPPorosity-1));


	dOverflowViscosity = dOverflowLiqVisc*viscCorrection;

	dOverflowNu = dOverflowViscosity/dOverflowDensity;
	dOverflowSolidsDensity = QOverflow.Density(MP_Sol);
	dOverflowDDiff = dOverflowSolidsDensity - dOverflowDensity;		// Density difference between particles and liquor
}






class CdPSolverFn : public MRootFinderBase    /// Solve for settling velocity==upflow velocity.
{
	QALClassifier & qalc;
	static MToleranceBlock s_Tol;

public:
	double Function(double x)   // return balance for particle size x
	{
		if (x==0.0) 
			return -qalc.dUpflowVelocity;
		else {
			double	dRe = qalc.dUpflowVelocity*x/qalc.dOverflowNu;
			qalc.dRe = dRe;
			double dCD = 24/dRe*(1.+0.14*pow(dRe, 0.7));
			double u = sqrt(4.0*9.80*x*qalc.dOverflowDDiff/(3.0*qalc.dOverflowDensity*dCD));
			return u - qalc.dUpflowVelocity;
		}
	};

	CdPSolverFn(QALClassifier &qalc_) : MRootFinderBase("d_50c", s_Tol),
	qalc(qalc_) { }
};

MToleranceBlock CdPSolverFn::s_Tol(TBF_Both, "QALClassifier:d_50c", 0.00005, 0, 100);


double QALClassifier::dP() {

	CdPSolverFn f(*this);
	f.SetTarget(0);
	f.Start(1.0e-12, 1.0);
	if (f.Function(1.0) < 0.0) {
		Log.Message(MMsg_Warning, "Excessive Upflow Velocity...");
		return 1.0;
	}
	f.Solve_Brent();
	return f.Result();
}


double QALClassifier::upflowVelocity() {
	return QOverflow.VolumeFlow()/(dTankArea*dClassEff);
}

double wtSub45(MStream & QS) {    // For a given stream, calculate wt % passing 45 micron screen
	MIPSD & QSPSD = QS.IF<MIPSD>(false);   /// Does the stream have PSD...
	int ic = QSPSD.getSizeCount();
	double s45 = 0.0;
	double stot = 0.0;
	double spprev = 0.0;
	for (int i=0; i<ic; i++) {
		double mp = QSPSD.getMass(i);
		stot += mp;
		double sp = QSPSD.getSize(i);
		if (sp <= d45micron) s45+=mp;
		if (sp > d45micron && spprev <=d45micron)      // Linear interpolation if in the middle of interval
			s45+=mp*(d45micron - spprev)/(sp-spprev);
		spprev = sp;
	}

	return 100*s45/stot;
}


void QALClassifier::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, -dThermalLoss);

    //if (iRctHeatMethod==RHM_Override1)
    //  HtOut += dReactionHeat;
    }
  }



#ifdef TTDEBUG

void QALClassifier::displayStream(MStream &ms, int scrn) {
   int ns = gs_MVDefn.Count();
   if (ns>40) ns = 40;       // Just in case there are more than 40 components
   for (int i=0; i<ns;  i++) dd[scrn][i] = ms.M[i];
}

void QALClassifier::displayPSD(MStream &ms, int scrn)
{
	MIPSD & mpsd = ms.IF<MIPSD>(false);   /// Does the mud stream have PSD...
	int ic = mpsd.getSizeCount();
	if (ic>40) ic=40;
	double x;
	for (int i=0; i<ic; i++) {
		switch (iPSD) {
			case 0:
				x = mpsd.getMass(i);
				break;
			case 1: 
				x = mpsd.getFrac(i);
				break;
			case 2: 
				x = mpsd.getSize(i);
				break;
		}
		dd[scrn][i]=x;
	}
}
#endif
