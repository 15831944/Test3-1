//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __TANK_CPP
#include "tank.h"
//#include "optoff.h"

CVolumeShapeLookup::CVolumeShapeLookup()
  {
  bActive = false;
  bCheckRqd = true;
  iCount = 0;
  bWithSurfAreaLookup = 0;
  ALvl = 0.0;
  AHeight = 0.0;
  AArea = 0.0;
  Vol = 0.0;
  //TestVol = 10.0;
  }

void CVolumeShapeLookup::BuildDataDefn(TaggedObject* pTagObj, DataDefnBlk & DDB)
  {
  if (bActive)
    {
    if (DDB.BeginObject(pTagObj, "LL", "LookupLevel", NULL, DDB_RqdPage))
      {
      DDB.Long("Count", "", DC_, "", xidLvl_Count, pTagObj, isParmStopped);
      DDB.CheckBoxBtn("WithSurfArea", "", DC_, "", &bWithSurfAreaLookup, pTagObj, isParmStopped);
      DDB.Button("SortAndCheck", "", DC_, "", xidLvl_Sort, pTagObj, isParm);
      DDEF_Flags Vis=DDB.Visibility(NM_Dynamic|SM_All|HM_All, iCount>0);    
      //DDB.Double("TestVol", "", DC_Vol, "m^3", &TestVol, pTagObj, isParm);
      DDB.Double("Volume", "", DC_Vol, "m^3", &Vol, pTagObj, isResult|InitHidden|noFile|noSnap);
      DDB.Double("ALvl", "", DC_Frac, "%", &ALvl, pTagObj, isResult);
      DDB.Double("AHeight", "", DC_L, "mm", &AHeight, pTagObj, isResult);
      if (bWithSurfAreaLookup)
        DDB.Double("AArea", "", DC_Area, "m^2", &AArea, pTagObj, isResult);

      DDB.SetVisibility(Vis);
      /*DDB.Text("Volume Lookup...");
      Strng Tg;
      for (int i=0; i<iCount; i++)
        {
        Tg.Set("Vol.[%d]", i);
        DDB.Double(Tg(), "", DC_Vol, "m^3", xidLvl_FirstVol+i, pTagObj, isParm);
        }
      DDB.Text("Height Lookup...");
      for (int i=0; i<iCount; i++)
        {
        Tg.Set("Height.[%d]", i);
        DDB.Double(Tg(), "", DC_L, "mm", xidLvl_FirstHeight+i, pTagObj, isParm);
        }
      if (bWithSurfAreaLookup)
        {
        DDB.Text("Surface Area Lookup...");
        for (int i=0; i<iCount; i++)
          {
          Tg.Set("Area.[%d]", i);
          DDB.Double(Tg(), "", DC_Area, "m^2", xidLvl_FirstArea+i, pTagObj, isParm);
          }
        }*/
      Strng Tg1,Tg2;
      if (DDB.BeginObject(pTagObj, "Vol", "LookupVolume", NULL, DDB_OptPage))
        {
        DDB.Text("Volume Lookup...");
        for (int i=0; i<iCount; i++)
          {
          Tg1.Set("Volume_%03d", i);
          Tg2.Set("[%d]", i);
          DDB.Double(Tg1(), Tg2(), DC_Vol, "m^3", xidLvl_FirstVol+i, pTagObj, isParm);
          }
        }
      DDB.EndObject();
      if (DDB.BeginObject(pTagObj, "Height", "LookupHeight", NULL, DDB_OptPage))
        {
        DDB.Text("Height Lookup...");
        for (int i=0; i<iCount; i++)
          {
          Tg1.Set("Height_%03d", i);
          Tg2.Set("[%d]", i);
          DDB.Double(Tg1(), Tg2(), DC_L, "mm", xidLvl_FirstHeight+i, pTagObj, isParm);
          }
        }
      DDB.EndObject();
      if (bWithSurfAreaLookup)
        {
        if (DDB.BeginObject(pTagObj, "Area", "LookupArea", NULL, DDB_OptPage))
          {
          DDB.Text("Surface Area Lookup...");
          for (int i=0; i<iCount; i++)
            {
            Tg1.Set("Area_%03d", i);
            Tg2.Set("[%d]", i);
            DDB.Double(Tg1(), Tg2(), DC_Area, "m^2", xidLvl_FirstArea+i, pTagObj, isParm);
            }
          }
        DDB.EndObject();
        }
      }
    DDB.EndObject();
    }
  }

