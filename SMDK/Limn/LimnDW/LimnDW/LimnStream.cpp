//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "limnstream.h"
#include "math.h"
#include "md_headers.h"
//#include "..\LimnDWM\DiamondFunctions.h"
#pragma comment(lib, "rpcrt4.lib")

const int DoDbg     = 0;//xf;
const int DoDbgCvt  = 0;//xf;


//===========================================================================
//
//
//
//===========================================================================

CLimnStream::CFeed::CFeed()
  {
  m_iSource  = 0;
  m_sSrcName = "..\\CfgFiles\\Default";
  
  m_OreSizeFeed.SetSize(gs_DWCfg.nOreSizes());     
  m_DmdSizeFeed.SetSize(gs_DWCfg.nDiamondSizes()); 
  m_DmdSGFeed.SetSize(gs_DWCfg.nSGs());          
  m_Densimetrics.SetSize(gs_DWCfg.nSGs()*gs_DWCfg.nOreSizes());     

  for (int i=0; i<m_OreSizeFeed.GetCount(); i++)
    m_OreSizeFeed[i]=0.0;
  for (int i=0; i<m_DmdSizeFeed.GetCount(); i++)
    m_DmdSizeFeed[i]=0.0;
  for (int i=0; i<m_DmdSGFeed.GetCount(); i++)
    m_DmdSGFeed[i]=0.0;
  for (int i=0; i<m_Densimetrics.GetCount(); i++)
    m_Densimetrics[i]=0.0;
  };

void CLimnStream::CFeed::ValidateData()
  {
  ValidateData(m_OreSizeFeed);
  ValidateData(m_DmdSizeFeed);
  ValidateData(m_DmdSGFeed);
  };

void CLimnStream::CFeed::ValidateData(CArray<double, double> & Data)
  {
  double Total=0;
  int N=Data.GetSize();
  for (int i=0; i<N; i++)
    Total+=Data[i];
  double Err = Total-1.0;
  if (Err>0.0)
    {
    for (int i=N-1; i>=0; i--)
      {
      double T=GEZ(Data[i]-Err);
      Err -= Data[i]-T;
      Data[i] = T;
      }
    }
  else if (Err<0.0)
    {
    Data[N-1] -= Err;
    }
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_SPECIEQUALITY(CLimnStream, "LimnStream", "Limn Stream", "Limn Stream Description", DLL_GroupName)

CLimnStream::CLimnStream(TaggedObject * pNd)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  if (!gs_DWCfg.Initialised())
    gs_DWCfg.Initialise();

  m_bIsMassForm = false;
  m_Data.SetSize(gs_DWCfg.DataBlockCount());
  for (int i=0; i<m_Data.GetCount(); i++)
    m_Data[i]=0;

  m_bCalculate = false;
  m_pFeed=NULL;

  if (0) // TempInit
    {
    for (int i=0; i<m_Data.GetCount(); i++)
      m_Data[i]=0.1;
    }


  if (DoDbg)
    {
    Dbg.PrintLn("CLimnStream::CLimnStream");
    //Dump();
    }
  }

//---------------------------------------------------------------------------

CLimnStream::~CLimnStream()
  {
  delete m_pFeed;
  if (DoDbg)
    Dbg.PrintLn("CLimnStream::~CLimnStream");
  }

//---------------------------------------------------------------------------

const int idx_OreSolidsFlow     = 1;
const int idx_PassingSize0      = 2;
const int idx_PassingSize1      = 3;
const int idx_PassingSize2      = 4;
const int idx_MeanSG            = 5;
const int idx_MeanSize          = 6;
const int idx_WaterFlow         = 7;
const int idx_SolidsFrac        = 8;
const int idx_PulpSG            = 9;
const int idx_FeSiFlow          = 10;
const int idx_DiamondRate       = 11;
const int idx_DiamondAssay      = 12;
const int idx_LiberatedFlow     = 13;
const int idx_RelativeDiamonds  = 14;
const int idx_LiberatedFraction = 15;
const int idx_RelativeRevenue   = 16;
const int idx_RevenueFlow       = 17;

const int idx_CopySummary       = 100;
const int idx_CopyMass          = 101;
const int idx_CopyFractional    = 102;
const int idx_Calculate         = 103;
const int idx_Source            = 104;
const int idx_SrcName           = 105;
const int idx_SrcLoad           = 106;


