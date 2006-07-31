//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "resource.h"

#define  __SCDCTRLS_CPP

#include "scdctrls.h"
#include "datacnvs.h"
#include "scd_wm.h"
#include "gdiblk.h"

//===========================================================================

COLORREF gs_CustomColours[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//===========================================================================

//long MemAvailInit=0;

//long MemUsed()
//  {
//  long mnow = GetFreeSpace(0);
//  if (MemAvailInit==0)
//    MemAvailInit=mnow;
//  return MemAvailInit-mnow;
//  }

//===========================================================================
#ifdef kjljljl
void DoSimpleHelp(PTWindowsObject w, pchar ContextStr)
  {
  char s[256];
  strcpy(s, "HELP:");
  strcat(s, ContextStr);
//  w->GetModule()->ExecDialog(new tSCD_Help(w));
//  w->GetModule()->ExecDialog(new tSCD_Help(w));
  MessageBox(w->HWindow, s, "Not yet Implemented", MB_ICONINFORMATION);
  }

//---------------------------------------------------------------------------

void SetInfoCaption(PTWindowsObject w, char * fmt, ...)
  {
  char Cap[256];
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(Cap, fmt, argptr);
  va_end(argptr);

  while (w->Parent!=NULL)
    w=w->Parent;
  w->SetCaption(Cap);

  }
#endif

//---------------------------------------------------------------------------
#ifdef jkhjkhkjhkjh
flag MakeWindowTop(PTWindow w)
  {
  if (GetTopWindow(w->Parent->HWindow) != w->HWindow)
    {
    SetWindowPos(w->HWindow, NULL, 0,0,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
    return 1;
    }
  return 0;
  }
#endif
//---------------------------------------------------------------------------



#define UseCourier     1
#define UseSystem      0
#define UseOEM         0
#define CmdFontHeight  10
#define CmdFontWidth   5

LOGFONT SysCADFontStruct[] = 
  {
    {
    -CmdFontHeight,                  // lfHeight         
    CmdFontWidth,                    // lfWidth          
    0,                               // lfEscapement     
    0,                               // lfOrientation    
    400,                             // lfWeight         
    0,                               // lfItalic         
    0,                               // lfUnderline      
    0,                               // lfStrikeOut      
    ANSI_CHARSET,                    // lfCharSet        
    OUT_DEFAULT_PRECIS,              // lfOutPrecision   
    CLIP_DEFAULT_PRECIS,             // lfClipPrecision  
    DEFAULT_QUALITY,                 // lfQuality        
    FIXED_PITCH | FF_MODERN,         // lfPitchAndFamily 
    "Courier"                        // lfFaceName
    },
    {
    -CmdFontHeight,                  // lfHeight         
    CmdFontWidth,                    // lfWidth          
    0,                               // lfEscapement     
    0,                               // lfOrientation    
    600,                             // lfWeight         
    0,                               // lfItalic         
    0,                               // lfUnderline      
    0,                               // lfStrikeOut      
    ANSI_CHARSET,                    // lfCharSet        
    OUT_DEFAULT_PRECIS,              // lfOutPrecision   
    CLIP_DEFAULT_PRECIS,             // lfClipPrecision  
    DEFAULT_QUALITY,                 // lfQuality        
    FIXED_PITCH | FF_MODERN,         // lfPitchAndFamily 
    "Courier"                        // lfFaceName
    },
    {
    -CmdFontHeight,                  // lfHeight         
    CmdFontWidth,                    // lfWidth          
    0,                               // lfEscapement     
    0,                               // lfOrientation    
    600,                             // lfWeight         
    1,                               // lfItalic         
    0,                               // lfUnderline      
    0,                               // lfStrikeOut      
    ANSI_CHARSET,                    // lfCharSet        
    OUT_DEFAULT_PRECIS,              // lfOutPrecision   
    CLIP_DEFAULT_PRECIS,             // lfClipPrecision  
    DEFAULT_QUALITY,                 // lfQuality        
    FIXED_PITCH | FF_MODERN,         // lfPitchAndFamily 
    "Courier"                        // lfFaceName
    }
  };

static HFONT SysFont[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

HFONT SysCADFont(byte No)
  {
  if (SysFont[No] == NULL)
    {
    #if UseCourier
    LOGFONT lf;
    lf.lfHeight         = -CmdFontHeight;
    lf.lfWidth          = CmdFontWidth;
    lf.lfEscapement     = 0;
    lf.lfOrientation    = 0;
    lf.lfWeight         = 400;
    lf.lfItalic         = 0;
    lf.lfUnderline      = 0;
    lf.lfStrikeOut      = 0;
    lf.lfCharSet        = ANSI_CHARSET;
    lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    lf.lfQuality        = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
    strcpy(lf.lfFaceName, "Courier");
    switch(No)
      {
      case 0:                                                         break;
      case 1: lf.lfWeight = 700;                                      break;
      case 2:                    lf.lfItalic = 1;                     break;
      case 3: lf.lfWeight = 700; lf.lfItalic = 1;                     break;
      case 4:                                     lf.lfUnderline = 1; break;
      case 5: lf.lfWeight = 700;                  lf.lfUnderline = 1; break;
      case 6: lf.lfItalic = 1;                    lf.lfUnderline = 1; break;
      case 7: lf.lfWeight = 700; lf.lfItalic = 1; lf.lfUnderline = 1; break;
      }

    SysFont[No] = CreateFontIndirect(&lf);
    #elif UseSystem
    SysFont[No] = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
    #elif UseOEM
    SysFont[No] = (HFONT)GetStockObject(OEM_FIXED_FONT);
    #endif
    }
  return SysFont[No];
  }

//---------------------------------------------------------------------------

HFONT SetSysCADFont(HDC my_HDC, byte No)
  {
  return (HFONT)SelectObject(my_HDC, SysCADFont(No));
  }

//---------------------------------------------------------------------------

int SysCADFontHeight(HDC my_HDC, pchar p)
  {
  SIZE Sz;
  GetTextExtentPoint(my_HDC, p, strlen(p), &Sz);
  return Sz.cy;
  }

//---------------------------------------------------------------------------

int SysCADFontWidth(HDC my_HDC, pchar p)
  {
  SIZE Sz;
  GetTextExtentPoint(my_HDC, p, strlen(p), &Sz);
  return Sz.cx;
  }

//--------------------------------------------------------------------------

void REMenuOptions(CWnd * pWnd, CString &Txt, eFilterRule Rule, bool ForFind)
  {
  CMenu Menu;
  Menu.CreatePopupMenu();
  bool ForRep=!ForFind;
       
  if (Rule==eFRWildCard)
    {
    Menu.AppendMenu(MF_STRING, 100, "* Match One or More");
    Menu.AppendMenu(MF_STRING, 101, "? Match One");
    }
  else if (ForFind)
    {
    Menu.AppendMenu(MF_STRING, 200, "^ Start of tag");
    Menu.AppendMenu(MF_STRING, 201, "$ End of tag");
    Menu.AppendMenu(MF_STRING, 202, ". Match Any character");
    Menu.AppendMenu(MF_STRING, 203, "* Match preceding zero or more");
    Menu.AppendMenu(MF_STRING, 204, "+ Match preceding one or more");
    Menu.AppendMenu(MF_STRING, 205, "? Match preceding zero or one");
    Menu.AppendMenu(MF_STRING, 206, "\\ Escape Character");
    Menu.AppendMenu(MF_STRING, 207, "[] Any one character in set");
    Menu.AppendMenu(MF_STRING, 208, "[^] Any one character not in set");
    }
  //Menu.AppendMenu(MF_STRING, 999, "Help");

  CPoint curPoint;
  GetCursorPos(&curPoint);

  int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, curPoint.x, curPoint.y, pWnd);
  Menu.DestroyMenu();                                           
  switch (RetCd)
    {
    case 100: Txt+="*"; break;
    case 101: Txt+="?"; break;
    case 200: Txt+="^"; break;
    case 201: Txt+="$"; break;
    case 202: Txt+="."; break;
    case 203: Txt+="*"; break;
    case 204: Txt+="+"; break;
    case 205: Txt+="?"; break;
    case 206: Txt+="\\"; break;
    case 207: Txt+="[]"; break;
    case 208: Txt+="[^]"; break;
    case 999: break;
    }
  }

//===========================================================================

#define MsgName(a) {#a,a},
#define WM_UNKNOWN 0

pchar WM_MsgName[0x410];
static far struct WinMsgName WM_Messages[] =
  {
  MsgName(WM_UNKNOWN)              // 0x0041
  MsgName(WM_COMPACTING)           // 0x0041
  MsgName(WM_WININICHANGE)         // 0x001A
  MsgName(WM_SYSCOLORCHANGE)       // 0x0015
  MsgName(WM_QUERYNEWPALETTE)      // 0x030F
  MsgName(WM_PALETTEISCHANGING)    // 0x0310
  MsgName(WM_PALETTECHANGED)       // 0x0311
  MsgName(WM_FONTCHANGE)           // 0x001D
  MsgName(WM_SPOOLERSTATUS)        // 0x002A
  MsgName(WM_DEVMODECHANGE)        // 0x001B
  MsgName(WM_TIMECHANGE)           // 0x001E
  MsgName(WM_NULL)                 // 0x0000
  MsgName(WM_USER)                 // 0x0400
  MsgName(WM_PENWINFIRST)          // 0x0380
  MsgName(WM_PENWINLAST)           // 0x038F
//  MsgName(WM_COALESCE_FIRST)       // 0x0390
//  MsgName(WM_COALESCE_LAST)        // 0x039F
  MsgName(WM_POWER)                // 0x0048
  MsgName(WM_QUERYENDSESSION)      // 0x0011
  MsgName(WM_ENDSESSION)           // 0x0016
  MsgName(WM_QUIT)                 // 0x0012
//  MsgName(WM_SYSTEMERROR)          // 0x0017
  MsgName(WM_CREATE)               // 0x0001
  MsgName(WM_NCCREATE)             // 0x0081
  MsgName(WM_DESTROY)              // 0x0002
  MsgName(WM_NCDESTROY)            // 0x0082
  MsgName(WM_SHOWWINDOW)           // 0x0018
  MsgName(WM_SETREDRAW)            // 0x000B
  MsgName(WM_ENABLE)               // 0x000A
  MsgName(WM_SETTEXT)              // 0x000C
  MsgName(WM_GETTEXT)              // 0x000D
  MsgName(WM_GETTEXTLENGTH)        // 0x000E
  MsgName(WM_WINDOWPOSCHANGING)    // 0x0046
  MsgName(WM_WINDOWPOSCHANGED)     // 0x0047
  MsgName(WM_MOVE)                 // 0x0003
  MsgName(WM_SIZE)                 // 0x0005
  MsgName(WM_QUERYOPEN)            // 0x0013
  MsgName(WM_CLOSE)                // 0x0010
  MsgName(WM_GETMINMAXINFO)        // 0x0024
  MsgName(WM_PAINT)                // 0x000F
  MsgName(WM_ERASEBKGND)           // 0x0014
  MsgName(WM_ICONERASEBKGND)       // 0x0027
  MsgName(WM_NCPAINT)              // 0x0085
  MsgName(WM_NCCALCSIZE)           // 0x0083
  MsgName(WM_NCHITTEST)            // 0x0084
  MsgName(WM_QUERYDRAGICON)        // 0x0037
  MsgName(WM_DROPFILES)            // 0x0233
  MsgName(WM_ACTIVATE)             // 0x0006
  MsgName(WM_ACTIVATEAPP)          // 0x001C
  MsgName(WM_NCACTIVATE)           // 0x0086
  MsgName(WM_SETFOCUS)             // 0x0007
  MsgName(WM_KILLFOCUS)            // 0x0008
  MsgName(WM_KEYDOWN)              // 0x0100
  MsgName(WM_KEYUP)                // 0x0101
  MsgName(WM_CHAR)                 // 0x0102
  MsgName(WM_DEADCHAR)             // 0x0103
  MsgName(WM_SYSKEYDOWN)           // 0x0104
  MsgName(WM_SYSKEYUP)             // 0x0105
  MsgName(WM_SYSCHAR)              // 0x0106
  MsgName(WM_SYSDEADCHAR)          // 0x0107
//  MakeMse(WM_KEYFIRST)             // 0x0100
//  MakeMse(WM_KEYLAST)              // 0x0108
  MsgName(WM_MOUSEMOVE)            // 0x0200
  MsgName(WM_LBUTTONDOWN)          // 0x0201
  MsgName(WM_LBUTTONUP)            // 0x0202
  MsgName(WM_LBUTTONDBLCLK)        // 0x0203
  MsgName(WM_RBUTTONDOWN)          // 0x0204
  MsgName(WM_RBUTTONUP)            // 0x0205
  MsgName(WM_RBUTTONDBLCLK)        // 0x0206
  MsgName(WM_MBUTTONDOWN)          // 0x0207
  MsgName(WM_MBUTTONUP)            // 0x0208
  MsgName(WM_MBUTTONDBLCLK)        // 0x0209
//  MsgName(WM_MOUSEFIRST)           // 0x0200
//  MsgName(WM_MOUSELAST)            // 0x0209
  MsgName(WM_NCMOUSEMOVE)          // 0x00A0
  MsgName(WM_NCLBUTTONDOWN)        // 0x00A1
  MsgName(WM_NCLBUTTONUP)          // 0x00A2
  MsgName(WM_NCLBUTTONDBLCLK)      // 0x00A3
  MsgName(WM_NCRBUTTONDOWN)        // 0x00A4
  MsgName(WM_NCRBUTTONUP)          // 0x00A5
  MsgName(WM_NCRBUTTONDBLCLK)      // 0x00A6
  MsgName(WM_NCMBUTTONDOWN)        // 0x00A7
  MsgName(WM_NCMBUTTONUP)          // 0x00A8
  MsgName(WM_NCMBUTTONDBLCLK)      // 0x00A9
  MsgName(WM_MOUSEACTIVATE)        // 0x0021
  MsgName(WM_CANCELMODE)           // 0x001F
  MsgName(WM_TIMER)                // 0x0113
  MsgName(WM_INITMENU)             // 0x0116
  MsgName(WM_INITMENUPOPUP)        // 0x0117
  MsgName(WM_MENUSELECT)           // 0x011F
  MsgName(WM_MENUCHAR)             // 0x0120
  MsgName(WM_COMMAND)              // 0x0111
  MsgName(WM_HSCROLL)              // 0x0114
  MsgName(WM_VSCROLL)              // 0x0115
  MsgName(WM_CUT)                  // 0x0300
  MsgName(WM_COPY)                 // 0x0301
  MsgName(WM_PASTE)                // 0x0302
  MsgName(WM_CLEAR)                // 0x0303
  MsgName(WM_UNDO)                 // 0x0304
  MsgName(WM_RENDERFORMAT)         // 0x0305
  MsgName(WM_RENDERALLFORMATS)     // 0x0306
  MsgName(WM_DESTROYCLIPBOARD)     // 0x0307
  MsgName(WM_DRAWCLIPBOARD)        // 0x0308
  MsgName(WM_PAINTCLIPBOARD)       // 0x0309
  MsgName(WM_SIZECLIPBOARD)        // 0x030B
  MsgName(WM_VSCROLLCLIPBOARD)     // 0x030A
  MsgName(WM_HSCROLLCLIPBOARD)     // 0x030E
  MsgName(WM_ASKCBFORMATNAME)      // 0x030C
  MsgName(WM_CHANGECBCHAIN)        // 0x030D
  MsgName(WM_SETCURSOR)            // 0x0020
  MsgName(WM_SYSCOMMAND)           // 0x0112
  MsgName(WM_MDICREATE)            // 0x0220
  MsgName(WM_MDIDESTROY)           // 0x0221
  MsgName(WM_MDIACTIVATE)          // 0x0222
  MsgName(WM_MDIRESTORE)           // 0x0223
  MsgName(WM_MDINEXT)              // 0x0224
  MsgName(WM_MDIMAXIMIZE)          // 0x0225
  MsgName(WM_MDITILE)              // 0x0226
  MsgName(WM_MDICASCADE)           // 0x0227
  MsgName(WM_MDIICONARRANGE)       // 0x0228
  MsgName(WM_MDIGETACTIVE)         // 0x0229
  MsgName(WM_MDISETMENU)           // 0x0230
  MsgName(WM_CHILDACTIVATE)        // 0x0022
  MsgName(WM_INITDIALOG)           // 0x0110
  MsgName(WM_NEXTDLGCTL)           // 0x0028
  MsgName(WM_PARENTNOTIFY)         // 0x0210
  MsgName(WM_ENTERIDLE)            // 0x0121
  MsgName(WM_GETDLGCODE)           // 0x0087
//  MsgName(WM_CTLCOLOR)             // 0x0019
  MsgName(WM_SETFONT)              // 0x0030
  MsgName(WM_GETFONT)              // 0x0031
  MsgName(WM_DRAWITEM)             // 0x002B
  MsgName(WM_MEASUREITEM)          // 0x002C
  MsgName(WM_DELETEITEM)           // 0x002D
  MsgName(WM_COMPAREITEM)          // 0x0039
  MsgName(WM_VKEYTOITEM)           // 0x002E
  MsgName(WM_CHARTOITEM)           // 0x002F
  MsgName(WM_QUEUESYNC)            // 0x0023
  MsgName(WM_COMMNOTIFY)           // 0x0044
  {"???????????",0}
  };

//---------------------------------------------------------------------------

void dbgWM_Msg(pchar Where, UINT message, WPARAM w, LPARAM l, flag DoAt)
  {
//  UINT i = Min(message, (UINT)(sizeof(WM_MsgName)/sizeof(WM_MsgName[0])-1));
//  dbgpln("WM_Msg.%-10.10s:%-25.25s [%3x] %4x %8x ", Where, WM_MsgName[i], i, w, l);
//  if (DoAt)
//    dbgat("WM_Msg.%-10.10s:%-25.25s [%3x] %4x %8x ", Where, WM_MsgName[i], i, w, l);


  UINT i = (UINT)(sizeof(WM_Messages)/sizeof(WM_Messages[0])-1);
  while (WM_Messages[i].MsgNum != message && i > 0) i--;
  
  dbgpln("WM_Msg.%-10.10s:%-25.25s [%3x] %4x %8x ", Where, WM_Messages[i].Name, message, w, l);

  }

//---------------------------------------------------------------------------

pchar EM_MsgName[WM_USER+40];
static far struct WinMsgName EM_Messages[] =
  {
  MsgName(EM_GETSEL)               // (WM_USER+0)
  MsgName(EM_SETSEL)               // (WM_USER+1)
  MsgName(EM_GETRECT)              // (WM_USER+2)
  MsgName(EM_SETRECT)              // (WM_USER+3)
  MsgName(EM_SETRECTNP)            // (WM_USER+4)
  MsgName(EM_LINESCROLL)           // (WM_USER+6)
  MsgName(EM_GETMODIFY)            // (WM_USER+8)
  MsgName(EM_SETMODIFY)            // (WM_USER+9)
  MsgName(EM_GETLINECOUNT)         // (WM_USER+10)
  MsgName(EM_LINEINDEX)            // (WM_USER+11)
  MsgName(EM_SETHANDLE)            // (WM_USER+12)
  MsgName(EM_GETHANDLE)            // (WM_USER+13)
  MsgName(EM_LINELENGTH)           // (WM_USER+17)
  MsgName(EM_REPLACESEL)           // (WM_USER+18)
//  MsgName(EM_SETFONT)              // (WM_USER+19)
  MsgName(EM_GETLINE)              // (WM_USER+20)
  MsgName(EM_LIMITTEXT)            // (WM_USER+21)
  MsgName(EM_CANUNDO)              // (WM_USER+22)
  MsgName(EM_UNDO)                 // (WM_USER+23)
  MsgName(EM_FMTLINES)             // (WM_USER+24)
  MsgName(EM_LINEFROMCHAR)         // (WM_USER+25)
//  MsgName(EM_SETWORDBREAK)         // (WM_USER+26)
  MsgName(EM_SETTABSTOPS)          // (WM_USER+27)
  MsgName(EM_SETPASSWORDCHAR)      // (WM_USER+28)
  MsgName(EM_EMPTYUNDOBUFFER)      // (WM_USER+29)
  MsgName(EM_GETFIRSTVISIBLELINE)  // (WM_USER+30)
  MsgName(EM_SETREADONLY)          // (WM_USER+31)
  MsgName(EM_SETWORDBREAKPROC)     // (WM_USER+32)
  MsgName(EM_GETWORDBREAKPROC)     // (WM_USER+33)
  MsgName(EM_GETPASSWORDCHAR)      // (WM_USER+34)
  {"???????????",0}
  };

//===========================================================================

void ScdCtrls_Entry()
  {
  int i;
  memset(WM_MsgName, 0, sizeof(WM_MsgName));

  for(i =0;(WM_Messages[i].Name[0]!='?');i++)
    {
    ASSERT(WM_Messages[i].MsgNum <= sizeof(WM_MsgName) / sizeof(WM_MsgName[0]));
    WM_MsgName[WM_Messages[i].MsgNum]= WM_Messages[i].Name;
    }
  }
                                 
//===========================================================================

void SetVisibleWindowPos(CWnd* pWnd, int xPos, int yPos, int Width, int Height, bool AsFloating)
  {//ensure window is visible, not off edge of screen because of change in resolution...
  CRect CR;
  AfxGetMainWnd()->GetClientRect(&CR); //get area of MDI framework client window

  if (AsFloating)
    {
    int XExtra = 0;//60;
    int YExtra = 0;//20;
    int CXSize = GetSystemMetrics(SM_CXSIZE);
    int CYSize = GetSystemMetrics(SM_CYSIZE);

    Width=Min(CR.Width()-XExtra, Width);
    Height=Min(CR.Height()-YExtra, Height);

    const int MaxX = CR.right - 1 - XExtra - CXSize;
    const int MaxY = CR.bottom - 1 - YExtra - CYSize;
    if (xPos>=MaxX - CXSize - XExtra)
      xPos = MaxX - CXSize - XExtra;
    if (xPos<-XExtra)
      xPos = 0;
    if (yPos>=MaxY - CYSize - YExtra)
      yPos = MaxY - CYSize - YExtra;
    if (yPos<-YExtra)
      yPos = 0;


    pWnd->SetWindowPos(NULL, xPos, yPos, Width, Height, 
      ((Width>0) ? 0 : SWP_NOSIZE) | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
  else
    {
    if (Width>0)
      {
      CR.right=Width;
      CR.bottom=Height;
      }

    const int MaxX = CR.right - 1;
    const int MaxY = CR.bottom - 1;
    const int XExtra = 60;
    const int YExtra = 20;
    if (xPos>=MaxX)
      xPos = MaxX - GetSystemMetrics(SM_CXSIZE) - XExtra;
    if (xPos<-XExtra)
      xPos = 0;
    if (yPos>=MaxY)
      yPos = MaxY - GetSystemMetrics(SM_CYSIZE) - YExtra;
    if (yPos<-YExtra)
      yPos = 0;
    pWnd->SetWindowPos(NULL, xPos, yPos, CR.Width(), CR.Height(), 
      ((Width>0) ? 0 : SWP_NOSIZE) | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
  }

//===========================================================================

CBitmapFile::CBitmapFile()
  {
  bValidBmp = FALSE;
  hbm = NULL;
  lpbmi = NULL;
  lpvBits = NULL;
  }

//---------------------------------------------------------------------------

CBitmapFile::~CBitmapFile()
  {
  if (lpbmi)
    delete []lpbmi;
  if (lpvBits)
    delete []lpvBits;
  }

//---------------------------------------------------------------------------

void CBitmapFile::Clear()
  {
  bValidBmp = FALSE;
  hbm = NULL;
  if (lpbmi)
    delete []lpbmi;
  lpbmi = NULL;
  if (lpvBits)
    delete []lpvBits;
  lpvBits = NULL;
  }

//---------------------------------------------------------------------------

BOOL CBitmapFile::LoadBitmap(char* pFilename, int MaxBitCount/*=8*/)
  {
  //BitCount
  Clear();
  WIN32_FIND_DATA FindFileData;
  HANDLE FH = FindFirstFile(pFilename, &FindFileData);
  if (FH!=INVALID_HANDLE_VALUE)
    {
    FindClose(FH);
    HANDLE H = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    DWORD dwRead;
    BITMAPFILEHEADER bmfh;
    ReadFile(H, &bmfh, sizeof(BITMAPFILEHEADER), &dwRead, NULL); // Retrieve the BITMAPFILEHEADER structure.
    ReadFile(H, &bmih, sizeof(BITMAPINFOHEADER), &dwRead, NULL); // Retrieve the BITMAPINFOHEADER structure.
    if (bmih.biBitCount>MaxBitCount)
      {
      CloseHandle(H);
      char Buff[512];
      sprintf(Buff, "BitCount %d exceeds maximum allowed of %d\nWill not load bitmap %s", bmih.biBitCount, MaxBitCount, pFilename);
      AfxMessageBox(Buff);
      return bValidBmp;
      }
    // Allocate memory for the BITMAPINFO structure.
    //biBitCount: 4 = 16 color; 8 = 256 color; 16 = 65536 color; 24 = 16777216 color
    DWORD dwSize = ((1<<bmih.biBitCount) * sizeof(RGBQUAD));
    lpbmi = new char[sizeof(BITMAPINFOHEADER) + dwSize];
    BITMAPINFO* p = (BITMAPINFO*)lpbmi;
    //Load BITMAPINFOHEADER into the BITMAPINFO structure.
    p->bmiHeader.biSize = bmih.biSize; 
    p->bmiHeader.biWidth = bmih.biWidth; 
    p->bmiHeader.biHeight = bmih.biHeight; 
    p->bmiHeader.biPlanes = bmih.biPlanes; 
    p->bmiHeader.biBitCount = bmih.biBitCount; 
    p->bmiHeader.biCompression = bmih.biCompression; 
    p->bmiHeader.biSizeImage = bmih.biSizeImage; 
    p->bmiHeader.biXPelsPerMeter = bmih.biXPelsPerMeter; 
    p->bmiHeader.biYPelsPerMeter = bmih.biYPelsPerMeter; 
    p->bmiHeader.biClrUsed = bmih.biClrUsed; 
    p->bmiHeader.biClrImportant = bmih.biClrImportant; 
    //Retrieve the color table.   1 << bmih.biBitCount == 2 ^ bmih.biBitCount 
    ReadFile(H, p->bmiColors, dwSize, &dwRead, NULL);
    //Allocate memory for the required number of bytes. 
    dwSize = bmfh.bfSize - bmfh.bfOffBits;
    lpvBits = new char[dwSize];
    //Retrieve the bitmap data. 
    ReadFile(H, lpvBits, dwSize, &dwRead, NULL);
    CloseHandle(H);
    bValidBmp = TRUE;
    }
  return bValidBmp;
  }

//---------------------------------------------------------------------------
  
BOOL CBitmapFile::Paint(CDC* pDC, int XPos, int YPos)
  {
  CRect Rect(XPos, YPos, 1, 1);
  Paint(pDC, &Rect, FALSE);
  return bValidBmp;
  }

//---------------------------------------------------------------------------

BOOL CBitmapFile::Paint(CDC* pDC, LPCRECT lpRect, BOOL Centre/*=TRUE*/, BOOL Stretch/*=FALSE*/)
  {
  if (bValidBmp)
    {                      
    if (hbm==NULL)
      {
      //Create a bitmap from the data stored in the .BMP file. 
      hbm = CreateDIBitmap(pDC->m_hDC, &bmih, CBM_INIT, (void*)lpvBits, (CONST BITMAPINFO*)lpbmi, DIB_RGB_COLORS);
      if (hbm==NULL)
        bValidBmp = FALSE;
      }
    if (hbm)
      {
      HDC hdcMem = CreateCompatibleDC(pDC->m_hDC); 
      BITMAP bm;
      SelectObject(hdcMem, hbm); 
      GetObject(hbm, sizeof(BITMAP), (LPSTR)&bm);
      if (Stretch)
        {
        int DstW = lpRect->right - lpRect->left;
        int DstH = lpRect->bottom - lpRect->top;
        StretchBlt(pDC->m_hDC, 0, 0, DstW, DstH, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        }
      else
        {
        int XPos = Centre ? Max((int)lpRect->left, (int)(lpRect->left + ((lpRect->right - lpRect->left - bm.bmWidth) / 2))) : lpRect->left;
        int YPos = Centre ? Max((int)lpRect->top, (int)(lpRect->top + ((lpRect->bottom - lpRect->top - bm.bmHeight) / 2))) : lpRect->top;
        BitBlt(pDC->m_hDC, XPos, YPos, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
        }
      DeleteDC(hdcMem);
      }
    }
  return bValidBmp;
  }

//===========================================================================

CTxtWnd::CTxtWnd()
  {
  bEnabled = 1;
  BkCol = RGB(191,191,191);
  EnTxCol = RGB(0, 0, 0);
  DisTxCol = RGB(127, 127, 127);
  Txt = "";
  pFont = NULL;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTxtWnd, CWnd)
  //{{AFX_MSG_MAP(CTxtWnd)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CTxtWnd::OnPaint() 
  {
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  if (pFont)
    dc.SelectObject(pFont);
  dc.SetBkColor(BkCol);
  if (bEnabled)
    dc.SetTextColor(EnTxCol);
  else
    dc.SetTextColor(DisTxCol);
  RECT Rect;
  GetClientRect(&Rect);
  dc.DrawText(Txt(), Txt.Length(), &Rect, DT_LEFT);
  // Do not call CStatic::OnPaint() for painting messages
  }

//===========================================================================

CStringDlg::CStringDlg(char* pTitle, char* pFieldName, char* pPrevData/*=""*/, CWnd* pParent/*=NULL*/)
	: CDialog(IDD_SIMPLE_STRING_DLG, pParent)
  {
  sTitle = pTitle;
  sFieldName = pFieldName;
  //{{AFX_DATA_INIT(CStringDlg)
  m_Data = pPrevData;
  //}}AFX_DATA_INIT
  }

//---------------------------------------------------------------------------

void CStringDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CStringDlg)
  DDX_Text(pDX, IDC_STRINGEDIT, m_Data);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CStringDlg, CDialog)
  //{{AFX_MSG_MAP(CStringDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CStringDlg::OnInitDialog() 
  {
  CDialog::OnInitDialog();

  SetWindowText((const char*)sTitle);
  SetDlgItemText(IDC_TXT_NAME, (const char*)sFieldName);

  return TRUE;
  }

//===========================================================================
//===========================================================================
// Implementation of owned buttons.
// Re-route WM_COMMAND notification to owner instead of parent.
// bLockout flag prevents infinite recursion, because owner will call
// OnChildNotify too.
BOOL COwnedButton::OnChildNotify(UINT msg, WPARAM wp, LPARAM lp, LRESULT* pLRes) 
  {
  static BOOL bLockout = FALSE;
  if (msg==WM_COMMAND && !bLockout) 
    {
    bLockout = TRUE;
    GetOwner()->SendMessage(msg, wp, lp);
    bLockout = FALSE;
    }
  return CButton::OnChildNotify(msg,wp,lp,pLRes);
  }

//---------------------------------------------------------------------------
//===========================================================================

CCustomListBox::CCustomListBox() 
  {
  idListBox = 0;
  idMoveUp = 0;
  idMoveDown = 0;
  idDelete = 0;
  bChanged = 0;
  }

//---------------------------------------------------------------------------

CCustomListBox::~CCustomListBox()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomListBox, CListBox)
  //{{AFX_MSG_MAP(CCustomListBox)
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(1, 0xFFFF, OnCmdRange)
  ON_UPDATE_COMMAND_UI_RANGE(1, 0xFFFF, OnCmdUiRange)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CCustomListBox::SubclassMoveUpDownControls(CWnd* pParent, UINT idListBox_, UINT idMoveUpBtn, UINT idMoveDownBtn)
  {
  ASSERT(idListBox_!=0 && idMoveUpBtn!=0 && idMoveUpBtn!=0); //set valid ID's
  ASSERT(pParent->GetDlgItem(idListBox_));       //was the ID for the list box correct??
  ASSERT(pParent->GetDlgItem(idListBox_)==this); //was the ID for the list box correct??
  ASSERT((GetStyle() & LBS_SORT)==0); //useless with a sorted list box
  ASSERT((GetStyle() & LBS_EXTENDEDSEL)==0); //useless with a multiple selection list box
  ASSERT((GetStyle() & LBS_MULTIPLESEL)==0); //useless with a multiple selection list box
  ASSERT(GetStyle() & LBS_NOTIFY); //notify style MUST be set
  if (idListBox_==0)
    {
    if (!SubclassDlgItem(idListBox_, pParent))
      return FALSE;
    }
  if (!MoveUpBtn.SubclassDlgItem(idMoveUpBtn, pParent))
    return FALSE;
  if (!MoveDownBtn.SubclassDlgItem(idMoveDownBtn, pParent))
    return FALSE;

  idListBox = idListBox_;
  idMoveUp = idMoveUpBtn;
  idMoveDown = idMoveDownBtn;
  // I own the buttons now
  MoveUpBtn.SetOwner(this);
  MoveDownBtn.SetOwner(this);
  UpdateButtons();  // Update (enable/disable) the buttons
  return TRUE;
  }

//---------------------------------------------------------------------------

BOOL CCustomListBox::SubclassDeleteControl(CWnd* pParent, UINT idListBox_, UINT idDeleteBtn)
  {
  ASSERT(idListBox_!=0 && idDeleteBtn!=0); //set valid ID's
  ASSERT(pParent->GetDlgItem(idListBox_));       //was the ID for the list box correct??
  ASSERT(pParent->GetDlgItem(idListBox_)==this); //was the ID for the list box correct??
  ASSERT((GetStyle() & LBS_EXTENDEDSEL)==0); //not implemented for a multiple selection list box
  ASSERT((GetStyle() & LBS_MULTIPLESEL)==0); //not implemented for a multiple selection list box
  ASSERT(GetStyle() & LBS_NOTIFY); //notify style MUST be set
  if (idListBox==0)
    {
    if (!SubclassDlgItem(idListBox_, pParent))
      return FALSE;
    }
  if (!DeleteBtn.SubclassDlgItem(idDeleteBtn, pParent))
    return FALSE;

  idListBox = idListBox_;
  idDelete = idDeleteBtn;
  // I own the buttons now
  DeleteBtn.SetOwner(this);
  UpdateButtons();  // Update (enable/disable) the buttons
  return TRUE;
  }

//---------------------------------------------------------------------------

int CCustomListBox::SetCurSel(int nSelect)
  { 
  int e = CListBox::SetCurSel(nSelect);
  GetParent()->UpdateDialogControls(this, FALSE);
  return e;
  }

//---------------------------------------------------------------------------

BOOL CCustomListBox::OnChildNotify(UINT msg, WPARAM wp, LPARAM lp, LRESULT* pLRes) 
  {
  if (msg==WM_COMMAND) 
    {
    const int nCode = HIWORD(wp);
    if (nCode==LBN_SELCHANGE)// || nCode==LBN_SELCANCEL)
      UpdateButtons();
    }
  return CListBox::OnChildNotify(msg, wp, lp, pLRes);
  }

//---------------------------------------------------------------------------
// Handle command in range 0 to 0xFFFF (all commands).
void CCustomListBox::OnCmdRange(UINT id)
  {
  if (id==idMoveUp)
    {
    int i = GetCurSel();
    if (i>0)
      {
      CString Txt;
      GetText(i, Txt);
      DeleteString(i);
      i = InsertString(i-1, (const char*)Txt);
      CListBox::SetCurSel(i);
      bChanged = 1;
      GetParent()->UpdateDialogControls(this, FALSE);
      }
    }
  else if (id==idMoveDown)
    {
    int i = GetCurSel();
    if (i>=0 && i<GetCount()-1)
      {
      CString Txt;
      GetText(i, Txt);
      DeleteString(i);
      i = InsertString(i+1, (const char*)Txt);
      CListBox::SetCurSel(i);
      bChanged = 1;
      GetParent()->UpdateDialogControls(this, FALSE);
      }
    }
  else if (id==idDelete)
    {
    int i = GetCurSel();
    if (i>=0 && i<GetCount())
      {
      DeleteString(i);
      if (GetCount()>0)
        CListBox::SetCurSel(i<GetCount() ? i : i-1);
      bChanged = 1;
      GetParent()->UpdateDialogControls(this, FALSE);
      }
    }
  }

//---------------------------------------------------------------------------
// Handle command update (0-0xFFFF, all commands).
void CCustomListBox::OnCmdUiRange(CCmdUI* pCmdUI)
  {
  if (pCmdUI->m_nID==idMoveUp)
    pCmdUI->Enable(GetCurSel()>0);
  else if (pCmdUI->m_nID==idMoveDown)
    pCmdUI->Enable(GetCurSel()<GetCount()-1 && GetCurSel()>=0);
  else if (pCmdUI->m_nID==idDelete)
    pCmdUI->Enable(GetCurSel()>=0);
  }

//===========================================================================

CHeaderListBox::CHeaderListBox() 
  {
  Header.SetOwner(this);
  iHeadCount = 0;
  iFontWidth = 0;
  pHDItems = NULL;
  bCreatedHeader=False;
  }

//---------------------------------------------------------------------------

CHeaderListBox::~CHeaderListBox()
  {
  if (pHDItems)
    delete []pHDItems;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CHeaderListBox, CCustomListBox)
  //{{AFX_MSG_MAP(CHeaderListBox)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WMU_SETHEADERTABS, OnSetHeaderTabs)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CHeaderListBox::SetHeaderCount(int Count)
  {
  iHeadCount = Count;
  if (pHDItems)
    delete []pHDItems; //discard previous
  pHDItems = new HD_ITEM[iHeadCount];
  for (int i=0; i<iHeadCount; i++)
    {
    pHDItems[i].mask = HDI_TEXT | HDI_WIDTH;
    pHDItems[i].cxy = 20;
    pHDItems[i].pszText = NULL;
    pHDItems[i].hbm = 0;
    pHDItems[i].cchTextMax = 0;
    pHDItems[i].fmt = HDF_CENTER | HDF_STRING;
    pHDItems[i].lParam = 0;
    }
  Headings.SetSize(iHeadCount);
  }

//---------------------------------------------------------------------------

void CHeaderListBox::SetHeaderItem(int index, char* pTxt, int Width, int fmt)
  {
  ASSERT(index<iHeadCount);
  pHDItems[index].cxy = Width;
  pHDItems[index].fmt = HDF_STRING | fmt;
  Headings[index] = pTxt;
  }

//---------------------------------------------------------------------------

BOOL CHeaderListBox::MakeHeader(UINT nID)
  {
  ASSERT(!bCreatedHeader);
  //create header control...
  RECT Rect;
  DWORD style = HDS_BUTTONS | HDS_HORZ /*| CCS_TOP | CCS_NODIVIDER */| WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;
  if (!Header.Create(style, Rect, GetParent(), nID))
    return FALSE;
  bCreatedHeader = TRUE;
  //move header control directly above list box...
  const int HeaderHeight = 19;
  WINDOWPLACEMENT WPB;
  WPB.length = sizeof(WPB);
  GetWindowPlacement(&WPB);
  WINDOWPLACEMENT WPC;
  WPC.length = sizeof(WPC);
  Header.GetWindowPlacement(&WPC);
  WPC.length = sizeof(WPC);
  WPC.rcNormalPosition.left = WPB.rcNormalPosition.left-1;
  WPC.rcNormalPosition.right = WPB.rcNormalPosition.right+1;
  WPC.rcNormalPosition.top = WPB.rcNormalPosition.top;
  WPC.rcNormalPosition.bottom = WPC.rcNormalPosition.top + HeaderHeight;
  WPB.length = sizeof(WPB);
  WPB.rcNormalPosition.top += (HeaderHeight - 1);
  BOOL b = SetWindowPlacement(&WPB);
  b = Header.SetWindowPlacement(&WPC);
  Header.SetFont(GetFont());
  UpdateHeader();
  return TRUE;
  }

//---------------------------------------------------------------------------

void CHeaderListBox::UpdateHeader()
  {
  ASSERT(bCreatedHeader);
  //delete old columns...
  while (Header.GetItemCount()>0)
    Header.DeleteItem(0);
  //setup columns...
  for (int i=0; i<iHeadCount; i++)
    {
    pHDItems[i].cchTextMax = Headings[i].GetLength();
    pHDItems[i].pszText = (pHDItems[i].cchTextMax>0 ? (char*)(const char*)(Headings[i]) : NULL);
    Header.InsertItem(i, &(pHDItems[i]));
    }
  SetTabs();
  }

//---------------------------------------------------------------------------

void CHeaderListBox::SetTabs()
  {
  if (iFontWidth==0)
    {//determine average width of font used in dialog box...
    CFont* pFont = GetParent()->GetFont();
    CClientDC dc(this);
    CDCResChk ResChk(dc);
    int PrevMapMode = dc.SetMapMode(MM_TEXT);
    CFont* OldFont= dc.SelectObject(pFont);
    TEXTMETRIC Metrics;
    if (dc.GetTextMetrics(&Metrics))
      iFontWidth = Metrics.tmAveCharWidth + 1;
    dc.SetMapMode(PrevMapMode);
    dc.SelectObject(OldFont);
    if (iFontWidth==0)
      {
      LONG BaseUnits = GetDialogBaseUnits();
      iFontWidth = LOWORD(BaseUnits);
      }
    }
  if (iHeadCount>1)
    {
    LPINT pTabs = new INT[iHeadCount-1];
    int TabPos = pHDItems[0].cxy;
    for (int i=1; i<iHeadCount; i++)
      {
      pTabs[i-1] = TabPos;
      TabPos += pHDItems[i].cxy;
      }
    for (i=0; i<iHeadCount-1; i++)
      pTabs[i] = (pTabs[i] * 4) / iFontWidth; //see docs on GetDialogBaseUnits and MapDialogRect
    VERIFY(SetTabStops(iHeadCount-1, pTabs)); //did you set the LBS_USETABSTOPS style ???
    delete []pTabs;
    Invalidate();
    }
  }

//---------------------------------------------------------------------------

void CHeaderListBox::GetTextInColumn(int nColumn, int nIndex, CString& rString)
  {
  rString = "";
  if (nColumn<0 || nColumn>=iHeadCount || nIndex<0 || nIndex>=GetCount())
    return;
  int Pos;
  CString s;
  GetText(nIndex, s);
  for (int i=0; i<nColumn; i++)
    {
    Pos = s.Find('\t');
    if (Pos==-1)
      return;
    s = s.Mid(Pos+1, 16384);
    }
  Pos = s.Find('\t');
  if (Pos>=0)
    s = s.Left(Pos);
  rString = s;
  }

//---------------------------------------------------------------------------

int CHeaderListBox::SetTextInColumn(int nColumn, int nIndex, LPCTSTR lpszItem)
  {
  if (nColumn<0 || nColumn>=iHeadCount || nIndex<0 || nIndex>=GetCount())
    return LB_ERR;
  BOOL WasSelected = (GetSel(nIndex)>0);
  int Pos;
  CString s;
  CString NewItem;
  GetText(nIndex, s);
  for (int i=0; i<nColumn; i++)
    {
    Pos = s.Find('\t');
    if (Pos>=0)
      {
      NewItem += s.Left(Pos+1);
      s = s.Mid(Pos+1, 16384);
      }
    else
      NewItem += '\t';
    }
  if (lpszItem)
    NewItem += lpszItem;
  Pos = s.Find('\t');
  if (Pos>=0)
    NewItem += s.Mid(Pos, 16384);
  DeleteString(nIndex);
  i = AddString((const char*)NewItem);
  if (i<0)
    return i;
  if (WasSelected)
    SetSel(i);
  return i;
  }

//---------------------------------------------------------------------------

int CHeaderListBox::FindStringExactInColumn(int nColumn, int nIndexStart, LPCTSTR lpszFind)
  {
  if (nColumn<0 || nColumn>=iHeadCount || lpszFind==NULL)
    return LB_ERR;
  const int len = strlen(lpszFind);
  int Start = (nIndexStart<0 ? 0 : nIndexStart);
  for (int i=Start; i<GetCount(); i++)
    {
    CString Txt;
    GetTextInColumn(nColumn, i, Txt);
    if (Txt.GetLength()==len && _stricmp((const char*)Txt, lpszFind)==0)
      return i;
    }
  return LB_ERR;
  }

//---------------------------------------------------------------------------

LRESULT CHeaderListBox::OnSetHeaderTabs(WPARAM wParam, LPARAM lParam)
  {
  if (iHeadCount>1)
    {
    for (int i=0; i<iHeadCount; i++)
      {
      pHDItems[i].mask = HDI_WIDTH;
      Header.GetItem(i, &(pHDItems[i]));
      }
    SetTabs();
    }
  return TRUE;
  }

//===========================================================================

BEGIN_MESSAGE_MAP(CListBoxHeader, CHeaderCtrl)
  //{{AFX_MSG_MAP(CListBoxHeader)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

BOOL CListBoxHeader::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
  {
  if (message==WM_NOTIFY)
    {
    NMHDR* pH = (NMHDR*)lParam;
    if (pH->code==HDN_ENDTRACK)
      {
      pOwner->PostMessage(WMU_SETHEADERTABS, wParam, lParam);
      }
    }
  return CHeaderCtrl::OnChildNotify(message, wParam, lParam, pLResult);
  }

//===========================================================================

BEGIN_MESSAGE_MAP(CComboCombo, CComboBox)
  //{{AFX_MSG_MAP(CComboCombo)
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(1, 0xFFFF, OnCmdRange)
  ON_UPDATE_COMMAND_UI_RANGE(1, 0xFFFF, OnCmdUiRange)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CComboCombo::CComboCombo()
  {
  m_idCombo = 0;
  m_idAdd = 0;
  m_idDel = 0;
  m_idPrev = 0;
  m_idNext = 0;
  }

//---------------------------------------------------------------------------
// Subclass a the combo box and Add/Delete buttons. 
// The Dialog must call this to tell the combo box what the control IDs 
// are and to hook everything up.
BOOL CComboCombo::SubclassAddDelControls(CWnd* pParent, UINT idCombo, UINT idAdd, UINT idDel)
  {
  ASSERT(idCombo!=0 && idAdd!=0 && idDel!=0);
  ASSERT(GetStyle() & LBS_NOTIFY); //notify style MUST be set
  if (m_idCombo==0)
    {
    if (!SubclassDlgItem(idCombo, pParent))
      return FALSE;
    }
  if (!m_buttonAdd.SubclassDlgItem(idAdd, pParent))
    return FALSE;
  if (!m_buttonDel.SubclassDlgItem(idDel, pParent))
    return FALSE;

  m_idCombo = idCombo;
  m_idAdd = idAdd;
  m_idDel = idDel;
  // I own the buttons now
  m_buttonAdd.SetOwner(this);
  m_buttonDel.SetOwner(this);
  UpdateButtons();  // Update (enable/disable) the buttons
  return TRUE;
  }

//---------------------------------------------------------------------------
// Subclass a the combo box and Prev/Next buttons. 
// The Dialog must call this to tell the combo box what the control IDs 
// are and to hook everything up.
BOOL CComboCombo::SubclassPrevNextControls(CWnd* pParent, UINT idCombo, UINT idPrev, UINT idNext)
  {
  ASSERT(idCombo!=0 && idPrev!=0 && idNext!=0);
  ASSERT(GetStyle() & LBS_NOTIFY); //notify style MUST be set
  if (m_idCombo==0)
    {
    if (!SubclassDlgItem(idCombo, pParent))
      return FALSE;
    }
  if (!m_buttonPrev.SubclassDlgItem(idPrev, pParent))
    return FALSE;
  if (!m_buttonNext.SubclassDlgItem(idNext, pParent))
    return FALSE;

  m_idCombo = idCombo;
  m_idPrev = idPrev;
  m_idNext = idNext;
  // I own the buttons now
  m_buttonPrev.SetOwner(this);
  m_buttonNext.SetOwner(this);
  UpdateButtons();  // Update (enable/disable) the buttons
  return TRUE;
  }

//---------------------------------------------------------------------------
// When the edit text or selection changes, 
// button states may change, so I need to update them.
BOOL CComboCombo::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
  {
  if (message==WM_COMMAND) 
    {
    int nCode = HIWORD(wParam);
    if (nCode==CBN_SELCHANGE || nCode==CBN_EDITCHANGE)
      UpdateButtons();
    }
  return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
  }

//---------------------------------------------------------------------------
// Helper function updates Add/Delete buttons
void CComboCombo::UpdateButtons()
  {
  // Update buttons, using myself as target!
  GetParent()->UpdateDialogControls(this, FALSE);
  }

//---------------------------------------------------------------------------
// Handle command in range 0 to 0xFFFF (all commands). Check for Add/Delete.
void CComboCombo::OnCmdRange(UINT id)
  {
  if (id==m_idAdd) 
    {
    // Add command: add contents of edit control to list box.
    // Beep if item already added.
    CString s;
    GetWindowText(s);
    if (FindStringExact(0, s) >= 0)
      MessageBeep(0);
    else 
      {
      AddString(s);
      SetEditSel(0, -1);
      Clear();
      SetFocus();
      }
    UpdateButtons();
    } 
  else if (id==m_idDel) 
    {// Delete command: Delete selected item.
    DeleteString(GetCurSel());
    UpdateButtons();
    }
  else if (id==m_idPrev)// && GetCurSel()>0)
    {
    SetCurSel(GetCurSel()-1);
    //I don't know what the message should be, so use our own user message
    //GetParent()->PostMessage(WM_COMMAND, MAKELONG((WORD)1, (WORD)CBN_SELCHANGE), (LPARAM)m_hWnd);
    GetParent()->PostMessage(WMU_COMBOBOXMSG, (WPARAM)id, (LPARAM)MAKELONG((WORD)m_idCombo,(WORD)CBN_SELCHANGE));
    UpdateButtons();
    }
  else if (id==m_idNext)// && GetCurSel()<GetCount()-1)
    {
    SetCurSel(GetCurSel()+1);
    //I don't know what the message should be, so use our own user message
    //GetParent()->PostMessage(WM_COMMAND, MAKELONG((WORD)1, (WORD)CBN_SELCHANGE), (LPARAM)m_hWnd);
    GetParent()->PostMessage(WMU_COMBOBOXMSG, (WPARAM)id, (LPARAM)MAKELONG((WORD)m_idCombo,(WORD)CBN_SELCHANGE));
    UpdateButtons();
    }
  }

//---------------------------------------------------------------------------
// Handle command update (0-0xFFFF, all commands). Check for Add/Delete.
void CComboCombo::OnCmdUiRange(CCmdUI* pCmdUI)
  {
  const UINT id = pCmdUI->m_nID;
  if (id==m_idAdd)
    pCmdUI->Enable(GetWindowTextLength()>0); // Add button is enabled iff there's text.
  else if (id==m_idDel)
    pCmdUI->Enable(GetCurSel()>=0); // Delete is enable iff an item is selected.
  else if (id==m_idNext)
    pCmdUI->Enable(GetCurSel()<GetCount()-1);
  else if (id==m_idPrev)
    pCmdUI->Enable(GetCurSel()>0);
  }

//===========================================================================

CCustomTreeCtrl::~CCustomTreeCtrl()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomTreeCtrl, CTreeCtrl)
  //{{AFX_MSG_MAP(CCustomTreeCtrl)
  ON_WM_RBUTTONDOWN()
  ON_WM_LBUTTONDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CCustomTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  PrevDownPoint = point;
  CTreeCtrl::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CCustomTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  PrevDownPoint = point;
  CTreeCtrl::OnLButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

HTREEITEM CCustomTreeCtrl::FindChildItem(HTREEITEM H, char* pTxt)
  {
  if (pTxt==NULL)
    return NULL;
  HTREEITEM HH = GetChildItem(H);
  if (HH==NULL)
    return NULL;
  while (HH)
    {
    CString s = GetItemText(HH);
    if (_stricmp((const char*)s, pTxt)==0)
      return HH;
    if (GetChildItem(HH))
      {
      HTREEITEM HHH = FindChildItem(HH, pTxt);
      if (HHH)
        return HHH;
      }
    HH = GetNextSiblingItem(HH);
    }
  return NULL;
  }

//---------------------------------------------------------------------------

HTREEITEM CCustomTreeCtrl::FindItem(char* pTxt, byte RqdLevel/*=0*/)
  {
  if (pTxt==NULL)
    return NULL;
  HTREEITEM H = GetNextItem(NULL, TVGN_CHILD);
  if (H==NULL)
    return NULL;
  return FindItem(H, 0, pTxt, RqdLevel); //call recursive search function
  }

//---------------------------------------------------------------------------

HTREEITEM CCustomTreeCtrl::FindItem(HTREEITEM H, byte Level, char* pTxt, byte RqdLevel)
  {
  while (H)
    {
    if (Level<RqdLevel)
      {
      HTREEITEM HH = GetChildItem(H);
      if (HH)
        {
        HTREEITEM HHH = FindItem(HH, Level+1, pTxt, RqdLevel);
        if (HHH)
          return HHH;
        }
      }
    else
      {
      CString s = GetItemText(H);
      if (_stricmp((const char*)s, pTxt)==0)
        return H;
      }
    H = GetNextSiblingItem(H);
    }
  return NULL;
  }

//===========================================================================

const int Img_SelAllOff = 0;
const int Img_SelAllOn  = 1;
const int Img_SelSomeOn = 2;
const int Img_Lock      = 3;

CSelectTreeCtrl::~CSelectTreeCtrl()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSelectTreeCtrl, CCustomTreeCtrl)
  //{{AFX_MSG_MAP(CSelectTreeCtrl)
  ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
  ON_WM_CHAR()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CSelectTreeCtrl::LoadImage()
  {
  CBitmap BM;
  VERIFY(BM.LoadBitmap(IDB_TREESELIMGS));
  VERIFY(SelImgList.Create(12, 12, FALSE, 0, 4));
  VERIFY(SelImgList.Add(&BM, (CBitmap*)NULL)>=0);
  SetImageList(&SelImgList, TVSIL_NORMAL);
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::DoCnt(HTREEITEM hStartItem, int Image, int& Cnt, int& TtlCnt)
  {//count the number of end items and number of end items with specified image
  int nImage, nSelectedImage;
  while (hStartItem)
    {
    const BOOL b = ItemHasChildren(hStartItem);
    if (!b)
      {
      TtlCnt++;
      if (GetItemImage(hStartItem, nImage, nSelectedImage) && nImage==Image)
        Cnt++;
      }
    else
      DoCnt(GetChildItem(hStartItem), Image, Cnt, TtlCnt);
    hStartItem = GetNextSiblingItem(hStartItem);
    }
  }

//---------------------------------------------------------------------------

BOOL CSelectTreeCtrl::DoGetItem(HTREEITEM hItem, int Image, BOOL MustBeEndItem)
  {//is the specified item image set and (optionally) is it an end item (has no children)
  if (hItem)
    {
    int nImage, nSelectedImage;
    if ((!MustBeEndItem || !ItemHasChildren(hItem)) && GetItemImage(hItem, nImage, nSelectedImage) && nImage==Image)
      return TRUE;
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::DoSetItem(HTREEITEM hItem, int Image, BOOL SetChildren)
  {
  if (hItem)
    {
    SetItemImage(hItem, Image, Image);
    if (SetChildren)
      DoSetAllChildrenImage(hItem, Image);
    }
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::DoSetAllChildrenImage(HTREEITEM h, int Image)
  {
  HTREEITEM hh = GetChildItem(h);
  while (hh)
    {
    SetItemImage(hh, Image, Image);
    if (ItemHasChildren(hh))
      DoSetAllChildrenImage(hh, Image);
    hh = GetNextSiblingItem(hh);
    }
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::DoFixParentImages(HTREEITEM hStartItem, int Image, int PartImage, int NoneImage)
  {
  while (hStartItem)
    {
    if (ItemHasChildren(hStartItem))
      {
      DoFixParentImages(GetChildItem(hStartItem), Image, PartImage, NoneImage);
      int Cnt=0;
      int TtlCnt=0;
      DoCnt(GetChildItem(hStartItem), Image, Cnt, TtlCnt);
      if (Cnt==TtlCnt)
        SetItemImage(hStartItem, Image, Image);
      else if (Cnt>0)
        SetItemImage(hStartItem, PartImage, PartImage);
      else if (Cnt==0)
        SetItemImage(hStartItem, NoneImage, NoneImage);
      }
    hStartItem = GetNextSiblingItem(hStartItem);
    }
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::SetAllChildrenSelectedImage(HTREEITEM h, int Selected)
  {
  HTREEITEM hh = GetChildItem(h);
  while (hh)
    {
    int nImage, nSelectedImage;
    if (GetItemImage(hh, nImage, nSelectedImage) && nImage!=Img_Lock)
      SetItemImage(hh, Selected, Selected);
    if (ItemHasChildren(hh))
      SetAllChildrenSelectedImage(hh, Selected);
    hh = GetNextSiblingItem(hh);
    }
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::ToggleItem(HTREEITEM hItem)
  {
  BOOL DidToggle = FALSE;
  if (hItem)
    {
    int nImage, nSelectedImage;
    if (GetItemImage(hItem, nImage, nSelectedImage) && nImage!=Img_Lock)
      {
      DidToggle = TRUE;
      const int Selected = (nImage==Img_SelAllOn) ? Img_SelAllOff : Img_SelAllOn;
      SetItemImage(hItem, Selected, Selected);
      if (ItemHasChildren(hItem))
        SetAllChildrenSelectedImage(hItem, Selected);
      /*HTREEITEM hh = GetParentItem(hItem);
      while (hh)
        {
        xxx SetItemImage(hh, Selected, Selected);
        hh = GetParentItem(hh);
        }*/
      }
    }
  if (DidToggle)
    {
    FixAllSelectedImages();

    //Notify parent...
    CWnd* pParent = GetParent();
    ASSERT_VALID(pParent);
    pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), STVN_SELECTEDCHANGED), (LPARAM)m_hWnd);
    }
  }

//---------------------------------------------------------------------------

BOOL CSelectTreeCtrl::IsAnItemSelected(HTREEITEM hStartItem)
  {//is at least one end item (has no children) from the specified item selected
  while (hStartItem)
    {
    int nImage, nSelectedImage;
    const BOOL b = ItemHasChildren(hStartItem);
    if (!b && GetItemImage(hStartItem, nImage, nSelectedImage) && nImage==Img_SelAllOn)
      return TRUE;
    if (b)
      {
      if (IsAnItemSelected(GetChildItem(hStartItem)))
        return TRUE;
      }
    hStartItem = GetNextSiblingItem(hStartItem);
    }
  return FALSE;
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::SelectAll(BOOL On)
  {
  const int Selected = (On!=0 ? Img_SelAllOn : Img_SelAllOff);
  HTREEITEM h = GetRootItem();
  while (h)
    {
    int nImage, nSelectedImage;
    if (GetItemImage(h, nImage, nSelectedImage) && nImage!=Img_Lock)
      {
      SetItemImage(h, Selected, Selected);
      if (ItemHasChildren(h))
        SetAllChildrenSelectedImage(h, Selected);
      }
    h = GetNextSiblingItem(h);
    }
  //Assume something changed, notify parent...
  CWnd* pParent = GetParent();
  ASSERT_VALID(pParent);
  pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), STVN_SELECTEDCHANGED), (LPARAM)m_hWnd);
  }

//---------------------------------------------------------------------------

int CSelectTreeCtrl::CntSelected(HTREEITEM hStartItem)
  {//count the number of end items selected
  int Cnt=0;
  int TtlCnt=0;
  DoCnt(hStartItem, Img_SelAllOn, Cnt, TtlCnt);
  return Cnt;
  }

//---------------------------------------------------------------------------

BOOL CSelectTreeCtrl::GetItemSelected(HTREEITEM hItem, BOOL MustBeEndItem/*=TRUE*/)
  {
  return DoGetItem(hItem, Img_SelAllOn, MustBeEndItem) || DoGetItem(hItem, Img_SelSomeOn, MustBeEndItem);
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::SetItemSelected(HTREEITEM hItem, flag On)
  {
  if (hItem)
    {
    int nImage, nSelectedImage;
    if (GetItemImage(hItem, nImage, nSelectedImage) && nImage!=Img_Lock)
      {
      const int Selected = (On!=0 ? Img_SelAllOn : Img_SelAllOff);
      SetItemImage(hItem, Selected, Selected);
      SetAllChildrenSelectedImage(hItem, Selected);
      }
    }
  }

//---------------------------------------------------------------------------

int CSelectTreeCtrl::CntLocked(HTREEITEM hStartItem)
  {//count the number of end items locked
  int Cnt=0;
  int TtlCnt=0;
  DoCnt(hStartItem, Img_Lock, Cnt, TtlCnt);
  return Cnt;
  }

//---------------------------------------------------------------------------

BOOL CSelectTreeCtrl::GetItemLocked(HTREEITEM hItem, BOOL MustBeEndItem/*=TRUE*/)
  {
  return DoGetItem(hItem, Img_Lock, MustBeEndItem);
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::SetItemLocked(HTREEITEM hItem)
  {
  DoSetItem(hItem, Img_Lock, false);
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::FixAllLockImages()
  {
  DoFixParentImages(GetRootItem(), Img_Lock, Img_SelAllOff, Img_SelAllOff);
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::FixAllSelectedImages()
  {
  DoFixParentImages(GetRootItem(), Img_SelAllOn, Img_SelSomeOn, Img_SelAllOff);
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  TV_HITTESTINFO HTI;
  HTI.pt = PrevDownPoint;
  if (HitTest(&HTI) && ((HTI.flags & TVHT_ONITEM) || (HTI.flags & TVHT_ONITEMRIGHT) || (HTI.flags & TVHT_ONITEMICON)))
    {
    ToggleItem(HTI.hItem);
    }
  *pResult = 0;
  }

//---------------------------------------------------------------------------

void CSelectTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
  {
  if (nChar==32 && GetSelectedItem())
    {
    ToggleItem(GetSelectedItem());
    }
  else
    CCustomTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
  }

//===========================================================================

CCustomListCtrl::~CCustomListCtrl()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomListCtrl, CListCtrl)
  //{{AFX_MSG_MAP(CCustomListCtrl)
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CCustomListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
  {
  PrevDownPoint = point;
  CListCtrl::OnLButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void CCustomListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
  {
  PrevDownPoint = point;
  CListCtrl::OnRButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

int CCustomListCtrl::FindString(int nIndexStart, LPCTSTR lpszFind)
  {//emulate FindString function in ListBox/ComboBox
  LVFINDINFO FI;
  FI.flags = LVFI_PARTIAL;
  FI.psz = lpszFind;
  return FindItem(&FI, nIndexStart);
  }

//---------------------------------------------------------------------------

int CCustomListCtrl::FindStringExact(int nIndexStart, LPCTSTR lpszFind)
  {//emulate FindStringExact function in ListBox/ComboBox
  LVFINDINFO FI;
  FI.flags = LVFI_STRING;
  FI.psz = lpszFind;
  return FindItem(&FI, nIndexStart);
  }

//===========================================================================
//
//
//
//===========================================================================
