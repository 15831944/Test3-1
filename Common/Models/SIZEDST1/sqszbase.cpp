//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __SQSZBASE_CPP
#include "sc_defs.h"
#include "sp_db.h"
#include "mdlcfg.h"
#include "sqszbase.h"
#include "sqszdst1.h"
#include "dbgmngr.h"
//#include "optoff.h"

#define  dbgSzDist   1

#if dbgSzDist
static CDbgMngr dbgSzDistInit("SzDist", "Initial");
#endif

//===========================================================================
//
//
//
//===========================================================================

flag fSzAscend=false;//true;
double gs_CountFactor = 1.0;
//double gs_CountFactor = 0.37011411; //for worsley WRONG because of MeanPartDiam fix #392
//double gs_CountFactor = 0.493456717; //for worsley

//===========================================================================
//
//
//
//===========================================================================

dword CSD_Column::sm_VarSlctMask = SzDSlct_Fp|SzDSlct_Qm|SzDSlct_M|SzDSlct_Cum;

CSD_Column::CSD_Column()
  {
  fOn=True;
  fForFlow=True;
  fForMass=True;
  fEditable=False;
  fForCum=False;
  fGrfOn=True;
  m_dwVarSlct=0xffffffff;
  iXid=0;
  iSpId=0;
  iDataId=0;
  dDispMin=0.0;
  //dCDispMax=1.0;
  dFDispMax=1.0;
  pCnv=NULL;
  pFmt=NULL;
  dwSaveFlags=0;
  iColWidth=10;
  };

CSD_Column::CSD_Column(ColumnInitInfo & CI, char * SpName, int Xid, int SpId, flag SimpleNames)
  {
  sName=CI.Name;
  sSpName=SpName;
  sFullName=CI.Name;
  sFullName+=".";
  sFullName+=SpName;
  sFullNameColon=CI.Name;
  sFullNameColon+=":";
  sFullNameColon+=SpName;
  fSimpleNames=SimpleNames;
  fOn=CI.fOn;
  fForFlow=CI.fForFlow;
  fForMass=CI.fForMass;
  fEditable=CI.fEditable;
  fForCum=CI.fForCum;
  m_dwVarSlct=CI.m_dwVarSlct;
  fGrfOn=CI.fGrfOn;
  iXid=Xid;
  iSpId=SpId;
  iDataId=CI.iDataId;
  dDispMin=0.0;
  //dCDispMax=1.0;
  dFDispMax=1.0;
  pCnv=CI.pCnv;
  pFmt=CI.pFmt;
  dwSaveFlags=CI.dwSaveFlags;
  if (fEditable)
    dwSaveFlags|=isParm;
  //iColWidth=Max(10, (fSimpleNames ? sName.Len() : sFullName.Len())+1);
  iColWidth=Max(Max(10, sName.Len()+1), sSpName.Len()+1);
  };

//===========================================================================
//
//
//
//===========================================================================

struct CMeasInfo
  {
  eSDMeasTypes T;
  char * Abr;
  char * Name;
  long RqdParts;
  CCnvIndex iValCnv;
  char * sValCnv;
  CCnvIndex iDspCnv;
  char * sDspCnv;
  };

static CMeasInfo MeasInfo[] =
  {
    {eSDMT_FracPassSize,  "F", "FracPassSize", 3, DC_L,    "um",   DC_Frac,      "%"     },
    {eSDMT_SizePassFrac,  "S", "SizePassFrac", 3, DC_Frac, "%",    DC_L,         "um"    },
    {eSDMT_PPG,           "P", "PPG",          2, DC_,     "",     DC_SpCnt,     "#/g"   },
    {eSDMT_SAM,           "M", "SAM",          2, DC_,     "",     DC_SurfAreaM, "m^2/g" },
    {eSDMT_SAL,           "V", "SAL",          2, DC_,     "",     DC_SurfAreaL, "m^2/L" },
    {eSDMT_Text,          "T", "Text",         1, DC_,     "",     DC_,          ""      },
    {eSDMT_Null}
  };

//===========================================================================
//
//
//
//===========================================================================

static flag RDCFG(CProfINIFile &Cfg, char *A, char*B, int N, Strng &Val)
  {
  Strng S;
  S.Set("%s%04i",B,N);
  Val=Cfg.RdStr(A, S(), "???");
  return (Val.XStrICmp("???")!=0 && Val.Length()>0);
  };

static flag RDCFG(CProfINIFile &Cfg, char *A, char*B,int N, char*C, Strng &Val)
  {
  Strng S;
  S.Set("%s%02i.%s",B,N,C);
  Val=Cfg.RdStr(A, S(), "???");
  return (Val.XStrICmp("???")!=0 && Val.Length()>0);
  };

static void WRCFG(CProfINIFile &Cfg, char *A, char*B, char*C, char* s)
  {
  Strng S;
  S.Set("%s.%s", B, C);
  Cfg.WrStr(A, S(), s);
  };

static void WRCFG(CProfINIFile &Cfg, char *A, char*B, char*C, double d)
  {
  Strng S;
  S.Set("%s.%s", B, C);
  Cfg.WrDouble(A, S(), d);
  };

static void WRCFG(CProfINIFile &Cfg, char *A, char*B, char*C, long l)
  {
  Strng S;
  S.Set("%s.%s", B, C);
  Cfg.WrLong(A, S(), l);
  };

static flag RDCFG(CProfINIFile &Cfg, char *A, char*B,int N, char*C, int M, Strng &Val)
  {
  Strng S;
  S.Set("%s%02i.%s%03i",B,N,C,M);
  Val=Cfg.RdStr(A, S(), "???");
  return (Val.XStrICmp("???")!=0 && Val.Length()>0);
  };

static flag RDCFG4(CProfINIFile &Cfg, char *A, char*B, char*C, int M, Strng &Val)
  {
  Strng S;
  S.Set("%s.%s%03i",B,C,M);
  Val=Cfg.RdStr(A, S(), "???");
  return (Val.XStrICmp("???")!=0 && Val.Length()>0);
  };

/*static flag RDCFG4(CProfINIFile &Cfg, char *A, char*B,int N, char*C, Strng &Val)
  {
  Strng S;
  S.Set("%s%04i.%s",B,N,C);
  Val=Cfg.RdStr(A, S(), "???");
  return (Val.XStrICmp("???")!=0 && Val.Length()>0);
  };

static flag RDCFG4(CProfINIFile &Cfg, char *A, char*B,int N, char*C, int M, Strng &Val)
  {
  Strng S;
  S.Set("%s%04i.%s%03i",B,N,C,M);
  Val=Cfg.RdStr(A, S(), "???");
  return (Val.XStrICmp("???")!=0 && Val.Length()>0);
  };*/

static void WRCFG4(CProfINIFile &Cfg, char *A, char*B, char*C, int M, double d)
  {
  Strng S;
  S.Set("%s.%s%03i",B,C,M);
  Cfg.WrDouble(A, S(), d);
  };

static void WRCFG4(CProfINIFile &Cfg, char *A, char*B, char*C, int M, char* s)
  {
  Strng S;
  S.Set("%s.%s%03i",B,C,M);
  Cfg.WrStr(A, S(), s);
  };

