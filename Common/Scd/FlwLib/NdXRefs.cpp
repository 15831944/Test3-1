#include "stdafx.h"

#define __NDXREFS_CPP

#include "NdXRefs.h"
#include "flwnode.h"
#include "..\..\..\smdk\include\md_share3.h"

#if WITHNODEPROCS

XID xidPRCName         = ModelXID(100000);
XID xidPRCPath         = ModelXID(100001);
XID xidEditProcBtn      = ModelXID(100002);
XID xidReloadProcBtn    = ModelXID(100003);
XID xidBrowseProcBtn    = ModelXID(100004);
XID xidTagCheckBtn      = ModelXID(100005);
XID xidEditFileBtn      = ModelXID(100010);
XID xidEditFileBtnLast  = ModelXID(100090);
XID xidEditFileNameNew  = ModelXID(100091);
XID xidEditFileBtnNew   = ModelXID(100092);

const int MaxProcArrayWatch     = 512;
//
//XID xidPRCWatch         = ModelXID(1100);                      // Keep MaxPGMWatch free after this
XID xidPRCArrayWatch   = ModelXID(100100);          // Keep MaxPGMArrayWatch free after this
XID xidPRCData         = xidPRCArrayWatch+MaxProcArrayWatch; // Keep everything free after this

//===========================================================================
//
//
//
//===========================================================================

static struct { long m_ID; LPCSTR m_Name; } s_ProcNames[]=
  {
    { NPM_InitialiseSolution,   "InitialiseSolution"  },
    { NPM_TerminateSolution,    "TerminateSolution"   },
    { NPM_StartStep,            "StartStep"           },
    { NPM_BeforeEvalProducts,   "BeforeEvalProducts"  },
    { NPM_AfterEvalProducts,    "AfterEvalProducts"   },
    { NPM_Any,                  ""                    },
  };


static const char* DefaultPRC =
";--- SysCAD Node Procedure (NPR) file ---\n"
"\n"
";--- variable declarations ---\n"
"\n"
";--- procedure declarations ---\n"
"\n"
";Function InitialiseSolution()\n"
";\n"
";  ;...\n"  
";\n"
";  return 0\n"  
";EndFunct\n"  
"\n"
";------------------------------\n"
"\n"
";Function StartStep()\n"
";\n"
";  ;...\n"  
";\n"
";  return 0\n"  
";EndFunct\n"  
"\n"
";------------------------------\n"
"\n"
";Function BeforeEvalProducts()\n"
";\n"
";  ;...\n"  
";\n"
";  return 0\n"  
";EndFunct\n"  
"\n"
";------------------------------\n"
"\n"
";Function AfterEvalProducts()\n"
";\n"
";  ;...\n"  
";\n"
";  return 0\n"  
";EndFunct\n"  
"\n"
";------------------------------\n"
"\n"
";Function TerminateSolution()\n"
";\n"
";  ;...\n"  
";\n"
";  return 0\n"  
";EndFunct\n"  
"\n"
";------------------------------\n"
"\n"
"$ ; --- end of file ---\n"
"\n";
static const int DefaultPRCLen = strlen(DefaultPRC);

static const char* DefaultPRCInc =
";--- SysCAD Node Procedure (NPR) include file ---\n"
";--- referenced in main NPR file with '>>'\n"
"\n"
";--- place functions or classes here ---\n"
"\n"
"\n"
"\n"
"; --- end of include file (Note: $ must NOT be used) ---\n"
"\n";
static const int DefaultPRCIncLen = strlen(DefaultPRCInc);

//===========================================================================
//
//
//
//===========================================================================

CNodeProcedures::CNodeProcedures(FlwNode * pNd)
  {
  m_pNd=pNd;

  m_pNd->fHasFiles = 1;
  m_bMustInit = 0;
  m_bIterOne = 0;
  m_bMustTerm = 0;
  m_iIterCnt  = 0;
  m_bJustLoaded = 0;
  m_bEmpty = 0;
  m_bPreset = 0;
  m_bAutoReload = true;
  m_bReloadReqd = false;
  m_bWithDBG = false;
  //AttachClassInfo(nc_Control, NULL, &NullFlwGroup);
  //fActiveHoldOK_Dyn=true;
  m_sProcName = "";
  m_bOn = 1;
  m_VarData = NULL;
  m_nMaxVarData = 0;
  SetMaxVarData(32L);

  for (int incs=0; incs<TP_MaxTtlIncludeFiles; incs++)
    {
    m_hProcess[incs] = NULL;
    m_dwProcessId[incs] = 0;
    m_EditTime[incs].dwLowDateTime=0;
    m_EditTime[incs].dwHighDateTime=0; 
    }

  m_lProcsChecked=0;
  m_lProcsFound=0;

  }

//---------------------------------------------------------------------------

CNodeProcedures::~CNodeProcedures()
  {
  }

//--------------------------------------------------------------------------

void CNodeProcedures::SetMaxVarData(long NewSize)
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

void CNodeProcedures::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void CNodeProcedures::CountWatchedVars(pGCVar VarList, int & Count)
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

