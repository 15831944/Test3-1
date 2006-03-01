//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __CRUSHER_CPP
#include "crusher.h"
#include "dbgmngr.h"

//#include "models.h"

#define dbgModels  WITHDEBUG
#if dbgModels
static CDbgMngr dbgCrusher("Models", "Crusher");
#endif

//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

//===========================================================================
//
//                                Crusher
//
//===========================================================================
/*#D:#T:Crusher
#X:#h<General Description>#n
This model is used to simulate a general crusher.  This is a very simple model 
for a crusher, which requires the user to either specify a discharge partition 
curve for the crusher product, or a selection / breakage function.
#nIf the discharge partition curve is chosen, the product from the crusher will 
always have the specified partition curve, no matter what the crusher feed 
characteristics are.
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
I            Input          1              1#n
O            Output         1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#n#i<Bond_WI:> The Bond Work index of the material to be crushed.  This number does  
not affect the discharge partition curve, and is only used to calculate the power  
used by the crusher. 
#n#i<Method:> The method by which the model must determine the crusher product.  Two 
choices are available:
#n#i<Select/Break:> The user specifies the selection / breakage function for the crusher.
#n#i<DischPart:> The discharge partition curve for the crusher product.  This curve 
can be read into the model from a file. 
#n#i<Datum Required:> The datum of the crusher relative to the common datum of the system. 

#n#n
#h<Associated variables>#n
#n#i<Datum:> The height of the crusher above the system datum. 
#n#i<Calc_Power:> The power required to produce the crusher product.  This number is 
calculated using the following formula: 
#n10 #* Feed tonnage #* Bond Work index #* (1/Sqrt(P80) - 1/Sqrt(F80)) 

#n#h<Other>#n
Default model prefix:CRS#n
Short name:Crusher#n
Model type:Unit#n
#G:Units
*/
//====================================================================================

const int ioidFeed     = FlwIOId(0);
//const int ioidFeedAir=1;
const int ioidProd     = FlwIOId(1);
const int EIOId_Supply = ElecIOId(0);

static IOAreaRec CrusherIOAreaList[] =
  {{"",              "Feed",       ioidFeed, LIO_In0,   nc_MLnk, 1, 20, IOSetXfer|IOGRP(1), 0.0F},
   {"",              "Product",    ioidProd, LIO_Out0,  nc_MLnk, 1,  1, IOSetXfer|IOGRP(1), 0.0F},
   {"ElecSupply",    "ElecIn",     EIOId_Supply, LIO_In,   nc_ELnk, 0,50},
   {NULL}};
           
static double Drw_Crusher[] = { DD_Poly, -10,-8, -10,8, 10,8, 10,-8, -10,-8,
                       DD_Poly, -8,8, -4,-8,
                       DD_Poly, -6,8, -2,-8,
                       DD_Poly,  6,8,  2,-8,
                       DD_Poly,  8,8,  4,-8,
                       DD_End };

//===========================================================================

IMPLEMENT_MODELUNITHIDDEN(Crusher, "Crusher", "", Drw_Crusher, "SizeAlteration", "CRSR", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_SIZEDST|TOC_SIZEDIST, 
                          "Size Alteration:Crusher", "") 

Crusher::Crusher (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  BWDist("DataOnly", "BWDist", this, TOA_Embedded, "Size", "Dist"),
  EDist("DataOnly", "E_Dist", this, TOA_Embedded),
  SelBrk("SelBreak", this, TOA_Embedded),
  DischPart("Partition", "DischPart", this, TOA_Embedded),
  Disch("Disch", this, TOA_Embedded),
  MSB("PL_Basic", this, &m_Pwr),
  m_Pwr("525c_A/C", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, CrusherIOAreaList);

  Power=0.0;

  BWI=16.0;
  iMethod=CM_PartCrv;
  }


//--------------------------------------------------------------------------

Crusher::~Crusher ()
  {
  };

//--------------------------------------------------------------------------

void Crusher::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  BuildDataDefnElevation(DDB);
  DDB.Text("");

  DDB.Text("");
  static DDBValueLst DDB0[]={
    {CM_PartCrv,  "PartitionCrv"},
    {CM_SelBreak, "Select/Break"},
    {0}};
  DDB.Byte  ("Method",   "", DC_,     "",  &iMethod,  this, isParm|DDEF_SETONCHANGE, DDB0);
//?  DDB.Bool("UseSelBrk",   "",  DC_,  "", &fUseSelBrk,   this, isParm, DDBYesNo);

  DDB.Text("");
  DDB.Double("Bond_WI",      "Bond_WI",    DC_WI ,  "kWh/t",  &BWI  ,   this, isParm);
  DDB.Double("Calc_Power",   "CalcPower",  DC_Pwr,  "kW",     &Power,   this, isResult|0);

  //DDB.Double("Density",      "Rho",  DC_Rho,  "kg/m^3", xidRho,   this, 0);
  //DDB.Double("Level",        "Lvl",  DC_Frac, "%",      xidLevel, this, 0);


  MSB.BuildDataDefn(DDB, this, "Speed", 1);
  m_Pwr.BuildDataDefn(DDB);
  //DDB.Object(&BWDist,    this);
  //DDB.Object(&EDist,     this);
  if (iMethod==CM_SelBreak)
    DDB.Object(&SelBrk, this);
  else
    DDB.Object(&DischPart, this);

  DDB.Object(&Disch,     this, NULL, NULL, DDB_RqdPage);
  

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag Crusher::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB)) 
    return 1;
  //if (Contents.DataXchg(DCB)) 
  //  return 1;
  if (MSB.DataXchg(DCB))
    return 1;
  if (m_Pwr.DataXchg(DCB))
    return 1;
  return 0;
  }

