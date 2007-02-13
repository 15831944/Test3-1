/================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __DUCT_CPP
#include "duct.h"
#include "m_bstres.h"

#define dbgSetZs         0

//#pragma message("NB NB NB optimise off -------------------------------------------------------")
//#pragma optimize("", off)

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
on. If it is on then the associated variables (m_VLE) must be configured.#n
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
#i<Vel> : ???#n
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
#i<MoleWt> : ???#n
#i<CpCv> : ???#n
#i<Sf> : The percentage of solids within the pipe.#n
#i<Lf> : The percentage of liquids within the pipe.#n
#i<Vf> : The percentage of vapours within the pipe.#n
#i<SatT@P> : ???#n
#i<SatP@T> : ???#n
#i<BPE> : ???#n
#i<Cp> : ???#n
#i<totH> : ???#n
#n#n
#n#h<Other>#n
Default model prefix:P#n
Short name:Pipe#n
Model type:Link
#G:Links
*/

//==========================================================================

IMPLEMENT_MODELLINK(Duct, "Duct", "1", "D", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT,
                   "Process:Piping:Duct",
                   "General purpose Duct")

Duct::Duct(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Lnk(pClass_, TagIn, pAttach, eAttach)
  {
  m_FEP.AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
  AttachIOAreas(TwoIOAreaListWithS);
  };

//--------------------------------------------------------------------------

void Duct::BuildDataDefn(DataDefnBlk &DDB)
  {
  flag IsXfer=((Joins.GetSize()>0) && Joins[0].InXferNet()) || XferFlowMode();
  flag IsSrcSnk=!IsXfer && GetActiveHold();

  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);


  DDB.Visibility();
  DDB.Text   ("Flow Conditions");
  DDB.Double("",              "QmPrvPB",   DC_Qm,   "kg/s",   xidFEQmPrvPB,     this, isResult|noView|NAN_OK);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All); //hss 13/2/98
  //DDB.Double("Mass_Flow_In",  "QmIn",      DC_Qm,   "kg/s",   xidQi,            this, isResult|InitHidden|0);
  ////DDB.Double("",              "QmPB",      DC_Qm,   "kg/s",   xidFEQmMemPB,     this, isResult|InitHidden|NAN_OK);
  //DDB.Double("Mass_Flow_Out", "QmOut",     DC_Qm,   "kg/s",   xidQo,            this, isResult|InitHidden|0);
  DDB.String("",              "QmEstSrc",  DC_,     "",       xidQmEstSrc,      this, isResult|0|isTag);
  DDB.Double("EstMassFlow",   "QmEst",     DC_Qm,   "kg/s",   xidQmEst,         this, isResult|0);

  //DDB.Visibility(NM_Probal|SM_All|HM_All);
  //DDB.Double ("Qm_MakeUp",     "",         DC_Qm,   "kg/s",   &dMkUpRqd,        this, isParm|NAN_OK);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Double ("Qm_MakeUp",       "", DC_Qm, "kg/s", &dMkUpRqd,        this, isParm|NAN_OK);
  //DDB.Double ("Qm_MakeUpAvail",  "", DC_Qm, "kg/s", xidIO_MkUpAvail0, this, isResult|InitHidden|NAN_OK);
  //DDB.Double ("Qm_MakeUpReqd",   "", DC_Qm, "kg/s", xidIO_MkUpReqd0,  this, isResult|InitHidden|NAN_OK);
  //DDB.Double ("Qm_MakeUpAvail1", "", DC_Qm, "kg/s", xidIO_MkUpAvail1, this, isResult|InitHidden|noView|NAN_OK);
  //DDB.Double ("Qm_MakeUpReqd1",  "", DC_Qm, "kg/s", xidIO_MkUpReqd1,  this, isResult|InitHidden|noView|NAN_OK);

  DDB.Visibility(NSHM_All);
  DDB.Double("Mass_Flow",     "Qm",        DC_Qm,   "kg/s",   xidQm,         this, isResult|isParmConstruct);
  DDB.Double("Vol_Flow",      "Qv",        DC_Qv,   "L/s",    xidQv,         this, isResult|noFile|noSnap);
  DDB.Double("NVol_Flow",     "NQv",       DC_NQv,  "NL/s",   xidNQv,        this, isResult|noFile|noSnap);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer);
  DDB.Double("Velocity",      "Vel",       DC_Ldt,  "m/s",    xidVelocity,   this, isResult|noFile|noSnap);
  DDB.Visibility(NSHM_All);

  DDB.Double("Temperature",  "T",         DC_T,    "C",      xidTemp,       this, isResult|noFile|noSnap);
  DDB.Double("Density",      "Rho",       DC_Rho,  "kg/m^3", xidRho,        this, isResult|noFile|noSnap);

//  DDB.Visibility(NM_Dynamic|SM_All|HM_All);  //hss 13/2/98
//  DDB.Double ("Fill",         "",         DC_Frac, "%",     &Fill,         this, 0);
//  DDB.Double ("FwdFill",      "",         DC_Frac, "%",     &FwdFill,      this, 0|InitHidden);
//  DDB.Double ("RevFill",      "",         DC_Frac, "%",     &RevFill,      this, 0|InitHidden);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile());
  DDB.Text   ("Pressure Audit", Marker|MarkerClosed);
  DDB.Double ("Boost_Head",   "dPb",      DC_DP,   "kPa",   xidFEDPb,      this, isResult|isParmConstruct);
  DDB.Double ("",             "dPbX",     DC_DP,   "kPa",   xidFEDPbX,     this, isResult|InitHidden|isParmConstruct);
  DDB.Double ("Dynamic_Head", "dPq",      DC_DP,   "kPa",   xidFEDPq,      this, isResult|isParmConstruct);
  DDB.Double ("Static_Head",  "dPz",      DC_DP,   "kPa",   xidFEDPz,      this, isResult|isParmConstruct);
  DDB.Double ("Momentum",     "dPm",      DC_DP,   "kPa",   xidMomentum,   this, isResult|InitHidden|isParmConstruct);
  DDB.Double ("Nett_Boost",   "Bst",      DC_DP,   "kPa",   xidNettBoost,  this, isResult|InitHidden|noFile|noSnap);

  DDB.Text   (" ");

  if (NoFlwIOs()>0 && Nd_Rmt(0) && IOPipeEntry_Rmt(0))
    DDB.Double ("Flange_Press_In", "PiR",      DC_P,    "kPag",  xidPInRmt,     this, isResult|noFile|noSnap);
  DDB.Double ("Press_Input",  "Pi",       DC_P,    "kPag",  xidPIn,        this, isResult|SM_Direct);
  DDB.Double ("Press_Change", "dP",       DC_DP,   "kPa",   xiddP,         this, isResult|noFile|noSnap);
  DDB.Double ("Press_Output", "Po",       DC_P,    "kPag",  xidPOut,       this, isResult|SM_Direct);
  if (NoFlwIOs()>1 && IOPipeEntry_Rmt(1))
    DDB.Double ("Flange_Press_Out","PoR",      DC_P,    "kPag",  xidPOutRmt,    this, isResult|noFile|noSnap);
  DDB.Double ("Ave_Mass_Flow","QmAv",     DC_Qm,   "kg/s",  xidIO_QmAvail, this, isResult|0|InitHidden);
  DDB.Double ("",             "QmSp",     DC_Qm,   "kg/s",  xidIO_QmSpace, this, isResult|0|InitHidden);

  Strng Page;
  Page.Set("%s..", Tag());
  DDB.Page(Page(), DDB_OptPage);

  //DDB.Visibility();
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);  //hss 14/2/98
  if (PrjFileVerNo()<73)
    {
    static DDBValueLst DDB0[]={
      {LFM_Xfer, "Transfer"},
      {LFM_Full, "Pipe"},
      //{LFM_SrcSnk, "SrcSnk", MDD_Hidden},
      {0}};
    DDB.Long  ("LnkMode",   "",       DC_,     "",      xidLnkMode, this, isParm|SetOnChange, DDB0);
    }

  if (NetDynamicMethod() && FullFlowMode())// CompleteDPFlowMode())
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