void CLimnStream::BuildDataFields()
  { 
  //DD.Double();
  DD.Page("Limn", false);
  DD.Text("");
  DD.Text("Calculated Values ...");

  DD.Double("OreSolidsFlow",                   "", idx_OreSolidsFlow      , MF_RESULT, MC_Qm("t/h")); 
  DD.Double(gs_DWCfg.PassSizePercentsText(0),  "", idx_PassingSize0       , MF_RESULT, MC_L("mm")); 
  DD.Double(gs_DWCfg.PassSizePercentsText(1),  "", idx_PassingSize1       , MF_RESULT, MC_L("mm")); 
  DD.Double(gs_DWCfg.PassSizePercentsText(2),  "", idx_PassingSize2       , MF_RESULT, MC_L("mm")); 
  DD.Double("MeanSG",                          "", idx_MeanSG             , MF_RESULT, MC_Rho("t/m^3")); 
  DD.Double("MeanSize",                        "", idx_MeanSize           , MF_RESULT, MC_L("mm")); 
  DD.Double("WaterFlow",                       "", idx_WaterFlow          , MF_RESULT, MC_Qm("t/h")); 
  DD.Double("SolidsFrac",                      "", idx_SolidsFrac         , MF_RESULT, MC_Frac("%")); 
  DD.Double("PulpSG",                          "", idx_PulpSG             , MF_RESULT, MC_Rho("t/m^3")); 
  DD.Double("FeSiFlow",                        "", idx_FeSiFlow           , MF_RESULT, MC_Qm("t/h")); 
  DD.Double("DiamondRate",                     "", idx_DiamondRate        , MF_RESULT, MC_Qm("Carat/h")); 
  DD.Double("DiamondAssay",                    "", idx_DiamondAssay       , MF_RESULT, MC_Assay("Carat/100t")); 
  DD.Double("LiberatedFlow",                   "", idx_LiberatedFlow      , MF_RESULT, MC_Qm("Carat/h")); 
  DD.Double("RelativeDiamonds",                "", idx_RelativeDiamonds   , MF_RESULT, MC_Frac("%")); 
  DD.Double("LiberatedFraction",               "", idx_LiberatedFraction  , MF_RESULT, MC_Frac("%")); 
  DD.Double("RelativeRevenue",                 "", idx_RelativeRevenue    , MF_RESULT, MC_Frac("%")); 
  DD.Double("RevenueFlow",                     "", idx_RevenueFlow        , MF_RESULT, MC_MoneyFlow("R/h")); 
  
  //Relative Carats
  //% Liberated  
  //Relative Revenue  
  //Rand / hr  



  DD.Text("");
  DD.Text("Copy to Clipboard ...");
  DD.Button("Summary",                  "", idx_CopySummary);
  DD.Button("Data as Mass",             "", idx_CopyMass);
  DD.Button("Data as Fractional",       "", idx_CopyFractional);

  DD.Text("");
  DD.Text("Options ...");
  DD.CheckBox("Calculator",             "",  idx_Calculate, MF_PARAMETER|MF_SET_ON_CHANGE);
  if (m_bCalculate && m_pFeed)
    {
    static MDDValueLst DDSource[]=
      {
      {0,  "CSV"},
      {1,  "XLS"},
      {0}
      };
    
    DD.Long("Source",     "",  idx_Source, MF_PARAMETER, DDSource);
    DD.String("SrcName",  "",  idx_SrcName, MF_PARAMETER);
    DD.Button("SrcLoad",  "",  idx_SrcLoad, MF_PARAMETER);

    DD.Page("...", false);
    DD.ObjectBegin("LimnDW_Fd",  "OreSizeFeed",  gs_DWCfg.nOreSizes()       );
    CString Tg;
    for (int i=0; i<gs_DWCfg.nOreSizes(); i++)
      {
      Tg.Format("Ore%s",gs_DWCfg.OreSizeText(i));
      DD.Double(Tg, "", &m_pFeed->m_OreSizeFeed[i], gs_DWCfg.nOreSizes()-1?MF_PARAMETER:0, MC_Frac("%"));
      }
    DD.ObjectEnd();
    DD.ObjectBegin("LimnDW_Fd",  "DmdSizeFeed",  gs_DWCfg.nDiamondSizes()   );
    for (int i=0; i<gs_DWCfg.nDiamondSizes(); i++)
      {
      Tg.Format("Dmd%s",gs_DWCfg.DiamondSizeText(i));
      DD.Double(Tg, "", &m_pFeed->m_DmdSizeFeed[i], i<gs_DWCfg.nDiamondSizes()-1?MF_PARAMETER:0, MC_Frac("%"));
      }
    DD.ObjectEnd();
    DD.ObjectBegin("LimnDW_Fd",  "DmdSGFeed",    gs_DWCfg.nSGs()            );
    for (int i=0; i<gs_DWCfg.nSGs(); i++)
      {
      Tg.Format("SG-%s",gs_DWCfg.SGTextShort(i));
      DD.Double(Tg, "", &m_pFeed->m_DmdSGFeed[i], i<gs_DWCfg.nSGs()-1?MF_PARAMETER:0, MC_Frac("%"));
      }
    DD.ObjectEnd();
    
    }
  };              

