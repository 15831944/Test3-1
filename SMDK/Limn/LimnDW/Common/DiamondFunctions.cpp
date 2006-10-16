/*  
**
**      DiamondWizard Related Functions for Limn: The Flowsheet Processor
**      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
**
**      copyright David M. Wiseman 2000
**
**
*/
#include "stdafx.h"

#ifndef LIMNDW
	#include "globals.h"
#endif
//
#include "LimnModel_DiamondWizard_CommonFunctions.h"
//
//
//
#ifndef LIMNDW				// Excel AddIn stuff not required for SysCAD
//
//
//												
//
//
//.............................................................................
//
/*
**              Called whenever DiamondWizard.XLL is opened
*/
C_LINKAGE
            int DLLFUNC xlAutoOpen( )
{                                   
    int retCode = 1 ;
   
    g->Excel.fn( xlGetName ) ;
    CString thisDLLtext = g->Excel.resultCString() ;   // full path to this module
    xl thisDLL( thisDLLtext ) ;
    
    
    g->Excel.fn( xlfGetWorkspace, xl(2) );
    g->ExcelVersion = g->Excel.resultCString() ;

                                                   
    for( int i = 0 ; i < nR ; i++ )
    {
        g->Excel.fn( xlfRegister, thisDLL,                 // Register the functions
                               g->moduleText[i],
                               g->typeText[i],
                               g->moduleText[i],
                               g->argumentText[i],
                               g->number_1,
                               g->category,
							   xl(),
							   xl(),
							   g->functionHelp[i],
							   g->argument1Help[i],
							   g->argument2Help[i],
							   g->argument3Help[i],
							   g->argument4Help[i],
							   g->argument5Help[i],
							   g->argument6Help[i],
							   g->argument7Help[i],
							   g->argument8Help[i]
							   ) ;
    } 
    

    return retCode ;

}                           
//
//
//
//.............................................................................
//
/*
**              Called whenever DiamondWizard.XLL is closed
*/
C_LINKAGE
            int DLLFUNC xlAutoClose( )
{
    int retCode = 1 ;
    
	for( int i = 0 ; i < nR ; i++ )
    {
        g->Excel.fn( xlfSetName, g->moduleText[i] ) ;         // clear the function names
    } 

    return retCode ;

}

//
//.............................................................................
//
/*
**              Called whenever DiamondWizard.XLL is added with the Add-In manager
*/
C_LINKAGE
            int DLLFUNC xlAutoAdd( )
{   
    int retCode = 1 ;

    return retCode ;
    
}