//    DDBValueLst DDBFill[]={
//      {LFM_None,   "None"},
//      {LFM_Simple, "Simple"},
//      {0}};
//    DDB.Byte       ("FillMode",   "", DC_, "", &iFillMode,  this, isParm, DDBFill);
    static DDBValueLst DDBVFX[]={
      {VFFX_Off,      "Off"},
      {VFFX_PreMix,   "PreMix"},
      {VFFX_PostMix,  "PostMix"},
      {VFFX_Plugflow, "PlugFlow"},
      {0}};
    DDB.Byte       ("VolFlwFX",     "", DC_, "", xidPFMethod,  this, isParm, DDBVFX);
    DDB.CheckBoxBtn("MultiFlwBlk",  "", DC_, "", xidMFBOn,     this, isParm);//|SetOnChange);
    DDB.CheckBoxBtn("TwoPhaseFlw",  "", DC_, "", &fTwoPhase,   this, isParm);//|SetOnChange);
    DDB.CheckBoxBtn("VolPrsFX",     "", DC_, "", &fVolPrsFX,   this, isParm|InitHidden);
    DDB.CheckBoxBtn("VolIsSmall",   "", DC_, "", &fVolIsSmall, this, isParm|InitHidden);
    DDB.CheckBoxBtn("VapLocked",    "", DC_, "", &fVapLocked,  this, isParm|InitHidden, "If set the network solution will assume that no vapour will flow through this pipe");
    DDB.CheckBox   ("IsDegFreedom", "", DC_, "", &fIsDegFree,  this, isParm);
    DDB.CheckBox   ("ApplyRhoH",    "", DC_, "", &fAppRhoH,    this, isParm);
    DDB.CheckBoxBtn("OrificePlate", "", DC_, "", xidWithOP,    this, isParm|SetOnChange);//|SetOnChange);
    }

  DDB.Visibility();
  DDB.Text("Product Options");
  m_RB.Add_OnOff(DDB);
  m_EHX.Add_OnOff(DDB);
  m_VLE.Add_OnOff(DDB);

  DDB.Text("Flow Configuration");
  static DDBValueLst DDB1[]=
    {
      {LFD_Default, "Default"},
      {LFD_Forward, "Forward"},
      {LFD_Reverse, "Reverse"},
      {0}
    };
  DDB.Byte  ("Direction",   "",       DC_,     "",      &m_iLF_Direction, this, isParm, DDB1);

  DDB.Visibility();

  if (DDB.BeginStruct(this, "Tear", NULL, DDB_NoPage, -1, MarkerClosed))
    {
    static DDBValueLst DDB2T[]=
      {
        {TP_First,  "First"},
        {TP_Normal, "Normal"},
        {TP_Last,   "Last"},
        {0}
      };
    static DDBValueLst DDB3T[]=
      {
        {TT_NoTear,     "NoTear"},
        //{TT_SystemTear, "SystemTear"}, // Cannot Select this
        {TT_ManualTear, "ManualTear"},
        {TT_Break,      "Break"},
        {0}
      };
    static DDBValueLst DDB4T[]=
      {
        {TT_NoTear,     "NoTear"},
        {TT_SystemTear, "SystemTear"},
        {TT_ManualTear, "ManualTear"},
        {TT_Break,      "Break"},
        {0}
      };
    static DDBValueLst DDB6T[]=
      {
        {TIH_ZeroNIters,     "Zero"                       },
        {TIH_HoldNIters,     "Hold"                       },
        {TIH_RampNIters,     "Ramp"                       },
        {0}
      };
    static DDBValueLst DDB7T[]=
      {
        {TIW_OnInit,         "OnInitialise", MDD_Default  },
        {TIW_OnStart,        "OnStart"                    },
        {TIW_Always,         "Always"                     },
        {TIW_Manual,         "ManualOnly"                 },
        {0}
      };

    static DDBValueLst DDB8T[]=
      {
        {SPI_QModeNone, "None"},
        {SPI_QModeQm,   "Mass"},
        {SPI_QModeQv,   "Volume"},
        {SPI_QModeNQv,  "NVolume"},
        {0},
      };
    DDEF_Flags B4TearVis=DDB.GetVisibility();
    DDB.Byte        ("Priority",      "",   DC_,    "",       xidTearPriority,     this, isParm|SetOnChange, DDB2T);
    DDB.Byte        ("RqdType",       "",   DC_,    "",       xidTearType,         this, isParm|SetOnChange, DDB3T);
    DDB.Byte        ("State",         "",   DC_,    "",       xidTearState,        this, 0, DDB4T);

    DDB.Visibility(NSHM_All, (NoFlwIOs()>1 && IOFlange(0)->RqdTearType()>=TT_ManualTear));

    if (PrjFileVerNo()>=78)
      {
      if (DDB.BeginStruct(this, "Init", NULL, DDB_NoPage))
        {
        DDB.Byte        ("QmMode",   "",   DC_,    "",       xidTearQmMode,       this, isParm|SetOnChange, DDB8T);
        DDB.Byte        ("How",      "",   DC_,    "",       xidTearInitHow,      this, isParm|SetOnChange, DDB6T);
        DDB.Byte        ("When",     "",   DC_,    "",       xidTearInitWhen,     this, isParm|SetOnChange, DDB7T);
        DDB.Byte        ("Count",    "",   DC_,    "",       xidTearCntRqd,       this, isParm);
        DDB.Byte        ("Iters",    "",   DC_,    "",       xidTearCntAct,       this, InitHidden);
        DDB.CheckBoxBtn ("Active",   "",   DC_,    "",       xidTearInitActive,   this, isParm|SetOnChange);

        DDB.Double      ("Qm_Rqd",   "",   DC_Qm,  "kg/s",   xidTearQmRqd,        this, isParm|NAN_OK);
        DDB.Double      ("Qv_Rqd",   "",   DC_Qv,  "L/s",    xidTearQvRqd,        this, isParm|NAN_OK);
        DDB.Double      ("NQv_Rqd",  "",   DC_NQv, "NL/s",   xidTearNQvRqd,       this, isParm|NAN_OK);
        DDB.Double      ("P_Rqd",    "",   DC_P,   "kPag",   xidTearPRqd,         this, isParm|NAN_OK);
        DDB.Double      ("T_Rqd",    "",   DC_T,   "C",      xidTearTRqd,         this, isParm|NAN_OK);
        }
      DDB.EndStruct();
      }
    else
      {
      DDB.Byte        ("Init.QmMode", "",   DC_,    "",       xidTearQmMode,       this, isParm|SetOnChange, DDB8T);
      DDB.Byte        ("Init.How",    "",   DC_,    "",       xidTearInitHow,      this, isParm|SetOnChange, DDB6T);
      DDB.Byte        ("Init.When",   "",   DC_,    "",       xidTearInitWhen,     this, isParm|SetOnChange, DDB7T);
      DDB.Byte        ("Init.Count",  "",   DC_,    "",       xidTearCntRqd,       this, isParm);
      DDB.Byte        ("Init.Iters",  "",   DC_,    "",       xidTearCntAct,       this, InitHidden);
      DDB.CheckBoxBtn ("Init.Active", "",   DC_,    "",       xidTearInitActive,   this, isParm|SetOnChange);
      DDB.Double      ("Qm_Rqd",      "",   DC_Qm,  "kg/s",   xidTearQmRqd,        this, isParm|NAN_OK);
      DDB.Double      ("Qv_Rqd",      "",   DC_Qv,  "L/s",    xidTearQvRqd,        this, isParm|NAN_OK);
      DDB.Double      ("NQv_Rqd",     "",   DC_NQv, "NL/s",   xidTearNQvRqd,       this, isParm|NAN_OK);
      DDB.Double      ("P_Rqd",       "",   DC_P,   "kPag",   xidTearPRqd,         this, isParm|NAN_OK);
      DDB.Double      ("T_Rqd",       "",   DC_T,   "C",      xidTearTRqd,         this, isParm|NAN_OK);
      }

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.CheckBoxBtn ("AdjQmToEstQm",  "",   DC_,    "",       &m_bAdjustToEstFlow,  this, isParm|SetOnChange);
    DDB.SetVisibility(B4TearVis);
    }
  DDB.EndStruct();

  if (1)//NetDynamicMethod() && IsSrcSnk)
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, NetDynamicMethod() && IsSrcSnk);
    DDB.BeginStruct(this, "SrcSnk", NULL, DDB_NoPage, -1, 0);//MarkerClosed);
    DDB.Double("P_Rqd",     "", DC_P,     "kPag", &SrcSnk.dPress,     this, isParm);
    DDB.Double("T_Rqd",     "", DC_T,     "C",    &SrcSnk.dTemp,      this, isParm);
