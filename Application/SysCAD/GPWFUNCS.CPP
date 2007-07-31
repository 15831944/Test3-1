//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"
#include "gpwfuncs.h"
#include "helpinfo.h"
#include "datacnvs.h"
#include "scd_wm.h"

//===========================================================================

void GPWFuncs_Entry()
  {
  }

//===========================================================================

IMPLEMENT_DYNCREATE(CCustomPropertyPage, CPropertyPage)

CCustomPropertyPage::CCustomPropertyPage(UINT nIDTemplate, UINT nIDCaption/* = 0*/) : CPropertyPage(nIDTemplate, nIDCaption)
  {
  iDialogID = nIDTemplate;
  bDidInit = FALSE;
  bActive = FALSE;
  pTTC = NULL;
  }

//--------------------------------------------------------------------------

CCustomPropertyPage::~CCustomPropertyPage()
  {
  bDidInit = FALSE;
  if (pTTC)
    {
    delete pTTC;
    pTTC = NULL;
    }
  }

//--------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomPropertyPage, CPropertyPage)
  //{{AFX_MSG_MAP(CCustomPropertyPage)
  //}}AFX_MSG_MAP
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------

BOOL CCustomPropertyPage::OnInitDialog() 
  {
  bDidInit = TRUE;
  CPropertyPage::OnInitDialog();
  if (HelpMngr.ShowToolTips())
    pTTC = new CCustomToolTipCtrl(this);
  return TRUE;
  }

//--------------------------------------------------------------------------

BOOL CCustomPropertyPage::PreTranslateMessage(MSG* pMsg) 
  {
  if (pTTC)
    pTTC->RelayEvent(pMsg);
  return CPropertyPage::PreTranslateMessage(pMsg);
  }

//---------------------------------------------------------------------------

BOOL CCustomPropertyPage::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
  {
  if (pTTC)
    return pTTC->OnToolTipNotify(pNMHDR, iDialogID);
  return FALSE;
  }

//===========================================================================

CCustomToolTipCtrl::CCustomToolTipCtrl(CWnd* pParentWnd)
  {
  ASSERT(pParentWnd);
  pParentWnd->EnableToolTips(TRUE);
  VERIFY(Create(pParentWnd));
  CWnd* w = pParentWnd->GetWindow(GW_CHILD);
  while (w)
    {
    AddTool(w);
    w = w->GetWindow(GW_HWNDNEXT);
    }
  if (GetToolCount()>0)
    Activate(TRUE);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomToolTipCtrl, CToolTipCtrl)
  //{{AFX_MSG_MAP(CCustomToolTipCtrl)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCustomToolTipCtrl::OnToolTipNotify(NMHDR* pNMHDR, UINT ParentID)
  {
  TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
  //UINT nID = pNMHDR->idFrom;
  if (pTTT->uFlags & TTF_IDISHWND)
    {
    // idFrom is actually the HWND of the tool
    CHelpItem* pHI = HelpMngr.GetHelpInfo(ParentID, ::GetDlgCtrlID((HWND)(pNMHDR->idFrom)));
    if (pHI && pHI->HasShortDesc())
      {
      pTTT->lpszText = pHI->ShortDesc();
      return TRUE;
      }
    }
  return FALSE;
  }

//===========================================================================

int StatusBarDriverIndicator = 0;
int StatusBarCmdIndicator = 0;

CCustomStatusBar* pStatusBar = NULL; //global pointer to the application status bar

//---------------------------------------------------------------------------

CCustomStatusBar::CCustomStatusBar()
  {
  iMsgPos = -1;
  bUseLocks = 0;
  iNoOfIndicators = 0;
  Indicators.SetSize(0);
  pStatusBar = this;
  }

//---------------------------------------------------------------------------

