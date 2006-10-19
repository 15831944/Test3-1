#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - TROMP CURVE
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_TrompCurve: public CLimn_ModelData_Base
  {
  public:
    C_ModelParameters_DiamondWizard_TrompCurve(void) : CLimn_ModelData_Base(&sm_Common)
      {
      m_nOSz = 0;
      m_nDSz = 0;
      };
    C_ModelParameters_DiamondWizard_TrompCurve(double* ModelParams, int nOSz, int nDSz) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      //m_nOSz=nOSz;
      //m_nDSz=nDSz;
      //m_pData=ModelParams;
      Initialise(nOSz, nDSz);
      }
    ~C_ModelParameters_DiamondWizard_TrompCurve(void)
      {
      };

    static CLimn_ModelData_Common sm_Common;

    CDoubleRef  FloatsStreamID; 
    CDoubleRef  WaterSplit;     
    CDoubleRef  FeSiSplit;      
    CVectorRef  Ep;        
    CVectorRef  Rho50;     
    CVectorRef  EpD;       
    CVectorRef  Rho50D;    

    bool Product1IsFloats()	   { return FloatsStreamID()  < 1.5 ? true : false ; };   

    void Initialise(int nOSz, int nDSz) 
      {
      CLimn_ModelData_Base::Initialise();

      m_nOSz  = nOSz;
      m_nDSz  = nDSz;

      FloatsStreamID  .Initialise(this,                         "FloatsStreamID",         1,  "",       true);
      WaterSplit      .Initialise(this,                         "WaterSplit",          0.25,  "Frac(.)");
      FeSiSplit       .Initialise(this,                         "FeSiSplit",           0.25,  "Frac(.)");
      Ep              .Initialise(this, "Ep",   m_nOSz, DI_OSz, "Ep",                 0.010,  "");
      Rho50           .Initialise(this, "Rho",  m_nOSz, DI_OSz, "Rho50",                  3,  "Rho(t/m^3)");
      EpD             .Initialise(this, "Ep",   m_nDSz, DI_DSz, "EpD",                0.010,  "");
      Rho50D          .Initialise(this, "Rho",  m_nDSz, DI_DSz, "Rho50D",                 3,  "Rho(t/m^3)");
      };

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
