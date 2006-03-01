//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
//

#include "stdafx.h"

#define  __BASECMDS_CPP

#include "sc_defs.h"
#include "syscad.h"
#include "gpfuncs.h"
#include "gpwfuncs.h"
#include "dlgbusy.h"
#include "fixedit.h"

#include "drwhdr.h"
#include "cmdmenu.h"
#include "basecmds.h"
#include "project.h"
#include "executiv.h"
#include "resource.h"

#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

BaseCmdBlk::BaseCmdBlk() :
  CommandBlk(35, 15)  
  {
  StepTime=300.0;
  }

//---------------------------------------------------------------------------

BaseCmdBlk::~BaseCmdBlk()
  {
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::BuildVNT()
  {
  //TRACE("Open Base VNT\n");

  //SETVNT("NEw",      "DOCument",   AName,   -1, (CmdFn)DoNewDoc,       EF_ERS);
  //SETVNT("NEw",      "PRoject",    N_A,     -1, (CmdFn)DoNewPrj,       EF_ERS);
  //SETVNT("OPen",     "PRoject",    AName,   -1, (CmdFn)DoOpenPrj,      EF_ERS);
  //SETVNT("CLose",    "PRoject",    N_A,     -1, (CmdFn)DoClosePrj,     EF_ERS);
  //SETVNT("SAve",     "PRoject",    AName,   -1, (CmdFn)DoSavePrj,      EF_ERS);
  //SETVNT("CONFigure","PRoject",    N_A,     -1, (CmdFn)DoCfgPrj,       EF_ERS);
  //SETVNT("LOad",     "DAtabase",   AName,   -1, (CmdFn)DoLoadDB,       EF_ERS);
  //SETVNT("LOad",     "SNapshot",   AName,   -1, (CmdFn)DoLoadSnapShot, EF_ERS);
  //SETVNT("SAve",     "DAtabase",   AName,   -1, (CmdFn)DoSaveDB,       EF_ERS);
  //SETVNT("SAve",     "SNapshot",   AName,   -1, (CmdFn)DoSaveSnapShot, EF_ERS);
  //SETVNT("SEt",      "DEBUG_AT",   "OFf ON ",-1, (CmdFn)SetDebugAt,    EF_ERS);
  //SETVNT("SEt",      "F1",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F2",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F3",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F4",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F5",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F6",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F7",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F8",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F9",         AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F10",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F11",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F12",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F13",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F14",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F15",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SEt",      "F16",        AName,   -1, (CmdFn)SetFnKey,       EF_ERS);
  //SETVNT("SHUTdown", N_A,          N_A,     -1, (CmdFn)DoShutDown,     EF_ERS);
  //SETVNT("RUn",      "Syscad",     "Step For Until",  -1, (CmdFn)DoRun,        EF_ERS);
  //SETVNT("STOp",     "Syscad",     N_A,     -1, (CmdFn)DoStop,        EF_ERS);
  //SETVNT("STEp",     "Syscad",     N_A,     -1, (CmdFn)DoStep,        EF_ERS);
  ////SETVNT("SOlve",    "NetFlows",   N_A,     -1, (CmdFn)DoNetSolve,    EF_ERS);
  //SETVNT("SYscad",   "SOlver",     N_A,     -1, (CmdFn)DoSolver,      EF_ERS);
  //SETVNT("Optimse",  "Syscad",     N_A,     -1, (CmdFn)DoOptimise,    EF_ERS);


  //SETCIS(ID_PROJECT_NEW         , "NEW PROJECT\r");
  //SETCIS(ID_PROJECT_OPEN        , "OPEN PROJECT\r");
  //SETCIS(ID_PROJECT_CLOSE       , "CLOSE PROJECT\r");
  //SETCIS(ID_PROJECT_SAVE        , "SAVE PROJECT \r");
  //SETCIS(ID_PROJECT_SAVEAS      , "SAVE PROJECT *\r");
  //SETCIS(ID_PROJECT_CONFIGURE   , "CONFIGURE PROJECT\r");
  SETCIS(ID_SNAPSHOT_SAVE       , "SAVE SNAPSHOT\r");
  SETCIS(ID_SNAPSHOT_LOAD       , "LOAD SNAPSHOT\r");
  //SETCIS(ID_EXEC_Optimise       , "Optimse Syscad\r");
  }

//---------------------------------------------------------------------------

flag BaseCmdBlk::UpdateCmdStr(int nID, CCmdUI* pCmdUI)
  {
  return False;
  };

//---------------------------------------------------------------------------

flag BaseCmdBlk::ProcessCmdStr(int nID, rStrng Str)
  {
  switch (nID)
    {
    case ID_EXEC_SolveNet:
       Str.Set("SOlve NetFlows Inoperative\r");
    }
  return True;
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::SetDebugAt()
  {
  switch (When)
    {
    case EX_EXEC:
//      dbgat_on=(Modifier==2);
      break;
    default:;
    }
  }

//---------------------------------------------------------------------------

void BaseCmdBlk::SetFnKey()
  {
  Strng S("");
  int i;
  switch (When)
    {
    case EX_MODIFIER_RDY:
      switch (Modifier)
        {
        case ModNoun:
          gs_pCmd->SetParmInfo(AName, 20, 1);
          break;
        }
      break;
    case EX_EXEC:
      for (i=2; i <= gs_pCmd->LastTokenIndex(); i++)
        S.Append("%s ", gs_pCmd->Token(i));
      gs_pCmd->SetFunctionKey(Noun,S());
      break;
    default:;
    }
  }

//---------------------------------------------------------------------------

void BaseCmdBlk::DoShutDown()
  {
  if (When==EX_EXEC)
    PostQuitMessage(0);
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoNewDoc()
  {
  VERIFY(0);
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoNewPrj()
  {
  if (When==EX_EXEC)
    gs_pPrj->OnNew();
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoOpenPrj()
  {
  if (When==EX_EXEC)
    {
    Strng PrjName(gs_pCmd->Token(2));  // Save Token : Close Project will Clear Tokens
    Strng Label(gs_pCmd->Token(3));
    gs_pPrj->OnOpen(PrjName());//, Label());
    }
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoClosePrj()
  {
  if (When==EX_EXEC)
    gs_pPrj->OnClose();
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoSavePrj()
  {
  if (When==EX_EXEC)
    {
    if (gs_Exec.Busy())
      {
      LogError("SysCAD", 0, "Must not be running");
      return;
      }
    if (gs_pCmd->LastTokenIndex() >= 2)
      {
      Strng PrjName(gs_pCmd->Token(2));  // Save Token : Close Project will Clear Tokens
      Strng Label(gs_pCmd->Token(3));
      gs_pPrj->OnSave(PrjName());//, Label());
      }
    else
      gs_pPrj->OnSave(NULL);//, NULL);
    }
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoCfgPrj()
  {
  if (When==EX_EXEC)
    gs_pPrj->OnConfigure();
  };

//---------------------------------------------------------------------------

//void BaseCmdBlk::DoLoadDB()
//  {
//  if (When==EX_EXEC)
//    {
//    dbgMemoryState("Load DB St");
//    Strng Label(gs_pCmd->Token(2));
//    gs_pPrj->DoLoadDB(Label());
//    dbgMemoryState("Load DB En");
//    }
//  };

//---------------------------------------------------------------------------
//
//void BaseCmdBlk::DoSaveDB()
//  {
//  if (When==EX_EXEC)
//    {
//    Strng Label(gs_pCmd->Token(2));
//    gs_pPrj->DoSaveDB(Label());
//    }
//  };
//
//---------------------------------------------------------------------------

void BaseCmdBlk::DoLoadSnapShot()
  {
  if (When==EX_EXEC)
    {
    dbgMemoryState("Load SnapShot St");
    Strng Label(gs_pCmd->Token(2));
    gs_pPrj->DoLoadSnapShotScenario(True, Label());
    dbgMemoryState("Load SnapShot En");
    }
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoSaveSnapShot()
  {
  if (When==EX_EXEC)
    {
    Strng Label(gs_pCmd->Token(2));
    gs_pPrj->DoSaveSnapShotScenario(True, Label());
    }
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoRun()
  {
  switch (When)
    {
    case EX_MODIFIER_RDY:
      switch (Modifier)
        {
        case ModNoun : 
                  Invalidate(EndTime); 
                  Invalidate(dTime);
                  //SteadyState=0;
                  break;
        case  1 : gs_pCmd->SetParmInfo(AFloat, 1); break; // Dt
        case  2 : gs_pCmd->SetParmInfo(AFloat, 1); break; // Dt
        case  3 : gs_pCmd->SetParmInfo(AFloat, 1); break; // Dt
        //se  4 : ToSteadyState=1;
        default:;
        }
      break;
    case EX_PARAMETER_RDY:
      switch (Modifier)
        {
        case  1 : gs_pCmd->dParm(StepTime); break; 
        case  2 : gs_pCmd->dParm(dTime); break;
        case  3 : gs_pCmd->dParm(EndTime); break; 
        default:;
        }
      break;
    case EX_EXEC:
      {
      if (Valid(EndTime))
        gs_Exec.SetStopTime(EndTime);
      else if (Valid(dTime))
        gs_Exec.SetStopTime(gs_Exec.Time()+dTime);
      else
        gs_Exec.SetStopTime(dNAN);

      if (Valid(StepTime))
        gs_Exec.SetStepSizeMax(StepTime);
      DoBreak();
      //gs_Exec.StartExec();
      }
      break;
    default:;  
    }  
  };

//---------------------------------------------------------------------------

void BaseCmdBlk::DoStop() 
  { 
  if (When==EX_EXEC) 
    DoBreak();
    //gs_Exec.Stop(); 
  };
void BaseCmdBlk::DoStep(CDoOneStepInfo Info) 
  { 
  if (When==EX_EXEC) 
    {/*gs_Exec.Step();*/}; 
  };
void BaseCmdBlk::DoSolver() 
  { 
  if (When==EX_EXEC) 
    gs_Exec.OpenSolver(); 
  };

//---------------------------------------------------------------------------
/*
void FlwCmdBlk::DoSolve()
  {
  if (When==EX_EXEC) ;
  if (!EngineOpen()) 
    return;
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC:
      {
      Strng Msg("SOLVE");
      BeginWaitCursor();
      pFE->ProcessMsg(Msg);
      EndWaitCursor();
      }
      break;
    case EX_RESULTS:
      DumpErrors();
      break;
    default:;  
    }  
  pDsp->Close();
  };

//---------------------------------------------------------------------------

void FlwCmdBlk::DoBalance()
  {
  if (When==EX_EXEC) ;
  if (!EngineOpen()) 
    return;
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC:
      {
      Strng Msg("BALANCE");
      if (pFE->ProBalInit(TCtrl)==0)
        {
        CBalanceDlg BalDlg(TCtrl, pFE);
        BalDlg.DoModal();
        }
//      BeginWaitCursor();
//      pFE->ProcessMsg(Msg);
//      EndWaitCursor();
      }
      break;
    case EX_RESULTS:
      {
      DumpReplies();
      DumpErrors();
      }
      break;
    default:;  
    }  
  pDsp->Close();
  };

//---------------------------------------------------------------------------

void FlwCmdBlk::DoOptimise()
  {
  if (When==EX_EXEC) ;
  if (!EngineOpen()) 
    return;
  pDsp->Open();
  switch (When)
    {
    case EX_EXEC:
      {
      Strng Msg("Optimise");
      if (pFE->ProBalInit(TCtrl)==0)
        {
        CBalanceDlg BalDlg(TCtrl, pFE);
        BalDlg.DoModal();
        }
//      BeginWaitCursor();
//      pFE->ProcessMsg(Msg);
//      EndWaitCursor();
      }
      break;
    case EX_RESULTS:
      {
      DumpReplies();
      DumpErrors();
      }
      break;
    default:;  
    }  
  pDsp->Close();
  };
*/
//---------------------------------------------------------------------------
