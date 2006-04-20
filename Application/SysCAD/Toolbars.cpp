//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "toolbars.h"
#include "licbase.h"

#include "afxpriv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

CDynToolBar* pGrfTB = NULL;
CToolBarMngr* pGrfTBMngr = NULL;
CToolBarMngr* pMainTBMngr = NULL;

//===========================================================================

CDynToolBar::CDynToolBar() : CToolBar()
  {
  ASSERT(FALSE);
  }

CDynToolBar::CDynToolBar(char* pTitle, CToolBarMngr* TBMngr, BOOL CanConfigure, UINT WindowID)
  {
  sTitle = pTitle;
  pTBMngr = TBMngr;
  bCanConfigure = CanConfigure;
  iWindowID = WindowID;
  iLen = 0;
  iBitLen = 0;
  iTxtLen = 0;
  iComboLen = 0;
  iEditLen = 0;
  pBits = NULL;
  pTxtButtons = NULL;
  pComboButtons = NULL;
  pEditButtons = NULL;
  Location.x = 5;
  Location.y = 70;
  bComboListChanged = 0;
  bButtonsOnce = 0;
  bWasVisible = 1;
  bUseEscape = 0;
  Btns = NULL;
  pTheBitmap = NULL;
  }

//---------------------------------------------------------------------------

CDynToolBar::~CDynToolBar()
  {
  if (pBits)
    delete []pBits;
  if (pTxtButtons)
    delete []pTxtButtons;
  if (pComboButtons)
    delete []pComboButtons;
  if (pEditButtons)
    delete []pEditButtons;
  if (Btns)
    delete []Btns;
  }

//---------------------------------------------------------------------------

int CDynToolBar::AddButtons(UINT* pBtnIDs, UINT Len, BYTE Style)
  {
  int ActualLen = 0;
  for (UINT i=0; i<Len; i++)
    {
    BtnIDs.Add(pBtnIDs[i]);
    if (pBtnIDs[i]==ID_SEPARATOR)
      BtnStyles.Add(ButSeparator);
    else
      {
      BtnStyles.Add(Style);
      ActualLen++;
      }
    }
  iLen += Len;
  return ActualLen;
  }

//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDynToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CDynToolBar,CToolBar)
//{{AFX_MSG_MAP(CDynToolBar)
  ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

int CDynToolBar::AddButtons(UINT BitmapID, UINT* pBtnIDs, UINT Len)
  {
  ASSERT(!bButtonsOnce); //cannot call this if AddButtonsOnce has been used
  CToolBarBitmap* pSrcBitmap = pTBMngr->FindBitmap(BitmapID);
  if (pSrcBitmap==NULL)
    return -1;
  UINT ActLen = AddButtons(pBtnIDs, Len, ButBitmap);
  UINT NewBitLen = iBitLen + ActLen;
  char* pNewBits = new char[pTBMngr->BtnSize * NewBitLen];
  DWORD w1 = NewBitLen * pTBMngr->BtnWidth; //total width of new bit structure
  DWORD w2 = pSrcBitmap->iLen * pTBMngr->BtnWidth; //total width of source bit structure
  if (pBits)
    {//copy old stuff...
    DWORD w = pTBMngr->BtnWidth * iBitLen; //total width of old bit structure
    for (UINT i=0; i<pTBMngr->BtnHeight; i++)
      memcpy(&pNewBits[i*w1], &pBits[i*w], w);
    delete []pBits;
    }
  UINT k = 0;
  for (UINT i=0; i<Len; i++)
    {
    if (pBtnIDs[i]!=ID_SEPARATOR)
      {
      int index = pSrcBitmap->GetBtnIndex(pBtnIDs[i]);
      DWORD o1 = pTBMngr->BtnWidth * (k+iBitLen); //offset
      if (index>=0)
        {
        DWORD o2 = pTBMngr->BtnWidth * index; //offset
        for (UINT j=0; j<pTBMngr->BtnHeight; j++)
          memcpy(&pNewBits[o1+(j*w1)], &(pSrcBitmap->pBits[o2+(j*w2)]), pTBMngr->BtnWidth);
        }
      else
        for (UINT j=0; j<pTBMngr->BtnHeight; j++)
          memset(&pNewBits[o1+j*w1], 0, pTBMngr->BtnWidth);
      k++;
      }
    }
  pBits = pNewBits;
  iBitLen = NewBitLen;
  return 0;
  }

//---------------------------------------------------------------------------

int CDynToolBar::AddButtonsOnce(UINT BitmapID, UINT* pBtnIDs, UINT Len)
  {
  ASSERT(!bButtonsOnce); //cannot call this if AddButtonsOnce has been used
  ASSERT(pBits==NULL); //cannot call this if AddButtons has been used
  pTheBitmap = pTBMngr->FindBitmap(BitmapID);
  if (pTheBitmap==NULL)
    return -1;
  bButtonsOnce = 1;
  AddButtons(pBtnIDs, Len, ButBitmap);
  iBitLen = pTheBitmap->iLen;
  const int Size = pTBMngr->BtnSize * iBitLen;
  pBits = new char[Size];
  memcpy(pBits, pTheBitmap->pBits, Size);
  return 0;
  }

//---------------------------------------------------------------------------

