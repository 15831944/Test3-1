// CmdCallback.cpp: implementation of the CScdCmdIF class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#define __SCDCOMIF_CPP
#include "scdver.h"
#if WITHCOMCMD

#include "ScdCmdIF.h"

#include "resource.h"
//#include "project.h"

#include "sc_defs.h"
#include "scdver.h"
//#include "appbase.h"
#include "syscad.h"
#include "mainfrm.h"
//#include "VisGrfFrm.h"
//#include "VisGrfDoc.h"
//#include "VisGrfVw.h"
#include "project.h"
#include "scd_wm.h"
#include "mdlrunmngr.h"
//#include "prjexec.h"
#include "dbgmngr.h"
#include "grfdoc.h"
#include "licbase.h"

//#include "optoff.h"

#define dbgScdCmdIF  01

#if dbgScdCmdIF
static CDbgMngr dbgComCmds("ScdCOM", "Cmds");
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScdCmdIF::CScdCmdIF()
{
  m_iGrfWndIndex=0;

}

CScdCmdIF::~CScdCmdIF()
{

}

// {B1CC253D-6CC1-4a0f-8656-273C8DD8D0D2}
CLSID CLSID_SysCADCmd = { 0xb1cc253d, 0x6cc1, 0x4a0f, { 0x86, 0x56, 0x27, 0x3c, 0x8d, 0xd8, 0xd0, 0xd2 } };

CLSID& CScdCmdIF::clsidGetCLSID()
{
	return CLSID_SysCADCmd;
}


LPWSTR CScdCmdIF::pwszGetProgID()
{
	return L"SysCAD.SysCADCmd.1";
}

LPWSTR CScdCmdIF::pwszGetVIProgID()
{
	return L"SysCAD.SysCADCmd";
}

LPWSTR CScdCmdIF::pwszGetClassDesc()
{
	return L"SysCAD SysCADCmd";
}

HRESULT CScdCmdIF::get_UpNAbout(VARIANT_BOOL * pVal)
  {
  * pVal=Project::sm_SysCADInited;
	return S_OK;//theApp.m_UpNAbout;
  }

HRESULT CScdCmdIF::WaitUpNAbout(double TimeOut)
  {
  // Time out in Seconds
  while (!Project::sm_SysCADInited && (TimeOut>0))
    {
    ::Sleep(1000);
    TimeOut-=0.100;
    }
  if (Project::sm_SysCADInited)
    return S_OK;
  else
    return gs_ScdCmdIF.ReportError("WaitUpNAbout Timeout", 4);
  }

static inline void LclPostMessage(UINT Msg, WPARAM w, LPARAM l=0)
  {
  while (!::/*AfxGet*/MainWnd()->PostMessage(Msg, w, l))
    {
#if dbgScdCmdIF  
if (1 || dbgComCmds())
  dbgpln("###### REPOST[%i] %2i:%2i",GetLastError(),Msg, w);
#endif
    Sleep(10);
    }
  }