//---------------------------------------------------------------------------

double CLimnStream::GetResult(int iResult)
  {
  if (!TestStateValid())
    {
    m_Results.SetSize(17);
    //Dbg.PrintLn("Update Results %s", Tag);

    ConvertToMassForm(Vector);
	  int xx=StreamSummaryCalculation(&m_Results[0], 
						                    	  &m_Data[0], 
                                    100, // TODO 
							                      100, // TODO 
                                    &gs_DWCfg) ;
    ConvertToFracForm(Vector);

    SetStateValid();
    }
  return m_Results[iResult];
  }

//---------------------------------------------------------------------------

bool CLimnStream::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idx_OreSolidsFlow      : DX.Double = GetResult(0)/(3.6);                 return true;
    case idx_PassingSize0       : DX.Double = GetResult(1)*0.001;                 return true;
    case idx_PassingSize1       : DX.Double = GetResult(2)*0.001;                 return true;
    case idx_PassingSize2       : DX.Double = GetResult(3)*0.001;                 return true;
    case idx_MeanSG             : DX.Double = GetResult(4)*1000;                  return true;
    case idx_MeanSize           : DX.Double = GetResult(5)*0.001;                 return true;
    case idx_WaterFlow          : DX.Double = GetResult(6)/(3.6);                 return true;
    case idx_SolidsFrac         : DX.Double = GetResult(7)*0.01;                  return true;
    case idx_PulpSG             : DX.Double = GetResult(8)*1000;                  return true;
    case idx_FeSiFlow           : DX.Double = GetResult(9)/(3.6);                 return true;
    case idx_DiamondRate        : DX.Double = GetResult(10)/(5000.0*1000*3.6);    return true;
    case idx_DiamondAssay       : DX.Double = GetResult(11)*5000*1000*100;        return true;
    case idx_LiberatedFlow      : DX.Double = GetResult(12)/(5000.0*1000*3.6);    return true;
    case idx_RelativeDiamonds   : DX.Double = GetResult(13);                      return true;
    case idx_LiberatedFraction  : DX.Double = GetResult(14)*0.01;                 return true;
    case idx_RelativeRevenue    : DX.Double = GetResult(15);                      return true;
    case idx_RevenueFlow        : DX.Double = GetResult(16);                      return true;

    case idx_CopySummary        : 
      {
      if (DX.HasReqdValue && DX.Bool)
        {
        CString Buff, V;
        for (int i=0; i<17; i++)
          {
          V.Format("%.12f\r\n", GetResult(i));
          Buff+=V;
          }
        CopyTextToClipboard(Buff);
        }
      DX.Bool =  false;
      return true;
      }

    case idx_CopyFractional     : 
    case idx_CopyMass           : 
      {
      if (DX.HasReqdValue && DX.Bool)
        {
        if (DX.Handle==idx_CopyMass)
          ConvertToMassForm(Vector);
        CString Buff, V;
        for (int r=0; r<RowCount(); r++)
          {
          for (int c=0; c<ColCount(); c++)
            {
            V.Format(c>0?"\t%.12f":"%.12f", m_Data[c+ColCount()*r]);
            Buff+=V;
            }
          Buff+="\r\n";
          }
        CopyTextToClipboard(Buff);
        if (DX.Handle==idx_CopyMass)
          ConvertToFracForm(Vector);
        }
      DX.Bool =  false;
      return true;
      }
    case idx_Calculate      :
      if (DX.HasReqdValue)
        {
        m_bCalculate = DX.Bool;
        if (m_bCalculate && m_pFeed==NULL)
          {
          m_pFeed=new CFeed;
          ReadData();
          }
        }
      DX.Bool = m_bCalculate;
      return true;
    case idx_Source         : 
      if (DX.HasReqdValue)
        m_pFeed->m_iSource = DX.Long;
      DX.Long   = m_pFeed->m_iSource;  
      return true;
    case idx_SrcName        : 
      if (DX.HasReqdValue)
        {
        m_pFeed->m_sSrcName = DX.String;
        ReadData();
        }
      DX.String = m_pFeed->m_sSrcName; 
      return true;
    case idx_SrcLoad        : 
      if (DX.HasReqdValue && DX.Bool)
        ReadData();
      return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

