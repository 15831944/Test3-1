//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992 - 1995
/*#M:PGM debugger.*/

#include "stdafx.h"
#include <math.h>
#include <stdlib.h>

#define  __PGM_D_CPP

#include "sc_defs.h"
#include "pgm_d.h"
#include "scd_wm.h"

const WORD DBG_MSG_UPDATE         = 1;
const WORD DBG_MSG_CAPTURE        = 2;
const WORD DBG_MSG_RELEASE        = 3;
const WORD DBG_MSG_CONTROLS       = 4;
const WORD DBG_MSG_INITSCROLLBARS = 5;
const WORD DBG_MSG_DBLCLICK       = 6;
const word DBG_MSG_HIDE           = 7;
const int TxtHeight = 12;
const int TxtWidth = 8;
const int ScrollBarWidth = 14;

//===========================================================================

CListWndHelper::CListWndHelper()
  { 
  iColsUsed = 0;
  iMaxWidth = 0;
  iMaxLines = 0;
  iCurLine = 0;
  iNextLine = 0;
  iMargin = 0;
  iHScrollPos = 0;
  iVScrollPos = 0;
  Color1 = RGB(255,0,0);
  bHorzLines = 0;
  bVertLines = 0;
  bForceHScroll = 0;
  bForceVScroll = 0;
  bWrapArround = 0;
  bAllowSelect = 1;
  bUseSelectSymb = 0;
  }

//---------------------------------------------------------------------------

void CListWndHelper::ToggleFlag(int line, long flag)
  {
  if (line>=0 && line<FlagsList.GetLen())
    {
    if ((FlagsList[line] & flag)==flag)
      FlagsList[line] &= (~flag);
    else
      FlagsList[line] |= flag;
    }
  }

//---------------------------------------------------------------------------

void CListWndHelper::ClearLine(int line)
  {
  if (line>=0 && iColsUsed>0 && line<StrList[0].GetLen())
    {
    for (int j=0; j<iColsUsed; j++)
      StrList[j][line] = "";
    }
  }

//===========================================================================

CPGMDbg::CPGMDbg()
  {
  bBreakLine = 0;
  bBreakPt = 0;
  }

//===========================================================================

