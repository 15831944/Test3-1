//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __ELECCB_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "elecblk.h"
#include "scd_wm.h"
//#include "optoff.h"

//===========================================================================

static double Drw_CElecBlk[] =
       {
       DD_Poly, -3,-3, -3,3, 3,3, 3,-3, -3,-3,
       //DD_Poly, -3,0, 3,0,
       //DD_Poly, 0, -3,0, 3,
       DD_End
       };

IOAreaRec ElecBlkIOAreaList[] =
  {//{"Input",         "In" ,         0,            LIO_In0 , nc_MLnk, 1,1, IOGRP(1)},
   //{"Input",         "I"  ,         0,            LIO_In  , nc_MLnk, 1,1, IOGRP(1)},
   //{"",              "Src",         0,            LIO_In  , nc_MLnk, 1,1, IOGRP(1)},
   //{"",              "S"  ,         0,            LIO_In  , nc_MLnk, 1,1, IOGRP(1)},
   //{"Output",        "Out",         1,            LIO_Out0, nc_MLnk, 1,1, IOGRP(1)},
   //{"Output",        "O"  ,         1,            LIO_Out , nc_MLnk, 1,1, IOGRP(1)},
   //{"",              "Dst",         1,            LIO_Out , nc_MLnk, 1,1, IOGRP(1)},
   //{"",              "D"  ,         1,            LIO_Out , nc_MLnk, 1,1, IOGRP(1)},
   //{"",              "Speed",       CIOId_Spd,    LIO_In  , nc_CLnk, 0,1},
   //{"ElecSupply",    "ElecIn",      EIOId_Supply, LIO_In  , nc_ELnk, 0,50},
   {NULL}};


IMPLEMENT_MODELUNIT(CElecBlk, "E_CdBlk", "", Drw_CElecBlk, "Electrical", "Ecb", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, 
                   "Electrical:Code Blk",
                   "Code Blk")

CElecBlk::CElecBlk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach),
  PgmMngr(/*this,*/ "ecb"),
  m_PGMTermStrips(this)
  {
  fHasFiles=1;
  iIterCnt = 0;
  m_bMustInit = 0;
  m_bIterOne = 0;
  m_bMustTerm = 0;
  bJustLoaded = 0;
  bEmpty = 0;
  bPreset = 0;
  bAutoReload = True;
  bReloadReqd = False;
  bWithDBG = False;
  AttachClassInfo(nc_Control, ElecBlkIOAreaList, &NullFlwGroup);
  fActiveHoldOK_Dyn=true;
  sPGMName = "";
  bOn = 1;
  m_VarData = NULL;
  m_nMaxVarData = 0;
  SetMaxVarData(32L);
  hProcess = NULL;
  EditTime.dwLowDateTime=0;
  EditTime.dwHighDateTime=0; 
  }

//--------------------------------------------------------------------------

CElecBlk::~CElecBlk()
  {
  SetMaxVarData(0);
  }

//--------------------------------------------------------------------------

LPTSTR CElecBlk::GetPrimaryCfg() 
  { 
  return sPGMName();
  };

//--------------------------------------------------------------------------

flag CElecBlk::PutPrimaryCfg(LPCTSTR Cfg) 
  { 
  return LoadPGM((LPTSTR)Cfg, false);
  };

//--------------------------------------------------------------------------

void CElecBlk::SetMaxVarData(long NewSize)
  {
  CSingleLock Lock(&m_VarDataSect, true);
  if (NewSize!=m_nMaxVarData)
    {
    GCVar** NewVarData = NULL;
    if (NewSize>0)
      {
      NewVarData = new pGCVar[NewSize];
      for (int i=0; (i<NewSize && i<m_nMaxVarData); i++)
        NewVarData[i] = m_VarData[i];
      }
    if (m_VarData)
      delete []m_VarData;
    m_VarData = NewVarData;
    m_nMaxVarData = NewSize;
    }
  }

//--------------------------------------------------------------------------

void CElecBlk::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

