#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - TROMP CURVE
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_TrompCurve: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelParameters_DiamondWizard_TrompCurve(void)
      {
      m_nOSz = 0;
	  m_nDSz = 0;
      };
    ~C_ModelParameters_DiamondWizard_TrompCurve(void){};


    C_ModelParameters_DiamondWizard_TrompCurve(double* ModelParams, int nOSz, int nDSz) // usually called from parameter passing
      { 
      m_nOSz=nOSz;
	  m_nDSz=nDSz;
      m_pData=ModelParams;
      }

    double & FloatsStreamID()  { return m_pData[0];						} ; 
    double & WaterSplit()      { return m_pData[1];						} ;
    double & FeSiSplit()       { return m_pData[2];						} ;
    double & Ep(int i)         { return m_pData[3+i];					} ;
    double & Rho50(int i)      { return m_pData[3 + m_nOSz + i];			} ;
    double & EpD(int i)        { return m_pData[3 + 2*m_nOSz + i];		} ;
    double & Rho50D(int i)     { return m_pData[3 + 2*m_nOSz + m_nDSz + i]; } ;

    int DataCount()            { return 3 + 2*m_nOSz + 2*m_nDSz;            } ;

    bool Product1IsFloats()	   { return FloatsStreamID()  < 1.5 ? true : false ; };   


#ifdef LIMNDW

	void Initialise() 
      {
		m_nOSz             = CONFIGURATION.nOSz();
		m_nDSz             = CONFIGURATION.nDSz();

		Allocate();

		FloatsStreamID()   = 1.0 ;
		WaterSplit()       = 0.25;
		FeSiSplit()        = 0.25;
		for (int i=0; i<m_nOSz; i++)
		{
			Ep(i) = 0.010;
			Rho50(i) = 3.00 ;
		}
		for (i=0; i<m_nDSz; i++)
		{
			EpD(i) = 0.010;
			Rho50D(i) = 3.00 ;
		}
      };
#endif

  protected:
    int            m_nOSz;
    int            m_nDSz;

  };
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_TrompCurve (int nRows,
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
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_TrompCurve ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_TrompCurve ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
