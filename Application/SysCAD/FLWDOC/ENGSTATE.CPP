//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
  
#define  __ENGSTATE_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "engstate.h"
#if WITHSFEWND 
#include "cmdmenu.h"
#include "gpwfuncs.h"
#include "prjexec.h"

//===========================================================================

#define  DEFAULT_RAC "DEFAULT.RAC"

IMPLEMENT_DYNCREATE(EngStateWnd, FxdEdtView);

//===========================================================================

EngStateWnd::EngStateWnd()
  {
  Chnged=False;
  EO_Register("StateWnd", EOWrite_Msg, /*Pri*/0,/*Stack*/0);
  }

//---------------------------------------------------------------------------

EngStateWnd::~EngStateWnd()
  {
  EO_DeRegister();
  }

//---------------------------------------------------------------------------

const int ESID_Method   = 1;
const int ESID_Control  = 2;
const int ESID_Time     = 3;
const int ESID_Target   = 4;
const int ESID_Stop     = 5;
const int ESID_dTime    = 6;
const int ESID_MultO    = 7;
const int ESID_MultI    = 8;
const int ESID_LoadO    = 9;
const int ESID_LoadI    = 10;
const int ESID_IterG    = 11;
const int ESID_IterB    = 12;
const int ESID_ProcU    = 13;
const int ESID_IterT    = 14;
const int ESID_Worst    = 15;

//---------------------------------------------------------------------------

void EngStateWnd::StartBuild()
  {
  FxdEdtView::StartBuild();
  }

//---------------------------------------------------------------------------

void EngStateWnd::Build(int NRows, int NCols, int ColIndent, pvoid BuildData)
  {
  StartBlk(6+NWorstLines, 0, NULL);

  int f=0;
  SetDesc(0,"Method ", -1,  8, 0, "");
  pFld[f++]=SetData(0,"", ESID_Method, 14, 2, "   ", FALSE);
  SetDesc(0,"Time  ", -1,  6, 0, "");
  pFld[f++]=SetData(0,"", ESID_Time,   14, 2, "  ", FALSE);
  SetDesc(1,"Control", -1,  8, 0, "");
  pFld[f++]=SetData(1,"", ESID_Control,14, 2, "   ", FALSE);
  SetDesc(1,"Stop  ", -1,  6, 0, "");
  pFld[f++]=SetData(1,"", ESID_Stop,   14, 2, "  ", FALSE);
  SetDesc(2,"Good Iter", -1, 9, 0, " ");
  pFld[f++]=SetData(2,"", ESID_IterG,  12, 2, "   ", FALSE);
  SetDesc(2,"dTime    ", -1, 9, 0, " ");
  pFld[f++]=SetData(2,"", ESID_dTime,  10, 2, "sec", FALSE);
  SetDesc(3,"Bad  Iter", -1, 9, 0, " ");
  pFld[f++]=SetData(3,"", ESID_IterB,  12, 2, "   ", FALSE);
  SetDesc(3,"Iter Time", -1, 9, 0, " ");
  pFld[f++]=SetData(3,"", ESID_IterT,  10, 2, "sec", FALSE);
  SetDesc(4,"Processor", -1, 9, 0, " ");
  pFld[f++]=SetData(4,"", ESID_ProcU,  12, 2, "%  ", FALSE);
  SetDesc(4,"Rel Speed", -1, 9, 0, " ");
  pFld[f++]=SetData(4,"", ESID_MultI,  10, 2, "%  ", FALSE);

  for (int r=0; r<NWorstLines; r++)
    pFld[f++]=WorstFld[r]=SetData(6+r," ", ESID_Worst+r, 48, 0, "", FALSE);
  
  FxdEdtView::Build(0, 0, 0);
  }

//---------------------------------------------------------------------------

void EngStateWnd::Load(FxdEdtInfo &EI, Strng & Str)
  {
  //long Index1;
  if (CurrentBlk(EI))//RowId>=RowShift)
    {
    Str="";
    }
  };

//---------------------------------------------------------------------------

long EngStateWnd::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    //TRACE1("ParseDataSlot %3i", RowId+Index1-1);
    }
  return 0;
  };

//--------------------------------------------------------------------------

void EngStateWnd::Initialise()
  {
  };

//---------------------------------------------------------------------------

