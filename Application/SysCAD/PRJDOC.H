//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//#ifdef __PRJDOC_CPP
//  #define DllImportExport DllExport
//#elif !defined(SCAPP1)
//  #define DllImportExport DllImport
//#else
  #define DllImportExport
//#endif

class DllImportExport CPrjDoc : public CDocument
  {
  protected:
    CPrjDoc();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CPrjDoc)
  public:
    flag bAllowClose:1,
         bOnCloseBusy:1;
    //{{AFX_VIRTUAL(CPrjDoc)
    public:
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    virtual void OnCloseDocument();
    protected:
    virtual BOOL OnNewDocument();
    //}}AFX_VIRTUAL
  public:
    virtual ~CPrjDoc();
    #ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
    #endif
  protected:
    //{{AFX_MSG(CPrjDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

#undef DllImportExport 
