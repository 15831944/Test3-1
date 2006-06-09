// Slot.cpp: implementation of the XSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysCADMarshal.h"
#include "Slot.h"
#include "slotmngr.h"
#include "opcstuff.h"
#include "tagobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// =======================================================================
//
//
//
// =======================================================================

CSlotCfgStrings::CSlotCfgStrings()
  {
  m_eDoWhat=SlotCfgNull;
  m_lNo=-1;
  };

// -----------------------------------------------------------------------

CSlotCfgStrings::CSlotCfgStrings(const CSlotCfgStrings & V)
  {
  *((CSlotValueSet*)this)=*((CSlotValueSet*)&V);
  m_eDoWhat     = V.m_eDoWhat;
  m_lNo         = V.m_lNo;
  m_sTag        = V.m_sTag;        
  m_sConnects   = V.m_sConnects;   
  m_sType       = V.m_sType;       
  m_sDriver     = V.m_sDriver;     
  m_sModifier   = V.m_sModifier;   
  m_sRange      = V.m_sRange;      
  m_sDesc       = V.m_sDesc;  
  };

// -----------------------------------------------------------------------

CSlotCfgStrings & CSlotCfgStrings::operator =(const CSlotCfgStrings & V)
  {
  *((CSlotValueSet*)this)=*((CSlotValueSet*)&V);
  m_eDoWhat     = V.m_eDoWhat;
  m_lNo         = V.m_lNo;
  m_sTag        = V.m_sTag;        
  m_sConnects   = V.m_sConnects;   
  m_sType       = V.m_sType;       
  m_sDriver     = V.m_sDriver;     
  m_sModifier   = V.m_sModifier;   
  m_sRange      = V.m_sRange;      
  m_sDesc       = V.m_sDesc;       
  return *this;
  };

// -----------------------------------------------------------------------

CSlotCfgStrings::~CSlotCfgStrings()
  {
  };

// =======================================================================
//
//
//
// =======================================================================

CSlotValue::CSlotValue()
  {
  m_lSlot           = -1;
  m_dwTransactionID = 0;
  m_lValueChanges   = 0;
  m_lValuesIgnored  = 0;
  m_lCfgSequence    = -1;//gs_SlotMngr.m_lCfgSequence;
  }
CSlotValue::CSlotValue(const CSlotValue & V) :
  CFullValue(V)
  {
  m_lSlot           = V.m_lSlot;
  m_dwTransactionID = V.m_dwTransactionID;
  m_lValueChanges   = V.m_lValueChanges;
  m_lValuesIgnored  = V.m_lValuesIgnored;
  m_lCfgSequence    = V.m_lCfgSequence;
  m_State           = V.m_State;
  }

CSlotValue & CSlotValue::operator =(const CSlotValue & V)
  {
  *((CFullValue*)this)=*((CFullValue*)&V);
  m_lSlot           = V.m_lSlot;
  m_dwTransactionID = V.m_dwTransactionID;
  m_lValueChanges   = V.m_lValueChanges;
  m_lValuesIgnored  = V.m_lValuesIgnored;
  m_lCfgSequence    = V.m_lCfgSequence;
  m_State           = V.m_State;
  return *this;
  }

void CSlotCfg::SetError(USHORT Error, bool LogIt)
  {
  m_State.m_nError=Error;
  if (Error && LogIt)
    ReportError(m_sTag, 0, "%s", SErr_String(Error));
  };
void CSlotCfg::SetError(USHORT Error, LPCSTR Fmt, ...)
  {
  m_State.m_nError=Error;
  if (Error)
    {
    char    Buff[2048];
    va_list argptr;
    va_start(argptr, Fmt);
    vsprintf(Buff, Fmt, argptr);
    va_end(argptr);

    ReportError(m_sTag, 0, "%s: %s", SErr_String(Error), Buff);
    }
  };
void CSlotCfg::SetError(USHORT Error, HRESULT hr, LPCSTR Fmt, ...)
  {
  m_State.m_nError=Error;
  if (Error)
    {
    char    Buff[2048];
    va_list argptr;
    va_start(argptr, Fmt);
    vsprintf(Buff, Fmt, argptr);
    va_end(argptr);

    ReportError(m_sTag, hr, "%s: %s", SErr_String(Error), Buff);
    }
  };

// =======================================================================
//
//
//
// =======================================================================

CSlotCfg::CSlotCfg()
  {
  m_lSlot       = -1;
  m_wNativeType = VT_NULL;
  m_bRead       = false;       
  m_bWrite      = false;      
  m_bLocal      = false;      
  m_bInUse      = false;  
  m_bInFilter   = true;
  m_lDevice     = -1;
  m_pLink       = NULL;
  m_bConnected  = false;
  m_hServer     = 0;
  }

CSlotCfg::~CSlotCfg()
  {
  }

CSlotCfg::CSlotCfg(const CSlotCfg & V):
  CSlotValue(V),
  CSlotCfgStrings(V)
  {
  m_lSlot       = V.m_lSlot;
  //m_sTag        = V.m_sTag;        
  //m_sDesc       = V.m_sDesc;       
  m_wNativeType = V.m_wNativeType;
  m_bRead       = V.m_bRead;       
  m_bWrite      = V.m_bWrite;      
  m_bLocal      = V.m_bLocal;      
  m_sOPCClient  = V.m_sOPCClient;  
  m_sOPCTag     = V.m_sOPCTag;     
  m_lDevice     = V.m_lDevice; 
  m_pLink       = V.m_pLink;
  m_bInUse      = V.m_bInUse; 
  m_bInFilter   = V.m_bInFilter;
  //m_sConnects   = V.m_sConnects;
  m_bConnected  = V.m_bConnected;
  m_hServer     = V.m_hServer;
  }

CSlotCfg & CSlotCfg::operator =(const CSlotCfg & V)
  {
  *((CSlotValue*)this)=*((CSlotValue*)&V);
  *((CSlotCfgStrings*)this)=*((CSlotCfgStrings*)&V);
  m_lSlot       = V.m_lSlot;
  //m_sTag        = V.m_sTag;        
  //m_sDesc       = V.m_sDesc;       
  m_wNativeType = V.m_wNativeType;
  m_bRead       = V.m_bRead;       
  m_bWrite      = V.m_bWrite;      
  m_bLocal      = V.m_bLocal;      
  m_sOPCClient  = V.m_sOPCClient;  
  m_sOPCTag     = V.m_sOPCTag;     
  m_lDevice     = V.m_lDevice;     
  m_pLink       = V.m_pLink;
  m_bInUse      = V.m_bInUse; 
  m_bInFilter   = V.m_bInFilter;
  //m_sConnects   = V.m_sConnects;
  m_bConnected  = V.m_bConnected;
  m_hServer     = V.m_hServer;
  return *this;
  }