CCustomStatusBar::~CCustomStatusBar()
  {
  pStatusBar = NULL;
  if (bUseLocks)
    DeleteCriticalSection(&LockSection);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomStatusBar, CStatusBar)
  //{{AFX_MSG_MAP(CCustomStatusBar)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI_RANGE(ID_INDICATOR_0, ID_INDICATOR_9, OnUpdateInd)
  ON_MESSAGE(WMU_UPDATEINDICATOR, OnUpdateIndicator)
  ON_MESSAGE(WMU_UPDATESTATUSBAR, OnUpdateStatusBar)
  ON_MESSAGE(WMU_DOUPDATESTATUSBAR, OnDoUpdateStatusBar)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CCustomStatusBar::UseLocks()
  {
  if (!bUseLocks)
    {
    InitializeCriticalSection(&LockSection);
    bUseLocks = 1;
    }
  }

//---------------------------------------------------------------------------

void CCustomStatusBar::SetNoOfIndicators(int NoOfIndicators)
  {
  iNoOfIndicators = NoOfIndicators;
  Indicators.SetSize(iNoOfIndicators);
  for (int i=0; i<iNoOfIndicators; i++)
    {
    Indicators[i] = " "; 
    //MFC has memory leak if indicator is "" (empty) when status bar is closed;
    //to ensure this never happens, never allow indicator text to be empty
    //According to the Knowledge base this has been fixed on VC++ 5.0 and with patch to 4.2
    PostMessage(WMU_UPDATEINDICATOR, i, 0);
    }
  }

//---------------------------------------------------------------------------

//if bUpdate is false the indicator pane will only be updated during idle time
void CCustomStatusBar::UpdateIndicator(int index, char* pTxt, BOOL bUpdate/*=TRUE*/)
  {
  ASSERT(index>=0 && index<iNoOfIndicators);
  Lock();
  Indicators[index] = ((pTxt && strlen(pTxt)>0) ? pTxt : " "); //MFC has memory leak if indicator is "" (empty) when status bar is closed; to ensure this never happens, never allow indicator text to be empty
  Unlock();
  //if (AfxGetThread()==MainApplicationThread)
  //  SendMessage(...)
  //else
  //  PostMessage(...)
  if (bUpdate)
    PostMessage(WMU_UPDATEINDICATOR, index, 0);
    //SendMessage(WMU_UPDATEINDICATOR, index, 0);
  }

//---------------------------------------------------------------------------

void CCustomStatusBar::OnUpdateInd(CCmdUI* pCmdUI)
  {
  Lock();
  pCmdUI->Enable(TRUE);
  pCmdUI->SetText((const char*)Indicators[pCmdUI->m_nID - ID_INDICATOR_0]);
  Unlock();
  }

//---------------------------------------------------------------------------

LRESULT CCustomStatusBar::OnUpdateIndicator(WPARAM wParam, LPARAM lParam)
  {
  //Lock();
  if (wParam<(WORD)iNoOfIndicators)
    SetPaneText(wParam+1, (const char*)Indicators[wParam], TRUE);
  //Unlock();
  return TRUE;
  }

//---------------------------------------------------------------------------

void CCustomStatusBar::SetMsg(char* fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  SendMessage(WMU_DOUPDATESTATUSBAR, 1, (LPARAM)Buff);
  }

//---------------------------------------------------------------------------

void CCustomStatusBar::SetMsg1(char* pStatusBarMsg)
  {
  SendMessage(WMU_DOUPDATESTATUSBAR, 1, (LPARAM)pStatusBarMsg);
  }

//---------------------------------------------------------------------------

LRESULT CCustomStatusBar::OnDoUpdateStatusBar(WPARAM wParam, LPARAM lParam)
  {
  if (lParam)
    {
    SetWindowText((char*)lParam);
    if (wParam==1)
      UpdateWindow();
    return TRUE;
    }
  Lock();
  if (iMsgPos<MaxStatusBarMsgs)
    {
    if (iMsgPos>=0)
      SetWindowText((const char*)Msgs[iMsgPos]);
    else
      {
      CString s;
      if (s.LoadString(AFX_IDS_IDLEMESSAGE))
        SetWindowText((const char*)s);
      else
        SetWindowText("");
      }
    Unlock();
    if (wParam==1)
      UpdateWindow();
    }
  else
    Unlock();
  return TRUE;
  }

