//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTCAT_H
#define  __HSTCAT_H

#ifndef __HSTMSG_H
#include "hstmsg.h"
#endif

//---------------------------------------------------------------------------
_FWDDEF(CHistorian)
_FWDDEF(CEvBlk)

//---------------------------------------------------------------------------

_FWDDEF(CCatItem)
class CCatItem
  {
  DEFINE_MEMSTATS(CCatItem)

  public:
    HD_CatItem    Data;
    long          lRecNo;   //record number of catalog item in catalog file
    static bool   m_bFixedDataExtn;
    static CString m_sFilenameExtn;

    CCatItem();
    void UpdateTime(pCEvBlk pEvBlk);
    static char* CatNoStr(WORD CatNo);
    static WORD CatNoFromStr(char* s);
    static void MakeFilename(WORD CatNo, CString &s);
    static void MakeCatFilename(WORD CatNo, CString &s);
    static bool FixedDataExtn() { return m_bFixedDataExtn; };

    inline char* CatNoStr() { return CatNoStr(Data.iCatNo); };
    inline void MakeFilename(CString &s) { MakeFilename(Data.iCatNo, s); };
    inline void MakeCatFilename(CString &s) { MakeCatFilename(Data.iCatNo, s); };
    void MakeMapName(CString& MapName);
    inline WORD CatNo() { return Data.iCatNo; };
    inline double StartTime() { return Data.dStartTime; };
    inline double LastTime() { return Data.dLastTime; };
    void dbgDump();
  };

//---------------------------------------------------------------------------

typedef CMap<WORD, WORD, long, long> CCatItemsMap;
template <> UINT AFXAPI HashKey<WORD> (WORD Key) ;

_FWDDEF(CHistCat)
class CHistCat
  {
  DEFINE_MEMSTATS(CHistCat)

  public:
    CString       sFilename;   //filename of catalog file
    HD_CatHead    Data;
    long          lAllocCnt;   //number of items allocated to memery for array
    CCatItem **   CatItems;    //pointer to array of catalog items
    CCatItemsMap  ItemsMap;    //collection, maping catalog numbers to index into array
    WORD          iCatItemCnt; //number of cat items for this scenario

    CHistCat();
    ~CHistCat();
    void Initialise();
    void ClearDataLists();
    void MakeCatItemSpace();
    void SetFilename(char* Filename);
    byte Load(flag HdOnly = false);
    flag Save(long CatNo = -1);
    void AddCatItem(WORD CatNo);
    void DeleteLastCatItem();
    pCCatItem Find(WORD CatNo);
    inline pCCatItem GetCatItemByIndex(long index)
      { /*ASSERT(index<Data.iCatItemCnt);*/ return CatItems[index]; };
    long GetCatItemByLastTime(double LastTime);
    long GetCatItemByStartTime(double StartTime);
    void dbgDump(char* p);
  };

//---------------------------------------------------------------------------
//#ifndef __HSTMAIN_H
//#include "hstmain.h"
//#endif
//---------------------------------------------------------------------------

#undef DllImportExport 

#endif
