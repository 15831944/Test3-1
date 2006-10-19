#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - DUAL EFFICIENCY CURVE
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_DualEfficiencyCurve: public CLimn_ModelData_Base
  {
  public:
    C_ModelParameters_DiamondWizard_DualEfficiencyCurve(void) : CLimn_ModelData_Base(&sm_Common)
      {
      m_nSGs=0;
      };
    C_ModelParameters_DiamondWizard_DualEfficiencyCurve(double* ModelParams, int nSG) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      //m_pData=ModelParams;
      Initialise(nSG);
      }
    ~C_ModelParameters_DiamondWizard_DualEfficiencyCurve(void)
      {
      };

    static CLimn_ModelData_Common sm_Common;

    CDoubleRef  OversizeStreamID; 
    CDoubleRef  UndersizeStreamID;
    CDoubleRef  WaterSplit_1;     
    CDoubleRef  FeSiSplit_1;      
    CDoubleRef  Alpha_1;          
    CDoubleRef  Rf_1;             
    CVectorRef  D50c_1;      
    CDoubleRef  DiamondD50c_1;    
    CDoubleRef  WaterSplit_2;     
    CDoubleRef  FeSiSplit_2;      
    CDoubleRef  Alpha_2;          
    CDoubleRef  Rf_2;             
    CVectorRef  D50c_2;      
    CDoubleRef  DiamondD50c_2;    

    bool Product1IsOversize()	 { return OversizeStreamID()  < 1.5 ? true : false ; };   
    bool Product3IsUndersize()	 { return UndersizeStreamID() > 2.5 ? true : false ; };   

    void Initialise(int nSGs) 
      {
      CLimn_ModelData_Base::Initialise();

      m_nSGs = nSGs;

      OversizeStreamID  .Initialise(this,                          "OversizeStreamID",        1,  "",       true);
      UndersizeStreamID .Initialise(this,                          "UndersizeStreamID",       2,  "",       true);
      MarkText("Curve:1");
      WaterSplit_1      .Initialise(this,                          "WaterSplit_1",         0.25,  "Frac(.)");
      FeSiSplit_1       .Initialise(this,                          "FeSiSplit_1",          0.25,  "Frac(.)");
      Alpha_1           .Initialise(this,                          "Alpha_1",                10,  "");
      Rf_1              .Initialise(this,                          "Rf_1",                    0,  "");
      D50c_1            .Initialise(this,  "D50", m_nSGs, DI_SG,   "D50c_1",                 30,  "L(mm)");
      DiamondD50c_1     .Initialise(this,                          "DiamondD50c_1",          30,  "L(mm)");
      MarkText("Curve:2");
      WaterSplit_2      .Initialise(this,                          "WaterSplit_2",         0.25,  "Frac(.)");
      FeSiSplit_2       .Initialise(this,                          "FeSiSplit_2",          0.25,  "Frac(.)");
      Alpha_2           .Initialise(this,                          "Alpha_2",                10,  "");
      Rf_2              .Initialise(this,                          "Rf_2",                    0,  "");
      D50c_2            .Initialise(this,  "D50", m_nSGs, DI_SG,   "D50c_2",                  7,  "L(mm)");
      DiamondD50c_2     .Initialise(this,                          "DiamondD50c_2",           7,  "L(mm)");
      };

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
