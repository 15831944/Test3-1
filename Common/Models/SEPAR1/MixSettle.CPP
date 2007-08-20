// SysCAD Copyright Kenwalt (Pty) Ltd 1995
 
#include "stdafx.h"
#include "sc_defs.h"
#define  MIXSETTLE_CPP
#include "mixsettle.h"
#include "sr_eqn.h"
#include "sr_ctrl.h"
//#include "optoff.h"

//==========================================================================
/*#D:#T:Solvent Extraction Mixer-Settler model. 
#X:#n#u<#l<#b<General Description>>>#N#NThe  Mixer-Settler model is designed for use as
a solvent extraction model which can also be configured to do organic stripping. It 
consists conceptually of two containers. One can be considered the mixer tank and the 
other the settler tank. The Mixer tank can accept two organic inputs and two aqueous 
inputs. At least one of each is required. Overflow from the Mixer to the Settler is 
accomplished by an internal stream called #i<MxrOFlow >. The over flow height is a 
configurable variable. #pThe Settler receives input from the #i<MxrOFlow > stream, 
accumulates the liquid phases and overflows into an aqueous and organic outlet stream. 
Up to two of each of these is permitted.  The Settler oulet streams are configured at 
a default connection height of 75% of the Settlers height.  Control of the flowrates 
from the Settler organic and aqueous outlet streams is up to the user.  The model ensures 
only that aqueous phases leave via the aqueous stream and organic phases leave through 
the organic stream.

The user is responsible for setting up a suite of species which accurately portrays the 
application he is evaluating.e.g. #i< H2O, H2SO4, Cu, Fe, CuSO4, FeSO4, Fe2{SO4}3, 
Kerosene, RH, R2Cu, R2Fe, R3Fe >. This suite of species is typically set up for chelating 
type of organic solvents such as the LIX hydroxy oxime copper extractants available from 
the Henkel Corporation. i.e. LIX 64N, 65N, 860, 864, 865, and 622.
#pThe model is capable of the following :
#p1. Based on a user specified separating density it is able to destinguish between
     an organic and an aqueous phase. The aqueous species being denser than the separating 
     density and the organic species less dense.  e.g. #i<RhoSep 999.0 kg/m^3 >
#p2. The user must identify a primary and optionally a secondary (undesired or contaminant) 
     metallic element of interest, that is to be extracted. (Full functionality of secondary 
     metal extraction has not yet been implemented). e.g. #i<Primary Metal 'Cu' >
#p3. The user must supply a reaction which utilises the species he has specified to be able
     to extract the metal of interest. e.g. #i< 2RH(l) + CuSO4(aq) = R2Cu(l) + H2SO4(l) >. 
     Or in the case of stripping #i<R2Cu(l) + H2SO4(l) = 2RH(l) + CuSO4(aq)  >
#p4. The model manages the flow of liquids into the Mixer and into the Settler and is 
     dependent upon favourable pressure gradients in the outlet pipes from the settler to 
     allow the flow of aqueous and organic phase from the Settler.
#p5. The model determines the concentration of the primary metal (in this case #i<Cu >) in 
     the streams entering the Mixer as well as the current concentration of the primary 
     metal within the Mixer. The extraction (or stripping) isotherm provided by the user 
     determines the required primary metal concentration within the aqueous and organic 
     phases.The model then calculates the extent to which the defined reaction should 
     proceed to give the desired concentrations.
#p6. The ratio of the organic phase volume flow to the aqueous phase volume flow within 
     the Mixer also fulfills a crucial role in the equilibrium extraction calculation. It 
     provides the slope of the McCabe-Thiele operating line (a straight line). The 
     Y-intercept being provided by the user as a variable called #i<MinOrgConc > in the 
     case of extraction, and #i<MinAqsConc > in the case of stripping.
#p7. The model is not currently sensitive to pH and assumes that the pH is correct for a 
     paticular equilibrium isotherm.


#n#n#u<#l<#b<Variables to be configured before use>>>#N#NImportant variables that should 
  be configured before use are the following:
#n#n#b<Mixer.H :>#nThe height of the mixer tank.
#n#n#b<Mixer.V :>#nThe volume of the mixer tank.
#n#n#b<Settler.H :>#nThe height of the settler tank. Generally the same as the Mixer height.
#n#n#b<Settler.V :>#nThe volume of the settler tank.
#n#n#b<#i<MxrOF_Level :>>#nThe mixer overflow level. The default is 85%.
#n#n#b<#i<RhoSep :>>#nThe aqueous/organic phase separating density. Default is 999.0 kg/m^3.
#n#n#b<#i<Mode :>>#nEither #i<Extraction > or #i<Stripping >.
#n#n#b<#i<MinOrgConc :>>#nThe minimum organic concentration (y-intercept) in the case of 
  extraction.
#n#n#b<#i<MinAqsConc :>>#nThe minimum aqueous concentration (y-intercept) in the case of 
  stripping.
#n#n#b<#i<Primary Metal :>>#nThe primary metallic element of interest.
#n#n#b<#i<Secondary Metal :>>#nThe secondary metallic element (contaminant) of interest.


#n#n#b<#i<Reaction_file :>>#NThe name of the file containing the reaction equation. The 
  file extension should be ".rct". e.g. #i<CUSEX.RCT>.
#n#n#b<#i<Reaction_On/Off :>>#NThe default is #b<On>. When setting the model up for the 
  first time, after entering and accepting a reaction file, the user should ensure this is 
  #b<On >  or else the reaction will not be functional.
#n#n#b<#i<LMS_On/Off :>>#nTurns the Least Mean Squared convergence algorithm on/off. The 
recommended state is On.
#n#n#b<#i<LMS_Beta :>>#nThe recommended beta value is 0.01. If too great convergeance may 
ocillate.
#n#n#b<#i<Isotherm :>>#nEnsure that a manufacturers solvent extraction/stripping isotherm 
is configured. Use of the Cubic-Spline data fitting equation is recommended.


#n#n#u<#l<#b<Output Variables>>>#N#NAll the shaded variables are outputs from the 
converter model and provide a view of the models performance.
#n#n#b<#i<Aqs_In_Conc :>>#NThe concentration of the (primary) metallic element of interest 
in the aqueous phase entering the mixer.
#n#n#b<#i<Org_In_Conc :>>#NThe concentration of the (primary) metallic element of interest 
in the organic phase entering the mixer.
#n#n#b<#i<Aqs_Out_Conc :>>#NThe concentration of the (primary) metallic element of interest 
in the aqueous phase leaving the settler.
#n#n#b<#i<Org_Out_Conc :>>#NThe concentration of the (primary) metallic element of interest 
in the organic phase leaving the settler.
#n#n#b<#i<OARatio :>>#nThe ratio of the volume flow of organic phase to aqueous phase 
within the mixer.
#n#n#b<#i<LMS_Err :>>#nThe Least Mean Squared convergence error. It should be a very small 
number.

*/
//=================================================================================================

const byte idMx  = 31;

const int ioidAqIn     = 0;
const int ioidAqOut    = 1;
const int ioidOrgIn    = 2;
const int ioidOrgOut   = 3;
const int ioidOverflow = 4;

static IOAreaRec MixerSettlerIOAreaList[] =
  {
    {"Aqueous_Inlet",   "AqIn"      , ioidAqIn,      LIO_In0 ,   nc_MLnk, 1, 20, IOOptsHide|IOGRP(1),             0.0f  },
    {"Aqueous_Outlet",  "AqOut"     , ioidAqOut,     LIO_Out0,   nc_MLnk, 1,  1, IOOptsHide|IOGRP(1)|IOSSDenied,  0.75f },
    {"Organic_Inlet",   "OrgIn"     , ioidOrgIn,     LIO_In1,    nc_MLnk, 1, 20, IOOptsHide|IOGRP(1)|IOSSDenied,  0.0f  },
    {"Organic_Outlet",  "OrgOut"    , ioidOrgOut,    LIO_Out1,   nc_MLnk, 1,  1, IOOptsHide|IOGRP(1),             0.75f },
    {"OverFlow",        "OverFlw"   , ioidOverflow,  LIO_Out,    nc_MLnk, 0, 10, IOOptsHide|IOGRP(1),             1.0f  },
    {NULL}
  };

