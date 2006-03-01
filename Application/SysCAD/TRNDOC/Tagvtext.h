//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TAGVDOC_H
  #include "tagvdoc.h"
#endif

_FWDDEF(CTagVwText)
class CTagVwText : public FxdEdtView
  {
  DECLARE_DYNCREATE(CTagVwText)
  protected:
    CTagVwText();     // protected constructor used by dynamic creation
    virtual ~CTagVwText();
    virtual void StartBuild();
    virtual void Build(int NRows=0, int NCols=0, int ColIndent=0, pvoid BuildData=NULL);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual flag Changed(FxdEdtInfo &EI);
    virtual void ClearChanged(FxdEdtInfo &EI);
    virtual void OnDrawBack(rGDIBlk GB, int PgNo, CRgn &ClipRgn);
    virtual void OnDrawLine(rGDIBlk GB, int PgNo, int RowNo);
    virtual void OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint);
    virtual void OnInitialUpdate();
    virtual int SlotIndex(FxdEdtInfo &EI);
    virtual int SlotIndex(pFxdEdtFld pFld);//nt Row, int Id);
    virtual int SlotIndex(int Row, int Id);
    virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
    inline pCTagVwDoc Doc() { return (pCTagVwDoc)(GetDocument()); };
    inline pCTagVwSlot DataSlot(int RowId) { return (RowId>=0 && RowId<Doc()->NoSlots() ? &Doc()->DS[RowId] : NULL); };
    inline int NoSlots() { return Doc()->NoSlots(); };
    int CalcRowRange(int &R1, int &R2);

    flag DoOnRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

  public:
    void DoOnFilePrint(BOOL UseColors);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    void StartFldUpdate();
    void UpdateFld(int i, pchar p);
    void EndFldUpdate();
    void SelectTrendTag(int i);

    int &TagDspWdtFn() { return Doc()->ColumnWidths[0]; };
    int &MeasDspWdtFn() { return Doc()->ColumnWidths[1]; };
    int &MinDspWdtFn() { return Doc()->ColumnWidths[2]; };
    int &MaxDspWdtFn() { return Doc()->ColumnWidths[3]; };
    static HWND CurrentTrend() { return m_hCurrentTrend; };

  protected:
    int              NUsed;
    int              iMarginWidth;

    CArray <pFxdEdtFld, pFxdEdtFld> ValFlds;
    CArray <pFxdEdtFld, pFxdEdtFld> MinFlds;
    CArray <pFxdEdtFld, pFxdEdtFld> MaxFlds;
    int              nValFlds;
    
    CClientDC*       pUpDC;
    CDCResChk*       pResChk;
    pCTagVwSlot      pMenuSlt; //current slot for the popup menu
    int              iMenuLine; //current trend line for the popup menu
    CPoint           MenuPt; //current position of popup menu
    CStringArray     m_sTag4Access; // 
    CBitmap          DriverBitmap;
    CBitmap          ArchiveBitmap;
    CBitmap          FindBitmap;
    flag             bUseColorPrint:1;

    UINT             m_uTimerId, m_uTimerCnt;

    int              m_iDragX;
    int              m_iDragI;
    static HWND      m_hCurrentTrend;
    int              m_AddTagSlt;

    //{{AFX_MSG(CTagVwText)
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateByMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnAddTag2Trend(WPARAM wParam, LPARAM lParam);
    afx_msg void OnMenu(UINT id);
    afx_msg void OnCnv(UINT id);
    afx_msg void OnFormat(UINT id);
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

_FWDDEF(CTagVwTextC)
class CTagVwTextC : public CTagVwText
  {
  DECLARE_DYNCREATE(CTagVwTextC)
  protected:
    CTagVwTextC();     // protected constructor used by dynamic creation
    virtual ~CTagVwTextC();
//    virtual void StartBuild();
    virtual void Build(int NRows=0, int NCols=0, int ColIndent=0, pvoid BuildData=NULL);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual flag Changed(FxdEdtInfo &EI);
    virtual void ClearChanged(FxdEdtInfo &EI);
//    virtual void OnDrawBack(rGDIBlk GB, int PgNo, CRgn &ClipRgn);
    virtual void OnDrawLine(rGDIBlk GB, int PgNo, int RowNo);
//    virtual void OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint);
    virtual void OnInitialUpdate();
    virtual int SlotIndex(FxdEdtInfo &EI);
    virtual int SlotIndex(pFxdEdtFld pFld);
    virtual int SlotIndex(int Row, int Id);
//    virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
    inline pCTagVwDocC Doc() { return (pCTagVwDocC)(GetDocument()); };
//    inline pCTagVwSlot DataSlot(int RowId) { return (RowId>=0 && RowId<Doc()->NSlots ? &Doc()->DS[RowId] : NULL); };

//    virtual flag DoMouseMove(UINT nFlags, CPoint point);

//    int CalcRowRange(int &R1, int &R2);

  public:
//    void DoOnFilePrint(BOOL UseColors);
//    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
//    void StartFldUpdate();
//    void UpdateFld(int i, pchar p);
//    void EndFldUpdate();
//    void SelectTrendTag(int i);

  protected:
    //int              m_NLoopRows;
    int              m_TrndRow1;
    int              m_LoopRow1;
    int              m_iMenuSlot;
    CBitmap          CtrlBitmap;
    CBitmap          MaskBitmap;

    int     LoopSlotOne() { return Doc()->LoopSlotOne(); };
    int     LoopSlotCount() { return Doc()->LoopSlotCount(); };
    
    flag DoOnRButtonUp(UINT nFlags, CPoint point);
    void SelectSlotForView(int iSltNo);
    void ToggleTuneRqst(int iSltNo);
    void ToggleAutoManual(int iSltNo);

    //{{AFX_MSG(CTagVwTextC)
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
//	  afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
//    afx_msg LRESULT OnUpdateByMsg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnMenuTune(UINT id);
//    afx_msg void OnCnv(UINT id);
//    afx_msg void OnFormat(UINT id);
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------
