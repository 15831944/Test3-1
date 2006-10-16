#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - GRINDING MILL
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_GrindingMill: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelParameters_DiamondWizard_GrindingMill(void)
      {
      m_nOSz=0;
      };
    ~C_ModelParameters_DiamondWizard_GrindingMill(void){};


    C_ModelParameters_DiamondWizard_GrindingMill(double* ModelParams, int nOSz) // usually called from parameter passing
      { 
      m_nOSz=nOSz;		// number of ore sizes
      m_pData=ModelParams;
      }

      double & redistributeDensimetrics()	{ return m_pData[0];			  }; 
      double & tau()						{ return m_pData[m_nOSz];		  };
      double & S(int i)						{ return m_pData[2 * m_nOSz + i]; } ;
      double & B(int i)						{ return m_pData[3 * m_nOSz + i]; } ;

 	  double* pS()						    { return &m_pData[2 * m_nOSz]; } ;
 	  double* pB()						    { return &m_pData[3 * m_nOSz]; } ;

      int DataCount()						{ return (3 + m_nOSz) * m_nOSz; };

	  bool DoRedistributeDensimetrics()		{ return redistributeDensimetrics() > 0.5 ? true : false ; }; 


#ifdef LIMNDW
	void Initialise() 
      {
		m_nOSz = CONFIGURATION.nOSz();

		Allocate();

		tau() = 0.14;
	    for (int i=0; i< m_nOSz; i++)
		{
			S(i) = 0.5 ;
			for (int j=0; i< m_nOSz; i++)
			{
				B(i) = 0.0 ;
			}
		}
		
      };
#endif

  protected:
    int    m_nOSz;

};
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_GrindingMill (int nRows,
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
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_GrindingMill ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_GrindingMill ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
//
//

//////
