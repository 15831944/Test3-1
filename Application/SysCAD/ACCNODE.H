//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __ACCNODE_H
#define __ACCNODE_H

#ifndef __CMDMENU_H
  #include "cmdmenu.h"
#endif

#ifndef __FIXEDIT_H
  #include "fixedit.h"
#endif

#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif

#ifndef __SCDTEMPL_H
  #include "scdtempl.h"
#endif
#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif
#ifndef __MDLVALUE_H
  #include "mdlvalue.h"
#endif

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adoEOF")

//===========================================================================

//const int MaxAccNdFlds=8192;//increased from 5000 kga 14/7/04 for Iluka MultiStore
const int MaxAccNdFlds=12500;//increased again kga 10/8/04 for Iluka MultiStore
const int MaxAccNdObjs=96;//increased from 64 kga 7/3/04

//===========================================================================

_FWDDEF(AccNodeWnd);
_FWDDEF(AccNdFld);
_FWDDEF(AccNdData);

//===========================================================================
//
//
//
//===========================================================================

class AccDataBlk
  {
  public:
    Strng sTag;
    Strng sRefTag;
    Strng sBlkName;
    
    CXM_ObjectData * pData;

    AccDataBlk(char * Tag, char *RefTag, char * BlkName, CXM_ObjectData * Data);
    ~AccDataBlk();

  };

//===========================================================================
//
//
//
//===========================================================================

class AccNdData
  {
  public :
    char           iType;
    DDEF_Flags     iFlags;
    Strng          sSymOrTag; // 
    Strng          sTagOnly;  // 
    Strng          sFullTag;  //
    Strng          sRefTag;
    Strng          sClsTag;
    Strng          sValue;
    //Strng          sNANStr;
    Strng          sTagComment;
    Strng          sDesc;
    int            iStrtFld;
    int            iEndFld;
    int            iLvl;
    double         dVal;
    Strng          sIndividualCnv;


    CnvAttribute    Cnv;
    FmtAttribute    Fmt;
    int             iViewIndex;
    
    signed char     iObjNo;
                    
    byte            fIsStrng:1, 
                    fIsParam:1, 
                    fKeepFldName:1,
                    fIsModified:1, 
                    fIsData:1, 
                    fIsStruct:1,
                    fHasNANOK:1,
                    fHasNANStr:1,
#if WITHSPECSHEETS
                    fIsSpecSheet:1,
#endif
                    ObjOn:1, 
                    fPageOn:1,
                    fIsEdtObj:1,
                    fIndexedStr:1,
                    fHasComment:1,
                    fIsStructTreeNd:1, 
                    fIsTreeNd:1,
                    fMultiStrng:1,
                    fUsed:1,
                    fHasCheck:1,
                    fHasButton:1,
                    fHasSetOnChange:1,
                    fRecorded:1,
                    fArchived:1,
                    m_iChildLvl:2,
                    m_fNxtIsChild:1,
                    m_fIsRowStart:1,
                    m_fIsRowsEnd:1,
                    m_IsLastDataCol:1;
    int             m_iMatrixCol;
    UCHAR           m_Markers;    
    Strng_List      ValLst;

    ObjectAttribute *pObjAttr;

    void            Init();
    void            Term();
    void            FormatFloat();
    void            AdjTextValue(char * NewCnv, bool SaveOld);
  };

//===========================================================================
//
//
//
//===========================================================================

class AccNdFld
  {
  public :
    AccNdData*     pData;
    int            iData;
    char           iType;
    Strng          sTagText;
    short          iLvl;
    
    flag           fIsData:1, fIsTreeNd:1, fIsTreeVis:1, fUseComment:1, fPadDash:1;
    short          iUseComment;

    FxdEdtFld*     ValFld;

    flag           HasData() { return pData!=NULL; };
    AccNdData &    operator ()() { return *pData; };
    AccNdData &    D() { return *pData; };

    void           Init();
    void           Term();
  };

//===========================================================================

const int MaxRptLvl=5;
class RptInfo
  {
  public:
    Strng_List  Lst;
    int         Reading;
    int         MaxCount;
    Strng_List  PriorMatches;
    Strng       WorkingMatch;
    int         DataFld1;
    int         DataLvl;

  };

class SSLoadData 
  {
  public:
    int            PageNo;
    flag           Loaded;
    flag           DataLine;
    flag           fDoingBlk;
    int            BlkDataFld;
    int            RptLvl;
    RptInfo        Info[MaxRptLvl];


    SSLoadData ()
      {
      PageNo=0;
      Loaded=DataLine=False;
      RptLvl=-1;
      fDoingBlk=0;
      BlkDataFld=-1;
      for (int i=0; i<MaxRptLvl; i++)
        {
        Info[i].Lst.Clear();
        Info[i].Reading=False;
        Info[i].MaxCount=0;
        };
      };
    flag           RptOK() { return RptLvl>=0; };
    RptInfo    &   Rpt() { return Info[RptLvl]; };
    RptInfo    &   operator[](int i) { return Info[i]; };

  };

