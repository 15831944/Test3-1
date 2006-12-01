//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __SRCSNK_CPP
#include "srcsnk.h"
#include "flshtrn.h"
//#include "optoff.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Feeder/CrossPage(1)
#X:#h<General Description>#nThis functions as a source of material or a sink
for material. It can also be used as a cross page connector, in which case it
would be connected to another Feeder/CrossPage unit (typically on another
flowsheet page). The model requires either a single inlet or a single outlet,
depending upon its intended use.#pIf two feeders are connected then the two
units are ignored and the two pipes act as a single pipe. The majority of
parameters and variables become meaningless. More than two units cannot be
connected together.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Snk          Input          1               1    OR#n
Src          Output         1               1#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Qm_Mode> : Select the required fedd mode.  The user may choose Total, Solids, 
Liquids or Vapours#n
#i<Mode> : Select the operational mode of the material. It can be a source
or a sink. For each option there are three modes, constant pressure, constant
mass flow rate or constant volumetric flow. Depending on the selection different
parameters need to be configured. #b<Note>, the nature of the flowsheet network
may result in material flowing in the oposite direction to that expected. The
mode may then automatically change ie SourceConstP may become SinkConstP.#n
#i<P_Rqd> : The required feed pressure.#n
#i<P_Std_Dev> : The Gausian standard deviation as a percent around which P_Rqd
can vary for constant pressure mode.#n
#i<Qm_Rqd> : The required mass flow of the material within the unit for constent 
Qm mode.#n
#i<Q_Std_Dev> : The Gausian standard deviation as a percent around which Qm_Rqd
can vary for constant Qm mode.#n
#i<Qv_Rqd> : The required mass flow of the material within the unit for constent 
Qv mode.#n
#i<Batch> : The quantity of material available at a source. While the flowsheet
is running this will decrease until it is zero.#n
#i<T_Rqd> : The required temperature of the material within the unit. This will
be the temperature of the material if the unit is acting as a source.#n
#i<T_Std_Dev> : The Gausian standard deviation as a percent around which T_Rqd
can vary when the unit is a source.#n
#i<VLEquilibrium> : This can be used to switch #x<Vapour Liquid Equilibrium Logic>
on. If it is on then the associated variables (VLE) must be configured.#n
#i<Z_Reqd> : The required datum of the unit.#n
#i<Conn_To> : The tag of the unit to which this unit is connected to. This is
only required when the unit is being used as a cross page connector. #b<Note>,
if you want to connect two feeders only one of the two must specify a connection
tag. More than two units cannot be connected together.#n
#i<Content.Model> : This allows the selection of the model that is to be used to calculate
how the species are combined, this is normally done using a mass weighted mean.#n
#i<Content.View> : This allows the selection of how the list of species is being displayed.
Options such as MassFlow, MoleFlow, VolFlow, MassFrac, ElemntFlow, CompntConc etc can
be selected.#n
#i<Content.Phase> : This allows the selection of which phase(s) for the list of species
is being displayed. Options such as All, Solids, Liquids and Vapours can be selected.#n
#i<Species List> : This is a list of all the available species for the project. If the
unit is being used as a source - then this is where the specie composition of the source
unit is determined.#n
#i<Specie Lockup> : This causes a percentage of the species, to be retained within the
unit. The amount of retention is dependent upon the type of unit.#n
#i<Size Distribution> : This associates a particle size distribution curve with each specie.
The relevant specie size distributions need to be configured.#n
#n#n
#h<Associated variables>#n
#i<P>: The pressure of the material from the unit,#n
#i<T>: Temperature of the material from the unit.#n
#i<Qm>: The mass flow of the material from the unit.#n
#i<Qv>: The volumetric flowrate of the material from the unit.#n
#i<Rho>: The density of the material flowing from the unit.#n
#i<Z>: The height of the unit, in relation to the common datum.#n
#i<Connected>: If the feeder is not connected or the solver is not running a '-' is shown.#n
#i<Sf>: The percentage of solids in the material flowing from the unit.#n
#i<Lf>: The percentage of liquids in the material flowing from the unit.#n
#i<Vf>: The percentage of vapours in the material flowing from the unit.#n
#n
#n#h<Other>#n
Default model prefix:XPG#n
Short name:Feed_XPG#n
Model type:Unit#n
#G:Units
*/

