//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "scdver.h"
//#include "scd_wm.h"
#if WITHOPC
#include "sc_defs.h"
#include "scd_wm.h"
#include "scdctrls.h"
#include "resource.h"
#include "srvdlgs.h"
#include "executiv.h"
#include "srvmngr.h"
#include "srvcallback.h"

//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

class COPCSrvrOptionsDlg : public CDialog
  {
  public:
	  COPCSrvrOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	  //{{AFX_DATA(COPCSrvrOptionsDlg)
	  enum { IDD = IDD_OPCSRVROPTIONSDLG };
	  BOOL	m_bShowStats;
	  BOOL	m_bLogNotes;
	  BOOL	m_bForceStart;
	  BOOL	m_bForceStop;
	  long	m_iForceCnt;
	  //}}AFX_DATA
	  //{{AFX_VIRTUAL(COPCSrvrOptionsDlg)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
	  //{{AFX_MSG(COPCSrvrOptionsDlg)
	  virtual void OnOK();
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };

//===========================================================================

COPCSrvrStatsDlg::COPCSrvrStatsDlg(CScdOPCManager* pOPCMngr, CWnd* pParent /*=NULL*/)
  : CDialog(COPCSrvrStatsDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(COPCSrvrStatsDlg)
	m_ShowStats = 0;
	//}}AFX_DATA_INIT
  pMngr = pOPCMngr;
  bDidInit = 0;
  Create(COPCSrvrStatsDlg::IDD, pParent); // NonModal Create
  }

//---------------------------------------------------------------------------

COPCSrvrStatsDlg::~COPCSrvrStatsDlg()
  {
  if (pMngr)
    pMngr->pTheOPCStatsDlg = NULL;
  }

//--------------------------------------------------------------------------

