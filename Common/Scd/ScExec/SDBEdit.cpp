// SDBEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "scdlib.h"
#define __SDBEDIT_CPP
//#include "scdlib.h"
#include "sp_db.h"
#include "SDBEdit.h"
#include "gpfuncs.h"
#include "dbhelper.h"
#include "scdctrls.h"
#include "errorlog.h"
#include "datacnvs.h"
#include "scdarray.h"
#include "ssactivecell.h"
#include "sscolumn.h"
#include "sscolumns.h"
#include "ssgroup.h"
#include "ssgroups.h"
#include "ssstyleset.h"
#include "ssstylesets.h"
#include "ssselbookmarks.h"
#include "nrecipe.h"
//#include "vcdatagrid.h"
//#include "adodc.h"
//#include "_recordset.h"
#include "resource.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define dbgMakeSpDefn1 0
#define dbgMakeSpDefn2 0
#define dbgMakeSpDefn3 0
#define dbgDumpAll     01
#define dbgDumpIns     01

extern AFX_EXTENSION_MODULE SCExecDLL;

//===========================================================================

struct MyFldInfo;
struct HSCInfo;
struct SDBInfo;

// ==========================================================================
//
//
//
// ==========================================================================

static void DumpAmt(char * Msg, CDArray & Amt, CCElementDataBase & EDB)
  {
  dbgp("                 %-4.4s",Msg);
  for (int e=0; e<MaxElements; e++)
    if (Amt[e]>0.0)
      dbgp("%-3s=%6.2f  ",EDB[e].Name, Amt[e]);
  dbgpln("");
  }

static flag MakeSpDefn1(Strng & Formula, Strng &Defn, int &Pos, CDArray &Amt, int Lvl, CCElementDataBase & EDB)
  {
  flag OK=true;
  double Mult=1.0;
  while (Pos<Formula.Length())
    {
    if (isupper(Formula[Pos]))
      {
      Strng El;
      El=Formula[Pos++];
      while (islower(Formula[Pos]))
        El+=Formula[Pos++];
      double Num=Formula[Pos]!='*' ? SafeAtoF(&Formula[Pos], 1.0) : 1.0;
      while (isdigit(Formula[Pos]) || Formula[Pos]=='.') Pos++;
      int iEl=EDB.Find(El());
      if (iEl>=0)
        Amt[iEl]+=Num;
      else
        {
        LogNote("MkSpDefn", 0, "'%s' unknown in %s", El(), Formula());
        OK=false;
        }
      #if dbgMakeSpDefn1
      dbgpln("  El:%s - %6.2f",El(), Num);
      #endif
      //DumpAmt("Amt);
      }
    else if (isdigit(Formula[Pos]))
      {
      Mult=Formula[Pos]!='*' ? SafeAtoF(&Formula[Pos], 1.0) : 1.0;
      while (isdigit(Formula[Pos]) || Formula[Pos]=='.') Pos++;
      #if dbgMakeSpDefn1
      dbgpln("  *:%6.2f",Mult);
      #endif
      }
    else if (Formula[Pos]=='(' || Formula[Pos]=='[')
      {
      #if dbgMakeSpDefn1
      dbgpln("  %c",Formula[Pos]);
      #endif
      Pos++;
      CDArray Amt1;
      Amt1.SetSize(MaxElements);
      MakeSpDefn1(Formula, Defn, Pos, Amt1, Lvl+1, EDB);
      //DumpAmt("+", Amt1);
      Amt+=Amt1;
      #if dbgMakeSpDefn1
      DumpAmt("=", Amt);
      #endif
      }
    else if (Formula[Pos]==')' || Formula[Pos]==']')
      {
      char Ch=Formula[Pos];
      Pos++;
      // Get Multiplier & drop out
      double Mult=Formula[Pos]!='*' ? SafeAtoF(&Formula[Pos], 1.0) : 1.0;
      while (isdigit(Formula[Pos]) || Formula[Pos]=='.') Pos++;
      Amt*=Mult;
      #if dbgMakeSpDefn1
      dbgpln("  %c:%6.2f",Ch,Mult);
      DumpAmt("", Amt);
      #endif
      break;
//      MakeSpDefn1(Formula, Defn, Pos, Amt, Lvl+1);
      }
    else if (Formula[Pos]=='*')
      {
      if (Lvl==0)
        {
        Pos++;
        //double Mult=Formula[Pos]!='*' ? SafeAtoF(&Formula[Pos], 1.0) : 1.0;
        //while (isdigit(Formula[Pos]) || Formula[Pos]=='.') Pos++;
        #if dbgMakeSpDefn1
        //dbgpln("  ******* :%6.2f",Mult);
        dbgpln("  *******");
        #endif
        CDArray Amt1;
        Amt1.SetSize(MaxElements);
        MakeSpDefn1(Formula, Defn, Pos, Amt1, Lvl+1, EDB);
        //Amt1*=Mult;
        Amt+=Amt1;
        #if dbgMakeSpDefn1
        DumpAmt("=",Amt1);
        #endif
        }
      else
        {
        #if dbgMakeSpDefn1
        dbgpln("  *******");
        #endif
        // Drop Out
        break;
        }
      }
    else
      {
      OK=false;
      LogNote("MkSpDefn", 0, "'%c' unknown in %s", Formula[Pos], Formula());
      Pos++;
      }
    }
  //DumpAmt(Amt);
  Amt*=Mult;
  return OK;
  }

// --------------------------------------------------------------------------

static flag MakeSpDefn(Strng & Formula, Strng &Defn, CCElementDataBase & EDB)
  {
  Strng A;
  CDArray Amt;
  Amt.SetSize(MaxElements);
  int Pos=0;
  flag OK=MakeSpDefn1(Formula, Defn, Pos, Amt, 0, EDB);
  Defn="";
  for (int e=0; e<MaxElements; e++)
    if (Amt[e]>0.0)
      {
      Defn+=EDB[e].Name;
      A.Set("%.3f",Amt[e]);
      int i=A.Len()-1;
      while (A[i]=='0' && i>0) i--;
      if (A[i]=='.' && i>0) i--;
      A.SetLength(i+1);
      Defn+=A;
      }

  #if dbgMakeSpDefn1
  DumpAmt("====",Amt);
  #endif
  return OK;
  }

// ==========================================================================
//
//
//
// ==========================================================================

#pragma pack(push, 1)
struct HSCData
  {
  char   Formula[24];
  char   StructuralFormula[32];
  char   Name[52];
  char   CommonName[32];
  char   ChemAbstractNumber[12];
  float  MW;
  float  MeltingPoint;
  float  BoilingPoint;
  float  T1;
  float  T2;
  char   Phase[2];
  float  H;
  float  S;
  float  A;
  float  B;
  float  C;
  float  D;
  float  Density;
  float  ColourRGB;
  float  Solubility;
  float  Pad1;
  float  Pad2;
  char   Reference[36];
  short  ReliabilityClass;
  };
#pragma pack(pop)

struct HSCInfo
  {
  Strng   FormulaRaw;
  Strng   Formula;
  Strng   StructuralFormula;
  Strng   Name;
  Strng   CommonName;
  Strng   ChemAbstractNumber;
  Strng   Phase;
  Strng   Defn;
  double  MW;
  double  MeltingPoint;
  double  BoilingPoint;
  double  m_T1;
  double  m_T2;
  double  H;
  double  S;
  double  A;
  double  B;
  double  C;
  double  D;
  double  Density;
  double  ColourRGB;
  double  Solubility;
  double  Pad1;
  double  Pad2;
  Strng   Reference;
  short   ReliabilityClass;

  flag    IsValid;
  Strng   Occurence;
  flag    m_FormatA;
  flag    m_FormatAOK;
  flag    m_Selected;
  int     m_iPhase;
  int     m_iPhaseInd;
  int     m_iRecNo;
  int     m_iInfoIndex;
  double  m_Hx;


  HSCInfo();
  double IntegrateCp(double T1, double T2);
  void CopyData(HSCData &Data, flag HIsForm);
  void UpdateA(int iRecNo, int & iPhase, Strng & sPrevFormula, HSCInfo *pPrevInfo);
  void UpdateB(CCElementDataBase & EDB, CArray<HSCInfo*,HSCInfo*> & Info, int iInfo);
  flag Valid() { return IsValid; };
  };

// --------------------------------------------------------------------------

HSCInfo::HSCInfo()
  {
  IsValid=false;
  m_FormatA=false;
  m_FormatAOK=false;
  m_Selected=false;
  m_iPhase=-1;
  m_iRecNo=-1;
  m_iInfoIndex=-1;
  }

// --------------------------------------------------------------------------

inline void Copy(Strng &Str, char * Data, int Len)
  {
  char Buff[1024];
  strncpy(Buff, Data, Len);
  Buff[Len]=0;
  Str=Buff;
  Str.LRTrim();
  };

// --------------------------------------------------------------------------

#define MLCP(T, P) (a + (b * T) + (c / GTZ(T*T)) + (d * T*T)) 

inline double HSCInfo::IntegrateCp(double T1, double T2)
  {
  double a=A;
  double b=B*1.0e-3;
  double c=C*1.0e+5;
  double d=D*1.0e-6;

  int Sgn=T1>T2 ? -1 : 1; 
  if (Sgn < 0)
    Exchange(T2, T1);

  double HCp=0.0;
  if (T2<m_T1)
    HCp = Sgn*(T2-T1)*MLCP(m_T1, P);
  else if (T1>=m_T2)
    HCp = Sgn*(T2-T1)*MLCP(m_T2, P);
  else
    {
    // Part Of Curve gets used
    if (T2>m_T2)
      {
      HCp = HCp + Sgn*(T2-m_T2)*MLCP(m_T2, P);
      T2=m_T2;
      }
    if (T1<m_T1)
      {
      HCp = HCp + Sgn*(m_T1-T1)*MLCP(m_T1, P);
      T1=m_T1;
      }

    HCp += Sgn*(a * (T2 - T1) + 
      ((b / 2.0) * (Sqr(T2) - Sqr(T1))) - 
      (c * (1.0 / T2 - 1.0 / T1)) + 
      ((d / 3.0) * (Pow(T2,3) - Pow(T1,3))));
    }

  return HCp;
  }

#undef MLCP

void HSCInfo::CopyData(HSCData &Data, flag HIsForm)
  {
  Copy(FormulaRaw, Data.Formula, sizeof(Data.Formula));
  Copy(Formula, Data.Formula, sizeof(Data.Formula));
  Copy(StructuralFormula, Data.StructuralFormula, sizeof(Data.StructuralFormula));
  Copy(Name, Data.Name, sizeof(Data.Name));
  Copy(CommonName, Data.CommonName, sizeof(Data.CommonName));
  Copy(ChemAbstractNumber, Data.ChemAbstractNumber, sizeof(Data.ChemAbstractNumber));
  if (fabs(Data.T1-298.15)<0.001)
    Data.T1 = 298.16F; //25dC in SysCAD is 298.16 !
  if (fabs(Data.T2-298.15)<0.001)
    Data.T2 = 298.16F; //25dC in SysCAD is 298.16 !
  MW                   = Data.MW;
  MeltingPoint         = Data.MeltingPoint;
  BoilingPoint         = Data.BoilingPoint;
  m_T1                 = Data.T1;
  m_T2                 = Data.T2;
  Copy(Phase, Data.Phase, sizeof(Data.Phase));

  H                    = Data.H*4.184*1000.0;
  S                    = Data.S*4.184;
  A                    = Data.A*4.184;
  B                    = Data.B*4.184;
  C                    = Data.C*4.184;
  D                    = Data.D*4.184;
  Density              = Data.Density*(Phase.XStrICmp("g")==0 ? 1.0 : 1000.0);
  ColourRGB            = Data.ColourRGB;
  Solubility           = Data.Solubility;
  Pad1                 = Data.Pad1;
  Pad2                 = Data.Pad2;
  Copy(Reference, Data.Reference, sizeof(Data.Reference));
  ReliabilityClass     = Data.ReliabilityClass;

  m_FormatA            = !FormulaRaw.CmpLastChar(')');
  m_FormatAOK          = m_FormatA;
  m_Hx                 = H;

  };

// --------------------------------------------------------------------------

void HSCInfo::UpdateA(int iRecNo, int & iPhase, Strng & sPrevFormula, HSCInfo *pPrevInfo)
  {
  m_iRecNo=iRecNo;
  if (Formula[0]=='e')
    IsValid=false;  // Electron
  else
    {
    if (Formula[0]>' ')
      {
      // Check Formula Closed
      int nParen=0;
      int nBrack=0;
      for (int i=0;i<Formula.Length(); i++)
        if (Formula[i]=='(')
          nParen++;
        else if (Formula[i]==')')
          nParen--;
        else if (Formula[i]=='[')
          nBrack++;
        else if (Formula[i]==']')
          nBrack--;

        IsValid=(nParen==0 && nBrack==0);
      }
    else
      IsValid=false;

    if (!IsValid)
      LogNote("HSC Import", 0, "Bad Formula %s", Formula());
    else if (Phase())
      {
      if (Phase.XStrCmp("s")==0)
        Occurence="s";
      else if (Phase.XStrCmp("l")==0 ||
        Phase.XStrCmp("a")==0 ||
        Phase.XStrCmp("ai")==0 ||
        Phase.XStrCmp("ao")==0 ||
        Phase.XStrCmp("aq")==0)
        Occurence="l";
      else if (Phase.XStrCmp("g")==0)
        {
        Occurence="g";
        m_FormatAOK=false;
        }
      else
        {
        m_FormatAOK=false;
        IsValid=false;
        LogNote("HSC Import", 0, "Bad Phase '%s' in %s", Phase(), Formula());
        }

      if (Formula=="AgBr")
        { int xxx=0; }
      if (Formula=="AgBrO3")
        { int xxx=0; }

      if (pPrevInfo && pPrevInfo->Formula!=Formula && pPrevInfo->m_iPhase==0)
        pPrevInfo->m_iPhase=-1;

      if (sPrevFormula!=Formula)
        iPhase=0;

      if (m_FormatAOK && m_FormatA)
        m_iPhase=iPhase++;

      sPrevFormula=Formula;
      }
    else
      {
      LogNote("HSC Import", 0, "No Phase in %s", Formula());
      IsValid=false;
      }
    }
  }

// --------------------------------------------------------------------------

void HSCInfo::UpdateB(CCElementDataBase & EDB, CArray<HSCInfo*,HSCInfo*> & Info, int iInfo)//, int & iPhase, Strng & sPrevFormula)
  {

  if (IsValid)
    {
    if (m_iPhase<0)
      m_FormatAOK=false;

    if (m_FormatAOK && m_iPhaseInd>=0)
      Phase.Set("%c%s", 'A'+m_iPhaseInd, Phase());
    //Phase.Set("%c", 'A'+m_iPhaseInd);

    if (Formula[Formula.Length()-1] != ')')
      {
      Formula+="(";
      Formula+=Phase();
      Formula+=")";
      }

    Strng T;
    T=Formula.Left(Formula.Length()-1); //trim right
    for (int i=T.Length()-1; i>=0; i--)
      if (T[i]=='(')
        break;
    Phase=T.Right(T.Length()-i-1);
    Formula=T.Left(i);
    Formula.LRTrim();

    if (IsValid)
      IsValid=MakeSpDefn(Formula, Defn, EDB);

    if (IsValid)
      {

      double dMoleWt=0.0;
      Strng Elements, Element;
      GetElements(Defn, Elements);
      while (Elements.Token(Element, " ,", 0, " ,"))
        {
        int e=EDB.Find(Element());
        if (e>=0)
          {
          double dMoles=GetElemAmt(Defn, Element);
          dMoleWt+=EDB[e].AtmWt*dMoles;
          }
        else
          LogNote("Import HSC", 0, "Element '%s' not found", Element());
        }
      if (fabs(MW-dMoleWt)>0.001*dMoleWt)
        LogWarning("Import HSC", 0, "MoleWt Difference %.3f <> %.3f in %s(%s)", dMoleWt, MW, Formula(), Phase());

      if (m_FormatAOK && m_iPhase>0)
        {
        int iInfo0=iInfo-1;
        while ((iInfo0>0) && (Info[iInfo0]->m_iPhase>0))
          iInfo0--;

        double Ht=Info[iInfo0]->H;
        for (int i=iInfo0+1; i<=iInfo; i++)
          {
          HSCInfo & I0=*Info[i-1];
          HSCInfo & I1=*Info[i];
          double Htemp = Ht + I0.IntegrateCp(I0.m_T1, I0.m_T2) + I1.m_Hx;
          dbgpln("  %15.3f (+Cp)%15.3f (+dH)%15.3f = %15.3f [%6.1f->%6.1f] %s %s > %s", 
            Ht, I0.IntegrateCp(I0.m_T1, I0.m_T2), I1.m_Hx, Htemp, I0.m_T1, I0.m_T2, I0.Formula(), I0.Phase(), I1.Phase());
          Ht = Htemp;
          }
        H = Ht - IntegrateCp(C2K(25), m_T1);

        dbgpln("  %15.3f (-Cp)%15.3f      %15s = %15.3f %16s %s %s", Ht, IntegrateCp(C2K(25), m_T1), "", H, "", Formula(), Phase());
        }
      }
    }
  }

// ==========================================================================
//
//
//
// ==========================================================================

struct SDBInfo
  {
  Strng Name;
  Strng Compound; 
  Strng Definition;
  Strng Phase;
  Strng Occurence;
  Strng Checked;
  Strng Ts;
  Strng Te;
  Strng Rho;
  Strng dHf;
  Strng S;
  Strng Cp;
  Strng Vp;
  Strng Pc;
  Strng Tc;
  Strng Vc;
  Strng Ac;
  Strng MolecularDiam;
  Strng MolarVol;
  Strng CpCv;
  Strng Reference;

  flag    IsValid;

  SDBInfo();
  void Update(CCElementDataBase & EDB);
  flag Valid() { return IsValid; };
  void CopyData(CStringArray &Values, int iOC_MolecularDiam, int iOC_MolVol, int iOC_CpCv, int iOC_Reference);

  };

// --------------------------------------------------------------------------

SDBInfo::SDBInfo()
  {
  IsValid=false;
  }

// --------------------------------------------------------------------------

void SDBInfo::CopyData(CStringArray &Values, int iOC_MolecularDiam, int iOC_MolVol, int iOC_CpCv, int iOC_Reference)
  {
  Name          = (char*)(const char*)Values[ 0];
  Compound      = (char*)(const char*)Values[ 1]; 
  Definition    = (char*)(const char*)Values[ 2];
  Phase         = (char*)(const char*)Values[ 3];
  Occurence     = (char*)(const char*)Values[ 4];
  Checked       = (char*)(const char*)Values[ 5];
  Ts            = (char*)(const char*)Values[ 6];
  Te            = (char*)(const char*)Values[ 7];
  Rho           = (char*)(const char*)Values[ 8];
  dHf           = (char*)(const char*)Values[ 9];
  S             = (char*)(const char*)Values[10];
  Cp            = (char*)(const char*)Values[11];
  Vp            = (char*)(const char*)Values[12];
  Pc            = (char*)(const char*)Values[13];
  Tc            = (char*)(const char*)Values[14];
  Vc            = (char*)(const char*)Values[15];
  Ac            = (char*)(const char*)Values[16];
  MolecularDiam = iOC_MolecularDiam >=0   ? (char*)(const char*)Values[iOC_MolecularDiam] : "";
  MolarVol      = iOC_MolVol>=0           ? (char*)(const char*)Values[iOC_MolVol] : "";
  CpCv          = iOC_CpCv>=0             ? (char*)(const char*)Values[iOC_CpCv] : "";
  Reference     = iOC_Reference>=0        ? (char*)(const char*)Values[iOC_Reference] : "";
  
  };

// --------------------------------------------------------------------------

void SDBInfo::Update(CCElementDataBase & EDB)
  {
  IsValid=(Name.XStrChr('$')==NULL && Compound.XStrChr('$')==NULL);
  }

//===========================================================================

//_FWDDEF(CSDBSheet)
class  CSCD_DB//: public CPropertySheet
  {
  //DECLARE_DYNAMIC(CSDBSheet)
  public:
    CSCD_DB(LPCTSTR Filename, flag ReadOnly=false);//, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
//    CSCD_DB(char *Filename, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CSCD_DB();


  protected:
//    CSaveRestResHandle SRH;

  public:
    ADOX::_CatalogPtr     m_pCat;
    ADODB::_ConnectionPtr m_pCnn;
    ADODB::_RecordsetPtr  m_pRS;
    Strng m_Filename;
    Strng m_ErrStr;
    long  m_ErrNo;
    flag  m_ReadOnly;

    void SetDatabaseFile(LPCTSTR Filename) { m_Filename=Filename; };
    char * GetDatabaseFile() { return m_Filename(); };
    void DoCatch(_com_error & e, LPCTSTR Msg);
    BOOL OpenDataBase();//char * Filename);
    BOOL AddTable(ADOX::_TablePtr &pTbl, MyFldInfo * MFI, char*TablName);
    BOOL CheckCompPhaseIndex(ADOX::_TablePtr &pTbl);
    BOOL CheckWater();
    BOOL AddCompPhaseIndex(ADOX::_TablePtr &pTbl);
    BOOL CheckFieldNames(ADOX::_TablePtr &pTbl, MyFldInfo * MFI, char*TablName);//, char ** FieldNames);
    BOOL CloseDB();

    BOOL AddOldCSVSpecies(LPCTSTR CSVFileName);
    BOOL AddOldCSVCnvs(LPCTSTR CSVFileName);

//    static BOOL UpgradeCSVDB(LPCTSTR CSVPath, CUpGrdCompletion &UGC);

  };

