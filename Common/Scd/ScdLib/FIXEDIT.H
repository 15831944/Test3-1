//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __FIXEDIT_H
#define __FIXEDIT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.H"
#endif
#ifndef __GDIBLK_H
  #include "gdiblk.h"
#endif
#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif

#ifdef __FIXEDIT_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

const byte FULL_UPDATE    = 0x01;
const byte DATA_FIELDS    = 0x02;
const byte DESC_FIELDS    = 0x04;
const byte PARM_FIELDS    = 0x08;
const byte GRAPH_FIELDS   = 0x10;
const byte SEPAR_FIELDS   = 0x20;
const byte ALL_FIELDS     = 0x3e;

const byte FXWC_BTN       = 1;
const byte FXWC_CHECK     = 2;
const byte FXWC_CHECKBTN  = 3;
const byte FXWC_DDLST     = 4;
const byte FXWC_DDLSTOPT  = 5;
const byte FXWC_TREEND    = 6;
const byte FXWC_BM1BTN    = 7;
const byte FXWC_BM2BTN    = 8;

const int  MaxFixedFields = 64;//kga 2/10/02 increased this from 48 to 64 for RTTS sizedist data
const int  MaxFixedRows   = 4096;
const int  MaxBlkIds      = 256;
const int  MaxFixedPages  = 64;
const int  MaxFixedControls=256;
const word FxdEdtCtrlID0  = 200;

_FWDDEF(FxdEdtFld)
_FWDDEF(FxdEdtRow)
_FWDDEF(FxdEdtBlk)
_FWDDEF(FxdEdtPage)
_FWDDEF(FxdEdtBook)
_FWDDEF(FxdEdtBookRef)
_FWDDEF(FxdEdtInfo)
_FWDDEF(FxdEdtView)
_FWDDEF(TagInfoBlk)

//===========================================================================

class DllImportExport TagInfoBlkItem
  {
  public:
    Strng           m_sObjClassId;
    Strng           m_sRefTag;
    CnvAttribute   *m_pCnv;
    FmtAttribute   *m_pFmt;

    byte            m_iType;
    Strng           m_sObjTag;
    TagInfoBlkItem()
      {
      m_pCnv=NULL;
      m_pFmt=NULL;
      }
  };

class DllImportExport TagInfoBlk
  {
  public:
    CArray <TagInfoBlkItem, TagInfoBlkItem&> m_Item;
  
  public:
    TagInfoBlk()
      {
      m_Item.SetSize(1);
      m_Item[0].m_pCnv=NULL;
      m_Item[0].m_pFmt=NULL;
      }
    TagInfoBlk(char * ObjClassId, byte Type, char * ObjTag, char * RefTag, CnvAttribute*Cnv=NULL, FmtAttribute * Fmt=NULL)
      {
      m_Item.SetSize(1);
      m_Item[0].m_sObjClassId=ObjClassId;
      m_Item[0].m_sRefTag=RefTag;
      m_Item[0].m_sObjTag=ObjTag?ObjTag:"";
      m_Item[0].m_iType=Type;
      m_Item[0].m_pCnv=Cnv;
      m_Item[0].m_pFmt=Fmt;
      };
    void Set(char * ObjClassId, byte Type, char * ObjTag, char * RefTag, CnvAttribute*Cnv=NULL, FmtAttribute * Fmt=NULL)
      {
      m_Item.SetSize(1);
      m_Item[0].m_sObjClassId=ObjClassId;
      m_Item[0].m_sRefTag=RefTag;
      m_Item[0].m_sObjTag=ObjTag?ObjTag:"";
      m_Item[0].m_iType=Type;
      m_Item[0].m_pCnv=Cnv;
      m_Item[0].m_pFmt=Fmt;
      };
    void Set(char * RefTag, CnvAttribute*Cnv=NULL, FmtAttribute * Fmt=NULL)
      {
      m_Item.SetSize(1);
      m_Item[0].m_sObjClassId="";
      m_Item[0].m_sRefTag=RefTag;
      m_Item[0].m_sObjTag="";
      m_Item[0].m_iType=0;
      m_Item[0].m_pCnv=Cnv;
      m_Item[0].m_pFmt=Fmt;
      };
    void Set(CnvAttribute*Cnv)
      {
      m_Item.SetSize(1);
      m_Item[0].m_pCnv=Cnv;
      };
    void Set(FmtAttribute* Fmt)
      {
      m_Item.SetSize(1);
      m_Item[0].m_pFmt=Fmt;
      };
    void Clear()
      {
      m_Item.SetSize(1);
      m_Item[0].m_sObjClassId="";
      m_Item[0].m_sRefTag="";
      m_Item[0].m_sObjTag="";
      m_Item[0].m_iType=0;
      m_Item[0].m_pCnv=NULL;
      m_Item[0].m_pFmt=NULL;
      };

    char *          ObjClassId(int i=0)  { return m_Item[i].m_sObjClassId(); };
    char *          RefTag(int i=0)      { return m_Item[i].m_sRefTag(); };
    char *          ObjTag(int i=0)      { return m_Item[i].m_sObjTag(); };
    byte            Type(int i=0)        { return m_Item[i].m_iType; };
    CnvAttribute  & Cnv(int i=0)         { return *m_Item[i].m_pCnv;};
    FmtAttribute  & Fmt(int i=0)         { return *m_Item[i].m_pFmt;};
    flag            CnvOK(int i=0)       { return m_Item[i].m_pCnv!=NULL;};
    flag            FmtOK(int i=0)       { return m_Item[i].m_pFmt!=NULL;};
    int             GetCount()           { return m_Item.GetSize(); };
  };