flag CVolumeShapeLookup::DataXchg(TaggedObject* pTagObj, DataChangeBlk & DCB)
  {
  if (DCB.lHandle==xidLvl_Lookup)
    {
    if (DCB.rB)// && (*DCB.rB!=0))
      bActive=(*DCB.rB!=0);
    DCB.B=bActive;
    return 1;
    }

  if (!bActive)
    return 0;

  switch (DCB.lHandle)
    {
    case xidLvl_Count:
      if (DCB.rL)
        {
        if (*DCB.rL!=iCount)
          {
          pTagObj->StructureChanged(pTagObj);
          SetCount(Range(0L, *DCB.rL, MaxLvlLookupCnt));
          }
        }
      DCB.L = iCount;
      return 1;
    case xidLvl_Sort:
      if (DCB.rB && (*DCB.rB!=0))
        SortAndValidate();
      DCB.B=0;
      return 1;
    default:
      if (DCB.lHandle>=xidLvl_FirstHeight && DCB.lHandle<xidLvl_FirstHeight+MaxLvlLookupCnt)
        {
        const int index = DCB.lHandle-xidLvl_FirstHeight;
        if (DCB.rD)
          {
          m_Heights[index] = Max(0.0, *DCB.rD);
          bCheckRqd = true;
          }
        DCB.D = m_Heights[index];
        return 1;
        }
      if (DCB.lHandle>=xidLvl_FirstVol && DCB.lHandle<xidLvl_FirstVol+MaxLvlLookupCnt)
        {
        const int index = DCB.lHandle-xidLvl_FirstVol;
        if (DCB.rD)
          {
          m_Volumes[index] = Max(0.0, *DCB.rD);
          bCheckRqd = true;
          }
        DCB.D = m_Volumes[index];
        return 1;
        }
      if (DCB.lHandle>=xidLvl_FirstArea && DCB.lHandle<xidLvl_FirstArea+MaxLvlLookupCnt)
        {
        const int index = DCB.lHandle-xidLvl_FirstArea;
        if (DCB.rD)
          {
          m_Areas[index] = Max(0.0, *DCB.rD);
          //bCheckRqd = true;
          }
        DCB.D = m_Areas[index];
        return 1;
        }
    }
  return 0;
  }

void CVolumeShapeLookup::SetCount(long NewCount)
  {
  NewCount = Range(0L, NewCount, MaxLvlLookupCnt);
  long PrevCnt = iCount;
  m_Heights.SetSize(NewCount);
  m_Volumes.SetSize(NewCount);
  m_Areas.SetSize(NewCount);
  if (PrevCnt>0 && NewCount>PrevCnt)
    {
    const double hd = (PrevCnt==1 ? m_Heights[0] : m_Heights[PrevCnt-1]-m_Heights[PrevCnt-2]);
    const double vd = (PrevCnt==1 ? m_Volumes[0] : m_Volumes[PrevCnt-1]-m_Volumes[PrevCnt-2]);
    const double ad = (PrevCnt==1 ? m_Areas[0] : m_Areas[PrevCnt-1]-m_Areas[PrevCnt-2]);
    for (int i=PrevCnt; i<NewCount; i++)
      {
      m_Heights[i] = m_Heights[i-1] + hd;
      m_Volumes[i] = m_Volumes[i-1] + vd;
      m_Areas[i] = m_Areas[i-1] + ad;
      }
    }
  bCheckRqd = true;
  iCount = NewCount;
  }

void CVolumeShapeLookup::SortAndValidate()
  {
  for (int i=1; i<iCount; i++)
    {
    for (long j=i; j>=1 && (m_Heights[j]<m_Heights[j-1]); j--)
      {
      double temp = m_Heights[j-1];
      m_Heights[j-1] = m_Heights[j];
      m_Heights[j] = temp;
      temp = m_Volumes[j-1];
      m_Volumes[j-1] = m_Volumes[j];
      m_Volumes[j] = temp;
      temp = m_Areas[j-1];
      m_Areas[j-1] = m_Areas[j];
      m_Areas[j] = temp;
      }
    }
  if (m_Heights[0]<1.0e-6)
    m_Heights[0] = 0.001;
  if (m_Volumes[0]<1.0e-6)
    m_Volumes[0] = 0.001;
  for (int i=1; i<iCount; i++)
    {
    if (m_Heights[i]<=m_Heights[i-1])
      m_Heights[i] = m_Heights[i-1]+0.001; //force increasing heights
    if (m_Volumes[i]<=m_Volumes[i-1])
      m_Volumes[i] = m_Volumes[i-1]+0.001; //force increasing volumes
    }
  bCheckRqd = false;
  }

