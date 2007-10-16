#include "stdafx.h"
#include ".\neutralgrf.h"
#include ".\dbhelper.h"

//#include "optoff.h"

//===========================================================================

static double DefValZero=0.0;
static double DefValOne=1.0;
static double DefValNAN=dNAN;

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralGroups::bDoneInit = 0;
CNeutralGroups::CNeutralGroups(KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  sTableName = "Groups";
  //if (!bDoneInit)
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGroups::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_None);
    CreateStr("Description", m_sDescription, sizeof(m_sDescription)-1, FF_AllowZeroLen);
    //CreateVal("OriginX", &xm_Origin.m_X, ADOX::adSingle, FF_Required);
    //CreateVal("OriginY", &xm_Origin.m_Y, ADOX::adSingle, FF_Required);
    //CreateVal("OriginZ", &xm_Origin.m_Z, ADOX::adSingle, FF_Required);

    CreateVal("LoX", &m_Lo.m_X, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("LoY", &m_Lo.m_Y, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("LoZ", &m_Lo.m_Z, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("HiX", &m_Hi.m_X, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("HiY", &m_Hi.m_Y, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("HiZ", &m_Hi.m_Z, ADOX::adDouble, FF_AllowZeroLen);

    CreateVal("AlignLoX", &m_AlignLo.m_X, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("AlignLoY", &m_AlignLo.m_Y, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("AlignLoZ", &m_AlignLo.m_Z, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("AlignHiX", &m_AlignHi.m_X, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("AlignHiY", &m_AlignHi.m_Y, ADOX::adDouble, FF_AllowZeroLen);
    CreateVal("AlignHiZ", &m_AlignHi.m_Z, ADOX::adDouble, FF_AllowZeroLen);
    
    //CreateIndex("OriginX", false, false);
    CreateIndex("Group", false, false);
    bDoneInit = 1;
    }
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CNeutralGroupTagEditor::CNeutralGroupTagEditor()
  {
  m_pOldTags   = NULL;
  m_pNewTags   = NULL;
  m_iFixupRule = eFixup_NoChange;
  };

//---------------------------------------------------------------------------

int CNeutralGroupTagEditor::FindOldTag(CStringArray * OldTags, LPCTSTR RqdTag)
  {
  if (OldTags)
    {
    for (int i=0; i<OldTags->GetCount(); i++)
      {
      if ((*OldTags)[i].CompareNoCase(RqdTag)==0)
        return i;
      }
    return -1;
    }
  return 0;
  }

//---------------------------------------------------------------------------

int CNeutralGroupTagEditor::FindNewTag(CStringArray * NewTags,  LPCTSTR RqdTag)
  {
  if (NewTags)
    {
    for (int i=0; i<NewTags->GetCount(); i++)
      {
      if ((*NewTags)[i].CompareNoCase(RqdTag)==0)
        return i;
      }
    }
  return -1;
  }

//---------------------------------------------------------------------------


CString CNeutralGroupTagEditor::AdjustTag(LPCTSTR OldTag)
  {
  return AdjustTag(OldTag, m_iFixupRule, m_sString1, m_sString2, m_pOldTags, m_pNewTags);
  }

CString CNeutralGroupTagEditor::AdjustTag(LPCTSTR OldTag, long FixupRule, CString & String1, CString & String2, CStringArray * OldTags, CStringArray * NewTags)
  {
  CString Ret("???");
  switch (FixupRule)
    {
    case eFixup_NoChange      : Ret=OldTag;                    break;
    case eFixup_Prefix        : Ret=String1+OldTag;            break;
    case eFixup_Suffix        : Ret=OldTag+String2;            break;
    case eFixup_PrefixSuffix  : Ret=String1+OldTag+String2;    break;
    case eFixup_ReplaceTag    : Ret=String1;                   break;
    case eFixup_ExchangeTags   : 
      {
      int i=FindOldTag(OldTags, OldTag);
      if (i>=0)
        {
        Ret=(*NewTags)[i];
        }
      else
        {
        ASSERT_ALWAYS(FALSE, "Bad Adjust Tag", __FILE__, __LINE__); 
        Ret=CString("Replacement")+OldTag;
        }
      break;
      }
    case eFixup_RegExpression: 
      {
      IRegExpPtr *pRE= new IRegExpPtr("VBScript.RegExp");

      _bstr_t bFndTxt(String1);
      _bstr_t bRepTxt(String2);
      _bstr_t bOldTag(OldTag);
        
      (*pRE)->Pattern = bFndTxt;
      _bstr_t bNewTag=(*pRE)->Replace(bOldTag, bRepTxt);
      delete pRE;
      Ret=(LPCTSTR)bNewTag;
      break;
      }
    }

  Strng S=Ret;
  if (TaggedObject::ValidateTagDotChanged(S))
    {//b) illegal characters
    Ret=S();
    //ErrMsg.Format("Tag %s changed to confrom to tag rules", Ret);
    //OK=false;
    }

  return Ret;
  };

//-------------------------------------------------------------------------

CString CNeutralGroupTagEditor::AdjustTagWild(LPCTSTR OldTag)
  {
  return AdjustTagWild(OldTag, m_iFixupRule, m_sString1, m_sString2, m_pOldTags, m_pNewTags);
  }

CString CNeutralGroupTagEditor::AdjustTagWild(LPCTSTR OldTag, long FixupRule, CString & String1, CString & String2, CStringArray * OldTags, CStringArray * NewTags)
  {
  switch (FixupRule)
    {
    case eFixup_NoChange      : return OldTag;
    case eFixup_Prefix        : ASSERT_ALWAYS(FALSE, "Invalid Path", __FILE__, __LINE__); return String1+OldTag;
    case eFixup_Suffix        : ASSERT_ALWAYS(FALSE, "Invalid Path", __FILE__, __LINE__); return OldTag+String2;
    case eFixup_PrefixSuffix  : ASSERT_ALWAYS(FALSE, "Invalid Path", __FILE__, __LINE__); return String1+OldTag+String2;
    case eFixup_ReplaceTag    : ASSERT_ALWAYS(FALSE, "Invalid Path", __FILE__, __LINE__); return String1;
    case eFixup_ExchangeTags   : 
      {
      CString New(OldTag), Tmp;
      IRegExpPtr *pRE= new IRegExpPtr("VBScript.RegExp");

      (*pRE)->Global     = _variant_t(true);//' Set do all occurs
      (*pRE)->IgnoreCase = _variant_t(true);//' Set case insensitivity.

      LPCTSTR Terms="[\\[\\]\\{\\}\\.]";

      CString FndTxt, RepTxt, NewLC(New);
      NewLC.MakeLower();
      for (int i=0; i<OldTags->GetCount(); i++)
        {
        if (New.Compare((*OldTags)[i])==0)
          {
          New=(*NewTags)[i];
          }
        else
          {

          CString FndTxt, RepTxt, OldLC((*OldTags)[i]);
          OldLC.MakeLower();

          if (NewLC.Find(OldLC)>=0)
            { 
            FndTxt.Format("(%s{1})%s(%s{1})", Terms, (*OldTags)[i], Terms);
            RepTxt.Format("$1%s$2", (*NewTags)[i]);

            _bstr_t bFndTxt(FndTxt);
            _bstr_t bRepTxt(RepTxt);
            _bstr_t bOldTag(New);
              
            if (strstr(OldTag, "FBEqn"))
              { int xxx=0; }
            (*pRE)->Pattern = bFndTxt;
            _bstr_t bNewTag=(*pRE)->Replace(bOldTag, bRepTxt);

            Tmp=New;
            New=(LPCTSTR)bNewTag;
            if (0 && New!=Tmp)
              dbgpln("%4i %-40s >> %-40s  %-20s  %-20s", i, Tmp, New, (*OldTags)[i], (*NewTags)[i]); 
            }
          }
        }

      delete pRE;

      return New;
      }
    case eFixup_RegExpression: 
      {
      ASSERT_ALWAYS(FALSE, "Invalid Path", __FILE__, __LINE__); 

      IRegExpPtr *pRE= new IRegExpPtr("VBScript.RegExp");

      _bstr_t bFndTxt(String1);
      _bstr_t bRepTxt(String2);
      _bstr_t bOldTag(OldTag);
        
      (*pRE)->Pattern = bFndTxt;
      _bstr_t bNewTag=(*pRE)->Replace(bOldTag, bRepTxt);
      delete pRE;
      return (LPCTSTR)bNewTag;
      }
    }
  return "???";
  };

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

BOOL CNeutralGroupInserts::bDoneInit = 0;
CNeutralGroupInserts::CNeutralGroupInserts(KWDatabase* m_pDB_) : CFileRec(false, true, m_pDB_)
  {
  sTableName = "GroupInserts";
  //if (!bDoneInit)
  Init();
  }

//-------------------------------------------------------------------------

void CNeutralGroupInserts::Init()
  {
  if (m_pDB)
    {
    DefineClass();
    CreateStr("Library", m_sLibrary, sizeof(m_sLibrary)-1, FF_Required);
    CreateStr("Group", m_sGroup, sizeof(m_sGroup)-1, FF_Required);
    //CreateVal("DoGraphics", &m_sDoGraphics, ADOX::adBoolean, FF_Required);
    //CreateVal("DoModels", &m_sDoModels, ADOX::adBoolean, FF_Required);
    CreateStr("Page", m_sInsertPg, sizeof(m_sInsertPg)-1, FF_None);
    CreateVal("InsertX", &m_Insert.m_X, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("InsertY", &m_Insert.m_Y, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("InsertZ", &m_Insert.m_Z, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("ScaleX", &m_Scale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("ScaleY", &m_Scale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("ScaleZ", &m_Scale.m_Z, ADOX::adDouble, FF_None, &DefValOne);

    CreateVal("TagX", &m_TagPt.m_X, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagY", &m_TagPt.m_Y, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagZ", &m_TagPt.m_Z, ADOX::adDouble, FF_None, &DefValNAN);
    CreateVal("TagScaleX", &m_TagScale.m_X, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleY", &m_TagScale.m_Y, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagScaleZ", &m_TagScale.m_Z, ADOX::adDouble, FF_None, &DefValOne);
    CreateVal("TagRotation", &m_TagRotation, ADOX::adDouble, FF_None, &DefValZero);
    CreateVal("TagVisible", &m_TagVisible, ADOX::adBoolean, FF_None);

    CreateVal("FixupRule", &m_iFixupRule, ADOX::adInteger, FF_None);
    CreateStr("FixupString1", m_sFixupString1, sizeof(m_sFixupString1)-1, FF_None);
    CreateStr("FixupString2", m_sFixupString2, sizeof(m_sFixupString2)-1, FF_None);
    CreateIndex("Library", false, false);
    bDoneInit = 1;
    }
  }

//===========================================================================
//
//
//
//===========================================================================