// --------------------------------------------------------------------------

const UCHAR FFM_Off             = 0x00;
const UCHAR FFM_HasGet          = 0x01;
const UCHAR FFM_HasGetCnt       = 0x02;
const UCHAR FFM_HasSet          = 0x04;             
const UCHAR FFM_HasSetCnt       = 0x08;          
const UCHAR FFM_HasMultiSets    = 0x10;
const UCHAR FFM_HasMultiSetCnts = 0x20;
const UCHAR FFM_HasReset        = 0x40;
const UCHAR FFM_HasInactive     = 0x80;

class DllImportExport FxdEdtFld
  {
  DEFINE_SPARES(FxdEdtFld)
  public:
    Strng           Str;
    char *          Tag;
    int             iCtrlIndex;   
    byte            FieldType;
    byte            m_iCtrlType;   
    byte            iPg;
    UCHAR           fEditable:1,fEditing:1,Just:2,ToUpdate:1,
                    AcceptChanges:1, Changed:1, IsButton:1; 
    UCHAR           IsIndexTab:1, ButtonDown:1, IsFxdStrings:1, HasNANStrings:1,
                    HasDDLst:1, HasEdtBox:1,
                    fItalic:1, fInError:1, fFixedPos:1, 
                    fDisabled:1, fIsResult:1;
    UCHAR           fSelected:1, fSelectable:1, fProtected:1, fUnderLine:1;
    UCHAR           m_Marks, m_MarksChgd;
    short           Len, Col1, ColN;
    short           BigStrLength;
    int             BlkId;
    int             FieldId;
    FxdEdtRow*      pRow;
    FxdEdtView*     pView;

    FxdEdtFld(FxdEdtView* pView_, FxdEdtRow* pRow_, int BlkId_, int FieldId_, int Col1_, int ColN_, pchar Str_, UCHAR Just_, BOOL Editable_, byte FieldType_);
    virtual ~FxdEdtFld();

    byte CtrlType();   

    inline int Xs(rGDIBlk GB);
    inline int Xe(rGDIBlk GB);
    inline int Ys(rGDIBlk GB);
    inline int Ye(rGDIBlk GB);

    void DrawMark(rGDIBlk GB, int iMark, bool InActiveColour);
    void Write(rGDIBlk GB, flag WithEdge, flag FullDraw);
    void Invalidate(rGDIBlk GB);
    virtual void FixAfterLoad(Strng &Str);//rTmpFldBuff Buff, flag DoCopyToStr=True);
    virtual void FixBeforeParse();

    void SetItalic(flag fItalic_=True) {fItalic=fItalic_;};
    flag IsItalic() {return fItalic;};

    void SetDisabled(flag fDisabled_=True) {fDisabled=fDisabled_;};
    flag IsDisabled() {return fDisabled;};

    void SetTag(char * TagStr, char * CnvTxt=NULL);
    void SetBigStrLength(int Len) { BigStrLength=Len; };
    void SetMarks(UCHAR M) { if (M!=m_Marks) { m_Marks=M; m_MarksChgd=1; }; };

  };

// --------------------------------------------------------------------------

class DllImportExport FxdEdtRow
  {
  DEFINE_SPARES(FxdEdtRow)
  public:
    FxdEdtFld*    Field[MaxFixedFields];
    short         NFields, Cols, RowNo;
    FxdEdtView*   pView;
    int           fFixedPos;

    FxdEdtRow(FxdEdtView* pView_);
    ~FxdEdtRow();
    FxdEdtFld* SetFld(FxdEdtView* pView_, int RowNo_, int BlkId_, pchar InitStr, int FieldId, int Width, UCHAR Just, BOOL Editable, byte FieldTyp);
    BOOL Visible(rGDIBlk GB);
    int Ys(rGDIBlk GB);
    int Ye(rGDIBlk GB);
    void Write(rGDIBlk GB);
    void Invalidate(rGDIBlk GB);
    FxdEdtFld* FldFromId(int IdReqd);
  };

// --------------------------------------------------------------------------

