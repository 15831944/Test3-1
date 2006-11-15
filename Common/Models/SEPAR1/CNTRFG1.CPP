//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1996
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __CNTRFG1_CPP
#include "cntrfg1.h"

//==========================================================================
/*#D:#T:Disk Centrifuge                       
#X:#h<General Description>#nThe Disk Centrifuge mixes perfectly all the incoming
feed streams and then, based on the user configuration, calculates the amount,
temperature and composition of material reporting to the products.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed            Input          1              20#n
Solids         Output          1              1#n
Centrate       Output          1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Datum_Rqd> : The required height of the unit relative to the common datum of 
the system.  Dynamic mode only#n
#i<Method> : The required method of calculating the distribution of the feed solids 
and liqids between the cake and the centrate.  The methods avaliable to the user are:#n
1. SolidsRec/SolidAnal: The user specifies the solids recovery to the cake and the 
solids analysis of the cake.#n
2. SolidsRec/MoistAnal: The user specifies the solids recovery to the cake and the
cake moisture.#n
3. CentConc/SolidAnal: The user specifies the centrate concentration and the solids 
analysis of the cake.#n
4. CentConc/MoistAnal: The user specifies the centrate concentration and the cake 
moisture.
#n#n
#h<Calculated variables>#n
The following variables are only visible in dynamic mode:#n
#i<Datum>: The actual height of the unit relative to the common datum of the system.#n
#i<Pressure>: The pressure within the unit.#n
#i<Temperature>: The temperature of the material contained within the unit.#n
#i<Density>: The density of the material within the unit. This includes the centrate 
and the cake.#n
#i<Level>: The level of the slurry within the unit.#n
#i<Mass_Accum>: The mass of material accumulated within the unit.#n
#i<Vol_Accum>: The volume of material accumulated within the unit.#n
#n
#n#h<Other>#n
Default model prefix:DC#n
Short name:Cntrfuge#n
Model type:Unit#n
#G:Units
*/

const byte ioidFeed          = 0;
const byte ioidCentrate      = 1;
const byte ioidSolids        = 2;
//const byte ioidVent          = 3;

static IOAreaRec CentrifugeIOAreaList[] =
  {{"",    "Feed"      , ioidFeed,     LIO_In0 ,    nc_MLnk, 1, 20, IOGRP(0)},
   {"",    "Centrate"  , ioidCentrate, LIO_Out0,    nc_MLnk, 1,  1, IOGRP(0)},
   {"",    "Solids"    , ioidSolids,   LIO_Out1,    nc_MLnk, 1,  1, IOGRP(0)},//cake
   //{"",    "Vent"      , ioidVent,     LIO_Out,    nc_MLnk, 0,  1, IOPipeEntry|IOGasVent, 1.0f},
   SPILL2AREA("Spills", IOId_Spill2Area),
   VENT2AREA("Vents",   IOId_Vent2Area),
   {NULL}}; //This lists the areas of the model where links can be attached.


static double Drw_Centrifuge[] = { DD_Poly, -10,8, 10,8, 10,-8, -10,-8, -10,8,
																					-10,6, 10,3, 10,-3, -10,-6, DD_End }; 

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Centrifuge1, "Cntrfuge", "1", Drw_Centrifuge, "Filter", "DC", TOC_PROBAL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Disk Centrifuge(1)",
                    "The Disk Centrifuge mixes perfectly all the incoming feed streams and then, "
                    "based on the user configuration, calculates the amount, temperature and "
                    "composition of material reporting to the products. ")

