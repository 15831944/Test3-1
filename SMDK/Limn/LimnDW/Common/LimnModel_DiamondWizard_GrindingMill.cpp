////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
#include "LimnModel_DiamondWizard_GrindingMill.h"
//////     
//
//----------------------------------------------------------------------------------------------
//
//
//		Diamond Wizard - Grinding Mill Model
//
//								Parameters: 
//									0. Flag to indicate whether Redistribution of Densimetrics is required
//									nOreSizes. Residence Time - tau
//									(2 * nOreSizes) to (3 * nOreSizes - 1). Selection Function - S
//									(4 * nOreSizes) to (((4+nOreSizes) * nOreSizes) - 1). Breakage Function B
//
//								NB: 
//									There are gaps in the above sequence to allow use of a rectangular structure
//									for the Breakage Function on the model worksheet
//
//
int _Model_DiamondWizard_GrindingMill (int nRows,
					 		 		   int nColumns,
                                       int nParameters,
                                       int nReturns,
                                       double* ModelParams,
                                       double* ModelReturn,
                                       double* CombinedFeed,
                                       double* Product1,
                                       double* Product2,
                                       double* Product3,
                                       double* Product4,
                                       double* Product5,
                                       double* Product6 ) 
{
	try
	{
		int configID = CONFIGURATION.ID() ;
		CDiamondWizardConfiguration* thisConfig  = CONFIGURATIONS.Configuration( configID ) ;	// Find configuration worksheet
		if ( thisConfig != NULL )
		{
			int nDSz = thisConfig->nDiamondSizes() ;			// Limits
			int nOSz = thisConfig->nOreSizes() ;
			int nSG  = thisConfig->nSGs() ;

			if ( nParameters != (nOSz * (3+nOSz)) )                             
			{                                                   
				return 1 ;                                      // 'Incorrect number of parameters'
			}        

			C_ModelParameters_DiamondWizard_GrindingMill p( ModelParams, nOSz ) ;	// model parameters - copied to class variables for clarity

			Matrix B(nOSz,nOSz) ;
			B << p.pB() ;					// fill a local matrix with the Breakage function data B

			ColumnVector S(nOSz) ;
			S << p.pS() ;					// ditto the Selection function S

			int iOSz ;							// Counters
			int jOSz ;
			int iDSz ;
			int iSG  ;
			int idx  ;
			
		//      ORE Calculations

			CLimnOreStreamMatrix feed( thisConfig ) ;
			feed.LoadFromLimnStream( CombinedFeed ) ;

			ColumnVector meanSGDistribution( nSG ) ;
			ColumnVector meanSizeDistribution( nOSz ) ;

			double FeedTPH = feed.ToSizeDistributionForm( meanSizeDistribution, meanSGDistribution) ;

			if ( FeedTPH > 0.0 )
			{

				ColumnVector TauS( nOSz ) ;
				ColumnVector PS( nOSz ) ;
				
				for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
				{
					TauS(iOSz) = 1 + p.tau() * S(iOSz) ;
				}

				CLimnOreStreamMatrix product( thisConfig ) ;
				product = 0.0 ;

				double totalProduct = 0.0 ;

				for ( iSG = 1 ; iSG <= nSG ; iSG++ )
				{
					product(1, iSG) = (feed(1,iSG) / 100.) / TauS(1) ;
					PS(1) = product(1,iSG) * S(1) ;
					totalProduct = product(1,iSG) ;

					for ( iOSz = 2 ; iOSz < nOSz ; iOSz++ )           // lower triangular breakage matrix
					{
						double flow = product(iOSz,iSG) ;
						for ( jOSz = 1 ; jOSz < iOSz ; jOSz++ )
						{
							flow += PS(jOSz) * B(iOSz, jOSz) ;
						}
						flow = ((feed(iOSz, iSG) / 100.0) + p.tau() * flow) / TauS(iOSz) ;
						PS(iOSz) = flow * S(iOSz) ;
						totalProduct += flow ;
						product(iOSz,iSG) = flow ;
					}

					product(nOSz, iSG) = 1 - totalProduct ;

				}

				product = 100.0 * product ;

				for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
				{
					double sizeFractionFlow = 0.0 ;
					for ( iSG = 1 ; iSG <= nSG ; iSG++ )
					{
						sizeFractionFlow += FeedTPH * product(iOSz, iSG) * meanSGDistribution(iSG) / 10000.0 ;		// density distribution is unchanged
					}
					if ( sizeFractionFlow < 0.0000001 ) sizeFractionFlow = 0.0 ;
					meanSizeDistribution(iOSz) = 100.0 * sizeFractionFlow / FeedTPH ;
				}
		

				if ( p.DoRedistributeDensimetrics() )
				{
					RedistributeOreDensimetricToFeedDistribution( thisConfig, FeedTPH, product, meanSizeDistribution, meanSGDistribution ) ;
				}

				product.ToAbsoluteMassForm( FeedTPH, meanSGDistribution ) ;

				product.SaveToLimnStream( Product1 ) ;

			
		//      DIAMOND Calculations
		//
		//				 See Kurt Petersen's workbook
		//				 "comminution solution update.xls" 26Feb2002
		//				 for the methodology!

				int iDDindex ;
				int jDDindex ;

				Matrix brokenOut(nOSz, nOSz) ;
				ColumnVector totalBroken(nOSz) ;
				ColumnVector brokenSum(nOSz) ;
				double brokenIn ;
																				// retrieve the calculated Ore Product information
				CLimnOreStreamMatrix oreProduct( thisConfig ) ;
				oreProduct.LoadFromLimnStream( Product1 ) ;			
				ColumnVector meanSGDistribution_OreProduct( nSG ) ;
				ColumnVector meanSizeDistribution_OreProduct( nOSz ) ;

				double OreProductTPH = oreProduct.ToSizeDistributionForm( meanSizeDistribution_OreProduct, meanSGDistribution_OreProduct) ;


				for ( iSG = 1 ; iSG <= nSG ; iSG++ )
				{
					for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
					{													// Modified model 27Feb02
						totalBroken(iOSz) = feed(iOSz, iSG) * (1 - (1 / (1 + p.tau() * S(iOSz)))) ;   // calculate the total broken
					}

					brokenOut = 0.0 ;								// initialise the matrix
	            
					totalBroken(nOSz) = 0.0 ;                        // nothing broken from lowest size!


					for ( iOSz = 2 ; iOSz < nOSz ; iOSz++ )           // distribution of material broken
					{
						for ( jOSz = 1 ; jOSz < iOSz ; jOSz++ )  // from each size fraction
						{
							brokenOut(iOSz, jOSz) = p.tau() * B(iOSz, jOSz) * S(jOSz) * oreProduct(jOSz, iSG) / (1 + (p.tau() * S(iOSz))) ;
						}
					}
																// calculate the last by difference
					for ( jOSz = 1 ; jOSz <= nOSz ; jOSz++ )
					{
						brokenSum(jOSz) = 0.0 ;
						for ( iOSz = (jOSz + 1) ; iOSz < nOSz ; iOSz++ )           // distribution of material broken
						{
							brokenSum(jOSz) += brokenOut(iOSz, jOSz) ;
						}
					}
					for ( jOSz = 1 ; jOSz < nOSz ; jOSz++ )
					{
						brokenOut(nOSz, jOSz) = totalBroken(jOSz) - brokenSum(jOSz) ;
						brokenOut(nOSz, jOSz) = brokenOut(nOSz, jOSz) ;
					}

					for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
					{
						for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
						{
							iDDindex = thisConfig->iDDLimnStreamIndex(iDSz,iOSz-1,iSG-1) ;

							if ( FeedTPH > 0.0000001 )
							{
								double flow = CombinedFeed[iDDindex] * (1 - totalBroken(iOSz) / (feed(iOSz, iSG) + 1E-31)) ;        // + 1E-31 to avoid div 0
                       			Product1[iDDindex] = flow ;
			
								brokenIn = 0.0 ;
								for ( jOSz = 1 ; jOSz < iOSz ; jOSz++ )
								{
									jDDindex = thisConfig->iDDLimnStreamIndex(iDSz,jOSz-1,iSG-1) ;
									brokenIn += (CombinedFeed[jDDindex] * (brokenOut(iOSz, jOSz) / (feed(jOSz, iSG) + 1E-31))) ;
								}

								Product1[iDDindex] += brokenIn ;
							}
							else										// Handle start up conditions
							{
								Product1[iDDindex] = 0.0 ;				// no feed, so no diamonds in the product
							}
						}
					}
				}
			///
			///		Then Water and FeSi splits
			///
				idx = thisConfig->iWaterLimnStreamIndex() ;
				Product1[idx] = CombinedFeed[idx] ;

				idx = thisConfig->iFeSiLimnStreamIndex() ;
				Product1[idx] = CombinedFeed[idx] ;
				
			}

 		return 0 ;

		}
		else
		{
			return 3 ;			// Error: "Could  not access global configuration data"
		}
	}
	catch(Exception) // catch exceptions thrown by NEWMAT
	{ 
		AfxMessageBox( Exception::what() ) ; 
		return 4 ;
	}
	catch(...)  // catch exceptions thrown by everything else
	{ 
		AfxMessageBox( "Exception caught in DiamondWizard_GrindingMill" ) ; 
		return 5 ;
	}
	return 0;

}
//////
/*C_LINKAGE DLLMODEL */int  _Error_DiamondWizard_GrindingMill ( int errorIndex, 
															 LPSTR errorBuffer ) 
{
	return TRUE ;
}

/*C_LINKAGE DLLMODEL */int  _Parameter_DiamondWizard_GrindingMill ( int parameterIndex, 
																 LPSTR parameterInfoBuffer ) 
{
	return TRUE ;
}

//////
//////
//////     
//
//