bool CSlotCfg::InFilter(long DevSelect, CSelection &Select)
  { 
  if (DevSelect>=0 && m_lDevice!=DevSelect)
    return false; 
  return Select.InFilter(m_sTag, m_pLink ? (LPCTSTR)m_pLink->m_sSimTag:NULL, m_sOPCTag, NULL, m_State.m_nError!=0, m_State.m_bHold);
  };

// =======================================================================
//
//
//
// =======================================================================

CSlot::CSlot()
  {
  m_Span.m_dSpan        = 1;
  m_Span.m_dOffset      = 0;
  m_Range.m_dMin        = 0;
  m_Range.m_dMax        = 1;
  m_bSltDbgOn           = false;
  m_ChgFlags            = IOMChgFlag_All;
  m_bVisible            = false;
  //m_bInTouchList    = 0;
  m_pGetConnect         = NULL;
  m_dwFlushChangeCalls  = 0;
  m_dwChangesInCall     = 0;
  };

CSlot::~CSlot()
  {
  for (int i=0; i<m_SetConnects.GetSize(); i++)
    delete m_SetConnects[i];
  delete m_pGetConnect;
  };

// --------------------------------------------------------------------------

bool CSlot::ParseType(LPCSTR S)
  {
  char Buff[2048];
  strcpy(Buff, S);
  StripSingleQuoteComment(Buff);

  if (Buff && strlen(Buff)>0)
    {
CSVColArray f;
    int Quote;
    if (Buff[0]=='\"')
      Buff[0]=' ';
    if (Buff[strlen(Buff)-1]=='\"')
      Buff[strlen(Buff)-1]=' ';
    int i = ParseCSVTokens(Buff, f, Quote);
    while (i>0)
      {
      i--;
      //if      (_stricmp(f[i], "r")==0)   Cfg.bRecordIt = 1;
      //if (_stricmp(f[i], "h")==0)   Cfg.bRecordIt = 1;
      if (_stricmp(f[i], "wo")==0)  m_bWrite=true;
      else if (_stricmp(f[i], "ro")==0)  m_bRead=true;
      else if (_stricmp(f[i], "rw")==0)  m_bRead=m_bWrite=true;
      else if (_stricmp(f[i], "l")==0)   m_bLocal = 1;
      else if (_stricmp(f[i], "b")==0)   ChangeType(VT_BOOL);
      else if (_stricmp(f[i], "u8")==0)  ChangeType(VT_UI1);
      else if (_stricmp(f[i], "u16")==0) ChangeType(VT_UI2);
      else if (_stricmp(f[i], "u32")==0) ChangeType(VT_UI4);
      else if (_stricmp(f[i], "i8")==0)  ChangeType(VT_I1);
      else if (_stricmp(f[i], "i16")==0) ChangeType(VT_I2);
      else if (_stricmp(f[i], "i32")==0) ChangeType(VT_I4);
      else if (_stricmp(f[i], "f32")==0) ChangeType(VT_R4);
      else if (_stricmp(f[i], "f64")==0) ChangeType(VT_R8);
      //else if (_stricmp(f[i], "f")==0)   Cfg.bEnableForces = 1;
      //else if (_stricmp(f[i], "iw")==0)  Cfg.bIgnoreWrites = 1;
      //else if (_stricmp(f[i], "ir")==0)  Cfg.bIgnoreReads = 1;
      //else if (_stricmp(f[i], "irw")==0)
      //  {
      //  Cfg.bIgnoreWrites = 1;
      //  Cfg.bIgnoreReads = 1;
      //  }
      else if (_strnicmp(f[i], "l=", 2)==0)
        {
        m_bLocal = 1;
        if (f[i][2])
          {
          //Cfg.bUseInitVal = 1;
          //Cfg.dInitVal = SafeAtoF(&(f[i][2]));
          }
        }
      else if (_strnicmp(f[i], "f=", 2)==0)
        {
        //Cfg.bEnableForces = 1;
        //if (f[i][2])
        //  Cfg.dForceVal = SafeAtoF(&(f[i][2]));
        }
      else if (_stricmp(f[i], "?")==0)
        {        
        SetError(SErr_SlotExcluded, false);
        return false;
        }
      else if (_stricmp(f[i], "!")==0)
        m_bSltDbgOn=1;
      else
        {
        SetError(SErr_Unknown, "Unknown '%s'", f[i]);
        return false;
        }
      }
    }
  else
    {
    // Dont Load
    return false;//!(m_bError=true);
    }

  //if (Cfg.iAction==XIO_Null)
  //  {
  //  SetError(1, "Valid IO type (ro,wo or rw) must be specified");
  //  return false;
  //  }
  //if (Cfg.iTyp==tt_NULL)
  //  {
  //  SetError(1, "Valid data type must be specified");
  //  return false;
  //  }
  //if (Cfg.bLocal && Cfg.iAction!=XIO_In)
  //  {
  //  Cfg.iAction = XIO_In;
  //  LogWarning("DrvMngr", 0, "Line %u: '%s' IO type changed to wo for a 'local'");
  //  }
  //if (!(Cfg.iAction & XIO_In))
  //  Cfg.bIgnoreWrites = 0;
  //if (!(Cfg.iAction & XIO_Out))
  //  Cfg.bIgnoreReads = 0;
  return true;
  }

// --------------------------------------------------------------------------

bool CSlot::ParseDriver(LPCSTR S)
  {
  char Buff[2048];
  strcpy(Buff, S);
  StripSingleQuoteComment(Buff);

  m_sOPCClient="";
  m_sOPCTag="";

  CSVColArray f;
  int Quote,nParms;

  if (strlen(Buff)==0 && 1)//gs_pPrj->bDrvLclTagSrvrOK)
    sprintf(Buff, "OPC(Local,%s)", m_sTag);

  //Cfg.sDrvCfg = pDrive;
  nParms = ParseCSVFunction(Buff, f, Quote) - 1;
  if (nParms>=2)
    {
    m_sOPCClient=f[1];
    m_sOPCTag=f[2];
    }
  else
    { // Assume OPC( is missing
    strcpy(Buff, S);
    StripSingleQuoteComment(Buff);
    nParms = ParseCSVTokens(Buff, f, Quote);
    if (nParms>=2)
      {
      m_sOPCClient=f[0];
      m_sOPCTag=f[1];
      }
    }

  if (m_sOPCTag.GetLength()==0)
    {
    SetError(SErr_MissingOPCTag, "Missing OPC Tag '%s'", m_sOPCTag);
    return false;
    }

  if (m_sOPCClient.CompareNoCase("Local")==0)
    {
    m_bLocal=true;
    m_sOPCTag="";
    }

  //if (Cfg.sDrvCfg.Length()==0 || Quote)
  //  {
  //  SetError(1, "Valid driver expected");
  //  return false;
  //  }
  //pCDriver pDrv = Drivers.Add(sPath(), f[0]);
  return true;
  }

