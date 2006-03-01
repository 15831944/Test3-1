#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CBulkTagChange dialog

class CBulkTagChange; // forward
class CBTCItem; // forward
class CBTCClass; // forward
class CBTCPage; // forward
class CBTCRepSeq; // forward

class CBTCRepSeq
  {
  public:
    CBTCRepSeq()
      {
      m_AdjustCnt=0;
      m_ReplaceUndoCnt=0;
      m_nDuplicateCount=0;
      }
    CBTCRepSeq(const CBTCRepSeq & Other)
      {
      *this=Other;
      }
    CBTCRepSeq & operator=(const CBTCRepSeq & Other)
      {
      m_sReplacement    = Other.m_sReplacement;
      m_sStatus         = Other.m_sStatus;
      m_nDuplicateCount = Other.m_nDuplicateCount;
      m_AdjustCnt       = Other.m_AdjustCnt;
      m_ReplaceUndoCnt      = Other.m_ReplaceUndoCnt;
      return *this;
      }

  public:
    CString           m_sReplacement;
    CString           m_sStatus;
    BOOL              m_nDuplicateCount;
    int               m_AdjustCnt;
    int               m_ReplaceUndoCnt;
  };

class CBTCItem : public CBTCRepSeq
  {
  friend class CBulkTagChange;

  enum eDoItTerms {/*eSRPage,*/ eSRCheck/*, eSRClass*/, eSRMarked, eSRSelected};

  public:
    CBTCItem(CBulkTagChange * Dlg, LPCTSTR OldTag, CBTCClass * ClassId);

    LPCTSTR     GetString (int j);
    void        PutString (int j, LPCTSTR Str);


    BOOL        PageChecked();
    BOOL        SetReqd(bool IgnoreCheck=false);
    void        ChangeSetReqd(eDoItTerms Term, BOOL On);
    void        SaveSetReqd();
    void        TestSetReqd();

    //BOOL        DoIt(bool IgnoreCheck=false);
    BOOL        ShowIt();
    void        Start();
    void        TagChanged();
    void        Replace(LPCTSTR New);
    void        Undo();
    LPCTSTR     StatusString();

  public:
    CBulkTagChange  & m_Dlg;
    CString           m_sCurrent;
    CString           m_sCurrentLwr;
    CString           m_sIntermediate;
    CString           m_sPages;
    CBTCClass       * m_pClassId;
    BOOL              m_Checked; 
    BOOL              m_Selected;
    BOOL              m_Marked;
    BOOL              m_Changed;
    BOOL              m_SetReqd;
    BOOL              m_SetWasReqd;
    BOOL              m_UseInter;
    //BOOL              m_nDuplicateCount;
    int               m_ApplyCnt;
    int               m_DisplayIndex;


    CArray<CBTCPage*, CBTCPage*> m_Pages;
    CList<CBTCRepSeq, CBTCRepSeq&> m_RepSeq;
  };

class CBTCMonitor
  {
  public:
    CBTCMonitor()
      {
      m_Count=0;
      }
    CBTCMonitor(const CBTCMonitor &Other)
      {
      *this=Other;
      }
    CBTCMonitor & operator = (const CBTCMonitor &Other)
      {
      m_Count   = Other.m_Count;
      m_sTag    = Other.m_sTag;
      m_Items.RemoveAll();
      POSITION Pos=Other.m_Items.GetHeadPosition();
      while (Pos)
        {
        CBTCItem*p= Other.m_Items.GetNext(Pos);
        m_Items.AddTail(p);
        }
      return *this;
      }

    int                           m_Count;
    CString                       m_sTag;
    CList <CBTCItem*, CBTCItem*>  m_Items;
  };

class CBTCClass
  {
  friend class CBulkTagChange;

  public:
    CBTCClass(CBulkTagChange * Dlg, LPCTSTR ClassId);

  public:
    CBulkTagChange  & m_Dlg;
    CString           m_sClassId;
    BOOL              m_Checked; 
  };

class CBTCPage
  {
  friend class CBulkTagChange;

  public:
    CBTCPage(CBulkTagChange * Dlg, LPCTSTR PageId);

  public:
    CBulkTagChange  & m_Dlg;
    CString           m_sPageId;
    BOOL              m_Checked; 
  };


class CBTCEdit : public CEdit
  {
  public:
    CBTCEdit(CBulkTagChange * Dlg) : m_Dlg(*Dlg) { m_IsValid=false; };
    virtual ~CBTCEdit() {};
    CBulkTagChange & m_Dlg;

    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnDestroy();
    afx_msg void OnKillFocus( CWnd* pNewWnd );

    DECLARE_MESSAGE_MAP()

  public:
    bool m_IsValid;
  };

class CBTCListCtrl : public CListCtrl
  {
  public:
    CBTCListCtrl(CBulkTagChange * Dlg) : m_Dlg(*Dlg) {};
    virtual ~CBTCListCtrl() {};
    CBulkTagChange & m_Dlg;

    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);
    DECLARE_MESSAGE_MAP()
  };

class CBTCMonitorMap : public CMap <LPCTSTR, LPCTSTR,  CBTCMonitor*, CBTCMonitor*> {};

