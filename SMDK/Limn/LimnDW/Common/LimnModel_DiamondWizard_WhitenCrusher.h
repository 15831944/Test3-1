#pragma once
//
//-------------------------------------------------------------------------------------------------------
//
//		DIAMOND WIZARD SPECIFIC - WHITEN CRUSHER
//
//				Parameter Access Class
class C_ModelParameters_DiamondWizard_WhitenCrusher: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelParameters_DiamondWizard_WhitenCrusher(void)
      {
      };
    ~C_ModelParameters_DiamondWizard_WhitenCrusher(void){};


    C_ModelParameters_DiamondWizard_WhitenCrusher(double* ModelParams) // usually called from parameter passing
      { 
      m_pData=ModelParams;
      }
        double & redistributeDensimetrics()	{ return m_pData[0];			 }; 
		double & CSS()						{ return m_pData[1];			 };  //	1. Crusher CSS
		double & K1_a(int i)				{ return m_pData[2+i];			 };  //	2-5. K1 parameters a0 - a3
		double & K2_a(int i)				{ return m_pData[6+i];			 };  //	6-9. K2 parameters a0 - a3
		double & K3_a()						{ return m_pData[10];			 };  //	10.  K3 parameter  a0
		double & T10_a(int i)				{ return m_pData[11+i];			 };  //	11-14. T10 parameters a0 - a3
		double & Tn_V_T10(int i, int j)		{ return m_pData[15 + i*5 + j];  };  //	15-39. Array of Tn v T10 (Spreadsheet vector form - i.e. Row Major order

	    int DataCount()						{ return 40;               };

		bool DoRedistributeDensimetrics()	{ return redistributeDensimetrics() > 0.5; }; 

		double* pTn_V_T10()					{ return &m_pData[15]; } ; 
		

#ifdef LIMNDW
    void Initialise() 
      {

		Allocate();

		redistributeDensimetrics() = 1.0 ;

		CSS()       = 25;
        K1_a(0)     = 0.0;
        K1_a(1)     = 1.0;
        K1_a(2)     = 0.0;
        K2_a(0)     = 40.0;
        K2_a(1)     = 0.0;
        K2_a(2)     = 0.0;
		K3_a()		= 2.30 ;
		T10_a(0)	= 10.50 ;
		T10_a(1)	= 0.0 ;
		T10_a(2)	= 0.0 ;

		double aaa[25] = 
        {0.00,	0.00,	0.00,	0.00,	0.00,
			   2.80,	3.30,	5.40,	21.20,	49.60,
			   5.70,	7.20,	10.30,	45.00,	74.90,
			   8.10,	10.80,	15.70,	61.40,	85.20,
			   10.00,	12.00,	22.00,	70.00,	95.00};
		for (int i=0; i<25; i++)
			pTn_V_T10()[i] = aaa[i] ;
	
	  };
#endif

  protected:

  };
//
//				Return Data Access Class
class C_ModelReturns_DiamondWizard_WhitenCrusher: public CLimn_ModelData_AbstractBase
  {
  public:
    C_ModelReturns_DiamondWizard_WhitenCrusher(void)
      {
      };
    ~C_ModelReturns_DiamondWizard_WhitenCrusher(void){};


    C_ModelReturns_DiamondWizard_WhitenCrusher(double* ModelParams) // usually called from parameter passing
      { 
      m_pData=ModelParams;
      }
        double & FeedTPH()	{ return m_pData[0];   }; 
		double & FeedP80()	{ return m_pData[1];   };

	    int DataCount()		{ return 2;            };

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
