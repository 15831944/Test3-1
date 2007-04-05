//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __CNTRFGMB_CPP
#include "cntrfgmb.h"

//===========================================================================
/*#D:#T:Centrifugal Filter
#X:#h<General Description>#nThe Centrifugal Filter is used to seperate the 
solids from the liquids in the feed stream.  There is the facility to 
wash the solids using two sets of wash water.  This water will displace a 
percentage of the liquid in the Centrifuge cake. 
#nIn ProBal mode the liquor used to wash the cake can be removed separately 
from the filtrate via the Washings connections.  You do not have to have 
links to the wash water or the washings connections.  However, you cannot 
have a link to the First Washings connection if you do not have a link to 
the wash water 1 connection (but not visa versa), and similarly for the 2nd 
washings connections.  Obviously, you can not have connections to the second 
wash links without connecting to first wash facility.
#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed            Input          1              20#n
WashWater1      Input          0              1#n
WashWater2      Input          0              1#n
Solids         Output          1              4#n
Filtrate       Output          1              4#n
Wash1          Output          0              4#n
Wash2          Output          0              4#n
#a#n

#n#h<Variables to be supplied by the user>#n
#n#i<% RqdFiltSolids:>The required % solids lost to the filtrate and wash waters from 
the Centrifugal Filter.
#n#i<RqdCakeMoist:>The required % moisture in the Centrifuge cake.  If wash water 
has been added to the Centrifugal Filter, the moisture will consist mainly of wash 
water.
#n#i<RqdWashEff:>This variable is only required in mass balancing mode.  The required 
displacement efficiency of the wash water, given as a number between 0 and 1.  
1 represents 100% efficiency, in which case all of the liquid remaining in the cake and 
the liquid removed in the Washings stream will consist of wash water.  A value of 0.9 
means that 10% of the liquid in the cake and the washings stream will consist of filtrate.
#n#i<Datum required:>The relative elevation of the Centrifugal Filter with 
respect to the common datum of the system.  Only required in Dynamic mode.

#n#n#h<Variables calculated by the model>#n
#n#i<Datum:>The height of the Centrifugal Filter above the system datum.  Dynamic only.
#n#i<CakeMoist:>The % moisture in the Centrifuge cake.
#n#i<FiltSolids:>The % solids in the filtrate.

#n#n#h<Other>#n
Default model prefix:CF#n
Short name:CntrfgMB#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte ioidFd   = 0;
const byte ioidLiq  = 1;
const byte ioidSol  = 2;
const byte ioidwash = 3;
const byte ioidFW   = 4;
const byte ioidwsh2 = 5;
const byte ioidSW   = 6;

static IOAreaRec CentrifugeMBIOAreaList[] =
  {{"Centrifuge Feed",   "Feed"       , ioidFd,   LIO_In0 ,    nc_MLnk, 1, 20, 0, 1.0f},
   {"Filtrate",          "Filtrate"   , ioidLiq,  LIO_Out0,    nc_MLnk, 1, 1,  0, (float)0.4},
   {"Solids",            "Cake"       , ioidSol,  LIO_Out1,    nc_MLnk, 1, 1,  0, 0.0f},
   {"1st Wash Water",    "WashWater1" , ioidwash, LIO_In1,     nc_MLnk, 0, 1,  0, 1.0f},
   {"First Washings",    "Wash1"      , ioidFW,   LIO_Out ,    nc_MLnk, 0, 1,  0, 1.0f},
   {"2nd Wash Water",    "WashWater2" , ioidwsh2, LIO_In2,     nc_MLnk, 0, 1,  0, 1.0f},
   {"Second Washings",   "Wash2"      , ioidSW,   LIO_Out ,    nc_MLnk, 0, 1,  0, 1.0f},
   SPILL2AREA,
   VENT2AREA,
   {NULL}};

static double Drw_CentrifugeMB[] = { DD_Poly, -10,-5, 0,-5, 10,-2, 10,2, 0,5, -10,5, -10,-5,
                            DD_Poly, 0,-5, 0,5,
                            DD_End }; 


//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CentrifugeMB, "CntrfgMB", "", Drw_CentrifugeMB, "Filter", "CF", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Centrifugal Filter", 
                    "The Centrifugal Filter allows the user to separate"
                    "the solids and liquids in the centrifuge and then to wash"
                    "the cake with one or two washes") 

CentrifugeMB::CentrifugeMB(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  QFd("QFd", this, TOA_Embedded)
  {
  AttachIOAreas(CentrifugeMBIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);
  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*//*SM_Buffered|*/HM_All);

  FlowRatio     = 0.0;
  RqdSolidsToFiltrate = 0.0001;
  RqdCakeMoist  = 0.02;
  WashEff       = 0.9;
  Disch         = 0;
  ActFiltSolids = 0.0;
  ActCakeLiq    = RqdCakeMoist;
  iWashEffSpecie= 0;
  dSpWashEff    = 0.0;
  dSpWashEff2   = 0.0;
  ActFiltSolConc=ActFiltSolConcT=0.0;
  ActCakeSolids=ActFiltSolids=0.0;
  }

