//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __FLWCMDS_H
#define  __FLWCMDS_H


#ifndef __ACCNODE_H
//  #include "accnode.h"
#endif

#ifndef __GRFCMCMD_H
  #include "grfcmcmd.h"
#endif

#ifndef __SFE_CLNT_H
  #include "sfe_clnt.h"
#endif

//===========================================================================

_FWDDEF(FlwCmdBlk)
class FlwCmdBlk : public GrfComCmdBlk
  {
  public:
    Strng          DataFmt;
    
    //Strng          EngineName;
    //SFEFlwLib     *pFE;
    EqnSlvCtrlBlk  EQCtrl;

  public:
    FlwCmdBlk(int ExtraCmds, int ExtraCIS);
    ~FlwCmdBlk();
    virtual void   InitForCmd();
    virtual void   OnActivateDoc(flag bActivate);
    //virtual void   OpenVNT();
    virtual void   BuildVNT();

    //void           OpenEngine();
    //void           CloseEngine();
    flag           EngineOpen();
    
    //SFEFlwLib     *FE();

    void           DoConfig();
    void           DoList();
    void           DoSetListFmt(); 

    void           DoReport(); 
    void           DoSolve(); 
    //void           DoBalance(); 
    //void           DoOptimise(); 
    //void           DoSetValues();
    //void           DoGetValues();

    //void           DoLoadV70();
    void           DoLoadUserDocs();

    virtual void   DumpReplies();
    virtual void   DumpErrors();
  
  public: // Document Methods
    virtual flag   TestModelTagUnique(pchar pTag, CUniqueTagRulesBlk & TRB/*pchar pHdr, char SepChar*/, Strng &UniqueTag, flag HdrAsIs);
    virtual flag   TestSymbolNameUnique(Strng &Sym, pchar pHdr, char SepChar, Strng &UniqueSym);
    virtual BOOL   DoReadDocument(const char *pszPathName, FILE* pFile);
    virtual BOOL   DoWriteDocument(const char *pszPathName, FILE* pFile);

    // CExecObj Overridables Etc
    //XInfoList      XData;

    /*
    virtual flag   EO_QueryTime(const rXB_TimeControl CB, double &TimeRqd, double &dTimeRqd);
    virtual flag   EO_Start(const rXB_TimeControl CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
    */
    /*
    virtual flag   EO_BeginSave(FilingControlBlock &FCB);
    virtual flag   EO_SaveTag(FilingControlBlock &FCB, CXMsgLst &XM);
    virtual flag   EO_SaveConn(FilingControlBlock &FCB, CXMsgLst &XM);
    virtual flag   EO_EndSave(FilingControlBlock &FCB);
    virtual flag   EO_BeginLoad(FilingControlBlock &FCB);
    virtual flag   EO_LoadTag(FilingControlBlock &FCB, CXMsgLst &XM);
    virtual flag   EO_LoadConn(FilingControlBlock &FCB, CXMsgLst &XM);
    virtual flag   EO_EndLoad(FilingControlBlock &FCB);
    */
  };

//===========================================================================

#endif
