////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
#include "LimnModel_DiamondWizard_DualEfficiencyCurve.h"
//////     
//
//----------------------------------------------------------------------------------------------
//
//
//		Diamond Wizard - Dual Efficiency Curve model
//
//								Parameters: 
//										0. A flag to identify the Oversize Stream (1, 2 or 3)
//										1. A flag to identify the Undersize Stream (1, 2 or 3)
//                                      2. top deck  - The water split fraction to Oversize
//                                      3. top deck  -  The FeSi split fraction to Oversize
//										4. top deck  -  The efficiency curve (to Oversize) Alpha value
//										5. top deck  -  The efficiency curve (to Oversize) Rf value
//										6 to 5+nSGs.  top deck  - A vector of efficiency curve (to Oversize) d50c values (one for each Ore SG)
//										6+nSGs.  top deck  - The efficiency curve (to Oversize) d50c value for diamond
//                                      7+nSGs. bottom deck  - The water split fraction to Oversize
//                                      8+nSGs. bottom deck  -  The FeSi split fraction to Oversize
//										9+nSGs. bottom deck  -  The efficiency curve (to Oversize) Alpha value
//										10+nSGs. bottom deck  -  The efficiency curve (to Oversize) Rf value
//										11+nSGs to 10+nSGs+nSGs.  bottom deck  - A vector of efficiency curve (to Oversize) d50c values (one for each Ore SG)
//										11+nSGs+nSGs.  bottom deck  - The efficiency curve (to Oversize) d50c value for diamond
//
//
int _Model_DiamondWizard_DualEfficiencyCurve (int nRows,
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

		int nComponents = nRows * nColumns ;

		C_ModelParameters_DiamondWizard_DualEfficiencyCurve p( ModelParams, nSG ) ;		// model parameters - copied to class variables for clarity


		int iDSz ;							// Counters
		int iOSz ;
		int iSG  ;

		int idx = 0 ;						// index into vector of stream data

		double partitionFraction ;


		if ( nParameters != (12 + (2 * nSG)) )                             
		{                                                   
			return 1 ;                                      // 'Incorrect number of parameters'
		}        
	            
	///
	///     Efficiency Curve size separator models
	///
	///     Apply to Ore first
	///
		double* strmOversize  = new double[nComponents] ;
		double* strmTopDeckUS = new double[nComponents] ;
		double* strmMids      = new double[nComponents] ;
		double* strmUndersize = new double[nComponents] ;

		for ( iOSz = 0 ; iOSz < nOSz ; iOSz++ )
		{
			for ( iSG = 0 ; iSG < nSG ; iSG++ )
			{
				idx = thisConfig->iODLimnStreamIndex ( iOSz, iSG ) ;

										// top deck separation
				partitionFraction = MeanEfficiencyPartitionFraction( p.Rf_1(), p.Alpha_1(), p.D50c_1(iSG), thisConfig->OreSizesTop(iOSz), 
					                                                                                       thisConfig->OreSizesBottom(iOSz) ) ;
				partitionFraction = 1 - partitionFraction ;
				strmOversize[idx] = partitionFraction * CombinedFeed[idx] ;
				strmTopDeckUS[idx] = CombinedFeed[idx] - strmOversize[idx] ;

										// bottom deck separation
				partitionFraction = MeanEfficiencyPartitionFraction( p.Rf_2(), p.Alpha_2(), p.D50c_2(iSG), thisConfig->OreSizesTop(iOSz), 
					                                                                                       thisConfig->OreSizesBottom(iOSz) ) ;
				partitionFraction = 1 - partitionFraction ;
				strmMids[idx] = partitionFraction * strmTopDeckUS[idx] ;
				strmUndersize[idx] = strmTopDeckUS[idx] - strmMids[idx] ;

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

											// top deck separation
					if ( thisConfig->IsLiberatedDiamond( iDSz, iOSz ) )			// check the diamond against all ore sizes
					{															// to see if it is liberated
						partitionFraction = MeanEfficiencyPartitionFraction(p.Rf_1(), p.Alpha_1(), p.DiamondD50c_1(), thisConfig->DiamondSizesTop(iDSz), 
							                                                                                        thisConfig->DiamondSizesBottom(iDSz)) ;
					}                
					else
					{
						partitionFraction = MeanEfficiencyPartitionFraction(p.Rf_1(), p.Alpha_1(), p.D50c_1(iSG), thisConfig->OreSizesTop(iOSz),
							                                                                                      thisConfig->OreSizesBottom(iOSz)) ;
					}
					partitionFraction = 1 - partitionFraction ;
					strmOversize[idx] = partitionFraction * CombinedFeed[idx] ;
					strmTopDeckUS[idx] = CombinedFeed[idx] - strmOversize[idx] ;

										// bottom deck separation
					if ( thisConfig->IsLiberatedDiamond( iDSz, iOSz ) )			// check the diamond against all ore sizes
					{															// to see if it is liberated
						partitionFraction = MeanEfficiencyPartitionFraction(p.Rf_2(), p.Alpha_2(), p.DiamondD50c_2(), thisConfig->DiamondSizesTop(iDSz), 
							                                                                                          thisConfig->DiamondSizesBottom(iDSz)) ;
					}                
					else
					{
						partitionFraction = MeanEfficiencyPartitionFraction(p.Rf_2(), p.Alpha_2(), p.D50c_2(iSG), thisConfig->OreSizesTop(iOSz),
							                                                                                      thisConfig->OreSizesBottom(iOSz)) ;
					}
					partitionFraction = 1 - partitionFraction ;
					strmMids[idx] = partitionFraction * strmTopDeckUS[idx] ;
					strmUndersize[idx] = strmTopDeckUS[idx] - strmMids[idx] ;
				}
			}
		}


	///
	///		Then Water and FeSi splits
	///
		idx = thisConfig->iWaterLimnStreamIndex() ;

		partitionFraction = p.WaterSplit_1() ;
		strmOversize[idx] = partitionFraction * CombinedFeed[idx] ;
		strmTopDeckUS[idx] = CombinedFeed[idx] - strmOversize[idx] ;

		partitionFraction = p.WaterSplit_2() ;
		strmMids[idx] = partitionFraction * strmTopDeckUS[idx] ;
		strmUndersize[idx] = strmTopDeckUS[idx] - strmMids[idx] ;


		idx = thisConfig->iFeSiLimnStreamIndex() ;

		partitionFraction = p.FeSiSplit_1() ;
		strmOversize[idx] = partitionFraction * CombinedFeed[idx] ;
		strmTopDeckUS[idx] = CombinedFeed[idx] - strmOversize[idx] ;

		partitionFraction = p.FeSiSplit_2() ;
		strmMids[idx] = partitionFraction * strmTopDeckUS[idx] ;
		strmUndersize[idx] = strmTopDeckUS[idx] - strmMids[idx] ;

	///
	///     Sort out stream ordering
	///
		int streamOS = 3 ;						// get an integer - take care of rounding...
		if (p.OversizeStreamID() < 1.5) 
			streamOS = 1 ;
		else if  (p.OversizeStreamID() < 2.5) 
			streamOS = 2 ;

		int streamUS = 3 ;
		if (p.UndersizeStreamID() < 1.5) 
			streamUS = 1 ;
		else if  (p.UndersizeStreamID() < 2.5) 
			streamUS = 2 ;
		
		switch( streamOS )
		{
		case 1:
			for ( idx = 0 ; idx < nComponents ; idx++ )
				Product1[idx] = strmOversize[idx] ;

			switch( streamUS )
			{
			case 2:
				for ( idx = 0 ; idx < nComponents ; idx++ )
				{
					Product2[idx] = strmUndersize[idx] ;
					Product3[idx] = strmMids[idx] ;
				}
				break;
			default:
				for ( idx = 0 ; idx < nComponents ; idx++ )
				{
					Product3[idx] = strmUndersize[idx] ;
					Product2[idx] = strmMids[idx] ;
				}
				break;
			}
			break;
		case 2:
			for ( idx = 0 ; idx < nComponents ; idx++ )
				Product2[idx] = strmOversize[idx] ;

			switch( streamUS )
			{
			case 1:
				for ( idx = 0 ; idx < nComponents ; idx++ )
				{
					Product1[idx] = strmUndersize[idx] ;
					Product3[idx] = strmMids[idx] ;
				}
				break;
			default:
				for ( idx = 0 ; idx < nComponents ; idx++ )
				{
					Product3[idx] = strmUndersize[idx] ;
					Product1[idx] = strmMids[idx] ;
				}
				break;
			}
			break;

		default:			// case 3, but catch all
			for ( idx = 0 ; idx < nComponents ; idx++ )
				Product3[idx] = strmOversize[idx] ;

			switch( streamUS )
			{
			case 2:
				for ( idx = 0 ; idx < nComponents ; idx++ )
				{
					Product2[idx] = strmUndersize[idx] ;
					Product1[idx] = strmMids[idx] ;
				}
				break;
			default:
				for ( idx = 0 ; idx < nComponents ; idx++ )
				{
					Product1[idx] = strmUndersize[idx] ;
					Product2[idx] = strmMids[idx] ;
				}
				break;
			}
			break;
		}	
			
		return 0 ;
	}
	else
	{
		return 3 ;			// Error: "Could  not access global configuration data"
	}
}
//////
int _Error_DiamondWizard_DualEfficiencyCurve ( int errorIndex, LPSTR errorBuffer ) 
{
	return TRUE ;
}

int  _Parameter_DiamondWizard_DualEfficiencyCurve ( int parameterIndex, LPSTR parameterInfoBuffer ) 
{
	return TRUE ;
}
//////
//////
//////
//////     
//
//