//===========================================================================

class RowItem
  {
  public:
    Strng Comp, Occ;
    float Ts, Te;
  };

typedef CArray<RowItem, RowItem&> CRowArray;

//===========================================================================

_FWDDEF(CSDBSheet)
class  CSDBSheet : public CPropertySheet, public CSCD_DB
  {
  DECLARE_DYNAMIC(CSDBSheet)
  public:
    CSDBSheet(char *Filename, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CSDBSheet(char *Filename, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CSDBSheet();

    CPropertyPage* pSpeciePage;         
    CPropertyPage* pTablePage;   
    CPropertyPage* pTestPage;   
    CPropertyPage* pCnvsPage;             
    CPropertyPage* pSolnPage;             
    
    flag IsStatsPage();
    //{{AFX_VIRTUAL(CMdlCfgCfg)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

  protected:
//    CSaveRestResHandle SRH;

    void StoreCurPageNo();
    //{{AFX_MSG(CSDBSheet)
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnNcDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  public:
    CRowArray m_Rows;

    static flag Edit(const char* DBFn, flag CreateAndExit);
  };


/////////////////////////////////////////////////////////////////////////////
// CSDBSpecie dialog

/////////////////////////////////////////////////////////////////////////////
// CSDBSpecie dialog

class CSDBSpecie : public CPropertyPage
  {
// Construction
public:
	CSDBSpecie(CSDBSheet * pSheet);//CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSDBSpecie)
	enum { IDD = IDD_PP_SPDB_EDIT };
	CEdit	m_Reference;
	CListCtrl	m_SpList;
	CEdit	m_Vp;
	CEdit	m_Vc;
	CEdit	m_Ts;
	CEdit	m_Te;
	CEdit	m_Tc;
	CEdit	m_SpName;
	CEdit	m_SpDefn;
	CEdit	m_S;
	CEdit	m_Rho;
	CComboBox	m_Phase;
	CEdit	m_Pc;
	CEdit	m_dHf;
	CEdit	m_Cp;
	CEdit	m_Compound;
	CEdit	m_Checked;
	CEdit	m_Ac;
	CButton	m_Update;
	CButton	m_Undo;
	CButton	m_Import;
	CButton	m_Delete;
	CButton	m_Add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDBSpecie)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
  void CommonSplistAction();

	// Generated message map functions
	//{{AFX_MSG(CSDBSpecie)
	virtual BOOL OnInitDialog();
	afx_msg void OnSpAdd();
	afx_msg void OnSpDelete();
	afx_msg void OnSpImport();
	afx_msg void OnSpUpdate();
	afx_msg void OnChangeAc();
	afx_msg void OnChangeChecked();
	afx_msg void OnChangeReference();
	afx_msg void OnChangeCompound();
	afx_msg void OnChangeCp();
	afx_msg void OnChangeDhf();
	afx_msg void OnChangePc();
	afx_msg void OnChangeRho();
	afx_msg void OnChangeS();
	afx_msg void OnChangeSpdefn();
	afx_msg void OnChangeSpname();
	afx_msg void OnChangeTc();
	afx_msg void OnChangeTemphi();
	afx_msg void OnChangeTemplo();
	afx_msg void OnChangeVc();
	afx_msg void OnChangeVp();
	afx_msg void OnSpUndo();
	afx_msg void OnEditupdatePhase();
	afx_msg void OnEditupdateOcc();
	afx_msg void OnOccGas();
	afx_msg void OnOccLiq();
	afx_msg void OnOccSol();
	afx_msg void OnSelchangePhase();
	afx_msg void OnClickSplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownSplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusSplist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    CSDBSheet * m_pSheet;
    flag        m_bDidInit;
    flag        m_bIsActive;

    flag m_bDoingAdd, m_bAddBusy;
    CButton& GasOccBtn() { return *((CButton*)GetDlgItem(IDC_OCC_GAS)); };
    CButton& LiqOccBtn() { return *((CButton*)GetDlgItem(IDC_OCC_LIQ)); };
    CButton& SolOccBtn() { return *((CButton*)GetDlgItem(IDC_OCC_SOL)); };

    Strng m_CompPhaseUpd;

  public:  
    void SomethingChanged();
    BOOL BuildRowList(char * PositionAt=NULL);
    BOOL LoadRecord(int ItemIndex);
    BOOL UpdateRecord(flag ForceIt);
    BOOL ImportRecords(CArray<HSCInfo*, HSCInfo*> & InfoArray);
    BOOL ImportRecord(SDBInfo &Info);
    void ClearDisplay(int DoEnable=-1);
    BOOL DeleteRecords();
    BOOL ImportSpecies();

    //  
    int GetFirstSelection()
      {
      POSITION pos = m_SpList.GetFirstSelectedItemPosition();
      if (pos)
        return m_SpList.GetNextSelectedItem(pos);      
//      int Indices[1];
//      if (m_SpList.GetSelItems(sizeof(Indices)/sizeof(Indices[0]), Indices))
//        return Indices[0];
      return -1;
      }
    CRowArray & Rows()   { return m_pSheet->m_Rows; };
    ADODB::_ConnectionPtr ConnectionPtr()  { return m_pSheet->m_pCnn; };
    ADODB::_RecordsetPtr  Recordset() { return m_pSheet->m_pRS; };
    LPCTSTR Filename()    { return (LPCTSTR)(LPTSTR)m_pSheet->m_Filename(); };
    long   ErrNo()       { return m_pSheet->m_ErrNo; };
    flag   ReadOnly()    { return m_pSheet->m_ReadOnly; };
    void DoCatch(_com_error & e, char * Msg) { m_pSheet->DoCatch(e, Msg); };
  
  };

/////////////////////////////////////////////////////////////////////////////
// CHSCSlct dialog

class CHSCSlct : public CDialog
  {
  // Construction
  public:
	  CHSCSlct(char * Filename, CWnd* pParent = NULL);   // standard constructor
    virtual ~CHSCSlct();

  // Dialog Data
	  //{{AFX_DATA(CHSCSlct)
	  enum { IDD = IDD_SPDB_SLCTHSC};
	  CHeaderListBox m_List;
	  //}}AFX_DATA


  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CHSCSlct)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:

	  // Generated message map functions
	  //{{AFX_MSG(CHSCSlct)
	  virtual BOOL OnInitDialog();
	  virtual void OnOK();
	afx_msg void OnImport();
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

    CArray <HSCInfo*, HSCInfo*> m_InfoSelected;
    Strng m_Filename;
    HANDLE m_hFile;
    CCElementDataBase m_EDB;
    CArray<HSCInfo*, HSCInfo*> m_InfoArray;


  public:
    int GetSelectedCount();
    flag GetSelectedInfo(int No1, int Count, CArray <HSCInfo*, HSCInfo*> & InfoArray);
    void CompleteUpdate(CArray<HSCInfo*,HSCInfo*> & InfoArray, bool DoDump);

  };

/////////////////////////////////////////////////////////////////////////////
// CHSCSlct dialog

class CSDBSlct : public CDialog
  {
  // Construction
  public:
	  CSDBSlct(char * Filename, CWnd* pParent = NULL);   // standard constructor
    virtual ~CSDBSlct();

  // Dialog Data
	  //{{AFX_DATA(CSDBSlct)
	  enum { IDD = IDD_SPDB_SLCTSDB };
	  CHeaderListBox m_List;
	  //}}AFX_DATA

    CDBHelper *pDB;
    int iOC_MolecularDiam;
    int iOC_MolVol;
    int iOC_CpCv;
    int iOC_Reference;

  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CSDBSlct)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  // Implementation
  protected:
    //{{AFX_MSG(CSDBSlct)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnImport();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    CArray <int, int&> iIndices;
    CArray <DWORD, DWORD&> dwPositions;
    Strng m_Filename;
    HANDLE m_hFile;
    CCElementDataBase m_EDB;

  public:
    int GetSelectedCount();
    DWORD * GetPositionArray () { return &dwPositions[0]; };
    flag GetSelectedInfo(int No, SDBInfo & Info);
  };

/////////////////////////////////////////////////////////////////////////////
// CSDBTable dialog

class CSDBTable : public CPropertyPage
  {
  public:
    CSDBTable(CSDBSheet * pSheet);//CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CSDBTable)
    enum { IDD = IDD_PP_SPDB_TBL };
    CSSDBGrid	m_Grid;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSDBTable)
    public:
    virtual void OnCancel();
    virtual void OnOK();
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CSDBTable)
    virtual BOOL OnInitDialog();
    afx_msg void OnInitColumnPropsSsdbgridctrlapt1();
    afx_msg void OnUnboundReadDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* StartLocation, BOOL ReadPriorRows);
    afx_msg void OnUnboundAddDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* NewRowBookmark);
    afx_msg void OnUnboundWriteDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* WriteLocation);
    afx_msg void OnUnboundDeleteRowSsdbgridctrlapt1(VARIANT FAR* Bookmark);
    afx_msg void OnUnboundPositionDataSsdbgridctrlapt1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation);
    DECLARE_EVENTSINK_MAP()
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    CSDBSheet * m_pSheet;
    flag        m_bDidInit;

  public:
    ADODB::_ConnectionPtr ConnectionPtr()  { return m_pSheet->m_pCnn; };
    ADODB::_RecordsetPtr  Recordset() { return m_pSheet->m_pRS; };
    LPCTSTR Filename()    { return (LPCTSTR)(LPTSTR)m_pSheet->m_Filename(); };
    long   ErrNo()       { return m_pSheet->m_ErrNo; };
    flag   ReadOnly()    { return m_pSheet->m_ReadOnly; };
    void DoCatch(_com_error & e, char * Msg) { m_pSheet->DoCatch(e, Msg); };

    void Buff2RecSet(LPDISPATCH RowBuff);
  };

/////////////////////////////////////////////////////////////////////////////
// CSDBTest dialog
class CSDBTest : public CPropertyPage
  {
// Construction
public:
	CSDBTest(CSDBSheet * pSheet);//CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSDBTest)
	enum { IDD = IDD_PP_SPDB_TEST };
	CEdit	m_Intervals;
	CTabCtrl	m_Tab;
	CButton	m_ShowKg;
	CButton	m_ShowK;
	CStatic	m_NoGood;
	CStatic	m_NoBad;
	CButton	m_ShowAll;
	CSSDBGrid	m_ThermoGrid;
	CSSDBGrid	m_MsgGrid;
	CSSDBGrid	m_RelDensGrid;
	CSSDBGrid	m_SolubleGrid;
	CSSDBGrid	m_MixHeatGrid;
	//}}AFX_DATA

  CArray<CSSDBGrid*, CSSDBGrid*> m_Grids;
  CCElementDataBase m_EDB;
  CSpecieDataBase m_SDB;
  CComponentDataBase m_CDB;
  CH2ODataBase m_H2O;
  int m_soi_SolS;
  int m_soi_SolE;
  int m_soi_LiqS;
  int m_soi_LiqE;
  int m_soi_VapS; 
  int m_soi_VapE; 
  int m_soi_ALL;

  PhMask m_som_Sol;
  PhMask m_som_Liq;
  PhMask m_som_Gas;
  PhMask m_som_SL;
  PhMask m_som_ALL;
  
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDBTest)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSDBTest)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickSsdbgridctrl1();
	afx_msg void OnSelChangeSsdbgridctrl1(short SelType, short FAR* Cancel, short FAR* DispSelRowOverflow);
	afx_msg void OnUnboundPositionDataSsdbgridctrl1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation);
	afx_msg void OnSpDotest();
	afx_msg void OnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CSDBSheet * m_pSheet;
  flag        m_bDidInit;
  int         m_NIntervals;

  public:
    CRowArray & Rows()   { return m_pSheet->m_Rows; };
    ADODB::_ConnectionPtr ConnectionPtr()  { return m_pSheet->m_pCnn; };
    ADODB::_RecordsetPtr  Recordset() { return m_pSheet->m_pRS; };
    LPCTSTR Filename()    { return (LPCTSTR)(LPTSTR)m_pSheet->m_Filename(); };
    long   ErrNo()       { return m_pSheet->m_ErrNo; };
    flag   ReadOnly()    { return m_pSheet->m_ReadOnly; };
    void DoCatch(_com_error & e, char * Msg) { m_pSheet->DoCatch(e, Msg); };  

    void TestThermo(int &nGood, int &nBad);
    void TestRelDens(int &nGood, int &nBad);
    void TestSoluble(int &nGood, int &nBad);
    void TestMixHeat(int &nGood, int &nBad);
  };

/////////////////////////////////////////////////////////////////////////////
// CSDBCnvs dialog

class CSDBCnvs : public CPropertyPage
  {
// Construction
public:
  CSDBCnvs(CSDBSheet * pSheet);//CWnd* pParent = NULL);   // standard constructor
  virtual ~CSDBCnvs();

// Dialog Data
	//{{AFX_DATA(CSDBCnvs)
	enum { IDD = IDD_PP_SPDB_CNVS };
	CSSDBGrid	m_Grid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDBCnvs)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSDBCnvs)
	virtual BOOL OnInitDialog();
	afx_msg void OnInitColumnPropsSsdbgridctrlapt1();
	afx_msg void OnUnboundReadDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* StartLocation, BOOL ReadPriorRows);
	afx_msg void OnUnboundAddDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* NewRowBookmark);
	afx_msg void OnUnboundWriteDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* WriteLocation);
	afx_msg void OnUnboundDeleteRowSsdbgridctrlapt1(VARIANT FAR* Bookmark);
	afx_msg void OnUnboundPositionDataSsdbgridctrlapt1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  CSDBSheet * m_pSheet;
  flag        m_bDidInit;
  ADODB::_RecordsetPtr m_pRSCnv;

public:
    ADODB::_ConnectionPtr ConnectionPtr()  { return m_pSheet->m_pCnn; };
    ADODB::_RecordsetPtr  Recordset() { return m_pRSCnv; };
    Strng &Filename()    { return m_pSheet->m_Filename; };
    Strng &ErrStr()      { return m_pSheet->m_ErrStr; };
    long   ErrNo()       { return m_pSheet->m_ErrNo; };
    flag   ReadOnly()    { return m_pSheet->m_ReadOnly; };
    void DoCatch(_com_error & e, char * Msg) { m_pSheet->DoCatch(e, Msg); };

    void Buff2RecSet(LPDISPATCH RowBuff);
  };

/////////////////////////////////////////////////////////////////////////////
// CSDBSoln dialog

class CSDBSoln : public CPropertyPage
  {
// Construction
public:
  CSDBSoln(CSDBSheet * pSheet);//CWnd* pParent = NULL);   // standard constructor
  virtual ~CSDBSoln();

// Dialog Data
	//{{AFX_DATA(CSDBSoln)
	enum { IDD = IDD_PP_SPDB_SOLN };
	CSSDBGrid	m_Grid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDBSoln)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSDBSoln)
	virtual BOOL OnInitDialog();
	afx_msg void OnInitColumnPropsSsdbgridctrlapt1();
	afx_msg void OnUnboundReadDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* StartLocation, BOOL ReadPriorRows);
	afx_msg void OnUnboundAddDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* NewRowBookmark);
	afx_msg void OnUnboundWriteDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* WriteLocation);
	afx_msg void OnUnboundDeleteRowSsdbgridctrlapt1(VARIANT FAR* Bookmark);
	afx_msg void OnUnboundPositionDataSsdbgridctrlapt1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  CSDBSheet * m_pSheet;
  flag        m_bDidInit;
  ADODB::_RecordsetPtr  m_pRSSoln;
    
public:
    ADODB::_ConnectionPtr ConnectionPtr()  { return m_pSheet->m_pCnn; };
    ADODB::_RecordsetPtr  Recordset() { return m_pRSSoln; };
    Strng &Filename()    { return m_pSheet->m_Filename; };
    Strng &ErrStr()      { return m_pSheet->m_ErrStr; };
    long   ErrNo()       { return m_pSheet->m_ErrNo; };
    flag   ReadOnly()    { return m_pSheet->m_ReadOnly; };
    void DoCatch(_com_error & e, char * Msg) { m_pSheet->DoCatch(e, Msg); };

    void Buff2RecSet(LPDISPATCH RowBuff);
  };

/////////////////////////////////////////////////////////////////////////////

#define StrSpecieTbl      "Species"
#define StrCnvTbl         "Cnvs"
#define StrSolnTbl        "Solutions"

#define StrCompPhaseInx   "CompPhase"

struct MyFldInfo
  {
  char *                      m_Name;
  char *                      m_Caption;
  int                         m_Wide;
  int                         m_Align;
  ADOX::DataTypeEnum          m_nType;               // Primary
  char *                      m_sType;               // Primary
  long                        m_lSize;                // Primary
  ADOX::ColumnAttributesEnum  m_lAttributes;          // Primary
  BOOL                        m_bRequired;            // Secondary
  BOOL                        m_bAllowZeroLength;     // Secondary

  long m_Pos;
  };