const word idmPGMName      = 1000;
const word idmPGMPath      = 1001;
const word idmEditPGMBtn   = 1002;
const word idmReloadPGMBtn = 1003;
const word idmBrowsePGMBtn = 1004;
const word idmDebugPGMBtn  = 1005;
const word idmTagCheckBtn  = 1006;
const word idmWatchesOn    = 1007;
const word idmPGMWatch     = 1100;                    // Keep MaxPGMWatch free after this
const word idmPGMData      = idmPGMWatch+MaxPGMWatch; // Keep everything free after this

//--------------------------------------------------------------------------

void CElecBlk::CountWatchedVars(pGCVar VarList, int & Count)
  {
  pGCVar pVar = VarList;
  while (pVar)
    {
    if (pVar->Watched())
      Count++;
    if (pVar->WhatAmI()==VarClass && !(pVar->m_flags & VarClassDefn))
      CountWatchedVars(((pGCClassVar)pVar)->m_pVarList, Count);
    pVar = pVar->Next();
    }
  }

//--------------------------------------------------------------------------

void CElecBlk::DDBAddWatchedVars(DataDefnBlk & DDB, pchar pHdTag, pGCVar VarList, int Lvl, int &nPg, int &nOnPg, flag ForceNewPage, int MaxPerPage)
  {
  Strng Tag;
  char HdTag[512];
  strcpy(HdTag, pHdTag); 
  if (strlen(pHdTag)>0) 
    strcat(HdTag, ".");

  pGCVar pVar = VarList;
  while (pVar)
    {
    //dbgpln("WV:%s", pVar->Name() ? pVar->Name() :"??") ;   
    if (pVar->Watched())
      {
      CSingleLock Lock(&m_VarDataSect, true);
      int nUseData = m_nVarData;
      m_nVarData++;
      if (m_nVarData>=m_nMaxVarData)
        SetMaxVarData(m_nMaxVarData+16);
      char S[256];
      if (ForceNewPage || (Lvl==0 && nOnPg>MaxPerPage))
        {
        ForceNewPage = 0;
        sprintf(S, "Data:%i",nPg+1);
        DDB.Page(S, DDB_RqdPage);
        nOnPg = 0;
        nPg++;
        }
      nOnPg++;

      m_VarData[nUseData] = pVar;
      Tag.Set("%s%s", HdTag, pVar->Name());
      //const DDEF_Flags flgs = ((!pVar->ReadOnly() ? 0 : isParm) | noFile);
      const DDEF_Flags flgs = ((pVar->ReadOnly() ? 0 : isParm) | noFile);
      if (pVar->m_pWatchInfo)
        {
        switch(pVar->WhatAmI())
          {
          case VarTag   :
          case VarDouble: 
            DDB.Double(Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), idmPGMData+nUseData, this, flgs|NAN_OK);
            DDB.Range(pVar->m_pWatchInfo->dMin, pVar->m_pWatchInfo->dMax);
            DDB.Description(pVar->m_pWatchInfo->sDesc()); 
            break;
          case VarLong  :
          case VarByte  : 
            DDB.Long  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), idmPGMData+nUseData, this, flgs);
            DDB.Range((long)pVar->m_pWatchInfo->dMin, (long)pVar->m_pWatchInfo->dMax);
            DDB.Description(pVar->m_pWatchInfo->sDesc()); 
            break;
          case VarBit   : 
            DDB.Bool  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), idmPGMData+nUseData, this, flgs);
            DDB.Range((byte)pVar->m_pWatchInfo->dMin, (byte)pVar->m_pWatchInfo->dMax);
            DDB.Description(pVar->m_pWatchInfo->sDesc()); 
            break;
          case VarStr   : 
            DDB.String(Tag(), "", DC_, "", idmPGMData+nUseData, this, flgs); 
            break;
          }
        }
      else
        {
        switch(pVar->WhatAmI())
          {
          case VarTag   :
          case VarDouble: 
            DDB.Double(Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), idmPGMData+nUseData, this, flgs|NAN_OK);
            break;
          case VarLong  :
          case VarByte  : 
            DDB.Long  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), idmPGMData+nUseData, this, flgs);
            break;
          case VarBit   : 
            DDB.Bool  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), idmPGMData+nUseData, this, flgs);
            break;
          case VarStr   : 
            DDB.String(Tag(), "", DC_, "", idmPGMData+nUseData, this, flgs); 
            break;
          }
        }
      }
    if (pVar->WhatAmI()==VarClass && !(pVar->m_flags & VarClassDefn))
      {
      Tag.Set("%s%s", HdTag, pVar->Name());
      flag ForcePg = (Lvl==0 && nOnPg>MaxPerPage);
      DDBAddWatchedVars(DDB, Tag(), ((pGCClassVar)pVar)->m_pVarList, Lvl+1, nPg, nOnPg, ForcePg, MaxPerPage);
      }
    pVar = pVar->Next();
    }
  }

