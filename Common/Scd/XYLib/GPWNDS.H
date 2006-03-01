//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1995

#ifndef  __GPWNDS_H
#define  __GPWNDS_H

#ifdef __GPWNDS_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

const byte MaxLabelLines = 5;

class DllImportExport CLabelWnd : public CWnd
  {
  protected:
    static CFont*     pFont;
    static int        CharX;
    static int        CharY;
    static COLORREF   BkCol;
    static COLORREF   TxCol;
    Strng             Txt[MaxLabelLines];
    byte              iLineCnt;
    flag              bVisible;

  public:
	  CLabelWnd();
	  //{{AFX_VIRTUAL(CLabelWnd)
	  //}}AFX_VIRTUAL
	  virtual ~CLabelWnd() {};
    void SetPosition(int x, int y, flag AutoSizeX = False, flag AutoSizeY = False);
    void SetPosition(int x, int y, int XChars);
    //void ShowText(flag AutoSizeX = False, flag AutoSizeY = False);
    inline void CreateIt(CWnd* pParentWnd) { RECT Rect; Create(NULL, "", WS_CHILD | WS_CLIPSIBLINGS, Rect, pParentWnd, (UINT)-1); };
    inline void SetText(byte LineNo, pchar pTxt) { ASSERT(LineNo>=0 && LineNo<iLineCnt); Txt[LineNo] = pTxt; };
    inline void SetCols(COLORREF NewBkCol, COLORREF NewTxtCol) { BkCol = NewBkCol; TxCol = NewTxtCol; };
    inline void SetFont(CFont* Font) { pFont = Font; };
    inline void SetLineCnt(byte LineCnt) { ASSERT(LineCnt<=MaxLabelLines); iLineCnt = LineCnt; };
    inline void Show() { if (!bVisible) { ShowWindow(SW_SHOW); bVisible = True; } };
    inline void Hide() { if (bVisible) { ShowWindow(SW_HIDE); bVisible = False; } };
  protected:
	  //{{AFX_MSG(CLabelWnd)
	  afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };

//===========================================================================

#undef DllImportExport

#endif