#if WITHSRCSNKDIFFS
    DDB.Double("QmDiff",    "", DC_Frac,  "%",    &SrcSnk.dQmDiff,    this, 0);
    DDB.Double("TempDiff",  "", DC_Frac,  "%",    &SrcSnk.dTempDiff,  this, 0);
#endif
    DDB.Bool  ("KeepImage", "", DC_,      "",     xidKeepSrcSnk,      this, isParm|InitHidden);
    DDB.Button("CopyCurrent",    "", DC_, "",     xidCopyCurrent,     this, isParm);
    DDB.EndStruct();
    }
//  static DDBValueLst DDB2[]={
//    {LDP_Fixed_Drop,    "Fixed_Drop"},
//    {LDP_Fixed_Boost,   "Fixed_Boost"},
//    {LDP_Darcy_DP,      "Darcy_DP"},
//    //{LDP_Linear_DP,     "Linear_DP"},  // hss 14/2/98
//    //{LDP_SquareLaw_DP,  "SquareLaw_DP"},
//    {LDP_Const_P,       "Const_Src_P"},
//    {0}};
//  DDB.Byte  ("Press_Mode",    "", DC_,  "", &iDP_Mode,    this, isParm | AffectsStruct, DDB2);
//
//  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Const_P);
//  DDB.Double("Lnk_P_Src",   "", DC_P, "kPag", &LnkP_S,  this, isParm);
//
//  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Fixed_Drop || iDP_Mode==LDP_Fixed_Boost);
//  DDB.Double("Fxd_dP",    "", DC_DP,  "kPa",  &FxddP,   this, isParm);
//
//  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Darcy_DP);
//  DDB.Double("Diam",      "", DC_L,   "mm",    &PipeD,    this, isParm);
//  static DDBValueLst DDB1P[]={
//    {(int)True,  "Calc_K"},
//    {(int)False, "Define_K" },
//    {0}};
//  DDB.Bool  ("K_Method",  "", DC_,    "",      &iCalc_K,   this, isParm, DDB1P);
//  if (!iCalc_K)
//    DDB.Double("K_Value", "", DC_,    "",      &K,        this, isParm);
//  else
//    {
//    DDB.Double("Length",    "", DC_L,   "m",     &PipeL,    this, isParm);
//    DDB.Double("Viscosity", "", DC_Visc,"Ns/m^2",&Visc,     this, isParm);
//    DDB.Double("Pipe_Rough","", DC_L,   "mm",    &Rough,    this, isParm);
//    }
//
//  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Linear_DP || iDP_Mode==LDP_SquareLaw_DP);
//  DDB.Double("S_MassFlw", "", DC_Qm,  "kg/s", &SQmCap,  this, isParm);
//  DDB.Double("S_dP",      "", DC_DP,  "kPa",  &SQmdP,   this, isParm);
//  DDB.Double("V_MassFlw", "", DC_Qm,  "kg/s", &VQmCap,  this, isParm);
//  DDB.Double("V_dP",      "", DC_DP,  "kPa",  &VQmdP,   this, isParm);

  if (OrigPrjFileVerNo()<83)
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsSrcSnk && (!IsXfer/* || dbgfile()*/));
    DDB.Double ("",               "Qm_Rqd",       DC_Qm,   "kg/s",  xidOldQmReqd,           this, isParm|NAN_OK, DDBNAN_NotSpecd);
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsSrcSnk && (IsXfer/* || dbgfile()*/));
    DDB.Double ("Qm_RqdCapacity", "Qm_Capacity",  DC_Qm,   "kg/s",  xidOldQmRqdCapacity,    this, isParm|NAN_OK, DDBNAN_NotSpecd);
    DDB.Double ("Qm_MinCapacity", "Qm_MinCap",    DC_Qm,   "kg/s",  xidOldQmMinCapacity,    this, isParm);
    DDB.Double ("Qm_MaxCapacity", "Qm_MaxCap",    DC_Qm,   "kg/s",  xidOldQmMaxCapacity,    this, isParm|NAN_OK, DDBNAN_NotSpecd);
    }

  //DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  //DDB.Visibility(NSHM_All); hss 14/2/98

  //DDB.Visibility(NM_Probal|SM_All|HM_All);
  //DDB.Double ("Qm_MakeUp",     "",      DC_Qm,      "kg/s",   &dMkUpRqd,   this, isParm|NAN_OK);//|AffectsStruct);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile());
  DDB.Text   ("Rise/Length", Marker);
  DDB.Double ("Rise",         "dZ",      DC_L, "m",     xidFEDZ,        this, isResult|0|isParmConstruct);
  DDB.Double ("Length",       "L",       DC_L, "m",     xidFELength,    this, isResult|0|isParmConstruct);
  DDB.Double ("SrcH",         "SrcHgt",  DC_L, "m",     xidSrcHgt,      this, isResult|noFile|noSnap);
  DDB.Double ("DstH",         "DstHgt",  DC_L, "m",     xidDstHgt,      this, isResult|noFile|noSnap);
  if (PrjFileVerNo()<100 && DDB.ForFileSnpScn())
    DDB.Double ("",             "dZ_Rqd",  DC_L, "m",   &m_RiseRqd,     this, isParmStopped|NAN_OK);
  else
    {
    DDB.Double ("RiseRqd",    "",        DC_L, "m",     &m_RiseRqd,     this, isParmStopped|NAN_OK);
    DDB.Double ("DatumDiff",  "",        DC_L, "m",     &m_DatumChg,    this, isParmStopped|NAN_OK);
    }

  DDB.Visibility();
  DDB.Text   ("Connections", Marker);
  DDB.String ("Link_Source",      "SrcIO",   DC_,  "",     xidSrcIOTag,    this, isTag|noFile|noSnap);
  DDB.String ("Link_Destination", "DstIO",   DC_,  "",     xidDstIOTag,    this, isTag|noFile|noSnap);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Double ("SrcHF",        "SrcHFrc", DC_Frac, "%",     xidSrcHFrc,     this, isParmStopped);
  DDB.Double ("DstHF",        "DstHFrc", DC_Frac, "%",     xidDstHFrc,     this, isParmStopped);


  DDB.Visibility(NSHM_All);
  cioOn.BuildDataDefn(this, "Flow", DDB);

