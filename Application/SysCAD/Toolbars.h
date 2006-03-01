//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

const BYTE MaxDynBitmaps = 5;
const BYTE MaxDynToolBars = 8;
enum DynButtonStyles { ButSeparator, ButBitmap, ButText, ButCombo, ButEdit };

class CDynToolBarInfo
  {
  public:
    char* pName;     //toolbar title
    UINT iBitmapID;  //resource ID of bitmap used
    UINT iToolbarID; //unique ID of toolbar
    UINT iLen;       //number of command IDs
    UINT* IDs;       //array of command IDs
  };

//---------------------------------------------------------------------------

class CDynToolBar : public CToolBar
  {
  DECLARE_DYNAMIC(CDynToolBar)
  friend class CToolBarMngr;
  friend class CToolBarBitmap;
  public:
    CDynToolBar();
    CDynToolBar(char* pTitle, CToolBarMngr* TBMngr, BOOL CanConfigure = FALSE, UINT WindowID = AFX_IDW_TOOLBAR);
    ~CDynToolBar();
    int AddButtons(UINT BitmapID, UINT* pBtnIDs, UINT Len);
    int AddButtonsOnce(UINT BitmapID, UINT* pBtnIDs, UINT Len);
    int ChangeButtons(UINT* pBtnIDs, UINT Len);
    void AddTextButton(char* pTxt, UINT BtnID);
    void AddComboButton(UINT BtnID);
    void AddEditButton(UINT BtnID);
    void AddSeparator();
    int Create(BOOL ShowTitle = FALSE, BOOL FloatIt = FALSE, DWORD dwDockingStyle = 0, DWORD dwBarStyle = CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, BOOL Show = TRUE, BOOL UseEscape = FALSE);
    void SaveState(char* pFilename);
    void LoadState(char* pFilename);
    BOOL GetPos(CPoint& Pt);
    int FindComboButton(CWnd* pWnd);
    int FindComboButtonIndex(CWnd* pWnd);
    int FindComboButtonIDIndex(int ComboButtonIDIndex);
    void SetTextButtonText(char* pTxt, int Index);
    void SetTitle(char* pTxt, BOOL SaveChange = TRUE);
    void AppendToTitle(char* pTxt);
    void Customize();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    inline void SetPos(int x, int y) { Location.x = x; Location.y = y; };
    inline const char* GetTitle() { return (const char*)sTitle; };
    inline int CurSelLen() { return sCurSel.GetLength(); };
    inline char* CurSel() { return (char*)(const char*)sCurSel; };
    inline CComboBox* GetComboButton(int Index) { return &pComboButtons[Index]; };
    inline CEdit* GetEditButton(int Index) { return &pEditButtons[Index]; };
    inline BOOL ComboListChanged() { return bComboListChanged; };
    inline BOOL Configurable() { return bCanConfigure; };
    void ChangeButtonIDS(UINT ID1, UINT ID2, UINT IDRQD);
  private:
    int AddButtons(UINT* pBtnIDs, UINT Len, BYTE Style);
    BOOL DoCreateButtons();
  protected:
    BOOL RClick(WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    BOOL BeginAdjust(WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    BOOL BeginDrag(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL CustomizeHelp(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL EndAdjust(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL EndDrag(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL GetButtonInfo(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL QueryDelete(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL QueryInsert(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
    BOOL Reset(WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
    BOOL ToolBarChange(WPARAM wParam, LPARAM lParam,LRESULT* pLResult);

  protected:
    CString         sTitle;         //floating toolbar title
    CToolBarMngr*   pTBMngr;        //pointer to parent/owner
    UINT            iWindowID;      //Window ID of toolbar if required
    CUIntArray      BtnIDs;         //array of Button IDs
    CByteArray      BtnStyles;      //array of Button styles
    CStringArray    BtnTxts;        //array of texts for text buttons
    UINT            iLen;           //number of IDs
    UINT            iBitLen;        //number of buttons with bitmaps
    UINT            iTxtLen;        //number of text buttons
    UINT            iComboLen;      //number of combo boxes
    UINT            iEditLen;       //number of edit boxes
    CToolBarBitmap* pTheBitmap;     //bitmap used (if only one is used)
    CBitmap         Bitmap;         //actual bitmap
    char*           pBits;          //array of bits for bitmap
    CButton*        pTxtButtons;    //array of text buttons
    CComboBox*      pComboButtons;  //array of combo buttons
    CEdit*          pEditButtons;   //array of edit buttons
    TBBUTTON*       Btns;           //array of button struct info
    CPoint          Location;       //location to show toolbar
    CString         sCurSel;        //currently selected string
    BYTE            bComboListChanged:1,//has the list of strings for ANY of the combo boxes changed
                    bCanConfigure:1,//can the buttons be rearranged etc
                    bButtonsOnce:1, //has the AddButtonsOnce method been called
                    bWasVisible:1,  //was the toolbar previously visible
                    bUseEscape:1;   //special for SysCAD tempary floating toolbars

    //{{AFX_MSG(CToolBar)
    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

class CToolBarBitmap
  {                               
  friend class CToolBarMngr;
  friend class CDynToolBar;
  public:
    CToolBarBitmap(UINT BitmapID, UINT* pBtnIDs, UINT Len, CToolBarMngr* TBMngr);
    ~CToolBarBitmap();
    int GetBtnIndex(UINT BtnID);
  protected:
    UINT           iBitmapID;
    CUIntArray     BtnIDs;
    UINT           iLen;
    CBitmap        Bitmap;
    char*          pBits;
    TBBUTTON*      Btns;           //array of button struct info
  };

//---------------------------------------------------------------------------

class CToolBarMngr
  {
  friend class CDynToolBar;
  friend class CToolBarBitmap;
  public:
    CToolBarMngr();
    ~CToolBarMngr();
    //void Initialise(CMainFrame* MainFrame);
    void Initialise(CMDIFrameWnd* MainFrame);
    int AddBitmap(UINT BitmapID, UINT* pBtnIDs, UINT Len);
    CToolBarBitmap* FindBitmap(UINT BitmapID);
    CDynToolBar* AddToolbar(char* pTitle, BOOL CanConfigure = FALSE, UINT WindowID = AFX_IDW_TOOLBAR);
    CDynToolBar* FindToolbar(char* pTitle);
    CDynToolBar* operator[](int i) { return ToolBars[i]; };
    int FindToolbar(CDynToolBar* pTB);
    void RemoveToolbar(char* pTitle);
    void RemoveAllToolbars();
    void SaveState(char* pFilename);
    void LoadState(char* pFilename);
    void ChangeButtonIDS(UINT ID1, UINT ID2, UINT IDRQD);
  protected:
    UINT             BitsPixel;
    UINT             BtnWidth;
    UINT             BtnHeight;
    UINT             BtnSize;
    BYTE             iBitmapCnt;
    CMDIFrameWnd*    pMainFrame;
    CToolBarBitmap*  Bitmaps[MaxDynBitmaps];
    CDynToolBar*     ToolBars[MaxDynToolBars];
    CFont            Font;
  };

//---------------------------------------------------------------------------

extern CDynToolBar* pGrfTB;        //the current graphics toolbar
extern CToolBarMngr* pGrfTBMngr;   //mngr for the dynamic temporary graphics toolbars
extern CToolBarMngr* pMainTBMngr;  //mngr for the main permanent SysCAD toolbars

//---------------------------------------------------------------------------
