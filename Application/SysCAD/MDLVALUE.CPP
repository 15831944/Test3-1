//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// MdlValue.cpp : implementation file

#include "stdafx.h"
#include "sc_defs.h"
#include "MdlValue.h"
#include "datacnvs.h"
#include "grf3drw.h"
#include "sfe_clnt.h"
#include "project.h"
#include "accnode.h"
#include "scd_wm.h"
#ifndef __FE_CONST_H
#include "..\flwlib\fe_const.h"
#endif

//#include "optoff.h"

extern "C" 
  {
  //  #include "c2cdefs.h"
  //  #include "c2ddefs.h"
#include "grldefs.h"
  extern int get_line_error();
  }

#define dbgLoadSave    0
#define dbgBuild       0
#define dbgValueShow   0
#define dbgGraphicShow 0

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CRptTagsAddRqst
  {
  public:
    Strng  m_Tag;
    int    m_X, m_Y;
    bool   m_Pinned;
    bool   m_Append;
    CWnd*  m_pParent;
  };

const int MinShownLines = 0;//;2; // CNM 5;
const int MaxSetPtrs    = 8;

typedef CMdlValueSet *ValueSetPtr;
typedef CMdlValueShow *ValueShwPtr;
typedef CMdlGraphicShow *GraphicShwPtr;
typedef CRptTagsAddRqst *CAddRequestPtr;
const char* BaseFileName = "Scd_RTL.ini";

class CRptTagListCommon
  {
  public:
    BOOL	          m_InBaseCfg;
    BOOL	          m_InCfg;
    long            m_lMaxValueLen;
    long            m_lMaxTags;
    CRptTagLists *  m_pRTL;
    Strng           m_sRTLFileName;
    Strng           m_sInCfgName;
    Strng           m_sObjClassId;

    int             m_iSequence;
    int             m_iClear;
    CList <ValueSetPtr, ValueSetPtr> m_SetPtrs;
    CList <ValueShwPtr, ValueShwPtr> m_ValueShwPtrs;
    CList <GraphicShwPtr, GraphicShwPtr> m_GraphicShwPtrs;
    CList <CAddRequestPtr, CAddRequestPtr> m_ValueAddPtrs;
    CList <CAddRequestPtr, CAddRequestPtr> m_GraphicAddPtrs;
    CList <ValueShwPtr, ValueShwPtr> m_SpareShwPtrs;
    CCriticalSection  m_MVCritSect;

    char *          m_pSectBuff;
    dword           m_dwSectBuff;
    CCriticalSection  m_LVCritSect;


    CRptTagListCommon()
      {
      m_InBaseCfg=false;
      m_InCfg=true;
      m_lMaxValueLen=20;
      m_lMaxTags=16;//MdlValueShowMaxTags;
      m_pRTL=NULL;
      m_pSectBuff=NULL;
      m_iClear=0;
      m_iSequence=0;
      }
    ~CRptTagListCommon()
      {
      delete m_pSectBuff;
      }
    char * RTLFileName()
      {
      if (m_InBaseCfg)
        m_sRTLFileName = BaseCfgFiles();
      else if (m_InCfg)
        m_sRTLFileName = CfgFiles();
      else
        m_sRTLFileName = PrjFiles();

      m_sRTLFileName.FnCheckEndBSlash();
      m_sRTLFileName+=BaseFileName; 
      return m_sRTLFileName();
      }
    char * InCfgName()
      {
      if (m_sObjClassId())
        m_sInCfgName=m_sObjClassId;
      else
        m_sInCfgName="???";
      return m_sInCfgName();
      }
    void SetObjClassId(CString &S) { m_sObjClassId=(char*)(const char*)S; };
    void SetObjClassId(char * p) { m_sObjClassId=p; };
    char * ObjClassId() { return m_sObjClassId(); };

    void LoadVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags);
    void SaveVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags, CIArray * pSaveOrder = NULL);

  };

//---------------------------------------------------------------------------

inline flag GetNextSZZ(pchar&pWrk, pchar&pItem, pchar&pData)
  {
  pItem=pWrk;
  if (pItem==NULL)
    return false;
  pData = strchr(pItem, '=');
  if (pData)
    pData++; 
  int len = strlen(pWrk);
  pWrk += (len + 1);
  if (*pWrk==0)
    pWrk=NULL;
  return true;
  }

//---------------------------------------------------------------------------

void CRptTagListCommon::LoadVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags)
  {
  //if (MdlType)
  //  m_sObjClassId=MdlType;
  Strng ObjClassId(MdlType);
  StripClassIdVersion(ObjClassId);

  m_LVCritSect.Lock();

  CProfINIFile PF(RTLFileName());
  nVarTags=0;

  if (m_dwSectBuff==0)
    {
    m_dwSectBuff=4096;
    m_pSectBuff=new char[m_dwSectBuff];
    ::memset(m_pSectBuff, 0, m_dwSectBuff);
    }

  Strng Section;
  Section.Set(Layout?"%s.View.Vars.%s":"%s.View.Vars", ObjClassId(), Layout);

#if dbgLoadSave
  dbgpln("LVT %s %s", Section(), PF.Filename());
#endif

  DWORD dw = PF.RdSection(Section(), m_pSectBuff, m_dwSectBuff);
  if (dw>m_dwSectBuff-(dword)2)
    {
    //section too large try again
    delete m_pSectBuff;
    m_dwSectBuff=(((dw+2)/1024)+1)*1024;
    m_pSectBuff=new char[m_dwSectBuff];
    ::memset(m_pSectBuff, 0, m_dwSectBuff);
    dw = PF.RdSection(Section(), m_pSectBuff, m_dwSectBuff);
    ASSERT_ALWAYS(dw<m_dwSectBuff, "Section Buffer Too Small", __FILE__, __LINE__);
    }

  if (dw>0)
    {
    Strng S("");
    char *pItem, *pData, *pWrk = m_pSectBuff;
    while (GetNextSZZ(pWrk, pItem, pData))
      {
      if (!pItem || *pItem!='V') // Skip UnitTags etc.
        continue;
#if dbgLoadSave
      dbgpln("<< %s %s", pItem, pData);
#endif
      if (pData)
        {
        CSVColArray Flds;
        int Quote;
        int nFlds=ParseCSVTokens(pData, Flds, Quote);
        if (nFlds>1 && Flds[1])
          {
          VarTags.SetSize(Max(nVarTags+1, VarTags.GetSize()));
          CVarTagItem &TI=VarTags[nVarTags++];

          TI.m_sLabel=Flds[0];
          TI.m_sTag=Flds[1];
          TI.m_Cnv.SetIndex(nFlds>2 && Flds[2] ? Flds[2] : "");
          TI.m_Cnv.SetText(nFlds>3 && Flds[3] ? Flds[3] : "");
          TI.m_Fmt.ParseStr(nFlds>4 && Flds[4] ? Flds[4] : "");
          }
        }
      }
    }  
  VarTags.SetSize(nVarTags);

  POSITION Pos=m_ValueShwPtrs.GetHeadPosition();
  while (Pos)
    {
    ValueShwPtr P=m_ValueShwPtrs.GetNext(Pos);
    P->SetReloadTagsList();
    }
  m_LVCritSect.Unlock();
  };

//---------------------------------------------------------------------------

void CRptTagListCommon::SaveVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags, CIArray * pSaveOrder)
  {
  Strng ObjClassId(MdlType);
  StripClassIdVersion(ObjClassId);

  WIN32_FIND_DATA fd;
  if (FileExists(RTLFileName(), fd))
    {
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("View", LF_Exclamation, "Cannot write to read-only file '%s'", RTLFileName());
      return;
      }
    }

  CProfINIFile PF(RTLFileName());
  //if (m_sObjClassId.GetLength()==0)
  //  return;
  m_LVCritSect.Lock();
  if (m_dwSectBuff==0)
    {
    m_dwSectBuff=4096;
    m_pSectBuff=new char[m_dwSectBuff];
    ::memset(m_pSectBuff, 0, m_dwSectBuff); 
    }

  Strng Section;
  Section.Set(Layout?"%s.View.Vars.%s":"%s.View.Vars", ObjClassId(), Layout);

#if dbgLoadSave
  dbgpln("SVT %s %s", Section(), PF.Filename());
#endif

  Strng S,F;
  dword dwLen=0;
  ::memset(m_pSectBuff, 0, m_dwSectBuff);
  char *pBuff=m_pSectBuff+dwLen;
  int N=pSaveOrder ? pSaveOrder->GetSize() : nVarTags;
  for (int i=0; i<N; i++)
    {
    CVarTagItem &TI=VarTags[pSaveOrder ? (*pSaveOrder)[i] : i];
    TI.m_Fmt.BuildStr(F);
    char * pFam=(TI.m_Cnv.Index()>=0 ? Cnvs[TI.m_Cnv.Index()]->Fam() : NULL);
    char * pTxt=TI.m_Cnv.TextLength()>0 ? TI.m_Cnv.Text() : "";
    S.Set("V%i=%s,%s,%s,%s,%s",i, TI.m_sLabel(), TI.m_sTag(), pFam ? pFam : "", pTxt, F());

#if dbgLoadSave
    dbgpln(">> %s", S());
#endif

    if (dwLen+S.Length()+2>m_dwSectBuff)
      {
      //section too large try again
      dword OldBuffLen=m_dwSectBuff;
      m_dwSectBuff=(((dwLen+S.Length()+2)/1024)+1)*1024;
      char * pTmp=new char[m_dwSectBuff];
      ::memset(pTmp, 0, m_dwSectBuff); 
      ::memmove(pTmp, m_pSectBuff, OldBuffLen);
      delete m_pSectBuff;
      m_pSectBuff=pTmp;
      pBuff=m_pSectBuff+dwLen;
      }
    strcpy(pBuff, S());
    dwLen+=S.Length()+1;
    pBuff=m_pSectBuff+dwLen;
    }  
  strcpy(pBuff, "");
  dwLen++;

  PF.WrSection(Section(), m_pSectBuff);
  if (pSaveOrder)
    pSaveOrder->SetSize(0);
  m_LVCritSect.Unlock();
  };


static CRptTagListCommon gs_RTLCom;

/////////////////////////////////////////////////////////////////////////////
// Move to TagObj

/////////////////////////////////////////////////////////////////////////////
// CMdlValueSet dialog

#define Vert 0
#define UseXSpares 01

//CMdlValueSet::CMdlValueSet(CWnd* pParent /*=NULL*/)
//	: CDialog(CMdlValueSet::IDD, pParent)
//{
//	//{{AFX_DATA_INIT(CMdlValueSet)
//	//}}AFX_DATA_INIT
//  m_Slider.SetRange(0,100);
//}

CMdlValueSet::CMdlValueSet(char * pTag, CMdlAction &Act, int X, int Y, CWnd* pParent /*=NULL*/)
: CDialog(CMdlValueSet::IDD, pParent)
  {
  Action=Act;
  sTag=pTag;
  iX=X;
  iY=Y;
  //{{AFX_DATA_INIT(CMdlValueSet)
  //}}AFX_DATA_INIT
  //  m_Slider.Create(0,100);

  Create(CMdlValueSet::IDD, pParent);
#if Vert
  Strng Name(Act.cName);
  Name.SetLength(Min(4, Name.GetLength()));
  SetWindowText(Name());
#else
  Strng Name(pTag);
  Name+=".";
  Name+=Act.cName;
  SetWindowText(Name());
#endif
  }


void CMdlValueSet::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMdlValueSet)
  DDX_Control(pDX, IDC_SLIDER1, m_Slider);
  //}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CMdlValueSet, CDialog)
  //{{AFX_MSG_MAP(CMdlValueSet)
  ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMdlValueSet message handlers


void CMdlValueSet::OnOK() 
  {
  if (IsDlgButtonChecked(IDC_CHECK1))
#if Vert
    Action.dValue=0.01*(100-m_Slider.GetPos())*(Action.dValueMax-Action.dValueMin)+Action.dValueMin;
#else
    Action.dValue=0.01*m_Slider.GetPos()*(Action.dValueMax-Action.dValueMin)+Action.dValueMin;
#endif
  else
    Action.dValue=dNAN;
  gs_Exec.SetModelAction(sTag(), Action);

  CDialog::OnOK();
  DestroyWindow();
  }

void CMdlValueSet::OnCancel() 
  {
  DestroyWindow();
  }

BOOL CMdlValueSet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
  {
  if (IsDlgButtonChecked(IDC_CHECK1))
#if Vert
    Action.dValue=0.01*(100-m_Slider.GetPos())*(Action.dValueMax-Action.dValueMin)+Action.dValueMin;
#else
    Action.dValue=0.01*m_Slider.GetPos()*(Action.dValueMax-Action.dValueMin)+Action.dValueMin;
#endif
  else
    Action.dValue=dNAN;
  gs_Exec.SetModelAction(sTag(), Action);

  if (IsDlgButtonChecked(IDC_CHECK1))
    {
    Strng S;
    S.Set("%6.2f", Action.dValue);
    SetDlgItemText( IDC_SMV_VALUE, S());
    }
  else
    {
    SetDlgItemText( IDC_SMV_VALUE, "*");
    }

  return CDialog::OnNotify(wParam, lParam, pResult);
  }

BOOL CMdlValueSet::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  Strng Name(sTag);
  Name+=".";
  Name+=Action.cName;
  //  SetWindowText(Name());
  //  SetDlgItemText( IDC_SMV_TAG, Name());

  m_Slider.SetRange(0,100);
  m_Slider.SetTicFreq(10);
#if Vert
  if (Valid(Action.dValue))
    m_Slider.SetPos(100-(int)((Action.dValue-Action.dValueMin)*100/GTZ(Action.dValueMax-Action.dValueMin)));
  else
    m_Slider.SetPos(100);
#else
  if (Valid(Action.dValue) && fabs(Action.dValueMax-Action.dValueMin)>1.0e-6)
    m_Slider.SetPos((int)Range(0.0,(Action.dValue-Action.dValueMin)*100/GTZ(Action.dValueMax-Action.dValueMin),100.0));
  else
    m_Slider.SetPos(0);
#endif
  CheckDlgButton( IDC_CHECK1, Valid(Action.dValue)?1:0);
  GetDlgItem(IDC_CHECK1)->EnableWindow(Action.fInvalidOK);
  GetDlgItem(IDC_SLIDER1)->EnableWindow(Valid(Action.dValue));
  RECT R;
  GetWindowRect(&R);
  MoveWindow(iX,iY, R.right-R.left, R.bottom-R.top);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

void CMdlValueSet::OnCheck1() 
  {
  if (IsDlgButtonChecked(IDC_CHECK1))
    {
    CheckDlgButton( IDC_CHECK1, 0);
    GetDlgItem(IDC_SLIDER1)->EnableWindow(false);
    }
  else
    {
    CheckDlgButton( IDC_CHECK1, 1);
    GetDlgItem(IDC_SLIDER1)->EnableWindow(true);

    CMdlActionArray CurrActs;
    if (gs_Exec.GetModelAction(sTag(), CurrActs))
      {
      if (Action.iIndex<CurrActs.GetSize())
        {
#if Vert
        if (Valid(Action.dBumpless))
          m_Slider.SetPos(100-(int)((Action.dBumpless-Action.dValueMin)*100/GTZ(Action.dValueMax-Action.dValueMin)));
        else
          m_Slider.SetPos(100);
#else
        if (Valid(Action.dBumpless))
          m_Slider.SetPos((int)((Action.dBumpless-Action.dValueMin)*100/GTZ(Action.dValueMax-Action.dValueMin)));
        else
          m_Slider.SetPos(0);
#endif
        }
      }
    }
  }

BOOL CMdlValueSet::DestroyWindow() 
  {
  gs_RTLCom.m_MVCritSect.Lock();
  POSITION Pos;
  Pos=gs_RTLCom.m_SetPtrs.Find(this);
  if (Pos)
    gs_RTLCom.m_SetPtrs.RemoveAt(Pos);
  gs_RTLCom.m_MVCritSect.Unlock();
  return CDialog::DestroyWindow();
  }

void CMdlValueSet::Add(char * pTag, CMdlAction & Act, int X, int Y, CWnd* pParent)
  {
  gs_RTLCom.m_MVCritSect.Lock();
  ValueSetPtr P;
  P=new CMdlValueSet(pTag, Act, X, Y, pParent);
  gs_RTLCom.m_SetPtrs.AddHead(P);
  gs_RTLCom.m_MVCritSect.Unlock();
  };

void CMdlValueSet::Clear()
  {
  gs_RTLCom.m_MVCritSect.Lock();
  while (!gs_RTLCom.m_SetPtrs.IsEmpty())
    {
    ValueSetPtr P;
    P=gs_RTLCom.m_SetPtrs.RemoveHead();
    P->DestroyWindow();
    delete P;
    }
  gs_RTLCom.m_MVCritSect.Unlock();
  }

void CMdlValueSet::StartUp()
  {
  };

void CMdlValueSet::ShutDown()
  {
  Clear();
  };

//===========================================================================
//
// CMdlValueShow dialog
//
//===========================================================================

const long TWideInit=40;
const long DWideInit=25;
const long CWideInit=25;

CMdlValueShow::CMdlValueShow(char * pTag, /*CMdlAction & Act,*/ int X, int Y, bool Pinned, CWnd* pParent /*=NULL*/)
: CDialog()//CMdlValueShow::IDD, pParent)
  {
  m_nTags=1;
  m_sTag[0]=pTag;  
  m_iX=X;
  m_iY=Y;
  m_bPinned=Pinned;
  m_bMustLoadTags=true;

  m_TWide=TWideInit;
  m_DWide=DWideInit;
  m_CWide=CWideInit;
  m_TotWide=10;
  m_TotDeep=10;
  m_bAllowMove=true;
  m_iSequence=-1;

  m_iChartItem=-1;
  for (int i=0; i<MdlValueShowMaxItems; i++)
    {
    m_ItemChartOn[i]=false;
    m_ItemChartWithZero[i]=false;
    }
  m_iTagSelected=0;
  m_ItemChartColourIndex=0;

  for (i=0; i<MdlValueShowMaxStatusBlks; i++)
    {
    m_StateShw[i]=0;
    for (int j=0; j<MdlValueShowMaxTags; j++)
      {
      m_StateCh[j][i]=0;
      m_StateCol[j][i]=0;
      }
    }

  //{{AFX_DATA_INIT(CMdlValueShow)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  Create(CMdlValueShow::IDD, pParent);
  SetWindowText(pTag);

  m_LastUpdateTicks=GetTickCount();
  }

