#pragma once

//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - WHITEN CRUSHER
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_WhitenCrusher : public CLimn_ModelData_Base
  {
  static CLimn_ModelData_Base sm_Base;
  public:
    C_ModelParameters_DiamondWizard_WhitenCrusher(void) : CLimn_ModelData_Base(&sm_Common) 
      {
      };
    C_ModelParameters_DiamondWizard_WhitenCrusher(double* ModelParams) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      //m_pData=ModelParams;
      Initialise();
      }
    ~C_ModelParameters_DiamondWizard_WhitenCrusher(void)
      {
      };

    //	0. Flag to indicate whether Redistribution of Densimetrics is required
    //	1. Crusher CSS
    //	2-5. K1 parameters a0 - a3
    //	6-9. K2 parameters a0 - a3
    //	10.  K3 parameter  a0
    //	11-14. T10 parameters a0 - a3
    //	15-39. Array of Tn v T10 (Spreadsheet vector form - i.e. Row Major order

    static CLimn_ModelData_Common sm_Common;

    CBooleanRef redistributeDensimetrics;
    CDoubleRef  CSS;
    CVectorRef  K1_a, K2_a;
    CDoubleRef  K3_a;
    CVectorRef  T10_a;
    CMatrixRef  Tn_V_T10;

    void Initialise() 
      {
      CLimn_ModelData_Base::Initialise();

      static const double K1_a_Default[]  = { 0.0, 1.0, 3.0, 0.0};
      static const double K2_a_Default[]  = {40.0, 0.0, 0.0, 0.0};
      static const double T10_a_Default[] = {10.5, 0.0, 0.0, 0.0};
      static const double Tn_V_T10_Default[25] = 
        {0.00,	0.00,	0.00,	0.00,	0.00,
        2.80,	3.30,	5.40,	21.20,	49.60,
        5.70,	7.20,	10.30,	45.00,	74.90,
        8.10,	10.80,	15.70,	61.40,	85.20,
        10.00,	12.00,	22.00,	70.00,	95.00};

      redistributeDensimetrics.Initialise(this,                                    "redistributeDensimetrics",  true);
      CSS                     .Initialise(this,                                    "CSS",                       25,               "L(mm)");
      K1_a                    .Initialise(this, "K",      4,     DI_None,          "K1_a",                      K1_a_Default,     ""); 
      K2_a                    .Initialise(this, "K",      4,     DI_None,          "K2_a",                      K2_a_Default,     ""); 
      K3_a                    .Initialise(this,                                    "K3_a",                      2.3,              ""); 
      T10_a                   .Initialise(this, "T10",    4,     DI_None,          "T10_a",                     T10_a_Default,    ""); 
      Tn_V_T10                .Initialise(this, "TnT10",  5, 5,  DI_None,DI_None,  "Tn_V_T10",                  Tn_V_T10_Default, "");
      };

  protected:

  };
//
//				Return Data Access Class
class C_ModelReturns_DiamondWizard_WhitenCrusher: public CLimn_ModelData_Base
  {
  public:
    C_ModelReturns_DiamondWizard_WhitenCrusher(void) : CLimn_ModelData_Base(&sm_Common)
      {
      };
    C_ModelReturns_DiamondWizard_WhitenCrusher(double* ModelParams) : CLimn_ModelData_Base(&sm_Common, ModelParams) // usually called from parameter passing
      { 
      //m_pData=ModelParams;
      Initialise();
      }
    ~C_ModelReturns_DiamondWizard_WhitenCrusher(void)
      {
      };

    static CLimn_ModelData_Common sm_Common;

    CDoubleRef  FeedTPH;
    CDoubleRef  FeedP80;

    //double & FeedTPH()	{ return m_pData[0];   }; 
    //double & FeedP80()	{ return m_pData[1];   };

    //int DataCount()		{ return 2;            };

    void Initialise() 
      {
      CLimn_ModelData_Base::Initialise();

      FeedTPH        .Initialise(this,                                    "FeedTPH",                       0,               "Qm(t/h)");
      FeedP80        .Initialise(this,                                    "FeedP80",                       0,               "L(mm)");


      };

  protected:

  };
//
//				Limn callable model
C_LINKAGE DLLMODEL int _Model_DiamondWizard_WhitenCrusher (int nRows,
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
C_LINKAGE int DLLMODEL  _Error_DiamondWizard_WhitenCrusher ( int errorIndex, LPSTR errorBuffer ) ;
//
//				Parameter information
C_LINKAGE int DLLMODEL  _Parameter_DiamondWizard_WhitenCrusher ( int parameterIndex, LPSTR parameterInfoBuffer ) ;
//
//
//
