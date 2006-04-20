//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SolveDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "scexec.h"
//#include "scd_wm.h"
//#include "executiv.h"
#include "dbgmngr.h"
//#include "errorlog.h"
//#include "slvcfg.h"
#include "SlvTool.h"
#include "SlvState.h"
#include "executiv.h"

#define dbgSolveDlg  01

#if dbgSolveDlg  
static CDbgMngr dbgSlvThread      ("SolveTool", "SlvThread");
static CDbgMngr dbgKeepTempFiles  ("SolveTool", "KeepTempFiles");
static CDbgMngr dbgXRefs          ("PGM",       "XRefs");
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// --------------------------------------------------------------------------
// Forward declaration

//UINT SolveThread(LPVOID pSlvTool_);

// --------------------------------------------------------------------------

#define SetWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) | BS_DEFPUSHBUTTON);
#define ClrWndDef(h) ::SetWindowLong(h, GWL_STYLE, ::GetWindowLong(h, GWL_STYLE) & ~BS_DEFPUSHBUTTON);

/////////////////////////////////////////////////////////////////////////////
// CSolveTool dialog

//CSolveTool::CSolveTool(CWnd* pParent /*=NULL*/)
//	: CDialog(CSolveTool::IDD, pParent),
//  Exec(*pExec)
//  {
//	//{{AFX_DATA_INIT(CSolveTool)
//	//}}AFX_DATA_INIT
//
//  wBtnState=0;
//
//  Exec.SwapToProBal(); 
//
//  //pSolveThread=NULL;
//  //bStopReqd=0;
//  //bThreadStuffBusy=0;
//  //bSolutionBusy=0;
//  //bSolutionPaused=0;
//
//  iOptMeth=CMM_None;//CMM_Controls;
//
//  Slv().Targets.SetSize(2);
//  Slv().Targets[0].Exp.sExp="[P_4.Qm]";
//  Slv().Targets[0].Val.sExp="15.0";
//  Slv().Targets[1].Exp.sExp="[P_2.Qm]";
//  Slv().Targets[1].Val.sExp="10.0";
//  //Slv().Targets.SetSize(2);
//  //Slv().Targets[1].sExp[0]="GetTag(\"P_X.Qm\")";
//  //Slv().Targets[1].sExp[1]="10.0";
//
//  Slv().Params.SetSize(2);
//  Slv().Params[0].Exp.sExp="[XPG_1.Qm_Rqd]";
//  Slv().Params[0].Min.sExp="0.0";
//  Slv().Params[0].Max.sExp="20.0";
//  Slv().Params[1].Exp.sExp="[X_1.AGM.A01.Split]";
//  Slv().Params[1].Min.sExp="0";
//  Slv().Params[1].Max.sExp="1";
//
//  //UpdateTick=0;
//  bProbStarted=0;
//  bOneClick=0;
//  bOneClickBusy=0;
//  
//  bChanged = 0;
//  EqnCB().fConfigOK = 0;
//  EqnCB().fConverged = 0;
//  bOptConverged = 0;
//  
//  m_Routes.SetSize(0);
//
//  if (!Create(CSolveTool::IDD, pParent))//, WS_VISIBLE | WS_OVERLAPPEDWINDOW))
//    {
//    TRACE("Failed to create SolveTool\n");
//    }
//  EO_Register("SolveDlg", EOExec_None, /*Pri*/0,/*Stack*/0);
//
//  Strng Fn(PrjDirectory());
//  Fn+=OPT_FILE;
//  ProfFn.SetProfFilename(Fn());
//
//  CfgName="Default";
//  LoadCurrentCfgName(CfgName);
//  LoadConfiguration(CfgName);
//
//  CRect WinRect, wr1, wr2, mwr;
//  GetWindowRect(&WinRect);
//  GetDlgItem(IDB_SLV_ADV)->GetWindowRect(&wr1);
//  GetDlgItem(IDC_WORSTERRS)->GetWindowRect(&wr2);
//  WndRightShift=wr2.right-wr1.right;
//
//  AfxGetMainWnd()->GetWindowRect(&mwr);
//  int dx=((mwr.right-mwr.left)-(WinRect.right-WinRect.left-WndRightShift))/2-WinRect.left;
//  int dy=((mwr.bottom-mwr.top)-(WinRect.bottom-WinRect.top))/2-WinRect.top;
//  //int dx=0, dy=0;
//  int x=ProfFn.RdInt("Solver", "DX", -10000);
//  int y=ProfFn.RdInt("Solver", "DY", -10000);
//  if (x>-10000)
//    {
//    int xw=WinRect.right-WinRect.left;
//    int yw=WinRect.bottom-WinRect.top;
//
//    WinRect.right =mwr.left+x+xw;
//    WinRect.left  =mwr.left+x;
//    WinRect.bottom=mwr.top+y+yw;
//    WinRect.top   =mwr.top+y;
//    }
//  else
//    {
//    WinRect.right+=dx;
//    WinRect.left+=dx;
//    WinRect.top+=dy;
//    WinRect.bottom+=dy;
//    }
//
//  RECT CR;
//  AfxGetMainWnd()->GetTopWindow()->GetClientRect(&CR);
//  int MaxX=CR.right-(2*GetSystemMetrics(SM_CXSIZE)+GetSystemMetrics(SM_CXSIZEFRAME));
//  int MaxY=CR.bottom-(GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYSIZEFRAME));
//  
//  if (WinRect.left>MaxX)
//    {
//    int dx=MaxX-WinRect.left;
//    WinRect.right+=dx;
//    WinRect.left+=dx;
//    }
//  if (WinRect.top>MaxY)
//    {
//    int dy=MaxY-WinRect.top;
//    WinRect.top+=dy;
//    WinRect.bottom+=dy;
//    }
//
//  MoveWindow(&WinRect, False);
//
//  //WndRightShift*=-1;
//  ToggleWindowWidth(False); // Always make it Narrow
//
//  if (ProfFn.RdInt("Solver", "Adv", -1)>0) // Expand if neccessary
//    ToggleWindowWidth(False);
//
//  ShowWindow(SW_SHOW);  
//  SetStateInfo();
//
//  bOneClick=ProfFn.RdInt("Solver", "OneClick", 0);
//  if (bOneClick)
//    PostMessage(WM_COMMAND, IDB_SLV_STRTSTOP, 0);
//  
//  }
//
//// --------------------------------------------------------------------------
//
//void CSolveTool::ToggleWindowWidth(flag RePaint)
//  {
//  CRect WinRect;
//  GetWindowRect(&WinRect);
//  WndRightShift *= -1;
//  WinRect.right += WndRightShift;
//  MoveWindow(&WinRect, RePaint);
//
//  CString Txt;
//  Txt= WndRightShift < 0 ? "Errors >>" : "<< Basic";
//  GetDlgItem(IDB_SLV_ADV)->SetWindowText((const char *)Txt);
//
//  }
//
//// --------------------------------------------------------------------------
//
//void CSolveTool::OnDestroy() 
//  {
//	CDialog::OnDestroy();
//	
//	// TODO: Add your message handler code here
//  CRect WinRect, mwr;
//  GetWindowRect(&WinRect);
//  AfxGetMainWnd()->GetWindowRect(&mwr);
//  int dx=WinRect.left-mwr.left;
//  int dy=WinRect.top-mwr.top;
//
//  ProfFn.WrInt("Solver", "Adv", Sign(WndRightShift));
//  ProfFn.WrInt("Solver", "DX", dx);
//  ProfFn.WrInt("Solver", "DY", dy);
//  ProfFn.WrInt("Solver", "OneClick", bOneClick);
//	
//  //gs_Exec.CB.bSolvingPB=False;
//  gs_Exec.ReturnFromProBal();
//
//  }
//
//// --------------------------------------------------------------------------
//
//BOOL CSolveTool::OnInitDialog() 
//  {
//	CDialog::OnInitDialog();
//	
//  SetButtonsForStart(False);
//
//	return FALSE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
//  }
//
//// --------------------------------------------------------------------------
//
//void CSolveTool::SetStateInfo()
//  {
//  char Buff[256];
//  char *pRqd="";
//  if (Exec.Waiting())
//    pRqd=" - Paused";
//  else if (Exec.Stopping())//bStopReqd)
//    pRqd=" - Stopping";
//  else if (Exec.Running())//bSolutionBusy)
//    pRqd=" - Solving";
//
//  GetWindowText(Buff, sizeof(Buff)-1);
//  char * p=strchr(Buff, ' ');
//  if (p)
//    strcpy(p, pRqd);
//  else
//    strcat(Buff, pRqd);
//  SetWindowText(Buff);
//  }
//
//// --------------------------------------------------------------------------
//
//void CSolveTool::DoDataExchange(CDataExchange* pDX)
//  {
//	CDialog::DoDataExchange(pDX);
//	
//  if (pDX->m_bSaveAndValidate)
//    {
//    }
//  else
//    {
//    //char S[5][50], Buff[80];
//    char Buff[80];
//    CProgressCtrl * pProg1 =(CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
//    CProgressCtrl * pProg2 =(CProgressCtrl *)GetDlgItem(IDC_PROGRESS2);
//    if (bProbStarted)
//      {
//      double Secs=Slv().RunSW.Secs();
//      int iMins=((int)Secs)/60;
//      int iSecs=((int)Secs)%60;
//
//      //sprintf(S[0], "Iterations\t:\t%i",EqnCB().nIters);
//      //sprintf(S[1], "Solve Time\t:\t%i:%02i",iMins,iSecs);
//      //if (EqnCB().nBad>0)
//      //  sprintf(S[2], "Max Error\t:\t%*.*f %%",rel+1, rel-2, EqnCB().Worst[0].dErr*100.0);
//      //else 
//      //  sprintf(S[2], "Max Error\t:\t%s", EqnCB().nIters>0 ? "Solved":"");
//      //sprintf(S[3], "Converged\t:\t%i of %i",Max(0, (int)(nTears-EqnCB().nBad)),nTears);
//      //strcpy(S[4], "");
//
//      int nTears=EqnCB().nUnknowns;
//      int Decades = (int)(0.99+100.0*fabs(log10(Range(1.0e-9, EqnCB().Cfg.dEps_Rel, 0.1))));
//      int ErrXfrm = (int)(0.99+100.0*fabs(log10(Range(1.0e-9, EqnCB().Worst[0].dErr, 0.9))));
//      int rel=(int)(0.99+fabs(log10(Range(1.0e-9, EqnCB().Cfg.dEps_Rel, 0.1))));
//
//
//      sprintf(Buff, EqnCB().nIters>0 ? "%i" : "",EqnCB().nIters);
//      GetDlgItem(IDC_ITERATIONS)->SetWindowText(Buff);
//      sprintf(Buff, EqnCB().nIters>0 ? "%i:%02i" : "",iMins,iSecs);
//      GetDlgItem(IDC_SOLVETIME)->SetWindowText(Buff);
//
//      //pProg1->SetPos(Decades-ErrXfrm);
//      pProg1->SetPos(ErrXfrm);
//      pProg1->SetRange(0, Decades);
//      if (EqnCB().nBad>0)
//        //sprintf(Buff, EqnCB().nIters>0 ? "%*.*f %%":"",rel+1, rel-2, EqnCB().Worst[0].dErr*100.0);
//        sprintf(Buff, EqnCB().nIters>0 ? "%*.*f %%":"",rel+2, rel-1, EqnCB().Worst[0].dErr*100.0);
//      else 
//        sprintf(Buff, EqnCB().nIters>0 ? "Solved":"");
//      XStrTrim(Buff);
//      GetDlgItem(IDC_MAXERROR)->SetWindowText(Buff);
//      
//      if (nTears>0)
//        {
//        pProg2->SetPos(Max(0, (int)(nTears-EqnCB().nBad)));
//        pProg2->SetRange(0, nTears);
//        }
//      else
//        {
//        pProg2->SetPos(1);
//        pProg2->SetRange(0, 1);
//        }
//      sprintf(Buff, EqnCB().nIters>0 ? "%i of %i":"",Max(0, (int)(nTears-EqnCB().nBad)),nTears);
//      GetDlgItem(IDC_CONVERGED)->SetWindowText(Buff);
//      }
//    else 
//      {
//      if (Exec.Starting())//SbSolutionBusy)
//        sprintf(Buff, "Starting");
//      else
//        strcpy(Buff, "");
//      GetDlgItem(IDC_ITERATIONS)->SetWindowText(Buff);
//
//      strcpy(Buff, "");
//      GetDlgItem(IDC_ITERATIONS)->SetWindowText(Buff);
//      GetDlgItem(IDC_SOLVETIME)->SetWindowText(Buff);
//      GetDlgItem(IDC_MAXERROR)->SetWindowText(Buff);
//      GetDlgItem(IDC_CONVERGED)->SetWindowText(Buff);
//      }
//    
//    //CListBox * pStateLst=(CListBox *)GetDlgItem(IDC_STATE);
//    //pStateLst->SetRedraw(False);
//    //pStateLst->ResetContent();
//    //int STabStops[] = {10*4,12*4}; // colwide 14, 16, 18 ... 
//    //pStateLst->SetTabStops(2, STabStops);
//    //for (int j=0; j<4; j++)
//    //  pStateLst->InsertString(j, S[j]);
//    //pStateLst->SetRedraw(True);
//    //pStateLst->RedrawWindow(); 
//    
//    if (bProbStarted)
//      {
//      CListBox * pErrLst=(CListBox *)GetDlgItem(IDC_WORSTERRS);
//      pErrLst->SetRedraw(False);
//      pErrLst->ResetContent();
//      int ETabStops[] = {12*4, 24*4, 26*4}; // colwide 14, 16, 18 ... 
//      pErrLst->SetTabStops(3, ETabStops);
//      for (int i=0; i<Min(EqnCB().nBad, (long)MAX_EQNSLV_WORST); i++)
//        pErrLst->InsertString(i, EqnCB().Worst[i].cStr);
//      pErrLst->SetRedraw(True);
//      pErrLst->RedrawWindow();
//
//	    //m_ElapsedTime="1";
//	    //m_Error="2";
//	    //m_Iters="3";
//      }
//
//    }
//  
//  //{{AFX_DATA_MAP(CSolveTool)
//	//}}AFX_DATA_MAP
//  }
//
//// --------------------------------------------------------------------------
//
//BEGIN_MESSAGE_MAP(CSolveTool, CDialog)
//	//{{AFX_MSG_MAP(CSolveTool)
//	ON_BN_CLICKED(IDB_SLV_CFG, OnSlvCfg)
//	ON_BN_CLICKED(IDB_SLV_RPT, OnSlvRpt)
//	ON_BN_CLICKED(IDB_SLV_ADV, OnSlvAdv)
//	ON_WM_DESTROY()
//	ON_BN_CLICKED(IDB_SLV_STRTSTOP, OnSlvStrtstop)
//	ON_BN_CLICKED(IDB_SLV_PS_CN_DN, OnSlvPsCnDn)
//	ON_COMMAND(IDOK,OnOK)
//	ON_COMMAND(IDCANCEL,OnCancel)
//	ON_BN_CLICKED(IDB_SLV_INIT, OnSlvInit)
//	//}}AFX_MSG_MAP
//	ON_MESSAGE(WMU_PB_UPDATEDATA, OnUpdateDataByMsg)
//END_MESSAGE_MAP()
//
///////////////////////////////////////////////////////////////////////////////
//// CSolveTool message handlers
//
//void CSolveTool::OnOK() 
//  {
//  }
//
//void CSolveTool::OnCancel() 
//  {
//  //if (!bSolutionBusy)
//  //  OnSlvDone();
//  }
//
//
//void CSolveTool::OnSlvStrtstop() 
//  {
//  if (!Exec.Running())//bThreadStuffBusy)
//    {
//    Exec.SolvePB();
//    ////if (SolveCnvPage.Inited())
//    ////  SolveCnvPage.UpdateData(TRUE);
//    ////if (SolveCfgPage.Inited())
//    ////  SolveCfgPage.UpdateData(TRUE);
//    //
//    //bStopReqd=0;
//    //bThreadStuffBusy=1;
//    //if ((pSolveThread = AfxBeginThread(SolveThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 1000*1024L, CREATE_SUSPENDED))==NULL)
//    //  {
//    //  MessageBox("Error Creating Solver Thread!","",MB_OK);
//    //  VERIFY(0);
//    //  bThreadStuffBusy=0;
//    //  }
//    //else
//    //  {
//    //  pSolveThread->ResumeThread();
//    //  hSolveThread=pSolveThread->m_hThread;
//    //  TRACE("Solve hThread %x\n",hSolveThread);
//    //  }
//    }
//  else if (Exec.Running())//!bStopReqd)
//    {
//    Exec.Stop();
//    //bStopReqd=1;
//    //bSolutionPaused=0;
//    }
//  SetStateInfo();
//  }
//
//void CSolveTool::OnSlvPsCnDn() 
//  {
//  if (Exec.Running())
//    {
//    if (Exec.Waiting())
//      Exec.Continue();
//    else
//      Exec.Pause();
//    
//    //bSolutionPaused=!bSolutionPaused;
//    SetStateInfo();
//    }
//  else
//    {
//    if (bChanged)
//      if (AfxMessageBox("Save Present Configuration", MB_YESNO|MB_ICONQUESTION)==IDYES)
//        DoSave();
//
// 	  ASSERT_VALID(this);
//    DestroyWindow();
//    }
//  }
//
//void CSolveTool::OnSlvInit() 
//  {
//  CWaitCursor Wait;
//  Strng_List TagList;
//	Exec.SetModelState(MSA_PBInit, TagList);  
//  }
//
//void CSolveTool::OnSlvCfg() 
//  {
//	// TODO: Add your control notification handler code here
//  CSlvCfgSheet CfgSheet(this, "Solver Configuration", this, 0);
//  CfgSheet.DoModal();
//  SetStateInfo();
//  }
//
//void CSolveTool::OnSlvRpt() 
//  {
//	// TODO: Add your control notification handler code here
//	
//  }
//
//void CSolveTool::OnSlvAdv() 
//  {
//	// TODO: Add your control notification handler code here
//
//  ToggleWindowWidth();
//  }
//
//void CSolveTool::PostNcDestroy() 
//  {
//	// TODO: Add your specialized code here and/or call the base class
//
//  Exec.ReturnFromProBal(); 
//  
//  EO_DeRegister();  
//  delete pSolveTool;
//  pSolveTool=NULL;
//	
//	CDialog::PostNcDestroy();
//  }
//
////---------------------------------------------------------------------------
//
//CSolveTool * CSolveTool::pSolveTool=NULL;
//
////---------------------------------------------------------------------------
//
//void CSolveTool::Open()
//  {
//  if (pSolveTool==NULL)
//    pSolveTool=new CSolveTool(AfxGetMainWnd()); 
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::Close()
//  {
//  
//  };
//
////---------------------------------------------------------------------------
//
//const int NButtons=4;
//
//void CSolveTool::SetButtonsForStart(flag DefIsCancel)
//  {
//  CButton & StrtStop = *(CButton*)GetDlgItem(IDB_SLV_STRTSTOP);
//  CButton & ContDone = *(CButton*)GetDlgItem(IDB_SLV_PS_CN_DN);
//  CButton & InitBtn = *(CButton*)GetDlgItem(IDB_SLV_INIT);
//  CButton & CfgBtn= *(CButton*)GetDlgItem(IDB_SLV_CFG);
//  CButton & RptBtn= *(CButton*)GetDlgItem(IDB_SLV_RPT);
//
//  StrtStop.SetWindowText("Start");
//  ContDone.SetWindowText("Done");
//  InitBtn.EnableWindow(True);
//  CfgBtn.EnableWindow(True);
//  RptBtn.EnableWindow(True);
//
//  if (DefIsCancel)
//    {
//    ClrWndDef(StrtStop.m_hWnd);
//    SetWndDef(ContDone.m_hWnd);
//    }
//  else
//    {
//    SetWndDef(StrtStop.m_hWnd);
//    ClrWndDef(ContDone.m_hWnd);
//    }
//  
//  StrtStop.RedrawWindow();
//  ContDone.RedrawWindow();
//  InitBtn.RedrawWindow();
//
//  if (DefIsCancel)
//    ContDone.SetFocus();
//  else
//    StrtStop.SetFocus();
//  }
//
////---------------------------------------------------------------------------
//
//void CSolveTool::SetButtonsForStop()
//  {
//  CButton & StrtStop = *(CButton*)GetDlgItem(IDB_SLV_STRTSTOP);
//  CButton & ContDone = *(CButton*)GetDlgItem(IDB_SLV_PS_CN_DN);
//  CButton & InitBtn = *(CButton*)GetDlgItem(IDB_SLV_INIT);
//  CButton & CfgBtn= *(CButton*)GetDlgItem(IDB_SLV_CFG);
//  CButton & RptBtn= *(CButton*)GetDlgItem(IDB_SLV_RPT);
//
//  StrtStop.SetWindowText("Stop");
//  ContDone.SetWindowText(Exec.Waiting() ? "Continue" : "Pause");
//  InitBtn.EnableWindow(False);
//  CfgBtn.EnableWindow(Exec.Waiting());
//  RptBtn.EnableWindow(False);
//
//  SetWndDef(StrtStop.m_hWnd);
//  ClrWndDef(ContDone.m_hWnd);
//
//  StrtStop.RedrawWindow();
//  ContDone.RedrawWindow();
//  InitBtn.RedrawWindow();
//
//  StrtStop.SetFocus();
//  }
//
////---------------------------------------------------------------------------
//
//LRESULT CSolveTool::OnUpdateDataByMsg(WPARAM wParam, LPARAM lParam)
//  {
//  UpdateData(False);
//
//  if (wParam>=1 && wParam<=3)
//    wBtnState=wParam;
//
//  if (wParam!=0)
//    switch (wBtnState)
//      {
//      case 0: break;
//      case 1: SetButtonsForStop(); break;
//      case 2: SetButtonsForStart(False); break;
//      case 3: SetButtonsForStart(True); break; // Converged
//      }
//
//  char Buff[256];
//  char *pRqd="";
//  if (Exec.Waiting())
//    pRqd=" - Paused";
//  else if (Exec.Stopping())//bStopReqd)
//    pRqd=" - Stopping";
//  else if (Exec.Running())//bSolutionBusy)
//    pRqd=" - Solving";
//
//  GetWindowText(Buff, sizeof(Buff)-1);
//  char * p=strchr(Buff, ' ');
//  if (p)
//    strcpy(p, pRqd);
//  else
//    strcat(Buff, pRqd);
//  SetWindowText(Buff);
//
//  return 0;
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::UpdateOptMeth(byte iReqdOptMeth)
//  {
//  bChanged |= (iOptMeth!=iReqdOptMeth);
//  iOptMeth=iReqdOptMeth;
//  }
//
////===========================================================================
////
////
////
////===========================================================================
//
//void CSolveTool::FixCfgList(Strng & Name)
//  {
//  char Buffer[65536], Buffer2[65536];
//  char *pBuff, *pBuff2;
//
//  pBuff2=Buffer2;
//  pBuff2+=1+sprintf(pBuff2, "Cfg=%s", Name);
//  if (ProfFn.RdSection("Contents", Buffer, sizeof(Buffer))>0)	
//    {
//    pBuff=Buffer;
//    while (strlen(pBuff)>0)
//      {
//      pchar pCfg, pCfgName;
//      ProfFn.GetProfItem(pBuff, pCfg, pCfgName);
//      if(_stricmp(pCfgName, Name())!=0)
//        pBuff2+=1+sprintf(pBuff2, "Cfg=%s", pCfgName);
//      }
//    }
//
//  pBuff2+=1+sprintf(pBuff2, "");
//  ASSERT((pBuff2-Buffer2)<sizeof(Buffer2));
//  ProfFn.WrSection("Contents", Buffer2);
//  }
//
////---------------------------------------------------------------------------
//
//flag CSolveTool::LoadCurrentCfgName(Strng & Name)
//  {
//  char Buffer[65536];
//  char *pBuff;
//
//  if (ProfFn.RdSection("Contents", Buffer, sizeof(Buffer))>0)	
//    {
//    pBuff=Buffer;
//    while (strlen(pBuff)>0)
//      {
//      pchar pCfg, pCfgName;
//      ProfFn.GetProfItem(pBuff, pCfg, pCfgName);
//      if (strlen(pCfgName)>0)
//        {
//        Name=pCfgName;
//        return True;
//        }
//      }
//    }
//  else
//    Name="Default";
//  return False;
//  }
//
////---------------------------------------------------------------------------
//
//void CSolveTool::SaveConfiguration(Strng & Name)
//  {
//  if (Name.Length()>0)
//    {
//    Strng Sect;
//    FixCfgList(Name);
//
//    char Buffer[65536];
//    char *pBuff=Buffer;
//    pBuff+=1+sprintf(pBuff, "CnvMethod=%i", EqnCB().Cfg.iConvergeMeth);
//    pBuff+=1+sprintf(pBuff, "Eps_Rel=%g", EqnCB().Cfg.dEps_Rel);
//    pBuff+=1+sprintf(pBuff, "Eps_Abs=%g", EqnCB().Cfg.dEps_Abs);
//    pBuff+=1+sprintf(pBuff, "Damping=%g", EqnCB().Cfg.dDamping);
//    pBuff+=1+sprintf(pBuff, "MaxIters=%i", EqnCB().Cfg.nMaxIters);
//
//    pBuff+=1+sprintf(pBuff, "OptMethod=%i", iOptMeth);
//    switch (iOptMeth)
//      {
//      case CMM_None: 
//        break;
//      case CMM_Controls:
//        break;
//
//      case CMM_LawBailey:
//      case CMM_FlexiPlex:
//
//        for (int t=0; t<Slv().Targets.GetSize(); t++)
//          {
//          CTarget &Trg=Slv().Targets[t];
//          pBuff+=1+sprintf(pBuff, "Target_Exp=%s", Trg.Exp.sExp());
//          pBuff+=1+sprintf(pBuff, "Target_Val=%s", Trg.Val.sExp());
//          }
//
//        for (int d=0; d<Slv().Params.GetSize(); d++)
//          {
//          CParam &Prm=Slv().Params[d];
//          pBuff+=1+sprintf(pBuff, "Parm_Exp=%s", Prm.Exp.sExp()); 
//          if (Prm.Min.sExp.Length()>0)
//            pBuff+=1+sprintf(pBuff, "Parm_Min=%s", Prm.Min.sExp()); 
//          if (Prm.Max.sExp.Length()>0)
//            pBuff+=1+sprintf(pBuff, "Parm_Max=%s", Prm.Max.sExp()); 
//
//          }
//        break;
//      }
//    //pBuff+=1+sprintf(pBuff, "\n");
//    pBuff+=1+sprintf(pBuff, "");
//    ASSERT((pBuff-Buffer)<sizeof(Buffer));
//
//    Sect.Set("%s.Cfg", Name());
//    ProfFn.WrSection(Sect(), Buffer);
//    bChanged = 0;
//    }
//  };
//
////---------------------------------------------------------------------------
//
//byte CSolveTool::LoadConfiguration(Strng &Name)
//  {
//  byte iOptMethReqd=CMM_None;
//
//  if (Name.Length()>0)
//    {
//    Strng Sect;
//    Sect.Set("%s.Cfg", Name());
//
//    FixCfgList(Name);
//
//    char Buffer[65536];
//    char *pBuff;
//    Slv().Targets.SetSize(0);
//    Slv().Params.SetSize(0);
//    if (ProfFn.RdSection(Sect(), Buffer, sizeof(Buffer))>0)	
//      {
//      pBuff=Buffer;
//      pchar pName, pExp;
//      flag NeedItem=1;
//      while (strlen(pBuff)>0)
//        {
//        if (NeedItem)
//          ProfFn.GetProfItem(pBuff, pName, pExp);
//        NeedItem=1;
//        if(_stricmp(pName, "CnvMethod")==0) EqnCB().Cfg.iConvergeMeth=(byte)atol(pExp);
//        else if(_stricmp(pName, "Eps_Rel")==0) EqnCB().Cfg.dEps_Rel = atof(pExp);
//        else if(_stricmp(pName, "Eps_Abs")==0) EqnCB().Cfg.dEps_Abs = atof(pExp);
//        else if(_stricmp(pName, "Damping")==0) EqnCB().Cfg.dDamping = atof(pExp);
//        else if(_stricmp(pName, "MaxIters")==0) EqnCB().Cfg.nMaxIters = atol(pExp);
//        else if(_stricmp(pName, "OptMethod")==0) UpdateOptMeth(Range(CMM_None, (byte)atol(pExp), CMM_Last));
//        else if (_stricmp(pName, "Target_Exp")==0)
//          {
//          Slv().Targets.SetSize(Slv().Targets.GetSize()+1);
//          CTarget &Trg=Slv().Targets[Slv().Targets.GetSize()-1];
//          Trg.Exp.sExp=pExp;
//          //g.Val.sExp="";
//          ProfFn.GetProfItem(pBuff, pName, pExp);
//          if(_stricmp(pName, "Target_Val")==0)
//            Trg.Val.sExp=pExp;
//          else
//            NeedItem=0;
//          }
//        else if(_stricmp(pName, "Parm_Exp")==0)
//          {
//          Slv().Params.SetSize(Slv().Params.GetSize()+1);
//          CParam &Prm=Slv().Params[Slv().Params.GetSize()-1];
//          Prm.Exp.sExp=pExp;
//          ProfFn.GetProfItem(pBuff, pName, pExp);
//          if(_stricmp(pName, "Parm_Min")==0)
//            {
//            Prm.Min.sExp=pExp;
//            ProfFn.GetProfItem(pBuff, pName, pExp);
//            }
//          if(_stricmp(pName, "Parm_Max")==0)
//            Prm.Max.sExp=pExp;
//          else
//            NeedItem=0;
//          }
//        }
//      }
//    bChanged = 0;
//    }
//  return iOptMethReqd;
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::UpdateCfgList()
//  {
//  /*
//  char Buffer[65536];
//  char *pBuff;
//
//  Strng Fn(PrjDirectory());
//  Fn+=OPT_FILE;
//  
//  CComboBox * pLst=(CComboBox *)ConfigPage.GetDlgItem(IDC_SLV_CONFIG);
//  pLst->ResetContent();
//  if (GetPrivateProfileSection("Contents", Buffer, sizeof(Buffer), Fn())>0)	
//    {
//    pBuff=Buffer;
//    while (strlen(pBuff)>0)
//      {
//      char * pCfg=pBuff;
//      char * pCfgName=strchr(pBuff, '=');
//      pBuff+=strlen(pBuff)+1;
//      if(pCfgName)
//        {
//        pCfgName++;
//        pLst->AddString(pCfgName);
//        }
//      }
//    pLst->SetCurSel(0);
//    }
//  */
//  }
//
////---------------------------------------------------------------------------
//
//void CSolveTool::ChangeConfig()
//  {
//  /*
//  char buff[512];
//  //pLst->GetWindowText(buff, sizeof(buff));
//  //pSheet->SaveConfig(buff);
//  
//  CComboBox * pLst=(CComboBox *)ConfigPage.GetDlgItem(IDC_SLV_CONFIG);
//  int i=pLst->GetCurSel();
//  if (i!=CB_ERR)
//    if (pLst->GetLBText(i, buff)!=CB_ERR)
//      {
//      flag DoIt=1;
//      if (bChanged)
//        DoIt=(AfxMessageBox("Overwrite previous Unsaved Configuration", MB_YESNO|MB_ICONQUESTION)==IDYES);
//
//      if (DoIt)
//        {
//        byte iOpM=LoadConfiguration(buff);
//        //UpdateOptMeth(Range(CMM_None, iOpM, CMM_Last));
//        //RecalcLayout();
//        for (int i=0;i<GetPageCount(); i++)
//          if (((CPropPageCommon*)GetPage(i))->Inited())
//            GetPage(i)->UpdateData(False);
//        }
//      else
//        UpdateCfgList();
//      }
//  **/
//  }
//
////---------------------------------------------------------------------------
//
//void CSolveTool::DoSave()
//  {
//  SaveConfiguration(CfgName);
//  //if (SolveCfgPage.Inited())
//  //  {
//  //  if (ConfigPage.Inited())
//  //    {
//  //    CComboBox * pLst=(CComboBox *)ConfigPage.GetDlgItem(IDC_SLV_CONFIG);
//  //    if (pLst)
//  //      {
//  //      char buff[512];
//  //      pLst->GetWindowText(buff, sizeof(buff));
//  //      if (strlen(buff)==0)
//  //        LogError("Solver", LF_Exclamation, "Invalid Config Name");
//  //      else
//  //        {
//  //        SaveConfiguration(buff);
//  //        UpdateCfgList();
//  //        }
//  //      }
//  //    }
//  //  }
//  //
//  }
//
////===========================================================================
////
////
////
////===========================================================================
//
//flag CSolveTool::FindNearXRef(CGCXRefItem & XRef)
//  {
//  CXM_ObjectTag  ObjTag(XRef.pName, TABOpt_AllInfo);
//  CXM_ObjectData ObjData(0);
//  CXM_Route      ObjRoute;
//
//  if (Index==0)
//    m_Routes.SetSize(0);
//
//  ObjRoute.Clear();
//  if (XReadTaggedItem(ObjTag, ObjData, ObjRoute))
//    {
//    CPkDataItem* pPItem=ObjData.FirstItem();
//    XRef.iType = pPItem->Type();
//    XRef.pVar->m_iCnvIndex = pPItem->CnvIndex();
//    XRef.lDataPath = 0;
//    if (IsNumData(XRef.iType))
//      {
//
//      for (int i=0; i<m_Routes.GetSize(); i++)
//        if (m_Routes[i]==ObjRoute)
//          break;
//
//      if (i>=m_Routes.GetSize())
//        {
//        i=m_Routes.GetSize();
//        m_Routes.SetSize(i+1);
//        m_Routes[i]=ObjRoute;
//        }
//      XRef.lDataPath = i;
//
//      //XRef.sCnvTxt = pPItem->CnvTxt();
//      return True;
//      }
//    }
//  return False;
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::GetNearExtRefs(GCXRefArray & XRefs)
//  {
//  CXM_ObjectData ObjData(0);
//  CXM_Route      ObjRoute;
//
//  for (int i=0; i<XRefs.GetSize(); i++)
//    if (XRefs[i].bMustGet)
//      {
//      CXM_ObjectTag  ObjTag(XRefs[i].pName, TABOpt_AllInfo);
//      ObjRoute.Clear();
//      flag OK=XReadTaggedItem(ObjTag, ObjData, ObjRoute);
//      if (OK)
//        {
//        CPkDataItem* pPItem=ObjData.FirstItem();
//
//        if (IsFloatData(XRefs[i].iType))
//          XRefs[i].pVar->set(pPItem->Value()->GetDouble());//f.iCnv, f.sCnvTxt()));
//        else
//          XRefs[i].pVar->set(pPItem->Value()->GetLong());//f.iCnv, f.sCnvTxt()));
//        }
//
//      #if dbgSolveDlg  
//      if (dbgXRefs())
//        dbgpln("Get Near XRef <<< %14.6g %s ", XRefs[i].pVar->getD(), XRefs[i].pName);
//      #endif
//      }
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::SetNearExtRefs(GCXRefArray & XRefs)
//  {
//  CXM_ObjectData ObjData(0);
//  CXM_Route      ObjRoute;
//
//  for (int i=0; i<XRefs.GetSize(); i++)
//    if (XRefs[i].bMustSet)
//      {
//      /**
//      CXM_ObjectTag  ObjTag(XRefs[i].pName, TABOpt_AllInfo);
//      ObjRoute.Clear();
//      if (XWriteTaggedItem(ObjTag, ObjData, ObjRoute))
//        {
//        CPkDataItem* pPItem=ObjData.FirstItem();
//
//        if (IsFloatData(XRefs[i].iType))
//          XRefs[i].pVar->set(pPItem->Value()->GetDouble());//f.iCnv, f.sCnvTxt()));
//        else
//          XRefs[i].pVar->set(pPItem->Value()->GetLong());//f.iCnv, f.sCnvTxt()));
//        }
//      */
//      CPkDataItem * pPItem=ObjData.FirstItem();
//      PkDataUnion PData; 
//      if (IsFloatData(XRefs[i].iType))
//        PData.SetTypeDouble(XRefs[i].iType, XRefs[i].pVar->getD());//, f.iCnv, f.sCnvTxt());
//      else
//        PData.SetTypeLong(XRefs[i].iType, XRefs[i].pVar->getL());//, f.iCnv, f.sCnvTxt());
//      ObjData.List.SetDataValue(pPItem, XRefs[i].pName, 0, PData); 
//      ObjData.SetSize();
//
//      ObjRoute=m_Routes[XRefs[i].lDataPath];
//      flag OK=0;
//      switch (XWriteTaggedItem(ObjData, ObjRoute))
//        {
//        case WTD_NotFound : 
//          LogError("SolveDlg", LF_Exclamation, "%s not found", XRefs[i].pName); 
//          break;
//        case WTD_InvalidData : 
//          LogError("SolveDlg", LF_Exclamation, "Data for %s is Invalid",XRefs[i].pName); 
//          OK=1;
//          break;
//        case WTD_OK:
//          OK=1;
//          break;
//        }
//      #if dbgSolveDlg  
//      if (dbgXRefs())
//        dbgpln("Set Near XRef >>> %14.6g %s ", XRefs[i].pVar->getD(), XRefs[i].pName);
//      #endif
//      }
//  };
//
////===========================================================================
////
////
////
////===========================================================================
//
////===========================================================================
////
////
////
////===========================================================================
//
////UINT SolveThread(LPVOID pSlvTool_)
////  {
////  CSolveTool * pSheet=(CSolveTool *)pSlvTool_;
////  #if dbgSolveDlg
////  if (dbgSlvThread())  
////    dbgpln("Solver_Thread Entry");
////  TRACE("Solver_Thread Entry\n");
////  #endif
////  
////  pSheet->ThreadStuff();
////
////  #if dbgSolveDlg
////  if (dbgSlvThread())  
////    dbgpln("Solver_Thread Exit");
////  TRACE("Solver_Thread Exit\n");
////  #endif
////
////  return 0;
////  }
//
////---------------------------------------------------------------------------
//
////void CSolveTool::ThreadStuff()
////  {
////  Slv().RunSW.ReStart();
////  if (bOneClick)
////    bOneClickBusy=1;
////  EqnCB().fConverged=0;
////
////  CExecObj *pObj=NULL;
////  Exec.GetSolutionObj(-1, pObj); // force rebuild of lists
////  for (int nObjs=0; Exec.GetSolutionObj(nObjs, pObj); nObjs++) {};
////  
////  if (nObjs==0)
////    {
////    LogError("EqnSolver", 0,"Nothing to solve");
////    goto Done;
////    }
////
////  if (iOptMeth>CMM_None && Slv().Targets.GetSize()>0)
////    {
////    Slv().Pgm.Reset(); 
////    char * tfn=_tempnam(PrjDirectory(), "~EQ");
////    Strng TempFileName(tfn);
////    TempFileName+=".PGM";
////    free(tfn);// malloced by _tempnam 
////
////    if (!CreatePgm(TempFileName()))
////      {
////      LogError("EqnSolver", 0, "BAD PGM Create %s", TempFileName());
////      goto Done;
////      }
////
////    if (!Slv().Pgm.Load(TempFileName(), this, DefSolverSet))
////      {
////      LogError("EqnSolver", 0, "BAD PGM Load %s", TempFileName());
////      goto Done;
////      }
////
////    int nBadExtRefs=Slv().Pgm.UpdateXRefLists();
////    if (nBadExtRefs>0)
////      {
////      LogError("EqnSolver", 0, "%i External Reference(s) not found", nBadExtRefs);
////      goto Done;
////      }
////
////    #if dbgSolveDlg  
////    if (!dbgKeepTempFiles())
////      remove(TempFileName());
////    #else
////      remove(TempFileName());
////    #endif
////
////    FindAllVars();
////
////    Slv().Pgm.GetNearExtRefs();
////    //Slv().Pgm.SetNearExtRefs(this);
////    VERIFY(Slv().Pgm.Execute(this, 0.0, 0.0, "InitCond_Start:", "InitCond_End:")==0);
////
////    if(InitProblem())
////      {
////      // Initialise CurValues
////      CDVector Parms;
////      Parms.SetSize(Slv().Params.GetSize());
////      GetParams(Parms);
////      GetTargets();
////
////      SolveProblem();
////      FindMinimum(iOptMeth);
////      }
////    TermProblem();
////    }
////  else
////    {
////    if (InitProblem())
////      {
////      // Initialise CurValues
////      //CDVector Parms;
////      //Parms.SetSize(Slv().Params.GetSize());
////      //GetParams(Parms);
////      //GetTargets();
////      
////      SolveProblem();
////      }
////    TermProblem();
////    }
////
////Done:
////  Slv().RunSW.Stop();
////  ///Exec.SolverStop();
////  bThreadStuffBusy=0;
////  pSolveThread=NULL;
////  //if (!bStopReqd)
////  //  hSolveThread=NULL;
////
////  if (bOneClickBusy && EqnCB().fConverged)
////    {
////    //PostMessage(WM_COMMAND, IDB_SLV_DONE, 0);
////    }
////  //bStopReqd=0;
////  SetStateInfo();
////
////  bOneClickBusy=0;
////  };
////
////===========================================================================
////
////                      Simple Probal Solve
////
////===========================================================================

