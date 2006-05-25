//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "tagvdoc.h"
#include "qrydlg.h"
//#include "scd_wm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

CQueryDlg::CQueryDlg(CWnd* pParent,
                     pCTagVwDoc Doc,
                     CTimeValue StartTime,
                     CTimeValue EndTime)
  : CDialog(CQueryDlg::IDD, pParent)
  {
  pTTC = NULL;
  //{{AFX_DATA_INIT(CQueryDlg)
  m_Filename = _T("");
  m_StartTime = _T("");
  m_EndTime = _T("");
  m_NoPts = 61;
  m_Tag = _T("");
  m_Option = -1;
  m_Headings = FALSE;
	m_TimeOptFull = FALSE;
	m_TimeOptUnits = 0;
	//}}AFX_DATA_INIT
  pDoc = Doc;
  m_StartTime = StartTime.Format(TD_TimeDate);
  m_EndTime = EndTime.Format(TD_TimeDate);
  Create(CQueryDlg::IDD, pParent);
  }

//--------------------------------------------------------------------------

CQueryDlg::~CQueryDlg()
  {
  delete pTTC;
  }

//---------------------------------------------------------------------------

void CQueryDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CQueryDlg)
  DDX_Control(pDX, IDC_TAGLIST, m_TagList);
  DDX_Text(pDX, IDC_FILENAME, m_Filename);
  DDX_Text(pDX, IDC_STARTTIME, m_StartTime);
  DDX_Text(pDX, IDC_ENDTIME, m_EndTime);
  DDX_Text(pDX, IDC_NOPTS, m_NoPts);
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Radio(pDX, IDC_OPTION, m_Option);
  DDX_Check(pDX, IDC_HEADINGS, m_Headings);
	DDX_Check(pDX, IDC_QTIMEOPTFULL, m_TimeOptFull);
	DDX_Radio(pDX, IDC_QTIME, m_TimeOptUnits);
	//}}AFX_DATA_MAP
  GetDlgItem(IDC_TAG)->EnableWindow(m_TagList.GetCount()>0);
  GetDlgItem(IDC_NOPTS)->EnableWindow(m_Option==1);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CQueryDlg, CDialog)
  //{{AFX_MSG_MAP(CQueryDlg)
  ON_BN_CLICKED(IDC_ADDTAG, OnAddtag)
  ON_BN_CLICKED(IDC_REMOVETAG, OnRemovetag)
  ON_LBN_SELCHANGE(IDC_TAGLIST, OnSelchangeTaglist)
  ON_EN_CHANGE(IDC_TAG, OnChangeTag)
  ON_BN_CLICKED(IDC_OPTION, OnOption)
  ON_BN_CLICKED(IDC_OPTION1, OnOption1)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CQueryDlg::OnInitDialog() 
  {
  pTTC = new CCustomToolTipCtrl(this);
  CProfINIFile PF(PrjIniFile());
  Strng s(PrjFilesAlias());
  s += "query.csv"; 
  s = PF.RdStr("Trends", "QryFilename", s());
  s.FnExpand();
  m_Filename = s();
  //m_DBFilename = PF.RdStr("Trends", "QryDBFilename", "c:\\query.mdb")();
  //m_DBTableName = PF.RdStr("Trends", "QryDBTablename", "HistData")();
  m_Option = PF.RdInt("Trends", "QryOption", 0);
  m_TimeOptUnits = PF.RdInt("Trends", "QryTimeOptUnits", 1);
  m_TimeOptFull = (PF.RdInt("Trends", "QryTimeOptFull", 1)!=0);
  m_Headings = (PF.RdInt("Trends", "QryHeadings", 1)!=0);
  m_NoPts = PF.RdLong("Trends", "QryNoPoints", 61);
  CDialog::OnInitDialog();

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

BOOL CQueryDlg::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC && HelpMngr.ShowToolTips())
    pTTC->RelayEvent(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CQueryDlg::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC && HelpMngr.ShowToolTips())
    return pTTC->OnToolTipNotify(pNMHDR, CQueryDlg::IDD);
  return FALSE;
  }

//---------------------------------------------------------------------------

void CQueryDlg::AddTag(char* Tag)
  {
  m_TagList.AddString(Tag);
  }

//---------------------------------------------------------------------------

void CQueryDlg::PostNcDestroy()
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

