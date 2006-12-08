#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - SIMPLE MIXER
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_BulkSplitter: public CLimn_ModelData_Base
  {
  public:
    C_ModelParameters_DiamondWizard_BulkSplitter(void) : CLimn_ModelData_Base(&sm_Common)
      {
      };
    C_ModelParameters_DiamondWizard_BulkSplitter(double* ModelParams) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      Initialise(); 
      }
    ~C_ModelParameters_DiamondWizard_BulkSplitter(void)
      {
      };

    static CLimn_ModelData_Common sm_Common;

    CDoubleRef SplitToProduct1;

	void Initialise() 
      {
      CLimn_ModelData_Base::Initialise();

      SplitToProduct1.Initialise(this,  "SplitToProduct1",  0.5,  "Frac(.)");
      };

  protected:

  };
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_BulkSplitter (int nRows,
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
                                                         double* Product6 ) ;
//
//				Error information
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_BulkSplitter ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_BulkSplitter ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
