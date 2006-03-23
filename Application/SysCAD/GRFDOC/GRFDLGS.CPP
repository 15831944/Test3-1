//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "..\resource.h"
#include "grfdlgs.h"
#include "gpwfuncs.h"
#include "grf3drw.h"
#include "cmdmenu.h"
#include "errorlog.h"
#include "msgwnd.h"
#include "dynmseg.h"

extern "C"
  {
#include "grldefs.h"
#include ".\grfdlgs.h"
  }

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

void CFillColourButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
  {
  //COLORREF RGB=grl_get_win_color(m_Colour);
  HBRUSH hBrush=CreateSolidBrush(m_ColourRGB);
  ::FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBrush);
  DeleteObject(hBrush);
  };

CFillColourButton::CFillColourButton ()
  {
  m_ColourRGB=RGB(0,255,255);//GR_CYAN;
  }

void CFillColourButton::SetColour(COLORREF Colour)
  {
  m_ColourRGB=Colour;
  Invalidate();
  };

//---------------------------------------------------------------------------

CCreateFillDlg::CCreateFillDlg(CWnd* pParent, char* pTag, char* pVar, double Min, double Max, int PointsCnt, BOOL ApplyFillToAll, COLORREF FillColour)
: CDialog(CCreateFillDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  //{{AFX_DATA_INIT(CCreateFillDlg)
  m_Vars = pVar;
  m_ApplyFillToAll = ApplyFillToAll;
  m_Min = Min;
  m_Max = Max;
  //}}AFX_DATA_INIT
  m_FillColour.m_ColourRGB = FillColour;
  iPointsCnt = PointsCnt;
  sTag = pTag;
  Create(CCreateFillDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CCreateFillDlg::~CCreateFillDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCreateFillDlg)
  DDX_Text(pDX, IDC_VARS, m_Vars);
  DDX_Check(pDX, IDC_FILLALL, m_ApplyFillToAll);
  DDX_Text(pDX, IDC_MIN, m_Min);
  DDX_Text(pDX, IDC_MAX, m_Max);
  DDX_Control(pDX, IDC_FILLCOLOURBTN, m_FillColour);
  //}}AFX_DATA_MAP
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCreateFillDlg, CDialog)
  //{{AFX_MSG_MAP(CCreateFillDlg)
  ON_EN_CHANGE(IDC_VARS, OnChangeVars)
  ON_BN_CLICKED(IDC_DELETE, OnDelete)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOKBtn)
  ON_BN_CLICKED(IDC_FILLCOLOURBTN, OnBnClickedFillcolourbtn)
END_MESSAGE_MAP()


//---------------------------------------------------------------------------