class CBulkTagChange : public CDialog
  {
  friend class CBTCItem;
  friend class CBTCEdit;
  friend class CBTCListCtrl;

  DECLARE_DYNAMIC(CBulkTagChange)

  public:
    CBulkTagChange(CStringList * pMarkedTags=NULL, CWnd* pParent = NULL);   // standard constructor
    virtual ~CBulkTagChange();

    // Dialog Data
    enum { IDD = IDD_BULKTAGCHANGE };

  protected:
    virtual void  DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL  OnInitDialog();
    virtual void  OnOK();
    virtual void  OnCancel();
    virtual BOOL  OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    virtual BOOL  PreTranslateMessage(MSG *pMsg);


    void          SetFilter();
    void          BuildTags();
    void          RebuildTags();
    void          ClearTags();
    void          LoadTagList();
    void          LoadPageList();
    void          LoadClassList();

    void          PositionControls(int cx, int cy);

    void          UpdateChangeCounts();
    void          SetTagCheckCtrls();
    void          SetPageCheckCtrls();
    void          SetClassCheckCtrls();
    void          ShowSelectedColumns();

    void          StartInplaceEdit(int Item, UINT nChar, UINT nFlags, POINT *Pt);
    void          FinishInplaceEdit();

    int           MonitorAdd(CBTCItem * pItem, BOOL SetRqd);
    int           MonitorRemove(CBTCItem * pItem, BOOL SetRqd);
    void          UpdateDuplicateCounts(CBTCMonitor * p);

    int           SetATag(LPCTSTR OldTag, LPCTSTR NewTag);
    bool          DoChecks();
    void          SaveAllSetReqd();
    void          TestAllSetReqd();

    int           TagIndex(int i) { return m_TagList.GetItemData(i); };
    int           AddApplyLog(char * fmt, ...);

    void          SaveProfile();

    void          SelectAll();
    void          ImportSelected();
    void          ExportSelected();


    DECLARE_MESSAGE_MAP()

  public:
    afx_msg void OnBnClickedOk();


  protected:
    CBTCListCtrl    m_TagList;
    CListCtrl       m_PageList;
    CListCtrl       m_ClassList;
    CButton         m_ShowAllBtn;
    CButton         m_MarkedOnlyBtn;
    CEdit           m_TagFilter;
    CComboBox       m_TagFilterRule;
    CEdit           m_Find;
    CEdit           m_Replace;
    CStatic         m_TagCount;
    CStatic         m_PageCount;
    CStatic         m_ClassCount;
    CButton         m_TagChecks;
    CButton         m_ClassChecks;
    CButton         m_PageChecks;
    CStatic         m_ChangeCount;
    CStatic         m_AdjustCount;
    CStatic         m_ApplyCount;
    CListCtrl       m_ApplyLog;
    CProgressCtrl   m_ApplyProgress;
    CButton         m_ApplyBtn;
    CButton         m_ReplaceBtn;
    CButton         m_UndoReplaceBtn;
    CButton         m_ShowClassesBtn;
    CButton         m_ShowPagesBtn;
    CButton         m_DoneBtn;
    CStatic         m_DuplicateCount;
    CStatic         m_ImportCount;
    CStatic         m_StaticApply;
    CStatic         m_StaticClip;

    BOOL            m_ShowAllTags;
    BOOL            m_MarkedOnly;
    BOOL            m_Loading;
    int             m_ClassColWidth;
    int             m_PageColWidth;

    int             m_nChangeCount;
    int             m_nAdjustCount;
    int             m_nApplyCount;
    int             m_nDuplicateCount; 
    int             m_nImportCount;

    int             m_ReplaceUndoCnt;
    int             m_InPlaceEditItem;

    CBTCEdit        m_InPlaceEdit;

    BOOL            m_bCaseSens;
    IRegExpPtr    * m_pFilterRE;

    CStringList   * m_pMarkedTags;
    CMap <LPCTSTR, LPCTSTR, POSITION, POSITION> m_MarkedTagMap;

    CArray <CBTCItem*, CBTCItem*> m_Tags;
    CMap <LPCTSTR, LPCTSTR,  CBTCItem*, CBTCItem*> m_TagMap;

    CBTCMonitorMap  m_Monitor;

    CArray <CBTCPage*, CBTCPage*> m_Pages;
    CMap <LPCTSTR, LPCTSTR,  CBTCPage*, CBTCPage*> m_PageMap;
    CArray <CBTCClass*, CBTCClass*> m_Classes;
    CMap <LPCTSTR, LPCTSTR,  CBTCClass*, CBTCClass*> m_ClassMap;

    CRect                 m_ClientRctInit;
    CArray<CRect, CRect&> m_CtrlRcts;
    bool                  m_Inited;
    int                   m_SetWhat;
    int                   m_SepPos[3];
    int                   m_SepInit[3];

  public:
    CArray <int, int> m_SortOrder;

  public:
    afx_msg void OnBnClickedUndoreplace();
    afx_msg void OnBnClickedDoreplace();
    afx_msg void OnBnClickedApply();
    afx_msg void OnLvnItemchangedTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchangedClassids(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchangedPages(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedShowalltags();
    afx_msg void OnBnClickedMarkedonly();
    afx_msg void OnBnClickedTagchecks();
    afx_msg void OnBnClickedClasschecks();
    afx_msg void OnBnClickedPagechecks();
    afx_msg void OnEnChangeTagfilter();
    afx_msg void OnCbnSelchangeTagfilterrule();
    afx_msg void OnBnClickedShowclasses();
    afx_msg void OnBnClickedShowpages();
    afx_msg void OnHdnItemclickTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnEndlabeleditTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemActivateTaglist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	  afx_msg void OnSize(UINT nType, int cx, int cy);
	  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    afx_msg void OnBnClickedFindoptions();
    afx_msg void OnBnClickedReplaceoptions();
    afx_msg void OnBnClickedFiltoptions();
    afx_msg void OnBnClickedExport();
    afx_msg void OnBnClickedImport();
};