class DllImportExport FxdEdtInfo
  {
  DEFINE_SPARES(FxdEdtInfo)
  public:
    int           BlkId;
    int           FieldId;
    FxdEdtPage  * Pg;
    int           PageNo;
    int           PgRowNo;
    int           BlkRowNo;
    int           Index;
    pvoid         Ptr;
    FxdEdtFld   * Fld;
    flag          fPasteBusy;

    FxdEdtInfo();
    FxdEdtInfo(FxdEdtView* FE_, pFxdEdtPage Pg_, FxdEdtFld* Fld_, int r);
  };

// --------------------------------------------------------------------------

class DllImportExport FxdEdtBlk
  {
  DEFINE_SPARES(FxdEdtBlk)
  public:
    int           Pg1;
    int           Row1;
    int           Len;
    int           Index;
    pvoid          Ptr;
    
    FxdEdtBlk();
    ~FxdEdtBlk();
  };

// --------------------------------------------------------------------------

class DllImportExport FxdEdtPage
  {
  DEFINE_SPARES(FxdEdtPage)
  public:
    FxdEdtView*    pView;
    short          No, Rows, MaxCols;
    int            CurBlkId;
    FxdEdtRow*     Row[MaxFixedRows];
    char           TabName[64];
    short          TabWide;
    short          TabPos;
    short          TabRow;
    short          FixedRows;
    bool           bLoadByCol;

    FxdEdtBlk      Blk[MaxBlkIds];
    short          Blks;
    
    FxdEdtPage(FxdEdtView* pFE_, pchar Name_, int No_, bool LoadByCol);
    virtual ~FxdEdtPage();
    void SetRows(int NRows_);
    FxdEdtFld* SetFld(byte FieldType, int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable);
    void LoadStrings();
    void ParseStrings();
    int MoveRows(int RowShift);
    virtual FxdEdtFld* FldFromId(int RowNo, int FieldId);
    virtual void SetFieldValue(rGDIBlk GB, FxdEdtFld* Fld, pchar Val);
    virtual void SetFieldValue(rGDIBlk GB,int RowNo, int FieldId, pchar Val);
  };

//=======================================================================

_FWDDEF(CFxdEdtListBox)
class CFxdEdtListBox : public CListBox
  {
  protected:
    flag    fCloseIt;
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
  };

//=======================================================================

_FWDDEF(CFxdEdtComboBox)
class CFxdEdtComboBox : public CComboBox
  {
  protected:
    flag    fCloseIt;
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
  };

//=======================================================================

class DllImportExport FxdEdtCtrl
  {
  DEFINE_SPARES(FxdEdtCtrl)
  public :
    FxdEdtFld*      pFld;
    pvoid           pCtrl;
    pStrng_List     pStrLst;
    flag            fPlus;
    //CFxdEdtListBox *pLstBox;
    //Strng           sLstBoxOrigValue;
    //int            iIndex;

    FxdEdtCtrl() 
      {
      pFld=NULL;
      pCtrl=NULL;
      pStrLst=NULL;
      //pLstBox=NULL;
      fPlus=1;
      };

  } ; 
typedef FxdEdtCtrl *pFxdEdtCtrl;

//=======================================================================