//  DDB.CheckBox("MultiFlwBlk",   "",        DC_,  "",      &m_MFB.fOn,  this, isParm|SetOnChange);
// ------------
  if (m_MFB.On())
    {
    DDB.Object(&m_MFB, this, "m_MFB", NULL, DDB_OptPage);
    }
  else if (PrjFileVerNo()<22)// if (!m_MFB.fOn)
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile()); //kga// hss 14/2/98
    DDB.Page("Eqn", DDB_OptPage);
    m_FEP.BuildDataDefn(FBDDOpt_WithAll, DDB, this, NULL, 3);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile()); //kga//hss 14/2/98
    if (NoFlwIOs()>0 && IOPipeEntry_Rmt(0))
      IOFB_Rmt(0,0)->BuildDataDefn(FBDDOpt_WithAll, DDB, this, "SrcEqn", 1);
    if (NoFlwIOs()>1 && IOPipeEntry_Rmt(1))
      IOFB_Rmt(1,0)->BuildDataDefn(FBDDOpt_WithAll, DDB, this, "DstEqn", 2);
    }
  else
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile()); //kga// hss 14/2/98
    DDB.Page("Eqns", DDB_OptPage);


    DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer || dbgfile());
    int Do0=(NoFlwIOs()>0) && Nd_Rmt(0);
    int Do1=(NoFlwIOs()>1) && Nd_Rmt(1);
    DDBPages Pg=DDB_RqdPage;
    if (!DDB.ForFileSnpScn())
      {
      if (Do0 && IOPipeEntry_Rmt(0))
        {
        IOFB_Rmt(0,0)->BuildDataDefn(FBDDOpt_WithEqn/*All*/, DDB, this, "Src", 1, Pg);
        Pg=DDB_NoPage;
        }
      }
    if (!DDB.ForFileSnpScn() || PrjFileVerNo()<40)
      m_FEP.BuildDataDefn(FBDDOpt_WithEqn, DDB, this, "Eqn", 3, Pg);
    if (m_bWithOP && NIOFBs(0)==2)
      IOFB(0,1)->BuildDataDefn(FBDDOpt_WithEqn, DDB, this, "OP", 4, DDB_NoPage);
    if (!DDB.ForFileSnpScn())
      {
      if (Do1 && IOPipeEntry_Rmt(1))
        IOFB_Rmt(1,0)->BuildDataDefn(FBDDOpt_WithEqn, DDB, this, "Dst", 2, DDB_NoPage);
      }
    }

  // ------------
  DDB.Visibility(); //hss 14/2/98

  m_RB.BuildDataDefn(DDB);
  m_EHX.BuildDataDefn(DDB);
  m_VLE.BuildDataDefn(DDB);
           
//  if (Qc)
//    DDB.Object(Qc, this, "Qc", NULL, DDB_RqdPage);
  if (m_PFI.CnExists)
    DDB.Object(&m_PFI.Cn, this, "Content", NULL, DDB_RqdPage);
  if (m_PFI.PfExists)
    DDB.Object(&m_PFI.Pf, this, "Queue", NULL, DDB_RqdPage);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, (NetDynamicMethod() && IsSrcSnk) || DDB.ForFileSnpScn());
  if (SrcSnk.pImage)
    DDB.Object(SrcSnk.pImage, this, "FlowsRqd", NULL, DDB_RqdPage);
