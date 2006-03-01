//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __Crystal_CPP
#include "Crystal.h"

//==========================================================================
/*#D:#T:Crystallizer                       
#X:#h<General Description>#nThe Crystallizer acts as a closed vessel which 
is used to produce a slurry containing solid crystals.  The unit works using solubility 
curves, if these are an integral part of the project the user does not need to supply a 
further solubility curve.  If the project is not using solubility in all of the streams, 
then the user must input solubility curves into the crystallizer model.
The material enters the Crystallizer via the feed inlet and the slurry is removed 
from the slurry outlet.  The evaporated liquor is removed from the vapour outlet.
#nThe user may input a crytallizer discharge partition curve to define the size distribution 
of the produced crystals.#n
#n#h<Variables to be supplied by the user>#n
#i<Datum :>The relative elevation of the Crystallizer with respect to the common
datum of the system.#n
#i<DischCrystal :>The size distribution curve of the crytal product.#n
#n#h<Variables associated with the Crystallizer>
#n#i<Density :>The density of the material in the Crystallizer.#n
#i<Pressure :>The pressure on the surface of the material is equivalent to the vapour pressure of 
the gases in the Crystallizer.#n
#i<Level :>A ratio, expressed as a percentage, of the total volume of 
the solids and liquids, in the autoclave, to the volumetric capacity of the Crystallizer.#n
#i<Mass_Accum :>The rate at which the mass of the material accumulates in
the Crystallizer.#n
#i<Vol_Accum :>The rate at which the volume of the material accumulates in
the Crystallizer.#n
#n#h<Other>#n
Default model prefix:CRY#n
Short name:Crystal#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte id_Feed   = 0;
const byte id_Slurry = 1;
const byte id_Vapour = 2;

static IOAreaRec CrystalIOAreaList[] =
  {{"Crystallizer Feed",          "Feed"     , id_Feed,    LIO_In0,     nc_MLnk, 0, 20,     0, (float)0.5},
   {"Crystal Slurry",             "Slurry"   , id_Slurry,  LIO_Out0,    nc_MLnk, 0, 20,     0, (float)0.5},
   {"Vapour Outlet",              "Vapour"   , id_Vapour,  LIO_Out1,    nc_MLnk, 0, 5,      0, (float)1.0},
   {"Heat Exchange Element",      "HX"       , idHX,       LIO_InOut,  nc_MLnk, 2, 20, IOGRP(1) },
   {"Heat Exchange Element In",   "HX_In"    , idHX,       LIO_InOut,  nc_MLnk, 2, 20, IOGRP(1) },
   {"Heat Exchange Element Out",  "HX_Out"   , idHX,       LIO_InOut,  nc_MLnk, 2, 20, IOGRP(1) },
   {NULL}}; //This lists the areas of the model where links can be attached.


long Drw_Crystal[] = { DD_Poly, -7,0, 7,0, 10,3, -10,3, -10,15, 10,15, 7,18, -7,18, 
                                -7,28, 7,28, 5,30, -5,30, -5,35, 5,35, 4,36, -4,36,
																-5,35, 5,35, 5,30, -5,30, -7,28, 7,28, 7,18, -7,18,
																-10,15, 10,15, 10,3, -10,3, -7,0,
                       DD_End }; //This provides the default drawing of the Crystallizer.
//#E
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Crystal, "Crystal", "1", Drw_Crystal, "CRY", TOC_ALL|TOC_GRP_SIZEDST|TOC_USER,
                    "Crystallizer",
                    "General Purpose Crystallizer") 

Crystal::Crystal(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  DischCrystal("DischCrystal", this, TOA_Embedded),
  Soluble("Solubility", "Soluble", this, TOA_Embedded),
  Disch("Disch", this, TOA_Embedded)
	  {
  AttachIOAreas(CrystalIOAreaList);
  Contents.SetClosed(False);
  SS_Lvl = 0.25;
  PRqd   = 0.0;

  DischCrystal.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
  DischCrystal.SetLength(5);
  DischCrystal.SetModeVisibility(false, "", "");
  DischCrystal.SetStateVisibility(true, "On", "Off");
  DischCrystal.SetCumulativeVisibility(true);
  for (int c=0; c<DischCrystal.NCurves(); c++)
    {
    DischCrystal.SetEditable(c, true);
    DischCrystal.SetAutoCorrect(c, true);
    }

  VLE.Open(NULL, true);
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Crystal::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(DYNMODE);
	DDB.Text    ("");
  //DDB.Double  ("Datum",          "Z",           DC_L,    "m",       &Z,         this, 0);
  DDB.Double  ("Pressure",       "P",           DC_P,    "kPag",    xidPMean,   this, 0);
  DDB.Double  ("Temperature",    "T",           DC_T,    "C",       xidTemp,    this, 0);
  DDB.Double  ("Density",        "Rho",         DC_Rho,  "kg/m^3",  xidRho,     this, 0);
  DDB.Double  ("Level",          "Lvl",         DC_Frac, "%",       xidLevel,   this, 0);
  DDB.Double  ("Mass_Accum",     "QmAcc",       DC_Qm,   "kg/s",    &QmAcc,     this, 0);
  DDB.Double  ("Vol_Accum",      "QvAcc",       DC_Qv,   "L/s",     &QvAcc,     this, 0);
  DDB.Visibility();
  
  DDB.Text    ("");
  static DDBValueLst DDB1[]={
    {(int)True,  "Yes"},
    {(int)False, "No" },
    {0}};
  DDB.Bool    ("CurvesReqd",     "CurvesReqd",  DC_,     "",       &CR,         this, isParm, DDB1);
  DDB.Visibility(PBMODE|SSMODE);
  DDB.Double  ("Flash_Pressure", "RqdPressure", DC_P,    "kPag",   &PRqd,       this, isParm);
  DDB.Visibility();
  DDB.Text    ("");
  //DDB.Double  ("Datum_Rqd",      "Z_Rqd",       DC_L    , "m",     &Z_Rqd,      this, isParmStopped|NAN_OK);
  DDB.Bool    ("Closed",         "",            DC_     , "",      xidClosed,   this, isParm, DDBYesNo);
  HX.Add_OnOff(DDB);
  RB.Add_OnOff(DDB);
  
  BuildDataDefnShowIOs(DDB);

  if (SolveMode()==DYNMODE)
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  if (HX())
    DDB.Object(HX(), this, NULL, NULL, DDB_RqdPage);
  RB.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

  DDB.Object(&DischCrystal, this);
  DDB.Object(&Disch,     this, NULL, NULL, DDB_RqdPage);

	if (CR)
    DDB.Object(&Soluble, this);

  DDB.EndStruct();
 
  };

//--------------------------------------------------------------------------

flag Crystal::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  switch (DCB.lHandle)
    {
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB); 
      DCB.B=Contents.Closed(); 
      return 1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag Crystal::ValidateData(ValidateDataBlk & VDB)
  {
  Contents.ValidateData(VDB);

  if (DischCrystal.Length()<=2) 
    {
    LogNote(Tag(), 0, "Bad Discharge Crystal Size curve");
    }
  return MN_Surge::ValidateData(VDB);//True;
  }

void Crystal::StartSolution()
  {
  MN_Surge::StartSolution();
  }

//--------------------------------------------------------------------------
/*#F:This determines the pressure of the contents and the pressure at each
inlet and outlet of the surge unit.*/ 
void Crystal::EvalJoinPressures()
  {
  switch (SolveMode())
    {
    case PBMODE:
    case SSMODE:
      {
      for (int j=0; j<NJoins(); j++)
        {
        double Pm=(j==0 ? PRqd : GetPBInputPressure(j));
        SetPBJoinPressure(j, Pm, true, true);
        #if dbgFlwNode
        if (dbgEvalJoinPress(sTag()))
          dbgpln("EJP: %12.2f  %s[%i]", Pm, sTag(), j );
        #endif
        }
      }
      //MdlNode::EvalJoinPressures();
      break;
    case DYNMODE:
      #if dbgEvalPres
      dbgpln("EvalJoinPressures:%s",FullObjTag());
      #endif

      IOP_RhoH_Info RhoHInfo(Contents);
      double Pm=ContainerMeanPress(RhoHInfo, 0.0);
      for (int i = 0; (i<NoFlwIOs()) && (IOId_Self(i)<idHX); i++)
        {
        Set_IOP_RhoH_Self(i,Pm,RhoHInfo);
        #if dbgEvalPres
        dbgpln("  %#11.5g %#11.5g %#11.5g %#11.5g %#11.5g %s",
               Pm,Contents.Level(),pC->FHgt,Contents.ShapeHeight(),Contents.Rho(som_Sol|som_Liq),Nd_Rmt(i)->FullObjTag());
        #endif
        }
      break;
    }
  };
 