class DllImportExport FxdEdtBook
  {
  public:
    FxdEdtBook();
    virtual ~FxdEdtBook();

    UCHAR          RebuildReqd:1, ReloadReqd:1, CaretOK:1, WasEditing:1,
                   fFixedPos:1, fForce1Page:1;

    short          CPgNo;
    short          Pages;
    FxdEdtPage*    CPg;
    FxdEdtPage*    Pg[MaxFixedPages];

    short          nCtrls;
    FxdEdtCtrl*    pCtrl;

    CFxdEdtComboBox *pComboBox;

    FxdEdtFld*     pPrevFld;
    short          TabIndexRows;

    SIZE           ChEditSize;
    POINT          ChEditPos;

    FxdEdtFld*     DownButton;
    FxdEdtFld*     FldEdit;

    FxdEdtView*    pView;
    CWnd*          pWnd;

    UINT           iCmdCutTag;
    UINT           iCmdCutVal;
    UINT           iCmdCopyTag;
    UINT           iCmdCopyRHSTag;
    UINT           iCmdCopyVal;
    UINT           iCmdPaste;
    UINT           iSendToTrend;
    UINT           iSendToQuickView;
    UINT           iSendToHistory;
    UINT           iSendToArchive;
    UINT           iSendToCustom;

    short          iSlctRow;
    short          iSlctFld;

  // Operations
  public:
    int StartPage(pchar PgName_, bool LoadByCol);
    int StartBlk(int Len, int Index_, pvoid Ptr_);
    flag CurrentBlk(FxdEdtInfo &EI);
    FxdEdtFld* SetFld(byte FieldType, int RowNo, pchar InitStr, int FieldId_=-1, int Width=1, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE);
    FxdEdtFld* SetDesc(int RowNo, pchar InitStr, int FieldId_=-1, int Width=0, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=FALSE);
    FxdEdtFld* SetSpace(int RowNo, int Width=0);
    FxdEdtFld* SetParm(int RowNo, pchar InitStr, int FieldId_=-1, int Width=1, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE);
    FxdEdtFld* SetData(int RowNo, pchar InitStr, int FieldId_=-1, int Width=0, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE);
    FxdEdtFld* SetDParm(int RowNo, pchar DStr, int DWidth, pchar InitStr, int FieldId_=-1, int Width=1, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE);
    FxdEdtFld* SetDData(int RowNo, pchar DStr, int DWidth, pchar InitStr, int FieldId_=-1, int Width=0, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE);
    FxdEdtFld* SetButton(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator);
    FxdEdtFld* SetBitmapButton1(int RowNo, int BitmapID, int FieldId, int Width, UCHAR Just, pchar Separator);
    FxdEdtFld* SetBitmapButton2(int RowNo, int BitmapID, int FieldId, int Width, UCHAR Just, pchar Separator);
    FxdEdtFld* SetCheckBox(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable=TRUE);
    FxdEdtFld* SetCheckBoxBtn(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable=TRUE);
    FxdEdtFld* SetTreeNd(int RowNo, flag fPlus, int FieldId, int Width, UCHAR Just, pchar Separator);
    
    void SetFixedPosition(int Fixed=True);
    void SetTag(pchar TagStr, char * CnvTxt=NULL);
    void SetBigStrLength(int Len);
    void SetMarkerFlags(UCHAR nMarker);
    void SetForce1Page(int Force=True);


    FxdEdtFld* AdjustFld(int PgNo, int RowNo, int FldNo, int WdtDelta);

    void FldHasFixedStrValue(int i, pchar pStrVal);
    void FldHasNANStrValue(int i, pchar pStrVal);
    
    void AppendCtrlList(FxdEdtFld* pFld_);//, pvoid pCtrl_);//t iIndex_, pStrng pStr_);
    void AppendStrValue(FxdEdtFld* pFld_, int iIndex_, pchar pStr_);
    void DeleteCtrlList();

    void DoRebuild() { RebuildReqd=1;};
    void DoReload() { ReloadReqd=1;};
    flag AnyFldsChanged();
    flag ClrFldsChanged();

    pFxdEdtPage   Page(int p) {return Pg[p];};
    pFxdEdtPage   CPage() {return CPg;};
    FxdEdtRow*    CRow(int r) {return CPg->Row[r];};
  
    int           EdtRow();
    FxdEdtFld*    CurrentFld();
    FxdEdtFld*    EdtFld();
    FxdEdtFld*    ButtonFld();
    int           EdtChar(FxdEdtFld* Fld);
    FxdEdtFld*    FirstSlctFld();
    FxdEdtFld*    NextSlctFld();
    flag          MoreSlctFlds();

    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);  
    virtual void OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual void OnDrawLine(rGDIBlk GB,int PgNo, int RowNo);  
    virtual void StartBuild();
    virtual int BuildStdPageHeader() { return 0; };
    virtual void Build(int NRows=0, int NCols=0, int ColIndent=0, pvoid BuildData=NULL);
    virtual void BuildDone();
    virtual void ChangeLayout(int TotalRows, int TotalCols);
    virtual void PageChanged(int NewPageNo, flag Initial);
    virtual void Load(FxdEdtInfo &EI, Strng &Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng &Str);
    virtual long Parse(FxdEdtInfo &EI, Strng &Str);
//    virtual void Load(FxdEdtInfo &EI, pchar Str, int Len);
//    virtual int ButtonPushed(FxdEdtInfo &EI, pchar Str, int Len);
//    virtual int Parse(FxdEdtInfo &EI, pchar Str);

   
//    virtual flag Changed(FxdEdtInfo &EI);
//    virtual void ClearChanged(FxdEdtInfo &EI);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);
    virtual flag DoKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists(TagInfoBlk & WrkIB);
    virtual flag DoCutTag(FxdEdtFld* pFld, char *buff, int buffsize);
    virtual flag DoCutVal(FxdEdtFld* pFld, char *buff, int buffsize);
    virtual flag DoCopyTag(FxdEdtFld* pFld, char *buff, int buffsize);
    virtual flag DoCopyRHSTag(FxdEdtFld* pFld, char *buff, int buffsize);
    virtual flag DoCopyVal(FxdEdtFld* pFld, char *buff, int buffsize);
    virtual flag DoPaste(FxdEdtFld* pFld, char *buff);

    void SetCmdCutTag     (UINT Cmd)  { iCmdCutTag      = Cmd; };
    void SetCmdCutVal     (UINT Cmd)  { iCmdCutVal      = Cmd; };
    void SetCmdCopyTag    (UINT Cmd)  { iCmdCopyTag     = Cmd; };
    void SetCmdCopyRHSTag (UINT Cmd)  { iCmdCopyRHSTag  = Cmd; };
    void SetCmdCopyVal    (UINT Cmd)  { iCmdCopyVal     = Cmd; };
    void SetCmdPaste      (UINT Cmd)  { iCmdPaste       = Cmd; };
    void SetSendToTrend   (UINT Cmd)  { iSendToTrend    = Cmd; };
    void SetSendToQuickView(UINT Cmd) { iSendToQuickView= Cmd; };
    void SetSendToHistory (UINT Cmd)  { iSendToHistory  = Cmd; };
    void SetSendToArchive (UINT Cmd)  { iSendToArchive  = Cmd; };
    void SetSendToCustom  (UINT Cmd)  { iSendToCustom   = Cmd; };

  };

