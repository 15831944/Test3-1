//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __TANK_CPP
#include "tank.h"
//#include "optoff.h"

//==========================================================================
/*#D:#T:CTank(1)
#X:#h<General Description>#nThe general purpose tank serves as a storage unit
for material. It is commonly used to store liquids, slurries and gases and can
either be open or closed. As a general purpose tank, any combination of general
splitter model, reactions and heat exchange can be configured if required.
Material can enter and leave from various positions and vents. When materials
are mixed in the tank, a homogenous product is produced.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Top           Input          0              20#n
Side          In/Out         0              20#n
Base          In/Out         0              20#n
OverFlw       Output         0              10#n
GasVent       Output         0              10          (Gas stream only)#n
   Additional inputs and outputs if heat Exchange is specified...#n
HX            In/Out         2              20#n
HX_In         Input          2              20#n
HX_Out        Output         2              20#n
#a#n
#h<Variables to be supplied by the user>#n
#i<General_Model> : This can be used to switch the #j<general split model>#d<General Model>
on. If it is on then the associated variables must be configured.#n
#i<Z_Reqd> : Datum.#n
#i<Closed> : Yes or no. Is the tank closed (a pressure vessel) or open (gases can 'escape').#n
#i<VLEquilibrium> : This can be used to switch #x<Vapour Liquid Equilibrium Logic>
on. If it is on then the associated variables (VLE) must be configured.#n
#i<HeatXChg> : This can be used to switch heat exchange functionality on. If it is on then the
associated variables (#x<heat exchange block>) must be configured.#n
#i<Reactions> : This can be used to switch reactions on. If it is on then the associated 
#x<reaction block> variables (RB) must be configured.#n
#i<Shape> : Select the unit shape.#n
#i<H> : Height of the vessel that contains material.#n
#i<V> : Volume of the vessel that contains material.#n
#i<Model> : This allows the selection of the model that is to be used to calculate
how the species are combined, this is normally done using a mass weighted mean.#n
#i<T_Rqd> : Set required temperature. Only used when SetTP is on.#n
#i<SetTP> : If this is set to on, for the next solver iteration only, the contents
of the unit will be 'flashed' to the specified temperature (T_Rqd).#n
#i<OverFlowTarget> : The tag of the unit to which overflow material must spill. If none
is specified, 'Floor' is assumed.#n
#i<GasVentTarget> : The tag of the unit to which vented gas must report.#n
#i<View> : This allows the selection of how the list of species is being displayed.
Options such as MassFlow, MoleFlow, VolFlow, MassFrac, ElemntFlow, CompntConc etc can
be selected.#n
#i<Phase> : This allows the selection of which phase(s) for the list of species is
being displayed. Options such as All, Solids, Liquids and Vapours can be selected.#n
#i<Species List> : This is a list of all the available species for the project. The
quantity of each specie in the unit is shown and may be altered.#n
#n#n
#h<Associated variables>#n
#i<Z> : The height of the unit, in relation to the common datum.#n
#i<P> : Pressure. If the tank is open, this is equivalent to the atmospheric pressure.
If the tank is closed, the pressure on the surface of the material is equivalent to
the vapour pressure of the gases in the tank.#n
#i<T> : The temperature of the material within the unit.#n
#i<Rho> : The density of the material within the unit.#n
#i<Lvl> : A ratio, expressed as a percentage, of the total volume of the solids and
liquids, in the vessel, to the volumetric capacity of the unit.#n
#i<QmAcc> : The rate at which the mass of the material accumulates in the unit.#n
#i<QvAcc> : The rate at which the volume of the material accumulates in the unit.#n
#n
#n#h<Other>#n
Default model prefix:TNK#n
Short name:CTank#n
Model type:Unit#n
#G:Units
*/
//==========================================================================
const int ioid_Top           = FlwIOId(0);
const int ioid_Side          = FlwIOId(1);
const int ioid_Base          = FlwIOId(2);
const int ioid_OverFlw       = FlwIOId(3);
const int ioid_GasVent       = FlwIOId(4);

