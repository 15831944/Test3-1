//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
//#include "resource.h"
#include "hstmain.h"
//#include "gpwfuncs.h"
#include "hstdlgs.h"
#include "scd_wm.h"
//#include "msgwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================

CHstSlotDlg::CHstSlotDlg(CSlot* Slot, CWnd* pParent /*=NULL*/)
  : CDialog(CHstSlotDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CHstSlotDlg)
  m_Tag = _T("");
  m_Desc = _T("");
  m_Eng = _T("");
  m_Type = _T("");
  m_RecordIt = FALSE;
  m_BoxcarAlg = -1;
  m_Decrease = 0.0;
  m_DeltaLo = 0.0;
  m_DeltaHi = 0.0;
  m_FltTau = 0.0;
  m_FltDelta = 0.0;
  m_NoNotRec = 0;
  m_NoRec = 0;
  m_FilterAlg = -1;
  m_FltPeriod = 0.0;
  m_WinFltCount = 0;
  //}}AFX_DATA_INIT
  pSlot = Slot;
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHstSlotDlg)
  DDX_Text(pDX, IDC_TAG, m_Tag);
  DDX_Text(pDX, IDC_DESC, m_Desc);
  DDX_Text(pDX, IDC_ENG, m_Eng);
  DDX_Text(pDX, IDC_TYPE, m_Type);
  DDX_Check(pDX, IDC_RECORDIT, m_RecordIt);
  DDX_Radio(pDX, IDC_BOXCARRADIO, m_BoxcarAlg);
  DDX_Text(pDX, IDC_DECREASE, m_Decrease);
  DDX_Text(pDX, IDC_DELTALO, m_DeltaLo);
  DDX_Text(pDX, IDC_DELTAHI, m_DeltaHi);
  DDX_Text(pDX, IDC_FILTTAU, m_FltTau);
  DDX_Text(pDX, IDC_FLTDELTA, m_FltDelta);
  DDX_Text(pDX, IDC_NONOTREC, m_NoNotRec);
  DDX_Text(pDX, IDC_NOREC, m_NoRec);
  DDX_Radio(pDX, IDC_FLTRADIO, m_FilterAlg);
  DDX_Text(pDX, IDC_FLTPERIOD, m_FltPeriod);
  DDX_Text(pDX, IDC_WINFLTCNT, m_WinFltCount);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CHstSlotDlg, CDialog)
  //{{AFX_MSG_MAP(CHstSlotDlg)
  ON_BN_CLICKED(IDC_RECORDIT, OnRecordit)
  //}}AFX_MSG_MAP
  ON_CONTROL_RANGE(BN_CLICKED, IDC_BOXCARRADIO, IDC_BOXCARRADIO2, OnRadio)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_FLTRADIO, IDC_FLTRADIO3, OnRadio)
  ON_CONTROL_RANGE(EN_CHANGE, IDC_WINFLTCNT, IDC_FLTDELTA, OnChange)
  ON_CONTROL_RANGE(EN_CHANGE, IDC_DELTALO, IDC_DECREASE, OnChange)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CHstSlotDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  Init();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::Init()
  {
  SlotCfgInfo SCI;
  pSlot->GetCfgInfo(SCI);
  m_Tag = SCI.mTag;
  m_Desc = SCI.mDesc;
  m_Eng = SCI.mEng;
  m_Type = tt_TypeString(SCI.mTyp);
  //if (SCI.mDriver)
  //  m_Source = "Driver";
  //else
  //  m_Source = "Other";
  m_RecordIt = SCI.mRecordIt;
  m_BoxcarAlg = SCI.mBoxcarAlg;
  m_DeltaLo = SCI.mDeltaLo;
  m_DeltaHi = SCI.mDeltaHi;
  m_Decrease = SCI.mChange;
  m_NoNotRec = SCI.mNoNotRec;
  m_NoRec = SCI.mNoRec;
  m_FilterAlg = SCI.mFilterAlg;
  m_FltTau = SCI.mFilterTau;
  m_FltDelta = SCI.mFltDelta;
  m_FltPeriod = SCI.mWinFltPeriod;
  m_WinFltCount = SCI.mWinFltCount;
  bCanChange = (!SCI.mDriver);
  Changed(False);
  Enable();
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::Enable()
  {
  GetDlgItem(IDC_DELTALO)->EnableWindow(bCanChange && m_BoxcarAlg>0);
  flag f = (bCanChange && m_BoxcarAlg==2);
  GetDlgItem(IDC_DELTAHI)->EnableWindow(f);
  GetDlgItem(IDC_NOREC)->EnableWindow(f);
  GetDlgItem(IDC_NONOTREC)->EnableWindow(f);
  GetDlgItem(IDC_DECREASE)->EnableWindow(f);
  GetDlgItem(IDC_WINFLTCNT)->EnableWindow(bCanChange && m_FilterAlg==1 || m_FilterAlg==3);
  f = (bCanChange && m_FilterAlg>1);
  GetDlgItem(IDC_FILTTAU)->EnableWindow(f);
  GetDlgItem(IDC_FLTPERIOD)->EnableWindow(f);
  GetDlgItem(IDC_FLTDELTA)->EnableWindow(f);
  GetDlgItem(IDC_RECORDIT)->EnableWindow(bCanChange);
  GetDlgItem(IDC_RECORDIT)->Invalidate();

  f = (m_BoxcarAlg>0);
  GetDlgItem(IDC_TXT_DELTALO)->EnableWindow(f);
  f = (m_BoxcarAlg==2);
  GetDlgItem(IDC_TXT_DELTAHI)->EnableWindow(f);
  GetDlgItem(IDC_TXT_CONSECREC)->EnableWindow(f);
  GetDlgItem(IDC_TXT_DECREASE)->EnableWindow(f);
  GetDlgItem(IDC_TXT_DELTAHI)->EnableWindow(f);
  GetDlgItem(IDC_TXT_CONSECNOTREC)->EnableWindow(f);
  f = (m_FilterAlg==1 || m_FilterAlg==3);
  GetDlgItem(IDC_TXT_COUNT)->EnableWindow(f);
  f = (m_FilterAlg>1);
  GetDlgItem(IDC_TXT_TAU)->EnableWindow(f);
  GetDlgItem(IDC_TXT_PERIOD)->EnableWindow(f);
  GetDlgItem(IDC_TXT_DELTA)->EnableWindow(f);
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::Changed(flag HasChanged)
  {
  bChanged = HasChanged;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::OnRecordit() 
  {
  Changed();
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::OnRadio(UINT nID) 
  {
  UpdateData(True);
  Changed();
  Enable();
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::OnChange(UINT nID) 
  {
  Changed();
  }

//---------------------------------------------------------------------------

void CHstSlotDlg::OnOK() 
  {
  UpdateData(True);
  if (bChanged)
    {
    SlotCfgInfo SCI;
    pSlot->GetCfgInfo(SCI);
    SCI.mRecordIt = m_RecordIt;
    SCI.mUseFilter = (m_FilterAlg>0);
    if (SCI.mUseFilter)
      {
      SCI.mFilterAlg = m_FilterAlg;
      SCI.mFilterTau= m_FltTau;
      SCI.mWinFltPeriod = m_FltPeriod;
      SCI.mWinFltCount = m_WinFltCount;
      SCI.mFltDelta = m_FltDelta;
      }
    SCI.mUseBoxcar = (m_BoxcarAlg>0);
    if (SCI.mUseBoxcar)
      {
      SCI.mBoxcarAlg = m_BoxcarAlg;
      SCI.mDeltaLo = m_DeltaLo;
      SCI.mDeltaHi = m_DeltaHi;
      SCI.mNoRec = m_NoRec;
      SCI.mNoNotRec = m_NoNotRec;
      SCI.mChange = m_Decrease;
      }
    pHistory->Lock();
    long SlotNo = pHistory->CfgSlot(&SCI);
    pHistory->Save(SlotNo);
    pHistory->Release();
    }
  CDialog::OnOK();
  }

//===========================================================================

CHstSlotInfoDlg::CHstSlotInfoDlg(char* pFirstTag, CWnd* pParent /*=NULL*/)
  : CDialog(CHstSlotInfoDlg::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CHstSlotInfoDlg)
  m_Desc = _T("");
  m_Eng = _T("");
  m_Source = _T("");
  m_Type = _T("");
  m_ChgFreq = 0.0;
  m_RecFreq = 0.0;
  m_SelAll = TRUE;
  m_Rec = 0;
  m_Cmp = 0;
  m_Flt = 0;
  m_Drv = 0;
  m_Frq = 0;
  m_Recording = _T("");
  //}}AFX_DATA_INIT
  sFirstTag = pFirstTag;
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHstSlotInfoDlg)
  DDX_Text(pDX, IDC_DESC, m_Desc);
  DDX_Text(pDX, IDC_ENG, m_Eng);
  DDX_Text(pDX, IDC_SOURCE, m_Source);
  DDX_Text(pDX, IDC_TYPE, m_Type);
  DDX_Text(pDX, IDC_RECORDING, m_Recording);
  DDX_Text(pDX, IDC_CHGFREQ, m_ChgFreq);
  DDX_Text(pDX, IDC_RECFREQ, m_RecFreq);
  DDX_Check(pDX, IDC_SELALL, m_SelAll);
  DDX_Radio(pDX, IDC_REC, m_Rec);
  DDX_Radio(pDX, IDC_CMP, m_Cmp);
  DDX_Radio(pDX, IDC_FLT, m_Flt);
  DDX_Radio(pDX, IDC_DRV, m_Drv);
  DDX_Radio(pDX, IDC_FRQ, m_Frq);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CHstSlotInfoDlg, CDialog)
  //{{AFX_MSG_MAP(CHstSlotInfoDlg)
  ON_CBN_SELCHANGE(IDC_TAG, OnSelchangeTag)
  ON_BN_CLICKED(IDC_APPLY, OnApply)
  ON_BN_CLICKED(IDC_SELALL, OnSelall)
  ON_BN_CLICKED(IDC_EDIT, OnEdit)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_COMBOBOXMSG, OnComboBoxMsg)
  ON_UPDATE_COMMAND_UI(IDC_APPLY, OnUpdateApply)
  ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateEdit)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_REC, IDC_DRV2, OnApplyChanged)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::BuildList()
  {
  m_TagList.ResetContent();
  ASSERT(pHistory);
  for (long i=0; i<pHistory->Data.lSlotsUsed; i++)
    {
    pCSlot pSlot = pHistory->Slots[i];
    if ( (m_SelAll) ||
         ( (m_Drv==0 || (m_Drv==1 && pSlot->Data.bDriver) || (m_Drv==2 && !pSlot->Data.bDriver)) &&
           (m_Rec==0 || (m_Rec==1 && pSlot->Data.bRecordIt) || (m_Rec==2 && !pSlot->Data.bRecordIt)) &&
           (m_Flt==0 || (m_Flt==1 && pSlot->Data.bUseFilter) || (m_Flt==2 && !pSlot->Data.bUseFilter)) &&
           (m_Cmp==0 || (m_Cmp==1 && !pSlot->Data.bUseBoxcar) || (m_Cmp==2 && !pSlot->Data.bUseBoxcar)) ) )
      m_TagList.AddString(pHistory->Slots[i]->Tag());
    }
  i = 0;
  if (sFirstTag.GetLength()>0)
    {
    i = m_TagList.FindStringExact(0, (const char*)sFirstTag);
    if (i<0)
      i = 0;
    }
  m_TagList.SetCurSel(i);
  OnSelchangeTag();
  m_TagList.UpdateButtons();
  //FilterChanged(False);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

BOOL CHstSlotInfoDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();
  if (!m_TagList.SubclassPrevNextControls(this, IDC_TAG, IDC_PREV, IDC_NEXT))
    TRACE("Failed to subclass combo box.\n");
  BuildList();
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::Enable()
  {
  GetDlgItem(IDC_REC)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_REC2)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_REC3)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_CMP)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_CMP2)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_CMP3)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_FLT)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_FLT2)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_FLT3)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_DRV)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_DRV2)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_DRV3)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_TXT_OPTIONS)->EnableWindow(!m_SelAll);
  GetDlgItem(IDC_REC)->Invalidate();
  GetDlgItem(IDC_CMP)->Invalidate();
  GetDlgItem(IDC_FLT)->Invalidate();
  GetDlgItem(IDC_DRV)->Invalidate();
  }

