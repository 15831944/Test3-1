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
a wash water stream, containing no liquid solvents, and data on filtration losses for 
a simple filtration wash.  If the unit does not require washing, the user does not 
have to specify a wash loss curve or number of stages.  The user must still specify 
solids moisture.

#nThe model will simulate any number of counter current washing steps, from 1 upwards.  The 
wash water is assumed to be added to the final washing stage, with the filtrate from each 
stage being added as a wash to the previous stage.  The model has only two outputs, the 
final filtrate and the washed cake.
#nThe washing loss vs. wash ratio curve for simple filtration washing is used to determine 
the wash losses of each washing stage. The wash loss should be specified as a fraction, 
i.e. 0 to 1.
#nFor example, the wash loss for stage one, f1, is found from the curve using :
#nWash ratio (N) = Volume of wash water / Volume of solution left in cake
#nLosses on stage 2 (f2) are found from 2 * N, stage 3 from 3 * N, etc.

#n#n#b<Assumptions>
#n1. The wash water is solute free.
#n2. Ideal displacement washing is assumed, i.e. volumetric liquid phase hold-up 
in the cake is constant throughout the washing process.
#n3. No solids are lost to the filtrate.

#n#n#h<Variables to be supplied by the user>
#n#i<Stages?> : The user must insert the number of counter current washing stages required.
#n#i<Sol Moist> : This is the percentage of liquids to solids in the final washed cake, 
the default is 10%.
#n#i<Datum Required> : The datum of the filter relative to the common datum of the system.
#n#i<WashLoss curve> : The user is required to supply the data points for the washing losses 
for simple filtration washing.  These can be read in from a spreadsheet file saved as a comma 
delimited file (CSV), or inputted directly into the model.  The wash losses must be read in as 
a fraction, i.e. from 0 to 1.

#n#n#h<Additional information available from the model>
#n#i<WashRatio:>The calculated wash ratio, N, on the Belt filter.   N = Volume of Wash Water / Volume of Liquid in Cake.
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

static IOAreaRec BeltFiltIOAreaList[] =
  {{"Feed",                       "Feed",       ioidFd, LIO_In0 ,   nc_MLnk, 1, 10, IOGRP(0)},
   {"Wash solution",              "WashSoln",   ioidWS, LIO_In1 ,   nc_MLnk, 0, 5,  IOGRP(0)},
   {"Liquid filtrate",            "Filtrate",   ioidFt, LIO_Out0,   nc_MLnk, 1, 1,  IOGRP(0)},
   {"Solid product",              "Solids",     ioidSo, LIO_Out1,   nc_MLnk, 1, 1,  IOGRP(0)|IOSetXfer},
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
  Stage           = 1;
  WR              = 1.0;
  RqdCakeMoist    = 0.1;
  ActCakeSolids   = 0.0;
  ActFiltSolids   = 0.0;
  ActCakeSolConc  = 0.0;
  ActFiltSolConc  = 0.0;
  bTrackStatus    = true;
  }

//--------------------------------------------------------------------------

BeltFilt::~BeltFilt()
  {
  }