//--------------------------------------------------------------------------

flag Crusher::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  if ((DischPart.Length()<=2) && (iMethod==CM_PartCrv))    
    {
    LogNote(Tag(), 0, "Bad Discharge Partition curve");
    //OK=0;
    }
  //DischPart.SetXPts(SQSzDist1::TylerSeries);


  return OK;
  }

//-------------------------------------------------------------------------- 

Z_States Crusher::Set_Zs(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  return SetDatums_Node(Pass, NULL, IOIn, Zi, Z_State_Src);
  };
 
//-------------------------------------------------------------------------- 

flag Crusher::Set_Sizes()
  {
  return True;
  };

//---------------------------------------------------------------------------

void Crusher::ConfigureJoins()
  {
  Init_NJoins(1);
  int i;
  switch (SolveMode())
    {
    case SM_Probal:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case SM_DynXfer:
    case SM_DynFull:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  };

//--------------------------------------------------------------------------

flag Crusher::InitialiseSolution()
  {
  // Make sure that Meansize & Appearance & hence other curves all have pts at the same places
  //if (DischPart.GetLen()>2)
  //  DischPart.SetXPts(SQSzDist1::TylerSeries);
  
  return True;
  };

//--------------------------------------------------------------------------

void Crusher::EvalProducts(long JoinMask)
  {
  flag On=(ProbalMode() || MSB.Speed(this)>0.9);

  int ioProd=IOWithId_Self(ioidProd);
  if (ioProd>=0 && On)
    {
    SigmaQInPMin(Disch, som_ALL, Id_2_Mask(ioidFeed));

    if (iMethod==CM_SelBreak)
      {
      if (SelBrk.Length()>2) 
        {
        double F80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);

        SQSzDist1::Ptr(Disch.Model())->Break0(SelBrk);

        double MassRate, P80;
        //double Excess;
        P80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);
        MassRate = Disch.QMass(som_Sol);
        if (Valid(P80) && (P80 > 1e-10) && Valid(F80) && (F80 > 1e-10))
          {
          F80 = F80 * 1e6;  // Convert to microns
          P80 = P80 * 1e6;
          Power  = 10.0 * MassRate * BWI * (1.0/Sqrt(P80) - 1.0/Sqrt(F80));  
        //  Excess = Motor - Power;
          }
        else
          {
          Power  = 0.0;
        //  Excess = 0.0;
          }
        //if (Excess > 0.0)
        //  Heat = Excess * Eff;
        //else
        //  Heat = 0.0;
        //
        //Disch.Set_totEnthalpy(Disch.totEnthalpy() + Heat);
        }
      else
        Power = 0.0;
      }
    else
      {
      if (DischPart.Length()>2) 
        {
        double F80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);

        SQSzDist1::Ptr(Disch.Model())->Crush0(DischPart);//, 0);

        double MassRate, P80;
        //double Excess;
        P80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);
        MassRate = Disch.QMass(som_Sol);
        if (Valid(P80) && (P80 > 1e-10) && Valid(F80) && (F80 > 1e-10))
          {
          F80 = F80 * 1e6;  // Convert to microns
          P80 = P80 * 1e6;
          Power  = 10.0 * MassRate * BWI * (1.0/Sqrt(P80) - 1.0/Sqrt(F80));  
        //  Excess = Motor - Power;
          }
        else
          {
          Power  = 0.0;
        //  Excess = 0.0;
          }
        //if (Excess > 0.0)
        //  Heat = Excess * Eff;
        //else
        //  Heat = 0.0;
        //
        //Disch.Set_totEnthalpy(Disch.totEnthalpy() + Heat);
        }
      else
        Power = 0.0;
      }

    IOConduit(ioProd)->QCopy(Disch);
    }
  else if (ioProd>=0)
    IOConduit(ioProd)->QZero();
  }

//--------------------------------------------------------------------------

void Crusher::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  MSB.Ctrl_ConnIDStr(i, ID, Tg);
  }

// --------------------------------------------------------------------------

void Crusher::EvalCtrlActions()
  {
  //int io=CIOWithId_Self(CIOId_Spd);
  //if (io>=0)
  //  MSB.SetSpeed(CIO_Value(io));
  //io=EIOWithId_Self(EIOId_Supply);
  //if (io>=0 && !EIO_Conduit(io).Live())
  //  MSB.SetOn(0);

  MSB.EvalCtrlActions(this);
  }

//---------------------------------------------------------------------------

void Crusher::EvalPowerRequired()
  {
  m_Pwr.Zero();
  double ShftPwr=Power;//FB.ShaftPower();
  if (Valid(ShftPwr))
    m_Pwr.SetPower(ShftPwr);
  }

//===========================================================================