Centrifuge1::Centrifuge1(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(CentrifugeIOAreaList, &PipeEntryGroup);
  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);

  iMeth=CSF_SolRec|CSF_SolidAnal;
  Reqd_SolMFrac = 0.9;
  Reqd_SolConc  = 700.0;   // Underflow solids at 25 dC in g/L
  Reqd_CenConc  = 0.1;     // Underflow solids at 25 dC in g/L  (100mg/L)
  Reqd_SolRec   = 0.9;
  ActCakeSolids      = Reqd_SolMFrac;
  ActCentrateSolids  = 0.0;
  ActCakeSolConc     = Reqd_SolConc;
  ActCentrateSolConc = Reqd_CenConc;

  //EHX.Open(&CEHX_LossPerQmClass);

  SolFlwFrac    = 0.0;
  CenFlwFrac    = 0.0;
  bTrackStatus  = true;
  }

// -------------------------------------------------------------------------

Centrifuge1::~Centrifuge1()
  {
  }

// -------------------------------------------------------------------------

void Centrifuge1::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|noFile|noSnap);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|noFile|noSnap);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|noFile|noSnap);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|noFile|noSnap);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, isResult|noFile|noSnap);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, isResult|noFile|noSnap);

  DDB.Text    ("");
  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);
  
  DDB.Visibility();
  DDB.Text    ("");
  static DDBValueLst DDB1[]={
    {CSF_SolRec|CSF_SolidAnal,    "SolidsRec/SolidAnal"},
    {CSF_SolRec|CSF_MoistAnal,    "SolidsRec/MoistAnal"},
    {CSF_CentConc|CSF_SolidAnal,  "CentConc/SolidAnal"},
    {CSF_CentConc|CSF_MoistAnal,  "CentConc/MoistAnal"},
    {0}};
  DDB.Text    ("Requirements");
  DDB.Byte    ("",  "Method",     DC_, "",    &iMeth, this, isParm, DDB1);

  if (PrjFileVerNo()<49)
    {
    DDB.Double  ("",  "RqdSolidsRec",     DC_Frac, "%",      &Reqd_SolRec,   this, isParm | (iMeth&CSF_SolRec    ? 0 : noView));
    DDB.Double  ("",  "RqdCentConc",      DC_Conc, "mg/L",   &Reqd_CenConc,  this, isParm | (iMeth&CSF_CentConc  ? 0 : noView));
    DDB.Double  ("",  "RqdSolidsAnal",    DC_Frac, "%",      &Reqd_SolMFrac, this, isParm | (iMeth&CSF_SolidAnal ? 0 : noView));
    DDB.Double  ("",  "RqdMoistAnal",     DC_Frac, "%",      xidMoistAnal,   this, isParm | (iMeth&CSF_MoistAnal ? 0 : noView));
    }
  else
    {
    DDB.Double  ("Rqd_SolidsToCake_Recovery","RqdSolidsRec",   DC_Frac, "%",      &Reqd_SolRec,        this, isParm | (iMeth&CSF_SolRec    ? 0 : noView));
    DDB.Double  ("Rqd_CentrateConc25",      "RqdCentConc25",   DC_Conc, "g/L",    &Reqd_CenConc,       this, isParm | (iMeth&CSF_CentConc  ? 0 : noView));
    DDB.Double  ("Rqd_Cake_Solids",         "RqdCakeSolids",   DC_Frac, "%",      &Reqd_SolMFrac,      this, isParm | (iMeth&CSF_SolidAnal ? 0 : noView));
    DDB.Double  ("Rqd_Cake_Moisture",       "RqdCakeMoist",    DC_Frac, "%",      xidMoistAnal,        this, isParm | (iMeth&CSF_MoistAnal ? 0 : noView));
    }
  DDB.Visibility();
  DDB.Text    ("Results");
  DDB.Double  ("Cake_Solids",             "CakeSolids",      DC_Frac, "%",      &ActCakeSolids,      this, isResult);//|noFile|noSnap);
  DDB.Double  ("Centrate_Solids",         "CentrateSolids",  DC_Frac, "%",      &ActCentrateSolids,  this, isResult);//|noFile|noSnap);
  DDB.Double  ("CakeSolidsConc@25",       "CakeSolConc25",   DC_Conc, "g/L",    &ActCakeSolConc,     this, isResult);//|noFile|noSnap);
  DDB.Double  ("CentrateSolidsConc@25",   "CentrateSolConc25",DC_Conc,"g/L",    &ActCentrateSolConc, this, isResult);//|noFile|noSnap);
  
  DDB.Text    ("");
  RB.Add_OnOff(DDB);
  EHX.Add_OnOff(DDB);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.CheckBox("",                        "TrackStatus",     DC_,     "",       &bTrackStatus,       this, isParm);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  
  RB.BuildDataDefn(DDB);
  EHX.BuildDataDefn(DDB);

  if (SolveSurgeMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag Centrifuge1::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMoistAnal:
      if (DCB.rD)
        Reqd_SolMFrac=(1.0-*DCB.rD);
      DCB.D=1.0-Reqd_SolMFrac;
      return 1;
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB); 
      DCB.B=Contents.Closed(); 
      return 1;
    };

  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }           