//
//.............................................................................
//
/*
**              Called whenever DiamondWizard.XLL is removed with the Add-In manager
*/
C_LINKAGE
            int DLLFUNC xlAutoRemove( )
{                                                    
    int retCode = 1 ;

    return retCode ;
}
//
//.............................................................................
//
/*
**              Called by Add-In manager to get descriptive name for this XLL
*/
C_LINKAGE
            LPXLOPER DLLFUNC xlAddInManagerInfo( LPXLOPER xAction )
{
    g->Excel.fn( xlCoerce, xlp(xAction), xl(xltypeInt) ) ;

    if(g->Excel.resultIntW() == 1) 
    {
        return xlExit("Limn:DiamondWizard V1.3 Functions") ;
    }
    else 
    {
        return xlExit(xlerrValue) ;
    }
}
//
//
//.............................................................................
//
/*
**
**						Test Function
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_A_TEST( LPXLOPER lpxConfigID, 
										  LPXLOPER lpxParam1,
										  LPXLOPER lpxParam2,
										  LPXLOPER lpxParam3,
										  LPXLOPER lpxParam4,
										  LPXLOPER lpxParam5  ) 
{   
    double* resultArray = &g->OutputBuffer[0] ;


    int iOSz = 0 ;							// Counters
	int jOSz = 0 ;
    int iSG  = 0 ;
	int iDSz = 0 ;

	int idx  = 0 ;
	int jdx  = 0 ;

    int nOSz = 0 ;							// Limits
    int nSG  = 0 ;
	int nDSz = 0 ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			double requiredT10 ;		// bring in the parameter data
			retCode = CoerceReference( lpxParam1, requiredT10 ) ;
			if ( retCode == xlerrNull )
			{
							//										// bring in the parameter data
				g->Excel.fn( xlCoerce, xlp( lpxParam2 ), xl(xltypeMulti) ) ;
				if ( retCode == xlerrNull )
				{
					if ( g->Excel.resultRows() != 5) return xlExit( xlerrRef ) ;
					if ( g->Excel.resultColumns() != 5) return xlExit( xlerrRef ) ;

					double t10Params[25] ;
					for ( int i = 0 ; i < 25 ; i++ )
					{
						t10Params[i] = g->Excel.resultArrayNum(i) ; 
					}

					DiagonalMatrix C(nOSz) ;
					C = 0.0 ;

					double K1 = 25.0 ;
					double K2 = 45.0 ;
					double K3 = 2.30 ;

					double* Size = thisConfig->pMeanOreSizemm() ;
					WhitenCrusherClassificationFunction( nOSz, Size, K1, K2, K3, C) ;

					Matrix T10_V_Tn(5,5) ;
					T10_V_Tn << t10Params ;

					LowerTriangularMatrix B(nOSz) ; 
					B = 0.0 ;

					WhitenCrusherBreakageFunction( nOSz, Size, requiredT10, T10_V_Tn, B) ;

					Matrix X(nOSz,nOSz) ; X = 0.0 ;

					WhitenCrusherTransformationMatrix( B, C, X ) ;

					for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
					{
						for ( jOSz = 1 ; jOSz <= nOSz ; jOSz++ )
						{
							resultArray[idx++] = X(iOSz,jOSz) ;
						}
					}

					return xlExit( nOSz, nOSz, resultArray ) ;				// OK Return - output the array

				}
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//.............................................................................
//
/*
**              Returns the Build Version of this Library
*/
C_LINKAGE 
            LPXLOPER DLLFUNC v1_3_DiamondWizardLibraryVersion()
{
	CString buildStamp = __TIME__ ; // date, time of Build
    buildStamp += " " ;
    buildStamp += __DATE__ ;

	return xlExit( buildStamp ) ;
}
//
//
//.............................................................................
//
/*
**                      Calculate the once off Configuration data
**						and store it in local storage.
**						Returns a unique ID to identify the 
**						particular configuration.
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondWizardConfigureXLL( LPXLOPER OreSizes,
															 LPXLOPER DiamondSizes,
															 LPXLOPER SGs,
															 LPXLOPER DiamondSG,
															 LPXLOPER Revenue,
															 LPXLOPER LiberationMatrix,
															 LPXLOPER PassSizePercents)
{   
	int retCode ;

	DWORD ConfigurationSheetID = DiamondSG->val.mref.idSheet ;


	CDiamondWizardConfiguration* thisConfig = g->DiamondWizardConfigurations.Update( ConfigurationSheetID );

	retCode = thisConfig->Initialise(OreSizes, DiamondSizes, SGs, DiamondSG, Revenue, LiberationMatrix, PassSizePercents ) ;

	if ( retCode == xlerrNull )
	{
		return xlExit( double( thisConfig->ID() ) ) ;				// return the configurationID
	}
	else
	{
		return xlExit( retCode ) ;
	}

}
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondWizardConfigureXLL_Densimetrics( LPXLOPER lpxConfigID, 
																		  LPXLOPER ReferenceDensimetrics)
{   
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			retCode = thisConfig->InitialiseReferenceDensimetrics( ReferenceDensimetrics ) ;

			if ( retCode == xlerrNull )
			{
				return xlExit( "Init_OK" ) ;
			}
			else
			{
				return xlExit( retCode ) ;
			}
		}
	}
	return xlExit( "BAD CONFIG ID" ) ;

}
//
//
//.............................................................................
//
/*
**                       Calculate Diamond Revenue
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondRevenue( LPXLOPER lpxConfigID, 
												  LPXLOPER lpxDiamondDeportment ) 
{   
    double theRevenue = 0.0 ;
    int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;

    int nDSz ;							// Limits
    int nOSz ;
    int nSG  ;

	int idx = 0 ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;
			
													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							theRevenue += ( thisConfig->Revenue(iDSz) * g->Excel.resultArrayNum(idx) ) ;   // accumulate the revenue the data for each element
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}
			}
			return xlExit( theRevenue ) ;		// OK Return
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//
//.............................................................................
//
/*
**                       Calculate Diamond Carats
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondCarats( LPXLOPER lpxConfigID, 
												  LPXLOPER lpxDiamondDeportment ) 
{   
    double theCarats = 0.0 ;
    int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;

    int nDSz ;							// Limits
    int nOSz ;
    int nSG  ;

	int idx = 0 ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;
			
													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							theCarats += g->Excel.resultArrayNum(idx) ;   // accumulate the carat data for each element
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}
			}
			return xlExit( theCarats ) ;		// OK Return
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//
//.............................................................................
//
/*
**                       Calculate Diamond Revenue Distribution by Diamond Size
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondRevenueDistribution( LPXLOPER lpxConfigID, 
															  LPXLOPER lpxDiamondDeportment ) 
{   
    double* theRevenue = &g->OutputBuffer[0] ;

	double totalRevenue = 0.0 ;
	double thisRevenue = 0.0 ;

    int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;

    int nDSz ;							// Limits
    int nOSz ;
    int nSG  ;

	int idx = 0 ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;
			
													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				theRevenue[iDSz] = 0.0 ;

				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{

						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							thisRevenue = ( thisConfig->Revenue(iDSz) * g->Excel.resultArrayNum(idx) ) ;	// revenue for this element
							theRevenue[iDSz] += thisRevenue ;												// accumulate the revenue per DiamondSize
							totalRevenue += thisRevenue ;													// accumulate total revenue
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}
			}


			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				if ( totalRevenue > 0.000000001 )
				{
					theRevenue[iDSz] = 100 * theRevenue[iDSz] / totalRevenue ;   // Expressed as a %
				}
				else
				{
					theRevenue[iDSz] = 0.0 ;
				}
			}
			
			return xlExit( nDSz, 1, theRevenue ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//
//.............................................................................
//
/*
**                       Calculate Diamond %Liberated Distribution by Diamond Size
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondLiberatedDistribution( LPXLOPER lpxConfigID, 
																LPXLOPER lpxDiamondDeportment ) 
{   
    double* theLiberated = &g->OutputBuffer[0] ;

	double thisMass ;
	double totalMass ;
	double thisLiberated ;

    int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;

    int nDSz ;							// Limits
    int nOSz ;
    int nSG  ;

	int idx = 0 ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;
			
													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				thisLiberated = 0.0 ;
				totalMass = 0.0 ;

				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{

						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							thisMass = g->Excel.resultArrayNum(idx) ;
							totalMass += thisMass ;

							if ( thisConfig->IsLiberatedDiamond( iDSz, iOSz ) )			// check the diamond against all ore sizes
							{															// to see if it is liberated
								thisLiberated += thisMass ;
							}
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}

				if ( totalMass > 0.000000001 )
				{
					theLiberated[iDSz] = 100 * thisLiberated / totalMass ;   // Expressed as a %
				}
				else
				{
					theLiberated[iDSz] = 0.0 ;
				}


			}

			
			return xlExit( nDSz, 1, theLiberated ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//
//.............................................................................
//
/*
**                       Calculate Diamond Mass Distribution by Diamond Size
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondTotalSizeDistribution( LPXLOPER lpxConfigID, 
																LPXLOPER lpxDiamondDeportment ) 
{   
    double* theMass = &g->OutputBuffer[0] ;

	double totalMass = 0.0 ;
	double thisMass = 0.0 ;

    int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;

    int nDSz ;							// Limits
    int nOSz ;
    int nSG  ;

	int idx = 0 ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;
			
													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				theMass[iDSz] = 0.0 ;

				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{

						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							thisMass = g->Excel.resultArrayNum(idx) ;										// mass for this element
							theMass[iDSz] += thisMass ;														// accumulate the mass per DiamondSize
							totalMass += thisMass ;															// accumulate total mass
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}
			}


			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				if ( totalMass > 0.000000001 )
				{
					theMass[iDSz] = 100 * theMass[iDSz] / totalMass ;   // Expressed as a %
				}
				else
				{
					theMass[iDSz] = 0.0 ;
				}
			}
			
			return xlExit( nDSz, 1, theMass ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//
//.............................................................................
//
/*
**
**						Function to convert from DiamondWizard
**						Stream Data Ore Distribution form to Mass form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreMassFromSizeDistribution( LPXLOPER lpxConfigID, 
															   LPXLOPER lpxOreDistribution, 
			                                                   LPXLOPER lpxTotalFlow ) 
{   
    double* OreMass = &g->OutputBuffer[0] ;

	double SGFractionFlow ;

    int iOSz ;							// Counters
    int iSG  ;
	int idx  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			double TotalFlow ;						// bring in the total flowrate data
			retCode = CoerceReference( lpxTotalFlow, TotalFlow ) ;
			if ( retCode == xlerrNull )
			{

														// bring in the ore distribution data
				g->Excel.fn( xlCoerce, xlp( lpxOreDistribution ), xl(xltypeMulti) ) ;
														// validity checks
				if ( g->Excel.resultColumns() !=  nSG       ) return xlExit( xlerrRef ) ;
				if ( g->Excel.resultRows()    != (nOSz + 1) ) return xlExit( xlerrRef ) ;

				if ( TotalFlow > 0.0000001 )				// Handle start up conditions
				{
					for ( iSG = 0 ; iSG < nSG ; iSG++ )
					{
						SGFractionFlow = TotalFlow * g->Excel.resultArrayNum( xlIndex( nOSz, iSG, nSG ) ) / 10000.0 ;

						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							idx = xlIndex( iOSz, iSG, nSG ) ;
							if (g->Excel.resultArrayXltype(idx) == xltypeNum )
							{
								OreMass[idx] = SGFractionFlow * g->Excel.resultArrayNum( idx ) ;
							}
							else
							{
								return xlExit (xlerrNum ) ;  // error return, range has non number
							}                                                                               
						}
					}
				}
				else
				{
					for ( iSG = 0 ; iSG < nSG ; iSG++ )				// Distribution data = 0 if flow = 0
					{
						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							idx = xlIndex( iOSz, iSG, nSG ) ;
							OreMass[idx] = 0 ;
						}
					}
				}
				
				return xlExit( nOSz, nSG, OreMass ) ;				// OK Return - output the array
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Function to convert from DiamondWizard
**						Stream Data Ore Mass form to Distribution form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreSizeDistributionFromMass( LPXLOPER lpxConfigID, 
															   LPXLOPER lpxOreMass ) 
{   
    double* OreDistribution = &g->OutputBuffer[0] ;

    double sizeFractionFlows[100] ;

	double TotalFlow ;

	double SGFractionFlows[100] ;

    int iOSz ;							// Counters
    int iSG  ;
	int idx  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			TotalFlow = 0.0 ;

													// bring in the ore mass data
			g->Excel.fn( xlCoerce, xlp( lpxOreMass ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nSG  ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != nOSz ) return xlExit( xlerrRef ) ;


			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				sizeFractionFlows[iOSz] = 0.0 ;
			}
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				SGFractionFlows[iSG] = 0.0 ;
			}
			
			for ( idx = 0 ; idx < (nSG+1)*(nOSz+1) ; idx++ )
			{
				OreDistribution[idx] = 0.0 ;
			}
			
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
				{
					double thisOreMass = g->Excel.resultArrayNum( xlIndex(iOSz, iSG, nSG) ) ;
					SGFractionFlows[iSG] += thisOreMass ;
					sizeFractionFlows[iOSz] += thisOreMass ;
				}
				
				TotalFlow += SGFractionFlows[iSG] ;

				if ( SGFractionFlows[iSG] < 0.0000001 ) SGFractionFlows[iSG] = 0.0 ;
				
		
				for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
				{
					idx = xlIndex(iOSz, iSG, nSG+1) ;
				    if( SGFractionFlows[iSG] > 0 )
					{
						OreDistribution[idx] = 100.0 * g->Excel.resultArrayNum( xlIndex(iOSz, iSG, nSG) ) / SGFractionFlows[iSG] ;
					}
					else
					{
						OreDistribution[idx] = 0 ;
					}
				}
			}

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
													// SG Distribution
				OreDistribution[ xlIndex(nOSz,iSG, nSG+1) ] = 100.0 * SGFractionFlows[iSG] / TotalFlow ;
			}

			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{										// Size Distribution
				if ( sizeFractionFlows[iOSz] < 0.0000001 )  sizeFractionFlows[iOSz] = 0.0 ;

				OreDistribution[ xlIndex(iOSz,nSG, nSG+1) ] = 100.0 * sizeFractionFlows[iOSz] / TotalFlow ;
			}
			
			return xlExit( nOSz+1, nSG+1, OreDistribution ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Function to calculate overall size distribution
**						from Stream Data Ore Mass data
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreTotalSizeDistribution( LPXLOPER lpxConfigID, 
															LPXLOPER lpxOreMass ) 
{   
    double* OreDistribution = &g->OutputBuffer[0] ;

    double sizeFractionFlows[100] ;

	double TotalFlow ;

    int iOSz ;							// Counters
    int iSG  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			TotalFlow = 0.0 ;
													// bring in the ore mass data
			g->Excel.fn( xlCoerce, xlp( lpxOreMass ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nSG  ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != nOSz ) return xlExit( xlerrRef ) ;


			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				sizeFractionFlows[iOSz] = 0.0 ;
				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					sizeFractionFlows[iOSz] += g->Excel.resultArrayNum( xlIndex(iOSz, iSG, nSG) ) ;
				}

				TotalFlow += sizeFractionFlows[iOSz] ;
			
			}
			
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				if ( sizeFractionFlows[iOSz] < 0.0000001 )  sizeFractionFlows[iOSz] = 0.0 ;

				OreDistribution[iOSz] = 100.0 * sizeFractionFlows[iOSz] / TotalFlow ;
			}
		
			return xlExit( nOSz, 1, OreDistribution ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Function to convert from DiamondWizard
**						Stream Data Ore SG Distribution form to Mass form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreMassFromSGDistribution( LPXLOPER lpxConfigID, 
															 LPXLOPER lpxOreDistribution, 
			                                                 LPXLOPER lpxTotalFlow ) 
{   
    double* OreMass = &g->OutputBuffer[0] ;

	double SizeFractionFlow ;

    int iOSz ;							// Counters
    int iSG  ;
	int idx  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			double TotalFlow ;						// bring in the total flowrate data
			retCode = CoerceReference( lpxTotalFlow, TotalFlow ) ;
			if ( retCode == xlerrNull )
			{

														// bring in the ore distribution data
				g->Excel.fn( xlCoerce, xlp( lpxOreDistribution ), xl(xltypeMulti) ) ;
														// validity checks
				if ( g->Excel.resultColumns() != (nSG + 1)  ) return xlExit( xlerrRef ) ;
				if ( g->Excel.resultRows()    !=  nOSz      ) return xlExit( xlerrRef ) ;

				if ( TotalFlow > 0.0000001 )				// Handle start up conditions
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						SizeFractionFlow = TotalFlow * g->Excel.resultArrayNum( xlIndex( iOSz, nSG, nSG+1 ) ) / 10000.0 ;

						for ( iSG = 0 ; iSG < nSG ; iSG++ )
						{
							idx = xlIndex( iOSz, iSG, nSG+1 ) ;
							if (g->Excel.resultArrayXltype(idx) == xltypeNum )
							{
								OreMass[ xlIndex( iOSz, iSG, nSG ) ] = SizeFractionFlow * g->Excel.resultArrayNum( idx ) ;
							}
							else
							{
								return xlExit (xlerrNum ) ;  // error return, range has non number
							}                                                                               
						}
					}
				}
				else
				{
					for ( iSG = 0 ; iSG < nSG ; iSG++ )				// Distribution data = 0 if flow = 0
					{
						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							idx = xlIndex( iOSz, iSG, nSG ) ;
							OreMass[idx] = 0 ;
						}
					}
				}
				
				return xlExit( nOSz, nSG, OreMass ) ;				// OK Return - output the array
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Function to convert from DiamondWizard
**						Stream Data Ore Mass form to SG Distribution form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreSGDistributionFromMass( LPXLOPER lpxConfigID, 
															 LPXLOPER lpxOreMass ) 
{   
    double* OreDistribution = &g->OutputBuffer[0] ;

    double sizeFractionFlows[100] ;

	double TotalFlow ;

	double SGFractionFlows[100] ;

    int iOSz ;							// Counters
    int iSG  ;
	int idx  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			TotalFlow = 0.0 ;

													// bring in the ore mass data
			g->Excel.fn( xlCoerce, xlp( lpxOreMass ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nSG  ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != nOSz ) return xlExit( xlerrRef ) ;


			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				sizeFractionFlows[iOSz] = 0.0 ;
			}
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				SGFractionFlows[iSG] = 0.0 ;
			}
			
			for ( idx = 0 ; idx < (nSG+1)*(nOSz+1) ; idx++ )
			{
				OreDistribution[idx] = 0.0 ;
			}
			
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					double thisOreMass = g->Excel.resultArrayNum( xlIndex(iOSz, iSG, nSG) ) ;
					SGFractionFlows[iSG] += thisOreMass ;
					sizeFractionFlows[iOSz] += thisOreMass ;
				}
				
				TotalFlow += sizeFractionFlows[iOSz] ;

				if ( sizeFractionFlows[iOSz] < 0.0000001 ) sizeFractionFlows[iOSz] = 0.0 ;
				
		
				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					idx = xlIndex(iOSz, iSG, nSG+1) ;
				    if( sizeFractionFlows[iOSz] > 0 )
					{
						OreDistribution[idx] = 100.0 * g->Excel.resultArrayNum( xlIndex(iOSz, iSG, nSG) ) / sizeFractionFlows[iOSz] ;
					}
					else
					{
						OreDistribution[idx] = 0 ;
					}
				}
			}

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
													// SG Distribution
				if ( SGFractionFlows[iSG] < 0.0000001 ) SGFractionFlows[iSG] = 0.0 ;
				OreDistribution[ xlIndex(nOSz,iSG, nSG+1) ] = 100.0 * SGFractionFlows[iSG] / TotalFlow ;
			}
				
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{										// Size Distribution
				OreDistribution[ xlIndex(iOSz,nSG, nSG+1) ] = 100.0 * sizeFractionFlows[iOSz] / TotalFlow ;
			}
			
			return xlExit( nOSz+1, nSG+1, OreDistribution ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Function to convert from DiamondWizard
**						Stream Data Densimetric data form to Mass form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreMassFromDensimetricDistribution( LPXLOPER lpxConfigID, 
															          LPXLOPER lpxDensimetricDistribution, 
													                  LPXLOPER lpxSizeDistribution, 
			                                                          LPXLOPER lpxTotalFlow ) 
{   
    double* OreMass = &g->OutputBuffer[0] ;

    int iOSz ;							// Counters
    int iSG  ;
	int idx  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode != xlerrNull )
    {
    	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID
    }

    CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
	if ( thisConfig == NULL )
    {
    	return xlExit ( xlerrRef ) ;	// error return, bad configuration worksheet
    }

    nOSz = thisConfig->nOreSizes() ;
	nSG  = thisConfig->nSGs() ;

    double TotalFlow ;						// bring in the total flowrate data
    retCode = CoerceReference( lpxTotalFlow, TotalFlow ) ;
    if ( retCode != xlerrNull )
    {
    	return xlExit ( xlerrRef ) ;	    // error return, bad Total Flow reference
    }
	
    if ( TotalFlow < 0.0000001 )				// Handle start up conditions
	{
		for ( iSG = 0 ; iSG < nSG ; iSG++ )				// Distribution data = 0 if flow = 0
		{
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				idx = xlIndex( iOSz, iSG, nSG ) ;
				OreMass[idx] = 0 ;
			}
		}
	}
    else
    {

        double* sizeFractionData = new double[nOSz] ;
												// bring in the size distribution data
		g->Excel.fn( xlCoerce, xlp( lpxSizeDistribution ), xl(xltypeMulti) ) ;
												// validity checks
		if ( g->Excel.resultRows() != nOSz ) 
        {
            delete sizeFractionData ;
            return xlExit( xlerrRef ) ;
        }
		for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
		{
			if (g->Excel.resultArrayXltype( iOSz ) == xltypeNum )
			{
    			sizeFractionData[iOSz] = g->Excel.resultArrayNum( iOSz );
            }
            else
            {
                delete sizeFractionData ;
				return xlExit (xlerrNum ) ;  // error return, range has non number
			}                                                                               
        }

												// bring in the densimetric distribution data
		g->Excel.fn( xlCoerce, xlp( lpxDensimetricDistribution ), xl(xltypeMulti) ) ;
												// validity checks
		if ( g->Excel.resultColumns() != nSG  )
        {
            delete sizeFractionData ;
            return xlExit( xlerrRef ) ;
        }
		if ( g->Excel.resultRows()    != nOSz )
        {
            delete sizeFractionData ;
            return xlExit( xlerrRef ) ;
        }

        double* densimetricDistribution = new double[nOSz * nSG] ;

		for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
		{
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				idx = xlIndex( iOSz, iSG, nSG ) ;
				if (g->Excel.resultArrayXltype(idx) == xltypeNum )
				{
					densimetricDistribution[ idx ] = g->Excel.resultArrayNum( idx ) ;
				}
				else
				{
                    densimetricDistribution[ idx ] = 0.0 ;
				}                                                                               
			}
		}


		OreMassFromDensimetricDistribution( thisConfig, densimetricDistribution, sizeFractionData, TotalFlow, OreMass ) ;

		delete sizeFractionData ;
		delete densimetricDistribution ;
	}

    return xlExit( nOSz, nSG, OreMass ) ;				// OK Return - output the array

}

//
//
//.............................................................................
//
/*
**
**						Function to calculate overall SG distribution
**						from Stream Data Ore Mass data
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_OreTotalSGDistribution( LPXLOPER lpxConfigID, 
														  LPXLOPER lpxOreMass ) 
{   
    double* OreDistribution = &g->OutputBuffer[0] ;

    double SGFractionFlows[100] ;

	double TotalFlow ;

    int iOSz ;							// Counters
    int iSG  ;

    int nOSz ;							// Limits
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

			TotalFlow = 0.0 ;
													// bring in the ore mass data
			g->Excel.fn( xlCoerce, xlp( lpxOreMass ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nSG  ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != nOSz ) return xlExit( xlerrRef ) ;


			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				SGFractionFlows[iSG] = 0.0 ;
				for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
				{
					SGFractionFlows[iSG] += g->Excel.resultArrayNum( xlIndex(iOSz, iSG, nSG) ) ;
				}

				TotalFlow += SGFractionFlows[iSG] ;
			
			}
			
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				if ( SGFractionFlows[iSG] < 0.0000001 )  SGFractionFlows[iSG] = 0.0 ;

				OreDistribution[iSG] = 100.0 * SGFractionFlows[iSG] / TotalFlow ;
			}
		
			return xlExit( 1, nSG, OreDistribution ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Functions to convert from DiamondWizard
**						Stream Data Diamond Deportment form to Distribution form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondSizeDistributionFromDeportment( LPXLOPER lpxConfigID, 
		 																 LPXLOPER lpxDiamondDeportment)
{   
    double* DiamondDistribution = &g->OutputBuffer[0] ;
    double thisMass ;
	double SGFractionMass ;

	int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;
	int idx  ;

	int nDSz ;							// Limits
    int nOSz ;							
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				SGFractionMass = 0.0 ;						// Sum the mass in each SG Fraction
				for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							SGFractionMass += g->Excel.resultArrayNum(idx) ; 
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}

				if ( SGFractionMass > 0.0000001 )			// Startup conditions?				
				{
					for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
					{
						thisMass = 0.0 ;
						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;
							thisMass += g->Excel.resultArrayNum(idx) ;			// Distribution calculations by Diamond Size
						}

						DiamondDistribution[ xlIndex( iDSz, iSG, nSG ) ] = 100.0 * thisMass / SGFractionMass ;
					}
				}
				else
				{																// 0 mass means 0 distribution
					for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
					{

						DiamondDistribution[ xlIndex( iDSz, iSG, nSG ) ] = 0.0 ;
					}
				}
			}
			return xlExit( nDSz, nSG, DiamondDistribution ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondSGDistributionFromDeportment( LPXLOPER lpxConfigID, 
		 															   LPXLOPER lpxDiamondDeportment)
{   
    double* DiamondDistribution = &g->OutputBuffer[0] ;
    double thisMass ;
	double SizeFractionMass ;

	int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;
	int idx  ;

	int nDSz ;							// Limits
    int nOSz ;							
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;

													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				SizeFractionMass = 0.0 ;						// Sum the mass in each Diamond Size Fraction
				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							SizeFractionMass += g->Excel.resultArrayNum(idx) ; 
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}                                                                               
					}
				}

				if ( SizeFractionMass > 0.0000001 )			// Startup conditions?				
				{
					for ( iSG = 0 ; iSG < nSG ; iSG++ )
					{
						thisMass = 0.0 ;
						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;
							thisMass += g->Excel.resultArrayNum(idx) ;			// Distribution calculations by SG for each diamond size
						}

						DiamondDistribution[ xlIndex( iDSz, iSG, nSG ) ] = 100.0 * thisMass / SizeFractionMass ;
					}
				}
				else
				{																// 0 mass means 0 distribution
					for ( iSG = 0 ; iSG < nSG ; iSG++ )
					{

						DiamondDistribution[ xlIndex( iDSz, iSG, nSG ) ] = 0.0 ;
					}
				}
			}
			return xlExit( nDSz, nSG, DiamondDistribution ) ;				// OK Return - output the array

		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//.............................................................................
//
/*
**
**						Functions to convert from DiamondWizard Stream Data
**						Ore and Diamond Mass form to Diamond Deportment form
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondDeportmentFromSizeDistribution( LPXLOPER lpxConfigID, 
																		 LPXLOPER lpxOreDistribution,
																		 LPXLOPER lpxDiamondDistribution,
 																		 LPXLOPER lpxDiamondFlowBasis )
{   

//	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

//}
//
/*
**
**						Function to convert from DiamondWizard Stream Data
**						Ore and Diamond Mass form to Diamond Deportment form
**

C_LINKAGE
            LPXLOPER DLLFUNC DiamondDeportmentFromDistribution( LPXLOPER lpxConfigID, 
																 LPXLOPER lpxOreDistribution,
																  LPXLOPER lpxDiamondDistribution,
 																   LPXLOPER lpxDiamondFlowBasis )
{
*/
    double* DiamondDeportment = &g->OutputBuffer[0] ;
	double SGFractionFlow[100] ;

	int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;
	int idx  ;

	int nDSz ;							// Limits
    int nOSz ;							
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;


			double DiamondFlowBasis ;						// bring in the total diamond flow data
			retCode = CoerceReference( lpxDiamondFlowBasis, DiamondFlowBasis ) ;
			if ( retCode == xlerrNull )
			{
														// bring in the ore distribution data
				g->Excel.fn( xlCoerce, xlp( lpxOreDistribution ), xl(xltypeMulti) ) ;
														// validity checks
				if ( g->Excel.resultColumns() != nSG )        return xlExit( xlerrRef ) ;
				if ( g->Excel.resultRows()    != (nOSz + 1) ) return xlExit( xlerrRef ) ;

				double* OreDistribution = new double[nOSz*nSG] ;			// Need local storage while accessing other data
				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						OreDistribution[ xlIndex( iOSz, iSG, nSG ) ] = g->Excel.resultArrayNum( xlIndex( iOSz, iSG, nSG ) );
					}

					SGFractionFlow[iSG] = DiamondFlowBasis * g->Excel.resultArrayNum( xlIndex( nOSz, iSG, nSG ) ) / 1000000.0 ;
				
				}

														// bring in the diamond distribution data
				g->Excel.fn( xlCoerce, xlp( lpxDiamondDistribution ), xl(xltypeMulti) ) ;
														// validity checks
				if ( ( g->Excel.resultColumns() != nSG  ) || ( g->Excel.resultRows() != nDSz ) )
				{
					delete OreDistribution ;
					return xlExit( xlerrRef ) ;
				}

				for ( iSG = 0 ; iSG < nSG ; iSG++ )
				{
					for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
					{
						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							idx = xlIndex(iDSz, iSG, nSG) ;

							if (g->Excel.resultArrayXltype(idx) == xltypeNum )
							{
								DiamondDeportment[ thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ] = SGFractionFlow[iSG] 
									                          * g->Excel.resultArrayNum( idx ) 
															       * OreDistribution[ xlIndex( iOSz, iSG, nSG ) ] ; 
							}
							else
							{
								delete OreDistribution ;
								return xlExit (xlerrNum ) ;  // error return, range has non number
							}                                                                               
						}
					}
				}
				if ( nSG % 2 != 0 )									// left over bits at end of deportment array (odd umber of SGs)
				{
					iSG = nSG ;
					for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
					{
						for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
						{
							DiamondDeportment[ thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ] = 0.00 ;
						}
					}
				}

				delete OreDistribution ;
				return xlExit( 2*nOSz, nDSz * int((nSG + 1) / 2), DiamondDeportment ) ;				// OK Return - output the array
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID or bad Total Flow

}
//
//
//
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondDeportmentFromSGDistribution( LPXLOPER lpxConfigID, 
													 				   LPXLOPER lpxOreDensimetricDistribution,
													 				   LPXLOPER lpxOreSizeDistribution,
																       LPXLOPER lpxDiamondSGAllocation,
                                                                       LPXLOPER lpxDiamondSizeDistribution,
 																       LPXLOPER lpxDiamondFlowBasis )
{   
    double* DiamondDeportment = &g->OutputBuffer[0] ;

	double OreSizeDistribution[100] ;
	double DmdSizeDistribution[100] ;
    double DmdSGAllocation[100] ;

	int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;
	int idx  ;

	int nDSz ;							// Limits
    int nOSz ;							
    int nSG  ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode != xlerrNull )	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID reference

    CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
	if ( thisConfig == NULL ) return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

    nDSz = thisConfig->nDiamondSizes() ;							
	nOSz = thisConfig->nOreSizes() ;
	nSG  = thisConfig->nSGs() ;

	double DiamondFlowBasis ;						// bring in the total diamond flow data
	retCode = CoerceReference( lpxDiamondFlowBasis, DiamondFlowBasis ) ;
	if ( retCode != xlerrNull )	return xlExit ( xlerrRef ) ;	// error return, bad flow basis reference


											// bring in the diamond size distribution data
	g->Excel.fn( xlCoerce, xlp( lpxDiamondSizeDistribution ), xl(xltypeMulti) ) ;
	if ( ( g->Excel.resultColumns() != 1 ) || ( g->Excel.resultRows() != nDSz )) return xlExit( xlerrRef ) ;	// validity checks
	for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
	{
		if (g->Excel.resultArrayXltype(iDSz) != xltypeNum ) return xlExit (xlerrNum ) ;  // error return, range has non number

		DmdSizeDistribution[iDSz] = g->Excel.resultArrayNum( iDSz ) ;
	}
	
											// bring in the diamond SG allocation data
	g->Excel.fn( xlCoerce, xlp( lpxDiamondSGAllocation ), xl(xltypeMulti) ) ;
	if ( ( g->Excel.resultColumns() != 1 ) || ( g->Excel.resultRows() != nSG )) return xlExit( xlerrRef ) ;	// validity checks
	for ( iSG = 0 ; iSG < nSG ; iSG++ )
	{
		if (g->Excel.resultArrayXltype(iSG) != xltypeNum ) return xlExit (xlerrNum ) ;  // error return, range has non number

		DmdSGAllocation[iSG] = g->Excel.resultArrayNum( iSG ) ;
	}

											// bring in the ore size distribution
	g->Excel.fn( xlCoerce, xlp( lpxOreSizeDistribution ), xl(xltypeMulti) ) ;
	if ( ( g->Excel.resultColumns() != 1 ) || ( g->Excel.resultRows() != nOSz )) return xlExit( xlerrRef ) ;	// validity checks
	for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
	{
		if (g->Excel.resultArrayXltype(iOSz) != xltypeNum ) return xlExit (xlerrNum ) ;  // error return, range has non number

		OreSizeDistribution[iOSz] = g->Excel.resultArrayNum( iOSz ) ;
	}

                                        // bring in the ore densimetric analysis distribution data
	g->Excel.fn( xlCoerce, xlp( lpxOreDensimetricDistribution ), xl(xltypeMulti) ) ;
	if ( ( g->Excel.resultColumns() != nSG ) || ( g->Excel.resultRows() != nOSz )) return xlExit( xlerrRef ) ;	// validity checks

	double* OreDistribution = new double[nOSz*nSG] ;			// Need local storage while accessing other data
	for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
	{
		for ( iSG = 0 ; iSG < nSG ; iSG++ )
		{
            idx = xlIndex( iOSz, iSG, nSG ) ;
			if (g->Excel.resultArrayXltype(idx) != xltypeNum ) 
            {
                delete OreDistribution ;
                return xlExit (xlerrNum ) ;  // error return, range has non number
            }

            OreDistribution[ idx ] = g->Excel.resultArrayNum( idx );
		}
	}

    DiamondDeportmentFromSGDistribution( thisConfig, 
										  OreDistribution, 
										   OreSizeDistribution, 
										    DmdSGAllocation, 
											 DmdSizeDistribution, 
											  DiamondFlowBasis,
											   DiamondDeportment) ;

	delete OreDistribution ;

	return xlExit( 2*nOSz, nDSz * int((nSG + 1) / 2), DiamondDeportment ) ;				// OK Return - output the array

}
//
//
//.............................................................................
//
/*
**                       Function to return the total percentage
**                       of liberated diamonds from the diamond allocation
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_TotalLiberatedDiamonds( LPXLOPER lpxConfigID, 
														  LPXLOPER lpxDiamondDeportment ) 
{   
    double PercentLiberated = 0.0 ;
	double totalMass = 0.0 ;
	double thisDiamondMass = 0.0 ;

    int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;

    int nDSz ;							// Limits
    int nOSz ;
    int nSG  ;

	int idx = 0 ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nDSz = thisConfig->nDiamondSizes() ;							
			nOSz = thisConfig->nOreSizes() ;
			nSG  = thisConfig->nSGs() ;
			
													// bring in the diamond mass data
			g->Excel.fn( xlCoerce, xlp( lpxDiamondDeportment ), xl(xltypeMulti) ) ;
													// validity checks
			if ( g->Excel.resultColumns() != nDSz * int((nSG + 1) / 2) ) return xlExit( xlerrRef ) ;
			if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlExit( xlerrRef ) ;

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
				{
					for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
					{
						idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ;

						if (g->Excel.resultArrayXltype(idx) == xltypeNum )
						{
							thisDiamondMass = g->Excel.resultArrayNum(idx) ;		// mass of diamonds in fraction
						}
						else
						{
							return xlExit (xlerrNum ) ;  // error return, range has non number
						}  
						if ( thisConfig->IsLiberatedDiamond( iDSz, iOSz ) )			// check the diamond against all ore sizes
						{															// to see if it is liberated
                                        
		                    PercentLiberated += thisDiamondMass ;					// accumulate liberated diamond mass
						}
						totalMass += thisDiamondMass ;								// accumulate total diamond mass
					}
				}
			}
                                // Calculate Percentage
			if (PercentLiberated > 0)
			{
				PercentLiberated = 100.0 * PercentLiberated / totalMass ;
			}
			else
			{
				PercentLiberated = 0.0 ;
			}

			return xlExit( PercentLiberated ) ;		// OK Return
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//.............................................................................
//
/*
**			Rosin-Rammler interpolation to find size of a passing percent
**			NB. Works on a percent retained distribution, and requires
**				the DiamondWizard Configuration sheet data for sizes.
**
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_PercentPassingSize( LPXLOPER lpxConfigID, 
													  LPXLOPER lpxPercent,
 													  LPXLOPER lpxPctRetained ) 
{   
  
	int iOSz ;					 // Counters
	
    int nOSz ;					 // Limit

	double Percent ;
	double theSize ;
	double OreDistribution[100] ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			nOSz = thisConfig->nOreSizes() ;

													// bring in the required percent
			retCode = CoerceReference( lpxPercent, Percent ) ;
			if ( retCode == xlerrNull )
			{

														// bring in the percent retained data
				g->Excel.fn( xlCoerce, xlp( lpxPctRetained ), xl(xltypeMulti) ) ;
														// validity checks
				if ( g->Excel.resultColumns() != 1    ) return xlExit( xlerrRef ) ;
				if ( g->Excel.resultRows()    != nOSz ) return xlExit( xlerrRef ) ;

				for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
				{
					if (g->Excel.resultArrayXltype(iOSz) != xltypeNum )	return xlExit (xlerrNum ) ;  // error return, range has non number
					OreDistribution[iOSz] = g->Excel.resultArrayNum(iOSz) ;
				}

				theSize = CalculatePassingSize( Percent, OreDistribution, nOSz, thisConfig ) ;
				return xlExit( theSize ) ;		// OK Return
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID

}
//
//.............................................................................
//
/*
**			Function to return the stream summary
**			(Used on stream data page & for units
**			 with no product streams (eg. mining)) 
**			
**			Data calculated (rows) are:
**                Ore Solids Flow (tph) 
**                Passing size - 25%  
**                Passing size - 50%  
**                Passing size - 75%  
**                Mean SG (t/m3) 
**                Mean Size (mm) 
**                Water Flow (tph) 
**                Percent Solids  
**                Pulp SG (t/m3) 
**                FeSi Flow (tph) 
**                Carats/hr  
**                Carats/100Tonne
**                Liberated Carats/hr
**                Relative Carats
**                % Liberated  
**                Relative Revenue  
**                Rand / hr
**
**			
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummary( LPXLOPER lpxConfigID, 
									 			 LPXLOPER lpxRevenueBase,
  					 						     LPXLOPER lpxCaratsBase,
		 										 LPXLOPER lpxLimnStream )
{   

    if ( !parameterCheckOK( lpxConfigID, lpxRevenueBase, lpxCaratsBase, lpxLimnStream) ) return xlExit ( xlerrRef ) ;	// error return, bad parameter

    double* SummaryData = &g->OutputBuffer[0] ;

	double revenueBase ;
	double caratsBase ;

	int xlStatus ;
	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
													// bring in the base revenue value
			retCode = CoerceReference( lpxRevenueBase, revenueBase ) ;
			if ( retCode == xlerrNull )
			{
    													// bring in the base carat value
	    		retCode = CoerceReference( lpxCaratsBase, caratsBase ) ;
			    if ( retCode == xlerrNull )
			    {
														    // do the stream's data
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryData, lpxLimnStream, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    return xlExit( c_numSummaryRows, 1, SummaryData ) ;		// OK Return - output the array

                }
            }
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID, bad revenue base or bad Total Flow

}
//
//.............................................................................
//
/*
**			Function to return the stream summary and audit check data 
**			for units with 1 product.
**			
**			Data calculated (rows) are:
**                Ore Solids Flow (tph) 
**                Passing size - 25%  
**                Passing size - 50%  
**                Passing size - 75%  
**                Mean SG (t/m3) 
**                Mean Size (mm) 
**                Water Flow (tph) 
**                Percent Solids  
**                Pulp SG (t/m3) 
**                FeSi Flow (tph) 
**                Carats/hr  
**                Carats/100Tonne
**                Liberated Carats/hr
**                Relative Carats
**                % Liberated  
**                Relative Revenue  
**                Rand / hr
**
**
**
**			For each column, Feed, Product1, AuditCheck.
**			
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummaryCalculations1( LPXLOPER lpxConfigID, 
									 						  LPXLOPER lpxRevenueBase,
									 						  LPXLOPER lpxCaratsBase,
		 													  LPXLOPER lpxLimnFFFF,
 															  LPXLOPER lpxLimnPPP1 )
{   

    if ( !parameterCheckOK( lpxConfigID, lpxRevenueBase, lpxCaratsBase, lpxLimnFFFF, lpxLimnPPP1) ) return xlExit ( xlerrRef ) ;	// error return, bad parameter

    double* SummaryData = &g->OutputBuffer[0] ;

	double revenueBase ;
	double caratsBase ;

	int xlStatus ;

	int iRow ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;

	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
													// bring in the base revenue value
			retCode = CoerceReference( lpxRevenueBase, revenueBase ) ;
			if ( retCode == xlerrNull )
			{
    													// bring in the base carat value
	    		retCode = CoerceReference( lpxCaratsBase, caratsBase ) ;
			    if ( retCode == xlerrNull )
			    {
														    // do each stream's data in turn
				    double SummaryFFFF[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryFFFF, lpxLimnFFFF, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    double SummaryPPP1[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryPPP1, lpxLimnPPP1, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    for ( iRow = 0 ; iRow < c_numSummaryRows ; iRow++ )
				    {
					    SummaryData[ xlIndex(iRow, 0, 3) ] = SummaryFFFF[iRow] ;
					    SummaryData[ xlIndex(iRow, 1, 3) ] = SummaryPPP1[iRow] ;
					    SummaryData[ xlIndex(iRow, 2, 3) ] = 0.0 ;
				    }
				    ///	Audit those things that do add up!!
				    iRow = 0 ;				// Ore Solids 
				    SummaryData[ xlIndex(iRow, 2, 3) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow] ) ;
				    iRow = 6 ;				// Water 
				    SummaryData[ xlIndex(iRow, 2, 3) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow] ) ;
				    iRow = 9 ;				// FeSi 
				    SummaryData[ xlIndex(iRow, 2, 3) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow] ) ;
				    iRow = 10 ;				// DiamondFlow 
				    SummaryData[ xlIndex(iRow, 2, 3) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow] ) ;

				    return xlExit( c_numSummaryRows, 3, SummaryData ) ;		// OK Return - output the array
                }
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID, bad revenue base or bad Total Flow

}
//
//.............................................................................
//
/*
**			Function to return the stream summary and audit check data 
**			for units with 2 products.
**			
**			Data calculated (rows) 
**                - see StreamSummaryCalculations1
**
**			For each column, Feed, Product1, Product2, AuditCheck.
**			
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummaryCalculations2( LPXLOPER lpxConfigID, 
									 						  LPXLOPER lpxRevenueBase,
									 						  LPXLOPER lpxCaratsBase,
		 													  LPXLOPER lpxLimnFFFF,
 															  LPXLOPER lpxLimnPPP1,
 														      LPXLOPER lpxLimnPPP2 )
{   

    if ( !parameterCheckOK( lpxConfigID, lpxRevenueBase, lpxCaratsBase, lpxLimnFFFF, lpxLimnPPP1, lpxLimnPPP2) ) return xlExit ( xlerrRef ) ;	// error return, bad parameter

    double* SummaryData = &g->OutputBuffer[0] ;

	double revenueBase ;
	double caratsBase ;

	int xlStatus ;

	int iRow ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
													// bring in the base revenue value
			retCode = CoerceReference( lpxRevenueBase, revenueBase ) ;
			if ( retCode == xlerrNull )
			{
    													// bring in the base carat value
	    		retCode = CoerceReference( lpxCaratsBase, caratsBase ) ;
			    if ( retCode == xlerrNull )
			    {
														    // do each stream's data in turn
				    double SummaryFFFF[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryFFFF, lpxLimnFFFF, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    double SummaryPPP1[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryPPP1, lpxLimnPPP1, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    double SummaryPPP2[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryPPP2, lpxLimnPPP2, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    for ( iRow = 0 ; iRow < c_numSummaryRows ; iRow++ )
				    {
					    SummaryData[ xlIndex(iRow, 0, 4) ] = SummaryFFFF[iRow] ;
					    SummaryData[ xlIndex(iRow, 1, 4) ] = SummaryPPP1[iRow] ;
					    SummaryData[ xlIndex(iRow, 2, 4) ] = SummaryPPP2[iRow] ;
					    SummaryData[ xlIndex(iRow, 3, 4) ] = 0.0 ;
				    }
				    ///	Audit those things that do add up!!
				    iRow = 0 ;				// Ore Solids 
				    SummaryData[ xlIndex(iRow, 3, 4) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow] ) ;
				    iRow = 6 ;				// Water 
				    SummaryData[ xlIndex(iRow, 3, 4) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow] ) ;
				    iRow = 9 ;				// FeSi 
				    SummaryData[ xlIndex(iRow, 3, 4) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow] ) ;
				    iRow = 10 ;				// DiamondFlow 
				    SummaryData[ xlIndex(iRow, 3, 4) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow] ) ;

				    return xlExit( c_numSummaryRows, 4, SummaryData ) ;		// OK Return - output the array
                }
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID, bad revenue base or bad Total Flow

}
//
//.............................................................................
//
/*
**			Function to return the stream summary and audit check data 
**			for units with 3 products.
**			
**			Data calculated (rows) are:
**                - see StreamSummaryCalculations1
**
**			For each column, Feed, Product1, Product2, Product3, AuditCheck.
**			
*/
C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummaryCalculations3( LPXLOPER lpxConfigID, 
									 						  LPXLOPER lpxRevenueBase,
									 						  LPXLOPER lpxCaratsBase,
		 													  LPXLOPER lpxLimnFFFF,
 															  LPXLOPER lpxLimnPPP1,
 														      LPXLOPER lpxLimnPPP2,
														      LPXLOPER lpxLimnPPP3 )
{   

    if ( !parameterCheckOK( lpxConfigID, lpxRevenueBase, lpxCaratsBase, lpxLimnFFFF, lpxLimnPPP1, lpxLimnPPP2, lpxLimnPPP3) ) return xlExit ( xlerrRef ) ;	// error return, bad parameter

    double* SummaryData = &g->OutputBuffer[0] ;

	double revenueBase ;
	double caratsBase ;

	int xlStatus ;

	int iRow ;

	int configID ;
	int retCode = CoerceReference( lpxConfigID, configID ) ;
	if ( retCode == xlerrNull )
	{
		CDiamondWizardConfiguration* thisConfig  = g->DiamondWizardConfigurations.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
													// bring in the base revenue value
			retCode = CoerceReference( lpxRevenueBase, revenueBase ) ;
			if ( retCode == xlerrNull )
			{
    													// bring in the base carat value
	    		retCode = CoerceReference( lpxCaratsBase, caratsBase ) ;
			    if ( retCode == xlerrNull )
			    {
														    // do each stream's data in turn
				    double SummaryFFFF[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryFFFF, lpxLimnFFFF, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    double SummaryPPP1[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryPPP1, lpxLimnPPP1, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    double SummaryPPP2[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryPPP2, lpxLimnPPP2, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    double SummaryPPP3[c_numSummaryRows] ;
				    xlStatus = StreamSummaryCalculationFromExcel( SummaryPPP3, lpxLimnPPP3, revenueBase, caratsBase, thisConfig ) ;
				    if ( xlStatus != xlerrNull ) return xlExit( xlStatus ) ;

				    for ( iRow = 0 ; iRow < c_numSummaryRows ; iRow++ )
				    {
					    SummaryData[ xlIndex(iRow, 0, 5) ] = SummaryFFFF[iRow] ;
					    SummaryData[ xlIndex(iRow, 1, 5) ] = SummaryPPP1[iRow] ;
					    SummaryData[ xlIndex(iRow, 2, 5) ] = SummaryPPP2[iRow] ;
					    SummaryData[ xlIndex(iRow, 3, 5) ] = SummaryPPP3[iRow] ;
					    SummaryData[ xlIndex(iRow, 4, 5) ] = 0.0 ;
				    }
				    ///	Audit those things that do add up!!
				    iRow = 0 ;				// Ore Solids 
				    SummaryData[ xlIndex(iRow, 4, 5) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow], SummaryPPP3[iRow] ) ;
				    iRow = 6 ;				// Water 
				    SummaryData[ xlIndex(iRow, 4, 5) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow], SummaryPPP3[iRow] ) ;
				    iRow = 9 ;				// FeSi 
				    SummaryData[ xlIndex(iRow, 4, 5) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow], SummaryPPP3[iRow] ) ;
				    iRow = 10 ;				// DiamondFlow 
				    SummaryData[ xlIndex(iRow, 4, 5) ] = AuditCheck(SummaryFFFF[iRow], SummaryPPP1[iRow], SummaryPPP2[iRow], SummaryPPP3[iRow] ) ;

				    return xlExit( c_numSummaryRows, 5, SummaryData ) ;		// OK Return - output the array
                }
			}
		}
	}

	return xlExit ( xlerrRef ) ;	// error return, bad configuration ID, bad revenue base or bad Total Flow

}
//
//
//
int StreamSummaryCalculationFromExcel( double SummaryXXXX[], 
                                       LPXLOPER lpxLimnXXXX, 
                                       double revenueBase, 
                                       double caratsBase, 
                                       CDiamondWizardConfiguration* thisConfig )
{

	int idx ;							// Counter

	int nComponents ;					// total number of components in stream matrix


	if (thisConfig == NULL) return xlerrRef ;

	int nDSz = thisConfig->nDiamondSizes() ;							
	int nOSz = thisConfig->nOreSizes() ;
	int nSG  = thisConfig->nSGs() ;

	// Bring in the stream data
	g->Excel.fn( xlCoerce, xlp( lpxLimnXXXX ), xl(xltypeMulti) ) ;
											// validity checks
	if ( g->Excel.resultColumns() != nSG + nDSz * int((nSG + 1) / 2) ) return xlerrRef ;
	if ( g->Excel.resultRows()    != (2 * nOSz) ) return xlerrRef ;

	nComponents = g->Excel.resultColumns() * g->Excel.resultRows() ;

	double* pData = new double[nComponents] ;

	for ( idx = 0 ; idx < nComponents ; idx++ )							//  Load up spreadsheet data into a local array
	{
		pData[idx] = g->Excel.resultArrayNum(idx) ;
	}

	int iRet = StreamSummaryCalculation( SummaryXXXX, pData, revenueBase, caratsBase, thisConfig ) ;
	delete pData ;

	return iRet ;

}
//
//.............................................................................
//
/*
**			Function to do the audit check (Polymorphic)
**	
*/
double AuditCheck( double FFFF, double PPP1, double PPP2, double PPP3)
{
	double theCheck = FFFF - PPP1 - PPP2 - PPP3 ;
	if ( fabs(theCheck) < 0.00000000001 ) theCheck = 0.0 ;
	return theCheck ;
}
double AuditCheck( double FFFF, double PPP1, double PPP2)
{
	double theCheck = FFFF - PPP1 - PPP2 ;
	if ( fabs(theCheck) < 0.00000000001 ) theCheck = 0.0 ;
	return theCheck ;
}
double AuditCheck( double FFFF, double PPP1)
{
	double theCheck = FFFF - PPP1 ;
	if ( fabs(theCheck) < 0.00000000001 ) theCheck = 0.0 ;
	return theCheck ;
}
//
//
//*******************************************************************************
//
//
//      Local Helper routines to handle Excel parameter passing etc.
//
//
//
//          polymorphic functions to handle coercion of references
//
int CoerceReference( LPXLOPER excelRef, CString& theCString )
{
    g->Excel.fn( xlCoerce, xlp(excelRef), xl(xltypeStr) ) ;
    if (g->Excel.resultXltype() != xltypeStr) return xlerrName ;   // error return
    theCString = g->Excel.resultCString() ;                  
    return xlerrNull ;                      // OK return
}
//
int CoerceReference( LPXLOPER excelRef, int& theInt )
{
    g->Excel.fn( xlCoerce, xlp(excelRef), xl(xltypeNum) ) ;
    if (g->Excel.resultXltype() != xltypeNum) return xlerrRef ;    // error return
    theInt = int(g->Excel.resultNum()) ; 
    return xlerrNull ;                      // OK return
}
//
int CoerceReference( LPXLOPER excelRef, double& theDouble )
{
    g->Excel.fn( xlCoerce, xlp(excelRef), xl(xltypeNum) ) ;
    if (g->Excel.resultXltype() != xltypeNum) return xlerrRef ;    // error return
    theDouble = g->Excel.resultNum() ; 
    return xlerrNull ;                      // OK return
}
//
//
//
//          polymorphic functions to check references
//
//
bool parameterCheckOK( LPXLOPER lpxParam1, 
                       LPXLOPER lpxParam2, 
                       LPXLOPER lpxParam3,
                       LPXLOPER lpxParam4,
                       LPXLOPER lpxParam5,
                       LPXLOPER lpxParam6,
                       LPXLOPER lpxParam7 )
{
    if ( (lpxParam1->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam2->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam3->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam4->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam5->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam6->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam7->xltype & xltypeValidReference) == 0 ) return false ;
    return true ;
}
//
bool parameterCheckOK( LPXLOPER lpxParam1, 
                       LPXLOPER lpxParam2, 
                       LPXLOPER lpxParam3,
                       LPXLOPER lpxParam4,
                       LPXLOPER lpxParam5,
                       LPXLOPER lpxParam6 )
{
    if ( (lpxParam1->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam2->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam3->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam4->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam5->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam6->xltype & xltypeValidReference) == 0 ) return false ;
    return true ;
}
//
bool parameterCheckOK( LPXLOPER lpxParam1, 
                       LPXLOPER lpxParam2, 
                       LPXLOPER lpxParam3,
                       LPXLOPER lpxParam4,
                       LPXLOPER lpxParam5 )
{
    if ( (lpxParam1->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam2->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam3->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam4->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam5->xltype & xltypeValidReference) == 0 ) return false ;
    return true ;
}
//
bool parameterCheckOK( LPXLOPER lpxParam1, 
                       LPXLOPER lpxParam2, 
                       LPXLOPER lpxParam3,
                       LPXLOPER lpxParam4 )
{
    if ( (lpxParam1->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam2->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam3->xltype & xltypeValidReference) == 0 ) return false ;
    if ( (lpxParam4->xltype & xltypeValidReference) == 0 ) return false ;
    return true ;
}