class AccNodeWnd : public FxdEdtView, public CExecObj
  {
  friend class CAccCopyBlk;
  friend class CAccessWnds;
    DECLARE_DYNCREATE(AccNodeWnd)
  protected:
    AccNodeWnd();      // protected constructor used by dynamic creation

  // Operations
  public:
    virtual void   Initialise();
    void           CloseWnd();
    void           ClearFlds(flag SaveCnvs);
    flag           CloseAccess();
    void           AccessNode(LPCTSTR pTag, bool AllowDeferred=true);
    void           DoDeferredAccess();
    flag           LoadAccessData();//flag ParmsOnly);
    flag           CloseAccessData(flag SaveIt, flag CancelIt, flag Disconnect);
    void           LoadNdHist();
    void           SaveNdHist();

    flag           RefreshData(flag SaveIt);
    flag           SaveData();
    flag           LoadData();
    flag           AnyFldsOrDataChanged();

    void           ShowDefWndTitle();
    char*          CurTag() { return NdTag(); };

  protected:
    void           MeasureFields();
    void           ConnectFields();
    void           ReFreshAttrs();
    void           SaveCurrentPageInfo();
    void           RestoreCurrentPageInfo();
    void           SetCurrentPage();
  public:
#if WITHSPECSHEETS
    flag           SSFindNext(FILE * hr, FILE * hw, flag WriteBuff, char * Buff, int BuffSize);
    flag           SSFind(FILE * hr, FILE * hw, const char * Name, flag WriteBuff, char * Buff, int BuffSize, flag &ReadOnly);
    flag           SSNames(Strng_List & Names, CByteArray & ReadOnlyList);
    flag           SSDelete(const char * Name);
    flag           SSLoad(const char * Name);
    flag           SSLoadLine(const char * LineData, SSLoadData & LD);
    flag           SSSave(const char * Name);
    flag           DeleteSpecSheet(const char * Name);
    flag           LoadSpecSheet(const char * Name);
    flag           SaveSpecSheet(const char * Name);
#endif


    // Implementation
  protected:
    virtual ~AccNodeWnd();
//    virtual void OnDraw(CDC* pDC);    // overridden to draw this view
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint);
    virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );

    virtual void StartBuild();
    virtual int  BuildStdPageHeader();
    virtual void Build(int NRows=0, int NCols=0, int ColIndent=0, pvoid BuildData=NULL);
    virtual void BuildDone();
    virtual void ChangeLayout(int TotalRows, int TotalCols);
    virtual void PageChanged(int NewPageNo, flag Initial);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual void OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual void OnDrawLine(rGDIBlk GB, int PgNo, int RowNo);

    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);
    virtual flag DoKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual flag DoCutVal(pFxdEdtFld pFld, char *buff, int buffsize);
    virtual flag DoCopyVal(pFxdEdtFld pFld, char *buff, int buffsize);
    virtual flag DoPaste(pFxdEdtFld pFld, char *buff);

    // Generated message map functions
  protected:
    //{{AFX_MSG(AccNodeWnd)
    afx_msg void OnDestroy();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //}}AFX_MSG
    afx_msg void OnAccCnv(UINT id);
    afx_msg void OnAccFmt(UINT id);
    afx_msg void OnAccShow(UINT id);
    afx_msg void OnAccView(UINT Id);
    afx_msg void OnAccOptions(UINT id);
    afx_msg void OnAccCopyBlk(UINT id);
    afx_msg void OnAccPasteBlk(UINT id);
    afx_msg void OnAccCutTag();
    afx_msg void OnAccCutVal();
    afx_msg void OnAccCopyTag();
    afx_msg void OnAccCopyRHSTag();
    afx_msg void OnAccCopyVal();
    afx_msg void OnAccPaste();
    afx_msg void OnAccEmptyPreset(UINT Id);
    afx_msg void OnAccMenu(UINT id);
    afx_msg LONG OnAccRefreshSave(UINT wParam, LONG lParam);
    afx_msg LONG OnAccRefreshCancel(UINT wParam, LONG lParam);
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

    DECLARE_MESSAGE_MAP()

    //virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

    // CExecObj Overrides
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag   EO_Start(CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
    virtual flag   EO_Starting(flag fBeginStarting);
    virtual flag   EO_Stopping(flag fBeginStopping);
    virtual void   EO_GlblResultValidity(flag IsValid, flag IsAlwaysValid);
    virtual flag   EO_BeginPause(CXM_TimeControl &CB);
    virtual flag   EO_EndPause(CXM_TimeControl &CB);
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_DeleteTag(pchar pDelTag);

    flag BuildDataMessage(long DataIndex, Strng & Tag, Strng & RefTag, Strng & BlkName, CXM_ObjectData &ObjData);
    void SendDataMessage(CXM_ObjectTag &ObjTag, CXM_ObjectData &ObjData);

    void ShowTT();      
    void HideTT();      

  public:
    static FmtAttribute GlblFmt;
  // Attributes
  protected:
    Strng          NdTag;
    Strng          WndTitle;
    CXM_Route       ObjRoute;

    int            NObjs;
    pTaggedObject  ObjInst[MaxAccNdObjs];
    pFxdEdtBookRef ObjEdit[MaxAccNdObjs];
    flag           ObjChgd[MaxAccNdObjs];
    short          ObjOnPage[MaxFixedPages];
    
    Strng          ObjClassId;

    int            NData;
    AccNdData      Data[MaxAccNdFlds];
    int            NFlds;
    AccNdFld       Flds[MaxAccNdFlds];
    
    int            NamW, CnvW, ValW, MatW, FldW;
    int            m_iPrevTTRow, m_iPrevTTCol;
    
#if WITHSPECSHEETS
    Strng          sSpecSheet;
    int            iSpecSheet;
#endif

    //CXM_ObjectData  OldObjData;
    flag           fLastSaveInvalid:1,
                   fAllFldsVis:1,
                   fKeepNdHist:1,
                   fGlobalFmtMenu:1;

    int            RqdPgLen[MaxFixedPages];
    int            RBActiveFld;
    CPoint         RBPoint;
    //Strng          sLastSelected;
    //Strng          sLastSelFullTag;
    CStringArray   sLastSelected;
    CStringArray   sLastSelFullTag;
    AccNdData*     pLastAccNdData;

    CSArray        NdHistList;
    int            iNdHistPos;

    CBitmap        ViewVarBitmap;
    CBitmap        HiddenBitmap;
 
    CCriticalSection m_DeferredLock;
    Strng          m_DeferredTag;

    // ToolTip
    CString        m_sTTCls;
    CString        m_sTTTextHead;
    CString        m_sTTTextDisp;
    HWND           m_hTTWnd;                 // handle to the ToolTip control
    bool           m_bTTVisible;

    CMDIChildWnd * m_pMyFrm;
};

