#pragma once

#ifndef __DBBASE_H
#include "dbbase.h"
#endif
#include "grfdoc.h"

//---------------------------------------------------------------------------

const int CommonDBMaxTagLen = 61;
const int CommonDBMaxDescLen = 256;
const int CommonDBMaxLibraryLen = 256;
const int CommonDBMaxPhNmLen = 64;

//---------------------------------------------------------------------------

class CDblTriPt;
class CFltTriPt;

class CDblTriPt 
  {
  public:
    CDblTriPt();
    CDblTriPt(const CDblTriPt & D);
    CDblTriPt(const CFltTriPt & D);
    CDblTriPt(double X, double Y, double Z);
    CDblTriPt & operator=(const CDblTriPt & D);
    CDblTriPt operator+(const CDblTriPt & D);
    CDblTriPt operator-(const CDblTriPt & D);
    double    m_X, m_Y, m_Z;
  };

inline CDblTriPt::CDblTriPt() { m_X=m_Y=m_Z=0; }
inline CDblTriPt::CDblTriPt (const CDblTriPt & D) { m_X=D.m_X; m_Y=D.m_Y; m_Z=D.m_Z; }
inline CDblTriPt::CDblTriPt(double X, double Y, double Z) { m_X=X; m_Y=Y; m_Z=Z; }
inline CDblTriPt & CDblTriPt::operator=(const CDblTriPt & D) { m_X=D.m_X; m_Y=D.m_Y; m_Z=D.m_Z;  return *this; }
inline CDblTriPt CDblTriPt::operator+(const CDblTriPt & D) { return CDblTriPt(m_X+D.m_X, m_Y+D.m_Y, m_Z+D.m_Z); }
inline CDblTriPt CDblTriPt::operator-(const CDblTriPt & D) { return CDblTriPt(m_X-D.m_X, m_Y-D.m_Y, m_Z-D.m_Z); }

//---------------------------------------------------------------------------

class CNeutralGroups : public CFileRec
  {
  public:
    CNeutralGroups(KWDatabase* pDB_);
    virtual void Init();
    virtual int Read() { return BaseRead(); };
    virtual int Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL bDoneInit;
  public:
    char        m_sGroup[CommonDBMaxTagLen];
    char        m_sDescription[CommonDBMaxDescLen];
    CDblTriPt   m_Lo;
    CDblTriPt   m_Hi;
    CDblTriPt   m_AlignLo;
    CDblTriPt   m_AlignHi;
  };

//---------------------------------------------------------------------------

class CNeutralGroupTagEditor
  {
  public:
    CNeutralGroupTagEditor();

    int       FindOldTag(LPCTSTR RqdTag) { return FindOldTag(m_pOldTags, RqdTag); };
    int       FindNewTag(LPCTSTR RqdTag) { return FindNewTag(m_pNewTags, RqdTag); };
    CString   AdjustTag(LPCTSTR OldTag);
    CString   AdjustTagWild(LPCTSTR OldTag);

    static int FindOldTag(CStringArray * OldTags, LPCTSTR RqdTag);
    static int FindNewTag(CStringArray * NewTags,  LPCTSTR RqdTag);
    static CString AdjustTag(LPCTSTR OldTag, long FixupRule, CString & String1, CString & String2, CStringArray * OldTags, CStringArray * NewTags);
    static CString AdjustTagWild(LPCTSTR OldTag, long FixupRule, CString & String1, CString & String2, CStringArray * OldTags, CStringArray * NewTags);

  protected:
    long                    m_iFixupRule;
    CString                 m_sString1;
    CString                 m_sString2;

    CStringArray          * m_pOldTags;
    CStringArray          * m_pNewTags;
  };
//---------------------------------------------------------------------------

class CNeutralGroupInserts : public CFileRec
  {
  public:
    CNeutralGroupInserts(KWDatabase* pDB_);
    //virtual ~CNeutralGroupInserts();
    virtual void Init();
    virtual int Read() { return BaseRead(); };
    virtual int Write(BOOL AddNew = TRUE) { return BaseWrite(AddNew); };

  protected:
    static BOOL bDoneInit;
  public:
    char        m_sLibrary[CommonDBMaxLibraryLen];
    char        m_sGroup[CommonDBMaxTagLen];
    char        m_sInsertPg[CommonDBMaxPhNmLen];
    CDblTriPt   m_Insert;
    CDblTriPt   m_Scale;
    int         m_iFixupRule;
    char        m_sFixupString1[CommonDBMaxTagLen];
    char        m_sFixupString2[CommonDBMaxTagLen];
  };

//---------------------------------------------------------------------------

class CNeutralGrpDescItem { public: CString m_sGroup; CString m_Desc; };
class CNeutralGrpDescList  : public CList<CNeutralGrpDescItem , CNeutralGrpDescItem&> {};

//---------------------------------------------------------------------------