#endif
//.............................................................................
//
/*
**			Local calculation function
**			Rosin-Rammler interpolation to find size of a passing percent
**			NB. Works on a percent retained distribution, and requires
**				the DiamondWizard Configuration sheet data for sizes.
*/
double CalculatePassingSize( double Percent, double* OreDistribution, int nOSz, CDiamondWizardConfiguration* thisConfig  )
{
    int iOSz ;

	double theSize ;

	double Fraction ;            // the passing fraction
	double x1       ;            // log ( top size of a size interval )
	double x2       ;            // log ( bottom size of a size interval )
	double y1       ;            // Rosin-Rammler top of range
	double y2       ;            // Rosin-Rammler bottom of range
	double Slope    ;            // slope of RR line
	double Upper    ;            // upper range value (for clipping)
	double Lower    ;            // lower range value (for clipping)

	double CumData_1 ;           // Cumulative retained data
	double CumData_2 ;

  double UpperLimit = thisConfig->UseLimnScaling() ? 99.9999999999 : 0.999999999999;
  double LowerLimit = thisConfig->UseLimnScaling() ?  0.0000000001 : 0.000000000001;
  double Scaling    = thisConfig->UseLimnScaling() ?  100.0        : 1.0           ;

  CumData_2 = Scaling - OreDistribution[0] ;

	for ( iOSz = 0 ; iOSz < nOSz-1 ; iOSz++ )
	{
		CumData_1 = CumData_2 ;
		CumData_2 = CumData_1 - OreDistribution[iOSz + 1] ;
		if ((CumData_1 >= Percent) && (CumData_2 < Percent))
		{
			x1 = log(thisConfig->MeanOreSize(iOSz)) ;
			x2 = log(thisConfig->MeanOreSize(iOSz+1)) ;
			Upper = CumData_1 ;
			if ( Upper > UpperLimit ) Upper = UpperLimit ;     // clip to less than 100
			Lower = CumData_2 ;
			if ( Lower < LowerLimit ) Lower = LowerLimit ;       // and greater than 0
			y1 = log(-log(Upper / Scaling)) ;
			y2 = log(-log(Lower / Scaling)) ;
			Slope = (y2 - y1) / (x2 - x1) ;
			Fraction = log(-log(Percent / Scaling)) ;
			theSize = exp(x2 + (Fraction - y2) / Slope) ;
			return theSize ;							// OK Return
		}
	}
	if (Percent > CumData_1) 
	{
		theSize = thisConfig->MeanOreSize(0) ;
	}
	else
	{
		theSize = 0.0 ;
	}

	return theSize ;

}
//
//
//
//
//*******************************************************************************
//
//
//      Local common calculation routines
//
//
//.............................................................................
//
/*
**			Function to return the stream summary data 
**			
**			Data calculated (rows) are:
**                Ore Solids Flow (tph) 
**                Passing size - 25%  
**                Passing size - 50%  
**                Passing size - 75%  
**                Mean SG (t/m3) 
**                Mean Size (mm) 
**                Water Flow (tph) 
**                Percent Solids  
**                Pulp SG (t/m3) 
**                FeSi Flow (tph) 
**                Carats/hr  
**                Carats/100Tonne
**                Liberated Carats/hr
**                Relative Carats
**                % Liberated  
**                Relative Revenue  
**                Rand / hr
**
**
**			Broken into two parts, one that can be called from Excel as a precursor to load data, and the one
**			that does the actual calcualtions which does not require any Excel resources...
*/
//
int StreamSummaryCalculation( double SummaryXXXX[], 
                              double* pData, 
                              double revenueBase, 
                              double caratsBase, 
                              CDiamondWizardConfiguration* thisConfig )
{

	int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;
	int idx  ;

	int nDSz ;							// Limits
    int nOSz ;							
    int nSG  ;

	double sizeFractionFlows[100] ;
	double OreDistribution[100] ;
	
	double theData ;
	double totalSolids = 0.0 ;
	double totalWater = 0.0 ;
	double solidsVolume_Sum = 0.0 ;
	double meanSize_Sum = 0.0 ;

	if (thisConfig == NULL) return xlerrRef ;

	nDSz = thisConfig->nDiamondSizes() ;							
	nOSz = thisConfig->nOreSizes() ;
	nSG  = thisConfig->nSGs() ;

//      ORE Summary Calculations

	for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
	{
		sizeFractionFlows[iOSz] = 0.0 ;
		for ( iSG = 0 ; iSG < nSG ; iSG++ )
		{
			idx = thisConfig->iODLimnStreamIndex ( iOSz, iSG ) ;
			theData = pData[idx] ;
			totalSolids  += theData ;
			solidsVolume_Sum   += theData / thisConfig->MeanSG(iSG) ;
			meanSize_Sum += theData * thisConfig->MeanOreSize(iOSz) ;
			sizeFractionFlows[iOSz] += theData ;
		}
	}

//-------------------- Solids Flow

	SummaryXXXX[0] = totalSolids;

//-------------------- Passing Sizes

	if ( totalSolids > 0.0 )
	{
		for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
		{
			if ( sizeFractionFlows[iOSz] < 0.0000001 )  sizeFractionFlows[iOSz] = 0.0 ;

			OreDistribution[iOSz] = 100.0 * sizeFractionFlows[iOSz] / totalSolids ;
		}
		SummaryXXXX[1] = CalculatePassingSize( thisConfig->PassSizePercents(0), OreDistribution, nOSz, thisConfig ) ;
		SummaryXXXX[2] = CalculatePassingSize( thisConfig->PassSizePercents(1), OreDistribution, nOSz, thisConfig ) ;
		SummaryXXXX[3] = CalculatePassingSize( thisConfig->PassSizePercents(2), OreDistribution, nOSz, thisConfig ) ;
	}
	else
	{
		SummaryXXXX[1] = 0.0 ;
		SummaryXXXX[2] = 0.0 ;
		SummaryXXXX[3] = 0.0 ;
	}

//-------------------- Mean Ore Size and SG

	if ( totalSolids > 0.0 )
	{
		SummaryXXXX[4] = totalSolids / solidsVolume_Sum ;
		SummaryXXXX[5] = meanSize_Sum / totalSolids ;
	}
	else
	{
		SummaryXXXX[4] = 0.0 ;
		SummaryXXXX[5] = 0.0 ;
	}

//--------------------	Water flow

	idx = thisConfig->iWaterLimnStreamIndex() ;
	totalWater = pData[idx] ;	
	SummaryXXXX[6] = totalWater ;                                                                               

//-------------------- Percent Solids

	if ( (totalSolids + totalWater) > 0.0 )
	{
		SummaryXXXX[7] = 100 * totalSolids / (totalSolids + totalWater) ;	
	}
	else
	{
		SummaryXXXX[7] = 0.0 ;
	}

//-------------------- Pulp SG

	if ( (totalSolids > 0.0) && (SummaryXXXX[4] > 0.0) )
	{
		SummaryXXXX[8] = (totalSolids + totalWater) / (solidsVolume_Sum + totalWater) ;	
	}
	else
	{
		SummaryXXXX[8] = 0.0 ;
	}

//-------------------- FeSi flow

	idx = thisConfig->iFeSiLimnStreamIndex() ;
	SummaryXXXX[9] = pData[idx] ;


//      DIAMOND Summary Calculations

    double DiamondMassLiberated = 0.0 ;
	double totalDiamondMass = 0.0 ;
	double thisDiamondMass = 0.0 ;
    double DiamondRevenue = 0.0 ;
						
	for ( iSG = 0 ; iSG < nSG ; iSG++ )
	{
		for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
		{
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				idx = thisConfig->iDDLimnStreamIndex( iDSz, iOSz, iSG ) ;
				thisDiamondMass = pData[idx] ;								// mass of diamonds in fraction

				if ( thisConfig->IsLiberatedDiamond( iDSz, iOSz ) )			// check the diamond against all ore sizes
				{															// to see if it is liberated
		            DiamondMassLiberated += thisDiamondMass ;				// accumulate liberated diamond mass
				}
				totalDiamondMass += thisDiamondMass ;						// accumulate total diamond mass

				DiamondRevenue += ( thisConfig->Revenue(iDSz) * thisDiamondMass ) ;   // accumulate the revenue the data for each element
			}
		}
	}


