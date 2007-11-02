//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#include "scdver.h"
#define  __PIPE_CPP
#include "pipe.h"
#include "models.h"

#define dbgSetZs         0

//==========================================================================
//
//
//
//==========================================================================
/*
#i<Qualities> : These are variables which are distinquished
by their prefix #i<_Qual>. These qualities, have two states
either "Present", in which case the quality is included in the
simulation of the unit or "-" in which case the quality is ignored.
*/

//==========================================================================
/*#D:#T:Pipe_GP
#X:#h<General Description>#nThe general purpose pipe connects any two process
units together. It is always connected from an outlet point on one unit to an
inlet point of another. It is commonly used to transport liquids, slurries and
gases. The pipe only has one inlet and one outlet point.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<LnkMode> : Select the link mode. This determines how material is transfered
between units. #u<Pipe> mode allows by-directional flow with pressure drops etc.
#u<Transfer> mode only allows material transfer in the forward direction, it
'instantaneously' delivers all material from one unit to the next.#n
#i<Direction> : Select the direction materials are allowed to flow in the pipe.
This is meaningless if the pipe is in transfer link mode. #u<Default> allows
material to flow in any direction depending on external considerations.
#u<Forward> allows material to only flow in the positive (forward) direction
from link source to link destination. #u<Reverse> allows material to only flow
in the negative (reverse) direction from link destination to link source.#n
#i<Qm_Reqd> : The mass flow #b<required> in the pipe. Usually enter * to indicate
a required value is not being set.#n
#i<VLEquilibrium> : This can be used to switch #x<Vapour Liquid Equilibrium Logic>
on. If it is on then the associated variables (VLE) must be configured.#n
#i<Reactions> : This can be used to switch reactions on. If it is on then the associated
#x<reaction block> variables (m_RB) must be configured.#n
#i<dZ_Rqd> : The required difference in elevation between the inlet and
outlet of the pipe.#n
#i<SrcHFrc> : The height as a fraction at which the base of the pipe physically
connects to the source unit. This may be meaningless when the pipe is connected
to a unit that as no volume or surge capacity.#n
#i<DstHFrc> : The height as a fraction at which the base of the pipe physically
connects to the destination unit. This may be meaningless when the pipe is connected
to a unit that as no volume or surge capacity.#n
#i<Flow.On> : This behaves as a simple valve. If #u<On> then material can flow
in the pipe, if #u<Off> then the pipe is effectively blocked and material will not
be allowed to flow.#n
#i<Eqn.Name> : This allows you to select the model from the #x<pipe flow equations>
which calculates the pressure drop, due to the flow of materials, across the pipe. Each
equation has its own specific parameters which need to be configured.#n
#i<Qi.Model> : This allows the selection of the model that is to be used to calculate
how the species are combined, this is normally done using a mass weighted mean.#n
#i<Qi.View> : This allows the selection of how the list of species is being displayed.
Options such as MassFlow, MoleFlow, VolFlow, MassFrac, ElemntFlow, CompntConc etc can
be selected.#n
#i<Qi.Phase> : This allows the selection of which phase(s) for the list of species is
being displayed. Options such as All, Solids, Liquids and Vapours can be selected.#n
#i<Specie Lockup> : This causes a percentage of the species, to be retained within the
unit. The amount of retention is dependent upon the type of unit.#n
#i<Size Distribution> : This associates a particle size distribution curve with each specie.#n
#n#n
#h<Associated variables>#n
#i<QmEst> : The estimated mass flow within the pipe. This will only differ from "Qm" if the
upstream unit cannot supply sufficient material.#n
#i<Qm> : The mass flow of the material through the pipe.#n
#i<Qv> : The volumetric flowrate of the material within the pipe.#n
#i<NQv> : The normalized volumetric flowrate, ie the flowrate measured at STP, of the material
within the pipe. This will only differ from "Qv" when pressurized gases are being conveyed,
since the volume of a gas is proportional to its pressure.#n
#i<Vel> : The velocity of the flow through the pipe.  This will only be calculated if the flow
equation chosen by the user requires a pipe diameter to be specified.#n
#i<T> : The temperature of the material within the pipe.#n
#i<Rho> : The density of the material within the pipe.#n
#i<dPb> : The extra pressure required to deliver the specified flowrate if there is insufficient
pressure at the source of the pipe.#n
#i<dPq> : The pressure drop in the pipe caused by the flow of material within the pipe.#n
#i<dPz> : The static head ie the pressure associated with the difference in elevation between
the inlet and outlet of the pipe.#n
#i<Bst> : The extra pressure (boost) required to deliver the specified flowrate if there is
insufficient pressure at the source of the pipe.#n
#i<PiR> : The remote pressure at the inlet of the pipe ie the pressure measured at the flange.#n
#i<Pi> : The pressure at the inlet of the pipe ie the pressure measured after the flange.#n
#i<dP> : The total pressure drop across the pipe.#n
#i<Po> : The pressure at the outlet of the pipe, ie the pressure measured before the flange.#n
#i<PoR> : The remote pressure at the outlet of the pipe, ie the pressure measured at the flange.#n
#i<dZ> : The difference in elevation between the inlet and outlet of the pipe.#n
#i<L> : The length of the pipe.#n
#i<SrcHgt> : The actual height of the base of the inlet of the pipe, in relation to the common datum.#n
#i<DstHgt> : The actual height of the base of the outlet of the pipe, in relation to the common datum.#n
#i<SrcIO> : The tag identifying the source (upstream) unit, followed by  "." and then the
name of the output point of the unit, to which the input of the pipe is connected.#n
#i<DstIO> : The tag identifying the destination (downstream) unit, followed by  "." and then
the name of the inlet point of the unit, to which the output of the pipe is connected.#n
#i<QMt> : The total mass flow of all the species within the pipe.#n
#i<QVt> : The total volumetric flow of all the species within the pipe.#n
#i<MoleWt> : The molecular weight of the material within the pipe.#n
#i<CpCv> : ???#n
#i<Sf> : The percentage of solids within the pipe.#n
#i<Lf> : The percentage of liquids within the pipe.#n
#i<Vf> : The percentage of vapours within the pipe.#n
#i<SatT@P> : The temperature at which the liquids within the pipe will boil at atmospheric pressure.
This is mainly of interest for elevated boiling point calculations.#n
#i<SatP@T> : The saturated pressure of the liquids within the pipe.#n
#i<BPE> : The Boiling Point Elevation of the liquids within the pipe.#n
#i<Cp> : The specific heat of the material within the pipe.#n
#i<totH> : The total enthalpy of the amterial within the pipe.#n
#n#n
#n#h<Other>#n
Default model prefix:P#n
Short name:Pipe#n
Model type:Link
#G:Links
*/

//==========================================================================
//==========================================================================

IMPLEMENT_MODELLINK(Pipe, "Pipe", "1", "P", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                   "Process:Piping:Pipe_GP",
                   "General purpose pipe")

Pipe::Pipe(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Lnk(pClass_, TagIn, pAttach, eAttach)
  {
  m_FEP.AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
  m_FEP.SetOptions(0, 1.0);
  AttachIOAreas(TwoIOAreaListWithMBS, &PipeGroup, &PipeGroup);

//  EHX.Open(&CEnvironHXClass);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(Pipe), 0, mmio_CONNECT, NULL);
  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(Pipe), 1, mmio_CONNECT, NULL);

  };