void CMdlValueShow::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMdlValueShow)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CMdlValueShow, CDialog)
  //{{AFX_MSG_MAP(CMdlValueShow)
  ON_WM_PAINT()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_NCPAINT()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_CLOSE()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_UPDATESHOWWND, OnUpdateWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMdlValueShow message handlers

BOOL CMdlValueShow::DestroyWindow() 
  {
  gs_RTLCom.m_MVCritSect.Lock();
  POSITION Pos;
  Pos=gs_RTLCom.m_ValueShwPtrs.Find(this);
  if (Pos)
    gs_RTLCom.m_ValueShwPtrs.RemoveAt(Pos);
  gs_RTLCom.m_MVCritSect.Unlock();
  return CDialog::DestroyWindow();
  }

BOOL CMdlValueShow::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  AtStart(m_sTag[0](), m_iX, m_iY, m_bPinned);
  return TRUE;
  }

void CMdlValueShow::AtStart(char * pTag, int X, int Y, bool Pinned)
  {
  SetWindowText(pTag);
  m_nTags=1;
  m_sTag[0]=pTag;  
  m_iX=X;
  m_iY=Y;
  m_bPinned=Pinned;

  m_iChartItem=-1;
  for (int i=0; i<MdlValueShowMaxItems; i++)
    {
    m_ItemChartOn[i]=false;      
    m_ItemChartWithZero[i]=false;
    }
  long Style=GetWindowLong(m_hWnd, GWL_STYLE);
  if (Pinned)
    Style|=/*WS_CAPTION|*/WS_SYSMENU;
  else
    Style=Style&~(/*WS_CAPTION|*/WS_SYSMENU);

  SetWindowLong(m_hWnd, GWL_STYLE, Style);

  //  long ExStyle=GetWindowLong(m_hWnd, GWL_EXSTYLE);
  //  Style|=WS_EX_TOOLWINDOW;
  //  SetWindowLong(m_hWnd, GWL_EXSTYLE, ExStyle);

  m_TagStr="Collecting Data !";
  m_DataStr[0]=" ";
  m_CnvStr=" ";
  m_nLines=0;
  RECT R;
  GetWindowRect(&R);
  MoveWindow(m_iX,m_iY, R.right-R.left, R.bottom-R.top);
  BringWindowToTop();

  if (!gs_Exec.Busy() || gs_Exec.Waiting())
    DoTheUpdate(); 
  }

struct Defaults {char * Lbl; char * Tag; CCnvIndex CnvTyp; char * CnvTxt;};
static Defaults StdDefs[]= {    
  {"Qm",    "Qm",     DC_Qm,    "kg/s"},
  {"Qv",    "Qv",     DC_Qv,    "m^3/s"},
  {"T",     "T",      DC_T,     "C"},
  {"P",     "P",      DC_P,     "kPag"},
  {"Pi",    "Pi",     DC_P,     "kPag"},
  {"Po",    "Po",     DC_P,     "kPag"},
  {"Lvl",   "Lvl",    DC_Frac,  "%"},
  {"Rho",   "Rho",    DC_Rho,   "kg/m^3"},
  {"Value", "Value",  DC_Frac,  "%"},
  {NULL, NULL, DC_, ""}
  };

/*#if WithQAL
static Defaults QALDefs[]= {
{"Qm",                "Qm",               DC_Qm,    "t/h"},
{"Qv",                "Qv",               DC_Qv,    "kL/h"},
{"Po",                "Po",               DC_P,     "kPa"},
{"T",                 "T",                DC_T,     "C"},
{"Qo.CausticConc25",  "Qo.CausticConc25", DC_Conc,  "g/L"},
{"Qo.A/C",            "Qo.A/C",           DC_Frac,  ""},
{"Qo.SolidsConc25",   "Qo.SolidsConc25",  DC_Conc,  "g/L"},
{"Qo.BPE",            "Qo.BPE",           DC_dT,    "C"},
{NULL, NULL, DC_, ""}
};
#endif*/


const dword VisStates = FNS_InActive|
FNS_Hold|
FNS_Error|
FNS_Warning|
FNS_Tear|
FNS_MacMdl|
FNS_On|
FNS_Off|
FNS_IsSrc|FNS_NettQmP|
FNS_IsSnk|FNS_NettQmN|
FNS_UFlw|FNS_LFlwP|FNS_LFlwN|FNS_LFlwVap|FNS_LFlwSol|
FNS_UNoFlw|FNS_LNoFlw|FNS_LnkXfer|
FNS_Marked;

void CMdlValueShow::DoTheUpdateByMsg()
  {
  //dbgpln("CMdlValueShow::DoTheUpdateByMsg");

  if ((GetTickCount()>m_LastUpdateTicks+500) || (GetTickCount()<m_LastUpdateTicks))
    {
    PostMessage(WMU_UPDATESHOWWND, 0, 0);
    }
  }

#define BLDSTATE(iTag, iState, Ch, Col)   \
  {                                       \
  m_StateShw[iState]=true;                \
  m_StateCh[iTag][iState]=Ch;             \
  m_StateCol[iTag][iState]=Col;           \
    }

void CMdlValueShow::DoTheUpdate() 
  {
  //dbgpln("CMdlValueShow::DoTheUpdate");
  if (m_nTags<1)
    {
    m_LastUpdateTicks=GetTickCount();
    return;
    }
  SetWindowText(m_nTags>1 ? m_sObjClassId() : m_sTag[0]());

  m_TagStr="";
  m_DataStr[0]="";
  m_CnvStr="";
  m_nLines=0;

  CXM_ObjectData ObjData;
  CXM_Route      Route;

  flag ItemUsed[MdlValueShowMaxItems];
  Strng ItemTag[MdlValueShowMaxItems];
  Strng ItemCnv[MdlValueShowMaxItems];
  for (int iT=0; iT<MdlValueShowMaxItems; iT++)
    ItemUsed[iT]=0;

  //m_nStates=0;
  for (int i=0;i<MdlValueShowMaxStatusBlks; i++)
    {
    m_StateShw[i]=0;
    for (int iTag=0; iTag<m_nTags; iTag++)
      m_StateCh[iTag][i]='\0';
    }
  for (int iTag=0; iTag<m_nTags; iTag++)
    {
    m_DataStr[iTag]="";

    Strng_List sStrList;

    Strng T;
    T.Set("%s.ClassId", m_sTag[iTag]);

    CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);

    if (m_sTag[iTag].Length()==0)
      continue;

    if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
      {
      m_sObjClassId="";
      CPkDataItem * pItem=ObjData.FirstItem();
      if (IsStrng(pItem->Type()))
        {
        m_sObjClassId = pItem->Value()->GetString("", "", 0, "");
        StripClassIdVersion(m_sObjClassId);        
        }

      int nTagItems=0;
      if (m_bMustLoadTags)
        {
        m_bMustLoadTags=false;
        if (m_sObjClassId())
          {
          gs_RTLCom.LoadVarTags(m_sObjClassId(), NULL, m_TagItems, nTagItems);
          if (nTagItems==0)
            {
            gs_RTLCom.m_InBaseCfg =true;      
            gs_RTLCom.LoadVarTags(m_sObjClassId(), NULL, m_TagItems, nTagItems);
            gs_RTLCom.m_InBaseCfg =false;      
            }
          //CRptTagLists::Show(m_sObjClassId(), false);
          }

        if (nTagItems==0)
          {
          Defaults *Defs = &StdDefs[0];
          /*#if WithQAL
          if (QALOptions())
          Defs=&QALDefs[0];
          #endif*/

          for (int iT=0; Defs[iT].Tag; iT++)
            {
            if (iT>=MdlValueShowMaxItems)
              {
              LogWarning("View", 0, "Too many show Items");
              break;
              }
            m_TagItems.SetSize(Max(iT+1, m_TagItems.GetSize()),MdlValueShowMaxItems);
            m_TagItems[iT].m_sLabel=Defs[iT].Lbl;
            m_TagItems[iT].m_sTag=Defs[iT].Tag;
            m_TagItems[iT].m_Fmt.Set("XX", 0, 2, 'f');
            m_TagItems[iT].m_Cnv.Set(Defs[iT].CnvTyp, Defs[iT].CnvTxt);
            }
          }
        else
          {
          if (nTagItems>=MdlValueShowMaxItems)
            {
            LogWarning("View", 0, "Too many show Items");
            m_TagItems.SetSize(MdlValueShowMaxItems,MdlValueShowMaxItems);
            }
          }
        }
      nTagItems=m_TagItems.GetSize();

      if (1)
        {
        T.Set("%s.Condition", m_sTag[iTag]());
        CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
        CXM_ObjectData ObjData;
        CXM_Route      Route;
        if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
          {
          CPkDataItem * pItem=ObjData.FirstItem();
          m_CondStr[iTag]="";
          if (IsStrng(pItem->Type()))
            m_CondStr[iTag]= pItem->Value()->GetString();
          else
            LogError("View", 0, "Tag '%s' not allowed.", T());
          }
        }

      for (int iT=0; iT<nTagItems; iT++)
        {
        T.Set("%s.%s", m_sTag[iTag](), m_TagItems[iT].m_sTag());
        CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
        CXM_ObjectData ObjData;
        CXM_Route      Route;

        if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
          {
          CPkDataItem * pItem=ObjData.FirstItem();
          int cType=pItem->Type();
          Strng &Lbl=m_TagItems[iT].m_sLabel;
          Strng &Tag=m_TagItems[iT].m_sTag;
          CnvAttribute &Cnv=m_TagItems[iT].m_Cnv;
          FmtAttribute &Fmt=m_TagItems[iT].m_Fmt;

          Cnv.SetIndex(pItem->CnvIndex());
          if ((Cnv.TextLength()==0) || (Cnvs[Cnv.Index()]->Find(Cnv.Text())==NULL))
            Cnv.SetText(pItem->CnvTxt());

          Strng UseCnvTxt;//(Cnv.Text);
          CDataCnv & C=*Cnvs[Cnv.Index()];
          if (C.OverTxt())
            UseCnvTxt=C.OverTxt();
          else if (Cnv.TextLength()>0)
            UseCnvTxt=Cnv.Text();
          else
            UseCnvTxt=" ";

          if (!ItemUsed[iT])
            {
            ItemUsed[iT]=1;
            ItemTag[iT]=Lbl();
            ItemCnv[iT]=UseCnvTxt();
            }

          sStrList.Clear();
          if (pItem->Contains(PDI_StrList))
            pItem->GetStrList(sStrList);

          Strng sValue("???");
          if (IsNumData(cType))
            {
            if (IsFloatData(cType))
              {
              double dValue=pItem->Value()->GetDouble(Cnv.Index(), UseCnvTxt());
              Fmt.FormatFloat(dValue, sValue);
              }
            else if (sStrList.Length()>0)
              {
              long i=pItem->Value()->GetLong(Cnv.Index(), UseCnvTxt());
              pStrng p=sStrList.AtIndexVal(i);
              if (p)
                sValue.Set("%i-%s", i, p->Str());
              else
                sValue.Set("%i", i);
              }
            else
              sValue = pItem->Value()->GetString("%i", "%g", Cnv.Index(), UseCnvTxt());
            }
          else if (IsData(cType))
            {
            sValue = pItem->Value()->GetString("%i", "%g", Cnv.Index(), UseCnvTxt());
            }
          else
            LogError("View", 0, "Tag '%s' not allowed.", T());

          if (!sValue.Length())
            sValue=" ";

          if (iT)
            m_DataStr[iTag]+="\r";

          if (sValue.Length()<gs_RTLCom.m_lMaxValueLen)
            m_DataStr[iTag]+=sValue();
          else
            {
            Strng S=sValue();
            S.SetLength(gs_RTLCom.m_lMaxValueLen-3);
            S+="...";
            m_DataStr[iTag]+=S();
            }
#if dbgBuild     
          dbgpln("%s = %s", T(),sValue());
#endif     
          }
        else
          {
#if dbgBuild     
          dbgpln("%s - MISSING", T());
#endif     
          if (iT)
            m_DataStr[iTag]+="\r";
          }
        }

      if (gs_pTheSFELib && m_sTag[iTag]())
        {
        if (gs_pPrj->ShowStatus()) //would be better if we could get the status here even if graphics colour status displays are off
          {
          dword Status=gs_pTheSFELib->FE_GetModelStatus(m_sTag[iTag]());
          if ((Status&VisStates)!=0)
            {
            int iState=0;
            if (Status & FNS_InActive)
              {
              BLDSTATE(iTag, iState, 'i', GR_MDLSTATUS_INACTIVE)
                iState++;
              }
            else
              {
              //if (Status & FNS_Marked)
              //  BLDSTATE(iTag, iState, '*', GR_HIGHLIGHT) 
              //iState++;

              if (Status & FNS_Error)
                BLDSTATE(iTag, iState, 'e', GR_MDLSTATUS_ERROR) 
                iState++;

              if (Status & FNS_Warning)
                BLDSTATE(iTag, iState, 'w', GR_MDLSTATUS_WARNING)
                iState++;

              if (Status & FNS_Tear)
                BLDSTATE(iTag, iState, 't', GR_MDLSTATUS_TEAR)
                iState++;

              if (Status & FNS_MacMdl)
                BLDSTATE(iTag, iState, 'm', GR_MDLSTATUS_MACMDL)
                iState++;

              if (Status & FNS_On)
                BLDSTATE(iTag, iState, '!', GR_MDLSTATUS_ON)
              else if (Status & FNS_Off)
              BLDSTATE(iTag, iState, '.', GR_MDLSTATUS_OFF)
              iState++;

              if (Status & (FNS_IsSrc|FNS_NettQmP))
                {
                if (Status & FNS_IsSrc)
                  BLDSTATE(iTag, iState, '+', GR_MDLSTATUS_ISSRC)
                else
                BLDSTATE(iTag, iState, '+', GR_MDLSTATUS_NETTQMP)
                }
              else if (Status & (FNS_IsSnk|FNS_NettQmN))
                {
                if (Status & FNS_IsSnk)
                  BLDSTATE(iTag, iState, '-', GR_MDLSTATUS_ISSNK)
                else
                BLDSTATE(iTag, iState, '-', GR_MDLSTATUS_NETTQMN)
                }
              iState++;

              if (Status & (FNS_UFlw|FNS_LFlwP|FNS_LFlwN|FNS_LFlwVap|FNS_LFlwSol))
                {
                if (Status & FNS_UFlw)
                  BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_UFLW)
                else if (Status & FNS_LFlwVap)
                BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_LFLWVAP)
                else if (Status & FNS_LFlwSol)
                BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_LFLWSOL)
                else if (Status & FNS_LFlwP)
                BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_LFLWP)
                else
                BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_LFLWN)
                }
              else if (Status & (FNS_UNoFlw|FNS_LNoFlw))
                {
                if (Status & FNS_UNoFlw)
                  BLDSTATE(iTag, iState, '0', GR_MDLSTATUS_UNOFLW)
                else
                BLDSTATE(iTag, iState, '0', GR_MDLSTATUS_LNOFLW)
                }
              iState++;

              if (Status & FNS_Hold)
                BLDSTATE(iTag, iState, 'h', GR_MDLSTATUS_INACTIVE)
                iState++;
              }
            }
          }
        }
      }
    }

  long nRemoved=0;
  for (iT=0; iT<MdlValueShowMaxItems; iT++)
    {
    if (!ItemUsed[iT])
      {
      // Remove Extra Blank Fields
      for (int iTag=0; iTag<m_nTags; iTag++)
        if (m_DataStr[iTag].Length()>0)
          {
          char Buff[4096];
          strcpy(Buff, m_DataStr[iTag]());
          char * p=Buff;
          int i=0;
          while (p && i++<iT-nRemoved)
            {
            if (p=strchr(p, '\r'))
              p=p+1;
            }
          if (p  &&strlen(p))
            {
            memmove(p, p+1, strlen(p)+1);
            // Done Restore
            m_DataStr[iTag]=Buff;
            }
          }
        nRemoved++;
      }
    else
      {
      if (m_nLines)
        {
        m_TagStr+="\r";
        m_CnvStr+="\r";
        }
      m_TagStr+=ItemTag[iT]();
      m_CnvStr+=ItemCnv[iT]();
      m_nLines++;
      }
    }
  while (m_nLines<MinShownLines)
    {
    if (m_nLines)
      {
      m_TagStr+="\r";
      m_CnvStr+="\r";              
      for (int iTag=0; iTag<m_nTags; iTag++);
      m_DataStr[iTag]+="\r";
      }
    m_TagStr+=" ";
    m_CnvStr+=" ";
    for (int iTag=0; iTag<m_nTags; iTag++);
    m_DataStr[iTag]+=" ";
    m_nLines++;
    }

  m_bAllowMove=true;

  Invalidate();

  m_LastUpdateTicks=GetTickCount();
  }

static COLORREF Colours[]=
  {
  GR_LIGHTBLUE,
  GR_LIGHTGREEN,
  GR_LIGHTRED,
  GR_LIGHTYELLOW,
  GR_LIGHTMAGENTA,
  GR_LIGHTCYAN,
  GR_WHITE,
  GR_BLACK,
  GR_BLUE,
  GR_GREEN,
  GR_RED,
  GR_MAGENTA,
  GR_CYAN,
  };