bool CLimnStream::ValidateDataFields()
  { 

  if (0)
    {
    Dbg.PrintLn("Vector @ Validate %s ", Tag);
    MVector V=Vector;
    for (int i=0; i<V.Count(); i++)
      Dbg.PrintLn("M[%-25s] : %10.4f", gs_MVDefn[i].Symbol(), V.M[i]);
    Dbg.PrintLn("");
    }
  if (m_pFeed)
    m_pFeed->ValidateData();

  LoadFeed();

  return true; 
  }; 

//---------------------------------------------------------------------------

void CLimnStream::ReadData()
  { 
  if (!m_bCalculate || m_pFeed==NULL)
    return;

  LoadArrayData("OreSizeFeed",  1.0, 1,               gs_DWCfg.nOreSizes(),      m_pFeed->m_OreSizeFeed);
  LoadArrayData("DmdSizeFeed",  1.0, 1,               gs_DWCfg.nDiamondSizes(),  m_pFeed->m_DmdSizeFeed);
  LoadArrayData("DmdSGFeed",    1.0, 1,               gs_DWCfg.nSGs(),           m_pFeed->m_DmdSGFeed);
  LoadArrayData("Densimetrics", 0.01, gs_DWCfg.nSGs(), gs_DWCfg.nOreSizes(),      m_pFeed->m_Densimetrics);
  int xxx=0;
  }

//---------------------------------------------------------------------------

void CLimnStream::LoadFeed()
  { 
  if (!m_bCalculate || m_pFeed==NULL)
    return;
 
  MVector Vec = Vector;

  if (0)
    {
    Dbg.PrintLn("Vector @ LoadFeed");
    MVector V=Vector;
    for (int i=0; i<V.Count(); i++)
      Dbg.PrintLn("M[%-25s] : %10.4f", gs_MVDefn[i].Symbol(), V.M[i]);
    Dbg.PrintLn("");
    }

  double OreMass=0;
  for (int iSG=0; iSG<gs_DWCfg.nSGs(); iSG++)
    OreMass += Vec.M[gs_DWCfg.OreSpIds(iSG)];

  CArray <double, double> OreMasses;
  OreMasses.SetSize(gs_DWCfg.OreBlockCount());
	OreMassFromDensimetricDistribution(&gs_DWCfg, 
											               &m_pFeed->m_Densimetrics[0], 
											               &m_pFeed->m_OreSizeFeed[0], 
											               OreMass,//double totalFlow, 
											               &OreMasses[0] ) ;

  CArray <double, double> DmdDeport;
  DmdDeport.SetSize(gs_DWCfg.DataBlockCount());//DiamondBlockCount());
	DiamondDeportmentFromSGDistribution(&gs_DWCfg,
                                      &m_pFeed->m_Densimetrics[0], 
                                      &m_pFeed->m_OreSizeFeed[0], 
                                      &m_pFeed->m_DmdSGFeed[0], 
                                      &m_pFeed->m_DmdSizeFeed[0], 
                                      Vec.M[gs_DWCfg.DiamondSpId()],
                                      &DmdDeport[0]) ;

  m_Data[gs_DWCfg.iWaterLimnStreamIndex()]  = Vec.M[gs_DWCfg.WaterSpId()];
  m_Data[gs_DWCfg.iFeSiLimnStreamIndex()]   = Vec.M[gs_DWCfg.FeSiSpId()];

  //for (int iSz= 0; iSz< gs_DWCfg.nOreSizes(); iSz++ )
  //  {
  //  Dbg.Print("Densimetrics:");
  //  for (int iSG= 0 ; iSG< gs_DWCfg.nSGs(); iSG++ )
  //    Dbg.Print(" %10.4f", m_pFeed->m_Densimetrics[xlIndex(iSz, iSG, gs_DWCfg.nSGs())]);
  //  Dbg.PrintLn("");
  //  }
  //Dbg.PrintLn("");
  //for (int i= 0 ; i< m_pFeed->m_OreSizeFeed.GetCount(); i++ )
  //  Dbg.PrintLn("m_OreSizeFeed[%3i] %10.4f", i, m_pFeed->m_OreSizeFeed[i]);
  //Dbg.PrintLn("");
  //for (int i= 0 ; i< m_pFeed->m_DmdSGFeed.GetCount(); i++ )
  //  Dbg.PrintLn("m_DmdSGFeed[%3i]   %10.4f", i, m_pFeed->m_DmdSGFeed[i]);
  //Dbg.PrintLn("");
  //for (int i= 0 ; i< m_pFeed->m_DmdSizeFeed.GetCount(); i++ )
  //  Dbg.PrintLn("m_DmdSizeFeed[%3i] %10.4f", i, m_pFeed->m_DmdSizeFeed[i]);
  //Dbg.PrintLn("");

  for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
    {
    for (int iSG = 0 ; iSG < nSGs(); iSG++ )
      {
      Ore(iOSz, iSG) = OreMasses[xlIndex( iOSz, iSG, nSGs() )] ;
      }
    }

  for (int iDSz = 0 ; iDSz< nDiamondSizes(); iDSz++ )
    {
    for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
      {
      for (int iSG = 0 ; iSG < nSGs(); iSG++ )
        {
        Diamond(iDSz, iOSz, iSG)=DmdDeport[gs_DWCfg.iDDIndex(iDSz, iOSz, iSG)];
        }
      }
    }

  m_bIsMassForm = true;

  if (DoDbg)
    Dump("LoadFeed", DoDbg);
  
  ConvertToFracForm(Vector, false);

  if (DoDbg)
    Dump("LoadFeed", DoDbg);
  }; 

