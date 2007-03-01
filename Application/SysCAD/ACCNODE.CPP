//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#define  __ACCNODE_CPP

#include "sc_defs.h"
#include "limits.h"
#include "datacnvs.h"
#include "fixedit.h"
#include "cmdmenu.h"
#include "sfe_clnt.h"
#include "accnode.h"
#include "chngtag.h"
#include "copyblk.h"
#if WITHSPECSHEETS
//ss#include "specsht.h"
#endif
#include "selctwnd.h"
#include "scd_wm.h"
#include "project.h"
#include "syscad.h"
#include "mdlvalue.h"
#include "msgwnd.h"
#include "gpwfuncs.h"
#include "mdlrunmngr.h"
#include "apprunmngr.h"
#include "tagvtext.h"
#include "ordwnd.h"
#include "statswnd.h"
#include "wirepanel.h"
#include ".\accnode.h"
#include "helpinfo.h"
#include "sp_db.h"

#include "ole_base.h"
#include "oleexcel.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgDataReadWrite    0
#define dbgANW              0
#define dbgDumpObject       0
#define dbgBld              0        

#define CHECKBOXESRIGHT     0
#define BASICWIDTH          20 /*14*/
#define MATRIXWIDTH         10 /*14*/
#define SHORTFLDS           0  /*1*/

//static int DoTagOnly=0;

#define WithSendToCustom 0

const int MaxNdHistListSize = 48;

//===========================================================================

const int Id_FldParms    = 1;                // Leave Gap After This
const int Id_FldCnvs     = 1+1*MaxAccNdFlds; // Leave Gap After This
const int Id_FldBtns     = 1+2*MaxAccNdFlds; // Leave Gap After This
const int Id_TreeNdBtn   = 1+3*MaxAccNdFlds; // Leave Gap After This
//const int Id_ViewIndex   = 3*MaxAccNdFlds; // Leave Gap After This

const int LastFieldId    = 1+5*MaxAccNdFlds;

const int Id_SetBtn      = 1+5*MaxAccNdFlds+1;
const int Id_GetBtn      = 1+5*MaxAccNdFlds+2;
const int Id_CloseBtn    = 1+5*MaxAccNdFlds+3;
const int Id_PrintBtn    = 1+5*MaxAccNdFlds+4;
const int Id_AllBtn      = 1+5*MaxAccNdFlds+5;
const int Id_ChgTagBtn   = 1+5*MaxAccNdFlds+6;
const int Id_SpecShtBtn  = 1+5*MaxAccNdFlds+7;
const int Id_TagOrSymBtn = 1+5*MaxAccNdFlds+8;
const int Id_Options     = 1+5*MaxAccNdFlds+9;
const int Id_Next        = 1+5*MaxAccNdFlds+10;
const int Id_Prev        = 1+5*MaxAccNdFlds+11;
const int Id_GoTo        = 1+5*MaxAccNdFlds+12;
const int Id_ViewIndex   = 1+5*MaxAccNdFlds+13;
const int Id_FindBtn     = 1+5*MaxAccNdFlds+14;
const int Id_GoToBtn     = 1+5*MaxAccNdFlds+15;

const int MaxLinesOnPage = 40;
const int MinLinesOnPage = 20;

//===========================================================================

void AppendTagComment(Strng & T, Strng & Com)
  {
  if (Com.GetLength()>0)
    {
    switch (Com[0])
      {
      case '<':
      case '(':
      case '[':
      case '{':
        T+=Com;
        return;
      }
    }
  T+="(";
  T+=Com;
  T+=")";
  };

//---------------------------------------------------------------------------

flag FullFldName(Strng &Tag, pAccNdData Data, int FldNo)
  {
  //flag FirstInObject=1;
  Tag="";
  for (int i=0; i <= FldNo; i++) // startat 1 to skip  UnitTag
    {
    if (Data[i].iEndFld>0)
      if (Data[i].iEndFld>FldNo)
        {
        ASSERT(Data[i].fIsStruct);
        //if (FirstInObject)
        //  Tag="";
        //FirstInObject=(Data[i].iType==tt_Object);
        if (Data[i].iType==tt_Object || Data[i].iType==tt_Element)
          {
          Tag="";
          }
        else
          {
          if (Tag.IsNotEmpty())
            Tag+=".";
          Tag+=Data[i].sSymOrTag();
          }
        }
      else
        i=Data[i].iEndFld;
    }
  if (Data[FldNo].fIsStruct)
    {
    char * p=(Data[FldNo].iType==tt_Object || Data[FldNo].iType==tt_Element) ? Data[FldNo].sValue() : Data[FldNo].sSymOrTag();
    if (p)
      {
      if (Tag.IsNotEmpty())
        Tag+=".";
      Tag+=p;
      }
    }
  else
    {
    char * p=Data[FldNo].sSymOrTag();
    if (p)
      {
      if (Tag.IsNotEmpty())
        Tag+=".";
      Tag+=p;
      }
    }
  //ASSERT(Tag.Length()>0);
  Tag.Trim();
  return (Tag.IsNotEmpty());
  }

//---------------------------------------------------------------------------

void AccNdData::Init()
  {
  iType           = tt_NULL;
  iFlags          = 0;
  sSymOrTag       = "";
  sTagOnly        = "";
  sFullTag        = "";
  sRefTag         = "";
  sValue          = "";
  sTagComment     = "";
  sDesc           = "";
  iStrtFld        = -1;
  iEndFld         = -1;
  iLvl            = -1;
  dVal            = 0.0;
  //Fmt.Set("f2", 0, DefaultFltFormat, 'f');
  Fmt             = AccNodeWnd::GlblFmt;
  // Cnv
  iObjNo          = -1;
  fIsStrng        = false;
  fIsParam        = false;
  fKeepFldName    = false;
  fIsModified     = false;
  fIsData         = false;
  fIsStruct       = false;
  fHasNANOK       = false;
  fHasNANStr      = false;
#if WITHSPECSHEETS
//ss  fIsSpecSheet    = false;
#endif
  ObjOn           = false;
  fPageOn         = false;
  fIsEdtObj       = false;
  fIndexedStr     = false;
  fHasComment     = false;
  fIsStructTreeNd = false;
  fIsTreeNd       = false;
  fMultiStrng     = false;
  fUsed           = false;
  fHasCheck       = false;
  fHasButton      = false;
  fHasSetOnChange = false;
  pObjAttr        = NULL;
  fRecorded       = false;
  fArchived       = false;
  m_Markers       = 0;
  m_iChildLvl     = 0;
  m_fNxtIsChild   = 0;
  m_fIsRowStart   = 0;
  m_fIsRowsEnd    = 0;
  m_IsLastDataCol = 0;
  m_iMatrixCol    = -1;
  ValLst.Clear();   
  iViewIndex=-1;

  };

//---------------------------------------------------------------------------

void AccNdData::Term()
  {
  Init();
  };

//---------------------------------------------------------------------------

void AccNdData::FormatFloat()
  {
  Fmt.FormatFloat(dVal, sValue, fHasNANStr ? &ValLst:NULL);
  }

//---------------------------------------------------------------------------

void AccNdData::AdjTextValue(char * NewCnv, bool SaveOld)
  {
  if (IsFloatData(iType))
    {
    if ((Cnv.Index())>0)
      {   
      CDataCnv & C=*Cnvs[Cnv.Index()];
      if (Valid(dVal))
        dVal=C.Normal(dVal, Cnv.Text());
      if (NewCnv)
        sIndividualCnv=NewCnv;
      if (C.OverTxt())
        {
        if (SaveOld)
          sIndividualCnv=Cnv.Text();
        Cnv.SetText(C.OverTxt());
        }
      else
        Cnv.SetText(sIndividualCnv());
      if (Valid(dVal))
        dVal=C.Human(dVal, Cnv.Text());
      }
    //if (Valid(dVal))
    FormatFloat();
    }
  else if (IsStrng(iType) && IsTimeCnv(Cnv.Index()))
    {
    CDataCnv & C=*Cnvs[Cnv.Index()];
    CTimeValue V((LPCTSTR)sValue());
    if (NewCnv)
      sIndividualCnv=NewCnv;
    if (C.OverTxt())
      {
      if (SaveOld)
        sIndividualCnv=Cnv.Text();
      Cnv.SetText(C.OverTxt());
      }
    else
      Cnv.SetText(sIndividualCnv());
    sValue=V.Format(Cnv.Text());
    }
  }

//===========================================================================

void AccNdFld::Init()
  {
 	pData=NULL;
	iData=0;
	iType=0;
  sTagText="";
	iLvl=0;
	fIsData=false;
	fIsTreeNd=false;
	fIsTreeVis=false;
	fUseComment=false;
	fPadDash=false;
	iUseComment=0;
  ValFld=NULL;
  };

//---------------------------------------------------------------------------

void AccNdFld::Term()
  {
  //ValFld = NULL;
  //iObjNo=-1;
  Init();
  };

//---------------------------------------------------------------------------
//===========================================================================
// AccNodeWnd

const int MaxTagCmds=200; // NBNB See separ in resource.h

FmtAttribute AccNodeWnd::GlblFmt;

IMPLEMENT_DYNCREATE(AccNodeWnd, FxdEdtView)

AccNodeWnd::AccNodeWnd()
  {
  m_pMyFrm=NULL;
  NFlds=0;
  NData=0;
  NObjs=0;
  fLastSaveInvalid=0;
  fAllFldsVis=0;
  fGlobalFmtMenu=0;
  fKeepNdHist=0;
  iNdHistPos=-1;
  RBActiveFld=-1;

  for (int i=0; i<MaxAccNdFlds; i++)
    {
    Flds[i].Init();
    Data[i].Init();
    }
  EO_Register("AccessNode", EOExec_Msg, /*Pri*/0,/*Stack*/0);

  ViewVarBitmap.LoadBitmap(IDB_VARVIEWNOX);
  HiddenBitmap.LoadBitmap(IDB_HIDDENVAR);

  //GlblFmt.Set("f5", 0, 5, 'f');
  GlblFmt.Set("f2", 0, 2, 'f');

  m_hTTWnd=NULL;                 // handle to the ToolTip control
  m_bTTVisible=false;
  m_iPrevTTRow=-1;
  m_iPrevTTCol=-1;

  sLastSelected.SetSize(MaxTagCmds);
  sLastSelFullTag.SetSize(MaxTagCmds);
  }

//---------------------------------------------------------------------------