static IOAreaRec Feed_XPGIOAreaList[] =
  {{"Sink of Material",       "Snk"  , SSIO_Snk, LIO_In0 ,  nc_MLnk, 0, 1, IOPipeEntry, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},
   {"Sink of Material",       "I"    , SSIO_Snk, LIO_In  ,  nc_MLnk, 0, 1, IOPipeEntry, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},
   {"Source of Material",     "Src"  , SSIO_Src, LIO_Out0,  nc_MLnk, 0, 1, IOPipeEntry, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},
   {"Source of Material",     "O"    , SSIO_Src, LIO_Out ,  nc_MLnk, 0, 1, IOPipeEntry, 0.0f, {0,0,0,0}, 0, &NullFlwGroup},
    MAKEUP2AREA(),
    BLEED2AREA(),
   {NULL}};

static double Drw_Feed_XPG[] = { DD_Poly, -10,3, 7,3, 10,0, 7,-3, -10,-3 , -7,0, -10,3,
                      DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Feed_XPG, "Feed_XPG", "1", Drw_Feed_XPG, "Feed", "XPG", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Process:Unit:Feeder/CrossPage(1)", 
                    "Simple Source and/or Sink of Material - "
                    "Also used as a Cross page connector") 

Feed_XPG::Feed_XPG(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_SrcSnk(pClass_, TagIn, pAttach, eAttach)
  {
  AttachIOAreas(Feed_XPGIOAreaList);
  Contents.SetView(SVV_AsMassFrac);  
  Flows.SetView(SVV_AsMassFlow);

  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(Feed_XPG), SSIO_Snk, mmio_CONNECT, NULL);
  RegisterMacroMdlNode(CMMFlashTrain::MMIOs, &typeid(Feed_XPG), SSIO_Src, mmio_CONNECT, NULL);

  SSMode=SSM_Source|SSM_ConstQm;
  }

//--------------------------------------------------------------------------

void Feed_XPG::PrepareDefinedData()
  {
#if WITHSPECSHEETS
  if (sSpecSheet.XStrCmp("-")!=0)
    sSpecSheet= Contents.Model()->ShortDesc();
#endif
  //Contents.SetView(SVV_AsMassFrac);
  //Flows.SetView(SVV_AsMassFlow);

  }

//--------------------------------------------------------------------------

static DDBValueLst DDB0[]={
  {SSQmRqdTot, "Total"},
  {SSQmRqdSol, "Solids"},
  {SSQmRqdLiq, "Liquid"},
  {SSQmRqdVap, "Vapour"},
  {0}};
static DDBValueLst DDBSrcSnk[]={
  {SSM_Source               ,  "Source"},
  {SSM_Source | SSM_ConstP  ,  "Source_ConstP"},
  {SSM_Source | SSM_ConstQm ,  "Source_ConstQm"},
  {SSM_Source | SSM_ConstQv ,  "Source_ConstQv"},
  {SSM_Source | SSM_ConstNQv,  "Source_ConstNQv"},
  {SSM_Sink                 ,  "Sink"},
  {SSM_Sink   | SSM_ConstP  ,  "Sink_ConstP"},
  {SSM_Sink   | SSM_ConstQm ,  "Sink_ConstQm"},
  {SSM_Sink   | SSM_ConstQv ,  "Sink_ConstQv"},
  {SSM_Sink   | SSM_ConstNQv,  "Sink_ConstNQv"},
  {0}};
static DDBValueLst DDB2[]={
  {False, "-"},
  {True,  "Yes"},
  {0}};
static DDBValueLst DDBQmRev[]={
  {SSQmRev_Null,  "Nothing"}, 
  {SSQmRev_Image, "Image"}, 
  {SSQmRev_InFlw, "PrevInFlow"},
  {0}};
static DDBValueLst DDBVLEO[]={
  {VLEO_Off,    "Off",          MDD_Default },
  {VLEO_SatLiq, "Saturated_Liq" },
  {VLEO_SatVap, "Saturated_Vap" },
  {VLEO_SatLiqP, "Saturated_Liq(P)" },
  {VLEO_SatVapP, "Saturated_Vap(P)" },
  {VLEO_SubLiq, "SubCooled_Liq" },
  {VLEO_SupVap, "SuperHeat_Vap" },
  {VLEO_SatVap, "On",           MDD_Hidden  },
  {0}};