void CQueryDlg::SaveOptions()
  {
  CProfINIFile PF(PrjIniFile());
  Strng s((const char*)m_Filename);
  s.FnContract();
  PF.WrStr("Trends", "QryFilename", s());
  //PF.WrStr("Trends", "QryDBFilename", (char*)(const char*)m_DBFilename);
  //PF.WrStr("Trends", "QryDBTablename", (char*)(const char*)m_DBTableName);
  PF.WrInt("Trends", "QryOption", m_Option);
  PF.WrInt("Trends", "QryTimeOptUnits", m_TimeOptUnits);
  PF.WrInt("Trends", "QryTimeOptFull", m_TimeOptFull);
  PF.WrInt("Trends", "QryHeadings", m_Headings);
  PF.WrLong("Trends", "QryNoPoints", m_NoPts);
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnOK()
  {
  UpdateData(True);
  SaveOptions();
  CTimeValue StartTime; 
  CTimeValue LastTime;
  flag Ok = StartTime.Parse(m_StartTime);
  Ok = LastTime.Parse(m_EndTime);

  short TagCnt = m_TagList.GetCount();

  CXM_Route HRoute;
  flag GotHistorian = pDoc->XFindObject(pExecName_Historian, HRoute);
  if (GotHistorian && TagCnt>0)
    {
    //if querytype=mdb then ... else ...
//TODO Implement option to retrieve historian trend data to MDB
    if (pDoc->StartFile((char*)(const char*)m_Filename))
      {
      //HRoute.dbgDump("Historian Route ");
      CXM_QueryHistoryOther *xb=new CXM_QueryHistoryOther (StartTime.Seconds, LastTime.Seconds, (long)pDoc, (byte)m_Option, (byte)m_TimeOptUnits, m_TimeOptFull, m_Headings, m_NoPts, dNAN, 1/*File*/, NULL, NULL, 0, 0.0);
      CXMsgLst XM;
      for (int i=0; i<TagCnt; i++)
        {
        CString Txt;
        m_TagList.GetText(i, Txt);
        if (!xb->xAddTag((char*)(const char*)Txt))
          {//???
          XM.PackMsg(xb);
          XM.PackMsg(new CXM_Route(HRoute));
          xb=new CXM_QueryHistoryOther (StartTime.Seconds, LastTime.Seconds, (long)pDoc, (byte)m_Option, (byte)m_TimeOptUnits, m_TimeOptFull, m_Headings, m_NoPts, dNAN, 1/*File*/, NULL, NULL, 0, 0.0);
          i--;
          }
        }
      if (!xb->Empty())
        {
        XM.PackMsg(xb);
        XM.PackMsg(new CXM_Route(HRoute));
        }
      else
        delete xb;

      pDoc->XSendMessage(XM, HRoute);
      }
    else
      LogError("Trend", 0, "Cannot open file '%s' to retrieve query.", (char*)(const char*)m_Filename);
    }
  else
    {
    if (!GotHistorian)
      LogError("Trend", 0, "No historian configured, cannot retrieve query to '%s'", (char*)(const char*)m_Filename);
    else
      LogError("Trend", 0, "No tags specified, cannot retrieve query to '%s'", (char*)(const char*)m_Filename);
    }
  DestroyWindow();
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnCancel()
  {
  UpdateData(True);
  SaveOptions();
  DestroyWindow();
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnAddtag() 
  {
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    m_TagList.InsertString(i, "NewTag");
    m_TagList.SetCurSel(i);
    }
  else
    {
    m_TagList.AddString("NewTag");
    m_TagList.SetCurSel(m_TagList.GetCount()-1);
    }
  UpdateData(False);
  OnSelchangeTaglist();
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnRemovetag() 
  {
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0)
    m_TagList.DeleteString(i);
  int Cnt = m_TagList.GetCount();
  if (Cnt>0)
    {
    if (i<Cnt)
      m_TagList.SetCurSel(i);
    else
      m_TagList.SetCurSel(Cnt-1);
    }
  OnSelchangeTaglist();
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnSelchangeTaglist()
  {
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_TagList.GetText(i, Txt);
    m_Tag = Txt;
    GotoDlgCtrl(GetDlgItem(IDC_TAG));
    }	
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnChangeTag() 
  {
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    m_TagList.DeleteString(i);
    m_TagList.InsertString(i, m_Tag);
    m_TagList.SetCurSel(i);
    }	
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnOption() 
  {
  UpdateData(True);
  }

//---------------------------------------------------------------------------

void CQueryDlg::OnOption1() 
  {
  UpdateData(True);
  }

//=========================================================================
//---------------------------------------------------------------------------

