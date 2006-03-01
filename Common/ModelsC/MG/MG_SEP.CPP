//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "sc_defs.h"
#include "datacnvs.h"
#include "ndtree.h"
#include "flwnode.h"
#define  __MG_SEP_CPP
#include "mg_sep.h"
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#include "net_err.h"

//#include "optoff.h"

#define DbgMS_Solve      0
#define dbgDerivs        DBG_Flows

#define dbgHeatX         0
#define dbgBoot          0
#define dbgVLL           0
#define dbgAbsorbFull    0
#define dbgAbsorb        0

#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

//==========================================================================

SPECIEBLK_L(Water,       "H2O"   , false  );
SPECIEBLK_V(Steam,       "H2O"   , false  );
SPECIEBLK_L(MEG,         "MEG"   , false  );

//==========================================================================

const int VLLIO_in   = 0;
const int VLLIO_out  = 1;
const int VLLIO_boot = 2;

//==========================================================================
// never checked mhm
double DewPt(rSpContainer Contents, rSpConduit Qi, double P)
  {
  //  Y0 lb/MMscf
  //  OpPres kPa abs
  //  IF (OpPres > RZero) AND (Y0 > RZero) THEN
  //    IF ((27.248 - 0.8446 * LN (OpPres) - LN (Y0)) <> RZero) THEN
  //      GasOutletDP := (5214.4 - 38.34 * LN (OpPres)) /
  //                     (27.248 - 0.8446 * LN (OpPres) - LN (Y0));

  double VolFlw=Qi.QMass(som_ALL)*Qi.NRho(som_Vap);//44Qi);      // m^3/s


  double H2oMassFlw=Qi.Qm(Water())+Qi.Qm(Steam());  // kg/s
  double Y0=kg_2_lb(H2oMassFlw)/GTZ(Nm3_2_MMscf(VolFlw));
  if ((P > 1.0) && (Y0 > 1.0e-10))
    {
    double Den= 27.248 - 0.8446 * log(P) - log(Y0);
    if (fabs(Den) > 1.0e-10)
      return (5214.4 - 38.34 * log(P)) / Den;
    }
  return dNAN();
  }

//==========================================================================
//
//                            Flare
//
//==========================================================================
/*#D:#T:Flare
#X:#n#h<General Description>#N#NThis is used to prevent a build up of  pressure,  by burning
off the excess material

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed        Input          1                20#n
#a

#n#n#h<Variables to be supplied by the user>
#n#i<Flare :>The type of flare to be used (to do). The default is VG1601

#n#n#h<Variables calculated by the model>
#n#i<Z :>The height of the flare, in relation to the common datum.
#n#i<Qm :>The mass flow of the material through the flare.
#n#i<Qv :>The volumetric flowrate of the material within the flare.
#n#i<PiR :>The remote pressure at the inlet of the flare i.e. the pressure
measured at the flange.

#n#n#h<Other>#n
Default model prefix:FL#n
Short name:Flare#n
Model type:Unit#n
#G:Units
*/
// To Improve
long Drw_Flare[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                     DD_End };

//--------------------------------------------------------------------------

static IOAreaRec FlareIOAreaList[] =
  {{"Feed",                 "Feed"      , 0, LIO_In ,    nc_MLnk, 1, 20},
   {NULL}};

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Flare, "Flare", "", Drw_Flare, "FL", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "Flare",
                    "Flare")


Flare::Flare(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
   MN_SrcSnk(pClass_, TagIn, pAttach, eAttach)//,
  {
  AttachClassInfo(nc_Process, FlareIOAreaList);
  FlareDrum=NULL;

  FlareTag="VG1601";

  ValidateDataBlk VDB;
  ValidateData(VDB);
  };

// -------------------------------------------------------------------------
const int idf_mqm = 111;

void Flare::BuildDataDefn(DataDefnBlk &DDB)
    {
  //  if (Page & 0x1)
  DDB.BeginStruct(this);
  DDB.Page   ("Page 1");
  DDB.Text   ("Datum");
  DDB.Double ("Datum",        "Z",    DC_L    , "m",    &dDatum,        this, isResult|0      | DYNMODE);
  DDB.Double ("Datum_Rqd",    "Z_Rqd",DC_L    , "m",    &dDatum_Rqd,    this, isParmStopped | DYNMODE|NAN_OK);
  DDB.Text   (" ");
  DDB.Double ("Mass_Flow",   "Qm",    DC_Qm,  "kg/s",  xidQm,     this, 0); //  ,NDT_ZERODEFAULT|NDT_SGN           )
  DDB.Double ("Vol_Flow",    "Qv",    DC_Qm,  "m^3/h", xidQv,     this, 0); //  ,NDT_SGN           )
  DDB.Text   (" ");
  DDB.Double ("Press_Input", "PiR",   DC_P,   "kPag",  xidPInRmt, this, 0); //  ,NDT_SGN    )
  DDB.Text   (" ");
//    }

//  if (Page & 0x2)
//    {
  DDB.Page   ("Page 2");
  DDB.String ("Flare",        "",           DC_,      "",     &FlareTag,   this,  isParm);
  DDB.Text   (" ");
//    }

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag Flare::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_SrcSnk::DataXchg(DCB))
    return 1;

  return 0;
  };