//--------------------------------------------------------------------------

void Crystal::EvalJoinFlows(int JoinNo)
  {
  switch (SolveMode())
    {
    case DYNMODE:
    case PBMODE:
      break;
    case SSMODE:
      {
      break;
      }
    }
  };
 
//--------------------------------------------------------------------------

void Crystal::EvalSteadyState()
  {
  };

//--------------------------------------------------------------------------

void Crystal::EvalProducts()
  {
  int iSl = IOWithId_Self(id_Slurry);
  rSpConduit Sl =*IOConduit(iSl);

  switch (SolveMode())
    {
    case PBMODE:
      {
			int ioHX_I = -1;
			int ioHX_O = -1;
			for (int i=0; i<NoFlwIOs(); i++)
				if (IOId_Self(i) == idHX)
					if (IO_In(i))
						ioHX_I = i;
					else
						ioHX_O = i;

      double ActQv, ActQl, P; 
      SigmaQInPMin(QFeed(), som_ALL, Id_2_Mask(id_Feed));
      int ioProd=IOWithId_Self(id_Slurry);

      VLE.QPFlash(QFeed(), PRqd, 0.0, 0);
      double temp= QFeed().Temp();

			if (HX() && (ioHX_I>=0) && (ioHX_O>=0))
				{
				double PriPIn = PRqd;
				HX.SetPrimary(NULL, QFeed(),PriPIn);

				rSpConduit CdOut = *IOConduit(ioHX_O);
				CdOut.QCopy(*IOConduit(ioHX_I));
				HX.SetSecondary(NULL, CdOut, IOP_Self(ioHX_I));
				}

      if (RB())
        RB.EvalProducts(QFeed());

			if (HX() && (ioHX_I>=0) && (ioHX_O>=0))
				HX.EvalProducts();

      ActQv = QFeed().QMass(som_Vap);
      ActQl = QFeed().QMass(som_SL);
      P     = PRqd;

      int nV=0, nL=0;
      for (i = 0; (i<NoFlwIOs()) && (IOId_Self(i)<idHX) ; i++)
        if (IO_Out(i))
          {
          if (IOId_Self(i)==id_Vapour)
            {
            IOConduit(i)->QSetM(QFeed(), som_Vap, ActQv, P);
            ActQv=0.0;
            }
          else
            {
            Disch.QSetM(QFeed(), som_SL, ActQl, P);

						for (int c=0; c<DischCrystal.NCurves(); c++)
							if (DischCrystal.IsCurveOn(c))
								SQSzDist1::Ptr(Disch.Model())->Crush1(c, DischCrystal, DischCrystal.NCurves()+c);
						
            //IOBuffer(i)->QCopy(Disch);
            //IOBuffer(i)->QSetF(Disch, som_ALL, 1.0, P);
            IOConduit(i)->QSetF(Disch, som_ALL, 1.0, P);

            ActQl=0.0;
            }
          }
      break;
      }
    default:
      MN_Surge::EvalProducts();
    }
  }
 
//==========================================================================
