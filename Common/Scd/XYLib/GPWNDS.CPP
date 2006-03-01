//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1995

#include "stdafx.h"

#define  __GPWNDS_CPP
#include "sc_defs.h"
#include "gpwnds.h"
#include "gdiblk.h"

//===========================================================================

COLORREF CLabelWnd::BkCol = RGB(0, 0, 0);
COLORREF CLabelWnd::TxCol = RGB(255, 255, 255);
CFont* CLabelWnd::pFont = NULL;
int CLabelWnd::CharX = 8;
int CLabelWnd::CharY = 14;

//---------------------------------------------------------------------------

CLabelWnd::CLabelWnd()
  {
  iLineCnt = 1;
  bVisible = False;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CLabelWnd, CWnd)
	//{{AFX_MSG_MAP(CLabelWnd)
	ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CLabelWnd::SetPosition(int x, int y, flag AutoSizeX, flag AutoSizeY)
  {
  RECT MyRect;
  GetClientRect(&MyRect);
  int width = MyRect.right;
  if (AutoSizeX)
    {
    int MaxLen = Txt[0].Length();
    for (int i=1; i<iLineCnt; i++)
      MaxLen = Max(MaxLen, Txt[i].Length());
    width = MaxLen * CharX;
    }
  int height = MyRect.bottom;
  if (AutoSizeY)
    height = iLineCnt * CharY;
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (GetWindowPlacement(&wp))
    {
    wp.rcNormalPosition.left = x;
    wp.rcNormalPosition.right = x + width;
    wp.rcNormalPosition.top = y;
    wp.rcNormalPosition.bottom = y + height;
    if (bVisible)
      wp.showCmd = SW_SHOW;
    else
      wp.showCmd = SW_HIDE;
    wp.length = sizeof(wp);
    SetWindowPlacement(&wp);
    }
  //Invalidate();
  }

//---------------------------------------------------------------------------

void CLabelWnd::SetPosition(int x, int y, int XChars)
  {
  WINDOWPLACEMENT wp;
  wp.length = sizeof(wp);
  if (GetWindowPlacement(&wp))
    {
    wp.rcNormalPosition.left = x;
    wp.rcNormalPosition.right = x + (XChars * CharX);
    wp.rcNormalPosition.top = y;
    wp.rcNormalPosition.bottom = y + (iLineCnt * CharY);
    if (bVisible)
      wp.showCmd = SW_SHOW;
    else
      wp.showCmd = SW_HIDE;
    wp.length = sizeof(wp);
    SetWindowPlacement(&wp);
    }
  //Invalidate();
  }

//---------------------------------------------------------------------------

void CLabelWnd::OnPaint() 
  {
  CPaintDC dc(this); // device context for painting
  CDCResChk ResChk(dc);

  CFont *pOldFont=NULL;

  if (pFont)
    pOldFont=dc.SelectObject(pFont);
  dc.SetBkColor(BkCol);
  dc.SetTextColor(TxCol);
  RECT Rect;
  GetClientRect(&Rect);
  CBrush Brush(BkCol);
  dc.FillRect(&Rect, &Brush);
  Rect.bottom = CharY;
  for (int i=0; i<iLineCnt; i++)
    {
    dc.DrawText(Txt[i](), Txt[i].Length(), &Rect, DT_LEFT);
    Rect.top += CharY;
    Rect.bottom += CharY;
    }
	// Do not call CStatic::OnPaint() for painting messages
  dc.SelectObject(pOldFont);
  }

void CLabelWnd::OnMouseMove(UINT nFlags, CPoint point)
  {
  Hide();
  CWnd::OnMouseMove(nFlags, point);
  }

//===========================================================================
