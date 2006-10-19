//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EXAMPLESCREEN_CPP
#include "examplescreen.h"
#include "math.h"

//====================================================================================

const int idFeed      = 0;
const int idOversize  = 1;
const int idUndersize = 2;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "Oversize",       "Oversize",  idOversize,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { "Undersize",      "Undersize", idUndersize, 1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { NULL },
  };

static double Drw_WhitenScreen[] = { MDrw_Poly,  -4.,3.,  5.,-1.,  1.,-3.,  -4.,3.,
                            MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(WhitenScreen, "WhitenScreen", DLL_GroupName)
void WhitenScreen_UnitDef::GetOptions()
  {
  SetDefaultTag("SC", true);
  SetDrawing("SizeSeparation", Drw_WhitenScreen);
  SetTreeDescription("Demo:Screen");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD);
  };

//---------------------------------------------------------------------------

WhitenScreen::WhitenScreen(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  dD50 = 0.001; //m
  dAlpha = 4.0;
  dFeedQm = 0.0;
  dProdQm0 = 0.0;
  dProdQm1 = 0.0;
  }

//---------------------------------------------------------------------------

void WhitenScreen::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void WhitenScreen::BuildDataFields()
  {
  DD.Text("Requirements...");
  DD.Double("d50", "", &dD50, MF_PARAMETER, MC_L);
  DD.Double("Alpha", "", &dAlpha, MF_PARAMETER, MC_None);
  DD.Double("Beta", "", &dBeta, MF_PARAMETER, MC_None);
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedQm", "", &dFeedQm, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("UnderflowQm", "", &dProdQm0, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("OverflowQm", "", &dProdQm1, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Page("PartCurve");
  CString Tg;
  for (int i=0; i<Eu.GetSize(); i++)
    {
    Tg.Format("Part%02d", i);
    DD.Double((char*)(const char*)Tg, "", &Eu[i], MF_RESULT|MF_NO_FILING, MC_Frac);
    }
  }

//---------------------------------------------------------------------------

void WhitenScreen::EvalProducts()
  {
  try
    {
    //
    // Make a copy of our input stream so we do not corrupt it
    //
    MStream QI;
    FlwIOs.AddMixtureIn_Id(QI, idFeed);

    //Alternatively, IF there is only one feed allowed, we could get a direct reference to the Stream
    //MStream & QI = FlwIOs[FlwIOs.First[idFeed]].Stream;

    //
    // Get References to our Output Streams using the FlwIOs helper class and IO ids
    //
    MStream & Qof = FlwIOs[FlwIOs.First[idOversize]].Stream;
    MStream & Quf = FlwIOs[FlwIOs.First[idUndersize]].Stream;

    //
    // Summary Mass Flow In
    //
    dFeedQm = QI.MassFlow();

    //
    // Sanity Check d50
    //
    if (dD50<1.0e-12)
      dD50 = 1.0e-12;

    //
    // Always initialise the outputs as a copy of the inputs.
    // This ensures all "qualities" are copied and initialised (eg PSD, SpecieModel, etc)
    //
    Qof = QI;
    Quf = QI;

    //
    // Determine if the input stream has size information present
    // NB: This will be successful if any of the species have PSD data
    // associated with them. We still need to manage those species that
    // do not have PSD data further down.
    //
    MIPSD & PSD = *QI.FindIF<MIPSD>();
    MIPSD & PSDof = *Qof.FindIF<MIPSD>();
    MIPSD & PSDuf = *Quf.FindIF<MIPSD>();

    if ( IsNothing(PSD)==false )
      {
      // Input Stream does have a PSD
      // which implies that some of the input stream species have associated size data

      long l_SizeCount = PSD.getSizeCount();
      long l_PSDVectorCount = PSD.getPSDVectorCount();

      //
      //  Build probability passing from specified d50, Alpha parameters and PSD size
      //  interval data
      //
      Eu.SetSize(l_SizeCount);
      for( int lSzIndex=1; lSzIndex < l_SizeCount; lSzIndex++ )
        {
        //const double LowSz = PSD.getSize(lSzIndex);
        //const double NominalSize = sqrt(LowSz * PSD.getSize(lSzIndex-1)); //use geometrical mean
        const double NominalSize = PSD.getDefn().getGeometricMean(lSzIndex); //use geometrical mean
        const double d = dAlpha*NominalSize/dD50;
        if (d>230.0)//limit of exp()
          Eu[lSzIndex] = 1.0;
        else
          {
          Eu[lSzIndex] = (exp(d)-1.0) / (exp(d) + exp(dAlpha) - 2.0);
          if (Eu[lSzIndex]>1.0)
            Eu[lSzIndex] = 1.0;
          }
        }
      Eu[0] = 1.0;

      //
      // Calculate the Split Between Undersize and Oversize for each species
      // that has size data.
      //
      for( int lPSDVec=0; lPSDVec < l_PSDVectorCount; lPSDVec++ )
        {
        // Example of getting the species Index in the stream related to this PSD
        // long lSpeciesIndex = getSpecieIndex(lPSDVec);

        // Create some memory for the input and output PSD mass data
        double* M = new double[l_SizeCount];
        double* M_os = new double[l_SizeCount];
        double* M_us = new double[l_SizeCount];

        // Populate the input mass vector
        // The underlying implementation only stores
        // fractions. Hence we cannot get access to the individual
        // masses for each size interval directly.
        // This method works out the actual mass
        // for each size interval and populates the specified vector
        PSD.ExtractMassVector(M, lPSDVec);

        //
        // Calculate the split
        //
        double l_OF = 0.0;
        double l_US = 0.0;

        for( int lSzIndex=0; lSzIndex < l_SizeCount; lSzIndex++ )
          {
          // Get the calculated split for the Size
          const double Split = Eu[lSzIndex];

          // The overflow
          l_OF = M[lSzIndex]*Split;
          M_os[lSzIndex] = l_OF;

          // The underflow
          l_US = M[lSzIndex] - l_OF;
          M_us[lSzIndex] = l_US;
          }

        //
        // Set the output Masses
        // The underlying implementation only stores the total mass
        // and the PSD fractions. This method re-calculates the total
        // mass and underlying PSD fractions from the supplied mass vector
        //
        PSDof.ReplaceMassVector(M_os, lPSDVec );
        PSDuf.ReplaceMassVector(M_us, lPSDVec );

        //
        // Cleanup
        //
        delete [] M;
        delete [] M_os;
        delete [] M_us;
        }


      //
      // Handle species without size data (send all to Quf)...
      //
      for (int l_SpeciesIndex=0;  l_SpeciesIndex < QI.Count(); l_SpeciesIndex ++)
        {
        if (QI.HasSizeData(l_SpeciesIndex)==false)
          //if (gs_MVDefn[l_SpeciesIndex].HasSizeData()==false)
          {
          // Species has no Size Data
          Qof.putM(l_SpeciesIndex, 0.0);
          Quf.putM(l_SpeciesIndex, QI.getM(l_SpeciesIndex) );
          }
        }

      }
    else
      {
      Log.Message(MMsg_Warning, "No PSD Properties present in feed, report to overflow");
      Qof = QI;
      Quf.ZeroMass();
      }

    //
    // Summary Mass Flow Out
    //
    dProdQm0 = Qof.MassFlow();
    dProdQm1 = Quf.MassFlow();
    }


  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//====================================================================================