//---------------------------------------------------------------------------

LRESULT CCustomStatusBar::OnUpdateStatusBar(WPARAM wParam, LPARAM lParam)
  {
  Lock();
  if (wParam & SBM_ADD)
    {
    iMsgPos++;
    ASSERT(iMsgPos<MaxStatusBarMsgs); //maybe max should be increased, or this is called recursively?
    if (iMsgPos<MaxStatusBarMsgs)
      Msgs[iMsgPos] = (char*)lParam;
    }
  else if (wParam & SBM_UPDATE)
    {
    if (iMsgPos<MaxStatusBarMsgs)
      Msgs[iMsgPos] = (char*)lParam;
    }
  else if (wParam & SBM_REMOVE)
    {
    if (iMsgPos>=0)
      iMsgPos--;
    }
  if (wParam & SBM_POSTMSG)
    PostMessage(WMU_DOUPDATESTATUSBAR, 0, 0);
  else
    SendMessage(WMU_DOUPDATESTATUSBAR, 1, 0);
  Unlock();
  return TRUE;
  }

//===========================================================================

CStatusBarMsg::CStatusBarMsg(char* pStatusBarMsg)
  {
  ASSERT(pStatusBarMsg);
  pStatusBar->SendMessage(WMU_UPDATESTATUSBAR, SBM_ADD, (LPARAM)pStatusBarMsg);
  }

//---------------------------------------------------------------------------

CStatusBarMsg::~CStatusBarMsg()
  {
  //pStatusBar->SendMessage(WMU_UPDATESTATUSBAR, SBM_POSTMSG | SBM_REMOVE, 0);
  pStatusBar->SendMessage(WMU_UPDATESTATUSBAR, SBM_REMOVE, 0);
  }

//---------------------------------------------------------------------------

void CStatusBarMsg::UpdateMsg1(char* pStatusBarMsg)
  {
  ASSERT(pStatusBarMsg);
  pStatusBar->SendMessage(WMU_UPDATESTATUSBAR, SBM_UPDATE, (LPARAM)pStatusBarMsg);
  }

//---------------------------------------------------------------------------

void CStatusBarMsg::UpdateMsg(char* fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  pStatusBar->SendMessage(WMU_UPDATESTATUSBAR, SBM_UPDATE, (LPARAM)Buff);
  }

//===========================================================================

CWaitMsgCursor::CWaitMsgCursor(char* pStatusBarMsg)
  : StatusBarMsg(pStatusBarMsg)
  {
  }

//---------------------------------------------------------------------------

CWaitMsgCursor::~CWaitMsgCursor()
  {
  }

//---------------------------------------------------------------------------

void CWaitMsgCursor::UpdateMsg(char* fmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, fmt);
  vsprintf(Buff, fmt, argptr);
  va_end(argptr);
  StatusBarMsg.UpdateMsg1(Buff);
  }

//===========================================================================