//--------------------------------------------------------------------------

void CElecBlk::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CElecBlk", NULL, DDB_NoPage);
  DDB.Text(" ");
  DDB.String("PGM_Name",       "", DC_,    "",    idmPGMName,               this, isParmStopped|SetOnChange);
  DDB.String("PGM_Path",       "", DC_,    "",    idmPGMPath,               this, isParmStopped|noFileAtAll);
  DDB.CheckBoxBtn("Enabled",   "", DC_, "",    &bOn,                     this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("AutoReload", "", DC_, "",    &bAutoReload,             this, isParm, DDBOnOff);
  DDB.String("State",        "", DC_,    "",    &PgmMngr.m_StateLine[0],  this, noSnap|noFile);
  DDB.Text("Error:");
  DDB.String("Msg_1",        "", DC_,    "",    &PgmMngr.m_StateLine[1],  this, noSnap|noFile);
  DDB.String("Msg_2",        "", DC_,    "",    &PgmMngr.m_StateLine[2],  this, noSnap|noFile);
  DDB.String("Msg_3",        "", DC_,    "",    &PgmMngr.m_StateLine[3],  this, noSnap|noFile);
  DDB.String("Msg_4",        "", DC_,    "",    &PgmMngr.m_StateLine[4],  this, noSnap|noFile);
  DDB.Text(" ");
  // These buttons should follow the ECBName
  DDB.Button("Edit_ECB",     "", DC_,    "",    idmEditPGMBtn,            this, isParmStopped);
  DDB.Button("Reload_ECB",   "", DC_,    "",    idmReloadPGMBtn,          this, isParmStopped);
  DDB.Button("Browse_ECB",   "", DC_,    "",    idmBrowsePGMBtn,          this, isParmStopped);
  DDB.Button("Check_tags",   "", DC_,    "",    idmTagCheckBtn,           this, isParmStopped);
  DDB.Text(" ");
  //DDB.Button("Debug ECB",    "", DC_,    "",    idmDebugPGMBtn,           this, isParm);
  //DDB.Text(" ");

  m_nVarData = 0;
  int nPg = 0;
  int nOnPg = 20;
  int nTtlCnt = 0;
  CountWatchedVars(PgmMngr.m_pVarList, nTtlCnt);
  SetMaxVarData(Max(32, nTtlCnt));
  const int MaxPerPage = Max(30, (int)(nTtlCnt/63)); //Note: 63 = "FixedEditView::MaxFixedPages-1"
  DDBAddWatchedVars(DDB, "", PgmMngr.m_pVarList, 0, nPg, nOnPg, 0, MaxPerPage);

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CElecBlk::LoadPGM(char * pPGMName, flag WithDebug)
  {
  CWaitCursor Wait;
  bJustLoaded = 1;
  bWithDBG = WithDebug;
  bReloadReqd = false;
  sPGMName = pPGMName;
  sPGMName.LRTrim();

  //CArray<CString, CString&> ConndIOPts;
  //for (int e=EIO1(); e<EION(); e++)
  //  {
  //  if (e==0 || ConndIOPts[ConndIOPts.GetUpperBound()].CompareNoCase((LPCTSTR)IODesc_Self(e)->pName)!=0)
  //    ConndIOPts.Add(CString((LPCTSTR)IODesc_Self(e)->pName));
  //  }

  //CETermStripDefnArray OldCblDefn(PgmMngr.m_TermStripDefns);
  Strng ss;
  if (sPGMName())
    {
    ss.FnDrivePathName(sPGMName());
    ss.FnCheckExtension("ecb");
    ss.FnSearchExpand(NULL, AF_All|AF_BackupFiles);
    sPGMName.FnContract(ss());
    }

  if (WithDebug)
    bReloadReqd=!PgmMngr.Debug(AF_All|AF_BackupFiles, ss(), this, DefGenConSet);
  else
    bReloadReqd=!PgmMngr.Load(AF_All|AF_BackupFiles, ss(), this, DefGenConSet);

  if (!bReloadReqd)
    {
    FixElectricalIO(ElecBlkIOAreaList, PgmMngr.m_TermStripDefns);
    m_PGMTermStrips=PgmMngr.m_TermStripDefns;
    }

  MyTagsHaveChanged();
  BumpElectricsTopoCount();

  return True;
  }

//--------------------------------------------------------------------------

flag CElecBlk::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmBrowsePGMBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Ext("*.ecb");
        CSCDFileDialog Dlg(true, NULL, Ext(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "ECB Files (*.ecb)|*.ecb||");
        Dlg.m_ofn.lpstrInitialDir = PrjFiles();
        Dlg.m_ofn.lpstrTitle = "Program File";
        HWND H=::GetActiveWindow();
        if (Dlg.DoModal()==IDOK)
          sPGMName=Dlg.GetPathName();
        ::SetActiveWindow(H);
        DCB.B=0;
        sPGMName.FnContract();
        bReloadReqd=true;
        if (bAutoReload)
          LoadPGM(sPGMName(), bWithDBG);
//        ValidateData(ValidateDataBlk & VDB);
        }
      DCB.B=0;
      return 1;
    case idmReloadPGMBtn:
      if (DCB.rB && (*DCB.rB!=0))
        LoadPGM(sPGMName(), False);
      DCB.B=0;
      return 1;
    case idmEditPGMBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        EditTime.dwLowDateTime=0;
        EditTime.dwHighDateTime=0; 
        if (sPGMName.Length()==0)
          {
          sPGMName=Tag();
          sPGMName+=".ecb";
          }
        Strng FileName(sPGMName());
        if (FileName.Length()>0)
          {
          DWORD ExitCode = 0;
          if (hProcess)
            GetExitCodeProcess(hProcess, &ExitCode);
          if (ExitCode==STILL_ACTIVE)
            {
            ActivateApp(dwProcessId);
            }
          else
            {
            Strng ss;
            hProcess = NULL;
            FileName.FnCheckExtension("ecb");
            FileName.FnSearchExpand(NULL, AF_All|AF_BackupFiles, AF_PrjFiles);
            Strng NPad=TxtEditCmdString(FileName());
            STARTUPINFO si;
            memset(&si, 0, sizeof(si));
            si.cb = sizeof(si);
            si.wShowWindow = SW_SHOWDEFAULT;
            PROCESS_INFORMATION pi;
            if (CreateProcess(NULL, NPad(), NULL, NULL, FALSE, 0, NULL, PrjFiles(), &si, &pi))
              {
              hProcess = pi.hProcess;
              dwProcessId = pi.dwProcessId;

              SYSTEMTIME ST;
              GetSystemTime(&ST);
              SystemTimeToFileTime(&ST, &EditTime);
              }
            }
          }
        }
      DCB.B=0;
      return 1;
    case idmDebugPGMBtn:
      if (DCB.rB && (*DCB.rB!=0))
        LoadPGM(sPGMName(), True);
      DCB.B=0;
      return 1;
    case idmTagCheckBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        if (PreStartCheck())
          LogNote(Tag(), 0, "No bad external tag references");
        m_bIterOne = 0;
        }
      DCB.B=0;
      return 1;
   
    case idmPGMName:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot
        Strng File0, Path0;
        File0.FnNameExt(DCB.rpC);
        Path0.FnDrivePath(DCB.rpC);

        Strng File1, Path1;
        File1.FnNameExt(sPGMName());
        Path1.FnDrivePath(sPGMName());
        
        Strng S;
        S=Path0.Length()>0 ? Path0() : Path1();
        S+=File0.Length()>0 ? File0() : File1();

        LoadPGM(S(), False);
        }
      if (DCB.ForView())
        {
        sPGMFile.FnNameExt(sPGMName());
        DCB.pC= sPGMFile();
        }
      else
        DCB.pC= sPGMName();
      return 1;
    
    case idmPGMPath:
      sPGMPath.FnDrivePath(sPGMName());
      if (SymbolicPaths())
        sPGMPath.FnContract();
      else
        sPGMPath.FnExpand();
      DCB.pC= sPGMPath();
      return 1;

    default:
      if (DCB.lHandle>=idmPGMData && DCB.lHandle<idmPGMData+m_nVarData)
        {
        GCVar* pV = m_VarData[DCB.lHandle-idmPGMData];
        if (pV)
          {
          if (pV->m_pCnvInfo)
            {
            switch(pV->WhatAmI())
              {
              case VarDouble: if (DCB.rD) pV->set(Cnvs[pV->m_pCnvInfo->Index()]->Human(*DCB.rD, pV->m_pCnvInfo->Text())); 
                              DCB.D=Cnvs[pV->m_pCnvInfo->Index()]->Normal(pV->getD(), pV->m_pCnvInfo->Text()); return True;
              case VarLong  : if (DCB.rL) pV->set(Cnvs[pV->m_pCnvInfo->Index()]->Human(*DCB.rL, pV->m_pCnvInfo->Text()));
                              DCB.L=(long)Cnvs[pV->m_pCnvInfo->Index()]->Normal(pV->getL(), pV->m_pCnvInfo->Text()); return True;
              case VarByte  : if (DCB.rL) pV->set(*DCB.rL); DCB.L=pV->getL(); return True;
              case VarBit   : if (DCB.rB) pV->set(*DCB.rB); DCB.B=pV->getB(); return True;
              case VarTag   : if (DCB.rD) pV->set(*DCB.rD); DCB.D=pV->getD(); return True;
              case VarStr   : if (DCB.rpC) ((pGCStrVar)pV)->set(DCB.rpC); DCB.pC=((pGCStrVar)pV)->getS(); return True;
              }
            }
          else
            {
            switch(pV->WhatAmI())
              {
              case VarDouble: if (DCB.rD) pV->set(*DCB.rD); DCB.D=pV->getD(); return True;
              case VarLong  : if (DCB.rL) pV->set(*DCB.rL); DCB.L=pV->getL(); return True;
              case VarByte  : if (DCB.rL) pV->set(*DCB.rL); DCB.L=pV->getL(); return True;
              case VarBit   : if (DCB.rB) pV->set(*DCB.rB); DCB.B=pV->getB(); return True;
              case VarTag   : if (DCB.rD) pV->set(*DCB.rD); DCB.D=pV->getD(); return True;
              case VarStr   : if (DCB.rpC) ((pGCStrVar)pV)->set(DCB.rpC); DCB.pC=((pGCStrVar)pV)->getS(); return True;
              }
            }
          return True;
          }
        }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CElecBlk::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=FlwNode::ValidateData(VDB);
  SetCI(1, bReloadReqd);
  return OK;
  }

