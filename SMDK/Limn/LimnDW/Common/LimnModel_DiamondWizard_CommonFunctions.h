#pragma once
//-------------------------------------------------------------------------------------------------------
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LIMNDW

#define CONFIGURATION   gs_DWCfg
#define CONFIGURATIONS  gs_DWCfgs

const int xlerrRef  = -1;
const int xlerrNull = -2;

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CLimn_ModelData_AbstractBase
  {
  public:
    CLimn_ModelData_AbstractBase(void)
      {
      m_pData=NULL;
      }
    ~CLimn_ModelData_AbstractBase(void)
      {
      }

    virtual int       DataCount() = 0;

	virtual void Allocate()
      {
      if (m_pData==NULL)
        {
        m_Data.SetSize(DataCount());
        m_pData=&m_Data[0];
        }
      }

    double *GetDataPtr()
      {
      Allocate();
      return m_pData;
      }

  protected:
    CArray<double, double>   m_Data;
    double                 * m_pData;
  };



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///			Calculation functions for Limn Diamond Wizard models
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					MeanEfficiencyPartitionFraction
///						Calculates a mean partition fraction for a size interval by linear interpolation over 29 sub intervals
double MeanEfficiencyPartitionFraction(double Rf, double Alpha, double D50c, double intervalTopSize, double intervalBottomSize) ;
//
//
///					MeanTrompPartitionFraction
///						Calculates a mean partition fraction for a density interval by linear interpolation over 29 sub intervals
double MeanTrompPartitionFraction(double EpD, double Rho50D, double intervalTopSG, double intervalBottomSG) ;
//
//-------------------------------------------------------------------------------------------------------
//
///     Function to calculate the transformation matrix X ( p = X.f )
///     according to the Whiten Crusher Model described in
///     the JKMRC monograph:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p138ff
///
void WhitenCrusherTransformationMatrix(const LowerTriangularMatrix& BreakageDistributionFunction, 
									   const DiagonalMatrix& ClassificationFunction, 
									   Matrix& X) ;
//
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
										 DiagonalMatrix& C) ;
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
								   LowerTriangularMatrix& B) ;



///
///     Function to perform the redistribution of densimetrics to match a feed stream
///     (this is a kludge to account for the fact that densimetrics are measured at
//       feed to the DMS. It also simulates liberation)
///
void RedistributeOreDensimetricToFeedDistribution(CDiamondWizardConfiguration* thisConfig,
												  double oreFlow,
												  Matrix& product,
												  ColumnVector& meanSizeDistribution,
												  ColumnVector& meanSGDistribution) ;

//-------------------------------------------------------------------------------------------------------
//
