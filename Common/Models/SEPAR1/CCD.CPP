//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __CCD_CPP
#include "CCD.h"

//==========================================================================
/*#D:#T:Counter Current Decantation System
#X:#h<General Description>#N#NThe Counter Current Decantation (CCD) model 
is used in Mass Balancing mode to quickly determine the concentration in 
the final over and under flow streams from a series of thickeners.  The 
user is required to specify the number of stages in the system.  The 
underflow from the model will have the same percentage solids as the feed 
to the system.  Any solids in the wash stream will report to the overflow
stream.
#nThis model is designed to be used as a quick means of determining the 
effect of differing numbers of stages or wash water ratio in a CCD circuit.  
The user may also use a number of Washers in series to model a CCD circuit.  
By using individual Washer models the user can specify mixing efficiencies, 
which is not available with this model.

#n#n The concentration, Cn, in the final thickener in a CCD circuit containing 
n stages is calculated using the folowing formula:
#n Cn = (Cl - Cw)/(1 + WR + WR^2 + .... + WR^n) + Cw
#nwhere	Cl	-	Concentration in the slurry feed to the CCD circuit
#n	Cw	-	Concentration in the wash water
#n	WR	-	Wash ratio = wash water flow / slurry liquor flow.

#n#b<Assumptions and Limitations>
#n1. No material is leached from the solids in the process.
#n2. The efficiency of slurry and wash liquor mixing is 100% in all stages.
#n3. The flowrate of solution from each thickener underflow is equal to the 
incoming slurry flowrate and the wash water flow equals the thickener overflows.
#n4. There is no allowance for evaporation or the addition of any additional 
streams.

#n#b<Reference:> P.M.Page, 'A simple equation for CCD calculations' E/MJ Operating 
Handbook of Mineral Processing, October 1976.

#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed          Input          1              10#n
Wash Water    Input          1              10#n
OverFlow      Output         1              4#n
UnderFlow     Output         1              4#n
#a

#n#h<Variables to be supplied by the user>
#n#i<Stages:>The number of stages in the CCD circuit.

#n#n#h<Variables calculated by the model>
#n#i<WashRatio:>The ratio of wash water to liquid in the CCD feed stream.
#n#i<UFSolids:>The actual % solids in the underflow from the nth stage.
#n#i<OFSolids:>The actual % solids in the overflow from the first stage.

#n#h<Other>#n
Default model prefix:CCD#n
Short name:CCD#n
Model type:Unit#n
#G:Units
*/

//==========================================================================

static MInitialiseTest InitTest(&CcdClass);
SPECIEBLK_L(InitTest, H2O,"H2O(l)",true);

const byte ioidFd = 0;
const byte ioidWW = 1;
const byte ioidOF = 2;
const byte ioidUF = 3;

static IOAreaRec CcdIOAreaList[] =
  {{"Ccd Feed",      "Feed"      , ioidFd,   LIO_In0 , nc_MLnk, 1, 10, IOPipeEntry, 1.0f},
	 {"Wash Water",    "WashWater" , ioidWW,   LIO_In1 , nc_MLnk, 1, 10, IOPipeEntry, 1.0f},
   {"Ccd Overflow",  "OverFlow"  , ioidOF,   LIO_Out0, nc_MLnk, 1, 1,  IOPipeEntry, (float)0.98},
   {"Ccd Underflow", "UnderFlow" , ioidUF,   LIO_Out1, nc_MLnk, 1, 1,  IOPipeEntry, 0.0f},
    SPILL2AREA,
    VENT2AREA(-1),
   {NULL}};

static double Drw_CCD[] = { DD_Poly, 0,-7, -16,0, -16,3, 16,3, 16,0, 0,-7, -20,-7, -20,7, 20,7, 20,-7, 0,-7, 
                            DD_TagPos, 0, -10.5,
                            DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Ccd, "CCD", "1", Drw_CCD, "WasherDecanter", "CCD", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:CCD", 
                    "CCD circuit that assumes perfect"
                    "mixing in all stages.")

//BEGIN_TAGOBJSELECTABLE(Ccd)              
//virtual bool Selectable() { return H2O.Exists(); }
//END_TAGOBJSELECTABLE()              

Ccd::Ccd(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(CcdIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);

  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*//*SM_Buffered|*/HM_All);

  Stage		 = 3;
  UFSolids = 0.0;
  OFSolids = 0.0;
  ActUFSolidsConc25 = 0.0;
  ActOFSolidsConc25 = 0.0;
	WR			 = 1.5;
  }