//--------------------------------------------------------------------------

flag Flare::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_SrcSnk::ValidateData(VDB);
  if (bObjModified)//DataChanged)
    FlareDrum=NULL;
  return OK;
  }

//--------------------------------------------------------------------------

void Flare::EvalJoinPressures()
  {
  double Pm=AtmosPress();
  for (int i = 0; i < NoFlwIOs(); i++)
    //if ((IOs + i)->Conn)
    //  {
    //  pFlange pC = IOFlange(i);
      Set_IOP_Self(i,Pm);
    //  }
  }

//--------------------------------------------------------------------------

//flag Flare::EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE)
//  {
////CNM  if (NoFlwIOs()>0)
////CNM    IORmt(0)->R_Fxd=1.0e-3;
//  return False;
//  }

//--------------------------------------------------------------------------

flag Flare::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  pFlwBlk pC = IOFB(IONo, FE);
  pC->SetQmFree();
  return True;
  };

// --------------------------------------------------------------------------

void Flare::EvalProducts()
  {
  MN_SrcSnk::EvalProducts();
  if ((FlareDrum==NULL) && (FlareTag[0]!='~'))
    {
    FlareDrum = (pFlashTank/*MdlNode*/)FamilyHead()->FindObj(FlareTag());
    if (FlareDrum==NULL)
      FlareTag[0]='~';
    }
  if (FlareDrum && (NoFlwIOs()>0))
    {
    //    \\FlareDrum->EvalDerivs_AddQm_____(FlareDrum->Contents, *IOConduit(0));
    //FlareDrum->Contents.InFlow().QAdd(*IOConduit(0), 1.0);

    SpContainer &C=FlareDrum->Contents;//M.OverFlow().Target();
    SpConduit &I=C.InFlow();
    if (C.InFlow().ZeroReqd())
      {
      C.InFlow().ClrZeroReqd();
      I.QSetF(*IOConduit(0), som_ALL, 1.0, AtmosPress());
      }
    else
      I.QAddF(*IOConduit(0), som_ALL, 1.0);

    //CNM    FlareDrum->Flux.QAdd(IOFlange(0)->Q, som_ALL, 1.0);
    }
  }

// --------------------------------------------------------------------------

void Flare::EvalDerivs()
  {
//  if ((FlareDrum==NULL) && (FlareTag[0]!='~'))
//    {
//    FlareDrum = (pFlashTank/*MdlNode*/)FamilyHead()->FindObj(FlareTag);
//    if (FlareDrum==NULL)
//      FlareTag[0]='~';
//    }
//  if (FlareDrum && (NoFlwIOs()>0))
//    {
////    \\FlareDrum->EvalDerivs_AddQm_____(FlareDrum->Contents, *IOConduit(0));
//    FlareDrum->Contents.AddDeriv(*IOConduit(0), 1.0);
////CNM    FlareDrum->Flux.QAdd(IOFlange(0)->Q, som_ALL, 1.0);
//    }
  };

// --------------------------------------------------------------------------

void Flare::EvalDiscrete()
  {
  }

//==========================================================================