//---------------------------------------------------------------------------

//int LoadArrayData(LPCTSTR Root, LPCTSTR File, int iSource, LPCTSTR RangeName, double Scale, int Cols, int Rows, CArray<double,double> &Data, MLog & Log)
//  {
//  int N=0;
//
//  CString Fn(Root);
//  Fn+=File;
//  switch (iSource)
//    {
//    case 0: // CSV
//      {
//      Fn+=".";
//      Fn+=RangeName;
//      if (Fn.Right(4).CompareNoCase(".CSV")!=0)
//        Fn+=".CSV";
//
//
//      FILE* f=fopen(Fn, "rt");
//      if (f)
//        {
//        for (int r=0; r<Rows; r++)
//          {
//          char Buff[16000];
//          if (fgets(Buff, sizeof(Buff)-1, f))
//            {
//            CSVColArray Tkns;
//            int NTkns=ParseCSVTokens(Buff, Tkns);
//            for (int c=0; c<Cols; c++)
//              Data.SetAtGrow(c + r*Cols, c<NTkns ? SafeAtoF(Tkns[c])*Scale : 0.0);
//            }
//          }
//        fclose(f);
//        }
//      else
//        {
//        Log.Message(MMsg_Error, "CSV File %s not opened", Fn);
//        }
//      break;
//      }
//    case 1: //XLS
//      {
//      Log.Message(MMsg_Error, "XLS Files not yet implemented");
//      break;
//      }
//    }
//
//  return Data.GetCount();
//  }

int CLimnStream::LoadArrayData(LPCTSTR RangeName, double Scale, int Cols, int Rows, CArray<double,double> &Data)//, MLog & Log)
  {
  int N=0;

  Dbg.PrintLn("LoadArrayData:%s",RangeName);

  CFeed &Fd = *m_pFeed;

  return ::LoadArrayData(PrjFiles(), Fd.m_sSrcName, Fd.m_iSource, RangeName, Scale, Cols, Rows, Data);//, Log);

  //CString Fn(PrjFiles());
  //Fn+=Fd.m_sSrcName;
  //switch (Fd.m_iSource)
  //  {
  //  case 0: // CSV
  //    {
  //    Fn+=".";
  //    Fn+=RangeName;
  //    if (Fn.Right(4).CompareNoCase(".CSV")!=0)
  //      Fn+=".CSV";


  //    FILE* f=fopen(Fn, "rt");
  //    if (f)
  //      {
  //      for (int r=0; r<Rows; r++)
  //        {
  //        char Buff[16000];
  //        if (fgets(Buff, sizeof(Buff)-1, f))
  //          {
  //          CSVColArray Tkns;
  //          int NTkns=ParseCSVTokens(Buff, Tkns);
  //          for (int c=0; c<Cols; c++)
  //            Data.SetAtGrow(c + r*Cols, c<NTkns ? SafeAtoF(Tkns[c])*0.01 : 0.0);
  //          }
  //        }
  //      fclose(f);
  //      }
  //    else
  //      {
  //      Log.Message(MMsg_Error, "CSV File %s not opened", Fn);
  //      }
  //    break;
  //    }
  //  case 1: //XLS
  //    {
  //    Log.Message(MMsg_Error, "XLS Files not yet implemented");
  //    break;
  //    }
  //  }

  //for (int r= 0; r< Rows; r++ )
  //  {
  //  Dbg.Print("%s:", RangeName);
  //  for (int c= 0 ; c<Cols; c++ )
  //    Dbg.Print(" %10.4f", Data[xlIndex(r, c, Cols)]);
  //  Dbg.PrintLn("");
  //  }

  //return Data.GetCount();
  }