static void DoTopBottom(double & D, CProfINIFile &Cfg, int iDistNo, char * What)
  {
  Strng S;
  if (RDCFG(Cfg, "SizeDistributions", "Dist", iDistNo, What, S))
    {
    D=SafeAtoF(S());
    char * p=S.XStrChr(' ');
    Strng CnvStr;
    if (p)
      {
      CnvStr=p;
      CnvStr.LRTrim();
      }

    CDataCnv *pC=p ? gs_CnvsMngr.FindSecCnv(DC_L, CnvStr()) : NULL;
    if (pC)
      D=pC->Normal(D, CnvStr());
    }
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::SortSizes()
  {
  const BOOL Ascending = TRUE;
  for (int i=1; i<ManualSizes.GetSize(); i++)
    for (int j=i; j>=1 && (ManualSizes[j]<ManualSizes[j-1]==Ascending); j--)
      {
      double temp = ManualSizes[j-1];
      ManualSizes[j-1] = ManualSizes[j];
      ManualSizes[j] = temp;
      }
  }

// --------------------------------------------------------------------------

flag CSieveSeriesHelper::ChangeUnits(char* pNewCnvTxt)
  {
  CDataCnv* pPrevCnv = pCnv;
  if (pNewCnvTxt)
    {
    pCnv = gs_CnvsMngr.FindSecCnv(DC_L, pNewCnvTxt);
    if (pCnv==NULL)
      pCnv = pPrevCnv;
    }
  if (pCnv==NULL)
    gs_CnvsMngr.SICnv(DC_L);
  Units = pCnv->Txt();
  flag Chgd = (pPrevCnv!=pCnv);
  if (Chgd)
    Precision = -1;
  return Chgd;
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::CalcDisplayPrecision()
  {
  if (pCnv==NULL)
    ChangeUnits(Units());
  char Buff[64];
  Precision = 15;
  int MinZeros = 999;
  for (int i=0; i<ManualSizes.GetSize(); i++)
    {
    sprintf(Buff, "%.*f", Precision, pCnv->Human(ManualSizes[i]));
    const int len = strlen(Buff);
    int j = len-1;
    while (j>=0 && Buff[j]=='0')
      j--;
    if (len-j-1>0 && len-j-1<MinZeros)
      MinZeros = len-j-1;
    }
  Precision = Max(1, Precision-MinZeros+1);
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::FormatSize(double d, CString& S)
  {
  if (pCnv==NULL)
    ChangeUnits(Units());
  if (Precision==-1)
    CalcDisplayPrecision();
  char Buff[64];
  sprintf(Buff, "%.*f", Precision, pCnv->Human(d));
  S = Buff;
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::CreateSeriesFromQ()
  {
  if (m_Data>0.0)
    {
    CalculatedSizes.RemoveAll();
    double X0 = m_DataMin;
    if (m_bQMidPt)
      {
      X0=X0/(1.0+0.5*(Ratio()-1.0));
      }

    CalculatedSizes.Add(X0);
    for (int i=0; (X0<m_DataMax) && (i<MaxIntervals-1); i++)
      {
      X0 *= Ratio();
      CalculatedSizes.Add(X0);
      }
    }
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::CreateSeriesSQRT()
  {
  double lRatio     = sqrt(2.0);
  double lMax       = m_DataMax;
  double lMin       = m_DataMin;

  CalculatedSizes.RemoveAll();
  double X0 = m_DataMin;

  CalculatedSizes.Add(X0);
  for (int i=0; (X0<lMax) && (i<MaxIntervals-1); i++)
    {
    X0 *= lRatio;
    CalculatedSizes.Add(X0);
    }

  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::CreateSeriesFromRatio()
  {
  double lRatio = m_Data;
  double lMax   = m_DataMax;
  double lMin   = m_DataMin;

  if (lRatio>0.0)
    {
    CalculatedSizes.RemoveAll();
    double X0 = m_DataMin;

    CalculatedSizes.Add(X0);
    for (int i=0; (X0<lMax) && (i<MaxIntervals-1); i++)
      {
      X0 *= lRatio;
      CalculatedSizes.Add(X0);
      }
    }
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::CreateSeriesArithmetic()
  {
  double lIncrement = m_Data;
  double lMax       = m_DataMax;
  double lMin       = m_DataMin;

  if (lIncrement>0.0)
    {
    CalculatedSizes.RemoveAll();
    double X0 = m_DataMin;

    CalculatedSizes.Add(X0);
    for (int i=0; (X0<lMax) && (i<MaxIntervals-1); i++)
      {
      X0 += lIncrement;
      CalculatedSizes.Add(X0);
      }
    }
  }

// --------------------------------------------------------------------------

void CSieveSeriesHelper::CreateSeriesFromData()
  {
    switch (m_SeriesDataType)
      {
      case e_Manual: // Manual

        break;
      case e_Q: // Q
        CreateSeriesFromQ();
        break;
      case e_Sqrt: // sqrt(2)
        CreateSeriesSQRT();
        break;
      case e_Ratio: // Ratio
         CreateSeriesFromRatio();
        break;
      case e_Arithmetic: // Arithmetic Increment
        CreateSeriesArithmetic();
        break;

      }
  }

// --------------------------------------------------------------------------

void CSizeDistHelper::SetDefaultTopBottom(CSieveSeriesHelper* pSeries)
  {

  if (pSeries)
    {

  if ( pSeries->m_SeriesDataType == e_Manual )
    {
      if (pSeries->ManualSizes.GetSize()>0)
        {
        dTopSize = pSeries->ManualSizes[pSeries->ManualSizes.GetSize()-1];
        dBottomSize = pSeries->ManualSizes[0]*0.5;
        }
    }
  else
    {
     if (pSeries->CalculatedSizes.GetSize()>0)
        {
        dTopSize = pSeries->CalculatedSizes[pSeries->CalculatedSizes.GetSize()-1];
        dBottomSize = pSeries->CalculatedSizes[0]*0.5;
        }
    }

    }
  else
    {
    dTopSize = 0.1;
    dBottomSize = 0.001;
    }

  dTopSizeDisplay = dTopSize*10.0;
  dBottomSizeDisplay = dBottomSize*0.1;
  }

// =========================================================================
//
//
//
// =========================================================================

static double TylerSeriesInit[] =
  {
    1.66, 2.23, 3.31, 4.69, 6.63, 9.37, 13.26,
    18.75,  26.52, 37.5, 53.03, 75,
    106.07, 150, 212.13, 300, 424.3, 600,
    848.53, 1200, 1697, 2400, 3394, 4800, 6788,
    9600, 13576, 19200, 27153, 38400, 54306, 76800,
    108612, 153600, 217223, 307200, 434446, 614400
  };

// =========================================================================

CSD_CfgHelper::~CSD_CfgHelper()
  {
  for (int i=0; i<SSs.GetSize(); i++)
    delete SSs[i];
  for (i=0; i<SDs.GetSize(); i++)
    delete SDs[i];
  }

// --------------------------------------------------------------------------

CSieveSeriesHelper* CSD_CfgHelper::FindSieveSeries(char* pSeriesName)
  {
  for (int i=0; i<SSs.GetSize(); i++)
    if (SSs[i]->m_Name.XStrICmp(pSeriesName)==0)
      return SSs[i];
  return NULL;
  }

// --------------------------------------------------------------------------

CSizeDistHelper* CSD_CfgHelper::FindDistribution(char* pDistName)
  {
  for (int i=0; i<SDs.GetSize(); i++)
    if (SDs[i]->Name.XStrICmp(pDistName)==0)
      return SDs[i];
  return NULL;
  }

// --------------------------------------------------------------------------
static int ParseParts(Strng &S, char *Parts[], int MaxParts )
  {
  int N=0;
  char * p=S();
  for (int i=0; p && *p && N<MaxParts; i++)
    {
    Parts[i]=p;
    p=strchr(p, ',');
    if (p)
      {
      *p=0;
      p++;
      }
    N++;
    }

  for ( ;i<MaxParts; i++)
    Parts[i]=NULL;

  return N;
  }

// --------------------------------------------------------------------------
void CSD_CfgHelper::Load(CProfINIFile & CfgPF)
  {
  gs_CountFactor = CfgPF.RdDouble("SizeDistributions", "CountFactor", 1.0);

  //load sieve series...
  SSs.SetSize(0);
  SDsMeas.SetSize(0);
  for (int iSerNo=0; ; iSerNo++)
    {
    Strng SeriesName, CnvStr, Val;
    if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Name", SeriesName))
      break;
    if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Cnvs", CnvStr))
      break;

    CSieveSeriesHelper* pSeries = new CSieveSeriesHelper(SeriesName(), CnvStr());
    SSs.Add(pSeries);

    CDataCnv* pC = gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
    if (!pC)
      {
      }

    double Data = 0.0;
    double X0 = 0.0;
    double X1 = 0.0;
    bool FoundNonZeroQ = false;
    SeriesDataType lSeriesDataType = e_Manual;
    if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Q", Val))
    {
      Data=SafeAtoF(Val());
      if (pSeries->m_Data>1.0e-9)
        FoundNonZeroQ = true;
      else
        FoundNonZeroQ = false;
    }

    if (FoundNonZeroQ)
      {
      // For backwards compatability if we find a non zero Q parameter assume
      // that we are using a Q SeriesDataType
      lSeriesDataType = e_Q;

      if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "QMin", Val))
          Val="1";
        X0=SafeAtoF(Val());

        if (pC)
          X0 = pC->Normal(X0, CnvStr());

        if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "QMax", Val))
          Val="100";

        double X1=SafeAtoF(Val());
        if (pC)
          X1 = pC->Normal(X1, CnvStr());


    }
    else
      {
        if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "SeriesDataType", Val))
          lSeriesDataType=(SeriesDataType)SafeAtoL(Val());

        if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Data", Val))
          Data = SafeAtoF(Val());

        if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "DataMin", Val))
            Val="1";
          X0=SafeAtoF(Val());

          if (pC)
            X0 = pC->Normal(X0, CnvStr());

          if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "DataMax", Val))
            Val="100";

          X1=SafeAtoF(Val());
          if (pC)
            X1 = pC->Normal(X1, CnvStr());

      }


    bool IsMidPt=false;
    if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "QIsMidPt", Val))
      {
      long L=SafeAtoL(Val());
      IsMidPt=(L!=0);
      }

   long Precision=2;
   if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Precision", Val))
      {
      Precision=SafeAtoL(Val());
      Precision = min(Precision,2);
      }

    if (X1<X0)
      X1 = X0*10.0;

    pSeries->m_SeriesDataType=lSeriesDataType;
    pSeries->m_Data=Data;
    pSeries->m_DataMin=X0;
    pSeries->m_DataMax=X1;
    pSeries->m_bQMidPt=IsMidPt;
    pSeries->Precision=Precision;


    if (pSeries->m_SeriesDataType != e_Manual)
      {
      // Create Series from Parameters
      pSeries->CreateSeriesFromData();

      }
    else
      {
      // Load Manual Series Data
      for (int n=0; ; n++)
        {
        if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Pt", n, Val))
          {
          if (pSeries->ManualSizes.GetCount()<MaxIntervals-1)
            {
          double I=SafeAtoF(Val());
          if (pC)
            I = pC->Normal(I, CnvStr());
          pSeries->ManualSizes.Add(I);
          }
          }
        else
          break;
        }
      }

    pSeries->SortSizes();
    }
  //ensure at least one sieve series exists
  if (SSs.GetSize()==0)
    CreateDefaultSS();

  //load size distributions definitions...
  SDs.SetSize(0);
  for (int iDistNo=0; ; iDistNo++)
    {
    Strng DistName, SeriesName;

    if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Name", DistName))
      break;
    if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Series", SeriesName))
      break;

    CSizeDistHelper* pSDist = new CSizeDistHelper(DistName(), SeriesName());
    SDs.Add(pSDist);

    CDistMeasHelper* pSDistMeas = new CDistMeasHelper();
    SDsMeas.Add(pSDistMeas);

    CSieveSeriesHelper* pSeries = FindSieveSeries(SeriesName());
    pSDist->SetDefaultTopBottom(pSeries);

    DoTopBottom(pSDist->dBottomSize,        CfgPF, iDistNo, "BSize");
    DoTopBottom(pSDist->dTopSize,           CfgPF, iDistNo, "TSize");
    DoTopBottom(pSDist->dBottomSizeDisplay, CfgPF, iDistNo, "BSizeDisp");
    DoTopBottom(pSDist->dTopSizeDisplay,    CfgPF, iDistNo, "TSizeDisp");
    //pSDist->dBottomSizeDisplay = Min(pSDist->dBottomSize*0.5, pSDist->dBottomSizeDisplay); //change display size???
    //pSDist->dTopSizeDisplay = Max(pSDist->dTopSize*2.0, pSDist->dTopSizeDisplay); //change display size???
    }
  //ensure at least one distribution definition exists
  if (SDs.GetSize()==0)
    CreateDefaultSD();
  //ensure at least one PSD Measurement definition exists
  if (SDsMeas.GetSize()==0)
    CreateDefaultMeasurement();

  //load specie data used by distributions...
  iDistNo = 0;
  CSizeDistHelper* pSDist = SDs[iDistNo];
  for (int N=0; ; N++)
    {
    Strng Specie;
    if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Sp", N, Specie))
      break;

    pSDist->Species.Add(Specie);
    }
  Strng S;
  S.Set("Dist%02i.DefaultSpecie", iDistNo);
  pSDist->DefaultSpecie=CfgPF.RdStr("SizeDistributions", S(), pSDist->Species.GetSize()>0 ? (pSDist->Species[0])() : "");
  //force all other distributions to use same specie cfg...
  for (iDistNo = 1; iDistNo<SDs.GetSize(); iDistNo++)
    {
    pSDist = SDs[iDistNo];
    for (int i=0; i<SDs[0]->Species.GetSize(); i++)
      pSDist->Species.Add(SDs[0]->Species[i]);
    pSDist->DefaultSpecie = SDs[0]->DefaultSpecie;
    }

  // Load the Size Distribution Measurement Definitions
  for (iDistNo = 0; iDistNo<SDs.GetSize(); iDistNo++)
    {
    Strng DistName;

    if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Name", DistName))
      break;

    for (int m=0; m<MaxCSDMeasurements; m++)
      {
      Strng Meas, S, CnvStr;
      if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Meas", m, Meas))
        break;

      char * Parts[10];
      int nParts=ParseParts(Meas, Parts, 10);
      if (nParts<1)
        break;

      S=Parts[0];
      CSD_Measurement M;
      CMeasInfo * pInfo=NULL;
      for (int i=0; MeasInfo[i].T!=eSDMT_Null; i++)
        {
        if (S.XStrICmp(MeasInfo[i].Name)==0 || S.XStrICmp(MeasInfo[i].Abr)==0)
          {
          pInfo=&MeasInfo[i];
          M.m_eType = MeasInfo[i].T;
          break;
          }
        }

      if (M.m_eType==eSDMT_Null)
        break;

      if (M.m_eType==eSDMT_Text)
        {
        M.m_sName = (nParts>=2)?Parts[1]:" ";
        SDsMeas[iDistNo]->Measurements.Add(M);
        continue;
        }
      if (nParts<pInfo->RqdParts)
        break;

      M.m_sName = Parts[1];
      if (M.m_eType==eSDMT_SizePassFrac || M.m_eType==eSDMT_FracPassSize)
        {
        M.m_dValue=SafeAtoF(Parts[2],1);
        S=Parts[2];
        char * p=S.XStrChr(' ');
        CnvStr="";
        if (p)
          {
          CnvStr=p;
          CnvStr.LRTrim();
          }
        if (CnvStr.GetLength()==0)
          CnvStr=pInfo->sValCnv;

        //CDataCnv *pC=p ? gs_CnvsMngr.FindSecCnv(pInfo->iValCnv, CnvStr()) : NULL;
        //if (pC)
        //  M.m_dValue=pC->Normal(M.m_dValue, CnvStr());

        M.ValCnv.SetText(CnvStr);

        if (M.m_eType==eSDMT_SizePassFrac)
          M.ValCnv.SetIndex(DC_Frac);
        else if (M.m_eType==eSDMT_FracPassSize)
          M.ValCnv.SetIndex(DC_L);

        M.m_dValue=M.ValCnv.Normal(M.m_dValue);

        }

      M.Cnv.SetIndex(pInfo->iDspCnv);
      CnvStr="";
      if (Parts[3])
        {
        CnvStr=Parts[3];
        CnvStr.LRTrim();
        }
      if (CnvStr.GetLength()==0)
        CnvStr=pInfo->sDspCnv;
      M.Cnv.SetText(CnvStr());

      SDsMeas[iDistNo]->Measurements.Add(M);
      }
    }

  }

// --------------------------------------------------------------------------

void CSD_CfgHelper::Save(CProfINIFile & CfgPF)
  {
  CfgPF.WrDouble("SizeDistributions", "CountFactor", gs_CountFactor);

  //save sieve series...
  for (int iSerNo=0; iSerNo<SSs.GetSize(); iSerNo++)
    {
    CDataCnv* pC = gs_CnvsMngr.FindSecCnv(DC_L, SSs[iSerNo]->Units());    Strng Sect;
    SSs[iSerNo]->m_Name.LRTrim();
    if (SSs[iSerNo]->m_Name.Len()==0)
      SSs[iSerNo]->m_Name = "New";
    Sect.Set("Size%02i", iSerNo);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "Name",           SSs[iSerNo]->m_Name());
    WRCFG(CfgPF, "SizeDistributions", Sect(), "Cnvs",           SSs[iSerNo]->Units());
    WRCFG(CfgPF, "SizeDistributions", Sect(), "SeriesDataType", (long)SSs[iSerNo]->m_SeriesDataType);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "Data",           SSs[iSerNo]->m_Data);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "DataMin",        pC ? pC->Human(SSs[iSerNo]->m_DataMin) : SSs[iSerNo]->m_DataMin);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "DataMax",        pC ? pC->Human(SSs[iSerNo]->m_DataMax) : SSs[iSerNo]->m_DataMax);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "QIsMidPt",       (long)SSs[iSerNo]->m_bQMidPt);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "Precision",      (long)SSs[iSerNo]->Precision);

    int i=0;
    double PrevSize = -1.0;
    if (SSs[iSerNo]->m_SeriesDataType == e_Manual)
      {
      // Only write series data for manual data entry type
      for (int n=0; n<SSs[iSerNo]->ManualSizes.GetSize(); n++)
        {
        const double d = SSs[iSerNo]->ManualSizes[n];
        if (d>0.0 && d!=PrevSize)
          {
          WRCFG4(CfgPF, "SizeDistributions", Sect(), "Pt", i++, pC ? pC->Human(d) : d);
          PrevSize = d;
          }
        }
      }

    while (1)
      {
      Strng Old;
      if (!RDCFG4(CfgPF, "SizeDistributions", Sect(), "Pt", i, Old))
        break;
      WRCFG4(CfgPF, "SizeDistributions", Sect(), "Pt", i, (char*)NULL);
      i++;
      }

    }

  //save size distributions definitions...
  for (int iDistNo=0; iDistNo<SDs.GetSize(); iDistNo++)
    {
    CSieveSeriesHelper* pSeries = FindSieveSeries(SDs[iDistNo]->SieveSeriesName());
    if (pSeries)
      {
      SDs[iDistNo]->Name.LRTrim();
      if (SDs[iDistNo]->Name.Len()==0)
        SDs[iDistNo]->Name = "Std";
      Strng Sect;
      Sect.Set("Dist%02i", iDistNo);
      WRCFG(CfgPF, "SizeDistributions", Sect(), "Name", SDs[iDistNo]->Name());
      WRCFG(CfgPF, "SizeDistributions", Sect(), "Series", SDs[iDistNo]->SieveSeriesName());

      CDataCnv* pC = gs_CnvsMngr.FindSecCnv(DC_L, pSeries->Units());
      if (pC==NULL)
        pC = gs_CnvsMngr.SICnv(DC_L);

      //check that various top/bottom sizes are meaningfull...
      if (pSeries->m_SeriesDataType == e_Manual)
      {
      SDs[iDistNo]->dTopSize = pSeries->ManualSizes.GetSize()>0 ? pSeries->ManualSizes[pSeries->ManualSizes.GetSize()-1] : 0.1;
      if (pSeries->ManualSizes.GetSize()>0 && SDs[iDistNo]->dBottomSize>=pSeries->ManualSizes[0])
        SDs[iDistNo]->dBottomSize = pSeries->ManualSizes[0]*0.5;
      if (SDs[iDistNo]->dBottomSize>=SDs[iDistNo]->dTopSize)
        SDs[iDistNo]->dBottomSize = SDs[iDistNo]->dTopSize*0.1;
      if (SDs[iDistNo]->dBottomSizeDisplay>=SDs[iDistNo]->dTopSizeDisplay)
        SDs[iDistNo]->dBottomSizeDisplay = SDs[iDistNo]->dTopSizeDisplay*0.1;
      }
      else
        {
      SDs[iDistNo]->dTopSize = pSeries->CalculatedSizes.GetSize()>0 ? pSeries->CalculatedSizes[pSeries->CalculatedSizes.GetSize()-1] : 0.1;
      if (pSeries->CalculatedSizes.GetSize()>0 && SDs[iDistNo]->dBottomSize>=pSeries->CalculatedSizes[0])
        SDs[iDistNo]->dBottomSize = pSeries->CalculatedSizes[0]*0.5;
      if (SDs[iDistNo]->dBottomSize>=SDs[iDistNo]->dTopSize)
        SDs[iDistNo]->dBottomSize = SDs[iDistNo]->dTopSize*0.1;
      if (SDs[iDistNo]->dBottomSizeDisplay>=SDs[iDistNo]->dTopSizeDisplay)
        SDs[iDistNo]->dBottomSizeDisplay = SDs[iDistNo]->dTopSizeDisplay*0.1;
        }

      char Buff[256];
      sprintf(Buff, "%.20g %s", pC->Human(SDs[iDistNo]->dTopSize), pC->Txt());
      WRCFG(CfgPF, "SizeDistributions", Sect(), "TSize", Buff);
      sprintf(Buff, "%.20g %s", pC->Human(SDs[iDistNo]->dBottomSize), pC->Txt());
      WRCFG(CfgPF, "SizeDistributions", Sect(), "BSize", Buff);
      sprintf(Buff, "%.20g %s", pC->Human(SDs[iDistNo]->dTopSizeDisplay), pC->Txt());
      WRCFG(CfgPF, "SizeDistributions", Sect(), "TSizeDisp", Buff);
      sprintf(Buff, "%.20g %s", pC->Human(SDs[iDistNo]->dBottomSizeDisplay), pC->Txt());
      WRCFG(CfgPF, "SizeDistributions", Sect(), "BSizeDisp", Buff);

      //Save the Distribution Measurement Configuration

      int m;
      for (m=0; m<SDsMeas[iDistNo]->Measurements.GetSize(); m++)
      {
        CSD_Measurement M = SDsMeas[iDistNo]->Measurements[m];

        Strng MeasStr;

        // Type Name
        Strng TypeName;
        int lMeasInfoIndex = 0;
        for (lMeasInfoIndex=0; MeasInfo[lMeasInfoIndex].T!=eSDMT_Null;lMeasInfoIndex++)
          if (M.m_eType == MeasInfo[lMeasInfoIndex].T)
            {
             TypeName = MeasInfo[lMeasInfoIndex].Name;
             break;
            }

        // Description Name
        Strng DescName = M.m_sName;
        //long  RqdParts = MeasInfo[M.m_eType].RqdParts;

        // Value NB: Conversion for Value is not currently stored in CSD_Measurement
        // for modification - TO DO:

        //CDataCnv *pC=gs_CnvsMngr.FindSecCnv(MeasInfo[lMeasInfoIndex].iValCnv, MeasInfo[lMeasInfoIndex].sValCnv);
        // Conversion for Result
        char* lValCnvTxt = M.ValCnv.Text();
        double dValue = M.m_dValue;
        //if (pC)
        // dValue= pC->Human(M.m_dValue);
        dValue = M.ValCnv.Human(dValue);
        char dValueBuff[80];
        sprintf(dValueBuff,"%g",dValue);

        // Conversion for Result
        char* lCnvTxt = M.Cnv.Text();

        if ((M.m_eType == eSDMT_FracPassSize )||(M.m_eType == eSDMT_SizePassFrac ))
          {
            MeasStr += TypeName;
            MeasStr += ",";
            MeasStr += DescName;
            MeasStr += ",";
            MeasStr += dValueBuff;
            MeasStr += " ";
            //MeasStr += MeasInfo[lMeasInfoIndex].sValCnv;
            MeasStr += lValCnvTxt;
            MeasStr += ",";
            MeasStr += M.Cnv.Text();
          }
        else
          {
            MeasStr += TypeName;
            MeasStr += ",";
            MeasStr += DescName;

          }

        WRCFG4(CfgPF, "SizeDistributions", Sect() , "Meas", m, MeasStr.Str());

      }

      // Clear previous measurement entries that are no longer used
      while (1)
      {
      Strng Old;
      if (!RDCFG4(CfgPF, "SizeDistributions", Sect(), "Meas", m, Old))
        break;
      WRCFG4(CfgPF, "SizeDistributions", Sect(), "Meas", m, (char*)NULL);
      m++;
      }

      }
    }
  if (1)
    {//ensure next distribution is seen as 'empty'
    Strng Sect;
    Sect.Set("Dist%02i", iDistNo);
    WRCFG(CfgPF, "SizeDistributions", Sect(), "Name", "");
    WRCFG(CfgPF, "SizeDistributions", Sect(), "Series", "");
    }

  if (SDs.GetSize()>0)
    {
    //save specie information from the first distribution only
    iDistNo = 0;
    Strng Sect;
    Sect.Set("Dist%02i", iDistNo);
    for (int i=0; i<SDs[iDistNo]->Species.GetSize(); i++)
      WRCFG4(CfgPF, "SizeDistributions", Sect(), "Sp", i, SDs[iDistNo]->Species[i]());
    WRCFG4(CfgPF, "SizeDistributions", Sect(), "Sp", i, "");//last entry blank
    WRCFG(CfgPF, "SizeDistributions", Sect(), "DefaultSpecie", SDs[iDistNo]->DefaultSpecie());
    }
  }

