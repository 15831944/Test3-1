////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
#include "LimnModel_DiamondWizard_EfficiencyCurve.h"


CLimn_ModelData_Common C_ModelParameters_DiamondWizard_EfficiencyCurve::sm_Common;
//////     
//
//----------------------------------------------------------------------------------------------
//
//
//		Diamond Wizard - Efficiency Curve model
//
//								Parameters: 
//										0. A flag to identify the Oversize Stream (1 or 2)
//                                      1. The water split fraction to Oversize
//                                      2. The FeSi split fraction to Oversize
//										4. The efficiency curve (to Oversize) Alpha value
//										5. The efficiency curve (to Oversize) Rf value
//										6 to 5+nSGs. A vector of efficiency curve (to Oversize) d50c values (one for each Ore SG)
//										6+nSGs. The efficiency curve (to Oversize) d50c value for diamond
//
//
int _Model_DiamondWizard_EfficiencyCurve (int nRows,
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

		C_ModelParameters_DiamondWizard_EfficiencyCurve p( ModelParams, nSG ) ;		// model parameters - copied to class variables for clarity


		int iDSz ;							// Counters
		int iOSz ;
		int iSG  ;

		int idx = 0 ;						// index into vector of stream data

		double partitionFraction ;


		if ( nParameters != (6 + nSG) )                             
		{                                                   
			return 1 ;                                      // 'Incorrect number of parameters'
		}        
	            
	///
	///     Efficiency Curve size separator models
	///
	///     Apply to Ore first
	///

		for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
		{
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				idx = thisConfig->iODLimnStreamIndex ( iOSz, iSG ) ;

				partitionFraction = MeanEfficiencyPartitionFraction( p.Rf(), p.Alpha(), p.D50c(iSG), thisConfig->OreSizesTop(iOSz), 
					                                                                             thisConfig->OreSizesBottom(iOSz) ) ;
	            
				if ( p.Product1IsOversize() ) partitionFraction = 1 - partitionFraction ;
				
				Product1[idx] = partitionFraction * CombinedFeed[idx] ;
				Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    
			}
		}

	///
	///		Then apply to diamonds
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
						partitionFraction = MeanEfficiencyPartitionFraction(p.Rf(), p.Alpha(), p.DiamondD50c(), thisConfig->DiamondSizesTop(iDSz), 
							                                                                              thisConfig->DiamondSizesBottom(iDSz)) ;
					}                
					else
					{
						partitionFraction = MeanEfficiencyPartitionFraction(p.Rf(), p.Alpha(), p.D50c(iSG), thisConfig->OreSizesTop(iOSz),
							                                                                            thisConfig->OreSizesBottom(iOSz)) ;
					}
					if ( p.Product1IsOversize() ) partitionFraction = 1 - partitionFraction ;
					
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
		if ( !p.Product1IsOversize() ) partitionFraction = 1 - partitionFraction ;
		Product1[idx] = partitionFraction * CombinedFeed[idx] ;
		Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    

		idx = thisConfig->iFeSiLimnStreamIndex() ;
		partitionFraction = p.FeSiSplit() ;
		if ( !p.Product1IsOversize() ) partitionFraction = 1 - partitionFraction ;
		Product1[idx] = partitionFraction * CombinedFeed[idx] ;
		Product2[idx] = CombinedFeed[idx] - Product1[idx] ;    

 		return 0 ;
	}
	else
	{
		return 3 ;			// Error: "Could  not access global configuration data"
	}
}
//////
int  _Error_DiamondWizard_EfficiencyCurve ( int errorIndex, LPSTR errorBuffer ) 
{
	return TRUE ;
}

int  _Parameter_DiamondWizard_EfficiencyCurve ( int parameterIndex, LPSTR parameterInfoBuffer ) 
{
	return TRUE ;
}

