//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __CMDMENU_H
#define  __CMDMENU_H

//===========================================================================

_FWDDEF(CommandWnd)

#ifndef __GPWFUNCS_H
#include "gpwfuncs.h"
#endif

#define DllImportExport

const int BtnW = 30;
const int BtnH = 16;
const int MaxToks = 256;//50;
          
//const int CmdWndMaxLines = 40;
const int MaxVNTDepth    = 8;
const int CmdLineLen     = 2048;//1024; //remember that MaxToks must be "proportional" to this!
const int CmdExtraLen    = 32;

const int EF_Edit   = 0x0001;
const int EF_Run    = 0x0002;
const int EF_Stop   = 0x0004;
const int EF_ERS    = EF_Edit | EF_Run | EF_Stop;

// const int EX_INIT  = 0x0001;  // First Modifier Complete
const int EX_BEGIN          = 0x00000001;  // Start Of Next Token ????????? not 
const int EX_MODIFIER_RDY   = 0x00000002;  // Token Complete 
const int EX_PARAMETER_RDY  = 0x00000004;  // Parm Complete
const int EX_TOKEN_RDY      = 0x00000006;  // Above Combined
const int EX_EXEC           = 0x00000008;  // on '\n' at end of command { or ':'}
const int EX_DOIT           = 0x00000010;  // 
const int EX_UNDO           = 0x00000020;  // 
const int EX_MOUSE_LDOWN    = 0x00000040;  // if mouse pressed 
const int EX_MOUSE_LMOVE    = 0x00000080;  // while mouse moves
const int EX_MOUSE_LUP      = 0x00000100;  // on mouse release
const int EX_MOUSE_RDOWN    = 0x00000200;  // if mouse pressed 
const int EX_MOUSE_RMOVE    = 0x00000400;  // while mouse moves
const int EX_MOUSE_RUP      = 0x00000800;  // on mouse release
const int EX_RESULTS        = 0x00001000;  // after executed 
const int EX_ALLUNDONE      = 0x00002000;  // after all undone
const int EX_TERM_SEMICOLON = 0x00004000;  
const int EX_MOUSE_LDCLK    = 0x00008000;  // double click on left 
const int EX_MOUSE_RDCLK    = 0x00010000;  // double click on right 


// const int EX_MSE   = EX_DWN | EX_MOV | EX_UP;

const int VNT_ScrollBefore = 0x01;

const byte ModVerb = 253;
const byte ModNoun = 254;
const byte ModMse  = 255;

extern const pchar N_A;
extern const pchar AName;
extern const pchar AFloat;
extern const pchar AnInt;
extern const pchar AAny;

_FWDDEF(VerbNounEntry);
_FWDDEF(CmdIDStrEntry);
_FWDDEF(CommandBlk);

typedef VerbNounEntry *VerbNounTable;
typedef CmdIDStrEntry *CmdIDStrTable;

//---------------------------------------------------------------------------

class DllImportExport CommandBlk
  {
  friend class CommandWnd;
  protected:
    int            MaxVN;
    int            VNLen;
    pVerbNounEntry VNTable;
    int            MaxCS;
    int            CSLen;
    pCmdIDStrEntry CSTable;
    word           When; 
    byte           Noun;
    byte           Modifier;
    pVerbNounEntry ObjAccFn;
    pVerbNounEntry ObjVwFn;

  public:
    CommandBlk(int MaxVNIn, int MaxCSIn);
    ~CommandBlk();
    virtual void InitForCmd();
    virtual void InitVNT();
    virtual void BuildVNT() = 0;
    virtual void SelectVNT() {};
    virtual flag UpdateCmdStr(int nID, CCmdUI* pCmdUI);
    virtual flag ProcessCmdStr(int nID, rStrng Str);
  };

//---------------------------------------------------------------------------

//typedef void(*VNTFn)(word When, byte Noun, byte Modifier, CommandWnd *Cmd, CWnd* Wnd);
//typedef void(*IFCFn)(CommandWnd *Cmd, CWnd* Wnd);
typedef void (CommandBlk::*CmdFn)();

class VerbNounEntry 
  {
  public :
    pchar   Verb;
    pchar   Noun;
    pchar   Mods;
    char    DefModNo;
    CmdFn   Exec;
    int     Flags;

    void Set(pchar Verb_, pchar Noun_, pchar Mods_, char DefModNo_, CmdFn Exec_, int Flags_/*, CWnd* BaseWnd_*/)
      {
      Verb=Verb_;
      Noun=Noun_;
      Mods=Mods_;
      DefModNo=DefModNo_;
      Exec=Exec_;
      Flags=Flags_;
      //BaseWnd=BaseWnd_;
      };

    void Set(CmdFn Exec_, int Flags_)
      {
      Verb=NULL;
      Noun=NULL;
      Mods=NULL;
      DefModNo=0;
      Exec=Exec_;
      Flags=Flags_;
      //BaseWnd=BaseWnd_;
      };

  };