void EngStateWnd::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
  {
  FxdEdtView::OnActivateView(bActivate, pActivateView, pDeactiveView);
  gs_pCmd->SetFocus();
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(EngStateWnd, FxdEdtView)
  //{{AFX_MSG_MAP(EngStateWnd)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void EngStateWnd::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  FxdEdtView::OnDrawBack(GB, PgNo/*, 0*/, ClipRgn);
  }

//---------------------------------------------------------------------------

void EngStateWnd::OnDrawLine(rGDIBlk GB,int PgNo, int RowNo)
  {
  if (CRow(RowNo)->Visible(GB))
    {
    CRow(RowNo)->Write(GB);
    }
  }

//---------------------------------------------------------------------------

flag EngStateWnd::EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd) 
  {
  return True;
  };

//---------------------------------------------------------------------------

flag EngStateWnd::EO_Start(const CXM_TimeControl &CB)                      
  {
//  gs_TheRunMngr.DoEO_Starting();
  return True;
  };

//---------------------------------------------------------------------------

void EngStateWnd::EO_QuerySubsReqd(CXMsgLst &XM)                      
  {
  const pchar RqdTags[] = {"$Dyn.Method",
                           "$Dyn.Time", 
                           "$Dyn.StepSizeCtrl",
                           "$Dyn.StopTime", 
                           "$Dyn.GoodIters", 
                           "$Dyn.dTime", 
                           "$Dyn.BadIters",
                           "$Dyn.IterTime",
                           "PlantModel.Stats.ProcessorUseA", //"$Dyn.ProcessorUse",
                           "PlantModel.Stats.RunSpdAvg",     //"$Dyn.RunMultI", 
                           "$Dyn.WorstMagInt0", 
                           "$Dyn.WorstMagInt1", 
                           "$Dyn.WorstMagInt2", 
                           "$Dyn.WorstMagInt3", 
                           "$Dyn.WorstMagInt4", 
                           "$Dyn.WorstMagInt5", 
                           "$Dyn.WorstMagInt6", 
                           "$Dyn.WorstMagInt7", 
                           "$Dyn.WorstMagInt8", 
                           "$Dyn.WorstMagInt9", 
                           NULL};
  CXM_ReadIndexedData * pMsg=(CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);
  flag ReadAll=pMsg->ReadAll;
  long DataIndex=pMsg->Start ? 0 : pMsg->LastIndex+1;

  Strng Tg;
  XM.Clear();
  for (; RqdTags[DataIndex]; DataIndex++)
    {
//    Tg.Set("%s.%s",sFlwLibTag(), RqdTags[DataIndex]); 
    Tg=RqdTags[DataIndex]; 
    
    CXM_DataRequest DRqst(DataIndex, Tg(), TABOpt_AllInfoOnce/*|TABOpt_CnvInfo*/, XIO_In);
    //CXM_DataRequest DRqst(DataIndex, RqdTags[DataIndex], TABOpt_AllInfoOnce/*|TABOpt_CnvInfo*/, XIO_In);
    //TABBuff Buff; 
    //sprintf(Buff, "%s ACFS 2 I ", RqdTags[DataIndex]);
    if (!XM.PackMsg(DRqst))//"TR", DataIndex, Buff, strlen(Buff)))
      break;
    }
  };

//---------------------------------------------------------------------------

void EngStateWnd::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)                      
  {
  XM.Clear();
  };

//---------------------------------------------------------------------------

flag EngStateWnd::EO_ReadSubsData(CXMsgLst &XM)                      
  {
  flag DataRead=0;
  return DataRead;
  };

//---------------------------------------------------------------------------

