//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "drv_dlgs.h"
#include "drv_mngr.h"
#include "project.h"
#include "dlgbusy.h"
#include "dbgmngr.h"
#include "..\schist\hstmsg.h"
#include "TagDB.h"
#include "dbhelper.h"
#include "sfe_clnt.h"
#include "scd_wm.h"
#include "noise.h"

//#include "optoff.h"

#define dbgDriverMan     1

#if dbgDriverMan
static CDbgMngr dbgDumpConfig ("DriverManager", "DumpConfig");
static CDbgMngr dbgGetValues  ("DriverManager", "GetValues");
static CDbgMngr dbgPutValues  ("DriverManager", "PutValues");
static CDbgMngr dbgChgValues  ("DriverManager", "ChgValues");
static CDbgMngr dbgSubsQueue  ("DriverManager", "SubsQueue");
#endif

//===========================================================================
//
//
//
//===========================================================================

CDriverManager* gs_pDrvMan = NULL;

//===========================================================================

CConnSubsItem::CConnSubsItem(CDrvSubsConn* pConSlot)
  {
  m_pCon = pConSlot;
  m_iAction = (m_pCon->m_bGet ? XIO_In : XIO_InOut);
  }

// --------------------------------------------------------------------------

void CConnSubsItem::AddConn(CDrvSubsConn* pConSlot)
  {
  //if (!pCon->bGet)
  if (!pConSlot->m_bGet) // CNM
    m_iAction = XIO_InOut; //ensure IO is in AND out
  pConSlot->m_pNxtSubs = m_pCon;
  m_pCon = pConSlot;
  }

//===========================================================================

CDriverArray::CDriverArray(CDriverManager *pMngr)
  {
  Maximum = 0;
  Used    = 0;
  pItem   = NULL;
  m_pMngr = pMngr;
  }

// --------------------------------------------------------------------------

CDriverArray::~CDriverArray()
  {
  Alloc(0);
  }

// --------------------------------------------------------------------------

void CDriverArray::Alloc(int iMax)
  {
  if (iMax==0)
    {
    if (pItem)
      {
      for(int i = 0; i < Used; i++)
        {
        pCDriver pDrv = *(pItem+i);
        HINSTANCE hLib = pDrv->hLib;
        delete pDrv;
        if (hLib)
          if (lDoFreeLibraries)
            AfxFreeLibrary(hLib);
        //delete *(pItem+i);
        *(pItem+i) = NULL;
        }
      free(pItem);
      }
    pItem = NULL;
    Maximum = Used = 0;
    return;
    }
  else if(pItem)
    {
    return;
    }
  Maximum = iMax;
  Used    = 0;
  pItem = (pCDriver *)calloc(sizeof(pCDriver),Maximum);
  if (pItem==NULL)
    Maximum = 0;
  }

// --------------------------------------------------------------------------

pCDriver CDriverArray::Find(const char *Name)
  {
  if (pItem==NULL)
    return NULL;
  for (int i=0; i<Used; i++)
    {
    pCDriver pDrv = *(pItem+i);
    if (strcmpi(pDrv->sTag(), Name)==0)
      return pDrv;
    }
  return NULL;
  }

// --------------------------------------------------------------------------

pCDriver CDriverArray::Add(const char *sPath, const char *pTag)
  {
  pCDriver pDrv = Find(pTag);
  if (pDrv)
    return pDrv;

  if (Used<Maximum)
    {
    Strng DrvCfg(sPath);
    DrvCfg += pTag;
    DrvCfg += ".DCF";
    Strng DrvDll(pTag);
    DrvDll += ".DLL";

    HINSTANCE hLib = NULL;
    hLib = AfxLoadLibrary(DrvDll());
    if (hLib)
      {
      fn_AddDrv f = (fn_AddDrv)GetProcAddress(hLib, "AddDrv");
      if (f==NULL)
        return NULL; //DLL existed, but was not the driver DLL
      pDrv = f((char*)pTag);
      ASSERT(pDrv); //DLL failed to create the driver
      }
    else
      LogError("DrvMngr", 0, "Unable to load driver %s", DrvDll());
    if (pDrv==NULL)
      return NULL;
    pDrv->hLib = hLib;
    pDrv->pDrvMngr = gs_pDrvMan;
    pDrv->CfgFile=DrvCfg;
    pDrv->DllFile=DrvDll;
    pDrv->bLclTagSrvrOK=gs_pPrj->bDrvLclTagSrvrOK;

    if (!pDrv->Configure(DrvCfg()))
      LogError("DrvMngr", 0, "Driver Configure Failed '%s' '%s'", pTag, DrvCfg());

    *(pItem+Used) = pDrv;
    Used++;
    
    return pDrv;
    }
  LogError("DrvMngr", 0, "Driver Space Expired can't add");
  return NULL;
  }

//===========================================================================

CDriverManager::CDriverManager() :
  SlotMap(),
  Drivers(this),
  CNodeXRefMngr()
  {
  UsedSlots = 0;
  UsedConns = 0;
  UsedCodeBlks = 0;
  EO_SubsRead_Busy = 0;
  BusyDriver = 0;
  BusyChanges = 0;
  //pChgHead = NULL;
  //pChgTail = NULL;
  bIsOpen = 0;
  bIgnoresOn = 0;
  bForcesOn = 0;
  bReadAll=0;
  bNoiseOn=1;

  bRecordHist = 0;
  bRecordHistDone = 0;
  bDoneLocalLoadSave = 0;

  iLoadSaveCnt     = 0;
  iRollingRdCntRqd = 0;
  iRollingRdCnt    = 0;
  iRollingRdIndex  = 0;
  iRollingWrCnt    = 0;
  iRollingWrCntRqd = 0;
  iRollingWrIndex  = 0;

  bDrvStartFBKCheck=FBKCheckNever;
  }

// --------------------------------------------------------------------------

CDriverManager::~CDriverManager()
  {
  Drivers.Alloc(0);
  while (!m_SltChanges.IsEmpty())
    delete m_SltChanges.RemoveHead();
  if (sPrevTempFile.Len())
    DeleteFile(sPrevTempFile());
  }

// --------------------------------------------------------------------------

void CDriverManager::Initialise()
  {
  dbgpln("DRV CDriverManager::Initialise()");
  dCurrentTime=0.0;
  bIsOpen=0;
  SlotMap.RemoveAll();
  SlotMap.InitHashTable(53);
  m_CodeBlkMap.RemoveAll();
  m_CodeBlkMap.InitHashTable(53);
  }

// --------------------------------------------------------------------------

pDrvSpan CDriverManager::FindSpan(long span, long offset, long clamplo, long clamphi)
  {
  const int Size = Spans.GetSize();
  for (int i=0; i<Size; i++)
    {
    if (Spans[i]->Span==span && Spans[i]->Offset==offset && Spans[i]->ClampLo==clamplo && Spans[i]->ClampHi==clamphi)
      return Spans[i];
    }
  return NULL;
  }

// --------------------------------------------------------------------------

pDrvSpan CDriverManager::AddSpan(long span, long offset, long clamplo, long clamphi)
  {
  pDrvSpan pI = FindSpan(span, offset, clamplo, clamphi);
  if (pI)
    return pI;
  pI = new DrvSpan(span, offset, clamplo, clamphi);
  Spans.Add(pI);
  return pI;
  }

// --------------------------------------------------------------------------

pDrvRange CDriverManager::FindRange(double RngLo, double RngHi)
  {
  const int Size = Ranges.GetSize();
  for (int i=0; i<Size; i++)
    {
    if (Ranges[i]->RngHi==RngHi && Ranges[i]->RngLo==RngLo)
      return Ranges[i];
    }
  return NULL;
  }

// --------------------------------------------------------------------------

pDrvRange CDriverManager::AddRange(double RngLo, double RngHi)
  {
  pDrvRange pI = FindRange(RngLo, RngHi);
  if (pI)
    return pI;
  pI = new DrvRange(RngLo, RngHi);
  Ranges.Add(pI);
  return pI;
  }

// --------------------------------------------------------------------------

pDrvHstCfg CDriverManager::FindHstCfg(const pDrvHstCfg pH)
  {
  const int Size = HstCfg.GetSize();
  for (int i=0; i<Size; i++)
    {
    if (pH->dFilterTau     == HstCfg[i]->dFilterTau   &&
        pH->dWinFltPeriod  == HstCfg[i]->dWinFltPeriod&&
        pH->dFltDelta      == HstCfg[i]->dFltDelta    &&
        pH->dDeltaLo       == HstCfg[i]->dDeltaLo     &&
        pH->dDeltaHi       == HstCfg[i]->dDeltaHi     &&
        pH->dDecrease      == HstCfg[i]->dDecrease    &&
        pH->iWinFltCount   == HstCfg[i]->iWinFltCount &&
        pH->iNoRec         == HstCfg[i]->iNoRec       &&
        pH->iNoNotRec      == HstCfg[i]->iNoNotRec    &&
        pH->iFilterAlg     == HstCfg[i]->iFilterAlg   &&
        pH->iBoxcarAlg     == HstCfg[i]->iBoxcarAlg)
      return HstCfg[i];
    }
  return NULL;
  }

// --------------------------------------------------------------------------

pDrvHstCfg CDriverManager::AddHstCfg(const pDrvHstCfg pH)
  {
  pDrvHstCfg pI = FindHstCfg(pH);
  if (pI)
    return pI;
  pI = new DrvHstCfg(pH);
  HstCfg.Add(pI);
  return pI;
  }

// --------------------------------------------------------------------------

pDrvArcCfg CDriverManager::FindArcCfg(const pDrvArcCfg pH)
  {
  const int Size = ArcCfg.GetSize();
  for (int i=0; i<Size; i++)
    {
    if (pH->m_sTable.XStrICmp(ArcCfg[i]->m_sTable)==0 &&
        pH->m_iFn == ArcCfg[i]->m_iFn )
      return ArcCfg[i];
    }
  return NULL;
  }

// --------------------------------------------------------------------------

pDrvArcCfg CDriverManager::AddArcCfg(const pDrvArcCfg pH)
  {
  pDrvArcCfg pI = FindArcCfg(pH);
  if (pI)
    return pI;
  pI = new DrvArcCfg(pH);
  ArcCfg.Add(pI);
  return pI;
  }

// --------------------------------------------------------------------------

int CDriverManager::FindSubsItem(char* pTag)
  {
  const int Size = ConnSubsItems.GetSize();
  const int len = strlen(pTag);
  for (int i=0; i<Size; i++)
    {
    if (ConnSubsItems[i]->m_pCon->m_sTag.Length()==len && stricmp(ConnSubsItems[i]->m_pCon->m_sTag(), pTag)==0)
      return i;
    }
  return -1;
  }

// --------------------------------------------------------------------------

int CDriverManager::Configure(const char * Path, const char * Name, flag IgnoresOn, flag ForcesOn, flag ReadAll, flag NoiseOn, byte DrvStartFBKCheck)
  {
  sPath = Path;
  if (sPath.Length()==0)
    sPath = PrjFiles(); //assume project directory if path not specified
  if (sPath.Length()>0 && sPath[sPath.Length()-1] != '\\')
    sPath += '\\';
  sName = Name;
  bIgnoresOn = IgnoresOn;
  bForcesOn = ForcesOn;
  bReadAll = ReadAll;
  bNoiseOn = NoiseOn;
  bDrvStartFBKCheck=DrvStartFBKCheck;
  return 0;
  }

// --------------------------------------------------------------------------

enum SCF_Cols
  {
  enTag,
  enConnection,
  enTyp,
  enDriver,
  enModifier,
  enRange,
  enFilter,
  enComprssn,
  enCnvs,
  enDescription,
//  enPV,
//  DrawingNo
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgOptions(CDrvCfgHelper &Cfg, char* pOpts, int line_number)
  {
  //get type and options ...
  Cfg.iAction = XIO_Null;
  Cfg.bRecordIt = 0;
  Cfg.bIgnoreWrites = 0;
  Cfg.bIgnoreReads = 0;
  Cfg.bLocal = 0;
  Cfg.bUseInitVal = 0;
  Cfg.dInitVal = 0.0;
  Cfg.bEnableForces = 0;
  Cfg.dForceVal = 0.0;
  Cfg.bSltDbgOn = 0;
  Cfg.iTyp = tt_NULL;
  StripSingleQuoteComment(pOpts);
  if (pOpts && strlen(pOpts)>0)
    {
    CSVColArray f;
    int Quote;
    if (pOpts[0]=='\"')
      pOpts[0]=' ';
    if (pOpts[strlen(pOpts)-1]=='\"')
      pOpts[strlen(pOpts)-1]=' ';
    int i = ParseCSVTokens(pOpts, f, Quote);
    while (i>0)
      {
      i--;
      //if      (stricmp(f[i], "r")==0)   Cfg.bRecordIt = 1;
      if (stricmp(f[i], "h")==0)   Cfg.bRecordIt = 1;
      //else if (stricmp(f[i], "w")==0)   Cfg.iAction = XIO_InOut;
      else if (stricmp(f[i], "wo")==0)  Cfg.iAction = XIO_In;
      else if (stricmp(f[i], "ro")==0)  Cfg.iAction = XIO_Out;
      else if (stricmp(f[i], "rw")==0)  Cfg.iAction = XIO_InOut;
      else if (stricmp(f[i], "l")==0)   Cfg.bLocal = 1;
      else if (stricmp(f[i], "b")==0)   Cfg.iTyp = tt_Bool;
      else if (stricmp(f[i], "u8")==0)  Cfg.iTyp = tt_Byte;
      else if (stricmp(f[i], "u16")==0) Cfg.iTyp = tt_Word;
      else if (stricmp(f[i], "u32")==0) Cfg.iTyp = tt_DWord;
      else if (stricmp(f[i], "i8")==0)  Cfg.iTyp = tt_Char;
      else if (stricmp(f[i], "i16")==0) Cfg.iTyp = tt_Short;
      else if (stricmp(f[i], "i32")==0) Cfg.iTyp = tt_Long;
      else if (stricmp(f[i], "f16")==0) Cfg.iTyp = tt_Flt16;
      else if (stricmp(f[i], "f32")==0) Cfg.iTyp = tt_Float;
      else if (stricmp(f[i], "f64")==0) Cfg.iTyp = tt_Double;
      else if (stricmp(f[i], "f")==0)   Cfg.bEnableForces = 1;
      else if (stricmp(f[i], "iw")==0)  Cfg.bIgnoreWrites = 1;
      else if (stricmp(f[i], "ir")==0)  Cfg.bIgnoreReads = 1;
      else if (stricmp(f[i], "irw")==0)
        {
        Cfg.bIgnoreWrites = 1;
        Cfg.bIgnoreReads = 1;
        }
      else if (strnicmp(f[i], "l=", 2)==0)
        {
        Cfg.bLocal = 1;
        if (f[i][2])
          {
          Cfg.bUseInitVal = 1;
          Cfg.dInitVal = atof(&(f[i][2]));
          }
        }
      else if (strnicmp(f[i], "f=", 2)==0)
        {
        Cfg.bEnableForces = 1;
        if (f[i][2])
          Cfg.dForceVal = atof(&(f[i][2]));
        }
      else if (stricmp(f[i], "?")==0)
        return -1; // dont load
      else if (stricmp(f[i], "!")==0)
        Cfg.bSltDbgOn=1;
      else
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Unknown '%s'", line_number, Cfg.sTag(), f[i]);
        return -1;
        }
      }
    }
  else
    {
    // Dont Load
    return -1;
    }

  if (Cfg.iAction==XIO_Null)
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Valid IO type (ro,wo or rw) must be specified", line_number, Cfg.sTag());
    return -1;
    }
  if (Cfg.iTyp==tt_NULL)
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Valid data type must be specified", line_number, Cfg.sTag());
    return -1;
    }
  if (Cfg.bLocal && Cfg.iAction!=XIO_In)
    {
    Cfg.iAction = XIO_In;
    LogWarning("DrvMngr", 0, "Line %u: '%s' IO type changed to wo for a 'local'", line_number, Cfg.sTag());
    }
  if (!(Cfg.iAction & XIO_In))
    Cfg.bIgnoreWrites = 0;
  if (!(Cfg.iAction & XIO_Out))
    Cfg.bIgnoreReads = 0;
  return 0;
  }