//---------------------------------------------------------------------------

enum 
  {
  idGetValue,
  idSetValue,
  idCalcOre,
  idCalcDmd,

  idMPI_EndOfProps 
  };

long CLimnStream::DefinedPropertyCount()
  {
  return idMPI_EndOfProps + 1 + MSpQualityBase::DefinedPropertyCount();
  }

//---------------------------------------------------------------------------

long CLimnStream::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index;//(Index-MSpModelBase::DefinedPropertyCount());
  switch (Inx)
    {
    case idGetValue:  Info.SetStructName("DemoQuals");
                      Info.Set(ePT_Double,    "Get",       "", MC_,    "",       0,      0,  MP_Result|MP_NoFiling, "A Desc"); return Inx;
    case idSetValue:  Info.Set(ePT_Double,    "Set",       "", MC_,    "",       0,      0,  MP_HardMin|MP_Parameter, "A Desc"); return Inx;
    case idCalcOre:   Info.Set(ePT_Double,    "CalcOre",   "", MC_,    "",       0,      0,  MP_Result|MP_NoFiling, "Mass Ore"); return Inx;
    case idCalcDmd:   Info.Set(ePT_Double,    "CalcDmd",   "", MC_,    "",       0,      0,  MP_Result|MP_NoFiling, "Mass Diamond"); return Inx;
    }
  return -1;
  };

//---------------------------------------------------------------------------

DWORD CLimnStream::GetPropertyVisibility(long Index)
  {
  return ePVis_All;
  };

//---------------------------------------------------------------------------

void CLimnStream::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  switch (Index)
    {
    //case   idGetValue: Value=m_dGetProp;  return ;
    //case   idSetValue: Value=m_dSetProp;  return ;
    case   idCalcOre:  
      {
      double Tot=0;
      for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
        {
        for (int iSG = 0 ; iSG < nSGs(); iSG++ )
          Tot+=Ore(iOSz, iSG);
        }
      Value=Tot;
      return ;
      }
    case   idCalcDmd: 
      {
      double Tot=0;
      for (int iSG = 0 ; iSG < nSGs(); iSG++ )
        {
        for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
          {
          for (int iDSz = 0 ; iDSz< nDiamondSizes(); iDSz++ )
            Tot+=Diamond(iDSz, iOSz, iSG);
          }
        }
      Value=Tot;
      return ;
      }
    };
  };

//---------------------------------------------------------------------------

void CLimnStream::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  //switch (Index)
  //  {
  //  case  1: m_dSetProp=Value; return;
  //  };
  };

//---------------------------------------------------------------------------

void CLimnStream::ConvertToMassForm(MVector & M, bool ApplyScale)
  {
  if (m_bIsMassForm)
    return;

  if (DoDbgCvt)
    Dump("ConvertToMassForm:Before", DoDbgCvt);

  if (ApplyScale)
    {
    for (int ii=0; ii<gs_DWCfg.m_SeqIndex.GetCount(); ii++)
      {
      int i = gs_DWCfg.m_SeqIndex[ii];
      int id = gs_DWCfg.m_SpIds[i];
      m_Data[i] *= M.M[id] * gs_DWCfg.m_MassFactor[id];
      };
    }
  else
    {
    for (int ii=0; ii<gs_DWCfg.m_SeqIndex.GetCount(); ii++)
      {
      int i = gs_DWCfg.m_SeqIndex[ii];
      int id = gs_DWCfg.m_SpIds[i];
      m_Data[i] *= M.M[id];
      };
    }

  //if (DoDbg)
  //  Dump("SetMassF", 0x7);
  if (DoDbgCvt)
    Dump("ConvertToMassForm:After", DoDbgCvt);

  m_bIsMassForm =  true;
  }

//---------------------------------------------------------------------------

