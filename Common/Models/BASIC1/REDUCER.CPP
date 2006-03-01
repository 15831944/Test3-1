//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include "sc_defs.h"
#define  __REDUCER_CPP
#include "reducer.h"

//==========================================================================
//
//
//
//==========================================================================
/*The reducer, joins together two pipes which have
different diameters. The reducer is always connected from an outlet point of one
pipe to an inlet point of another. It is commonly used to regulate the flow of
liquids, slurries and gases. The reducer only has one inlet and one outlet point.
*/
//==========================================================================

static double Drw_Reducer[] = { DD_Poly, -4,3, -4,-3, 4,-1, 4,1, -4,3,
                       DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Reducer, "Reducer", "1", Drw_Reducer, "Piping", "RDCR", TOC_ALL|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Process:Piping:Reducer(1)",
                    "Reducer") 

Reducer::Reducer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
   MN_BstRes(pClass_, TagIn, pAttach, eAttach)//, VPB("VL_Basic", this)
  {
  AttachIOAreas(TwoIOAreaList, &SizeChangeGroup, &NullFlwGroup);
  m_FEP.AssignFlwEqnGroup(SizeChangeGroup, SizeChangeGroup.Default()/*"VE_Line"*/, this);
  //AttachIOAreas(TwoIOAreaList, &ValveGroup, &NullFlwGroup);
  //FB.AssignFlwEqnGroup(ValveGroup, ValveGroup.Default()/*"VE_Line"*/, this);
  //VPB.AssignLogicGroup("VPL_Smpl", this); 
  //Initialise();
  }

//--------------------------------------------------------------------------
/*
void Reducer::Initialise()
  {
  FB.Initialise();
  }
*/
//--------------------------------------------------------------------------

void Reducer::BuildDataDefn(DataDefnBlk &DDB)
  {
  static DDBValueLst DDB1[]={
    {BRPM_None, "None"},
    {BRPM_Drop, "Drop"},
    {BRPM_Fixed, "Fixed"},
    {BRPM_Atmos, "Atmos"},
    {0}};
  DDB.BeginStruct(this);
  if (ProbalMode())
    {
    DDB.Text    ("");
    DDB.Text    ("Requirements");
    DDB.Byte    ("PressMethod",     "",     DC_,      "",      xidPressMethod,  this, isParm|SetOnChange, DDB1);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Drop);
    DDB.Double  ("PressDropRqd",    "PDrop", DC_DP,   "kPa",   xidPDropRqd,     this, isParm);
    DDB.Visibility(SHM_All, iPressMethod==BRPM_Fixed);
    DDB.Double  ("OutletPressRqd",  "PRqd", DC_P,     "kPag",  xidPoutRqd,      this, isParm|NAN_OK);
    DDB.Visibility();
    m_VLE.Add_OnOff(DDB);
    DDB.Text    ("");
    DDB.Text    ("Results");
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureIn",   "Ti",   DC_T,     "C",     xidTempIn,       this, isResult|noFile|noSnap);
    DDB.Double  ("TemperatureOut",  "To",   DC_T,     "C",     xidTempOut,      this, isResult|noFile|noSnap);
    DDB.Double  ("PressIn",         "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult|isParmConstruct);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("PressOut",        "Po",   DC_P,     "kPag",  xidPOut,         this, isResult|isParmConstruct);
    DDB.Double  ("VapourFracIn",    "Vfi",  DC_Frac,  "%",     xidVapFracIn,    this, isResult|noFile|noSnap);
    DDB.Double  ("VapourFracOut",   "Vfo",  DC_Frac,  "%",     xidVapFracOut,   this, isResult|noFile|noSnap);
    //DDB.Double  ("SatT",            "",     DC_T,     "C",     &dSatTOut,       this, isResult|noFile|noSnap);
    //DDB.Double  ("SatP",            "",     DC_P,     "kPag",  &dSatPOut,       this, isResult|noFile|noSnap|InitHidden);
    //DDB.Double  ("H2O_Frac",        "",     DC_Frac,  "%",     &dH20Frac,       this, isResult|noFile|noSnap|InitHidden);*/
    if (0)
      {
      DDB.Text    ("");
      DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap|InitHidden);
      DDB.Double  ("Vap_Frac",        "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap|InitHidden);
      }
    DDB.Double  ("PrevPOut",        "",     DC_P,     "kPag",  &dPout,          this, isParm|noView);
    DDB.Double  ("PrevDuty",        "",     DC_Pwr,   "kW",    &dDuty,          this, isParm|noView);
    }
  else
    {
    DDB.Text    ("");
    DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|0);
    DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap);
    DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap);
    //DDB.Double  ("",                "QmPrvPB",DC_Qm,  "kg/s",  xidFEQmPrvPB,    this, isResult|InitHidden);
    ////DDB.Double  ("",                "QmPB", DC_Qm,    "kg/s",  xidFEQmMemPB,    this, isResult|InitHidden);
    DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap);
    //DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap);
    BuildDataDefnElevation(DDB);
    DDB.Text    ("");
    //DDB.Double  ("VapourFrac",      "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap);
    //DDB.Double  ("mVapourFrac",     "mVf",  DC_Frac,  "%",     xidMoleVapFrac,  this, isResult|noFile|noSnap);
    //DDB.Text   ("Pressure Audit", Marker|MarkerClosed);
    DDB.Double  ("Press_InEst",     "PiEst",DC_P,     "kPag",  xidPInEst,       this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("Press_Input",     "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult);
    DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Press_Output",    "Po",   DC_P,     "kPag",  xidPOut,         this, isResult);
    DDB.Double  ("Press_OutEst",    "PoEst",DC_P,     "kPag",  xidPOutEst,      this, isResult|isParmConstruct|InitHidden);
    DDB.Double  ("",                "dPbX", DC_DP,    "kPa",   xidFEDPbX,       this, isResult|isParmConstruct|InitHidden);
    }

  DDB.Text   ("");
  BuildDataDefnShowIOs(DDB);

  DDB.Visibility();

  if (ProbalMode())
    {
    m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
    }
  else
    {
    DDB.Page   ("Parms");
    //DDB.Text   ("");
    //DDB.Double ("TempDrop",     "",       DC_dT,   "C",     &dTempDrop,    this, isParm|NAN_OK);
    DDB.Text   ("");
    //VLE.Add_OnOff(DDB);

    m_FEP.BuildDataDefn(FBDDOpt_WithEqn, DDB, this, NULL, -1, DDB_RqdPage);
    if (NoFlwIOs()>0 && !DDB.ForFileSnpScn() && (!DDB.ForView() || dbgfile()))
      IOFB(0,0)->BuildDataDefn(FBDDOpt_WithData, DDB, this, "In", 1);

    //DDB.Text("");
    //VPB.BuildDataDefn(DDB, this, "Posn", 1);

    //VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

    //if (NoFlwIOs()>0)
    //  DDB.Object(IOConduit(0), this);
    }

  DDB.Visibility();
  DDB.Text("");
  m_FTB.BuildDataDefn(DDB, "Thermal", NULL, DDB_RqdPage, FEOF_WithPVConst);

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag Reducer::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_BstRes::DataXchg(DCB))
    return 1;
  //if (VPB.DataXchg(DCB))
  //  return 1;
  if (m_FEP.DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag Reducer::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_BstRes::ValidateData(VDB);
  if (!m_FEP.ValidateData(VDB))
    OK=False;
  return OK;
  }

//==========================================================================