//---------------------------------------------------------------------------

flag Centrifuge1::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
  Reqd_SolMFrac=ValidateRange(VDB, "Reqd_SolMFrac", 0.0, Reqd_SolMFrac, 1.0);
  return OK;
  }

//--------------------------------------------------------------------------

void Centrifuge1::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      for (int j=0; j<NoProcessJoins(); j++)
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

void Centrifuge1::EvalProducts(CNodeEvalIndex & NEI)
  {
  const int ioFeed = IOWithId_Self(ioidFeed);
  if (ioFeed>=0)
    {
    const int ioSolids = IOWithId_Self(ioidSolids); //this is the cake
    const int ioCentrate = IOWithId_Self(ioidCentrate);
    //const int iVent = IOWithId_Self(ioidVent);
    SpConduit & Qs = *IOConduit(ioSolids);
    SpConduit & Qc = *IOConduit(ioCentrate);
    //SpConduit * pVent = (iVent>=0 ? IOConduit(iVent) : NULL);

    StkSpConduit Qm("Qm", chLINEID(), this);//, TOA_Free);
    //Qm().QZero();

    SigmaQInPMin(Qm(), som_ALL, Id_2_Mask(ioidFeed));
    flag HasFlw = (Qm().QMass(som_ALL)>UsableMass);

    RB.EvalProducts(Qm());
    EHX.EvalProducts(Qm());

    const double Qg = Qm().QMass(som_Gas);
    SetCI(4, Qg>1e-9);

    /*
    //put all vapours (if any) to vent (if present)...
    bool bVentErr = false;
    const double Qg = Qm().QMass(som_Gas);
    if (Qg>1e-12)
      {
      if (pVent)
        pVent->QSetM(Qm(), som_Gas, Qg, Std_P);
      else
        bVentErr = true;
      }
    else if (pVent)
      pVent->QZero();
    */

    // Density Calc should be done @ Temp // 25dC
    // double Ts=Qm.Temp();
    // Qm.SetTemp(C_2_K(25.0));

    double RhoS=GTZ(Qm().Rho(som_Sol));
    double RhoL=GTZ(Qm().Rho(som_Liq));

    double Fs=Qm().MassFrac(som_Sol);
    double Fl=Qm().MassFrac(som_Liq);

    double Ms=Qm().QMass(som_Sol);
    double Ml=Qm().QMass(som_Liq);

    double MFracUs = Range(0.0, Reqd_SolMFrac, 1.0);
    double ConcUs = Range(1.0, Reqd_SolConc, 5000.0);
    double ConcFd = Ms/GTZ(Qm().QVolume(som_SL));
    double MFracFd = Ms/GTZ(Qm().QMass(som_SL));

    if (iMeth & CSF_CentConc)
      {
      double ConcOs = Range(0.0, Reqd_CenConc, ConcUs-1.0e-6);
      if (ConcFd>=ConcUs)
        {
        // Too Dense
        Qs.QSetF(Qm(), som_SL, 1.0, Std_P);
        Qc.QSetF(Qm(), som_SL, 0.0, Std_P);
        }
      else if (ConcFd<=ConcOs)
        {
        // Too Fluid
        Qs.QSetF(Qm(), som_SL, 0.0, Std_P);
        Qc.QSetF(Qm(), som_SL, 1.0, Std_P);
        }
      else
        {
        // MassFrac in Over & Under
        double Os=RhoS*ConcOs/GTZ(RhoL*RhoS-RhoL*ConcOs+RhoS*ConcOs);
        double Us;
        Us=MFracUs;
        
        Os=Range(0.0, Os, 1.0);
        Us=Range(0.0, Us, 1.0);
      
        double FQm=Ms+Ml;
        double UQm=FQm*Range(0.0, (Fs-Os)/GTZ(Us-Os), 1.0);
        double OQm=FQm-UQm;
      
        double Fs2U=Range(0.0, UQm*Us/GTZ(Ms), 1.0);
        double Fl2U=Range(0.0, UQm*(1.0-Us)/GTZ(Ml), 1.0);
      
        Qs.QSetF(Qm(), som_Sol, Fs2U, Std_P);
        Qs.QAddF(Qm(), som_Liq, Fl2U);
      
        Qc.QSetF(Qm(), som_Sol, 1.0-Fs2U, Std_P);
        Qc.QAddF(Qm(), som_Liq, 1.0-Fl2U);
        }
      
      }
    else if (iMeth & CSF_SolRec)
      {
      double URec = Range(0.0, Reqd_SolRec,  1.0);

      double Msu=URec*Ms;
      double Mlu=Range(0.0, Msu*(1.0/MFracUs-1.0), Ml);

      Qs.QSetM(Qm(), som_Sol, Msu, Std_P);
      Qs.QAddM(Qm(), som_Liq, Mlu);

      Qc.QSetM(Qm(), som_Sol, GEZ(Ms-Msu), Std_P);
      Qc.QAddM(Qm(), som_Liq, GEZ(Ml-Mlu));
      }
    else
      {
      // Should not get here
      ASSERT(0);
      }

    SolFlwFrac=Qs.QMass()/GTZ(IOQmEst_Out(ioSolids));
    if (SolFlwFrac>1.0)
      Qs.QScaleMass(som_ALL, 1.0/SolFlwFrac);
    CenFlwFrac=Qc.QMass()/GTZ(IOQmEst_Out(ioCentrate));
    if (CenFlwFrac>1.0)
      Qc.QScaleMass(som_ALL, 1.0/CenFlwFrac);
    Qs.SetTemp(Qm().Temp());
    Qc.SetTemp(Qm().Temp());

    //results...
    ActCakeSolids      = Qs.MassFrac(som_Sol);
    ActCentrateSolids  = Qc.MassFrac(som_Sol);
    ActCakeSolConc     = Qs.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);
    ActCentrateSolConc = Qc.PhaseConc(C_2_K(25.0), som_Sol, som_ALL);

    double CSErr = fabs(ActCakeSolids - Reqd_SolMFrac);
	  SetCI(1, bTrackStatus && HasFlw && CSErr > 1.0e-6);
    double CCErr = fabs(ActCentrateSolConc - Reqd_CenConc);
	  SetCI(2, bTrackStatus && HasFlw && (iMeth&CSF_CentConc) && CCErr > 1.0e-5);
    //SetCI(3, HasFlw && bVentErr);
    }
  else
    {
    SolFlwFrac=0.0;
    CenFlwFrac=0.0;
    }
  }

// --------------------------------------------------------------------------

flag Centrifuge1::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tCake Solids (or Moisture) Requirements not met.";    return 1;
    case  2: pS="W\tCentrate Solids Concentration Requirements not met."; return 1;
    case  3: pS="W\tNo Vent Defined."; return 1;
    case  4: pS="E\tDo not expect vapour in feed (or reaction output)."; return 1;
    default:                                               
      return MN_Surge::CIStrng(No, pS);
    }
  }

//==========================================================================