//=======================================================================

const byte DCC_CutTag     = 1;
const byte DCC_CutVal     = 2;
const byte DCC_CopyTag    = 3;
const byte DCC_CopyVal    = 4;
const byte DCC_CopyRHSTag = 5;

class DllImportExport FxdEdtView : public CView, public FxdEdtBook
  {
  DECLARE_DYNCREATE(FxdEdtView)
protected:
  FxdEdtView();       // protected constructor used by dynamic creation

  virtual BOOL OnPreparePrinting( CPrintInfo* pInfo );
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView );
  virtual void OnPrint(CDC *pDC, CPrintInfo* pInfo);
  virtual void DoOnDraw(rGDIBlk rGB);    // overridden to draw this view

public:
  static bool      m_WheelChangeVal;
  static GDIBlkCfg GDICfg;
  GDIBlk           ScrGB;
  int              CPgNoSP;
  CScrollBar      *VScrl;
  CScrollBar      *HScrl;
  flag             VScrlOn:1,
                   HScrlOn:1,
                   RedrawNeeded:1,
                   bShiftDown:1,
                   bControlDown:1,
                   bChanged:1,
                   bWinCtrlListBusy:1;
  
  DWORD            dwRebuildCnt;
  HCURSOR          hRqdCursor;
public:
  virtual BOOL Create(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle, 
                      const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
//  virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
   
  void DeleteWinControls();
  void PositionWinControls();
  void SelectPage(int No, flag Initial=false);
  flag LocateFromCR(int ColNo, int RowNo, FxdEdtInfo &EI);
  virtual void InitDC(CDC* pDC);
  virtual void OnDraw(CDC* pDC);    // overridden to draw this view
  virtual void OnInitialUpdate();   // first time after construct
  virtual void CalculateLayoutSize(rGDIBlk GB);
  virtual void LoadStrings();
  virtual void ParseStrings();
  virtual void OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint);

  virtual void StartCharEdit(FxdEdtFld* Fld);
  virtual void EndCharEdit(FxdEdtFld* Fld);
  void FindNextEdtFld(BOOL Editable=TRUE);
  void FindPrevEdtFld(BOOL Editable=TRUE);
  virtual void FixCaret();
  virtual void PositionCaret(byte Options);//flag fRedraw, flag ClearSelected, flag SetSelected);
  virtual void PositionCaretAt(int ColNo, int RowNo);
  virtual void PositionScrollBarsAt(int X, int Y);
  virtual void FixScrollSizes(flag fShowHideBars);
  CPoint& Point2RowCols(rGDIBlk GB, CPoint point, CPoint& RP);
  void    SetEditPos(CPoint point);
  void    ClrCurrentEdit(BOOL Cancel);
  void    SetRqdCursor(HCURSOR hCursor);
  bool    Changed() { return bChanged; };
  void    ClrChanged() { bChanged=false; };

  void PutToFld(FxdEdtFld* pFld, Strng & Buff);
  void PasteFromClipBoard();//FxdEdtFld* pFld);
//  void DoAccCutTag();
//  void DoAccCutVal();
//  void DoAccCopyTag();
//  void DoAccCopyVal();
//  void DoAccPaste();
  void MoveCursorTo(int PageNo, int RowNo, int FldNo, flag SelectFld);
protected:
  virtual ~FxdEdtView();
  //{{AFX_MSG(FxdEdtView)
  afx_msg void OnSetFocus(CWnd* pOldWnd);
  afx_msg void OnKillFocus(CWnd* pOldWnd);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt); 
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnCut();
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  afx_msg LRESULT OnEditDone(WPARAM wParam, LPARAM lParam);
  //}}AFX_MSG
  afx_msg BOOL OnUDNDeltaPos(UINT id, NMHDR * pNotifyStruct, LRESULT * result);
  DECLARE_MESSAGE_MAP()

  void OnCutTag();
  void OnCopyTag();
  void OnCopyRHSTag();
  void DoCopyCut(int What);

  void AdjustValue(CString S, bool Up, int deltadecimal);
  virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
//  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  };

// --------------------------------------------------------------------------