void CNodeProcedures::DDBAddWatchedVars(DataDefnBlk & DDB, pchar pHdTag, pGCVar VarList, int Lvl, int &nPg, int &nOnPg, flag ForceNewPage, int MaxPerPage)
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
      if (ForceNewPage || (Lvl==0 && nOnPg>MaxPerPage && pVar->WhatAmI()!=VarLabel))
        {
        ForceNewPage = 0;
        sprintf(S, "Data:%i",nPg+1);
        DDB.Page(S, DDB_RqdPage);
        nOnPg = 0;
        nPg++;
        }
      nOnPg++;

      char LabelBuff[512];
      m_VarData[nUseData] = pVar;
      Tag.Set("%s%s", HdTag, pVar->Name());
      //const DDEF_Flags flgs = ((!pVar->ReadOnly() ? 0 : isParm) | noFile);
      const DDEF_Flags flgs = ((pVar->ReadOnly() ? 0 : isParm) | noFile);
      if (pVar->m_pWatchInfo)
        {
        switch (pVar->WhatAmI())
          {
          case VarTag   :
          case VarDouble: 
            DDB.Double(Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), xidPRCData+nUseData, m_pNd, flgs|NAN_OK);
            DDB.Range(pVar->m_pWatchInfo->dMin, pVar->m_pWatchInfo->dMax);
            DDB.Description(pVar->m_pWatchInfo->sDesc()); 
            break;
          case VarLong  :
          case VarByte  : 
            DDB.Long  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), xidPRCData+nUseData, m_pNd, flgs);
            DDB.Range((long)pVar->m_pWatchInfo->dMin, (long)pVar->m_pWatchInfo->dMax);
            DDB.Description(pVar->m_pWatchInfo->sDesc()); 
            break;
          case VarBit   : 
            DDB.Bool  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), xidPRCData+nUseData, m_pNd, flgs);
            DDB.Range((byte)pVar->m_pWatchInfo->dMin, (byte)pVar->m_pWatchInfo->dMax);
            DDB.Description(pVar->m_pWatchInfo->sDesc()); 
            break;
          case VarStr   : 
            DDB.String(Tag(), "", DC_, "", xidPRCData+nUseData, m_pNd, flgs); 
            break;
          case VarLabel :
            {
            if (pVar->m_flags & VarPageLabel)
              {
              DDB.Page(pVar->GetStrVal(LabelBuff), DDB_RqdPage);
              nOnPg = 0;
              nPg++;
              }
            else
              {
              DDB.Text(pVar->GetStrVal(LabelBuff)); 
              }
            break;
            }
          }
        }
      else
        {
        switch(pVar->WhatAmI())
          {
          case VarTag   :
          case VarDouble: 
            DDB.Double(Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), xidPRCData+nUseData, m_pNd, flgs|NAN_OK);
            break;
          case VarLong  :
          case VarByte  : 
            DDB.Long  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), xidPRCData+nUseData, m_pNd, flgs);
            break;
          case VarBit   : 
            DDB.Bool  (Tag(), "", pVar->CnvIndex(), pVar->CnvTxt(), xidPRCData+nUseData, m_pNd, flgs);
            break;
          case VarStr   : 
            DDB.String(Tag(), "", DC_, "", xidPRCData+nUseData, m_pNd, flgs); 
            break;
          case VarLabel : 
            {
            if (pVar->m_flags & VarPageLabel)
              {
              DDB.Page(pVar->GetStrVal(LabelBuff), DDB_RqdPage);
              nOnPg = 0;
              nPg++;
              }
            else
              {
              DDB.Text(pVar->GetStrVal(LabelBuff)); 
              }
            break;
            }
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

void CNodeProcedures::DDBAddArrayWatchedVars(DataDefnBlk & DDB, int &nPg, int MaxPerPage)
  {
  Strng Tag;
  int nOnPg = 0;
  for (int i=0; i<m_PgmMngr.m_ExtraWatchArray.GetSize() && i<MaxProcArrayWatch; i++)
    {
    if (nOnPg>MaxPerPage)
      {
      char S[8];
      sprintf(S, "D%i", nPg+1);
      DDB.Page(S, DDB_RqdPage);
      nOnPg = 0;
      nPg++;
      }
    nOnPg++;
    GCArrayWatch* pAW = m_PgmMngr.m_ExtraWatchArray[i];
    DDB.Double(pAW->m_sTg(), "", DC_, "", xidPRCArrayWatch+i, m_pNd, (pAW->m_bReadOnly ? 0 : isParm)|NAN_OK|noFile);
    }
  }
//---------------------------------------------------------------------------

