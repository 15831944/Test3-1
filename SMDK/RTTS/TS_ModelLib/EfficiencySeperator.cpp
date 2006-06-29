//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EfficiencySeperator_CPP
#include "EfficiencySeperator.h"

//====================================================================================
//             Efficiency Curve
//====================================================================================

   CSeperator_EfficiencyCurve::CSeperator_EfficiencyCurve()
	{
	m_dTopAlpha				= 1.0;
	m_dTopBeta				= 0.0;
	m_dTopBetaStar			= 1.0;
	m_dTopD50				= 0.1;
	m_dTopC					= 1.0;
	m_dTopWaterSplitToUS	= 1.0;

	m_dBottomAlpha			= 1.0;
	m_dBottomBeta			= 0.0;
	m_dBottomBetaStar		= 1.0;
	m_dBottomD50			= 0.1;
	m_dBottomC				= 1.0;
	m_dBottomWaterSplitToUS	= 1.0;

	}

	void CSeperator_EfficiencyCurve::BuildDataFields(MDataDefn &DB)
	
	{ 
     DB.ObjectBegin("TS_EffCurve", "EffCurve");


	 DB.Text("Efficiency Curve");
	 //DB.StructBegin("TS_Top", "Top" );
	 DB.StructBegin("Top");
	 DB.Double("Alpha"			, "", &m_dTopAlpha	       , MF_PARAMETER ,  MC_None);
	 DB.Double("Beta"			, "", &m_dTopBeta	       , MF_PARAMETER ,  MC_None);
	 DB.Double("BetaStar"		, "", &m_dTopBetaStar	   , MF_RESULT ,  MC_None);
	 DB.Double("D50"			, "", &m_dTopD50	       , MF_PARAMETER ,  MC_L("mm"));
	 DB.Double("C"				, "", &m_dTopC			   , MF_PARAMETER ,  MC_None);
	 DB.Double("WaterSplitToUS"	, "", &m_dTopWaterSplitToUS, MF_PARAMETER ,  MC_Frac("%"));
	 DB.StructEnd();

	 //DB.StructBegin("TS_Bottom", "Bottom" );
 	 DB.StructBegin("Bottom" );
	 DB.Double("Alpha"			, "", &m_dBottomAlpha	       , MF_PARAMETER ,  MC_None);
	 DB.Double("Beta"			, "", &m_dBottomBeta	       , MF_PARAMETER ,  MC_None);
	 DB.Double("BetaStar"		, "", &m_dBottomBetaStar	   , MF_RESULT ,     MC_None);
	 DB.Double("D50"			    , "", &m_dBottomD50		   , MF_PARAMETER ,  MC_L("mm"));
	 DB.Double("C"				, "", &m_dBottomC			   , MF_PARAMETER ,  MC_None);
	 DB.Double("WaterSplitToUS"	, "", &m_dBottomWaterSplitToUS , MF_PARAMETER ,  MC_Frac("%"));
	 DB.StructEnd();

	//
	// Outputs
	//
	DB.Page("Top Deck Partition");
    CString Tg;
    for (int i=(m_TopEu.GetSize()-1); i>=0; i--)
    {
      Tg.Format("Top.I%02d", i);
      DB.Double((char*)(const char*)Tg, "", &m_TopEu[i], MF_RESULT, MC_Frac("%"));
    }
    DB.Page("Bottom Deck Partition");
    for (int i=(m_BottomEu.GetSize()-1); i>=0; i--)
    {
      Tg.Format("Bottom.I%02d", i);
      DB.Double((char*)(const char*)Tg, "", &m_BottomEu[i], MF_RESULT, MC_Frac("%"));
    }
	DB.ObjectEnd();

	}

//====================================================================================

	void CSeperator_EfficiencyCurve::EvalProducts(MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit)
	
	{ 
    // Get info on Size Distribution
    MIPSD & l_PSD=Feed.IF<MIPSD>(false);
    if (IsNothing(l_PSD))
      {
      int xx=0;
      return; //Why do we get here!!!
      }
    long    l_SizeCount      = l_PSD.getSizeCount();
    long    l_PSDVectorCount = l_PSD.getPSDVectorCount();


	//
	// Calculate Probability Passing for Each Deck
	// We could calculate this in Build Data definition
	//
    CalculateEfficiencyCurve( l_PSD , m_dTopD50 , m_dTopAlpha, m_dTopBeta, m_dTopC,
							  m_dTopBetaStar,
		                      m_TopEu );
    CalculateEfficiencyCurve( l_PSD , m_dBottomD50 , m_dBottomAlpha, m_dBottomBeta, m_dBottomC,
						      m_dBottomBetaStar,
		                      m_BottomEu );
 

       if ( bTwoDecks )
        {
            // Execute the first seperator
			//MStream l_IntermediateProduct = Product1; // Cannot do this as = operator does not make copy
			Seperate(m_TopEu,m_dTopWaterSplitToUS,
				     Feed,
		             Product1 ,
		             /*l_IntermediateProduct*/Product2);

            // Execute the second seperator
			Seperate(m_BottomEu,m_dBottomWaterSplitToUS,
				     /*l_IntermediateProduct*/Product2,
		             Product2 ,
		             Product3);
  
 
        }
        else
        {
            // Execute the first seperator
			Seperate(m_TopEu,m_dTopWaterSplitToUS,
				     Feed,
		             Product1 ,
		             Product3);

        }
	
	}