class DllImportExport FxdEdtBookRef
  {
  public:
    FxdEdtView*    pView;
    Strng          sFullObjTag;
    TagInfoBlk     WrkIB;

    FxdEdtBookRef(FxdEdtView* pView_) {pView=pView_;};
    virtual ~FxdEdtBookRef()          {};
    
    void           SetView(FxdEdtView* pView_) { pView=pView_ ; };
    FxdEdtView&    View() { return *pView; };

    void SetFullObjTag(char * pTag) { sFullObjTag=pTag;};
    char *FullObjTag() { return sFullObjTag();};

    int StartPage(pchar PgName_, bool LoadByCol=false)   { return pView->StartPage(PgName_, LoadByCol);};
    int StartBlk(int Len, int Index_, pvoid Ptr_) { return pView->StartBlk(Len, Index_, Ptr_); };
    flag CurrentBlk(FxdEdtInfo &EI) {return pView->CurrentBlk(EI);};

    FxdEdtFld* SetFld(byte FieldType, int RowNo, pchar InitStr, int FieldId_=-1, int Width=1, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE)
      {return pView->SetFld(FieldType, RowNo, InitStr, FieldId_, Width, Just, Separator, Editable);};
    FxdEdtFld* SetDesc(int RowNo, pchar InitStr, int FieldId_=-1, int Width=0, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=FALSE)
      {return pView->SetDesc(RowNo, InitStr, FieldId_, Width, Just, Separator, Editable);};

    FxdEdtFld* SetSpace(int RowNo, int Width=0)
      {return pView->SetSpace(RowNo, Width);};

    FxdEdtFld* SetParm(int RowNo, pchar InitStr, int FieldId_=-1, int Width=1, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE)
      {return pView->SetParm(RowNo, InitStr, FieldId_, Width, Just, Separator, Editable);};

    FxdEdtFld* SetData(int RowNo, pchar InitStr, int FieldId_=-1, int Width=0, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE)
      {return pView->SetData(RowNo, InitStr, FieldId_, Width, Just, Separator, Editable);};

    FxdEdtFld* SetDParm(int RowNo, pchar DStr, int DWidth, pchar InitStr, int FieldId_=-1, int Width=1, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE)
      {return pView->SetDParm(RowNo, DStr, DWidth, InitStr, FieldId_, Width, Just, Separator, Editable);};

    FxdEdtFld* SetDData(int RowNo, pchar DStr, int DWidth, pchar InitStr, int FieldId_=-1, int Width=0, UCHAR Just=0, pchar Separator=NULL, BOOL Editable=TRUE)
      {return pView->SetDData(RowNo, DStr, DWidth, InitStr, FieldId_, Width, Just, Separator, Editable);};

    FxdEdtFld* SetButton(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator)
      {return pView->SetButton(RowNo, InitStr, FieldId, Width, Just, Separator); };

    FxdEdtFld* SetBitmapButton1(int RowNo, int BitmapID, int FieldId, int Width, UCHAR Just, pchar Separator)
      {return pView->SetBitmapButton1(RowNo, BitmapID, FieldId, Width, Just, Separator); };

    FxdEdtFld* SetBitmapButton2(int RowNo, int BitmapID, int FieldId, int Width, UCHAR Just, pchar Separator)
      {return pView->SetBitmapButton2(RowNo, BitmapID, FieldId, Width, Just, Separator); };

    FxdEdtFld* SetCheckBox(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
      {return pView->SetCheckBox(RowNo, InitStr, FieldId, Width, Just, Separator, Editable); };

    FxdEdtFld* SetCheckBoxBtn(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
      {return pView->SetCheckBoxBtn(RowNo, InitStr, FieldId, Width, Just, Separator, Editable); };

    FxdEdtFld* SetTreeNd(int RowNo, flag fPlus, int FieldId, int Width, UCHAR Just, pchar Separator)
      {return pView->SetTreeNd(RowNo, fPlus, FieldId, Width, Just, Separator); };
    void SetFixedPosition(int Fixed=True) 
      { pView->SetFixedPosition(Fixed); };
    void SetTag(pchar TagStr, char * CnvTxt=NULL)
      {pView->SetTag(TagStr, CnvTxt); };
    void SetBigStrLength(int Len)
      {pView->SetBigStrLength(Len); };
    void SetMarkerFlags(int nMarker)
      {pView->SetMarkerFlags(nMarker); };
    void SetForce1Page(int Force=True) 
      { pView->SetForce1Page(Force); };


    void FldHasFixedStrValue(int i, pchar pStrVal) { pView->FldHasFixedStrValue(i, pStrVal);};
    void FldHasNANStrValue(int i, pchar pStrVal) { pView->FldHasNANStrValue(i, pStrVal);};
    
    virtual void PutDataStart() {};
    virtual void PutDataDone()  {};
    virtual void GetDataStart() {};
    virtual void GetDataDone()  {};

    virtual void StartBuild() {};
    virtual void Build() {};
    virtual void BuildDone() {};
    virtual void ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols) {};
    virtual void PageChanged(int NewPageNo, flag Initial) {};
    virtual void Load(FxdEdtInfo &EI, Strng & Str) {};
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str) { return 0; };
    virtual long Parse(FxdEdtInfo &EI, Strng & Str) { return 0; };
//    virtual void Load(FxdEdtInfo &EI, pchar Str, int Len) {};
//    virtual int ButtonPushed(FxdEdtInfo &EI, pchar Str, int Len) { return 0; };
//    virtual int Parse(FxdEdtInfo &EI, pchar Str) { return 0; };
//    virtual flag Changed(FxdEdtInfo &EI) {return False;};
//    virtual void ClearChanged(FxdEdtInfo &EI) {};
    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn) {};
    virtual void OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn) {};
    virtual flag DoLButtonDown(UINT nFlags, CPoint point){return False;};
    virtual flag DoLButtonUp(UINT nFlags, CPoint point){return False;};
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point){return False;};
    virtual flag DoRButtonDown(UINT nFlags, CPoint point){return False;};
    virtual flag DoRButtonUp(UINT nFlags, CPoint point){return False;};
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point){return False;};
    virtual flag DoMouseMove(UINT nFlags, CPoint point){return False;};
    virtual flag DoKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){return False;};

    virtual flag DoAccCnv(UINT Id){return false;};
    virtual flag DoAccFmt(UINT Id){return false;};
    virtual flag DoAccRptTagLists() { return pView->DoAccRptTagLists(WrkIB); };
  };