void CNodeProcedures::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginObject(m_pNd, "Procs", "NdProcs", 0, DDB_RqdPage))
    {
    DDB.Text(" ");
    DDB.String("Procs_Name",      "", DC_, "",    xidPRCName,               m_pNd, isParm|SetOnChange);
    DDB.String("Procs_Path",      "", DC_, "",    xidPRCPath,               m_pNd, noFileAtAll);
    DDB.CheckBoxBtn("On",         "", DC_, "",    &m_bOn,                     m_pNd, isParmStopped, DDBYesNo);
    DDB.CheckBoxBtn("AutoReload", "", DC_, "",    &m_bAutoReload,             m_pNd, isParm, DDBOnOff);
    DDB.String("State",           "", DC_, "",    &m_PgmMngr.m_StateLine[0],  m_pNd, noSnap|noFile);
    DDB.Text("Error:");
    DDB.String("Msg_1",           "", DC_, "",    &m_PgmMngr.m_StateLine[1],  m_pNd, noSnap|noFile);
    DDB.String("Msg_2",           "", DC_, "",    &m_PgmMngr.m_StateLine[2],  m_pNd, noSnap|noFile);
    DDB.String("Msg_3",           "", DC_, "",    &m_PgmMngr.m_StateLine[3],  m_pNd, noSnap|noFile);
    DDB.String("Msg_4",           "", DC_, "",    &m_PgmMngr.m_StateLine[4],  m_pNd, noSnap|noFile);
    DDB.Text(" ");
    // These buttons should follow the ProcsName
    DDB.Button("Edit_Procs",      "", DC_, "",    xidEditProcBtn,            m_pNd, isParm);
    DDB.Button("Reload_Procs",    "", DC_, "",    xidReloadProcBtn,          m_pNd, isParm);
    DDB.Button("Browse_Procs",    "", DC_, "",    xidBrowseProcBtn,          m_pNd, isParm);
    DDB.Button("Check_tags",      "", DC_, "",    xidTagCheckBtn,           m_pNd, isParmStopped);

    // #744 Allow editting of Includes
    if (DDB.ForView())
      {
      DDB.Text("");
      for (int i=0; i<m_PgmMngr.m_FilesUsed.GetLen(); i++)
        {
        Strng Fn, Tg;
        Fn.FnName(m_PgmMngr.m_FilesUsed[i]());
        if (i==0)
          Tg = "Edit Main Proc";
        else
          Tg.Set("Edit include %d", i);
        DDB.Button(Tg(),         Fn(),   DC_,    "",    xidEditFileBtn+i,            m_pNd, isParm);
        }
      DDB.String("NewInclude",   "",       DC_,      "",    xidEditFileNameNew,            m_pNd, isParm|SetOnChange|noFileAtAll);
      if (m_sEditFileNameNew.GetLength()>0)
        DDB.Button(m_sEditFileNameNew(),  "Create", DC_,    "",    xidEditFileBtnNew,            m_pNd, isParm);
      }

    DDB.Text(" ");
    //DDB.Button("Debug_Proc",    "", DC_,    "",    xidDebugProcBtn,           m_pNd, isParm);
    //DDB.Text(" ");

    //look ahead to see how many tags need to be display & how many tab pages..
    //TODO: These calcs need improving for user defined pages, etc
    m_nVarData = 0;
    int nPg = 0;
    int nOnPg = 20;
    int nTtlCnt = 0;
    CountWatchedVars(m_PgmMngr.m_pVarList, nTtlCnt); 
    SetMaxVarData(Max(32, nTtlCnt));
    const int MaxPerPage = Max(30, (int)(nTtlCnt/(MaxAccWndTabPages-1)));
    //now add the watched variables..
    DDBAddWatchedVars(DDB, "", m_PgmMngr.m_pVarList, 0, nPg, nOnPg, 0, MaxPerPage);

    //now add 'special' watched array variables..
    if (m_PgmMngr.m_ExtraWatchArray.GetSize()>0)
      {
      nPg = 0;
      DDB.Page("D0", DDB_RqdPage);
      DDBAddArrayWatchedVars(DDB, nPg, MaxPerPage);
      }

    POSITION Pos=m_pNd->m_BuildResults.m_BadTags.GetHeadPosition();
    if (Pos)
      {
      DDB.Text(" ");
      while (Pos)
        {
        CXRefBuildResult & R=m_pNd->m_BuildResults.m_BadTags.GetNext(Pos);
        CString S(R.m_sTag);
        if (R.m_dwReason & FXR_Found           ) S+= " ";
        if (R.m_dwReason & FXR_NotFound        ) S+= " Not Found";
        if (R.m_dwReason & FXR_NotParam        ) S+= " Not Parameter";
        if (R.m_dwReason & FXR_BadValue        ) S+= " Not Number or String";
        if (R.m_dwReason & FXR_ParamStopped    ) S+= " Parameter Stopped";
        if (R.m_dwReason & FXR_BadCnv          ) S+= " Bad Cnv";
        if (R.m_dwReason & FXR_DynTag          ) S+= " DynTag";
        if (R.m_dwReason & FXR_DynTagMissing   ) S+= " DynTag Missing";
        if (R.m_dwReason & FXR_DynTagNeverFound) S+= " DynTag Never Found";
        DDB.Text((LPTSTR)(LPCTSTR)S);
        }
      }

    }
  DDB.EndObject();
  };

//--------------------------------------------------------------------------

