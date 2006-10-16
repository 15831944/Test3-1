////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
#include "LimnModel_DiamondWizard_TrompCurve.h"
//////     
//
//----------------------------------------------------------------------------------------------
//
//
//		Diamond Wizard - Tromp Curve model
//
//								Parameters: 
//										0. A flag to identify the Floats product Stream (1 or 2)
//                                      1. The water split fraction to Oversize
//                                      2. The FeSi split fraction to Oversize
//										3 to 2+nOreSizes. The tromp curve Ep values for Ore
//										3+nOreSizes to 2+2*nOreSizes. The tromp curve Rho50 values for Ore
//										3+2*nOreSizes to 2 + 2*nOreSizes + nDiamondSizes. The tromp curve Ep values for Diamond
//										3 + 2*nOreSizes + nDiamondSizes to 2 + 2*nOreSizes + 2*nDiamondSizes. The tromp curve Rho50 values for Diamond
//
int _Model_DiamondWizard_TrompCurve (int nRows,
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
	int configID = CONFIGURATION.ID() ;
	CDiamondWizardConfiguration* thisConfig  = CONFIGURATIONS.Configuration( configID ) ;	// Find configuration worksheet
	if ( thisConfig != NULL )
	{
		int nDSz = thisConfig->nDiamondSizes() ;			// Limits
		int nOSz = thisConfig->nOreSizes() ;
		int nSG  = thisConfig->nSGs() ;

		C_ModelParameters_DiamondWizard_TrompCurve p( ModelParams, nOSz, nDSz ) ;		// model parameters - copied to class variables for clarity

		int iDSz ;							// Counters
		int iOSz ;
		int iSG  ;

		int idx = 0 ;						// index into vector of stream data

		double partitionFraction ;

		double lockedSG ;


		if ( nParameters != (3 + 2*nOSz + 2*nDSz) )                             
		{                                                   
			return 1 ;                                      // 'Incorrect number of parameters'
		}        
	    
	                                                        
	///
	///     Tromp Curve SG separator models
	///
	///     Apply to Ore first
	///
		for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
		{
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				idx = thisConfig->iODLimnStreamIndex ( iOSz, iSG ) ;

				partitionFraction = MeanTrompPartitionFraction( p.Ep(iOSz), p.Rho50(iOSz), thisConfig->SGsTop(iSG), 
					                                                                       thisConfig->SGsBottom(iSG) ) ;
	            
				if (!p.Product1IsFloats()) partitionFraction = 1 - partitionFraction ;
				
				Product1[idx] = partitionFraction * CombinedFeed[idx] ;
				Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    
			}
		}

	///
	///		Then apply to diamonds
	///     - handles locked diamond separation and
	///       separation of any liberated diamonds
	///       in the feed
	///
		for ( iSG = 0 ; iSG < nSG ; iSG++ )
		{
			for ( iDSz = 0 ; iDSz < nDSz ; iDSz++ )
			{
				for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
				{
					idx = thisConfig->iDDLimnStreamIndex( iDSz, iOSz, iSG ) ;
					if ( thisConfig->IsLiberatedDiamond( iDSz, iOSz ) )			// check the diamond against all ore sizes
					{															// to see if it is liberated
						lockedSG = thisConfig->DiamondSG() ;
						partitionFraction = 1 - 1 / (1 + exp(1.098 * (p.Rho50D(iDSz) - lockedSG) / p.EpD(iDSz))) ;
					}                
					else
					{
	                    lockedSG = thisConfig->LockedParticleSG(iDSz, iOSz, iSG) ;
						partitionFraction = 1 - 1 / (1 + exp(1.098 * (p.Rho50(iOSz) - lockedSG) / p.Ep(iOSz))) ;
					}
					if ( !p.Product1IsFloats() ) partitionFraction = 1 - partitionFraction ;
					
					Product1[idx] = partitionFraction * CombinedFeed[idx] ;
					Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    
				}
			}
		}


	///
	///		Then Water and FeSi splits
	///
		idx = thisConfig->iWaterLimnStreamIndex() ;
		partitionFraction = p.WaterSplit() ;
		if ( !p.Product1IsFloats() ) partitionFraction = 1 - partitionFraction ;
		Product1[idx] = partitionFraction * CombinedFeed[idx] ;
		Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    

		idx = thisConfig->iFeSiLimnStreamIndex() ;
		partitionFraction = p.FeSiSplit();
		if ( !p.Product1IsFloats() ) partitionFraction = 1 - partitionFraction ;
		Product1[idx] = partitionFraction * CombinedFeed[idx] ;
		Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    

 		return 0 ;
	}
	else
	{
		return 3 ;			// Error: "Could  not access global configuration data"
	}
}
///
///
/*C_LINKAGE DLLMODEL */int  _Error_DiamondWizard_TrompCurve( int errorIndex, LPSTR errorBuffer ) 
{
	return TRUE ;
}

/*C_LINKAGE DLLMODEL */int  _Parameter_DiamondWizard_TrompCurve( int parameterIndex,  LPSTR parameterInfoBuffer ) 
{
	return TRUE ;
}
//
//
