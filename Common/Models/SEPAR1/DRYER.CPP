//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#define  __DRYER_CPP
#include "dryer.h"

//===========================================================================
//
//                                 Dryer
//
//===========================================================================
/*#D:#T:Evaporative Dryer                        
#X:#h<General Description>#nThe dryer is used to remove the water from the solids 
in the feed stream.  The model will use the user defined moisture remaining in the 
cake to calculate the amount of water to be evaporated off.  Any liquid impurities 
will remain in the cake moisture.  The unit also allows for some solids loss with 
the evaporated moisture.
#nThe model assumes that the cake has a minimum of at least 0.1% moisture.

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed          Input          1              20#n
Moisture      Output         1              4#n
Cake          Output         1              4#n
#a

#n#h<Variables to be supplied by the user>
#n#i<% RqdSolidsLoss :>The required % solids lost in the moisture stream from 
the dryer.
#n#i<RqdCakeMoist :>The required % moisture in the dryer cake.  This must be 
a number between 0.1% and 99%.
#n#i<ReqdTemp :>The required exit temperature of the dryer cake and moisture.

#n#i<Datum required :>The relative elevation of the dryer with respect to the 
common datum of the system.  This is only required in Dynamic mode.#n

#n#h<Variables calculated by the model>
#n#i<Datum :>The height of the dryer above the system datum.  Dynamic mode only.
#n#i<CakeMoist :>The calculated fraction of moisture in the dryer cake.#n

#n#h<Other>#n
Default model prefix:DRY#n
Short name:Dryer#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

static MInitialiseTest InitTest(&DryerClass);
SPECIEBLK_L(InitTest, Water, "H2O(l)", true);
SPECIEBLK_V(InitTest, Steam, "H2O(g)", true);

const byte ioidFd   = 0;
const byte ioidLiq  = 2;
const byte ioidSol  = 3;

static IOAreaRec DryerIOAreaList[] =
  {
    {"Dryer Feed",          "Feed"      , ioidFd,   LIO_In0 ,    nc_MLnk, 1, 20, 0,          1.0f},
    {"Evaporated Moisture", "Moisture"  , ioidLiq,  LIO_Out0,    nc_MLnk, 1, 1,  IOSSDenied, (float)0.4},
    {"Dried Solids",        "Cake"      , ioidSol,  LIO_Out1,    nc_MLnk, 1, 1,  IOSSDenied, 0.0f},
    SPILL2AREA,
    VENT2AREA,
    {NULL}
  };

/*#G:This provides the default drawing of the Dryer.*/
static double Drw_Dryer[] = { DD_Poly, -15,6, 15,6, 15,-6, -15,-6, -15,6,
															-12,6, -12,9, -10,9, -10,-9, -12,-9, -12,6,
															10,6, 10,9, 12,9, 12,-9, 10,-9, 10,6, DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Dryer, "Dryer", "", Drw_Dryer, "Tank", "DRY", TOC_PROBAL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Dryer",
                    "Solids Dryer") 

//BEGIN_TAGOBJSELECTABLE(Dryer)              
//virtual bool Selectable() { return Water.Exists() && Steam.Exists(); }
//END_TAGOBJSELECTABLE()              

Dryer::Dryer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(DryerIOAreaList);
  Contents.SetClosed(False);
  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*//*SM_Buffered|*/HM_All);

  fOn           = True;
  RqdFiltSolids = 0.00001;
  RqdCakeMoist  = 0.02;
  FiltSol       = 0.001;
  CakeLiq       = 0.1;
  TReqd         = 333.16;
  CakeMoist     = 0.0;
  m_EnergyRqd   = 0.0;
  dWaterEvaporated = 0.0;
  };