flag CNodeProcedures::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidBrowseProcBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Ext("*.npr");
        Strng Dir = FnDefaultControlsFolder();
        CSCDFileDialog Dlg(true, NULL, Ext(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "Procs Files (*.npr)|*.npr||");
        Dlg.m_ofn.lpstrInitialDir = Dir();
        Dlg.m_ofn.lpstrTitle = "Program File";
        HWND H=::GetActiveWindow();
        if (Dlg.DoModal()==IDOK)
          m_sProcName=Dlg.GetPathName();
        ::SetActiveWindow(H);
        DCB.B=0;
        m_sProcName.FnContract();
        m_bReloadReqd=true;
        if (m_bAutoReload)
          LoadProc(m_sProcName(), m_bWithDBG);
//        ValidateData(ValidateDataBlk & VDB);
        }
      DCB.B=0;
      return 1;
    case xidReloadProcBtn:
      if (DCB.rB && (*DCB.rB!=0))
        LoadProc(m_sProcName(), False);
      DCB.B=0;
      return 1;
    case xidEditProcBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        m_EditTime[0].dwLowDateTime=0;
        m_EditTime[0].dwHighDateTime=0; 
        if (m_sProcName.Length()==0)
          {
          m_sProcName=FnDefaultControlsFolder();
          m_sProcName+=m_pNd->Tag();
          m_sProcName+=".npr";
          if (FileExists(m_sProcName()))
            {
            LoadProc(m_sProcName(), m_bWithDBG);
            }
          }
        Strng FileName(m_sProcName());
        Strng FullFilename;
        int RetCode = DCB.DoEditBtn(m_hProcess[0], m_dwProcessId[0], "", FileName(), "npr", &FullFilename, true);
        if (RetCode==-1)
          {
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            fwrite(DefaultPRC, sizeof(char), DefaultPRCLen, f);
            fclose(f);
            }
          RetCode = DCB.DoEditBtn(m_hProcess[0], m_dwProcessId[0], "", FileName(), "npr");
          }
        if (RetCode==0)
          {
          SYSTEMTIME ST;
          GetSystemTime(&ST);
          SystemTimeToFileTime(&ST, &m_EditTime[0]);
          }
        }
      DCB.B=0;
      return 1;
    case xidEditFileNameNew:
      if (DCB.rpC)
        {
        m_sEditFileNameNew=DCB.rpC;
        m_sEditFileNameNew.FnName();
        //m_sEditFileNameNew.FnName();
        }
      DCB.pC=m_sEditFileNameNew();
      return 1;
    case xidEditFileBtnNew:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng FileName=FnDefaultControlsFolder();
        
        HANDLE   hNewProcess=NULL;
        DWORD    dwNewProcessId;

        FileName+=m_sEditFileNameNew;
        FileName+=".npr";
        Strng FullFilename;
        int RetCode = DCB.DoEditBtn(hNewProcess, dwNewProcessId, "", FileName(), "npr", &FullFilename, true);
        if (RetCode==-1)
          {
          FILE* f = fopen(FullFilename(), "wt");
          if (f)
            {
            fwrite(DefaultPRCInc, sizeof(char), DefaultPRCIncLen, f);
            fclose(f);
            }
          RetCode = DCB.DoEditBtn(hNewProcess, dwNewProcessId, "", FileName(), "npr");
          }
        m_sEditFileNameNew="";
        }
      DCB.B=0;
      return 1;
    //case xidDebugProcsBtn:
    //  if (DCB.rB && (*DCB.rB!=0))
    //    LoadProc(m_sProcName(), True);
    //  DCB.B=0;
    //  return 1;
    case xidTagCheckBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        if (PreStartCheck())
          LogNote(m_pNd->Tag(), 0, "No bad external tag references");
        m_bIterOne = 0;
        }
      DCB.B=0;
      return 1;
   
    case xidPRCName:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot
        Strng File0, Path0;
        File0.FnNameExt(DCB.rpC);
        Path0.FnDrivePath(DCB.rpC);

        Strng File1, Path1;
        File1.FnNameExt(m_sProcName());
        Path1.FnDrivePath(m_sProcName());
        
        Strng S;
        S=Path0.Length()>0 ? Path0() : Path1();
        S+=File0.Length()>0 ? File0() : File1();

        LoadProc(S(), False);
        }
      if (DCB.ForView())
        {
        m_sProcFile.FnNameExt(m_sProcName());
        DCB.pC= m_sProcFile();
        }
      else
        DCB.pC= m_sProcName();
      return 1;
    
    case xidPRCPath:
      m_sProcPath.FnDrivePath(m_sProcName());
      if (SymbolicPaths())
        m_sProcPath.FnContract();
      else
        m_sProcPath.FnExpand();
      DCB.pC= m_sProcPath();
      return 1;

    #if WithProcWatches
    case xidWatchesOn:
      if (DCB.rB)
        bWatchesOn=*DCB.rB;
      DCB.B=bWatchesOn;
      return 1;
    #endif
    default:
      if (DCB.lHandle>=xidEditFileBtn && DCB.lHandle<=xidEditFileBtnLast)
        {
        if (DCB.rB && (*DCB.rB!=0))
          {
          int inc=DCB.lHandle-xidEditFileBtn;
          Strng FileName=m_PgmMngr.m_FilesUsed[inc];
          //EditTime.dwLowDateTime=0;
          //EditTime.dwHighDateTime=0; 
          //if (m_sProcName.Length()==0)
          //  {
          //  m_sProcName=FnDefaultControlsFolder();
          //  m_sProcName+=Tag();
          //  m_sProcName+=".npr";
          //  if (FileExists(m_sProcName()))
          //    {
          //    LoadProc(m_sProcName(), m_bWithDBG);
          //    }
          //  }
          //Strng FileName(m_sProcName());
          Strng FullFilename;
          int RetCode = DCB.DoEditBtn(m_hProcess[inc], m_dwProcessId[inc], "", FileName(), "npr", &FullFilename, true);
          if (RetCode==-1)
            {
            FILE* f = fopen(FullFilename(), "wt");
            if (f)
              {
              if (DCB.lHandle==xidEditFileBtn)
                fwrite(DefaultPRC, sizeof(char), DefaultPRCLen, f);
              else
                fwrite(DefaultPRCInc, sizeof(char), DefaultPRCIncLen, f);
              fclose(f);
              }
            RetCode = DCB.DoEditBtn(m_hProcess[inc], m_dwProcessId[inc], "", FileName(), "npr");
            }
          //if (RetCode==0)
          //  {
          //  SYSTEMTIME ST;
          //  GetSystemTime(&ST);
          //  SystemTimeToFileTime(&ST, &EditTime);
          //  }
          }
        DCB.B=0;
        return 1;
        }

      if (DCB.lHandle>=xidPRCData && DCB.lHandle<xidPRCData+m_nVarData)
        {
        GCVar* pV = m_VarData[DCB.lHandle-xidPRCData];
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
      else if (DCB.lHandle>=xidPRCArrayWatch && DCB.lHandle<xidPRCArrayWatch+m_PgmMngr.m_ExtraWatchArray.GetSize())
        {
        GCArrayWatch* pAW = m_PgmMngr.m_ExtraWatchArray[DCB.lHandle-xidPRCArrayWatch];
        if (pAW)
          {
          if (DCB.rD && !DCB.ForFileSnpScn()) 
            pAW->set(*DCB.rD);
          DCB.D=pAW->getD();
          return True;
          }
        }
    }
  return False;
  }