static IOAreaRec TankIOAreaListOld[] =
  {
    {"Top of Tank",                "Top",     ioid_Top,         LIO_In0Out,  nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOShwFracHgt|IOShwAperture, 1.0f},
    {"Input",                      "I",       ioid_Top,         LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOShwFracHgt|IOShwAperture, 1.0f},
    {"Side of Tank",               "Side",    ioid_Side,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOShwFracHgt|IOShwAperture, (float)0.5},
    {"Bottom of Tank",             "Base",    ioid_Base,        LIO_InOut0,  nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOShwFracHgt|IOShwAperture, 0.0f},
    {"Output",                     "O",       ioid_Base,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOShwFracHgt|IOShwAperture, 0.0f},
    {"OverFlow",                   "OverFlw", ioid_OverFlw,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry, 1.0f},
    {"GasVent",                    "GasVent", ioid_GasVent,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry, 1.0f},
    {"Heat Exchange Element",      "HX",      IOId_HX,          LIO_InOut,   nc_MLnk, 2, 20, IOGRP(1) },
    {"Heat Exchange Element In",   "HX_In",   IOId_HX,          LIO_In1,     nc_MLnk, 2, 20, IOGRP(1) },
    {"Heat Exchange Element Out",  "HX_Out",  IOId_HX,          LIO_Out1,    nc_MLnk, 2, 20, IOGRP(1) },
    SPILL2AREA("Spills",  IOId_Spill2Area),
    VENT2AREA("Vents",    IOId_Vent2Area),
    {NULL}
  }; //This lists the areas of the model where links can be attached.


static IOAreaRec TankIOAreaListNew[] =
  {
    {"Input",                      "I",       ioid_Top,         LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOShwFracHgt|IOShwAperture, 1.0f},
    {"Side of Tank",               "Side",    ioid_Side,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOShwFracHgt|IOShwAperture|IOHidden, (float)0.5},
    {"Output",                     "O",       ioid_Base,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOShwFracHgt|IOShwAperture, 0.0f},
    {"OverFlow",                   "OverFlw", ioid_OverFlw,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry, 1.0f},
    {"GasVent",                    "GasVent", ioid_GasVent,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry, 1.0f},
    {"Heat Exchange Element",      "HX",      IOId_HX,          LIO_InOut,   nc_MLnk, 2, 20, IOGRP(1) },
    {"Heat Exchange Element In",   "HX_In",   IOId_HX,          LIO_In1,     nc_MLnk, 2, 20, IOGRP(1) },
    {"Heat Exchange Element Out",  "HX_Out",  IOId_HX,          LIO_Out1,    nc_MLnk, 2, 20, IOGRP(1) },
    SPILL2AREA("Spills", IOId_Spill2Area),
    VENT2AREA("Vents",   IOId_Vent2Area),
    {NULL}
  };    

static double Drw_Tank[] = { DD_Poly, -10,10, -10,-10, 10,-10, 10,10,
                      DD_End }; //This provides the default drawing of the Tank.

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CTank, "Tank", "1", Drw_Tank, "Tank", "TNK", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Tank(1)",   
                    "General Purpose Mixed Tank") 

CTank::CTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  if (OrigPrjFileVerNo()<90)
    AttachIOAreas(TankIOAreaListOld, &PipeEntryGroup);
  else
    AttachIOAreas(TankIOAreaListNew, &PipeEntryGroup);

  Contents.SetClosed(False);
  Contents.SetPreset(&m_Preset, &m_PresetImg);
  Contents.SetSQFlags(SQF_Editable, true);
  Contents.dwDisplay&=~SPDF_ShapeNameDisplay;
  };

//--------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/

