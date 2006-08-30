//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#ifndef __SLCTWND_H
  #include "selctwnd.h"
#endif

//---------------------------------------------------------------------------

class CPrjFormFileItem 
  {
  public:
    Strng Grp, Fn, Path, Exists;
  };
class CPrjFormFileArray : public CArray <CPrjFormFileItem, CPrjFormFileItem&>
  {
  public:
    int Find(char * pFn)
      {
      Strng Fn, Path;
      Fn.FnNameExt(pFn);
      Path.FnDrivePath(pFn);
      for (int i=0; i<GetSize(); i++)
        {
        if (Fn.XStrICmp(ElementAt(i).Fn())!=0)
          continue;

        if (Path.Len()==0 && ElementAt(i).Path.Len()==0)
          return 1;
        if (Path.Len()!=0 && ElementAt(i).Path.Len()!=0)
          break;
        if (Path.XStrICmp(ElementAt(i).Path())==0)
          return i;
        }
      return -1;
      };
  };
class CPrjFormFilePtrArray : public CArray <CPrjFormFileItem*, CPrjFormFileItem*> {};

class CPrjFormTagRefItem 
  {
  public:
    Strng Used, Type, Tag, sGetCnt, sSetCnt;
    int iGetCnt, iSetCnt;
  };
class CPrjFormTagRefArray : public CArray <CPrjFormTagRefItem, CPrjFormTagRefItem&> {};
class CPrjFormTagRefPtrArray : public CArray <CPrjFormTagRefItem*, CPrjFormTagRefItem*> {};

class CPrjForm : public CFormView
  {
  protected:
    CPrjForm();           // protected constructor used by dynamic creation
    virtual ~CPrjForm();
    DECLARE_DYNCREATE(CPrjForm)
  public:
    //{{AFX_DATA(CPrjForm)
    enum { IDD = IDD_PF_FORM };
    CCustomListCtrl FileList;
    CCustomListCtrl EnvList;
    CCustomTreeCtrl MdlTree;
    CCustomTreeCtrl FileTree;
    CCustomTreeCtrl CnvTree;
    CCustomListCtrl TagRefList;
    CTabCtrl m_Tab;
    BOOL m_Symbolic;
    int m_FileView;
    int		m_ModelGroup;
    //}}AFX_DATA
  public:
    //{{AFX_VIRTUAL(CPrjForm)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL
  public:
    static int iOrdCol[4];
    static int iOrdDir[4];
    static int iTROrdCol[5];
    static int iTROrdDir[5];
  protected:
    CTagTrees             TagTrees;
    CPrjFormFileArray     m_FilesInList;
    CPrjFormFilePtrArray  m_FilesOrder;
    int nFilesInList;
    CPrjFormTagRefArray    m_TagRefList;
    CPrjFormTagRefPtrArray m_TagRefOrder;
    int nTagRefInList;

    CImageList FileImgList;
    int iPrevTab;
    Strng sCurrentSeln;
    DWORD dwCurrentData;
    HTREEITEM hCurrentSeln;
    flag  bMdlTreeBuilt:1,
          bFileTreeBuilt:1,
          bCnvTreeBuilt:1,
          bTagRefListBuilt:1,
          bEnvListBuilt:1,
          bShowAllClasses:1,
          bMissingNotified:1;

    HANDLE hCfgProcess;            //process handle for the editor
    DWORD dwCfgProcessId;          //process ID for the editor
    
    void DoShowHide();
    void ChangeTab();
    void SortFileList();
    void ChkSymbolic();
    int TagRefHitTest();
    void SortTagRefList();
    void RebuildMdlTree();
    void RebuildFileTree();
    void RebuildCnvTree();
    void RebuildEnvList();
    void RebuildTagRefList();
    void MakeFilename(CString PathTxt, CString FileTxt, Strng& FilenameOnly, Strng& FullFilename);

    #ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
    #endif
    //{{AFX_MSG(CPrjForm)
    afx_msg void OnRefresh();
    afx_msg void OnButton2();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickEnvList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkEnvList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickMdlTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkMdlTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickFileTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkFileTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemclickPfFilelist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickRefTagList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkRefTagList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSymbolic();
    afx_msg void OnFileViewFolder();
    afx_msg void OnFileViewGroup();
    afx_msg void OnFileViewList();
    afx_msg void OnModelGroupRadio();
    //}}AFX_MSG
    afx_msg void OnMenu(UINT id);
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

class CPrjFrameWnd : public CMDIChildWnd
  {
  DECLARE_DYNCREATE(CPrjFrameWnd)
  protected:
    CPrjFrameWnd();           // protected constructor used by dynamic creation
  public:
    //{{AFX_VIRTUAL(CPrjFrameWnd)
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  protected:
    virtual ~CPrjFrameWnd();
    //{{AFX_MSG(CPrjFrameWnd)
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------
