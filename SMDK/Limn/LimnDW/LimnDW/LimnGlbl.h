//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __LIMNGLBL_H_
#define __LIMNGLBL_H_
#include "md_headers.h"
#include <vector>

//===========================================================================
//
//===========================================================================

//////////////////////////////////
//
//
//  CDiamondWizardConfiguration.h
//
//    . stores data from the DiamondWizard Configuration page
//    . calculates derived data using that information
//    . returns configuration data to the other DiamondWizard
//      functions as required
//                  
//
//___________________________________________________________________________________________
//
//
// Copyright 2000 David Wiseman Pty Ltd
//
//___________________________________________________________________________________________

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

/////////////////////////////////////////////////////////////////////////////////////
//
//
//           CDiamondWizardConfiguration class - data for a single 
//        Limn:DataWizard ConfigurationSheet
//                              
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
typedef void * LPXLOPER;

class CDiamondWizardConfiguration : public CObject
  {
  friend class CLimnStream;

  DECLARE_DYNAMIC(CDiamondWizardConfiguration)

  protected:

    // Attributes
  public:

    CDiamondWizardConfiguration( void );                // Constructor

    // Implementation
  public:
    virtual ~CDiamondWizardConfiguration( void );      // Destructor

    // Operations
  public:           
    void FreeLocalData( void );                        // Tidy up local data allocations from previous calls to Initialise

    int Initialise ( LPXLOPER OreSizeData,             // Initialisation - set up calculated data
      LPXLOPER DiamondSizeData,
      LPXLOPER OreSGData,
      LPXLOPER DiamondSG,
      LPXLOPER Revenue,
      LPXLOPER LiberationMatrix,
      LPXLOPER PassSizePercents);

	  int InitialiseReferenceDensimetrics(LPXLOPER ReferenceDensimetrics) ; // special load of reference densimetrics for DLL models

    bool                    Initialised()                       { return m_bInitialised; }
    bool                    Initialise();//LPCTSTR Fn);

    //           Helper functions to return various Configuration info
    int                     nOreSizes()                                        { return m_numOreSizes;   }
    int                     nDiamondSizes()                                    { return m_numDiamondSizes; }
    int                     nSGs()                                             { return m_numSGs;       }
    int                     nOSz()                                             { return m_numOreSizes;   }
    int                     nDSz()                                             { return m_numDiamondSizes; }


                            // index into linear data space representing the 2D
                            // spreadsheet representing the 2D ore mass data
    int                     iODIndex ( int iOSz, int iSG );
                            // index into linear data space representing the 2D
                            // spreadsheet representing the 3D diamond deportment data
    int                     iDDIndex ( int iDSz, int iOSz, int iSG );
                            // Diamond Data Index into linear data space representing the Limn Stream data,
                            // i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
    int                     iDDLimnStreamIndex ( int iDSz, int iOSz, int iSG );
                            // Ore Data Index into linear data space representing the Limn Stream data,
                            // i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
    int                     iODLimnStreamIndex ( int iOSz, int iSG );
													  // Water Index into linear data space representing the Limn Stream data,
													  // i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
	  int		                  iWaterLimnStreamIndex() ;
													  // FeSi Index into linear data space representing the Limn Stream data,
													  // i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
	  int		                  iFeSiLimnStreamIndex() ;

    // Basic indices into the raw data array
    int                     RowCount()                                         { return m_nRowCount;       }
    int                     ColCount()                                         { return m_nColCount;       }
    int                     DataBlockCount()                                   { return m_nDataBlockCount; }
    int                     OreBlockCount()                                    { return m_numOreSizes*m_numSGs; }
    int                     DiamondBlockCount()                                { return m_numDiamondSizes*m_numOreSizes*m_numSGs; }

    double                  OreSizesTop(int i )                                { return m_OreSizesTop[i];         }
    double                  OreSizesBottom(int i )                             { return m_OreSizesBottom[i];      }
    double                  MeanOreSize(int i )                                { return m_OreSize[i];             }
    double                  OreSizesTopSI(int i )                              { return m_OreSizesTopSI[i];         }
    double                  OreSizesBottomSI(int i )                           { return m_OreSizesBottomSI[i];      }
    double                  MeanOreSizeSI(int i )                              { return m_OreSizeSI[i];             }
    LPCTSTR                 OreSizeText(int i )                                { return m_OreSizeText[i];         }
	
    double                * pMeanOreSize()	                                   { return &m_OreSize[0];            }
    double                * pMeanOreSizeSI()	                                 { return &m_OreSizeSI[0];          }

    double                  DiamondSizesTop(  int i )                          { return m_DiamondSizesTop[i];     }
    double                  DiamondSizesBottom( int i )                        { return m_DiamondSizesBottom[i];  }
    double                  MeanDiamondSize(  int i )                          { return m_DiamondSize[i];         }
    double                  DiamondSizesTopSI(  int i )                        { return m_DiamondSizesTopSI[i];     }
    double                  DiamondSizesBottomSI( int i )                      { return m_DiamondSizesBottomSI[i];  }
    double                  MeanDiamondSizeSI(  int i )                        { return m_DiamondSizeSI[i];       }
    LPCTSTR                 DiamondSizeText(int i )                            { return m_DiamondSizeText[i];     }

    double                  SGsTop(int i )                                     { return m_SGsTop[i];              }
    double                  SGsBottom(int i )                                  { return m_SGsBottom[i];           }
    double                  MeanSG(int i )                                     { return m_SG[i];                  }
    LPCTSTR                 SGText(int i )                                     { return m_SGText[i];              }
    LPCTSTR                 SGTextShort(int i )                                { return m_SGTextShort[i];         }

    double                  DiamondSG()                                        { return m_DiamondSG;              }
    double                  Revenue(int i )                                    { return m_Revenue[i];             }
    double                  PassSizePercents( int i )                          { return m_PassSizePercents[i];    }
    LPCTSTR                 PassSizePercentsText( int i )                      { return m_PassSizePercentsText[i]; }

    bool                    IsLiberatedDiamond( int iDSz, int iOSz )           { return m_Liberation[ liberatedDiamondIndex(iDSz, iOSz) ]; }

    double                  LockedParticleSG( int iDSz, int iOSz, int iSG )    { return m_LockedParticleSG[ lockedParticleIndex(iDSz,iOSz,iSG) ]; }

  	double	                ReferenceDensimetrics( int iOSz, int iSG )         { return m_ReferenceDensimetrics[ referenceDensimetricIndex(iOSz, iSG) ] ; }

    int                     ID( void )                                         { return m_thisConfigID;           }
    void                    SetID( int theNewID )                              { m_thisConfigID = theNewID;       }

    LPCTSTR                 OreSpNames(int iSG)                                { return m_OreSpNames[iSG];   };
    int                     OreSpIds(int iSG)                                  { return m_OreSpIds[iSG];     };
    long                    OreSpPhases(int iSG)                               { return m_OreSpPhases[iSG];  };
                                                
    LPCTSTR                 DiamondSpName()                                    { return m_DiamondSpName;     };
    int                     DiamondSpId()                                      { return m_DiamondSpId;       };
    long                    DiamondPhase()                                     { return m_DiamondPhase;      };
                                                
    LPCTSTR                 WaterSpName()                                      { return m_WaterSpName;       };
    int                     WaterSpId()                                        { return m_WaterSpId;         };
    long                    WaterPhase()                                       { return m_WaterPhase;        };
                                                
    LPCTSTR                 FeSiSpName()                                       { return m_FeSiSpName;        };
    int                     FeSiSpId()                                         { return m_FeSiSpId;          };
    long                    FeSiPhase()                                        { return m_FeSiPhase;         };

    bool                    UseLimnScaling()                                   { return false;  }  // needs work
  protected:
    int                     liberatedDiamondIndex( int iDSz, int iOSz    )     { return iDSz*m_numOreSizes + iOSz; }
    int                     lockedParticleIndex( int iDSz, int iOSz, int iSG ) { return iDSz*m_numDiamondSizes + iOSz*m_numOreSizes + iSG; }
  	int	                  	referenceDensimetricIndex( int iOSz, int iSG )     { return iOSz*m_numSGs + iSG ; }
    // 
    // Data
  protected:
    bool                      m_bInitialised;


    int                       m_numOreSizes;   
    int                       m_numDiamondSizes; 
    int                       m_numSGs;       

    int                       m_numLimnStreamDataColumns;

    int                       m_nRowCount;       
    int                       m_nColCount;       
    int                       m_nDataBlockCount; 

    CArray<double, double>    m_OreSizesTop;
    CArray<double, double>    m_OreSizesBottom;
    CArray<double, double>    m_OreSize;
    CArray<double, double>    m_OreSizesTopSI;
    CArray<double, double>    m_OreSizesBottomSI;
    CArray<double, double>    m_OreSizeSI;
    CArray<CString, CString&> m_OreSizeText;

    CArray<double, double>    m_DiamondSizesTop;
    CArray<double, double>    m_DiamondSizesBottom;
    CArray<double, double>    m_DiamondSize;
    CArray<double, double>    m_DiamondSizesTopSI;
    CArray<double, double>    m_DiamondSizesBottomSI;
    CArray<double, double>    m_DiamondSizeSI;
    CArray<CString, CString&> m_DiamondSizeText;

    CArray<double, double>    m_SGsTop;
    CArray<double, double>    m_SGsBottom;
    CArray<double, double>    m_SG;
    CArray<CString, CString&> m_SGText;
    CArray<CString, CString&> m_SGTextShort;

    double                    m_DiamondSG;
    CArray<double, double>    m_Revenue;
    CString                   m_RevenueSym;
                              
    double                    m_LiberationFactor;
    CArray<bool, bool>        m_Liberation;

    CArray<double, double>    m_ReferenceDensimetrics;
    CArray<double, double>    m_ReferenceDensimetricsSI;

    CArray<double, double>    m_PassSizePercents;
    CArray<CString, CString&> m_PassSizePercentsText;

    CArray<double, double>    m_LockedParticleSG  ;


    CArray<CString, CString&> m_OreSpNames;
    CArray<int, int&>         m_OreSpIds;
    CArray<long, long&>       m_OreSpPhases;

    CString                   m_DiamondSpName;
    int                       m_DiamondSpId;
    long                      m_DiamondPhase;

    CString                   m_WaterSpName;
    int                       m_WaterSpId;
    long                      m_WaterPhase;

    CString                   m_FeSiSpName;
    int                       m_FeSiSpId;
    long                      m_FeSiPhase;

    CArray<int, int>          m_SpIds;
    CArray<long, long>        m_SpPhases;
    CArray<double, double>    m_MassFactor;

    CArray<int, int>          m_SeqIndex;  // A Sequense of index numbers of only those that need calcs

    int                       m_thisConfigID;

  };


