//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "syscad.h"
#include "tagvdoc.h"
#include "tagvtrnd.h"
#include "tagvtext.h"
#include "tagvdlgs.h"
#include "project.h"
#include "qrydlg.h"
#include "errorlog.h"
#include "licbase.h"
#include "scd_wm.h"
#include "dbgmngr.h"
#include "msgwnd.h"
#include "mdlrunmngr.h"
//#include "optoff.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define    dbgTrends       01

#if dbgTrends 
static CDbgMngr dbgHistoryData  ("Trends", "HistoryData");
static CDbgMngr dbgReBuild      ("Trends", "ReBuild");
static CDbgMngr dbgReBuildPts   ("Trends", "ReBuildPts");
static CDbgMngr dbgQueryHistory ("Trends", "QueryHistory");

inline double dbgTimeRound(double Tim) {return Tim-3600.0*floor(Tim/3600.0);};
inline double dbgTimeRound(CTimeValue Tim) { return Tim.Seconds; };//-3600.0*floor(Tim/3600.0);};
#endif

#define ALLOWFUNCS 0 /* CNM - FOR NOW DISBALE FUNCTIONS in TrendWindows*/

#if (!ALLOWFUNCS)
  //TODO Function evaluation disabled in trends!
#endif

//===========================================================================

IMPLEMENT_SPARES(CTrendPtBlk, 2000)

IMPLEMENT_MEMSTATS(CTagVwCmdBlk)
IMPLEMENT_MEMSTATS(CTagVwSlot)
IMPLEMENT_MEMSTATS(TrndTimeBase)
IMPLEMENT_MEMSTATS(CTrendPtBlkList)
IMPLEMENT_MEMSTATS(CTrendSlot)
IMPLEMENT_MEMSTATS(TrackTrend)

//===========================================================================

CTagVwCmdBlk::CTagVwCmdBlk(int ExtraCmds):
    DocCmdBlk(10+ExtraCmds,20)
  {
  pTrnd = NULL;
  pText = NULL;
  }

//---------------------------------------------------------------------------

CTagVwCmdBlk::~CTagVwCmdBlk()
  {
  }

//---------------------------------------------------------------------------

void CTagVwCmdBlk::InitForCmd()
  {
  }

//---------------------------------------------------------------------------

void CTagVwCmdBlk::OnActivateDoc(flag bActivate)
  {
  DocCmdBlk::OnActivateDoc(bActivate);
  }

//---------------------------------------------------------------------------

void CTagVwCmdBlk::BuildVNT()
  {
  SETVNT("SCRoll",      "Timebase",            "Start FBack BAck FOrward FForward End In Out Dialog Global Local",   1, (CmdFn)&CTagVwCmdBlk::DoScroll,        EF_ERS);

  SETCIS(ID_TREND_START,               "Scroll Timebase Start\r");
  SETCIS(ID_TREND_SCROLL_BACKWARD,     "Scroll Timebase Back\r");
  SETCIS(ID_TREND_SCROLL_BACKWARDFAST, "Scroll Timebase FBack\r");
  SETCIS(ID_TREND_SCROLL_FORWARD,      "Scroll Timebase FOrward\r");
  SETCIS(ID_TREND_SCROLL_FORWARDFAST,  "Scroll Timebase FForward\r");
  SETCIS(ID_TREND_END,                 "Scroll Timebase End\r");
  SETCIS(ID_TREND_ZOOM_IN,             "Scroll Timebase In\r");
  SETCIS(ID_TREND_ZOOM_OUT,            "Scroll Timebase Out\r");
  SETCIS(ID_TREND_SET,                 "Scroll Timebase Dialog\r");
  SETCIS(ID_TREND_GLOBAL,              "Scroll Timebase ");

  DocCmdBlk::BuildVNT();
  }

//---------------------------------------------------------------------------

flag CTagVwCmdBlk::UpdateCmdStr(int nID, CCmdUI* pCmdUI)
  {
  //TRACE("UpdateCmdStr %i",nID);
  switch (nID)
    {
    case ID_TREND_GLOBAL: pCmdUI->SetCheck(CTagVwDoc::TimeBaseGlobal()); return True;
    }
  return False;
  }

//---------------------------------------------------------------------------

flag CTagVwCmdBlk::ProcessCmdStr(int nID, rStrng Str)
  {
  switch (nID)
    {
    case ID_TREND_GLOBAL: 
      Str += CTagVwDoc::TimeBaseGlobal() ? "Local\r" : "Global\r"; break;
    }
  return True;
  }

//---------------------------------------------------------------------------
    
void CTagVwCmdBlk::DoScroll()
  {
  switch (When)
    {
    case EX_MODIFIER_RDY:
      switch (Modifier)
        {
        case 1 : ((CTagVwDoc*)pDoc)->AdjustTimebase(-1.0e38, 1.0, 0, 0); break; // STart
        case 2 : ((CTagVwDoc*)pDoc)->AdjustTimebase(-0.9,    1.0, 0, 0); break; // FBack
        case 3 : ((CTagVwDoc*)pDoc)->AdjustTimebase(-0.2,    1.0, 0, 0); break; // Back
        case 4 : ((CTagVwDoc*)pDoc)->AdjustTimebase( 0.2,    1.0, 0, 1); break; // Forward
        case 5 : ((CTagVwDoc*)pDoc)->AdjustTimebase( 0.9,    1.0, 0, 1); break; // FForward
        case 6 : ((CTagVwDoc*)pDoc)->AdjustTimebase( 1.0e38, 1.0, 1, 1); break; // End
        case 7 : ((CTagVwDoc*)pDoc)->AdjustTimebase( 0.0,    0.5, 0, 0); break; // In
        case 8 : ((CTagVwDoc*)pDoc)->AdjustTimebase( 0.0,    2.0, 0, 1); break; // Out
        case 9 : { CTBaseSetDlg Dlg((CTagVwDoc*)pDoc, pTrnd); Dlg.DoModal(); } ; break;
        case 10: ((CTagVwDoc*)pDoc)->SetTimeGlobal(True); break;
        case 11: ((CTagVwDoc*)pDoc)->SetTimeGlobal(False); break;
        default:;
        }
      break;
    case EX_RESULTS:
      DumpErrors();
      break;
    default:;
    }
  }

//---------------------------------------------------------------------------

void CTagVwCmdBlk::DumpErrors()
  {
  }

//===========================================================================

void CTagVwCmdBlk::AttachDoc(pDocRoot pDoc_, CView* pTrnd_, CView* pText_)
  {
  DocCmdBlk::AttachDoc(pDoc_);
  pTrnd=pTrnd_;
  pText=pText_;
  }

//---------------------------------------------------------------------------

BOOL CTagVwCmdBlk::ReadDocument(const char *pszPathName, FILE* pFile)
  {
  return True;
  }

//---------------------------------------------------------------------------

BOOL CTagVwCmdBlk::WriteDocument(const char *pszPathName, FILE* pFile)
  {
  return True;
  }

//===========================================================================

CTagVwSlot::CTagVwSlot()
  {                                      
  Clear(-1);
  }

//---------------------------------------------------------------------------

CTagVwSlot::~CTagVwSlot()
  {             
  }

//---------------------------------------------------------------------------

void CTagVwSlot::Clear(int iSlot)
  {
  cType=0;
  sValue="";
  sMinVal="";
  sMaxVal="";
  dValue=dNAN;
  dValueLowest=dNAN;
  dValueHighest=dNAN;
  nTrendNo=-1;
  dwWidth=1;// + i/ColourArrayLen;
  lPen.lopnStyle=BS_SOLID;
  if (iSlot<0)
    lPen.lopnColor=RGB(0xff,0xff,0xff);
  else // by default only use first 8 colours (the bright ones)
    lPen.lopnColor=CTagVwDoc::PenColours[iSlot % Min(8, ColourArrayLen)];
  //lPen.lopnWidth=1;
  bTagValid=0;
  bValueValid=0;
  bFunctValid=0;
  bHasFunct=0;
  bChgd=0;
  m_bEditableRunning=1;
  m_bEditableStopped=1;
  bIsResult=1;
  bDriverTag=0;
  bArchiveTag=0;
  bCtrlSlot=0;
  bHex=0;
  bRecorded=0;
  pFunctVar=NULL;
  //Cnv.Index()=0;
  //Cnv.Text()="";
  iNextChgdFld=-1;
  Fmt=DefinedFmts[3];
  bSlotType=STT_Empty;
  bScaleApplied=TAS_FitZ10;
  //bAutoScaleOn=(PrjFileVerNo()>=57 ? true : false);
  bAutoScaleOn=false;// CNM (PrjFileVerNo()>=57 ? true : false);
  m_XRefStatus=FFM_Off;
  }

//---------------------------------------------------------------------------

void CTagVwSlot::ClearIt(pCTagVwDoc pDoc)
  {
  SetTag("");
  ConnectTag(pDoc, CT_GetRecorded);
  SetTrending(-1, pDoc);
  }

//---------------------------------------------------------------------------

/*CTagVwSlot& CTagVwSlot::operator=(const CTagVwSlot& Slt)
  {
  sTag = Slt.sTag;
  sValue = Slt.sValue;
  sMinVal = Slt.sMinVal;
  sMaxVal = Slt.sMaxVal;
  sDesc = Slt.sDesc;
  nTrendNo = Slt.nTrendNo;
  Brush = Slt.Brush;
  dwWidth = Slt.dwWidth;
  bTagValid = Slt.bTagValid;
  bValueValid = Slt.bValueValid;
  bChgd = Slt.bChgd;
  bHex = Slt.bHex;
  bRecorded = Slt.bRecorded;
  iNextChgdFld = Slt.iNextChgdFld;
  cType = Slt.cType;
  Cnv.Index() = Slt.Cnv.Index();
  Cnv.Text() = Slt.Cnv.Text();
  Fmt = Slt.Fmt;
  }*/

//---------------------------------------------------------------------------

