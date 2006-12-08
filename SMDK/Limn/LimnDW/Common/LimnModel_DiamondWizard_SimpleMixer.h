#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - SIMPLE MIXER
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_SimpleMixer: public CLimn_ModelData_Base
  {
  public:
    C_ModelParameters_DiamondWizard_SimpleMixer(void)  : CLimn_ModelData_Base(&sm_Common)
      {
      };
    C_ModelParameters_DiamondWizard_SimpleMixer(double* ModelParams) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      Initialise(); // number of ore sizes
      };
    ~C_ModelParameters_DiamondWizard_SimpleMixer(void)
      {
      };
  
    static CLimn_ModelData_Common sm_Common;

    //CBooleanRef redistributeDensimetrics; 
    //CDoubleRef tau;	    					
    //CVectorRef S;						
    //CMatrixRef B;						
 

	void Initialise() 
      {
      CLimn_ModelData_Base::Initialise();
      //m_nOSz = nOSz;

      //redistributeDensimetrics      .Initialise(this,                                        "RedistributeDensimetrics",      1);
      //MarkParameterGap(nOSz-1);
      //tau                           .Initialise(this,                                        "Tau",                        0.14,  "");
      //MarkParameterGap(nOSz-1);
      //S                             .Initialise(this,   "S", m_nOSz,          DI_OSz,        "S",                           0.5,  "");
      //B                             .Initialise(this,   "B", m_nOSz, m_nOSz , DI_OSz, DI_OSz,"B",                           0.0,  "");

      };

  protected: ;

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
