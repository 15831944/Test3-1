//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __SP_DB_CPP
#include "sp_db.h"
#include "dbhelper.h"
#include "sp_dbextra.h"
#include "surfaces.h"
//#include "optoff.h"

#if dbgBuildSDB
#include "dbgmngr.h"
static CDbgMngr dbgBuildSpc           ("SysVec",  "SpDB-BuildSpc");
static CDbgMngr dbgBuildPhs           ("SysVec",  "SpDB-BuildPhs");
static CDbgMngr dbgBuildLoops         ("SysVec",  "SpDB-BuildLoops");
static CDbgMngr dbgBuildEqns          ("SysVec",  "SpDB-BuildEqns");
static CDbgMngr dbgBuildEqnsBoth      ("SysVec",  "SpDB-BuildEqnsBoth");
static CDbgMngr dbgSkipLists          ("SysVec",  "SpDB-SkipLists");
static CDbgMngr dbgDumpH2O            ("SysVec",  "SpDB-DumpH2O");
static CDbgMngr dbgDumpH2OLoT         ("SysVec",  "SpDB-DumpH2OLoT");
static CDbgMngr dbgDumpH2OHiT         ("SysVec",  "SpDB-DumpH2OHiT");
static CDbgMngr dbgDumpH2OLoP         ("SysVec",  "SpDB-DumpH2OLoP");
static CDbgMngr dbgDumpH2OHiP         ("SysVec",  "SpDB-DumpH2OHiP");
#endif

#if dbgSysVec
#if (!dbgBuildSDB)
#include "dbgmngr.h"
#endif
static CDbgMngr dbgDumpVectors        ("SysVec",  "DumpVectors");
static CDbgMngr dbgConfig             ("SysVec",  "Config");
static CDbgMngr dbgExpressionLoad     ("SysVec",  "ExpressionLoad");
static CDbgMngr dbgExpressionParse    ("SysVec",  "ExpressionParse");
static CDbgMngr dbgExpressionEval     ("SysVec",  "ExpressionEval");
static CDbgMngr dbgExpressionEvalAll  ("SysVec",  "ExpressionEvalAll");
static CDbgMngr dbgDumpTemplates      ("SysVec",  "DumpTemplates");
static long s_doExpressionEval=0;
#endif

#define CTX_FORCEFULLINIT 0

// ==========================================================================

extern AFX_EXTENSION_MODULE SCExecDLL;

static LPSTR SpecieDBLogHdr = "SpecieDB";
// ===========================================================================
//
//
//
//===========================================================================

long FixSpecieTypeId(long Type, long CfgFilePrjFileVerNo)
  {
  if (CfgFilePrjFileVerNo>=37)
    return Type;
  switch (Type)
    {
    case spl_Text_Old: return spl_Text;
    case spl_Page_Old: return spl_Page;
    case spl_AllSum_Old: return spl_AllSum;
    default: return Type;
    }
  }

// ==========================================================================

flag IsSpcName(Strng & S) { return S.XStrChr('(')!=NULL; };
Strng CmpName(Strng & S) { Strng C=S; char*p=C.XStrChr('('); if (p) C.SetLength(p-C()); return C; };
Strng CmpName(const char * m_pS) { Strng C=m_pS; char*p=C.XStrChr('('); if (p) C.SetLength(p-C()); return C; };

// ==========================================================================

flag SpDB_TokenOk(char* p)
  {
  return (p && strlen(p)>0 && (isdigit(*p) || *p=='-' || *p=='+' || *p=='.'));
  }

//---------------------------------------------------------------------------

double SpDB_TokenValue(char* p /*CTokenFile &Tkns*/, double Default)
  {
  if (SpDB_TokenOk(p))
    return SafeAtoF(p);
  else
    return Default;
  }

//---------------------------------------------------------------------------

void SpDB_ParseTokens(char * Buff, char* C[MaxSPDBCols])
  {
  C[0]=&Buff[0];
  for (int j=1,i=0; j<MaxSPDBCols-1; j++)
    C[j]="";//NULL;
  C[j]=NULL;
  // Kill EOL
  char *p=strchr(Buff, '\n');
  if (p!=NULL)
    *p=0;
  // Kill Tabs
  while ((p=strchr(Buff, '\t'))!=NULL)
    *p=' ';
  int Quote=0;
  int Paren=0;
  int Brace=0;
  for (i=0,j=0;Buff[i]!=0;i++)
    if (Buff[i]=='\"')
      Quote = (Quote == 0) ? 1 : (Quote == 1) ? 0 : Quote;
    else if (Buff[i]=='\'')
      Quote = (Quote == 0) ? 2 : (Quote == 2) ? 0 : Quote;
    else if (Buff[i]=='(')
      Paren++;
    else if (Buff[i]==')')
      Paren--;
    else if (Buff[i]=='[')
      Brace++;
    else if (Buff[i]==']')
      Brace--;
    else if (Quote==0 && Paren==0 && Brace==0)
      if (Buff[i]==',')
        {
        j++;
        C[j]=&Buff[i+1];
        Buff[i]=0;
        }
      if (Quote)
        LogError(SpecieDBLogHdr, 0, "Bad Token - Mismatched Quotes");

      for (j=0;C[j] && (j<MaxSPDBCols);j++)
        {
        XStrTrim(C[j]);
        if (j<2)
          {
          const int kend=strlen(C[j]);
          for (int k=0; k<kend; k++)
            if (C[j][k]==' ')
              C[j][k]='_';
          }
        }
  }

// ==========================================================================

double BigVapP      = 1.0e10; // kPa
double DefaultRho   = 1.0e-12;
double MeasTolerance     = 1.0e-20;
double FracMeasTolerance = 1.0e-8;

// ==========================================================================

bool CSpDBEqn::m_bDoTheBreak=0;

CSpDBEqn::CSpDBEqn(CSpConstData * pConst, CSpFidelData * pFData)
  { 
  m_pConst = pConst;
  m_pFData = pFData; 
  };
CSpDBEqn::~CSpDBEqn()
  {
  };

void CSpDBEqn::Attach(CSpConstData *pConst, CSpFidelData *pFData)
  {
  m_pConst = pConst;
  m_pFData = pFData; 
  };

// ==========================================================================
//
//
//
// ==========================================================================

CCElementDataBase::CCElementDataBase()
  {
  m_iNo=0;
  m_iNoStart=0;
  Init();
  };

//---------------------------------------------------------------------------

CCElementDataBase::~CCElementDataBase()
  {
  Term();
  };

// --------------------------------------------------------------------------

void CCElementDataBase::Init()
  {
  m_iNo=0;
  m_iNoStart=0;
  m_Data.SetSize(0);

  //Term();
  m_Data.Add(CElementD("H",     1,   1.00794    ));
  m_Data.Add(CElementD("He",    2,   4.00206    ));
  m_Data.Add(CElementD("Li",    3,   6.941      ));
  m_Data.Add(CElementD("Be",    4,   9.012182   ));
  m_Data.Add(CElementD("B",     5,  10.811      ));
  m_Data.Add(CElementD("C",     6,  12.011      ));
  m_Data.Add(CElementD("N",     7,  14.00674    ));
  m_Data.Add(CElementD("O",     8,  15.9994     ));
  m_Data.Add(CElementD("F",     9,  18.9984032  ));
  m_Data.Add(CElementD("Ne",   10,  20.1797     ));
  m_Data.Add(CElementD("Na",   11,  22.989768   ));
  m_Data.Add(CElementD("Mg",   12,  24.3050     ));
  m_Data.Add(CElementD("Al",   13,  26.981539   ));
  m_Data.Add(CElementD("Si",   14,  28.0855     ));
  m_Data.Add(CElementD("P",    15,  30.97362    ));
  m_Data.Add(CElementD("S",    16,  32.066      ));
  m_Data.Add(CElementD("Cl",   17,  35.4527     ));
  m_Data.Add(CElementD("Ar",   18,  39.948      ));
  m_Data.Add(CElementD("K",    19,  39.0983     ));
  m_Data.Add(CElementD("Ca",   20,  40.078      ));
  m_Data.Add(CElementD("Sc",   21,  44.955910   ));
  m_Data.Add(CElementD("Ti",   22,  47.88       ));
  m_Data.Add(CElementD("V",    23,  50.9415     ));
  m_Data.Add(CElementD("Cr",   24,  51.9961     ));
  m_Data.Add(CElementD("Mn",   25,  54.93085    ));
  m_Data.Add(CElementD("Fe",   26,  55.847      ));
  m_Data.Add(CElementD("Co",   27,  58.9332     ));
  m_Data.Add(CElementD("Ni",   28,  58.6934     ));
  m_Data.Add(CElementD("Cu",   29,  63.546      ));
  m_Data.Add(CElementD("Zn",   30,  65.39       ));
  m_Data.Add(CElementD("Ga",   31,  69.723      ));
  m_Data.Add(CElementD("Ge",   32,  72.61       ));
  m_Data.Add(CElementD("As",   33,  74.92159    ));
  m_Data.Add(CElementD("Se",   34,  78.96       ));
  m_Data.Add(CElementD("Br",   35,  79.904      ));
  m_Data.Add(CElementD("Kr",   36,  83.80       ));
  m_Data.Add(CElementD("Rb",   37,  85.4678     ));
  m_Data.Add(CElementD("Sr",   38,  87.62       ));
  m_Data.Add(CElementD("Y",    39,  88.90585    ));
  m_Data.Add(CElementD("Zr",   40,  91.224      ));
  m_Data.Add(CElementD("Nb",   41,  92.90638    ));
  m_Data.Add(CElementD("Mo",   42,  95.94       ));
  m_Data.Add(CElementD("Tc",   43,  98.0        ));
  m_Data.Add(CElementD("Ru",   44, 101.07       ));
  m_Data.Add(CElementD("Rh",   45, 102.9055     ));
  m_Data.Add(CElementD("Pd",   46, 106.42       ));
  m_Data.Add(CElementD("Ag",   47, 107.8682     ));
  m_Data.Add(CElementD("Cd",   48, 112.411      ));
  m_Data.Add(CElementD("In",   49, 114.818      ));
  m_Data.Add(CElementD("Sn",   50, 118.71       ));
  m_Data.Add(CElementD("Sb",   51, 121.757      ));
  m_Data.Add(CElementD("Te",   52, 127.6        ));
  m_Data.Add(CElementD("I",    53, 126.90447    ));
  m_Data.Add(CElementD("Xe",   54, 131.29       ));
  m_Data.Add(CElementD("Cs",   55, 132.90543    ));
  m_Data.Add(CElementD("Ba",   56, 137.327      ));
  m_Data.Add(CElementD("La",   57, 138.9055     ));
  m_Data.Add(CElementD("Ce",   58, 140.12       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Pr",   59, 140.907      )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Nd",   60, 144.24       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Pm",   61, 145.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Sm",   62, 150.35       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Eu",   63, 151.96       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Gd",   64, 157.25       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Tb",   65, 158.924      )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Dy",   66, 162.50       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Ho",   67, 164.930      )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Er",   68, 167.26       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Tm",   69, 168.934      )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Yb",   70, 173.04       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Lu",   71, 174.97       )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Hf",   72, 178.49       ));
  m_Data.Add(CElementD("Ta",   73, 180.9479     ));
  m_Data.Add(CElementD("W",    74, 183.84       ));
  m_Data.Add(CElementD("Re",   75, 186.207      ));
  m_Data.Add(CElementD("Os",   76, 190.23       ));
  m_Data.Add(CElementD("Ir",   77, 192.22       ));
  m_Data.Add(CElementD("Pt",   78, 195.08       ));
  m_Data.Add(CElementD("Au",   79, 196.96654    ));
  m_Data.Add(CElementD("Hg",   80, 200.59       ));
  m_Data.Add(CElementD("Tl",   81, 204.3833     ));
  m_Data.Add(CElementD("Pb",   82, 207.2        ));
  m_Data.Add(CElementD("Bi",   83, 208.98037    ));
  m_Data.Add(CElementD("Po",   84, 208.9824     ));
  m_Data.Add(CElementD("At",   85, 209.9871     ));
  m_Data.Add(CElementD("Rn",   86, 222.0176     ));
  m_Data.Add(CElementD("Fr",   87, 223.0197     ));
  m_Data.Add(CElementD("Ra",   88, 226.0254     ));
  m_Data.Add(CElementD("Ac",   89, 227.0278     ));
  m_Data.Add(CElementD("Th",   90, 232.0381     ));
  m_Data.Add(CElementD("Pa",   91, 231.03588    ));
  m_Data.Add(CElementD("U",    92, 238.0289     ));
  m_Data.Add(CElementD("Np",   93, 237.048      ));
  m_Data.Add(CElementD("Pu",   94, 244.0642     ));
  m_Data.Add(CElementD("Am",   95, 243.0614     ));
  m_Data.Add(CElementD("Cm",   96, 247.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Bk",   97, 247.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Cf",   98, 249.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Es",   99, 254.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Fm",  100, 253.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Md",  101, 256.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("No",  102, 253.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD("Lr",  103, 257.0        )); // from elec eng hand book (Siemens)
  m_Data.Add(CElementD(NULL,  104,   0.0        ));
  m_Data.Add(CElementD(NULL,  105,   0.0        ));
  m_Data.Add(CElementD(NULL,  106,   0.0        ));
  m_Data.Add(CElementD(NULL,  107,   0.0        ));
  m_Data.Add(CElementD(NULL,  108,   0.0        ));
  m_Data.Add(CElementD(NULL,  109,   0.0        ));
  m_Data.Add(CElementD(NULL,  110,   0.0        ));
  m_Data.Add(CElementD(NULL,  111,   0.0        ));
  m_Data.Add(CElementD(NULL,  112,   0.0        ));
  m_Data.Add(CElementD(NULL,  113,   0.0        ));
  m_Data.Add(CElementD(NULL,  114,   0.0        ));
  m_Data.Add(CElementD(NULL,  115,   0.0        ));
  m_Data.Add(CElementD(NULL,  116,   0.0        ));
  m_Data.Add(CElementD(NULL,  117,   0.0        ));
  m_Data.Add(CElementD(NULL,  118,   0.0        ));
  m_Data.Add(CElementD(NULL,  119,   0.0        ));


  for (m_iNo=0; m_Data[m_iNo].m_Name!=NULL; m_iNo++)
    {
    m_Data[m_iNo].m_SpComp= new CompFracArray;
    m_Data[m_iNo].m_fAllocdMem=false;
    };
  m_iNoStart=m_iNo;

  DDBElementList.Empty();
  for (int e=0; e<m_iNo; e++)
    DDBElementList.Add(e, m_Data[e].m_Name);

  };

//---------------------------------------------------------------------------

void CCElementDataBase::Term()
  {
  for (int i=0; i<m_iNo; i++)
    {
    delete m_Data[i].m_SpComp;
    if (m_Data[i].m_fAllocdMem)
      {
      delete m_Data[i].m_Name;
      m_Data[i].m_Name=NULL;
      }
    }
  m_iNo=0;
  m_iNoStart=0;
  m_Data.SetSize(0);
  }

//---------------------------------------------------------------------------

CElementD & CCElementDataBase::operator [](int s) {return m_Data[s];};

//---------------------------------------------------------------------------

int CCElementDataBase::Find(const char * pElName)
  {
  for (word i=0; i<m_iNo; i++)
    if (strcmp(m_Data[i].m_Name, pElName)==0)
      return i;
  return -1;
  };

//---------------------------------------------------------------------------

flag CCElementDataBase::CheckElDef(Strng &ElDef)
  {
  Strng ElDefRet;
  flag Ok=(ElDef.Length()>0);
  bool FndOne = false;
  int i=0;
  for ( ; Ok; )
    {
    if (i>=ElDef.Length())
      break;
    if (isalpha(ElDef[i]) && isupper(ElDef[i]))
      {
	  FndOne = true;
      Strng El, No, Wt;
      while (isalpha(ElDef[i])) 
		El+=ElDef[i++];
      if (!isdigit(ElDef[i]) && ElDef[i]!='.')
        {
		Strng c;
		c = ElDef[i];
        LogError(SpecieDBLogHdr, 0, "Illegal character '%s', Element name must only contain characters and begin with uppercase character.", c());
        Ok=false;
	    }
	  if (ElDef[i]=='.')
	    No="0";
      while (isdigit(ElDef[i]) || ElDef[i]=='.' || ElDef[i]=='/')
	    No+=ElDef[i++];
      if (i < ElDef.Length() && ElDef[i]=='(')
        {
        i++;
        while (isdigit(ElDef[i]) || ElDef[i]=='.')
		  Wt+=ElDef[i++];
        Ok=Ok && (ElDef[i]==')');
        i++;
        }
      // Force second char and on to be lowercase
      flag DoMsg=0;
      Strng ElOld(El);
      for (int j=1; j<El.Length(); j++)
        if (isupper(El[j]))
          {
          El[j]=tolower(El[j]);
          DoMsg=1;
          }
        if (DoMsg)
          LogError(SpecieDBLogHdr, 0, "Pseudo element name '%s' corrected to '%s'", ElOld(), El());
        if (No.Length()==0)
          {
          LogError(SpecieDBLogHdr, 0, "Number of Atoms of '%s' in '%s' not specified", El(), ElDef());
          No="1";
          Ok=false;
          }
        if (Ok && (Find(El())<0) && Wt.Length()>0)
          {
          m_Data[m_iNo].m_fAllocdMem=true;
          m_Data[m_iNo].m_Name=new char[El.Length()+1];
          strcpy(m_Data[m_iNo].m_Name, El());
          m_Data[m_iNo].m_AtmNo=m_iNo+1;
          m_Data[m_iNo].m_AtmWt=SafeAtoF(Wt());
          m_Data[m_iNo].m_SpComp= new CompFracArray;
          Ok=(m_Data[m_iNo].m_AtmWt>=0.5);
          m_iNo++;
          }
        if (Ok && Find(El())<0)
          {
          LogError(SpecieDBLogHdr, 0, "Invalid element '%s' or molecular weight not specified for pseudo element '%s' eg Or1(100)", El(), El());
          Ok=false;
          }
        if (Ok)
          {
          ElDefRet+=El();
          ElDefRet+=No();
          }
      }
    else
      i++;
    }
  if (!FndOne)
    {
    LogError(SpecieDBLogHdr, 0, "Element name must only contain characters and begin with uppercase character, '%s' is invalid", ElDef());
    Ok = false;
    }
  if (Ok)
    ElDef=ElDefRet();
  return Ok;
  };

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void GetElements (Strng &ElDef, Strng &Els)
  {
  int i=0;
  Els="";
  for ( ; ; )
    {
    if (i>=ElDef.Length())
      break;
    if (isalpha(ElDef[i]) && isupper(ElDef[i]))
      {
      Strng El, No;
      while (isalpha(ElDef[i])) El+=ElDef[i++];
      while (isdigit(ElDef[i]) || ElDef[i]=='.' || ElDef[i]=='/')No+=ElDef[i++];
      Els+=El();
      Els+=',';
      }
    else
      i++;
    }
  }

//---------------------------------------------------------------------------

double GetElemAmt(Strng &ElDef, Strng& Els)
  {
  double Amt=0.0;
  pchar p=ElDef();
  while ((p=strstr(p, Els()))!=NULL)
    {
    p+=Els.Length();
    if (isdigit(*p))
      break;
    }
  if (p)
    {
    Amt=SafeAtoF(p);
    while (isdigit(*p) || *p=='.') p++;
    if (*p=='/') // ie a Fraction
      {
      p++;
      Amt/=Max(0.1, SafeAtoF(p));
      };
    }

  return Amt;
  }

// ==========================================================================
//
//
//
// ==========================================================================

SpPropOveride::SpPropOveride()
  {
  bAdd=SPO_NULL;
  bRem=SPO_NULL;
  bUse=SPO_NULL;

  fVCp=1.0f;
  fLCp=4.18f;
  fSCp=1.0f;

  fVDens=1.0f;
  fLDens=1000.0f;
  fSDens=2000.0f;

  fVDynVisc=0.000120f;
  fLDynVisc=0.001000f;

  fLThermCnd = 0.00058f;   // Water
  fVThermCnd = 0.000016f;  // Steam

  fSurfT=0.03f;

  fBPEle=0.0f;
  };

void SpPropOveride::CopySrc(SpPropOveride & Src)
  {
  if (((bAdd & SPO_VCp  )==0)     && (Src.bUse & SPO_VCp  ))      fVCp        = Src.fVCp  ;
  if (((bAdd & SPO_LCp  )==0)     && (Src.bUse & SPO_LCp  ))      fLCp        = Src.fLCp  ;
  if (((bAdd & SPO_SCp  )==0)     && (Src.bUse & SPO_SCp  ))      fSCp        = Src.fSCp  ;
  if (((bAdd & SPO_VDens)==0)     && (Src.bUse & SPO_VDens))      fVDens      = Src.fVDens;
  if (((bAdd & SPO_LDens)==0)     && (Src.bUse & SPO_LDens))      fLDens      = Src.fLDens;
  if (((bAdd & SPO_SDens)==0)     && (Src.bUse & SPO_SDens))      fSDens      = Src.fSDens;
  if (((bAdd & SPO_VDynVisc)==0)  && (Src.bUse & SPO_VDynVisc))   fVDynVisc   = Src.fVDynVisc;
  if (((bAdd & SPO_LDynVisc)==0)  && (Src.bUse & SPO_LDynVisc))   fLDynVisc   = Src.fLDynVisc;
  if (((bAdd & SPO_VThermCnd)==0) && (Src.bUse & SPO_VThermCnd))  fVThermCnd  = Src.fVThermCnd;
  if (((bAdd & SPO_LThermCnd)==0) && (Src.bUse & SPO_LThermCnd))  fLThermCnd  = Src.fLThermCnd;
  if (((bAdd & SPO_SurfT)==0)     && (Src.bUse & SPO_SurfT))      fSurfT      = Src.fSurfT;
  if (((bAdd & SPO_BPEle)==0)     && (Src.bUse & SPO_BPEle))      fBPEle      = Src.fBPEle;

  bUse = (bAdd | Src.bUse) & ~bRem;
  }

void SpPropOveride::AddSrc(SpPropOveride & Src)
  {
  if (((bAdd & SPO_VCp  )==0)     && (Src.bUse & SPO_VCp  ))      fVCp        = Src.fVCp  ;
  if (((bAdd & SPO_LCp  )==0)     && (Src.bUse & SPO_LCp  ))      fLCp        = Src.fLCp  ;
  if (((bAdd & SPO_SCp  )==0)     && (Src.bUse & SPO_SCp  ))      fSCp        = Src.fSCp  ;
  if (((bAdd & SPO_VDens)==0)     && (Src.bUse & SPO_VDens))      fVDens      = Src.fVDens;
  if (((bAdd & SPO_LDens)==0)     && (Src.bUse & SPO_LDens))      fLDens      = Src.fLDens;
  if (((bAdd & SPO_SDens)==0)     && (Src.bUse & SPO_SDens))      fSDens      = Src.fSDens;
  if (((bAdd & SPO_VDynVisc)==0)  && (Src.bUse & SPO_VDynVisc))   fVDynVisc   = Src.fVDynVisc;
  if (((bAdd & SPO_LDynVisc)==0)  && (Src.bUse & SPO_LDynVisc))   fLDynVisc   = Src.fLDynVisc;
  if (((bAdd & SPO_VThermCnd)==0) && (Src.bUse & SPO_VThermCnd))  fVThermCnd  = Src.fVThermCnd;
  if (((bAdd & SPO_LThermCnd)==0) && (Src.bUse & SPO_LThermCnd))  fLThermCnd  = Src.fLThermCnd;
  if (((bAdd & SPO_SurfT)==0)     && (Src.bUse & SPO_SurfT))      fSurfT      = Src.fSurfT;
  if (((bAdd & SPO_BPEle)==0)     && (Src.bUse & SPO_BPEle))      fBPEle      = Src.fBPEle;

  bUse |= (bAdd | Src.bUse) & ~bRem;
  };

//===========================================================================
//
//
//
// ==========================================================================

static CAcidBaseData s_AcidData[] =
  {
    {"HCl",         1, {1,               0,                   0                   }},
    {"HNO3",        1, {1,               0,                   0                   }},
    {"H2SO4",       1, {1,               0.0102,              0                   }},
    {"H2CrO4",      1, {0.18,            0.00000032,          0                   }},
    {"HIO3",        1, {0.16,            0,                   0                   }},
    {"H2C2O4",      1, {0.056,           0.000054,            0                   }},
    {"H3PO3",       1, {0.03,            0.00000017,          0                   }},
    {"H5IO6",       1, {0.028,           0.0000000053,        0                   }},
    {"H2SO3",       1, {0.014,           0.000000065,         0                   }},
    {"HClO2",       1, {0.0112,          0,                   0                   }},
    {"H3PO4",       1, {0.0072,          0.00000063,          0.00000000000042    }},
    {"H3ASO4",      1, {0.006,           0.00000011,          0.000000000003      }},
    {"H3SeO3",      1, {0.0035,          0.00000005,          0                   }},
    {"C3H4O4",      1, {0.0015,          0.000002,            0                   }},
    {"C2H3O2Cl",    1, {0.0014,          0,                   0                   }},
    {"C4H6O6",      1, {0.001,           0.000045,            0                   }},
    {"H3C6H5O7",    1, {0.00075,         0.000017,            0.0000004           }},
    {"HNO2",        1, {0.00071,         0,                   0                   }},
    {"HF",          1, {0.00068,         0,                   0                   }},
    {"HCNO",        1, {0.00035,         0,                   0                   }},
    {"HCOOH",       1, {0.00018,         0,                   0                   }},
    {"C3H6O3",      1, {0.00014,         0,                   0                   }},
    {"C6H5COOH",    1, {0.000063,        0,                   0                   }},
    {"HN3",         1, {0.000019,        0,                   0                   }},
    {"CH3COOH",     1, {0.000018,        0,                   0                   }},
    {"CH3CH2COOH",  1, {0.000013,        0,                   0                   }},
    {"H2C6H6O6",    1, {0.00001,         0.000000000005,      0                   }},
    {"H2CO3",       1, {0.00000045,      0.000000000047,      0                   }},
    {"H2S",         1, {0.00000009,      0.00000000000000001, 0                   }},
    {"HClO3",       1, {0.000000029,     0,                   0                   }},
    {"HBrO",        1, {0.0000000023,    0,                   0                   }},
    {"H3BO3",       1, {0.00000000073,   0.00000000000018,    0                   }},
    {"HCN",         1, {0.00000000062,   0,                   0                   }},
    {"H3AsO3",      1, {0.0000000006,    0,                   0                   }},
    {"C6H5OH",      1, {0.0000000001,    0,                   0                   }},
    {"HIO",         1, {0.000000000023,  0,                   0                   }},
    {"H2O2",        1, {0.0000000000024, 0,                   0                   }},
    {"NaOH",        0, {1             }},
    {"LiOH",        0, {1             }},
    {"KOH",         0, {1             }},
    {"Na2O",        0, {1             }},
    {"LiO2",        0, {1             }},
    {"K2O",         0, {1             }},
    {"MgO",         0, {1             }},
    {"CaO",         0, {1             }},
    {"Mg[OH]2",     0, {1             }},
    {"Ca[OH]2",     0, {0.00374       }},
    {"[CH3CH2]2NH", 0, {0.00086       }},
    {"[CH3]2NH",    0, {0.00059       }},
    {"[CH3CH2]3N",  0, {0.00052       }},
    {"CH3NH2",      0, {0.00044       }},
    {"CO3--",       0, {0.00021       }},
    {"HOCH2CH2NH2", 0, {0.000032      }},
    {"NH3",         0, {0.0000176     }},
    {"C10H14N2",    0, {0.0000007     }},
    {"ClO-",        0, {0.00000031    }},
    {"HS-",         0, {0.0000001     }},
    {"H2NOH",       0, {0.000000011   }},
    {"C5H5N",       0, {0.0000000017  }},
    {"C6H5NH2",     0, {0.0000000004  }},
    {}
  };


CAcidBaseData * FindAcidBaseData(LPCSTR Compound)
  {
  for (int i=0; s_AcidData[i].m_sName; i++)
    {
    if ((strncmp(s_AcidData[i].m_sName, Compound, strlen(s_AcidData[i].m_sName))==0) &&
        (Compound[strlen(s_AcidData[i].m_sName)]=='('))
      return &s_AcidData[i];
    }
  return NULL;
  }

//===========================================================================
//
//
//
// ==========================================================================

LPCTSTR OccurenceNames[] =
  {
  "Solid",
    "Liquid",
    "Vapour",
    "Last",
    "Null",
  };

// ==========================================================================
//
//
//
// ==========================================================================

CSpConstData::CSpConstData()
  {
  m_dMoleWt=0;

  m_bPcOK=false;
  m_bTcOK=false;
  m_bVcOK=false;
  m_bAcOK=false;
  m_bMvOK=false;
  m_bMdOK=false;

  m_dPc=1;

  m_dPc=1000.0;
  m_dTc=1000.0;
  m_dVc=1.0;
  m_dAc=1.0;
  m_dMv=22.414;
  m_dMd=dNAN;

  m_bDaOK   = false;
  m_iDaType = CSpConstData::DaType_IsNull;
  m_dDa[0]  = 0.0; 
  m_dDa[1]  = 0.0; 
  m_dDa[2]  = 0.0; 
  };

// ==========================================================================
//
//
//
// ==========================================================================

CSpCommonData::CSpCommonData()
  {
  m_bVpOK = false;
  m_pVp   = NULL;

  m_pConst = NULL;
  m_pCmp   = NULL;
  }

//-----------------------------------------------------------------------

static CToleranceBlock s_VapTTol(TBF_BothSys, "SpecieDB:VapTFnd", 0.0, 1.0e-9, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

class CVapTFndC : public MRootFinderBase
  {
  public:
    CComponent     * m_pC;
    CSpCommonData  * m_pCom;

    CVapTFndC(CComponent *pC, CSpCommonData *pCom) :
    MRootFinderBase("CVapTFndC", s_VapTTol)
      {
      m_pC=pC;
      m_pCom=pCom;
      };
    LPCTSTR ObjTag() { return (LPCTSTR)m_pC->SymOrTag(); };
    double Function(double T) 
      {
      double Ret=m_pCom->VapourP(T); 
      if (0)//Iterations()>(8*m_Tol.GetMaxIters())/10)
        dbgpln("CVapTFnd %8.2f %8.2f  %s", T, Ret, m_pC->SymOrTag());
      return Ret; 
      };
  };

//-----------------------------------------------------------------------

double CSpCommonData::VapourT(double P, double dPrvVapT)
  {
  if (!VpOK())
    return 0.0;

  if (!Valid(dPrvVapT))
    dPrvVapT=373.0;
  CVapTFndC VapT(m_pCmp, this);
  VapT.SetTarget(P);
  VapT.SetLimits(1.0, m_pConst->TCrit());
  dPrvVapT=Range(1.0, dPrvVapT, m_pConst->TCrit());
  if (VapT.SolveFromEst(dPrvVapT, 1.0, 50.0)==RF_OK)
    dPrvVapT=VapT.Result();
  else
    LogError("CDB", 0, "Vapour Temp not Converged @ %.2f kPa %s", P, m_pCmp->SymOrTag());

  return dPrvVapT;
  };

// ==========================================================================
//
//
//
// ==========================================================================

CSpFidelData::CSpFidelData()
  {
  m_pSp=NULL;
  m_pConst=NULL;
  m_pCommon=NULL;
  m_bIsCopy=false;
  m_iLastTPoint=0;
  m_dTs[m_iLastTPoint]=0.0;
  m_dTe[m_iLastTPoint]=1.0e6;
  for (int i=0; i<MaxTPoints; i++)
    {
    m_dTs[i]    = 0.0;
    m_dTe[i]    = 1.0e6;
    m_pRho[i]   = NULL;
    m_pCp[i]    = NULL;
    m_pH[i]     = NULL;
    m_pS[i]     = NULL;
    //m_pVp[i]    = NULL;
    m_dH25[i]   = 0;
    m_dS25[i]   = 0;
    m_dCHf[i]   = 0;
    m_dCdHf[i]  = 0;
    m_dCZRef[i] = 0;
    m_dCSf[i]   = 0;
    }

  m_dHf0      = 0;
  m_dHfBase0  = 0;

  m_dS0       = 0;
  m_dSBase0   = 0;

  m_dCpCv     = 0;

  m_dTRef     = C_2_K(25.0);

  m_bHasHFn   = false;
  m_bHasSFn   = false;

  m_bCpOK     = false;
  m_bH25OK    = false;
  m_bS25OK    = false;
  //m_bVpOK     = false;

  m_iHfBase   = -1;

  m_bChecked  = false;
  };

//---------------------------------------------------------------------------

CSpFidelData::~CSpFidelData()
  {
  for (int i=0; i<MaxTPoints; i++)
    {
    if (!m_bIsCopy)
      {
      delete m_pRho[i];
      delete m_pCp[i];
      delete m_pH[i];
      delete m_pS[i];
      //delete m_pVp[i];
      }

    m_pRho[i]=NULL;
    m_pCp[i]=NULL;
    m_pH[i]=NULL;
    m_pS[i]=NULL;
    //m_pVp[i]=NULL;
    }
  }

// ==========================================================================
//
//
//
// ==========================================================================

CDensCorr::CDensCorr()
  {
  m_iSolute=-1;
  m_iSolvent=-1;
  m_pDensCorrFn=NULL;
  m_DensCorrZero=0.0;
  };

CDensCorr::CDensCorr(int Solute, int Solvent,
                     CSpDBEqn *DensCorrFn, double DensCorrZero,
                     double LimitFrac, double PureValue, bool DoWarn)
  {
  m_iSolute        = Solute;
  m_iSolvent       = Solvent;
  m_pDensCorrFn    = DensCorrFn;
  m_DensCorrZero   = DensCorrZero;
  m_LimitFrac      = LimitFrac;        
  m_PureValue      = PureValue;
  m_DoWarn         = DoWarn;
  };

CDensCorr::~CDensCorr()
  {
  };

void CDensCorr::Clear()
  {
  delete m_pDensCorrFn;
  m_pDensCorrFn=NULL;
  };

double CDensCorr::Evaluate(long Fidelity, double T, double P, CSVData M)
  {
  double DensSolvent    = SDB[m_iSolvent].Density(Fidelity, T,P, NULL, M);

  double MassSolvent    = M[m_iSolvent];
  double MassSolute     = M[m_iSolute];
  double MassTotal      = MassSolute+MassSolvent;
  double MassFrac       = MassSolute/GTZ(MassSolute+MassSolvent);
  double MassFracCalc   = Min(MassFrac, m_LimitFrac);

  double MixDens        = DensSolvent * (1.0+DensCorr(MassFracCalc));

  double AppDensSolute  = MassSolute / GTZ(MassTotal/MixDens - MassSolvent/DensSolvent);

  if (MassFrac>m_LimitFrac)
    {
    AppDensSolute=AppDensSolute+(m_PureValue-AppDensSolute)*(MassFrac-m_LimitFrac)/GTZ(1.0-m_LimitFrac);
    }    

  return AppDensSolute;
  };

// ==========================================================================

CCpCorr::CCpCorr()
  {
  m_iSolute     = -1;
  m_pCpCorrFn   = NULL;
  m_dCpCorrZero = 0.0;
  };

CCpCorr::CCpCorr(int Solute, CSpDBEqn *CpCorrFn, double CpCorrZero)
  {
  m_iSolute        = Solute;
  m_pCpCorrFn      = CpCorrFn;
  m_dCpCorrZero    = CpCorrZero;
  };

CCpCorr::~CCpCorr()
  {
  };

void CCpCorr::Clear()
  {
  delete m_pCpCorrFn;
  m_pCpCorrFn=NULL;
  };

// ==========================================================================

CSolubility::CSolubility()
  {
  m_iSoluteA=-1;
  m_iSoluteS=-1;
  m_pSolubleFn=NULL;
  m_pHeatOfMixFn=NULL;
  };

CSolubility::CSolubility(int SoluteA, int SoluteS,
                         CSpDBEqn *SolubleFn, CSpDBEqn *HeatOfMixFn)
  {
  m_iSoluteA       = SoluteA;
  m_iSoluteS       = SoluteS;
  m_pSolubleFn     = SolubleFn;
  m_pHeatOfMixFn   = HeatOfMixFn;
  };

CSolubility::~CSolubility()
  {
  };

void CSolubility::Clear()
  {
  delete m_pSolubleFn;
  delete m_pHeatOfMixFn;
  m_pSolubleFn=NULL;
  m_pHeatOfMixFn=NULL;
  };

// ==========================================================================
//
//
//
// ==========================================================================

CSpecie::CSpecie(CComponent * pCmp, CSpecieDataBase & SDB, CComponentDataBase & CDB, CH2ODataBase & H2ODB, const char * pCmpTag): \
m_SDB(SDB),
m_CDB(CDB),
m_H2O(H2ODB),
m_Cmp(*pCmp),
m_Const(*pCmp),
m_Common(pCmp->m_Common)
  {
  sCmpTag=pCmpTag;
  sTag="";
  sSym="";
  m_sPhaseSymVisible="";
  m_sPhaseSymActual="";

  m_PhInx=0;
  m_iBOccT=0;

  m_eOcc=BOT_Null;
  m_PhMsk=0;
  m_OccMsk=0;

  m_dTsTol=25;
  m_dTeTol=25;

  iSId=-1;
  iCId=-1;

  m_bCpOvrMask=0;
  m_bDensOvrMask=0;
  m_iOverIndex=0;

  pPrev=NULL;
  pNext=NULL;
  pFirst=NULL;

  m_bReference=false;
  m_bPrjDB=true;

  m_iSolvent=-1;

  m_pDensCalc=NULL;

  m_bHasCpCorrs=false;
  m_bCpInDirect=false;

  m_bVpSpecd = false;
  m_bTcSpecd = false;
  m_bPcSpecd = false;
  m_bVcSpecd = false;
  m_bAcSpecd = false;

  };

//---------------------------------------------------------------------------

CSpecie::~CSpecie()
  {
  if (CDB.m_pWater==this)
    CDB.m_pWater=NULL;
  if (CDB.m_pSteam==this)
    CDB.m_pSteam=NULL;

  delete m_pDensCalc;

  for (int i=0; i<m_DensCorrs.GetSize(); i++)
    m_DensCorrs[i].Clear();
  for (int i=0; i<m_CpCorrs.GetSize(); i++)
    m_CpCorrs[i].Clear();
  for (int i=0; i<m_Solubilities.GetSize(); i++)
    m_Solubilities[i].Clear();
  };

//---------------------------------------------------------------------------

void CSpecie::SetData(CSpCommonDataA &C, CSpFidelDataA &D)
  {
  for (int f=0; f<MaxFidelity; f++)
    {
    m_Common[f]=C[f];
    m_Common[f].m_pCmp = &m_Cmp;
    m_Common[f].m_pConst = &m_Const;

    m_FData[f]=D[f];
    m_FData[f].m_pSp=this;
    m_FData[f].m_bIsCopy=true;

    m_FData[f].m_pCommon = &m_Common;
    m_FData[f].m_pConst = &m_Const;

    for (int i=0; i<SpDBEqnCnt; i++)
      {
      for (int j=0; j<MaxTPoints; j++)
        {
        if (m_FData[f].m_pEqns[i][j])
          {
          m_FData[f].m_pEqns[i][j]->m_pConst=&m_Const;
          m_FData[f].m_pEqns[i][j]->m_pFData=&m_FData[f];
          }
        }
      }
    }
  };

//---------------------------------------------------------------------------

void CSpFidelData::AddSpecieTData(CSpFidelData * pFData)
  {
  for (int i=0; i<m_iLastTPoint+1; i++)
    if (pFData->m_dTs[0] < m_dTs[i])
      break;

  m_iLastTPoint++;
  for (int j=m_iLastTPoint; j>i; j--)
    {
    m_dTs[j]    = m_dTs[j-1];
    m_dTe[j]    = m_dTe[j-1];
    m_pRho[j]   = m_pRho[j-1];
    m_pCp[j]    = m_pCp[j-1];
    m_pH[j]     = m_pH[j-1];
    m_pS[j]     = m_pS[j-1];
    m_dH25[j]   = m_dH25[j-1];
    m_dS25[j]   = m_dS25[j-1];
    m_dCZRef[j] = m_dCZRef[j-1];
    m_dCdHf[j]  = m_dCdHf[j-1];
    m_dCHf[j]   = m_dCHf[j-1];
    m_dCSf[j]   = m_dCSf[j-1];
    }

  m_dTs[i]=pFData->m_dTs[0];
  m_dTe[i]=pFData->m_dTe[0];

  m_pRho[i]   = pFData->m_pRho[0];
  m_pCp[i]    = pFData->m_pCp[0];
  m_pH[i]     = pFData->m_pH[0];
  m_pS[i]     = pFData->m_pS[0];

  m_dH25[i]   = pFData->m_dH25[0];
  m_dS25[i]   = pFData->m_dS25[0];
  m_dCZRef[i] = pFData->m_dCZRef[0];
  m_dCdHf[i]  = pFData->m_dCdHf[0];
  m_dCHf[i]   = pFData->m_dCHf[0];
  m_dCSf[i]   = pFData->m_dCSf[0];

  pFData->m_pRho[0]  = NULL;
  pFData->m_pCp[0]   = NULL;
  pFData->m_pH[0]    = NULL;
  pFData->m_pS[0]    = NULL;

  };

//---------------------------------------------------------------------------

static flag PhaseNamePrimary(int eOcc, LPSTR PhName)
  {
  switch (eOcc)
    {
    case BOT_Liquid:
      return _stricmp(PhName, "l")==0;
    case BOT_Gas:
      return _stricmp(PhName, "v")==0 || _stricmp(PhName, "g")==0;
    case BOT_Solid:
      return _stricmp(PhName, "s")==0;
    }
  return false;
  }

//---------------------------------------------------------------------------

static void AppendErrorMsg(CSpecie *pSp, DWORD Type, LPCTSTR fmt, ...)
  {
  char Buff[512];
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);

  //dbgpln("AppendErrorMsg: [0x%08x] %5i %s", pSp, pSp?pSp->m_sVpErrors.Length():-1, Buff);

  if (pSp)
    {
    Strng S(Buff);
    S.SetIndex(Type);
    pSp->m_sVpErrors.Append(S);
    }
  else
    {
    switch (Type)
      {
      case LogFlag_Fatal:   LogFatal  (SpecieDBLogHdr, 0, "%s", Buff);    break;
      case LogFlag_Error:   LogError  (SpecieDBLogHdr, 0, "%s", Buff);    break;
      case LogFlag_Warning: LogWarning(SpecieDBLogHdr, 0, "%s", Buff);    break;
      case LogFlag_Note:    LogNote   (SpecieDBLogHdr, 0, "%s", Buff);    break;
      default:
      _asm int 3;
      }
    }

  }

static void ParseAValue(CSpecie *pSp, bool &OK, double & Value, Strng & Str, flag & Specd, double Default, double Scale, LPCSTR StrValue, LPCSTR SpName, LPCSTR Name)
  {
  if (StrValue && strlen(StrValue)>0)
    {
    if (!OK)
      {
      Value=Scale*SpDB_TokenValue((LPSTR)StrValue, Default);
      Str=StrValue;
      Specd=true;
      OK=true;
      }
    else if (Str.Len()>0 && Str.XStrICmp(StrValue)!=0)
      AppendErrorMsg(pSp, LogFlag_Fatal , "%s: %s = '%s' not the same as previous definition", SpName, Name, StrValue);
    else
      AppendErrorMsg(pSp, LogFlag_Note, "%s: %s = '%s' ignored (already defined)", SpName, Name, StrValue);
    }
  }

static void ParseAnEqn(CSpecie *pSp, bool &OK, CSpDBEqn * & Eqn, Strng & Str, flag & Specd, CSpConstData * pConst, CSpFidelData * pFData, LPCSTR StrValue, LPCSTR SpName, LPCSTR Name)
  {
  if (StrValue && strlen(StrValue)>0)
    {
    if (!OK)
      {
      Eqn=CSpDBEqn::Construct(pConst, pFData, StrValue);
      Str=StrValue;
      Specd=true;
      OK=true;
      }
    else if (Str.Len()>0 && Str.XStrICmp(StrValue)!=0)
      AppendErrorMsg(pSp, LogFlag_Fatal , "%s: %s = '%s' not the same as previous definition", SpName, Name, StrValue);
    else
      AppendErrorMsg(pSp, LogFlag_Note, "%s: %s value %s ignored (already defined)", SpName, Name, StrValue);
    }
  }

flag CSpecie::LoadADO(CComponent * pC, CStringArray& Values, /*CSpecie *pPrvSp,*/ flag UseH2OFns)
  {
  m_bPrjDB=true;

  m_FData[0].m_pSp=this;
  m_FData[1].m_pSp=this;

  CSpFidelData & DataLo=m_FData[0];

  m_sPhaseSymVisible = (const char*)Values[3];
  m_sPhaseSymActual  = (const char*)Values[3];
  
  DataLo.m_dCpCv=1.4;
  Strng Phase = (const char*)Values[4];
  // work around for old projects / databases
  //if (PhaseName.XStrICmp("aq")==0 || PhaseName.XStrICmp("a")==0)
  //  Phase=PhaseName;

  switch (Phase[0])
    {
    case 'l' : m_eOcc = BOT_Liquid;  break;
    case 'v' : m_eOcc = BOT_Gas;     break;
    case 'g' : m_eOcc = BOT_Gas;     break;
    case 's' :
    default  : m_eOcc = BOT_Solid;   break;
    }
  iCId=-1;

  int ColNo=5;
  const char* pChecked = (const char*)Values[ColNo++];
  
  CString sTs(Values[ColNo++]);
  CString sTe(Values[ColNo++]);

  DataLo.m_dTs[0]=SafeAtoF(sTs, DataLo.m_dTs[0]);
  DataLo.m_dTe[0]=SafeAtoF(sTe, DataLo.m_dTe[0]);
  if (DataLo.m_dTs[0]<50.0)
    DataLo.m_dTs[0] = 50.0;
  if (DataLo.m_dTe[0]<DataLo.m_dTs[0])
    DataLo.m_dTe[0] = DataLo.m_dTs[0];
  m_dTsTol=25;
  m_dTeTol=25;

  if (UseH2OFns && ((m_eOcc==BOT_Liquid) || (m_eOcc==BOT_Gas)))
    {
    if (m_eOcc==BOT_Liquid)
      {
      if (pC->m_bLiqDone)
        return false;
      pC->m_bLiqDone=true;
      SetData(m_H2O.SelectedBlk().m_Common, m_H2O.SelectedBlk().m_Liq);
      }
    else
      {
      if (pC->m_bVapDone)
        return false;
      pC->m_bVapDone=true;
      SetData(m_H2O.SelectedBlk().m_Common, m_H2O.SelectedBlk().m_Vap);
      }
    }
  else
    {
    if (Values[ColNo].GetLength()==0)
      {
      DataLo.m_pRho[0]=NULL;
      }
    else
      {
      // Check to see if this is a Vapour and whether
      // only a constant RHo has been is specified
      Strng S((const char*)Values[ColNo]);
      if (m_eOcc==BOT_Gas)
        {
        Strng SU=S;
        SU.Upper();
        if (SU.XStrStr("IDEALGASDENSITY"))
          S.Set("IdealGasDensity(%.10g)", m_Const.m_dMoleWt);
        else if (S.XStrChr('(')==NULL)
          S.Set("LinearGasDensity(%s)", (const char*)Values[ColNo]);
        }
      DataLo.m_pRho[0]=CSpDBEqn::Construct(pC, &DataLo, S());
      }
    ColNo++;

    DataLo.m_bH25OK=(Values[ColNo].GetLength()>0);
    DataLo.m_dH25[0]=SpDB_TokenValue((char*)(const char*)Values[ColNo++], 0.0);      // Heat of formation @ 298
    DataLo.m_bS25OK=(Values[ColNo].GetLength()>0);
    DataLo.m_dS25[0]=SpDB_TokenValue((char*)(const char*)Values[ColNo++], 0.0);      // Entropy @ 298

    Strng CpS=(const char*)Values[ColNo++];
    CpS.Trim();
    DataLo.m_bCpOK = (CpS.Len()>0);

    DataLo.m_pH[0]=NULL;
    DataLo.m_pS[0]=NULL;
    DataLo.m_pCp[0]=DataLo.m_bCpOK ? CSpDBEqn::Construct(pC, &DataLo, CpS()) : NULL;
    DataLo.m_dTRef=C_2_K(25.0);

    Strng ID;
    ID.Set("%s[%s->%s]", pC->SymOrTag(), sTs, sTe);
    if (m_eOcc!=BOT_Solid)
      {
      // parsing of Vp equation moved down so that crit consts are valid
      const int VpColNo=ColNo++; 

      ParseAValue(this, m_Const.m_bPcOK, m_Const.m_dPc, m_Const.m_sPc, m_bPcSpecd,    1.0, 1000.0, (LPCSTR)Values[ColNo++], ID(), "Critical Pressure");
      ParseAValue(this, m_Const.m_bTcOK, m_Const.m_dTc, m_Const.m_sTc, m_bTcSpecd, 1000.0,    1.0, (LPCSTR)Values[ColNo++], ID(), "Critical Temp");
      ParseAValue(this, m_Const.m_bVcOK, m_Const.m_dVc, m_Const.m_sVc, m_bVcSpecd,    1.0,    1.0, (LPCSTR)Values[ColNo++], ID(), "Critical Volume");
      ParseAValue(this, m_Const.m_bAcOK, m_Const.m_dAc, m_Const.m_sAc, m_bAcSpecd,    1.0,    1.0, (LPCSTR)Values[ColNo++], ID(), "Acentricity");
      
      ParseAnEqn(this, m_Common[0].m_bVpOK, m_Common[0].m_pVp, m_Common[0].m_sVp, m_bVpSpecd, pC, NULL, (LPCSTR)Values[VpColNo], ID(), "Vapour Pressure");

      if (m_Common[0].m_bVpOK && !m_Const.m_bTcOK)
        AppendErrorMsg(this, LogFlag_Warning, "%s: Critical Temperature not defined (required for vapour pressure)", ID());
      }
    else
      {
      const int VpColNo=ColNo++; 
      if (Values[ColNo++].GetLength()>0)  LogNote(SpecieDBLogHdr, 0, "%s: %s value ignored in Solid Phase", ID(), "Critical Pressure");
      if (Values[ColNo++].GetLength()>0)  LogNote(SpecieDBLogHdr, 0, "%s: %s value ignored in Solid Phase", ID(), "Critical Temp");
      if (Values[ColNo++].GetLength()>0)  LogNote(SpecieDBLogHdr, 0, "%s: %s value ignored in Solid Phase", ID(), "Critical Volume");
      if (Values[ColNo++].GetLength()>0)  LogNote(SpecieDBLogHdr, 0, "%s: %s value ignored in Solid Phase", ID(), "Acentricity");
      if (Values[VpColNo].GetLength()>0)  LogNote(SpecieDBLogHdr, 0, "%s: %s value ignored in Solid Phase", ID(), "Vapour Pressure");
      }

    flag DummySpecd=false;

    m_Const.m_iDaType=CSpConstData::DaType_IsNull;
    if (m_eOcc==BOT_Liquid)
      {
      if (m_SDB.m_iOC_Dissociation>=0)
        {
        Strng TmpSym;
        TmpSym.Set("%s(%s)", pC->SymOrTag(), Phase);
        CSVColArray C;
        Strng TheString;
        int Quote=0;
        int N=ParseCSVFunction((LPSTR)(LPCSTR)Values[m_SDB.m_iOC_Dissociation], C, Quote);
        if (N>=2)
          {
          if (stricmp(C[0], "Ka")==0)
            {
            m_Const.m_iDaType = CSpConstData::DaType_IsAcid;
            m_Const.m_dDa[0] = SpDB_TokenValue((LPSTR)C[1], 0.0);
            m_Const.m_dDa[1] = N>=3 ? SpDB_TokenValue((LPSTR)C[2], 0.0) : 0.0;
            m_Const.m_dDa[2] = N>=4 ? SpDB_TokenValue((LPSTR)C[3], 0.0) : 0.0;
            }
          else if (stricmp(C[0], "Kb")==0)
            {
            m_Const.m_iDaType = CSpConstData::DaType_IsBase;
            m_Const.m_dDa[0] = SpDB_TokenValue((LPSTR)C[1], 0.0);
            }
          else
            LogError(SpecieDBLogHdr, 0, "Invalid Dissociation Equation for '%s' must be Ka or Kb", TmpSym());
          }
        else if (N>0)
          LogError(SpecieDBLogHdr, 0, "Invalid Dissociation Equation for '%s' must be Ka(a,b,c) or Kb(a)", TmpSym());
        }
      }

    if (m_SDB.m_iOC_MolecularDiam>=0)
      ParseAValue(NULL, m_Const.m_bMdOK, m_Const.m_dMd, m_Const.m_sMd, DummySpecd, dNAN, 1.0, (LPCSTR)Values[m_SDB.m_iOC_MolecularDiam], ID(), "Molecular Diam");
    else
      m_Const.m_dMd=dNAN;

    if (m_SDB.m_iOC_MolVol>=0)
      ParseAValue(NULL, m_Const.m_bMvOK, m_Const.m_dMv, m_Const.m_sMv, DummySpecd, 22.414, 1.0, (LPCSTR)Values[m_SDB.m_iOC_MolVol], ID(), "Molar Volume"); // 22.414 is Ideal
    else
      m_Const.m_dMv=22.414;

    if (m_SDB.m_iOC_CpCv>=0)
      DataLo.m_dCpCv=SpDB_TokenValue((char*)(const char*)Values[m_SDB.m_iOC_CpCv], 1.37);              // Cp/Cv Ratio
    else
      DataLo.m_dCpCv=1.37;
    }

  char *pRef;
  if (m_SDB.m_iOC_Reference>=0)
    pRef=(char*)(const char*)Values[m_SDB.m_iOC_Reference];
  else
    pRef=NULL;

  DataLo.m_bChecked = (pChecked!=NULL && strlen(pChecked)>0);
  m_bReference = (pRef!=NULL && strlen(pRef)>0);

  //  if (ActualData && m_sPhaseSym()!=NULL)
  //    {
  //    if (pChecked==NULL || strlen(pChecked)==0) // Checked Column
  //      LogWarning(SpecieDBLogHdr, 0, "Specie Data for %s(%s) not Checked", pC->SymOrTag(), m_sPhaseSym());
  //    }

  //VpOk=(Vpa!=0.0 || Vpb!=0.0 || Vpc!=0.0 || Vpd!=0.0);

#if dbgBuildSDB
  if (dbgBuildSpc())
    {
    dbgp("   DBLine :%-15.15s %-15.15s %-7.7s, Vp0:%-10s Vp1:%-10s",
      pC->SymOrTag(), pC->m_sElDef(), m_sPhaseSymVisible() ? m_sPhaseSymVisible() : "",
      pC->m_Common[0].m_pVp?pC->m_Common[0].m_pVp->VName():"",
      pC->m_Common[1].m_pVp?pC->m_Common[1].m_pVp->VName():"");
    for (int i=0; i<=DataLo.m_iLastTPoint; i++)
      dbgpln("%*s  T:%10.2f > %10.2f Rho:%-20s Cp:%-20s H:%-20s %08x  %08x %08x",
      i==0?0:78,"",
      K_2_C(DataLo.m_dTs[0]), K_2_C(DataLo.m_dTe[0]),
      DataLo.m_pRho[i]?DataLo.m_pRho[i]->VName():"",
      DataLo.m_pCp[i]?DataLo.m_pCp[i]->VName():"",
      DataLo.m_pH[i]?DataLo.m_pH[i]->VName():"",
      DataLo.m_pRho[i], DataLo.m_pCp[i], DataLo.m_pH[i]);
    }
#endif
  return true;
  }

//---------------------------------------------------------------------------

#define FindExistingPtr(Val, j)                               \
  for (j=0; j<=DataLo.m_iLastTPoint; j++)                     \
    {                                                         \
    if (Val[j])                                               \
      break;                                                  \
    }

//---------------------------------------------------------------------------

flag CSpecie::FixMissingData(CComponent * pC)
  {
  CSpFidelData & DataLo=m_FData[0];

  Strng S;
  int j;

  m_Common[0].m_pCmp = pC;
  m_Common[0].m_pConst = pC;

  if (m_Common[0].m_pVp==NULL)
    m_Common[0].m_pVp=CSpDBEqn::Construct(pC, &DataLo, "0.0");

  for (int i=0; i<=DataLo.m_iLastTPoint; i++)
    {
    if (DataLo.m_pRho[i]==NULL)
      {
      // Copy from other temp range ?
      FindExistingPtr(DataLo.m_pRho, j);
      if (j<=DataLo.m_iLastTPoint)
        { // Yes
        DataLo.m_pRho[i]=CSpDBEqn::Construct(pC, &DataLo, NULL, DataLo.m_pRho[j]);
        }
      else
        {
        switch (m_eOcc)
          {
          case BOT_Solid:
            DataLo.m_pRho[i]=CSpDBEqn::Construct(pC, &DataLo, "2000.0");
            break;
          case BOT_Gas:
            S.Set("IdealGasDensity(%.10g)", m_Const.MoleWt());    // PV=nrT / PV=RT
            DataLo.m_pRho[i]=CSpDBEqn::Construct(pC, &DataLo, S());
            break;
          default :
            DataLo.m_pRho[i]=CSpDBEqn::Construct(pC, &DataLo, "1000.0");
            break;
          }
        }
      }

    //    if (!fCpOK)
    if (DataLo.m_pCp[i]==NULL)
      {
      // Copy from other temp range ?
      FindExistingPtr(DataLo.m_pCp, j);
      if (j<=DataLo.m_iLastTPoint)
        { // Yes
        DataLo.m_pCp[i]=CSpDBEqn::Construct(pC, &DataLo, NULL, DataLo.m_pCp[j]);
        LogNote(SpecieDBLogHdr, 0, "Cp Data for %s(%s) copied ", pC->Sym/*OrTag*/(), m_sPhaseSymVisible());
        }
      else
        {
        switch (m_eOcc)
          {
          case BOT_Solid:  S="1.0"; break;
          case BOT_Gas: S="1.0"; break;
          default :       S="2.0"; break;
          }
        LogNote(SpecieDBLogHdr, 0, "Cp Data for %s(%s) assumed to be %s", pC->Sym/*OrTag*/(), m_sPhaseSymVisible(), S());
        pC->m_nAssumedCps++;
        DataLo.m_pCp[i]=CSpDBEqn::Construct(pC, &DataLo, S());
        }
      }


    if (DataLo.m_pRho[i]) DataLo.m_pRho[i]->m_pConst=pC;
    if (DataLo.m_pCp[i]) DataLo.m_pCp[i]->m_pConst=pC;
    if (DataLo.m_pH[i]) DataLo.m_pH[i]->m_pConst=pC;
    if (DataLo.m_pS[i]) DataLo.m_pS[i]->m_pConst=pC;
    //if (DataLo.m_pVp[i]) DataLo.m_pVp[i]->m_pFData=&DataLo;
    }


#if dbgBuildSDB
  if (dbgBuildSpc())
    {
    dbgp("  Specie: %-7.7s %08x %08x   ",//%10.2f %10.2f %10.2f %10.2f",
      m_sPhaseSymVisible() ? m_sPhaseSymVisible() : "", &m_Const, &m_Common);
    for (int i=0; i<=DataLo.m_iLastTPoint; i++)
      dbgpln("%*s @%08x T:%10.2f > %10.2f %-20s %-20s %-20s Const:%08x  %08x %08x FData:%08x  %08x %08x  ",//%08x",
      i==0?0:38,"",
      &DataLo,
      K_2_C(DataLo.m_dTs[i]), K_2_C(DataLo.m_dTe[i]),
      DataLo.m_pRho[i]?DataLo.m_pRho[i]->VName():"*Rho*",
      DataLo.m_pCp[i]?DataLo.m_pCp[i]->VName():"*Cp*",
      DataLo.m_pH[i]?DataLo.m_pH[i]->VName():"*H*",
      DataLo.m_pRho[i] ? DataLo.m_pRho[i]->m_pConst:NULL,
      DataLo.m_pCp[i] ? DataLo.m_pCp[i]->m_pConst:NULL,
      DataLo.m_pH[i] ? DataLo.m_pH[i]->m_pConst:NULL,
      DataLo.m_pRho[i] ? DataLo.m_pRho[i]->m_pFData:NULL,
      DataLo.m_pCp[i] ? DataLo.m_pCp[i]->m_pFData:NULL,
      DataLo.m_pH[i] ? DataLo.m_pH[i]->m_pFData:NULL);
    }
#endif
  return true;
  }

//---------------------------------------------------------------------------

flag CSpecie::CopyDataForMultiFidelity()
  {
  m_Common[1]=m_Common[0];

  m_FData[0].m_pSp=this;
  m_FData[1]=m_FData[0];
  m_FData[1].m_bIsCopy=true;

  for (int f=0; f<MaxFidelity; f++)
    {
    for (int i=0; i<SpDBEqnCnt; i++)
      {
      for (int j=0; j<MaxTPoints; j++)
        {
        if (m_FData[f].m_pEqns[i][j])
          m_FData[f].m_pEqns[i][j]->m_pFData=&m_FData[f];
        }
      }
    }
  flag OK=true;
  return OK;
  };

//---------------------------------------------------------------------------

const double DefaultLimitFrac = 0.8;
const double DefaultPureValue = 1000;
const bool   DefaultDoWarn    = true;

int CSpecie::AddSolnInfo(int iSoluteA, int iSoluteS, int iSolvent, char* pDensCorr, char* pCpCorr, char* pSoluble, char* pHeatOfMix)
  {
  ASSERT_ALWAYS(iSId==iSolvent, "iSId!=iSolvent", __FILE__, __LINE__);
  
  int Ret=0;
  // Test for existance
  for (int i=0; i<m_DensCorrs.GetSize(); i++)
    {
    if (m_DensCorrs[i].m_iSolute==iSoluteA)
      {
      Ret=2;
      goto DensOK;
      }
    }

  bool HasLimit=false;
  CSpDBEqn * pDensCorrFn = NULL;
  double LimitFrac  = DefaultLimitFrac;
  double PureValue  = DefaultPureValue;
  bool   DoWarn     = DefaultDoWarn;
  int Count=strlen(pDensCorr);
  if (pDensCorr && strlen(pDensCorr)>0)
    {
    Strng Str,Fn,Limit;
    int iChar=0;
    int iStart=0;
    int iParen=-1;
    while (iChar<Count)
      {
      switch (pDensCorr[iChar])
        {
        case '(': iParen=Max(1, iParen+1); break;
        case ')': iParen=iParen-1;         break;
        default:;
        }
      Str+=pDensCorr[iChar];
      if (iParen==0)
        {
        iChar++;
        Str.Trim();
        if (Str.XStrNICmp("Limit", 5)==0)
          {
          CSVColArray Cols;
          int N=ParseCSVFunction(Str(), Cols);
          if (N==4)
            {
            HasLimit=true;
            LimitFrac=SafeAtoF(Cols[1], DefaultLimitFrac);
            PureValue=SafeAtoF(Cols[2], DefaultPureValue);
            DoWarn=!(stricmp(Cols[3], "Off")==0);
            int xxx=0;
            }
          else
            Ret=4;
          }
        else
          pDensCorrFn = (pDensCorr && strlen(pDensCorr)>0) ? CSpDBEqn::Construct(&m_Const, &m_FData[0], pDensCorr) : NULL;
        Str="";
        //SKipCOmma
        while (iChar<Count && (pDensCorr[iChar]==' ' || pDensCorr[iChar]==','))
          iChar++;
        iParen=-1;
        }
      else
        iChar++;
      }
    }
  if (pDensCorrFn && !HasLimit)
    Ret=5;

  double DensCorrZero  = pDensCorrFn ? pDensCorrFn->AtT(0.0) : 0.0;

  SDB[iSoluteA].m_pDensCalc = new CDensCorr(iSoluteA, iSolvent, pDensCorrFn, DensCorrZero, LimitFrac, PureValue, DoWarn);

  m_DensCorrs.Add(CDensCorr(iSoluteA, iSolvent, pDensCorrFn, DensCorrZero, LimitFrac, PureValue, DoWarn));

DensOK:

  for (int i=0; i<m_CpCorrs.GetSize(); i++)
    if (m_CpCorrs[i].m_iSolute==iSoluteA)
      {
      Ret=2;
      goto CpOK;
      }
  CSpDBEqn * pCpCorrFn   = (pCpCorr && strlen(pCpCorr)>0) ? CSpDBEqn::Construct(&m_Const, &m_FData[0], pCpCorr) : NULL;
  m_bHasCpCorrs   = (pCpCorrFn!=NULL);
  double CpOffSet = m_bHasCpCorrs ? pCpCorrFn->AtT(0.0) : 0.0;
  //m_bHasCpCorrs   = (pCpCorrFn!=NULL); why was this line here?
  m_CpCorrs.Add(CCpCorr(iSoluteA, pCpCorrFn, CpOffSet));
CpOK:

  for (int i=0; i<m_Solubilities.GetSize(); i++)
    if (m_Solubilities[i].m_iSoluteA==iSoluteA)
      {
      Ret=2;
      goto SolOK;
      }
  CSpDBEqn * pSolubleFn  = NULL;
  if (iSoluteS>=0 && pSoluble && strlen(pSoluble)>0)
    {
    pSolubleFn = CSpDBEqn::Construct(&m_Const, &m_FData[0], pSoluble);
    m_Solubilities.Add(CSolubility(iSoluteA, iSoluteS, pSolubleFn,  NULL));
    }
SolOK:

  if (pDensCorrFn==NULL && pCpCorrFn==NULL && pSolubleFn==NULL)
    Ret=3;


  return Ret;
  }

//---------------------------------------------------------------------------

double CSpecie::DensCorrMass(double *M)
  {
  double m=M[iSId]; //m_iSolvent (eg H2O(l))
  for (int i=0; i<m_DensCorrs.GetSize(); i++)
    m+=M[m_DensCorrs[i].m_iSolute]; //+solute (eg NaCl(aq))
  return m;
  }

//---------------------------------------------------------------------------

double CSpecie::DensCorrFactor(double *M)
  {
  double C=0.0;
  for (int i=0; i<m_DensCorrs.GetSize(); i++)
    {
    CDensCorr & SI=m_DensCorrs[i];
    const double F=Range(0.0, M[SI.m_iSolute]/GTZ(M[iSId]+M[SI.m_iSolute]), 2.0);
    C+=SI.DensCorr(F);
    }
  return (1.0+C);
  }

//---------------------------------------------------------------------------

double CSpecie::CpCorrFactor(double *M)
  {
  double C=0.0;
  for (int i=0; i<m_CpCorrs.GetSize(); i++)
    {
    CCpCorr & SI=m_CpCorrs[i];
    const double F=Range(0.0, M[SI.m_iSolute]/GTZ(M[iSId]+M[SI.m_iSolute]), 2.0);
    C+=SI.CpCorr(F);
    }
  return (1.0+C);
  };

//---------------------------------------------------------------------------

double CSpecie::mlHs(long Fidelity, double T, double P, SpPropOveride *Ovr, double *M)
  {
  if (Ovr && (Ovr->bUse & m_bCpOvrMask))
    return MoleWt()*Ovr->fCp[m_iOverIndex]*(T-ZeroCinK);
  double X=0;
  if (!m_bCpInDirect)
    {
    int i=FindTIndex(Fidelity, T);
    X=(m_FData[Fidelity].m_pH[i] ? m_FData[Fidelity].m_pH[i]->AtTP(T, P) : m_FData[Fidelity].m_pCp[i]->Integrate_dT(m_FData[Fidelity].m_dTs[i], T, P));
    if (M && m_bHasCpCorrs)
      X*=CpCorrFactor(M);
    }
  X+=m_FData[Fidelity].m_dCZRef[FindTIndex(Fidelity, T)];
  return X;
  };

//---------------------------------------------------------------------------

double CSpecie::mlHz(long Fidelity, double T, double P, SpPropOveride *Ovr, double *M)
  {
  if (Ovr && (Ovr->bUse & m_bCpOvrMask))
    return MoleWt()*Ovr->fCp[m_iOverIndex]*(T-ZeroCinK);
  double X=0;
  if (!m_bCpInDirect)
    {
    X=m_FData[Fidelity].mlHf(T, P);
    if (M && m_bHasCpCorrs)
      X*=CpCorrFactor(M);
    }
  X+=m_FData[Fidelity].m_dCdHf[FindTIndex(Fidelity, T)];
  return X;
  };

//---------------------------------------------------------------------------

double CSpecie::mlHf(long Fidelity, double T, double P, SpPropOveride *Ovr, double *M)
  {
  if (Ovr && (Ovr->bUse & m_bCpOvrMask))
    return m_FData[Fidelity].m_dCHf[m_FData[Fidelity].m_iHfBase]+MoleWt()*Ovr->fCp[m_iOverIndex]*(T-C_2_K(0));
  double X=0;
  if (!m_bCpInDirect)
    {
    X=m_FData[Fidelity].mlHf(T, P);
    if (M && m_bHasCpCorrs)
      X*=CpCorrFactor(M);
    }
  X+=m_FData[Fidelity].m_dCHf[FindTIndex(Fidelity, T)];
  return X;
  };

//---------------------------------------------------------------------------

double CSpecie::mlCp(long Fidelity, double T, double P, SpPropOveride *Ovr, double *M)
  {
  if (m_bCpInDirect)
    return 0.0;
  if (Ovr && (Ovr->bUse & m_bCpOvrMask))
    return MoleWt()*Ovr->fCp[m_iOverIndex];
  double X=m_FData[Fidelity].mlCp(T, P);
  if (M && m_bHasCpCorrs)
    X*=CpCorrFactor(M);
  return X;
  };

//---------------------------------------------------------------------------

double CSpecie::mlSf(long Fidelity, double T, double P, SpPropOveride *Ovr, double *M)
  {
  if (m_bCpInDirect)
    return 0.0;
  int i=FindTIndex(Fidelity, T);
  double X=m_FData[Fidelity].mlSf(T,P);
  if (M && m_bHasCpCorrs)
    X*=CpCorrFactor(M);
  X+=m_FData[Fidelity].m_dCSf[i];
  return X;
  };

//---------------------------------------------------------------------------

double CSpecie::mlGf(long Fidelity, double T, double P, SpPropOveride *Ovr, double *M)
  {
  return mlHf(Fidelity, T,P,Ovr,M)-mlSf(Fidelity, T,P,Ovr,M)*T;
  };

//---------------------------------------------------------------------------

double CSpecie::mlCpMean(long Fidelity, double T, double P, double T0, double P0, SpPropOveride *Ovr, double *M)
  {
  DoBreak();
  if (m_bCpInDirect)
    return 0.0;
  if (Ovr && (Ovr->bUse & m_bCpOvrMask))
    return MoleWt()*Ovr->fCp[m_iOverIndex];
  int i=FindTIndex(Fidelity, T);
  double X=m_FData[Fidelity].m_pCp[i]->AtTP(T, P);
  if (M && m_bHasCpCorrs)
    X*=CpCorrFactor(M);
  return X;
  };

// ==========================================================================
//
//
//
// ==========================================================================

CComponent::CComponent(CSpecieDataBase & m_SDB, CComponentDataBase & CDB, CH2ODataBase & H2ODB, const char * Sym__) : \
m_SDB(SDB),
m_CDB(CDB),
m_H2O(H2ODB),
m_ElComp(*(new ElemCompArray))
  {
  Sym=Sym__;
  if (m_CDB.No() == MaxComponents)
    {
    LogWarning(SpecieDBLogHdr, 0, "Too Many Species %s", SymOrTag());
    return;
    }

  for (int i=0; i<MaxPhases; i++)
    {
    m_SpPhaseD[i] = NULL;
    m_PhaseData[i] = NULL;
    m_iPhaseData[i] = -1;
    }

  m_nPhaseData=0;
  m_nAssumedCps=0;
  m_nDiscarded=0;
  m_bLiqDone=false;
  m_bVapDone=false;
  };

//---------------------------------------------------------------------------

CComponent::~CComponent()
  {
  for (int i=0; i<m_nPhaseData; i++)
    delete m_PhaseData[i];
  delete &m_ElComp;
  };

//---------------------------------------------------------------------

char * CComponent::SymOrTag()
  {
  return Sym();
  //return Sym.Length()>0 ||  ? Sym(): xTag();
  };

//---------------------------------------------------------------------

int CComponent::FindPhaseDataIndex(char *pPhaseName)
  {
  if (m_nPhaseData==0)
    return -1;
  for (int i=m_nPhaseData-1; i>0; i--)
    if (strcmp(pPhaseName, m_PhaseData[i]->m_sPhaseSymVisible())==0)
      break;
  return i;
  };

//-----------------------------------------------------------------------

int CComponent::SolPhInx(LPCSTR PhSym, bool OtherwiseFirst)
  {
  for (int i=m_CDB.m_ph_SolS; i<=m_CDB.m_ph_SolE; i++)
    if (m_SpPhaseD[i] && CDB.m_PhInfo[i].m_Sym.XStrCmp(PhSym)==0)
      return m_SpPhaseD[i]->iSId;
  if (OtherwiseFirst)
    {
    for (int i=m_CDB.m_ph_SolS; i<=m_CDB.m_ph_SolE; i++)
      if (m_SpPhaseD[i])
        return m_SpPhaseD[i]->iSId;
    }
  return -1;
  }
int CComponent::LiqPhInx(LPCSTR PhSym, bool OtherwiseFirst)
  {
  for (int i=m_CDB.m_ph_LiqS; i<=m_CDB.m_ph_LiqE; i++)
    if (m_SpPhaseD[i] && CDB.m_PhInfo[i].m_Sym.XStrCmp(PhSym)==0)
      return m_SpPhaseD[i]->iSId;
  if (OtherwiseFirst)
    {
    for (int i=m_CDB.m_ph_LiqS; i<=m_CDB.m_ph_LiqE; i++)
      if (m_SpPhaseD[i])
        return m_SpPhaseD[i]->iSId;
    }
  return -1;
  }
int CComponent::VapPhInx(LPCSTR PhSym, bool OtherwiseFirst)
  {
  for (int i=m_CDB.m_ph_GasS; i<=m_CDB.m_ph_GasE; i++)
    if (m_SpPhaseD[i] && CDB.m_PhInfo[i].m_Sym.XStrCmp(PhSym)==0)
      return m_SpPhaseD[i]->iSId;
  if (OtherwiseFirst)
    {
    for (int i=m_CDB.m_ph_GasS; i<=m_CDB.m_ph_GasE; i++)
      if (m_SpPhaseD[i])
        return m_SpPhaseD[i]->iSId;
    }
  return -1;
  }

// ==========================================================================
//
//
//
// ==========================================================================

#if WITHDEBUGDUMPENTHALPY
int CSpecieDataBase::DebugHDumpOn=0;
#endif
#ifndef _RELEASE
int CSpecieDataBase::gs_nDoTestEstTP=10;
#endif

CSpecieDataBase::CSpecieDataBase()
  {
  Init();
  };

//---------------------------------------------------------------------------

CSpecieDataBase::~CSpecieDataBase()
  {
  Term();
  };

//---------------------------------------------------------------------------

void CSpecieDataBase::Init()
  {
  m_iNo=0;
  //m_iLastSkipList=0;
  m_DensCorrSps.SetSize(0);
  m_CpCorrSps.SetSize(0);
  m_SolubleSps.SetSize(0);
  //  Term();
  m_pExtraProps=NULL;
  m_iOC_Dissociation=-1;
  m_iOC_MolecularDiam=-1;
  m_iOC_MolVol=-1;
  m_iOC_CpCv=-1;
  m_iOC_Reference=-1;
  m_iH2Ol=-1;
  m_iCO2l=-1;
  m_iH2Og=-1;
  m_iCO2g=-1;
#ifndef _RELEASE
  CSpecieDataBase::gs_nDoTestEstTP=0;
#endif
  };

//---------------------------------------------------------------------------

void CSpecieDataBase::Term()
  {
  m_iNo=0;
  m_DensCorrSps.SetSize(0);
  m_CpCorrSps.SetSize(0);
  m_SolubleSps.SetSize(0);
  m_Map.RemoveAll();
  };

//---------------------------------------------------------------------------

int CSpecieDataBase::GetIndexArray(PhMask Phases, CSVLongArray*pInx/*=NULL*/)
  {
  int N=0;
  for (int s=0; s<m_iNo; s++)
    if (m_SpD[s]->m_PhMsk & Phases)
      N++;

  if (pInx)
    {
    int j=0;
    pInx->SetSize(N);
    for (int s=0; s<m_iNo; s++)
      if (m_SpD[s]->m_PhMsk & Phases)
        (*pInx)[j++]=s;
    ASSERT(j==N);
    }
  return N;
  };

//---------------------------------------------------------------------------

void CSpecieDataBase::Add(CSpecie * SD)
  {
  if (m_iNo == MaxSpecies)
    {
    LogWarning(SpecieDBLogHdr, 0, "Too Many Species %s", SD->SymOrTag());
    //return 0;
    }

  m_SpD[m_iNo] = SD;
  SD->iSId=m_iNo;
  m_iNo++;
  };

//--------------------------------------------------------------------------

int CSpecieDataBase::Find(const char * pSpName)
  {
  if (pSpName)
    for (word i=0; i<m_iNo; i++)
      if (strcmp(m_SpD[i]->SymOrTag(), pSpName)==0)
        return i;
  return -1;
  };

//--------------------------------------------------------------------------

void CSpecieDataBase::AddSpCnv(CCnvIndex DC_Family, char * pSpName, char * CnvRqd, CCnvIndex & dc, pchar & pCnvTxt)
  {
  if (gs_WithSpecieCnvs)
    {
    CDataCnv* pE=Cnvs[DC_Family];

    Strng PCnv;
    PCnv.Set("%s.%s", pE->Fam(), pSpName);
    dc=gs_CnvsMngr.FindCnv(PCnv());
    if (dc<0)
      {//create new "extended" set of cnvs
      Strng SCnv;
      SCnv.Set("%s.%s", pE->Txt(), pSpName);
      dc=gs_CnvsMngr.AddPri(PCnv(), SCnv(), 0.0, 1000.0, pE->Desc(), true);
      gs_CnvsMngr.AddSec(dc, pE->Txt(), pE->SclValue(), pE->OffValue(), true); //add 'original' SI unit as is
      for (pE=pE->Next(); pE; pE=pE->Next())
        {
        SCnv.Set("%s.%s", pE->Txt(), pSpName);
        gs_CnvsMngr.AddSec(dc, SCnv(), pE->SclValue(), pE->OffValue(), true);
        gs_CnvsMngr.AddSec(dc, pE->Txt(), pE->SclValue(), pE->OffValue(), true); //add 'original' as is with no extension
        }
      }

    pCnvTxt=Cnvs[dc]->Txt();
    if (strlen(CnvRqd)>0)
      {
      PCnv.Set("%s.%s", CnvRqd, pSpName);
      for (pE=Cnvs[dc]; pE; pE=pE->Next())
        if (PCnv.XStrCmp(pE->Txt())==0)
          {
          pCnvTxt=pE->Txt();
          break;
          }
      }
    }
  else
    {
    dc = DC_Family;
    pCnvTxt=Cnvs[dc]->Txt();
    if (strlen(CnvRqd)>0)
      {
      Strng PCnv(CnvRqd);
      CDataCnv* pE;
      for (pE=Cnvs[dc]; pE; pE=pE->Next())
        if (PCnv.XStrCmp(pE->Txt())==0)
          {
          pCnvTxt=pE->Txt();
          break;
          }
      }
    }
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::Density(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double m=0.0;
  double v=0.0;
  SpecieIter I(Phase);

  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      double vv=m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
      m += M[i];
      v += M[i]*vv;
      }
    }

  return (m >= 1.0e-100/*TraceMass*/) ? m/GTZ(v) : DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::Density(long Fidelity, CIArray & SpIds, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double m=0.0;
  double v=0.0;

  int i;
  for (int j=0; j<SpIds.GetSize() && (i=SpIds[j])>=0; j++)
    {
    if (M[i] > MeasTolerance)
      {
      const double vv=m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
      m += M[i];
      v += M[i]*vv;
      }
    }

  return (m >= 1.0e-100/*TraceMass*/) ? m/GTZ(v) : DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::Density(long Fidelity, CSysVector & Scale, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double m=0.0;
  double v=0.0;
  SpecieIter I(som_ALL);

  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      const double vv=m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
      m += Scale[i]*M[i];
      v += Scale[i]*M[i]*vv;
      }
    }

  return (m >= 1.0e-100/*TraceMass*/) ? m/GTZ(v) : DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::msVolume(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double m=0.0;
  double v=0.0;
  SpecieIter I(Phase);

  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      m += M[i];
      v += M[i]*m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
      }
    }

  return (m >= 1.0e-100/*TraceMass*/) ? v/GTZ(m) : 1.0/DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::msVolumeFrac(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, double *M, double *Frac)
  {
  double m=0.0;
  double v=0.0;

  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance) 
      {
      m += M[i];
      Frac[i] = M[i]*m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
      v += Frac[i];
      }
    }

  if (m >= 1.0e-100)
    {
    v=GTZ(v);
    for (int i=-1; I.Loop(i); )
      //if (m_SpD[i]->DensityDirect())
      Frac[i] /= v;
    return v/GTZ(m);
    }

  for (i=-1; I.Loop(i); )
    //if (m_SpD[i]->DensityDirect())
    Frac[i] = 0.0;
  return 1.0/DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::Volume(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double v=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      v += M[i]*m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
    }
  return v;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::Volume(long Fidelity, CIArray & SpIds, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double v=0.0;
  int i;
  for (int j=0; j<SpIds.GetSize() && (i=SpIds[j])>=0; j++)
    {
    if (M[i] > MeasTolerance)
      v += M[i]*m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
    }
  return v;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::Volume(long Fidelity, CSysVector &Scale, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double v=0.0;
  SpecieIter I(som_ALL);
  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance) 
      v += Scale[i]*M[i]*m_SpD[i]->msVolume(Fidelity, T, P, Ovr, M);
    }
  return v;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::NDensity(long Fidelity, PhMask Phase, SpPropOveride *Ovr, double *M)
  {
  double m=0.0;
  double v=0.0;
  SpecieIter I(Phase);

  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      m += M[i];
      v += M[i]*m_SpD[i]->msVolume(Fidelity, Norm_T, Norm_P, Ovr, M);
      }
    }
  
  return (m >= 1.0e-100/*TraceMass*/) ? m/GTZ(v) : DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::NDensity(long Fidelity, CSysVector & Scale, SpPropOveride *Ovr, double *M)
  {
  double m=0.0;
  double v=0.0;
  SpecieIter I(som_ALL);

  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      m += Scale[i]*M[i];
      v += Scale[i]*M[i]*m_SpD[i]->msVolume(Fidelity, Norm_T, Norm_P, Ovr, M);
      }
    }
  return (m >= 1.0e-100/*TraceMass*/) ? m/GTZ(v) : DefaultRho;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::NVolume(long Fidelity, PhMask Phase, SpPropOveride *Ovr, double *M)
  {
  double v=0.0;
  SpecieIter I(Phase);

  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      v += M[i]*m_SpD[i]->msVolume(Fidelity, Norm_T, Norm_P, Ovr, M);
      }
    }
  return v;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::NVolume(long Fidelity, CIArray & SpIds, SpPropOveride *Ovr, double *M)
  {
  double v=0.0;
  int i;
  for (int j=0; j<SpIds.GetSize() && (i=SpIds[j])>=0; j++)
    {
    if (M[i] > MeasTolerance)
      {
      v += M[i]*m_SpD[i]->msVolume(Fidelity, Norm_T, Norm_P, Ovr, M);
      }
    }
  return v;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::NVolume(long Fidelity, CSysVector & Scale, SpPropOveride *Ovr, double *M)
  {
  double v=0.0;
  SpecieIter I(som_ALL);
  for (int i=-1; I.Loop(i); )
    {
    if (M[i] > MeasTolerance)
      {
      v += Scale[i]*M[i]*m_SpD[i]->msVolume(Fidelity, Norm_T, Norm_P, Ovr, M);
      }
    }
  return v;
  };

//--------------------------------------------------------------------------

double CSpecieDataBase::VolFrac(long Fidelity, PhMask Phase, double T, double P, SpPropOveride *Ovr, double *M)
  {
  double t=SDB.Volume(Fidelity, som_ALL, T, P, Ovr, M);
  double s=SDB.Volume(Fidelity, Phase, T, P, Ovr, M);
  return s/GTZ(t);
  };

// ==========================================================================
//
//
//
// ==========================================================================

//--------------------------------------------------------------------------

inline double SumMass(PhMask Phase, double *M)
  {
  double m=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    m += M[i];
  return m;
  };

double CSpecieDataBase::CpCv(long Fidelity, PhMask Phase, double *M)
  {
  double c=0.0;
  double m=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    if (M[i] > MeasTolerance)
      {
      m += M[i];
      c += M[i]*SDB[i].CpCv(Fidelity);
      }
    return c/GTZ(m);
  };

// --------------------------------------------------------------------------

double CSpecieDataBase::totCp(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  double Et=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      Et+=M[i]*SDB[i].msCp(Fidelity, T_, P_, Ovr, M);
  return Et;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msCp(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M, double *pTotalM)
  {
  double Et=0.0, Mt=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      double e=SDB[i].msCp(Fidelity, T_, P_, Ovr, M);
      Et+=M[i]*e;
      Mt+=M[i];
      }
    }
  if (pTotalM)
    *pTotalM=Mt;
  return Et/GTZ(Mt);
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::totHs(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  double Et=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      Et+=M[i]*SDB[i].msHs(Fidelity, T_, P_, Ovr, M);
  return Et;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::totHz(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  double Et=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      Et+=M[i]*SDB[i].msHz(Fidelity, T_, P_, Ovr, M);
  return Et;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msHs(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M, double *pTotalM)
  {
  double Et=0.0, Mt=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Et+=M[i]*SDB[i].msHs(Fidelity, T_, P_, Ovr, M);
      Mt+=M[i];
      }
    }
  if (pTotalM)
    *pTotalM=Mt;
  return Et/GTZ(Mt);
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msHz(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M, double *pTotalM)
  {
//CStopWatch SW;
//SW.Start();
  double Et=0.0, Mt=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Et+=M[i]*SDB[i].msHz(Fidelity, T_, P_, Ovr, M);
      Mt+=M[i];
      }
    }
  if (pTotalM)
    *pTotalM=Mt;
//dbgpln("msHz                                                  %10.2f", SW.LapTime()*1e6);
  return Et/GTZ(Mt);
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::totHf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
//CStopWatch SW;
//SW.Start();
  double Et=0.0;
  SpecieIter I(Phase);
#if WITHDEBUGDUMPENTHALPY
  if (DebugHDumpOn)
    {
    double Mt=0;
    for (int i=-1; I.Loop(i); )
      if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
        Mt+=M[i];
    dbgpln("totHF: T:%10.3f P:%10.3f M:%10.3f", T_, P_, Mt);
    }
#endif
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
#if WITHDEBUGDUMPENTHALPY
      if (DebugHDumpOn)
        dbgpln("     :                                          %10.3g %15.6f %s",
        M[i],SDB[i].msHf(Fidelity, T_, P_, Ovr, M),SDB[i].SymOrTag());
#endif
      Et+=M[i]*SDB[i].msHf(Fidelity, T_, P_, Ovr, M);
      int xxx=0;
      }
    }
//dbgpln("totHf                                                 %10.2f", SW.LapTime()*1e6);
  return Et;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msHf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M, double *pTotalM)
  {
  double Et=0.0, Mt=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Et+=M[i]*SDB[i].msHf(Fidelity, T_, P_, Ovr, M);
      Mt+=M[i];
      }
    }
  if (pTotalM)
    *pTotalM=Mt;
  return Et/GTZ(Mt);
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::totSf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  double Et=0.0;
  SpecieIter I(Phase);
#if WITHDEBUGDUMPENTHALPY
  if (DebugHDumpOn)
    {
    double Mt=0;
    for (int i=-1; I.Loop(i); )
      if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
        Mt+=M[i];
    dbgpln("totSf: T:%10.3f P:%10.3f M:%10.3f", T_, P_, Mt);
    }
#endif
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
#if WITHDEBUGDUMPENTHALPY
      if (DebugHDumpOn)
        dbgpln("     :                                          %10.3f %15.6f %s",
        M[i],SDB[i].msSf(Fidelity, T_, P_, Ovr, M),SDB[i].SymOrTag());
#endif
      Et+=M[i]*SDB[i].msSf(Fidelity, T_, P_, Ovr, M);
      }
    }
  return Et;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msSf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M, double *pTotalM)
  {
  double Et=0.0, Mt=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Et+=M[i]*SDB[i].msSf(Fidelity, T_, P_, Ovr, M);
      Mt+=M[i];
      }
    }
  if (pTotalM)
    *pTotalM=Mt;
  return Et/GTZ(Mt);
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::totGf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  double Et=0.0;
  SpecieIter I(Phase);
#if WITHDEBUGDUMPENTHALPY
  if (DebugHDumpOn)
    {
    double Mt=0;
    for (int i=-1; I.Loop(i); )
      if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
        Mt+=M[i];
    dbgpln("totGf: T:%10.3f P:%10.3f M:%10.3f", T_, P_, Mt);
    }
#endif
  for (int i=-1; I.Loop(i); )
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
#if WITHDEBUGDUMPENTHALPY
      if (DebugHDumpOn)
        dbgpln("     :                                          %10.3f %15.6f %s",
        M[i],SDB[i].msGf(Fidelity, T_, P_, Ovr, M),SDB[i].SymOrTag());
#endif
      Et+=M[i]*SDB[i].msGf(Fidelity, T_, P_, Ovr, M);
      }
    return Et;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msGf(long Fidelity, PhMask Phase, double T_, double P_, SpPropOveride *Ovr, double *M, double *pTotalM)
  {
  double Et=0.0, Mt=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    {
    if ((M[i] > MeasTolerance) && SDB[i].CpDirect())
      {
      Et+=M[i]*SDB[i].msGf(Fidelity, T_, P_, Ovr, M);
      Mt+=M[i];
      }
    }
  if (pTotalM)
    *pTotalM=Mt;
  return Et/GTZ(Mt);
  }

// --------------------------------------------------------------------------

//double CSpecieDataBase::msHsMix(double FSol, double Hs, double FLiq, double Hl, double FVap, double Hv, double T_, double P_, SpPropOveride *Ovr, double *M)
//  {
//  if (Ovr)
//    {
//    if (Ovr->bUse & SPO_SCp)
//      Hs= Ovr->fSCp * (T_-ZeroCinK);
//    if (Ovr->bUse & SPO_LCp)
//      Hl= Ovr->fLCp * (T_-ZeroCinK);
//    if (Ovr->bUse & SPO_VCp)
//      Hv= Ovr->fVCp * (T_-ZeroCinK);
//    }
//  if (!Valid(Hs))
//    Hs=msHz(som_Sol, T_, P_, Ovr, M);
//  if (!Valid(Hl))
//    Hl=msHz(som_Liq, T_, P_, Ovr, M);
//  if (!Valid(Hv))
//    Hv=msHz(som_Gas, T_, P_, Ovr, M);
//
//  return FSol*Hs+FLiq*Hl+FVap*Hv;
//  }

double CSpecieDataBase::msHmMix(long Fidelity, double Fm, double Hm, double Fo, double *Mo, double T_, double P_, SpPropOveride *Ovr)
  {
  double Ho=0.0;
  if (Fo>1.0e-9)
    {
    //Mo.Normalise();
    if (Ovr)
      {
      double Hs = (Ovr->bUse & SPO_SCp) ? Ovr->fSCp * (T_-ZeroCinK) : msHz(Fidelity, som_Sol, T_, P_, Ovr, Mo);
      double Hl = (Ovr->bUse & SPO_LCp) ? Ovr->fLCp * (T_-ZeroCinK) : msHz(Fidelity, som_Liq, T_, P_, Ovr, Mo);
      double Hv = (Ovr->bUse & SPO_VCp) ? Ovr->fVCp * (T_-ZeroCinK) : msHz(Fidelity, som_Gas, T_, P_, Ovr, Mo);
      Ho=Hs+Hl+Hv;
      }
    else
      Ho=msHz(Fidelity, som_ALL, T_, P_, Ovr, Mo);
    }
  return Fm*Hm+Fo*Ho;
  }
// --------------------------------------------------------------------------

double CSpecieDataBase::msHmMix(long Fidelity, double FSol, double Hs, double FLiq, double Hl, double FVap, double Hv, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  //ASSERT(FSol+FLiq+FVap==1.0)
  if (Ovr)
    {
    if (Ovr->bUse & SPO_SCp)
      Hs = Ovr->fSCp * (T_-ZeroCinK);
    if (Ovr->bUse & SPO_LCp)
      Hl = Ovr->fLCp * (T_-ZeroCinK);
    if (Ovr->bUse & SPO_VCp)
      Hv = Ovr->fVCp * (T_-ZeroCinK);
    }

  double d=0.0;
  if (FSol>FracMeasTolerance)
    {
    if (!Valid(Hs))
      Hs=msHz(Fidelity, som_Sol, T_, P_, Ovr, M);
    d += FSol*Hs;
    }
  if (FLiq>FracMeasTolerance)
    {
    if (!Valid(Hl))
      Hl=msHz(Fidelity, som_Liq, T_, P_, Ovr, M);
    d += FLiq*Hl;
    }
  if (FVap>FracMeasTolerance)
    {
    if (!Valid(Hv))
      Hv=msHz(Fidelity, som_Gas, T_, P_, Ovr, M);
    d += FVap*Hv;
    }

  return d;
  //return FSol*Hs + FLiq*Hl + FVap*Hv;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::msCpMix(long Fidelity, double FSol, double Cs, double FLiq, double Cl, double FVap, double Cv, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  //ASSERT(FSol+FLiq+FVap==1.0)
  if (Ovr)
    {
    if (Ovr->bUse & SPO_SCp)
      Cs = Ovr->fSCp;
    if (Ovr->bUse & SPO_LCp)
      Cl = Ovr->fLCp;
    if (Ovr->bUse & SPO_VCp)
      Cv = Ovr->fVCp;
    }

  double d=0.0;
  if (FSol>FracMeasTolerance)
    {
    if (!Valid(Cs))
      Cs=msCp(Fidelity, som_Sol, T_, P_, Ovr, M);
    d += FSol*Cs;
    }
  if (FLiq>FracMeasTolerance)
    {
    if (!Valid(Cl))
      Cl=msCp(Fidelity, som_Liq, T_, P_, Ovr, M);
    d += FLiq*Cl;
    }
  if (FVap>FracMeasTolerance)
    {
    if (!Valid(Cv))
      Cv=msCp(Fidelity, som_Gas, T_, P_, Ovr, M);
    d += FVap*Cv;
    }

  return d;
  //return FSol*Cs + FLiq*Cl + FVap*Cv;
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::RhoMix(long Fidelity, double FSol, double Ds, double FLiq, double Dl, double FVap, double Dv, double T_, double P_, SpPropOveride *Ovr, double *M)
  {
  //ASSERT(FSol+FLiq+FVap==1.0)
  if (Ovr)
    {
    if (Ovr->bUse & SPO_SDens)
      Ds = Ovr->fSDens;
    if (Ovr->bUse & SPO_LDens)
      Dl = Ovr->fLDens;
    if (Ovr->bUse & SPO_VDens)
      Dv = Ovr->fVDens;
    }

  double d=0.0;
  if (FSol>FracMeasTolerance)
    {
    if (!Valid(Ds))
      {
      double SumG=0.0, SumM=0.0;
      SpecieIter I(som_Sol);
      for (int sn=-1; I.Loop(sn); )
        {
        SumG += M[sn]*SDB[sn].msVolume(Fidelity, T_, P_, Ovr, M);
        SumM += M[sn];
        }
      Ds = SumM/GTZ(SumG);
      }
    d += FSol/GTZ(Ds);
    }

  if (FLiq>FracMeasTolerance)
    {
    if (!Valid(Dl))
      {
      double SumG=0.0, SumM=0.0;
      SpecieIter I(som_Liq);
      for (int sn=-1; I.Loop(sn); )
        {
        SumG += M[sn]*SDB[sn].msVolume(Fidelity, T_, P_, Ovr, M);
        SumM += M[sn];
        }
      Dl = SumM/GTZ(SumG);
      }
    d += FLiq/GTZ(Dl);
    }

  if (FVap>FracMeasTolerance)
    {
    if (!Valid(Dv))
      {
      double SumG=0.0, SumM=0.0;
      SpecieIter I(som_Gas);
      for (int sn=-1; I.Loop(sn); )
        {
        SumG += M[sn]*SDB[sn].msVolume(Fidelity, T_, P_, Ovr, M);
        SumM += M[sn];
        }
      Dv = SumM/GTZ(SumG);
      }
    d += FVap/GTZ(Dv);
    }

  return 1.0/GTZ(d);
  //return 1.0/GTZ(FSol/GTZ(Ds)+FLiq/GTZ(Dl)+FVap/GTZ(Dv));
  }

// --------------------------------------------------------------------------

double CSpecieDataBase::HeatOfCombustionHi(long Fidelity, PhMask Phase, double T_, double P_, double *M)
  {
  const long iH = 1-1;
  const long iC = 6-1;

  int Ns,i;
  CompFracArray &CFH=*(EDB[iH].m_SpComp);
  Ns=CFH.GetSize();
  double MH=0;
  for (i=0; i<Ns; i++)
    {
    int s=CFH[i].iSpNo;
    if (s!=m_iH2Ol && s!=m_iH2Og && SDB[s].PhaseIs(Phase))
      MH+=M[s]*CFH[i].dMassFrac;
    }

  CompFracArray &CFC=*(EDB[iC].m_SpComp);
  Ns=CFC.GetSize();
  double MC=0;
  for (i=0; i<Ns; i++)
    {
    int s=CFC[i].iSpNo;
    if (s!=m_iCO2l && s!=m_iCO2g && SDB[s].PhaseIs(Phase))
      MC+=M[s]*CFC[i].dMassFrac;
    }

  const double aH=1.00794;
  const double aO=15.9994;
  const double aC=12.011;
  double HfH20l=(m_iH2Ol>=0) ? SDB[m_iH2Ol].msHf(Fidelity, T_, P_, NULL, NULL) : -15866.64 + (T_-Std_T) * 4.1820;
  double HfCO2g=(m_iCO2g>=0) ? SDB[m_iCO2g].msHf(Fidelity, T_, P_, NULL, NULL) : -8941.44 + (T_-Std_T) * 0.8323;

  double HV=-(MH*(HfH20l)*((2*aH+aO)/(2*aH))+MC*(HfCO2g)*((aC+2*aO)/(aC)));
  return HV;
  };


// --------------------------------------------------------------------------

double CSpecieDataBase::HeatOfCombustionLo(long Fidelity, PhMask Phase, double T_, double P_, double *M)
  {
  const long iH = 1-1;
  const long iC = 6-1;

  int Ns,i;
  CompFracArray &CFH=*(EDB[iH].m_SpComp);
  Ns=CFH.GetSize();
  double MH=0;
  for (i=0; i<Ns; i++)
    {
    int s=CFH[i].iSpNo;
    if (s!=m_iH2Ol && s!=m_iH2Og && SDB[s].PhaseIs(Phase))
      MH+=M[s]*CFH[i].dMassFrac;
    }

  CompFracArray &CFC=*(EDB[iC].m_SpComp);
  Ns=CFC.GetSize();
  double MC=0;
  for (i=0; i<Ns; i++)
    {
    int s=CFC[i].iSpNo;
    if (s!=m_iCO2l && s!=m_iCO2g && SDB[s].PhaseIs(Phase))
      MC+=M[s]*CFC[i].dMassFrac;
    }

  const double aH=1.00794;
  const double aO=15.9994;
  const double aC=12.011;
  double HfH20g=(m_iH2Og>=0) ? SDB[m_iH2Og].msHf(Fidelity, T_, P_, NULL, NULL) : -13451.47 + (T_-Std_T) * 2.0774;
  double HfCO2g=(m_iCO2g>=0) ? SDB[m_iCO2g].msHf(Fidelity, T_, P_, NULL, NULL) : -8941.44 + (T_-Std_T) * 0.8323;

  double HV=-(MH*(HfH20g)*((2*aH+aO)/(2*aH))+MC*(HfCO2g)*((aC+2*aO)/(aC)));

  return HV;
  };

// ==========================================================================
//
//
//
// ==========================================================================

void CH2ODataBase::Initialise()
  {
  m_iSelected=-1;
  for (int i=0; i<m_nMdls; i++)
    m_Mdl[i]->Destroy();

  for (m_nMdls=0; m_nMdls<MaxH2OMdls; )
    {
    TagObjClass *pC=TagObjClass::FindClassIndexed(H2OPropsGroup, m_nMdls);
    if (pC)
      {
      m_Mdl[m_nMdls++]=(H2OPropertiesBlock*)pC->Construct(NULL, "Lcl", NULL, TOA_Free);
      }
    else
      break;
    }
  };

void CH2ODataBase::Terminate()
  {
  for (int i=0; i<m_nMdls; i++)
    {
    //delete m_Mdl[i];//->Destroy();
    m_Mdl[i]->Destroy();
    m_Mdl[i]=NULL;
    }
  m_nMdls=0;
  };

DDBValueLst  * CH2ODataBase::DDBList()
  {
  static DDBValueLstMem DDB;
  TagObjClass::GetClassValueLst(H2OPropsGroup, DDB);
  //TagObjClass::GetSDescValueLst(H2OPropsGroup, DDB);
  return DDB.List();
  };

void CH2ODataBase::Select(long l)
  {
  m_iSelected=Range(0l, l, m_nMdls-1l);
  };

void CH2ODataBase::Select(const char * ClassId)
  {
  m_iSelected=-1;
  for (int i=0; i<m_nMdls; i++)
    if (_stricmp(m_Mdl[i]->ClassId(), ClassId)==0)
      {
      m_iSelected=i;
      break;
      }
    //if not selected use latest
    if (m_iSelected<0)
      Select(IF97H2OName);
  };

long CH2ODataBase::Selected()
  {
  return m_iSelected;
  };

char* CH2ODataBase::SelectedClassId()
  {
  if (m_iSelected>=0 && m_iSelected<m_nMdls)
    return m_Mdl[m_iSelected]->ClassId();
  return "";
  };

H2OPropertiesBlock & CH2ODataBase::SelectedBlk()
  {
  return *m_Mdl[m_iSelected];
  };

// ==========================================================================
//
//
//
// ==========================================================================

DDBValueLst  * CComponentDataBase::H2ODDBList()
  {
  return m_H2O.DDBList();
  };

void CComponentDataBase::SelectH2OProperties(long l)
  {
  m_H2O.Select(l);
  H2OPropertiesBlock & Blk=m_H2O.SelectedBlk();
  if (m_pWater)
    m_pWater->SetData(Blk.m_Common, Blk.m_Liq);
  if (m_pSteam)
    m_pSteam->SetData(Blk.m_Common, Blk.m_Vap);
  FixOffsetConstants();

#if dbgBuildSDB
  if (dbgDumpH2O())
    {
    if (m_pSteam)
      m_pSteam->Dump();
    if (m_pWater)
      m_pWater->Dump();
    }
#endif
  };

void CComponentDataBase::SelectH2OProperties(const char * ClassId)
  {
  m_H2O.Select(ClassId);
  H2OPropertiesBlock & Blk=m_H2O.SelectedBlk();
  if (m_pWater)
    m_pWater->SetData(Blk.m_Common, Blk.m_Liq);
  if (m_pSteam)
    m_pSteam->SetData(Blk.m_Common, Blk.m_Vap);
  FixOffsetConstants();

#if dbgBuildSDB
  if (dbgDumpH2O())
    {
    if (m_pSteam)
      m_pSteam->Dump();
    if (m_pWater)
      m_pWater->Dump();
    }
#endif
  };

long CComponentDataBase::H2OPropertiesSelected()
  {
  return m_H2O.Selected();
  };

char* CComponentDataBase::H2OPropertiesSelectedClassId()
  {
  return m_H2O.SelectedClassId();
  };

H2OPropertiesBlock & CComponentDataBase::H2OPropertiesSelectedBlk()
  {
  return m_H2O.SelectedBlk();
  };

// ==========================================================================
//
//
//
// ==========================================================================

CDBInitRec::CDBInitRec(double MinT_, double MaxT_, double MinP_, double MaxP_, flag UseStdFns4H2O_)
  {
  MinT = MinT_;
  MaxT = MaxT_;
  MinP = MinP_;
  MaxP = MaxP_;
  UseStdFns4H2O = UseStdFns4H2O_;
  NSpPrj = 0;
  NSpDensCorrPrj = 0;
  NSpSolFnPrj = 0;
  };

// ==========================================================================
//
//
//
// ==========================================================================

char * CheckSymbolName(const char * In, Strng & Buff)
  {
  // Check for () in specie names
  if (In==NULL || strlen(In)==0)
    return "";
  Buff=In;
  Strng TheOldSym(In);
  flag ParensFound=false;
  while (Buff.XStrChr('('))
    {
    *Buff.XStrChr('(')='[';
    ParensFound=true;
    }
  while (Buff.XStrChr(')'))
    {
    *Buff.XStrChr(')')=']';
    ParensFound=true;
    }
  if (ParensFound)
    LogWarning(SpecieDBLogHdr, 0, "Specie name %s changed to %s", TheOldSym(), Buff());
  return Buff();
  }

// ==========================================================================
//
//
//
// ==========================================================================

CComponentDataBase::CComponentDataBase(flag ForTestingOnly): \
m_CDB(*this)
  {
  m_ForTestingOnly=ForTestingOnly;
  m_iNo=0;
  m_pWater=NULL;
  m_pSteam=NULL;
  Init();
  };

//---------------------------------------------------------------------------

CComponentDataBase::~CComponentDataBase()
  {
  Term();
  };

//---------------------------------------------------------------------------

void CComponentDataBase::Init()
  {
  m_iNo=0;
  bCDBBad=true;
  //Term();
  m_iNo=0;
  bCDBBad=true;
  for (int i=0; i<=BOT_All; i++)
    {
    m_PhInfoCount[i]=0;
    m_PhInfoFirst[i]=0;
    m_PhInfoLast[i]=-1;
    }

  //  Annotation.Clear();
  };

//---------------------------------------------------------------------------

void CComponentDataBase::Term()
  {
  CSpCommonDataA NullCommonDataA;
  CSpFidelDataA NullData;
  if (m_pWater)
    m_pWater->SetData(NullCommonDataA, NullData);
  if (m_pSteam)
    m_pSteam->SetData(NullCommonDataA, NullData);

  for (int i = 0; i < m_iNo; i++)
    {
    delete CmD[i];
    CmD[i]=NULL;
    }

  BIP.SetSize(0,0);

  m_iNo=0;
  bCDBBad=true;

  m_ph_SolS = 0;
  m_ph_SolE = 0;
  m_ph_LiqS = 0;
  m_ph_LiqE = 0;
  m_ph_GasS = 0;
  m_ph_GasE = 0;
  m_ph_ALL = 0;
  m_ph_AqsInx = 0;

  m_som_Sol = 0;
  m_som_Liq = 0;
  m_som_Gas = 0;
  m_som_SL = 0;
  m_som_ALL = 0;

  for (int i=0; i<BOT_All+1; i++)
    {
    m_PhInfoCount[i]   = 0;
    m_PhInfoFirst[i]   = 0;
    m_PhInfoLast[i]   = 0;
    }

  for (int i=0; i<MaxPhases; i++)
    m_PhInfo[i].Clear();

  m_DefFlashMethod =-1;
  m_DefFlashCmpIndex =-1;
  m_DefFlashLiqIndex =-1;
  m_DefFlashVapIndex =-1;

  DDBCompList.Empty();
  DDBCompListMix.Empty();
  DDBCompListMixImg.Empty();
  DDBCompListVapLiq.Empty();
  DDBCompListDash.Empty();
  DDBCompListDashMix.Empty();
  DDBCompListDashMixImg.Empty();
  DDBCompListDashVapLiq.Empty();

  m_Flashable.RemoveAll();

  m_DDBFlashable.Empty();
  m_FlashCmps.RemoveAll();
  m_FlashCmpsI.RemoveAll();

  m_DDBFlashMethods.Empty();
  m_FlashMethods.RemoveAll();
  m_FlashMethodsI.RemoveAll();
  };

//---------------------------------------------------------------------------

flag CComponentDataBase::LoadComponentsADO(CDBInitRec & CDBInit, char* DataFile, Strng_List & RqdCmp, CRqdSpList & RqdSpc, flag H2OAsAqueous)
  {
  flag Ok=true;

  CFileStatus State;
  if (CFile::GetStatus(DataFile, State))
    {
    //NBNB: The '°' in 'S°298' causes problems with other language utilities (eg chinese characters)
    const int iOC_S298 = 10; //index of S298 in columns
    char* SpColNames[] = { "Name", "Compound", "Definition", "Phase", "Occurence",
      "Checked", "Ts", "Te", "Rho", "dHf", "S°298",
      "Cp", "Vp", "Pc", "Tc", "Vc", "Ac", NULL };
    char* SpColNames2[] = { "Name", "Compound", "Definition", "Phase", "Occurence",
      "Checked", "Ts", "Te", "Rho", "dHf", "S298",
      "Cp", "Vp", "Pc", "Tc", "Vc", "Ac", NULL };
    CDBHelper DB(DataFile, "Species");
    if (DB.Open(DBConnect_Default, SpColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
      {
      BOOL ColNamesOK = DB.CheckFieldNames(SpColNames);
      if (!ColNamesOK && DB.FieldIndex(iOC_S298)<0)
        {//problem with S°298, try alternate field names
        ColNamesOK = DB.CheckFieldNames(SpColNames2);
        }
      if (ColNamesOK)
        {
        char* OptColNames[] = { "Dissociation", "MolecularDiam", "MolarVol", "CpCv", "Reference", NULL };
        DB.CheckOptionalFieldNames(OptColNames);

        m_SDB.m_iOC_Dissociation  = (DB.FieldIndex(DB.NFieldIndexes()-5)<0 ? -1 : DB.NFieldIndexes()-5);
        m_SDB.m_iOC_MolecularDiam = (DB.FieldIndex(DB.NFieldIndexes()-4)<0 ? -1 : DB.NFieldIndexes()-4);
        m_SDB.m_iOC_MolVol        = (DB.FieldIndex(DB.NFieldIndexes()-3)<0 ? -1 : DB.NFieldIndexes()-3);
        m_SDB.m_iOC_CpCv          = (DB.FieldIndex(DB.NFieldIndexes()-2)<0 ? -1 : DB.NFieldIndexes()-2);
        m_SDB.m_iOC_Reference     = (DB.FieldIndex(DB.NFieldIndexes()-1)<0 ? -1 : DB.NFieldIndexes()-1);

        CStringArray Values;
        flag DataReqd=0, EndFound=0, AppendComp=0, PrevWasLoaded=0;
        CComponent * pC=NULL;
        while (!DB.IsEOF())
          {
          DB.GetNextRow(Values);

          Strng SymBuff;
          const char* pTag = (const char*)Values[0];
          const char* pSym = CheckSymbolName((const char*)Values[1], SymBuff);
          if (strlen(pSym)==0)
            break;
          if (strlen(pTag)==0)
            pTag = pSym;

          const char* pElDef = (const char*)Values[2];

          flag UseH2OFns=UseStdFns4H2O && (_stricmp(pElDef, "H2O1")==0);
          // Now decide on the new Component
          pC = NULL;
          if (RqdCmp.Find(pSym, true)!=NULL) // do we want this one ?
            {
            for (int c=0; c<No(); c++)
              if (strcmp(CmD[c]->Sym(), pSym)==0) // already got it
                pC = CmD[c];

            // Check getting from the same Database
            if (pC && (pC->m_PhaseData[0]->m_bPrjDB != True/*PrjDB*/))
              {
              pC=NULL; // Skip - aleady got from other source
              }
            else if (pC==NULL)
              {
              Strng ElDef(pElDef);
              if (!m_EDB.CheckElDef(ElDef))
                {
                Ok=false;
                LogError(SpecieDBLogHdr, 0, "Invalid Compound Definition '%s' for '%s'.  Each element must begin with uppercase character and end with number eg C1O2 or Ore1(90)", ElDef(), pSym);
                bCDBBad=true;
                }

#if dbgBuildSDB
              if (dbgBuildSpc())
                {
                dbgpln("========================");
                dbgp(" %-15s %-25s ",pSym, pTag);
                }
#endif

              pC = new CComponent(m_SDB, m_CDB, m_H2O, /*pTag,*/ pSym);
              //pC->Name=pComp;
              pC->m_sElDef=ElDef;
              pC->m_dMoleWt=0.0;
              Strng Elements, Element;
              GetElements(pC->m_sElDef, Elements);
              bool EDone[MaxElements];
              for (int e=0; e<MaxElements; e++)
                EDone[e]=false;
              while (Elements.Token(Element, " ,", 0, " ,"))
                {
                int e=m_EDB.Find(Element());
                if (e>=0)
                  {
                  if (!EDone[e])
                    {

#if dbgBuildSDB
                    if (dbgBuildSpc())
                      dbgp(" %s %g ",Element(), GetElemAmt(pC->m_sElDef, Element));
#endif
                    double dMoles=GetElemAmt(pC->m_sElDef, Element);
                    pC->m_dMoleWt+=m_EDB[e].m_AtmWt*dMoles;
                    CElemComp EC(e, dMoles);
                    pC->m_ElComp.Add(EC);
                    EDone[e]=true;
                    }
                  else
                    {
                    LogError(SpecieDBLogHdr, 0, "%s: Element '%s' referenced more than once in definition", pC->SymOrTag(), Element());
                    Ok=false;
                    }
                  }
                else
                  LogError(SpecieDBLogHdr, 0, "Element '%s' not found", Element());
                }
#if dbgBuildSDB
              if (dbgBuildSpc())
                dbgpln("  MoleWt= %g",pC->MoleWt());
#endif
              Add(pC);
              }
            }
          if (pC)
            {
            CSpecie * pSp = new CSpecie(pC, m_SDB, m_CDB, m_H2O, pTag);
            if (pSp->LoadADO(pC, Values, UseH2OFns))
              {
              if (UseH2OFns)
                {
                if (pSp->m_eOcc==BOT_Liquid)
                  m_pWater=pSp;
                else if (pSp->m_eOcc==BOT_Gas)
                  m_pSteam=pSp;
                else
                  ASSERT_ALWAYS(0, "Solid water phase not allowed!", __FILE__, __LINE__);
                //DoBreak(); // should not get here
                }
              
             if (H2OAsAqueous && _stricmp(pC->m_sElDef(), "H2O1")==0)
               {
               if ((pSp->m_eOcc==BOT_Liquid) && (pSp->m_sPhaseSymActual=="l"))
                 pSp->m_sPhaseSymActual="aq";
               }

             ASSERT(pC->m_nPhaseData<MaxPhases);
              //flag GotIt=false;
              CSpecie * pSpPh = NULL;
              for (int i=0; i<pC->m_nPhaseData; i++)
                {
                if (pC->m_PhaseData[i]->m_sPhaseSymActual.XStrCmp(pSp->m_sPhaseSymActual())==0)
                  {
                  pSpPh=pC->m_PhaseData[i];//GotIt=true;
                  break;
                  }
                }
              if (pSpPh)
                {
                // Specie Exists -- append data
                pSpPh->m_FData[0].AddSpecieTData(pSp->m_FData);
                pSpPh->m_sVpErrors.Append(&pSp->m_sVpErrors);
                delete pSp;
                }
              else
                {
                pC->m_PhaseData[pC->m_nPhaseData++]=pSp;
                // Sort into solids(s,...) liquids(l,aq,...) gasses(g,... ???
                for (int i=pC->m_nPhaseData-1; i>0; i--)
                  {
                  CSpecie * Sp1=pC->m_PhaseData[i-1];
                  CSpecie * Sp2=pC->m_PhaseData[i];
                  if (Sp2->m_eOcc > Sp1->m_eOcc)
                    break;
                  else if (Sp2->m_eOcc == Sp1->m_eOcc)
                    {
                    if (Sp2->m_sPhaseSymVisible.XStrCmp(Sp1->m_sPhaseSymVisible)>0)
                      break;
                    if (!PhaseNamePrimary(Sp2->m_eOcc, Sp2->m_sPhaseSymVisible()) && PhaseNamePrimary(Sp1->m_eOcc, Sp1->m_sPhaseSymVisible()))
                      break;
                    }
                  Exchange(pC->m_PhaseData[i-1], pC->m_PhaseData[i]);
                  }
                }
              }
            else
              delete pSp;
            }
          }
        }
      else
        {
        if (DB.FieldIndex(iOC_S298)<0)
          LogError(SpecieDBLogHdr, 0, "Try changing field name 'S°298' to 'S298' in table 'Species' in '%s'", DataFile);
        else
          LogError(SpecieDBLogHdr, 0, "Incorrect field names: Table 'Species' in '%s'", DataFile);
        Ok = false;
        }
      DB.Close();
      }
    else
      {
      LogError(SpecieDBLogHdr, 0, "Unable to open Table 'Species' in '%s'", DataFile);
      Ok = false;
      }

    return Ok;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::LoadComponents(CDBInitRec & CDBInit, char* DataFile, Strng_List & RqdCmp, CRqdSpList & RqdSpc, flag H2OAsAqueous)
  {
  Strng Ext;
  Ext.FnExt(DataFile);
  if (Ext.XStrICmp(".xls")==0 || Ext.XStrICmp(".mdb")==0)
    {
#if dbgBuildSDB
    if (dbgtestfileopen() || dbgBuildSpc())
      dbgpln("Load Species [ADO] From %s", DataFile);
#endif
    return LoadComponentsADO(CDBInit, DataFile, RqdCmp, RqdSpc, H2OAsAqueous);
    }

  return False;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::LoadSolnInfoADO(CDBInitRec & CDBInit, char* DataFile)
  {
  flag Ok=true;

  CFileStatus State;
  if (CFile::GetStatus(DataFile, State))
    {
    char* DcColNames[] = { "Solvent", "Solute", NULL }; // Ignore Reference
    CDBHelper DB(DataFile, "Solutions");
    if (DB.Open(DBConnect_Default, DcColNames[0], DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
      {
      if (DB.CheckFieldNames(DcColNames))
        {
        char* OptColNames[] = {"OtherPhase", "SolubilityFn", "DensCorrFn", "CpCorrFn", NULL };
        DB.CheckOptionalFieldNames(OptColNames);
        int iOther  = (DB.FieldIndex(DB.NFieldIndexes()-4)<0 ? -1 : DB.NFieldIndexes()-4);
        int iSolFn  = (DB.FieldIndex(DB.NFieldIndexes()-3)<0 ? -1 : DB.NFieldIndexes()-3);
        int iDensFn = (DB.FieldIndex(DB.NFieldIndexes()-2)<0 ? -1 : DB.NFieldIndexes()-2);
        int iCpFn   = (DB.FieldIndex(DB.NFieldIndexes()-1)<0 ? -1 : DB.NFieldIndexes()-1);

        CStringArray Values;
        //flag DataReqd=0, EndFound=0, AppendComp=0, PrevWasLoaded=0;
        //CComponent * pC=NULL;
        int iLine=0;
        while (!DB.IsEOF())
          {
          iLine++;
          DB.GetNextRow(Values);

          const char* pSolvent = (const char*)Values[0];
          const char* pSoluteA = (const char*)Values[1];
          const char* pSoluteS = iOther >= 0 ? (const char*)Values[iOther] : NULL;
          const char* pDensFn  = iDensFn >= 0 ? (const char*)Values[iDensFn] : NULL;
          const char* pSolFn   = iSolFn >= 0 ? (const char*)Values[iSolFn] : NULL;
          const char* pCpFn    = iCpFn >= 0 ? (const char*)Values[iCpFn] : NULL;
          //MUST have a valid Solvent and SoluteA!

          if (strlen(pSolvent)==0 || strlen(pSoluteA)==0)// || strlen(pDCFn)==0)
            {
            //LogError(SpecieDBLogHdr, 0, "Bad Data in Table 'Solutions' Line %i", iLine);
            LogError(SpecieDBLogHdr, 0, "A Solvent and Solute must be specified in the 'Solutions' table (Line %i)", iLine);
            continue;
            }

          int iSolvent = m_SDB.Find(pSolvent);
          int iSoluteA = m_SDB.Find(pSoluteA);
          int iSoluteS = (pSoluteS && strlen(pSoluteS)>0) ? m_SDB.Find(pSoluteS) : -1;
          flag Bad=false;
          if (iSolvent<0)
            {
            Bad=true;
            LogError(SpecieDBLogHdr, 0, "Solvent %s not found in table 'Solutions' (Line %i)", pSolvent, iLine);
            }
          else if (!m_SDB[iSolvent].IsLiq())
            {
            Bad=true;
            LogError(SpecieDBLogHdr, 0, "Solvent %s should be a liquid", pSolvent);
            }
          if (iSoluteA<0)
            {
            Bad=true;
            LogError(SpecieDBLogHdr, 0, "Solute %s for Solvent %s not found in table 'Solutions' (Line %i)", pSoluteA, pSolvent, iLine);
            }
          else if (!m_SDB[iSoluteA].IsLiq())
            {
            Bad=true;
            LogError(SpecieDBLogHdr, 0, "Solute %s for Solvent %s should be a liquid", pSoluteA, pSolvent);
            }
          if (iSolvent>=0 && iSolvent==iSoluteA)
            {
            Bad=true;
            LogError(SpecieDBLogHdr, 0, "The Solvent %s cannot be the same as the Solute", pSolvent);
            }
          if (pSolFn && strlen(pSolFn)>0)
            {
            if (iSoluteS<0)
              {
              //Bad=true;
              LogError(SpecieDBLogHdr, 0, "OtherPhase %s not found for Solute %s, Solvent %s", pSoluteS, pSoluteA, pSolvent);
              }
            else if (m_SDB[iSoluteS].IsGas())
              {
              //Bad=true;
              iSoluteS=-1;
              LogError(SpecieDBLogHdr, 0, "OtherPhase %s should not be a gas for Solute %s, Solvent %s", pSoluteS, pSoluteA, pSolvent);
              }
            }
          if (!Bad && m_SDB[iSoluteA].m_iSolvent>=0)
            {
            Bad=true;
            LogError(SpecieDBLogHdr, 0, "Specie %s (for solvent %s) already has a Solvent %s", pSoluteA, pSolvent, m_SDB[m_SDB[iSoluteA].m_iSolvent].SymOrTag());
            }

          if (Bad)
            continue;

          switch (m_SDB[iSolvent].AddSolnInfo(iSoluteA, iSoluteS, iSolvent, (char*)pDensFn, (char*)pCpFn, (char*)pSolFn, ""))
            {
            case 0:
              {
              if (m_SDB[iSoluteA].m_iSolvent<0)
                {
                m_SDB[iSoluteA].m_iSolvent=iSolvent;
                m_SDB[iSoluteA].m_bCpInDirect=m_SDB[iSolvent].m_bHasCpCorrs;
                if (m_SDB[iSoluteA].m_pDensCalc)
                  CDBInit.NSpDensCorrPrj++;
                if (m_SDB[iSolvent].m_Solubilities.GetCount()>0)
                  CDBInit.NSpSolFnPrj++;
                }
              else
                {
                ASSERT(FALSE); //this error condition should be trapped earlier!!!
                LogError(SpecieDBLogHdr, 0, "Specie %s already has a Solvent %s",
                  pSoluteA, m_SDB[m_SDB[iSoluteA].m_iSolvent].SymOrTag());
                }
              break;
              }
            case 1:
              LogError(SpecieDBLogHdr, 0, "Bad RelSG Specie %s in solvent %s", pSoluteA, pSolvent);
              break;
            case 2: // Already Loaded data...
              //LogWarning(SpecieDBLogHdr, 0, "DensCorrFn, SolubilityFn or HDilutionFn already loaded for Specie %s in solvent %s", pSoluteA, pSolvent);
              LogWarning(SpecieDBLogHdr, 0, "DensCorrFn or CpCorr or SolubilityFn already loaded for Specie %s in solvent %s", pSoluteA, pSolvent);
              break;
            case 3:
              //LogError(SpecieDBLogHdr, 0, "No valid data for DensCorrFn, SolubilityFn or HDilutionFn for Specie %s in solvent %s", pSoluteA, pSolvent);
              LogError(SpecieDBLogHdr, 0, "No valid data for DensCorrFn or SolubilityFn for Specie %s in solvent %s", pSoluteA, pSolvent);
              break;
            case 4:
              //LogError(SpecieDBLogHdr, 0, "No valid data for DensCorrFn, SolubilityFn or HDilutionFn for Specie %s in solvent %s", pSoluteA, pSolvent);
              LogError(SpecieDBLogHdr, 0, "Bad Limit Info for DensCorrFn or SolubilityFn for Specie %s in solvent %s", pSoluteA, pSolvent);
              break;
            case 5:
              LogError(SpecieDBLogHdr, 0, "No Limit set for DensCorrFn for Specie %s in solvent %s Assuming:Limit(%.2f, %.1f, %s)", 
                pSoluteA, pSolvent, DefaultLimitFrac, DefaultPureValue, DefaultDoWarn?"ON":"OFF");
              break;
            }
          }
        }
      else
        {
        LogError(SpecieDBLogHdr, 0, "Incorrect field names: Table 'Solutions' in '%s'", DataFile);
        Ok = false;
        }
      DB.Close();
      }
    else
      {
      LogWarning(SpecieDBLogHdr, 0, "Table 'Solutions' not found in '%s'", DataFile);
      }
    }
  return Ok;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::LoadSolnInfo(CDBInitRec & CDBInit, char* DataFile)
  {
  Strng Ext;
  Ext.FnExt(DataFile);
  if (Ext.XStrICmp(".xls")==0 || Ext.XStrICmp(".mdb")==0)
    {
#if dbgBuildSDB
    if (dbgtestfileopen() || dbgBuildSpc())
      dbgpln("Load SolnInfo [ADO] From %s", DataFile);
#endif
    return LoadSolnInfoADO(CDBInit, DataFile);
    }

  return false;
  }

//--------------------------------------------------------------------------

const long SysVecFnStkSize=32;
//MaxPhases Dependent
static PhMask Fn2PhMasks[]={0/*som_ALL*/, 0/*som_Sol*/, 0/*som_Liq*/, 0/*som_Gas*/, // 0/*som_SL*/,
                            0/*som_ALL*/, 0/*som_Sol*/, 0/*som_Liq*/, 0/*som_Gas*/,
                            0/*som_ALL*/,
                            0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
                            0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
                            0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000, 
                            0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000
                            };
//--------------------------------------------------------------------------

//MaxPhases Dependent
CSysVecInfo::CSpViewMask CSysVecInfo::sm_SpVwPhMask[SpVwPhMaskIndex_Count] =
  {
    { 0/*som_ALL*/, "All"},
    { 0/*som_Sol*/, "Sol"},
    { 0/*som_Liq*/, "Liq"},
    { 0/*som_Gas*/, "Gas"},
    { 0x00000001, "Ph0"},
    { 0x00000002, "Ph1"},
    { 0x00000004, "Ph2"},
    { 0x00000008, "Ph3"},
    { 0x00000010, "Ph4"},
    { 0x00000020, "Ph5"},
    { 0x00000040, "Ph6"},
    { 0x00000080, "Ph7"},
    { 0x00000100, "Ph8"},
    { 0x00000200, "Ph9"},
    { 0x00000400, "Ph10"},
    { 0x00000800, "Ph11"},
    { 0x00001000, "Ph12"},
    { 0x00002000, "Ph13"},
    { 0x00004000, "Ph14"},
    { 0x00008000, "Ph15"},
    { 0x00010000, "Ph16"},
    { 0x00020000, "Ph17"},
    { 0x00040000, "Ph18"},
    { 0x00080000, "Ph19"},
    { 0x00100000, "Ph20"},
    { 0x00200000, "Ph21"},
    { 0x00400000, "Ph22"},
    { 0x00800000, "Ph23"},
    { 0x01000000, "Ph24"},
    { 0x02000000, "Ph25"},
    { 0x04000000, "Ph26"},
    { 0x08000000, "Ph27"},
    { 0x10000000, "Ph28"},
    { 0x20000000, "Ph29"},
    { 0x40000000, "Ph30"},
    { 0x80000000, "Ph31"},
  };

//---------------------------------------------------------------------------

void CComponentDataBase::AddSpecieD(int c, CComponent & C, CSpecie * pSpD, int p)
  {
  CSpecie & Sp=*pSpD;

  Sp.iCId   = c;
  Sp.pFirst = C.m_PhaseData[0];
  Sp.pPrev  = p>0 ? C.m_PhaseData[p-1] : NULL;
  Sp.pNext  = p<C.m_nPhaseData-1 ? C.m_PhaseData[p+1] : NULL;

  if (p<C.m_nPhaseData)//-1)
    {
    m_SDB.Add(C.m_PhaseData[p]);
#if dbgBuildSDB
    if (dbgBuildSpc())
      dbgpln("                                           Add: %2i %-35.35s %s",
      C.m_PhaseData[p]->iSId,Sp.SymOrTag(), OccurenceNames[Sp.m_eOcc]);
#endif
    }
  };

//---------------------------------------------------------------------------

flag CComponentDataBase::CheckCommonData()
  {      
  bool AllOK=true;

  for (int c=0; c<m_iNo; c++)
    {
    CComponent &C=*(CmD[c]);
    flag HasSol=false;
    flag HasLiq=false;
    flag HasGas=false;

    for (int s=0; s<C.m_nPhaseData; s++)
      {
      CSpecie &S=*C.m_PhaseData[s];
      for (int f=0; f<MaxFidelity; f++)
        {
        double TLastEnd=S.m_FData[f].m_dTe[0];
        for (int t=1; t<S.m_FData[f].m_iLastTPoint; t++)
          {
          if (fabs(TLastEnd-S.m_FData[f].m_dTs[t])>0.01)
            {
            AppendErrorMsg(&S, LogFlag_Fatal, "%s: Temperature Segments not contiguous @ %.2fK cf %.2fK", C.SymOrTag(), TLastEnd, S.m_FData[f].m_dTs[t]);
            }
          TLastEnd=S.m_FData[f].m_dTe[t];
          }
        }
      }

    for (int s=0; s<C.m_nPhaseData; s++)
      {
      CSpecie &S=*C.m_PhaseData[s];
      HasSol = HasSol || S.m_eOcc==BOT_Solid;
      HasLiq = HasLiq || S.m_eOcc==BOT_Liquid;
      HasGas = HasGas || S.m_eOcc==BOT_Gas;
      }

    if (HasLiq && HasGas)
      { // Only One Exists - Ignore Vp Stuff - Set to bad Values ?
      for (int p=0; p<C.m_nPhaseData; p++)
        {
        CSpecie &S = *C.m_PhaseData[p];
        CSpCommonData &CD = S.m_Common[0];

        if (S.m_eOcc==BOT_Liquid && (S.m_bVpSpecd || S.m_bTcSpecd || S.m_bPcSpecd || S.m_bVcSpecd || S.m_bAcSpecd))
          {
          Strng Msg;
          for (int i=0; i<5; i++)
            {
            LPSTR pTxt=NULL;
            switch (i)
              {
              case 0: pTxt = S.m_bVpSpecd ? "VapourP" : NULL;  break;
              case 1: pTxt = S.m_bTcSpecd ? "Tc"      : NULL;  break;
              case 2: pTxt = S.m_bPcSpecd ? "Pc"      : NULL;  break;
              case 3: pTxt = S.m_bVcSpecd ? "Vc"      : NULL;  break;
              case 4: pTxt = S.m_bAcSpecd ? "Ac"      : NULL;  break;
              }
            if (pTxt)
              {
              if (Msg.Len()>0)
                Msg+=", ";
              Msg+=pTxt;
              }
            }
          AppendErrorMsg(&S, LogFlag_Fatal, "%s: %s found in the Liquid Phase - Expected in the Vapour Phase Only", C.SymOrTag(), Msg());
          }
        }
      }

    if (HasLiq != HasGas)
      { // Only One Exists - Ignore Vp Stuff - Set to bad Values ?
      for (int p=0; p<C.m_nPhaseData; p++)
        {
        CSpecie &S=*C.m_PhaseData[p];

        for (Strng * pS=S.m_sVpErrors.First(); pS; pS=pS->Next())
          dbgpln("Ignore %s", (*pS)());

        S.m_sVpErrors.Clear();
        //for (int f=0; f<MaxFidelity; f++)
        //  {
        //  //delete S.m_Common[f].m_pVp;
        //  //S.m_Common[f].m_pVp=NULL;
        //  }
        }
      }

    for (int p=0; p<C.m_nPhaseData; p++)
      {
      CSpecie &S=*C.m_PhaseData[p];
      for (Strng * pS=S.m_sVpErrors.First(); pS; pS=pS->Next())
        {
        switch (pS->Index())
          {
          case LogFlag_Fatal:   LogError  (SpecieDBLogHdr, 0, "%s", (*pS)());    AllOK= false;   break;
          case LogFlag_Error:   LogError  (SpecieDBLogHdr, 0, "%s", (*pS)());                    break;
          case LogFlag_Warning: LogWarning(SpecieDBLogHdr, 0, "%s", (*pS)());                    break;
          case LogFlag_Note:    LogNote   (SpecieDBLogHdr, 0, "%s", (*pS)());                    break;
          default:
          _asm int 3;
          }
        }
      S.m_sVpErrors.Clear();
      }
    }

  return AllOK;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::BuildSpecies(CDBInitRec & CDBInit, Strng_List & RqdCmp, CRqdSpList & RqdSpc)
  {
  // Sort Component Names;
  CComponent * CmDTmp[MaxComponents];
  int c1=0;
  for (pStrng m_pS=RqdCmp.First(); m_pS; m_pS=RqdCmp.Next())
    {
    int i=Find(m_pS->Str());
    if (i>=0)
      {
      CmDTmp[c1]=CmD[i];
      CmD[i]=NULL;
      c1++;
      }
    else
      LogError(SpecieDBLogHdr, 0, "Component '%s' not Found", m_pS->Str());
    }
  m_iNo=c1;
  for (int i=0; i<m_iNo; i++)
    CmD[i]=CmDTmp[i];

#if dbgBuildSDB
  if (dbgBuildSpc())
    for (int xx=0; xx<m_iNo; xx++)
      dbgpln("ActCmp : %s", CmD[xx]->SymOrTag());
#endif

  for (int c=0; c<m_iNo; c++)
    {
    CComponent & C=*(CmD[c]);
    CSpecie * pSpData=NULL;

    //FixAValC(C.m_dPc, C.m_bPcOK, 1000., CmD[c], C.m_CommonpSpData);
    //FixAValC(C.m_dTc, C.m_bTcOK, 1000., CmD[c], pSpData);
    //FixAValC(C.m_dVc, C.m_bVcOK, 1., CmD[c], pSpData);
    //FixAValC(C.m_dAc, C.m_bAcOK, 1., CmD[c], pSpData);

#if dbgBuildSDB
  if (dbgBuildSpc())
    {
    dbgpln("Component :%-15.15s %-15.15s %10.2f %10.2f %10.2f %10.2f Cnst:%08x Cmn0:%08x Cmn1:%08x Vp0:%-10s Vp1:%-10s",
      C.SymOrTag(), C.m_sElDef(), C.m_dPc, C.m_dTc, C.m_dVc, C.m_dAc, 
      dynamic_cast<CSpConstData*>(&C), &C.m_Common[0], &C.m_Common[1], 
      C.m_Common[0].m_pVp?C.m_Common[0].m_pVp->VName():"",
      C.m_Common[1].m_pVp?C.m_Common[1].m_pVp->VName():"");
    }
#endif

    for (int p=0; p<C.m_nPhaseData; p++)
      C.m_PhaseData[p]->FixMissingData(CmD[c]);
    }

  for (c=0; c<m_iNo; c++)
    {
    CComponent &C=*(CmD[c]);

    C.iCId=c;
    int PrvOcc=BOT_Null;
    int nPhaseInOcc=1;

    for (int f=0; f<MaxFidelity; f++)
      {
      if (C.m_Common[f].m_pVp)
        C.m_Common[f].m_pVp->Attach(&C, NULL);
      }


    // Remove Species if out of Temp Range or Component not Required
    for (int p=0; p<C.m_nPhaseData; )
      {
      CSpecie & Sp=*C.m_PhaseData[p];
      if (Sp.sCmpTag.Length())
        Sp.sTag.Set("%s(%s)", Sp.sCmpTag(), Sp.m_sPhaseSymVisible());
      else
        Sp.sTag="";
      if (CmD[c]->Sym.Length())
        Sp.sSym.Set("%s(%s)", CmD[c]->Sym(), Sp.m_sPhaseSymVisible());
      else
        Sp.sSym="";
      //Sp.m_dMoleWt=C.MoleWt();

      if (Sp.m_eOcc==BOT_Gas)
        {
        CRqdSpItem * pRS=RqdSpc.Find(Sp.SymOrTag());
        //ASSERT(pRS!=NULL);
        if (pRS && pRS->m_bUseIdeal)
          {
          Strng S;
          for (int f=0; f<MaxFidelity; f++)
            {
            for (int i=0; i<MaxTPoints; i++)
              {
              for (int e=0; e<SpDBEqnCnt; e++)
                {
                if (Sp.m_FData[f].m_pEqns[e][i])
                  Sp.m_FData[f].m_pEqns[e][i]->Attach(&C, &Sp.m_FData[f]);
                }

              if (Sp.m_FData[f].m_pRho[i] && (&Sp!=m_pSteam || f==0))
                {
                // Change to use IdealGasForms for all Gasses BUT HiFidelity Steam
                //  delete Sp.m_FData[f].m_pRho[i]; // This data is woned by the H2ODB
                S.Set("IdealGasDensity(%.10g)", Sp.MoleWt());
                Sp.m_FData[f].m_pRho[i]=CSpDBEqn::Construct(&Sp.m_Const, &Sp.m_FData[f], S());
                }
              }
            }
          }
        }
      
#if dbgBuildSDB
      if (dbgBuildSpc())
        dbgp("Specie: %7.2f > %7.2f %-20.20s", K_2_C(C.m_PhaseData[p]->LoT(0)), K_2_C(C.m_PhaseData[p]->HiT(0)), Sp.SymOrTag());
#endif

      flag RemoveItTmp=0;
      flag RemoveItLst=0;

      RemoveItTmp = (C.m_nPhaseData>1) && (C.m_PhaseData[p]->LoT(0) > CDBInit.MaxT) || (C.m_PhaseData[p]->HiT(0) < CDBInit.MinT);
      Strng *pC=RqdCmp.Find(C.Sym(), true);
      if (pC && pC->Index()==0)
        if (!RqdSpc.Find(Sp.Sym(), true))
          RemoveItLst=true;

      if (RemoveItTmp || RemoveItLst)
        {
        if (!RemoveItLst)
          {
          C.m_nDiscarded++;
          LogNote(SpecieDBLogHdr, 0, "Specie '%s' discarded (Not in Project Temperature Range)", Sp.Sym());
          }
#if dbgBuildSDB
        if (dbgBuildSpc())
          dbgp(" --- Discard");
#endif

        CSpCommonDataA NullCommonDataA;
        CSpFidelDataA NullData;
        if (m_pWater==C.m_PhaseData[p])
          m_pWater->SetData(NullCommonDataA, NullData);
        else if (m_pSteam==C.m_PhaseData[p])
          m_pSteam->SetData(NullCommonDataA, NullData);

        delete C.m_PhaseData[p];
        C.m_nPhaseData--;
        for (int q=p; q<C.m_nPhaseData; q++)
          C.m_PhaseData[q]=C.m_PhaseData[q+1];
        C.m_PhaseData[q]=NULL;
        }
      else
        {
        CSpecie &Sp=*C.m_PhaseData[p];
        PrvOcc=Sp.m_eOcc;
        p++;
        }

#if dbgBuildSDB
      if (dbgBuildSpc())
        dbgpln("");
#endif
      }

    // if no phases in compnent - remove it
    if (C.m_nPhaseData==0)
      {
      delete CmD[c];
      for (int cc=c; cc<m_iNo-1; cc++)
        CmD[cc]=CmD[cc+1];
      CmD[cc]=NULL;
      c--;
      m_iNo--;
      }
    }

  CRqdSpListIter IRqdSpc(RqdSpc);
  for (CRqdSpItem *pRS=IRqdSpc.First(); pRS; pRS=IRqdSpc.Next())
    {
    if (pRS->m_iDuplicate<0)
      {
      Strng Spc=pRS->m_Name;
      Strng Cmp=Spc;
      Cmp.SetLength(Cmp.XStrCSpn("("));
      int c=Find(Cmp());
      if (c>=0)
        {
        CComponent & C=*(CmD[c]);
        for (int p=0; p<C.m_nPhaseData; p++)
          if (strcmp(C.m_PhaseData[p]->SymOrTag(), Spc())==0)
            {
            AddSpecieD(c, C, C.m_PhaseData[p], p);
            break;
            }
        }
      }
    else
      {
      INCOMPLETECODE(__FILE__, __LINE__)
        //pRS->m_iDuplicate
        //AddSpecieD(c, C, C.PhaseData[p], p);
      }
    }

  for (c=0; c<No(); c++)
    {
    CComponent & C=*(CmD[c]);
    for (int p=0; p<C.m_nPhaseData; p++)
      if (C.m_PhaseData[p]->iCId<0)
        AddSpecieD(c, C, C.m_PhaseData[p], p);
    }

  //Find Phase Syms
  for (int s=0; s<m_SDB.m_iNo; s++)
    {
    CSpecie & S=m_SDB[s];
    Strng PhaseSym=S.m_sPhaseSymActual;

    bool Found=false;
    for (int ip=0; ip<m_PhInfoCount[BOT_All]; ip++)
      {
      if (m_PhInfo[ip].m_eOcc==S.m_eOcc)
        {
        int iCmp=m_PhInfo[ip].m_Sym.XStrCmp(PhaseSym());
        if (iCmp==0)
          {
          Found=true; // found it
          break; // keep ip correct
          }
        }
      if (m_PhInfo[ip].m_eOcc>S.m_eOcc)
        break;
      }

    if (!Found)
      {
      //dbgpln("Insert Phase at %3i %s", ip, S.m_sPhaseSym());
      if (m_PhInfoCount[BOT_All]+1<MaxPhases)
        {
        m_PhInfoCount[S.m_eOcc]++;
        m_PhInfoCount[BOT_All]++;
        for (int i=m_PhInfoCount[BOT_All]; i>ip; i--)
          m_PhInfo[i]=m_PhInfo[i-1];
        m_PhInfo[ip].m_Sym=PhaseSym;
        m_PhInfo[ip].m_eOcc=S.m_eOcc;
        m_PhInfo[ip].m_nSpecies=0;
        }
      else
        {
        LogError(SpecieDBLogHdr, 0, "Too Many Phases");
        }
      }
    m_PhInfo[ip].m_nSpecies++;
    }

  dword Msk=1;
  dword somPhaseInOcc[BOT_Null+1] = {0};
  for (int phi=0; phi<m_PhInfoCount[BOT_All]; phi++)
    {
    CPhaseInfo  & Ph = m_PhInfo[phi];
    int           O  = Ph.m_eOcc;
    Ph.m_PhMsk       = Msk;
    Ph.m_PhInx       = phi;
    somPhaseInOcc[O] |= Msk;

    Ph.m_Tag         = Ph.m_Sym;

    if (m_PhInfoLast[O]<0)
      m_PhInfoFirst[O]=phi;
    m_PhInfoLast[O]=phi;
    Msk<<=1;
    }

  //Find Phase Tags
  for (int phi=0; phi<m_PhInfoCount[BOT_All]; phi++)
    {
    CPhaseInfo & Ph = m_PhInfo[phi];
    if (Ph.m_Sym=="s")
      Ph.m_Tag="Solid";
    else if (Ph.m_Sym=="l")
      Ph.m_Tag="Liquid";
    else if (Ph.m_Sym=="g")
      Ph.m_Tag="Gas";                                             
    }

  //Find Phase Tags
  Strng *pS=CDBInit.m_PhNames.First();
  for ( ; pS; pS=pS->Next())
    {
    Strng S=pS->Str();
    char * pComma=S.XStrChr(',');
    if (pComma)
      {
      int Len=(pComma-S());
      if (Len>0 && S.Length()-Len-1>0)
        {
        Strng Sym=S.Left(Len);
        Strng Tag=S.Right(S.Length()-Len-1);

        for (int phi=0; phi<m_PhInfoCount[BOT_All]; phi++)
          {
          CPhaseInfo & Ph = m_PhInfo[phi];
          if (Ph.m_Sym.XStrCmp(Sym())==0)
            {
            Ph.m_Tag=Tag;
            break;
            }
          }
        }
      }
    }


#if dbgBuildSDB
  if (dbgBuildPhs())
    {
    for (int phi=0; phi<m_PhInfoCount[BOT_All]; phi++)
      {
      CPhaseInfo & Ph = m_PhInfo[phi];
      dbgpln("PhaseInfo %2i %3i %2i %2i %#08x %2i %#08x %-8s %s",
        phi, Ph.m_nSpecies, Ph.m_eOcc, Ph.m_PhInx, Ph.m_PhMsk,
        m_PhInfoCount[Ph.m_eOcc], somPhaseInOcc[Ph.m_eOcc], Ph.m_Sym(), Ph.m_Tag());
      }
    }
#endif

  m_som_Sol = somPhaseInOcc[BOT_Solid];
  m_som_Liq = somPhaseInOcc[BOT_Liquid];
  m_som_Gas = somPhaseInOcc[BOT_Gas];
  m_som_SL  = m_som_Sol|m_som_Liq;
  m_som_ALL = m_som_Sol|m_som_Liq|m_som_Gas;
  m_ph_SolS = 0;
  m_ph_SolE = m_ph_SolS+m_PhInfoCount[BOT_Solid]-1;
  m_ph_LiqS = m_ph_SolE+1;
  m_ph_LiqE = m_ph_LiqS+m_PhInfoCount[BOT_Liquid]-1;
  m_ph_GasS = m_ph_LiqE+1;
  m_ph_GasE = m_ph_GasS+m_PhInfoCount[BOT_Gas]-1;
  m_ph_ALL  = m_ph_GasE+1;
  m_ph_AqsInx = -1;

  m_PhInfoCount[BOT_All]=m_PhInfoCount[BOT_Solid]+m_PhInfoCount[BOT_Liquid]+m_PhInfoCount[BOT_Gas];
  m_PhInfoFirst[BOT_All]=0;
  m_PhInfoLast[BOT_All]=m_PhInfoCount[BOT_All]-1;

  CSysVecInfo::sm_SpVwPhMask[0].m_iMsk=m_som_ALL;
  CSysVecInfo::sm_SpVwPhMask[1].m_iMsk=m_som_Sol;
  CSysVecInfo::sm_SpVwPhMask[2].m_iMsk=m_som_Liq;
  CSysVecInfo::sm_SpVwPhMask[3].m_iMsk=m_som_Gas;

  Fn2PhMasks[0] = m_som_ALL;      // SFS_SumSLG
  Fn2PhMasks[1] = m_som_Sol;      // SFS_SumS
  Fn2PhMasks[2] = m_som_Liq;      // SFS_SumL
  Fn2PhMasks[3] = m_som_Gas;      // SFS_SumG
  //Fn2PhMasks[4] = som_SL;       // SFS_SumSL
  Fn2PhMasks[4] = m_som_ALL;      // SFS_SumPhSLG
  Fn2PhMasks[5] = m_som_Sol;      // SFS_SumPhS
  Fn2PhMasks[6] = m_som_Liq;      // SFS_SumPhL
  Fn2PhMasks[7] = m_som_Gas;      // SFS_SumPhG
  Fn2PhMasks[8] = m_som_ALL;      // SFS_SumIPhSLG

#if dbgBuildSDB
  if (dbgBuildPhs())
    {
    dbgpln("som_Sol  %#08x  ph_SolS %4i  ph_SolE %4i",m_som_Sol ,m_ph_SolS, m_ph_SolE);
    dbgpln("som_Liq  %#08x  ph_LiqS %4i  ph_LiqE %4i",m_som_Liq ,m_ph_LiqS, m_ph_LiqE);
    dbgpln("som_Gas  %#08x  ph_VapS %4i  ph_VapE %4i",m_som_Gas ,m_ph_GasS, m_ph_GasE);
    dbgpln("som_SL   %#08x",m_som_SL  );
    dbgpln("som_ALL  %#08x",m_som_ALL );
    dbgpln("ph_Aqs   %4i", m_ph_AqsInx);
    }
#endif

  // Check that all Species Have been loaded & setup CSpecieBlk CId's
  flag Ok=true;
  if (!m_ForTestingOnly)
    {
    for (CSpecieBlk * pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
      {
      Strng C=CmpName(pSb->m_sCmpName); // Trim off Qualifiers eg (ag), (l) etc
      int iCId=Find(C());
      if (iCId>=0)
        {
        flag COk=true;
        Strng S(pSb->m_sSpName()), PhNm;
        int i=S.XStrCSpn("(");
        if (i<S.Length())
          {
          for (int j=i+1; j<S.Length(); j++)
            if (S[j]==')')
              break;
            else
              PhNm+=S[j];
          while (COk)
            {
            CSpecie * p=CmD[iCId]->FindPhaseData(PhNm());
            if (p==NULL)
              {
              if (iCId < No()-1 && strcmp(CmD[iCId]->SymOrTag(), CmD[iCId+1]->SymOrTag())==0)
                iCId++;
              else
                COk=false;
              }
            else
              break;
            }
          }
        if (COk)
          pSb->iCId=iCId;
        else if (!pSb->m_fOpt)
          LogError(SpecieDBLogHdr, 0, "Required Specie '%s' not Loaded %s", pSb->m_sSpName(), pSb->m_sFile());
        }
      else if (!pSb->m_fOpt)
        LogError(SpecieDBLogHdr, 0, "Required Specie '%s' not Loaded %s", C(), pSb->m_sFile());
      }
    }
  //int PrvCId=-1;
  int NNotChecked=0;
  Strng NotCheckedList;
  CDBInit.NSpPrj=0;
  CDBInit.NSpDensCorrPrj=0;
  CDBInit.NSpSolFnPrj=0;

  ////TODO Multi Phase / Occurence problem!
  ////KGA : 17/1/2002 : Problem with multi-occurances in a phase for a specie
  ////1) The logic for creating extra phases/occurences is DEPENDENT on specie sequence in array!
  ////2) When "extra phase" is created then other bugs occur (eg display in pipe in MassFrac view with
  ////   ByPhase ticked).  There may be (or may not) be other problems when using phase calcs?!?!
  //#define TempRemoveExtraOcc 0 //for now, there will only ever be the "three standard Sol Liq Vap phases"

  //  DoBreak();

  //CompOccurs PrvOcc=BOT_Null;
  //phi=INT_MAX;
  int Phi=-1;
  for (int s=0; s<m_SDB.m_iNo; s++)
    {
    CSpecie & S=m_SDB[s];
    for (i=m_PhInfoFirst[S.m_eOcc]; i<=m_PhInfoLast[S.m_eOcc]; i++)
      {
      CPhaseInfo & Ph = m_PhInfo[i];
      if (S.m_sPhaseSymActual.XStrCmp(Ph.m_Sym)==0)
        {
        Msk=Ph.m_PhMsk;
        Phi=Ph.m_PhInx;   
        break;                          
        }
      }
    ASSERT(i<m_PhInfoCount[BOT_All]);

    S.m_PhMsk=Msk;
    if (S.IsSol())
      {
      S.m_OccMsk=som_Sol;
      S.m_iBOccT=BOT_Solid;
      S.m_bCpOvrMask=SPO_SCp;
      S.m_bDensOvrMask=SPO_SDens;
      S.m_iOverIndex=SPIndex_Sol;
      }
    else if (S.IsLiq())
      {
      S.m_OccMsk=som_Liq;
      S.m_iBOccT=BOT_Liquid;
      S.m_bCpOvrMask=SPO_LCp;
      S.m_bDensOvrMask=SPO_LDens;
      S.m_iOverIndex=SPIndex_Liq;
      }
    else
      {
      S.m_OccMsk=som_Gas;
      S.m_iBOccT=BOT_Gas;
      S.m_bCpOvrMask=SPO_VCp;
      S.m_bDensOvrMask=SPO_VDens;
      S.m_iOverIndex=SPIndex_Vap;
      }

    S.m_PhInx=Phi;
    //PrvOcc=S.eOcc;
    //PrvCId=S.iCId;
    int c=S.iCId;
    CmD[c]->m_SpPhaseD[Phi]=m_SDB.m_SpD[s];

    S.m_bIsAqueous=S.m_sPhaseSymActual.XStrICmp("aq")==0 || S.m_sPhaseSymActual.XStrICmp("a")==0;

#if dbgBuildSDB
    if (dbgBuildSpc())
      dbgpln("c%4i/s%4i)Cmp:%-15.15s  Spc:%-15.15s  %#08x %3i %3i", c, s, CmD[c]->SymOrTag(), S.SymOrTag(), Msk, S.m_PhInx, S.m_eOcc);
#endif
    if (!(S.m_FData[0].m_bChecked || S.m_FData[1].m_bChecked || S.m_bReference))
      {
      if (NNotChecked)
        NotCheckedList+=", ";
      NotCheckedList+=S.SymOrTag();
      NNotChecked++;
      }
    ASSERT(S.m_bPrjDB);
    CDBInit.NSpPrj++;
    }

  if (!Ok)
    return false;

  int nAssumedCps=0;
  int nDiscarded=0;
  Strng AssumedCpsList;
  Strng DiscardedList;
  for (c=0; c<m_iNo; c++)
    {
    CComponent &C=m_CDB[c];
    if (C.m_nAssumedCps)
      {
      if (nAssumedCps)
        AssumedCpsList+=", ";
      AssumedCpsList+=C.Sym();
      nAssumedCps+=C.m_nAssumedCps;
      }
    if (C.m_nDiscarded)
      {
      if (nDiscarded)
        DiscardedList+=", ";
      DiscardedList+=C.Sym();
      nDiscarded+=C.m_nDiscarded;
      }

    for (int i=0; i<C.m_nPhaseData; i++)
      C.m_iPhaseData[i]=C.m_PhaseData[i]->iSId;
    }


  // Set up elemental Info;
  for (s=0; s<m_SDB.m_iNo; s++)
    {
    CComponent &C=m_CDB[m_SDB[s].CId()];
    ElemCompArray &EC=C.m_ElComp;
    for (i=0; i<EC.GetSize(); i++)
      {
      int e=EC[i].iENo;
      CCompFrac CF;
      CF.iSpNo=s;
      CF.dMassFrac=EC[i].dMoles*m_EDB[e].m_AtmWt/C.m_dMoleWt;
      m_EDB[e].m_SpComp->Add(CF);
      }
    }

  // Setup Used Elements List
  for (int e=0; e<EDB.Count(); e++)
    {
    if (EDB[e].m_SpComp->GetCount()>0)
      EDB.m_InUse.Add(&EDB[e]);
    }

  //...
  if (nAssumedCps)
    {
    if (AssumedCpsList.Len()>120)
      {
      AssumedCpsList = AssumedCpsList.Left(120);
      AssumedCpsList += "...";
      }
    LogWarning(SpecieDBLogHdr, 0, "%i Species have assumed Cps   [ %s ]", nAssumedCps, AssumedCpsList());
    }
  if (nDiscarded)
    LogWarning(SpecieDBLogHdr, 0, "%i Species discarded   [ %s ]", nDiscarded, DiscardedList);
  if (CDBInit.NSpPrj)
    LogNote(SpecieDBLogHdr, 0, "%i Species Loaded from %s", CDBInit.NSpPrj, PrjDataFile());
  if (NNotChecked)
    {
    if (NotCheckedList.Len()>110)
      {
      NotCheckedList = NotCheckedList.Left(110);
      NotCheckedList += "...";
      }
    LogWarning(SpecieDBLogHdr, 0/*|LF_Exclamation*/, "%i Species in database not 'checked'.  [ %s ]", NNotChecked, NotCheckedList());
    }
  if (m_SDB.m_iNo==0)
    {
    LogError(SpecieDBLogHdr, LF_Exclamation, "No Species Loaded");
    return false;
    }

  flag ROk=true;
  // Check for valid Phase tags
  for (int phi=0; phi<m_PhInfoCount[BOT_All]; phi++)
    {
    CPhaseInfo & Ph = m_PhInfo[phi];
    if (strpbrk(Ph.m_Sym(), " ,.%()[]{}:\\<>!^\t\n\v\f\r"))
      {
      ROk=false;
      LogError(SpecieDBLogHdr, 0, "Illegal character in phase symbol '%s'", Ph.m_Sym());
      }
    if (strpbrk(Ph.m_Tag(), " ,.%()[]{}:\\<>!^\t\n\v\f\r") && strcmp(Ph.m_Tag(), Ph.m_Sym())!=0)
      {
      ROk=false;
      LogError(SpecieDBLogHdr, 0, "Illegal character in phase tag '%s'", Ph.m_Tag());
      }
    }
  if (!ROk)
    return false;

  // Check for valid specie tags and symbols
  for (s=0; s<m_SDB.m_iNo; s++)
    {
    CSpecie & Sp=m_SDB[s];
    char* prob = strchr(Sp.sSym(), ',');
    if (prob)
      {
      ROk=false;
      LogError(SpecieDBLogHdr, 0, "',' not allowed in specie symbol '%s'", Sp.sSym());
      }
    prob = strchr(Sp.sTag(), ',');
    if (prob)
      {
      ROk=false;
      LogError(SpecieDBLogHdr, 0, "',' not allowed in specie tag '%s'", Sp.sTag());
      }
    }
  if (!ROk)
    return false;

  // Check temp range Overlap
  for (s=0; s<m_SDB.m_iNo; s++)
    {
    flag SOK=true;
    CSpecie & Sp=m_SDB[s];
    for (int Fidelity=0; Fidelity<2; Fidelity++)
      {
      CSpFidelData & D=m_SDB[s].m_FData[Fidelity];
      for (i=0; i<D.m_iLastTPoint; i++)
        {
        if (D.m_dTe[i] > D.m_dTs[i+1]+0.1)
          SOK=false;
        }
      }
    if (!SOK)
      {
      LogError(SpecieDBLogHdr, 0, "Temp Range Overlap %s", Sp.SymOrTag());
      ROk=false;
      }
    }
  if (!ROk)
    return false;

  // Check Tag Uniqueness; and same occurence for same phasename 
  for (s=0; s<m_SDB.m_iNo; s++)
    {
    CSpecie & Sp=m_SDB[s];
    for (int s0=s+1; s0<m_SDB.m_iNo; s0++)
      {
      if (m_SDB[s0].sTag.XStrCmp(m_SDB[s].sTag)==0)
        {
        LogError(SpecieDBLogHdr, 0, "Species %s & %s both have Tag = %s", m_SDB[s].sSym(), m_SDB[s0].sSym(), m_SDB[s].sTag());
        ROk=false;
        }
      if (m_SDB[s0].m_sPhaseSymVisible.XStrCmp(m_SDB[s].m_sPhaseSymVisible)==0)
        {
        if (m_SDB[s0].m_OccMsk != m_SDB[s].m_OccMsk)
          {
          LogError(SpecieDBLogHdr, 0, "Species %s & %s with the same phase name (%s) can not have different occurences (%s & %s)", m_SDB[s].sSym(), m_SDB[s0].sSym(), m_SDB[s].m_sPhaseSymVisible(), 
            (m_SDB[s0].m_OccMsk==som_Sol ? "s" : m_SDB[s0].m_OccMsk==som_Liq ? "l" : "g"), (m_SDB[s].m_OccMsk==som_Sol ? "s" : m_SDB[s].m_OccMsk==som_Liq ? "l" : "g"));
          ROk=false;
          }
        }
      }
    }
  if (!ROk)
    return false;

  for (s=0; s<m_SDB.m_iNo; s++)
    {
    CSpecie & Sp=m_SDB[s];
    char *p=Sp.sTag.XStrChr(' ');
    if (p)
      {
      //this message is annoying. Remove?
      // I Agree - LogNote(SpecieDBLogHdr, 0, "Removing ' ' from Specie Tag '%s' for use in project", Sp.sTag());
      for ( ; p; p=Sp.sTag.XStrChr(' '))
        *p='_';
      }
    }
  
  for (s=0; s<m_SDB.m_iNo; s++)
    {
    CSpecie & Sp=m_SDB[s];
    if (Sp.IsLiq() && Sp.m_Const.m_iDaType==CSpConstData::DaType_IsNull)
      {
      // fill in the blanks
      CAcidBaseData * p=FindAcidBaseData(Sp.Sym());
      if (p)
        {
        Sp.m_Const.m_iDaType= p->m_Acid ? CSpConstData::DaType_IsAcid : CSpConstData::DaType_IsBase;
        for (int a=0; a<3; a++)
          Sp.m_Const.m_dDa[a]=p->m_dDa[a];
        }
      }
    switch (Sp.m_Const.m_iDaType)
      {
      case CSpConstData::DaType_IsAcid:
        m_SDB.m_Acids.Add(s);
        Sp.m_Const.m_sKaKbDesc.Set("Ka(%.2g,%.2g,%s)", Sp.m_Const.m_dDa[0], Sp.m_Const.m_dDa[1], Sp.m_Const.m_dDa[2]!=0?",..":""); 
        dbgpln("Acid: %-15s %s", Sp.Sym(), Sp.m_Const.m_sKaKbDesc());
        break;
      case CSpConstData::DaType_IsBase:
        m_SDB.m_Bases.Add(s);
        Sp.m_Const.m_sKaKbDesc.Set("Ka(%.2g)", Sp.m_Const.m_dDa[0]);
        dbgpln("Base: %-15s %s", Sp.Sym(), Sp.m_Const.m_sKaKbDesc());
        break;
      }
    }
  
  return true;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::LoadBIPs(CDBInitRec & CDBInit, char* FilePath)
  {
  ////TODO Put specie database BIPs back
#if IncludeBIP
  flag Ok=true;
  Strng DataFile(FilePath);
  if (DataFile.Length() && DataFile[DataFile.Length()-1]!='\\')
    DataFile += '\\';
  DataFile += "SPECIEBI.CSV";
  FILE *f=fopen(DataFile(), "rt");
  if (f)
    {
    int nCompIDs=0;
    int BipCompIds[MaxSpecies];

    char Buff[4096];
    char *Pos=&Buff[0];
    fgets(Buff, sizeof(Buff), f);
    int LnNo=1;

    char *p;
    for (p=StrToken(Pos, ",\n"); !feof(f) ; p=StrToken(Pos, ",\n"))
      {
      if (p && _stricmp(p, "Compound")==0)
        break;
      fgets(Buff, sizeof(Buff), f);
      LnNo++;
      Pos=&Buff[0];
      }

    flag DataReqd=0, EndFound=0;
    CComponent * pC=NULL;
    int iCId=-1;
    while (!feof(f))
      {
      fgets(Buff, sizeof(Buff), f);
      LnNo++;

      pchar C[MaxSPDBCols];
      SpDB_ParseTokens(Buff, C);

      if (C[0]==NULL)
        break;
      iCId=Find(C[0]);
      if (iCId<0)
        break;
      BipCompIds[nCompIDs++]=iCId;
      for (int i=0; i<nCompIDs; i++)
        {
        if (C[i+1]==NULL)
          break;
        double b = strlen(C[i+1])>0 ? SafeAtoF(C[i+1]) : 0.0;
        int c1=BipCompIds[i];
        int c2=BipCompIds[nCompIDs-1];
        BIP[c1][c2]=b;
        BIP[c2][c1]=b;
        }
      }

    for (int i=0; i<nCompIDs; i++)
      BIP[i][i]=0.0;

    CDbgMngr dbgBIPs("Specie_DB", "BIPs");
    if (dbgBIPs())
      {
      dbgp("%14s","");
      for (int c=0;c<m_CDB.No(); c++)
        dbgp(" %14s",m_CDB[c].SymOrTag());
      dbgpln("");
      for (int c1=0;c1<m_CDB.No(); c1++)
        {
        dbgp("%-14s",m_CDB[c1].SymOrTag());
        for (int c2=0;c2<m_CDB.No(); c2++)
          if (m_CDB.BIP[c1][c2]!=0.0)
            dbgp(" %14g",m_CDB.BIP[c1][c2]);
          else
            dbgp(" %14s",".");
        dbgpln("");
        }
      }

    fclose(f);
    return 1;
    }
#endif
  return 0;
  }

//---------------------------------------------------------------------------

double CSpFidelData::CalcH(double T, bool AddConst, int iSeg)
  {
  double H=0;
  if (iSeg<0)
    iSeg=FindTIndex(T);
  if (m_pH[iSeg])
    {
    H=m_pH[iSeg]->AtTP(T, Std_P);
    if (AddConst)
      H+=m_dCHf[iSeg];
    }
  else
    {
    H=m_pCp[iSeg]->Integrate_dT(m_dTs[iSeg], T, Std_P);
    if (AddConst)
      H+=m_dCHf[iSeg];
    }
  return H;
  }

//---------------------------------------------------------------------------

double CSpFidelData::CalcDH(double T1, double T2, int iSeg)
  {
  double H=0;
  flag Rev=T2<T1;
  if (Rev)
    Exchange(T1, T2);
  int iSeg1, iSeg2;
  if (iSeg<0)
    {
    iSeg1=FindTIndex(T1);
    iSeg2=FindTIndex(T2);
    }
  else
    {
    iSeg1=iSeg;
    iSeg2=iSeg;
    }
  for (int i=iSeg1; i<=iSeg2; i++)
    {
    if (m_pH[i])
      H+=m_pH[i]->AtTP(T2, Std_P)-m_pH[i]->AtTP(T1, Std_P);
    else
      H+=m_pCp[i]->Integrate_dT(T1, T2, Std_P);
    }
  if (Rev)
    return -H;
  return H;
  }

//---------------------------------------------------------------------------

double CSpFidelData::CalcS(double T, bool AddConst, int iSeg)
  {
  double S=0;
  if (iSeg<0)
    iSeg=FindTIndex(T);
  if (m_pS[iSeg])
    {
    S=m_pS[iSeg]->AtTP(T, Std_P);
    if (AddConst)
      S+=m_dCSf[iSeg];
    }
  else
    {
    S=m_pCp[iSeg]->IntegrateOverT_dT(m_dTs[iSeg], T, Std_P);
    if (AddConst)
      S+=m_dCSf[iSeg];
    }
  return S;
  }

//---------------------------------------------------------------------------

double CSpFidelData::CalcDS(double T1, double T2, int iSeg)
  {
  double S=0;
  flag Rev=T2<T1;
  if (Rev)
    Exchange(T1, T2);
  int iSeg1, iSeg2;
  if (iSeg<0)
    {
    iSeg1=FindTIndex(T1);
    iSeg2=FindTIndex(T2);
    }
  else
    {
    iSeg1=iSeg;
    iSeg2=iSeg;
    }
  for (int i=iSeg1; i<=iSeg2; i++)
    {
    if (m_pS[i])
      S+=m_pS[i]->AtTP(T2, Std_P)-m_pS[i]->AtTP(T1, Std_P);
    else
      S+=m_pCp[i]->IntegrateOverT_dT(T1, T2, Std_P);
    }
  if (Rev)
    return -S;
  return S;
  }

//-----------------------------------------------------------------------

double CSpecie::msdHIdeal(long Fidelity, double T1, double T2)
  {
  return m_FData[Fidelity].CalcDH(T1, T2)/MoleWt();
  };

double CSpecie::msdSIdeal(long Fidelity, double T1, double T2)
  {
  return m_FData[Fidelity].CalcDS(T1, T2)/MoleWt();
  };

////-----------------------------------------------------------------------
//
//class CVapTFndS : public MRootFinderBase
//  {
//  public:
//    CSpecie*m_pS;
//    long iFidelity;
//    CVapTFndS(CSpecie*pS_, long Fidelity) :
//    MRootFinderBase("CVapTFndC", s_VapTTol)
//      {
//      m_pS=pS_;
//      iFidelity=Fidelity;
//      };
//    LPCTSTR ObjTag() { return (LPCTSTR)m_pS->SymOrTag(); };//Mdl->FullObjTag(); };
//    double Function(double T) { return m_pS->VapourP(iFidelity, T); };
//  };
//
//
//double CSpecie::VapourT(long Fidelity, double P_)
//  {
//  if (!VpOK())
//    return 0.0;
//
//  double dPrvVapT=Min(373.0, TCrit()-10);
//  CVapTFndS VapT(this, Fidelity);
//  VapT.SetTarget(P_);
//  VapT.SetLimits(1.0, TCrit());
//
//  if (0)
//    {
//    dbgpln("============================ %s", sTag());
//    for (double t=10; t<TCrit()+30; t+=10)
//      dbgpln("                                                      %10.3f %10.3f", t, VapT.Function(t));
//    dbgpln("++++++++++++");
//    }
//  dPrvVapT=Range(1.0, dPrvVapT, TCrit());
//  if (VapT.SolveFromEst(dPrvVapT, 1.0, 10.0)==RF_OK)
//    dPrvVapT=VapT.Result();
//  else
//    LogError("Specie", 0, "%s VapourT not Converged", SymOrTag());
//
//  if (0)
//    dbgpln("++++++++++++");
//  return dPrvVapT;
//  };

//---------------------------------------------------------------------------

double     CSpecie::msVolume(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)
  {
  if (Ovr && (Ovr->bUse & m_bDensOvrMask))
    return 1.0/Ovr->fDens[m_iOverIndex];

  double D;
  if (m_pDensCalc && M)
    D=m_pDensCalc->Evaluate(Fidelity, T, P, M);
  else
    D=m_FData[Fidelity].SpRho(T,P);

  return 1.0/GTZ(D);
  }

double     CSpecie::Density(long Fidelity, double T, double P, SpPropOveride *Ovr, CSVData M)
  {
  if (Ovr && (Ovr->bUse & m_bDensOvrMask))
    return 1.0/Ovr->fDens[m_iOverIndex];

  double D;
  if (m_pDensCalc && M)
    D=m_pDensCalc->Evaluate(Fidelity, T, P, M);
  else
    D=m_FData[Fidelity].SpRho(T,P);

  return D;
  };

double     CSpecie::NmsVolume(long Fidelity, SpPropOveride *Ovr, CSVData M)
  {
  if (Ovr && (Ovr->bUse & m_bDensOvrMask))
    return 1.0/Ovr->fDens[m_iOverIndex];

  double D;
  if (m_pDensCalc && M)
    D=m_pDensCalc->Evaluate(Fidelity, Norm_T, Norm_P, M);
  else
    D=m_FData[Fidelity].SpRho(Norm_T, Norm_P);

  return 1.0/GTZ(D);
  };

double     CSpecie::NDensity(long Fidelity, SpPropOveride *Ovr, CSVData M)
  {
  if (Ovr && (Ovr->bUse & m_bDensOvrMask))
    return 1.0/Ovr->fDens[m_iOverIndex];

  double D;
  if (m_pDensCalc && M)
    D=m_pDensCalc->Evaluate(Fidelity, Norm_T, Norm_P, M);
  else
    D=m_FData[Fidelity].SpRho(Norm_T, Norm_P);

  return D;
  };

//---------------------------------------------------------------------------

extern bool DBDataRaw;

void CSpecie::Dump(LPCTSTR Location, long NPts)
  {
  static double dtA[] = {1000,750,500,200,150,100,75,50, 25,20,15,10,7.5,5,2.5,2,1.5,1,0.5,0.25,0.2,0.15,0.1,0.05,0.025,0.02,0.015,0};
  static double dpA[] = {1000,750,500,200,150,100,75,50, 25,20,15,10,7.5,5,2.5,2,1.5,1,0.5,0.25,0.2,0.15,0.1,0.05,0.025,0.02,0.015,0};
  struct TmpNames
    {
    char * Name;
    bool WithP;
    };

  TmpNames TmpData[]=
    {
    "Cp", true,
      "H", true,
      "S", true,
      "Rho", true,
      "Vp", false,
      NULL
    };
  if (Location==NULL)
    Location=TemporaryFiles();
  NPts=Range(10L,NPts,250L);

  bool Mem0=CSurfaceCommon::m_DoSmoothing;
  bool Mem1=CSurfaceCommon::m_UseSurface;
  bool Mem2=DBDataRaw;

  for (int iFidelity=0; iFidelity<2; iFidelity++)
    {
    for (int What=0; TmpData[What].Name; What++)
      {
      for (int XXX=0; XXX<1; XXX++)
        {
        CSurfaceCommon::m_DoSmoothing=(XXX==2);
        CSurfaceCommon::m_UseSurface=(XXX>0);

        Strng Fn;
        Fn.Set("%s%s%s%s.%s",
          Location, SymOrTag(),
          (XXX==0 ? "N" : XXX==1 ? "S": XXX==2 ? "SS" : "#"),
          (iFidelity>0 ? "H" : "L"),
          TmpData[What].Name);
        Fn+=".csv";
        FILE *f=fopen(Fn(), "wt");
        if (f)
          {
          double * vpprev=new double [NPts*2];
          for (int YYY=0; YYY<4; YYY++)
            {
            bool DoingTDerivs=(YYY==1);
            bool DoingPDerivs=(YYY==2);
            DBDataRaw=(YYY==3);

            bool WithP=TmpData[What].WithP;
#if dbgBuildSDB
            double t1=(dbgDumpH2OLoT()>0 ? dbgDumpH2OLoT() : m_CDB.MinT);
            double t2=(dbgDumpH2OHiT()>0 ? dbgDumpH2OHiT() : m_CDB.MaxT);
            double p1=(dbgDumpH2OLoP()>0 ? dbgDumpH2OLoP() : m_CDB.MinP);
            double p2=(dbgDumpH2OHiP()>0 ? dbgDumpH2OHiP() : m_CDB.MaxP);
#else
            double t1=m_CDB.MinT;
            double t2=m_CDB.MaxT;
            double p1=m_CDB.MinP;
            double p2=m_CDB.MaxP;
#endif

            t2=Max(t2,t1+10);
            p2=Max(p2,p1+10);
            if (0) //hard wired pt;
              {
              p1=12748.35-250*4;
              p2=12748.35+250*4;
              t1=602.57-20;
              t2=602.57+20;
              }

            double v0=0;

            double dt=(t2-t1)/NPts;
            double dp=(p2-p1)/NPts;

            if (0)
              {
              // round up scales;
              for (int i=sizeof(dtA)/sizeof(dtA[0])-1;(dt>dtA[i] && i>0); i--)
                {};
              dt=dtA[i];
              for (i=sizeof(dpA)/sizeof(dpA[0])-1;(dp>dpA[i] && i>0); i--)
                {};
              dp=dpA[i];
              }

            if (WithP)
              {
              fprintf(f, "%s:", (DoingTDerivs ? "dT" : (DoingPDerivs ? "dP" : DBDataRaw? "Raw":"")));
              fprintf(f, "%s.%s\n",SymOrTag(), TmpData[What].Name);
              fprintf(f, "%s\n","Press(Bar),Temp(K)...");
              fprintf(f, "%s,","");
              int it=0;
              double t=t1;
              for ( ; it<NPts; it++, t+=dt)
                //            for (double t=t1; t<=t2; t+=dt)
                if (!DoingTDerivs || it>0)
                  fprintf(f, "%g,", (t));
              fprintf(f, "\n");
              double pold=p1;
              // always do npts
              int ip=0;
              double p=p1;
              //            for (double p=p1; p<=p2-0.1*dp; p+=dp, ip++)
              for ( ; ip<NPts; ip++, p+=dp)
                {
                //if (pold<100 && p>=100)
                //  p=100;
                if (!DoingPDerivs || ip>0)
                  fprintf(f, "%g,", p/100);
                double told=t1;
                double vtprev=0;
                int it=0;
                double t=t1;
                for ( ; it<NPts; it++, t+=dt)
                  //int it=0;
                  //for (double t=t1; t<=t2-0.1*dt; t+=dt, it++)
                  if (p<=p2 && t<=t2)
                    {
                    //if (told<273.15 && t>=273.15)
                    //  t=273.15;
                    double v=0;
                    switch (What)
                      {
                      case 0: v=msCp(iFidelity, t,p,NULL,NULL); break;
                      case 1: v=msHf(iFidelity, t,p,NULL,NULL); break;
                      case 2: v=msSf(iFidelity, t,p,NULL,NULL); break;
                      case 3: v=Density(iFidelity, t,p,NULL,NULL); break;
                        // msFreeEnergy (double T, double P, SpPropOveride *Ovr, double *M);
                      default:;
                      }
                    bool DoPrint=true;
                    double vmem=v;
                    if (DoingTDerivs)
                      {
                      if (it==0)
                        DoPrint=false;
                      else
                        v-=vtprev;
                      vtprev=vmem;
                      }
                    else if (DoingPDerivs)
                      {
                      if (ip==0)
                        DoPrint=false;
                      else
                        v-=vpprev[it];
                      vpprev[it]=vmem;
                      }
                    else
                      {
                      if (ip==0 && it==0)
                        v0=v;
                      //                    v-=v0;
                      }

                    if (DoPrint)
                      fprintf(f, "%g,",v);
                    told=t;
                    }
                  else
                    fprintf(f, ",");
                if (!DoingPDerivs || ip>0)
                  fprintf(f, "\n");
                pold=p;
                }
              }
            else
              {
              fprintf(f, "%s,","Temp(C),Press(kPa)\n");
              double told=t1;
              for (double t=t1; t<=t2-0.1*dt; t+=dt)
                {
                if (told<273.15 && t>=273.15)
                  t=273.15;
                double v=0;
                switch (What)
                  {
                  case 4: v=VapourP(iFidelity, t); break;
                  default:;
                  }
                fprintf(f, "%g,%g\n",K_2_C(t),v);
                told=t;
                }
              }
            fprintf(f, "\n====\n\n");
            }
          delete vpprev;
          fclose(f);
          }
        }
      }
    }
  //  CSpecieDataBase::SetFidelity(FidelityMem);
  CSurfaceCommon::m_DoSmoothing=Mem0;
  CSurfaceCommon::m_UseSurface=Mem1;
  DBDataRaw=Mem2;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::CopyDataForMultiFidelity()
  {
  flag OK=true;

  for (int s=0; s<SDB.Count(); s++)
    if (!SDB[s].CopyDataForMultiFidelity())
      OK=false;

  return OK;
  };

//---------------------------------------------------------------------------

#define TheSepar1 "==========================================================================="
#define TheSepar2 "---------------------------------------------------------------------------"

flag CComponentDataBase::FixOffsetConstants()
  {
  flag Bad=false;
  const double T0=C_2_K(0);
  const double T25=C_2_K(25);
  const double T100=C_2_K(100);

  for (int Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
    {
    for (int c=0; c<m_CDB.No(); c++)
      {
      CComponent & C=*CmD[c];
      int iPh, iPhCorr=-1;
      double CorrHf[MaxPhases], CorrActHfMin=1e30;
      double CorrSf[MaxPhases], CorrActSfMin=1e30;
      for (iPh=0; iPh<C.m_nPhaseData; iPh++)
        {
        int i, iSeg;
        CSpecie  &Sp=*C.m_PhaseData[iPh];
        CSpFidelData & D=Sp.m_FData[Fidelity];

        D.m_bHasHFn=(D.m_pH[0]!=NULL);
        D.m_bHasSFn=(D.m_pS[0]!=NULL);
        for (iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
          {
          if (D.m_bHasHFn!=(D.m_pH[iSeg]!=NULL))
            {
            LogError(SpecieDBLogHdr, LF_Exclamation, "Internal:Seg H Functions Different");
            Bad=true;
            }
          if (D.m_bHasSFn!=(D.m_pS[iSeg]!=NULL))
            {
            LogError(SpecieDBLogHdr, LF_Exclamation, "Internal:Seg S Functions Different");
            Bad=true;
            }
          }

        D.m_iHfBase=0;
        for (iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
          if (D.m_dTs[iSeg]<= D.m_dTRef && D.m_dTRef<=D.m_dTe[iSeg])
            {
            D.m_iHfBase=iSeg;
            break;
            }

          if (iSeg>D.m_iLastTPoint)
            {
            //LogError(SpecieDBLogHdr, LF_Exclamation, "Internal:Ref Seg Not Found");
            //Bad=true;
            if (D.m_dTRef < D.m_dTs[0])
              iSeg=0;
            else if (D.m_dTRef>D.m_dTe[D.m_iLastTPoint])
              iSeg=D.m_iLastTPoint;
            else
              {
              LogError(SpecieDBLogHdr, LF_Exclamation, "Internal:Ref Seg Not Found");
              Bad=true;
              }
            }
          // Set up m_dCHf/m_dCSf consts at begining of each segment based on DH/DS.. relative to m_dTRef
          // i.e. ensure that curves are continuous across segments

          // HRef & SRef are those specified in the DataBase for the segment that
          // includes TRef m_dCHf & m_dCSf are the const to be added to the fn evaluated
          // from m_dTs to T of interest
          double TRef = D.m_bH25OK ? D.m_dTRef : T0;
          if (!D.m_bHasHFn)
            {
            double HRef = (D.m_bH25OK ? D.m_dH25[iSeg] : 0.0);
            D.m_dCHf[iSeg]=HRef+D.CalcDH(TRef, D.m_dTs[iSeg], iSeg);
            for (i=iSeg; i<D.m_iLastTPoint; i++)
              D.m_dCHf[i+1]=D.m_dCHf[i]+D.CalcDH(D.m_dTs[i], D.m_dTe[i], i);
            for (i=iSeg; i>0; i--)
              D.m_dCHf[i-1]=D.m_dCHf[i]+D.CalcDH(D.m_dTe[i-1], D.m_dTs[i-1], i-1);
            }
          else
            {
            for (i=0; i<=D.m_iLastTPoint; i++)
              D.m_dCHf[i]=0;
            }

          if (!D.m_bHasSFn)
            {
            double SRef = (D.m_bS25OK ? D.m_dS25[iSeg] : 0.0);
            D.m_dCSf[iSeg]=SRef+D.CalcDS(TRef, D.m_dTs[iSeg], iSeg);
            for (i=iSeg; i<D.m_iLastTPoint; i++)
              D.m_dCSf[i+1]=D.m_dCSf[i]+D.CalcDS(D.m_dTs[i], D.m_dTe[i], i);
            for (i=iSeg; i>0; i--)
              D.m_dCSf[i-1]=D.m_dCSf[i]+D.CalcDS(D.m_dTe[i-1], D.m_dTs[i-1], i-1);
            }
          else
            {
            for (i=0; i<=D.m_iLastTPoint; i++)
              D.m_dCSf[i]=0;
            }

          CorrHf[iPh]=0;
          if (D.m_bHasHFn && D.m_bH25OK)
            {
            // Must correct Differences and ...
            D.m_dCHf[iPh]=0;
            double ActHfRef=D.CalcH(D.m_dTRef, false);
            double EstHfRef=D.m_dH25[D.m_iHfBase];
            CorrHf[iPh]=EstHfRef-ActHfRef;
            if (fabs(ActHfRef)<CorrActHfMin)
              {
              iPhCorr=iPh;
              CorrActHfMin=fabs(ActHfRef);
              }
            }
          CorrSf[iPh]=0;
          if (D.m_bHasSFn && D.m_bS25OK)
            {
            // Must correct Differences and ...
            D.m_dCSf[iPh]=0;
            double ActSfRef=D.CalcS(D.m_dTRef, false);
            double EstSfRef=D.m_dS25[D.m_iHfBase];
            CorrSf[iPh]=EstSfRef-ActSfRef;
            if (fabs(ActSfRef)<CorrActSfMin)
              {
              iPhCorr=iPh;
              CorrActSfMin=fabs(ActSfRef);
              }
            }
        }
      // Adjust m_dCHf for Fns where actual enthalpy functions avail
      for (iPh=0; iPh<C.m_nPhaseData; iPh++)
        {
        CSpecie  &Sp=*C.m_PhaseData[iPh];
        CSpFidelData & D=Sp.m_FData[Fidelity];
        if (D.m_bHasHFn)
          {
          for (int i=0; i<=D.m_iLastTPoint; i++)
            D.m_dCHf[i]=CorrHf[iPhCorr];
          }
        if (D.m_bHasSFn)
          {
          for (int i=0; i<=D.m_iLastTPoint; i++)
            D.m_dCSf[i]=CorrSf[iPhCorr];
          }
        }
      }
    }

#if dbgBuildSDB
  if (dbgBuildEqns())
    {
    for (int Pass=0; Pass<(dbgBuildEqnsBoth()?2:1); Pass++)
      {
      dbgp(TheSepar1);
      if (Pass==0)
        dbgpln(" Basis : kg");
      else
        dbgpln(" Basis : kmol");
      dbgp("    %14s", "Temp Range");
      dbgp(" %+12s", Pass ? "mlHf":"msHf");
      dbgp(" %+12s", Pass ? "mlCHf":"msCHf");
      dbgp(" %+12s", Pass ? "mlHs":"msHs");
      dbgp(" %+12s", Pass ? "mlHe":"msHe");
      dbgp(" %+12s", Pass ? "mlH100":"msH100");
      dbgpln("");

      for (int Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
        {
        for (int c=0; c<m_CDB.No(); c++)
          {
          CComponent & C=*CmD[c];
          for (int iPh=0; iPh<C.m_nPhaseData; iPh++)
            {
            CSpecie  &Sp=*C.m_PhaseData[iPh];
            CSpFidelData  &D=Sp.m_FData[Fidelity];
            for (int iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
              {
              double Scl=Pass==0 ? 1/Sp.MoleWt() : 1;
              dbgp("%s", iSeg==0?"-- ":"   ");
              dbgp("%s", iSeg==D.m_iHfBase?">":" ");
              dbgp("%6.1f->%6.1f", D.m_dTs[iSeg], D.m_dTe[iSeg]);
              if (D.m_bH25OK)
                dbgp(" %12.3f", D.m_dH25[iSeg]*Scl);
              else
                dbgp(" %12s", "");
              dbgp(" %12.3f", D.m_dCHf[iSeg]*Scl);
              dbgp(" %12.3f", D.CalcH(D.m_dTs[iSeg], true)*Scl);
              dbgp(" %12.3f", D.CalcH(D.m_dTe[iSeg], true)*Scl);
              if (D.m_dTs[iSeg]<=C_2_K(100) && D.m_dTe[iSeg]>=C_2_K(100))
                dbgp(" %12.3f", D.CalcH(C_2_K(100), true)*Scl);
              else
                dbgp(" %12s", "");
              dbgpln("  %-10s %s", D.m_pH[iSeg] ? D.m_pH[iSeg]->VName() : D.m_pCp[iSeg]->VName(), Sp.SymOrTag());
              }
            }
          }
        }
      //      }
      //    for (Pass=0; Pass<2; Pass++)
      //      {
      dbgp(TheSepar2);
      if (Pass==0)
        dbgpln(" Basis : kg");
      else
        dbgpln(" Basis : kmol");
      dbgp("    %14s", "Temp Range");
      dbgp(" %+12s", Pass ? "mlSf":"msSf");
      dbgp(" %+12s", Pass ? "mlCS":"msCS");
      dbgp(" %+12s", Pass ? "mlSs":"msSs");
      dbgp(" %+12s", Pass ? "mlSe":"msSe");
      dbgp(" %+12s", Pass ? "mlS100":"msS100");
      dbgpln("");
      for (Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
        {
        for (int c=0; c<m_CDB.No(); c++)
          {
          CComponent & C=*CmD[c];
          for (int iPh=0; iPh<C.m_nPhaseData; iPh++)
            {
            CSpecie  &Sp=*C.m_PhaseData[iPh];
            CSpFidelData  &D=Sp.m_FData[Fidelity];
            for (int iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
              {
              double Scl=Pass==0 ? 1/Sp.MoleWt() : 1;
              dbgp("%s", iSeg==0?"-- ":"   ");
              dbgp("%s", iSeg==D.m_iHfBase?">":" ");
              dbgp("%6.1f->%6.1f", D.m_dTs[iSeg], D.m_dTe[iSeg]);
              if (D.m_bS25OK)
                dbgp(" %12.3f", D.m_dS25[iSeg]*Scl);
              else
                dbgp(" %12s", "");
              dbgp(" %12.3f", D.m_dCSf[iSeg]*Scl);
              dbgp(" %12.3f", D.CalcS(D.m_dTs[iSeg], true)*Scl);
              dbgp(" %12.3f", D.CalcS(D.m_dTe[iSeg], true)*Scl);
              if (D.m_dTs[iSeg]<=C_2_K(100) && D.m_dTe[iSeg]>=C_2_K(100))
                dbgp(" %12.3f", D.CalcS(C_2_K(100), true)*Scl);
              else
                dbgp(" %12s", "");
              dbgpln("  %-10s %s", D.m_pS[iSeg] ? D.m_pS[iSeg]->VName() : D.m_pCp[iSeg]->VName(), Sp.SymOrTag());
              }
            }
          }
        }
      }
    dbgpln("");
    }
#endif

  for (Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
    {
    for (int c=0; c<m_CDB.No(); c++)
      {
      CComponent & C=*CmD[c];
      double iSpH0Min = -1;
      double dHf0Min = 1e30;
      for (int iPh=0; iPh<C.m_nPhaseData; iPh++)
        {
        CSpecie  &Sp=*C.m_PhaseData[iPh];
        CSpFidelData  &D=Sp.m_FData[Fidelity];
        D.m_dHf0=D.CalcH(ZeroCinK, true);
        if (D.m_dHf0<dHf0Min)
          {
          dHf0Min=D.m_dHf0;
          iSpH0Min=Sp.iSId;
          }
        }
      for (iPh=0; iPh<C.m_nPhaseData; iPh++)
        {
        CSpecie  &Sp=*C.m_PhaseData[iPh];
        CSpFidelData  &D=Sp.m_FData[Fidelity];

        D.m_dHfBase0=dHf0Min;
        for (int iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
          {
          D.m_dCZRef[iSeg]=D.m_dCHf[iSeg]-D.m_dHf0;
          D.m_dCdHf[iSeg]=D.m_dCHf[iSeg]-dHf0Min;
          }
        }
      }
    }

#if dbgBuildSDB
  if (dbgBuildEqns())
    {
    for (int Pass=0; Pass<(dbgBuildEqnsBoth()?2:1); Pass++)
      {
      dbgpln(TheSepar1);
      dbgpln(" References & Constants of integration");
      if (Pass==0)
        dbgpln(" Basis : kg");
      else
        dbgpln(" Basis : kmol");

      dbgp("  %3s", "Seg");
      dbgp(" %4s", "");
      dbgp(" %6s", "TRef");
      dbgp(" %4s", "");
      dbgp(" %8s", "Ts");
      dbgp(" %8s", "Te");
      dbgp(" %12s", "H@25");
      dbgp(" %12s", "CHf");
      dbgp(" %12s", "CdHf");
      dbgp(" %12s", "CZRef");
      dbgp(" %12s", "Hf0");
      dbgp(" %12s", "HfBase0");
      dbgp("  %12s", "S@25");
      dbgp(" %12s", "CSf");
      dbgpln("");

      for (int Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
        {
        for (int c=0; c<m_CDB.No(); c++)
          {
          CComponent & C=*CmD[c];
          for (int iPh=0; iPh<C.m_nPhaseData; iPh++)
            {
            CSpecie  &Sp=*C.m_PhaseData[iPh];
            CSpFidelData &D=Sp.m_FData[Fidelity];
            for (int iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
              {
              char Buff[256];
              double Scl=Pass==0 ? 1/Sp.MoleWt() : 1;
              dbgp("%2s %2i", iSeg==0?"--":"", iSeg);
              dbgp(" %4s", Fidelity==0?"LoFi":"HiFi");
              dbgp(" %6.2f", D.m_dTRef);

              dbgp(" %4s",D.m_bCpOK ? "CpOK":"  ");
              dbgp(" %8.2f", K2C(D.m_dTs[iSeg]));
              dbgp(" %8.2f", K2C(D.m_dTe[iSeg]));
              sprintf(Buff, "%12.3f", D.m_dH25[iSeg]*Scl);
              dbgp(" %12s",  D.m_bH25OK?Buff:"");
              dbgp(" %12.3f", D.m_dCHf[iSeg]*Scl);
              dbgp(" %12.3f", D.m_dCdHf[iSeg]*Scl);
              dbgp(" %12.3f", D.m_dCZRef[iSeg]*Scl);
              dbgp(" %12.3f", D.m_dHf0*Scl);
              dbgp(" %12.3f", D.m_dHfBase0*Scl);

              dbgpln("  %-10s %s", D.m_pH[iSeg] ? D.m_pH[iSeg]->VName() : D.m_pCp[iSeg]->VName(), Sp.SymOrTag());
              }
            }
          }
        }
      dbgpln("");
      dbgp("  %3s", "Seg");
      dbgp(" %4s", "");
      dbgp(" %6s", "TRef");
      dbgp(" %+8s", "Ts");
      dbgp(" %+8s", "Te");
      dbgp(" %4s", "");
      dbgp(" %12s", "S@25");
      dbgp(" %12s", "CSf");
      dbgpln("");

      for (Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
        {
        for (int c=0; c<m_CDB.No(); c++)
          {
          CComponent & C=*CmD[c];
          for (int iPh=0; iPh<C.m_nPhaseData; iPh++)
            {
            CSpecie  &Sp=*C.m_PhaseData[iPh];
            CSpFidelData &D=Sp.m_FData[Fidelity];
            for (int iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
              {
              char Buff[256];
              double Scl=Pass==0 ? 1/Sp.MoleWt() : 1;
              dbgp("%2s %2i", iSeg==0?"--":"", iSeg);
              dbgp(" %4s", Fidelity==0?"LoFi":"HiFi");
              dbgp(" %6.2f", D.m_dTRef);

              dbgp(" %4s",D.m_bCpOK ? "CpOK":"  ");
              dbgp(" %8.2f", K2C(D.m_dTs[iSeg]));
              dbgp(" %8.2f", K2C(D.m_dTe[iSeg]));

              sprintf(Buff, "%12.3f", D.m_dS25[iSeg]*Scl);
              dbgp(" %12s",  D.m_bS25OK?Buff:"");
              dbgp(" %12.3f", D.m_dCSf[iSeg]*Scl);

              dbgpln("  %-10s %s", D.m_pH[iSeg] ? D.m_pH[iSeg]->VName() : D.m_pCp[iSeg]->VName(), Sp.SymOrTag());
              }
            }
          }
        }
      }
    dbgpln(TheSepar1);
    }
#endif
  return Bad;
  }

//---------------------------------------------------------------------------

flag CComponentDataBase::Initialise(CDBInitRec & CDBInit, char* PrjDBFn, flag UseLibDB, flag H2OAsAqueous, flag DoTest/*=true*/)
  {
  m_EDB.Init();
  m_SDB.Init();
  //SVI.Init();
  Init();

  m_H2O.Initialise();
  //m_H2O.Select(SaturatedH2OName);
  m_H2O.Select(IF97H2OName);//IFC67H2OName);

  MinT=CDBInit.MinT;
  MaxT=CDBInit.MaxT;
  MinP=CDBInit.MinP;
  MaxP=CDBInit.MaxP;
  PrjDataFile=PrjDBFn;
  UseStdFns4H2O = true;//ALWAYS  CDBInit.UseStdFns4H2O;
  m_pWater=NULL;
  m_pSteam=NULL;

  m_DefFlashMethod = OrigPrjFileVerNo()<103 ? SMFM_TotalP : SMFM_PartialP;
  m_DefFlashCmpIndex=-1;
  m_DefFlashLiqIndex=-1;
  m_DefFlashVapIndex=-1;

  bCDBBad=false;

#if dbgBuildSDB
  if (dbgBuildSpc())
    {
    dbgpln("---------------------------------------------------------");
    CRqdSpListIter IRqdSpc(CDBInit.RqdSpecies);
    for (CRqdSpItem *pRS=IRqdSpc.First(); pRS; pRS=IRqdSpc.Next())
      dbgpln("  Rqd Specie      : %s", pRS->m_Name());
    }
#endif

  // Extend Specie & Component List
  // SpecieBlks only Reference one specie
  // Add Reqd Species to List
  if (!m_ForTestingOnly)
    {

    for (CSpecieBlk* pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
      {
      //Strng C=pSb->m_sCmpName;
      //Strng S, P;

      //// default Postfix
      //switch(pSb->eOcc)
      //  {
      //  case BOT_Null   :  DoBreak();       break;
      //  case BOT_Solid  :  P="(s)"; break;
      //  case BOT_Liquid :  P="(l)"; break;
      //  case BOT_Gas :  P="(g)"; break;
      //  }

      //// Find Postfix in CmpName
      //char * pStart=C.XStrRChr('(');
      //if (pStart)
      //  {
      //  P=pStart;
      //  C.SetLength(C.Length()-P.Length());
      //  }

      //S=C;
      //S+=P;

      //pSb->m_sCmpName   = C;
      //pSb->m_sSpName    = S;
      //pSb->m_sSpPostfix = P;

      }

    for (CSpecieBlk* pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
      {
      pSb->iCId=-1;
      if (!pSb->m_fOpt)
        {
        Strng &S=pSb->m_sSpName;
        if (IsSpcName(S))
          SVI.ParseSpecie(S(), NULL, NULL, NULL, NULL, CDBInit, CSysVecItem::DuplicateOK);
        }
      }
    }

#if dbgBuildSDB
  if (dbgBuildSpc())
    {
    dbgpln("---------------------------------------------------------");
    CRqdSpListIter IRqdSpc(CDBInit.RqdSpecies);
    for (CRqdSpItem *pRS=IRqdSpc.First(); pRS; pRS=IRqdSpc.Next())
      dbgpln("  Rqd Specie      : %s", pRS->m_Name());
    }
#endif

  // Build Specie & Component List
  CRqdSpList RqdSpc;
  Strng_List RqdCmp;
  CRqdSpListIter IRqdSpc(CDBInit.RqdSpecies);
  for (CRqdSpItem *pRS=IRqdSpc.First(); (pRS); pRS=IRqdSpc.Next())
    if (pRS->m_iDuplicate<0)
      {
      Strng S=pRS->m_Name;
      Strng C=CmpName(S);
      int i = S.Find("(");
      BOOL Err = (i==0);
      if (!Err)
        Err = (S.FindFrom(i+1, "(")>=0);
      if (Err)
        {
        LogError("RqdSpecies", LF_Exclamation, "Illegal use of '(' in specie : '%s'", S.Str());
        bCDBBad = true;
        }

      // IndexValue==1 Means All Components
      flag IsASpc=IsSpcName(S);
      if (IsASpc && RqdSpc.Find(S(), true)==NULL)
        RqdSpc.AddTail(*pRS);

      Strng *pC=RqdCmp.Find(C(), true);
      if (pC==NULL)
        pC=RqdCmp.Append(C());

      pC->SetIndex(pC->Index() || !IsASpc);
      }

#if dbgBuildSDB
    if (dbgBuildSpc())
      {
      dbgpln("---------------------------------------------------------");
      for (pStrng m_pS=RqdCmp.First(); m_pS; m_pS=RqdCmp.Next())
        dbgpln("  Rqd Comp [%s]  : %s", m_pS->Index()?"All":"   ", m_pS->Str());
      CRqdSpListIter IRqdSpc(RqdSpc);
      for (CRqdSpItem *pRS=IRqdSpc.First(); pRS; pRS=IRqdSpc.Next())
        dbgpln("  Rqd Specie      : %s", pRS->m_Name());
      dbgpln("---------------------------------------------------------");
      }
#endif

    if (!LoadComponents(CDBInit, PrjDBFn, RqdCmp, RqdSpc, H2OAsAqueous))
      {
      LogError(SpecieDBLogHdr, LF_Exclamation, "Specie Data not Loaded");
      bCDBBad=true;
      return false;
      }

    if (!CheckCommonData())
      {
      bCDBBad=true;
      return false;
      }
      

    if (!BuildSpecies(CDBInit, RqdCmp, RqdSpc))
      {
      bCDBBad=true;
      return false;
      }

    BIP.SetSize(m_CDB.No(), m_CDB.No());

    LoadBIPs(CDBInit, PrjDBFn);

    if (!SVI.Finalise(CDBInit))
      {
      bCDBBad=true;
      return false;
      }
    //flag Ok=true;
    if (!m_ForTestingOnly)
      {
      int nNotLoaded=0;
      Strng NotLoadedList;
      for (CSpecieBlk * pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
        {
        flag DoneErr=False;
        //pSb->iSId=m_SDB.Find(pSb->m_sSpName());

        // First look for the Aq Version of the specie
        if (pSb->m_fAllowAq) 
          {
          Strng S(pSb->m_sSpName()), T;
          char *p=S.XStrRChr('(');
          if (p)
            {
            S.SetLength(p-S());
            T=S;
            T+="(aq)";
            pSb->iSId=m_SDB.Find(T());
            if (pSb->iSId<0)
              {
              T=S;
              T+="(a)";
              pSb->iSId=m_SDB.Find(T());
              }
            }
          }

        if (pSb->iSId<0) 
          pSb->iSId=m_SDB.Find(pSb->m_sSpName());

        if (pSb->iCId>=0)
          pSb->MW=m_CDB[pSb->iCId].m_dMoleWt;
        else if (!pSb->m_fOpt)
          {
          DoneErr=True;
          LogError(SpecieDBLogHdr, 0, "Component '%s' not found in Specie Database.\n\nRequired by : %s", pSb->m_sCmpName(), pSb->m_sFile());
          if (nNotLoaded)
            NotLoadedList+=", ";
          NotLoadedList+=pSb->m_sSpName();
          nNotLoaded++;
          }

        if (pSb->iSId<0 && !pSb->m_fOpt)
          {
          Strng S=pSb->m_sSpPostfix;
          //switch(pSb->eOcc)
          //  {
          //  case BOT_Null  :   DoBreak();   break;
          //  case BOT_Solid :   S+="Solid";  break;
          //  case BOT_Liquid:   S+="Liquid"; break;
          //  case BOT_Gas:   S+="Vapour"; break;
          //  }

          LogError(SpecieDBLogHdr, 0, "Specie '%s' %s phase not found in Specie Database.\n\nRequired by : %s", pSb->m_sSpName(), S(), pSb->m_sFile);
          if (!DoneErr)
            {
            if (nNotLoaded)
              NotLoadedList+=", ";
            NotLoadedList+=pSb->m_sSpName();
            nNotLoaded++;
            }
          }
        }

      if (nNotLoaded)
        {
        if (NotLoadedList.Len()>512)
          {
          NotLoadedList = NotLoadedList.Left(512);
          NotLoadedList += "...";
          }
        Strng Msg;
        Msg.Set("%i Species required by model DLL files not loaded   [ %s ]", nNotLoaded, NotLoadedList());
        if (PrjFileVerNo()<51 || UseLibDB)
          {//verbose description of possible error...
          LogError(SpecieDBLogHdr, 0, Msg());
          Msg+="\n\nCheck:\n";
          if (UseLibDB)
            Msg.Append("1) Specie data is no longer loaded from default.mdb, import or edit the\n   required specie data in syscad.mdb located at '%s'\n", PrjDBFn);
          else
            Msg.Append("1) Specie data in syscad.mdb located at '%s'\n", PrjDBFn);
          Msg+="2) List of species required in configuration file.";
          AfxMessageBox(Msg());
          //LogError(SpecieDBLogHdr, LF_Exclamation, Msg());
          }
        else
          LogError(SpecieDBLogHdr, LF_Exclamation, Msg());
        bCDBBad=true;
        }
      }

    CopyDataForMultiFidelity();

    // Load Density Corrections and Solubility functions
    if (!bCDBBad)
      {
      LoadSolnInfo(CDBInit, PrjDBFn);
      //for (int s=0; s<No(); s++)
      for (int s=0; s<m_SDB.m_iNo; s++)
        {
        if (m_SDB[s].m_DensCorrs.GetSize()>0)
          m_SDB.m_DensCorrSps.Add(s);
        if (m_SDB[s].m_CpCorrs.GetSize()>0)
          m_SDB.m_CpCorrSps.Add(s);
        if (m_SDB[s].m_Solubilities.GetSize()>0)
          m_SDB.m_SolubleSps.Add(s);
        }

      if (CDBInit.NSpDensCorrPrj)
        LogNote(SpecieDBLogHdr, 0, "%i Species have density correction functions (Loaded from %s)", CDBInit.NSpDensCorrPrj, PrjDataFile());
      if (CDBInit.NSpSolFnPrj)
        LogNote(SpecieDBLogHdr, 0, "%i Species have solubility functions (Loaded from %s)", CDBInit.NSpSolFnPrj, PrjDataFile());
      }

    // Fix Integration Constants
    if (!bCDBBad)
      bCDBBad=FixOffsetConstants();


    if (!bCDBBad) // Set Ranges Tolerances
      {
      for (int s=0; s<m_SDB.m_iNo; s++)
        {
        CRqdSpItem * pRS=RqdSpc.Find(m_SDB[s].SymOrTag());
        if (pRS)
          {
          if (pRS->m_dLoTol>=0)
            m_SDB[s].m_dTsTol=Min(2000.0,pRS->m_dLoTol);
          if (pRS->m_dHiTol>=0)
            m_SDB[s].m_dTeTol=Min(2000.0,pRS->m_dHiTol);
          }
        }

      if (0)
        {
        dbgpln("Solids : %08x", som_Sol);
        dbgpln("Liquids: %08x", som_Liq);
        dbgpln("Vapours: %08x", som_Gas);
        for (int c=0; c<m_CDB.m_iNo; c++)
          {
          for (int j=0; j<m_CDB[c].m_nPhaseData; j++)
            {
            CSpecie  &Sp=*m_CDB[c].m_PhaseData[j];
            dbgpln("PhaseName: %3i %08x, %-20s %s %s", Sp.m_PhInx, Sp.m_PhMsk, m_CDB[c].Sym(), Sp.m_sPhaseSymVisible(), Sp.m_sPhaseSymActual());
            }
          }
        }
      }

    if (DoTest && !bCDBBad) // Test Ranges
      {
      for (int iFidelity=0; iFidelity<2; iFidelity++)
        {
        bool Fidelity=(iFidelity==1);
        for (int s=0; s<m_SDB.m_iNo; s++)
          // Exclude Aqueous Species and Water from Test
          {
          CSpecie * pSp=&m_SDB[s];
          if (pSp!=m_pWater && pSp!=m_pSteam &&
            !pSp->IsAqueous() && pSp->CpDirect())
            {
            for (int Fidelity=0; Fidelity<MaxFidelity; Fidelity++)
              {
              CSpFidelData & D=m_SDB[s].m_FData[Fidelity];
              for (int iSeg=0; iSeg<=D.m_iLastTPoint; iSeg++)
                {
                double Rho=D.SpRho(D.m_dTs[iSeg]+1.0,Std_P);
                if (Rho<1.0e-6)
                  {
                  LogError(SpecieDBLogHdr, 0, "Bad Density %g %s", Rho, pSp->SymOrTag());
                  bCDBBad=true;
                  }
                }
              const double T_a = D.LoT();
              const double T_b = D.TestLoT();
              //double T=Max(MinT, T_a - T_b); //kga surely this is wrong!?!?
              double T=Max(MinT, T_a);
              double PrevEnth=pSp->mlHf(Fidelity, T, Std_P, NULL, NULL), Enth;
              const double dT = 5.0;//10.0;
              for (T+=dT ; T<=Min(MaxT, D.TestHiT()); T+=dT)
                {
                Enth=pSp->mlHf(Fidelity, T, Std_P, NULL, NULL);
                if (Enth<PrevEnth)
                  {
                  LogError(SpecieDBLogHdr, 0, "Bad Enthalpy : mlH decreases with T %s (error at T=%gK using test interval %gK)", pSp->SymOrTag(), T, dT);
                  bCDBBad=true;
                  break;
                  }
                if (Enth<=PrevEnth+1.0e-6)
                  {
                  LogError(SpecieDBLogHdr, 0, "Bad Enthalpy : mlH increases too slowly with T %s (error at T=%gK using test interval %gK)", pSp->SymOrTag(), T, dT);
                  bCDBBad=true;
                  break;
                  }
                PrevEnth=Enth;
                }
              }
            }
          }
        }
      }


    if (!bCDBBad)
      {
      m_SDB.m_Map.InitHashTable((m_SDB.m_iNo*3)/2+1);
      for (int s=0; s<m_SDB.m_iNo; s++)
        {
        m_SDB.m_Map.SetAt(m_SDB[s].Sym(), s);
        m_SDB.m_Map.SetAt(m_SDB[s].Tag(), s);
        }

      m_SDB.DDBSpList.Empty();
      m_SDB.DDBSpListDash.Empty();
      m_SDB.DDBSolSpList.Empty();
      m_SDB.DDBSolSpListDash.Empty();
      m_SDB.DDBLiqSpList.Empty();
      m_SDB.DDBLiqSpListDash.Empty();
      m_SDB.DDBVapSpList.Empty();
      m_SDB.DDBVapSpListDash.Empty();

      m_SDB.DDBSpListDash.Add(-1, "-");
      m_SDB.DDBSolSpListDash.Add(-1, "-");
      m_SDB.DDBLiqSpListDash.Add(-1, "-");
      m_SDB.DDBVapSpListDash.Add(-1, "-");

      for (int i=0; i<m_SDB.Count(); i++)
        {
        m_SDB.DDBSpList.Add(i, m_SDB[i].SymOrTag());
        m_SDB.DDBSpListDash.Add(i, m_SDB[i].SymOrTag());
        if (m_SDB[i].PhaseIs(m_som_Sol))
          {
          m_SDB.DDBSolSpList.Add(i, m_SDB[i].SymOrTag());
          m_SDB.DDBSolSpListDash.Add(i, m_SDB[i].SymOrTag());
          }
        if (m_SDB[i].PhaseIs(m_som_Liq))
          {
          m_SDB.DDBLiqSpList.Add(i, m_SDB[i].SymOrTag());
          m_SDB.DDBLiqSpListDash.Add(i, m_SDB[i].SymOrTag());
          }
        if (m_SDB[i].PhaseIs(m_som_Gas))
          {
          m_SDB.DDBVapSpList.Add(i, m_SDB[i].SymOrTag());
          m_SDB.DDBVapSpListDash.Add(i, m_SDB[i].SymOrTag());
          }
        }

      DDBCompListDash.Empty();
      DDBCompListMix.Empty();
      DDBCompListMixImg.Empty();
      DDBCompListDash.Add(-1, "-");
      DDBCompListDashVapLiq.Add(-1, "-");
      DDBCompListMix.Add(m_CDB.MixtureId(), "Mixture");
      DDBCompListMixImg.Add(m_CDB.MixtureId(), "Mixture");
      DDBCompListMixImg.Add(m_CDB.ImageId(), "MixImage");
      for (int c=0; c<m_CDB.No(); c++)
        {
        DDBCompList.Add(c, m_CDB[c].SymOrTag());
        DDBCompListMix.Add(c, m_CDB[c].SymOrTag());
        DDBCompListDash.Add(c, m_CDB[c].SymOrTag());
        if (m_CDB[c].LiqPhInx()>=0 && m_CDB[c].VapPhInx()>=0)
          {
          DDBCompListVapLiq.Add(c, m_CDB[c].SymOrTag());
          DDBCompListDashVapLiq.Add(c, m_CDB[c].SymOrTag());
          }
        }
      }

    SVI.SetUpSkipList(0);
    SVI.SetUpSkipList(m_som_Gas);
    SVI.SetUpSkipList(m_som_Liq);
    SVI.SetUpSkipList(m_som_Sol);
    SVI.SetUpSkipList(m_som_SL);
    SVI.SetUpSkipList(m_som_ALL);

    SVI.SetUpSkipListRev(0);
    SVI.SetUpSkipListRev(m_som_Gas);
    SVI.SetUpSkipListRev(m_som_Liq);
    SVI.SetUpSkipListRev(m_som_Sol);
    SVI.SetUpSkipListRev(m_som_SL);
    SVI.SetUpSkipListRev(m_som_ALL);

    // Find Species for Heat of combustion
    m_SDB.m_iH2Ol = SDB.Find("H2O(l)");
    m_SDB.m_iCO2l = SDB.Find("CO2(l)");
    m_SDB.m_iH2Og = SDB.Find("H2O(g)");
    m_SDB.m_iCO2g = SDB.Find("CO2(g)");

    LPCTSTR Flash_Component= "H2O";
    m_DefFlashCmpIndex=CDB.Find(Flash_Component);
    m_DefFlashLiqIndex=SDB.Find(CString(Flash_Component)+="(l)");
    m_DefFlashVapIndex=SDB.Find(CString(Flash_Component)+="(g)");
    if (m_DefFlashLiqIndex<0 || m_DefFlashVapIndex<0)
      m_DefFlashCmpIndex=-1;

    if (m_DefFlashCmpIndex<0)
      LogError("Specie Database",0,"Default Flash Component %s not found", Flash_Component);

    m_Flashable.SetSize(0);
    for (int i=0; i<CDB.No(); i++)
      {
      if (CDB[i].VpOK() && (CDB[i].LiqPhInx("l", true)>=0 || CDB[i].LiqPhInx("aq", true)>=0) && (CDB[i].VapPhInx("g", true)>=0))
        {
        m_Flashable.Add(i);
        }
      else
        {
        if (i==m_DefFlashCmpIndex)
          {
          m_DefFlashCmpIndex=-1;
          m_DefFlashLiqIndex=-1;
          m_DefFlashVapIndex=-1;
          LogError("Specie Database",0,"Default Flash Component %s not flashable", Flash_Component);
          }
        }
      }

    m_DDBFlashable.Add(CDB.No(), "<Default>");
    m_FlashCmps.SetSize(CDB.No());
    m_FlashCmpsI.SetSize(CDB.No());
    for (int i=0; i<CDB.m_Flashable.GetCount(); i++)
      {
      int c=CDB.m_Flashable[i];
      m_DDBFlashable.Add(c, CDB[c].SymOrTag());
      m_FlashCmps[c]=CDB[c].SymOrTag();
      m_FlashCmpsI[c]="<";
      m_FlashCmpsI[c]+=CDB[c].SymOrTag();
      m_FlashCmpsI[c]+=">";
      }

    m_DDBFlashMethods.Add(SMFM_Default,      "<Default>");
    m_DDBFlashMethods.Add(SMFM_PartialP,     "PartialP");
    m_DDBFlashMethods.Add(SMFM_TotalP,       "TotalP");
    m_DDBFlashMethods.Add(SMFM_Undefined,    "Undefined", MDD_NoSelect|MDD_Hidden);

    m_FlashMethods.SetAtGrow(SMFM_PartialP,  "PartialP");
    m_FlashMethods.SetAtGrow(SMFM_TotalP,    "TotalP");
    m_FlashMethodsI.SetAtGrow(SMFM_PartialP, "<PartialP>");
    m_FlashMethodsI.SetAtGrow(SMFM_TotalP,   "<TotalP>");


    if (1)
      {
      dbgpln("Components:");
      for (int i=0; i<CDB.No(); i++)
        {
        dbgpln("%3i %-20s", i, CDB[i].SymOrTag());
        }
      dbgpln("Species:");
      for (int i=0; i<SDB.Count(); i++)
        {
        dbgpln("%3i %-20s %-20s", i, SDB[i].Sym(), SDB[i].Tag());
        }
      }

#if dbgBuildSDB
    if (dbgBuildLoops())
      {
      SpecieIter SIVap(m_som_Gas);
      SpecieIter SILiq(m_som_Liq);
      SpecieIter SISol(m_som_Sol);
      SpecieIter SISL (m_som_SL);
      SpecieIter SIALL(m_som_ALL);

      int i;
      dbgpln("Specie Vap ----------");
      for (i=SIVap.ClcInx1(); SIVap.Loop(i); )
        dbgpln("  %3i) %s", i, SVImg(i).FullTag());
      dbgpln("Specie Liq ----------");
      for (i=SILiq.ClcInx1(); SILiq.Loop(i); )
        dbgpln("  %3i) %s", i, SVImg(i).FullTag());
      dbgpln("Specie Sol ----------");
      for (i=SISol.ClcInx1(); SISol.Loop(i); )
        dbgpln("  %3i) %s", i, SVImg(i).FullTag());
      dbgpln("Specie SL  ----------");
      for (i=SISL.ClcInx1(); SISL.Loop(i); )
        dbgpln("  %3i) %s", i, SVImg(i).FullTag());
      dbgpln("Specie ALL ----------");
      for (i=SIALL.ClcInx1(); SIALL.Loop(i); )
        dbgpln("  %3i) %s", i, SVImg(i).FullTag());

      dbgpln("Components ----------");
      dbgpln("  #     si  li  ai  vi  ci");
      for (i=0; i<m_CDB.No(); i++)
        dbgpln("  %3i) %3i %3i %3i %3i %s", i, m_CDB[i].SolPhInx(), m_CDB[i].LiqPhInx(), m_CDB[i].VapPhInx(), m_CDB[i].CmpInx(), m_CDB[i].SymOrTag());

      }
#endif

    return !bCDBBad;
    //return Ok;
  }




//---------------------------------------------------------------------------
inline double LclChkZero(double x) { return ((fabs(x) > 1e-4) ? x : 0.0);}

void CComponentDataBase::DumpTest()
  {
  /***
  double T1=073.16, T2=2073.15;
  dbgpln("=================================================================================================================================================");
  dbgpln("Specie Disp Sequernce");
  for (int s=0; s<m_SDB.m_iNo; s++)
  dbgpln("%s", m_SDB[s].SymOrTag());

  dbgpln("Specie Test Dump");

  for (int c=0; c<m_CDB.m_iNo; c++)
  {
  dbgpln("-------------------------------------------------------------------------------------------------------------------------------------------------");
  for (int j=0; j<m_CDB[c].nPhaseData; j++)
  {
  int s=m_CDB[c].PhaseData[j]->iSId;
  CSpecie & Sp=m_SDB[s];
  dbgpln("---------------- %s", Sp.SymOrTag());
  dbgpln("Tc=%10.1f", Sp.TCrit());
  dbgpln("Pc=%10.1f", Sp.PCrit());
  dbgpln("Vc=%10.1f", Sp.VCrit());
  dbgpln("MW=%10.1f", Sp.MoleWt());
  dbgpln(
  //"  %6s %6s %6s nT Ti %10s %10s %10s %10s %10s %10s %10s",
  //" %6s %6s %6s nT Ti %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s",
  " %6s %6s Ti    %10s %10s %10s %10s %10s %10s %10s %10s %10s",// %10s %10s %10s",
  "tK", "tC", //"tF",
  //"msHs(SP)",
  "msHs",
  "msHz",
  "msHf",
  //"msHf0",
  //"msDHf",
  //"msDHs",
  //"msDHz",
  "msDHf",
  "msCp",
  "msSf",
  "msGf",
  //"msCpdHfEst",
  //"msCpHfEst",
  //"mlCp",
  "VapP",
  "Rho");
  double dt=50.0,t1=T1-dt, t=0, tp, vp=0,vpp;//floor((t2-t1)/10),t=t1;
  flag Done298=0;

  if (Sp.sCmpTag.XStrICmp("Water")==0 || Sp.sCmpTag.XStrICmp("Steam")==0)
  { int xxx=0; }

  for (int i=0; i <= 30; i++)
  {
  tp=t;
  vpp=vp;
  if (i>20)
  dt=100.0;
  char vps[1024];
  if (t1< 298.16 && t1+dt>298.16 && !Done298)
  {
  t=298.16;
  Done298=1;
  }
  else //if (i==2)
  {
  t1=t1+dt;
  t=t1;
  }

  //dbgp  (" %6.1f %6.1f %6.1f %2i %2i",t, K_2_C(t), K_2_F(t), Sp.m_iLastTPoint+1, Sp.FindTIndex(t));
  dbgp  (" %6.1f %6.1f %2i %2i",t, K_2_C(t), Sp.m_iLastTPoint+1, Sp.FindTIndex(t));
  vp=Sp.VapourP(Fidelity, t);
  //dbgp  (" %10.1f", LclChkZero(Sp.msHs(t, Std_P, NULL, NULL)));
  dbgp  (" %10.1f", LclChkZero(Sp.msHs(Fidelity, t, Std_P, NULL, NULL)));
  dbgp  (" %10.1f", LclChkZero(Sp.msHz(Fidelity, t, Std_P, NULL, NULL)));
  dbgp  (" %10.1f", LclChkZero(Sp.msHf(Fidelity, t, Std_P, NULL, NULL)));
  //dbgp  (" %10.1f", LclChkZero(Sp.msHf0()));
  //dbgp  (" %10.1f", LclChkZero(Sp.msdHf0()));
  if (j>0)
  {
  int s1=m_CDB[c].PhaseData[j-1]->iSId;
  double vp1=m_SDB[s1].VapourP(t);
  dbgp  (" %10.1f", LclChkZero(Sp.msHf(Fidelity, t, Std_P, NULL, NULL)-m_SDB[s1].msHf(Fidelity, t, Std_P, NULL, NULL)));
  }
  else
  dbgp  (" %10s ", "");

  dbgp  (" %10.4f", Sp.msCp(Fidelity, t, vp, NULL, NULL));
  dbgp  (" %10.1f", LclChkZero(Sp.msSf(Fidelity, t, Std_P, NULL, NULL)));
  dbgp  (" %10.1f", LclChkZero(Sp.msGf(Fidelity, t, Std_P, NULL, NULL)));

  //if (i>0)
  //  {
  //  dbgp  (" %10.4f", LclChkZero(Sp.msHz(t, vp, NULL, NULL)-Sp.msHz(tp, vpp, NULL, NULL))/(t-tp));
  //  dbgp  (" %10.4f", LclChkZero(Sp.msHf(t, vp, NULL, NULL)-Sp.msHf(tp, vpp, NULL, NULL))/(t-tp));
  //  }
  //else
  //  dbgp  (" %10s %10s", "", "");

  sprintf(vps, vp<1.0e5 ? "%10.3f":"%10.3e", vp);
  dbgp  (" %10s", vps);
  if (Sp.m_bfDensInDirect)
  dbgp  (" %10s", " ---");
  else
  dbgp  (" %10.3f", LclChkZero(Sp.Density(Fidelity, t, Std_P, NULL, NULL)));
  dbgpln("");
  }
  }
  dbgpln(" ");
  }
  dbgpln("=================================================================================================================================================");
  **/
  };

//---------------------------------------------------------------------------

flag CComponentDataBase::Terminate()
  {
  m_H2O.Terminate();
  Term();
  //SVI.Term();
  m_SDB.Term();
  m_EDB.Term();
  return true;
  };

//---------------------------------------------------------------------------

int CComponentDataBase::Add(CComponent * CD)
  {
  if (m_iNo == MaxComponents)
    {
    LogWarning(SpecieDBLogHdr, 0, "Too Many Components %s", CD->SymOrTag());
    return 0;
    }

  CmD[m_iNo] = CD;
  CD->iCId=m_iNo;
  return m_iNo++;
  };

//--------------------------------------------------------------------------

int CComponentDataBase::Find(const char * pSpName)
  {
  for (word i=0; i<m_iNo; i++)
    {
    if (CmD[i]==NULL)
      continue;
    if (CmD[i]->Sym.XStrCmp(pSpName)==0)
      return i;
    //if (CmD[i]->Tag.XStrCmp(pSpName)==0)
    //  return i;
    for (int j=0; j<CmD[i]->m_nPhaseData; j++)
      if (CmD[i]->m_PhaseData[j]->sCmpTag.XStrCmp(pSpName)==0)
        return i;
    }
  return -1;
  };

//---------------------------------------------------------------------------

//CSpecieD * CComponentDataBase::FindPhaseChange(double TRef, flag Above)
//  {
//  if (Above)
//    {
//    for (int i=0; i<nPhSeq; i++)
//      if (PhSeq[i].T>TRef)
//        return PhSeq[i].pPD;
//    }
//  else
//    {
//    for (int i=nPhSeq-1; i>=0; i--)
//      if (PhSeq[i].T<TRef)
//        return PhSeq[i].pPD;
//    }
//  return NULL;
//  };

//===========================================================================
//
//
//
//===========================================================================
//void CSpecieBlk::AttachSelectableFns()
//  {
//  for (CSpecieBlk * pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
//    pSb->Attach();
//  };
//void CSpecieBlk::DetachSelectableFns()
//  {
//  for (CSpecieBlk * pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
//    pSb->Detach();
//  };

void CSpecieBlk::TestInitialiseOK()
  {
  for (CSpecieBlk * pSb=CSpecieBlk::sm_pFirst; (pSb); pSb=pSb->m_pNxt)
    {
    //if (!Exists()
    MInitialiseTest &IT=pSb->m_InitTest;
    if (!pSb->Exists() && IT.ObjClass())
      {
      IT.ObjClass()->SetSelectable(false);
      }
    }
  };

CSpecieBlk::CSpecieBlk(MInitialiseTest & InitTest, LPCTSTR pName, LPCTSTR pFile, LPCTSTR pDll) :
  m_InitTest(InitTest)
  {
  ASSERT_ALWAYS(strchr(pName, '(')!=0, "must be the full specie name", __FILE__, __LINE__);
  //m_pTagObjClass=ObjClass;
  m_sSpName=pName; // Just in case needed for MdlCfg
  m_sSpName.Trim(); // Just in case
  m_sCmpName=pName;
  m_sCmpName[strchr(pName, '(')-pName]=0; // remove phase name
  m_sCmpName.Trim(); // Just in case
  m_sSpPostfix=strchr(pName, '(');

  m_sFile.FnName((char*)pFile);

  m_sDll=pDll;
  // Link into chain
  if (sm_pFirst)
    {
    m_pNxt=sm_pFirst; // Link int chain
    while (m_pNxt->m_pNxt) m_pNxt=m_pNxt->m_pNxt;
    m_pNxt->m_pNxt=this;
    m_pNxt=NULL;
    }
  else
    {
    sm_pFirst=this;
    m_pNxt=NULL;
    }

  //m_pObjClass=pClass;

  iCId=-1;
  iSId=-1;
  iAltId=-1;
  //eOcc=BOT_Null;
  m_fOpt = false;
  m_fAllowAq=  false;
  dbgpln("SpecieBlk: %s (%s : %s)", m_sCmpName(), pFile, pDll);
  }

//---------------------------------------------------------------------------

CSpecieBlk::~CSpecieBlk()
  {
  dbgpln("---SB:%s", m_sSpName());
  if (sm_pFirst==this)
    {
    sm_pFirst=m_pNxt;
    }
  else
    {
    CSpecieBlk * p=sm_pFirst;
    while (p->m_pNxt!=this)
      p=p->m_pNxt;
    p->m_pNxt=m_pNxt;
    }
  };

// ===========================================================================
//
//
//
// ==========================================================================

bool gs_WithSpecieCnvs = true;

CComponentDataBase xgs_CDB(false);
CComponentDataBase &gs_CDB=xgs_CDB;

// local externs
CComponentDataBase &CDB=gs_CDB;
CCElementDataBase &EDB=gs_CDB.m_EDB;
CSpecieDataBase &SDB=gs_CDB.m_SDB;
CH2ODataBase &H2ODB=gs_CDB.m_H2O;
int &ph_SolS=gs_CDB.m_ph_SolS;
int &ph_SolE=gs_CDB.m_ph_SolE;
int &ph_LiqS=gs_CDB.m_ph_LiqS;
int &ph_LiqE=gs_CDB.m_ph_LiqE;
int &ph_GasS=gs_CDB.m_ph_GasS;
int &ph_GasE=gs_CDB.m_ph_GasE;
int &ph_ALL=gs_CDB.m_ph_ALL;

PhMask &som_Sol=gs_CDB.m_som_Sol;
PhMask &som_Liq=gs_CDB.m_som_Liq;
PhMask &som_Gas=gs_CDB.m_som_Gas;
PhMask &som_SL=gs_CDB.m_som_SL;
PhMask &som_ALL=gs_CDB.m_som_ALL;

// ===========================================================================

CSpecieBlk * CSpecieBlk::sm_pFirst;

// ===========================================================================
//
//
//
// ==========================================================================


flag SpecieIter::FirstSpc(PhMask Mask, int &i)
  {
  int* m_pSkipList=SVI.SetUpSkipList(Mask);
  i=0+m_pSkipList[0];
  return i<SVSpcCount();
  };

void  SpecieIter::CreateFrom(bool * RqdList, bool Invert)
  {
  m_SpcCount=SDB.Count();
  m_ValCount=SVValueCount();
  if (!m_pSkipList)
    {
    m_pSkipList=new int[m_ValCount+SVClcCount()+1];
    m_pSkipList=&m_pSkipList[SVClcCount()];
    }
  for (int i=-SVClcCount(); i<m_ValCount+1; i++)
    m_pSkipList[i]=MaxSpecies;
  int j=-1;
  for (i=0; i<m_ValCount; i++)
    {
    if (RqdList[i] == !Invert)
      {
      m_pSkipList[j+1]=i-j-1;
      j=i;
      }
    }
  m_fMustDelete=true;
  };

// ===========================================================================
//
//
//
// ==========================================================================


long CSpecie::DefinedPropertyCount()
  {
  return 16 + CPropertyBase::DefinedPropertyCount();
  }

long CSpecie::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index-CPropertyBase::DefinedPropertyCount();
  switch(Inx)
    {
    case  0: Info.SetStructName("SpecieProps", 0);
             Info.Set(ePT_String,    "Tag",                      "",           DC_,         "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  1: Info.Set(ePT_String,    "Symbol",                   "",           DC_,         "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  2: Info.Set(ePT_String,    "Definition",               "",           DC_,         "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  3: Info.Set(ePT_Double,    "MolecularWt",              "MoleWt",     DC_,         "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  4: Info.Set(ePT_String,    "Phase",                    "",           DC_,         "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  5: Info.Set(ePT_Long,      "Occurence",                "",           DC_,         "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  6: Info.Set(ePT_Double,    "Ts",                       "",           DC_T,        "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  7: Info.Set(ePT_Double,    "Te",                       "",           DC_T,        "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  8: Info.Set(ePT_Double,    "Density",                  "Rho",        DC_Rho,      "",  0,      0,  MP_HardMin,    ""); return Inx;
    case  9: Info.Set(ePT_Double,    "HeatOfFormation",          "Hf",         DC_HMs,      "",  0,      0,  MP_HardMin,    ""); return Inx;
    case 10: Info.Set(ePT_Double,    "S",                        "",           DC_SMs,      "",  0,      0,  MP_HardMin,    ""); return Inx;
    case 11: Info.Set(ePT_Double,    "Cp",                       "",           DC_CpMs,     "",  0,      0,  MP_HardMin,    ""); return Inx;
    case 12: Info.Set(ePT_Double,    "VapourPressure",           "Vp",         DC_P,        "",  0,      0,  MP_HardMin,    ""); return Inx;
    case 13: Info.Set(ePT_Double,    "CriticalPressure",         "Pc",         DC_P,        "",  0,      0,  MP_HardMin,    ""); return Inx;
    case 14: Info.Set(ePT_Double,    "CriticalTemperature",      "Tc",         DC_T,        "",  0,      0,  MP_HardMin,    ""); return Inx;
    case 15: Info.Set(ePT_Double,    "CriticalVolume",           "Vc",         DC_Vol,      "",  0,      0,  MP_HardMin,    ""); return Inx;
    };
  return CPropertyBase::DefinedPropertyInfo(Index, Info);
  };

DWORD CSpecie::GetPropertyVisibility(long Index)
  {
  return CPropertyBase::GetPropertyVisibility(Index);
  }

void CSpecie::GetPropertyValue(long Index, PhMask Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  //_asm int 3;
  long m_lFidelity=0;
  switch (Index-CPropertyBase::DefinedPropertyCount())
    {
    case 0:   Value=Tag();                                return;
    case 1:   Value=Sym();                                return;
    case 2:   Value=CDB[iCId].m_sElDef();                 return;
    case 3:   Value=MoleWt();                             return;
    case 4:   Value=m_sPhaseSymVisible();                 return;
    case 5:   Value=(long)m_PhMsk;                        return;
    case 6:   Value=LoT(m_lFidelity);                     return;
    case 7:   Value=HiT(m_lFidelity);                     return;
    case 8:   Value=Density(m_lFidelity, T,P,NULL,NULL);  return;
    case 9:   Value=msHf(m_lFidelity, T,P,NULL,NULL);     return;
    case 10:  Value=msSf(m_lFidelity, T,P,NULL,NULL);     return;
    case 11:  Value=msCp(m_lFidelity, T,P,NULL,NULL);     return;
    case 12:  Value=VapourP(m_lFidelity, T);              return;
    case 13:  Value=PCrit();                              return;
    case 14:  Value=TCrit();                              return;
    case 15:  Value=VCrit();                              return;
    default:  Value=dNAN;                               return;
    }
  return CPropertyBase::GetPropertyValue(Index, Phase, T, P, Value);
  };

void CSpecie::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  //switch (Index-CPropertyBase::DefinedPropertyCount())
  //  {
  //  case   0:
  //  case   1:
  //    return;
  //  };
  CPropertyBase::PutPropertyValue(Index, Value);
  };

// ===========================================================================
//
//
//
// ==========================================================================

// NB NB Same order as enum SPM_View

static CSV_ViewCnv gs_SPMDC_         = {&DC_,            ""          };

static CSV_ViewCnv gs_SPMDC_Frac     = {&DC_Frac,        "%"         };
static CSV_ViewCnv gs_SPMDC_FracSol  = {&DC_FracSol,     "%(Sol)"    };
static CSV_ViewCnv gs_SPMDC_FracLiq  = {&DC_FracLiq,     "%(Liq)"    };
static CSV_ViewCnv gs_SPMDC_FracGas  = {&DC_FracVap,     "%(Vap)"    };

//MaxPhases Dependent
static CSV_ViewCnv gs_SPMDC_FracPh[MaxPhases] =
  {
    {&DC_FracPh00,    "%{Ph00}"   },
    {&DC_FracPh01,    "%{Ph01}"   },
    {&DC_FracPh02,    "%{Ph02}"   },
    {&DC_FracPh03,    "%{Ph03}"   },
    {&DC_FracPh04,    "%{Ph04}"   },
    {&DC_FracPh05,    "%{Ph05}"   },
    {&DC_FracPh06,    "%{Ph06}"   },
    {&DC_FracPh07,    "%{Ph07}"   },
    {&DC_FracPh08,    "%{Ph08}"   },
    {&DC_FracPh09,    "%{Ph09}"   },
    {&DC_FracPh10,    "%{Ph10}"   },
    {&DC_FracPh11,    "%{Ph11}"   },
    {&DC_FracPh12,    "%{Ph12}"   },
    {&DC_FracPh13,    "%{Ph13}"   },
    {&DC_FracPh14,    "%{Ph14}"   },
    {&DC_FracPh15,    "%{Ph15}"   },
    {&DC_FracPh16,    "%{Ph16}"   },
    {&DC_FracPh17,    "%{Ph17}"   },
    {&DC_FracPh18,    "%{Ph18}"   },
    {&DC_FracPh19,    "%{Ph19}"   },
    {&DC_FracPh20,    "%{Ph20}"   },
    {&DC_FracPh21,    "%{Ph21}"   },
    {&DC_FracPh22,    "%{Ph22}"   },
    {&DC_FracPh23,    "%{Ph23}"   },
    {&DC_FracPh24,    "%{Ph24}"   },
    {&DC_FracPh25,    "%{Ph25}"   },
    {&DC_FracPh26,    "%{Ph26}"   },
    {&DC_FracPh27,    "%{Ph27}"   },
    {&DC_FracPh28,    "%{Ph28}"   },
    {&DC_FracPh29,    "%{Ph29}"   },
    {&DC_FracPh30,    "%{Ph30}"   },
    {&DC_FracPh31,    "%{Ph31}"   },
  };

static CSV_ViewCnv gs_SPMDC_M       = {&DC_M,         "kg"        };
static CSV_ViewCnv gs_SPMDC_P       = {&DC_P,         "kpa"       };
static CSV_ViewCnv gs_SPMDC_Qm      = {&DC_Qm,        "kg/s"      };
static CSV_ViewCnv gs_SPMDC_KgMl    = {&DC_KgMl,      "kmol"      };
static CSV_ViewCnv gs_SPMDC_QKgMl   = {&DC_QKgMl,     "kmol/s"    };
static CSV_ViewCnv gs_SPMDC_Vol     = {&DC_Vol,       "m^3"       };
static CSV_ViewCnv gs_SPMDC_Qv      = {&DC_Qv,        "m^3/s"     };
static CSV_ViewCnv gs_SPMDC_NVol    = {&DC_NVol,      "Nm^3"      };
static CSV_ViewCnv gs_SPMDC_NQv     = {&DC_NQv,       "Nm^3/s"    };
static CSV_ViewCnv gs_SPMDC_Conc    = {&DC_Conc,      "g/L"       };

static CSV_ViewCnv * SPMDC_[]       = { &gs_SPMDC_,         };
//MaxPhases Dependent
static CSV_ViewCnv * SPMDC_Frac[]   = {
                                        &gs_SPMDC_Frac     ,
                                        &gs_SPMDC_FracSol  ,
                                        &gs_SPMDC_FracLiq  ,
                                        &gs_SPMDC_FracGas  ,
                                        &gs_SPMDC_FracPh[0] ,
                                        &gs_SPMDC_FracPh[1] ,
                                        &gs_SPMDC_FracPh[2] ,
                                        &gs_SPMDC_FracPh[3] ,
                                        &gs_SPMDC_FracPh[4] ,
                                        &gs_SPMDC_FracPh[5] ,
                                        &gs_SPMDC_FracPh[6] ,
                                        &gs_SPMDC_FracPh[7] ,
                                        &gs_SPMDC_FracPh[8] ,
                                        &gs_SPMDC_FracPh[9] ,
                                        &gs_SPMDC_FracPh[10] ,
                                        &gs_SPMDC_FracPh[11] ,
                                        &gs_SPMDC_FracPh[12] ,
                                        &gs_SPMDC_FracPh[13] ,
                                        &gs_SPMDC_FracPh[14] ,
                                        &gs_SPMDC_FracPh[15] ,
                                        &gs_SPMDC_FracPh[16] ,
                                        &gs_SPMDC_FracPh[17] ,
                                        &gs_SPMDC_FracPh[18] ,
                                        &gs_SPMDC_FracPh[19] ,
                                        &gs_SPMDC_FracPh[20] ,
                                        &gs_SPMDC_FracPh[21] ,
                                        &gs_SPMDC_FracPh[22] ,
                                        &gs_SPMDC_FracPh[23] ,
                                        &gs_SPMDC_FracPh[24] ,
                                        &gs_SPMDC_FracPh[25] ,
                                        &gs_SPMDC_FracPh[26] ,
                                        &gs_SPMDC_FracPh[27] ,
                                        &gs_SPMDC_FracPh[28] ,
                                        &gs_SPMDC_FracPh[29] ,
                                        &gs_SPMDC_FracPh[30] ,
                                        &gs_SPMDC_FracPh[31] ,
                                        };
static CSV_ViewCnv * SPMDC_M[]      = { &gs_SPMDC_M,        };
static CSV_ViewCnv * SPMDC_P[]      = { &gs_SPMDC_P,        };
static CSV_ViewCnv * SPMDC_Qm[]     = { &gs_SPMDC_Qm,       };
static CSV_ViewCnv * SPMDC_KgMl[]   = { &gs_SPMDC_KgMl,     };
static CSV_ViewCnv * SPMDC_QKgMl[]  = { &gs_SPMDC_QKgMl,    };
static CSV_ViewCnv * SPMDC_Vol[]    = { &gs_SPMDC_Vol,      };
static CSV_ViewCnv * SPMDC_Qv[]     = { &gs_SPMDC_Qv,       };
static CSV_ViewCnv * SPMDC_NVol[]   = { &gs_SPMDC_NVol,     };
static CSV_ViewCnv * SPMDC_NQv[]    = { &gs_SPMDC_NQv,      };
static CSV_ViewCnv * SPMDC_Conc[]   = { &gs_SPMDC_Conc,     };

CSV_ViewBasis SVV_NameF[] =
                                      {
                                        {SVVB_All, ""},
                                        {SVVB_End, NULL}
                                      };
CSV_ViewBasis SVV_NameS[] =
                                      {
                                        {SVVB_All,   ""},
                                        {SVVB_Occ,   ":Ph"},
                                        {SVVB_Phase, ":IPh"},
                                        {SVVB_End,   NULL}
                                      };
CSV_ViewBasis SVV_NameE[] =
                                      {
                                        {SVVB_All, ""},
                                        {SVVB_Sol, ":S"},
                                        {SVVB_Liq, ":L"},
                                        {SVVB_Gas, ":V"},
                                        {SVVB_End, NULL}
                                      };
CSV_ViewBasis SVV_NameC[] =
                                      {
                                        {SVVB_All, ""},
                                        {SVVB_Sol, ":S"},
                                        {SVVB_Liq, ":L"},
                                        {SVVB_Gas, ":V"},
                                        {SVVB_End, NULL}
                                      };

const CSV_ViewInfo gs_ViewInfo[] =
  {
    {"",              "",       0,                  SPMDC_,      false, NULL     , 0                },
    {"Ind. Frac",     "Fr",     xidSpRawFrac,       SPMDC_Frac,   true, SVV_NameF, VAMsk_SpRaw      },
    {"Ind. Mass",     "IMs",    xidSpRawMass,       SPMDC_M,     true,  SVV_NameF, VAMsk_SpRaw      },
    {"Ind. MassFlow", "IQm",    xidSpRawMassFlow,   SPMDC_Qm,     true, SVV_NameF, VAMsk_SpRaw      },
    {"Mass",          "M",      xidSpMass,          SPMDC_M,     false, SVV_NameS, VAMsk_SpMass     },
    {"MassFlow",      "QM",     xidSpMass,          SPMDC_Qm,    false, SVV_NameS, VAMsk_SpMass     },
    {"MassFrac",      "MF",     xidSpMassFrac,      SPMDC_Frac,   true, SVV_NameS, VAMsk_SpMass     },
    {"Moles",         "Ml",     xidSpMole,          SPMDC_KgMl,  false, SVV_NameS, VAMsk_SpMole     },
    {"MoleFlow",      "QMl",    xidSpMole,          SPMDC_QKgMl, false, SVV_NameS, VAMsk_SpMole     },
    {"MoleFrac",      "MlF",    xidSpMoleFrac,      SPMDC_Frac,   true, SVV_NameS, VAMsk_SpMole     },
    {"Vol",           "Vl",     xidSpVol,           SPMDC_Vol,   false, SVV_NameS, VAMsk_SpVol      },
    {"VolFlow",       "QVl",    xidSpVol,           SPMDC_Qv,    false, SVV_NameS, VAMsk_SpVol      },
    {"VolFrac",       "VlF",    xidSpVolFrac,       SPMDC_Frac,   true, SVV_NameS, VAMsk_SpVol      },
    {"PartialP",      "PP",     xidSpPartialP,      SPMDC_P,     false, SVV_NameS, VAMsk_SpPartialP },
    {"PartialPFrac",  "PPF",    xidSpPartialPFrac,  SPMDC_Frac,   true, SVV_NameS, VAMsk_SpPartialP },
    {"NormVol",       "NVl",    xidSpNVol,          SPMDC_NVol,  false, SVV_NameS, VAMsk_SpNVol     },
    {"NormVolFlow",   "NQVl",   xidSpNVol,          SPMDC_NQv,   false, SVV_NameS, VAMsk_SpNVol     },
    {"NormVolFrac",   "NVlF",   xidSpNVolFrac,      SPMDC_Frac,   true, SVV_NameS, VAMsk_SpNVol     },
    {"SpecieConc",    "Cnc",    xidSpConc,          SPMDC_Conc,  false, SVV_NameS, VAMsk_SpConc     },
    {"Elemnts",       "El",     xidSpElem,          SPMDC_M,     false, SVV_NameE, VAMsk_SpElem     },
    {"ElemntFlow",    "QEl",    xidSpElem,          SPMDC_Qm,    false, SVV_NameE, VAMsk_SpElem     },
    {"ElemntFrac",    "ElF",    xidSpElemFrac,      SPMDC_Frac,   true, SVV_NameE, VAMsk_SpElem     },
    {"ElemntConc",    "ElC",    xidSpElemConc,      SPMDC_Conc,  false, SVV_NameE, VAMsk_SpElem     },
    {"ElemntConc25",  "ElC25",  xidSpElemConc25,    SPMDC_Conc,  false, SVV_NameE, VAMsk_SpElem     },
    {"Compnts",       "CM",     xidSpComp,          SPMDC_M,     false, SVV_NameC, VAMsk_SpComp     },
    {"CompntFlow",    "CQM",    xidSpComp,          SPMDC_Qm,    false, SVV_NameC, VAMsk_SpComp     },
    {"CompntFrac",    "CMF",    xidSpCompFrac,      SPMDC_Frac,   true, SVV_NameC, VAMsk_SpComp     },
    {"CompntConc",    "CMC",    xidSpCompConc,      SPMDC_Conc,  false, SVV_NameC, VAMsk_SpComp     },
    {"CompntConc25",  "CMC25",  xidSpCompConc25,    SPMDC_Conc,  false, SVV_NameC, VAMsk_SpComp     }
  };

const LPCTSTR SV_ViewStr[] = {"NULL",
        "AsRawFrac",
        "AsRawMass",   "AsRawMassFlow",
        "AsMass",      "AsMassFlow",      "AsMassFrac",
        "AsMole",      "AsMoleFlow",      "AsMoleFrac",
        "AsVol",       "AsVolFlow",       "AsVolFrac",
        "AsPartialP",  "AsPartailPFrac",
        "AsNVol",      "AsNVolFlow",      "AsNVolFrac",
        "AsSpcConc",
        "AsElem",      "AsElemFlow",      "AsElemFrac",    "AsElemConc",    "AsElemConc25",
        "AsComp",      "AsCompFlow",      "AsCompFrac",    "AsCompConc",    "AsCompConc25"
        };
const LPCTSTR SV_ViewBasisStr[] = { "All", "Sol", "Liq", "Gas", "Occ", "Phase", "End" };
const LPCTSTR SV_CalcTypesStr[] = {"Null", "General", "Sum", "Total", "PhSum", "PhTotal", "IPhSum", "IPhTotal" };

byte SVOperatorsXID(XID xid)
  {
  switch (xid)
    {
    case xidSpMass:
    case xidSpMassFlow:
    case xidSpMassFrac: return SVOP_Mass;
    case xidSpMole:
    case xidSpMoleFlow:
    case xidSpMoleFrac: return SVOP_Mole;
    case xidSpVol:
    case xidSpVolFlow:
    case xidSpVolFrac:  return SVOP_Vol;
    case xidSpNVol:
    case xidSpNVolFlow:
    case xidSpNVolFrac: return SVOP_NVol;
    case xidSpConc:     return SVOP_Mass;
    };
  return SVOP_;
  };

CSV_ViewCnv * CSysVecItem::ViewBasisCnv(eScdSVView Vw, eScdSVViewBasis Ph)
  {
  if (gs_ViewInfo[Vw].m_IsFrac)
    return gs_ViewInfo[Vw].Cnvs[m_ViewBasisRules[Ph].m_iSpVwMaskIndex];
  return gs_ViewInfo[Vw].Cnvs[0];
  };

PhMask CSysVecItem::ViewBasisMask(eScdSVViewBasis Ph)
  {
  return CSysVecInfo::SpVwPhMask(m_ViewBasisRules[Ph].m_iSpVwMaskIndex);
  };
LPCSTR CSysVecItem::ViewBasisName(eScdSVViewBasis Ph)
  {
  return CSysVecInfo::SpVwPhMaskName(m_ViewBasisRules[Ph].m_iSpVwMaskIndex);
  };
long CSysVecItem::ViewBasisEditable(eScdSVView Vw, eScdSVViewBasis Ph)
  {
  if (gs_ViewInfo[Vw].m_IsFrac)
    return m_ViewBasisRules[Ph].m_bEditable;
  return true;
  };
long CSysVecItem::ViewBasisUsed(eScdSVViewBasis Ph)
  {
  return m_ViewBasisRules[Ph].m_bUsed;
  };

// ==========================================================================
//
//                             Specie Array Info
//
// ==========================================================================

LPCSTR CSysVecItemTypeNm[]= { "Null", "Specie", "Calculation", "Attribute", "Annotation"};

CSysVecInfo SVI; // the global instance

inline LPCTSTR SafeStr(LPCTSTR p) { return p &&strlen(p) ? p : ""; };
inline bool ValidSpecieTag(Strng &Tag, bool ZeroLenOK)
  {
  for (int i=0; i<Tag.GetLength(); i++)
    if (Tag[i]==' ')
      Tag[i]='_';
  return (ZeroLenOK || Tag.GetLength()>0);
  }


const int InfoColNo=9;

//--------------------------------------------------------------------------
//MaxPhases Dependent
enum eSymbolType
  {
  SFS_Null,
  SFS_SumSLG,
  SFS_SumS,
  SFS_SumL,
  SFS_SumG,
  //SFS_SumSL,
  SFS_SumPhSLG,
  SFS_SumPhS,
  SFS_SumPhL,
  SFS_SumPhG,
  SFS_SumIPhSLG,
  SFS_SumIPh00,
  SFS_SumIPh01,
  SFS_SumIPh02,
  SFS_SumIPh03,
  SFS_SumIPh04,
  SFS_SumIPh05,
  SFS_SumIPh06,
  SFS_SumIPh07,
  SFS_SumIPh08,
  SFS_SumIPh09,
  SFS_SumIPh10,
  SFS_SumIPh11,
  SFS_SumIPh12,
  SFS_SumIPh13,
  SFS_SumIPh14,
  SFS_SumIPh15,
  SFS_SumIPh16,
  SFS_SumIPh17,
  SFS_SumIPh18,
  SFS_SumIPh19,
  SFS_SumIPh20,
  SFS_SumIPh21,
  SFS_SumIPh22,
  SFS_SumIPh23,
  SFS_SumIPh24,
  SFS_SumIPh25,
  SFS_SumIPh26,
  SFS_SumIPh27,
  SFS_SumIPh28,
  SFS_SumIPh29,
  SFS_SumIPh30,
  SFS_SumIPh31,
  SFS_MW,
  SFS_LastFunction,

  SFS_FirstOperator,
  SFS_Plus=SFS_FirstOperator,
  SFS_Minus,
  SFS_Mult,
  SFS_Div,
  SFS_Open,
  SFS_Close,
  SFS_And,
  SFS_Or,
  SFS_LastOperator,

  SFS_Const,
  SFS_CmpLT,
  SFS_CmpLE,
  SFS_CmpEQ,
  SFS_CmpNE,
  SFS_CmpGE,
  SFS_CmpGT,
  SFS_Not,
  SFS_LastVisTkn,
  SFS_SVIndex,
  SFS_CfgIndex,
  SFS_LastTkn,
  };

enum eInfixType
  {
  IFC_Null,
  IFC_Const,
  IFC_Var,
  IFC_OBrace,
  IFC_CBrace,
  IFC_Operator,
  IFC_Funct,
  IFC_Cmp,
  IFC_Not
  };


//MaxPhases Dependent
const LPSTR PhSumNames[] =
  {"SumIPh00", "SumIPh01", "SumIPh02", "SumIPh03", "SumIPh04", "SumIPh05", "SumIPh06", "SumIPh07",
   "SumIPh08", "SumIPh09", "SumIPh10", "SumIPh11", "SumIPh12", "SumIPh13", "SumIPh14", "SumIPh15",
   "SumIPh16", "SumIPh17", "SumIPh18", "SumIPh19", "SumIPh20", "SumIPh21", "SumIPh22", "SumIPh23",
   "SumIPh24", "SumIPh25", "SumIPh26", "SumIPh27", "SumIPh28", "SumIPh29", "SumIPh30", "SumIPh31"
  };
//MaxPhases Dependent
const int   PhSumSyms[]  =
  {SFS_SumIPh00, SFS_SumIPh01, SFS_SumIPh02, SFS_SumIPh03, SFS_SumIPh04, SFS_SumIPh05, SFS_SumIPh06, SFS_SumIPh07,
   SFS_SumIPh08, SFS_SumIPh09, SFS_SumIPh10, SFS_SumIPh11, SFS_SumIPh12, SFS_SumIPh13, SFS_SumIPh14, SFS_SumIPh15,
   SFS_SumIPh16, SFS_SumIPh17, SFS_SumIPh18, SFS_SumIPh19, SFS_SumIPh20, SFS_SumIPh21, SFS_SumIPh22, SFS_SumIPh23,
   SFS_SumIPh24, SFS_SumIPh25, SFS_SumIPh26, SFS_SumIPh27, SFS_SumIPh28, SFS_SumIPh29, SFS_SumIPh30, SFS_SumIPh31,
  };

static struct CFnTkns
  {
  eSymbolType   m_iSym;
  LPCTSTR       m_sTkn;
  eInfixType    m_eInfix;
  int           m_Prec;
  bool          m_bShwIndex;
  bool          m_bShwValue;
  }
FnTkns[SFS_LastTkn+1] =
  {
    { SFS_Null,           "Null",           IFC_Null,       0,             },
    { SFS_SumSLG,         "SumSLG",         IFC_Funct,      0,             },
    { SFS_SumS,           "SumS",           IFC_Funct,      0,             },
    { SFS_SumL,           "SumL",           IFC_Funct,      0,             },
    { SFS_SumG,           "SumG",           IFC_Funct,      0,             },
    //{ SFS_SumSL,          "SumSL",          IFC_Funct,      0,             },
    { SFS_SumPhSLG,       "SumPhSLG",       IFC_Funct,      0,             },
    { SFS_SumPhS,         "SumPhS",         IFC_Funct,      0,             },
    { SFS_SumPhL,         "SumPhL",         IFC_Funct,      0,             },
    { SFS_SumPhG,         "SumPhG",         IFC_Funct,      0,             },
    { SFS_SumIPhSLG,      "SumIPhSLG",      IFC_Funct,      0,             },
    { SFS_SumIPh00,       "SumIPh00",       IFC_Funct,      0,             },
    { SFS_SumIPh01,       "SumIPh01",       IFC_Funct,      0,             },
    { SFS_SumIPh02,       "SumIPh02",       IFC_Funct,      0,             },
    { SFS_SumIPh03,       "SumIPh03",       IFC_Funct,      0,             },
    { SFS_SumIPh04,       "SumIPh04",       IFC_Funct,      0,             },
    { SFS_SumIPh05,       "SumIPh05",       IFC_Funct,      0,             },
    { SFS_SumIPh06,       "SumIPh06",       IFC_Funct,      0,             },
    { SFS_SumIPh07,       "SumIPh07",       IFC_Funct,      0,             },
    { SFS_SumIPh08,       "SumIPh08",       IFC_Funct,      0,             },
    { SFS_SumIPh09,       "SumIPh09",       IFC_Funct,      0,             },
    { SFS_SumIPh10,       "SumIPh10",       IFC_Funct,      0,             },
    { SFS_SumIPh11,       "SumIPh11",       IFC_Funct,      0,             },
    { SFS_SumIPh12,       "SumIPh12",       IFC_Funct,      0,             },
    { SFS_SumIPh13,       "SumIPh13",       IFC_Funct,      0,             },
    { SFS_SumIPh14,       "SumIPh14",       IFC_Funct,      0,             },
    { SFS_SumIPh15,       "SumIPh15",       IFC_Funct,      0,             },
    { SFS_SumIPh16,       "SumIPh16",       IFC_Funct,      0,             },
    { SFS_SumIPh17,       "SumIPh17",       IFC_Funct,      0,             },
    { SFS_SumIPh18,       "SumIPh18",       IFC_Funct,      0,             },
    { SFS_SumIPh19,       "SumIPh19",       IFC_Funct,      0,             },
    { SFS_SumIPh20,       "SumIPh20",       IFC_Funct,      0,             },
    { SFS_SumIPh21,       "SumIPh21",       IFC_Funct,      0,             },
    { SFS_SumIPh22,       "SumIPh22",       IFC_Funct,      0,             },
    { SFS_SumIPh23,       "SumIPh23",       IFC_Funct,      0,             },
    { SFS_SumIPh24,       "SumIPh24",       IFC_Funct,      0,             },
    { SFS_SumIPh25,       "SumIPh25",       IFC_Funct,      0,             },
    { SFS_SumIPh26,       "SumIPh26",       IFC_Funct,      0,             },
    { SFS_SumIPh27,       "SumIPh27",       IFC_Funct,      0,             },
    { SFS_SumIPh28,       "SumIPh28",       IFC_Funct,      0,             },
    { SFS_SumIPh29,       "SumIPh29",       IFC_Funct,      0,             },
    { SFS_SumIPh30,       "SumIPh30",       IFC_Funct,      0,             },
    { SFS_SumIPh31,       "SumIPh31",       IFC_Funct,      0,             },
    //MaxPhases Dependent
    { SFS_MW,             "MW",             IFC_Funct,      0,             },  // does not work yet
    { SFS_LastFunction,   NULL,             IFC_Null,       0,             },
    { SFS_Plus,           "+",              IFC_Operator,  26,             },
    { SFS_Minus,          "-",              IFC_Operator,  26,             },
    { SFS_Mult,           "*",              IFC_Operator,  28,             },
    { SFS_Div,            "/",              IFC_Operator,  28,             },
    { SFS_Open,           "(",              IFC_OBrace,     0,             },
    { SFS_Close,          ")",              IFC_CBrace,     0,             },
    { SFS_And,            "And",            IFC_Operator,  18,             },
    { SFS_Or,             "Or",             IFC_Operator,  18,             },
    { SFS_LastOperator,   NULL,             IFC_Null,       0,             },
    { SFS_Const,          "Const",          IFC_Const,      0,             },
    { SFS_CmpLT,          "<",              IFC_Cmp,       22,             },
    { SFS_CmpLE,          "<=",             IFC_Cmp,       22,             },
    { SFS_CmpEQ,          "==",             IFC_Cmp,       20,             },
    { SFS_CmpNE,          "<>",             IFC_Cmp,       20,             },
    { SFS_CmpGE,          ">=",             IFC_Cmp,       22,             },
    { SFS_CmpGT,          ">",              IFC_Cmp,       22,             },
    { SFS_Not,            "!",              IFC_Not,       32,             },
    { SFS_LastVisTkn,     NULL,             IFC_Null,       0,             },
    { SFS_SVIndex,        "SVValue",        IFC_Var,        0,       true  },
    { SFS_CfgIndex,       "CfgValue",       IFC_Var,        0,       true  },
    { SFS_LastTkn,        NULL,             IFC_Null,       0,             },
  };

static struct CFnSums
  {
  eSymbolType   m_iSym;
  LPCTSTR       m_sTag;
  LPCTSTR       m_sSym;
  SV_CalcTypes m_iCalcType;
  }
FnSums[] =
  {
    { SFS_SumSLG,         "Total",    "Ttl",      SVCT_Total,         },
    { SFS_SumS,           "Solids",   "Solids",   SVCT_Sum,           },
    { SFS_SumL,           "Liquids",  "Liquids",  SVCT_Sum,           },
    { SFS_SumG,           "Gasses",   "Gasses",   SVCT_Sum,           },
    //{ SFS_SumSL,          "Slurry",   "Slr",      SVCT_Sum,           },
    { SFS_SumPhSLG,       "Total",    "Ttl",      SVCT_PhTotal,       },
    { SFS_SumPhS,         "Solids",   "Sol",      SVCT_PhSum,         },
    { SFS_SumPhL,         "Liquids",  "Liq",      SVCT_PhSum,         },
    { SFS_SumPhG,         "Gasses",   "Gas",      SVCT_PhSum,         },
    { SFS_SumIPhSLG,      "Total",    "Ttl",      SVCT_IPhTotal,      },
    { SFS_SumIPh00,       "Ph00",     "Ph00",     SVCT_IPhSum,        },
    { SFS_SumIPh01,       "Ph01",     "Ph01",     SVCT_IPhSum,        },
    { SFS_SumIPh02,       "Ph02",     "Ph02",     SVCT_IPhSum,        },
    { SFS_SumIPh03,       "Ph03",     "Ph03",     SVCT_IPhSum,        },
    { SFS_SumIPh04,       "Ph04",     "Ph04",     SVCT_IPhSum,        },
    { SFS_SumIPh05,       "Ph05",     "Ph05",     SVCT_IPhSum,        },
    { SFS_SumIPh06,       "Ph06",     "Ph06",     SVCT_IPhSum,        },
    { SFS_SumIPh07,       "Ph07",     "Ph07",     SVCT_IPhSum,        },
    { SFS_SumIPh08,       "Ph08",     "Ph08",     SVCT_IPhSum,        },
    { SFS_SumIPh09,       "Ph09",     "Ph09",     SVCT_IPhSum,        },
    { SFS_SumIPh10,       "Ph10",     "Ph10",     SVCT_IPhSum,        },
    { SFS_SumIPh11,       "Ph11",     "Ph11",     SVCT_IPhSum,        },
    { SFS_SumIPh12,       "Ph12",     "Ph12",     SVCT_IPhSum,        },
    { SFS_SumIPh13,       "Ph13",     "Ph13",     SVCT_IPhSum,        },
    { SFS_SumIPh14,       "Ph14",     "Ph14",     SVCT_IPhSum,        },
    { SFS_SumIPh15,       "Ph15",     "Ph15",     SVCT_IPhSum,        },
    { SFS_SumIPh16,       "Ph16",     "Ph16",     SVCT_IPhSum,        },
    { SFS_SumIPh17,       "Ph17",     "Ph17",     SVCT_IPhSum,        },
    { SFS_SumIPh18,       "Ph18",     "Ph18",     SVCT_IPhSum,        },
    { SFS_SumIPh19,       "Ph19",     "Ph19",     SVCT_IPhSum,        },
    { SFS_SumIPh20,       "Ph20",     "Ph20",     SVCT_IPhSum,        },
    { SFS_SumIPh21,       "Ph21",     "Ph21",     SVCT_IPhSum,        },
    { SFS_SumIPh22,       "Ph22",     "Ph22",     SVCT_IPhSum,        },
    { SFS_SumIPh23,       "Ph23",     "Ph23",     SVCT_IPhSum,        },
    { SFS_SumIPh24,       "Ph24",     "Ph24",     SVCT_IPhSum,        },
    { SFS_SumIPh25,       "Ph25",     "Ph25",     SVCT_IPhSum,        },
    { SFS_SumIPh26,       "Ph26",     "Ph26",     SVCT_IPhSum,        },
    { SFS_SumIPh27,       "Ph27",     "Ph27",     SVCT_IPhSum,        },
    { SFS_SumIPh28,       "Ph28",     "Ph28",     SVCT_IPhSum,        },
    { SFS_SumIPh29,       "Ph29",     "Ph29",     SVCT_IPhSum,        },
    { SFS_SumIPh30,       "Ph30",     "Ph30",     SVCT_IPhSum,        },
    { SFS_SumIPh31,       "Ph31",     "Ph31",     SVCT_IPhSum,        },
    //MaxPhases Dependent
  };

//--------------------------------------------------------------------------

class CSysVecFnCtx
  {
  public:
    byte              m_Op;
    CSysVector      * m_pData1;
    CSysVector      * m_pData2;
    double            m_dScale;
    int               m_iSgn;
    double            m_dDeltaTime;
    CSysVector      * m_pFlw;
    CSysVector      * m_pM;
    double            m_dAdvance;

    long              m_iFidelity;
    double            m_Temperature;
    double            m_Pressure;

    CSysVecFnCtx(CSysVector &Data)
      {
      #if (CTX_FORCEFULLINIT)
      m_Op          = -1;
      m_pData1      = NULL;
      m_pData2      = NULL;
      m_dScale      = dNAN;;
      m_iSgn        = 0;
      m_dDeltaTime  = dNAN;
      m_pFlw        = NULL;
      m_pM          = NULL;
      m_dAdvance    = dNAN;
      m_iFidelity   = -1;
      m_Temperature = dNAN;
      m_Pressure    = dNAN;
      #endif

      m_iFidelity   = -1;
      m_Op          = SVOP_Mass;
      m_pData1      = &Data;
      };

    CSysVecFnCtx(CSysVector &Data, byte Op, long Fidelity, double Temperature, double Pressure)
      {
      #if (CTX_FORCEFULLINIT)
      m_Op          = -1;
      m_pData1      = NULL;
      m_pData2      = NULL;
      m_dScale      = dNAN;;
      m_iSgn        = 0;
      m_dDeltaTime  = dNAN;
      m_pFlw        = NULL;
      m_pM          = NULL;
      m_dAdvance    = dNAN;
      m_iFidelity   = -1;
      m_Temperature = dNAN;
      m_Pressure    = dNAN;
      #endif

      m_Op          = Op;
      m_pData1      = &Data;
      m_iFidelity   = Fidelity;
      m_Temperature = Temperature;
      m_Pressure    = Pressure;
      };

    CSysVecFnCtx(CSysVector &Data1, CSysVector &Data2, double Scale)
      {
      #if (CTX_FORCEFULLINIT)
      m_Op          = -1;
      m_pData1      = NULL;
      m_pData2      = NULL;
      m_dScale      = dNAN;;
      m_iSgn        = 0;
      m_dDeltaTime  = dNAN;
      m_pFlw        = NULL;
      m_pM          = NULL;
      m_dAdvance    = dNAN;
      m_iFidelity   = -1;
      m_Temperature = dNAN;
      m_Pressure    = dNAN;
      #endif

      m_iFidelity   = -1;
      m_Op          = SVOP_Mass;
      m_pData1      = &Data1;
      m_pData2      = &Data2;
      m_dScale      = Scale;
      };
    CSysVecFnCtx(int Sgn, double dTime, CSysVector &Flw, CSysVector &M, double Advance)
      {
      #if (CTX_FORCEFULLINIT)
      m_Op          = -1;
      m_pData1      = NULL;
      m_pData2      = NULL;
      m_dScale      = dNAN;;
      m_iSgn        = 0;
      m_dDeltaTime  = dNAN;
      m_pFlw        = NULL;
      m_pM          = NULL;
      m_dAdvance    = dNAN;
      m_iFidelity   = -1;
      m_Temperature = dNAN;
      m_Pressure    = dNAN;
      #endif

      m_iFidelity   = -1;
      m_Op          = SVOP_Mass;
      m_iSgn        = Sgn;
      m_dDeltaTime  = dTime;
      m_pFlw        = &Flw;
      m_pM          = &M;
      m_dAdvance    = Advance;
      };

  };

//--------------------------------------------------------------------------

class CSysVecFnSymbol
  {
  public:
    CSysVecFnSymbol(long iSym=0, long Value=0) { m_iSym = iSym; m_iValue=Value; };
    CSysVecFnSymbol(long iSym, double Value)   { m_iSym = iSym; m_dValue=Value; };

    long     m_iSym;
    union
      {
      double m_dValue;
      long   m_iValue;
      };
  };

class CSysVecFnSymArray : public CArray<CSysVecFnSymbol, CSysVecFnSymbol&> {};

class CSysVecFn
  {
  friend class CSysVecInfo;
  friend class CFlwSolver;
  friend class CSysVecItem;
  public:
    CSysVecFn()                { m_iSVIndex=-1; m_bIsSpcsSum = false; };
    virtual ~CSysVecFn()       { };
    bool              SimpleParse(LPCSTR Str);
    void              AddTkn(CSysVecFnSymbol & Tkn);

    bool              ParseToken(Strng &Tkn, long &TknType, LPCSTR &pStr);
    bool              ParseExpression(LPCSTR Str);
    bool              CheckForPureSum();
    void              DumpExpression();
    bool              CheckForPureSum(CIArray & SumIndices, CIArray & Done);

    double            Evaluate(CSysVecFnCtx &C);

  protected:
    Strng             m_sFormula;
    long              m_iSVIndex;

    CSysVecFnSymArray m_Tkns;

    bool              m_bIsSpcsSum;
    CIArray           m_IndicesOfSumSpcs;

  };

//--------------------------------------------------------------------------

double CSysVecFn::Evaluate(CSysVecFnCtx & C)
  {
#if dbgSysVec
  static long EvalLevel=-1;
  bool DoDbg=dbgExpressionEval() && s_doExpressionEval || dbgExpressionEvalAll();
  if (DoDbg)
    {
    EvalLevel++;
    if (EvalLevel==0)
      dbgp("%*s%s:",EvalLevel*2,"", "EvFn");
    else
      {
      dbgpln("");
      dbgp("%*s%s:",4+EvalLevel*2,"", "");
      }
    }
#endif

  double Stk[SysVecFnStkSize];
  long   iStk=-1;
  if (m_Tkns.GetSize())
    {
    for (int t=0; t<m_Tkns.GetSize(); t++)
      {
      CSysVecFnSymbol &T=m_Tkns[t];
#if dbgSysVec
      if (DoDbg)
        {
        switch (T.m_iSym)
          {
          case SFS_SVIndex:
            dbgp(" [%s]", SVI.Img(T.m_iValue)->FullTag());
            break;
          case SFS_CfgIndex:
            dbgp(" [%s]", SVI.Cfg(T.m_iValue)->FullTag());
            break;
          case SFS_Const:
            dbgp(" %.3f", T.m_dValue);
            break;
          default:
            {
            if (FnTkns[T.m_iSym].m_bShwIndex)
              dbgp(" %s[%i]", FnTkns[T.m_iSym].m_sTkn, T.m_iValue);
            else if (FnTkns[T.m_iSym].m_bShwValue)
              dbgp(" %s %.3f", FnTkns[T.m_iSym].m_sTkn, T.m_dValue);
            else if (T.m_iSym==SFS_Const)
              dbgp(" %.3f", T.m_dValue);
            else
              dbgp(" %s", FnTkns[T.m_iSym].m_sTkn);
            }
          }
        }
#endif
      switch (T.m_iSym)
        {
        case SFS_SumSLG:
        case SFS_SumS:
        case SFS_SumL:
        case SFS_SumG:
        //case SFS_SumSL:
        case SFS_SumPhSLG:
        case SFS_SumPhS:
        case SFS_SumPhL:
        case SFS_SumPhG:
        case SFS_SumIPhSLG:
        case SFS_SumIPh00:
        case SFS_SumIPh01:
        case SFS_SumIPh02:
        case SFS_SumIPh03:
        case SFS_SumIPh04:
        case SFS_SumIPh05:
        case SFS_SumIPh06:
        case SFS_SumIPh07:
        case SFS_SumIPh08:
        case SFS_SumIPh09:
        case SFS_SumIPh10:
        case SFS_SumIPh11:
        case SFS_SumIPh12:
        case SFS_SumIPh13:
        case SFS_SumIPh14:
        case SFS_SumIPh15:
        case SFS_SumIPh16:
        case SFS_SumIPh17:
        case SFS_SumIPh18:
        case SFS_SumIPh19:
        case SFS_SumIPh20:
        case SFS_SumIPh21:
        case SFS_SumIPh22:
        case SFS_SumIPh23:
        case SFS_SumIPh24:
        case SFS_SumIPh25:
        case SFS_SumIPh26:
        case SFS_SumIPh27:
        case SFS_SumIPh28:
        case SFS_SumIPh29:
        case SFS_SumIPh30:
        case SFS_SumIPh31:
        //MaxPhases Dependent
          switch (C.m_Op)
            {
            case SVOP_:       Stk[++iStk]=-1;  break;
            case SVOP_Mass:   Stk[++iStk]=C.m_pData1->Mass(Fn2PhMasks[T.m_iSym-SFS_SumSLG]);  break;
            case SVOP_Mole:   Stk[++iStk]=C.m_pData1->Moles(Fn2PhMasks[T.m_iSym-SFS_SumSLG]); break;
            case SVOP_Vol:    ASSERT(C.m_iFidelity>=0);
                              Stk[++iStk]=C.m_pData1->Volume(C.m_iFidelity, Fn2PhMasks[T.m_iSym-SFS_SumSLG], C.m_Temperature, C.m_Pressure, NULL); break;
            case SVOP_NVol:   ASSERT(C.m_iFidelity>=0);
                              Stk[++iStk]=C.m_pData1->NVolume(C.m_iFidelity, Fn2PhMasks[T.m_iSym-SFS_SumSLG], NULL); break;
            }
          break;
        case SFS_MW:          Stk[iStk]  = Stk[iStk];                                               break;
        case SFS_Const:       Stk[++iStk]=T.m_dValue;                                               break;
        case SFS_Plus:        Stk[iStk-1]=Stk[iStk-1]+Stk[iStk]; iStk--;                            break;
        case SFS_Minus:       Stk[iStk-1]=Stk[iStk-1]-Stk[iStk]; iStk--;                            break;
        case SFS_Mult:        Stk[iStk-1]=Stk[iStk-1]*Stk[iStk]; iStk--;                            break;
        case SFS_Div:         Stk[iStk-1]=Stk[iStk-1]/NZ(Stk[iStk]); iStk--;                        break;
        case SFS_SVIndex:     Stk[++iStk]=C.m_pData1->ValueOp(T.m_iValue, C.m_Op, C.m_iFidelity, C.m_Temperature, C.m_Pressure); break;

        default:              INCOMPLETECODE(__FILE__, __LINE__); break;
        }
      if (iStk>=SysVecFnStkSize)
        {
        LogError("Specie Database",0,"Stack overflow");
        return 0;
        }
      }
    }
  else
    Stk[++iStk]=0;

  double Value=Stk[iStk--];
  ASSERT(iStk==-1);
#if dbgSysVec
  if (DoDbg)
    {
    //dbgp("=%10.4f)", Value);
    dbgpln(" = %10.4f", Value);
    EvalLevel--;
    if (EvalLevel>=0)
      dbgp("%*s%s:",4+EvalLevel*2,"", "");
    }
#endif
  return Value;
  };

//--------------------------------------------------------------------------

void CSysVecFn::AddTkn(CSysVecFnSymbol & Tkn)
  {
  m_Tkns.Add(Tkn);
  };

bool CSysVecFn::SimpleParse(LPCSTR Str)
  {
  if (Str)
    m_sFormula=Str;
  else
    Str=(LPCSTR)m_sFormula();

  m_Tkns.SetSize(0,8);
  // Simple
  if (Str)
    {
    LPCSTR p=Str;
    int Len=strlen(p);
    while (*p)
      {
      Strng Tkn;
      if (*p=='[') // parse speciename
        {
        int BraceLvl=0;
        p++;
        do
          {
          if (*p=='[')
            BraceLvl++;
          else if (*p==']')
            BraceLvl--;
          if (BraceLvl>=0)
            {
            Tkn+=(*p>' '?*p:'_');
            }
          p++;
          }
        while (*p && BraceLvl>=0);
        // this is a specie/attname
        for (long i=0; i<SVCfgCount(); i++)
          {
          if (Tkn.XStrICmp(SVI.Cfg(i)->FullTag())==0)
            {
            m_Tkns.Add(CSysVecFnSymbol(SFS_SVIndex, SVI.Cfg(i)->SVIndex()));
            break;
            }
          }
        if (i>=SVCfgCount())
          LogError("SpecieConfig", LF_Exclamation, "Invalid Data Name %s", Tkn());
        }
      else
        {
        if (isalnum(*p)) // parse function name
          {
          do
          Tkn+=*p++;
          while (*p && (isalnum(*p)));
          }
        else
          Tkn=*p++;
        // Tkn is a Symbol
        long i;
        for (i=0; FnTkns[i].m_iSym < SFS_LastVisTkn; i++)
          {
          ASSERT(FnTkns[i].m_iSym==i);
          if (Tkn.XStrICmp(FnTkns[i].m_sTkn)==0)
            {
            eSymbolType SymT=FnTkns[i].m_iSym;
            m_Tkns.Add(CSysVecFnSymbol(SymT));
            break;
            }
          }
        if (FnTkns[i].m_iSym >= SFS_LastVisTkn)
          LogError("SpecieConfig", LF_Exclamation, "Invalid Token %s", Tkn());
        }
      }
    }

  return true;
  };

//--------------------------------------------------------------------------

const int SVErrType_Load = 0;
bool SetError(long When, long What)
  {
  Strng errDesc;
  switch (What)
    {
    case 122 : errDesc = "Valid expression expected."; break;
    case 123 : errDesc = "Invalid symbol at start of expression."; break;
    case 138 : errDesc = "Comparison operator '==' expected."; break;
    case 139 : errDesc = "Comparison operator '!=' expected."; break;
    default: errDesc = "Unknown Error Number";
    };

  Strng Buff0;
  Buff0.Set("Build error:%s", errDesc);

  if (0)
    {
    LogError("SpecieDatabase", 0, Buff0());
    }
  return true;
  };

enum TknTypes { Tkn_Nothing, Tkn_Specie, Tkn_Name, Tkn_Number, Tkn_Other};

bool CSysVecFn::ParseToken(Strng &Tkn, long &TknType, LPCSTR &pStr)
  {
  Tkn="";
  TknType=Tkn_Nothing;
  while (*pStr==' ') // skip white space
    pStr++;
  if (*pStr=='[') // parse speciename
    {
    int BraceLvl=0;
    pStr++;
    do
      {
      if (*pStr=='[')
        BraceLvl++;
      else if (*pStr==']')
        BraceLvl--;
      if (BraceLvl>=0)
        {
        Tkn+=(*pStr>' '?*pStr:'_');
        }
      pStr++;
      }
    while (*pStr && BraceLvl>=0);
    // this is a specie/attname
    TknType=Tkn_Specie;
    }
  else
    {
    if (isdigit(*pStr) || ((*pStr=='-' || *pStr=='+') && isdigit(*(pStr+1))))
      {
      TknType=Tkn_Number;
      Tkn+=*pStr++;
      while (isdigit(*pStr)) Tkn+=*pStr++;
      if (*pStr=='.') Tkn+=*pStr++;
      while (isdigit(*pStr)) Tkn+=*pStr++;
      if (tolower(*pStr)=='e' && (*pStr=='+' || *pStr=='-' || isdigit(*pStr)))
        {
        Tkn+=*pStr++; // e
        Tkn+=*pStr++; // +,-,n
        while (isdigit(*pStr)) Tkn+=*pStr++;
        }
      }
    else if (isalpha(*pStr)) // parse function name
      {
      TknType=Tkn_Name;
      do
      Tkn+=*pStr++;
      while (*pStr && (isalnum(*pStr)));
      }
    else
      {
      TknType=Tkn_Other;
      Tkn=*pStr++;
      }
    // Tkn is a Symbol
    }


  return Tkn.Length()>0;
  }

//--------------------------------------------------------------------------

bool CSysVecFn::ParseExpression(LPCSTR Str)
  {
  if (Str)
    m_sFormula=Str;
  else
    Str=(LPCSTR)m_sFormula();

  m_Tkns.SetSize(0,8);
  bool ErrorOccurred=false;
  if (Str)
    {
    CSysVecFnSymbol Sym;

    Strng Tkn, Tkn2;
    long TknType;

    CSysVecFnSymbol opStack[1024];
    int opStackPos = -1;
    eInfixType InfixChar = IFC_Null;
    eInfixType PrevInfixChar;
    flag Pushed =  false;
    flag Valid = true;
    flag First = true;
    LPCSTR pStr=Str;
    ParseToken(Tkn, TknType, pStr);
    while (Valid)
      {
      PrevInfixChar = InfixChar;
      Sym.m_iSym = SFS_Null;
      if (TknType == Tkn_Number)
        {                                //numerical constant...
        Sym.m_iSym = SFS_Const;
        Sym.m_dValue = 1.2345;
        InfixChar = IFC_Const;
        }
      else if (TknType == Tkn_Specie)
        {                                // variable or funct expected...
        for (long i=0; i<SVCfgCount(); i++)
          {
          CSysVecItem *I=SVI.Cfg(i);
          if (Tkn.XStrICmp(I->FullTag())==0 || Tkn.XStrICmp(I->FullSym())==0)
            {
            Sym.m_iSym=SFS_SVIndex;
            Sym.m_iValue=I->SVIndex();
            InfixChar = IFC_Var;
            goto DoneSpc;
            }
          }
        LogError("SpecieConfig", LF_Exclamation, "Invalid dataname %s in expression", Tkn());
        Sym.m_iSym = SFS_Const;
        Sym.m_dValue = 1;
        InfixChar = IFC_Const;
DoneSpc:;
        }
      else if (TknType == Tkn_Name)
        {
        // Tkn is a Symbol
        for (long i=0; FnTkns[i].m_iSym < SFS_LastFunction; i++)
          {
          ASSERT(FnTkns[i].m_iSym==i);
          if (Tkn.XStrICmp(FnTkns[i].m_sTkn)==0)
            {
            Sym.m_iSym = FnTkns[i].m_iSym;
            InfixChar  = FnTkns[i].m_eInfix;
            goto DoneTkn;
            }
          }
        LogError("SpecieConfig", LF_Exclamation, "Invalid Function Name %s", Tkn());
        Sym.m_iSym = SFS_Const;
        Sym.m_dValue = 1;
        InfixChar = IFC_Const;
DoneTkn:;
        }
      else
        {
        switch (Tkn[0])
          {
          case '=' :
            ParseToken(Tkn2, TknType, pStr);
            if (Tkn2[0]!='=')
              ErrorOccurred=SetError(SVErrType_Load, 138);
            Sym.m_iSym = SFS_CmpEQ;
            InfixChar = IFC_Cmp;
            break;
          case '>' :
            InfixChar = IFC_Cmp;
            ParseToken(Tkn2, TknType, pStr);
            if (Tkn2[0] == '=')
              {
              Sym.m_iSym = SFS_CmpGE;
              Tkn[1] = Tkn2[0];
              Tkn[2] = 0;
              }
            else
              {
              Sym.m_iSym = SFS_CmpGT;
              }
            break;
          case '<' :
            InfixChar = IFC_Cmp;
            ParseToken(Tkn2, TknType, pStr);
            if (Tkn2[0] == '=')
              {
              Sym.m_iSym = SFS_CmpLE;
              Tkn[1] = Tkn2[0];
              Tkn[2] = 0;
              }
            else if (Tkn2[0] == '>')
              {
              Sym.m_iSym = SFS_CmpNE;
              Tkn[1] = Tkn2[0];
              Tkn[2] = 0;
              }
            else
              {
              Sym.m_iSym = SFS_CmpLT;
              }
            break;
          case '!' :
            ParseToken(Tkn2, TknType, pStr);
            if (Tkn2[0]!='=')
              ErrorOccurred=SetError(SVErrType_Load, 139);
            InfixChar = IFC_Cmp;
            Sym.m_iSym = SFS_CmpNE;
            Tkn[1] = '>';
            Tkn[2] = 0;
            break;
            //      case '[' :
            //        pDef = &GCTag;
            //        InfixChar = SFS_Var;
            //        break;
          default:
            break;
          }
        }

      if (Sym.m_iSym == SFS_Null)
        {
        for (long i=SFS_FirstOperator; FnTkns[i].m_iSym < SFS_LastOperator; i++)
          {
          ASSERT(FnTkns[i].m_iSym==i);
          if (Tkn.XStrICmp(FnTkns[i].m_sTkn)==0)
            {
            Sym.m_iSym = FnTkns[i].m_iSym;
            InfixChar  = FnTkns[i].m_eInfix;
            break;
            }
          }
        if (Sym.m_iSym!=SFS_Null)
          {                              //valid Def found...
          //if (!(pDef->m_defFlags & m_pH->m_iInsSet))
          //  SetError(SVErrType_Load, 130);
          //if (pDef->m_defFlags & DefRetStr)
          //  SetError(SVErrType_Load, 120);
          switch(Tkn[0])
            {
            case '+' :
            case '-' :
              if ( (PrevInfixChar == IFC_Operator) ||
                (PrevInfixChar == IFC_OBrace) ||
                (PrevInfixChar == IFC_Not) ||
                (PrevInfixChar == IFC_Cmp) ||
                (First) )
                {
                ParseToken(Tkn, TknType, pStr);
                if (TknType == Tkn_Number)
                  {                      //numerical constant...
                  Sym.m_iSym = SFS_Const;//pDef = &GCConst;
                  InfixChar  = IFC_Const;
                  }
                else
                  Sym.m_iSym = SFS_Null; //forces error
                }
              else
                InfixChar = IFC_Operator;
              break;
            case '(' :
              InfixChar = IFC_OBrace;
              break;
            case ')' :
              InfixChar = IFC_CBrace;
              break;
            default  :                   //valid function expected...
              if (FnTkns[Sym.m_iSym].m_eInfix==IFC_Operator)//pDef->m_defFlags & DefOperator)
                {
                if (Sym.m_iSym==SFS_Not)
                  InfixChar = IFC_Not;
                else
                  InfixChar = IFC_Operator;
                }
              else
                InfixChar = IFC_Funct;
              break;
            }
          }
        }

      Valid = (Sym.m_iSym!=SFS_Null);
      if (Valid)
        {
        if (First)
          {
          Valid = ( (InfixChar == IFC_Var) ||
            (InfixChar == IFC_Const) ||
            (InfixChar == IFC_Funct) ||
            (InfixChar == IFC_Not) ||
            (InfixChar == IFC_OBrace) );
          }
        else
          {
          switch (PrevInfixChar)
            {
            case IFC_Var :
            case IFC_Const :
            case IFC_Funct :
            case IFC_CBrace :
              Valid = ( (InfixChar == IFC_Operator) ||
                (InfixChar == IFC_CBrace) ||
                (InfixChar == IFC_Cmp) );
              break;
            case IFC_Cmp :
            case IFC_Operator :
            case IFC_OBrace :
            case IFC_Not :
              Valid = ( (InfixChar == IFC_Var) ||
                (InfixChar == IFC_Const) ||
                (InfixChar == IFC_Funct) ||
                (InfixChar == IFC_Not) ||
                (InfixChar == IFC_OBrace) );
              break;
            }
          }
        }
      if ((First) && (!Valid))
        ErrorOccurred=SetError(SVErrType_Load, 123);
      First = FALSE;

      #if dbgSysVec
      if (dbgExpressionLoad())
          dbgpln("Tkn:%-25s  Valid:%d  opStackPos:%d", Tkn, Valid, opStackPos);
       #endif

      if (Valid)
        {
        switch (InfixChar)
          {
          case (IFC_Const) :
            {
            Sym.m_dValue=SafeAtoF(Tkn());
            m_Tkns.Add(Sym);
            break;
            }
          case (IFC_Var)   :
            switch (Sym.m_iSym)
              {
              case SFS_SVIndex:
                m_Tkns.Add(Sym);
                break;
              case SFS_CfgIndex:
                m_Tkns.Add(Sym);
                break;
              default:
                DoBreak();
                break;
              }
            break;
          case (IFC_Funct) :
            m_Tkns.Add(Sym);
            break;
          case (IFC_OBrace) :
            opStack[++opStackPos] = CSysVecFnSymbol(SFS_Open);
            break;
          case (IFC_CBrace) :
            while ((opStackPos >= 0) && (opStack[opStackPos].m_iSym != SFS_Open))
              m_Tkns.Add(opStack[opStackPos--]);
            if (opStackPos < 0)
              Valid = FALSE;
            else
              opStackPos--;
            break;
          default:
            {
            Pushed = FALSE;
            while (!Pushed)
              {
              if ((opStackPos < 0) ||
                (FnTkns[Sym.m_iSym].m_Prec > FnTkns[opStack[opStackPos].m_iSym].m_Prec))
                {
                opStack[++opStackPos] = Sym;
                Pushed = true;
                }
              else
                m_Tkns.Add(opStack[opStackPos--]);////opStack[opStackPos--]->Construct(*this);
              }
            }
          }
        }
      if (Valid)
        ParseToken(Tkn, TknType, pStr);
      }
    while(opStackPos >= 0)
      {
      if (opStack[opStackPos].m_iSym == SFS_Open)
        {
        ErrorOccurred=SetError(SVErrType_Load, 122);
        opStackPos--;
        }
      else
        m_Tkns.Add(opStack[opStackPos--]);
      }
    }

  if (ErrorOccurred)
    m_Tkns.SetSize(0);

  m_bIsSpcsSum=false;

  return !ErrorOccurred;
  }

//--------------------------------------------------------------------------

bool CSysVecFn::CheckForPureSum()
  {
  CIArray Done;
  m_IndicesOfSumSpcs.SetSize(0);
  m_bIsSpcsSum=CheckForPureSum(m_IndicesOfSumSpcs, Done);
  if (!m_bIsSpcsSum)
    m_IndicesOfSumSpcs.SetSize(0);
  return m_bIsSpcsSum;
  }

//--------------------------------------------------------------------------

bool CSysVecFn::CheckForPureSum(CIArray & SumIndices, CIArray & Done)
  {
  bool Pure=true;
  for (int i=0; i<m_Tkns.GetSize(); i++)
    {
    CSysVecFnSymbol & Sym=m_Tkns[i];
    switch (Sym.m_iSym)
      {
      case SFS_SVIndex:
        if (Sym.m_iValue<0)
          { // Calculation
          Pure=SVI.Img(Sym.m_iValue)->m_Fn.CheckForPureSum(SumIndices, Done);
          }
        else if (Sym.m_iValue>=SVSpcCount())
          Pure=false;
        else
          {
          if (Sym.m_iValue<Done.GetSize())
            {
            if (Done[Sym.m_iValue])
              Pure=false;
            else
              Done[Sym.m_iValue]=1;
            }
          else
            Done.SetAtGrow(Sym.m_iValue, 1);
          SumIndices.Add(Sym.m_iValue);
          }
        break;
      case SFS_Open: // Valid Symbol for Puresum
      case SFS_Close:// Valid Symbol for Puresum
      case SFS_Plus: // Valid Symbol for Puresum
        break;
      case SFS_SumSLG:
      case SFS_SumS:
      case SFS_SumL:
      case SFS_SumG:
      //case SFS_SumSL:
      case SFS_SumPhSLG:
      case SFS_SumPhS:
      case SFS_SumPhL:
      case SFS_SumPhG:
      case SFS_SumIPhSLG:
      case SFS_SumIPh00:
      case SFS_SumIPh01:
      case SFS_SumIPh02:
      case SFS_SumIPh03:
      case SFS_SumIPh04:
      case SFS_SumIPh05:
      case SFS_SumIPh06:
      case SFS_SumIPh07:
      case SFS_SumIPh08:
      case SFS_SumIPh09:
      case SFS_SumIPh10:
      case SFS_SumIPh11:
      case SFS_SumIPh12:
      case SFS_SumIPh13:
      case SFS_SumIPh14:
      case SFS_SumIPh15:
      case SFS_SumIPh16:
      case SFS_SumIPh17:
      case SFS_SumIPh18:
      case SFS_SumIPh19:
      case SFS_SumIPh20:
      case SFS_SumIPh21:
      case SFS_SumIPh22:
      case SFS_SumIPh23:
      case SFS_SumIPh24:
      case SFS_SumIPh25:
      case SFS_SumIPh26:
      case SFS_SumIPh27:
      case SFS_SumIPh28:
      case SFS_SumIPh29:
      case SFS_SumIPh30:
      case SFS_SumIPh31:
      //MaxPhases Dependent
        {
        PhMask Ph=0;
        switch (Sym.m_iSym)
          {
          case SFS_SumSLG:    Ph=gs_CDB.m_som_ALL;  break;
          case SFS_SumS:      Ph=gs_CDB.m_som_Sol;  break;
          case SFS_SumL:      Ph=gs_CDB.m_som_Liq;  break;
          case SFS_SumG:      Ph=gs_CDB.m_som_Gas;  break;
          //case SFS_SumSL:     Ph=gs_CDB.m_som_SL;   break;
          case SFS_SumPhSLG:  Ph=gs_CDB.m_som_ALL;  break;
          case SFS_SumPhS:    Ph=gs_CDB.m_som_Sol;  break;
          case SFS_SumPhL:    Ph=gs_CDB.m_som_Liq;  break;
          case SFS_SumPhG:    Ph=gs_CDB.m_som_Gas;  break;
          case SFS_SumIPhSLG: Ph=gs_CDB.m_som_ALL;  break;
          case SFS_SumIPh00:  Ph=0x00000001;        break;
          case SFS_SumIPh01:  Ph=0x00000002;        break;
          case SFS_SumIPh02:  Ph=0x00000004;        break;
          case SFS_SumIPh03:  Ph=0x00000008;        break;
          case SFS_SumIPh04:  Ph=0x00000010;        break;
          case SFS_SumIPh05:  Ph=0x00000020;        break;
          case SFS_SumIPh06:  Ph=0x00000040;        break;
          case SFS_SumIPh07:  Ph=0x00000080;        break;
          case SFS_SumIPh08:  Ph=0x00000100;        break;
          case SFS_SumIPh09:  Ph=0x00000200;        break;
          case SFS_SumIPh10:  Ph=0x00000400;        break;
          case SFS_SumIPh11:  Ph=0x00000800;        break;
          case SFS_SumIPh12:  Ph=0x00001000;        break;
          case SFS_SumIPh13:  Ph=0x00002000;        break;
          case SFS_SumIPh14:  Ph=0x00004000;        break;
          case SFS_SumIPh15:  Ph=0x00008000;        break;
          case SFS_SumIPh16:  Ph=0x00010000;        break;
          case SFS_SumIPh17:  Ph=0x00020000;        break;
          case SFS_SumIPh18:  Ph=0x00040000;        break;
          case SFS_SumIPh19:  Ph=0x00080000;        break;
          case SFS_SumIPh20:  Ph=0x00100000;        break;
          case SFS_SumIPh21:  Ph=0x00200000;        break;
          case SFS_SumIPh22:  Ph=0x00400000;        break;
          case SFS_SumIPh23:  Ph=0x00800000;        break;
          case SFS_SumIPh24:  Ph=0x01000000;        break;
          case SFS_SumIPh25:  Ph=0x02000000;        break;
          case SFS_SumIPh26:  Ph=0x04000000;        break;
          case SFS_SumIPh27:  Ph=0x08000000;        break;
          case SFS_SumIPh28:  Ph=0x10000000;        break;
          case SFS_SumIPh29:  Ph=0x20000000;        break;
          case SFS_SumIPh30:  Ph=0x40000000;        break;
          case SFS_SumIPh31:  Ph=0x80000000;        break;
          //MaxPhases Dependent
          }
        for (int i=0; i<SVSpcCount(); i++)
          {
          int s=SVI.Spc(i)->SDBIndex();
          if (SDB[s].m_PhMsk & Ph)
            {
            if (s<Done.GetSize())
              {
              if (Done[s])
                Pure=false;
              else
                Done[s]=1;
              }
            else
              Done.SetAtGrow(s, 1);
            SumIndices.Add(s);
            }
          }

        break;
        }
      default:
        Pure=false;
      }
    }
  return Pure;
  }

//--------------------------------------------------------------------------

void CSysVecFn::DumpExpression()
  {
#if dbgSysVec
  if (dbgExpressionParse())
    {
    dbgpln("---------------");
    dbgpln("%s", m_sFormula());
    for (int i=0; i<m_Tkns.GetSize(); i++)
      {
      //dbgp("%3i %3i %s ", i, m_Tkns[i].m_iSym, FnTkns[m_Tkns[i].m_iSym].m_sTkn);
      switch (m_Tkns[i].m_iSym)
        {
        case SFS_SVIndex:
          dbgp("[%i %s] ", m_Tkns[i].m_iValue, SVI.Img(m_Tkns[i].m_iValue)->FullTag());
          break;
        case SFS_CfgIndex:
          dbgp("[%i %s] ", m_Tkns[i].m_iValue, SVI.Cfg(m_Tkns[i].m_iValue)->FullTag());
          break;
        case SFS_Const:
          dbgp("%.4f ", m_Tkns[i].m_dValue);
          break;
        default:
          dbgp("%s ", FnTkns[m_Tkns[i].m_iSym].m_sTkn);
        }
      //dbgpln("");
      }
    dbgpln("");
    if (m_bIsSpcsSum)
      {
      dbgp(" - PureSum: ");
      for (int i=0; i<m_IndicesOfSumSpcs.GetSize(); i++)
        dbgp(" %i", m_IndicesOfSumSpcs[i]);
      dbgpln("");
      }
    }
#endif
  }

//--------------------------------------------------------------------------

CSysVecItem::CSysVecItem(CSysVecItemType Type) : \
m_Fn(*(new CSysVecFn)),
m_Add(*(new CSysVecFn)),
m_Sub(*(new CSysVecFn)),
m_Scl(*(new CSysVecFn)),
m_AddDeriv(*(new CSysVecFn)),
m_sFormula(m_Fn.m_sFormula),
m_bIsSpcsSum(m_Fn.m_bIsSpcsSum),
m_IndicesOfSumSpcs(m_Fn.m_IndicesOfSumSpcs)
  {
  m_eType             = Type;
  m_bTagSpecd         = false;
  m_bSymSpecd         = false;
  m_iLevel            = 0;
  m_iCfgParent        = -1;
  m_iSVParent         = -1;
  m_pSVParent         = NULL;
  m_iCfgIndex         = -1;
  m_iSVIndex          = -1;
  m_iTypeIndex        = -1;
  m_iCnv              = DC_;
  m_iSDBIndex         = -1;
  m_iAnnotation       = SVA_Null;
  m_iPhCnt            = 0;
  m_iPhMsk            = 0;
  m_iOccMsk           = 0;
  m_iPhInx            = -1;
  m_iOccInx           = -1;
  m_iCalcType         = SVCT_General;
  m_iAttrType         = SVAT_Null;

  m_bInitHidden       = false;
  m_dwOptions         = 0;

  m_dMinValue         = dNAN;
  m_dMaxValue         = dNAN;
  };

//--------------------------------------------------------------------------

CSysVecItem::~CSysVecItem()
  {
  delete &m_Fn;
  delete &m_Add;
  delete &m_Sub;
  delete &m_Scl;
  delete &m_AddDeriv;
  };

//--------------------------------------------------------------------------

bool CSysVecItem::ParseBase(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options)
  {
  m_sParent=SafeStr(C[1]);
  m_sTag=SafeStr(C[2]);
  m_sSym=SafeStr(C[3]);

  m_sParent.Trim();
  m_sTag.Trim();
  m_sSym.Trim();

  m_bTagSpecd=m_sTag.Length()>0;
  m_bSymSpecd=m_sSym.Length()>0;

  if (!m_bTagSpecd)
    m_sTag=m_sSym;
  if (!m_bSymSpecd)
    m_sSym=m_sTag;

  int s=SVCfgCount();
  m_iCfgParent=-1;
  m_iSVParent=-1;
  m_pSVParent=NULL;
  m_dwOptions=Options;

  if (!ValidSpecieTag(m_sTag, false))
    {
    LogError("SpecieConfig", LF_Exclamation, "Invalid Tag %s", m_sTag());
    return false;
    }
  if (!ValidSpecieTag(m_sSym, false))
    {
    LogError("SpecieConfig", LF_Exclamation, "Invalid Tag %s", m_sSym());
    return false;
    }
  if (!ValidSpecieTag(m_sParent, true))
    {
    LogError("SpecieConfig", LF_Exclamation, "Invalid Tag %s", m_sFullTag());
    return false;
    }

  //for (int i=0; i<s; i++)
  //  {
  //  if ((SVI.Cfg(i)->m_eType!=SVT_Annotation) && SVI.Cfg(i)->m_sFullTag.XStrICmp(m_sFullTag)==0)
  //    {
  //    if ((Options & DuplicateOK)==0)
  //      LogError("SpecieConfig", LF_Exclamation, "Name %s Already exists ", m_sFullTag());
  //    return false;
  //    }
  //  }

  return true;
  }

//--------------------------------------------------------------------------

bool CSysVecItem::ParseCnv(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options)
  {
  m_iCnv=DC_;
  m_sCnv="";
  Strng S=SafeStr(C[5]);
  S.Trim();
  if (S=="*")
    {
    m_iCnv=-1;
    }
  else if (S())
    {
    CSVColArray Parm;
    int Quote,nParms;
    nParms = ParseCSVFunction(S(), Parm, Quote) - 1;
    if (Quote || nParms!=1)
      goto bad;
    m_iCnv = gs_CnvsMngr.FindCnv(Parm[0]);
    if (m_iCnv<0)
      goto bad;
    if (Cnvs[m_iCnv]->Find(Parm[1])==NULL)
      goto bad;
    m_sCnv= Parm[1];
    }
  return true;

bad:
  LogError("SpecieConfig", 0, "Invalid conversion %s", S());
  return false;
  };

//--------------------------------------------------------------------------

CSysVecItem * CSysVecInfo::ParseSpecie(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options, int InsertIndex)
  {
  CSysVecItem * p= new CSysVecItem(SVT_Specie);

  if (!p->ParseBase(nToks, C, CDBI, Options))
    goto bad;
  if (!p->ParseCnv(nToks, C, CDBI, Options))
    goto bad;

  p->m_sSDBTag=SafeStr(C[4]);
  p->m_sSDBTag.Trim();

  if (!ValidSpecieTag(p->m_sSDBTag, false))
    {
    LogError("SpecieConfig", LF_Exclamation, "Invalid SpecieDatabase Tag %s", p->m_sSDBTag());
    goto bad;
    }

  bool DupSpc=false;
  int NCfg=m_Cfg.GetCount();
  for (int i=0; i<NCfg && !DupSpc; i++)
    {
    CSysVecItem *p1=Cfg(i);
    if (p1->Type()==SVT_Specie)
      {
      //dbgpln("PS:%3i %-20s %-20s", i, p1->m_sSDBTag(), p->m_sSDBTag());
      if (p1 && p1->m_sSDBTag.XStrCmp(p->m_sSDBTag)==0)
        {
        p->m_iSDBIndex=p1->m_iSDBIndex;
        DupSpc=true;
        }
      }
    }
  if (!DupSpc)
    {
    p->m_iSDBIndex=CDBI.RqdSpecies.GetCount();
    CRqdSpItem I(p->m_sSDBTag.StrC());
    I.m_dLoTol=SafeAtoF(C[InfoColNo], 25);
    I.m_dHiTol=SafeAtoF(C[InfoColNo+1],25);
    if (nToks>InfoColNo+2)
      I.m_bUseIdeal=(C[InfoColNo+2] && strlen(C[InfoColNo+2])>0 && C[InfoColNo+2][0]=='1');
    CDBI.RqdSpecies.AddTail(I);
    m_nSVSpcCount++;
    m_Cfg.InsertAt(Min(m_Cfg.GetSize(), InsertIndex), p);
    goto good;
    }
  else
    {
    if ((Options & CSysVecItem::DuplicateOK)==0)
      LogError("SpecieConfig", LF_Exclamation, "Specie %s already used (TEMPORARY)", p->m_sSDBTag());
    goto bad;
    //CRqdSpItem I(C[3]);
    //I.m_iDuplicate=p->m_iSDBIndex;
    //CDBI.RqdSpecies.AddTail(I);
    }

bad:
  delete p;
  return NULL;
good:
  //dbgpln("ParseSpecie %3i '%s'",  p->m_iSDBIndex, p->m_sSDBTag());
  return p;
  }

//--------------------------------------------------------------------------

CSysVecItem * CSysVecInfo::ParseCalculation(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options, int InsertIndex)
  {
  CSysVecItem * p= new CSysVecItem(SVT_Calculation);

  if (p->ParseBase(nToks, C, CDBI, Options) && p->ParseCnv(nToks, C, CDBI, Options))
    {
    p->m_Fn.m_sFormula=SafeStr(C[InfoColNo]);
    p->m_Fn.m_sFormula.Trim();
    m_nSVClcCount++;
    m_Cfg.InsertAt(Min(m_Cfg.GetSize(), InsertIndex), p);
    return p;
    }
  delete p;
  return NULL;
  }

//--------------------------------------------------------------------------

CSysVecItem * CSysVecInfo::ParseAttribute(int nToks, CSVColArray & C, CDBInitRec &CDBI, dword Options, int InsertIndex)
  {
  CSysVecItem * p= new CSysVecItem(SVT_Attribute);

  if (p->ParseBase(nToks, C, CDBI, Options) && p->ParseCnv(nToks, C, CDBI, Options))
    {
    p->m_Add.m_sFormula=SafeStr(C[InfoColNo]);
    p->m_Sub.m_sFormula=SafeStr(C[InfoColNo+1]);
    p->m_Scl.m_sFormula=SafeStr(C[InfoColNo+2]);
    p->m_AddDeriv.m_sFormula=SafeStr(C[InfoColNo+3]);
    p->m_Add.m_sFormula.Trim();
    p->m_Sub.m_sFormula.Trim();
    p->m_Scl.m_sFormula.Trim();
    p->m_AddDeriv.m_sFormula.Trim();

    m_nSVAttCount++;
    m_Cfg.InsertAt(Min(m_Cfg.GetSize(), InsertIndex), p);
    return p;
    }
  delete p;
  return NULL;
  }

//--------------------------------------------------------------------------

CSysVecItem * CSysVecInfo::ParseAnnotation(int nToks, CSVColArray & C, CDBInitRec &CDBI, CSysVecItemAnno What, dword Options, int InsertIndex)
  {
  CSysVecItem * p= new CSysVecItem(SVT_Annotation);

  p->m_sTag=SafeStr(C[2]);
  p->m_sTag.Trim();
  if (p->m_sTag.GetLength()==0)
    p->m_sTag=" ";
  p->m_sSym=p->m_sTag;
  //p->m_iLevel=SafeAtoI(C[4],0);

  p->m_iAnnotation=What;

  p->m_sFullTag=p->m_sTag;
  p->m_sFullSym=p->m_sSym;
  m_nSVTxtCount++;
  m_Cfg.InsertAt(Min(m_Cfg.GetSize(), InsertIndex), p);
  goto good;

  //bad:
  delete p;
  return NULL;
good:
  return p;
  }

//--------------------------------------------------------------------------

CSysVecItem * CSysVecInfo::ParseSpecie(LPSTR Tag, LPSTR Sym, LPSTR LoTol, LPSTR HiTol, LPSTR UseIdeal, CDBInitRec &CDBI, dword Options, int InsertIndex)
  {
  int nToks1=1;
  CSVColArray C1;
  C1[0]="Specie";   // Key
  C1[1]="";         // Parent
  C1[2]=Tag;        // Tag  // this will get extarcted from SDB Name if reqd
  C1[3]=Sym;        // Sym
  C1[4]=Sym?Sym:Tag; // SDBTag
  C1[5]="*";         // CNV
  C1[6]="";
  C1[7]="";
  C1[8]="";
  C1[9]=LoTol;      // LoTol
  C1[10]=HiTol;      // HiTol
  C1[11]=UseIdeal;   // UseIdeal

  if (Tag)      nToks1=4;
  if (LoTol)    nToks1=9;
  if (HiTol)    nToks1=10;
  if (UseIdeal) nToks1=11;

  return ParseSpecie(nToks1, C1, CDBI, Options, InsertIndex);
  };

CSysVecItem * CSysVecInfo::ParseCalculation(LPSTR Tag, LPSTR Sym, LPSTR CalcStr, CDBInitRec &CDBI, dword Options, int InsertIndex)
  {
  int nToks1=9;
  CSVColArray C1;
  C1[0]="Calculation";  // Key
  C1[1]="";             // Parent
  C1[2]=Tag;            // Tag
  C1[3]=Sym;            // Sym
  C1[4]="";
  C1[5]="*";            // Cnv
  C1[6]="";
  C1[7]="";
  C1[8]="";
  C1[9]=CalcStr;
  return ParseCalculation(nToks1, C1, CDBI, Options, InsertIndex);
  }

CSysVecItem * CSysVecInfo::ParseAnnotation(LPSTR What, LPSTR Tag, CDBInitRec &CDBI, dword Options, int InsertIndex)
  {
  int nToks1=3;
  CSVColArray C1;
  C1[0]=What;   // Key
  C1[1]="";     // Parent
  C1[2]=Tag;    // Tag
  C1[3]=Tag;    // Sym
  C1[4]="";
  C1[5]="";
  return ParseAnnotation(nToks1, C1, CDBI, SVA_Text, Options, InsertIndex);
  }

//--------------------------------------------------------------------------

bool CSysVecInfo::ParseSpecieTemplateNames2(int nToks, CSVColArray & C, CDBInitRec &CDBI)
  {
  m_Templates.RemoveAll();

  // first col is specie names
  // thereafter they are the column name
  m_Templates.AddTail(new CSysVecTemplateItem("-"));

  for (int i=1; i<nToks; i++)
    {
    CString N=C[i];
    N.Trim(" ");
    if (N.GetLength()>0)
      {
      CSysVecTemplateItem *pT=new CSysVecTemplateItem(N);
      pT->m_SaveIt=false;
      m_Templates.AddTail(pT);
      }
    else
      break;
    }

  return true;
  };

//--------------------------------------------------------------------------

bool CSysVecInfo::ParseSpecieTemplateValues2(int nToks, CSVColArray & C, CDBInitRec &CDBI)
  {
  if (nToks<1)
    return false;

  CString Name=C[0];
  Name.Trim(" ");
  //int iSp=FindCfgIndex(SpName);
  for (int iEntry=0; iEntry<SVValueCount(); iEntry++)
    if (Name.Compare(Img(iEntry)->FullTag())==0)
      goto Found;

  LogWarning("SpecieTemplate",0,"Entry %s not found", Name);
   return false;

Found:
  POSITION Pos=m_Templates.GetHeadPosition();
  if (Pos)
    {
    CSysVecTemplateItem *T=m_Templates.GetNext(Pos); // skip the first
    for (int t=1; Pos && t<nToks; t++)
      {
      CSysVecTemplateItem *T=m_Templates.GetNext(Pos);
      CString N=C[t];
      N.Trim(" ");
      if ((N.GetLength()>0) && (N.Compare("*")!=0))
        T->m_Values[iEntry]=SafeAtoF(N, 0);
      }
    }

  return true;
  };

//--------------------------------------------------------------------------

void CSysVecInfo::FinaliseSpecieTemplateValues()
  {
  POSITION Pos=m_Templates.GetHeadPosition();
  while (Pos)
    {
    CSysVecTemplateItem *T=m_Templates.GetNext(Pos);
    double Sum=0;
    for (int i=0; i<SVValueCount(); i++)
      {
      if (Img(i)->Type()==SVT_Specie && Finite(T->m_Values[i]))
        Sum+=T->m_Values[i];
      }
    Sum=GTZ(Sum);
    for (int i=0; i<SVValueCount(); i++)
      {
      if (Img(i)->Type()==SVT_Specie && Finite(T->m_Values[i]))
        T->m_Values[i]/=Sum;
      }
    }

#if dbgSysVec
  if (dbgDumpTemplates())
    {
    dbgp("%-25s ", "");
    POSITION Pos=m_Templates.GetHeadPosition();
    while (Pos)
      {
      CSysVecTemplateItem *T=m_Templates.GetNext(Pos);
      dbgp("%15s ", T->m_Name);
      }
    dbgpln("");

    for (int i=0; i<SVValueCount(); i++)
      {
      dbgp("%-25s ", Img(i)->FullTag());
      switch (Img(i)->Type())
        {
        case SVT_Specie:
        //case SVT_Calculation:
        case SVT_Attribute:
          {
          POSITION Pos=m_Templates.GetHeadPosition();
          while (Pos)
            {
            CSysVecTemplateItem *T=m_Templates.GetNext(Pos);
            if (Finite(T->m_Values[i]))
              dbgp("%15.4f ", T->m_Values[i]);
            else
              dbgp("%15s ", "*");
            }
          }
        break;
        }
      dbgpln("");
      }
    }
#endif
  }

//--------------------------------------------------------------------------

CSysVecInfo::CSysVecInfo()  { Init(); };
CSysVecInfo::~CSysVecInfo() { Term(); };

//--------------------------------------------------------------------------

bool CSysVecInfo::Init()
  {
  m_nSVSpcCount   = 0;
  m_nSVAttCount   = 0;
  m_nSVClcCount   = 0;
  m_nSVTxtCount   = 0;
  m_nSVValueCount = 0;

  m_Img           = NULL;
  m_HasCalcs      = NULL; // for speed
  m_ChildCount    = NULL;

  m_ShowGroups    = false;

  m_SkipListFwdAll  = NULL;
  m_SkipListFwdSL   = NULL;
  m_SkipListFwdSol  = NULL;
  m_SkipListFwdLiq  = NULL;
  m_SkipListFwdGas  = NULL;
  m_SkipListFwdMap.InitHashTable(FindNextPrimeNumber(2000));

  m_SkipListRevAll  = NULL;
  m_SkipListRevSL   = NULL;
  m_SkipListRevSol  = NULL;
  m_SkipListRevLiq  = NULL;
  m_SkipListRevGas  = NULL;
  m_SkipListRevMap.InitHashTable(FindNextPrimeNumber(2000));

  return true;
  };

//--------------------------------------------------------------------------

bool CSysVecInfo::Term()
  {
  for (int i=0; i<m_Cfg.GetCount(); i++)
    delete m_Cfg[i];

  if (m_SkipListFwdAll)
    delete &(m_SkipListFwdAll[-m_nSVClcCount]);
  if (m_SkipListFwdSL )
    delete &(m_SkipListFwdSL [-m_nSVClcCount]);
  if (m_SkipListFwdSol)
    delete &(m_SkipListFwdSol[-m_nSVClcCount]);
  if (m_SkipListFwdLiq)
    delete &(m_SkipListFwdLiq[-m_nSVClcCount]);
  if (m_SkipListFwdGas)
    delete &(m_SkipListFwdGas[-m_nSVClcCount]);
  
  POSITION Pos=m_SkipListFwdMap.GetStartPosition();
  while (Pos != NULL)
    {
    dword Mask;
    int *pLst;
    m_SkipListFwdMap.GetNextAssoc( Pos, Mask, pLst);
    m_SkipListFwdMap.RemoveKey(Mask);
    delete &(pLst[-m_nSVClcCount]);
    }

  if (m_SkipListRevAll)
    delete &(m_SkipListRevAll[-m_nSVClcCount]);
  if (m_SkipListRevSL )
    delete &(m_SkipListRevSL [-m_nSVClcCount]);
  if (m_SkipListRevSol)
    delete &(m_SkipListRevSol[-m_nSVClcCount]);
  if (m_SkipListRevLiq)
    delete &(m_SkipListRevLiq[-m_nSVClcCount]);
  if (m_SkipListRevGas)
    delete &(m_SkipListRevGas[-m_nSVClcCount]);

  Pos=m_SkipListRevMap.GetStartPosition();
  while (Pos != NULL)
    {
    dword Mask;
    int *pLst;
    m_SkipListRevMap.GetNextAssoc( Pos, Mask, pLst);
    m_SkipListRevMap.RemoveKey(Mask);
    delete &(pLst[-m_nSVClcCount]);
    }

  m_SkipListFwdAll  = NULL;
  m_SkipListFwdSL   = NULL;
  m_SkipListFwdSol  = NULL;
  m_SkipListFwdLiq  = NULL;
  m_SkipListFwdGas  = NULL;

  m_SkipListRevAll  = NULL;
  m_SkipListRevSL   = NULL;
  m_SkipListRevSol  = NULL;
  m_SkipListRevLiq  = NULL;
  m_SkipListRevGas  = NULL;

  m_nSVValueCount = 0;
  m_nSVSpcCount   = 0;
  m_nSVAttCount   = 0;
  m_nSVClcCount   = 0;
  m_nSVTxtCount   = 0;

  m_HasCalcs      = NULL;
  m_ChildCount    = NULL;
  m_Img           = NULL;


  m_Cfg.SetSize(0);
  m_Clc.SetSize(0);
  m_Att.SetSize(0);
  m_ImgArray.SetSize(0);
  m_Spc.SetSize(0);

  m_HasCalcsArray.SetSize(0);
  m_ChildCountArray.SetSize(0);
  m_CalcsWithChildren.SetSize(0);
  m_Templates.RemoveAll();
  
  return true;
  }

//--------------------------------------------------------------------------

bool CSysVecInfo::Finalise(CDBInitRec &CDBI)
  {
  //dbgpln("========CSysVecInfo::Finalise========");
  //dbgpln("SDB.No() %i", SDB.No());
  // Check that All Species got Loaded
  int iSdb=0;
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    //dbgp("%3i %3i %-12s", iCfg, iSdb, CSysVecItemTypeNm[Type(iCfg)]);
    CSysVecItem *p=Cfg(iCfg);
    if (Type(iCfg)==SVT_Specie)
      {
      //dbgp(" %-15s %-15s %s", p->m_sSDBTag(), SDB[iSdb].Sym(), SDB[iSdb].Tag());
      if (iSdb>=SDB.Count() || p->m_sSDBTag.XStrCmp(SDB[iSdb].Sym())!=0)
        {
        // If we get here it implies that a specie was not loaded
        // change the config to Text
        Strng Txt;
        Txt.Set(" ERROR : %s not found in SpecieDB", p->m_sSDBTag());
        LogError("SpecieConfig", 0, "Specie %s not found in SpecieDB", p->m_sSDBTag());

        // remove specie line from cfg
        delete p;
        m_Cfg.RemoveAt(iCfg);

        // add a line of text
        ParseAnnotation("Text", Txt(), CDBI, 0, iCfg);
        }
      else
        {
        p->m_iSDBIndex=iSdb;
        // Fetch Tag and Sym from SDB if they were not specified
        if (!p->m_bTagSpecd)
          p->m_sTag=SDB[iSdb].Tag();
        if (!p->m_bSymSpecd)
          p->m_sSym=SDB[iSdb].Sym();

        iSdb++;
        }
      }
    //dbgpln("");
    }
  m_nSVSpcCount=iSdb;

  int iSpcCnt=0;
  int iAttCnt=0;
  int iClcCnt=0;
  int iTxtCnt=0;

  bool HasSol=false;
  bool HasLiq=false;
  bool HasVap=false;
  const int NSums = sizeof(FnSums)/sizeof(FnSums[0]);
  bool SumOK[NSums];
  for (int i=0; i<NSums; i++)
    SumOK[i]=false;

  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);
    if (Type(iCfg)==SVT_Specie)
      {
      HasSol=HasSol || SDB[p->m_iSDBIndex].IsSol();
      HasLiq=HasLiq || SDB[p->m_iSDBIndex].IsLiq();
      HasVap=HasVap || SDB[p->m_iSDBIndex].IsGas();
      }
    else if (Type(iCfg)==SVT_Calculation)
      {
      for (int i=0; i<NSums; i++)
        {
        if (p->m_Fn.m_sFormula.XStrCmp(FnTkns[FnSums[i].m_iSym].m_sTkn)==0)
          {
          p->m_iCalcType=FnSums[i].m_iCalcType;
          SumOK[i]=1;
          }
        }
      }
    }

  // Check for Extras
  int InsertAt=0;
  Strng S;
  CSysVecItem *I;
  ParseAnnotation("Text", "-- Species --", CDBI, 0, InsertAt++);

  for (int i=0; i<NSums; i++)
    {
    if ((FnSums[i].m_iSym-SFS_SumIPh00)>=CDB.PhaseCount())
      break;
    if (!SumOK[i] && ((Fn2PhMasks[i]&CDB.m_som_ALL)!=0))
      {
      Strng Tag=(LPSTR)(LPCTSTR)FnSums[i].m_sTag;
      Strng Sym=(LPSTR)(LPCTSTR)FnSums[i].m_sSym;
      if (FnSums[i].m_iSym>=SFS_SumIPh00)
        {
        int iPh=FnSums[i].m_iSym-SFS_SumIPh00;
        Tag=CDB.PhaseInfo(iPh).m_Tag;
        Sym=CDB.PhaseInfo(iPh).m_Sym;
        }

      I=ParseCalculation(Tag(), Sym(), (LPSTR)(LPCTSTR)FnTkns[FnSums[i].m_iSym].m_sTkn, CDBI, 0, InsertAt++);
      I->m_iCalcType=FnSums[i].m_iCalcType;
      }
    }

  if (InsertAt)
    {
    if (Type(InsertAt)!=SVT_Annotation)
      {
      I=ParseAnnotation("Text", " ", CDBI, 0, InsertAt++);
//      I->m_bIsPhaseCalc=true;
      }
    }

  // Cfg now OK
  m_nSVValueCount=m_nSVSpcCount+m_nSVAttCount;

#if dbgSysVec
  if (dbgConfig())
    {
    dbgpln("SDI:SVCfgCount: %i", SVCfgCount());
    dbgpln("SDI:SVSpcCount: %i", SVSpcCount());
    dbgpln("SDI:SVAttCount: %i", SVAttCount());
    dbgpln("SDI:SVClcCount: %i", SVClcCount());
    dbgpln("SDI:SVTxtCount: %i", SVTxtCount());
    dbgpln("SDI:SVValueCount: %i", SVValueCount());
    }
#endif

  //Extras Done
  m_ImgArray.SetSize(SVClcCount()+Max(1L,SVValueCount()));
  m_HasCalcsArray.SetSize(SVClcCount()+Max(1L,SVValueCount()));
  m_ChildCountArray.SetSize(SVClcCount()+Max(1L,SVValueCount()));

  // These adjustments will allow for negative indices
  m_Img         = m_ImgArray.GetSize() ? &m_ImgArray[SVClcCount()] : NULL;
  m_HasCalcs    = m_HasCalcsArray.GetSize() ? &m_HasCalcsArray[SVClcCount()] : NULL;
  m_ChildCount  = m_ChildCountArray.GetSize() ? &m_ChildCountArray[SVClcCount()] : NULL;

  m_CalcsWithChildren.SetSize(0);

  for (int iImg=-SVClcCount(); iImg<SVValueCount(); iImg++)
    {
    m_Img[iImg]         = NULL;
    m_HasCalcs[iImg]    = 0;
    m_ChildCount[iImg]  = 0;
    }

  // Add items to Img and other arrays.
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Calculation)
      {
      CSysVecItem *p=Cfg(iCfg);
      m_Clc.Add(p);
      //m_ImgArray.Add(p);
      p->m_iCfgIndex=iCfg;
      p->m_iTypeIndex=iClcCnt;
      p->m_iSVIndex=-SVClcCount()+iClcCnt;
      m_Img[p->m_iSVIndex]=p;

      iClcCnt++;
      }
    }

  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Specie)
      {
      CSysVecItem *p=Cfg(iCfg);
      m_Spc.Add(p);
      //m_ImgArray.Add(p);
      p->m_iCfgIndex  = iCfg;
      p->m_iTypeIndex = iSpcCnt;
      p->m_iSVIndex   = iSpcCnt;
      m_Img[p->m_iSVIndex]=p;

      PhMask PM       = SDB[iSpcCnt].m_PhMsk;
      PhMask OM       = 0;
      if (PM&som_Sol) OM|=som_Sol;
      if (PM&som_Liq) OM|=som_Liq;
      if (PM&som_Gas) OM|=som_Gas;
      p->m_iPhMsk     = PM;
      p->m_iOccMsk    = OM;
      p->m_iPhInx     = SDB[iSpcCnt].m_PhInx;
      p->m_iOccInx    = SDB[iSpcCnt].BaseOccType();
      iSpcCnt++;
      }
    }
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Attribute)
      {
      CSysVecItem *p=Cfg(iCfg);
      m_Att.Add(p);
      //m_ImgArray.Add(p);
      p->m_iCfgIndex  = iCfg;
      p->m_iTypeIndex = iAttCnt;
      p->m_iSVIndex   = SVSpcCount()+iAttCnt;
      m_Img[p->m_iSVIndex]=p;

      p->m_iPhMsk     = 0;
      p->m_iOccMsk    = 0;
      p->m_iPhInx     = -1;
      p->m_iOccInx    = -1;
      iAttCnt++;
      }
    }

  //m_Img=&m_ImgArray[iClcCnt];

  // Find Parents and Create FUllTag;
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);
    switch (Type(iCfg))
      {
      case SVT_Specie:
      case SVT_Calculation:
      case SVT_Attribute:
        {
        if (p->m_sParent.Len()>0)
          {
          for (int ip=0; ip<iCfg && p->m_iCfgParent<0; ip++)
            {
            CSysVecItem *IP=SVI.Cfg(ip);
            if (IP->m_sFullTag.XStrICmp(p->m_sParent)==0 || IP->m_sFullSym.XStrICmp(p->m_sParent)==0)
              {
              p->m_iCfgParent=ip;
              p->m_pSVParent=IP;
              }
            }
          if (p->m_iCfgParent<0)
            {
            LogError("SpecieConfig", LF_Exclamation, "Unknown or Invalid Parent Tag %s", p->m_sParent());
            return false;
            }
          }

        p->m_iLevel=0;
        int iParent=p->m_iCfgParent;
        p->m_sFullTag=p->m_sTag;
        p->m_sFullSym=p->m_sSym;
        while (iParent>=0)
          {
          Strng S;
          CSysVecItem &Prnt=*SVI.Cfg(iParent);
          p->m_iLevel++;
          S.Set("%s.%s", Prnt.Tag(), p->m_sFullTag());
          p->m_sFullTag=S;
          S.Set("%s.%s", Prnt.Sym(), p->m_sFullSym());
          p->m_sFullSym=S;
          iParent=Prnt.m_iCfgParent;
          }
        if (!ValidSpecieTag(p->m_sFullTag, false))
          {
          LogError("SpecieConfig", LF_Exclamation, "Invalid Tag %s", p->m_sFullTag());
          return false;
          }
        if (!ValidSpecieTag(p->m_sFullSym, false))
          {
          LogError("SpecieConfig", LF_Exclamation, "Invalid Symbol %s", p->m_sFullSym());
          return false;
          }

        // check that this tag does not exist
        if ((p->m_dwOptions & CSysVecItem::DuplicateOK)==0)
          for (int is=0; is<iCfg; is++)
            {
            if ((SVI.Cfg(is)->m_eType!=SVT_Annotation) && (SVI.Cfg(is)->CalcType()<=SVCT_General))
              {
              if (SVI.Cfg(is)->m_sFullTag.XStrICmp(p->m_sFullTag)==0)
                LogError("SpecieConfig", LF_Exclamation, "Tag %s Already exists ", p->m_sFullTag());
              else if (SVI.Cfg(is)->m_sFullSym.XStrICmp(p->m_sFullSym)==0)
                LogError("SpecieConfig", LF_Exclamation, "Symbol %s Already exists ", p->m_sFullSym());
              else
                continue;
              return false;
              }
            }
        }
      }
    }

  // now do the parsing of Expresssions
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Calculation)
      {
      if (!Cfg(iCfg)->m_Fn.ParseExpression(NULL))
        LogError("SpecieConfig", LF_Exclamation, "Invalid Expression %s", Cfg(iCfg)->m_Fn.m_sFormula());
      }
    }

  // go through attributes and find their phases
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Attribute)
      {
      CSysVecItem * p=Cfg(iCfg);
      //dbgpln("----------------------- %s", p->m_sFullTag());
      p->m_iPhMsk  = p->m_pSVParent->PhaseMask();
      p->m_iPhInx  = p->m_pSVParent->PhaseInx();
      p->m_iPhCnt  = p->m_pSVParent->PhaseCount();
      p->m_iOccMsk = p->m_pSVParent->OccMask();
      p->m_iOccInx = p->m_pSVParent->OccInx();
      }
    }

  // now go through calculations and find their phases
  // Keep going until there is no change to account for forward/reverse references
  bool OccChanged=true;
  for (int Pass=0; OccChanged && (Pass<1000); Pass++)
    {
    OccChanged=false;
    for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
      {
      if (Type(iCfg)==SVT_Calculation)
        {
        CSysVecItem * p=Cfg(iCfg);
        //dbgpln("----------------------- %s", p->m_sFullTag());
        p->m_Fn.CheckForPureSum();
        PhMask OccMsk=0;
        PhMask PhMsk=0;
        int OccInx=-2;
        int PhInx=-2;
        CSysVecFn &Fn=p->m_Fn;
        if (Fn.m_bIsSpcsSum)
          {
          CIArray & Inx=Fn.m_IndicesOfSumSpcs;
          for (int j=0; j<Inx.GetSize(); j++)
            {
            int iSdb=Inx[j];
            int ot=SDB[iSdb].BaseOccType();
            OccInx=(OccInx==-2 || OccInx==ot) ? ot : -1;
            switch (ot)
              {
              case BOT_Solid:   OccMsk|=som_Sol; break;
              case BOT_Liquid:  OccMsk|=som_Liq; break;
              case BOT_Gas:     OccMsk|=som_Gas; break;
              }

            int ph=SDB[iSdb].m_PhInx;
            PhInx=(PhInx==-2 || PhInx==ph) ? ph : -1;
            PhMsk|=SDB[iSdb].m_PhMsk;
            }
          }
        else
          {
          for (int t=0; t<Fn.m_Tkns.GetSize(); t++)
            {
            CSysVecFnSymbol &T=Fn.m_Tkns[t];
            switch (T.m_iSym)
              {
              //case SFS_SVIndex:
              //  OccMsk|=SVI.Img(T.m_iValue)-OccMask();
              //  PhMsk|=SVI.Img(T.m_iValue)->PhaseMask();
              //  break;
              //case SFS_CfgIndex:
              //  OccMsk|=SVI.Cfg(T.m_iValue)->OccMask();
              //  PhMsk|=SVI.Cfg(T.m_iValue)->PhaseMask();
              //  break;

              case SFS_SVIndex:
              case SFS_CfgIndex:
                {
                int ot=SVI.Img(T.m_iValue)->OccInx();
                OccInx=(OccInx==-2 || OccInx==ot) ? ot : -1;
                switch (ot)
                  {
                  case BOT_Solid:   OccMsk|=som_Sol; break;
                  case BOT_Liquid:  OccMsk|=som_Liq; break;
                  case BOT_Gas:     OccMsk|=som_Gas; break;
                  }

                int ph=SVI.Img(T.m_iValue)->PhaseInx();
                PhInx=(PhInx==-2 || PhInx==ph) ? ph : -1;
                PhMsk|=SVI.Img(T.m_iValue)->PhaseMask();
                break;
                }
              }
            }
          int xxx=0;
          }
        OccChanged = OccChanged || (p->m_iPhMsk != PhMsk) || (p->m_iOccMsk != OccMsk);
        p->m_iPhMsk  = PhMsk;
        p->m_iOccMsk = OccMsk;
        p->m_iPhInx  = Max(-1, PhInx);
        p->m_iOccInx = Max(-1, OccInx);
        p->m_Fn.DumpExpression();
        }
      }
    }

  for (int p=CDB.PhaseFirst(); p<=CDB.PhaseLast(); p++)
    {
    sm_SpVwPhMask[SpVwPhMaskIndex_Phase0+p].m_Sym=CDB.PhaseInfo(p).m_Sym();
    CString S;
    S.Format("%%(%s)", CDB.PhaseInfo(p).m_Sym());
    gs_SPMDC_FracPh[p].m_Txt=S;
    Cnvs[DC_FracPh00+p]->AddSecondaryCnv((LPSTR)(LPCTSTR)S, 100.0, 0.0, false);
    }

  // count the species & phases in each occurrence
  long CntSpcsPh[MaxPhases]={0};
  long CntSpcsOcc[BOT_All]={0};
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);
    PhMask PM=p->m_iPhMsk;
    if (PM)
      {
      byte PhCnt=0;
      while (PM)
        {
        if (PM&0x0000001)
          PhCnt++;
        PM=PM>>1;
        }
      p->m_iPhCnt=PhCnt;
      //dbgpln(">>>>>>>>> %#010x %3i %s", p->m_iPhMsk, p->m_iPhCnt,p->FullTag());
      if (Type(iCfg)==SVT_Specie)
        {
        CntSpcsPh[Cfg(iCfg)->m_iPhInx]++;
        CntSpcsOcc[Cfg(iCfg)->m_iOccInx]++;
        }
      }
    }

  // Check that Parents Are OK;
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);
    if (Type(iCfg)==SVT_Attribute)
      {
      if (p->m_iCfgParent>=0)
        {
        if (p->m_pSVParent->m_iPhCnt!=1)
          {
          p->m_iCfgParent=-1;
          p->m_iSVParent=-1;
          p->m_pSVParent=NULL;
          LogError("SpecieConfig", 0, "Parent of Attribute %s has %i phases instead of one", p->FullTag(), p->m_pSVParent->m_iPhCnt);
          }
        }
      else
        {
        LogError("SpecieConfig", 0, "Attribute %s has no Parent", p->FullTag());
        }
      }
    }

  //find SuperPhaseStuff
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);

    p->m_ViewBasisRules[SVVB_All].m_iSpVwMaskIndex    = SpVwPhMaskIndex_All;
    p->m_ViewBasisRules[SVVB_Sol].m_iSpVwMaskIndex    = SpVwPhMaskIndex_Solid;
    p->m_ViewBasisRules[SVVB_Liq].m_iSpVwMaskIndex    = SpVwPhMaskIndex_Liquid;
    p->m_ViewBasisRules[SVVB_Gas].m_iSpVwMaskIndex    = SpVwPhMaskIndex_Gas;
    p->m_ViewBasisRules[SVVB_Occ].m_iSpVwMaskIndex    = SpVwPhMaskIndex_Solid+p->m_iOccInx;
    p->m_ViewBasisRules[SVVB_Phase].m_iSpVwMaskIndex  = SpVwPhMaskIndex_Phase0+p->m_iPhInx;

    switch (Type(iCfg))
      {
      case SVT_Specie:
        p->m_ViewBasisRules[SVVB_All].m_bEditable         = true;
        p->m_ViewBasisRules[SVVB_Sol].m_bEditable         = true;
        p->m_ViewBasisRules[SVVB_Liq].m_bEditable         = true;
        p->m_ViewBasisRules[SVVB_Gas].m_bEditable         = true;
        p->m_ViewBasisRules[SVVB_Occ].m_bUsed             = true;
        p->m_ViewBasisRules[SVVB_Phase].m_bUsed           = true;
        p->m_ViewBasisRules[SVVB_Occ].m_iSpVwMaskIndex    =  SpVwPhMaskIndex_Solid+p->m_iOccInx;
        p->m_ViewBasisRules[SVVB_Phase].m_iSpVwMaskIndex  =  SpVwPhMaskIndex_Phase0+p->m_iPhInx;
        p->m_ViewBasisRules[SVVB_Occ].m_bEditable         = (CntSpcsOcc[p->m_iOccInx]>1);
        p->m_ViewBasisRules[SVVB_Phase].m_bEditable       = (CntSpcsPh[p->m_iPhInx]>1);
        if (0)
          {
          dbgpln("Spc   %3i %s %08x %08x %3i %3i %-5s %-5s %s",
            iCfg, "   ", p->m_iOccMsk, p->m_iPhMsk, p->m_iOccInx, p->m_iPhInx,
            (LPCTSTR)sm_SpVwPhMask[p->m_ViewBasisRules[SVVB_Occ].m_iSpVwMaskIndex].m_Sym,
            (LPCTSTR)sm_SpVwPhMask[p->m_ViewBasisRules[SVVB_Phase].m_iSpVwMaskIndex].m_Sym,
            p->FullTag());
          }
        break;
      case SVT_Attribute:
        break;
      case SVT_Calculation:
        {
        p->m_ViewBasisRules[SVVB_All].m_bEditable   = p->FnIsSpcsSum() && (p->CalcType() > SVCT_General) && (p->CalcType() != SVCT_Total);
        p->m_ViewBasisRules[SVVB_Sol].m_bEditable   = p->FnIsSpcsSum();
        p->m_ViewBasisRules[SVVB_Liq].m_bEditable   = p->FnIsSpcsSum();
        p->m_ViewBasisRules[SVVB_Gas].m_bEditable   = p->FnIsSpcsSum();
        p->m_ViewBasisRules[SVVB_Occ].m_bEditable   = p->CalcType() == SVCT_PhSum;
        p->m_ViewBasisRules[SVVB_Phase].m_bEditable = p->CalcType() == SVCT_IPhSum;

        p->m_ViewBasisRules[SVVB_All].m_bUsed       = p->CalcType() <= SVCT_Total;
        p->m_ViewBasisRules[SVVB_Occ].m_bUsed       = p->CalcType() == SVCT_General || p->CalcType() >= SVCT_PhSum && p->CalcType()  <= SVCT_PhTotal;
        p->m_ViewBasisRules[SVVB_Phase].m_bUsed     = p->CalcType() == SVCT_General || p->CalcType() >= SVCT_IPhSum && p->CalcType() <= SVCT_IPhTotal;

        if (p->CalcType() >= SVCT_PhSum)
          {
          p->m_ViewBasisRules[SVVB_Occ].m_iSpVwMaskIndex=SpVwPhMaskIndex_All;
          p->m_ViewBasisRules[SVVB_Phase].m_iSpVwMaskIndex=SpVwPhMaskIndex_All;
          }
        else
          {
          p->m_ViewBasisRules[SVVB_Occ].m_iSpVwMaskIndex    =  SpVwPhMaskIndex_Solid+p->m_iOccInx;
          p->m_ViewBasisRules[SVVB_Phase].m_iSpVwMaskIndex  =  SpVwPhMaskIndex_Phase0+p->m_iPhInx;
          }
        if (0)
          {
          dbgpln("Calc  %3i %s %08x %08x %3i %3i %-5s %-5s %s",
            iCfg, "   ", p->m_iOccMsk, p->m_iPhMsk, p->m_iOccInx, p->m_iPhInx,
            (LPCTSTR)sm_SpVwPhMask[p->m_ViewBasisRules[SVVB_Occ].m_iSpVwMaskIndex].m_Sym,
            (LPCTSTR)sm_SpVwPhMask[p->m_ViewBasisRules[SVVB_Phase].m_iSpVwMaskIndex].m_Sym,
            p->FullTag());
          }

        break;
        }
      }
    }

  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Attribute)
      {
      CSysVecItem *p=Cfg(iCfg);
      if (p->m_Add.m_sFormula.XStrNICmp("Quality", 7)==0)
        {
        p->m_iAttrType = SVAT_Qual;
        Strng S=p->m_Add.m_sFormula;
        CSVColArray C;
        int Quote=0;
        int n=ParseCSVFunction(S(), C, Quote);
        if (n>=2)
          p->m_dMinValue=SafeAtoF(C[1], dNAN);
        else
          p->m_dMinValue=dNAN;
        if (n>=3)
          p->m_dMaxValue=SafeAtoF(C[2], dNAN);
        else
          p->m_dMaxValue=dNAN;
        int xxx=0;
        }
      else if (p->m_Add.m_sFormula.XStrICmp("Amount")==0)
        {
        p->m_iAttrType = SVAT_Amnt;
        }
      else
        {
        p->m_iAttrType = SVAT_Calc;
        p->m_Add.ParseExpression(NULL);
        p->m_Sub.ParseExpression(NULL);
        p->m_Scl.ParseExpression(NULL);
        p->m_AddDeriv.ParseExpression(NULL);
        }

      bool HasCalcs=(p->m_Add.m_Tkns.GetSize()>0) ||
        (p->m_Sub.m_Tkns.GetSize()>0) ||
        (p->m_Scl.m_Tkns.GetSize()>0) ||
        (p->m_AddDeriv.m_Tkns.GetSize()>0);
      m_HasCalcsArray[p->m_iSVIndex]=HasCalcs;
      }
    }

  // Parent / Children
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);
    if (p->m_pSVParent)
      {
      p->m_iSVParent=p->m_pSVParent->m_iSVIndex;
      p->m_pSVParent->m_ChildrenCfgIndex.Add(p->m_iCfgIndex);
      p->m_pSVParent->m_ChildrenSVIndex.Add(p->m_iSVIndex);
      }
    }

  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    CSysVecItem *p=Cfg(iCfg);

    p->m_Fn.m_iSVIndex=p->SVIndex();
    p->m_Add.m_iSVIndex=p->SVIndex();
    p->m_Sub.m_iSVIndex=p->SVIndex();
    p->m_Scl.m_iSVIndex=p->SVIndex();
    p->m_AddDeriv.m_iSVIndex=p->SVIndex();

    switch (Type(iCfg))
      {
      case SVT_Calculation:
        if (p->ChildCount()>0)
          m_CalcsWithChildren.Add(p->SVIndex());
        m_ChildCount[p->m_iSVIndex]=p->m_ChildrenCfgIndex.GetSize();
        break;
      case SVT_Specie:
        m_ChildCount[p->m_iSVIndex]=p->m_ChildrenCfgIndex.GetSize();
        break;
      };
    }

  // Text
  for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
    {
    if (Type(iCfg)==SVT_Annotation)
      {
      Cfg(iCfg)->m_iCfgIndex = iCfg;
      Cfg(iCfg)->m_iTypeIndex = -1;
      Cfg(iCfg)->m_iSVIndex   = -1;
      }
    }

#if dbgSysVec
  if (dbgConfig())
    {
    dbgp("CalcsWithChildren:");
    for (int i=0; i<m_CalcsWithChildren.GetSize(); i++)
      dbgp(" %i", m_CalcsWithChildren[i]);
    dbgpln("");
    // DEBUG
    dbgpln("");
    dbgpln("Cfg --------------------------------------------------------");
    dbgpln("SDI:%-12s %3s %3s %3s %3s %10s %3s %3s %10s %3s %3s %3s %-12s %-30s %-12s ",
             "Type","Cfg","SV","Ty","PIx","PhMsk", "Cnt","OIx","OccMsk","Par","#Ch","Lvl","Cnv", "Tag", "Sym");
    for (int iCfg=0; iCfg<SVCfgCount(); iCfg++)
      {
      CSysVecItem *p=Cfg(iCfg);

      if (p->Type()==SVT_Annotation)
        {
        dbgpln("SDI:%-12s %s", CSysVecItemTypeNm[p->m_eType], p->FullTag());
        continue;
        }

      Strng Cnv("*");
      if (p->CnvIndex()>=0)
        Cnv.Set("%s(%s)", Cnvs[p->CnvIndex()]->Fam(), p->CnvString()?p->CnvString():"");

      dbgp("SDI:%-12s %3i %3i %3i %3i %#010x %3i %3i %#010x %3i %3i %3i %-12s %-30s %-12s",
        CSysVecItemTypeNm[p->m_eType], p->CfgIndex(), p->SVIndex(), p->TypeIndex(),
        p->PhaseInx(), p->PhaseMask(), p->PhaseCount(), p->OccInx(), p->OccMask(),
        p->m_iCfgParent, m_ChildCount[p->SVIndex()], p->Level(), Cnv(), p->FullTag(), p->FullSym());
      switch (p->Type())
        {
        case SVT_Specie:
          dbgp("SDB{%3i %-20s}", Cfg(iCfg)->m_iSDBIndex, Cfg(iCfg)->m_sSDBTag());
          break;
        case SVT_Calculation:
          dbgp("%-8s = %s", SV_CalcTypesStr[Cfg(iCfg)->CalcType()], Cfg(iCfg)->m_Fn.m_sFormula());
          break;
        case SVT_Attribute:
          dbgp("'%s', '%s', '%s', '%s'",
            Cfg(iCfg)->m_Add.m_sFormula()     ?Cfg(iCfg)->m_Add.m_sFormula():""      ,
            Cfg(iCfg)->m_Sub.m_sFormula()     ?Cfg(iCfg)->m_Sub.m_sFormula():""      ,
            Cfg(iCfg)->m_Scl.m_sFormula()     ?Cfg(iCfg)->m_Scl.m_sFormula():""      ,
            Cfg(iCfg)->m_AddDeriv.m_sFormula()?Cfg(iCfg)->m_AddDeriv.m_sFormula():"");
          break;
        }
      dbgp(" ");
      if (p->m_ChildrenSVIndex.GetCount())
        {
        dbgp("#%2i", p->m_ChildrenSVIndex.GetCount());
        dbgp("[");
        CArray<long,long> & Ch=p->m_ChildrenSVIndex;
        for (int c=0; c<Ch.GetCount(); c++)
          dbgp(c>0?",%i":"%i", Ch[c]);
        dbgp("]");
        }
      dbgpln("");
      }


    dbgpln("");
    dbgpln("Img --------------------------------------------------------");

    dbgp("    %-12s                     ","");
    dbgp("|%20s ", "All");
    dbgp("|%20s ", "Solid");
    dbgp("|%20s ", "Liquid");
    dbgp("|%20s ", "Gas");
    dbgp("|%20s ", "Phase");
    dbgp("|%20s ", "IndPhase");
    dbgpln("|");

    dbgp("SDI:%-12s %3s %3s %3s %3s %3s %10s %10s ","Type","Cfg","SV","Ty","#Ch","Lvl","OccMask","PhMask");
    for (int x=0; x<SVVB_End; x++)
      dbgp("|%3s       %10s ","Inx","Msk");
    dbgpln("|%-20s ","Tag");
    for (int i=-SVClcCount(); i<SVValueCount(); i++)
      {
      CSysVecItem *p=&SVImg(i);
      dbgp("SDI:%-12s %3i %3i %3i %3i %3i %#010x %#010x ",
        CSysVecItemTypeNm[p->m_eType], p->CfgIndex(), p->SVIndex(), p->TypeIndex(), m_ChildCount[p->SVIndex()], p->Level(),
        p->OccMask(), p->PhaseMask());

      for (eScdSVViewBasis b=SVVB_Start; b<SVVB_End; b++)
        {
        dbgp("|%s%s%2i %-4s %#010x ",
              p->ViewBasisUsed(b)?"U":" ",
              p->ViewBasisEditable(SVV_AsMassFrac, b)?"E":" ",
              p->m_ViewBasisRules[b].m_iSpVwMaskIndex,
              p->ViewBasisName(b),
              p->ViewBasisMask(b));
        }
      dbgp("|%-30s ", p->FullTag());

      if (p->m_iCfgParent>=0)
        dbgp("Par:[%i]%s ",p->m_iSVParent, p->m_pSVParent->Tag());
      if (p->m_ChildrenSVIndex.GetCount())
        {
        dbgp("[");
        CArray<long,long> & Ch=p->m_ChildrenSVIndex;
        for (int c=0; c<Ch.GetCount(); c++)
          dbgp(c>0?",%i":"%i", Ch[c]);
        dbgp("]");
        }
      dbgpln("");
      }

    ASSERT(iSpcCnt==SVSpcCount());
    ASSERT(iAttCnt==SVAttCount());
    ASSERT(iClcCnt==SVClcCount());
    dbgpln("------------------------------------------------------------");
    }
#endif
  return true;
  }

//---------------------------------------------------------------------------

int * CSysVecInfo::SetUpSkipList(PhMask Mask)
  {
  dword PhM=Mask;
  int *pLst;
  if (Mask==som_ALL)
    pLst=m_SkipListFwdAll;
  else if (Mask==som_SL)
    pLst=m_SkipListFwdSL;
  else if (Mask==som_Sol)
    pLst=m_SkipListFwdSol;
  else if (Mask==som_Liq)
    pLst=m_SkipListFwdLiq;
  else if (Mask==som_Gas)
    pLst=m_SkipListFwdGas;
  else if (!m_SkipListFwdMap.Lookup(Mask, pLst))
    pLst=NULL;

  if (pLst==NULL)//m_SkipList[PhM]==NULL)
    {
    //m_iLastSkipList=Max(m_iLastSkipList, (int)PhM);
    int * pTmp=new int[SVClcCount()+SVValueCount()+1];
    pLst=&pTmp[SVClcCount()];
    for (int i=-SVClcCount(); i<SVValueCount()+1; i++)
      pLst[i]=MaxSpecies+MaxCalculations+MaxAttributes+1;
    int j=-(1+SVClcCount());
    for (int i=-SVClcCount(); i<SVValueCount(); i++)
      {
      if (m_Img[i]->m_iPhMsk & Mask)
        {
        for (int k=1; j+k<=i; k++)
          pLst[j+k]=i-j-k;
        j=i;
        };
      }

    if (Mask==som_ALL)
      m_SkipListFwdAll=pLst;
    else if (Mask==som_SL)
      m_SkipListFwdSL=pLst;
    else if (Mask==som_Sol)
      m_SkipListFwdSol=pLst;
    else if (Mask==som_Liq)
      m_SkipListFwdLiq=pLst;
    else if (Mask==som_Gas)
      m_SkipListFwdGas=pLst;
    else 
      m_SkipListFwdMap.SetAt(Mask, pLst);

#if dbgSysVec
    if (dbgSkipLists())
      {
      dbgpln("SetUpSkipList %08x =============", Mask);
      for (int i=-SVClcCount(); i<SVValueCount(); i++)
        dbgpln("%3i %3i %s", i, pLst[i], m_Img[i]->FullTag());
      dbgpln("------------------------------------");
      SpecieIter I(Mask);
      for (int i=I.ClcInx1(); I.LoopAtt(i); )
        dbgpln("%3i %s", i, m_Img[i]->FullTag());
      dbgpln("====================================");
      }
#endif
    }
  return pLst;
  };

//---------------------------------------------------------------------------

int * CSysVecInfo::SetUpSkipListRev(PhMask Mask)
  {
  dword PhM=Mask;
  int * pLst;
  if (Mask==som_ALL)
    pLst=m_SkipListRevAll;
  else if (Mask==som_SL)
    pLst=m_SkipListRevSL;
  else if (Mask==som_Sol)
    pLst=m_SkipListRevSol;
  else if (Mask==som_Liq)
    pLst=m_SkipListRevLiq;
  else if (Mask==som_Gas)
    pLst=m_SkipListRevGas;
  else if (!m_SkipListRevMap.Lookup(Mask, pLst))
    pLst=NULL;

  if (pLst)//m_SkipListRev[PhM]==NULL)
    {
    //m_iLastSkipList=Max(m_iLastSkipList, (int)PhM);
    int *pTmp=new int[SVClcCount()+SVValueCount()+1];
    pLst=&pTmp[SVClcCount()];
    for (int i=-SVClcCount(); i<SVValueCount()+1; i++)
      pLst[i]=(MaxSpecies+MaxCalculations+MaxAttributes+1);
    int j=SVSpcCount();
    for (i=SVSpcCount()-1; i>=0; i--)
      {
      if (m_Img[i]->m_iPhMsk&Mask)
        {
        for (int k=1; j-k>=i; k++)
          pLst[j-k]=-(i-j+k);
        j=i;
        };
      }

    if (Mask==som_ALL)
      m_SkipListRevAll=pLst;
    else if (Mask==som_SL)
      m_SkipListRevSL=pLst;
    else if (Mask==som_Sol)
      m_SkipListRevSol=pLst;
    else if (Mask==som_Liq)
      m_SkipListRevLiq=pLst;
    else if (Mask==som_Gas)
      m_SkipListRevGas=pLst;
    else 
      m_SkipListRevMap.SetAt(Mask, pLst);

#if dbgSysVec
    if (dbgSkipLists())
      {
      dbgpln("SetUpSkipListRev %08x =============", Mask);
      for (int i=-SVClcCount(); i<SVValueCount(); i++)
        dbgpln("%3i %3i %s", i, pLst[i], m_Img[i]->FullTag());
      dbgpln("------------------------------------");
      SpecieIter I(Mask);
      for (int i=SVSpcCount()-1; I.LoopRevClc(i); )
        dbgpln("%3i %s", i, m_Img[i]->FullTag());
      dbgpln("====================================");
      };
#endif
    }
  return pLst;
  };

//--------------------------------------------------------------------------

CSysVecTemplateItem::CSysVecTemplateItem(LPCTSTR Name)
  {
  m_Name=Name?Name:"";
  m_SaveIt=true;
  m_Values.SetSize(SVValueCount());
  for (int i=0; i<m_Values.GetSize(); i++)
    m_Values[i]=dNAN;
  };
CSysVecTemplateItem::CSysVecTemplateItem(const CSysVecTemplateItem & O)
  {
  m_Name=O.m_Name;
  for (int i=0; i<m_Values.GetSize(); i++)
    m_Values[i]=O.m_Values[i];
  };
CSysVecTemplateItem & CSysVecTemplateItem::operator=(const CSysVecTemplateItem & O)
  {
  m_Name=O.m_Name;
  for (int i=0; i<m_Values.GetSize(); i++)
    m_Values[i]=O.m_Values[i];
  return *this;
  };
CSysVecTemplateItem & CSysVecTemplateItem::operator=(CSysVector & O)
  {
  for (int i=0; i<SVValueCount(); i++)
    m_Values[i]=O.VValue[i];//O.m_Values[i];
  return *this;
  };

//--------------------------------------------------------------------------

CSysVecTemplateItem * CSysVecTemplateList::FindTemplate(LPCTSTR Name, bool AddIt)
  {
  POSITION Pos=SVI.m_Templates.GetHeadPosition();
  CSysVecTemplateItem *pT=NULL;
  while (Pos)
    {
    pT=SVI.m_Templates.GetNext(Pos);
    if (pT->m_Name.CompareNoCase(Name)==0)
      return pT;
    }

  if (AddIt)
    {
    SVI.m_Templates.AddTail(new CSysVecTemplateItem(Name));
    return SVI.m_Templates.GetTail();
    }
  return NULL;
  }

// ==========================================================================
//
//                               Specie Vector
//
// ==========================================================================

#if WITHSPVECTORTIMERS
CGlblStopWatch CSysVector::sm_swZero  (gs_swFlwSolve, "CSysVector", "Zero");
CGlblStopWatch CSysVector::sm_swSet   (gs_swFlwSolve, "CSysVector", "Set");
CGlblStopWatch CSysVector::sm_swSum   (gs_swFlwSolve, "CSysVector", "Sum");
CGlblStopWatch CSysVector::sm_swAdd   (gs_swFlwSolve, "CSysVector", "Add");
CGlblStopWatch CSysVector::sm_swMass  (gs_swFlwSolve, "CSysVector", "Mass");
CGlblStopWatch CSysVector::sm_swNorm  (gs_swFlwSolve, "CSysVector", "Norm");
CGlblStopWatch CSysVector::sm_swDens  (gs_swFlwSolve, "CSysVector", "Dens");
CGlblStopWatch CSysVector::sm_swHeat  (gs_swFlwSolve, "CSysVector", "Heat");
CGlblStopWatch CSysVector::sm_swOther (gs_swFlwSolve, "CSysVector", "Other");
#else
#endif

//--------------------------------------------------------------------------

double CSysVector::ValueOp(int i, byte Op, long Fidelity, double T, double P)
  {
  if (i<0)
    return EvalFn(i, Op, Fidelity, T, P);
  switch (Op)
    {
    case SVOP_Mole: return m_d[i]/SDB[i].MoleWt();
    case SVOP_Vol:  return m_d[i]/GTZ(SDB[i].Density(Fidelity, T, P, NULL, m_d));
    case SVOP_NVol: return m_d[i]/GTZ(SDB[i].NDensity(Fidelity, NULL, m_d));
    default:        return m_d[i];
    }
  };

//--------------------------------------------------------------------------

void CSysVector::SetValueOP(int i, byte Op, double V, long Fidelity, double T, double P)
  {
  if (i>=0)
    {
    switch (Op)
      {
      case SVOP_Mole: m_d[i] = V * SDB[i].MoleWt();                           break;
      case SVOP_Vol:  m_d[i] = V * SDB[i].Density(Fidelity, T, P, NULL, m_d); break;
      case SVOP_NVol: m_d[i] = V * SDB[i].NDensity(Fidelity, NULL, m_d);      break;
      default:        m_d[i] = V;                                             break;
      }
    }
  };

//--------------------------------------------------------------------------

double CSysVector::Mass2OtherFactor(int i, byte Op, long Fidelity, double T, double P, CSVData M)
  {
  if (i<0)
    DoBreak();
  switch (Op)
    {
    case SVOP_Mole: return 1.0/SDB[i].MoleWt();
    case SVOP_Vol:  return 1.0/SDB[i].Density(Fidelity, T, P, NULL, M);
    case SVOP_NVol: return 1.0/SDB[i].NDensity(Fidelity, NULL, M);
    default:        return 1.0;
    }
  };

//--------------------------------------------------------------------------

double CSysVector::EvalFn(int SVIndex)
  {
  CSysVecItem & Itm=SVImg(SVIndex);
  if (Itm.FnIsSpcsSum())
    {
    double d=0;
    CIArray &A=Itm.FnIndicesOfSumSpcs();
    int n=A.GetSize();
    for (int i=0; i<n; i++)
      d+=m_d[A[i]];
    return d;
    }
  CSysVecFnCtx C(*this);
  return SVImg(SVIndex).m_Fn.Evaluate(C);
  };

//--------------------------------------------------------------------------

double CSysVector::EvalFn(int SVIndex, byte Op, long Fidelity, double T, double P)
  {
  CSysVecFnCtx C(*this, Op, Fidelity, T, P);
  return SVImg(SVIndex).m_Fn.Evaluate(C);
  };

//--------------------------------------------------------------------------


void CSysVector::SclVMass(int SVIndex, double Scale)
  {
  CSysVecItem & Itm = SVImg(SVIndex);
  int n=Itm.ChildCount();
  for (int i=0; i<n; i++)
    {
    int Ai=Itm.m_ChildrenSVIndex[i];
    CSysVecItem & AItm = SVImg(Ai);
    ASSERT(AItm.Type()==SVT_Attribute);
    switch (AItm.AttrType())
      {
      case SVAT_Qual:                                                                   break;
      case SVAT_Amnt: m_d[Ai]*=Scale;                                                   break;
      case SVAT_Calc: m_d[Ai]=Itm.m_Scl.Evaluate(CSysVecFnCtx(*this, *this, Scale));     break;
      }
    }
  m_d[SVIndex]*=Scale;
  };

//--------------------------------------------------------------------------

void CSysVector::SetVMass(int SVIndex, CSysVector &Vec2, double Value)
  {
  ASSERT(SVIndex>=0 && SVIndex<SVSpcCount());

  CSysVecItem & Itm = SVImg(SVIndex);
  int n=Itm.ChildCount();
  ASSERT(Itm.Type()==SVT_Specie);

  for (int i=0; i<n; i++)
    {
    int Ai=Itm.m_ChildrenSVIndex[i];
    CSysVecItem & AItm = SVImg(Ai);
    ASSERT(AItm.Type()==SVT_Attribute);
    switch (AItm.AttrType())
      {
      case SVAT_Qual: m_d[Ai]=Vec2.m_d[Ai];                                                                 break;
      case SVAT_Amnt: m_d[Ai]=Vec2.m_d[Ai]*Value/GTZ(Vec2.m_d[SVIndex]);                                    break;
      case SVAT_Calc: m_d[Ai]=Itm.m_Scl.Evaluate(CSysVecFnCtx(*this, Vec2, Value/GTZ(Vec2.m_d[SVIndex])));  break;
      }
    }
  m_d[SVIndex]=Value;
  };

//--------------------------------------------------------------------------

void CSysVector::AddVMass(int SVIndex, CSysVector &Vec2, double Value)
  {
  ASSERT(SVIndex>=0 && SVIndex<SVSpcCount());

  CSysVecItem & Itm = SVImg(SVIndex);
  int n=Itm.ChildCount();
  for (int i=0; i<n; i++)
    {
    int Ai=Itm.m_ChildrenSVIndex[i];
    CSysVecItem & AItm = SVImg(Ai);
    ASSERT(AItm.Type()==SVT_Attribute);
    switch (AItm.AttrType())
      {
      case SVAT_Qual:
        {
        double m1=m_d[Ai];
        double m2=Value;
        double Frac2=m2/GTZ(m1+m2);
        m_d[Ai]=(1-Frac2)*m_d[Ai]+Frac2*Vec2.m_d[Ai];
        break;
        }
      case SVAT_Amnt:
        m_d[Ai]+=Vec2.m_d[Ai]*Value/GTZ(Vec2.m_d[SVIndex]);
        break;
      case SVAT_Calc:
        m_d[Ai]=Itm.m_Add.Evaluate(CSysVecFnCtx(*this, Vec2, Value/GTZ(Vec2.m_d[SVIndex])));
        break;
      }
    }
  m_d[SVIndex]+=Value;

  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::operator=(CSysVector &Vec2)
  {
  DMPSYSVEC(1, "=Vec", Vec2);
  SPMVECTORTIMERLAP(sm_swOther);

  for (int i=0; i < SVValueCount(); i++)
    m_d[i]=Vec2.m_d[i];

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  }

//--------------------------------------------------------------------------

CSysVector &CSysVector::operator+=(CSysVector &Vec2)
  {
  DMPSYSVEC(1, "+=Vec", *this);
  DMPSYSVEC(0, "  +", Vec2);
  SPMVECTORTIMERLAP(sm_swOther);

  for (int i=SVAtt1(); i<SVAttN(); i++)
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual:
        {
        long iParent=Itm.m_iSVParent;
        double m1=VValue[iParent];
        double m2=Vec2.VValue[iParent];
        double Frac2=m2/GTZ(m1+m2);
        m_d[i]=(1-Frac2)*m_d[i]+Frac2*Vec2.m_d[i];
        break;
        }
      case SVAT_Amnt:
        m_d[i]+=Vec2.m_d[i];
        break;
      case SVAT_Calc:
        m_d[i]=Itm.m_Add.Evaluate(CSysVecFnCtx(*this, Vec2, 1));
        break;
      }
    }

  for (int i=SVSpc1(); i<SVSpcN(); i++)
    m_d[i]+=Vec2.m_d[i];

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  }

//--------------------------------------------------------------------------

CSysVector &CSysVector::operator-=(CSysVector &Vec2)
  {
  DMPSYSVEC(1, "-=Vec", *this);
  DMPSYSVEC(0, "  -", Vec2);
  SPMVECTORTIMERLAP(sm_swOther);

  for (int i=SVAtt1(); i<SVAttN(); i++)
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual:                                                           break;
      case SVAT_Amnt: m_d[i]=GEZ(m_d[i]-Vec2.m_d[i]);                           break;
      case SVAT_Calc: m_d[i]=Itm.m_Sub.Evaluate(CSysVecFnCtx(*this, Vec2, 1));  break;
      }
    }

  for (int i=SVSpc1(); i<SVSpcN(); i++)
    m_d[i]=GEZ(m_d[i]-Vec2.m_d[i]);

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  }

//--------------------------------------------------------------------------

CSysVector &CSysVector::Set(CSysVector &Vec2)
  {
  DMPSYSVEC(1, "Set.Vec1", Vec2);
  SPMVECTORTIMERLAP(sm_swSet);

  for (int i=0; i < SVValueCount(); i++)
    m_d[i]=Vec2.m_d[i];

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::Set(CSysVector &Vec2, PhMask Phase)
  {
  DMPSYSVEC(1, "Set.Vec2", Vec2);
  Zero();
  SPMVECTORTIMERLAP(sm_swSet);

  SpecieIter I(Phase);
  for (int i=I.SpcInx1(); I.LoopAtt(i); )
    m_d[i]=Vec2.m_d[i];

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::Set(CSysVector &Vec2, PhMask Phase, double Scl, double LoLimit)
  {
  DMPSYSVEC(1, "Set.Vec3", Vec2);
  ASSERT(Scl>=0.0);
  Zero();
  SPMVECTORTIMERLAP(sm_swSet);

  SpecieIter I(Phase);
  for (int i=I.AttInx1()  ; I.LoopAtt(i);  )
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual: m_d[i]=Vec2.m_d[i]; break;
      case SVAT_Amnt: m_d[i]=Vec2.m_d[i]*Scl; break;
      case SVAT_Calc: m_d[i]=Itm.m_Scl.Evaluate(CSysVecFnCtx(*this, Vec2, Scl)); break;
      }
    }
  if (LoLimit)
    {
    for (int i=-1; I.Loop(i); )
      {
      if (Vec2.m_d[i]>=LoLimit)
        m_d[i]=Vec2.m_d[i]*Scl;
      else
        m_d[i]=0.0;
      }
    }
  else
    {
    for (int i=-1; I.Loop(i); )
      m_d[i]=Vec2.m_d[i]*Scl;
    }

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::Set(CSysVector &Vec2, PhMask Phase, double Scl, CSVData SclA)
  {
  DMPSYSVEC(1, "Set.Vec4", Vec2);
  ASSERT(Scl>=0.0);
  Zero();
  SPMVECTORTIMERLAP(sm_swSet);

  SpecieIter I(Phase);
  for (int i=I.AttInx1()  ; I.LoopAtt(i);  )
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual: m_d[i]=Vec2.m_d[i]; break;
      case SVAT_Amnt: m_d[i]=Vec2.m_d[i]*Scl*SclA[i]; break;
      case SVAT_Calc: m_d[i]=Itm.m_Scl.Evaluate(CSysVecFnCtx(*this, Vec2, Scl*SclA[i])); break;
      }
    }

  for (int i=-1; I.Loop(i); )
    m_d[i]=Vec2.m_d[i]*Scl*SclA[i];

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::Set(CSysVector &Vec2, double SScl, double LScl, double VScl)
  {
  DMPSYSVEC(1, "Set.Vec5", Vec2);
  SPMVECTORTIMERLAP(sm_swSet);
  ASSERT(SScl>=0.0);
  ASSERT(LScl>=0.0);
  ASSERT(VScl>=0.0);

  Set(Vec2, som_Sol, SScl);
  Add(Vec2, som_Liq, LScl);
  Add(Vec2, som_Gas, VScl);

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  }

//--------------------------------------------------------------------------

CSysVector &CSysVector::Add(CSysVector &Vec2, PhMask Phase)
  {
  DMPSYSVEC(1, "Add.Vec1", *this);
  DMPSYSVEC(0, "  +", Vec2);
  SPMVECTORTIMERLAP(sm_swAdd);

  SpecieIter I(Phase);
  for (int i=I.AttInx1()  ; I.LoopAtt(i);  )
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual:
        {
        long iParent=Itm.m_iSVParent;
        double m1=VValue[iParent];
        double m2=Vec2.VValue[iParent];
        double Frac2=m2/GTZ(m1+m2);
        m_d[i]=(1-Frac2)*m_d[i]+Frac2*Vec2.m_d[i];
        break;
        }
      case SVAT_Amnt:
        m_d[i]+=Vec2.m_d[i];
        break;
      case SVAT_Calc:
        m_d[i]=Itm.m_Add.Evaluate(CSysVecFnCtx(*this, Vec2, 1));
        break;
      }
    }

  for (int i=-1; I.Loop(i); )
    m_d[i]+=Vec2.m_d[i];

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::Add(CSysVector &Vec2, PhMask Phase, double Scl)
  {
  DMPSYSVEC(1, "Add.Vec2", *this);
  DMPSYSVEC(0, "  +", Vec2);
  SPMVECTORTIMERLAP(sm_swAdd);

  Scl=GEZ(Scl);
  SpecieIter I(Phase);
  for (int i=I.AttInx1()  ; I.LoopAtt(i);  )
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual:
        {
        long iParent=Itm.m_iSVParent;
        double m1=VValue[iParent];
        double m2=Scl*Vec2.VValue[iParent];
        double Frac2=m2/GTZ(m1+m2);
        m_d[i]=(1-Frac2)*m_d[i]+Frac2*Vec2.m_d[i];
        break;
        }
      case SVAT_Amnt:
        m_d[i]+=Scl*Vec2.m_d[i];
        break;
      case SVAT_Calc:
        m_d[i]=Itm.m_Add.Evaluate(CSysVecFnCtx(*this, Vec2, Scl));
        break;
      }
    }

  for (int i=-1; I.Loop(i); )
    m_d[i]+=Vec2.m_d[i]*Scl;

  ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  };

//--------------------------------------------------------------------------

CSysVector &CSysVector::Add(CSysVector &Vec2, double SScl, double LScl, double VScl)
  {
  DMPSYSVEC(1, "Add.Vec3", *this);
  DMPSYSVEC(0, "  +", Vec2);

  Add(Vec2, som_Sol, SScl);
  Add(Vec2, som_Liq, LScl);
  Add(Vec2, som_Gas, VScl);

  DMPSYSVEC(0, "  =", *this);
  return *this;
  }

//--------------------------------------------------------------------------

CSysVector &CSysVector::AddDeriv(int Sgn, double dTime, CSysVector &Flw, CSysVector &M, double Advance)
  {
  DMPSYSVEC(1, "AddDer.Vec", *this);
  DMPSYSVEC(0, "Flw", Flw);
  DMPSYSVEC(0, "M", M);
  SPMVECTORTIMERLAP(sm_swOther);

  for (int i=SVAtt1(); i<SVAttN(); i++)
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual:
        if (Sgn>0)
          {
          long iParent=Itm.m_iSVParent;
          double q1=M.VValue[iParent];
          double q2=dTime*Flw.VValue[iParent];
          double Frac2=q2/GTZ(q1+q2);
          m_d[i]+=(((1-Frac2)*M.m_d[i]+Frac2*Flw.m_d[i])-M.m_d[i])/dTime;
          }
        break;
      case SVAT_Amnt: m_d[i]+=Sgn*Flw.m_d[i];                                               break;
      case SVAT_Calc: m_d[i]=Itm.m_AddDeriv.Evaluate(CSysVecFnCtx(Sgn, dTime, Flw, M, Advance));  break;
      }
    }

  for (int i=SVSpc1(); i<SVSpcN(); i++)
    m_d[i]+=Sgn*Flw.m_d[i];

  //ASSERT_MASS_OK(this);
  DMPSYSVEC(0, "  =", *this);
  return *this;
  }

//--------------------------------------------------------------------------

void CSysVector::Mult(PhMask Phase, double Scl)
  {
  DMPSYSVEC(1, "Mult1", *this);
  SPMVECTORTIMERLAP(sm_swOther);

  SpecieIter I(Phase);
  for (int i=I.AttInx1()  ; I.LoopAtt(i);  )
    {
    CSysVecItem & Itm = SVImg(i);
    switch (Itm.AttrType())
      {
      case SVAT_Qual:                                                             break;
      case SVAT_Amnt: m_d[i]*=Scl;                                                break;
      case SVAT_Calc: m_d[i]=Itm.m_Scl.Evaluate(CSysVecFnCtx(*this, *this, Scl)); break;
      }
    }

  for (int i=-1; I.Loop(i); )
    m_d[i]*=Scl;

  DMPSYSVEC(0, "  =", *this);
  ASSERT_MASS_OK(this);
  };

//--------------------------------------------------------------------------

void CSysVector::Mult(CIArray & SpIds, double Scl)
  {
  DMPSYSVEC(1, "Mult2", *this);
  SPMVECTORTIMERLAP(sm_swOther);

  for (int j=0,i=0; j<SpIds.GetSize() && (i=SpIds[j])>=0; j++)
    {
    CSysVecItem & Itm = SVImg(i);
    ASSERT(Itm.Type()==SVT_Specie);
    SclVMass(i, Scl);
    }

  DMPSYSVEC(0, "  =", *this);
  ASSERT_MASS_OK(this);
  };

//--------------------------------------------------------------------------

void CSysVector::Mult(CSysVector & Scale, double Scl)
  {
  DMPSYSVEC(1, "Mult2", *this);
  SPMVECTORTIMERLAP(sm_swOther);

  for (int i=0; i<SVSpcCount(); i++)
    {
    CSysVecItem & Itm = SVImg(i);
    ASSERT(Itm.Type()==SVT_Specie);
    SclVMass(i, Scale[i]*Scl);
    }

  DMPSYSVEC(0, "  =", *this);
  ASSERT_MASS_OK(this);
  };

//--------------------------------------------------------------------------

flag CSysVector::AddValid(CSysVector &Vec2)
  {
  SPMVECTORTIMERLAP(sm_swAdd);
  DoBreak();
  flag HasValid=0;
  for (int i=0; i < SVSpcCount(); i++)
    if (Valid(Vec2.m_d[i]))
      {
      m_d[i]=Valid(m_d[i]) ? m_d[i]+Vec2.m_d[i] : Vec2.m_d[i];
      HasValid=1;
      }
    return HasValid;
  }

//--------------------------------------------------------------------------

void CSysVector::Zero()
  {
  //CGlblStopWatch *pxx=&CSysVector::sm_swZero;
  //CStopWatchLap XX(CSysVector::sm_swZero);
  SPMVECTORTIMERLAP(sm_swZero);
  memset(&m_d[0], 0, sizeof(m_d[0])*SVValueCount());
  };

//--------------------------------------------------------------------------

void CSysVector::ZeroPhase(PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swZero);
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    m_d[i]=0.0;
  };

//--------------------------------------------------------------------------

double CSysVector::Normalise()
  {
  SPMVECTORTIMERLAP(sm_swNorm);
  double n=0.0;
  for (int i=0; i < SVSpcCount(); i++) n+=m_d[i];
  if (n > 1.0e-30)
    for (int i=0; i < SVSpcCount(); i++) m_d[i]/=n;
  else
    for (int i=0; i < SVSpcCount(); i++) m_d[i]=0.0;
  return n;
  };

//--------------------------------------------------------------------------

void CSysVector::SetTo(double Value)
  {
  SPMVECTORTIMERLAP(sm_swOther);
  for (int i=0; i < SVSpcCount(); i++)
    m_d[i]=Value;
  };

//--------------------------------------------------------------------------

void CSysVector::SetToOnesAndZeros(flag Invert, double Tol)
  {
  SPMVECTORTIMERLAP(sm_swOther);
  double V0=Invert ? 1.0 : 0.0;
  double V1=Invert ? 0.0 : 1.0;
  for (int i=0; i < SVSpcCount(); i++)
    m_d[i]=(fabs(m_d[i])<=Tol) ? V0 : V1;
  };

//-------------------------------------------------------------------------- {

void CSysVector::SetToValue(PhMask Phase, double Value)
  {
  SpecieIter I(Phase);
  for (int s=-1; I.Loop(s); )
    m_d[s]=Value;
  }

//--------------------------------------------------------------------------

void CSysVector::SetToValue(int iSpId, double Value)
  {
  m_d[iSpId]=Value;
  }

//--------------------------------------------------------------------------

void CSysVector::ToMass(CSysVector &MA)
  {
  for (int i=0; i < SVSpcCount(); i++)
    m_d[i]=MA[i]*SDB[i].MoleWt();
  }

//--------------------------------------------------------------------------

void CSysVector::ToMoles(CSysVector &MA)
  {
  for (int i=0; i < SVSpcCount(); i++)
    m_d[i]=MA[i]/SDB[i].MoleWt();
  }

//--------------------------------------------------------------------------

double CSysVector::Sum(PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swSum);
  double s=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    s+=m_d[i];
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Mass()
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    s+=m_d[i];
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Mass(PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    s+=m_d[i];
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Mass(CIArray & SpIds)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  int i=0;
  for (int j=0; j<SpIds.GetSize() && (i=SpIds[j])>=0; j++)
    s+=m_d[i];
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Mass(CSysVector & Scale)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    s+=Scale[i]*m_d[i];
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Moles(PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  SpecieIter I(Phase);
  for (int i=-1; I.Loop(i); )
    s+=m_d[i]/SDB[i].MoleWt();
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Moles(CIArray & SpIds)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  int i=0;
  for (int j=0; j<SpIds.GetSize() && (i=SpIds[j])>=0; j++)
    s+=m_d[i]/SDB[i].MoleWt();
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::Moles(CSysVector & Scale)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    s+=Scale[i]*m_d[i]/SDB[i].MoleWt();
  return s;
  };

//--------------------------------------------------------------------------

double CSysVector::MassFrac(PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0,t=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    {
    double m=m_d[i];
    t+=m;
    if (SDB[i].PhaseIs(Phase))
      s+=m;
    }
  return s/GTZ(t);
  };

//--------------------------------------------------------------------------

double CSysVector::MoleFrac(PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double s=0.0,t=0.0;
  for (int i=0; i < SVSpcCount(); i++)
    {
    double m=m_d[i]/SDB[i].MoleWt();
    t+=m;
    if (SDB[i].PhaseIs(Phase))
      s+=m;
    }
  return s/GTZ(t);
  };


//--------------------------------------------------------------------------

double CSysVector::ElementMass(CIArray & ElementIds, PhMask Phase)
  {
  SPMVECTORTIMERLAP(sm_swMass);
  double tot=0.0;
  for (int ie=0; ie<ElementIds.GetCount(); ie++)
    {
    CompFracArray &CFH=*(EDB[ie].m_SpComp);
    int Ns=CFH.GetSize();
    for (int is=0; is<Ns; is++)
      {
      int s=CFH[is].iSpNo;
      if (SDB[s].PhaseIs(Phase))
        tot+=m_d[s]*CFH[is].dMassFrac;
      }
    }
  return tot;
  };

// ===========================================================================

static long HdCnt=0;
void DumpSysVec(bool Start, LPSTR Hd, CSysVector & Vec)
  {
#if dbgSysVec
  if (dbgDumpVectors())
    {
    if (Start)
      {
      dbgpln("");
      if (1 || (HdCnt==0))
        {
        dbgp("%10s", "");
        for (int i=0; i<SVValueCount(); i++)
          dbgp(" %10.10s", SVImg(i).Tag());
        dbgpln("");
        }
      HdCnt=(HdCnt+1)%10;
      }
    dbgp(Start?"%-10s":"%10s", Hd);
    for (int i=0; i<SVValueCount(); i++)
      if (!Finite(Vec[i]))
        dbgp(" %10s", "***BAD***");
      else if (fabs(Vec[i])>1e6)
        dbgp(" %10.2e", Vec[i]);
      else if (Vec[i]==0.0)
        dbgp(" %10s", "0");
      else
        dbgp(" %10.2f", Vec[i]);
    dbgpln("");
    }
#endif
  }

// ===========================================================================
//
//
//
// ==========================================================================
