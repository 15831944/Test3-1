
#include "stdafx.h"
//#include "rtts.h"
#include "RTDlgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

CRTTSCfgPage::CRTTSCfgPage(CMdlCfgSheet * Sheet) 
  : CMdlCfgBase(CRTTSCfgPage::IDD, Sheet)
  {
  //{{AFX_DATA_INIT(CRTTSCfgPage)
	m_DBName = _T("");
	m_TableName = _T("");
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

CRTTSCfgPage::~CRTTSCfgPage()
  {
  }

//---------------------------------------------------------------------------

void CRTTSCfgPage::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRTTSCfgPage)
	DDX_Control(pDX, IDC_ASSAYLIST, m_AssayList);
	DDX_Text(pDX, IDC_DBNAME, m_DBName);
	DDX_Text(pDX, IDC_TABLENAME, m_TableName);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CRTTSCfgPage, CMdlCfgBase)
	//{{AFX_MSG_MAP(CRTTSCfgPage)
	ON_BN_CLICKED(IDC_RT_ADD, OnAddAssay)
	ON_BN_CLICKED(IDC_RT_CHANGE, OnChangeAssay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CRTTSCfgPage::OnInitDialog() 
  {
	CMdlCfgBase::OnInitDialog();

  VERIFY(m_AssayList.SubclassMoveUpDownControls(this, IDC_ASSAYLIST, IDC_RT_MOVEUP, IDC_RT_MOVEDOWN));
  VERIFY(m_AssayList.SubclassDeleteControl(this, IDC_ASSAYLIST, IDC_RT_REMOVE));

  //load cfg data...
  Strng S,Tag;
  S = Cfg.RdStr("ExtraSpecieProperties", "Database", "OreProps.mdb");
  m_DBName = S();
  S = Cfg.RdStr("ExtraSpecieProperties", "TableName", "OreProperties");
  m_TableName = S();
  //m_AssayList.ResetContent();
  for (int Cnt=0;;)
    {
    Tag.Set("Assay%02i", Cnt++);
    S = Cfg.RdStr("AssayColumns", Tag(), "???");
    if (S.XStrICmp("???")==0)
      break;
    m_AssayList.AddString(S());
    }
  if (m_AssayList.GetCount()>0)
    m_AssayList.SetCurSel(0);

  UpdateData(FALSE);
  bInited=true;
	return TRUE;
  }

//---------------------------------------------------------------------------

void CRTTSCfgPage::OnOK() 
  {
	CMdlCfgBase::OnOK();
  }

//---------------------------------------------------------------------------

BOOL CRTTSCfgPage::OnKillActive() 
  {
	BOOL OK=CMdlCfgBase::OnKillActive();
  if (bInited)
    {
    UpdateData(TRUE);
    Cfg.WrStr("ExtraSpecieProperties", "Database", (const char*)m_DBName);
    Cfg.WrStr("ExtraSpecieProperties", "TableName", (const char*)m_TableName);
    CString Txt;
    Strng Tag;
    int i;
    for (i=0; i<m_AssayList.GetCount(); i++)
      {
      m_AssayList.GetText(i, Txt);
      Tag.Set("Assay%02i", i);
      Cfg.WrStr("AssayColumns", Tag(), (const char*)Txt);
      }
    for (i=m_AssayList.GetCount(); i<m_AssayList.GetCount()+3; i++)
      {
      Tag.Set("Assay%02i", i);
      Cfg.WrStr("AssayColumns", Tag(), (char*)NULL);
      }

    }  
  return OK;
  }

//---------------------------------------------------------------------------

BOOL CRTTSCfgPage::OnSetActive()
  {
	BOOL OK=CMdlCfgBase::OnSetActive();
  //if (bInited)
  //  {
  //  }
  return OK;
  }

//---------------------------------------------------------------------------

void CRTTSCfgPage::OnAddAssay() 
  {
  CStringDlg Dlg("Assay", "Assay Field Name:");
  if (Dlg.DoModal()==IDOK)
    {
    Strng s(Dlg.Data());
    s.LRTrim();
    if (s.Len()>0)
      {
      int i = m_AssayList.AddString(s());
      m_AssayList.SetCurSel(i);
      }
    }
  }

//---------------------------------------------------------------------------

void CRTTSCfgPage::OnChangeAssay() 
  {
  int i = m_AssayList.GetCurSel(); 
  if (i>=0)
    {
    CString Txt;
    m_AssayList.GetText(i, Txt);
    CStringDlg Dlg("Assay", "Assay Field Name:", (char*)(const char*)Txt);
    if (Dlg.DoModal()==IDOK)
      {
      Strng s(Dlg.Data());
      s.LRTrim();
      if (s.Len()>0)
        {
        m_AssayList.DeleteString(i);
        if (m_AssayList.GetCount()==i)
          i = m_AssayList.InsertString(-1, s());
        else
          i = m_AssayList.InsertString(i, s());
        m_AssayList.SetCurSel(i);
        }
      }
    }
  }

//---------------------------------------------------------------------------
//===========================================================================