class CmdIDStrEntry 
  {
  public :
    int     iID;
    pchar   pStr;
    byte    bOnNewline:1;
    void Set (int ID, pchar Str)
      {
      iID=ID;
      pStr=Str;
      bOnNewline=1;
      }
    void SetD (int ID, pchar Str)
      {
      iID=ID;
      pStr=Str;
      bOnNewline=0;
      }
  };

#define SETVNT VNTable[VNLen++].Set
#define SETCIS CSTable[CSLen++].Set
#define SETCISD CSTable[CSLen++].SetD
//typedef VerbNounEntry *VerbNounTable;

typedef struct TokenListStruct {
  pchar         Str;
  pchar         StrInText;
  pStrng        Prmpt;
  word          Done;
  char          NDigs;
  byte          NParms;
  flag          FreeForm;
  Cursor_Types  CursNo;
  CommandBlk    *CBlk;
  VerbNounEntry *CCmd;
  byte          NounNo,ModNo;
  void*         Data;
  }TokenListStruct;
typedef TokenListStruct TokenList[MaxToks];

typedef struct CmdLnStruct {
  char      AsTokens[CmdLineLen]; //WARNING : no string overflow checks!!!
  char      Text[CmdLineLen]; //WARNING : no string overflow checks!!!
  char      MoreText[CmdLineLen]; //WARNING : no string overflow checks!!!
  int       Id;
  flag      Insert;
  char      Extra[CmdExtraLen]; //WARNING : no string overflow checks!!!
  }CmdLnStruct;

//===========================================================================

#define MaxFnKeyNo  16