#if WITHSRCSNKDIFFS
  if (SrcSnk.pSpDiff)
    DDB.Object(SrcSnk.pSpDiff, this, "FlowsDiff", NULL, DDB_RqdPage);
#endif
  DDB.Visibility();

  if ((NoFlwIOs() > 1) && IOFlange(0))
    {
    if (IOConduitIn(0))
      {
      DDB.Visibility(NSHM_All, True);
      IOConduitIn(0)->Tag("QiI");
      DDB.Object(IOConduitIn(0), this, "QiI", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
      DDB.Visibility(NSHM_All);
      }
    IOConduit(0)->Tag("Qi");
    DDB.Object(IOConduit(0), this, "Qi", Nd_Rmt(0)->FullObjTag(), DDB_RqdPage);
    }
  if (NoFlwIOs() >= 2)
    {
    IOConduit(1)->Tag("Qo");
    DDB.Object(IOConduit(1), this, "Qo", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
    if (IOConduitIn(1))
      {
      DDB.Visibility(NSHM_All, True);
      IOConduitIn(1)->Tag("QoI");
      DDB.Object(IOConduitIn(1), this, "QoI", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
      DDB.Visibility(NSHM_All);
      }
    }

  if ((NoFlwIOs()>=1) && IOFlange(0) && IOFlange(0)->TearImageExists())
    {
    DDB.Visibility(NSHM_All, IOFlange(0)->RqdTearType()>=TT_ManualTear);
    IOFlange(0)->TearImage()->Tag("QEst");
    DDB.Object(IOFlange(0)->TearImage(), this, "QEst", Nd_Rmt(0)->FullObjTag(), DDB_RqdPage);
    DDB.Visibility();
    }

  DDB.Visibility();
  DDB.Text("");
  m_FTB.BuildDataDefn(DDB, "Thermal", NULL, DDB_RqdPage, FEOF_WithAutoComp|FEOF_WithPVConst);

  BuildDataDefnAreaIOs(DDB);

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag Duct::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo==3 && m_FEP.DataXchg(DCB))
    return true;
  if (NoFlwIOs()>=2 && DCB.dwUserInfo==4 && IOFB(0,1)->DataXchg(DCB))
    return true;
  if (NoFlwIOs()>=1 && DCB.dwUserInfo==1 && IOFB_Rmt(0,0)->DataXchg(DCB))
    return 1;
  if (NoFlwIOs()>=2 && DCB.dwUserInfo==2 && IOFB_Rmt(1,0)->DataXchg(DCB))
    return 1;
  //if (m_SpillNoFlwIOs()>=2 && DCB.dwUserInfo==2 && IOFB_Rmt(1,0)->DataXchg(DCB))
  //  return 1;
  if (MN_Lnk::DataXchg(DCB))
    return 1;
  if (m_RB.DataXchg(DCB))
    return 1;
 
  return 0;
  }

// --------------------------------------------------------------------------

flag Duct::ValidateData(ValidateDataBlk & VDB)
  {
  //if (m_PFI.m_bHow==VFFX_Plugflow)
  //  {
  //  LogNote(FullObjTag(), 0, "Discrete Effects not implemented");
  //  m_PFI.m_bHow=VFFX_PostMix;
  //  }
  flag OK=MN_Lnk::ValidateData(VDB);
  if (!m_RB.ValidateData(VDB))
    OK=False;
  return OK;
  }

//--------------------------------------------------------------------------

void Duct::PostConnect(int IONo)
  {
  MN_Lnk::PostConnect(IONo);
  };

//---------------------------------------------------------------------------

void Duct::PreDisConnect(int IONo)
  {
  MN_Lnk::PreDisConnect(IONo);
  }

//--------------------------------------------------------------------------
/*
void Duct::BuildDataDefn(DataDefnBlk &DDB)
  {
  flag IsXfer=((Joins.GetSize()>0) && Joins[0].InXferNet()) || (iLF_Mode==LFM_Xfer);
  //int i;
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  static DDBValueLst DDB0[]={
    {LFM_Xfer, "Transfer"},
    {LFM_CompleteDP, "Pipe"},
    {0}};
  DDB.Byte  ("LnkMode",   "",       DC_    , "",      xidLnkMode, this, isParm|SetOnChange, DDB0);

  DDB.Visibility();
  DDB.Text("Flow Configuration");
  static DDBValueLst DDB1[]={
    {LFD_Default, "Default"},
    {LFD_Forward, "Forward"},
    {LFD_Reverse, "Reverse"},
    {0}};
  DDB.Byte  ("Direction",   "",       DC_    , "",      &iLF_Direction, this, isParm, DDB1);

  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Text("");
  static DDBValueLst DDB2[]={
    {LDP_Fixed_Drop   , "Fixed_Drop"},
    {LDP_Fixed_Boost  , "Fixed_Boost"},
    {LDP_Linear_DP    , "Linear_DP"},
    {LDP_SquareLaw_DP , "SquareLaw_DP"},
    {LDP_Const_P      , "Const_Src_P"},
    {0}};
  DDB.Byte  ("Press_Mode",    "", DC_,  "", &iDP_Mode,    this, isParm | AffectsStruct , DDB2);

  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Const_P);
  DDB.Double("Lnk_P_Src",   "", DC_P, "kPag", &LnkP_S,  this, isParm);

  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Fixed_Drop || iDP_Mode==LDP_Fixed_Boost);
  DDB.Double("Fxd_dP",    "", DC_DP,  "kPa",  &FxddP,   this, isParm);

  DDB.Visibility(NM_Probal|SM_All|HM_All, iDP_Mode==LDP_Linear_DP || iDP_Mode==LDP_SquareLaw_DP);
  DDB.Double("S_MassFlw", "", DC_Qm,  "kg/s", &SQmCap,  this, isParm);
  DDB.Double("S_dP",      "", DC_DP,  "kPa",  &SQmdP,   this, isParm);
  DDB.Double("V_MassFlw", "", DC_Qm,  "kg/s", &VQmCap,  this, isParm);
  DDB.Double("V_dP",      "", DC_DP,  "kPa",  &VQmdP,   this, isParm);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer);//xiLF_Mode==LFM_OEPDuct);
  DDB.Double ("",             "Qm_Rqd", DC_Qm  , "kg/s",  &Reqd_Qm      ,this  ,isParm|NAN_OK);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, IsXfer);//xiLF_Mode!=LFM_OEPDuct);
  DDB.Double ("Qm_Capacity",  "",       DC_Qm  , "kg/s",  &Max_Qm       ,this  ,isParm|NAN_OK);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.CheckBoxBtn("HasVolume",  "",       DC_    , "",      &fHasVolume, this, isParm);
  DDB.CheckBoxBtn("VolIsSmall", "",       DC_    , "",      &fVolIsSmall, this, isParm);

  DDB.Visibility(NSHM_All);
  //DDB.Bool   ("",             "Flash", DC_    , "",       &bFlashIt     ,this  ,isParm|AffectsStruct, DDBYesNo);
  m_VLE.Add_OnOff(DDB);
  m_RB.Add_OnOff(DDB);
  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.Double ("Qm_MakeUp",     "",      DC_Qm     , "kg/s",   &dMkUpRqd, this, isParm|NAN_OK);//|AffectsStruct);

  DDB.Visibility();
  DDB.Text   ("Flow Conditions");

  DDB.Double("",             "QmIn",   DC_Qm  , "kg/s",   xidQi         ,this, InitHidden|0);
//  DDB.Double("",             "QmStb",     DC_Qm  , "kg/s",   xidQmStb,        this, InitHidden|StbDDBFlags());
//  DDB.Double("",             "QmImp",     DC_Qm  , "kg/s",   xidQmImp,        this, InitHidden|ImpDDBFlags());
//#if WithChdSoln
//  DDB.Double("",             "QmChd",     DC_Qm  , "kg/s",   xidQmChd,        this, InitHidden|ChdDDBFlags());
//#endif
  DDB.Double("",             "QmPrvPB",   DC_Qm  , "kg/s",   xidFEQmPrvPB,     this, InitHidden|NAN_OK);
  //DDB.Double("",             "QmPB",      DC_Qm  , "kg/s",   xidFEQmMemPB,     this, InitHidden|NAN_OK);
  DDB.Double("EstMassFlow",  "QmEst",     DC_Qm  , "kg/s",   xidQmEst,        this, 0);
//  DDB.Double("",             "QmAve",     DC_Qm  , "kg/s",   xidQmAve,        this, 0);
  DDB.Visibility(NSHM_All);
  DDB.Double("Mass_Flow",    "Qm",     DC_Qm  , "kg/s",   xidQm         ,this, isParmConstruct);
  DDB.Double("",             "QmOut",  DC_Qm  , "kg/s",   xidQo         ,this, InitHidden|isParmConstruct);
  DDB.Double("Vol_Flow",     "Qv",     DC_Qv  , "L/s",    xidQv         ,this, noFile|noSnap);
  DDB.Double("NVol_Flow",    "NQv",    DC_NQv , "NL/s",   xidNQv        ,this, noFile|noSnap);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer);//xiLF_Mode==LFM_OEPDuct);
  DDB.Double("Velocity",     "Vel",    DC_Ldt , "m/s",    xidVelocity   ,this, noFile|noSnap);
  DDB.Visibility(NSHM_All);

  DDB.Double("Temperature",  "T",      DC_T   , "C",      xidTemp       ,this, noFile|noSnap);
  DDB.Double("Density" ,     "Rho",    DC_Rho , "kg/m^3", xidRho        ,this, noFile|noSnap);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer);
  DDB.Text   ("Pressure Audit", Marker);
  DDB.Double ("Boost_Head",   "dPb",    DC_DP  , "kPa",   xidFEDPb, this, isParmConstruct);
  DDB.Double ("",             "dPbX",   DC_DP  , "kPa",   xidFEDPbX,this, InitHidden|isParmConstruct);
  DDB.Double ("Dynamic_Head", "dPq",    DC_DP  , "kPa",   xidFEDPq, this, isParmConstruct);
  DDB.Double ("Static_Head",  "dPz",    DC_DP  , "kPa",   xidFEDPz, this, isParmConstruct);
  DDB.Double ("Momentum",     "dPm",    DC_DP  , "kPa",   xidMomentum   ,this, isParmConstruct);
  DDB.Double ("Nett_Boost",   "Bst",    DC_DP  , "kPa",   xidNettBoost  ,this, noFile|noSnap);

//  DDB.Double ("MassAccSL",    "",       DC_M   , "kg",    &m_MassL      ,this, 0);
//  DDB.Double ("MassAccV",     "",       DC_M   , "kg",    &m_MassV      ,this, 0);

  DDB.Text   (" ");
  if (NoFlwIOs()>0 && IOPipeEntry_Rmt(0))
    DDB.Double ("",             "PiR",    DC_P   , "kPag",  xidPInRmt     ,this, noSnap);
  DDB.Double ("Press_Input" , "Pi",     DC_P   , "kPag",  xidPIn        ,this, 0);
//  DDB.Double ("",             "PiStb",  DC_P   , "kPag",  xidPInStb     ,this, InitHidden|noSnap);
//  DDB.Double ("",             "PiImp",  DC_P   , "kPag",  xidPInImp     ,this, InitHidden|noSnap);
//  DDB.Double ("",             "PiAve",  DC_P   , "kPag",  xidPInAve     ,this, InitHidden|noSnap);
//#if WithChdSoln
//  DDB.Double ("",             "PiChd",  DC_P   , "kPag",  xidPInChd     ,this, InitHidden|noSnap);
//#endif
  if (NoFlwIOs()>0)
    {
    DDB.Double ("",             "P0",     DC_P   , "kPag",  &IOs[0].PB.P, this, noSnap);
    DDB.Double ("",             "P0S",     DC_P   , "kPag",  &IOs[0].PB.PAtStart, this, noSnap);
    }
  DDB.Double ("Press_Change", "dP",     DC_DP  , "kPa",   xiddP         ,this, noFile|noSnap);
//  DDB.Double ("",             "PoStb",  DC_P   , "kPag",  xidPOutStb    ,this, InitHidden|noSnap);
//  DDB.Double ("",             "PoImp",  DC_P   , "kPag",  xidPOutImp    ,this, InitHidden|noSnap);
//  DDB.Double ("",             "PoAve",  DC_P   , "kPag",  xidPOutAve    ,this, InitHidden|noSnap);
//#if WithChdSoln
//  DDB.Double ("",             "PoChd",  DC_P   , "kPag",  xidPOutChd    ,this, InitHidden|noSnap);
//#endif
  if (NoFlwIOs()>1)
    {
    DDB.Double ("",             "P1",     DC_P   , "kPag",  &IOs[1].PB.P, this, noSnap);
    DDB.Double ("",             "P1S",     DC_P   , "kPag",  &IOs[1].PB.PAtStart, this, noSnap);
    }
  DDB.Double ("Press_Output", "Po",     DC_P   , "kPag",  xidPOut       ,this, 0);
  if (NoFlwIOs()>1 && IOPipeEntry_Rmt(1))
    DDB.Double ("",             "PoR",    DC_P   , "kPag",  xidPOutRmt    ,this, noSnap);
  DDB.Double ("PMean",        "",       DC_P  ,  "kPag",  xidPMean      ,this, noFile|noSnap);

  DDB.Visibility();
  Strng Page;
  Page.Set("%s..", sTag());
  DDB.Page(Page(), DDB_OptPage);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, !IsXfer);//xiLF_Mode==LFM_OEPDuct);
  DDB.Text   ("Rise/Length", Marker);
  DDB.Double ("Rise",          "dZ",     DC_L   , "m",     xidFEDZ       ,this, 0);
  DDB.Double ("Length",        "L",      DC_L   , "m",     xidFELength   ,this, 0);
  DDB.Double ("SrcH",         "SrcHgt",  DC_L,    "m",     xidSrcHgt     ,this, noFile|noSnap);
  DDB.Double ("DstH",         "DstHgt",  DC_L,    "m",     xidDstHgt     ,this, noFile|noSnap);
  DDB.Double ("",             "dZ_Rqd",  DC_L,    "m",     &dZ_Rqd       ,this, isParmStopped|NAN_OK);
  //DDB.Double ("",             "RhoForZ", DC_Rho,  "kg/m^3", &RhoForZ      ,this,  InitHidden);
  DDB.Double ("",             "ReqdVf",  DC_,     "%",     &FixedVf      ,this, isParm|NAN_OK);

  DDB.Visibility();
  DDB.Text   ("Connections");
  DDB.String ("SrcIO",         "",      DC_,     "",       xidSrcIOTag   ,this, noFile|noSnap);
  DDB.String ("DstIO",         "",      DC_,     "",       xidDstIOTag   ,this, noFile|noSnap);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Double ("SrcHF",        "SrcHFrc", DC_Frac, "%",     xidSrcHFrc    ,this, isParmStopped);
  DDB.Double ("DstHF",        "DstHFrc", DC_Frac, "%",     xidDstHFrc    ,this, isParmStopped);

  DDB.Visibility(NSHM_All);
  cioOn.BuildDataDefn(this, "Flow", DDB);

  FB.BuildDataDefn(FBDDOpt_WithEqn, DDB, this);

  if (NoFlwIOs()>0)
    {
    IOFB(0,0)->BuildDataDefn(FBDDOpt_WithData, DDB, this, "In", 1);
    if (IOPipeEntry_Rmt(0))
      IOFB_Rmt(0)->BuildDataDefn(DDB, this, "SrcEqn", 1);
    }
  if (NoFlwIOs()>1 && IOPipeEntry_Rmt(1))
    {
    IOFB(1,0)->BuildDataDefn(FBDDOpt_WithData, DDB, this, "Out", 2);
    if (IOPipeEntry_Rmt(0))
      IOFB_Rmt(1)->BuildDataDefn(DDB, this, "DstEqn", 2);
    }
//    dynamic_cast<FlwBlk*>(IOFlange())->BuildDataDefn(DDB, this);

  m_VLE.BuildDataDefn(DDB);
  m_RB.BuildDataDefn(DDB);
  //if (m_RB())
  //  DDB.Object(m_RB(), this, NULL, NULL, DDB_RqdPage);

//  if (Qc)
//    DDB.Object(Qc, this, "Qc", NULL, DDB_RqdPage);
  if (m_pContents)
    DDB.Object(m_pContents, this, "Content", NULL, DDB_RqdPage);

  if (NoFlwIOs() > 1)
    {
    if (IOConduitIn(0))
      {
      DDB.Visibility(NSHM_All, False);
      IOConduitIn(0)->Tag("QiI");
      DDB.Object(IOConduitIn(0), this, "QiI", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
      DDB.Visibility(NSHM_All);
      }
    IOConduit(0)->Tag("Qi");
    DDB.Object(IOConduit(0), this, "Qi", Nd_Rmt(0)->FullObjTag(), DDB_RqdPage);
    }
  if (NoFlwIOs() >= 2)// && (m_VLE.Enabled()|| m_RB()!=NULL || IOFlange(0)->IsTear() || IOFlange(1)->IsTear()))
    {
    IOConduit(1)->Tag("Qo");
    DDB.Object(IOConduit(1), this, "Qo", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
    if (IOConduitIn(1))
      {
      DDB.Visibility(NSHM_All, False);
      IOConduitIn(1)->Tag("QoI");
      DDB.Object(IOConduitIn(1), this, "QoI", Nd_Rmt(1)->FullObjTag(), DDB_RqdPage);
      DDB.Visibility(NSHM_All);
      }
    }

  if ((NoFlwIOs()>=1) && IOFlange(0)->TearImageExists())
    {
    DDB.Visibility(NSHM_All, IOFlange(0)->RqdTearType()==TT_ManualTear);
    IOFlange(0)->TearImage()->Tag("QEst");
    DDB.Object(IOFlange(0)->TearImage(), this, "QEst", Nd_Rmt(0)->FullObjTag(), DDB_RqdPage);
    DDB.Visibility();
    }

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag Duct::DataXchg(DataChangeBlk & DCB)
  {
  if (NoFlwIOs()>=1 && DCB.dwUserInfo==1 && IOFB_Rmt(0)->DataXchg(DCB))
    return 1;
  if (NoFlwIOs()>=2 && DCB.dwUserInfo==2 && IOFB_Rmt(1)->DataXchg(DCB))
    return 1;
  if (MN_Lnk::DataXchg(DCB))
    return 1;
  if (m_RB.DataXchg(DCB))
    return 1;

  return 0;
  }

// --------------------------------------------------------------------------

flag Duct::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Lnk::ValidateData(VDB);
//  if (!FB.ValidateData(VDB))
//    OK=False;
  if (!m_RB.ValidateData(VDB))
    OK=False;
  return OK;
  }

//--------------------------------------------------------------------------

void Duct::PostConnect(int IONo)
  {
  MN_Lnk::PostConnect(IONo);

  IOFB_Rmt(IONo)->AssignFlwEqnGroup(PipeEntryGroup, NULL, this);
  };

//---------------------------------------------------------------------------

void Duct::PreDisConnect(int IONo)
  {
  MN_Lnk::PreDisConnect(IONo);
  }
*/
//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Diameter Ratio Eqn for an Orifice
#X:#h<General Description>#nThis is a flow equation for an orifice.
This calculates the pressure drop within an orifice, according to the ratio of
the orifice diameter to the pipe inner diameter.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Diam_Orif> : The orifice diameter.#n
#i<Diam_Pipe> : The inner pipe diameter.#n
#n#l<#b<Note>> : This ratio is ranged. The Diam_Orif is ranged between the values
of 0.2 #* Diam_Pipe and 0.8 #* Diam_Pipe.#n
#n
#n#h<Other>#n
Short name:OE_DRat#n
Model type:Orifice Flow Equation#n
#G:Orifice Flow Equations
*/
IMPLEMENT_FLWEQN(DuctBF, ValveGroup.Name(), "VBF", "", TOC_SYSTEM,
                 "Duct Butterfly",
                 "");

DuctBF::DuctBF(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  Diam_Pipe=0.1;//100mm
  KvMax=0;
  KvActual=0;
  }

//--------------------------------------------------------------------------

DuctBF::~DuctBF()
  {
  }

//--------------------------------------------------------------------------

void DuctBF::BuildDataDefn(DataDefnBlk & DDB)
  {
//  DDB.Double ("Orifice Diameter",    "Diam_Orif",  DC_L, "mm", &Diam_Orifice, this, isParm);
  DDB.Double ("Inner Diameter", "Diam_Pipe",  DC_L, "mm", &Diam_Pipe,  this, isParmStopped);
  DDB.Double ("KvMax",          "",           DC_,  "",   &KvMax,      this, isParmConstruct);
  DDB.Double ("KvActual",       "",           DC_,  "",   &KvActual,   this, isParmConstruct);
  }

//--------------------------------------------------------------------------

flag DuctBF::ValidateData(ValidateDataBlk & VDB)
  {
  KvMax=0.0792*Pow(Diam_Pipe*1000, 1.9876);
  return true;
  }

//--------------------------------------------------------------------------

flag DuctBF::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  // Based on Data Sheet for Keystone Butterfly Valves for GASSES
  //
  //  Kv=Q / 28.5 * Sqrt( SG / (P2*DP) )
  //  Q NM3/h
  //  SG (Air=1)
  //  DP (bar) Less than half inlet Pressure
  //  P2 Downstream P (bar)
  //
  // See SpeadSheet 'Damper DP's.XLS'
  //
  // NBNB Currently the downstream Pressure P2 is ignored
  //

  const double KvRatio[10]=
    { 0,
      0.007230939,
      0.033183246,
      0.074907242,
      0.131935905,
      0.210684783,
      0.334080729,
      0.552022395,
      0.855421723,
      1
    };

  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmMeasRange(Max(0.001, FE.MeanRho(pProps)), 1.0e-6);
    FE.SetQmFree();

    double Rho = FE.MeanRho(pProps);
    double NRho = FE.MeanRho(pProps)*FE.MeanPress()/Norm_P*Norm_T/GTZ(FE.MeanTemp(pProps));

    //  Fitted to Data

    double AirRhoSTD=1.0;
    double OpenAngle=90.0*Regulation;
    int iOpen=Range(0, (int)OpenAngle/10, 9);
    double OpenLo=10.0*iOpen;
    double OpenHi=10.0*(iOpen+1);
    KvActual=KvMax*(KvRatio[iOpen]+(KvRatio[iOpen+1]-KvRatio[iOpen])*(OpenAngle-OpenLo)/(OpenHi-OpenLo));

    double Qm=fabs(FE.QmMeas());

    double SG=Rho/AirRhoSTD;
    double ConstX=3600.0/(NRho*28.5*KvActual);
    double DP=-FE.QmSign()*100*SG*Sqr(Qm*ConstX);
    double DPDQ=-FE.QmSign()*2*100*SG*Qm*Sqr(ConstX);
    FE.SetDPq(DP, DPDQ);

    if (pAttachedTo->fDoDbgBrk)
      { int xxx=0; };
    }
  else
    {
    FE.SetQmReqd(0.0);
    }

