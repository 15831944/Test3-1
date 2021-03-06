//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TAGVSPLT_H
#define __TAGVSPLT_H

class CTagVwSplit : public CMDIChildWnd
  {
  DECLARE_DYNCREATE(CTagVwSplit)
  protected:
    CTagVwSplit();      // protected constructor used by dynamic creation
    CSplitterWnd  m_wndSplitter;
    pCTagVwDoc    pDoc;
    bool          m_CreateDone;
  public:
    virtual ~CTagVwSplit();
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    //pCTagVwDoc Doc() { return (CTagVwDoc*)GetDocument(); };
    pCTagVwDoc Doc() { return pDoc; };
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    static  void DoBehaviourChange();

    //{{AFX_MSG(CTagVwSplit)
    afx_msg void OnFilePrint();
    afx_msg void OnFilePrintMultiple();
    //afx_msg void OnFilePrintPreview();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  protected:
    int m_PrevClientX;
    int m_PrevClientY;
  };

//---------------------------------------------------------------------------

class CTagVwSplitC : public CTagVwSplit
  {
  DECLARE_DYNCREATE(CTagVwSplitC)
  protected:
    CTagVwSplitC();      // protected constructor used by dynamic creation
    //CSplitterWnd  m_wndSplitter;
    pCTagVwDocC   pDocC;
  public:
    virtual ~CTagVwSplitC();
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
//    //pCTagVwDoc Doc() { return (CTagVwDoc*)GetDocument(); };
//    pCTagVwDoc Doc() { return pDoc; };
//    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    //{{AFX_MSG(CTagVwSplitC)
//    afx_msg void OnFilePrint();
//    //afx_msg void OnFilePrintPreview();
//    afx_msg void OnSetFocus(CWnd* pOldWnd);
//    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------
#endif