// --------------------------------------------------------------------------

bool CSlot::ParseRange(LPCSTR S)
  {
  char Buff[2048];
  strcpy(Buff, S);
  StripSingleQuoteComment(Buff);

  //get output ranges...
  if (Type()==VT_BOOL)
    {
    m_Range.m_dMin = 0.0;
    m_Range.m_dMax = 1.0;
    }
  else
    {
CSVColArray f;
    int Quote,nParms;
    nParms = ParseCSVFunction(Buff, f, Quote) - 1;
    if ((nParms==0) || (strlen(Buff)==0))
      m_Range.m_bValid = false;
    else if (Quote)
      {
      SetError(SErr_Range, "Range expected.");
      return false;
      }
    else if ((_stricmp(f[0], "none")==0) || (strlen(f[0])==0))
      m_Range.m_bValid = false;
    else if (nParms!=2 || _stricmp(f[0], "range")!=0)
      {
      SetError(SErr_Range, "Ranges(Min,Max) expected.");
      return false;
      }
    else
      {
      m_Range.m_dMin=SafeAtoF(f[1]);
      m_Range.m_dMax=SafeAtoF(f[2]);
      if (fabs(m_Range.m_dMax-m_Range.m_dMin)<1.0e-6)
        {
        SetError(SErr_Range, "Ranges(Min,Max) expected. Max must not equal Min.");
        return false;
        }
      m_Range.m_bValid=true;
      }
    }

  return true;
  }

// --------------------------------------------------------------------------

bool CSlot::ParseModifier(LPCSTR S)
  {
  char Mods[2048];
  strcpy(Mods, S);
  StripSingleQuoteComment(Mods);

  if (strlen(Mods)>0)
    {
    LPSTR ff[9];
    ff[0]=&Mods[0];
    for (int i=1; i<9; i++)
      ff[i]=NULL;
    i=1;
    int Pos=0;
    while (strstr(&Mods[Pos],"),"))
      {
      if (i>=3)
        {
        SetError(SErr_Modifier, "Too many Modifier functions found");
        return false;
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
        SetError(SErr_Range, "Valid modifier expected");
        return false;
        }
      else if (_stricmp(f[0],"span")==0)
        {
        if (nParms!=2)
          {
          SetError(SErr_Span, "Valid modifier expected Span(span,offset).");
          return false;
          }
        if (Type()==VT_BOOL)
          {
          SetError(SErr_Span, "Span invalid for boolean slot");
          return NULL;
          }
        m_Span.m_dSpan = SafeAtoF(f[1]);
        m_Span.m_dOffset = SafeAtoF(f[2]);
        if (fabs(m_Span.m_dSpan)<1.0e-20)
          {
          SetError(SErr_Span, "Valid modifier expected Span(span,offset). Span must be > 1e-20");
          return false;
          }
        m_Span.m_bValid=true;
        }
      else if (_stricmp(f[0],"clamp")==0)
        {
        if (nParms!=2)
          {
          SetError(SErr_Clamp, "Valid modifier expected Clamp(Lo,Hi).");
          return false;
          }
        if (!m_Span.m_bValid)
          {
          SetError(SErr_Clamp, "Valid modifier expected Clamp used with no Span.");
          return false;
          }
        m_Clamp.m_dLo = SafeAtoF(f[1]);
        m_Clamp.m_dHi = SafeAtoF(f[2]);
        if (m_Clamp.m_dLo>m_Clamp.m_dHi)
          {
          SetError(SErr_Clamp, "Valid modifier expected Clamp(Lo,Hi). Lo <= Hi");
          return false;
          }
        m_Clamp.m_bValid=true;
        }
      //else if (_stricmp(f[0],"bit")==0) // Single Bit in Word numbering 1..16
      //  {
      //  if (nParms!=1)
      //    {
      //    SetError(1, "Valid modifier expected Bit(No).");
      //    return false;
      //    }
      //  int bitbase, wordwidth;
      //  Cfg.pDrv->BitBase(bitbase, wordwidth);
      //
      //  int bit = atoi(f[1]);
      //  if (bit>=bitbase && bit <= bitbase+wordwidth-1)
      //    {
      //    Cfg.nS.Offset = bit-bitbase;  // shift
      //    Cfg.nS.Span = (long)((DWORD)1 << (DWORD)Cfg.nS.Offset); // mask
      //    }
      //  else
      //    {
      //    SetError(1, "Valid bit no expected Bit(No). No must be 1 to 16.");
      //    return false;
      //    }
      //  }
      //else if (_stricmp(f[0],"mask")==0) // Mask multiple bits and shift
      //  {
      //  if (nParms!=1)
      //    {
      //    SetError(1, "Valid modifier expected Mask(HexNo).");
      //    return false;
      //    }
      //  Cfg.nS.Span = SafeAtoL(f[1]);
      //  if (Cfg.nS.Span<0)
      //    {
      //    SetError(1, "Valid modifier expected Mask(HexNo).");
      //    return false;
      //    }
      //  if (Cfg.nS.Span)
      //    {
      //    int x = Cfg.nS.Span;
      //    while (1)
      //      {
      //      if (x & 0x1) break;
      //      Cfg.nS.Offset++;
      //      x = x >> 1;
      //      }
      //    }
      //  }
      else if (_stricmp(f[0],"none")==0)
        m_Span.m_bValid = false;
      else if (strlen(f[0])>0)
        {
        SetError(SErr_Modifier, "Valid modifier expected : '%s'", f[0]);
        return false;
        }
      }
    }

  return true;
  }

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

