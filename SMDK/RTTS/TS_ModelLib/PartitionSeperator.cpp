//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __PartitionSeperator_CPP
#include "PartitionSeperator.h"
#pragma optimize("", off)

#define COMPILE_COMPONENTPARTITION 1


//====================================================================================
//             ComponentPartition
//====================================================================================

CSeperator_ComponentPartition::CSeperator_ComponentPartition()
{
#ifdef COMPILE_COMPONENTPARTITION
	m_lPartitionSel = 0;
	m_lSpeciesSel = 0;
	m_DDPSDPartition = 0;
	m_dTopWaterSplitToUS = 1.0;
	m_dBottomWaterSplitToUS = 1.0;

	int nspecies = gs_MVDefn.Count();
	int nspecies_withsize = 0;
	for (int i = 0; i < nspecies; i++ )
	{
		if ( gs_MVDefn[i].HasSizeData() ) nspecies_withsize++;
	}
	m_NSpeciesWithSizeData = nspecies_withsize;

	m_DDSpeciesPartition = new MDDValueLst[m_NSpeciesWithSizeData+1];
	m_DDSpeciesPartition[m_NSpeciesWithSizeData].m_dwFlags = 0;
	m_DDSpeciesPartition[m_NSpeciesWithSizeData].m_lVal = 0;
	m_DDSpeciesPartition[m_NSpeciesWithSizeData].m_pStr = 0;
	int k = 0;
	for (int i = 0; i < nspecies; i++ )
	{
		if ( gs_MVDefn[i].HasSizeData() )
		{
			m_DDSpeciesPartition[k].m_dwFlags = 0;
			//m_DDSpeciesPartition[k].m_lVal = i;
			m_DDSpeciesPartition[k].m_lVal = k;
			const LPCSTR s = gs_MVDefn[i].Symbol();
			m_DDSpeciesPartition[k].m_pStr = (LPTSTR)s;		
			k++;
		}
	}

	// Initialise discharge partitions for all possible
	// PSD Definitions for each species with size data 
	// that could be used for this project
	int npsds = gs_PSDDefns.Count();
	m_CTopPartition.SetSize(m_NSpeciesWithSizeData);
	m_CBottomPartition.SetSize(m_NSpeciesWithSizeData);
	for (int i = 0; i < m_NSpeciesWithSizeData; i++ )
	{
		m_CTopPartition[i].m_Partition.SetSize(npsds);
		m_CBottomPartition[i].m_Partition.SetSize(npsds);
	}
	m_DDPSDPartition = new MDDValueLst[npsds+1];
	m_DDPSDPartition[npsds].m_dwFlags = 0;
	m_DDPSDPartition[npsds].m_lVal = 0;
	m_DDPSDPartition[npsds].m_pStr = 0;


	for (int i = 0; i < m_NSpeciesWithSizeData; i++ )
	{
		for (int j=0;j< npsds; j++)
		{
			MPSDDefn& PSDDefn = gs_PSDDefns[j]; 
			int NIntervals = PSDDefn.getSizeCount();
			
			m_CTopPartition[i].m_Partition[j].m_EU.SetSize(NIntervals);
			//m_CTopPartition[i].m_Partition[j].m_dWaterSplitToUS = 1;
			m_CTopPartition[i].m_Partition[j].m_Name = PSDDefn.Name();

			m_CBottomPartition[i].m_Partition[j].m_EU.SetSize(NIntervals);
			//m_CBottomPartition[i].m_Partition[j].m_dWaterSplitToUS = 1;
			m_CBottomPartition[i].m_Partition[j].m_Name = PSDDefn.Name();
		
			m_DDPSDPartition[j].m_dwFlags = 0;
			m_DDPSDPartition[j].m_lVal = j;
			const LPCSTR s = m_CTopPartition[i].m_Partition[j].m_Name.GetString();
			m_DDPSDPartition[j].m_pStr = (LPTSTR)s;		
		}	

	}
#endif
}

//====================================================================================

void CSeperator_ComponentPartition::BuildDataFields(MDataDefn &DB)
	