//  if (Regulation > MinValveOpening)
//    {
//    FE.RangeQmMeas(1.0e-6);
//    FE.SetQmFree();
//
//    double Rho = FE.MeanRho(pProps);
//    // Area of Ellipse
//    double Major=Diam_Pipe/2;
//    double Minor=Diam_Pipe/2*cos(Regulation*PI/2.0);
//    double DiscArea=PI*Minor*Major;
//    double OpenArea=PI*Sqr(Diam_Pipe)/4-DiscArea;
//    double EquivOrifDiam=Sqrt(OpenArea*4/PI);
////    double Diam_Orifice=Diam_Pipe*cos((1-Regulation)*PI/2.0);
//    //double Rho=
//    EquivOrifDiam= Min(EquivOrifDiam, 0.99*Diam_Pipe);
//    double DR = EquivOrifDiam/GTZ(Diam_Pipe);//diameter ratio
//    double DRpowFour = Pow(DR,4.0);//diameter ratio raised to power of 4
//    double DRexpr = 1.0-DRpowFour;//1-(diam ratio to power of 4)
//    double E = Pow(DRexpr,-0.5);//Velocity_of_Approach factor
//    double DC = 0.5959+0.0312*Pow(DR,2.1)-0.184*Pow(DR,8.0)+0.039
//               *DRpowFour*Pow(DRexpr,-1.0)-0.01584*Pow(DR,3.0);//Discharge coefficient
//    FE.SetDPb(0.0, 0.0);
////    FE.SetDPq(-FE.QmSign()*Sqr((4000.0*FE.QmMeas())/(DC*E*PI*Diam_Orifice*Rho))/(2.0*Rho),
////              -FE.QmSign()*(FE.QmMeas()/Rho)*Sqr(4000.0/(DC*E*PI*Diam_Orifice*Rho)));
//    double ConstX=DC*E*PI*EquivOrifDiam*Rho;
////  CNM ?? What is the 4000.0
//    double DP=-FE.QmSign()*Sqr(FE.QmMeas())/(Sqr(ConstX)*(2.0*Rho));
//    double DPDQ=-FE.QmSign()*FE.QmMeas()/(Sqr(ConstX)*Rho);
//    FE.SetDPq(DP, DPDQ);
//
//
//dbgpln("%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g",  Regulation, DP, DPDQ, DR, DRpowFour, DRexpr, E, DC);
//
//    if (pAttachedTo->fDoDbgBrk)
//      { int xxx=0; };
//    }
//  else
//    {
//dbgpln("Regulation\tDP\tDPDQ\tDR\tDRpowFour\tDRexpr\tE\tDC");
//    FE.SetQmReqd(0.0);
//    }

//  #if dbgValve
//  dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",FE.QmMeas(),Rhoi,OpCv,dPq,dPqdQ, FullTag());
//  #endif
  return True;
  }

//==========================================================================
//
//
//
//==========================================================================
