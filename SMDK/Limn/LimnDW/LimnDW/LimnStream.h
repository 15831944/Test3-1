//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __LIMNSTREAM_H_
#define __LIMNSTREAM_H_
#include "md_headers.h"
#include "limnglbl.h"
#include <vector>

//===========================================================================
//
//===========================================================================

DEFINE_SPECIEQUALITY(CLimnStream)

class CLimnStream : public MSpQualityBase
  {
  ATTACH_SPECIEQUALITY(CLimnStream);

  public:
    class CFeed
      {
      friend class CLimnStream;
      public:
        CFeed();
        void ValidateData();
        void ValidateData(CArray<double, double> &Data);
      protected:
        long             m_iSource;
        CString          m_sSrcName;
        CArray<double, double> m_DmdSGFeed;
        CArray<double, double> m_DmdSizeFeed;
        CArray<double, double> m_OreSizeFeed;
        CArray<double, double> m_Densimetrics;
      };

  public:
	  CLimnStream(TaggedObject * pNd);
	  ~CLimnStream();

    void           BuildDataFields();
    bool           ExchangeDataFields();
    bool           ValidateDataFields();

    void           ReadData();
    void           LoadFeed();
    int            LoadArrayData(LPCTSTR RangeName, double Scale, int Cols, int Rows, CArray<double,double> &Data);

    void           ConvertToMassForm(MVector & M, bool ApplyScale=true);
    void           ConvertToFracForm(MVector & M, bool ApplyScale=true);

    // SpQuality Overides
    virtual void   ExchangeSpecies(MSpQualityBase * Other);

    virtual bool   EquilRqd4HEval();// { return false; };
    virtual void   Equilibrate();

    virtual void   ZeroMass();
    virtual void   ZeroDeriv();
    virtual void   ScaleMass(long Phase, double Mult);
    virtual void   ScaleMass(CArray<int,int> &SpIds, double Mult);

    virtual void   SetMassF(MSpQualityBase * QualSet, MArray & MSet);
    virtual void   AddMassF(MSpQualityBase * QualAdd, MArray & MAdd);
    virtual void   SubMassF(MSpQualityBase * QualSub, MArray & MSub);

    virtual void   Copy(MVector &V2, MSpQualityBase * QualCopy);
    virtual void   AddDeriv(MVector &V2, MSpQualityBase * Qual2, double Sgn_, double DeltaTime);
    virtual void   AddDiscrete(MVector &V2, MSpQualityBase * Qual2, double Sgn_, double DeltaTime);

  protected:
    // CPropertyBase Overides
    long           DefinedPropertyCount();
    long           DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    DWORD          GetPropertyVisibility(long Index);
    void           GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    void           PutPropertyValue(long Index, MPropertyValue & Value);

    //Limn Indexing etc
    int                     nOreSizes()                                        { return gs_DWCfg.m_numOreSizes;                     }
    int                     nDiamondSizes()                                    { return gs_DWCfg.m_numDiamondSizes;                 }
    int                     nSGs()                                             { return gs_DWCfg.m_numSGs;                          }

    int                     iDDIndex ( int iDSz, int iOSz, int iSG )           { return gs_DWCfg.iDDIndex (iDSz, iOSz, iSG );            }
    int                     iDDLimnStreamIndex ( int iDSz, int iOSz, int iSG ) { return gs_DWCfg.iDDLimnStreamIndex ( iDSz, iOSz, iSG ); }
    int                     iODLimnStreamIndex ( int iOSz, int iSG )           { return gs_DWCfg.iODLimnStreamIndex ( iOSz, iSG );       }   
    int                     iWaterLimnStreamIndex()                            { return gs_DWCfg.iWaterLimnStreamIndex();                }
    int                     iFeSiLimnStreamIndex()                             { return gs_DWCfg.iFeSiLimnStreamIndex();                 }

    double               &  Diamond( int iDSz, int iOSz, int iSG )             { return m_Data[gs_DWCfg.iDDLimnStreamIndex ( iDSz, iOSz, iSG )]; }
    double               &  Ore( int iOSz, int iSG )                           { return m_Data[gs_DWCfg.iODLimnStreamIndex ( iOSz, iSG )];       }   
    double               &  Water()                                            { return m_Data[gs_DWCfg.iWaterLimnStreamIndex()];                }
    double               &  FeSi()                                             { return m_Data[gs_DWCfg.iFeSiLimnStreamIndex()];                 }

    int                     RowCount()                                         { return gs_DWCfg.m_nRowCount;              }
    int                     ColCount()                                         { return gs_DWCfg.m_nColCount;              }
    int                     DataBlockCount()                                   { return gs_DWCfg.m_nDataBlockCount;        }
   
    double                  OreSizesTop(int i )                                { return gs_DWCfg.m_OreSizesTop[i];         }
    double                  OreSizesBottom(int i )                             { return gs_DWCfg.m_OreSizesBottom[i];      }
    double                  MeanOreSize(int i )                                { return gs_DWCfg.m_OreSize[i];             }
    LPCTSTR                 OreSizeText(int i )                                { return gs_DWCfg.m_OreSizeText[i];         }

    double                  DiamondSizesTop(  int i )                          { return gs_DWCfg.m_DiamondSizesTop[i];     }
    double                  DiamondSizesBottom( int i )                        { return gs_DWCfg.m_DiamondSizesBottom[i];  }
    double                  MeanDiamondSize(  int i )                          { return gs_DWCfg.m_DiamondSize[i];         }
    LPCTSTR                 DiamondSizeText(int i )                            { return gs_DWCfg.m_DiamondSizeText[i];     }

    double                  SGTop(int i )                                      { return gs_DWCfg.m_SGsTop[i];              }
    double                  SGBottom(int i )                                   { return gs_DWCfg.m_SGsBottom[i];           }
    double                  MeanSG(int i )                                     { return gs_DWCfg.m_SG[i];                  }
    LPCTSTR                 SGText(int i )                                     { return gs_DWCfg.m_SGText[i];              }
    LPCTSTR                 SGTextShort(int i )                                { return gs_DWCfg.m_SGTextShort[i];         }

    double                  DiamondSG(int i )                                  { return gs_DWCfg.m_DiamondSG;              }
    double                  Revenue(int i )                                    { return gs_DWCfg.m_Revenue[i];             }
    double                  PassSizePercents( int i )                          { return gs_DWCfg.m_PassSizePercents[i-1];  }
    LPCTSTR                 PassSizePercentsText( int i )                      { return gs_DWCfg.m_PassSizePercentsText[i-1]; }

    bool                    IsLiberatedDiamond( int iDSz, int iOSz )           { return gs_DWCfg.IsLiberatedDiamond(iDSz, iOSz); }

    double                  LockedParticleSG( int iDSz, int iOSz, int iSG )    { return gs_DWCfg.LockedParticleSG(iDSz,iOSz,iSG); }

  //protected:
  //  int                     liberatedDiamondIndex( int iDSz, int iOSz    )     { return iDSz*m_numOreSizes + iOSz; }
  //  int                     lockedParticleIndex( int iDSz, int iOSz, int iSG ) { return iDSz*m_numDiamondSizes + iOSz*m_numOreSizes + iSG; }

    double                  GetResult(int iResult);

  public:
    void                    Dump(LPCTSTR Tag, DWORD What);

    double                * Data() { return &m_Data[0]; }

  protected:

    // This Data can either be in Absolute Mass Or Fractional Form. The flag Tells you
    bool                    m_bIsMassForm;
    CArray<double, double&> m_Data;
    bool                    m_bCalculate;
    CFeed                 * m_pFeed;

    CArray<double, double&> m_Results;

    static CString          sm_sBaseTag;
    static double           sm_BaseCarats;
    static double           sm_BaseRevenue;
  };

//===========================================================================
//
//===========================================================================

#endif //__LIMNPSD_H_