// --------------------------------------------------------------------------

int CSD_CfgHelper::CreateDefaultSD()
  {
  int index = -1;
  if (SSs.GetSize()>0)
    {
    CSieveSeriesHelper* pSeries = SSs[0];
    CSizeDistHelper* pSDist;
    int i=0;
    char Buff[16];
    Buff[0] = 0;
    while (Buff[0]==0)
      {
      sprintf(Buff, "Dist%d", i);
      pSDist = FindDistribution(Buff);
      if (pSDist)
        {
        i++;
        Buff[0] = 0;
        }
      }
    pSDist = new CSizeDistHelper(Buff, pSeries->m_Name());
    index = SDs.Add(pSDist);

    pSDist->SetDefaultTopBottom(pSeries);

    /*for (int N=0; ; N++)
      {
      ????
      pSDist->Species.Add(Specie);
      }*/

    pSDist->DefaultSpecie = (pSDist->Species.GetSize()>0 ? (pSDist->Species[0])() : "");
    }
  return index;
  }

// --------------------------------------------------------------------------

int CSD_CfgHelper::CreateDefaultSS()
  {
  CSieveSeriesHelper* pSeries;
  int i=0;
  char Buff[16];
  Buff[0] = 0;
  while (Buff[0]==0)
    {
    sprintf(Buff, "Tyler%d", i);
    pSeries = FindSieveSeries(Buff);
    if (pSeries)
      {
      i++;
      Buff[0] = 0;
      }
    }

  pSeries = new CSieveSeriesHelper(Buff, "um");
  pSeries->CreateSeriesFromRatio();
  int index = SSs.Add(pSeries);
  /*int n = sizeof(TylerSeriesInit)/sizeof(TylerSeriesInit[0]);
  for (i=0; i<n; i++)
    {
    double I = TylerSeriesInit[i]*1.0e-6;
    pSeries->Sizes.Add(I);
    }*/
  return index;
  }

// --------------------------------------------------------------------------

int CSD_CfgHelper::CreateDefaultMeasurement()
  {
  CDistMeasHelper* pSDistMeas = new CDistMeasHelper();
  int index = SDsMeas.Add(pSDistMeas);
  return index;
  }

// --------------------------------------------------------------------------

void CSD_CfgHelper::RemoveDistribution(int index)
  {
  if (index<SDs.GetSize())
    {
    delete SDs[index];
    SDs[index] = NULL;
    SDs.RemoveAt(index);
    }
  }

// --------------------------------------------------------------------------

void CSD_CfgHelper::RemoveSieveSeries(int index)
  {
  if (index<SSs.GetSize())
    {
    delete SSs[index];
    SSs[index] = NULL;
    SSs.RemoveAt(index);
    }
  }

// ==========================================================================

// ==========================================================================

inline char * NextActualToken(char * Skip, Strng & CurTkn, CTokenFile &Tkns)
  {
  CurTkn=Tkns.NextToken();
  if (Skip && CurTkn.XStrICmp(Skip)==0)
    CurTkn=Tkns.NextToken();
  return CurTkn();
  }

// --------------------------------------------------------------------------

inline flag ListContinues(Strng & CurTkn, CTokenFile &Tkns)
  {
  CurTkn=Tkns.NextToken();
  if (CurTkn.XStrICmp(",")==0)
    {
    CurTkn=Tkns.NextToken();
    return true;
    }
  return false;
  }

inline double ToMM(double x) { return x*1000.0; };

// ==========================================================================
//
//
//
// ==========================================================================

SfeSD_Defn::SfeSD_Defn()
 //:
 // XCnv    (DC_L,       "um"),
 // YFCnv   (DC_Frac,    "%"),
 // YNFCnv  (DC_Frac,    "%"),
 // YQmCnv  (DC_Qm,      "kg/s"),
 // YQnCnv  (DC_CntRate, "#.10^6/s"),
 // YMCnv   (DC_M,       "kg"),
 // YNCnv   (DC_Cnt,     "#.10^6"),
 // YNpMCnv (DC_SpCnt,   "#.10^6/g"),
 // TBCnv   (DC_L,       "um"),
 // XFmt    ("", 0, 1, 'f'),
 // YFFmt   ("", 0, 2, 'f'),
 // YNFFmt  ("", 0, 2, 'f'),
 // YQmFmt  ("", 0, 2, 'f'),
 // YQnFmt  ("", 0, 3, 'e'),
 // YMFmt   ("", 0, 2, 'f'),
 // YNFmt   ("", 0, 3, 'e'),
 // YNpMFmt ("", 0, 3, 'e'),
 // TBFmt   ("", 0, 1, 'f')
  {
  Clear();
  }

// --------------------------------------------------------------------------

SfeSD_Defn::~SfeSD_Defn()
  {
  //for (int i=0; i<Intervals.GetSize(); i++)
//  for (int i=0; i<NIntervals(); i++)
//    delete Intervals[i];
//  //for (int j=0; j<Distributions.GetSize(); j++)
//  for (int j=0; j<NDistributions(); j++)
//    delete Distributions[j];
  Clear();
  }

void SfeSD_Defn::Clear()
  {
  XCnv    .Set(DC_L,       "um");
  YFCnv   .Set(DC_Frac,    "%");
  YNFCnv  .Set(DC_Frac,    "%");
  YQmCnv  .Set(DC_Qm,      "kg/s");
  YQnCnv  .Set(DC_CntRate, "#.10^6/s");
  YMCnv   .Set(DC_M,       "kg");
  YNCnv   .Set(DC_Cnt,     "#.10^6");
  YNpMCnv .Set(DC_SpCnt,   "#.10^6/g");
  TBCnv   .Set(DC_L,       "um");
  XFmt    .Set("", 0, 1, 'f');
  YFFmt   .Set("", 0, 2, 'f');
  YNFFmt  .Set("", 0, 2, 'f');
  YQmFmt  .Set("", 0, 2, 'f');
  YQnFmt  .Set("", 0, 3, 'e');
  YMFmt   .Set("", 0, 2, 'f');
  YNFmt   .Set("", 0, 3, 'e');
  YNpMFmt .Set("", 0, 3, 'e');
  TBFmt   .Set("", 0, 1, 'f');

  for (int i=0; i<NIntervals(); i++)
    delete Intervals[i];
  //for (int j=0; j<Distributions.GetSize(); j++)
  for (int j=0; j<NDistributions(); j++)
    delete Distributions[j];
  Intervals.SetSize(0);
  Distributions.SetSize(0);
  LblStyle = LS_Short;//LS_Full;
  //SQSzDist1Class.SetSelectable(false); what is this doing here!!!
  }
// --------------------------------------------------------------------------

/*flag SfeSD_Defn::Parse(Strng &What, Strng & CurTkn, CTokenFile &Tkns)
  {
  flag ParseOK=false;

  if (What.XStrICmp("Size_Series")==0)
    {
    ParseOK=true;
    ParseSeries(CurTkn, Tkns);
    }
  else if (What.XStrICmp("Tyler_Series")==0)
    {
    ParseOK=true;
    ParseTyler(CurTkn, Tkns);
    }
  else if (What.XStrICmp("Distribution_Graphics")==0)
    {
    ParseOK=true;
    NextActualToken("=", CurTkn, Tkns);
    SQSzDist1Edt::iGraphOn=SafeAtoL(CurTkn());
    CurTkn=Tkns.NextToken();
    }
  else if (What.XStrICmp("Partition_Graphics")==0)
    {
    ParseOK=true;
    NextActualToken("=", CurTkn, Tkns);
    SzPartCrv1Edt::iGraphOn=SafeAtoL(CurTkn());
    CurTkn=Tkns.NextToken();
    }
  else if (What.XStrICmp("Intervals_Ascending")==0)
    {
    ParseOK=true;
    NextActualToken("=", CurTkn, Tkns);
    fSzAscend=SafeAtoL(CurTkn())!=0;
    CurTkn=Tkns.NextToken();
    }
  else if (What.XStrICmp("Size_Distribution")==0)
    {
    if (NDistributions()<MaxDistributions)
      {
      ParseOK=true;
      ParseDistribution(CurTkn, Tkns);
      }
    else
      LogError("SzDist", 0, "Max %i Distributions", MaxDistributions);
    }

  return ParseOK;
  }*/

// --------------------------------------------------------------------------
/*
static int ParseParts(Strng &S, char *Parts[], int MaxParts )
  {
  int N=0;
  char * p=S();
  for (int i=0; p && *p && N<MaxParts; i++)
    {
    Parts[i]=p;
    p=strchr(p, ',');
    if (p)
      {
      *p=0;
      p++;
      }
    N++;
    }

  for ( ;i<MaxParts; i++)
    Parts[i]=NULL;

  return N;
  };
*/

// --------------------------------------------------------------------------

