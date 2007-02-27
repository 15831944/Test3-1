#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CExploreScd dialog

class CExploreScd;
class CXTTag;
class CXTClass; 
class CXTPage; 
class CGrfDoc;

class CXTTagHPair
  {
  public:
    CXTTagHPair()
      {
      m_pTag=NULL;
      m_hTag=0;
      }
    CXTTagHPair(CXTTag * pTag)
      {
      m_pTag=pTag;
      m_hTag=0;
      }
    CXTTag      * m_pTag;
    HTREEITEM     m_hTag;
  };

class CXTPageHPair
  {
  public:
    CXTPageHPair()
      {
      m_pPage=NULL;
      m_hPage=0;
      }
    CXTPageHPair(CXTPage * pPage)
      {
      m_pPage=pPage;
      m_hPage=0;
      }
    CXTPage     * m_pPage;
    HTREEITEM     m_hPage;
  };


class CXTTagArray         : public CArray <CXTTag*, CXTTag*> {};
class CXTTagHPairArray    : public CArray <CXTTagHPair*, CXTTagHPair*> {};
class CXTTagHPairMap      : public CMap   <CXTTag*, CXTTag*, CXTTagHPair*, CXTTagHPair*> {};
class CXTTagMap           : public CMap   <LPCTSTR, LPCTSTR, CXTTag*, CXTTag*> {};
class CXTHTagMap          : public CMap   <HTREEITEM, HTREEITEM, CXTTag*, CXTTag*> {};
class CXTPageArray        : public CArray <CXTPage*, CXTPage*> {};
class CXTPageHPairArray   : public CArray <CXTPageHPair*, CXTPageHPair*> {};
class CXTPageHPairMap     : public CMap   <CXTPage*, CXTPage*, CXTPageHPair*, CXTPageHPair*> {};
class CXTPageMap          : public CMap   <LPCTSTR, LPCTSTR, CXTPage*, CXTPage*> {};
class CXTClassArray       : public CArray <CXTClass*, CXTClass*> {};
class CXTClassMap         : public CMap   <LPCTSTR, LPCTSTR, CXTClass*, CXTClass*> {};

enum 
  { 
  TrID_GraphicHdr, TrID_Graphic, 
  TrID_TrendHdr, TrID_Trend, 
  TrID_OtherHdr, TrID_Other, 
  TrID_NodeHdr, TrID_Node, 
  TrID_ClassHdr, TrID_Class, 
  //TrID_LostGrfHdr, TrID_LostGrf,
  //TrID_LostMdlHdr, TrID_LostMdl,
  };

class CXTTreeInfo
  {
  public:
    CXTTreeInfo(int Id, CString *p=NULL)
      {
      m_Id=Id;
      if (p)
        {
        m_OwnsStr=false;
        m_pStr=p;
        }
      else
        {
        m_OwnsStr=true;
        m_pStr=new CString;
        }
      }
    CXTTreeInfo(int Id, LPCTSTR p)
      {
      m_Id=Id;
      m_OwnsStr=true;
      m_pStr=new CString(p);
      }
    ~CXTTreeInfo()
      {
      if (m_OwnsStr)
        delete m_pStr;
      }
    LPTSTR Text() { return (LPTSTR)(LPCTSTR)(*m_pStr); }

  int m_Id;
  bool m_OwnsStr;
  CString * m_pStr;

  };

class CXTTag : public CXTTreeInfo
  {
  friend class CExploreScd;

  public:
    CXTTag(CExploreScd * Dlg, LPCTSTR Tag, CXTClass * ClassId);
    ~CXTTag();

    int Icon();

  public:
    CExploreScd   & m_Dlg;
    CString         m_sTag;
    CString         m_sTagLwr;
    CXTClass      * m_pClass;
    BOOL            m_Selected;
    BOOL            m_Marked;
    BOOL            m_InUse;
    BOOL            m_ModelOK;
    HTREEITEM       m_hTreeItem;
    HTREEITEM       m_hClassItem;
    int             m_iAccPage;
    //HTREEITEM       m_hLostGrf;

    CXTPageHPairArray  m_Pages;
    CXTPageHPairMap    m_PHMap;

  };

//class CXTNMTag : public CXTTreeInfo
//  {
//  friend class CExploreScd;
//
//  public:
//    CXTNMTag(CExploreScd * Dlg, LPCTSTR Tag);
//    ~CXTNMTag();
//
//  public:
//    CExploreScd   & m_Dlg;
//    CString         m_sTag;
//    CString         m_sTagLwr;
//    BOOL            m_Selected;
//    BOOL            m_Marked;
//    BOOL            m_InUse;
//    HTREEITEM       m_hTreeItem;
//
//    CXTPageHPairArray  m_Pages;
//    CXTPageHPairMap    m_PHMap;
//
//  };

class CXTClass : public CXTTreeInfo
  {
  friend class CExploreScd;

  public:
    CXTClass(CExploreScd * Dlg, LPCTSTR ClassId);
    ~CXTClass();

  public:
    CExploreScd   & m_Dlg;
    CString         m_sClassId;
    BOOL            m_Selected; 
    HTREEITEM       m_hClassItem;
    int             m_nCount;
    int             m_nSelected;

    CXTTagHPairArray  m_Tags;

  };