static MyFldInfo SpFI[] =
  { // Name          Wd  Al   Type                  Len  Attributes                                        Reqd   ZeroOK
    { "Name"        , "Name"        , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Compound"    , "Compound"    , 14, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Phase"       , "Phase"       ,  3, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    10, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Definition"  , "Defn"        ,  8, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Occurence"   , "Occ"         ,  3, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    10, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Checked"     , "Chkd"        ,  5, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Ts"          , "Ts(K)"       ,  5, 1,  ADOX::adSingle,     "ADOX::adSingle",   4, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Te"          , "Te(K)"       ,  5, 1,  ADOX::adSingle,     "ADOX::adSingle",   4, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Rho"         , "Rho(kg/m^3)" , 10, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "dHf"         , "Hf(J/mol)"   ,  8, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "S298"        , "S(J/mol.K)"  ,  8, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Cp"          , "Cp(J/mol.K)" , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Vp"          , "Vp(kPa)"     , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Pc"          , "Pc(MPa)"     ,  6, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Tc"          , "Tc(K)"       ,  6, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Vc"          , "Vc(m^3)"     ,  6, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Ac"          , "Ac"          ,  6, 1,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { "Reference"   , "Reference"   , 25, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { NULL },
  };

static MyFldInfo CnvFI[] =
  { // Name          Wd  Al   Type                  Len  Attributes                        Reqd   ZeroOK
    { "Conversion"  , "Conversion"   , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Unit"        , "Unit"         , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "ScaleOffset" , "ScaleOffset"  , 20, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Description" , "Description"  , 20, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, false},
    { "Reference"   , "Reference"    , 25, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false, true },
    { NULL },
  };

static MyFldInfo SolnFI[] =
  { // Name          Wd  Al   Type                  Len  Attributes                        Reqd   ZeroOK
    { "Solvent"      , "Solvent"      , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "Solute"       , "Solute"       , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    //{ "OtherPhase"   , "OtherPhase"   , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), true , false},
    { "OtherPhase"   , "OtherPhase"   , 10, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",    64, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false , true},
    { "DensCorrFn"   , "DensCorrFn"   , 18, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false,  true},
    { "SolubilityFn" , "SolubilityFn" , 18, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false,  true},
    { "HDilutionFn"  , "HDilutionFn"  , 18, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   128, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false,  true},
    { "Reference"    , "Reference"    , 25, 0,  ADOX::adVarWChar,   "ADOX::adVarWChar",   255, /*dbVariableField|dbUpdatableField*/ADOX::ColumnAttributesEnum(0), false,  true },
    { NULL },
  };

static MyFldInfo * FindFld(LPCTSTR Name,  MyFldInfo * FI)
  {
  for (int i=0; FI[i].m_Name; i++)
    if (_stricmp(Name, FI[i].m_Name)==0)
      return &FI[i];
  return NULL;
  }

#define StrSpName         SpFI[0].m_Name
#define StrCompound       SpFI[1].m_Name
#define StrPhase          SpFI[2].m_Name
#define StrSpDefn         SpFI[3].m_Name
#define StrOcc            SpFI[4].m_Name
#define StrChecked        SpFI[5].m_Name
#define StrTs             SpFI[6].m_Name
#define StrTe             SpFI[7].m_Name
#define StrRho            SpFI[8].m_Name
#define StrdHf            SpFI[9].m_Name
#define StrS              SpFI[10].m_Name
#define StrCp             SpFI[11].m_Name
#define StrVp             SpFI[12].m_Name
#define StrPc             SpFI[13].m_Name
#define StrTc             SpFI[14].m_Name
#define StrVc             SpFI[15].m_Name
#define StrAc             SpFI[16].m_Name
#define StrReference      SpFI[17].m_Name

//===========================================================================
//
//
//
//===========================================================================

CSCD_DB::CSCD_DB(LPCTSTR Filename, flag ReadOnly)
  {
  m_Filename=Filename;
  m_ReadOnly=ReadOnly;
  }

//---------------------------------------------------------------------------

CSCD_DB::~CSCD_DB()
  {
  }


void CSCD_DB::DoCatch(_com_error & e, LPCTSTR Msg)
  {
  CString strErrCode;
  m_ErrStr=Msg;
  m_ErrStr+="\n";
  if (1)//e && AfxIsValidAddress(e->m_pErrorInfo, FALSE))
    {
    m_ErrNo=e.Error();
    m_ErrStr += strErrCode;
    m_ErrStr += (LPCTSTR)e.Source();
    m_ErrStr += _T(": ");
    m_ErrStr += (LPCTSTR)e.Description();
    }
  else
    m_ErrStr += "Unknown Exception";
  LogError("SpecieDB", LF_Exclamation, m_ErrStr());
  //AfxMessageBox(m_ErrStr());

  }

// --------------------------------------------------------------------------

BOOL CSCD_DB::OpenDataBase()//char * Filename)
  {
  try
    {
    m_pCnn=ADODB::_ConnectionPtr(__uuidof(ADODB::Connection));
    m_pRS=ADODB::_RecordsetPtr(__uuidof (ADODB::Recordset));
    m_pCat=ADOX::_CatalogPtr(__uuidof (ADOX::Catalog));
    }
  catch(_com_error &e)
    {
    DoCatch(e, _T("Unable to Create Database Ptrs\n"));
    return FALSE;
    }
  CString S;
  if (FileExists(m_Filename()))
    {
    try
      {
      S.Format(DBConnectOpenString(DBConnect_Default), m_Filename());
      m_pCnn->Open(_bstr_t(S), "", "", ADODB::adConnectUnspecified);
      m_pCat->PutActiveConnection(_variant_t((IDispatch*)m_pCnn));
      }
    catch(_com_error & e)
      {
      DoCatch(e, _T("Unable to Open Database\n") );
      return FALSE;
      }
    }
  else
    {
    try
      {
      S.Format(DBConnectCreateString(DBConnect_Default), m_Filename());
      m_pCat->Create(_bstr_t(S));
      m_pCnn=m_pCat->GetActiveConnection();
      }
    catch(_com_error & e)
      {
      DoCatch(e, _T("Unable to Create Database\n"));
      return FALSE;
      }
    } 

  // Check Specie Table
  ADOX::_TablePtr pSpcTb;
  try
    {
    pSpcTb=m_pCat->Tables->GetItem(_variant_t(StrSpecieTbl));
    }
  catch(_com_error & e)
    {
    // Item not found in this collection. (Error 3265)
    if (SCODE_CODE(e.Error())==ADODB::adErrItemNotFound)//->m_pErrorInfo->m_lErrorCode==3265)
      {
      AddTable(pSpcTb/*Species*/, SpFI, StrSpecieTbl);
      }
    else
      {
      DoCatch(e, "Open Species Table");
      return false;
      }
    }
  if (!CheckFieldNames(pSpcTb/*Species*/, SpFI, StrSpecieTbl))//, SpColNames))
    return false;

  // Check Conversion Table
  ADOX::_TablePtr pCnvTb;
  try
    {
    pCnvTb=m_pCat->Tables->GetItem(StrCnvTbl);
    }
  catch(_com_error & e)
    {
    // Item not found in this collection. (Error 3265)
    if (SCODE_CODE(e.Error())==ADODB::adErrItemNotFound)//->m_pErrorInfo->m_lErrorCode==3265)
      {
      AddTable(pCnvTb, CnvFI, StrCnvTbl);
      }
    else
      {
      DoCatch(e, "Open Conversions Table");
      return false;
      }
    }
  if (!CheckFieldNames(pCnvTb, CnvFI, StrCnvTbl))//, SpColNames))
    return false;

  // Check Solutions Table
  ADOX::_TablePtr pSlnTb;
  try
    {
    pSlnTb=m_pCat->Tables->GetItem(StrSolnTbl);
    }
  catch(_com_error & e)
    {
    // Item not found in this collection. (Error 3265)
    if (SCODE_CODE(e.Error())==ADODB::adErrItemNotFound)//->m_pErrorInfo->m_lErrorCode==3265)
      {
      AddTable(pSlnTb/*Soln*/, SolnFI, StrSolnTbl);
      }
    else
      {
      DoCatch(e, "Open Soln Table");
      return false;
      }
    }
  if (!CheckFieldNames(pSlnTb/*Soln*/, SolnFI, StrSolnTbl))//, SpColNames))
    return false;


  CheckCompPhaseIndex(pSpcTb/*Species*/);

  CheckWater();

  try
    {
    CString Slct;
    Slct.Format("SELECT * FROM %s ORDER BY %s,%s,%s;", StrSpecieTbl, StrCompound, StrPhase, StrTs);
    m_pRS->Open(_variant_t(Slct), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);

    //Slct.Format("%s ASC, %s ASC, %s ASC;", StrSpecieTbl, StrCompound, StrPhase, StrTs);
    //m_pRS->Optimize=true;
    //m_pRS->Sort=Slct;
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Open RecordSet");
    return false;
    }


  return true;
  }

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------

BOOL CSCD_DB::AddTable(ADOX::_TablePtr &pTbl, MyFldInfo * MFI, char*TablName)
  {
  try
    {
    pTbl=ADOX::_TablePtr(__uuidof(ADOX::Table));    
    pTbl->Name=TablName;
    for (int f=0; MFI[f].m_Name; f++)
      {
      ADOX::ColumnAttributesEnum Att = (MFI[f].m_bRequired ? MFI[f].m_lAttributes : ADOX::adColNullable);
      MyAddField(pTbl, m_pCat, MFI[f].m_Name, MFI[f].m_nType, MFI[f].m_lSize, Att);
      }
    m_pCat->Tables->Append(_variant_t((IDispatch*)pTbl));//Tbl.Append();
    }
  catch(_com_error & e)
    {
    Strng Msg;
    Msg.Set("Creating '%s' Table", TablName);
    DoCatch(e, Msg());
    return false;
    }
  return true;
  }

// --------------------------------------------------------------------------

BOOL CSCD_DB::CheckCompPhaseIndex(ADOX::_TablePtr &pTbl)
  {
  BOOL OK=true;

  ADOX::_IndexPtr pIdx;
  try
    {
    pIdx=pTbl->Indexes->GetItem(StrCompPhaseInx);
    }
  catch(_com_error & e)
    {
    // Item not found in this collection. (Error 3265)
    if (SCODE_CODE(e.Error())==ADODB::adErrItemNotFound)//->m_pErrorInfo->m_lErrorCode==3265)
      {
      return AddCompPhaseIndex(pTbl);
      }
    else
      {
      DoCatch(e, "Open Index");
      return false;
      }
    }

  return OK;
  };

// --------------------------------------------------------------------------

BOOL CSCD_DB::AddCompPhaseIndex(ADOX::_TablePtr  &pTbl)
  {
  try
    {
    ADOX::_IndexPtr pIdx=ADOX::_IndexPtr(__uuidof(ADOX::Index));
    pIdx->Name=StrCompPhaseInx;
    pIdx->Columns->Append(StrCompound,  ADOX::adVarWChar, 0);
    pIdx->Columns->Append(StrPhase,     ADOX::adVarWChar, 0);
    pIdx->Columns->Append(StrTs,        ADOX::adSingle,   0);
    pIdx->PutPrimaryKey(0);
    pIdx->PutUnique(0);

    pTbl->Indexes->Append(_variant_t((IDispatch*)pIdx));
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Create Index");
    return false;
    }
  return true;
  };

// --------------------------------------------------------------------------

BOOL CSCD_DB::CheckFieldNames(ADOX::_TablePtr &pTbl, MyFldInfo * MFI, char*TablName)//, char ** FieldNames)
  {
  const int FldCnt = pTbl->Columns->Count;

  int ExpectedFldCnt = 0;
  while (MFI[ExpectedFldCnt].m_Name)
    ExpectedFldCnt++;

  CArray <bool, bool> FoundIt;
  FoundIt.SetSize(ExpectedFldCnt);
  for (long i=0; i<ExpectedFldCnt; i++)
    FoundIt[i]=false;

  for (long j=0; (j<FldCnt); j++)
    {
    //Tbl.GetFieldInfo(j, fi);
    for (i=0; i<ExpectedFldCnt; i++)
      {
      ADOX::_ColumnPtr pCol=pTbl->Columns->GetItem((long)j);
      if (_stricmp(MFI[i].m_Name, pCol->Name)==0)
        {
        if (pCol->Type==MFI[i].m_nType)
          {
          FoundIt[i] = TRUE;
          //TODO Check field parameters and change if required
          }
        else
          //LogError(TablName, LF_Exclamation, "Field '%s' should be of type %s (%s)", MFI[i].m_Name, (LPCTSTR)CCrack::strFieldType(MFI[i].m_nType), MFI[i].m_sType);
          LogError(TablName, LF_Exclamation, "Field '%s' should be of data type %s", MFI[i].m_Name, MFI[i].m_sType);
        }
      }
    }
  
  flag Fnd=true;
  for (int f=0; f<ExpectedFldCnt; f++)
    if (!FoundIt[f])
      {
      try
        {
        ADOX::ColumnAttributesEnum Att = (MFI[f].m_bRequired ? MFI[f].m_lAttributes : ADOX::adColNullable);
        //Required        = ((pCol->Attributes & ADODB::adFldIsNullable)==0);
        MyAddField(pTbl, m_pCat, MFI[f].m_Name, MFI[f].m_nType, MFI[f].m_lSize, Att);
        }
      catch(_com_error & e)
        {
        DoCatch(e, "Create New Field");
        Fnd=false;
        }
      }

  return Fnd;
  }

//---------------------------------------------------------------------------

BOOL CSCD_DB::CloseDB()
  {
  try
    {
    m_pRS->Close();
    m_pCnn->Close();

    m_pRS.Release();
    m_pCnn.Release();
    m_pCat.Release();

    }
  catch(_com_error & e)
    {
    DoCatch(e, "Closing Database");
    return false;
    }
  return true;
  };

//---------------------------------------------------------------------------

void INS(ADODB::_RecordsetPtr pRS, const char* Nm, const char* Str_, bool Flt)
  {                                                               
  COleVariant var;
  if (Str_ && strlen(Str_)>0)                                       
    {                                                             
    if (Flt)                                                      
      {                                                           
      var=(float)SafeAtoF((char*)(const char*)Str_, 298.16);       
      }                                                           
    else                                                          
      {
      //var.SetString((const char*)Str_, VT_BSTRT);                  
      var=T2BSTR(Str_); 
      }
    pRS->Fields->GetItem(Nm)->Value=var;                          
    }                                                             
  else                                                            
    {                                                             
    if (Str_)                                                      
      {                                                           
      if (Flt)                                                    
        var=(float)0.0F;                                          
      else                                                        
        var=T2BSTR(Str_); 
      pRS->Fields->GetItem(Nm)->Value=var;                        
      }                                                           
    else                                                          
      {                                                           
       _variant_t nvar;                                           
      pRS->Fields->GetItem(Nm)->Value=nvar;                       
      }                                                           
    }                                                             
  }

// --------------------------------------------------------------------------

BOOL CSCD_DB::CheckWater()
  {
  //ASSERT(m_pRS==NULL);
  ADODB::_RecordsetPtr pRS=ADODB::_RecordsetPtr(__uuidof (ADODB::Recordset));
  for (int j=0; j<2; j++)
    {
    try
      {
      CString Slct;
      Slct.Format("SELECT * FROM %s WHERE %s=\"H2O\" AND %s=\"%s\";", StrSpecieTbl, StrCompound, StrPhase, j==0 ? "g" : "l");
      pRS->Open(_variant_t(Slct), _variant_t((IDispatch*)m_pCnn), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Open RecordSet");
      return false;
      }
    try
      {
      if (pRS->BOF)// || pRS->adEOF
        {
        int Lst = (j==0 ? 1 : 3);
        for (int i=j; i<Lst; i++)
          {
          pRS->AddNew();
          //COleVariant var;
          INS(pRS, StrSpName  , (i==0 ? "Steam" : "Water") ,false);
          INS(pRS, StrCompound, "H2O"     ,false);
          INS(pRS, StrSpDefn  , "H2O1"    ,false);
          INS(pRS, StrPhase   , (i==0 ? "g" : "l") ,false);
          INS(pRS, StrOcc     , (i==0 ? "g" : "l") ,false);
          INS(pRS, StrChecked , "" ,false);
          INS(pRS, StrTs      , (i==2 ? "372.78" : "298.15") ,true);
          INS(pRS, StrTe      , (i==0 ? "513.15" :      (i==1 ? "372.78"      : "500")) ,true);
          INS(pRS, StrRho     , (i==0 ? "0.8037" :      (i==1 ? "LiqH2ORho()" : "")) ,false);
          INS(pRS, StrdHf     , (i==0 ? "-241826.833" : (i==1 ? "-285829.970" : "-285965.245")) ,false);
          INS(pRS, StrS       , (i==0 ? "188.832" :     (i==1 ? "69.948"      : "0.0")) ,false);
          INS(pRS, StrCp      , (i==0 ? "CRC_Cp(-38.14955, 463.4602, 6.24E-05, -762.3604)" : (i==1 ? "CRC1_Cp(53.927,31.712,31.365,8.167)" : "HSC_Cp(16.749,62.120,32.798,90.391)")) ,false);
          INS(pRS, StrVp      , (i==0 ? "Vp(-3433.74,-12.0063,0.004782,41.1767)" : "Vp(-3433.74,-12.0063,0.004782,41.1767)") ,false);
          INS(pRS, StrPc      , (i==0 ? "22.06" : "") ,false);
          INS(pRS, StrTc      , (i==0 ? "647.14" : "") ,false);
          INS(pRS, StrVc      , (i==0 ? "0.0568" : "") ,false);
          INS(pRS, StrAc      , (i==0 ? "0.3442" : "") ,false);
          INS(pRS, StrReference, "" ,false);
          pRS->Update();
          }
        }
      pRS->Close();
      }
    catch(_com_error & e)
      {
      Strng S;
      S.Set("Inserting Record H2O(%s)", (j==0 ? "g" : "l"));
      DoCatch(e, S());
      }
    }
  return true;
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_DYNAMIC(CSDBSheet, CPropertySheet)

CSDBSheet::CSDBSheet(char *Filename, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CSCD_DB(Filename), CPropertySheet(nIDCaption, pParentWnd, iSelectPage)//,
//  m_RS(&m_DB)//, pParent)
  {
  //m_pCnn=NULL;
  //m_pRS=NULL;
  ASSERT(FALSE); //DO NOT EXPECT TO GET HERE !!!
  }

//---------------------------------------------------------------------------

CSDBSheet::CSDBSheet(char *Filename, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
  : CSCD_DB(Filename), CPropertySheet(pszCaption, pParentWnd, iSelectPage)
  {
  ASSERT_VALID(this);
  //m_Filename=Filename;
  //CSaveRestResHandle SRH(SCExecDLL.hModule);
  pSpeciePage    = new CSDBSpecie(this);
  pTablePage     = new CSDBTable(this);
  pTestPage      = new CSDBTest(this);
  pCnvsPage      = new CSDBCnvs(this);
  pSolnPage      = new CSDBSoln(this);

  AddPage(pSpeciePage);
  AddPage(pTablePage);
  AddPage(pSolnPage);
  AddPage(pTestPage);
  AddPage(pCnvsPage);

  //m_pRS=NULL;
  //m_pCnn=NULL;
  //m_ReadOnly=false;
  }

//---------------------------------------------------------------------------

CSDBSheet::~CSDBSheet()
  {
  ASSERT_VALID(this);
  delete pSpeciePage;
  delete pTablePage;
  delete pTestPage;
  delete pCnvsPage;
  delete pSolnPage;

  //delete m_pRS;
  //delete m_pCnn;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSDBSheet, CPropertySheet)
  //{{AFX_MSG_MAP(CSDBSheet)
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()
  ON_COMMAND(IDOK,OnOK)
  ON_COMMAND(IDCANCEL,OnCancel)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CSDBSheet::OnInitDialog()
  {
  ASSERT_VALID(this);
  if (!OpenDataBase())//m_Filename()))
    {
    LogError("SpecieDB", LF_Exclamation, "Database not Opened");
//    Strng S;
//    S.Set("%s not opened"
//    AfxMessage(
    return false;
    }

//  if (m_pCnn->CanTransact())
//    m_pCnn->m_pWorkspace->BeginTrans();
////  else
////    m_Cancel.ShowWindow(SW_HIDE);

  BOOL b = CPropertySheet::OnInitDialog();

  GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);
  GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);

  return b;
  }

//---------------------------------------------------------------------------

void CSDBSheet::StoreCurPageNo()
  {
  CPropertyPage* pPage = GetActivePage();
  if (pPage)
    {
//    for (int i=0; i<GetPageCount(); i++)
//      if (GetPage(i)==pPage)
//        {
//        CProfINIFile PF(PrjIniFile());
//        PF.WrInt("General", "GenOptionsPageNo", i);
//        }
    }
  }

//---------------------------------------------------------------------------

void CSDBSheet::OnOK()
  {
  ASSERT_VALID(this);
//  StoreCurPageNo();
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnOK();
  if (GetActivePage()->OnKillActive())
    {
    CloseDB();
    EndDialog(IDOK);
    }
  }

//---------------------------------------------------------------------------

void CSDBSheet::OnCancel()
  {
  ASSERT_VALID(this);
  for (int i=0; i<GetPageCount(); i++)
    GetPage(i)->OnCancel();
  CloseDB();
  EndDialog(IDCANCEL);
  }

//---------------------------------------------------------------------------

void CSDBSheet::OnClose()
  {
  ASSERT_VALID(this);
  OnCancel();
  }

//---------------------------------------------------------------------------

flag CSDBSheet::IsStatsPage()
  {
  return FALSE;//(GetActivePage() && GetActivePage()->IsKindOf(RUNTIME_CLASS(CStatsPage)));
  }

//---------------------------------------------------------------------------

void CSDBSheet::OnNcDestroy()
  {
  CPropertySheet::OnNcDestroy();
//  delete this;
  }

// --------------------------------------------------------------------------

flag CSDBSheet::Edit(LPCTSTR DBFn, flag CreateAndExit)
  {
  CSDBSheet DBE((char *)DBFn, "Specie Database", AfxGetMainWnd(), 0);
  if (CreateAndExit)
    return DBE.OpenDataBase();

  DBE.DoModal();
  return true;
  };

/////////////////////////////////////////////////////////////////////////////
// CSDBSpecie dialog

CSDBSpecie::CSDBSpecie(CSDBSheet * pSheet)//CWnd* pParent /*=NULL*/)
  : CPropertyPage(CSDBSpecie::IDD)//,
//  m_RS(&m_DB)//, pParent)
  {
	//{{AFX_DATA_INIT(CSDBSpecie)
	//}}AFX_DATA_INIT
  m_bDidInit=false;
  m_pSheet=pSheet;
  m_bIsActive=false;
  }


void CSDBSpecie::DoDataExchange(CDataExchange* pDX)
  {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDBSpecie)
	DDX_Control(pDX, IDC_REFERENCE, m_Reference);
	DDX_Control(pDX, IDC_SPLIST, m_SpList);
	DDX_Control(pDX, IDC_VP, m_Vp);
	DDX_Control(pDX, IDC_VC, m_Vc);
	DDX_Control(pDX, IDC_TEMPLO, m_Ts);
	DDX_Control(pDX, IDC_TEMPHI, m_Te);
	DDX_Control(pDX, IDC_TC, m_Tc);
	DDX_Control(pDX, IDC_SPNAME, m_SpName);
	DDX_Control(pDX, IDC_SPDEFN, m_SpDefn);
	DDX_Control(pDX, IDC_S, m_S);
	DDX_Control(pDX, IDC_RHO, m_Rho);
	DDX_Control(pDX, IDC_PHASE, m_Phase);
	DDX_Control(pDX, IDC_PC, m_Pc);
	DDX_Control(pDX, IDC_DHF, m_dHf);
	DDX_Control(pDX, IDC_CP, m_Cp);
	DDX_Control(pDX, IDC_COMPOUND, m_Compound);
	DDX_Control(pDX, IDC_CHECKED, m_Checked);
	DDX_Control(pDX, IDC_AC, m_Ac);
	DDX_Control(pDX, ID_SP_UPDATE, m_Update);
	DDX_Control(pDX, ID_SP_UNDO, m_Undo);
	DDX_Control(pDX, ID_SP_IMPORT, m_Import);
	DDX_Control(pDX, ID_SP_DELETE, m_Delete);
	DDX_Control(pDX, ID_SP_ADD, m_Add);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSDBSpecie, CPropertyPage)
	//{{AFX_MSG_MAP(CSDBSpecie)
	ON_BN_CLICKED(ID_SP_ADD, OnSpAdd)
	ON_BN_CLICKED(ID_SP_DELETE, OnSpDelete)
	ON_BN_CLICKED(ID_SP_IMPORT, OnSpImport)
	ON_BN_CLICKED(ID_SP_UPDATE, OnSpUpdate)
	ON_EN_CHANGE(IDC_AC, OnChangeAc)
	ON_EN_CHANGE(IDC_CHECKED, OnChangeChecked)
	ON_EN_CHANGE(IDC_REFERENCE, OnChangeReference)
	ON_EN_CHANGE(IDC_COMPOUND, OnChangeCompound)
	ON_EN_CHANGE(IDC_CP, OnChangeCp)
	ON_EN_CHANGE(IDC_DHF, OnChangeDhf)
	ON_EN_CHANGE(IDC_PC, OnChangePc)
	ON_EN_CHANGE(IDC_RHO, OnChangeRho)
	ON_EN_CHANGE(IDC_S, OnChangeS)
	ON_EN_CHANGE(IDC_SPDEFN, OnChangeSpdefn)
	ON_EN_CHANGE(IDC_SPNAME, OnChangeSpname)
	ON_EN_CHANGE(IDC_TC, OnChangeTc)
	ON_EN_CHANGE(IDC_TEMPHI, OnChangeTemphi)
	ON_EN_CHANGE(IDC_TEMPLO, OnChangeTemplo)
	ON_EN_CHANGE(IDC_VC, OnChangeVc)
	ON_EN_CHANGE(IDC_VP, OnChangeVp)
	ON_BN_CLICKED(ID_SP_UNDO, OnSpUndo)
	ON_CBN_EDITUPDATE(IDC_PHASE, OnEditupdatePhase)
	ON_BN_CLICKED(IDC_OCC_GAS, OnOccGas)
	ON_BN_CLICKED(IDC_OCC_LIQ, OnOccLiq)
	ON_BN_CLICKED(IDC_OCC_SOL, OnOccSol)
	ON_CBN_SELCHANGE(IDC_PHASE, OnSelchangePhase)
	ON_NOTIFY(NM_CLICK, IDC_SPLIST, OnClickSplist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_SPLIST, OnKeydownSplist)
	ON_NOTIFY(NM_SETFOCUS, IDC_SPLIST, OnSetfocusSplist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSDBSpecie::OnInitDialog()
  {
	if (!m_bDidInit)
    {
    CWaitCursor W;
  	CPropertyPage::OnInitDialog();
	
    #if dbgMakeSpDefn1
    Strng Def;
    //Strng F("*3Ca3(PO4)2*Ca(OH)2");
    Strng F("BaO*TiO2");
    while (1)
      {
      dbgfiletrunc(0);
      dbgpln("%s",F());
      MakeSpDefn(F, Def);
      }
    #endif

    m_bDoingAdd=false;
    m_bAddBusy=false;

    m_Update.EnableWindow(false);
    m_Undo.EnableWindow(false);
    m_Delete.EnableWindow(false);

    m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
    m_Compound.EnableWindow(m_bDoingAdd);
    m_Phase.EnableWindow(m_bDoingAdd);

    //CListCtrl *pMsgCtrl = (CListCtrl*)GetDlgItem(IDC_MSGLIST);
    //if (pMsgCtrl->m_hWnd)
    //  {
    m_SpList.InsertColumn(0, "Specie", LVCFMT_LEFT, 140);
    m_SpList.InsertColumn(1, "From", LVCFMT_RIGHT, 45);
    m_SpList.InsertColumn(2, "To(K)", LVCFMT_RIGHT, 45);
      //m_SpList.SetImageList(&m_ImgList , LVSIL_SMALL);
    //  }

    BuildRowList();

    //m_SpList.SetSel(0);
    m_SpList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_SpList.SetSelectionMark(0);


    m_bDidInit=true;
    }
  //InitDataBase(m_pSheet->GetDatabaseFile());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

void CSDBSpecie::OnSpAdd()
  {
  m_bDoingAdd=true;
  m_bAddBusy=false;
  m_Compound.EnableWindow(m_bDoingAdd);
  m_Phase.EnableWindow(m_bDoingAdd);
  m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
  m_Delete.EnableWindow(!m_bDoingAdd && !ReadOnly());
  m_Update.EnableWindow(true && !ReadOnly());
  m_Undo.EnableWindow(true && !ReadOnly());
  }

void CSDBSpecie::OnSpDelete()
  {
  DeleteRecords();

  m_bDoingAdd=false;
  m_bAddBusy=false;
  m_Compound.EnableWindow(m_bDoingAdd);
  m_Phase.EnableWindow(m_bDoingAdd);
  m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
  m_Delete.EnableWindow(false && !ReadOnly());
  m_Update.EnableWindow(false && !ReadOnly());
  m_Undo.EnableWindow(false && !ReadOnly());

  BuildRowList();
  }

void CSDBSpecie::OnSpImport()
  {
  ClearDisplay(0);
  if (ImportSpecies())
    {
    Recordset()->Requery(ADODB::adCmdUnknown);
    ClearDisplay(1);
    BuildRowList(m_CompPhaseUpd());
    }
  else
    ClearDisplay(1);

  if (GetFirstSelection()>=0)
    LoadRecord(GetFirstSelection());

  m_bDoingAdd=false;
  m_bAddBusy=false;
  m_Compound.EnableWindow(m_bDoingAdd);
  m_Phase.EnableWindow(m_bDoingAdd);
  m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
  m_Delete.EnableWindow(GetFirstSelection()>=0 && !ReadOnly());
  m_Update.EnableWindow(false);
  m_Undo.EnableWindow(false);
  }

void CSDBSpecie::OnSpUpdate()
  {
  if (UpdateRecord(true))
    {
    Recordset()->Requery(ADODB::adCmdUnknown);
    if (m_bDoingAdd)
      BuildRowList(m_CompPhaseUpd());
    m_bDoingAdd=false;
    m_bAddBusy=false;

    m_Compound.EnableWindow(m_bDoingAdd);
    m_Phase.EnableWindow(m_bDoingAdd);
    m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
    m_Delete.EnableWindow(GetFirstSelection()>=0 && !ReadOnly());
    m_Update.EnableWindow(false);
    m_Undo.EnableWindow(false);
    }
  else
    {
    }	
  }

void CSDBSpecie::OnSpUndo()
  {
  LoadRecord(GetFirstSelection());
  m_bDoingAdd=false;
  m_bAddBusy=false;

  m_Compound.EnableWindow(m_bDoingAdd);
  m_Phase.EnableWindow(m_bDoingAdd);
  m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
  m_Delete.EnableWindow(GetFirstSelection()>=0 && !ReadOnly());
  m_Update.EnableWindow(false);
  m_Undo.EnableWindow(false);
  }

// --------------------------------------------------------------------------

void CSDBSpecie::OnChangeAc()              {	SomethingChanged(); };
void CSDBSpecie::OnChangeChecked()         {	SomethingChanged(); };
void CSDBSpecie::OnChangeReference()       {	SomethingChanged(); };
void CSDBSpecie::OnChangeCompound()        {	SomethingChanged(); };
void CSDBSpecie::OnChangeCp()              {	SomethingChanged(); };
void CSDBSpecie::OnChangeDhf()             {	SomethingChanged(); };
void CSDBSpecie::OnEditupdateOcc()         {	SomethingChanged(); };
void CSDBSpecie::OnChangePc()              {	SomethingChanged(); };
void CSDBSpecie::OnEditupdatePhase()       {	SomethingChanged(); };
void CSDBSpecie::OnChangeRho()             {	SomethingChanged(); };
void CSDBSpecie::OnChangeS()               {	SomethingChanged(); };
void CSDBSpecie::OnChangeSpdefn()          {	SomethingChanged(); };
void CSDBSpecie::OnChangeSpname()          {	SomethingChanged(); };
void CSDBSpecie::OnChangeTc()              {	SomethingChanged(); };
void CSDBSpecie::OnChangeTemphi()          {	SomethingChanged(); };
void CSDBSpecie::OnChangeTemplo()          {	SomethingChanged(); };
void CSDBSpecie::OnChangeVc()              {	SomethingChanged(); };
void CSDBSpecie::OnChangeVp()              {	SomethingChanged(); };

void CSDBSpecie::OnOccGas()                {	SomethingChanged(); };
void CSDBSpecie::OnOccLiq()                {	SomethingChanged(); };
void CSDBSpecie::OnOccSol()                {	SomethingChanged(); };
void CSDBSpecie::OnSelchangePhase()        {	SomethingChanged(); };

void CSDBSpecie::SomethingChanged()
  {
  m_Update.EnableWindow(true && !ReadOnly());
  m_Undo.EnableWindow(true && !ReadOnly());
  };

void CSDBSpecie::OnCancel()
  {
  if (m_bDidInit)
    {
//    if (Recordset()->GetAbsolutePosition()>=0)
//      UpdateRecord(false);
    CPropertyPage::OnCancel();
    }
  }

void CSDBSpecie::OnOK()
  {
	if (m_bDidInit)
    {
    if (m_bIsActive && Recordset()->GetAbsolutePosition()>=0)
      UpdateRecord(false);

    //CDBInitRec CDBI("H2O", C_2_K(0.0), C_2_K(1000.0), 50.0, 350.0);
    //for (int i=0; i<Rows().GetSize(); i++)
    //  {
    //  Strng S;//=new Strng("");
    //  S.Set("%s(%s)", Rows()[i].Comp(),Rows()[i].Occ());
    //  CDBI.RqdSpecies.Append(S());
    //  }
    CPropertyPage::OnOK();
    }
  }

void CSDBSpecie::CommonSplistAction()
  {
  //if (Recordset()->GetAbsolutePosition()>=0)
  //  UpdateRecord(false);

  LockWindowUpdate();
  LoadRecord(GetFirstSelection());
  m_bDoingAdd=false;
  m_bAddBusy=false;
  m_Compound.EnableWindow(m_bDoingAdd);
  m_Phase.EnableWindow(m_bDoingAdd);
  m_Add.EnableWindow(!m_bDoingAdd && !ReadOnly());
  m_Delete.EnableWindow(GetFirstSelection()>=0 && !ReadOnly());
  m_Update.EnableWindow(false);
  m_Undo.EnableWindow(false);
  UnlockWindowUpdate();
  }

void CSDBSpecie::OnClickSplist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	CommonSplistAction();
	*pResult = 0;
  }

void CSDBSpecie::OnKeydownSplist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
  //CommonSplistAction();
  m_SpList.PostMessage(WM_SETFOCUS);//Kludge to force code execution AFTER selection change
	*pResult = 0;
  }

void CSDBSpecie::OnSetfocusSplist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	CommonSplistAction();
	*pResult = 0;
  }

BOOL CSDBSpecie::OnSetActive()
  {
  m_bIsActive=true;
  if (GetFirstSelection()>=0)
    LoadRecord(GetFirstSelection());
	//LoadRecord(NULL);//GetFirstSelection());

	return CPropertyPage::OnSetActive();
  }

BOOL CSDBSpecie::OnKillActive()
  {
  //if (Recordset()->GetAbsolutePosition()>=0)
  //  UpdateRecord(false);
	m_bIsActive=false;
	return CPropertyPage::OnKillActive();
  }

/////////////////////////////////////////////////////////////////////////////
// CSDBSpecie Other Stuff

// --------------------------------------------------------------------------

inline LPTSTR ToStrng(LPCTSTR T) { return (LPTSTR)T;}

BOOL CSDBSpecie::BuildRowList(char * PositionAt)
  {
  USES_CONVERSION;
  Rows().SetSize(0, 128);
  if (!Recordset()->BOF)
    {
    int i=0;
    Recordset()->Filter="";
    //Recordset()->Sort="";
    Recordset()->MoveFirst();
    while (!Recordset()->adEOF)
      {
      COleVariant strValue;
      strValue=Recordset()->Fields->GetItem(StrCompound)->Value;
      Strng CompName;
      Strng FixedCompName;
      if (strValue.vt!=VT_NULL && strchr(OLE2T(V_BSTR(&strValue)), '$')==0)
        {
        CompName=OLE2T(V_BSTR(&strValue));
        CheckSymbolName(CompName(), FixedCompName);
        Rows().SetSize(i+1, 128);
        Rows()[i].Comp=FixedCompName();//V_BSTR(&strValue);

         strValue=Recordset()->Fields->GetItem(StrPhase)->Value;
        if (strValue.vt==VT_NULL)
          LogWarning("SDBEdit", 0, "%s Occurrence missing", FixedCompName());
        Rows()[i].Occ=(strValue.vt==VT_NULL) ? "?": OLE2T(V_BSTR(&strValue));
        
         strValue=Recordset()->Fields->GetItem(StrTs)->Value;
        if (strValue.vt==VT_NULL)
          LogWarning("SDBEdit", 0, "%s Start temperature missing", FixedCompName());
        Rows()[i].Ts=(strValue.vt==VT_NULL) ? 273.0f : (strValue.vt==VT_R4) ? V_R4(&strValue) : (float)SafeAtoF(OLE2T(V_BSTR(&strValue)));
        
         strValue=Recordset()->Fields->GetItem(StrTe)->Value;
        if (strValue.vt==VT_NULL)
          LogWarning("SDBEdit", 0, "%s End temperature missing", FixedCompName());
        Rows()[i].Te=(strValue.vt==VT_NULL) ? Rows()[i].Ts+100.0f : (strValue.vt==VT_R4) ? V_R4(&strValue) : (float)SafeAtoF(OLE2T(V_BSTR(&strValue)));
        i++;
        }
      else
        FixedCompName="$Error";

      if (FixedCompName!=CompName && !ReadOnly())
        {
        try
          {
          //Recordset()->Edit();
          strValue.SetString(FixedCompName(), VT_BSTRT);
          Recordset()->Fields->GetItem(StrCompound)->Value=strValue;
          Recordset()->Update();
          }
        catch(_com_error & e)
          {
          DoCatch(e, "Fixing Record");
          }
        }

      Recordset()->MoveNext();
      }	
    }

  m_SpList.LockWindowUpdate();
  m_SpList.DeleteAllItems();//ResetContent();
  int Wide=1;
  for (int i=0; i<Rows().GetSize(); i++)
    {
    Strng S, Ts, Te;
    S=Rows()[i].Comp();
    S+="(";
    S+=Rows()[i].Occ();
    S+=")";
    Ts.Set("%.1f", Rows()[i].Ts);
    Te.Set("%.1f", Rows()[i].Te);
    
    int n=m_SpList.InsertItem(m_SpList.GetItemCount(), S());
    m_SpList.SetItemText(n, 1, Ts());
    m_SpList.SetItemText(n, 2, Te());
    m_SpList.SetItemData(n, (DWORD)i);
    };

  if (PositionAt)
    {
//    DoBreak();
    LVFINDINFO FI;
    FI.flags=LVFI_STRING;
    FI.psz=PositionAt;
    int i=m_SpList.FindItem(&FI, -1);

    if (i>=0)
      m_SpList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
    }
  m_SpList.UnlockWindowUpdate();

  return true;
  }

// --------------------------------------------------------------------------

#define GET(Fld, Nm)                                             \
  var=Recordset()->Fields->GetItem(Nm)->Value;                           \
  if (var.vt==VT_R4)                                             \
    Fld.SetWindowText((AStr.Set("%.3f",V_R4(&var)), AStr()));    \
  else                                                           \
    Fld.SetWindowText((var.vt!=VT_NULL) ? OLE2T(V_BSTR(&var)) : "");

// --------------------------------------------------------------------------

BOOL CSDBSpecie::LoadRecord(int ItemIndex)
  {
  USES_CONVERSION;
  int RowIndex=m_SpList.GetItemData(ItemIndex);
  if (ItemIndex>=0)
    {
    m_SpList.SetItemState(ItemIndex, LVIS_SELECTED, LVIS_SELECTED);
    m_SpList.EnsureVisible(ItemIndex, FALSE);
    }

  Strng Fnd, Name, X1, X2;
  X1.Set("%.3f", Rows()[RowIndex].Ts-0.1);
  X2.Set("%.3f", Rows()[RowIndex].Ts+0.1);
  Fnd.Set("[Compound]='%s' AND [Phase]='%s' AND [Ts] >= %s AND [Ts] <= %s",
           Rows()[RowIndex].Comp(), Rows()[RowIndex].Occ(), X1(), X2());

  try
    {
    Recordset()->Filter=Fnd();
    Recordset()->MoveFirst();
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Find Record");
    return false;
    }

  if (Recordset()->BOF)
    return false;

  COleVariant var;
  Strng       AStr;

  var=Recordset()->Fields->GetItem(StrSpName)->Value;                           
  if (var.vt==VT_R4)                                             
    m_SpName.SetWindowText((AStr.Set("%.3f",V_R4(&var)), AStr()));    
  else                                                           
    m_SpName.SetWindowText((var.vt!=VT_NULL) ? OLE2T(V_BSTR(&var)) : "");

  GET(m_SpName   , StrSpName  );
  GET(m_Compound , StrCompound);
  GET(m_SpDefn   , StrSpDefn  );
  GET(m_Phase    , StrPhase   );

  var=Recordset()->Fields->GetItem(StrOcc)->Value;
  Strng Occ((var.vt!=VT_NULL) ? OLE2T(V_BSTR(&var)) : "?");
  GasOccBtn().SetCheck(Occ.XStrICmp("g")==0);
  LiqOccBtn().SetCheck(Occ.XStrICmp("l")==0);
  SolOccBtn().SetCheck(Occ.XStrICmp("s")==0);

  GET(m_Checked   , StrChecked );
  GET(m_Ts        , StrTs      );
  GET(m_Te        , StrTe      );
  GET(m_Rho       , StrRho     );
  GET(m_dHf       , StrdHf     );
  GET(m_S         , StrS       );
  GET(m_Cp        , StrCp      );
  GET(m_Vp        , StrVp      );
  GET(m_Pc        , StrPc      );
  GET(m_Tc        , StrTc      );
  GET(m_Vc        , StrVc      );
  GET(m_Ac        , StrAc      );
  GET(m_Reference , StrReference);
  return true;
  }

// --------------------------------------------------------------------------

#define UPDS(Fld, Nm, Chg1, Str, Flt)                                   \
      {                                                                 \
      if (!m_bDoingAdd)                                                 \
        {                                                               \
        var=Recordset()->Fields->GetItem(Nm)->Value;                    \
        switch (var.vt)                                                 \
          {                                                             \
          case VT_R4:                                                   \
            StrO.Format("%.3f",V_R4(&var));                             \
            break;                                                      \
          default:                                                      \
            StrO=(var.vt!=VT_NULL) ? OLE2T(V_BSTR(&var)) : "";          \
            break;                                                      \
          }                                                             \
        }                                                               \
      if (m_bDoingAdd || Str!=StrO)                                     \
        {                                                               \
        Chgd=true;                                                      \
        Chg1=true;                                                      \
        if (Str.GetLength()>0)                                          \
          {                                                             \
          if (Flt)                                                      \
            {                                                           \
            var=Max(50.0F, (float)SafeAtoF((char*)(const char*)Str, 298.16));  \
            }                                                           \
          else                                                          \
            var.SetString((const char*)Str, VT_BSTR);                   \
          Recordset()->Fields->GetItem(Nm)->Value=var;                  \
          }                                                             \
        else                                                            \
          {                                                             \
          _variant_t nvar;                                              \
          if (!Flt)                                                     \
            nvar=T2BSTR("");                                            \
          Recordset()->Fields->GetItem(Nm)->Value=nvar;                 \
          }                                                             \
        }                                                               \
      if (1)                                                            \
        {                                                               \
        _variant_t xxx=Recordset()->Fields->GetItem(Nm)->Value;         \
        if (xxx.vt==VT_BSTR)                                            \
          dbgpln("%s = %s (%s)", Nm, OLE2CT(V_BSTR(&xxx)), Str);        \
        }                                                               \
      }

#define UPD(Fld, Nm, Chg1, flt)                                         \
      Fld.GetWindowText(Str);                                           \
      UPDS(Fld, Nm, Chg1, Str, flt)

// --------------------------------------------------------------------------

BOOL CSDBSpecie::UpdateRecord(flag ForceIt)//int ItemIndex)
  {
  USES_CONVERSION;
  if (1)//Recordset()->CanUpdate())
    {
    try
      {
      if (m_bDoingAdd)
        {
        if (!m_bAddBusy)
          Recordset()->AddNew();
        m_bAddBusy=true;
        }
      else
        {
        //Recordset()->Edit();
        }
      int Chgd=m_bDoingAdd, OChgd, CChgd=false, PChgd=false;
      CString Str, StrO;
      COleVariant var;
      UPD(m_SpName,   StrSpName  , OChgd, false)
      UPD(m_Compound, StrCompound, CChgd, false)
      UPD(m_SpDefn,   StrSpDefn  , OChgd, false)
      UPD(m_Phase,    StrPhase   , PChgd, false)

      if (GasOccBtn().GetCheck())
        Str="g";
      else if (LiqOccBtn().GetCheck())
        Str="l";
      else
        Str="s";
      UPDS(m_Phase,   StrOcc     , OChgd, Str, false)
      UPD(m_Checked,  StrChecked , OChgd, false)
      UPD(m_Ts,       StrTs      , OChgd, true)
      UPD(m_Te,       StrTe      , OChgd, true)
      UPD(m_Rho,      StrRho     , OChgd, false)
      UPD(m_dHf,      StrdHf     , OChgd, false)
      UPD(m_S,        StrS       , OChgd, false)
      UPD(m_Cp,       StrCp      , OChgd, false)
      UPD(m_Vp,       StrVp      , OChgd, false)
      UPD(m_Pc,       StrPc      , OChgd, false)
      UPD(m_Tc,       StrTc      , OChgd, false)
      UPD(m_Vc,       StrVc      , OChgd, false)
      UPD(m_Ac,       StrAc      , OChgd, false)
      UPD(m_Reference, StrReference, OChgd, false);

      m_Compound.GetWindowText(Str);
      m_CompPhaseUpd=(const char*)Str;
      m_Phase.GetWindowText(Str);
      m_CompPhaseUpd+="(";
      m_CompPhaseUpd+=(const char*)Str;
      m_CompPhaseUpd+=")";

      ForceIt=true;
      if (Chgd && (ForceIt || (AfxMessageBox("Save Changes", MB_ICONQUESTION|MB_YESNO)==IDYES)))
        {
        Recordset()->Update();
        //Recordset()->Filter="";
        //Recordset()->MoveFirst();
        m_bAddBusy=false;
        }

      //m_bDoingAdd=false;
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Update Record");

      return false;
      }
    }

  return true;
  }

// --------------------------------------------------------------------------

#define IMPS(Nm, Str)                               \
  {                                                 \
  if (Str())                                        \
    {                                               \
    var=T2BSTR(Str());                              \
    Recordset()->Fields->GetItem(Nm)->Value=var;    \
    }                                               \
  else                                              \
    {                                               \
    var=T2BSTR("");                                 \
    Recordset()->Fields->GetItem(Nm)->Value=var;    \
    /*_variant_t nvar;*/                                \
    /*Recordset()->Fields->GetItem(Nm)->Value=nvar;*/   \
    }                                               \
  }

// --------------------------------------------------------------------------

#define IMPSD(Nm, Str)                              \
  {                                                 \
  if (Str.Length()>0)                               \
    {                                               \
    double D=atof(Str());                           \
    var=D;                                          \
    Recordset()->Fields->GetItem(Nm)->Value=var;    \
    }                                               \
  else                                              \
    {                                               \
    _variant_t nvar;                                \
    Recordset()->Fields->GetItem(Nm)->Value=nvar;   \
    }                                               \
  }

// --------------------------------------------------------------------------

#define IMPD(Nm, D)                                 \
  {                                                 \
    {                                               \
    _variant_t nvar(D);                             \
    Recordset()->Fields->GetItem(Nm)->Value=nvar;   \
    }                                               \
  }

// --------------------------------------------------------------------------

BOOL CSDBSpecie::ImportRecords(CArray<HSCInfo*, HSCInfo*> & InfoArray)
  {
  if (1)//Recordset()->CanUpdate())
    {
    try
      {
      for (int i=0; i<InfoArray.GetCount(); i++)
        {
        HSCInfo & Info=*InfoArray[i];
        Recordset()->AddNew();
        Strng NullStr;
        Strng Str("???");
        COleVariant var;
        IMPS(StrSpName,   Info.Name)
        IMPS(StrCompound, Info.Formula)
        IMPS(StrSpDefn,   Info.Defn)
        IMPS(StrPhase,    Info.Phase)
        IMPS(StrOcc,      Info.Occurence)

        IMPS(StrChecked,  NullStr);//Info.Reference)
        IMPD(StrTs,       Info.m_T1)
        IMPD(StrTe,       Info.m_T2)
        IMPS(StrRho,      (Str.Set(Info.Density>1.0e-6?"%.3f":"", Info.Density), Str))
        IMPS(StrdHf,      (Str.Set("%.3f", Info.H), Str))
        IMPS(StrS,        (Str.Set("%.3f", Info.S), Str))
        IMPS(StrCp,       (Str.Set("HSC_Cp(%.3f,%.3f,%.3f,%.3f)", Info.A, Info.B, Info.C, Info.D), Str))
        IMPS(StrVp,       NullStr)
        IMPS(StrPc,       NullStr)
        IMPS(StrTc,       NullStr)
        IMPS(StrVc,       NullStr)
        IMPS(StrAc,       NullStr)
        IMPS(StrReference, NullStr)

        m_CompPhaseUpd=Info.Formula;
        m_CompPhaseUpd+="(";
        m_CompPhaseUpd+=Info.Phase;
        m_CompPhaseUpd+=")";

        dbgpln("  Import [%6.1f->%6.1f] %15.6f %15.6f %s %s",  Info.m_T1, Info.m_T2, Info.H, Info.m_Hx, Info.Formula(), Info.Phase());
        Recordset()->Update();
        }
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Import Record");
      return false;
      }
    }

  return true;
  }

// --------------------------------------------------------------------------

BOOL CSDBSpecie::ImportRecord(SDBInfo &Info)
  {
  if (1)//Recordset()->CanUpdate())
    {
    try
      {
      Recordset()->AddNew();
      Strng NullStr;
      Strng Str("???");
      COleVariant var;

      IMPS(StrSpName,   Info.Name)
      IMPS(StrCompound, Info.Compound)
      IMPS(StrSpDefn,   Info.Definition)
      IMPS(StrPhase,    Info.Phase)
      IMPS(StrOcc,      Info.Occurence)

      IMPS(StrChecked,  Info.Checked)
      IMPSD(StrTs,      Info.Ts)
      IMPSD(StrTe,      Info.Te)
      IMPS(StrRho,      Info.Rho)
      IMPS(StrdHf,      Info.dHf)
      IMPS(StrS,        Info.S)
      IMPS(StrCp,       Info.Cp)
      IMPS(StrVp,       Info.Vp)
      IMPS(StrPc,       Info.Pc)
      IMPS(StrTc,       Info.Tc)
      IMPS(StrVc,       Info.Vc)
      IMPS(StrAc,       Info.Ac)
      IMPS(StrReference, Info.Reference)

      m_CompPhaseUpd=Info.Compound;
      m_CompPhaseUpd+="(";
      m_CompPhaseUpd+=Info.Phase;
      m_CompPhaseUpd+=")";

      Recordset()->Update();
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Import Record");
      return false;
      }
    }

  return true;
  }

// --------------------------------------------------------------------------

#define CLR(Fld, Nm)   Fld.SetWindowText("");

// --------------------------------------------------------------------------

void CSDBSpecie::ClearDisplay(int DoEnable)
  {
  CLR(m_SpName,   StrSpName  )
  CLR(m_Compound, StrCompound)
  CLR(m_SpDefn,   StrSpDefn  )
  CLR(m_Phase,    StrPhase   )
  GasOccBtn().SetCheck(false);
  LiqOccBtn().SetCheck(true);
  SolOccBtn().SetCheck(false);
  CLR(m_Checked,  StrChecked )
  CLR(m_Ts,       StrTs      )
  CLR(m_Te,       StrTe      )
  CLR(m_Rho,      StrRho     )
  CLR(m_dHf,      StrdHf     )
  CLR(m_S,        StrS       )
  CLR(m_Cp,       StrCp      )
  CLR(m_Vp,       StrVp      )
  CLR(m_Pc,       StrPc      )
  CLR(m_Tc,       StrTc      )
  CLR(m_Vc,       StrVc      )
  CLR(m_Ac,       StrAc      )
  CLR(m_Reference, StrReference)
  if (DoEnable>=0)
    {
    flag Enabled=DoEnable>0 && !ReadOnly();
    m_SpList.EnableWindow(Enabled);
    m_SpName.EnableWindow(Enabled);
    m_Compound.EnableWindow(Enabled);
    m_SpDefn.EnableWindow(Enabled);
    m_Phase.EnableWindow(Enabled);
    GasOccBtn().EnableWindow(Enabled);
    LiqOccBtn().EnableWindow(Enabled);
    SolOccBtn().EnableWindow(Enabled);
    m_Checked.EnableWindow(Enabled);
    m_Ts.EnableWindow(Enabled);
    m_Te.EnableWindow(Enabled);
    m_Rho.EnableWindow(Enabled);
    m_dHf.EnableWindow(Enabled);
    m_S.EnableWindow(Enabled);
    m_Cp.EnableWindow(Enabled);
    m_Vp.EnableWindow(Enabled);
    m_Pc.EnableWindow(Enabled);
    m_Tc.EnableWindow(Enabled);
    m_Vc.EnableWindow(Enabled);
    m_Ac.EnableWindow(Enabled);
    m_Reference.EnableWindow(Enabled);
    }
  }

// --------------------------------------------------------------------------

BOOL CSDBSpecie::DeleteRecords()
  {
  ClearDisplay();

  Strng Fnd, Name, X1, X2;
  POSITION pos = m_SpList.GetFirstSelectedItemPosition();
  while (pos)
    {
    int RowIndex= m_SpList.GetNextSelectedItem(pos);
    Strng Fnd, Name, X1, X2;
    X1.Set("%.3f", Rows()[RowIndex].Ts-0.1);
    X2.Set("%.3f", Rows()[RowIndex].Ts+0.1);
    Fnd.Set("[Compound] = '%s' AND [Phase] = '%s' AND [Ts] >= %s AND [Ts] <= %s",
             Rows()[RowIndex].Comp(), Rows()[RowIndex].Occ(), X1(), X2());
    try
      {
      Recordset()->Filter=Fnd();
      Recordset()->MoveFirst();
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Find Record for delete");
      return false;
      }

    if (Recordset()->BOF)
      return false;

    try
      {
      Recordset()->Delete(ADODB::adAffectCurrent);
      Recordset()->Filter="";
      Recordset()->MoveFirst();
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Delete Record");
      return false;
      }
    }

  return true;
  }

// ==========================================================================
//
//
//
// ==========================================================================

int TestPos(void * p, void * q) { return ((DWORD)p)<((DWORD)q); };

BOOL CSDBSpecie::ImportSpecies()
  {
  Strng s;

  s=BaseCfgFiles();
  s+=BCfgDBFileName();
  s=ScdPFUser.RdStr("ImportSpecies", "File", s());

  CSCDFileDialog Dlg(true, NULL, s(), OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_HIDEREADONLY, 
                    "SysCAD Database(*.mdb)|*.mdb|HSC Databases (*.hsc)|*.hsc||");
  //Dlg.m_ofn.lpstrInitialDir = "C:\\HSC3\\";
  Dlg.m_ofn.lpstrTitle = "Import Species from Database";

  // What was this here for ... CNM
  //if (Dlg.DoModal()==IDOK)
  //  {
  //  ScdPFUser.WrStr("ImportSpecies", "File", Dlg.GetPathName());
  //  CSCDDatabase::DoSDBEdit(Dlg.GetPathName());
  //  }


  flag OK=false;
  if (Dlg.DoModal()==IDOK)
    {
    ScdPFUser.WrStr("ImportSpecies", "File", Dlg.GetPathName());
    // Repaint
    RedrawWindow();
    Strng Fn((char*)(const char*)Dlg.GetPathName());
    Strng Ext;
    Ext.FnExt(Fn());

    if (Ext.XStrICmp(".hsc")==0)
      {
      CHSCSlct HSCSlct((char*)(const char*)Dlg.GetPathName());
      int nRet = HSCSlct.DoModal();
      if (nRet==IDOK)
        {
        int N=HSCSlct.GetSelectedCount();
        if (N>0)
          {
          CArray <HSCInfo*, HSCInfo*> InfoArray;
          HSCSlct.GetSelectedInfo(0, N, InfoArray);
          if (ImportRecords(InfoArray))
            OK=true;

          }
        }
      }
    else if (Ext.XStrICmp(".mdb")==0)
      {
      if (_stricmp((LPCTSTR)Filename(), Fn())==0)
        {
        LogWarning("MDB Import", LF_Exclamation, "Cannot select file to import data to itself!");
        }
      else
        {
        CSDBSlct SDBSlct((char*)(const char*)Dlg.GetPathName());
        int nRet = SDBSlct.DoModal();
        if (nRet==IDOK)
          {
          int N=SDBSlct.GetSelectedCount();
          if (N>0)
            {
            SDBInfo Info;
            //Sort
            HpSort(N, (void**)SDBSlct.GetPositionArray(), TestPos);
            for (int i=0; i<N; i++)
              {
              if (SDBSlct.GetSelectedInfo(i, Info))
                if (ImportRecord(Info))
                  OK=true;
              }      
            }
          }
        }
      }
    }

  return OK;
  }

/////////////////////////////////////////////////////////////////////////////
// CHSCSlct dialog


CHSCSlct::CHSCSlct(char * Filename, CWnd* pParent /*=NULL*/)
	: CDialog(CHSCSlct::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CHSCSlct)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_Filename=Filename;
  m_hFile=INVALID_HANDLE_VALUE;
  m_EDB.Init();
  }

// --------------------------------------------------------------------------

CHSCSlct::~CHSCSlct()
  {
  for (int iInfo=0; iInfo<m_InfoArray.GetSize(); iInfo++)
    delete m_InfoArray[iInfo];

  if (m_hFile!=INVALID_HANDLE_VALUE)
    CloseHandle(m_hFile);
  m_EDB.Term();
  }

// --------------------------------------------------------------------------

void CHSCSlct::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHSCSlct)
	DDX_Control(pDX, IDC_SPLIST, m_List);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CHSCSlct, CDialog)
	//{{AFX_MSG_MAP(CHSCSlct)
	ON_BN_CLICKED(IDB_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSCSlct message handlers

BOOL CHSCSlct::OnInitDialog()
  {
	CDialog::OnInitDialog();
  CWaitCursor Curs;

  //TODO Test and allow import of different formats/versions of HSC
  m_hFile=CreateFile(m_Filename(),
                      GENERIC_READ, FILE_SHARE_READ, NULL,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
  flag Ok = (m_hFile!=INVALID_HANDLE_VALUE);
  if (Ok)
    {//test HSC file format/version...
    const dword Sz = GetFileSize(m_hFile, NULL);
    if ((Sz % sizeof(HSCData))!=0)
      {
      LogWarning("HSC Import", LF_Exclamation, "Format of HSC file old or not recognised\n'%s'", m_Filename());
      Ok = false;
      }
    }
  else
    {
    LogWarning("HSC Import", LF_Exclamation, "Cannot Open file \n'%s'\n%s", m_Filename(), FindWinError(GetLastError()));
    }
  if (Ok)
    {
    int FormLen=10;
    int NameLen=10;
    HSCData Data;
    DWORD NBytesRead;

    int iPhase=0;
    Strng sPrevFormula;
    m_InfoArray.SetSize(0, 1024);
    HSCInfo * pPrevInfo=NULL;
    int iRecNo=0;
    while (ReadFile(m_hFile, &Data, sizeof(Data), &NBytesRead, NULL) && (NBytesRead==sizeof(Data)))
      {
      HSCInfo * pInfo=new HSCInfo;
      pInfo->CopyData(Data, true);//, SumH);
      pInfo->UpdateA(iRecNo++, iPhase, sPrevFormula, pPrevInfo);
      if (pInfo->IsValid)
        {
        pInfo->m_iInfoIndex=m_InfoArray.GetCount();
        m_InfoArray.Add(pInfo);
        pPrevInfo=pInfo;
        }
      }

    CompleteUpdate(m_InfoArray, dbgDumpAll);
  
    for (int iInfo=0; iInfo<m_InfoArray.GetSize(); iInfo++)
      {
      HSCInfo &Info=*m_InfoArray[iInfo];
      if (Info.Valid())
        {
        Strng S,X;
        X.Set("%s(%s)", Info.Formula(),Info.Phase());
        S.Set("%s\t%8.2f\t%8.2f\t%s", X(), Info.m_T1, Info.m_T2, Info.Name()?Info.Name():"");
        S.LTrim();
        FormLen=Max(FormLen, X.Length());
        NameLen=Max(NameLen, Info.Name.Length());
        int Pos=m_List.AddString(S());
        if (Pos!=LB_ERR && Pos!=LB_ERRSPACE)
          {
          if (m_List.SetItemData(Pos, (DWORD)m_InfoArray[iInfo])==LB_ERR)
            {
            int xxx=0;
            };
          }
        }
      }

    CRect Rect;
    Rect.SetRect(0,0,4,8);
    MapDialogRect(&Rect);

    m_List.SetHeaderCount(4);
    m_List.SetHeaderItem(0, "Formula",  Rect.Width()*FormLen);
    m_List.SetHeaderItem(1, "T1 (K)",   Rect.Width()*9);
    m_List.SetHeaderItem(2, "T2 (K)",   Rect.Width()*9);
    m_List.SetHeaderItem(3, "Name",     Rect.Width()*NameLen);
    m_List.MakeHeader(34763);

    }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

// --------------------------------------------------------------------------

void CHSCSlct::CompleteUpdate(CArray<HSCInfo*,HSCInfo*> & InfoArray, bool DoDump)
  {

  for (int i=0; i<InfoArray.GetSize(); )
    {
    HSCInfo &Infoi=*InfoArray[i];
    Infoi.m_iPhaseInd=Infoi.m_iPhase;
    if (Infoi.m_FormatAOK)
      {
      int n=1;
      Infoi.m_iPhaseInd=n-1;//Infoj.m_iPhase;
      for (int j=i+1; j<InfoArray.GetSize(); j++)
        {
        HSCInfo &Infoj=*InfoArray[j];
        Infoj.m_iPhaseInd=n;//Infoj.m_iPhase;
        if (Infoj.Formula==Infoi.Formula && Infoj.Occurence==Infoi.Occurence)
          n++;
        else
          break;
        }
      if (n==1)
        Infoi.m_iPhaseInd=-1;

      i+=n;
      }
    else
      i++;
    }

  if (DoDump)
    dbgpln("-----------------------------------------------");

  for (int iInfo=0; iInfo<InfoArray.GetSize(); iInfo++)
    {
    HSCInfo &Info=*InfoArray[iInfo];
    Info.UpdateB(m_EDB, InfoArray, iInfo);
    if (DoDump)
      {
      dbgpln("%3.3s %s %s %s %2i %2i %-25.25s %-8.8s %-3.3s %8.2f %8.2f %-30.30s %8.2f %15.2f %8.2f %8.2f %8.2f %8.2f %12.5f %-55.55s %-33.33s",
        Info.Valid()?"":"BAD",
        Info.m_FormatA?"A":" ",
        Info.m_FormatAOK?"OK":"  ",
        Info.m_FormatAOK && Info.m_iPhase>0?"Xfrm":"    ",
        Info.m_iPhase,
        Info.m_iPhaseInd,
        Info.FormulaRaw(), 
        //Info.Formula(), 
        Info.Phase(), Info.Occurence(), Info.m_T1, Info.m_T2, Info.Defn(),
        Info.S, Info.H, Info.A, Info.B, Info.C, Info.D, Info.Density,
        Info.Name()?Info.Name():"", Info.StructuralFormula()?Info.StructuralFormula():"");
      }
    }

  if (DoDump)
    dbgpln("-----------------------------------------------");

  }

// --------------------------------------------------------------------------

int CHSCSlct::GetSelectedCount()
  {
  return m_InfoSelected.GetSize();
  };

// --------------------------------------------------------------------------

flag CHSCSlct::GetSelectedInfo(int No1, int Count, CArray <HSCInfo*, HSCInfo*> & InfoArray)
  {
  for (int i=0; i<m_InfoArray.GetSize(); i++)
    m_InfoArray[i]->m_Selected=false;

  for (int i=No1; i<No1+Count; i++)
    {
    HSCInfo * pInfo = m_InfoSelected[i];
    if (pInfo->IsValid)
      {
      pInfo->m_Selected=true;
      int iPrevPhase=pInfo->m_iPhase;
      int j;
      for (j=pInfo->m_iInfoIndex-1 ; j>=0 && m_InfoArray[j]->m_iPhase>=0; j--)
        m_InfoArray[j]->m_Selected=true;
      for (j=pInfo->m_iInfoIndex+1 ; j<m_InfoArray.GetSize() && m_InfoArray[j]->m_iPhase>iPrevPhase; j++)
        {
        m_InfoArray[j]->m_Selected=true;
        iPrevPhase=m_InfoArray[j]->m_iPhase;
        }
      }
    else 
      { ASSERT(FALSE); /*return false;*/ }
    }

  InfoArray.SetSize(0, 1024);
  for (int i=0; i<m_InfoArray.GetSize(); i++)
    {
    HSCInfo * pInfo = m_InfoArray[i];
    if (pInfo->m_Selected)
      InfoArray.Add(pInfo);
    }

  CompleteUpdate(InfoArray, dbgDumpIns);

  return true;
  };

// --------------------------------------------------------------------------

void CHSCSlct::OnOK()
  {
  int N=m_List.GetSelCount();
  int M=m_InfoSelected.GetSize();
  CArray <int, int&> iIndices;
  iIndices.SetSize(N);
  m_InfoSelected.SetSize(M+N);
  if (N>0)
    {
    int NSel=m_List.GetSelItems(N, &iIndices[0]);
    ASSERT(NSel==N);
    for (int i=0; i<N; i++)
      m_InfoSelected[M+i]=(HSCInfo*)m_List.GetItemData(iIndices[i]);
    }	


  CDialog::OnOK();
  }

// --------------------------------------------------------------------------

void CHSCSlct::OnImport()
  {
  int N=m_List.GetSelCount();
  int M=m_InfoSelected.GetSize();
  CArray <int, int&> iIndices;
  iIndices.SetSize(N);
  m_InfoSelected.SetSize(M+N);
  if (N>0)
    {
    int NSel=m_List.GetSelItems(N, &iIndices[0]);
    ASSERT(NSel==N);
    for (int i=0; i<N; i++)
      {
      m_InfoSelected[M+i]=(HSCInfo*)m_List.GetItemData(iIndices[i]);
      m_List.SetSel(iIndices[i],false);
      ASSERT_ALWAYS((i<1 || iIndices[i]>iIndices[i-1]), "BAD SPECIE IMPORT LIST ORDER", __FILE__, __LINE__);
      }
    for (int i=N-1; i>=0; i--)
      m_List.DeleteString(iIndices[i]);
    }	
  }

/////////////////////////////////////////////////////////////////////////////
// CSDBSlct dialog


CSDBSlct::CSDBSlct(char * Filename, CWnd* pParent /*=NULL*/)
	: CDialog(CSDBSlct::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CSDBSlct)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_Filename=Filename;
  m_hFile=INVALID_HANDLE_VALUE;
  m_EDB.Init();
  pDB=NULL;
  }

// --------------------------------------------------------------------------

CSDBSlct::~CSDBSlct()
  {
  if (m_hFile!=INVALID_HANDLE_VALUE)
    CloseHandle(m_hFile);
  m_EDB.Term();
  if (pDB)
    {
    pDB->Close();
    delete pDB;
    }
  }

// --------------------------------------------------------------------------

void CSDBSlct::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDBSlct)
	DDX_Control(pDX, IDC_SPLIST, m_List);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSDBSlct, CDialog)
	//{{AFX_MSG_MAP(CSDBSlct)
	ON_BN_CLICKED(IDB_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDBSlct message handlers

BOOL CSDBSlct::OnInitDialog()
  {
	CDialog::OnInitDialog();
  CWaitCursor Curs;

  int FormLen=10;
  int NameLen=10;
  Strng PrevFormula;


  CFileStatus State;
  if (CFile::GetStatus(m_Filename(), State))
    {
    //NBNB: The '°' in 'S°298' causes problems with other language utilities (eg chinese characters)
    const int iOC_S298 = 10; //index of S298 in columns
    char* SpColNames[] = { "Name", "Compound", "Definition", "Phase", "Occurence",
      "Checked", "Ts", "Te", "Rho", "dHf", "S°298",
      "Cp", "Vp", "Pc", "Tc", "Vc", "Ac", NULL };
    char* SpColNames2[] = { "Name", "Compound", "Definition", "Phase", "Occurence",
      "Checked", "Ts", "Te", "Rho", "dHf", "S298",
      "Cp", "Vp", "Pc", "Tc", "Vc", "Ac", NULL };
    //CDBHelper DB(m_Filename(), "Species");
    pDB=new CDBHelper(m_Filename(), "Species");
    if (pDB->Open(DBConnect_Default, SpColNames[0], DBH_HasHeadings|DBH_ReadOnly|/*DBH_ForwardOnly|*/DBH_TrimStrings))
      {
      BOOL ColNamesOK = pDB->CheckFieldNames(SpColNames);
      if (!ColNamesOK && pDB->FieldIndex(iOC_S298)<0)
        {//problem with S°298, try alternate field names
        ColNamesOK = pDB->CheckFieldNames(SpColNames2);
        }
      if (ColNamesOK)
        {
        char* OptColNames[] = { "MolecularDiam", "MolarVol", "CpCv", "Reference", NULL };
        pDB->CheckOptionalFieldNames(OptColNames);
        iOC_MolecularDiam = (pDB->FieldIndex(pDB->NFieldIndexes()-4)<0 ? -1 : pDB->NFieldIndexes()-4);
        iOC_MolVol        = (pDB->FieldIndex(pDB->NFieldIndexes()-3)<0 ? -1 : pDB->NFieldIndexes()-3);
        iOC_CpCv          = (pDB->FieldIndex(pDB->NFieldIndexes()-2)<0 ? -1 : pDB->NFieldIndexes()-2);
        iOC_Reference     = (pDB->FieldIndex(pDB->NFieldIndexes()-1)<0 ? -1 : pDB->NFieldIndexes()-1);

        CStringArray Values;
        flag DataReqd=0, EndFound=0, AppendComp=0, PrevWasLoaded=0;
        CComponent * pC=NULL;
        while (!pDB->IsEOF())
          {
          //dwRecNo=pDB->GetLineNo();
          pDB->GetNextRow(Values);
          int i= pDB->GetLineNo();
          SDBInfo Info;
          Info.CopyData(Values, iOC_MolecularDiam, iOC_MolVol, iOC_CpCv, iOC_Reference); //,  true, SumH);
          Info.Update(m_EDB);
          #if dbgDumpAll
          dbgpln("%3.3s %-25.25s %-3.3s %8.8s %8.8s %-30.30s %8.8s %8.8s %8.8s %-55.55s",
                 Info.Valid()?"":"BAD",
                 Info.Compound(), Info.Occurence(), Info.Ts(), Info.Te(), Info.Definition(),
                 Info.S(), Info.dHf(), Info.Cp(),
                 Info.Name()?Info.Name():"");
          #endif
          if (Info.Valid())
            {
            Strng S,X;
            X.Set("%s(%s)", Info.Compound(),Info.Phase());
            S.Set("%s\t%8.2f\t%8.2f\t%s", X(), Info.Ts.SafeAtoF(), Info.Te.SafeAtoF(), Info.Name()?Info.Name():"");
            S.LTrim();
            FormLen=Max(FormLen, X.Length());
            NameLen=Max(NameLen, Info.Name.Length());
            int Pos=m_List.AddString(S());

            if (Pos!=LB_ERR && Pos!=LB_ERRSPACE)
              {
              if (m_List.SetItemData(Pos, pDB->GetLineNo())==LB_ERR)
                {
                int xxx=0;
                };
              }
            }
          }
        }
      else
        {
        if (pDB->FieldIndex(iOC_S298)<0)
          LogError("Specie Import", 0, "Try changing field name 'S°298' to 'S298' in table 'Species' in '%s'", m_Filename());
        else
          LogError("Specie Import", 0, "Incorrect field names: Table 'Species' in '%s'", m_Filename());
        //Ok = false;
        }
      pDB->MoveFirst();
      }

    CRect Rect;
    Rect.SetRect(0,0,4,8);
    MapDialogRect(&Rect);

    m_List.SetHeaderCount(4);
    m_List.SetHeaderItem(0, "Formula",  Rect.Width()*FormLen);
    m_List.SetHeaderItem(1, "T1 (K)",   Rect.Width()*9);
    m_List.SetHeaderItem(2, "T2 (K)",   Rect.Width()*9);
    m_List.SetHeaderItem(3, "Name",     Rect.Width()*NameLen);
    m_List.MakeHeader(34763);

    }
  else
    {
    LogWarning("MDB Import", LF_Exclamation, "Cannot Open file \n'%s'\n%s", m_Filename(), FindWinError(GetLastError()));
    }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

// --------------------------------------------------------------------------

int CSDBSlct::GetSelectedCount()
  {
  return dwPositions.GetSize();
  };

// --------------------------------------------------------------------------

flag CSDBSlct::GetSelectedInfo(int No, SDBInfo & Info)
  {
//  DWORD dwCurrentFilePosition=dwOffsets[No];
  long RecNo=dwPositions[No];
//  double SumH=0.0;
//  flag HIsForm=true;

//  HSCData Data;
  CStringArray Values;
  while (pDB->GetLineNo()<RecNo)
    pDB->GetNextRow(Values);

  ASSERT(pDB->GetLineNo()==RecNo);
    
  Info.CopyData(Values, iOC_MolecularDiam, iOC_MolVol, iOC_CpCv, iOC_Reference); //,  true, SumH);
    //Info.CopyData(Data, HIsForm, SumH);
    //HIsForm=false;
  //if (dwOffset==0)
  //  {
  Info.Update(m_EDB);
  #if dbgDumpIns
  dbgpln("%3.3s %-25.25s %-3.3s %8.8s %8.8s %-30.30s %8.8s %8.8s %8.8s %-55.55s",
         Info.Valid()?"":"BAD",
         Info.Compound(), Info.Occurence(), Info.Ts(), Info.Te(), Info.Definition(),
         Info.S(), Info.dHf(), Info.Cp(),
         Info.Name()?Info.Name():"");
  #endif
  return Info.Valid();
//  }
//    
////    }
//  return false;
  };

// --------------------------------------------------------------------------

void CSDBSlct::OnOK()
  {
  int N=m_List.GetSelCount();
  int M=dwPositions.GetSize();
  iIndices.SetSize(N);
  dwPositions.SetSize(M+N);
  if (N>0)
    {
    int NSel=m_List.GetSelItems(N, &iIndices[0]);
    ASSERT(NSel==N);
    for (int i=0; i<N; i++)
      dwPositions[M+i]=m_List.GetItemData(iIndices[i]);
    }	
	CDialog::OnOK();
  }

// --------------------------------------------------------------------------

void CSDBSlct::OnImport()
  {
  int N=m_List.GetSelCount();
  int M=dwPositions.GetSize();
  iIndices.SetSize(N);
  dwPositions.SetSize(M+N);
  if (N>0)
    {
    int NSel=m_List.GetSelItems(N, &iIndices[0]);
    ASSERT(NSel==N);
    for (int i=0; i<N; i++)
      {
      dwPositions[M+i]=m_List.GetItemData(iIndices[i]);
      m_List.SetSel(iIndices[i],false);
      }
    }	
  }

/////////////////////////////////////////////////////////////////////////////
// CSDBTable dialog


CSDBTable::CSDBTable(CSDBSheet * pSheet)//CWnd* pParent /*=NULL*/)
	: CPropertyPage(CSDBTable::IDD)//, pParent)
  {
	//{{AFX_DATA_INIT(CSDBTable)
	//}}AFX_DATA_INIT
  m_bDidInit=false;
  m_pSheet=pSheet;
  }


void CSDBTable::DoDataExchange(CDataExchange* pDX)
  {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDBTable)
	DDX_Control(pDX, IDC_SSDBGRIDCTRLAPT1, m_Grid);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSDBTable, CPropertyPage)
	//{{AFX_MSG_MAP(CSDBTable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSDBTable::OnInitDialog()
  {
	CPropertyPage::OnInitDialog();

  m_bDidInit=true;
  try
    {
    CWaitCursor W;
    CSSColumns Cols=m_Grid.GetColumns();
    //short g=-1;
    //float GrpW=0.0;

    Cols.Add(0);
    CSSColumn Col=Cols.GetItem(COleVariant(0L));
    Col.SetCaption("No");
    Col.SetAlignment(1);
    Col.SetWidth(4*8.0f);
    for (short f=0; SpFI[f].m_Name; f++)
      {
      //if (f==0 || f==2)
      //  {
      //  Grps.Add(++g);
      //  GrpW=0;
      //  }
      Cols.Add(f+1);
      //CSSGroup Grp=m_Grid.GetGroups().GetItem(COleVariant(g));
      CSSColumn Col=Cols.GetItem(COleVariant((long)(f+1)));
      SpFI[f].m_Pos=f;
      Col.SetCaption(SpFI[f].m_Caption);
      Col.SetAlignment(SpFI[f].m_Align);
      //Col.SetGroup(g);
      float W=SpFI[f].m_Wide*8.0f;
      Col.SetWidth(W);
      //GrpW+=W;
      //Grp.SetWidth(GrpW);
      }
    m_Grid.SetSplitterPos(2);

    }
  catch(_com_error & e)
    {
    DoCatch(e, "Building Dialog");
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }


void CSDBTable::OnCancel()
  {
	if (m_bDidInit)
	  CPropertyPage::OnCancel();
  }

void CSDBTable::OnOK()
  {
	if (m_bDidInit)
	  CPropertyPage::OnOK();
  }


BOOL CSDBTable::OnSetActive()
  {
  if (m_bDidInit)
    m_Grid.ReBind();
	return CPropertyPage::OnSetActive();
  }

BOOL CSDBTable::OnKillActive()
  {
  if (m_Grid.GetDataChanged())
    m_Grid.Update();
	return CPropertyPage::OnKillActive();
  }

BEGIN_EVENTSINK_MAP(CSDBTable, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CSDBTable)
	ON_EVENT(CSDBTable, IDC_SSDBGRIDCTRLAPT1, 1 /* InitColumnProps */, OnInitColumnPropsSsdbgridctrlapt1, VTS_NONE)
	ON_EVENT(CSDBTable, IDC_SSDBGRIDCTRLAPT1, 23 /* UnboundReadData */, OnUnboundReadDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT VTS_BOOL)
	ON_EVENT(CSDBTable, IDC_SSDBGRIDCTRLAPT1, 24 /* UnboundAddData */, OnUnboundAddDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CSDBTable, IDC_SSDBGRIDCTRLAPT1, 25 /* UnboundWriteData */, OnUnboundWriteDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CSDBTable, IDC_SSDBGRIDCTRLAPT1, 26 /* UnboundDeleteRow */, OnUnboundDeleteRowSsdbgridctrlapt1, VTS_PVARIANT)
	ON_EVENT(CSDBTable, IDC_SSDBGRIDCTRLAPT1, 27 /* UnboundPositionData */, OnUnboundPositionDataSsdbgridctrlapt1, VTS_PVARIANT VTS_I4 VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSDBTable::OnInitColumnPropsSsdbgridctrlapt1()
  {
	
  }

void CSDBTable::OnUnboundReadDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* StartLocation, BOOL ReadPriorRows)
  {
	USES_CONVERSION;
  long i;
	long r = 0;

	RowBufferDisp ssRowBuf;
  ssRowBuf.AttachDispatch(RowBuf, FALSE);

  if (Recordset()->BOF && Recordset()->adEOF)
    return;

  if( StartLocation->vt == VT_NULL)
	  {
    Recordset()->Filter="";
		if (ReadPriorRows)
			Recordset()->MoveLast();
		else
			Recordset()->MoveFirst();
	  }
	else
	  {	
		Recordset()->Bookmark=StartLocation; //->iVal);//->iVal);

		if (ReadPriorRows)
			Recordset()->MovePrevious();
		else
			Recordset()->MoveNext();
    }

	if (ssRowBuf)
	  {	
		const long num = ssRowBuf.GetRowCount();
		BOOL bDone = FALSE;  //to exit the loop

		for(i=0; i<num && !bDone;i++)
		  {	
		
			if(Recordset()->BOF || Recordset()->adEOF)
				bDone = TRUE;
			else
			  {
        long APos=Recordset()->GetAbsolutePosition();
  			COleVariant va = APos;
			  ssRowBuf.SetValue((short)i,(short)0,va);
        for (long j=0; SpFI[j].m_Name; j++)
				  {	
					COleVariant va = Recordset()->Fields->GetItem((long)SpFI[j].m_Pos)->Value;
					switch(va.vt)
					  {
					  case VT_BSTR:
						  {
						  ssRowBuf.SetValue((short)i,(short)j+1,va);
						  break;
						  }
            default:
              ssRowBuf.SetValue((short)i,(short)j+1,va);
              break;
					  }
				  }

        ssRowBuf.SetBookmark(i, Recordset()->GetBookmark());
				
				if(ReadPriorRows)
					Recordset()->MovePrevious();
				else
					Recordset()->MoveNext();

				r+=1;
			  }		
		  }
		
		ssRowBuf.SetRowCount(r);
    }
	}	

void CSDBTable::Buff2RecSet(LPDISPATCH RowBuf)
  {
	RowBufferDisp ssRowBuf;
  ssRowBuf.AttachDispatch(RowBuf, FALSE);
	long ct = Recordset()->Fields->Count;

	if (ssRowBuf)
	  {	
		for(short j=0;SpFI[j].m_Name;j++)
      {	
			COleVariant va = ssRowBuf.GetValue(0,j+1);
			switch(va.vt)
				{
				case VT_NULL:
          break;
				case VT_BSTR:
					{
 					CString s=V_BSTR(&va);
          if (s.GetLength()==0)
            s=" ";
					Recordset()->Fields->GetItem((long)SpFI[j].m_Pos)->Value=(const char*)s;
					break;
					}
        default:
          Recordset()->Fields->GetItem((long)SpFI[j].m_Pos)->Value=va;
          break;
				}
      }
    }
  }

void CSDBTable::OnUnboundAddDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* NewRowBookmark)
  {
  try
    {
    Recordset()->AddNew();
    Buff2RecSet(RowBuf);

    Recordset()->Update();
    Recordset()->MoveLast();
    *NewRowBookmark = Recordset()->GetBookmark();
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Adding Record");
    }
  }
void CSDBTable::OnUnboundWriteDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* WriteLocation)
  {
  try
    {
    Recordset()->Bookmark=*WriteLocation;
    //Recordset()->Edit();

    Buff2RecSet(RowBuf);
    Recordset()->Update();
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Updating Record");
    }
	}

void CSDBTable::OnUnboundDeleteRowSsdbgridctrlapt1(VARIANT FAR* Bookmark)
  {
  try
    {
    Recordset()->Bookmark=*Bookmark;
    Recordset()->Delete(ADODB::adAffectCurrent);
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Deleting Record");
    }
	}

void CSDBTable::OnUnboundPositionDataSsdbgridctrlapt1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation)
  {
	}

/////////////////////////////////////////////////////////////////////////////
// CSDBTest dialog

CSDBTest::CSDBTest(CSDBSheet * pSheet)//CWnd* pParent /*=NULL*/)
	: CPropertyPage(CSDBTest::IDD),//, pParent)
//  m_EDB,
//  m_SDB,
  m_CDB(true)
  {
	//{{AFX_DATA_INIT(CSDBTest)
	//}}AFX_DATA_INIT
  m_bDidInit=false;
  m_pSheet=pSheet;
  m_NIntervals=10;
  }


void CSDBTest::DoDataExchange(CDataExchange* pDX)
  {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDBTest)
	DDX_Control(pDX, IDC_INTERVALS, m_Intervals);
	DDX_Control(pDX, IDC_TAB2, m_Tab);
	DDX_Control(pDX, IDC_SHOWKG, m_ShowKg);
	DDX_Control(pDX, IDC_SHOWK, m_ShowK);
	DDX_Control(pDX, IDC_NOGOOD, m_NoGood);
	DDX_Control(pDX, IDC_NOBAD, m_NoBad);
	DDX_Control(pDX, IDC_SHOWALL, m_ShowAll);
	DDX_Control(pDX, IDC_SSDBGRIDCTRL1, m_ThermoGrid);
	DDX_Control(pDX, IDC_SSDBGRIDCTRL2, m_MsgGrid);
	DDX_Control(pDX, IDC_SSDBGRIDCTRL3, m_RelDensGrid);
	DDX_Control(pDX, IDC_SSDBGRIDCTRL4, m_SolubleGrid);
	DDX_Control(pDX, IDC_SSDBGRIDCTRL5, m_MixHeatGrid);
	//}}AFX_DATA_MAP
  }