// --------------------------------------------------------------------------

inline CWnd&    Wnd(pFxdEdtBook pBook) { return *(pBook->pWnd); };

// --------------------------------------------------------------------------

inline int FxdEdtFld::Xs(rGDIBlk GB) {return (GB.LuMargin.cx+GB.LuBorder.cx+(Col1-(fFixedPos ? 0 : GB.ChScroll.cx))*GB.ColWdt());};
inline int FxdEdtFld::Xe(rGDIBlk GB) {return (GB.LuMargin.cx+GB.LuBorder.cx+(ColN-(fFixedPos ? 0 : GB.ChScroll.cx))*GB.ColWdt()-1);};
inline int FxdEdtFld::Ys(rGDIBlk GB) {return (GB.LuMargin.cy+GB.LuBorder.cy+(pRow->RowNo-(fFixedPos ? 0 : GB.ChScroll.cy))*GB.RowHgt());};
inline int FxdEdtFld::Ye(rGDIBlk GB) {return (GB.LuMargin.cy+GB.LuBorder.cy+(pRow->RowNo+1-(fFixedPos ? 0 : GB.ChScroll.cy))*GB.RowHgt()-1);};

//---------------------------------------------------------------------------

inline int FxdEdtBook::StartPage(pchar PgName_, bool LoadByCol)
  {
  ASSERT_RDB(Pages<MaxFixedPages, "Too many pages!");
  if (Pages<MaxFixedPages)
    {
    Pg[Pages]=new FxdEdtPage((FxdEdtView*)this, PgName_, Pages, LoadByCol);
    CPg=Pg[Pages];
    CPgNo=Pages;
    //FixedRows=0;
    ++Pages;

    return BuildStdPageHeader();
    }
  return 0;
  }

//---------------------------------------------------------------------------

inline int FxdEdtBook::StartBlk(int Len, int Index_, pvoid Ptr_) 
  {
  int PgHdLen=0;
  if (Pages==0)
    PgHdLen=StartPage("", false);

  ++CPg->Blks;
  ++CPg->CurBlkId;
  ASSERT(CPg->CurBlkId<MaxBlkIds);
  rFxdEdtBlk bp=CPg->Blk[CPg->CurBlkId-1];
  rFxdEdtBlk b=CPg->Blk[CPg->CurBlkId];
  b.Len=Len+PgHdLen;
  b.Row1=bp.Row1+bp.Len;
  b.Index=Index_+PgHdLen;
  b.Ptr=Ptr_;
  return PgHdLen;
  };

//---------------------------------------------------------------------------

