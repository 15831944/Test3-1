//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $                            
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "project.h"
#include "WndSlct.h"
#include "scd_wm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
// CWndArrangeDlg dialog

//===========================================================================

CWndArrangeDlg::CWndArrangeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWndArrangeDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CWndArrangeDlg)
  m_MaximiseMain  = ScdPFUser.RdInt("ArrangeWindows", "MaximiseMain", 1)!=0;
  m_GrfMinimise   = ScdPFUser.RdInt("ArrangeWindows", "GrfMinimise", 1)!=0;
  m_OthMinimise   = ScdPFUser.RdInt("ArrangeWindows", "OthMinimise", 1)!=0;
  m_TrnMinimise   = ScdPFUser.RdInt("ArrangeWindows", "TrnMinimise", 1)!=0;
  m_GrfStyle      = ScdPFUser.RdInt("ArrangeWindows", "GrfStyle", 1);
  m_OthStyle      = ScdPFUser.RdInt("ArrangeWindows", "OthStyle", 1);
  m_TrnStyle      = ScdPFUser.RdInt("ArrangeWindows", "TrnStyle", 1);

  //}}AFX_DATA_INIT
  }

//--------------------------------------------------------------------------

void CWndArrangeDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CWndArrangeDlg)
  DDX_Check(pDX, IDC_AW_MAXMAIN, m_MaximiseMain);
  DDX_Check(pDX, IDC_AW_GRFMINIMISE, m_GrfMinimise);
  DDX_Check(pDX, IDC_AW_OTHMINIMISE, m_OthMinimise);
  DDX_Check(pDX, IDC_AW_TRNMINIMISE, m_TrnMinimise);
  DDX_Radio(pDX, IDC_AW_GRFPOSNONE, m_GrfStyle);
  DDX_Radio(pDX, IDC_AW_OTHPOSNONE, m_OthStyle);
  DDX_Radio(pDX, IDC_AW_TRNPOSNONE, m_TrnStyle);
  //}}AFX_DATA_MAP
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWndArrangeDlg, CDialog)
  //{{AFX_MSG_MAP(CWndArrangeDlg)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

void CWndArrangeDlg::OnBnClickedOk()
  {
  OnOK();

  // TODO: Add your control notification handler code here
  ScdPFUser.WrInt("ArrangeWindows", "MaximiseMain",m_MaximiseMain?1:0);
  ScdPFUser.WrInt("ArrangeWindows", "GrfMinimise", m_GrfMinimise?1:0);
  ScdPFUser.WrInt("ArrangeWindows", "OthMinimise", m_OthMinimise?1:0);
  ScdPFUser.WrInt("ArrangeWindows", "TrnMinimise", m_TrnMinimise?1:0);
  ScdPFUser.WrInt("ArrangeWindows", "GrfStyle",    m_GrfStyle);
  ScdPFUser.WrInt("ArrangeWindows", "OthStyle",    m_OthStyle);
  ScdPFUser.WrInt("ArrangeWindows", "TrnStyle",    m_TrnStyle);
  }

//===========================================================================