static double Drw_MixerSettler[] = { DD_Poly, -30,6, -30,-5, 28,-5, 28,5, -17,5, -17,-5, -17,6, -30,6,
                                    DD_Poly, -23,10, -24,10, -24,-2, -26,-2, -26,-4, -21,-4, -21,-2, -23,-2, -23,10,
        					                  DD_Poly, -23,-2, -24,-2,
        					                  DD_Poly, -25,-2, -25,-4,
         					                  DD_Poly, -24,-2, -24,-4,
        					                  DD_Poly, -23,-2, -23,-4,
         					                  DD_Poly, -22,-2, -22,-4,
        					                  DD_Poly,  28,2, 30,2, 30,5,
        					                  DD_Poly,  28,-5, 30,-5, 30,-3, 28,-3,
                                    DD_TagPos, 0, -8.5,
         					                  DD_End }; //This provides the default drawing of the MixerSettler.

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(MixerSettler, "SolvExtr", "1", Drw_MixerSettler, "Tank", "MXS", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Separation:Mixer/Settler",   
                    "Solvent Extraction Mixer Settler") 

//===========================================================================

MixerSettler::MixerSettler(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) : 
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  Mixer("Mixer", this, TOA_Embedded), 
  Settler("Settler", this, TOA_Embedded), 
  QMxInEst("QMxInEst", this, TOA_Embedded),
  QMxOFlow("MxrOFlow", this, TOA_Embedded),
  PriIsotherm("DataOnly", "Isotherm", this , TOA_Embedded),
//  SecIsotherm("DataOnly", "IsothermII", this , TOA_Embedded),
  RB(this, false)
  {
  AttachClassInfo(nc_Process,NULL);
  AttachIOAreas(MixerSettlerIOAreaList);
  Mixer.SetStateAction(IE_Integrate);
  Settler.SetStateAction(IE_Integrate);
  SetAllowedModes(true, NM_All|SM_Direct|/*SM_Inline|*//*SM_Buffered|*/HM_All);

  MixOFLev    = 0.85;
  MxResT      = 1.0;
  StResT      = 1.0;
  QmAcc       = 0.0;
  QvAcc       = 0.0;

  RhoSep      = 950.0;
  SEff        = 1.0;
  AqsInConc   = 0.0;
  OrgInConc   = 0.0;
  AqsOutConc  = 0.0;
  OrgOutConc  = 0.0;
  AqLoss      = 0.0;
  OgLoss      = 0.0;
  OARatio     = 0.0;

  Extraction  = True;
  ReactionsOn = True;
  DoLMS       = False;
  PriMetal    = "";
  SecMetal    = "";
  iMethod     = SVM_Isotherms;
  iSol        = 0;
  MinOrgConc  = 0.0;
  MinAqsConc  = 0.0;
  IntersectPt1= 0.0;
  IntersectPt2= 0.0;
  dReactExt   = 0.1;
  MaxLoadedConc= 0.0;
  MaxStripConc = 0.0;

  Mixer.SetClosed(False);
  Settler.SetClosed(False);

  PriIsotherm.SetLength(7);
  PriIsotherm.SetPt(0, 0.0001, 0.0001);
  PriIsotherm.SetPt(1, 0.5, 5.0);
  PriIsotherm.SetPt(2, 1.0, 12.5);
  PriIsotherm.SetPt(3, 2.0, 17.5);
  PriIsotherm.SetPt(4, 4.0, 21.0);
  PriIsotherm.SetPt(5, 8.0, 22.5);
  PriIsotherm.SetPt(6, 16.0, 23.5);

  //SecIsotherm.SetLength(2);
  //SecIsotherm.SetPt(0,0.0,0.0);
  //SecIsotherm.SetPt(1,1.0,1000.0);

  Aqueous.Zero();
  Organic.Zero();

  LMS1.Beta   = 0.01;
  LMS1.Weight = 1.0;
  LMS1.Err    = 0.0;
  LMS1.Out    = 0.0;
  LMS1.RqdOut = 0.0;
  LMS1.Input  = 0.0;
  LMS1.NewOut = 0.0;

  }; 

//----------------------------------------------------------

/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void MixerSettler::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_Buffered|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Double  ("Pressure",        "P",      DC_P,    "kPag",   xidPMean,       this, 0);
  DDB.Double  ("Temperature",     "T",      DC_T,    "C",      xidTemp,        this, 0);
  DDB.Double  ("Density",         "Rho",    DC_Rho,  "kg/m^3", xidRho,         this, 0);
  DDB.Double  ("Level",           "Lvl",    DC_Frac, "%",      xidLevel,       this, 0);
  DDB.Double  ("Mass_Accum",      "QmAcc",  DC_Qm,   "kg/s",   &QmAcc,         this, 0);
  DDB.Double  ("Vol_Accum",       "QvAcc",  DC_Qv,   "L/s",    &QvAcc,         this, 0);
  DDB.Double  ("MixerResidence",  "MxResT", DC_Time, "min",    &MxResT,        this, 0);
  DDB.Double  ("SettlerResidence","StResT", DC_Time, "min",    &StResT,        this, 0);
  DDB.Double  ("MxrOF_Level",     "",       DC_Frac ,"%",      &MixOFLev,      this, isParm);
  DDB.Visibility();

  DDB.Text    ("");
  DDB.Double  ("Sep_Density",     "RhoSep",       DC_Rho,  "kg/m^3", &RhoSep,     this, isParm);
	DDB.Double  ("Aqueous_Losses",  "AqLoss",       DC_Frac, "%",      &AqLoss,     this, isParm);
	DDB.Double  ("Organic_Losses",  "OgLoss",       DC_Frac, "%",      &OgLoss,     this, isParm);

  DDB.Text    ("Feed Conditions");
  DDB.Double  ("AqueousConcIn",   "AqsConcIn",    DC_Conc, "g/L",    &AqsInConc,  this, 0);
  DDB.Double  ("OrganicConcIn",   "OrgConcIn",    DC_Conc, "g/L",    &OrgInConc,  this, 0);
  DDB.Double  ("Organic/Aqueous", "OARatio",      DC_,     "",       &OARatio,    this, 0);

  DDB.Text    ("Product Conditions");
  DDB.Double  ("AqueousConcOut",  "AqsConcOut",   DC_Conc, "g/L",    &AqsOutConc, this, 0);
  DDB.Double  ("OrganicConcOut",  "OrgConcOut",   DC_Conc, "g/L",    &OrgOutConc, this, 0);
  DDB.Text    ("");

  static DDBValueLst DDB3[]={
    {SVM_Isotherms,  "Isotherm"},
    {SVM_Reactions,  "None"},
    {0}};
  DDB.Byte    ("ExtractionMethod", "Method",      DC_,     "",       &iMethod,    this, isParm|SetOnChange, DDB3);
  
  DDB.Visibility(NSHM_All, iMethod==SVM_Isotherms);
	DDB.Text    ("");
	static DDBValueLst DDB0[]={
		{(int)True,  "Extraction"},
		{(int)False, "Stripping" },
		{0}};
	DDB.Bool    ("Mode",          "",           DC_,      "",    &Extraction, this, isParm, DDB0);

	DDB.Double  ("Stage_Eff",     "StageEff",   DC_Frac,  "%",   &SEff,       this, isParm);
	DDB.Byte    ("Ext_Metal",     "Solute?",    DC_,      "",    &iSol,       this, isParm, SDB.DDBLiqSpList());
  DDB.Visibility(NSHM_All, iMethod==SVM_Isotherms && Extraction);
  DDB.Double  ("",              "MinOrgConc", DC_Conc, "g/L",  &MinOrgConc, this, isResult);
  DDB.Visibility(NSHM_All, iMethod==SVM_Isotherms && !Extraction);
  DDB.Double  ("",              "MinAqsConc", DC_Conc, "g/L",  &MinAqsConc, this, isResult);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, iMethod==SVM_Isotherms);
#ifndef _RELEASE
  DDB.Double  ("",              "IntersectPt1", DC_Conc, "g/L",  &IntersectPt1, this, isResult|InitHidden);
  DDB.Double  ("",              "IntersectPt2", DC_Conc, "g/L",  &IntersectPt2, this, isResult|InitHidden);
