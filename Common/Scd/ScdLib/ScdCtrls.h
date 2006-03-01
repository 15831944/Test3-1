//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SCDCTRLS_H
#define  __SCDCTRLS_H


#ifdef __SCDCTRLS_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

typedef word           Cursor_Types;

//extern DllImportExport long MemAvailInit;
//DllImportExport long MemUsed();

extern DllImportExport LOGFONT SysCADFontStruct[];
DllImportExport HFONT SysCADFont(byte No);
DllImportExport HFONT SetSysCADFont(HDC my_HDC, byte No);
DllImportExport int SysCADFontHeight(HDC my_HDC, pchar p);
DllImportExport int SysCADFontWidth(HDC my_HDC, pchar p);

enum  eFilterRule  { eFROff, eFRContains, eFRWildCard, eFRRegExp, eFRCount };
const LPCTSTR eFilterRuleNames[eFRCount] = { "Off", "Contains","Wildcards","Regular Expressions"};
DllImportExport void REMenuOptions(CWnd * pWnd, CString &Txt, eFilterRule Rule, bool ForFind);

typedef struct WinMsgName {char Name[25]; word MsgNum;} WinMsgName;

DllImportExport void dbgWM_Msg(pchar Where, UINT message, WPARAM w, LPARAM l, flag DoAt);

//===========================================================================

DllImportExport void ScdCtrls_Entry();

DllImportExport void SetVisibleWindowPos(CWnd* pWnd, int xPos, int yPos, int Width=-1, int Height=-1, bool AsFloating=false);

//===========================================================================

class DllImportExport CBitmapFile
  {
  public:
    CBitmapFile();
    ~CBitmapFile();
    void Clear();
    BOOL LoadBitmap(char* pFilename, int MaxBitCount=8);
    BOOL Paint(CDC* pDC, int XPos, int YPos);
    BOOL Paint(CDC* pDC, LPCRECT lpRect, BOOL Centre=TRUE, BOOL Stretch=FALSE);
    HBITMAP hBitmap() { return hbm; };
    BITMAPINFOHEADER & HDR() { return bmih; };

  protected:
    BOOL              bValidBmp; //has a bitmap been loaded
    BITMAPINFOHEADER  bmih;      //BITMAPINFOHEADER struct
    char*             lpbmi;     //points to variable length BITMAPINFO
    char*             lpvBits;   //points to data for bitmap bits
    HBITMAP           hbm;       //bitmap handle (for dc)
  };

//===========================================================================