void CCreateFillDlg::SetTag(char* pTag)
  {
  sTag = pTag;
  SetDlgItemText(IDC_TAG, pTag);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::SetVar(char* pVar)
  {
  UpdateData(TRUE);
  m_Vars = pVar;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::SetMin(double d)
  {
  UpdateData(TRUE);
  m_Min = d;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::SetMax(double d)
  {
  UpdateData(TRUE);
  m_Max = d;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::SetApplyToAll(BOOL b)
  {
  UpdateData(TRUE);
  m_ApplyFillToAll = b;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::SetPointsCount(int Cnt)
  {
  iPointsCnt = Cnt;
  if (iPointsCnt<0)
    {
    GetDlgItem(IDC_POINTSCNT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_POINTSCNTTXT)->ShowWindow(SW_HIDE);
    }
  else
    {
    char Buff[16];
    sprintf(Buff, "%d", Cnt);
    SetDlgItemText(IDC_POINTSCNT, Buff);
    GetDlgItem(IDC_DELETE)->ShowWindow(SW_HIDE);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::SetFillColour(COLORREF Colour)
  {
  m_FillColour.SetColour(Colour);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

COLORREF CCreateFillDlg::FillColour()
  {
  return m_FillColour.Colour();
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::OnBnClickedFillcolourbtn()
  {
  CColorDialog ColDlg(FillColour(), CC_RGBINIT | CC_SHOWHELP, this);
  ColDlg.m_cc.lpCustColors = CustomColours;
  if (ColDlg.DoModal()==IDOK)
    {
    SetFillColour(ColDlg.GetColor());
    }
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::OnUpdateOKBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(sTag.GetLength()>0 && m_Vars.GetLength()>0 && (iPointsCnt>2 || iPointsCnt<0));
  }

//---------------------------------------------------------------------------

BOOL CCreateFillDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  if (iPointsCnt<0)
    {
    GetDlgItem(IDC_POINTSCNT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_POINTSCNTTXT)->ShowWindow(SW_HIDE);
    }
  else
    GetDlgItem(IDC_DELETE)->ShowWindow(SW_HIDE);
  SetDlgItemText(IDC_TAG, (const char*)sTag);
  UpdateDialogControls(this, FALSE);
  ShowWindow(SW_SHOW);
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::OnChangeVars()
  {
  UpdateData(TRUE);
  }

//---------------------------------------------------------------------------

void CCreateFillDlg::OnDelete()
  {
  gs_pCmd->ExtendCmdLine(" delete \r");
  }

//===========================================================================
// CCreateBarDlg dialog used for creating dynamic bars, annotation and toggles

CCreateBarDlg::CCreateBarDlg(CWnd* pParent, Strng_List* TagReq, Strng_List* VarReq, bool TagsOn, double Min, double Max, COLORREF Col, double Scale, byte UseType)
: CDialog(CCreateBarDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  //{{AFX_DATA_INIT(CCreateBarDlg)
  m_TagList = _T("");
  m_VarList = _T("");
  m_Min = Min;
  m_Max = Max;
  m_Scale = Scale;
  //}}AFX_DATA_INIT
  pTagReq = TagReq;
  pVarReq = VarReq;
  bTagsOn = TagsOn;
  iUseType = UseType;
  bDoneCreate = false;
  m_FillColour.m_ColourRGB = Col;
  Create(CCreateBarDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CCreateBarDlg::~CCreateBarDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCreateBarDlg)
  DDX_Text(pDX, IDC_TAGLIST, m_TagList);
  DDX_Text(pDX, IDC_VARLIST, m_VarList);
  DDX_Text(pDX, IDC_MIN, m_Min);
  DDX_Text(pDX, IDC_MAX, m_Max);
  DDX_Text(pDX, IDC_SCALE, m_Scale);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_COLOURBTN, m_FillColour);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCreateBarDlg, CDialog)
  //{{AFX_MSG_MAP(CCreateBarDlg)
  ON_BN_CLICKED(IDC_CREATE, OnCreateBtn)
  ON_EN_CHANGE(IDC_TAGLIST, OnChangeTaglist)
  ON_EN_CHANGE(IDC_VARLIST, OnChangeVarlist)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_COLOURBTN, OnBnClickedFillcolourbtn)
  ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOKBtn)
  ON_UPDATE_COMMAND_UI(IDC_CREATE, OnUpdateCreateBtn)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCreateBarDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  UpdateTagList(FALSE);
  UpdateVarList(FALSE);
  if (iUseType!=0)
    {
    GetDlgItem(IDC_MIN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MAX)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_TXTMIN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_TXTMAX)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_COLOURBTN)->ShowWindow(SW_HIDE);
    }
  switch (iUseType)
    {
    case 0: SetWindowText("Create Dynamic Bars"); break;
    case 1: SetWindowText("Create Annotation"); break;
    case 2: SetWindowText("Create Toggle"); break;
    }
  SetTagsOn(bTagsOn);
  UpdateDialogControls(this, FALSE);
  ShowWindow(SW_SHOW);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::SetMin(double d)
  {
  UpdateData(TRUE);
  m_Min = d;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::SetMax(double d)
  {
  UpdateData(TRUE);
  m_Max = d;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::SetScale(double d)
  {
  UpdateData(TRUE);
  m_Scale = d;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::UpdateList(BOOL bRetrieve, Strng_List& Lst, CString& s)
  {
  if (bRetrieve)
    {
    UpdateData(TRUE);
    Lst.Clear();
    Strng Txt = (const char*)s;
    Lst.AppendTokString((char*)(const char*)s, " ");
    }
  else
    {
    Strng Txt;
    Lst.BuildTokString(Txt, " ");
    s = Txt();
    UpdateData(FALSE);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::UpdateTagList(BOOL bRetrieve)
  {
  UpdateList(bRetrieve, *pTagReq, m_TagList);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::UpdateVarList(BOOL bRetrieve)
  {
  UpdateList(bRetrieve, *pVarReq, m_VarList);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::SetTagsOn(bool TagsOn)
  {
  bTagsOn = TagsOn;
  if (TagsOn)
    {
    SetDlgItemText(IDC_MSG, "Select/enter tags and variables");
    switch (iUseType)
      {
      case 0: SetDlgItemText(IDC_CREATE, "Create Dynamic Bars"); break;
      case 1: SetDlgItemText(IDC_CREATE, "Create Annotation"); break;
      case 2: SetDlgItemText(IDC_CREATE, "Create Toggle"); break;
      }
    }
  else
    {
    SetDlgItemText(IDC_MSG, "Select position and press OK or Position Selected");
    SetDlgItemText(IDC_CREATE, "Position Selected");
    }
  GetDlgItem(IDC_TAGLIST)->EnableWindow(TagsOn);
  GetDlgItem(IDC_VARLIST)->EnableWindow(TagsOn);
  GetDlgItem(IDC_MIN)->EnableWindow(TagsOn);
  GetDlgItem(IDC_MAX)->EnableWindow(TagsOn);
  GetDlgItem(IDC_SCALE)->EnableWindow(TagsOn);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::SetDoneCreate()
  {
  bDoneCreate = 1;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnUpdateCreateBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_TagList.GetLength()>0 && m_VarList.GetLength()>0);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnCreateBtn()
  {
  gs_pCmd->ExtendCmdLine(";");
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnUpdateOKBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!bTagsOn || bDoneCreate);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::SetFillColour(COLORREF Colour)
  {
  m_FillColour.SetColour(Colour);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

COLORREF CCreateBarDlg::FillColour()
  {
  return m_FillColour.Colour();
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnBnClickedFillcolourbtn()
  {
  CColorDialog ColDlg(FillColour(), CC_RGBINIT | CC_SHOWHELP, this);
  ColDlg.m_cc.lpCustColors = CustomColours;
  if (ColDlg.DoModal()==IDOK)
    {
    SetFillColour(ColDlg.GetColor());
    }
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnChangeTaglist()
  {
  UpdateData(TRUE);
  }

//---------------------------------------------------------------------------

void CCreateBarDlg::OnChangeVarlist()
  {
  UpdateData(TRUE);
  }

//===========================================================================
// CUpdateBarDlg dialog used for editing dynamic bars and toggles

CUpdateBarDlg::CUpdateBarDlg(CWnd* pParent, Strng_List* TagReq, Strng_List* VarReq, Strng_List* MinReq, Strng_List* MaxReq, Strng_List* ColReq, DWORD BarsStyle, double Scale, long DispOpts, bool ForBars)
: CDialog(CUpdateBarDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  //{{AFX_DATA_INIT(CUpdateBarDlg)
  m_Tag = _T("");
  m_Var = _T("");
  m_Min = 0.0;
  m_Max = 0.0;
  m_Scale = Scale;
  //}}AFX_DATA_INIT
  m_iDispOpts = DispOpts;
  pTagReq = TagReq;
  pVarReq = VarReq;
  pMinReq = MinReq;
  pMaxReq = MaxReq;
  pColReq = ColReq;
  dwBarsStyle = BarsStyle;
  bChanged = false;
  bForBars = ForBars;
  m_FillColour.m_ColourRGB = DEFAULTBARSCOLOUR;
  if (bForBars)
    {
    if (pColReq)
      {
      pStrng a = pColReq->First();
      if (a)
        m_FillColour.m_ColourRGB = atol(a->Str());
      }
    }
  Create(CUpdateBarDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CUpdateBarDlg::~CUpdateBarDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUpdateBarDlg)
  DDX_Control(pDX, IDC_BARLIST, m_BarList);
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Text(pDX, IDC_VAR, m_Var);
  DDX_Text(pDX, IDC_MINVAL, m_Min);
  DDX_Text(pDX, IDC_MAXVAL, m_Max);
  DDX_Text(pDX, IDC_SCALE, m_Scale);
  DDX_Control(pDX, IDC_DISPOPTS, m_DispOpts);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_BARCOLOURBTN, m_FillColour);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CUpdateBarDlg, CDialog)
  //{{AFX_MSG_MAP(CUpdateBarDlg)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_LBN_SELCHANGE(IDC_BARLIST, OnSelchangeBarlist)
  ON_EN_CHANGE(IDC_SCALE, OnChangeScale)
  ON_CBN_SELCHANGE(IDC_DISPOPTS, OnSelchangeDispOpts)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BARCOLOURBTN, OnBnClickedFillcolourbtn)
  ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_TAG, IDC_MAXVAL, OnKillfocus)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_TAG, IDC_MAXVAL, OnUpdateEdit)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CUpdateBarDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  if (bForBars)
    {
    m_BarList.SetHeaderCount(5);
    m_BarList.SetHeaderItem(0, "Tag", 160);
    m_BarList.SetHeaderItem(1, "Var", 130);
    m_BarList.SetHeaderItem(2, "Min", 50);
    m_BarList.SetHeaderItem(3, "Max", 50);
    m_BarList.SetHeaderItem(4, "Colour", 70);
    }
  else
    {
    m_BarList.SetHeaderCount(2);
    m_BarList.SetHeaderItem(0, "Tag", 180);
    m_BarList.SetHeaderItem(1, "Var", 160);
    }
  m_BarList.MakeHeader(34763);
  m_BarList.ResetContent();
  VERIFY(m_BarList.SubclassMoveUpDownControls(this, IDC_BARLIST, IDC_MOVEUP, IDC_MOVEDOWN));
  VERIFY(m_BarList.SubclassDeleteControl(this, IDC_BARLIST, IDC_DELETE));
  Strng s;
  pStrng a = pTagReq->First();
  pStrng b = pVarReq->First();
  pStrng c = pMinReq->First();
  pStrng d = pMaxReq->First();
  pStrng e = pColReq ? pColReq->First() : NULL;
  if (bForBars)
    {
    char DefCol[32];
    sprintf(DefCol, "%d", DEFAULTBARSCOLOUR);
    while (a && b && c && d)
      {
      s.Set("%s\t%s\t%s\t%s\t%s", a->Str(), b->Str(), c->Str(), d->Str(), e ? e->Str() : DefCol);
      m_BarList.AddString(s());
      a = pTagReq->Next();
      b = pVarReq->Next();
      c = pMinReq->Next();
      d = pMaxReq->Next();
      e = pColReq ? pColReq->Next() : NULL;
      }
    m_DispOpts.AddString("With labels");
    m_DispOpts.AddString("No labels");
    m_DispOpts.SetCurSel(m_iDispOpts);
    }
  else
    {
    while (a && b)
      {
      s.Set("%s\t%s", a->Str(), b->Str());
      m_BarList.AddString(s());
      a = pTagReq->Next();
      b = pVarReq->Next();
      }
    GetDlgItem(IDC_DISPOPTS)->EnableWindow(false);
    GetDlgItem(IDC_DISPOPTS)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MINVAL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_MAXVAL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BARCOLOURBTN)->ShowWindow(SW_HIDE);
    }
  m_BarList.SetCurSel(0);
  OnSelchangeBarlist();
  ShowWindow(SW_SHOW);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::ParseLine(int index)
  {
  char Buff[1024];
  m_BarList.GetText(index, Buff);
  CSVColArray C;
  ParseTabTokens(Buff, C);
  m_Tag = C[0];
  m_Var = C[1];
  m_Min = (C[2] ? atof(C[2]) : 0.0);
  m_Max = (C[3] ? atof(C[3]) : 100.0);
  m_Col = (C[4] ? atol(C[4]) : DEFAULTBARSCOLOUR);
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::UpdateLists()
  {
  UpdateData(TRUE);
  pTagReq->Clear();
  pVarReq->Clear();
  pMinReq->Clear();
  pMaxReq->Clear();
  pColReq->Clear();
  char Buff[64];
  for (int i=0; i<m_BarList.GetCount(); i++)
    {
    ParseLine(i);
    pTagReq->Append((const char*)m_Tag);
    pVarReq->Append((const char*)m_Var);
    sprintf(Buff, "%g", m_Min);
    pMinReq->Append(Buff);
    sprintf(Buff, "%g", m_Max);
    pMaxReq->Append(Buff);
    sprintf(Buff, "%d", m_Col);
    pColReq->Append(Buff);
    }
  }

//--------------------------------------------------------------------------

void CUpdateBarDlg::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::SetFillColour(COLORREF Colour)
  {
  m_FillColour.SetColour(Colour);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

COLORREF CUpdateBarDlg::FillColour()
  {
  return m_FillColour.Colour();
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnBnClickedFillcolourbtn()
  {
  CColorDialog ColDlg(FillColour(), CC_RGBINIT | CC_SHOWHELP, this);
  ColDlg.m_cc.lpCustColors = CustomColours;
  if (ColDlg.DoModal()==IDOK)
    {
    int i = m_BarList.GetCurSel();
    if (i>=0)
      {
      bChanged = true;
      m_Col = ColDlg.GetColor();
      OnKillfocus(0); //force current line to be changed
      }
    }
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnSelchangeDispOpts()
  {
  UpdateData(TRUE);
  long Prev = m_iDispOpts;
  m_iDispOpts = Max(0, m_DispOpts.GetCurSel());
  if (Prev!=m_iDispOpts)
    bChanged = true;
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnSelchangeBarlist()
  {
  int i = m_BarList.GetCurSel();
  if (i>=0)
    {
    ParseLine(i);
    UpdateData(FALSE);
    SetFillColour(m_Col);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnAdd()
  {
  bChanged = true;
  int i = m_BarList.GetCurSel();
  if (i==LB_ERR && m_BarList.GetCount()>0)
    i = 0;
  CString Txt;
  if (i>=0)
    m_BarList.GetText(i, Txt);
  else
    {
    if (bForBars)
      Txt.Format("XXX\tYYY\t0\t100\t%d", DEFAULTBARSCOLOUR);
    else
      Txt = "XXX\tYYY";
    }
  i = m_BarList.AddString((const char*)Txt);
  m_BarList.SetCurSel(i);
  OnSelchangeBarlist();
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnKillfocus(UINT nID)
  {
  int i = m_BarList.GetCurSel();
  if (i>=0)
    {
    UpdateData(TRUE);
    Strng s;
    if (bForBars)
      s.Set("%s\t%s\t%g\t%g\t%d", (const char*)m_Tag, (const char*)m_Var, m_Min, m_Max, m_Col);
    else
      s.Set("%s\t%s", (const char*)m_Tag, (const char*)m_Var);
    CString Txt;
    m_BarList.GetText(i, Txt);
    if (Txt.GetLength()!=s.Length() || strcmp((const char*)Txt, s())!=0)
      {
      m_BarList.DeleteString(i);
      i = m_BarList.InsertString(i, s());
      m_BarList.SetCurSel(i);
      bChanged = true;
      }
    }
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnUpdateEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_BarList.GetCurSel()>=0);
  }

//---------------------------------------------------------------------------

void CUpdateBarDlg::OnChangeScale()
  {
  bChanged = true;
  }

//---------------------------------------------------------------------------
//===========================================================================

CUpdateAnnotDlg::CUpdateAnnotDlg(CWnd* pParent, Strng_List* TagReq, Strng_List* VarReq, Strng_List* FmtReq, double Scale)
: CDialog(CUpdateAnnotDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  //{{AFX_DATA_INIT(CUpdateAnnotDlg)
  m_Tag = _T("");
  m_Var = _T("");
  m_Fmt = _T("");
  m_Scale = Scale;
  //}}AFX_DATA_INIT
  pTagReq = TagReq;
  pVarReq = VarReq;
  pFmtReq = FmtReq;
  bChanged = 0;
  Create(CUpdateAnnotDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CUpdateAnnotDlg::~CUpdateAnnotDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUpdateAnnotDlg)
  DDX_Control(pDX, IDC_VARLIST, m_VarList);
  DDX_Control(pDX, IDC_TAGLIST, m_TagList);
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Text(pDX, IDC_VAR, m_Var);
  DDX_Text(pDX, IDC_FMT, m_Fmt);
  DDX_Text(pDX, IDC_SCALE, m_Scale);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CUpdateAnnotDlg, CDialog)
  //{{AFX_MSG_MAP(CUpdateAnnotDlg)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_LBN_SELCHANGE(IDC_TAGLIST, OnSelchangeTaglist)
  ON_LBN_SELCHANGE(IDC_VARLIST, OnSelchangeVarlist)
  ON_BN_CLICKED(IDC_ADD2, OnAdd2)
  ON_EN_CHANGE(IDC_SCALE, OnChangeScale)
  //}}AFX_MSG_MAP
  ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_TAG, IDC_TAG, OnKillfocus)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_TAG, IDC_TAG, OnUpdateEdit)
  ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_VAR, IDC_FMT, OnKillfocus2)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_VAR, IDC_FMT, OnUpdateEdit2)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CUpdateAnnotDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  VERIFY(m_TagList.SubclassMoveUpDownControls(this, IDC_TAGLIST, IDC_MOVEUP, IDC_MOVEDOWN));
  VERIFY(m_TagList.SubclassDeleteControl(this, IDC_TAGLIST, IDC_DELETE));
  VERIFY(m_VarList.SubclassMoveUpDownControls(this, IDC_VARLIST, IDC_MOVEUP2, IDC_MOVEDOWN2));
  VERIFY(m_VarList.SubclassDeleteControl(this, IDC_VARLIST, IDC_DELETE2));
  m_VarList.SetHeaderCount(2);
  m_VarList.SetHeaderItem(0, "Var", 100);
  m_VarList.SetHeaderItem(1, "Fmt", 80);
  m_VarList.MakeHeader(34761);
  m_VarList.ResetContent();
  pStrng a = pTagReq->First();
  while (a)
    {
    m_TagList.AddString(a->Str());
    a = pTagReq->Next();
    }
  Strng s;
  a = pVarReq->First();
  pStrng b = pFmtReq->First();
  while (a && b)
    {
    s.Set("%s\t%s", a->Str(), b->Str());
    m_VarList.AddString(s());
    a = pVarReq->Next();
    b = pFmtReq->Next();
    }
  m_TagList.SetCurSel(0);
  OnSelchangeTaglist();
  m_VarList.SetCurSel(0);
  OnSelchangeVarlist();
  ShowWindow(SW_SHOW);
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnAdd()
  {
  bChanged = 1;
  int i = m_TagList.GetCurSel();
  if (i==LB_ERR && m_TagList.GetCount()>0)
    i = 0;
  CString Txt;
  if (i>=0)
    m_TagList.GetText(i, Txt);
  else
    Txt = "XXX";
  i = m_TagList.AddString((const char*)Txt);
  m_TagList.SetCurSel(i);
  OnSelchangeTaglist();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnAdd2()
  {
  bChanged = 1;
  int i = m_VarList.GetCurSel();
  if (i==LB_ERR && m_VarList.GetCount()>0)
    i = 0;
  CString Txt;
  if (i>=0)
    m_VarList.GetText(i, Txt);
  else
    Txt = "YYY\t%.3g";
  i = m_VarList.AddString((const char*)Txt);
  m_VarList.SetCurSel(i);
  OnSelchangeVarlist();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnSelchangeTaglist()
  {
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    m_TagList.GetText(i, m_Tag);
    UpdateData(FALSE);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnSelchangeVarlist()
  {
  int i = m_VarList.GetCurSel();
  if (i>=0)
    {
    m_VarList.GetTextInColumn(0, i, m_Var);
    m_VarList.GetTextInColumn(1, i, m_Fmt);
    UpdateData(FALSE);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::UpdateLists()
  {
  UpdateData(TRUE);
  pTagReq->Clear();
  for (int i=0; i<m_TagList.GetCount(); i++)
    {
    CString Txt;
    m_TagList.GetText(i, Txt);
    pTagReq->Append((const char*)Txt);
    }
  pVarReq->Clear();
  pFmtReq->Clear();
  for (i=0; i<m_VarList.GetCount(); i++)
    {
    CString Txt;
    m_VarList.GetTextInColumn(0, i, Txt);
    pVarReq->Append((const char*)Txt);
    m_VarList.GetTextInColumn(1, i, Txt);
    pFmtReq->Append((const char*)Txt);
    }
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnKillfocus(UINT nID)
  {
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    UpdateData(TRUE);
    CString Txt;
    m_TagList.GetText(i, Txt);
    if (Txt.GetLength()!=m_Tag.GetLength() || strcmp((const char*)Txt, (const char*)m_Tag)!=0)
      {
      m_TagList.DeleteString(i);
      i = m_TagList.InsertString(i, (const char*)m_Tag);
      m_TagList.SetCurSel(i);
      bChanged = 1;
      }
    }
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnUpdateEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_TagList.GetCurSel()>=0);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnKillfocus2(UINT nID)
  {
  int i = m_VarList.GetCurSel();
  if (i>=0)
    {
    UpdateData(TRUE);
    Strng s;
    s.Set("%s\t%s", (const char*)m_Var, (const char*)m_Fmt);
    CString Txt;
    m_VarList.GetText(i, Txt);
    if (Txt.GetLength()!=s.Length() || strcmp((const char*)Txt, s())!=0)
      {
      m_VarList.DeleteString(i);
      i = m_VarList.InsertString(i, s());
      m_VarList.SetCurSel(i);
      bChanged = 1;
      }
    }
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnUpdateEdit2(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_VarList.GetCurSel()>=0);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg::OnChangeScale()
  {
  bChanged = 1;
  }

//===========================================================================
// CUpdateAnnotDlg1 dialog

CUpdateAnnotDlg1::CUpdateAnnotDlg1(CWnd* pParent, Strng_List* TagReq, Strng_List* TLbReq, Strng_List* VarReq, Strng_List* VLbReq, Strng_List* FmtReq, Strng_List* CvsReq, double Scale)
: CDialog(CUpdateAnnotDlg1::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  //{{AFX_DATA_INIT(CUpdateAnnotDlg1)
  m_Tag = _T("");
  m_TagLabel = _T("");
  m_Var = _T("");
  m_Scale = Scale;
  m_Cnv = _T("");
  m_VarLabel = _T("");
  m_Fmt = _T("");
  //}}AFX_DATA_INIT
  pTagReq = TagReq;
  pTLbReq = TLbReq;
  pVarReq = VarReq;
  pVLbReq = VLbReq;
  pFmtReq = FmtReq;
  pCvsReq = CvsReq;
  bChanged = 0;
  Create(CUpdateAnnotDlg1::IDD, pParent);
  }

//---------------------------------------------------------------------------

CUpdateAnnotDlg1::~CUpdateAnnotDlg1()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUpdateAnnotDlg1)
  DDX_Control(pDX, IDC_TAGLIST, m_TagList);
  DDX_Control(pDX, IDC_VARLIST, m_VarList);
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Text(pDX, IDC_TAGLABEL, m_TagLabel);
  DDX_Text(pDX, IDC_VAR, m_Var);
  DDX_Text(pDX, IDC_SCALE, m_Scale);
  DDX_Text(pDX, IDC_CNV, m_Cnv);
  DDX_Text(pDX, IDC_VARLABEL, m_VarLabel);
  DDX_CBString(pDX, IDC_FMT, m_Fmt);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CUpdateAnnotDlg1, CDialog)
  //{{AFX_MSG_MAP(CUpdateAnnotDlg1)
  ON_BN_CLICKED(IDC_ADD, OnAdd)
  ON_LBN_SELCHANGE(IDC_TAGLIST, OnSelchangeTaglist)
  ON_LBN_SELCHANGE(IDC_VARLIST, OnSelchangeVarlist)
  ON_BN_CLICKED(IDC_ADD2, OnAdd2)
  ON_EN_CHANGE(IDC_SCALE, OnChangeScale)
  ON_CBN_SELCHANGE(IDC_FMT, OnSelchangeFmt)
  //}}AFX_MSG_MAP
  ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_TAG, IDC_TAGLABEL, OnKillfocus)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_TAG, IDC_TAGLABEL, OnUpdateEdit)
  ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_VAR, IDC_VARLABEL, OnKillfocus2)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_VAR, IDC_VARLABEL, OnUpdateEdit2)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CUpdateAnnotDlg1::OnInitDialog()
  {
  CDialog::OnInitDialog();
  VERIFY(m_TagList.SubclassMoveUpDownControls(this, IDC_TAGLIST, IDC_MOVEUP, IDC_MOVEDOWN));
  VERIFY(m_TagList.SubclassDeleteControl(this, IDC_TAGLIST, IDC_DELETE));
  m_TagList.SetHeaderCount(2);
  m_TagList.SetHeaderItem(0, "Tag", 110);
  m_TagList.SetHeaderItem(1, "Label", 288);
  m_TagList.MakeHeader(34761);
  m_TagList.ResetContent();
  VERIFY(m_VarList.SubclassMoveUpDownControls(this, IDC_VARLIST, IDC_MOVEUP2, IDC_MOVEDOWN2));
  VERIFY(m_VarList.SubclassDeleteControl(this, IDC_VARLIST, IDC_DELETE2));
  m_VarList.SetHeaderCount(4);
  m_VarList.SetHeaderItem(0, "Variable", 110);
  m_VarList.SetHeaderItem(1, "Format", 70);
  m_VarList.SetHeaderItem(2, "Cnv", 62);
  m_VarList.SetHeaderItem(3, "Label", 153);
  m_VarList.MakeHeader(34762);
  m_VarList.ResetContent();
  Strng s;
  pStrng a = pTagReq->First();
  pStrng b = pTLbReq->First();
  while (a && b)
    {
    s.Set("%s\t%s", a->Str(), b->Str());
    m_TagList.AddString(s());
    a = pTagReq->Next();
    b = pTLbReq->Next();
    }
  a = pVarReq->First();
  b = pVLbReq->First();
  pStrng c = pFmtReq->First();
  pStrng d = pCvsReq->First();
  while (a && b && c && d)
    {
    s.Set("%s\t%s\t%s\t%s", a->Str(), c->Str(), d->Str(), b->Str());
    m_VarList.AddString(s());
    a = pVarReq->Next();
    b = pVLbReq->Next();
    c = pFmtReq->Next();
    d = pCvsReq->Next();
    }
  CComboBox* w = (CComboBox*)GetDlgItem(IDC_FMT);
  //ASSERT(w->IsKindOf(RUNTIME_CLASS(CComboBox)));
  for (int i=0; i<DefinedFmts.GetSize(); i++)
    w->AddString(DefinedFmts[i].sNm());
  m_TagList.SetCurSel(0);
  OnSelchangeTaglist();
  m_VarList.SetCurSel(0);
  OnSelchangeVarlist();
  ShowWindow(SW_SHOW);
  return TRUE;  // return TRUE unless you set the focus to a control
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnAdd()
  {
  bChanged = 1;
  int i = m_TagList.GetCurSel();
  if (i==LB_ERR && m_TagList.GetCount()>0)
    i = 0;
  CString Txt;
  if (i>=0)
    m_TagList.GetText(i, Txt);
  else
    Txt = "XXX\tXXX";
  i = m_TagList.AddString((const char*)Txt);
  m_TagList.SetCurSel(i);
  OnSelchangeTaglist();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnAdd2()
  {
  bChanged = 1;
  int i = m_VarList.GetCurSel();
  if (i==LB_ERR && m_VarList.GetCount()>0)
    i = 0;
  CString Txt;
  if (i>=0)
    m_VarList.GetText(i, Txt);
  else
    Txt = "YYY\t*.##\t*\tYYY";
  i = m_VarList.AddString((const char*)Txt);
  m_VarList.SetCurSel(i);
  OnSelchangeVarlist();
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnSelchangeTaglist()
  {
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    m_TagList.GetTextInColumn(0, i, m_Tag);
    m_TagList.GetTextInColumn(1, i, m_TagLabel);
    UpdateData(FALSE);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnSelchangeVarlist()
  {
  int i = m_VarList.GetCurSel();
  if (i>=0)
    {
    m_VarList.GetTextInColumn(0, i, m_Var);
    m_VarList.GetTextInColumn(1, i, m_Fmt);
    m_VarList.GetTextInColumn(2, i, m_Cnv);
    m_VarList.GetTextInColumn(3, i, m_VarLabel);
    CComboBox* w = (CComboBox*)GetDlgItem(IDC_FMT);
    i = w->FindStringExact(-1, (const char*)m_Fmt);
    if (i<0)
      {
      m_Fmt = "*.##";
      i = w->FindStringExact(-1, (const char*)m_Fmt);
      }
    w->SetCurSel(i);
    UpdateData(FALSE);
    }
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::UpdateLists()
  {
  UpdateData(TRUE);
  CString Txt;
  pTagReq->Clear();
  pTLbReq->Clear();
  for (int i=0; i<m_TagList.GetCount(); i++)
    {
    m_TagList.GetTextInColumn(0, i, Txt);
    pTagReq->Append((const char*)Txt);
    m_TagList.GetTextInColumn(1, i, Txt);
    pTLbReq->Append((const char*)Txt);
    }
  pVarReq->Clear();
  pVLbReq->Clear();
  pFmtReq->Clear();
  pCvsReq->Clear();
  for (i=0; i<m_VarList.GetCount(); i++)
    {
    m_VarList.GetTextInColumn(0, i, Txt);
    pVarReq->Append((const char*)Txt);
    m_VarList.GetTextInColumn(1, i, Txt);
    pFmtReq->Append((const char*)Txt);
    m_VarList.GetTextInColumn(2, i, Txt);
    if (Txt.GetLength()==0)
      Txt = "*";
    pCvsReq->Append((const char*)Txt);
    m_VarList.GetTextInColumn(3, i, Txt);
    pVLbReq->Append((const char*)Txt);
    }
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnKillfocus(UINT nID)
  {
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    UpdateData(TRUE);
    BOOL DoUpdate = FALSE;
    char* p = m_TagLabel.GetBuffer(0);
    char* pp = strchr(p, ' ');
    while (pp)
      {
      DoUpdate = TRUE;
      pp[0] = '_';
      pp = strchr(p, ' ');
      }
    m_TagLabel.ReleaseBuffer();
    Strng s;
    s.Set("%s\t%s", (const char*)m_Tag, (const char*)m_TagLabel);
    CString Txt;
    m_TagList.GetText(i, Txt);
    if (Txt.GetLength()!=s.Len() || strcmp((const char*)Txt, s())!=0)
      {
      m_TagList.DeleteString(i);
      i = m_TagList.InsertString(i, s());
      m_TagList.SetCurSel(i);
      DoUpdate = FALSE;
      UpdateData(FALSE);
      bChanged = 1;
      }
    if (DoUpdate)
      UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnUpdateEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_TagList.GetCurSel()>=0);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnKillfocus2(UINT nID)
  {
  int i = m_VarList.GetCurSel();
  if (i>=0)
    {
    UpdateData(TRUE);
    BOOL DoUpdate = FALSE;
    char* p = m_VarLabel.GetBuffer(0);
    char* pp = strchr(p, ' ');
    while (pp)
      {
      DoUpdate = TRUE;
      pp[0] = '_';
      pp = strchr(p, ' ');
      }
    m_VarLabel.ReleaseBuffer();
    if (m_Cnv.GetLength()==0)
      {
      m_Cnv = "*";
      DoUpdate = TRUE;
      }
    Strng s;
    s.Set("%s\t%s\t%s\t%s", (const char*)m_Var, (const char*)m_Fmt, (const char*)m_Cnv, (const char*)m_VarLabel);
    CString Txt;
    m_VarList.GetText(i, Txt);
    if (Txt.GetLength()!=s.Len() || strcmp((const char*)Txt, s())!=0)
      {
      m_VarList.DeleteString(i);
      i = m_VarList.InsertString(i, s());
      m_VarList.SetCurSel(i);
      DoUpdate = FALSE;
      UpdateData(FALSE);
      bChanged = 1;
      }
    if (DoUpdate)
      UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnUpdateEdit2(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_VarList.GetCurSel()>=0);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnSelchangeFmt()
  {
  CComboBox* w = (CComboBox*)GetDlgItem(IDC_FMT);
  int i = w->GetCurSel();
  if (i>=0)
    {
    w->GetLBText(i, m_Fmt);
    UpdateData(FALSE);
    }
  OnKillfocus2(IDC_FMT);
  }

//---------------------------------------------------------------------------

void CUpdateAnnotDlg1::OnChangeScale()
  {
  bChanged = 1;
  }

//===========================================================================
// CLayersDlg dialog
CLayersDlg::CLayersDlg(GrfCmdBlk* GrfCmdBlk, CWnd* pParent)
: CDialog(CLayersDlg::IDD, pParent)
  {
  gs_pCmd->SetDlgBusy();
  pGrfCmdBlk = GrfCmdBlk;
  //{{AFX_DATA_INIT(CLayersDlg)
  //}}AFX_DATA_INIT
  Create(CLayersDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CLayersDlg::~CLayersDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CLayersDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLayersDlg)
  DDX_Control(pDX, IDC_LAYERLIST, m_LayList);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLayersDlg, CDialog)
  //{{AFX_MSG_MAP(CLayersDlg)
  ON_BN_CLICKED(IDC_LAYOFF, OnLayoff)
  ON_BN_CLICKED(IDC_LAYON, OnLayon)
  ON_BN_CLICKED(IDC_LCOLOR, OnLcolor)
  ON_NOTIFY(NM_CLICK, IDC_LAYERLIST, OnClickLayerlist)
  //}}AFX_MSG_MAP
  //ON_UPDATE_COMMAND_UI_RANGE(IDC_LAYON, IDC_LCOLOR, OnUpdateBtns)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_LAYON, IDC_LAYOFF, OnUpdateBtns)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CLayersDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  m_LayList.InsertColumn(0, "Layer", LVCFMT_LEFT, 116);
  m_LayList.InsertColumn(1, "Frozen", LVCFMT_CENTER, 48);
  m_LayList.InsertColumn(2, "Colour", LVCFMT_LEFT, 64);
  m_LayList.InsertColumn(3, "Used", LVCFMT_CENTER, 40);
  m_LayList.InsertColumn(4, "View", LVCFMT_CENTER, 36);
  m_LayList.InsertColumn(5, "On", LVCFMT_CENTER, 36);
  m_LayList.InsertColumn(6, "LineType", LVCFMT_LEFT, 100);
  Refresh();
  CenterWindow();
  ShowWindow(SW_SHOW);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CLayersDlg::Refresh()
  {
  m_LayList.DeleteAllItems();
  LVITEM Item;
  Item.mask = LVIF_TEXT;
  Item.state = 0;
  Item.stateMask = 0;
  Item.cchTextMax = 0;
  Item.iImage = 0;
  char Buff[256];
  DML_LIST llist = pGrfCmdBlk->pDrw->LayerLst;
  DML_ITEM it;
  DML_WALK_LIST(llist, it)
    {
    int k = 1;
    int j = m_LayList.InsertItem(0, (LPCTSTR)DXF_LAYER_NAME_GET((DXF_TABLE_ENTRY) DML_RECORD(it)));
    int Flags = (int)DXF_LAYER_FLAGS((DXF_TABLE_ENTRY) DML_RECORD(it));
    int Col = (int)DXF_LAYER_COLOR((DXF_TABLE_ENTRY) DML_RECORD(it));
    Item.iItem = j;
    Item.iSubItem = k++;
    Item.pszText = ((Flags & 64) ? "Yes" : "No ");
    m_LayList.SetItem(&Item);
    Item.iSubItem = k++;
    sprintf(Buff, "Colour_%02d", Col);
    Item.pszText = Buff;
    m_LayList.SetItem(&Item);
    Item.iSubItem = k++;
    Item.pszText = ((Flags & 4) ? "Yes" : "No ");
    m_LayList.SetItem(&Item);
    Item.iSubItem = k++;
    Item.pszText = ((Flags & 1) ? "Yes" : "No ");
    m_LayList.SetItem(&Item);
    Item.iSubItem = k++;
    Item.pszText = ((Col>0) ? "Yes" : "No ");
    m_LayList.SetItem(&Item);
    Item.iSubItem = k++;
    Item.pszText = (char*)DXF_LAYER_LTYPE_GET((DXF_TABLE_ENTRY) DML_RECORD(it));
    m_LayList.SetItem(&Item);
    }
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CLayersDlg::SetLayer(BOOL Show)
  {
  int Sel[128];
  int j = 0;
  for (int i=0; i<m_LayList.GetItemCount(); i++)
    {
    if (m_LayList.GetItemState(i, LVIS_SELECTED))
      {
      Sel[j++] = i;
      CString s = m_LayList.GetItemText(i, 0);
      char Buff[256];
      sprintf(Buff, " layer_name %s %s ", (const char*)s, (Show ? "show" : "hide"));
      gs_pCmd->ExtendCmdLine(Buff);
      }
    }
  if (j>0)
    {
    pGrfCmdBlk->pWnd->Invalidate();
    Refresh();
    }
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnLayon()
  {
  SetLayer(TRUE);
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnLayoff()
  {
  SetLayer(FALSE);
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnLcolor()
  {
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnUpdateBtns(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_LayList.GetSelectedCount()>0);
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnClickLayerlist(NMHDR* pNMHDR, LRESULT* pResult)
  {
  UpdateDialogControls(this, FALSE);
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CLayersDlg::OnOK()
  {
  gs_pCmd->ExtendCmdLine("\r");
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------
//===========================================================================

double CUpdateTagsDlg::PrevSize = 3.0;
double CUpdateTagsDlg::PrevRotation = 0.0;
BOOL CUpdateTagsDlg::PrevDoSize = FALSE;
BOOL CUpdateTagsDlg::PrevDoRotation = FALSE;
BOOL CUpdateTagsDlg::PrevDoPosition = FALSE;
int CUpdateTagsDlg::PrevShowHide = 0;
BOOL CUpdateTagsDlg::PrevPosOK = FALSE;
int CUpdateTagsDlg::PrevXPos = 0;
int CUpdateTagsDlg::PrevYPos = 0;
int WinFullHeightUpdateTags = 176;
int WinHalfHeightUpdateTags = 72;

//---------------------------------------------------------------------------
// CUpdateTagsDlg dialog
CUpdateTagsDlg::CUpdateTagsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CUpdateTagsDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CUpdateTagsDlg)
  m_Size = PrevSize;
  m_Rotation = PrevRotation;
  m_DoSize = PrevDoSize;
  m_DoRotation = PrevDoRotation;
  m_DoPosition = PrevDoPosition;
  m_ShowHide = PrevShowHide;
  //}}AFX_DATA_INIT
  gs_pCmd->SetDlgBusy();
  Create(CUpdateTagsDlg::IDD, pParent);
  if (PrevDoPosition)
    gs_pCmd->ExtendCmdLine("position ");
  }

//---------------------------------------------------------------------------

CUpdateTagsDlg::~CUpdateTagsDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CUpdateTagsDlg)
  DDX_Text(pDX, IDC_ANNOTSIZE, m_Size);
  DDX_Text(pDX, IDC_ROTATION, m_Rotation);
  DDX_Check(pDX, IDC_DOSIZE, m_DoSize);
  DDX_Check(pDX, IDC_DOROTATION, m_DoRotation);
  DDX_Check(pDX, IDC_DOPOSITION, m_DoPosition);
  DDX_Radio(pDX, IDC_SHOW, m_ShowHide);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CUpdateTagsDlg, CDialog)
  //{{AFX_MSG_MAP(CUpdateTagsDlg)
  ON_BN_CLICKED(IDC_SHOWALLTAGS, OnShowall)
  ON_BN_CLICKED(IDC_HIDEALLTAGS, OnHideall)
  ON_BN_CLICKED(IDC_APPLY, OnApply)
  ON_BN_CLICKED(IDC_DOPOSITION, OnDoposition)
  ON_BN_CLICKED(IDC_DOSIZE, OnDosize)
  ON_BN_CLICKED(IDC_DOROTATION, OnDorotation)
  ON_BN_CLICKED(IDC_IWIN, OnIwin)
  ON_BN_CLICKED(IDC_CWIN, OnCwin)
  ON_BN_CLICKED(IDC_NEXT, OnNext)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
  //ON_UPDATE_COMMAND_UI_RANGE(IDC_?, IDC_?, OnUpdateEdit)
  ON_UPDATE_COMMAND_UI(IDC_DOSIZE, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_DOROTATION, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_NEXT, OnUpdateNext)
  ON_UPDATE_COMMAND_UI(IDC_APPLY, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_SHOW, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_HIDE, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_IWIN, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_CWIN, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(IDC_ANNOTSIZE, OnUpdateSize)
  ON_UPDATE_COMMAND_UI(IDC_ROTATION, OnUpdateRotation)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CUpdateTagsDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  SetDescription("");
  IWinBitmap.LoadMappedBitmap(IDB_BTN_IWIN);
  CWinBitmap.LoadMappedBitmap(IDB_BTN_CWIN);
  ((CButton*)GetDlgItem(IDC_IWIN))->SetBitmap((HBITMAP)IWinBitmap);
  ((CButton*)GetDlgItem(IDC_CWIN))->SetBitmap((HBITMAP)CWinBitmap);

  if (PrevPosOK)
    SetVisibleWindowPos(this, PrevXPos, PrevYPos);
  else
    ShowWindow(SW_SHOW);
  CRect WinRect;
  GetWindowRect(&WinRect);
  const int DlgTop = WinRect.top;
  WinFullHeightUpdateTags = WinRect.bottom - DlgTop;
  GetDlgItem(IDC_STATIC1)->GetWindowRect(&WinRect);
  WinHalfHeightUpdateTags = WinRect.bottom - DlgTop + 3;
  UpdateDialogControls(this, FALSE);
  DoShowMore();
  return TRUE;
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnDestroy()
  {
  PrevSize = m_Size;
  PrevRotation = m_Rotation;
  PrevDoSize = m_DoSize;
  PrevDoRotation = m_DoRotation;
  PrevDoPosition = m_DoPosition;
  PrevShowHide = m_ShowHide;
  RECT Rect;
  GetWindowRect(&Rect);
  PrevXPos = Rect.left;
  PrevYPos = Rect.top;
  PrevPosOK = TRUE;
  CDialog::OnDestroy();
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::DoShowMore()//flag First/*=False*/)
  {
  CRect WinRect;
  GetWindowRect(&WinRect);
  WinRect.bottom = WinRect.top + (m_DoPosition ? WinHalfHeightUpdateTags : WinFullHeightUpdateTags);
  MoveWindow(&WinRect, TRUE);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::SetDoPosition(BOOL DoPosition)
  {
  m_DoPosition = DoPosition;
  UpdateData(FALSE);
  DoShowMore();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::SetDescription(char* pTxt)
  {
  SetDlgItemText(IDC_STATIC1, pTxt);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnOK()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  gs_pCmd->SetDlgBusy(0);//do this just in case the above command fails
  //CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  gs_pCmd->SetDlgBusy(0);//do this just in case the above command fails
  //CDialog::OnCancel();
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnShowall()
  {
  m_DoPosition = 0;
  gs_pCmd->ExtendCmdLine("noposition nosize norotation show all ; ");
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnHideall()
  {
  m_DoPosition = 0;
  gs_pCmd->ExtendCmdLine("noposition nosize norotation hide all ; ");
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnApply()
  {
  UpdateData(TRUE);
  m_Size = Max(0.001, m_Size);
  m_Rotation = Range(-360.0, m_Rotation, 360.0);
  Strng s;
  s.Set("%s ", m_ShowHide==0 ? "show" : "hide");
  if (m_DoSize)
    s.Append("size %g ", m_Size);
  else
    s.Append("nosize ");
  if (m_DoRotation)
    s.Append("rotation %g ; ", m_Rotation);
  else
    s.Append("norotation ; ");
  gs_pCmd->ExtendCmdLine(s());
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  //gs_pCmd->ExtendCmdLine("\r");
  //PostMessage(WM_COMMAND, IDOK);
  //pGrfCmdBlk->pWnd->PostMessage(WM_COMMAND, ID_GRF_TagAnnot);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnDoposition()
  {
  UpdateData(TRUE);
  Strng s;
  s.Set("%s ", m_DoPosition==0 ? "noposition " : "position ");
  gs_pCmd->ExtendCmdLine(s());
  DoShowMore();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnUpdatePosition(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!m_DoPosition);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnNext()
  {
  gs_pCmd->ExtendCmdLine("next ");
  }

void CUpdateTagsDlg::OnUpdateNext(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_DoPosition);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnDosize()
  {
  UpdateData(TRUE);
  UpdateDialogControls(this, FALSE);
  }

void CUpdateTagsDlg::OnUpdateSize(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!m_DoPosition && m_DoSize);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnDorotation()
  {
  UpdateData(TRUE);
  UpdateDialogControls(this, FALSE);
  }

void CUpdateTagsDlg::OnUpdateRotation(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!m_DoPosition && m_DoRotation);
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnIwin()
  {
  gs_pCmd->ExtendCmdLine("iwin ");
  }

//---------------------------------------------------------------------------

void CUpdateTagsDlg::OnCwin()
  {
  gs_pCmd->ExtendCmdLine("cwin ");
  }

//---------------------------------------------------------------------------
//===========================================================================

BOOL CScaleRotateDlg::PrevPosOK = FALSE;
int CScaleRotateDlg::PrevXPos = 0;
int CScaleRotateDlg::PrevYPos = 0;

//---------------------------------------------------------------------------

CScaleRotateDlg::CScaleRotateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CScaleRotateDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CScaleRotateDlg)
  //}}AFX_DATA_INIT
  Create(CScaleRotateDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::My_DDX_Text(CDataExchange* pDX, int nIDC, double& value, double Default)
  {
  if (pDX->m_bSaveAndValidate)
    {
    CString s;
    GetDlgItemText(nIDC, s);
    value = SafeAtoF((const char *)s, Default);
    }
  else
    {
    DDX_Text(pDX, nIDC, value);
    }
  }

void CScaleRotateDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScaleRotateDlg)
  My_DDX_Text(pDX, IDC_XPOS, Data.XPos, 0.0);
  My_DDX_Text(pDX, IDC_YPOS, Data.YPos, 0.0);
  My_DDX_Text(pDX, IDC_XSCALE, Data.XScale, 1.0);
  My_DDX_Text(pDX, IDC_YSCALE, Data.YScale, 1.0);
  My_DDX_Text(pDX, IDC_ROTATIONGRF, Data.Rotation, 0.0);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CScaleRotateDlg, CDialog)
  //{{AFX_MSG_MAP(CScaleRotateDlg)
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDC_APPLY, OnApply)
  ON_BN_CLICKED(IDC_RESTORE, OnRestore)
  ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
  //}}AFX_MSG_MAP
  ON_CONTROL_RANGE(EN_CHANGE, IDC_XPOS, IDC_ROTATIONGRF, OnChangeEdit)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_XPOS, IDC_ROTATIONGRF, OnUpdateEdit)
  ON_UPDATE_COMMAND_UI(IDC_APPLY, OnUpdateApply)
  ON_UPDATE_COMMAND_UI(IDC_ACCEPT, OnUpdateRestore)
  ON_UPDATE_COMMAND_UI(IDC_RESTORE, OnUpdateAccept)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CScaleRotateDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  m_Changed = FALSE;
  m_AppChanged = FALSE;
  m_ValidGrf = FALSE;
  SetDlgItemText(IDC_PREVXPOS, "");
  SetDlgItemText(IDC_PREVYPOS, "");
  SetDlgItemText(IDC_PREVXSCALE, "");
  SetDlgItemText(IDC_PREVYSCALE, "");
  SetDlgItemText(IDC_PREVROTATION, "");
  if (PrevPosOK)
    SetVisibleWindowPos(this, PrevXPos, PrevYPos);
  else
    ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::OnDestroy()
  {
  RECT Rect;
  GetWindowRect(&Rect);
  PrevXPos = Rect.left;
  PrevYPos = Rect.top;
  PrevPosOK = TRUE;
  CDialog::OnDestroy();
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::SetParms(double XPos, double YPos, double XScale, double YScale, double Rotation)
  {
  Data.XPos = XPos;
  Data.YPos = YPos;
  Data.XScale = XScale;
  Data.YScale = YScale;
  Data.Rotation = Rotation;
  PrevData = Data;
  LastData = Data;
  SetDlgItemText(IDC_PREVXPOS, "-");
  SetDlgItemText(IDC_PREVYPOS, "-");
  SetDlgItemText(IDC_PREVXSCALE, "-");
  SetDlgItemText(IDC_PREVYSCALE, "-");
  SetDlgItemText(IDC_PREVROTATION, "-");
  m_Changed = FALSE;
  m_ValidGrf = TRUE;
  m_AppChanged = FALSE;
  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::UpdateParms(double XPos, double YPos, double XScale, double YScale, double Rotation)
  {
  Data.XPos = XPos;
  Data.YPos = YPos;
  Data.XScale = XScale;
  Data.YScale = YScale;
  Data.Rotation = Rotation;
  LastData = Data;
  m_ValidGrf = TRUE;
  UpdateData(FALSE);
  TestChanged();
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::SetGroup(double XScale, double YScale, double Rotation)
  {
  m_ValidGrf = FALSE;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::OnChangeEdit(UINT nID)
  {
  // TODO: If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function to send the EM_SETEVENTMASK message to the control
  // with the ENM_CHANGE flag ORed into the lParam mask.

  UpdateData(TRUE);
  TestChanged();
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::OnUpdateEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_ValidGrf);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::TestChanged()
  {
  m_Changed = (Data!=PrevData);
  m_AppChanged = (Data!=LastData);
  //.XScale!=PrevData.XScale || Data.YScale!=PrevData.YScale || Data.Rotation!=PrevData.Rotation ||
  //XPos!=m_PrevXPos || m_YPos!=m_PrevYPos);
  char Buff[256];
  sprintf(Buff, "%.3f", PrevData.XPos);
  SetDlgItemText(IDC_PREVXPOS, Data.XPos!=PrevData.XPos ? Buff : "-");
  sprintf(Buff, "%.3f", PrevData.YPos);
  SetDlgItemText(IDC_PREVYPOS, Data.YPos!=PrevData.YPos ? Buff : "-");
  sprintf(Buff, "%.3f", PrevData.XScale);
  SetDlgItemText(IDC_PREVXSCALE, Data.XScale!=PrevData.XScale ? Buff : "-");
  sprintf(Buff, "%.3f", PrevData.YScale);
  SetDlgItemText(IDC_PREVYSCALE, Data.YScale!=PrevData.YScale ? Buff : "-");
  sprintf(Buff, "%.3f", PrevData.Rotation);
  SetDlgItemText(IDC_PREVROTATION, Data.Rotation!=PrevData.Rotation ? Buff : "-");
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::OnRestore()
  {
  SetParms(PrevData.XPos, PrevData.YPos, PrevData.XScale, PrevData.YScale, PrevData.Rotation);
  gs_pCmd->ExtendCmdLine("d1 ");
  }

void CScaleRotateDlg::OnUpdateRestore(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_Changed && m_ValidGrf);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::OnApply()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("d1 ");
  LastData = Data;
  TestChanged();
  }

void CScaleRotateDlg::OnUpdateApply(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_AppChanged && m_ValidGrf);
  }

//---------------------------------------------------------------------------

void CScaleRotateDlg::OnAccept()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("d1 ");
  SetParms(Data.XPos, Data.YPos, Data.XScale, Data.YScale, Data.Rotation);
  }

void CScaleRotateDlg::OnUpdateAccept(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_Changed && m_ValidGrf);
  }

//---------------------------------------------------------------------------
//===========================================================================

IMPLEMENT_DYNAMIC(CGrfTextDlg, CDialog)

CGrfTextDlg::CGrfTextDlg(CWnd* pParent /*=NULL*/) \
	: CDialog(CGrfTextDlg::IDD, pParent)
  , m_sText(_T(""))
  , m_TextScale(2.0)
  , m_Rotation(0.0)
  , m_TextWidth(0.8)
  {
  gs_pCmd->SetDlgBusy();
  Create(CGrfTextDlg::IDD, pParent);
  }

//---------------------------------------------------------------------------

CGrfTextDlg::~CGrfTextDlg()
  {
  gs_pCmd->SetDlgBusy(0);
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_TEXTEDIT, m_sText);
  DDX_Text(pDX, IDC_TEXTSCALE, m_TextScale);
  DDX_Text(pDX, IDC_TEXTROTATION, m_Rotation);
  DDX_Text(pDX, IDC_TEXTWIDTH, m_TextWidth);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGrfTextDlg, CDialog)
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(ID_SELECTNEXT, OnBnClickedSelectNext)
  ON_BN_CLICKED(ID_CREATENEXT, OnBnClickedCreateNext)
  ON_BN_CLICKED(ID_APPLY, OnBnClickedApply)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CGrfTextDlg::OnInitDialog()
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  m_TextScale=PF.RdDouble("InsertText", "Size", m_TextScale);
  m_Rotation=PF.RdDouble("InsertText", "Rotation", m_Rotation);
  m_TextWidth=PF.RdDouble("InsertText", "Width", m_TextWidth);
  UpdateData(FALSE);

  int Left=PF.RdInt("InsertText", "$Window.Left", -3000);
  int Top=PF.RdInt("InsertText", "$Window.Top", -3000);
  if (Left>-3000)
    SetVisibleWindowPos(this, Left, Top);
  //SetWindowPos(NULL, Left, Top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
  else
    CenterWindow();

  ShowWindow(SW_SHOW);
  UpdateDialogControls(this, FALSE);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::OnDestroy()
  {
  CProfINIFile PF(PrjIniFile());
  PF.WrDouble("InsertText", "Size", m_TextScale);
  PF.WrDouble("InsertText", "Rotation", m_Rotation);
  PF.WrDouble("InsertText", "Width", m_TextWidth);

  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (GetWindowPlacement(&wp))
    {
    PF.WrInt("InsertText", "$Window.Left", wp.rcNormalPosition.left);
    PF.WrInt("InsertText", "$Window.Top", wp.rcNormalPosition.top);
    }
  CDialog::OnDestroy();
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::SetCreateMode(bool CreateMode)
  {
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::SetTextSelected(bool ValidText)
  {
  GetDlgItem(IDC_TEXTTXT)->SetWindowText(ValidText ? "Change Text" : "New Text");
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::SetData(double Size, double Width, double Rotation)
  {
  UpdateData(TRUE);
  m_TextScale = Size;
  m_TextWidth = Width;
  m_Rotation = Rotation;
  bChanged = false;
  UpdateData(FALSE);
  }

void CGrfTextDlg::SetScale(double Size)
  {
  UpdateData(TRUE);
  m_TextScale = Size;
  UpdateData(FALSE);
  }

void CGrfTextDlg::SetRotation(double Rotation)
  {
  UpdateData(TRUE);
  m_Rotation = Rotation;
  UpdateData(FALSE);
  }

void CGrfTextDlg::SetText(char* Txt)
  {
  UpdateData(TRUE);
  m_sText = Txt;
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::CompleteForUse()
  {
  UpdateData(TRUE);
  m_TextScale = Max(0.001, m_TextScale);
  m_TextWidth = Range(0.01, m_TextWidth, 100.0);
  m_Rotation = Range(-360.0, m_Rotation, 360.0);
  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::OnBnClickedOk()
  {
  UpdateData(TRUE);
  gs_pCmd->ExtendCmdLine("\r");
  //OnOK();
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::OnBnClickedCancel()
  {
  gs_pCmd->ExtendCmdLine("\x1b");
  //OnCancel();
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::OnBnClickedSelectNext()
  {
  gs_pCmd->ExtendCmdLine("; update ");
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::OnBnClickedCreateNext()
  {
  gs_pCmd->ExtendCmdLine("; create ");
  }

//---------------------------------------------------------------------------

void CGrfTextDlg::OnBnClickedApply()
  {
  gs_pCmd->ExtendCmdLine("Apply ");
  }

//---------------------------------------------------------------------------
//===========================================================================



