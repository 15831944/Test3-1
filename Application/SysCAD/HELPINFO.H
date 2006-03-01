//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __HELPINFO_H
#define __HELPINFO_H

#ifndef  __GPWFUNCS_H
#include "gpwfuncs.h"
#endif

//---------------------------------------------------------------------------

class CHelpItem
  {
  public:
    CHelpItem() 
      { iItemID = 0; };
    CHelpItem(UINT ItemID, char* ShortDesc, char* LongDesc)
      { iItemID = ItemID; sShortDesc = ShortDesc; sLongDesc = LongDesc; };
    inline BOOL HasShortDesc() { return (sShortDesc.Len()>0); };
    inline char* ShortDesc() { return sShortDesc(); };
    inline char* LongDesc(int index) { return sLongDesc(); };

  //protected:
    UINT iItemID;
    Strng sShortDesc;
    Strng sLongDesc;
  };

//---------------------------------------------------------------------------

#pragma warning (disable : 4097)

typedef CArray <CHelpItem, CHelpItem&> CBaseHelpItemArray;

class CHelpItemArray : public CBaseHelpItemArray
  {
  friend class CHelpInfoMngr;
  public:
    CHelpItemArray(UINT GroupID);
    int FindItem(UINT ItemID);
    inline CHelpItem* HelpItem(int index) { return &(m_pData[index]); };
    inline char* ShortDesc(int index) { return m_pData[index].sShortDesc(); };
    inline char* LongDesc(int index) { return m_pData[index].sLongDesc(); };
    inline CHelpItemArray* Next() { return pNxt; };
  protected:
    UINT iGroupID;
    CHelpItemArray* pNxt;
  };

#pragma warning (4 : 4097)

//---------------------------------------------------------------------------


class CHelpInfoMngr
  {
  public:
    CHelpInfoMngr();
    void Init();
    void Term();
    void UpdateOptions();
    void AddHelpInfo(UINT GroupID, UINT ItemID, char* ShortDesc, char* LongDesc);
    CHelpItem* GetHelpInfo(UINT GroupID, UINT ItemID);
    inline BOOL ShowToolTips() { return bShowTooltips; };

  protected:
    CHelpItemArray* FindGroup(UINT GroupID);

    CHelpItemArray* pFirst;
    WORD      bShowTooltips:1;
  };

//---------------------------------------------------------------------------

extern CHelpInfoMngr HelpMngr;

//---------------------------------------------------------------------------
#endif