flag CSolveTool::Open()
  {
  CSolveState::Open(this);
  return True;
  }


flag CSolveTool::Close()
  {
  //CSolveState::Hide(True);
  CSolveState::Close();
  return True;
  }

flag CSolveTool::InitProblem(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  //RunSW.ReStart();
  
  CSolveState::Show();
  CSolveState::Update();


  //EqnSlvCtrlBlk &CB = SolverPage.EqnCB(); 
  flag OK=1;

//  if (bOneClick)
//    bOneClickBusy=1;

//  if (iOptMeth>CMM_None && Slv().Targets.GetSize()>0)
//    {
//    Slv().Pgm.Reset(); 
//    char * tfn=_tempnam(PrjDirectory(), "~EQ");
//    Strng TempFileName(tfn);
//    TempFileName+=".PGM";
//    free(tfn);// malloced by _tempnam 
//
//    if (!CreatePgm(TempFileName()))
//      {
//      LogError("EqnSolver", 0, "BAD PGM Create %s", TempFileName());
//      return False;
//      }
//
//    if (!Slv().Pgm.Load(TempFileName(), this, DefSolverSet))
//      {
//      LogError("EqnSolver", 0, "BAD PGM Load %s", TempFileName());
//      return False;
//      }
//
//    int nBadExtRefs=Slv().Pgm.UpdateXRefLists();
//    if (nBadExtRefs>0)
//      {
//      LogError("EqnSolver", 0, "%i External Reference(s) not found", nBadExtRefs);
//      return False;
//      }
//
//    #if dbgSolveDlg  
//    if (!dbgKeepTempFiles())
//      remove(TempFileName());
//    #else
//      remove(TempFileName());
//    #endif
//
//    FindAllVars();
//
//    Slv().Pgm.GetNearExtRefs();
//    //Slv().Pgm.SetNearExtRefs(this);
//    VERIFY(Slv().Pgm.Execute(this, 0.0, 0.0, "InitCond_Start:", "InitCond_End:")==0);
//    }

  
  //bSolutionBusy=1;
  //BalReportPage.DoBalReport();
  
  //CExecObj *pObj=NULL;
  //for (int i=0; Exec.GetSolutionObj(i, pObj); i++) 
  //  if (pObj->EO_InitSolution(EqnCB())!=0)
  //    OK=0;
  //bProbStarted=1;
//  EqnCB().Init();
  //nIters=0;
  //EqnCB.fConverged=True;
  //EqnCB.fConfigOK=True;

//  SendMessage(WMU_PB_UPDATEDATA, OK ? 1 : 2 , 0);
//  SetStateInfo();

  return OK;
  }