// --------------------------------------------------------------------------

const byte CRIO_In  = 1;
const byte CRIO_Out = 2;

struct CRmtIOTypes
  {
  LPCTSTR  m_Name;
  byte     m_Type;
  //...
  };
const CRmtIOTypes RmtIO[] =
  {
    {"In",      CRIO_In},
    {"Switch",  CRIO_In},
    {"Out",     CRIO_Out},
    {NULL, },
  };

// --------------------------------------------------------------------------

inline void CompactSepars(LPTSTR Buff)
  {
  char * p=Buff;
  while ((p=strchr(p,')'))!=NULL)
    {
    char *p1=p+1;
    while (*p1==' ') p1++;
    if (*p1==',' || *p1==':')
      memmove(p+1,p1,strlen(p1)+1);
    p++;
    }
  }

// --------------------------------------------------------------------------

bool CDriverManager::ParseConnOperator(CDrvSubsConn *pC, CDrvCfgHelper &Cfg, char *pMod, int line_number)
  {
  CSVColArray f;
  int Quote,nParms;

  if (pC->m_nOps>=DrvSubsConnMaxOps)
    LogError("DrvMngr", 0, "Line %u: '%s' Too Many Connection Operators Max %i", line_number, Cfg.sTag(), DrvSubsConnMaxOps);
      
  nParms = ParseCSVFunction(pMod, f, Quote) - 1;
  if (Quote)
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected", line_number, Cfg.sTag());
    return false;
    }
  if (stricmp(pMod,"Filter")==0)
    {
    if (nParms<1)// CNM Allow filter of integral types || !IsFloatData(Cfg.iTyp))
      {
      if (nParms<1)
        LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Filter(Type,[Parm1])", line_number, Cfg.sTag());
      if (IsFloatData(Cfg.iTyp))
        LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Filter can only be used for a float slot", line_number, Cfg.sTag());
      return false;
      }
    else 
      {
      pC->AddOp(new CDrvSubsConnOp_Filter((byte)SafeAtoL(f[1],1), SafeAtoF(f[2],0.0)));
      }
    }
  else if (stricmp(pMod,"Noise")==0)
    {
    if (nParms<3 || !IsFloatData(Cfg.iTyp))
      {
      if (nParms<3)
        LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Noise(Relative, Type=0,StdDev=1,[Mean=0])", line_number, Cfg.sTag());
      if (IsFloatData(Cfg.iTyp))
        LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Noise can only be used for a float slot", line_number, Cfg.sTag());
      return false;
      }
    else 
      {
      // NS_Gaus1     = 0 
      // NS_Gaus      = 1 
      // NS_Flat1     = 2 
      // NS_Flat      = 3 
      // NS_Pois1     = 4 
      // NS_Pois      = 5 
      // NS_Gamma1    = 6 
      // NS_Gamma     = 7 
      // NS_Weibull1  = 8 
      // NS_Weibull   = 9 

      pC->AddOp(new CDrvSubsConnOp_Noise(SafeAtoL(f[1],0)!=0, NoiseTypes(SafeAtoL(f[2],0)), SafeAtoF(f[3], 1.0), SafeAtoF(f[4], 0.0)));
      }
    }
  else if (stricmp(pMod,"Not")==0 || stricmp(pMod,"Invert")==0 || stricmp(pMod,"Inv")==0)
    {
    if (nParms!=0)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Not() or Invert()", line_number, Cfg.sTag());
      return false;
      }
    else 
      {
      pC->AddOp(new CDrvSubsConnOp_Not());
      }
    }
  else if (stricmp(pMod,"Profile")==0)
    {
    if (nParms<1 || nParms>2)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Profile(FileName, [Reverse])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }

    if (f[1]==NULL || strlen(f[1])==0)
      LogError("DrvMngr", 0, "Line %u: '%s' Invalid Filename", line_number, f[1] ? f[1] : "");
    CDrvSubsConnPrf * pProfile=new CDrvSubsConnPrf;
    Strng Fn=sPath;
    Fn+=f[1];
    long err=pProfile->Parse(Fn());
    if (err==0)
      {
      pC->AddOp(new CDrvSubsConnOp_Profile(SafeAtoL(f[2],0)!=0, pProfile));
      }
    else
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Parse error %s", line_number, Cfg.sTag(), pProfile->ErrorString(err));
      delete pProfile;
      }
    }
  else if (stricmp(pMod,"Scale")==0 || stricmp(pMod,"Scl")==0 || stricmp(pMod,"Tweak")==0)
    {
    if (nParms<1 || nParms>2)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Scale(Mult, Off)", line_number, Cfg.sTag());
      return false;
      }
    else 
      {
      pC->AddOp(new CDrvSubsConnOp_Scale(SafeAtoF(f[1],1.0), SafeAtoF(f[2],0.0)));
      }
    }
  else if (stricmp(pMod,"Sqr")==0)
    {
    if (nParms<1 || nParms>5)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator expected: Sqr(mult, [minI], [maxI], [minO], [maxO])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }

    double dMult=SafeAtoF(f[1],1.0);
    double dMin1=SafeAtoF(f[2],0.0);
    double dMax1=SafeAtoF(f[3],0.0);
    double dMin2=SafeAtoF(f[4],dMin1);
    double dMax2=SafeAtoF(f[5],dMax1);
    pC->AddOp(new CDrvSubsConnOp_Sqr(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (stricmp(pMod,"Sqrt")==0)
    {
    if (nParms<1 || nParms>5)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Sqrt(mult, [minI], [maxI], [minO], [maxO])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }

    double dMult=SafeAtoF(f[1],1.0);
    double dMin1=SafeAtoF(f[2],0.0);
    double dMax1=SafeAtoF(f[3],0.0);
    double dMin2=SafeAtoF(f[4],dMin1);
    double dMax2=SafeAtoF(f[5],dMax1);
    pC->AddOp(new CDrvSubsConnOp_Sqrt(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (stricmp(pMod,"Range")==0 || stricmp(pMod,"Rng")==0)
    {
    if (nParms<1 || nParms>2)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Range([min], [max])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid Connection Operator  expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    double dMin=SafeAtoF(f[1],-1.0e100);
    double dMax=SafeAtoF(f[2],1.0e100);
    pC->AddOp(new CDrvSubsConnOp_Range(dMin, dMax));
    }
  else if (stricmp(pMod,"Cmp")==0)
    {
    if (nParms!=2 || IsFloatData(Cfg.iTyp))
    // cnm if (nParms!=3 || Cfg.iTyp!=tt_Bool)
      {
      if (nParms!=2)
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Cmp(</<=/==/>=/>,value)", line_number, Cfg.sTag());
      // cnm
      if (IsFloatData(Cfg.iTyp))
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Cmp(</<=/==/>=/>,value) can only be used for a Non Float slot", line_number, Cfg.sTag());
      return NULL;
      }
    int Op=100;
    if (stricmp(f[1], "<")==0)
      Op=-2;
    else if (stricmp(f[1], "<=")==0)
      Op=-1;
    else if (stricmp(f[1], "==")==0)
      Op=0;
    else if (stricmp(f[1], ">=")==0)
      Op=1;
    else if (stricmp(f[1], ">")==0)
      Op=2;
    else
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Comparison operators '<,<=,==,>=,>' expected as 2nd parameter.", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    pC->AddOp(new CDrvSubsConnOp_Cmp(Op, SafeAtoF(f[2], 0.0)));
    }
  else
    LogError("DrvMngr", 0, "Line %u: '%s' Unknown connection '%s'", line_number, Cfg.sTag(), f[0]?f[0]:"");


  return true;
  };

// --------------------------------------------------------------------------

CDrvSubsConn * CDriverManager::ParseConnFunction(CDrvCfgHelper &Cfg, char* pConn, int line_number)
  {
  CDrvSubsConn *TheConn=NULL;

  CSVColArray f;
  int Quote,nParms;
      
  nParms = ParseCSVFunction(pConn, f, Quote) - 1;
  if (Quote)
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected", line_number, Cfg.sTag());
    return NULL;
    }
  if (stricmp(f[0],"get")==0/* || stricmp(f[0],"getinv")==0*/)
    {
    if (nParms!=1)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: get(tag)", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], stricmp(f[0],"getinv")==0);//, CCS_Null, 0);
    }
  else if (stricmp(f[0],"set")==0 || stricmp(f[0],"setinv")==0)
    {
    if (nParms<1 || nParms>3)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Incorrect number of parameters for set(...) or setinv(...)", line_number, Cfg.sTag());
      return NULL;
      }
    if (nParms==3 && Cfg.iTyp!=tt_Bool)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: can only use two delay times for a bit slot", line_number, Cfg.sTag());
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_Out))
    //  LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have read access", line_number, Cfg.sTag());
    DWORD DelayTime1 = 0;
    if (f[2]==NULL || strlen(f[2])==0)
      DelayTime1 = 0;
    else if (strnicmp(f[2], "inf", 3)==0)
      DelayTime1 = InfiniteDelay;
    else
      DelayTime1 = atol(f[2]);
    DWORD DelayTime2 = 0;
    flag UseDelay2 = 0;
    if (nParms==3)
      {
      UseDelay2 = 1;
      if (strnicmp(f[3], "inf", 3)==0)
        DelayTime2 = InfiniteDelay;
      else
        DelayTime2 = atol(f[3]);
      }
    CDrvSubsConn::AddSetConnect(Cfg.pConSlot, f[1], stricmp(f[0],"setinv")==0, DelayTime1, DelayTime2, UseDelay2);
    }
  else if (stricmp(f[0],"getcmp")==0)
    {
    if (nParms!=3 || IsFloatData(Cfg.iTyp))
    // cnm if (nParms!=3 || Cfg.iTyp!=tt_Bool)
      {
      if (nParms!=3)
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getcmp(tag,</<=/==/>=/>,value)", line_number, Cfg.sTag());
      // cnm
      if (IsFloatData(Cfg.iTyp))
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getcmp(tag,</<=/==/>=/>,value) can only be used for a Non Float slot", line_number, Cfg.sTag());
      return NULL;
      }
    int Op=100;
    if (stricmp(f[2], "<")==0)
      Op=-2;
    else if (stricmp(f[2], "<=")==0)
      Op=-1;
    else if (stricmp(f[2], "==")==0)
      Op=0;
    else if (stricmp(f[2], ">=")==0)
      Op=1;
    else if (stricmp(f[2], ">")==0)
      Op=2;
    if (Op=100)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Comparison operator '<', '<=', '==', '>=' or '>' expected as 2nd parameter.", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    
    TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], false);
    if (TheConn)
      TheConn->AddOp(new CDrvSubsConnOp_Cmp(Op, SafeAtoF(f[3], 0.0)));
    }
  else if (stricmp(f[0],"getabs")==0)
    {
    if (nParms!=1 || !IsFloatData(Cfg.iTyp))
      {
      if (nParms!=1)
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getabs(tag)", line_number, Cfg.sTag());
      if (IsFloatData(Cfg.iTyp))
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getabs(tag) can only be used for a float slot", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], false);//, CCS_Abs, 0);
    if (TheConn)
      TheConn->AddOp(new CDrvSubsConnOp_Abs);
    }
  else if (stricmp(f[0],"getrev")==0)
    {
    if (nParms!=2 || !IsFloatData(Cfg.iTyp))
      {
      if (nParms!=2)
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getrev(tag, value)", line_number, Cfg.sTag());
      if (!IsFloatData(Cfg.iTyp))
        LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getrev(tag, value) can only be used for a float slot", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    
    TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], false);//, CCS_Rev, atof(f[2]));
    if (TheConn)
      TheConn->AddOp(new CDrvSubsConnOp_Rev(SafeAtoF(f[2], 0.0)));
    }
  else if (stricmp(f[0],"getsqr")==0)
    {
    if (nParms<2 || nParms>6)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getsqr(tag, value, [minI], [maxI], [minO], [maxO])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }

  TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], false);//, CCS_Sqr, SafeAtoF(f[2],1.0), Min1, Max1, Min2, Max2);
    double dMult=SafeAtoF(f[2],1.0);
    double dMin1=SafeAtoF(f[3],0.0);
    double dMax1=SafeAtoF(f[4],0.0);
    double dMin2=SafeAtoF(f[5],dMin1);
    double dMax2=SafeAtoF(f[6],dMax1);
    if (TheConn)
      TheConn->AddOp(new CDrvSubsConnOp_Sqr(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (stricmp(f[0],"getsqrt")==0)
    {
    if (nParms<2 || nParms>6)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getsqrt(tag, value, [minI], [maxI], [minO], [maxO])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }

    TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], false);//, CCS_Sqrt, SafeAtoF(f[2],1.0), Min1, Max1, Min2, Max2);
    double dMult=SafeAtoF(f[2],1.0);
    double dMin1=SafeAtoF(f[3],0.0);
    double dMax1=SafeAtoF(f[4],0.0);
    double dMin2=SafeAtoF(f[5],dMin1);
    double dMax2=SafeAtoF(f[6],dMax1);
    if (TheConn)
      TheConn->AddOp(new CDrvSubsConnOp_Sqrt(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (stricmp(f[0],"getprofile")==0)
    {
    if (nParms<2 || nParms>3)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: getprofile(Tag, FileName, [Reverse])", line_number, Cfg.sTag());
      return NULL;
      }
    if (!(Cfg.iAction & XIO_In))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have write access", line_number, Cfg.sTag());
      return NULL;
      }
    
    if (f[2]==NULL || strlen(f[2])==0)
      LogError("DrvMngr", 0, "Line %u: '%s' Invalid Filename", line_number, f[2] ? f[2] : "");
    CDrvSubsConnPrf * pProfile=new CDrvSubsConnPrf;
    Strng Fn=sPath;
    Fn+=f[2];
    long err=pProfile->Parse(Fn());
    if (err==0)
      {
      TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, f[1], false);//, CCS_Profile, 1.0);//, pProfile);
      if (TheConn)
        TheConn->AddOp(new CDrvSubsConnOp_Profile(SafeAtoL(f[3],0)!=0, pProfile));
      }
    else
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Parse error %s", line_number, Cfg.sTag(), pProfile->ErrorString(err));
      delete pProfile;
      }
    }
  else if (stricmp(f[0],"setrev")==0)
    {
    if (nParms!=3)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Incorrect number of parameters for setrev(tag,val,delay)", line_number, Cfg.sTag());
      return NULL;
      }
    if (!IsFloatData(Cfg.iTyp))
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: can only use setrev for float slot", line_number, Cfg.sTag());
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_Out))
    //  LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected: Slot must have read access", line_number, Cfg.sTag());
     
    DWORD DelayTime1 = 0;
    if (strnicmp(f[3], "inf", 3)==0)
      DelayTime1 = InfiniteDelay;
    else
      DelayTime1 = atol(f[3]);
    DWORD DelayTime2 = 0;
    flag UseDelay2 = 0;
    TheConn=CDrvSubsConn::AddSetConnect(Cfg.pConSlot, f[1], false, DelayTime1, DelayTime2, UseDelay2);//, true, atof(f[2]));
    }
  else if (stricmp(f[0],"none")==0)
    {
    }
  else if (strlen(f[0])>0)
    {
    bool Input=false;
    for (int i=0; RmtIO[i].m_Name; i++)
	    {
      if (stricmp(f[0], RmtIO[i].m_Name)==0)
	      {
        Input=(RmtIO[i].m_Type==CRIO_In);
        break;
        }
      }
    if (RmtIO[i].m_Name)
      {
	    Strng TheTag=Cfg.sTag;
      if (nParms>=1 && f[1] && strlen(f[1])>0)
	      TheTag=f[1];

      if (gs_pTheSFELib>=0)
	      gs_pTheSFELib->FE_IOPnt_Add((LPCTSTR)TheTag(), Input, (LPCTSTR)Cfg.sDesc(), Cfg.iTyp, f[2], NULL);
       // Now push
      // Must also get/set it
      Strng FullTag("$IO.");
      FullTag+=TheTag;
      if (Input)
        TheConn=CDrvSubsConn::AddGetConnect(Cfg.pConSlot, FullTag(), false);
      else
        CDrvSubsConn::AddSetConnect(Cfg.pConSlot, FullTag(), false, 0, 0, false);
      }
    else
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid connection expected", line_number, Cfg.sTag());
      return NULL;
      }
    }
  return TheConn;
  }