//--------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void CentrifugeMB::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    (" ");

  DDB.Double  ("Density",    "Rho",           DC_Rho,  "kg/m^3", xidRho,         this, isResult|0);
  DDB.Double  ("Level",      "Lvl",           DC_Frac, "%",      xidLevel,       this, isResult|0);
  DDB.Double  ("Mass_Accum", "QmAcc",         DC_Qm,   "kg/s",   &m_QmAcc,       this, isResult|0);
  DDB.Double  ("Vol_Accum",  "QvAcc",         DC_Qv,   "L/s",    &m_QvAcc,       this, isResult|0);
  DDB.Visibility();

  DDB.Text    ("");
  DDB.Text    ("Requirements");
  if (PrjFileVerNo()<49)
    DDB.Double  ("",           "SolidLoss",     DC_Frac, "%",      &RqdSolidsToFiltrate, this, isParm);//SolidLoss
  else
    DDB.Double  ("Rqd_Solids_To_Filtrate",  "RqdSolToFilt",    DC_Frac, "%",      &RqdSolidsToFiltrate,this, isParm);//SolidLoss
  DDB.Double  ("Rqd_Cake_Moisture",       "RqdCakeMoist",    DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
  DDB.Double  ("Rqd_Wash_Efficiency",     "RqdWashEff",      DC_Frac, "%",      &WashEff,            this, isParm);
  DDB.Text    ("Results");
  //DDB.Double  ("Wash_Efficiency",         "WashEff",         DC_Frac, "%",      &ActWashEff,         this, isResult);//|noFile|noSnap);
  DDB.Double  ("Cake_Moisture",           "CakeMoist",       DC_Frac, "%",      &ActCakeLiq,         this, isResult);//|noFile|noSnap);
  DDB.Double  ("Cake_Solids",             "CakeSolids",      DC_Frac, "%",      &ActCakeSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("Filtrate_Solids",         "FiltSolids",      DC_Frac, "%",      &ActFiltSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("FiltrateSolidsConc@25",   "FiltSolConc25",   DC_Conc, "g/L",    &ActFiltSolConc,     this, isResult);//|noFile|noSnap);
  DDB.Double  ("FiltrateSolidsConc@T",    "FiltSolConc@T",   DC_Conc, "g/L",    &ActFiltSolConcT,    this, isResult);//|noFile|noSnap|InitHidden);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  //DDB.Byte    ("",                        "ScanEffSpecie",   DC_,     "",       &iScanEffSpecie,     this, isParm, SDB.DDBLiqSpList());
  //DDB.Double  ("Scandrett_Wash_Eff",      "ScandrettEff",    DC_Frac, "%",      &ActScandrettEff,    this, isResult);//|noFile|noSnap);
  DDB.Byte    ("",                        "WashComp",        DC_,     "",       &iWashEffSpecie,     this, isParm, SDB.DDBLiqSpList());
  DDB.Double  ("",                        "CompWashEff1",    DC_Frac, "%",      &dSpWashEff,         this, isResult);//|noFile|noSnap);
  DDB.Double  ("",                        "CompWashEff2",    DC_Frac, "%",      &dSpWashEff2,        this, isResult);//|noFile|noSnap);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Bool    ("",           "CakeDisch?",    DC_     , "",      &Disch,         this, isParm, DDBYesNo);
  DDB.Visibility();

  DDB.Text    (" ");
  RB.Add_OnOff(DDB);
  m_EHX.Add_OnOff(DDB);

  DDB.Text    (" ");
  BuildDataDefnShowIOs(DDB);

  RB.BuildDataDefn(DDB);
  m_EHX.BuildDataDefn(DDB);

  if (SolveSurgeMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

void CentrifugeMB::EvalJoinPressures(long JoinMask)
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
/*This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, 
out of each outlet of the surge unit.
*/
void CentrifugeMB::EvalProducts(CNodeEvalIndex & NEI)
  {
  StkSpConduit Sd("Sd", chLINEID(), this);
  StkSpConduit Wd("Wd", chLINEID(), this);

  SpConduit Dummy1;
  SpConduit Dummy2;

  switch (SolveMethod())
    {
    case SM_Direct:
      {
      const int iCl = IOWithId_Self(ioidLiq);
      const int iCs = IOWithId_Self(ioidSol);
      SpConduit & Cl = *IOConduit(iCl);
      SpConduit & Cs = *IOConduit(iCs);

      double wash1, wash2, MSol, MLiq, MSol2Filt, MLiq2Cake,
             RSol, TotLiq, Sol2Filt; 
      const int iWw = IOWithId_Self(ioidwash);
      const int iW2 = IOWithId_Self(ioidwsh2);
      const int iFW = IOWithId_Self(ioidFW);
      const int iSW = IOWithId_Self(ioidSW);

      SpConduit & QWw = (iWw >= 1 ? *IOConduit(iWw) : Dummy1);
      SpConduit & QW2 = (iW2 >= 1 ? *IOConduit(iW2) : Dummy2);

      //RqdSolidsToFiltrate = Range(1.0e-8, RqdSolidsToFiltrate, 0.3);
      RqdSolidsToFiltrate = Range(0.0, RqdSolidsToFiltrate, 1.0);
      RqdCakeMoist  = Range(0.0, RqdCakeMoist,  0.99);
      WashEff       = Range(0.01, WashEff, 1.0);

      SigmaQInPMin(QFd, som_ALL, Id_2_Mask(ioidFd));
      //double CFeed = QFd.SpecieConc(QFd.Temp(), iScanEffSpecie, som_Liq);
      double CFeed = QFd.SpecieConc(QFd.Temp(), iWashEffSpecie, som_Liq);
      double CWash1 = QWw.SpecieConc(QWw.Temp(), iWashEffSpecie, som_Liq);
      double CWash2 = QW2.SpecieConc(QW2.Temp(), iWashEffSpecie, som_Liq);

      RB.EvalProducts(QFd);
      m_EHX.EvalProducts(QFd);

		  MSol    = QFd.QMass(som_Sol);
      MLiq    = QFd.QMass(som_Liq);
      wash1   = QWw.QMass(som_Liq);
      wash2   = QW2.QMass(som_Liq);
      
      //will not achieve requirements if there are any solids in wash!!!
      MSol2Filt = MSol * RqdSolidsToFiltrate;
      RSol      = MSol - MSol2Filt;
      MLiq2Cake = (RqdCakeMoist * RSol)/(1 - RqdCakeMoist); 
      TotLiq    = max(MLiq + wash1 + wash2 - MLiq2Cake, 1e-6);
      Sol2Filt  = GEZ(MLiq - MLiq2Cake) * MSol2Filt/TotLiq;

      Cs.QSetM(QFd, som_Sol, RSol, IOP_Self(iCs));
      Cs.QAddM(QWw, som_Sol, QWw.QMass(som_Sol));
      Cs.QAddM(QW2, som_Sol, QW2.QMass(som_Sol));

      Cl.QSetM(QFd, som_Liq, GEZ(MLiq - MLiq2Cake), IOP_Self(iCl));
      Cl.QAddM(QFd, som_Sol, Sol2Filt);

      //=======================================================
      double CCake1 = 0.0;
      if ((MSol > 1e-6) && (MLiq > 1e-6))
        {
        double Cf, Cw1, Cw2, e1, e2, w1, w2;

        w1 = min(MLiq2Cake*WashEff, wash1);
        w2 = min(MLiq2Cake*WashEff, wash2);
        e1 = w1/max(MLiq2Cake,1e-6);
        e2 = w2/max(MLiq2Cake,1e-6);

        // The liquid composition of the solids.
        Cf  = MLiq2Cake * (1.0 - e1 - e2 + e1*e2);
        Cw1 = w1 - w2*e1;
        Cw2 = w2;

        Cs.QAddM(QFd, som_Liq, Cf);
        Cs.QAddM(QWw, som_Liq, Cw1);
        CCake1 = Cs.SpecieConc(Cs.Temp(), iWashEffSpecie, som_Liq);
        Cs.QAddM(QW2, som_Liq, Cw2);

        // The filtrate and washing streams
        double Ff, Fw1, Fw2;
        Ff  = MLiq2Cake - Cf;
        Fw1 = wash1 - Cw1;
        Fw2 = wash2 - Cw2;

        if (iFW >= 1)
          {
          rSpConduit FW =*IOConduit(iFW);
          
          if (iSW >= 1)
            {//more than one washing output stream connected
            double Sw1, Sw2;
            rSpConduit SW =*IOConduit(iSW);
            Sw1 = wash1 * MSol2Filt/TotLiq;
            Sw2 = wash2 * MSol2Filt/TotLiq;

            FW.QSetM(QFd, som_Liq, w1, IOP_Self(iFW));
            FW.QAddM(QWw, som_Liq, wash1 - w1);
            FW.QAddM(QFd, som_Sol, Sw1);

            SW.QSetM(QFd, som_Liq, w2 - w1*e2, IOP_Self(iSW));
            SW.QAddM(QWw, som_Liq, w1*e2);
            SW.QAddM(QW2, som_Liq, Fw2);
            SW.QAddM(QFd, som_Sol, Sw2);
            }
          else  // everything goes to the first washings stream
            {
            FW.QSetM(QFd, som_Liq, Ff, IOP_Self(iFW));
            FW.QAddM(QWw, som_Liq, Fw1);
            FW.QAddM(QW2, som_Liq, Fw2);
            FW.QAddM(QFd, som_Sol, MSol2Filt - Sol2Filt);
            }
          }
        else  // everything goes to the filtrate
          {
          Cl.QAddM(QFd, som_Liq, Ff);
          Cl.QAddM(QWw, som_Liq, Fw1);
          Cl.QAddM(QW2, som_Liq, Fw2);
          Cl.QAddM(QFd, som_Sol, MSol2Filt - Sol2Filt);
          }
        }
      else
        {
        Cs.QAddM(QFd, som_Sol, MSol2Filt);
        Cl.QAddM(QFd, som_Liq, MLiq2Cake);
        Cl.QAddM(QWw, som_Liq, wash1);
        Cl.QAddM(QW2, som_Liq, wash2);
        }

      ActCakeLiq      = Cs.MassFrac(som_Liq);
      ActCakeSolids   = Cs.MassFrac(som_Sol);
      ActFiltSolids   = Cl.MassFrac(som_Sol);
      ActFiltSolConc  = Cl.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActFiltSolConcT = Cl.PhaseConc(Cl.Temp(), som_Sol, som_ALL);
      //double Cuf = Cs.SpecieConc(Cs.Temp(), iScanEffSpecie, som_Liq);
      //double Cof = Cl.SpecieConc(Cl.Temp(), iScanEffSpecie, som_Liq);
      //ActScandrettEff = (CFeed - Cuf)/NZ(CFeed - Cof);
      double CCake2 = Cs.SpecieConc(Cs.Temp(), iWashEffSpecie, som_Liq);
      dSpWashEff  = (CFeed - CCake1)/NZ(CFeed - CWash1);
      dSpWashEff2 = (CFeed - CCake2)/NZ(CFeed - CWash2);
      break;
      }
    case SM_Inline:
    case SM_Buffered:
      {
      Contents.ZeroDeriv();
      //double CFeed = Sd().SpecieConc(Sd().Temp(), iScanEffSpecie, som_Liq);
      //double CFeed = Sd().SpecieConc(Sd().Temp(), iWashEffSpecie, som_Liq);
      //double CWash1 = QWw.SpecieConc(QWw.Temp(), iWashEffSpecie, som_Liq);
      //double CWash2 = QW2.SpecieConc(QW2.Temp(), iWashEffSpecie, som_Liq);
      RB.EvalProducts(Sd());
      m_EHX.EvalProducts(Sd());

      double SolMass = Contents.Mass(som_Sol);
      double LiqMass = Contents.Mass(som_Liq);

      //RqdSolidsToFiltrate = Range(1.0e-8, RqdSolidsToFiltrate, 0.3);
      RqdSolidsToFiltrate  = Range(0.0, RqdSolidsToFiltrate, 1.0);
      RqdCakeMoist   = Range(0.0, RqdCakeMoist, 1.0);
      double CakeSol, LiqSol;
      LiqSol  = RqdSolidsToFiltrate / GTZ(1.0 - RqdSolidsToFiltrate);
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
      EvalProducts_SurgeLevel(ContentHgtOrd, &m_QFeed);

      if (NoProcessJoins()>=1)
        Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, &m_QProd, NULL, NULL, NULL);

      ActCakeLiq      = IOConduit(IOWithId_Self(ioidSol))->MassFrac(som_Liq);
      ActCakeSolids   = IOConduit(IOWithId_Self(ioidSol))->MassFrac(som_Sol);
      ActFiltSolids   = IOConduit(IOWithId_Self(ioidSol))->MassFrac(som_Sol);
      ActFiltSolConc  = IOConduit(IOWithId_Self(ioidSol))->PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActFiltSolConcT = IOConduit(IOWithId_Self(ioidSol))->PhaseConc(IOConduit(IOWithId_Self(ioidSol))->Temp(), som_Sol, som_ALL);
      //double Cuf = IOConduit(IOWithId_Self(ioidSol))->SpecieConc(IOConduit(IOWithId_Self(ioidSol))->Temp(), iScanEffSpecie, som_Liq);
      //double Cof = IOConduit(IOWithId_Self(ioidSol))->SpecieConc(IOConduit(IOWithId_Self(ioidSol))->Temp(), iScanEffSpecie, som_Liq);
      //ActScandrettEff = (CFeed - Cuf)/NZ(CFeed - Cof);

      break;
      }
    }
  }

//--------------------------------------------------------------------------
// ===========================================================================*/