//--------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Ccd::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
	DDB.Text    ("");
  DDB.Double  ("Pressure",                "P",               DC_P,    "kPag",   xidPMean,            this, isResult|0);
  DDB.Double  ("Density",                 "Rho",             DC_Rho,  "kg/m^3", xidRho,              this, isResult|0);
  DDB.Double  ("Level",                   "Lvl",             DC_Frac, "%",      xidLevel,            this, isResult|0);
  DDB.Double  ("Mass_Accum",              "QmAcc",           DC_Qm,   "kg/s",   &m_QmAcc,            this, isResult|0);
  DDB.Double  ("Vol_Accum",               "QvAcc",           DC_Qv,   "L/s",    &m_QvAcc,            this, isResult|0);
  DDB.Visibility();

	DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.Long	  ("No_Of_Stages",            "Stages",			     DC_,		  "",	      &Stage,              this, isParm);
  DDB.Text    ("Results");
  DDB.Double  ("",                        "WashRatio",       DC_,		  "",	      &WR,				         this, isResult|0);
	DDB.Double  ("UnderFlow_Solids",        "UFSolids",        DC_Frac, "%",      &UFSolids,           this, isResult|0);
  DDB.Double  ("OverFlow_Solids",         "OFSolids",        DC_Frac, "%",      &OFSolids,           this, isResult|0);
  DDB.Double  ("UnderFlow_SolidsConc@25", "UFSolConc25",     DC_Conc, "g/L",    &ActUFSolidsConc25,  this, isResult|InitHidden);
  DDB.Double  ("OverFlow_SolidsConc@25",  "OFSolConc25",     DC_Conc, "g/L",    &ActOFSolidsConc25,  this, isResult|InitHidden);

  DDB.Text    (" ");
	BuildDataDefnShowIOs(DDB);

  if (SolveSurgeMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

void Ccd::ConcBal(CSysVector &Fd,  CDVector& c, CDVector& mass, double UVol, double max, int water)
	{
  SpConduit & Co = *IOConduit(IOWithId_Self(ioidOF));
  SpConduit & Cu = *IOConduit(IOWithId_Self(ioidUF));

	double loss, wash;
	// Calculate the mass in the underflow, place balance in overflow
	for (int i=0; i<max; i++)
		{
		if ((SDB[i].IsLiq()) && (i != water))
			{
			loss = Range(0.0, c[i] * UVol, mass[i]);
			wash = mass[i] - loss;
			Cu.SetVMass(i, Fd, loss);
			Co.SetVMass(i, Fd, wash);
			}
		}
	}

//--------------------------------------------------------------------------

void Ccd::ConfigureJoins()
  {
  switch (NetMethod())
    {
    case NM_Probal:
      for (int i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    default:
      MN_Surge::ConfigureJoins();
      break;
    }
  }
 
//--------------------------------------------------------------------------

void Ccd::EvalJoinPressures(long JoinMask)
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
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  }
 
//--------------------------------------------------------------------------
/*#F:
This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Ccd::EvalProducts(CNodeEvalIndex & NEI)
  {
  Stage=Max(Stage, 0L);
  StkSpConduit Fd("Fd", chLINEID(), this);
  StkSpConduit Ww("Ww", chLINEID(), this);
  StkSpConduit Mix("Mix", chLINEID(), this);

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      {
      const int iCo = IOWithId_Self(ioidOF);
      const int iCu = IOWithId_Self(ioidUF);
      SpConduit & Co = *IOConduit(iCo);
      SpConduit & Cu = *IOConduit(iCu);

      SigmaQInPMin(Fd(), som_ALL, Id_2_Mask(ioidFd));
      SigmaQInPMin(Ww(), som_ALL, Id_2_Mask(ioidWW));
      SigmaQInPMin(Mix(), som_ALL, Id_2_Mask(ioidFd)|Id_2_Mask(ioidWW));
      const double FinalT = Mix().Temp(); //assume perfect mixing for energy balance and both outlet streams have equal temperature
  
      // *** NBNBNB A Big Kludge
      SpModel *pM=Fd().Model();
      for (int q=0; q<pM->nQuals; q++)
        if (pM->Quals[q].m_pQ)
          pM->Quals[q].m_pQ->Equilibrate();
      // *** Replace with Sd().EquilibrateQualities();
  
			double Qs, Ql, Qws, Qwl;
			Qs = Fd().QMass(som_Sol);
      Ql = Fd().QMass(som_Liq);
			Qwl = Ww().QMass(som_Liq);
			Qws = Ww().QMass(som_Sol);

      Cu.QSetM(Fd(), som_Sol, Qs, Std_P);
      Co.QSetM(Ww(), som_Sol, Qws, Std_P);

			if ((Ql > 1e-6) && (Qwl > 1e-6))
				{
				WR = Qwl/Ql;
				double Denom = 1.0;
				double Smax  = Stage + 1;

				for(int j=1; j<Smax; j++)
					Denom += pow(WR,j);

				int water = H2O.LiqPhInx(), max = SDB.Count();
				double FeedLiq, WashLiq, FeedDen, WashDen, UFVol, WaterIn, Cl, Cw;   

			  FeedDen = Fd().Rho(som_Liq) ;
			  WashDen = Ww().Rho(som_Liq) ;
				FeedLiq = Ql/FeedDen;
				WashLiq = Qwl/WashDen;
				UFVol   = FeedLiq;
				WaterIn = Fd().VMass[water] + Ww().VMass[water];
				CDVector conc(max), Feedmass(max);

				for (int i=0; i<max; i++)
					{
					if ((SDB[i].IsLiq()) && (i != water))
						{
						Cl = Fd().VMass[i] / FeedLiq;
						Cw = Ww().VMass[i] / WashLiq;
						conc[i]     = (Cl - Cw)/Denom + Cw;
						Feedmass[i] = Fd().VMass[i] + Ww().VMass[i];
						}
					}

				double temp, err, templiq, UFwater;
				for (int l=0; l<20; l++)
					{
					Co.SetVMass(water, Fd(), 0.0);
					Cu.SetVMass(water, Fd(), 0.0);
					ConcBal(Fd(), conc, Feedmass, UFVol, max, water);
					// Set water to ensure UF liquid is equal to feed liquid
					templiq = Cu.QMass(som_Liq);
					UFwater = Range(0.0, Ql - templiq, Ql);
					Cu.SetVMass(water, Fd(), UFwater);
					Co.SetVMass(water, Fd(), WaterIn - UFwater);
					temp = Cu.QMass(som_Liq)/Cu.Rho(som_Liq);
					err  = fabs(UFVol - temp);
					if (err < 1e-9)
						l = 20;
					else
						UFVol = temp;
					}
        Co.SetTempPress(FinalT, Std_P);
        Cu.SetTempPress(FinalT, Std_P);
/*
        //Now do it properly to get an energy balance...
        SpMArray Filt;
        for (int i=0; i<max; i++)
          {
          if (SDB[i].IsSol())
            {
            Filt[i] =
            }
	 				else if ((SDB[i].IsLiq()) && (i != water))
            Filt[i] =
          }
  			Co.QSetF(Ww(), Filt1, 1.0);
  			Co.QAddF(Fd(), Filt2, 1.0);
  			Cu.QSetF(Fd(), Filt3, 1.0);
  			Cu.QAddF(Ww(), Filt4, 1.0);*/
				}
			else
				{
				Cu.QAddM(Fd(), som_Liq, Ql);
				Co.QAddM(Ww(), som_Liq, Qwl);
        Co.SetTempPress(FinalT, Std_P);
        Cu.SetTempPress(FinalT, Std_P);
				if (Ql > 1e-6)
					WR = Qwl / Ql;
				else
					WR = 0.0;
				}

      /*//put all vapours (if any) to vent (if present)
      int iVent = IOWithId_Self(idVent);
      if (iVent>=0)
        {
        rSpConduit Cvent = *IOConduit(iVent);
        double Qg = Fd().QMass(som_Gas);
        Cvent.QSetM(Fd(), som_Gas, Qg, Std_P);
        Qg = Ww().QMass(som_Gas);
        Cvent.QAddM(Ww(), som_Gas, Qg);
        }*/

      //results...
      UFSolids = Cu.MassFrac(som_Sol);
      OFSolids = Co.MassFrac(som_Sol);
      ActUFSolidsConc25 = Cu.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActOFSolidsConc25 = Co.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

      //double OFErr = fabs(OFSolids - RqdOFSolids);
      //SetCI(1, bTrackStatus && OFErr > 1.0e-6);
      //double UFErr = fabs(UFSolids - RqdUFSolids);
      //SetCI(2, bTrackStatus && UFErr > 1.0e-6);

      break;
      }
    //case SM_Inline:
    case SM_Buffered:
      {
      Contents.ZeroDeriv();

      double SolMass = Contents.Mass(som_Sol);
      double LiqMass = Contents.Mass(som_Liq);
      double Sol_Rho = Contents.Rho(som_Sol) ;
      double Liq_Rho = Contents.Rho(som_Liq) ;

      SigmaQInPMin(m_QFeed, som_ALL, First64IOIds);
      EvalProducts_SurgeLevel(SolveInlineMethod(), false, ContentHgtOrd/*, &m_QFeed*/);

      if (NoProcessJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, &m_QProdSrg, NULL, NULL, NULL);
      
      UFSolids = IOConduit(IOWithId_Self(ioidUF))->MassFrac(som_Sol);
      OFSolids = IOConduit(IOWithId_Self(ioidOF))->MassFrac(som_Sol);
      ActUFSolidsConc25 = IOConduit(IOWithId_Self(ioidUF))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActOFSolidsConc25 = IOConduit(IOWithId_Self(ioidOF))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

      break;
      }
    }
  };

//===========================================================================