static long nColours=sizeof(Colours)/sizeof(Colours[0]);

void CMdlValueShow::OnPaint() 
  {
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  CRect ClientRct;
  GetClientRect(&ClientRct);

  CFont * OldFont=dc.SelectObject(GetFont()); // Italic
  int OldBkMode=dc.SetBkMode(TRANSPARENT);
  CPen * OldPen=dc.GetCurrentPen();

  GetClientRect(&ClientRct);
  CRect RctX;
  RctX.SetRect(0,0,1,1);

  CRect RctT=RctX;
  CRect RctC=RctX;
  CRect RctCI=RctX;
  CRect RctD[MdlValueShowMaxTags];
  for (int iTag=0; iTag<m_nTags; iTag++)
    RctD[iTag]=RctX;

  dc.DrawText("X",  1,  RctX, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 

  long NBlks=0;
  for (int i=0;i<MdlValueShowMaxStatusBlks; i++)
    if(m_StateShw[i])
      NBlks++;
  ASSERT_ALWAYS(NBlks<=MdlValueShowMaxStatusBlks, "Too Many Status Blocks", __FILE__, __LINE__);

    long BlkW=RctX.Width();
  long BlkH=RctX.Height();
  long BlkWTot=BlkW*NBlks;

  if (m_TagStr.Length())
    dc.DrawText(m_TagStr(),  m_TagStr.Length(),  RctT, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
  if (m_CnvStr.Length())
    dc.DrawText(m_CnvStr(),  m_CnvStr.Length(),  RctC, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
  for (iTag=0; iTag<m_nTags; iTag++)
    {
    if (m_DataStr[iTag].Length())
      dc.DrawText(m_DataStr[iTag](), m_DataStr[iTag].Length(), RctD[iTag], DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_RIGHT); 
    }

  //flag DoCond=(m_CondStr.Length()>0);

  //  dword StatusA[MdlValueShowMaxTags];
  //  for (iTag=0; iTag<m_nTags; iTag++)
  //    StatusA[iTag]=0;

  flag DoStatus=true;

  m_iTagSelected=Range(0L, m_iTagSelected, m_nTags);

  int nStatus=1;
  int nConds=1;
  char * p=m_CondStr[m_iTagSelected]();
  if (p)
    {
    while ((p=strchr(p, '\n'))!=NULL)
      {
      p++;
      nConds++;
      }
    dc.DrawText(m_CondStr[m_iTagSelected](),  m_CondStr[m_iTagSelected].Length(),  RctCI, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
    }

  long XW=RctX.right;
  long XW2=RctX.right/2;

  m_TWide=Max(m_TWide, RctT.right+XW2);
  m_CWide=Max(m_CWide, RctC.right+XW2);
  m_DWide=Max(m_DWide, BlkWTot/*+3*XW*/);
  for (iTag=0; iTag<m_nTags; iTag++)
    m_DWide=Max(m_DWide, RctD[iTag].right/*+XW*/);
  m_TotWide=Max(m_TWide+XW+m_nTags*(m_DWide+XW)+m_CWide+XW2, (long)RctCI.Width());


  flag DoChart=0;
  if (m_nTags>1)
    {
    DoChart=m_iChartItem>=0;
    for (int i=0; i<m_nLines; i++)
      DoChart = DoChart || m_ItemChartOn[i];
    }

  long TagLine=(m_nTags>1)? 1:0;
  long LineHgt=RctX.Height();
  long CondDeep=nConds*LineHgt;
  long ChartRows=4;
  long ChartDeep= DoChart ? ChartRows*LineHgt : 0;
  long StatusDeep=1*LineHgt;
  long HeadDeep= TagLine*LineHgt;

  m_TotDeep=RctT.bottom+HeadDeep+CondDeep+StatusDeep+ChartDeep;

  CRect DTRct;
  AfxGetMainWnd()->GetWindowRect(&DTRct);
  CRect WRct;
  GetWindowRect(&WRct);
  long ICx=GetSystemMetrics(SM_CXSMICON);
  long ICy=GetSystemMetrics(SM_CYSMICON);
  long ICyC=GetSystemMetrics(SM_CYSMCAPTION);

  bool DoMove=false;
  if (m_bAllowMove)
    {
    if (m_TotWide>ClientRct.right || m_TotDeep>ClientRct.bottom || m_TotDeep<ClientRct.bottom)
      {
      WRct.right+=m_TotWide-ClientRct.right;
      WRct.bottom+=m_TotDeep-ClientRct.bottom;
      DoMove=true;
      }

    int XErrL=Min(0L, WRct.right-(DTRct.left+m_TotWide));
    int XErrR=Max(0L, WRct.left-(DTRct.right-m_TotWide));
    int YErrT=Min(0L, WRct.top-DTRct.top);
    int YErrB=Max(0L, WRct.top/*+ICyC*/-(DTRct.bottom-(m_TotDeep+ICyC)));

    if (XErrL || XErrR || YErrT || YErrB)
      {
      WRct.OffsetRect(-(XErrL+XErrR), -(YErrT+YErrB));
      DoMove=true;
      }
    }

  if (DoMove)
    {
    m_bAllowMove=false;
    Invalidate();
    MoveWindow(WRct);
    }
  else
    {
    CPen Pen1(BS_SOLID,1,RGB(0x80, 0x80, 0x80));
    CPen Pen2(BS_SOLID,2,RGB(0x80, 0x80, 0x80));

    CPen*pPenMem=dc.SelectObject(&Pen1);
    int iItem=nStatus>0 ? -1:0;
    for (int i=TagLine+nConds; i<=m_nLines+TagLine+nConds+nStatus; i++)
      {
      int y=i*LineHgt+1;
      POINT Pts[2];
      Pts[0].x=0;
      Pts[0].y=y;
      Pts[1].x=ClientRct.right;
      Pts[1].y=y;
      dc.Polyline(Pts, 2);

      if (iItem>=0)
        m_ItemClientRow[iItem]=y;
      iItem++;
      }
    if (iItem)
      m_ItemClientRow[iItem]=m_ItemClientRow[iItem-1]+LineHgt;
    int StatusStart=HeadDeep+1+CondDeep;
    int ItemStart=StatusStart+LineHgt;
    int ItemEnd=ItemStart+m_nLines*LineHgt;

    if (m_nTags<2)
      {
      POINT Pts[2];
      Pts[0].x=m_TWide+XW2;
      Pts[0].y=StatusStart;
      Pts[1].x=Pts[0].x;
      Pts[1].y=ClientRct.bottom;
      dc.Polyline(Pts, 2);
      Pts[0].x+=m_DWide+XW;
      Pts[1].x=Pts[0].x;
      dc.Polyline(Pts, 2);
      }
    else
      {
      POINT Pts[5];
      Pts[0].x=m_TWide+XW2;
      Pts[0].y=StatusStart;
      Pts[1].x=Pts[0].x;
      Pts[1].y=ItemEnd;
      dc.Polyline(Pts, 2);
      Pts[0].x+=m_CWide+XW;
      Pts[1].x=Pts[0].x;
      Pts[1].y=ClientRct.bottom;
      dc.Polyline(Pts, 2);
      m_TagClientCol[0]=Pts[0].x;
      for (iTag=0; iTag<m_nTags; iTag++)
        {
        Pts[0].x+=m_DWide+XW;
        Pts[1].x=Pts[0].x;
        dc.Polyline(Pts, 2);
        m_TagClientCol[iTag+1]=Pts[0].x;
        }
      // Hilight Selected Tag
      if (1)
        {
        CPen*pPenMem=dc.SelectObject(&Pen2);

        CRect SlctRct;
        SlctRct.top=StatusStart;
        SlctRct.bottom=ClientRct.bottom+1;
        SlctRct.left=m_TWide+XW2+m_CWide+XW+(m_iTagSelected)*(m_DWide+XW);
        SlctRct.right=SlctRct.left+(m_DWide+XW)+1;

        Pts[0]=SlctRct.TopLeft();
        Pts[1]=SlctRct.TopLeft();
        Pts[2]=SlctRct.BottomRight();
        Pts[3]=SlctRct.BottomRight();
        Pts[4]=Pts[0];

        Pts[1].x=Pts[2].x;
        Pts[3].x=Pts[0].x;
        dc.Polyline(Pts, 5);
        dc.SelectObject(pPenMem);
        }
      }
    dc.SelectObject(pPenMem);

    CRect Rct=ClientRct;

    if (1)
      {
      Rct=ClientRct;
      Rct.top+=HeadDeep+1;
      Rct.bottom+=HeadDeep+1;
      dc.DrawText(m_CondStr[m_iTagSelected](), m_CondStr[m_iTagSelected].Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      }

    if (m_nTags>1)
      {
      Strng sTag("Tag:");
      sTag+=m_sTag[m_iTagSelected];

      Rct=ClientRct;
      dc.DrawText(sTag(), sTag.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 

      if (DoChart)
        {
        for (int iT=0; iT<m_nLines; iT++)
          {
          if (m_ItemChartOn[iT] || (iT==m_iChartItem))
            {
            Rct=ClientRct;
            Rct.OffsetRect(Rct.left+m_TWide, ItemStart+iT*LineHgt);
            Rct.right=Rct.left+(XW*2)/3;
            Rct.bottom=Rct.top+LineHgt;

            byte ColInd=m_ItemChartOn[iT] ? m_ItemChartColour[iT] : 0;
            if (m_ItemChartWithZero[iT])
              {
              int x=Rct.Width()/2;
              int y=Rct.Height()/2;
              dc.FillSolidRect(Rct.left, Rct.top, x, y, grl_get_win_color(Colours[ColInd]));
              dc.FillSolidRect(Rct.left+x, Rct.top+y, Rct.Width()-x, Rct.Height()-y, grl_get_win_color(Colours[ColInd]));
              }
            else
              dc.FillSolidRect(Rct.left, Rct.top, Rct.Width(), Rct.Height(), grl_get_win_color(Colours[ColInd]));
            }
          }
        }

      if (DoStatus)
        {
        Strng sStatus("Status");
        Rct=ClientRct;
        Rct.OffsetRect(0, StatusStart);
        Rct.right=Rct.left+m_TWide;
        dc.DrawText(sStatus(), sStatus.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
        }

      Rct=ClientRct;
      Rct.OffsetRect(0, ItemStart);
      Rct.right=Rct.left+m_TWide;
      dc.DrawText(m_TagStr(), m_TagStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      Rct.OffsetRect(m_TWide+XW, 0);

      Rct.right=Rct.left+m_CWide;
      dc.DrawText(m_CnvStr(), m_CnvStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      Rct.OffsetRect(m_CWide+XW, 0);

      long iPos=0;
      for (iTag=0; iTag<m_nTags; iTag++)
        {
        Rct.right=Rct.left+m_DWide;
        dc.DrawText(m_DataStr[iTag](), m_DataStr[iTag].Length(), Rct, DT_TOP|DT_NOPREFIX|DT_RIGHT); 
        Rct.OffsetRect(m_DWide+XW, 0);
        }
      }
    else
      {
      if (DoStatus)
        {
        Strng sStatus("Status");
        Rct=ClientRct;
        Rct.OffsetRect(0, StatusStart);
        Rct.right=Rct.left+m_TWide;
        dc.DrawText(sStatus(), sStatus.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
        }
      Rct=ClientRct;
      Rct.OffsetRect(0, ItemStart);
      Rct.right=Rct.left+m_TWide;
      dc.DrawText(m_TagStr(), m_TagStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      Rct.OffsetRect(m_TWide+XW, 0);

      Rct.right=Rct.left+m_DWide;
      dc.DrawText(m_DataStr[0](), m_DataStr[0].Length(), Rct, DT_TOP|DT_NOPREFIX|DT_RIGHT); 
      Rct.OffsetRect(m_DWide+XW, 0);

      Rct.right=Rct.left+m_CWide;
      dc.DrawText(m_CnvStr(), m_CnvStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
      }

    if (DoStatus)
      {
      for (iTag=0; iTag<m_nTags; iTag++)
        {
        long XOff=m_TWide+XW2+(iTag+1)*(m_DWide+XW)-BlkWTot;
        if (m_nTags>1)
          XOff+=m_CWide+XW;

        Rct=ClientRct;
        Rct.OffsetRect(XOff+(NBlks-1)*BlkW, StatusStart+1);
        Rct.right=Rct.left+BlkW;
        Rct.bottom=Rct.top+BlkH;
        //dword Status=StatusA[iTag];
        long StateW=BlkW;
        long StateH=BlkH-1;

        for (int i=MdlValueShowMaxStatusBlks-1; i>=0; i--)
          {
          if(m_StateShw[i] && m_StateCh[iTag][i])
            {
            dc.FillSolidRect(Rct.left,Rct.top,StateW,StateH, grl_get_win_color(m_StateCol[iTag][i]));
            dc.DrawText(&m_StateCh[iTag][i], 1, Rct, DT_VCENTER|DT_NOPREFIX|DT_CENTER); 
            Rct.OffsetRect(-StateW,0);
            }
          }
        }
      }
    if (DoChart)
      {
      int nCharts=0;
      for (int iT=0; iT<m_nLines; iT++)
        {
        if (m_ItemChartOn[iT] || (iT==m_iChartItem))
          nCharts++;
        }
      long BarGap=2;
      long BarWide=m_DWide/Max(nCharts,4)-BarGap;
      long iPos=0;

      for (iT=0; iT<m_nLines; iT++)
        {
        if (m_ItemChartOn[iT] || (iT==m_iChartItem))
          {
          double Vals[MdlValueShowMaxTags];
          // Extract Vals
          double MaxV=-1.0e30;
          double MinV=+1.0e30;
          for (int iTag=0; iTag<m_nTags; iTag++)
            {
            char * p=m_DataStr[iTag]();
            int i=0;
            while (p && i++<iT)
              {
              if (p=strchr(p, '\r'))
                p=p+1;
              }
            Vals[iTag]=(p) ? SafeAtoF(p) : 0;
            MaxV=Max(MaxV, Vals[iTag]);
            MinV=Min(MinV, Vals[iTag]);
            }
          if (m_ItemChartWithZero[iT])
            {
            MaxV=Max(MaxV, 0.0);
            MinV=Min(MinV, 0.0);
            }
          Rct=ClientRct;
          Rct.OffsetRect(m_TWide+XW+m_CWide+XW+iPos*(BarWide+BarGap)-BarGap, ItemStart+2+m_nLines*LineHgt);
          Rct.right=Rct.left+BarWide;
          Rct.bottom=ClientRct.bottom;
          for (iTag=0; iTag<m_nTags; iTag++)
            {
            byte ColInd=m_ItemChartOn[iT] ? m_ItemChartColour[iT] : 0;
            if (0)
              {
              long Drop=long((Rct.Height()-1)*(MaxV-Vals[iTag])/GTZ(MaxV-MinV));
              dc.FillSolidRect(Rct.left,Rct.top+Drop,Rct.Width(), Rct.Height()-Drop, grl_get_win_color(Colours[ColInd]));
              }
            else
              {
              double R=GTZ(MaxV-MinV);
              long H=(Rct.Height()-2);
              long T,B;
              if (Vals[iTag]>0.0)
                {
                T=long(H*(Vals[iTag]-MinV)/R);
                B=long(H*(Max(0.0, MinV)-MinV)/R);
                }
              else
                {
                T=long(H*(Min(0.0, MaxV)-MinV)/R);
                B=long(H*(Vals[iTag]-MinV)/R);
                }
              T=1+H-T;
              B=1+H-B;
              dc.FillSolidRect(Rct.left,Rct.top+T,Rct.Width(), B-T, grl_get_win_color(Colours[ColInd]));
              }
            Rct.OffsetRect(m_DWide+XW, 0);
            }
          iPos++;
          }
        }
      }
    }

  dc.SetBkMode(OldBkMode);
  dc.SelectObject(OldFont);
  dc.SelectObject(OldPen);

  // Do not call CDialog::OnPaint() for painting messages
  }

void CMdlValueShow::TryAdd(char * pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent)
  {
  if (dbgValueShow)
    dbgpln("CMdlValueShow::TryAdd");

  if (1)//gs_Exec.Busy()) //thread problem ---
    {
    CAddRequestPtr Add = new CRptTagsAddRqst;

    Add->m_Tag=pTag;
    Add->m_X=X;
    Add->m_Y=Y;
    Add->m_Pinned=Pinned;
    Add->m_Append=Append;
    Add->m_pParent=pParent;

    gs_RTLCom.m_MVCritSect.Lock();	
    gs_RTLCom.m_ValueAddPtrs.AddTail(Add);
    gs_RTLCom.m_MVCritSect.Unlock();
    if (!gs_Exec.Busy() || gs_Exec.Waiting())
      UpdateAll();
    }
  else
    {
    Add(pTag, X, Y, Pinned, Append, pParent);
    }

  //Sleep(500); // Temp
  }

void CMdlValueShow::Add(char * pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent)
  {
  Strng T, ObjClassId;
  T.Set("%s.ClassId", pTag);

  if (dbgValueShow)
    dbgpln("CMdlValueShow::Add - Start");

  CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route      Route;

  if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem=ObjData.FirstItem();
    if (IsStrng(pItem->Type()))
      {
      ObjClassId = pItem->Value()->GetString("", "", 0, "");
      StripClassIdVersion(ObjClassId);
      }
    }
  else
    LogError((char*)pTag, 0, "ClassId not found");

  gs_RTLCom.m_MVCritSect.Lock();	
  if (Append)
    {
    POSITION Pos=gs_RTLCom.m_ValueShwPtrs.GetHeadPosition();
    while (Pos)
      {
      ValueShwPtr P=gs_RTLCom.m_ValueShwPtrs.GetNext(Pos);
      if (P->AcceptNewTag(pTag, ObjClassId()))
        {
        P->DoTheUpdate();
        gs_RTLCom.m_MVCritSect.Unlock();	
        return;
        }
      }
    }

  if (dbgValueShow)
    dbgpln("CMdlValueShow::Add - AddIt");

  ValueShwPtr P=new CMdlValueShow(pTag, X, Y, Pinned, pParent);
  P->m_iSequence=gs_RTLCom.m_iSequence;
  gs_RTLCom.m_ValueShwPtrs.AddHead(P);
  //  P->AtStart(pTag, X, Y, Pinned);
  P->ShowWindow(SW_SHOWNOACTIVATE);

  gs_RTLCom.m_MVCritSect.Unlock();	

  };

void CMdlValueShow::TryClear(bool All)
  {
  gs_RTLCom.m_MVCritSect.Lock();	
  
  if (dbgValueShow)
    dbgpln("CMdlValueShow::TryClear");
  
  gs_RTLCom.m_iClear=All?2:1;
  gs_RTLCom.m_MVCritSect.Unlock();
  if (!gs_Exec.Busy() || gs_Exec.Waiting())
    UpdateAll();
  }

bool CMdlValueShow::Clear(bool All)
  {
  bool Ret=true;
  gs_RTLCom.m_MVCritSect.Lock();	
  
  if (dbgValueShow)
    dbgpln("CMdlValueShow::Clear %s", All?"All":"");
  
  if (All)
    {
    while (!gs_RTLCom.m_ValueShwPtrs.IsEmpty())
      {
      ValueShwPtr P=gs_RTLCom.m_ValueShwPtrs.GetHead();
      P->DestroyWindow();
      delete P;
      }
    while (!gs_RTLCom.m_ValueAddPtrs.IsEmpty())
      {
      CAddRequestPtr P=gs_RTLCom.m_ValueAddPtrs.GetHead();
      delete P;
      }
    }
  else
    {
    POSITION Pos=gs_RTLCom.m_ValueShwPtrs.GetHeadPosition();
    while (Pos)
      {
      POSITION DelP=Pos;
      ValueShwPtr P=gs_RTLCom.m_ValueShwPtrs.GetNext(Pos);
      if (!P->m_bPinned)
        {
        if (P->m_iSequence==gs_RTLCom.m_iSequence)
          Ret=false;
        else
          {
          P->ShowWindow(SW_HIDE);
          P->DestroyWindow();
          delete P;
          }
        }
      }
    }
  gs_RTLCom.m_MVCritSect.Unlock();
  return Ret;
  }

void CMdlValueShow::UpdateAll()
  {
  gs_RTLCom.m_MVCritSect.Lock();
  
  if (dbgValueShow)
    dbgpln("CMdlValueShow::UpdateAll");

  while (!gs_RTLCom.m_ValueAddPtrs.IsEmpty())
    {
    CAddRequestPtr A=gs_RTLCom.m_ValueAddPtrs.RemoveHead();
    if (gs_RTLCom.m_iClear==0)
      Add(A->m_Tag(), A->m_X, A->m_Y, A->m_Pinned, A->m_Append, A->m_pParent);
    if (dbgValueShow)
      dbgpln("                             ADD   >>>>>>> %s", A->m_Tag());
    //    P->DoTheAddByMsg(A);
    delete A;
    }

  POSITION Pos=gs_RTLCom.m_ValueShwPtrs.GetHeadPosition();
  while (Pos)
    {
    ValueShwPtr P=gs_RTLCom.m_ValueShwPtrs.GetNext(Pos);
    P->DoTheUpdateByMsg();
    }
  if (gs_RTLCom.m_iClear)
    {
    if (dbgValueShow)
      dbgpln("                             Clear >>>>>>> %s",gs_RTLCom.m_iClear==2?"All":"");
    
    if (Clear(gs_RTLCom.m_iClear==2))
      gs_RTLCom.m_iClear=0;
    }
  gs_RTLCom.m_iSequence++;
  if (gs_RTLCom.m_iSequence>1000)
    gs_RTLCom.m_iSequence=0;
  gs_RTLCom.m_MVCritSect.Unlock();	
  }

LRESULT CMdlValueShow::OnUpdateWnd(WPARAM wParam, LPARAM lParam)
  {
  DoTheUpdate();
  return 0;
  }

void CMdlValueShow::DeleteItem(char * pObjClassId, int Index)
  {
  if (Index<0)
    return;
  Strng Sect, Item;
  Sect.Set("%s.View.Vars", pObjClassId);
  CProfINIFile PF(gs_RTLCom.RTLFileName());
  Item.Set("V%i",Index);
  PF.WrStr(Sect(), Item(), "");
  };

void CMdlValueShow::AddItem(TagInfoBlk & IB)
  {
  CRptTagLists::AddItem(IB);
  }

flag CMdlValueShow::FindItem(char * pObjClassId, int Index, CVarTagItem & TagItem)
  {
  Strng ObjClassId(pObjClassId);
  StripClassIdVersion(ObjClassId);
  Strng Sect, Item;
  Sect.Set("%s.View.Vars", ObjClassId());
  CProfINIFile PF(gs_RTLCom.RTLFileName());
  Item.Set("V%i",Index);
  Strng Val(PF.RdStr(Sect(), Item(), ""));//, Val()));

  char * pData=Val();
  if(pData)
    {
    char* pFld[10];
    for (int nFlds=0; pData; nFlds++)
      {
      if (nFlds)
        {
        *pData=0; // Terminate Fld
        pData++; // Point @ Data
        }
      pFld[nFlds]=pData;
      pData=strchr(pData, ',');
      }
    for (int i=0; i<nFlds; i++)
      XStrTrim(pFld[i]);

    TagItem.m_Fmt.SetType('-');

    TagItem.m_sLabel=nFlds>0 && pFld[0] ? pFld[0] : "";
    TagItem.m_sTag=nFlds>1 && pFld[1] ? pFld[1] : "";
    TagItem.m_Cnv.SetIndex(nFlds>2 && pFld[2] ? pFld[2] : "");
    TagItem.m_Cnv.SetText(nFlds>3 && pFld[3] ? pFld[3] : "");
    TagItem.m_Fmt.ParseStr(nFlds>4 && pFld[4] ? pFld[4] : "");

    return True;
    }
  return False;
  }

void CMdlValueShow::StartUp()
  {
  };

void CMdlValueShow::ShutDown()
  {
  Clear(true);
  };

void CMdlValueShow::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlValueShow::OnLButtonDblClk");
  CDialog::OnLButtonDblClk(nFlags, point);
  TryClear(true);
  }

void CMdlValueShow::OnMouseMove(UINT nFlags, CPoint point) 
  {
  CDialog::OnMouseMove(nFlags, point);
  //  if ((nFlags & MK_LBUTTON) && (nFlags & (MK_SHIFT|MK_CONTROL)))
  //    {
  //    int i=m_iChartItem;
  //    if (point.y>=m_ItemClientRow[0])
  //      {
  //      i=0;
  //      while (i<m_nLines&& m_ItemClientRow[i+1]<point.y)
  //        i++;
  //      m_iChartWithZero=((nFlags & MK_CONTROL)!=NULL);      
  //      DoTheUpdate();
  //      }
  //    else
  //      i=-1;
  //    if (i!=m_iChartItem)     
  //      {
  //      m_iChartWithZero=((nFlags & MK_CONTROL)!=NULL);      
  //      DoTheUpdate();
  //      }
  //    }
  }

//static Strng m_sDragTag;
CMdlValueShow * CMdlValueShow::m_pStartWnd=NULL;

void CMdlValueShow::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlValueShow::OnLButtonDown");
  CDialog::OnLButtonDown(nFlags, point);
  if (nFlags & (MK_SHIFT|MK_CONTROL))
    {
    if (point.y>=m_ItemClientRow[0])
      {
      m_iChartItem=0;
      while (m_iChartItem<m_nLines&& m_ItemClientRow[m_iChartItem+1]<point.y)
        m_iChartItem++;
      m_ItemChartWithZero[m_iChartItem]=((nFlags & MK_CONTROL)!=NULL);      
      DoTheUpdate();
      }
    else
      m_iChartItem=-1;
    }
  else
    {
    m_pStartWnd=this;
    }
  }

void CMdlValueShow::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlValueShow::OnLButtonUp");
  CDialog::OnLButtonUp(nFlags, point);
  if (m_pStartWnd && m_pStartWnd!=this)
    {
    int n=0;
    for (int i=0; i<m_pStartWnd->m_nTags; i++)
      if (AcceptNewTag(m_pStartWnd->m_sTag[i](), m_pStartWnd->m_sObjClassId()))
        n++;
    if (n)
      {
      m_pStartWnd->DestroyWindow();
      delete m_pStartWnd;
      DoTheUpdate();
      }
    }
  else if (m_iChartItem>=0)
    {
    m_iChartItem=-1;
    DoTheUpdate();
    }
  else
    {
    int i=0;
    while (i<m_nTags && m_TagClientCol[i+1]<point.x)
      i++;
    if (i<m_nTags && i!=m_iTagSelected)
      {
      m_iTagSelected=i;
      DoTheUpdate();
      }
    }
  m_pStartWnd=NULL;
  }

void CMdlValueShow::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlValueShow::OnRButtonDown");
  CDialog::OnRButtonDown(nFlags, point);
  }

void CMdlValueShow::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlValueShow::OnRButtonUp");
  CDialog::OnRButtonUp(nFlags, point);

  if (point.y>=m_ItemClientRow[0] && m_nTags>1)
    {
    int i=0;
    while (i<m_nLines&& m_ItemClientRow[i+1]<point.y)
      i++;
    if (i<m_nLines)
      {
      bool &WithZero=m_ItemChartWithZero[i];
      bool &BarOn=m_ItemChartOn[i];
      bool OldBarOn=BarOn;

      if (nFlags & (MK_SHIFT|MK_CONTROL))
        {
        BarOn=!BarOn;
        WithZero=((nFlags & MK_CONTROL)!=NULL);
        }
      else
        {
        CRect WRect;
        GetWindowRect(&WRect);
        CPoint  RBPoint;
        RBPoint.x = WRect.left+point.x;
        RBPoint.y = WRect.top+point.y;

        CMenu Menu;
        Menu.CreatePopupMenu();

        Menu.AppendMenu(MF_STRING, 100, "&Bars With Zero");
        Menu.AppendMenu(MF_STRING, 101, "&Bars Zero Supressed");
        Menu.AppendMenu(MF_STRING, 102, "&Bars Off");

        int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, RBPoint.x, RBPoint.y, this);
        Menu.DestroyMenu();                                           
        switch (RetCd)
          {
          case 100: BarOn=true;   WithZero=true;   break;
          case 101: BarOn=true;   WithZero=false;  break;
          case 102: BarOn=false;  WithZero=false;  break;
          }
        }

      if (BarOn != OldBarOn)
        {
        m_ItemChartColour[i]=byte(m_ItemChartColourIndex);
        m_ItemChartColourIndex=(m_ItemChartColourIndex+1)%nColours;
        m_ItemChartWithZero[i]=WithZero;
        }
      DoTheUpdate();
      }
    }
  }

//---------------------------------------------------------------------------

bool CMdlValueShow::AcceptNewTag(char * pTag, char * pObjClassId)
  {
  if (pObjClassId && (m_sObjClassId.XStrICmp(pObjClassId)==0))
    if (m_nTags<MdlValueShowMaxTags && m_nTags<gs_RTLCom.m_lMaxTags)
      {
      m_sTag[m_nTags++]=pTag;
      // In case wnd is hidden
      ShowWindow(SW_SHOWNOACTIVATE);
      return true;
      }
    return false;
  };

//---------------------------------------------------------------------------

void CMdlValueShow::OnNcPaint() 
  {
  CDialog::OnNcPaint();
  }

//---------------------------------------------------------------------------

void CMdlValueShow::OnClose() 
  {
  //_bMustDestroy=true;
  CDialog::OnClose();
  DestroyWindow();
  delete this;
  }

//===========================================================================
//
// CMdlGraphicShow dialog
//
//===========================================================================

//const long TWideInit=40;
//const long DWideInit=25;
//const long CWideInit=25;

CMdlGraphicShow::CMdlGraphicShow(char * pTag, /*CMdlAction & Act,*/ int X, int Y, bool Pinned, CWnd* pParent /*=NULL*/)
: CDialog()//CMdlGraphicShow::IDD, pParent)
  {
  m_nTags=1;
  m_sTag[0]=pTag;  
  m_iX=X;
  m_iY=Y;
  m_bPinned=Pinned;
  m_bMustLoadTags=true;

  //m_TWide=TWideInit;
  //m_DWide=DWideInit;
  //m_CWide=CWideInit;
  //m_TotWide=10;
  //m_TotDeep=10;
  m_bAllowMove=true;
  m_iSequence=-1;

  m_MousePt.x=-1;
  m_MouseFlags=0;

  //m_iChartItem=-1;
  //for (int i=0; i<MdlValueShowMaxItems; i++)
  //  {
  //  m_ItemChartOn[i]=false;
  //  m_ItemChartWithZero[i]=false;
  //  }
  //m_iTagSelected=0;
  //m_ItemChartColourIndex=0;

  //for (int i=0; i<MdlValueShowMaxStatusBlks; i++)
  //  {
  //  m_StateShw[i]=0;
  //  for (int j=0; j<MdlValueShowMaxTags; j++)
  //    {
  //    m_StateCh[j][i]=0;
  //    m_StateCol[j][i]=0;
  //    }
  //  }

  //{{AFX_DATA_INIT(CMdlGraphicShow)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  Create(CMdlGraphicShow::IDD, pParent);
  SetWindowText(pTag);

  CRect ClientRct;
  CPoint Point(-1,-1);
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_Create, this, ClientRct, Point, 0);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);
  }

void CMdlGraphicShow::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMdlGraphicShow)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
  }


BEGIN_MESSAGE_MAP(CMdlGraphicShow, CDialog)
  //{{AFX_MSG_MAP(CMdlGraphicShow)
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_NCPAINT()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_CLOSE()
  ON_WM_SIZE()
  ON_WM_MOVE()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_UPDATESHOWWND, OnUpdateWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMdlGraphicShow message handlers

BOOL CMdlGraphicShow::DestroyWindow() 
  {
  gs_RTLCom.m_MVCritSect.Lock();
  POSITION Pos;
  Pos=gs_RTLCom.m_GraphicShwPtrs.Find(this);
  if (Pos)
    gs_RTLCom.m_GraphicShwPtrs.RemoveAt(Pos);
  gs_RTLCom.m_MVCritSect.Unlock();
  return CDialog::DestroyWindow();
  }

BOOL CMdlGraphicShow::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  AtStart(m_sTag[0](), m_iX, m_iY, m_bPinned);
  return TRUE;
  }