//--------------------------------------------------------------------------

flag CElecBlk::PreStartCheck()
  {
  m_bMustInit = 1;
  m_bIterOne = 0;
  m_bMustTerm = 0;
  if (bOn)
    {
    }
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

bool CElecBlk::IsXRefListActive() { return !GetActiveHold() && bOn!=0; }

//--------------------------------------------------------------------------

int CElecBlk::UpdateXRefLists(CXRefBuildResults & Results) 
  {
  if (1)//bOn)
    PgmMngr.UpdateXRefLists(Results);
  //bDoneExtRefs = 1;
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CElecBlk::UnlinkAllXRefs()
  {
  PgmMngr.UnlinkAllXRefs();

  CNodeXRefMngr::UnlinkAllXRefs();
  }

//--------------------------------------------------------------------------

void CElecBlk::CollectElectrics(CNodeElectricsArray & TSCA)
  {
  FlwNode::CollectElectrics(TSCA);

  CNodeElectrics & TSP=m_Electrics;
  for (int i=0; i<m_PGMTermStrips.GetSize(); i++)
    m_Electrics.AddTermStrip(m_PGMTermStrips[i]);
  };

//--------------------------------------------------------------------------

void CElecBlk::ConnectElectrics()
  {
  FlwNode::ConnectElectrics();

  DoEvalCtrl("ConnectElectrics");
  };

//--------------------------------------------------------------------------

void CElecBlk::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  iIterCnt = -1;
  DoEvalCtrl("CtrlInitialise"); 
  m_bIterOne = 1;
  };

//--------------------------------------------------------------------------

void CElecBlk::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  iIterCnt++;
  DoEvalCtrl("CtrlStrategy"); 
  m_bIterOne = 0;
  m_bMustTerm = 1;
  }

