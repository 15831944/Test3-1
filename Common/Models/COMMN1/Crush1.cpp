//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __CRUSH1_CPP
#include "crush1.h"
//#include "optoff.h"

//===========================================================================

XID xidMethod           = MdlBsXID(10021);
XID xidEnterExtents     = MdlBsXID(10022);

IMPLEMENT_TAGOBJ(CrushBlk, "CrshBlk", "1", "CrshBlk", "", "CB", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST, "Crush Blk", "Crush1 Block");

CrushBlk::CrushBlk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, TagIn, pAttach, eAttach),
  SelBrk("SelBreak", this, TOA_Embedded),
  DischParts("DischPart", this, TOA_Embedded),
  DataParts("TestData", this, TOA_Embedded),
  Extents("Extents", this, TOA_Embedded)
  {
  iMethod=CM_PartCrv_Individ;
  bForcePartCrv = false;
  bEnterExtents = false;
  bTrackStatus = true;

  dK1 = 0.001;
  dK2 = 0.1;
  dK3 = 2.3;
  dt10 = 15.0;

  DischParts.SetNCurves(1);
  DischParts.SetLength(5);
  DischParts.SetModeVisibility(false, "", "");
  DischParts.SetStateVisibility(true, "On", "Off");
  DischParts.SetCumulativeVisibility(true);
  for (int c=0; c<DischParts.NCurves(); c++)
    {
    DischParts.SetEditable(c, true);
    DischParts.SetAutoCorrect(c, true);
    }

  DataParts.SetNCurves(2);
  DataParts.SetLength(5);
  DataParts.SetModeVisibility(false, "", "");
  DataParts.SetStateVisibility(false, "On", "Off");
  DataParts.SetTitle(0, "In0");
  DataParts.SetTitle(1, "Out0");
  DataParts.SetCumulativeVisibility(true);

  Extents.SetNCurves(1);
  Extents.SetLength(5);
  Extents.SetModeVisibility(false, "", "");
  Extents.SetStateVisibility(true, "On", "Off");
  Extents.SetCumulativeVisibility(false);
  }

//--------------------------------------------------------------------------

CrushBlk::~CrushBlk()
  {
  }

//--------------------------------------------------------------------------