int CDynToolBar::ChangeButtons(UINT* pBtnIDs, UINT Len)
  {
  ASSERT(bButtonsOnce); //AddButtonsOnce should have been called
  CToolBarCtrl& bc = GetToolBarCtrl();
  BOOL Done = bc.DeleteButton(0);
  while (Done)
    Done = bc.DeleteButton(0);
  iLen = 0;
  BtnIDs.SetSize(0);
  BtnStyles.SetSize(0);
  AddButtons(pBtnIDs, Len, ButBitmap);
  if (!DoCreateButtons())
    return -1;
  return 0;
  }

//---------------------------------------------------------------------------

void CDynToolBar::AddTextButton(char* pTxt, UINT BtnID)
  {
  AddButtons(&BtnID, 1, ButText);
  iTxtLen++;
  BtnTxts.Add(pTxt);
  }

//---------------------------------------------------------------------------

void CDynToolBar::AddComboButton(UINT BtnID)
  {
  AddButtons(&BtnID, 1, ButCombo);
  iComboLen++;
  }

//---------------------------------------------------------------------------

void CDynToolBar::AddEditButton(UINT BtnID)
  {
  AddButtons(&BtnID, 1, ButEdit);
  iEditLen++;
  }

//---------------------------------------------------------------------------

void CDynToolBar::AddSeparator()
  {
  UINT BtnID = ID_SEPARATOR;
  AddButtons(&BtnID, 1, ButSeparator);
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::DoCreateButtons()
  {
  if (Btns)
    delete []Btns;
  CToolBarCtrl& bc = GetToolBarCtrl();
  int BitCnt = 0;
  Btns = new TBBUTTON[iLen];
  for (UINT i=0; i<iLen; i++)
    {
    switch (BtnStyles[i])
      {
      case ButBitmap:
        if (bButtonsOnce)
          {
          int index = pTheBitmap->GetBtnIndex(BtnIDs[i]);
          // CNM VERIFY(index>=0);
          Btns[i].iBitmap = index;
          }
        else
          Btns[i].iBitmap = BitCnt++;
        Btns[i].fsStyle = TBSTYLE_BUTTON;
        Btns[i].idCommand = BtnIDs[i];
        break;
      case ButSeparator:
      case ButText:
      case ButCombo:
        Btns[i].iBitmap = 0;
        Btns[i].fsStyle = TBSTYLE_SEP;
        Btns[i].idCommand = 0;
        break;
      }
    Btns[i].fsState = TBSTATE_ENABLED;
    Btns[i].dwData = 0;
    Btns[i].iString = 0;
    }
  if (!bc.AddButtons(iLen, Btns))
    {
    delete []Btns;
    Btns = NULL;
    return FALSE;
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

int CDynToolBar::Create(BOOL ShowTitle, BOOL FloatIt, DWORD dwDockingStyle, DWORD dwBarStyle, BOOL Show, BOOL UseEscape)
  {
  DWORD dwStyle = WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY;
  if (Show)
    dwStyle |= WS_VISIBLE;
  if (bCanConfigure)
    dwStyle |= CCS_ADJUSTABLE;
  if (!CToolBar::Create(pTBMngr->pMainFrame, dwStyle, iWindowID))
    return -1;
  //if (!SetButtons(pBtnIDs, iLen))
  //  return -2; 

  BITMAP bm;
  bm.bmType = 0;
  bm.bmWidth = pTBMngr->BtnWidth * iBitLen * 8 / pTBMngr->BitsPixel;
  bm.bmHeight = pTBMngr->BtnHeight;
  bm.bmWidthBytes = pTBMngr->BtnWidth * iBitLen;
  bm.bmPlanes = 1;
  bm.bmBitsPixel = pTBMngr->BitsPixel;
  bm.bmBits = (LPVOID)pBits;
  if (!Bitmap.CreateBitmapIndirect(&bm))
    return -3;
  CToolBarCtrl& bc = GetToolBarCtrl();
  if (bc.AddBitmap(iBitLen/*(bButtonsOnce ? iBitLen : iLen)*/, &Bitmap)<0)
    return -4;
  if (!DoCreateButtons())
    return -5;

  if (iTxtLen>0)
    {
    pTxtButtons = new CButton[iTxtLen];
    UINT TxtCnt = 0;
    CRect rect;
    for (UINT i=0; i<iLen; i++)
      if (BtnStyles[i]==ButText)
        {
        SetButtonInfo(i, BtnIDs[i], TBBS_SEPARATOR, (BtnTxts[TxtCnt].GetLength()+3)*6);
        GetItemRect(i, &rect);
        pTxtButtons[TxtCnt].Create((const char*)(BtnTxts[TxtCnt]), WS_VISIBLE|WS_TABSTOP, rect, this, BtnIDs[i]);
        pTxtButtons[TxtCnt].SetFont(&(pTBMngr->Font));
        TxtCnt++;
        }
    }
  
  if (iComboLen>0)
    {
    bComboListChanged = 0;
    const int nDropHeight = 100;
    pComboButtons = new CComboBox[iComboLen];
    UINT ComboCnt = 0;
    CRect rect;
    for (UINT i=0; i<iLen; i++)
      if (BtnStyles[i]==ButCombo)
        {
        SetButtonInfo(i, BtnIDs[i], TBBS_SEPARATOR, 120);
        GetItemRect(i, &rect);
        rect.top = 3;
        rect.bottom = rect.top + nDropHeight;
        pComboButtons[ComboCnt].Create(CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_VISIBLE|WS_TABSTOP, rect, this, ID_TOOLBAR_COMBO_0+ComboCnt);//BtnIDs[i]);
        pComboButtons[ComboCnt].SetFont(&(pTBMngr->Font));
        ComboCnt++;
        }
    }

  if (iEditLen>0)
    {
    pEditButtons = new CEdit[iEditLen];
    UINT EditCnt = 0;
    CRect rect;
    for (UINT i=0; i<iLen; i++)
      if (BtnStyles[i]==ButEdit)
        {
        SetButtonInfo(i, BtnIDs[i], TBBS_SEPARATOR, 120);
        GetItemRect(i, &rect);
        //rect.top = 3;
        //rect.bottom = rect.top + nDropHeight;
        pEditButtons[EditCnt].Create(WS_VISIBLE|WS_TABSTOP, rect, this, BtnIDs[i]);//ID_TOOLBAR_EDIT_0+EditCnt);
        pEditButtons[EditCnt].SetFont(&(pTBMngr->Font));
        EditCnt++;
        }
    }
  
  SetBarStyle(GetBarStyle() | dwBarStyle);
  if (ShowTitle)
    SetWindowText((const char*)sTitle);
  EnableDocking(dwDockingStyle);
  if (FloatIt)
    pTBMngr->pMainFrame->FloatControlBar(this, Location);
  else
    pTBMngr->pMainFrame->DockControlBar(this);
  bUseEscape = UseEscape;
  bWasVisible = Show;
  return 0;
  }

//---------------------------------------------------------------------------

void CDynToolBar::SaveState(char* pFilename)
  {
  CProfINIFile PF(pFilename);
  Strng Section;
  //Section.Set("ToolBar%s_%d", gs_License.IsRunTime() ? "_rt" : "", iWindowID);
  Section.Set("ToolBar_%d", iWindowID);
  DWORD BarStyle = GetBarStyle();
  DWORD Style = GetStyle();
  CRect Rect;
  CWnd* pWnd = GetParent();
  if (BarStyle & CBRS_FLOATING)
    {
    if (pWnd && pWnd->GetParent())
      {
      pWnd->GetParent()->GetWindowRect(&Rect);
      if (PrjFileVerNo()>=25)
        {
        CRect MRect;
        AfxGetMainWnd()->GetWindowRect(MRect);
        Rect.OffsetRect(-MRect.left, -MRect.top);
        }
      PF.WrInt(Section(), "Left", Rect.left);
      PF.WrInt(Section(), "Top", Rect.top);
      }
    }
  else
    {
    if (pWnd)
      {
      //pWnd->GetWindowRect(&Rect);
      GetWindowRect(&Rect);
      if (PrjFileVerNo()>=25)
        {
        CRect MRect;
        AfxGetMainWnd()->GetWindowRect(MRect);
        Rect.OffsetRect(-MRect.left, -MRect.top);
        }
      //pWnd->ScreenToClient(&Rect);
      PF.WrInt(Section(), "Left", Rect.left);
      PF.WrInt(Section(), "Top", Rect.top);
      }
    }
  PF.WrStr(Section(), "Title", (char*)(const char*)sTitle);
  PF.WrLong(Section(), "BarStyle", BarStyle);
  flag Visible = ((Style & WS_VISIBLE) != 0);
  PF.WrInt(Section(), "Visible", Visible);
  if (bCanConfigure)
    {
    CToolBarCtrl& bc = GetToolBarCtrl();
    const int Cnt = bc.GetButtonCount();
    PF.WrInt(Section(), "ButtonIDCount", Cnt);
    Strng Item;
    TBBUTTON Btn;
    for (int i=0; i<Cnt; i++)
      {
      Item.Set("ID_%d", i);
      if (bc.GetButton(i, &Btn))
        PF.WrInt(Section(), Item(), Btn.idCommand);
      else
        PF.WrInt(Section(), Item(), ID_SEPARATOR);
      }
    }
  }

//---------------------------------------------------------------------------

void CDynToolBar::LoadState(char* pFilename)
  {
  CProfINIFile PF(pFilename);
  CMDIFrameWnd* pM = pTBMngr->pMainFrame;
  Strng Section;
  Section.Set("ToolBar_%d", iWindowID);
  Strng NewTitle = PF.RdStr(Section(), "Title", (char*)(const char*)sTitle);
  if (NewTitle.Length()!=sTitle.GetLength() || strcmp(NewTitle(), (const char*)sTitle)!=0)
    {
    SetWindowText(NewTitle());
    sTitle = NewTitle();
    }
  DWORD BarStyle = GetBarStyle();
  DWORD Style = GetStyle();
  BarStyle = PF.RdLong(Section(), "BarStyle", BarStyle);
  BarStyle &= CBRS_ALL; //CNM Prevent an ASSERT
  SetBarStyle(BarStyle);

  if (bCanConfigure)
    {
    CToolBarCtrl& bc = GetToolBarCtrl();
    int Cnt = bc.GetButtonCount();
    Cnt = PF.RdInt(Section(), "ButtonIDCount", Cnt);
    BOOL Chngd = (Cnt!=bc.GetButtonCount());
    UINT* NewIDs = new UINT[Cnt];
    Strng Item;
    for (int i=0; i<Cnt; i++)
      {
      Item.Set("ID_%d", i);
      int Cmd = (i<(int)iLen ? BtnIDs[i] : ID_SEPARATOR);
      Cmd = PF.RdInt(Section(), Item(), Cmd);
      NewIDs[i] = (UINT)Cmd;
      Chngd = (Chngd || (NewIDs[i]!=BtnIDs[i]));
      }
    if (Chngd)
      ChangeButtons(NewIDs, Cnt);
    delete []NewIDs;
    }

  flag Visible = ((Style & WS_VISIBLE) != 0);
  Visible = PF.RdInt(Section(), "Visible", Visible);
  pM->ShowControlBar(this, Visible, FALSE);
  pM->RecalcLayout(); //get MFC to adjust the dimensions of all docked ToolBars so that GetWindowRect will be accurate
  CRect Rect;
  CWnd* pWnd = GetParent();
  if (BarStyle & CBRS_FLOATING)
    {
    if (pWnd && pWnd->GetParent())
      {
      if (PrjFileVerNo()>=25)
        {
        CRect MRect;
        AfxGetMainWnd()->GetWindowRect(MRect);
        Rect.OffsetRect(-MRect.left, -MRect.top);
        int L=PF.RdInt(Section(), "Left", Rect.left);
        int T=PF.RdInt(Section(), "Top", Rect.top);
        Rect.OffsetRect(L-Rect.left, T-Rect.top);
        Rect.OffsetRect(+MRect.left, +MRect.top);
        }
      else
        {
        pWnd->GetParent()->GetWindowRect(&Rect);
        Location.x = PF.RdInt(Section(), "Left", Rect.left);
        Location.y = PF.RdInt(Section(), "Top", Rect.top);
        }
      pM->FloatControlBar(this, Location);
      }
    }
  else
    {
    GetWindowRect(&Rect);
    if (PrjFileVerNo()>=25)
      {
      CRect MRect;
      AfxGetMainWnd()->GetWindowRect(MRect);
      Rect.OffsetRect(-MRect.left, -MRect.top);
      int L=PF.RdInt(Section(), "Left", Rect.left);
      int T=PF.RdInt(Section(), "Top", Rect.top);
      Rect.OffsetRect(L-Rect.left, T-Rect.top);
      Rect.OffsetRect(+MRect.left, +MRect.top);
      }
    else
      {
    //Rect.left = PF.RdInt(Section(), "Left", Rect.left);
    //Rect.top = PF.RdInt(Section(), "Top", Rect.top);
      int L=PF.RdInt(Section(), "Left", Rect.left);
      int T=PF.RdInt(Section(), "Top", Rect.top);
      Rect.right  += L-Rect.left;
      Rect.bottom += T-Rect.top;
      Rect.left   += L-Rect.left;
      Rect.top    += T-Rect.top;
      }
    //Rect.top = PF.RdInt(Section(), "Top", Rect.top);
    UINT n = 0;
    if (BarStyle & CBRS_ALIGN_TOP)
      {
      Rect.top -= 1;
      Rect.bottom -= 1;
      n = AFX_IDW_DOCKBAR_TOP;
      }
    if (n==0 && BarStyle & CBRS_ALIGN_BOTTOM)
      {
      Rect.top += 1;
      Rect.bottom += 1;
      n = AFX_IDW_DOCKBAR_BOTTOM;
      }
    if (n==0 && BarStyle & CBRS_ALIGN_LEFT)
      {
      Rect.left -= 1;
      Rect.right -= 1;
      n = AFX_IDW_DOCKBAR_LEFT;
      }
    if (n==0 && BarStyle & CBRS_ALIGN_RIGHT)
      {
      Rect.left += 1;
      Rect.right += 1;
      n = AFX_IDW_DOCKBAR_RIGHT;
      }
    pM->DockControlBar(this, n, &Rect);
    }
  pM->RecalcLayout();
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::GetPos(CPoint& Pt)
  {
  if (GetParent() && GetParent()->GetParent())
    {
    RECT Rect;
    GetParent()->GetParent()->GetWindowRect(&Rect);
    Location.x = Rect.left;
    Location.y = Rect.top;
    Pt = Location;
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

int CDynToolBar::FindComboButtonIndex(CWnd* pWnd)
  {
  for (int i=0; i<(int)iComboLen; i++)
    if (&pComboButtons[i]==pWnd)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

int CDynToolBar::FindComboButtonIDIndex(int ComboButtonIDIndex)
  {
  int ComboCnt = 0;
  for (int i=0; i<(int)iLen; i++)
    if (BtnStyles[i]==ButCombo && ComboCnt++==ComboButtonIDIndex)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

void CDynToolBar::SetTitle(char* pTxt, BOOL SaveChange)
  {
  SetWindowText(pTxt);
  if (SaveChange)
    sTitle = pTxt;
  if ((GetBarStyle() & CBRS_FLOATING) && (GetStyle() & WS_VISIBLE))
    {
    RECT Rect;
    CWnd* pWnd = GetParent();
    if (pWnd && pWnd->GetParent())
      {
      pWnd->GetParent()->GetWindowRect(&Rect);
      Location.x = Rect.left;
      Location.y = Rect.top;
      }
    //new title in floating toolbar is not shown unless the toolbar is docked and 
    //then floated; ie unless the toolbar frame is re-created
    ShowWindow(SW_HIDE); //reduce flicker
    pTBMngr->pMainFrame->DockControlBar(this);
    ShowWindow(SW_SHOWNA);
    pTBMngr->pMainFrame->FloatControlBar(this, Location);
    }
  }

//---------------------------------------------------------------------------

void CDynToolBar::AppendToTitle(char* pTxt)
  {
  CString NewTitle = sTitle;
  NewTitle += " - ";
  NewTitle += pTxt;
  SetTitle((char*)(const char*)NewTitle, FALSE);
  }

//---------------------------------------------------------------------------

void CDynToolBar::SetTextButtonText(char* pTxt, int Index)
  {
  pTxtButtons[Index].SetWindowText(pTxt);
  BtnTxts[Index] = pTxt;
  }

//---------------------------------------------------------------------------

void CDynToolBar::Customize()
  {
  CToolBarCtrl& TBC = GetToolBarCtrl();
  TBC.Customize();
  pTBMngr->pMainFrame->RecalcLayout();
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::PreTranslateMessage(MSG* pMsg)
  {// Handle a few messages for our children...
  CWnd* pWnd = FromHandle(pMsg->hwnd); //Get a temp CWnd object for the message we're processing.
  //if (pMsg->message!=WM_PAINT)
  //  {
  //  sprintf(buff, "H=%08x ", (DWORD)(pWnd->m_hWnd));
  //  dbgWM_Msg(buff, pMsg->message, pMsg->wParam, pMsg->lParam, 1);
  //  }
  if (IsChild(pWnd))
    {//It's a child of ours...
    CWnd* pParent = pWnd->GetParent();
    if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CComboBox)))
      {//The parent is a ComboBox!
      switch (pMsg->message)
        {
        case WM_KEYDOWN:
          if (pMsg->wParam==VK_RETURN)
            {
            int Index = FindComboButtonIndex(pParent);
            if (Index>=0)
              {
              int IDIndex = FindComboButtonIDIndex(Index);
              if (IDIndex>=0)
                {
                CComboBox* pCBox = (CComboBox*)pParent;
                int nIndex = pCBox->GetCurSel();
                if (nIndex==CB_ERR)
                  {
                  pCBox->GetWindowText(sCurSel);
                  if (sCurSel.GetLength()>0 && pCBox->FindStringExact(-1, (const char*)sCurSel)==CB_ERR)
                    {
                    bComboListChanged = 1;
                    pCBox->InsertString(0, (const char*)sCurSel);
                    pCBox->SetCurSel(0);
                    }
                  }
                else
                  {
                  pCBox->GetLBText(nIndex, sCurSel);
                  if (nIndex>0)
                    {
                    bComboListChanged = 1;
                    pCBox->DeleteString(nIndex);
                    pCBox->InsertString(0, (const char*)sCurSel);
                    pCBox->SetCurSel(0);
                    }
                  }
                AfxGetMainWnd()->PostMessage(WM_COMMAND, BtnIDs[IDIndex]);
                }
              }
            }
          else if (bUseEscape && pMsg->wParam==VK_ESCAPE)
            {
            AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_GRF_Esc);
            }
          break;
        }//end switch
      }
    else if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CEdit)))
      {//The parent is a CEdit!
      switch (pMsg->message)
        {
        case WM_KEYDOWN:
          if (pMsg->wParam==VK_RETURN)
            {
            AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_GRF_Done);
            }
          else if (bUseEscape && pMsg->wParam==VK_ESCAPE)
            {
            AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_GRF_Esc);
            }
          break;
        }//end switch
      }
    }
  return CToolBar::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

// Hook into MFC code to overwrite some behaviour, want to 
LRESULT CDynToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
  {
  if (bUseEscape)
    {
    BOOL bVis = IsWindowVisible();
    if (bWasVisible && !bVis)
      AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_GRF_Esc);
    bWasVisible = (bVis!=0);
    }
  return CControlBar::OnIdleUpdateCmdUI(wParam, lParam);
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// MFC routes the notifications sent to the parent of the control to
// the control before the parent can process the notification.
// The control object can handle the notification or ignore it.
// If the notification is handled then return TRUE. Otherwise MFC
// will route it to the parent of the control.
BOOL CDynToolBar::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
  {
  if (message == WM_NOTIFY)
    {
    NMHDR *pNMHDR = (NMHDR*)lParam;
    switch (pNMHDR->code)
      {
      case NM_RCLICK:         return RClick(wParam, lParam, pLResult);
      case TBN_BEGINADJUST:   return BeginAdjust(wParam, lParam, pLResult);
      case TBN_BEGINDRAG:     return BeginDrag(wParam, lParam, pLResult);
      case TBN_CUSTHELP:      return CustomizeHelp(wParam, lParam, pLResult);
      case TBN_ENDADJUST:     return EndAdjust(wParam, lParam, pLResult);
      case TBN_ENDDRAG:       return EndDrag(wParam, lParam, pLResult);
      case TBN_GETBUTTONINFO: return GetButtonInfo(wParam, lParam, pLResult);
      case TBN_QUERYDELETE:   return QueryDelete(wParam, lParam, pLResult);
      case TBN_QUERYINSERT:   return QueryInsert(wParam, lParam, pLResult);
      case TBN_RESET:         return Reset(wParam, lParam, pLResult);
      case TBN_TOOLBARCHANGE: return ToolBarChange(wParam, lParam, pLResult);
      }
    }

  return CToolBar::OnChildNotify(message, wParam, lParam, pLResult);
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::RClick(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  if (bCanConfigure)
    {
    Customize();
    return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::BeginAdjust(WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
  {
  // the customize dialog box is about to be displayed
  // save toolbar state before customization using the dialog
  // Use this information to restore the state if reset button is pressed
  CToolBarCtrl& TBC = GetToolBarCtrl();
  TBC.SaveState(HKEY_CURRENT_USER, _T("Software\\Kenwalt\\SysCAD"), _T("Dynamic Tool Bar"));
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::BeginDrag(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // we are not implementing custon drag and drop 
  *pLResult = FALSE;
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::CustomizeHelp(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // Sample displays a message box but a valid help topic
  // can be displayed for the customize dialog for this toolbar
  //AfxMessageBox(_T("Help not implemented!"));
  //return TRUE;
  return FALSE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::EndAdjust(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // the customize dialog box has been closed
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::EndDrag(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // Code to handle custom drag and drop. This message indicates that 
  // the item is being dropped
  *pLResult = FALSE;
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::GetButtonInfo(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // This notification message has to be handled correctly if 
  // all operations in the custom dialogbox has to function correctly
  // We have to supply information for the button specified by pTBN->tbButton
  //
  // This notification is sent in the following cases
  // 
  // After TBN_BEGINADJUST the control sends these notifications until
  // * pLResult is TRUE. We have to supply valid values when this value is
  // set to TRUE. Here the control is collecting information for all
  // the buttons that have to be displayed in the dialog box
  // 
  // The control sends this notification to get information about
  // a button if the user is trying to add it to the toolbar or 
  // rearranging the buttons on the toolbar from within the dialog
  TBNOTIFY* pTBN = (TBNOTIFY*)lParam;
  UINT StrID;
  if (bButtonsOnce)
    {
    if (pTBN->iItem >= (int)(pTheBitmap->iLen))
      *pLResult = FALSE;
    else
      {
      StrID = pTheBitmap->BtnIDs[pTBN->iItem];
      pTBN->tbButton = pTheBitmap->Btns[pTBN->iItem];
      *pLResult = TRUE;
      }
    }
  else
    {
    if (pTBN->iItem >= (int)iLen)
      *pLResult = FALSE;
    else
      {
      StrID = BtnIDs[pTBN->iItem];
      pTBN->tbButton = Btns[pTBN->iItem];
      *pLResult = TRUE;
      }
    }
  if (*pLResult == TRUE)
    {
    CString Txt;
    Txt.LoadString(StrID);
    Strng s;
    if (Txt.Find('\n')>=0)
      {
      s = (const char*)Txt.Mid(Txt.Find('\n')+1, 256);
      Txt = Txt.Left(Txt.Find('\n'));
      if (s.Length()>0)
        s += " : ";
      }
    s += (const char*)Txt;
    if (s.Length()>0)
      {
      _tcsncpy(pTBN->pszText, s(), pTBN->cchText - 1);
      pTBN->pszText[pTBN->cchText - 1] = '\0';
      }
    else
      pTBN->pszText[0] = '\0';
    }
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::QueryDelete(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // in this sample any button can be deleted
  // if a particular button cannot be deleted set *pResult to FALSE for that item
  *pLResult = TRUE;
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::QueryInsert(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult)
  {
  // in this sample buttons can be inserted at any location on the toolbar
  *pLResult = TRUE;
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::Reset(WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
  {
  // User has pressed the reset button
  // restore the state of the toolbar to the state it was before customization
  CToolBarCtrl& TBC = GetToolBarCtrl();
  TBC.RestoreState(HKEY_CURRENT_USER, _T("Software\\Kenwalt\\SysCAD"), _T("Dynamic Tool Bar"));
  *pLResult = TRUE;
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CDynToolBar::ToolBarChange(WPARAM wParam, LPARAM lParam,LRESULT* pLResult)
  {
  // the toolbar has changed
  return TRUE;
  }

//---------------------------------------------------------------------------

void CDynToolBar::ChangeButtonIDS(UINT ID1, UINT ID2, UINT IDRQD)
  {
  if (bCanConfigure)
    {
    CToolBarCtrl& bc = GetToolBarCtrl();
    int Cnt = bc.GetButtonCount();
    BOOL Chngd = FALSE;
    UINT* NewIDs = new UINT[Cnt];
    Strng Item;
    for (int i=0; i<Cnt; i++)
      {
      TBBUTTON B;
      bc.GetButton(i, &B);
      NewIDs[i]=B.idCommand;
      if (B.idCommand==(int)ID1 || B.idCommand==(int)ID2)
        {
        Chngd=true;
        NewIDs[i]=IDRQD;
        }
      }
    if (Chngd)
      ChangeButtons(NewIDs, Cnt);
    delete []NewIDs;
    }
  // the toolbar has changed
  }

//---------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// This has been overridden so we can handle the tooltip TTN_NEEDTEXT//
// notification message                                              //
///////////////////////////////////////////////////////////////////////
/*BOOL CDynToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
  {
  ASSERT(pResult != NULL);
  NMHDR* pNMHDR = (NMHDR*)lParam;
  HWND hWndCtrl = pNMHDR->hwndFrom;
  // get the child ID from the window itself
  // UINT nID = _AfxGetDlgCtrlID(hWndCtrl); 
  //////////////////////////////////////////////////////////////////
  // If TTN_NEEDTEXT we cannot get the ID from the tooltip window //
  //////////////////////////////////////////////////////////////////
  int nCode = pNMHDR->code;
  //
  // if it is the following notification message 
  // nID has to obtained from wParam
  //
  if (nCode == TTN_NEEDTEXTA || nCode == TTN_NEEDTEXTW)
    {
    UINT nID;   // = _AfxGetDlgCtrlID(hWndCtrl);
    nID = (UINT)wParam;

    ASSERT((UINT)pNMHDR->idFrom == (UINT)wParam);
    UNUSED(wParam);  // not used in release build
    ASSERT(hWndCtrl != NULL);
    ASSERT(::IsWindow(hWndCtrl));

    if (AfxGetThreadState()->m_hLockoutNotifyWindow == m_hWnd)
      return TRUE;        // locked out - ignore control notification

    // reflect notification to child window control
    if (ReflectLastMsg(hWndCtrl, pResult))
      return TRUE;        // eaten by child

    AFX_NOTIFY notify;
    notify.pResult = pResult;
    notify.pNMHDR = pNMHDR;
    //return OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL);
    //CString toolTipText = NeedText(nID, &notify, lResult);
    LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)&notify;
    ASSERT(nID == lpTTT->hdr.idFrom); 
    CString toolTipText;
    toolTipText.LoadString(nID);
    int nLength = (toolTipText.GetLength() > 79) ? 79 : toolTipText.GetLength();
    toolTipText = toolTipText.Left(nLength);
    if (nCode == TTN_NEEDTEXTW)
      {
      LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)&notify;
      #ifndef _UNICODE
      mbstowcs(lpTTT->szText, (LPCSTR)toolTipText, toolTipText.GetLength() + 1); 
      #else
      _tcsncpy(lpTTT->szText, toolTipText, toolTipText.GetLength() + 1);
      #endif
      }
    else
      {
      LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)&notify;
      _tcscpy(lpTTT->szText,(LPCTSTR)toolTipText);
      }
    return TRUE;
    }
  return CToolBar::OnNotify(wParam, lParam, pResult);
  }*/

//===========================================================================

CToolBarBitmap::CToolBarBitmap(UINT BitmapID, UINT* pBtnIDs, UINT Len, CToolBarMngr* TBMngr)
  {
  iBitmapID = BitmapID;
  iLen = Len;
  int OK = Bitmap.LoadMappedBitmap(iBitmapID);
  BITMAP bm;
  OK = Bitmap.GetBitmap(&bm);
  TBMngr->BitsPixel = bm.bmBitsPixel;
  TBMngr->BtnWidth = 2 * TBMngr->BitsPixel;
  TBMngr->BtnSize = TBMngr->BtnWidth * TBMngr->BtnHeight;
  DWORD ExpectedLen = Len * TBMngr->BtnWidth * TBMngr->BtnHeight;
  pBits = new char[ExpectedLen];
  DWORD len = Bitmap.GetBitmapBits(ExpectedLen, (LPVOID)pBits);
  VERIFY(len==ExpectedLen);
  Btns = new TBBUTTON[iLen];
  BtnIDs.SetSize(iLen);
  for (UINT i=0; i<iLen; i++)
    {
    BtnIDs[i] = pBtnIDs[i];
    Btns[i].iBitmap = i;
    Btns[i].fsStyle = TBSTYLE_BUTTON;
    Btns[i].idCommand = pBtnIDs[i];
    Btns[i].fsState = TBSTATE_ENABLED;
    Btns[i].dwData = 0;
    Btns[i].iString = 0;
    }
  }

//---------------------------------------------------------------------------

CToolBarBitmap::~CToolBarBitmap()
  {
  if (pBits)
    delete []pBits;
  if (Btns)
    delete []Btns;
  }

//---------------------------------------------------------------------------

int CToolBarBitmap::GetBtnIndex(UINT BtnID)
  {
  for (UINT i=0; i<iLen; i++)
    if (BtnIDs[i]==BtnID)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------
//===========================================================================
    
CToolBarMngr::CToolBarMngr()
  {
  pMainFrame = NULL;
  iBitmapCnt = 0;
  for (int i=0; i<MaxDynBitmaps; i++)
    Bitmaps[i] = NULL;
  for (i=0; i<MaxDynToolBars; i++)
    ToolBars[i] = NULL;
  }

//---------------------------------------------------------------------------

CToolBarMngr::~CToolBarMngr()
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i])
      delete ToolBars[i];
  for (i=0; i<MaxDynBitmaps; i++)
    if (Bitmaps[i])
      delete Bitmaps[i];
  }

//---------------------------------------------------------------------------

//void CToolBarMngr::Initialise(CMainFrame* MainFrame)
void CToolBarMngr::Initialise(CMDIFrameWnd* MainFrame)
  {
  pMainFrame = MainFrame;
  BtnHeight = 15;
  BitsPixel = 8;
  BtnWidth = 2 * BitsPixel;
  BtnSize = BtnWidth * BtnHeight;

  //Create a font to be used on the toolbars...
  LOGFONT logFont;
  memset(&logFont, 0, sizeof(logFont));
  if (!::GetSystemMetrics(SM_DBCSENABLED))
    {
    // Since design guide says toolbars are fixed height so is the font.
    logFont.lfHeight = -12;
    logFont.lfWeight = FW_REGULAR;//FW_BOLD;
    logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    lstrcpy(logFont.lfFaceName, "MS Sans Serif");
    Font.CreateFontIndirect(&logFont);
    }
  else
    Font.Attach(::GetStockObject(SYSTEM_FONT));
  }

//---------------------------------------------------------------------------

int CToolBarMngr::AddBitmap(UINT BitmapID, UINT* pBtnIDs, UINT Len)
  {
  if (iBitmapCnt>=MaxDynBitmaps)
    return -1;
  if (FindBitmap(BitmapID))
    return -2; //allready exists
  Bitmaps[iBitmapCnt] = new CToolBarBitmap(BitmapID, pBtnIDs, Len, this);
  iBitmapCnt++;
  return iBitmapCnt-1;
  }

//---------------------------------------------------------------------------

CToolBarBitmap* CToolBarMngr::FindBitmap(UINT BitmapID)
  {
  for (int i=0; i<iBitmapCnt; i++)
    if (Bitmaps[i]->iBitmapID==BitmapID)
      return Bitmaps[i];
  return NULL;
  }

//---------------------------------------------------------------------------

CDynToolBar* CToolBarMngr::AddToolbar(char* pTitle, BOOL CanConfigure, UINT WindowID)
  {
  if (FindToolbar(pTitle))
    return NULL; //toolbar with this title allready exists
  int index = 0;
  while (index<MaxDynToolBars && ToolBars[index])
    index++;
  if (index>=MaxDynToolBars)
    return NULL;
  ToolBars[index] = new CDynToolBar(pTitle, this, CanConfigure, WindowID);
  return ToolBars[index];
  }

//---------------------------------------------------------------------------

CDynToolBar* CToolBarMngr::FindToolbar(char* pTitle)
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i] && _stricmp((const char*)ToolBars[i]->sTitle, pTitle)==0)
      return ToolBars[i];
  return NULL;
  }

//---------------------------------------------------------------------------

int CToolBarMngr::FindToolbar(CDynToolBar* pTB)
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i] && ToolBars[i]==pTB)
      return i;
  return -1;
  }

//---------------------------------------------------------------------------

void CToolBarMngr::RemoveToolbar(char* pTitle)
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i] && _stricmp((const char*)ToolBars[i]->sTitle, pTitle)==0)
      {
      delete ToolBars[i];
      ToolBars[i] = NULL;
      }
  }

//---------------------------------------------------------------------------

void CToolBarMngr::RemoveAllToolbars()
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i])
      {
      delete ToolBars[i];
      ToolBars[i] = NULL;
      }
  }

//---------------------------------------------------------------------------

void CToolBarMngr::SaveState(char* pFilename)
  {
  pMainFrame->RecalcLayout(); //get MFC to adjust the dimensions of all docked ToolBars so that GetWindowRect will be accurate
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i])
      ToolBars[i]->SaveState(pFilename);
  }

