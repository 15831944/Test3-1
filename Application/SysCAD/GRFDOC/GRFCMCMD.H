//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __GRFCMCMD_H
#define  __GRFCMCMD_H

#ifndef __ACCNODE_H
//  #include "accnode.h"
#endif

#ifndef __DOCROOT_H
  #include "docroot.h"
#endif

#ifndef __GRFWND_H
  #include "grfwnd.h"
#endif

#ifndef __SFE_CLNT_H
  #include "sfe_clnt.h"
#endif

#ifndef __PROJECT_H
  #include "project.h"
#endif

//===========================================================================

_FWDDEF(GrfComCmdBlk)
class GrfComCmdBlk : public DocCmdBlk, public CExecObj
  {
  public:
    Grf3D_Display *pDsp;
    CGrfWnd       *pGWnd;
    //COleClientDoc* pDoc;
    //CString        AutoXEC;
    GrfComCmdBlk  *pGrf;
    GrfComCmdBlk  *pMdl;

  public:
    GrfComCmdBlk(int ExtraCmds, int ExtraCIS);//pRootWnd Root_, pRootDoc Doc_);
    ~GrfComCmdBlk();
    virtual void   InitForCmd();
    virtual void   OnActivateDoc(flag bActivate);
    //virtual void   OpenVNT();
    virtual void   BuildVNT();

    void           DoActivate();
    //void           DoLoad();
    //void           DoSave();
    //void           DoClear();

  public: // Document Methods
    virtual flag   TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs);
    virtual flag   TestSymbolNameUnique(Strng &Sym, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueSym);

    virtual void   DumpErrors();
    virtual void   AttachDoc(pDocRoot pDoc_, pGrfComCmdBlk pGrf_, pGrfComCmdBlk pMdl_);
    virtual BOOL   DoReadDocument(const char *pszPathName, FILE* pFile);
    virtual BOOL   DoWriteDocument(const char *pszPathName, FILE* pFile);

  };

//===========================================================================

#endif