#endif
  DDB.Double  ("",              "ReactExtent", DC_Frac, "%",     &dReactExt,  this, isResult);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, iMethod==SVM_Reactions || (iMethod==SVM_Isotherms && Extraction));
  DDB.Double  ("",              "MaxLoadedConc", DC_Conc, "g/L", &MaxLoadedConc, this, isResult);
  DDB.Visibility(NM_Probal|SM_All|SM_Inline|HM_All, iMethod==SVM_Reactions || (iMethod==SVM_Isotherms && !Extraction));
  DDB.Double  ("",              "MaxStripConc", DC_Conc, "g/L",  &MaxStripConc, this, isResult);
  DDB.Visibility();

	DDB.Text    ("");
	DDBValueLstMem DDB1;
	DDB1.Empty();
	Strng S; 
	for (long k=0;k<EDB.Count() ;k++)
		{
		pchar El=EDB[k].m_Name;
		if (El)
			{
			S=El;
			DDB1.Add(S());
			}
		else
			break;
		}
	DDB.String("Primary_Metal",         "", DC_,         "",     &PriMetal,  this, isParm, DDB1());

	/*DDBValueLstMem DDB2;
	DDB2.Empty();
	Strng T;
	for (long j=0;j<EDB.Count() ;j++)
		{
		pchar El;
		if (j==0)
			El="None";
		else
			El=EDB[j].Name;
		if (El)
			{
			T=El;
			DDB2.Add(T());
			}
		else
			break;
		}
	DDB.String("Secondary_Metal",         "", DC_,         "",     &SecMetal,  this, isParm, DDB2());*/

	DDB.Text    ("");
  RB.Add_OnOff(DDB);

  DDB.Visibility(SM_Buffered|HM_All);
	DDB.Text    ("");
  DDB.Bool    ("LMS_On/Off", "", DC_,  "", &DoLMS,       this, isParm, DDBOnOff);
  DDB.Double  ("LMS_Beta",   "", DC_,  "", &LMS1.Beta,   this, isParm);
  DDB.Double  ("LMS_Weight", "", DC_,  "", &LMS1.Weight, this, isParm);
  DDB.Double  ("LMS_Err",    "", DC_,  "", &LMS1.Err,    this, 0);
  DDB.Visibility();

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  RB.BuildDataDefn(DDB);

  DDB.Visibility(NSHM_All, iMethod==SVM_Isotherms);
  DDB.Object(&PriIsotherm, this, NULL, NULL, DDB_RqdPage); //(pTaggedObject)
  //if ((SecMetal.Str() != NULL)&&(SecMetal.Find("None")<0))
  //  DDB.Object(&SecIsotherm, this, NULL, NULL, DDB_RqdPage); //(pTaggedObject)
  DDB.Visibility();

  if (SolveBufferedMethod())
    {
    DDB.Visibility(SM_Buffered|HM_All);
	  DDB.Object(&Mixer,       this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&Settler,     this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&QMxOFlow,    this, NULL, NULL, DDB_RqdPage);
    DDB.Visibility();
    }

  DDB.EndStruct();
 
  };

// -------------------------------------------------------------------------

MixerSettler::~MixerSettler()
  {  
  };

// -------------------------------------------------------------------------

flag MixerSettler::DataXchg(DataChangeBlk & DCB)
  {
	if (RB.DataXchg(DCB))
    return 1;

  if (MdlNode::DataXchg(DCB)) 
    return 1;
  
  return False;
  }

//----------------------------------------------------

flag MixerSettler::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK= MdlNode::ValidateData(VDB);
  Mixer.ValidateData(VDB);
  Settler.ValidateData(VDB);
  return OK;
  }

// ---------------------------------------------------------------------------

flag MixerSettler::InitialiseSolution()
  {
  switch (SolveMethod())
    {
    case SM_Direct:
    case SM_Inline:
      Mixer.SetStateAction(IE_Disabled);
	    Settler.SetStateAction(IE_Disabled);
      break;
    case SM_Buffered:
      Mixer.SetStateAction(IE_Integrate);
	    Settler.SetStateAction(IE_Integrate);
      break;
    }

  return 1;
  };

//-------------------------------------------------------------------------- 

static const IDMask MMsk=Id_2_Mask(ioidAqIn)|Id_2_Mask(ioidOrgIn);
static const IDMask SMsk=Id_2_Mask(ioidAqOut)|Id_2_Mask(ioidOrgOut)|Id_2_Mask(ioidOverflow);

void MixerSettler::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  CSetDatumsData SDD[]=
    {
      {MMsk, &Mixer},
      {SMsk, &Settler},
      {0}
    };
  SetDatums_Node(Pass, List, IOIn, SDD);
  };
 
//--------------------------------------------------------------------------

void MixerSettler::SetDatumsDone()
  {
  MdlNode::SetDatumsDone();
  SortSurgeIOData SDD[]=
    {
      { MMsk,    &Mixer,   NULL/*ContentHgtOrd*/  },
      { SMsk,    &Settler, NULL },
      { 0},
    };
  //SortSurgeIO(SDD);
  //TODO SortSurgeIOData must be rewitten to new specs
  };

//-------------------------------------------------------------------------- 

flag MixerSettler::Set_Sizes()
  {
  return True;
  };

//---------------------------------------------------------------------------

void MixerSettler::ConfigureJoins()
  {
  int i;
  if (NetMethod()==NM_Probal || SolveMethod()!=SM_Buffered)
    {
    for (i=0; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0);
    /*for (i=0; (i<NoFlwIOs()) && (IOId_Self(i)<idOrgIn); i++)
      SetIO_Join(i, idAqIn);
	  for (i=0; (i<NoFlwIOs()) && (IOId_Self(i)<idOverflow) && (IOId_Self(i)>idAqOut); i++)
      SetIO_Join(i, idOrgIn);*/
    }
  else
    {
    for (i=0; (i<NoProcessIOs()) && (IOId_Self(i)<idMx); i++)
      SetIO_Open(i, 0, false, ESS_Allowed);
    for (  ; i<NoProcessIOs(); i++)
      SetIO_Join(i, 0);
    }
  };  

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/ 

void MixerSettler::EvalJoinPressures(long JoinMask)
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
      {
      double Pm = (Mixer.Closed() ? Mixer.Press() : AtmosPress());
	    double Pms = (Settler.Closed() ? Settler.Press() : AtmosPress());
      for (int i = 0; (i<NoFlwIOs()) && (IOId_Self(i)<idMx); i++)
        {
        ASSERT(IOFlange(i)!=NULL);
		    if ((i==ioidAqIn) || (i==ioidOrgIn))
		      Set_IOP_Self(i,Pm + Gc_Kpa * Max((Mixer.Level() - IOAbsFracHgt_Term(i, Mixer)), 0.0) * Mixer.ShapeHeight() * Mixer.Rho(som_SL));
		    else
		      Set_IOP_Self(i,Pms + Gc_Kpa * Max((Settler.Level() - IOAbsFracHgt_Term(i, Settler)), 0.0) * Settler.ShapeHeight() * Settler.Rho(som_SL));
        }
      break;
      }
    }

  }; 

 //--------------------------------------------------------------------------

void MixerSettler::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  };
 
 //--------------------------------------------------------------------------

void MixerSettler::EvalSteadyState()
  {
  };

//--------------------------------------------------------------------------

/* Modifies the private method MeasureRqdFlows found in M_Base.cpp. Called locally by 
   overridden method EvalProducts_SurgeLevel   */
