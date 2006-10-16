#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - EFFICIENCY CURVE
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_EfficiencyCurve: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelParameters_DiamondWizard_EfficiencyCurve(void)
      {
      m_nSGs=0;
      };
    ~C_ModelParameters_DiamondWizard_EfficiencyCurve(void){};


    C_ModelParameters_DiamondWizard_EfficiencyCurve(double* ModelParams, int nSG) // usually called from parameter passing
      { 
      m_nSGs=nSG;
      m_pData=ModelParams;
      }

      double & OversizeStreamID()  { return m_pData[0];         }; 
      double & WaterSplit()        { return m_pData[1];         };
      double & FeSiSplit()         { return m_pData[2];         } ;
      double & Alpha()             { return m_pData[3];         } ;
      double & Rf()                { return m_pData[4];         } ;
      double & D50c(int i)         { return m_pData[5+i];       } ;
      double & DiamondD50c()       { return m_pData[5+m_nSGs];  };
  
      int DataCount()              { return 5 + m_nSGs + 1;       };
  
	  bool Product1IsOversize()	   { return OversizeStreamID()  < 1.5 ? true : false ; };   


#ifdef LIMNDW
	void Initialise() 
      {
      m_nSGs             = CONFIGURATION.nSGs();

      Allocate();

      WaterSplit()       = 0.25;
      FeSiSplit()        = 0.25;
      Alpha()            = 10.0;
      Rf()               = 0.0;
      for (int i=0; i<CONFIGURATION.nSGs(); i++)
        D50c(i) = 0.010;
      DiamondD50c()      = 0.010;
      };
#endif

  protected:
    int            m_nSGs;

  };
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_EfficiencyCurve (int nRows,
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
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_EfficiencyCurve ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_EfficiencyCurve ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