void CMdlGraphicShow::AtStart(char * pTag, int X, int Y, bool Pinned)
  {
  SetWindowText(pTag);
  m_nTags=1;
  m_sTag[0]=pTag;  
  m_iX=X;
  m_iY=Y;
  m_bPinned=Pinned;

  //m_iChartItem=-1;
  //for (int i=0; i<MdlValueShowMaxItems; i++)
  //  {
  //  m_ItemChartOn[i]=false;      
  //  m_ItemChartWithZero[i]=false;
  //  }
  long Style=GetWindowLong(m_hWnd, GWL_STYLE);
  if (Pinned)
    Style|=/*WS_CAPTION|*/WS_SYSMENU;
  else
    Style=Style&~(/*WS_CAPTION|*/WS_SYSMENU);

  SetWindowLong(m_hWnd, GWL_STYLE, Style);

  //  long ExStyle=GetWindowLong(m_hWnd, GWL_EXSTYLE);
  //  Style|=WS_EX_TOOLWINDOW;
  //  SetWindowLong(m_hWnd, GWL_EXSTYLE, ExStyle);

  //m_TagStr="Collecting Data !";
  //m_DataStr[0]=" ";
  //m_CnvStr=" ";
  //m_nLines=0;
  RECT R;
  GetWindowRect(&R);
  MoveWindow(m_iX,m_iY, R.right-R.left, R.bottom-R.top);
  BringWindowToTop();

  if (!gs_Exec.Busy() || gs_Exec.Waiting())
    DoTheUpdate(); 
  }

void CMdlGraphicShow::DoTheUpdateByMsg()
  {
  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::DoTheUpdateByMsg");
  
  PostMessage(WMU_UPDATESHOWWND, 0, 0);
  }