//TODO MeasureRqdFlows must be rewitten to new specs
//#if CNMTOREMOVE
//void MixerSettler::MeasureRqdFlows(SpContainer &Contents, double SLEVDensity, double SLEVSLfrac,
//                              HgtOrdInfo & Info, 
//                              int io0, int ioN, 
//                              double & IoLvl,
//                              double & QOutMRqd, double & QOutVRqd, 
//                              double & QOutMAvl, double & QOutVAvl)
////void MixerSettler::MeasRqdFlows(rSpContainer M, 
////                 HgtOrdInfo &Info, int io0, int ioN, double & QOutMRqd, double & QOutVRqd)
//  {
//  double SLRho = Contents.Rho(som_SL);
//  QOutMRqd = 0.0;
//  QOutVRqd = 0.0;
//  for (int k=io0; k<ioN; k++)
//    {
//    int i = Info.Hgt[k];
//    QOutMRqd += IOQmEst_Out(i);
//    if (IO_Self(i).IsPMUImage())
//      QOutVRqd += IOQmEst_Out(i)/Min_FE_Rho(IOConduit(i)->Rho(som_SL));
//    else if (IO_Self(i).IsPMUPassing() || IO_Self(i).IsPMUBlocking())
//      {
//      SpMArray Filt(*(IOConduit(i)->Model())), Mass(*(Contents.Model()));
//      Filt.SetToOnesAndZeros(IO_Self(i).IsPMUBlocking());
//      Mass *= Filt;
//      QOutVRqd += IOQmEst_Out(i)/Min_FE_Rho(Mass.Rho(som_SL, Contents.Temp(), Contents.Press(), Contents.Ovr()));
//      }
//    else
//      QOutVRqd += IOQmEst_Out(i)/Min_FE_Rho(SLRho);
//    }
//  }
//
//
////--------------------------------------------------------------------------
//
///* Modifies the private method SetOutputFlows found in M_Base.cpp. Called locally by 
//   overridden method EvalProducts_SurgeLevel */
//
//double MixerSettler::SetOutFlows(rSpContainer M, 
//                             HgtOrdInfo &Info, int io0, int ioN, double SFrac, double VFrac)
//  {
//  double QvActual_SL = 0.0;
//  for (int k=io0; k<ioN; k++)
//    {
//    double Qs,Qv;
//    char i = Info.Hgt[k];
//    rSpConduit Fo = *IOConduit(i);
//    if (IO_Self(i).IsPMUImage())
//      {
//      IO_Self(i).SetPMUImage(False);
//      Qv = VFrac * IOQmEst_Out(i);
//      Qs = SFrac * IOQmEst_Out(i);
//      M.QOutEst().ChangeModel(M.pModel->ClassId());
//      M.QOutEst().QCopy(Fo);
//	    M.QOutEst().SetTemp(M.Temp());
//      if (M.Mixed())
//        {
//        // Level is ignored
//        Fo.QSetM(M.QOutEst(), som_ALL, Qs+Qv, IOP_Self(i));
//        }
//      else
//        {
//        Fo.QSetM(M.QOutEst(), som_Gas, Qv, IOP_Self(i));
//        Fo.QAddM(M.QOutEst(), som_SL,  Qs);
//        }
//       } 
//      else if (IO_Self(i).IsPMUPassing() || IO_Self(i).IsPMUBlocking())
//      {
//      SpMArray FiltSL(*(IOConduit(i)->Model()));
//      FiltSL.SetToOnesAndZeros(IO_Self(i).IsPMUBlocking());
//      SpMArray FiltVap(FiltSL);
//      FiltSL.ZeroPhase(som_Gas);
//      FiltVap.ZeroPhase(som_SL);
//
//      Qv = VFrac * IOQmEst_Out(i);
//      Qs = SFrac * IOQmEst_Out(i);
//      Fo.QSetM(M, FiltVap, Qv, IOP_Self(i));
//      Fo.QAddM(M, FiltSL, Qs);
//      IO_Self(i).SetPMUPassing(False);
//      IO_Self(i).SetPMUBlocking(False);
//      }
//    else
//      {
//	    double QmO=IOQmEst_Out(i);
//	    Qv = VFrac * QmO;
//      Qs = SFrac * QmO;
//      Fo.QSetM(M, som_Gas, Qv, IOP_Self(i));
//      Fo.QAddM(M, som_SL, Qs);
//      }
//    QvActual_SL += Fo.QVolume(som_SL);
//
//    }
//  return QvActual_SL;
//  }
//#endif

//-------------------------------------------------------------

/*#F:
This determines what material is leaving through each outlet based on levels,
height of outlets above the base of the vessel, makeup of contents etc.

The Availability of material in/out/contained is taken into account.
*/


/* Overrides EvalProducts_SurgeLevel found in M_Base.cpp */
//#if CNMTOREMOVE
//void MixerSettler::EvalProducts_SurgeLevel(rSpContainer M, rSpConduit QInEst, dword IOIds)
//  {
//  #if dbgDerivs
//  long dbg1=dbgDerivsSrg1();
//  long dbg2=dbgDerivsSrg2();
//  #endif
//  
//  #if dbgDerivs
//  flag fFirstIO=1;
//  #endif
//
//  double QvEst_SL = QInEst.QVolume(som_SL);
//  double L = M.Level();
//
//  SpMArray SLEVImg;
//  M.GetSLEVImage(SLEVImg);
//  double SLEVRho=M.SLEVDensity();
//  double SLEVMSLfrac=SLEVImg.MassFrac(som_SL);
//
//  HgtOrdInfo Info;
//  SetFracDiams(IOIds, M);
//  GetHeightOrder(IOIds, M, Info);
//  int Oi = Info.NHgt;
//  double QvActual_SL = 0.0;
//
//  int LastIO = (M.Mixed() ? Oi : Oi+1); // Extra for overflow
//  
//  double FHgt = 0.0;
//  for (int io0 = 0; io0 < LastIO;  )
//    {
//    int ioN =-1;
//    double QOutVRqd;
//    double QOutMRqd;
//    double QOutVAvl;
//    double QOutMAvl;
//    double DiamFrac;
//    double IoLvl;
//
//    flag DoingIO=(io0<Oi);
//
//    if (DoingIO)
//      {
//      DiamFrac = 0.02;
//      for (ioN = io0+1; (ioN<Oi && IOSameHgt(Info.Hgt[ioN])) ; ioN++) {};
////      MeasRqdFlows(M, Info, io0, ioN, QOutMRqd, QOutVRqd);
//      MeasureRqdFlows(M, SLEVRho, SLEVMSLfrac, Info, io0, ioN, IoLvl, QOutMRqd, QOutVRqd, QOutMAvl, QOutVAvl);
//      FHgt = IOAbsFracHgt_Term(Info.Hgt[io0], M);
//      FHgt = Range(0.0, FHgt, 1.0-DiamFrac); 
//      }
//    else
//      {
//      ioN      = io0+1;
//      DiamFrac = 0.001;
//      QOutMRqd = 1.0e6;
//      QOutVRqd = QOutMRqd/M.Rho(som_SL);
//      FHgt     = Max(1.0, FHgt+DiamFrac);
//      }
//
//    double OvrDmp = 0.0;//M.OverFlowDamping;//*Max(0.0, (DmpEnd-L)/DmpEnd);
//    
//    double SFrac;
//    double VFrac;
//    double SFracEquil;
//    double LevelEquil;
//
//    if (M.Mixed())
//      {
//      // VapourFrac is that which exists in the tank
//      VFrac = M.MassFrac(som_Gas);//QAvVap/GTZ(QAvVap+QAvSL);
//      SFrac = 1.0 - VFrac;
//      }
//    else
//      {
//      double OutT=FHgt+1.0*DiamFrac; // Frac height of top of Outlet
//      double OutB=FHgt-0.0*DiamFrac; // Frac height of bottom of Outlet
//
//      double QvNett=(QvEst_SL-QvActual_SL-QOutVRqd);
//      
//      SFracEquil=(QvEst_SL-QvActual_SL)/QOutVRqd;
//      LevelEquil=OutB+SFracEquil*DiamFrac;
//      
//      double dTimeScl=1.0+Range(0.0, OvrDmp, 2.0);
//      double dT2Target=(ICGetTimeIncMx()*dTimeScl)/*-ICGetdTime_FromStartOfStep()*/;
//      double dL=(QvNett*dT2Target)/GTZ(M.ShapeVolume());
//      
//      if (L>=LevelEquil)
//        {
//        if (SFracEquil >= 1.0)      // will rise
//          SFrac=1.0;
//        else if (SFracEquil >= 0.0) // will level out
//          SFrac=SFracEquil+(1.0-SFracEquil)*(LevelEquil-L)/NZ(dL);
//        else // (SFracEquil < 0.0)  // will drop
//          SFrac=0.0;
//        }
//      else // (L<=LevelEquil)
//        {
//        if (SFracEquil <= 0.0)      // will drop
//          SFrac=0.0;
//        else if (SFracEquil <= 1.0) // will level out
//          SFrac=SFracEquil+(1.0-SFracEquil)*(LevelEquil-L)/NZ(dL);
//        else // (SFracEquil > 1.0)  // will rise
//          SFrac=1.0;
//        }
//
//      SFrac=Range(0.0, SFrac, 1.0);
//      VFrac=Range(0.0, 1.0-SFrac, 1.0);
//
//      #if dbgDerivs
//      if (dbg1) 
//        {
//        dbgp(" %s:%5.1f ",fFirstIO ? "FHgt":"    ", FHgt*100);
//        if (L>LevelEquil) dbgp("++ "); else if (L<LevelEquil) dbgp("-- "); else dbgp("== "); 
//        if (SFracEquil > 1.0) dbgp("Rise  "); else if (SFracEquil >= 0.0) dbgp("Equil "); else dbgp("Drop  "); 
//        dbgp(" L:%13.6g eqL:%13.6g", L*100, LevelEquil*100); 
//        if (dbg2)
//          dbgp(" dL/dL:%13.6g dT:%9.4f", (LevelEquil-L)/NZ(dL),dT2Target); 
//        dbgp(" eqSf:%13.6g", SFracEquil); 
//        dbgpln(" Sf:%13.6g Vf:%13.6g : %s.%s", SFrac, VFrac, sTag(), M.Tag()); 
//        fFirstIO=0;
//        }
//      #endif
//      }
//
//    if (DoingIO)
//      QvActual_SL+=SetOutFlows(M, Info, io0, ioN, SFrac, VFrac);
//    else //if (pOverFlow!=NULL)
//      {
//      if (SFrac>=0.0) // Got FLow
//        {
//        double Qs=SFrac*QOutMRqd;
//        M.OverFlow().QSetM(M, som_SL, Qs, M.Press());
//        QvActual_SL+=Qs;
//        }
//      else if (L<0.90) // GetRid of Overflow Conduit
//        M.SpillClear();
//      else if (M.SpillExists())
//        M.OverFlow().QZero();
//      }
//
//    io0=ioN;
//    }
//
//  }
//#endif
//--------------------------------------------------------------------------
// Find the mass of Primary metal in solution.
double MixerSettler::MetalMass(SpConduit &QIn)
  {
  const char* Element = PriMetal.Str();
  if (Element==NULL || Element[0]==0)
    return 0.0; //NO Metal specified!

  double SpcMass, MM = 0.0, NumElems = 0.0;
  for (int i=0; i<SDB.Count(); i++)
    {
    NumElems = GetElemAmt(CDB[SDB[i].iCId].m_sElDef, PriMetal);
    
    if (NumElems > 0) 
      {
      SpcMass = QIn.VMass[i];//kg
      MM += SpcMass * ((NumElems * EDB[EDB.Find(Element)].m_AtmWt)
	                     /GTZ(SDB[i].MoleWt()));  
      }
    }
  return MM;
  }