//--------------------------------------------------------------------------

Pipe::~Pipe()
  {
  };

//--------------------------------------------------------------------------

void Pipe::BuildDataDefn(DataDefnBlk &DDB)
  {
  //const flag IsXfer=((Joins.GetSize()>0) && Joins[0].InXferNet()) || XferFlwMode();
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  if (!NetProbalMethod() && PrjFileVerNo()<73)
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Text   ("");
    static DDBValueLst DDB0a[]={
      {LFM_Xfer,    "Transfer"},
      {LFM_Full,    "Pipe"},
      //{LFM_SrcSnk,  "SrcSnk", MDD_Hidden},
      {0}};
    static DDBValueLst DDB0b[]={
      {LFM_Xfer,    "Transfer"},
      {LFM_Full,    "Pipe", MDD_Hidden},
      //{LFM_SrcSnk,  "SrcSnk", MDD_Hidden},
      {0}};
    if (SolveInlineMethod())
      DDB.Long ("LnkMode",       "",     DC_,     "",      xidLnkMode, this, 0/*isParmStopped|SetOnChange*/, DDB0b);
    else
      DDB.Long ("LnkMode",       "",     DC_,     "",      xidLnkMode, this, isParmStopped|SetOnChange, DDB0a);
    }

  const flag IsXfer=(!NetProbalMethod() && (((Joins.GetSize()>0) && Joins[0].InXferNet()) || XferFlowMode()));
  const flag IsPipe=!IsXfer;
  const flag IsSrcSnk=!IsXfer && GetActiveHold();

  DDB.Visibility();
  DDB.Text   ("");

  DDB.Text   ("Results: Flow Conditions");
  DDB.Double("",              "QmPrvPB",  DC_Qm,   "kg/s",   xidFEQmPrvPB,     this, isResult|noView|NAN_OK);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.String("",              "QmEst.Type",DC_,   "",     xidQmEstType,     this, isResult|InitHidden);
  DDB.String("",              "QmEst.Src", DC_,   "",     xidQmEstSrc,      this, isResult|isTag|InitHidden);
  DDB.Double("EstMassFlow",   "QmEst",    DC_Qm, "kg/s",  xidQmEst,         this, NetDynamicMethod() ? isResult : isResult|InitHidden);

  DDB.Visibility();
  DDB.Double ("Mass_Flow",    "Qm",   DC_Qm,   "kg/s",   xidQm,         this, isResult|isParmConstruct);
  DDB.Double ("Vol_Flow",     "Qv",   DC_Qv,   "L/s",    xidQv,         this, isResult|noFile|noSnap);
  DDB.Double ("NVol_Flow",    "NQv",  DC_NQv,  "NL/s",   xidNQv,        this, isResult|noFile|noSnap);
  DDB.Double ("TemperatureIn","Ti",   DC_T,    "C",      xidTempIn,     this, isResult|noFile|noSnap);
  DDB.Double ("TemperatureOut","To",  DC_T,    "C",      xidTempOut,    this, isResult|noFile|noSnap);
  DDB.Double ("Temperature",  "T",    DC_T,    "C",      xidTemp,       this, isResult|/*noView|*/noFile|noSnap|InitHidden);
  DDB.Double ("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,        this, isResult|noFile|noSnap);
  DDB.Double ("NDensity",     "NRho", DC_Rho,  "kg/m^3", xidNRho,       this, isResult|noFile|noSnap);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer);
  if (NetDynamicMethod())
    DDB.Double("Velocity",      "Vel",       DC_Ldt,  "m/s",    xidVelocity,   this, isResult|noFile|noSnap);

  //if (1 && (NoFlwIOs() > 1) && IOFlange(1))
  //  {
  //  DDB.Double ("ChkVel",       "",         DC_Ldt,  "m/s",   xidChokeVelocity, this, isResult|InitHidden|noFile|noSnap);
  //  }

  if (NetProbalMethod())
    {
    DDB.Double ("Press_Input",  "Pi",       DC_P,   "kPag",  xidPIn,        this, isResult);
    DDB.Double ("Press_Output", "Po",       DC_P,   "kPag",  xidPOut,       this, isResult);
    }
  else
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile());
    DDB.Text   (" ");
    if (NoFlwIOs()>0 && Nd_Rmt(0) && (IOPipeEntry_Rmt(0) || IOPipeJoin_Rmt(0)))
      DDB.Double ("Flange_Press_In", "PiR", DC_P,   "kPag",  xidPInRmt,     this, isResult|noFile|noSnap);
    DDB.Double ("Press_InEst" , "PiEst",    DC_P,   "kPag",  xidPInEst     ,this, isResult|isParmConstruct|InitHidden);
    DDB.Double ("Press_Input",  "Pi",       DC_P,   "kPag",  xidPIn,        this, isResult);
    DDB.Double ("Boost_Head",   "dPb",      DC_DP,  "kPa",   xidFEDPb,      this, isResult|InitHidden|isParmConstruct);
    DDB.TagComment("+");
    DDB.Double ("",             "dPbX",     DC_DP,  "kPa",   xidFEDPbX,     this, isResult|InitHidden|isParmConstruct);
    DDB.TagComment("+");
    DDB.Double ("Dynamic_Head", "dPq",      DC_DP,  "kPa",   xidFEDPq,      this, isResult|InitHidden|isParmConstruct);
    DDB.TagComment("+");
    DDB.Double ("Static_Head",  "dPz",      DC_DP,  "kPa",   xidFEDPz,      this, isResult|InitHidden|isParmConstruct);
    DDB.TagComment("+");
    DDB.Double ("Press_Output", "Po",       DC_P,   "kPag",  xidPOut,       this, isResult);
    DDB.TagComment("=");
    DDB.Double ("Press_OutEst", "PoEst",    DC_P,   "kPag",  xidPOutEst    ,this, isResult|isParmConstruct|InitHidden);
    if (NoFlwIOs()>1 && Nd_Rmt(1) && (IOPipeEntry_Rmt(1) || IOPipeJoin_Rmt(1)))
      DDB.Double ("Flange_Press_Out","PoR", DC_P,   "kPag",  xidPOutRmt,    this, isResult|noFile|noSnap);

    DDB.Text   (" ");
    if (NoFlwIOs()>0 && Nd_Rmt(0) && (IOPipeEntry_Rmt(0) || IOPipeJoin_Rmt(0)))
      DDB.Double ("Flange_DP_In", "DPiR",   DC_DP,  "kPa",   xidDPInRmt,     this, isResult|noFile|noSnap);
    DDB.Double   ("Press_Change", "dP",     DC_DP,  "kPa",   xidDPRmt,       this, isResult|noFile|noSnap);
    if (NoFlwIOs()>1 && Nd_Rmt(1) && (IOPipeEntry_Rmt(1) || IOPipeJoin_Rmt(1)))
      DDB.Double ("Flange_DP_Out","DPoR",   DC_DP,  "kPa",   xidDPOutRmt,    this, isResult|noFile|noSnap);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Double ("Ave_Mass_Flow","QmAv",     DC_Qm,  "kg/s",  xidIO_QmAvail, this, isResult|0|InitHidden);
    DDB.Double ("",             "QmSp",     DC_Qm,  "kg/s",  xidIO_QmSpace, this, isResult|0|InitHidden);

    //DDB.Text   (" ");
    //if (NoFlwIOs()>0 && Nd_Rmt(0) && IOPipeEntry_Rmt(0))
    //  DDB.Double ("Flange_DP_In", "DPiR",   DC_DP,  "kPa",   xidDPInRmt,     this, isResult|noFile|noSnap);
    //DDB.Double   ("Press_Change", "dP",     DC_DP,  "kPa",   xidDPRmt,       this, isResult|noFile|noSnap);
    //if (NoFlwIOs()>1 && IOPipeEntry_Rmt(1))
    //  DDB.Double ("Flange_DP_Out","DPoR",   DC_DP,  "kPa",   xidDPOutRmt,    this, isResult|noFile|noSnap);
    }

  if (!NetProbalMethod() && !HeatSkipMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Text   (" ");
    if (NoFlwIOs()>0 && Nd_Rmt(0) && (IOPipeEntry_Rmt(0) || IOPipeJoin_Rmt(0)))
      DDB.Double ("", "Hz@TiR", DC_HMs,  "kJ/kg",  xidHzInRmt,   this, isResult|noFile|noSnap);
    DDB.Double   ("", "Hz@Ti",  DC_HMs,  "kJ/kg",  xidHzIn,      this, isResult|noFile|noSnap);
    DDB.Double   ("", "Hz@To",  DC_HMs,  "kJ/kg",  xidHzOut,     this, isResult|noFile|noSnap);
    if (NoFlwIOs()>1 && Nd_Rmt(1) && (IOPipeEntry_Rmt(1) || IOPipeJoin_Rmt(1)))
      DDB.Double ("", "Hz@ToR", DC_HMs,  "kJ/kg",  xidHzOutRmt,  this, isResult|noFile|noSnap);

    DDB.Text   (" ");
    if (NoFlwIOs()>0 && Nd_Rmt(0) && (IOPipeEntry_Rmt(0) || IOPipeJoin_Rmt(0)))
      DDB.Double ("", "totHz@TiR", DC_Pwr, "kJ/s",  xidtotHzInRmt,   this, isResult|noFile|noSnap);
    DDB.Double   ("", "totHz@Ti",  DC_Pwr, "kJ/s",  xidtotHzIn,      this, isResult|noFile|noSnap);
    DDB.Double   ("", "totHz@To",  DC_Pwr, "kJ/s",  xidtotHzOut,     this, isResult|noFile|noSnap);
    if (NoFlwIOs()>1 && Nd_Rmt(1) && (IOPipeEntry_Rmt(1) || IOPipeJoin_Rmt(1)))
      DDB.Double ("", "totHz@ToR", DC_Pwr, "kJ/s",  xidtotHzOutRmt,  this, isResult|noFile|noSnap);
    }

  DDB.Visibility();
  DDB.Text   ("Connections", Marker);
  DDB.String ("Link_Source",      "SrcIO",       DC_,  "",     xidSrcIOTag,      this, isTag|noFile|noSnap);
  DDB.String ("Link_Destination", "DstIO",       DC_,  "",     xidDstIOTag,      this, isTag|noFile|noSnap);
  DDB.String ("",                 "SrcFlange",   DC_,  "",     xidSrcFlangeTag,  this, isTag|noFile|noSnap);
  DDB.String ("",                 "DstFlange",   DC_,  "",     xidDstFlangeTag,  this, isTag|noFile|noSnap);

  DDB.Visibility(NSHM_All);
  //Strng Page;
  //Page.Set("%s..", Tag());
  //DDB.Page(Page(), NetProbalMethod() ? DDB_OptPage : DDB_RqdPage);
  if (NetProbalMethod())
    {
    //DDB.Page("..", DDB_OptPage);
    DDB.Text  (StdSepUnderbar);
    }
  else
    {
    DDB.Page("..", DDB_RqdPage);
    }

  DDB.Text   ("Requirements");
  cioOn.BuildDataDefn(this, "Flow", DDB);

  if (!NetProbalMethod())
    {
    DDB.Text   ("");
    if (IsXfer || !DDB.ForFileSnpScn() || PrjFileVerNo()<40)
      {
      DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsSrcSnk && IsXfer);
      m_FEP.BuildDataDefn(FBDDOpt_WithQmSpec, DDB, this, "Flw", 3, DDB_NoPage);
      }

    static DDBValueLst DDBXfCapOpt[]={
      {XCO_Spill,   "Spill"   },
      {XCO_Accept,  "Accept"  },
      {0}};
    DDB.Byte  ("OverCapacity", "",     DC_,     "",      &m_iXferCapOption, this, isParm|SetOnChange, DDBXfCapOpt);
    }

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  if (SolveBufferedMethod())// PipeFlowMode())
    {
    DDB.Text("Flow Options");
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsSrcSnk && (!IsXfer/* || dbgfile()*/));
    static DDBValueLst DDBPhasesR[]=
      {
        {FPh_Liquid,   "Liquids"   },
        {FPh_Gasses,   "Gasses"   },
        {FPh_Both,     "Both"     },
        {0}
      };
    static DDBValueLst DDBPhasesA[]=
      {
        {FPh_Liquid,   "Liquids"   },
        {FPh_Gasses,   "Gasses"   },
        {FPh_Both,     "Both"     },
        {FPh_UnKnown,  "Unknown"  },
        {FPh_None,     "None"         },
        {0}
      };
    DDB.Byte   ("NetFlow.Allow",      "",             DC_,     "",      xidFlwPhaseRqd,  this, isParm|SetOnChange, DDBPhasesR);
    DDB.Byte   ("NetFlow.Actual",     "",             DC_,     "",      xidFlwPhaseAct,  this, 0, DDBPhasesA);
    Strng S;
    if (CFlwBlkData::sm_iFlwPh_Force)
      S="Frc";
    if (CFlwBlkData::sm_iFlwPh_Block)
      S+=S.GetLength()>0?".Blk":"Blk";
    if (S.GetLength())
      DDB.TagComment(S());

    static DDBValueLst DDB1[]={
      {FBPS_Default,    "Default"},
      {FBPS_FwdOnly,    "Forward"},
      {FBPS_RevOnly,    "Reverse"},
      {FBPS_FwdOnly|FBPS_Inherit,    "(Forward)"},
      {FBPS_RevOnly|FBPS_Inherit,    "(Reverse)"},
      {FBPS_Default|FBPS_Inherit,    "(Default)"},
      {0}};
    DDB.Byte("PropSelect",   "",       DC_,     "",      xidPropertySelect, this, isParm, DDB1);

