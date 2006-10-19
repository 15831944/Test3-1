//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __GlobalPartitionSeperator_CPP
#include "GlobalPartitionSeperator.h"
#pragma optimize("", off)

//====================================================================================
//             Global Partition
//====================================================================================

CSeperator_GlobalPartition::CSeperator_GlobalPartition()
{

	m_lPartitionSel = 0;
	m_DDPSDPartition = 0;

	// Initialise discharge partitions for all possible
	// PSD Definitions that could be used for this project
	int n = gs_PSDDefns.Count();
	m_CTopPartition.SetSize(n);
	m_CBottomPartition.SetSize(n);
	m_DDPSDPartition = new MDDValueLst[n+1];
	m_DDPSDPartition[n].m_dwFlags = 0;
	m_DDPSDPartition[n].m_lVal = 0;
	m_DDPSDPartition[n].m_pStr = 0;

	m_dTopWaterSplitToUS = 1.0;
	m_dBottomWaterSplitToUS = 1.0;

	for (int i=0; i< n; i++)
	{
		MPSDDefn& PSDDefn = gs_PSDDefns[i]; 
		int NIntervals = PSDDefn.getSizeCount();
		
		m_CTopPartition[i].m_EU.SetSize(NIntervals);
		//m_CTopPartition[i].m_dWaterSplitToUS = 1;
		m_CTopPartition[i].m_Name = PSDDefn.Name();

		m_CBottomPartition[i].m_EU.SetSize(NIntervals);
		//m_CBottomPartition[i].m_dWaterSplitToUS = 1;
		m_CBottomPartition[i].m_Name = PSDDefn.Name();
	
		m_DDPSDPartition[i].m_dwFlags = 0;
		m_DDPSDPartition[i].m_lVal = i;
		const LPCSTR s = m_CTopPartition[i].m_Name.GetString();
		m_DDPSDPartition[i].m_pStr = (LPTSTR)s;
		
		
	}
}

//====================================================================================

void CSeperator_GlobalPartition::BuildDataFields(MDataDefn &DB)
	
{ 

    DB.ObjectBegin("TS_GlobalSep", "GlobalPartition");
	DB.Page("Top Deck Partition");
    CString Tg1,Tg2;
	DB.Long  ("Partition", "", &m_lPartitionSel  , MF_PARAMETER, m_DDPSDPartition );
	DB.Double("TopWaterSplitToUS"	, "TopWaterSplit", &m_dTopWaterSplitToUS, MF_PARAMETER ,  MC_Frac("%"));

	for (int i = 0; i < m_CTopPartition.GetSize() ; i++)
	{
		DWORD flags;
		if ( i == m_lPartitionSel )
			flags = MF_PARAMETER;
		else
			flags = MF_PARAMETER | MF_NO_VIEW;

		//
		// Top Deck
		//
		Tg1.Format("Top%02d",i);
		//DB.StructBegin("TS_Top", Tg1 );
		DB.StructBegin(Tg1 );

		Tg1.Format("TS_TopGlobalSep%02d",i);
		Tg2.Format("GlobalSep%02d",i);
		DB.ObjectBegin(Tg1, /*Tg2*/"GlobalSep");
		//DB.Double("WaterSplitToUS"	, "", &m_CTopPartition[m_lPartitionSel].m_dWaterSplitToUS, flags ,  MC_Frac("%"));
		for (int j=(m_CTopPartition[m_lPartitionSel].m_EU.GetSize()-1); j>=0; j--)
		{
			Tg1.Format("I%02d", j);
			DB.Double((char*)(const char*)Tg1, "", &m_CTopPartition[m_lPartitionSel].m_EU[j], flags , MC_Frac("%"));
		}
		DB.ObjectEnd();

		DB.StructEnd();


	}
 
	DB.Page("Bottom Deck Partition");
	DB.Double("BottomWaterSplitToUS"	, "BottomWaterSplit", &m_dBottomWaterSplitToUS, MF_PARAMETER ,  MC_Frac("%"));

	for (int i = 0; i < m_CBottomPartition.GetSize() ; i++)
	{
		DWORD flags;
		if ( i == m_lPartitionSel )
			flags = MF_PARAMETER;
		else
			flags = MF_PARAMETER | MF_NO_VIEW;

		//
		// Bottom Deck
		//
		Tg1.Format("Bottom%02d",i);
		//DB.StructBegin("TS_Bottom", Tg1 );
		DB.StructBegin( Tg1 );

		Tg1.Format("TS_BottomGlobalSep%02d",i);
		Tg2.Format("GlobalSep%02d",i);
		DB.ObjectBegin(Tg1, /*Tg2*/"GlobalSep");

		//DB.Double("WaterSplitToUS"	, "", &m_CBottomPartition[m_lPartitionSel].m_dWaterSplitToUS, flags ,  MC_Frac("%"));
		for (int j=(m_CBottomPartition[m_lPartitionSel].m_EU.GetSize()-1); j>=0; j--)
		{
			Tg1.Format("I%02d", j);
			DB.Double((char*)(const char*)Tg1, "", &m_CBottomPartition[m_lPartitionSel].m_EU[j], flags , MC_Frac("%"));
		}

		DB.ObjectEnd();

		DB.StructEnd();


	}

	DB.ObjectEnd();

}