class DllImportExport CTxtWnd : public CWnd
  {
  public:
    CTxtWnd();
    //{{AFX_VIRTUAL(CTxtWnd)
    //}}AFX_VIRTUAL
    virtual ~CTxtWnd() {};
    void Enable(flag Enable) { bEnabled = Enable; Invalidate(); };
    void SetBkCol(COLORREF Col) { BkCol = Col; };
    void SetEnabledCol(COLORREF Col) { EnTxCol = Col; };
    void SetDisabledCol(COLORREF Col) { DisTxCol = Col; };
    void SetText(char* p) { Txt = p; };
    void SetFont(CFont* Font) { pFont = Font; };
  protected:
    CFont*      pFont;
    byte        bEnabled:1;
    COLORREF    BkCol;
    COLORREF    EnTxCol;
    COLORREF    DisTxCol;
    Strng       Txt;
    //{{AFX_MSG(CTxtWnd)
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
//simple dialog used to enter a single string...
class DllImportExport CStringDlg : public CDialog
  {
  public:
    CStringDlg(char* pTitle, char* pFieldName, char* pPrevData = "", CWnd* pParent = NULL);
    char* Data() { return (char*)(const char*)m_Data; };

    CString sTitle;
    CString sFieldName;

    //{{AFX_DATA(CStringDlg)
    //enum { IDD = IDD_SIMPLE_STRING_DLG };
    CString	m_Data;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CStringDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CStringDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
// "Owned" button: sends notifications to owner instead of parent.
class DllImportExport COwnedButton : public CButton 
  {
  public:
    virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
  };

//===========================================================================

class DllImportExport CCustomListBox : public CListBox
  {
  public:
    CCustomListBox();
    BOOL SubclassMoveUpDownControls(CWnd* pParent, UINT idListBox_, UINT idMoveUpBtn, UINT idMoveDownBtn);
    BOOL SubclassDeleteControl(CWnd* pParent, UINT idListBox_, UINT idDeleteBtn);
    int SetCurSel(int nSelect); //override
    inline void UpdateButtons() { GetParent()->UpdateDialogControls(this, FALSE); }; // Update buttons, using myself as target!
    inline BOOL Changed() { return bChanged; }; //has the list box been changed by any of the sub-classed buttons
    inline void SetChanged(BOOL Changed) { bChanged = Changed; };

  public:
    //{{AFX_VIRTUAL(CCustomListBox)
    //}}AFX_VIRTUAL
  public:
    virtual ~CCustomListBox();
  protected:
    UCHAR bChanged:1;          //has the list changed, because of subclassed buttons
    COwnedButton MoveUpBtn;    //Move selected item up button
    COwnedButton MoveDownBtn;  //Move selected item down button
    COwnedButton DeleteBtn;    //Remove selected item button
    UINT idListBox;            //ID of ListBox
    UINT idMoveUp;             //ID of MoveUp button
    UINT idMoveDown;           //ID of MoveDown button
    UINT idDelete;             //ID of Delete button

  protected:
    virtual BOOL OnChildNotify(UINT msg, WPARAM wp, LPARAM lp, LRESULT* pLRes);
    //{{AFX_MSG(CCustomListBox)
    //}}AFX_MSG
    afx_msg void OnCmdRange(UINT id);
    afx_msg void OnCmdUiRange(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class DllImportExport CListBoxHeader : public CHeaderCtrl
  {
  public:
    CListBoxHeader() { pOwner = NULL; };
    inline void SetOwner(CWnd* pWnd) { pOwner = pWnd; };
    //{{AFX_VIRTUAL(CListBoxHeader)
    public:
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    //}}AFX_VIRTUAL
  public:
    virtual ~CListBoxHeader() {};
  protected:
    CWnd* pOwner; //pointer to window associated with the header
    //{{AFX_MSG(CListBoxHeader)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

class DllImportExport CHeaderListBox : public CCustomListBox
  {
  public:
    CHeaderListBox();
    void SetHeaderCount(int Count);
    void SetHeaderItem(int index, char* Txt, int Width, int fmt = HDF_LEFT);
    BOOL MakeHeader(UINT nID);
    void UpdateHeader();
    void GetTextInColumn(int nColumn, int nIndex, CString& rString);
    int SetTextInColumn(int nColumn, int nIndex, LPCTSTR lpszItem);
    int FindStringExactInColumn(int nColumn, int nIndexStart, LPCTSTR lpszFind);
    
  public:
    //{{AFX_VIRTUAL(CHeaderListBox)
    //}}AFX_VIRTUAL
  public:
    virtual ~CHeaderListBox();
  protected:
    CListBoxHeader Header;  //header control
    int iHeadCount;         //number of columns
    HD_ITEM* pHDItems;      //array of column info
    CStringArray Headings;  //array of column titles
    int iFontWidth;         //width of font used
    BOOL bCreatedHeader;    //true once header control has been created

  protected:
    void SetTabs();
    //{{AFX_MSG(CHeaderListBox)
    //}}AFX_MSG
    afx_msg LRESULT OnSetHeaderTabs(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
// COMCOMBO Shows how to implement a self-contained combo box with buttons.

// Combo box with Add/Delete buttons.
class DllImportExport CComboCombo : public CComboBox 
  {
  private:
    COwnedButton m_buttonAdd;     //"Add" button
    COwnedButton m_buttonDel;     //"Delete" button
    COwnedButton m_buttonPrev;    //"Prev" button
    COwnedButton m_buttonNext;    //"Next" button
    UINT m_idCombo;               //ID of combo box
    UINT m_idAdd;                 //ID of add button
    UINT m_idDel;                 //ID of delete button
    UINT m_idPrev;                //ID of prev button
    UINT m_idNext;                //ID of next button
  public:
    CComboCombo();
    virtual ~CComboCombo() {};
    //{{AFX_VIRTUAL(CComboCombo)
    public:
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    //}}AFX_VIRTUAL
    void UpdateButtons();         //helper to update button states
    BOOL SubclassAddDelControls(CWnd* pParent, UINT idCombo, UINT idAdd, UINT idDel);
    BOOL SubclassPrevNextControls(CWnd* pParent, UINT idCombo, UINT idPrev, UINT idNext);

  protected:
    //{{AFX_MSG(CComboCombo)
    //}}AFX_MSG
    afx_msg void OnCmdRange(UINT id);
    afx_msg void OnCmdUiRange(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };
                              
//===========================================================================

class DllImportExport CCustomTreeCtrl : public CTreeCtrl
  {
  public:
    CPoint PrevDownPoint; //last point where left OR right mouse button was pressed DOWN

    CCustomTreeCtrl() {};
    virtual ~CCustomTreeCtrl();
    HTREEITEM FindChildItem(HTREEITEM H, char* pTxt);
    HTREEITEM FindItem(char* pTxt, byte RqdLevel=0);
    //{{AFX_VIRTUAL(CCustomTreeCtrl)
    //}}AFX_VIRTUAL
  protected:
    HTREEITEM FindItem(HTREEITEM H, byte Level, char* pTxt, byte RqdLevel);
    //{{AFX_MSG(CCustomTreeCtrl)
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

//see commctrl.h
#ifdef UNICODE
#define STVN_SELECTEDCHANGED    (TVN_FIRST-73)
#else
#define STVN_SELECTEDCHANGED    (TVN_FIRST-22)
#endif

class DllImportExport CSelectTreeCtrl : public CCustomTreeCtrl
  {
  public:
    CSelectTreeCtrl() {};
    virtual ~CSelectTreeCtrl();
    void    LoadImage();
    void    ToggleItem(HTREEITEM hItem);
    BOOL    IsAnItemSelected(HTREEITEM hStartItem);
    void    SelectAll(BOOL On);
    int     CntSelected(HTREEITEM hStartItem);
    BOOL    GetItemSelected(HTREEITEM hItem, BOOL MustBeEndItem=TRUE);
    void    SetItemSelected(HTREEITEM hItem, flag Selected);
    int     CntLocked(HTREEITEM hStartItem);
    BOOL    GetItemLocked(HTREEITEM hItem, BOOL MustBeEndItem=TRUE);
    void    SetItemLocked(HTREEITEM hItem);
    void    FixAllLockImages();
    void    FixAllSelectedImages();
    //{{AFX_VIRTUAL(CSelectTreeCtrl)
    //}}AFX_VIRTUAL
  protected:
    CImageList SelImgList;
    void DoCnt(HTREEITEM hStartItem, int Image, int& Cnt, int& TtlCnt);
    BOOL DoGetItem(HTREEITEM hItem, int Image, BOOL MustBeEndItem);
    void DoSetItem(HTREEITEM hItem, int Image, BOOL SetChildren);
    void DoSetAllChildrenImage(HTREEITEM h, int Image);
    void DoFixParentImages(HTREEITEM hStartItem, int Image, int PartImage, int NoneImage);
    void SetAllChildrenSelectedImage(HTREEITEM h, int Selected);
    //{{AFX_MSG(CSelectTreeCtrl)
    afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class DllImportExport CCustomListCtrl : public CListCtrl
  {
  public:
    CPoint PrevDownPoint; //last point where left OR right mouse button was pressed DOWN

    CCustomListCtrl() {};
    //{{AFX_VIRTUAL(CCustomListCtrl)
    //}}AFX_VIRTUAL
  public:
    virtual ~CCustomListCtrl();
    void SetItemImage(int iPos, int iImage) 
      { 
      SetItem(iPos, 0, LVIF_IMAGE, "", iImage, 0, 0, 0); 
      };
    int GetItemImage(int iPos) 
      { 
      LVITEM item;
      memset(&item, 0, sizeof(item));
      item.mask = LVIF_IMAGE;
      item.iItem = iPos;
      if (GetItem(&item))
        return item.iImage;
      return -1;
      };
    int GetFirstSelectedItem() 
      {
      POSITION pos = GetFirstSelectedItemPosition();
      return pos ? GetNextSelectedItem(pos) : -1;
      };
    int FindString(int nIndexStart, LPCTSTR lpszFind);
    int FindStringExact(int nIndexStart, LPCTSTR lpszFind);
  
  protected:
    //{{AFX_MSG(CCustomListCtrl)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

extern DllImportExport COLORREF CustomColours[16];

//===========================================================================

//===========================================================================

#undef DllImportExport
#endif