//--------------------------------------------------------------------------

void CElecBlk::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  //iIterCnt++;
  DoEvalCtrl("CtrlActions"); 
  m_bIterOne = 0;
  m_bMustTerm = 1;
  }

//--------------------------------------------------------------------------

//void CElecBlk::EvalPowerRequired()
//  {
//  DoEvalCtrl("EvalPowerRequired"); 
//  }

//--------------------------------------------------------------------------

void CElecBlk::EvalCtrlTerminate(eScdCtrlTasks Tasks)
  {
  DoEvalCtrl("CtrlTerminate"); 
  }

//--------------------------------------------------------------------------

void CElecBlk::DoEvalCtrl(LPCTSTR TriggerName)
  {
  if (bOn && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {
    SetCI(1, bReloadReqd);
//    if (bIterationOne && SolveDirectMethod())
//      PgmMngr.InitPBConvergence();
    CGExecContext ECtx(this);
    ECtx.dIC_Time = ICGetTime();
    ECtx.dIC_dTime = XRunning() ? ICGetTimeInc() : 0.0;
    ECtx.SetFlags(m_bMustInit && !XRunning() && XStarting(), m_bIterOne, m_bMustTerm && !XRunning(), bJustLoaded, bEmpty, bPreset);
    PgmMngr.Execute(ECtx, TriggerName, &m_Electrics);
    ClrCI(4);
    if (PgmMngr.m_iRunTimeErr==1)
      SetCI(4, "E\tRun time error during iteration %d", iIterCnt);
    else if (PgmMngr.m_iRunTimeErr>1)
      SetCI(4, "E\t%d run time errors during iteration %d", PgmMngr.m_iRunTimeErr, iIterCnt);
    ClrCI(5);
    if (PgmMngr.m_iMathRunTimeErr==1)
      SetCI(5, "E\tMath run time error (iteration %d)", iIterCnt);
    else if (PgmMngr.m_iMathRunTimeErr>1)
      SetCI(5, "E\t%d math run time errors (iteration %d)", PgmMngr.m_iMathRunTimeErr, iIterCnt);
    for (int i=0; i<MaxCondMsgs; i++)
      {
      if (PgmMngr.m_CINOn[i])
        SetCI(6+i, "N\t%s", PgmMngr.m_CINMsgs[i]());
      else
        ClrCI(6+i);
      if (PgmMngr.m_CIEOn[i])
        SetCI(6+MaxCondMsgs+i, "E\t%s", PgmMngr.m_CIEMsgs[i]());
      else
        ClrCI(6+MaxCondMsgs+i);
      }
    bJustLoaded = 0;
    bEmpty = 0;
    bPreset = 0;
    if (ECtx.DoXStop)
      {
      LogError(Tag(), 0, "SysCAD stopped by ECB");
      ExecObj()->XStop();
      }
    if (ECtx.DoXIdle)
      {
      LogError(Tag(), 0, "SysCAD paused by ECB");
      ExecObj()->XIdle();
      }
    }
  else
    {
    ClrCI(4);
    ClrCI(5);
    for (int i=0; i<MaxCondMsgs*2; i++)
      ClrCI(6+i);
    }
  }

//--------------------------------------------------------------------------

flag CElecBlk::GetOtherData(FilingControlBlock &FCB)
  {
  if (FCB.SaveAs() && hProcess)
    {//during a save-as we must reset the link to the "old" edit files...
    //LogNote(Tag(), 0, "Remove link to currently open ecb file. (File should be closed)");
    hProcess = NULL; //this is probably not needed for "far-away" files where location does not change
    }
  
  return PgmMngr.SaveScenario(FCB);
  }

//--------------------------------------------------------------------------

flag CElecBlk::PutOtherData(FilingControlBlock &FCB)
  {
  return PgmMngr.LoadScenario(FCB);
  }


//---------------------------------------------------------------------------

void CElecBlk::OnAppActivate(BOOL bActive) 
  { 
  if (bOn)
    {
    Strng sFn;
    flag Reload=false;
    if (sPGMName() && (EditTime.dwLowDateTime || EditTime.dwHighDateTime))
      {
      if (sFn.FnExpandQueryReload(sPGMName(), EditTime))
        {
        Reload=true;
        EditTime.dwLowDateTime=0;
        EditTime.dwHighDateTime=0; 
        }
      }
    for (int i=0; i<PgmMngr.m_FilesUsed.GetSize(); i++)
      if (sFn.FnExpandQueryReload(PgmMngr.m_FilesUsed[i](), PgmMngr.m_FilesTime[i]))
        Reload=true;
    
    if (Reload)
      {
      bReloadReqd=true;
      if (bAutoReload)
        {
        LoadPGM(sPGMName(), bWithDBG);
        char* pTag = new char[strlen(Tag())+1];
        strcpy(pTag, Tag());
        AfxGetMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_ACCREFRESHSAVE, (LPARAM)pTag);
        }
      ValidateDataBlk VDB;
      ValidateData(VDB);
      }
    }
  }