//
//                        Flash  Tank
//
//==========================================================================
/*#D:#T:FlashTank
#X:#n#h<General Description>#N#NThis tank is used to cause the incoming material to obtain
a state of equilibrium, so that the various component phases can be separated.

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed           Input           1              20#n
Liquid         Output          1              1#n
Vapour         Output          1              1#n
#a

#n#n#h<Variables to be supplied by the user>
#n#i<Z_Rqd :>The required datum of the flashtank.
#n#i<Rs :>The resistance of the flashtank (to do).
#n#i<Tau :>The damping constant of the flashtank.
#n#i<Flash :>This determines whether flashing will occur in the tank , 1 indicates that it occurs, 0
indicates that it does not occur. The default is 1
#n#i<FlashIn :>This determines whether the material is able to flash as it enters the flashtank  (to
do) , 1 indicates that it occurs, 0 indicates that it does not occur. The default is 0
#n#i<T_Rqd :>The required temperature of the material within the flashtank.
#n#i<P_Rqd :>The required pressure of the material within the flashtank.
#n#i<Ev_T :>The time in which evaporation occurs. The default is  5 sec
#n#i<Shape :>This allows the selection of the shape of container for the flashtank, at present only
one shape exists i.e. 'Vert, Cyl.' (a vertical cylinder).
#n#i<H :>The height of the container. The default is 1.0 m
#n#i<V :>The volume of the container. The default is 10.0 m^3
#n#i<Top :>The state of the top of the container, there are two options :
'Open' : the container is open to the atmosphere.
'Closed' : the container is sealed off from the atmosphere.
 The default is Open
#n#i<State :>This determines the manner in which the contents of the container exist, there are two
options :
'Mixed' : The component species are perfectly mixed to form a homogenous product.
'LvlInterface'. Each component specie separates out, resulting in a layering of the species. The default is
Mixed
#n#n#b<Model :>This allows the selection of the model that is be used to calculate how
the species are combined, there are two options :
#n'Mass Wt Mean' : A mass weighted mean is used.
#n'Light/Heavy Oil' : This uses relative density.
#n#i<T :>The temperature of the material within the flashtank. The default is 20 deg C

  #n#n#b<Species List :>This is a list of all the species, used in the particular flow engine, with their
current mass in kilograms. This is where the specie composition of the flashtank is determined. The
default, for each species, is 0 kg

#n#n#h<Variables calculated by the model>
#n#i<Z :>The height of the flashtank, in relation to the common datum, in metres.
#n#i<P :>The pressure of the material within the flashtank.
#n#i<Rho :>The density of the material within the flashtank.
#n#i<NdP :>The normalised pressure drop across the flashtank.
#n#i<P :>The pressure of the material within the flashtank.
#n#i<Lvl :> The level of material within the flashtank or the ratio of the total volume of the solids
and liquids within the flashtank to the volumetric capacity of the flashtank, expressed as a percentage.
#n#i<Note :> If the level exceeds 100%, the flashtank overflows and the material is lost to the system.
#n#i<Mt :>#NThe total mass of all the species within the flashtank.
#n#i<Rho :>#NThe density of the material within the flashtank.
#n#i<Sf :>#NThe percentage of solids within the flashtank.
#n#i<Lf :>#NThe percentage of liquids within the flashtank.
#n#i<Vf :>#NThe percentage of vapours within the flashtank.

#n#n#h<Other>#n
Default model prefix:FT#n
Short name:FlashT#n
Model type:Unit#n
#G:Units
*/

// To Improve
long Drw_FlashTank[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                         DD_End };

IMPLEMENT_MODELUNIT(FlashTank, "FlashT", "", Drw_FlashTank, "FT", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "FlashTank",
                    "FlashTank")



//--------------------------------------------------------------------------

FlashTank::FlashTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment  eAttach) :
  Tank(pClass_, TagIn, pAttach, eAttach)
  {
  Contents.SetClosed(True);
  };

//--------------------------------------------------------------------------

double FlashTank::mDewPt(pMdlNode This)
  {
  //  Y0 lb/MMscf
  //  OpPres kPa abs
  //  IF (OpPres > RZero) AND (Y0 > RZero) THEN
  //    IF ((27.248 - 0.8446 * LN (OpPres) - LN (Y0)) <> RZero) THEN
  //      GasOutletDP := (5214.4 - 38.34 * LN (OpPres)) /
  //                     (27.248 - 0.8446 * LN (OpPres) - LN (Y0));

  if (This->NoFlwIOs()==2)
    {
    int i= (This->IO_In(0) ? 0 : 1);
    return DewPt(Contents, *This->IOConduit(i), Contents.Press());//This->mPMean(NULL));
    }
  return dNAN();
  };

//--------------------------------------------------------------------------

double FlashTank::mH2oVVFrac(pMdlNode This)
  {
  if (This->NoFlwIOs()==2)
    {
    int i= (This->IO_In(0) ? 0 : 1);
    rSpConduit q=*This->IOConduit(i);
    SpMArray S;
    S[Water()]=1;
    double p=Contents.Press();//This->mPMean(NULL);
    return q.QVolume(S)/q.QVolume(som_SL);
//    return This->Contents.QVolume(q,S,p)/This->Contents.QVolume(q,som_SL,p);
    }
  return dNAN();
  };
//--------------------------------------------------------------------------