AccNodeWnd::~AccNodeWnd()
  {
  //TODO: Need to delete the stored attribute settings on project close!!!

  //ObjectAttributeIter Obj(ObjAttributes);
  //for (ObjectAttribute *p=Obj.First(); p; p=Obj.Next())
  //  delete p;

  EO_DeRegister();
  ClearFlds(false);

  //ObjectAttributeIter Obj(ObjAttributes);
  //for (ObjectAttribute *p=Obj.First(); p; p=Obj.Next())
  //  delete p;
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(AccNodeWnd, FxdEdtView)
  //{{AFX_MSG_MAP(AccNodeWnd)
  ON_WM_DESTROY()
  ON_WM_ACTIVATE()
  ON_WM_SHOWWINDOW()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_RBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
  //}}AFX_MSG_MAP
  ON_COMMAND_RANGE(IDM_ACC_CNV1, IDM_ACC_CNVN, OnAccCnv)
  ON_COMMAND_RANGE(IDM_ACC_FMT1, IDM_ACC_FMTN, OnAccFmt)
  ON_COMMAND_RANGE(IDM_ACC_HIDE, IDM_ACC_SHOW, OnAccShow)
  ON_COMMAND_RANGE(IDM_ACC_VIEW0, IDM_ACC_CUSTOM0, OnAccView)
  ON_COMMAND_RANGE(IDM_ACC_HIDEIFZERO, IDM_ACC_UPDATEONRUN, OnAccOptions)
  //ON_COMMAND_RANGE(IDM_ACC_ALLFIELDS, IDM_ACC_PRINT, OnAccOptions)
  ON_COMMAND_RANGE(IDM_ACC_COPYBLK_0, IDM_ACC_COPYBLK_19, OnAccOptions)
  ON_COMMAND_RANGE(IDM_ACC_PASTEBLK_0, IDM_ACC_PASTEBLK_19, OnAccOptions)
  ON_COMMAND_RANGE(IDM_ACC_SHOWGROUPS, IDM_ACC_SPAREOPTION9, OnAccOptions)
  ON_COMMAND(IDM_ACC_CUTTAG,     OnAccCutTag)
  ON_COMMAND(IDM_ACC_CUTVAL,     OnAccCutVal)
  ON_COMMAND(IDM_ACC_COPYTAG,    OnAccCopyTag)
  ON_COMMAND(IDM_ACC_COPYRHSTAG, OnAccCopyRHSTag)
  ON_COMMAND(IDM_ACC_COPYVAL,    OnAccCopyVal)
  ON_COMMAND(IDM_ACC_PASTE,      OnAccPaste)
  ON_COMMAND_RANGE(IDM_ACC_PBINITNODE, IDM_ACC_PRESETALL,  OnAccEmptyPreset)
  ON_COMMAND_RANGE(IDM_ACC_ACCESSTAG_0, IDM_ACC_BTNCOPYTAG_0, OnAccMenu)
  ON_COMMAND_RANGE(IDM_PRJ_ACCESS, IDM_PRJ_ACCESSLAST, OnAccMenu)
  ON_MESSAGE(WMU_ACCREFRESHSAVE, OnAccRefreshSave)
  ON_MESSAGE(WMU_ACCREFRESHCANCEL, OnAccRefreshCancel)
  ON_NOTIFY_EX_RANGE(TTN_GETDISPINFO, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()
 
//---------------------------------------------------------------------------

BOOL AccNodeWnd::PreCreateWindow( CREATESTRUCT& cs )
  {
  return FxdEdtView::PreCreateWindow(cs);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
  {
  //TRACE1("AccNodeWnd::OnActivateView %i\n", bActivate);

  gs_AccessWnds.MarkAsActive(this);

  FxdEdtView::OnActivateView(bActivate, pActivateView, pDeactiveView );
  EnableToolTips(TRUE);   // enable tool tips for view

  }

//---------------------------------------------------------------------------

void AccNodeWnd::SaveCurrentPageInfo()
  {
  if (ObjClassId.IsNotEmpty())
    {
    CProfINIFile PF(PrjIniFile());
    int p= CPgNo;
    int y= VScrlOn ? ScrGB.ChScroll.cy : -1;
    int x= HScrlOn ? ScrGB.ChScroll.cx : -1;
    int c= ChEditPos.x;
    int r= ChEditPos.y;
    Strng S;
    S.Set("%i,%i,%i,%i,%i",p,x,y,c,r);
    PF.WrStr("AccessPage", ObjClassId(), S());
    }
  }

//---------------------------------------------------------------------------

void AccNodeWnd::RestoreCurrentPageInfo()
  {
  if (ObjClassId.IsNotEmpty())
    {
    CProfINIFile PF(PrjIniFile());
    Strng S=PF.RdStr("AccessPage", ObjClassId(), "-1");
    int p=0;
    int y=0;
    int x=0;
    int c=0;
    int r=0;
    int n=(S.Length()>0) ? sscanf(S(), "%i,%i,%i,%i,%i",&p,&x,&y,&c,&r) : 0;

    if (n>=3)
      PositionScrollBarsAt(x, y);
    if (n>=5)
      PositionCaretAt(c, r);
    }
  }

//---------------------------------------------------------------------------

void AccNodeWnd::SetCurrentPage()
  {
  CProfINIFile PF(PrjIniFile());
  int PgNo=PF.RdInt("AccessPage", ObjClassId(), -1);
  if (PgNo>=0)
    SelectPage(PgNo, false);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::ShowDefWndTitle()
  {
  WndTitle.Set("Access - %s", NdTag()); 
  m_pMyFrm->SetWindowText(WndTitle()); 
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  FxdEdtView::OnDrawBack(GB, PgNo, ClipRgn);
  if (PgNo>=0 && ObjOnPage[PgNo]>=0)
    ObjEdit[ObjOnPage[PgNo]]->OnDrawBack(GB, PgNo, ClipRgn);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  FxdEdtView::OnOverDraw(GB, PgNo, ClipRgn);
  if (PgNo>=0 && ObjOnPage[PgNo]>=0)
    ObjEdit[ObjOnPage[PgNo]]->OnOverDraw(GB, PgNo, ClipRgn);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnDrawLine(rGDIBlk GB, int PgNo, int RowNo)
  {
  if (CRow(RowNo)->Visible(GB))
    {
    CRow(RowNo)->Write(GB);
    if (!GB.IsPrinting())
      {
      FxdEdtInfo EI;
      if (LocateFromCR(0, RowNo, EI))
        {
        //dbgpln("Draw Trace %i.3",RowNo);

        FxdEdtRow & Row = *CRow(EI.PgRowNo);
        FxdEdtFld* Fld = Row.FldFromId(Id_ViewIndex);
  //      if (Slt && Fld)// && (Slt->bRecorded || Slt->nTrendNo>=0))

        int fi=-1;
        for (int f=0; f<Row.NFields; f++)
          if (Row.Field[f]->FieldId>=Id_FldParms && Row.Field[f]->FieldId<=LastFieldId)
            {
            fi = (Row.Field[f]->FieldId-Id_FldParms) % MaxAccNdFlds;
            break;
            }

        if (Fld)// && (Slt->bRecorded || Slt->nTrendNo>=0))
          {
          int y1=Fld->Ys(GB)+1;
          int x2=Fld->Xs(GB);
          CDC &DC = GB.DC();
          CDCResChk ResChk(DC);

          CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);
          CBrush* OldBrush = NULL;
          COLORREF OldBk = DC.SetBkColor(GB.crGrfBack);
          //int Recx = x2+1;//iMarginWidth;
          //int Recy = y1+4;
          if (fi>=0 && Flds[fi].pData)
            {
            if (fAllFldsVis)
              {
              if (!Flds[fi].fIsTreeVis)
                {
                CDC memdc;
                memdc.CreateCompatibleDC(&DC);
                CDCResChk ResChk(memdc);
                CBitmap* poldbmp = memdc.SelectObject(&HiddenBitmap);
                DC.BitBlt(x2, y1+1, 8,8, &memdc, 0, 0, SRCCOPY);
                memdc.SelectObject(poldbmp);
                }
              x2 += 9;
              }
            if (Flds[fi].pData->iViewIndex>=0)
              {                                               
              CDC memdc;
              memdc.CreateCompatibleDC(&DC);
              CDCResChk ResChk(memdc);
              CBitmap* poldbmp = memdc.SelectObject(&ViewVarBitmap);
              DC.BitBlt(x2, y1, 6,6, &memdc, 0, 0, SRCCOPY);
              memdc.SelectObject(poldbmp);
              }
            x2 += 8;
            if (Flds[fi].pData->fRecorded && Flds[fi].pData->fArchived)
              {
              x2+=0;
              y1+=2;
              }
            else
              {
              x2+=2;
              y1+=4;
              }
            if (Flds[fi].pData->fRecorded)
              {
              CRect TrndR(x2, y1, x2+7, y1+7);
              CBrush Brush(RGB(255, 255, 0));
              if (OldBrush)
                DC.SelectObject(&Brush);
              else
                OldBrush = DC.SelectObject(&Brush);
              DC.Ellipse(TrndR);
              y1+=4;
              x2+=4;
              }
            if (Flds[fi].pData->fArchived)
              {
              CRect TrndR(x2, y1, x2+7, y1+7);
              CBrush Brush(RGB(0, 255, 255));
              if (OldBrush)
                DC.SelectObject(&Brush);
              else
                OldBrush = DC.SelectObject(&Brush);
              DC.Ellipse(TrndR);
              }
            }
          DC.SelectObject(OldPen);
          if (OldBrush)
            DC.SelectObject(OldBrush);
          DC.SetBkColor(OldBk);
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnInitialUpdate()
  {
  FxdEdtView::OnInitialUpdate();

  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint)
  {
  FxdEdtView::OnUpdate(pSender, lHint, pHint);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::StartBuild()
  {
  //NUsed=0;
  FxdEdtView::StartBuild();
  }

//---------------------------------------------------------------------------

void AccNodeWnd::BuildDone()
  {
  //NUsed=0;
  FxdEdtView::BuildDone();
  SetCurrentPage();
  RestoreCurrentPageInfo();
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::LoadAccessData()
  {
  CWaitCursor Wait;

  for (int i = 0; i < MaxAccNdFlds; i++)
    {
    Flds[i].Init();
    Data[i].Init();
    }

  #if WITHSPECSHEETS
  //ss  sSpecSheet="Standard";
  //ss  iSpecSheet=-1;
  #endif

  CXM_ObjectData ObjData(0);
  flag Ok = false;
  Strng TryNdTag;
  int iLastDot=-1;
  for (;;)
    {
    TryNdTag=NdTag;
    for (int i=iLastDot+1; i<NdTag.GetLength(); i++)
      {
      if (TryNdTag[i]=='.')
        {
        TryNdTag.SetLength(i);
        iLastDot=i;
        break;
        }
      }
    WndTitle.Set("Loading - %s", TryNdTag());
    m_pMyFrm->SetWindowText(WndTitle());
    
    TABOptions ForXtra=0;//(fAllFldsVis?TABOpt_ForFile|TABOpt_ForSnapShot|TABOpt_ForScenario:0);
    CXM_ObjectTag ObjTag(TryNdTag(), ForXtra|TABOpt_AllInfo|TABOpt_ForView|TABOpt_XRefStatus);
    ObjData.Clear();

    ObjRoute.Clear();
    Ok = (XReadTaggedItem(ObjTag, ObjData, ObjRoute)!=0);
    if (Ok || (TryNdTag.GetLength()==NdTag.GetLength()))
      break;
    }

    //LogNote("Acc", 0, "XRead %s", SW.MinSecHunDesc(sSWTmp));
  if (Ok)
    {
    NdTag=TryNdTag;
    fLastSaveInvalid=0;
    #if dbgDumpObject
    ObjRoute.dbgDump("Access : Load");
    ObjData.List.dbgDump(1, "Access : Load");
    #endif
    int iNo=0;
    int ObjLvl=0;
    int iStructLvl=0;
    pObjectAttribute pObjAttr[256];
    pObjAttr[ObjLvl]=NULL;

    ObjClassId="";

    int iMatrixCol=-1;
    CPkDataIter Iter;
    int ReportedMaxFieldsError = 0;
    flag FirstPage=1;
        
    //AccNdData * pPrevD = NULL;
    int iRowStartData=-1;
    //AccNdData * pPrevD = NULL;
    for (CPkDataItem* pPItem=ObjData.FirstItem(Iter); pPItem; )
      {
      int ItemAdvanced=false;
      DDEF_Flags Flags=pPItem->Flags();
      flag Vis=1;
      if (IsData(pPItem->Type()) || (pPItem->Type()==tt_Text))
        {
        Vis=((Flags & MODEVISIBLE)!=0 && (Flags & DDEF_NOVIEW)==0);
        if (Vis && (Flags & DDEF_HIDEIFZERO) && GetHideZeros() && (pPItem->Type()==tt_Double))
          {
          if (pPItem->Value()->Double==0.0)
            Vis=false;
          }
        }

      if (iNo>=MaxAccNdFlds)
        {
#ifndef _RELEASE
        if (ReportedMaxFieldsError==0)
          AfxMessageBox("Potential Critical Error!!!\nToo many visible Access Window fields!");
#endif
        ReportedMaxFieldsError++;
        }
      if (Vis && iNo<MaxAccNdFlds)
        {
        AccNdData & d=Data[iNo];

        d.fIsModified = 0;
        d.iFlags      = pPItem->Flags();
        d.iType       = pPItem->Type();

        d.sSymOrTag   = pPItem->SymOrTag();
        d.sTagOnly    = pPItem->TagStr();
        d.sTagComment = pPItem->TagComment();
        d.sDesc       = pPItem->Description();
        d.m_Markers   = pPItem->GetTagRefStatus();

        d.fHasNANOK   = IsFloatData(d.iType) && ((d.iFlags&NAN_OK)!=0);
        d.fHasNANStr=false;
        if (IsFloatData(d.iType) && d.fHasNANOK)
          {
          pPItem->GetStrList(d.ValLst);
          if (d.ValLst.Length()>0)
            d.fHasNANStr=true;
          }

        d.fIsStructTreeNd = 0;
        d.fIsTreeNd    = ((d.iFlags & DDEF_TREENODE)!=0);
        d.iLvl         = iStructLvl+(IsData(d.iType) ? 1 : 0);
        d.fIsStruct    = IsStructure(d.iType);
        d.fKeepFldName = 0;
        d.fMultiStrng  = 0;
        d.fHasCheck    = ((d.iFlags & DDEF_CHECKBOX)!=0);
        d.fHasButton   = ((d.iFlags & DDEF_BUTTON)!=0);
        d.fHasSetOnChange = ((d.iFlags & DDEF_SETONCHANGE)!=0);
        
        d.m_iChildLvl   = (d.iFlags & DDEF_CHILDMASK) >> DDEF_CHILDSHIFT;
        d.m_fNxtIsChild = 0;
        
        d.m_fIsRowStart = (d.iFlags & DDEF_STARTROW)!=0;
        d.m_fIsRowsEnd  = (d.iFlags & DDEF_ENDROWS)!=0;
        d.m_IsLastDataCol = 0;

        if (d.m_fIsRowStart)
          {
          iMatrixCol=0;
          iRowStartData=iNo;
          }
        if (iMatrixCol>=0)
          {
          d.m_iMatrixCol=iMatrixCol;
          if (IsData(d.iType)) 
            {
            iMatrixCol++;
            for (int i=iRowStartData; i<iNo; i++)
              Data[i].m_IsLastDataCol = 0;
            d.m_IsLastDataCol = 1;
            }
          }
        if (d.m_fIsRowsEnd)
          iMatrixCol=-1;

        if (d.m_iChildLvl && IsData(Data[iNo-1].iType) && Data[iNo-1].m_iChildLvl<d.m_iChildLvl)
          {
          Data[iNo-1].m_fNxtIsChild=1;
          //m_iChildLvl//d.fIsTreeNd=1;
          int xxx=0;
          }

        CCnvIndex iCnv = pPItem->CnvIndex();
        d.Cnv.SetIndex(iCnv);
        //// Set to primary Text if sec text supplied is BAD
        //if ((iCnv)>0 && Cnvs[iCnv]->Find(pPItem->CnvTxt())==NULL)
        //  {
        //  d.Cnv.SetText(Cnvs[iCnv]->Txt()); // Set to Primary Value
        //  if (pPItem->CnvTxt())
        //    LogWarning(d.sSymOrTag(), 0, "Unknown Conversion '%s' Set to '%s' for %s", pPItem->CnvTxt(), Cnvs[iCnv]->Txt(), d.sClsTag());
        //  }
        //else
        d.Cnv.SetText(pPItem->CnvTxt());
        d.sIndividualCnv = d.Cnv.Text();
 
        if (IsFloatData(d.iType))
          {
          //int Width    = (int)DDEF_WIDTH(d.iFlags);
          //int Precision= (int)DDEF_PRECISION(d.iFlags);
          d.dVal = pPItem->Value()->GetDouble(d.Cnv.Index(), d.Cnv.Text());
          }
        else
          {
          d.sValue = pPItem->Value()->GetString("%i", "%g", d.Cnv.Index(), d.Cnv.Text());
          if (d.iType==tt_RqdPage || d.iType==tt_OptPage)
            {
            d.fKeepFldName=FirstPage;
            FirstPage=0;
            }
          }
        d.AdjTextValue(NULL, false);

#if WITHSPECSHEETS
//ss        d.fIsSpecSheet=0;
//ss        if (IsLayout(d.iType))
//ss          d.sSymOrTag = d.sValue();
//ss        else if (d.sSymOrTag.XStrICmp("SpecSheet")==0)
//ss          {
//ss          d.fIsSpecSheet=1;
//ss          sSpecSheet=d.sValue();
//ss          iSpecSheet=iNo;
//ss          }
#else
        if (IsLayout(d.iType))
          d.sSymOrTag = d.sValue();
#endif

        if (IsStrng(d.iType) && (d.iFlags & MultiLineStr))
          {
          d.fMultiStrng=(d.sValue.XStrChr('\n')!=NULL);
          char *p=d.sValue(), *p1;
          if (p)
            {
            while ((p1=strchr(p, '\n'))!=NULL)
              {
              *p1=0;
              if (strlen(p)>0)
                d.ValLst.Append(p);
              p=p1+1;
              }
            if (strlen(p)>0)
              d.ValLst.Append(p);
            if (d.ValLst.Length()>0)
              d.sValue=d.ValLst.First()->Str();
            else
              d.sValue=" ";
            }
          else
            d.sValue=" ";
          }
        else if (!d.fHasCheck)
          {
          d.fIndexedStr=!IsFloatData(d.iType) && pPItem->IndexedStrList();
          pPItem->GetStrList(d.ValLst);

          if (d.fIndexedStr)
            {
            int i=atol(d.sValue());
            for (pStrng p=d.ValLst.First(); p; p=d.ValLst.Next())
              if (p->Index()==i)
                break;
            if (p==NULL)
              p=d.ValLst.First();
            if (p==NULL)
              d.sValue="?";
            else if (p->Index()==i)
              d.sValue=p->Str();
            else
              d.sValue=d.ValLst.First()->Str();
            }
          }

        d.iObjNo   = -1;
        d.fIsEdtObj = 0;
        //dbgpln("Item %5i %5i %5i %-10s %08x %s %s", iNo, ObjLvl, NObjs, TagObjTypeNames[d.iType].Str, pPItem, d.sValue(), d.sSymOrTag());
        if (IsStructure(d.iType))
          {
          if (d.iType==tt_Object || d.iType==tt_Element)
            {
            if (ObjLvl==0 && ObjClassId.IsEmpty())
              ObjClassId=d.sValue();

            ObjLvl++;

            pObjAttr[ObjLvl]=ObjAttributes.FindObject(d.sValue());

            d.fIsEdtObj = (ObjLvl>=1) && TagObjClass::EditorExists("", d.sValue(), d.sSymOrTag());
            if (d.fIsEdtObj)
              {
              //dbgpln("  AddObject");
              TaggedObject::SetBuildingAccess(true);
              ASSERT_ALWAYS(NObjs<MaxAccNdObjs, "Too many AccessWindow Objects!", __FILE__, __LINE__);
              if (NObjs<MaxAccNdObjs)
                {
                if ((ObjInst[NObjs]=TagObjClass::Construct("", d.sValue(), NULL, d.sSymOrTag(), NULL, TOA_Free))!=NULL)
                  {
                  //;ObjEdit[NObjs]->PutDatatart();
                  ObjInst[NObjs]->PutDefinedData(d.sSymOrTag(), ObjData.List, Iter,
                    TABOpt_AllInfo|TABOpt_ForView|TABOpt_ForViewConstruct, NULL);
                  ItemAdvanced=true;
                  ObjEdit[NObjs]=ObjInst[NObjs]->ConstructEdit(this);
                  ObjEdit[NObjs]->PutDataDone();
                  ObjChgd[NObjs]=0;
                  d.iObjNo=NObjs++;
                  d.iEndFld=iNo;
                  d.iStrtFld=iNo;
                  }
                else
                  {
                  char buff[256];
                  sprintf(buff, "Object %s not found", d.sSymOrTag());
                  AfxMessageBox(buff);
                  }
                }
              else
                {
                char buff[256];
                sprintf(buff, "Object %s not created, too many objects (>%d)", d.sSymOrTag(), MaxAccNdObjs);
                AfxMessageBox(buff);
                }
              TaggedObject::SetBuildingAccess(false);
              }
            d.pObjAttr=pObjAttr[ObjLvl/*-1*/];
            }
          else if (d.iType==tt_Object_E || d.iType==tt_Element_E)
            {
            d.pObjAttr=pObjAttr[ObjLvl];
            ObjLvl--;
            }
          else
            d.pObjAttr=pObjAttr[ObjLvl];
          if (IsStructEnd(d.iType))
            {
            int Lvl=0;
            for (int i=iNo; i>0; i--)
              {
              if (IsStructEnd(Data[i].iType))
                Lvl++;
              else if (IsStructStart(Data[i].iType) && (!Data[i].fIsEdtObj))
                Lvl--;
              if (Lvl==0)
                {
                Data[i].iEndFld=iNo;
                d.iStrtFld=i;
                break;
                }
              }
            }
          if (IsStructStart(d.iType) && (!d.fIsEdtObj))
            iStructLvl+=2;
          else if (IsStructEnd(d.iType))
            iStructLvl-=2;

          d.fIsStructTreeNd = IsStructStart(d.iType) && (!d.fIsEdtObj);
          d.fIsTreeNd = d.fIsTreeNd || d.fIsStructTreeNd;
          }
        else 
          d.pObjAttr=pObjAttr[ObjLvl];
        NData=Max(NData, iNo+1);
        iNo++;
        }

      if (!ItemAdvanced)
        pPItem=ObjData.NextItem(Iter);
      else
        pPItem=ObjData.CurrentItem(Iter);
      }

    if (ReportedMaxFieldsError)
      {
      LogError("Access", 0/*LF_Exclamation*/, "ERROR: %d too many Access window fields!", ReportedMaxFieldsError);
      }

    CXM_Route HRoute, ARoute;
    flag GotHistorian=false;
    flag GotArchive=false;
    if (gs_pPrj && gs_pPrj->m_bAccHistoryInfoVis)
      {
      GotHistorian=XFindObject(pExecName_Historian, HRoute);
      GotArchive=XFindObject(pExecName_ArcMngr, ARoute);
      }
    flag HistInfoVis=(GotHistorian && gs_pPrj && gs_pPrj->m_bAccHistoryInfoVis);
    flag ArcInfoVis=(GotArchive && gs_pPrj && gs_pPrj->m_bAccHistoryInfoVis);

    int nTagItems=0;
    CVarTagArray TagItems;
    CRptTagLists::LoadVarTags(ObjClassId(), NULL, TagItems, nTagItems);

    for (int di = 0; di < NData; di++)
      {
      AccNdData & d=Data[di];

      d.fIsStrng=IsStrng(d.iType) || (!d.fHasNANStr && (d.ValLst.Length()>0) && !d.fHasButton);
#if WITHSPECSHEETS
//ss      d.fIsParam=(((d.iFlags & DDEF_PARAM)!=0) && !d.fIsSpecSheet);
#else
      d.fIsParam=((d.iFlags & DDEF_PARAM)!=0);
#endif
      if (d.fIsParam && ((d.iFlags & DDEF_PARAMSTOPPED)!=0) && XRunning())
        d.fIsParam=false;
      if (d.fMultiStrng)
        d.fIsParam=0;
      d.fIsData=IsData(d.iType);
      d.fPageOn=1;
      d.ObjOn=1;
      d.fHasComment=(d.sTagComment.IsNotEmpty()>0);

      //d.pDTagStart=d.sDTag.Str();
      if (d.fIsStrng)
        d.sValue.Trim();

      d.sClsTag=Data[0].sValue();
      d.sRefTag="";
      if (!IsLayout(d.iType) && d.sSymOrTag.IsNotEmpty())
        {
        for (int i=1; i < di; i++)
          if (Data[i].iEndFld>0)
            if (Data[i].iEndFld>di)
              {
              if (Data[i].fIsStruct)
                {
                if (d.sRefTag.IsNotEmpty())
                  {
                  d.sRefTag+=".";
                  }
                d.sRefTag+=Data[i].sSymOrTag();

                if (Data[i].iType==tt_Object)
                  d.sClsTag="";
                else
                  d.sClsTag+=".";
                d.sClsTag+=Data[i].sSymOrTag();//sValue();
                }
              }
            else
              i=Data[i].iEndFld;
        if (d.sRefTag.IsNotEmpty())
          d.sRefTag+=".";
        d.sRefTag+=d.sSymOrTag();

        d.sClsTag+=".";
        d.sClsTag+=d.sSymOrTag();
        }

      if (d.sRefTag.Length()<=0)
        d.sFullTag=" ";
      else if (di>0 || d.fHasButton)//NdTag.Length()>0)
        d.sFullTag.Set("%s.%s", NdTag(), d.sRefTag());
      else
        d.sFullTag.Set("%s",d.sRefTag());

      if (d.iObjNo>=0)
        ObjEdit[d.iObjNo]->SetFullObjTag(d.sFullTag());

      // Test Cnvs;
      // Set to primary Text if sec text supplied is BAD
      if ((d.Cnv.Index())>0 && Cnvs[d.Cnv.Index()]->Find(d.Cnv.Text())==NULL)
        {
        if (d.Cnv.Text())
          LogWarning(d.sSymOrTag(), 0, "Unknown Conversion '%s' Set to '%s' for %s", d.Cnv.Text(), Cnvs[d.Cnv.Index()]->Txt(), d.sFullTag());
        d.Cnv.SetText(Cnvs[d.Cnv.Index()]->Txt()); // Set to Primary Value
        }
      d.sIndividualCnv = d.Cnv.Text();

      for (int iMVS=0; iMVS<nTagItems; iMVS++)
        if (d.sRefTag.XStrICmp(TagItems[iMVS].m_sTag)==0)
          {
          d.iViewIndex=iMVS;
          break;
          }

#pragma chNOTE("NBNB Implement faster method to check for tags in historian and/or archiver!!!")
      if (HistInfoVis && IsNumData(d.iType))
        {// NBNB Very Slow
        CXM_HistoryExists *xb=new CXM_HistoryExists (0, d.sFullTag());
        CXMsgLst XM;
        XM.PackMsg(xb);
        DWORD RetCode = XSendMessage(XM, HRoute);
        d.fRecorded = ((RetCode & RC_SM_HistRecOn)!=0);
        }
      if (ArcInfoVis && IsNumData(d.iType))
        {// NBNB Very Slow
        CXM_ArchiveExists *xb=new CXM_ArchiveExists (d.sFullTag());
        CXMsgLst XM;
        XM.PackMsg(xb);
        DWORD RetCode = XSendMessage(XM, ARoute);
        d.fArchived = ((RetCode & RC_SM_ArcExists)!=0);
        }

      }
#if WITHSPECSHEETS
//ss    SSLoad(sSpecSheet());
#else
    for (int i=0; i<NData; i++)
      Data[i].fUsed=false;
    NFlds=NData;
    for (i = 0; i < NFlds; i++)
      {
      AccNdFld  &f  = Flds[i];
      AccNdData &d  = Data[i];
      f.pData       = &Data[i];
      f.iData       = i;
      //f.sTagText    = d.sSymOrTag;
      //f.sTagText    = (DoTagOnly && d.sTagOnly.Length()>0) ? d.sTagOnly : d.sSymOrTag;
      f.sTagText    = GetDisplayTag(d.sTagOnly, d.sSymOrTag); 

      f.iType       = d.iType;
      f.fIsData     = d.fIsData;
      f.iLvl        = d.iLvl;
      f.fIsTreeNd   = d.fIsTreeNd;
      f.fUseComment = d.fHasComment;
      f.iUseComment = i;
      }
#endif
    ConnectFields();
    }

//dbgpln("AccNodeWnd::LoadAccessData <<");
  return true;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::CloseAccessData(flag SaveIt, flag CancelIt, flag Disconnect)
  {
  if (ObjClassId.IsNotEmpty())
    {
    SaveCurrentPageInfo();
    //CProfINIFile PF(PrjIniFile());
    //PF.WrInt("AccessPages", ObjClassId(), CPgNo);
    //int y= VScrlOn ? VScrl->GetScrollPos() : -1;
    //int x= HScrlOn ? HScrl->GetScrollPos() : -1;
    //int c= ChEditPos.x;
    //int r= ChEditPos.y;
    //int xxx=0;
    }

  flag OK=0;
  if ((SaveIt || CancelIt) && ObjRoute.NodeObjId(0)>=0)//lNdEOId>=0)
    {
    flag DoWrite=0;

    CXM_ObjectData ObjData(0, TABOpt_ForView);
    if (NData>0 && SaveIt)
      {
      CPkDataIter Iter;
      CPkDataItem * pPItem=ObjData.FirstItem(Iter);

      for (int fi=0; fi<NData; fi++)
        {
        AccNdData & f=Data[fi];
        char* pVal=f.sValue() ? f.sValue() : "";
        if (IsStructStart(f.iType))
          {
          if (f.fIsEdtObj)
            {
            ObjEdit[f.iObjNo]->GetDataStart();
            ObjInst[f.iObjNo]->GetDefinedData(f.sSymOrTag()/*NdTag()*/, ObjData.List, TABOpt_AllInfo|TABOpt_ForView|NSHM_All/*|TABOpt_CnvInfo*/, /*pPItem,*/ NULL);
            ObjEdit[f.iObjNo]->GetDataDone();
            DoWrite=1;
            }
          else
            ObjData.List.SetStructureS(/*pPItem,*/ f.iType, f.sSymOrTag(), pVal ? pVal : "", "", "", "", NULL, NULL, 0);
          }
        else if (IsStructEnd(f.iType))
          {
          ObjData.List.SetStructureE(/*pPItem,*/ f.iType, f.sSymOrTag());
          }
#if WITHSPECSHEETS
//ss        else if (f.fIsData && (f.fIsParam || f.fIsSpecSheet))
#else
        else if (f.fIsData)
#endif
          if (f.fIsModified)
            {
            DoWrite=1;
            flag DoWrt=1;

            if (f.ValLst.Length()>0 && f.fIndexedStr)
              {
              f.sValue.Trim();
              pVal=f.sValue() ? f.sValue() : "";
              for (pStrng pS=f.ValLst.First(); pS; pS=pS->Next())
                {
                pchar pStr=pS->Str() ? pS->Str() : "";
                if (_stricmp(pStr, pVal)==0)
                  {
                  DoWrt=0;
                  char Buff[256];
                  sprintf(Buff, "%i", pS->Index());
                  PkDataUnion PData;
                  PData.SetTypeString(f.iType, Buff, f.Cnv.Index(), f.Cnv.Text());
                  ObjData.List.SetDataValue(/*pPItem,*/ /*f.iType,*/ f.sSymOrTag(), PData);
                  break;
                  }
                }
              }
            if (DoWrt)
              {
              PkDataUnion PData;
              if (f.iType==tt_Double)
                PData.SetTypeDouble(f.iType, f.dVal, f.Cnv.Index(), f.Cnv.Text());
              else
                PData.SetTypeString(f.iType, f.sValue() ? f.sValue() : "", f.Cnv.Index(), f.Cnv.Text());
              ObjData.List.SetDataValue(/*pPItem,*/ /*f.iType,*/ f.sSymOrTag(), PData);
              }
            }
        }

      //ObjData.SetSize();
      }
    else if (CancelIt)
      {
      //ClrCurrentEdit(true); //KGA
      DoWrite=0;
      }

    if (DoWrite)
      {
      #if dbgDumpObject
      ObjRoute.dbgDump("Access : Save");
      ObjData.List.dbgDump(1, "Access : Save");
      #endif

      //XOpenTransaction();
      //ObjRoute.SetAtStart();
      switch (XWriteTaggedItem(ObjData, ObjRoute))
        {
        case TOData_NotFound :
          LogError("Access", 0/*LF_Exclamation*/, "%s not found", NdTag());
          break;
        case TOData_InvalidData :
          fLastSaveInvalid=1;
          LogError(NdTag(), 0/*LF_Exclamation*/, "Invalid data");
          OK=1;
          break;
        case TOData_OK:
          OK=1;
//??      gs_Exec.m_Seq.NoteManSet(CExecSequence::VSS_Access, WrkTag(), DU);
          break;
        }
      if (gs_pTheSFELib)
        gs_pTheSFELib->SetLoadAllModelStatus();
      //XCloseTransaction();
      }
    else
      OK=1;
    }
  else if (!SaveIt && !CancelIt) // Refresh
    OK=1;

  ClrFldsChanged();
  for (int fi=0; fi<NData; fi++)
    {
#if WITHSPECSHEETS
//ss    if (Data[fi].fIsData && (Data[fi].fIsParam || Data[fi].fIsSpecSheet))
#else
    if (Data[fi].fIsData && Data[fi].fIsParam)
#endif
      Data[fi].fIsModified = 0;
    }
  if (Disconnect)
    ObjRoute.Clear();//lNdEOId=-1;

  Invalidate();
  XUpdateDisplays(false, false);

  Pages=0; // NBNB This should be in a routine FxdEdtView::Clear();

  DeleteWinControls();

  for (int i=0; i < MaxFixedPages; i++)
    if (Pg[i])
       {
       delete Pg[i];
       Pg[i]=NULL;
       }

  Pages=0;
  int PrvPgNo=CPgNo;
  CPgNo=-1;
  CPg=NULL;

  return OK;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::LoadNdHist()
  {
  NdHistList.SetSize(MaxNdHistListSize);
  int Cnt = 0;
  char Buffer[8192];
  CProfINIFile PF(PrjIniFile());
  if (PF.RdSection("AccessWndTagHistory", Buffer, sizeof(Buffer))>0)
    {
    char* pBuff = Buffer;
    char* pName;
    char* pFindTxt;
    while (pBuff[0] && Cnt<MaxNdHistListSize)
      {
      PF.GetProfItem(pBuff, pName, pFindTxt);
      if (pFindTxt)
        NdHistList[Cnt++] = pFindTxt;
      }
    }
  NdHistList.SetSize(Cnt);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::SaveNdHist()
  {
  char Buff[8192];
  CProfINIFile PF(PrjIniFile());
  int Pos = 0;
  const int Size = NdHistList.GetSize();
  for (int i=0; i<Size; i++)
    {
    int Len = NdHistList[i].Len();
    if (Len>0)
      {
      sprintf(&Buff[Pos], "Hist_%02d=%s", i, NdHistList[i]());
      Pos = Pos + 8 + Len;
      Buff[Pos++] = 0;
      }
    }
  Buff[Pos++] = 0;
  Buff[Pos++] = 0;
  PF.WrSection("AccessWndTagHistory", Buff);
  }

//---------------------------------------------------------------------------

#if WITHSPECSHEETS
//ssflag AccNodeWnd::SSFindNext(FILE * hr, FILE * hw, flag WriteBuff, char * Buff, int BuffSize)
//ss  {
//ss  //int l=strlen(Name);
//ss  while (fgets(Buff, BuffSize, hr))
//ss    {
//ss    char *p=Buff;
//ss    while (*p==' ') p++; // Skip Blanks
//ss    if (*p=='[')
//ss      break;
//ss    if (hw)
//ss      fprintf(hw, "%s", Buff);
//ss    }
//ss  return 1;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SSFind(FILE * hr, FILE * hw, const char * Name, flag WriteBuff, char * Buff, int BuffSize, flag & ReadOnly)
//ss  {
//ss  flag Found=false;
//ss  ReadOnly=false;
//ss  if (hw && WriteBuff)
//ss    fprintf(hw, "%s", Buff);
//ss  int l=strlen(Name);
//ss  while (fgets(Buff, BuffSize, hr))
//ss    {
//ss    char *p=Buff;
//ss    while (*p==' ') p++; // Skip Blanks
//ss    if (*p=='[')
//ss      if (_strnicmp(++p, Name, l)==0)
//ss        if (*(p+l)==']')
//ss          {
//ss          Found=true;
//ss          p+=l+1; // skip over ']'
//ss          while (*p==' ') p++; // Skip Blanks
//ss          if (*p=='\t')
//ss            {
//ss            p++;
//ss            while (*p!='\t' && *p!='\n' && *p!=0)
//ss              {
//ss              if (*p=='R')
//ss                ReadOnly=true;
//ss              p++;
//ss              }
//ss            }
//ss
//ss          break;
//ss          }
//ss    if (hw)
//ss      fprintf(hw, "%s", Buff);
//ss    }
//ss  return Found;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SSNames(Strng_List & Names, CByteArray & ReadOnlyList)
//ss  {
//ss  Names.Clear();
//ss  ReadOnlyList.SetSize(0);
//ss  flag GotDefault=0;
//ss  //Strng Fn(PrjDirectory());
//ss  //Fn+=ObjClassId;
//ss//  Strng ss;
//ss  Strng Fn(ObjClassId);
//ss  Fn+=".SCS.Txt";
//ss  Fn.FnSearchExpand();
//ss  FILE * h=fopen(Fn(), "rt");
//ss  if (h)
//ss    {
//ss    char Buff[4096];
//ss    CSVColArray C
//ss    while (fgets(Buff, sizeof(Buff), h))
//ss      {
//ss      int nCols=ParseTabTokens(Buff, C);
//ss      if (nCols>=1)
//ss        {
//ss        char *p=C[0];
//ss        while (*p==' ') p++; // Skip Blanks
//ss        if (*p=='[')
//ss          {
//ss          p++;
//ss          for (int l=0; p[l]!=0; l++)
//ss            if (p[l]==']')
//ss              {
//ss              flag ReadOnly=false;
//ss              if (nCols>1 && C[1])
//ss                {
//ss                char* pp = C[1];
//ss                while (*pp==' ') pp++; // Skip Blanks
//ss                while (*pp!='\t' && *pp!='\n' && *pp!=0)
//ss                  {
//ss                  if (*pp=='R')
//ss                    ReadOnly=true;
//ss                  pp++;
//ss                  }
//ss                }
//ss
//ss              p[l]=0;
//ss              //if (strlen(p)==0)
//ss              //  p="Standard";
//ss              Names.Append(p);
//ss              ReadOnlyList.Add(ReadOnly);
//ss              if (_stricmp(p, "Standard")==0)
//ss                GotDefault=1;
//ss              break;
//ss              }
//ss          }
//ss        }
//ss      }
//ss    fclose(h);
//ss    }
//ss  if (!GotDefault)
//ss    {
//ss    Names.Append("Standard");
//ss    ReadOnlyList.Add(0);//1);
//ss    }
//ss  Names.Append("-");
//ss  ReadOnlyList.Add(1);
//ss  return 1;
//ss  };
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SSDelete(const char * Name)
//ss  {
//ss  //Strng FnR(PrjDirectory()), FnW(PrjDirectory());
//ss  //FnR+=ObjClassId;
//ss//  Strng ss;
//ss  Strng FnR(ObjClassId);
//ss  FnR+=".SCS.Txt";
//ss  FnR.FnSearchExpand();
//ss  Strng FnW(FnR);
//ss  FnW.SetLength(FnW.Length()-4);
//ss  FnW+=".SCX";
//ss    //FnW+=ObjClassId;
//ss  //FnW+=".SCX";
//ss  FILE * hr=fopen(FnR(), "rt");
//ss  FILE * hw=fopen(FnW(), "wt");
//ss  if (hw && hr)
//ss    {
//ss    char Buff[4096];
//ss    flag ReadOnly, RO;
//ss    SSFind(hr, hw, Name, false, Buff, sizeof(Buff), ReadOnly);
//ss    if (!ReadOnly)
//ss      {
//ss      SSFindNext(hr, NULL, false, Buff, sizeof(Buff));
//ss      SSFind(hr, hw, "@@##@@##@@", true, Buff, sizeof(Buff), RO);
//ss      fclose(hr);
//ss      fclose(hw);
//ss      if (_unlink(FnR())==0)
//ss        {
//ss        if (rename(FnW(), FnR())==0)
//ss          return 1;
//ss        else
//ss          LogError("Access", 0, "SpecSheet file %s not renamed", FnW());
//ss        }
//ss      else
//ss        LogError("Access", 0, "SpecSheet file %s not deleted", FnR());
//ss      }
//ss    else
//ss      LogError("Access", 0, "SpecSheet file %s not deleted (ReadOnly)", FnR());
//ss    }
//ss  else
//ss    LogError("Access", 0, "SpecSheet file %s not opened", FnW());
//ss  return 0;
//ss  };
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SSSave(const char * Name)
//ss  {
//ss  if (_stricmp(Name, "-")==0)
//ss    return false;
//ss
//ss  //Strng FnR(PrjDirectory()), FnW(PrjDirectory());
//ss  //FnR+=ObjClassId;
//ss  //FnR+=".SCS.Txt";
//ss  //FnW+=ObjClassId;
//ss  //FnW+=".SCX";
//ss//  Strng ss;
//ss  Strng FnR(ObjClassId);
//ss  FnR+=".SCS.Txt";
//ss//  FnR=AdaptFilename2(ss, FnR());
//ss  FnR.FnSearchExpand();
//ss  Strng FnW(FnR);
//ss  FnW.SetLength(FnW.Length()-4);
//ss  FnW+=".SCX";
//ss  FILE * hr=fopen(FnR(), "rt");
//ss  FILE * hw=fopen(FnW(), "wt");
//ss  if (hw)
//ss    {
//ss    char Buff[4096];
//ss    flag ReadOnly=false;
//ss    if (hr)
//ss      {
//ss      SSFind(hr, hw, Name, false, Buff, sizeof(Buff), ReadOnly);
//ss      SSFindNext(hr, NULL, false, Buff, sizeof(Buff));
//ss      }
//ss    else
//ss      fprintf(hw, "Type\tFlags\tFieldName\tAccessTag\tFormat\tPrecision\tConversion\n");
//ss    if (!ReadOnly)
//ss      {
//ss      fprintf(hw, "[%s]\t \t|======================\n", Name);
//ss      for (int i=0; i<NData; i++)
//ss        {
//ss        AccNdData &d=Data[i];
//ss        switch (d.iType)
//ss          {
//ss          case tt_RqdPage:
//ss          case tt_OptPage:
//ss            fprintf(hw, "%s\t%s\t%s\t%s\t%c\t%i\t%s\n", d.iType==tt_OptPage ? "Po" : "Pr",
//ss                     "v", d.sSymOrTag(), "", ' ', 0, "");
//ss            break;
//ss          case tt_Text:
//ss            fprintf(hw, "%s\t%s\t%s\t%s\t%c\t%i\t%s\n", "T", "v-", d.sSymOrTag() ? d.sSymOrTag() : "",
//ss                    "", d.Fmt.Type(), d.Fmt.Prec(), d.Cnv.Text() ? d.Cnv.Text() : "");
//ss            break;
//ss          default:
//ss            if (!IsStructure(d.iType))
//ss              fprintf(hw, "%s\t%s\t%s\t%s\t%c\t%i\t%s\n", "D", "v", d.sSymOrTag(),
//ss                      d.sRefTag() ? d.sRefTag() : "", d.Fmt.Type(),
//ss                      d.Fmt.Prec(), d.Cnv.Text() ? d.Cnv.Text() : "");
//ss          }
//ss        }
//ss      if (hr)
//ss        {
//ss        flag ReadOnly;
//ss        SSFind(hr, hw, "@@##@@##@@", false, Buff, sizeof(Buff), ReadOnly);
//ss        fclose(hr);
//ss        }
//ss      fclose(hw);
//ss      if (hr==NULL || _unlink(FnR())==0)
//ss        {
//ss        if (rename(FnW(), FnR())==0)
//ss          {
//ss          sSpecSheet=Name;
//ss          if (iSpecSheet>=0)
//ss            {
//ss            Data[iSpecSheet].sValue=Name;
//ss            Data[iSpecSheet].fIsModified=1;
//ss            }
//ss
//ss          return 1;
//ss          }
//ss        else
//ss          LogError("Access", 0, "SpecSheet file %s not renamed", FnW());
//ss        }
//ss      else
//ss        LogError("Access", 0, "SpecSheet file %s not deleted", FnR());
//ss      }
//ss    else
//ss      LogError("Access", 0, "SpecSheet %s not deleted (ReadOnly)", FnR());
//ss    }
//ss  else
//ss    LogError("Access", 0, "SpecSheet file %s not opened", FnW());
//ss  return 0;
//ss  };
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SSLoad(const char * Name)
//ss  {
//ss  SSReadOnly=false;
//ss  for (int i=0; i<NData; i++)
//ss    Data[i].fUsed=false;
//ss  if ((strlen(Name)>0) && (_stricmp(Name, "-")!=0))
//ss    {
//ss    flag GotPers=0;
//ss   // NBNB use PrjBaseDirectory if Reqd
//ss//    Strng ss;
//ss    Strng Fn(ObjClassId);
//ss    Fn+=".SCS.Txt";
//ss    Fn.FnSearchExpand();
//ss    //Strng Fn(PrjDirectory());
//ss    //Fn+=ObjClassId;
//ss    //Fn+=".SCS.Txt";
//ss    FILE * h=fopen(Fn(), "rt");
//ss    if (h)
//ss      {
//ss      char Buff[4096];
//ss      if (SSFind(h, NULL, Name, false, Buff, sizeof(Buff), SSReadOnly))
//ss        {
//ss        SSLoadData SSLD;
//ss        GotPers=1;
//ss        NFlds=0;
//ss        for (int i = 0; i < MaxAccNdFlds; i++)
//ss          Flds[i].Init();
//ss
//ss        flag Done=0;
//ss        fgets(Buff, sizeof(Buff), h);
//ss        while (!feof(h) && !Done)
//ss          {
//ss          Done=!SSLoadLine(Buff, SSLD);
//ss          fgets(Buff, sizeof(Buff), h);
//ss          }
//ss        }
//ss
//ss      fclose(h);
//ss      }
//ss    if (GotPers && (NFlds>0))
//ss      return true;
//ss    //if (_stricmp(Name, "Standard")!=0)
//ss    //  return false;
//ss    }
//ss
//ss  NFlds=NData;
//ss  for (i = 0; i < NFlds; i++)
//ss    {
//ss    AccNdFld  &f  = Flds[i];
//ss    AccNdData &d  = Data[i];
//ss    f.pData       = &Data[i];
//ss    f.iData       = i;
//ss    //f.sTagText    = d.sSymOrTag;
//ss    //f.sTagText    = (DoTagOnly && d.sTagOnly.Length()>0) ? d.sTagOnly : d.sSymOrTag;
//ss    f.sTagText    = GetDisplayTag(d.sTagOnly, d.sSymOrTag); 
//ss
//ss    f.iType       = d.iType;
//ss    f.fIsData     = d.fIsData;
//ss    f.iLvl        = d.iLvl;
//ss    f.fIsTreeNd   = d.fIsTreeNd;
//ss    f.fUseComment = d.fHasComment;
//ss    f.iUseComment = i;
//ss    }
//ss  return true;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//sstypedef Strng PartStrngs[MaxRptLvl];
//sstypedef int   MatchNumbers[MaxRptLvl];
//ss
//ssint Split2PartsStar(char * Str, PartStrngs &Parts, flag ToUpper)
//ss  {
//ss  int nParts=0;
//ss  char * ps=Str;
//ss  char * pe=strchr(ps, '*');
//ss  while (pe)
//ss    {
//ss    *pe=0;
//ss    Parts[nParts++]=ps;
//ss    ps=pe+1;
//ss    pe=strchr(ps, '*');
//ss    }
//ss  //if (ps && strlen(ps)>0)
//ss  Parts[nParts++]=ps;
//ss  if (ToUpper)
//ss    for (int i=0; i<nParts; i++)
//ss      Parts[i].Upper();
//ss  return nParts;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssint Split2PartsParts(char * Str, PartStrngs &Parts, MatchNumbers &MatchNos, flag ToUpper)
//ss  {
//ss  Strng Prt;
//ss  int nParts=0;
//ss  Prt.Set("\\%i", nParts);
//ss  char * ps=Str;
//ss  char * pe=strstr(ps, Prt());
//ss  while (pe)
//ss    {
//ss    *pe=0;
//ss    Parts[nParts]=ps;
//ss    MatchNos[nParts]=nParts;
//ss    nParts++;
//ss    ps=pe+strlen(Prt());
//ss    Prt.Set("\\%i", nParts);
//ss    pe=strstr(ps, Prt());
//ss    }
//ss  //if (ps && strlen(ps)>0)
//ss  Parts[nParts]=ps;
//ss  MatchNos[nParts]=-1;
//ss  if (ToUpper)
//ss    for (int i=0; i<nParts; i++)
//ss      Parts[i].Upper();
//ss  return nParts;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag FindPartMatches(Strng S, int nParts, PartStrngs &Parts, PartStrngs &Matches, SSLoadData & LD)
//ss  {
//ss  S.Upper();
//ss  char * From=S();
//ss  char * PartPos[MaxRptLvl];
//ss  flag OK=1;
//ss  for (int i=0; i<nParts && OK ; i++)
//ss    {
//ss    if (Parts[i].Length()>0)
//ss      PartPos[i]=strstr(From, Parts[i]());
//ss    else
//ss      PartPos[i]=From;//+strlen(From); // go to end
//ss    if (PartPos[i]==NULL)
//ss      OK=false;
//ss    else
//ss      {
//ss      if (i>0)
//ss        {
//ss        Matches[i-1]="";
//ss        char* p0=PartPos[i-1]+Parts[i-1].Length();
//ss        char* p1=PartPos[i];
//ss        if (p0 && p1)
//ss          for (char* p=p0; p<p1; p++)
//ss            Matches[i-1]+=*p;
//ss        }
//ss      From+=Parts[i].Length();
//ss      }
//ss    }
//ss  return OK;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SSLoadLine(const char * LineData, SSLoadData & LD)
//ss  {
//ss  LD.Loaded=true;
//ss  LD.DataLine=false;
//ss  char Buff[4096];
//ss  CSVColArray C*pNewLine;
//ss
//ss  strcpy(Buff, LineData);
//ss  pNewLine=strchr(Buff, '\n');
//ss  if (pNewLine)
//ss    *pNewLine=0;
//ss  strcat(Buff, " \t \t \t \t "); // ensure that 4 columns exist
//ss  int nCols=ParseTabTokens(Buff, C);
//ss
//ss  char c0=C[0][0];
//ss  if (LD.RptOK() && LD.Rpt().Reading)
//ss    if ((c0!='{') && (c0!='}'))
//ss      {
//ss      LD.Rpt().Lst.Append(LineData);
//ss      }
//ss
//ss  int Visible = (strchr(C[1], 'v')!=NULL);
//ss  switch (c0)
//ss    {
//ss    case '[':
//ss      LD.Loaded=false;
//ss      break;
//ss    case '{':
//ss      LD.RptLvl++;
//ss      if (LD.RptLvl<MaxRptLvl)
//ss        {
//ss        LD.Rpt().Reading=true;
//ss        LD.Rpt().MaxCount=atoi(C[1]);
//ss        LD.Rpt().PriorMatches.Clear();
//ss        LD.Rpt().WorkingMatch="";
//ss        LD.Rpt().DataFld1=NFlds;
//ss        LD.Rpt().DataLvl=1000;
//ss        }
//ss      break;
//ss    case '}':
//ss      if (LD.RptLvl<MaxRptLvl)
//ss        {
//ss        LD.Rpt().Reading=false;
//ss        for (int Loop=0; Loop<LD.Rpt().MaxCount; Loop++)
//ss          {
//ss          int NFldsStart=NFlds;
//ss
//ss          // Find The match for this Pass
//ss
//ss          flag FoundMatch=false;
//ss          int DataFld1;
//ss          int DataLvl;
//ss          for (pStrng p=LD.Rpt().Lst.First(); p && !FoundMatch; p=LD.Rpt().Lst.Next())
//ss            {
//ss            char Buff[4096];
//ss            CSVColArray C*pNewLine;
//ss
//ss            strcpy(Buff, p->Str());
//ss            pNewLine=strchr(Buff, '\n');
//ss            if (pNewLine)
//ss              *pNewLine=0;
//ss            strcat(Buff, " \t \t \t \t "); // ensure that 4 columns exist
//ss            int nCols=ParseTabTokens(Buff, C);
//ss
//ss            PartStrngs Parts;
//ss            PartStrngs Matches;
//ss            int nParts=Split2PartsStar(C[3], Parts, true);
//ss
//ss            if (nParts>=LD.RptLvl+2)
//ss              for (int iFld=0; (iFld<NData) && !FoundMatch; iFld++)
//ss                if (Data[iFld].fIsData && !Data[iFld].fUsed)
//ss                  if (FindPartMatches(Data[iFld].sRefTag, nParts, Parts, Matches, LD))
//ss                    if (!LD.Rpt().PriorMatches.Find(Matches[LD.RptLvl]()))
//ss                      {
//ss                      LD.Rpt().PriorMatches.Append(Matches[LD.RptLvl]());
//ss                      LD.Rpt().WorkingMatch=Matches[LD.RptLvl];
//ss                      FoundMatch=true;
//ss                      DataFld1=iFld;
//ss                      DataLvl=Data[iFld].iLvl;
//ss                      }
//ss            }
//ss
//ss          flag FoundOne=false;
//ss          // if found use the match to find lines required for this pass
//ss          if (FoundMatch)
//ss            {
//ss            //LD.BlkDataFld=DataFld1;
//ss            while ((DataFld1>=0) && (Data[DataFld1].iLvl>=DataLvl))
//ss              DataFld1--;
//ss            LD.Rpt().DataFld1=DataFld1;
//ss            LD.Rpt().DataLvl=Data[DataFld1].iLvl;
//ss            for (pStrng p=LD.Rpt().Lst.First(); p; p=LD.Rpt().Lst.Next())
//ss              {
//ss              SSLoadLine(p->Str(), LD);
//ss              FoundOne=FoundOne ||(LD.Loaded && LD.DataLine);
//ss              }
//ss            }
//ss          if (!FoundOne)
//ss            {
//ss            NFlds=NFldsStart;
//ss            break;
//ss            }
//ss          }
//ss        LD.Rpt().Lst.Clear();
//ss        }
//ss      LD.RptLvl--;
//ss      break;
//ss    case 'D':
//ss      {
//ss      if (!Visible)
//ss        break;
//ss
//ss      // Split Tag;
//ss      PartStrngs Parts;
//ss      PartStrngs Matches;
//ss      int nParts=Split2PartsStar(C[3], Parts, true);
//ss
//ss      if (LD.RptOK() && LD.Rpt().Reading)
//ss        break;
//ss
//ss      int iFld=0;
//ss      int Found=false;
//ss      if (LD.fDoingBlk)
//ss        {
//ss        iFld=LD.BlkDataFld++;
//ss        Found=true;
//ss        }
//ss      else if ((LD.RptLvl<0) || (nParts==1))
//ss        {
//ss        for (iFld=0; (iFld<NData && !Found); iFld++)
//ss          if (Data[iFld].fIsData && Data[iFld].sRefTag.XStrICmp(C[3])==0)
//ss            {
//ss            Found=true;
//ss            break;
//ss            }
//ss        }
//ss      else
//ss        {
//ss        for (iFld=LD.Rpt().DataFld1; ((iFld<NData) && (Data[iFld].iLvl>=LD.Rpt().DataLvl)); iFld++)
//ss          if (Data[iFld].fIsData && !Data[iFld].fUsed)
//ss            if (FindPartMatches(Data[iFld].sRefTag, nParts, Parts, Matches, LD))
//ss              if (LD.Rpt().WorkingMatch.XStrICmp(Matches[LD.RptLvl])==0)
//ss                {
//ss                Found=true;
//ss                break;
//ss                }
//ss        }
//ss
//ss      if (Found)
//ss        {
//ss        AccNdFld  &f  = Flds[NFlds];
//ss        AccNdData &d  = Data[iFld];
//ss        f.pData       = &Data[iFld];
//ss        f.iData       = iFld;
//ss        if (!d.fKeepFldName)
//ss          {
//ss          PartStrngs   TagParts;
//ss          MatchNumbers MatchNos;
//ss          int nTagParts=Split2PartsParts(C[2], TagParts, MatchNos, false);
//ss          if (nTagParts)
//ss            {
//ss            f.sTagText = "";
//ss            for (int iM=0; iM<nTagParts; iM++)
//ss              {
//ss              f.sTagText += TagParts[iM];
//ss              if ((iM<nTagParts-1) && (MatchNos[iM]<=LD.RptLvl))
//ss                f.sTagText += LD[MatchNos[iM]].WorkingMatch;
//ss              }
//ss            }
//ss          else
//ss            f.sTagText = C[2];
//ss          }
//ss        f.iType       = Data[iFld].iType;
//ss        f.fIsData     = d.fIsData;
//ss        f.iLvl        = d.iLvl;
//ss        f.fIsTreeNd   = d.fIsTreeNd;
//ss        f.fUseComment = d.fHasComment;
//ss        f.iUseComment = iFld;
//ss        d.fUsed       = true;
//ss
//ss        if (strchr(C[1], 'c')) // Find a comment
//ss          {
//ss          int ic=iFld;
//ss          while (ic>=0)
//ss            {
//ss            int ic1=ic;
//ss            while (ic>=0 && Data[ic].iLvl>=Data[ic1].iLvl)
//ss              ic--;
//ss            if (Data[ic].fHasComment)
//ss              break;
//ss            }
//ss          if (ic>=0)
//ss            {
//ss            f.fUseComment = 1;
//ss            f.iUseComment = ic;
//ss            }
//ss          }
//ss
//ss        d.Fmt.SetType((C[4] ? *(C[4]) : (char)'f'));
//ss        d.Fmt.SetPrec((byte)atol(C[5] ? C[5] : "2"));
//ss        d.AdjTextValue(C[6], false);
//ss
//ss        NFlds++;
//ss        LD.DataLine=true;
//ss        }
//ss      }
//ss      break;
//ss    case 'T':
//ss      {
//ss      if (!Visible)
//ss        break;
//ss      if (LD.RptOK() && LD.Rpt().Reading)
//ss        break;
//ss      int Copy = (strchr(C[1], 'c')!=NULL);
//ss
//ss      rAccNdFld f = Flds[NFlds];
//ss      f.pData      = NULL;
//ss      f.iData      = -1;
//ss      if (LD.fDoingBlk)
//ss        {
//ss        int iFld=LD.BlkDataFld++;
//ss        AccNdData &d = Data[iFld];
//ss        //f.sTagText = (DoTagOnly && d.sTagOnly.Length()>0) ? d.sTagOnly : d.sSymOrTag;//Data[iFld].sTagText;
//ss        f.sTagText    = GetDisplayTag(d.sTagOnly, d.sSymOrTag); 
//ss        d.fUsed=true;
//ss        //Found=true;
//ss        }
//ss      else if (Copy && (LD.RptLvl>=0))
//ss        {
//ss        int Found=false;
//ss        f.sTagText = "";
//ss        for (int iFld=LD.Rpt().DataFld1; ((iFld<NData) && (Data[iFld].iLvl>=LD.Rpt().DataLvl)); iFld++)
//ss          if ((Data[iFld].iType==tt_Text) && !Data[iFld].fUsed)
//ss            {
//ss            AccNdData &d = Data[iFld];
//ss            f.sTagText    = GetDisplayTag(d.sTagOnly, d.sSymOrTag); 
//ss            //f.sTagText = (DoTagOnly && d.sTagOnly.Length()>0) ? d.sTagOnly : d.sSymOrTag;
//ss            d.fUsed=true;
//ss            Found=true;
//ss            break;
//ss            }
//ss        }
//ss      else
//ss        {
//ss        PartStrngs   TagParts;
//ss        MatchNumbers MatchNos;
//ss        int nTagParts=Split2PartsParts(C[2], TagParts, MatchNos, false);
//ss        if (nTagParts)
//ss          {
//ss          f.sTagText = "";
//ss          for (int iM=0; iM<nTagParts; iM++)
//ss            {
//ss            f.sTagText += TagParts[iM];
//ss            if ((iM<nTagParts-1) && (MatchNos[iM]<=LD.RptLvl))
//ss              f.sTagText += LD[MatchNos[iM]].WorkingMatch;
//ss            }
//ss          }
//ss        else
//ss          f.sTagText = C[2];
//ss        }
//ss
//ss      f.iType      = tt_Text;
//ss      f.fIsData    = false;
//ss      f.iLvl       = 0;
//ss      f.fIsTreeNd  = false;
//ss      f.fUseComment= false;
//ss      f.fPadDash   = (strstr(C[1], "-")!=NULL);
//ss      NFlds++;
//ss      }
//ss      break;
//ss    case 'P':
//ss      {
//ss      if (!Visible)
//ss        break;
//ss      if (LD.RptOK() && LD.Rpt().Reading)
//ss        break;
//ss      if (LD.fDoingBlk)
//ss        break;
//ss      rAccNdFld f = Flds[NFlds];
//ss      f.pData      = NULL;
//ss      f.iData      = -1;
//ss      f.sTagText   = LD.PageNo==0 ? NdTag() : C[2];
//ss      f.iType      = (strstr(C[0], "r")) ? tt_RqdPage : tt_OptPage;
//ss      f.fIsData    = false;
//ss      f.iLvl       = 0;
//ss      f.fIsTreeNd  = false;
//ss      f.fUseComment= false;
//ss      NFlds++;
//ss      LD.PageNo++;
//ss      }
//ss      break;
//ss    case 'B':
//ss      {
//ss      if (!Visible)
//ss        break;
//ss      //if (LD.RptLvl<0)
//ss      //  break;
//ss      if (LD.RptOK() && LD.Rpt().Reading)
//ss        break;
//ss      // Split Tag;
//ss      LD.fDoingBlk=true;
//ss      //PartStrngs Parts;
//ss      //PartStrngs Matches;
//ss      //int nParts=Split2PartsStar(C[3], Parts, true);
//ss
//ss      int iFld=0;
//ss      int Found=false;
//ss      if ((LD.RptLvl<0))// || (nParts==1))
//ss        {
//ss        for (iFld=0; (iFld<NData && !Found); iFld++)
//ss          if (Data[iFld].fIsStruct && Data[iFld].sRefTag.XStrICmp(C[3])==0)
//ss            {
//ss            Found=true;
//ss            break;
//ss            }
//ss        }
//ss      else
//ss        {
//ss        DoBreak();
//ss        }
//ss
//ss      if (Found)
//ss        {
//ss
//ss        int WithOrigPgs = (strstr(C[1], "p")!=NULL);
//ss        int WithRqdPage = (strstr(C[1], "P")!=NULL);
//ss        int WithOptPage = (strstr(C[1], "O")!=NULL);
//ss        //AccNdFld  &f  = Flds[NFlds];
//ss        AccNdData &d  = Data[iFld];
//ss        d.fUsed       = true;
//ss
//ss
//ss        if (strlen(C[2])>0)
//ss          {
//ss          if (WithRqdPage || WithOptPage)
//ss            {
//ss            AccNdFld  &f  = Flds[NFlds++];
//ss            f.sTagText   = C[2];
//ss            f.iType      = WithRqdPage ? tt_RqdPage : tt_OptPage;
//ss            f.fIsData    = false;
//ss            f.pData      = NULL;
//ss            f.iLvl       = 0;
//ss            f.fIsTreeNd  = false;
//ss            f.fUseComment= false;
//ss            f.fPadDash   = false;
//ss            }
//ss
//ss          AccNdFld  &f  = Flds[NFlds++];
//ss          f.sTagText   = C[2];
//ss          f.iType      = tt_Text;
//ss          f.fIsData    = false;
//ss          f.pData      = NULL;
//ss          f.iLvl       = 0;
//ss          f.fIsTreeNd  = false;
//ss          f.fUseComment= false;
//ss          f.fPadDash   = (strstr(C[1], "-")!=NULL);
//ss          }
//ss
//ss
//ss        iFld++;
//ss
//ss        for (  ;(iFld<NData) && (Data[iFld].iLvl>d.iLvl); iFld++)
//ss          {
//ss          AccNdData &d  = Data[iFld];
//ss          if ((d.iType==tt_RqdPage || d.iType==tt_OptPage) && !WithOrigPgs)
//ss            continue;
//ss          AccNdFld  &f  = Flds[NFlds++];
//ss          f.pData       = IsLayout(d.iType) ? NULL : &Data[iFld];
//ss          f.iData       = iFld;
//ss          f.sTagText    = GetDisplayTag(d.sTagOnly, d.sSymOrTag); 
//ss          // f.sTagText    = (DoTagOnly && d.sTagOnly.Length()>0) ? d.sTagOnly : d.sSymOrTag;
//ss          f.iType       = d.iType;
//ss          f.fIsData     = d.fIsData;
//ss          f.iLvl        = d.iLvl;
//ss          f.fIsTreeNd   = d.fIsTreeNd;
//ss          f.fUseComment = d.fHasComment;
//ss          f.iUseComment = iFld;
//ss          if (d.fIsData)
//ss            LD.DataLine=true;
//ss          }
//ss        }
//ss
//ss      LD.fDoingBlk=false;
//ss      }
//ss      break;
//ss    default:
//ss      LD.Loaded=false;
//ss    }
//ss  return LD.Loaded;
//ss  }
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::DeleteSpecSheet(const char * Name)
//ss  {
//ss  return SSDelete(Name);
//ss  };
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::LoadSpecSheet(const char * Name)
//ss  {
//ss  if (SSLoad(Name))
//ss    {
//ss    sSpecSheet=Name;
//ss    if (iSpecSheet>=0)
//ss      {
//ss      Data[iSpecSheet].sValue=Name;
//ss      Data[iSpecSheet].fIsModified=1;
//ss      }
//ss
//ss    ConnectFields();
//ss    MeasureFields();//, false);
//ss    DoRebuild();
//ss    return true;
//ss    }
//ss  return false;
//ss  };
//ss
//ss//---------------------------------------------------------------------------
//ss
//ssflag AccNodeWnd::SaveSpecSheet(const char * Name)
//ss  {
//ss  return SSSave(Name);
//ss  };
//ss
//ss//---------------------------------------------------------------------------
//ss/*
//ssvoid AccNodeWnd::DumpSpecSheet(char * Name)
//ss  {
//ss// NBNB use PrjBaseDirectory if Reqd
//ss  Strng Fn(PrjDirectory());
//ss  Fn+=ObjClassId;
//ss  Fn+=".SCS.Txt";
//ss  FILE * h=fopen(Fn(), "wt");
//ss  if (h)
//ss    {
//ss    fprintf(h, "Type\tFlags\tFieldName\tAccessTag\tFmt\tPrecision\tConversion\n");
//ss    fprintf(h, "[%s]\n", Name);
//ss    for (int i=0; i<NData; i++)
//ss      {
//ss      AccNdData &d=Data[i];
//ss      switch (d.iType)
//ss        {
//ss        case tt_RqdPage:
//ss        case tt_OptPage:
//ss          fprintf(h, "%s\t%s\t%s\t%s\t%c\t%i\t%s\n", d.iType==tt_OptPage ? "Po" : "Pr",
//ss                   "v", d.sSymOrTag(), "", ' ', 0, "");
//ss          break;
//ss        case tt_Text:
//ss          fprintf(h, "%s\t%s\t%s\t%s\t%c\t%i\t%s\n", "T", "v-", d.sSymOrTag() ? d.sSymOrTag() : "",
//ss                  "", d.Fmt.Type(), d.Fmt.Prec(), d.Cnv.Text() ? d.Cnv.Text() : "");
//ss          break;
//ss        default:
//ss          if (!IsStructure(d.iType))
//ss            fprintf(h, "%s\t%s\t%s\t%s\t%c\t%i\t%s\n", "D", "v", d.sSymOrTag(),
//ss                    d.sRefTag() ? d.sRefTag() : "", d.Fmt.Type(),
//ss                    d.Fmt.Prec(), d.Cnv.Text() ? d.Cnv.Text() : "");
//ss        }
//ss      }
//ss    fclose(h);
//ss    }
//ss  else
//ss    LogError("Access", 0, "SpecSheet file %s not opened", Fn());
//ss  };
//ss*/
#endif

//---------------------------------------------------------------------------

void AccNodeWnd::ConnectFields()
  {
  int i;
  for (i = 0; i < NFlds; i++)
    {
    AccNdFld  &f = Flds[i];
    AccNdData &d = Flds[i]();

    f.fIsTreeVis = true;

    if (!f.fIsTreeVis)
      {
      int xxx=0;
      }
    if (f.HasData())
      {
      f.fIsTreeVis = ((d.iFlags & DDEF_TREECLOSED)==0);//true;
      if (IsFloatData(f.iType))
        d.FormatFloat();

      if (d.pObjAttr)
        {
        Strng Tag;
        CnvAttribute NewCnvs;
        if (FullFldName(Tag, Data, f.iData))
          {
          DataAttribute* p=d.pObjAttr->FieldInList(Tag());
          if ((p==NULL) && ((d.iFlags & DDEF_HIDDEN)!=0))
            d.pObjAttr->SetFieldVisible(Tag(), d.iFlags, false);
          f.fIsTreeVis = d.pObjAttr->FieldVisible(Tag(), d.iFlags, f.fIsTreeVis);

          CCnvIndex iCnv=d.Cnv.Index();
          if (iCnv>0)
            {
            //if (iCnv>0 && !d.pObjAttr->ValidateCnv(Tag(), iCnv))
            //  LogWarning(d.sSymOrTag(), 0, "Unknown Conversion changed to '%s'", Cnvs[iCnv]->Txt());
            NewCnvs.SetText("");
            if (d.pObjAttr->FieldFmtCnvs(Tag(), d.Fmt, NewCnvs))
              {
              if (iCnv>0 && NewCnvs.TextLength()>0 && Cnvs[iCnv]->Find(NewCnvs.Text())==NULL)
                NewCnvs.SetText(""); //don't adjust
              d.AdjTextValue(NewCnvs.Text(), false);
              }
            }
		  else if (IsFloatData(f.iType))
		    {
            if (d.pObjAttr->FieldFmt(Tag(), d.Fmt))
              d.FormatFloat();
		    }
          }
        }
      }
    }

  #if dbgANW
  for (int ii=0; ii<NData; ii++)
    {
    AccNdData &f=Data[ii];
    Strng T1,T2,T3;
    T1.Set("%3i", f.iEndFld);
    T2.Set("%3i", f.iStrtFld);
    T3.Set(f.sTagComment.IsNotEmpty() ? "(%s)" : " ", f.sTagComment());
    dbgpln("ANW %3i %3.3s %3.3s %2i %2i  %-25s(%2i:%-8s) %-30s= %s", ii,
           (f.iEndFld>0) ? T1():"", (f.iStrtFld>0) ? T2():"",
           f.iType, f.iLvl, f.sSymOrTag(),  f.Cnv.Index(), f.Cnv.Text(), T3(),
           f.sValue.IsNotEmpty() ? f.sValue() : "");
    }
  #endif

  }

//---------------------------------------------------------------------------

void AccNodeWnd::MeasureFields()//, flag FullNames)
  {
  const int DataWide=10;

  bool InARow=false;
  NamW = 5;
  CnvW = 1;
  ValW = DataWide;
  MatW = DataWide;
  FldW=0;

  for (int i = 0; i < NFlds; i++)
    {
    rAccNdFld f=Flds[i];
    int NW=1;
    int CW=1;
    int VW=1;
    int MW=1;
    if (f.HasData())
      {
      AccNdData &d=Data[i];
      if (d.m_fIsRowStart)
        InARow=true;
      if (d.fIsData)
        {
        if (InARow)
          {
          //NW=f.sTagText.Length()+d.m_iChildLvl;
          //if (f.fUseComment && !d.fHasCheck)
          //  NW+=2+Data[f.iUseComment].sTagComment.Length();
          //CW=d.Cnv.TextLength()+2;
          MW=d.sValue.Length();
          //if (d.fIsStrng && d.fIsParam)
          //  VW=Max(VW, 20);
          }
        else
          {
          NW=f.sTagText.Length()+d.m_iChildLvl;
          if (f.fUseComment && !d.fHasCheck)
            NW+=2+Data[f.iUseComment].sTagComment.Length();
          CW=d.Cnv.TextLength()+2;
          VW=d.sValue.Length();
          if (d.fIsStrng && d.fIsParam)
            VW=Max(VW, 20);
          }
        }
      if (d.m_fIsRowsEnd)
        InARow=false;
      }
    NamW=Max(NamW, 1+NW);
    CnvW=Max(CnvW, CW);
    ValW=Max(ValW, VW);
    MatW=Max(MatW, MW);
    FldW=Max(FldW, NamW+ValW+CnvW);
    }

  for (int pg=0;pg<MaxFixedPages; pg++)
    ObjOnPage[pg]=-1;

  }


//---------------------------------------------------------------------------

void AccNodeWnd::ReFreshAttrs()
  {
  Strng Tag;
  for (int i = 0; i < NFlds; i++)
    {
    AccNdFld & f = Flds[i];
    AccNdData & d = Flds[i]();
    if (f.HasData() && d.pObjAttr)
      if (FullFldName(Tag, Data, f.iData))
        f.fIsTreeVis = d.pObjAttr->FieldVisible(Tag(), d.iFlags, f.fIsTreeVis);
    }
  }
             
//---------------------------------------------------------------------------

int AccNodeWnd::BuildStdPageHeader()
  {
  SetForce1Page(true);
  StartBlk(2, 0, NULL);
  int L=0;
  SetFixedPosition(true);
  SetSpace(L, 1);
  if (fAllFldsVis)
    SetSpace(L, 1);
  SetButton(L, "OK",      Id_SetBtn,      11, 0, " ");
  SetButton(L, "Cancel",  Id_CloseBtn,    9, 0, " ");
  SetButton(L, "Options", Id_Options,     9, 0, " ");
  //SetBitmapButton2(L, fAllFldsVis ? IDB_ACCALLFIELDSON_BTN : IDB_ACCALLFIELDSOFF, Id_AllBtn, 4, 0, " ");
  //SetButton(L, fAllFldsVis ? "Hide Fields" : "All Fields", Id_AllBtn, 10, 0, " ");
  SetButton(L, fAllFldsVis ? "VisFlds" : "AllFlds", Id_AllBtn, 8, 0, " ");
  SetBitmapButton2(L, GDIBlk::LowRes() ? IDB_ACCFIND_LOWRESBTN : IDB_ACCFIND_BTN, Id_FindBtn, 2, 0, " ");
  
  L++;
  SetSpace(L, 1);
  if (fAllFldsVis)
    SetSpace(L, 1);
  SetButton(L, "<",   Id_Prev,    2, 0, " ");
  //SetButton(L, "*",   Id_GetBtn,  3, 0, " ");
  SetBitmapButton2(L, GDIBlk::LowRes() ? IDB_ACCREFRESH_LOWRESBTN : IDB_ACCREFRESH_BTN, Id_GetBtn, 2, 0, " ");
  SetButton(L, ">",   Id_Next,    2, 0, " ");
  //SetButton(L, "-",   Id_GoToBtn, 2, 0, " ");
  SetBitmapButton2(L, GDIBlk::LowRes() ? IDB_ACCPREV_LOWRESBTN : IDB_ACCPREV_BTN, Id_GoToBtn, 2, 0, " ");
  SetParm(L, "", Id_GoTo, 18, 0);
  for (int i=0; i<NdHistList.GetSize(); i++)
    FldHasFixedStrValue(i, NdHistList[i]());
  SetSpace(L, 2);
  //SetBitmapButton2(L, IDB_ACCDISPTAGS_BTN, Id_TagOrSymBtn, 2, 0, " ");
  //SetButton(L, GetDisplayTagsOnly() ? "Symbols" : "Tags", Id_TagOrSymBtn, 10, 0, " ");
  SetButton(L, GetDisplayTagsOnly() ? "Tag>Sym" : "Sym>Tag", Id_TagOrSymBtn, 8, 0, " ");
  SetBitmapButton2(L, GDIBlk::LowRes() ? IDB_ACCCHANGETAG_LOWRESBTN : IDB_ACCCHANGETAG_BTN, Id_ChgTagBtn, 3, 0, " ");

  SetFixedPosition(false);

  return L+1;
  };

//---------------------------------------------------------------------------

void AccNodeWnd::Build(int NRows, int NCols, int ColIndent, pvoid BuildData)
  {
  Strng FldStr, ValStr;
  int  PgNo=-1,WdtAdj;
  int LnNo=0;
  RBActiveFld=-1;

  int LastLvl=256;
  flag WasObj=0;
  for (int di = NData-1; di>0; di--)
    {
    if (WasObj && Data[di].fIsStruct)
      Data[di].ObjOn=0;
    if (Data[di].fIsStruct)
      WasObj=1;
    else if (Data[di].fIsData)
      WasObj=0;
    }

  int FldDelta[MaxFixedPages];
  for (int i=0; i<MaxFixedPages; i++)
    FldDelta[i]  =-1000;

  Strng ObjRefTag;
  int  ObjRefFldNo=-1;
  flag NoPageYet=1;
  flag FirstFieldOnPage=0;
  flag NewObjRef=0;
  flag VisInTree[256];
  for (i=0; i<256; i++)
    VisInTree[i]=1;
  //int nFldsOnPage=0;
  int RqdPgLen=0;
  CRect CRect;
  GetClientRect(&CRect);
  int LinesVis=CRect.Height()/ScrGB.RowHgt();
  bool DoingMatrix=false;
  bool StartOfMatrix=false;
  bool WasDoingMatrix=false;
  int  iMatrixCol=0;

  for (int fi = 0; (fi < NFlds); fi++ )
    {
    //int i;
    AccNdFld & f=Flds[fi];
    AccNdData & d=f();//Data[fi];
    flag IsPage=false;
    
    if (d.m_fIsRowStart)
      {
      StartOfMatrix=!DoingMatrix;
      DoingMatrix=true;
      iMatrixCol=0;
      if (!StartOfMatrix)
        LnNo++;
      }

    if (dbgBld)
      dbgpln("%4i %4i %-10s %s %s %3i %s %s %s %s", 
              fi, LnNo, tt_TypeString(d.iType), 
              d.m_fIsRowStart?"S":" ", d.m_fIsRowsEnd?"E":" ", d.m_iMatrixCol, d.m_IsLastDataCol?"Last":"    ",   
              DoingMatrix?"Dm":"  ", StartOfMatrix?"Sm":"  ", d.sSymOrTag());
    switch (f.iType)
      {
      case tt_OptPage:
        {
        int LnsToNxtPg=0;
        if (f.iType==tt_OptPage)
          for (int nfi = fi+1;nfi < NFlds; nfi++)
            {
            AccNdFld & nf=Flds[nfi];
            if (nf.iType==tt_RqdPage || nf.iType==tt_OptPage)
              break;
            else 
              {
              flag ThisVisInTree=nf.fIsTreeVis || nf.fIsTreeNd;
              int iVisLvl=nf.iLvl;
              //while (iVisLvl>=0 && (ThisVisInTree=VisInTree[iVisLvl])!=0)
              while (iVisLvl>=0 && ThisVisInTree)
                ThisVisInTree=VisInTree[iVisLvl--];

              if (ThisVisInTree)
                {
                AccNdData &d = Data[nf.iData];
                if (d.m_iMatrixCol<0)
                  LnsToNxtPg++;
                else if (d.m_iMatrixCol==0 && IsData(d.iType))
                  LnsToNxtPg++;
                }
              }
            }

          IsPage=(NoPageYet || (LnNo+LnsToNxtPg > Max(MaxLinesOnPage, LinesVis)));
          //IsPage=(NoPageYet || (LnNo > MinLinesOnPage && LnNo+LnsToNxtPg > MaxLinesOnPage));
          break;
        }
      case tt_RqdPage:
        IsPage=true;
        break;
      }

    if ((IsPage && FirstFieldOnPage && !NoPageYet) ||
      (fi+1<NFlds && Flds[fi+1].iType==tt_Object && Flds[fi+1]().fIsEdtObj))
      IsPage=0;

    if (f.fIsTreeNd)
      for (int i=f.iLvl+1; i<256; i++)
        VisInTree[i]=f.fIsTreeVis;
    else if (f.iLvl<LastLvl)
      for (int i=f.iLvl; i<256; i++)
        VisInTree[i]=f.fIsTreeVis;
    LastLvl=f.iLvl;

    flag ThisVisInTree=f.fIsTreeVis || f.fIsTreeNd;
    int iVisLvl=f.iLvl;
    //while (iVisLvl>=0 && (ThisVisInTree=VisInTree[iVisLvl])!=0)
    while (iVisLvl>=0 && ThisVisInTree)
      ThisVisInTree=VisInTree[iVisLvl--];

    //dbgpln("Fld Typ:%-16s S:%i E:%i Pg:%i Lvl:%3i Vis:%i ThisVis:%i TNd:%i :%s",
    //       tt_TypeString(f.iType), f.iStrtFld, f.iEndFld, IsPage, f.iLvl, f.fIsTreeVis, ThisVisInTree || fAllFldsVis,
    //       f.fIsTreeNd, f.sDTag());

    if (IsPage)
      {
      //while (LnNo<RqdPgLen)
      //  SetSpace(LnNo++,50);
      NoPageYet=0;
      RqdPgLen=2;
      int l=0;
      for (int i = (IsPage ? fi+1 : fi); i < NFlds; i++)
        {
        AccNdFld & f=Flds[i];
        switch (f.iType)
          {
          case tt_Object:
          case tt_Struct:
          case tt_Element:
          case tt_Array:
            l++; RqdPgLen=Max(RqdPgLen, l); 
          case tt_RqdPage:
          case tt_OptPage:i=NFlds;                        break;
          case tt_Column: l=0;                            break;
          default:        l++; RqdPgLen=Max(RqdPgLen, l); break;
          }
        }

      // Extras 
      RqdPgLen+=5;
      PgNo++;
      //StartPage(IsPage ? f.sTagText() : "Page 1", false);
      //TODO:Rather than ObjClassId use a better short type description!!!???
      Strng NextPageTab;
      if (PgNo==0)
        StartPage(IsPage ? ObjClassId() : "Page 1", false);
      else if (PgNo<3 && strnicmp(f.sTagText(), NdTag(), NdTag.Len())==0)
        {
        NextPageTab = ObjClassId();
        NextPageTab += "..";
        StartPage(NextPageTab(), false);
        }
      else
        StartPage(f.sTagText(), false);

      StartBlk(RqdPgLen, 0, NULL);
      LnNo=0;
      FirstFieldOnPage=1;
      }

    if (d.m_iChildLvl)
      {
      int xxx=0;
      }

    //dbgpln("                                              XXXX");

    if (((f.fIsData || f.iType==tt_Text) && (NewObjRef || FirstFieldOnPage/* || DoingMatrix*/)))
      {
      flag ObjTagVisInTree=ThisVisInTree || fAllFldsVis;
      if (ObjRefFldNo>=0 && Flds[ObjRefFldNo].fIsTreeNd)
        {
        int iVisLvl=Flds[ObjRefFldNo].iLvl;
        while (iVisLvl>=0 && (ObjTagVisInTree=VisInTree[iVisLvl])!=0)
          iVisLvl--;
        }


      if (ObjTagVisInTree)
        {
        if (dbgBld)
          dbgpln("                                              %s %s %i", 
                   DoingMatrix?"Dm":"  ", WasDoingMatrix?"WasDm":"     ", iMatrixCol);
        if (DoingMatrix)
          {
          //if (iMatrixCol==0)
          //  {
          //  //Strng T(ObjRefTag());
          //  //if (ObjRefFldNo>=0 && Flds[ObjRefFldNo].fUseComment)
          //  //  AppendTagComment(T, Flds[ObjRefFldNo]().sTagComment);
          //  //T+="...";
          //  //int FldNo=0;
          //  //if (ObjRefFldNo>=0 && Flds[ObjRefFldNo].fIsTreeNd)
          //  //  {
          //  //  SetTreeNd(LnNo, !Flds[ObjRefFldNo].fIsTreeVis,  Id_TreeNdBtn+ObjRefFldNo, 1, 0, "");
          //  //  FldNo=ObjRefFldNo;
          //  //  ObjRefFldNo=-1;
          //  //  }
          //  //else
          //  SetSpace(LnNo,1);
          //  SetDesc(LnNo," ",Id_ViewIndex,fAllFldsVis ? 3 : 2,0,"");
          //  SetDesc(LnNo,"",-1,NamW/*+ValW+CnvW*/,0," ");

          //  //FirstFieldOnPage=0;
          //  //NewObjRef=0;
          //  //LnNo++;
          //  }
          }
        else
          {
          if (WasDoingMatrix)
            {
            if (dbgBld)
              dbgpln("                                              ADD Blank ");
            SetSpace(LnNo,1);
            SetDesc(LnNo," ",Id_ViewIndex,fAllFldsVis ? 3 : 2,0,"");
            SetDesc(LnNo,"",-1,NamW,0," ");
            }
          else
            {
            if (dbgBld)
              dbgpln("                                              ADD %s ... ", ObjRefTag());
            Strng T(ObjRefTag());
            if (ObjRefFldNo>=0 && Flds[ObjRefFldNo].fUseComment)
              AppendTagComment(T, Flds[ObjRefFldNo]().sTagComment);
            T+="...";
            int FldNo=0;
            if (ObjRefFldNo>=0 && Flds[ObjRefFldNo].fIsTreeNd)
              {
              SetTreeNd(LnNo, !Flds[ObjRefFldNo].fIsTreeVis,  Id_TreeNdBtn+ObjRefFldNo, 1, 0, "");
              FldNo=ObjRefFldNo;
              ObjRefFldNo=-1;
              }
            else
              SetSpace(LnNo,1);
            SetDesc(LnNo," ",Id_ViewIndex,fAllFldsVis ? 3 : 2,0,"");
            SetDesc(LnNo,T(),-((int)Id_FldParms+FldNo),NamW+ValW+CnvW,0," ");
            }

          FirstFieldOnPage=0;
          NewObjRef=0;
          LnNo++;
          }
        WasDoingMatrix=DoingMatrix;
        }
      }

    switch (f.iType)
      {
      case tt_Column:
        LnNo=1;
        break;
      case tt_Char:
      case tt_Bool:
      case tt_Bit:
      case tt_Byte:
      case tt_Word:
      case tt_DWord:
      case tt_Int:
      case tt_Short:
      case tt_Long:
      case tt_Float:
      case tt_Double:
      case tt_pChar:
      case tt_ppChar:
      case tt_Strng:
        {
        if (!ThisVisInTree && !fAllFldsVis)
          break;
        ValStr=d.sValue;
        WdtAdj=0;
        int VW=ValW, XVW=0;
        if (DoingMatrix)
          {
          if (VW>MATRIXWIDTH)
            {
            XVW=VW-MATRIXWIDTH;
            VW=MATRIXWIDTH;
            }
          }
        else
          {
          if (VW>BASICWIDTH)
            {
            XVW=VW-BASICWIDTH;
            VW=BASICWIDTH;
            }
          }

#if CHECKBOXESRIGHT
        if (d.fHasCheck)
          {
          WdtAdj=Max(0,VW-2); 
          }
        else
#endif
          if (d.ValLst.Length()==0)
            {
            switch (f.iType)
              {
              case tt_Bool   :
              case tt_Bit    : if (SHORTFLDS) WdtAdj=Max(0,VW-3); break;
              case tt_Byte   :
              case tt_Char   : if (SHORTFLDS) WdtAdj=Max(0,VW-5); break;
              case tt_Word   :
              case tt_Short  : if (SHORTFLDS) WdtAdj=Max(0,VW-7); break;
              case tt_DWord  :
              case tt_Long   : if (SHORTFLDS) WdtAdj=Max(0,VW-12); break;
              case tt_Double :
              case tt_Float  : if (SHORTFLDS) WdtAdj=Max(0,VW-14); break;
              case tt_pChar  :
              case tt_ppChar :
                //            case tt_Strng  : WdtAdj=0; VW=30; break;
              case tt_Strng  : WdtAdj=0; VW=Range(20, ValStr.Length(), 45); break;
              default        : WdtAdj=0; break;
              }
            }
          else // if (d.ValLst.Length()>0)
            {
            VW=BASICWIDTH;
            WdtAdj=0;
            for (pStrng p=d.ValLst.First(); p; p=p->Next())
              VW=Max(VW, p->Length()+2);
            }

          if (WdtAdj>0)
            {
            ValStr.Trim();
            ValStr.RPad(VW+WdtAdj);
            }

          //Strng NameBuff(d.sDesc.Length()>0?"?":" ");
          Strng NameBuff;//(" ");
          NameBuff.Set(" %*.*s", d.m_iChildLvl,d.m_iChildLvl, ".......");
          if (d.fHasCheck)
            {
            NameBuff+=f.sTagText;
            int xxx=0;
            }
          else if (d.fHasButton)
            {
            if (d.ValLst.Length()>0)
              NameBuff+=f.sTagText;
            else
              NameBuff=f.sTagText;
            }
          else
            NameBuff+=f.sTagText;
          Strng TagComStr;
          if (f.fUseComment)
            {
            AppendTagComment(TagComStr, Data[f.iUseComment].sTagComment);
            if (strstr(TagComStr(), "ExoTh"))
              { int xxx=0;}
            }
          int TagComLen=TagComStr.GetLength();

          //if (StartOfMatrix && iMatrixCol==0)
          //  {
          //  if (f.fIsTreeNd)
          //    SetTreeNd(LnNo, !f.fIsTreeVis,  Id_TreeNdBtn+fi, 1, 0, "");
          //  else
          //    SetSpace(LnNo,1);
          //  SetDesc(LnNo,"  ",Id_ViewIndex,fAllFldsVis ? 3 : 2,0,"");

          //  Strng T(f.sTagText());
          //  if (T.IsNotEmpty() && f.fPadDash)
          //    {
          //    char c=T[0];
          //    c=isalnum(c)!=0 ? '-' : c;
          //    T.LRPad(f.sTagText.Length()+2, ' '); // Force 1 Space on Each Side
          //    T.LRPad(f.sTagText.Length()+2, c); // Force C On on Each Side
          //    T.LRPad((int)NamW, c);
          //    }
          //  f.ValFld=SetDesc(LnNo,T(),-((int)Id_FldParms+fi),FldW,3," ");
          //  f.ValFld->SetItalic();
          //  LnNo++;
          //  FirstFieldOnPage=0;
          //  }
          
          if (!DoingMatrix || iMatrixCol==0)
            {
            if (0 && d.m_fNxtIsChild)
              SetTreeNd(LnNo, !f.fIsTreeVis,  Id_TreeNdBtn+fi, 1, 0, "");
            else
              SetSpace(LnNo,1);//+d.m_iChildLvl*2);

            SetDesc(LnNo,"  ",Id_ViewIndex,fAllFldsVis ? 3 : 2,0,"");
            //SetSpace(LnNo,d.m_iChildLvl);
            }

          if (d.fHasCheck)
            {
#if CHECKBOXESRIGHT
            int DWidth = NamW-TagComLen+WdtAdj;
            int VWidth = VW-WdtAdj;
#else
            int DWidth = NamW-TagComLen;
            int VWidth = 2;
#endif
            FxdEdtFld* pFld=SetDesc(LnNo,NameBuff(), -(Id_FldParms+fi), DWidth, 3, TagComStr());
            pFld->fUnderLine=true;
            if (d.sFullTag())
              SetTag(d.sFullTag());
            SetMarkerFlags(d.m_Markers);

            if (d.fHasButton)
              f.ValFld=SetCheckBoxBtn(LnNo, ValStr(),  Id_FldBtns+fi, VWidth, 0, "", d.fIsParam);
            else
              f.ValFld=SetCheckBox(LnNo, ValStr(),  Id_FldBtns+fi, VWidth, 0, "", d.fIsParam);
            }
          else if (d.fHasButton)
            {
            if (d.ValLst.Length()>0)
              {
              SetDesc(LnNo, NameBuff(),Id_FldBtns+fi,NamW-TagComLen, 3,TagComStr());
              if (d.sFullTag())
                SetTag(d.sFullTag());
              pStrng p=d.ValLst.First(); 
              f.ValFld=SetButton(LnNo, p->Str(),  Id_FldBtns+fi, Min(NamW+ValW, Max(BASICWIDTH, NameBuff.Length()+2)), 0, "");
              //            f.ValFld=SetButton(LnNo, p->Str(),  Id_FldBtns+fi, Max(ValW, 14), 0, "");
              }
            else
              {
              Strng Nm(" ");
              if (d.sTagOnly.GetLength()>0)
                Nm+=d.sTagOnly();
              else
                Nm+=NameBuff();
              SetDesc(LnNo, Nm(),Id_FldBtns+fi,NamW-TagComLen, 3,TagComStr());
              if (d.sFullTag())
                SetTag(d.sFullTag());
              f.ValFld=SetButton(LnNo, NameBuff(),  Id_FldBtns+fi, Min(NamW+ValW, Max(BASICWIDTH, NameBuff.Length()+2)), 0, "");
              }
            }
          else if (d.fIsParam)
            {
            if (DoingMatrix)
              {
              if (iMatrixCol==0)
                SetDesc(LnNo, "", -1, NamW+WdtAdj, 3, "");
              }
            else
              {
              FxdEdtFld* pFld=SetDesc(LnNo, NameBuff(), -(Id_FldParms+fi), NamW+WdtAdj-TagComLen, 3, TagComStr());
              pFld->fUnderLine=true;
              if (d.sFullTag())
                SetTag(d.sFullTag(), d.Cnv.Text());

              SetMarkerFlags(d.m_Markers);
              }

            if (dbgBld)
              dbgpln("                                              %3i %3i '%s' ", VW, WdtAdj, ValStr());

            const flag HasTag = ((d.iFlags & DDEF_TAGPARM) && d.sValue.Len()>0);
            //char*pSep=DoingMatrix?"":d.fHasNANStr?"  ":" ";
            char*pSep=!DoingMatrix || d.m_IsLastDataCol?"  ":"";
            f.ValFld=SetData(LnNo, ValStr(), Id_FldParms+fi, VW-WdtAdj, (d.fIsStrng && !IsTimeCnv(d.Cnv.Index()))?0:2, pSep);
            if (d.iFlags & DDEF_FUNCTPARM)
              {
              SetBigStrLength(1024);
              }

            if (d.ValLst.Length()>0)
              {
              if (IsFloatData(d.iType) && d.fHasNANStr)
                {
                for (pStrng p=d.ValLst.First(); p; p=p->Next())
                  FldHasNANStrValue(p->Index(), p->Str());
                }
              else
                {
                for (pStrng p=d.ValLst.First(); p; p=p->Next())
                  FldHasFixedStrValue(p->Index(), p->Str());
                }
              }

            if (HasTag)
              {
              if (GDIBlk::LowRes())
                {
                SetBitmapButton1(LnNo, IDB_ACCFIND_LOWRES, Id_FldBtns+fi, 2, 0, "");
                SetBitmapButton1(LnNo, IDB_ACCACCESS_LOWRES, Id_FldBtns+fi, 2, 0, "");
                }
              else
                {
                //SetBitmapButton2(LnNo, IDB_ACCFIND_BTN, Id_FldBtns+fi, 2, 0, "");
                //SetBitmapButton2(LnNo, IDB_ACCACCESS_BTN, Id_FldBtns+fi, 2, 0, "");
                SetBitmapButton1(LnNo, IDB_ACCFIND, Id_FldBtns+fi, 2, 0, "");
                SetBitmapButton1(LnNo, IDB_ACCACCESS_LOWRES, Id_FldBtns+fi, 2, 0, "");
                }
              }
            if (!DoingMatrix || d.m_IsLastDataCol)
              SetDesc(LnNo, "", Id_FldCnvs+fi, 20, 0);          
            }
          else if (d.fMultiStrng)
            {
            FxdEdtFld* pFld=SetDesc(LnNo, NameBuff(), -(Id_FldParms+fi), NamW+WdtAdj-TagComLen, 3, TagComStr());
            pFld->fUnderLine=true;
            if (d.sFullTag())
              SetTag(d.sFullTag());

            SetMarkerFlags(d.m_Markers);

            f.ValFld=SetData(LnNo, ValStr(), Id_FldParms+fi, VW-WdtAdj, (d.fIsStrng && !IsTimeCnv(d.Cnv.Index()))?0:2, "  ", false);
            if (d.ValLst.Length()>0)
              for (pStrng p=d.ValLst.First(); p; p=p->Next())
                FldHasFixedStrValue(p->Index(), p->Str());
            SetDesc(LnNo, "", Id_FldCnvs+fi, 20,0);
            }
          else
            {
            if (!DoingMatrix)
              {
              FxdEdtFld* pFld=SetDesc(LnNo, NameBuff(), -(Id_FldParms+fi), NamW+WdtAdj-TagComLen, 3, TagComStr());
              pFld->fUnderLine=true;
              if (d.sFullTag())
                SetTag(d.sFullTag(), d.Cnv.Text());

              SetMarkerFlags(d.m_Markers);
              }
            const flag HasTag = ((d.iFlags & DDEF_TAGPARM) && d.sValue.Len()>0);
            //char*pSep=DoingMatrix?"":" ";
            char*pSep=!DoingMatrix || d.m_IsLastDataCol?"  ":"";
            f.ValFld=SetData(LnNo, ValStr(), Id_FldParms+fi, VW-WdtAdj, (d.fIsStrng && !IsTimeCnv(d.Cnv.Index()))?0:2, pSep, false);

            if (HasTag)
              {
              if (GDIBlk::LowRes())
                {
                SetBitmapButton1(LnNo, IDB_ACCFIND_LOWRES, Id_FldBtns+fi, 2, 0, "");
                SetBitmapButton1(LnNo, IDB_ACCACCESS_LOWRES, Id_FldBtns+fi, 2, 0, "");
                }
              else
                {
                //SetBitmapButton2(LnNo, IDB_ACCFIND_BTN, Id_FldBtns+fi, 2, 0, "");
                //SetBitmapButton2(LnNo, IDB_ACCACCESS_BTN, Id_FldBtns+fi, 2, 0, "");
                SetBitmapButton1(LnNo, IDB_ACCFIND, Id_FldBtns+fi, 2, 0, "");
                SetBitmapButton1(LnNo, IDB_ACCACCESS_LOWRES, Id_FldBtns+fi, 2, 0, "");
                }
              }
            if (!DoingMatrix)
              SetDesc(LnNo, "", Id_FldCnvs+fi, 20,0);
            }

          if (d.sFullTag())
            SetTag(d.sFullTag(), d.Cnv.Text());
          if (!d.fHasButton || d.fHasCheck || (d.ValLst.Length()>0))
            {
            NameBuff.Trim();
            FldDelta[PgNo]=Max(FldDelta[PgNo], (NameBuff.Length()+TagComLen+2)-NamW);
            }

          if (!DoingMatrix)
            LnNo++;
          FirstFieldOnPage=0;
          break;
        }
      case tt_Text:
        {
        if (!ThisVisInTree && !fAllFldsVis)
          break;
        if (f.fIsTreeNd)
          SetTreeNd(LnNo, !f.fIsTreeVis,  Id_TreeNdBtn+fi, 1, 0, "");
        else
          SetSpace(LnNo,1);
        SetDesc(LnNo,"  ",Id_ViewIndex,fAllFldsVis ? 3 : 2,0,"");

        Strng T(f.sTagText());
        if (T.IsNotEmpty() && f.fPadDash)
          {
          char c=T[0];
          c=isalnum(c)!=0 ? '-' : c;
          T.LRPad(f.sTagText.Length()+2, ' '); // Force 1 Space on Each Side
          T.LRPad(f.sTagText.Length()+2, c); // Force C On on Each Side
          T.LRPad((int)NamW, c);
          }
        f.ValFld=SetDesc(LnNo,T(),-((int)Id_FldParms+fi),FldW,3," ");
        f.ValFld->SetItalic();
        LnNo++;
        FirstFieldOnPage=0;
        break;
        }
      case tt_RqdPage:
      case tt_OptPage:
        break;
      case tt_Object:
        if (d.fIsEdtObj)
          {
          ObjEdit[d.iObjNo]->Build();
          do
          ObjOnPage[++PgNo]=d.iObjNo;
          while (PgNo<Pages-1);
          //WasObjPg=1;
          }
        // NB No break;
      case tt_Object_E:
      case tt_Struct:
      case tt_Struct_E:
      case tt_Element:
      case tt_Element_E:
      case tt_Array:
      case tt_Array_E:
        if (d.ObjOn)
          {
          ObjRefFldNo=fi;
          int di=Flds[fi].iData;
          ObjRefTag="";//NdTag(); dont even put Unit Tag in Desc
          for (int i=1; i <= di; i++) // start at 1 to skip  UnitTag
            if (Data[i].iEndFld>0)
              if (Data[i].iEndFld>fi)
                {
                if (Data[i].fIsStruct)
                  {
                  if (ObjRefTag.IsNotEmpty())
                    ObjRefTag+=".";
                  ObjRefTag+=Data[i].sSymOrTag();
                  }
                }
              else
                i=Data[i].iEndFld;
          NewObjRef=1;
          }
        break;
      case tt_NULL: break;
      }
    if (DoingMatrix && f.fIsData)
      iMatrixCol++;
    if (d.m_fIsRowsEnd)
      {
      DoingMatrix=false;
      LnNo++;
      }
    StartOfMatrix=false;
    }

  if (1)
    {
    for (i=0; i<MaxFixedPages; i++)
      {
      if (FldDelta[i]>-1000 && Pg[i])
        {
        int fno=2;
        for (int r=2; r<Pg[i]->Rows; r++)
          AdjustFld(i, r, fno, FldDelta[i]);
        }
      }
    }

  FxdEdtView::Build(0, 0, 0);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::ChangeLayout(int TotalRows, int TotalCols)
  {

  for (int p=0; p<Pages; p++)
    if (ObjOnPage[p]>=0)
      ObjEdit[ObjOnPage[p]]->ChangeLayout(*(Pg[p]), TotalRows, TotalCols);
  };

//---------------------------------------------------------------------------

void AccNodeWnd::PageChanged(int NewPageNo, flag Initial)
  {
  if (!Initial && ObjClassId.IsNotEmpty())
    {
    //SaveCurrentPageInfo();
//      CProfINIFile PF(PrjIniFile());
//    PF.WrInt("AccessPages", ObjClassId(), CPgNo);
    }
  }

//---------------------------------------------------------------------------

void AccNodeWnd::Load(FxdEdtInfo &EI, Strng & Str)
  {
  int p=EI.PageNo;
  if (CurrentBlk(EI))
    { // Header
    switch (EI.FieldId)
      {
      //case Id_Next:
      //case Id_Prev:
      //case Id_GoToBtn:
      case Id_GoTo:
        Str="Go To ...";
        EI.Fld->fEditable=false;
        EI.Fld->fIsResult=false;
        break;
      default: 
        break;
      }
    }
  else if (ObjOnPage[p]>=0)
    {
    ObjEdit[ObjOnPage[p]]->Load(EI, Str);
    }
  else if (CurrentBlk(EI))
    {
    int f=EI.FieldId-Id_FldParms;
    if (f<NFlds)
      {
      //ASSERT_ALWAYS(f>=0, "Bad Access field Index !!", __FILE__, __LINE__);
      if (f>=0 && Flds[f].HasData())
        {
        Str=Flds[f]().sValue();
        //char * pc=Flds[f]().sValue();
        //Str= pc ? pc : "";
        EI.Fld->fInError=((Flds[f]().iFlags & DDEF_INERROR)!=0);
        EI.Fld->fIsResult=((Flds[f]().iFlags & DDEF_RESULT)!=0);
        EI.Fld->SetMarks(Flds[f]().m_Markers);
        }
      else
        Str="";
      }
    else
      {
      int f=EI.FieldId-Id_FldCnvs;
      if (f<NFlds)
        {
        //ASSERT_ALWAYS(f>=0, "Bad Access field Index !!", __FILE__, __LINE__);
        Str=" ";
        if (f>=0 && Flds[f].HasData())
          {
          AccNdData & d=Flds[f]();
          if (IsFloatData(d.iType) || (IsStrng(d.iType) && IsTimeCnv(d.Cnv.Index())))// && Valid(d.dVal))
            {
            CDataCnv & C=*Cnvs[d.Cnv.Index()];
            if (C.OverTxt())
              Str+=C.OverTxt();
            else if (d.Cnv.Text())
              Str+=d.Cnv.Text();
            }
          }
        else
          Str="";
        }
      }
    }
  };

//---------------------------------------------------------------------------

long AccNodeWnd::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  int i=0;
  int p=EI.PageNo;
  int fi=EI.FieldId-Id_FldParms;
  if (CurrentBlk(EI))
    {
    int p=EI.PageNo;
    switch (EI.FieldId)
      {
      case Id_GoTo:
        {
        // Not part of object
        EI.Fld->Changed=false;

        char* pTxt = new char[Str.Len()+1];
        strcpy(pTxt, Str());
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS_NOERRDLG, (LPARAM)pTxt);
        break;
        }
      default: 
        break;
      }
    // Header
    }
  else if (ObjOnPage[p]>=0)
    {
    ObjEdit[ObjOnPage[p]]->Parse(EI, Str);
    ObjChgd[ObjOnPage[p]]=1;
    // CNM Crashes in XY Fns
    //if (Flds[fi].HasData())
    //  Flds[fi]().fIsModified=1;
    }
  else if (CurrentBlk(EI))
    {
    ASSERT(fi>=0);
    if (fi<NFlds && Flds[fi].HasData())
      {
      AccNdFld & f=Flds[fi];
      AccNdData & d=f();
      //char B[256];
      if (d.fIndexedStr)
        d.sValue=Str;
      else
        switch(d.iType)
          {
          case tt_pChar     : d.sValue=Str; break;
          case tt_ppChar    : d.sValue=Str; break;
          case tt_Strng     : d.sValue=Str; break;
          //case tt_ppChar    : Flds[f].sValue=Str; break;
          case tt_Object    : break;
          case tt_Struct    : break;
          case tt_Element   : break;
          case tt_Array     : break;
          case tt_NULL      : break;
          default:
            if (IsNumData(f.iType))
              {
              bool GotIt=false;

              int L=Str.Length();
              if (L>0 && isalpha(Str[0]) && IsFloatData(f.iType) && d.fHasNANOK && d.fHasNANStr)
                {
                for (Strng *p=d.ValLst.First(); p; p=p->Next())
                  {
                  if (Str.XStrNICmp(p->Str(), L)==0)
                    {
                    d.dVal=SetNANFlagShft(p->Index());
                    d.FormatFloat();//f.dVal);
                    GotIt=true;
                    break;
                    }
                  }
                }
              if (!GotIt)
                {
                PkDataUnion DU;
                DU.SetTypeString(d.iType, Str(), d.Cnv.Index(), d.Cnv.Text());
                if (IsFloatData(d.iType))
                  {
                  d.dVal=DU.GetDouble(d.Cnv.Index(), d.Cnv.Text());
                  d.FormatFloat();//f.dVal);
                  }
                else
                  d.sValue = DU.GetString("%i", "%g", d.Cnv.Index(), d.Cnv.Text());
                }
              }
            break;
          }
      d.fIsModified=1;
      if (d.fHasSetOnChange)
        {
        if (CloseAccessData(true, false, false))
          {
          ClearFlds(true);

          LoadAccessData();
          MeasureFields();
          DoRebuild();
          }
        }
      }
    }
  return 0;
  };

//---------------------------------------------------------------------------

long AccNodeWnd::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  int p=EI.PageNo;
  if (CurrentBlk(EI))
    { // Header
    int p=EI.PageNo;
    switch (EI.FieldId)
      {
      case Id_Next:
      case Id_Prev:
        {
        m_pMyFrm->SetWindowText("Access ...");
        flag DidPost = false;
        // Not part of object
        EI.Fld->Changed=false;

        CSArray Tags;
        CSArray DescTags;
        Strng Tg;
        RequestModelIOConnRec ConnInfo;
        RequestModelIOInfoRec IOInfo;
        for (int i=0; gs_pTheSFELib->RequestModelIOConn(NdTag(), i, ConnInfo); i++)
          if (!ConnInfo.fIsDirectConnect && ((ConnInfo.iDirn<0 && (EI.FieldId==Id_Next)) ||
                                           (ConnInfo.iDirn>0 && (EI.FieldId==Id_Prev))))
            {
            if (gs_pTheSFELib->RequestModelIOInfoById(NdTag(), ConnInfo.iId, IOInfo))
              Tg.Set("%s : %s", IOInfo.Name(), ConnInfo.Tag());
            else
              Tg=ConnInfo.Tag();
            DescTags.AddStrng(Tg());
            Tags.AddStrng(ConnInfo.Tag());
            }
        if (Tags.GetSize()>0)
          {
          int iIO=0;
          if (Tags.GetSize()>1)
            {
            CMenu Menu;
            Menu.CreatePopupMenu();

            for (int i=0; i<Tags.GetSize(); i++)
              Menu.AppendMenu(MF_STRING, IDM_DSTIO_0+i,  DescTags[i]());

            POINT ScreenPoint = {(int)EI.Fld->Xe(ScrGB), (int)EI.Fld->Ye(ScrGB)};// = point;
            ClientToScreen(&ScreenPoint);
            CRect ClickRect(0,0,2048,2048);
            int RetCd=Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD, 
              ScreenPoint.x, ScreenPoint.y, this, &ClickRect);
            if (RetCd)
              iIO=RetCd-IDM_DSTIO_0;
            else
              iIO=-1;
            Menu.DestroyMenu();
            }
          if (iIO>=0)
            {
            char* pTxt=new char[Tags[iIO].Length()+1];
            strcpy(pTxt, Tags[iIO]());
            ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS_NOERRDLG, (LPARAM)pTxt);
            DidPost = true;
            }
          }
        if (!DidPost)
          ShowDefWndTitle();
        break;
        }
      case Id_GoToBtn: //access most recent/previous in history
        if (NdHistList.GetSize()>1)
          {
          m_pMyFrm->SetWindowText("Access ...");
          char* pTxt = new char[NdHistList[1].Len()+1];
          strcpy(pTxt, NdHistList[1]());
          ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS_NOERRDLG, (LPARAM)pTxt);
          }
        break;
      case Id_SetBtn:
      case Id_GetBtn:
        {
        CWaitCursor Wait;
        WndTitle.Set("%s - %s", EI.FieldId==Id_SetBtn ? "Saving" : "Loading", NdTag());
        m_pMyFrm->SetWindowText(WndTitle());
        flag DoSaveIt = (EI.FieldId==Id_SetBtn);
        if (0)
          {//check for changes and save when "refresh" button/option selected...
          if (EI.FieldId==Id_GetBtn && AnyFldsOrDataChanged())
            {
            if (gs_pPrj && gs_pPrj->m_bAccAutoSaveOn)
              DoSaveIt = true;
            else
              {
              Strng Msg;
              Msg.Set(" Save changes made in access window for '%s' ? ", NdTag());
              if (AfxMessageBox(Msg(), MB_ICONQUESTION|MB_YESNO)==IDYES)
                DoSaveIt = true;
              }
            }
          }
        if (CloseAccessData(DoSaveIt, false, false))
          {
          ClearFlds(DoSaveIt);//EI.FieldId==Id_SetBtn);

          LoadAccessData();
          MeasureFields();//, false);
          DoRebuild();
          }
        ShowDefWndTitle();
        break;
        }
      case Id_CloseBtn:
        CloseAccessData(false, fLastSaveInvalid, true);
        CloseWnd();
        break;
      case Id_AllBtn:
        //OnAccOptions(IDM_ACC_ALLFIELDS); //if called directly the rebuild destroys current context
        PostMessage(WM_COMMAND, (WPARAM)IDM_ACC_ALLFIELDS, (LPARAM)0);
        break;
      case Id_TagOrSymBtn:
        m_pMyFrm->SetWindowText("Access ...");
        //OnAccOptions(IDM_ACC_DISPLAYTAGS); //if called directly the rebuild destroys current context
        PostMessage(WM_COMMAND, (WPARAM)IDM_ACC_DISPLAYTAGS, (LPARAM)0);
        break;
      case Id_ChgTagBtn:
        OnAccOptions(IDM_ACC_CHANGETAG);
        break;
      case Id_FindBtn:
        sLastSelected[0] = NdTag();
        OnAccMenu(IDM_ACC_FINDTAG_0);
        break;
      case Id_PrintBtn:
        OnFilePrint();
        break;
#if WITHSPECSHEETS
//ss      case Id_SpecShtBtn:
//ss        {
//ss        CSpecSheet SpecSheet;
//ss        SpecSheet.DoModal();
//ss        break;
//ss        }
#endif
      case Id_Options:
        {
        //RBPoint
        CRect WRect;
        GetWindowRect(&WRect);
        CRect CRect;
        GetClientRect(&CRect);
  
        //RBPoint.x = WRect.left+CRect.left+ScrGB.ColWdt()*EI.Fld->Col1;
        //RBPoint.y = WRect.top+CRect.top+ScrGB.RowHgt()*(EI.Fld->pRow->RowNo+TabIndexRows+2);
        RBPoint.x = WRect.left+CRect.left+EI.Fld->Xs(ScrGB);
        RBPoint.y = WRect.top+CRect.top+EI.Fld->Ye(ScrGB)+ScrGB.RowHgt();

        CMenu Menu;
        Menu.CreatePopupMenu();

        Menu.AppendMenu(MF_STRING, IDM_ACC_ALLFIELDS,     "&All Fields");
        Menu.AppendMenu(MF_STRING, IDM_ACC_DISPLAYTAGS,   "&Display Tags");
        Menu.AppendMenu(MF_STRING, IDM_ACC_HIDEIFZERO,    "Hide &Zeros");
        Menu.AppendMenu(MF_STRING, IDM_ACC_SHOWGROUPS,    "Show Groups");
        Menu.AppendMenu(MF_STRING, IDM_ACC_AUTOSAVE,      "A&uto Save");
        Menu.AppendMenu(MF_STRING, IDM_ACC_UPDATEONRUN,   "Update on &Run");
        Menu.AppendMenu(MF_STRING, IDM_ACC_HISTORYINFO,   "&Historian/Archive Status");
        FmtAttribute::SetCmdIndex(IDM_ACC_FMT1);
        fGlobalFmtMenu=1;
        CMenu FormatMenu2;
        FormatMenu2.CreatePopupMenu();
        GlblFmt.AddToMenu(FormatMenu2);
        Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu2.m_hMenu, "Global Default Format");
        Menu.AppendMenu(MF_SEPARATOR);
#if WITHSPECSHEETS
//ss        Menu.AppendMenu(MF_STRING, IDM_ACC_SPECSHEET,   "&Spec Sheet ...");
#endif
        Menu.AppendMenu(MF_STRING, IDM_ACC_PRINT,         "&Print ...");
#if WITHDOCUMENTATIONLINK
        Menu.AppendMenu(MF_STRING, IDM_ACC_DOCUMENT,      "Docu&ment ...");
#endif
        Menu.AppendMenu(MF_STRING, IDM_ACC_CHANGETAG,     "&Change Tag ...");
        Menu.AppendMenu(MF_STRING, IDM_ACC_COPYBLK_0,     "Data &Transfer ...");
        Menu.AppendMenu(MF_STRING, IDM_ACC_QUICKVIEW,     "&Quick View ...");
        Menu.AppendMenu(MF_SEPARATOR);
        Menu.AppendMenu(MF_STRING|(DefNetProbalMode()?0:MF_GRAYED), IDM_ACC_PBINITNODE,  "&Initialise");
        Menu.AppendMenu(MF_STRING, IDM_ACC_EMPTYNODE,     "&Empty");
        Menu.AppendMenu(MF_STRING, IDM_ACC_ZEROFLOWSNODE, "&Zero Flows");
        Menu.AppendMenu(MF_STRING|(DefNetDynamicMode()?0:MF_GRAYED), IDM_ACC_PRESETNODE, "P&reset");
        Menu.AppendMenu(MF_STRING/*|(TaggedObject::NetDynamicMethod()?0:MF_GRAYED)*/, IDM_ACC_RESETSTATSNODE, "Reset &Stats");
        //Menu.AppendMenu(MF_STRING, IDM_ACC_PASTEBLK_0,  "Paste &Block ...");
        
        //Menu.AppendMenu(MF_POPUP, (unsigned int)CopyMenu.m_hMenu, "&Copy Block");
        //Menu.AppendMenu(MF_POPUP, (unsigned int)PasteMenu.m_hMenu, "&Paste Block");

        if (fAllFldsVis)
          Menu.CheckMenuItem(IDM_ACC_ALLFIELDS, MF_BYCOMMAND|MF_CHECKED);
        if (GetDisplayTagsOnly())
          Menu.CheckMenuItem(IDM_ACC_DISPLAYTAGS, MF_BYCOMMAND|MF_CHECKED);
        if (GetHideZeros())
          Menu.CheckMenuItem(IDM_ACC_HIDEIFZERO, MF_BYCOMMAND|MF_CHECKED);
        if (SVI.GetShowGroups())
          Menu.CheckMenuItem(IDM_ACC_SHOWGROUPS, MF_BYCOMMAND|MF_CHECKED);
        if (gs_pPrj && gs_pPrj->m_bAccAutoSaveOn)
          Menu.CheckMenuItem(IDM_ACC_AUTOSAVE, MF_BYCOMMAND|MF_CHECKED);
        if (gs_pPrj && gs_pPrj->m_bAccUpdateOnRun)
          Menu.CheckMenuItem(IDM_ACC_UPDATEONRUN, MF_BYCOMMAND|MF_CHECKED);
        if (gs_pPrj && gs_pPrj->m_bAccHistoryInfoVis)
          Menu.CheckMenuItem(IDM_ACC_HISTORYINFO, MF_BYCOMMAND|MF_CHECKED);
        
        //Menu.AppendMenu(MF_STRING, IDM_ACC_FINDTAG, s());
        //Menu.AppendMenu(MF_STRING, IDM_ACC_MDLHELP, "Model &Help");

        Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, this);
        Menu.DestroyMenu();

        }
        break;
      }
    }
  else if (ObjOnPage[p]>=0)
    {
    ObjEdit[ObjOnPage[p]]->ButtonPushed(EI, Str);
    }
  else if (CurrentBlk(EI))
    {
    int p=EI.PageNo;
    if (EI.FieldId>=Id_FldBtns && EI.FieldId<Id_FldBtns+NFlds)
      {
      int fi=EI.FieldId-Id_FldBtns;
      ASSERT(fi<MaxAccNdFlds && fi>=0);
      AccNdFld & f=Flds[fi];
      if (f.HasData())
        {
        AccNdData & d=f();
        ASSERT(d.fHasButton || d.fHasCheck || (d.iFlags & DDEF_TAGPARM));
        if ((d.iFlags & DDEF_TAGPARM) && d.fHasButton)
          {
          char* pTxt = NULL;
          if (d.ValLst.Length()>0)
            {
            Strng * pS=d.ValLst.First();
            pTxt = new char[pS->Len()+1];
            strcpy(pTxt, pS->Str());
            }
          else
            {
            pTxt = new char[d.sTagOnly.Len()+1];
            strcpy(pTxt, d.sTagOnly());
            }
          ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS_NOERRDLG, (LPARAM)pTxt);
          }
        else if (d.iFlags & DDEF_TAGPARM)
          {
          if (!IsStrng(d.iType))
            {//assume field tag is tag...
            sLastSelected[0] = d.sSymOrTag();
            if (sLastSelected[0].GetLength()>3 && strncmp(d.sSymOrTag(), "IO.", 3)==0)
              sLastSelected[0] = sLastSelected[0].Mid(3, 1024);
            }
          else
            {//assume field data is tag...
            sLastSelected[0] = d.sValue();
            }
          flag HasTag = (sLastSelected[0].GetLength()>0 && (!(d.iFlags & DDEF_FUNCTPARM) || d.sValue[0]!='='));
          if (HasTag)
            {
            const int DotPos = sLastSelected[0].Find('.');
            if (DotPos>=0)
              sLastSelected[0] = sLastSelected[0].Left(DotPos);
            flag TryAccess = (EI.Fld->BigStrLength==IDB_ACCACCESS_BTN || EI.Fld->BigStrLength==IDB_ACCACCESS_LOWRES);
            if (TryAccess)
              {
              CXM_ObjectTag ObjTag((LPSTR)(LPCSTR)sLastSelected[0], TABOpt_Exists);
              CXM_ObjectData ObjData;
              CXM_Route Route;
              if (!XReadTaggedItem(ObjTag, ObjData, Route))
                {
                pStatusBar->SetMsg("Tag '%s' NOT found", sLastSelected[0]);
                LogNote((LPSTR)(LPCSTR)sLastSelected[0], 0, "Unable to find and access tag");
                HasTag = 0;
                }
              }
            
            if (HasTag)
              {
              char* pTxt = NULL;
              pTxt = new char[sLastSelected[0].GetLength()+1];
              strcpy(pTxt, (LPSTR)(LPCSTR)sLastSelected[0]);
              ScdMainWnd()->PostMessage(WMU_TAGACTION, TryAccess ? SUB_TAGACTION_FINDANDACCESS : SUB_TAGACTION_FIND, (LPARAM)pTxt);
              }
            }
          }
        else
          {
          d.sValue=Str;
          d.fIsModified=1; // force save of the data
          if (d.fHasButton && !d.fHasCheck)
            {
            d.sValue="1";
            }
          // Check to see if Data must be closed etc
          // do for Button & CheckBoxBtn but not CheckBox
          if (d.fHasButton || d.fHasSetOnChange)
            {
            if (CloseAccessData(true, false, false))
              {
              ClearFlds(true);

              LoadAccessData();
              MeasureFields();
              DoRebuild();
              }
            }
          }
        }
      }
    else if (EI.FieldId>=Id_TreeNdBtn && EI.FieldId<Id_TreeNdBtn+NFlds)
      {
      int fi=EI.FieldId-Id_TreeNdBtn;
      ASSERT(fi<MaxAccNdFlds && fi>=0);
      AccNdFld & f=Flds[fi];
      if (f.HasData())
        {
        AccNdData & d=f();
        ASSERT(d.fIsTreeNd || d.m_fNxtIsChild);
        Strng Tag;
        if (FullFldName(Tag, Data, f.iData))
          {
          f.fIsTreeVis= d.pObjAttr->SetFieldVisible(Tag(), d.iFlags, !f.fIsTreeVis);
          ReFreshAttrs();
          DoRebuild();
          }
        }
      }
    }
  return 0;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnDestroy()
  {
  FxdEdtView::OnDestroy();

  // TODO: Add your message handler code here
  CMDIChildWnd *p=(CMDIChildWnd*)GetParent();
  //ASSERT(gs_pAccFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
  }

//---------------------------------------------------------------------------

void AccNodeWnd::Initialise()
  {
  //TRACE("Initialise Access\n");
  }

//---------------------------------------------------------------------------

void AccNodeWnd::CloseWnd()
  {
  //TRACE("Clear Access\n");

  ClearFlds(false);

  CMDIChildWnd *gs_pAccFrame=(CMDIChildWnd *)GetParent();
  ASSERT(gs_pAccFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
  gs_pAccFrame->ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::ClearFlds(flag SaveCnvs)
  {
  int i,o;
  for (i = 0; i < NFlds; i++)
    Flds[i].Term();
  for (i = 0; i < NData; i++)
    Data[i].Term();
  for (o=0;o<NObjs; o++)
    {
    delete ObjEdit[o];
    ObjInst[o]->Destroy();
    }

  //DeleteWinControls();
  NObjs=0;
  NFlds=0;
  NData=0;
  for (i = 0; i < MaxAccNdFlds; i++)
    {
    Flds[i].Init();
    Data[i].Init();
    }

  if (SaveCnvs)
    {
    // This must be done last because Destructors of ObjInsts
    // may modify this list
    ObjAttributes.Save();
    }
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::RefreshData(flag SaveIt)
  {
  //dbgpln("AccNodeWnd::RefreshData(%s)", SaveIt ? "Save":"");
  if (NdTag.Length()>0)
    if (CloseAccessData(SaveIt, false, false))
      {
      ClearFlds(SaveIt);

      LoadAccessData();
      MeasureFields();//, false);
      DoRebuild();
      LoadStrings();
      //SetCurrentPage(this);
      ShowDefWndTitle();
      Invalidate();
      }

  return true;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::SaveData()
  {
  return true;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::LoadData()
  {
  return true;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::AnyFldsOrDataChanged()
  {
  if (AnyFldsChanged())
    return true;
  
  for (int fi=0; fi<NData; fi++)
#if WITHSPECSHEETS
//ss    if (Data[fi].fIsData && (Data[fi].fIsParam || Data[fi].fIsSpecSheet) && Data[fi].fIsModified)
#else
    if (Data[fi].fIsData && Data[fi].fIsParam && Data[fi].fIsModified)
#endif
      return true;

  for (int o=0; o<NObjs; o++)
    if (ObjChgd[o])
      return true;

  return false;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::CloseAccess()
  {
  //TRACE1("AccessNode %s\n", NdTag());
  CMDIChildWnd *gs_pAccFrame=(CMDIChildWnd *)GetParent();
  ASSERT(gs_pAccFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

  ClrCurrentEdit(false); //KGA

  int Ret=IDNO;
  if (AnyFldsOrDataChanged())
    {
    if (gs_pPrj && gs_pPrj->m_bAccAutoSaveOn)
      {
      Ret=IDYES;
      }
    else
      {
      Strng Msg;
      Msg.Set(" Save changes made in access window for '%s' ? ", NdTag());
      Ret=AfxMessageBox(Msg(), MB_ICONQUESTION|MB_YESNOCANCEL);
      }
    }
  if (Ret!=IDCANCEL)
    {
    CWaitCursor Wait;
    if (Ret==IDYES)
      {
      WndTitle.Set("Saving - %s", NdTag());
      gs_pAccFrame->SetWindowText(WndTitle());
      }

    CloseAccessData(Ret==IDYES, Ret==IDCANCEL, true);

    ClearFlds(true);//Ret==IDYES);
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::DoDeferredAccess()
  {
  Strng Tag;
  if (1)
    {
    CSingleLock L(&m_DeferredLock);
    Tag=m_DeferredTag;
    m_DeferredTag="";
    }
  if (Tag())
    AccessNode(Tag(), false);
  }

void AccNodeWnd::AccessNode(LPCTSTR  pNdTag, bool AllowDeferred)
  {
  if (pNdTag==NULL || pNdTag[0]==0)
    return;

  if (gs_Exec.Busy() && !gs_Exec.Waiting() && AllowDeferred)
    {
    gs_Exec.SetStopSleep();
    CSingleLock L(&m_DeferredLock);
    m_DeferredTag=pNdTag;
    return;
    }


  if (0)
    {
  
    CXM_Route XRoute;
    CExcelBasicIO XX(gs_pPrj->m_pFlwLib, &XRoute, "D:\\SysCAD Projects\\D\\De Beers\\Kenwalt example.xls");
    long R, C;
    CArray<double, double> Dbls;
    XX.GetDoubleValueRange("Stream_Densimetrics", "OreSizefeed", R, C, Dbls);


    int xxxxx=0;
    }





  gs_AccessWnds.MarkAsActive(this);

  Strng RqdNdTag(pNdTag);
  int iSpc=RqdNdTag.Find(" ");
  if (iSpc>0)
    RqdNdTag.SetLength(iSpc);

  char Buff[512];
  sprintf(Buff, "Access %s", RqdNdTag());
  CWaitMsgCursor Wait(Buff);

  //TRACE1("AccessNode %s\n", RqdNdTag());
  CMDIChildWnd *gs_pAccFrame=(CMDIChildWnd *)GetParent();
  ASSERT(gs_pAccFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
  gs_pAccFrame->SetWindowText("Access ...");
  if (CloseAccess())
    {
    ObjectAttribute *pAttr=ObjAttributes.FindObject("AccNodeWnd");
    if (pAttr)
      {
      pAttr->FieldFmt("GlblFmt",   GlblFmt);
      }

    NdTag.Set(RqdNdTag());
    ObjRoute.Clear();

    if (!fKeepNdHist)
      {
      for (int i=0; i<NdHistList.GetSize(); i++)
        if (NdHistList[i].XStrICmp(NdTag)==0)
          {
          NdHistList.RemoveAt(i);
          break;
          }

      NdHistList.InsertAt(0, NdTag);
      if (NdHistList.GetSize()>MaxNdHistListSize)
        NdHistList.SetSize(MaxNdHistListSize);
      
      iNdHistPos=Range(0, iNdHistPos-1, NdHistList.GetSize()-1);
      }
    fKeepNdHist=0;

    #if dbgDataReadWrite
    dbgpln("Get Data From %s", NdTag());
    #endif

    LoadAccessData();
    if (NData<=0)
      LogError("Access", 0, "Tag not found:%s", NdTag());
    if (NData<0 && NFlds>0)
      LogError("Access", 0, "Empty Specsheet:%s", NdTag());

    MeasureFields();//, false);

    gs_pAccFrame->ShowWindow(CWindowLists::GetWndMaxMode() ? SW_SHOWMAXIMIZED : SW_RESTORE);
    DoRebuild();
    LoadStrings();
    //SetCurrentPage(this);
    FixScrollSizes(true);

    ShowDefWndTitle();
    gs_pAccFrame->MDIActivate();
    //ScdApp()->DoWaitCursor(-1);
    SetFocus();
    }
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
  {
  FxdEdtView::OnActivate(nState, pWndOther, bMinimized);

  // TODO: Add your message handler code here

  CMDIChildWnd *gs_pAccFrame=(CMDIChildWnd *)GetParent();
  ASSERT(gs_pAccFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

  WINDOWPLACEMENT WP;
  WP.length=sizeof(WP);
  if (gs_pAccFrame->GetWindowPlacement(&WP))
    {
    CMDIFrameWnd*MDIFrame=(CMDIFrameWnd*)gs_pAccFrame->GetParent();
    CRect rc;
    MDIFrame->GetClientRect(&rc);

    if ((WP.rcNormalPosition.bottom-WP.rcNormalPosition.top) > (rc.bottom-rc.top))
      {
      WP.rcNormalPosition.left=rc.left+(2*(rc.right-rc.left))/3;
      WP.rcNormalPosition.right=rc.right;
      WP.rcNormalPosition.top=rc.top;
      WP.rcNormalPosition.bottom=rc.bottom;
      WP.length=sizeof(WP);
      gs_pAccFrame->SetWindowPlacement(&WP);
      }
    }
  }

//---------------------------------------------------------------------------

int GetFieldIndex(int i)
  {
  if (i<0)
    return abs(i)-Id_FldParms;
  else if (i>=Id_FldCnvs)
    return i-Id_FldCnvs;
  else
    return i-Id_FldParms;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoLButtonDown(UINT nFlags, CPoint point)
  {
  SaveCurrentPageInfo();
  if (CPgNo>=0)
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoLButtonDown(nFlags, point))
        return 1;

  return FxdEdtView::DoLButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoLButtonUp(UINT nFlags, CPoint point)
  {
  if (CPgNo>=0)
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoLButtonUp(nFlags, point))
        return 1;

  flag ret=FxdEdtView::DoLButtonUp(nFlags, point);
  return ret;
  }

//---------------------------------------------------------------------------
#define WithLeftDblClk_ToggleVisible 0
//kga: 30/11/04: left double click action to hide fields causes more trouble than it is worth!!!

flag AccNodeWnd::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  if (CPgNo>=0)
    {
    if (ObjOnPage[CPgNo]>=0)
      {
      if (ObjEdit[ObjOnPage[CPgNo]]->DoLButtonDblClk(nFlags, point))
        return 1;
      }

#if WithLeftDblClk_ToggleVisible
    FxdEdtInfo EI;
    if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
      {
      if (EI.FieldId<0 && EI.Fld)
        {
        int fi=abs(EI.FieldId)-Id_FldParms;
        if (fi < NFlds)
          if (Flds[fi].fIsData)
            {
            AccNdFld & f=Flds[fi];
            Strng Tag;
            if (FullFldName(Tag, Data, f.iData))
              {
              f.fIsTreeVis= f().pObjAttr->SetFieldVisible(Tag(), f().iFlags, !f.fIsTreeVis);
              ReFreshAttrs();
              DoRebuild();
              }
            }
        }
      }
#endif    
    }
  return true;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccCnv(UINT Id)
  {
  if (CPgNo>=0)
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoAccCnv(Id-IDM_ACC_CNV1))
        return ;//1;

  if (RBActiveFld>=0 && RBActiveFld < NFlds)
    {
    rAccNdFld f=Flds[RBActiveFld];
    if (f.HasData())
      {
      AccNdData &d=f();
      CCnvIndex CnvInx=f().Cnv.Index();
      pCDataCnv pC=Cnvs[CnvInx];
      for (int i=Id-IDM_ACC_CNV1; i>0; i--)
        pC=pC->Next();

      CDataCnv & C=*Cnvs[CnvInx];
      if (pC==NULL)
        {  // Toggle Overide
        if (C.OverTxt())
          C.SetOverTxt("");
        else
          C.SetOverTxt(d.Cnv.Text());
        gs_CnvsMngr.Save();
        for (int i = 0; i < NFlds; i++)
          {
          rAccNdFld f=Flds[i];
          f().AdjTextValue(NULL, false);
          if (f.ValFld)
            f.ValFld->Invalidate(ScrGB);
          }
        }
      else //if (pC)
        {
        if (C.OverTxt())
          {
          C.SetOverTxt(pC->Txt());
          gs_CnvsMngr.Save();
          for (int i = 0; i < NFlds; i++)
            {
            rAccNdFld f=Flds[i];
            f().AdjTextValue(NULL, false);
            if (f.ValFld)
              f.ValFld->Invalidate(ScrGB);
            }
          }
        else
          {
          d.AdjTextValue(pC->Txt(), false);
          Strng Tag;
          if (FullFldName(Tag, Data, f.iData))
            d.pObjAttr->SetFieldCnvs(Tag(), d.Cnv);
          f.ValFld->Invalidate(ScrGB);        

          for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
            {
            //int fi=abs(pFld->FieldId)-Id_FldParms;
            int fi=GetFieldIndex(pFld->FieldId);
            if (pFld->FieldId!=0 && fi<MaxAccNdFlds)
              {
              rAccNdFld f=Flds[fi];
              if (f.HasData() && (f().Cnv.Index()==CnvInx))
                {
                AccNdData &d=f();
                d.AdjTextValue(pC->Txt(), false);
                f.ValFld->Invalidate(ScrGB);
                if (FullFldName(Tag, Data, f.iData))
                  d.pObjAttr->SetFieldCnvs(Tag(), d.Cnv);
                }
              }
            }
          }
        }

      ReFreshAttrs();
      DoRebuild();
      }
    }
  RBActiveFld=-1;
  };

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccFmt(UINT Id)
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++)
    if (i==Id-IDM_ACC_FMT1)
      break;
  if (fGlobalFmtMenu)
    {
    if (i<(UINT)DefinedFmts.GetSize())
      {
      GlblFmt=DefinedFmts[i];
      ObjectAttribute *pAttr=ObjAttributes.FindObject("AccNodeWnd");
      if (pAttr)
        pAttr->SetFieldFmt  ("GlblFmt", GlblFmt);
      }
    //todo: force update of all fields???
    RBActiveFld=-1;
    return;
    }

  if (CPgNo>=0)
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoAccFmt(Id-IDM_ACC_FMT1))
        return;

  if (RBActiveFld>=0 && RBActiveFld < NFlds)
    {
    AccNdFld & f=Flds[RBActiveFld];
    if (f.HasData())
      {
//      AccNdData &d=f();
      if (i<(UINT)DefinedFmts.GetSize())
        {
        for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
          {
          //int fi=abs(pFld->FieldId)-Id_FldParms;
          int fi=GetFieldIndex(pFld->FieldId);
          if (pFld->FieldId!=0 && fi<MaxAccNdFlds)
            {
            rAccNdFld f=Flds[fi];
            if (f.HasData())
              {
              AccNdData &d=f();
              d.Fmt=DefinedFmts[i];
              if (IsFloatData(f.iType))
                d.FormatFloat();
              Strng Tag;
              if (FullFldName(Tag, Data, f.iData))
                d.pObjAttr->SetFieldFmt(Tag(), DefinedFmts[i]);
              }
            }
          }
        }
      ReFreshAttrs();
      DoRebuild();
      }
    }
  RBActiveFld=-1;
  };

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccShow(UINT Id)
  {
  flag DoIt=false;
  for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
    {
    //int fi=abs(pFld->FieldId)-Id_FldParms;
    int fi=GetFieldIndex(pFld->FieldId);
    if (pFld->FieldId!=0 && fi<MaxAccNdFlds)
      {
      rAccNdFld f=Flds[fi];
    //if (RBActiveFld >=0 && RBActiveFld < NFlds)
    //  {
    //  rAccNdFld f=Flds[RBActiveFld];
      if (f.HasData())
        {
        AccNdData &d=f();
        Strng Tag;
        if (FullFldName(Tag, Data, f.iData))
          {
          f.fIsTreeVis=d.pObjAttr->SetFieldVisible(Tag(), d.iFlags, Id==IDM_ACC_SHOW);
          DoIt=true;
          }
        }
      }
    }
  if (DoIt)
    {
    ReFreshAttrs();
    DoRebuild();
    }
  RBActiveFld=-1;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccView(UINT Id)
  {
  if (CPgNo>=0)
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoAccRptTagLists())
        return ;//1;
  for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
    {
    //int fi=abs(pFld->FieldId)-Id_FldParms;
    int fi=GetFieldIndex(pFld->FieldId);
    if (pFld->FieldId!=0 && fi<MaxAccNdFlds)
      {
      AccNdFld & f=Flds[fi];
      if (f.HasData())
        {
        AccNdData &d=f();
        switch (Id)
          {
          case IDM_ACC_TREND0:
            {
            if (::SendMessage(CTagVwText::CurrentTrend(), WMU_ADDTAG2TREND, 0, (LPARAM)d.sFullTag())==0)
              LogNote("Access", 0, "Tag %s not added to Trend", d.sFullTag()); 
            break;
            }
          case IDM_ACC_VIEW0:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
            break;
            }
          case IDM_ACC_ARCHIVE0:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_ADDTOARCHIVE, (long)(IB));
            break;
            }
          case IDM_ACC_CUSTOM0:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
            break;
            }
          case IDM_ACC_HISTORY_ON:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_HISTORYON, (long)(IB));
            break;
            }
          case IDM_ACC_HISTORY_OFF:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_HISTORYOFF, (long)(IB));
            break;
            }
          case IDM_ACC_HISTORY_ALLON:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_ALLHISTORYON, (long)(IB));
            break;
            }
          case IDM_ACC_HISTORY_ALLOFF:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_ALLHISTORYOFF, (long)(IB));
            break;
            }
          case IDM_ACC_HISTORY_ALLMDLON:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_ALLMDLHISTORYON, (long)(IB));
            break;
            }
          case IDM_ACC_HISTORY_ALLMDLOFF:
            {
            TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
            ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_ALLMDLHISTORYOFF, (long)(IB));
            break;
            }
          }
        }
      }
    }
  RBActiveFld=-1;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccOptions(UINT Id)
  {
  switch (Id)
    {
    case IDM_ACC_ALLFIELDS:
      fAllFldsVis=!fAllFldsVis;
      DoRebuild();
      break;
    case IDM_ACC_AUTOSAVE:
      if (gs_pPrj)
        gs_pPrj->m_bAccAutoSaveOn=!gs_pPrj->m_bAccAutoSaveOn;
      //DoRebuild();
      break;
    case IDM_ACC_UPDATEONRUN:
      if (gs_pPrj)
        gs_pPrj->m_bAccUpdateOnRun=!gs_pPrj->m_bAccUpdateOnRun;
      //DoRebuild();
      break;
    case IDM_ACC_CHANGETAG:
      {
      CChangeTag ChgTag(NdTag(), true);
      ChgTag.DoModal();
      }
      break;
    case IDM_ACC_PRINT:
      OnFilePrint();
      break;
#if WITHDOCUMENTATIONLINK
    case IDM_ACC_DOCUMENT:
//MSWORD        ?
      //gs_pPrj->Document(NdTag());
      break;
#endif
#if WITHSPECSHEETS
//ss    case IDM_ACC_SPECSHEET:
//ss      {
//ss      CSpecSheet SpecSheet;
//ss      SpecSheet.DoModal();
//ss      }
//ss      break;
#endif
    case IDM_ACC_DISPLAYTAGS:
      SetDisplayTagsOnly(!GetDisplayTagsOnly());
#if WITHSPECSHEETS
//ss      LoadSpecSheet(sSpecSheet());
#endif
      if (CloseAccessData(true, false, false))
        {
        ClearFlds(false);

        LoadAccessData();
        MeasureFields();//, false);
        DoRebuild();
        ShowDefWndTitle();
        }
      break;
    case IDM_ACC_HIDEIFZERO:
      SetHideZeros(!GetHideZeros());
#if WITHSPECSHEETS
//ss      LoadSpecSheet(sSpecSheet());
#endif
      if (CloseAccessData(true, false, false))
        {
        ClearFlds(false);

        LoadAccessData();
        MeasureFields();//, false);
        DoRebuild();
        ShowDefWndTitle();
        }
      break;
    case IDM_ACC_HISTORYINFO:
      if (gs_pPrj)
        gs_pPrj->m_bAccHistoryInfoVis=!gs_pPrj->m_bAccHistoryInfoVis;
      if (CloseAccessData(false, false, false))
        {
        ClearFlds(false);

        LoadAccessData();
        MeasureFields();//, false);
        DoRebuild();
        ShowDefWndTitle();
        }
      break;
    case IDM_ACC_QUICKVIEW:
      {
      TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), tt_NULL, NdTag(), "", NULL, NULL);
      ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 1, (long)(IB));
      }
      break;
    case IDM_ACC_COPYBLK_0:
      {
      CCopyTagBlkDlg CopyBlkDlg(this, ObjClassId(), NdTag());
      CopyBlkDlg.DoModal();
      }
      break;
    case IDM_ACC_PASTEBLK_0:
      {
      /*CAccPasteBlk PasteBlk;
      PasteBlk.DoModal();*/
      }
      break;
    case IDM_ACC_SHOWGROUPS:
      SVI.SetShowGroups(!SVI.GetShowGroups());
      if (CloseAccessData(true, false, false))
        {
        ClearFlds(false);

        LoadAccessData();
        MeasureFields();//, false);
        DoRebuild();
        ShowDefWndTitle();
        }
      break;
    case IDM_ACC_SPAREOPTION2:
      break;
    case IDM_ACC_SPAREOPTION3:
      break;
    case IDM_ACC_SPAREOPTION4:
      break;
    case IDM_ACC_SPAREOPTION5:
      break;
    case IDM_ACC_SPAREOPTION6:
      break;
    case IDM_ACC_SPAREOPTION7:
      break;
    case IDM_ACC_SPAREOPTION8:
      break;
    case IDM_ACC_SPAREOPTION9:
      break;
    default:;
    }
  
  // FOrce Redraw with rebuild if neccessary ...
  if (RebuildReqd || ReloadReqd)
    {
    MeasureFields();
    LoadStrings();
    //SetCurrentPage(this);
    Invalidate();
    //FixScrollSizes();
    }
