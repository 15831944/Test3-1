//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "execlib.h"
#include "drv_base.h"
#include "scopcsrv.h"
#include "opcdlgs.h"
#include "scd_wm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#ifdef USE_THIS_CODE


IMPLEMENT_DYNCREATE(COPCStatsPage, CPropertyPage)

// --------------------------------------------------------------------------

COPCStatsPage::COPCStatsPage() : CPropertyPage(COPCStatsPage::IDD)
  {
  ASSERT(FALSE);
  }

// --------------------------------------------------------------------------

COPCStatsPage::COPCStatsPage(pCOPCDriver Drv) : CPropertyPage(COPCStatsPage::IDD)
  {
  //{{AFX_DATA_INIT(COPCStatsPage)
  m_IterTime = 0.0;
  //}}AFX_DATA_INIT
  for (int i=0; i<MaxOPCClnts; i++)
    {
    m_Coms[i] = "";
    m_Times[i] = 0.0;
    m_Procs[i] = 0.0;
    }
  bDidInit = False;
  pDrv = Drv;
  }

// --------------------------------------------------------------------------

COPCStatsPage::~COPCStatsPage()
  {
  }

// --------------------------------------------------------------------------

void COPCStatsPage::DoDataExchange(CDataExchange* pDX)
  {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COPCStatsPage)
  DDX_Control(pDX, IDC_STATSLIST, m_StatsList);
  DDX_Text(pDX, IDC_ITERTIME, m_IterTime);
  //}}AFX_DATA_MAP

  for (int i=0; i<MaxOPCClnts; i++)
    {
    DDX_Text(pDX, IDC_ACOM+i, m_Coms[i]);
    DDX_Text(pDX, IDC_ATIME+i, m_Times[i]);
    DDX_Text(pDX, IDC_APROC+i, m_Procs[i]);
    }
  }

// --------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COPCStatsPage, CPropertyPage)
  //{{AFX_MSG_MAP(COPCStatsPage)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_DRVUPDATESTATS, OnUpdateStats)
END_MESSAGE_MAP()

// --------------------------------------------------------------------------

BOOL COPCStatsPage::OnInitDialog()
  {
  CPropertyPage::OnInitDialog();
  SW.Clear();
  SW.Start();
  PrevIterTime = SW.Lap();
  bDidInit = True;
  INT TabStops[MaxOPCClnts];
  for (int i=0; i<MaxOPCClnts; i++)
    TabStops[i] = i * 18;
  char Buff[128];
  for (i=0; i<MaxOPCClnts; i++)
    {
    if (i<pDrv->ClntsInUse)
      sprintf(Buff, "%s", pDrv->Clnts[i]->sName());
    else
      sprintf(Buff, "%s", "");
    m_Coms[i] = Buff;
    }
  UpdateData(FALSE);
  m_StatsList.SetTabStops(MaxOPCClnts, TabStops);
  return TRUE;
  }

// --------------------------------------------------------------------------

LRESULT COPCStatsPage::OnUpdateStats(WPARAM wParam, LPARAM lParam)
  {
  if (bDidInit)
    {
    m_StatsList.ResetContent();
    char Buff[512];
    for (int i=0; i<MaxDriverStats; i++)
      {
      sprintf(Buff, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s",
                      pDrv->StatsCnt[0][i], pDrv->StatsCnt[1][i],
                      pDrv->StatsCnt[2][i], pDrv->StatsCnt[3][i],
                      pDrv->StatsCnt[4][i], pDrv->StatsCnt[5][i],
                      pDrv->StatsCnt[6][i], pDrv->StatsCnt[7][i],
                      (const char*)(pDrv->Stats[i]));
      m_StatsList.AddString(Buff);
      for (int j=0; j<MaxOPCClnts; j++)
        pDrv->StatsCnt[j][i] = 0;
      }
    for (i=0; i<MaxOPCClnts; i++)
      {
      m_Times[i] = pDrv->AveLoopTime(i);
      m_Procs[i] = 0.0;
      }
    m_IterTime = SW.Lap()-PrevIterTime;
    PrevIterTime = SW.Lap();
    UpdateData(FALSE);
    }
  return True;
  }