byte CTagVwSlot::TestSlotType()
  {
  bTagValid=0;
  bFunctValid=0;
  bHasFunct=0;
  bRecorded=0;
  bDriverTag=0;
  bArchiveTag=0;
  sTagOnly="";
  sCnvTxt="";
  if (sTag.Length()>0)
    {
    if (sTag[0] == '=')
      bSlotType=STT_Funct;
    else if (sTag[0] == '\'')
      bSlotType=STT_Comment;
    else
      {
      TaggedObject::SplitTagCnv(sTag(), sTagOnly, sCnvTxt);
      bSlotType=STT_DataPending;
      }
    }
  else
    bSlotType=STT_Empty;
  return bSlotType;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::SetSlotAsFunct(pCTagVwDoc pDoc)
  {
  pFunctVar=NULL;
  cType=tt_Double;
  m_bEditableRunning=0;
  m_bEditableStopped=0;
  bIsResult=0;
  bFunctValid=0;
  bHasFunct=1;
  bChgd=1;
  bValueValid=0;
  pDoc->bFunctBuildRqd=1;
  sValue="?";
  dValue=dNAN;
  dValueLowest=dNAN;
  dValueHighest=dNAN;

  //sMinVal="";
  //sMaxVal="";
  //sDesc="";
  sStrList.Clear();
  /*for (int i=0; i<NSlots; i++)
    if (&(pDoc->DS[i])==this)
      {
      char FunctName[16];
      sprintf(FunctName, "F%d", i);
      pDoc->AddFunct(FunctName, this);
      break;
      }*/
  return true;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::SetSlotAsData(pCTagVwDoc pDoc, CPkDataItem & Item)
  {
  bSlotType=STT_Data;
  cType=Item.Type();
  Cnv.SetIndex(Item.CnvIndex());
  m_bEditableStopped=((Item.Flags() & isParm)==isParm);
  m_bEditableRunning=((Item.Flags() & isParmStopped)==isParm);
  bIsResult = ((Item.Flags() & isResult)!=0);
  bDriverTag=((Item.Flags() & isDriverTag)!=0);
  bArchiveTag=((Item.Flags() & isArchiveTag)!=0);
//bDriverTag=1;//kga!
  if ((sCnvTxt.Len()>0) && (Cnvs[Cnv.Index()]->Find(sCnvTxt())!=NULL))
    Cnv.SetText(sCnvTxt());
  else
    Cnv.SetText(Item.CnvTxt());
  sCnvTxt=Cnv.Text();
  if (sCnvTxt.Len()>0)// && Cnv.Index()!=DC_TimeStr)
    sTag.Set("%s (%s)", sTagOnly(), sCnvTxt());
  else
    sTag.Set("%s", sTagOnly());
  TaggedObject::SplitTagCnv(sTag(), sTagOnly, sCnvTxt);
  if (bDriverTag)
    sDesc=Item.Description();
  sStrList.Clear();
  if (Item.Contains(PDI_StrList))
    Item.GetStrList(sStrList);

  if (IsNumData(cType))
    {
    bChgd=1;
    if (IsFloatData(cType))
      {
      dValue=Item.Value()->GetDouble(Cnv.Index(), Cnv.Text());
      Fmt.FormatFloat(dValue, sValue, (sStrList.Length()>0?&sStrList:NULL));
      //Fmt.FormatFloat(Item.Value()->GetDouble(Cnv.Index(), Cnv.Text()), sValue);
      bValueValid=Valid(dValue);
      }
    else if (sStrList.Length()>0)
      {
      long i=Item.Value()->GetLong(Cnv.Index(), Cnv.Text());
      //dValue=i;
      pStrng p=sStrList.AtIndexVal(i);
      if (p)
        sValue.Set("%i:%s", i, p->Str());
      else
        sValue.Set("%i", i);
      bValueValid=1;
      }
    else
      {
      sValue = Item.Value()->GetString("%i", "%g", Cnv.Index(), Cnv.Text());
      bValueValid=1;
      }

    pPkDataUnion pDMin, pDMax;
    sMinVal.Trim();
    sMaxVal.Trim();
    if (!IsNumData(cType) && sStrList.Length()>0)
      {
      sMinVal.Set("%i:%s", sStrList.First()->Index(), sStrList.First()->Str());
      sMaxVal.Set("%i:%s", sStrList.Last()->Index(), sStrList.Last()->Str());
      }
    else if (Item.GetRange(pDMin, pDMax))
      {
      if (sMinVal.Length()<=0)
        sMinVal=pDMin->GetString("%i", "%g", Cnv.Index(), Cnv.Text());
      if (sMaxVal.Length()<=0) 
        sMaxVal=pDMax->GetString("%i", "%g", Cnv.Index(), Cnv.Text());
      }
    else if (IsFloatData(cType) && ((Cnv.Index())>0))
      {
      pCDataCnv pCnv = gs_CnvsMngr.FindSecCnv(Cnv.Index(), Cnv.Text());
      if (pCnv)
        {
        if (sMinVal.Length()<=0) 
          sMinVal.Set("%10.2f", pCnv->MinValue());
        if (sMaxVal.Length()<=0) 
          sMaxVal.Set("%10.2f", pCnv->MaxValue());
        }
      }
    else
      {
      //if (sMin
      //sMinVal=".";
      //sMaxVal=".";
      }
    //pDoc->XBuildMyDataLists();
    bTagValid=1;
    }
  else if (IsData(cType))
    {
    bValueValid=1;
    bChgd=1;
    sValue = Item.Value()->GetString("%i", "%g", Cnv.Index(), Cnv.Text());
    //pDoc->XBuildMyDataLists();
    bTagValid=1;
    }
  else
    {
    Strng DocTitle((const char*)(pDoc->GetTitle()));
    LogError(DocTitle(), 0, "Tag '%s' not allowed.", sTagOnly());
    }

//  if (1 && bTagValid)
//    {
//    GetRecordedFlag();
//    }

  return bTagValid;
  }

//---------------------------------------------------------------------------

void CTagVwSlot::SetSlotAsEmpty(pCTagVwDoc pDoc)
  {
  bSlotType=STT_Empty;
  sValue="";
  for (int i=0; i<NoTrends(pDoc); i++)
    if (Trend(pDoc, i).pSlt == this)
      {
      Trend(pDoc, i).Connect(i, NULL, pDoc);
      break;
      }
  }

//---------------------------------------------------------------------------

//void CTagVwSlot::GetRecordedFlag(pCTagVwDoc pDoc)
//  {
//  CXM_Route HRoute;
//  if (pDoc->XFindObject(pExecName_Historian, HRoute))
//    {
//    CXM_HistoryExists xb(sTagOnly());
//    CXMsgLst XM;
//    XM.PackMsg(xb);
//    DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
//    bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
//    }
//  }

//---------------------------------------------------------------------------

flag CTagVwSlot::ConnectTag(pCTagVwDoc pDoc, byte Opts)
  {
  //SetSlotAsClear();
  switch (TestSlotType())
    {
    case STT_Funct:
      if (SetSlotAsFunct(pDoc))
        return true;
    case STT_DataPending:
      {
      //Strng WrkTag, WrkCnvTxt;
      //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
      CXM_Route Route;
      //CXMsgLst XM;
      BOOL OK = TRUE;
      if (Opts & CT_CheckFirst)
        {
        CXM_ObjectTag ObjTag(sTagOnly(), TABOpt_Exists);
        CXM_ObjectData ObjData;

        DWORD RetCode = pDoc->XReadTaggedItem(ObjTag, ObjData, Route);
        if (RetCode==0)
          {
          OK = FALSE;
          Strng DocTitle((const char*)(pDoc->GetTitle()));
          LogWarning(DocTitle(), 0, "Tag '%s' not found.", sTagOnly());
          }
        else if (((RetCode & RC_RTI_NumData)==0) && ((RetCode & RC_RTI_StrData)==0))
          {
          OK = FALSE;
          Strng DocTitle((const char*)(pDoc->GetTitle()));
          LogWarning(DocTitle(), 0, "Tag '%s' not allowed.", sTagOnly());
          }
        Route.Clear();
        //XM.Clear();
        }

      if (OK)
        {
        CXM_ObjectTag ObjTag(sTagOnly(), TABOpt_AllInfoOnce);
        CXM_ObjectData ObjData;

        if (pDoc->XReadTaggedItem(ObjTag, ObjData, Route))
          {
          SetSlotAsData(pDoc, *ObjData.FirstItem());
        
          if (bTagValid)
            {
            if (Opts & CT_GetRecorded)
              {
              CXM_Route HRoute;
              if (pDoc->XFindObject(pExecName_Historian, HRoute))
                {
                CXM_HistoryExists *xb=new CXM_HistoryExists (0, sTagOnly());
                CXMsgLst XM;
                XM.PackMsg(xb);
                DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
                bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
                }
              }
            pDoc->XBuildMyDataLists();
            return 1;
            }
          }
        else
          {
          Strng DocTitle((const char*)(pDoc->GetTitle()));
          LogWarning(DocTitle(), 0, "Tag '%s' not found.", sTagOnly());
          }
        }
      }
      break;
    }

  SetSlotAsEmpty(pDoc);
//cnmt5=SW.Secs(); SW.ReStart();
  pDoc->XBuildMyDataLists();
//cnmt6=SW.Secs(); SW.ReStart();

//cnmdbgpln("Con Tag %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %s",t0,t1,t2,t3,t4,t5,t6,sTag());    

  return 0;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::SetTrending(int TrendNo, pCTagVwDoc pDoc)
  {
  if (TrendNo<0)
    {
    nTrendNo=-1;
    return True;
    }
  bRecorded = 0;
  if (bHasFunct)
    {
    nTrendNo = TrendNo;
    return True;
    }

  CXM_Route HRoute;
  flag HistOn = (pDoc->XFindObject(pExecName_Historian, HRoute));
  
  //if (sMinVal.Length()<=0) 
  //  sMinVal="-";
  //if (sMaxVal.Length()<=0) 
  //  sMaxVal="-";

  //Strng WrkTag, WrkCnvTxt;
  //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
  if (HistOn)
    {
    CXM_HistoryExists *xb=new CXM_HistoryExists (0, sTagOnly());
    CXMsgLst XM;
    XM.PackMsg(xb);
    DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
    bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
    if (RetCode & RC_SM_HistExists)
      {//allready recording...
      nTrendNo=TrendNo;
      return True;
      }
    /*if ((RetCode & RC_SM_HistExists) && ((RetCode & RC_SM_HistRecOn)==0))
      {//switch recording on in historian...
      CXMsgLst XM;
      CXM_HistRecordingOn xb(WrkTag(), True);
      XM.PackMsg(xb);
      RetCode = pDoc->XSendMessage(XM, HRoute);
      bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
      nTrendNo=TrendNo;
      return True;
      }
    else if (RetCode & RC_SM_HistExists)
      {//allready recording...
      nTrendNo=TrendNo;
      return True;
      }*/
    }

  CXM_ObjectTag  ObjTag(sTagOnly(), TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route      Route;
  if (pDoc->XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem *pItem = ObjData.FirstItem();
    byte cType = pItem->Type();
    if (IsNumData(cType))
      {
      if (HistOn)
        {
        #if UseNewTrendDlg
        if (gs_pPrj->bShowNewTrndLineDlg)
          {
          CNewTrndSlotDlg Dlg;
          Dlg.bHistOn = HistOn;
          Dlg.m_Tag = sTagOnly();
          Dlg.m_Desc = pItem->Description();
          Dlg.m_Type = tt_TypeString(cType);
          Dlg.m_Eng = pItem->CnvTxt();
          Dlg.m_DispMin = atof(sMinVal() ? sMinVal() : "0");
          Dlg.m_DispMax = Max(Dlg.m_DispMin, atof(sMaxVal() ? sMaxVal() : "1"));
          Dlg.m_RecordIt = gs_pPrj->bDefaultRecordIt;
          Dlg.DoModal();
          if (Dlg.m_RecordIt)
            {
            CXMsgLst XM;
            DataUnion Val;
            Val.Set(*(pItem->Value()));
            CXM_KeepHistory xb(&Val, 0, cType, sTagOnly(), 
                              pItem->CnvIndex(),
                              Dlg.m_Eng.GetBuffer(0),
                              Dlg.m_Desc.GetBuffer(0), 
                              0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0,
                              0, 0, 0, 0, True, False);
            XM.PackMsg(xb);
            DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
            if (RetCode)
              bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
            }
          Dlg.m_DispMax = Max(Dlg.m_DispMin, Dlg.m_DispMax);
          sMinVal.Set("%g", Dlg.m_DispMin);
          sMaxVal.Set("%g", Dlg.m_DispMax);
          }
        else if (gs_pPrj->bDefaultRecordIt)
        #endif
        if (gs_pPrj->bDefaultRecordIt)
          {
          CXMsgLst XM;
          DataUnion Val;
          Val.Set(*(pItem->Value()));
          CXM_KeepHistory *xb=new CXM_KeepHistory (&Val, 0, cType, sTagOnly(), 
                                                  pItem->CnvIndex(),
                                                  pItem->CnvTxt(),
                                                  pItem->Description(), 
                                                  0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0,
                                                  0, 0, 0, 0, True, False);
          XM.PackMsg(xb);
          DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
          if (RetCode)
            bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
          }
        }
      nTrendNo = TrendNo;
      }
    else
      {
      //????? Strings Etc.
      }
    }
  else
    LogWarning("Trends", 0, "Tag %s not found", sTagOnly());

  return True;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::RecordTag(pCTagVwDoc pDoc, flag RecordingOn)
  {
  //Note:for driver tags, the recording option must be set in the driver slot configuration file
  if (sTag.Len()>0 && sTag[0]!='\'' && sTag[0]!='=' && !bDriverTag)
    {
    CXM_Route HRoute;
    flag HistOn = (pDoc->XFindObject(pExecName_Historian, HRoute));
    if (HistOn)
      {
      //Strng WrkTag, WrkCnvTxt;
      //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
      CXM_HistoryExists *xb=new CXM_HistoryExists (0, sTagOnly());
      CXMsgLst XM;
      XM.PackMsg(xb);
      DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
      bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
      if (RetCode & RC_SM_HistExists)
        {
        flag IsRecordingOn = ((RetCode & RC_SM_HistRecOn)!=0);
        if ((IsRecordingOn!=0)!=(RecordingOn!=0))
          {
          XM.Clear();
          CXM_HistRecordingOn *xb=new CXM_HistRecordingOn (sTagOnly(), RecordingOn);
          XM.PackMsg(xb);
          RetCode = pDoc->XSendMessage(XM, HRoute);
          bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
          return True; //change made
          }
        return False;
        }

      if (RecordingOn)
        {
        CXM_ObjectTag  ObjTag(sTagOnly(), TABOpt_AllInfoOnce);//TABOpt_Parms);
        CXM_ObjectData ObjData;
        CXM_Route      Route;
        if (pDoc->XReadTaggedItem(ObjTag, ObjData, Route))
          {
          CPkDataItem *pItem = ObjData.FirstItem();
          byte cType = pItem->Type();
          if (IsNumData(cType) || (AllowHistStr && IsStrng(cType)))
            {
            CXMsgLst XM;
            DataUnion Val;
            Val.Set(*(pItem->Value()));
            CXM_KeepHistory *xb=new CXM_KeepHistory (&Val, 0, cType, sTagOnly(), 
                                                    pItem->CnvIndex(),
                                                    pItem->CnvTxt(),
                                                    pItem->Description(), 
                                                    0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0,
                                                    0, 0, 0, 0, True, False);
            XM.PackMsg(xb);
            DWORD RetCode = pDoc->XSendMessage(XM, HRoute);
            if (RetCode)
              {
              bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
              return True; //change made
              }
            }
          }
        }
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::ShowDriverTagInfo(pCTagVwDoc pDoc, WORD DrvOptMask)
  {
  //0x0001  show in status bar
  //0x0002  show in message window (LogNote)
  //0x0004  show in driver slot dialog
  //0x0008  paste driver tag/address in clipboard
  ASSERT(bDriverTag);
  if (sTag.Len()>0 && sTag[0]!='\'' && sTag[0]!='=')
    {
    CXM_Route DRoute;
    const flag DrvOn = (pDoc->XFindObject(pExecName_DrvMngr, DRoute));
    if (DrvOn)
      {
      //Strng WrkTag, WrkCnvTxt;
      //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
      CXM_DrvShowTagInfo *xb=new CXM_DrvShowTagInfo (sTagOnly(), DrvOptMask);
      CXMsgLst XM;
      XM.PackMsg(xb);
      DWORD RetCode = pDoc->XSendMessage(XM, DRoute);
      if (RetCode)
        return True;
      }
    }

  return False;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::ShowArchiveTagInfo(pCTagVwDoc pDoc, WORD ArcOptMask)
  {
  //0x0001  show in status bar
  //0x0002  show in message window (LogNote)
  //0x0004  show in driver slot dialog
  ASSERT(bArchiveTag);
  if (sTag.Len()>0 && sTag[0]!='\'' && sTag[0]!='=')
    {
    LogNote("Trend", 0, "Archive Info Incomplete");
    CXM_Route DRoute;
    const flag ArcOn = (pDoc->XFindObject(pExecName_ArcMngr, DRoute));
    if (ArcOn)
      {
      //Strng WrkTag, WrkCnvTxt;
      //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
      CXM_ArcShowTagInfo *xb=new CXM_ArcShowTagInfo (sTagOnly(), ArcOptMask);
      CXMsgLst XM;
      XM.PackMsg(xb);
      DWORD RetCode = pDoc->XSendMessage(XM, DRoute);
      if (RetCode)
        return True;
      }
    }

  return False;
  }
//---------------------------------------------------------------------------

flag CTagVwSlot::ChangeTag(char* pNewTag, pCTagVwDoc pDoc, COLORREF DefColour)
  {
  Strng WrkTag, WrkCnvTxt, OldWrkTag, OldWrkCnvTxt;
  Strng NewTag(pNewTag);
  NewTag.Trim(" \t\n\r\v\f");
  if (NewTag())
    TaggedObject::SplitTagCnv(NewTag(), WrkTag, WrkCnvTxt);
  if (sTag())
    TaggedObject::SplitTagCnv(sTag(), OldWrkTag, OldWrkCnvTxt);
  if ( (WrkTag.Len()!=OldWrkTag.Len() || (WrkTag.IsNotEmpty() && strcmp(WrkTag(), OldWrkTag())!=0)) ||
       (bHasFunct && (sTag.Len()!=NewTag.Len() || (NewTag.IsNotEmpty() && strcmp(NewTag(), sTag())!=0))) )
    {//tag changed...
    Clear(-1);
    lPen.lopnColor = DefColour;
    sDesc="";
    SetTag(pNewTag);
    ConnectTag(pDoc, CT_CheckFirst|CT_GetRecorded);
    if (!bHasFunct)
      SetTrending(-1, pDoc);
    return True;
    }

  if (sTag.Length()>0 && sTag[0] != '\'' && sTag[0] != '=')
    {
    if (Cnv.Index()!=0 && WrkCnvTxt.Length()>0 && strcmp(WrkCnvTxt(), OldWrkCnvTxt())!=0)
      ChangeCnv(WrkCnvTxt());

    //get current value
    LoadData(pDoc);
    }
  return False;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::ChangeCnv(char* pNewCnv)
  {
  flag Chngd = 0;
  if (Cnv.Index()!=0 && pNewCnv && strcmp(pNewCnv, Cnv.Text())!=0)
    {
    if (Cnvs[Cnv.Index()]->Find(pNewCnv))
      {//conversion changed...
      Chngd = 1;
      if (IsNumData(cType))
        {
        Strng s = sValue;
        s.Trim();
        if (s.Length()>0 && _stricmp("*", s())!=0)
          {
          double d = atof(s());
          d = Cnvs[Cnv.Index()]->Normal(d, Cnv.Text());
          dValue = Cnvs[Cnv.Index()]->Human(d, pNewCnv);
          Fmt.FormatFloat(dValue, sValue, (sStrList.Length()>0?&sStrList:NULL));
          }
        s = sMinVal;
        s.Trim();
        if (s.Length()>0 && _stricmp("*", s())!=0)
          {
          double d = atof(s());
          d = Cnvs[Cnv.Index()]->Normal(d, Cnv.Text());
          d = Cnvs[Cnv.Index()]->Human(d, pNewCnv);
          sMinVal.Set("%g", d);
          }
        s = sMaxVal;
        s.Trim();
        if (s.Length()>0 && _stricmp("*", s())!=0)
          {
          double d = atof(s());
          d = Cnvs[Cnv.Index()]->Normal(d, Cnv.Text());
          d = Cnvs[Cnv.Index()]->Human(d, pNewCnv);
          sMaxVal.Set("%g", d);
          }
        }
      else if (IsStrng(cType) && IsTimeCnv(Cnv.Index()))
        {
        Strng s = sValue;
        s.Trim();
        CTimeValue X((LPCTSTR)s());
        sValue=X.Format(pNewCnv);
        }
      Cnv.SetText(pNewCnv);

//      Strng WrkTag, WrkCnvTxt;
//      TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
      sTag.Set("%s (%s)", sTagOnly(), Cnv.Text());
      }
    }
  return Chngd;
  }

//---------------------------------------------------------------------------

flag CTagVwSlot::SetOutput(pCTagVwDoc pDoc, pchar Str, flag PromptNeeded, flag PermissionNeeded)
  {
  flag Ok=True;
  if (PermissionNeeded)
    {
    }
  if (Ok && PromptNeeded)
    {
    //Strng WrkTag,WrkCnvTxt;
    //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
    CXM_ObjectTag  ObjTag(sTagOnly(), 0);//CNM TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    CXM_Route      Route;
    Ok = (pDoc->XReadTaggedItem(ObjTag, ObjData, Route)!=0);
    
    if (Ok)
      {
      CPkDataItem *pPItem=ObjData.FirstItem();
      if ((pPItem->Flags() & isParm)!=0)
        {
        PkDataUnion DU;
        DU.SetTypeString(pPItem->Type(), Str, Cnv.Index(), Cnv.Text());
        CXM_ObjectData ObjData(0, 0, sTagOnly(), 0, DU);
        dbgpln("Set %s : %s", sTagOnly(), Str);
        //Ok = (pDoc->XWriteTaggedItem(ObjData, Route)==TOData_OK);
        Ok = (pDoc->XWriteTaggedItem(ObjData, Route)!=TOData_NotFound);//???
        if (Ok)
          gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Trend, sTagOnly(), Str, Cnv.Text());
        }
      else
        Ok = False;
      }
    }
  return Ok;
  }

//---------------------------------------------------------------------------

void CTagVwSlot::LoadData(pCTagVwDoc pDoc)
  {
  //Strng WrkTag, WrkCnvTxt;
  //TaggedObject::SplitTagCnv(sTag(), WrkTag, WrkCnvTxt);
  
  CXMsgLst XM;
  CXM_ObjectTag ObjTag(sTagOnly(), 0);//CNM TABOpt_AllInfoOnce);
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (pDoc->XReadTaggedItem(ObjTag, ObjData, Route))
    {
    CPkDataItem *pItem = ObjData.FirstItem();
    if (pItem->Contains(PDI_Flags))
      {
      m_bEditableStopped=((pItem->Flags() & isParm)==isParm);
      m_bEditableRunning=((pItem->Flags() & isParmStopped)==isParm);
      bIsResult = ((pItem->Flags() & isResult)!=0);
      bDriverTag = ((pItem->Flags() & isDriverTag)!=0);
      bArchiveTag=((pItem->Flags() & isArchiveTag)!=0);
      }
    else
      {
      m_bEditableRunning = 0;
      m_bEditableStopped = 0;
      bIsResult = 0;
      bDriverTag = 0;
      bArchiveTag = 0;
      }
    if (pItem->Contains(PDI_Value))
      if (IsNumData(cType))
        {
        if (IsFloatData(cType))
          {
          dValue=pItem->Value()->GetDouble(Cnv.Index(), Cnv.Text());
          Fmt.FormatFloat(dValue, sValue, (sStrList.Length()>0?&sStrList:NULL));
          }
        else if (sStrList.Length()>0)
          {
          long i=pItem->Value()->GetLong(Cnv.Index(), Cnv.Text());
          pStrng p=sStrList.AtIndexVal(i);
          if (p)
            sValue.Set("%i:%s", i, p->Str());
          else
            sValue.Set("%i", i);
          }
        else
          sValue = pItem->Value()->GetString("%i", "%g", Cnv.Index(), Cnv.Text());
        }
      else if (IsData(cType))
        sValue = pItem->Value()->GetString("%i", "%g", Cnv.Index(), Cnv.Text());
    }
  }

//---------------------------------------------------------------------------

void CTagVwSlot::DoFit(byte How, double &Mn, double &Mx, bool KeepMn, bool KeepMx)
  {
  switch (How)
    {
    case TAS_FitZMin:
    case TAS_FitZ1:
    case TAS_FitZ10:
    case TAS_FitZ50:
      if (Mn>0)
        {
        KeepMn=true;
        Mn=0;
        }
      if (Mx<0)
        {
        KeepMx=true;
        Mx=0;
        }
      break;
    }

  //              Off   Min     1   10   50  ZMin    Z1  Z10  Z50  
  double Adj[] = {0.0,  0.0, 0.01, 0.1, 0.5,  0.0, 0.01, 0.1, 0.5};
  
  double Diff=(Mx-Mn)/(1-Adj[How]);
  
  if (KeepMn)
    {
    Mx=Mn+Diff;
    }
  else if (KeepMx)
    {
    Mn=Mx-Diff;
    }
  else
    {
    double Mid=0.5*(Mx+Mn);
    Mn=Mid-(0.5*Diff);
    Mx=Mid+(0.5*Diff);
    }
  }

//---------------------------------------------------------------------------

void CTagVwSlot::SetRange(double Mn, double Mx)
  {
  int i;
  Strng S;
  S.Set("%.8g",Mn);
  i=S.Length()-1;
  if (S.Find(".")>=0)
    {
    while (i>1 && S[i]=='0')
      S[i--]=' ';
    }
  S.LRTrim();
  sMinVal=S;
  S.Set("%.8g",Mx);
  i=S.Length()-1;
  if (S.Find(".")>=0)
    {
    while (i>1 && S[i]=='0')
      S[i--]=' ';
    }
  S.LRTrim();
  sMaxVal=S;
  };

//---------------------------------------------------------------------------

void CTagVwSlot::UpdateYMnMx(pCTagVwDoc pDoc)
  {
  if (nTrendNo>=0)
    {
    Trend(pDoc, nTrendNo).UpdateYMnMx(
      atof(sMinVal() ? sMinVal() : "0"), 
      atof(sMaxVal() ? sMaxVal() : "1"));
    }
  }

//===========================================================================
//
//
//
//===========================================================================

CTrendPtBlk::CTrendPtBlk()
  {
  iNPts=0;
  iNewPts=0;
  }

//---------------------------------------------------------------------------

CTrendPtBlk::~CTrendPtBlk()
  {
  }

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*#F:Adds a point to a trend.*/
flag CTrendPtBlk::AddPt(CTimeValue Time, double Val, byte Typ, int SltNo, int Fwd, CTrendSclBlk & Scl, CTagVwDoc *Doc)
  {
  CPoint pt(TimePixel(Time, Doc->TB), YPixel(Val, Scl));
  
  int iNPts_Entry=iNPts;
  if (iNPts==0)
    {
    Pt[iNPts++]=pt;
    #if dbgTrends 
    if (dbgReBuildPts())
      dbgpln("RB:AddPt             %4i x:%5i y:%5i ",iNPts, pt.x, pt.y);
    #endif
    }
  else
    {
    ASSERT(iNPts>=0 && iNPts<=MaxTrendPtBlk); 

    int ep=iNPts-1;
    long dx=(pt.x - Pt[ep].x);
    if (Fwd && (dx > 0) || !Fwd  && (dx < 0))
      {
      if (iNPts>=MaxTrendPtBlk) 
        return False;
      Pt[iNPts++]=pt;
      #if dbgTrends 
      if (dbgReBuildPts())
        dbgpln("RB:AddPt             %4i x:%5i y:%5i ",iNPts, pt.x, pt.y);
      #endif
      }
    else
      {
      if (iNPts<3 || Pt[ep-1].x != Pt[ep].x)
        {
        if (iNPts>=MaxTrendPtBlk-3) 
          return False;
        for (int i=0; i<3; i++)
          Pt[iNPts++] = Pt[ep++];
        }
      Pt[ep-2].y = Max(pt.y, Pt[ep-2].y);
      Pt[ep-1].y = Min(pt.y, Pt[ep-1].y);
      Pt[ep]=pt;
      ASSERT(iNPts>=0 && iNPts<=MaxTrendPtBlk);

      #if dbgTrends 
      if (dbgReBuildPts())
        dbgpln("RB:AddPt          <> %4i x:%5i y:%5i Ent:%5i Max:%5i Min:%5i Exit:%5i ",
              iNPts, pt.x, pt.y, Pt[ep-3].y, Pt[ep-2].y, Pt[ep-1].y, Pt[ep].y);
      #endif
      }
    }
  iNewPts+=(iNPts-iNPts_Entry);
  return True;
  }

//---------------------------------------------------------------------------

void CTrendPtBlk::ShiftPts(CTimeValue d, CTrendSclBlk & Scl, CTagVwDoc *Doc)
  {
  ASSERT(iNPts>=0 && iNPts<=MaxTrendPtBlk);
  double XSc=TrendXMax/(Doc->TB.EndTime-Doc->TB.StartTime).Seconds;
  long dx=(int)(XSc*d.Seconds);

  for (int i=0; i<iNPts; i++)
    Pt[i].x-=dx;

  }

//---------------------------------------------------------------------------

void CTrendPtBlk::UpdateYMnMx(CTrendSclBlk & OldScl, CTrendSclBlk & NewScl, CTagVwDoc *Doc)
  {
  ASSERT(iNPts>=0 && iNPts<=MaxTrendPtBlk);

  double YMult=double(OldScl.YMx-OldScl.YMn)/TrendYMax;
  for (int i=0; i<iNPts; i++)
    {
    double Val = OldScl.YMn+YMult*Pt[i].y;
    Pt[i].y=YPixel(Val, NewScl);
    }
  }

//---------------------------------------------------------------------------

flag CTrendPtBlk::AnyInRange()
  {
  ASSERT(iNPts>=0 && iNPts<=MaxTrendPtBlk);
  if (iNPts==0)
    return False;
  if (Pt[0].x>=0 && Pt[0].x<TrendXMax)
    return True;
  if (Pt[iNPts-1].x>=0 && Pt[iNPts-1].x<TrendXMax)
    return True;
  return False;
  }

//===========================================================================

CTrendSlot::CTrendSlot()
  {
  iNo=-1;
  pSlt=NULL;
  pDoc=NULL;
  LastMeasTime=dNAN;
  LastMeasValue=0;
  dBuildStartTime=dNAN;
  }
  
//---------------------------------------------------------------------------  

CTrendSlot::~CTrendSlot()
  {
  while (!PtBlks.IsEmpty())
    {
    CTrendPtBlk* p=PtBlks.RemoveHead();
    delete p;
    }
  //while (!FreeBlks.IsEmpty())
  //  {
  //  CTrendPtBlk* p=FreeBlks.RemoveHead();
  //  delete p;
  //  }
  }

//---------------------------------------------------------------------------

CTrendPtBlk* CTrendSlot::GetPtBlk()
  {
  CTrendPtBlk* p=new CTrendPtBlk;
  //if (FreeBlks.IsEmpty()) 
  //  {
  //  p=new CTrendPtBlk;
  //  }
  //else 
  //  p=FreeBlks.RemoveHead();
  p->iNPts=0;
  p->iNewPts=0;
  return p;
  }

//---------------------------------------------------------------------------

void CTrendSlot::Clear()
  {
  while (!PtBlks.IsEmpty())
    delete PtBlks.RemoveHead();
    //{
    //CTrendPtBlk* p=PtBlks.RemoveHead();
    //FreeBlks.AddHead(p);
    //}
  pStartOfTail=NULL;
  LastMeasTime=dNAN;
  LastMeasValue=0;
  }

//---------------------------------------------------------------------------

void CTrendSlot::ClearTail()
  {
  #if dbgTrends 
  if (dbgReBuild()) dbgp("Slot:ClearTail() [%i]", iNo);
  #endif
  if (pStartOfTail!=NULL)
    {
    while (!PtBlks.IsEmpty())
      {
      CTrendPtBlk* p=PtBlks.RemoveTail();
      delete p;//FreeBlks.AddHead(p);
      if (p==pStartOfTail)
        break;
      }
    pStartOfTail=NULL;
  
    if (!PtBlks.IsEmpty())
      {
      CTrendPtBlk* pt=PtBlks.GetTail();
      CTrendPtBlk* p=GetPtBlk();
      if (pt->NPts()>0)
        {
        p->Pt[0]=pt->Pt[0];
        p->iNPts=1;
        p->iNewPts=1;
        }
      else
        {
        #if dbgTrends 
        if (dbgReBuild()) dbgp("No Data");
        #endif
        }

      PtBlks.AddTail(p);
      pStartOfTail=p;
      }
    }
  #if dbgTrends 
  if (dbgReBuild()) dbgpln("");
  #endif
  }

//---------------------------------------------------------------------------

void CTrendSlot::AddStartPt(CTimeValue Tim, double Val)
  {
  #if dbgTrends 
  if (dbgReBuild()) dbgpln("Slot::AddStartPoint() [%i]",iNo);
  #endif
  CTrendPtBlk* p=GetPtBlk();
  if (Valid(Val))
    p->AddPt(Tim, Val, 0, iNo, True, Scl, pDoc);
  PtBlks.AddHead(p);
  p=GetPtBlk();
  if (Valid(Val))
    p->AddPt(Tim, Val, 0, iNo, False, Scl, pDoc);
  PtBlks.AddTail(p);
  pStartOfTail=p;
  }

//---------------------------------------------------------------------------

void CTrendSlot::Start() 
  { 
  if (!PtBlks.IsEmpty())
    {
    CTrendPtBlk* pn=GetPtBlk();
    PtBlks.AddHead(pn); // Force A Break;
    }
  }

//---------------------------------------------------------------------------

void CTrendSlot::Execute() 
  { 
  }

//---------------------------------------------------------------------------

void CTrendSlot::Stop() 
  { 
  }

//---------------------------------------------------------------------------

void CTrendSlot::Connect(int No, CTagVwSlot* Slt_, pCTagVwDoc Doc_)
  {
  iNo=No;
  pSlt=Slt_;
  pDoc=Doc_;
  Clear();
  }

//---------------------------------------------------------------------------  

void CTrendSlot::AppendMeasured(CTimeValue TheTime)
  {
  #if XXdbgReBuild
  dbgpln("RB:Trend Build From Measured");
  dbgpln("RB:Start %li",BldPts.StartTime-Hst().TimeRunStarted);
  dbgpln("RB:End   %li",BldPts.EndTime-Hst().TimeRunStarted);
  dbgpln("RB:Time:%li  %g",gs_Exec.TheTime, HS->fMeas());
  #endif

  if (pSlt)
    {
    LastMeasTime=TheTime;
    if (pSlt->sValue() && pSlt->ValueValid() && Valid(pSlt->dValue))
      {
      //LastMeasValue = atof(pSlt->sValue());
      LastMeasValue = pSlt->dValue;
      if (fabs(pSlt->dValue)<1.0e30)
        if (Valid(pSlt->dValueLowest))
          {
          pSlt->dValueLowest=Min(pSlt->dValueLowest, pSlt->dValue);
          pSlt->dValueHighest=Max(pSlt->dValueHighest, pSlt->dValue);
          }
        else
          {
          pSlt->dValueLowest=pSlt->dValue;
          pSlt->dValueHighest=pSlt->dValue;
          }

      #if dbgTrends 
      if (dbgReBuild()) dbgpln("Slot::AppendMeasured() [%i] %8.1f = %9.2f %s", iNo, dbgTimeRound(LastMeasTime), LastMeasValue, pSlt->sTag());
      #endif
      if (PtBlks.IsEmpty())
        AddStartPt(LastMeasTime, LastMeasValue);
      else
        {
        CTrendPtBlk* p=PtBlks.GetHead();
        if (!p->AddPt(LastMeasTime, LastMeasValue, 0, iNo, True, Scl, pDoc))
          {
          CTrendPtBlk* pn=GetPtBlk();
          PtBlks.AddHead(pn);
          pn->Pt[0]=p->Pt[p->NPts()-1];
          pn->iNPts++;
          pn->iNewPts++;
          if (!pn->AddPt(LastMeasTime, LastMeasValue, 0, iNo, True, Scl, pDoc))
            VERIFY(0); // A Problem should never happen
          }
        }
      }
    else  
      {
      LastMeasValue=dNAN;
      LastMeasTime=0.0;
      #if dbgTrends 
      if (dbgReBuild()) dbgpln("Slot::AppendMeasured() [%i] %8.1f =   BREAK   %s",iNo,dbgTimeRound(LastMeasTime),pSlt->sTag());
      #endif
      if (PtBlks.IsEmpty())
        AddStartPt(0.0, dNAN);
      else
        {
        CTrendPtBlk* p=PtBlks.GetHead();
        if (p->NPts()>0)
          PtBlks.AddHead(GetPtBlk());
        }
      }
    }
  }

//---------------------------------------------------------------------------
/*#F:The document is adding a point to the trend.*/

void CTrendSlot::BuildFromEvent(CTimeValue Time, double Val)
  {
  #if dbgTrends 
  if (dbgReBuild()) dbgpln("Slot::BuildFromEvent() [%i] %8.1f = %9.2f %s",iNo,dbgTimeRound(Time),Val,pSlt->sTag());
  #endif
  if (ValidTime(dBuildStartTime) && (Time > dBuildStartTime+0.005))
    {
    #if dbgTrends 
    if (dbgReBuild()) dbgpln("  -DISCARD");
    #endif
    return;
    }

  if (pSlt)
    if (Valid(Val))
      {
      if (pSlt->Cnv.Index() >= 0)
        Val=Cnvs[pSlt->Cnv.Index()]->Human(Val, pSlt->Cnv.Text());

      if (fabs(Val)<1.0e30)
        if (Valid(pSlt->dValueLowest))
          {
          pSlt->dValueLowest=Min(pSlt->dValueLowest, Val);
          pSlt->dValueHighest=Max(pSlt->dValueHighest, Val);
          }
        else
          {
          pSlt->dValueLowest=Val;
          pSlt->dValueHighest=Val;
          }

      if (PtBlks.IsEmpty())
        AddStartPt(Time, Val);
      else
        {
        CTrendPtBlk* p=PtBlks.GetTail();
        if (!p->AddPt(Time, Val, 0, iNo, False, Scl, pDoc))
          {
          CTrendPtBlk* pn=GetPtBlk();
          PtBlks.AddTail(pn);
          pn->Pt[0]=p->Pt[p->NPts()-1];
          pn->iNPts++;
          pn->iNewPts++;
          if (!pn->AddPt(Time, Val, 0, iNo, False, Scl, pDoc))
            VERIFY(0); // A Problem
          }
        }
      }
    else  
      {
      if (PtBlks.IsEmpty())
        AddStartPt(0.0, dNAN);
      else
        {
        CTrendPtBlk* p=PtBlks.GetTail();
        if (p->NPts()>0)
          PtBlks.AddTail(GetPtBlk());
        }
      }
  }

//---------------------------------------------------------------------------

void CTrendSlot::ShiftDrwPts(CTimeValue d)
  {
  POSITION Pos=PtBlks.GetHeadPosition();
  while (Pos)
    {
    CTrendPtBlk* p=PtBlks.GetNext(Pos);
    p->ShiftPts(d, Scl, pDoc);
    }

  if (1)
    {
    while (!PtBlks.IsEmpty() && (PtBlks.GetCount()>2))
      {
      if (PtBlks.GetHead()->AnyInRange())
        break;
      delete PtBlks.RemoveHead();
      }

    while (!PtBlks.IsEmpty() && (PtBlks.GetCount()>2))
      {
      if (PtBlks.GetTail()->AnyInRange())
        break;
      delete PtBlks.RemoveTail();
      }
    }
  }

//---------------------------------------------------------------------------

void CTrendSlot::UpdateYMnMx(double NewMin, double NewMax)
  {
  CTrendSclBlk OldScl=Scl;
  Scl.YMn=NewMin;
  Scl.YMx=NewMax;

  POSITION Pos=PtBlks.GetHeadPosition();
  while (Pos)
    {
    CTrendPtBlk* p=PtBlks.GetNext(Pos);
    p->UpdateYMnMx(OldScl, Scl, pDoc);
    }
  }

//===========================================================================

TrndTimeBase CTagVwDoc::GTB;
BOOL CTagVwDoc::fTimeBaseGlobal = 1;

ColourArray CTagVwDoc::PenColours = { RGB(0xff, 0xff, 0xff),
                                      RGB(0x00, 0x00, 0xff),
                                      RGB(0x00, 0xff, 0x00),
                                      RGB(0xff, 0x00, 0x00),
                                      RGB(0x00, 0xff, 0xff),
                                      RGB(0xff, 0x00, 0xff),
                                      RGB(0xff, 0xff, 0x00),
                                      RGB(0x00, 0x00, 0x80),
                                      RGB(0x00, 0x80, 0x00),
                                      RGB(0x80, 0x00, 0x00),
                                      RGB(0x00, 0x80, 0x80),
                                      RGB(0x80, 0x00, 0x80),
                                      RGB(0x80, 0x80, 0x00),
                                      RGB(0x00, 0x80, 0xff),
                                      RGB(0x00, 0xff, 0x80),
                                      RGB(0xff, 0x80, 0x00) };

/*StyleArray CTagVwDoc::PenStyles = {   2, { 10000,   0,   0,   0,   0,   0},
                                      2, {    40,  40,   0,   0,   0,   0},
                                      2, {    70,  70,   0,   0,   0,   0},
                                      2, {    90,  90,   0,   0,   0,   0},
                                      4, {    70,  40,  40,  40,   0,   0},
                                      4, {    90,  40,  40,  40,   0,   0},
                                      4, {    90,  40,  70,  40,   0,   0},
                                      6, {    90,  40,  40,  40,  40,  40}};*/

//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CTagVwDoc, DocRoot);

BEGIN_MESSAGE_MAP(CTagVwDoc, DocRoot)
  //{{AFX_MSG_MAP(CTagVwDoc)
  ON_COMMAND(ID_TREND_GETQUERY, OnTrendGetquery)
  ON_COMMAND(ID_TREND_OPTIONS, OnTrendOptions)
  ON_COMMAND(ID_TREND_SLOTS, OnTrendSlots)
  ON_COMMAND(ID_TREND_REFRESH, OnTrendRefresh)
  ON_COMMAND(ID_TREND_REFRESHALL, OnTrendRefreshall)
  ON_COMMAND(ID_TREND_RECORD, OnTrendRecord)
  ON_COMMAND(ID_TREND_RECORDALL, OnTrendRecordall)
  ON_UPDATE_COMMAND_UI(ID_TREND_RECORD, OnUpdateTrendRecord)
  ON_UPDATE_COMMAND_UI(ID_TREND_RECORDALL, OnUpdateTrendRecordall)
  ON_COMMAND_RANGE(IDM_TRNDSCLALL_FITMIN, IDM_TRNDSCLALL_FITAUTOOFF, OnTrendFitAll)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CTagVwDoc::CTagVwDoc()://int NSlots_) :
  TCB(0)
  {
  DS.SetSize(0,16);
  DS.CheckSize(MinTrendSlots);
//  Trend.SetSize(8,16);
  Trend.SetSize(0,16);
//  for (int i=0; i<NoTrends(); i++)
//    Trend[i].Scl.pTB=&TB;
  //NSlots=NSlots_;
  bool NewDefaultColors = false;
  if (NewDefaultColors)
    {
    BackGroundColour = RGB(0x80, 0x80, 0x80); //RGB(0x00, 0x00, 0x00);
    BorderColour     = RGB(0x00, 0x00, 0x00); //RGB(0x80, 0x80, 0x80);
    GridColour       = RGB(0x00, 0x00, 0x00); //RGB(0x80, 0x80, 0x80);
    CTimeColour      = RGB(0x00, 0xff, 0x00);
    DigColour        = RGB(0xff, 0xff, 0xff);
    TextColourBusy   = RGB(0x00, 0x00, 0x00); //RGB(0xff, 0xff, 0xff);
    TextColourNotBusy= RGB(0x00, 0x00, 0x00); //RGB(0xff, 0xff, 0xff);
    }
  else
    {
    BackGroundColour = RGB(0x00, 0x00, 0x00);
    BorderColour     = RGB(0x80, 0x80, 0x80);
    GridColour       = RGB(0x80, 0x80, 0x80);
    CTimeColour      = RGB(0x00, 0xff, 0x00);
    DigColour        = RGB(0xff, 0xff, 0xff);
    TextColourBusy   = RGB(0xff, 0xff, 0xff);
    TextColourNotBusy= RGB(0xff, 0xff, 0xff);
    }
  GridStyle        = PS_DOT;
  CTimeStyle       = PS_SOLID;
  DigStyle         = PS_DOT;

  ShiftRatio = 0.15;
  iRqstNo = 0;
  iLastRqstNo = 0;
  fFixTimeBaseReqd = 0;
  fFirstActivate = 1;
  pTrnd=NULL;
  pText=NULL;
  //pFMngr=NULL;
  iFirstChgdFld = -1;
  iExecCnt = 0;
  bJustLoaded = 0;
  InitializeCriticalSection(&LockSection);
  bBusyBuildingPGM = 0;
  pFile=NULL;
  iNXGridDivs = 5;
  iNYGridDivs = 4;
  SD = 0;
  ED = 0;
  ST = 0;
  ET = 0;// CNM 1;
  Dur = 1;
  HS = 0;
  Scroll = 1;
  m_iFnctSubsStartIndex=-1;

  TB.EndTime = 3600.0;
  TB.StartTime = 0.0;
  TB.TrackingTime = 1;
  TB.AllowLatch = 1;

  iSltHiLite=-1;
  fSltHiLiteOn=false;

  ColumnWidths[0]=0;
  ColumnWidths[1]=0;
  ColumnWidths[2]=0;
  ColumnWidths[3]=0;

//  EO_Register(pExecName_TrendDoc, EOWrite_Msg | EOExec_Thread, /*Pri*/0,/*Stack*/0);
  EO_Register(pExecName_TrendDoc, EOWrite_Msg | EOExec_Msg, /*Pri*/0,/*Stack*/0);
  }

//---------------------------------------------------------------------------

CTagVwDoc::~CTagVwDoc()
  {
  if (pFile)
    {
    pFile->Close();
    delete pFile;
    }
  //if (FnMngrPresent())
  //  delete pFMngr;
  EO_DeRegister();

  DeleteCriticalSection(&LockSection);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnAttachDoc()
  {
  DocRoot::OnAttachDoc();
  TCB.AttachDoc(this, pTrnd, pText);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnActivate(flag bActivate)
  {
  DocRoot::OnActivate(bActivate);
  UpdateAllViewsByMsg(NULL, bActivate ? TGU_NowActivated : TGU_NowDeactivated, NULL);
  if (bActivate && fFirstActivate && !gs_pPrj->LoadBusy())
    {
    fFirstActivate = 0;
    PreloadSlots();
    ConnectAllTags();
    ReConnectTrends();
    TryScrollWindow();
    UpdateAllViewsByMsg(NULL, TGU_LoadComplete, NULL);
    FixTimeBase();
    }
  }

//---------------------------------------------------------------------------

BOOL CTagVwDoc::OnNewDocument()
  {
  if (!DocRoot::OnNewDocument())
    return False;
  if (gs_pPrj && !gs_pPrj->bDoingLoad)
    {
    if (gs_License.TrendWindowsAllowed()!=CK_InfiniteTrends && CWindowLists::GetTrendWndCount()>gs_License.TrendWindowsAllowed())
      {
      LogWarning("SysCAD", 0, "Cannot open new trend window, license allows maximum of %d", gs_License.TrendWindowsAllowed());
      return False;
      }
    }
  return True;
  }

//---------------------------------------------------------------------------

BOOL CTagVwDoc::OnOpenDocument(const char* pszPathName)
  {
  if (gs_pPrj && !gs_pPrj->bDoingLoad)
    {
    if (gs_License.TrendWindowsAllowed()!=CK_InfiniteTrends && CWindowLists::GetTrendWndCount()>gs_License.TrendWindowsAllowed())
      {
      bIsOpen = True; //ensures document/window is closed properly
      LogWarning("SysCAD", 0, "Cannot open trend window, license allows maximum of %d", gs_License.TrendWindowsAllowed());
      return False;
      }
    }
  return DocRoot::OnOpenDocument(pszPathName);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OpenVNT()
  {
  DocRoot::OpenVNT();
  gs_pCmd->OpenCommandBlk(&TCB);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::ConnectAllTags()
  {
  for (int i=0; i<NoSlots();i ++)
    if (DS[i].sTag.Length()>0)
      DS[i].ConnectTag(this, 0);//CT_GetRecorded);
  
  GetAllRecordedTags();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::GetAllRecordedTags()
  {
  CXM_Route HRoute;
  if (XFindObject(pExecName_Historian, HRoute))
    {
    CXM_HistoryExists *xb=new CXM_HistoryExists;
    CXMsgLst XM;
    for (int i=0; i<NoSlots(); i++)
      {
      if ((DS[i].bSlotType==STT_Data) && (!xb->xAddTag(i, DS[i].TagOnly())))
        {
        XM.PackMsg(xb);
        xb=new CXM_HistoryExists;
        i--;
        }
      }

    if (!xb->Empty())
      XM.PackMsg(xb);
    else
      delete xb;
  
    XSendMessage(XM, HRoute);
  
    while (XM.MsgAvail())
      {
      CXM_HistoryExists* pxb=(CXM_HistoryExists*)XM.MsgPtr(XM_HistoryExists);
      for (char *pTag=pxb->FirstTag(); pTag; pTag=pxb->NextTag(pTag))
        DS[pxb->GetIndex(pTag)].bRecorded = ((pxb->GetFlags(pTag) & RC_SM_HistRecOn)!=0);
      }

    //CXM_HistoryExists *xb=NULL;

    //bool Send=false;
    //bool Busy=true;
    //for (int i=0; Busy; )
    //  {
    //  if (!xb)
    //    xb=new CXM_HistoryExists;
    //  if ((DS[i].bSlotType==STT_Data) && (!xb->xAddTag(i, DS[i].TagOnly())))
    //    {
    //    i--;
    //    Send=true;
    //    }
    //  i++;
    //  
    //  Busy=(i<NoSlots());
    //  if (!xb->Empty() && (!Busy || Send))
    //    {
    //    CXMsgLst XM;
    //    XM.PackMsg(xb);
    //    XSendMessage(XM, HRoute);
    //    CXM_HistoryExists* pxb=XM.HistoryExists();//(sTagOnly());
    //    for (char *pTag=pxb->FirstTag(); pTag; pTag=pxb->NextTag(pTag))
    //      DS[pxb->GetIndex(pTag)].bRecorded = ((pxb->GetFlags(pTag) & RC_SM_HistRecOn)!=0);
    //    Send=false;
    //    xb=NULL;
    //    }
    //  }
    }
  }

//---------------------------------------------------------------------------

int CTagVwDoc::FreeTrendNo()
  {
  Lock();
  int i;
  CArray <int, int> Used;
  Used.SetSize(NoTrends());
  for (i=0; i<NoTrends(); i++) 
    Used[i]=-1;
  for (i=0; i<NoSlots(); i++)
    if (DS[i].nTrendNo>=0)
      Used[DS[i].nTrendNo]=i;
  for (i=0; i<NoTrends() && Used[i]>=0; i++);
  
  if (i>=NoTrends())
    {
    i=Trend.GetSize();
    Trend.SetSize(i+1,32);
    //Trend[i].Scl.pTB=&TB;
    }
  Free();
  return i;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::ToggleTrending(int iSltNo)
  {
  Lock();
  CTagVwSlot* Slt=&DS[iSltNo];
  flag Ok=False;
  if (Slt)
    {
    if (Slt->nTrendNo >= 0)
      {
      Slt->SetTrending(-1, this);
      UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
      ReConnectTrends();
      FixTimeBase();
      }
    else
      {
      Slt->SetTrending(FreeTrendNo(), this);
      UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
      ReConnectTrends();
      FixTimeBase();
      }
    Ok=True;
    }
  Free();
  return Ok;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::ToggleTrending(CIArray & iSltNo)
  {
  Lock();
  flag Ok=false;
  for (int i=0; i<iSltNo.GetSize(); i++)
    {
    CTagVwSlot* Slt=&DS[iSltNo[i]];
    if (Slt)
      {
      Slt->SetTrending((Slt->nTrendNo>=0) ? -1 : FreeTrendNo(), this);
      Ok=True;
      }
    }

  if (Ok)
    {
    ReConnectTrends();
    UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
    FixTimeBase();
    }
  Free();
  return Ok;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::SetTrending(CIArray & iSltNo, flag SetOn)
  {
  Lock();
  flag Ok=false;
  for (int i=0; i<iSltNo.GetSize(); i++)
    {
    CTagVwSlot* Slt=&DS[iSltNo[i]];
    if (Slt && ((Slt->nTrendNo<0) == SetOn))
      {
      Slt->SetTrending(SetOn ? FreeTrendNo() : -1, this);
      Ok=True;
      }
    }

  if (Ok)
    {
    ReConnectTrends();
    UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
    FixTimeBase();
    }
  Free();
  return Ok;
  }

//---------------------------------------------------------------------------

void CTagVwDoc::DeleteContents()
  {
  // TODO: add additional cleanup before doc-items are deleted
  DocRoot::DeleteContents();  // delete doc-items

  for (int i=0;i<NoSlots();i ++)
    {
    DS[i].Clear(i);
    //DS[i].dwWidth=1;// + i/ColourArrayLen;
    //DS[i].lPen.lopnStyle=BS_SOLID;
    //DS[i].lPen.lopnColor=PenColours[i % ColourArrayLen];
    //DS[i].lPen.lopnHatch=0;
    }

  fFirstActivate=1;

  iNXGridDivs=5;
  iNYGridDivs=4;
  SD = 0;
  ED = 0;
  ST = 0;
  ET = 0;// CNM 1;
  Dur=1;
  HS=0;
  Scroll=1;
  }

//---------------------------------------------------------------------------

enum Trend_Cols
  {
  enType,
  enLineNo,
  enTag,
  enMin,
  enMax,
  enTrendNo,
  enColor,
  enFormat,
  enDisplay,
  enOther,
  enDescription,
  enAutoScale
  };

BOOL CTagVwDoc::NewReadDocument(FILE* pFile)
  {
  char buff[2048];
  CSVColArray C;
  CSVColArray f;
  int Quote,nParms;
  fgets(buff, sizeof(buff), pFile); //get first title line
  while (!feof(pFile))
    {
    if (fgets(buff, sizeof(buff)-1, pFile))
      {
      int nFlds = ParseCSVTokens(buff, C, Quote);
      for (int i=0; i<nFlds; i++)
        {
        if (C[i])
          XStrTrim(C[i], "\"\' \t");
        }
      if ((_stricmp(C[enType], "Tag")==0) || (_stricmp(C[enType], "CtrlTag")==0) || (_stricmp(C[enType], "Comment")==0) || (_stricmp(C[enType], "Function")==0) )
        {
        i = SafeAtoI(C[enLineNo], -1);
        DS.CheckSize(i+1);
       
        if (i<0 || i>=NoSlots() || C[enLineNo]==NULL || strlen(C[enLineNo])==0)
          break;
        Strng Tg(C[enTag]);
        if (_stricmp(C[enType], "Comment")==0 && C[enTag][0]!='\'')
          Tg.Set("\'%s", C[enTag]);
        DS[i].bCtrlSlot=(_stricmp(C[enType], "CtrlTag")==0);
        DS[i].SetTag(Tg());
        DS[i].sMinVal=C[enMin];
        DS[i].sMaxVal=C[enMax];
        DS[i].bHex=0;
        //if (C[enDisplay]) 
        //  DS[i].bHex=SafeAtoI(strcmp(C[enDisplay], "???")==0);
        if (C[enTrendNo])
          {
          DS[i].nTrendNo=SafeAtoI(C[enTrendNo], -1);
          if (DS[i].nTrendNo>=NoTrends())
            Trend.SetSize(DS[i].nTrendNo+1);
          }
        if (C[enColor]) 
          {
          nParms = ParseCSVFunction(C[enColor], f, Quote);
          DS[i].lPen.lopnColor=RGB(SafeAtoI(f[1], 0), SafeAtoI(f[2], 0), SafeAtoI(f[3], 0));
          }
        if (C[enFormat])
          DS[i].Fmt.SetFmt(C[enFormat]);
        if (C[enDescription])
          DS[i].sDesc=C[enDescription];
        if (C[enAutoScale])
          {
          long S=SafeAtoI(C[enAutoScale], TAS_Off);
          DS[i].bAutoScaleOn=(S>0);
          DS[i].bScaleApplied=(byte)abs(S);
          }
        }
      else if (_stricmp(C[enType], "Other")==0)
        {
        nParms = ParseCSVFunction(C[enTag], f, Quote);
        if (nParms>0 && f[0])
          {
          if (_stricmp(f[0], "Counts")==0)
            {
            DS.CheckSize(SafeAtoI(f[1], DS.GetSize()));
            Trend.CheckSize(SafeAtoI(f[2], Trend.GetSize()));
            }
          else if (_stricmp(f[0], "Labels")==0)
            {
            SD  = (SafeAtoI(f[1], SD)==0 ? 0 : 1);
            ST  = (SafeAtoI(f[2], ST)==0 ? 0 : 1);
            ED  = (SafeAtoI(f[3], ED)==0 ? 0 : 1);
            ET  = (SafeAtoI(f[4], ET)==0 ? 0 : 1);
            Dur = (SafeAtoI(f[5], Dur)==0 ? 0 : 1);
            HS  = (SafeAtoI(f[6], HS)==0 ? 0 : 1);
            }
          else if (_stricmp(f[0], "Grid")==0)
            {
            iNXGridDivs = SafeAtoI(f[1], iNXGridDivs);
            iNYGridDivs = SafeAtoI(f[2], iNXGridDivs);
            }
          else if (_stricmp(f[0], "Timebase")==0)
            {
            TB.StartTime.Seconds = SafeAtoF(f[1], GTB.StartTime.Seconds);
            TB.EndTime.Seconds = TB.StartTime.Seconds+SafeAtoF(f[2], 1.0);
            TB.EndTime=TB.EndTime.GreaterThan(TB.StartTime);
            TB.TrackingTime = (SafeAtoI(f[3], TB.TrackingTime)==0 ? 0 : 1);
            TB.AllowLatch = 0;
            }
          else if (_stricmp(f[0], "General")==0)
            {
            Scroll = (SafeAtoI(f[1], Scroll)==0 ? 0 : 1);
            }
          else if (_stricmp(f[0], "ColumnWidths")==0)
            {
            ColumnWidths[0]= SafeAtoI(f[1], ColumnWidths[0]);
            ColumnWidths[1]= SafeAtoI(f[2], ColumnWidths[1]);
            ColumnWidths[2]= SafeAtoI(f[3], ColumnWidths[2]);
            ColumnWidths[3]= SafeAtoI(f[4], ColumnWidths[3]);
            }
          else if (_stricmp(f[0], "BackGroundColour")==0)
            {
            int R=GetRValue(BackGroundColour);
            int G=GetGValue(BackGroundColour);
            int B=GetBValue(BackGroundColour);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            BackGroundColour=RGB(R,G,B);
            }
          else if (_stricmp(f[0], "BorderColour")==0)
            {
            int R=GetRValue(BorderColour);
            int G=GetGValue(BorderColour);
            int B=GetBValue(BorderColour);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            BorderColour=RGB(R,G,B);
            }
          else if (_stricmp(f[0], "GridColour")==0)
            {
            int R=GetRValue(GridColour);
            int G=GetGValue(GridColour);
            int B=GetBValue(GridColour);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            GridColour=RGB(R,G,B);
            }
          else if (_stricmp(f[0], "CTimeColour")==0)
            {
            int R=GetRValue(CTimeColour);
            int G=GetGValue(CTimeColour);
            int B=GetBValue(CTimeColour);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            CTimeColour=RGB(R,G,B);
            }
          else if (_stricmp(f[0], "DigColour")==0)
            {
            int R=GetRValue(DigColour);
            int G=GetGValue(DigColour);
            int B=GetBValue(DigColour);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            DigColour=RGB(R,G,B);
            }
          else if (_stricmp(f[0], "TextColourBusy")==0)
            {
            int R=GetRValue(TextColourBusy);
            int G=GetGValue(TextColourBusy);
            int B=GetBValue(TextColourBusy);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            TextColourBusy=RGB(R,G,B);
            }
          else if (_stricmp(f[0], "TextColourNotBusy")==0)
            {
            int R=GetRValue(TextColourNotBusy);
            int G=GetGValue(TextColourNotBusy);
            int B=GetBValue(TextColourNotBusy);
            R = SafeAtoI(f[1], R);
            G = SafeAtoI(f[2], G);
            B = SafeAtoI(f[3], B);
            TextColourNotBusy=RGB(R,G,B);
            }
          }
        }
      }
    }
  return true;
  }

//---------------------------------------------------------------------------
//June 1999: New tabular/database/csv type trend document implemented
#define AllowOldTrendFormatLoad 0 //June 2002 support for recovering old type trend format removed

BOOL CTagVwDoc::ReadDocument(const char* pszPathName, FILE* pFile)
  {
  DeleteContents(); // Temporary - Until fixed in project

  if (!feof(pFile))
    {
    bJustLoaded = 1;
    char c;
    flag Busy=1;
    c=fgetc(pFile);
    if (c=='T')
      {
      NewReadDocument(pFile); //load new style trend document...
      Busy=0; //allready loaded
      }
    ungetc(c, pFile);
  
#if AllowOldTrendFormatLoad
    char buff[2046];
    while (Busy)
      {//load old style (up to June 1999) trend document...
      c=fgetc(pFile);
      switch (c)
        {
        case ';': fgets(buff, sizeof(buff), pFile); break;
        case 'n': fgets(buff, sizeof(buff), pFile); break;
        case 's': 
          {
          fgets(buff, sizeof(buff), pFile);
          pchar p=buff;
          int i=atoi(StrToken(p));
          if (i<0 || i>=NoSlots() || p==NULL)
            break;
          DS[i].SetTag(StrToken(p));
          DS[i].sMinVal=StrToken(p);
          DS[i].sMaxVal=StrToken(p);
          if (p) DS[i].bHex=atoi(StrToken(p));
          if (p) DS[i].nTrendNo=atoi(StrToken(p));
          if (p) DS[i].lPen.lopnColor=atoi(StrToken(p));
          if (p) DS[i].Fmt.SetFmt(StrToken(p));
          break;
          };
        case 'f':
          {
          fgets(buff, sizeof(buff), pFile);
          pchar p=buff;
          iNXGridDivs = atoi(StrToken(p));
          iNYGridDivs = atoi(StrToken(p));
          ST = atoi(StrToken(p));
          SD = atoi(StrToken(p));
          pchar p1 = StrToken(p);
          if (p1) ET = atoi(p1);
          p1 = StrToken(p);
          if (p1) ED = atoi(p1);
          p1 = StrToken(p);
          if (p1) Dur = atoi(p1);
          p1 = StrToken(p);
          if (p1) HS = atoi(p1);
          p1 = StrToken(p);
          if (p1) Scroll = atoi(p1);
          break;
          };
        case 't':
        case 'g':
          {
          fgets(buff, sizeof(buff), pFile);
          pchar p=buff;
          double ts=atof(StrToken(p));
          double td=atof(StrToken(p));
          flag tr=atoi(StrToken(p));
          if (c=='g')
            {
            GTB.StartTime=ts;
            GTB.EndTime=ts+td;
            GTB.TrackingTime=tr;
            GTB.AllowLatch=0;
            }
          else
            {
            TB.StartTime=ts;
            TB.EndTime=ts+td;
            TB.TrackingTime=tr;
            TB.AllowLatch=0;
            }

          break;
          }
        case 'c': fgets(buff, sizeof(buff), pFile); break; //custom colours NOT stored with each trend document
        default :
          ungetc(c, pFile);
          Busy=0;
        }
      }
#else
    if (Busy)
      {
      Strng DocTitle((const char*)(GetTitle()));
      LogError(DocTitle(), 0, "Old format trend document cannot be recovered.");
      }
#endif
    }

  flag b=TCB.ReadDocument(pszPathName, pFile);

  return b;
  }

//---------------------------------------------------------------------------

BOOL CTagVwDoc::WriteDocument(const char* pszPathName, FILE* pFile)
  {
  fprintf(pFile, "Type,LineNo,Tag,Min,Max,TrendNo,Color,Format,Display,Other,Description,AutoScaleType\n");
  fprintf(pFile, "Other,-1,\"Counts(%i,%i)\",,,-1,,,,,\"Counts(Slots,Trends)\"\n", NoSlots(), NoTrends());
  for (int i=0; i<NoSlots(); i++)
    if (DS[i].sTag.Len()>0)
      {
      fprintf(pFile, "%s,%i,\"%s\",\"%s\",\"%s\",%i,",
        (DS[i].IsCtrlSlot() ? "CtrlTag" : (DS[i].IsComment() ? "Comment" : (DS[i].HasFunct() ? "Function" : "Tag"))), 
        i, DS[i].sTag(), DS[i].sMinVal() ? DS[i].sMinVal() : "", DS[i].sMaxVal() ? DS[i].sMaxVal() : "", DS[i].nTrendNo);
      fprintf(pFile, "\"RGB(%i,%i,%i)\",\"%s\",\"Decimal\",\"\",\"%s\",%i\n",
        0x000000ff & DS[i].lPen.lopnColor, 0x000000ff & (DS[i].lPen.lopnColor >> 8), 0x000000ff & (DS[i].lPen.lopnColor >> 16), 
        DS[i].Fmt.Name(), DS[i].sDesc() ? DS[i].sDesc() : "", (DS[i].bAutoScaleOn ? 1:-1)*DS[i].bScaleApplied);
      }  

  fprintf(pFile, "Other,-1,\"Labels(%i,%i,%i,%i,%i,%i)\",,,-1,,,,,\"ShowLabels(StartDate,StartTime,EndDate,EndTime,Duration,HundredSecs)\"\n", SD, ST, ED, ET, Dur, HS);
  fprintf(pFile, "Other,-1,\"Grid(%i,%i)\",,,-1,,,,,\"Grid(NoOfXDivisions,NoOfYDivisions)\"\n", iNXGridDivs, iNYGridDivs);
  fprintf(pFile, "Other,-1,\"Timebase(%g,%g,%i)\",,,-1,,,,,\"Timebase(Start,Duration,Tracking)\"\n", TimeBaseStart().Seconds, TimeBaseDuration().Seconds, TimeBaseTracking());
  fprintf(pFile, "Other,-1,\"General(%i)\",,,-1,,,,,\"General(Scroll)\"\n", Scroll);
  fprintf(pFile, "Other,-1,\"ColumnWidths(%i,%i,%i,%i)\",,,-1,,,,,\"ColumnWidths(Tag,Value,Min,Max)\"\n", ColumnWidths[0], ColumnWidths[1], ColumnWidths[2], ColumnWidths[3]);

  fprintf(pFile, "Other,-1,\"BackGroundColour (%i,%i,%i)\",,,-1,,,,,\"BackGroundColour(R,G,B)\"\n", GetRValue(BackGroundColour), GetGValue(BackGroundColour), GetBValue(BackGroundColour));
  fprintf(pFile, "Other,-1,\"BorderColour(%i,%i,%i)\",,,-1,,,,,\"BorderColour(R,G,B)\"\n", GetRValue(BorderColour), GetGValue(BorderColour), GetBValue(BorderColour));
  fprintf(pFile, "Other,-1,\"GridColour(%i,%i,%i)\",,,-1,,,,,\"GridColour(R,G,B)\"\n", GetRValue(GridColour), GetGValue(GridColour), GetBValue(GridColour));
  fprintf(pFile, "Other,-1,\"CTimeColour(%i,%i,%i)\",,,-1,,,,,\"CTimeColour(R,G,B)\"\n", GetRValue(CTimeColour), GetGValue(CTimeColour), GetBValue(CTimeColour));
  fprintf(pFile, "Other,-1,\"DigColour(%i,%i,%i)\",,,-1,,,,,\"DigColour(R,G,B)\"\n", GetRValue(DigColour), GetGValue(DigColour), GetBValue(DigColour));
  fprintf(pFile, "Other,-1,\"TextColourBusy(%i,%i,%i)\",,,-1,,,,,\"TextColourBusy(R,G,B)\"\n", GetRValue(TextColourBusy), GetGValue(TextColourBusy), GetBValue(TextColourBusy));
  fprintf(pFile, "Other,-1,\"TextColourNotBusy(%i,%i,%i)\",,,-1,,,,,\"TextColourNotBusy(R,G,B)\"\n", GetRValue(TextColourNotBusy), GetGValue(TextColourNotBusy), GetBValue(TextColourNotBusy));

  return TCB.WriteDocument(pszPathName, pFile);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::ReConnectTrends()
  {
  Lock();
  //dbgpln("Reconnect Trends================");
  for (int i=0; i<NoTrends(); i++)
    Trend[i].Connect(i, NULL, this);
  for (i=0; i<NoSlots(); i++)
    {
    int t=DS[i].nTrendNo;
    if (t>=0)
      {
      if (t>=NoTrends())
        Trend.SetSize(t+1);
      Trend[t].Connect(t, &DS[i], this);
      }
    }  
  Free();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::ClearAllTrends()
  {
  Lock();
  for (int t=0; t<NoTrends(); t++)
    Trend[t].Clear();
  Free();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::FixTimeBase()
  {
  Lock();
  if (fTimeBaseGlobal)
    TB=GTB;

  CTimeValue ST=TB.StartTime;
  CTimeValue ETr=Min(TB.EndTime, gs_Exec.TheTime);
  CTimeValue ET=TB.EndTime;

  ClearAllTrends();
  for (int i=0; i<NoTrends(); i++)
    if (Trend[i].Connected())
      {
      Trend[i].pSlt->dValueLowest=dNAN;
      Trend[i].pSlt->dValueHighest=dNAN;

      Trend[i].pSlt->UpdateYMnMx(this);

      CTimeValue LMT=Trend[i].LastMeasTime;
      if (gs_Exec.Busy() && ValidTime(LMT) && (LMT >= ST) && (LMT <= ET))
        {
        Trend[i].AppendMeasured(LMT);
        Trend[i].dBuildStartTime=LMT;
        }
      else
        Trend[i].dBuildStartTime=dNAN;

      #if dbgTrends 
      if (dbgReBuild()) dbgpln("  Slot::Rqst() [%i] %s",i,Trend[i].pSlt->sTag());
      #endif
      }

  CXM_Route HRoute;
  if (XFindObject(pExecName_Historian, HRoute))
    {
    flag FoundOne = False;
    CXM_QueryHistory *xb=NULL;
    for (int i=0; i<NoTrends(); i++)
      //if (Trend[i].Connected())
      if (Trend[i].Connected() && Trend[i].pSlt->bRecorded) //is this better? What if you stop recording a slot but you want to browse the historian?
        {
        #if dbgTrends 
        if (dbgReBuild()) dbgpln("  Slot::Rqst() [%i] %s",i,Trend[i].pSlt->sTag());
        #endif
        if (!xb)
          xb = new CXM_QueryHistory (ST.Seconds, ETr.Seconds, ++iRqstNo, (long)this);
        xb->xAddTag(i, Trend[i].pSlt->sTag());
        FoundOne = True;
        }

    if (FoundOne)
      {
      CXMsgLst XM;
      XM.PackMsg(xb);
      XM.PackMsg(new CXM_Route(HRoute));
      #if dbgTrends
      if (dbgReBuild())
        dbgpln("TrendDoc::FixTimeBase() S:%8.1f RqE:%8.1f E:%8.1f", dbgTimeRound(ST), dbgTimeRound(ETr), dbgTimeRound(ET));
      if (dbgQueryHistory())
        XM.dbgDump(0, "Trends");
      #endif
  

      if (!XSendMessage(XM, HRoute))
        dbgpln("No Data Found in Historian");

//Sleep(1000);      
      }
    }
  Free();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::TryScrollWindow()
  {
  CTagVwTrend* pTrend = (CTagVwTrend*)pTrnd;
  if (!pTrend->MouseFlags)
    if (TB.TrackingTime && gs_Exec.TheTime >= TB.EndTime)
      {
      #if dbgTrends 
      if (dbgReBuild()) 
        dbgpln("Trend::ScrollWindow()");
      #endif
      CTimeValue PosCurs = TB.EndTime-((TB.EndTime-TB.StartTime)*ShiftRatio);
      CTimeValue d = gs_Exec.TheTime-PosCurs;
      for (int i=0; i<NoTrends(); i++)
        if (Trend[i].Connected())
          Trend[i].ShiftDrwPts(d);
      TB.StartTime += d;
      TB.EndTime += d;
      CFloatLParamUnion FL;
      FL.f=(float)d.Seconds;
      SendOrPostMessage(pTrnd, !gs_TheRunMngr.AutomationBusy(), WMU_SCROLL, 0, FL.L);
      }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::SetTimebase(CTimeValue Start, CTimeValue Duration, flag bDoGlobalUpdate)
  {
  Lock();
    
  Duration=Max(Duration,Max(0.01, CTimeValue::MinResolution));
  TB.StartTime=Start;
  TB.EndTime=Start+Duration;
  CTimeValue ExecTime = gs_Exec.TheTime;
  if (TB.EndTime > gs_Exec.TheTime)
    {
    CTimeValue d(TB.EndTime - (gs_Exec.TheTime+1.0+(Duration*ShiftRatio)));
    TB.StartTime-=d;
    TB.EndTime-=d;
    }
  if (TB.AllowLatch && (TB.EndTime>=gs_Exec.TheTime))
    TB.TrackingTime=1;

  GTB=TB;
  
  Free();

  UpdateTimebase(TB);

  if (bDoGlobalUpdate && fTimeBaseGlobal)
    {
    for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
      if (ScdApp()->TemplateExists(iTemp))
        {
        POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
        while (Pos)
          {
          CTagVwDoc* pUpdDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
          if (pUpdDoc!=this)
            pUpdDoc->UpdateTimebase(GTB);
          }
        }
    }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::AdjustTimebase(double ShiftFrac, double Scale, flag TrackingOn, flag AllowLatch)
  {
  Lock();
  
  CTimeValue Shift;
  CTimeValue ExecTime=gs_Exec.TheTime;
  if (ShiftFrac > 1.0e35)
//    Shift=gs_Exec.TheTime-TB.EndTime+0.1;//10;
    Shift=ExecTime-(TB.StartTime+(TB.EndTime-TB.StartTime)*(1.0-ShiftRatio));
  else if (ShiftFrac < -1.0e35)
    Shift=CTimeValue(0.0);
  else
    Shift=((TB.EndTime-TB.StartTime)*ShiftFrac);

  double Win=(TB.EndTime-TB.StartTime).Seconds;
  double edWin=0;
  double sdWin=0;
  if (fabs(Scale-1.0) > 0.01)
    {
    long IntScl=100L; // Minimum is 100th of second
    if (Win*Scale*IntScl<INT_MAX)
      {
      long nWin=(long)(Win*Scale*IntScl+0.5);
      if (nWin > 3600L*24L*IntScl)          // if >= 1 day then window modulo 6 hour
        nWin=(nWin/(3600L*6*IntScl))*3600L*6*IntScl;
      else if (nWin > 3600L*2*IntScl)        // if >= 2 hour then window modulo 1 hour
        nWin=(nWin/(3600L*IntScl))*3600L*IntScl;
      else if (nWin > 3600L*IntScl)        // if >= 1 hour then window modulo 20 mins
        nWin=(nWin/(1200L*IntScl))*1200L*IntScl;
      else if (nWin > 300L*IntScl)         // if >= 5 mins then window modulo 5 mins
        nWin=(nWin/(300L*IntScl))*300L*IntScl;
      else if (nWin > 60L*IntScl)         // if >= 1 mins then window modulo 1 mins
        nWin=(nWin/(60L*IntScl))*60L*IntScl;
      else if (nWin > 10L*IntScl)         // if >= 10 secs then window modulo 10 secs
        nWin=(nWin/(10L*IntScl))*10L*IntScl;
      else 
        nWin=Max(1L, nWin);

      double dWin=(((double)nWin)/IntScl-Win);
 
      edWin=(TB.TrackingTime ? 0 : dWin/2);
      sdWin=-(dWin-edWin);
      }
    else
      {
      double dWin=Win*Scale-Win;
      edWin=(TB.TrackingTime ? 0 : dWin/2);
      sdWin=-(dWin-edWin);
      }
    }

  TB.StartTime+=Shift+sdWin;
  TB.EndTime+=Shift+edWin;

  if (TB.EndTime > gs_Exec.TheTime)
    {
    CTimeValue PosCurs=TB.EndTime-((TB.EndTime-TB.StartTime)*ShiftRatio);
    CTimeValue d=PosCurs-gs_Exec.TheTime;
    TB.StartTime-=d;
    TB.EndTime-=d;
    }
  TB.TrackingTime=TrackingOn;//(TB.EndTime>=gs_Exec.TheTime);
  TB.AllowLatch=AllowLatch;
  GTB=TB;

  Free();
  UpdateTimebase(TB);
  if (fTimeBaseGlobal)
    {
    for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
      if (ScdApp()->TemplateExists(iTemp))
        {
        POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
        while (Pos)
          {
          CTagVwDoc* pUpdDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
          if (pUpdDoc!=this)
            pUpdDoc->UpdateTimebase(GTB);
          }
        }
    }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::DoSetTimeGlobal(flag On) 
  {
  for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
    if (ScdApp()->TemplateExists(iTemp))
      {
      POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
      if (Pos)
        {
        ((CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos)))->SetTimeGlobal(On);
        break;
        }
      }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::SetTimeGlobal(flag On) 
  { 
  fTimeBaseGlobal=On; 
  GTB=TB; 
  if (fTimeBaseGlobal)
    {
    for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
      if (ScdApp()->TemplateExists(iTemp))
        {
        POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
        while (Pos)
          {
          CTagVwDoc* pUpdDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
          if (pUpdDoc->TB != GTB)
            pUpdDoc->UpdateTimebase(GTB);
          }
        }
    }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::UpdateTimebase(TrndTimeBase & TTB)
  {
  TB=TTB;
  UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay, NULL);
  FixTimeBase();
  }

//---------------------------------------------------------------------------

int CTagVwDoc::FindClosestTrendPt(POINT &Pt, int TrndNo, TrackTrend &Trk)//, int &TrndPt, long &Distance, POINT &XY)
  {
  flag Found=0;
  if (Trend[TrndNo].pSlt)
    {
    POSITION Pos=Trend[TrndNo].PtBlks.GetHeadPosition();
    while (Pos)
      {
      CTrendPtBlk* p=Trend[TrndNo].PtBlks.GetNext(Pos);
      int np=p->NPts();
      for (int j=1; j<np; j++)
        {
        POINT St, En;
        if (p->Pt[j-1].x <= p->Pt[j].x)
          {
          St=p->Pt[j-1];
          En=p->Pt[j];
          }
        else
          {
          St=p->Pt[j];
          En=p->Pt[j-1];
          }

        long SqLenS = Sqr(En.x - St.x) + Sqr(En.y - St.y);
        long SqLen1=Sqr(Pt.x - St.x) + Sqr(Pt.y - St.y);
        long SqLen2=Sqr(Pt.x - En.x) + Sqr(Pt.y - En.y);
        if (abs(SqLen1 - SqLen2) < SqLenS)
          { //              Between end #1 & end # 2
          double Ratio=(SqLen1 - SqLen2 + SqLenS) / (2.0 * SqLenS);
          POINT TXY;
          TXY.x=(long)(St.x + Ratio * (En.x - St.x));
          TXY.y=(long)(St.y + Ratio * (En.y - St.y));
          long Dist=(Sqr(Pt.x - TXY.x) + Sqr(Pt.y - TXY.y));
          if (Dist < Trk.Distance)
            {
            Trk.XY=TXY;
            Trk.Distance=Dist;
            Trk.Pt=j;
            Trk.No=TrndNo;
            Trk.pBlk=p;
            Found=1;
            }
          }
        else
          {
          if (SqLen2 > SqLen1)
            {//                    Closest to end # 1
            if (SqLen1 < Trk.Distance)
              {
              Trk.XY=St;
              Trk.Distance=SqLen1;
              Trk.Pt=j;
              Trk.No=TrndNo;
              Trk.pBlk=p;
              Found=1;
              }
            }
          else
            {//                    Closest to end # 2
            if (SqLen2 < Trk.Distance)
              {
              Trk.XY=En;
              Trk.Distance=SqLen2;
              Trk.Pt=j;
              Trk.No=TrndNo;
              Trk.pBlk=p;
              Found=1;
              }
            }
          }
        }
      }
    }
  return Found;
  };

//===========================================================================

int CTagVwDoc::FindClosestTrendPtVert(POINT &Pt, int TrndNo, TrackTrend &Trk)//, int &TrndPt, long &Distance, POINT &XY)
  {
  flag Found=0;
  if (Trend[TrndNo].pSlt)
    {
    POSITION Pos=Trend[TrndNo].PtBlks.GetHeadPosition();
    while (Pos)
      {
      CTrendPtBlk* p=Trend[TrndNo].PtBlks.GetNext(Pos);
      int np=p->NPts();
      for (int j=1; j<np; j++)
        {
        POINT St, En;
        if (p->Pt[j-1].x <= p->Pt[j].x)
          {
          St=p->Pt[j-1];
          En=p->Pt[j];
          }
        else
          {
          St=p->Pt[j];
          En=p->Pt[j-1];
          }

        if (St.x <= Pt.x && En.x >= Pt.x)
          {
          POINT TXY;
          TXY.x=Pt.x;
          if (St.x == En.x)
            TXY.y=Max(St.y, En.y);
          else
            TXY.y=St.y+(long)((En.y - St.y)*((Pt.x-St.x)/(float)(En.x-St.x)));
          long Dist=Sqr(Pt.y - TXY.y);
          if (Dist < Trk.Distance)
            {
            Trk.XY=TXY;
            Trk.Distance=Dist;
            Trk.Pt=j;
            Trk.No=TrndNo;
            Trk.pBlk=p;
            Found=1;
            }
          break;
          }
        }
      }
    }
  return Found;
  };

//---------------------------------------------------------------------------

int CTagVwDoc::FindClosestTrend(FCT_How How, POINT &Pt, TrackTrend &ClsTrnd, int& LastTrndNo, int RqdTrendNo/*=-1*/)
  {
  Lock();

  flag Found=0;
  if (RqdTrendNo>=0)
    LastTrndNo=RqdTrendNo;
  ClsTrnd.No=-1;
  ClsTrnd.Pt=-1;
  ClsTrnd.Distance=LONG_MAX;
  for (int ii=0,i=LastTrndNo; ii<NoTrends(); ii++,i++)
    {
    if (i>=NoTrends())
      i=0;
    if (Trend[i].pSlt)
      {
      switch (How)
        {
        case FCT_Radial:
          if (FindClosestTrendPt(Pt, i, ClsTrnd))//.Pt, ClsTrnd.Distance, ClsTrnd.XY))
            {
            //ClsTrnd.No=i;
            Found=1;
            }
          break;
        case FCT_Vertical:
          if (FindClosestTrendPtVert(Pt, i, ClsTrnd))//.Pt, ClsTrnd.Distance, ClsTrnd.XY))
            {
            //ClsTrnd.No=i;
            Found=1;
            }
          break;
        }
      }
    if (RqdTrendNo>=0)
      break;
    }

  Free();
  LastTrndNo=Max(0, ClsTrnd.No);
  if (!Found)
    ClsTrnd.Clear();
  return Found;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::StartFile(char* Filename)
  {
  if (pFile)
    return false;
  CFileException e;
  pFile = new CStdioFile;
  if ( !pFile->Open(Filename, CFile::modeCreate | CFile::modeWrite, &e) )
    {
    dbgpln("File %s could not be opened, error:%i", Filename, e.m_cause);
    delete pFile;
    pFile = NULL;
    return false;
    }
  return true;
  }

//---------------------------------------------------------------------------

int CTagVwDoc::FindTrendTag(FindTrendTagHelper &Pos)
  {
  if (Pos.iLastPos<0 || Pos.iLastPos>=NoSlots())
    Pos.iLastPos = 0;
  for ( ; Pos.iLastPos<NoSlots(); Pos.iLastPos++)
    if (DS[Pos.iLastPos].sTag.FindI(Pos.sTag())>=0)
      {
      Pos.iFoundPos = Pos.iLastPos;
      Pos.iFoundCnt++;
      return Pos.iFoundPos;
      }
  Pos.iFoundPos = -1;
  return Pos.iFoundPos;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::FindTag(FindTrendTagHelper &Pos)
  {
  if (Pos.sTag.Len()>0)
    {
    const int i = FindTrendTag(Pos);
    if (i>=0)
      {
      POSITION pos = GetFirstViewPosition();
      if (pos != NULL)
        {
        CView* pView = GetNextView(pos);
        if (pView->GetFocus()!=pView)
          MDIActivateThis(pView); //bring the window to the front
        if (pView->GetParent()->GetParent()->IsIconic())
          pView->GetParent()->GetParent()->ShowWindow(SW_RESTORE);
        if (dynamic_cast<CTagVwText*>(pView)==NULL)
          pView = GetNextView(pos);
        CTagVwText * pTxtView=dynamic_cast<CTagVwText*>(pView);
        if (pTxtView)
          {
          pTxtView->SelectTrendTag(i);
          }
        return True;
        }
      }
    }
  return False;
  }

//---------------------------------------------------------------------------

void CTagVwDoc::AdjustTimebaseToEndAll()
  {
  for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
    if (ScdApp()->TemplateExists(iTemp))
      {
      POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
      while (Pos)
        {
        CTagVwDoc* pUpdDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
        CTimeValue duration = pUpdDoc->TB.EndTime - pUpdDoc->TB.StartTime;
        CTimeValue start = gs_Exec.TheTime+10.0; //force to end
        pUpdDoc->SetTimebase(start, duration, false);
        }
      }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::RebuildAll()
  {
  for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
    if (ScdApp()->TemplateExists(iTemp))
      {
      POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
      while (Pos)
        {
        CTagVwDoc* pUpdDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
        pUpdDoc->Rebuild();
        }
      }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::Rebuild()
  {
  char Buff[256];
//  CStopWatch SW;
//  SW.Start();
  sprintf(Buff, "Reconnecting all tags for %s", (const char*)GetTitle());
  CWaitMsgCursor Wait(Buff);
  PreloadSlots();
  ConnectAllTags();
  //for (int i=0; i<NoSlots(); i++)
  //  if (DS[i].sTag.Length()>0)
  //    DS[i].ConnectTag(this);
  BuildFunctions();
  ReConnectTrends();
  TryScrollWindow();
  
  UpdateAllViewsByMsg(NULL, TGU_LoadComplete, NULL);
  FixTimeBase();
//  SW.Stop();
//  dbgpln("Rebuild : %10.4f", SW.Secs()*1e3);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendRefresh() 
  {
  Rebuild();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendRefreshall() 
  {
  RebuildAll();
  }

inline flag EnableHistorianOK() { return gs_pPrj && gs_pPrj->pPrjDoc && gs_pPrj->bHstOK; };

void CTagVwDoc::OnTrendRecord() 
  {
  if (EnableHistorianOK())
    {
    CWaitCursor Wait;
    flag Chng = 0;
    for (int i=0; i<NoSlots(); i++)
      if (DS[i].sTag.Length()>0)
        Chng |= DS[i].RecordTag(this, True);
    if (Chng)
      UpdateAllViewsByMsg(NULL, TGU_TagView, NULL);
    }
  }

void CTagVwDoc::OnUpdateTrendRecord(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(EnableHistorianOK());
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendRecordall() 
  {
  for (int iTemp=iTrendTemplate; iTemp<=iControlTemplate; iTemp++)
    if (ScdApp()->TemplateExists(iTemp))
      {
      POSITION Pos = ScdApp()->Template(iTemp).GetFirstDocPosition();
      while (Pos)
        {
        CTagVwDoc* pUpdDoc = (CTagVwDoc*)(ScdApp()->Template(iTemp).GetNextDoc(Pos));
        pUpdDoc->OnTrendRecord();
        }
      }
  }

void CTagVwDoc::OnUpdateTrendRecordall(CCmdUI* pCmdUI) 
  {
  pCmdUI->Enable(EnableHistorianOK());
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendFitAll(UINT Id) 
  {
  Lock();
  int OK=false;
  for (int i=0; i<NoSlots(); i++)
    if (ProcessScaleMenu(Id, &DS[i], DS[i].dValue))
      OK=true;
  if (OK)
    {
    UpdateAllViews(NULL, TGU_ClearTrendDisplay|0x1000, NULL);
    FixTimeBase();
    }
  Free();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::LoadData(CTagVwSlot * pSlot, int UpdateOpts)
  {
  if (pSlot)
    {
    pSlot->LoadData(this);
    }
  else
    {
    for (int i=0; i<NoSlots(); i++)
      if (DS[i].sTag.Length()>0)
        DS[i].LoadData(this);
    }
  if (UpdateOpts)
    UpdateAllViewsByMsg(NULL, UpdateOpts/*TGU_LoadDataComplete*/, NULL);
  }

//---------------------------------------------------------------------------

void CTagVwDoc::InsertLine(int iLine, int N)
  {
  if (N<1)
    return;
  Lock();
  iLine = Max(0, iLine);
  for (int i=NoSlots()-1; i>=iLine+N; i--)
    DS[i] = DS[i-N];
  for ( ; i>=iLine; i--)
    DS[i].ClearIt(this);
  ReConnectTrends();
  TryScrollWindow();
  UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
  FixTimeBase();
  Free();
  XBuildMyDataLists();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::DeleteLine(int iLine, int N)
  {
  if (N<1)
    return;
  Lock();
  iLine = Max(0, iLine);
  for (int i=iLine; i<NoSlots()-N; i++)
    DS[i] = DS[i+N];
  for (  ; i<NoSlots(); i++)
    DS[i].ClearIt(this);
  ReConnectTrends();
  TryScrollWindow();
  UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
  FixTimeBase();
  XBuildMyDataLists();
  Free();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::ClearLine(int iLine, int N)
  {
  if (N<1)
    return;
  Lock();
  iLine = Max(0, iLine);
  for (int i=iLine; i<iLine+N; i++)
    DS[i].ClearIt(this);
  ReConnectTrends();
  TryScrollWindow();
  UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
  FixTimeBase();
  XBuildMyDataLists();
  Free();
  }

//---------------------------------------------------------------------------
/*Responds to a message sent by the historian, which is returning data points
for a trend.*/
DWORD CTagVwDoc::EO_Message(CXMsgLst &XM, CXM_Route &Route)
  {
  DWORD RetCode = 0;
  flag DoFlushEvents=0;
  
  Lock();

  //long nTrndEvnts=0;
  //dbgpln("Receive EO_Message %li",GetTickCount());

  #if dbgTrends
  if (dbgHistoryData())
    XM.dbgDump(0, "TAGDoc");
  #endif
  CTagVwTrend* pTrend = (CTagVwTrend*)pTrnd;
  while (XM.MsgAvail())
    switch (XM.MsgId())
      {
      case XM_HistoryDataError:
        {
        CXM_HistoryDataError* p = XM.HistoryDataError();
        switch (p->lErrorNumber)
          {
          case 1:
            LogNote("Trend", 0, "Historian data file %s not available", p->cFileName);
            break;
          case 2:
            {
            TRACE("RESTART Trend Data\n");
            }
            break;
          }
        break;
        }
      case XM_HistoryData:
        {
        CXM_HistoryData* p = XM.HistoryData();
        #if dbgTrends
        if (dbgHistoryData())
          dbgpln("History Data %4i  %14.3f = %12g",p->iTrndNo, p->dTime, p->dVal);
        #endif

        int iT=p->iTrndNo, iR=p->iRqstNo;
        ASSERT(iT>=0 && iT<NoTrends());

        //add the point to the specified trend no...
        if (iR==iRqstNo)
          {
          if (iR!=iLastRqstNo)
            {
            #if dbgTrends 
            if (dbgReBuild()) dbgpln("Trend::OnTrendEvent() [%i] {%i} %s",iT,iR,Trend[iT].pSlt->sTag());
            #endif
            iLastRqstNo=iR;
            }
          Trend[iT].BuildFromEvent(p->dTime, p->dVal);
          DoFlushEvents=1;
          }

        RetCode = 1;
        //nTrndEvnts++;
        break;
        }
      case XM_QueryString:
        {
        if (!pFile)
          break;
        CXM_QueryString* p = XM.QueryString();
        if (strcmp(p->String(), "TheEnd")==0)
          {
          pFile->Close();
          LogNote("Trend", 0, "Query to file complete '%s'", (const char*)(pFile->GetFileName()));
          delete pFile;
          pFile = NULL;
          }
        else
          {
          pFile->WriteString(p->String());
          pFile->WriteString("\n");
          }
        break;
        }    
      default: 
        ASSERT(0);
      }
  Free();

  if (RetCode)
    {
    if (DoFlushEvents)
      pTrnd->SendMessage(WMU_ONUPDATE, TGU_UpdateAll, 0);
    }
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_LoadComplete(FilingControlBlock &FCB)
  {
  // This Recconnect Occurs if Loading Trend Document as part of a Project
  if (FCB.Type()==TOF_DBase)
    Rebuild();
  return True;
  }

//---------------------------------------------------------------------------

int CTagVwDoc::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {//will I allow the tag to be changed...
  return EOCT_NOTFOUND;
  }

//---------------------------------------------------------------------------

int CTagVwDoc::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {//a tag has been changed, change trend view...
  RequestUpdateXRefLists();

  int Err = EOCT_NOTFOUND;
  const int len = strlen(pOldTag);
  for (int i=0; i<NoSlots(); i++)
    if (DS[i].TagValid() && (DS[i].sTag.Len()>=len))
      {
      if (_strnicmp(pOldTag, DS[i].sTag(), len)==0 && (DS[i].sTag.Len()==len || DS[i].sTag[len]=='.' || DS[i].sTag[len]==' '))
        {
        Strng s = pNewTag;
        s += DS[i].sTag.Mid(len, 256);
        DS[i].SetTag(s());
        Err = EOCT_DONE;
        }
      else
        {
        int OpenPos = DS[i].sTag.Find('[');
        if (OpenPos>0)
          {
          int ClosePos = DS[i].sTag.Find(']');
          if (ClosePos-OpenPos-1==len && _strnicmp(pOldTag, &(DS[i].sTag[OpenPos+1]), len)==0)
            {
            Strng s = DS[i].sTag.Left(OpenPos+1);
            s += pNewTag;
            s += DS[i].sTag.Mid(ClosePos, 256);
            DS[i].SetTag(s());
            Err = EOCT_DONE;
            }
          }
        }
      }
  if (Err==EOCT_DONE)
    {
    UpdateAllViewsByMsg(NULL, TGU_Toggle, NULL);
    XBuildMyDataLists();
    }
  return Err;
  }

//---------------------------------------------------------------------------

int CTagVwDoc::EO_QueryDeleteTag(pchar pDelTag)
  {//will I allow the tag to be deleted...
  return EODT_NOTFOUND; 
  }

//---------------------------------------------------------------------------

int CTagVwDoc::EO_DeleteTag(pchar pDelTag)
  {//a tag has been deleted, ...
  int RetCode = EODT_NOTFOUND;

  //TODO Delete tag functionality temporary removed from trends!
  //KGA 24/2/98 this must be fixed: eg when deleting a tie with attached
  //pipes, the call to ConnectTag(...) which calls EO_ReadTaggedItem() 
  //causes a crash!

  /*
  const int len = strlen(pDelTag);
  //go thru all tags, if the deleted tag is found then update all
  for (int i=0; i<NoSlots(); i++)
    if (DS[i].TagValid() && (DS[i].sTag.Len()>=len))
      {
      if (_strnicmp(pDelTag, DS[i].sTag(), len)==0 && (DS[i].sTag.Len()==len || DS[i].sTag[len]=='.' || DS[i].sTag[len]==' '))
        {
        DS[i].ConnectTag(this, TRUE);
        RetCode = EODT_DONE;
        }
      }
  if (RetCode==EODT_DONE)
    {
    UpdateAllViewsByMsg(NULL, TGU_Toggle, NULL);
    XBuildMyDataLists();
    }
  */
  return RetCode;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd) 
  {
  return True;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_Start(CXM_TimeControl &CB)                      
  {
  iExecCnt = 0;
  UpdateAllViewsByMsg(NULL, TGU_Start, NULL, gs_TheRunMngr.AutomationBusy());
  iFirstChgdFld = -1;
  BuildFunctions();
  //UpdateAllViewsByMsg(NULL, -1);
  bJustLoaded = 0;
  return True;
  }

//---------------------------------------------------------------------------

void CTagVwDoc::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  CXM_ReadIndexedData* pMsg = XM.ReadIndexedData();
  flag ReadAll = pMsg->ReadAll;
  long DataIndex = pMsg->LastIndex+1;
  if (pMsg->Start)
    {
    DataIndex = 0;
    m_iFnctSubsStartIndex=NoSlots();
    }

  //Strng WrkTag, WrkCnvTxt;
  XM.Clear();
  for (     ; DataIndex<NoSlots(); DataIndex++)
    if (DS[DataIndex].TagValid() && (DS[DataIndex].sTag.Length()>0))
      {
      //TaggedObject::SplitTagCnv(DS[DataIndex].sTag(), WrkTag, WrkCnvTxt);

      //CXM_DataRequest DRqst(DataIndex, WrkTag(), TABOpt_AllInfoOnce, XIO_In);
      //KGA 18/7/96 : changed from above line to that seen below:
      //            : Trend windows did not refresh measured values properly!
      //            : Is this the correct fix???
      //            : eg had a tag in driver, PGM and trend  --> DID NOT WORK !!
      CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex, DS[DataIndex].sTagOnly(), TABOpt_AllInfoOnce|TABOpt_XRefStatus, XIO_InOut);
      if (!XM.PackMsg(DRqst))
        return;
      }

  #if ALLOWFUNCS
  if (FnMngrPresent())
    {
    CXRefArray & XRefs = FnMngr().XRefs(XR_Far);
    if (DataIndex<m_iFnctSubsStartIndex)
      DataIndex = m_iFnctSubsStartIndex;
    for ( ; DataIndex-m_iFnctSubsStartIndex<XRefs.GetSize(); DataIndex++)
      {
      CXM_DataRequest DRqst(DataIndex, XRefs[DataIndex-m_iFnctSubsStartIndex]->RefTag(), TABOpt_AllInfoOnce, XIO_InOut);
        //(XRefs[DataIndex-TrndSubsStartIndex].pVar->m_flags & VarTagOut) ? XIO_InOut : XIO_In);
      if (!XM.PackMsg(DRqst))
        break;
      }
    }
  #endif
  }

//---------------------------------------------------------------------------

void CTagVwDoc::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)                      
  {
  //XM.Clear();
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_TagsNotAvail(CXMsgLst &XM)
  {
  while (XM.MsgAvail())
    {
    CXM_TagNotAvail* pX = XM.TagNotAvail();
    Strng DocTitle((const char*)(GetTitle()));
    LogWarning(DocTitle(), 0, "Tag '%s' not found.", pX->Tag);
    }
  XM.Clear();
  return True;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_ReadSubsData(CXMsgLst &XM)                      
  {
  flag DataRead=0;
  return DataRead;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)                      
  {
  //iFirstChgdFld=-1;
  int iChgdFld=iFirstChgdFld;
  while (XM.MsgAvail())
    {
    CXM_ObjectData* pX=XM.ObjectData();
    CPkDataItem *pItem=pX->FirstItem();
    if (IsData(pItem->Type()))
      {
      int iIndex=pX->Index;
      if (iIndex < NoSlots())
        {
        if (DS[iIndex].bTagValid)
          {
          CTagVwSlot& Slt = DS[iIndex];
          Slt.cType=pItem->Type(); // update type in case it changes
          if (IsNumData(Slt.cType))
            {
            Slt.bValueValid = 1;
            if (IsFloatData(Slt.cType))
              {
              Slt.dValue=pItem->Value()->GetDouble(Slt.Cnv.Index(), Slt.Cnv.Text());
              Slt.Fmt.FormatFloat(Slt.dValue, Slt.sValue, (Slt.sStrList.Length()>0?&Slt.sStrList:NULL));
              }
            else if (Slt.sStrList.Length()>0)
              {
              long i=pItem->Value()->GetLong(Slt.Cnv.Index(), Slt.Cnv.Text());
              pStrng p=Slt.sStrList.AtIndexVal(i);
              if (p)
                Slt.sValue.Set("%i:%s", i, p->Str());
              else
                Slt.sValue.Set("%i", i);
              }
            else
              {
              if (IsIntData(Slt.cType))
                Slt.dValue=pItem->Value()->GetLong();
              Slt.sValue = pItem->Value()->GetString("%i", "%g", Slt.Cnv.Index(), Slt.Cnv.Text());
              }
            }
          else if (IsData(Slt.cType))
            {
            Slt.bValueValid = 1;
            Slt.sValue = pItem->Value()->GetString("%i", "%g", Slt.Cnv.Index(), Slt.Cnv.Text());
            }

          if (pItem->Contains(PDI_Flags))
            {
            Slt.m_bEditableStopped=((pItem->Flags() & isParm)==isParm);
            Slt.m_bEditableRunning=((pItem->Flags() & isParmStopped)==isParm);
            Slt.bIsResult = ((pItem->Flags() & isResult)!=0);
            Slt.bDriverTag = ((pItem->Flags() & isDriverTag)!=0);
            Slt.bArchiveTag = ((pItem->Flags() & isArchiveTag)!=0);
            }
          
          if (pItem->Contains(PDI_XRefStatus))
            Slt.m_XRefStatus = pItem->GetTagRefStatus();
          
          if (0)
            {
            dbgp("TrendSubsData:");
            if (pItem->Contains(PDI_XRefStatus))
              {
              dbgp(" %04x", Slt.m_XRefStatus);
              }
            else
              dbgp("     ");
            dbgpln(" %s", Slt.sTag());
            }

          if (iFirstChgdFld<0)
            iFirstChgdFld=iIndex;
          else
            DS[iChgdFld].iNextChgdFld=iIndex;
          iChgdFld=iIndex;
          DS[iChgdFld].iNextChgdFld=-1;
          }
        }
      else if (iIndex>=m_iFnctSubsStartIndex)
        {
        #if ALLOWFUNCS
        if (FnMngrPresent())
          {
          CXRefArray & XRefs = FnMngr().XRefs(XR_Far);
          if (iIndex-m_iFnctSubsStartIndex< XRefs.GetSize())
            XRefs[iIndex-m_iFnctSubsStartIndex]->SetXRefValue(pItem->Value()->GetDouble(), true);
          }
        #endif
        }
      }
    }
  if (LastBlock)
    {
    TryScrollWindow();
   
    Lock();
    for (int i=0; i<NoTrends(); i++)
      if (Trend[i].Connected())
        {
        Trend[i].AppendMeasured(gs_Exec.TheTime);
        Trend[i].Execute();
        }
    Free();

    //// Update Calculations!
    //#if ALLOWFUNCS
    //if (FnMngrPresent())
    //  EvaluateFunctions();
    //#endif

    UpdateAllViewsByMsg(NULL, TGU_UpdateAll, NULL, gs_TheRunMngr.AutomationBusy());
    }
  return True;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet) 
  {
  if (iExecCnt<10)
    iExecCnt++;
  iFirstChgdFld = -1;
  #if ALLOWFUNCS
  if (FnMngrPresent())
    EvaluateFunctions();
  #endif
  
  #if WITHAUTOSCALE 
  bool DoUpd=false;
  Lock();
  for (int i=0; i<NoTrends(); i++)
    {
    CTrendSlot &T=Trend[i];
    CTagVwSlot *pSlot=Trend[i].pSlt;
    if (T.Connected() && pSlot->bAutoScaleOn && Valid(pSlot->dValueLowest))
      {
      if (pSlot->dValueLowest<T.Scl.YMn ||
          pSlot->dValueHighest>T.Scl.YMx)
        {
        bool KeepMn=pSlot->dValueLowest >= T.Scl.YMn;
        bool KeepMx=pSlot->dValueHighest <= T.Scl.YMx;
        double Mn=Min(pSlot->dValueLowest, T.Scl.YMn);
        double Mx=Max(pSlot->dValueHighest,T.Scl.YMx);
        pSlot->DoFit(pSlot->bScaleApplied, Mn, Mx, KeepMn, KeepMx);
        pSlot->SetRange(Mn, Mx);
        pSlot->UpdateYMnMx(this);
        DoUpd=true;
        }
      }
    }
  Free();

  if (DoUpd)
    {
    UpdateAllViewsByMsg(NULL, TGU_UpdateMinMax, NULL);
    UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_UpdateAll, NULL);
    }
  #endif
  return False;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_Stop(CXM_TimeControl &CB)                       
  {
  UpdateAllViewsByMsg(NULL, TGU_Stop, NULL);
  for (int i=0; i<NoSlots(); i++)
    DS[i].bValueValid=0;

  return True;
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_Starting(flag fBeginStarting)
  {
  if (!fBeginStarting)
    {
    }
  return True;
  };

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_Stopping(flag fBeginStopping)
  {
  if (!fBeginStopping)
    {
    //UpdateAllViewsByMsg(NULL, TGU_LoadComplete, NULL);
    //UpdateAllViewsByMsg(NULL, TGU_Stop, NULL);
    }
  return True;
  };

//---------------------------------------------------------------------------

flag CTagVwDoc::EO_RequestTagInfo(RequestTagInfoRec& Rqst, ReplyTagInfoRec& Info)
  {
  switch (Rqst.RQ_Type)
    {
    case RQ_Files:
      if (Rqst.bFirst)
        {
        Info.sData = (const char*)GetPathName();
        // Will always return the current Prj
        Info.sData.FnContract();
        return True;
        }
      break;
    }
  return False;
  }

//---------------------------------------------------------------------------

void CTagVwDoc::EO_GlblResultValidity(flag IsValid, flag IsAlwaysValid)
  {
  UpdateAllViewsByMsg(NULL, TGU_Repaint, NULL, TRUE);
  }

//---------------------------------------------------------------------------
// CTagVwDoc diagnostics

#ifdef _DEBUG
void CTagVwDoc::AssertValid() const
  {
  DocRoot::AssertValid();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::Dump(CDumpContext& dc) const
  {
  DocRoot::Dump(dc);
  }

#endif //_DEBUG

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendOptions() 
  {
  TrendOptionsDlg();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::TrendOptionsDlg()
  {
  CString s = GetTitle();
  if (s.Find('.')>=0)
    s = s.Left(s.Find('.'));
  CTrndDlg Dlg((char*)(const char*)s, AfxGetMainWnd());
  Dlg.DoModal();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendGetquery() 
  {
  TrendQueryDlg();
  }

//---------------------------------------------------------------------------

void CTagVwDoc::TrendQueryDlg()
  {
  if (!gs_License.Blocked())
    {
    CTimeValue ST=TB.StartTime, ET=TB.EndTime;
    CQueryDlg* pDlg = new CQueryDlg(pTrnd, this, ST, ET);
    for (int i=0; i<NoTrends(); i++)
      if (Trend[i].Connected())
        pDlg->AddTag(Trend[i].pSlt->sTag());
    }
  }

//---------------------------------------------------------------------------

void CTagVwDoc::OnTrendSlots()
  {
  TrendSlotsDlg("");
  }

//---------------------------------------------------------------------------

void CTagVwDoc::TrendSlotsDlg(char* pTag)
  {
  /*CTrndInfoDlg Dlg(this, pTag);
  Dlg.DoModal();
  if (Dlg.bDidChange)
    {
    if (Dlg.bDidTagChange)
      {
      Rebuild();
      }
    else
      {
      Lock();
      UpdateAllViewsByMsg(NULL, TGU_ClearTrendDisplay|TGU_Toggle, NULL);
      FixTimeBase();
      Free();
      }
    }*/


  /*CXM_Route HRoute;
  flag bHistOn = (XFindObject(pExecName_Historian, HRoute));
  flag bHistExists = 0;

  if (bHistOn)
    {
    Strng sTg(pTag);
    if (sTg.Len()>0)
      {
      Strng WrkTag, WrkCnvTxt;
      TaggedObject::SplitTagCnv(sTg(), WrkTag, WrkCnvTxt);
      CXM_HistoryExists xb(WrkTag());
      CXMsgLst XM;
      XM.PackMsg(xb);
      DWORD RetCode = XSendMessage(XM, HRoute);
      bHistExists = ((RetCode & RC_SM_HistExists)!=0);
      //bRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
      if (bHistExists)
        {
        CXM_HistorySlotDlg xb(WrkTag(), 1);
        CXMsgLst XM;
        XM.PackMsg(xb);
        XSendMessage(XM, HRoute);
        }
      }
    else
      {

      }
    }*/
  }

//---------------------------------------------------------------------------

void CTagVwDoc::CreateScaleMenu(CMenu &Menu, CTagVwSlot* pSlot)
  {
//  char* Tag = pSlot->sTag();
//        char* Desc = Trend.pSlt->sDesc();
//        double YMn = Trend.Scl.YMn;
//        double YMx = Trend.Scl.YMx;
//        double Val = YMn+((YMx-YMn)*ClsTrnd.XY.y)/TrendYMax;
  CMenu ScaleMenu;
  ScaleMenu.CreatePopupMenu();
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL0, "10%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL1, "20%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL2, "50%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL3, "80%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL4, "100%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL5, "125%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL6, "200%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL7, "500%");
  ScaleMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_SCL8, "1000%");

  CMenu MoveMenu;
  MoveMenu.CreatePopupMenu();
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS0, "90%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS1, "80%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS2, "70%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS3, "60%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS4, "Centre");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS5, "40%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS6, "30%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS7, "20%");
  MoveMenu.AppendMenu(MF_STRING, IDM_TRNDSCL_POS8, "10%");

  /*if (pSlot)
    {
    Menu.AppendMenu(MF_STRING, IDM_TRNDSCL_TAG, pSlot->sTag());
    Menu.AppendMenu(MF_SEPARATOR);
    }*/

  CMenu FitMenu;
  FitMenu.CreatePopupMenu();
  CreateFitMenu(FitMenu, false, false);

  //Menu.AppendMenu(MF_STRING, IDM_TRNDSCL_FIT, "&Fit");
  Menu.AppendMenu(MF_POPUP, (unsigned int)FitMenu.m_hMenu, "&Fit");
  Menu.AppendMenu(MF_POPUP, (unsigned int)ScaleMenu.m_hMenu, "&Scale");
  Menu.AppendMenu(MF_POPUP, (unsigned int)MoveMenu.m_hMenu, "&Position");
#if WITHAUTOSCALE 
  Menu.AppendMenu(MF_SEPARATOR);
  Menu.AppendMenu(MF_STRING, IDM_TRNDSCL_FITAUTOON , "Auto On");
  Menu.AppendMenu(MF_STRING, IDM_TRNDSCL_FITAUTOOFF, "Auto Off");
  if (pSlot)
    {
    if (pSlot->bAutoScaleOn)
      Menu.CheckMenuItem(IDM_TRNDSCL_FITAUTOON, MF_BYCOMMAND|MF_CHECKED);
    else
      Menu.CheckMenuItem(IDM_TRNDSCL_FITAUTOOFF, MF_BYCOMMAND|MF_CHECKED);
    }
#endif
  }

//---------------------------------------------------------------------------

void CTagVwDoc::CreateFitMenu(CMenu &Menu, flag ForAll, flag WithAutoOnOff)
  {
//  Menu.CreatePopupMenu();
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITMIN     : IDM_TRNDSCL_FITMIN    , "Range 100%");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FIT1       : IDM_TRNDSCL_FIT1      , "Range  99%");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FIT10      : IDM_TRNDSCL_FIT10     , "Range  90%");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FIT50      : IDM_TRNDSCL_FIT50     , "Range  50%");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITZMIN    : IDM_TRNDSCL_FITZMIN   , "Range 100% + Zero");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITZ1      : IDM_TRNDSCL_FITZ1     , "Range  99% + Zero");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITZ10     : IDM_TRNDSCL_FITZ10    , "Range  90% + Zero");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITZ50     : IDM_TRNDSCL_FITZ50    , "Range  50% + Zero");
  Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITDEFAULT : IDM_TRNDSCL_FITDEFAULT, "Default");
#if WITHAUTOSCALE 
  if (WithAutoOnOff)
    {
    Menu.AppendMenu(MF_SEPARATOR);
    Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITAUTOON  : IDM_TRNDSCL_FITAUTOON , "Auto On");
    Menu.AppendMenu(MF_STRING, ForAll ? IDM_TRNDSCLALL_FITAUTOOFF : IDM_TRNDSCL_FITAUTOOFF, "Auto Off");
    }
#endif
  }

//---------------------------------------------------------------------------

flag CTagVwDoc::ProcessScaleMenu(int RetCd, CTagVwSlot* pSlot, double Val)
  {
  flag OK=false;
  flag DoScl=false;
  flag ClrScl=false;

  switch (RetCd)
    {
    case IDM_TRNDSCL_TAG:  
      {
      OK=true; 
//      DoScl=false;
      Strng AccTg(pSlot->sTag());
      const int DotPos = AccTg.Find('.');
      if (DotPos>=0)
        AccTg = AccTg.Left(DotPos);
      char* pTxt = new char[AccTg.Len()+1];
      strcpy(pTxt, AccTg());
      AfxGetMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS_NOERRDLG, (LPARAM)pTxt);
      break;
      }
    default:
      {
      
      double Mn=atof(pSlot->sMinVal() ? pSlot->sMinVal() : "0");
      double Mx=atof(pSlot->sMaxVal() ? pSlot->sMaxVal() : "1");
      double Mid=0.5*(Mx+Mn);
      double Diff=(Mx-Mn);
      double HDiff=0.5*Diff;

      int i;
      double Frac;

      static const double MvC[]=
        { 0.10, 0.20, 0.50, 0.80, 1.00, 1.25, 2.00, 5.00, 10.00 }; 

      if (RetCd>=IDM_TRNDSCL_FITMIN && RetCd<IDM_TRNDSCL_FITAUTOOFF ||
          RetCd>=IDM_TRNDSCLALL_FITMIN && RetCd<IDM_TRNDSCLALL_FITAUTOOFF)
        {
        if (Valid(pSlot->dValueLowest))
          {
          Mn=pSlot->dValueLowest;
          Mx=pSlot->dValueHighest;
          switch (RetCd)
            {
            case IDM_TRNDSCL_FITMIN:
            case IDM_TRNDSCLALL_FITMIN:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_FitMin, Mn, Mx);
              break;
            case IDM_TRNDSCL_FIT1:
            case IDM_TRNDSCLALL_FIT1:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_Fit1, Mn, Mx);
              break;
            case IDM_TRNDSCL_FIT10:
            case IDM_TRNDSCLALL_FIT10:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_Fit10, Mn, Mx);
              break;
            case IDM_TRNDSCL_FIT50:
            case IDM_TRNDSCLALL_FIT50:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_Fit50, Mn, Mx);
              break;
            case IDM_TRNDSCL_FITZMIN:
            case IDM_TRNDSCLALL_FITZMIN:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_FitZMin, Mn, Mx);
              break;
            case IDM_TRNDSCL_FITZ1:
            case IDM_TRNDSCLALL_FITZ1:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_FitZ1, Mn, Mx);
              break;
            case IDM_TRNDSCL_FITZ10:
            case IDM_TRNDSCLALL_FITZ10:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_FitZ10, Mn, Mx);
              break;
            case IDM_TRNDSCL_FITZ50:
            case IDM_TRNDSCLALL_FITZ50:
              pSlot->DoFit(pSlot->bScaleApplied=TAS_FitZ50, Mn, Mx);
              break;
            case IDM_TRNDSCL_FITDEFAULT:
            case IDM_TRNDSCLALL_FITDEFAULT:
              ClrScl=true;
              break;
            case IDM_TRNDSCL_FITAUTOON:
            case IDM_TRNDSCLALL_FITAUTOON:
              pSlot->bAutoScaleOn=true;
              if (pSlot->bScaleApplied==TAS_Off)
                pSlot->bScaleApplied=TAS_FitZ10;
              break;
            case IDM_TRNDSCL_FITAUTOOFF:
            case IDM_TRNDSCLALL_FITAUTOOFF:
              pSlot->bAutoScaleOn=false;
              break;
            }

          OK=true;
          DoScl=true;
          }
        }
      else if (RetCd==IDM_TRNDSCL_FITAUTOOFF || RetCd==IDM_TRNDSCLALL_FITAUTOOFF)
        {
        pSlot->bAutoScaleOn=false;
        OK=true;
        //DoScl=true;
        }
      else if (RetCd==IDM_TRNDSCL_FITAUTOON || RetCd==IDM_TRNDSCLALL_FITAUTOON)
        {
        pSlot->bAutoScaleOn=true;
        if (pSlot->bScaleApplied==TAS_Off)
          pSlot->bScaleApplied=TAS_FitZ10;
        OK=true;
        //DoScl=true;
        }
      else if (RetCd>=IDM_TRNDSCL_SCL0 && RetCd<=IDM_TRNDSCL_SCL9)
        {
        OK=true; 
        DoScl=true;
        i=RetCd-IDM_TRNDSCL_SCL0;
        Mn=Val-(Val-Mn)/MvC[i];
        Mx=Val+(Mx-Val)/MvC[i];
        }
      else if (RetCd>=IDM_TRNDSCL_POS0 && RetCd<=IDM_TRNDSCL_POS9)
        {
        OK=true;
        DoScl=true;
        Frac=0.9-(RetCd-IDM_TRNDSCL_POS0)*0.1;
        Mn=Val-Frac*Diff; 
        Mx=Val+(1.0-Frac)*Diff; 
        }
      
      if (ClrScl)
        {
        pSlot->sMinVal="";
        pSlot->sMaxVal="";
        pSlot->ConnectTag(this, CT_GetRecorded);
        }
      else if (DoScl)
        {
        pSlot->SetRange(Mn, Mx);
        }
      }
      break;
    }      

  return OK;
  }

//---------------------------------------------------------------------------

void CTagVwDoc::HiLiteTrend(int Slt2HiLite)
  {
  if (!gs_pPrj->bFlashTrend)
    return;

  if (Slt2HiLite==iSltHiLite)
    return;

  CTagVwTrend* pTrend = (CTagVwTrend*)pTrnd;
  if (iSltHiLite>=0 && fSltHiLiteOn && iSltHiLite<NoSlots())
    {
    CTagVwSlot & Slt=DS[iSltHiLite];
    pTrend->HiLiteTrend(Slt.nTrendNo, false);
    fSltHiLiteOn=false;
    }
  if (Slt2HiLite>=0 && !fSltHiLiteOn && Slt2HiLite<NoSlots())
    {
    CTagVwSlot & Slt=DS[Slt2HiLite];
    pTrend->HiLiteTrend(Slt.nTrendNo, true);
    fSltHiLiteOn=true;
    }
  iSltHiLite=Slt2HiLite;
  };

//---------------------------------------------------------------------------

void CTagVwDoc::BlinkTrend()
  {
  if (!gs_pPrj->bFlashTrend)
    return;

  if (iSltHiLite<0 || iSltHiLite>=NoSlots())
    return;

  CTagVwTrend* pTrend = (CTagVwTrend*)pTrnd;
  if (fSltHiLiteOn)
    {
    CTagVwSlot & Slt=DS[iSltHiLite];
    pTrend->HiLiteTrend(Slt.nTrendNo, false);
    fSltHiLiteOn=false;
    }
  else
    {
    CTagVwSlot & Slt=DS[iSltHiLite];
    pTrend->HiLiteTrend(Slt.nTrendNo, true);
    fSltHiLiteOn=true;
    }
  };

//---------------------------------------------------------------------------

flag CTagVwDoc::AddFunct(char* pName, CTagVwSlot* pSlt)
  {
  return True;
  /*if (pFMngr==NULL)
    {
    pFMngr = new CFnMngr    ;
    FnMngr().Reset(this);
    }
  flag b = FnMngr().AddFunct(pName, pSlt->sTag());
  return b;*/
  }

//--------------------------------------------------------------------------

void CTagVwDoc::BuildFunctions()
  {
  #if ALLOWFUNCS
  bFunctBuildRqd = 0;
  Lock();//need to lock while building pgm to prevent execution of PGM while compiling PGM
  bBusyBuildingPGM = 1;
  FnMngrClear();
  for (int i=0; i<NoSlots(); i++)
    if (DS[i].bHasFunct)//DS[i].sTag.Len()>0 && DS[i].sTag[0]=='=')
      {
      CTagVwSlot& Slt = DS[i];
      char FunctName[16];
      sprintf(FunctName, "F%d", i);
      FnMngrRequired();
      flag b = FnMngr().AddFunct(FunctName, Slt.sTag(), NULL);
      if (b)
        {
        Slt.bFunctValid = 1;
        Slt.pFunctVar = FnMngr().GetFunctVar(FunctName);
        }
      else
        {
        bFunctBuildRqd = 1;
        Slt.sValue = FnMngr().m_StateLine[0]();
        Slt.sMinVal = "";
        Slt.sMaxVal = "";
        Slt.sDesc = FnMngr().m_StateLine[1]();
        char Buff[64];
        sprintf(Buff, "PGM %s", FunctName);
        Strng s;
        s.Set("%s : %s", FnMngr().m_StateLine[0](), FnMngr().m_StateLine[1]());
        LogWarning(Buff, 0, s());
        LogNote(Buff, 0, "%s", FnMngr().m_StateLine[2]());
        LogNote(Buff, 0, "%s", FnMngr().m_StateLine[3]());
        Slt.bFunctValid = 0;
        }
      }
  CXRefBuildResults Results;
  FnMngrTryUpdateXRefLists(Results);
  bBusyBuildingPGM = 0;
  Free();
  if (FnMngrPresent())
    {
    if (Results.TotalErrors()>0)
      LogWarning((char*)(const char*)GetTitle(), 0, "%i bad external reference%s", Results.TotalErrors(), (Results.TotalErrors()>1 ? "s" : ""));
    int nBadCnvs = 0;//FnMngr().CheckCnvs();
    if (nBadCnvs>0)
      LogWarning((char*)(const char*)GetTitle(), 0, "%i engineering unit%s invalid", nBadCnvs, (nBadCnvs>1 ? "s are" : " is"));
    UpdateAllViewsByMsg(NULL, TGU_TagView, NULL);
    }
  #endif
  }

//--------------------------------------------------------------------------

void CTagVwDoc::EvaluateFunctions()
  {
  #if ALLOWFUNCS
  if (FnMngrPresent())
    {
    //if (bAboutToStart && SolveDirectMethod())
    //  PgmMngr.InitPBConvergence();
    //FnMngr().Execute(this, 0.0/*ICGetTime()*/, 0.0/*IC.GetTimeInc()*/, DoXStop, NULL, NULL, 0/*bAboutToStart*/, gs_Exec.GetSolveMode(), 0);

    Lock();//need to lock while executing PGM to prevent compiling of PGM

    CGExecContext ECtx(this);
    FnMngr().Execute(ECtx);
    if (ECtx.DoXStop)
      {
      LogError((char*)(const char*)GetTitle(), 0, "SysCAD stopped by function");
      XStop();
      }
    if (ECtx.DoXIdle)
      {
      LogError((char*)(const char*)GetTitle(), 0, "SysCAD paused by function");
      XIdle();
      }

    int iChgdFld = iFirstChgdFld;
    flag Chng = 0;
    for (int iIndex=0; iIndex<NoSlots(); iIndex++)
      {
      if (DS[iIndex].sTag.Len()>0 && DS[iIndex].sTag[0]=='=')
        {
        CTagVwSlot& Slt = DS[iIndex];
        flag ValChng = (iExecCnt<4);
        if (Slt.bFunctValid)
          {
          double d = Slt.pFunctVar->getD();
          ValChng |= ( !Slt.bValueValid || (Valid(Slt.dValue)!=Valid(d)) || (Valid(Slt.dValue) && Valid(d) && Slt.dValue!=d) );
          if (ValChng)
            {
            Slt.bValueValid = 1;
            Slt.dValue = d;
            Slt.Fmt.FormatFloat(Slt.dValue, Slt.sValue);
            }
          }
        else
          Slt.sValue = "?";
        if (ValChng)
          {
          if (iFirstChgdFld<0)
            iFirstChgdFld = iIndex;
          else
            DS[iChgdFld].iNextChgdFld = iIndex;
          iChgdFld = iIndex;
          Slt.iNextChgdFld = -1;
          Chng = 1;
          }
        }
      }
    Free();
    if (Chng)
      UpdateAllViewsByMsg(NULL, TGU_UpdateAll, NULL);
    }
  #endif
  }

//--------------------------------------------------------------------------

/*flag CTagVwDoc::PreStartCheck()
  {
  if (bOn)
    {
    int nBadExtRefs = PgmMngr.UpdateXRefLists();
    if (nBadExtRefs>0)
      {
      LogError("GenCon", 0, "%s: %i external reference%s not found", Tag(), nBadExtRefs, (nBadExtRefs>1 ? "s" : ""));
      return False;
      }
    int nBadCnvs = PgmMngr.CheckCnvs();
    if (nBadCnvs>0)
      LogError("GenCon", 0, "%s: %i engineering unit%s invalid", Tag(), nBadCnvs, (nBadCnvs>1 ? "s are" : " is"));
    bAboutToStart = 1;
    }
  return True;
  }*/

//--------------------------------------------------------------------------

int CTagVwDoc::ResolveNearXRef(CXRefItem * XRef, flag MustBeParm)
  {//variables that are owned by this executive object. (Trends have none!)
  return FXR_NotFound;
  }

//---------------------------------------------------------------------------

int CTagVwDoc::ResolveFarXRef(CXRefItem * XRef)
  {//variables external to trend doc (ie all tags!)
  int RetCode = XRef->ResolveFarXRef(this, XRef->RefTag(), NULL);
  //if ((RetCode & FXR_NotFound)==0)
  //  XRef->SetRefCnvIndex(XRef->m_iCnvIndex);
  return RetCode;
  }

//---------------------------------------------------------------------------

int CTagVwDoc::ResolveIOXRef(CXRefItem * XRef)
  {
  return FXR_NotFound;
  }

//--------------------------------------------------------------------------

int CTagVwDoc::UpdateXRefLists(CXRefBuildResults & Results) 
  {
  //if (bOn)
  FnMngrTryUpdateXRefLists(Results);
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CTagVwDoc::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();

  CNodeXRefMngr::UnlinkAllXRefs();

  };

//--------------------------------------------------------------------------

char* CTagVwDoc::GetOwnerTag() 
  { 
  return (char*)(const char*)GetTitle();
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CTagVwDocC, CTagVwDoc);

BEGIN_MESSAGE_MAP(CTagVwDocC, CTagVwDoc)
  //{{AFX_MSG_MAP(CTagVwDocC)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CTagVwDocC::CTagVwDocC()://int NSlots_) :
  CTagVwDoc()//NSlots_) 
  {
  m_iLoopSlotOne=0;
  m_nLoopSlotCount=0;
  m_iViewControl=0;
  //m_nCtrls=0;
  m_Loops.SetSize(0,16);
  //LoadCtrls();
  }

//---------------------------------------------------------------------------

void CTagVwDocC::ConnectAllTags()
  {
  if (m_RootTag.Length()==0)
    CTagVwDoc::ConnectAllTags();
  else
    {
    CXM_Route Route;
    //CXMsgLst XM;
    BOOL OK = TRUE;

    CMap <char*, char*, int, int> TagMap;
    TagMap.InitHashTable(FindNextPrimeNumber((NoSlots()*140)/100)); //+40%
      
    for (int i=0; i<NoSlots();i ++)
      switch (DS[i].TestSlotType())
        {
        case STT_Funct:
          DS[i].SetSlotAsFunct(this);
          break;
        case STT_DataPending:
          TagMap.SetAt(DS[i].TagOnly(), i);
          //dbgpln("%3i %s", i, DS[i].Tag());
          break;
        default:
          DS[i].SetSlotAsEmpty(this);
          break;
        }

    CSArray ITags;
    ITags.SetSize(32);
    Strng BlockTag, ItemTag;
    int iLvl=-1;
    CXM_ObjectTag ObjTag(m_RootTag(), TABOpt_AllInfoOnce);
    CXM_ObjectData ObjData;
    if (XReadTaggedItem(ObjTag, ObjData, Route))
      {
      CPkDataIter Iter;
      for (CPkDataItem* pPItem=ObjData.List.FirstItem(Iter); pPItem; pPItem=ObjData.List.NextItem(Iter))
        {
        byte Type=pPItem->Type();
        if (IsBlockStart(Type))
          {
          if (++iLvl>0)
            ITags[iLvl]=ITags[iLvl-1]+"."+pPItem->TagStr();
          else
            ITags[iLvl]=pPItem->TagStr();
          //dbgpln("++      %s",ITags[iLvl]());
          }
        else if (IsBlockEnd(Type))
          {
          --iLvl;
          if (iLvl<0)
            break;
          //dbgpln("--      %s",ITags[iLvl]());
          }
        else if (IsData(Type))
          {
          ItemTag=ITags[iLvl]+"."+pPItem->TagStr();
          int iSlt=-1;
          if (TagMap.Lookup(ItemTag(), iSlt))
            DS[iSlt].SetSlotAsData(this, *pPItem);
          
          //dbgpln("    %3i %s", iSlt,ItemTag());
          }
        else 
          {
          ASSERT_ALWAYS(IsLayout(Type), "Unknown Item Type")
          }
        }
      }

    // Scan Other slots and try to connect
    for (i=0; i<NoSlots(); i++)
      if (DS[i].bSlotType==STT_DataPending)
        DS[i].ConnectTag(this, 0);//FALSE);

    GetAllRecordedTags();
    }
  }

//--------------------------------------------------------------------------

static void GetCnvTxt(char * Tag, Strng &CnvTxt)
  {
  Strng WrkTag, WrkCnvTxt;

  if (Tag)
    {
    TaggedObject::SplitTagCnv(Tag, WrkTag, WrkCnvTxt);
    CnvTxt.Concat(" (", WrkCnvTxt(), ")");
    }
  else 
    CnvTxt="";
  }

void CTagVwDocC::PreloadSlots()
  {
  m_iLoopSlotOne=0;
  // CArray <CCtrlTagInfo, CCtrlTagInfo&> Ctrls;
  for (int l=0; l<m_Loops.GetSize(); l++)
    m_Loops[l].m_InUse=false;
  //int nCtrls=0;
  CArray <CTagVwCtrlInfo, CTagVwCtrlInfo&> New;
  New.SetSize(0,64);

  // Collect new Tags & find those already in lst
//  CtrlSeqBlkIter CSBs(CtrlSeqBlk::List);
//  for (CtrlSeqBlk* pT=CSBs.First(); pT; pT=CSBs.Next())
  CtrlSequencer::LockCSBs();
  for (int i=0; i<CtrlSequencer::NCSBs(); i++)
    {    
    CtrlSeqBlk &T=CtrlSequencer::CSBlk(i);
    New.SetSize(New.GetSize()+1);
    int n=New.GetUpperBound();
    Strng XX=T.PIDTag();
    New[n].m_PIDTag=T.PIDTag();
    New[n].m_SptTag=T.SptTag();
    New[n].m_MeasTag=T.MeasTag();
    New[n].m_OutTag=T.OutTag();
    New[n].m_InUse=true;
    New[n].m_InLst=false;
    for (l=0; l<m_Loops.GetSize(); l++)
      if (m_Loops[l].m_PIDTag.XStrICmp(New[n].m_PIDTag())==0)
        {
        m_Loops[l].m_InUse=true;
        New[n].m_InLst=true;
        }
    }
  CtrlSequencer::UnlockCSBs();
  
  // Overwite or delete unused in m_Loops
  int nLoops=m_Loops.GetSize();
  int n=0;
  for (l=0; l<m_Loops.GetSize(); l++)
    if (!m_Loops[l].m_InUse)
      {
      while (n<New.GetSize() && New[n].m_InLst)
        n++;
      if (n<New.GetSize())
        {
        m_Loops[l]=New[n];
        m_Loops[l].m_InUse=true;
        n++;
        }
      else
        {
        for (int ll=l; ll<m_Loops.GetSize()-1; ll++)
          m_Loops[ll]=m_Loops[ll+1];
        //m_Loops.SetSize(Max(0, m_Loops.GetSize()-1));
        }
      }

  // Add new to Loops 
  while (n<New.GetSize())
    {
    if (!New[n].m_InLst)
      {
      int l=m_Loops.GetSize();
      m_Loops.SetSize(l+1);
      m_Loops[l]=New[n];
      m_Loops[l].m_InUse=true;
      m_Loops[l].m_InLst=true;
      }
    n++;
    }

  for (int iFirstOther=0; iFirstOther<NoSlots(); iFirstOther++)
    if (!DS[iFirstOther].bCtrlSlot)
      break;
  int nCtrlSlots=CtrlHeaderSlots+m_Loops.GetSize()*SlotsPerCtrl;
  int nOtherSlots=NoSlots()-iFirstOther;
  int nSlotDiff=nCtrlSlots-iFirstOther;
  if (nSlotDiff>0)
    {
    DS.ChangeSize(nSlotDiff);
    for (int i=NoSlots()-1; i>=iFirstOther+nSlotDiff; i--)
      DS[i]=DS[i-nSlotDiff];
    }
  else if (nSlotDiff<0)
    {
    for (int i=iFirstOther; i<NoSlots(); i++)
      DS[i+nSlotDiff]=DS[i];
    DS.ChangeSize(nSlotDiff);
    }

  int iSlot=0;
  Strng CnvTxt;
  char * pHeadTag="$Ctrl.";
  Strng LoopTag;
  if (m_Loops.GetSize())
    {
    m_iViewControl=Range(0, m_iViewControl, m_Loops.GetSize()-1);
    LoopTag=pHeadTag+m_Loops[m_iViewControl].m_PIDTag+".";

    DS.CheckSize(iSlot+CtrlHeaderSlots);

    DS[iSlot++].SetTag(pHeadTag, "SequencerActive");
    DS[iSlot++].SetTag(pHeadTag, "TuneWithHold");
    DS[iSlot++].SetTag(pHeadTag, "HoldAll");

    DS[iSlot++].SetTag(LoopTag(), "PlantGain");
    DS[iSlot++].SetTag(LoopTag(), "PlantDelay");
    DS[iSlot++].SetTag(LoopTag(), "PlantPeriod");
    DS[iSlot++].SetTag(LoopTag(), "PIDGain");
    DS[iSlot++].SetTag(LoopTag(), "PIDIc");
    DS[iSlot++].SetTag(LoopTag(), "PIDDc");

    DS[iSlot++].SetTag(pHeadTag, "MinSettleCount");
    DS[iSlot++].SetTag(pHeadTag, "MaxSettleCount");
    DS[iSlot++].SetTag(pHeadTag, "TestDelta");
    DS[iSlot++].SetTag(pHeadTag, "MaxPBDerate");


    m_iLoopSlotOne=iSlot;
    //m_iViewControl=0;
    } 
  else
    {
    m_iLoopSlotOne=0;
    m_iViewControl=0;
    }

  for (l=0; l<m_Loops.GetSize(); l++)
    {
    //T=pHeadTag+m_Loops[l].m_PIDTag;
    LoopTag=pHeadTag+m_Loops[l].m_PIDTag+".";
    
    DS.CheckSize(iSlot+4);

    DS[iSlot].SetTag(LoopTag(), "StatusWord");
    DS[iSlot].sDesc=m_Loops[l].m_PIDTag;
    iSlot++;

    GetCnvTxt(m_Loops[l].m_SptTag(), CnvTxt);
    DS[iSlot].SetTag(LoopTag(), "Spt", CnvTxt());
    DS[iSlot].sDesc=m_Loops[l].m_SptTag;
    iSlot++;

    GetCnvTxt(m_Loops[l].m_MeasTag(), CnvTxt);
    DS[iSlot].SetTag(LoopTag(), "Meas", CnvTxt());
    DS[iSlot].sDesc=m_Loops[l].m_MeasTag;
    iSlot++;

    GetCnvTxt(m_Loops[l].m_OutTag(), CnvTxt);
    DS[iSlot].SetTag(LoopTag(), "Out", CnvTxt());
    DS[iSlot].sDesc=m_Loops[l].m_OutTag;
    iSlot++;
    };
  m_nLoopSlotCount=iSlot-m_iLoopSlotOne;
  ASSERT_ALWAYS((m_Loops.GetSize()==0) || (nCtrlSlots==iSlot), "Bad CtrlTrend Setup");

  for (i=0; i<iSlot; i++)
    DS[i].bCtrlSlot=true;
  for ( ; i<NoSlots(); i++)
    DS[i].bCtrlSlot=false;

  m_RootTag=pHeadTag;
  };

//--------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