BEGIN_MESSAGE_MAP(CSDBTest, CPropertyPage)
	//{{AFX_MSG_MAP(CSDBTest)
	ON_BN_CLICKED(ID_SP_DOTEST, OnSpDotest)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, OnSelchangeTab2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSDBTest::OnInitDialog()
  {
	CPropertyPage::OnInitDialog();

  ScdPFUser.RdInt("SCDEdit", "ShowK", 0);

  CRect Rct, PRct;
  GetWindowRect(&PRct);
  m_ThermoGrid.GetWindowRect(&Rct);
  m_RelDensGrid.MoveWindow(Rct.left-PRct.left, Rct.top-PRct.top, Rct.Width(), Rct.Height());
  m_SolubleGrid.MoveWindow(Rct.left-PRct.left, Rct.top-PRct.top, Rct.Width(), Rct.Height());
  m_MixHeatGrid.MoveWindow(Rct.left-PRct.left, Rct.top-PRct.top, Rct.Width(), Rct.Height());
  m_RelDensGrid.ShowWindow(SW_HIDE);
  m_SolubleGrid.ShowWindow(SW_HIDE);
  m_MixHeatGrid.ShowWindow(SW_HIDE);

  m_Grids.SetSize(5);
  m_Grids[0]=&m_ThermoGrid;
  m_Grids[1]=&m_RelDensGrid;
  m_Grids[2]=&m_SolubleGrid;
  m_Grids[3]=&m_MixHeatGrid;
  m_Grids[4]=&m_MsgGrid;

//typedef struct _TC_ITEM {
//    UINT mask;         
//    UINT lpReserved1;  // reserved; do not use
//    UINT lpReserved2;  // reserved; do not use
//    LPSTR pszText;     
//    int cchTextMax;    
//    int iImage;        
//    LPARAM lParam;     
//} TC_ITEM;
  TC_ITEM It0= {TCIF_TEXT ,0, 0, "Thermo",           0, 0, 0};
  TC_ITEM It1= {TCIF_TEXT ,0, 0, "Relative Density", 0, 0, 0};
  TC_ITEM It2= {TCIF_TEXT ,0, 0, "Solubility",       0, 0, 0};
  TC_ITEM It3= {TCIF_TEXT ,0, 0, "Heat of Mixing",   0, 0, 0};

  m_Tab.InsertItem(0, &It0);
  m_Tab.InsertItem(1, &It1);
  m_Tab.InsertItem(2, &It2);
  m_Tab.InsertItem(3, &It3);

  //m_Tab.InsertItem(0, "Thermo");
  //m_Tab.InsertItem(1, "Relative Density");
  //m_Tab.InsertItem(2, "Solubility");
  //m_Tab.InsertItem(3, "Heat of Mixing");
  m_bDidInit=true;
	m_NoBad.SetWindowText("");
	m_NoGood.SetWindowText("");
	m_ShowKg.SetCheck(ScdPFUser.RdInt("SCDEdit", "ShowKg", 0));
	m_ShowK.SetCheck(ScdPFUser.RdInt("SCDEdit", "ShowK", 0));
	m_ShowAll.SetCheck(ScdPFUser.RdInt("SCDEdit", "ShowAll", 0));

  Strng S;
  S.Set("%i", m_NIntervals);
  m_Intervals.SetWindowText(S());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }
void CSDBTest::OnCancel()
  {
	if (m_bDidInit)
	  CPropertyPage::OnCancel();
  }

void CSDBTest::OnOK()
  {
	if (m_bDidInit)
    {
    CPropertyPage::OnOK();
	  ScdPFUser.WrInt("SCDEdit", "ShowKg", m_ShowKg.GetCheck());
	  ScdPFUser.WrInt("SCDEdit", "ShowK", m_ShowK.GetCheck());
	  ScdPFUser.WrInt("SCDEdit", "ShowAll", m_ShowAll.GetCheck());
    CPropertyPage::OnOK();
    }
  }

void CSDBTest::OnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult)
  {
	// TODO: Add your control notification handler code here

  int sel=m_Tab.GetCurSel();
  m_ThermoGrid.ShowWindow((sel==0) ? SW_SHOW : SW_HIDE);
  m_RelDensGrid.ShowWindow((sel==1) ? SW_SHOW : SW_HIDE);
  m_SolubleGrid.ShowWindow((sel==2) ? SW_SHOW : SW_HIDE);
  m_MixHeatGrid.ShowWindow((sel==3) ? SW_SHOW : SW_HIDE);
	*pResult = 0;
  }

void CSDBTest::TestThermo(int &nGood, int &nBad)
  {
  Strng SBlnk(" "), S;
  CSSColumns Cols=m_ThermoGrid.GetColumns();
  CSSColumn Col1=Cols.GetItem(COleVariant(1L));
  CSSColumn Col3=Cols.GetItem(COleVariant(3L));
  CSSColumn Col4=Cols.GetItem(COleVariant(4L));
  Col1.SetCaption(m_ShowK.GetCheck() ? "Temp(K)" : "Temp(C)");
  Col3.SetCaption(m_ShowKg.GetCheck() ? "H(kJ/kg)" : "H(kJ/kmol)");
  Col4.SetCaption(m_ShowKg.GetCheck() ? "Cp(kJ/kg.K)" : "Cp(kJ/kmol.K)");

  long row=0;
  for (long iFidelity=0; iFidelity<2; iFidelity++)
    {
    //bool Fidelity=(iFidelity==1);
    for (int s=0; s<m_SDB.m_iNo; s++)
      {
      for (int Pass=(m_ShowAll.GetCheck() ? 1 : 0); Pass<2; Pass++)
        {
        if (Pass>0 && row>0)
          m_ThermoGrid.AddItem(SBlnk(), COleVariant(row++));
        //int iDt=10;
        double T1=m_SDB[s].LoT(iFidelity);
        double T2=m_SDB[s].HiT(iFidelity);
        double dt=(T2-T1)/m_NIntervals;
        double t=T1-dt;
        double PrevH=-1e100;

        Strng Note;
        flag fBad=false;
        for (int it=0; it<=m_NIntervals; it++)
          {
          t+=dt;
          Strng S, VpS;
          double vp=m_SDB[s].VapourP(iFidelity, t);
          VpS.Set(vp<1.0e5 ? "%.3f":"%.3e", vp/1000);
          double H=m_ShowKg.GetCheck() ? m_SDB[s].msHz(iFidelity, t, Std_P, NULL, NULL) : m_SDB[s].mlHz(iFidelity, t, Std_P, NULL, NULL);
          double Cp=m_ShowKg.GetCheck() ? m_SDB[s].msCp(iFidelity, t, Std_P, NULL, NULL) : m_SDB[s].mlCp(iFidelity, t, Std_P, NULL, NULL);

          Strng Hs(" "), Cps(" ");
      //    if (!m_SDB[s].IsAqueous())
            {
            Hs.Set("%.1f", H);
            Cps.Set("%.4f", Cp);
            }

          Note="";
          if (!m_SDB[s].IsAqueous() && (H<PrevH+1.0e-6))
            Note="Enthalpy Drops";
          double Rho=m_SDB[s].SpRho(iFidelity, t,Std_P);
          if (Rho<1.0e-6)
            {
            if (Note())
              Note+="/";
            Note+="Bad Density";
            }
          if (Pass>0)
            {
            S.Set("%s\t%.1f\t%i\t%s\t%s\t%s\t%.4f\t%s\t%i",
              it==0 ? (m_SDB[s].SymOrTag()?m_SDB[s].SymOrTag():""):"",
                  m_ShowK.GetCheck()?t:K_2_C(t),
                  m_SDB[s].FindTIndex(iFidelity, t),
                  Hs(), Cps(), VpS(), Rho, Note() ? Note() : " ", s);
            m_ThermoGrid.AddItem(S(), COleVariant(row++));
            }
          fBad=fBad || (Note()!=NULL);
          PrevH=H;
          }
        if (Pass>0)
          if (fBad)
            nBad++;
          else
            nGood++;
        if (m_ShowAll.GetCheck()==0 && !fBad)
          break;
        }
      }
    }
  }

