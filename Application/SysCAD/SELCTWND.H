//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __SLCTWND_H
#define __SLCTWND_H

#include "resource.h"

//---------------------------------------------------------------------------

_FWDDEF(CModelTypeList);
class CModelTypeList : public CStringArray
  {
  public:
    CModelTypeList(char* pModelName) { sModelName = pModelName; };
    CModelTypeList() {};
    void Reset() { SetSize(0); };
    void Reset(char* pModelName) { sModelName = pModelName; SetSize(0); };
    char* ModelName() { return (char*)(const char*)sModelName; };
    char* GetTagAt(int index) { return (char*)(const char*)(GetAt(index)); };

  public:
    CString        sModelName;
    byte           bFlagged:1,
                   bUnit:1;       //true if unit, else a link
  };
 
//---------------------------------------------------------------------------

class CTagTreeItem
  {
  friend class CTagTree;
  public:
    CTagTreeItem(char* pTag, char* pValue, DWORD Flags, int SeqNo);
    CTagTreeItem(char* pTag, char* pValue, DWORD Flags, int SeqNo, CTagTreeItem* Parent);
    CTagTreeItem(char* pTag, char* pValue, DWORD Flags, int SeqNo, CTagTreeItem* Parent, CTagTreeItem* Nxt);
    ~CTagTreeItem();
    char* Tag() { return sTag(); };
    char* Value() { return sValue(); };
    int ValueLen() { return sValue.Len(); };
    DWORD Flags() { return dwFlags; };
    int SeqNo() { return iSeqNo; };
    void GetFullTag(Strng& FullTag);
    CTagTreeItem*  Child() { return pChild; };
    CTagTreeItem*  Parent() { return pParent; };
    CTagTreeItem*  Next() { return pNxt; };
    CTagTreeItem*  InsertChildAtEnd(char* pTag, char* pValue, DWORD Flags, int SeqNo);
    CTagTreeItem*  InsertChildAtBegining(char* pTag, char* pValue, DWORD Flags, int SeqNo);
    CTagTreeItem*  FindChildTag(char* pTag);

  protected:
    flag DoPopulateTree(HTREEITEM hParent, CTreeCtrl& Tree, TV_INSERTSTRUCT& Ins, DWORD dwMask, DWORD dwExcludeMask);

    Strng          sTag;
    Strng          sValue;
    DWORD          dwFlags;
    int            iSeqNo;
    CTagTreeItem*  pNxt;
    CTagTreeItem*  pChild;
    CTagTreeItem*  pParent;
  };


class CTagTree
  {
  public:
    CTagTree() { pRoot = NULL; };
    ~CTagTree() { DeleteAllItems(); };
    void DeleteAllItems();
    void Build(char* pRootTag);
    void PopulateTree(CTreeCtrl& Tree, TV_INSERTSTRUCT& Ins, DWORD dwMask, DWORD dwExcludeMask);
    CTagTreeItem* Root() { return pRoot; };
  protected:

    CTagTreeItem*  pRoot;
  };


typedef CArray <pCModelTypeList, pCModelTypeList&> CModelTypeListArray; 

class CTagTrees
  {
  public:
    CTagTrees();
    ~CTagTrees();
    void Rebuild(int Request);
    int FindModelName(char* pModelName);
    int FindPartModelName(char* pModelName, int IndexStart=-1);
    CModelTypeListArray& GetList() { return List; };
    //static BuildTagTree(

  protected:
    CModelTypeListArray List;
  };

//---------------------------------------------------------------------------

//extern CTagTrees TagTrees;
//
//---------------------------------------------------------------------------

enum TagSelectAllowed { SelAny, SelFull, SelRoot };

class CSelectTagDlg : public CDialog
  {
  public:
    CSelectTagDlg(char* pTag, byte SelnAllowed = SelFull, flag AllowTagEdit = 1, CWnd* pParent = NULL);
    //{{AFX_DATA(CSelectTagDlg)
    enum { IDD = IDD_SELECTTAGDLG };
    CComboBox Seln;
    CString sTag;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSelectTagDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    CTagTree T;
    static CTagTrees TagTrees;

    TagSelectAllowed eSelType;
    int iLevel;
    int iMdlIndex;
    int iLastTagLen;
    CString sRootTag;
    CString sCurTag;
    CTagTreeItem* pTI;
    Strng sCnvTxt;
    byte bAllowTagEdit:1,
         bDoneRebuild:1,
         bBackward:1,
         bEnableOK:1;

    void CalcSelnList();
    void CalcSubSeln();
    void CalcTagTree();
    void CalcTag();
    void CalcLevel(flag UseLast);
    //{{AFX_MSG(CSelectTagDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnBack();
    afx_msg void OnSelchangeSeln();
    afx_msg void OnFwd();
    afx_msg void OnChangeTag();
    afx_msg void OnRoot();
    //}}AFX_MSG
    afx_msg void OnUpdateFwd(CCmdUI* pCmdUi);
    afx_msg void OnUpdateBck(CCmdUI* pCmdUi);
    afx_msg void OnUpdateOK(CCmdUI* pCmdUi);
    afx_msg void OnUpdateTagEdit(CCmdUI* pCmdUi);
    afx_msg void OnUpdateSelnCombo(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

#endif