/////////////////////////////////////////////////////////////////////////////////////
//
//
//           CDiamondWizardConfigurations class - Collection Class to hold multiple
//       instances of CDiamondWizardConfiguration objects 
//                              
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//

class CDiamondWizardConfigurations
  {

  protected:

    // Attributes
  public:

    CDiamondWizardConfigurations( void );            // Constructor

    // Implementation
  public:
    virtual ~CDiamondWizardConfigurations( void );        // Destructor

    // Operations
  public:           
    //           Helper functions to handle Configuration ID
    int       ID()          { return m_nextConfigID; } 
    void      ResetID()     { m_nextConfigID = 0; }
    int       IncrementID();

    //           Helper functions to handle the collection
    CDiamondWizardConfiguration*  Update( DWORD configSheetID );

    CDiamondWizardConfiguration*  Configuration( int configID );

    // Data
  protected:
    int    m_nextConfigID  ;
#ifdef NOT_DEFINED 

    // a map of all DiamondWizard Configurations available to this XLL
    // keyed by Excel Sheet ID
    CMap <DWORD, DWORD, CDiamondWizardConfiguration*, CDiamondWizardConfiguration*& > m_ExcelSheetIDToConfiguration;    
    // a map of all DiamondWizard Configurations available to this XLL
    // keyed by configuration ID
    CMap <int, int,  CDiamondWizardConfiguration*, CDiamondWizardConfiguration*& > m_configIDToConfiguration;
#endif
  };