{ 
#ifdef COMPILE_COMPONENTPARTITION
    CString Tg1,Tg2;

    DB.ObjectBegin("TS_PartSep", "PartitionSep");

	DB.Page("Top Deck Partition");
	DB.Long  ("Partition", "", &m_lPartitionSel  , MF_PARAMETER, m_DDPSDPartition );
	DB.Long  ("Species",   "", &m_lSpeciesSel    , MF_PARAMETER, m_DDSpeciesPartition );
	DB.Double("TopWaterSplitToUS"	, "TopWaterSplit", &m_dTopWaterSplitToUS, MF_PARAMETER ,  MC_Frac("%"));

	for ( int i = 0; i < m_NSpeciesWithSizeData; i++)
	{
		for (int j = 0; j < m_CTopPartition[i].m_Partition.GetSize() ; j++)
		{
			DWORD flags;
			if (( j == m_lPartitionSel )&&(i == m_lSpeciesSel))
				flags = MF_PARAMETER;
			else
				flags = MF_PARAMETER | MF_NO_VIEW;

			//
			// Top Deck
			//
			Tg1.Format("Top%02d_%02d",i,j);
			//DB.StructBegin("TS_Top", Tg1 );
			DB.StructBegin( Tg1 );

			Tg1.Format("TS_TopGlobalSep%02d_%02d",i,j);
			Tg2.Format("GlobalSep%02d_%02d",i,j);
			DB.ObjectBegin(Tg1, /*Tg2*/"GlobalSep");
			for (int k=(m_CTopPartition[i].m_Partition[j].m_EU.GetSize()-1); k>=0; k--)
			{
				Tg1.Format("I%02d", k);
				DB.Double((char*)(const char*)Tg1, "", &m_CTopPartition[i].m_Partition[j].m_EU[k], flags , MC_Frac("%"));
			}
			DB.ObjectEnd();

			DB.StructEnd();
		}

	}

	DB.Page("Bottom Deck Partition");
	DB.Double("BottomWaterSplitToUS"	, "BottomWaterSplit", &m_dBottomWaterSplitToUS, MF_PARAMETER ,  MC_Frac("%"));

	for ( int i = 0; i < m_NSpeciesWithSizeData; i++)
	{

		for (int j = 0; j < m_CBottomPartition[i].m_Partition.GetSize() ; j++)
		{
			DWORD flags;
			if (( j == m_lPartitionSel )&&(i == m_lSpeciesSel))
				flags = MF_PARAMETER;
			else
				flags = MF_PARAMETER | MF_NO_VIEW;

			//
			// Bottom Deck
			//
			Tg1.Format("Bottom%02d_%02d",i,j);
			//DB.StructBegin("TS_Bottom", Tg1 );
			DB.StructBegin( Tg1 );

			Tg1.Format("TS_BottomGlobalSep%02d_%02d",i,j);
			Tg2.Format("GlobalSep%02d_%02d",i,j);
			DB.ObjectBegin(Tg1, /*Tg2*/"GlobalSep");
			//DB.Double("WaterSplitToUS"	, "", &m_CBottomPartition[i].m_Partition[0].m_dWaterSplitToUS, flags ,  MC_Frac("%"));
			for (int k=(m_CBottomPartition[i].m_Partition[j].m_EU.GetSize()-1); k>=0; k--)
			{
				Tg1.Format("I%02d", k);
				DB.Double((char*)(const char*)Tg1, "", &m_CBottomPartition[i].m_Partition[j].m_EU[k], flags , MC_Frac("%"));
			}
			DB.ObjectEnd();

			DB.StructEnd();


		}

	}

	DB.ObjectEnd();
#endif
}

//====================================================================================

void CSeperator_ComponentPartition::EvalProducts(MStream &Feed ,
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
				Seperate(&m_CTopPartition,iPSD,
						 m_dTopWaterSplitToUS,
						 Feed,
						 Product1 ,
						 Product2);

				// Execute the second seperator
				Seperate(&m_CBottomPartition,iPSD,
					     m_dBottomWaterSplitToUS,
						 Product2,
						 Product2 ,
						 Product3);
	  
	 
			}
			else
			{
				// Execute the first seperator
				Seperate(&m_CTopPartition,iPSD,
						 m_dBottomWaterSplitToUS,
						 Feed,
						 Product1 ,
						 Product3);
			}
	}
}

//====================================================================================

void CSeperator_ComponentPartition::Seperate(CArray <CPartitionArray, CPartitionArray&> *pCPartition,
											 long in_PSD,
											 double  in_WaterSplitToUS,
											 MStream &in_Feed,
		                                     MStream &out_ProductA ,
		                                     MStream &out_ProductB)
	{
#ifdef COMPILE_COMPONENTPARTITION
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
          const double Split = (*pCPartition)[lPSDVec].m_Partition[in_PSD].m_EU[lSzIndex];

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
#endif
	}

//====================================================================================