void FlashTank::BuildDataDefn(DataDefnBlk &DDB)
  {
  DDB.BeginStruct(this);
  DDB.Double ("Pressure",     "P",    DC_P,    "kPag",   xidPMean,        this, 0);
  DDB.Double ("Temperature",  "T",    DC_T,    "C",      xidTemp,        this, 0);
  DDB.Double ("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,       this, 0);
  DDB.Double ("Level",        "Lvl",  DC_Frac, "%",      xidLevel,     this, 0);

  DDB.Text   ("Datum");
  DDB.Double ("Datum",        "Z",    DC_L    , "m",    &dDatum,        this, isResult|0      | DYNMODE);
  DDB.Double ("Datum_Rqd",    "Z_Rqd",DC_L    , "m",    &dDatum_Rqd,    this, isParmStopped | DYNMODE|NAN_OK);

  DDB.Text   (" ");
  DDB.Double  ("POffset",      "",    DC_DP,    "kPa",     &POffset,          this, 0);

  DDB.Text    ("");
  DDB.CheckBoxBtn("Closed",    "",      DC_     , "",      xidClosed,      this,
    isParmStopped|(!gs_Environment.OK() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);
  DDB.CheckBoxBtn("Networked", "", DC_     , "",       xidNetPressDamp,       this,
    isParmStopped|(!gs_Environment.OK() || !Contents.Closed() ? noView|noFile|noSnap|noScenario:0) , DDBYesNo);

  VLE.Add_OnOff(DDB);

  VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
  if (SolveMode()==DYNMODE)
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.Page("IOs");
  Strng Commnt, Nm;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    Nm.Set("IO_%02i", i);
    DDB.Double(Nm(),     "",  DC_Qm , "kg/s",    xidIO_Qm0+i,       this, 0);
#ifndef UsingZTagObj
    DDB.VolatileTags(Nm(), Nm());
#endif

    Commnt.Set("%s", Nd_Rmt(i)->FullObjTag());
    DDB.TagComment(Commnt());
    }
  DDB.Double("IO_Sigma",    "",  DC_Qm , "kg/s",    xidIO_QmSigma,       this, 0);

  DDB.EndStruct();
  };

// -------------------------------------------------------------------------

flag FlashTank::ValidateData(ValidateDataBlk & VDB)
  {
  return Tank::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

void FlashTank::ConvergeStates()
  {
  Tank::ConvergeStates();
  }

//==========================================================================
//
//              Vapour / Liquid / Liquid Separator
//                         1G
//==========================================================================
/*#D:#T:VLL_Seperator
#X:#n#h<General Description>#N#NThis causes the incoming material to flash and in so doing
to separate into MEG and water, condensate and vapour.

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed         Input          1              20#n
Outlet       Output         1              20#n
BaseOut      Output         1              20#n
#a

#n#h<Variables to be supplied by the user>
#n#i<Z_Rqd :> The required datum of the VLL separator.
#n#i<Flash :>This determines whether flashing will occur in the tank , 1 indicates that it occurs, 0
indicates that it does not occur. The default is 1
#n#i<FlashIn :>This determines whether the material is able to flash as it enters the flashtank  (to
do) , 1 indicates that it occurs, 0 indicates that it does not occur. The default is 0
#n#i<T_Rqd :>The required temperature of the material within the VLL separator.
#n#i<P_Rqd :>The required pressure of the material within the VLL separator.
#n#i<Ev_T :>The time in which evaporation occurs. The default is  5 sec
#n#i<Shape :>This allows the selection of the shape of container for the flashtank, at present only
one shape exists i.e. 'Vert, Cyl.' (a vertical cylinder).
#n#i<H :>The height of the container. The default is 1.0 m
#n#i<V :>The volume of the container. The default is 10.0 m^3
#n#i<Top :>The state of the top of the container, there are two options :
#n'Open' : the container is open to the atmosphere.
#n'Closed' : the container is sealed off from the atmosphere.
#nThe default is Open
#n#i<State :>This determines the manner in which the contents of the container exist, there are two
options :
#n'Mixed' : The component species are perfectly mixed to form a homogenous product.
#n'LvlInterface'. Each component specie separates out, resulting in a layering of the species. The default is
Mixed
#n#i<Model :>This allows the selection of the model that is
be used to calculate how the species are combined, there are two options :
'Mass Wt Mean' : A mass weighted mean is used.
'Light/Heavy Oil' : This uses relative density.
#n#i<T :>The temperature of the material within the VLL separator. The default is 20 dC
#n#i<Species List :>This is a list of all the species, used in the particular flow engine, with their
current mass in kilograms. This is where the specie composition of the VLL separator is determined. The
default, for each species, is 0 kg

#n#n#h<Variables calculated by the model>
#n#i<Z :>The height of the VLL separator, in relation to the common datum.
#n#i<P :>The pressure of the material within the VLL separator.
#n#i<Rho :>The density of the material within the VLL separator.
#n#i<Lvl :>The level of material within the VLL separator or the ratio of the total volume of the
solids and liquids within the VLL separator to the volumetric capacity of the VLL separator, expressed as
a percentage. #p#b<Note :> If the level exceeds 100%, the VLL separator overflows and the material is
lost to the system.
#n#i<NdP :>The normalised pressure drop across the VLL separator.
#n#i<P :>The pressure of the material within the VLL separator.
#n#i<Mt :>The total mass of all the species within the VLL separator.
#n#i<Sf :>The percentage of solids within the VLL separator.
#n#i<Lf :>The percentage of liquids within the VLL separator.
#n#i<Vf :>The percentage of vapours within the VLL separator.

#n#n#h<Parameters  specifically associated with Boot>
#n#i<QFlash :>Whether the mass flow must take flashing into account or not, there are two options
'Yes' and 'No'. The default is No
#n#i<QmAcc :>The rate at which the mass of the material accumulates in the boot.
#n#i<QvAcc :>The rate at which the volume of the material accumulates in the boot.
#n#i<FTau :>The damping constant. The default is 1.0 sec
#n#i<Species List :>A list of all the species, used in the particular flow engine, showing each
specie's individual mass flow in the overflow of the boot.

#n#n#h<Other>#n
Default model prefix:VLL#n
Short name:VLLSep#n
Model type:Unit#n
#G:Units
*/

// To Improve
long Drw_VLL_Sep[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                     DD_End };

//--------------------------------------------------------------------------

static IOAreaRec VLL_SepIOAreaList[] =
  {{"Feed",                 "Feed"      , VLLIO_in,   LIO_In,    nc_MLnk, 1, 20},
   {"Outlet",               "Outlet"    , VLLIO_out,  LIO_Out,   nc_MLnk, 1, 20},
   {"BaseOut",              "BaseOut"   , VLLIO_boot, LIO_Out,   nc_MLnk, 1, 20},
   {NULL}};

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(VLL_Sep, "VLLSep", "", Drw_VLL_Sep, "VLL", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "VLL_Seperator",
                    "VLL_Seperator")

VLL_Sep::VLL_Sep(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlashTank(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, VLL_SepIOAreaList);
  BootVolume = 3.0;
  H2ORemEff  = 1.0;  // hss 20/01/97

  H2oSettled   = 0;
  dH2oSettled  = 0;
  MEGSettled   = 0;  // hss 12/01/2000
  dMEGSettled  = 0;
  TotalSettled = 0.0;

  H2OValidFlags = 0;
  pH2oSettled = new Integrator ("H2oSettled", H2oSettled, dH2oSettled, H2oSettled, H2OValidFlags);
  pMEGSettled = new Integrator ("MEGSettled", MEGSettled, dMEGSettled, MEGSettled, H2OValidFlags);

  ExtraH2oSettled = 0.0;

  pH2oSettled->SetStateAction(IE_Integrate);
  pMEGSettled->SetStateAction(IE_Integrate);

  };

//--------------------------------------------------------------------------

VLL_Sep::~VLL_Sep()
  {
  delete pH2oSettled;
  delete pMEGSettled;
  };

//--------------------------------------------------------------------------

void VLL_Sep::BuildDataDefn(DataDefnBlk &DDB)
  {
  DDB.BeginStruct(this);
  DDB.SuppressNextBlock();
  FlashTank::BuildDataDefn(DDB);
  DDB.Page    ("Boot", DDB_RqdPage);
  DDB.Double  ("",                    "Boot.IF",     DC_Frac, "%",       xidBH2oIf,    this, 0);
  DDB.Double  ("",                    "Boot.Volume", DC_Vol,  "m^3",     &BootVolume,  this, isParm);
  DDB.Double  ("",                    "Boot.Liq",    DC_M,    "kg",      &TotalSettled,this, (fDoDbgBrk ? isParm : 0)|InitHidden);
  //DDB.Double  ("H2ORemoveEfficiency","H2ORemEff",   DC_Frac, "%",       &H2ORemEff,   this, isParm);
  DDB.Double  ("RemovalEfficiency",   "RemEff",      DC_Frac, "%",       &H2ORemEff,   this, isParm);
  DDB.Double  ("H2oSettled",          "", DC_M, "kg", &H2oSettled,   this, isParmConstruct|InitHidden);
  DDB.Double  ("MEGSettled",          "", DC_M, "kg", &MEGSettled,   this, isParmConstruct|InitHidden);
  DDB.Text   (" ");
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------------

flag VLL_Sep::DataXchg(DataChangeBlk & DCB)
  {
  if (FlashTank::DataXchg(DCB))
    return 1;

  switch(DCB.lHandle)
    {
    case xidBH2oIf:
      //DCB.D = (H2oSettled*SDB[Water()].msVolume(Contents.Temp(), Contents.Press(), Contents.MArray()))/BootVolume;
      DCB.D = (H2oSettled*SDB[Water()].msVolume(Contents.Temp(), Contents.Press(), Contents.Ovr(), Contents.MArray())
              + MEGSettled*SDB[MEG()].msVolume(Contents.Temp(), Contents.Press(), Contents.Ovr(), Contents.MArray()))/BootVolume;
      return 1;
    }
  return 0;
  };

// -------------------------------------------------------------------------

flag VLL_Sep::ValidateData(ValidateDataBlk & VDB)
  {
  H2ORemEff = ValidateRange(VDB, "H2ORemEff", 0.0, H2ORemEff, 1.0);

  SetVLEMixFrac(H2oSettled);
  return FlashTank::ValidateData(VDB);
  };

// ---------------------------------------------------------------------------

flag VLL_Sep::InitialiseSolution()
  {
  switch (SolveMode())
    {
    case PBMODE:
      {
      pH2oSettled->SetStateAction(IE_Disabled);
      pMEGSettled->SetStateAction(IE_Disabled);
      break;
      }
    case SSMODE:
      {
      pH2oSettled->SetStateAction(IE_SaveState);
      pMEGSettled->SetStateAction(IE_SaveState);
      break;
      }
    case DYNMODE:
      {
      pH2oSettled->SetStateAction(IE_Integrate);
      pMEGSettled->SetStateAction(IE_Integrate);
      break;
      }
    }

  return FlashTank::InitialiseSolution();
  };

//--------------------------------------------------------------------------

void VLL_Sep::EvalProducts()
  {

  Contents.ZeroDeriv();
  QFeed().QZero();

  SigmaQIn(QFeed(), 0xffffffff);

  double  TQvIn=0.0;
  double  QvIn=0.0;
  for (int i=0; i<NoFlwIOs(); i++)
    if (IO_In(i))
      {
      QvIn=IOConduit(i)->QVolume(som_SL);
      TQvIn+=QvIn;
      }
  double  TQvInX=QFeed().QVolume(som_SL);

  /* hss I have included a variable to define the % of water removed in the boot.
     Previously it was 100%, which Andre Vogel says is incorrect.  The user now
     has the ability to set the efficiency at a lower value and hence allow some
     of the water to leave with the condensate. 20/01/97

     12/01/2000 hss I have included MEG in the boot contents, as it is also removed
     from the system via the boot. */

  SpMArray OutImg, BootImg;
  OutImg = Contents.MArray();
  OutImg[Water()] -= H2oSettled;
  OutImg[MEG()]   -= MEGSettled;
  SetProdMakeup(PMU_IOId | PMU_Image, VLLIO_out,  OutImg, Contents.Temp(), Contents.Press(), Contents.Model());

  // The Boot
  BootImg = Contents.MArray();
  double LoBLFScl=1.0e-6;
  double BootLiqFrac=(BootImg[Water()]+BootImg[MEG()])/GTZ(BootImg.Mass(som_SL));
  double BootLiqScl=1;
  // @BootLiqFrac==0 then this =1
  double OthrLiqScl=Range(0.0, 0.0+2*(LoBLFScl-BootLiqFrac)/LoBLFScl, 1.0);

  for (i=0; i<SDB.No(); i++)
    if (SDB[i].IsVap())
      BootImg[i] = 0;
    else if (i==Water() || i==MEG())
      BootImg[i] *= BootLiqScl;
    else
      BootImg[i] *= OthrLiqScl;

  SetProdMakeup(PMU_IOId | PMU_Image, VLLIO_boot, BootImg, Contents.Temp(), Contents.Press(), Contents.Model());
  /* OLD
  BootImg = Contents.MArray();
  BootImg[Water()] = 0.0;
  BootImg[MEG()] = 0.0;
  SetProdMakeup(PMU_IOId | PMU_Blocking, VLLIO_boot, BootImg, Contents.Temp(), Contents.Press(), Contents.Model());
  */
  EvalProducts_SurgeLevel(ContentHgtOrd);

  }

//--------------------------------------------------------------------------

void VLL_Sep::EvalDerivs()
  {
  FlashTank::EvalDerivs();

  double QmIn, QmOut, MegIn, MegOut;
  QmIn   = 0.0;
  QmOut  = 0.0;
  MegIn  = 0.0;
  MegOut = 0.0;
  for (int i=0; i<NoFlwIOs(); i++)
    {
    SpConduit & Cd=*IOConduit(i);
    if (IO_In(i))
      {
      QmIn  += Cd.Qm(Water())+Cd.Qm(Steam());
      MegIn += Cd.Qm(MEG());
      }
    else if (IO_Out(i) && IOId_Self(i)==VLLIO_boot)
      {
      QmOut  += Cd.Qm(Water())+Cd.Qm(Steam());
      MegOut += Cd.Qm(MEG());
      }
    }

  dH2oSettled = QmIn*H2ORemEff-QmOut;
  dMEGSettled = MegIn*H2ORemEff-MegOut;
  }

//--------------------------------------------------------------------------

void VLL_Sep::ApplyDerivs(double dTime, flag DoDbg)
  {
  FlashTank::ApplyDerivs(dTime, DoDbg);

  H2oSettled += dH2oSettled*dTime;
  H2oSettled  = Range(0.0 , H2oSettled, Contents.SpMass(Water()));

  MEGSettled += dMEGSettled*dTime;
  MEGSettled  = Range(0.0 , MEGSettled, Contents.SpMass(MEG()));

  TotalSettled = H2oSettled + MEGSettled;

  }

//--------------------------------------------------------------------------

void VLL_Sep::SetVLEMixFrac(double H2OSettled)
  {
  VLE.SetMixFrac(1.0, 1.0);
  VLE.SetMixFrac(Water.ci(), 1.0, 1.0-H2oSettled/GTZ(Contents.SpMass(Water())));
  }

//--------------------------------------------------------------------------

flag VLL_Sep::EvalPressureSens()
  {
  ExtraH2oSettled=0.0;
//  SetVLEMixFrac(H2oSettled);

  double BigDiff=0.0;

  // Must be closed and have contents
  if (!Contents.Closed() || Contents.Mass()< 1.0e-6)
    {
    ZeroVLSensitivities(0);
    return True;
    }

  Contents.SetHoldVentExpand();
  ContStartInfo Start(Contents);

//  ContStartInfo Start(C);
  // Vapour Sensitivity
  double QMassV=1.0;
  double DeltaFrac=Range(1.0e-6, 0.1*Max(1.0, Start.MassV)/NZ(fabs(QMassV)), 1.0);
  Contents.AddM(Contents, som_Vap, DeltaFrac);
  double AvgPSensV=(Contents.Press()-Start.Pres)/DeltaFrac;
  RestoreContents(Contents, Start);

  // Liquid Sensitivity
  double QMassL=1.0;
  DeltaFrac=Range(1.0e-6, 0.1*Max(1.0, Start.MassL)/NZ(fabs(QMassL)), 1.0);
  Contents.AddM(Contents, som_SL, DeltaFrac);
  double AvgPSensL=(Contents.Press()-Start.Pres)/DeltaFrac;
  RestoreContents(Contents, Start);

  SetEquilibratingV2L(0, Contents, Start);
  //SetVLSensitivities(0, Contents, Start);
  SetStaticHeadSensitivities(0, Contents, Start);
  // Flash to Equilibrium


  ContStartInfo Equil(Contents);

  double DeltaMass, Mv0, Mv1, P0, P1;
  JoinRec & J=Joins[0];
  for (int c=0; c<J.NConns(); c++)
    {
    int i=J.IONo(c);
    SpConduit &Cd=*IOConduit(i);

    double XH2OScl;
    if (IOId_Self(i)==VLLIO_boot)
      XH2OScl=1.0;
    else if (IO_In(i))
      XH2OScl=1.0-H2ORemEff;
    else
      XH2OScl=0.0;

    // Transfer to retain equil due to movement of vapour
    RestoreContents(Contents, Equil);

    DeltaMass=Max(1.0e-6, 0.001*Equil.MassV);
    P0=Contents.Press();
    Contents.AddM(Cd, som_Vap, DeltaMass);
    P1=Contents.Press();
    double PSensV=(fabs(P1-P0)>1.0e-6) ? (P1-P0)/DeltaMass : AvgPSensV;

    Mv0=Contents.Mass(som_Vap);
    ExtraH2oSettled=XH2OScl*Cd.Qm(Steam())*DeltaMass/GTZ(Cd.QMass(som_Vap));
    ConvergeStates();
    Mv1=Contents.Mass(som_Vap);
    double V2LQVFrac=(Mv0-Mv1)/DeltaMass;

    // Transfer to retain equil due to movement of liquor
    RestoreContents(Contents, Equil);

    DeltaMass=Max(1.0e-6, 0.001*Equil.MassL);
    P0=Contents.Press();
    Contents.AddM(Cd, som_SL, DeltaMass);
    P1=Contents.Press();
    //double PSensL=(P1-P0)/DeltaMass;
    double PSensL=(fabs(P1-P0)>1.0e-6) ? (P1-P0)/DeltaMass : AvgPSensL;

    Mv0=Contents.Mass(som_Vap);
    ExtraH2oSettled=XH2OScl*Cd.Qm(Water())*DeltaMass/GTZ(Cd.QMass(som_Liq));
    ConvergeStates();
    Mv1=Contents.Mass(som_Vap);

    double V2LQLFrac=(Mv0-Mv1)/DeltaMass;

    Set_IO_PSensVL(i, PSensV, PSensL);
    Set_IO_V2LQmFact(i, V2LQVFrac, V2LQLFrac);

//    dbgpln("     V2L  V:%14.6g L:%14.6g  %14.6g %14.6g  %s",
//      V2LQVFrac, V2LQLFrac, XH2OScl, ExtraH2oSettled, Nd_Rmt(i)->FullObjTag());
    }

  RestoreContents(Contents, Start);
  ExtraH2oSettled=0.0;

  Contents.ClrHoldVentExpand();

  return true;//False;
  };

//--------------------------------------------------------------------------

void VLL_Sep::ConvergeStates()
  {
  SetVLEMixFrac(H2oSettled+ExtraH2oSettled);
  FlashTank::ConvergeStates();
  }

//==========================================================================
//
//                    Vapour / Liquid  Separator
//
//==========================================================================
/*#D:#T:VL Seperator
#X:#n#u<#l<#b<General Description>>>#N#NThis causes the incoming material to flash and in so doing
to separate into liquid and vapour. The model requires an inlet, an outlet and a base outlet and can
accommodate a maximum of 20 of each.
#n#n#u<#l<#b<Parameters  to be supplied by the user>>>#n
#n#b<Z_Rqd :>#n The required datum of the VL separator, in metres.
#n#b<Rs :>#n The resistance of the flashtank (to do).
#n#b<Tau :>#n The damping constant of the flashtank, in seconds.
#n#n#b<Flash :>#NThis determines whether flashing will occur in the tank , 1 indicates that it occurs, 0
indicates that it does not occur. The default is 1
#n#n#b<FlashIn :>#NThis determines whether the material is able to flash as it enters the flashtank  (to
do) , 1 indicates that it occurs, 0 indicates that it does not occur. The default is 0
#n#b<T_Rqd :>#n The required temperature of the material within the VL separator, in degrees Celsius.
#n#b<P_Rqd :>#n The required pressure of the material within the VL separator, in kiloPascals (gauge).
#n#b<Ev_T :>#n The time in which evaporation occurs, in seconds. The default is  5 sec
#n#n#b<Shape :>#NThis allows the selection of the shape of container for the flashtank, at present only
one shape exists i.e. 'Vert, Cyl.' (a vertical cylinder).
#n#b<H :> The height of the container, in metres. The default is 1.0 m
#n#b<V :> The volume of the container, in cubic metres. The default is 10.0 m^3
#n#b<Top :> The state of the top of the container, there are two options :
'Open' : the container is open to the atmosphere.
'Closed' : the container is sealed off from the atmosphere.
 The default is Open
#n#b<State :> This determines the manner in which the contents of the container exist, there are two
options :
'Mixed' : The component species are perfectly mixed to form a homogenous product.
'LvlInterface'. Each component specie separates out, resulting in a layering of the species. The default is
Mixed
#n#n#b<Model :>#NThis allows the selection of the model that is
be used to calculate how the species are combined, there are two options :
'Mass Wt Mean' : A mass weighted mean is used.
'Light/Heavy Oil' : This uses relative density.
#n#b<T :>#n The temperature of the material within the VL separator, in degrees Celsius. The default is
20 dC
#n#n#b<Species List :>#nThis is a list of all the species, used in the particular flow engine, with their
current mass in kilograms. This is where the specie composition of the VL separator is determined. The
default, for each species, is 0 kg
##n#n#u<#l<#b<Parameters  associated with the VL separator>>>#n
#n#n#b<Tag :>#NThe unique name given to the VL separator, by the user,
to be able to refer to and identify the VL separator.
#n#n#b<Parent :>#NThe flow engine in which the VL separator is being used.
#n#n#b<Type :>#NThis refers to the name of the model being used
to simulate the VL separator.
#n#n#b<Z :>#NThe height of the VL separator, in relation to the common datum, in metres.
#n#n#b<P :>#NThe pressure of the material within the VL separator,
in kiloPascals (gauge).
#n#n#b<Rho :>#NThe density of the material within the VL separator, in kilograms per cubic metre.
#n#n#b<Lvl :>#N The level of material within the VL separator or the ratio of the total volume of the
solids and liquids within the VL separator to the volumetric capacity of the VL separator, expressed as
a percentage. #p#b<Note :> If the level exceeds 100%, the VL separator overflows and the material is
lost to the system.
#n#n#b<NdP :>#NThe normalised pressure drop across the VL separator, in kiloPascals (gauge).
#n#n#b<P :>#NThe pressure of the material within the VL separator,
in kiloPascals (gauge).
#n#n#b<Mt :>#NThe total mass of all the species within the
VL separator, in kilograms.
#n#n#b<Sf :>#NThe percentage of solids within the VL separator.
#n#n#b<Lf :>#NThe percentage of liquids within the VL separator.
#n#n#b<Vf :>#NThe percentage of vapours within the VL separator.
##n#n#u<#l<#b<Parameters  specifically associated with Boot>>>#n
#n#n#b<QFlash :>#NWhether the mass flow must take flashing into account or not, there are two options
'Yes' and 'No'. The default is No
#n#n#b<QmAcc :>#nThe rate at which the mass of the material accumulates in the boot, in tons per hour.
#n#n#b<QvAcc :>#nThe rate at which the volume of the material accumulates in the boot, in cubic metres
per hour.
#n#n#b<FTau :>#NThe damping constant in seconds. The default is 1.0 sec
#n#n#b<Species List :>#nA list of all the species, used in the particular flow engine, showing each
specie's individual mass flow in the overflow of the boot, in kilograms per second.
*/

// To Improve
long Drw_VL_Sep[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                      DD_End };


IMPLEMENT_MODELUNIT(VL_Sep, "VLSep", "", Drw_VL_Sep, "VL", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "VL Seperator",
                    "Vapour Liquid Seperator")


//--------------------------------------------------------------------------

static IOAreaRec VL_SepIOAreaList[] =
  {{"Feed",                   "Feed"      , 0, LIO_In,    nc_MLnk, 0, 20},
   {"Outlet",                 "Outlet"    , 2, LIO_Out,   nc_MLnk, 0, 20},
   {NULL}};

//--------------------------------------------------------------------------

VL_Sep::VL_Sep(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment
eAttach) :
  FlashTank(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, VL_SepIOAreaList);
  };

//--------------------------------------------------------------------------

//CNM void VL_Sep::EvalProducts()
//CNM   {
//CNM   Contents.ZeroDeriv();
//CNM   QFeed().QZero();
//CNM   SigmaQIn(QFeed(), 0xffffffff);
//CNM   EvalProducts_SurgeLevel(Contents, QFeed(), 0xffffffff);
//CNM   };

//--------------------------------------------------------------------------

//CNM void VL_Sep::EvalDerivs()
//CNM   {
//CNM   FlashTank::EvalDerivs();
//CNM   };