//===========================================================================

class CAccDoc : public CDocument
  {
  protected:
    DECLARE_DYNCREATE(CAccDoc)
  public:
    CAccDoc();
    //{{AFX_VIRTUAL(CAccDoc)
    protected:
    virtual BOOL OnNewDocument();
    //}}AFX_VIRTUAL
  public:
    virtual ~CAccDoc();
    #ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
    #endif
  protected:
    //{{AFX_MSG(CAccDoc)
    afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

const long NAccessWnds=2;
class CAccessWnds 
  {
  public:
    long           m_nWnds;
    long           m_iActive;
    AccNodeWnd *   m_pAccWnds[NAccessWnds];
    CMDIChildWnd * m_pAccFrames[NAccessWnds];

    CAccessWnds(); 
    ~CAccessWnds(); 

    AccNodeWnd *   Wnd(long i) { return m_pAccWnds[i]; };
    CMDIChildWnd * Frame(long i) { return m_pAccFrames[i]; };

    void           Add(AccNodeWnd *pAccWnd, CMDIChildWnd *pAccFrame);
    void           MarkAsActive(AccNodeWnd *pAccWnd);

    void           AccessNode(long Which, LPCTSTR pTag, bool AllowDeferred=true);
    void           DoDeferredAccess(long Which);
    void           CloseAccessData(long Which, flag SaveIt, flag CancelIt, flag Disconnect);
    void           CloseWnd(long Which);
    void           PostMessage(long Which, UINT Msg, WPARAM W=0, LPARAM L=0);
    void           LoadNdHist(long Which);
    void           SaveNdHist(long Which);

    void           RefreshData(long Which, flag SaveIt);
    
    char*          CurTag(long Which);

    ADODB::_RecordsetPtr m_RS1;
    _bstr_t       m_bsConnect;
    _bstr_t       m_bsSource;

    bool          InitHelp(LPCTSTR Path);
    void          TermHelp();
    CString       GetHelp(LPCTSTR Tag);

  };

//===========================================================================

extern CAccessWnds gs_AccessWnds;

//===========================================================================
#endif