HRESULT CScdCmdIF::LoadProject( LPCTSTR/*PTCHAR*/ pszProjectName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### LoadProject %s",pszProjectName);
#endif
  LPTSTR S=new char[strlen(pszProjectName)+1];
  strcpy(S, pszProjectName);
  LclPostMessage(WMU_COM_CMD, ComCmd_LoadProject, (long)S);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveProject( LPCTSTR/*PTCHAR*/ pszProjectName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### LoadProject %s",pszProjectName);
#endif
  LPTSTR S=new char[strlen(pszProjectName)+1];
  strcpy(S, pszProjectName);
  LclPostMessage(WMU_COM_CMD, ComCmd_SaveProject, (long)S);
	return S_OK;
  };

HRESULT CScdCmdIF::LoadSnapshot( LPCTSTR/*PTCHAR*/ pszSnapshotName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### LoadSnapshot %s",pszSnapshotName);
#endif
  LPTSTR S=new char[strlen(pszSnapshotName)+1];
  strcpy(S, pszSnapshotName);
  LclPostMessage(WMU_COM_CMD, ComCmd_LoadSnapshot, (long)S);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveSnapshot( LPCTSTR/*PTCHAR*/ pszSnapshotName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### SaveSnapshot %s",pszSnapshotName);
#endif
  LPTSTR S=new char[strlen(pszSnapshotName)+1];
  strcpy(S, pszSnapshotName);
  LclPostMessage(WMU_COM_CMD, ComCmd_SaveSnapshot, (long)S);
	return S_OK;
  };

HRESULT CScdCmdIF::LoadScenario( LPCTSTR/*PTCHAR*/ pszScenarioName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### LoadScenario %s",pszScenarioName);
#endif
  LPTSTR S=new char[strlen(pszScenarioName)+1];
  strcpy(S, pszScenarioName);
  LclPostMessage(WMU_COM_CMD, ComCmd_LoadScenario, (long)S );
	return S_OK;
  };

HRESULT CScdCmdIF::SaveScenario( LPCTSTR/*PTCHAR*/ pszScenarioName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### SaveScenario %s",pszScenarioName);
#endif
  LPTSTR S=new char[strlen(pszScenarioName)+1];
  strcpy(S, pszScenarioName);
  LclPostMessage(WMU_COM_CMD, ComCmd_SaveScenario, (long)S );
	return S_OK;
  };

HRESULT CScdCmdIF::LoadBacktrack( LPCTSTR/*PTCHAR*/ pszBacktrackName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### LoadBacktrack %s",pszBacktrackName);
#endif
  LPTSTR S=new char[strlen(pszBacktrackName)+1];
  strcpy(S, pszBacktrackName);
  LclPostMessage(WMU_COM_CMD, ComCmd_LoadBacktrack, (long)S);
	return S_OK;
  };

HRESULT CScdCmdIF::SaveBacktrack( LPCTSTR/*PTCHAR*/ pszBacktrackName )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### SaveBacktrack %s",pszBacktrackName);
#endif
  LPTSTR S=new char[strlen(pszBacktrackName)+1];
  strcpy(S, pszBacktrackName);
  LclPostMessage(WMU_COM_CMD, ComCmd_SaveBacktrack, (long)S);
	return S_OK;
  };

HRESULT CScdCmdIF::CloseProject( )
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### CloseProject");
#endif
  LclPostMessage(WMU_COM_CMD, ComCmd_CloseProject);
	return S_OK;
  };

HRESULT CScdCmdIF::SetTag(LPCTSTR Tag, VARIANT * Value)
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### SetTag %s", Tag);
#endif
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=Value;
  LRESULT lRet=::/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_SetTag, (LPARAM)&T);
  gs_Events.Send(ComCmd_SetTag,NULL/*pCmdBlk*/,lRet);
  //return lRet==0 ? S_OK : E_FAIL;
  return lRet>=0 ? lRet : E_FAIL;
  };

HRESULT CScdCmdIF::GetTag(LPCTSTR Tag, VARIANT * Value)
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### GetTag %s", Tag);
#endif
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=Value;
  LRESULT lRet=::/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_GetTag, (LPARAM)&T);
  gs_Events.Send(ComCmd_GetTag,NULL/*pCmdBlk*/,lRet);
  if (GetComCallLogLnCnt())
    LogNote("XXX", 0, "%s", GetComCallLogLn());
  //return lRet==0 ? S_OK : E_FAIL;
  return lRet>=0 ? lRet : E_FAIL;
  };

HRESULT CScdCmdIF::put_TagValue(LPCTSTR Tag, VARIANT Value)
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### SetTag %s", Tag);
#endif
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=&Value;
  LRESULT lRet=::/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_SetTag, (LPARAM)&T);
  gs_Events.Send(ComCmd_SetTag,NULL/*pCmdBlk*/,lRet);
  return lRet==0 ? S_OK : E_FAIL;
  };

HRESULT CScdCmdIF::get_TagValue(LPCTSTR Tag, VARIANT * Value)
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### GetTag %s", Tag);
#endif
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=Value;
  LRESULT lRet=::/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_GetTag, (LPARAM)&T);
  gs_Events.Send(ComCmd_GetTag,NULL/*pCmdBlk*/,lRet);
  return lRet==0 ? S_OK : E_FAIL;
  };