//-------------------- Diamond Flow Units  

		SummaryXXXX[10] = totalDiamondMass ;

//-------------------- Diamond Grade cpht  

	if (totalSolids > 0)
	{
		SummaryXXXX[11] = 100.0 * totalDiamondMass / totalSolids ;
	}
	else
	{
		SummaryXXXX[11] = 0.0 ;
	}


//-------------------- Liberated Flow Units  

		SummaryXXXX[12] = DiamondMassLiberated ;


//-------------------- Relative Carats

	if (caratsBase > 0)
	{
		SummaryXXXX[13] = 100.0 * (totalDiamondMass / caratsBase) ;
	}
	else
	{
		SummaryXXXX[13] = 0.0 ;
	}
	

//-------------------- % Liberated

	if ((DiamondMassLiberated > 0) && (totalDiamondMass > 0))
	{
		SummaryXXXX[14] = 100.0 * DiamondMassLiberated / totalDiamondMass ;
	}
	else
	{
		SummaryXXXX[14] = 0.0 ;
	}

//-------------------- Relative Revenue Units

	if (revenueBase > 0)
	{
		SummaryXXXX[15] = 100.0 * (DiamondRevenue / revenueBase) ;
	}
	else
	{
		SummaryXXXX[15] = 0.0 ;
	}
	