//--------------------------------------------------------------------------
// Find the extent of metal transfer using Isotherms
double MixerSettler::MetalExtract(double StripMass, double StripVol, double ExtConc, double StripConc, double Ratio)
  {
	const double YMax = PriIsotherm.ScanYMax();
	const double YMin = PriIsotherm.ScanYMin();
  IntersectPt1 = PriIsotherm.Xy(ExtConc);  
  const double MinStrip = Max(0.0, ExtConc - Ratio * IntersectPt1);  // y-intercept of the operating curve
	double Loaded   = Ratio * StripConc + MinStrip;        // Loaded conc from operating curve
	if (Loaded>YMax) Loaded = YMax;
	if (Loaded<YMin) Loaded = YMin;
  IntersectPt2 = PriIsotherm.Xy(Loaded); // Stripped conc. from equilibrium isotherm
	const double Stripped = IntersectPt2;

	SEff = Range(0.01, SEff, 1.0);
	double ER = 0.0;
	if (StripMass > 1e-9)
    {
	  const double MExtract = StripVol * (StripConc - Stripped/SEff);
	  ER = Range(0.0, MExtract/StripMass, 1.0);
    }
  if (Extraction==True)
    {
    MinOrgConc = MinStrip;
    }
  else
    {
    MinAqsConc = MinStrip;
    }

  return ER;
  }

//--------------------------------------------------------------------------