//---------------------------------------------------------------------------

flag CSolveTool::SolveProblem(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  //EqnSlvCtrlBlk &CB = SolverPage.EqnCB(); 
  flag OK=1;
/**
  SendMessage(WMU_PB_UPDATEDATA, 1, 0);

  bProbStarted=1;
  EqnCB().nIters=0;
  EqnCB().fConfigOK=0;
  EqnCB().fConfigOK=1;
  //bSolutionPaused=0;
  SetStateInfo();

  while (EqnCB().fConfigOK && !EqnCB().fConfigOK && !bStopReqd && EqnCB().nIters<EqnCB().Cfg.nMaxIters)
    {
    if (bSolutionPaused)
      {
      Sleep(500); // Simple Pause
      }
    else
      {
      EqnCB().nBad=0;
      EqnCB().nUnknowns=0;
      EqnCB().fConfigOK=True;
      //EqnCB().bConverged=False;
      EqnCB().fConverged=TRUE;
      CExecObj *pObj=NULL;
      for (int i=0; Exec.GetSolutionObj(i, pObj); i++) 
        switch (pObj->EO_StepSolution(EqnCB()))
          {
          case 0:;                        break;
          case 1:EqnCB().fConverged=0; break;
          case 2:EqnCB().fConfigOK=0;  break;
          };
      EqnCB().nIters++;
      SendMessage(WMU_PB_UPDATEDATA, 0, 0);
      SetStateInfo();
      }
    }
**/
  return OK;
  }

