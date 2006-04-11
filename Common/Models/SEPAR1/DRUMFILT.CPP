//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __DRUMFILT_CPP
#include "DrumFilt.h"

//==========================================================================
/*#D:#T:DrumFilt
#X:#n#h<General Description>#NThe drum filter separates solids and liquids based 
on user defined variables.  There is also the facility to add wash water to the 
unit.
#nThe user defines the amount of liquid in the filter cake and the amount of solids 
reporting to the filtrate.  In addition, the user specifies the fractional bypass, which 
defines the fraction of the wash water which channels through the cake and reports 
directly to the filtrate.
#nObviously, if there is no wash water connected to the drum filter, then the user can 
ignore the washing variable.

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed          Input          1              20#n
Wash          Input          0              2#n
Filtrate      Output         1              4#n
Cake          Output         1              1#n
#a

#n#n#h<Variables to be supplied by the user>#n
#n#i<% CakeMoist:> The required % liquids in the filter cake.
#n#i<% FiltSolids:> The required % solids in the filtrate.
#n#i<% Bypass:> The fraction of wash water which bypass the cake and reports directly to 
the filtrate. 
#n#i<Datum required:> The relative elevation of the drum filter with respect to the 
common datum of the system.

#n#n#h<Variables calculated by the model>#n
#n#i<Datum:> The height of the washer above the system datum.
#n#i<CakeSolids:> The actual % solids in the filter cake.
#n#i<FiltrateSolids:> The actual % solids in the filtrate.

#n#n#h<Other>#n
Default model prefix:DFT#n
Short name:DrumFilt#n
Model type:Unit#n
#G:Units
*/

//==========================================================================

enum FSM_FiltrateMethods { FSM_FiltSolFrac, FSM_FiltSolConc };

const byte ioidFd = 0;
const byte ioidWs = 1;
const byte ioidFt = 2;
const byte ioidFC = 3;
const byte ioidVent = 4;

static IOAreaRec DrumFiltIOAreaList[] =
  {
    {"Filter Feed", "Feed",     ioidFd,           LIO_In0,  nc_MLnk, 1, 20, IOPipeEntry, 1.0f,  {0,0,0,0}, 0, &PipeEntryGroup},
    {"Wash Water",  "Wash",     ioidWs,           LIO_In1,  nc_MLnk, 0, 5,  IOPipeEntry, 1.0f,  {0,0,0,0}, 0, &PipeEntryGroup},
    {"Filtrate",    "Filtrate", ioidFt,           LIO_Out0, nc_MLnk, 1, 1,  IOPipeEntry, 0.98f, {0,0,0,0}, 0, &PipeEntryGroup},
    {"Filter Cake", "Cake",     ioidFC,           LIO_Out1, nc_MLnk, 1, 1,  IOPipeEntry, 0.0f,  {0,0,0,0}, 0, &NullFlwGroup},
    {"",            "Vent",     ioidVent,         LIO_Out,  nc_MLnk, 0, 1,  IOPipeEntry, 1.0f},
    SPILL2AREA("Spills", IOId_Spill2Area),
    VENT2AREA("Vents",   IOId_Vent2Area),
    {NULL}
  };

static double Drw_DrumFilt[] = { DD_Arc, 0, 0, 8, 
                      DD_Poly, -12,-2, 0,-12, 12,-4, 8,-2,
                      DD_End };
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(DrumFilt, "DrumFilt", "0", Drw_DrumFilt, "Filter", "DFT", TOC_PROBAL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Drum Filter", 
                    "Drum Filter based on user defined solid- "
                    "liquid separation.")