//---------------------------------------------------------------------------

LRESULT CHstSlotInfoDlg::OnComboBoxMsg(WPARAM wParam, LPARAM lParam)
  {
  UINT BoxID = (UINT)wParam;
  UINT ButtonID = (UINT)LOWORD(lParam);
  UINT BoxCmd = (UINT)HIWORD(lParam);
  switch (BoxCmd)
    {
    case CBN_SELCHANGE: OnSelchangeTag(); break;
    }
  return True;
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnSelchangeTag() 
  {
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0 && pHistory->Data.lSlotsUsed>0)
    {
    CString Txt;
    m_TagList.GetLBText(i, Txt);
    long SlotIndex = pHistory->GetSlot((pchar)(const char*)Txt);
    if (SlotIndex>=0)
      {
      pCSlot pSlot = pHistory->Slots[SlotIndex];
      SlotCfgInfo SCI;
      pSlot->GetCfgInfo(SCI);
      m_Desc = SCI.mDesc;
      m_Eng = SCI.mEng;
      m_Type = tt_TypeString(SCI.mTyp);
      m_Source = (SCI.mDriver ? "Driver" : "Other");
      m_Recording = (SCI.mRecordIt ? "Yes" : "No");
      if (pHistory->dwStatsCnt>0.0)
        {
        m_RecFreq = floor((double)pSlot->dwWriteCnt / pHistory->dwStatsCnt + 0.5) * 100.0;
        m_ChgFreq = floor((double)pSlot->dwChangeCnt / pHistory->dwStatsCnt + 0.5) * 100.0;
        }
      else
        {
        m_RecFreq = 0;
        m_ChgFreq = 0;
        }
      }
    }
  Enable();
  UpdateDialogControls(this, FALSE);
  UpdateData(False);
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnApply() 
  {
  UpdateData(True);
  BuildList();
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnUpdateApply(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(bFilterChanged && pHistory->Data.lSlotsUsed>0);
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnApplyChanged(UINT nID) 
  {
  FilterChanged();
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::FilterChanged(flag HasChanged)
  {
  bFilterChanged = HasChanged;
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnSelall() 
  {
  UpdateData(True);
  FilterChanged();
  Enable();
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnUpdateEdit(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(m_TagList.GetCurSel()>=0 && pHistory->Data.lSlotsUsed>0);
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnEdit() 
  {
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_TagList.GetLBText(i, Txt);
    if (pHistory->SlotInfoDlg((pchar)(const char*)Txt, 1))
      OnSelchangeTag();
    }
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnOK() 
  {//close button...
  UpdateData(True);
  int i = m_TagList.GetCurSel();
  if (i>=0)
    {
    CString Txt;
    m_TagList.GetLBText(i, Txt);
    CProfINIFile PF(PrjIniFile());
    PF.WrStr("Historian", "PrevBrowseTag", (char*)(const char*)Txt);
    }
  CDialog::OnOK();
  }

//---------------------------------------------------------------------------

void CHstSlotInfoDlg::OnCancel() 
  {
  OnOK();
  //CDialog::OnCancel();
  }

//===========================================================================
#endif
