//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GRFWND_H
#define  __GRFWND_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#ifndef __GRF3DRW_H
#include "grf3drw.h"
#endif

//#ifdef __GRFWND_CPP
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif

extern CString MakeValidBlockName(LPTSTR BlockName);
//extern Strng MakeValidSymbolName(LPTSTR SymbolName);

//===========================================================================

enum DynamicWnd_Mode {DW_Edit, DW_Access, DW_Zoom, DW_Graphics, DW_Report};

//---------------------------------------------------------------------------

class CGrfSubs
  {
  public:
    Strng sTag;
    pDynamicSegment pDS;  //first Dynamic Segment using this tag

    CGrfSubs(char* pTag);
  };

//---------------------------------------------------------------------------

_FWDDEF(DispInfo)
class DispInfo
  {
  public:
    pDynamicSegment pDS;
    int iFindCnt;
    Strng sTag;
    Strng sVar;
    double dRqdValue;
    flag bDrawRqd;
    //double          Min, Max;
    
    DispInfo(pDynamicSegment p_) { pDS = p_; sTag = pDS->GetTag(); sVar = pDS->GetVar(); iFindCnt = 0; bDrawRqd=false; dRqdValue=dNAN;/*Min=0.0; Max=1.0;*/ };
  };

//---------------------------------------------------------------------------

_FWDDEF(CGrfWnd)

//---------------------------------------------------------------------------

class DllImportExport CGrfWnd : public CView, public CExecObj
  {
  DECLARE_DYNCREATE(CGrfWnd)

  HCURSOR BaseCursor, OptionCursor; 
  dword   PrevNumStatusColourChgs;
//  public:

  protected:
    CGrfWnd();
  public:
    virtual ~CGrfWnd();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual BOOL Terminate();
    virtual BOOL Open(char* FlwSheetName_);
    virtual BOOL Close();
    virtual BOOL SetCursor(UINT ReqdCursor=0);

    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
    bool         UpdateForPaint(bool ForceUpd, bool ColoursChgd);
    virtual void OnDraw(CDC* pDC);    // overridden to draw this view
    virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );

    Grf3D_Display Dsp;
    CDocument*    pDoc;
    flag          bDonePaint;
    flag          bPretendPrinting;
    
  protected:  
    //{{AFX_MSG(CGrfWnd)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnFilePrint();
  	//}}AFX_MSG
    afx_msg LRESULT OnDoGrfCmd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDoSetFocus(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDoDspUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDoWriteSubs(WPARAM wParam, LPARAM lParam);

//    afx_msg void OnUnitIOMenu(UINT id);
    DECLARE_MESSAGE_MAP();  

  public:
    //XInfoList      XData;
    CArray <pDispInfo, pDispInfo> m_XData;
    CCriticalSection  m_XDataLock;

    void           DelDynamicSegment(pDynamicSegment pDS);
    void           SendCmdToAll(char* pCmdStr);
    void           CopyBMPtoClipBoard(int Format);

    virtual flag   EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info);
    virtual int    EO_QueryChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    EO_ChangeTag(pchar pOldTag, pchar pNewTag);

    virtual flag   EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd);
    virtual flag   EO_Start(const CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_TagsNotAvail(CXMsgLst &XM);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(const CXM_TimeControl &CB);
    virtual flag   EO_Stopping(flag fBeginStopping);
    virtual void   EO_DisplayUpdate(flag ForceUpd, flag ColoursChgd, const CXM_TimeControl &CB);
    
   };

//---------------------------------------------------------------------------

class DllImportExport CGrfFrameWnd : public CMDIChildWnd
  {
  DECLARE_DYNCREATE(CGrfFrameWnd)
  protected:
    CGrfFrameWnd();           // protected constructor used by dynamic creation
  public:
    //{{AFX_VIRTUAL(CGrfFrameWnd)
    //}}AFX_VIRTUAL
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  protected:
    virtual ~CGrfFrameWnd();
    //{{AFX_MSG(CGrfFrameWnd)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

#undef DllImportExport
#endif