//--------------------------------------------------------------------------

int CElecBlk::FilesUsed(CFilesUsedArray & Files)
  {
  if (sPGMName.Len()>0)
    {
    Strng ss;
    Files.AddFile(FnExpand(ss, sPGMName()), FU_CopyFile|FU_EditTxt);
    const int RefFilesUsed = PgmMngr.m_FilesUsed.GetSize();
    for (int i=1; i<RefFilesUsed; i++)
      Files.AddFile(FnExpand(ss, PgmMngr.m_FilesUsed[i]()), FU_CopyFile|FU_EditTxt);
    return RefFilesUsed;
    }
  return 0;
  }

//--------------------------------------------------------------------------

void   CElecBlk::SetState(eScdMdlStateActs RqdState)
  {
  FlwNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit: break;
    case MSA_Empty: bEmpty = 1; break;
    case MSA_PreSet: bPreset = 1; break;
    case MSA_SteadyState: break;
    }
  }

//--------------------------------------------------------------------------

int CElecBlk::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CElecBlk::DeleteTag(pchar pDelTag)
  {
  return EODT_DONE;
  }


//--------------------------------------------------------------------------

flag CElecBlk::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tPGM reload required";       return 1;
    case  2: pS="E\tBad external tag references"; return 1;
    case  3: pS="W\tEngineering units invalid"; return 1;
    case  4: pS="E\tRun time error"; return 1;
    case  5: pS="E\tMath run time error"; return 1;
    default:                                               
      return FlwNode::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------
                                          
dword CElecBlk::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  };

//==========================================================================