void CrushBlk::Add_StandardDataDefn(DataDefnBlk & DDB, flag Vis)
  {
  DDEF_Flags Old = DDB.GetVisibility();
  DDB.Visibility(NSHM_All, Vis);
  DDB.Text("");
  DDB.CheckBox("TrackStatus", "", DC_, "", &bTrackStatus, this, isParm);
  static DDBValueLst DDB0[]={
    {(int)CM_PartCrv_Overall, "Overall_PartCrv"},
    {(int)CM_PartCrv_Individ, "Individual_PartCrv" },
    {(int)CM_SelBreak, "Select/Break"},
    {(int)CM_TestData_Overall, "Overall_TestData"},
    {(int)CM_TestData_Individ, "Individual_TestData" },
    //{(int)CM_JK, "JK" },
    {0}};
  DDB.Byte    ("Method",       "", DC_, "", xidMethod,      this, isParmStopped/*|DDEF_SETONCHANGE*/, DDB0);
  DDB.Visibility(NSHM_All, Vis && (iMethod==CM_PartCrv_Overall || iMethod==CM_PartCrv_Individ));
  DDB.CheckBox("ForcePartCrv", "", DC_, "", &bForcePartCrv, this, isParm, DDBYesNo);
  DDB.Visibility(NSHM_All, Vis && (iMethod==CM_TestData_Overall || iMethod==CM_TestData_Individ));
  DDB.CheckBox("EnterExtents", "", DC_, "", xidEnterExtents, this, isParmStopped, DDBYesNo);
  DDB.Visibility(NSHM_All, Vis && (iMethod==CM_JK));
  DDB.Double("",          "K1",         DC_L,    "mm",   &dK1,     this,  isParm);
  DDB.Double("",          "K2",         DC_L,    "mm",   &dK2,     this,  isParm);
  DDB.Double("",          "K3",         DC_,     "",     &dK3,     this,  isParm);
  DDB.Double("",          "t10",        DC_,     "",     &dt10,    this,  isParm);
  //DDB.Visibility(NSHM_All, Vis);
  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

void CrushBlk::Add_ObjectDataDefn(DataDefnBlk & DDB, flag Vis)
  {
  DDEF_Flags Old = DDB.GetVisibility();
  DDB.Visibility(NSHM_All, Vis);
  switch (iMethod)
    {
    case CM_PartCrv_Overall:
    case CM_PartCrv_Individ:
      DDB.Object(&DischParts, this, NULL, NULL, DDB_RqdPage);
      break;
    case CM_SelBreak:
    case CM_JK:
      DDB.Object(&SelBrk, this, NULL, NULL, DDB_RqdPage);
      break;
    case CM_TestData_Overall:
    case CM_TestData_Individ:
      if (!bEnterExtents)
        DDB.Object(&DataParts, this, NULL, NULL, DDB_RqdPage);
      DDB.Object(&Extents, this, NULL, NULL, DDB_RqdPage);
      break;
    }
  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

void CrushBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  }

//--------------------------------------------------------------------------

flag CrushBlk::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidMethod:
      if (DCB.rB)
        {
        iMethod=*DCB.rB;
        switch (iMethod)
          {
          case CM_PartCrv_Overall:
          case CM_PartCrv_Individ:
            {
            int N0=DischParts.NCurves();
            if (iMethod==CM_PartCrv_Overall)
              DischParts.SetNCurves(1);
            else
              DischParts.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
            for (int c=Max(N0,1); c<DischParts.NCurves(); c++)
              DischParts.Curve(c) = DischParts.Curve(c-1);
            for (c=0; c<DischParts.NCurves(); c++)
              {
              DischParts.SetEditable(c, true);
              DischParts.SetAutoCorrect(c, true);
              }
            break; //assumes one sieve-size/distribution only
            }
          case CM_SelBreak:
            break;
          case CM_JK:
            break;
          case CM_TestData_Overall:
          case CM_TestData_Individ:
            {
            if (iMethod==CM_TestData_Overall)
              {
              DataParts.SetNCurves(2);
              Extents.SetNCurves(1);
              }
            else
              {
              DataParts.SetNCurves(Max(2, SD_Defn.NPriIds(0)*2));
              Extents.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
              }
            Strng Nm;
            for (int c=0; c<DataParts.NCurves()/2; c++)
              {
              Nm.Set("Frac_In%d", c);
              DataParts.SetTitle((c*2), Nm());
              Nm.Set("Frac_Out%d", c);
              DataParts.SetTitle((c*2)+1, Nm());
              /*Nm.Set("Cum_In%d", c);
              DataParts.SetTitle(DataParts.NCurves()+(c*2), Nm());
              Nm.Set("Cum_Out%d", c);
              DataParts.SetTitle(DataParts.NCurves()+(c*2)+1, Nm());*/
              }
            for (c=0; c<DataParts.NCurves(); c++)
              {
              DataParts.SetEditable(c, true);
              DataParts.SetAutoCorrect(c, true);
              }
            for (c=0; c<Extents.NCurves(); c++)
              Extents.SetEditable(c, bEnterExtents);
            break; //assumes one sieve-size/distribution only
            }
          }
        }
      DCB.B=iMethod;
      return 1;
    case xidEnterExtents:
      if (DCB.rB)
        {
        bEnterExtents=*DCB.rB;
        for (int c=0; c<Extents.NCurves(); c++)
          Extents.SetEditable(c, bEnterExtents);
        }
      DCB.B=bEnterExtents;
      return 1;
    }
  
  return 0;
  }

//--------------------------------------------------------------------------

flag CrushBlk::ValidateData(ValidateDataBlk & VDB)
  {
  switch (iMethod)
    {
    case CM_PartCrv_Overall:
    case CM_PartCrv_Individ:
      if (DischParts.Length()<=2)
        LogWarning(BaseTag(), 0, "Bad Discharge Partition curve");
      break;
    case CM_SelBreak:
    case CM_JK:
      break;
    case CM_TestData_Overall:
    case CM_TestData_Individ:
      {
      if (DataParts.Length()<=2)
        LogWarning(BaseTag(), 0, "Bad Discharge Partition curve");
      /*if (bEnterExtents)
        for (int c=0; c<Extents.NCurves(); c++)
          {
		      CDArray & Ext=Extents.Curve(c);
          Ext[0] = 1.0;
          }
      else
        CalcExtents();*/
      break;
      }
    }
  return true;
  }

//--------------------------------------------------------------------------