flag SfeSD_Defn::LoadConfiguration(CProfINIFile & CfgPF)
  {
  Clear();

  CSD_CfgHelper SDCfg;
  SDCfg.Load(CfgPF);
  for (int i=0; i<SDCfg.SSs.GetSize(); i++)
    {
    CSieveSeriesHelper& SSH = *(SDCfg.SSs[i]);
    Intervals.Add(new CSD_Intervals);
    CSD_Intervals& Series = *(Intervals[i]);
    Series.m_sName = SSH.m_Name;
    //Series.m_Q=SSH.m_Data;
    Series.m_SeriesDataType = SSH.m_SeriesDataType;
    Series.m_Data=SSH.m_Data;
    Series.SetSize(0);
    if (SSH.m_SeriesDataType == e_Manual)
      {
        for (int j=0; j<SSH.ManualSizes.GetSize(); j++)
          Series.Add(SSH.ManualSizes[j]);
      }
    else
      {
        for (int j=0; j<SSH.CalculatedSizes.GetSize(); j++)
          Series.Add(SSH.CalculatedSizes[j]);
      }


    }

  for (int iDistNo=0; iDistNo<SDCfg.SDs.GetSize(); iDistNo++)
    {
    CSizeDistHelper& SDH = *(SDCfg.SDs[iDistNo]);
    Distributions.Add(new CSD_DistDefn);
    CSD_DistDefn& Dist = *(Distributions[iDistNo]);
    Dist.sName = SDH.Name;
    Dist.dBottomSize = SDH.dBottomSize;
    Dist.dTopSize = SDH.dTopSize;
    Dist.dBottomSizeDisp = SDH.dBottomSizeDisplay;
    Dist.dTopSizeDisp = SDH.dTopSizeDisplay;

    CSD_Intervals *pInt = NULL;   // Pointer to Actual Intervals;
    for (int i=0; i<Intervals.GetSize(); i++)
      if (SDH.SieveSeriesName.XStrICmp(Intervals[i]->m_sName)==0)
        {
        pInt = Intervals[i];
        break;
        }

    if (pInt==NULL && Intervals.GetSize()>0)
      {//error!  not found, use first available
      pInt = Intervals[0];
      LogWarning("SizeDist", 0, "For distribution '%s':  Sieve series '%s' not found, using '%s'", Dist.sName(), SDH.SieveSeriesName(), pInt->m_sName());
      SDH.SieveSeriesName = pInt->m_sName;
      }

    if (pInt)
      {
      CSD_Intervals &Intervals = *pInt;
      Dist.Intervals.m_sName = Intervals.m_sName;
      //Dist.Intervals.m_Q = Intervals.m_Q;
      Dist.Intervals.m_SeriesDataType = Intervals.m_SeriesDataType;
      Dist.Intervals.m_Data=Intervals.m_Data;


      for (int i=0; i<Intervals.GetSize(); i++)
        if ((Intervals[i]>=Dist.dBottomSize) &&
            ((Dist.Intervals.GetSize()==0) || (Intervals[i]<=Dist.dTopSize)))
          Dist.Intervals.Add(Intervals[i]);
      if (Dist.Intervals.GetSize()==0 || Dist.Intervals[Dist.Intervals.GetSize()-1]<Dist.dTopSize)
        Dist.Intervals.Add(Dist.dTopSize); // Oversize Interval

      if (Dist.dBottomSizeDisp>Dist.Intervals[0])
        {
        LogWarning("SizeDist", 0, "For distribution '%s':  Graph display bottom size %g is greater than sieve series bottom size %g",
                    Dist.sName(),Dist.dBottomSizeDisp,Dist.Intervals[0]);
        }
      if (Dist.dTopSizeDisp<Dist.Intervals[Dist.Intervals.GetSize()-1])
        {
        LogWarning("SizeDist", 0, "For distribution '%s':  Graph display top size %g is less than sieve series top size %g",
                   Dist.sName(),Dist.dTopSizeDisp,Dist.Intervals[Dist.Intervals.GetSize()-1]);
        }
      }

    //kga July 99: Code is still lying arround to have multiple distributions, each
    //with multiple interval series, each with multiple species; various column
    //options existed.  Current assuption is that one distribution is used with
    //one series with a list of species.
    //Need to check if variations work! Cannot mix distributions at a unit/pipe; maybe
    //different distributions can be used in different areas of the flowsheet?; etc
    #ifdef OldStuff
    for (int N=0; ; N++)
      {
      Strng Species;
      if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Sp", N, Species))
        break;

      char * Parts[256];
      int nParts=ParseParts(Species, Parts, 256);
      if (nParts==0)
        break;

      Dist.sSzNames.SetSize(N+1);
      for (int iSp=0; iSp<nParts; iSp++)
        {
        Strng S(Parts[iSp]);
        if (S.XStrICmp("$Default$")!=0)
          Dist.sSzNames[N].Add(S);
        else
          Dist.iDefaultDist=N;
        }
      }

    for (int C=0; ; C++)
      {
      Strng Column;
      if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Col", C, Column))
        break;

      char * Parts[10];
      int nParts=ParseParts(Column, Parts, 10);
      if (nParts!=2)
        break;
      CSD_ColumnOpt C;
      C.sName=Parts[1];
      C.iOpt=SafeAtoI(Parts[0]);
      Dist.ColumnOpts.Add(C);
      }
    #else
    Dist.sSzNames.SetSize(SDH.Species.GetSize());
    for (int N=0; N<SDH.Species.GetSize(); N++)
      {
      Dist.sSzNames[N].Add(SDH.Species[N]);
      if (SDH.DefaultSpecie.XStrICmp(SDH.Species[N])==0)
        Dist.iDefaultDist=N;
      }
    #endif


    for (int m=0; m<SDCfg.SDsMeas[iDistNo]->Measurements.GetSize(); m++)
      {
      CSD_Measurement M = SDCfg.SDsMeas[iDistNo]->Measurements[m];
      Dist.Measurements.Add(M);
      }

#ifdef NOTUSINGMEASHELPER
    for (int m=0; m<MaxCSDMeasurements; m++)
      {
      Strng Meas, S, CnvStr;
      if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Meas", m, Meas))
        break;

      char * Parts[10];
      int nParts=ParseParts(Meas, Parts, 10);
      if (nParts<1)
        break;

      static struct CMeasInfo
        {
          eSDMeasTypes T;
          char * Abr;
          char * Name;
          long RqdParts;
          CCnvIndex iValCnv;
          char * sValCnv;
          CCnvIndex iDspCnv;
          char * sDspCnv;
        } MeasInfo[] =
        {
          {eSDMT_FracPassSize,  "F", "FracPassSize", 3, DC_L,    "um",   DC_Frac,      "%"     },
          {eSDMT_SizePassFrac,  "S", "SizePassFrac", 3, DC_Frac, "Frac", DC_L,         "um"    },
          {eSDMT_PPG,           "P", "PPG",          2, DC_,     "",     DC_SpCnt,     "#/g"   },
          {eSDMT_SAM,           "M", "SAM",          2, DC_,     "",     DC_SurfAreaM, "m^2/g" },
          {eSDMT_SAL,           "V", "SAL",          2, DC_,     "",     DC_SurfAreaL, "m^2/L" },
          {eSDMT_Text,          "T", "Text",         1, DC_,     "",     DC_,          ""      },
          {eSDMT_Null}
        };

      S=Parts[0];
      CSD_Measurement M;
      CMeasInfo * pInfo=NULL;
      for (int i=0; MeasInfo[i].T!=eSDMT_Null; i++)
        {
        if (S.XStrICmp(MeasInfo[i].Name)==0 || S.XStrICmp(MeasInfo[i].Abr)==0)
          {
          pInfo=&MeasInfo[i];
          M.m_eType = MeasInfo[i].T;
          break;
          }
        }

      if (M.m_eType==eSDMT_Null)
        break;

      if (M.m_eType==eSDMT_Text)
        {
        M.m_sName = (nParts>=2)?Parts[1]:" ";
        Dist.Measurements.Add(M);
        continue;
        }
      if (nParts<pInfo->RqdParts)
        break;

      M.m_sName = Parts[1];
      if (M.m_eType==eSDMT_SizePassFrac || M.m_eType==eSDMT_FracPassSize)
        {
        M.m_dValue=SafeAtoF(Parts[2],1);
        S=Parts[2];
        char * p=S.XStrChr(' ');
        CnvStr="";
        if (p)
          {
          CnvStr=p;
          CnvStr.LRTrim();
          }
        if (CnvStr.GetLength()==0)
          CnvStr=pInfo->sValCnv;
        CDataCnv *pC=p ? gs_CnvsMngr.FindSecCnv(pInfo->iValCnv, CnvStr()) : NULL;
        if (pC)
          M.m_dValue=pC->Normal(M.m_dValue, CnvStr());
        }
      M.Cnv.SetIndex(pInfo->iDspCnv);
      CnvStr="";
      if (Parts[4])
        {
        CnvStr=Parts[4];
        CnvStr.LRTrim();
        }
      if (CnvStr.GetLength()==0)
        CnvStr=pInfo->sDspCnv;
      M.Cnv.SetText(CnvStr());

      Dist.Measurements.Add(M);
      }
#endif

    // No measurements defined, add defaults...
    if (Dist.Measurements.GetSize()==0)
      {
      struct dSizeInfo {eSDMeasTypes Typ; double d; char * Name; CCnvIndex iValCnv; char * sDspCnv; };
      const dSizeInfo dSzInfo[] =
        {
          //{eSDMT_SizePassFrac, 0.99, "d99", DC_L, "um"},
          {eSDMT_SizePassFrac, 0.80, "d80", DC_L, "um"},
          {eSDMT_SizePassFrac, 0.50, "d50", DC_L, "um"},
          //{eSDMT_SizePassFrac, 0.30, "d30", DC_L, "um"},
          //{eSDMT_SizePassFrac, 0.20, "d20", DC_L, "um"},
          {eSDMT_Text,         0.0, "", DC_, ""},
          {eSDMT_FracPassSize, 0.0001, "m100", DC_Frac, "%"},
          {eSDMT_Text,         0.0, "", DC_, ""},
          {eSDMT_PPG,          0.0, "PPG", DC_SpCnt,     "#/g" },
          {eSDMT_SAM,          0.0, "SAM", DC_SurfAreaM, "m^2/g" },
          {eSDMT_SAL,          0.0, "SAL", DC_SurfAreaL, "m^2/L" },
        };
      const int ActDSizes=sizeof(dSzInfo)/sizeof(dSzInfo[0]);
      for (int i=0; i<ActDSizes; i++)
        {
        CSD_Measurement M;
        M.m_eType  = dSzInfo[i].Typ;
        M.m_dValue = dSzInfo[i].d;
        M.m_sName  = dSzInfo[i].Name;
        M.Cnv.SetIndex(dSzInfo[i].iValCnv);
        M.Cnv.SetText(dSzInfo[i].sDspCnv);
        Dist.Measurements.Add(M);
        }
      }

    }


  return true;
  }

// --------------------------------------------------------------------------

/*flag SfeSD_Defn::LoadConfiguration(CProfINIFile & CfgPF)
  {
  //SQSzDist1Edt::iGraphOn=CfgPF.RdInt("SizeDistributions", "Distribution_Graphics", SQSzDist1Edt::iGraphOn);
  //SzPartCrv1Edt::iGraphOn=CfgPF.RdInt("SizeDistributions", "Partition_Graphics", SzPartCrv1Edt::iGraphOn);
  //fSzAscend=CfgPF.RdInt("SizeDistributions", "Intervals_Ascending", fSzAscend);

  for (int iSerNo=0; ; iSerNo++)
    {
    Strng SeriesName, CnvStr, Val;
    if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Name", SeriesName))
      break;
    if (!RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Cnvs", CnvStr))
      break;

    Intervals.Add(new CSD_Intervals);
    CSD_Intervals &Series=*(Intervals[Intervals.GetUpperBound()]);
    Series.sName=SeriesName;
    Series.SetSize(0);

    CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
    if (!pC)
      {
      }
    for (int n=0; ; n++)
      {
      if (RDCFG(CfgPF, "SizeDistributions", "Size", iSerNo, "Pt", n, Val))
        {
        double I=SafeAtoF(Val());
        if (pC)
          I=pC->Normal(I, CnvStr());
        Series.Add(I);
        }
      else
        break;
      }
    }

  for (int iDistNo=0; ; iDistNo++)
    {
    Strng DistName, SeriesName;//, BottomSize, TopSize, BottomSizeDisp, TopSizeDisp;

    if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Name", DistName))
      break;
    if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Series",    SeriesName))
      break;
    //RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "BSize",     BottomSize);
    //RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "TSize",     TopSize);
    //RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "BSizeDisp", BottomSizeDisp);
    //RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "TSizeDisp", TopSizeDisp);

    Distributions.Add(new CSD_DistDefn);
    CSD_DistDefn &Dist=*(Distributions[Distributions.GetUpperBound()]);
    Dist.sName=DistName;

    CSD_Intervals *pInt=NULL;   // Pointer to Actual Intervals;
    for (int i=0; i<Intervals.GetSize(); i++)
      if (SeriesName.XStrICmp(Intervals[i]->sName)==0)
        {
        pInt=Intervals[i];
        Dist.dBottomSize=(*pInt)[0];
        Dist.dTopSize=(*pInt)[(*pInt).GetUpperBound()];
        Dist.dBottomSizeDisp=(*pInt)[0]*0.1;
        Dist.dTopSizeDisp=(*pInt)[(*pInt).GetUpperBound()]*10.0;
        break;
        }

    DoTopBottom(Dist.dBottomSize,     CfgPF, iDistNo, "BSize");
    DoTopBottom(Dist.dTopSize,        CfgPF, iDistNo, "TSize");
    DoTopBottom(Dist.dBottomSizeDisp, CfgPF, iDistNo, "BSizeDisp");
    DoTopBottom(Dist.dTopSizeDisp,    CfgPF, iDistNo, "TSizeDisp");
    Dist.dBottomSizeDisp=Min(Dist.dBottomSize*0.5, Dist.dBottomSizeDisp);
    Dist.dTopSizeDisp=Max(Dist.dTopSize*2.0, Dist.dTopSizeDisp);

    if (pInt)
      {
      CSD_Intervals &Intervals=*pInt;
      Dist.Intervals.sName=Intervals.sName;
      for (int i=0; i<Intervals.GetSize(); i++)
        if ((Intervals[i]>=Dist.dBottomSize) &&
            ((Dist.Intervals.GetSize()==0) || (Intervals[i]<=Dist.dTopSize)))
          Dist.Intervals.Add(Intervals[i]);
      if (Dist.Intervals[Dist.Intervals.GetSize()-1]<Dist.dTopSize)
        Dist.Intervals.Add(Dist.dTopSize); // Oversize Interval
      }

    for (int N=0; ; N++)
      {
      Strng Species;
      if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Sp", N, Species))
        break;

      char * Parts[1024];
      int nParts=ParseParts(Species, Parts, 1024);
      if (nParts==0)
        break;

      Dist.sSzNames.SetSize(N+1);
      for (int iSp=0; iSp<nParts; iSp++)
        {
        Strng S(Parts[iSp]);
        if (S.XStrICmp("$Default$")!=0)
          Dist.sSzNames[N].Add(S);
        else
          Dist.iDefaultDist=N;
        }
      }

    for (int C=0; ; C++)
      {
      Strng Column;
      if (!RDCFG(CfgPF, "SizeDistributions", "Dist", iDistNo, "Col", C, Column))
        break;

      char * Parts[10];
      int nParts=ParseParts(Column, Parts, 10);
      if (nParts!=2)
        break;
      CSD_ColumnOpt C;
      C.sName=Parts[1];
      C.iOpt=SafeAtoI(Parts[0]);
      Dist.ColumnOpts.Add(C);
      }
    }

  return true;
  };*/

