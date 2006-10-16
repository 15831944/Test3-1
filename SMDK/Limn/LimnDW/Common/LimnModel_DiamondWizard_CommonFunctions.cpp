////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif

#include "LimnModel_DiamondWizard_CommonFunctions.h"
//////     
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///			Calculation functions for Limn Diamond Wizard models
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					MeanEfficiencyPartitionFraction
///						Calculates a mean partition fraction for a size interval by linear interpolation over 29 sub intervals
double MeanEfficiencyPartitionFraction(double Rf, double Alpha, double D50c, double intervalTopSize, double intervalBottomSize)
{
    double partitionFraction = 0.0 ;
    double increment ;
    double meanSize ;
    double topSize ;
    double bottomSize ;
    double alpha_d_d50c ;

    increment = (intervalTopSize - intervalBottomSize) / 30.0 ;
    topSize = intervalTopSize ;
    
    for ( int i = 1 ; i < 30 ; i++ )       
	{
        bottomSize = topSize - increment ;
        
        meanSize = sqrt(topSize * bottomSize) ;
    
        alpha_d_d50c = (Alpha * meanSize / D50c) ;  // intermediate calc
        
        if (alpha_d_d50c < 700)                     // clip to reasonable values
		{
            partitionFraction = partitionFraction + ((1 - Rf) * ((exp(Alpha) - 1) / (exp(alpha_d_d50c) + exp(Alpha) - 2))) ;
		}
        
        topSize = bottomSize ;
	}
	
    return partitionFraction / 29.0	;				// return the average

}
///					MeanTrompPartitionFraction
///						Calculates a mean partition fraction for a density interval by linear interpolation over 29 sub intervals
double MeanTrompPartitionFraction(double EpD, double Rho50D, double intervalTopSG, double intervalBottomSG)
{
    double partitionFraction = 0.0 ;
    double increment ;
    double meanSG ;
    double topSG ;
    double bottomSG ;

    increment = (intervalTopSG - intervalBottomSG) / 30.0 ;
    topSG = intervalTopSG ;
    
    for ( int i = 1 ; i < 30 ; i++ )       
	{
        bottomSG = topSG - increment ;
        
        meanSG = (topSG + bottomSG) / 2.0 ;
    
        partitionFraction = partitionFraction + (1 - 1 / (1 + exp(1.098 * (Rho50D - meanSG) / EpD))) ;
    
        topSG = bottomSG ;
	}
	
    return partitionFraction / 29.0	;				// return the average

}