void CLimnStream::ConvertToFracForm(MVector & M, bool ApplyScale)
  {
  if (!m_bIsMassForm)
    return;
  if (DoDbgCvt)
    Dump("ConvertToFracForm:Before", DoDbgCvt);

  MArray Tot;
  // Current Total
  for (int ii=0; ii<gs_DWCfg.m_SeqIndex.GetCount(); ii++)
    {
    int i=gs_DWCfg.m_SeqIndex[ii];
    Tot[gs_DWCfg.m_SpIds[i]] += m_Data[i];
    };
  
  if (ApplyScale)
    {
    for (int id=0; id<gs_MVDefn.Count(); id++)
      {
      M.M[id]=Tot[id]/gs_DWCfg.m_MassFactor[id];
      Tot[id]=GTZ(Tot[id]);
      }
    M.MarkStateChanged();
    }
  else
    {
    for (int id=0; id<gs_MVDefn.Count(); id++)
      {
      M.M[id]=Tot[id];
      Tot[id]=GTZ(Tot[id]);
      }
    M.MarkStateChanged();
    }
  for (int ii=0; ii<gs_DWCfg.m_SeqIndex.GetCount(); ii++)
    {
    int i  = gs_DWCfg.m_SeqIndex[ii];
    m_Data[i] = m_Data[i]/Tot[gs_DWCfg.m_SpIds[i]];
    };
  m_bIsMassForm = false;
  if (DoDbgCvt)
    Dump("ConvertToFracForm:After", DoDbgCvt);
  };

//---------------------------------------------------------------------------

void CLimnStream::ExchangeSpecies(MSpQualityBase * Other) 
  {
  int xxx=0;
  };
bool CLimnStream::EquilRqd4HEval()
  {
  return false;
  };
void CLimnStream::Equilibrate() 
  {
  MVector M1(this);
  //m_dGetProp=M1.Density()/1000;
  };
void CLimnStream::ZeroMass() 
  {
  for (int i=0; i<m_Data.GetCount(); i++)
    m_Data[i] = 0;
  };
void CLimnStream::ZeroDeriv() 
  {
  };
void CLimnStream::ScaleMass(long Phase, double Mult) 
  {
  ASSERT_ALWAYS(!m_bIsMassForm, "Mass Format not Expected")
  //for (int i=0; i<m_Data.GetCount(); i++)
  //  m_Data[i] *= Mult;
  };
void CLimnStream::ScaleMass(CArray<int,int> &SpIds, double Mult)
  {
  };
void CLimnStream::SetMassF(MSpQualityBase * QualSet, MArray & MSet) 
  {
  ASSERT_ALWAYS(!m_bIsMassForm, "Mass Format not Expected")
  CLimnStream * pQ2=dynamic_cast<CLimnStream*>(QualSet);

  for (int i=0; i<m_Data.GetCount(); i++)
    m_Data[i] = pQ2->m_Data[i];

  //if (DoDbg)
  //  Dump("SetMassF", 0x7);
  };

void CLimnStream::AddMassF(MSpQualityBase * QualAdd, MArray & MAdd) 
  {
  ASSERT_ALWAYS(!m_bIsMassForm, "Mass Format not Expected");

  CLimnStream * pQualAdd=dynamic_cast<CLimnStream*>(QualAdd);
  // These numbers are always Fractional
  
  MVector M1=Vector; //
  MArray MFrac1;
  for (int id=0; id<gs_MVDefn.Count(); id++)
    MFrac1[id] = M1.M[id]/GTZ(M1.M[id]+MAdd[id]);

  for (int ii=0; ii<gs_DWCfg.m_SeqIndex.GetCount(); ii++)
    {
    int i = gs_DWCfg.m_SeqIndex[ii];
    int id = gs_DWCfg.m_SpIds[i];
    m_Data[i] = m_Data[i]*MFrac1[id] + pQualAdd->m_Data[i]*(1.0-MFrac1[id]);
    };

  if (DoDbg)
    Dump("AddMassF", 0x7);
  };

void CLimnStream::SubMassF(MSpQualityBase * QualSub, MArray & M2) 
  {
  };

void CLimnStream::Copy(MVector &V2, MSpQualityBase * QualCopy) 
  {
  CLimnStream * pQ2=dynamic_cast<CLimnStream*>(QualCopy);
  for (int i=0; i<m_Data.GetCount(); i++)
    m_Data[i] = pQ2->m_Data[i];
  if (DoDbg)
    Dump("Copy", 0x7);
  };

void CLimnStream::AddDeriv(MVector &V2, MSpQualityBase * pQual2, double Sgn_, double DeltaTime) 
  {
  };

