//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DOCROOT_H
#define  __DOCROOT_H

#ifndef _GPFUNCS_H
  #include "gpfuncs.h"
#endif
#ifndef _CMDMENU_H
  #include "cmdmenu.h"
#endif

//===========================================================================

_FWDDEF(DocRoot)
_FWDDEF(DocCmdBlk)

//===========================================================================

class DocCmdBlk : public CommandBlk
  {
  public:
    //pGrf3D_Display pDsp;
    CView*         pWnd;
    //COleClientDoc* pDoc;
    pDocRoot       pDoc;
    CString        AutoXEC;
    //pGrfCmdBlk     pGrf;
    //pGrfCmdBlk     pMdl;

  public:
    DocCmdBlk(int ExtraCmds, int ExtraCIS);//pRootWnd Root_, pRootDoc Doc_);
    ~DocCmdBlk();
    virtual void   InitForCmd();
    virtual void   OnActivateDoc(flag bActivate);
    //virtual void   OpenVNT();
    virtual void   BuildVNT();

    //void           DoActivate();
    void           DoNew();
    void           DoOpen();
    void           DoSave();
    void           DoClose();
    void           DoClear();

  public: // Document Methods
    //virtual pNodeGrfInfo GetNodeDrawings();
    //virtual int    AddNodeModel(pchar ModelType, pchar Tag);
    //virtual int    DeleteNodeModel(pchar Tag);
    //virtual int    AddNodeConnect(pchar Type, pchar Tag, pchar SrcTag, pchar SrcOut, pchar DstTag, pchar DstIn);
    virtual flag   TestModelTagUnique(pchar pTag, pchar pHdr, char SepChar, Strng &UniqueTag, flag HdrAsIs) { return true; };
    //virtual void   DumpErrors();
    virtual void   AttachDoc(pDocRoot pDoc_);//, pDocCmdBlk pGrf_, pDocCmdBlk pMdl_);
    virtual BOOL   OpenDocument(const char *pszPathName, FILE* pFile);
    virtual BOOL   SaveDocument(const char *pszPathName, FILE* pFile);

  };

//===========================================================================

class DocRoot : public CDocument
  {
  DECLARE_SERIAL(DocRoot)
protected:
  DocRoot();      // protected constructor used by dynamic creation
  static pDocRoot pFirstDoc;
  pDocRoot        pNextDoc;

public:
  flag            bIsOpen;
  long            lVNTChkSum; //

  static pDocRoot FirstDoc() { return pFirstDoc; };
  pDocRoot        NextDoc() { return pNextDoc; };

public:
  virtual void OnAttachDoc();
  virtual void OnActivate(flag bActivate);//CView* pWnd, pGrf3D_Display pDsp);
  virtual void SetTitle(LPCTSTR lpszTitle);
  virtual void SetPathName(LPCTSTR pszPathName, BOOL bAddToMRU = TRUE);
  virtual void OpenVNT();
 
protected:
  virtual ~DocRoot();
  virtual BOOL OnNewDocument();                            // standard MFC methods
  virtual BOOL OnOpenDocument(const char *pszPathName);    // mapped to command line commands
  virtual void OnCloseDocument();
  virtual BOOL OnSaveDocument(const char *pszPathName);
  virtual void DeleteContents();
  void             SetDocFirst();
  void             AddDoc();
  void             RemoveDoc();

public:                // Grf Commands
  virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
  virtual BOOL OpenDocument(const char* pszPathName);     // do the actual work
  virtual BOOL ReadDocument(const char* pszPathName, FILE* pFile); 
  virtual BOOL CloseDocument();
  virtual BOOL SaveDocument(const char* pszPathName);
  virtual BOOL WriteDocument(const char* pszPathName, FILE* pFile); 
  virtual void UpdateAllViewsByMsg(CView* pSender, LPARAM lHint, CObject* pHint, BOOL PostMsg=FALSE);

  virtual flag TestModelTagUnique(pchar pTag, pchar pHdr, char SepChar, Strng &UniqueTag, flag HdrAsIs) { return true; };

  virtual int GetTagList(Strng_List & Lst) { return 0; };

protected:
  //{{AFX_MSG(DocRoot)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//===========================================================================

class OleDocRoot : public COleLinkingDoc
  {
  DECLARE_SERIAL(OleDocRoot)
protected:
  OleDocRoot();      // protected constructor used by dynamic creation
//  static pOleDocRoot pFirstDoc;
//  pOleDocRoot        pNextDoc;

public:
  flag            bIsOpen;
  CString         m_strReqdPathName; 
//  long            lVNTChkSum; //

//  static pOleDocRoot FirstDoc() { return pFirstDoc; };
//  pOleDocRoot        NextDoc() { return pNextDoc; };

public:
//  virtual void OnAttachDoc();
  virtual void OnActivate(flag bActivate);//CView* pWnd, pGrf3D_Display pDsp);
  virtual void SetTitle(LPCTSTR lpszTitle);
  virtual void SetPathName(LPCTSTR pszPathName, BOOL bAddToMRU = TRUE);
//  virtual void OpenVNT();
 
protected:
  virtual ~OleDocRoot();
  virtual BOOL OnNewDocument();                            // standard MFC methods
  virtual BOOL OnOpenDocument(const char *pszPathName);    // mapped to command line commands
  virtual void OnCloseDocument();
  virtual BOOL OnSaveDocument(const char *pszPathName);
  virtual void DeleteContents();
//  void             SetDocFirst();
//  void             AddDoc();
//  void             RemoveDoc();

public:                // Grf Commands
//  virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
//  virtual BOOL OpenDocument(const char* pszPathName);     // do the actual work
//  virtual BOOL ReadDocument(const char* pszPathName, FILE* pFile); 
//  virtual BOOL CloseDocument();
//  virtual BOOL SaveDocument(const char* pszPathName);
//  virtual BOOL WriteDocument(const char* pszPathName, FILE* pFile); 
  virtual void UpdateAllViewsByMsg(CView* pSender, LPARAM lHint, CObject* pHint, BOOL PostMsg=FALSE);

  virtual flag TestModelTagUnique(pchar pTag, pchar pHdr, char SepChar, Strng &UniqueTag, flag HdrAsIs) { return true; };

  virtual int GetTagList(Strng_List & Lst) { return 0; };

protected:
  //{{AFX_MSG(OleDocRoot)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//===========================================================================

#endif
