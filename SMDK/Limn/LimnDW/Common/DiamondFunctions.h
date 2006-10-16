#pragma once

// DiamondFunctions.h : main header file for the DIAMONDFUNCTIONS XLL

#ifdef LIMNDW
	#include "md_headers.h"
	#include <vector>
#endif

//	Inline functions

inline int xlIndex(int iRow, int iColumn, int nColumns)
	{ 
		return iColumn + iRow * nColumns ; 
	}
              
// User defined structures 



//  global variables
              
                                
//  global routines  

#ifndef LIMNDW

	C_LINKAGE LPXLOPER DLLFUNC v1_3_A_TEST( LPXLOPER lpxConfigID, 
											LPXLOPER lpxParam1,
											LPXLOPER lpxParam2,
											LPXLOPER lpxParam3,
											LPXLOPER lpxParam4,
											LPXLOPER lpxParam5  ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondWizardLibraryVersion() ; 

	C_LINKAGE int DLLFUNC xlAutoOpen( ) ;

	C_LINKAGE int DLLFUNC xlAutoClose( ) ; 

	C_LINKAGE int DLLFUNC xlAutoAdd( ) ; 

	C_LINKAGE int DLLFUNC xlAutoRemove( ) ;  

	C_LINKAGE LPXLOPER DLLFUNC xlAddInManagerInfo( LPXLOPER xAction ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondWizardConfigureXLL( LPXLOPER lpxOreSizes,
			 												   LPXLOPER lpxDiamondSizes,
	 														   LPXLOPER lpxSGs,
															   LPXLOPER lpxDiamondSG,
															   LPXLOPER lpxRevenue,
															   LPXLOPER lpxLiberationMatrix,
															   LPXLOPER PassSizePercents) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondWizardConfigureXLL_Densimetrics( LPXLOPER lpxConfigID, 
																			LPXLOPER ReferenceDensimetrics) ;


	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondRevenue( LPXLOPER lpxConfigID, 
													LPXLOPER lpxDiamondAllocation ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondRevenueDistribution( LPXLOPER lpxConfigID, 
														 	    LPXLOPER lpxDiamondAllocation ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondLiberatedDistribution( LPXLOPER lpxConfigID, 
																  LPXLOPER lpxDiamondAllocation ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondSizeDistribution( LPXLOPER lpxConfigID, 
										 					 LPXLOPER lpxDiamondAllocation ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_OreMassFromSizeDistribution( LPXLOPER lpxConfigID, 
																 LPXLOPER lpxOreDistribution, 
																 LPXLOPER lpxTotalFlow ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_OreMassFromSGDistribution( LPXLOPER lpxConfigID, 
											  				   LPXLOPER lpxOreDistribution, 
													           LPXLOPER lpxTotalFlow ) ;


    C_LINKAGE LPXLOPER DLLFUNC v1_3_OreMassFromDensimetricDistribution( LPXLOPER lpxConfigID, 
			  												            LPXLOPER lpxDensimetricDistribution, 
													                    LPXLOPER lpxSizeDistribution, 
			                                                            LPXLOPER lpxTotalFlow ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_OreSizeDistributionFromMass( LPXLOPER lpxConfigID, 
															     LPXLOPER lpxOreMass ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_OreSGDistributionFromMass( LPXLOPER lpxConfigID, 
															   LPXLOPER lpxOreMass ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_OreTotalSizeDistribution( LPXLOPER lpxConfigID, 
			                                                  LPXLOPER lpxOreMass ) ;

	C_LINKAGE LPXLOPER DLLFUNC v1_3_OreTotalSGDistribution( LPXLOPER lpxConfigID, 
			                                                LPXLOPER lpxOreMass ) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondSizeDistributionFromDeportment( LPXLOPER lpxConfigID, 
			                                                             LPXLOPER lpxDiamondDeportment) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondSGDistributionFromDeportment( LPXLOPER lpxConfigID, 
			                                                           LPXLOPER lpxDiamondDeportment) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondDeportmentFromSizeDistribution( LPXLOPER lpxConfigID, 
																	     LPXLOPER lpxOreDistribution,
																		 LPXLOPER lpxDiamondDistribution,
 																		 LPXLOPER lpxDiamondFlowBasis ) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_DiamondDeportmentFromSGDistribution( LPXLOPER lpxConfigID, 
													 				   LPXLOPER lpxOreDensimetricDistribution,
													 				   LPXLOPER lpxOreSizeDistribution,
																       LPXLOPER lpxDiamondSGAllocation,
                                                                       LPXLOPER lpxDiamondSizeDistribution,
 																       LPXLOPER lpxDiamondFlowBasis ) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_TotalLiberatedDiamonds( LPXLOPER lpxConfigID, 
		                                                  LPXLOPER lpxDiamondDeportment ) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_PercentPassingSize( LPXLOPER lpxConfigID, 
													  LPXLOPER lpxPercent,
 													  LPXLOPER lpxPctRetained ) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummary( LPXLOPER lpxConfigID, 
									 			 LPXLOPER lpxRevenueBase,
  					 						     LPXLOPER lpxCaratsBase,
		 										 LPXLOPER lpxLimnStream ) ;

	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummaryCalculations1( LPXLOPER lpxConfigID, 
									 						  LPXLOPER lpxRevenueBase,
               					 						      LPXLOPER lpxCaratsBase,
		 			                                          LPXLOPER lpxLimnFFFF,
														      LPXLOPER lpxLimnPPP1 ) ;
	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummaryCalculations2( LPXLOPER lpxConfigID, 
									 						  LPXLOPER lpxRevenueBase,
               					 						      LPXLOPER lpxCaratsBase,
		 													  LPXLOPER lpxLimnFFFF,
															  LPXLOPER lpxLimnPPP1,
														      LPXLOPER lpxLimnPPP2 ) ;
	C_LINKAGE
            LPXLOPER DLLFUNC v1_3_StreamSummaryCalculations3( LPXLOPER lpxConfigID, 
									 						  LPXLOPER lpxRevenueBase,
               					 						      LPXLOPER lpxCaratsBase,
		 													  LPXLOPER lpxLimnFFFF,
														      LPXLOPER lpxLimnPPP1,
														      LPXLOPER lpxLimnPPP2,
														      LPXLOPER lpxLimnPPP3 ) ;

    C_LINKAGE LPXLOPER DLLFUNC v1_3_DiamondCarats( LPXLOPER lpxConfigID, 
													LPXLOPER lpxDiamondAllocation ) ;



								//	Local calculation functions
	int StreamSummaryCalculationFromExcel( double SummaryXXXX[], 
			                               LPXLOPER lpxLimnXXXX, 
						                   double revenueBase, 
									       double caratsBase, 
										   CDiamondWizardConfiguration* thisConfig ) ;

	double AuditCheck( double FFFF, double PPP1) ;
	double AuditCheck( double FFFF, double PPP1, double PPP2) ;
	double AuditCheck( double FFFF, double PPP1, double PPP2, double PPP3) ;



								// polymorphic functions to handle coercion of references
	int CoerceReference( LPXLOPER excelRef, CString& theCString ) ;
	int CoerceReference( LPXLOPER excelRef, int& theInt ) ;
	int CoerceReference( LPXLOPER excelRef, double& theDouble ) ;

                                // polymorphic functions to check references
    bool parameterCheckOK( LPXLOPER lpxParam1, 
                           LPXLOPER lpxParam2, 
                           LPXLOPER lpxParam3,
                           LPXLOPER lpxParam4,
                           LPXLOPER lpxParam5,
                           LPXLOPER lpxParam6,
                           LPXLOPER lpxParam7 ) ;
    bool parameterCheckOK( LPXLOPER lpxParam1, 
                           LPXLOPER lpxParam2, 
                           LPXLOPER lpxParam3,
                           LPXLOPER lpxParam4,
                           LPXLOPER lpxParam5,
                           LPXLOPER lpxParam6 ) ;
    bool parameterCheckOK( LPXLOPER lpxParam1, 
                           LPXLOPER lpxParam2, 
                           LPXLOPER lpxParam3,
                           LPXLOPER lpxParam4,
                           LPXLOPER lpxParam5 ) ;
    bool parameterCheckOK( LPXLOPER lpxParam1, 
                           LPXLOPER lpxParam2, 
                           LPXLOPER lpxParam3,
                           LPXLOPER lpxParam4 ) ;

#endif  // LIMNDW
///
///
///---------------------------------------------------------------------------------------------------
///
	double CalculatePassingSize( double Percent, double* OreDistribution, int nOsz, CDiamondWizardConfiguration* thisConfig ) ;

	void OreMassFromDensimetricDistribution( CDiamondWizardConfiguration* thisConfig, 
											 double* densimetricDistribution, 
											 double* sizeFractionData, 
											 double totalFlow, 
											 double* oreMass ) ;

	void DiamondDeportmentFromSGDistribution( CDiamondWizardConfiguration* thisConfig,
												double* OreDistribution, 
												double* OreSizeDistribution, 
												double* DiamondSGAllocation, 
												double* DiamondSizeDistribution, 
												double DiamondFlowBasis,
												double* DiamondDeportment) ;

	int	StreamSummaryCalculation( double SummaryXXXX[], 
								  double* pData, 
								  double revenueBase, 
								  double caratsBase, 
								  CDiamondWizardConfiguration* thisConfig ) ;