inline flag FxdEdtBook::CurrentBlk(FxdEdtInfo &EI) 
  {
  EI.Pg->CurBlkId++; 
  if (EI.Pg->CurBlkId==EI.BlkId)
    {
    int cb=EI.BlkId;
    EI.BlkId=cb;
    EI.BlkRowNo=EI.PgRowNo-EI.Pg->Blk[cb].Row1;
    EI.Index=EI.Pg->Blk[cb].Index;
    EI.Ptr=EI.Pg->Blk[cb].Ptr;
    return True;
    }
  else
    return False;
  };

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetFld(byte FieldType, int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  return pPrevFld=CPg->SetFld(FieldType, RowNo, InitStr, FieldId, Width, Just, Separator, Editable);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetDesc(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  return SetFld(DESC_FIELDS, RowNo, InitStr, FieldId, Width, Just, Separator, Editable);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetSpace(int RowNo, int Width)
  {
  return SetDesc(RowNo, "", -1, Width, 0, "", FALSE);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetParm(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  return SetFld(PARM_FIELDS, RowNo, InitStr, FieldId, Width, Just, Separator, Editable);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetData(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  return SetFld(DATA_FIELDS, RowNo, InitStr, FieldId, Width, Just, Separator, Editable);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetDParm(int RowNo, pchar DStr, int DWidth, pchar InitStr, int FieldId_, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  SetDesc(RowNo, DStr, -FieldId_, DWidth, 3);
  return SetParm(RowNo, InitStr, FieldId_, Width, Just, Separator, Editable);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetDData(int RowNo, pchar DStr, int DWidth, pchar InitStr, int FieldId_, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  SetDesc(RowNo, DStr, -FieldId_, DWidth, 3);
  return SetData(RowNo, InitStr, FieldId_, Width, Just, Separator, Editable);
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetButton(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator)
  {
  Just=1;
  FxdEdtFld* f=SetFld(PARM_FIELDS, RowNo, InitStr, FieldId, Width, Just, Separator, False);
  if (f)
    {
    f->IsButton=1;
    f->iCtrlIndex=nCtrls;
    f->m_iCtrlType=FXWC_BTN;
    AppendCtrlList(f);
    //nCtrls++;
    }
  return f;
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetBitmapButton1(int RowNo, int BitmapID, int FieldId, int Width, UCHAR Just, pchar Separator)
  {
  Just=1;
  FxdEdtFld* f=SetFld(PARM_FIELDS, RowNo, " ", FieldId, Width, Just, Separator, False);
  if (f)
    {
    f->IsButton=1;
    f->iCtrlIndex=nCtrls;
    f->m_iCtrlType=FXWC_BM1BTN;
    f->BigStrLength=BitmapID;
    AppendCtrlList(f);
    //nCtrls++;
    }
  return f;
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetBitmapButton2(int RowNo, int BitmapID, int FieldId, int Width, UCHAR Just, pchar Separator)
  {
  Just=1;
  FxdEdtFld* f=SetFld(PARM_FIELDS, RowNo, " ", FieldId, Width, Just, Separator, False);
  if (f)
    {
    f->IsButton=1;
    f->iCtrlIndex=nCtrls;
    f->m_iCtrlType=FXWC_BM2BTN;
    f->BigStrLength=BitmapID;
    AppendCtrlList(f);
    //nCtrls++;
    }
  return f;
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetCheckBox(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  Just=1;
  FxdEdtFld* f=SetFld(PARM_FIELDS, RowNo, InitStr, FieldId, Width, Just, Separator, Editable);
  if (f)
    {
    f->IsButton=1;
    f->iCtrlIndex=nCtrls;
    f->m_iCtrlType=FXWC_CHECK;
    AppendCtrlList(f);
    //nCtrls++;
    }
  return f;
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetCheckBoxBtn(int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
  Just=1;
  FxdEdtFld* f=SetFld(PARM_FIELDS, RowNo, InitStr, FieldId, Width, Just, Separator, Editable);
  if (f)
    {
    f->IsButton=1;
    f->iCtrlIndex=nCtrls;
    f->m_iCtrlType=FXWC_CHECKBTN;
    AppendCtrlList(f);
    //nCtrls++;
    }
  return f;
  }

//---------------------------------------------------------------------------

inline FxdEdtFld* FxdEdtBook::SetTreeNd(int RowNo, flag fPlus, int FieldId, int Width, UCHAR Just, pchar Separator)
  {
  Just=1;
  FxdEdtFld* f=SetFld(PARM_FIELDS, RowNo, "", FieldId, Width, Just, Separator, False);
  if (f)
    {
    f->IsButton=1;
    f->iCtrlIndex=nCtrls;
    f->m_iCtrlType=FXWC_TREEND;
    AppendCtrlList(f);
    pCtrl[f->iCtrlIndex].fPlus=fPlus;
    //nCtrls++;
    }
  return f;
  }

//---------------------------------------------------------------------------

inline void FxdEdtBook::SetTag(char * TagStr, char * CnvTxt)
  {
  if (pPrevFld)
    pPrevFld->SetTag(TagStr, CnvTxt);
  }

//---------------------------------------------------------------------------

inline void FxdEdtBook::SetBigStrLength(int Len)
  {
  if (pPrevFld)
    pPrevFld->SetBigStrLength(Len);
  }

//---------------------------------------------------------------------------

inline void FxdEdtBook::SetMarkerFlags(UCHAR nMarks)
  {
  if (pPrevFld)
    pPrevFld->SetMarks(nMarks);
  }

//---------------------------------------------------------------------------

inline void FxdEdtBook::SetFixedPosition(int Fixed)
  {
  fFixedPos=Fixed;
  }

//---------------------------------------------------------------------------

inline void FxdEdtBook::SetForce1Page(int Force)
  {
  fForce1Page=Force;
  }

// --------------------------------------------------------------------------

DllImportExport flag MDIActivateThis(CWnd* This, int Deep=2);

// ==========================================================================

#undef DllImportExport

#endif
