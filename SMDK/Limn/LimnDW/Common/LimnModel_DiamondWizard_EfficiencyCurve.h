#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - EFFICIENCY CURVE
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_EfficiencyCurve: public CLimn_ModelData_Base
  {
  public:
    C_ModelParameters_DiamondWizard_EfficiencyCurve(void) : CLimn_ModelData_Base(&sm_Common)
      {
      m_nSGs=0;
      };
    C_ModelParameters_DiamondWizard_EfficiencyCurve(double* ModelParams, int nSG) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      //m_pData=ModelParams;
      Initialise(nSG); 
      }
    ~C_ModelParameters_DiamondWizard_EfficiencyCurve(void)
      {
      };

    static CLimn_ModelData_Common sm_Common;

    CDoubleRef OversizeStreamID;  
    CDoubleRef WaterSplit;        
    CDoubleRef FeSiSplit;         
    CDoubleRef Alpha;             
    CDoubleRef Rf;                
    CVectorRef D50c;         
    CDoubleRef DiamondD50c;       


    bool Product1IsOversize()	   { return OversizeStreamID()  < 1.5 ? true : false ; };   

    void Initialise(int nSGs) 
      {
      CLimn_ModelData_Base::Initialise();

      m_nSGs             = nSGs;

      OversizeStreamID  .Initialise(this,                       "OversizeStreamID",      1,   "",       true);
      WaterSplit        .Initialise(this,                       "WaterSplit",         0.25,   "Frac(.)");
      FeSiSplit         .Initialise(this,                       "FeSiSplit",          0.25,   "Frac(.)");
      Alpha             .Initialise(this,                       "Alpha",                10,   "");
      Rf                .Initialise(this,                       "Rf",                    0,   "");
      D50c              .Initialise(this, "D50", m_nSGs, DI_SG, "D50c",                 10,   "L(mm)");
      DiamondD50c       .Initialise(this,                       "DiamondD50c",          10,   "L(mm)");

      };

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