void COPCSrvrStatsDlg::PostNcDestroy() 
  {
  CDialog::PostNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COPCSrvrStatsDlg)
	DDX_Control(pDX, IDC_OPCSTATSLIST, m_StatsList);
	DDX_Radio(pDX, IDC_OPCSHOWSTATS, m_ShowStats);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COPCSrvrStatsDlg, CDialog)
  //{{AFX_MSG_MAP(COPCSrvrStatsDlg)
	ON_BN_CLICKED(IDC_OPCFORCE, OnForce)
	ON_BN_CLICKED(IDC_OPCSHOWSTATS, OnShowStats)
	ON_BN_CLICKED(IDC_OPCSHOWTAGS, OnShowStats)
	ON_BN_CLICKED(IDC_OPCREFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_OPCOPTIONS, OnOptions)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_OPCSRVRUPDATESTATS, OnUpdateStats)
  ON_UPDATE_COMMAND_UI(IDC_OPCFORCE, OnUpdateForce)
  ON_UPDATE_COMMAND_UI(IDC_OPCREFRESH, OnUpdateRefresh)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL COPCSrvrStatsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  CProfINIFile PF(PrjIniFile());
  int xPos = PF.RdInt("General", "OPCServerXPos", 10);
  int yPos = PF.RdInt("General", "OPCServerYPos", 50);
  //UpdateData(FALSE);
  SetVisibleWindowPos(this, xPos, yPos);
  m_StatsList.SetTabStops(22);
  iHistIndex = 0;
  bDidInit = 1;
  //UpdateDialogControls(this, FALSE);
  PostMessage(WMU_OPCSRVRUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  return TRUE;
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::OnOK() 
  {
  RECT Rect;
  GetWindowRect(&Rect);
  CProfINIFile PF(PrjIniFile());
  PF.WrInt("General", "OPCServerXPos", Rect.left);
  PF.WrInt("General", "OPCServerYPos", Rect.top);
  CDialog::OnOK();
  DestroyWindow();
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::OnCancel() 
  {
  OnOK();
  }

//---------------------------------------------------------------------------

LRESULT COPCSrvrStatsDlg::OnUpdateStats(WPARAM wParam, LPARAM lParam)
  {
  static int UpdateCnt = 0;
  const char* UpdateRotate[4] = {"/","-","\\","|"};
  wParam &= (~(SUB_UPDATE_SENDMSG|SUB_UPDATE_SUBSCHANGED));
  const bool IsExec = (wParam==SUB_UPDATE_EOWRITE || wParam==SUB_UPDATE_EOEXEC);
  if (!IsExec)
    UpdateDialogControls(this, FALSE);
  if (bDidInit && pMngr->OPCExec())
    {
    ASSERT(pMngr->OPCExec());
    const int TagCnt = pMngr->OPCExec()->iTagCnt;
    char Buff[128];
    if (!XBusy())
      {
      SetDlgItemText(IDC_OPCSTATUS_TXT, ".");
      sprintf(Buff, "Stopped! (%d tags)", TagCnt);
      }
    else
      {
      if (IsExec)
        {
        if (++UpdateCnt>3)
          UpdateCnt=0;
        }
      SetDlgItemText(IDC_OPCSTATUS_TXT, UpdateRotate[UpdateCnt]);
      sprintf(Buff, "%d subscription tags", TagCnt);
      //sprintf(Buff, "%d subscription tags.  UpdateRate:%d", TagCnt, pMngr->OPCExec()->dwMeasScanRate);
      }
    SetDlgItemText(IDC_OPCSUBSCNT_TXT, Buff);

    if (m_ShowStats==0)
      {//show stats...
      m_StatsList.ResetContent();
      sprintf(Buff, "Read\tOPC:%d   SysCAD:%d", pMngr->OPCExec()->iClientReadOKCnt, pMngr->OPCExec()->iScdReadOKCnt);
      m_StatsList.InsertString(-1, Buff);
      sprintf(Buff, "Write\tOPC:%d   SysCAD:%d", pMngr->OPCExec()->iClientWriteOKCnt, pMngr->OPCExec()->iScdWriteOKCnt);
      m_StatsList.InsertString(-1, Buff);
      sprintf(Buff, "Scan\t%d   (UpdateRate:%d)", pMngr->OPCExec()->iScanCnt, pMngr->OPCExec()->dwMeasScanRate);
      m_StatsList.InsertString(-1, Buff);
      if (pMngr->OPCExec()->iWriteFailCnt)
        {
        sprintf(Buff, "Error\t%d Write Errors", pMngr->OPCExec()->iWriteFailCnt);
        m_StatsList.InsertString(-1, Buff);
        }
      /*sprintf(Buff, "%d\tSteps", pMngr->OPCExec()->iExecuteCnt);
      m_StatsList.InsertString(-1, Buff);*/

      if (TagCnt>0)
        {
        int j = (iHistIndex==0 ? MaxOPCStatsHistory-1 : iHistIndex-1);
        for (int i=0; i<MaxOPCStatsHistory; i++)
          {
          if (Hist[j]())
            m_StatsList.InsertString(-1, Hist[j]());
          if (--j<0)
            j = MaxOPCStatsHistory-1;
          }
        if (IsExec)
          {//iter completed
          if (pMngr->OPCExec()->iWriteFailCnt)
            Hist[iHistIndex++].Set("%d\tW:%d(%d) R:%d(%d)  [WriteErrors:%d]", pMngr->OPCExec()->iExecuteCnt, pMngr->OPCExec()->iClientWriteOKCnt, pMngr->OPCExec()->iScdWriteOKCnt, pMngr->OPCExec()->iClientReadOKCnt, pMngr->OPCExec()->iScdReadOKCnt, pMngr->OPCExec()->iWriteFailCnt);
          else
            Hist[iHistIndex++].Set("%d\tW:%d(%d) R:%d(%d) Sc:%d", pMngr->OPCExec()->iExecuteCnt, pMngr->OPCExec()->iClientWriteOKCnt, pMngr->OPCExec()->iScdWriteOKCnt, pMngr->OPCExec()->iClientReadOKCnt, pMngr->OPCExec()->iScdReadOKCnt, pMngr->OPCExec()->iScanCnt);
          if (iHistIndex==MaxOPCStatsHistory)
            iHistIndex = 0;
          }
        }
      if (IsExec)
        pMngr->OPCExec()->StatsCntReset();
      UpdateData(FALSE);
      }
    else if (m_ShowStats==1)
      {//show tag list & values...
      if (bSubsChanged)
        {
        m_StatsList.ResetContent();
        //m_StatsList.InsertString(-1, "Not implemented");
        pMngr->OPCExec()->BuildSubsList(&m_StatsList);
        UpdateData(FALSE);
        bSubsChanged = 0;
        }
      }
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::OnRefresh() 
  {
  if (m_ShowStats==1)
    bSubsChanged = 1;
  PostMessage(WMU_OPCSRVRUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  }

void COPCSrvrStatsDlg::OnUpdateRefresh(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(TRUE);//m_ShowStats==1 || !XBusy());
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::OnOptions() 
  {
  COPCSrvrOptionsDlg Dlg;
  Dlg.DoModal();
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::OnForce() 
  {
  //if (XBusy())
  if (pMngr->OPCExec())
    //pMngr->OPCExec()->DoWriteAll();
    pMngr->OPCExec()->ForceWriteSubsDataAll();
  PostMessage(WMU_OPCSRVRUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  }

void COPCSrvrStatsDlg::OnUpdateForce(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(TRUE);//iTagCnt>0);//XBusy() && iTagCnt>0);
  }

//---------------------------------------------------------------------------

void COPCSrvrStatsDlg::OnShowStats() 
  {
  bSubsChanged = 1;
  UpdateData(TRUE);
  //if (m_ShowStats!=0)
    PostMessage(WMU_OPCSRVRUPDATESTATS, (WPARAM)SUB_UPDATE_REDRAW, (LPARAM)0);
  }

//---------------------------------------------------------------------------
//===========================================================================
// COPCSrvrOptionsDlg dialog

COPCSrvrOptionsDlg::COPCSrvrOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COPCSrvrOptionsDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(COPCSrvrOptionsDlg)
  m_bShowStats = CScdOPCManager::sm_pTheMngr->bShowStatsOnConnect;
  m_bLogNotes = CScdOPCManager::sm_pTheMngr->bDoLogNotes;
  m_bForceStart = CScdOPCManager::sm_pTheMngr->bForceOnStart;
  m_bForceStop = CScdOPCManager::sm_pTheMngr->bForceOnStop;
  m_iForceCnt = CScdOPCManager::sm_pTheMngr->iForceCnt;
	//}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void COPCSrvrOptionsDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COPCSrvrOptionsDlg)
	DDX_Check(pDX, IDC_OPCOPT_SHOWSTATS, m_bShowStats);
	DDX_Check(pDX, IDC_OPCOPT_LOGNOTES, m_bLogNotes);
	DDX_Check(pDX, IDC_OPCOPT_FORCESTART, m_bForceStart);
	DDX_Check(pDX, IDC_OPCOPT_FORCESTOP, m_bForceStop);
	DDX_Text(pDX, IDC_OPCOPT_FORCECOUNT, m_iForceCnt);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COPCSrvrOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COPCSrvrOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL COPCSrvrOptionsDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here

  return TRUE;
  }

//---------------------------------------------------------------------------

void COPCSrvrOptionsDlg::OnOK() 
  {
  UpdateData(TRUE);
  CScdOPCManager::sm_pTheMngr->bShowStatsOnConnect = m_bShowStats!=0;
  CScdOPCManager::sm_pTheMngr->bDoLogNotes = m_bLogNotes!=0;
  CScdOPCManager::sm_pTheMngr->bForceOnStart = m_bForceStart!=0;
  CScdOPCManager::sm_pTheMngr->bForceOnStop = m_bForceStop!=0;
  CScdOPCManager::sm_pTheMngr->iForceCnt = m_iForceCnt;
  //CProfINIFile PF(Prj???IniFile());
  //PF.WrInt("OPCServer", "", );
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------
#endif
