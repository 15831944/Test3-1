//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GRFDOC_H
#define  __GRFDOC_H

#ifndef _DOCROOT_H
#include "docroot.h"
#endif
#ifndef _GRFCMDS_H
#include "grfcmds.h"
#endif
#ifndef _FLWCMDS_H
#include "flwcmds.h"
#endif
#ifndef __DBBASE_H
#include "dbbase.h"
#endif
//#include "neutralgrf.h"

//---------------------------------------------------------------------------

_FWDDEF(CGrfDoc)
class CGrfDoc : public DocRoot
  {
  DECLARE_SERIAL(CGrfDoc)
  protected:
    CGrfDoc();      // protected constructor used by dynamic creation
  public:
//    InsCmdBlk      ICB;
    GrfCmdBlk      GCB;
    FlwCmdBlk      FCB;
    GrfComCmdBlk*  pCurrentCB;
    bool           bModelsActive;
    Strng          m_sDocGUID;

  public:
    virtual void OnAttachDoc();
    virtual void OnActivate(flag bActivate);
    virtual void OpenVNT();
  protected:
    virtual ~CGrfDoc();
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(const char* pszPathName);
    //virtual void OnCloseDocument();
    //virtual BOOL OnSaveDocument(const char *pszPathName);
    virtual void SetPathName( const char* pszPathName );

  public:
    //virtual BOOL DoClearDocument();
    //virtual BOOL DoLoadDocument(const char* pszPathName);
    //virtual BOOL DoSaveDocument(const char* pszPathName);
    virtual BOOL ReadDocument(const char* pszPathName, FILE* pFile);
    virtual BOOL WriteDocument(const char* pszPathName, FILE* pFile);

    virtual int GetTagList(Strng_List & Lst);
    virtual flag TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs) 
      { return pCurrentCB->TestModelTagUnique(pTag, TRB/*pHdr, SepChar*/, UniqueTag, HdrAsIs); };

    virtual int ClearSelectedTags();
    virtual int GetSelectedTags(Strng_List & Lst);
  
    //void ExportGraphicsData(CNeutralGrfImportExport & EGH);
  //protected:
    int  GetTagListInfo(bool AllInserts, CGrfTagInfoArray & GTIA); 
    //{{AFX_MSG(CGrfDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

#endif