// --------------------------------------------------------------------------

flag SfeSD_Defn::Initialise()
  {
  int ViewIsMass=1;
  int ViewIsCount=!ViewIsMass;

  #if UseAllColumns
  ColumnInitInfo CI[]=
    {
    //  Name,      fFileIt, fOn,       fForFlow,fForMass,fForCum,     Slct,                       iDataId,    pCnv,     pFmt, fEditable,fGrfOn,dwSaveFlags,fDone
      { "FP",      true,  ViewIsMass  && true,  true,  true,  false,  SzDSlct_Fp,                 DI_MFp,     &YFCnv,   &YFFmt,   true,  true, 0, false},
      { "Qm",      false, ViewIsMass  && false, true,  false, false,  SzDSlct_Qm,                 DI_Qm,      &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      { "M",       false, ViewIsMass  && false, false, true,  false,  SzDSlct_M,                  DI_M,       &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},
      { "FPCum",   false, ViewIsMass  && true,  true,  true,  true,   SzDSlct_Fp |SzDSlct_Cum,    DI_MFpCum,  &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QmCum",   false, ViewIsMass  && true,  true,  false, true,   SzDSlct_Qm |SzDSlct_Cum,    DI_QmCum,   &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      { "MCum",    false, ViewIsMass  && true,  false, true,  true,   SzDSlct_M  |SzDSlct_Cum,    DI_MCum,    &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},
      { "FPCumG",  false, ViewIsMass  && false, true,  true,  true,   SzDSlct_Fp |SzDSlct_CumG,   DI_MFpCumG, &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QmCumG",  false, ViewIsMass  && false, true,  false, true,   SzDSlct_Qm |SzDSlct_CumG,   DI_QmCumG,  &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      { "MCumG",   false, ViewIsMass  && false, false, true,  true,   SzDSlct_M  |SzDSlct_CumG,   DI_MCumG,   &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},

      { "SpN",     false, ViewIsCount && true,  true,  true,  false,  SzDSlct_NpM,                DI_NpM,     &YNpMCnv, &YNpMFmt, true,  true, noFile|noSnap, false},
      { "SpNCum",  false, ViewIsCount && true,  true,  true,  true,   SzDSlct_NpM |SzDSlct_Cum,   DI_NpMCum,  &YNpMCnv, &YNpMFmt, false, true, noFile|noSnap, false},
      { "SpNCumG", false, ViewIsCount && false, true,  true,  true,   SzDSlct_NpM |SzDSlct_CumG,  DI_NpMCumG, &YNpMCnv, &YNpMFmt, false, true, noFile|noSnap, false},

      { "NFp",     false, ViewIsCount && false, true,  true,  false,  SzDSlct_NFp,                DI_NFp,     &YNFCnv,  &YNFFmt,  true,  true, noFile|noSnap, false},
      { "Qn",      false, ViewIsCount && false, true,  false, false,  SzDSlct_Qn,                 DI_Qn,      &YQnCnv,  &YQnFmt,  false/*true*/,  true, noFile|noSnap, false},
      { "N",       false, ViewIsCount && false, false, true,  false,  SzDSlct_N,                  DI_N,       &YNCnv,   &YNFmt,   false/*true*/,  true, noFile|noSnap, false},
      { "NFpCum",  false, ViewIsCount && false, true,  true,  true,   SzDSlct_NFp |SzDSlct_Cum,   DI_NFpCum,  &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QnCum",   false, ViewIsCount && false, true,  false, true,   SzDSlct_Qn  |SzDSlct_Cum,   DI_QnCum,   &YQnCnv,  &YQnFmt,  false, true, noFile|noSnap, false},
      { "NCum",    false, ViewIsCount && false, false, true,  true,   SzDSlct_N   |SzDSlct_Cum,   DI_NCum,    &YNCnv,   &YNFmt,   false, true, noFile|noSnap, false},
      { "NFpCumG", false, ViewIsCount && false, true,  true,  true,   SzDSlct_NFp |SzDSlct_CumG,  DI_NFpCumG, &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QnCumG",  false, ViewIsCount && false, true,  false, true,   SzDSlct_Qn  |SzDSlct_CumG,  DI_QnCumG,  &YQnCnv,  &YQnFmt,  false, true, noFile|noSnap, false},
      { "NCumG",   false, ViewIsCount && false, false, true,  true,   SzDSlct_N   |SzDSlct_CumG,  DI_NCumG,   &YNCnv,   &YNFmt,   false, true, noFile|noSnap, false},
    };
  #else
  #if UseExtraColumns
  ColumnInitInfo CI[]=
    {
    //  Name,      fFileIt, fOn,       fForFlow,fForMass,fForCum,     Slct,                       iDataId,    pCnv,     pFmt, fEditable,fGrfOn,dwSaveFlags,fDone
      { "FP",      true,  ViewIsMass  && true,  true,  true,  false,  SzDSlct_Fp,                 DI_MFp,     &YFCnv,   &YFFmt,   true,  true, 0, false},
      { "Qm",      false, ViewIsMass  && false, true,  false, false,  SzDSlct_Qm,                 DI_Qm,      &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      { "M",       false, ViewIsMass  && false, false, true,  false,  SzDSlct_M,                  DI_M,       &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},
      { "FPCum",   false, ViewIsMass  && true,  true,  true,  true,   SzDSlct_Fp |SzDSlct_Cum,    DI_MFpCum,  &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QmCum",   false, ViewIsMass  && true,  true,  false, true,   SzDSlct_Qm |SzDSlct_Cum,    DI_QmCum,   &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      { "MCum",    false, ViewIsMass  && true,  false, true,  true,   SzDSlct_M  |SzDSlct_Cum,    DI_MCum,    &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},

      { "SpN",     false, ViewIsCount && true,  true,  true,  false,  SzDSlct_NpM,                DI_NpM,     &YNpMCnv, &YNpMFmt, true,  true, noFile|noSnap, false},
      { "SpNCum",  false, ViewIsCount && true,  true,  true,  true,   SzDSlct_NpM |SzDSlct_Cum,   DI_NpMCum,  &YNpMCnv, &YNpMFmt, false, true, noFile|noSnap, false},

      { "NFp",     false, ViewIsCount && false, true,  true,  false,  SzDSlct_NFp,                DI_NFp,     &YNFCnv,  &YNFFmt,  true,  true, noFile|noSnap, false},
      { "Qn",      false, ViewIsCount && false, true,  false, false,  SzDSlct_Qn,                 DI_Qn,      &YQnCnv,  &YQnFmt,  false/*true*/,  true, noFile|noSnap, false},
      { "N",       false, ViewIsCount && false, false, true,  false,  SzDSlct_N,                  DI_N,       &YNCnv,   &YNFmt,   false/*true*/,  true, noFile|noSnap, false},
      { "NFpCum",  false, ViewIsCount && false, true,  true,  true,   SzDSlct_NFp |SzDSlct_Cum,   DI_NFpCum,  &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QnCum",   false, ViewIsCount && false, true,  false, true,   SzDSlct_Qn  |SzDSlct_Cum,   DI_QnCum,   &YQnCnv,  &YQnFmt,  false, true, noFile|noSnap, false},
      { "NCum",    false, ViewIsCount && false, false, true,  true,   SzDSlct_N   |SzDSlct_Cum,   DI_NCum,    &YNCnv,   &YNFmt,   false, true, noFile|noSnap, false},
    };
  #else
  ColumnInitInfo CI[]=
    {
    //  Name,      fFileIt, fOn,       fForFlow,fForMass,fForCum,     Slct,                       iDataId,    pCnv,     pFmt, fEditable,fGrfOn,dwSaveFlags,fDone
      { "FP",      true,  ViewIsMass  && true,  true,  true,  false,  SzDSlct_Fp,                 DI_MFp,     &YFCnv,   &YFFmt,   true,  true, 0, false},
      { "Qm",      false, ViewIsMass  && false, true,  false, false,  SzDSlct_Qm,                 DI_Qm,      &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      { "M",       false, ViewIsMass  && false, false, true,  false,  SzDSlct_M,                  DI_M,       &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},
      { "FPCum",   false, ViewIsMass  && true,  true,  true,  true,   SzDSlct_Fp |SzDSlct_Cum,    DI_MFpCum,  &YNFCnv,  &YNFFmt,  false, true, noFile|noSnap, false},
      { "QmCum",   false, ViewIsMass  && true,  true,  false, true,   SzDSlct_Qm |SzDSlct_Cum,    DI_QmCum,   &YQmCnv,  &YQmFmt,  false, true, noFile|noSnap, false},
      //{ "MCum",    false, ViewIsMass  && true,  false, true,  true,   SzDSlct_M  |SzDSlct_Cum,    DI_MCum,    &YMCnv,   &YMFmt,   false, true, noFile|noSnap, false},
    };
  #endif
  #endif

  flag OK=true;
  CIArray SolidRefd;
  SolidRefd.SetSize(SDB.Count());

  if (NDistributions()==0)
    {
    LogWarning("SizeDist", 0, "No Distributions defined - Adding default");
    // Init A Tyler Series
    Intervals.Add(new CSD_Intervals);
    CSD_Intervals &Series=*(Intervals[Intervals.GetUpperBound()]);
    Series.m_sName="Tyler";
    //Series.m_Q=0;
    Series.m_Data=0.0;
    Series.m_SeriesDataType=e_Manual;
    Series.SetSize(0);

    double LoLim=30.0e-6; // um
    double HiLim=10.0e-3; // mm

    int n=sizeof(TylerSeriesInit)/sizeof(TylerSeriesInit[0]);
    for (int i=0; i<n; i++)
      {
      double I=TylerSeriesInit[i]*1.0e-6;
      if (I>=LoLim && I<HiLim)
        Series.Add(I);
      }

    // Init A Distribution;
    Distributions.Add(new CSD_DistDefn);
    CSD_DistDefn &Dist=*(Distributions[Distributions.GetUpperBound()]);
    Dist.sName="Distribution";
    CSD_Intervals *pInt=NULL;   // Pointer to Actual Intervals;
    pInt=Intervals[0];
    Dist.dBottomSize=(*pInt)[0];
    Dist.dTopSize=(*pInt)[(*pInt).GetUpperBound()];
    Dist.dBottomSizeDisp=(*pInt)[0]*0.1;
    Dist.dTopSizeDisp=(*pInt)[(*pInt).GetUpperBound()]*10.0;

    if (pInt)
      {
      CSD_Intervals &Intervals=*pInt;
      Dist.Intervals.m_sName=Intervals.m_sName;
      //Dist.Intervals.m_Q=Intervals.m_Q;
      Dist.Intervals.m_Data=Intervals.m_Data;
      Dist.Intervals.m_SeriesDataType=Intervals.m_SeriesDataType;
      for (int i=0; i<Intervals.GetSize(); i++)
        if ((Intervals[i]>=Dist.dBottomSize) &&
            ((Dist.Intervals.GetSize()==0) || (Intervals[i]<=Dist.dTopSize)))
          {
          Dist.Intervals.Add(Intervals[i]);
          }
      double OvrInt=Dist.dTopSizeDisp;
      Dist.Intervals.Add(OvrInt);
      }
    int N=0;
    Strng S;//="SolidSpecie1";
    for (i=0; i<SDB.Count(); i++)
      if (SDB[i].IsSol())
        {
        // First Solid;
        S=SDB[i].SymOrTag();
        break;
        }
    if (i<SDB.Count())
      {
      Dist.sSzNames.SetSize(N+1);
      Dist.sSzNames[N].Add(S);
      }
    else
      {
      LogError("SzDist", 0, "No solid Species");
//      OK=false;
      }
    }

  if (OK)
    {
    for (int d=0; d<NDistributions(); d++)
      {
      for (int i=0; i<SDB.Count(); i++)
        SolidRefd[i]=false;
      CSD_DistDefn &D=*(Distributions[d]);
      const int ncnt = D.sSzNames.GetSize();
      D.iSzIds.SetSize(ncnt);
      for (int k=0; k<ncnt; k++)
        {
        int iDefault=-1;
        for (int l=0; l<D.sSzNames[k].GetSize(); l++)
          {
          if (D.sSzNames[k][l].XStrChr('(')==NULL)
            D.sSzNames[k][l]+="(s)";
          int iId=SDB.Find(D.sSzNames[k][l].Str());
          if (iId<0)
            {
            LogError("SzDist", 0, "Specie %s not found (Distribution %s)", D.sSzNames[k][l].Str(), D.Name());
            OK=false;
            }
          else if (!SDB[iId].IsSol())
            {
            LogError("SzDist", 0, "Specie %s not Solid (Distribution %s)", D.sSzNames[k][l].Str(), D.Name());
            OK=false;
            }
          else
            {
            SolidRefd[iId]=true;
            D.iSzIds[k].Add(iId);
            }
          }
        if (D.iSzIds[k].GetSize()==0)
          {
          //LogError("SzDist", 0, "No species for distribution %s found", D.Name());
          OK=false;
          }
        }
      if (D.iDefaultDist>=0)
        {
        for (int i=0; i<SDB.Count(); i++)
          if (SDB[i].IsSol() && !SolidRefd[i])
            {
            Strng S(SDB[i].SymOrTag());
            D.sSzNames[D.iDefaultDist].Add(S);
            D.iSzIds[D.iDefaultDist].Add(i);
            SolidRefd[i]=true;
            }
        }



      // Sort the Size Distribution Species indexes into the same order as the Species Display Order
      if (OK)
        {
        for (int k=0; k<ncnt; k++)
          {
          for (int l=k+1; l<ncnt; l++)
            {
            if ( D.iSzIds[k][0] > D.iSzIds[l][0] )
              {
              CIArray lSzIds;
              CSArray lSzNames;
              lSzIds = D.iSzIds[l];
              lSzNames = D.sSzNames[l];

              D.iSzIds[l] = D.iSzIds[k];
              D.sSzNames[l] = D.sSzNames[k];

              D.iSzIds[k] = lSzIds;
              D.sSzNames[k] = lSzNames;
              }
            }
          }
        }

      D.GrdIds.SetSize(D.GrdNames.GetSize());
      for (k=0; k<D.GrdNames.GetSize(); k++)
        {
        if (D.GrdNames[k].XStrChr('(')==NULL)
          D.GrdNames[k]+="(s)";
        D.GrdIds[k]=SDB.Find(D.GrdNames[k].Str());
        if (D.GrdIds[k]<0)
          {
          LogError("SzDist", 0, "Specie %s not found", D.GrdNames[k].Str());
          OK=false;
          }
        else if (!SDB[D.GrdIds[k]].IsSol())
          {
          LogError("SzDist", 0, "Specie %s not Solid", D.GrdNames[k].Str());
          OK=false;
          }
        }

      Strng S;
      int nCols=0;
      int DoneColumnsWithAll=0;
      const bool MultiOres = (D.sSzNames.GetSize()>1);
      const int nCI = sizeof(CI)/sizeof(CI[0]);
      if (D.ColumnOpts.GetSize()==0)
        {
        for (int ci=0; ci<nCI; ci++)
          {
          if (MultiOres)
            {
            CSD_Column C(CI[ci], "All", xidSzCol+(nCols++), -1, false);
            C.dwSaveFlags &= (~isParm);
            D.Columns.Add(C);
            }

          for (int s=0; s<D.sSzNames.GetSize(); s++)
            {
            CSD_Column C(CI[ci], D.sSzNames[s][0](), xidSzCol+(nCols++), s, D.sSzNames.GetSize()==1);
            D.Columns.Add(C);
            if (CI[ci].fFileIt)
              D.FileColIndices.Add(D.Columns.GetUpperBound());
            }
          }
        DoneColumnsWithAll=D.Columns.GetSize();
        }
      else
        {
        for (int co=0; co<D.ColumnOpts.GetSize(); co++)
          if (D.ColumnOpts[co].iOpt>0)
            {
            for (int ci=0; ci<nCI; ci++)
              if (D.ColumnOpts[co].sName.XStrICmp(CI[ci].Name)==0)
                {
                CI[ci].fDone=true;
                CI[ci].fOn=D.ColumnOpts[co].iOpt>1;
                if (MultiOres)
                  {
                  CSD_Column C(CI[ci], "All", xidSzCol+(nCols++), -1, false);
                  C.dwSaveFlags &= (~isParm);
                  D.Columns.Add(C);
                  }

                for (int s=0; s<D.sSzNames.GetSize(); s++)
                  {
                  CSD_Column C(CI[ci], D.sSzNames[s][0](), xidSzCol+(nCols++), s, D.sSzNames.GetSize()==1);
                  D.Columns.Add(C);
                  if (CI[ci].fFileIt)
                    D.FileColIndices.Add(D.Columns.GetUpperBound());
                  }
                break;
                }
            if (ci==nCI)
              LogError("SzDist", 0, "Column %s unknown", D.ColumnOpts[co].sName());
            }

        DoneColumnsWithAll=D.Columns.GetSize();
        for (int ci=0; ci<nCI; ci++)
          if (CI[ci].fFileIt && !CI[ci].fDone)
            {
            CI[ci].fOn=false;
            for (int s=0; s<D.sSzNames.GetSize(); s++)
              {
              CSD_Column C(CI[ci], D.sSzNames[s][0](), xidSzCol+(nCols++), s, D.sSzNames.GetSize()==1);
              D.Columns.Add(C);
              if (CI[ci].fFileIt)
                D.FileColIndices.Add(D.Columns.GetUpperBound());
              }
            }
        }
      ASSERT(nCols==D.Columns.GetSize());
      if (D.Columns.GetSize()>=MaxColumns)
        {
        //LogError("SzDist", 0, "Too many columns, try reduce options in PlantModel.Globals!");
        LogError("SzDist", /*LF_Exclamation*/0, "Require %i columns for PSD: Max allowed is %i!", D.Columns.GetSize(), MaxColumns);
        D.Columns.SetSize(MaxColumns);
        }

      if (1)
        {
        //sort columns into required display order...
        CIArray Ord;
        Ord.SetSize(SDB.Count());
        int Cnt=0;
        SpecieIter I(som_ALL);
        for (int SpId=-1; I.Loop(SpId); )
          {
          int c = D.FindPriSzIdIndex(SpId);
          if (c>=0)
            Ord[Cnt++] = c;
          }
        //ASSERT_RDB(Cnt==D.sSzNames.GetSize(), "Should be ==");
        // Cnt is not equal to D.sSzNames.GetSize() when species are in distribution list but NOT in project!

        const int cmax = D.Columns.GetSize();
        D.DispColIndices.SetSize(cmax);
        int c=0;
        while (c<cmax)
          {
          if (MultiOres && c<DoneColumnsWithAll)
            {
            D.DispColIndices[c]=c;
            c++;
            }
          const int base = c;
          for (int s=0; (s<D.sSzNames.GetSize()) && (c<cmax); s++)
            D.DispColIndices[c++]=base+Ord[s];
          }
        }
      }
    }

  #if dbgSzDist
  if (dbgSzDistInit())
    {
    dbgpln("----------Size Distributions");
    for (int i=0; i<Intervals.GetSize(); i++)
      {
      CSD_Intervals &I=*(Intervals[i]);
      dbgp("Series : %s [",I.Name());
      for (int k=0; k<I.GetSize(); k++)
        dbgp(" %8.4f",ToMM(I[k]));
      dbgpln("]");
      }
    for (int j=0; j<NDistributions(); j++)
      {
      CSD_DistDefn &D=*(Distributions[j]);
      CSD_Intervals &I=D.Intervals;
      dbgpln("Distribution : %s",D.Name());
      dbgp("  Series : %s [",I.Name());
      for (int k=0; k<I.GetSize(); k++)
        dbgp(" %8.4f",ToMM(I[k]));
      dbgpln("]");
      dbgpln("  Species : ");
      for (k=0; k<D.sSzNames.GetSize(); k++)
        {
        dbgp("  : ");
        for (int l=0; l<D.sSzNames[k].GetSize(); l++)
          dbgp(" %s[%i]",D.sSzNames[k][l].Str(),D.iSzIds[k][l]);
        dbgpln("");
        }
      dbgp("  Grade Species : ");
      for (k=0; k<D.GrdNames.GetSize(); k++)
        dbgp(" %s[%i]",D.GrdNames[k].Str(),D.GrdIds[k]);
      dbgpln("");
      }

    }
  #endif

  SQSzDist1Class.SetSelectable(SQSzDist1Class.Selectable() && OK);

  return OK;
  }

// --------------------------------------------------------------------------

flag SfeSD_Defn::Terminate()
  {
  for (int i=0; i<NIntervals(); i++)
    delete Intervals[i];
  Intervals.SetSize(0);
  for (int d=0; d<NDistributions(); d++)
    delete Distributions[d];
  Distributions.SetSize(0);
  return true;
  };

// --------------------------------------------------------------------------

void SfeSD_Defn::ParseSeries(Strng & CurTkn, CTokenFile &Tkns)
  {
  if (CurTkn.XStrICmp(":")==0)
    CurTkn=Tkns.NextToken();
  Intervals.Add(new CSD_Intervals);
  CSD_Intervals &Series=*(Intervals[Intervals.GetUpperBound()]);
  Series.m_sName=CurTkn;
  //Series.m_Q=0;
  Series.m_Data=0.0;
  Series.m_SeriesDataType=e_Manual;
  Series.SetSize(0);

  NextActualToken(",", CurTkn, Tkns);
  Strng CnvStr=CurTkn;
  CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
  if (!pC)
    {
    }

  NextActualToken(",", CurTkn, Tkns);

  int n=0;
  while (!Tkns.AtEOF() && (Tkns.GetCurTokenType()==TokNumeric))
    {
    //if (n>=Series.GetSize())
    //  Series.SetSize(n+16);
    double I=SafeAtoF(CurTkn());
    if (pC)
      I=pC->Normal(I, CnvStr());
    Series.Add(I);
    //Series[n++]=I;
    //CurTkn=Tkns.NextToken();
    if (!ListContinues(CurTkn, Tkns))
      {
      //Tkns.SetReturnSameToken(true);
      break;
      }
    }

//  double TopSize=1e10;
//  Series.Add(TopSize);
  }

// --------------------------------------------------------------------------

void SfeSD_Defn::ParseTyler(Strng & CurTkn, CTokenFile &Tkns)
  {
  if (CurTkn.XStrICmp(":")==0)
    CurTkn=Tkns.NextToken();


  Intervals.Add(new CSD_Intervals);
  CSD_Intervals &Series=*(Intervals[Intervals.GetUpperBound()]);
  Series.m_sName=CurTkn;
  //Series.m_Q=0;
  Series.m_Data=0.0;
  Series.m_SeriesDataType=e_Manual;
  Series.SetSize(0);

  NextActualToken(",", CurTkn, Tkns);
  Strng CnvStr=CurTkn;
  CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
  if (!pC)
    {
    }

  NextActualToken(",", CurTkn, Tkns);

  double LoLim=0.0;
  double HiLim=1.0e6;
  int i=0;
  while (i<2 && !Tkns.AtEOF() && (Tkns.GetCurTokenType()==TokNumeric))
    {
    double D=SafeAtoF(CurTkn());
    if (pC)
      D=pC->Normal(D, CnvStr());
    if (i==0)
      LoLim=D;
    else if (i==1)
      HiLim=D;
    if (!ListContinues(CurTkn, Tkns))
      break;
    i++;
    }

  int n=sizeof(TylerSeriesInit)/sizeof(TylerSeriesInit[0]);
  for (i=0; i<n; i++)
    {
    double I=TylerSeriesInit[i]*1.0e-6;
    if (I>=LoLim && I<HiLim)
      Series.Add(I);
    }

//  double TopSize=1e10;
//  Series.Add(TopSize);
  }

// --------------------------------------------------------------------------

void SfeSD_Defn::ParseDistribution(Strng & CurTkn, CTokenFile &Tkns)
  {
  if (CurTkn.XStrICmp(":")==0)
    CurTkn=Tkns.NextToken();
  Distributions.Add(new CSD_DistDefn);
  CSD_DistDefn &Dist=*(Distributions[Distributions.GetUpperBound()]);
  Dist.sName=CurTkn;
  //NextActualToken(",", CurTkn, Tkns);

  //int n=0;
  while (!Tkns.AtEOF())
    {
    Strng What=NextActualToken(NULL, CurTkn, Tkns);
    if (What.XStrICmp("Series")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      CSD_Intervals *pInt=NULL;   // Pointer to Actual Intervals;
      for (int i=0; i<Intervals.GetSize(); i++)
        if (CurTkn.XStrICmp(Intervals[i]->m_sName)==0)
          {
          pInt=Intervals[i];
          Dist.dBottomSize=(*pInt)[0];
          Dist.dTopSize=(*pInt)[(*pInt).GetUpperBound()];
          Dist.dBottomSizeDisp=(*pInt)[0]*0.1;
          Dist.dTopSizeDisp=(*pInt)[(*pInt).GetUpperBound()]*10.0;
          break;
          }
      while (ListContinues(CurTkn, Tkns))
        {
        if (CurTkn.XStrICmp("BottomSize")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          Dist.dBottomSize=SafeAtoF(CurTkn());
          CurTkn=Tkns.NextToken();
          Strng CnvStr=CurTkn;
          CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
          if (pC)
            Dist.dBottomSize=pC->Normal(Dist.dBottomSize, CnvStr());
          else
            Tkns.SetReturnSameToken(true);
          Dist.dBottomSizeDisp=Max(0.0, Min(Dist.dBottomSize*0.5, Dist.dBottomSizeDisp));
          }
        else if (CurTkn.XStrICmp("TopSize")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          Dist.dTopSize=SafeAtoF(CurTkn());
          CurTkn=Tkns.NextToken();
          Strng CnvStr=CurTkn;
          CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
          if (pC)
            Dist.dTopSize=pC->Normal(Dist.dTopSize, CnvStr());
          else
            Tkns.SetReturnSameToken(true);
          Dist.dTopSizeDisp=Max(Dist.dTopSize*2.0, Dist.dTopSizeDisp);
          }
        else if (CurTkn.XStrICmp("BottomSizeDisp")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          Dist.dBottomSizeDisp=SafeAtoF(CurTkn());
          CurTkn=Tkns.NextToken();
          Strng CnvStr=CurTkn;
          CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
          if (pC)
            Dist.dBottomSizeDisp=pC->Normal(Dist.dBottomSizeDisp, CnvStr());
          else
            Tkns.SetReturnSameToken(true);
          Dist.dBottomSizeDisp=Max(0.0, Min(Dist.dBottomSize*0.5, Dist.dBottomSizeDisp));
          }
        else if (CurTkn.XStrICmp("TopSizeDisp")==0)
          {
          NextActualToken("=", CurTkn, Tkns);
          Dist.dTopSizeDisp=SafeAtoF(CurTkn());
          CurTkn=Tkns.NextToken();
          Strng CnvStr=CurTkn;
          CDataCnv *pC=gs_CnvsMngr.FindSecCnv(DC_L, CnvStr());
          if (pC)
            Dist.dTopSizeDisp=pC->Normal(Dist.dTopSizeDisp, CnvStr());
          else
            Tkns.SetReturnSameToken(true);
          Dist.dTopSizeDisp=Max(Dist.dTopSize*2.0, Dist.dTopSizeDisp);
          }
        }
      Tkns.SetReturnSameToken(true);

      //NextActualToken(",", CurTkn, Tkns);
      //if (CurTkn.XStrICmp("Bottom")==0)
      //  {
      //  NextActualToken("=", CurTkn, Tkns);
      //  Dist.dBottomSize=SafeAtoF(CurTkn());
      //  }
      //NextActualToken(",", CurTkn, Tkns);
      //if (CurTkn.XStrICmp("Top")==0)
      //  {
      //  NextActualToken("=", CurTkn, Tkns);
      //  Dist.dTopSize=SafeAtoF(CurTkn());
      //  }

      if (pInt)
        {
        CSD_Intervals &Intervals=*pInt;
        Dist.Intervals.m_sName=Intervals.m_sName;
        //Dist.Intervals.m_Q=Intervals.m_Q;
        Dist.Intervals.m_Data=Intervals.m_Data;
        Dist.Intervals.m_SeriesDataType=Intervals.m_SeriesDataType;
        //int N=0;
        for (int i=0; i<Intervals.GetSize(); i++)
          if ((Intervals[i]>=Dist.dBottomSize) &&
              ((Dist.Intervals.GetSize()==0) || (Intervals[i]<=Dist.dTopSize)))
            {
            Dist.Intervals.Add(Intervals[i]);
            //Dist.Intervals.SetSize(N+1);
            //Dist.Intervals[N]=Intervals[i];
            //N++;
            }
        //double OvrInt=Dist.dTopSizeDisp;//BigSizeInterval;
        //Dist.Intervals.Add(OvrInt); // Oversize Interval
        if (Dist.Intervals[Dist.Intervals.GetSize()-1]<Dist.dTopSize)
          Dist.Intervals.Add(Dist.dTopSize); // Oversize Interval
        }
      }
    else if (What.XStrICmp("Species")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      int N=0, Done=false;
      Strng S;
      while (!Tkns.AtEOF() && !Done)
        {
        //if (CurTkn.XStrICmp("End")==0)
        //  break;
        Dist.sSzNames.SetSize(N+1);
        S=CurTkn();
        Dist.sSzNames[N].Add(S);
        if (!ListContinues(CurTkn, Tkns))
          {
          if (CurTkn.XStrICmp("{")==0)
            {
            NextActualToken("{", CurTkn, Tkns);
            while (!Tkns.AtEOF())
              {
              if (CurTkn.XStrICmp("Default")!=0)
                {
                S=CurTkn();
                Dist.sSzNames[N].Add(S);
                }
              else
                Dist.iDefaultDist=N;
              if (!ListContinues(CurTkn, Tkns)) // Skip ,/}
                {
                //NextActualToken("}", CurTkn, Tkns);
                if (!ListContinues(CurTkn, Tkns))
                //if (CurTkn.XStrICmp(",")!=0)
                  Done=true;
                break;
                }
              }
            }
          else
            Done=true;
          }
        N++;
        }
      Tkns.SetReturnSameToken(true);
      }
    else if (What.XStrICmp("Grade")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      int N=0;
      while (!Tkns.AtEOF())
        {
        //if (CurTkn.XStrICmp("End")==0)
        //  break;
        Dist.GrdNames.SetSize(N+1);
        Dist.GrdNames[N]=CurTkn();
        N++;
        if (!ListContinues(CurTkn, Tkns))
          {
          Tkns.SetReturnSameToken(true);
          break;
          }
        }
      }
    else if (What.XStrICmp("BondWI")==0)
      {
      NextActualToken("=", CurTkn, Tkns);
      //int N=0;
      while (!Tkns.AtEOF())
        {
        //if (CurTkn.XStrICmp("End")==0)
        //  break;
        //Dist.SzNames.SetSize(N+1);
        //Dist.SzNames[N]=CurTkn();
        //N++;
        if (!ListContinues(CurTkn, Tkns))
          {
          Tkns.SetReturnSameToken(true);
          break;
          }
        }
      }
    else if (What.XStrICmp("Columns")==0)
      {
      CurTkn=Tkns.NextToken();
      int SpNo=0;
      while (!Tkns.AtEOF())
        {
        if (CurTkn.XStrICmp("End")==0)
          {
          CurTkn=Tkns.NextToken();
          break;
          }
        else if (CurTkn.XStrICmp("Columns")==0)
          {
          CurTkn=Tkns.NextToken();
          if (CurTkn.XStrICmp(":")==0)
            CurTkn=Tkns.NextToken(); // SkipEnd
          //break;
          }
        else
          {
          CSD_ColumnOpt C;
          C.sName=CurTkn();
          NextActualToken("=", CurTkn, Tkns);
          C.iOpt=SafeAtoI(CurTkn());
          Dist.ColumnOpts.Add(C);
          CurTkn=Tkns.NextToken();
          }
        if (CurTkn.XStrICmp(",")==0)
          CurTkn=Tkns.NextToken();
        }
      }
    else
      break;
    //if (CurTkn.XStrICmp(",")==0)
    //  CurTkn=Tkns.NextToken();
    }
  //Series.SetSize(n);
  }

// --------------------------------------------------------------------------

CSD_DistDefn * SfeSD_Defn::FindDistributionFor(int SpecieId)
  {
  for (int i=0; i<NDistributions(); i++)
    {
    CSD_DistDefn * D=Distributions[i];
    for (int j=0; j<D->NPriIds(); j++)
      if (D->iSzIds[j][0]==SpecieId)
        return D;
    }

  return NULL;
  }

// --------------------------------------------------------------------------

bool SfeSD_Defn::GetInfo()
  {//return true if valid data item returned
  if (bInitRequest)
    {
    iGroupIndex = 0;
    iItemIndex = 0;
    bInitRequest = 0;
    bStringData = 0;
    Headings.SetSize(NDistributions());
    for (int i=0; i<NDistributions(); i++)
      {
      if (DistExists(i))
        Headings[i] = Distributions[i]->Name();
      else
        Headings[i] = "";
      }
    }
  else
    {
    iItemIndex++;
    }

  if (iGroupIndex>=0 && iItemIndex>=0 && iGroupIndex<NDistributions())
    {
    while (iGroupIndex<NDistributions() && !DistExists(iGroupIndex))
      iGroupIndex++;
    if (iGroupIndex<NDistributions())
      {
      CSD_DistDefn& Dist = *(Distributions[iGroupIndex]);
      if (iItemIndex>=Dist.Intervals.GetSize())
        {
        iItemIndex = 0;
        iGroupIndex++;
        while (iGroupIndex<NDistributions() && !DistExists(iGroupIndex))
          iGroupIndex++;
        }
      }
    if (iGroupIndex<NDistributions())
      {
      CSD_DistDefn& Dist = *(Distributions[iGroupIndex]);
      if (iItemIndex<Dist.Intervals.GetSize())
        {
        dItem = Dist.Intervals[iItemIndex];
        return true;
        }
      }
    }
  return false;
  }

// ==========================================================================
//
//
//
// ==========================================================================

SfeSD_Defn SD_Defn;

// ==========================================================================
//
//
//
// ==========================================================================
//                        Specie Quality - Size Distribution

// --------------------------------------------------------------------------

static long CSD_SpDistCnt=0;
CSD_SpDist::CSD_SpDist()
  {
  //dbgpln("CSD_SpDist() %i", ++CSD_SpDistCnt);
  };
CSD_SpDist::CSD_SpDist(const CSD_SpDist & Other)
  {
  //dbgpln("CSD_SpDist(const CSD_SpDist & Other) %i", ++CSD_SpDistCnt);
  //iSpId=Other.iSpId;
  FracPass=Other.FracPass;
  //BondWI=Other.BondWI;
  //Grade=Other.Grade;
  };
CSD_SpDist & CSD_SpDist::operator=(const CSD_SpDist & Other)
  {
  FracPass=Other.FracPass;
  //BondWI=Other.BondWI;
  //Grade=Other.Grade;
  return *this;
  };
CSD_SpDist::~CSD_SpDist()
  {
  //dbgpln("~CSD_SpDist() %i", CSD_SpDistCnt--);
  };

IMPLEMENT_SPARES(CSD_SpDist, 1000)
IMPLEMENT_SPARES(CSD_Distribution, 1000)

// --------------------------------------------------------------------------

static long CSD_DistributionCnt=0;

CSD_Distribution::CSD_Distribution(CSD_DistDefn* pDefn) :
  rDefn(*pDefn), rIntervals(pDefn->Intervals)
  {
  //dbgpln("CSD_Distribution(CSD_DistDefn* pDefn) %i", ++CSD_DistributionCnt);
  iCurrentDataId=-1;
  iCurrentSpId=-1;

  PriSp.SetSize(NPriIds());
  for (int s=0; s<NPriIds(); s++)
    {
    PriSp[s]->FracPass.SetSize(NIntervals());
    PriSp[s]->WorkFrac.SetSize(NIntervals());
    //PriSp[s]->BondWI.SetSize(NIntervals());
    //PriSp[s]->Grade.SetSizes(NGrades(), NIntervals());

    //PriSp[s]->FracPass[0]=1.0;// All Material is Fine;
    PriSp[s]->FracPass[NIntervals()-1]=1.0;// All Material is Coarse;
    }
  }


CSD_Distribution::CSD_Distribution(const CSD_Distribution & Other) :
  rDefn(Other.rDefn), rIntervals(Other.rDefn.Intervals)
  {
  //dbgpln("CSD_Distribution(const CSD_Distribution & Other) %i", ++CSD_DistributionCnt);
  iCurrentDataId=-1;
  iCurrentSpId=-1;

  PriSp.SetSize(NPriIds());
  for (int s=0; s<NPriIds(); s++)
    {
    PriSp[s]->FracPass=Other.PriSp[s]->FracPass;
    PriSp[s]->WorkFrac=Other.PriSp[s]->WorkFrac;
    //PriSp[s]->BondWI=Other.PriSp[s]->BondWI;
    //PriSp[s]->Grade=Other.PriSp[s]->Grade;

    //PriSp[s]->FracPass[0]=1.0;// All Material is Fine;
    PriSp[s]->FracPass[NIntervals()-1]=1.0;// All Material is Coarse;
    }
  }

// --------------------------------------------------------------------------

CSD_Distribution::~CSD_Distribution()
  {
  //dbgpln("~CSD_Distribution() %i", CSD_DistributionCnt--);
  };

// --------------------------------------------------------------------------

CSD_Distribution& CSD_Distribution::operator=(const CSD_Distribution & Other)
  {
  for (int s=0; s<NPriIds(); s++)
    {
    *PriSp[s]=*Other.PriSp[s];
    }
  return *this;
  }

// --------------------------------------------------------------------------

flag CSD_Distribution::ValidateData(ValidateDataBlk & VDB)
  {
  //flag OK=true;
  return true;
  }

// --------------------------------------------------------------------------

void CSD_Distribution::ZeroMass()
  {
  for (int s=0; s<NPriIds(); s++)
    {
    CSD_SpDist & S=*PriSp[s];
    S.FracPass.OpMult(0.0);
    }
  }

// --------------------------------------------------------------------------

void CSD_Distribution::AddMassF(CSysVector &M1, double M1Mult, CSD_Distribution *pD2, CSysVector &M2, double M2Mult)
  {
  if (pD2)
    {
    //AllocDist(d);
    CSD_Distribution & D2=*pD2;
    for (int s=0; s<NPriIds(); s++)
      {
      CSD_SpDist & S=*PriSp[s];
      CSD_SpDist & S2=*D2.PriSp[s];
      double m1=0.0;
      double m2=0.0;
      for (int l=0; l<NSecIds(s); l++)
        {
        m1+=M1[SzId(s,l)]*M1Mult;
        m2+=M2[SzId(s,l)]*M2Mult;
        }
      double f1=m1/GTZ(m1+m2);
      if (f1 >= 1.0e-8)
        {
        double f2=m2/GTZ(m1+m2);
        //dBondWorkIndex=f1*dBondWorkIndex+f2*pQ2->dBondWorkIndex;
        S.FracPass.OpMult(f1);
        S.FracPass.OpAdd(S2.FracPass, f2, 0.0);
        S.FracPass.Normalise();
        }
      else
        {
        S=S2;
        //dBondWorkIndex=pQ2->dBondWorkIndex;
        }
      }
    }
  else //if (Distributions[d])// && !P2.Distributions[d])
    {
    for (int s=0; s<NPriIds(); s++)
      {
      CSD_SpDist & S=*PriSp[s];
      double m1=0.0;
      double m2=0.0;
      for (int l=0; l<NSecIds(s); l++)
        {
        m1+=M1[SzId(s,l)]*M1Mult;
        m2+=M2[SzId(s,l)]*M2Mult;
        }
      double f1=m1/GTZ(m1+m2);
      //dBondWorkIndex=f1*dBondWorkIndex+f2*pQ2->dBondWorkIndex;
      S.FracPass.OpMult(f1);
      }
    }
  }

// --------------------------------------------------------------------------

void CSD_Distribution::ClearWorkSpace()
  {
  pResults=NULL;
  WorkSpace.SetSize(NIntervals());
  for (int i=0; i<NIntervals(); i++)
    WorkSpace[i]=0.0;
  }

// --------------------------------------------------------------------------

void CSD_Distribution::ClearWorkSpace1()
  {
  WorkSpace1.SetSize(NIntervals());
  for (int i=0; i<NIntervals(); i++)
    WorkSpace1[i]=0.0;
  }

// --------------------------------------------------------------------------

void CSD_Distribution::DistTotals(SpPropOveride *Ovr, CSysVector &M1, int iSpPriId, double &Mass, double &Volume)
  {
  Mass=0.0;
  Volume=0.0;
  for (int l=0; l<NSecIds(iSpPriId); l++)
    {
    int Id=SzId(iSpPriId,l);
    Mass+=M1[Id];
    Volume+=M1[Id]*SDB[Id].msVolume(SpModel::Fidelity(), Std_T, Std_P, Ovr, M1.SVData());
    }
  }

// --------------------------------------------------------------------------

void CSD_Distribution::CalcDensity(SpPropOveride *Ovr, CSysVector &M1, int iSpId)
  {
  SpMass.SetSize(NPriIds());
  SpDensity.SetSize(NPriIds());

  int s0=(iSpId>=0) ? iSpId : 0;
  int sN=(iSpId>=0) ? iSpId : NPriIds()-1;
  for (int s=s0; s<=sN; s++)
    {
    SpMass[s]=0.0;
    double Volume=0.0;
    for (int l=0; l<NSecIds(s); l++)
      {
      int Id=SzId(s,l);
      SpMass[s]+=M1[Id];
      Volume+=M1[Id]*SDB[Id].msVolume(SpModel::Fidelity(), Std_T, Std_P, Ovr, M1.SVData());
      }
    SpDensity[s]=SpMass[s]/GTZ(Volume);
    SpDensity[s]=Max(0.01, SpDensity[s]);
    }
  }

// --------------------------------------------------------------------------

double CSD_Distribution::GetMass(SpPropOveride *Ovr, CSysVector &M1, int iSpId)
  {
  ClearWorkSpace();

  int s0=(iSpId>=0) ? iSpId : 0;
  int sN=(iSpId>=0) ? iSpId : NPriIds()-1;
  double TotMass=0.0;
  for (int s=s0; s<=sN; s++)
    {
    double Mass=0.0;
    for (int l=0; l<NSecIds(s); l++)
      Mass+=M1[SzId(s,l)];
    TotMass+=Mass;

    CSD_SpDist & S=*PriSp[s];
    for (int i=0; i<NIntervals(); i++)
      WorkSpace[i]+=S.FracPass[i]*Mass;
    }
  TotMass=GTZ(TotMass);
  SetResults(WorkSpace);
  return TotMass;
  };

// --------------------------------------------------------------------------

double CSD_Distribution::GetMassFrac(SpPropOveride *Ovr, CSysVector &M1, int iSpId)
  {
  ClearWorkSpace();
  WorkSpace1.SetSize(NIntervals());

  int s0=(iSpId>=0) ? iSpId : 0;
  int sN=(iSpId>=0) ? iSpId : NPriIds()-1;
  double TotMass=0.0;
  for (int s=s0; s<=sN; s++)
    {
    double Mass=0.0;
    for (int l=0; l<NSecIds(s); l++)
      Mass+=M1[SzId(s,l)];
    TotMass+=Mass;

    CSD_SpDist & S=*PriSp[s];
    for (int i=0; i<NIntervals(); i++)
      WorkSpace[i]+=S.FracPass[i]*Mass;

    }
  TotMass=GTZ(TotMass);
  for (int i=0; i<NIntervals(); i++)
    WorkSpace1[i]=WorkSpace[i]/TotMass;
  SetResults(WorkSpace1);
  return TotMass;
  };

// --------------------------------------------------------------------------

void CSD_Distribution::GetSpCount(SpPropOveride *Ovr, CSysVector &M1, int iSpId)
  {
  ClearWorkSpace();
  ClearWorkSpace1();

  CalcDensity(Ovr, M1, iSpId);

  double TotMass=0.0;
  int s0=(iSpId>=0) ? iSpId : 0;
  int sN=(iSpId>=0) ? iSpId : NPriIds()-1;
  for (int s=s0; s<=sN; s++)
    {

    CSD_SpDist & S=*PriSp[s];
    for (int i=0; i<NIntervals(); i++)
      {
      double MassInInt=S.FracPass[i]*SpMass[s];
      double CountInInt=PartCount(i, MassInInt, SpDensity[s]);
      TotMass+=MassInInt;
      WorkSpace[i]+=CountInInt;
      }
    }
  TotMass=GTZ(TotMass);

  for (int i=0; i<NIntervals(); i++)
    WorkSpace[i]/=TotMass;
  SetResults(WorkSpace);
  };

// --------------------------------------------------------------------------

void CSD_Distribution::GetCount(SpPropOveride *Ovr, CSysVector &M1, int iSpId)
  {
  ClearWorkSpace();

  CalcDensity(Ovr, M1, iSpId);

  int s0=(iSpId>=0) ? iSpId : 0;
  int sN=(iSpId>=0) ? iSpId : NPriIds()-1;
  for (int s=s0; s<=sN; s++)
    {
    CSD_SpDist & S=*PriSp[s];
    for (int i=0; i<NIntervals(); i++)
      {
      double MassInInt=S.FracPass[i]*SpMass[s];
      double CountInInt=PartCount(i, MassInInt, SpDensity[s]);
      WorkSpace[i]+=CountInInt;
/*double MPM = MeanPartMass(i, SpDensity[s]);
//  { double R=MeanPartDiam(iInt)/2; return 4/3*PI*(R*R*R)*MeanDens; };
//double PartCount(int iInt, double Mass, double MeanDens) { return Mass/GTZ(MeanPartMass(iInt, MeanDens)); };
      double L = rDefn.Intervals[i];
      double ni = MassInInt*6.0/(1.4618*PI*SpDensity[s]*L*L*L);
WorkSpace[i]+=ni;*/
      }
    }
  SetResults(WorkSpace);
  };

// --------------------------------------------------------------------------

void CSD_Distribution::GetCountFrac(SpPropOveride *Ovr, CSysVector &M1, int iSpId)
  {
  GetCount(Ovr, M1, iSpId);
  WorkSpace1.SetSize(NIntervals());

  double TotCount=0.0;
  for (int i=0; i<NIntervals(); i++)
    TotCount+=WorkSpace[i];

  TotCount=GTZ(TotCount);
  for (i=0; i<NIntervals(); i++)
    WorkSpace1[i]=WorkSpace[i]/TotCount;
  SetResults(WorkSpace1);
  };

// --------------------------------------------------------------------------

void CSD_Distribution::SetCountFrac(SpPropOveride *Ovr, CSysVector &M1, int iSpId, int iInt, double Val)
  {
  if (iSpId>=0)
    {
    GetCountFrac(Ovr, M1, iSpId);

    CDArray ReqdFracs(Results());

    // This is the required frac distribution of counts
    ReqdFracs[iInt]=Val;
    ReqdFracs.AdjustCumulativeEnd(1.0, 0.0, 1.0);

    CDArray RelativeMass;
    RelativeMass.SetSize(NIntervals());

    double TMass=0.0;
    for (int i=0; i<NIntervals(); i++)
      {
      RelativeMass[i]=ReqdFracs[i]/GTZ(PartDensity(i, SpDensity[iSpId]));
      TMass+=RelativeMass[i];
      }

    // This are the mass fractions
    for (i=0; i<NIntervals(); i++)
      {
      RelativeMass[i]/=TMass;
      PriSp[iSpId]->FracPass[i]=RelativeMass[i];
      }
    }
  };

// --------------------------------------------------------------------------

void CSD_Distribution::SetSpCount(SpPropOveride *Ovr, CSysVector &M1, int iSpId, int iInt, double Val)
  {
  if (iSpId>=0)
    {
    GetSpCount(Ovr, M1, iSpId);

    CDArray ReqdCount(Results());

    ReqdCount[iInt]=Val;
//    ReqdFracs.AdjustCumulativeEnd(1.0, 0.0, 1.0);

    CDArray RelativeMass;
    RelativeMass.SetSize(NIntervals());

    double TMass=0.0;
    for (int i=0; i<NIntervals(); i++)
      {
      RelativeMass[i]=ReqdCount[i]*MeanPartMass(i, SpDensity[iSpId]);
      RelativeMass[i]=Min(1.0-TMass, RelativeMass[i]);
      TMass+=RelativeMass[i];
      }
    // The Error goes into the top
    RelativeMass[NIntervals()-1]+=(1.0-TMass);

    // These are the mass fractions
    for (i=0; i<NIntervals(); i++)
      {
      RelativeMass[i]/=TMass;
      PriSp[iSpId]->FracPass[i]=RelativeMass[i];
      }
    }
  };

// --------------------------------------------------------------------------

flag CSD_Distribution::CalculateResults(SpPropOveride *Ovr, CSysVector &M1, int DataId, int SpId)
  {
  if (iCurrentDataId!=DataId || iCurrentSpId!=SpId)
    {
    iCurrentDataId=DataId;
    iCurrentSpId=SpId;

    switch (DataId)
      {
      case DI_Qm:
      case DI_M :
        GetMass(Ovr, M1, SpId);
        return 1;
      case DI_QmCum:
      #if UseExtraColumns
      case DI_MCum :
      #endif
        GetMass(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, false, 0);//!fSzAscend);
        return 1;
      case DI_MFp:
        if (SpId>=0)
          SetResults(PriSp[SpId]->FracPass);
        else
          GetMassFrac(Ovr, M1, SpId);
        return 1;
      case DI_MFpCum:
        if (SpId>=0)
          {
          WorkSpace=PriSp[SpId]->FracPass;
          SetResults(WorkSpace);
          }
        else
          GetMassFrac(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, false, 0);//!fSzAscend);
        return 1;
      #if UseAllColumns
      case DI_QmCumG:
      case DI_MCumG :
        GetMass(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, true, 0);//!fSzAscend);
        return 1;
      case DI_MFpCumG:
        if (SpId>=0)
          {
          WorkSpace=PriSp[SpId]->FracPass;
          SetResults(WorkSpace);
          }
        else
          GetMassFrac(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, true, 0);//!fSzAscend);
        return 1;
      case DI_NpMCumG :
        GetSpCount(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, true, 0);//!fSzAscend);
        return 1;
      case DI_QnCumG:
      case DI_NCumG :
        GetCount(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, true, 0);//!fSzAscend);
        return 1;
      case DI_NFpCumG:
          GetCountFrac(Ovr, M1, SpId);
          Results().ToCumulative(0.0, dNAN, true, 0);//!fSzAscend);
        return 1;
      #endif
      #if UseExtraColumns
      case DI_NpM:
        GetSpCount(Ovr, M1, SpId);
        return 1;
      case DI_NpMCum :
        GetSpCount(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, false, 0);//!fSzAscend);
        return 1;
      case DI_Qn:
      case DI_N :
        GetCount(Ovr, M1, SpId);
        return 1;
      case DI_QnCum:
      case DI_NCum :
        GetCount(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, false, 0);//!fSzAscend);
        return 1;
      case DI_NFp:
        GetCountFrac(Ovr, M1, SpId);
        return 1;
      case DI_NFpCum:
        GetCountFrac(Ovr, M1, SpId);
        Results().ToCumulative(0.0, dNAN, false, 0);//!fSzAscend);
        return 1;
      #endif
      default :
        return 0;
      }
    }
  return 1;
  }

// --------------------------------------------------------------------------
// ==========================================================================

double CSD_Distribution::Crush0(C2DFn & PartFn)
  {
  CDArray FracPart;
  FracPart.SetSize(NIntervals());
  double CumPrev=0.0;
  for (int i=0; i<NIntervals()-1; i++)
    {
    double Cum=Range(0.0, PartFn.Yx(rIntervals[i]), 1.0);
    FracPart[i]=Cum-CumPrev;
    CumPrev=Cum;
    }
  FracPart[NIntervals()-1]=1.0-CumPrev;

  for (int s=0; s<NPriIds(); s++)
    {
    CSD_SpDist & S=*PriSp[s];
    for (int i=0;i<NIntervals(); i++)
      S.FracPass[i]=FracPart[i];
    }
  return 0.0;
  }

// --------------------------------------------------------------------------

double CSD_Distribution::Crush0(SzPartCrv1 & PartFn, int CurveNo)
  {
  CDArray FracPart;
  FracPart.SetSize(NIntervals());
  double CumPrev=0.0;
  for (int i=0; i<NIntervals()-1; i++)
    {
    double Cum=Range(0.0, PartFn.FractionPassingSize(CurveNo, rIntervals[i]), 1.0);
    FracPart[i]=Cum-CumPrev;
    CumPrev=Cum;
    }
  FracPart[NIntervals()-1]=1.0-CumPrev;

  for (int s=0; s<NPriIds(); s++)
    {
    CSD_SpDist & S=*PriSp[s];
    for (int i=0;i<NIntervals(); i++)
      S.FracPass[i]=FracPart[i];
    }
  return 0.0;
  }

// --------------------------------------------------------------------------

double CSD_Distribution::Break0(SzSelBrk1 & SB)
  {
  CDArray FPOut;
  FPOut.SetSize(NIntervals());
  double CumPrev=0.0;
  for (int s=0; s<NPriIds(); s++)
    {
    CSD_SpDist & S=*PriSp[s];
    CDArray & FPIn = S.FracPass;
    for (int i=0; i<NIntervals(); i++)
      {
//      double Sel=SB.Selection(i);
      FPOut[i]=(1.0-SB.Selection(i))*FPIn[i];
      for (int j=i+1; j<NIntervals(); j++)
        FPOut[i]+=SB.Selection(j)*FPIn[j]*SB.Breakage(j,i);
      }
    for (i=0;i<NIntervals(); i++)
      S.FracPass[i]=FPOut[i];
    }
  return 0.0;
  }

// --------------------------------------------------------------------------

double CSD_Distribution::CrushExt0(SzPartCrv1 &Extents, CSD_Distribution &FdD, int CurveNo)
  {
  for (int s=0; s<NPriIds(); s++)
    CrushExt1(s, Extents, FdD, CurveNo);
  return 0.0;
  }

// --------------------------------------------------------------------------

double CSD_Distribution::Crush1(int s, C2DFn & PartFn)
  {
  CDArray FracPart;
  FracPart.SetSize(NIntervals());
  double CumPrev=0.0;
  for (int i=0; i<NIntervals()-1; i++)
    {
    double Cum=Range(0.0, PartFn.Yx(rIntervals[i]), 1.0);
    FracPart[i]=Cum-CumPrev;
    CumPrev=Cum;
    }
  FracPart[NIntervals()-1]=1.0-CumPrev;

  //for (int s=0; s<NPriIds(); s++)
    //{
    CSD_SpDist & S=*PriSp[s];
    for (int j=0;j<NIntervals(); j++)
      S.FracPass[j]=FracPart[j];
    //}
  return 0.0;
  }

// --------------------------------------------------------------------------

double CSD_Distribution::Crush1(int s, SzPartCrv1 & PartFn, int CurveNo)
  {
  CDArray FracPart;
  FracPart.SetSize(NIntervals());
  double CumPrev=0.0;
  for (int i=0; i<NIntervals()-1; i++)
    {
    double Cum=Range(0.0, PartFn.FractionPassingSize(CurveNo, rIntervals[i]), 1.0);
    FracPart[i]=Cum-CumPrev;
    CumPrev=Cum;
    }
  FracPart[NIntervals()-1]=1.0-CumPrev;

  //for (int s=0; s<NPriIds(); s++)
    //{
    CSD_SpDist & S=*PriSp[s];
    for (int j=0;j<NIntervals(); j++)
      S.FracPass[j]=FracPart[j];
    //}
  return 0.0;
  }

// --------------------------------------------------------------------------

double CSD_Distribution::CrushExt1(int s, SzPartCrv1 &Extents, CSD_Distribution &FdD, int CurveNo)
  {
  CSD_SpDist & FdS=*(FdD.PriSp[s]);
  CSD_SpDist & S=*PriSp[s];
  CDArray & Ext=Extents.Curve(CurveNo);

  double Ttl=0.0;
  double Prev=0.0;
  for (int i=NIntervals()-1; i>0; i--)
    {
    const double In = FdS.FracPass[i]+Prev;
    S.FracPass[i] = In*Ext[i];
    Prev = In - S.FracPass[i];
    Ttl += S.FracPass[i];
    }
  S.FracPass[0] = 1.0-Ttl;
  return 0.0;
  }

// ==========================================================================
//
//
//
// ==========================================================================