void CMdlGraphicShow::DoTheUpdate() 
  {
  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::DoTheUpdate");
  
  if (m_nTags<1)
    return;
  SetWindowText(m_nTags>1 ? m_sObjClassId() : m_sTag[0]());

  Invalidate(0); //  RedrawWindow(0,0,RDW_INVALIDATE|RDW_UPDATENOW);
  //m_TagStr="";
  //m_DataStr[0]="";
  //m_CnvStr="";
  //m_nLines=0;

  //CXM_ObjectData ObjData;
  //CXM_Route      Route;

  //flag ItemUsed[MdlValueShowMaxItems];
  //Strng ItemTag[MdlValueShowMaxItems];
  //Strng ItemCnv[MdlValueShowMaxItems];
  //for (int iT=0; iT<MdlValueShowMaxItems; iT++)
  //  ItemUsed[iT]=0;

  ////m_nStates=0;
  //for (int i=0;i<MdlValueShowMaxStatusBlks; i++)
  //  {
  //  m_StateShw[i]=0;
  //  for (int iTag=0; iTag<m_nTags; iTag++)
  //    m_StateCh[iTag][i]='\0';
  //  }
  //for (int iTag=0; iTag<m_nTags; iTag++)
  //  {
  //  m_DataStr[iTag]="";
  //  
  //  Strng_List sStrList;

  //  Strng T;
  //  T.Set("%s.ClassId", m_sTag[iTag]);

  //  CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);

  //  if (m_sTag[iTag].Length()==0)
  //    continue;
  //  
  //  if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
  //    {
  //    m_sObjClassId="";
  //    CPkDataItem * pItem=ObjData.FirstItem();
  //    if (IsStrng(pItem->Type()))
  //      {
  //      m_sObjClassId = pItem->Value()->GetString("", "", 0, "");
  //      StripClassIdVersion(m_sObjClassId);        
  //      }

  //    int nTagItems=0;
  //    if (m_bMustLoadTags)
  //      {
  //      m_bMustLoadTags=false;
  //      if (m_sObjClassId())
  //        {
  //        gs_RTLCom.LoadVarTags(m_sObjClassId(), NULL, m_TagItems, nTagItems);
  //        if (nTagItems==0)
  //          {
  //          gs_RTLCom.m_InBaseCfg =true;      
  //          gs_RTLCom.LoadVarTags(m_sObjClassId(), NULL, m_TagItems, nTagItems);
  //          gs_RTLCom.m_InBaseCfg =false;      
  //          }
  //        //CRptTagLists::Show(m_sObjClassId(), false);
  //        }

  //      if (nTagItems==0)
  //        {
  //        Defaults *Defs = &StdDefs[0];
  //        /*#if WithQAL
  //        if (QALOptions())
  //          Defs=&QALDefs[0];
  //        #endif*/
  //    
  //        for (int iT=0; Defs[iT].Tag; iT++)
  //          {
  //          if (iT>=MdlValueShowMaxItems)
  //            {
  //            LogWarning("View", 0, "Too many show Items");
  //            break;
  //            }
  //          m_TagItems.SetSize(Max(iT+1, m_TagItems.GetSize()),MdlValueShowMaxItems);
  //          m_TagItems[iT].m_sLabel=Defs[iT].Lbl;
  //          m_TagItems[iT].m_sTag=Defs[iT].Tag;
  //          m_TagItems[iT].m_Fmt.Set("XX", 0, 2, 'f');
  //          m_TagItems[iT].m_Cnv.Set(Defs[iT].CnvTyp, Defs[iT].CnvTxt);
  //          }
  //        }
  //      else
  //        {
  //        if (nTagItems>=MdlValueShowMaxItems)
  //          {
  //          LogWarning("View", 0, "Too many show Items");
  //          m_TagItems.SetSize(MdlValueShowMaxItems,MdlValueShowMaxItems);
  //          }
  //        }
  //      }
  //    nTagItems=m_TagItems.GetSize();

  //    if (1)
  //      {
  //      T.Set("%s.Condition", m_sTag[iTag]());
  //      CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
  //      CXM_ObjectData ObjData;
  //      CXM_Route      Route;
  //      if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
  //        {
  //        CPkDataItem * pItem=ObjData.FirstItem();
  //        m_CondStr[iTag]="";
  //        if (IsStrng(pItem->Type()))
  //          m_CondStr[iTag]= pItem->Value()->GetString();
  //        else
  //          LogError("View", 0, "Tag '%s' not allowed.", T());
  //        }
  //      }

  //    for (int iT=0; iT<nTagItems; iT++)
  //      {
  //      T.Set("%s.%s", m_sTag[iTag](), m_TagItems[iT].m_sTag());
  //      CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
  //      CXM_ObjectData ObjData;
  //      CXM_Route      Route;

  //      if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
  //        {
  //        CPkDataItem * pItem=ObjData.FirstItem();
  //        int cType=pItem->Type();
  //        Strng &Lbl=m_TagItems[iT].m_sLabel;
  //        Strng &Tag=m_TagItems[iT].m_sTag;
  //        CnvAttribute &Cnv=m_TagItems[iT].m_Cnv;
  //        FmtAttribute &Fmt=m_TagItems[iT].m_Fmt;

  //        Cnv.SetIndex(pItem->CnvIndex());
  //        if ((Cnv.TextLength()==0) || (Cnvs[Cnv.Index()]->Find(Cnv.Text())==NULL))
  //          Cnv.SetText(pItem->CnvTxt());

  //        Strng UseCnvTxt;//(Cnv.Text);
  //        CDataCnv & C=*Cnvs[Cnv.Index()];
  //        if (C.OverTxt())
  //          UseCnvTxt=C.OverTxt();
  //        else if (Cnv.TextLength()>0)
  //          UseCnvTxt=Cnv.Text();
  //        else
  //          UseCnvTxt=" ";

  //        if (!ItemUsed[iT])
  //          {
  //          ItemUsed[iT]=1;
  //          ItemTag[iT]=Lbl();
  //          ItemCnv[iT]=UseCnvTxt();
  //          }

  //        sStrList.Clear();
  //        if (pItem->Contains(PDI_StrList))
  //          pItem->GetStrList(sStrList);

  //        Strng sValue("???");
  //        if (IsNumData(cType))
  //          {
  //          if (IsFloatData(cType))
  //            {
  //            double dValue=pItem->Value()->GetDouble(Cnv.Index(), UseCnvTxt());
  //            Fmt.FormatFloat(dValue, sValue);
  //            }
  //          else if (sStrList.Length()>0)
  //            {
  //            long i=pItem->Value()->GetLong(Cnv.Index(), UseCnvTxt());
  //            pStrng p=sStrList.AtIndexVal(i);
  //            if (p)
  //              sValue.Set("%i-%s", i, p->Str());
  //            else
  //              sValue.Set("%i", i);
  //            }
  //          else
  //            sValue = pItem->Value()->GetString("%i", "%g", Cnv.Index(), UseCnvTxt());
  //          }
  //        else if (IsData(cType))
  //          {
  //          sValue = pItem->Value()->GetString("%i", "%g", Cnv.Index(), UseCnvTxt());
  //          }
  //        else
  //          LogError("View", 0, "Tag '%s' not allowed.", T());
  //      
  //        if (!sValue.Length())
  //          sValue=" ";
  //        
  //        if (iT)
  //          m_DataStr[iTag]+="\r";
  //        
  //        if (sValue.Length()<gs_RTLCom.m_lMaxValueLen)
  //          m_DataStr[iTag]+=sValue();
  //        else
  //          {
  //          Strng S=sValue();
  //          S.SetLength(gs_RTLCom.m_lMaxValueLen-3);
  //          S+="...";
  //          m_DataStr[iTag]+=S();
  //          }
  //        #if dbgBuild     
  //        dbgpln("%s = %s", T(),sValue());
  //        #endif     
  //        }
  //      else
  //        {
  //        #if dbgBuild     
  //        dbgpln("%s - MISSING", T());
  //        #endif     
  //        if (iT)
  //          m_DataStr[iTag]+="\r";
  //        }
  //      }

  //    if (gs_pTheSFELib && m_sTag[iTag]())
  //      {
  //      dword Status=gs_pTheSFELib->FE_GetModelStatus(m_sTag[iTag]());
  //      if ((Status&VisStates)!=0)
  //        {
  //        int iState=0;
  //        if (Status & FNS_InActive)
  //          {
  //          BLDSTATE(iTag, iState, 'i', GR_MDLSTATUS_INACTIVE)
  //          iState++;
  //          }
  //        else
  //          {
  //          //if (Status & FNS_Marked)
  //          //  BLDSTATE(iTag, iState, '*', GR_HIGHLIGHT) 
  //          //iState++;

  //          if (Status & FNS_Error)
  //            BLDSTATE(iTag, iState, 'e', GR_MDLSTATUS_ERROR) 
  //          iState++;

  //          if (Status & FNS_Warning)
  //            BLDSTATE(iTag, iState, 'w', GR_MDLSTATUS_WARNING)
  //          iState++;

  //          if (Status & FNS_Tear)
  //            BLDSTATE(iTag, iState, 't', GR_MDLSTATUS_TEAR)
  //
  //          iState++;
  //          if (Status & FNS_MacMdl)
  //            BLDSTATE(iTag, iState, 'm', GR_MDLSTATUS_MACMDL)
  //          iState++;

  //          if (Status & FNS_On)
  //            BLDSTATE(iTag, iState, '!', GR_MDLSTATUS_ON)
  //          else if (Status & FNS_Off)
  //            BLDSTATE(iTag, iState, '.', GR_MDLSTATUS_OFF)
  //          iState++;

  //          if (Status & (FNS_IsSrc|FNS_NettQmP))
  //            {
  //            if (Status & FNS_IsSrc)
  //              BLDSTATE(iTag, iState, '+', GR_MDLSTATUS_ISSRC)
  //            else
  //              BLDSTATE(iTag, iState, '+', GR_MDLSTATUS_NETTQMP)
  //            }
  //          else if (Status & (FNS_IsSnk|FNS_NettQmN))
  //            {
  //            if (Status & FNS_IsSnk)
  //              BLDSTATE(iTag, iState, '-', GR_MDLSTATUS_ISSNK)
  //            else
  //              BLDSTATE(iTag, iState, '-', GR_MDLSTATUS_NETTQMN)
  //            }
  //          iState++;

  //          if (Status & (FNS_UFlw|FNS_LFlwP|FNS_LFlwN))
  //            {
  //            if (Status & FNS_UFlw)
  //              BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_UFLW)
  //            else if (Status & FNS_LFlwP)
  //              BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_LFLWP)
  //            else
  //              BLDSTATE(iTag, iState, 'f', GR_MDLSTATUS_LFLWN)
  //            }
  //          else if (Status & (FNS_UNoFlw|FNS_LNoFlw))
  //            {
  //            if (Status & FNS_UNoFlw)
  //              BLDSTATE(iTag, iState, '0', GR_MDLSTATUS_UNOFLW)
  //            else
  //              BLDSTATE(iTag, iState, '0', GR_MDLSTATUS_LNOFLW)
  //            }
  //          iState++;

  //          if (Status & FNS_Hold)
  //            BLDSTATE(iTag, iState, 'h', GR_MDLSTATUS_INACTIVE)
  //          iState++;
  //          }
  //        };
  //      }
  //    }
  //  }

  //long nRemoved=0;
  //for (iT=0; iT<MdlValueShowMaxItems; iT++)
  //  if (!ItemUsed[iT])
  //    {
  //    // Remove Extra Blank Fields
  //    for (int iTag=0; iTag<m_nTags; iTag++)
  //      if (m_DataStr[iTag].Length()>0)
  //        {
  //        char Buff[4096];
  //        strcpy(Buff, m_DataStr[iTag]());
  //        char * p=Buff;
  //        int i=0;
  //        while (p && i++<iT-nRemoved)
  //          {
  //          if (p=strchr(p, '\r'))
  //            p=p+1;
  //          }
  //        if (p  &&strlen(p))
  //          {
  //          memmove(p, p+1, strlen(p)+1);
  //          // Done Restore
  //          m_DataStr[iTag]=Buff;
  //          }
  //        }
  //    nRemoved++;
  //    }
  //  else
  //    {
  //    if (m_nLines)
  //      {
  //      m_TagStr+="\r";
  //      m_CnvStr+="\r";
  //      }
  //    m_TagStr+=ItemTag[iT]();
  //    m_CnvStr+=ItemCnv[iT]();
  //    m_nLines++;
  //    }

  //while (m_nLines<MinShownLines)
  //  {
  //  if (m_nLines)
  //    {
  //    m_TagStr+="\r";
  //    m_CnvStr+="\r";              
  //    for (int iTag=0; iTag<m_nTags; iTag++);
  //      m_DataStr[iTag]+="\r";
  //    }
  //  m_TagStr+=" ";
  //  m_CnvStr+=" ";
  //  for (int iTag=0; iTag<m_nTags; iTag++);
  //    m_DataStr[iTag]+=" ";
  //  m_nLines++;
  //  }

  m_bAllowMove=true;

  Invalidate();
  }


BOOL CMdlGraphicShow::OnEraseBkgnd(CDC * pCDC)
  {
  CRect ClientRct;
  GetClientRect(&ClientRct);

  CMdlGraphicWnd MGW(MGT_EraseBkgnd, this, ClientRct, pCDC);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);
  return MGW.m_bReturn;
  };