//    DDBValueLst DDBFill[]={
//      {LFM_None,   "None"},
//      {LFM_Simple, "Simple"},
//      {0}};
//    DDB.Byte       ("FillMode",   "",  DC_,     "",      &iFillMode,  this, isParm, DDBFill);
#if WITHMULTIFLWBLK
    DDB.CheckBoxBtn("MultiFlwBlk",  "",  DC_, "",  xidMFBOn,     this, isParm);//|SetOnChange);
#endif
#if WITHTWOPHASE
    DDB.CheckBoxBtn("TwoPhaseFlw",  "",  DC_, "",  &fTwoPhase,   this, isParm);//|SetOnChange);
#endif

    static DDBValueLst DDBVFX[]={
      {VFFX_Off,      "Off"},
      {VFFX_PreMix,   "PreMix"},
      {VFFX_PostMix,  "PostMix"},
      {VFFX_Plugflow, "PlugFlow"},
      {0}};
    static DDBValueLst DDBPFT[]={
      {PFT_Incompressible,  "Incompressible"  },
      {PFT_Compressible,    "Compressible"    },
      //{PFT_Mixed,           "Mixed"           },
      {0}};

    if (PrjFileVerNo()>=59)
      {
      DDB.Byte     ("VolFlwFX",         "",   DC_,  "",  xidVolFlwFX,       this, isParmStopped|SetOnChange, DDBVFX);
      if (m_PFI.Method == VFFX_Plugflow)
        {
        DDB.Byte   ("PlugFlowType",     "",   DC_,  "",  xidPlugFlowType,   this, isParmStopped|SetOnChange, DDBPFT);
        if (m_PFI.Type != PFT_Incompressible)
          {
          DDB.Long ("NoSections",       "",   DC_,  "",  xidPlugFlowSects,  this, isParmStopped);
          }
        }
      }
    else
      DDB.CheckBoxBtn("Vol_FlwFX",      "",   DC_,  "",  xidVolFlwFX,       this, isParm, DDBVFX);

    DDB.CheckBoxBtn("Vol_PresFX",       "",   DC_,  "",  xidVolPrsFX,       this, isParm|InitHidden);
    DDB.CheckBoxBtn("VolIsSmall",       "",   DC_,  "",  &fVolIsSmall,      this, isParm|InitHidden);
    DDB.CheckBoxBtn("VapLocked",        "",   DC_,  "",  &fVapLocked,       this, isParm|InitHidden);
    DDB.CheckBox   ("IsDegFreedom",     "",   DC_,  "",  &fIsDegFree,       this, isParm|SetOnChange);
    DDB.CheckBox   ("ApplyRhoH",        "",   DC_,  "",  &fAppRhoH,         this, isParm|SetOnChange);
    DDB.CheckBox   ("FindEquilRhoH",    "",   DC_,  "",  &fFindRhoH,        this, isParm|SetOnChange);
    DDB.CheckBoxBtn("OrificePlate",     "",   DC_,  "",  xidWithOP,         this, isParmStopped|SetOnChange);
    }

  DDB.Visibility();
  m_BlkEval.Add_OnOff(DDB);  
  
  if (!NetProbalMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.CheckBoxBtn("CollectStats",  "",  DC_, "",  xidSI_On,   this, isParmStopped);//|SetOnChange);
    //TODO: Check: is iLF_Direction only used in dynamic!?!
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);//SM_Direct|HM_All);
    DDB.Text("Flow Configuration");
    static DDBValueLst DDB1[]={
      {LFD_Default, "Default"},
      {LFD_Forward, "Forward"},
      {LFD_Reverse, "Reverse"},
      {0}};
    DDB.Byte  ("Direction",   "",       DC_,     "",      &m_iLF_Direction, this, isParm, DDB1);
    }

  if (NetProbalMethod())
    {
    DDB.Visibility(NM_Probal|SM_All|HM_All);
    DDB.Text   ("MakeUp");
    //DDB.Double ("Qm_MakeUp.Dst",   "", DC_Qm, "kg/s", xidIO_MkUpAvail1, this, isResult|InitHidden|NAN_OK);
    DDB.Double ("Qm_MakeUp.Dst",   "", DC_Qm, "kg/s", xidIO_MkUpReqd1,  this, isResult|NAN_OK);
    DDB.Double ("Qm_MakeUp",       "", DC_Qm, "kg/s",    &m_dMkUpRqd,   this, isParm|NAN_OK);

    DDB.Double ("Qm_MakeUp.Src",   "", DC_Qm, "kg/s", xidIO_MkUpReqd0,  this, isResult|NAN_OK);
    DDB.Double ("Qm_Avail.Src",    "", DC_Qm, "kg/s", xidIO_MkUpAvail0, this, isResult|InitHidden|NAN_OK);
    DDB.Text   ("");
    }

  DDB.Visibility();
  if (DDB.BeginStruct(this, "Tear", NULL, DDB_NoPage, -1, MarkerClosed))
    {
    static DDBValueLst DDBTearPri[]=
      {
        {TP_First,  "First"},
        {TP_Normal, "Normal"},
        {TP_Last,   "Last"},
        {0}
      };
    static DDBValueLst DDBTearTypeRqd[]=
      {
        {TT_NoTear,     "NoTear"},
        //{TT_SystemTear, "SystemTear"}, // Cannot Select this
        {TT_ManualTear, "ManualTear"},
        {TT_Break,      "Break"},
        {0}
      };
    static DDBValueLst DDBTearState[]=
      {
        {TT_NoTear,     "NoTear"},
        {TT_SystemTear, "SystemTear"},
        {TT_ManualTear, "ManualTear"},
        {TT_Break,      "Break"},
        {0}
      };
    static DDBValueLst DDBHow[]=
      {
        {TIH_ZeroNIters,  "Zero"                       },
        {TIH_HoldNIters,  "Hold"                       },
        {TIH_RampNIters,  "Ramp"                       },
        {0}
      };
    static DDBValueLst DDBWhen[]=
      {
        {TIW_OnInit,      "OnInitialise", MDD_Default  },
        {TIW_OnStart,     "OnStart"                    },
        //{TIW_Always,      "Always"                     },
        {TIW_Manual,      "OnDemand"                 },
        {0}
      };
    static DDBValueLst DDBInitQmMode[] =
      {
      //{ SPI_QModeNone, "None" },
        { SPI_QModeQm,   "Mass" },
        { SPI_QModeQv,   "Volume" },
        { SPI_QModeNQv,  "NVolume" },
      };
    static DDBValueLst DDBInitEstUsage[]=
      {
      //{TIEU_None,     "None"},
        {TIEU_FullEst,  "Full"},
        {TIEU_PartEst,  "Partial"},
        {0},
      };
    static DDBValueLst DDBInitState[]=                             
      {                                                            
        {TIH_Off,        "Off"                      },                       
        {TIH_ZeroNIters, "Zeroing"                  },                       
        {TIH_HoldNIters, "Holding"                  },                       
        {TIH_RampNIters, "Ramping"                  },                       

        //{TIH_ZeroNIters, "WillZero"                 },                       
        //{TIH_HoldNIters, "WillHold"                 },                       
        //{TIH_RampNIters, "WillRamp"                 },                       
        //{TIH_Zeroing,    "Zeroing"                  },
        //{TIH_Holding,    "Holding"                  },
        //{TIH_Ramping,    "Ramping"                  },
        {0},
      };

    DDEF_Flags B4TearVis=DDB.GetVisibility();
    DDB.Byte        ("Priority",      "",   DC_,    "",       xidTearPriority,     this, isParm|SetOnChange, DDBTearPri);
    DDB.Byte        ("RqdType",       "",   DC_,    "",       xidTearType,         this, isParm|SetOnChange, DDBTearTypeRqd);
    DDB.Byte        ("State",         "",   DC_,    "",       xidTearState,        this, 0, DDBTearState);
    DDB.String      ("TearFlange",    "",   DC_,    "",       xidTearFlangeTag,    this, 0|isTag);
    DDB.String      ("TearBlock",     "",   DC_,    "",       xidTearBlockTag,     this, 0|isTag);

    bool TearVis=(NoFlwIOs()>1 && IOFlange(0)->RqdTearType()>=TT_ManualTear);
    DDB.Visibility(NSHM_All, TearVis);

    if (PrjFileVerNo()>=78)
      {
      if (DDB.BeginStruct(this, "Init", NULL, DDB_NoPage))
        {
        DDB.Byte        ("How",      "",   DC_,    "",       xidTearInitHow,      this, isParmStopped|SetOnChange, DDBHow);
        DDB.Byte        ("When",     "",   DC_,    "",       xidTearInitWhen,     this, isParmStopped|SetOnChange, DDBWhen);
        static DDBValueLst DDBStart[] = {{0, "Start"}, {1, "Busy"}, {0}};
        DDB.Button      ("Start",   "",   DC_,    "",       xidTearInitActivate, this, isParm|SetOnChange);//, DDBStart);
        bool HoldCntVis=true;
        DDB.Visibility(NSHM_All, TearVis && HoldCntVis);
        DDB.Byte        ("Count",    "",   DC_,    "",       xidTearCntRqd,       this, isParmStopped);
        DDB.Byte        ("Iters",    "",   DC_,    "",       xidTearCntAct,       this, InitHidden);
        DDB.Visibility(NSHM_All, TearVis);

        DDB.Byte        ("State",    "",   DC_,    "",       xidTearInitActive,   this, 0, DDBInitState);

        SpImgMode Md=SPI_QModeNone;
        DDBValueLstMem DDBInitQmModeX;
        //DDBInitQmModeX.Add(SPI_QModeNone, "None");
        DDBInitQmModeX.Add(SPI_QModeQm,   "Mass");

        if (NoProcessIOs()>0 && IOFlange(0) && IOFlange(0)->TearImage())
          {
          Md=IOFlange(0)->TearImage()->QMode();

          SpImage &Img=*IOFlange(0)->TearImage();
          if (Img.SpVarsAvail() & VAMsk_SpVol)
            DDBInitQmModeX.Add(SPI_QModeQv,   "Volume");
          if (Img.SpVarsAvail() & VAMsk_SpNVol)
            DDBInitQmModeX.Add(SPI_QModeNQv,  "NVolume");
          }

        DDB.Visibility(NSHM_All, TearVis);
        DDB.Double      ("T_Rqd",         "",   DC_T,   "C",      xidTearTRqd,         this, isParm       |NAN_OK, DDBNAN_NotSpecd);
        DDB.Double      ("P_Rqd",         "",   DC_P,   "kPag",   xidTearPRqd,         this, isParm       |NAN_OK, DDBNAN_NotSpecd);
        DDB.Byte        ("Estimate.Usage","",   DC_,    "",       xidTearInitEstUsage, this, isParmStopped|SetOnChange, DDBInitEstUsage);
        DDB.Byte        ("Flow.Mode",     "",   DC_,    "",       xidTearQmMode,       this, isParmStopped|SetOnChange, DDBInitQmModeX());
        DDB.Visibility(NSHM_All, TearVis && (Md!=SPI_QModeNone));
        DDB.Double      ("Qm_Rqd",        "",   DC_Qm,  "kg/s",   xidTearQmRqd,        this, (Md==SPI_QModeQm?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
        DDB.Double      ("Qv_Rqd",        "",   DC_Qv,  "L/s",    xidTearQvRqd,        this, (Md==SPI_QModeQv?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
        DDB.Double      ("NQv_Rqd",       "",   DC_NQv, "NL/s",   xidTearNQvRqd,       this, (Md==SPI_QModeNQv?isParm:isResult)|NAN_OK, DDBNAN_NotSpecd);
        }
      DDB.EndStruct();
      }
    else
      {
      DDB.Byte        ("Init.QmMode",   "",   DC_,    "",       xidTearQmMode,       this, isParm|SetOnChange, DDBInitQmMode);
      DDB.Byte        ("Init.How",      "",   DC_,    "",       xidTearInitHow,      this, isParm|SetOnChange, DDBHow);
      DDB.Byte        ("Init.When",     "",   DC_,    "",       xidTearInitWhen,     this, isParm|SetOnChange, DDBWhen);
      DDB.Byte        ("Init.Count",    "",   DC_,    "",       xidTearCntRqd,       this, isParm);
      DDB.Byte        ("Init.Iters",    "",   DC_,    "",       xidTearCntAct,       this, InitHidden);
      DDB.CheckBoxBtn ("Init.Active",   "",   DC_,    "",       xidTearInitActive,   this, isParm|SetOnChange);
      DDB.Double      ("Qm_Rqd",        "",   DC_Qm,  "kg/s",   xidTearQmRqd,        this, isParm|NAN_OK);
      DDB.Double      ("Qv_Rqd",        "",   DC_Qv,  "L/s",    xidTearQvRqd,        this, isParm|NAN_OK);
      DDB.Double      ("NQv_Rqd",       "",   DC_NQv, "NL/s",   xidTearNQvRqd,       this, isParm|NAN_OK);
      DDB.Double      ("P_Rqd",         "",   DC_P,   "kPag",   xidTearPRqd,         this, isParm|NAN_OK);
      DDB.Double      ("T_Rqd",         "",   DC_T,   "C",      xidTearTRqd,         this, isParm|NAN_OK);
      }

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.CheckBoxBtn ("AdjQmToEstQm",  "",   DC_,    "",       &m_bAdjustToEstFlow,  this, isParm|SetOnChange);
    DDB.SetVisibility(B4TearVis);
    }
  DDB.EndStruct();

  if (IsPipe)//NetDynamicMethod() && IsSrcSnk)
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, NetDynamicMethod() && IsSrcSnk);
    if (DDB.BeginStruct(this, "SrcSnk", NULL, DDB_NoPage, -1, 0))
      {
      DDB.Double("P_Rqd",     "", DC_P,     "kPag", &SrcSnk.dPress,     this, isParm);
      DDB.Double("T_Rqd",     "", DC_T,     "C",    &SrcSnk.dTemp,      this, isParm);
#if WITHSRCSNKDIFFS
      DDB.Double("QmDiff",    "", DC_Frac,  "%",    &SrcSnk.dQmDiff,    this, 0);
      DDB.Double("TempDiff",  "", DC_Frac,  "%",    &SrcSnk.dTempDiff,  this, 0);
#endif
      DDB.CheckBoxBtn("KeepImage", "", DC_, "",     xidKeepSrcSnk,      this, isParm|InitHidden);
      DDB.Button("CopyCurrent",    "", DC_, "",     xidCopyCurrent,     this, isParm);
      }
    DDB.EndStruct();
    }

#if WITHPBDPSTUFF
  if (NetProbalMethod())
    {
    //DDB.Text("");
    static DDBValueLst DDB2[]={
      {LDP_Fixed_Drop,    "Fixed_Drop"},
      {LDP_Fixed_Boost,   "Fixed_Boost"},
      {LDP_Darcy_DP,      "Darcy_DP"},
      //{LDP_Linear_DP,     "Linear_DP"},
      //{LDP_SquareLaw_DP,  "SquareLaw_DP"},
      {LDP_Const_P,       "Const_Src_P"},
      //{LDP_BRC_DP,        "BRC_DP"},  ////Bernoulli Rouse Corrected
      {0}};
    DDB.Visibility(NM_Probal|SM_All|HM_All);
    DDB.Text("Pressure Drop Requirements");
    DDB.Byte  ("Press_Mode",	  "",	DC_,	"",	   &iDP_Mode,	  this,	isParm|AffectsStruct|SetOnChange, DDB2);

    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Const_P);
    DDB.Double("Lnk_P_Src",	  "",	DC_P,	"kPag",	 &LnkP_S,	    this,	isParm);

    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Fixed_Drop || iDP_Mode==LDP_Fixed_Boost);
    DDB.Double("Fxd_dP",	  "",	DC_DP,	"kPa",	 &FxddP,	    this,	isParm);

    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Darcy_DP || iDP_Mode==LDP_BRC_DP);
    DDB.Double("Diam",      "",	DC_L, 	"mm", 	 &PipeD,		  this,	isParm);
    DDB.Double("ScaleBuildup", "", DC_L, "mm",   &PipeScale,  this,	isParm);
    DDB.Double("PipeDeltaZ", "", DC_L, "m",      &PipeDeltaZ, this,	isParm);
    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Darcy_DP);
    static DDBValueLst DDB1P[]={
      {(int)True,  "Calc_K"},
      {(int)False, "Define_K" },
      {0}};
    DDB.Bool  ("K_Method",  "", DC_,    "",      &iCalc_K,    this, isParm|SetOnChange, DDB1P);
    DDB.Visibility(NM_Probal|SM_All|HM_All, (iDP_Mode==LDP_Darcy_DP && iCalc_K) || iDP_Mode==LDP_BRC_DP);
    if (SolveDirectMethod())
      DDB.Double("Length",    "L", DC_L, 	"m",   &PipeL,		  this,	isParm);
    DDB.Double("FittingsLength", "FitL", DC_L, "m", &FitPipeL, this,	isParm);
    DDB.Visibility(NM_Probal|SM_All|HM_All, (iDP_Mode==LDP_Darcy_DP && iCalc_K));
    static DDBValueLst DDB2P[]={
      {(int)True,  "Calc_f"},
      {(int)False, "Define_f" },
      {0}};
    DDB.Bool  ("f_Method",  "", DC_,    "",      &iCalc_FFac, this, isParm, DDB2P);
    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Darcy_DP && iCalc_K && iCalc_FFac);
    DDB.Double("Viscosity", "",	DC_Visc,"cP",    &Visc,	  	  this,	isParm);
    DDB.Double("Pipe_Rough","",	DC_L,   "mm",    &Rough,  	  this,	isParm);
    DDB.Double("ReynoldsNo", "Re", DC_, "",      &Re,         this, isResult);
    DDB.Visibility(NM_Probal|SM_All|HM_All, (iDP_Mode==LDP_Darcy_DP && iCalc_K) || iDP_Mode==LDP_BRC_DP);
    DDB.Double("FricFac",  "",	DC_,    "",      &FricFac,    this,	(iCalc_FFac && iDP_Mode!=LDP_BRC_DP) ? isResult : isParm);
    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Darcy_DP);
    DDB.Double("K_Value",   "",	DC_,  	"",   	 &K,    		  this,	iCalc_K ? isResult : isParm);
    DDB.Double("K_MinorLosses", "",	DC_, "",   	 &KMinorLoss, this,	isParm);
    DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Darcy_DP || iDP_Mode==LDP_BRC_DP);
    DDB.Double("Velocity", "Vel", DC_Ldt, "m/s", &Vel,      this, isResult);

    //DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Linear_DP || iDP_Mode==LDP_SquareLaw_DP);
    //DDB.Double("S_MassFlw",	"",	DC_Qm,	"kg/s",	 &SQmCap,	  this,	isParm);
    //DDB.Double("S_dP",	    "",	DC_DP,	"kPa",	 &SQmdP,		this,	isParm);
    //DDB.Double("V_MassFlw",	"",	DC_Qm,	"kg/s",	 &VQmCap,	  this,	isParm);
    //DDB.Double("V_dP",	    "",	DC_DP,	"kPa",	 &VQmdP,		this,	isParm);

    DDB.Visibility(NM_Probal|SM_All|HM_All);
    DDB.Double("Pdrop",	    "",	DC_DP,	"kPa",	 &PB_dp,		  this,	isResult);
    }