//--------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void BeltFilt::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text    ("");
  DDB.Text    ("Requirements");
  if (PrjFileVerNo()<49)
    {
  	DDB.Long    ("No_Of_Stages",            "Stages?",         DC_,     "",       &Stage,              this, isParm);
    DDB.Double  ("Solids_Moisture",         "SolMoist",        DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
    }
  else
    {
  	DDB.Long    ("No_Of_Stages",            "Stages",          DC_,     "",       &Stage,              this, isParm);
    DDB.Double  ("Rqd_Cake_Moisture",       "RqdCakeMoist",    DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
    }

  DDB.Text    ("Results");
	DDB.Double  ("",                        "WashRatio",       DC_,     "",       &WR,                 this, isResult|0);
  DDB.Double  ("Cake_Solids",             "CakeSolids",      DC_Frac, "%",      &ActCakeSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("Filtrate_Solids",         "FiltSolids",      DC_Frac, "%",      &ActFiltSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("CakeSolidsConc@25",       "CakeSolConc25",   DC_Conc, "g/L",    &ActCakeSolConc,     this, isResult|InitHidden);//|noFile|noSnap);
  DDB.Double  ("FiltrateSolidsConc@25",   "FiltSolConc25",   DC_Conc, "g/L",    &ActFiltSolConc,     this, isResult|InitHidden);//|noFile|noSnap);

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

void BeltFilt::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  }

//--------------------------------------------------------------------------

void BeltFilt::filt(CDMatrix& w, CDVector& d, int j, long max)
  {
  max = max + 1;
  double den = WR - d[1];
  w[j][2] = (WR * w[j][1] - d[1]) / den;
  for (int i=3; i<max; i++)
    {
    w[j][i] = (WR - d[1] + d[2]) * w[j][i-1] - d[i-1];
    for (int k=3; k<i; k++)
      w[j][i] = w[j][i] - (d[k-1] - d[k]) * w[j][i-k+1];
    w[j][i] = w[j][i] / den;
    }
  }

//--------------------------------------------------------------------------

void BeltFilt::washcon(CDMatrix& w, CDMatrix& c, double Cin, int j, long temp)
  {
  c[j][1] = 1.0 - WR * (w[j][1] - w[j][2]);
  for (int i = 2; i<temp; i++)
    c[j][i] = c[j][i-1] - WR * (w[j][i] - w[j][i+1]);
  for (i = 1; i<temp; i++)
    {
    c[j][i] = c[j][i] * Cin;
    }
  }

//--------------------------------------------------------------------------

void BeltFilt::Conc(CDVector& d, long temp, double CLiq, double FLiq)
  {
  SpConduit & Qs = *IOConduit(IOWithId_Self(ioidSo));
  SpConduit & Qf = *IOConduit(IOWithId_Self(ioidFt));

  const int water = H2O.LiqPhInx(), max = SDB.Count();
  const double Liq = QFd.QMass(som_Liq);
  const double c0 = 1.0;
  CDMatrix w(max,temp+1),
           c(max,temp);
  for (int i=0; i<max; i++)
    {
    if ((SDB[i].IsLiq()) && (i != water))
      {
      const double mi = QFd.VMass[i];
      const double Cin = mi / Liq;
      if (Cin > 1e-9)
        {
        w[i][1] = 0.9;
        filt(w, d, i, temp);
        w[i][1] = ((w[i][1] - w[i][temp])/GTZ(c0 - w[i][temp]));
        filt(w, d, i, temp);
        washcon(w, c, Cin, i, temp);
        const double loss = Max(0.0, c[i][temp-1] * CLiq);
        const double wash = QFd.VMass[i] - loss;
        Qs.SetVMass(i, QFd, loss);
        Qf.SetVMass(i, QFd, wash);
        }
      }
    }
  }
 
//--------------------------------------------------------------------------

void BeltFilt::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NJoins(); j++)
        {
        double P = PBPress();//AtmosPress(IODatum_Term(j));
        SetPBJoinPressure(j, P, true, true);
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
change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void BeltFilt::EvalProducts(long JoinMask)
  {
  Stage=Max(Stage, 0L);
  SpConduit & Qs = *IOConduit(IOWithId_Self(ioidSo));
  SpConduit & Qf = *IOConduit(IOWithId_Self(ioidFt));

  if (fDoDbgBrk)
    { int xxx=0; }

  QFd.QZero();
  SigmaQInPMin(QFd, som_ALL, Id_2_Mask(ioidFd)| Id_2_Mask(ioidWS));
  flag HasFlw = (QFd.QMass(som_ALL)>UsableMass);

  double QmSIn = QFd.QMass(som_Sol);
  double QmLIn = QFd.QMass(som_Liq);
	RqdCakeMoist = Range(0.001, RqdCakeMoist, 0.99);

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

	Qs.QSetM(QFd, som_Sol, QmSIn, Std_P);
	Qf.QSetM(QFd, som_Sol, 0.0, Std_P);

	Qs.QAddM(QFd, som_Liq, SoLiq);
	Qf.QAddM(QFd, som_Liq, FtLiq);

  const int iWw = IOWithId_Self(ioidWS);

  WR = 0.0;
  if ((iWw >= 1) && (QmSIn > 1.0e-3) && (Stage > 0) && (SoLiq > 0.0))
    {
    SpConduit *pw = IOConduit(IOWithId_Self(ioidWS));
    const double QmWIn = pw->QMass(som_Liq);

		if (QmWIn > 1.0e-6)
			{
			WR = QmWIn / SoLiq;

			const long temp = Stage + 1;
			CDVector  d(temp),
								f(temp);
			f[0] = 1.0;
			for (long i=1 ; i<temp ; i++)
				{
				f[i] = WashLossCurve.Yx(WR * i);
				d[i] = f[i-1] - f[i];
				}
			Conc(d, temp, SoLiq, FtLiq);

			// Correct the percentage liquid in cake, if necessary.
			const int water = H2O.LiqPhInx();
			const double CakeLiq = Qs.QMass(som_Liq);
  		const double CorH2O = SoLiq - CakeLiq;
			if (fabs(CorH2O)>1e-18)
				{
			  double CakeH2O = Qs.VMass[water];
			  double FiltH2O = Qf.VMass[water];
				CakeH2O = CakeH2O + CorH2O;
				FiltH2O = FiltH2O - CorH2O;
				Qs.SetVMass(water, QFd, CakeH2O);
				Qf.SetVMass(water, QFd, FiltH2O);
				}
			}
    }
  
  //results...
  Qs.SanityCheck();
  Qf.SanityCheck();
  ActCakeSolids    = Qs.MassFrac(som_Sol);
  ActFiltSolids    = Qf.MassFrac(som_Sol);
  ActCakeSolConc   = Qs.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  ActFiltSolConc   = Qf.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

	double MErr = fabs(1.0 - ActCakeSolids - RqdCakeMoist);
	SetCI(1, bTrackStatus && HasFlw && MErr > 1.0e-6);
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
    case  1: pS="W\tCake Moisture Requirements not met."; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
// =========================================================================