///
///
///     Function to calculate the transformation matrix X ( p = X.f )
///     according to the Whiten Crusher Model described in
///     the JKMRC monograph:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p138ff
///
void WhitenCrusherTransformationMatrix(const LowerTriangularMatrix& B, const DiagonalMatrix& C, Matrix& X)
{
    int matrixSize = B.Nrows() ;
    IdentityMatrix I(matrixSize);					 
						            /// do the matrix calculations
	X = (I - C) * (I - (B * C)).i() ;

}
///
///
///     Function to calculate the Classification Function
///     for the Whiten Crusher Model.
///     Refer:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p141
///
void WhitenCrusherClassificationFunction(int nSizes,
										 double* Size, 
                                         double K1, 
                                         double K2, 
                                         double K3,
										 DiagonalMatrix& C)
{
	double cVal ;
    for ( int i = 0 ; i < nSizes ; i++ )
	{
		if ( Size[i] < K1 )
		{
            cVal = 0.0 ;
		}
		else if ( Size[i] > K2 )
		{
            cVal = 1.0 ;
		}
        else
		{
            cVal = 1 - pow(((K2 - Size[i]) / (K2 - K1)), K3) ;
		}

		C(i+1,i+1) = cVal ;

	}
}
///
///
///     Function to calculate the value of the breakage distribution
///     function given T10 and a table ot T10 versus Tn
///     Refer:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p143
///
///     Uses spline interpolation
///
void WhitenCrusherBreakageFunction(int nSizes, 
								   double* Size,
                                    double requiredT10,
                                     const Matrix& T10_V_Tn,
									  LowerTriangularMatrix& B)
{
    int nT10 = 5 ;
    int nTn = 7 ;
    
    int iT10 ;
    int iTn ;
    
    int i ;
    int j ;
    int k ;

    double S0 ;
    
    double interpTn[7] ;			 /// extra columns at T1000, T10=0 & T1, T10=100 to help extrapolation

									/// Pin down the end of the curve at T100000, T10=0 to help extrapolation
									/// Calculate 1/Tn fractions
									/// Pin down the end of the curve at T1, T10=100 to help extrapolation
	double fractionTn[7] =  {1.0 / 100000.0, 
							 1.0 / 75.0,     
							 1.0 / 50.0,
                             1.0 / 25.0,
							 1.0 / 4.0,
                             1.0 / 2.0,
							 1.0 } ;

	double X[5] = { 0.0, 10.0, 20.0, 30.0, 40.0 } ;               /// T10 data to interpolate on

	double Y[5] ;
    
    double cumulativeB[100] ;

    double fractionSize ;
    
    CSpline Spline ;
    
                            /// Interpolate to get a range of Tn data at the given T10

	for ( iTn = 0 ; iTn < nTn - 2 ; iTn++ )    /// Do all normal Tn values
	{
        for ( iT10 = 0 ; iT10 < nT10 ; iT10++ )             /// This particular Tn data slice
		{
            Y[iT10] = T10_V_Tn(iT10+1, iTn+1) ;
		}
        
        Spline.Make( X, Y, nT10 ) ;                             /// Calculate the spline
        
        interpTn[iTn+1] = Spline.Evaluate(requiredT10) ;     /// Evaluate the spline
	}
    
    interpTn[0] = 0.0000001 ;
    interpTn[6] = 100.0 ;

                            /// Then interpolate to get the CUMULATIVE breakage distibution function by size
    Spline.Make( fractionTn, interpTn, nTn ) ;                /// Calculate the spline
    S0 = Size[0] ;
    cumulativeB[0] = 1.0 ;                                     /// 100% passing top size
    for ( i = 1 ; i < nSizes ; i++)
	{
        fractionSize = Size[i] / S0 ;
        cumulativeB[i] = Spline.Evaluate(fractionSize) / 100.0 ;  /// Evaluate the spline for all size ratios
	}
        
	ColumnVector Bzero(nSizes) ;
	Bzero = 0.0 ;

	for ( i = 2 ; i <= nSizes ; i++ )	/// first entry in breakage distribution function always 0 (nothing breaks into the top size), so start one down
	{
		double bdv = cumulativeB[i - 2] - cumulativeB[i - 1] ;
		if ( bdv < 0.0 ) bdv = 0.0 ;				//Modification by Kurt Petersen 04/06/2002
		Bzero(i) = bdv ;			    /// convert cumulative to actual breakage distribution function
	}

	for ( i = 1 ; i <= nSizes ; i++ )
	{
		k = 1 ;
		for ( j = i ; j <= nSizes ; j++ )
		{
			B(j,i) = Bzero(k++) ;         /// fill columns of lower triangular
		}
	}

}
///
///     Function to perform the redistribution of densimetrics to match a feed stream
///     (this is a kludge to account for the fact that densimetrics are measured at
//       feed to the DMS. It also simulates liberation)
///

