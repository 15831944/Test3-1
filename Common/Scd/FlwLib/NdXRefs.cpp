#include "stdafx.h"

#define __NDXREFS_CPP

#include "NdXRefs.h"
#include "flwnode.h"
#include "..\..\..\smdk\include\md_share3.h"
#include "scd_wm.h"

#if WithPGMTagChange
#include < fstream >
#include <RegexWrap.h>
#pragma LIBCOMMENT("..\\..\\scd\\RegexWrap\\", "\\RegexWrap" )
#endif

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
  for (int i=0; i<m_ProcMngr.m_ExtraWatchArray.GetSize() && i<MaxProcArrayWatch; i++)
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
    GCArrayWatch* pAW = m_ProcMngr.m_ExtraWatchArray[i];
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
    DDB.String("State",           "", DC_, "",    &m_ProcMngr.m_StateLine[0],  m_pNd, noSnap|noFile);
    DDB.Text("Error:");
    DDB.String("Msg_1",           "", DC_, "",    &m_ProcMngr.m_StateLine[1],  m_pNd, noSnap|noFile);
    DDB.String("Msg_2",           "", DC_, "",    &m_ProcMngr.m_StateLine[2],  m_pNd, noSnap|noFile);
    DDB.String("Msg_3",           "", DC_, "",    &m_ProcMngr.m_StateLine[3],  m_pNd, noSnap|noFile);
    DDB.String("Msg_4",           "", DC_, "",    &m_ProcMngr.m_StateLine[4],  m_pNd, noSnap|noFile);
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
      for (int i=0; i<m_ProcMngr.m_FilesUsed.GetLen(); i++)
        {
        Strng Fn, Tg;
        Fn.FnName(m_ProcMngr.m_FilesUsed[i]());
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
    CountWatchedVars(m_ProcMngr.m_pVarList, nTtlCnt); 
    SetMaxVarData(Max(32, nTtlCnt));
    const int MaxPerPage = Max(30, (int)(nTtlCnt/(MaxAccWndTabPages-1)));
    //now add the watched variables..
    DDBAddWatchedVars(DDB, "", m_ProcMngr.m_pVarList, 0, nPg, nOnPg, 0, MaxPerPage);

    //now add 'special' watched array variables..
    if (m_ProcMngr.m_ExtraWatchArray.GetSize()>0)
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
          Strng FileName=m_ProcMngr.m_FilesUsed[inc];
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
      else if (DCB.lHandle>=xidPRCArrayWatch && DCB.lHandle<xidPRCArrayWatch+m_ProcMngr.m_ExtraWatchArray.GetSize())
        {
        GCArrayWatch* pAW = m_ProcMngr.m_ExtraWatchArray[DCB.lHandle-xidPRCArrayWatch];
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
    m_bReloadReqd=!m_ProcMngr.Debug(AF_All|AF_BackupFiles, ss(), m_pNd, DefGenConSet);
  else
    m_bReloadReqd=!m_ProcMngr.Load(AF_All|AF_BackupFiles, ss(), m_pNd, DefGenConSet);

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
//      m_ProcMngr.InitPBConvergence();
    CGExecContext ECtx(m_pNd);
    ECtx.dIC_Time = ICGetTime();
    ECtx.dIC_dTime = XRunning() ? ICGetTimeInc() : 0.0;
    ECtx.m_HoldNearXRefGet = !DoGet;
    ECtx.m_HoldNearXRefSet = !DoSet;

    //flag Initialise = (m_bMustInit && !XRunning() && XStarting());
    flag Initialise = (m_bMustInit && !XRunning());
    ECtx.SetFlags(Initialise, m_bIterOne, m_bMustTerm && !XRunning(), m_bJustLoaded, m_bEmpty, m_bPreset);
       
    m_ProcMngr.Execute(ECtx, What); 
    if (m_ProcMngr.m_ProcExecuted)
      m_lProcsFound|=ProcIDMask;

    m_pNd->ClrCI(4);
    if (m_ProcMngr.m_iRunTimeErr==1)
      m_pNd->SetCI(4, "E\tRun time error during iteration %d", m_iIterCnt);
    else if (m_ProcMngr.m_iRunTimeErr>1)
      m_pNd->SetCI(4, "E\t%d run time errors during iteration %d", m_ProcMngr.m_iRunTimeErr, m_iIterCnt);
    m_pNd->ClrCI(5);
    if (m_ProcMngr.m_iMathRunTimeErr==1)
      m_pNd->SetCI(5, "E\tMath run time error during iteration %d", m_iIterCnt);
    else if (m_ProcMngr.m_iMathRunTimeErr>1)
      m_pNd->SetCI(5, "E\t%d math run time errors during iteration %d", m_ProcMngr.m_iMathRunTimeErr, m_iIterCnt);
    for (int i=0; i<MaxCondMsgs; i++)
      {
      if (m_ProcMngr.m_CINOn[i])
        m_pNd->SetCI(6+i, "N\t%s", m_ProcMngr.m_CINMsgs[i]());
      else
        m_pNd->ClrCI(6+i);
      if (m_ProcMngr.m_CIEOn[i])
        m_pNd->SetCI(6+MaxCondMsgs+i, "E\t%s", m_ProcMngr.m_CIEMsgs[i]());
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

//--------------------------------------------------------------------------

flag CNodeProcedures::GetOtherData(FilingControlBlock &FCB)
  {
  if (FCB.SaveAs() && m_hProcess[0])
    {//during a save-as we must reset the link to the "old" edit files...
    //LogNote(Tag(), 0, "Remove link to currently open pgm file. (File should be closed)");
    //this is probably not needed for "far-away" files where location does not change
    for (int incs=0; incs<TP_MaxTtlIncludeFiles; incs++)
      {
      m_hProcess[incs] = NULL;
      m_dwProcessId[incs] = 0;
      m_EditTime[incs].dwLowDateTime=0;
      m_EditTime[incs].dwHighDateTime=0; 
      }
    }
  
  return m_ProcMngr.SaveScenario(FCB);
  }

//--------------------------------------------------------------------------

flag CNodeProcedures::PutOtherData(FilingControlBlock &FCB)
  {
  return m_ProcMngr.LoadScenario(FCB);
  }

//---------------------------------------------------------------------------

void CNodeProcedures::OnAppActivate(BOOL bActive) 
  { 
  if (m_bOn && bActive)
    {//perhaps auto-reload PGM if date/time changed...
    Strng sFn;
    flag Reload=false;
    if (m_sProcName() && (m_EditTime[0].dwLowDateTime || m_EditTime[0].dwHighDateTime))
      {
      if (sFn.FnExpandQueryReload(m_sProcName(), m_EditTime[0]))
        {
        Reload=true;
        m_EditTime[0].dwLowDateTime=0;
        m_EditTime[0].dwHighDateTime=0; 
        }
      }
    for (int i=0; i<m_ProcMngr.m_FilesUsed.GetSize(); i++)
      if (sFn.FnExpandQueryReload(m_ProcMngr.m_FilesUsed[i](), m_ProcMngr.m_FilesTime[i]))
        Reload=true;
    
    if (Reload)
      {
      m_bReloadReqd=true;
      if (m_bAutoReload)
        {
        LoadProc(m_sProcName(), m_bWithDBG);
        if (1)
          {
          char* pTag = new char[strlen(m_pNd->Tag())+1];
          strcpy(pTag, m_pNd->Tag());
          ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCREFRESHSAVE, (LPARAM)pTag);
          }
        }
      ValidateDataBlk VDB;
      ValidateData(VDB);
      }
    }
  }

//--------------------------------------------------------------------------

int CNodeProcedures::FilesUsed(CFilesUsedArray & Files)
  {
  if (m_sProcName.Len()>0)
    {
    Strng ss;
    Files.AddFile(FnExpand(ss, m_sProcName()), FU_CopyFile|FU_EditTxt);
    const int RefFilesUsed = m_ProcMngr.m_FilesUsed.GetSize();
    for (int i=1; i<RefFilesUsed; i++)
      Files.AddFile(FnExpand(ss, m_ProcMngr.m_FilesUsed[i]()), FU_CopyFile|FU_EditTxt);
    return RefFilesUsed;
    }
  return 0;
  }

// -------------------------------------------------------------------------
#if WithPGMTagChange
string IntToString(int num)
{
  ostringstream myStream; //creates an ostringstream object
  myStream << num << flush;
  /*
   * outputs the number into the string stream and then flushes
   * the buffer (makes sure the output is put into the stream)
   */
  return(myStream.str()); //returns the string form of the stringstream object
}
#endif

// Replaces pOldTag for pNewTag.  Renames the original file as fn.bck
void CNodeProcedures::DoTextFileChangeTag(Strng fn, pchar pOldTag, pchar pNewTag, int& TagChangeCount, bool lookInComments, bool listChanges)
{
  TagChangeCount = 0;
#if WithPGMTagChange
  string lineList = "";
  const int BUFSIZE = 10000;
  char buf[BUFSIZE];

  CString out;
  CString regexStart = "(?<=[\\(\\[\\'\\\"\\.])";
  CString regexTag   = pOldTag;
  CString regexEnd   = "(?=[\\)\\]\\'\\\"\\.])";

  CString commentRegex = "^[ \t]*;";

  CString regexStr = regexStart + regexTag + regexEnd;
  
  CString fnIn = fn();
  CString fnTmp = fnIn + ".tmp";
  CString fnBck = fnIn + ".bck";

  ifstream pgmIn(fnIn);
  ofstream pgmOut(fnTmp);

  if (pgmIn.eof())
  {
    // complain...
  }
  else
  {
    CMRegex *regex = new CMRegex();

    int lineNo = 0;

    while (!pgmIn.eof())
    {
      lineNo++;

      pgmIn.getline(buf, BUFSIZE-1);

      if ((lookInComments)||(!regex->IsMatch(buf, commentRegex)))
      {
      out = regex->Replace(buf, regexStr, pNewTag, CMRegex::IgnoreCase);

        int matches = regex->Matches(buf, regexStr).Count();
        if (matches > 0)
        {
          lineList += IntToString(lineNo) + ", ";
          TagChangeCount += matches;
        }
      }

      pgmOut << out;
      if (!pgmIn.eof())
        pgmOut << endl;
    }

    delete regex;
  }

  if (listChanges)
  {
    pgmOut << endl;
    pgmOut << ";" + string(CTime::GetCurrentTime().Format("%x %X")) + ":  '" + string(pOldTag) + "' changed to '" + string(pNewTag) + "'.  " + IntToString(TagChangeCount)
              + " occurances on lines " + lineList.substr(0, lineList.length()-2) + ".";
    // %x %X -- uses currently defined time/date format for current locale...
  }

  pgmIn.close();
  pgmOut.close();

  remove(fnBck);
  rename(fnIn, fnBck);
  rename(fnTmp, fnIn);
#endif
}

//--------------------------------------------------------------------------

int CNodeProcedures::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (m_sProcName.Len()>0)
    {
    Strng fn;
	  int TagChangeCount;

    FnExpand(fn, m_sProcName());
    DoTextFileChangeTag(fn(), pOldTag, pNewTag, TagChangeCount, true, true);
    if (TagChangeCount>0)
      {
      LogNote(m_pNd->Tag(), 0, "%d occurances of %s changed in %s", TagChangeCount, pOldTag, m_sProcName());
      }

    const int RefFilesUsed = m_ProcMngr.m_FilesUsed.GetSize();
    for (int i=1; i<RefFilesUsed; i++)
      {
      FnExpand(fn, m_ProcMngr.m_FilesUsed[i]());
      //pkh todo 
      //DoTextFileChangeTag(fn(), pOldTag, pNewTag, ResultData);
      //if (ResultData.iTagChangeCount>0)
      //  {
      //  LogNote(Tag(), 0, "%d occurances of %s changed in %s", ResultData.iTagChangeCount, pOldTag, m_ProcMngr.m_FilesUsed[i]());
      //  }
      }
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CNodeProcedures::DeleteTag(pchar pDelTag)
  {
  return EODT_DONE;
  }

#endif

//===========================================================================
//
//
//
//===========================================================================

#if WITHNODETAGIO

CNodeTagIOItem::CNodeTagIOItem(FlwNode *pNd, bool Subscribed, LPCSTR Tag, LPCSTR Name, long IOFlags) : m_Var(BXO_Blank)
  {
  m_pNd          = pNd;
  m_bSubscribed  = Subscribed;

  if (pNd->m_pTagIO==NULL)
    pNd->OpenTagIO();

  m_pTagIO       = pNd->m_pTagIO;
  m_MyPos        = NULL;

  m_lIdNo        = m_pTagIO->m_lIdCount++;
  m_lUserHandle  = 0;

  m_iTypeRead    = tt_NULL;
  m_iCnvRead     = 0;
  
  m_bTagChanged  = false;
  m_bValid       = false; 
  m_bInUse       = false; 
  m_bValueValid  = false;

  SetTag(Tag);

  m_sName        = Name;
  if (m_sName.GetLength()==0) 
    m_sName.Format("Tag%03i", m_lIdNo);
  
  m_IOFlags      = IOFlags;

  if (m_bSubscribed)
    m_pTagIO->Add(this);
  }

//---------------------------------------------------------------------------

CNodeTagIOItem::~CNodeTagIOItem()
  {
  if (m_bSubscribed)
    m_pTagIO->Remove(this);
  }

//---------------------------------------------------------------------------

void CNodeTagIOItem::SetTag(LPCSTR NewTag)
  {
  Strng sTag, sCnv;
  TaggedObject::SplitTagCnv((LPSTR)(NewTag?NewTag:""), sTag, sCnv);

  bool bTagChanged=false;
  if (sTag.GetLength()>0)
    bTagChanged = m_sTagOnly.CompareNoCase(sTag())!=0;
  else
    bTagChanged = m_sTagOnly.GetLength()>0;

  if (bTagChanged)
    {
    m_bTagChanged=true;
    if (m_bSubscribed)
      {
      m_sTagOnly = sTag();
      m_sFullTag = m_sTagOnly;
      }
    else
      {
      m_sTagOnly = sTag();
      if (sCnv.Len())
        m_sFullTag.Format("%s (%s)", sTag(), sCnv());
      else
        m_sFullTag = m_sTagOnly;
      }

    m_sCnv         = sCnv();

    if (m_bSubscribed)
      m_pTagIO->m_bValidateReqd=true;
    }
  };

//---------------------------------------------------------------------------

long CNodeTagIOItem::Configure(long UserHandle, LPCSTR ItemTag, LPCSTR ItemName, long IOFlags)
  {
  m_lUserHandle= UserHandle;
  m_bValid = false;

  if (ItemTag && ItemTag[0])
    SetTag(ItemTag);

  m_IOFlags = IOFlags;
  m_sName = ItemName;

  MTagIOResult Res=CheckTag();
  if (Res==MTagIO_OK)
    {
    if (!m_bInUse || m_bTagChanged)
      {
      m_bTagChanged = false;
      m_bInUse = true; 
      m_pTagIO->m_nCount++;
      m_pNd->MyTagsHaveChanged();

      m_Var.SetVar(m_sTagOnly.GetBuffer(), false, NULL, NULL);
      }
    }
  return Res;
  }

//---------------------------------------------------------------------------

MTagIOResult CNodeTagIOItem::CheckTag()
  {
  if (m_sName.GetLength()==0) 
    m_sName.Format("Tag%03i", m_lIdNo);

  if (m_pNd->m_fGlobalConnectBusy)
    return MTagIO_GlblConnectBusy;

  m_bValid = false;

  if (m_sTagOnly.GetLength()==0)
    return MTagIO_NotSpecified;

  CXM_Route ObjRoute;
  CXM_ObjectTag ObjTag((LPSTR)(LPCSTR)m_sTagOnly, TABOpt_ValCnvs);
  CXM_ObjectData ObjData;
  long Ret=MTagIO_OK;
  if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, ObjRoute))
    {
    CPkDataItem * pItem=ObjData.FirstItem();
    const byte cType = pItem->Type();
    if (IsData(cType))
      {
      m_Value = *pItem->Value();
      m_bValueValid  = true;
      m_iTypeRead    = pItem->Type();
      m_iCnvRead     = pItem->CnvIndex();

      if (m_sCnv.GetLength()>0 && Cnvs[m_iCnvRead]->Find((LPSTR)(LPCSTR)m_sCnv)==NULL)
        return MTagIO_BadCnv;

      m_bValid       = true;

      return MTagIO_OK;
      }
    return MTagIO_NotAllowed;
    }
  return MTagIO_NotFound;
  };

//---------------------------------------------------------------------------

MTagIOResult CNodeTagIOItem::ReadValue()
  {
  if (m_pNd->m_fGlobalConnectBusy)
    return MTagIO_GlblConnectBusy;

  m_bValid       = false;

  CXM_Route ObjRoute;
  CXM_ObjectTag ObjTag((LPSTR)(LPCSTR)m_sTagOnly, TABOpt_ValCnvs);
  CXM_ObjectData ObjData;
  if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, ObjRoute))
    {
    CPkDataItem * pItem=ObjData.FirstItem();
    const byte cType = pItem->Type();
    if (IsData(cType))
      {
      m_Value = *pItem->Value();
      m_bValueValid  = true;
      m_iTypeRead    = pItem->Type();
      m_iCnvRead     = pItem->CnvIndex();

      if (m_sCnv.GetLength()>0 && Cnvs[m_iCnvRead]->Find((LPSTR)(LPCSTR)m_sCnv)==NULL)
        return MTagIO_BadCnv;

      m_bValid       = true;
      return MTagIO_OK;
      }
    return MTagIO_NotAllowed;
    }
  return MTagIO_NotFound;
  };