void CMdlGraphicShow::OnPaint() 
  {
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  CRect ClientRct;
  GetClientRect(&ClientRct);

  CFont * OldFont=dc.SelectObject(GetFont()); // Italic
  int OldBkMode=dc.SetBkMode(TRANSPARENT);
  CPen * OldPen=dc.GetCurrentPen();

  //GetClientRect(&ClientRct);
  CRect RctX;
  RctX.SetRect(0,0,1,1);

  CRect RctT=RctX;
  CRect RctC=RctX;
  CRect RctCI=RctX;
  CRect RctD[MdlValueShowMaxTags];
  for (int iTag=0; iTag<m_nTags; iTag++)
    RctD[iTag]=RctX;

  dc.DrawText("X",  1,  RctX, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 

  CMdlGraphicWnd MGW(MGT_Paint, this, ClientRct, &dc, &RctX);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);

  long NBlks=0;
  //for (int i=0;i<MdlValueShowMaxStatusBlks; i++)
  //  if(m_StateShw[i])
  //    NBlks++;
  //ASSERT_ALWAYS(NBlks<=MdlValueShowMaxStatusBlks, "Too Many Status Blocks", __FILE__, __LINE__);

  long BlkW=RctX.Width();
  long BlkH=RctX.Height();
  long BlkWTot=BlkW*NBlks;

  //  if (m_TagStr.Length())
  //    dc.DrawText(m_TagStr(),  m_TagStr.Length(),  RctT, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //  if (m_CnvStr.Length())
  //    dc.DrawText(m_CnvStr(),  m_CnvStr.Length(),  RctC, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //  for (iTag=0; iTag<m_nTags; iTag++)
  //    {
  //    if (m_DataStr[iTag].Length())
  //      dc.DrawText(m_DataStr[iTag](), m_DataStr[iTag].Length(), RctD[iTag], DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_RIGHT); 
  //    }
  //
  //  //flag DoCond=(m_CondStr.Length()>0);
  //
  ////  dword StatusA[MdlValueShowMaxTags];
  ////  for (iTag=0; iTag<m_nTags; iTag++)
  ////    StatusA[iTag]=0;
  //
  //  flag DoStatus=true;
  //
  //  m_iTagSelected=Range(0L, m_iTagSelected, m_nTags);
  //
  //  int nStatus=1;
  //  int nConds=1;
  //  char * p=m_CondStr[m_iTagSelected]();
  //  if (p)
  //    {
  //    while ((p=strchr(p, '\n'))!=NULL)
  //      {
  //      p++;
  //      nConds++;
  //      }
  //    dc.DrawText(m_CondStr[m_iTagSelected](),  m_CondStr[m_iTagSelected].Length(),  RctCI, DT_CALCRECT|DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //    }
  //
  //  long XW=RctX.right;
  //  long XW2=RctX.right/2;
  //
  //  m_TWide=Max(m_TWide, RctT.right+XW2);
  //  m_CWide=Max(m_CWide, RctC.right+XW2);
  //  m_DWide=Max(m_DWide, BlkWTot/*+3*XW*/);
  //  for (iTag=0; iTag<m_nTags; iTag++)
  //    m_DWide=Max(m_DWide, RctD[iTag].right/*+XW*/);
  //  m_TotWide=Max(m_TWide+XW+m_nTags*(m_DWide+XW)+m_CWide+XW2, (long)RctCI.Width());
  //  
  //
  //  flag DoChart=0;
  //  if (m_nTags>1)
  //    {
  //    DoChart=m_iChartItem>=0;
  //    for (int i=0; i<m_nLines; i++)
  //      DoChart = DoChart || m_ItemChartOn[i];
  //    }
  //
  //  long TagLine=(m_nTags>1)? 1:0;
  //  long LineHgt=RctX.Height();
  //  long CondDeep=nConds*LineHgt;
  //  long ChartRows=4;
  //  long ChartDeep= DoChart ? ChartRows*LineHgt : 0;
  //  long StatusDeep=1*LineHgt;
  //  long HeadDeep= TagLine*LineHgt;
  //    
  //  m_TotDeep=RctT.bottom+HeadDeep+CondDeep+StatusDeep+ChartDeep;
  //
  //  CRect DTRct;
  //  AfxGetMainWnd()->GetWindowRect(&DTRct);
  //  CRect WRct;
  //  GetWindowRect(&WRct);
  //  long ICx=GetSystemMetrics(SM_CXSMICON);
  //  long ICy=GetSystemMetrics(SM_CYSMICON);
  //  long ICyC=GetSystemMetrics(SM_CYSMCAPTION);
  //
  //  bool DoMove=false;
  //  if (m_bAllowMove)
  //    {
  //    if (m_TotWide>ClientRct.right || m_TotDeep>ClientRct.bottom || m_TotDeep<ClientRct.bottom)
  //      {
  //      WRct.right+=m_TotWide-ClientRct.right;
  //      WRct.bottom+=m_TotDeep-ClientRct.bottom;
  //      DoMove=true;
  //      }
  //
  //    int XErrL=Min(0L, WRct.right-(DTRct.left+m_TotWide));
  //    int XErrR=Max(0L, WRct.left-(DTRct.right-m_TotWide));
  //    int YErrT=Min(0L, WRct.top-DTRct.top);
  //    int YErrB=Max(0L, WRct.top/*+ICyC*/-(DTRct.bottom-(m_TotDeep+ICyC)));
  //    
  //    if (XErrL || XErrR || YErrT || YErrB)
  //      {
  //      WRct.OffsetRect(-(XErrL+XErrR), -(YErrT+YErrB));
  //      DoMove=true;
  //      }
  //    }
  //
  //  if (DoMove)
  //    {
  //    m_bAllowMove=false;
  //    Invalidate();
  //    MoveWindow(WRct);
  //    }
  //  else
  //    {
  //    CPen Pen1(BS_SOLID,1,RGB(0x80, 0x80, 0x80));
  //    CPen Pen2(BS_SOLID,2,RGB(0x80, 0x80, 0x80));
  //
  //    CPen*pPenMem=dc.SelectObject(&Pen1);
  //    int iItem=nStatus>0 ? -1:0;
  //    for (int i=TagLine+nConds; i<=m_nLines+TagLine+nConds+nStatus; i++)
  //      {
  //      int y=i*LineHgt+1;
  //      POINT Pts[2];
  //      Pts[0].x=0;
  //      Pts[0].y=y;
  //      Pts[1].x=ClientRct.right;
  //      Pts[1].y=y;
  //      dc.Polyline(Pts, 2);
  //
  //      if (iItem>=0)
  //        m_ItemClientRow[iItem]=y;
  //      iItem++;
  //      }
  //    if (iItem)
  //      m_ItemClientRow[iItem]=m_ItemClientRow[iItem-1]+LineHgt;
  //    int StatusStart=HeadDeep+1+CondDeep;
  //    int ItemStart=StatusStart+LineHgt;
  //    int ItemEnd=ItemStart+m_nLines*LineHgt;
  //
  //    if (m_nTags<2)
  //      {
  //      POINT Pts[2];
  //      Pts[0].x=m_TWide+XW2;
  //      Pts[0].y=StatusStart;
  //      Pts[1].x=Pts[0].x;
  //      Pts[1].y=ClientRct.bottom;
  //      dc.Polyline(Pts, 2);
  //      Pts[0].x+=m_DWide+XW;
  //      Pts[1].x=Pts[0].x;
  //      dc.Polyline(Pts, 2);
  //      }
  //    else
  //      {
  //      POINT Pts[5];
  //      Pts[0].x=m_TWide+XW2;
  //      Pts[0].y=StatusStart;
  //      Pts[1].x=Pts[0].x;
  //      Pts[1].y=ItemEnd;
  //      dc.Polyline(Pts, 2);
  //      Pts[0].x+=m_CWide+XW;
  //      Pts[1].x=Pts[0].x;
  //      Pts[1].y=ClientRct.bottom;
  //      dc.Polyline(Pts, 2);
  //      m_TagClientCol[0]=Pts[0].x;
  //      for (iTag=0; iTag<m_nTags; iTag++)
  //        {
  //        Pts[0].x+=m_DWide+XW;
  //        Pts[1].x=Pts[0].x;
  //        dc.Polyline(Pts, 2);
  //        m_TagClientCol[iTag+1]=Pts[0].x;
  //        }
  //      // Hilight Selected Tag
  //      if (1)
  //        {
  //        CPen*pPenMem=dc.SelectObject(&Pen2);
  //
  //        CRect SlctRct;
  //        SlctRct.top=StatusStart;
  //        SlctRct.bottom=ClientRct.bottom+1;
  //        SlctRct.left=m_TWide+XW2+m_CWide+XW+(m_iTagSelected)*(m_DWide+XW);
  //        SlctRct.right=SlctRct.left+(m_DWide+XW)+1;
  //
  //        Pts[0]=SlctRct.TopLeft();
  //        Pts[1]=SlctRct.TopLeft();
  //        Pts[2]=SlctRct.BottomRight();
  //        Pts[3]=SlctRct.BottomRight();
  //        Pts[4]=Pts[0];
  //      
  //        Pts[1].x=Pts[2].x;
  //        Pts[3].x=Pts[0].x;
  //        dc.Polyline(Pts, 5);
  //        dc.SelectObject(pPenMem);
  //        }
  //      }
  //    dc.SelectObject(pPenMem);
  //
  //    CRect Rct=ClientRct;
  //
  //    if (1)
  //      {
  //      Rct=ClientRct;
  //      Rct.top+=HeadDeep+1;
  //      Rct.bottom+=HeadDeep+1;
  //      dc.DrawText(m_CondStr[m_iTagSelected](), m_CondStr[m_iTagSelected].Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //      }
  //
  //    if (m_nTags>1)
  //      {
  //      Strng sTag("Tag:");
  //      sTag+=m_sTag[m_iTagSelected];
  //
  //      Rct=ClientRct;
  //      dc.DrawText(sTag(), sTag.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //
  //      if (DoChart)
  //        {
  //        for (int iT=0; iT<m_nLines; iT++)
  //          {
  //          if (m_ItemChartOn[iT] || (iT==m_iChartItem))
  //            {
  //            Rct=ClientRct;
  //            Rct.OffsetRect(Rct.left+m_TWide, ItemStart+iT*LineHgt);
  //            Rct.right=Rct.left+(XW*2)/3;
  //            Rct.bottom=Rct.top+LineHgt;
  //
  //            byte ColInd=m_ItemChartOn[iT] ? m_ItemChartColour[iT] : 0;
  //            if (m_ItemChartWithZero[iT])
  //              {
  //              int x=Rct.Width()/2;
  //              int y=Rct.Height()/2;
  //              dc.FillSolidRect(Rct.left, Rct.top, x, y, grl_get_win_color(Colours[ColInd]));
  //              dc.FillSolidRect(Rct.left+x, Rct.top+y, Rct.Width()-x, Rct.Height()-y, grl_get_win_color(Colours[ColInd]));
  //              }
  //            else
  //              dc.FillSolidRect(Rct.left, Rct.top, Rct.Width(), Rct.Height(), grl_get_win_color(Colours[ColInd]));
  //            }
  //          }
  //        }
  //
  //      if (DoStatus)
  //        {
  //        Strng sStatus("Status");
  //        Rct=ClientRct;
  //        Rct.OffsetRect(0, StatusStart);
  //        Rct.right=Rct.left+m_TWide;
  //        dc.DrawText(sStatus(), sStatus.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //        }
  //
  //      Rct=ClientRct;
  //      Rct.OffsetRect(0, ItemStart);
  //      Rct.right=Rct.left+m_TWide;
  //      dc.DrawText(m_TagStr(), m_TagStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //      Rct.OffsetRect(m_TWide+XW, 0);
  //
  //      Rct.right=Rct.left+m_CWide;
  //      dc.DrawText(m_CnvStr(), m_CnvStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //      Rct.OffsetRect(m_CWide+XW, 0);
  //
  //      long iPos=0;
  //      for (iTag=0; iTag<m_nTags; iTag++)
  //        {
  //        Rct.right=Rct.left+m_DWide;
  //        dc.DrawText(m_DataStr[iTag](), m_DataStr[iTag].Length(), Rct, DT_TOP|DT_NOPREFIX|DT_RIGHT); 
  //        Rct.OffsetRect(m_DWide+XW, 0);
  //        }
  //      }
  //    else
  //      {
  //      if (DoStatus)
  //        {
  //        Strng sStatus("Status");
  //        Rct=ClientRct;
  //        Rct.OffsetRect(0, StatusStart);
  //        Rct.right=Rct.left+m_TWide;
  //        dc.DrawText(sStatus(), sStatus.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //        }
  //      Rct=ClientRct;
  //      Rct.OffsetRect(0, ItemStart);
  //      Rct.right=Rct.left+m_TWide;
  //      dc.DrawText(m_TagStr(), m_TagStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //      Rct.OffsetRect(m_TWide+XW, 0);
  //
  //      Rct.right=Rct.left+m_DWide;
  //      dc.DrawText(m_DataStr[0](), m_DataStr[0].Length(), Rct, DT_TOP|DT_NOPREFIX|DT_RIGHT); 
  //      Rct.OffsetRect(m_DWide+XW, 0);
  //
  //      Rct.right=Rct.left+m_CWide;
  //      dc.DrawText(m_CnvStr(), m_CnvStr.Length(), Rct, DT_TOP|DT_NOPREFIX|DT_LEFT); 
  //      }
  //
  //    if (DoStatus)
  //      {
  //      for (iTag=0; iTag<m_nTags; iTag++)
  //        {
  //        long XOff=m_TWide+XW2+(iTag+1)*(m_DWide+XW)-BlkWTot;
  //        if (m_nTags>1)
  //          XOff+=m_CWide+XW;
  //
  //        Rct=ClientRct;
  //        Rct.OffsetRect(XOff+(NBlks-1)*BlkW, StatusStart+1);
  //        Rct.right=Rct.left+BlkW;
  //        Rct.bottom=Rct.top+BlkH;
  //        //dword Status=StatusA[iTag];
  //        long StateW=BlkW;
  //        long StateH=BlkH-1;
  //
  //        for (int i=MdlValueShowMaxStatusBlks-1; i>=0; i--)
  //          {
  //          if(m_StateShw[i] && m_StateCh[iTag][i])
  //            {
  //            dc.FillSolidRect(Rct.left,Rct.top,StateW,StateH, grl_get_win_color(m_StateCol[iTag][i]));
  //            dc.DrawText(&m_StateCh[iTag][i], 1, Rct, DT_VCENTER|DT_NOPREFIX|DT_CENTER); 
  //            Rct.OffsetRect(-StateW,0);
  //            }
  //          }
  //        }
  //      }
  //    if (DoChart)
  //      {
  //      int nCharts=0;
  //      for (int iT=0; iT<m_nLines; iT++)
  //        {
  //        if (m_ItemChartOn[iT] || (iT==m_iChartItem))
  //          nCharts++;
  //        }
  //      long BarGap=2;
  //      long BarWide=m_DWide/Max(nCharts,4)-BarGap;
  //      long iPos=0;
  //
  //      for (iT=0; iT<m_nLines; iT++)
  //        {
  //        if (m_ItemChartOn[iT] || (iT==m_iChartItem))
  //          {
  //          double Vals[MdlValueShowMaxTags];
  //          // Extract Vals
  //          double MaxV=-1.0e30;
  //          double MinV=+1.0e30;
  //          for (int iTag=0; iTag<m_nTags; iTag++)
  //            {
  //            char * p=m_DataStr[iTag]();
  //            int i=0;
  //            while (p && i++<iT)
  //              {
  //              if (p=strchr(p, '\r'))
  //                p=p+1;
  //              }
  //            Vals[iTag]=(p) ? SafeAtoF(p) : 0;
  //            MaxV=Max(MaxV, Vals[iTag]);
  //            MinV=Min(MinV, Vals[iTag]);
  //            }
  //          if (m_ItemChartWithZero[iT])
  //            {
  //            MaxV=Max(MaxV, 0.0);
  //            MinV=Min(MinV, 0.0);
  //            }
  //          Rct=ClientRct;
  //          Rct.OffsetRect(m_TWide+XW+m_CWide+XW+iPos*(BarWide+BarGap)-BarGap, ItemStart+2+m_nLines*LineHgt);
  //          Rct.right=Rct.left+BarWide;
  //          Rct.bottom=ClientRct.bottom;
  //          for (iTag=0; iTag<m_nTags; iTag++)
  //            {
  //            byte ColInd=m_ItemChartOn[iT] ? m_ItemChartColour[iT] : 0;
  //            if (0)
  //              {
  //              long Drop=long((Rct.Height()-1)*(MaxV-Vals[iTag])/GTZ(MaxV-MinV));
  //              dc.FillSolidRect(Rct.left,Rct.top+Drop,Rct.Width(), Rct.Height()-Drop, grl_get_win_color(Colours[ColInd]));
  //              }
  //            else
  //              {
  //              double R=GTZ(MaxV-MinV);
  //              long H=(Rct.Height()-2);
  //              long T,B;
  //              if (Vals[iTag]>0.0)
  //                {
  //                T=long(H*(Vals[iTag]-MinV)/R);
  //                B=long(H*(Max(0.0, MinV)-MinV)/R);
  //                }
  //              else
  //                {
  //                T=long(H*(Min(0.0, MaxV)-MinV)/R);
  //                B=long(H*(Vals[iTag]-MinV)/R);
  //                }
  //              T=1+H-T;
  //              B=1+H-B;
  //              dc.FillSolidRect(Rct.left,Rct.top+T,Rct.Width(), B-T, grl_get_win_color(Colours[ColInd]));
  //              }
  //            Rct.OffsetRect(m_DWide+XW, 0);
  //            }
  //          iPos++;
  //          }
  //        }
  //      }
  //    }

  dc.SetBkMode(OldBkMode);
  dc.SelectObject(OldFont);
  dc.SelectObject(OldPen);

  // Do not call CDialog::OnPaint() for painting messages
  }

void CMdlGraphicShow::OnSize(UINT nType, int cx, int cy)
  {
  CPoint Point(-1,-1);
  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_Size, this, ClientRct, Point, 0);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);
  };

void CMdlGraphicShow::OnMove(int x, int y)
  {
  CPoint Point(-1,-1);
  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_Move, this, ClientRct, Point, 0);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);
  };

void CMdlGraphicShow::TryAdd(char * pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent)
  {
  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::TryAdd");
  
  if (1)//gs_Exec.Busy()) //thread problem ---
    {
    CAddRequestPtr Add = new CRptTagsAddRqst;

    Add->m_Tag=pTag;
    Add->m_X=X;
    Add->m_Y=Y;
    Add->m_Pinned=Pinned;
    Add->m_Append=Append;
    Add->m_pParent=pParent;

    gs_RTLCom.m_MVCritSect.Lock();	
    gs_RTLCom.m_GraphicAddPtrs.AddTail(Add);
    gs_RTLCom.m_MVCritSect.Unlock();
    if (!gs_Exec.Busy())
      UpdateAll();
    }
  else
    {
    Add(pTag, X, Y, Pinned, Append, pParent);
    }
  }

void CMdlGraphicShow::Add(char * pTag, int X, int Y, bool Pinned, bool Append, CWnd* pParent)
  {
  Strng T, ObjClassId;
  T.Set("%s.ClassId", pTag);

  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::Add - Start");

  CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route      Route;

  if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
    {
    CPkDataItem * pItem=ObjData.FirstItem();
    if (IsStrng(pItem->Type()))
      {
      ObjClassId = pItem->Value()->GetString("", "", 0, "");
      StripClassIdVersion(ObjClassId);
      }
    }
  else
    LogError((char*)pTag, 0, "ClassId not found");

  gs_RTLCom.m_MVCritSect.Lock();	
  if (Append)
    {
    POSITION Pos=gs_RTLCom.m_GraphicShwPtrs.GetHeadPosition();
    while (Pos)
      {
      GraphicShwPtr P=gs_RTLCom.m_GraphicShwPtrs.GetNext(Pos);
      if (P->AcceptNewTag(pTag, ObjClassId()))
        {
        P->DoTheUpdate();
        gs_RTLCom.m_MVCritSect.Unlock();	
        return;
        }
      }
    }

  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::Add - AddIt");
  
  GraphicShwPtr P=new CMdlGraphicShow(pTag, X, Y, Pinned, pParent);
  P->m_iSequence=gs_RTLCom.m_iSequence;
  gs_RTLCom.m_GraphicShwPtrs.AddHead(P);
  //  P->AtStart(pTag, X, Y, Pinned);
  P->ShowWindow(SW_SHOWNOACTIVATE);
  gs_RTLCom.m_MVCritSect.Unlock();	
  };

void CMdlGraphicShow::TryClear(bool All)
  {
  gs_RTLCom.m_MVCritSect.Lock();	
  
  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::TryClear");
  
  gs_RTLCom.m_iClear=All?2:1;
  gs_RTLCom.m_MVCritSect.Unlock();
  if (!gs_Exec.Busy())
    UpdateAll();
  }

bool CMdlGraphicShow::Clear(bool All)
  {
  bool Ret=true;
  gs_RTLCom.m_MVCritSect.Lock();	

  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::Clear");
  
  if (All)
    {
    while (!gs_RTLCom.m_GraphicShwPtrs.IsEmpty())
      {
      GraphicShwPtr P=gs_RTLCom.m_GraphicShwPtrs.GetHead();
      P->DestroyWindow();
      delete P;
      }
    while (!gs_RTLCom.m_GraphicAddPtrs.IsEmpty())
      {
      CAddRequestPtr P=gs_RTLCom.m_GraphicAddPtrs.GetHead();
      delete P;
      }
    }
  else
    {
    POSITION Pos=gs_RTLCom.m_GraphicShwPtrs.GetHeadPosition();
    while (Pos)
      {
      POSITION DelP=Pos;
      GraphicShwPtr P=gs_RTLCom.m_GraphicShwPtrs.GetNext(Pos);
      if (!P->m_bPinned)
        {
        if (P->m_iSequence==gs_RTLCom.m_iSequence)
          Ret=false;
        else
          {
          P->ShowWindow(SW_HIDE);
          P->DestroyWindow();
          delete P;
          }
        }
      }
    }
  gs_RTLCom.m_MVCritSect.Unlock();
  return Ret;
  }

void CMdlGraphicShow::UpdateAll()
  {
  gs_RTLCom.m_MVCritSect.Lock();
  
  if (dbgGraphicShow)
    dbgpln("CMdlGraphicShow::UpdateAll");

  while (!gs_RTLCom.m_GraphicAddPtrs.IsEmpty())
    {
    CAddRequestPtr A=gs_RTLCom.m_GraphicAddPtrs.RemoveHead();
    if (gs_RTLCom.m_iClear==0)
      Add(A->m_Tag(), A->m_X, A->m_Y, A->m_Pinned, A->m_Append, A->m_pParent);
    
    if (dbgGraphicShow)
      dbgpln("                             ADD   >>>>>>> %s", A->m_Tag());
    //    P->DoTheAddByMsg(A);
    delete A;
    }

  POSITION Pos=gs_RTLCom.m_GraphicShwPtrs.GetHeadPosition();
  while (Pos)
    {
    GraphicShwPtr P=gs_RTLCom.m_GraphicShwPtrs.GetNext(Pos);
    P->DoTheUpdateByMsg();
    }
  if (gs_RTLCom.m_iClear)
    {
    if (dbgGraphicShow)
      dbgpln("                             Clear >>>>>>> %s",gs_RTLCom.m_iClear==2?"All":"");
    
    if (Clear(gs_RTLCom.m_iClear==2))
      gs_RTLCom.m_iClear=0;
    }
  gs_RTLCom.m_iSequence++;
  if (gs_RTLCom.m_iSequence>1000)
    gs_RTLCom.m_iSequence=0;
  gs_RTLCom.m_MVCritSect.Unlock();	
  }