flag CrushBlk::IsValidData(SpConduit & QFd)
  {
  flag ValidData = true;
  switch (iMethod)
    {
    case CM_PartCrv_Overall:
    case CM_PartCrv_Individ:
      if (DischParts.Length()<=2)
        ValidData = false;
      break;
    case CM_SelBreak:
      break;
    case CM_JK:
      if (dK1>=dK2)
        ValidData = false;
      break;
    case CM_TestData_Overall:
    case CM_TestData_Individ:
      if (DataParts.Length()<=2)
        ValidData = false;
      break;
    }

  SpModel * pMdl = QFd.Model();
  //SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, false);
  SQSzDist1 * pSz = SQSzDist1::Ptr(pMdl, true);
  if (pSz && pSz->DistributionsExist())
    ClrCI(1);
  else
    {
    if (QFd.QMass()>UsableMass)
      SetCI(1);
    else
      ClrCI(1);
    ValidData = false;
    }

  if (ValidData)
    {
    if (DischParts.GetSizeDefn()!=pSz->DistIndex())
      DischParts.SetSizeDefn(pSz->DistIndex());//change to correct sizedistribution defn
    if (DataParts.GetSizeDefn()!=pSz->DistIndex())
      DataParts.SetSizeDefn(pSz->DistIndex());//change to correct sizedistribution defn
    if (Extents.GetSizeDefn()!=pSz->DistIndex())
      Extents.SetSizeDefn(pSz->DistIndex());//change to correct sizedistribution defn
    if (SelBrk.GetSizeDefn()!=pSz->DistIndex())
      SelBrk.SetSizeDefn(pSz->DistIndex());//change to correct sizedistribution defn
    }
  return ValidData;
  }

//--------------------------------------------------------------------------

void CrushBlk::CrushProduct(SpConduit & QFd, SpConduit & Disch)
  {
  switch (iMethod)
    {
    case CM_JK:
      if (DischParts.IsCurveOn(0))
        {
        const int len = SelBrk.Length();
        for (int i=0; i<len; i++)
          {
          const double Siz = SelBrk.SizePts()[i];
          if (Siz<dK1)
            SelBrk.SetSelection(i, 0.0);
          else if (Siz>dK2)
            SelBrk.SetSelection(i, 1.0);
          else
            SelBrk.SetSelection(i, 1.0-Pow((dK2-Siz)/(dK2-dK1), dK3));
          }
        SQSzDist1::Ptr(Disch.Model())->Break0(SelBrk);
        }
      break;
    case CM_SelBreak:
      if (DischParts.IsCurveOn(0))
        SQSzDist1::Ptr(Disch.Model())->Break0(SelBrk);
      break;
    case CM_PartCrv_Individ:
    case CM_PartCrv_Overall:
      {
      if (iMethod==CM_PartCrv_Overall)
        {
        if (DischParts.IsCurveOn(0))
          SQSzDist1::Ptr(Disch.Model())->Crush0(DischParts, DischParts.NCurves());
        }
      else
        {
        for (int c=0; c<DischParts.NCurves(); c++)
          if (DischParts.IsCurveOn(c))
            //Crush1(Specie with SizeDist Index, curve data object, curve number of Cum % to be used);
            SQSzDist1::Ptr(Disch.Model())->Crush1(c, DischParts, DischParts.NCurves()+c);
        }
      flag DoSetCI=false;
      for (int c=0; c<DischParts.NCurves(); c++)
        {
        if (!SQSzDist1::Ptr(Disch.Model())->CheckDischIsValid1(c, QFd))
          {
          DoSetCI=true;
          if (!bForcePartCrv)
            SQSzDist1::Ptr(Disch.Model())->CorrectDisch1(c, QFd);
          }
        }
      SetCI(2, bTrackStatus && DoSetCI);
      break;
      }
    case CM_TestData_Overall:
    case CM_TestData_Individ:
      if (!bEnterExtents)
        CalcExtents();
      if (iMethod==CM_TestData_Overall)
        {
        if (Extents.IsCurveOn(0))
          SQSzDist1::Ptr(Disch.Model())->CrushExt0(Extents, QFd, 0);
        }
      else
        {
        for (int c=0; c<Extents.NCurves(); c++)
          if (Extents.IsCurveOn(c))
            //CrushExt1(Specie with SizeDist Index, breakage extents data object, feed distribution, curve number to be used);
            SQSzDist1::Ptr(Disch.Model())->CrushExt1(c, Extents, QFd, c);
        }

      break;
    }
  }

//--------------------------------------------------------------------------

void CrushBlk::CalcExtents()
  {
  ASSERT(iMethod==CM_TestData_Overall || iMethod==CM_TestData_Individ);

  for (int c=0; c<Extents.NCurves(); c++)
    {
    CDArray & Ext=Extents.Curve(c);
    CDArray & In=DataParts.Curve(c*2);
    CDArray & Out=DataParts.Curve((c*2)+1);
    double prev = 0.0;
    for (int i=Ext.GetSize()-1; i>0; i--)
      {
      double AmntAvail = prev + In[i];
      double Extent = (AmntAvail<1e-12 ? 0.0 : Range(0.0, Out[i]/AmntAvail, 1.0));
      prev = AmntAvail - (AmntAvail*Extent);
      Ext[i] = Extent;
      }
    Ext[0] = 1.0;
    for (i=1; (i<Ext.GetSize() && Ext[i]<1e-12); i++)
      Ext[i] = 1.0;
    }
  }