static DDBValueLst DDBKeepTrace[]={
  {0,    "Nothing"},
  {1,    "Trace"},
  {0}};
  
void Feed_XPG::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);

  DDB.Text   ("Flow Conditions");
  DDB.Double ("Mass_Flow",    "Qm",   DC_Qm,   "kg/s",   xidQm,         this, isResult|noFile|noSnap);
  DDB.Double ("Vol_Flow",     "Qv",   DC_Qv,   "L/s",    xidQv,         this, isResult|noFile|noSnap);
  DDB.Double ("NVol_Flow",    "NQv",  DC_NQv,  "NL/s",   xidNQv,        this, isResult|noFile|noSnap);
  DDB.Double ("Temperature",  "T",    DC_T,    "C",      xidTemp,       this, isResult|noFile|noSnap);
  DDB.Double ("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,        this, isResult|noFile|noSnap);
  DDB.Double ("NDensity",     "NRho", DC_Rho,  "kg/m^3", xidNRho,       this, isResult|noFile|noSnap);
  DDB.Double ("Pressure",     "P",    DC_P,    "kPag",   xidPMean,      this, isResult|noFile|noSnap);

  DDB.Text   ("Mode of Operation");
  DDB.Visibility(NM_Dynamic|SM_All|HM_All|SM_Direct);
  DDB.Byte   ("",             "Qm_Mode", DC_, "", &QmRqdMode,  this, (fCrossConnected?0:isParm)|SetOnChange, DDB0);
  DDB.Byte   ("",             "Mode",    DC_, "", xidSSMode,   this, (fCrossConnected?0:isParm) | SetOnChange | AffectsStruct, DDBSrcSnk);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, (SSMode & SSM_Sink));
  DDB.Byte  ("", "RevFeed", DC_, "", &iRevFlw,   this, (fCrossConnected?0:isParm), DDBQmRev);

  DDB.Visibility(NM_Probal|SM_All|HM_All);
  DDB.CheckBoxBtn("MakeUpMode", "", DC_, "", &bMakeUpMode, this, (fCrossConnected?0:isParm), DDBOnBlank);


  flag ParmOK=true;
  if (0 && (SSMode & (SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))) // taken out by CNM
    {
    int TstArray[]={
      SVV_AsMass,     SVV_AsMassFlow,    
      SVV_AsMole,     SVV_AsMoleFlow,    
      SVV_AsVol,      SVV_AsVolFlow,     
      SVV_AsNVol,     SVV_AsNVolFlow, 
      SVV_AsElem,     SVV_AsElemFlow,    
      SVV_AsComp,     SVV_AsCompFlow,    
      -1};
    for (int i=0; ParmOK && TstArray[i]>=0; i++)
      if (Contents.GetView()==TstArray[i])
        ParmOK=false;
    }
  DDB.Visibility(NSHM_All, ((SSMode & SSM_ConstQm)!=0)||
                            NetProbalMethod() && (((SSMode & (SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))==0)));
  DDB.Double ("MassFlow_Rqd", "Qm_Rqd", DC_Qm , "kg/s", xidQmRqd,       this, ParmOK ? (fCrossConnected?0:isParm) : isResult);
  if (bMakeUpMode)
    DDB.TagComment("Min");
  DDB.Visibility(NSHM_All, ((SSMode & SSM_ConstQv)!=0));
  DDB.Double ("VolFlow_Rqd",  "Qv_Rqd", DC_Qv , "L/s",  xidQvRqd,       this, ParmOK ? (fCrossConnected?0:isParm) : isResult);
  if (bMakeUpMode)
    DDB.TagComment("Min");
  DDB.Visibility(NSHM_All, ((SSMode & SSM_ConstNQv)!=0));
  DDB.Double ("NVolFlow_Rqd",  "NQv_Rqd", DC_NQv , "NL/s",  xidNQvRqd,  this, ParmOK ? (fCrossConnected?0:isParm) : isResult);

  if (NetDynamicMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All, ((SSMode & (SSM_ConstQm|SSM_ConstQv|SSM_ConstNQv))!=0));
    DDB.Double("Q_Std_Dev",     "",    DC_Frac,  "%",     &dQ_Std_Dev,    this, (fCrossConnected?0:isParm)|InitHidden);
    DDB.Double("Q_Tau",         "",    DC_Time,  "s",     &dQ_Tau,        this, (fCrossConnected?0:isParm)|InitHidden);
    }
  DDB.Visibility();
  DDB.Text  (SSMode & SSM_Sink ? "Sink Conditions" : "Feed Conditions");
  DDB.Byte  ("VLEquilibrium",  "",             DC_, "", xidSSVLEOnOff,  this, (fCrossConnected?0:isParm)|SetOnChange, DDBVLEO);
  DDB.Visibility(NSHM_All);
  int PIsParm=(iVLEOpt!=VLEO_SatLiq && iVLEOpt!=VLEO_SatVap)&&!fCrossConnected;
  int TIsParm=(iVLEOpt!=VLEO_SatLiqP && iVLEOpt!=VLEO_SatVapP)&&!fCrossConnected;
  DDB.Double("P_Rqd",         "",    DC_P   , "kPag",   xidPRqd,        this, (PIsParm?isParm:0)|NAN_OK);
  if (NetDynamicMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Double("P_Std_Dev",     "",    DC_Frac,  "%",     &dP_Std_Dev,    this, (PIsParm?isParm:0)|InitHidden);
    DDB.Double("P_Tau",         "",    DC_Time,  "s",     &dP_Tau,        this, (PIsParm?isParm:0)|InitHidden);
    DDB.Double("Head",          "",    DC_L,     "m",     &dHead,         this, (fCrossConnected?0:isParm));
    }

  DDB.Visibility(NSHM_All);
  //DDB.Text  ("Temperature");
  DDB.Double("T_Rqd",         "",    DC_T   , "C",      xidTRqd,        this, (TIsParm?isParm:0)|NAN_OK);
  if (NetDynamicMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Double("T_Std_Dev",     "",    DC_Frac,  "%",     &dT_Std_Dev,    this, (TIsParm?isParm:0)|InitHidden);
    DDB.Double("T_Tau",         "",    DC_Time,  "s",     &dT_Tau,        this, (TIsParm?isParm:0)|InitHidden);
    }

  DDB.Visibility();
  DDB.Byte  ("ZeroOutput",  "",             DC_, "", &m_bKeepTrace,     this, (fCrossConnected?0:isParm)|SetOnChange, DDBKeepTrace);

  if (NetDynamicMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    DDB.Text  ("Batch Feed");
    DDB.Double("Batch",         "",    DC_M   , "kg",     &BatchAvail,    this, (fCrossConnected?0:isParm)|NAN_OK);
    DDB.Double("BatchAddition", "",    DC_M   , "kg",     xidBatchAddition, this, (fCrossConnected?0:isParm)|noFileAtAll);
    }
  
  //VLE.Add_OnOff(DDB);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  BuildDataDefnElevation(DDB);
  
  DDB.Visibility(NSHM_All, !fCrossConnected);
  DDB.Text  ("");
  m_BlkEval.Add_OnOff(DDB);  

  DDB.Visibility();
  DDB.Text  ("");
  DDB.Text   ("Cross Page Connection");
  DDB.CheckBoxBtn("",       "ConnectionOn", DC_, "", &fConnectionOn,   this, sConnectTag.Length()>0 ? isParmStopped : isParmConstruct);
  DDB.String ("Connection", "Conn_To",      DC_, "", xidConnectTag,    this, 
    (fConnectInitiator && pConnectedTo || !pConnectedTo) && fConnectionOn ? isParmStopped|isTag : isTag);
  DDB.String ("",           "Connected_To", DC_, "", xidConnectedTag,  this, DDEF_WRITEPROTECT|isTag);
  DDB.Bool   ("Connected",  "",             DC_, "", &fCrossConnected, this, DDEF_WRITEPROTECT, DDB2);

  //if (DDB.BeginStruct(NULL, "Fetch", NULL, DDB_NoPage, -1, MarkerClosed))
  if (1)
    {
    DDBValueLst DDBFetchOp[]={
      {CFWhenOff,         "Off",       },
      {CFWhenOnceOff,     "OnceOff"    },
      {CFWhenContinuous,  "Continuous" },
      {0}};

    int Any = 0;
    int QDrv = SSMode&SSM_ConstP ? MDD_Hidden : 0;
    DDBValueLst DDBFetchWhat[]={
      {CFWhatNull,                                    "Nothing"             }, 
      {CFWhatTemp|CFWhatPress,                        "T,P"                 },
      {CFWhatComp,                                    "Comp"                },
      {CFWhatFlow,                                    "Flow",           QDrv},
      {CFWhatTemp|CFWhatComp,                         "T,Comp"              },
      {CFWhatTemp|CFWhatPress|CFWhatComp,             "T,P,Comp"            },
      {CFWhatTemp|CFWhatPress|CFWhatFlow,             "T,P,Flow",       QDrv},
      {CFWhatComp|CFWhatFlow,                         "Comp,Flow",      QDrv},
      {CFWhatTemp|CFWhatPress|CFWhatComp|CFWhatFlow,  "T,P,Comp,Flow",  QDrv},
      {0}};

    DDB.Visibility();
    DDB.Text  ("");
    DDB.Text   ("Composition Fetch");
    DDB.Byte  ("CF.When",    "", DC_, "", xidCFWhen,     this, isParm|SetOnChange, DDBFetchOp);
    DDB.Byte  ("CF.What",    "", DC_, "", &m_iCFWhat,    this, isParm|SetOnChange, DDBFetchWhat);

    if (m_iCFWhat&CFWhatComp)
      {
      for (int iQ=0; iQ<sm_nCFQuals; iQ++)
        DDB.CheckBox(sm_CFQualsNm[iQ].Str(), "", DC_, "", &m_CFQualsOn[iQ], this, isParm); 
      }

    DDB.Visibility(NSHM_All, (SSMode&SSM_ConstP)==0);
    DDB.Float ("CF.Ratio",   "", DC_, "", &m_fCFRatio,   this, isParm);
    DDB.Visibility();
    DDB.String("CF.Source",  "", DC_, "", xidCFTag,      this, isParm|isTag);
    DDB.Button("CF.Fetch",   "", DC_, "", xidCFFetchNow, this, isParm);
    DDB.Visibility();
    }
  //DDB.EndStruct();

  if (!NetProbalMethod())
    {
    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    P_RegB.BuildDataDefn(DDB, this, "Rate", 1);
    T_RegB.BuildDataDefn(DDB, this, "Temp", 2);
    }

  DDB.Visibility(NSHM_All);

  m_VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

  DDB.Visibility(NSHM_All, False);
  DDB.Double("POut",         "",    DC_P   , "kPag",   &dP_Out,  this, isResult|0);
  DDB.Double("QOut",         "",    DC_Qm  , "kg/s",   &dQ_Out,  this, isResult|0);
  DDB.Double("TOut",         "",    DC_T   , "C",      &dT_Out,  this, isResult|0);

  DDB.Visibility(NSHM_All, !fCrossConnected);
  m_BlkEval.BuildDataDefn(DDB);

  DDB.Visibility(NSHM_All, (SSMode & SSM_Source) || (iRevFlw==SSQmRev_Image));
  DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
  DDB.Visibility();
  DDB.Object(&Flows, this, NULL, NULL, DDB_RqdPage);

  BuildDataDefnAreaIOs(DDB);

  DDB.EndStruct();
  };  

//--------------------------------------------------------------------------

flag Feed_XPG::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidSSVLEOnOff:
      {
      if (DCB.rB)
        {
        iVLEOpt=*DCB.rB;
        if (iVLEOpt!=VLEO_Off && !m_VLE.Enabled()) 
          m_VLE.Open();
        else if (iVLEOpt==VLEO_Off && m_VLE.Enabled()) 
          m_VLE.Close();
        }
      DCB.B=iVLEOpt;
      }
      return 1;
    }
  
  if (MN_SrcSnk::DataXchg(DCB)) 
    return 1;

  if (m_VLE.DataXchg(DCB))
    return 1;

  return 0;
  }

//==========================================================================