LRESULT CMdlGraphicShow::OnUpdateWnd(WPARAM wParam, LPARAM lParam)
  {
  DoTheUpdate();
  return 0;
  }

void CMdlGraphicShow::DeleteItem(char * pObjClassId, int Index)
  {
  if (Index<0)
    return;
  Strng Sect, Item;
  Sect.Set("%s.View.Vars", pObjClassId);
  CProfINIFile PF(gs_RTLCom.RTLFileName());
  Item.Set("V%i",Index);
  PF.WrStr(Sect(), Item(), "");
  };

void CMdlGraphicShow::AddItem(TagInfoBlk & IB)
  {
  CRptTagLists::AddItem(IB);
  }

flag CMdlGraphicShow::FindItem(char * pObjClassId, int Index, CVarTagItem & TagItem)
  {
  Strng ObjClassId(pObjClassId);
  StripClassIdVersion(ObjClassId);
  Strng Sect, Item;
  Sect.Set("%s.View.Vars", ObjClassId());
  CProfINIFile PF(gs_RTLCom.RTLFileName());
  Item.Set("V%i",Index);
  Strng Val(PF.RdStr(Sect(), Item(), ""));//, Val()));

  char * pData=Val();
  if(pData)
    {
    char* pFld[10];
    for (int nFlds=0; pData; nFlds++)
      {
      if (nFlds)
        {
        *pData=0; // Terminate Fld
        pData++; // Point @ Data
        }
      pFld[nFlds]=pData;
      pData=strchr(pData, ',');
      }
    for (int i=0; i<nFlds; i++)
      XStrTrim(pFld[i]);

    TagItem.m_Fmt.SetType('-');

    TagItem.m_sLabel=nFlds>0 && pFld[0] ? pFld[0] : "";
    TagItem.m_sTag=nFlds>1 && pFld[1] ? pFld[1] : "";
    TagItem.m_Cnv.SetIndex(nFlds>2 && pFld[2] ? pFld[2] : "");
    TagItem.m_Cnv.SetText(nFlds>3 && pFld[3] ? pFld[3] : "");
    TagItem.m_Fmt.ParseStr(nFlds>4 && pFld[4] ? pFld[4] : "");

    return True;
    }
  return False;
  }

void CMdlGraphicShow::StartUp()
  {
  };

void CMdlGraphicShow::ShutDown()
  {
  Clear(true);
  };
void CMdlGraphicShow::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlGraphicShow::OnLButtonDblClk");
  CDialog::OnLButtonDblClk(nFlags, point);
  TryClear(true);
  }

void CMdlGraphicShow::OnMouseMove(UINT nFlags, CPoint point) 
  {
  CDialog::OnMouseMove(nFlags, point);

  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_MouseMove, this, ClientRct, point, nFlags);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);

  //  if ((nFlags & MK_LBUTTON) && (nFlags & (MK_SHIFT|MK_CONTROL)))
  //    {
  //    int i=m_iChartItem;
  //    if (point.y>=m_ItemClientRow[0])
  //      {
  //      i=0;
  //      while (i<m_nLines&& m_ItemClientRow[i+1]<point.y)
  //        i++;
  //      m_iChartWithZero=((nFlags & MK_CONTROL)!=NULL);      
  //      DoTheUpdate();
  //      }
  //    else
  //      i=-1;
  //    if (i!=m_iChartItem)     
  //      {
  //      m_iChartWithZero=((nFlags & MK_CONTROL)!=NULL);      
  //      DoTheUpdate();
  //      }
  //    }
  }

//static Strng m_sDragTag;
CMdlGraphicShow * CMdlGraphicShow::m_pStartWnd=NULL;

void CMdlGraphicShow::OnLButtonDown(UINT nFlags, CPoint point) 
  {


  //dbgpln("CMdlGraphicShow::OnLButtonDown");
  CDialog::OnLButtonDown(nFlags, point);

  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_LButtonDown, this, ClientRct, point, nFlags);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);

  if (nFlags & (MK_SHIFT|MK_CONTROL))
    {
    //if (point.y>=m_ItemClientRow[0])
    //  {
    //  m_iChartItem=0;
    //  while (m_iChartItem<m_nLines&& m_ItemClientRow[m_iChartItem+1]<point.y)
    //    m_iChartItem++;
    //  m_ItemChartWithZero[m_iChartItem]=((nFlags & MK_CONTROL)!=NULL);      
    //  DoTheUpdate();
    //  }
    //else
    //  m_iChartItem=-1;
    }
  else
    {
    m_pStartWnd=this;
    }
  }

void CMdlGraphicShow::OnLButtonUp(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlGraphicShow::OnLButtonUp");
  CDialog::OnLButtonUp(nFlags, point);

  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_LButtonUp, this, ClientRct, point, nFlags);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);

  if (m_pStartWnd && m_pStartWnd!=this)
    {
    int n=0;
    for (int i=0; i<m_pStartWnd->m_nTags; i++)
      if (AcceptNewTag(m_pStartWnd->m_sTag[i](), m_pStartWnd->m_sObjClassId()))
        n++;
    if (n)
      {
      m_pStartWnd->DestroyWindow();
      delete m_pStartWnd;
      DoTheUpdate();
      }
    }
  //else if (m_iChartItem>=0)
  //  {
  //  m_iChartItem=-1;
  //  DoTheUpdate();
  //  }
  else
    {
    int i=0;
    //while (i<m_nTags && m_TagClientCol[i+1]<point.x)
    //  i++;
    //if (i<m_nTags && i!=m_iTagSelected)
    //  {
    //  m_iTagSelected=i;
    //  DoTheUpdate();
    //  }
    }
  m_pStartWnd=NULL;
  }

void CMdlGraphicShow::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlGraphicShow::OnRButtonDown");
  CDialog::OnRButtonDown(nFlags, point);
  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_RButtonDown, this, ClientRct, point, nFlags);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);
  }

void CMdlGraphicShow::OnRButtonUp(UINT nFlags, CPoint point) 
  {
  //dbgpln("CMdlGraphicShow::OnRButtonUp");
  CDialog::OnRButtonUp(nFlags, point);

  CRect ClientRct;
  GetClientRect(&ClientRct);
  CMdlGraphicWnd MGW(MGT_RButtonUp, this, ClientRct, point, nFlags);
  gs_Exec.OperateModelGraphic(m_sTag[0](), MGW, m_Graphics[0]);

  //if (point.y>=m_ItemClientRow[0] && m_nTags>1)
  //  {
  //  int i=0;
  //  while (i<m_nLines&& m_ItemClientRow[i+1]<point.y)
  //    i++;
  //  if (i<m_nLines)
  //    {
  //    bool &WithZero=m_ItemChartWithZero[i];
  //    bool &BarOn=m_ItemChartOn[i];
  //    bool OldBarOn=BarOn;
  //    
  //    if (nFlags & (MK_SHIFT|MK_CONTROL))
  //      {
  //      BarOn=!BarOn;
  //      WithZero=((nFlags & MK_CONTROL)!=NULL);
  //      }
  //    else
  //      {
  //      CRect WRect;
  //      GetWindowRect(&WRect);
  //      CPoint  RBPoint;
  //      RBPoint.x = WRect.left+point.x;
  //      RBPoint.y = WRect.top+point.y;

  //      CMenu Menu;
  //      Menu.CreatePopupMenu();

  //      Menu.AppendMenu(MF_STRING, 100, "&Bars With Zero");
  //      Menu.AppendMenu(MF_STRING, 101, "&Bars Zero Supressed");
  //      Menu.AppendMenu(MF_STRING, 102, "&Bars Off");

  //      int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, RBPoint.x, RBPoint.y, this);
  //      Menu.DestroyMenu();                                           
  //      switch (RetCd)
  //        {
  //        case 100: BarOn=true;   WithZero=true;   break;
  //        case 101: BarOn=true;   WithZero=false;  break;
  //        case 102: BarOn=false;  WithZero=false;  break;
  //        }
  //      }

  //    if (BarOn != OldBarOn)
  //      {
  //      m_ItemChartColour[i]=byte(m_ItemChartColourIndex);
  //      m_ItemChartColourIndex=(m_ItemChartColourIndex+1)%nColours;
  //      m_ItemChartWithZero[i]=WithZero;
  //      }
  //    DoTheUpdate();
  //    }
  //  }
  }

//---------------------------------------------------------------------------

bool CMdlGraphicShow::AcceptNewTag(char * pTag, char * pObjClassId)
  {
  if (pObjClassId && (m_sObjClassId.XStrICmp(pObjClassId)==0))
    if (m_nTags<MdlValueShowMaxTags && m_nTags<gs_RTLCom.m_lMaxTags)
      {
      m_sTag[m_nTags++]=pTag;
      // In case wnd is hidden
      ShowWindow(SW_SHOWNOACTIVATE);
      return true;
      }
    return false;
  };

//---------------------------------------------------------------------------

void CMdlGraphicShow::OnNcPaint() 
  {
  CDialog::OnNcPaint();
  }


//---------------------------------------------------------------------------

void CMdlGraphicShow::OnClose() 
  {
  //_bMustDestroy=true;
  CDialog::OnClose();
  DestroyWindow();
  delete this;
  }

/////////////////////////////////////////////////////////////////////////////
// CRptTagLists dialog


CRptTagLists::CRptTagLists(CWnd* pParent /*=NULL*/)
: CDialog(CRptTagLists::IDD, pParent)
  {
  //m_pSectBuff=NULL;
  //m_dwSectBuff=0;
  m_nVarTags=0;

  //{{AFX_DATA_INIT(CRptTagLists)
  m_UnitTagList = _T("");
  m_MaxValueLen = gs_RTLCom.m_lMaxValueLen;
  m_MaxTags = gs_RTLCom.m_lMaxTags;
  //}}AFX_DATA_INIT


  Create(CRptTagLists::IDD, pParent);

  CRect WinRect;
  GetWindowRect(&WinRect);
  //WndRightShift *= -1;

  CWnd * pCtl=GetDlgItem(IDC_UNITTAGLIST);
  CRect CtlRect;
  pCtl->GetWindowRect(&CtlRect);

  WinRect.right = /*WinRect.left +*/ CtlRect.left;

  MoveWindow(&WinRect, true);
  }

//---------------------------------------------------------------------------
CRptTagLists::~CRptTagLists()
  {
  }

//---------------------------------------------------------------------------

void CRptTagLists::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRptTagLists)
  DDX_Control(pDX, IDC_INCFG, m_InCfgCtrl);
  DDX_Control(pDX, IDC_VARTAGLIST, m_VarTagList);
  DDX_Control(pDX, IDC_MODELTYPE, m_ModelType);
  DDX_LBString(pDX, IDC_UNITTAGLIST, m_UnitTagList);
  DDX_Text(pDX, IDC_MAXVALUELEN, m_MaxValueLen);
  DDX_Text(pDX, IDC_MAXLISTTAGS, m_MaxTags);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CRptTagLists, CDialog)
  //{{AFX_MSG_MAP(CRptTagLists)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_BN_CLICKED(IDC_DELETE, OnDelete)
  ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
  ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
  ON_BN_CLICKED(IDC_QREPORT, OnQreport)
  ON_CBN_SELCHANGE(IDC_MODELTYPE, OnSelchangeModeltype)
  ON_NOTIFY(LVN_ENDLABELEDIT, IDC_VARTAGLIST, OnEndlabeleditVartaglist)
  ON_NOTIFY(NM_DBLCLK, IDC_VARTAGLIST, OnDblclkVartaglist)
  ON_BN_CLICKED(IDC_INCFG, OnIncfg)
  ON_NOTIFY(NM_RCLICK, IDC_VARTAGLIST, OnRclickVartaglist)
  ON_EN_KILLFOCUS(IDC_MAXVALUELEN, OnKillfocusMaxvaluelen)
  ON_EN_KILLFOCUS(IDC_MAXLISTTAGS, OnKillfocusMaxTags)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRptTagLists message handlers

const int NQVColumns=4;

BOOL CRptTagLists::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  m_ModelTypesLoaded=false;

  //StripClassIdVersion(ObjClassId);
  //  VERIFY(m_VarTagList.SubclassMoveUpDownControls(this, IDC_VARTAGLIST, IDC_MOVEUP, IDC_MOVEDOWN));
  CRect Cr;
  m_VarTagList.GetClientRect(&Cr);
  m_VarTagList.InsertColumn(0, "Label", LVCFMT_LEFT, (Cr.Width()*4)/12);
  m_VarTagList.InsertColumn(1, "Tag",   LVCFMT_LEFT, (Cr.Width()*4)/12);
  m_VarTagList.InsertColumn(2, "Cnv",   LVCFMT_LEFT, (Cr.Width()*5)/24);
  m_VarTagList.InsertColumn(3, "Fmt",   LVCFMT_LEFT, (Cr.Width()*3)/24);

  ASSERT(NQVColumns==4);

  CProfINIFile PF(PrjIniFile());
  gs_RTLCom.m_InBaseCfg =false;
  gs_RTLCom.m_InCfg =PF.RdInt("RptListsInCfg", gs_RTLCom.InCfgName(), gs_RTLCom.m_InCfg);
  gs_RTLCom.m_lMaxValueLen=PF.RdInt("RptListsMaxValLen", "All", gs_RTLCom.m_lMaxValueLen);
  gs_RTLCom.m_lMaxTags=PF.RdInt("RptListsMaxTags", "All", gs_RTLCom.m_lMaxTags);

  m_InCfgCtrl.SetCheck(gs_RTLCom.m_InCfg ? 1:0);
  m_MaxValueLen=gs_RTLCom.m_lMaxValueLen;
  m_MaxTags=gs_RTLCom.m_lMaxTags;

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CRptTagLists::StartUp()
  {
  CMdlValueShow::StartUp();
  CMdlValueSet::StartUp();
  CMdlGraphicShow::StartUp();
  gs_RTLCom.m_pRTL = new CRptTagLists;
  gs_RTLCom.m_pRTL->Hide();
  }

//---------------------------------------------------------------------------

void CRptTagLists::ShutDown()
  {
  if (gs_RTLCom.m_pRTL)
    {
    CMdlValueShow::ShutDown();
    CMdlValueSet::ShutDown();
    CMdlGraphicShow::ShutDown();

    delete gs_RTLCom.m_pRTL;
    }
  gs_RTLCom.m_pRTL=NULL;
  }

//---------------------------------------------------------------------------

void CRptTagLists::LoadModelType()
  {
  m_ModelType.ResetContent();
  TagObjClass *pTOC = TagObjClass::FirstClass();
  while (pTOC)
    {
    char *pG=pTOC->Group();
    if (_stricmp(pG, FlwUnitGrp)==0 || 
      _stricmp(pG, FlwLinkGrp)==0 || 
      _stricmp(pG, CtrlLinkGrp)==0 || 
      _stricmp(pG, ElecLinkGrp)==0 || 
      _stricmp(pG, AirLinkGrp)==0)
      {
      Strng CLSID(pTOC->ClassIdNoVersion());
      if (m_ModelType.FindString(-1, CLSID())==CB_ERR)
        {
        m_ModelType.AddString(CLSID());
        }
      }
    pTOC = pTOC->NextClass();
    }
  };

//---------------------------------------------------------------------------

void CRptTagLists::LoadCtrls()
  {
  Strng F;
  m_VarTagList.DeleteAllItems();
  for (int i=0; i<m_nVarTags; i++)
    {
    CVarTagItem &TI=m_VarTags[i];
    LVITEM LVI;
    LVI.mask=LVIF_TEXT|LVIF_PARAM;
    LVI.iItem=i;
    LVI.iSubItem=0;
    LVI.pszText=TI.m_sLabel() ? TI.m_sLabel() : TI.m_sTag();
    LVI.lParam=i;
    m_VarTagList.InsertItem(&LVI);

    LVI.mask=LVIF_TEXT;
    LVI.iSubItem=1;
    LVI.pszText=TI.m_sTag();
    m_VarTagList.SetItem(&LVI);

    LVI.mask=LVIF_TEXT;
    LVI.iSubItem=2;
    LVI.pszText=TI.m_Cnv.Text();
    m_VarTagList.SetItem(&LVI);

    LVI.mask=LVIF_TEXT;
    LVI.iSubItem=3;
    LVI.pszText=TI.m_Fmt.BuildStr(F);
    m_VarTagList.SetItem(&LVI);
    }
  };

//---------------------------------------------------------------------------

void CRptTagLists::DoShow(LPCSTR RqdClass, LPCSTR Layout, flag ForceShow)
  {
  //  WINDOWPLACEMENT WP;
  //  WP.length=sizeof(WP);
  //  if (GetWindowPlacement(&WP))
  //    {
  //    if (WP.showCmd==SW_HIDE)
  //      return;
  //    }
  if (!IsWindowVisible() && !ForceShow)
    return;

  int CurSel=m_ModelType.GetCurSel();
  int RqdSel=CurSel;

  if (!m_ModelTypesLoaded || ForceShow)
    {
    LoadModelType();
    m_ModelTypesLoaded=true;
    }

  if (RqdClass)
    {
    int i=m_ModelType.FindString(-1, RqdClass);
    if (i==CB_ERR)
      {
      Strng s(RqdClass);
      StripClassIdVersion(s);
      i=m_ModelType.FindString(-1, s());
      }
    if (i!=CB_ERR)
      RqdSel=i;
    }


  if (1)//RqdSel!=CurSel)
    {
    m_ModelType.SetCurSel(Range(0, RqdSel, m_ModelType.GetCount()-1));

    CurSel=m_ModelType.GetCurSel();
    if (CurSel!=CB_ERR)
      {
      CString S;
      m_ModelType.GetLBText(CurSel, S);

      gs_RTLCom.SetObjClassId(S);

      CProfINIFile PFI(PrjIniFile());
      gs_RTLCom.m_InCfg =PFI.RdInt("RptListsInCfg", gs_RTLCom.InCfgName(), gs_RTLCom.m_InCfg);
      gs_RTLCom.m_lMaxValueLen=PFI.RdInt("RptListsMaxValLen", "All", gs_RTLCom.m_lMaxValueLen);
      gs_RTLCom.m_lMaxTags=PFI.RdInt("RptListsMaxTags", "All", gs_RTLCom.m_lMaxTags);

      m_InCfgCtrl.SetCheck(gs_RTLCom.m_InCfg ? 1:0);
      m_MaxValueLen=gs_RTLCom.m_lMaxValueLen;
      m_MaxTags=gs_RTLCom.m_lMaxTags;

      gs_RTLCom.LoadVarTags(S, Layout, m_VarTags, m_nVarTags);
      LoadCtrls();
      }

    }
  ShowWindow(SW_SHOW);
  };