DrumFilt::DrumFilt(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  Qw("Qw", this)
  {
  AttachIOAreas(DrumFiltIOAreaList);
  Contents.SetClosed(false);

  iRqdFiltMethod  = FSM_FiltSolFrac;
  RqdFiltSolids   = 0.0001;
  ActFiltSolids   = RqdFiltSolids;
  FiltSolidsUsed  = RqdFiltSolids;
  RqdCakeMoist    = 0.1;
  ActCakeSolids   = 1.0 - RqdCakeMoist;
  RqdBypass       = 0.0;
  RqdFiltSolConc  = 1.0;
  ActCakeSolConc  = 0.0;
  ActFiltSolConc  = RqdFiltSolConc;
  //ActFiltSolConcT = RqdFiltSolConc;
  //iScanEffSpecie  = 0;
  //ActScandrettEff = 0.0;
  iWashEffSpecie  = 0;
  dSpWashEff      = 0.0;
  bTrackStatus    = true;
  fOn=True;
  }

//--------------------------------------------------------------------------

DrumFilt::~DrumFilt()
  {
  }

//--------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void DrumFilt::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|0);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|0);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|0);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,   "kg/s",   &QmAcc,         this, isResult|0);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,   "L/s",    &QvAcc,         this, isResult|0);
  DDB.Visibility();

  DDB.Text    ("");
  if (PrjFileVerNo()<48)
    {
    DDB.Double  ("ReqdCakeMoist",    "CakeLiq",      DC_Frac, "%",      &RqdCakeMoist,   this, isParm);
    DDB.Double  ("ReqdFiltSolids",   "FiltSolids",   DC_Frac, "%",      &RqdFiltSolids,  this, isParm);
    DDB.Double  ("Wash_Bypass",      "Bypass",       DC_Frac, "%",      &RqdBypass,      this, isParm);
    DDB.Text    ("");
    DDB.Double  ("Cake_Solids",      "CakeSolid",    DC_Frac, "%",      &ActCakeSolids,     this, isResult|0);
    DDB.Double  ("Filtrate_Solids",  "FiltSolid",    DC_Frac, "%",      &ActFiltSolids,     this, isResult|0);
    }
  else
    {
    static DDBValueLst DDB0[]={
      {FSM_FiltSolFrac, "FiltrateSolidsFraction"},
      {FSM_FiltSolConc, "FiltrateConcentration"},
      {0}};
    DDB.Text    ("Requirements");
    DDB.CheckBox("On",                      "",                DC_,     "",       &fOn,                this, isParm);
    DDB.Byte    ("Filtrate_Method",         "Method",          DC_,     "",       &iRqdFiltMethod,     this, isParm|AffectsStruct, DDB0);
    DDB.Visibility(SHM_All, (iRqdFiltMethod == FSM_FiltSolFrac));
    DDB.Double  ("Rqd_Filtrate_Solids",     "RqdFiltSolids",   DC_Frac, "%",      &RqdFiltSolids,      this, isParm);
    DDB.Visibility(SHM_All, (iRqdFiltMethod == FSM_FiltSolConc));
    DDB.Double  ("Rqd_FiltrateSolidsConc25","RqdFiltSolConc25",DC_Conc, "g/L",    &RqdFiltSolConc,     this, isParm);
    DDB.Visibility();
    DDB.Double  ("Filtrate_Solids_Used",    "FiltSolidsUsed",  DC_Frac, "%",      &FiltSolidsUsed,     this, isResult|InitHidden);//|noView);
    DDB.Double  ("Rqd_Cake_Moisture",       "RqdCakeMoist",    DC_Frac, "%",      &RqdCakeMoist,       this, isParm);
    DDB.Double  ("Wash_Bypass",             "WashBypass",      DC_Frac, "%",      &RqdBypass,	         this, isParm);
    DDB.Text    ("Results");
    DDB.Double  ("Cake_Solids",             "CakeSolids",      DC_Frac, "%",      &ActCakeSolids,      this, isResult);//|noFile|noSnap);
    DDB.Double  ("Filtrate_Solids",         "FiltSolids",      DC_Frac, "%",      &ActFiltSolids,      this, isResult);//|noFile|noSnap);
    DDB.Double  ("CakeSolidsConc@25",       "CakeSolConc25",   DC_Conc, "g/L",    &ActCakeSolConc,     this, isResult);//|noFile|noSnap);
    DDB.Double  ("FiltrateSolidsConc@25",   "FiltSolConc25",   DC_Conc, "g/L",    &ActFiltSolConc,     this, isResult);//|noFile|noSnap);
    //DDB.Double  ("FiltrateSolidsConc@T",    "FiltSolConc@T",   DC_Conc, "g/L",    &ActFiltSolConcT,    this, isResult);//|noFile|noSnap|InitHidden);
    //DDB.Byte    ("",                        "ScanEffSpecie",   DC_,     "",       &iScanEffSpecie,     this, isParm, SDB.DDBLiqSpList());
    //DDB.Double  ("Scandrett_Wash_Eff",      "ScandrettEff",    DC_Frac, "%",      &ActScandrettEff,    this, isResult);//|noFile|noSnap);
    DDB.Byte    ("",                        "WashComp",        DC_,     "",       &iWashEffSpecie,     this, isParm, SDB.DDBLiqSpList());
    DDB.Double  ("",                        "CompWashEff",     DC_Frac, "%",      &dSpWashEff,         this, isResult);//|noFile|noSnap);
    }

  DDB.Text    ("");
  //RB.Add_OnOff(DDB);
  DDB.Visibility(SM_Direct|HM_All);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  //RB.BuildDataDefn(DDB);

  if (SolveDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------
// x is required filtrate solids fraction
//
class CFiltSolFracFnd : public MRootFinderBase
  {
  public:
    SpConduit &Sd, &Fd, &Ww, &Ff, &Fc;
    double RqdFiltSolidsUsed;
  protected:
    double HTot;
    double FT;
    double Qs;
    double Ql;
    double Us;
    double QlWw;
    double QlFd;

    double POut;
    double RqdBypass;
    double RqdCakeMoist;

    static CToleranceBlock s_Tol;

  public:
    CFiltSolFracFnd(SpConduit &Sd_, SpConduit &Fd_, SpConduit &Ww_, SpConduit &Ff_, SpConduit &Fc_, 
      double POut_, double RqdBypass_, double RqdCakeMoist_);
    double Function(double x);
  };

CToleranceBlock CFiltSolFracFnd::s_Tol(TBF_Both, "DrumFilter:SolidsFrac", 0.0, 1.0e-8);

//--------------------------------------------------------------------------
    
CFiltSolFracFnd::CFiltSolFracFnd(SpConduit &Sd_, SpConduit &Fd_, SpConduit &Ww_, SpConduit &Ff_, SpConduit &Fc_, 
      double POut_, double RqdBypass_, double RqdCakeMoist_) :
  MRootFinderBase("DrumFilt", s_Tol),//1.0e-8), 
  Sd(Sd_), Fd(Fd_), Ww(Ww_), Ff(Ff_), Fc(Fc_)
  {
  POut = POut_;
  RqdBypass = RqdBypass_;
  RqdCakeMoist = RqdCakeMoist_;

  HTot = Sd.totHf();
  FT = Sd.Temp();
	Qs = Sd.QMass(som_Sol);
  Ql = Sd.QMass(som_Liq);
  Us = 1.0 - RqdCakeMoist;

  QlWw = Ww.QMass(som_Liq);
	QlFd = Fd.QMass(som_Liq);
  
  RqdFiltSolidsUsed = 0.0;
  }

//---------------------------------------------

double CFiltSolFracFnd::Function(double x) 
  {
  RqdFiltSolidsUsed = x;
  //RB.EvalProducts(Sd());
  double Os = RqdFiltSolidsUsed;
  
  //cake...
  const double Sc = Range(0.0, Us*(Os * Ql - Qs * (1.0 - Os))/LTZ(Os - Us), Qs);
  const double Lc = Range(0.0, Sc*(1.0 - Us)/GTZ(Us), Ql);
  Fc.QSetM(Sd, som_Sol, Sc, POut);

  //filtrate...
  const double Sf = Qs - Sc;
  const double Lf = Ql - Lc;
  Ff.QSetM(Sd, som_Sol, Sf, POut);

  //TODO:Calculate PreWashCake for display as separate 'QPWC' in Access window. This is feed before washing less filtrate.

  const flag OldCode = 1;
  if (OldCode)
    {
    double AWs  = QlWw * (1.0 - RqdBypass);
	  if (Lc <= AWs)
		  {
		  Fc.QAddM(Ww, som_Liq, Lc);
		  Ff.QAddM(Ww, som_Liq, QlWw - Lc);
		  Ff.QAddM(Fd, som_Liq, QlFd);
		  }
    else
	    {
		  double washsol = Lc - AWs;
		  Fc.QAddM(Ww, som_Liq, AWs);
		  Fc.QAddM(Fd, som_Liq, washsol);
		  Ff.QAddM(Ww, som_Liq, QlWw - AWs);
		  Ff.QAddM(Fd, som_Liq, QlFd - washsol);
		  }
    }
  else
    {
    //this is not correct for drum-filter, with code below it then acts as the washer model.
    //See model documentation for more info...
    double Splt2Filt=(Lf-RqdBypass*QlWw)/GTZ(Lf+Lc-RqdBypass*QlWw);

    const double Ww2Cake=(1.0-Splt2Filt)*(1.0-RqdBypass)*QlWw;
    const double Ww2Filt=(Splt2Filt)*(1.0-RqdBypass)*QlWw + RqdBypass*QlWw;
    const double Fd2Cake=(1.0-Splt2Filt)*QlFd;
    const double Fd2Filt=(Splt2Filt)*QlFd;

    Fc.QAddM(Ww, som_Liq, Ww2Cake);
    Fc.QAddM(Fd, som_Liq, Fd2Cake);
    Ff.QAddM(Ww, som_Liq, Ww2Filt);
    Ff.QAddM(Fd, som_Liq, Fd2Filt);
    }

  Fc.SanityCheck();
  Ff.SanityCheck();

  if (1)
    {// Correct Enthalpy...
    Fc.SetTemp(FT);
    Ff.SetTemp(FT);
    double P = POut;
    double H = Fc.totHf()+Ff.totHf();
    double dT = 0.0, H0, T0;
    for (int Hiter=0; Hiter<10; Hiter++)
      {
      double Err;
      if (ConvergedVV(HTot, H, 1.0e-12, 1.0e-12, Err))
        break;
      if (dT!=0.0)
        dT = (HTot-H)*(FT-T0)/NZ(H-H0);
      else
        dT = 0.1;
      T0 = FT;
      H0 = H;
      FT += dT;
      H = Fc.totHf(som_ALL, FT, P)+Ff.totHf(som_ALL, FT, P);
      }
    Fc.SetTemp(FT);
    Ff.SetTemp(FT);
    }

  double Conc25 = Ff.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
  return Conc25;
  }

//--------------------------------------------------------------------------

void DrumFilt::EvalJoinPressures(long JoinMask)
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
      MN_Surge::EvalJoinPressures(JoinMask);
      break;
    }
  }
 