//-------------------- Revenue per hour

	if (totalDiamondMass > 0)
	{
		SummaryXXXX[16] = DiamondRevenue ;
	}
	else
	{
		SummaryXXXX[16] = 0.0 ;
	}
	
	return xlerrNull ;
}

void OreMassFromDensimetricDistribution( CDiamondWizardConfiguration* thisConfig, double* densimetricDistribution, double* sizeFractionData, double totalFlow, double* oreMass )
{
    int iOSz ;							// Counters
    int iSG  ;
	int idx  ;

	int nOSz = thisConfig->nOreSizes() ;
	int nSG  = thisConfig->nSGs() ;

	double SizeFractionFlow ;
  double Scaling = (thisConfig->UseLimnScaling() ? 10000.0 : 1.0);

	for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
	{
  SizeFractionFlow = totalFlow * sizeFractionData[ iOSz ] / Scaling;

		for ( iSG = 0 ; iSG < nSG ; iSG++ )
		{
			idx = xlIndex( iOSz, iSG, nSG ) ;
			//CNM if (g->Excel.resultArrayXltype(idx) == xltypeNum )
			{
				oreMass[ idx ] = SizeFractionFlow * densimetricDistribution[idx] ;
      }
		}
	}
}

void DiamondDeportmentFromSGDistribution( CDiamondWizardConfiguration* thisConfig,
										  double* OreDistribution, 
										   double* OreSizeDistribution, 
										    double* DmdSGAllocation, 
											 double* DmdSizeDistribution, 
											  double DiamondFlowBasis,
											   double* DiamondDeportment)
{
	double thisDmdFlow ;
	double totalDmdFlow ;

	int iDSz ;							// Counters
    int iOSz ;
    int iSG  ;
	int idx  ;

	int nDSz ;							// Limits
    int nOSz ;							
    int nSG  ;

    nDSz = thisConfig->nDiamondSizes() ;							
	nOSz = thisConfig->nOreSizes() ;
	nSG  = thisConfig->nSGs() ;

	totalDmdFlow = 0.0 ;

	for ( iSG = 0 ; iSG < nSG ; iSG++ )
	{
		for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
		{
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				thisDmdFlow = DmdSizeDistribution[iDSz] * DmdSGAllocation[iSG] 
														  * OreDistribution[ xlIndex( iOSz, iSG, nSG ) ]
															* OreSizeDistribution[iOSz] ; 
				
				DiamondDeportment[ thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ] =  thisDmdFlow ;

				totalDmdFlow += thisDmdFlow ;

			}
		}
	}
	if ( nSG % 2 != 0 )									// left over bits at end of deportment array (odd number of SGs)
	{
		iSG = nSG ;
		for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
		{
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				DiamondDeportment[ thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ] = 0.00 ;
			}
		}
	}

  //CNM
  if (totalDmdFlow<1e-30)
    totalDmdFlow=1e-30;

	for ( iSG = 0 ; iSG < nSG ; iSG++ )					// normalise to total diamond flow units
	{
		for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
		{
			for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			{
				idx = thisConfig->iDDIndex ( iDSz, iOSz, iSG ) ; 

				DiamondDeportment[ idx ] =  DiamondDeportment[ idx ] *  DiamondFlowBasis / totalDmdFlow ;
			}
		}
	}
}
//*******************************************************************************
//
