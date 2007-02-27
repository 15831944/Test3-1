// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __BELTFILT_CPP
#include "beltfilt.h"
//#include "optoff.h"

//==========================================================================
/*#D:#T:Belt Filter
#X:Reference : Chemical Engineering Journal Predict performance of belt filter washing.  A.Tomiak.
#n#n#h<General Description>
#nThis model is used to simulate a belt filter with counter current washing.
The main requirements for using this model are : a feed containing solids and liquids, 
a wash water stream, and data on filtration losses for a simple filtration wash.  
If the unit does not require washing, the user does not have to specify a wash loss curve or number of stages.  
The user must still specify solids moisture.

#nThe model will simulate any number of counter current washing steps, from 1 upwards.  The 
wash water is assumed to be added to the final washing stage, with the filtrate from each 
stage being added as a wash to the previous stage.  The model has two required outputs, the 
final filtrate and the washed cake, and a optional output, the washings stream.  If the user does not
connect the washings stream, then the model will combine the washings with the filtrate.
#nThe washing loss vs. wash ratio curve for simple filtration washing is used to determine 
the wash losses of each washing stage. The wash loss should be specified as a fraction, 
i.e. 0 to 1.
#nFor example, the wash loss for stage one, f1, is found from the curve using :
#nWash ratio (N) = Mass of wash water / Mass of solution left in cake
#nLosses on stage 2 (f2) are found from 2 * N, stage 3 from 3 * N, etc.

#n#n#b<Assumptions>
#n1. Ideal displacement washing is assumed, i.e. mass liquid phase hold-up in the cake is constant throughout the washing process.
#n3. No solids are lost to the filtrate.

#n#n#h<Variables to be supplied by the user>
#n#i<Stages?> : The user must insert the number of counter current washing stages required.
#n#i<Sol Moist> : This is the percentage of liquids to solids in the final washed cake, the default is 10%.
#n#i<WashLoss curve> : The user is required to supply the data points for the washing losses 
for simple filtration washing.  These can be read in from a spreadsheet file saved as a comma 
delimited file (CSV), or inputted directly into the model.  The wash losses must be read in as 
a fraction, i.e. from 0 to 1.

#n#n#h<Additional information available from the model>
#n#i<WashRatio:>The calculated wash ratio, N, on the Belt filter.   N = Mass of Wash Water / Mass of Liquid in Cake.
#n#n#h<Other>
Default model prefix:BT#n
Short name:BeltFilt#n
Model type:Unit#n
#G:Units
*/
//==========================================================================
                                
static MInitialiseTest InitTest(&BeltFiltClass);
SPECIEBLK_L(InitTest, H2O, "H2O(l)", true);

const byte ioidFd = 0;
const byte ioidWS = 1;
const byte ioidFt = 2;
const byte ioidSo = 3;
const byte ioidWh = 4;

static IOAreaRec BeltFiltIOAreaList[] =
  {{"Feed",                       "Feed",       ioidFd, LIO_In0 ,   nc_MLnk, 1, 10, IOGRP(0)},
   {"Wash solution",              "WashSoln",   ioidWS, LIO_In1 ,   nc_MLnk, 0, 5,  IOGRP(0)},
   {"Liquid filtrate",            "Filtrate",   ioidFt, LIO_Out0,   nc_MLnk, 1, 1,  IOGRP(0)},
   {"Solid product",              "Solids",     ioidSo, LIO_Out1,   nc_MLnk, 1, 1,  IOGRP(0)|IOSetXfer},
	 {"Liquid washings",            "Washings",   ioidWh, LIO_Out,    nc_MLnk, 0, 1,  IOGRP(0)},
   {NULL}};