CPGMDebugWnd::CPGMDebugWnd(CWnd* pParent)
	: CDialog(CPGMDebugWnd::IDD, pParent),
    CodeWnd(13,68),
    VarWnd(11,48)
  {
	pDbgMngr = NULL;
  pPrevDbgMngr = NULL;
  bDoBreak = 0;
	//{{AFX_DATA_INIT(CPGMDebugWnd)
	m_PgmName = _T("");
	m_LineNo = 0;
	m_VarOption = -1;
	//}}AFX_DATA_INIT
  Create(CPGMDebugWnd::IDD, pParent); // NonModal Create
  //ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

CPGMDebugWnd::~CPGMDebugWnd()
  {
  CPGMDbgMngr::pWnd = NULL;
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::Close()
  {
  PostNcDestroy();
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::PostNcDestroy() 
  {
	CDialog::PostNcDestroy();
  delete this;
  }

//---------------------------------------------------------------------------

int CPGMDebugWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
  {
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectWnd;
	GetWindowRect(rectWnd);
  int Height = 1 + ((TxtHeight+1) * CodeWnd.iHeight);
  int Width = 5 + (TxtWidth * CodeWnd.iWidth);
	CRect rect(8, 202, 8 + Width + ScrollBarWidth, 202 + Height + ScrollBarWidth);
	CodeWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rect, this, (UINT)-1);
  Height = 1 + ((TxtHeight+1) * VarWnd.iHeight);
  Width = 5 + (TxtWidth * VarWnd.iWidth);
	rect.top = 35;
	rect.bottom = rect.top + Height + ScrollBarWidth;
	rect.right = rect.left + Width  + ScrollBarWidth;
	VarWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rect, this, (UINT)-1);
	CenterWindow();
	return 0;
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::Hook(pCPGMDbgMngr DbgMngr)
  {
  pDbgMngr = DbgMngr;
  CodeWnd.AttachHelper(&(pDbgMngr->CodeInfo));
  if (pDbgMngr->iVarOption==0)
    VarWnd.AttachHelper(&(pDbgMngr->VarInfo));
  else if (pDbgMngr->iVarOption==1)
    VarWnd.AttachHelper(&(pDbgMngr->VarChngdInfo));
  else
    VarWnd.AttachHelper(&(pDbgMngr->VarWatchInfo));
  if (bDoBreak && pDbgMngr==pPrevDbgMngr)
    {
    bDoBreak = 0;
	  pDbgMngr->bRunTillBreak = 0;
    }
  //if (IsWindowVisible())
  //  PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::UnHook(pCPGMDbgMngr DbgMngr)
  {
  ASSERT(pDbgMngr==DbgMngr);
  if (!bDoBreak)
    pPrevDbgMngr = pDbgMngr;
  pDbgMngr = NULL;
  //CodeWnd.iEndLine = -1;
  //if (IsWindowVisible())
  //  PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPGMDebugWnd)
	DDX_Text(pDX, IDC_PGMNAME, m_PgmName);
	DDX_Text(pDX, IDC_LINENO, m_LineNo);
	DDX_Radio(pDX, IDC_VAROPTION, m_VarOption);
	//}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPGMDebugWnd, CDialog)
	//{{AFX_MSG_MAP(CPGMDebugWnd)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
  ON_MESSAGE(WMU_UPDATEPGMDEBUG, OnUpdateData)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP, IDC_STEP+5, OnBtnPressed)
  //ON_UPDATE_COMMAND_UI_RANGE(IDC_STEP, IDC_STEP+5, OnUpdateBtn)
  ON_UPDATE_COMMAND_UI(IDC_STEP, OnUpdateStep)
  ON_UPDATE_COMMAND_UI(IDC_RUN, OnUpdateRun)
  ON_UPDATE_COMMAND_UI(IDC_BREAK, OnUpdateBreak)
  ON_UPDATE_COMMAND_UI(IDC_CLOSEIT, OnUpdateClose)
  ON_UPDATE_COMMAND_UI(IDC_BREAKPT, OnUpdateBreakpt)
  ON_UPDATE_COMMAND_UI(IDC_WATCH, OnUpdateWatch)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_VAROPTION, IDC_VAROPTION3, OnVaroption)
  ON_UPDATE_COMMAND_UI_RANGE(IDC_VAROPTION, IDC_VAROPTION3, OnUpdateVaroption)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

LRESULT CPGMDebugWnd::OnUpdateData(WPARAM wParam, LPARAM lParam)
  {
  switch (wParam)
    {
    case DBG_MSG_UPDATE:
      UpdateData(False);
      break;
    case DBG_MSG_CAPTURE:
      if (!IsWindowVisible())
        ShowWindow(SW_SHOW);
      AfxGetMainWnd()->EnableWindow(FALSE);
      EnableWindow();
      SetForegroundWindow();
    case DBG_MSG_RELEASE:
      AfxGetMainWnd()->EnableWindow();
      break;
    case DBG_MSG_CONTROLS:
      if (IsWindowVisible())
        {
        UpdateDialogControls(this, FALSE);
        CodeWnd.UpdateScrollBars();
        VarWnd.UpdateScrollBars();
        }
      break;
    case DBG_MSG_INITSCROLLBARS:
      CodeWnd.InitScrollBars();
      VarWnd.InitScrollBars();
      break;
    case DBG_MSG_DBLCLICK:
      {
      if ((CListWnd*)lParam==&VarWnd)
        OnBtnPressed(IDC_WATCH);
      else if ((CListWnd*)lParam==&CodeWnd)
        OnBtnPressed(IDC_BREAKPT);
      break;
      }
    case DBG_MSG_HIDE:
      ShowWindow(SW_HIDE);
      break;
    }
  return True;
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::OnBtnPressed(UINT nID)
  {
 
  switch (nID)
    {
    case IDC_STEP:
    case IDC_RUN:
      if (pDbgMngr && pDbgMngr->bPaused)
        {
        if (nID==IDC_RUN)
  	      pDbgMngr->bRunTillBreak = 1;
        SetEvent(CPGMDbgMngr::hEvent);
        PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
        }
      break;
    case IDC_BREAK:
      if (pDbgMngr)
  	    pDbgMngr->bRunTillBreak = 0;
      else
        bDoBreak = 1;
      PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
      break;
    case IDC_CLOSEIT:
      if (!pDbgMngr || !pDbgMngr->bPaused)
        ShowWindow(SW_HIDE);
      break;
    case IDC_BREAKPT:
      if (pDbgMngr && pDbgMngr->bPaused && pDbgMngr->pInsMngr)
        {
        int NewLine = pDbgMngr->pInsMngr->SetBrkPt(CodeWnd.pInfo->iCurLine+1) - 1;
        CodeWnd.pInfo->ToggleFlag(NewLine, LW_SYMB2);
        CodeWnd.SetCurLine(NewLine);
        PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
        }
      break;
    case IDC_WATCH:
      if (pDbgMngr && pDbgMngr->bPaused && (pDbgMngr->iVarOption==0 || pDbgMngr->iVarOption==2))
        {
        if (pDbgMngr->iVarOption==0)
          pDbgMngr->VarInfo.ToggleFlag(pDbgMngr->VarInfo.iCurLine, LW_SYMB1);
        else
          {
          for (int i=0; i<pDbgMngr->VarInfo.iMaxLines; i++)
            {
            if ((pDbgMngr->VarInfo.FlagsList[i] & LW_SYMB1) && pDbgMngr->VarInfo.StrList[0][i]==VarWnd.pInfo->StrList[0][VarWnd.pInfo->iCurLine])
              {
              pDbgMngr->VarInfo.ToggleFlag(i, LW_SYMB1);
              i = pDbgMngr->VarInfo.iMaxLines;
              }
            }
          }
        pDbgMngr->BuildWatchInfo();
        VarWnd.InitScrollBars();
        VarWnd.Invalidate();
        }
      break;
    default:
      ASSERT(FALSE);
    }
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::OnUpdateStep(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pDbgMngr && pDbgMngr->bPaused);
  }

void CPGMDebugWnd::OnUpdateRun(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pDbgMngr && pDbgMngr->bPaused);
  }

void CPGMDebugWnd::OnUpdateBreak(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!pDbgMngr || !pDbgMngr->bPaused);
  }

void CPGMDebugWnd::OnUpdateClose(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(!pDbgMngr || !pDbgMngr->bPaused);
  }

void CPGMDebugWnd::OnUpdateBreakpt(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pDbgMngr && pDbgMngr->bPaused && pDbgMngr->pInsMngr);
  }

