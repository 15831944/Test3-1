//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __FLOT_CPP
#include "flot.h"

#define dbgFlot    0

//==========================================================================
/*#D:#T:Flotation Tank                      
#X:#h<General Description>#nThe flotation cell separates the feed stream 
into a tailings stream and a floats stream.  In dynamic mode the separation is based 
on user defined curves relating the material floated to the concentration of reagent.  
In mass balancing mode the user has the option to base the recovery on size fractions, 
or on the percentage recovery and grade of a specified specie in the flotation stream.
#n1.	Recovery based on size distribution - This option may only be chosen if the feed 
stream to the flotation cell actually has a size distribution.  In this case, the 
recovery of each solid in each size interval should be specified.  The flotation mechanism 
is simulated by ensuring that coarse or fine fractions are not floated.  The user specifies 
the specie of interest in the stream and the model will then calculate the overall 
recovery and grade of that specie in the float stream.
#n2.	The user specifies the required recovery and grade of the specie to be floated.  
The user may also specify a second specie to be floated and the grade of that specie.  
The unit will attempt to provide the required grade, but if there is insufficient material, 
the secondary grade will not be achieved.
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed           Input          1              20#n
Float          Output         1              20#n
Tails          Output         1              20#n#a
#n#h<Variables to be supplied by the user>#n
#n#i<Datum :>The relative elevation of the tank with respect to the common
datum of the system.
#n#i<FlotWater :>The amount of liquid reporting to the floation stream, expressed as 
a mass percentage of the stream.  If the user inputs a value of 20%, this means that 
the floation stream will consist of 80% solids and 20% liquids.  The liquids have the 
same make-up as the liquid feed to the flotation cell.
#n#i<Grade :>The grade of the floation stream.  If the user specifies 80%, then the 
floation stream will consist of 80% desired material and the remaining 20% will be made 
up of the other solids in the cell.
#n#i<FineEntrain? :>The user may choose to allow mainly the fine gangue to be entrained 
with the float stream.
#n#i<Flot? :>The specie which will be preferentially floated in the cell.  By clicking 
in this window the user will see a list of the available species.
#n#i<Coll? :>The specie which is defined as the collector.  This variable is required 
for dynamic mode, where the flotation is a function of the reagent concentration.
#n#i<Method :>The user may specify an overall recovery or recovery as a function of 
size fraction.
#n#i<2ndFloat? :>The user may choose to float a second specie.  If 'Yes' is chosen the 
following two variables become visible.
#n#i<2ndFlot :>The user may choose the second specie from the list of species in the project.
#n#i<2ndGrade :>The required grade of the second specie.
#n#i<Recovery :>This variable is visible only if the user specifies 'Recovery' as the 
method of recovery.  The user inputs the overall recovery of the desired specie as a 
percentage.
#n#i<SizeRec :>These pages are visible if the user selects 'SizeRecovery' as the method of 
recovery.  The user is required to input the desired recovery in each size fraction.  Before 
using this method be sure that the feed stream to the flotation cell has a size distribution.  
There is no point in specifying a recovery based on size distribution if the feed does not 
have any size characteristics.  In this case, the model will be unable to calculate any 
values and the feed stream will report to the tailings stream, with no recovery to the float 
stream.
#n#i<Reactions :>The user may input reactions within the floation cell using a reaction file 
as documented under 'Reactions.'#n
#n#h<Variables associated with the Flotation Cell>
#n#i<Density :>The density of the material in the flotation cell.
#n#i<Pressure :>This is equivalent to the atmospheric pressure.
#n#i<Level :>A ratio, expressed as a percentage, of the total volume of 
the solids and liquids, in the flotation cell, to the volumetric capacity of the cell. 
If the level exceeds 100%, the cell overflows and the material is lost to
the system..
#n#i<Mass_Accum :>The rate at which the mass of the material accumulates in
the flotation cell.
#n#i<Vol_Accum :>The rate at which the volume of the material accumulates in
the flotation cell.#n
#n#h<Other>#n
Default model prefix:FLT#n
Short name:FlotTank#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

SPECIEBLK_S(KCl,"KCl",  false)
SPECIEBLK_S(NaCl,"NaCl",  false)
SPECIEBLK_S(InSols,"InSols",  false)

const byte idFeed   = 0;
const byte idFlot   = 1;
const byte idTails  = 2;

static IOAreaRec FlotIOAreaList[] =
  {{"Cell Feed",   "Feed"      , idFeed   , LIO_In0 ,   nc_MLnk, 1, 20, 0, (float)1.0},
   {"Floats",      "Float"     , idFlot   , LIO_Out0,   nc_MLnk, 1, 20, 0, (float)0.8},
   {"Tails",       "Tails"     , idTails  , LIO_Out1,   nc_MLnk, 1, 20, 0, (float)0.0},
   {NULL}}; //This lists the areas of the model where links can be attached.


long Drw_Flot[] = { DD_Poly, -20,0, 20,-8, 20,4, -20,4, -20,0, 20,0,
                      DD_End }; //This provides the default drawing of the Flotation tank.
//#E
//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Flot, "FlotTank", "", Drw_Flot, "FLT", TOC_ALL|TOC_GRP_SIZEDST|TOC_USER,
                    "Flotation Tank",   
                    "Flotation Tank") 

Flot::Flot(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_Surge(pClass_, TagIn, pAttach, eAttach),
  QFd("QFd", this, TOA_Embedded),
  FlotFn ("DataOnly", "Flot_Collect",       this, TOA_Embedded),
  SizeRec("Flot_Size_Recovery", this, TOA_Embedded)
  {
  AttachIOAreas(FlotIOAreaList);
  Contents.SetClosed(False);
  SS_Lvl  = 0.25;
  Rec     = 0.1;
  Eff     = 0.8;
  Grd     = 0.5;
	Grade   = 0.7;
	Grade2  = 0.1;
	TotRecover = 0.9;
  iFlot   = 0;
  iSec    = 0;
  iColl   = 0;
	SecFlot = False;
	RM      = True;
	Fine    = False;

  SizeRec.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
  SizeRec.SetLength(5);
  SizeRec.SetModeVisibility(false, "", "");
  SizeRec.SetStateVisibility(false, "On", "Off");
  SizeRec.SetCumulativeVisibility(false);
  for (int c=0; c<SizeRec.NCurves(); c++)
    SizeRec.SetEditable(c, true);
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model and calls the
BuildDataDefn for objects Tled by the model.*/
void Flot::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility();
  GSM.Add_OnOff(DDB);

  DDB.Visibility(DYNMODE);
	DDB.Text    ("");
  //DDB.Double  ("Datum_Rqd",      "Z_Rqd",         DC_L    , "m",     &Z_Rqd,      this, isParm|NAN_OK);
  //DDB.Double  ("Datum",          "Z",             DC_L,    "m",      &Z,          this, 0);
  DDB.Double  ("Pressure",       "P",             DC_P,    "kPag",   xidPMean,    this, noFile|noSnap);
  DDB.Double  ("Temperature",    "T",             DC_T,    "C",      xidTemp,     this, noFile|noSnap);
  DDB.Double  ("Density",        "Rho",           DC_Rho,  "kg/m^3", xidRho,      this, noFile|noSnap);
  DDB.Double  ("Level",          "Lvl",           DC_Frac, "%",      xidLevel,    this, noFile|noSnap);
  DDB.Double  ("Mass_Accum",     "QmAcc",         DC_Qm,  "kg/s",    &QmAcc,      this, noFile|noSnap);
  DDB.Double  ("Vol_Accum",      "QvAcc",         DC_Qv,  "L/s",     &QvAcc,      this, noFile|noSnap);
  DDB.Visibility();

  DDB.Text    ("");
  static DDBValueLst DDB1[]={
    {(int)True,  "SizeRecovery"},
    {(int)False, "NoSizeRecovery" },
    {0}};
  DDB.Bool    ("RecoveryMethod", "Method",        DC_,     "",       &RM,         this, isParm, DDB1);
	DDB.Long    ("Float",          "Flot?",         DC_,     "",       &iFlot,      this, isParm, SDB.DDBSolSpList());
  if (!RM)
		{
    DDB.Double("Recovery",       "Recovery",      DC_Frac, "%",      &Eff,        this, isParm);
		DDB.Double("Grade",          "Grade",         DC_Frac, "%",      &Grd,        this, isParm);

		DDB.Text    ("");
		static DDBValueLst DDB0[]={
			{(int)True,  "Yes"},
			{(int)False, "No" },
			{0}};
		DDB.Bool  ("SecondFloat?",   "SecFloat?",     DC_,     "",       &SecFlot,    this, isParm, DDB0);
		if (SecFlot)
			{
			DDB.Long  ("SecondFloat",  "SecFlot",       DC_,     "",       &iSec,       this, isParm, SDB.DDBSolSpList());
			DDB.Double("SecondGrade",  "SecGrade",      DC_Frac, "%",      &Grade2,     this, isParm);
			}
		}
  if (RM)
		{
    DDB.Double("TotalRec",       "TotalRec",      DC_Frac, "%",      &TotRecover, this, 0);
		DDB.Double("Grade",          "Grade",         DC_Frac, "%",      &Grade,      this, 0);
		}
  DDB.Text("");

  
  DDB.Text    ("");
  //DDB.Double  ("SS_Lvl",         "",              DC_Frac, "%",      &SS_Lvl,     this, isParm);
  DDB.Double  ("WaterInConc",    "FlotWater",     DC_Frac, "%",      &Rec,        this, isParm);


  DDB.Visibility(DYNMODE);
  DDB.Byte    ("Collect",        "Coll?",         DC_,     "",       &iColl,      this, isParm, SDB.DDBSpList());
  DDB.Visibility();
  RB.Add_OnOff(DDB);
  BuildDataDefnShowIOs(DDB);

  if (RM)
    DDB.Object(&SizeRec, this, NULL, NULL, DDB_RqdPage);

  RB.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);
  
  GSM.BuildDataDefn(DDB);

  if (SolveMode()==DYNMODE)
    {
    DDB.Object(&Contents, this, NULL, NULL, DDB_RqdPage);
    DDB.Object(&PresetImg, this, NULL, NULL, DDB_RqdPage);
    }

  DDB.Visibility(DYNMODE);
  DDB.Object(&FlotFn, this, NULL, NULL, DDB_RqdPage);
  DDB.Visibility();

  DDB.EndStruct();
 
  };