class DllImportExport CommandWnd : public CView
  {
  DECLARE_DYNCREATE(CommandWnd)
  protected:
    CommandWnd();     // protected constructor used by dynamic creation
  public:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
    virtual void Initialise();//pCommandBlk CBlk, CWnd* BaseWndIn);
    virtual void Terminate();//CWnd* BaseWndIn);
    virtual ~CommandWnd();
    virtual void OnDraw(CDC* pDC);    // overridden to draw this view
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    
    void SetScaling(CDC* dc);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnBumpInput();
    afx_msg void OnClose();
    
    void SetFunctionKey(int SettingFnNo, pchar FnStr);
    void AutoSpace(flag On);
    int ProcessAStr(pchar p);
    void ProcessAChar(flag IsVirtual, byte c, char DoRepaint);
    void ParseCmdLine();
    void UndoCmdLine();
    flag ExecCmdLine(word When, int TokenNo = -1);
    flag ExecObjAcc(word When);
    flag ExecObjVw(word When);
    //void CommitCmdLine();
    void LnIsCommand(char Yes);
    void Print(pchar fmt, ...);
    void FlushPrintList();
    //void NewLn();
    void RedirectOutput(pchar Fn, char Append);
    //void DeletingCmdBlk(pCommandBlk CmdBlk_);
    void OpenCommandBlk(pCommandBlk CmdBlkIn, flag Permanent=False);
    void CloseCommandBlk(pCommandBlk CmdBlkIn);
    void CloseCommandBlksTo(pCommandBlk CmdBlkIn);
    pchar Token(int i) {return TL[Range(0,i,LastTokenNo+1)].Str;};
    pchar LastToken() {return TL[LastTokenNo].Str;};
    byte LastTokenIndex() {return LastTokenNo;};
    void NextTokenIs(pchar what);

    void BlockKBD() {/**dbgpln("Set KBDBlock = 1");**/KBDBlock = 1;};
    void FreeKBD() {/**dbgpln("Set KBDBlock = 0");**/KBDBlock = 0;};
    flag KBDBlocked() {return KBDBlock;};
    void BlockBTN() {/**dbgpln("set BTNBlock = 1");**/BTNBlock = 1;};
    void FreeBTN() {/**dbgpln("set BTNBlock = 0");**/BTNBlock = 0;};
    flag BTNBlocked() {return BTNBlock;};
    void BlockDblClk() {/**dbgpln("set DblClkBlock = 1");**/DblClkBlock = 1;};
    void FreeDblClk() {/**dbgpln("set DblClkBlock = 0");**/DblClkBlock = 0;};
    flag DblClkBlocked() {return DblClkBlock;};
    void SetRetCode(int RetCode) { iLastRetCode = RetCode; };
    void SetDlgBusy(flag Busy = 1) { bDlgBusy = Busy; SetBusyWnd(Busy); };
    flag IsDlgBusy() { return bDlgBusy; };
    void SetBusyWnd(flag Busy) { pBusyDocument = Busy ? CurrentVNTDocument : NULL; };
    CDocument* BusyDocument() { return pBusyDocument; };
    flag IsEscBusy() { return bEscBusy; };
    void SetShiftCtrl(flag ShftDown, flag CtrlDown) { fShiftDown = ShftDown; fCtrlDown = CtrlDown; };
    flag ShiftDown() { return fShiftDown; };
    flag CtrlDown() { return fCtrlDown; };
    int LastRetCode() {return iLastRetCode;};
    Cursor_Types CursNo() {return TL[LastTokenNo].CursNo;};
    int  TokenIsModParm(int TokenNo = -1);
    int  ParmNumber();
//    byte PrvModifier(); 
//    byte PrvModifierIndex(); 
//    byte PrvModifierRelIndex(); 
    pchar BTNDefModifier();
    flag BTNDefModifierReqd();
    void SetDigInfo(Cursor_Types CursNo, int NDigs = -1);
    void SetParmInfo(pchar ParmType, byte NParms, flag FreeForm = 0);
    void SetDefaultModifier(int DefNo_);
    void  SetDataBlk(void* b);
    void *GetDataBlk(int which=0);
    flag  OwnerOfDataBlk();
    int   DigNo() {return NDigs;};
    static HPEN DigPen;
//    void Show_Dig_Point(tWndDigInfo &WDI, Pt_SLW &Pt, flag On=1);
//    void Hide_Dig_Point(tWndDigInfo &WDI, Pt_SLW &Pt);
//    void Show_Dig_Line(tWndDigInfo &WDI, Pt_SLW &Pt1, Pt_SLW &Pt2, flag On=1);
//    void Hide_Dig_Line(tWndDigInfo &WDI, Pt_SLW &Pt1, Pt_SLW &Pt2);
    void dParm(double &d);
    void fParm(float &f);
    void iParm(int &i);

    flag FindCmdID(int nID);
    flag UpdateCmdID(int nID, CCmdUI* pCmdUI);
    flag ExecCmdID(int nID);
    void SetEnable(flag On);

    //void VNTInvalid(); 
    void VNTChanged(); 
    //void SetDocForCmds(CDocument* pDoc);

    void ChangeToken(const char* pTok, int iTokenNo=-1);
    void ExtendCmdLine(const char* pTok);
    void ExtendModifier(const char* pTok);
    flag TestAddAnotherParm();
    
    flag Busy() { return strlen(CL.Text)>0; };

  private:

    struct 
      {                
      int        Line;
      StrngToken Text;
      }               FndRep;

    int               iLastRetCode;  //used for a return code by ProcessAStr
    CmdLnStruct       CL;
    byte              bFontsInited:1,
                      CmdLnTo:1,
                      fShiftDown:1,
                      fCtrlDown:1,
                      KBDBlock:1,
                      BTNBlock:1,
                      DblClkBlock:1,
                      AutSpc:1,
                      IsUserText:1;
    byte              bExecBusy:1,
                      bVNTChanged:1,
                      bChgCmdLnBusy:1,
                      bDlgBusy:1,
                      bEscBusy:1, //Esc pressed, busy doing UndoAll
                      VScrlOn:1,
                      HScrlOn:1,
                      bEnabled:1;
    
    SIZE              CharSize, CharOffset;
    int               CharAscent;
    //CSize             ChScrollBy;
                      
    Strng_List        Lines, PrintList;
    pchar             CmdLn;
    
    TokenList         TL;
    int               LastTokenNo;
    int               LinesInRect;
    
    int               DefModNo;

    int               CurCmdNo;
    StrngToken        BTNDefModSpc;
    VerbNounEntry     *BTNDefModVNT;

    StrngToken        PushedModifier;
    StrngToken        ExtendedModifier;
                      
    int               LastCmdBlk_Cur;
    pCommandBlk       CmdBlk_Cur[MaxVNTDepth];
    flag              CmdBlk_Cur_P[MaxVNTDepth];

    int               LastCmdBlk_Wrk;
    pCommandBlk       CmdBlk_Wrk[MaxVNTDepth];
    flag              CmdBlk_Wrk_P[MaxVNTDepth];
    
    Strng             FnStrng[MaxFnKeyNo];

    FILE              *OutFile;

    CDocument*        CurrentVNTDocument;
    CDocument*        pBusyDocument;
    
    int               NDigs;
    Strng             NewCmdLn;
    //CScrollBar *Scroller;
    CScrollBar        *VScrl, *HScrl;

    CFont       SysFont;
    //CSize       Ext;
    //CPoint      Org;

    void              Repaint(char All);
    void              Scroll(flag DoRepaint);
    void              AddTxt(pchar t);
    void              AppendNewCmdLn();
    //flag              ResetScroller(flag Force);   
    flag              CmdLnVisible(int &Row);
    int               ChWorkHeight(CRect &Cr, flag HScrollOn);
    int               ChWorkWidth(CRect &Cr, flag VScrollOn);
    void              FixScrollSizes(flag fShowHideBars, flag bReset);
    void              DisplayCmdLn(CDC * dc);
    flag              StrIsCmdLn(pchar p); 
    void              FindAndReplaceCmdLn(pchar F);
    void              ClearCmdLn();
    void              InitForCmd(int Strt=0, int End=-1);
    void              ActivateVNT();
    CDocument*        FindVNTDocument();

  protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd * pOldWnd);
    afx_msg void OnKillFocus(CWnd * pNewWnd);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg LONG OnCmdWndStr(UINT wParam, LONG lParam);    
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

extern DllImportExport CommandWnd *gs_pCmd;
extern DllImportExport CMDIChildWnd *gs_pCmdFrame;

//---------------------------------------------------------------------------


#undef DllImportExport 
#endif