//--------------------------------------------------------------------------

flag CrushBlk::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tNo Size distributions defined";   return 1;
    case  2: 
      if (bForcePartCrv)
        pS="N\tForced larger sizes";
      else
        pS="N\tSizes redistributed";
      return 1;
    default:                                               
      return TaggedObject::CIStrng(No, pS);
    }
  }




//===========================================================================
//
//                                Crush1
//
//===========================================================================
/*#D:#T:Crush1
#X:#h<General Description>#n
This model is used to simulate a general Crush1.  This is a very simple model 
for a Crush1, which requires the user to either specify a discharge partition 
curve for the Crush1 product, or a selection / breakage function.
#nIf the discharge partition curve is chosen, the product from the Crush1 will 
always have the specified partition curve, no matter what the Crush1 feed 
characteristics are.
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
I            Input          1              1#n
O            Output         1              1#n
#a#n
#h<Variables to be supplied by the user>#n
#n#i<Bond_WI:> The Bond Work index of the material to be crushed.  This number does  
not affect the discharge partition curve, and is only used to calculate the power  
used by the Crush1. 
#n#i<Method:> The method by which the model must determine the Crush1 product.  Two 
choices are available:
#n#i<Select/Break:> The user specifies the selection / breakage function for the Crush1.
#n#i<DischPart:> The discharge partition curve for the Crush1 product.  This curve 
can be read into the model from a file. 
#n#i<Datum Required:> The datum of the Crush1 relative to the common datum of the system. 

#n#n
#h<Associated variables>#n
#n#i<Datum:> The height of the Crush1 above the system datum. 
#n#i<Calc_Power:> The power required to produce the Crush1 product.  This number is 
calculated using the following formula: 
#n10 #* Feed tonnage #* Bond Work index #* (1/Sqrt(P80) - 1/Sqrt(F80)) 

#n#h<Other>#n
Default model prefix:CRS#n
Short name:Crush1#n
Model type:Unit#n
#G:Units
*/
//====================================================================================


const int ioidFeed     = FlwIOId(0);
const int ioidProd     = FlwIOId(1);
const int EIOId_Supply = ElecIOId(0);

static IOAreaRec Crush1IOAreaList[] =
  {{"",              "Feed"      , ioidFeed, LIO_In0 ,  nc_MLnk, 1, 20, IOSetXfer|IOGRP(1), 0.0F},
   {"",              "Product"   , ioidProd, LIO_Out0,  nc_MLnk, 1,  1, IOSetXfer|IOGRP(1), 0.0F},
   {"ElecSupply",    "ElecIn",     EIOId_Supply, LIO_In,   nc_ELnk, 0,50},
   {NULL}};
           
static double Drw_Crush1[] = { DD_Poly, -8.0,-5.6, -8.0,5.6, 8.0,5.6, 8.0,-5.6, -8.0,-5.6,
                               DD_Poly, -8.0,-5.6, -4.0,3.2, -4.0,5.6,
                               DD_Poly, 8.0,-5.6, 4.0,3.2, 4.0,5.6,
                               DD_Poly, -5.6,-4.0, -2.4,3.2, 2.4,3.2, 5.6,-4.0, -5.6,-4.0,
                               DD_TagPos, 0, -9.9,
                               DD_End };

//===========================================================================

IMPLEMENT_MODELUNIT(Crush1, "Crush", "1", Drw_Crush1, "SizeAlteration", "CRS", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_SIZEDST|TOC_SIZEDIST, 
                    "Size Alteration:Crush1", "") 

Crush1::Crush1 (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  CB(&CrushBlkClass, "CB", this, TOA_Embedded),
  Disch("Disch", this, TOA_Embedded),
  MSB("PL_Basic", this, &m_Pwr),
  m_Pwr("525c_A/C", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, Crush1IOAreaList);

  bOnLine = true;
  DischOnSpeed=0.9;
  Power=0.0;
  BWI=16.0;
  }

//--------------------------------------------------------------------------

Crush1::~Crush1 ()
  {
  }

//--------------------------------------------------------------------------