//---------------------------------------------------------------------------


flag CSolveTool::StepProblemBegin(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  //EqnSlvCtrlBlk &CB = SolverPage.EqnCB(); 
  flag OK=1;

//  EqnSlv().StartStep();
  //EqnCB.nBad=0;
  //EqnCB.nUnknowns=0;
  //EqnCB.fConfigOK=True;
  //EqnCB.fConverged=True;

//  if (iOptMeth>CMM_None && Slv().Targets.GetSize()>0)
//    {
//    CDVector Parms;
//    Parms.SetSize(Slv().Params.GetSize());
//    GetParams(Parms);
//    GetTargets();
//    };
//  //SendMessage(WMU_PB_UPDATEDATA, 0, 0);
//  SetStateInfo();

  return OK;
  }

//---------------------------------------------------------------------------

flag CSolveTool::StepProblemEnd(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  CSolveState::Update();

//  //EqnSlvCtrlBlk &CB = SolverPage.EqnCB(); 
  flag OK=1;
////    while (EqnCB().fConfigOK && !EqnCB().fConfigOK && !bStopReqd && EqnCB().nIters<EqnCB().Cfg.nMaxIters)
////  if (iOptMeth>CMM_None && Slv().Targets.GetSize()>0)
////    {
////    FindMinimum(iOptMeth);
////    }
//
//  if (!EqnCB.ConfigOK() || EqnCB.Converged() || EqnCB.NIters()>=EqnCB.Cfg.nMaxIters)
//    Exec.Stop();
//
//  EqnCB.BumpNIters();
////  SendMessage(WMU_PB_UPDATEDATA, 0, 0);
  //SetStateInfo();

  return OK;
  }