//  RedrawWindow();
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccCopyBlk(UINT Id)
  {
  int i=Id-IDM_ACC_COPYBLK_0;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccPasteBlk(UINT Id)
  {
  int i=Id-IDM_ACC_PASTEBLK_0;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccCutTag()     { OnCutTag();     };
void AccNodeWnd::OnAccCutVal()     { OnCut();        };
void AccNodeWnd::OnAccCopyTag()    { OnCopyTag();    };
void AccNodeWnd::OnAccCopyRHSTag() { OnCopyRHSTag(); };
void AccNodeWnd::OnAccCopyVal()    { OnCopy();       };
void AccNodeWnd::OnAccPaste()      { OnPaste();      };

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccEmptyPreset(UINT Id)   
  {
  Strng_List SL;
  eScdMdlStateActs Op;
  switch (Id)
    {
    case IDM_ACC_PBINITNODE:
      SL.Append(NdTag());
    //case IDM_ACC_PBINITALL:
      Op=MSA_PBInit;
      break;
    case IDM_ACC_EMPTYNODE:
      SL.Append(NdTag());
    case IDM_ACC_EMPTYALL:
      Op=MSA_Empty;
      break;
    case IDM_ACC_ZEROFLOWSNODE:
      SL.Append(NdTag());
    case IDM_ACC_ZEROFLOWSALL:
      Op=MSA_ZeroFlows;
      break;
    case IDM_ACC_PRESETNODE:
      SL.Append(NdTag());
    case IDM_ACC_PRESETALL:
      Op=MSA_PreSet;
      break;
    case IDM_ACC_RESETSTATSNODE:
      SL.Append(NdTag());
    //case IDM_ACC_RESETSTATSALL:
      Op=MSA_DynStatsRunInit;
      break;
    }

  if (CloseAccessData(true, false, false))
    {
    ClearFlds(true);

    gs_Exec.SetModelState(Op, SL);

    LoadAccessData();
    MeasureFields();//, false);
    DoRebuild();
    LoadStrings();
    //SetCurrentPage(this);
    ShowDefWndTitle();
    Invalidate();
    }
  };

//---------------------------------------------------------------------------

void AccNodeWnd::OnAccMenu(UINT id)
  {
  // These cmmds are now MaxTagCmds apart;
  long Index=id % MaxTagCmds;
  id=(id/MaxTagCmds)*MaxTagCmds;
  switch (id)
    {
    case IDM_ACC_ACCESSTAG_0:
    case IDM_ACC_FINDTAG_0:
    case IDM_ACC_MDLHELP_0:
      if (sLastSelected[Index].GetLength()>0)
        {
        WPARAM wp;
        switch (id)
          {
          case IDM_ACC_ACCESSTAG_0: wp = SUB_TAGACTION_FINDANDACCESS;  break;
          case IDM_ACC_FINDTAG_0  : wp = SUB_TAGACTION_FIND;    break;
          case IDM_ACC_MDLHELP_0  : wp = SUB_TAGACTION_MDLHELP; break;
          }
        char* pTxt = new char[sLastSelected[Index].GetLength()+1];
        strcpy(pTxt, sLastSelected[Index]);
        ScdMainWnd()->PostMessage(WMU_TAGACTION, wp, (LPARAM)pTxt);
        }
      break;
    case IDM_ACC_SELECTTAG_0:
      {//Select a valid tag using select tag dialog box...
      CSelectTagDlg Dlg(pLastAccNdData->sValue(), SelFull, 1, this);
      if (Dlg.DoModal()==IDOK && Dlg.sTag.GetLength()>0)
        {//TODO KGA is this elegant??? NO!
        pLastAccNdData->sValue = (const char*)Dlg.sTag;
        DoRebuild();
        }
      break;
      }
    case IDM_ACC_REFRESH_0:
      //RefreshData(false);//true);
      //SendMessage(WMU_ACCREFRESHCANCEL);
      //SendMessage(WMU_ACCREFRESHSAVE);
      PostMessage(WMU_ACCREFRESHSAVE);
      break;
    case IDM_ACC_BTNCOPYTAG_0:
      if (sLastSelFullTag[Index].GetLength()>0)
        CopyTextToClipboard(this, (LPSTR)(LPCSTR)sLastSelFullTag[Index]);
      break;
    /*case ID_EDIT_RHSCOPYTAG:
      if (sLastSelFullTag.GetLength()>0)
        {
        if (sLastSelFullTag.Find('.')>0)
          CopyTextToClipboard(this, sLastSelFullTag.Mid(sLastSelFullTag.Find('.')+1, 256)());
        else
          CopyTextToClipboard(this, sLastSelFullTag());
        }
      break;*/
    default:
      {
      RequestModelIOConnRec ConnInfo;
      int j = id - IDM_PRJ_ACCESSFIRST;
      if (gs_pTheSFELib->RequestModelIOConn((LPSTR)(LPCSTR)sLastSelected[Index], j, ConnInfo))
        AccessNode(ConnInfo.Tag());
      }
    }
  }

//---------------------------------------------------------------------------

afx_msg LONG AccNodeWnd::OnAccRefreshSave(UINT wParam, LONG lParam)
  {
////TODO RESTORE
  RefreshData(true);
  return 0;
  }

//---------------------------------------------------------------------------

afx_msg LONG AccNodeWnd::OnAccRefreshCancel(UINT wParam, LONG lParam)
  {
////TODO RESTORE
  RefreshData(false);
  return 0;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoRButtonDown(UINT nFlags, CPoint point)
  {
  SaveCurrentPageInfo();
  if (CPgNo>=0)
    {
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoRButtonDown(nFlags, point))
        return 1;
    }

  flag ret=FxdEdtView::DoRButtonDown(nFlags, point);
  return ret;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoRButtonUp(UINT nFlags, CPoint point)
  {
  CnvAttribute::SetCmdIndex(IDM_ACC_CNV1);
  FmtAttribute::SetCmdIndex(IDM_ACC_FMT1);
  SetCmdCutTag      (IDM_ACC_CUTTAG);
  SetCmdCutVal      (IDM_ACC_CUTVAL);
  SetCmdCopyTag     (IDM_ACC_COPYTAG);
  SetCmdCopyRHSTag  (IDM_ACC_COPYRHSTAG);
  SetCmdCopyVal     (IDM_ACC_COPYVAL);
  SetCmdPaste       (IDM_ACC_PASTE);
  SetSendToTrend    (IDM_ACC_TREND0);
  SetSendToQuickView(IDM_ACC_VIEW0);
  SetSendToHistory  (IDM_ACC_HISTORY_ON);
  SetSendToArchive  (IDM_ACC_ARCHIVE0);
  SetSendToCustom   (IDM_ACC_CUSTOM0);
  flag DidPopupMenu=0;
  flag ObjDone=false;
  if (CPgNo>=0)
    {
    if (ObjOnPage[CPgNo]>=0)
      {
      ObjDone=true;
      if (ObjEdit[ObjOnPage[CPgNo]]->DoRButtonUp(nFlags, point))
        return 1;
      }
    }
  flag ret=FxdEdtView::DoRButtonUp(nFlags, point);
  if (CPgNo>=0 && !ObjDone)
    {
    flag BtnMenu = false;
    FxdEdtInfo EI;
    int fi=-1; 
    if (LocateFromCR((int)ChEditPos.x, (int)ChEditPos.y, EI))
      {
      if (EI.Fld)
        {
        fi=GetFieldIndex(EI.FieldId);
        if (EI.Fld && !EI.Fld->IsButton && EI.FieldId>=Id_FldBtns)
          {
          //fi=GetFieldIndex(EI.FieldId);
          fi=EI.FieldId-Id_FldBtns;
          if (fi<NFlds && Flds[fi].fIsData && Flds[fi].HasData())
            BtnMenu = true;
          else
            fi=-1;
          }
        else if (!(fi<NFlds && Flds[fi].fIsData && Flds[fi].HasData()))
          fi=-1;
        }
      }
    if (fi==-1)
      {
      if (LocateFromCR(Max((int)ChEditPos.x-8, 1), (int)ChEditPos.y, EI))
        {
        if (EI.Fld && EI.Fld->IsButton && EI.FieldId>=Id_FldBtns)
          {
          //fi=GetFieldIndex(EI.FieldId);
          fi=EI.FieldId-Id_FldBtns;
          if (fi<NFlds && Flds[fi].fIsData && Flds[fi].HasData())
            BtnMenu = true;
          else
            fi=-1;
          }
        }
      }
    if (fi>=0)
      {
      AccNdFld & f=Flds[fi];
      RBActiveFld = fi;
      AccNdData & d=f();
      CRect WRect;
      GetWindowRect(&WRect);
      RBPoint.x = WRect.left+point.x;     
      RBPoint.y = WRect.top+point.y;
      //RBPoint.x = WRect.left;//+point.x;     
      //RBPoint.y = WRect.top+EI.Fld->Ye(ScrGB);//point.y;

      Strng Tag;
      if (FullFldName(Tag, Data, f.iData))
        {
        DidPopupMenu=1;
        pLastAccNdData = &d;
        CMenu Menu;
        Menu.CreatePopupMenu();
        fGlobalFmtMenu=0;
        CMenu FormatMenu;
        FormatMenu.CreatePopupMenu();
        d.Fmt.AddToMenu(FormatMenu);
        CMenu CnvMenu;
        CnvMenu.CreatePopupMenu();
        d.Cnv.AddToMenu(CnvMenu, true);
        
        Strng ClassNV(GetClassIdNoVersion(ObjClassId()));

        Strng S1(ClassNV),S2(ClassNV);
        S1+="'s On";
        S2+="'s Off";
        
        CMenu RecMenu;
        RecMenu.CreatePopupMenu();
        RecMenu.AppendMenu(MF_STRING, IDM_ACC_HISTORY_ON, "On");
        RecMenu.AppendMenu(MF_STRING, IDM_ACC_HISTORY_OFF, "Off");
        RecMenu.AppendMenu(MF_STRING, IDM_ACC_HISTORY_ALLMDLON, S1());
        RecMenu.AppendMenu(MF_STRING, IDM_ACC_HISTORY_ALLMDLOFF, S2());
        RecMenu.AppendMenu(MF_STRING, IDM_ACC_HISTORY_ALLON, "All On");
        RecMenu.AppendMenu(MF_STRING, IDM_ACC_HISTORY_ALLOFF, "All Off");

        Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
        if (d.Cnv.Index()<=0 || !(IsFloatData(d.iType)||(IsStrng(d.iType)&& IsTimeCnv(d.Cnv.Index()))))
          Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

        Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");

        if (!IsFloatData(d.iType))
          Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
        if (BtnMenu)
          {
          sLastSelFullTag[0] = d.sFullTag();
          Menu.AppendMenu(MF_SEPARATOR);
          Menu.AppendMenu(MF_STRING, IDM_ACC_BTNCOPYTAG_0, "Copy Full &Tag");
          }
        else
          {
          Menu.AppendMenu(MF_STRING, IDM_ACC_TREND0, "Send To &Trend\tF5");
          Menu.AppendMenu(MF_STRING, IDM_ACC_VIEW0, "Send To &QuickView\tF6");
          #if WithSendToCustom
          Menu.AppendMenu(MF_STRING|MF_GRAYED, IDM_ACC_CUSTOM0, "Send To &Custom\tF7");
          #endif
          Menu.AppendMenu(MF_POPUP, (unsigned int)RecMenu.m_hMenu, "&Record in historian");
          Menu.AppendMenu(MF_STRING, IDM_ACC_ARCHIVE0, "Add to Archive");

          if (!IsData(d.iType))
            Menu.EnableMenuItem(2, MF_BYPOSITION|MF_GRAYED);
        
          //flag Vis = d.pObjAttr->FieldVisible(Tag(), d.iFlags, true);
          //if (Vis)
          //  {
          //  Menu.AppendMenu(MF_STRING, IDM_ACC_HIDE,   "&Show");
          //  Menu.CheckMenuItem(IDM_ACC_HIDE, MF_BYCOMMAND|MF_CHECKED);
          //  }
          //else
          //  Menu.AppendMenu(MF_STRING, IDM_ACC_SHOW,   "&Show");
          Menu.AppendMenu(MF_STRING, IDM_ACC_SHOW,   "&Show");
          Menu.AppendMenu(MF_STRING, IDM_ACC_HIDE,   "&Hide");

          Menu.AppendMenu(MF_SEPARATOR);
          sLastSelFullTag[0] = d.sFullTag();
          if (EI.Fld->Tag==NULL && d.sFullTag.GetLength())
            {
            Menu.AppendMenu(MF_STRING, IDM_ACC_BTNCOPYTAG_0, "Copy Full &Tag");
            }
          else
            {
            Menu.AppendMenu(MF_STRING, IDM_ACC_COPYTAG, "Copy Full &Tag");
            if (EI.Fld->Tag==NULL && d.sFullTag.GetLength())
              Menu.EnableMenuItem(IDM_ACC_COPYTAG, MF_BYCOMMAND|MF_GRAYED);
            }
          bool EdtFld = !(EI.FieldId<Id_FldParms || EI.FieldId>=Id_FldCnvs);
          Menu.AppendMenu(MF_STRING, IDM_ACC_COPYRHSTAG, "Copy Tag");
          Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
          Menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");
          if (!EdtFld)
            Menu.EnableMenuItem(ID_EDIT_PASTE, MF_BYCOMMAND|MF_GRAYED);
    
          if (d.iFlags & DDEF_TAGPARM)
            {
            if (!IsStrng(d.iType))
              {//assume field tag is tag...
              sLastSelected[0] = d.sSymOrTag();
              if (sLastSelected[0].GetLength()>3 && strncmp(d.sSymOrTag(), "IO.", 3)==0)
                sLastSelected[0] = sLastSelected[0].Mid(3, 1024);
              }
            else
              {//assume field data is tag...
              sLastSelected[0] = d.sValue();
              }
            flag DoneSeparator = 0;
            flag HasTag = (sLastSelected[0].GetLength()>0 && (!(d.iFlags & DDEF_FUNCTPARM) || d.sValue[0]!='='));
            if (HasTag)
              {
              const int DotPos = sLastSelected[0].Find('.');
              if (DotPos>=0)
                sLastSelected[0] = sLastSelected[0].Left(DotPos);
              CXM_ObjectTag ObjTag((LPSTR)(LPCSTR)sLastSelected[0], TABOpt_Exists);
              CXM_ObjectData ObjData;
              CXM_Route Route;
              if (!XReadTaggedItem(ObjTag, ObjData, Route))
                HasTag = 0;
              if (HasTag)
                {
                DoneSeparator = 1;
                Menu.AppendMenu(MF_SEPARATOR);
                Strng s;
                s.Set("&Access %s...", (LPSTR)(LPCSTR)sLastSelected[0]);
                Menu.AppendMenu(MF_STRING, IDM_ACC_ACCESSTAG_0, s());
                s.Set("&Find %s...", (LPSTR)(LPCSTR)sLastSelected[0]);
                Menu.AppendMenu(MF_STRING, IDM_ACC_FINDTAG_0, s());
                }
              }
            if ((d.iFlags & DDEF_TAGPARM) && (d.iFlags & DDEF_PARAM))
              {
              if (!DoneSeparator)
                Menu.AppendMenu(MF_SEPARATOR);
              Menu.AppendMenu(MF_STRING, IDM_ACC_SELECTTAG_0, "&Select tag");
              //Menu.EnableMenuItem(IDM_ACC_SELECTTAG, MF_BYCOMMAND|MF_GRAYED); // Temporaray 
              }
            if (HasTag && gs_pTheSFELib)
              gs_pTheSFELib->AddAttachedToMenu(Menu, (LPSTR)(LPCSTR)sLastSelected[0]);
            }
          #if WITHXREFUPDATES
          else if (d.m_Markers)// & FFM_HasSet)
            {
            CXRefInfoArray Refs;
            gs_pTheSFELib->RequestTagRefInfo(d.sFullTag(), Refs);
            if (Refs.GetSize())
              {
              //Menu.AppendMenu(MF_SEPARATOR);
              bool HasConns=0;
              CMenu CnMenu;
              CnMenu.CreatePopupMenu();
              if (1)
                {
                bool FirstOne=true;
                int iTag=1;
                for (int iPass=0; iPass<2; iPass++)
                  {
                  for (int ii=0; ii<Refs.GetSize(); ii++)
                    {
                    CTagRefInfoItem  *Ref = Refs[ii];
                    CString s, sID;
                    if (Ref->m_fIsSetRef && iPass==0)
                      {
                      sLastSelected[iTag] = Ref->m_sRmtTag();
                      s.Format(Ref->m_iSetCnt ? "%s -->(Set)":"%s -->()", Ref->m_sRmtID());
                      }
                    if (Ref->m_fIsGetRef && iPass==1)
                      {
                      sLastSelected[iTag] = Ref->m_sRmtTag();
                      s.Format(Ref->m_iGetCnt ? "(Get)--> %s":"()--> %s", Ref->m_sRmtID());
                      }

                    if (s.GetLength()>0)
                      {
                      HasConns=1;

                      CMenu XMenu;
                      XMenu.CreatePopupMenu();
                      XMenu.AppendMenu(MF_STRING|MF_GRAYED, 0, sLastSelected[iTag]);
                      XMenu.AppendMenu(MF_SEPARATOR);
                      XMenu.AppendMenu(MF_STRING, IDM_ACC_ACCESSTAG_0+iTag, "&Access");
                      XMenu.AppendMenu(MF_STRING, IDM_ACC_FINDTAG_0+iTag,   "&Find");
                      Menu.AppendMenu(MF_POPUP|(FirstOne?MF_MENUBARBREAK:0), (unsigned int)XMenu.m_hMenu, s);
                      FirstOne=false;
                      iTag++;
                      }
                    }
                  }
                }
              //Menu.AppendMenu(MF_POPUP|(HasConns?0:MF_GRAYED), (unsigned int)CnMenu.m_hMenu, "Connects");
              }
            }
          #endif
          }

        //Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, this);
        Menu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, this);
        Menu.DestroyMenu();
        }
      }
    }
  if (!DidPopupMenu)
    {
    CRect WRect;
    GetWindowRect(&WRect);
    RBPoint.x = WRect.left+point.x;
    RBPoint.y = WRect.top+point.y;
    sLastSelected[0] = NdTag();
    Strng s;
    CMenu Menu;
    Menu.CreatePopupMenu();
    Menu.AppendMenu(MF_STRING, IDM_ACC_REFRESH_0, "&Refresh");
    s.Set("&Find %s...", (LPSTR)(LPCSTR)sLastSelected[0]);
    Menu.AppendMenu(MF_STRING, IDM_ACC_FINDTAG_0, s());
    Menu.AppendMenu(MF_STRING, IDM_ACC_MDLHELP_0, "Model &Help");

    if (gs_pTheSFELib)
      gs_pTheSFELib->AddAttachedToMenu(Menu, (LPSTR)(LPCSTR)sLastSelected[0]);

    Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, this);
    Menu.DestroyMenu();
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  if (CPgNo>=0)
    {
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoRButtonDblClk(nFlags, point))
        return 1;
    }
  return FxdEdtView::DoRButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

void AccNodeWnd::ShowTT()
  {
  if (!HelpMngr.ShowToolTips())
    return;

  INITCOMMONCONTROLSEX iccex; 
  // struct specifying info about tool in ToolTip control
  TOOLINFO ti;
  unsigned int uid = 0;       // for ti initialization
  RECT rect;                  // for client area coordinates

  if (!m_bTTVisible)
    {

    if (m_hTTWnd==NULL)
      {
      /* INITIALIZE COMMON CONTROLS */
      iccex.dwICC = ICC_WIN95_CLASSES;
      iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
      InitCommonControlsEx(&iccex);

      /* CREATE A TOOLTIP WINDOW */
      m_hTTWnd = CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        m_hWnd, NULL, ::AfxGetInstanceHandle(), NULL);

      ::SetWindowPos(m_hTTWnd,
        HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

      /* GET COORDINATES OF THE MAIN CLIENT AREA */
      ::GetClientRect (m_hWnd, &rect);

      /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
      ti.cbSize = sizeof(TOOLINFO);
      ti.uFlags = TTF_SUBCLASS ;//| TTF_TRACK;// | TTF_ABSOLUTE;
      ti.hwnd = m_hWnd;
      ti.hinst = ::AfxGetInstanceHandle();
      ti.uId = uid;
      ti.lpszText = LPSTR_TEXTCALLBACK;
      // ToolTip control will cover the whole window
      ti.rect.left = rect.left;    
      ti.rect.top = rect.top;
      ti.rect.right = rect.right;
      ti.rect.bottom = rect.bottom;

      /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
      //TRACE("---ADDTOOL\n");
      ::SendMessage(m_hTTWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
      }
    else
      ::SendMessage(m_hTTWnd, TTM_ACTIVATE,  (WPARAM) TRUE, 0);
    }
  m_bTTVisible=true;

  if (1)
    {
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS ;
    ti.hwnd = m_hWnd;
    ti.hinst = ::AfxGetInstanceHandle();
    ti.uId = uid;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    //TRACE("---UPDATETIPTEXT\n");
    ::SendMessage(m_hTTWnd, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
    }
  }

//---------------------------------------------------------------------------

void AccNodeWnd::HideTT()//UINT nFlags, CPoint point)
  {
  if (m_bTTVisible)
    {
    //TRACE("---HIDE\n");
    //::SendMessage(m_hTTWnd, TTM_POP, 0, 0);
    ::SendMessage(m_hTTWnd, TTM_ACTIVATE,  (WPARAM) FALSE, 0);
    }
  m_bTTVisible=false;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoMouseMove(UINT nFlags, CPoint point)
  {
  //#if _DEBUG
  //static long iStatic1=0;
  //TRACE1("MouseMove ### %i\n", iStatic1++);
  //#endif

  if (CPgNo>=0)
    {
    if (ObjOnPage[CPgNo]>=0)
      if (ObjEdit[ObjOnPage[CPgNo]]->DoMouseMove(nFlags, point))
        return 1;
    }
  if (HelpMngr.ShowToolTips() && CPg)
    {
    int Col=point.x/ScrGB.ColWdt();
    //int Row=point.y/ScrGB.RowHgt()-2;
    int Row=(point.y-CPg->Row[0]->Ys(ScrGB))/ScrGB.RowHgt();

    FxdEdtInfo EI;
    bool TTOn=false;
    if (m_iPrevTTRow==Row)// && m_iPrevTTCol==Col)
      {
      TTOn=true;
      }
    else if (Row>=0 && LocateFromCR(Col, Row, EI))
      {
      m_iPrevTTRow=-1;
      m_iPrevTTCol=-1;
      if (EI.FieldId<0 && EI.Fld)
        {
        int fi=abs(EI.FieldId)-Id_FldParms;
        if (fi < NFlds)
          {
          if (Flds[fi].fIsData)
            {
            AccNdFld & f=Flds[fi];
            m_sTTCls=f.pData->sClsTag();
            if (f.pData && f.pData->iFlags&DDEF_ISSPECIE)
              m_sTTTextHead="Specie : ";
            else if (f.pData && f.pData->iFlags&DDEF_ISSPECIEATT)
              m_sTTTextHead="Attribute : ";
            else if (f.pData && f.pData->iFlags&DDEF_ISSPECIECALC)
              m_sTTTextHead="Calculation : ";
            else
              m_sTTTextHead="";

            m_sTTTextHead+=f.pData->sFullTag();
            if (f.pData->sDesc())
              {
              m_sTTTextHead+="\r\n";
              m_sTTTextHead+=f.pData->sDesc();
              }
            //#if _DEBUG
            //static long iStatic=0;
            //TRACE1("MouseMove --- %i\n", iStatic++);
            //#endif

            ShowTT();
            TTOn=true;
            m_iPrevTTRow=Row;
            m_iPrevTTCol=Col;
            }
          //else
          //  HideTT();
          }
        }
      }
    if (!TTOn)
      {
      m_sTTTextHead="";
      HideTT();
      m_iPrevTTRow=-1;
      m_iPrevTTCol=-1;
      }
    }
  else
    {
    m_iPrevTTRow=-1;
    m_iPrevTTCol=-1;
    }
  return FxdEdtView::DoMouseMove(nFlags, point);
  }

//---------------------------------------------------------------------------

//Notification handler
BOOL AccNodeWnd::OnToolTipNotify(UINT id, NMHDR *pNMHDR,
                                 LRESULT *pResult)
  {
  switch (pNMHDR->code)
    {
    case TTN_GETDISPINFO:
      {
      LPNMTTDISPINFO lpttd = (LPNMTTDISPINFO)pNMHDR;
      if (m_sTTCls.GetLength()>0)
        {
        CString S=gs_AccessWnds.GetHelp(m_sTTCls);
        LPCTSTR ULine = "----------------------------------------------------------------------";
        m_sTTTextDisp=m_sTTTextHead;
        m_sTTTextDisp+="\r\n";
        m_sTTTextDisp+=ULine;
        m_sTTTextDisp+="\r\n";
        if (S.GetLength()>0)
          m_sTTTextDisp+=S;
        else
          m_sTTTextDisp+=m_sTTCls;
        }
      else
        m_sTTTextDisp="";
      //#if _DEBUG
      //static long iStatic=0;
      //TRACE1("TTNotify      %i\n", iStatic++);
      //#endif
      lpttd->lpszText = (LPTSTR)(LPCTSTR)m_sTTTextDisp;
      lpttd->uFlags |= TTF_DI_SETITEM;

      ::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 300);
      return TRUE;
      }
    }
  return FALSE;    // message was handled
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if (CPg)
    {
    switch (nChar)
      {
      case VK_F5:
      case VK_F6:
        {
        for (pFxdEdtFld pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
          {
          int fi=GetFieldIndex(pFld->FieldId);
          if (pFld->FieldId!=0 && fi<MaxAccNdFlds)
            {
            AccNdFld & f=Flds[fi];
            if (f.HasData())
              {
              AccNdData & d=f();
              switch (nChar)
                {
                case VK_F6:
                  {
                  TagInfoBlk * IB = new TagInfoBlk(ObjClassId(), d.iType, NdTag(), d.sRefTag(), &d.Cnv, &d.Fmt);
                  ScdMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
                  break;
                  }
                case VK_F5:
                  {
                  if (::SendMessage(CTagVwText::CurrentTrend(), WMU_ADDTAG2TREND, 0, (LPARAM)d.sFullTag())==0)
                    LogNote("Access", 0, "Tag %s not added to Trend", d.sFullTag()); 
                  }
                  break;
                }
              }
            }
          }
        RBActiveFld=-1;
        return true;
        }
      case VK_RETURN:
        if (bControlDown)
          PostMessage(WMU_ACCREFRESHSAVE);
        return true;

      }
    }
  return FxdEdtView::DoKeyDown(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::DoCutVal(pFxdEdtFld pFld, char *buff, int buffsize)
  {
  if (pFld->FieldId<0)
    {
    int fi=-(Id_FldParms+pFld->FieldId);
    ASSERT(fi<MaxAccNdFlds && fi>=0);
    if (Flds[fi].HasData())
      {
      int di=Flds[fi].iData;
      strncpy(buff, Data[di].sFullTag(), buffsize);
      XStrTrim(buff);
      AppendCnvText(buff, Data[di].Cnv.Text());
      //strcat(buff, " (");
      //strcat(buff, Data[di].Cnv.Text());
      //strcat(buff, ")");
      return true;
      }
    }

  return FxdEdtBook::DoCutVal(pFld, buff, buffsize);
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::DoCopyVal(pFxdEdtFld pFld, char *buff, int buffsize)
  {
  if (pFld->FieldId<0)
    {
    int fi=-(Id_FldParms+pFld->FieldId);
    ASSERT(fi<MaxAccNdFlds && fi>=0);
    if (Flds[fi].HasData())
      {
      const int di=Flds[fi].iData;
      strncpy(buff, Data[di].sFullTag(), buffsize);
      XStrTrim(buff);
      AppendCnvText(buff, Data[di].Cnv.Text());

//      strcat(buff, " (");
//      strcat(buff, Data[di].Cnv.Text());
//      strcat(buff, ")");
      return true;
      }
    }
  return FxdEdtBook::DoCopyVal(pFld, buff, buffsize);
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::DoPaste(pFxdEdtFld pFld, char *buff)
  {
  return FxdEdtBook::DoPaste(pFld, buff);
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
  return true;
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_Start(CXM_TimeControl &CB)
  {
  return true;
  };

//---------------------------------------------------------------------------

void AccNodeWnd::EO_QuerySubsReqd(CXMsgLst &XM)
  {
  XM.Clear();
  };

//---------------------------------------------------------------------------

void AccNodeWnd::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
  //XM.Clear();
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_ReadSubsData(CXMsgLst &XM)
  {
  #if dbgDataReadWrite
  dbgpln("AccNodeWnd Read  ============================================================");
  #endif
  flag DataRead=0;
  return DataRead;
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
  #if dbgDataReadWrite
  dbgpln("AccNodeWnd Write ============================================================");
  #endif
  return true;
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
  return false;
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_Stop(CXM_TimeControl &CB)
  {
  return true;
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_Starting(flag fBeginStarting)
  {
  if (!fBeginStarting && !FxdEdtView::Changed() && gs_pPrj->m_bAccUpdateOnRun)
    {
    SendOrPostMessage(this, !gs_TheRunMngr.AutomationBusy(), WMU_ACCREFRESHCANCEL);
    COrdWindow::Refresh();
    CWirePanelWindow::Refresh();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_Stopping(flag fBeginStopping)
  {
  if (!fBeginStopping && !FxdEdtView::Changed() && gs_pPrj->m_bAccUpdateOnRun)
    {
    SendOrPostMessage(this, !gs_TheRunMngr.AutomationBusy(), WMU_ACCREFRESHCANCEL);
    COrdWindow::Refresh();
    CStatsWindow::Refresh();
    CWirePanelWindow::Refresh();
    }
  return true;
  };

//---------------------------------------------------------------------------

void AccNodeWnd::EO_GlblResultValidity(flag IsValid, flag IsAlwaysValid)
  {
//  SendMessage(WMU_ACCREFRESHCANCEL);
//  PostMessage(WMU_ACCREFRESHCANCEL);
  };

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_BeginPause(CXM_TimeControl &CB)
  {
  if (!FxdEdtView::Changed() && gs_pPrj->m_bAccUpdateOnRun)
    {
    SendMessage(WMU_ACCREFRESHCANCEL);
    COrdWindow::Refresh();
    CStatsWindow::Refresh();
    CWirePanelWindow::Refresh();
    }
  return true;
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::EO_EndPause(CXM_TimeControl &CB)
  {
  return true;
  }

//---------------------------------------------------------------------------

int AccNodeWnd::EO_QueryChangeTag(pchar pOldTag, pchar pNewTag)
  {
  return EOCT_NOTFOUND;
  }

//--------------------------------------------------------------------------

int AccNodeWnd::EO_ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  const int Size = NdHistList.GetSize();
  const int len = strlen(pOldTag);
  for (int i=0; i<Size; i++)
    {
    if (NdHistList[i].Len()==len && _strnicmp(NdHistList[i](), pOldTag, len)==0)
      NdHistList[i] = pNewTag;
    }
  return EOCT_DONE;
  }

//---------------------------------------------------------------------------

int AccNodeWnd::EO_DeleteTag(pchar pDelTag)
  {
  const int len = strlen(pDelTag);
  for (int i=0; i<NdHistList.GetSize(); i++)
    {
    if (NdHistList[i].Len()==len && _strnicmp(NdHistList[i](), pDelTag, len)==0)
      NdHistList.RemoveAt(i);
    }
  if (NdTag.Len()==len && _strnicmp(NdTag(), pDelTag, len)==0)
    {
    /*gs_pAccWnd->*/CloseAccessData(false, true, true);
    /*gs_pAccWnd->*/CloseWnd();
    }
  return EODT_DONE;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::OnShowWindow(BOOL bShow, UINT nStatus)
  {
  FxdEdtView::OnShowWindow(bShow, nStatus);
  }

//---------------------------------------------------------------------------

flag AccNodeWnd::BuildDataMessage(long DataIndex, Strng & Tag, Strng & RefTag, Strng & BlkName, CXM_ObjectData &ObjData)
  {
  for (int di = 0; (di < NData); di++ )
    {
    AccNdData &d=Data[di];
//    dbgpln("Fld %3i Typ:%-16s S:%i E:%i Lvl:%3i :%s",
//          di, tt_TypeString(d.iType), d.iStrtFld, d.iEndFld, d.iLvl, d.sSymOrTag());

    if (di==DataIndex)
      {
      if (FullFldName(RefTag, Data, di))
        {
      //RefTag=d.sRefTag;
        CXM_ObjectTag ObjTag(d.sFullTag(), TABOpt_AllInfo|TABOpt_ForView);
        CXM_Route ObjRoute;

        DWORD RetCode = XReadTaggedItem(ObjTag, ObjData, ObjRoute);
        if (RetCode)
          return true;
        }
      }
    }
  return false;
  }

//---------------------------------------------------------------------------

void AccNodeWnd::SendDataMessage(CXM_ObjectTag &ObjTag, CXM_ObjectData &ObjData)
  {
  }

//===========================================================================
//
//
//
//===========================================================================

//CAccDoc is implemented for printing functionality in the AccessWindow
IMPLEMENT_DYNCREATE(CAccDoc, CDocument)

CAccDoc::CAccDoc()
  {
  m_bAutoDelete = false;
  }

//---------------------------------------------------------------------------

BOOL CAccDoc::OnNewDocument()
  {
  if (!CDocument::OnNewDocument())
    return false;
  return true;
  }

//---------------------------------------------------------------------------

CAccDoc::~CAccDoc()
  {
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAccDoc, CDocument)
  //{{AFX_MSG_MAP(CAccDoc)
  ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

#ifdef _DEBUG
void CAccDoc::AssertValid() const
  {
  CDocument::AssertValid();
  }

void CAccDoc::Dump(CDumpContext& dc) const
  {
  CDocument::Dump(dc);
  }
#endif //_DEBUG

//---------------------------------------------------------------------------

void CAccDoc::OnUpdateFileClose(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(false);
  }

//---------------------------------------------------------------------------

void CAccDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(false);
  }

//---------------------------------------------------------------------------

void CAccDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
  {
  pCmdUI->Enable(false);
  }

//===========================================================================
//
//
//
//===========================================================================



CAccessWnds::CAccessWnds()
  {
  m_nWnds=0;
  m_iActive=0;
  //m_RS1=NULL;
  }; 

CAccessWnds::~CAccessWnds()
  {
  }; 

bool CAccessWnds::InitHelp(LPCTSTR Path)
  {
  CString Connect;
  Connect.Format("Provider=%s;Data Source=%s", "Microsoft.Jet.OLEDB.4.0", (LPCTSTR)Path);
                   // OR Driver={Microsoft Access Driver (*.mdb)};DBQ=physical path to .mdb file
  m_bsConnect=Connect;
  try
    {
    HRESULT hr=m_RS1.CreateInstance(__uuidof(ADODB::Recordset));
    if (SUCCEEDED(hr))
      {
      
      return true;
      }
    else
      {
      LogError("TagHelp", 0, "ADODB::Recordset not Created : %s", Path);
      }
    }
  catch (...)
    {
    LogError("TagHelp", 0, "Exception Occurred");
    int xxx=0;
    }
  return false;
  }

void CAccessWnds::TermHelp()
  {
  if (m_RS1)
    m_RS1.Release();
  }

CString CAccessWnds::GetHelp(LPCTSTR Tag)
  {
  USES_CONVERSION;
  CString Desc;
  try
    {
    CString Src;
    Src.Format("SELECT * FROM TagDescriptor WHERE ClassTag = '%s'", Tag);
    _bstr_t Source(Src);
    m_RS1->Open(Source, m_bsConnect, ADODB::adOpenForwardOnly, ADODB::adLockOptimistic/*ReadOnly*/, -1);
    if ( m_RS1->adEOF == VARIANT_FALSE )
      {
      _variant_t v=m_RS1->Fields->Item[ _variant_t("Description")]->Value;
      
      if (v.vt==VT_BSTR)
        {
        Desc=W2CT(v.bstrVal);
        };

      dbgpln("Desc(%s):%s", Tag,(LPCTSTR)Desc);
      //m_RS1->MoveNext();
      }
    m_RS1->Close();
    }
  catch(...)//CADOExeception * pE)
    {
    //m_RS1->Close();
    }
  return Desc;
  }
void CAccessWnds::Add(AccNodeWnd *pAccWnd, CMDIChildWnd *pAccFrame)
  {
  if (m_nWnds<2)
    {
    m_pAccWnds[m_nWnds]=pAccWnd;
    m_pAccFrames[m_nWnds]=pAccFrame;
    pAccWnd->m_pMyFrm=pAccFrame;
    m_nWnds++;
    }
  }

void CAccessWnds::MarkAsActive(AccNodeWnd *pAccWnd)
  {
  for (int i=0; i<NAccessWnds; i++)
    if (pAccWnd==m_pAccWnds[i])
      {
      m_iActive=i;
      break;
      };
  };

void CAccessWnds::AccessNode(long Which, LPCTSTR pTag, bool AllowDeferred) 
  {
  if (Which>=0 && Which<NAccessWnds)
    m_pAccWnds[Which]->AccessNode(pTag, AllowDeferred);
  else if (Which==-1)
    m_pAccWnds[m_iActive]->AccessNode(pTag, AllowDeferred);
  else
    {
    m_iActive=(m_iActive+1)%NAccessWnds;
    m_pAccWnds[m_iActive]->AccessNode(pTag, AllowDeferred);
    }
  };

void CAccessWnds::CloseAccessData(long Which, flag SaveIt, flag CancelIt, flag Disconnect) 
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->CloseAccessData(SaveIt, CancelIt, Disconnect);
  };
void CAccessWnds::CloseWnd(long Which)
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->CloseWnd();
  };

void CAccessWnds::DoDeferredAccess(long Which)
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->DoDeferredAccess();
  };

void CAccessWnds::PostMessage(long Which, UINT Msg, WPARAM W, LPARAM L)
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->PostMessage(Msg, W, L);
  };

void CAccessWnds::LoadNdHist(long Which)
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->LoadNdHist();
  };
void CAccessWnds::SaveNdHist(long Which)
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->SaveNdHist();
  };
void CAccessWnds::RefreshData(long Which, flag SaveIt)
  {
  long iS=(Which>=0 ? Which:0);
  long iN=(Which>=0 ? Which+1:NAccessWnds);
  for (int i=iS; i<iN; i++)
    m_pAccWnds[i]->RefreshData(SaveIt);
  };

char* CAccessWnds::CurTag(long Which)
  {
  if (Which>=0 && Which<NAccessWnds)
    return m_pAccWnds[Which]->CurTag();
  return NULL;
  };

//===========================================================================
//
//
//
//===========================================================================

CAccessWnds gs_AccessWnds;

//===========================================================================
//
//
//
//===========================================================================