extern CDiamondWizardConfiguration gs_DWCfg;
extern CDiamondWizardConfigurations gs_DWCfgs;

extern int LoadArrayData(LPCTSTR Root, LPCTSTR File, int iSource, LPCTSTR RangeName, double Scale, int Cols, int Rows, CArray<double,double> &Data);//, MLog & Log);


#ifdef NOT_DEFINED

class CLimnGlobals
  {
  friend class CLimnStream;

  //class CInterval
  //  {
  //  public:
  //    CInterval();
  //    CInterval(double Start, double End, LPCTSTR Text);

  //    double   m_Start;
  //    double   m_End;
  //    double   m_Mean;
  //    CString  m_Text;
  //  };

  public:
    CLimnGlobals();
    ~CLimnGlobals();

    bool       Initialise();
    bool       Initialised()                  { return m_bInitialised;   }


    //long       OreSGCount() const             { return m_OreSG.GetCount();     }
    //long       DmdSzCount() const             { return m_DmdSz.GetCount(); }
    //long       OreSzCount() const             { return m_OreSz.GetCount();     }

    //double     DmdSG()                        { return m_DmdSG;      }

    //double     OreSGStart(int Interval)       { return m_OreSG[Interval].m_Start; };
    //double     OreSGEnd(int Interval)         { return m_OreSG[Interval].m_End;   };
    //double     OreSGMean(int Interval)        { return m_OreSG[Interval].m_Mean;  };
    //LPCTSTR    OreSGText(int Interval)        { return m_OreSG[Interval].m_Text;  };

    //double     OreSzStart(int Interval)       { return m_OreSz[Interval].m_Start; };
    //double     OreSzEnd(int Interval)         { return m_OreSz[Interval].m_End;   };
    //double     OreSzMean(int Interval)        { return m_OreSz[Interval].m_Mean;  };
    //LPCTSTR    OreSzText(int Interval)        { return m_OreSz[Interval].m_Text;  };

    //double     DmdSzStart(int Interval)       { return m_DmdSz[Interval].m_Start; };
    //double     DmdSzEnd(int Interval)         { return m_DmdSz[Interval].m_End;   };
    //double     DmdSzMean(int Interval)        { return m_DmdSz[Interval].m_Mean;  };
    //LPCTSTR    DmdSzText(int Interval)        { return m_DmdSz[Interval].m_Text;  };

    //long       OreStorageSize()               { return OreSGCount()*OreSzCount(); }
    //long       DmdStorageSize()               { return OreSGCount()*OreSzCount()*DmdSzCount(); }


  protected:
    bool       m_bInitialised;

    //double     m_DmdSG;
    //                                                               
    //CArray<CInterval, CInterval&> m_OreSG;
    //CArray<CInterval, CInterval&> m_OreSz;
    //CArray<CInterval, CInterval&> m_DmdSz;

  };

//===========================================================================
//
//===========================================================================

extern CLimnGlobals gs_LimnGlbl;

#endif

//===========================================================================
//
//=========================================================================== 

#endif //__LIMNPSD_H_