int CDriverManager::Slt_CfgConnection(CDrvCfgHelper &Cfg, char* pConn, int line_number)
  {
  Cfg.sConn = pConn;
  Cfg.pConSlot = NULL;
  StripSingleQuoteComment(pConn);
  if (pConn && strlen(pConn)>0)
    {
    //char* f[MaxCSVCols];
    //int Quote,nParms;
    char Conns[4096];
    strcpy(Conns, pConn);
    CompactSepars(Conns);
    char* ff[9];
    ff[0]=&Conns[0];
    for (int i=1; i<9; i++)
      ff[i]=NULL;
    i=1;
    int Pos=0;
    while (strstr(&Conns[Pos],"),"))
      {
      if (i>=9)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Too many connection functions found", line_number, Cfg.sTag());
        return -1;
        }
      Pos = (strstr(&Conns[Pos],"),") - Conns) + 1;
      Conns[Pos++]=0;
      ff[i++]=&Conns[Pos];
      }
    while (i>0)
      {
      i--;
      char * pMod=strstr(ff[i], "):");
      if (pMod)
        {
        *(pMod+1)=0;
        pMod+=2;
        }
      CDrvSubsConn *pC=ParseConnFunction(Cfg, ff[i], line_number);

      while (pC && pMod)
        {
        char * pNxtMod=strstr(pMod, "):");
        if (pNxtMod)
          {
          *(pNxtMod+1)=0;
          pNxtMod+=2;
          }
        if (!ParseConnOperator(pC, Cfg, pMod, line_number))
          break;
        pMod=pNxtMod;
        }
      }
    }
  return 0;
  }

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgDriver(CDrvCfgHelper &Cfg, char *pDrive, int line_number)
  {
  StripSingleQuoteComment(pDrive);
  //get driver...
  CSVColArray f;
  int Quote,nParms;

  char LclBuff[256];
  if (strlen(pDrive)==0 && gs_pPrj->bDrvLclTagSrvrOK)
    {
    //sprintf(LclBuff, "Null(,%s)", Cfg.sTag());
    sprintf(LclBuff, "OPC(Local,%s)", Cfg.sTag());
    pDrive=LclBuff;
    }
  Cfg.sDrvCfg = pDrive;
  nParms = ParseCSVFunction(pDrive, f, Quote) - 1;
  // StrStripMatchedQuotes(C[j]);

  if (Cfg.sDrvCfg.Length()==0 || Quote)
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Valid driver expected", line_number, Cfg.sTag());
    return -1;
    }
  pCDriver pDrv = Drivers.Add(sPath(), f[0]);
  if (pDrv==NULL)
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Valid driver expected", line_number, Cfg.sTag());
    return -1;
    }
  Cfg.pDrv = pDrv;
  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgArchiveDB(CDrvCfgHelper &Cfg, char *pACfg, int line_number)
  {
  StripSingleQuoteComment(pACfg);
  if (pACfg && strlen(pACfg)>0)
    {
    CSVColArray f;
    int Quote,nParms;
    char ACfg[4096];
    strcpy(ACfg, pACfg);
    CompactSepars(ACfg);
    char* ff[9];
    ff[0]=&ACfg[0];
    for (int i=1; i<9; i++)
      ff[i]=NULL;
    i=1;
    int Pos=0;
    while (strstr(&ACfg[Pos],"),"))
      {
      if (i>=9)
        {
        LogError("ArchiveDB", 0, "Line %u: '%s' Too many Archive functions found", line_number, Cfg.sTag());
        return -1;
        }
      Pos = (strstr(&ACfg[Pos],"),") - ACfg) + 1;
      ACfg[Pos++]=0;
      ff[i++]=&ACfg[Pos];
      }
    while (i>0)
      {
      i--;
      nParms = ParseCSVFunction(ff[i], f, Quote) - 1;

      if (Quote || f[0]==NULL || nParms<1)
        {
        LogError("ArchiveDB", 0, "%s has Invalid Archive Spec: %s", Cfg.sTag(), ACfg);
        return -1;
        }

      for (int j=1; j<=nParms; j++)
        {
        byte What=ADBFn_Null;
        if      (stricmp(f[j], ADBFuncTags[ADBFn_Instant])==0)      What=ADBFn_Instant;
        else if (stricmp(f[j], ADBFuncTags[ADBFn_Minimum])==0)      What=ADBFn_Minimum;
        else if (stricmp(f[j], ADBFuncTags[ADBFn_Maximum])==0)      What=ADBFn_Maximum;
        else if (stricmp(f[j], ADBFuncTags[ADBFn_PeriodAvg])==0)    What=ADBFn_PeriodAvg;
        //else if (stricmp(f[j], ADBFuncTags[ADBFn_RunningAvg])==0)   What=ADBFn_RunningAvg
        else if (stricmp(f[j], ADBFuncTags[ADBFn_Count])==0)        What=ADBFn_Count;

        DrvArcCfg * p=new DrvArcCfg;
        p->m_sTable=f[0];
        p->m_iFn=What;
        Cfg.nA.Add(p);
        }
      }
    }


//#if WITHDARC
//  return m_ADB.AddItem(Cfg.sTag(), pDB, Cfg.lSlotIndex);
//#endif
  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgModifier(CDrvCfgHelper &Cfg, char *pMod, int line_number)
  {
  //get modifier...(span,offset)
  Cfg.bUseSpan = false;
  Cfg.nS.Span = 0;
  Cfg.nS.Offset = 0;
  Cfg.nS.ClearClamp();
  StripSingleQuoteComment(pMod);
  if (pMod && strlen(pMod)>0)
    {
    char Mods[4096];
    strcpy(Mods, pMod);
    char* ff[9];
    ff[0]=&Mods[0];
    for (int i=1; i<9; i++)
      ff[i]=NULL;
    i=1;
    int Pos=0;
    while (strstr(&Mods[Pos],"),"))
      {
      if (i>=3)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Too many Modifier functions found", line_number, Cfg.sTag());
        return -1;
        }
      Pos = (strstr(&Mods[Pos],"),") - Mods) + 1;
      Mods[Pos++]=0;
      ff[i++]=&Mods[Pos];
      }
    while (i>0)
      {
      i--;

      CSVColArray f;
      int Quote,nParms;
      nParms = ParseCSVFunction(ff[i], f, Quote) - 1;
      //if (strlen(pMod)==0)
      //  Cfg.bUseSpan = false;
      //else
      if (Quote)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected", line_number, Cfg.sTag());
        return -1;
        }
      else if (stricmp(f[0],"span")==0)
        {
        Cfg.bUseSpan = true;
        if (nParms!=2)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Span(span,offset).", line_number, Cfg.sTag());
          return -1;
          }
        if (Cfg.iTyp==tt_Bool)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Span invalid for bit slot", line_number, Cfg.sTag());
          return NULL;
          }
        Cfg.nS.Span = atol(f[1]);
        Cfg.nS.Offset = atol(f[2]);
        if (Cfg.nS.Span==0)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Span(span,offset). Span may NOT be 0.", line_number, Cfg.sTag());
          return -1;
          }
        }
      else if (stricmp(f[0],"clamp")==0)
        {
        if (nParms!=2)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Clamp(Lo,Hi).", line_number, Cfg.sTag());
          return -1;
          }
        if (!Cfg.bUseSpan)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Clamp used with no Span.", line_number, Cfg.sTag());
          return -1;
          }
        Cfg.nS.ClampLo= atol(f[1]);
        Cfg.nS.ClampHi= atol(f[2]);
        if (Cfg.nS.ClampLo>Cfg.nS.ClampHi)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Clamp(Lo,Hi). Lo <= Hi", line_number, Cfg.sTag());
          return -1;
          }
        }
      else if (stricmp(f[0],"bit")==0) // Single Bit in Word numbering 1..16
        {
        if (nParms!=1)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Bit(No).", line_number, Cfg.sTag());
          return -1;
          }
        int bitbase, wordwidth;
        Cfg.pDrv->BitBase(bitbase, wordwidth);

        int bit = atoi(f[1]);
        if (bit>=bitbase && bit <= bitbase+wordwidth-1)
          {
          Cfg.nS.Offset = bit-bitbase;  // shift
          Cfg.nS.Span = (long)((DWORD)1 << (DWORD)Cfg.nS.Offset); // mask
          }
        else
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid bit no expected Bit(No). No must be 1 to 16.", line_number, Cfg.sTag());
          return -1;
          }
        }
      else if (stricmp(f[0],"mask")==0) // Mask multiple bits and shift
        {
        if (nParms!=1)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Mask(HexNo).", line_number, Cfg.sTag());
          return -1;
          }
        Cfg.nS.Span = atol(f[1]);
        if (Cfg.nS.Span<0)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected Mask(HexNo).", line_number, Cfg.sTag());
          return -1;
          }
        if (Cfg.nS.Span)
          {
          int x = Cfg.nS.Span;
          while (1)
            {
            if (x & 0x1) break;
            Cfg.nS.Offset++;
            x = x >> 1;
            }
          }
        }
      else if (stricmp(f[0],"none")==0)
        Cfg.bUseSpan = false;
      else if (strlen(f[0])>0)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid modifier expected.", line_number, Cfg.sTag());
        return -1;
        }
      }
    }

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgRange(CDrvCfgHelper &Cfg, char *pRng, int line_number)
  {
  StripSingleQuoteComment(pRng);
  //get output ranges...
  Cfg.bUseRange = true;
  if (Cfg.iTyp==tt_Bool)
    {
    Cfg.nR.RngLo = 0.0;
    Cfg.nR.RngHi = 1.0;
    }
  else
    {
    CSVColArray f;
    int Quote,nParms;
    if (pRng)
      nParms = ParseCSVFunction(pRng, f, Quote) - 1;
    else
      {
      nParms=0;
      Quote=1;
      }
    if ((pRng==NULL || strlen(pRng)==0) && !IsFloatData(Cfg.iTyp))
      Cfg.bUseRange = false;
    else if (Quote)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Range expected.", line_number, Cfg.sTag());
      return -1;
      }
    else if ((stricmp(f[0], "none")==0) || (strlen(f[0])==0))
      Cfg.bUseRange = false;
    else if (nParms!=2 || stricmp(f[0], "range")!=0)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Ranges(Min,Max) expected.", line_number, Cfg.sTag());
      return -1;
      }
    else
      {
      Cfg.nR.RngLo = atof(f[1]);
      //  CNM
      Cfg.nR.RngHi = atof(f[2]);
      if (fabs(Cfg.nR.RngHi-Cfg.nR.RngLo)<1.0e-6)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Ranges(Min,Max) expected. Max must not equal Min.", line_number, Cfg.sTag());
        return -1;
        }
//      Cfg.nR.RngHi = Max(Cfg.nR.RngLo, atof(f[2]));
//      if (Cfg.nR.RngHi<=Cfg.nR.RngLo)
//        {
//        LogError("DrvMngr", 0, "Line %u: '%s' Ranges(Min,Max) expected. Max MUST be greater than Min.", line_number, Cfg.sTag());
//        return -1;
//        }
      }
    }

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgFilter(CDrvCfgHelper &Cfg, char *pFlt, int line_number)
  {
  Cfg.nH.iFilterAlg = HSFA_None;
  Cfg.nH.iBoxcarAlg = HSA_None;
  //get filters...
  StripSingleQuoteComment(pFlt);
  if (pFlt && strlen(pFlt)>0)
    {
    char Filters[1024];
    strcpy(Filters, pFlt);
    char* ff[5];
    ff[0]=&Filters[0];
    for (int i=1; i<5; i++)
      ff[i]=NULL;
    i=1;
    int Pos=0;
    while (strstr(&Filters[Pos],"),"))
      {
      Pos = (strstr(&Filters[Pos],"),") - Filters) + 1;
      Filters[Pos++]=0;
      ff[i++]=&Filters[Pos];
      }
    while (i>0)
      {
      i--;
      CSVColArray f;
      int Quote,nParms;
      nParms = ParseCSVFunction(ff[i], f, Quote) - 1;
      if (Quote)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Invalid filter.", line_number, Cfg.sTag());
        return -1;
        }
      if (stricmp(f[0],"window")==0)
        {
        if (nParms!=1)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid filter expected Window(len).", line_number, Cfg.sTag());
          return -1;
          }
        Cfg.nH.iFilterAlg |= HSFA_MovingAve;
        Cfg.nH.iWinFltCount = atol(f[1]);
        }
      else if (stricmp(f[0],"first")==0)
        {
        if (nParms!=3)
          {
          LogError("DrvMngr", 0, "Line %u: '%s' Valid filter expected First(Tau,Period,len).", line_number, Cfg.sTag());
          return -1;
          }
        Cfg.nH.iFilterAlg |= HSFA_Tau;
        Cfg.nH.dFilterTau = atof(f[1]);
        Cfg.nH.dWinFltPeriod = atof(f[2]);
        Cfg.nH.dFltDelta = atof(f[3]);
        }
      else if (strlen(f[0])>0)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid filter expected", line_number, Cfg.sTag());
        return -1;
        }
      }
    }

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgCompression(CDrvCfgHelper &Cfg, char *pCmp, int line_number)
  {
  //get compression...
  StripSingleQuoteComment(pCmp);
  if (pCmp && strlen(pCmp)>0)
    {
    CSVColArray f;
    int Quote,nParms;
    nParms = ParseCSVFunction(pCmp, f, Quote) - 1;
    if (Quote)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid compression expected.", line_number, Cfg.sTag());
      return -1;
      }
    if (stricmp(f[0],"box")==0)
      {
      if (nParms!=1)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid compression expected Box(delta).", line_number, Cfg.sTag());
        return -1;
        }
      Cfg.nH.iBoxcarAlg    = HSA_Boxcar;
      Cfg.nH.dDeltaLo      = atof(f[1]);
      }
    else if (stricmp(f[0],"abox")==0)
      {
      if (nParms!=5)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid compression expected ABox(DeltaLo,DeltaHi,NoRec,NoNotRec,Change).", line_number, Cfg.sTag());
        return -1;
        }
      Cfg.nH.iBoxcarAlg    = HSA_AdaptiveBoxcar;
      Cfg.nH.dDeltaLo      = atof(f[1]);
      Cfg.nH.dDeltaHi      = atof(f[2]);
      Cfg.nH.iNoRec        = atol(f[3]);
      Cfg.nH.iNoNotRec     = atol(f[4]);
      Cfg.nH.dDecrease     = atof(f[5]);
      }
    else if (strlen(f[0])>0)
      {
      LogError("DrvMngr", 0, "Line %u: '%s' Valid compression expected", line_number, Cfg.sTag());
      return -1;
      }
    }

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgCnvs(CDrvCfgHelper &Cfg, char *pCnv, int line_number)
  {
  //get cnvs...
  Cfg.m_iCnv = 0;
  StripSingleQuoteComment(pCnv);
  if (pCnv && strlen(pCnv)>0)
    {
    // ?? is this a Cnv or a Value List
    int l=strlen(pCnv);
    for (int i=0; i<l; i++)
      if (!isalnum(pCnv[i]) && pCnv[i]!='_')
        break;

    if (i<l && pCnv[i]=='(')
      {
      //Is Cnv
      CSVColArray f;
      int Quote,nParms;
      nParms = ParseCSVFunction(pCnv, f, Quote) - 1;
      if (Quote || nParms!=1)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid conversion expected.", line_number, Cfg.sTag());
        return -1;
        }
      Cfg.m_iCnv = gs_CnvsMngr.FindCnv(f[0]);
      if (Cfg.m_iCnv<0)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid conversion expected.", line_number, Cfg.sTag());
        return -1;
        }
      if (Cnvs[Cfg.m_iCnv]->Find(f[1])==NULL)
        {
        LogError("DrvMngr", 0, "Line %u: '%s' Valid conversion expected.", line_number, Cfg.sTag());
        return -1;
        }
      Cfg.m_sCnvUnits = f[1];
      }
    else
      {
      // assume ValueList
      Cfg.m_pValLst = new DDBValueLstMem;
      Strng X(pCnv);
      Cfg.m_pValLst->LoadFromCSV(X);
      }
    }

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_CfgTODO(CDrvCfgHelper &Cfg, char *pIOPnt, int line_number)
  {
  StripSingleQuoteComment(pIOPnt);
  if (pIOPnt && strlen(pIOPnt)>0)
    {
    }

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::Slt_Configure(DWORD line_number, int nFlds, char **c)//char *line)
  {
  //Tag,Connection,Type,Driver,Modifier,Range,Filters,Compression,Cnvs,Description,DrawingNo,PV

  CDrvCfgHelper Cfg;
  Cfg.lLineNumber=line_number;
  //get tag...
  Cfg.sTag = c[enTag];
  Cfg.sTag.Trim(" \t");
  if (Cfg.sTag.Len()>SCDPartTagLen-2)
    {
    LogError("DrvMngr", 0, "Line %u: Tag name too long", line_number);
    return -1;
    }
  if (TaggedObject::ValidateTagChanged(Cfg.sTag))
    LogNote("DrvMngr", 0, "Line %u: Tag '%s' altered to '%s'", line_number, c[enTag], Cfg.sTag());
  if (Cfg.sTag.Length()==0)
    {
    LogError("DrvMngr", 0, "Line %u: Tag not specified", line_number);
    return -1;
    }
  CXM_ObjectTag ObjTag(Cfg.sTag(), TABOpt_Exists);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route))
    {
    LogError("DrvMngr", 0, "Line %u: Tag '%s' already exists", line_number, Cfg.sTag());
    return -1;
    }

  int ret;
  if ((ret=Slt_CfgOptions(Cfg, c[enTyp], line_number))!=0) return ret;

  if ((IsFloatData(Cfg.iTyp) && nFlds<9) ||
      ((Cfg.iTyp==tt_Bool) && nFlds<4) || // bool mask bitno (if reqd) should be checked for in individual drivers
      (IsIntData(Cfg.iTyp) && nFlds<4) ||
      (IsUnSgnData(Cfg.iTyp) && nFlds<4))
    {
    LogError("DrvMngr", 0, "Line %u: '%s' Too few fields", line_number, Cfg.sTag());
    return -1;
    }

  //description...
  Cfg.sDesc = c[enDescription] ? c[enDescription] : "";

  // first do the cnvs so that the mapping is avail
  if ((ret=Slt_CfgCnvs(Cfg, c[enCnvs], line_number))!=0) return ret;

  if ((ret=Slt_CfgConnection(Cfg, c[enConnection], line_number))!=0) return ret;
  if ((ret=Slt_CfgDriver(Cfg, c[enDriver], line_number))!=0) return ret;
  if ((ret=Slt_CfgModifier(Cfg, c[enModifier], line_number))!=0) return ret;
  if ((ret=Slt_CfgRange(Cfg, c[enRange], line_number))!=0) return ret;
  if ((ret=Slt_CfgFilter(Cfg, c[enFilter], line_number))!=0) return ret;
  if ((ret=Slt_CfgCompression(Cfg, c[enComprssn], line_number))!=0) return ret;

  if (m_enArchiveDB>=0)
    if ((ret=Slt_CfgArchiveDB(Cfg, c[m_enArchiveDB], line_number))!=0) return ret;
  if (m_enTODO>=0)
    if ((ret=Slt_CfgTODO(Cfg, c[m_enTODO], line_number))!=0) return ret;


  //check if the driver is happy with the slot configuration...

  if (!Cfg.pDrv->CfgSlot(&Cfg, NULL))
    {
    if (!Cfg.bErrorFlagged)
      LogError("DrvMngr", 0, "Line %u: '%s' Valid driver configuration expected.", line_number, Cfg.sTag());
    return -1;
    }

  int TagIndex = FindChanForTag(Cfg.sTag());
  if (TagIndex == -1) // Does entry exist ?
    TagIndex = AddSlot(&Cfg);
  else
    LogError("DrvMngr", 0, "Line %u: Ignored entry tag '%s' allready exists.", line_number, Cfg.sTag());

  return TagIndex;
  }