HRESULT CScdCmdIF::get_TagType(LPCTSTR Tag, long *pVal)
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### TagType %s", Tag);
#endif
  if (Tag==NULL)
    {
    *pVal = -2;
    return S_OK;
    }
  if (Tag[0]==0)
    {
    *pVal = -3;
    return S_OK;
    }
  if (gs_pPrj==NULL)
    {
    *pVal = -4;
    return S_OK;
    }

  VARIANT Value;
  CScdCmdTag T;
  T.m_pTag=Tag;
  T.m_pvtValue=&Value;
  LRESULT lRet=::/*AfxGet*/MainWnd()->SendMessage(WMU_COM_CMD, ComCmd_get_TagType, (LPARAM)&T);
  gs_Events.Send(ComCmd_get_TagType,NULL/*pCmdBlk*/,lRet);
  if (lRet==0)
    {
    *pVal = Value.lVal;
    return S_OK;
    }
  *pVal = -5;
  return E_FAIL;
  };

HRESULT CScdCmdIF::StartDynamicSolve()
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### StartDynamicSolve");
#endif
  LclPostMessage(WMU_COM_CMD, ComCmd_StartDynamicSolve);
  return S_OK;
  }

HRESULT CScdCmdIF::StartProbalSolve()
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### StartProbalSolve");
#endif
  LclPostMessage(WMU_COM_CMD, ComCmd_StartProbalSolve);
  return E_FAIL;
  }

HRESULT CScdCmdIF::Start()
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### Start");
#endif
  LclPostMessage(WMU_COM_CMD, ComCmd_Start);
  return S_OK;
  }

HRESULT CScdCmdIF::Step(long IterMark, double WaitForNext)
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### Step  %i %12.4f", IterMark, WaitForNext);
#endif
  CDoOneStepInfo Info;
  Info.bHoldAdv=false;
  Info.lWaitForNextMS=(long)(WaitForNext*1000.0);
  LclPostMessage(WMU_COM_CMD, ComCmd_Step, Info.lParam);
  return S_OK;
  }

HRESULT CScdCmdIF::Stop()
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### Stop");
#endif
  LclPostMessage(WMU_COM_CMD, ComCmd_Stop);
	return S_OK;
  }

HRESULT CScdCmdIF::Idle()
  {
#if dbgScdCmdIF  
if (dbgComCmds())
  dbgpln("### Idle");
#endif
  LclPostMessage(WMU_COM_CMD, ComCmd_Idle);
	return S_OK;
  }