//---------------------------------------------------------------------------

flag CSolveTool::TermProblem(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  CSolveState::Hide(False);
  //RunSW.Stop();

  
  //EqnSlvCtrlBlk &CB = SolverPage.EqnCB(); 

  //CExecObj *pObj=NULL;
  //for (int i=0; Exec.GetSolutionObj(i, pObj); i++) 
  //  pObj->EO_TermSolution(EqnCB());

  //bSolutionBusy=0;

  //BalReportPage.DoBalReport();
  //if (EqnCB().fConverged)
  //  Beep(1000, 10);

//  if (bOneClickBusy && EqnCB().fConverged)
//    {
//    PostMessage(WM_COMMAND, IDB_SLV_PS_CN_DN, 0);
//    }
//
//  SendMessage(WMU_PB_UPDATEDATA, EqnCB().fConverged ? 3 : 2, 0);
//  SetStateInfo();
//
//  bOneClickBusy=0;

//  if (BalReportPage.Inited())
//    BalReportPage.UpdateData(FALSE);
  
  return True;
  }

//---------------------------------------------------------------------------

flag CSolveTool::BeginPause(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  CSolveState::Update();
//  SetStateInfo();
//  SendMessage(WMU_PB_UPDATEDATA, 4, 0);
  return True;
  }