//--------------------------------------------------------------------------

flag CNodeProcedures::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=true;//FlwNode::ValidateData(VDB);
  m_pNd->SetCI(35, m_bReloadReqd && m_pNd->GetActive());
  return OK;
  }

//--------------------------------------------------------------------------

flag CNodeProcedures::LoadProc(char * pProcName, flag WithDebug)
  {
  CWaitCursor Wait;
  m_bJustLoaded = 1;
  m_bWithDBG = WithDebug;
  m_bReloadReqd = false;
  m_sProcName = pProcName;
  m_sProcName.LRTrim();
  m_lProcsChecked=0;
  m_lProcsFound=0;

  Strng ss;
  if (m_sProcName())
    {
    ss.FnDrivePathName(m_sProcName());
    ss.FnCheckExtension("npr");
    ss.FnSearchExpand(NULL, AF_All|AF_BackupFiles);
    m_sProcName.FnContract(ss());
    }

  if (WithDebug)
    m_bReloadReqd=!m_PgmMngr.Debug(AF_All|AF_BackupFiles, ss(), m_pNd, DefGenConSet);
  else
    m_bReloadReqd=!m_PgmMngr.Load(AF_All|AF_BackupFiles, ss(), m_pNd, DefGenConSet);

  //long Ret = UpdateXRefLists(); 
  m_pNd->MyTagsHaveChanged();

#if WithProcWatches
  for (int i=0; i<MaxProcWatch; i++)
    Watch[i].pV=NULL;
  #endif
  return True;
  }

//--------------------------------------------------------------------------

flag CNodeProcedures::PreStartCheck()
  {
  m_bMustInit = 1;
  m_bIterOne = 0;
  m_bMustTerm = 0;
  if (m_bOn)
    {
    }
  return true;
  }


//void GControl::EvalCtrlTerminate(eScdCtrlTasks Tasks)
//  {
//  if (XRefListActive())
//    {
//    DoEvalCtrl(); 
//    }
//  }

//--------------------------------------------------------------------------