void CSDBTest::TestRelDens(int &nGood, int &nBad)
  {
  Strng SBlnk(" "), S;
  CSSColumns Cols=m_RelDensGrid.GetColumns();

  long row=0;
  CDArray M;
  M.SetSize(m_SDB.Count());
  long iFidelity=0; 
  for (int j=0; j<m_SDB.m_DensCorrSps.GetSize(); j++)
    {
    int iSolvent=m_SDB.m_DensCorrSps[j];
    CSpecie & Solvent=m_SDB[iSolvent];
    for (int iDensCorr=0; iDensCorr<Solvent.DensCorrCount(); iDensCorr++)
      {
      M.SetAll(0.0);
      for (int Pass=(m_ShowAll.GetCheck() ? 1 : 0); Pass<2; Pass++)
        {
        if (Pass>0 && row>0)
          m_RelDensGrid.AddItem(SBlnk(), COleVariant(row++));

        Strng Note;
        flag fBad=false;
        CDensCorr &SItem=Solvent.DensCorr(iDensCorr);
        M[SItem.m_iSoluteA]=0.0;
        M[iSolvent]=1.0;
        double Rho0=m_SDB[iSolvent].SpRho(iFidelity, C_2_K(25.0), Std_T);
        for (int it=0; it<=m_NIntervals; it++)
          {
          M[SItem.m_iSoluteA]=(1.0/m_NIntervals)*it;
          M[iSolvent]=1.0;
          double Rho=m_SDB.Density(iFidelity, som_ALL, C_2_K(25.0), Std_T, NULL, &M[0]);
          double SG=Rho/Rho0;
          Note="";
          if (it==0 && (fabs(SG-1.0)>1.0e-4))
            Note="SG NonUnity";
          if (SG<0.1)
            {
            if (Note())
              Note+="/";
            Note+="SG very Low";
            }
          if (SG>10.0)
            {
            if (Note())
              Note+="/";
            Note+="SG very High";
            }

          if (Pass>0)
            {
            Strng S;
            S.Set("%s\t%s\t%.2f\t%.3f\t%.3f\t%s",
              it==0 ? (m_SDB[iSolvent].SymOrTag()?m_SDB[iSolvent].SymOrTag():""):"",
              it==0 ? (m_SDB[SItem.m_iSoluteA].SymOrTag()?m_SDB[SItem.m_iSoluteA].SymOrTag():""):"",
              100.0*M[SItem.m_iSoluteA]/M[iSolvent],
              Rho,
              SG,
              Note() ? Note() : " ");
            m_RelDensGrid.AddItem(S(), COleVariant(row++));
            }
          fBad=fBad || (Note()!=NULL);
          }
        if (Pass>0)
          if (fBad)
            nBad++;
          else
            nGood++;
        if (m_ShowAll.GetCheck()==0 && !fBad)
          break;
        }
      }
    }
  }