//--------------------------------------------------------------------------

flag Flot::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidClosed:
      if (DCB.rB)
        Contents.SetClosed(*DCB.rB); 
      DCB.B=Contents.Closed(); 
      return 1;
    }

  if (MN_Surge::DataXchg(DCB)) 
    return 1;

  return 0;
  }

//---------------------------------------------------------------------------

flag Flot::ValidateData(ValidateDataBlk & VDB)
  {
  //Contents.ValidateData(ValidateDataBlk & VDB);

  return MN_Surge::ValidateData(VDB);
  }

void Flot::StartSolution()
  {
  MN_Surge::StartSolution();
  }

//--------------------------------------------------------------------------

void Flot::EvalJoinPressures()
  {
  switch (SolveMode())
    {
    case PBMODE:
    case SSMODE:
      {
      for (int j=0; j<NJoins(); j++)
        {
        double Pm=AtmosPress(IODatum_Term(j));//(j==0 ? PRqd : GetPBInputPressure(j));
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
      MN_Surge::EvalJoinPressures();
      break;
    }
  };

//--------------------------------------------------------------------------

void Flot::EvalJoinFlows(int JoinNo)
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

void Flot::EvalSteadyState()
  {
  };

//--------------------------------------------------------------------------

void Flot::EvalProducts()
  {
  double Tc;
  SigmaQInPMin(QFd, som_ALL, Id_2_Mask(idFeed));
  int iFl = IOWithId_Self(idFlot);
  int iTl = IOWithId_Self(idTails);
  rSpConduit Fl =*IOConduit(iFl);
  rSpConduit Tl =*IOConduit(iTl);

  SpMArray Mix, flot, sink;
  CDArray  TTemp, Feed, FTemp, YTemp, ATemp, 
			     FlotRec, MassSizeInt, entrain, solids;
  Eff = Range(0.01, Eff, 1.0);

  switch (SolveMode())
    {
    case PBMODE:
    case SSMODE:
      {
      if (GSM.Enabled())
        {
        if (NJoins()>=1)
          Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, RB(), GSM(), NULL);
        return;
        }
      double Solids, FMass, Entrain, gangue;
      Solids = QFd.QMass(som_Sol);
      
      RB.EvalProducts(QFd);
      Tc  = QFd.Temp();
      Mix = *QFd.Model();
      
			if (Solids < 1e-6)
				{
        Tl.QCopy(QFd);
        Fl.QZero();
        return;
        }
      
      /* Move the required % of the specie to be floated to the flotation stream, 
	    together with the required recovery of aqueous components.
      Calculate the amount of solids to report to the flotation stream based on 
      the user specified grade.  If recovery has been specified as a function of
      size, then this has to be carried out for each size fraction. */
      flot = Mix;
      sink = Mix;
      if (!RM)  // Recovery is NOT a function of size.
        {
				Grd    = Range(0.01, Grd, 1.0);
				Grade2 = Range(0.001, Grade2, 1.0 - Grd);

				// Calculate mass of floated specie and mass of gangue in floats
        FMass   = Mix[(int)iFlot] * Eff;
        flot[(int)iFlot] = FMass;
		    sink[(int)iFlot] = Mix[(int)iFlot] - flot[(int)iFlot];
        Entrain = FMass * (1.0/Grd - 1.0);
        Solids  = Solids - Mix[(int)iFlot];
        gangue  = Entrain / GTZ(Solids);

        
        /* If a second specie is also floated, calculate the amount of this specie
				   reporting to the floats */ 
				if (SecFlot)
					{
					double FlotMass, SecMass;
					FlotMass = FMass / Grd;
					SecMass  = FlotMass * Grade2;
					if (Mix[(int)iSec] < SecMass)
						{
						SecMass = Mix[(int)iSec];
						LogNote(Tag(), 0, "Insufficient Secondary specie in Flotation");
						}
					flot[(int)iSec] = SecMass;
					sink[(int)iSec] = Mix[(int)iSec] - flot[(int)iSec];

					Entrain = Entrain - SecMass;
					Solids  = Solids - Mix[(int)iSec];
					gangue  = Entrain / GTZ(Solids);
					}
				

				for (int i=0; i<SDB.No(); i++)
          {
					if (!SecFlot)
						iSec = iFlot;
					if ((i != (int)iFlot) && (i != (int)iSec) )
		        {
            if (SDB[i].IsLiq())  // Reqd liquid split to floats
		          {
              flot[i] = Mix[i] * Rec;
		          sink[i] = Mix[i] - flot[i];
		          }
            if (SDB[i].IsSol())
		          {
							double masscheck = Mix[i] * gangue;
							if (masscheck > Mix[i])
							  {
							  flot[i] = Mix[i];
                sink[i] = 0.0;
								}
              else
							  {
                flot[i] = Mix[i] * gangue;
		            sink[i] = Mix[i] - flot[i];
								}
		          }
		        if (SDB[i].IsVap()) // Allow all gases to escape
			        {
              flot[i] = 0.0;
		          sink[i] = 0.0;
		          }
		        }
	        }
          
          SetProdMakeup(PMU_IOId |PMU_Image, idFlot,  flot, Tc, Std_P, QFd.Model());
          SetProdMakeup(PMU_IOId |PMU_Image, idTails, sink, Tc, Std_P, QFd.Model());
        }

      if (RM)   // Recovery IS a function of size
				{
        SigmaQInPMin(QFd, som_ALL, Id_2_Mask(idFeed));
				double TonsTotal = QFd.QMass(som_Sol);
        const double TonsLimit = 1e-6;

        SQSzDist1 &Sz   =*SQSzDist1::Ptr(QFd.Model());
        SQSzDist1 &SzFl =*SQSzDist1::Ptr(Fl.Model());
        SQSzDist1 &SzTl =*SQSzDist1::Ptr(Tl.Model());
        
				// Transfer All Qualities / Solids to Tails and Liquids 50/50
				Fl.QSetF(QFd, SetMass_Frac, 0.0, 0.5, 0.0, Std_P);
				Tl.QSetF(QFd, SetMass_Frac, 1.0, 0.5, 0.0, Std_P);

				double FlSol = 0.0;
				double TlSol = 0.0;
				flag NoSize  = True;
				for (int d=0; d<Sz.NDistributions(); d++)
          if (Sz.DistExists(d))
            {
            SzFl.AllocDist(d);
            SzTl.AllocDist(d);
            CSD_Distribution &D  = Sz.Dist(d);
            CSD_Distribution &DT = SzTl.Dist(d);
            CSD_Distribution &DF = SzFl.Dist(d);

    				// Find the total mass in each size interval
						//=============================================

						CDArray & Size = D.PriSp[0]->FracPass;
						int len = Size.GetSize();
						if (len>0)
							{
							MassSizeInt.SetSize(len);
				  	  for(int i=0 ; i<len ; i++)
							  MassSizeInt[i] = 0.0;
							}

            for (int s=0; s<D.NPriIds(); s++)
							{
							CDArray & Size = D.PriSp[s]->FracPass;
              
							int len=Size.GetSize();
							if (len>0)
								{
								CDArray SInt;
								SInt.SetSize(len);
								double Fractions, x1, y1, tmpF;
								Fractions = 0.0;
								for(long i=0 ; i<len ; i++)
									Fractions += Size[i];
								if (Fractions <= 0.0)
									Fractions = 1.0;

								for(i=0 ; i<len ; i++)
									SInt[i] = Size[i] / Fractions;

								double TonsS = 0.0;
								for (int s1=0; s1<D.NSecIds(s); s1++)
										TonsS+= QFd.Qm(D.SzId(s,s1));
								for(i=0 ; i<len ; i++)
									{
									x1      = D.Intervals()[i];
									y1      = SInt[i];
      						tmpF    = y1 * TonsS;
									MassSizeInt[i] += tmpF;
									}
								}
							}
								//============================================

						/* First calculate the mass of each specie floated in each size interval.
						   Note: This code is hard wired for Cleveland.*/
						    
            for (s=0; s<D.NPriIds(); s++)
							{
								CDArray & Size = D.PriSp[s]->FracPass;
								CDArray & ASizeRec = SizeRec.Curve(s);
								flag  flot = False;

              
								int len = Size.GetSize();
								if (len>0)
									{
									NoSize = False;
									CDArray & Float   = DF.PriSp[s]->FracPass;
									CDArray & Tailing = DT.PriSp[s]->FracPass;
									TTemp.SetSize(len);
									Feed.SetSize(len);
									FTemp.SetSize(len);
									YTemp.SetSize(len);
                  FlotRec.SetSize(len);
                  entrain.SetSize(len);
									solids.SetSize(len);

									double Fractions, x1, y1, tmpF, tmpO;
									Fractions = 0.0;
									for(long i=0 ; i<len ; i++)
										Fractions += Size[i];
									if (Fractions <= 0.0)
										Fractions = 1.0;

									for(i=0 ; i<len ; i++)
										YTemp[i] = Size[i] / Fractions;

									double TonsS = 0.0;
									for (int s1=0; s1<D.NSecIds(s); s1++)
											TonsS+= QFd.Qm(D.SzId(s,s1));
										for(i=0 ; i<len ; i++)
											{
											x1      = D.Intervals()[i];
											y1      = YTemp[i];
      								tmpF    = y1 * TonsS;
											Feed[i] = tmpF;
											}
            
										double SpcTlSol = 0.0;
										double SpcFlSol = 0.0;
										for (i=0; i<len; i++)
        							{
											Float[i] = Range(0.0, ASizeRec[i], 1.0);
											tmpO = Float[i];
											tmpF = Feed[i];
											FTemp[i]  = Float[i] * Feed[i];
											SpcFlSol += FTemp[i];
											TTemp[i]  = Feed[i] - FTemp[i];
											SpcTlSol += TTemp[i];
											}

										if (SpcTlSol < TonsLimit)
											{
											SpcFlSol   = GTZ(SpcFlSol);
											Tailing[0] = 0.0;
											Float[0]   = FTemp[0] / SpcFlSol;
											for(i=1 ; i<len ; i++)
      									{
												Tailing[i] = 0.0;
												Float[i]   = (FTemp[i] / SpcFlSol);// + Float[i-1];
												}
											}
										if (SpcFlSol < TonsLimit)
											{
											SpcTlSol   = GTZ(SpcTlSol);
											Tailing[0] = TTemp[0] / SpcTlSol;
											Float[0]   = 0.0;
											for(i=1 ; i<len ; i++)
      									{
												Float[i]   = 0.0;
												Tailing[i] = (TTemp[i] / SpcTlSol);// + Tailing[i-1];
												}
											}
										if ((SpcTlSol > TonsLimit) && (SpcFlSol > TonsLimit))
											{
											for(i=0 ; i<len ; i++)
      									{
												Tailing[i] = (TTemp[i] / SpcTlSol);
												Float[i]   = (FTemp[i] / SpcFlSol);
												}
											}

										double TotSol    = GTZ(SpcTlSol + SpcFlSol);
										double TlSolFrac = SpcTlSol/TotSol;
										double FlSolFrac = SpcFlSol/TotSol;

										int Id;
										if (s == 0)
											Id = KCl.si();
										if (s == 1)
											Id = NaCl.si();
										if (s == 2)
											Id = InSols.si();
 
 											Tl.SetQm(Id, QFd.Qm(Id) * TlSolFrac);
											Fl.SetQm(Id, QFd.Qm(Id) * FlSolFrac);

										TlSol += SpcTlSol;
										FlSol += SpcFlSol;
									}
              }
				//=========================================================
				// Calculate the grade and recovery of the desired specie.

				double MassFloat, MassIn, MassSpecie;
				MassFloat  = Max(1e-6, Fl.QMass(som_Sol));
				MassIn     = Max(1e-6, QFd.Qm((int)iFlot));
				MassSpecie = Fl.Qm((int)iFlot);
				
				TotRecover = MassSpecie/MassIn;
				Grade      = MassSpecie/MassFloat; 

				//---------------------------------------------------------
				// Now calculate the amount of liquids reporting to the float stream
											       
        double FlLiq, TlLiq;
        double QmSIn = QFd.QMass(som_Sol);
        double QmLIn = QFd.QMass(som_Liq);
        if (QmLIn > 1.0e-6)
          {

          if (Rec>1.0e-12)
            {
            Rec = Range(0.0, Rec, 0.999);
            FlLiq = FlSol / (1.0 - Rec) - FlSol;
            FlLiq = Range(0.0, FlLiq, QmLIn);
            }
          else
            FlLiq = 0.0;
          TlLiq = QmLIn - FlLiq;

          // Convert Liquids to % of feed
          FlLiq = FlLiq / QmLIn;
          TlLiq = TlLiq / QmLIn;
          }
        else
          {
          FlLiq = 0.0;
          TlLiq = 0.0;
          }
        // Convert Solids to % of feed
        TlSol = TlSol / TonsTotal;  
        FlSol = FlSol / TonsTotal;

        for (s=0; s<SDB.No(); s++)
          if (SDB[s].IsLiq())
            {
            Fl.SetQm(s, QFd.Qm(s) * FlLiq);
            Tl.SetQm(s, QFd.Qm(s) * TlLiq);
            }

        Fl.SetTemp(QFd.Temp());
        Tl.SetTemp(QFd.Temp());

        if (NoSize)
          {
          LogNote(Tag(), 0, "No Size distr. in Flotation feed");
          Tl.QCopy(QFd);
          Fl.QZero();
          }
        }
       }
      break;
      } 
    case DYNMODE:
      {
      Contents.ZeroDeriv();
      RB.EvalProducts(QFd);
      Tc  = Contents.Temp();
      Mix = (*Contents.Model());

      double MolConc, Liquid;
      Liquid  = Contents.Mass(som_Liq);
      if (Liquid >= 1.0)
        MolConc = Contents.SpMass(iColl)/SDB[iColl].MoleWt() / Liquid;
      else
        MolConc = 0.0;
      /* Determine flotation efficiency as a function of collector
         molar concentration. */
      Eff = Range(0.0, FlotFn.Yx(MolConc), 1.0);

      /*Move the required % of the specie to be floated to the flotation stream, 
	    together with the collector.*/
      flot = Mix;
      sink = Mix;
      for (int i=0; i<SDB.No(); i++)
        {
        if (i == (int)iFlot || i == iColl)
		      {
          flot[i] = Mix[i] * Eff;
		      sink[i] = Mix[i] - flot[i];
		      } 
        else   
  	      if (SDB[i].IsSol())  // Solids report to tails
		        {
            flot[i] = 0.0;
		        sink[i] = Mix[i];
		        }
		      else  // Allow all gases to escape
			      {
            flot[i] = 0.0;
		        sink[i] = 0.0;
		        }
		    } 

      for (i=0; i<NoFlwIOs(); i++)
        if (IO_Out(i))
          switch (IOId_Self(i))
            {
            case idFlot : SetProdMakeup(PMU_IOId |PMU_Image, i, flot, Tc, Std_P, QFd.Model()); break;
            case idTails: SetProdMakeup(PMU_IOId |PMU_Image, i, sink, Tc, Std_P, QFd.Model()); break;
            default:      SetProdMakeup(PMU_IOId |PMU_Image, i, Mix,  Tc, Std_P, QFd.Model()); break;
            }  
	    break;
      }
	  }
  };    

//==========================================================================
