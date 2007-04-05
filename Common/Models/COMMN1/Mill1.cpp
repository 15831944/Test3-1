//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __MILL1_CPP
#include "Mill1.h"
                
//===========================================================================
//
//                                   Mill
//
//===========================================================================
/*#D:#T:Mill                    
#X:#h<General Description>#nThis model is used to simulate a rod or ball 
mill.  This is a very simple model for a mill, for a more detailed model use the 
BallMill model, which requires the user to configure more parameters.   
#n#nTwo options are available for determining the size distribution in the mill product:
#na) The user may specify individual discharge partition curves for each defined size 
distribution.  For example, if two size distributions have been specified in the configuration 
file, then the user may configure two separate mill discharge curves.  Note : If individual 
discharge curves are required, the user does not have to specify the same number of discharge 
curves as the number of size distributions.  Those size distributions for which no discharge 
partition curve is specified will have no work done on them as they pass through the mill.
#nb) The user may specify an overall discharge curve.  In this case all of the size 
distributions from the mill will be equal.
#n#nA percentage of the unused portion of the installed power is used to heat the mill 
product.  The power used to mill the material is obtained from the product of the bond 
work index and the mass throughput.#n
#n#h<Variables to be supplied by the user>#n
#i<Motor :>The installed motor power of the mill.#n 
#i<HeatPerc :>The percentage of the unused power which is used to heat the product.#n
#i<Bond_WI :>The Bond Work index of the material to be milled.  This number does 
not affect the discharge partition curve, and is only used to calculate the power 
used by the mill.#n
#i<DischSize :>The user may choose Individual or Overall.  If the 'Individual' option 
is chosen, then the user will be required to input a number of discharge partition curves.  
The number will be equal to the number of size distributions in the project.  If 'Overall' 
is chosen, the user must input a single discharge partition curve.#n
#i<DischPart(number) :>The discharge partition curve(s) for the mill product.  These curves 
can be read into the model from a file.#n
#i<Datum Required :>The datum of the mill relative to the common datum of the system.#n

#n#h<Variables calculated by the model>#n
#i<Datum :>The height of the Mill above the system datum.#n
#i<Calc_Power :>#NThe power required to produce particles corresponding to the discharge 
  partion curve.  This number is produced from a simple calculation : 
  #n10 #* Feed tonnage #* Bond Work index #* (1/Sqrt(P80) - 1/Sqrt(F80))#n
#i<Calc_Heat :>#NThe power used to heat material in the mill.  This value is calculated 
  as follows : (Installed Power - Calc Power) #* HeatPerc.#n
#n#h<Other>#n
Default model prefix:MLL#n
Short name:Mill#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

//XID xidMethod = MdlBsXID(10004);

const int ioidFeed     = FlwIOId(0);
const int ioidProd     = FlwIOId(1);
const int EIOId_Supply = ElecIOId(0);

static IOAreaRec MillIOAreaList[] =
  {{"Mill Feed",       "Feed"      , ioidFeed, LIO_In0 ,  nc_MLnk, 1, 20, /*dwIOIsBuffer|*/IOGRP(1), 0.0F},
   {"Mill Discharge",  "Product"   , ioidProd, LIO_Out0,  nc_MLnk, 1,  1, IOSetXfer|IOGRP(1), 0.0F},
    SPILL2AREA,
    VENT2AREA,
   {"ElecSupply",    "ElecIn",     EIOId_Supply, LIO_In,   nc_ELnk, 0,50},
   {NULL}};
           
/*long Drw_Mill[] = {DD_Poly, -4,-2, -4,2, -2,8, 2,8, 4,2, 4,-2, 2,-8, -2,-8, -4,-2,
                         DD_Poly, -2,-8, -2,8,
                         DD_Poly, 2,-8, 2,8,
                         DD_Poly, -4,-2, -10,2, -10,4, -6,4, -4,2,
                         DD_End };*/