void CSDBTest::TestSoluble(int &nGood, int &nBad)
  {
  Strng SBlnk(" "), S;
  CSSColumns Cols=m_SolubleGrid.GetColumns();
  CSSColumn Col2=Cols.GetItem(COleVariant(2L));
  Col2.SetCaption(m_ShowK.GetCheck() ? "Temp(K)" : "Temp(C)");

  long row=0;

  for (int j=0; j<m_SDB.m_SolubleSps.GetSize(); j++)
    {
    int iSolvent=m_SDB.m_SolubleSps[j];
    CSpecie & Solvent=m_SDB[iSolvent];
    for (int iSoln=0; iSoln<Solvent.SolubilityCount(); iSoln++)
      {
      //M.SetAll(0.0);
      for (int Pass=(m_ShowAll.GetCheck() ? 1 : 0); Pass<2; Pass++)
        {
        if (Pass>0 && row>0)
          m_SolubleGrid.AddItem(SBlnk(), COleVariant(row++));

        double T1=m_SDB[iSolvent].LoT(0);
        double T2=m_SDB[iSolvent].HiT(0);
        double dt=(T2-T1)/m_NIntervals;
        double t=T1-dt;

        Strng Note;
        flag fBad=false;
        for (int it=0; it<=m_NIntervals; it++)
          {
          t+=dt;
          Strng Note;
          flag fBad=false;
          CSolubility &SItem=Solvent.Solubility(iSoln);
          Note="";
          Strng SolS(" ");
          if (SItem.m_pSolubleFn)
            {
            double Sol=SItem.m_pSolubleFn->AtT(t);
            SolS.Set("%.3f", Sol);
            if (Sol<0.0)
              {
              if (Note())
                Note+="/";
              Note+="Negative ";
              }
            }

          if (Pass>0)
            {
            S.Set("%s\t%s\t%.2f\t%s\t%s",
              it==0 ? (m_SDB[iSolvent].SymOrTag()?m_SDB[iSolvent].SymOrTag():""):"",
              it==0 ? (m_SDB[SItem.m_iSoluteA].SymOrTag()?m_SDB[SItem.m_iSoluteA].SymOrTag():""):"",
              m_ShowK.GetCheck()?t:K_2_C(t),
              SolS(),
              Note() ? Note() : " ");
            m_SolubleGrid.AddItem(S(), COleVariant(row++));
            }
          fBad=fBad || (Note()!=NULL);
          }
        if (Pass>0)
          if (fBad)
            nBad++;
          else
            nGood++;
        if (m_ShowAll.GetCheck()==0 && !fBad)
          break;
        }
      }
    }

  }

void CSDBTest::TestMixHeat(int &nGood, int &nBad)
  {
//  Strng SBlnk(" "), S;
//  CSSColumns Cols=m_MixHeatGrid.GetColumns();
//
//  long row=0;
//  CDArray M;
//  M.SetSize(m_SDB.Count());
//  for (int j=0; j<m_SDB.m_SolnSps.GetSize(); j++)
//    {
//    int iSolvent=m_SDB.m_SolnSps[j];
//    CSpecie & Solvent=m_SDB[iSolvent];
//    for (int iSoln=0; iSoln<Solvent.NSolnItems(); iSoln++)
//      {
////      M.SetAll(0.0);
//      for (int Pass=(m_ShowAll.GetCheck() ? 1 : 0); Pass<2; Pass++)
//        {
//        if (Pass>0 && row>0)
//          m_MixHeatGrid.AddItem(SBlnk(), COleVariant(row++));
//
//        Strng Note;
//        flag fBad=false;
//        CSolnItem &SItem=Solvent.SolnItem(iSoln);
//
//        for (long iFidelity=0; iFidelity<2; iFidelity++)
//          {
//          //bool Fidelity=(iFidelity==1);
//
//          M[SItem.m_iSoluteA]=0.0;
//          M[iSolvent]=1.0;
//  //        double H0=m_SDB[iSolvent].msEnthalpy(C_2_K(25.0), Std_T);
//          for (int it=0; it<=m_NIntervals; it++)
//            {
//            M[SItem.m_iSoluteA]=(0.9/m_NIntervals)*it;
//            M[iSolvent]=1.0-M[SItem.m_iSoluteA];
//
//            Strng NS;
//            if (it>0)
//              {
//              double N=M[iSolvent]/M[SItem.m_iSoluteA];
//              NS.Set("%.1f", N);
//              }
//            else
//              NS="Infinite";
//
//            double h0=m_SDB[iSolvent].mlHf(iFidelity, Std_T, Std_P, NULL, NULL);
//            double h1=m_SDB[SItem.m_iSoluteA].mlHf(iFidelity, Std_T, Std_P, NULL, NULL);
//            double h2=m_SDB[SItem.m_iSoluteS].mlHf(iFidelity, Std_T, Std_P, NULL, NULL);
//
//            double H1=M[iSolvent]*m_SDB[iSolvent].mlHf(iFidelity, Std_T, Std_P, NULL, NULL) +
//                      M[SItem.m_iSoluteA]*m_SDB[SItem.m_iSoluteA].mlHf(iFidelity, Std_T, Std_P, NULL, NULL);
//            H1/=(M[iSolvent]+M[SItem.m_iSoluteA]);
//
//            M[iSolvent]+=1000000;
//            double H0=M[iSolvent]*m_SDB[iSolvent].mlHf(iFidelity, Std_T, Std_P, NULL, NULL) +
//                      M[SItem.m_iSoluteA]*m_SDB[SItem.m_iSoluteA].mlHf(iFidelity, Std_T, Std_P, NULL, NULL);
//            H0/=(M[iSolvent]+M[SItem.m_iSoluteA]);
//            double DT=0.0;
//
//  dbgpln("%-15.15s  %-15.15s  %8.8s  h:%12.1f  l:%12.1f  a:%12.1f  d:%12.1f  c:%12.1f  %12.1f  %12.1f  %12.1f  %12.1f",
//    it==0 ? (m_SDB[iSolvent].SymOrTag()?m_SDB[iSolvent].SymOrTag():""):"",
//    it==0 ? (m_SDB[SItem.m_iSoluteA].SymOrTag()?m_SDB[SItem.m_iSoluteA].SymOrTag():""):"",
//    NS(),
//    h0,
//    h2,
//    h1,
//    h2-h1,
//    (H1-H0)/4.184,
//    H1-H0,
//    H1,H0,
//    DT);
//            //Note="";
//            //if (it==0 && (fabs(SG-1.0)>1.0e-4))
//            //  Note="SG NonUnity";
//            //if (SG<0.1)
//            //  {
//            //  if (Note())
//            //    Note+="/";
//            //  Note+="SG very Low";
//            //  }
//            //if (SG>10.0)
//            //  {
//            //  if (Note())
//            //    Note+="/";
//            //  Note+="SG very High";
//            //  }
//
//            if (Pass>0)
//              {
//              Strng S;
//              S.Set("%s\t%s\t%s\t%.1f\t%.1f\t%.1f\t%.1f\t%s",
//                it==0 ? (m_SDB[iSolvent].SymOrTag()?m_SDB[iSolvent].SymOrTag():""):"",
//                it==0 ? (m_SDB[SItem.m_iSoluteA].SymOrTag()?m_SDB[SItem.m_iSoluteA].SymOrTag():""):"",
//                NS(),
//                H1-H0,
//                H1,H0,
//                DT,
//                Note() ? Note() : " ");
//              m_MixHeatGrid.AddItem(S(), COleVariant(row++));
//              }
//            fBad=fBad || (Note()!=NULL);
//            }
//
//          if (Pass>0)
//            if (fBad)
//              nBad++;
//            else
//              nGood++;
//          if (m_ShowAll.GetCheck()==0 && !fBad)
//            break;
//          }
//        }
//      }
//    }
  }

void CSDBTest::OnSpDotest()
  {
  CWaitCursor W;
  CDBInitRec CDBI(C_2_K(0.0), C_2_K(1000.0), 50.0, 350.0, false);
  for (int i=0; i<Rows().GetSize(); i++)
    {
    Strng S;
    S.Set("%s(%s)", Rows()[i].Comp(),Rows()[i].Occ());
    CRqdSpItem I(S());
    CDBI.RqdSpecies.AddTail(I);
    }

  CString StrN;
  m_Intervals.GetWindowText(StrN);
  m_NIntervals=SafeAtoL(StrN, 10);

  COleVariant var;
  int sel=m_Tab.GetCurSel();
  int iMsgSel=4;

  for (i=0; i<5; i++)
    {
    m_Grids[i]->RemoveAll();
    m_Grids[i]->SetEnabled(false);
    if (i==sel || i==iMsgSel)
      m_Grids[i]->SetRedraw(false);
    }
  m_NoBad.SetWindowText("Busy");
  m_NoGood.SetWindowText("Busy");
  gs_MsgLog.Lock();
  long CurrentMsgID=gs_MsgLog.NextIDNo();
  gs_MsgLog.Unlock();
  //  CLogMsgArray ErrMsgs;
//  SetRefLogMsgArray(&ErrMsgs);
  if (m_CDB.Initialise(CDBI, (LPTSTR)Filename(), NULL, false))
    {
    int nBad=0;
    int nGood=0;

    TestThermo(nGood, nBad);
    TestRelDens(nGood, nBad);
    TestSoluble(nGood, nBad);
    TestMixHeat(nGood, nBad);

    //Sleep(2000);
    Strng S;
    S.Set("%i", nBad);
    m_NoBad.SetWindowText(S());
    S.Set("%i", nGood);
    m_NoGood.SetWindowText(S());
    }
  else
    {
    LogError("SDB Test", LF_Exclamation, "Specie DB Failed to Initialise");
    m_NoBad.SetWindowText("");
    m_NoGood.SetWindowText("");
    }

  gs_MsgLog.Lock();
  long NMsgs=gs_MsgLog.WndQMsgCount();
//  SetRefLogMsgArray(NULL);

  long nMsgs=0;
  for (i=0; i<NMsgs; i++)
    {
    CMsgLogItem & Itm=gs_MsgLog.GetWndQMsg(i);
    if (Itm.m_lIDNo>CurrentMsgID)
      {
      m_MsgGrid.SetRedraw(true);
      Strng S;
      dword iTyp=CMessageLog::MsgTypeWord(Itm.m_dwFlags);
      LPCTSTR pTyp=CMessageLog::MsgTypeString(iTyp);
      if (pTyp)
        {
        S.Set("%s\t%i\t%s %s", pTyp, Itm.m_lIterNo, Itm.Source(), Itm.Description());
        m_MsgGrid.AddItem(S(), COleVariant(nMsgs++));
        }
      }
    }
  gs_MsgLog.Unlock();

  for (i=0; i<5; i++)
    {
    if (i==sel || i==iMsgSel)
      m_Grids[i]->SetRedraw(true);
    m_Grids[i]->SetEnabled(true);
    }
  m_Tab.SetCurSel(sel);
  m_CDB.Terminate();
  }

BOOL CSDBTest::OnSetActive()
  {
	return CPropertyPage::OnSetActive();
  }

BOOL CSDBTest::OnKillActive()
  {
//  m_CDB.Terminate();
	return CPropertyPage::OnKillActive();
  }