flag EngStateWnd::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)                      
  {
  if (FirstBlock)
    {
    //dbgMemoryState("EngStateWnd");

//    PROCESS_MEMORY_COUNTERS PMC; 
//    PMC.cb=sizeof(PMC);
//    GetProcessMemoryInfo(GetProcessHandle(), &PMC, sizeof(PMC));

    }

  CClientDC DC(this);
  CDCResChk ResChk(DC);

  InitDC(&DC);
  ScrGB.Attach(&DC, NULL);
  
  static Strng GoodStr("0");
  static Strng BadStr("0");
  for (long i=0; i<XM.NoMsgs(); i++)
    {
    CXM_ObjectData* pMsg = XM.ObjectData();
    long DataIndex=pMsg->Index;
    CPkDataItem* pItem=pMsg->FirstItem();
    PkDataUnion* pData=pMsg->FirstItem()->Value();
    if (IsData(pData->Type()))
      {
      if (pItem->Contains(PDI_CnvInfo))
        {
        CnvTxt[DataIndex]=pItem->CnvTxt();
        iCnv[DataIndex]=pItem->CnvIndex();
        }
      switch (DataIndex)
        {
        case 1:
        case 3:
          if (Valid(pData->GetDouble()))
            {
            char buff[256];
            SecstoHMSDate(pData->GetDouble(), buff, TimeCB().m_bSyncWithClock);
            CPg->SetFieldValue(ScrGB, pFld[DataIndex], buff); 
            }
          else
            CPg->SetFieldValue(ScrGB, pFld[DataIndex], ""); 
          break;
        case 0:
        case 2:
          if ((IsIntData(pItem->Type()) || IsUnSgnData(pItem->Type())) && pItem->Contains(PDI_StrList))
            {
            if (pItem->IndexedStrList())
              {
              Strng_List StrLst;
              pItem->GetStrList(StrLst);
              int i=pItem->Value()->GetLong();
              for (pStrng p= StrLst.First(); p; p=p->pNxt)
                if (p->Index()==i)
                  {
                  CPg->SetFieldValue(ScrGB, pFld[DataIndex], p->Str());
                  break;
                  }
              }
            }
          break;
        default:
          {
          char* pStr=pData->GetString("%i", "%5.2f", iCnv[DataIndex], CnvTxt[DataIndex].Str());
          CPg->SetFieldValue(ScrGB, pFld[DataIndex], pStr);//SDI.pVal);//buff); 
          switch (DataIndex)
            {
            case 5: 
              //pStatusBar->UpdateIndicator(2, SDI.pVal, TRUE); 
              break; // dTime
            case 4://7: 
            case 6://8: 
              {
              if (DataIndex==4)
                GoodStr=pStr; // Good Steps
              else
                BadStr=pStr;  // Bad Steps
              char Buff[256];
              strcpy(Buff, GoodStr());
              strcat(Buff, "/"); 
              strcat(Buff, BadStr());
              pStatusBar->UpdateIndicator(3, Buff, TRUE); 
              break; // Good/Bad Steps
              }
            }
          }
        }
      }
    }
  ScrGB.Detach();
  return True;
  };

//---------------------------------------------------------------------------

flag EngStateWnd::EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet) 
  {
  //gs_TheRunMngr.DoEO_Execute();
  return False;
  };

//---------------------------------------------------------------------------

flag EngStateWnd::EO_Stop(const CXM_TimeControl &CB)                       
  {
  return True;
  };

//===========================================================================

IMPLEMENT_DYNCREATE(CEngStateFrm, CMDIChildWnd)

CEngStateFrm::CEngStateFrm()
  {
  }

//---------------------------------------------------------------------------

CEngStateFrm::~CEngStateFrm()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CEngStateFrm, CMDIChildWnd)
  //{{AFX_MSG_MAP(CEngStateFrm)
  ON_WM_GETMINMAXINFO()
  ON_WM_CLOSE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CEngStateFrm::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
  {
  CMDIChildWnd::OnGetMinMaxInfo(lpMMI);

  EngStateWnd* p=(EngStateWnd*)GetTopWindow();
  if (p)
    {
    //adjust for frame...
    CRect Cr, Wr;
    GetWindowRect(&Wr);
    GetClientRect(&Cr);
    int w = (Wr.right-Wr.left) - (Cr.right-Cr.left);
    int h = (Wr.bottom-Wr.top) - (Cr.bottom-Cr.top);
    if (w==0)
      {//window was probably minimized, put any big numbers to allow for frame...
      w = 12;
      h = 48;
      }
    lpMMI->ptMaxTrackSize.x = (int)p->ScrGB.LuDataSize.cx + w + (int)p->ScrGB.LuBorder.cx;
    lpMMI->ptMaxTrackSize.y = (int)p->ScrGB.LuDataSize.cy + h + (int)p->ScrGB.LuBorder.cy;
    }
  }

//---------------------------------------------------------------------------

void CEngStateFrm::OnClose() 
  {
  //CMDIChildWnd::OnClose(); Do NOT call base, we are not allowed to close this window!
  }

//---------------------------------------------------------------------------

#endif