#endif

  if (IsXfer)
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsSrcSnk && (IsXfer || dbgfile()));
    }

#ifdef OldCode
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, /*HasFullFlow() &&*/ !IsXfer);// || dbgfile());
  DDB.Text   ("Rise/Length", Marker);
  if (NetDynamicMethod())
    DDB.Double ("PipeLength",   "",       DC_L, "m",  xidFELength,    this, isResult|0|isParmConstruct);
  DDB.Double ("SrcH",           "SrcHgt", DC_L, "m",  xidSrcHgt,      this, isResult|noFile|noSnap);
  DDB.Double ("DstH",           "DstHgt", DC_L, "m",  xidDstHgt,      this, isResult|noFile|noSnap);
  if (PrjFileVerNo()>=100)
    DDB.Double ("Rise",         "",       DC_L, "m",  xidFEDZ,        this, isResult|0|isParmConstruct);
  else
    DDB.Double ("Rise",         "dZ",     DC_L, "m",  xidFEDZ,        this, isResult|0|isParmConstruct);
  if (PrjFileVerNo()<100 && DDB.ForFileSnpScn())
    DDB.Double ("",             "dZ_Rqd", DC_L, "m",  &m_RiseRqd,     this, isParmStopped|NAN_OK);
  else
    {
    DDB.Double ("RiseRqd",      "",       DC_L, "m",  &m_RiseRqd,     this, isParmStopped|NAN_OK);
    DDB.Double ("DatumDiff",    "",       DC_L, "m",  &m_DatumChg,    this, isParmStopped|NAN_OK);
    }
  xx