void CVolumeShapeLookup::Calculate(double Volume)
  {
  Vol = Volume;
  if (iCount==0)
    {
    AHeight = 0.0;
    ALvl = 0.0;
    AArea = 0.0;
    return;
    }

  if (bCheckRqd)
    SortAndValidate();
  int i=0;
  while (i<iCount && Volume>m_Volumes[i])
    {
    i++;
    }
  if (i==iCount)
    {
    AHeight = m_Heights[iCount-1];//full
    if (bWithSurfAreaLookup)
      AArea = m_Areas[iCount-1];
    }
  else if (i==0)
    {
    const double f = Volume/m_Volumes[i];
    AHeight = m_Heights[i]*f;
    if (bWithSurfAreaLookup)
      AArea = m_Areas[i]*f;
    }
  else
    {
    const double f = (Volume-m_Volumes[i-1])/(m_Volumes[i]-m_Volumes[i-1]);
    AHeight = m_Heights[i-1] + (m_Heights[i]-m_Heights[i-1])*f;
    if (bWithSurfAreaLookup)
      AArea = m_Areas[i-1] + (m_Areas[i]-m_Areas[i-1])*f;
    }
  ALvl = AHeight/m_Heights[iCount-1];
  }

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
//const int ioid_OverFlw       = SpillIOId(0);
//const int ioid_GasVent       = VentIOId(0);
const int ioid_OverFlw       = FlwIOId(3);
const int ioid_GasVent       = FlwIOId(4);

//static IOAreaRec TankIOAreaListOld[] =
//  {
//    {"Top of Tank",                "Top",     ioid_Top,         LIO_In0Out,  nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 1.0f},
//    {"Input",                      "I",       ioid_Top,         LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 1.0f},
//    {"Side of Tank",               "Side",    ioid_Side,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOChgFracHgt|IOChgAperture, (float)0.5},
//    {"Bottom of Tank",             "Base",    ioid_Base,        LIO_InOut0,  nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 0.0f},
//    {"Output",                     "O",       ioid_Base,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOApertureHoriz|IOChgFracHgt|IOChgAperture, 0.0f},
//    {"OverFlow",                   "OverFlw", ioid_OverFlw,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry|IOOverflow, 1.0f},
//    {"GasVent",                    "GasVent", ioid_GasVent,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry|IOGasVent , 1.0f},
//    {"Heat Exchange Element",      "HX",      IOId_HX,          LIO_InOut,   nc_MLnk, 2, 20, IOGRP(1) },
//    {"Heat Exchange Element In",   "HX_In",   IOId_HX,          LIO_In1,     nc_MLnk, 2, 20, IOGRP(1) },
//    {"Heat Exchange Element Out",  "HX_Out",  IOId_HX,          LIO_Out1,    nc_MLnk, 2, 20, IOGRP(1) },
//    SPILL2AREA("Spills",  IOId_Spill2Area),
//    VENT2AREA("Vents",    IOId_Vent2Area),
//    {NULL}
//  }; //This lists the areas of the model where links can be attached.


static CAltIOName AIOTop[]   = {{"Top",   90}, {}};
static CAltIOName AIOBase[]  = {{"Base",  90}, {}};


static IOAreaRec TankIOAreaList[] =
  {
    {"Input",                      "I",       ioid_Top,         LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz,  1.0f,  {0,0,0,0}, AIOTop },
    {"Side of Tank",               "Side",    ioid_Side,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOChgFracHgt|IOChgAperture|IOHidden,         0.5f,  {0,0,0,0}         },
    {"Output",                     "O",       ioid_Base,        LIO_InOut,   nc_MLnk, 0, 20, IOPipeEntry|IOChgFracHgt|IOChgAperture|IOApertureHoriz,  0.0f,  {0,0,0,0}, AIOBase},
    {"OverFlow",                   "OverFlw", ioid_OverFlw,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry|IOBlockVapours|IOSSDenied,                   1.0f,  {0,0,0,0}         },
    {"GasVent",                    "GasVent", ioid_GasVent,     LIO_Out,     nc_MLnk, 0, 10, IOPipeEntry|IOPassVapours|IOSSDenied,                   1.05f, {0,0,0,0}         },
    {"Heat Exchange Element",      "HX",      IOId_HX,          LIO_InOut,   nc_MLnk, 2, 20, IOGRP(1)},
    {"Heat Exchange Element In",   "HX_In",   IOId_HX,          LIO_In1,     nc_MLnk, 2, 20, IOGRP(1)},
    {"Heat Exchange Element Out",  "HX_Out",  IOId_HX,          LIO_Out1,    nc_MLnk, 2, 20, IOGRP(1)},
    SPILL2AREA,
    VENT2AREA(ioid_GasVent),
    MAKEUP2AREA(),
    BLEED2AREA(),
    {NULL}
  };    