//--------------------------------------------------------------------------

void DrumFilt::EvalProducts(long JoinMask)
  {
  StkSpConduit Sd("Sd", chLINEID(), this);
  StkSpConduit Fd("Fd", chLINEID(), this);
  StkSpConduit Ww("Ww", chLINEID(), this);

  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
    case SM_Buffered:
      {
  	  RqdBypass      = Range(0.0, RqdBypass, 1.0);
	    RqdFiltSolids  = Range(0.0, RqdFiltSolids, 1.0);
      RqdCakeMoist   = Range(0.0, RqdCakeMoist,  0.99);
      RqdFiltSolConc = Range(0.0, RqdFiltSolConc, 5000.0);

      const int iFiltrate = IOWithId_Self(ioidFt);
      const int iCake = IOWithId_Self(ioidFC);
      SpConduit & Ff = *IOConduit(iFiltrate);
      SpConduit & Fc = *IOConduit(iCake);
      const int iVent = IOWithId_Self(ioidVent);
      SpConduit * pVent = (iVent>=0 ? IOConduit(iVent) : NULL);

      SigmaQInPMin(Sd(), som_ALL, Id_2_Mask(ioidFd)|Id_2_Mask(ioidWs));
      SigmaQInPMin(Fd(), som_ALL, Id_2_Mask(ioidFd));
      SigmaQInPMin(Ww(), som_ALL, Id_2_Mask(ioidWs));
  
      //// Select Model Transfer 
      //Fc.SelectModel(&Fd(), false);
      //Ff.SelectModel(&Fd(), false);
     
      bool HasFlw = (Sd().QMass(som_ALL)>UsableMass);
      bool HasFeed = (Fd().QMass(som_ALL)>UsableMass);
      //double CFeed = Fd().SpecieConc(Fd().Temp(), iScanEffSpecie, som_Liq);
      double CFeed = Fd().SpecieConc(Fd().Temp(), iWashEffSpecie, som_Liq);
      double CWash = Ww().SpecieConc(Ww().Temp(), iWashEffSpecie, som_Liq);

      double RqdFiltSolidsUsed;
      flag bVentErr = false;
      if (fOn && HasFeed)
        {
        flag Ok = true;
        //double POut = AtmosPress(); //force outlet to Atmos P
        double POut = Std_P; //force outlet to Std_P
        CFiltSolFracFnd Fnd(Sd(), Fd(), Ww(), Ff, Fc, POut, RqdBypass, RqdCakeMoist);
        if (iRqdFiltMethod == FSM_FiltSolFrac)
          {
          RqdFiltSolidsUsed = RqdFiltSolids;
          Fnd.Function(RqdFiltSolidsUsed);
          }
        else if (iRqdFiltMethod == FSM_FiltSolConc)
          {
          RqdFiltSolidsUsed = Valid(FiltSolidsUsed) ? FiltSolidsUsed : 0.01;
          Fnd.SetTarget(RqdFiltSolConc);
          Fnd.SetEstimate(RqdFiltSolidsUsed, 1.0);
          Ok = false;
          int iRet = Fnd.Start(0.0, 1.0);
          if (iRet==RF_EstimateOK) //estimate is good, solve not required
            {
            double dd = Fnd.Result();
            RqdFiltSolidsUsed = Fnd.RqdFiltSolidsUsed;
            Ok = true;
            }
          else
            {
            if (iRet==RF_BadEstimate)
              iRet = Fnd.Start(0.0, 1.0); // Restart
            if (iRet==RF_OK)
              if (Fnd.Solve_Brent()==RF_OK)
                {
                double dd = Fnd.Result();
                RqdFiltSolidsUsed = Fnd.RqdFiltSolidsUsed;
                Ok = true;
                }
            }
          }
        if (!Ok)
          {
          //SetCI(???);
          }

        //use result anyway...
        RqdFiltSolidsUsed = Fnd.RqdFiltSolidsUsed; 

        //put all vapours (if any) to vent (if present)...
        const double Qg = Sd().QMass(som_Gas);
        if (Qg>1e-12)
          {
          if (pVent)
            pVent->QSetM(Sd(), som_Gas, Qg, POut);
          else
            bVentErr = true;
          }
        else if (pVent)
          pVent->QZero();
        }
      else
        {//off or no feed
        RqdFiltSolidsUsed = 0.0;
        SigmaQInPMin(Fc, som_ALL, Id_2_Mask(ioidFd));
        SigmaQInPMin(Ff, som_ALL, Id_2_Mask(ioidWs));
        }

      //results...
      ActCakeSolids   = Fc.MassFrac(som_Sol);
	    ActFiltSolids   = Ff.MassFrac(som_Sol);
      ActCakeSolConc  = Fc.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      ActFiltSolConc  = Ff.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
      //ActFiltSolConcT = Ff.PhaseConc(Co.Temp(), som_Sol, som_ALL);
      FiltSolidsUsed = RqdFiltSolidsUsed;
      //double CCake = Fc.SpecieConc(Fc.Temp(), iScanEffSpecie, som_Liq);
      //double CFilt = Ff.SpecieConc(Ff.Temp(), iScanEffSpecie, som_Liq);
      //ActScandrettEff = (CFeed - CCake)/NZ(CFeed - CFilt); //Scandrett NOT aplicable to filter!
      double CCake = Fc.SpecieConc(Fc.Temp(), iWashEffSpecie, som_Liq);
      dSpWashEff = (CFeed - CCake)/NZ(CFeed - CWash);

      double CMErr = fabs(1.0 - ActCakeSolids - RqdCakeMoist);
		  SetCI(1, fOn && bTrackStatus && HasFlw && CMErr > 1.0e-6);
      if (iRqdFiltMethod == FSM_FiltSolConc)
        {
        double FCErr = fabs(ActFiltSolConc - RqdFiltSolConc);
		    SetCI(2, fOn && bTrackStatus && HasFlw && FCErr > 1.0e-5);
        }
      else
        {
        ClrCI(2);
        }
      SetCI(3, fOn && HasFlw && bVentErr);
      break;
      }
    }
  }

//--------------------------------------------------------------------------

flag DrumFilt::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tCake Moisture Requirements not met."; return 1;
    case  2: pS="W\tFiltrate Solids Concentration Requirements not met."; return 1;
    case  3: pS="W\tNo Vent Defined."; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
//==========================================================================



