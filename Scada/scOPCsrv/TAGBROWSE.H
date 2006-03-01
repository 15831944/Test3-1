//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TAGBROWSE_H
#define __TAGBROWSE_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

//#include "project.h"
//#include "OPCSrvr/srvCallback.h"

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
    void Build(/*Project*//*CScdOPCCallBack*/CExecObj* pPrj,char* pRootTag);
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
extern CTagTrees TagTrees;
//---------------------------------------------------------------------------

enum TagSelectAllowed { SelAny, SelFull, SelRoot };



//---------------------------------------------------------------------------

#endif