void Crush1::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDB.CheckBox("On", "", DC_, "", &bOnLine, this, isParm);
  DDB.Text("Power");
  DDB.Double("Bond_WI",        "BondWI",    DC_WI ,   "kWh/t", &BWI,        this, isParm);
  DDB.Double("Calc_Power",     "UsedPower", DC_Pwr,   "kW",    &Power,      this, isResult|0);

  DDB.Text("");
  DDB.Double("Density",        "Rho",       DC_Rho,   "kg/m^3", xidRho,      this, isResult|0);
  BuildDataDefnElevation(DDB);
  CB.Add_StandardDataDefn(DDB, true);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All);
  DDB.Text("");
  DDB.Double("DischOnSpeed",   "",          DC_Frac,  "%",     &DischOnSpeed, this, isParm);
  MSB.BuildDataDefn(DDB, this, "Speed", 1);
  m_Pwr.BuildDataDefn(DDB);
  DDB.Visibility();

  DDB.Text("");
  BuildDataDefnShowIOs(DDB);

  CB.Add_ObjectDataDefn(DDB, true);

  DDB.Object(&Disch,     this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag Crush1::DataXchg(DataChangeBlk & DCB)
  {
  if (CB.DataXchg(DCB))
    return 1;

  if (MSB.DataXchg(DCB))
    return 1;
  if (m_Pwr.DataXchg(DCB))
    return 1;
  if (MdlNode::DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag Crush1::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK = MdlNode::ValidateData(VDB);
  //OK = OK && CB.ValidateData(VDB);
  return OK;
  }

//-------------------------------------------------------------------------- 

void Crush1::SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn)
  {
  SetDatums_Node(Pass, List, IOIn, NULL);
  }
 
//-------------------------------------------------------------------------- 

flag Crush1::Set_Sizes()
  {
  return True;
  }

//---------------------------------------------------------------------------

void Crush1::ConfigureJoins()
  {
  Init_NoJoins(1);
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Open(i, 0, false, ESS_Denied);
      break;
    }
  };

//--------------------------------------------------------------------------

flag Crush1::InitialiseSolution()
  {
  return True;
  };

//--------------------------------------------------------------------------

void Crush1::EvalProducts(CNodeEvalIndex & NEI)
  {
  flag On = (bOnLine && (NetProbalMethod() || MSB.Speed(this)>DischOnSpeed));
  const int ioProd = IOWithId_Self(ioidProd);
  if (ioProd>=0 && On)
    {
    SigmaQInPMin(Disch, som_ALL, Id_2_Mask(ioidFeed));
    BOOL ValidData = (Disch.QMass(som_Sol)>1.0e-6);
		SpConduit Sd("Sd", this, TOA_Free);
    Sd.QCopy(Disch);
    ValidData = ValidData && CB.IsValidData(Sd);

    if (ValidData)
      {
      double F80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);

      CB.CrushProduct(Sd, Disch);

      //double Excess;
      double MassRate = Disch.QMass(som_Sol);
      double P80 = SQSzDist1::Ptr(Disch.Model())->SizePassingFraction(-1, -1, 0.80);
      if (Valid(P80) && (P80 > 1e-10) && Valid(F80) && (F80 > 1e-10))
        {
        F80 = F80 * 1e6;  // Convert to microns
        P80 = P80 * 1e6;
        Power  = 10.0 * MassRate * BWI * (1.0/Sqrt(P80) - 1.0/Sqrt(F80));  
        //Excess = Motor - Power;
        }
      else
        {
        Power  = 0.0;
        //Excess = 0.0;
        }
      //if (Excess > 0.0)
      //  Heat = Excess * Eff;
      //else
      //  Heat = 0.0;

      //Disch.Set_totEnthalpy(Disch.totEnthalpy() + Heat);
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

void Crush1::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  MSB.Ctrl_ConnIDStr(i, ID, Tg);
  }

// --------------------------------------------------------------------------

void Crush1::EvalCtrlActions(eScdCtrlTasks Tasks)
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

void Crush1::EvalPowerRequired()
  {
  m_Pwr.Zero();
  double ShftPwr=Power;//FB.ShaftPower();
  if (Valid(ShftPwr))
    m_Pwr.SetPower(ShftPwr);
  }

//--------------------------------------------------------------------------

dword Crush1::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs()>1)
    {
    const int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
    Status |= (IOConduit(iIn)->QMass()>gs_DisplayZeroFlow ? FNS_UFlw : FNS_UNoFlw);
    }
  else if (NoFlwIOs()==1)
    Status |= FNS_Error;
  return Status;
  }

//--------------------------------------------------------------------------
//===========================================================================
