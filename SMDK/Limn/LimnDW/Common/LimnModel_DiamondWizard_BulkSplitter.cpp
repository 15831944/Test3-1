////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
#include "LimnModel_DiamondWizard_BulkSplitter.h"

CLimn_ModelData_Common C_ModelParameters_DiamondWizard_BulkSplitter::sm_Common;

//////     
//
//----------------------------------------------------------------------------------------------
//
//
//		Diamond Wizard - Simple Mixer model
//
//								Parameters: 
//										0. Split of all components to Product 1
//
int _Model_DiamondWizard_BulkSplitter (int nRows,
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
		C_ModelParameters_DiamondWizard_BulkSplitter p( ModelParams ) ;		// model parameters - copied to class variables for clarity

		int nComponents = nRows * nColumns ;

		int idx = 0 ;						// index into vector of stream data

		for ( idx = 0 ; idx < nComponents ; idx++ )
		{
			Product1[idx] = p.SplitToProduct1() * CombinedFeed[idx] ;				// that's it!
			Product2[idx] = CombinedFeed[idx] - Product1[idx] ;
		}

 		return 0 ;
	}
	else
	{
		return 3 ;			// Error: "Could  not access global configuration data"
	}
}
//////
int  _Error_DiamondWizard_BulkSplitter ( int errorIndex, LPSTR errorBuffer ) 
{
	return TRUE ;
}

int  _Parameter_DiamondWizard_BulkSplitter ( int parameterIndex, LPSTR parameterInfoBuffer ) 
{
	return TRUE ;
}