// --------------------------------------------------------------------------


//===========================================================================

IMPLEMENT_DYNCREATE(COPCHoldPage, CPropertyPage)

COPCHoldPage::COPCHoldPage() : CPropertyPage(COPCHoldPage::IDD)
  {
  ASSERT(FALSE);
  }

// --------------------------------------------------------------------------

COPCHoldPage::COPCHoldPage(pCOPCDriver Drv) : CPropertyPage(COPCHoldPage::IDD)
  {
  //{{AFX_DATA_INIT(COPCHoldPage)
  m_DelayTime = 0;
  m_FullAddr = _T("");
  m_UseDelay2 = FALSE;
  m_DelayTime2 = 0;
  m_Type = _T("");
  m_IO = _T("");
  //}}AFX_DATA_INIT
  bDidInit = False;
  pDrv = Drv;
  }

// --------------------------------------------------------------------------

COPCHoldPage::~COPCHoldPage()
  {
  }

// --------------------------------------------------------------------------

BOOL COPCHoldPage::OnInitDialog()
  {
  CPropertyPage::OnInitDialog();
  bDidInit = True;
  pCurSlot = NULL;
  RefreshDrvTagList();
  return TRUE;
  }

// --------------------------------------------------------------------------

void COPCHoldPage::DoDataExchange(CDataExchange* pDX)
  {
  CPropertyPage::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COPCHoldPage)
  DDX_Control(pDX, IDC_TAGCOMBO, m_TagCombo);
  DDX_Control(pDX, IDC_OPLIST, m_OpList);
  DDX_Text(pDX, IDC_DELAYTIME, m_DelayTime);
  DDV_MinMaxDWord(pDX, m_DelayTime, 0, 1000000);
  DDX_Text(pDX, IDC_FULLADDR, m_FullAddr);
  DDX_Check(pDX, IDC_USEDELAY2, m_UseDelay2);
  DDX_Text(pDX, IDC_DELAYTIME2, m_DelayTime2);
  DDX_Text(pDX, IDC_TYPE, m_Type);
  DDX_Text(pDX, IDC_IO, m_IO);
  //}}AFX_DATA_MAP
  }

// --------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COPCHoldPage, CPropertyPage)
  //{{AFX_MSG_MAP(COPCHoldPage)
  ON_BN_CLICKED(IDC_HOLD, OnHold)
  ON_CBN_SELCHANGE(IDC_TAGCOMBO, OnSelchangeTagcombo)
  ON_LBN_SELCHANGE(IDC_OPLIST, OnSelchangeOplist)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_HOLD, OnUpdateHoldBtn)
  ON_UPDATE_COMMAND_UI(IDC_DELAYTIME, OnUpdateDelay)
  ON_UPDATE_COMMAND_UI(IDC_DELAYTIME2, OnUpdateDelay2)
  ON_UPDATE_COMMAND_UI(IDC_USEDELAY2, OnUpdateUseDelay2)
END_MESSAGE_MAP()

// --------------------------------------------------------------------------

