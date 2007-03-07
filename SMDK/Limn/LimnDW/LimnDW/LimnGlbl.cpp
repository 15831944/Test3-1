//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "math.h"
#include "md_headers.h"
#define __LIMNGLBL_CPP
#include "limnglbl.h"
//#pragma comment(lib, "rpcrt4.lib")

const int DoDbg = 0;

/////////////////////////////////////////////////////////////////////////////
// Class CDiamondWizardConfiguration
//
IMPLEMENT_DYNAMIC(CDiamondWizardConfiguration, CObject)
//
/////////////////////////////////////////////////////////////////////////////////////
//
CDiamondWizardConfiguration::CDiamondWizardConfiguration( void )						// Constructor
  {
  m_bInitialised = false;

  m_numOreSizes       = 0;   
  m_numDiamondSizes   = 0; 
  m_numSGs            = 0;       

  m_numLimnStreamDataColumns  = 0;

  m_nRowCount       = 0;       
  m_nColCount       = 0;       
  m_nDataBlockCount = 0; 

  SetID( -1 ) ;
  }
//
//           Helper/Calculation functions to return various DiamondWizard info
//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//                                 

int CDiamondWizardConfiguration::Initialise( LPXLOPER OreSizeData,             // Initialisation - set up calculated data
                                            LPXLOPER DiamondSizeData,
                                            LPXLOPER SGData,
                                            LPXLOPER DiamondSG,
                                            LPXLOPER Revenue,
                                            LPXLOPER LiberationMatrix,
                                            LPXLOPER PassSizePercents )

  {
#ifdef NOT_DEFINED

  int iDSz ;
  int iOSz ;
  int iSG ;

  int idx ;

  FreeLocalData() ;						// tidy up from any previous calls to this routine

  // bring in the ore size data
  g->Excel.fn( xlCoerce, xlp(OreSizeData), xl(xltypeMulti) ) ;
  if (g->Excel.resultXltype() != xltypeMulti) return xlerrRef ;	// valid reference

  m_numOreSizes = g->Excel.resultRows() ;
  m_OreSizesTop    = new double[m_numOreSizes] ;							// define local arraya
  m_OreSizesBottom = new double[m_numOreSizes] ;
  m_OreSize        = new double[m_numOreSizes] ;		

  if (g->Excel.resultColumns() == 1)
    {
    for ( iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
      {
      if (g->Excel.resultArrayXltype(iOSz) == xltypeNum )
        {
        m_OreSizesBottom[iOSz] = 0.0 ;
        m_OreSizesTop[iOSz] = 0.0 ;
        m_OreSize[iOSz] = g->Excel.resultArrayNum(iOSz) ;		// get the data for each element
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }
  else if (g->Excel.resultColumns() == 3)
    {
    idx = 0 ;
    for ( iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
      {
      if (g->Excel.resultArrayXltype(iOSz) == xltypeNum )
        {
        m_OreSizesBottom[iOSz] = g->Excel.resultArrayNum(idx++) ;		// get the data for each element
        m_OreSizesTop[iOSz] = g->Excel.resultArrayNum(idx++) ;
        m_OreSize[iOSz] = g->Excel.resultArrayNum(idx++) ;
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }
  else
    {
    return xlerrRef ;			// must be 1 or 3 columns wide
    }

  // bring in the diamond size data
  g->Excel.fn( xlCoerce, xlp(DiamondSizeData), xl(xltypeMulti) ) ;
  if (g->Excel.resultXltype() != xltypeMulti) return xlerrRef ;	// valid reference

  m_numDiamondSizes = g->Excel.resultRows() ;
  m_DiamondSizesTop    = new double[m_numDiamondSizes] ;							// define local arrays
  m_DiamondSizesBottom = new double[m_numDiamondSizes] ;
  m_DiamondSize        = new double[m_numDiamondSizes] ;

  if (g->Excel.resultColumns() == 1)
    {
    for ( iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
      {
      if (g->Excel.resultArrayXltype(iDSz) == xltypeNum )
        {
        m_DiamondSizesBottom[iDSz] = 0.0 ;
        m_DiamondSizesTop[iDSz] = 0.0 ;
        m_DiamondSize[iDSz] = g->Excel.resultArrayNum(iDSz) ;	// get the data for each element
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }
  else if (g->Excel.resultColumns() == 3)
    {
    idx = 0 ;
    for ( iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
      {
      if (g->Excel.resultArrayXltype(iDSz) == xltypeNum )
        {
        m_DiamondSizesBottom[iDSz] = g->Excel.resultArrayNum(idx++) ;		// get the data for each element
        m_DiamondSizesTop[iDSz] = g->Excel.resultArrayNum(idx++) ;
        m_DiamondSize[iDSz] = g->Excel.resultArrayNum(idx++) ;
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }
  else
    {
    return xlerrRef ;			// must be 1 or 3 columns wide
    }

  // bring in the SG data
  g->Excel.fn( xlCoerce, xlp(SGData), xl(xltypeMulti) ) ;
  if (g->Excel.resultXltype() != xltypeMulti) return xlerrRef ;	// valid reference

  m_numSGs = g->Excel.resultRows() ;
  m_SGsTop = new double[m_numSGs] ;								// define local arrays
  m_SGsBottom = new double[m_numSGs] ;
  m_SG = new double[m_numSGs] ;

  if (g->Excel.resultColumns() == 1)
    {
    for ( iSG = 0 ; iSG < m_numSGs ; iSG++ )
      {
      if (g->Excel.resultArrayXltype(iSG) == xltypeNum )
        {
        m_SGsBottom[iSG] = 0.0 ;
        m_SGsTop[iSG] = 0.0 ;
        m_SG[iSG] = g->Excel.resultArrayNum(iSG) ;				// get the data for each element
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }
  else if (g->Excel.resultColumns() == 3)
    {
    idx = 0 ;
    for ( iSG = 0 ; iSG < m_numSGs ; iSG++ )
      {
      if (g->Excel.resultArrayXltype(iSG) == xltypeNum )
        {
        m_SGsBottom[iSG] = g->Excel.resultArrayNum(idx++) ;				// get the data for each element
        m_SGsTop[iSG] = g->Excel.resultArrayNum(idx++) ;	
        m_SG[iSG] = g->Excel.resultArrayNum(idx++) ;	
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }
  else
    {
    return xlerrRef ;			// must be 1 or 3 columns wide
    }


  // precalculate something used a lot in indexing innto the array
  m_numLimnStreamDataColumns = m_numSGs + m_numDiamondSizes * ( INT((m_numSGs+1)/2 ) ) ;


  // bring in the diamond SG
  g->Excel.fn( xlCoerce, xlp(DiamondSG), xl(xltypeNum) ) ;
  if (g->Excel.resultXltype() != xltypeNum) return xlerrNum ;		// valid number
  m_DiamondSG = g->Excel.resultNum() ;							// get it


  // bring in the Revenue data
  g->Excel.fn( xlCoerce, xlp(Revenue), xl(xltypeMulti) ) ;
  if (g->Excel.resultXltype() != xltypeMulti) return xlerrRef ;	// valid reference
  if (g->Excel.resultColumns() != 1) return xlerrRef ;			// only 1 column wide
  if (g->Excel.resultRows() != m_numDiamondSizes) return xlerrRef ;	// and as long as diamond sizes

  m_Revenue = new double[m_numDiamondSizes] ;							// define a local array
  for ( iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
    {
    if (g->Excel.resultArrayXltype(iDSz) == xltypeNum )
      {
      m_Revenue[iDSz] = g->Excel.resultArrayNum(iDSz) ;		// get the data for each element
      }
    else
      {
      return xlerrNum ;										// error return, range has non number
      }                                                                               
    }

  // bring in the Liberation data
  g->Excel.fn( xlCoerce, xlp(LiberationMatrix), xl(xltypeMulti) ) ;
  if (g->Excel.resultXltype() != xltypeMulti) return xlerrRef ;	// valid reference
  if (g->Excel.resultColumns() != m_numDiamondSizes) return xlerrRef ;	// as wide as the diamond sizes
  if (g->Excel.resultRows() != m_numOreSizes) return xlerrRef ;			// and as long as ore sizes

  m_Liberation = new bool[m_numDiamondSizes*m_numOreSizes] ;				// define a local array
  double aValue ;
  bool   bValue ;
  int ida = 0 ;
  idx = 0 ;
  for ( iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
    {
    for ( iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
      {
      if (g->Excel.resultArrayXltype(ida) == xltypeNum )
        {
        aValue = g->Excel.resultArrayNum(ida++) ;					// get the data for each element

        bValue = false ;
        if ( aValue > 0.5 ) bValue = true ;

        idx = liberatedDiamondIndex( iDSz, iOSz ) ;
        m_Liberation[idx] = bValue ;
        }
      else
        {
        return xlerrNum ;										// error return, range has non number
        }                                                                               
      }
    }

  // bring in the Passing Size Percents
  g->Excel.fn( xlCoerce, xlp(PassSizePercents), xl(xltypeMulti) ) ;
  if (g->Excel.resultXltype() != xltypeMulti) return xlerrRef ;	// valid reference
  if (g->Excel.resultColumns() != 1) return xlerrRef ;			// single column
  if (g->Excel.resultRows() != 3) return xlerrRef ;				// 3 rows

  m_PassSizePercents = new double[3] ;							// define a local array
  for ( int iPSP = 0 ; iPSP < 3 ; iPSP++ )
    {
    if (g->Excel.resultArrayXltype(iPSP) == xltypeNum )
      {
      m_PassSizePercents[iPSP] = g->Excel.resultArrayNum(iPSP) ;		// get the data for each element
      }
    else
      {
      return xlerrNum ;										// error return, range has non number
      }                                                                               
    }


  // Calculate locked diamond SGs
  double oreVolume ;
  double diamondVolume ;
  double particleOreMass ;
  double particleDiamondMass ;
  double theSG ;
  double volK = (4 / 3) * 3.1415912 ;
  // Set up SG array
  m_LockedParticleSG = new double[m_numDiamondSizes*m_numOreSizes*m_numSGs] ;

  for ( iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
    {
    for ( iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
      {
      for ( iSG = 0 ; iSG < m_numSGs ; iSG++ )
        {
        oreVolume = volK * pow((m_OreSize[iOSz] / 2), 3) ;
        diamondVolume = volK * pow((m_DiamondSize[iDSz] / 2), 3) ;
        particleOreMass = m_SG[iSG] * (oreVolume - diamondVolume) ;
        particleDiamondMass = m_DiamondSG * diamondVolume ;
        theSG =  (particleOreMass + particleDiamondMass) / oreVolume ;
        if ( theSG > m_DiamondSG ) theSG = m_DiamondSG ;

        m_LockedParticleSG[ lockedParticleIndex(iDSz, iOSz, iSG) ] = theSG ;
        }
      }
    }

  return xlerrNull ;
#else
  return -1;
#endif
  }

//===========================================================================
//
//
//
//===========================================================================

bool CDiamondWizardConfiguration::Initialise()//LPCTSTR Fn)
  {
  if (m_bInitialised)
    return m_bInitialised;

  CString PFFn(CfgFiles());
  PFFn+="LimnCfg";//Fn;// "LimnPSD.ini";
  if (PFFn.Right(4).CompareNoCase(".INI")!=0)
    PFFn+=".ini";

  CProfINIFile PF(PFFn);

  m_DiamondSG  = PF.RdDouble("General", "DiamondSG", 3.515);
  m_RevenueSym  = PF.RdStr("General", "RevenueSym", "?");
  m_LiberationFactor = PF.RdDouble("General", "LiberationFactor", 1.0);

  m_PassSizePercents.SetSize(3);
  m_PassSizePercents[0]=PF.RdDouble("General", "PassSizePercent.0", 25.0)*0.01;
  m_PassSizePercents[1]=PF.RdDouble("General", "PassSizePercent.1", 50.0)*0.01;
  m_PassSizePercents[2]=PF.RdDouble("General", "PassSizePercent.2", 75.0)*0.01;

  m_PassSizePercentsText.SetSize(m_PassSizePercents.GetCount());
  for (int i=0; i<m_PassSizePercents.GetCount(); i++)
    m_PassSizePercentsText[i].Format("PassingSize-%i%%", int(m_PassSizePercents[i]*100.0+0.5));

  CString Tg, Line, PrevTkn0;
  LPCTSTR SeriesSections[] = {"OreSG", "OreSize", "DiamondSize"};
  for (int iSeries=0; iSeries<3; iSeries++)
    {
    for (int iInt=0; ; iInt++)
      {
      Tg.Format("I%04i", iInt);
      Line = PF.RdStr(SeriesSections[iSeries], Tg, "");
      if (Line.GetLength()==0)
        break;
      CSVColArray Tkns;
      int N=ParseCSVTokens(Line.GetBuffer(), Tkns);
      if (N<=0)
        break;

      if (iInt>0)
        {
        double Top    = SafeAtoF(PrevTkn0);
        double Bottom = SafeAtoF(Tkns[0]);
        if (iSeries==0)
          Exchange(Top, Bottom);
        double GeoMean = Bottom>0.0 ? Sqrt(Top*Bottom):Top/Sqrt(2.0);
        double ArtMean = Bottom>0.0 ? 0.5*(Top+Bottom):Top/2.0;
        CString Text  = Tkns[1];
        switch (iSeries)
          {
          case 0: 
            {
            m_SGsTop.Add(Top);
            m_SGsBottom.Add(Bottom);
            m_SG.Add(ArtMean);
            m_SGText.Add(Text);
            Text.Format("%.2f", ArtMean);
            m_SGTextShort.Add(Text);
            break;
            }
          case 1: 
            {
            m_OreSizesTop.Add(Top);
            m_OreSizesBottom.Add(Bottom);
            m_OreSize.Add(GeoMean);
            m_OreSizesTopSI.Add(Top*0.001);
            m_OreSizesBottomSI.Add(Bottom*0.001);
            m_OreSizeSI.Add(GeoMean*0.001);
            m_OreSizeText.Add(Text);
            break;
            }
          case 2:
            {
            m_DiamondSizesTop.Add(Top);
            m_DiamondSizesBottom.Add(Bottom);
            m_DiamondSize.Add(GeoMean);
            m_DiamondSizesTopSI.Add(Top*0.001);
            m_DiamondSizesBottomSI.Add(Bottom*0.001);
            m_DiamondSizeSI.Add(GeoMean*0.001);
            m_DiamondSizeText.Add(Text);
            m_Revenue.Add(SafeAtoF(Tkns[2], 0.0));
            break;
            }
          }
        }
      PrevTkn0=Tkns[0];
      }
    }
  
  m_numOreSizes     = m_OreSize.GetCount();
  m_numDiamondSizes = m_DiamondSize.GetCount(); 
  m_numSGs          = m_SG.GetCount();       

  // precalculate something used a lot in indexing innto the array
  m_numLimnStreamDataColumns = m_numSGs + m_numDiamondSizes * ( INT((m_numSGs+1)/2 ) ) ;

  m_Liberation.SetSize(m_numDiamondSizes*m_numOreSizes);				// define a local array
  for (int iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
    {
    for (int iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
      {
      bool   bValue = m_DiamondSize[iDSz]>=m_LiberationFactor*m_OreSize[iOSz];
      m_Liberation[liberatedDiamondIndex( iDSz, iOSz )] = bValue ;
      }
    }


  LoadArrayData(CfgFiles(), "Default", 0, "Densimetrics", 1.0, m_numSGs, m_numOreSizes, m_ReferenceDensimetrics);//, Log);

  // Do a Whole bunch of checks !!!!!!!!!!

  // Calculate locked diamond SGs
  double oreVolume ;
  double diamondVolume ;
  double particleOreMass ;
  double particleDiamondMass ;
  double theSG ;
  double volK = (4 / 3) * 3.1415912 ;
  // Set up SG array
  m_LockedParticleSG.SetSize(m_numDiamondSizes*m_numOreSizes*m_numSGs);

  for (int iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
    {
    for (int iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
      {
      for (int iSG = 0 ; iSG < m_numSGs ; iSG++ )
        {
        oreVolume = volK * pow((m_OreSize[iOSz] / 2), 3) ;
        diamondVolume = volK * pow((m_DiamondSize[iDSz] / 2), 3) ;
        particleOreMass = m_SG[iSG] * (oreVolume - diamondVolume) ;
        particleDiamondMass = m_DiamondSG * diamondVolume ;
        theSG =  (particleOreMass + particleDiamondMass) / oreVolume ;
        if ( theSG > m_DiamondSG ) theSG = m_DiamondSG ;

        m_LockedParticleSG[ lockedParticleIndex(iDSz, iOSz, iSG) ] = theSG ;
        }
      }
    }

  m_nRowCount       = (m_numOreSizes*2);
  m_nColCount       = (m_numSGs + m_numDiamondSizes * ((m_numSGs+1)/2));
  m_nDataBlockCount = m_nRowCount * m_nColCount;

  // Specie IDs etc
  m_OreSpNames.SetSize(nSGs());
  m_OreSpIds.SetSize(nSGs());
  m_OreSpPhases.SetSize(nSGs());

  CString Name;
  int id;
  for (int iSG=0; iSG<nSGs(); iSG++)
    {
    Tg.Format("Ore%04i", iSG);
    Name=PF.RdStr("SpecieNames", Tg, "?");
    Name.Trim();
    id = gs_MVDefn.Lookup(Name);
    if (id>=0)
      {
      m_OreSpNames[iSG]   = Name;
      m_OreSpIds[iSG]     = id;
      m_OreSpPhases[iSG]  = gs_MVDefn[id].Phase();
      }
    else
      {
      _asm int 3; //LogError
      }
    }

  Name=PF.RdStr("SpecieNames", "Diamond", "?");
  Name.Trim();
  id = gs_MVDefn.Lookup(Name);
  if (id>=0)
    {
    m_DiamondSpName = Name;
    m_DiamondSpId   = id;
    m_DiamondPhase  = gs_MVDefn[id].Phase();
    }
  else
    {
    _asm int 3; //LogError
    }

  Name=PF.RdStr("SpecieNames", "Water", "?");
  Name.Trim();
  id = gs_MVDefn.Lookup(Name);
  if (id>=0)
    {
    m_WaterSpName   = Name;
    m_WaterSpId     = id;
    m_WaterPhase    = gs_MVDefn[id].Phase();
    }
  else
    {
    _asm int 3; //LogError
    }

  Name=PF.RdStr("SpecieNames", "FeSi", "?");
  Name.Trim();
  id = gs_MVDefn.Lookup(Name);
  if (id>=0)
    {
    m_FeSiSpName    = Name;
    m_FeSiSpId      = id;;
    m_FeSiPhase     = gs_MVDefn[id].Phase();
    }
  else
    {
    _asm int 3; //LogError
    }

  m_SpIds.SetSize(m_nDataBlockCount);
  m_SpPhases.SetSize(m_nDataBlockCount);


  for (int i=0; i<m_nDataBlockCount; i++)
    {
    m_SpIds[i]=-1;
    m_SpPhases[i]=0;
    }

  m_SeqIndex.Add(iWaterLimnStreamIndex());
  m_SpIds[iWaterLimnStreamIndex()]    = m_WaterSpId;
  m_SpPhases[iWaterLimnStreamIndex()] = m_WaterPhase;

  m_SeqIndex.Add(iFeSiLimnStreamIndex());
  m_SpIds[iFeSiLimnStreamIndex()]    = m_FeSiSpId;
  m_SpPhases[iFeSiLimnStreamIndex()] = m_FeSiPhase;

  m_MassFactor.SetSize(gs_MVDefn.Count());
  for (int i=0; i<gs_MVDefn.Count(); i++)
    m_MassFactor[i]=0.001*3600; // everything to Tons/h;

  m_MassFactor[m_DiamondSpId]=5000.0*3600; // diamonds in Carats/h;

  for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
    {
    for (int iSG = 0 ; iSG < nSGs(); iSG++ )
      {
      int i = iODLimnStreamIndex(iOSz, iSG);
      m_SeqIndex.Add(i);
      m_SpIds[i]    = m_OreSpIds[iSG];
      m_SpPhases[i] = m_OreSpPhases[iSG];
      }
    }

  for (int iSG = 0 ; iSG < nSGs(); iSG++ )
    {
    for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
      {
      for (int iDSz = 0 ; iDSz< nDiamondSizes(); iDSz++ )
        {
        int i = iDDLimnStreamIndex(iDSz, iOSz, iSG);
        m_SeqIndex.Add(i);
        m_SpIds[i]    = m_DiamondSpId;
        m_SpPhases[i] = m_DiamondPhase;
        }
      }
    }

  m_bInitialised = true;

  if(DoDbg)
    {
    MDebug Dbg;
    Dbg.PrintLn("Limn Config ===================================================");

    Dbg.PrintLn("");
    Dbg.PrintLn("DiamondSG           : %10.3f", m_DiamondSG);
    Dbg.PrintLn("RevenueSym          : %10s",   m_RevenueSym);
    Dbg.PrintLn("LiberationFactor    : %10.3f", m_LiberationFactor);

    Dbg.PrintLn("");
    for (int i=0; i<m_PassSizePercents.GetCount(); i++)
      Dbg.PrintLn("PassSizePercent[%i]  : %10.3f '%s'", i, m_PassSizePercents[i], m_PassSizePercentsText[i]);


    Dbg.PrintLn("");
    Dbg.PrintLn("Ore SGs:%i", nSGs());
    for (int i=0; i<nSGs(); i++)
      Dbg.PrintLn("%3i %8.2f %8.2f %8.2f  '%s'", i, SGsBottom(i), SGsTop(i), MeanSG(i), SGText(i));

    Dbg.PrintLn("");
    Dbg.PrintLn("Ore Sizes:%i", nOreSizes());
    for (int i=0; i<nOreSizes(); i++)
      Dbg.PrintLn("%3i %8.2f %8.2f %8.2f  '%s'", i, OreSizesBottom(i)*1000, OreSizesTop(i)*1000, MeanOreSize(i)*1000, OreSizeText(i));

    Dbg.PrintLn("");
    Dbg.PrintLn("Diamond Sizes:%i", nDiamondSizes());
    for (int i=0; i<nDiamondSizes(); i++)
      Dbg.PrintLn("%3i %8.2f %8.2f %8.2f  '%s'", i, DiamondSizesBottom(i)*1000, DiamondSizesTop(i)*1000, MeanDiamondSize(i)*1000, DiamondSizeText(i));

    Dbg.PrintLn("");
    Dbg.PrintLn("Liberation");
    Dbg.Print("%10s", "");
    for (int iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
      Dbg.Print(" %+6.2f", MeanDiamondSize(iDSz)*1000);
    Dbg.PrintLn("");

    for ( iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
      {
       Dbg.Print("%+10.2f", MeanOreSize(iOSz)*1000);
      for ( iDSz = 0 ; iDSz < m_numDiamondSizes; iDSz++ )
        Dbg.Print(" %6s", IsLiberatedDiamond( iDSz, iOSz )?"Lib":".");
      Dbg.PrintLn("");
      }


    Dbg.PrintLn("");
    Dbg.PrintLn("LockedParticleSG");
    for (int iDSz = 0 ; iDSz < m_numDiamondSizes ; iDSz++ )
      {
      Dbg.PrintLn("%10s", DiamondSizeText(iDSz));
      Dbg.Indent(2);
      for (int iOSz = 0 ; iOSz < m_numOreSizes ; iOSz++ )
        {
        Dbg.Print("%+10.2f", MeanOreSize(iOSz)*1000);
        for (int iSG = 0 ; iSG < m_numSGs ; iSG++ )
          {
          Dbg.Print("%+10.3f", m_LockedParticleSG[ lockedParticleIndex(iDSz, iOSz, iSG) ]);
          }
        Dbg.PrintLn("");
        }
      Dbg.Indent(-2);
      }

    Dbg.PrintLn("===============================================================");
    }
  return m_bInitialised;
  };


//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
void CDiamondWizardConfiguration::FreeLocalData( void )							// Tidy up local data allocations
  {
  m_OreSize.SetSize(0);
  m_OreSizesTop.SetSize(0);
  m_OreSizesTop.SetSize(0);
  m_OreSizesBottom.SetSize(0);
  m_DiamondSize.SetSize(0);
  m_DiamondSizesTop.SetSize(0);
  m_DiamondSizesBottom.SetSize(0);
  m_SG.SetSize(0);
  m_SGsTop.SetSize(0);
  m_SGsBottom.SetSize(0);
  m_Revenue.SetSize(0);
  m_Liberation.SetSize(0);
  m_LockedParticleSG.SetSize(0);
  m_PassSizePercents.SetSize(0);
  }

//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//
CDiamondWizardConfiguration::~CDiamondWizardConfiguration( void )						// Destructor
  {
  FreeLocalData() ;
  }
//
//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//

//													index into linear data space representing the 2D
//													spreadsheet representing the 3D diamond deportment data
int CDiamondWizardConfiguration::iODIndex ( int iOSz, int iSG )
  {		
  return iSG + m_numSGs * iOSz;
  }
//													index into linear data space representing the 2D
//													spreadsheet representing the 3D diamond deportment data
int CDiamondWizardConfiguration::iDDIndex(  int iDSz, int iOSz, int iSG  )
  {		
  return iDSz + m_numDiamondSizes*(  (INT((m_numSGs+1)/2) * ( (iSG % 2)*m_numOreSizes + iOSz)) + INT(iSG/2) ) ;
  }
//													Diamond Data Index into linear data space representing the Limn Stream data,
//													 i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
int	CDiamondWizardConfiguration::iDDLimnStreamIndex ( int iDSz, int iOSz, int iSG )
  {
  return m_numSGs + iDSz + m_numLimnStreamDataColumns * ((iSG % 2) * m_numOreSizes) + (m_numLimnStreamDataColumns * iOSz) + (m_numDiamondSizes * INT(iSG/2)) ;
  //
  }
//													Ore Data Index into linear data space representing the Limn Stream data,
//													 i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
int	CDiamondWizardConfiguration::iODLimnStreamIndex ( int iOSz, int iSG )
  {
  return iSG + iOSz * m_numLimnStreamDataColumns ;
  }

//
//
//													Water Index into linear data space representing the Limn Stream data,
//													 i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
int	CDiamondWizardConfiguration::iWaterLimnStreamIndex()
{
	return (1+m_numOreSizes) * m_numLimnStreamDataColumns ;
}
//
//
//													FeSi Index into linear data space representing the Limn Stream data,
//													 i.e. the 2D spreadsheet representing 2D ore mass data and the 3D diamond deportment data.
int	CDiamondWizardConfiguration::iFeSiLimnStreamIndex()
{
	return (3+m_numOreSizes) * m_numLimnStreamDataColumns ;
}

CDiamondWizardConfiguration gs_DWCfg;
CDiamondWizardConfigurations gs_DWCfgs;

//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//
//
/////////////////////////////////////////////////////////////////////////////////////
//
//
//           CDiamondWizardConfigurations class - Collection Class to hold multiple
//			 instances of CDiamondWizardConfiguration objects 
//
//
//
//			NB. There are 2 map relating the configurationID, 
//				the Excel SheetID and the actual configuration Object.
//
//				The primary map relates Excel SheetIDs to the configuration object
//				A secondary map relates configurationIDs to the configuration object
//
//				This is necessary because, while the configuration recognises itself
//				by the unique sheetID, the calculation functions that use the
//				configuration information require an ID that will change whenever
//				Configuration:Initialise is called, so that the functions dependent
//				on the configuration get refreshed also.
//                              
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//

CDiamondWizardConfigurations::CDiamondWizardConfigurations( void )						// Constructor
  {
  ResetID() ;
  }
//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//
CDiamondWizardConfigurations::~CDiamondWizardConfigurations( void )						// Destructor
  {
#ifdef NOT_DEFINED 
  // Delete all the configuration objects
  POSITION pos = m_ExcelSheetIDToConfiguration.GetStartPosition();
  while( pos != NULL )
    {
    CDiamondWizardConfiguration* pWizdConfig ;
    DWORD key;
    m_ExcelSheetIDToConfiguration.GetNextAssoc( pos, key, pWizdConfig );
    delete pWizdConfig;
    }

  m_ExcelSheetIDToConfiguration.RemoveAll();
#endif
  }
//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//
int CDiamondWizardConfigurations::IncrementID( void )
  { 
#ifdef NOT_DEFINED 
  m_nextConfigID++ ; 
  if ( m_nextConfigID > 32000 ) 
    {
    m_nextConfigID = 1 ;
    }
#endif
  return ID() ;
  } 
//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//           Function to return a Pointer to the configuration, given a configuration ID.
//	
CDiamondWizardConfiguration* CDiamondWizardConfigurations::Configuration( int configID )
  {
#ifdef NOT_DEFINED 
  POSITION pos = m_configIDToConfiguration.GetStartPosition();
  while (pos != NULL)
    {
    int iKey;
    CDiamondWizardConfiguration* pConfig;
    m_configIDToConfiguration.GetNextAssoc(pos, iKey, pConfig);
    TRACE("ID, Pointer = %i, 0x%X\n",iKey,pConfig) ;
    }

  CDiamondWizardConfiguration* pWizdConfig ;

  if ( !m_configIDToConfiguration.Lookup( configID, pWizdConfig ) )
    {
    pWizdConfig = NULL ;
    }

  return pWizdConfig ;
#else
  return &gs_DWCfg;
#endif
  }
//
//  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
//
//           Function to update the collection.
//				Creates a new entry, or returns a pointer to a matching entry if one exists.
//				Increments the ConfigurationID
CDiamondWizardConfiguration* CDiamondWizardConfigurations::Update( DWORD ExcelSheetID )
  {
#ifdef NOT_DEFINED 
  CDiamondWizardConfiguration* pWizdConfig ;

  if ( !m_ExcelSheetIDToConfiguration.Lookup( ExcelSheetID, pWizdConfig ) )
    {
    pWizdConfig = new CDiamondWizardConfiguration ;
    m_ExcelSheetIDToConfiguration.SetAt( ExcelSheetID, pWizdConfig ) ;
    }

  int configID = pWizdConfig->ID() ;
  if ( configID != -1 )
    {
    m_configIDToConfiguration.RemoveKey( configID ) ;
    }

  configID = IncrementID() ;								// bump the global configuration ID

  pWizdConfig->SetID( configID ) ;						// save it with the new configuration

  m_configIDToConfiguration.SetAt( configID, pWizdConfig ) ;		// set up the link

  return pWizdConfig ;
#else
  return &gs_DWCfg;
#endif
  }


//===========================================================================
//
//
//
//===========================================================================

int LoadArrayData(LPCTSTR Root, LPCTSTR File, int iSource, LPCTSTR RangeName, double Scale, int Cols, int Rows, CArray<double,double> &Data)//, MLog & Log)
  {
  int N=0;

  CString Fn(Root);
  Fn+=File;
  switch (iSource)
    {
    case 0: // CSV
      {
      Fn+=".";
      Fn+=RangeName;
      if (Fn.Right(4).CompareNoCase(".CSV")!=0)
        Fn+=".CSV";


      FILE* f=fopen(Fn, "rt");
      if (f)
        {
        for (int r=0; r<Rows; r++)
          {
          char Buff[16000];
          if (fgets(Buff, sizeof(Buff)-1, f))
            {
            CSVColArray Tkns;
            int NTkns=ParseCSVTokens(Buff, Tkns);
            for (int c=0; c<Cols; c++)
              Data.SetAtGrow(c + r*Cols, c<NTkns ? SafeAtoF(Tkns[c])*Scale : 0.0);
            }
          }
        fclose(f);
        }
      else
        {
        _asm int 3;
        //Log.Message(MMsg_Error, "CSV File %s not opened", Fn);
        }
      break;
      }
    case 1: //XLS
      {
      _asm int 3;
      //Log.Message(MMsg_Error, "XLS Files not yet implemented");
      break;
      }
    }

  return Data.GetCount();
  }

//===========================================================================
//
//
//
//===========================================================================

#ifdef NOT_DEFINED

CLimnGlobals::CInterval::CInterval()
  {
  m_Start = 0;
  m_End   = 0;
  m_Mean  = 0;
  }

CLimnGlobals::CInterval::CInterval(double Start, double End, LPCTSTR Text)
  {
  m_Start = Start;
  m_End   = End;
  m_Mean  = Sqrt(Start*End);
  m_Text  = Text;
  }


CLimnGlobals::CLimnGlobals()
  {
  m_bInitialised    = false;
  //m_DmdSG           = 3.515;
  };

CLimnGlobals::~CLimnGlobals()
  {
  };

bool CLimnGlobals::InitialiseFromINI(LPCTSTR Fn)
  {
  m_bInitialised    = false;

  CString PFFn(CfgFiles());
  PFFn+=Fn;// "LimnPSD.ini";

  CProfINIFile PF(PFFn);

  CString Tg, Line, PrevTkn0;
  LPCTSTR SeriesSections[] = {"OreSG", "OreSize", "DiamondSize"};
  for (int iSeries=0; iSeries<3; iSeries++)
    {
    for (int iInt=0; ; iInt++)
      {
      Tg.Format("I%04i", iInt);
      Line = PF.RdStr(SeriesSections[iSeries], Tg, "");
      if (Line.GetLength()==0)
        break;
      CSVColArray Tkns;
      int N=ParseCSVTokens(Line.GetBuffer(), Tkns);
      if (N<=0)
        break;

      if (iInt>0)
        {
        switch (iSeries)
          {
          case 0: m_OreSG.Add(CInterval(SafeAtoF(PrevTkn0), SafeAtoF(Tkns[0]), Tkns[1]));  break;
          case 1: m_OreSz.Add(CInterval(SafeAtoF(PrevTkn0), SafeAtoF(Tkns[0]), Tkns[1]));  break;
          case 2: m_DmdSz.Add(CInterval(SafeAtoF(PrevTkn0), SafeAtoF(Tkns[0]), Tkns[1]));  break;
          }
        }
      PrevTkn0=Tkns[0];
      }
    }

  m_bInitialised = true;
  if(DoDbg)
    {
    MDebug Dbg;
    Dbg.PrintLn("LimnPSD");
    Dbg.PrintLn("OreSG:%i", OreSGCount());
    for (int i=0; i<OreSGCount(); i++)
      Dbg.PrintLn("%3i %8.2f %8.2f %8.2f  '%s'", i, m_OreSG[i].m_Start, m_OreSG[i].m_End, m_OreSG[i].m_Mean, m_OreSG[i].m_Text);

    Dbg.PrintLn("OreSz:%i", OreSzCount());
    for (int i=0; i<OreSzCount(); i++)
      Dbg.PrintLn("%3i %8.2f %8.2f %8.2f  '%s'", i, m_OreSz[i].m_Start, m_OreSz[i].m_End, m_OreSz[i].m_Mean, m_OreSz[i].m_Text);

    Dbg.PrintLn("OreSz:%i", DmdSzCount());
    for (int i=0; i<DmdSzCount(); i++)
      Dbg.PrintLn("%3i %8.2f %8.2f %8.2f  '%s'", i, m_DmdSz[i].m_Start, m_DmdSz[i].m_End, m_DmdSz[i].m_Mean, m_DmdSz[i].m_Text);

    }
  return m_bInitialised;
  };

//===========================================================================
//
//
//
//===========================================================================

CLimnGlobals gs_LimnGlbl;

//===========================================================================
//
//
//
//===========================================================================

#endif