bool CSlot::ParseConnOperator(CSlotConnect *pC, LPSTR pOp)
  {
  CSVColArray f;
  int Quote,nParms;

  //dbgpln("      ConnectOp:%s", pOp);
  bool IsFloatDataVT = Type()==VT_R4 || Type()==VT_R8;

  nParms = ParseCSVFunction(pOp, f, Quote) - 1;
  if (Quote)
    {
    SetError(SErr_ConnectionOp, "Valid Connection Operator expected");
    return false;
    }
  if (_stricmp(pOp,"Filter")==0)
    {
    if (nParms<1)// CNM Allow filter of integral types || !IsFloatDataVT)
      {
      if (nParms<1)
        SetError(SErr_FilterOp, "Valid Connection Operator expected: Filter(Type,[Parm1])");
      if (IsFloatDataVT)
        SetError(SErr_FilterOp, "Valid Connection Operator expected: Filter can only be used for a float slot");
      return false;
      }
    else
      {
      pC->AddOp(new CSlotConnOp_Filter((byte)SafeAtoL(f[1],1), SafeAtoF(f[2],0.0)));
      }
    }
  else if (_stricmp(pOp,"Noise")==0)
    {
    if (nParms<3 || !IsFloatDataVT)
      {
      if (nParms<3)
        SetError(SErr_NoiseOp, "Valid Connection Operator expected: Noise(Relative, Type=0,StdDev=1,[Mean=0])");
      if (IsFloatDataVT)
        SetError(SErr_NoiseOp, "Valid Connection Operator expected: Noise can only be used for a float slot");
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

      pC->AddOp(new CSlotConnOp_Noise(NoiseTypes(SafeAtoL(f[2],0)), SafeAtoL(f[1],0)!=0, SafeAtoF(f[3], 1.0), SafeAtoF(f[4], 0.0)));
      }
    }
  else if (_stricmp(pOp,"Oscillator")==0)
    {
    if (nParms<3 || !IsFloatDataVT)
      {
      if (nParms<3)
        SetError(SErr_OscillatorOp, "Valid Connection Operator expected: Oscillator(Period, Relative, Type=0 [, Lo=0.0, [Hi=0.1]]) {Type 0=Square,1=Ramp,2=SawTooth}");
      //if (IsFloatDataVT)
      //  SetError(SErr_OscillatorOp, "Valid Connection Operator expected: Oscillator can only be used for a float slot");
      return false;
      }
    else
      {
      pC->AddOp(new CSlotConnOp_Oscillator(Range(0L,SafeAtoL(f[1],0),2L),
                                           (float)Range(0.05,SafeAtoF(f[2],1),1000.0), 
                                           SafeAtoL(f[3],0)!=0, 
                                           (float)SafeAtoF(f[4], 0.0), 
                                           (float)SafeAtoF(f[5], 0.1)));
      }
    }
  else if (_stricmp(pOp,"Not")==0 || _stricmp(pOp,"Invert")==0 || _stricmp(pOp,"Inv")==0)
    {
    if (nParms!=0)
      {
      SetError(SErr_InvertOp, "Valid Connection Operator expected: Not() or Invert()");
      return false;
      }
    else
      {
      pC->AddOp(new CSlotConnOp_Not());
      }
    }
  else if (_stricmp(pOp,"Profile")==0)
    {
    if (nParms<1 || nParms>2)
      {
      SetError(SErr_ProfileOp, "Valid Connection Operator  expected: Profile(FileName, [Reverse])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid Connection Operator  expected: Slot must have write access");
    //  return NULL;
    //  }

    if (f[1]==NULL || strlen(f[1])==0)
      {
      SetError(SErr_InvalidFilename, "Invalid Filename", f[1] ? f[1] : "");
      return false;
      }
    
    // incomplete 
    SetError(SErr_ProfNotDone);
    return false;

    CSlotConnPrf * pProfile=new CSlotConnPrf;
    CString Fn=gs_SlotMngr.m_Cfg.m_sCfgFile;
    Fn+=f[1];
    long err=pProfile->Parse(Fn);
    if (err==0)
      {
      pC->AddOp(new CSlotConnOp_Profile(SafeAtoL(f[2],0)!=0, pProfile));
      }
    else
      {
      SetError(SErr_ProfileOp, "Valid Connection Operator expected: Parse error %s", pProfile->ErrorString(err));
      delete pProfile;
      }
    }
  else if (_stricmp(pOp,"Scale")==0 || _stricmp(pOp,"Scl")==0 || _stricmp(pOp,"Tweak")==0)
    {
    if (nParms<1 || nParms>2)
      {
      SetError(SErr_ScaleOp, "Valid Connection Operator expected: Scale(Mult, Off)");
      return false;
      }
    else
      {
      pC->AddOp(new CSlotConnOp_Scale(SafeAtoF(f[1],1.0), SafeAtoF(f[2],0.0)));
      }
    }
  else if (_stricmp(pOp,"Sqr")==0)
    {
    if (nParms<1 || nParms>5)
      {
      SetError(SErr_SqrOp, "Valid Connection Operator expected: Sqr(mult, [minI], [maxI], [minO], [maxO])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid Connection Operator  expected: Slot must have write access");
    //  return NULL;
    //  }

    double dMult=SafeAtoF(f[1],1.0);
    double dMin1=SafeAtoF(f[2],0.0);
    double dMax1=SafeAtoF(f[3],0.0);
    double dMin2=SafeAtoF(f[4],dMin1);
    double dMax2=SafeAtoF(f[5],dMax1);
    pC->AddOp(new CSlotConnOp_Sqr(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (_stricmp(pOp,"Sqrt")==0)
    {
    if (nParms<1 || nParms>5)
      {
      SetError(SErr_SqrtOp, "Valid Connection Operator  expected: Sqrt(mult, [minI], [maxI], [minO], [maxO])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid Connection Operator  expected: Slot must have write access");
    //  return NULL;
    //  }

    double dMult=SafeAtoF(f[1],1.0);
    double dMin1=SafeAtoF(f[2],0.0);
    double dMax1=SafeAtoF(f[3],0.0);
    double dMin2=SafeAtoF(f[4],dMin1);
    double dMax2=SafeAtoF(f[5],dMax1);
    pC->AddOp(new CSlotConnOp_Sqrt(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (_stricmp(pOp,"Range")==0 || _stricmp(pOp,"Rng")==0)
    {
    if (nParms<1 || nParms>2)
      {
      SetError(SErr_RangeOp, "Valid Connection Operator  expected: Range([min], [max])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid Connection Operator  expected: Slot must have write access");
    //  return NULL;
    //  }
    double dMin=SafeAtoF(f[1],-1.0e100);
    double dMax=SafeAtoF(f[2],1.0e100);
    pC->AddOp(new CSlotConnOp_Range(dMin, dMax));
    }
  else if (_stricmp(pOp,"Cmp")==0)
    {
    if (nParms!=2 || IsFloatDataVT)
    // cnm if (nParms!=3 || Cfg.iTyp!=tt_Bool)
      {
      if (nParms!=2)
        SetError(SErr_CmpOp, "Valid connection expected: Cmp(</<=/==/>=/>,value)");
      // cnm
      if (IsFloatDataVT)
        SetError(SErr_CmpOp, "Valid connection expected: Cmp(</<=/==/>=/>,value) can only be used for a Non Float slot");
      return NULL;
      }
    int Op=100;
    if (_stricmp(f[1], "<")==0)
      Op=-2;
    else if (_stricmp(f[1], "<=")==0)
      Op=-1;
    else if (_stricmp(f[1], "==")==0)
      Op=0;
    else if (_stricmp(f[1], ">=")==0)
      Op=1;
    else if (_stricmp(f[1], ">")==0)
      Op=2;
    else
      {
      SetError(SErr_CmpOp, "Valid connection expected: Comparison operators '<,<=,==,>=,>' expected as 2nd parameter.");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }
    pC->AddOp(new CSlotConnOp_Cmp(Op, SafeAtoF(f[2], 0.0)));
    }
  else
    SetError(SErr_ConnectionOp, "Unknown connection '%s'", f[0]?f[0]:"");


  return true;
  };

// --------------------------------------------------------------------------

CSlotConnect * CSlot::ParseConnFunction(LPSTR pConn)
  {
  CSlotConnect *TheConn=NULL;

  CSVColArray f;
  int Quote,nParms;

  //dbgpln("    ConnectFn:%s", pConn);
  bool IsFloatDataVT = Type()==VT_R4 || Type()==VT_R8;

  nParms = ParseCSVFunction(pConn, f, Quote) - 1;
  if (Quote)
    {
    SetError(SErr_ConnectionFn, "Valid connection expected");
    return NULL;
    }
  if (_stricmp(f[0],"get")==0/* || _stricmp(f[0],"getinv")==0*/)
    {
    if (nParms!=1)
      {
      SetError(SErr_GetFn, "Valid connection expected: get(tag)");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }
    TheConn=AddGetConnect(f[1], _stricmp(f[0],"getinv")==0);//, CCS_Null, 0);
    }
  else if (_stricmp(f[0],"set")==0 || _stricmp(f[0],"setinv")==0)
    {
    if (nParms<1 || nParms>3)
      {
      SetError(SErr_SetFn, "Valid connection expected: Incorrect number of parameters for set(...) or setinv(...)");
      return NULL;
      }
    //if (nParms==3 && Cfg.iTyp!=tt_Bool)
    //  {
    //  SetError(1, "Valid connection expected: can only use two delay times for a bit slot");
    //  return NULL;
    //  }
    //if (!(Cfg.iAction & XIO_Out))
    //  SetError(1, "Valid connection expected: Slot must have read access");
    DWORD DelayTime1 = 0;
    if (f[2]==NULL || strlen(f[2])==0)
      DelayTime1 = 0;
    else if (_strnicmp(f[2], "inf", 3)==0)
      DelayTime1 = InfiniteDelay;
    else
      DelayTime1 = SafeAtoL(f[2]);
    DWORD DelayTime2 = 0;
    bool UseDelay2 = 0;
    if (nParms==3)
      {
      UseDelay2 = 1;
      if (_strnicmp(f[3], "inf", 3)==0)
        DelayTime2 = InfiniteDelay;
      else
        DelayTime2 = SafeAtoL(f[3]);
      }
    TheConn=AddSetConnect(f[1], _stricmp(f[0],"setinv")==0, DelayTime1, DelayTime2, UseDelay2);
    }
  else if (_stricmp(f[0],"getcmp")==0)
    {
    if (nParms!=3 || IsFloatDataVT)
    // cnm if (nParms!=3 || Cfg.iTyp!=tt_Bool)
      {
      if (nParms!=3)
        SetError(SErr_GetCmpFn, "Valid connection expected: getcmp(tag,</<=/==/>=/>,value)");
      // cnm
      if (IsFloatDataVT)
        SetError(SErr_GetCmpFn, "Valid connection expected: getcmp(tag,</<=/==/>=/>,value) can only be used for a Non Float slot");
      return NULL;
      }
    int Op=100;
    if (_stricmp(f[2], "<")==0)
      Op=-2;
    else if (_stricmp(f[2], "<=")==0)
      Op=-1;
    else if (_stricmp(f[2], "==")==0)
      Op=0;
    else if (_stricmp(f[2], ">=")==0)
      Op=1;
    else if (_stricmp(f[2], ">")==0)
      Op=2;
    if (Op==100)
      {
      SetError(SErr_GetCmpFn, "Valid connection expected: Comparison operator '<', '<=', '==', '>=' or '>' expected as 2nd parameter.");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }

    TheConn=AddGetConnect(f[1], false);
    if (TheConn)
      TheConn->AddOp(new CSlotConnOp_Cmp(Op, SafeAtoF(f[3], 0.0)));
    }
  else if (_stricmp(f[0],"getabs")==0)
    {
    if (nParms!=1 || !IsFloatDataVT)
      {
      if (nParms!=1)
        SetError(SErr_GetAbsFn, "Valid connection expected: getabs(tag)");
      if (IsFloatDataVT)
        SetError(SErr_GetAbsFn, "Valid connection expected: getabs(tag) can only be used for a float slot");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }
    TheConn=AddGetConnect(f[1], false);//, CCS_Abs, 0);
    if (TheConn)
      TheConn->AddOp(new CSlotConnOp_Abs);
    }
  else if (_stricmp(f[0],"getrev")==0)
    {
    if (nParms!=2 || !IsFloatDataVT)
      {
      if (nParms!=2)
        SetError(SErr_GetRevFn, "Valid connection expected: getrev(tag, value)");
      if (!IsFloatDataVT)
        SetError(SErr_GetRevFn, "Valid connection expected: getrev(tag, value) can only be used for a float slot");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }

    TheConn=AddGetConnect(f[1], false);//, CCS_Rev, SafeAtoF(f[2]));
    if (TheConn)
      TheConn->AddOp(new CSlotConnOp_Rev(SafeAtoF(f[2], 0.0)));
    }
  else if (_stricmp(f[0],"getsqr")==0)
    {
    if (nParms<2 || nParms>6)
      {
      SetError(SErr_GetSqrFn, "Valid connection expected: getsqr(tag, value, [minI], [maxI], [minO], [maxO])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }

    TheConn=AddGetConnect(f[1], false);//, CCS_Sqr, SafeAtoF(f[2],1.0), Min1, Max1, Min2, Max2);
    double dMult=SafeAtoF(f[2],1.0);
    double dMin1=SafeAtoF(f[3],0.0);
    double dMax1=SafeAtoF(f[4],0.0);
    double dMin2=SafeAtoF(f[5],dMin1);
    double dMax2=SafeAtoF(f[6],dMax1);
    if (TheConn)
      TheConn->AddOp(new CSlotConnOp_Sqr(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (_stricmp(f[0],"getsqrt")==0)
    {
    if (nParms<2 || nParms>6)
      {
      SetError(SErr_GetSqrtFn, "Valid connection expected: getsqrt(tag, value, [minI], [maxI], [minO], [maxO])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }

    TheConn=AddGetConnect(f[1], false);//, CCS_Sqrt, SafeAtoF(f[2],1.0), Min1, Max1, Min2, Max2);
    double dMult=SafeAtoF(f[2],1.0);
    double dMin1=SafeAtoF(f[3],0.0);
    double dMax1=SafeAtoF(f[4],0.0);
    double dMin2=SafeAtoF(f[5],dMin1);
    double dMax2=SafeAtoF(f[6],dMax1);
    if (TheConn)
      TheConn->AddOp(new CSlotConnOp_Sqrt(dMult, dMin1, dMax1, dMin2, dMax2));
    }
  else if (_stricmp(f[0],"getprofile")==0)
    {
    if (nParms<2 || nParms>3)
      {
      SetError(SErr_GetProfileFn, "Valid connection expected: getprofile(Tag, FileName, [Reverse])");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_In))
    //  {
    //  SetError(1, "Valid connection expected: Slot must have write access");
    //  return NULL;
    //  }

    if (f[2]==NULL || strlen(f[2])==0)
      SetError(1, "Invalid Filename '%s'", f[2] ? f[2] : "");
    CSlotConnPrf * pProfile=new CSlotConnPrf;
    CString Fn=gs_SlotMngr.m_Cfg.m_sCfgFile;
    Fn+=f[2];
    long err=pProfile->Parse(Fn);
    if (err==0)
      {
      TheConn=AddGetConnect(f[1], false);//, CCS_Profile, 1.0);//, pProfile);
      if (TheConn)
        TheConn->AddOp(new CSlotConnOp_Profile(SafeAtoL(f[3],0)!=0, pProfile));
      }
    else
      {
      SetError(SErr_GetProfileFn, "Valid connection expected: Parse error %s", pProfile->ErrorString(err));
      delete pProfile;
      }
    }
  else if (_stricmp(f[0],"setrev")==0)
    {
    if (nParms!=3)
      {
      SetError(SErr_SetRevFn, "Valid connection expected: Incorrect number of parameters for setrev(tag,val,delay)");
      return NULL;
      }
    if (!IsFloatDataVT)
      {
      SetError(SErr_SetRevFn, "Valid connection expected: can only use setrev for float slot");
      return NULL;
      }
    //if (!(Cfg.iAction & XIO_Out))
    //  SetError(1, "Valid connection expected: Slot must have read access");

    DWORD DelayTime1 = 0;
    if (_strnicmp(f[3], "inf", 3)==0)
      DelayTime1 = InfiniteDelay;
    else
      DelayTime1 = SafeAtoL(f[3]);
    DWORD DelayTime2 = 0;
    bool UseDelay2 = 0;
    TheConn=AddSetConnect(f[1], false, DelayTime1, DelayTime2, UseDelay2);//, true, SafeAtoF(f[2]));
    }
  else if (_stricmp(f[0],"none")==0)
    {
    }
  else if (strlen(f[0])>0)
    {
    //bool Input=false;
    //for (int i=0; RmtIO[i].m_Name; i++)
    //  {
    //  if (_stricmp(f[0], RmtIO[i].m_Name)==0)
    //    {
    //    Input=(RmtIO[i].m_Type==CRIO_In);
    //    break;
    //    }
    //  }
    //if (RmtIO[i].m_Name)
    //  {
    //  CString TheTag=Cfg.sTag;
    //  if (nParms>=1 && f[1] && strlen(f[1])>0)
    //    TheTag=f[1];
    //
    //  if (gs_pTheSFELib>=0)
    //    gs_pTheSFELib->FE_IOPnt_Add((LPCTSTR)TheTag(), Input, (LPCTSTR)Cfg.sDesc(), Cfg.iTyp, f[2], NULL);
    //   // Now push
    //  // Must also get/set it
    //  CString FullTag("$IO.");
    //  FullTag+=TheTag;
    //  if (Input)
    //    TheConn=AddGetConnect(FullTag(), false);
    //  else
    //    AddSetConnect(FullTag(), false, 0, 0, false);
    //  }
    //else
    //  {
    //  SetError(1, "Valid connection expected");
    //  return NULL;
    //  }
    }
  else
    {
    SetError(SErr_ConnectionFn, "Valid connection expected");
    return NULL;
    }
  return TheConn;
  }

// --------------------------------------------------------------------------

CSlotConnect * CSlot::AddGetConnect(LPSTR pTag, bool Inv)//, byte Op, double LclConst)
  {
  if (pTag && strlen(pTag)>0)
    {
    CSlotConnect* pNew = new CSlotConnect(pTag, false, Inv);
    if (m_pGetConnect)
      SetError(SErr_TooManyConnects, "Only One Get COnnection allowed");
    else
      m_pGetConnect=pNew;
    return pNew;
    }
  return NULL;
  }

// --------------------------------------------------------------------------

CSlotConnect * CSlot::AddSetConnect(LPSTR pTag, bool Inv, DWORD DelayTime1, DWORD DelayTime2, bool UseDelay2)//, byte Op, double LclConst)
  {
  if (pTag && strlen(pTag)>0)
    {
    CSlotConnect* pNew = new CSlotConnect(pTag, true, Inv);
    pNew->SetDelayTimes(DelayTime1, DelayTime2, UseDelay2, Inv);
    m_SetConnects.Add(pNew);
    return pNew;
    //  }
    }
  return NULL;
  }

// --------------------------------------------------------------------------

bool CSlot::ParseConnects(LPCSTR S)
  {
  char Conns[2048];
  strcpy(Conns, S);
  StripSingleQuoteComment(Conns);

  if (strlen(Conns)>0)
    {
    CompactSepars(Conns);
    LPSTR ff[9];
    ff[0]=&Conns[0];
    for (int i=1; i<9; i++)
      ff[i]=NULL;
    i=1;
    int Pos=0;
    int nBraceCnt=0;
    int nParenCnt=0;
    int Len=strlen(Conns);
    for (Pos=0; Pos<Len; Pos++)
    //while (strstr(&Conns[Pos],"),"))
      {
      if      (Conns[Pos]=='(') nBraceCnt++; 
      else if (Conns[Pos]==')') nBraceCnt--; 
      else if (Conns[Pos]=='\"') nParenCnt=(nParenCnt+1)%2; 
      else if (Conns[Pos]=='\'') nParenCnt=(nParenCnt+1)%2; 
      else if (Conns[Pos]==',' && nBraceCnt==0 && nParenCnt==0)
        {
        if (i>=9)
          {
          SetError(SErr_TooManyConnectFns, "Too many connection functions found");
          return false;
          }
      //Pos = (strstr(&Conns[Pos],"),") - Conns) + 1;
        Conns[Pos]=0;
        ff[i++]=&Conns[Pos+1];
        }
      }
    while (i>0)
      {
      i--;
      char * pOp=strstr(ff[i], "):");
      if (pOp)
        {
        *(pOp+1)=0;
        pOp+=2;
        }
      CSlotConnect *pC=ParseConnFunction(ff[i]);

      while (pC && pOp)
        {
        char * pNxtOp=strstr(pOp, "):");
        if (pNxtOp)
          {
          *(pNxtOp+1)=0;
          pNxtOp+=2;
          }
        if (!ParseConnOperator(pC, pOp))
          break;
        pOp=pNxtOp;
        }
      }
    }

  return true;
  }

// -----------------------------------------------------------------------

bool CSlot::Parse(CSlotCfgStrings * pAdd)
  {
  (*(CSlotCfgStrings*)this)=*pAdd;
  //m_sTag=pAdd->m_sTag;
  //m_sDesc=pAdd->m_sDesc;
  //m_sConnects=pAdd->m_sConnects;
  m_lDevice=0;
  m_pLink=NULL;
  ChangeType(VT_BOOL);
  m_bRead=false;
  m_bWrite=false;
  m_bLocal=false;
  m_State.m_nError=0;
  if (!ParseType(pAdd->m_sType))
    goto Err;
  if (!ParseDriver(pAdd->m_sDriver))
    goto Err;
  if (!ParseRange(pAdd->m_sRange))
    goto Err;
  if (!ParseModifier(pAdd->m_sModifier))
    goto Err;
  if (!ParseConnects(pAdd->m_sConnects))
    goto Err;

  return true;
Err:
  ASSERT(m_State.m_nError);////m_bError=true;
  m_bRead=m_bWrite=false;
  return false;
  };

// -----------------------------------------------------------------------

bool CSlot::MustReConnect(CSlot * Other)
  {
  return (m_bRead!=Other->m_bRead ||
          m_bWrite!=Other->m_bWrite ||
          Type()!=Other->Type()||
          m_sOPCTag.CompareNoCase(Other->m_sOPCTag)!=0 ||
          m_sOPCClient.CompareNoCase(Other->m_sOPCClient)!=0 ||
          m_sConnects.CompareNoCase(Other->m_sConnects)!=0);
  };

// -----------------------------------------------------------------------

bool CSlot::TransferConfiguration(CSlot * Other)
  {
  (*(CSlotCfgStrings*)this)=*Other;
  m_Span    =Other->m_Span;
  m_Range   =Other->m_Range;
  m_Clamp   =Other->m_Clamp;
  return true;
  };

// -----------------------------------------------------------------------

void CSlot::ApplySpanInComing(VARTYPE SlotType, COleVariant &V)
  {
  // Apply Span COMING into Slot
  if (m_Span.m_bValid)
    {
    // Apply Span before going to Device
    if (IsFloatDataVT(V.vt))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_R8);
      if (FAILED(hr))
        SetError(SErr_ApplySpanInComing, hr, "");
      V.dblVal=(V.dblVal-m_Span.m_dOffset)/m_Span.m_dSpan;
      }
    else if (IsSignedDataVT(V.vt))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_I4);
      if (FAILED(hr))
        SetError(SErr_ApplySpanInComing, hr, "");
      V.lVal=(V.lVal-(long)m_Span.m_dOffset)/(long)m_Span.m_dSpan;
      }
    else if (IsUnsignedDataVT(V.vt))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_UI4);
      if (FAILED(hr))
        SetError(SErr_ApplySpanInComing, hr, "");
      V.ulVal=(V.ulVal-(unsigned long)m_Span.m_dOffset)/(unsigned long)m_Span.m_dSpan;
      }
    }
  }

// -----------------------------------------------------------------------

void CSlot::ApplySpanOutGoing(VARTYPE SlotType, COleVariant &V)
  {
  if (m_Span.m_bValid)
    {
    // Apply Span GOING to Device
    if (IsFloatDataVT(SlotType))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_R8);
      if (FAILED(hr))
        SetError(SErr_ApplySpanOutGoing, hr, "");
      V.dblVal=m_Span.m_dOffset+V.dblVal*m_Span.m_dSpan;
      if (m_Clamp.m_bValid)
        V.dblVal=Range(m_Clamp.m_dLo, V.dblVal, m_Clamp.m_dHi);
      }
    else if (IsSignedDataVT(SlotType))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_I4);
      if (FAILED(hr))
        SetError(SErr_ApplySpanOutGoing, hr, "");
      V.lVal=(long)m_Span.m_dOffset+V.lVal*(long)m_Span.m_dSpan;
      if (m_Clamp.m_bValid)
        V.lVal=Range((long)m_Clamp.m_dLo, V.lVal, (long)m_Clamp.m_dHi);
      }
    else if (IsUnsignedDataVT(SlotType))
      {
      HRESULT hr=VariantChangeType(&V, &V, 0, VT_UI4);
      if (FAILED(hr))
        SetError(SErr_ApplySpanOutGoing, hr, "");
      V.ulVal=(unsigned long)m_Span.m_dOffset+V.ulVal*(unsigned long)m_Span.m_dSpan;
      if (m_Clamp.m_bValid)
        V.ulVal=Range((unsigned long)m_Clamp.m_dLo, V.ulVal, (unsigned long)m_Clamp.m_dHi);
      }
    }
  }

// -----------------------------------------------------------------------

bool CSlot::SetValue(CChangeItem * pRqst)
  {
  ASSERT(pRqst && pRqst->m_lDstInx==m_lSlot);
  
  if (m_State.m_bHold && !pRqst->m_bOverrideHold)
    {
    m_ChgFlags|=IOMChgFlag_Ignored;
    m_lValuesIgnored++;
    return false;
    }

  if (pRqst->m_eSrc==eCSD_Device)
    {
    if (!m_bRead)
      {
      //m_lValuesIgnored++;
      return false;
      }

    if (VariantsEqual(&m_vValue, &pRqst->m_vValue)
        && m_wQuality==pRqst->m_wQuality
        //&& m_ftTimeStamp==pRqst->m_ftTimeStamp
        )
      {
      m_ChgFlags|=IOMChgFlag_Ignored;
      m_lValuesIgnored++;
      return false;
      }
    }

  m_ChgFlags|=IOMChgFlag_Changes|IOMChgFlag_Value|IOMChgFlag_HoldValue;
  m_lValueChanges++;


  VARTYPE SlotType=Type();
  FullValue()=*pRqst;
  if (m_bRead && (pRqst->m_eSrc==eCSD_Device))
    ApplySpanInComing(SlotType, FullValue().m_vValue);

  HRESULT hr=FullValue().ChangeType(SlotType);
  if (FAILED(hr))
    SetError(SErr_SetValue, hr, "");
  
  m_dwTransactionID=pRqst->m_dwTransactionID;

  if (m_bConnected && m_bWrite && (pRqst->m_eSrc!=eCSD_Device))
    {
    CFullValue FV(FullValue());
    ApplySpanOutGoing(SlotType, FV);
    HRESULT hr=FV.ChangeType(m_wNativeType);
    if (FAILED(hr))
      SetError(SErr_SetValueDevice, hr, "");
    gs_SlotMngr.m_Devices[m_lDevice]->AppendWriteRqst(m_lSlot, m_hServer, FV);
    }

  ProcessConnects();

  SendDisplayValue();
  return true;
  };

// -----------------------------------------------------------------------

bool CSlot::WriteCurrentValue2Device()
  {
  if (m_bConnected && m_bWrite)
    {
    CFullValue FV(FullValue());
    ApplySpanOutGoing(Type(), FV);
    HRESULT hr=FV.ChangeType(m_wNativeType);
    if (FAILED(hr))
      SetError(SErr_SetValueDevice, hr, "");
    gs_SlotMngr.m_Devices[m_lDevice]->AppendWriteRqst(m_lSlot, m_hServer, FV, true);
    return true;
    }
  return false;
  }

// -----------------------------------------------------------------------

void CSlot::Save(CProfINIFile & SavePF)
  {
  if (m_bInUse)
    {
    CString Line, V;
    VariantToString(*this, V, false);
    Line.Format("%i,%i,%i,%s ", m_lSlot, m_bSltDbgOn, m_State.m_bHold?1:0, V);
    SavePF.WrStr("Slots", m_sTag, Line);
    }
  };

// -----------------------------------------------------------------------

void CSlot::Load(CProfINIFile & SavePF)
  {
  if (!m_bRead)
     return;

  CString Line, V;
  Line=SavePF.RdStr("Slots", m_sTag, "");
  if (Line.GetLength()>0)
    {
    int iStart=0;
    int lSlot = SafeAtoL(Line.Tokenize(",", iStart));
    int lDbg  = SafeAtoL(Line.Tokenize(",", iStart));
    int lHold = SafeAtoL(Line.Tokenize(",", iStart));
    V=Line.Tokenize("'", iStart);
    
    m_bSltDbgOn=lDbg!=0;
    m_State.m_bHold=lHold!=0;

    CFullValue FV(OPC_QUALITY_GOOD);
    StringToVariant(Type(), V, FV, false);
    gs_SlotMngr.AppendChange(eCSD_File, -1, eCSD_Slot, m_lSlot, -1/*gs_SlotMngr.GetTransactionID()*/, FV, NULL, true);
    }
  };

// -----------------------------------------------------------------------

void CSlot::SendDisplayValue()
  {
  //dbgpln("SendDisplayValue %4i %s", m_lSlot, m_bVisible?"Visible":"Hidden");



  if (m_bVisible && gs_SlotMngr.m_bMonitorOn)
    {
    BOOL OK=theApp.PostThreadMessage(WMU_UPDATEONESLOT, m_lSlot, (long)new CSlotValue(*this)); 
    if (!OK)
      dbgpln("SendDisplayValue Failed %4i", m_lSlot);
    }
  }

// -----------------------------------------------------------------------

void CSlot::SendStateValue()
  {
  //dbgpln("SendDisplayValue %4i %s", m_lSlot, m_bVisible?"Visible":"Hidden");
  //if (m_bVisible && gs_SlotMngr.m_bMonitorOn)
    {
    BOOL OK=theApp.PostThreadMessage(WMU_UPDATEONESLOTSTATE, m_lSlot, StateLong()); 
    if (!OK)
      dbgpln("SendDisplayValue Failed %4i", m_lSlot);
    }
  }

// -----------------------------------------------------------------------

//void CSlot::SetRqdValue(LPCTSTR ValueID)
//  {
//  };

// -----------------------------------------------------------------------

void CSlot::RemoveConnects()
  {
  for (int c=0; c<m_SetConnects.GetSize(); c++)
    delete m_SetConnects[c];
  m_SetConnects.SetSize(0);

  m_ReflectedGets.SetSize(0);

  delete m_pGetConnect;
  m_pGetConnect=NULL;
  }

// -----------------------------------------------------------------------

void CSlot::InitialiseConnects()
  {
  for (int c=0; c<m_SetConnects.GetSize(); c++)
    m_SetConnects[c]->Initialise();

  m_ReflectedGets.SetSize(0);

  if (m_pGetConnect)
    m_pGetConnect->Initialise();
  }

// -----------------------------------------------------------------------

void CSlot::ConnectConnects()
  {
  for (int c=0; c<m_SetConnects.GetSize(); c++)
    m_SetConnects[c]->Connect(this);

  if (m_pGetConnect)
    m_pGetConnect->Connect(this);
  }

// -----------------------------------------------------------------------

void CSlot::CorrectConnects(CLongArray & NewNos)
  {
  for (int c=0; c<m_SetConnects.GetSize(); c++)
    m_SetConnects[c]->CorrectConnects(NewNos);

  if (m_pGetConnect)
    m_pGetConnect->CorrectConnects(NewNos);
  };

// -----------------------------------------------------------------------

long CSlot::ProcessConnects()
  {
  long n=0;
  for (int c=0; c<m_SetConnects.GetSize(); c++)
    m_SetConnects[c]->Process(eCSD_Slot, m_lSlot, FullValue());
  for (c=0; c<m_ReflectedGets.GetSize(); c++)
    m_ReflectedGets[c]->Process(eCSD_Slot, m_lSlot, FullValue());
  return n;
  };

// -----------------------------------------------------------------------

//void CSlot::SetError(1, LPCSTR Fmt, ...)
//  {
//  char Buff[2048];
//  va_list argptr;
//  va_start(argptr, Fmt);
//  vsprintf(Buff, Fmt, argptr);
//  va_end(argptr);
//
//  m_bError=true;
//  m_sError=Buff;
//  ::SetError(1, m_sTag, 0, "%s", Buff);
//  }

// -----------------------------------------------------------------------

IMPLEMENT_SPARES(CSlot, 0);
IMPLEMENT_SPARES(CSlotCfg, 0);
//IMPLEMENT_SPARES(CDspSlot, 0);
IMPLEMENT_SPARES(CSlotValue, 10000);

// =======================================================================
//
//
//
// =======================================================================