#else
  if (NetDynamicMethod())
    {
    DWORD LSOpts=PrjFileVerNo()<38 ? 0 : noFile|noSnap;
    dword Flgs0=NoFlwIOs()>=1 ? IODesc_Rmt(0)->m_dwFlags : 0;
    dword Flgs1=NoFlwIOs()>=2 ? IODesc_Rmt(1)->m_dwFlags : 0;

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);// || dbgfile());
    DDB.Text   ("End Points", Marker);
    DDB.Double ("SrcHF",        "SrcHFrc", DC_Frac, "%",     xidSrcHFrc,     this,   ((Flgs0&IOChgFracHgt)?isParmStopped:0)|LSOpts);
    DDB.Double ("DstHF",        "DstHFrc", DC_Frac, "%",     xidDstHFrc,     this,   ((Flgs1&IOChgFracHgt)?isParmStopped:0)|LSOpts);
    DDB.Double ("SrcAperture",  "",        DC_Frac, "%",     xidSrcAperture,  this, (Flgs0&IOChgFracHgt&&SolveInlineMethod_Rmt(0)?0:noView)|LSOpts);
    DDB.Double ("DstAperture",  "",        DC_Frac, "%",     xidDstAperture,  this, (Flgs1&IOChgFracHgt&&SolveInlineMethod_Rmt(1)?0:noView)|LSOpts);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All, HasModeNear(-1, LFM_Full) && !IsXfer);// || dbgfile());
    DDB.Text   ("Rise/Length", Marker);
    DDB.Double ("SrcH",           "SrcHgt", DC_L, "m",  xidSrcHgt,      this, isResult|noFile|noSnap);
    DDB.Double ("DstH",           "DstHgt", DC_L, "m",  xidDstHgt,      this, isResult|noFile|noSnap);
    DDB.Double ("PipeLength",     "",       DC_L, "m",  xidFELength,    this, isResult|0|isParmConstruct);
    //DDB.Double ("SrcHF",        "SrcHFrc", DC_Frac, "%",     xidSrcHFrc,     this,   ((Flgs0&IOChgFracHgt)?isParmStopped:0)|LSOpts);
    //DDB.Double ("DstHF",        "DstHFrc", DC_Frac, "%",     xidDstHFrc,     this,   ((Flgs1&IOChgFracHgt)?isParmStopped:0)|LSOpts);
    //DDB.Double ("SrcAperture",  "",        DC_Frac, "%",     xidSrcAperture,  this, (Flgs0&IOChgFracHgt&&SolveInlineMethod_Rmt(0)?0:noView)|LSOpts);
    //DDB.Double ("DstAperture",  "",        DC_Frac, "%",     xidDstAperture,  this, (Flgs1&IOChgFracHgt&&SolveInlineMethod_Rmt(1)?0:noView)|LSOpts);
    if (PrjFileVerNo()>=100)
      DDB.Double ("Rise",         "",       DC_L, "m",  xidFEDZ,        this, isResult|0|isParmConstruct);
    else
      DDB.Double ("Rise",         "dZ",     DC_L, "m",  xidFEDZ,        this, isResult|0|isParmConstruct);
    if (PrjFileVerNo()<100 && DDB.ForFileSnpScn())
      DDB.Double ("",             "dZ_Rqd", DC_L, "m",  &m_RiseRqd,     this, isParmStopped|NAN_OK);
    else
      {
      DDB.Double ("RiseRqd",      "",       DC_L, "m",  &m_RiseRqd,     this, isParmStopped|NAN_OK);
      DDB.Double ("DatumDiff",    "",       DC_L, "m",  &m_DatumChg,    this, isParmStopped|NAN_OK);
      }
    }