//---------------------------------------------------------------------------

void CToolBarMngr::LoadState(char* pFilename)
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i])
      ToolBars[i]->LoadState(pFilename);
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CToolBarMngr::ChangeButtonIDS(UINT ID1, UINT ID2, UINT IDRQD)
  {
  for (int i=0; i<MaxDynToolBars; i++)
    if (ToolBars[i])
      ToolBars[i]->ChangeButtonIDS(ID1, ID2, IDRQD);

//  if (bCanConfigure)
//    {
//    CToolBarCtrl& bc = GetToolBarCtrl();
//    int Cnt = bc.GetButtonCount();
//    for (int i=0; i<Cnt; i++)
//      {
//      TBBUTTON B;
//      bc.GetButton(0, &B);
//      if (B.idCommand==ID1 || B.idCommand==ID2)
//        {
//        }
//
//      //int Cmd = (i<(int)iLen ? BtnIDs[i] : ID_SEPARATOR);
//      //Cmd = PF.RdInt(Section(), Item(), Cmd);
//      //NewIDs[i] = (UINT)Cmd;
//      //Chngd = (Chngd || (NewIDs[i]!=BtnIDs[i]));
//      }
//    if (Chngd)
//      ChangeButtons(NewIDs, Cnt);
//    delete []NewIDs;
//    }
//  // the toolbar has changed
//  return TRUE;
  }

//===========================================================================
