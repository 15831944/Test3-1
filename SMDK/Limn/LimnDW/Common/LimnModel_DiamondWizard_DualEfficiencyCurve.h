#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - DUAL EFFICIENCY CURVE
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_DualEfficiencyCurve: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelParameters_DiamondWizard_DualEfficiencyCurve(void)
      {
      m_nSGs=0;
      };
    ~C_ModelParameters_DiamondWizard_DualEfficiencyCurve(void){};


    C_ModelParameters_DiamondWizard_DualEfficiencyCurve(double* ModelParams, int nSG) // usually called from parameter passing
      { 
      m_nSGs=nSG;
      m_pData=ModelParams;
      }

		double & OversizeStreamID()  { return m_pData[0];                 }; 
		double & UndersizeStreamID() { return m_pData[1];                 }; 
		double & WaterSplit_1()      { return m_pData[2];                 };
		double & FeSiSplit_1()       { return m_pData[3];                 };
		double & Alpha_1()           { return m_pData[4];                 };
		double & Rf_1()              { return m_pData[5];                 };
		double & D50c_1(int i)       { return m_pData[6+i];               };
		double & DiamondD50c_1()     { return m_pData[6+m_nSGs];          };
		double & WaterSplit_2()      { return m_pData[7+m_nSGs];          };
		double & FeSiSplit_2()       { return m_pData[8+m_nSGs];          };
		double & Alpha_2()           { return m_pData[9+m_nSGs];          };
		double & Rf_2()              { return m_pData[10+m_nSGs];         };
		double & D50c_2(int i)       { return m_pData[11+m_nSGs+i];       };
		double & DiamondD50c_2()     { return m_pData[11+m_nSGs+m_nSGs];  };

		int DataCount()             { return 11 + m_nSGs + m_nSGs + 1;    };

		bool Product1IsOversize()	 { return OversizeStreamID()  < 1.5 ? true : false ; };   
		bool Product3IsUndersize()	 { return UndersizeStreamID() > 2.5 ? true : false ; };   


#ifdef LIMNDW

    void Initialise() 
      {
      m_nSGs             = CONFIGURATION.nSGs();

      Allocate();

      WaterSplit_1()       = 0.25;
      FeSiSplit_1()        = 0.25;
      Alpha_1()            = 10.0;
      Rf_1()               = 0.0;

	  for (int i=0; i<CONFIGURATION.nSGs(); i++)
        D50c_1(i) = 30.0;

	  DiamondD50c_1()      = 30.0;

      WaterSplit_2()       = 0.25;
      FeSiSplit_2()        = 0.25;
      Alpha_2()            = 10.0;
      Rf_2()               = 0.0;

	  for (int i=0; i<CONFIGURATION.nSGs(); i++)
        D50c_2(i) = 7.0;

	  DiamondD50c_2()      = 7.0;

	  };

#endif

  protected:
    int            m_nSGs;

  };
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_DualEfficiencyCurve (int nRows,
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
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_DualEfficiencyCurve ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_DualEfficiencyCurve ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