static double Drw_Mill[] = {DD_Poly, -6,-3, -6,3, -3,12, 3,12, 6,3, 6,-3, 3,-12, -3,-12, -6,-3,
                         DD_Poly, -3,-12, -3,12,
                         DD_Poly, 3,-12, 3,12,
                         DD_Poly, -6,-3, -15,3, -15,6, -9,6, -6,3,
                         DD_End };

//===========================================================================

IMPLEMENT_MODELUNIT(Mill1, "Mill", "1", Drw_Mill, "SizeAlteration", "MLL", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_SIZEDST|TOC_SIZEDIST, 
                    "Size Alteration:Mill(1)", "Simple Mill model")

Mill1::Mill1 (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  CB(&CrushBlkClass, "CB", this, TOA_Embedded),
  Disch("Disch", this, TOA_Embedded),
  MSB("PL_Basic", this, &m_Pwr),
  m_Pwr("525c_A/C", this, TOA_Embedded)
  {               
  AttachClassInfo(nc_Process, MillIOAreaList);
  //AttachIOAreas(MillIOAreaList);
  Contents.SetClosed(False);
  SS_Lvl = 0.25;

  bOnLine = true;
  DischOnSpeed=0.9;
  Motor = 1000.0;
  Power = 800.0;
  BWI   = 16.0;
  Eff   = 0.75;
  Heat  = 0.0;
  }

                
//--------------------------------------------------------------------------

Mill1::~Mill1 ()
  {
  }

//--------------------------------------------------------------------------

void Mill1::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDB.CheckBox("On", "", DC_, "", &bOnLine, this, isParm);
  DDB.Text("Power");
  DDB.Double("InstalledPower", "Motor",     DC_Pwr,   "kW",    &Motor,      this, isParm);
  DDB.Double("PercentHeat",    "HeatPerc",  DC_Frac,  "%",     &Eff,        this, isParm);
  DDB.Double("Bond_WI",        "BondWI",    DC_WI ,   "kWh/t", &BWI,        this, isParm);
  DDB.Double("Calc_Power",     "UsedPower", DC_Pwr,   "kW",    &Power,      this, isResult|0);
  DDB.Double("Calc_Heat",      "HeatPower", DC_Pwr,   "kW",    &Heat,       this, isResult|0);

  DDB.Text("");
  DDB.Double("Density",        "Rho",       DC_Rho,  "kg/m^3", xidRho,      this, isResult|0);
  BuildDataDefnElevation(DDB);
  CB.Add_StandardDataDefn(DDB, true);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Text("");
  DDB.Double("DischOnSpeed",   "",          DC_Frac,  "%",     &DischOnSpeed, this, isParm);
  MSB.BuildDataDefn(DDB, this, "Speed", 1);
  m_Pwr.BuildDataDefn(DDB);
  DDB.Visibility();
  
  RB.Add_OnOff(DDB);

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  CB.Add_ObjectDataDefn(DDB, true);
  RB.BuildDataDefn(DDB);

  if (NetDynamicMethod())
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&m_PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.Object(&Disch,     this, NULL, NULL, DDB_RqdPage);

  
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag Mill1::DataXchg(DataChangeBlk & DCB)
  {
  if (CB.DataXchg(DCB))
    return 1;

  if (MSB.DataXchg(DCB)) 
    return 1;
  if (m_Pwr.DataXchg(DCB))
    return 1;

  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag Mill1::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MN_Surge::ValidateData(VDB);
  //OK = OK && CB.ValidateData(VDB);
  return OK;
  }

//-------------------------------------------------------------------------- 

void Mill1::StartSolution()
  {
  MN_Surge::StartSolution();
  }
 
//-------------------------------------------------------------------------- 

/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/ 
void Mill1::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      MdlNode::EvalJoinPressures(JoinMask);
      break;
    case NM_Dynamic:
      IOP_RhoH_Info RhoHInfo(Contents);
      double Pm=ContainerMeanPress(RhoHInfo, POffset);
      Set_JoinP(0, Pm);
      for (int i=0; (i<NoProcessIOs()) && (IOId_Self(i)<IOId_HX); i++)
        Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
      break;
    }
  };