void CNodeProcedures::DoEvalProc(long ProcIDMask, bool DoGet, bool DoSet)
  {
  //dbgpln("!@@@@@@@@@@@@@@@@@@@@@@@@ %i %i %i %s", bOn, GetActiveHold(), ICGetTimeInc() > 0.0, Tag());
 
  LPCSTR What=NULL;
  for (int iP=0; What==NULL; iP++)
    {
    if (s_ProcNames[iP].m_ID==ProcIDMask)
      What=s_ProcNames[iP].m_Name;     
    };
  ASSERT_ALWAYS(What!=NULL, "Missing Procedure", __FILE__, __LINE__);

  if (m_bOn && !m_pNd->GetActiveHold() && ICGetTimeInc() > 0.0)
    {
    m_pNd->SetCI(1, m_bReloadReqd);
//    if (bIterationOne && NetProbalMethod())
//      m_PgmMngr.InitPBConvergence();
    CGExecContext ECtx(m_pNd);
    ECtx.dIC_Time = ICGetTime();
    ECtx.dIC_dTime = XRunning() ? ICGetTimeInc() : 0.0;
    ECtx.m_HoldNearXRefGet = !DoGet;
    ECtx.m_HoldNearXRefSet = !DoSet;

    //flag Initialise = (m_bMustInit && !XRunning() && XStarting());
    flag Initialise = (m_bMustInit && !XRunning());
    ECtx.SetFlags(Initialise, m_bIterOne, m_bMustTerm && !XRunning(), m_bJustLoaded, m_bEmpty, m_bPreset);
       
    m_PgmMngr.Execute(ECtx, What); 
    if (m_PgmMngr.m_ProcExecuted)
      m_lProcsFound|=ProcIDMask;

    m_pNd->ClrCI(4);
    if (m_PgmMngr.m_iRunTimeErr==1)
      m_pNd->SetCI(4, "E\tRun time error during iteration %d", m_iIterCnt);
    else if (m_PgmMngr.m_iRunTimeErr>1)
      m_pNd->SetCI(4, "E\t%d run time errors during iteration %d", m_PgmMngr.m_iRunTimeErr, m_iIterCnt);
    m_pNd->ClrCI(5);
    if (m_PgmMngr.m_iMathRunTimeErr==1)
      m_pNd->SetCI(5, "E\tMath run time error during iteration %d", m_iIterCnt);
    else if (m_PgmMngr.m_iMathRunTimeErr>1)
      m_pNd->SetCI(5, "E\t%d math run time errors during iteration %d", m_PgmMngr.m_iMathRunTimeErr, m_iIterCnt);
    for (int i=0; i<MaxCondMsgs; i++)
      {
      if (m_PgmMngr.m_CINOn[i])
        m_pNd->SetCI(6+i, "N\t%s", m_PgmMngr.m_CINMsgs[i]());
      else
        m_pNd->ClrCI(6+i);
      if (m_PgmMngr.m_CIEOn[i])
        m_pNd->SetCI(6+MaxCondMsgs+i, "E\t%s", m_PgmMngr.m_CIEMsgs[i]());
      else
        m_pNd->ClrCI(6+MaxCondMsgs+i);
      }
    m_bJustLoaded = 0;
    m_bEmpty = 0;
    m_bPreset = 0;
    if (ECtx.DoXStop)
      {
      LogError(m_pNd->Tag(), 0, "SysCAD stopped by PGM");
      m_pNd->ExecObj()->XStop();
      }
    if (ECtx.DoXIdle)
      {
      LogError(m_pNd->Tag(), 0, "SysCAD paused by PGM");
      m_pNd->ExecObj()->XIdle();
      }
    m_bMustInit = 0;
    }
  else
    {
    m_pNd->ClrCI(4);
    m_pNd->ClrCI(5);
    for (int i=0; i<MaxCondMsgs*2; i++)
      m_pNd->ClrCI(6+i);
    }
  }

#endif

//===========================================================================
//
//
//
//===========================================================================

#if WITHNODETAGIO

CNodeTagIOList::CTagItem::CTagItem(LPCSTR Tag, LPCSTR Name, long Options) : m_Var(BXO_Blank)
  {
  Strng sTag, sCnv;
  TaggedObject::SplitTagCnv((LPSTR)Tag, sTag, sCnv);

  m_sTag         = sTag();
  m_sCnv         = sCnv();
  m_sFullTag.Format("%s (%s)", sTag(), sCnv());
  m_sName        = Name;
  if (m_sName.GetLength()==0) 
    m_sName.Format("Tag%03i", m_lIdNo);
  m_lOptions     = Options;
  m_DblValue     = dNAN;//0.0;
  m_lIdNo        = -1;
  m_bValid       = false; 
  };

CNodeTagIOList::CTagItem::~CTagItem()
  {
  };

//===========================================================================
//
//
//
//===========================================================================

CNodeTagIOList::CNodeTagIOList(FlwNode * pNd, long EstTagCount)
  {
  m_pNd=pNd;
  m_TagMap.InitHashTable(FindNextPrimeNumber(EstTagCount));
  m_NameMap.InitHashTable(FindNextPrimeNumber(EstTagCount));
  m_Items.SetSize(0,16);
  
  m_bShowTags=false;
  }

//---------------------------------------------------------------------------

CNodeTagIOList::~CNodeTagIOList()
  {
  RemoveAll();
  }

//---------------------------------------------------------------------------

void CNodeTagIOList::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginObject(m_pNd, "TagIO", "NdTagIO", 0, DDB_RqdPage))
    {
    DDB.Text(" ");
    for (int i=0; i<m_Items.GetCount(); i++)
      {
      if (m_Items[i])
        {
        CTagItem *p=m_Items[i];
        DWORD Opts=(p->m_lOptions & (TIO_Parm|TIO_Set))==(TIO_Parm|TIO_Set) ? isParm:0;
        DDB.Double((LPSTR)p->Name(), "", p->CnvIndex(), (LPSTR)p->CnvText(), &p->m_DblValue, m_pNd, Opts|NAN_OK|noFileAtAll);
        if (m_bShowTags)
          DDB.TagComment((LPSTR)p->Tag());
        }
      }
    DDB.Text(" ");
    DDB.CheckBoxBtn("ShowTags", "", DC_, "", &m_bShowTags, m_pNd, isParm);
    }
  DDB.EndObject();
  };

//---------------------------------------------------------------------------

flag CNodeTagIOList::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  };

//---------------------------------------------------------------------------

