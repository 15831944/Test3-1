//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "mdlcfg.h"
#include "sqszbase.h"
#include "SqSzPPg.h"
#include "scd_wm.h"
#include ".\sqszppg.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

CSqSzPPg::CSqSzPPg(CMdlCfgSheet * Sheet)
	: CMdlCfgBase(CSqSzPPg::IDD, Sheet)
  {
	//{{AFX_DATA_INIT(CSqSzPPg)
	m_SeriesData = "0.0";
	m_SeriesDataMin = "0.0";
	m_SeriesDataMax = "0.0";
	m_SeriesQMidPt = FALSE;
	//}}AFX_DATA_INIT
  pSerCnv = NULL;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSqSzPPg)
  DDX_Control(pDX, IDC_SER_SIZES, m_Sizes);
  DDX_Control(pDX, IDC_SER_NAME, m_SeriesNames);
  DDX_Control(pDX, IDC_SER_UNITS, m_Units);
  DDX_Text(pDX, IDC_SER_DATA, m_SeriesData);
  DDX_Text(pDX, IDC_SER_QMIN, m_SeriesDataMin);
  DDX_Text(pDX, IDC_SER_QMAX, m_SeriesDataMax);
  DDX_Check(pDX, IDC_SER_QMIDPT, m_SeriesQMidPt);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_SER_DATA_TYPE, mSerDataType);
  DDX_Control(pDX, IDC_SER_QCREATE, m_CreateBtn);
  DDX_Control(pDX, IDC_SER_QMIDPT, m_QMidPtBtn);
  DDX_Control(pDX, IDC_SER_DATA, m_DataBtn);
  DDX_Control(pDX, IDC_DATA_NAME, m_DataNameTxt);
  DDX_Control(pDX, IDC_SER_ADDSIZE, m_AddBtn);
  DDX_Control(pDX, IDC_SER_REMOVESIZE, m_RemoveBtn);
  DDX_Control(pDX, IDC_SER_SIZES_VIEW, m_DisplaySizes);
  DDX_Control(pDX, IDC_SPIN_DECIMALS, m_SpinDecimalsBtn);
  DDX_Control(pDX, IDC_EDIT2, m_SpinTxt);
  DDX_Control(pDX, IDC_EDIT_NINTS, m_EditNInts);
  DDX_Control(pDX, IDC_SER_QMIN, m_EditMin);
  DDX_Control(pDX, IDC_SER_QMAX, m_EditMax);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSqSzPPg, CMdlCfgBase)
	//{{AFX_MSG_MAP(CSqSzPPg)
	ON_CBN_SELCHANGE(IDC_SER_NAME, OnSelchangeSerName)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SER_SIZES, OnEndlabeleditSerSizes)
	ON_NOTIFY(NM_DBLCLK, IDC_SER_SIZES, OnDblclkSerSizes)
	ON_BN_CLICKED(IDC_SER_ADDSIZE, OnSerAddsize)
	ON_BN_CLICKED(IDC_SER_REMOVESIZE, OnSerRemovesize)
	ON_CBN_SELCHANGE(IDC_SER_UNITS, OnSelchangeSerUnits)
	ON_CBN_EDITCHANGE(IDC_SER_NAME, OnEditchangeSerName)
	ON_BN_CLICKED(IDC_SER_ADDSER, OnSerAddSeries)
	ON_BN_CLICKED(IDC_SER_REMOVESER, OnSerRemoveSeries)
	ON_EN_CHANGE(IDC_SER_Q, OnChangeSerData)
	ON_EN_CHANGE(IDC_SER_QMIN, OnChangeSerDatamin)
	ON_EN_CHANGE(IDC_SER_QMAX, OnChangeSerDatamax)
	ON_BN_CLICKED(IDC_SER_QMIDPT, OnSerQmidpt)
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_ONUPDATE, OnDoSomething)
  ON_UPDATE_COMMAND_UI(IDC_SER_REMOVESER, OnUpdateSerRemoveSeries)
  ON_UPDATE_COMMAND_UI(IDC_SER_QCREATE, OnUpdateSerQCreate)
  ON_CBN_SELCHANGE(IDC_SER_DATA_TYPE, OnCbnSelchangeSerDataType)
  ON_BN_CLICKED(IDC_SER_TO_MANUAL, OnBnClickedSerToManual)
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DECIMALS, OnDeltaposSpinDecimals)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CSqSzPPg::OnInitDialog() 
  {
	CMdlCfgBase::OnInitDialog();
  m_Sizes.InsertColumn(0, "Size", LVCFMT_LEFT, 90);
  CurSeriesIndex = -1;
  pSerCnv = NULL;

  m_SpinDecimalsBtn.SetRange(0,3);

  PopulateAll();
  bInited=true;

	return TRUE;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::PopulateAll()
  {
  /* Available Vars - For checking that species exist etc.
  flag            bInited;
  CMdlCfgSheet *  pSheet;
  
  CProfINIFile  & Cfg;
  Strng_List    & Species;
  Strng         & SDBDef;
  Strng         & SDBCfg;
  CSArray       & FlashSpecies;
  SpMdlArray    & SpModels;
  CDllInfoArray & DLLs;
  */
  H.Load(Cfg); //loads all size distribution info from cfg ini file

  m_SeriesNames.ResetContent();
  for (int i=0; i<H.SSs.GetSize(); i++)
    m_SeriesNames.AddString(H.SSs[i]->m_Name());

  m_Units.ResetContent();
  CDataCnv* pC = gs_CnvsMngr.SICnv(DC_L);
  while (pC)
    {
    m_Units.AddString(pC->Txt());
    pC = pC->Next();
    }

  if (m_SeriesNames.GetCount()>0)
    {
    CurSeriesIndex = Range(0, CurSeriesIndex, m_SeriesNames.GetCount());
    m_SeriesNames.SetCurSel(CurSeriesIndex);
    CurSeriesIndex = -1;
    OnSelchangeSerName();
    }
  else
    CurSeriesIndex = -1;


  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

int CSqSzPPg::PopulateSizes(BOOL DoSort, int SetSelIndex)
  {
  int iPos = -1;
  if (CurSeriesIndex>=0)
    {
    m_Sizes.LockWindowUpdate();
    CSieveSeriesHelper& SSH = *(H.SSs[CurSeriesIndex]);
    if (DoSort)
      SSH.SortSizes();

    int PrevSel = -1;
    CString PrevTxt;
    if (SetSelIndex<0)
      {
      PrevSel = m_Sizes.GetFirstSelectedItem();
      if (PrevSel>=0)
        PrevTxt = m_Sizes.GetItemText(PrevSel, 0);
      }
    int PrevScrollPos = m_Sizes.GetScrollPos(SB_VERT);
    if (PrevScrollPos>0)
      {
      CSize sz = m_Sizes.ApproximateViewRect();
      PrevScrollPos = PrevScrollPos*sz.cy/m_Sizes.GetItemCount();
      }

    m_Sizes.DeleteAllItems();
    CString S;
    for (int i=0; i<SSH.ManualSizes.GetSize(); i++)
      {
      SSH.FormatSize(SSH.ManualSizes[i], S);
      m_Sizes.InsertItem(i, S);
      }

    //set new selection...
    if (SetSelIndex>=0)
      {
      iPos = Min(SetSelIndex, m_Sizes.GetItemCount());
      }
    else
      {
      if (PrevSel>=0)
        iPos = m_Sizes.FindStringExact(-1, PrevTxt);
      }
    if (PrevScrollPos>0)
      {
      //m_Sizes.SetScrollPos(SB_VERT, PrevScrollPos);
      CSize sz(0, PrevScrollPos);
      m_Sizes.Scroll(sz);
      }
    if (iPos>=0)
      {
      m_Sizes.SetItemState(-1, 0, LVIS_SELECTED);
      m_Sizes.SetItemState(iPos, LVIS_SELECTED, LVIS_SELECTED);
      m_Sizes.EnsureVisible(iPos, false);
      }
    m_Sizes.UnlockWindowUpdate();

    // Manual Entry so Update Display as Well
    //PopulateDisplaySizes();

    char s[80];
    sprintf(s, "%d", SSH.ManualSizes.GetSize());
    m_EditNInts.SetWindowText(s);
    }

  return iPos;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::PopulateDisplaySizes(void)
  {
  if (CurSeriesIndex>=0)
    {
    m_DisplaySizes.ResetContent();
    CSieveSeriesHelper& SSH = *(H.SSs[CurSeriesIndex]);
    CString S;
    for (int i=0; i<SSH.CalculatedSizes.GetSize(); i++)
      {
      SSH.Precision = m_SpinDecimalsBtn.GetPos32();
      SSH.FormatSize(SSH.CalculatedSizes[i], S);
      m_DisplaySizes.AddString(S);
      }

     char s[80];
     sprintf(s,"%d",SSH.CalculatedSizes.GetSize());
     m_EditNInts.SetWindowText(s);
     }

  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnOK() 
  {
	CMdlCfgBase::OnOK();
  }

//---------------------------------------------------------------------------

BOOL CSqSzPPg::OnKillActive() 
  {
	BOOL OK=CMdlCfgBase::OnKillActive();
  if (bInited)
    {
    H.Save(Cfg);
    }  
  return OK;
  }

//---------------------------------------------------------------------------

BOOL CSqSzPPg::OnSetActive()
  {
	BOOL OK=CMdlCfgBase::OnSetActive();
  if (bInited)
    {
    PopulateAll();
    }
  return OK;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSelchangeSerName()
  {
  int j = m_SeriesNames.GetCurSel();
  if (CurSeriesIndex!=j)
    {
    if (j!=CB_ERR)
      {
      UpdateData(TRUE);
      pSerCnv = gs_CnvsMngr.FindSecCnv(DC_L, H.SSs[j]->Units());
      if (pSerCnv==NULL)
        pSerCnv = gs_CnvsMngr.SICnv(DC_L);
      H.SSs[j]->Units = pSerCnv->Txt();
      m_Units.SelectString(-1, H.SSs[j]->Units());

      CurSeriesIndex = j;
      mSerDataType.SetCurSel( H.SSs[j]->m_SeriesDataType );

      if (H.SSs[j]->m_SeriesDataType == e_Arithmetic)
        // Arithmetic increment needs to take into account the units.
        m_SeriesData.Format("%f", pSerCnv->Human(H.SSs[j]->m_Data));
      else
        m_SeriesData.Format("%f", H.SSs[j]->m_Data);

      m_SeriesDataMin.Format("%f", pSerCnv->Human(H.SSs[j]->m_DataMin));
      m_SeriesDataMax.Format("%f", pSerCnv->Human(H.SSs[j]->m_DataMax));
      m_SeriesQMidPt = H.SSs[j]->m_bQMidPt;

      SetPrecision(H.SSs[j]->Precision);

      OnCbnSelchangeSerDataType();

      UpdateData(FALSE);
      }
    CurSeriesIndex = j;
    if ( H.SSs[CurSeriesIndex]->m_SeriesDataType == e_Manual )
        PopulateSizes(FALSE, -1);
    else
        PopulateDisplaySizes();
  }

  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnEditchangeSerName() 
  {//change name of series...
  if (CurSeriesIndex>=0)
    {
    UpdateData(TRUE);
    CString S;
    m_SeriesNames.GetWindowText(S);
    if (S.GetLength()>0 && strcmp((const char*)S, H.SSs[CurSeriesIndex]->m_Name())!=0)
      {
      H.SSs[CurSeriesIndex]->m_Name = (const char*)S;
      WORD pos = LOWORD(m_SeriesNames.GetEditSel());
      m_SeriesNames.ResetContent();
      for (int i=0; i<H.SSs.GetSize(); i++)
        m_SeriesNames.AddString(H.SSs[i]->m_Name());
      m_SeriesNames.SetCurSel(CurSeriesIndex);
      m_SeriesNames.SetEditSel(pos, pos);
      }
    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSelchangeSerUnits() 
  {
  if (CurSeriesIndex>=0)
    {
    int j = m_Units.GetCurSel();
    if (j!=CB_ERR)
      {
      if (pSerCnv==NULL)
        {
        pSerCnv = gs_CnvsMngr.SICnv(DC_L);
        H.SSs[CurSeriesIndex]->Units = pSerCnv->Txt();
        }
      CString s;
      m_Units.GetLBText(j, s);
      if (strcmp((const char*)s, pSerCnv->Txt())!=0)
        {
        H.SSs[CurSeriesIndex]->ChangeUnits((char*)(const char*)s);
        pSerCnv = gs_CnvsMngr.FindSecCnv(DC_L, H.SSs[CurSeriesIndex]->Units());
        //m_Units.SelectString(-1, H.SSs[CurSeriesIndex]->Units());
        if ( H.SSs[CurSeriesIndex]->m_SeriesDataType == e_Manual )
            PopulateSizes(FALSE, -1);
        else
            PopulateDisplaySizes();

        if (H.SSs[CurSeriesIndex]->m_SeriesDataType == e_Arithmetic)
          // Arithmetic increment needs to take into account the units.
          m_SeriesData.Format("%f", pSerCnv->Human(H.SSs[CurSeriesIndex]->m_Data));

        m_SeriesDataMin.Format("%f", pSerCnv->Human(H.SSs[CurSeriesIndex]->m_DataMin));
        m_SeriesDataMax.Format("%f", pSerCnv->Human(H.SSs[CurSeriesIndex]->m_DataMax));

        SetPrecision(H.SSs[CurSeriesIndex]->Precision);
 
        SeriesCreate();

        UpdateData(FALSE);
        }
      }
    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnChangeSerData() 
  {
  if (CurSeriesIndex>=0)
    {
    double PrevVal = H.SSs[CurSeriesIndex]->m_Data;
    UpdateData(TRUE);

    if (H.SSs[CurSeriesIndex]->m_SeriesDataType == e_Arithmetic)
      // Arithmetic increment needs to take into account the units.
      H.SSs[CurSeriesIndex]->m_Data = pSerCnv->Normal(SafeAtoF((const char*)m_SeriesData));
    else
      H.SSs[CurSeriesIndex]->m_Data = SafeAtoF((const char*)m_SeriesData);

    //if (fabs(H.SSs[CurSeriesIndex]->m_Data-PrevVal)>1.0e-18)
      {
      SeriesCreate();
      UpdateData(FALSE);
      UpdateDialogControls(this, FALSE);
      }
    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnChangeSerDatamin() 
  {
  if (CurSeriesIndex>=0)
    {
    UpdateData(TRUE);
    H.SSs[CurSeriesIndex]->m_DataMin = pSerCnv->Normal(SafeAtoF((const char*)m_SeriesDataMin));
    SeriesCreate();
    UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnChangeSerDatamax() 
  {
  if (CurSeriesIndex>=0)
    {
    UpdateData(TRUE);
    H.SSs[CurSeriesIndex]->m_DataMax = pSerCnv->Normal(SafeAtoF((const char*)m_SeriesDataMax));
    SeriesCreate();
    UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSerQmidpt() 
  {
  if (CurSeriesIndex>=0)
    {
    UpdateData(TRUE);
    H.SSs[CurSeriesIndex]->m_bQMidPt = (m_SeriesQMidPt!=0);

    SeriesCreate();

    UpdateData(FALSE);

    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSerQCreate() 
  {

//
// Turn this into XFER to Manual
//

  }

//---------------------------------------------------------------------------

void CSqSzPPg::SeriesCreate() 
  {

  // Number of decimal places to round to
  int n = m_SpinDecimalsBtn.GetPos32();

  if (CurSeriesIndex>=0)
    {

  int Selection = mSerDataType.GetCurSel();

  switch (Selection)
    {
    case e_Manual: // Manual
 
      // The artifcat of chaning type will reset the manual sizes
      // to those of the last type.
      // TO DO: We need a Manual Buffer that we can restore.
      PopulateSizes(FALSE, -1);
      break;
    case e_Q: // Q
      H.SSs[CurSeriesIndex]->CreateSeriesFromQ();
      PopulateDisplaySizes();
      break;
    case e_Sqrt: // sqrt(2)
      H.SSs[CurSeriesIndex]->CreateSeriesSQRT();
      PopulateDisplaySizes();
      break;
    case e_Ratio: // Ratio
      H.SSs[CurSeriesIndex]->CreateSeriesFromRatio();
      PopulateDisplaySizes();
      break;
    case e_Arithmetic: // Arithmetic Increment
      H.SSs[CurSeriesIndex]->CreateSeriesArithmetic();
      PopulateDisplaySizes();
      break;

    }

    }
  }

void CSqSzPPg::OnUpdateSerQCreate(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(SafeAtoF((const char*)m_SeriesData)>0.0);
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnEndlabeleditSerSizes(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  CEdit* pEdit = m_Sizes.GetEditControl();
  int j=m_Sizes.GetFirstSelectedItem();
  if (pEdit && j>=0)
    {
    CString S;
    pEdit->GetWindowText(S);
    double PrevVal = pSerCnv->Human(H.SSs[CurSeriesIndex]->ManualSizes[j]);
    double d = pSerCnv->Normal(SafeAtoF((const char*)S, PrevVal));
    H.SSs[CurSeriesIndex]->FormatSize(d, S);
    m_Sizes.SetItemText(j, 0, S);
    if (d!=PrevVal)
      {
      H.SSs[CurSeriesIndex]->ManualSizes[j] = d;
      PopulateSizes(TRUE, -1);
      }
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnDblclkSerSizes(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int j=m_Sizes.GetFirstSelectedItem();
	if (j>=0)
    {
    //m_Sizes.SetItemState(-1, 0, LVIS_SELECTED);
    //m_Sizes.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
    //m_Sizes.EnsureVisible(j, false);
    m_Sizes.EditLabel(j);	
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSerAddsize() 
  {
  if (CurSeriesIndex>=0)
    {
    int j = m_Sizes.GetFirstSelectedItem();
    if (j<0)
      j = H.SSs[CurSeriesIndex]->ManualSizes.GetUpperBound()-1;
    if (j<0)
      H.SSs[CurSeriesIndex]->ManualSizes.InsertAt(0, 0.0005);
    else
      H.SSs[CurSeriesIndex]->ManualSizes.InsertAt(j, H.SSs[CurSeriesIndex]->ManualSizes[j]);
    j = PopulateSizes(TRUE, j+1);
    if (j>=0)
      {
      m_Sizes.SetFocus();
      m_Sizes.EditLabel(j);	
      //PostMessage(WMU_ONUPDATE, j, 1L);
      }
    }
  }

//---------------------------------------------------------------------------

LRESULT CSqSzPPg::OnDoSomething(WPARAM wParam, LPARAM lParam)
  {
  switch (lParam)
    {
    case 1: 
      m_Sizes.SetFocus();
      m_Sizes.EditLabel((int)wParam);
      break;
    case 2: 
      m_Sizes.SetItemState(-1, 0, LVIS_SELECTED);
      m_Sizes.SetItemState((int)wParam, LVIS_SELECTED, LVIS_SELECTED);
      m_Sizes.EnsureVisible((int)wParam, false);
      break;
    case 3: 
      m_Sizes.SetFocus();
      Sleep(10);
      PostMessage(WMU_ONUPDATE, wParam, 2L);
      Sleep(10);
      PostMessage(WMU_ONUPDATE, wParam, 4L);
      break;
    case 4:
      Sleep(10);
      PostMessage(WMU_ONUPDATE, wParam, 1L);
      break;
    default:;
    }
  return True;
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSerRemovesize() 
  {
  if (CurSeriesIndex>=0)
    {
    int j = m_Sizes.GetFirstSelectedItem();
    if (j>=0)
      {
      H.SSs[CurSeriesIndex]->ManualSizes.RemoveAt(j);
      PopulateSizes(TRUE, j);
      }
    }
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSerAddSeries()
  {
  int index = H.CreateDefaultSS();
  index = m_SeriesNames.AddString(H.SSs[index]->m_Name());
  m_SeriesNames.SetCurSel(index);
  OnSelchangeSerName();
  }

//---------------------------------------------------------------------------

void CSqSzPPg::OnSerRemoveSeries()
  {
  //todo...
  }

void CSqSzPPg::OnUpdateSerRemoveSeries(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(false);
  }


//---------------------------------------------------------------------------

void CSqSzPPg::OnCbnSelchangeSerDataType()
  {
  //
  // Change the Data Window Entry Depending on Data Entry Type
  // 

  int Selection = mSerDataType.GetCurSel();

  if ( CurSeriesIndex >= 0 )
    {
    H.SSs[CurSeriesIndex]->m_SeriesDataType = (SeriesDataType)Selection;
    m_SpinTxt.EnableWindow(false);
    m_EditNInts.EnableWindow(false);


    switch (Selection)
      {
      case e_Manual: // Manual
        m_Sizes.ShowWindow(true);
        m_Sizes.EnableWindow(true);
        m_CreateBtn.ShowWindow(false);
        m_QMidPtBtn.ShowWindow(false);
        m_DataBtn.ShowWindow(false);
        m_DataNameTxt.ShowWindow(false);
        m_AddBtn.ShowWindow(true);
        m_AddBtn.EnableWindow(true);
        m_RemoveBtn.ShowWindow(true);
        m_RemoveBtn.EnableWindow(true);
        m_DisplaySizes.ShowWindow(false);
        m_SpinDecimalsBtn.EnableWindow(false);
        m_EditMin.EnableWindow(false);
        m_EditMax.EnableWindow(false);

        break;
      case e_Q: // Q
        m_Sizes.ShowWindow(false);
        m_CreateBtn.ShowWindow(true);
        m_CreateBtn.EnableWindow(true);
        m_QMidPtBtn.ShowWindow(true);
        m_DataBtn.ShowWindow(true);
        m_DataNameTxt.SetWindowText("Q");
        m_DataNameTxt.ShowWindow(true);
        m_AddBtn.ShowWindow(false);
        m_RemoveBtn.ShowWindow(false);
        m_DisplaySizes.EnableWindow(true);
        m_DisplaySizes.ShowWindow(true);
        m_SpinDecimalsBtn.EnableWindow(true);
        m_EditMin.EnableWindow(true);
        m_EditMax.EnableWindow(true);

        break;
      case e_Sqrt: // sqrt(2)
        m_Sizes.ShowWindow(false);
        m_CreateBtn.ShowWindow(true);
        m_CreateBtn.EnableWindow(true);
        m_QMidPtBtn.ShowWindow(false);
        m_DataBtn.ShowWindow(false);
        m_DataNameTxt.ShowWindow(false);
        m_AddBtn.ShowWindow(false);
        m_RemoveBtn.ShowWindow(false);
        m_DisplaySizes.EnableWindow(true);
        m_DisplaySizes.ShowWindow(true);
        m_SpinDecimalsBtn.EnableWindow(true);
        m_EditMin.EnableWindow(true);
        m_EditMax.EnableWindow(true);

        break;
      case e_Ratio: // Ratio
        m_Sizes.ShowWindow(false);
        m_CreateBtn.ShowWindow(true);
        m_CreateBtn.EnableWindow(true);
        m_QMidPtBtn.ShowWindow(false);
        m_DataBtn.ShowWindow(true);
        m_DataNameTxt.SetWindowText("Ratio");
        m_DataNameTxt.ShowWindow(true);
        m_AddBtn.ShowWindow(false);
        m_RemoveBtn.ShowWindow(false);
        m_DisplaySizes.EnableWindow(true);
        m_DisplaySizes.ShowWindow(true);
        m_DisplaySizes.EnableWindow(true);
        m_DisplaySizes.ShowWindow(true);
        m_SpinDecimalsBtn.EnableWindow(true);
        m_EditMin.EnableWindow(true);
        m_EditMax.EnableWindow(true);

        break;
      case e_Arithmetic: // Arithmetic Increment
        m_Sizes.ShowWindow(false);
        m_CreateBtn.ShowWindow(true);
        m_CreateBtn.EnableWindow(true);
        m_QMidPtBtn.ShowWindow(false);
        m_DataBtn.ShowWindow(true);
        m_DataNameTxt.SetWindowText("Increment");
        m_DataNameTxt.ShowWindow(true);
        m_AddBtn.ShowWindow(false);
        m_RemoveBtn.ShowWindow(false);
        m_DisplaySizes.EnableWindow(true);
        m_DisplaySizes.ShowWindow(true);
        m_SpinDecimalsBtn.EnableWindow(true);
        m_EditMin.EnableWindow(true);
        m_EditMax.EnableWindow(true);

        break;

      }

      SeriesCreate();

      UpdateData(FALSE);
    }
  }

void CSqSzPPg::OnBnClickedSerToManual()
  {
  if ( CurSeriesIndex >= 0 )
    {

    // Transfer Current Size Data to Manual Edit List

    CSieveSeriesHelper& SSH = *(H.SSs[CurSeriesIndex]);
    CString S;
    SSH.ManualSizes.SetSize( SSH.CalculatedSizes.GetSize() );
    for (int i=0; i<SSH.CalculatedSizes.GetSize(); i++)
      {
      SSH.ManualSizes[i] = SSH.CalculatedSizes[i];
      }


    PopulateSizes(FALSE, -1);

    // Change the SeriesDataType to Manual
    H.SSs[CurSeriesIndex]->m_SeriesDataType = e_Manual;
    mSerDataType.SetCurSel(e_Manual);

    // Change the display
    OnCbnSelchangeSerDataType();

    }

  }

void CSqSzPPg::SetPrecision(long n)
  {
  m_SeriesDataPrecision = n;
  m_SpinDecimalsBtn.SetPos32(n);
  char s[5];
  sprintf(s, "%d", m_SeriesDataPrecision);
  m_SpinTxt.SetWindowText(s);
  }

void CSqSzPPg::UpdatePrecision(void)
  {
  if ( CurSeriesIndex >= 0 )
    {
    //
    // Could not get spin control to automatically link to EditBox text
    // so I set here manually instead
    //
    UpdateData(TRUE);

    char s[5];
    m_SeriesDataPrecision = m_SpinDecimalsBtn.GetPos32();
    sprintf(s, "%d", m_SeriesDataPrecision);
    m_SpinTxt.SetWindowText(s);

    H.SSs[CurSeriesIndex]->Precision = m_SeriesDataPrecision;

    SeriesCreate();

    UpdateData(FALSE);
    }
  }

void CSqSzPPg::OnDeltaposSpinDecimals(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

  *pResult = 0;

  UpdatePrecision();
  }


//===========================================================================
//
// Distribution Tab Page
//
//===========================================================================


CSqSzDPg::CSqSzDPg(CMdlCfgSheet * Sheet)
	: CMdlCfgBase(CSqSzDPg::IDD, Sheet)
  {
	//{{AFX_DATA_INIT(CSqSzDPg)
	m_DstUnits = _T("");
	m_DstTSize = 0.0;
	m_DstBSize = 0.0;
	m_DstTSizeDisp = 0.0;
	m_DstBSizeDisp = 0.0;
	//}}AFX_DATA_INIT
  pDstSerCnv = NULL;
  }

//---------------------------------------------------------------------------

void CSqSzDPg::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSqSzDPg)
  DDX_Control(pDX, IDC_DST_NAMECOMBO, m_DstNames);
  DDX_Control(pDX, IDC_DST_DEFSPECIE, m_DstDefSp);
  DDX_Control(pDX, IDC_DST_SPECIES, m_SpList);
  DDX_Control(pDX, IDC_DST_SERIES, m_DstSeries);
  DDX_Text(pDX, IDC_DST_UNITS, m_DstUnits);
  DDX_Text(pDX, IDC_DST_TSIZE, m_DstTSize);
  DDX_Text(pDX, IDC_DST_BSIZE, m_DstBSize);
  DDX_Text(pDX, IDC_DST_TSIZEDISP, m_DstTSizeDisp);
  DDX_Text(pDX, IDC_DST_BSIZEDISP, m_DstBSizeDisp);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSqSzDPg, CMdlCfgBase)
  //{{AFX_MSG_MAP(CSqSzDPg)
  ON_CBN_SELCHANGE(IDC_DST_SERIES, OnSelChangeDstSeries)
  ON_NOTIFY(NM_DBLCLK, IDC_DST_SPECIES, OnDblclkSpecies)
  ON_CBN_SELCHANGE(IDC_DST_NAMECOMBO, OnSelChangeDstNames)
	ON_BN_CLICKED(IDC_DST_ADDDIST, OnDstAddDist)
	ON_BN_CLICKED(IDC_DST_REMOVEDIST, OnDstRemoveDist)
	ON_CBN_EDITCHANGE(IDC_DST_NAMECOMBO, OnEditchangeDstName)
	//}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(IDC_DST_REMOVEDIST, OnUpdateDstRemoveDist)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CSqSzDPg::OnInitDialog() 
  {
	CMdlCfgBase::OnInitDialog();
  m_SpList.InsertColumn(0, "Specie", LVCFMT_LEFT, 140);
  CBitmap BM;                                           
  VERIFY(BM.LoadBitmap(IDB_LISTIMGS));
  VERIFY(ImgList.Create(12, 12, FALSE, 0, 3));
  VERIFY(ImgList.Add(&BM, (CBitmap*)NULL)>=0);
  m_SpList.SetImageList(&ImgList, LVSIL_SMALL);
  CurDstIndex = -1;
  PopulateAll();
  bInited=true;
	return TRUE;
  }

//---------------------------------------------------------------------------

void CSqSzDPg::PopulateAll()
  {
  /* Available Vars - For checking that species exist etc.
  flag            bInited;
  CMdlCfgSheet *  pSheet;
  
  CProfINIFile  & Cfg;
  Strng_List    & Species;
  Strng         & SDBDef;
  Strng         & SDBCfg;
  CSArray       & FlashSpecies;
  SpMdlArray    & SpModels;
  CDllInfoArray & DLLs;
  */
  H.Load(Cfg); //loads all size distribution info from cfg ini file

  m_DstNames.ResetContent();
  if (H.SDs.GetSize()==0)
    H.CreateDefaultSD();
  for (int i=0; i<H.SDs.GetSize(); i++)
    m_DstNames.AddString(H.SDs[i]->Name());
  
  if (m_DstNames.GetCount()>0)
    {
    CurDstIndex = Range(0, CurDstIndex, m_DstNames.GetCount());
    m_DstNames.SetCurSel(CurDstIndex);
    CurDstIndex = -1;
    OnSelChangeDstNames();
    }
  else
    CurDstIndex = -1;
  ASSERT_RDB(CurDstIndex>=0, "One MUST exist!", __FILE__, __LINE__);

  BuildSpList();
  CurDefSpecie = H.SDs[0]->DefaultSpecie();

  UpdateData(FALSE);
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSqSzDPg::PopulateDistData()
  {
  if (CurDstIndex>=0)
    {
    CSizeDistHelper* pSDist = H.SDs[CurDstIndex];
    pDstSerCnv = NULL;
    CurDstSeriesIndex = -1;
    m_DstSeries.ResetContent();
    for (int i=0; i<H.SSs.GetSize(); i++)
      m_DstSeries.AddString(H.SSs[i]->m_Name());
    int DstSeriesIndex = 0;
    DstSeriesIndex = m_DstSeries.FindStringExact(-1, pSDist->SieveSeriesName());
    if (DstSeriesIndex<0)
      DstSeriesIndex = 0;
    m_DstSeries.SetCurSel(DstSeriesIndex);
    OnSelChangeDstSeries();

    m_DstTSize = pDstSerCnv ? pDstSerCnv->Human(pSDist->dTopSize) : pSDist->dTopSize;
    m_DstBSize = pDstSerCnv ? pDstSerCnv->Human(pSDist->dBottomSize) : pSDist->dBottomSize;
    m_DstTSizeDisp = pDstSerCnv ? pDstSerCnv->Human(pSDist->dTopSizeDisplay) : pSDist->dTopSizeDisplay;
    m_DstBSizeDisp = pDstSerCnv ? pDstSerCnv->Human(pSDist->dBottomSizeDisplay) : pSDist->dBottomSizeDisplay;
    }
  }

//---------------------------------------------------------------------------

void CSqSzDPg::SaveDistData()
  {
  if (CurDstIndex>=0 && H.SDs.GetSize()>0)
    {
    CSizeDistHelper* pSDist = H.SDs[CurDstIndex];
    CString S;
    m_DstNames.GetWindowText(S);
    if (S.GetLength()>0)
      pSDist->Name = (const char*)S;
    CString Sel;
    m_DstSeries.GetLBText(CurDstSeriesIndex, Sel);
    pSDist->SieveSeriesName = (const char*)Sel;
    pSDist->dBottomSize = pDstSerCnv ? pDstSerCnv->Normal(m_DstBSize) : m_DstBSize;
    pSDist->dTopSizeDisplay = pDstSerCnv ? pDstSerCnv->Normal(m_DstTSizeDisp) : m_DstTSizeDisp;
    pSDist->dBottomSizeDisplay = pDstSerCnv ? pDstSerCnv->Normal(m_DstBSizeDisp) : m_DstBSizeDisp;
    }
  }

//---------------------------------------------------------------------------

void CSqSzDPg::BuildSpList()
  {
  m_SpList.LockWindowUpdate();
  m_SpList.DeleteAllItems();
  Strng Tag, Tmp;
  for (int i=0; ; i++)
    {
    Tag.Set("S%04i",i);
    Tmp = Cfg.RdStr("Species", Tag(), "");
    if (Tmp.Len()>2)
      {
      DWORD Type=Tmp.SafeAtoL(-1);
      if (Type==spl_SpecieId)
        {
        int Comma = Tmp.Find(',');
        if (Comma>0)
          {
          Tmp=Tmp.Right(Tmp.Len()-Comma-1);
          Tmp.LRTrim();
          }
        Comma = Tmp.Find(',');
          {
          Tmp=Tmp.Left(Comma);
          Tmp.LRTrim();
          }
        if (Tmp())
          {
          int index = m_Species.FindBySpecieName(Tmp());
          if (index>=0 && m_Species[index].Occ==BOT_Solid)
            m_SpList.InsertItem(m_SpList.GetItemCount(), Tmp(), 0);
          }
        }
      }
    else
      break;
    }
  /*for (i=0; i<Species.GetSize(); i++)
    {
    if (Species[i].Occ==BOT_Solid)
      {
      int n=m_SpList.InsertItem(m_SpList.GetItemCount(), Species[i].Comp(), 0);
      }    
    }*/

  m_DstDefSp.ResetContent();
  if (H.SDs.GetSize()>0)
    {//mark all selected species in list...
    LVFINDINFO FI;
    FI.flags = LVFI_STRING;
    Strng Tmp;
    CSizeDistHelper* pSDist = H.SDs[0];
    for (i=0; i<pSDist->Species.GetSize(); i++)
      {
      Tmp = (pSDist->Species[i])();
      FI.psz = Tmp();
      int iSrcPos = m_SpList.FindItem(&FI, -1);
      if (iSrcPos>=0)
        {
        m_SpList.SetItemImage(iSrcPos, 1);
        }
      }
    }

  m_SpList.UnlockWindowUpdate();
  BuildDefSpList();
  }

//---------------------------------------------------------------------------

void CSqSzDPg::BuildDefSpList()
  {
  m_DstDefSp.ResetContent();
  if (H.SDs.GetSize()>0)
    {
    CSizeDistHelper* pSDist = H.SDs[0];
    for (int i=0; i<m_SpList.GetItemCount(); i++)
      if (m_SpList.GetItemImage(i)==1)
        m_DstDefSp.AddString((const char*)(m_SpList.GetItemText(i, 0)));

    if (m_DstDefSp.GetCount()>0)
      {
      int index = m_DstDefSp.FindStringExact(-1, (const char*)CurDefSpecie);
      m_DstDefSp.SetCurSel(index>=0 ? index : 0);
      if (index<0)
        m_DstDefSp.GetLBText(0, CurDefSpecie);
      }
    }
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnDblclkSpecies(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int i = m_SpList.GetFirstSelectedItem();
  if (i>=0)
    {
    int j = m_SpList.GetItemImage(i);
    m_SpList.SetItemImage(i, (j==0 ? 1 : 0));
    j = m_DstDefSp.GetCurSel();
    if (j>=0)
      m_DstDefSp.GetLBText(j, CurDefSpecie);
    BuildDefSpList();
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnOK() 
  {
	CMdlCfgBase::OnOK();
  }

//---------------------------------------------------------------------------

BOOL CSqSzDPg::OnKillActive() 
  {
	BOOL OK=CMdlCfgBase::OnKillActive();
  if (bInited)
    {
    UpdateData(TRUE);
    if (H.SDs.GetSize()>0)
      {//save specie data
      CSizeDistHelper* pSDist = H.SDs[0];
      pSDist->Species.RemoveAll();
      Strng S;
      for (int i=0; i<m_SpList.GetItemCount(); i++)
        if (m_SpList.GetItemImage(i)==1)
          {
          S = m_SpList.GetItemText(i, 0);
          pSDist->Species.Add(S);
          }
      if (pSDist->Species.GetSize()==0 && m_SpList.GetItemCount()>0)
        {//nothing selected!!! select first!
        S = m_SpList.GetItemText(0, 0);
        pSDist->Species.Add(S);
        CurDefSpecie = S();
        }
      int j = m_DstDefSp.GetCurSel();
      if (j>=0)
        m_DstDefSp.GetLBText(j, CurDefSpecie);
      pSDist->DefaultSpecie = CurDefSpecie;
      }

    SaveDistData();
    H.Save(Cfg);
    }  
  return OK;
  }

//---------------------------------------------------------------------------

BOOL CSqSzDPg::OnSetActive()
  {
	BOOL OK=CMdlCfgBase::OnSetActive();
  if (bInited)
    {
    PopulateAll();
    }
  return OK;
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnSelChangeDstNames() 
  {
  int j = m_DstNames.GetCurSel();
  if (CurDstIndex!=j)
    {
    UpdateData(TRUE);
    SaveDistData();
    CurDstIndex=j;
    PopulateDistData();
    UpdateData(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnSelChangeDstSeries()
  {
  int j = m_DstSeries.GetCurSel();
  if (CurDstSeriesIndex!=j)
    {
    if (j!=CB_ERR)
      {
      UpdateData(TRUE);
      pDstSerCnv = gs_CnvsMngr.FindSecCnv(DC_L, H.SSs[j]->Units());
      if (pDstSerCnv==NULL)
        {
        pDstSerCnv = gs_CnvsMngr.SICnv(DC_L);
        H.SSs[j]->Units = pDstSerCnv->Txt();
        }
      m_DstUnits = H.SSs[j]->Units();
      m_DstTSize = pDstSerCnv->Human(H.SSs[j]->ManualSizes.GetSize()>0 ? H.SSs[j]->ManualSizes[H.SSs[j]->ManualSizes.GetSize()-1] : 0.1);
      UpdateData(FALSE);
      }
    CurDstSeriesIndex = j;
    }
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnEditchangeDstName() 
  {//change name of distribution...
  if (CurDstIndex>=0 && H.SDs.GetSize()>0)
    {
    UpdateData(TRUE);
    CString S;
    m_DstNames.GetWindowText(S);
    if (S.GetLength()>0)
      {
      CSizeDistHelper* pSDist = H.SDs[CurDstIndex];
      pSDist->Name = (const char*)S;
      WORD pos = LOWORD(m_DstNames.GetEditSel());
      m_DstNames.ResetContent();
      for (int i=0; i<H.SDs.GetSize(); i++)
        m_DstNames.AddString(H.SDs[i]->Name());
      m_DstNames.SetCurSel(CurDstIndex);
      m_DstNames.SetEditSel(pos, pos);
      }
    }
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnDstAddDist() 
  {
  UpdateData(TRUE);
  SaveDistData();
  int index = H.CreateDefaultSD();
  index = m_DstNames.AddString(H.SDs[index]->Name());
  m_DstNames.SetCurSel(index);
  OnSelChangeDstNames();
  UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------

void CSqSzDPg::OnDstRemoveDist() 
  {
  if (CurDstIndex>=0 && H.SDs.GetSize()>1)
    {
    H.RemoveDistribution(CurDstIndex);
    CurDstIndex=0;
    m_DstNames.ResetContent();
    for (int i=0; i<H.SDs.GetSize(); i++)
      m_DstNames.AddString(H.SDs[i]->Name());
    m_DstNames.SetCurSel(CurDstIndex);
    PopulateDistData();
    UpdateData(FALSE);
    UpdateDialogControls(this, FALSE);
    }
  }

void CSqSzDPg::OnUpdateDstRemoveDist(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(H.SDs.GetSize()>1);
  }


//===========================================================================
//
// Measurement Tab Page
//
//===========================================================================


CSqSzDMeasPg::CSqSzDMeasPg(CMdlCfgSheet * Sheet)
	: CMdlCfgBase(CSqSzDMeasPg::IDD, Sheet)
  {
	//{{AFX_DATA_INIT(CSqSzDMeasPg)

	//}}AFX_DATA_INIT
 CurDistIndex = 0;
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::DoDataExchange(CDataExchange* pDX)
  {
  CMdlCfgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSqSzDMeasPg)

  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_MEAS_LIST, m_MeasList);
  DDX_Control(pDX, IDC_DISTRIBUTION_SEL, m_DistSel);
  DDX_Control(pDX, IDC_MEAS_NAME, m_EditName);
  DDX_Control(pDX, IDC_SEL_MEAS_TYPE, m_MeasTypeSel);
  DDX_Control(pDX, IDC_COMBO_VAL_UNITS, m_ComboValueUnits);
  DDX_Control(pDX, IDC_COMBO_RES_UNITS, m_ComboResultUnits);
  DDX_Control(pDX, IDC_EDIT_VALUE, m_EditBoxValue);
  DDX_Control(pDX, IDC_BUTTON_MEAS_UP, m_BtnMeasUp);
  DDX_Control(pDX, IDC_BUTTON_MEAS_DOWN, m_BtnMeasDown);
  DDX_Control(pDX, IDC_COUNTFACTOR, m_CountFactor);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSqSzDMeasPg, CMdlCfgBase)
  //{{AFX_MSG_MAP(CSqSzDMeasePg)
	//}}AFX_MSG_MAP
  ON_CBN_SELCHANGE(IDC_DISTRIBUTION_SEL, OnCbnSelchangeDistributionSel)
  ON_BN_CLICKED(IDC_MEAS_ADD, OnBnClickedMeasAdd)
  ON_BN_CLICKED(IDC_MEAS_REMOVE, OnBnClickedMeasRemove)
  ON_EN_CHANGE(IDC_MEAS_NAME, OnEnChangeMeasName)
  ON_LBN_SELCHANGE(IDC_MEAS_LIST, OnLbnSelchangeMeasList)
  ON_CBN_SELCHANGE(IDC_SEL_MEAS_TYPE, OnCbnSelchangeSelMeasType)
  ON_EN_CHANGE(IDC_EDIT_VALUE, OnEnChangeEditValue)
  ON_CBN_SELCHANGE(IDC_COMBO_VAL_UNITS, OnCbnSelchangeComboValUnits)
  ON_CBN_SELCHANGE(IDC_COMBO_RES_UNITS, OnCbnSelchangeComboResUnits)
  ON_BN_CLICKED(IDC_BUTTON_MEAS_UP, OnBnClickedButtonMeasUp)
  ON_BN_CLICKED(IDC_BUTTON_MEAS_DOWN, OnBnClickedButtonMeasDown)
  ON_EN_CHANGE(IDC_COUNTFACTOR, OnEnChangeCountFactor)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CSqSzDMeasPg::OnInitDialog() 
  {
	CMdlCfgBase::OnInitDialog();
  PopulateAll();
  bInited=true;
	return TRUE;
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnOK() 
  {
	CMdlCfgBase::OnOK();
  }
//---------------------------------------------------------------------------

void CSqSzDMeasPg::ValidateNames(void)

  {

   int lNDists = H.SDsMeas.GetCount();

   for (int lDistNo = 0; lDistNo < lNDists; lDistNo++)
     {
     int lNMeas = H.SDsMeas[lDistNo]->Measurements.GetSize();
     for (int m=0; m<lNMeas; m++)
       {
        CSD_Measurement M = H.SDsMeas[lDistNo]->Measurements[m];
      
        Strng NewTag(M.m_sName);
        if (TaggedObject::ValidateTagChanged(NewTag))
          {
            // Name has been changed
            // Silently change the original name
            M.m_sName = NewTag;
            H.SDsMeas[lDistNo]->Measurements[m] = M;
          }

       }
     }

  }

//---------------------------------------------------------------------------

BOOL CSqSzDMeasPg::OnKillActive() 
  {
	BOOL OK=CMdlCfgBase::OnKillActive();
  if (bInited)
    {

      // Check Measurement Names comply with TagName conventions
      ValidateNames();

    gs_CountFactor = dCountFactor;
      H.Save(Cfg);
    }  
  return OK;
  }

//---------------------------------------------------------------------------

BOOL CSqSzDMeasPg::OnSetActive()
  {
	BOOL OK=CMdlCfgBase::OnSetActive();
  if (bInited)
    {

    }
  return OK;
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::PopulateAll(void)
  {
  //Loads all size distribution and measurement info from cfg ini file
  H.Load(Cfg); 

  // Populate the Distribution Pull Down List
  PopulateDistributionList();

  // Populate the measurement list for the distribution 
  PopulateMeasurmentList();

  // Populate the Type List
  m_MeasTypeSel.ResetContent();

  m_MeasTypeSel.AddString("FracPassSize");
  m_MeasTypeSel.AddString("SizePassFrac");
  m_MeasTypeSel.AddString("PPG");
  m_MeasTypeSel.AddString("SAM");
  m_MeasTypeSel.AddString("SAL");
  m_MeasTypeSel.AddString("Text");

  OnLbnSelchangeMeasList();

  dCountFactor = gs_CountFactor;
  char buff[32];
  sprintf(buff, "%f", gs_CountFactor);
  m_CountFactor.SetWindowText(buff);

  UpdateData(FALSE);
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::PopulateDistributionList(void)

  {
  m_DistSel.ResetContent();
  for (int d=0;d<H.SDs.GetSize();d++)
    m_DistSel.AddString(H.SDs[d]->Name.Str());

  CurDistIndex=0;
  m_DistSel.SetCurSel(CurDistIndex);
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::PopulateMeasurmentList(void)

  {

  if ( CurDistIndex >= 0 )
    {
    m_MeasList.ResetContent();
    for (int m=0; m<H.SDsMeas[CurDistIndex]->Measurements.GetSize(); m++)
      {
      CSD_Measurement M = H.SDsMeas[CurDistIndex]->Measurements[m];

      m_MeasList.AddString(M.m_sName.Str());
      }

      m_MeasList.SetCurSel(0);

    }
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnCbnSelchangeDistributionSel()
  {

    CurDistIndex = m_DistSel.GetCurSel();
    PopulateMeasurmentList();
    OnLbnSelchangeMeasList();

  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnBnClickedMeasAdd()
  {
  // Add a new measurement entry for the distribution list
  if ( CurDistIndex >= 0 )
    {

      int cnt = H.SDsMeas[CurDistIndex]->Measurements.GetSize();
      CSD_Measurement M;

      cnt++;
      M.m_sName = "Name";
      M.ValCnv.SetText("%");
      M.Cnv.SetText("um");
      //M.Fmt = ??;
      M.m_dValue = 10;
      M.m_eType = eSDMT_Text;
      H.SDsMeas[CurDistIndex]->Measurements.Add(M);
      m_MeasList.AddString(M.m_sName.Str());
      
      int n = m_MeasList.GetCount();
      //m_MeasList.SelectString(-1,M.m_sName.Str());
      m_MeasList.SetCurSel(n-1);

      OnLbnSelchangeMeasList();

      UpdateData(false);
    }

  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnBnClickedMeasRemove()
  {

  long sel = m_MeasList.GetCurSel();
  if ( CurDistIndex >= 0 )
    {

    if ((sel >= 0) && (sel<H.SDsMeas[CurDistIndex]->Measurements.GetSize()))
      {

        // Remove the selected measurement from the measurement display
        // list and Distribution Measurment Array
        H.SDsMeas[CurDistIndex]->Measurements.RemoveAt(sel);

        m_MeasList.DeleteString(sel);

        if (sel > 1)
          m_MeasList.SetCurSel(sel-1);
        else
          m_MeasList.SetCurSel(0);

        OnLbnSelchangeMeasList();

        UpdateData(false);
      }


    }
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnEnChangeMeasName()
  {

  if (CurDistIndex >= 0)
    {
    long sel = m_MeasList.GetCurSel();
    if (sel >= 0)
      {
      CString rString;
      m_EditName.GetWindowText(rString);
      H.SDsMeas[CurDistIndex]->Measurements[sel].m_sName = rString;

      // How do you replace the text of a selection in a list box???

      // Brute force method for the moment
 
      m_MeasList.DeleteString(sel);
      m_MeasList.InsertString(sel, rString);
      m_MeasList.SetCurSel(sel);

      UpdateData(false);
      }   
    }


  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnLbnSelchangeMeasList()
  {

       long sel = m_MeasList.GetCurSel();
        if (sel >= 0)
          {

           m_MeasTypeSel.EnableWindow(true);
           m_EditName.EnableWindow(true);
           m_BtnMeasUp.EnableWindow(true);
           m_BtnMeasDown.EnableWindow(true);
   
          // Get a copy of the Measurment Details
          CSD_Measurement M = H.SDsMeas[CurDistIndex]->Measurements[sel];

          // Update the Name  Edit Window Text
          m_EditName.SetWindowText(M.m_sName.Str());

          // Update the Measurement Type Combo Box Selection
          // Need to make this smarter by referencing the CMeasInfo data structure in sqzbase.cpp
          char* str = NULL;
          switch(M.m_eType)
            {
            case eSDMT_FracPassSize: str = "FracPassSize"; break;
            case eSDMT_SizePassFrac: str = "SizePassFrac"; break;
            case eSDMT_PPG         : str = "PPG";          break;
            case eSDMT_SAM         : str = "SAM";          break;
            case eSDMT_SAL         : str = "SAL";          break;
            case eSDMT_Text        : str = "Text";         break;

            }

          m_MeasTypeSel.SelectString(-1,str);
          int l = m_MeasTypeSel.GetCurSel();

          // Update Measured Value Parameter
          //m_Value = M.m_dValue;
          char valbuff[80];
          sprintf(valbuff,"%g",M.ValCnv.Human(M.m_dValue));
          m_EditBoxValue.SetWindowText(valbuff);
          

          // Build the Selectable Value and Result Units
          // and set the current Value and Result units
          // based on the selected Measurment 
          m_ComboValueUnits.ResetContent();
          m_ComboResultUnits.ResetContent();

          CDataCnv* lengthcnv = Cnvs[DC_L];
          CDataCnv* fraccnv   = Cnvs[DC_Frac];
          CDataCnv* unitscnv  = NULL;
          CDataCnv* resultcnv = NULL;

          if (M.m_eType == eSDMT_FracPassSize)
          {
              unitscnv = lengthcnv;
              resultcnv = fraccnv;
          }
          else if (M.m_eType == eSDMT_SizePassFrac)
          {
              unitscnv = fraccnv;
              resultcnv = lengthcnv;
          }

 
          if ((M.m_eType == eSDMT_FracPassSize)||
              (M.m_eType == eSDMT_SizePassFrac)
              )
            {

                m_ComboValueUnits.EnableWindow(true);
                m_ComboResultUnits.EnableWindow(true);
                m_ComboValueUnits.EnableWindow(true);
                m_ComboResultUnits.EnableWindow(true);
                m_EditBoxValue.EnableWindow(true);
                m_EditBoxValue.EnableWindow(true);

                // Units
                CDataCnv* lCnv = unitscnv;
                while ( lCnv != NULL )
                {
                  char* s = lCnv->Txt();
                  m_ComboValueUnits.AddString(s);
                  lCnv = lCnv->Next();
                }

                // Results
                lCnv = resultcnv;
                while ( lCnv != NULL )
                {
                  char* s = lCnv->Txt();
                  m_ComboResultUnits.AddString(s);
                  lCnv = lCnv->Next();
                }

                m_ComboValueUnits.SelectString(-1,M.ValCnv.Text());
                m_ComboResultUnits.SelectString(-1,M.Cnv.Text());


            }
          else
            {
              m_EditBoxValue.EnableWindow(false);
              m_ComboValueUnits.EnableWindow(false);
              m_ComboResultUnits.EnableWindow(false);
            }

          }  
        else
          {
              m_MeasTypeSel.EnableWindow(false);
              m_EditName.EnableWindow(false);
              m_EditBoxValue.EnableWindow(false);
              m_ComboValueUnits.EnableWindow(false);
              m_ComboResultUnits.EnableWindow(false);
              m_BtnMeasUp.EnableWindow(false);
              m_BtnMeasDown.EnableWindow(false);
          }

        UpdateData(false);

  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnCbnSelchangeSelMeasType()
  {

        UpdateData(true);
        long sel = m_MeasList.GetCurSel();
        if ((sel >= 0) && (CurDistIndex>=0))
          {
          CSD_Measurement M = H.SDsMeas[CurDistIndex]->Measurements[sel];
 
          // Need to make this smarter by referencing the CMeasInfo data structure in sqzbase.cpp

          long sel_type = m_MeasTypeSel.GetCurSel();

          CString rString;
          m_MeasTypeSel.GetLBText(sel_type,rString);

          if ( rString == "FracPassSize" )
            {
              H.SDsMeas[CurDistIndex]->Measurements[sel].m_eType = eSDMT_FracPassSize;
            }
          else
         if ( rString == "SizePassFrac" )
            {
              H.SDsMeas[CurDistIndex]->Measurements[sel].m_eType = eSDMT_SizePassFrac;
            }
          else
         if ( rString == "PPG" )
            {
              H.SDsMeas[CurDistIndex]->Measurements[sel].m_eType = eSDMT_PPG;
            }
         else
         if ( rString == "SAM" )
            {
              H.SDsMeas[CurDistIndex]->Measurements[sel].m_eType = eSDMT_SAM;
            }
         else if ( rString == "SAL" )
            {
              H.SDsMeas[CurDistIndex]->Measurements[sel].m_eType = eSDMT_SAL;
            }
         else if ( rString == "Text" )
            {
              H.SDsMeas[CurDistIndex]->Measurements[sel].m_eType = eSDMT_Text;
            }

          }    

        // As the measurement type has changed
        // We also need to update the display
        // as the various entry windows are visible depedent
        // on the type
        OnLbnSelchangeMeasList();
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnEnChangeEditValue()
  {

  UpdateData(true);
  if (CurDistIndex >= 0)
    {
    long sel = m_MeasList.GetCurSel();
    if (sel >= 0)
      {
          CString rString;
          m_EditBoxValue.GetWindowText(rString);
          double lVal = atof(rString);
          lVal = H.SDsMeas[CurDistIndex]->Measurements[sel].ValCnv.Normal(lVal);
          H.SDsMeas[CurDistIndex]->Measurements[sel].m_dValue = lVal;

      }   
    }

  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnCbnSelchangeComboValUnits()
  {
  UpdateData(true);
  if (CurDistIndex >= 0)
    {
    long sel = m_MeasList.GetCurSel();
    if (sel >= 0)
      {
          long sel_units = m_ComboValueUnits.GetCurSel();

          CString rString;
          m_ComboValueUnits.GetLBText(sel_units,rString);

          const char* s = rString;
          H.SDsMeas[CurDistIndex]->Measurements[sel].ValCnv.SetText((char*)s);
          OnEnChangeEditValue();
      }   
    }
  }

//---------------------------------------------------------------------------


void CSqSzDMeasPg::OnCbnSelchangeComboResUnits()
  {
  UpdateData(true);
  if (CurDistIndex >= 0)
    {
    long sel = m_MeasList.GetCurSel();
    if (sel >= 0)
      {
         long sel_result = m_ComboResultUnits.GetCurSel();

          CString rString;
          m_ComboResultUnits.GetLBText(sel_result,rString);

          const char* s = rString;
          H.SDsMeas[CurDistIndex]->Measurements[sel].Cnv.SetText((char*)s);
          OnEnChangeEditValue();
      }   
    }
  }
//---------------------------------------------------------------------------


void CSqSzDMeasPg::OnBnClickedButtonMeasUp()
  {

    // Move measurement Up
    if (CurDistIndex >= 0)
    {
        long sel = m_MeasList.GetCurSel();
        if (sel > 0)
        { 
            // Swap the measurement order with the previous measurement

            CSD_Measurement temp;
            CString selString;
            selString = H.SDsMeas[CurDistIndex]->Measurements[sel].m_sName.Str();

            CString selm1String;
            selm1String = H.SDsMeas[CurDistIndex]->Measurements[sel-1].m_sName.Str();

            temp = H.SDsMeas[CurDistIndex]->Measurements[sel];
            H.SDsMeas[CurDistIndex]->Measurements[sel] = H.SDsMeas[CurDistIndex]->Measurements[sel-1];
            H.SDsMeas[CurDistIndex]->Measurements[sel-1] = temp;


            m_MeasList.DeleteString(sel);
            m_MeasList.InsertString(sel,selm1String);

            m_MeasList.DeleteString(sel-1);
            m_MeasList.InsertString(sel-1,selString);
            m_MeasList.SetCurSel(sel-1);


        }   
    }
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnBnClickedButtonMeasDown()
  {

   // Move measurement Down
    if (CurDistIndex >= 0)
    {
        long sel = m_MeasList.GetCurSel();
        if (sel < (m_MeasList.GetCount()-1))
        { 
            // Swap the measurement order with the previous measurement

            CSD_Measurement temp;
            CString selString;
            selString = H.SDsMeas[CurDistIndex]->Measurements[sel].m_sName.Str();

            CString selp1String;
            selp1String = H.SDsMeas[CurDistIndex]->Measurements[sel+1].m_sName.Str();


            temp = H.SDsMeas[CurDistIndex]->Measurements[sel];
      H.SDsMeas[CurDistIndex]->Measurements[sel] = H.SDsMeas[CurDistIndex]->Measurements[sel+1];
            H.SDsMeas[CurDistIndex]->Measurements[sel+1] = temp;


            m_MeasList.DeleteString(sel);
            m_MeasList.InsertString(sel,selp1String);

            m_MeasList.DeleteString(sel+1);
            m_MeasList.InsertString(sel+1,selString);
            m_MeasList.SetCurSel(sel+1);
      }   
    }
  }

//---------------------------------------------------------------------------

void CSqSzDMeasPg::OnEnChangeCountFactor()
  {
  CString rString;
  m_CountFactor.GetWindowText(rString);
  dCountFactor = atof(rString);
  if (dCountFactor<1.0e-12)
    dCountFactor=1.0;
    }

//---------------------------------------------------------------------------