static double Drw_Tank[] = { DD_Poly,   -10,10, -10,-10, 10,-10, 10,10,
                             DD_TagPos, 0, -13.54, 0, 1,
                             DD_ValuesPos , 11, 6, -1, 1,
                             DD_End     }; //This provides the default drawing of the Tank.

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CTank, "Tank", "1", Drw_Tank, "Tank", "TNK", 
                    TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Tank(1)",   
                    "General Purpose Mixed Tank") 

CTank::CTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach)
  {
  //if (OrigPrjFileVerNo()<90)
  //  AttachIOAreas(TankIOAreaListOld, &PipeEntryGroup);
  //else
  //  AttachIOAreas(TankIOAreaListNew, &PipeEntryGroup);
  AttachIOAreas(TankIOAreaList, &PipeEntryGroup);

  //m_Spill.SetLclIOId(ioid_OverFlw);
  //m_Vent.SetLclIOId(ioid_GasVent);

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

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  if (PrjFileVerNo()<94 && DDB.ForFileSnpScn())
    {
    DDB.CheckBox("InlineIntegral",    "",  DC_,     "",      xidInlineIntegralOn,                this, isParmStopped|SetOnChange);
    }

  BuildDataDefnElevation(DDB);
  if (SolveSurgeMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_DynBoth|HM_All);
    DDB.Text    ("");
    DDB.Double  ("Pressure",     "P",     DC_P,    "kPag",   xidPMean,       this, isResult|noFile|noSnap);
    DDB.Double  ("Temperature",  "T",     DC_T,    "C",      xidTemp,        this, isResult|noFile|noSnap);
    DDB.Double  ("Density",      "Rho",   DC_Rho,  "kg/m^3", xidRho,         this, isResult|noFile|noSnap);
    DDB.Double  ("Level",        "Lvl",   DC_Frac, "%",      xidLevel,       this, isResult|noFile|noSnap);
    DDB.Double  ("Mass_Accum",   "QmAcc", DC_Qm,  "kg/s",    &m_QmAcc,       this, isResult|noFile|noSnap);
    DDB.Double  ("Vol_Accum",    "QvAcc", DC_Qv,  "L/s",     &m_QvAcc,       this, isResult|noFile|noSnap);
    DDB.Text    ("");
    }

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  AddMdlClosed(DDB);
  AddMdlNetworked(DDB);

  DDB.Visibility(NM_Probal|SM_All|HM_All);
  m_PCtrl0.BuildDataDefn(DDB, this, NetProbalMethod(), PC_BasicPB, PC_BasicDyn);//PC_Atmos|PC_Reqd);

  if (NetProbalMethod())
    DDB.CheckBox("CyclicStorage",     "",  DC_,     "",      &m_CyclicStorage.m_bOn, this, isParm|SetOnChange);

  DDB.Visibility();
  m_BlkEval.Add_OnOff(DDB);  

  DDB.Visibility(NM_All|SM_All|HM_All);//|(m_CyclicStorage.m_bOn ? 0:SM_Direct));
  HX.Add_OnOff(DDB);
  RB.Add_OnOff(DDB);
  if (1)//NetProbalMethod())
    m_EHX.Add_OnOff(DDB);
  //TODO RESTORE : VLE in ProBal mode!
  if (NetDynamicMethod())
    m_VLE.Add_OnOff(DDB);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  if (SolveInlineMethod())
    DDB.CheckBoxBtn("LevelControl", "",  DC_,     "",      &m_SrgCtrl.m_bEnabled,           this, isParmStopped|SetOnChange);
  if (SolveSurgeMethod())
    {
    DDB.CheckBoxBtn("ThermalBody", "", DC_, "", &m_EqThermals.m_bActive, this, isParmStopped|SetOnChange);
    DDB.CheckBox("LevelLookup", "", DC_, "", xidLvl_Lookup, this, isParmStopped|SetOnChange);
    }
    
  //DDB.Visibility(NM_Dynamic|SM_All|HM_All|(m_CyclicStorage.m_bOn ? 0:SM_Direct));
  DDB.Visibility(NM_Probal|SM_Direct|HM_All);
  GSM.Add_OnOff(DDB);
  DDB.Visibility(NM_All|SM_All|HM_All);
  DDB.CheckBox("ShowQFeed",         "",  DC_,     "",      &m_bShowQFeed,         this, isParm|SetOnChange);
  DDB.Visibility(NM_Probal|SM_Direct|HM_All);
  DDB.CheckBox("ShowQProd",         "",  DC_,     "",      &m_bShowQProd,         this, isParm|SetOnChange);

  if (NetProbalMethod() && m_CyclicStorage.m_bOn)
    {
    DDB.Visibility(NM_Probal|SM_All|HM_All, m_CyclicStorage.m_bOn);
    DDB.Text("");
    if (DDB.BeginStruct(this, "CyclicStorage"))
      {
      //DDB.CheckBox("On",              "",  DC_,     "",      &m_CyclicStorage.m_bOn,          this, isParm|SetOnChange);
      DDB.CheckBox("CycleIt",         "",  DC_,     "",      xidPBSurgeCycleIt,               this, isParm|SetOnChange);
      DDB.Double  ("CycleTime",       "",  DC_Time, "s",     &m_CyclicStorage.m_dCycleTime,   this, isParm);
      DDB.Double  ("Retained",        "",  DC_Frac, "%",     &m_CyclicStorage.m_dFracRetained, this, isParm);
      DDB.Double  ("TotalMass",       "",  DC_M,    "kg",    &m_CyclicStorage.m_dTotalMass,   this, isResult);
      DDB.Double  ("TotalVolume",     "",  DC_Vol,  "L",     &m_CyclicStorage.m_dTotalVolume, this, isResult);
      }
    DDB.EndStruct();
    }

  DDB.Visibility();

  if (SolveInlineMethod())
    m_SrgCtrl.BuildDataDefn(DDB, "LCtrl", this);

  DDB.Visibility();
  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);
  BuildDataDefnIOOpts(DDB);

  DDB.Visibility();
  m_BlkEval.BuildDataDefn(DDB);


  HX.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
  RB.BuildDataDefn(DDB);
  if (1)//NetProbalMethod())
    m_EHX.BuildDataDefn(DDB);
  if (NetDynamicMethod())
    m_VLE.BuildDataDefn(DDB);
  if (NetProbalMethod())
    GSM.BuildDataDefn(DDB);
  if (SolveSurgeMethod())
    {
    //DDB.CheckBox("LevelLookup", "", DC_, "", xidLvl_Lookup, this, isParmStopped|SetOnChange);
    if (LvlLookup.Active())
      LvlLookup.BuildDataDefn(this, DDB);
    }

  if (SolveSurgeMethod())
    m_EqThermals.BuildDataDefn(DDB, "Body", NULL, m_EqThermals.m_bActive ? DDB_RqdPage : DDB_OptPage);

  if (1)
    {
    DDB.Visibility(NM_All|SM_All|HM_All, m_bShowQFeed);
    DDB.Object(&m_QFeed, this, NULL, NULL, DDB_RqdPage);
    }
  if (NetProbalMethod())
    {
    DDB.Visibility(NM_Probal|SM_All|HM_All, m_bShowQProd);
    DDB.Object(&m_QProd, this, NULL, NULL, DDB_RqdPage);
    DDB.Visibility();
    }

  if (NetDynamicMethod() || (NetProbalMethod() && m_CyclicStorage.m_bOn))
    {
    DDB.Visibility();
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    }
  if ( (NetDynamicMethod() && Contents.GetPreset().DoImg()) || 
       (NetProbalMethod() && m_CyclicStorage.m_bOn) )
    {
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  //BuildDataDefnAreaIOs(DDB);

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CTank::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;
  if (m_VLE.DataXchg(DCB))
    return 1;
  if (HX.DataXchg(DCB))
    return 1;
  if (SolveSurgeMethod() && m_EqThermals.DataXchg(DCB))
    return 1;  
  if (SolveSurgeMethod() && LvlLookup.DataXchg(this, DCB))
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

void CTank::EvalProducts(CNodeEvalIndex & NEI)
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    default:
      {
      //SetProdMakeup(PMU_IOId|PMU_Passing, ioid_Condens, Contents, som_SL);
      MN_Surge::EvalProducts(NEI);

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

  if (SolveSurgeMethod() && LvlLookup.Active())
    {
    const double Vol = Contents.Volume();
    LvlLookup.Calculate(Vol);
    }
  }

//--------------------------------------------------------------------------

void CTank::EvalDerivs(CNodeEvalIndex & NEI)
  {
  MN_Surge::EvalDerivs(NEI);
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
    //    VLE.VFlash(Contents, 0.0, VLEF_Null);
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