//---------------------------------------------------------------------------

void CRptTagLists::AddItem(TagInfoBlk & IB)
  {
  gs_RTLCom.m_pRTL->AddItem(IB.ObjClassId(), IB.RefTag(), IB.Cnv(), IB.Fmt());
  }

//---------------------------------------------------------------------------

void CRptTagLists::AddItem(LPCSTR pObjClassId, LPCSTR pRefTag, CnvAttribute &Cnv, FmtAttribute &Fmt)
  {
  if (pRefTag==NULL)
    return;

  Strng ObjClassId;
  Strng TheTag;
  Strng CnvTxt;
  TaggedObject::SplitTagCnv((char*)pRefTag, TheTag, CnvTxt);  

  if (pObjClassId==NULL)
    {
    Strng OTag;//(TheTag);
    int L=TheTag.Length();
    int D=TheTag.XStrChr('.')-TheTag();
    if (D>0)
      {
      OTag=TheTag.Left(D);
      TheTag=TheTag.Right(L-1-D);
      }
    else
      OTag="??";

    Strng T;
    T.Set("%s.ClassId", OTag());

    CXM_ObjectTag  ObjTag(T(), TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    CXM_Route      Route;

    if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, Route))
      {
      CPkDataItem * pItem=ObjData.FirstItem();
      if (IsStrng(pItem->Type()))
        ObjClassId = pItem->Value()->GetString("", "", 0, "");
      }
    else
      LogError((char*)pRefTag, 0, "ClassId not found");
    }
  else
    ObjClassId=pObjClassId;

  StripClassIdVersion(ObjClassId);

  //Strng Sect, Item;
  //Sect.Set("%s.View.Vars", ObjClassId());

  DoShow(ObjClassId(), NULL, true); // Will Load Data

  flag Found=false;
  for (int i=0; i<m_nVarTags; i++)
    if (m_VarTags[i].m_sTag.XStrICmp(TheTag())==0)
      {
      Found=true;
      //for (; i<m_nVarTags-1; i++)
      //  m_VarTags[i]=m_VarTags[i+1];
      //m_nVarTags--;
      break;
      }

    if (!Found)
      {
      m_VarTags.SetSize(Max(m_nVarTags+1, m_VarTags.GetSize()));
      CVarTagItem &TI=m_VarTags[m_nVarTags++];
      TI.m_sLabel=TheTag();
      TI.m_sTag=TheTag();
      TI.m_Cnv=Cnv;
      TI.m_Fmt=Fmt;

      gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
      LoadCtrls();
      gs_AccessWnds.PostMessage(-1, WMU_ACCREFRESHCANCEL);
      }
  }

//---------------------------------------------------------------------------

void CRptTagLists::DoHide()
  {
  ShowWindow(SW_HIDE);
  };

//---------------------------------------------------------------------------

void CRptTagLists::Show(LPCSTR RqdClass, flag ForceShow)
  {
  if (gs_RTLCom.m_pRTL)
    gs_RTLCom.m_pRTL->DoShow(RqdClass, NULL, ForceShow);
  };

//---------------------------------------------------------------------------

void CRptTagLists::Hide()
  {
  if (gs_RTLCom.m_pRTL)
    gs_RTLCom.m_pRTL->DoHide();
  };

void CRptTagLists::LoadVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags)
  { 
  gs_RTLCom.LoadVarTags(MdlType, Layout, VarTags, nVarTags);
  }

void CRptTagLists::SaveVarTags(LPCSTR MdlType, LPCSTR Layout, CVarTagArray & VarTags, int &nVarTags, CIArray * SaveOrder)
  {
  gs_RTLCom.SaveVarTags(MdlType, Layout, VarTags, nVarTags, SaveOrder);
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnAdd() 
  {
  /*
  int OneDone=false;
  m_VarTagList.LockWindowUpdate();
  int j=Max(0, m_VarTagList.GetFirstSelectedItem());//+1);
  POSITION pos = m_SpDBList.GetFirstSelectedItemPosition();
  int iLastSel=-1;
  while (pos)
  {
  iLastSel=m_SpDBList.GetNextSelectedItem(pos);
  CString S=m_SpDBList.GetItemText(iLastSel,0);

  int iPos=m_VarTagList.FindStringExact(-1, S);
  if (iPos<0) // Not Found
  {
  int n=m_VarTagList.InsertItem(j++, S, 1);
  m_VarTagList.SetItemData(n, SetType(spl_SpecieId)|m_SpDBList.GetItemData(iLastSel));
  m_SpDBList.SetItemImage(iLastSel, 1);
  OneDone=true;
  }
  }

  if (OneDone)
  {
  m_VarTagList.SetItemState(-1, 0, LVIS_SELECTED);
  m_VarTagList.SetItemState(j-1, LVIS_SELECTED, LVIS_SELECTED);
  m_VarTagList.EnsureVisible(j-1, false);
  m_VarTagList.UnlockWindowUpdate();
  if (iLastSel)
  {
  m_SpDBList.SetItemState(-1, 0, LVIS_SELECTED);
  m_SpDBList.SetItemState(iLastSel, LVIS_SELECTED, LVIS_SELECTED);
  }
  }
  else
  m_VarTagList.UnlockWindowUpdate();
  */  
  gs_AccessWnds.PostMessage(-1, WMU_ACCREFRESHCANCEL);
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnDelete() 
  {
  m_VarTagList.LockWindowUpdate();
  int iFirst=m_VarTagList.GetItemCount();
  int j;

  POSITION pos;
  while ((pos = m_VarTagList.GetFirstSelectedItemPosition())!=NULL)
    {
    j=m_VarTagList.GetNextSelectedItem(pos);
    m_VarTagList.DeleteItem(j);
    iFirst=Min(iFirst, j);
    }
  if (iFirst<m_VarTagList.GetItemCount())
    {
    m_VarTagList.SetItemState(iFirst, LVIS_SELECTED, LVIS_SELECTED);
    m_VarTagList.EnsureVisible(iFirst, false);
    }
  m_VarTagList.UnlockWindowUpdate();

  m_SaveOrder.SetSize(m_VarTagList.GetItemCount());
  for (int i=0; i<m_VarTagList.GetItemCount(); i++)
    {
    m_SaveOrder[i]=m_VarTagList.GetItemData(i);
    m_VarTagList.SetItemData(i, i); // renumber
    }
  gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags, &m_SaveOrder);
  gs_RTLCom.LoadVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
  //  LoadCtrls();
  gs_AccessWnds.PostMessage(-1, WMU_ACCREFRESHCANCEL);
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnMoveup() 
  {
  int aMove[1000];
  int iMove=0;
  int iDst=m_VarTagList.GetItemCount();

  POSITION pos = m_VarTagList.GetFirstSelectedItemPosition();
  while (pos && iMove<1000)
    {
    aMove[iMove]=m_VarTagList.GetNextSelectedItem(pos);
    iDst=Min(iDst,aMove[iMove]);
    iMove++;
    }

  iDst=Max(0, iDst-1);
  int iFirst=iDst;
  m_VarTagList.LockWindowUpdate();
  for (int j=0; j<iMove; j++, iDst++)
    {
    char Buff[5][256];
    LVITEM LVI[5];
    for (int i=0; i<NQVColumns; i++)
      {
      //LVI[i].mask=LVIF_TEXT|LVIF_IMAGE|LVIF_STATE|LVIF_PARAM;
      LVI[i].mask=LVIF_TEXT|(i>0 ? 0 : LVIF_IMAGE|LVIF_STATE|LVIF_PARAM);
      LVI[i].iItem=aMove[j];
      LVI[i].iSubItem=i;
      LVI[i].stateMask=0xFFFFFFFF;
      LVI[i].pszText=Buff[i];
      LVI[i].cchTextMax=sizeof(Buff[i]);
      m_VarTagList.GetItem(&LVI[i]);
      LVI[i].iItem=iDst;
      }
    m_VarTagList.DeleteItem(aMove[j]);
    m_VarTagList.InsertItem(&LVI[0]);
    for (i=1; i<NQVColumns; i++)
      m_VarTagList.SetItem(&LVI[i]);
    }

  m_VarTagList.EnsureVisible(iFirst, false);
  m_VarTagList.UnlockWindowUpdate();

  m_SaveOrder.SetSize(m_nVarTags);
  for (int i=0; i<m_nVarTags; i++)
    m_SaveOrder[i]=m_VarTagList.GetItemData(i);
  gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags, &m_SaveOrder);
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnMovedown() 
  {
  int aMove[1000];
  int iMove=0;
  int iDst=-1;

  POSITION pos = m_VarTagList.GetFirstSelectedItemPosition();
  while (pos && iMove<1000)
    {
    aMove[iMove]=m_VarTagList.GetNextSelectedItem(pos);
    iDst=Max(iDst,aMove[iMove]);
    iMove++;
    }

  //char Buff[256];
  m_VarTagList.LockWindowUpdate();
  iDst=Min(m_VarTagList.GetItemCount()-1, iDst+1);
  int iFirst=iDst;
  for (int j=iMove-1; j>=0; j--, iDst--)
    {
    char Buff[5][256];
    LVITEM LVI[5];
    for (int i=0; i<NQVColumns; i++)
      {
      LVI[i].mask=LVIF_TEXT|(i>0 ? 0 : LVIF_IMAGE|LVIF_STATE|LVIF_PARAM);
      LVI[i].iItem=aMove[j];
      LVI[i].iSubItem=i;
      LVI[i].stateMask=0xFFFFFFFF;
      LVI[i].pszText=Buff[i];
      LVI[i].cchTextMax=sizeof(Buff[i]);
      m_VarTagList.GetItem(&LVI[i]);
      LVI[i].iItem=iDst;
      }
    m_VarTagList.DeleteItem(aMove[j]);
    m_VarTagList.InsertItem(&LVI[0]);
    for (i=1; i<NQVColumns; i++)
      m_VarTagList.SetItem(&LVI[i]);
    }

  m_VarTagList.EnsureVisible(iFirst, false);
  m_VarTagList.UnlockWindowUpdate();

  m_SaveOrder.SetSize(m_nVarTags);
  for (int i=0; i<m_nVarTags; i++)
    m_SaveOrder[i]=m_VarTagList.GetItemData(i);
  gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags, &m_SaveOrder);
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnOK() 
  {
  //  m_SaveOrder.SetSize(m_nVarTags);
  //  for (int i=0; i<m_nVarTags; i++)
  //    m_SaveOrder[i]=m_VarTagList.GetItemData(i);
  //  gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags, &m_SaveOrder);
  Hide();

  //	CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnQreport() 
  {
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnSelchangeModeltype() 
  {
  //SaveVarTags(NULL);
  int i=m_ModelType.GetCurSel();
  if (i!=CB_ERR)
    {
    CString S;
    m_ModelType.GetLBText(i, S);
    gs_RTLCom.SetObjClassId(S);

    CProfINIFile PFI(PrjIniFile());

    gs_RTLCom.m_InCfg =PFI.RdInt("RptListsInCfg", gs_RTLCom.InCfgName(), gs_RTLCom.m_InCfg);
    gs_RTLCom.m_lMaxValueLen=PFI.RdInt("RptListsMaxValLen", "All", gs_RTLCom.m_lMaxValueLen);
    gs_RTLCom.m_lMaxTags=PFI.RdInt("RptListsMaxTags", "All", gs_RTLCom.m_lMaxTags);

    m_InCfgCtrl.SetCheck(gs_RTLCom.m_InCfg ? 1:0);
    m_MaxValueLen=gs_RTLCom.m_lMaxValueLen;
    m_MaxTags=gs_RTLCom.m_lMaxTags;

    gs_RTLCom.LoadVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
    LoadCtrls();
    }	
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnEndlabeleditVartaglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  if (pDispInfo->item.pszText && strlen(pDispInfo->item.pszText)>0) // Edit Accepted
    {
    int i=pDispInfo->item.iItem;
    m_VarTags[i].m_sLabel=pDispInfo->item.pszText;
    gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
    m_VarTagList.SetItemText(i,0,pDispInfo->item.pszText);
    }

  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnDblclkVartaglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LPNMLISTVIEW pInfo = (NMLISTVIEW*)pNMHDR;
  int j=pInfo->iItem;
  if (j<0)
    {
    CPoint Pt;
    for (int i=0; m_VarTagList.GetItemPosition(i, &Pt); i++)
      if (Pt.y>=pInfo->ptAction.y)
        break;
    j=i-1;
    }

  if (j>=0 && j<m_VarTagList.GetItemCount())
    {
    m_VarTagList.SetItemState(-1, 0, LVIS_SELECTED);
    m_VarTagList.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
    m_VarTagList.EnsureVisible(j, false);
    m_VarTagList.EditLabel(j);
    }

  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnIncfg() 
  {
  // TODO: Add your control notification handler code here
  gs_RTLCom.m_InCfg=!gs_RTLCom.m_InCfg;
  CProfINIFile PFI(PrjIniFile());
  m_InCfgCtrl.SetCheck(gs_RTLCom.m_InCfg ? 1:0);
  PFI.WrInt("RptListsInCfg", gs_RTLCom.InCfgName(), gs_RTLCom.m_InCfg);

  gs_RTLCom.LoadVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
  LoadCtrls();
  gs_AccessWnds.PostMessage(-1, WMU_ACCREFRESHCANCEL);
  }

void CRptTagLists::OnKillfocusMaxvaluelen() 
  {
  UpdateData(true);
  CProfINIFile PFI(PrjIniFile());
  m_MaxValueLen=Range(5L, m_MaxValueLen, 40L);
  gs_RTLCom.m_lMaxValueLen=m_MaxValueLen;
  PFI.WrInt("RptListsMaxValLen", "All", gs_RTLCom.m_lMaxValueLen);
  UpdateData(false);
  }

void CRptTagLists::OnKillfocusMaxTags() 
  {
  UpdateData(true);
  CProfINIFile PFI(PrjIniFile());
  m_MaxTags=Range(1L, m_MaxTags, (long)MdlValueShowMaxTags);
  gs_RTLCom.m_lMaxTags=m_MaxTags;
  PFI.WrInt("RptListsMaxTags", "All", gs_RTLCom.m_lMaxTags);
  UpdateData(false);
  }

//---------------------------------------------------------------------------

void CRptTagLists::OnRclickVartaglist(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LPNMLISTVIEW pInfo = (NMLISTVIEW*)pNMHDR;
  int j=pInfo->iItem;

  if (j<0)
    {
    CPoint Pt;
    for (int i=0; m_VarTagList.GetItemPosition(i, &Pt); i++)
      if (Pt.y>=pInfo->ptAction.y)
        break;
    j=i-1;
    }

  if (j>=0 && j<m_VarTagList.GetItemCount())
    {
    CMenu Menu;
    Menu.CreatePopupMenu();

    CnvAttribute &Cnv=m_VarTags[j].m_Cnv;
    FmtAttribute &Fmt=m_VarTags[j].m_Fmt;

    FmtAttribute::SetCmdIndex(IDM_ACC_FMT1);
    CMenu FmtMenu;
    FmtMenu.CreatePopupMenu();
    Fmt.AddToMenu(FmtMenu);

    CnvAttribute::SetCmdIndex(IDM_ACC_CNV1); 
    CMenu CnvMenu;
    CnvMenu.CreatePopupMenu();
    Cnv.AddToMenu(CnvMenu);

    Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
    Menu.AppendMenu(MF_POPUP, (unsigned int)FmtMenu.m_hMenu, "&Format");
    //Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Edit");

    CPoint ScreenPoint(pInfo->ptAction);
    m_VarTagList.ClientToScreen(&ScreenPoint);
    int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, ScreenPoint.x, ScreenPoint.y, this);
    //Shared.DoRButtonMenu(RetCd);
    Menu.DestroyMenu();
    if (RetCd>=IDM_ACC_CNV1 && RetCd<=IDM_ACC_CNVN)
      {
      pCDataCnv pC=Cnvs[Cnv.Index()];
      for (int i=RetCd-IDM_ACC_CNV1; i>0; i--)
        pC=pC->Next();
      if (pC)
        Cnv.SetText(pC->Txt());
      gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
      m_VarTagList.SetItemText(j, 2, Cnv.Text());
      }
    else if (RetCd>=IDM_ACC_FMT1 && RetCd<=IDM_ACC_FMTN)
      {
      for (long i=0; i<DefinedFmts.GetSize(); i++) 
        if (i==RetCd-IDM_ACC_FMT1) 
          break;

      if (i<DefinedFmts.GetSize())
        Fmt=DefinedFmts[i];
      gs_RTLCom.SaveVarTags(gs_RTLCom.ObjClassId(), NULL, m_VarTags, m_nVarTags);
      m_VarTagList.SetItemText(j, 3, Fmt.Name());
      }
    }
  *pResult = 0;
  }

//===========================================================================
//
//
//
//===========================================================================