void CTank::BuildDataDefn(DataDefnBlk & DDB)
  {
  Contents.SetSQFlags(SQF_Editable, true);//why does setting this flag above in constructor not work?
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);

  if (PrjFileVerNo()<94)
    {
    DDB.Text    ("");
    DDB.CheckBox("InlineIntegral",    "",  DC_,     "",      xidInlineIntegralOn,                this, isParmStopped|SetOnChange);
    }
  BuildDataDefnElevation(DDB);
  DDB.Text    ("");
  DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|noFile|noSnap);
  DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|noFile|noSnap);
  DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|noFile|noSnap);
  DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|noFile|noSnap);
  DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &QmAcc,         this, isResult|noFile|noSnap);
  DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &QvAcc,         this, isResult|noFile|noSnap);

  DDB.Text    ("");


  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);

  DDB.Visibility(SM_Probal|HM_All);
  m_PBPCtrl0.BuildDataDefn(DDB, this, PBPC_Basic);
  if (DDB.BeginStruct(this, "CyclicStorage"))
    {
    DDB.CheckBox("On",              "",  DC_,     "",      &m_CyclicStorage.m_bOn,          this, isParm|SetOnChange);
    DDB.Visibility(SM_Probal|HM_All, m_CyclicStorage.m_bOn);    
    DDB.CheckBox("CycleIt",         "",  DC_,     "",      xidPBSurgeCycleIt,               this, isParm|SetOnChange);
    DDB.Double  ("CycleTime",       "",  DC_Time, "s",     &m_CyclicStorage.m_dCycleTime,   this, isParm);
    DDB.Double  ("Retained",        "",  DC_Frac, "%",     &m_CyclicStorage.m_dFracRetained, this, isParm);
    DDB.Double  ("TotalMass",       "",  DC_M,    "kg",    &m_CyclicStorage.m_dTotalMass,   this, isResult);
    DDB.Double  ("TotalVolume",     "",  DC_Vol,  "L",     &m_CyclicStorage.m_dTotalVolume, this, isResult);
    }
  DDB.EndStruct();

  DDB.Visibility(SM_DynBoth|HM_All|(m_CyclicStorage.m_bOn ? 0:SM_Probal));
  GSM.Add_OnOff(DDB);
  DDB.Visibility(SM_DynBoth|HM_All|(m_CyclicStorage.m_bOn ? 0:SM_Probal));
  //TODO RESTORE : VLE in ProBal mode!
  HX.Add_OnOff(DDB);
  RB.Add_OnOff(DDB);
  if (ProbalMode())
    EHX.Add_OnOff(DDB);
  if (DynamicMode())
    VLE.Add_OnOff(DDB);
  DDB.Visibility(SM_DynBoth|HM_All);
  if (InlineIntegral() && DynamicMode())
    DDB.CheckBoxBtn("LevelControl", "",  DC_,     "",      &m_SrgCtrl.m_bEnabled,           this, isParmStopped|SetOnChange);
  DDB.Visibility(SM_DynBoth|HM_All|(m_CyclicStorage.m_bOn ? 0:SM_Probal));
  DDB.Visibility(SM_Probal|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &bShowQFeed,         this, isParm|SetOnChange);
  DDB.CheckBox("ShowQProd",         "",  DC_,     "",      &bShowQProd,         this, isParm|SetOnChange);
  DDB.Visibility();
 
  if (InlineIntegral() && DynamicMode())
    m_SrgCtrl.BuildDataDefn(DDB, "LCtrl", this);

  DDB.Visibility();
  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  HX.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
  RB.BuildDataDefn(DDB);
  if (ProbalMode())
    EHX.BuildDataDefn(DDB);
  if (DynamicMode())
    VLE.BuildDataDefn(DDB);
  if (ProbalMode())
    GSM.BuildDataDefn(DDB);

  if (DynFullMode())
    m_EqThermals.BuildDataDefn(DDB, "Body", NULL, DDB_RqdPage);

  if (ProbalMode())
    {
    QFeed(); // Ensure that exist so that they recover
    if (QFeed.Exists())
      {
      DDB.Visibility(SM_Probal|HM_All, bShowQFeed);
      DDB.Object(&QFeed, this, NULL, NULL, DDB_RqdPage);
      }
    QProd(); // Ensure that exist so that they recover
    if (QProd.Exists())
      {
      DDB.Visibility(SM_Probal|HM_All, bShowQProd);
      DDB.Object(&QProd, this, NULL, NULL, DDB_RqdPage);
      }
    DDB.Visibility();
    }

  if (DynamicMode() || ProbalMode() && m_CyclicStorage.m_bOn)
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
  if (DynamicMode() && Contents.GetPreset().DoImg() || 
      ProbalMode() && m_CyclicStorage.m_bOn)
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);

  if (DynamicMode())
    {
    if (m_Spill.Enabled || m_Vent.Enabled || m_AccIn.Enabled || m_AccOut.Enabled)
      {
      DDB.Page("AreaIO", DDB_RqdPage);
      m_Spill.BuildDataDefn(DDB, NULL, DDB_NoPage, 1);
      m_Vent.BuildDataDefn(DDB, NULL, DDB_NoPage, 2);
      m_AccIn.BuildDataDefn(DDB, NULL, DDB_NoPage, 3);
      m_AccOut.BuildDataDefn(DDB, NULL, DDB_NoPage, 4);
      }
    }

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CTank::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  if (VLE.DataXchg(DCB))
    return 1;
  if (HX.DataXchg(DCB))
    return 1;
  if (!DynFlowMode() && m_EqThermals.DataXchg(DCB))
    return 1;  

  return 0;
  }