void CPGMDebugWnd::OnUpdateWatch(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pDbgMngr && pDbgMngr->bPaused && (pDbgMngr->iVarOption==0 || pDbgMngr->iVarOption==2));
  }

//---------------------------------------------------------------------------

void CPGMDebugWnd::OnUpdateVaroption(CCmdUI* pCmdUi)
  {
  pCmdUi->Enable(pDbgMngr && pDbgMngr->bPaused);
  }

void CPGMDebugWnd::OnVaroption(UINT nID)
  {
  
  if (pDbgMngr)
    {
    UpdateData(TRUE);
	  pDbgMngr->iVarOption = m_VarOption;
    if (pDbgMngr->iVarOption==0)
      VarWnd.pInfo = &(pDbgMngr->VarInfo);
    else if (pDbgMngr->iVarOption==1)
      VarWnd.pInfo = &(pDbgMngr->VarChngdInfo);
    else
      VarWnd.pInfo = &(pDbgMngr->VarWatchInfo);
    VarWnd.InitScrollBars();
    VarWnd.UpdateScrollBars();
    if (pDbgMngr->iVarOption==0)
      VarWnd.SetCurLine(VarWnd.pInfo->iCurLine, 1);
    else if (pDbgMngr->iVarOption==1)
      VarWnd.SetCurLine(VarWnd.pInfo->iNextLine, 2);
    else 
      VarWnd.SetCurLine(VarWnd.pInfo->iCurLine, 1);
    UpdateDialogControls(this, FALSE);
    //VarWnd.Invalidate();
    UpdateData(FALSE);
    }
  }

//===========================================================================

pCPGMDebugWnd CPGMDbgMngr::pWnd = NULL;
HANDLE CPGMDbgMngr::hEvent = NULL;

//---------------------------------------------------------------------------

CPGMDbgMngr::CPGMDbgMngr()
  {
  bRunTillBreak = 0;
  bPaused = 0;
  iLineNo = 0;
  iVarOption = 0;
  pInsMngr = NULL;
  }

//---------------------------------------------------------------------------