// --------------------------------------------------------------------------

int CDriverManager::Slt_Configure2(DWORD SlotNo, CDrvTimeout * pTO)
  {
  return Slots[SlotNo]->pDrv->InitSlot(Slots[SlotNo], pTO);
  }

// --------------------------------------------------------------------------

int CDriverManager::CB_Configure(DWORD line_number, int nFlds, char **c)
  {
  if (nFlds>=3 && c[0] && c[2])
    {
    Strng BlkName=c[0];
    Strng BlkOpts=c[1]?c[1]:"";
    Strng BlkFile=sPath;
    BlkFile+=c[2];

    CDrvCodeBlk * p=new CDrvCodeBlk;

    p->m_Code.m_bLogAllBuildErrs=true;
    if (p->m_Code.Load(AF_All|AF_BackupFiles, BlkFile(), this, DefGenConSet))
      {
      p->m_sTag=BlkName;
      m_CodeBlkMap.SetAt(p->m_sTag(), UsedCodeBlks);
      UsedCodeBlks=m_CodeBlks.Add(p)+1;
      }
    else
      {
      LogError("DrvMngr", 0, "CodeBlock %s not loaded", BlkName());
      delete p;
      }
    }
  else
    {
    LogError("DrvMngr", 0, "CodeBlocks Insufficient Data", c[0]?c[0]:"");
    }
  return 0;
  }

// --------------------------------------------------------------------------

void CDriverManager::ConfigurationDump()
  {
  #if dbgDriverMan
  Strng OFilename = sPath;
  OFilename += sName;
  if (OFilename.Find('.')>=0)
    OFilename = OFilename.Left(OFilename.Find('.'));
  OFilename += ".dmp";
  FILE* fout = fopen(OFilename(), "wt");
  if (fout==NULL)
    return;
  fprintf(fout,"================================RANGES==================================================\n");
  for(int rr = 0; rr < Ranges.GetSize(); rr++)
    fprintf(fout,"RANGES[%03d] %10g to %10g\n",
            rr, Ranges[rr]->RngLo, Ranges[rr]->RngHi);
  fprintf(fout,"================================SPANS==================================================\n");
  for(rr = 0; rr < Spans.GetSize(); rr++)
    {
    fprintf(fout,"SPANS[%03d] span(%8d 0x%08x) offset(%5d 0x%08x)",
      rr,Spans[rr]->Span, Spans[rr]->Span, Spans[rr]->Offset, Spans[rr]->Offset);
    if (Spans[rr]->ClampValid())
      fprintf(fout," clamp(%10d %10d)",
        Spans[rr]->ClampLo, Spans[rr]->ClampHi);
    fprintf(fout,"\n");
    }
  fprintf(fout,"================================HSTCFGS==================================================\n");
  for(rr = 0; rr < HstCfg.GetSize(); rr++)
    {
    pDrvHstCfg pH =  HstCfg[rr];
    fprintf(fout,"HSTCFG[%03d] Tau %6g Period %6g Wind %2d fltr %d Compr %d\n",
       rr, pH->dFilterTau,pH->dWinFltPeriod,pH->iWinFltCount,
       pH->iFilterAlg,pH->iBoxcarAlg);
    }
  fprintf(fout,"================================ARCCFGS==================================================\n");
  for(rr = 0; rr < ArcCfg.GetSize(); rr++)
    {
    pDrvArcCfg pH =  ArcCfg[rr];
    fprintf(fout,"ARCCFG[%03d] Table %-20s ",
       rr, pH->m_sTable());
    }
  fprintf(fout,"================================SLOTS==================================================\n");
  for(rr = 0; rr < UsedSlots; rr++)
    {
    CDrvSlot *pSlt = Slots[rr];
    fprintf(fout,"SLOTS[%04d] iTyp %2d %s %s %-15.15s %-10.10s (%5s)",
       rr, pSlt->iTyp, 
       pSlt->m_bHasSet?"Set":"   ",
       pSlt->m_bHasSetAsGet?"SetG":"    ",
       pSlt->sTag(),
       pSlt->sDescription()?pSlt->sDescription():"",
       pSlt->m_sCnvTxt()?pSlt->m_sCnvTxt():"");
    fprintf(fout,pSlt->pSpan?"span(%5u,%5u)":"                 ",
       (pSlt->pSpan ? pSlt->pSpan->Span : 0.0), (pSlt->pSpan ? pSlt->pSpan->Offset : 0.0));
    fprintf(fout,pSlt->pRange?"range(%7g,%7g)":"                      ",
       (pSlt->pRange ? pSlt->pRange->RngLo : 0.0), (pSlt->pRange ? pSlt->pRange->RngHi : 0.0));
    
    if (pSlt->pSpan && pSlt->pSpan->ClampValid())
      fprintf(fout," clamp(%10d %10d)",
        pSlt->pSpan->ClampLo, pSlt->pSpan->ClampHi);
    fprintf(fout,"\n");

    }
  fprintf(fout,"=============DRIVERS==================\n");
  for(rr = 0; rr < Drivers.InUse(); rr++)
    {
    pCDriver pDrv = Drivers.At(rr);
    fprintf(fout,"DRIVERS[%02d] [%s]\n", rr, pDrv->sTag());
    pDrv->DumpIOMap();
    }
  fprintf(fout,"================================END===================================\n");
  fclose(fout);
  #endif
  }

// --------------------------------------------------------------------------

int CDriverManager::Open()
  {
 // EO_SubsRead_Busy = 0;
 // EO_Register(pExecName_DrvMngr, EOWrite_Thread | EORead_Thread | EOExec_Thread,/*Pri*/THREAD_PRIORITY_ABOVE_NORMAL,/*Stack*/50000, /*HashSize*/ 101);
 //
 // m_GotDrvManRoute=XFindObject(pExecName_DrvMngr, m_DrvManRoute, true);

  return DoOpen();
  }

