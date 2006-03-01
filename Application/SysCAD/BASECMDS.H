//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// basecmds.h 
//

//===========================================================================

_FWDDEF(BaseCmdBlk)

class BaseCmdBlk : public CommandBlk
  {
  public:
    BaseCmdBlk();
    ~BaseCmdBlk();
    
    double         dTime, EndTime, StepTime;
//    flag           ToSteadyState;

    //void           OpenEngine();
    //void           CloseEngine();
    void           SetDebugAt();
    void           SetFnKey();
    void           DoShutDown();

    void           DoNewPrj();
    void           DoOpenPrj();
    void           DoClosePrj();
    void           DoSavePrj();
    void           DoCfgPrj();

    void           DoNewDoc();

//    void           DoSaveDB();
//    void           DoLoadDB();

    void           DoSaveSnapShot();
    void           DoLoadSnapShot();

//  void           DoAdvSolve(flag DoAdvance);
//  void           DoAdvance(); 
//  void           DoSolve(); 
    void           DoRun(); 
    void           DoStop(); 
    void           DoStep(CDoOneStepInfo Info); 
    //void           DoNetSolve(); 
    void           DoSolver(); 
    //void           DoOptimise(); 

    virtual void   BuildVNT();
    virtual flag   UpdateCmdStr(int nID, CCmdUI* pCmdUI);
    virtual flag   ProcessCmdStr(int nID, rStrng Str);
  };

//---------------------------------------------------------------------------

extern pBaseCmdBlk gs_pBaseCmds;

//===========================================================================