static double Drw_BeltFilt[] = { DD_Arc, -16, 0, 2, 
												DD_Poly, -16,2, 16,2,
												DD_Arc, 16, 0, 2, 
												DD_Poly, 16,-2, 0,-6, -16,-2,
                        DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(BeltFilt, "BeltFilt", "1", Drw_BeltFilt, "Filter", "BT", TOC_PROBAL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Belt Filter",
                    "Belt Filter") 

//BEGIN_TAGOBJSELECTABLE(BeltFilt)              
//virtual bool Selectable() { return H2O.Exists(); }
//END_TAGOBJSELECTABLE()              

BeltFilt::BeltFilt(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  QFd("QFd", this, TOA_Embedded),
  WashLossCurve("DataOnly", "WashLoss", this, TOA_Embedded)
  {
  AttachIOAreas(BeltFiltIOAreaList);
  WR              = 1.0;
  RqdCakeMoist    = 0.1;
  ActCakeSolids   = 0.0;
  ActFiltSolids   = 0.0;
  ActCakeSolConc  = 0.0;
  ActFiltSolConc  = 0.0;
  bTrackStatus    = true;
  #ifndef _RELEASE
  iTrackSpecie    = 0;
  #endif
	FiltrateFracinWashedCake = 0.1;
  SetNoOfStages(1);
  }

//--------------------------------------------------------------------------

BeltFilt::~BeltFilt()
  {
  }

//--------------------------------------------------------------------------

XID xid_Stages        = MdlBsXID(4401);

void BeltFilt::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text    ("");
  DDB.Text    ("Requirements");
  if (PrjFileVerNo()<49)
    {
  	DDB.Long    ("No_Of_Stages",            "Stages?",         DC_,     "",       xid_Stages,          this, isParm);
    DDB.Double  ("Solids_Moisture",         "SolMoist",        DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
    }
  else
    {
  	DDB.Long    ("No_Of_Stages",            "Stages",          DC_,     "",       &Stage,              this, isParm);
    DDB.Double  ("Rqd_Cake_Moisture",       "RqdCakeMoist",    DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
    }
  #ifndef _RELEASE
  DDB.Byte    ("",                        "DisplayComp",     DC_,     "",       &iTrackSpecie,       this, isParm, SDB.DDBLiqSpList());
  #endif

  DDB.Text    ("Results");
	DDB.Double  ("",                        "WashRatio",       DC_,     "",       &WR,                 this, isResult|0);
	DDB.Double  ("Wash_Efficiency",			    "WashEfficiency",  DC_Frac, "%",      &WashEff,						 this, isResult|0);
	DDB.Double  ("FracFeedLiquorinCake",    "FracFeedinCake",  DC_Frac, "%",      &FiltrateFracinWashedCake, this, isResult|0);
  DDB.Double  ("Cake_Solids",             "CakeSolids",      DC_Frac, "%",      &ActCakeSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("Filtrate_Solids",         "FiltSolids",      DC_Frac, "%",      &ActFiltSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("CakeSolidsConc@25",       "CakeSolConc25",   DC_Conc, "g/L",    &ActCakeSolConc,     this, isResult|InitHidden);//|noFile|noSnap);
  DDB.Double  ("FiltrateSolidsConc@25",   "FiltSolConc25",   DC_Conc, "g/L",    &ActFiltSolConc,     this, isResult|InitHidden);//|noFile|noSnap);
  DDB.Text    ("");
  #ifndef _RELEASE
  char Buff[1024];
  for (int i=0; i<=Stage; i++)
    {
		sprintf(Buff, "%2d: CakeConc:%g", i, CakeConc[i]);
    DDB.Text    (Buff);
    }
  #endif

  BuildDataDefnElevation(DDB);
  if (SolveDirectMethod())
    DDB.Text    ("");
  DDB.Visibility(SM_Direct|HM_All);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();
  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  
  DDB.Object(&WashLossCurve, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  }

//---------------------------------------------------------------------------

flag BeltFilt::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xid_Stages:
      if (DCB.rL)
        {
        //if (*DCB.rL!=Stage)
        //  StructureChanged(this);
        SetNoOfStages(*DCB.rL);
        }
      DCB.L = Stage;
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

void BeltFilt::ConfigureJoins()
  {
  for (int i=0; i<NoProcessIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  }

//---------------------------------------------------------------------------

void BeltFilt::SetNoOfStages(long NewCount)
  {
  Stage = Range(0L, NewCount, 100L);
  #ifndef _RELEASE
	CakeConc.SetSize(Stage + 1);
  #endif
  }

//--------------------------------------------------------------------------

void BeltFilt::filt(CDVector& w, CDVector& d, long max)
  {
  CDVector BFconst(max);
  BFconst[0] = WR - d[1];
	BFconst[1] = BFconst[0] + d[2];
	w[2] = (WR * w[1] - d[1])/BFconst[0];
	for (int i=3; i<=max; i++)
    {
		w[i] = BFconst[1] * w[i-1] - d[i-1];
		for (int k=3; k<i; k++)
			{
			BFconst[k-1] = d[k-1] - d[k];
			w[i] = w[i] - BFconst[k-1] * w[i-k+1];
			}
		w[i] = w[i] /BFconst[0];
		}
  }

//--------------------------------------------------------------------------

void BeltFilt::washcon(CDVector& w, CDVector& c, long temp)
  {
  c[0] = 1.0;
  for (int i = 1; i<temp; i++)
    c[i] = c[i-1] - WR * (w[i] - w[i+1]);
  }

//--------------------------------------------------------------------------

void BeltFilt::Conc(CDVector& d, long temp, double Cin)
  {
  CDVector Wassumed(temp+1);
	CDVector c(temp);
	CDVector w(temp+1);
	
	Wassumed[1] = 0.9;
	filt(Wassumed, d, temp);
 	w[1] = (Wassumed[1] - Wassumed[temp])/GTZ(1.0 - Wassumed[temp]);
	filt(w, d, temp);
	washcon(w, c, temp);
	
	//Calculate the fraction of filtrate left in the cake and hence the wash efficiency.
	FiltrateFracinWashedCake = Max(0.0,c[temp-1]);
	WashEff = 1.0 - FiltrateFracinWashedCake;
	
  #ifndef _RELEASE
	// Calculate concentration in cake per stage for selected specie
	for (int i=0; i<temp; i++)
		CakeConc[i] = Max(0.0, c[i]) * Cin;
  #endif
	}
 
//--------------------------------------------------------------------------

void BeltFilt::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double P = PBPress();//AtmosPress(IODatum_Term(j));
        SetJoinPressure(j, P);
        }
      break;
      }
    case NM_Dynamic:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    }
  }
 
//--------------------------------------------------------------------------
/*#F:
This determines what material should leave through each outlet,and rate of 
change of the contents resulting from the flow of material out of each outlet
of the surge unit.
*/
void BeltFilt::EvalProducts(CNodeEvalIndex & NEI)
  {
	//input streams
  const int iFeed     = IOWithId_Self(ioidFd);
  const int iWw				= IOWithId_Self(ioidWS);
  //output streams
  const int iFiltrate = IOWithId_Self(ioidFt);
  const int iCake     = IOWithId_Self(ioidSo);
  const int iWashings = IOWithId_Self(ioidWh);

  SpConduit & Qs = *IOConduit(iCake);
  SpConduit & Qf = *IOConduit(iFiltrate);

  if (fDoDbgBrk)
    { int xxx=0; }

  #ifndef _RELEASE
  CakeConc.SetAll(0.0);
  #endif
	FiltrateFracinWashedCake = 1.0;
	WashEff = 0.0;

  /* First do the work for the filtration section of the Belt Filter.  This merely separates the solids from liquids
		 in the feed, then calculates the amount of filtrate in the formed cake based on the user defined cake moisture.*/
	QFd.QZero();
	SigmaQInPMin(QFd, som_ALL, Id_2_Mask(ioidFd));
	SigmaQInPMin(QWs, som_ALL, Id_2_Mask(ioidWS));
  flag HasFlw = (QFd.QMass(som_ALL)+QWs.QMass(som_ALL)>UsableMass);

  double QmSIn = QFd.QMass(som_Sol);
  double QmLIn = QFd.QMass(som_Liq);
	RqdCakeMoist = Range(0.001, RqdCakeMoist, 0.99);

  bool WaterError = false;

  double SoLiq, FtLiq;
  if (QmLIn > 0.0)
    {
		const double LT = QmSIn * RqdCakeMoist/(1.0 - RqdCakeMoist);
    SoLiq = Range(0.0, LT, QmLIn);
    FtLiq = QmLIn - SoLiq;
    }
  else
    {
    SoLiq = 0.0;
    FtLiq = 0.0;
    }
  if (QmLIn <1e-5)
    QmLIn = 1.0;

	Qwash.QZero();
  Qs.QZero();
  Qf.QZero();
  Qs.QSetM(QFd, som_Sol, QmSIn, Std_P);
	Qf.QSetM(QFd, som_Sol, 0.0, Std_P);
	Qf.QAddM(QFd, som_Liq, FtLiq);

  /*Then do the work for the washing section of the Belt filter.*/
	WR = 0.0;
  if ((iWw >= 0) && (QmSIn > 1.0e-3) && (Stage > 0) && (SoLiq > 0.0))
    {
    double QmWIn = QWs.QMass(som_Liq); //total wash liq
		double QmWSolIn = QWs.QMass(som_Sol); //solids in wash water - should be zero

		// If there are solids in the wash water, add it to the filter cake and also add sufficent wash water to give required cake moisture.
		double WSolMoisture = 0.0;
    if (QmWSolIn > 1.0e-6)  
			{
			WSolMoisture = QmWSolIn * RqdCakeMoist/(1.0 - RqdCakeMoist);
			if (QmWIn <= WSolMoisture)
				WSolMoisture = QmWIn;//todo error message
			QmWIn = Max(0.0, QmWIn - WSolMoisture);
			}
    
    if (QmWIn > 1.0e-6)
			{
			WR = QmWIn / SoLiq;

			const long temp = Stage + 1;
			CDVector d(temp);
			CDVector f(temp);
      f[0] = 1.0;
			for (long i=1; i<temp; i++)
				{
				f[i] = WashLossCurve.Yx(WR * i);
				d[i] = f[i-1] - f[i];
        }
      #ifndef _RELEASE
			// Calculate the concentration of the selected specie in the filtrate in the formed cake (c0)
			double mi_Specie = QFd.VMass[iTrackSpecie];
			double Cin_Specie = mi_Specie / QmLIn * 1000.0;	// Mass concentration g/kg in feed to Belt filter
      #else
      double Cin_Specie = 1.0;
      #endif
				
			// Call the function to determine the fraction of filtrate remaining in the filter cake due to inefficient washing.
			Conc(d, temp, Cin_Specie);

			// Calculate the amount of wash water displacing the filtrate in the washed cake and add the liquids to the cake and the washings
			double WashwatertoCake = Min((1 - FiltrateFracinWashedCake) * SoLiq, SoLiq);
			WashwatertoCake = Range(0.001, WashwatertoCake, QmWIn);

			Qs.QAddM(QFd, som_Liq, SoLiq*FiltrateFracinWashedCake);
			Qs.QAddM(QWs, som_Liq, WashwatertoCake);

			Qwash.QSetM(QFd, som_Liq, SoLiq - SoLiq*FiltrateFracinWashedCake);
			Qwash.QAddM(QWs, som_Liq, QmWIn - WashwatertoCake);

			//Add any solids and required moisture from wash water stream.
			Qs.QAddF(QWs, som_Sol, 1.0);	
			Qs.QAddM(QWs, som_Liq, WSolMoisture);

      if (iWashings<0)
        {//washing stream is NOT connected
        Qf.QAddF(Qwash, som_ALL, 1.0);
        }
			}
    }
	else
		Qs.QAddM(QFd, som_Liq, SoLiq);
  
  if (iWashings>=0)
    {//washing stream is connected
    SpConduit & QWashOut = *IOConduit(iWashings);
    QWashOut.QCopy(Qwash);
    }

  //results...
  Qs.SanityCheck();
  Qf.SanityCheck();
  ActCakeSolids    = Qs.MassFrac(som_Sol);
  ActFiltSolids    = Qf.MassFrac(som_Sol);
  ActCakeSolConc   = Qs.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  ActFiltSolConc   = Qf.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

	const double MErr = fabs(1.0 - ActCakeSolids - RqdCakeMoist);
	SetCI(1, bTrackStatus && HasFlw && MErr > 1.0e-6);
	SetCI(2, bTrackStatus && HasFlw && WaterError);
  }

//--------------------------------------------------------------------------

dword BeltFilt::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  }

//--------------------------------------------------------------------------

flag BeltFilt::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tCake Moisture requirements not met."; return 1;
    case  2: pS="W\tDo not expect solute in wash water."; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
// =========================================================================