BEGIN_EVENTSINK_MAP(CSDBTest, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CSDBTest)
	ON_EVENT(CSDBTest, IDC_SSDBGRIDCTRL1, -600 /* Click */, OnClickSsdbgridctrl1, VTS_NONE)
	ON_EVENT(CSDBTest, IDC_SSDBGRIDCTRL1, 32 /* SelChange */, OnSelChangeSsdbgridctrl1, VTS_I2 VTS_PI2 VTS_PI2)
	ON_EVENT(CSDBTest, IDC_SSDBGRIDCTRL1, 27 /* UnboundPositionData */, OnUnboundPositionDataSsdbgridctrl1, VTS_PVARIANT VTS_I4 VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSDBTest::OnClickSsdbgridctrl1()
  {
  USES_CONVERSION;
  COleVariant BM=m_ThermoGrid.RowBookmark(m_ThermoGrid.GetRow());
  CSSSelBookmarks SBM=m_ThermoGrid.GetSelBookmarks();


  if (SBM.GetCount()>0)
    {
    COleVariant Inx=0L;
    COleVariant BM=SBM.GetItem(Inx);
    int SpNo=-1;
    CSSColumns Cols=m_ThermoGrid.GetColumns();
    for (int c=0; c<Cols.GetCount(); c++)
      {
      CSSColumn Col=Cols.GetItem(COleVariant((long)c));
      CString Cap=Col.GetCaption();
      if (Cap.CompareNoCase("SpecieNo")==0)
        {
        COleVariant vs=Col.CellValue(BM);
        SpNo=SafeAtoL(OLE2T(V_BSTR(&vs)),-1);
        break;
        }
      }
    /*
    CVcDataGrid DG=m_Chart.GetDataGrid();
    if (SpNo>=0)
      {
      int m_NIntervals=10;
      double T1=m_SDB[SpNo].LoT();
      double T2=m_SDB[SpNo].HiT();
      double dt=(T2-T1)/m_NIntervals;
      double t=T1-dt;
      DG.SetColumnCount(2);
      DG.SetRowCount(m_NIntervals+1);
      for (int i=0; i <= m_NIntervals; i++)
        {
        t+=dt;
        double vp=m_SDB[SpNo].VapourP(t);
        double H=m_SDB[SpNo].mlEnthalpy(C_2_K(25.0), t);
        DG.SetData(i+1, 1, H, 0);
//        DG.SetData(i+1, 2, vp, 0);
        DG.SetData(i+1, 2, H, 0);
//        DG.SetData(i+1, 4, t, 0);
        }
      DG.RandomDataFill();
//        DG.
      DG
      m_Chart.GetAx.SetChartData(

      }
    else
      {
      DG.SetRowCount(0);
      }
    m_Chart.Refresh();
    */
    }
  }

void CSDBTest::OnSelChangeSsdbgridctrl1(short SelType, short FAR* Cancel, short FAR* DispSelRowOverflow)
  {
	// TODO: Add your control notification handler code here
	
  }

void CSDBTest::OnUnboundPositionDataSsdbgridctrl1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation)
  {
	// TODO: Add your control notification handler code here
	
  }

/////////////////////////////////////////////////////////////////////////////
// CSDBTest message handlers



/////////////////////////////////////////////////////////////////////////////
// CSDBCnvs dialog


CSDBCnvs::CSDBCnvs(CSDBSheet * pSheet)//CWnd* pParent = NULL);   // standard constructor
	: CPropertyPage(CSDBCnvs::IDD)
  {
	//{{AFX_DATA_INIT(CSDBCnvs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_bDidInit=false;
  m_pSheet=pSheet;
  m_pRSCnv=NULL;
  }

CSDBCnvs::~CSDBCnvs()
  {
  }

void CSDBCnvs::DoDataExchange(CDataExchange* pDX)
  {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDBCnvs)
	DDX_Control(pDX, IDC_SSDBGRIDCTRLCNVS, m_Grid);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSDBCnvs, CPropertyPage)
	//{{AFX_MSG_MAP(CSDBCnvs)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSDBCnvs::OnInitDialog()
  {
	CPropertyPage::OnInitDialog();

  m_bDidInit=true;
  try
    {
    CWaitCursor W;
    m_pRSCnv=ADODB::_RecordsetPtr(__uuidof (ADODB::Recordset));
    try
      {
      CString Slct;
      Slct.Format("SELECT * FROM %s", StrCnvTbl);
      m_pRSCnv->Open(_variant_t(Slct), _variant_t((IDispatch*)ConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Open RecordSet");
      return false;
      }

    CSSColumns Cols=m_Grid.GetColumns();
	  long ct = Recordset()->Fields->Count;
    long rc = Recordset()->GetRecordCount();//    m_pRSCnvs->
    for (short f=0; f<ct; f++)
      {
      Cols.Add(f);
      CSSColumn Col=Cols.GetItem(COleVariant(f));
      CString FldName=(LPCTSTR)Recordset()->Fields->GetItem((long)f)->Name;
      MyFldInfo * pF=FindFld(FldName, CnvFI);
      if (pF)
        {
        Col.SetCaption(pF->m_Caption);
        Col.SetWidth(pF->m_Wide*8.0f);
        Col.SetAlignment(pF->m_Align);
        }
      else
        {
        Col.SetCaption(FldName);
        Col.SetWidth(3*8.0f);
        Col.SetAlignment(1);
        }

//      if (_stricmp(FI.m_strName, "ID")==0)
//        Col.SetWidth(4*8.0f);
//      else if (_stricmp(FI.m_strName, "Conversion")==0)
//        Col.SetWidth(8*8.0f);
      }
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Building Dialog");
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }


void CSDBCnvs::OnCancel()
  {
	if (m_bDidInit)
	  CPropertyPage::OnCancel();
  }

void CSDBCnvs::OnOK()
  {
	if (m_bDidInit)
	  CPropertyPage::OnOK();
  }


BOOL CSDBCnvs::OnSetActive()
  {
  //m_Grid.ReBind();
	return CPropertyPage::OnSetActive();
  }

BOOL CSDBCnvs::OnKillActive()
  {
  if (m_Grid.GetDataChanged())
    m_Grid.Update();
	return CPropertyPage::OnKillActive();
  }

BEGIN_EVENTSINK_MAP(CSDBCnvs, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CSDBCnvs)
	ON_EVENT(CSDBCnvs, IDC_SSDBGRIDCTRLCNVS, 1 /* InitColumnProps */, OnInitColumnPropsSsdbgridctrlapt1, VTS_NONE)
	ON_EVENT(CSDBCnvs, IDC_SSDBGRIDCTRLCNVS, 23 /* UnboundReadData */, OnUnboundReadDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT VTS_BOOL)
	ON_EVENT(CSDBCnvs, IDC_SSDBGRIDCTRLCNVS, 24 /* UnboundAddData */, OnUnboundAddDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CSDBCnvs, IDC_SSDBGRIDCTRLCNVS, 25 /* UnboundWriteData */, OnUnboundWriteDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CSDBCnvs, IDC_SSDBGRIDCTRLCNVS, 26 /* UnboundDeleteRow */, OnUnboundDeleteRowSsdbgridctrlapt1, VTS_PVARIANT)
	ON_EVENT(CSDBCnvs, IDC_SSDBGRIDCTRLCNVS, 27 /* UnboundPositionData */, OnUnboundPositionDataSsdbgridctrlapt1, VTS_PVARIANT VTS_I4 VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSDBCnvs::OnInitColumnPropsSsdbgridctrlapt1()
  {
	
  }

void CSDBCnvs::OnUnboundReadDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* StartLocation, BOOL ReadPriorRows)
  {
	USES_CONVERSION;
  long i;
	long r = 0;

	RowBufferDisp ssRowBuf;
  ssRowBuf.AttachDispatch(RowBuf, FALSE);

  if(Recordset()->BOF && Recordset()->adEOF)
    return;

	long ct = Recordset()->Fields->Count;
  
	if( StartLocation->vt == VT_NULL)
	  {
		if (ReadPriorRows)
			Recordset()->MoveLast();
		else
			Recordset()->MoveFirst();
	  }
	else
	  {	
		Recordset()->Bookmark=*StartLocation; //->iVal);//->iVal);

		if (ReadPriorRows)
			Recordset()->MovePrevious();
		else
			Recordset()->MoveNext();
			
	  }

	if (ssRowBuf)
	  {	
		const long num = ssRowBuf.GetRowCount();
		BOOL bDone = FALSE;  //to exit the loop

		for(i=0; i<num && !bDone;i++)
		  {	
		
			if(Recordset()->BOF || Recordset()->adEOF)
				bDone = TRUE;
			else
			  {
				for(long j=0;j < ct;j++)
				  {	
					COleVariant va = Recordset()->Fields->GetItem((long)j)->Value;
					switch(va.vt)
					  {
					  case VT_BSTR:
						  {
						  ssRowBuf.SetValue((short)i,(short)j,va);
						  break;
						  }
            default:
              ssRowBuf.SetValue((short)i,(short)j,va);
              break;
					  }
				  }

        ssRowBuf.SetBookmark(i, Recordset()->GetBookmark());
				
				if(ReadPriorRows)
					Recordset()->MovePrevious();
				else
					Recordset()->MoveNext();

				r+=1;
			  }		
		  }
		
		ssRowBuf.SetRowCount(r);
    }
	}	

void CSDBCnvs::Buff2RecSet(LPDISPATCH RowBuf)
  {
	RowBufferDisp ssRowBuf;
  ssRowBuf.AttachDispatch(RowBuf, FALSE);
	long ct = Recordset()->Fields->Count;

	if (ssRowBuf)
	  {	
		for(short j=0;j < ct;j++)
      {	
			COleVariant va = ssRowBuf.GetValue(0,j);
			switch(va.vt)
				{
				case VT_NULL:
          break;
				case VT_BSTR:
					{
 					CString s=V_BSTR(&va);
          if (s.GetLength()==0)
            s=" ";
					Recordset()->Fields->GetItem((long)j)->Value=(LPCTSTR)s;
					break;
					}
        default:
          Recordset()->Fields->GetItem((long)j)->Value=va;
          break;
				}
      }
    }
  }

void CSDBCnvs::OnUnboundAddDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* NewRowBookmark)
  {
  try
    {
    Recordset()->AddNew();
    Buff2RecSet(RowBuf);

    Recordset()->Update();
    Recordset()->MoveLast();
    *NewRowBookmark = Recordset()->Bookmark;
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Adding Record");
    }
  }
void CSDBCnvs::OnUnboundWriteDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* WriteLocation)
  {
  try
    {
    Recordset()->Bookmark=*WriteLocation;
    //Recordset()->Edit();

    Buff2RecSet(RowBuf);
    Recordset()->Update();
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Updating Record");
    }
	}

void CSDBCnvs::OnUnboundDeleteRowSsdbgridctrlapt1(VARIANT FAR* Bookmark)
  {
  try
    {
    Recordset()->Bookmark=*Bookmark;
    Recordset()->Delete(ADODB::adAffectCurrent);
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Deleting Record");
    }
	}

void CSDBCnvs::OnUnboundPositionDataSsdbgridctrlapt1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation)
  {
	}

/////////////////////////////////////////////////////////////////////////////
// CSDBCnvs message handlers

/////////////////////////////////////////////////////////////////////////////
// CSDBSoln dialog


CSDBSoln::CSDBSoln(CSDBSheet * pSheet)//CWnd* pParent = NULL);   // standard constructor
	: CPropertyPage(CSDBSoln::IDD)
  {
	//{{AFX_DATA_INIT(CSDBSoln)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_bDidInit=false;
  m_pSheet=pSheet;
  m_pRSSoln=NULL;
  }

CSDBSoln::~CSDBSoln()
  {
  }

void CSDBSoln::DoDataExchange(CDataExchange* pDX)
  {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDBSoln)
	DDX_Control(pDX, IDC_SSDBGRIDCTRLSOLNS, m_Grid);
	//}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CSDBSoln, CPropertyPage)
	//{{AFX_MSG_MAP(CSDBSoln)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSDBSoln::OnInitDialog()
  {
	CPropertyPage::OnInitDialog();

  m_bDidInit=true;
  try
    {
    CWaitCursor W;
    m_pRSSoln=ADODB::_RecordsetPtr(__uuidof (ADODB::Recordset));
    try
      {
      CString Slct;
      Slct.Format("SELECT * FROM %s", StrSolnTbl);
      m_pRSSoln->Open(_variant_t(Slct), _variant_t((IDispatch*)ConnectionPtr()), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
      }
    catch(_com_error & e)
      {
      DoCatch(e, "Open RecordSet");
      return false;
      }

    CSSColumns Cols=m_Grid.GetColumns();
	  long ct = Recordset()->Fields->Count;
    long rc = Recordset()->RecordCount;//    m_pRSSolns->
    for (short f=0; f<ct; f++)
      {
      Cols.Add(f);
      CSSColumn Col=Cols.GetItem(COleVariant(f));
      CString FldName=(LPCTSTR)Recordset()->Fields->GetItem((long)f)->Name;

      MyFldInfo * pF=FindFld(FldName, SolnFI);
      if (pF)
        {
        Col.SetCaption(pF->m_Caption);
        Col.SetWidth(pF->m_Wide*8.0f);
        Col.SetAlignment(pF->m_Align);
        }
      else
        {
        Col.SetCaption(FldName);
        Col.SetWidth(3*8.0f);
        Col.SetAlignment(1);
        }
      }
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Building Dialog");
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }


void CSDBSoln::OnCancel()
  {
	if (m_bDidInit)
	  CPropertyPage::OnCancel();
  }

void CSDBSoln::OnOK()
  {
	if (m_bDidInit)
	  CPropertyPage::OnOK();
  }


BOOL CSDBSoln::OnSetActive()
  {
  //m_Grid.ReBind();
	return CPropertyPage::OnSetActive();
  }

BOOL CSDBSoln::OnKillActive()
  {
  if (m_Grid.GetDataChanged())
    m_Grid.Update();
	return CPropertyPage::OnKillActive();
  }

BEGIN_EVENTSINK_MAP(CSDBSoln, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CSDBSoln)
	ON_EVENT(CSDBSoln, IDC_SSDBGRIDCTRLSOLNS, 1 /* InitColumnProps */, OnInitColumnPropsSsdbgridctrlapt1, VTS_NONE)
	ON_EVENT(CSDBSoln, IDC_SSDBGRIDCTRLSOLNS, 23 /* UnboundReadData */, OnUnboundReadDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT VTS_BOOL)
	ON_EVENT(CSDBSoln, IDC_SSDBGRIDCTRLSOLNS, 24 /* UnboundAddData */, OnUnboundAddDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CSDBSoln, IDC_SSDBGRIDCTRLSOLNS, 25 /* UnboundWriteData */, OnUnboundWriteDataSsdbgridctrlapt1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CSDBSoln, IDC_SSDBGRIDCTRLSOLNS, 26 /* UnboundDeleteRow */, OnUnboundDeleteRowSsdbgridctrlapt1, VTS_PVARIANT)
	ON_EVENT(CSDBSoln, IDC_SSDBGRIDCTRLSOLNS, 27 /* UnboundPositionData */, OnUnboundPositionDataSsdbgridctrlapt1, VTS_PVARIANT VTS_I4 VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSDBSoln::OnInitColumnPropsSsdbgridctrlapt1()
  {
	
  }

void CSDBSoln::OnUnboundReadDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* StartLocation, BOOL ReadPriorRows)
  {
	USES_CONVERSION;
  long i;
	long r = 0;

	RowBufferDisp ssRowBuf;
  ssRowBuf.AttachDispatch(RowBuf, FALSE);

	if(Recordset()->BOF && Recordset()->adEOF)
    return;
	
  long ct = Recordset()->Fields->Count;

	if( StartLocation->vt == VT_NULL)
	  {
		if (ReadPriorRows)
			Recordset()->MoveLast();
		else
			Recordset()->MoveFirst();
	  }
	else
	  {	
		Recordset()->Bookmark=*StartLocation; //->iVal);//->iVal);

		if (ReadPriorRows)
			Recordset()->MovePrevious();
		else
			Recordset()->MoveNext();
			
	  }

	if (ssRowBuf)
	  {	
		const long num = ssRowBuf.GetRowCount();
		BOOL bDone = FALSE;  //to exit the loop

		for(i=0; i<num && !bDone;i++)
		  {	
		
			if(Recordset()->BOF || Recordset()->adEOF)
				bDone = TRUE;
			else
			  {
				for(long j=0;j < ct;j++)
				  {	
					COleVariant va = Recordset()->Fields->GetItem((long)j)->Value;
					switch(va.vt)
					  {
					  case VT_BSTR:
						  {
						  ssRowBuf.SetValue((short)i,(short)j,va);
						  break;
						  }
            default:
              ssRowBuf.SetValue((short)i,(short)j,va);
              break;
					  }
				  }

        ssRowBuf.SetBookmark(i, Recordset()->GetBookmark());
				
				if(ReadPriorRows)
					Recordset()->MovePrevious();
				else
					Recordset()->MoveNext();

				r+=1;
			  }		
		  }
		
		ssRowBuf.SetRowCount(r);
    }
	}	

void CSDBSoln::Buff2RecSet(LPDISPATCH RowBuf)
  {
	RowBufferDisp ssRowBuf;
  ssRowBuf.AttachDispatch(RowBuf, FALSE);
	long ct = Recordset()->Fields->Count;

	if (ssRowBuf)
	  {	
		for(short j=0;j < ct;j++)
      {	
			COleVariant va = ssRowBuf.GetValue(0,j);
			switch(va.vt)
				{
				case VT_NULL:
          break;
				case VT_BSTR:
					{
 					CString s=V_BSTR(&va);
          if (s.GetLength()==0)
            s=" ";
					Recordset()->Fields->GetItem((long)j)->Value=(LPCTSTR)s;
					break;
					}
        default:
          Recordset()->Fields->GetItem((long)j)->Value=va;
          break;
				}
      }
    }
  }

void CSDBSoln::OnUnboundAddDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* NewRowBookmark)
  {
  try
    {
    Recordset()->AddNew();
    Buff2RecSet(RowBuf);

    Recordset()->Update();
    Recordset()->MoveLast();
    *NewRowBookmark = Recordset()->Bookmark;
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Adding Record");
    }
  }
void CSDBSoln::OnUnboundWriteDataSsdbgridctrlapt1(LPDISPATCH RowBuf, VARIANT FAR* WriteLocation)
  {
  try
    {
    Recordset()->Bookmark=*WriteLocation;
    //Recordset()->Edit();

    Buff2RecSet(RowBuf);
    Recordset()->Update();
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Updating Record");
    }
	}

void CSDBSoln::OnUnboundDeleteRowSsdbgridctrlapt1(VARIANT FAR* Bookmark)
  {
  try
    {
    Recordset()->Bookmark=*Bookmark;
    Recordset()->Delete(ADODB::adAffectCurrent);
    }
  catch(_com_error & e)
    {
    DoCatch(e, "Deleting Record");
    }
	}

void CSDBSoln::OnUnboundPositionDataSsdbgridctrlapt1(VARIANT FAR* StartLocation, long NumberOfRowsToMove, VARIANT FAR* NewLocation)
  {
	}

/////////////////////////////////////////////////////////////////////////////
// CSDBSoln message handlers

BOOL CSDBSpecie::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
  {
	BOOL b=CPropertyPage::OnNotify(wParam, lParam, pResult);
  HD_NOTIFY* pNM = (HD_NOTIFY*)lParam;
  switch (pNM->hdr.code)
    {
    case LVN_SETDISPINFO:
      {int xxx=0;}
      break;
    case LVN_GETDISPINFO:
      {
      NMLVDISPINFO * pnmv = (NMLVDISPINFO*) lParam; 
      long i=pnmv->item.lParam;
      Strng S;
      switch (pnmv->item.iSubItem)
        {
        case 0:
//          pnmv->item.iImage=It.iImg;
          S=Rows()[i].Comp();
          //Wide=Max(Wide, Rows()[i].Comp.Length());
          S+="(";
          S+=Rows()[i].Occ();
          S+=")";
          pnmv->item.pszText=S();
          break;
        case 1:
          S.Set("%.1f", Rows()[i].Ts);
          pnmv->item.pszText=S();
          break;
        case 2:
          S.Set("%.1f", Rows()[i].Te);
          pnmv->item.pszText=S();
          break;
        }
      }
      break;
    }
  return b;
  }

// --------------------------------------------------------------------------
// ==========================================================================

flag CSCDDatabase::DoSDBEdit(const char* DBFn)
  {
  return CSDBSheet::Edit(DBFn, false);
  }

// --------------------------------------------------------------------------

flag CSCDDatabase::DoSDBCreate(const char* DBFn)
  {
  return CSDBSheet::Edit(DBFn, true);
  }

// --------------------------------------------------------------------------

//BOOL CSCDDatabase::UpgradeCSVDB(LPCTSTR CSVPath, CUpGrdCompletion &UGC)
//  {
//  return CSCD_DB::UpgradeCSVDB(CSVPath, UGC);
//  }

// --------------------------------------------------------------------------