void CLimnStream::AddDiscrete(MVector &V2, MSpQualityBase * pQual2, double Sgn_, double DeltaTime) 
  {
  CLimnStream * pQ2=dynamic_cast<CLimnStream*>(pQual2);
  if (Sgn_>0.0)
    {
    MVector M1=Vector; //
    MArray m1;
    MArray m2;
    for (int id=0; id<gs_MVDefn.Count(); id++)
      {
      m1[id] = M1.M[id];
      m2[id] = V2.M[id]*DeltaTime;
      }
    for (int ii=0; ii<gs_DWCfg.m_SeqIndex.GetCount(); ii++)
      {
      int i = gs_DWCfg.m_SeqIndex[ii];
      int id = gs_DWCfg.m_SpIds[i];
      m_Data[i] = (m_Data[i]*m1[id] + pQ2->m_Data[i]*m2[id])/(GTZ(m1[id]+m2[id]));
      };
    }
  };

void CLimnStream::Dump(LPCTSTR Where, DWORD What) 
  {
  if (What)
    {
    Dbg.Lock();
    Dbg.PrintLn("Dump %s ----------------------------------------- %s : %s", m_bIsMassForm?"MassForm":"FracForm", Where, Tag);

    if (0)
      {
      Dbg.PrintLn("");
      Dbg.PrintLn("Data:");
      for (int r = 0 ; r < gs_DWCfg.RowCount(); r++ )
        {
        Dbg.Print("%3i)", r);
        for (int c= 0 ; c < gs_DWCfg.ColCount(); c++ )
          Dbg.Print(" %10.2f", m_Data[r*gs_DWCfg.ColCount() + c]);
        Dbg.PrintLn("");
        }
      Dbg.PrintLn("");
      }

    if (What)
      {
      MVector V=Vector;
      for (int i=0; i<V.Count(); i++)
        Dbg.PrintLn("M[%-25s] : %15.4f", gs_MVDefn[i].Symbol(), V.M[i]*(m_bIsMassForm?3.6:1.0));
      Dbg.PrintLn("");
      }
    if (What & 0x01)
      Dbg.PrintLn("Water  : %10.4f", Water());
    if (What & 0x02)
      Dbg.PrintLn("FeSi   : %10.4f", FeSi());

    if (What & 0x04)
      {
      Dbg.PrintLn("Ore:");
      Dbg.Indent(4);
      Dbg.Print("%10s", "Size\\SG");
      for (int iSG = 0 ; iSG < nSGs(); iSG++ )
        Dbg.Print(" %10.4f", MeanSG(iSG ));
      Dbg.PrintLn("");
      Dbg.Print("%10s", "-------");
      for (int iSG = 0 ; iSG < nSGs(); iSG++ )
        Dbg.Print(" %10s", "--------");
      Dbg.PrintLn("");
      for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
        {
        Dbg.Print("%+10.2f", MeanOreSize(iOSz));
        for (int iSG = 0 ; iSG < nSGs(); iSG++ )
          Dbg.Print(" %10.4f", Ore(iOSz, iSG ));
        Dbg.PrintLn("");
        }
      Dbg.Indent(-4);
      }

    if (What & 0x08)
      {
      Dbg.PrintLn("");
      Dbg.PrintLn("Diamonds:");
      for (int iSG = 0 ; iSG < nSGs(); iSG++ )
        {
        Dbg.PrintLn("SG : %s", SGText(iSG));
        Dbg.Indent(2);

        bool DoIt=false;
        for (int iOSz = 0 ; !DoIt && iOSz < nOreSizes() ; iOSz++ )
          {
          for (int iDSz = 0 ; !DoIt && iDSz< nDiamondSizes(); iDSz++ )
            DoIt=Diamond(iDSz, iOSz, iSG )>1.0e-6;
          }

        if (!DoIt)
          {
          Dbg.Print("%10s", "");
          Dbg.PrintLn("ZERO");
          }
        else
          {
          Dbg.Print("%10s", "OSz\\DSz");
          for (int iDSz = 0 ; iDSz< nDiamondSizes(); iDSz++ )
            Dbg.Print(" %10s", DiamondSizeText(iDSz));
          Dbg.PrintLn("");
          Dbg.Print("%10s", "-------");
          for (int iDSz = 0 ; iDSz< nDiamondSizes(); iDSz++ )
            Dbg.Print(" %10s", "--------");
          Dbg.PrintLn("");
          for (int iOSz = 0 ; iOSz < nOreSizes() ; iOSz++ )
            {
            Dbg.Print("%+10.2f", MeanOreSize(iOSz));
            for (int iDSz = 0 ; iDSz< nDiamondSizes(); iDSz++ )
              Dbg.Print(" %10.4f", Diamond(iDSz, iOSz, iSG ));
            Dbg.PrintLn("");
            }
          }
        Dbg.Indent(-2);
        }
      }
    Dbg.PrintLn("-------------------------------------------------------");
    Dbg.UnLock();
    }
  };