//---------------------------------------------------------------------------

flag CSolveTool::EndPause(/*CExecutive & Exec, EqnSlvCtrlBlk & EqnCB*/)
  {
  CSolveState::Update();
//  SetStateInfo();
//  SendMessage(WMU_PB_UPDATEDATA, 4, 0);
  return True;
  }

//===========================================================================
//
//
//
//===========================================================================

//flag CSolveTool::CreatePgm(char *PGMName)
//  {
//  for (int t=0; t<Slv().Targets.GetSize(); t++)
//    {
//    Slv().Targets[t].Exp.sPGM="";
//    Slv().Targets[t].Val.sPGM="";
//    }
//
//  for (int d=0; d<Slv().Params.GetSize(); d++)
//    {
//    Slv().Params[d].Exp.sPGM="";
//    Slv().Params[d].Min.sPGM="";
//    Slv().Params[d].Max.sPGM="";
//    }
//  
//  FILE *f=fopen(PGMName, "wt"); 
//  fprintf(f, "; SysCAD Optimiser PGM\n;\n");
//  fprintf(f, ";\n");
//  fprintf(f, "InitCond_Start:\n");
//  fprintf(f, ";//                                        <<<<<<<<\n");
//  fprintf(f, "InitCond_End:\n");
//  fprintf(f, ";\n");
//  fprintf(f, "Targets_Start:\n");
//  fprintf(f, ";//                                        Common Code <<<<<<<<\n");
//
//  int nTrg=0;
//  int nVal=0;
//  for (t=0; t<Slv().Targets.GetSize(); t++)
//    {
//    fprintf(f, "; Target[%i]\n",t);
//    //fprintf(f, "Target%i:\n",t); // Label ?
//    CTarget &Trg=Slv().Targets[t];
//    Trg.Exp.sPGM.Set("T%i___", nTrg++);
//    Trg.Val.sPGM.Set("V%i___", nVal++);
//
//    fprintf(f, "  float %s, %s\n",Trg.Exp.sPGM(),Trg.Val.sPGM());
//    fprintf(f, "  %s=%s\n", Trg.Exp.sPGM(), Trg.Exp.sExp());
//    fprintf(f, "  %s=%s\n", Trg.Val.sPGM(), Trg.Val.sExp());
//    }
//  fprintf(f, "Targets_End:\n");
//  fprintf(f, ";\n");
//
//  fprintf(f, "Params_In_Start:\n");
//  fprintf(f, ";//                                        Common Code <<<<<<<<\n");
//  int nDegF=0;
//  int nMin=0;
//  int nMax=0;
//  for (d=0; d<Slv().Params.GetSize(); d++)
//    {
//    fprintf(f, "; Degree of Freedom[%i]\n",d);
//    //fprintf(f, "DegreeOfFreedom%i:\n",d); // Label ?
//    CParam &Prm=Slv().Params[d];
//    Prm.Exp.sPGM.Set("P%i___", nDegF++);
//    Prm.Min.sPGM.Set("Mn%i___", nMin++);
//    Prm.Max.sPGM.Set("Mx%i___", nMax++);
//
//    fprintf(f, "  float %s, %s, %s\n",Prm.Exp.sPGM(), Prm.Min.sPGM(), Prm.Max.sPGM());
//    fprintf(f, "  %s=%s\n", Prm.Exp.sPGM(), Prm.Exp.sExp());
//    if (Prm.Min.sExp.Length()>0)
//      fprintf(f, "  %s=%s\n", Prm.Min.sPGM(), Prm.Min.sExp());
//    if (Prm.Max.sExp.Length()>0)
//      fprintf(f, "  %s=%s\n", Prm.Max.sPGM(), Prm.Max.sExp());
//    }
//
//  fprintf(f, "Params_In_End:\n");
//  fprintf(f, ";\n");
//
//  fprintf(f, "Params_Out_Start:\n");
//  for (d=0; d<Slv().Params.GetSize(); d++)
//    {
//    CParam &Prm=Slv().Params[d];
//    fprintf(f, "  %s=%s\n", Prm.Exp.sExp(), Prm.Exp.sPGM());
//    }
//
//  fprintf(f, "Params_Out_End:\n");
//  fprintf(f, ";\n");
//
//
//  fprintf(f, "Actions_Start:\n");
//  fprintf(f, ";//                                        <<<<<<<<\n");
//  fprintf(f, "Actions_End:\n");
//
//  fprintf(f, ";\n");
//  fprintf(f, "$\n");
//  fclose(f);
//  
//  return True;
//  }
//
////---------------------------------------------------------------------------
//
//void CSolveTool::FindAllVars()
//  {
//  short Index;
//  for (int t=0; t<Slv().Targets.GetSize(); t++)
//    {
//    CTarget &Trg=Slv().Targets[t];
//    Trg.Exp.pVar=Slv().Pgm.FindVar(Trg.Exp.sPGM(), Index);
//    ASSERT(Trg.Exp.pVar);
//    Trg.Val.pVar=Slv().Pgm.FindVar(Trg.Val.sPGM(), Index);
//    ASSERT(Trg.Val.pVar);
//    }
//
//  for (int d=0; d<Slv().Params.GetSize(); d++)
//    {
//    CParam &Prm=Slv().Params[d];
//    Prm.Exp.pVar=Slv().Pgm.FindVar(Prm.Exp.sPGM(), Index);
//    ASSERT(Prm.Exp.pVar);
//    if (Prm.Min.sPGM.Length()>0)
//      {
//      Prm.Min.pVar=Slv().Pgm.FindVar(Prm.Min.sPGM(), Index);
//      ASSERT(Prm.Min.pVar);
//      }
//    if (Prm.Max.sPGM.Length()>0)
//      {
//      Prm.Max.pVar=Slv().Pgm.FindVar(Prm.Max.sPGM(), Index);
//      ASSERT(Prm.Max.pVar);
//      }
//    }
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::GetTargets()
//  {
//  Slv().Pgm.GetNearExtRefs();
//  //dbgpln("ExecTargets In");
//  VERIFY(Slv().Pgm.Execute(this, 0.0, 0.0, "Targets_Start:", "Targets_End:")==0);
//  for (int t=0; t<Slv().Targets.GetSize(); t++)
//    {
//    Slv().Targets[t].Exp.sCurVal.Set("%g",Slv().Targets[t].Exp());
//    //dbgpln("Get Trgs[%2i]=%14.6g [==%14.6g]",t, Slv().Targets[t].Exp(), Slv().Targets[t].Val());
//    }
//  /*
//  for (int t=0; t<Slv().Targets.GetSize(); t++)
//    {                                 
//    CTarget &Trg=Slv().Targets[t];
//    Trg.Exp.dVal=Trg.Exp.pVar->getD();
//    Trg.Val.dVal=Trg.Val.pVar->getD();
//    }
//  */
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::GetParams(CDVector &Parms)
//  {
//  Slv().Pgm.GetNearExtRefs();
//  //dbgpln("ExecParms In");
//  VERIFY(Slv().Pgm.Execute(this, 0.0, 0.0, "Params_In_Start:", "Params_In_End:")==0);
//
//  for (int p=0; p<Slv().Params.GetSize(); p++)
//    {
//    Slv().Params[p].Exp.sCurVal.Set("%g",Slv().Params[p].Exp());
//    Parms[p]=Slv().Params[p].Exp();
//    //dbgpln("Get Parm[%i]=%g",p,Parms[p]);
//    }
//  /*
//  for (int d=0; d<Slv().Params.GetSize(); d++)
//    {
//    CParam &Prm=Slv().Params[d];
//    Prm.Exp.dVal=Prm.Exp.pVar->getD();
//    if (Prm.Min.pVar)
//      Prm.Min.dVal=Prm.Min.pVar->getD();
//    if (Prm.Max.pVar)
//      Prm.Max.dVal=Prm.Max.pVar->getD();
//    }
//  */
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::SetParams(CDVector &Parms)
//  {
//  for (int p=0; p<Slv().Params.GetSize(); p++)
//    {
//    Slv().Params[p].Exp=Parms[p];
//    Slv().Params[p].Exp.sCurVal.Set("%g",Slv().Params[p].Exp());
//    //dbgpln("Set Parm[%2i]=%14.6g [%14.6g >> %14.6g]",p, Slv().Params[p].Exp(), Slv().Params[p].Min(), Slv().Params[p].Max());
//    }
//  //dbgpln("ExecParms Out");
//  VERIFY(Slv().Pgm.Execute(this, 0.0, 0.0, "Params_Out_Start:", "Params_Out_End:")==0);
//  Slv().Pgm.SetNearExtRefs();
//  };
//
////===========================================================================
////
////
////
////===========================================================================
//
//void CSolveTool::InitParms(CDVector &Parms, CLVector &ParmHolds)
//  {
//  GetParams(Parms);
//  //for (int i=0; i<NParms(); i++)
//  //  Parms[i]=Slv().Params[i].Exp();//.dVal;
//  Slv().StdD.SetSize(NParms());
//  Slv().StdD.SetAll(1.0);
//  };
//
////---------------------------------------------------------------------------
//
//flag CSolveTool::GetParmRanges(/*MinFinder &Fndr, */CDVector &ParmMins, CDVector &ParmMaxs)
//  {
//  return False;
//  };
//
////---------------------------------------------------------------------------
//
//double CSolveTool::CostFn(CDVector &Parms)
//  {
//  SetParams(Parms);
//  SolveProblem();//StepProblem();
//  GetTargets();
//  double Cost=0.0;
//  for (int i=0; i<Slv().Targets.GetSize(); i++)
//    Cost+=Sqr(Slv().Targets[i].Exp()/*.dVal*/-Slv().Targets[i].Val()/*.dVal*/);
//  return Cost;
//  };
//
//// -------------------------------------------------------------------------
//// EvalDeriv evaluates the derivative of the isotherm response with respect to 
//// parameter k at data point i.
//double CSolveTool::EvalDeriv(CDVector &Parms, CDVector &ParmTols, long i, long k )
//  {                 
//  double h = ParmTols[k] * 10.0;
//  //if (fabs(h) < fabs(ar_Perturb))
//  //  h = ar_Perturb;
//  Parms[k] += h;
//  double PerturbFp = CostFn(Parms) / Slv().StdD[i];
//  Parms[k] -= (2.0 * h);
//  double PerturbFm = CostFn(Parms) / Slv().StdD[i];
//  Parms[k] += h;
//  return (PerturbFp - PerturbFm) / (2.0 * h);
//  }
//
////---------------------------------------------------------------------------
//
//flag CSolveTool::ConstructLBMatrix(CDVector &Parms, CDVector &ParmTols, CDMatrix &Z, CDVector &C)
//  {
//  static CDVector Derivs;
//
//  Derivs.SetSize(NParms());
//  Derivs.SetAll(0.0);
//  C.SetSize(NParms());
//  C.SetAll(0.0);
//  Z.SetSize(NParms(), NParms());
//  Z.SetAll(0.0);
//  CostFn(Parms); //??
//  //Find the residuals
//  for (int i=0; i<Slv().Targets.GetSize(); i++)
//    {
//    //Get all derivs at data point i
//    for (long k=0; k<NParms(); k++)
//      Derivs[k] = EvalDeriv(Parms, ParmTols, i, k);
// 
//    Derivs.dbgDump("Derivs");
//    // Calc Slv().Targets At Parms
//    SetParams(Parms);
//    SolveProblem();
//    GetTargets();
//    for (k=0; k<NParms(); k++)
//      C[k] += -(Slv().Targets[i].Exp()-Slv().Targets[i].Val()) * (fabs(Derivs[k]) / Slv().StdD[i]);
//    //Construct the matrix
//    for (long NRow=0; NRow<NParms(); NRow++)
//      for (long NCol=0; NCol<NParms(); NCol++)
//        Z[NRow][NCol] += (Derivs[NRow] * Derivs[NCol]);
//    }
//
//  return True;
//  };
//
////---------------------------------------------------------------------------
//
//flag CSolveTool::InitConvergence(double &StepSize, double &ConverTol) 
//  {
//  // Accept Default
//  //StepSize=0.01;
//  return True;
//  };
//
////---------------------------------------------------------------------------
//
//int CSolveTool::NEqualityConstraints() 
//  {
//  return 0;
//  };
//void CSolveTool::EqualityConstraints(CDVector &Parms, CDVector &Residual)
//  {
//  };
//
////---------------------------------------------------------------------------
//
//int CSolveTool::NInEqualityConstraints()
//  {
//  int n=0;
//  for (int d=0; d<Slv().Params.GetSize(); d++)
//    {
//    CParam &Prm=Slv().Params[d];
//    if (Prm.Min.sExp.Length()>0)
//      n++;
//    if (Prm.Max.sExp.Length()>0)
//      n++;
//    }
//  return n;
//  };
//
////---------------------------------------------------------------------------
//
//void CSolveTool::InEqualityConstraints(CDVector &Parms, CDVector &Residual)
//  {
//  SetParams(Parms);
//  SolveProblem();//StepProblem();
//  GetTargets();
//  int n=0;
//  for (int d=0; d<Slv().Params.GetSize(); d++)
//    {
//    CParam &Prm=Slv().Params[d];
//    if (Prm.Min.sExp.Length()>0)
//      Residual[n++]=Prm.Exp()-Prm.Min();
//    if (Prm.Max.sExp.Length()>0)
//      Residual[n++]=Prm.Max()-Prm.Exp();
//    }
//  };

//===========================================================================
//
//
//
//===========================================================================

CSolveTool SolveTool;//() { return *CSolveTool::pSolveTool; };

//===========================================================================
//
//
//
//===========================================================================