//--------------------------------------------------------------------------

void Mill1::EvalJoinFlows(int JoinNo)
  {
  switch (NetMethod())
    {
    case NM_Probal:
    case NM_Dynamic:
      break;
    }
  };

//--------------------------------------------------------------------------

void Mill1::EvalSteadyState()
  {
  };

//--------------------------------------------------------------------------

void Mill1::EvalProducts(CNodeEvalIndex & NEI)
  {
  Eff = Range(0.0, Eff, 1.0);
  flag On = (bOnLine && (NetProbalMethod() || MSB.Speed(this)>DischOnSpeed));
  const int ioProd = IOWithId_Self(ioidProd);

  for (int i=0; i<NoFlwIOs(); i++)
    if (IOId_Self(i)==ioidFeed && IO_Out(i))
      IOConduit(i)->QZero();
  
  if (ioProd>=0 && On)
    {
    StkSpConduit QFd("Fd", chLINEID(), this);

    SigmaQInPMin(QFd(), som_ALL, Id_2_Mask(ioidFeed));
    BOOL ValidData = (QFd().QMass(som_Sol)>1.0e-6);
    //SpConduit & QFd = *IOConduit(ioidFeed);

    Disch.QCopy(QFd());

    //SpConduit & QFd = *IOConduit(ioidFeed);
    ValidData = ValidData && CB.IsValidData(Disch);//QFd);

    if (ValidData && On)
      {
      double F80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);

      CB.CrushProduct(QFd(), Disch);

      double Excess;
      double MassRate = Disch.QMass(som_Sol);
      double P80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);
      if (Valid(P80) && (P80 > 1e-10) && Valid(F80) && (F80 > 1e-10))
        {
        F80 = F80 * 1e6;  // Convert to microns
        P80 = P80 * 1e6;
        Power  = 10.0 * MassRate * BWI * (1.0/Sqrt(P80) - 1.0/Sqrt(F80));  
        Excess = Motor - Power;
        }
      else
        {
        Power  = 0.0;
        Excess = 0.0;
        }
      if (Excess > 0.0)
        Heat = Excess * Eff;
      else
        Heat = 0.0;

      Disch.Set_totHf(Disch.totHf() + Heat);
      }
    else
      {
      //Disch.QZero();
      Power = 0.0;
      }

    IOConduit(ioProd)->QCopy(Disch);
    }
  else if (ioProd>=0)
    {
    SpConduit & Qp=*IOConduit(ioProd); //Qp product
    Qp.QZero();
    SigmaQInPMin(Qp, som_ALL, Id_2_Mask(ioidFeed)); //set product = feed
    }
  }

//--------------------------------------------------------------------------

void Mill1::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  MSB.Ctrl_ConnIDStr(i, ID, Tg);
  }

//--------------------------------------------------------------------------

void Mill1::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  MSB.EvalCtrlActions(this);
  }

//---------------------------------------------------------------------------

void Mill1::EvalPowerRequired()
  {
  m_Pwr.Zero();
  double ShftPwr=Power;//FB.ShaftPower();
  if (Valid(ShftPwr))
    m_Pwr.SetPower(ShftPwr);
  }

//--------------------------------------------------------------------------

void Mill1::ClosureInfo()
  {
  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    SpConduit &Sd=*GetWrkConduit("SNLF", chLINEID());

    int ioProd=IOWithId_Self(ioidProd);
    rSpConduit Pr =*IOConduit(ioProd);

    SigmaQInPMin(Sd, som_ALL, Id_2_Mask(ioidFeed));

    CI.m_HfGain+=Sd.totHf();
    CI.m_HsGain+=Sd.totHs(); 
    CI.m_HfLoss+=Pr.totHf();
    CI.m_HsLoss+=Pr.totHs(); 
    if (m_EHX.Enabled())
      CI.m_EHXPowerIn += m_EHX.HeatFlow();
    }
  };

//===========================================================================