int CDriverManager::ConfigCodeBlocks()
  {

  flag OpenOK=false;
  Strng Filename = sPath;
  Filename += "ScdCodeBlks.xls";
  Strng Ext;
  Ext.FnExt(Filename());
  Ext.Upper();
  DWORD line_number = 0;

  if (Ext.XStrCmp(".XLS")==0 || Ext.XStrCmp(".MDB")==0)
    {
    BOOL OK = TRUE;
    CFileStatus State;
    if (CFile::GetStatus(Filename(), State))
      {
      LPTSTR ColNames[] = {"scdLoopTag", "scdSchedule", "scdCodeBlock", "scdDescription", NULL};
      CDBHelper DB(Filename(), "ScdDrvr");
      if (DB.Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
        {
        OpenOK=true;
        if (DB.CheckFieldNames(ColNames))
          {
          bIsOpen = true;
          CStringArray Values;
          while (!DB.IsEOF())
            {
            DB.GetNextRow(Values);
            // reconstruct buffer
            CSVColArray c;
            const int BuffSize=8192;
            char Buff[BuffSize], *pB=Buff;
            for (int i=0; i<Values.GetSize(); i++)
              {
              strcpy(pB, Values[i]);
              c[i]=pB;
              pB+=strlen(pB)+1;
              ASSERT(pB-Buff<BuffSize);
              }
            c[i]=NULL;

            CB_Configure(++line_number, Values.GetSize(), c);
            if ((line_number%10)==0)
              CDlgBusy::SetLine(3, "Loading CodeBlock :%i  (%i slots)", line_number, UsedCodeBlks);
            }
          LogNote("DrvMngr", 0, "%u out of %u CodeBlocks loaded", UsedCodeBlks, line_number);
          }
        else
          {
          LogError("Cnvs", 0, "Correct column field names for '%s' not found.", Filename);
          }
        DB.Close();
        }
      }
    }
  else
    {
    FILE* f=fopen(Filename(), "rt");
    if (f)
      {
      OpenOK=true;
      PSTR pEmpty = " ";
      bIsOpen = true;
      char Buff[4096];
       // Skip Title Line
      if (fgets(Buff, sizeof(Buff)-1, f))
        line_number++;
      while (!feof(f))
        {
        if (fgets(Buff, sizeof(Buff)-1, f))
          {
          CSVColArray c;
          int Quote;
          int nFlds = ParseCSVTokens(Buff, c, Quote);
          if (Quote)
            {
            LogError("DrvMngr", 0, "Line %u: Quotes mismatched", line_number);
            return -1;
            }
          // StrStripMatchedQuotes(C[j]);

          CB_Configure(++line_number,nFlds, c);
          if ((line_number%10)==0)
            CDlgBusy::SetLine(3, "Loading CodeBlock :%i  (%i slots)", line_number, UsedCodeBlks);
          }
        }
      fclose(f);
      LogNote("DrvMngr", 0, "%u out of %u CodeBlocks loaded", UsedCodeBlks, line_number-1);
      }
    }
  if ((line_number%10)==0)
    CDlgBusy::SetLine(3, "Lines Loaded :%i  (%i CodeBlocks)", line_number-1, UsedSlots);

  return 0;
  };

// --------------------------------------------------------------------------

int CDriverManager::DoOpen()
  {
  CDlgBusy::Open("\nOpening Driver Manager");
  int Err = 0;
  dbgpln("DRV CDriverManager::Open()");
  Initialise();
  bIsOpen = false;
  m_enArchiveDB=-1;
  m_enTODO=-1;
  if (sName.Length()>0)
    {
    //// KCG
    //AfxMessageBox("Temporary Mods for MossGas");
    // prepare arrays...
    ConnSubsItems.SetSize(0, 256);
    ConSlots.SetSize(0, 256);
    Slots.SetSize(0, 256);
    UsedSlots = 0;
    UsedConns = 0;
    Spans.SetSize(0, 128);
    Ranges.SetSize(0, 128);
    HstCfg.SetSize(0, 128);
    ArcCfg.SetSize(0, 128);
    Drivers.Alloc(10);

    EO_SubsRead_Busy = 0;

    ConfigCodeBlocks();

    flag OpenOK=false;
    Strng Filename = sPath;
    Filename += sName;
    CDlgBusy::SetLine(1, "Opening Driver Manager : %s", sName());
    Strng Ext;
    Ext.FnExt(Filename());
    Ext.Upper();
    DWORD line_number = 0;
    if (Ext.XStrCmp(".XLS")==0 || Ext.XStrCmp(".MDB")==0)
      {
      BOOL OK = TRUE;
      CFileStatus State;
      if (CFile::GetStatus(Filename(), State))
        {
        LPTSTR ColNames[] = {"scdLoopTag", "scdConnection", "scdTyp", "scdDriver", "scdModifier", "scdRange", "scdFilter", "scdCompression", "scdConversion", "scdDescription", NULL};
        CDBHelper DB(Filename(), "ScdDrvr");
        if (DB.Open(DBConnect_Find, ColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
          {
          OpenOK=true;
          if (DB.CheckFieldNames(ColNames))
            {
            LPTSTR OptColNames[] = {"scdArchive", "scdTODO", NULL };
            DB.CheckOptionalFieldNames(OptColNames);
            m_enArchiveDB= (DB.FieldIndex(DB.NFieldIndexes()-2)<0 ? -1 : DB.NFieldIndexes()-2);
            m_enTODO= (DB.FieldIndex(DB.NFieldIndexes()-1)<0 ? -1 : DB.NFieldIndexes()-1);

            bIsOpen = true;
            CStringArray Values;
            while (!DB.IsEOF())
              {
              DB.GetNextRow(Values);
              // reconstruct buffer
              CSVColArray c;
              const int BuffSize=8192;
              char Buff[BuffSize], *pB=Buff;
              for (int i=0; i<Values.GetSize(); i++)
                {
                strcpy(pB, Values[i]);
                c[i]=pB;
                pB+=strlen(pB)+1;
                ASSERT(pB-Buff<BuffSize);
                }
              c[i]=NULL;

              Slt_Configure(++line_number, Values.GetSize(), c);
              if ((line_number%10)==0)
                CDlgBusy::SetLine(3, "Loading line :%i  (%i slots)", line_number, UsedSlots);
              }
            LogNote("DrvMngr", 0, "%u out of %u driver slots loaded", UsedSlots, line_number);
            }
          else
            {
            LogError("Cnvs", 0, "Correct column field names for '%s' not found.", Filename);
            }
          DB.Close();
          }
        }
      }
    else
      {
      FILE* f=fopen(Filename(), "rt");
      if (f)
        {
        OpenOK=true;
        PSTR pEmpty = " ";
        bIsOpen = true;
        char Buff[4096];
         // Skip Title Line
        if (fgets(Buff, sizeof(Buff)-1, f))
          line_number++;
        while (!feof(f))
          {
          if (fgets(Buff, sizeof(Buff)-1, f))
            {
            CSVColArray c;
            int Quote;
            int nFlds = ParseCSVTokens(Buff, c, Quote);
            if (Quote)
              {
              LogError("DrvMngr", 0, "Line %u: Quotes mismatched", line_number);
              return -1;
              }
            // StrStripMatchedQuotes(C[j]);

            Slt_Configure(++line_number,nFlds, c);
            if ((line_number%10)==0)
              CDlgBusy::SetLine(3, "Loading line :%i  (%i slots)", line_number, UsedSlots);
            }
          }
        fclose(f);
        LogNote("DrvMngr", 0, "%u out of %u driver slots loaded", UsedSlots, line_number-1);
        }
      }
    if ((line_number%10)==0)
      CDlgBusy::SetLine(3, "Lines Loaded :%i  (%i slots)", line_number-1, UsedSlots);

    CDrvTimeout TO(false, 2);
    flag Done;
    long Timeouts=0;
    long ValuesRead=0;
    long Values2Read=0;
    for (int s=0; s<Slots.GetSize(); s++)
        if (Slots[s]->pDrv->bTwoCfgPasses && !Slots[s]->bValInited)
          Values2Read++;
    do
      {
      Done=true;
      for (int s=0; s<Slots.GetSize(); s++)
        if (Slots[s]->pDrv->bTwoCfgPasses && !Slots[s]->bValInited)
          {
          if ((ValuesRead%10)==0)
            CDlgBusy::SetLine(3, "Reading Initial Values :%i of %i", ValuesRead+1, Values2Read);
          int Ret=Slt_Configure2(s, &TO);
          if (Ret<0)
            {
            Done=false;
            Timeouts++;
            }
          else if (Ret==0)
            ValuesRead++;
          }
      }
    while (!Done);
    if (Timeouts>0)
      {
      for (int s=0; s<Slots.GetSize(); s++)
        if (Slots[s]->pDrv->bTwoCfgPasses && !Slots[s]->bValInited)
          LogError("DrvMngr", 0, "Point %s timeout during initialise", Slots[s]->sTag());
      }

    if (!OpenOK)
      {
      LogError("DrvMngr", 0, "'%s' not opened", Filename());
      #if dbgDriverMan
      if (dbgDumpConfig())
        ConfigurationDump();
      #endif  
      }
    }

//Sleep(120000);

  if (bIsOpen)
    {
    if (1)
      {
      EO_Register(pExecName_DrvMngr, EOWrite_Thread | EORead_Thread | EOExec_Thread,/*Pri*/THREAD_PRIORITY_ABOVE_NORMAL,/*Stack*/50000, /*HashSize*/ 101);
      }
    else
      {
      #ifndef _DEBUG
      INCOMPLETECODE(); // restore below
      #endif
      EO_Register(pExecName_DrvMngr, EOWrite_Msg | EORead_Msg | EOExec_Msg,/*Pri*/THREAD_PRIORITY_ABOVE_NORMAL,/*Stack*/50000, /*HashSize*/ 101);
      }

    EO_AllowForceWriteOnStart(true);
    
    //setup slot map...
    SlotMap.RemoveAll();
    UINT PrimeNo = FindNextPrimeNumber((UINT)(UsedSlots*1.18));
    SlotMap.InitHashTable(PrimeNo);
    ConSlots.SetSize(UsedConns, 256);
    for (long i=0; i<Slots.GetSize(); i++)
      SlotMap.SetAt(Slots[i]->sTag(), i);

    //setup connection array
    int SlotConnCnt = 0;
    UsedConns = 0;
    int UsedSubs = 0;
    for (i=0; i<Slots.GetSize(); i++)
      {
      CDrvSlot& S = *(Slots[i]);
      CDrvSubsConn *pCon = S.m_pConnects;
      if (pCon)
        SlotConnCnt++;
      while (pCon)
        {
        CDrvSubsConn *DelCon = NULL;
        int SlotNo = FindChanForTag(pCon->m_sTag());
        
        int CBNo = -1;
        if (SlotNo>=0)
          {
          if (Slots[SlotNo]->pDrv==S.pDrv && pCon->m_bGet)
            {
            LogError("DrvMngr", 0, "Connection unable to 'get' driver tag %s for slot %s", pCon->m_sTag(), Slots[i]->sTag());
            DelCon = pCon;
            }
          else
            {
            if (SlotNo>=0 && Slots[SlotNo]->pDrv==S.pDrv)
              {
              pCon->bInternal = 1;
              pCon->m_pSlot = Slots[SlotNo];
              pCon->m_Typ = Slots[SlotNo]->iTyp;
              Slots[SlotNo]->bHasFarCon = 1;
              if (pCon->m_bGet)
                Slots[SlotNo]->bHasGetConn = 1;
              }
            }
          }
        else // if (SlotNo <0)
          {
          CDrvCodeBlk *pCdBlk;
          GCVar       *pVar;
          CBNo=FindCodeBlkForTag(pCon->m_sTag(), pCdBlk, pVar);
          if (CBNo>=0)
            {
            pCon->bInternal = 1;
            pCon->m_pSlot = NULL;
            pCon->m_pCdBlk=pCdBlk;
            pCon->m_pCdBlkVar=pVar;
            switch (pVar->WhatAmI())
              {
              case VarDouble :  pCon->m_Typ = tt_Double; break;
              case VarLong   :  pCon->m_Typ = tt_Long; break;
              case VarByte   :  pCon->m_Typ = tt_Byte; break;
              case VarBit    :  pCon->m_Typ = tt_Bool; break;
              //case VarStr    :  pCon->m_Typ = tt_Str; break;
              //case VarFunct  :  pCon->m_Typ = tt_Double; break;
              //case VarTag    :  pCon->m_Typ = tt_Double; break;
              //case VarClass  :  pCon->m_Typ = tt_Double; break;
              default:
                LogError("DrvMngr", 0, "Invalid Variable Type %s for slot %s", pCon->m_sTag(), Slots[i]->sTag());
                DelCon = pCon;
                break;
              };
            if (pCon->m_bGet)
              {
              // Must add this slot to CodeBlk for PostExec Processing
              S.m_bHasSetAsGet=1;
              pCon->m_pCdBlk->m_GetsFromMe.Add(pCon);              
              S.bHasGetConn = 1;
              }
            }
          else
            {
            pCon->bInternal = 0;
            //LogError("DrvMngr", 0, "Connection unable to find CodeBlk Variable %s for slot %s", pCon->m_sTag(), Slots[i]->sTag());
            //DelCon = pCon;
            }
          }

        if (!DelCon)
          {
          //if (!pCon->m_bGet)
          //  S.bHasSet = 1;
          flag ConErr=0;
          pCon->m_pParentSlot = Slots[i];
          if (!pCon->bInternal)
            {
            CXM_ObjectTag  ObjTag(pCon->m_sTag(), TABOpt_AllInfoOnce);
            CXM_ObjectData ObjData;
            CXM_Route      Route;
            if (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route)!=0)
              {
              CPkDataItem * pPItem=ObjData.FirstItem();
              if (pCon->m_bGet || (pPItem->Flags() & isParm)!=0)
                {
                if (pPItem->Contains(PDI_Value))
                  pCon->m_Typ = pPItem->Type();
                else
                  LogError("DrvMngr", 0, "Unknown Type for connection tag %s for %s", pCon->m_sTag(), pCon->m_pParentSlot->sTag());
                if (pPItem->Contains(PDI_CnvInfo))
                  {
                  pCon->m_iCnv = pPItem->CnvIndex();
                  long i=(pCon->m_iCnv);
                  if (i!=0 && Cnvs[i]->Find(pCon->m_sCnvTxt())==NULL)
                    LogError("DrvMngr", 0, "Cnv %s not found for connection tag %s for %s", pCon->m_sCnvTxt(), pCon->m_sTag(), pCon->m_pParentSlot->sTag());
                  }
            
                int SubsIndex = FindSubsItem(pCon->m_sTag());
                if (SubsIndex>=0)
                  ConnSubsItems[SubsIndex]->AddConn(pCon);
                else
                  {
                  ConnSubsItems.Add(new CConnSubsItem(pCon));
                  SubsIndex = UsedSubs;
                  UsedSubs++;
                  }
                pCon->iSubsIndex = SubsIndex;
                }
              else
                {
                ConErr=1;
                LogError("DrvMngr", 0, "Connection Tag %s can not be set (it is read only) for %s", pCon->m_sTag(), pCon->m_pParentSlot->sTag());
                DelCon=pCon;
                pCon->iSubsIndex = 0xffff;
                }
              }
            else
              {
              ConErr=1;
              LogError("DrvMngr", 0, "Connection Tag %s not found for %s", pCon->m_sTag(), pCon->m_pParentSlot->sTag());
              DelCon=pCon;
              pCon->iSubsIndex = 0xffff;
              }
            }
          if (!pCon->m_bGet && !ConErr)
            S.m_bHasSet = 1;
          if (!DelCon)
            {
            ConSlots.SetAt(UsedConns, pCon);
            UsedConns++;
            }
          }
        
        pCon = pCon->m_pNxt;
        
        if (DelCon)
          {//invalid connection, delete it...
          if (DelCon==S.m_pConnects)
            S.m_pConnects = pCon;
          else
            {
            CDrvSubsConn *pTempCon = S.m_pConnects;
            while (pTempCon->m_pNxt!=DelCon)
              pTempCon = pTempCon->m_pNxt;
            pTempCon->m_pNxt = pCon;
            }
          delete DelCon;
          }
        }
      if ((i%20)==0)
        CDlgBusy::SetLine(3, "Slot connects:%i of %i", i, UsedSlots);
      }
    
      for (i=0; i<Slots.GetSize(); i++)
      {
      CDrvSlot& S = *(Slots[i]);
      if (!(S.iAction & XIO_Out) && !S.bHasGetConn)
        LogError("DrvMngr", 0, "'%s' Invalid connection : Slot must have read access", S.sTag());
      }

    CDlgBusy::SetLine(3, "Slot connects:%i of %i", UsedSlots, UsedSlots);
    ConSlots.SetSize(UsedConns, 256);
    LogNote("DrvMngr", 0, "%u slots have %u connections", SlotConnCnt, UsedConns);

    //let drivers know all slots are now loaded...
    for (i=0; i<Drivers.InUse(); i++)
      Drivers.At(i)->ClearConfigBusy();
    //wait for each driver/driver thread do it's setup stuff before continueing...
    Sleep(50);
    for (i=0; i<Drivers.InUse(); i++)
      {
      int j=0;
//TODO restore

      while (!Drivers.At(i)->IsConfigComplete() && j<100) // 10 secs
        {
        Sleep(100);
        j++;
        }
      if (!Drivers.At(i)->IsConfigComplete())
        LogError("DrvMngr", 0, "Driver thread initialisation/configuration not complete!");
      }

    if (sPrevTempFile.Len())
      {//restore previous state...
      FilingControlBlock FCB(PrjFiles(), DefaultCfgLabel, TOF_DBase);
      HANDLE H = CreateFile(sPrevTempFile(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
      if (H!=INVALID_HANDLE_VALUE)
        {
        DWORD dwSize = GetFileSize(H, NULL);
        if (dwSize>0 && dwSize!=0xFFFFFFFF)
          {
          char Buff[9];
          FCB.InitialiseForRead(H, 0, dwSize);
          //SetFilePointer(H, 0, NULL, FILE_BEGIN);
          FCB.ReadFile((LPVOID)Buff, 9, &dwSize);
          ASSERT(strncmp("TAGS....", Buff, 8)==0);
          if (!FCB.Eof())
            {
            LoadTags(FCB);
            for (long j=0; j<Slots.GetSize(); j++)
              {
              CDrvSlot& S = *(Slots[j]);
              if (S.bUseInitVal && (!bIgnoresOn || !S.bIgnoreWrites))
                {
                PkDataUnion Data(S.dInitVal);
                S.PutTagValue(Data);
                }
              }
            }
          CloseHandle(H);
          }
        }
      }

    //is it safe to assume the historian EO is opened before the driver manager EO ???
    //RecordSlots();
    }
  else
    Err = 2;

  #if dbgDriverMan
  if (dbgDumpConfig())
    ConfigurationDump();
  #endif  
  
  XBuildDataLists(); //otherwise if connect column tags are removed, and the driver is reloaded, SysCAD crashes
  CDlgBusy::Close();

  return Err;
  }

// --------------------------------------------------------------------------

flag CDriverManager::Close(BOOL SaveState/*=true*/)
  {
  flag ret=DoClose();

  //EO_DeRegister();
  return ret;
  }

//---------------------------------------------------------------------------

flag CDriverManager::DoClose(BOOL SaveState/*=true*/)
  {
  if (SaveState)
    {
    if (sPrevTempFile.Len()==0)
      {
      char File[MAX_PATH];
      //CNM if (GetTempFileName(PrjFiles(), "DRV", 0, File))
      if (GetTempFileName(TemporaryFiles(), "DRV", 0, File))
        sPrevTempFile = File;
      else
        sPrevTempFile = "";
      }
    if (sPrevTempFile.Len())
      {
      FilingControlBlock FCB(PrjFiles(), DefaultCfgLabel, TOF_DBase);
      HANDLE H = CreateFile(sPrevTempFile(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
      if (H!=INVALID_HANDLE_VALUE)
        {
        FCB.InitialiseForWrite(H, 0);
        SaveTags(FCB, true);
        CloseHandle(H);
        }
      }
    }
  else
    {
    if (sPrevTempFile.Len())
      {
      DeleteFile(sPrevTempFile());
      sPrevTempFile = "";
      }
    }

  dbgpln("DRV CDriverManager::Close");
  if (pDrvSheet)
    {
    pDrvSheet->PressButton(PSBTN_CANCEL);
    delete pDrvSheet;
    }
  if (bIsOpen)
    {
    CDlgBusy::Open("\n\nClosing Driver Manager");
    for (long i=0; i<Drivers.InUse(); i++)
      Drivers.At(i)->Close();
    Sleep(100);
    EO_DeRegister();
    const int ConSize = ConSlots.GetSize();
    for (i=0; i<ConSize; i++)
      {
      CDrvSubsConn *pCon = ConSlots[i];
      delete pCon;
      }
    ConSlots.SetSize(0, 256);
    const int SubsSize = ConnSubsItems.GetSize();
    for (i=0; i<SubsSize; i++)
      {
      pCConnSubsItem pSubsItem = ConnSubsItems[i];
      delete pSubsItem;
      }
    ConnSubsItems.SetSize(0, 256);
    Slots.SetSize(0, 256);
    SlotMap.RemoveAll();
    const int SpnSize = Spans.GetSize();
    for (i=0; i<SpnSize; i++)
      delete Spans[i];
    Spans.SetSize(0, 128);
    const int RngSize = Ranges.GetSize();
    for (i=0; i<RngSize; i++)
      delete Ranges[i];
    Ranges.SetSize(0, 128);
    const int HCfgSize = HstCfg.GetSize();
    for (i=0; i<HCfgSize; i++)
      delete HstCfg[i];
    HstCfg.SetSize(0, 128);
    const int ACfgSize = ArcCfg.GetSize();
    for (i=0; i<ACfgSize; i++)
      delete ArcCfg[i];
    ArcCfg.SetSize(0, 128);
    UsedConns = 0;
    UsedSlots = 0;
    UsedCodeBlks = 0;
    Drivers.Alloc(0); // Delete all Drivers (DLL's will be unloaded)
    while (!m_SltChanges.IsEmpty())
      delete m_SltChanges.RemoveHead();

    for (i=0; i<m_CodeBlks.GetSize(); i++)
      delete m_CodeBlks[i];
    m_CodeBlks.SetSize(0);
    m_CodeBlkMap.RemoveAll();

    CDlgBusy::Close();
    }

  Initialise();
  return true;
  }

//---------------------------------------------------------------------------

int CDriverManager::NoDrivers() { return Drivers.InUse(); };
DriverStates CDriverManager::DriverState(int i) { return DS_Inactive; };

//---------------------------------------------------------------------------
//#ifdef xxxxxxxxxxxxxxxxx
void CDriverManager::RecordSlots()
  {
  return;
  //the implementation below is VERY VERY slow...
/***  CNM
  if (!bRecordHist)
    {
    bRecordHistDone = 1;
    return;
    }
  Strng WrkTag, WrkCnvTxt;
  bRecordHistDone = 0;
  CXM_Route HRoute;
  if (XFindObject(pExecName_Historian, HRoute))
    {
    bRecordHistDone = 1;
    for (long SlotNo=0; SlotNo<Slots.GetSize(); SlotNo++)
      {
      if (Slots[SlotNo]->sTag.Length()>0 && !Slots[SlotNo]->bHstCfgd)
        {
        rCDrvSlot Slt=*(Slots[SlotNo]);
        TaggedObject::SplitTagCnv(Slt.sTag(), WrkTag, WrkCnvTxt);
        CXMsgLst XM;
        CXM_KeepHistory xb(SlotNo,
                          (byte)Slt.iTyp,
                          Slt.sTag(),
                          Slt.iCnv,
                          Slt.sCnvTxt(),
                          Slt.sDescription(),
                          Slt.pHstCfg->dFilterTau,
                          Slt.pHstCfg->dWinFltPeriod,
                          Slt.pHstCfg->iWinFltCount,
                          Slt.pHstCfg->dFltDelta,
                          Slt.pHstCfg->dDecrease,
                          Slt.pHstCfg->dDeltaLo,
                          Slt.pHstCfg->dDeltaHi,
                          Slt.pHstCfg->iNoRec,
                          Slt.pHstCfg->iNoNotRec,
                          Slt.pHstCfg->iFilterAlg,
                          Slt.pHstCfg->iBoxcarAlg,
                          (flag)Slt.bRecordIt,
                          true);
        XM.PackMsg(xb);
        if (XSendMessage(XM, HRoute))
          Slt.bHstCfgd=1;
        }
      if ((SlotNo%10)==0)
        CDlgBusy::SetLine(3, "Slot recording:%i of %i", SlotNo, UsedSlots);
      }
    }
    **/
  }
//#endif
//---------------------------------------------------------------------------

int CDriverManager::FindChanForTag(const char * pTag)
  {// Find SlotNo for Tag
  long Index;
  if (SlotMap.Lookup((char*)pTag, Index))
    return Index;
  return -1;
  /*int n = Slots.GetSize();
  for (int i=0; i<n; i++)
    {
    if (stricmp(Slots[i]->sTag(), pTag)==0)
      return i;
    }
  return -1;*/
  }

//---------------------------------------------------------------------------

int CDriverManager::FindCodeBlkForTag(const char* pTag, CDrvCodeBlk *&pCdBlk, GCVar * &pVar)
  {// Find CB for Tag
  char Buff[1024];
  strcpy(Buff, pTag);
  char *p=strchr(Buff, '.');
  if (p)
    {
    *p=0;
    char *pCBTg=Buff;
    char *pVarTg=p+1;
    long Index;
    if (m_CodeBlkMap.Lookup((char*)pCBTg, Index))
      {
      pCdBlk=m_CodeBlks[Index];
      pVar=m_CodeBlks[Index]->m_Code.m_pVarList->FindTag(pVarTg);
      //while (pVar)
      //  {
      //  if (stricmp(pCBTg, pVar->FindTag())
      //  pVar=pVar->Next();
      //  }
      if (pVar)
        return Index;
      }
    }
  pCdBlk=NULL;
  pVar=NULL;
  return -1;
  /*int n = Slots.GetSize();
  for (int i=0; i<n; i++)
    {
    if (stricmp(Slots[i]->sTag(), pTag)==0)
      return i;
    }
  return -1;*/
  }

//---------------------------------------------------------------------------

int CDriverManager::AddSlot(CDrvCfgHelper * pCfg)
  {
  ASSERT(pCfg->pDrv); //a driver must exist for the slot
  ASSERT(pCfg->sTag.Length()>0); //a valid tag must be entered

  if (pCfg->sTag.XStrICmp("ZIL-1157")==0)
    {
    int b=0;
    }

  
  pDrvSpan pSpan = NULL;
  if (pCfg->bUseSpan)
    pSpan = AddSpan(pCfg->nS.Span, pCfg->nS.Offset, pCfg->nS.ClampLo, pCfg->nS.ClampHi);
  pDrvRange pRange = NULL;
  if (pCfg->bUseRange)
    pRange = AddRange(pCfg->nR.RngLo, pCfg->nR.RngHi);
  //convert all values to SI units
  if (pCfg->m_iCnv>0)
    {
    CDataCnv &C=*Cnvs[pCfg->m_iCnv];
    pCfg->dForceVal    = C.Normal(pCfg->dForceVal, pCfg->m_sCnvUnits());
    pCfg->dInitVal     = C.Normal(pCfg->dInitVal, pCfg->m_sCnvUnits());
    pCfg->nH.dFltDelta = C.Normal(pCfg->nH.dFltDelta, pCfg->m_sCnvUnits());
    pCfg->nH.dDeltaLo  = C.Normal(pCfg->nH.dDeltaLo, pCfg->m_sCnvUnits());
    pCfg->nH.dDeltaHi  = C.Normal(pCfg->nH.dDeltaHi, pCfg->m_sCnvUnits());
    pCfg->nH.dDecrease = C.Normal(pCfg->nH.dDecrease, pCfg->m_sCnvUnits());
    }
//  pDrvHstCfg pHstCfg = AddHstCfg(&(pCfg->nH));
//  pDrvArcCfg pArcCfg = NULL;//AddArcCfg(&(pCfg->nA));

  CSPDrvSlot pSlt;
  int SlotNo = FindChanForTag(pCfg->sTag());
  if (SlotNo == -1)
    {
    pCfg->lSlotIndex=UsedSlots;
    pSlt = pCfg->pDrv->AddSlot(pCfg);
    if (pSlt==NULL)
      {
      LogError("DrvMngr", 0, "Unable to add driver slot '%s'", pCfg->sTag());
      return -1;
      }
    SlotNo = UsedSlots;
    Slots.InsertAt(UsedSlots++, pSlt);
    }
  if (SlotNo >= 0)
    {
    pSlt = Slots[SlotNo];
    pSlt->SlotNo        = SlotNo;
    pSlt->m_iCnv        = pCfg->m_iCnv;
    pSlt->m_sCnvTxt     = pCfg->m_sCnvUnits();
    pSlt->m_pValLst     = pCfg->m_pValLst;
    pSlt->sDescription  = pCfg->sDesc();
    pSlt->sConnection   = pCfg->sConn();
    pSlt->pDrv          = pCfg->pDrv;
    pSlt->pSpan         = pSpan;
    pSlt->pRange        = pRange;

    pSlt->pHstCfg       = AddHstCfg(&(pCfg->nH));;
    for (int a=0; a<pCfg->nA.GetSize(); a++)
      pSlt->ArcCfg.Add(AddArcCfg(pCfg->nA[a]));//       = pHstCfg;

    pSlt->m_pConnects      = pCfg->pConSlot;
    pSlt->iAction       = pCfg->iAction;
    pSlt->iTyp          = pCfg->iTyp;
    pSlt->bRecordIt     = pCfg->bRecordIt;
    pSlt->bLocal        = pCfg->bLocal;
    pSlt->bUseInitVal   = pCfg->bUseInitVal;
    pSlt->dInitVal      = pCfg->dInitVal;
    pSlt->bIgnoreReads  = pCfg->bIgnoreReads;
    pSlt->bIgnoreWrites = pCfg->bIgnoreWrites;
    pSlt->bSltDbgOn     = pCfg->bSltDbgOn;
    pSlt->m_bReqdByExec = 0;
    pSlt->bHstCfgd      = 0;
    pSlt->bArcCfgd      = 0;
    pSlt->bValInited    = 0;
    SlotMap.SetAt(pSlt->sTag(), (long)SlotNo);
    CDrvSubsConn *pCon = pSlt->m_pConnects;
    while (pCon)
      {
      UsedConns++;
      pCon = pCon->m_pNxt;
      }
    bRecordHist |= pSlt->bRecordIt;
    }
  else
    {
    CDrvSubsConn *pCon = pCfg->pConSlot;
    while (pCon)
      {
      UsedConns++;
      CDrvSubsConn *pDel = pCon;
      pCon = pCon->m_pNxt;
      delete pDel;
      }
    }
  return SlotNo;
  }

//===========================================================================

CDrvSltChange *CDriverManager::FindSltChange(long DataIndex)
  {//lock should allready be in place!
  POSITION pos=m_SltChanges.GetHeadPosition();
  while (pos)
    {
    CDrvSltChange *pChg = m_SltChanges.GetNext(pos);
    if (pChg->DataIndex==DataIndex)
      return pChg;
    }
  return NULL;
  }

//---------------------------------------------------------------------------

void CDriverManager::AddSltChange(long DataIndex, double dVal, CDrvSubsConn * pCon, long Level)
  {
  CDrvSlot *pSlt=pCon->m_pSlot;
  CDrvCodeBlk *pCdBlk=pCon->m_pCdBlk;
  #if dbgDriverMan
  LPTSTR pTag=pCon?pCon->m_sTag():"";
  if (!pTag) pTag="";
  #endif

  if (!Valid(dVal))
    {
    #if dbgDriverMan
    if (dbgSubsQueue())
      dbgpln("%*s..AddSltChange INVALID dVal [%4i] %s", (Level*8)+4, "", DataIndex, pTag);
    #endif
    }
  if (IsNumData(pCon->m_Typ))
    {
    LockXO();
    //Look in list to see if this tag is allready sitting here waiting to be sent...
    CDrvSltChange *pChg = FindSltChange(DataIndex);
    if (pChg)
      {//change value
      #if dbgDriverMan
      if (dbgSubsQueue())
        dbgpln("%*s..AddSltChange Changed %10g > %10g [%4i] %s", (Level*8)+4, "", pChg->dVal, dVal, DataIndex, pTag);
      #endif
      pChg->dVal = dVal;
      }
    else
      {//new, add to list...
      #if dbgDriverMan
      if (dbgSubsQueue())
        {
        if (pSlt)
          dbgpln("%*s..AddSltChange %10g (%-3s) [%4i] %s", (Level*8)+4, "", dVal, pSlt->m_sCnvTxt()?pSlt->m_sCnvTxt():"", DataIndex, pTag);
        else
          dbgpln("%*s..AddSltChange %10g (%-3s) [%4i] %s", (Level*8)+4, "", dVal, "", DataIndex, "");
        }
      #endif

      if (pCon->m_pSlot)
        pChg = new CDrvSltChange(pCon->m_pSlot, DataIndex, dVal, pCon->m_Typ, pCon->m_iCnv, pCon->m_sCnvTxt());
      else if (pCon->m_pCdBlk)
        pChg = new CDrvSltChange(pCon->m_pCdBlk, pCon->m_pCdBlkVar, DataIndex, dVal, pCon->m_Typ, pCon->m_iCnv, pCon->m_sCnvTxt());
      else
        pChg = new CDrvSltChange(NULL, DataIndex, dVal, pCon->m_Typ, pCon->m_iCnv, pCon->m_sCnvTxt());
      if (pChg)
        m_SltChanges.AddTail(pChg);
      }
    UnlockXO();
    }
  else
    {
    #if dbgDriverMan
    if (dbgSubsQueue())
      {
      if (pSlt)
        dbgpln("%*s..SKIP AddSltChange [typ:%i]'%s'", (Level*8)+4, "", pCon->m_Typ, pTag);
      else
        dbgpln("%*s..SKIP AddSltChange [typ:%i]", (Level*8)+4, "", pCon->m_Typ);
      }
    #endif
    ASSERT(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CDriverManager::AddSltChange(long DataIndex, double dVal, CDrvSlot * pSlt, long Level)
  {
  if (!Valid(dVal))
    {
    #if dbgDriverMan
    if (dbgSubsQueue())
      dbgpln("%*s..AddSltChange INVALID dVal [%4i] %s", (Level*8)+4, "", DataIndex, pSlt?pSlt->sTag():"");
    #endif
    }
  
  if (IsNumData(pSlt->iTyp))
    {
    LockXO();
    //Look in list to see if this tag is allready sitting here waiting to be sent...
    CDrvSltChange *pChg = FindSltChange(DataIndex);
    if (pChg)
      {//change value
      #if dbgDriverMan
      if (dbgSubsQueue())
        dbgpln("%*s..AddSltChange Changed %10g > %10g [%4i] %s", (Level*8)+4, "", pChg->dVal, dVal, DataIndex, pSlt?pSlt->sTag():"");
      #endif
      pChg->dVal = dVal;
      }
    else
      {//new, add to list...
      #if dbgDriverMan
      if (dbgSubsQueue())
        dbgpln("%*s..AddSltChange %10g (%-3s) [%4i] %s", (Level*8)+4, "", dVal, pSlt->m_sCnvTxt()?pSlt->m_sCnvTxt():"", DataIndex, pSlt?pSlt->sTag():"");
      #endif
      pChg = new CDrvSltChange(pSlt, DataIndex, dVal, pSlt->iTyp, pSlt->m_iCnv, pSlt->m_sCnvTxt());
      m_SltChanges.AddTail(pChg);
      }
    UnlockXO();
    }
  else
    {
    #if dbgDriverMan
    if (dbgSubsQueue())
      {
      dbgpln("%*s..SKIP AddSltChange [typ:%i]'%s'", (Level*8)+4, "", pSlt->iTyp, pSlt->sTag()?pSlt->sTag():"");
      }
    #endif
    ASSERT(FALSE);
    }
  }

//---------------------------------------------------------------------------

DWORD CDriverManager::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  DWORD RetCode = 0;
  while (XM.MsgAvail())
    switch (XM.MsgId())
      {
      case XM_DrvShowTagInfo :
        {
        CXM_DrvShowTagInfo *pX = XM.DrvShowTagInfo();
        const int SlotNo = FindChanForTag(pX->cTag);
        if (SlotNo>=0)
          {//tag found, show driver tag info as required...
          ASSERT(pX->iDrvOptMask!=0);
          RetCode = 1; //valid driver tag
          CString s;
          if (pX->iDrvOptMask & 0x0001)
            {//show in status bar
            Slots[SlotNo]->GetLongDrvDesc(s);
            pStatusBar->SetMsg1((char*)(const char*)s); //place temporary message on status bar
            }
          if (pX->iDrvOptMask & 0x0002)
            {//show in message window
            if ((pX->iDrvOptMask & 0x0001)==0)
              Slots[SlotNo]->GetLongDrvDesc(s);
            LogNote("Driver", 0, (char*)(const char*)s);
            }
          if (pX->iDrvOptMask & 0x0004)
            {//show in driver slot dialog
            Options(pX->cTag);
            }
          if (pX->iDrvOptMask & 0x0008)
            {//paste driver tag/address in clipboard
            Slots[SlotNo]->GetTagOrAddrDesc(s);
            CopyTextToClipboard(AfxGetMainWnd(), (char*)(const char*)s);
            }
          }
        break;
        }
      default:
        ASSERT(0);
      }
  XM.Clear();
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd)
  {
  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_Start(const CXM_TimeControl &CB)
  {
  if (!bRecordHistDone)
    RecordSlots(); //ensure the historian knows what to record
  flag OK = 1;

  int i;

//TODO FIX - should this be here or not!
//  for (i=0; i<Slots.GetSize(); i++)
//    Slots[i]->m_bReqdByExec = Slots[i]->m_bHasSet;

  switch (bDrvStartFBKCheck)
    {
    case FBKCheckSkipOnce:
      bDrvStartFBKCheck=FBKCheckAlways;
      // fall thru
    case FBKCheckNever:
      break;
    case FBKCheckOnce:
      bDrvStartFBKCheck=FBKCheckNever;
      // fall thru
    case FBKCheckAlways: 
      RefreshConnections();
      break;
    }

  for (i=0; i<Drivers.InUse(); i++)
    OK = OK && Drivers.At(i)->Start();

  return OK;
  }

//---------------------------------------------------------------------------

void CDriverManager::EO_QueryHistoryCfg(CXMsgLst &XM)
  {
  CXM_ReadIndexedData * pMsg = XM.ReadIndexedData();
  flag ReadAll = pMsg->ReadAll;
  long DataIndex = pMsg->Start ? 0 : pMsg->LastIndex+1;
  Strng WrkTag, WrkCnvTxt;
  XM.Clear();
  for (   ; DataIndex<Slots.GetSize(); DataIndex++)
    if (Slots[DataIndex]->sTag.Len()>0 && Slots[DataIndex]->bRecordIt && !Slots[DataIndex]->bHstCfgd)
      {
      rCDrvSlot Slt = *(Slots[DataIndex]);
      TaggedObject::SplitTagCnv(Slt.sTag(), WrkTag, WrkCnvTxt);
      PkDataUnion PkVal;
      Slt.GetValue(PkVal);
      DataUnion Val;
      Val.Set(PkVal);
      CXM_KeepHistory *HCfg=new CXM_KeepHistory (&Val, DataIndex,
                                              (byte)Slt.iTyp,
                                              Slt.sTag(),
                                              Slt.m_iCnv,
                                              Slt.m_sCnvTxt(),
                                              Slt.sDescription(),
                                              Slt.pHstCfg->dFilterTau,
                                              Slt.pHstCfg->dWinFltPeriod,
                                              Slt.pHstCfg->iWinFltCount,
                                              Slt.pHstCfg->dFltDelta,
                                              Slt.pHstCfg->dDecrease,
                                              Slt.pHstCfg->dDeltaLo,
                                              Slt.pHstCfg->dDeltaHi,
                                              Slt.pHstCfg->iNoRec,
                                              Slt.pHstCfg->iNoNotRec,
                                              Slt.pHstCfg->iFilterAlg,
                                              Slt.pHstCfg->iBoxcarAlg,
                                              (flag)Slt.bRecordIt,
                                              true);

      if (XM.PackMsg(HCfg))
        {
        Slt.bHstCfgd=1;
        }
      else
        break;
      }
  }

//---------------------------------------------------------------------------

void CDriverManager::EO_QueryArchiveCfg(CXMsgLst &XM)
  {
  CXM_ReadIndexedData * pMsg = XM.ReadIndexedData();
  flag ReadAll = pMsg->ReadAll;
  long DataIndex;
  if (pMsg->Start)
    {
    for (long i=0; i<Slots.GetSize(); i++)
      Slots[i]->bArcCfgd=false;
    DataIndex = 0;
    }
  else
    DataIndex = pMsg->LastIndex+1;

  Strng WrkTag, WrkCnvTxt;
  XM.Clear();
  for (   ; DataIndex<Slots.GetSize(); DataIndex++)
    if (Slots[DataIndex]->sTag.Len()>0 && !Slots[DataIndex]->bArcCfgd)
      {
      rCDrvSlot Slt = *(Slots[DataIndex]);
      TaggedObject::SplitTagCnv(Slt.sTag(), WrkTag, WrkCnvTxt);
      PkDataUnion PkVal;
      Slt.GetValue(PkVal);
      DataUnion Val;
      Val.Set(PkVal);
      int aPkd=-1;
      for (int a=0; a<Slt.ArcCfg.GetSize(); a++)
        {
        CXM_KeepArchive *ACfg=new CXM_KeepArchive (&Val, DataIndex,
                                                  (byte)Slt.iTyp,
                                                  Slt.sTag(),
                                                  Slt.m_iCnv,
                                                  Slt.m_sCnvTxt(),
                                                  Slt.ArcCfg[a]->m_sTable(),
                                                  Slt.ArcCfg[a]->m_iFn,
                                                  Slt.sDescription());

        if (XM.PackMsg(ACfg))
          aPkd=a;
        else // full 
          goto Done;
        }
      if (aPkd==Slt.ArcCfg.GetSize())
        Slt.bArcCfgd=1;
      }
Done:;
  }

//---------------------------------------------------------------------------

void CDriverManager::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  CXM_ReadIndexedData * pMsg = (CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll = pMsg->ReadAll;
  long DataIndex = pMsg->Start ? 0 : pMsg->LastIndex - ConIndexStart + 1;
  XM.Clear();
  for (  ; DataIndex<ConnSubsItems.GetSize(); DataIndex++)
    {
    pCConnSubsItem pSubsItem = ConnSubsItems[DataIndex];
    CXM_DataRequest *DRqst=new CXM_DataRequest (ConIndexStart+DataIndex, pSubsItem->m_pCon->m_sTag(), TABOpt_AllInfoOnce, pSubsItem->m_iAction);
    if (!XM.PackMsg(DRqst))
      {
      delete DRqst;
      break;
      }
    }
  }

//---------------------------------------------------------------------------

void CDriverManager::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  while (XM.MsgAvail())
    {
    CXM_DataRequest * Msg = XM.DataRequest();
    int SlotNo = FindChanForTag(Msg->Tag);
    if (SlotNo<0)
      {//tag is not in slot list, check with drivers...
      CDrvCfgHelper DrvCfg;
      for (int i=0; i<Drivers.InUse() && SlotNo<0; i++)
        {
        if (Drivers.At(i)->ReadTaggedItem(Msg->Tag, &DrvCfg))
          SlotNo = AddSlot(&DrvCfg); //allows driver to dynamically add slots
        }
      }
    if (SlotNo>=0)
      {//tag is in slot list...
      Slots[SlotNo]->m_bReqdByExec = 1;
      CXM_DataAvailable *DA=new CXM_DataAvailable (SlotNo, Msg->Tag, Msg->Action);
      XMRet.PackMsg(DA);
      }
    }
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_ReadSubsData(CXMsgLst &XM)
  {
  flag DataRead = 0;
  CXM_ReadIndexedData *pMsg = XM.ReadIndexedData();
  if (pMsg->Start)
    for (int i=0; i<Drivers.InUse(); i++)
      Drivers.At(i)->ReadSubsData();
  long DataIndex = pMsg->Start ? 0 : pMsg->LastIndex+1;
  if (pMsg->ReadAll || bReadAll)
    {//return all driver points...
    if (pMsg->Start)
      for (int i=0; i<Drivers.InUse(); i++)
        Drivers.At(i)->ClearChanges();
    XM.Clear();

    while (DataIndex < Slots.GetSize())
      {
      CDrvSlot& S = *(Slots[DataIndex]);
      if (S.m_bReqdByExec & (!bIgnoresOn || !S.bIgnoreReads))
        {

        if (S.sTag.XStrICmp("ZIL-1157")==0)
          {
          int b=0;
          }

        CXM_ObjectData *Data=new CXM_ObjectData (DataIndex);
        CPkDataItem * pPItem = Data->List.FirstItem();
        CPkDataItem * pPrevItem = pPItem;
        if (S.GetTagValue(Data->List, pPItem, true))
          {
          if (bForcesOn && S.bEnableForces)
            pPrevItem->Value()->PutDouble(S.dForceVal);
          else if (DataIndex>=ConIndexStart)
            {
            ASSERT_ALWAYS(false, "Bad Data Index"); //how can we ever get here???
            }
          //Data->SetSize();
          if (XM.PackMsg(Data))
            {
            DataRead = 1;
            S.ClearChanged();
            }
          else
            break; //message full
          }
        }
      DataIndex++;
      }
    }
  else
    {//return driver points that have changed (if any)...
    XM.Clear();
    if (!EO_SubsRead_Busy)
      {
      EO_SubsRead_Busy = 1;
      BusyDriver = 0;
      if (BusyDriver < Drivers.InUse())
       {
        BusyChanges = Drivers.At(BusyDriver)->NumberOfChanges();
        }
      }
    while (1)
      {
      while (BusyChanges)
        {
        CDrvSlot *pSlt = Drivers.At(BusyDriver)->GetNextChange();
        //ASSERT(pSlt); //Unexpected NULL pSlt, should not get here !
        BusyChanges--;
        if (pSlt)
          {
          if (pSlt->sTag.XStrICmp("06P1102D.A")==0)
            {
            int b=0;
            }
          DataIndex = pSlt->SlotNo;
          if (pSlt->m_bReqdByExec && (pSlt->iAction & XIO_Out) && (!bIgnoresOn || !pSlt->bIgnoreReads))
            {
            CXM_ObjectData *Data=new CXM_ObjectData (DataIndex);
            CPkDataItem * pPItem = Data->List.FirstItem();
            CPkDataItem * pPrevItem = pPItem;
            if (pSlt->GetTagValue(Data->List, pPItem, false))
              {
              if (bForcesOn && pSlt->bEnableForces)
                pPrevItem->Value()->PutDouble(pSlt->dForceVal);
              else if (DataIndex>=ConIndexStart)
                {
                ASSERT_ALWAYS(false, "Bad Data Index"); //how can we ever get here???
                }
              //Data->SetSize();
              if (pSlt->Changed())
                {
                if (XM.PackMsg(Data))
                  {
                  #if dbgDriverMan
                  if (dbgGetValues())
                    dbgpln("Drvr: Get   %-20s      = %g EO_ReadSubsData", pSlt->sTag(), pSlt->GetRawValueDble());
                  #endif
                  DataRead = 1;
                  pSlt->ClearChanged();
                  }
                else
                  {
                  #if dbgDriverMan
                  LogError("DrvMngr", 0, "Change not sent for %s", pSlt->sTag());
                  #endif
                  delete Data;
                  //NOT good, change is not sent !!!
                  break; //message full
                  }
                }
              }
            }
          }
        }
      BusyDriver++;
      if (BusyDriver >= Drivers.InUse())
        {
        EO_SubsRead_Busy = 0;
        break;
        }
      else
        BusyChanges = Drivers.At(BusyDriver)->NumberOfChanges();
      }

    }

  //return all values that have been set...
  LockXO(); //no more must be added or changed while busy!!!
  while (!m_SltChanges.IsEmpty())
    {
    CDrvSltChange * pChg=m_SltChanges.GetHead();

//    ASSERT_ALWAYS(Slots[pChg->DataIndex]==pChg->m_pSlot, "Slots[pChg->DataIndex]!=pChg->pSlot")

    CXM_ObjectData *Data=new CXM_ObjectData (pChg->DataIndex);
    //CPkDataItem * pPItem = Data->List.FirstItem();
    PkDataUnion PData;

    if (bForcesOn && pChg->DataIndex<Slots.GetSize() && Slots[pChg->DataIndex]->bEnableForces)
      {
      CDrvSlot *pSlt=Slots[pChg->DataIndex];
      PData.SetTypeDouble(pChg->iTyp, pSlt->dForceVal, pSlt->m_iCnv, pSlt->m_sCnvTxt());
      #if dbgDriverMan
      if (dbgChgValues())
        dbgpln("Drvr: Force %-20s (%-3s) = %g ", pSlt->sTag(), 
               pSlt->m_sCnvTxt()?pSlt->m_sCnvTxt():"", pSlt->dForceVal);
      #endif
      }
    else
      {
      PData.SetTypeDouble(pChg->iTyp, pChg->dVal, pChg->m_iCnv, pChg->m_sCnvTxt());
      #if dbgDriverMan
      if (dbgChgValues())
        {
        if (pChg->m_pSlot)
          dbgpln("Drvr: ChgSlot   %-20s (%-3s) = %g ", pChg->m_pSlot->sTag(), 
                  pChg->m_sCnvTxt()?pChg->m_sCnvTxt():"", pChg->dVal);
        else if (pChg->m_pCdBlk)
          dbgpln("Drvr: ChgCdBlk  %-20s (%-3s) = %g ", pChg->m_pCdBlk->m_sTag(), 
                  pChg->m_sCnvTxt()?pChg->m_sCnvTxt():"", pChg->dVal);
        else
          dbgpln("Drvr: Chg       [%20i](%-3s) = %g ", 
                  pChg->DataIndex, pChg->m_sCnvTxt()?pChg->m_sCnvTxt():"", pChg->dVal);
        }
      #endif
      }

    Data->List.SetDataValue(/*pPItem,*/ NULL, PData);
    //Data->SetSize();
    if (XM.PackMsg(Data))
      {
      DataRead = 1;
      delete m_SltChanges.RemoveHead();
      }
    else
      break; //message full
    }
  UnlockXO();
  return DataRead;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  while (XM.MsgAvail())
    {
    CXM_ObjectData *pX = XM.ObjectData();
    CPkDataItem *pPItem = pX->FirstItem();
    if (pX->Index<ConIndexStart)
      {
      int SlotNo = pX->Index;
      if (SlotNo>=0 && (!bIgnoresOn || !Slots[SlotNo]->bIgnoreWrites))
        {
        if (!bForcesOn || !Slots[SlotNo]->bEnableForces)
          Slots[SlotNo]->PutTagValue(*(pPItem->Value()));
        else
          {
          PkDataUnion Data(Slots[SlotNo]->dForceVal);
          Slots[SlotNo]->PutTagValue(Data);
          }
        #if dbgDriverMan
        if (dbgPutValues())
          dbgpln("Drvr: Put   %-20s      = %g EO_WriteSubsData", Slots[SlotNo]->sTag(), Slots[SlotNo]->GetRawValueDble());
        #endif
        }
      }
    else
      {
      if (!bIgnoresOn)
        {
        CConnSubsItem *pSubsItem = ConnSubsItems[pX->Index - ConIndexStart];
        CDrvSubsConn *pCon = pSubsItem->m_pCon;

        while (pCon)
          {
          if (!pCon->m_pParentSlot->bIgnoreWrites)
            pCon->PutSubsValue2Slot(this, pPItem, bForcesOn, TimeCB().m_StepSize, bNoiseOn);
          pCon = pCon->m_pNxtSubs;
          }
        }
      }
    }
  return true;
  }

//--------------------------------------------------------------------------

DWORD CDriverManager::EO_ReadTaggedItem(CXM_ObjectTag & ObjTag, CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  DWORD RetCode = 0;
  int SlotNo = FindChanForTag(ObjTag.Tag);
  if (SlotNo<0)
    {//tag not found, check with the driver...
    CDrvCfgHelper DrvCfg;
    for (int i=0; i<Drivers.InUse() && SlotNo<0; i++)
      {
      if (Drivers.At(i)->ReadTaggedItem(ObjTag.Tag, &DrvCfg))
        SlotNo = AddSlot(&DrvCfg); //allows driver to dynamically add slots
      }
    }
  if (SlotNo>=0)
    {//tag found...
    RetCode = (RC_RTI_Exists | RC_RTI_DriverTag);
    if (IsNumData(Slots[SlotNo]->iTyp))
      RetCode |= RC_RTI_NumData;
    else if (IsStrng(Slots[SlotNo]->iTyp))
      RetCode |= RC_RTI_StrData;
    Slots[SlotNo]->m_bReqdByExec = 1;
    ObjData.List.Clear();
    if ((ObjTag.Options & TABOpt_Exists)==0)
      {
      if (!bIgnoresOn || !Slots[SlotNo]->bIgnoreReads)
        {
        CPkDataItem * pPItem = ObjData.List.FirstItem();
        CPkDataItem * pPrevItem = pPItem;
        if (Slots[SlotNo]->GetTagValue(ObjData.List, pPItem, true))
          {
          if (bForcesOn && Slots[SlotNo]->bEnableForces)
            pPrevItem->Value()->PutDouble(Slots[SlotNo]->dForceVal);
          //ObjData.SetSize();
          }
        }
      }
    }

  return RetCode;
  }

//--------------------------------------------------------------------------

int CDriverManager::EO_WriteTaggedItem(CXM_ObjectData &ObjData, CXM_Route &Route)
  {
  CPkDataItem * pItem = ObjData.FirstItem();
  int SlotNo = FindChanForTag(pItem->SymOrTag());
  if (SlotNo>=0 && (!bIgnoresOn || !Slots[SlotNo]->bIgnoreWrites))
    {
    if (bForcesOn && Slots[SlotNo]->bEnableForces)
      {
      double d = Slots[SlotNo]->dForceVal;
      PkDataUnion Data(d);
      if (Slots[SlotNo]->PutTagValue(Data) && Slots[SlotNo]->m_bReqdByExec) //send to driver...
        AddSltChange(SlotNo, d, Slots[SlotNo]); //send to SysCAD EO objects...
      }
    else
      {
      double d = pItem->Value()->GetDouble();
      if (Slots[SlotNo]->PutTagValue(*(pItem->Value())) && Slots[SlotNo]->m_bReqdByExec) //send to driver...
        AddSltChange(SlotNo, d, Slots[SlotNo]); //send to SysCAD EO objects...
      }
    #if dbgDriverMan
    if (dbgPutValues())
      dbgpln("Drvr: Put   %-20s      = %g EO_WriteTaggedItem", Slots[SlotNo]->sTag(), Slots[SlotNo]->GetRawValueDble());
    #endif
    return TOData_OK;
    }
  return TOData_NotFound;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  if (pDrvSheet)
    pDrvSheet->UpdateStats();

  if (CB.m_Time>dCurrentTime)
    {
    if (iRollingRdCntRqd)
      {
      if (iRollingRdCnt==iRollingRdCntRqd)
        {
        //todo:iRollingRdIndex += ??? //check slot XIO to find next input
        //todo:Slots[iRollingRdIndex]->pDrv->RefreshSlotData(Slots[iRollingRdIndex], false, true, false, NULL);
        iRollingRdCnt=0;
        }
      else
        iRollingRdCnt++;
      }
    if (iRollingWrCntRqd)
      {
      if (iRollingWrCnt==iRollingWrCntRqd)
        {
        //todo:iRollingWrIndex += ??? //check slot XIO to find next output
        //todo:Slots[iRollingWrIndex]->pDrv->RefreshSlotData(Slots[iRollingWrIndex], false, false, true, NULL);
        iRollingWrCnt=0;
        }
      else
        iRollingWrCnt++;
      }
    }

  dCurrentTime = CB.m_Time;
  for (int i=0; i<Drivers.InUse(); i++)
    Drivers.At(i)->Execute(CB);
  return false;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_Stop(const CXM_TimeControl &CB)
  {

  if (pDrvSheet)
    pDrvSheet->Stop();

  dCurrentTime = CB.m_Time;
  for (int i=0; i<Drivers.InUse(); i++)
    Drivers.At(i)->Stop();

  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_TagsNotAvail(CXMsgLst &XM)
  {
  while (XM.MsgAvail())
    {
    CXM_TagNotAvail * pX = XM.TagNotAvail();
    LogError("DrvMngr", 0, "Tag not found %s", pX->Tag);
    }
  XM.Clear();
  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_BeginSave(FilingControlBlock &FCB)
  {
  bDoneLocalLoadSave = 0;
  if (bIsOpen)
    {
    for (int i=0; i<Drivers.InUse(); i++)
      {
      Drivers.At(i)->bDoneSave = 0;
      Drivers.At(i)->BeginSave(FCB);
      }
//    if (FCB.SaveAs())
//      FCB.CopyFile(sName());
    }
  iLoadSaveCnt = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_SaveDefinedData(FilingControlBlock &FCB, Strng &Tag, CXMsgLst &XM)
  {
  while (1)
    {
    while (iLoadSaveCnt<Drivers.InUse() && !Drivers.At(iLoadSaveCnt)->HasLoadSave())
      iLoadSaveCnt++;
    if (iLoadSaveCnt>=Drivers.InUse())
      {//finished save data for each driver
      if (bDoneLocalLoadSave)
        return false;
      Tag = "DrvMngrData";
      CXM_ObjectData *ObjData=new CXM_ObjectData (0);//indexed to the first Data Item which is all thats required
      //ObjData->SetSize();
      XM.PackMsg(ObjData);
      return true;
      }
    if (Drivers.At(iLoadSaveCnt)->SaveDefinedData(FCB, Tag, XM))
      return true;
    iLoadSaveCnt++;
    }
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_SaveOtherData(FilingControlBlock &FCB)
  {
  DWORD NWrite;
  char Buff[64]; // CNM Spares
  if (iLoadSaveCnt<Drivers.InUse())
    {
    pCDriver pDrv = Drivers.At(iLoadSaveCnt);
    if (!pDrv->bDoneSave)
      {
      byte Len=pDrv->sTag.Len();
      ASSERT_ALWAYS(Len<' ', "Bad Driver Tag Len");
      //ASSERT(pDrv->sTag.Len()<9);
      strcpy(Buff, pDrv->sTag());
      Buff[Len]=0;
      FCB.WriteFile(&Len, sizeof(Len), &NWrite);
      FCB.WriteFile((LPVOID)Buff, Len, &NWrite);
      pDrv->bDoneSave = 1;
      }
    return pDrv->SaveOtherData(FCB);
    }
  if (bDoneLocalLoadSave)
    return false;
  bDoneLocalLoadSave = 1;
  SaveTags(FCB, true); // CNM false);
  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_BeginLoad(FilingControlBlock &FCB)
  {
  bDoneLocalLoadSave = 0;
  if (bIsOpen)
    {
    for (int i=0; i<Drivers.InUse(); i++)
      Drivers.At(i)->BeginLoad(FCB);
    }
  iLoadSaveCnt = 0;
  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_LoadDefinedData(FilingControlBlock &FCB, CXMsgLst &XM)
  {
  for (long i=0; i<Drivers.InUse(); i++)
    if (Drivers.At(i)->LoadDefinedData(FCB, XM))
      return true;
  //finished loading each driver
  if (bDoneLocalLoadSave)
    return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_LoadOtherData(FilingControlBlock &FCB)
  {
  if (FCB.Eof())
    return false;
  DWORD NRead;
  char Buff[512];
//      Buff[Len]=0;
//      FCB.WriteFile(&Len, sizeof(Len), &NWrite);
//      FCB.WriteFile((LPVOID)Buff, Len, &NWrite);
  // Read One Byte if > ' ' then old style 9 chars
  FCB.ReadFile((LPVOID)Buff, 1, &NRead);
  if (Buff[0]>' ')
    FCB.ReadFile((LPVOID)&Buff[1], 8, &NRead);
  else
    {
    //Buff[0] is Len Byte
    byte Len=Buff[0];
    FCB.ReadFile((LPVOID)Buff, Len, &NRead);
    Buff[Len]=0;
    }
  pCDriver pDrv = Drivers.Find(Buff);
  if (pDrv)
    {
    if (pDrv->LoadOtherData(FCB))
      return true;
    }
  else if (strncmp("TAGS....", Buff, 8)==0)
    {
    bDoneLocalLoadSave = 1;
    if (!FCB.Eof())
      LoadTags(FCB);
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_EndLoad(FilingControlBlock &FCB)
  {
  for (long i=0; i<Slots.GetSize(); i++)
    {
    CDrvSlot& S = *(Slots[i]);
    if (/*S.bLocal && CNM */ S.bUseInitVal && (!bIgnoresOn || !S.bIgnoreWrites))
      {
      PkDataUnion Data(S.dInitVal);
      S.PutTagValue(Data);
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

int CDriverManager::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {//will I allow the tag to be changed...
  /*int SlotNo = FindChanForTag(pOldTag);
  if (SlotNo>=0)
    return EOCT_NOTALLOWED;*/
  if (gs_Exec.Busy())
    {
    //do not allow user to change a solver tag that is refered to by the driver
    //connect column while syscad is running
    const int len = strlen(pOldTag);
    const int j = ConnSubsItems.GetSize();
    for (int i=0; i<j; i++)
      {
      if (strnicmp(pOldTag, ConnSubsItems[i]->m_pCon->m_sTag(), len)==0)
        return EOCT_DRVNOTALLOWED;
      }
    }
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CDriverManager::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {//a tag has been changed, ...
  const int len = strlen(pOldTag);
  const int j = ConnSubsItems.GetSize();
  for (int i=0; i<j; i++)
    {
    pCConnSubsItem pSubsItem = ConnSubsItems[i];
    if (strnicmp(pOldTag, pSubsItem->m_pCon->m_sTag(), len)==0)
      {
      ASSERT(!gs_Exec.Busy());
      LogWarning("DrvMngr", 0, "Changing tag '%s' affects a driver connection tag for '%s'", pOldTag, pSubsItem->m_pCon->m_pParentSlot->sTag());
      }
    }
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CDriverManager::EO_QueryDeleteTag(pchar pDelTag)
  {//will I allow the tag to be deleted...
  /*int SlotNo = FindChanForTag(pOldTag);
  if (SlotNo>=0)
    return EODT_NOTALLOWED;*/
  return EODT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CDriverManager::EO_DeleteTag(pchar pDelTag)
  {//a tag has been deleted, ...
  return EODT_NOTFOUND;
  }

//---------------------------------------------------------------------------

void CDriverManager::EO_OnAppActivate(BOOL bActive)
  {
  }

//---------------------------------------------------------------------------

flag CDriverManager::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  if (bIsOpen)
    {
    switch (Rqst.RQ_Type)
      {
      case RQ_Files:
        if (Rqst.bFirst)
          {
          iLoadSaveCnt = 0;
          Info.sData = sPath;
          Info.sData += sName;
          Info.sData.FnContract();
          Info.bForceCopy = true;
          Info.bPreventCopy = false;
          return true;
          }

        if (iLoadSaveCnt<Drivers.InUse())
          {
          Info.sData = Drivers.At(iLoadSaveCnt)->CfgFile();
          Info.sData.FnContract();
          Info.bForceCopy = true;
          Info.bPreventCopy = false;
          iLoadSaveCnt++;
          return true;
          }
          //TODO ... ask individual drivers what files they use...
          //Drivers.At(iLoadSaveCnt)->GetFilesUsed())
          //Info.sData = ???;
          //iLoadSaveCnt++;
          //return true;
        break;
      case RQ_Tags: //return info on all tags based on request criteria...
      case RQ_TagsConns: //return info on all tags based on request criteria...
        if (Info.Count()<Slots.GetSize() && (Rqst.pGroup==NULL || stricmp(Rqst.pGroup, "Driver")==0))
          {
          Info.sData = Slots[Info.Count()]->sTag();
          return true;
          }
        break;
      case RQ_TagCount:
        if (Rqst.bFirst)
          {
          Info.dwData = Slots.GetSize();
          Info.bDWDataValid = 1;
          return true;
          }
        break;
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void CDriverManager::SaveTags(FilingControlBlock &FCB, BOOL SaveAll)
  {
  DWORD NWrite;
  char Buff[9];
  strcpy(Buff, "TAGS....");
  FCB.WriteFile((LPVOID)Buff, 9, &NWrite);
  for (long i=0; i<Slots.GetSize(); i++)
    {
    double d;
    long len;
    CDrvSlot& S = *(Slots[i]);
    if ((S.bLocal || (SaveAll && (S.iAction & XIO_In))) && (!bIgnoresOn || !S.bIgnoreReads))
      {
      CXM_ObjectData Data(i);
      CPkDataItem * pPItem = Data.List.FirstItem();
      CPkDataItem * pPrevItem = pPItem;
      if (S.GetTagValue(Data.List, pPItem, false))
        {
        if (bForcesOn && S.bEnableForces)
          pPrevItem->Value()->PutDouble(S.dForceVal);
        d = pPrevItem->Value()->GetDouble();
        //Data.SetSize();
        len = S.sTag.Length();
        FCB.WriteFile((LPVOID)&len, sizeof(long), &NWrite);
        FCB.WriteFile((LPVOID)S.sTag(), len+1, &NWrite);
        FCB.WriteFile((LPVOID)&d, sizeof(double), &NWrite);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CDriverManager::LoadTags(FilingControlBlock &FCB)
  {
  DWORD NRead;
  char Buff[512];
  long len;
  double d;
  while (!FCB.Eof())
    {
    FCB.ReadFile((LPVOID)&len, sizeof(long), &NRead);
    FCB.ReadFile((LPVOID)Buff, len+1, &NRead);
    FCB.ReadFile((LPVOID)&d, sizeof(double), &NRead);
    int SlotNo = FindChanForTag(Buff);
    if (SlotNo>=0)
      {//tag found...
      Slots[SlotNo]->bUseInitVal = 1;
      if (Valid(d))
        Slots[SlotNo]->dInitVal = d;
      }
    }
  }

//---------------------------------------------------------------------------

flag CDriverManager::ForceSlot(CDrvSlot& S)
  {
  flag Ok = 0;
  if ((S.iAction & XIO_In) && bForcesOn && S.bEnableForces)
    {
    CXM_ObjectTag  ObjTag(S.sTag(), TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    CXM_Route      Route;
    Ok = (gs_pPrj->XReadTaggedItem(ObjTag, ObjData, Route)!=0);
    if (Ok)
      {
      CPkDataItem * pPItem=ObjData.FirstItem();
      PkDataUnion DU;
      DU.SetTypeDouble(pPItem->Type(), 0.0, S.m_iCnv, S.m_sCnvTxt());
      //DU.PutDouble(0.0); //put anything, it will be replaced with the force value
      CXM_ObjectData OD(0, 0, S.sTag(), DU);
      Ok = (gs_pPrj->XWriteTaggedItem(OD, Route)==TOData_OK);
      //if (Ok)
      //  gs_pExec->m_Seq.NoteManSet(CExecSequence::VSS_DDE, WrkTag(), 0.0, S.sCnvTxt());
      }
    }
  return Ok;
  }

//---------------------------------------------------------------------------

void CDriverManager::RefreshAllSlotData(bool DoInputs, bool DoOutputs)
  {//do reads & writes of ALL slots...
  for (int s=0; s<Slots.GetSize(); s++)
    Slots[s]->pDrv->RefreshSlotData(Slots[s], false, DoInputs, DoOutputs, NULL);
  //Do we also have to refresh all slots with a get in connection column???
  }

//---------------------------------------------------------------------------

void CDriverManager::RefreshConnections()
  {//do required action for all connections for all slots...
  for (int s=0; s<Slots.GetSize(); s++)
    Slots[s]->pDrv->RefreshSlotData(Slots[s], true, true, true, NULL);
  //TODO: now refresh all slots with a get in connection column!
  }

//---------------------------------------------------------------------------

LRESULT CDriverManager::ProcessMessage(WPARAM wParam, LPARAM lParam)
  {
  for (int i=0; i<NoDrivers(); i++)
    {
    pCDriver pDrv = Drivers.At(i);
    LRESULT res = pDrv->ProcessMessage(wParam, lParam);
    }
  return true;
  }

//---------------------------------------------------------------------------

void CDriverManager::Options(char* pGotoTag/*=NULL*/)
  {
  if (pDrvSheet==NULL)
    {
    pDrvSheet = new CDriverSheet("Drivers", AfxGetMainWnd(), pGotoTag==NULL ? 0 : 1);
    if (Drivers.InUse()>0)
      pDrvSheet->AddPage(new CDriverPage());
    if (Slots.GetSize()>0)
      pDrvSheet->AddPage(new CSlotsPage());
    CPropertyPage* Pages[16];
    for (int i=0; i<Drivers.InUse(); i++)
      {
      pCDriver pDrv = Drivers.At(i);
      int Cnt = pDrv->AddPages(Pages);
      for (int j=0; j<Cnt; j++)
        pDrvSheet->AddPage(Pages[j]);
      }
    if (pDrvSheet->GetPageCount()>0)
      {
      //if (!pSheet->Create(AfxGetMainWnd(), WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION, WS_EX_DLGMODALFRAME/* | WS_EX_SMCAPTION*/))
      if (!pDrvSheet->Create(AfxGetMainWnd(),  WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE, WS_EX_DLGMODALFRAME/* | WS_EX_SMCAPTION*/))
        {
        TRACE("Failed to create Drivers PropertySheet\n");
        delete pDrvSheet;
        pDrvSheet=NULL;
        }
      }
    else
      {
      delete pDrvSheet;
      pDrvSheet=NULL;
      }
    }
  if (pDrvSheet && pGotoTag)
    {
    pDrvSheet->DisplayTag(pGotoTag);
    }
  }

//---------------------------------------------------------------------------

void CDriverManager::CloseOptions()
  {
  if (pDrvSheet)
    {
    pDrvSheet->PressButton(PSBTN_CANCEL);
    delete pDrvSheet;
    }
  pDrvSheet=NULL;
  }

//---------------------------------------------------------------------------
//===========================================================================
//
//
//
//===========================================================================