void RedistributeOreDensimetricToFeedDistribution(CDiamondWizardConfiguration* thisConfig,
												  double oreFlow,
												  Matrix& product,
												  ColumnVector& meanSizeDistribution,
												  ColumnVector& meanSGDistribution)
{    
    int iSG ;
    int iOSz ;

	int nOSz = thisConfig->nOreSizes() ;
	int nSGs  = thisConfig->nSGs() ;

	double sizeFlow ;

	if ( oreFlow > 0.0 )
	{
													// work out the mean size distribution
		for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
		{
			sizeFlow = 0.0 ;
			for ( iSG = 1 ; iSG <= nSGs ; iSG++ )
			{
	            sizeFlow += oreFlow * product(iOSz, iSG) * meanSGDistribution(iSG) / 10000.0 ;
			}
	        if ( sizeFlow < 0.0000001 ) sizeFlow = 0.0 ;
			meanSizeDistribution(iOSz) = 100.0 * sizeFlow / oreFlow ;
		}

		for ( iSG = 1 ; iSG <= nSGs ; iSG++ )
		{
			meanSGDistribution(iSG) = 0.0 ;        // new SG Distribution
			for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
			{
				meanSGDistribution(iSG) += thisConfig->ReferenceDensimetrics(iOSz-1, iSG-1) * meanSizeDistribution(iOSz) / 100.0  ;
			}
		}

		for ( iSG = 1 ; iSG <= nSGs ; iSG++ )		// redistribute feed densimetrics to new size and density distributions
		{
			for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
			{
				if ( meanSGDistribution(iSG) > 0.0 )
				{
					product(iOSz, iSG) = meanSizeDistribution(iOSz) * thisConfig->ReferenceDensimetrics(iOSz-1, iSG-1) / meanSGDistribution(iSG) ;
				}
				else
				{
					product(iOSz, iSG) = 0.0 ;
				}
			}
		}
	}	
}
/*
'''
'''     Function to return the diamond allocation as
'''     a result of a breakage process.
'''     i.e. Calculate p = x.f and allocate diamonds
'''     into the daughter fragments according to the
'''     distribution of diamonds in the parent sizes
'''
'''                 this function recalculates the breakage function
'''                 and transformation matrix for each SG, based on
'''                 differing T10 values for each SG
'''
Function SG_Dependent_DiamondMatrixCrusherModel(diamondFeedMass As Range, _
                                                 Size As Range, _
                                                  requiredT10 As Range, _
                                                   T10_V_Tn As Range, _
                                                    ClassificationFunction As Range) As Variant
    Dim diamondProduct() As Double
    
    Dim X As Variant                ''' transformation matrix
    Dim B As Variant                ''' breakage distribution function
    
    
    Dim productSum As Double
    Dim feedSum As Double
    
    Dim iDSz As Integer                 ''' Counters
    Dim iOSz As Integer
    Dim iPOsz As Integer
    Dim iSG As Integer
    Dim iOSindex As Integer             ''' Indices into 2D representation
    Dim iDSindex As Integer             ''' of 3D deportment matrix
    Dim iPOSindex As Integer            ''' Ore Size Index into Product matrix
    
    On Error Resume Next        '' ignore div by zero errors etc
    
    ReDim diamondProduct((nOreSizes * 2), (nDiamondSizes * Int((nSGs + 1) / 2)))
    
    
    For iSG = 1 To nSGs
                            ''''''''''''' modifications to calculate SG specific Breakage Function
                            ''''''''''''' and transformation matrix "on the fly"
                                           '' calculate b for one of the row of T10 values from spreadsheet
        B = WhitenCrusherBreakageFunction(Size, requiredT10(1, iSG), T10_V_Tn)
        X = VBA_WhitenCrusherTransformationMatrix(B, ClassificationFunction)
        
        For iDSz = 1 To nDiamondSizes
            iDSindex = DiamondSizeIndex(iDSz, iSG)
            
            productSum = 0
            feedSum = 0
            
            For iPOsz = 1 To nOreSizes - 1
                iPOSindex = OreSizeIndex(iPOsz, iSG)
                diamondProduct(iPOSindex, iDSindex) = 0
            
                For iOSz = 1 To iPOsz
                    iOSindex = OreSizeIndex(iOSz, iSG)
                    diamondProduct(iPOSindex, iDSindex) = _
                        diamondProduct(iPOSindex, iDSindex) + _
                            (diamondFeedMass(iOSindex, iDSindex) * _
                                X(iPOsz, iOSz))
                Next iOSz
                    
                productSum = productSum + diamondProduct(iPOSindex, iDSindex)
                feedSum = feedSum + diamondFeedMass(iOSindex, iDSindex)

            Next iPOsz
                                ''' Calculate finest fraction by difference
            iOSindex = OreSizeIndex(nOreSizes, iSG)
            feedSum = feedSum + diamondFeedMass(iOSindex, iDSindex)
            
            diamondProduct(iOSindex, iDSindex) = feedSum - productSum
            
            
        Next iDSz
    Next iSG
    
    SG_Dependent_DiamondMatrixCrusherModel = diamondProduct
    
End Function

'''
'''     See Kurt Petersen's workbook
'''     "comminution solution update.xls" 26Feb2002
'''     for the methodology!
'''
'''
Function DiamondBreakageModel(diamondFeedMass As Range, _
                               oreFeedDistribution As Range, _
                                oreProductDistribution As Range, _
                                 B As Range, _
                                  s As Range, _
                                   Tau As Double) As Variant
    Dim diamondProduct() As Double
    Dim FS() As Double
    
    Dim brokenOut() As Double
    Dim brokenSum() As Double
    Dim totalBroken() As Double
    Dim brokenIn As Double
    Dim productSum As Double
    Dim feedSum As Double
    
    Dim iDSz As Integer                 ''' Counters
    Dim iOSz As Integer
    Dim j As Integer
    Dim iSG As Integer
    Dim iOSindex As Integer             ''' Indices into 2D representation
    Dim iDSindex As Integer             ''' of 3D deportment matrix
    Dim jOSindex As Integer
    
    On Error Resume Next        '' ignore div by zero errors etc
    
    ReDim diamondProduct((nOreSizes * 2), (nDiamondSizes * Int((nSGs + 1) / 2)))
    ReDim SPon1pTS(nOreSizes)
        
    ReDim brokenOut(nOreSizes, nOreSizes)
    ReDim brokenSum(nOreSizes)
    ReDim totalBroken(nOreSizes)
    
    For iSG = 1 To nSGs
        
        For iOSz = 1 To nOreSizes
                                                                ''' Modified model 27Feb02
                                                                    ''' calculate the total broken
            totalBroken(iOSz) = oreFeedDistribution(iOSz, iSG) * (1 - (1 / (1 + Tau * s(iOSz, 1))))
                                                                    ''' initialise the matrix
            For j = 1 To nOreSizes
                brokenOut(iOSz, j) = 0#
            Next j
            
        Next iOSz
            
        totalBroken(nOreSizes) = 0#                             ''' nothing broken from lowest size!
        
        
        For iOSz = 2 To (nOreSizes - 1)         ''' distribution of material broken
            For j = 1 To (iOSz - 1)          ''' from each size fraction
                brokenOut(iOSz, j) = Tau * B(iOSz, j) * s(j, 1) * oreProductDistribution(j, iSG) / (1 + (Tau * s(iOSz)))
            Next j
        Next iOSz
        
                                                ''' calculate the last by difference
        For j = 1 To nOreSizes
            brokenSum(j) = 0#
            For iOSz = (j + 1) To (nOreSizes - 1)
                brokenSum(j) = brokenSum(j) + brokenOut(iOSz, j)
            Next iOSz
        Next j
        
        For j = 1 To nOreSizes - 1
            brokenOut(nOreSizes, j) = totalBroken(j) - brokenSum(j)
            brokenOut(nOreSizes, j) = brokenOut(nOreSizes, j)
        Next j
        
                                                ''' now do the allocation
        For iOSz = 1 To nOreSizes
        
            For iDSz = 1 To nDiamondSizes
                iDSindex = DiamondSizeIndex(iDSz, iSG)
            
                iOSindex = OreSizeIndex(iOSz, iSG)
                diamondProduct(iOSindex, iDSindex) = _
                    diamondFeedMass(iOSindex, iDSindex) * _
                       (1 - totalBroken(iOSz) / (oreFeedDistribution(iOSz, iSG) + 1E-31))
                       
                feedSum = feedSum + diamondFeedMass(iOSindex, iDSindex)
                       
                brokenIn = 0
                For j = 1 To (iOSz - 1)
                    jOSindex = OreSizeIndex(j, iSG)
                    brokenIn = brokenIn + (diamondFeedMass(jOSindex, iDSindex) * (brokenOut(iOSz, j) / (oreFeedDistribution(j, iSG) + 1E-31)))        '' + 1E-31 to avoid div 0)
                Next j
                    
                diamondProduct(iOSindex, iDSindex) = diamondProduct(iOSindex, iDSindex) + brokenIn
            
            Next iDSz
            
            If feedSum < 0.0000001 Then             '' Handle start up conditions
                    iOSindex = OreSizeIndex(iOSz, iSG)  '' If no feed, make no product!
                    diamondProduct(iOSindex, iDSindex) = 0#
            End If
        
        Next iOSz
    Next iSG
    
    DiamondBreakageModel = diamondProduct
    
End Function


*/
//////     
//
//