void MixerSettler::EvalProducts(CNodeEvalIndex & NEI)
  {
  StkSpConduit Aq("Aq", chLINEID(), this);
  StkSpConduit Og("Og", chLINEID(), this);

  switch (SolveMethod())
    {
    case SM_Direct:
      {
      ClrCI(1);
      SigmaQInPMin(Aq(), som_ALL, Id_2_Mask(ioidAqIn));
      SigmaQInPMin(Og(), som_ALL, Id_2_Mask(ioidOrgIn));

      double Ratio; 
      const double AqIn    = Aq().QVolume(som_Liq);
      const double OrgIn   = Og().QVolume(som_Liq);
      const double AqInQm  = Aq().QMass(som_Liq);
      const double OrgInQm = Og().QMass(som_Liq);
      //double AqT   = Aq().Temp();
      //double OrgT  = Og().Temp();
      if ((OrgIn > 1e-9) && (AqIn > 1e-9))
        {
        Ratio   = OrgIn/AqIn;
        OARatio = Ratio;
        if (!Extraction)
          Ratio = 1.0/Ratio;
        }
      else
        {
        Ratio = 0.0;
        OARatio = Ratio;
        }

      // Find metal concentrations in aqueous and organic feeds in g/l
      double AqMass = 0.0;
      if (AqIn>0.0)
        {
        AqMass = MetalMass(Aq());
	      AqsInConc = AqMass/AqIn;
        }
      else
        AqsInConc = 0.0;
      double OgMass = 0.0;
      if (OrgIn>0.0)
        {
        OgMass = MetalMass(Og());
        OrgInConc = OgMass/OrgIn;
        }
      else
        OrgInConc = 0.0;

      //===============================================
      // Isotherm Method
      if (iMethod==SVM_Isotherms)
        {
        if (Extraction)
          dReactExt = MetalExtract(AqMass, AqIn, OrgInConc, AqsInConc, Ratio);
        else
          dReactExt = MetalExtract(OgMass, OrgIn, AqsInConc, OrgInConc, Ratio);
        
        // Set the extent of reaction
        if (RB() && RB()->Eqns.GetSize()>0)
          {
          CR_Eqn* pCREqn=(CR_Eqn*)RB()->Eqns[0];
          if (pCREqn->SpecieReactantIndex(iSol)>=0 || pCREqn->SpecieProductIndex(iSol)>=0)
            {
            pCREqn->SetExtent(iSol, dReactExt);
            }
          else
            {
      		  SetCI(1, true);
            }
          }
        }
      //================================================

      SpMArray Mix;
      SigmaQInPMin(QMxInEst, som_ALL, 0xffffffff);

      SpConduit & Aqs = *IOConduit(IOWithId_Self(ioidAqOut)); //Raffinate
      SpConduit & Orgs = *IOConduit(IOWithId_Self(ioidOrgOut)); //LoadedOrganic

      if (RB())
	      RB()->EvalProducts(QMxInEst);
      double Tc = QMxInEst.Temp();
      Mix=(*QMxInEst.Model());

      // Allow only organic losses in the aqueous stream and
      // the aqueous losses in the organic stream.
      double A=0.0;
      double O=0.0;
      AqLoss = Range(0.0, AqLoss, 0.9);
      OgLoss = Range(0.0, OgLoss, 0.9);
      for (int i=0; i<SDB.Count(); i++)
        {
        double dens = GetDensity(i, Tc, Std_P, QMxInEst.Ovr());
        if (dens < RhoSep)
          {
          Aqs.SetVMass(i, Mix, Mix[i] * OgLoss);
          Orgs.SetVMass(i, Mix, Mix[i] - Aqs.VMass[i]);
          }
        else
          {
          Orgs.SetVMass(i, Mix, Mix[i] * AqLoss);
          Aqs.SetVMass(i, Mix, Mix[i] - Orgs.VMass[i]);
          }
        A+=Aqs.VMass[i];
        O+=Orgs.VMass[i];
        }

      Aqs.SetTempPress(QMxInEst.Temp(), QMxInEst.Press());
      Orgs.SetTempPress(QMxInEst.Temp(), QMxInEst.Press());

      for (i=0; i<NoFlwIOs(); i++)
        {
        if (IO_Out(i))
          {
          switch (IOId_Self(i))
            {
            case ioidAqOut : IOConduit(i)->QAdjustQmTo(som_ALL, A/NIOsWithId_Self(ioidAqOut));  break;
            case ioidOrgOut: IOConduit(i)->QAdjustQmTo(som_ALL, O/NIOsWithId_Self(ioidOrgOut)); break;
            default:         IOConduit(i)->QAdjustQmTo(som_ALL, 0.0);                         break;
            }
          }
        }

      // Find metal concentrations in aqueous and organic products in g/l
      Aqs.SanityCheck();
      Orgs.SanityCheck();

      const double AqOut  = Aqs.QVolume(som_Liq);
      const double OrgOut = Orgs.QVolume(som_Liq);

      double AqMassOut = 0.0;
      if (AqOut>0.0)
        {
        AqMassOut = MetalMass(Aqs);
        AqsOutConc = AqMassOut/AqOut;
        }
      else
        AqsOutConc = 0.0;
      double OgMassOut = 0.0;
      if (OrgOut>0.0)
        {
        OgMassOut = MetalMass(Orgs);
        OrgOutConc = OgMassOut/OrgOut;
        }
      else
        OrgOutConc = 0.0;

      MaxLoadedConc = AqMass/GTZ((AqMass+OrgInQm)/Orgs.Rho(som_Liq));//Based on 100% Extraction
      MaxStripConc = OgMass/GTZ((OgMass+AqInQm)/Aqs.Rho(som_Liq));

      //MN_Surge::EvalProducts(CNodeEvalIndex & NEI);
      break;
      }
    case SM_Inline:
      {
      ClrCI(1);
      SigmaQInPMin(Aq(), som_ALL, Id_2_Mask(ioidAqIn));
      SigmaQInPMin(Og(), som_ALL, Id_2_Mask(ioidOrgIn));

      double Ratio; 
      const double AqIn    = Aq().QVolume(som_Liq);
      const double OrgIn   = Og().QVolume(som_Liq);
      const double AqInQm  = Aq().QMass(som_Liq);
      const double OrgInQm = Og().QMass(som_Liq);
      //double AqT   = Aq().Temp();
      //double OrgT  = Og().Temp();
      if ((OrgIn > 1e-9) && (AqIn > 1e-9))
        {
        Ratio   = OrgIn/AqIn;
        OARatio = Ratio;
        if (!Extraction)
          Ratio = 1.0/Ratio;
        }
      else
        {
        Ratio = 0.0;
        OARatio = Ratio;
        }

      // Find metal concentrations in aqueous and organic feeds in g/l
      double AqMass = 0.0;
      if (AqIn>0.0)
        {
        AqMass = MetalMass(Aq());
	      AqsInConc = AqMass/AqIn;
        }
      else
        AqsInConc = 0.0;
      double OgMass = 0.0;
      if (OrgIn>0.0)
        {
        OgMass = MetalMass(Og());
        OrgInConc = OgMass/OrgIn;
        }
      else
        OrgInConc = 0.0;

      //===============================================
      // Isotherm Method
      if (iMethod==SVM_Isotherms)
        {
        if (Extraction)
          dReactExt = MetalExtract(AqMass, AqIn, OrgInConc, AqsInConc, Ratio);
        else
          dReactExt = MetalExtract(OgMass, OrgIn, AqsInConc, OrgInConc, Ratio);
        
        // Set the extent of reaction
        if (RB() && RB()->Eqns.GetSize()>0)
          {
          CR_Eqn* pCREqn=(CR_Eqn*)RB()->Eqns[0];
          if (pCREqn->SpecieReactantIndex(iSol)>=0 || pCREqn->SpecieProductIndex(iSol)>=0)
            {
            pCREqn->SetExtent(iSol, dReactExt);
            }
          else
            {
      		  SetCI(1, true);
            }
          }
        }
      //================================================

      SpMArray Mix, Aqs, Ogs;
      SigmaQInPMin(QMxInEst, som_ALL, 0xffffffff);

      if (RB())
	      RB()->EvalProducts(QMxInEst);
      double Tc = QMxInEst.Temp();
      Mix=(*QMxInEst.Model());

      // Allow only organic losses in the aqueous stream and
      // the aqueous losses in the organic stream.
      double A=0.0;
      double O=0.0;
      AqLoss = Range(0.0, AqLoss, 0.9);
      OgLoss = Range(0.0, OgLoss, 0.9);
      for (int i=0; i<SDB.Count(); i++)
        {
        //double dens = SDB[i].DensityInDirect() ? 1000.0 : SDB[i].Density(Tc, Std_P, QMxInEst.Ovr(), NULL);
        //double dens = SDB[i].Density(Tc, Std_P, QMxInEst.Ovr(), NULL);
        //double dens = SDB[i].DensityInDirect() ? 1000.0 : SDB[i].Density(Tc, Std_P, QMxInEst.Ovr(), NULL);
        //double dens = SDB[i].DensityInDirect() ? SDB[i].SpRho(Tc, Std_P) : SDB[i].Density(Tc, Std_P, QMxInEst.Ovr(), NULL);
        //double dens = SDB[i].DensityInDirect() ? SDB[SDB[i].Solvent()].Density(Tc, Std_P, QMxInEst.Ovr(), NULL) : SDB[i].Density(Tc, Std_P, QMxInEst.Ovr(), NULL);
        double dens = GetDensity(i, Tc, Std_P, QMxInEst.Ovr());
        if (dens < RhoSep)
          {
          Aqs.SetVMass(i, Mix, Mix[i] * OgLoss);
          Ogs.SetVMass(i, Mix, Mix[i] - Aqs[i]);
          }
        else
          {
          Ogs.SetVMass(i, Mix, Mix[i] * AqLoss);
          Aqs.SetVMass(i, Mix, Mix[i] - Ogs[i]);
          }
        A+=Aqs[i];
        O+=Ogs[i];
        }

      for (i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          {
          switch (IOId_Self(i))
            {
            case ioidAqOut : SetProdMakeup(PMU_IOId |PMU_Filter, ioidAqOut, Aqs, Tc, Std_P, Aq().Model()); break;
            //case ioidAqOut : SetProdMakeup(PMU_IONo |PMU_Image, i, Aqs, Tc, Std_P, Aq().Model()); break;
            case ioidOrgOut: SetProdMakeup(PMU_IOId |PMU_Filter, ioidOrgOut, Ogs, Tc, Std_P, Og().Model()); break;
            //default:         SetProdMakeup(PMU_IOId |PMU_Image, i, Mix, Tc, Std_P, Sd().Model()); break;
            }
          }

      for (i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          {
          switch (IOId_Self(i))
            {
            case ioidAqOut : IOConduit(i)->QAdjustQmTo(som_ALL, A/NIOsWithId_Self(ioidAqOut));  break;
            case ioidOrgOut: IOConduit(i)->QAdjustQmTo(som_ALL, O/NIOsWithId_Self(ioidOrgOut)); break;
            default:         IOConduit(i)->QAdjustQmTo(som_ALL, 0.0);                         break;
            }
          }

      // Find metal concentrations in aqueous and organic products in g/l
      SpConduit & AqO = *IOConduit(IOWithId_Self(ioidAqOut)); //Raffinate
      SpConduit & OrgO = *IOConduit(IOWithId_Self(ioidOrgOut)); //LoadedOrganic

      AqO.SanityCheck();
      OrgO.SanityCheck();

      const double AqOut  = AqO.QVolume(som_Liq);
      const double OrgOut = OrgO.QVolume(som_Liq);

      double AqMassOut = 0.0;
      if (AqOut>0.0)
        {
        AqMassOut = MetalMass(AqO);
        AqsOutConc = AqMassOut/AqOut;
        }
      else
        AqsOutConc = 0.0;
      double OgMassOut = 0.0;
      if (OrgOut>0.0)
        {
        OgMassOut = MetalMass(OrgO);
        OrgOutConc = OgMassOut/OrgOut;
        }
      else
        OrgOutConc = 0.0;

      MaxLoadedConc = AqMass/GTZ((AqMass+OrgInQm)/OrgO.Rho(som_Liq));//Based on 100% Extraction
      MaxStripConc = OgMass/GTZ((OgMass+AqInQm)/AqO.Rho(som_Liq));

      //MN_Surge::EvalProducts(CNodeEvalIndex & NEI);
      break;
      }
    case SM_Buffered:
      {
      SigmaQInPMin(QMxInEst, som_ALL, 0x5); /*0 & 2*/

      ASSERT_ALWAYS(FALSE, "MixerSettler REWRITE NEEDED", __FILE__, __LINE__);
#if CNMTOREMOVE
      EvalProducts_SurgeLevel(Mixer, QMxInEst, 0x5); /*IOids 0 & 2*/
#endif

      SpMArray Mix, Aqs, Orgs;
      Mix=(*Settler.Model());

      Orgs=Mix;
      Aqs=Mix;

      for (int i=0; i<SDB.Count(); i++)
        {
        if (!SDB[i].IsGas())
          {
          //if (SDB[i].Density(Std_T, Std_P, QMxInEst.Ovr(), NULL) <= RhoSep)
          if (GetDensity(i, Std_T, Std_P, QMxInEst.Ovr()) <= RhoSep)
            Aqs.SetVMass(i, Mix, 0.0);
          else
            Orgs.SetVMass(i, Mix, 0.0);
          }
        }

      Orgs.Normalise();
      Aqs.Normalise();

      for (i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          switch (IOId_Self(i))
            {
            case ioidAqOut : SetProdMakeup(PMU_IONo |PMU_Filter, i, Aqs, Std_T, Std_P, Aq().Model()); break;
            case ioidOrgOut: SetProdMakeup(PMU_IONo |PMU_Filter, i, Orgs, Std_T, Std_P, Og().Model()); break;
            //default:       SetProdMakeup(PMU_IONo |PMU_Image, i, Mix, Std_P); break;
            }   

      ASSERT_ALWAYS(FALSE, "MixerSettler REWRITE NEEDED", __FILE__, __LINE__);
#if CNMTOREMOVE
      EvalProducts_SurgeLevel(Settler, QMxOFlow, 0xa /*IOids 1 & 3 */);
#endif
      break;
      }
    }
  }


//--------------------------------------------------------------------------

void MixerSettler::EvalDerivs(CNodeEvalIndex & NEI)
  {
  Mixer.ZeroDeriv();
	Settler.ZeroDeriv();
  if (!GetActiveHold())
    {
    MxResT = Mixer.Volume(som_SL)/GTZ(QMxInEst.QVolume(som_SL));
    StResT = Settler.Volume(som_SL)/GTZ(QMxOFlow.QVolume(som_SL));

    IDMask MIOIds=0x5;
    for (int i = 0; i < NoFlwIOs(); i++)
      if ((IOIdMask_Self(i) & MIOIds)!=0)
        Mixer.AddDeriv(*IOConduit(i), IOSign(i));

    Settler.AddDeriv(QMxOFlow, 1.0);

    IDMask SIOIds=0xa;
    for (int j = 0; j < NoFlwIOs(); j++)
      if ((IOIdMask_Self(j) & SIOIds)!=0)
        Settler.AddDeriv(*IOConduit(j), IOSign(j));

    double QvMxAcc=0.0;
    CDensityInfo C(Settler.Fidelity(), Settler.DensityMethod(), Mixer.Temp(), Mixer.Press(), Mixer.Ovr(), NULL);
    for (int k=0; k<SDB.Count();k++)
      if (!SDB[k].IsGas())
        QvMxAcc+=Mixer.VMassDot[k]*(SDB[k].DensityX(C) ? C.msVolume() : 0.0);
      else if (!Mixer.Closed())
        //  Vapours do not accumlate
        Mixer.SetVValueDot(k, 0.0);
      
    double MaxOFV=MixOFLev*Mixer.ShapeVolume();
    double V=Mixer.Volume(som_SL)+QvMxAcc*ICGetTimeInc();
    double QvMxOver=(V-MaxOFV)/ICGetTimeInc();

  
  
    if (QvMxOver>0.0)
      {
      QMxOFlow.QSetM(Mixer, som_SL, QvMxOver*Mixer.Rho(som_SL), QMxOFlow.Press());
      //EvalDerivs_SubQm_____(Mixer, QMxOFlow);
	    Mixer.AddDeriv(QMxOFlow, -1.0);
      }
    }  

  QmAcc=0.0;
  QvAcc=0.0;
  CDensityInfo CM(Settler.Fidelity(), Settler.DensityMethod(), Mixer.Temp(), Mixer.Press(), Mixer.Ovr(), NULL);
  CDensityInfo CS(Settler.Fidelity(), Settler.DensityMethod(), Settler.Temp(), Settler.Press(), Settler.Ovr(), NULL);
  for (int i=0; i<SDB.Count();i++)
    if (!SDB[i].IsGas())
      {
      QmAcc+=Mixer.VMassDot[i]+Settler.VMassDot[i];
      QvAcc+=Mixer.VMassDot[i]*(SDB[i].DensityX(CM) ? CM.msVolume(): 0.0)+ Settler.VMassDot[i]*(SDB[i].DensityX(CS) ? CS.msVolume(): 0.0);
      }
  };

//--------------------------------------------------------------------------

void MixerSettler::EvalDiscrete(flag ControlEnabled, flag ControlDisabled)
  {
  SpMArray  AqueousIn; //=QMxInEst.MArray();
  SpMArray	OrganicIn; //=QMxInEst.MArray();

  for (int j=0; j<SDB.Count(); j++)
   {
   if (!SDB[j].IsGas())
	   {
     //if (SDB[j].Density(Std_T, Std_P, QMxInEst.Ovr(), NULL) > RhoSep)
     if (GetDensity(j, Std_T, Std_P, QMxInEst.Ovr()) > RhoSep)
       AqueousIn.SetVMass(j, QMxInEst, QMxInEst.VMass[j]);
     else
       OrganicIn.SetVMass(j, QMxInEst, QMxInEst.VMass[j]);
     }
   }

  double OrgMMass, AqsMMass;
  OrgInConc=MetalConc(&QMxInEst, OrganicIn, PriMetal, &OrgMMass);
  AqsInConc=MetalConc(&QMxInEst, AqueousIn, PriMetal, &AqsMMass);

  int metalsthere=0;
  metalsthere=(OrgInConc>0.0)&&(AqsInConc>0.0);

  for (int k=0; k<SDB.Count(); k++)
    {
    if (!SDB[k].IsGas())
      {
      //if (SDB[k].Density(Std_T, Std_P, Mixer.Ovr(), NULL) > RhoSep)
      if (GetDensity(k, Std_T, Std_P, Mixer.Ovr()) > RhoSep)
        Aqueous.SetVMass(k, Mixer, Mixer.VMass[k]);
      else
        Organic.SetVMass(k, Mixer, Mixer.VMass[k]);
      }
    }

  double OrgVol, AqsVol;
  OrgVol=Mixer.Volume(Organic);
  AqsVol=Mixer.Volume(Aqueous);
  OARatio = OrgVol/GTZ(AqsVol);

 if ((RB()!=NULL)&&(ReactionsOn==True)&&metalsthere) // do reactions in the mixer
   {
   double OrgConc, AqsConc, OrgMass, AqsMass;
   OrgConc=MetalConc(&Mixer, Organic, PriMetal, &OrgMass);
   AqsConc=MetalConc(&Mixer, Aqueous, PriMetal, &AqsMass);

   double LOConc, PEConc;
   double frac=0.0;
   double RemSpcMass, dSpcMass, YMin, YMax;//, InitYConc;
   YMax=PriIsotherm.ScanYMax();
   YMin=PriIsotherm.ScanYMin();

   if (Extraction==True)
    {
    double Aq1 = PriIsotherm.Xy(OrgInConc); // Stripped organic in
    MinOrgConc = Max(0.0, OrgInConc - OARatio * Aq1);  // y-intercept of the operating curve
    LOConc=FindNewYConc(&AqsInConc, &OrgInConc, &MinOrgConc);
    
    if (DoLMS==True)
      {
      OrgConc=Range(YMin, OrgConc, YMax);
      LMS1.Out=PriIsotherm.Xy(OrgConc);
      LMS1.RqdOut=AqsConc;
      LMS1.Input= LOConc;
      LMS1.DoCalc();
      LMS1.Weight=Range(0.0, LMS1.Weight, 20.0);
      LOConc=LMS1.NewOut; 
      }

	  RemSpcMass=(AqsMass+OrgMass) - (LOConc * OrgVol);
	  dSpcMass=AqsMass-RemSpcMass;
	  if (AqsMass > 0.0) frac=(dSpcMass/AqsMass);
	  }
	 else  //Stripping
	  {
    double Og1 = PriIsotherm.Xy(AqsInConc); // Stripped organic in
    MinAqsConc = Max(0.0, AqsInConc - 1.0/GTZ(OARatio) * Og1);  // y-intercept of the operating curve
    PEConc=FindNewYConc(&OrgInConc, &AqsInConc, &MinAqsConc);
    if (DoLMS==True)
     {
     AqsConc=Range(YMin, AqsConc, YMax);
     LMS1.Out=PriIsotherm.Xy(AqsConc);
     LMS1.RqdOut=OrgConc;
     LMS1.Input= PEConc;
     LMS1.DoCalc();
     LMS1.Weight=Range(0.0, LMS1.Weight, 20.0);
     PEConc=LMS1.NewOut; 
     }

    RemSpcMass=(AqsMass+OrgMass) - (PEConc * AqsVol);
    dSpcMass=OrgMass-RemSpcMass;
    if (OrgMass > 0.0)  frac=Range(0.0, (dSpcMass/OrgMass), 1.0);
	  }

	 for (int k=0; k<RB()->NEqns(); k++)
	   {
     CR_Eqn* pCREqn=(CR_Eqn*) RB()->Eqns[k];
     CEC_FracExt*  pCntrl=(CEC_FracExt*) pCREqn->pControl;
     int spcid=pCntrl->m_Spc.m_SpcId;
     pCREqn->SetExtent(spcid, frac);
	   }
   if (ICGetTimeInc() > 0.0) RB()->EvalDiscrete(Mixer, QMxInEst, QMxOFlow);
	 }

  SpMArray  AqueousOut; //=QMxOFlow.MArray();
  SpMArray	OrganicOut; //=QMxOFlow.MArray();

  for (j=0; j<SDB.Count(); j++)
  {
  if (!SDB[j].IsGas())
    {
    //if (SDB[j].Density(Std_T, Std_P, QMxOFlow.Ovr(), NULL) > RhoSep)
    if (GetDensity(j, Std_T, Std_P, QMxOFlow.Ovr()) > RhoSep)
      AqueousOut.SetVMass(j, QMxOFlow, QMxOFlow.VMass[j]);
    else
      OrganicOut.SetVMass(j, QMxOFlow, QMxOFlow.VMass[j]);
    }
  }

  double dumass;
  OrgOutConc=MetalConc(&QMxOFlow, OrganicOut, PriMetal, &dumass);
  AqsOutConc=MetalConc(&QMxOFlow, AqueousOut, PriMetal, &dumass);

  // Kill any vapours present if the vessel is open - hss to complete!!!
  if (!Mixer.Closed())
    for (int i=0; i<SDB.Count();i++)
      if (SDB[i].IsGas())
        Mixer.SetVMass(i, Mixer, 0.0);
        
  // Qualities
  for (int i = 0;  (i<NoFlwIOs()) && (IOId_Self(i)<idMx); i++)
    if (IO_In(i))
	  {
    Mixer.AddDiscrete(*IOConduit(i), IOSign(i));
	  Settler.AddDiscrete(*IOConduit(i), IOSign(i));
	  }
  }

//--------------------------------------------------------------------------
double MixerSettler::FindNewYConc(double* InX, double* InY, double* Yintcpt)
  {
  const int n=20;
  double OutY, /*LoX,*/ OARat, Ycon;
  OARat=GTZ(OARatio);
  OutY=(OARatio * (*InX)) + *Yintcpt;
  if ((OutY > *InY)&&(OARat>0.1)&&(*InX>0.0))
    {
    const double YMax = PriIsotherm.ScanYMax();
    if (OutY>YMax)
      OutY=YMax;
    double nOutX, nInY, nInX;
    nOutX=PriIsotherm.Xy(OutY);
    nInY=(OARatio * nOutX) + *Yintcpt;
    if (nInY<=(*InY))  
      {
      return OutY;
      }
      
    double dx, dy;
    dx=*InX-nOutX; 
    dy=OutY-nInY;
    
    double inc=(OutY-(*InY))/n;
    double maxa=dx*dy;
    double area, parea;
    parea=maxa;
    Ycon=OutY;
    for (int i=n;i>0;i--) 
     {
     OutY-=inc;
     nInX=(OutY - *Yintcpt)/OARat;
     nOutX=PriIsotherm.Xy(OutY);
     nInY=(OARat * nOutX) + *Yintcpt;
     dx=nInX-nOutX; 
     dy=OutY-nInY;
     area=dx*dy;
     if (area>=maxa) 
       Ycon=OutY;
     if (nInY<=(*InY))  
       {
       return Ycon;      
       break;
       }
     if (area<parea)
       break;
     parea=area;
     }
    return Ycon;
    }
  else
    return *InY;
  }

//---------------------------------------------------------------------------

double MixerSettler::MetalConc(SpContainer* WV, SpMArray &Phse, Strng Metal, double* MetMass)
  {
  double PhseVol=0.0;
  const char* Element=Metal.Str();
  double NumElems, SpcMass;
  *MetMass=0.0;
	 
  PhseVol=WV->Volume(Phse);  //m^3
  if (PhseVol>0.0)
   {
    for (int i=0; i<SDB.Count(); i++)
      {
	    NumElems=0.0;
      if (Phse[i] > 0.0)
         NumElems=GetElemAmt(CDB[SDB[i].iCId].m_sElDef, Metal);

      if (NumElems > 0) 
        {
        SpcMass=WV->VMass[i];//kg
        *MetMass+=SpcMass * ((NumElems* EDB[EDB.Find(Element)].m_AtmWt)
	                       /GTZ(SDB[i].MoleWt()));  
        }
      }
     }

  if (*MetMass > 0.0)
    return *MetMass/GTZ(PhseVol);   // kg/m^3 or g/l
  else
	return 0.0;

  }

//---------------------------------------------------------------------------

double MixerSettler::MetalConc(SpConduit* FlwIn, SpMArray &Phse, Strng Metal, double* MetMass)
  {
  double PhseVol=0.0;
  const char* Element=Metal.Str();
  double NumElems, SpcMass;
  *MetMass=0.0;
							 
  PhseVol=FlwIn->QVolume(som_ALL, Phse);  //m^3/s
  if (PhseVol>0.0)
   {
    for (int i=0; i<SDB.Count(); i++)
     {
     NumElems=0.0;
     if (Phse[i] > 0.0)
        NumElems=GetElemAmt(CDB[SDB[i].iCId].m_sElDef, Metal);

     if (NumElems > 0) 
       {
       SpcMass=FlwIn->VMass[i];//kg/s
       *MetMass+=SpcMass * ((NumElems* EDB[EDB.Find(Element)].m_AtmWt)
	                       /GTZ(SDB[i].MoleWt()));  
       }
     }
   }

  if (*MetMass > 0.0)
    return *MetMass/GTZ(PhseVol);   // kg/m^3 or g/l
  else
	return 0.0;
  }

//--------------------------------------------------------------------------

double MixerSettler::GetDensity(int index, double T, double P, SpPropOveride *Ovr)
  {
  //return SDB[index].Density(T, P, Ovr, NULL);
  //return SDB[index].DensityInDirect() ? 1000.0 : SDB[index].Density(T, P, Ovr, NULL);
  //return SDB[index].DensityInDirect() ? SDB[index].SpRho(Tc, Std_P) : SDB[index].Density(T, P, Ovr, NULL);
  return SDB[index].DensityXZero(CDensityInfo(Settler.Fidelity(), Settler.DensityMethod(), T, P, Ovr, NULL));
  }

//--------------------------------------------------------------------------

dword MixerSettler::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;        
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IOConduit(i)->QMass()>gs_DisplayZeroFlow)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  }

//--------------------------------------------------------------------------

flag MixerSettler::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tInvalid Solute for reaction (using Isotherm method)."; return 1;
    default:                                               
      return MdlNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------

#if DOPBSMOOTHING  
void MixerSettler::EvalMsAGDatas()
  {
  if (RB.Enabled())
    {
    if (MsJoins.GetSize()<1)
      MsJoins.SetSize(1);
//    MsJoins[0].MInP=*RB.MassSumSrc();
//    MsJoins[0].MOutP=*RB.MassSumSnk();
    }

  MdlNode::EvalMsAGDatas();
  };
#endif

//--------------------------------------------------------------------------