HRESULT CScdCmdIF::Wait(/*[in]*/ double Seconds) { return E_FAIL; };
HRESULT CScdCmdIF::WaitCount(/*[in]*/ long Iterations) { return E_FAIL; };
HRESULT CScdCmdIF::WaitTillSteadyState() { return E_FAIL; };
HRESULT CScdCmdIF::WaitTillStop() { return E_FAIL; };
HRESULT CScdCmdIF::ResetWait() { return E_FAIL; };
HRESULT CScdCmdIF::GenerateTagReport(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_FAIL; };
HRESULT CScdCmdIF::GenerateTrendReport(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_FAIL; };
HRESULT CScdCmdIF::ProcessSetTags(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_FAIL; };
HRESULT CScdCmdIF::SaveAndCloseReport(/*[in]*/ LPCTSTR Filename) { return E_FAIL; };
HRESULT CScdCmdIF::ExecuteMacro(/*[in]*/ LPCTSTR Filename, /*[in]*/ LPCTSTR Reportname) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpFilename(/*[out, retval]*/ BSTR *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpFilename(/*[in]*/ BSTR newVal) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpSort(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpSort(/*[in]*/ VARIANT_BOOL newVal) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpMaxCount(/*[out, retval]*/ long *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpMaxCount(/*[in]*/ long newVal) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpRelativeTolerance(/*[out, retval]*/ double *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpRelativeTolerance(/*[in]*/ double newVal) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpAbsoluteTolerance(/*[out, retval]*/ double *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpAbsoluteTolerance(/*[in]*/ double newVal) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpShowDifferentStrings(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpShowDifferentStrings(/*[in]*/ VARIANT_BOOL newVal) { return E_FAIL; };
HRESULT CScdCmdIF::get_CmpShowMissingTags(/*[out, retval]*/ VARIANT_BOOL *pVal) { return E_FAIL; };
HRESULT CScdCmdIF::put_CmpShowMissingTags(/*[in]*/ VARIANT_BOOL newVal) { return E_FAIL; };
HRESULT CScdCmdIF::CompareReset() { return E_FAIL; };
HRESULT CScdCmdIF::CompareScenarios(/*[in]*/ LPCTSTR Filename1, /*[in]*/ LPCTSTR Filename2) { return E_FAIL; };
HRESULT CScdCmdIF::CompareScenarioToCurrent(/*[in]*/ LPCTSTR Filename) { return E_FAIL; };
HRESULT CScdCmdIF::TestScenario(/*[in]*/ LPCTSTR Filename) { return E_FAIL; };
HRESULT CScdCmdIF::RestartHistorian() { return E_FAIL; };
HRESULT CScdCmdIF::Sleep(double Seconds) { return E_FAIL; };

HRESULT CScdCmdIF::Exit()
  {
  LclPostMessage(WMU_COM_CMD, ComCmd_Exit);
	return S_OK;
  }

HRESULT CScdCmdIF::get_IsStopped(/*[out, retval]*/ VARIANT_BOOL *pVal) 
  {
  * pVal=gs_TheRunMngr.Stopped();  
	return S_OK;
  };
HRESULT CScdCmdIF::get_IsIdling(/*[out, retval]*/ VARIANT_BOOL *pVal)
  {
  * pVal=gs_TheRunMngr.Idling();  
	return S_OK;
  };
HRESULT CScdCmdIF::get_IsRunning(/*[out, retval]*/ VARIANT_BOOL *pVal)
  {
  * pVal=gs_TheRunMngr.Running();  
	return S_OK;
  };
HRESULT CScdCmdIF::get_StepSize(/*[out, retval]*/ double *pVal) 
  { 
  * pVal=gs_Exec.StepSizeMax();  
  return S_OK;
  };
HRESULT CScdCmdIF::put_StepSize(/*[in]*/ double newVal)
  { 
  gs_Exec.SetStepSizeMax(Range(0.001,newVal,1000.0));  
  return S_OK;
  };
HRESULT CScdCmdIF::get_RealTimeOn(/*[out, retval]*/ VARIANT_BOOL *pVal) 
  { 
  * pVal=gs_Exec.RealTime();  
  return S_OK;
  };
HRESULT CScdCmdIF::put_RealTimeOn(/*[in]*/ VARIANT_BOOL newVal)
  { 
  gs_Exec.SetRealTime((BOOL)newVal);  
  return S_OK;
  };
HRESULT CScdCmdIF::get_RealTimeMultiplier(/*[out, retval]*/ double *pVal) 
  { 
  * pVal=gs_Exec.RealTimeMult();  
  return S_OK;
  };
HRESULT CScdCmdIF::put_RealTimeMultiplier(/*[in]*/ double newVal)
  { 
  gs_Exec.SetRealTimeMult(newVal);
  return S_OK;
  };
HRESULT CScdCmdIF::get_SequenceNo(/*[out, retval]*/ long *pVal) 
  { 
  * pVal=gs_Exec.m_Seq.SequenceNo();  
  return S_OK;
  };
HRESULT CScdCmdIF::put_SequenceNo(/*[in]*/ long newVal)
  { 
  gs_Exec.m_Seq.SetSequenceNo(newVal);
  return S_OK;
  };
HRESULT CScdCmdIF::SetAppWndState(/*[in]*/ long ReqdState)
  { 
  HRESULT ret=S_OK;
  switch (ReqdState)
    {
    case 1: 
      if (!/*AfxGet*/MainWnd()->SetForegroundWindow()) 
        ret=E_FAIL; 
      break;
    case 2: 
    case 3: 
    case 4: 
      {
      WINDOWPLACEMENT WP;
      /*AfxGet*/MainWnd()->GetWindowPlacement(&WP);
      switch (ReqdState)
        {
        case 2: WP.showCmd=SW_SHOWNORMAL; break; 
        case 3: WP.showCmd=SW_SHOWMAXIMIZED; break; 
        case 4: WP.showCmd=SW_SHOWMINIMIZED; break;
        }
      if (!/*AfxGet*/MainWnd()->SetWindowPlacement(&WP))
        ret=E_FAIL;
      break;
      }
    case 5: // Activate User Buttons
    case 6: // Deactivate User Buttons
      {
      gs_TheRunMngr.m_ComUIActive=(ReqdState==5);
      break;
      }
    }
  return ret;
  };

HRESULT CScdCmdIF::VariableSetReplayItem(/* [in] */ long SequenceNo,/* [in] */ LPCTSTR Tag,/* [in] */ VARIANT Value,/* [in] */ LPCTSTR CnvTxt)
  {
  gs_Exec.m_Seq.VariableSetReplayItem(SequenceNo, Tag, Value, CnvTxt);
  return S_OK;
  }
HRESULT CScdCmdIF::VariableSetReplayCtrl(/* [in] */ long Command, /* [in] */ long Param)
  {
  gs_Exec.m_Seq.VariableSetReplayCtrl(Command, Param);
  return S_OK;
  }
	
//---------------------------------------------------------------------------

class CScdCmdIF_FlagSetReset
  {
  public:
    CScdCmdIF_FlagSetReset() { ComCallStart(); };
    ~CScdCmdIF_FlagSetReset() { ComCallEnd(); };
  };

int CScdCmdIF::CheckTag(LPCTSTR Tag)
  {
  if (gs_pPrj)
    {
    if (gs_pPrj->FlwLib()->TestModelTagUnique((char*)Tag))
      return 0;
    else
      return -1;
    }
  return -2;
  }

CDocument* CScdCmdIF::GetGrfDoc(int index)
  {
  CDocument* pDoc=NULL;
  int GrfDocCnt=-1;
  POSITION PosT = ScdApp()->GetFirstDocTemplatePosition();
  bool Done = false;
  while (!Done && PosT)
    {
    CDocTemplate* pTempl = ScdApp()->GetNextDocTemplate(PosT);
    POSITION PosD = pTempl->GetFirstDocPosition();
    while (!Done && PosD)
      {
      pDoc = pTempl->GetNextDoc(PosD);
      POSITION pos = pDoc->GetFirstViewPosition();
      if (pos)
        {
        CView* pFirstView = pDoc->GetNextView(pos);
        CWnd* w = pFirstView->GetParent();
        if (pDoc->IsKindOf(RUNTIME_CLASS(CGrfDoc)))
          {
          ASSERT(w->IsKindOf(RUNTIME_CLASS(CGrfFrameWnd)));
          GrfDocCnt++;
          if (GrfDocCnt==index)
            Done = true;
          }
        }
      }
    }
  if (!Done)
    return NULL;
  return pDoc;
  }

HRESULT CScdCmdIF::InsertNode(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation )
  {
  CScdCmdIF_FlagSetReset Flg;
  RequestModelInfoRec MInfo;
  bool DataErr = (CheckTag(Tag)!=0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag exists : InsertNode '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }
  DWORD LicCat = gs_License.LicCatagories();
  int nModels = 0;
  bool Found=false;
  while (!Found && gs_pPrj->RequestModelInfoByGroupIndex(FlwUnitGrp, nModels, MInfo))
    {
    if ( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) )
      {
      if (MInfo.Name.XStrICmp(Class)==0)
        Found=true;
      }
    nModels++;
    }
  DataErr=!Found;

  CDocument* pDoc=NULL;
  if (!DataErr)
    {
    pDoc=GetGrfDoc(m_iGrfWndIndex);
    DataErr = (pDoc==NULL);
    }

  if (!DataErr)
    {
    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    CInsertBlk CB; 

    CB.ATag=Tag;
    CB.AModel=Class;
//  CB.ASymbol="XPG.default";
//  CB.ATagBase="XPG";
    CB.Pt.World.X=X;
    CB.Pt.World.Y=Y;
    CB.Pt.World.Z=Z;
    CB.Scl.X=XScale;
    CB.Scl.Y=YScale;
    CB.Scl.Z=YScale;
    CB.Rotate=(float)Rotation;
    //DXF_ENTITY e;
    //int MdlInsertErr;

    CB.ATagBase=MInfo.DefTag();
    CB.ASymbol=MInfo.DefTag();
    CB.ASymbol+=".default";

    pGDoc->GCB.DoInsertNode(&CB);
    }

  if (DataErr)
    LogError("ScdCOM", 0, "InsertNode '%s' of type '%s' failed!", Tag, Class);
  return S_OK;
  }

HRESULT CScdCmdIF::InsertLink(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag, /*[in]*/ LPCTSTR SrcTag, /*[in]*/ LPCTSTR SrcIO, /*[in]*/ LPCTSTR DstTag, /*[in]*/ LPCTSTR DstIO, /*[in]*/ double Xs, /*[in]*/ double Ys, /*[in]*/ double Zs, /*[in]*/ double Xd, /*[in]*/ double Yd, /*[in]*/ double Zd )
  {
  CScdCmdIF_FlagSetReset Flg;
  RequestModelInfoRec MInfo;
  bool DataErr = (CheckTag(Tag)!=0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag exists : InsertLink '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }
  DWORD LicCat = gs_License.LicCatagories();
  int nModels = 0;
  bool Found=false;
  while (!Found && gs_pPrj->RequestModelInfoByGroupIndex(FlwLinkGrp, nModels, MInfo))
    {
    if ( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) )
      {
      if (MInfo.Name.XStrICmp(Class)==0)
        Found=true;
      }
    nModels++;
    }
  DataErr=!Found;

  CDocument* pDoc=NULL;
  if (!DataErr)
    {
    pDoc=GetGrfDoc(m_iGrfWndIndex);
    DataErr = (pDoc==NULL);
    }

  if (!DataErr)
    {
    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    CConnectBlk CB; 

    CB.ATag=Tag;
    CB.AModel=Class;
    CB.SrcTag=SrcTag;
    CB.SrcIO=SrcIO;
    CB.DstTag=DstTag;
    CB.DstIO=DstIO;
    CB.iLineMethod=0;
    CB.NVerts=2;
    CB.Verts[0].World.X=Xs;
    CB.Verts[0].World.Y=Ys;
    CB.Verts[0].World.Z=Zs;
    CB.Verts[1].World.X=Xd;
    CB.Verts[1].World.Y=Yd;
    CB.Verts[1].World.Z=Zd;

    CB.ATagBase=MInfo.DefTag();
    CB.ASymbol=MInfo.DefTag();
    CB.ASymbol+=".default";
    
    pGDoc->GCB.DoInsertLink(&CB);
    }

  //gs_Exec.m_Seq.VariableSetReplayCtrl(Command, Param);
  if (DataErr)
    LogError("ScdCOM", 0, "InsertLink '%s' from '%s' to '%s' failed!", Tag, SrcTag, DstTag);
  return S_OK;
  }

HRESULT CScdCmdIF::CreateUnit(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag )
  { 
  CScdCmdIF_FlagSetReset Flg;
  bool DataErr = (CheckTag(Tag)!=0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag exists : CreateUnit '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }

  RequestModelInfoRec MInfo;
  DataErr = !gs_pPrj->RequestModelInfoByClassId((char*)Class, MInfo);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Unknown Class Type : CreateUnit '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }
  DWORD LicCat = gs_License.LicCatagories();
  DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
  if (DataErr || _stricmp(MInfo.Group(), FlwUnitGrp)!=0)
    {
    LogError("ScdCOM", 0, "Class Type NOT allowed: CreateUnit '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }

  LPCTSTR SubClass=NULL; // tobe Set / passed into this subroutine
  int err = gs_pPrj->AddNodeModel((char*)Class, (char*)SubClass, (char*)Tag);
  if (err)
    {
    LogError("ScdCOM", 0, "Error %i : CreateUnit '%s' of type '%s' failed!", err, Tag, Class);
    return S_OK;
    }
  //gs_LicenseCnt.CalcUnits();

  if (DataErr)
    LogError("ScdCOM", 0, "CreateUnit '%s' of type '%s' failed!", Tag, Class);
  return S_OK;
  }

HRESULT CScdCmdIF::CreateUnitGrf(/*[in]*/ LPCTSTR Tag, /*[in]*/ double X, /*[in]*/ double Y, /*[in]*/ double Z, /*[in]*/ LPCTSTR Symbol, /*[in]*/ double XScale, /*[in]*/ double YScale, /*[in]*/ double Rotation )
  {
  CScdCmdIF_FlagSetReset Flg;
  bool DataErr = (CheckTag(Tag)==0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag not found : CreateUnitGrf '%s' failed!", Tag);
    return S_OK;
    }
  CDocument* pDoc=NULL;
  if (!DataErr)
    {
    pDoc=GetGrfDoc(m_iGrfWndIndex);
    DataErr = (pDoc==NULL);
    }

  if (!DataErr)
    {
    RequestModelInfoRec MInfo;
    Strng ClassId;
    flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
    ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
    b = gs_pPrj->RequestModelInfoByClassId(ClassId(), SubClassId(), MInfo);
    ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);

    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    CInsertBlk CB; 

    CB.ATag=Tag;
    CB.AModel=ClassId();
    CB.Pt.World.X=X;
    CB.Pt.World.Y=Y;
    CB.Pt.World.Z=Z;
    CB.Scl.X=XScale;
    CB.Scl.Y=YScale;
    CB.Scl.Z=YScale;
    CB.Rotate=(float)Rotation;
    CB.ATagBase=MInfo.DefTag();
    if (Symbol && strchr(Symbol, '.'))
      CB.ASymbol=Symbol;
    else
      {
      CB.ASymbol=MInfo.DefTag();
      CB.ASymbol+='.';
      CB.ASymbol+=Symbol;
      }
    //CB.ASymbol+=".dxf";

    pGDoc->GCB.DoInsertNodeGrf(&CB);
    }

  if (DataErr)
    LogError("ScdCOM", 0, "CreateUnitGrf '%s' failed!", Tag);
  return S_OK;
  }

HRESULT CScdCmdIF::CreateLink(/*[in]*/ LPCTSTR Class, /*[in]*/ LPCTSTR Tag, /*[in]*/ LPCTSTR SrcTag, /*[in]*/ LPCTSTR SrcIO, /*[in]*/ LPCTSTR DstTag, /*[in]*/ LPCTSTR DstIO)
  { 
  CScdCmdIF_FlagSetReset Flg;
  bool DataErr = (CheckTag(Tag)!=0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag exists : CreateLink '%s' failed!", Tag);
    return S_OK;
    }
  DataErr = (CheckTag(SrcTag)==0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "SrcTag '%s' not found : CreateLink '%s' failed!", SrcTag, Tag);
    return S_OK;
    }
  DataErr = (CheckTag(DstTag)==0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "DstTag '%s' not found : CreateLink '%s' failed!", DstTag, Tag);
    return S_OK;
    }

  RequestModelInfoRec MInfo;
  DataErr = !gs_pPrj->RequestModelInfoByClassId((char*)Class, MInfo);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Unknown Class Type : CreateLink '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }
  DWORD LicCat = gs_License.LicCatagories();
  DataErr = !( ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0) && ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0) );
  if (DataErr || _stricmp(MInfo.Group(), FlwLinkGrp)!=0)
    {
    LogError("ScdCOM", 0, "Class Type NOT allowed: CreateLink '%s' of type '%s' failed!", Tag, Class);
    return S_OK;
    }

  int err = gs_pPrj->AddNodeConnect((char*)Class, (char*)Tag, (char*)SrcTag, (char*)SrcIO, (char*)DstTag, (char*)DstIO);
  if (err)
    {
    LogError("ScdCOM", 0, "Connection not Made:\n%s:%s\n%s.%s->%s.%s", Class, Tag, SrcTag, SrcIO, DstTag, DstIO);
    return S_OK;
    }
  //gs_LicenseCnt.CalcUnits();

  if (DataErr)
    LogError("ScdCOM", 0, "CreateLink '%s' failed!", Tag);
  return S_OK;
  }
HRESULT CScdCmdIF::CreateLinkGrf(/*[in]*/ LPCTSTR Tag, /*[in]*/ long DrawLineMethod)
  {
  CScdCmdIF_FlagSetReset Flg;
  bool DataErr = (CheckTag(Tag)==0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag not found : CreateLinkGrf '%s' failed!", Tag);
    return S_OK;
    }
  CDocument* pDoc=NULL;
  if (!DataErr)
    {
    pDoc=GetGrfDoc(m_iGrfWndIndex);
    DataErr = (pDoc==NULL);
    }

  if (!DataErr)
    {
    RequestModelInfoRec MInfo;
    Strng ClassId;
    flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
    ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
    b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
    ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);
    RequestConnModelInfoRec MCInfo;
    b = gs_pPrj->RequestModelConnInfo((char*)Tag, MCInfo);
    ASSERT_RDB(b, "ModelConnInfo for Class should be found", __FILE__, __LINE__);

    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    CConnectBlk CB; 

    CB.ATag=Tag;
    CB.AModel=ClassId();
    CB.SrcTag=MCInfo.SrcTag();
    //CB.SrcIO=SrcIO;
    CB.DstTag=MCInfo.DstTag();
    //CB.DstIO=DstIO;
    CB.iLineMethod=DrawLineMethod;
    CB.NVerts=-1;

    CB.ATagBase=MInfo.DefTag();
    CB.ASymbol=MInfo.DefTag();
    CB.ASymbol+=".default";
    
    pGDoc->GCB.DoInsertLinkGrf(&CB);
    }

  if (DataErr)
    LogError("ScdCOM", 0, "CreateLinkGrf '%s' failed!", Tag);
  return S_OK;
  }
HRESULT CScdCmdIF::CreateLinkGrfLine(/*[in]*/ LPCTSTR Tag, /*[in]*/ double Xs, /*[in]*/ double Ys, /*[in]*/ double Zs, /*[in]*/ double Xd, /*[in]*/ double Yd, /*[in]*/ double Zd)
  {
  CScdCmdIF_FlagSetReset Flg;
  bool DataErr = (CheckTag(Tag)==0);
  if (DataErr)
    {
    LogError("ScdCOM", 0, "Tag not found : CreateLinkGrfLine '%s' failed!", Tag);
    return S_OK;
    }
  CDocument* pDoc=NULL;
  if (!DataErr)
    {
    pDoc=GetGrfDoc(m_iGrfWndIndex);
    DataErr = (pDoc==NULL);
    }

  if (!DataErr)
    {
    RequestModelInfoRec MInfo;
    Strng ClassId, SubClassId;
    flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
    ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
    b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
    ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);
    RequestConnModelInfoRec MCInfo;
    b = gs_pPrj->RequestModelConnInfo((char*)Tag, MCInfo);
    ASSERT_RDB(b, "ModelConnInfo for Class should be found", __FILE__, __LINE__);

    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    CConnectBlk CB; 

    CB.ATag=Tag;
    CB.AModel=ClassId();
    CB.SrcTag=MCInfo.SrcTag();
    //CB.SrcIO=SrcIO;
    CB.DstTag=MCInfo.DstTag();
    //CB.DstIO=DstIO;
    CB.iLineMethod=0;
    CB.NVerts=2;
    CB.Verts[0].World.X=Xs;
    CB.Verts[0].World.Y=Ys;
    CB.Verts[0].World.Z=Zs;
    CB.Verts[1].World.X=Xd;
    CB.Verts[1].World.Y=Yd;
    CB.Verts[1].World.Z=Zd;

    CB.ATagBase=MInfo.DefTag();
    CB.ASymbol=MInfo.DefTag();
    CB.ASymbol+=".default";
    
    pGDoc->GCB.DoInsertLinkGrf(&CB);
    }

  if (DataErr)
    LogError("ScdCOM", 0, "CreateLinkGrfLine '%s' failed!", Tag);
  return S_OK;
  }

HRESULT CScdCmdIF::get_GrfWndIndex(/*[out, retval]*/ long *pVal) 
  { 
  * pVal=m_iGrfWndIndex;  
  return S_OK;
  }
HRESULT CScdCmdIF::put_GrfWndIndex(/*[in]*/ long newVal)
  { 
  m_iGrfWndIndex=newVal;
  return S_OK;
  }
HRESULT CScdCmdIF::ZoomWindowFull()
  { 
  CDocument* pDoc=NULL;
  pDoc=GetGrfDoc(m_iGrfWndIndex);
  if (pDoc)
    {
    CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
    pGDoc->GCB.pDsp->Open();
    pGDoc->GCB.pDsp->SetZoom(Zoom_All);
    pGDoc->GCB.pDrw->EntityInvalidate();
    pGDoc->GCB.pWnd->Invalidate(True);
    pGDoc->GCB.pDsp->Close();
    }
  return S_OK;
  }

//---------------------------------------------------------------------------

#if WITHCOMCMD
CScdCmdIF     gs_ScdCmdIF;
#endif


#endif