//====================================================================================

void CSeperator_GlobalPartition::EvalProducts(MStream &Feed ,
	                         MStream &Product1,
							 MStream &Product2,
							 MStream &Product3,
							 bool    bTwoDecks,
							 bool    bInit)
{ 

    // Get info on Size Distribution
    MIPSD & l_PSD=*Feed.GetIF<MIPSD>();
    long    l_SizeCount      = l_PSD.getSizeCount();
    long    l_PSDVectorCount = l_PSD.getPSDVectorCount();


	if ( IsNothing(l_PSD)==false )
	{

		// Determine which PSD we are using so we can
		// choose the correct discharge partition
		//
		// This is a temporary solution until we expose
		// PSDin.getDefnIndex();
		//
		int iPSD = 0;
		MPSDDefn& PSDDefn = l_PSD.getDefn();
		int ndefs = gs_PSDDefns.Count();
		for (int i=0; i< ndefs ; i++)
		{
			MPSDDefn& PSDDefSrch = gs_PSDDefns[i];

			if (&PSDDefSrch == &PSDDefn)
			{
				iPSD = i;
				break;
			}
		}

		if ( bTwoDecks )
			{
				// Execute the first seperator
				Seperate(m_CTopPartition[iPSD].m_EU,
					     m_dTopWaterSplitToUS,
						 Feed,
						 Product1 ,
						 Product2);

				// Execute the second seperator
				Seperate(m_CBottomPartition[iPSD].m_EU,
					     m_dBottomWaterSplitToUS,
						 Product2,
						 Product2 ,
						 Product3);
	  
	 
			}
			else
			{
				// Execute the first seperator
				Seperate(m_CTopPartition[iPSD].m_EU,
					     m_dTopWaterSplitToUS,
						 Feed,
						 Product1 ,
						 Product3);
			}
	}


}
//====================================================================================

void CSeperator_GlobalPartition::Seperate(CArray <double, double&> &in_Eu,
											 double  in_WaterSplitToUS,
											 MStream &in_Feed,
		                                     MStream &out_ProductA ,
		                                     MStream &out_ProductB)
	{

      // Get info on Size Distribution
      MIPSD & l_PSD=*in_Feed.GetIF<MIPSD>();
      long    l_SizeCount      = l_PSD.getSizeCount();
      long    l_PSDVectorCount = l_PSD.getPSDVectorCount();

      // Get references to Product Stream PSDs
      MIPSD & PSDof = *out_ProductA.FindIF<MIPSD>();
      MIPSD & PSDuf = *out_ProductB.FindIF<MIPSD>();

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