class CXTPage : public CXTTreeInfo
  {
  friend class CExploreScd;

  public:
    CXTPage(CExploreScd * Dlg, int IdNo, LPCTSTR PageId, LPCTSTR PageName);
    ~CXTPage();

  public:
    CExploreScd   & m_Dlg;
    CString         m_sPageId;
    CString         m_sPageName;
    BOOL            m_Selected;
    BOOL            m_InUse;
    HTREEITEM       m_hPage;
    int             m_iType;
    CGrfDoc*        m_pGrfDoc;

    Strng_List        m_GrfTagList;
    Strng_List        m_NoModelList;
    CXTTagHPairArray  m_TagHs;
    CXTTagHPairMap    m_TagHMap;
  };

class CExploreScd : public CDialog
  {
  DECLARE_DYNAMIC(CExploreScd)

  public:
    CExploreScd(CWnd* pParent = NULL); // standard constructor
    virtual ~CExploreScd();

    static void OpenIt();
    static void CloseIt();
    static bool TestOpen();
    static void RefreshIt(BOOL Complete);
    static void ChkRefreshIt();
    static BOOL UseScdExplorer() { return sm_bUseScdExplorer; };
    static void SetUseScdExplorer(BOOL Use) { sm_bUseScdExplorer = Use; };

    static void ArrangeWindows(int RqdLayoutStyle = -1);
    static void MainWndMovedTo(int x, int y);

    // Dialog Data
    enum { IDD = IDD_EXPLORESCD };

  protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    void InitTags();

    void GetRawTags();
    void GetRawPages(bool ChangesOK);
    void FindTagPages();
    void RemoveUnusedItems();
    void SortAll();
    //void FinaliseLostTags();
    void FinaliseHeaders();
    void BuildTags();
    void ReBuildTags();
    void ClearTags();

    void PositionControls(int cx, int cy);
    void FixFilterStuff();
    void SetFilter();
    void SetTagFilter(CXTTag & Tg, BOOL DoSetup);
    void UpdateSelectDisplay();

    bool LoadTagTree(bool DoKbdTest);
    //void LoadTagItem(CXTTag & Tg, bool UpdateTag=false);
    void ActivateWndByName(LPCTSTR Txt);
    void AddTagToTree(CXTTag *pTag, CXTTag *pPrev);
    void RemoveTagFromTree(CXTTag * pTag);
    void AddPageToTree(CXTPage * pPage);
    void RemovePageFromTree(CXTPage * pPage);
    void DumpAll(LPCTSTR Where);

    void DeletePage(CXTPage * pPage);
    void RenamePage(CXTPage * pPage);

    void CopyTagList2Clipboard(HTREEITEM hRootItem, int DoLevelCount, LPCTSTR Header, CString & LineHeader, CString & Buffer, bool TopLevel=true);

    void SavePos();
    void RestorePos();
    void SaveProfile();
    void SaveExpanded();

    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();

    static CExploreScd  * sm_pTheWnd;
    static BOOL           sm_bDoRefresh;
    static BOOL           sm_bDoRefreshAll;
    static BOOL           sm_bUseScdExplorer;
    static BOOL           sm_bInited;

    CTreeCtrl             m_Tree;
    CComboBox             m_TagFilterRule;
    CEdit                 m_TagFilter;

    CXTTreeInfo           m_GraphicTitle;
    CXTTreeInfo           m_ClassTitle;
    CXTTreeInfo           m_OtherTitle;
    CXTTreeInfo           m_TrendTitle;
    CXTTreeInfo           m_NodeTitle;
    //CXTTreeInfo           m_LostGrfTitle;
    //CXTTreeInfo           m_LostMdlTitle;

    CImageList            m_TreeImgList;
    int                   m_WindowCount;
    int                   m_GraphicCount;
    int                   m_TrendCount;

    //BOOL                  m_HasLostTags;

    HTREEITEM             m_hGraphItem;
    HTREEITEM             m_hTrendItem;
    HTREEITEM             m_hOtherItem;
    HTREEITEM             m_hWndItems[3];
    HTREEITEM             m_hClassItem;
    HTREEITEM             m_hNodesItem;
    //HTREEITEM             m_hLostGrfItem;
    //HTREEITEM             m_hLostMdlItem;

    BOOL                  m_bGraphExpanded;
    BOOL                  m_bTrendExpanded;
    BOOL                  m_bOtherExpanded;
    BOOL                  m_bClassExpanded;
    BOOL                  m_bNodesExpanded;
    //BOOL                  m_bLostGrfExpanded;
    //BOOL                  m_bLostMdlExpanded;

    eFilterRule           m_FilterRule; 
    CString               m_FilterString;
    BOOL                  m_bCaseSens;
    IRegExpPtr *          m_pFilterRE;
    int                   m_nSelected;
    //int                   m_nLostGrf;
    //int                   m_nLostMdl;

    int                   m_ChangeBusy;


    CPoint                m_TopLeft;            // Relative to Main Wnd Client TopLeft
    CRect                 m_ClientRctInit;
    CArray<CRect, CRect&> m_CtrlRcts;

    CXTTagArray           m_Tags;
    CXTTagMap             m_TagMap;
    CXTHTagMap            m_HTagMap;
    CXTPageArray          m_Pages;
    CXTPageMap            m_PageMap;
    CXTClassArray         m_Classes;
    CXTClassMap           m_ClassMap;

    HTREEITEM    InsertItem( LPCTSTR lpszItem, int nImage, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter=TVI_LAST);

    static const int ODC_Left = 1;
    static const int ODC_Right = 2;
    static const int ODC_Select = 3;

    void         OnDoClicks(NMHDR *pNMHDR, LRESULT *pResult, int Where);

    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeTagfilter();
    afx_msg void OnCbnSelchangeTagfilterrule();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult);
  };