//--------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Dryer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    (" ");
  DDB.Double  ("Density",    "Rho",             DC_Rho,  "kg/m^3", xidRho,         this, 0);
  DDB.Double  ("Level",      "Lvl",             DC_Frac, "%",      xidLevel,       this, 0);
  DDB.Double  ("Mass_Accum", "QmAcc",           DC_Qm,   "kg/s",   &m_QmAcc,       this, 0);
  DDB.Double  ("Vol_Accum",  "QvAcc",           DC_Qv,   "L/s",    &m_QvAcc,       this, 0);
  DDB.Visibility();
  
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.CheckBox("On",         "",                DC_,     "",       &fOn,           this, isParm);
  DDB.Double  ("",           "ReqdTemp",        DC_T,    "C",      &TReqd,         this, isParm);
  DDB.Double  ("",           "RqdSolidsLoss",   DC_Frac, "%",      &RqdFiltSolids, this, isParm);
  DDB.Double  ("",           "RqdCakeMoist",    DC_Frac, "%",      &RqdCakeMoist,  this, isParm);
  RB.Add_OnOff(DDB);
  DDB.Text    ("");
  DDB.Text    ("Results");
  DDB.Double  ("",           "CakeMoist",       DC_Frac, "%",      &CakeMoist,     this, isResult);
  DDB.Double  ("",           "WaterEvaporated", DC_Qm,   "kg/s",   &dWaterEvaporated, this, isResult);
  DDB.Double  ("",           "EnergyRqd",       DC_Pwr,  "kW",     &m_EnergyRqd,   this, isResult);

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  RB.BuildDataDefn(DDB);

  if (SolveSurgeMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void Dryer::EvalJoinPressures(long JoinMask)
  {
  if (NoFlwIOs()>0)
    {
    if (NetMethod()==NM_Probal || SolveMethod()!=SM_Buffered)
      {
      for (int j=0; j<NoProcessJoins(); j++)
        {
        double P = PBPress();//AtmosPress(IODatum_Term(j));
        SetJoinPressure(j, P);
        }
      }
    else
      {
      MN_Surge::EvalJoinPressures(JoinMask);
      }
    }
  }
 
//--------------------------------------------------------------------------
/*#F:
This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Dryer::EvalProducts(CNodeEvalIndex & NEI)
  {
  StkSpConduit Sd("Sd", chLINEID(), this);

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      {
      const int iCl = IOWithId_Self(ioidLiq);
      const int iCs = IOWithId_Self(ioidSol);
      SpConduit & Cl = *IOConduit(iCl);
      SpConduit & Cs = *IOConduit(iCs);

      double MSol, MLiq, MSol2Filt, MLiq2Cake, MLiq2Filt; 
      SigmaQInPMin(Sd(), som_ALL, Id_2_Mask(ioidFd));

      flag HasFlwIn = (Sd().QMass()>UsableMass);
      if (fOn && HasFlwIn)
        {
        RB.EvalProducts(Sd());
  		  
        m_VLE.SetHfInAtZero(Sd());

        MSol = Sd().QMass(som_Sol);
        MLiq = Sd().QMass(som_Liq);

			  if ((MLiq > 1e-9) && (MSol > 1e-9))
				  {
				  RqdFiltSolids = Range(0.0, RqdFiltSolids, 0.2);
				  RqdCakeMoist  = Range(0.001, RqdCakeMoist, 0.99);

				  MSol2Filt = MSol * RqdFiltSolids;
				  MLiq2Cake = (RqdCakeMoist * MSol)/GTZ(1.0 - RqdCakeMoist);
				  //MLiq2Cake = (RqdCakeMoist * (MSol - MSol2Filt))/GTZ(1.0 - RqdCakeMoist);
				  MLiq2Filt = MLiq - MLiq2Cake;

				  Cs.QSetM(Sd(), som_Liq, MLiq2Cake, IOP_Self(iCs));
				  Cl.QSetM(Sd(), som_Sol, MSol2Filt, IOP_Self(iCl));

				  /*Calculate water in cake, then evaporate the excess water, 
				    leaving behind the liquid impurities in the remaining liquid.*/
				  const int water    = Water();
				  const int watervap = Steam();
				  double MassWater, PercWater, WaterInCake, Water2Filt;
				  MassWater   = Sd().VMass[water];
				  PercWater   = MassWater/MLiq;
				  //PercWater   = (MassWater-MLiq2Filt)/MLiq2Cake;
				  WaterInCake = PercWater * MLiq2Cake;
				  Water2Filt  = MassWater - WaterInCake;
          if (Water2Filt<0.0)
            {
            Water2Filt = 0.0;
  				  Cs.SetVMass(water, Sd(), MassWater);
            //SetCI();
            }
				  Cl.SetVMass(watervap, Sd(), Water2Filt);
          dWaterEvaporated = Water2Filt;
				  for (int i=0; i < SDB.Count(); i++)
					  if ((SDB[i].IsLiq()) && (i != water))
						  {
						  Cl.SetVMass(i, Sd(), 0.0);
						  Cs.SetVMass(i, Sd(), Sd().VMass[i]);
						  }

				  Cs.QAddM(Sd(), som_Sol, MSol - MSol2Filt);
				  Cl.QAddM(Sd(), som_Gas, Sd().QMass(som_Gas)); //put any extra vapours to moisture evap vent
          }
			  else
				  {
          dWaterEvaporated = 0.0;
				  Cs.QCopy(Sd());
				  Cl.QZero();
				  }

        Cs.SetTemp(TReqd);
        Cl.SetTemp(TReqd);

        m_VLE.AddHfOutAtZero(Cl);
        m_VLE.AddHfOutAtZero(Cs);

        Cs.SanityCheck();
        Cl.SanityCheck();
        }
			else
				{
        dWaterEvaporated = 0.0;
				Cs.QCopy(Sd());
				Cl.QZero();
				}

      //TODO Calculate energy required to evap water at required temp
      m_EnergyRqd = Cs.totHf()+Cl.totHf()-Sd().totHf();

      
      CakeMoist = Cs.QMass(som_Liq)/GTZ(Cs.QMass(som_ALL));

      break;
      }
    case SM_Buffered:
      {
      Contents.ZeroDeriv();
      RB.EvalProducts(Sd());

      double SolMass = Contents.Mass(som_Sol);
      double LiqMass = Contents.Mass(som_Liq);

      RqdFiltSolids  = Range(0.0, RqdFiltSolids, 1.0);
      RqdCakeMoist   = Range(0.0, RqdCakeMoist, 1.0);
      double CakeSol, LiqSol;
      LiqSol  = RqdFiltSolids / GTZ(1.0 - RqdFiltSolids);
      CakeSol = (1.0 - RqdCakeMoist) / GTZ(RqdCakeMoist);


      if (SolMass>1.0e-12 && LiqMass>1.0e-12)
        {
        SetProdMakeup(PMU_IOId | PMU_SLRatio, ioidLiq, Contents, LiqSol);
        SetProdMakeup(PMU_IOId | PMU_SLRatio, ioidSol, Contents, CakeSol);
        }
      else
        {
        // Just Operate as a Tank
        }

      SigmaQInPMin(m_QFeed, som_ALL, First64IOIds);
      EvalProducts_SurgeLevel(SolveInlineMethod(), false, ContentHgtOrd/*, &m_QFeed*/);

      //if (NoProcessJoins()>=1)
      //  Xfer_EvalProducts(0, Joins[0].Pressure(), &QSigmaOut, NULL, NULL, NULL);

      FiltSol = IOConduit(IOWithId_Self(ioidLiq))->MassFrac(som_Sol);
      CakeLiq = IOConduit(IOWithId_Self(ioidSol))->MassFrac(som_Liq);

      break;
      }
    }
  }

//--------------------------------------------------------------------------

void Dryer::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (RB.Enabled())
      RB.GetClosureInfo(CI);

    CI.m_HfGainAtZero+=m_VLE.HfGainAtZero();
	  CI.m_PowerIn+=m_EnergyRqd; 
    }
  };

//--------------------------------------------------------------------------
// ===========================================================================*/