CPGMDbgMngr::~CPGMDbgMngr()
  {
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::Open()
  {
  if (pWnd==NULL)
    {
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    pWnd = new CPGMDebugWnd(AfxGetMainWnd());
    }
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::Close()
  {
  if (pWnd)
    {
    pWnd->Close();
    CloseHandle(hEvent);
    pWnd = NULL;
    }
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::Reset()
  {
  if (pWnd)
    pWnd->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_HIDE);
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::ConnectWindow()
  {
  pWnd->Hook(this);
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::DisConnectWindow()
  {
  pWnd->UnHook(this);
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::InitVarList(int& Cnt, pGCVar pVar, pchar pClassName)
  {
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass)
      {
      if ((pVar->m_flags & VarClassDefn)==0)
        InitVarList(Cnt, ((pGCClassVar)pVar)->m_pVarList, pVar->Name());
      }
    else
      {
      if (((pVar->m_flags & VarConst)==0) && (pVar->WhatAmI()!=VarFunct))
        {
        int len = VarInfo.StrList[0].GetLen();
        if (Cnt>=len)
          {
          VarInfo.StrList[0].SetSize(len + 16);
          VarInfo.StrList[2].SetSize(len + 16);
          }
        VarInfo.StrList[0][Cnt] = pClassName;
        if (strlen(pClassName)>0)
          VarInfo.StrList[0][Cnt] += '.';
        VarInfo.StrList[0][Cnt] += pVar->Name();
        VarInfo.ColWidths[0] = Max(VarInfo.ColWidths[0], VarInfo.StrList[0][Cnt].Length());
        switch (pVar->WhatAmI())
          {
          case VarDouble: 
            VarInfo.StrList[2][Cnt] = "Double";
            break;
          case VarLong:
            VarInfo.StrList[2][Cnt] = "Long";
            break;
          case VarByte:
            VarInfo.StrList[2][Cnt] = "Byte";
            break;
          case VarBit:
            VarInfo.StrList[2][Cnt] = "Bit";
            break;
          case VarStr:
            VarInfo.StrList[2][Cnt] = "Str";
            break;
          case VarFunct:
            VarInfo.StrList[2][Cnt] = "Funct";
            break;
          case VarTag:
            VarInfo.StrList[2][Cnt] = "Tag";
            break;
          case VarClass:
            VarInfo.StrList[2][Cnt] = "Class";
            break;
          default:
            VarInfo.StrList[2][Cnt] = "";
          }
        Cnt++;
        }
      }
    pVar = pVar->Next();
    }
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::InitInfo(rGCInsMngr IB, int LastLine)
  {
  CodeInfo.StrList[0].SetSize(LastLine);
  CodeInfo.FlagsList.SetSize(LastLine);
  CodeInfo.iMaxLines = LastLine;
  CodeInfo.iColsUsed = 1;
  CodeInfo.iMargin = 2;
  CodeInfo.ColWidths[0] = 0;
  CodeInfo.bHorzLines = 0;
  CodeInfo.bVertLines = 0;
  CodeInfo.bForceHScroll = 1;
  CodeInfo.bForceVScroll = 1;
  CodeInfo.bWrapArround = 0;
  CodeInfo.bAllowSelect = 1;
  CodeInfo.bUseSelectSymb = 0;

  VarInfo.iColsUsed = 4;
  for (int i=0; i<VarInfo.iColsUsed; i++)
    {
    VarInfo.StrList[i].SetSize(16);
    VarInfo.ColWidths[i] = 0;
    }
  int Cnt = 0;
  InitVarList(Cnt, IB.m_pVarList, "");
  VarInfo.iMaxLines = Cnt;
  VarInfo.iMargin = 1;
  VarInfo.ColWidths[1] = 20;
  VarInfo.ColWidths[2] = 6;
  VarInfo.ColWidths[3] = 20;
  VarInfo.iMaxWidth = VarInfo.iMargin;
  for (i=0; i<VarInfo.iColsUsed; i++)
    {
    VarInfo.StrList[i].SetSize(Cnt);
    VarInfo.iMaxWidth += VarInfo.ColWidths[i];
    }
  VarInfo.FlagsList.SetSize(Cnt);
  VarInfo.bHorzLines = 1;
  VarInfo.bVertLines = 1;
  VarInfo.bForceHScroll = 1;
  VarInfo.bForceVScroll = 1;
  VarInfo.bWrapArround = 0;
  VarInfo.bAllowSelect = 0;
  VarInfo.bUseSelectSymb = 1;

  VarChngdInfo.iColsUsed = 4;
  VarChngdInfo.iMaxLines = 60;
  for (i=0; i<VarChngdInfo.iColsUsed; i++)
    {
    VarChngdInfo.StrList[i].SetSize(VarChngdInfo.iMaxLines);
    VarChngdInfo.ColWidths[i] = VarInfo.ColWidths[i];
    }
  VarChngdInfo.FlagsList.SetSize(VarChngdInfo.iMaxLines);
  VarChngdInfo.iMaxWidth = VarInfo.iMaxWidth;
  VarChngdInfo.iMargin = 1;
  VarChngdInfo.bHorzLines = 0;
  VarChngdInfo.bVertLines = 1;
  VarChngdInfo.bForceHScroll = 1;
  VarChngdInfo.bForceVScroll = 1;
  VarChngdInfo.bWrapArround = 1;
  VarChngdInfo.bAllowSelect = 0;
  VarChngdInfo.bUseSelectSymb = 0;

  VarWatchInfo.iColsUsed = 4;
  VarWatchInfo.iMaxLines = 0;
  for (i=0; i<VarWatchInfo.iColsUsed; i++)
    {
    VarWatchInfo.StrList[i].SetSize(VarWatchInfo.iMaxLines);
    VarWatchInfo.ColWidths[i] = VarInfo.ColWidths[i];
    }
  VarWatchInfo.FlagsList.SetSize(VarWatchInfo.iMaxLines);
  VarWatchInfo.iMaxWidth = VarInfo.iMaxWidth;
  VarWatchInfo.iMargin = 1;
  VarWatchInfo.bHorzLines = 1;
  VarWatchInfo.bVertLines = 1;
  VarWatchInfo.bForceHScroll = 1;
  VarWatchInfo.bForceVScroll = 1;
  VarWatchInfo.bWrapArround = 0;
  VarWatchInfo.bAllowSelect = 0;
  VarWatchInfo.bUseSelectSymb = 1;
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::AddLine(int LineNo, const char* p)
  {
  ASSERT(LineNo<CodeInfo.StrList[0].GetLen());
  if (p)
    {
    CodeInfo.iMaxWidth = Max(CodeInfo.iMaxWidth, CodeInfo.iMargin + (int)strlen(p));
    CodeInfo.ColWidths[0] = CodeInfo.iMaxWidth;
    CodeInfo.StrList[0][LineNo].Set("%s", p);
    }
  else
    CodeInfo.StrList[0][LineNo].Set("");
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::ListVarValues(int& Cnt, pGCVar pVar)
  {
  char Buff[4096];
  while (pVar)
    {
    if (pVar->WhatAmI()==VarClass)
      {
      if ((pVar->m_flags & VarClassDefn)==0)
        ListVarValues(Cnt, ((pGCClassVar)pVar)->m_pVarList);
      }
    else
      {
      if (((pVar->m_flags & VarConst)==0) && (pVar->WhatAmI()!=VarFunct))
        {
        VarInfo.StrList[1][Cnt] = pVar->GetStrVal(Buff);
        if (VarInfo.StrList[1][Cnt]==VarInfo.StrList[3][Cnt])
          VarInfo.FlagsList[Cnt] &= (~LW_COL1);
        else
          VarInfo.FlagsList[Cnt] |= LW_COL1;
        Cnt++;
        }
      }
    pVar = pVar->Next();
    }
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::ListVarChngdValues()
  {
  int i = (VarChngdInfo.iNextLine - 1 + VarChngdInfo.iMaxLines) % VarChngdInfo.iMaxLines;
  while (VarChngdInfo.FlagsList[i] & LW_COL1)
    {
    VarChngdInfo.FlagsList[i] &= (~LW_COL1);
    i--;
    if (i<0)
      i = VarChngdInfo.iMaxLines-1;
    }
  flag Changed = False;
  for (i=0; i<VarInfo.iMaxLines; i++)
    {
    if (!(VarInfo.StrList[1][i] == VarInfo.StrList[3][i]))
      {
      for (int j=0; j<4; j++)
        VarChngdInfo.StrList[j][VarChngdInfo.iNextLine] = VarInfo.StrList[j][i];
      VarChngdInfo.FlagsList[VarChngdInfo.iNextLine] = LW_COL1;
      VarChngdInfo.IncLine();
      Changed = True;
      }
    }
  if (Changed)
    {
    i = (VarChngdInfo.iNextLine - 1) % VarChngdInfo.iMaxLines;
    VarChngdInfo.FlagsList[i] |= LW_LINE;
    for (int j=1; j<3; j++)
      {
      VarChngdInfo.ClearLine((i+j) % VarChngdInfo.iMaxLines);
      VarChngdInfo.FlagsList[(i+j) % VarChngdInfo.iMaxLines] = 0;
      }
    }
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::ListVarWatchValues()
  {
  int Cnt=0;
  for (int i=0; i<VarInfo.iMaxLines; i++)
    {
    if (VarInfo.FlagsList[i] & LW_SYMB1)
      {
      VarWatchInfo.FlagsList[Cnt] = VarInfo.FlagsList[i];
      VarWatchInfo.StrList[1][Cnt] = VarInfo.StrList[1][i];
      VarWatchInfo.StrList[3][Cnt] = VarInfo.StrList[3][i];
      Cnt++;
      }
    }
  }
          
//---------------------------------------------------------------------------

void CPGMDbgMngr::BuildWatchInfo()
  {
  int Cnt=0;
  for (int i=0; i<VarInfo.iMaxLines; i++)
    {
    if (VarInfo.FlagsList[i] & LW_SYMB1)
      {
      if (Cnt >= VarWatchInfo.FlagsList.GetLen())
        {
        VarWatchInfo.FlagsList.SetSize(Cnt+1);
        for (int j=0; j<VarWatchInfo.iColsUsed; j++)
          VarWatchInfo.StrList[j].SetSize(Cnt+1);
        }
      VarWatchInfo.FlagsList[Cnt] = VarInfo.FlagsList[i];
      for (int j=0; j<VarWatchInfo.iColsUsed; j++)
        VarWatchInfo.StrList[j][Cnt] = VarInfo.StrList[j][i];
      Cnt++;
      }
    }
  VarWatchInfo.FlagsList.SetSize(Cnt);
  for (int j=0; j<VarWatchInfo.iColsUsed; j++)
    VarWatchInfo.StrList[j].SetSize(Cnt);
  VarWatchInfo.iMaxLines = Cnt;
  VarWatchInfo.iCurLine = Max(0, Min(VarWatchInfo.iCurLine, VarWatchInfo.iMaxLines-1));
  }

//---------------------------------------------------------------------------

void CPGMDbgMngr::Advance(pCPGMDbg pDbg, pCPGMDbg pNxtDbg, rGCInsMngr IB)
  {
  if (pDbg->bBreakPt || (!bRunTillBreak && pDbg->bBreakLine))
    {
    pInsMngr = &IB;
    pWnd->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CAPTURE);
    bRunTillBreak = 0;
    //set dialog fields...
    pWnd->m_VarOption = iVarOption;
    iLineNo = pNxtDbg->iLineNo;
    pWnd->m_PgmName = sPgmName();
    CodeInfo.FlagsList[iLineNo-1] |= LW_SYMB1;
    pWnd->CodeWnd.SetCurLine(iLineNo-1);
    pWnd->m_LineNo = iLineNo-1;
    //set pgm data variables...
    VarInfo.StrList[3] = VarInfo.StrList[1];
    int Cnt = 0;
    ListVarValues(Cnt, IB.m_pVarList);
    ListVarChngdValues();
    ListVarWatchValues();
    if (iVarOption==0)
      pWnd->VarWnd.SetCurLine(VarInfo.iCurLine, 1);
    else if (iVarOption==1)
      pWnd->VarWnd.SetCurLine(VarChngdInfo.iNextLine, 2);
    else 
      pWnd->VarWnd.SetCurLine(VarWatchInfo.iCurLine, 1);
    pWnd->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_UPDATE);
    ResetEvent(hEvent);
    bPaused = 1;
    pWnd->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
    WaitForSingleObject(hEvent, INFINITE);
    bPaused = 0;
    pWnd->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_RELEASE);
    pWnd->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_CONTROLS);
    CodeInfo.FlagsList[iLineNo-1] &= (~LW_SYMB1);
    pInsMngr = NULL;
    }
  }

//===========================================================================

CFont CListWnd::Font;
flag CListWnd::bCreatedFont = False;

CListWnd::CListWnd(int Height, int Width)
  {
  pInfo = NULL;
  iHeight = Height;
  iWidth = Width;
  iStartLine = 0;
  iEndLine = -1;
  if (!bCreatedFont)
    {
    bCreatedFont = True;
    Font.CreateFont(TxtHeight,               // int nHeight, 
                    0,                       // int nWidth, 
                    0,                       // int nEscapement, 
                    0,                       // int nOrientation, 
                    FW_REGULAR,              // int nWeight, 
                    0,                       // BYTE bItalic, 
                    0,                       // BYTE bUnderline, 
                    0,                       // BYTE cStrikeOut, 
                    1,                       // BYTE nCharSet, 
                    OUT_TT_PRECIS,           // BYTE nOutPrecision, 
                    CLIP_DEFAULT_PRECIS,     // BYTE nClipPrecision, 
                    DEFAULT_QUALITY,         // BYTE nQuality, 
                    FIXED_PITCH|FF_MODERN,   // BYTE nPitchAndFamily, 
                    "Courier"                // LPCSTR lpszFacename 
                   );
    }
  }

//---------------------------------------------------------------------------

CListWnd::~CListWnd()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CListWnd, CWnd)
	//{{AFX_MSG_MAP(CListWnd)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

int CListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
  {
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
	CRect rectWnd;
	GetWindowRect(rectWnd);
	//SetWindowPos(NULL, 0, 0, rectWnd.Width(), rectWnd.Height(),	SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	CRect VRect(rectWnd.Width() - ScrollBarWidth, 0, rectWnd.Width(), rectWnd.Height() - ScrollBarWidth);
  if (!VScrollBar.Create(SBS_VERT, VRect, this, (UINT)-1))
    return -1;
	CRect HRect(0, rectWnd.Height() - ScrollBarWidth, rectWnd.Width() - ScrollBarWidth, rectWnd.Height());
  if (!HScrollBar.Create(SBS_HORZ, HRect, this, (UINT)-1))
    return -1;
	return 0;
  }

//---------------------------------------------------------------------------

void CListWnd::OnPaint() 
  {
  if (pInfo==NULL)
    return;

	CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  CPen Pen;
  Pen.CreatePen(PS_SOLID, 0, RGB(0,0,0));
  CPen* OldPen = dc.SelectObject(&Pen);
  RECT Rect;
  GetClientRect(&Rect);
  dc.Rectangle(&Rect);
  if (VScrollBar.IsWindowVisible())
    Rect.right -= (ScrollBarWidth-1);
  if (HScrollBar.IsWindowVisible())
    Rect.bottom -= (ScrollBarWidth-1);
  CBrush Brush;
  Brush.CreateSolidBrush(RGB(255,255,255));
  CBrush* OldBrush = dc.SelectObject(&Brush);
  COLORREF PrevBkCol = RGB(255,255,255);
  COLORREF OldBkColor = dc.SetBkColor(PrevBkCol);
  dc.Rectangle(&Rect);
  dc.SetBoundsRect(&Rect, DCB_DISABLE);
	CFont* OldFont = dc.SelectObject(&Font);
  COLORREF PrevTxCol = RGB(0,0,0);
  COLORREF OldTextColor = dc.SetTextColor(PrevTxCol);

  //display text...
  int j,x;
  int y = 1;
  flag Changed = False;
  int FirstCol = 0;
  int FirstChar = pInfo->iHScrollPos;
  x = 0;
  for (j=0; j<pInfo->iColsUsed; j++)
    {
    x += pInfo->ColWidths[j];
    if (pInfo->iHScrollPos<x)
      {
      FirstCol = j;
      break;
      }
    FirstChar -= pInfo->ColWidths[j];
    }
  flag AllDone = (pInfo->iMaxLines<=0);
  int i = iStartLine;
  while (!AllDone)
    {
    if (i==iEndLine)
      AllDone = True;
    if (pInfo->bAllowSelect && i==pInfo->iCurLine)
      {
      PrevBkCol = dc.SetBkColor(RGB(0,0,128));
      PrevTxCol = dc.SetTextColor(RGB(255,255,255));
      Changed = True;
      }
    else if (i<pInfo->FlagsList.GetLen() && (pInfo->FlagsList[i] & LW_COL1))
      {
      PrevTxCol = dc.SetTextColor(pInfo->Color1);
      Changed = True;
      }
    Strng s;
    x = 2 + (pInfo->iMargin * TxtWidth);
    for (j=FirstCol; j<pInfo->iColsUsed; j++)
      {
      s = pInfo->StrList[j][i]();
      if (pInfo->bAllowSelect && i==pInfo->iCurLine)
        s.RPad(pInfo->ColWidths[j]);
      if (j==FirstCol)
        {
        dc.TextOut(x, y, &s[FirstChar], Min(s.Length(), pInfo->ColWidths[j])-FirstChar);
        x += ((TxtWidth * (pInfo->ColWidths[j]-FirstChar)) + 3);
        }
      else
        {
        dc.TextOut(x, y, s(), Min(s.Length(), pInfo->ColWidths[j]));
        x += ((TxtWidth * pInfo->ColWidths[j]) + 3);
        }
      }
    if (Changed)
      {
      dc.SetBkColor(PrevBkCol);
      dc.SetTextColor(PrevTxCol);
      Changed = False;
      }
    y += (TxtHeight + 1);
    i++;
    if (i>=pInfo->iMaxLines)
      i = 0;
    if (i==iStartLine)
      AllDone = True;
    }

  //display grid lines
  CPen Pen2;
  Pen2.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
  dc.SelectObject(&Pen2);
  if (pInfo->bVertLines && pInfo->iColsUsed>1)
    {
    x = 3 + (pInfo->iMargin * TxtWidth);
    for (j=FirstCol; j<pInfo->iColsUsed; j++)
      {
      if (j==FirstCol)
        x += (TxtWidth * (pInfo->ColWidths[j]-FirstChar));
      else
        x += (TxtWidth * pInfo->ColWidths[j]);
      dc.MoveTo(x, Rect.top);
      dc.LineTo(x, Rect.bottom);
      x += 3;
      }
    /*x = (TxtWidth * pInfo->ColWidths[0]) + 3 + (pInfo->iMargin * TxtWidth) - (pInfo->iHScrollPos * TxtWidth);
    for (j=Max(1,FirstCol); j<pInfo->iColsUsed; j++)
      {
      dc.MoveTo(x, Rect.top);
      dc.LineTo(x, Rect.bottom);
      x += ((TxtWidth * pInfo->ColWidths[j]) + 3);
      }*/
    }
  y = 2 + TxtHeight;
  for (i=iStartLine; i<=iEndLine; i++)
    {
    if (pInfo->bHorzLines || (i<pInfo->FlagsList.GetLen() && (pInfo->FlagsList[i] & LW_LINE)))
      {
      dc.MoveTo(Rect.left, y);
      dc.LineTo(Rect.right, y);
      }
    y += (TxtHeight + 1);
    }

  //display margin symbols...
  if (pInfo->iMargin>0)
    {
    CBrush Brush1;
    Brush1.CreateSolidBrush(RGB(255,255,0));
    //CPen Pen1;
    //Pen1.CreatePen(PS_SOLID, 0, RGB(255,255,0));
    CBrush Brush2;
    Brush2.CreateSolidBrush(RGB(128,0,0));
    CPen Pen2;
    Pen2.CreatePen(PS_SOLID, 0, RGB(128,0,0));
    CBrush Brush3;
    Brush3.CreateSolidBrush(RGB(0,0,128));
    y = 2;
    for (i=iStartLine; i<=iEndLine; i++)
      {
      if (pInfo->bUseSelectSymb && i==pInfo->iCurLine)
        {
        dc.SelectObject(&Pen);
        dc.SelectObject(&Brush3);
        dc.Rectangle(2, y, 2 + (TxtWidth*pInfo->iMargin), y + TxtHeight);
        }
      if (i<pInfo->FlagsList.GetLen() && (pInfo->FlagsList[i] & LW_SYMB2))
        {
        dc.SelectObject(&Pen2);
        dc.SelectObject(&Brush2);
        dc.Ellipse(2, y, 2 + Min(TxtWidth*pInfo->iMargin,TxtHeight), y + TxtHeight);
        }
      if (i<pInfo->FlagsList.GetLen() && (pInfo->FlagsList[i] & LW_SYMB1))
        {
        dc.SelectObject(&Pen);
        dc.SelectObject(&Brush1);
        dc.Rectangle(2, y+3, 2 + (TxtWidth*pInfo->iMargin), y + TxtHeight-3);
        }
      y += (TxtHeight + 1);
      }
    }

  //are the following lines neccessary ???
	dc.SelectObject(OldFont);
	dc.SetBkColor(OldBkColor);
  dc.SetTextColor(OldTextColor);
  dc.SelectObject(OldPen);
  dc.SelectObject(OldBrush);
	// Do not call CWnd::OnPaint() for painting messages
  }

//---------------------------------------------------------------------------

void CListWnd::AttachHelper(pCListWndHelper Info)
  { 
  pInfo = Info;
  if (pInfo==NULL)
    return;
  //if (GetParent()->IsWindowVisible())
    GetParent()->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_INITSCROLLBARS);
  }

//---------------------------------------------------------------------------

void CListWnd::SetCurLine(int CurLine, byte style, flag Repaint)
  {
  if (pInfo==NULL)
    return;

  ASSERT(pInfo);
  const int Mx = pInfo->iMaxLines-1;
  int PrevCurLine = pInfo->iCurLine;
  pInfo->iCurLine = Range(0, CurLine, Mx);
  if (pInfo->iCurLine<iStartLine || pInfo->iCurLine>iEndLine)
    {
    if (pInfo->bWrapArround)
      {
      switch (style)
        {
        case 0:
          iStartLine = pInfo->iCurLine;
          iEndLine = iStartLine+iHeight-1;
          break;
        case 1:
          iStartLine = pInfo->iCurLine - (iHeight / 2);
          iEndLine = iStartLine+iHeight-1;
          break;
        case 2:
          iEndLine = pInfo->iCurLine;
          iStartLine = iEndLine-iHeight+1;
          break;
        default:
          ASSERT(FALSE);
        }
      if (iEndLine>Mx)
        iEndLine -= pInfo->iMaxLines;
      if (iStartLine<0)
        iStartLine += pInfo->iMaxLines;
      }
    else
      {
      switch (style)
        {
        case 0:
          iStartLine = pInfo->iCurLine;
          iEndLine = Min(Mx, iStartLine+iHeight-1);
          break;
        case 1:
          iStartLine = Max(0, pInfo->iCurLine - (iHeight / 2));
          iEndLine = Min(pInfo->iMaxLines-1, iStartLine+iHeight-1);
          break;
        case 2:
          iEndLine = pInfo->iCurLine;
          iStartLine = Max(0, iEndLine-iHeight+1);
          break;
        default:
          ASSERT(FALSE);
        }
      iStartLine = Max(0, Min(iStartLine, iEndLine-iHeight+1));
      iEndLine = Min(Mx, Max(iEndLine, iStartLine+iHeight-1));
      pInfo->iVScrollPos = Min(iMaxVScroll, iStartLine);
      }
    }
  if (Repaint)
    Invalidate();
  }

//---------------------------------------------------------------------------

void CListWnd::InitScrollBars()
  {
  if (pInfo==NULL)
    return;
  ASSERT(pInfo);
  iMaxVScroll = Max(0, pInfo->iMaxLines-iHeight);
  iMaxHScroll = Max(0, pInfo->iMaxWidth-iWidth+pInfo->iMargin);
  VScrollBar.SetScrollRange(0, iMaxVScroll, FALSE);
  HScrollBar.SetScrollRange(0, iMaxHScroll, FALSE);
  }

//---------------------------------------------------------------------------

void CListWnd::UpdateScrollBars()
  {
  if (pInfo==NULL)
    return;
  ASSERT(pInfo);
  VScrollBar.ShowScrollBar(TRUE);
  VScrollBar.SetScrollPos(pInfo->iVScrollPos, TRUE);
  HScrollBar.ShowScrollBar(TRUE);
  HScrollBar.SetScrollPos(pInfo->iHScrollPos, TRUE);
  }

//---------------------------------------------------------------------------

void CListWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
  {
  if (pInfo==NULL)
    return;

  if (pScrollBar==&VScrollBar)
    {
    switch (nSBCode)
      {
      case SB_BOTTOM:// Scroll to bottom.
        pInfo->iVScrollPos = iMaxVScroll;
        break;
      case SB_TOP://Scroll to top.
        pInfo->iVScrollPos = 0;
        break;
      case SB_LINEDOWN://Scroll one line down.
        pInfo->iVScrollPos = pInfo->iVScrollPos + 1;
        break;
      case SB_LINEUP://Scroll one line up.
        pInfo->iVScrollPos = pInfo->iVScrollPos - 1;
        break;
      case SB_PAGEDOWN://Scroll one page down.
        pInfo->iVScrollPos = pInfo->iVScrollPos + iHeight;
        break;
      case SB_PAGEUP://Scroll one page up.
        pInfo->iVScrollPos = pInfo->iVScrollPos - iHeight;
        break;
      case SB_THUMBPOSITION://Scroll to the absolute position. The current position is provided in nPos.
        pInfo->iVScrollPos = nPos;
        break;
      case SB_THUMBTRACK://Drag scroll box to specified position. The current position is provided in nPos.
        pInfo->iVScrollPos = nPos;
        break;
      case SB_ENDSCROLL://End scroll.
        pInfo->iVScrollPos = Range(0, pInfo->iVScrollPos, iMaxVScroll);
        iStartLine = pInfo->iVScrollPos;
        iEndLine = Min(pInfo->iMaxLines-1, iStartLine + iHeight - 1);
        //SetCurLine(pInfo->iCurLine);
        VScrollBar.SetScrollPos(pInfo->iVScrollPos, TRUE);
        Invalidate();
        break;
      }
    }
	
  //CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void CListWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
  {
  if (pInfo==NULL)
    return;

  if (pScrollBar==&HScrollBar)
    {
    switch (nSBCode)
      {
      case SB_LEFT:
        pInfo->iHScrollPos = 0;
        break;
      case SB_RIGHT:
        pInfo->iHScrollPos = iMaxHScroll;
        break;
      case SB_LINELEFT:
        pInfo->iHScrollPos = pInfo->iHScrollPos-(iWidth/4);
        break;
      case SB_LINERIGHT:
        pInfo->iHScrollPos = pInfo->iHScrollPos+(iWidth/4);
        break;
      case SB_PAGELEFT:
        pInfo->iHScrollPos = pInfo->iHScrollPos - iWidth;
        break;
      case SB_PAGERIGHT:
        pInfo->iHScrollPos = pInfo->iHScrollPos + iWidth;
        break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
        pInfo->iHScrollPos = nPos;
        break;
      case SB_ENDSCROLL:
        pInfo->iHScrollPos = Range(0, pInfo->iHScrollPos, iMaxHScroll);
        HScrollBar.SetScrollPos(pInfo->iHScrollPos, TRUE);
        Invalidate();
        break;
      }
    }
		
	//CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void CListWnd::OnLButtonDown(UINT nFlags, CPoint point) 
  {
	if (pInfo)
    {
    int line = iStartLine + ((point.y - 1) / (TxtHeight+1));
    if (line!=pInfo->iCurLine)
      {
      pInfo->iCurLine = line;
      Invalidate();
      }
    }
	CWnd::OnLButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CListWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
  {
	GetParent()->PostMessage(WMU_UPDATEPGMDEBUG, DBG_MSG_DBLCLICK, (long)this);
	CWnd::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------


void CPGMDebugWnd::OnMouseMove(UINT nFlags, CPoint point) 
  {
   	// TODO: Add your message handler code here and/or call default
  //LoadCursor

  //if (point.x<0 || point.y<0)
  //HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_UPARROW);
	CDialog::OnMouseMove(nFlags, point);
  }