flag CNodeTagIOList::ValidateData(ValidateDataBlk & VDB)
  {
  return true;
  };

//---------------------------------------------------------------------------

long CNodeTagIOList::Add(LPCSTR ItemTag, LPCSTR Name, long Options)
  {
  if (FindTag(ItemTag)<0)
    {
    if (Name && strlen(Name)>0 && FindName(Name)>=0)
      return -2;

    CTagItem * p = new CTagItem(ItemTag, Name, Options);
    m_TagMap.SetAt(p->m_sTag, p);
    m_NameMap.SetAt(p->m_sName, p);
    p->m_Var.SetVar(p->m_sFullTag.GetBuffer(), false, NULL, NULL);

    for (int i=0; i<m_Items.GetCount(); i++)
      {
      if (m_Items[i]==NULL)
        {
        m_Items[i]=p;
        p->m_lIdNo=i;
        p->m_bValid = true; 
        return i;
        }
      }
    p->m_lIdNo=m_Items.Add(p);
    p->m_bValid = true;
    return m_Items.GetCount()-1;
    }
  return -1;
  }

//---------------------------------------------------------------------------

bool CNodeTagIOList::Remove(long Index)
  {
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    {
    CTagItem * pRemove = m_Items[Index];
    m_Items[Index]=NULL;

    CTagItem * p;
    if (m_TagMap.Lookup(pRemove->m_sTag, p))
      m_TagMap.RemoveKey(pRemove->m_sTag);
    if (m_NameMap.Lookup(pRemove->m_sName, p))
      m_NameMap.RemoveKey(pRemove->m_sName);
 
    delete pRemove;
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CNodeTagIOList::RemoveAll()
  {
  for (int i=0; i<m_Items.GetCount(); i++)
    delete m_Items[i];
  m_Items.SetSize(0,16);
  m_TagMap.RemoveAll();
  m_NameMap.RemoveAll();
  };

//---------------------------------------------------------------------------

long CNodeTagIOList::GetCount()
  {
  return m_TagMap.GetCount();
  };

//---------------------------------------------------------------------------

long CNodeTagIOList::FindTag(LPCSTR ItemTag)
  {
  CTagItem * p;
  if (m_TagMap.Lookup(ItemTag, p))
    return p->m_lIdNo;
  return -1;
  }

//---------------------------------------------------------------------------

long CNodeTagIOList::FindName(LPCSTR Name)
  {
  CTagItem * p;
  if (m_NameMap.Lookup(Name, p))
    return p->m_lIdNo;
  return -1;
  }

//---------------------------------------------------------------------------

long CNodeTagIOList::GetType(long Index)         
  { 
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    return -1;//m_Items[Index]->m_Var.mm_sTag; 
  return -1;
  }


LPCSTR CNodeTagIOList::GetTag(long Index)         
  { 
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    return m_Items[Index]->m_sTag; 
  return "?";
  }

LPCSTR CNodeTagIOList::GetFullTag(long Index)         
  { 
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    return m_Items[Index]->m_sFullTag; 
  return "?";
  }

LPCSTR CNodeTagIOList::GetCnvText(long Index)     
  { 
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL && m_Items[Index]->m_Var.m_pXRef)
    return m_Items[Index]->m_sCnv; 
  return "";                    
  }

long CNodeTagIOList::GetOptions(long Index)     
  {
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    return m_Items[Index]->m_lOptions; 
  return 0;
  }

double CNodeTagIOList::GetDValue(long Index, bool UseCnv)
  {
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    {
    CTagItem * pItem = m_Items[Index];
    if (!UseCnv || pItem->m_Var.m_pXRef==NULL)
      return pItem->m_DblValue;
    else
      {
      CXRefItem * pXRef=pItem->m_Var.m_pXRef;
      return Cnvs[pXRef->m_iCnvIndex]->Human(pItem->m_DblValue, pXRef->m_sCnv());
      }
    }
  return dNAN;
  };

void   CNodeTagIOList::SetDValue(long Index, double Value, bool UseCnv)
  {
  if (Index>=0 && Index<m_Items.GetCount() && m_Items[Index]!=NULL)
    {
    CTagItem * pItem = m_Items[Index];
    if (!UseCnv || pItem->m_Var.m_pXRef==NULL)
      pItem->m_DblValue = Value;
    else
      {
      CXRefItem * pXRef=pItem->m_Var.m_pXRef;
      pItem->m_DblValue = Cnvs[pXRef->m_iCnvIndex]->Normal(Value, pXRef->m_sCnv());
      } 
    }
  }

bool   CNodeTagIOList::Remove(LPCSTR ItemTag)                             { return Remove(FindTag(ItemTag));              }
long   CNodeTagIOList::GetType(LPCSTR Tag)                                { return GetType(FindTag(Tag));                 }
LPCSTR CNodeTagIOList::GetFullTag(LPCSTR Tag)                             { return GetFullTag(FindTag(Tag));              }
LPCSTR CNodeTagIOList::GetCnvText(LPCSTR Tag)                             { return GetCnvText(FindTag(Tag));              }
long   CNodeTagIOList::GetOptions(LPCSTR Tag)                             { return GetOptions(FindTag(Tag));              }
double CNodeTagIOList::GetDValue(LPCSTR Tag, bool UseCnv)                 { return GetDValue(FindTag(Tag), UseCnv);       }
void   CNodeTagIOList::SetDValue(LPCSTR Tag, double Value, bool UseCnv)   { SetDValue(FindTag(Tag), Value, UseCnv);       }

//---------------------------------------------------------------------------

int CNodeTagIOList::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)
    {
    //FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<m_Items.GetCount(); i++)
      {
      if (m_Items[i])
        {
        CTagItem *p=m_Items[i];
        int RetCode = p->m_Var.UpdateXRef(p, (p->m_lOptions & TIO_Set)!=0, true /*Always Get the latest*/, 
                                          FunctNo, m_pNd, -1, p->Tag(), p->Name(), "CNodeTagList:Output", Results);
        if (RetCode!=BXR_OK)
          {
          p->m_bValid = 0;
          }
        }
      }
    //FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CNodeTagIOList::UnlinkAllXRefs()
  {
  //FnMngrClear();
  //FnMngrTryUnlinkAllXRefs();
  for (int i=0; i<m_Items.GetCount(); i++)
    {
    if (m_Items[i])
      m_Items[i]->m_Var.UnlinkXRefs();
    }

  //CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CNodeTagIOList::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  //if (Tasks&CO_InitPrf)
  //  {
  //  for (int i=0; i<iCount; i++)
  //    //DataBlk[i]->DoLoad();
  //    DataBlk[i]->Prof.StartAll(ICGetTime());
  //  }
  };

//--------------------------------------------------------------------------

//void CNodeTagIOList::EvalCtrlStrategy(eScdCtrlTasks Tasks)
//  {
//  if (1)//XRefListActive() && ICGetTimeInc() > 0.0)
//    {
//    //GetNearXRefValues();
//
//    //if (FnMngrPresent())
//    //  {
//    //  //solve npr functions...
//    //  CGExecContext ECtx(this);
//    //  ECtx.dIC_Time = ICGetTime();
//    //  ECtx.dIC_dTime = ICGetTimeInc();
//    //  ECtx.OnStart = bAboutToStart;
//    //  ECtx.HoldNearXRefXfer=true;
//    //  FnMngr().Execute(ECtx);
//    //  bAboutToStart = 0;
//    //  if (ECtx.DoXStop)
//    //    {
//    //    LogError(Tag(), 0, "SysCAD stopped by function");
//    //    ExecObj()->XStop();
//    //    }
//    //  if (ECtx.DoXIdle)
//    //    {
//    //    LogError(Tag(), 0, "SysCAD paused by function");
//    //    ExecObj()->XIdle();
//    //    }
//    //  }
//
//    for (int i=0; i<m_Items.GetCount(); i++)
//      {
//      if (m_Items[i])
//        {
//        CTagItem * p=m_Items[i];
//        //if (Valid(p->ColData[j]->m_dOutput) || !HasNANFlag(p->ColData[j]->m_dOutput, NF_Free))
//        p->m_Var.PutValue(p->m_DblValue);
//        }
//      }
//    //SetNearXRefValues();
//    }
//  }

//--------------------------------------------------------------------------

void CNodeTagIOList::GetAllValues(bool CallGetNearXRefs)
  {
  if (1)//XRefListActive() && ICGetTimeInc() > 0.0)
    {
    if (CallGetNearXRefs)
      {
      m_pNd->GetNearXRefValues();

      //if (FnMngrPresent())
      //  {
      //  //solve npr functions...
      //  CGExecContext ECtx(this);
      //  ECtx.dIC_Time = ICGetTime();
      //  ECtx.dIC_dTime = ICGetTimeInc();
      //  ECtx.OnStart = bAboutToStart;
      //  ECtx.HoldNearXRefXfer=true;
      //  FnMngr().Execute(ECtx);
      //  bAboutToStart = 0;
      //  if (ECtx.DoXStop)
      //    {
      //    LogError(Tag(), 0, "SysCAD stopped by function");
      //    ExecObj()->XStop();
      //    }
      //  if (ECtx.DoXIdle)
      //    {
      //    LogError(Tag(), 0, "SysCAD paused by function");
      //    ExecObj()->XIdle();
      //    }
      //  }
      }

    for (int i=0; i<m_Items.GetCount(); i++)
      {
      CTagItem * p=m_Items[i];
      if (p && (p->m_lOptions & TIO_Get))
        p->m_Var.GetValue(p->m_DblValue, true);//(p->m_lOptions &TIO_SICnv)!=0);
      }
    }
  };

//--------------------------------------------------------------------------

void CNodeTagIOList::SetAllValues(bool CallSetNearXRefs)
  {
  if (1)//XRefListActive() && ICGetTimeInc() > 0.0)
    {
    for (int i=0; i<m_Items.GetCount(); i++)
      {
      CTagItem * p=m_Items[i];
      if (p && (p->m_lOptions & TIO_Set))
        p->m_Var.PutValue(p->m_DblValue, true);
      }

    if (CallSetNearXRefs)
      m_pNd->SetNearXRefValues();
    }
  };

#endif

//===========================================================================
//
//
//
//===========================================================================