//---------------------------------------------------------------------------

flag CTank::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Surge::ValidateData(VDB);
//  Contents.
//  bool TAdj=!Contents.Closed() || !VLE.Enabled() || VLE.HasTPEnvelope();
//
////  #pragma chMSG(Temporary TAdj Overide)
////  TAdj=True;
//  Contents.GetPresetInfo().bTAdj=TAdj;
  return OK;
  }

//--------------------------------------------------------------------------

void CTank::EvalProducts(long JoinMask)
  {
  switch (SolveMode())
    {
    case SM_Probal:
    default:
      {
      //SetProdMakeup(PMU_IOId|PMU_Passing, ioid_Condens, Contents, som_SL);
      MN_Surge::EvalProducts(JoinMask);

      // Temp for SKO
      //StkSpConduit InAvg("QPF", chLINEID(), this);
      //InAvg().QZero();
      //for (int i=0; i<NoProcessIOs(); i++)
      //  if (IOQm_In(i)>SmallPosFlow)
      //    InAvg().QAddM(*IOConduit(i), som_ALL, IOQm_In(i));
      //// InAvg() (s SpConduit) should now contain the mass weighted mean of what comes in
      //// Use this as the image to set the outputs.
      //for (int i=0; i<NoProcessIOs(); i++)
      //  {
      //  if (IOQm_Out(i)>SmallPosFlow)
      //    {
      //    IOConduit(i)->QSetM(InAvg(), som_ALL, IOQm_Out(i));
      //    IOConduit(i)->SetTempPress(Contents.Temp(), Contents.Press());
      //    }
      //  }
      //double T=Contents.Temp();
      //double P=Contents.Press();
      //double L=Contents.Level();
      //double M=Contents.Mass();

      //P=GTZ(P);
      //if (Contents.Closed())
      //  {
      //  Contents.SetM(InAvg(), som_ALL, M, P);
      //  Contents.SetTemp(T);
      //  //Converge The Mass in the closed valume to give the required Pressure
      //  double PErr=Contents.Press()-P;
      //  while (fabs(PErr)<1e-6)
      //    {
      //    Contents.ScaleMass(som_ALL, 1.0/(Contents.Press()/P));
      //    PErr=Contents.Press()-P;
      //    }
      //  }
      //else
      //  {
      //  // ToDo
      //  __asm int 3;
      //  }
      
      }
    }
  }


//--------------------------------------------------------------------------

void CTank::EvalDerivs(long JoinMask)
  {
  MN_Surge::EvalDerivs(JoinMask);
  if (!GetActiveHold())
    {
    //if (VLE.Enabled())
    //  {
    //  // Short Exploratory Step to estimate Rates of Flashing
    //  Contents.SetHoldVentExpand();
    //  SpMArray &ContentsMA=Contents.Model()->MArray();
    //  SpMArray &ContentsdMA=Contents.Model()->dM.MArray();
    //
    //  SpMArray Mass0(ContentsMA);
    //  double totH0=Contents.totHf();
    //  //double dtotH0=Contents.dtotEnthalpy();
    //  double dT=ICGetTimeInc();
    //  double DeltaMass=0.1;
    //  double DeltaFrac=1.0;
    //  for (int s=0; s<SDB.No(); s++)
    //    // Only Negetive
    //    if (ContentsdMA[s]<-1.0e-9)
    //      DeltaFrac=Min(DeltaFrac, fabs(DeltaMass*ContentsMA[s]/NZ(ContentsdMA[s] * dT)));
    //  if (DeltaFrac<1.0e-12)
    //    {
    //    //LogNote(FullObjTag(), 0, "Small Delta Frac");
    //    }
    //  else
    //    {
    //
    //    dT*=DeltaFrac;
    //    for (s=0; s<SDB.No(); s++)
    //      ContentsMA[s]+=dT*ContentsdMA[s];
    //    Contents.Set_totHf(totH0+dT*dtotH0);
    //    SpMArray Mass1(ContentsMA);
    //
    //    VLE.QVFlash(Contents, 0.0, VLEF_Null);
    //
    //    for (s=0; s<SDB.No(); s++)
    //      ContentsdMA[s]+=(ContentsMA[s]-Mass1[s])/GTZ(dT);
    //    }
    //
    //  // Restore
    //  ContentsMA=Mass0;
    //  Contents.Set_totHf(totH0);
    //
    //  Contents.ClrHoldVentExpand();
    //  }
    }
  }

// ===========================================================================
