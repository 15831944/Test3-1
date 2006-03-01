//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "scd_wm.h"
#include "scdctrls.h"
#include "resource.h"
#include "dde_dlgs.h"
#include "dde_mngr.h"
#include "dde_exec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

CDDEStatsDlg::CDDEStatsDlg(CScdDDEManager* pDDEMngr, CWnd* pParent /*=NULL*/)
  : CDialog(CDDEStatsDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CDDEStatsDlg)
	m_ShowStats = 0;
	//}}AFX_DATA_INIT
  pMngr = pDDEMngr;
  bDidInit = 0;
  Create(CDDEStatsDlg::IDD, pParent); // NonModal Create
  }

//---------------------------------------------------------------------------

CDDEStatsDlg::~CDDEStatsDlg()
  {
  if (pMngr)
    pMngr->pTheDDEStatsDlg = NULL;
  }

//--------------------------------------------------------------------------

void CDDEStatsDlg::PostNcDestroy() 
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

void CDDEStatsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDDEStatsDlg)
	DDX_Control(pDX, IDC_DDESTATSLIST, m_StatsList);
	DDX_Radio(pDX, IDC_DDESHOWSTATS, m_ShowStats);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDDEStatsDlg, CDialog)
  //{{AFX_MSG_MAP(CDDEStatsDlg)
	ON_BN_CLICKED(IDC_DDEFORCE, OnForce)
	ON_BN_CLICKED(IDC_DDESHOWSTATS, OnShowStats)
	ON_BN_CLICKED(IDC_DDESHOWTAGS, OnShowStats)
	ON_BN_CLICKED(IDC_DDEREFRESH, OnRefresh)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DDEUPDATESTATS, OnUpdateStats)
  ON_UPDATE_COMMAND_UI(IDC_DDEFORCE, OnUpdateForce)
  ON_UPDATE_COMMAND_UI(IDC_DDEREFRESH, OnUpdateRefresh)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CDDEStatsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  int xPos = PF.RdInt("General", "DDEXPos", 10);
  int yPos = PF.RdInt("General", "DDEYPos", 50);
  //UpdateData(FALSE);
  SetVisibleWindowPos(this, xPos, yPos);
  m_StatsList.SetTabStops(20);
  bDidInit = 1;
  //UpdateDialogControls(this, FALSE);
  PostMessage(WMU_DDEUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  return TRUE;
  }

//---------------------------------------------------------------------------

void CDDEStatsDlg::OnOK() 
  {
  RECT Rect;
  GetWindowRect(&Rect);
  CProfINIFile PF(PrjIniFile());
  PF.WrInt("General", "DDEXPos", Rect.left);
  PF.WrInt("General", "DDEYPos", Rect.top);
  CDialog::OnOK();
  DestroyWindow();
  }

//---------------------------------------------------------------------------

void CDDEStatsDlg::OnCancel() 
  {
  OnOK();
  }

//---------------------------------------------------------------------------

const char* DDEStatDescs[] = { "Writes", "Requests", "Pokes", "Invalid tag requests" };

LRESULT CDDEStatsDlg::OnUpdateStats(WPARAM wParam, LPARAM lParam)
  {
  static int UpdateCnt = 0;
  const char* UpdateRotate[4] = {"/","-","\\","|"};
  wParam &= (~(SUB_UPDATE_SENDMSG|SUB_UPDATE_SUBSCHANGED));
  const bool IsExec = (wParam==SUB_UPDATE_EOWRITE || wParam==SUB_UPDATE_EOEXEC);
  if (!IsExec)
    UpdateDialogControls(this, FALSE);
  if (bDidInit && pMngr->DDEExec())
    {
    const int TagCnt = pMngr->DDEExec()->SubsData.GetSize();
    char Buff[128];
    if (!XBusy())
      {
      SetDlgItemText(IDC_DDESTATUS_TXT, ".");
      sprintf(Buff, "Stopped! (%d tags)", TagCnt);
      }
    else
      {
      if (IsExec)
        {
        if (++UpdateCnt>3)
          UpdateCnt=0;
        }
      SetDlgItemText(IDC_DDESTATUS_TXT, UpdateRotate[UpdateCnt]);
      sprintf(Buff, "%d subscription tags", TagCnt);
      }
    SetDlgItemText(IDC_DDESUBSCNT_TXT, Buff);

    if (m_ShowStats==0)
      {//show stats...
      m_StatsList.ResetContent();
      for (int i=0; i<MaxDDEStats; i++)
        {
        sprintf(Buff, "%d\t%s", pMngr->DDEExec()->StatsCnt[i], DDEStatDescs[i]);
        m_StatsList.InsertString(-1, Buff);
        if (IsExec)
          pMngr->DDEExec()->StatsCnt[i] = 0;
        }
      UpdateData(FALSE);
      }
    else if (m_ShowStats==1)
      {//show tag list & values...
      if (bSubsChanged)
        {
        m_StatsList.ResetContent();
        //pMngr->DDEExec()->BuildTagList(&m_StatsList);
        pMngr->DDEExec()->BuildSubsList(&m_StatsList);
        UpdateData(FALSE);
        bSubsChanged = 0;
        }
      }
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

void CDDEStatsDlg::OnRefresh() 
  {
  if (m_ShowStats!=0)
    {
    bSubsChanged = 1;
    PostMessage(WMU_DDEUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
    }
  }

void CDDEStatsDlg::OnUpdateRefresh(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_ShowStats!=0);
  }

//---------------------------------------------------------------------------

void CDDEStatsDlg::OnForce() 
  {
  if (XBusy())
    //pMngr->DDEExec()->DoWriteAll();
    pMngr->DDEExec()->ForceAllWrites();
  //PostMessage(WMU_DDEUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  }

void CDDEStatsDlg::OnUpdateForce(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(XBusy());
  }

//---------------------------------------------------------------------------

void CDDEStatsDlg::OnShowStats() 
  {
  bSubsChanged = 1;
  UpdateData(TRUE);
  if (m_ShowStats!=0)
    PostMessage(WMU_DDEUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  }

//---------------------------------------------------------------------------

