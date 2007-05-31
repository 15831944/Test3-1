////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
#include "LimnModel_DiamondWizard_WhitenCrusher.h"

CLimn_ModelData_Common C_ModelParameters_DiamondWizard_WhitenCrusher::sm_Common;
CLimn_ModelData_Common C_ModelReturns_DiamondWizard_WhitenCrusher::sm_Common;

//////     
//
//----------------------------------------------------------------------------------------------
//
//
//		Diamond Wizard - Whiten Crusher Model
//
//								Parameters: 
//									0. Flag to indicate whether Redistribution of Densimetrics is required
//									1. Crusher CSS
//									2-5. K1 parameters a0 - a3
//									6-9. K2 parameters a0 - a3
//									10.  K3 parameter  a0
//									11-14. T10 parameters a0 - a3
//									15-39. Array of Tn v T10 (Spreadsheet vector form - i.e. Row Major order
//
//								Returned Data: 
//									0. TPH Feedrate
//									1. Feed P80 (F80)
//
//
int _Model_DiamondWizard_WhitenCrusher (int nRows,
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
	if ( nParameters != 40 )                             
	{                                                   
		return 1 ;                                      // 'Incorrect number of parameters'
	}        

	int configID = CONFIGURATION.ID() ;
	CDiamondWizardConfiguration* thisConfig  = CONFIGURATIONS.Configuration( configID ) ;	// Find configuration worksheet
	if ( thisConfig != NULL )
	{
		int nDSz = thisConfig->nDiamondSizes() ;			// Limits
		int nOSz = thisConfig->nOreSizes() ;
		int nSG  = thisConfig->nSGs() ;

		C_ModelParameters_DiamondWizard_WhitenCrusher p( ModelParams ) ;		// model parameters - copied to class variables for clarity
		C_ModelReturns_DiamondWizard_WhitenCrusher    r( ModelReturn ) ;		// model returned data


		int iOSz ;							// Counters
		int j ;
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
			double FeedP80 = CalculatePassingSize( 80, meanSizeDistribution.Store(), nOSz, thisConfig ) ;

			double K1 = p.K1_a(0) + (p.K1_a(1) * p.CSS()) + (p.K1_a(2) * FeedTPH) + (p.K1_a(3) * FeedP80) ;
			double K2 = p.K2_a(0) + (p.K2_a(1) * p.CSS()) + (p.K2_a(2) * FeedTPH) + (p.K2_a(3) * FeedP80) ;
			double K3 = p.K3_a() ;

			DiagonalMatrix C(nOSz) ;
			C = 0.0 ;

			double* Size = thisConfig->pMeanOreSize() ;
			WhitenCrusherClassificationFunction( nOSz, Size, K1, K2, K3, C) ;

			double requiredT10 = p.T10_a(0) + (p.T10_a(1) * p.CSS()) + (p.T10_a(2) * FeedTPH) + (p.T10_a(3) * FeedP80) ;

			Matrix Tn_V_T10(5,5) ;
			//Tn_V_T10 << p.pTn_V_T10() ;
			Tn_V_T10 << p.Tn_V_T10.Ptr() ;

			LowerTriangularMatrix B(nOSz) ; 
			B = 0.0 ;

			WhitenCrusherBreakageFunction( nOSz, Size, requiredT10, Tn_V_T10, B) ;

			Matrix X(nOSz, nOSz) ;
			X = 0.0 ;

			WhitenCrusherTransformationMatrix( B, C, X ) ;

			CLimnOreStreamMatrix product( thisConfig ) ;

			for ( iSG = 1 ; iSG <= nSG ; iSG++ )
			{
				product.Column(iSG) = X * feed.Column(iSG) ;							// matrix math p = x.f

				double SGSum =  product.Column(iSG).Sum() ;									// submesh by difference
				double SGSumError = 100 - SGSum ;
				double subMesh = product(nOSz,iSG) ;
				subMesh += SGSumError ;
				product(nOSz,iSG) = subMesh ;
			}

			if ( p.redistributeDensimetrics() )
			{
				RedistributeOreDensimetricToFeedDistribution( thisConfig, FeedTPH, product, meanSizeDistribution, meanSGDistribution ) ;
			}

			product.ToAbsoluteMassForm( FeedTPH, meanSGDistribution ) ;

			product.SaveToLimnStream( Product1 ) ;

		
	//      DIAMOND Calculations

			int iDDindex ;
			int jDDindex ;

			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
				{
					double productSum = 0.0 ;
					double feedSum = 0.0 ;
	
					for ( j = 0 ; j < (nOSz-1) ; j++ )
					{
						jDDindex = thisConfig->iDDLimnStreamIndex(iDSz,j,iSG) ;
						Product1[jDDindex] = 0.0 ;
		
						for ( iOSz = 0 ; iOSz <= j ; iOSz++ )
						{
							iDDindex = thisConfig->iDDLimnStreamIndex(iDSz,iOSz,iSG) ;
							Product1[jDDindex] += (CombinedFeed[iDDindex] * X(j+1,iOSz+1) ) ;
						}

						productSum += Product1[jDDindex] ;
						feedSum += CombinedFeed[jDDindex] ;
					}
													// Calculate finest fraction by difference
					jDDindex = thisConfig->iDDLimnStreamIndex(iDSz,nOSz-1,iSG) ;
					feedSum += CombinedFeed[jDDindex] ;
					Product1[jDDindex] = feedSum - productSum ;
				}
			}


		///
		///		Then Water and FeSi splits
		///
			idx = thisConfig->iWaterLimnStreamIndex() ;
			Product1[idx] = CombinedFeed[idx] ;

			idx = thisConfig->iFeSiLimnStreamIndex() ;
			Product1[idx] = CombinedFeed[idx] ;
			
			r.FeedP80() = FeedP80 ;
			r.FeedTPH() = FeedTPH ;					// put model extra calculations into the Return Data buffer
		}
    else
    {
      // Pass 'Zero' Feed Straight Thru
	
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
		    for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
			  {
		      int iODindex=thisConfig->iODLimnStreamIndex(iOSz, iSG);
				  Product1[iODindex] = CombinedFeed[iODindex];	
	          
          for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			    {
					  int iDDindex = thisConfig->iDDLimnStreamIndex(iDSz,iOSz,iSG) ;
					  Product1[iDDindex] = CombinedFeed[iDDindex];	
				  }
        }
      }
			
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
//////
/*C_LINKAGE DLLMODEL */int  _Error_DiamondWizard_WhitenCrusher ( int errorIndex, 
															 LPSTR errorBuffer ) 
{
	return TRUE ;
}

/*C_LINKAGE DLLMODEL */int  _Parameter_DiamondWizard_WhitenCrusher ( int parameterIndex, 
																 LPSTR parameterInfoBuffer ) 
{
	return TRUE ;
}
//
//
