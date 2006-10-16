#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - SIMPLE MIXER
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_SimpleMixer: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelParameters_DiamondWizard_SimpleMixer(void)
      {
      };
    ~C_ModelParameters_DiamondWizard_SimpleMixer(void){};


    C_ModelParameters_DiamondWizard_SimpleMixer(double* ModelParams) // usually called from parameter passing
      { 
      	m_pData=ModelParams;
	  }
  
      int DataCount()              { return 1;       };
 

#ifdef LIMNDW
	void Initialise() 
      {

      Allocate();

      };
#endif

  protected:

  };
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_SimpleMixer (int nRows,
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
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_SimpleMixer ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_SimpleMixer ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