#endif

  DDB.Visibility();
  if (!NetProbalMethod())
    {
    //DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Text("");
    DDB.CheckBox   ("FwdOnly","",         DC_, "",  &m_fFwdOnly,   this, isParm|SetOnChange);
    }

  if (IsPipe)
    {
    DDB.Text("");
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_fFwdOnly);
    DDB.Double ("UDFOpenP",       "",     DC_P, "kPa", xidUDFOpenP   ,this, isParm);
    DDB.Long   ("UDFActionCount", "",     DC_,  "",    xidUDFActCnt  ,this, 0|InitHidden);
    static DDBValueLst DDBUDF[]={
      {0,    ""},
      {1,   "Closed"},
      {0}};
    DDB.Bool   ("UDFState",       "",     DC_,  "",    xidUDFState   ,this, 0, DDBUDF);
    DDB.Visibility(NSHM_All);
    }
  if (pStats && NetDynamicMethod())
    {
    pStats->BuildDataDefn(DDB, this);
    }

  DDB.Visibility(NSHM_All);

//  DDB.CheckBox("MultiFlwBlk",   "",        DC_,  "",      &MFB.fOn,  this, isParm|SetOnChange);
// ------------
  //if (IsPipe)
  if (IsPipe && !NetProbalMethod()) //kga 23/11/04 do we need all these tags in ProBal???
    {
    if (m_MFB.On())
      {
      DDB.Object(&m_MFB, this, "MFB", NULL, DDB_OptPage);
      }
    else
      {
      DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile());
      DDB.Page("Eqns", DDB_OptPage);

      //DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile()); //kga//hss 14/2/98
      DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile()); //kga//hss 14/2/98
      int Do0=(NoFlwIOs()>0) && Nd_Rmt(0);
      int Do1=(NoFlwIOs()>1) && Nd_Rmt(1);
      DDBPages Pg=NetProbalMethod() ? DDB_OptPage : DDB_RqdPage;
      if (!DDB.ForFileSnpScn())
        {
        //if (Do0 && IOPipeEntry_Rmt(0))
        if (Do0 && (IOPipeEntry_Rmt(0) || IOPipeJoin_Rmt(0)))
          {
          IOFB_Rmt(0,0)->BuildDataDefn(FBDDOpt_WithEqn/*All*/, DDB, this, "Src", 1, Pg);
          Pg=DDB_NoPage;
          }
        }
      //if (!DDB.ForFileSnpScn() || PrjFileVerNo()<40)
      //m_FEP.BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_WithDPSpec|(!IsXfer ? FBDDOpt_WithQmSpec:0), DDB, this, "Eqn", 3, Pg);
      m_FEP.BuildDataDefn(FBDDOpt_WithEqn|FBDDOpt_WithDPSpec|FBDDOpt_WithQmSpec, DDB, this, "Eqn", 3, Pg);
      if (m_bWithOP && NIOFBs(0)==2)
        IOFB(0,1)->BuildDataDefn(FBDDOpt_WithEqn, DDB, this, "OP", 4, DDB_NoPage);
      if (!DDB.ForFileSnpScn())
        {
        if (Do1 && (IOPipeEntry_Rmt(1) || IOPipeJoin_Rmt(1)))
          IOFB_Rmt(1,0)->BuildDataDefn(FBDDOpt_WithEqn, DDB, this, "Dst", 2, DDB_NoPage);
        }
      }
    }

  DDB.Visibility();
  m_BlkEval.BuildDataDefn(DDB);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, (NetDynamicMethod() && IsSrcSnk) || DDB.ForFileSnpScn());
  if (IsPipe)
    {
    if (SrcSnk.pImage)
      DDB.Object(SrcSnk.pImage, this, "FlowsRqd", NULL, DDB_RqdPage);
  #if WITHSRCSNKDIFFS
    if (SrcSnk.pSpDiff)
      DDB.Object(SrcSnk.pSpDiff, this, "FlowsDiff", NULL, DDB_RqdPage);
  #endif
    }
  DDB.Visibility();

  if (m_PFI.Method==VFFX_Plugflow)
    {
    DDB.Page("PlugFlow", DDB_RqdPage);
    DDB.Long("SamplePtCount", "", DC_, "", xidSamplePtCount, this, isParmStopped|SetOnChange, "The Number of points at which Conditions or composition can be measured");
    if (DDB.BeginArray(this, "SamplePos", "PFSamplePos", GetSamplePtCount()))
      {
      for (int i=0; i<GetSamplePtCount(); i++)
        {
        DDB.BeginElement(this, i, NULL);
        DDB.Double("Position",  "RPos", DC_Frac, "%",   &m_SamplePt[i].m_dFracPos, this, isParmStopped, NULL, "The Position Relative to the 'Start' of the Pipe");
        DDB.Double("Distance",  "APos", DC_L,    "m",   &m_SamplePt[i].m_dAbsPos,  this, 0, NULL, "The Distance from the 'Start' of the Pipe");
        DDB.Double("Velocity",  "Vel",  DC_Ldt,  "m/s", &m_SamplePt[i].m_dVel,     this, 0, NULL, "The Velocity at this Position");
        }
      }
    DDB.EndArray();
    }

  if (IsPipe && (NoFlwIOs()>=1) && IOFlange(0) &&
      ((IOFlange(0)->RqdTearType()>=TT_ManualTear) || (IOFlange(0)->TearImageExists())))
    {
    SpImage &Img=*IOFlange(0)->TearImage();
    DDB.Visibility(NSHM_All, IOFlange(0)->RqdTearType()>=TT_ManualTear && 
                            IOFlange(0)->TearInitEstUsage()>TIEU_None);
    static const BDDSpecies_Blk sbImageM[]= { {SVV_AsMassFlow,    SVV_AsMassFrac,  true,  true, VAMsk_SpMass },
                                              {SVV_AsMassFrac,    SVV_AsMassFrac,  true,  true, VAMsk_SpMass }, };
    static const BDDSpecies_Blk sbImageV[]= { {SVV_AsVolFlow,     SVV_AsMassFrac,  true,  true, VAMsk_SpVol  },
                                              {SVV_AsMassFrac,    SVV_AsMassFrac,  true,  true, VAMsk_SpMass }, };
    static const BDDSpecies_Blk sbImageN[]= { {SVV_AsNVolFlow,    SVV_AsMassFrac,  true,  true, VAMsk_SpNVol },
                                              {SVV_AsMassFrac,    SVV_AsMassFrac,  true,  true, VAMsk_SpMass }, };

    SpImgMode Md=SPI_QModeNone;
    if (IOFlange(0)->TearImage())
      Md=Img.QMode();
    const BDDSpecies_Blk *pBlk=Img.pViewBlk;
    int BlkSize=Img.iViewBlkSize;
    switch (Md)
      {
      case SPI_QModeQv : 
        Img.pViewBlk=sbImageV;
        Img.iViewBlkSize=sizeof(sbImageV);
        break;
      case SPI_QModeNQv : 
        Img.pViewBlk=sbImageN;
        Img.iViewBlkSize=sizeof(sbImageN);
        break;
      default: 
        Img.pViewBlk=sbImageM;
        Img.iViewBlkSize=sizeof(sbImageM);
        break;
      }
    Img.SetTag("QEst");
    DDB.Object(IOFlange(0)->TearImage(), this, "QEst", Nd_Rmt(0)->FullObjTag(), DDB_RqdPage);
    Img.pViewBlk=pBlk;
    Img.iViewBlkSize=BlkSize;
    DDB.Visibility();
    }

  if ((NoFlwIOs() > 1) && IOFlange(0))
    {
    //if (IOConduitIn(0))
    //  {
    //  DDB.Visibility(NSHM_All);
    //  IOConduitIn(0)->SetTag("QiI");
    //  DDB.Object(IOConduitIn(0), this, "QiI", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
    //  DDB.Visibility(NSHM_All);
    //  }
    IOConduit(0)->SetTag("Qi");
    DDB.Object(IOConduit(0), this, "Qi", Nd_Rmt(0)->FullObjTag(), DDB_RqdPage);
    }

  if (m_PFI.CnExists)
    DDB.Object(&m_PFI.Cn, this, NULL/*PrjFileVerNo()<47?"Content":"Cn"*/, NULL, DDB_RqdPage, 100);
  //if (m_PFI.PfExists)
  //  DDB.Object(&m_PFI.Pf, this, NULL/*PrjFileVerNo()<47?"Content":"Cn"*/, NULL, DDB_RqdPage, 200);

  if (m_PFI.Method==VFFX_Plugflow)
    {
    if (DDB.BeginArray(this, "SamplePt", "PFSamplePt", GetSamplePtCount()))
      {
      for (int i=0; i<GetSamplePtCount(); i++)
        {
        DDB.BeginElement(this, i, NULL);//, lPlugFlowCdUI+i);
        //m_SamplePt[i].m_dCnd->Tag("QoI");
        DDB.Object(m_SamplePt[i].m_pCnd, this, "Sp", NULL, DDB_RqdPage);
        }
      }
    DDB.EndArray();
    }

  if (NoFlwIOs() >= 2)
    {
    IOConduit(1)->SetTag("Qo");
    DDB.Object(IOConduit(1), this, "Qo", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
    //if (IOConduitIn(1))
    //  {
    //  DDB.Visibility(NSHM_All);
    //  IOConduitIn(1)->SetTag("QoI");
    //  DDB.Object(IOConduitIn(1), this, "QoI", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
    //  DDB.Visibility(NSHM_All);
    //  }
    }

  DDB.Visibility();
  DDB.Text("");
  m_FTB.BuildDataDefn(DDB, "Thermal", NULL, DDB_RqdPage, FEOF_WithAutoComp|FEOF_WithPVConst);

  //BuildDataDefnAreaIOs(DDB);

  if (DDB.ForFileSnpScn() && NetDynamicMethod())
    {
    if (!m_pLastFlw)
      m_pLastFlw=new SpConduit("LastFlw", this, TOA_Embedded);
    DDB.Object(m_pLastFlw, this, "LastFlw", NULL, DDB_OptPage, 250);
    }
  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag Pipe::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==3 && m_FEP.DataXchg(DCB))
    return true;
  if (NoFlwIOs()>=2 && DCB.dwUserInfo==4 && IOFB(0,1)->DataXchg(DCB))
    return true;
  if (NoFlwIOs()>=1 && DCB.dwUserInfo==1 && IOFB_Rmt(0,0)->DataXchg(DCB))
    return 1;
  if (NoFlwIOs()>=2 && DCB.dwUserInfo==2 && IOFB_Rmt(1,0)->DataXchg(DCB))
    return 1;
  if (MN_Lnk::DataXchg(DCB))
    return 1;
  if (m_BlkEval.DataXchg(DCB))
    return 1;
  if (m_RB.DataXchg(DCB))
    return 1;

  return 0;
  }

// --------------------------------------------------------------------------

flag Pipe::ValidateData(ValidateDataBlk & VDB)
  {
  //if (m_PFI.Method==VFFX_Plugflow)
  //  {
  //  LogNote(FullObjTag(), 0, "Discrete Effects not implemented");
  //  m_PFI.Method=VFFX_PostMix;
  //  }
  flag OK=MN_Lnk::ValidateData(VDB);
	if (!m_RB.ValidateData(VDB))
    OK=False;
  return OK;
  }

//--------------------------------------------------------------------------

void Pipe::PostConnect(int IONo)
  {
  MN_Lnk::PostConnect(IONo);
  }

//---------------------------------------------------------------------------

void Pipe::PreDisConnect(int IONo)
  {
  MN_Lnk::PreDisConnect(IONo);
  }

//==========================================================================