//====================================================================================

	void CSeperator_EfficiencyCurve::Seperate(CArray <double, double&> &in_Eu,
											  double  in_WaterSplitToUS,
											  MStream &in_Feed,
		                                      MStream &out_ProductA ,
		                                      MStream &out_ProductB)
	{

      // Get info on Size Distribution
      MIPSD & l_PSD=in_Feed.IF<MIPSD>();
      long    l_SizeCount      = l_PSD.getSizeCount();
      long    l_PSDVectorCount = l_PSD.getPSDVectorCount();

      // Get references to Product Stream PSDs
      MIPSD & PSDof = out_ProductA.IF<MIPSD>(false);
      MIPSD & PSDuf = out_ProductB.IF<MIPSD>(false);

      // Create some memory for the input and output PSD mass data
      double* M = new double[l_SizeCount];
      double* M_os = new double[l_SizeCount];
      double* M_us = new double[l_SizeCount];


      //
      // Calculate the Split Between Undersize and Oversize for each species
      // that has size data.
      //
      for( int lPSDVec=0; lPSDVec < l_PSDVectorCount; lPSDVec++ )
        {
  

        // Populate the input mass vector
        // The underlying implementation only stores
        // fractions. Hence we cannot get access to the individual
        // masses for each size interval directly.
        // This method works out the actual mass
        // for each size interval and populates the specified vector
        l_PSD.ExtractMassVector(M, lPSDVec);

        //
        // Calculate the split
        //
        double l_OF = 0.0;
        double l_US = 0.0;

        for( int lSzIndex=0; lSzIndex < l_SizeCount; lSzIndex++ )
          {
          // Get the calculated split for the Size
          const double Split = in_Eu[lSzIndex];

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

        }

       //
       // Cleanup
       //

       delete [] M;
       delete [] M_os;
       delete [] M_us;

	   //
	   // Liquid Split
	   //

	    for (int l_SpeciesIndex=0;  l_SpeciesIndex < in_Feed.Count(); l_SpeciesIndex ++)
        {
			if ( in_Feed.IsLiquid(l_SpeciesIndex) )
			{

				double Liquid = in_Feed.getM(l_SpeciesIndex);
			    double LiquidToUS = in_WaterSplitToUS*Liquid;
				double LiquidToOS = Liquid - LiquidToUS;

				out_ProductA.putM(l_SpeciesIndex, LiquidToOS);
				out_ProductB.putM(l_SpeciesIndex, LiquidToUS);

			}

        }

	}

//====================================================================================

double CSeperator_EfficiencyCurve::CalcBetaStar(double in_Alpha, double in_Beta )
  {
  double BetaStar = 1.0 - in_Beta; 
  double PN = (1.0 + in_Beta*BetaStar) * (Exps(in_Alpha) - 1.0) / (Exps(in_Alpha*BetaStar) + Exps(in_Alpha) - 2.0); //Partition Number
  int iter = 0;
  const int MaxIter = 100;
  const double Tol = 1.0e-4;
  while (iter++<MaxIter)
    {
    BetaStar += (BetaStar*(PN-0.5)/10.0); //update estimate of BetaStar
    PN = (1.0 + in_Beta*BetaStar) * (Exps(in_Alpha) - 1.0) / (Exps(in_Alpha*BetaStar) + Exps(in_Alpha) - 2.0); //Recalc Partition Number
    if (fabs(PN-0.5)<Tol)
      break;
    }
  //if (iter>=MaxIter)
  //  LogError(Tag(), 0, "Unable to calculate BetaStar!");
  return BetaStar;
  }

//====================================================================================

	void CSeperator_EfficiencyCurve::CalculateEfficiencyCurve(
		                      MIPSD &in_PSD , double in_D50 ,
							  double in_Alpha, double in_Beta, double in_C,
							  double &inout_BetaStar,
		                      CArray <double, double&> &out_Eu )
	{
     long    l_SizeCount      = in_PSD.getSizeCount();
     long    l_PSDVectorCount = in_PSD.getPSDVectorCount();

	  //
	  // Calculate BetaStar
	  //
	  inout_BetaStar = CalcBetaStar(in_Alpha, in_Beta );

      //
      //  Build probability passing from specified d50, Alpha parameters and PSD size
      //  interval data
      //
      out_Eu.SetSize(l_SizeCount);
      for( int lSzIndex=0; lSzIndex < l_SizeCount; lSzIndex++ )
        {
        const double NominalSize = in_PSD.getDefn().getGeometricMean(lSzIndex); //use geometrical mean
        const double d = in_Alpha*inout_BetaStar*NominalSize/in_D50;
        if (d>230.0)//limit of exp()
          out_Eu[lSzIndex] = 1.0;
        else
          {

          //out_Eu[lSzIndex] = (exp(d)-1.0) / (exp(d) + exp(in_Alpha) - 2.0);
		  //out_Eu[lSzIndex] = 1.0 - in_C*(exp(in_Alpha)-1.0) / (exp(d) + exp(in_Alpha) - 2.0);
		  double num = (1.0+in_Beta*inout_BetaStar*NominalSize/in_D50)*(exp(in_Alpha)-1.0 );
		  double den = exp(d) + exp(in_Alpha) - 2.0;
		  out_Eu[lSzIndex] = 1.0 - in_C*num/GTZ(den);
		
          if (out_Eu[lSzIndex]>1.0)
            out_Eu[lSzIndex] = 1.0;
		  if (out_Eu[lSzIndex]<0.0)
           out_Eu[lSzIndex] = 0.0;
          }
        }
      //out_Eu[0] = 0.0;

	}

//====================================================================================