//---------------------------------------------------------------------------

MTagIOResult CNodeTagIOItem::WriteValue()
  {
  if (m_pNd->m_fGlobalConnectBusy)
    return MTagIO_GlblConnectBusy;

  CXM_Route ObjRoute;
  CXM_ObjectTag ObjTag((LPSTR)(LPCSTR)m_sTagOnly, TABOpt_ValCnvs);
  CXM_ObjectData ObjData;
  if (gs_Exec.XReadTaggedItem(NULL, ObjTag, ObjData, ObjRoute))
    {
    CPkDataItem * pItem=ObjData.FirstItem();
    const byte cType = pItem->Type();
    if (IsData(cType))
      {
      //m_Value = *pItem->Value();
      m_bValueValid  = true;
      m_iTypeRead    = pItem->Type();
      m_iCnvRead     = pItem->CnvIndex();

      if (m_sCnv.GetLength()>0 && Cnvs[m_iCnvRead]->Find((LPSTR)(LPCSTR)m_sCnv)==NULL)
        return MTagIO_BadCnv;

      m_bValid       = true;

      PkDataUnion DU;
      switch (m_Value.TIOType)
        {
        case MTagType_Long:    DU.SetTypeLong(tt_Long, m_Value.Long);    break;
        case MTagType_Double:  DU.SetTypeDouble(tt_Double, m_Value.DoubleSI);    break;
        case MTagType_String:  DU.SetTypeString(tt_Strng, (LPSTR)m_Value.String);    break;
        }
      CXM_ObjectData ObjData(0, 0, (LPSTR)(LPCSTR)m_sTagOnly, 0, DU);
      if (gs_Exec.XWriteTaggedItem(NULL, ObjData, ObjRoute)==TOData_OK)
        return MTagIO_OK;
      return MTagIO_NotAllowed;
      }
    return MTagIO_NotAllowed;
    }
  return MTagIO_NotFound;
  };