void COPCHoldPage::RefreshDrvTagList()
  {
  for (int s=0; s<pDrv->Slots.GetSize(); s++)
    m_TagCombo.AddString(pDrv->Slots[s]->sTag());
  if (m_TagCombo.GetCount()>=0)
    {
    CProfINIFile PF(PrjIniFile());
    Strng sTag;
    sTag=PF.RdStr("OPC", "PrevSlot");

    m_TagCombo.SelectString(-1, sTag());
//    int i=m_TagCombo.FindString(-1, sTag());
//    m_TagCombo.SetCurSel(i);
    }
  UpdateData(FALSE);
  OnSelchangeTagcombo();
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnUpdateHoldBtn(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pCurSlot && m_OpList.GetCount()>0 && m_OpList.GetCurSel()>=0);
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnHold()
  {
  UpdateData(TRUE);
  if (pCurSlot && m_OpList.GetCount()>0 && m_OpList.GetCurSel()>=0)
    {
    int i = m_OpList.GetCurSel();
    int j = 0;
    pCDrvSubsConn pCon = pCurSlot->pConSlot;
    while (j<i && pCon)
      {
      j++;
      pCon = pCon->pNxt;
      }
    pCon->SetDelayTimes(m_DelayTime, m_DelayTime2, m_UseDelay2);
    }
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnSelchangeTagcombo()
  {
  UpdateData(TRUE);
  m_FullAddr = "";
  m_Type = "";
  m_IO = "";
  m_OpList.ResetContent();
  int i = m_TagCombo.GetCurSel();
  if (i!=CB_ERR)
    {
    CString Txt;
    m_TagCombo.GetLBText(i, Txt);
    pCurSlot = pDrv->FindSlot((pchar)(const char*)Txt);
    if (pCurSlot)
      {
      char Buff[512];
      pCDrvSubsConn pCon = pCurSlot->pConSlot;
      while (pCon)
        {
        if (!pCon->m_bGet/* && pCon->bInternal*/)
          {
          sprintf(Buff, "%s  %s", (pCon->m_bGet ? "Get" : "Set"), pCon->m_sTag());
          m_OpList.AddString(Buff);
          }
        pCon = pCon->pNxt;
        }
      m_FullAddr = ((pCOPCSlot)(pCurSlot))->m_sFullAddr;
      m_Type = tt_TypeString(pCurSlot->iTyp);
      m_IO = ((pCurSlot->iAction & XIO_Out) ? "Read" : "");
      m_IO += ((pCurSlot->iAction & XIO_In) ? "Write" : "");
      }
    }
  if (m_OpList.GetCount()>=0)
    m_OpList.SetCurSel(0);
  UpdateData(FALSE);
  OnSelchangeOplist();
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnSelchangeOplist()
  {
  UpdateData(TRUE);
  m_DelayTime = 0;
  m_DelayTime2 = 0;
  m_UseDelay2 = False;
  if (pCurSlot)
    {
    int i = m_OpList.GetCurSel();
    if (i!=CB_ERR)
      {
      int j = 0;
      pCDrvSubsConn pCon = pCurSlot->pConSlot;
      while (j<i && pCon)
        {
        j++;
        pCon = pCon->pNxt;
        }
      m_DelayTime = pCon->dwDelayTime;
      m_DelayTime2 = pCon->dwDelayTime2;
      m_UseDelay2 = pCon->bUseDelay2;
      }
    }
  UpdateDialogControls(this, FALSE);
  UpdateData(FALSE);
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnUpdateDelay(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pCurSlot && m_OpList.GetCount()>0 && m_OpList.GetCurSel()>=0);
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnUpdateDelay2(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pCurSlot && pCurSlot->iTyp==tt_Bool && m_OpList.GetCount()>0 && m_OpList.GetCurSel()>=0);
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnUpdateUseDelay2(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pCurSlot && pCurSlot->iTyp==tt_Bool && m_OpList.GetCount()>0 && m_OpList.GetCurSel()>=0);
  }

// --------------------------------------------------------------------------

void COPCHoldPage::OnOK()
  {
  if (bDidInit)
    {
    int i=-1;
    if (m_TagCombo.GetCount())
      i=m_TagCombo.GetCurSel();
    CString Str;
    if (i!=LB_ERR)
      m_TagCombo.GetLBText(i, Str);
    CProfINIFile PF(PrjIniFile());
    PF.WrStr("OPC", "PrevSlot", Str);
    }
  CPropertyPage::OnOK();
  };

// --------------------------------------------------------------------------

void COPCHoldPage::OnCancel()
  {
  if (bDidInit)
    {
    int i=-1;
    if (m_TagCombo.GetCount())
      i=m_TagCombo.GetCurSel();
    CString Str;
    if (i!=LB_ERR)
      m_TagCombo.GetLBText(i, Str);
    CProfINIFile PF(PrjIniFile());
    PF.WrStr("OPC", "PrevSlot", Str);
    }
  CPropertyPage::OnCancel();
  }

// --------------------------------------------------------------------------

#endif