/*
Sample Code for loading a bitmap:

In the sample application, a user is able to open a .BMP file that contains a bitmapped image and 
display that image in the client area of the application's window. The user selects the file to be
opened when the application displays the Open dialog box. (For more information about the Open 
dialog box, see Common Dialog Box Library.) 

After the user selects a file and closes the dialog box, the file and path names are stored in 
members of the OPENFILENAME structure. The application uses this data to open the appropriate 
file and retrieve the bitmap header and data. The following code sample can be used to retrieve 
this data. 
  
// Retrieve a handle identifying the file.

hfbm = CreateFile(ofn.lpstrFile, GENERIC_READ, 
      FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES) NULL, 
      OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 
      (HANDLE) NULL); 

// Retrieve the BITMAPFILEHEADER structure.

ReadFile(hfbm, &bmfh, sizeof(BITMAPFILEHEADER), &dwRead, (LPOVERLAPPED)NULL); 

// Retrieve the BITMAPFILEHEADER structure.

ReadFile(hfbm, &bmih, sizeof(BITMAPINFOHEADER), &dwRead, (LPOVERLAPPED)NULL); 

// Allocate memory for the BITMAPINFO structure.

hmem1 = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + 
   ((1<<bmih.biBitCount) * sizeof(RGBQUAD))); 

lpbmi = GlobalLock(hmem1); 

//   Load BITMAPINFOHEADER into the BITMAPINFO 
//   structure. 
 
 lpbmi->bmiHeader.biSize = bmih.biSize; 
 lpbmi->bmiHeader.biWidth = bmih.biWidth; 
 lpbmi->bmiHeader.biHeight = bmih.biHeight; 
 lpbmi->bmiHeader.biPlanes = bmih.biPlanes; 
 lpbmi->bmiHeader.biBitCount = bmih.biBitCount; 
 lpbmi->bmiHeader.biCompression = bmih.biCompression; 
 lpbmi->bmiHeader.biSizeImage = bmih.biSizeImage; 
 lpbmi->bmiHeader.biXPelsPerMeter = bmih.biXPelsPerMeter; 
 lpbmi->bmiHeader.biYPelsPerMeter = bmih.biYPelsPerMeter; 
 lpbmi->bmiHeader.biClrUsed = bmih.biClrUsed; 
 lpbmi->bmiHeader.biClrImportant = bmih.biClrImportant; 
 
//   Retrieve the color table. 
//   1 << bmih.biBitCount == 2 ^ bmih.biBitCount 
 
 ReadFile(hfbm, lpbmi->bmiColors, 
     ((1<<bmih.biBitCount) * sizeof(RGBQUAD)), 
     &dwRead, (LPOVERLAPPED) NULL); 
 
// Allocate memory for the required number of 
// bytes. 
 
 hmem2 = GlobalAlloc(GHND, (bmfh.bfSize - bmfh.bfOffBits)); 
 
 lpvBits = GlobalLock(hmem2); 
 
// Retrieve the bitmap data. 
 
 ReadFile(hfbm, lpvBits, 
     (bmfh.bfSize - bmfh.bfOffBits), 
     &dwRead, (LPOVERLAPPED) NULL); 
 
// Create a bitmap from the data stored in the 
// .BMP file. 
 
 hbm = CreateDIBitmap(hdc, &bmih, 
     CBM_INIT, lpvBits, 
     lpbmi, DIB_RGB_COLORS); 
 
// Unlock the global memory objects and 
// close the .BMP file. 
 
 GlobalUnlock(hmem1); 
 GlobalUnlock(hmem2); 
 CloseHandle(hfbm); 
 
// Set the fDisplayBitmap flag.
 
 if (hbm) 
     fDisplayBitmap = TRUE; 
 else 
     TextOut(hdc, 100, 100, "LoadBitmap Failed", 17); 
 
// Paint the window (and draw the bitmap).
 
 GetClientRect(hwnd, &rect); 
 InvalidateRect(hwnd, &rect, TRUE); 
 UpdateWindow(hwnd); 
 
//Once the bitmap data is retrieved, the bitmapped image can be drawn in the application's client area. The following code sample is used to draw the bitmap. 

case WM_PAINT:  
    BeginPaint(hwnd, &ps); 
        if (fDisplayBitmap) { 
            hdcMem = CreateCompatibleDC(ps.hdc); 
            SelectObject(hdcMem, hbm); 
            GetObject(hbm, sizeof(BITMAP), (LPSTR) &bm); 
            BitBlt(ps.hdc, 0, 0, bm.bmWidth, bm.bmHeight, 
                hdcMem, 0, 0, SRCCOPY); 
            DeleteDC(hdcMem); 
        } 
    EndPaint(hwnd, &ps); 
    break; 
*/