//===========================================================================
//
//
//
//===========================================================================

CNodeTagIOList::CNodeTagIOList(FlwNode * pNd)
  {
  m_pNd=pNd;
  m_nCount=0;
  m_lIdCount=0;
  
  m_bShowTags=false;
  m_bValidateReqd=true;
  dbgpln("CNodeTagIOList::CTOR");
  }

//---------------------------------------------------------------------------

CNodeTagIOList::~CNodeTagIOList()
  {
  dbgpln("CNodeTagIOList::DTOR");
  RemoveAll();
  }

//---------------------------------------------------------------------------

void CNodeTagIOList::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginObject(m_pNd, "TagIO", "NdTagIO", 0, DDB_RqdPage))
    {
    DDB.Text(" ");
    POSITION Pos=m_Items.GetHeadPosition();
    while (Pos)
      {
      CNodeTagIOItem *p=m_Items.GetNext(Pos);
      if (p && p->m_bInUse)
        {
        DWORD Opts=(p->m_IOFlags & (MTagIO_Parm|MTagIO_Set))==(MTagIO_Parm|MTagIO_Set) ? isParm : 0;
        double xxx=-123;
        DDB.Double((LPSTR)p->Name(), "", p->CnvIndex(), (LPSTR)p->CnvText(), &xxx/*p->m_DblValue*/, m_pNd, Opts|NAN_OK|noFileAtAll);
        if (m_bShowTags)
          DDB.TagComment((LPSTR)p->TagOnly());
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

bool CNodeTagIOList::StartValidate()
  {
  return true;
  };
bool CNodeTagIOList::EndValidate()
  {
  m_bValidateReqd=false;
  return true;
  };

bool CNodeTagIOList::ValidateReqd()
  {
  if (m_bValidateReqd)
    {
    bool DoIt = !m_pNd->m_fGlobalConnectBusy && !m_pNd->m_bFileIOBusy;
    if (DoIt)
      {}//m_bValidateReqd=false;
    else
      m_pNd->m_fRevalidate=true; 
    return DoIt; 
    }
  return false;
  }

//---------------------------------------------------------------------------

void CNodeTagIOList::Add(CNodeTagIOItem * pItem)
  {
  ASSERT(pItem->m_MyPos==NULL);

  m_bValidateReqd=true;
  pItem->m_MyPos=m_Items.AddTail(pItem);
  };        

//---------------------------------------------------------------------------

void CNodeTagIOList::Remove(CNodeTagIOItem * pItem)
  {
  ASSERT(pItem->m_MyPos!=NULL);

  m_bValidateReqd=true;
  //pItem->m_lIdNo=m_Items.GetCount();
  m_Items.RemoveAt(pItem->m_MyPos);
  pItem->m_MyPos=NULL;

  if (pItem && pItem->m_bInUse)
    {
    pItem->m_bInUse=false;
    m_nCount--;
    m_pNd->MyTagsHaveChanged();
    }
  };        

//---------------------------------------------------------------------------

void CNodeTagIOList::RemoveAll()
  {
  POSITION Pos=m_Items.GetHeadPosition();
  while (Pos)
    {
    CNodeTagIOItem *pRemove =m_Items.GetNext(Pos);
    if (pRemove && pRemove->m_bInUse)
      {
      pRemove->m_bInUse=false;
      m_nCount--;
      m_pNd->MyTagsHaveChanged();
      }
    }
  };

//---------------------------------------------------------------------------

void CNodeTagIOList::UpdateList()
  {
  };

//---------------------------------------------------------------------------

int CNodeTagIOList::UpdateXRefLists(CXRefBuildResults & Results)
  {
  dbgpln("CNodeTagIOList::UpdateXRefLists");
  //FnMngrClear();
  int FunctNo = 0;
  POSITION Pos=m_Items.GetHeadPosition();
  while (Pos)
    {
    CNodeTagIOItem *p=m_Items.GetNext(Pos);
    if (p && p->m_bValid)
      {
      CString Id;
      Id.Format("%s [%s]", m_pNd->FullObjTag(), p->Name());
      bool DoingSet = (p->m_IOFlags & MTagIO_Set)!=0;
      int RetCode = p->m_Var.UpdateXRef(p, DoingSet, !DoingSet,//true /*Always Get the latest*/, 
        FunctNo, m_pNd, -1, p->Name(),Id, "CNodeTagList:Output", Results);
      if (RetCode!=BXR_OK)
        {
        p->m_bValid = 0;
        }
      }
    }
  //FnMngrTryUpdateXRefLists(Results);

  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CNodeTagIOList::UnlinkAllXRefs()
  {
  dbgpln("CNodeTagIOList::UnlinkAllXRefs");
  //FnMngrClear();
  //FnMngrTryUnlinkAllXRefs();
  POSITION Pos=m_Items.GetHeadPosition();
  while (Pos)
    {
    CNodeTagIOItem *p=m_Items.GetNext(Pos);
    p->m_Var.UnlinkXRefs();
    }

  UpdateList();
  //CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CNodeTagIOList::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  };

//--------------------------------------------------------------------------

void CNodeTagIOList::GetAllValues(bool CallGetNearXRefs)
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

  POSITION Pos=m_Items.GetHeadPosition();
  while (Pos)
    {
    CNodeTagIOItem *p=m_Items.GetNext(Pos);
    if (p && p->m_bInUse && p->m_bValid && (p->m_IOFlags & MTagIO_Get))
      {
      double D;
      p->m_Var.GetValue(D, true);//(p->m_lOptions &MTIO_SICnv)!=0);
      p->m_Value.DoubleSI=D;
      p->m_bValueValid = true;
      }
    }
  };

//--------------------------------------------------------------------------

void CNodeTagIOList::SetAllValues(bool CallSetNearXRefs)
  {
  POSITION Pos=m_Items.GetHeadPosition();
  while (Pos)
    {
    CNodeTagIOItem *p=m_Items.GetNext(Pos);
    if (p && p->m_bInUse && p->m_bValueValid && (p->m_IOFlags & MTagIO_Set))
      {
      p->m_Var.PutValue(p->m_Value.DoubleSI, true);
      p->m_bValueValid = false;
      }
    }

  if (CallSetNearXRefs)
    m_pNd->SetNearXRefValues();
  };

//--------------------------------------------------------------------------

LPCSTR CNodeTagIOList::ResultString(MTagIOResult Res)
  {
  switch (Res)
    {
    case MTagIO_OK:               return "OK";
    case MTagIO_NotSpecified:     return "Not Specified";
    case MTagIO_NotFound:         return "Not Found";
    case MTagIO_BadDataType:      return "Bad DataType";
    case MTagIO_NotAllowed:       return "Not Allowed";
    case MTagIO_ReadOnly:         return "Read Only";
    case MTagIO_BadCnv:           return "Bad Conversion";
    case MTagIO_WriteFail:        return "Write Fail";
    case MTagIO_NoSpace:          return "No Space";
    case MTagIO_InternalError:    return "Internal Error";
    case MTagIO_GlblConnectBusy:  return "GlblConnect Busy";
    case MTagIO_FileIOBusy:       return "FileIO Busy";
    default:                      return "Unknown Result";
    }
  };                           

#endif

//===========================================================================
//
//
//
//===========================================================================
