//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define  __CMDMENU_CPP
#include "sc_defs.h"
#include "grf3drw.h"
#include "cmds.h"
#include "resource.h"
#include "cmdmenu.h"
#include "docroot.h"
#include "project.h"
#include "syscad.h"

#define AddRet         0x01
#define AddSpc         0x02

#define dbgOnCmdWndStr  0
#define dbgProcess      0
#define dbgParseCmdLine 0
#define dbgExecCmdLine  0
#define dbgLoadEx       0
#define dbgButtonUpDn   0

const pchar N_A    = NULL;
const pchar AName  = "_A";
const pchar AFloat = "_F";
const pchar AnInt  = "_I";
const pchar AAny   = "_X";

#define STATIC_FN

//===========================================================================

CommandBlk::CommandBlk(int MaxVNIn, int MaxCSIn)
  {
  VNLen=0;
  MaxVN=Max(2,MaxVNIn+1);
  VNTable=new VerbNounEntry[MaxVN];

  CSLen=0;
  MaxCS=Max(2,MaxCSIn+1);
  CSTable=new CmdIDStrEntry[MaxCS];

  ObjAccFn=new VerbNounEntry;
  ObjAccFn->Exec=NULL;
  ObjVwFn=new VerbNounEntry;
  ObjVwFn->Exec=NULL;
  gs_pCmd->VNTChanged();
  };

//---------------------------------------------------------------------------

CommandBlk::~CommandBlk()
  {
  //gs_pCmd->VNTInvalid();
  delete [] VNTable;
  delete [] CSTable;
  delete ObjAccFn;
  delete ObjVwFn;
  };

//---------------------------------------------------------------------------

void CommandBlk::InitForCmd()
  {
  };

//---------------------------------------------------------------------------

void CommandBlk::InitVNT()
  {
  VNLen=0;
  VNTable[0].Set(NULL,NULL,NULL,0,NULL,0);
  CSLen=0;
  CSTable[0].Set(0,NULL);
  ObjAccFn->Set(NULL,NULL,NULL,0,NULL,0);
  ObjVwFn->Set(NULL,NULL,NULL,0,NULL,0);
  };

//---------------------------------------------------------------------------

flag CommandBlk::UpdateCmdStr(int nID, CCmdUI* pCmdUI)
  {
  return False;
  };

//---------------------------------------------------------------------------

flag CommandBlk::ProcessCmdStr(int nID, rStrng Str)
  {
  return True;
  };

/*
//---------------------------------------------------------------------------

void CommandBlk::SetVNT(pchar Verb_, pchar Noun_, pchar Mods_, char DefModNo_, CmdFn Exec_, int Flags_)
  {
  int n1=(((VNLen)/10)+1)*10;
  int n2=(((VNLen+1)/10)+1)*10;
  if (n2>n1)
    {
    VerbNounTable VNT2=new VerbNounEntry[n2];
    for (int i=0; i<n1; i++)
      VNT2[i]=VNTable[i];
    if (VNT)
      delete[] VNT;
    VNT=VNT2;
    }

  VNTable[VNLen++].Set(Verb_, Noun_, Mods_, DefModNo_, Exec_, Flags_);
  };

//---------------------------------------------------------------------------

void CommandBlk::SetObjAccFn(CmdFn Exec_, int Flags_);
  {
  if (ObjAccFn==NULL)
    ObjAccFn= new VernNounEntry;
  ObjAccFn->Set(Exec_, Flags_);
  };
*/
//===========================================================================

IMPLEMENT_DYNCREATE(CommandWnd, CView)

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CommandWnd, CView )
  ON_COMMAND(IDM_BUMPINPUT, OnBumpInput)
  ON_MESSAGE(IDM_CMDWND_STR, OnCmdWndStr)
  ON_WM_CREATE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_SYSCHAR()
//  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_HSCROLL()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_CLOSE()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CommandWnd::CommandWnd()
  {
  //Scroller=NULL;
  HScrl=NULL;
  VScrl=NULL;
  VScrlOn=0;
  HScrlOn=0;

  CurCmdNo = 1;
  CL.Id = CurCmdNo;
  ClearCmdLn();
  StrNull(CL.AsTokens);

  DefModNo=-1;

  fShiftDown = 0;
  fCtrlDown = 0;
  AutSpc = 0;
  KBDBlock = 0;
  BTNBlock = 1;
  DblClkBlock = 0;
  OutFile = NULL;
  StrNull(BTNDefModSpc);
  BTNDefModVNT=NULL;
  StrNull(PushedModifier);
  StrNull(ExtendedModifier);

  //CmdLn=CmdLnIn;
  CmdLn=NULL;
  CmdLnTo=(CmdLn!=NULL);

  bVNTChanged=0;
  bEnabled=1;
  //bVNTInvalid=0;
  LastCmdBlk_Cur = -1;
  LastCmdBlk_Wrk = -1;
  for (int i=0; i<MaxVNTDepth; i++)
    {
    CmdBlk_Cur[i] = NULL;
    CmdBlk_Wrk[i] = NULL;
    }
  LastTokenNo = 0;
  for (i=0; i<MaxToks; i++)
    {
    TL[i].Prmpt = NULL;
    TL[i].CBlk = NULL;
    TL[i].CCmd = NULL;
    TL[i].Data = NULL;
    }

  InitForCmd();

  bFontsInited=0;

  LinesInRect = 0; //40;

  for (i= 0; i < LinesInRect; i++)
    {
    char xxx[20];
    sprintf(xxx, "Line %i", i);
    pStrng s = new Strng();
    Lines.Append(s);
    }

  //pDocForCmds=NULL;

  //bCmdFromMenu=0;
  //bStrIsCmdFromMenu=0;

  /****
  for (int i = 0; i < strlen(cmd); i++)
    {
    int c = VkKeyScan(cmd[i]);
    SendMessage(Cmd->HWindow, WM_CHAR, LOBYTE(c), 0);
    }
  if (dowhat & AddSpc)
    SendMessage(Cmd->HWindow, WM_CHAR, VK_SPACE, 0);
  if (dowhat & AddRet)
    SendMessage(Cmd->HWindow, WM_CHAR, VK_RETURN, 0);
???
  ***/
  pBusyDocument = NULL;
  bChgCmdLnBusy = 0;
  bExecBusy = 0;
  bEscBusy = 0;
  bDlgBusy = 0;
  };

//---------------------------------------------------------------------------

CommandWnd::~CommandWnd()
  {
  if (HScrl) delete HScrl;
  if (VScrl) delete VScrl;
  gs_pCmd = NULL; //KGA 17/6/96 : fix shut-down bug where command window is 
               //closed before CloseCommandBlk(..) is called
  };

//---------------------------------------------------------------------------

BOOL CommandWnd::PreCreateWindow( CREATESTRUCT& cs )
  {
  //cs.style |= WS_CLIPSIBLINGS;
  cs.lpszClass=AfxRegisterWndClass(/*CS_DBLCLKS |*/ CS_HREDRAW | CS_VREDRAW , //| CS_OWNDC,
                                    ScdApp()->LoadStandardCursor(IDC_IBEAM),
                                   (HBRUSH)GetStockObject(WHITE_BRUSH), 0);
  return CView::PreCreateWindow(cs);
  };

//---------------------------------------------------------------------------

int CommandWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!bFontsInited)
    {
    bFontsInited=1;
    SysFont.CreateFontIndirect(&SysCADFontStruct[0]);

    CClientDC dc(this);
    CDCResChk ResChk(dc);

    CFont *OldFont= dc.SelectObject(&SysFont);

    TEXTMETRIC Metrics;
    if (dc.GetTextMetrics(&Metrics))
      {
      CharSize.cx = Metrics.tmMaxCharWidth;
      CharSize.cy = Metrics.tmHeight + Metrics.tmExternalLeading;
      CharAscent = Metrics.tmAscent;
      CharOffset.cx = CharSize.cx/2;
      CharOffset.cy = 0;
      }
    else
      {
      CharSize.cx = 6;
      CharSize.cy = 10;
      CharAscent = 2;
      CharOffset.cx = 4;
      CharOffset.cy = 0;
      }
    dc.SelectObject(OldFont);
    }
 
  return 0;
  }

//---------------------------------------------------------------------------

void CommandWnd::Initialise()//pCommandBlk CBlk, CWnd* BaseWndIn)
  {
  //OpenVerbNounTable(VNTIn, BaseWndIn, NULL);
  CRect r;
  GetClientRect(&r);
  FixScrollSizes(TRUE, TRUE);
  /**
  Scroller = new CScrollBar();
  Scroller->Create(WS_CHILD|WS_VISIBLE|SBS_VERT|SBS_RIGHTALIGN, r, this, -1);
  Scroller->SetScrollPos(0, FALSE);
  Scroller->SetScrollRange(0,LinesInRect);
  **/
  }

//---------------------------------------------------------------------------

void CommandWnd::Terminate()//CWnd* BaseWndIn)
  {
//  CloseVerbNounTable(BaseWndIn);
  //delete Scroller;
  }

//---------------------------------------------------------------------------

void CommandWnd::ClearCmdLn()
  {
  StrNull(CL.Text);
  StrNull(CL.MoreText);
//  StrNull(CL.AsTokens);
  StrNull(CL.Extra);
  FndRep.Line=0;
  StrNull(FndRep.Text);
  StrNull(BTNDefModSpc);
  BTNDefModVNT=NULL;
  StrNull(PushedModifier);
  StrNull(ExtendedModifier);
  DefModNo=-1;
  IsUserText=0;
  }

//---------------------------------------------------------------------------

void CommandWnd::InitForCmd(int Strt, int End)
  {
  ASSERT(End<MaxToks);
  bExecBusy = 0;
  if (End ==-1)
    End=MaxToks-1;
  for (int i = Strt; i <= End ; i++)
    {
    if (TL[i].Prmpt)
      delete TL[i].Prmpt;
    TL[i].Str = NULL;
    TL[i].StrInText = NULL;
    TL[i].Prmpt = NULL;
    TL[i].Done = 0;
    TL[i].NDigs = 0;
    TL[i].NParms = 0;
    TL[i].FreeForm = 0;
    TL[i].CursNo = GC_NoCurs;
    TL[i].CBlk = NULL;
    TL[i].CCmd = NULL;
    TL[i].NounNo = 0;
    TL[i].ModNo = 0;
    TL[i].Data = NULL;
    }

  if (Strt==0)
    {
    //bCmdFromMenu=bStrIsCmdFromMenu;
    for (i = 0; i <= LastCmdBlk_Cur ; i++)
      CmdBlk_Cur[i]->InitForCmd();
    }
  }

//---------------------------------------------------------------------------

CDocument* CommandWnd::FindVNTDocument()
  {
  /*
  CMainFrame *pM = (CMainFrame *)SysCAD.m_pMainWnd;
  CFrameWnd *pF  = pM->GetActiveFrame();
  CDocument* pDoc = pF->GetActiveDocument();
  */
  CDocument* pDoc = DocRoot::FirstDoc();
  //TRACE("pDocX %8x %8x\n", pF, pDoc);
  //ASSERT(pDoc->IsKindOf(DocRoot)); // Must be a Root Document
  return pDoc;
  }

//---------------------------------------------------------------------------

void CommandWnd::ActivateVNT()
  {
  // Unwind (Close) Currently Open VNTs and then Open those VNTs which are required
  bEnabled=true;
  if (strlen(CL.Text)==0)
    {
    FlushPrintList();
    CDocument *pD = FindVNTDocument();
    pDocRoot pDoc = (pDocRoot)pD;
//    pDocRoot pDoc = (pDocRoot)(pDocForCmds ? pDocForCmds : SysCAD.DocumentNo(0));
    if (bVNTChanged || pD!=CurrentVNTDocument/*bVNTInvalid*/ || (pDoc && pDoc->lVNTChkSum==0))
      {
      //TRACE("ActivateVNT\n");

      CurrentVNTDocument=pD;
      bVNTChanged=0;
      //bVNTInvalid=0;

      while (LastCmdBlk_Wrk >=0 && !CmdBlk_Wrk_P[LastCmdBlk_Wrk]) // close all temp cmdblks
        LastCmdBlk_Wrk--;
      if (pDoc)
        {
        pDoc->OpenVNT();
        pStatusBar->UpdateIndicator(0, (char*)(const char*)pDoc->GetTitle(), FALSE);
        }
      else //if (MainWnd())
        pStatusBar->UpdateIndicator(0, "", FALSE);
            
      LastCmdBlk_Cur=LastCmdBlk_Wrk;

      long lVNTChkSum=0; 
      for (int i = 0; i <= LastCmdBlk_Cur ; i++)
        {
        CmdBlk_Cur[i]=CmdBlk_Wrk[i];
        CmdBlk_Cur_P[i]=CmdBlk_Wrk_P[i];
        lVNTChkSum=(lVNTChkSum << 1) ^ (long)CmdBlk_Cur[i];
        }

      if (pDoc)
        {
        if ((pDoc->lVNTChkSum==0 || pDoc->lVNTChkSum!=lVNTChkSum) && !gs_pPrj->LoadBusy())
          {
          //TRACE("SelectVNT %8x - %8x\n", pDoc->lVNTChkSum ,lVNTChkSum);
          pDoc->lVNTChkSum=lVNTChkSum;
          for (int i = 0; i <= LastCmdBlk_Cur ; i++)
            CmdBlk_Cur[i]->SelectVNT();
          }
        // Force rebuild of VNT after Document Load
        if (gs_pPrj->LoadBusy())
          pDoc->lVNTChkSum=0;
        }
        
      static int OpenNo=0; 
      OpenNo++; 
      for (i = 0; i <= LastCmdBlk_Cur ; i++)
        {
        pCommandBlk CB=CmdBlk_Cur[i];
        //TRACE("%3i]Open VNT %8x \n", OpenNo,CB);
        //CB->VNLen=0;
        CB->InitVNT();
        CB->BuildVNT();
        CB->VNTable[CB->VNLen++].Set(NULL,NULL,NULL,0,NULL,0);
        ASSERT_ALWAYS(CB->VNLen<CB->MaxVN, "Too Many Verbs");
        CB->CSTable[CB->CSLen++].Set(0,NULL);
        ASSERT_ALWAYS(CB->CSLen<CB->MaxCS, "Too Many Cmds");
        }
      //InitForCmd();
      }
    InitForCmd();
    }
    //for (i = 0; i <= LastCmdBlk_Cur ; i++)
    //  CmdBlk_Cur[i]->InitForCmd();
  }

//---------------------------------------------------------------------------

void CommandWnd::OpenCommandBlk(pCommandBlk CmdBlk_, flag Permanent/*, CWnd* BaseWndIn*/)
  {
  //TRACE("--Open CommandBlk %x \n",CmdBlk_);
  LastCmdBlk_Wrk++;
  CmdBlk_Wrk[LastCmdBlk_Wrk] = CmdBlk_;
  CmdBlk_Wrk_P[LastCmdBlk_Wrk] = Permanent;
  //bVNTChanged=1;

  /**
  CmdBlk_->VNLen=0;
  CmdBlk_->BuildVNT();
  CmdBlk_->VNTable[CmdBlk_->VNLen++].Set(NULL,NULL,NULL,0,NULL,0);
  VERIFY(CmdBlk_->VNLen<CmdBlk_->MaxVN);
  CmdBlk_->CSTable[CmdBlk_->CSLen++].Set(0,NULL);
  VERIFY(CmdBlk_->CSLen<CmdBlk_->MaxCS);
  **/
  //IFC[LastCmdBlk] = IFCIn;
  //BaseWnd[LastCmdBlk] = BaseWndIn;
  /**
  int i = 0;
  while (VNTable[LastCmdBlk][i].Flags)
    {
    if (VNTable[LastCmdBlk][i].BaseWnd == NULL)
      VNTable[LastCmdBlk][i].BaseWnd = BaseWndIn;
    i++;
    }
  **/
  }

//---------------------------------------------------------------------------

void CommandWnd::CloseCommandBlk(pCommandBlk CmdBlk_)
  {
  //ProcessAStr("\x1b");
  //int i = LastCmdBlk_Wrk, Done=0;
  int Done=0;
  while (LastCmdBlk_Wrk >= 0 && !Done)
    {
    Done=CmdBlk_==CmdBlk_Wrk[LastCmdBlk_Wrk];
    //CmdBlk[i]->CloseVNT();
    //TRACE("--CloseCommandBlk %x\n",CmdBlk_Wrk[LastCmdBlk_Wrk]);
    CmdBlk_Wrk[LastCmdBlk_Wrk] = NULL;
    LastCmdBlk_Wrk--;
    //LastCmdBlk_Wrk=i;
    }
  //VNTChg=1;
  }

//---------------------------------------------------------------------------

void CommandWnd::CloseCommandBlksTo(pCommandBlk CmdBlk_)
  {
  for (int i=0; i<LastCmdBlk_Wrk; i++)
    if (CmdBlk_Wrk[i] == CmdBlk_)
      {
      CloseCommandBlk(CmdBlk_Wrk[i+1]);
      break;
      }
  //VNTChg=1;
  }

//---------------------------------------------------------------------------

void CommandWnd::Repaint(char All)
  {
  int V;
  if (All)// || (Scroller==NULL))
    {

    //::InvalidateRect(m_hWnd, NULL, TRUE);
    InvalidateRect(NULL, TRUE);
    RedrawWindow();
//cnm    ::SendMessage(m_hWnd, WM_PAINT,0,0);
    }
  else if (CmdLnVisible(V))
    {
    CRect Rect, Cr;
    //::GetClientRect(m_hWnd, &Rect);
    GetClientRect(&Rect);
    Cr=Rect;
    
    //Rect.top = CharOffset.cy+((Lines.Length()/*-Scroller->GetScrollPos()*/)*CharSize.cy);
    Rect.top = CharOffset.cy+V*CharSize.cy;
    Rect.bottom = Rect.top+CharSize.cy;
    Rect.left  = CharOffset.cx;
    Rect.right = CharOffset.cx+CharSize.cx*ChWorkWidth(Cr, VScrlOn);
    //::InvalidateRect(m_hWnd, &Rect, TRUE);
    InvalidateRect(&Rect, FALSE);//TRUE);
//cnm    ::SendMessage(m_hWnd, WM_PAINT,0,0);
    }
  }

//---------------------------------------------------------------------------

STATIC_FN int MakeCmdLine(Strng &S, CmdLnStruct &CL)
  {
  if (CL.Id > 0)
    {
    S.Set("%i> %s", CL.Id, CL.Text);
    int l = S.Length();
    if (strlen(CL.Extra) > 0)
      {
      S.Append(CL.Extra);
      S.Append(" ");
      }
    S.Append(CL.MoreText);
    return l;
    }
  else if (CL.Id < 0)
    S.Set("%s", CL.Text);
  else
    S.Set(" ");
  return 0;
  }

//---------------------------------------------------------------------------
/*
flag CommandWnd::ResetScroller(flag Force)
  {
  FixScrollSizes(TRUE, Force);
  //if (Scroller)
    {
    RECT Rect;
    ::GetClientRect(m_hWnd, &Rect);
    LinesInRect = ((Rect.bottom-Rect.top) / CharSize.cy); //-1;
    long dy = LinesInRect;
    long y = Max((long)(Lines.Length()+1)-dy,(long)0);
    int YPos, YRange;
    
    //Scroller->GetScrollRange(&YMn, &YMx);
    //YPos=Scroller->GetScrollPos();
    //YRange = YMx-YMn;
    
    YPos=0;
    YRange = 0;
    if (Force || YRange < y || YPos < y)
      {
      
      //Scroller->SetScrollPos(y);
      //Scroller->SetScrollRange(0,y);
  //x    Scroller->SetPageSize();
  //x    Scroller->ScrollTo(0,y); // will repaint
  //x    ScrollWindow(0,-CharSize.cy);
      Invalidate();
//cnm      UpdateWindow();
      return 1;
      }
    }
  return 0;
  }
*/
//---------------------------------------------------------------------------

void CommandWnd:: SetScaling(CDC * dc)
  {
  CRect rect;
  GetClientRect( rect );
  //Ext.cx=(rect.right-rect.left)/CharSize.cx;
  //Ext.cy=(rect.bottom-rect.top)/CharSize.cy;
  //Org.x=0;
  //Org.y=0;//Max((int)0, (int)((Lines.Length()-Ext.cy+1)*CharSize.cy));

  dc->SetMapMode(MM_TEXT);
//  dc->SetWindowExt(1,1);
  dc->SetWindowOrg(0,0);
//  dc->SetViewportExt(CharSize.cx, CharSize.cy);

  dc->SetViewportOrg(0,0);//Org.x, -Org.y-CharOffset.cy);

//  dc->SetViewportOrg(((rect.right-rect.left)-(Ext.cx*CharSize.cx))/2,
//                     ((rect.bottom-rect.top)-(Ext.cy*CharSize.cy))/2);

//  dbgpln("SetScaling: Ext:%i,%i  Org:%i,%i Lines%i", Ext.cx, Ext.cy, Org.x, Org.y, Lines.Length());

  }

//---------------------------------------------------------------------------

flag CommandWnd::CmdLnVisible(int &Row)
  {
  flag Vis;
  if (VScrlOn)
    {
    CRect Cr;
    GetClientRect(Cr);
    int VMn, VMx, Wh;
    VScrl->GetScrollRange(&VMn, &VMx);
    Wh=ChWorkHeight(Cr, HScrlOn);
    Row=Wh-1 + (VMx-VScrl->GetScrollPos());
    Vis=(Row>=0 && Row < Wh);
    }
  else
    {
    Row=Lines.Length();
    Vis=True;
    }
  //TRACE("CmdLnVis %s, %i", Vis? "Visible":"       ", Row);
  return Vis;
  }

//---------------------------------------------------------------------------

void CommandWnd::DisplayCmdLn(CDC *dc)
  {
  int V;
  if (CmdLnVisible(V))
    {
    Strng S("");
    int cx = MakeCmdLine(S, CL);
  //  int xl = S.Length()-strlen(CL.Text);
  //  int l = S.Length();
    S += "                                                           ";
    flag Makedc=(dc==NULL);
    if (Makedc)
      {
      dc = new CClientDC(this);
      SetScaling(dc);
      }

    POINT Pt;

    //int YScr= VScrl ? VScrl->GetScrollPos() : 0;
    int H=HScrl->GetScrollPos();

    Pt.x=CharOffset.cx - CharSize.cx*H;
    Pt.y=CharOffset.cy + CharSize.cy*V;//(ChWorkHeight(Cr, VScrlOn)-1);

    //Pt.x=CharOffset.cx;
    //Pt.y=Row*CharSize.cy;

    CFont *OldFont = dc->SelectObject(&SysFont);
    dc->TextOut(Pt.x,Pt.y, S.Str(), S.Length());

    //dbgpln("DspCmdLn Pt%i,%i Lines%i Char.y%i YScr%i",Pt.x,Pt.y, Lines.Length(), CharSize.cy, YScr);

    Pt.x += CharSize.cx * cx;
    Pt.y += CharSize.cy;
    SetCaretPos(Pt);

    dc->SelectObject(OldFont);
    if (Makedc)
      {
  //    dc->DeleteDC();
      delete dc;
      }
    }
  }

//---------------------------------------------------------------------------

//void CommandWnd::OnPaint()
void CommandWnd::OnDraw(CDC *pDC)
  {
//  CPaintDC dc( this );
  SetScaling(pDC);

//  dc.SetTextAlign( TA_BASELINE | TA_CENTER );
//  dc.SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
//  dc.SetBkMode(TRANSPARENT);


  CFont *OldFont = pDC->SelectObject(&SysFont);

  CRect Cr;
  GetClientRect( Cr );
  //dbgpln("Paint %i, %i ",Cr.top,Cr.bottom);

  /***
  pStrng p = Lines.First();
  while ((Lines.Length() > LinesInCr) && (p->Length()==0))
    {
    Lines.Remove(p) ;
    delete p;
    p = Lines.First();
    }
  ***/
  
  //int VMn, VMx;
  //VScrl->GetScrollRange(&VMn, &VMx);
  //int V=VScrl->GetScrollPos();
  
  pStrng s=Lines.Last();
  int V,VLast;
  if (CmdLnVisible(V))
    {
    DisplayCmdLn(pDC);
    V--;
    }
  else
    {
    V--;
    VLast=ChWorkHeight(Cr, VScrlOn);
    while (s && V>VLast)
      {
      V--;
      s = s->Prev();
      }
    }

  int H=HScrl->GetScrollPos();

  int x=CharOffset.cx - CharSize.cx*H;
  int y=CharOffset.cy + CharSize.cy*V;//(ChWorkHeight(Cr, VScrlOn)-1);
  while (s)
    {
    pchar p=s->Str();
    flag IsCmd=StrIsCmdLn(p);

    pDC->TextOut(x + (IsCmd ? 0 : CharSize.cx*2),y, s->Str(), s->Length());
    s = s->Prev();
    y -= CharSize.cy;
    }


//  pDC->SelectObject(hOldFont);

  if (CmdLnTo)
    {
    CmdLnTo=0;

    const int MaxCmds=10;
    int nl=0;
    pchar pl[MaxCmds], p=CmdLn;
    while (*p==' ') p++;
    if (*p=='"')
      {
      do
        {
        p++;
        pl[nl++]=p;
        p=strchr(p, '\"');
        if (p)
          {
          *p='\0';
          p++;
          while (*p==' ' || *p==',') p++;
          }
        }
      while (p && *p=='"' && (nl < MaxCmds));
      }
    else
      pl[nl++]=p;

    for (int i=0; i<nl; i++)
      if (pl[i] && (strlen(pl[i])>0))
        {
//dbgpln("\"%s\"",pl[i]);
        ProcessAStr(pl[i]);
        ProcessAStr("\r");
        }
    }
  if (OldFont)
    pDC->SelectObject(OldFont);
  }

//---------------------------------------------------------------------------

flag CommandWnd::StrIsCmdLn(pchar p) 
  {
  int IsCmd=1;
  while (p && *p)
    if (isdigit(*p))
      p++;
    else
      {
      if (*p!='>')
        IsCmd=0;
      break;
      }
  return IsCmd;
  }
//---------------------------------------------------------------------------

void CommandWnd::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
  {
  if (lHint==0)
    Invalidate();
  };

//---------------------------------------------------------------------------

void CommandWnd::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
  {
  CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
  //bCmdActive=bActivate;
  /**
  if (bActivate)
    {
    DOCPOSITION DPos;
    CDocument* pDoc=SysCAD.FirstDoc(RUNTIME_CLASS(CDocument), DPos);
    if (pDoc)
      {
      POSITION VPos = pDoc->GetFirstViewPosition();
      CView* pFirstView = pDoc->GetNextView( VPos );
      MDIActivateThis(pFirstView);
      //pGrfWnd pWnd=(pGrfWnd)pFirstView;

      //pDoc->OnCloseDocument();
      //pDoc=SysCAD.NextDoc(Pos);
      }
//    int i=0;
    }
  **/
  }

//---------------------------------------------------------------------------

void CommandWnd::LnIsCommand(char Yes)
  {
  if ((CL.Id >= 0) && !Yes)
    {
    if (CL.Id > 0)
      CurCmdNo--;
    CL.Id = -1;
    }
  else if ((CL.Id <= 0) && Yes)
    CL.Id = ++CurCmdNo;
  }

//---------------------------------------------------------------------------

void CommandWnd::Print(pchar fmt, ...)
  {
  char Msg[512];
  va_list argptr;
  va_start(argptr,fmt);
  vsprintf(Msg, fmt, argptr);
  va_end(argptr);

  pStrng p=new Strng (Msg);
  PrintList.Append(p);
  }

//---------------------------------------------------------------------------

void CommandWnd::FlushPrintList()
  {
  int n=0;
  while (PrintList.Length()>0)
    {
    pStrng pS=PrintList.First();
    PrintList.Remove(pS);
    pchar pMsg=pS->Str();
    if (pMsg==NULL)
      pMsg="";

    if (OutFile)
      fprintf(OutFile, "; %s", pMsg);

    pchar p = strchr(pMsg, '\n');
    if (p)
      *p = 0;

    //CmdLnVisible(&V1);
    strcat(CL.Text, pMsg);
    LnIsCommand(0);
    DisplayCmdLn(NULL);
    //CmdLnVisible(&V2);

    if (p)
      Scroll(0);
    delete pS;
    n++;
    }
  LnIsCommand(1);
  if (n>0)
    {
    FixScrollSizes(TRUE, TRUE);
    Invalidate();
    }
  //else
  //  DisplayCmdLn(NULL);
  }

//---------------------------------------------------------------------------

static FILE *(Inputfile[5]);
static pStrng_List InputParms[5];
static byte Inputfiles=0;

STATIC_FN far void DoRedirectInput(/*CommandWnd *Cmd,*/ pchar FnTok)
  {
///  Cmd->ProcessAChar('\r');

  if (Inputfiles >= 5)
    gs_pCmd->Print("Too many Execute files\n");
  else
    {
    char Fn[512];
    strcpy(Fn, "");
    if (strpbrk(FnTok, ":\\")==NULL)
      {
      strcat(Fn, CfgFiles());
//      strcat(Fn, RootDirectory());
//      strcat(Fn,"DOCS\\");
      }
    strcat(Fn, FnTok);
    if (strstr(Fn, ".xec")==NULL)
      strcat(Fn, ".xec");
    FILE *f = fopen(Fn, "rt");

    #if dbgProcess
    dbgpln("RedirectInput:%s",Fn);
    #endif

    if (f == NULL)
      gs_pCmd->Print("Execute file '%s' not found\n",Fn);
    else
      {
      InputParms[Inputfiles] = new Strng_List();
      int t=2;
      pchar pt=gs_pCmd->Token(t);
      while (pt && (strlen(pt)>0))
        {
        dbgpln("Token:%s",pt);
        InputParms[Inputfiles]->Append(pt);
        t++;
        pt=gs_pCmd->Token(t);
        }
      Inputfile[Inputfiles++] = f;
      }
    }
  }

//---------------------------------------------------------------------------

STATIC_FN far void BumpRedirectedInput(/*CommandWnd *Cmd*/)
  {
  if (Inputfiles>0)
    if (feof(Inputfile[Inputfiles-1]))
      {
      Inputfiles--;
      fclose(Inputfile[Inputfiles]);
      delete InputParms[Inputfiles];
      }
    else
      {
      int PMRet = gs_pCmd->PostMessage(WM_COMMAND, IDM_BUMPINPUT, 0);
      // dbgpln("Post:CM_BUMPINPUT %i",PMRet);
      }
  }

//---------------------------------------------------------------------------

STATIC_FN far void GetRedirectedInput(/*CommandWnd *Cmd*/)
  {
  if (Inputfiles>0)
    {
    char line[256];
    if (fgets(line, sizeof(line)-1,Inputfile[Inputfiles-1]))
      {
      pchar p = strtok(line, "\n");

      #if dbgProcess
      dbgpln("RedirectedInput:%s",line);
      #endif

      if (p)
        {
        pStrng Parm= InputParms[Inputfiles-1]->First();
        for (int Pn=1; Pn <= 9; Pn++)
          {
          char Ps[20];
          sprintf(Ps, "%%%i", Pn);

          pchar ss;
          while (ss=strstr(p, Ps))
            if (Parm)
              {
              memmove(ss+Parm->Length(), ss+strlen(Ps), strlen(ss+strlen(Ps))+1);
              memmove(ss, Parm->Str(), Parm->Length());
              }
            else
              memmove(ss, ss+strlen(Ps), strlen(ss+strlen(Ps))+1);
          if (Parm)
            Parm= /*(InputParms[Inputfiles-1]*/Parm->Next();
          }
        }

      gs_pCmd->ProcessAStr(p);
      gs_pCmd->ProcessAChar(False, '\r', TRUE);
      }
    else
      {
      Inputfiles--;
      fclose(Inputfile[Inputfiles]);
      }
    }
  }

//---------------------------------------------------------------------------

STATIC_FN far void DoRedirectOutput(/*CommandWnd *Cmd*/)
  {
  gs_pCmd->RedirectOutput(gs_pCmd->Token(1), (strcmp(gs_pCmd->Token(0), ">>")==0));
  }

//---------------------------------------------------------------------------

void CommandWnd::RedirectOutput(pchar Fn, char Append)
  {
  if (OutFile)
    fclose(OutFile);
  OutFile = NULL;
  if (Fn)
    {
    if (Append)
      OutFile = fopen(Fn , "a+t");
    if (!OutFile)
      OutFile = fopen(Fn , "wt");
    if (OutFile == NULL)
      Print("File '%s' Not Opened\n",Fn);
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::Scroll(flag DoRepaint)
  {
  flag LinesRmd=0;
  pStrng s = new Strng("");
  MakeCmdLine(*s, CL);
  Lines.Append(s);
  while (Lines.Length() > 200)
    {
    pStrng pS=Lines.First();
    Lines.Remove(pS);
    delete pS;
    LinesRmd=1;
    }

  while (Lines.First() && !StrIsCmdLn(Lines.First()->Str()))
    {
    pStrng pS=Lines.First();
    Lines.Remove(pS);
    delete pS;
    LinesRmd=1;
    }

  ClearCmdLn();
  CL.Id = 0;

  if (DoRepaint || LinesRmd)
    FixScrollSizes(TRUE, 0);//if (!ResetScroller(0)) Repaint(TRUE);
  DisplayCmdLn(NULL);
  }

//---------------------------------------------------------------------------

void CommandWnd::AddTxt(pchar t)
  {
  }

//---------------------------------------------------------------------------

void CommandWnd::FindAndReplaceCmdLn(pchar F)
  {
  flag Done = 0;
  int Pass = 0, Line;
  StrngToken T;
  char ReqdMore[512];
  if (strlen(FndRep.Text) == 0)
    {
    strnset(FndRep.Text, 0, sizeof(FndRep.Text));
    strncpy(FndRep.Text, CL.Text, sizeof(FndRep.Text)-1);
    }
  do
    {
    Pass++;
    if (Pass==2)
      FndRep.Line=0;
    Line=0;
    pStrng S = Lines.Last();
    while (S && !Done)
      {
      int CP = S->CurrentPosn();
      S->TokensFrom(0);
      S->Token(T);
      int l = strlen(T);
      if (T[l-1] == '>')
        {
        S->Token(T);
        if (strnicmp(T, FndRep.Text, strlen(FndRep.Text))==0)
          {
          if (FndRep.Line == Line)
            {
            StrNull(CL.Extra);
            pchar p1,p2,p = &(S->Str()[l]);
            p1 = StrToken(p, " ");
            p2 = StrToken(p, " ");
            if (p1 && p2)
              {
              strcpy(CL.Text, p1);
              strcat(CL.Text, " ");
              strcat(CL.Text, p2);
              strcat(CL.Text, " ");
              strcpy(ReqdMore, "");
              p2 = StrToken(p, " ");
              while (p2)
                {
                strcat(ReqdMore, p2);
                strcat(ReqdMore, " ");
                p2 = StrToken(p, " ");
                }
              Done = 1;
              }
            }
          Line++;
          }
        }
      S->TokensFrom(CP);
      S = /*Lines.*/S->Prev();
      }
    }
  while (!Done && (Pass < 2));
  if (Done)
    {
    FndRep.Line++;
    StrNull(CL.MoreText);
//    strcat(CL.MoreText, ReqdMore);
    StrNull(CL.Extra);
    }
  else
    FndRep.Line=0;
  }

//---------------------------------------------------------------------------

void CommandWnd::SetFunctionKey(int SettingFnNo, pchar FnStr)
  {
  if (SettingFnNo >=1 && SettingFnNo<=MaxFnKeyNo)
    {
    FnStrng[SettingFnNo-1] = FnStr;
    Strng S;
    S.Set("F%i : %s",SettingFnNo,FnStr);
//x    SetInfoCaption(this, S.Str());
    }
  else
    MessageBox("Invalid Function Key Number", "ERROR", MB_ICONEXCLAMATION|MB_OK);
  };

//---------------------------------------------------------------------------

void CommandWnd::AutoSpace(flag On)
  {
  AutSpc = On;
  }

//---------------------------------------------------------------------------

STATIC_FN void dbgProcessAChar(CmdLnStruct &CL, flag IsVirtual, byte c, pchar s)
  {
  #if dbgProcess
  dbgp("%s V%3i",s,c);
  if (IsVirtual)
    dbgp(" V%3i",c);
  else if (c >= ' ')
    dbgp(" C'%c'",c);
  else
    dbgp(" C%3i",c);
  dbgp(" '%s|%s|%s'",CL.Text,CL.Extra,CL.MoreText);
  dbgpln("");
  #endif
  }

//---------------------------------------------------------------------------

void CommandWnd::ProcessAChar(flag IsVirtual, byte c, char DoRepaint)
  {
  int L = strlen(CL.Text);
  if (L>=CmdLineLen-2)
    {
    //Do something to prevent ERRORS!!!
    }
  flag BumpInput=0;
  if (KBDBlocked())
    {
    if (!IsVirtual && (c=='\x1b'))
      {
      BlockBTN();
      FreeKBD();
      }
    else
      {
      #if dbgProcess
      dbgpln("Unexpected Keyboard Input");
      #endif
//x      SetInfoCaption(this, "Unexpected Keyboard Input !");
//    MessageBox(this->m_hWnd, "Unexpected Keyboard Input", "COMMAND", MB_ICONEXCLAMATION|MB_OK);
      }
    }
  else
    {
    dbgProcessAChar(CL, IsVirtual, c, ">>>>  ");
    if (IsVirtual)
      {
      if (c >= VK_F1 && c <= VK_F16)
        if (FnStrng[c-VK_F1].Length()>0)
          {
          ProcessAStr(FnStrng[c-VK_F1].Str());
          c = 0;
          }

      char PrvCh;
      switch (c)
        {
        case 0:
          break;
        case VK_DOWN:
        case VK_UP:
          {
          int VMn, VMx;
          VScrl->GetScrollRange(&VMn, &VMx);
          VScrl->SetScrollPos(Range(VMn, VScrl->GetScrollPos() + (c==VK_UP ? -1 : 1), VMx));
          Invalidate();
          break;
          }
        case VK_NEXT:
        case VK_PRIOR:
          {
          CRect Cr;
          GetClientRect(Cr);
          int VMn, VMx;
          int PgL=ChWorkHeight(Cr, HScrlOn);
          VScrl->GetScrollRange(&VMn, &VMx);
          VScrl->SetScrollPos(Range(VMn, VScrl->GetScrollPos() + (c==VK_PRIOR ? -PgL : PgL), VMx));
          Invalidate();
          break;
          }
        default:
          FixScrollSizes(FALSE, TRUE);//ResetScroller(0);
          switch (c)
            {
            case VK_ESCAPE :
              while (strlen(CL.Text)>0)
                ProcessAChar(False, '\b', False);
              break;
            case VK_F8:
              FindAndReplaceCmdLn(CL.Text);
              ParseCmdLine();
              break;
            case VK_HOME:
              while (strlen(CL.Text)>0)
                ProcessAChar(TRUE, VK_LEFT, False);
            case VK_LEFT:
              flag HasExtra;
              do
                {
                HasExtra = 0;
                dbgProcessAChar(CL, IsVirtual, c, "LEFT  ");
                if (L > 0)
                  {
                  char Ch = CL.Text[L-1];
                  CL.Text[L-1] = 0;
                  L--;
                  StrNull(CL.Extra);
                  ParseCmdLine();
                  HasExtra = (strlen(CL.Extra) > 0);
                  if (!HasExtra)
                    {
                    memmove(&CL.MoreText[1], &CL.MoreText[0], strlen(CL.MoreText)+1);
                    CL.MoreText[0] = Ch;
                    }
                  }
                }
              while (c == VK_HOME && L > 0 || HasExtra);
              dbgProcessAChar(CL, IsVirtual, c, "LEFT3 ");
              break;
            case VK_END:
              break;
            case VK_RIGHT:
              PrvCh = (strlen(CL.Text) ? CL.Text[strlen(CL.Text)-1] : 0);
              do
                {
                if (strlen(CL.Extra) > 0)
                  {
                  strcat(CL.Text, CL.Extra);
                  StrCatC(CL.Text, ' ');
                  PrvCh = ' ';
                  StrNull(CL.Extra);
                  }
                else if (strlen(CL.MoreText) > 0)
                  {
                  if (PrvCh != ' ' || CL.MoreText[0] != ' ')
                    {
                    StrCatC(CL.Text, CL.MoreText[0]);
                    PrvCh = CL.MoreText[0];
                    }
                  memmove(&CL.MoreText[0], &CL.MoreText[1], strlen(CL.MoreText));
                  }
                ParseCmdLine();
                }
              while (c == VK_END && strlen(CL.MoreText) > 0);
              break;
            case VK_DELETE:
              if (strlen(CL.MoreText) > 0)
                {
                memmove(&CL.MoreText[0], &CL.MoreText[1], strlen(CL.MoreText)+1);
                ParseCmdLine();
                }
              break;
            case VK_INSERT:
              CL.Insert = !CL.Insert;
              break;
            default:
              break;
            }
          break;
        }
      }

    if (!IsVirtual)
      {
      ActivateVNT();
      
      FixScrollSizes(FALSE, TRUE);//ResetScroller(0);
      switch (c)
        {
        case '\x1b' :
          bEscBusy = 1;
          while (strlen(CL.Text)>0)
            ProcessAChar(False, '\b', False);
          bEscBusy = 0;
          break;
        case '\b':
          if(L > 0)
            {
            CL.Text[L-1] = NULL;
            UndoCmdLine();
            StrNull(CL.Extra);
            ParseCmdLine();
            }
          break;
        case ' ':
        case ',':
//xx        case ':':
        case ';':
          strcat(CL.Text, CL.Extra);
          StrCatC(CL.Text, c);
          StrNull(CL.Extra);
          ParseCmdLine();
          //ExecCmdLine(EX_TOKEN_RDY);
          break;
        case '\r':
          if (strlen(CL.Text) > 0)
            {
            strcat(CL.Text, CL.Extra);
            StrNull(CL.Extra);
            //strcat(CL.Text, " ");
            StrCatC(CL.Text, ' ');
            strcat(CL.Text, CL.MoreText);
            StrNull(CL.MoreText);
            if (OutFile)
              fprintf(OutFile, "%s\n", CL.Text);

            //TRACE("Cmd:%s", CL.Text);

            ParseCmdLine();
            //ExecCmdLine(EX_TOKEN_RDY);
            LnIsCommand(1);
            Scroll(1);

            ExecCmdLine(EX_EXEC);
            if (FindVNTDocument()==CurrentVNTDocument)//!bVNTInvalid)
              {
              for (int DoToken = 0; DoToken<=LastTokenNo; DoToken++)
                if (TL[DoToken].Data)
                  ExecCmdLine(EX_DOIT, DoToken);
              ExecCmdLine(EX_RESULTS);
              }

            FixScrollSizes(TRUE, TRUE);//ResetScroller(0);
            LnIsCommand(1);
            //InitForCmd();
            //CommitCmdLine();
            //CurrentCmd = NULL;
            }
          BumpInput=1;
          break;
        default:
          if (c > ' ' || c==0)
            {
            if (strlen(CL.Extra) > 0)
              {
              StrCatC(CL.Text, CL.Extra[0]);
              memmove(CL.Extra, &(CL.Extra[1]), strlen(CL.Extra));
              }
            else if (c>' ')
              StrCatC(CL.Text, IsUserText ? c : tolower(c));
            ParseCmdLine();
            if (AutSpc && (strlen(CL.Extra) > 0))
              ProcessAStr(" ");
            }
          break;
        }
      }
    dbgProcessAChar(CL, IsVirtual, c, "<<<<  ");
    DisplayCmdLn(NULL);
    AppendNewCmdLn();
    ActivateVNT();
    }

  if (BumpInput)
    BumpRedirectedInput();
  };

//---------------------------------------------------------------------------

int CommandWnd::ProcessAStr(pchar p)
  {
  #if dbgProcess
  dbgpln("ProcessAStr:%s",p?p:"");
  #endif
  iLastRetCode = 0;
  if (p && strlen(p)>0)
    {
    int i = strlen(p);
    while (*p != 0)
      {
      ProcessAChar(False, *p, ((--i)==0));
      p++;
      }
    }
  else
    ProcessAChar(False, 0, TRUE);
  return iLastRetCode;
  }

//---------------------------------------------------------------------------

STATIC_FN int ModifierLength(pchar p)
  {
  int l = strcspn(p, " ");
  return  (l > 0 ? l : strlen(p));
  }

//---------------------------------------------------------------------------

int VNTRqdLen(pchar s)
  {
  if (s)
    {
    int i=0,j=ModifierLength(s);
    while (i < j && (s[i] < 'a' || s[i] > 'z')) i++;
    return i;
    }
  else
    return -1;
  }

//---------------------------------------------------------------------------

STATIC_FN flag TokEq(pchar a, pchar b)
  {
  return (strnicmp(a,b, VNTRqdLen(b)) == 0);
  }

//---------------------------------------------------------------------------

STATIC_FN flag TokIsParm(pchar a)
  {
  return a[0] == '_';
  }

//---------------------------------------------------------------------------

STATIC_FN void SkipModifier(pchar &p)
  {
  int l = strcspn(p, " ");
  p+= (p[l] == ' ' ? l+strspn(&p[l], " ") : l);
  }

//---------------------------------------------------------------------------

STATIC_FN void LoadExtra(CmdLnStruct &CL, pchar ActToken, pchar RqdToken, pchar StrInText)
  {
  if (RqdToken && !TokIsParm(RqdToken))
    {
    int ActTokLen = strlen(ActToken);
    int RqdTokLen = Min(strlen(RqdToken), strcspn(RqdToken, " "));
    if ((ActTokLen < RqdTokLen) || strncmp(ActToken, RqdToken, ActTokLen))
      {
      int cpylen = Max(RqdTokLen-ActTokLen,0);
      #if dbgLoadEx
      dbgp("LoadExtra %2i %2i '%s' '%s'",ActTokLen, RqdTokLen, ActToken, RqdToken);
      #endif
      if (cpylen)
        memcpy(&CL.Extra[0], &RqdToken[ActTokLen], cpylen);
      CL.Extra[cpylen] = 0;
      memcpy(StrInText, RqdToken, Min(ActTokLen, RqdTokLen));
      #if dbgLoadEx
      dbgpln(" '%s' '%s'",ActToken, CL.Extra);
      #endif
      }
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::ChangeToken(const char * pTok, int iTokenNo)
  {
  if (iTokenNo<0)
    iTokenNo=LastTokenNo;

  NewCmdLn="";
  for (int i=0; i<=LastTokenNo; i++)
    {
    if (i==iTokenNo)
      NewCmdLn+=pTok;
    else
      NewCmdLn+=TL[i].Str;
    pchar pTerm=TL[i].StrInText+strlen(TL[i].Str); // points to char beyond end of token
    NewCmdLn+=*pTerm;
    }
  //dbgpln("--ChangeToken:%s",NewCmdLn());

  /*
  NewCmdLn.CL.Text;
  NewCmdLn.Trim();
  int i=NewCmdLn.Length()-1;
  while (i>=0 && NewCmdLn[i]!=' ') i--;
  NewCmdLn.SetLength(Max(0,i));
  NewCmdLn+=" ";
  NewCmdLn+=pTok;
  NewCmdLn+=" ";
  */
  }

//---------------------------------------------------------------------------

LONG CommandWnd::OnCmdWndStr(UINT wParam, LONG lParam)
  {
  pStrng p=(pStrng)lParam;

  #if dbgOnCmdWndStr
    dbgpln("CmdChar:%c",p->Str()[wParam]);
  #endif

  PostMessage(WM_CHAR, p->Str()[wParam], 0);
  wParam++;
  if ((int)wParam<p->Length())  
    PostMessage(IDM_CMDWND_STR, wParam, (LPARAM)(p));
  else
    {
    #if dbgOnCmdWndStr
      dbgpln("CmdChar:delete %s",p->Str());
    #endif
    delete p;
    }

  return 1;
  }

//---------------------------------------------------------------------------

void  CommandWnd::ExtendCmdLine(const char * pTok)
  {
  pStrng p=new Strng;
  if (strcmp(pTok, "\r")!=0 && strcmp(pTok, "\b")!=0)
    {
    *p+=" ";
    *p+=pTok;
    *p+=" ";
    }
  else
    *p+=pTok;
  if (p->Length()>0)
    {
    #if dbgOnCmdWndStr
      dbgpln("CmdChar:Post   %s",p->Str());
    #endif
    PostMessage(IDM_CMDWND_STR, 0, (LPARAM)(p));
    }
  else
    delete p;

  /*
  if (NewCmdLn.Length()==0)
    NewCmdLn=CL.Text;

  NewCmdLn.Trim();
  if (strcmp(pTok, "\r")!=0)
    {
    NewCmdLn+=" ";
    NewCmdLn+=pTok;
    NewCmdLn+=" ";
    }
  else
    NewCmdLn+=pTok;
  */
    
  //dbgpln("--ExtendCmdLn:%s",p->Str());
  }

//---------------------------------------------------------------------------
/*
void CommandWnd::PushModifier(int DefNo_)
  {
  StrNull(PushedModifier);
  if (BTNDefModVNT && DefNo_>0)
    {
    pchar p = BTNDefModVNT->Mods;
    while ((--DefNo_)>0)
      SkipModifier(p);
    int l = ModifierLength(p);
    char buff[256];
    strncpy(buff, p, l);
    buff[l]=0;
    //PushedModifier[l]=0;
    AppendNewToken(buff);
    }
  }
**/

//---------------------------------------------------------------------------

void CommandWnd::ExtendModifier(const char * p)
  {
  if (NewCmdLn.Length()==0)
    NewCmdLn=CL.Text;

  NewCmdLn.Trim();
  //NewCmdLn+=" ";
  NewCmdLn+=p;
  //dbgpln("--ExtendModifier:%s",NewCmdLn());
//  NewCmdLn+=" ";
/*
  if (p)
    strcat(ExtendedModifier, p);
*/
  /*
  int l=strlen(CL.Text)-1;
  if (l>=0)
    {
    int l1=l;
    while (l>=0 && CL.Text[l]==' ') l--;
    CL.Text[i+1]=0;
    strcat(CL.Text, p);
    for (int i=0; i<l; i++)
      strcat(CL.Text, " ");
    }
  */
  };

//---------------------------------------------------------------------------

flag CommandWnd::TestAddAnotherParm()
  {
  flag Added=0;
  if (CL.Text[strlen(CL.Text)-1]==',')
    {
    Added=1;
    int i=LastTokenNo;
    while (TL[i].NParms<=0)
      i--;
    ASSERT(i>0);
    TL[i].NParms++;
    }
  return Added;
  };

//---------------------------------------------------------------------------

void CommandWnd::AppendNewCmdLn()
  {
  if (NewCmdLn.Length()>0 && !bChgCmdLnBusy)
    {
    //dbgpln("->NewCmdLn%s",NewCmdLn());
    bChgCmdLnBusy=1;
    Strng ThisCmdLn(NewCmdLn);
    NewCmdLn="";
    bChgCmdLnBusy=0;

    pchar p=CL.Text;
    pchar q=ThisCmdLn();
    while (*p!=0 && *q!=0 && tolower(*p)==tolower(*q)) {  p++; q++; };
    
    while (*p!=0)                       // backspace
      ProcessAChar(False, '\b', False);
    while (*q!=0)                       // forward space
      {
      ProcessAChar(False, *q, False);
      q++;
      }
    
    //dbgpln("<-NewCmdLn");
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::ParseCmdLine()
  {
  #ifndef _RELEASE
  //some debug to try catch potential memory scribles, bugs, etc ... 
  if (strlen(CL.Text)>=CmdLineLen)
    DoBreak();
  if (strlen(CL.MoreText)>=CmdLineLen || strlen(CL.AsTokens)>=CmdLineLen || strlen(CL.Extra)>=CmdExtraLen)
    DoBreak();
  if (LastCmdBlk_Cur>=MaxVNTDepth || LastCmdBlk_Wrk>=MaxVNTDepth)
    DoBreak();
  if (LastTokenNo>=MaxToks)
    DoBreak();
  #endif
  strcpy(CL.AsTokens, CL.Text);

  #if dbgParseCmdLine
  dbgindent(2);
  dbgpln("ParseCmdLine %s", CL.AsTokens);
  #endif

//  pchar Terms = " :;,!\t\n\r";
  pchar Terms = " ;,!\t\n\r";
  pchar p = CL.AsTokens;
  pchar pe = &p[strlen(p)-1];
  flag LastTokenComplete = (strspn(pe, Terms)>0);
  #if dbgParseCmdLine
  pchar Mod=NULL;
  #endif
  //bVNTInvalid=0;
  CommandBlk *CBlk = NULL, *blk;
  VerbNounEntry *CCmd = NULL, *vnt;
  int CmdNo, NounNo=1, ModNo=1, LTNo=0;
  pchar RqdToken[3] = {NULL,NULL,NULL};
  CmdFn/*VNTFn*/ NounExec=NULL;
  flag OnlyParmNouns = 1;
  p+= strspn(p, " ");
  if (*p==';')
    {
    LastTokenNo = -1;
    return;
    }

  for (int TokNo=0; TokNo < MaxToks; TokNo++)
    {
    TL[TokNo].Str = StrToken(p, Terms, Terms);
    TL[TokNo].StrInText = CL.Text+(TL[TokNo].Str-CL.AsTokens);
    if (TL[TokNo].Str)
      LTNo = TokNo;
    }
  //NOTE: All is not well when LTNo==MaxToks-1, TL[] is full!

  #if xdbgParseCmdLine
  for (int it=0; it <= LTNo; it++)
    dbgp(" %s %i,%i,%i|", TL[it].Str, TL[it].Done,TL[it].ModNo,TL[it].NParms);
  dbgpln("");
  #endif

  if (TL[0].Str)
    for (int vPass = 0; (vPass < 2) && (!CCmd); vPass++)
      for (int v = LastCmdBlk_Cur; (v >= 0) && (!CCmd); v--)
        for (CmdNo = 0; (CmdBlk_Cur[v]->VNTable[CmdNo].Flags) && (!CCmd); CmdNo++)
          if (TokEq(TL[0].Str, CmdBlk_Cur[v]->VNTable[CmdNo].Verb))
            {
            vnt = &CmdBlk_Cur[v]->VNTable[CmdNo];
            RqdToken[0] = vnt->Verb;
            if (!vnt->Noun)
              {
              CBlk=CmdBlk_Cur[v];
              CCmd=vnt;
              }
            else
              {
              if (NounExec != vnt->Exec)
                {
                NounNo=1;
                NounExec = vnt->Exec;
                }
              else
                NounNo++;
              flag NounIsParm = TokIsParm(vnt->Noun);
              if (!NounIsParm && (vPass == 0))
                OnlyParmNouns = 0;
              if ((LTNo >= 1) && ((NounIsParm && vPass > 0) || TokEq(TL[1].Str, vnt->Noun)))
                {
                RqdToken[1] = vnt->Noun;
                CBlk=CmdBlk_Cur[v];
                CCmd=vnt;
                }
              }
            }

  StrNull(BTNDefModSpc);
  BTNDefModVNT=NULL;
  //DefModNo=-1;

  LastTokenNo = LTNo;
  for (int UndoToken = MaxToks-1; UndoToken>LastTokenNo; UndoToken--)
    {
    if (TL[UndoToken].CCmd)
      {
      ExecCmdLine(EX_UNDO, UndoToken);
      flag FoundCCmd=0;
      for (int i=0; i<UndoToken && !FoundCCmd; i++)
        FoundCCmd=(TL[i].CCmd!=NULL);
      if(!FoundCCmd)
        ExecCmdLine(EX_ALLUNDONE, UndoToken);      
      InitForCmd(UndoToken, UndoToken);
      }
    }

  if (CCmd && CCmd->Mods)
    {
    vnt = CCmd;
    blk = CBlk;
    BTNDefModVNT=vnt;

//    Cmd = NULL;
/**
    int i = DefModNo <0 ? vnt->DefModNo : DefModNo;
    //DefModNo=-1;
    if (i>0)
      {
      pchar p = vnt->Mods;
      while ((--i)>0)
        SkipModifier(p);
      int l = ModifierLength(p);
      strncpy(BTNDefModSpc, p, l);
      BTNDefModSpc[l]=0;
      }
**/

    if (LTNo >= 2 && TL[LTNo].Str)
      {
      byte ModToken = TokenIsModParm();
      if (ModToken)
        {
        CCmd = TL[ModToken].CCmd;
        CBlk = TL[ModToken].CBlk;
        NounNo = TL[ModToken].NounNo;
        ModNo = TL[ModToken].ModNo;
        }
      else
        {
        ModNo=1;
        pchar p = vnt->Mods;
        while ((strlen(p)>0) && !TokEq(TL[LTNo].Str, p))
          {
          ModNo++;
          SkipModifier(p);
          }
        if ((strlen(p) > 0) )// || ModHasIsParm)
          {
          RqdToken[2] = p;
          CCmd = vnt;
          CBlk = blk;
          #if dbgParseCmdLine
          Mod = p;
          #endif
          }
        else if (LastTokenComplete)
          {
          dbgpln("Delete Token:%s", TL[LTNo].Str);
          *TL[LTNo].StrInText = 0;
          CCmd=NULL;
          CBlk=NULL;
          }
        }
      }
    }

  LoadExtra(CL, TL[LTNo].Str, RqdToken[Min(LTNo,2)], TL[LTNo].StrInText);

  if (CCmd && (strlen(CL.Extra) == strspn(CL.Extra, " ")) && (!TL[LTNo].Done))
    {
    TL[LTNo].CCmd = CCmd;
    TL[LTNo].CBlk = CBlk;
    TL[LTNo].NounNo = NounNo;
    TL[LTNo].ModNo = ModNo;
    #if xdbgParseCmdLine
    dbgp("Tokens: %s %s", RqdToken[0],RqdToken[1],RqdToken[2]);
    dbgpln("   //DefMod '%s'", BTNDefModifier());
    #endif
    }

  if (LastTokenComplete && CCmd)
    if (/*(LTNo < 2) && */ ((TL[LTNo].Done & EX_TOKEN_RDY) == 0))
      {
      //TL[LTNo].Cmd = Cmd;
      //TL[LTNo].NounNo = NounNo;
      //TL[LTNo].ModNo = 0;
      //LastTokenNo = LTNo;
      ExecCmdLine(EX_TOKEN_RDY, LTNo);

      if (LTNo < 2 && !TokenIsModParm())
        if (LTNo==0 && OnlyParmNouns)
          SetParmInfo(AAny, 1);
        else if (LTNo==1)
          {
          flag OnlyParmMods = 1;
          pchar p = CCmd->Mods;
          while (p && (strlen(p)>0) && OnlyParmMods)
            {
            if (!TokIsParm(p))
              OnlyParmMods = 0;
            SkipModifier(p);
            }
          if (OnlyParmMods)
            SetParmInfo(AAny, MaxToks-2, 1);
          }

      pchar p=CL.Text+(strlen(CL.Text)-1);
      while (*(p-1)==' ')p--;
//xx      if ((TL[LTNo].Done & EX_MODIFIER_RDY) && (TL[LTNo].NParms || TL[LTNo].NDigs))
//xx        {
//xx        *p++=':';
//xx        *p=0;
//xx        }
//xx      else
     if (TL[LTNo].Done & EX_PARAMETER_RDY)
        {
        int i=LTNo;
        while (i>0 && (TL[i].Done & EX_PARAMETER_RDY)) i--;
        flag AnotherParm = ((i>=0) && ((LTNo-i)<TL[i].NParms));
        i=LTNo;
        while (i>0 && (TL[i].NParms==0)) i--;
        flag FreeForm = ((i>=0) && TL[i].FreeForm);
        if ((AnotherParm && !FreeForm) || TL[LTNo].NDigs)
          {
          *p++=',';
          *p=0;
          }
        }
      /**
      if (TL[LTNo].NDigs)
        {
        *p++='@';
        *p=0;
        }
      **/
      }
    else
      {
      // mhm
      if( TL[0].StrInText[strlen(TL[0].StrInText)-1] == ';' )
        ExecCmdLine(EX_TERM_SEMICOLON,-1);
      }

  #if dbgParseCmdLine
  dbgindent(-2);
  #endif
  };

//---------------------------------------------------------------------------

void CommandWnd::UndoCmdLine()
  {
  }

//---------------------------------------------------------------------------

STATIC_FN pchar ExecStr(int When)
  {
  switch (When)
    {
    case EX_BEGIN          : return "BEGIN  ";
    case EX_MODIFIER_RDY   : return "MODIF  ";
    case EX_PARAMETER_RDY  : return "PARAM  ";
    case EX_EXEC           : return "EXEC   ";
    case EX_DOIT           : return "DOIT   ";
    case EX_UNDO           : return "UNDO   ";
    case EX_MOUSE_LDOWN    : return "LDOWN  ";
    case EX_MOUSE_LMOVE    : return "LMOVE  ";
    case EX_MOUSE_LUP      : return "LUP    ";
    case EX_MOUSE_RDOWN    : return "RDOWN  ";
    case EX_MOUSE_RMOVE    : return "RMOVE  ";
    case EX_MOUSE_RUP      : return "RUP    ";
    case EX_MOUSE_LDCLK    : return "LBCLK  ";
    case EX_MOUSE_RDCLK    : return "LBCLK  ";
    case EX_RESULTS        : return "RESULTS";
    case EX_ALLUNDONE      : return "ALLUNDO";
    default                : return "?????";
    }
  }

//---------------------------------------------------------------------------

STATIC_FN pchar ExecState(int Done)
  {
  static far char Tmp[20];
  sprintf(Tmp, "{_________}");
  if (Done & EX_BEGIN)         Tmp[1]='B';
  if (Done & EX_MODIFIER_RDY)  Tmp[2]='R';
  if (Done & EX_PARAMETER_RDY) Tmp[3]='P';
  if (Done & EX_EXEC)          Tmp[4]='X';
  if (Done & EX_DOIT)          Tmp[5]='D';
  if (Done & EX_UNDO)          Tmp[6]='U';
  if (Done & EX_MOUSE_LDOWN)   Tmp[7]='d';
  if (Done & EX_MOUSE_LMOVE)   Tmp[8]='m';
  if (Done & EX_MOUSE_LUP)     Tmp[9]='u';
  if (Done & EX_MOUSE_RDOWN)   Tmp[7]='d';
  if (Done & EX_MOUSE_RMOVE)   Tmp[8]='m';
  if (Done & EX_MOUSE_RUP)     Tmp[9]='u';
  if (Done & EX_RESULTS)       Tmp[10]='r';
  if (Done & EX_ALLUNDONE)     Tmp[11]='A';
  if (Done & EX_MOUSE_LDCLK)   Tmp[12]='Z';
  if (Done & EX_MOUSE_RDCLK)   Tmp[13]='Y';
  return Tmp;
  }

//---------------------------------------------------------------------------

flag CommandWnd::ExecCmdLine(word When, int TokenNo)
  {
  if (TokenNo < 0)
    TokenNo = LastTokenNo;
  if (TokenNo < 0)
    return 0;

  if (When & EX_TOKEN_RDY)
     if (TokenIsModParm(TokenNo))
       When = EX_PARAMETER_RDY;
     else
       When = EX_MODIFIER_RDY;

  VerbNounEntry *CCmd = TL[TokenNo].CCmd;
  CommandBlk    *CBlk = TL[TokenNo].CBlk;
  byte NounNo = TL[TokenNo].NounNo;
  byte ModNo = TL[TokenNo].ModNo;

  if (CCmd)
    {


    flag IsDig = ((When & (EX_MOUSE_LMOVE|EX_MOUSE_LUP|EX_MOUSE_LDOWN|
                           EX_MOUSE_RMOVE|EX_MOUSE_RUP|EX_MOUSE_RDOWN|
                           EX_MOUSE_LDCLK|EX_MOUSE_RDCLK)) !=0);
// mhm change    flag IsDig = ((When & (/*EX_DONE|*/EX_MOUSE_LMOVE|EX_MOUSE_LUP|EX_MOUSE_RMOVE|EX_MOUSE_RUP)) !=0);
        if (!(TL[TokenNo].Done & When) || IsDig || (When & EX_TERM_SEMICOLON) )
// mhm changeif (!(TL[TokenNo].Done & When) || IsDig )






    //flag IsDig = ((When & (/*EX_DONE|*/EX_MOUSE_LMOVE|EX_MOUSE_LUP|EX_MOUSE_RMOVE|EX_MOUSE_RUP)) !=0);
    //if (!(TL[TokenNo].Done & When) || IsDig)
      {
      if (IsDig && (When & (EX_MOUSE_LUP|EX_MOUSE_RUP)))
        {

/**
        pchar p = CL.Text+(strlen(CL.Text)-1);
        while (*p==' ')p--;
        *(++p)='!';
        *(++p)=' ';
        *(++p)=0;
**/
/*
        int i =TokenNo;
        while (TL[i].Done & EX_PARAMETER_RDY) i--;
        ModNo=TL[i].ModNo;
*/
        }

      switch (TokenNo)
        {
        case 0 : ModNo = ModVerb; break;
        case 1 : ModNo = ModNoun; break;
        default:;
        }

      #if dbgExecCmdLine
      dbgnln();
      char MStr[10];
      switch (TokenNo)
        {
        case 0 : strcpy(MStr, "Vb"); break;
        case 1 : strcpy(MStr, "Nn"); break;
        default: sprintf(MStr,"%2i",ModNo);
        }

      dbgpln("==>> Exec %s %s[%2i.%i.%s] %s %s %s ",
              ExecState(TL[TokenNo].Done),ExecStr(When),TokenNo, NounNo, MStr,Token(0), Token(1),(TokenNo> 1?Token(TokenNo):NULL));
      dbgindent(2);
      //TRACE("==>> Exec %s %s[%2i.%i.%s] %s %s %s ",
      //        ExecState(TL[TokenNo].Done),ExecStr(When),TokenNo, NounNo, MStr,Token(0), Token(1),(TokenNo> 1?Token(TokenNo):NULL));
      #endif

      switch (When)
        {
        case EX_MODIFIER_RDY  : IsUserText=1; break;
        case EX_PARAMETER_RDY : IsUserText=0; break;
        }

      switch (When)
        {
        case EX_MODIFIER_RDY :
        case EX_PARAMETER_RDY :
          NDigs=0;
          break;
        case EX_MOUSE_LUP   :
//        case EX_MOUSE_RUP   :
          NDigs++;
          break;
        }

      int LTNo = LastTokenNo;
      LastTokenNo = TokenNo;

      //WDI.Open();
      CBlk->When=When;
      CBlk->Noun=NounNo;
      CBlk->Modifier=ModNo;
      // this, CCmd->BaseWnd);
      bExecBusy=1;
      (CBlk->*(CCmd->Exec))();
      bExecBusy=0;

//  (x.Blk->*x.pFn)(43);

      //CCmd->Exec(When, NounNo, ModNo, this, CCmd->BaseWnd);
      //Cmd->Exec(When, NounNo, ModNo, this, Cmd->BaseWnd);
      //Cmd->WDI.CLose();

      LastTokenNo = LTNo;
      TL[TokenNo].Done |= When;

      switch (When)
        {
        case EX_TERM_SEMICOLON :
        case EX_MODIFIER_RDY :
        case EX_PARAMETER_RDY :
          if (TL[TokenNo].NDigs != 0)
            {
            #if dbgButtonUpDn
            dbgpln("BlockKBD-FreeBTN");
            TRACE("BlockKBD-FreeBTN\n");
            #endif
            BlockKBD();
            FreeBTN();
            }
          break;
        case EX_MOUSE_LUP   :
//        case EX_MOUSE_RUP   :
          if (TL[TokenNo].NDigs > 0)
            TL[TokenNo].NDigs--;
          if (TL[TokenNo].NDigs == 0)
            {
            #if dbgButtonUpDn
            dbgpln("FreeKBD-BlockBTN");
            TRACE("FreeKBD-BlockBTN\n");
            #endif
            FreeKBD();
            BlockBTN();
            }
          break;
        default:;
        }

      #if dbgExecCmdLine
      dbgindent(-2);
      dbgpln("<<==");
      TRACE("<<==\n");
      #endif

      DisplayCmdLn(NULL);
      AppendNewCmdLn();

      /*
      if ((When & (EX_MODIFIER_RDY|EX_PARAMETER_RDY|EX_MOUSE_LUP))!=0)
        {
        if (strlen(ExtendedModifier)>0)
          {
          int l=strlen(CL.Text)-1;
          if (l>=0)
            {
            int l1=l;
            while (l>=0 && CL.Text[l]==' ') l--;
            CL.Text[l+1]=0;
            strcat(CL.Text, ExtendedModifier);
            for (int i=0; i<l1-l; i++)
              strcat(CL.Text, " ");
            }
          StrNull(ExtendedModifier);

          
          //StrngToken PM;
          //strcpy(PM, PushedModifier);
          //StrNull(PushedModifier);
          ////ProcessAStr(" ");
          ProcessAStr(NULL);
          }
        if (strlen(PushedModifier)>0)
          {
          StrngToken PM;
          strcpy(PM, PushedModifier);
          StrNull(PushedModifier);
          ProcessAStr(PM);
          //ProcessAStr(": ");
          }
        }
      */
      return 1;
      }
    else
      {
      return 0;
      }
    }
  else if ((When & EX_EXEC) && (Token(0)!=NULL))
    {
    if (strcmp(Token(0), "<") == 0)
      DoRedirectInput(Token(1));
    else if (strcmp(Token(0), ">") == 0)
      DoRedirectOutput();
    else if ((strlen(Token(0)) > 1) && (Token(0)[0] == '!'))
      Print(" !!!!!!!!!!!\n");
    else
      DoRedirectInput(Token(0));

//      {
//      #if dbgExecCmdLine
//      dbgpln("==== Unknown command ==== %i",TokenNo);
//      #endif
//      Print(" Unknown command\n");
//      }
    }
  return 0;
  };

//---------------------------------------------------------------------------

flag CommandWnd::ExecObjAcc(word When)
  {
  VerbNounEntry *CCmd = NULL;
  CommandBlk    *CBlk = NULL;
  for (int v = LastCmdBlk_Cur; (v >= 0) && (CCmd==NULL || CCmd->Exec==NULL); v--)
    {
    CBlk=CmdBlk_Cur[v];
    CCmd=CmdBlk_Cur[v]->ObjAccFn;
    }
  if (CCmd!=NULL && CCmd->Exec!=NULL)
    {
    //VerbNounEntry *CCmd = TL[TokenNo].CCmd;
    //CommandBlk    *CBlk = TL[TokenNo].CBlk;

    CBlk->When=When;
    CBlk->Noun=0xff;//-1;//NounNo;
    CBlk->Modifier=0xff;//-1;//ModNo;
    bExecBusy=1;
    (CBlk->*(CCmd->Exec))();
    bExecBusy=0;


    //(CBlk->*(ObjAccFn))();
    //*(ObjAccFn->Exec)();
    }

  return 0;
  };

//---------------------------------------------------------------------------

flag CommandWnd::ExecObjVw(word When)
  {
  VerbNounEntry *CCmd = NULL;
  CommandBlk    *CBlk = NULL;
  for (int v = LastCmdBlk_Cur; (v >= 0) && (CCmd==NULL || CCmd->Exec==NULL); v--)
    {
    CBlk=CmdBlk_Cur[v];
    CCmd=CmdBlk_Cur[v]->ObjVwFn;
    }
  if (CCmd!=NULL && CCmd->Exec!=NULL)
    {
    //VerbNounEntry *CCmd = TL[TokenNo].CCmd;
    //CommandBlk    *CBlk = TL[TokenNo].CBlk;

    CBlk->When=When;
    CBlk->Noun=0xff;//-1;//NounNo;
    CBlk->Modifier=0xff;//-1;//ModNo;
    bExecBusy=1;
    (CBlk->*(CCmd->Exec))();
    bExecBusy=0;


    //(CBlk->*(ObjAccFn))();
    //*(ObjAccFn->Exec)();
    }

  return 0;
  };

//---------------------------------------------------------------------------

int CommandWnd::TokenIsModParm(int TokenNo)
  {
  if (TokenNo < 0)
    TokenNo=LastTokenNo;
  int i=TokenNo-1;
  while ((i >= 0) && (TL[i].Done & EX_PARAMETER_RDY))
    i--;
  if (i < 0)
    return 0;
  int ParmNo = LastTokenNo-i;
  int t = (ParmNo <= TL[i].NParms ? i : 0);
  // dbgpln("TokenIsModParm() %i",t);
  return t;
  }

//---------------------------------------------------------------------------

int CommandWnd::ParmNumber()
  {
  /// dbgpln("CommandWnd::ParmNumber() %i",LastTokenNo-1-TokenIsModParm());
  return (LastTokenNo-1-TokenIsModParm());
  }

//---------------------------------------------------------------------------

pchar CommandWnd::BTNDefModifier()
  {
  BTNDefModSpc[0]=0;
  if (BTNDefModVNT)
    {
    int i = DefModNo <0 ? BTNDefModVNT->DefModNo : DefModNo;
    DefModNo=-1;
    if (i>0)
      {
      pchar p = BTNDefModVNT->Mods;
      while ((--i)>0)
        SkipModifier(p);
      int l = ModifierLength(p);
      strncpy(BTNDefModSpc, p, l);
      BTNDefModSpc[l]=0;
      }
    }
  return BTNDefModSpc;
  };

//---------------------------------------------------------------------------

flag CommandWnd::BTNDefModifierReqd()
  {
  flag b=0;
  if (BTNDefModVNT)
    {
    int i = DefModNo <0 ? BTNDefModVNT->DefModNo : DefModNo;
    b=(i>0);
    }
  if (b)
    {
//    b= ((LastTokenNo < 2) || (LastTokenNo >= 2) && (TL[LastTokenNo].Done & EX_TOKEN_RDY)));
    b= (TL[LastTokenNo].NDigs==0);
    }
  //if (b)
  //  dbgpln("BTNDefModifierReqd:%s  %i %s",BTNDefModSpc, LastTokenNo, ExecState(TL[LastTokenNo].Done));
  return b;
  };

//---------------------------------------------------------------------------

void CommandWnd::SetDigInfo(Cursor_Types CursNo, int NDigs)
  {
  #if dbgExecCmdLine
  dbgpln("SetDigInfo [%i] Nd%i Cu%i",LastTokenNo,NDigs,CursNo);
  TRACE("SetDigInfo [%i] Nd%i Cu%i\n",LastTokenNo,NDigs,CursNo);
  #endif
  if (NDigs >=0)
    {
    TL[LastTokenNo].NDigs = NDigs;
    TL[LastTokenNo].CursNo = CursNo;
    }
  else
    {
    int LT = LastTokenNo;
    while (LT >= 0 && TL[LT].CursNo==GC_NoCurs) LT--;
    if (LT >=0)
      TL[LT].CursNo = CursNo;
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::SetParmInfo(pchar ParmType, byte NParms, flag FreeForm)
  {
  #if dbgExecCmdLine
  dbgpln("SetParmInfo [%i] Np%i %s",LastTokenNo,NParms,ParmType);
  TRACE("SetParmInfo [%i] Np%i %s\n",LastTokenNo,NParms,ParmType);
  #endif
  TL[LastTokenNo].NParms = NParms;
  TL[LastTokenNo].FreeForm = FreeForm;
  }

//---------------------------------------------------------------------------

void CommandWnd::SetDefaultModifier(int DefNo_)
  {
  #if dbgExecCmdLine
  dbgpln("SetDefaultParm [%i]",DefNo_);
  TRACE("SetDefaultParm [%i]\n",DefNo_);
  #endif
  DefModNo=DefNo_;
  //TRACE1("Def Modifier = %i",DefNo_);

/*
  if (BTNDefModifierReqd())
    {
    ProcessAStr(BTNDefModifier());
    ProcessAStr(" ");
    }
**/
  //DefModNo=-1;

  //TL[LastTokenNo].NParms = NParms;
  //TL[LastTokenNo].FreeForm = FreeForm;
  }

//---------------------------------------------------------------------------

void  CommandWnd::SetDataBlk(void* b)
  {
  TL[LastTokenNo].Data = b;
  };

//---------------------------------------------------------------------------

void *CommandWnd::GetDataBlk(int which)
  {
  int i=LastTokenNo+1;
  while (which<=0)
    {
    i--;
    while (i>=0 && TL[i].Data == NULL) i--;
    which++;
    }
  return (i>=0 ? TL[i].Data : NULL);
  };

//---------------------------------------------------------------------------

flag  CommandWnd::OwnerOfDataBlk()
  {
  return (TL[LastTokenNo].Data != NULL);
  };

//---------------------------------------------------------------------------

//void CommandWnd::CommitCmdLine()
//  {
//  }

//---------------------------------------------------------------------------

void CommandWnd::OnLButtonDown(UINT nFlags, CPoint point)
  {
  CView::OnLButtonDown(nFlags, point);
//x  if (!MakeWindowTop(this))
//    {
//    }
  };

//---------------------------------------------------------------------------

void CommandWnd::OnRButtonDown(UINT nFlags, CPoint point)
  {
  CView::OnRButtonDown(nFlags, point);
//x  if (!MakeWindowTop(this))
//    {
//    }
  };

//---------------------------------------------------------------------------


void CommandWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CView::OnKeyDown(nChar, nRepCnt, nFlags);  // cnm

  if ((nFlags & 0x2000)==0) // Alt key not held down
    {
    char vk = nChar;
    if ((vk >= VK_F1 && vk <= VK_F16) ||
        (vk >= VK_PRIOR && vk <= VK_DOWN) ||
        (vk >= VK_INSERT && vk <= VK_DELETE)) // || (vk == VK_ESCAPE))
      {
      ProcessAChar(TRUE, vk, TRUE);
      }
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CView::OnChar(nChar, nRepCnt, nFlags);  // cnm
  //dbgpln("CommandWnd::OnChar %x", nChar);
  if ((nFlags & 0x2000)==0) // Alt key not held down
    {
    ProcessAChar(False, nChar, TRUE);
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CView::OnSysChar(nChar, nRepCnt, nFlags); // cnm
  if ((nFlags & 0x2000)==0) // Alt key not held down
    {
    char vk = nChar;
    ProcessAChar(TRUE, vk, TRUE);
    }
  }

//---------------------------------------------------------------------------

void CommandWnd::OnBumpInput()
  {
  GetRedirectedInput();
  }

//---------------------------------------------------------------------------

void CommandWnd::OnClose()
  {
  //GetRedirectedInput();
  }

//---------------------------------------------------------------------------

void CommandWnd::OnSize(UINT nType, int cx, int cy)
  {
  CView::OnSize(nType, cx, cy);
  FixScrollSizes(TRUE, TRUE);//ResetScroller(1);
  CWnd::OnSize(nType, cx, cy);
  }

//---------------------------------------------------------------------------

int CommandWnd::ChWorkHeight(CRect &Cr, flag HScrollOn)
  {
  return (Cr.bottom-Cr.top-2*CharOffset.cy-(HScrollOn ? GetSystemMetrics(SM_CYHSCROLL) : 0)) / CharSize.cy;
  }

//---------------------------------------------------------------------------

int CommandWnd::ChWorkWidth(CRect &Cr, flag VScrollOn)
  {
  return (Cr.right-Cr.left-2*CharOffset.cx-(VScrollOn ? GetSystemMetrics(SM_CXVSCROLL) : 0)) / CharSize.cx;
  }

//---------------------------------------------------------------------------

void CommandWnd::FixScrollSizes(flag fShowHideBars, flag bReset)
  {
  //if (CPg)
    {
    CRect Cr, r;
    GetClientRect(&Cr);
    if (HScrl==NULL)
      {
      r=Cr;
      r.left=r.right-10;
      VScrl= new CScrollBar();
      VScrl->Create(WS_CHILD|WS_VISIBLE|SBS_VERT, r, this, 0);
      r=Cr;
      r.top=r.bottom-10;
      HScrl= new CScrollBar();
      HScrl->Create(WS_CHILD|WS_VISIBLE|SBS_HORZ, r, this, 0);
      }

    int MaxLines=Lines.Length()+1;
    int MaxCols=40; // Temp
    int vsbs=GetSystemMetrics(SM_CXVSCROLL);
    int hsbs=GetSystemMetrics(SM_CYHSCROLL);
    int VPos=VScrl->GetScrollPos(), VPosIn=VPos;
    int HPos=HScrl->GetScrollPos(), HPosIn=HPos;
    if (bReset)
      {
      VPos=MaxLines;
      HPos=0;
      }
    //??
    //int sbs=ScrGB.GBC().ScrollBarSize;
    //int vsbs=ScrGB.VScrlWidth();//sbs>0 ? ScrGB.ColWdt()*sbs/10 : GetSystemMetrics(SM_CXVSCROLL);
    //int hsbs=ScrGB.HScrlHeight();//sbs>0 ? ScrGB.ColWdt()*sbs/10 : GetSystemMetrics(SM_CXHSCROLL);
    //CRect Vr=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);

    if (fShowHideBars)
      {
      flag OldVScrlOn=VScrlOn;
      flag OldHScrlOn=HScrlOn;
      //VScrlOn=(Vr.top>0) || (ScrGB.ChWorkHeight(False))<=CPg->Rows;
      //HScrlOn=(Vr.left>0) || (ScrGB.ChWorkWidth(False))<=CPg->MaxCols;
      flag HOn, VOn;
      HScrlOn=False;
      VScrlOn=False;
      do
        {
        HOn=HScrlOn;
        VOn=VScrlOn;
        VScrlOn=VScrlOn || (ChWorkHeight(Cr, HScrlOn)) < MaxLines;
        HScrlOn=HScrlOn || (ChWorkWidth(Cr, VScrlOn)) < MaxCols;
        }
      while (HOn!=HScrlOn || VOn!=VScrlOn);


      if (OldVScrlOn!=VScrlOn)
        VScrl->ShowScrollBar(VScrlOn);
      if (OldHScrlOn!=HScrlOn)
        HScrl->ShowScrollBar(HScrlOn);

      r=Cr; // Adjust Width Of Scroll Bar
      r.InflateRect(1,1);
      //++r.bottom;
      //++r.right;
      r.left=r.right-vsbs;
      if (HScrlOn)
        r.bottom=r.bottom-hsbs;
      VScrl->MoveWindow(&r, False);
      r=Cr;
      r.InflateRect(1,1);//--r.left;
      //++r.right;
      //++r.bottom;
      if (VScrlOn)
        r.right=r.right-hsbs;
      r.top=r.bottom-vsbs;
      HScrl->MoveWindow(&r, False);
      
      }
    
    int RowRange=Max(0, MaxLines-ChWorkHeight(Cr, HScrlOn));
    VPos=Range(0, VPos, RowRange); 
    VScrl->SetScrollRange(0, RowRange, False); 
    VScrl->SetScrollPos(VPos, True); 

    int ColRange=Max(0,MaxCols-ChWorkWidth(Cr, VScrlOn));
    HPos=Range(0, HPos, ColRange); 
    HScrl->SetScrollRange(0, ColRange, False); 
    HScrl->SetScrollPos(HPos, True); 
    
    if (VPos!=VPosIn || HPos!=HPosIn)// || bReset)
      {
      Invalidate();
      }
    }
  }
//---------------------------------------------------------------------------

void CommandWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  // TODO: Add your message handler code here and/or call default
  

  //if (CPg)
    {
    CRect Cr;
    GetClientRect(&Cr);
    int sp=VScrl->GetScrollPos();
    //ACE1("VScroll %i",sp);
    //ect Vr=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
    int PgL=ChWorkHeight(Cr, HScrlOn);
    /*
    int nMapMode;
    SIZE sizeTotal, sizePage, sizeLine;
    GetDeviceScrollSizes(nMapMode, sizeTotal, sizePage, sizeLine);
    */
    switch (nSBCode)
      {
      case SB_BOTTOM        : sp=0x7fffffff; break;
      case SB_LINEDOWN      : sp+=1; break;
      case SB_LINEUP        : sp-=1; break;
      case SB_PAGEDOWN      : sp+=PgL; break;
      case SB_PAGEUP        : sp-=PgL; break;
      case SB_THUMBPOSITION :  // Scroll to the absolute position. The current position is provided in nPos.
      case SB_THUMBTRACK    : sp=(nPos-sp); break; // Drag scroll box to specified position. The current position is provided in nPos.
      case SB_TOP           : sp=0; break;
      case SB_ENDSCROLL     : break;
      }

    int Mn,Mx;
    VScrl->GetScrollRange(&Mn, &Mx);
    VScrl->SetScrollPos(Range(Mn, sp, Mx));
    //xScrollSizes(False);
    Invalidate();
    //PositionCaret(TRUE);//nSBCode==SB_ENDSCROLL || nSBCode==SB_THUMBTRACK || nSBCode==SB_THUMBPOSITION);

    /**/
  //  CPoint XScrollPt=GetScrollPosition();
  //  ScrollBy = XScrollPt-ScrollPt;
  //  PositionCaret();
    }
  //else
    //CView::OnVScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void CommandWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  // TODO: Add your message handler code here and/or call default
  
  //if (CPg)
    {
    CRect Cr, r;
    GetClientRect(&Cr);
    int sp=HScrl->GetScrollPos();
    //TRACE1("HScroll %i",sp);
    //CRect Vr=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
    int PgL=ChWorkWidth(Cr, VScrlOn);
    switch (nSBCode)
      {
      case SB_RIGHT         : sp=0x7fffffff; break;
      case SB_LINERIGHT     : sp+=1; break;
      case SB_LINELEFT      : sp-=1; break;
      case SB_PAGERIGHT     : sp+=PgL; break;
      case SB_PAGELEFT      : sp-=PgL; break;
      case SB_THUMBPOSITION :  // Scroll to the absolute position. The current position is provided in nPos.
      case SB_THUMBTRACK    : sp=(nPos-sp); break;
      case SB_LEFT          : sp=0; break;
      case SB_ENDSCROLL     : break;
      }
    
    int Mn,Mx;
    HScrl->GetScrollRange(&Mn, &Mx);
    HScrl->SetScrollPos(Range(Mn, sp, Mx));
    Invalidate();
    //PositionCaret(TRUE);//nSBCode==SB_ENDSCROLL || nSBCode==SB_THUMBTRACK || nSBCode==SB_THUMBPOSITION);
//    if(nSBCode!=SB_ENDSCROLL)
//      FixScrollSizes(False);
    //PositionCaret();
    }
  //else
    //CView::OnHScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void CommandWnd::OnSetFocus(CWnd * pOldWnd)
  {
  CreateSolidCaret(CharSize.cx, 2);
  POINT Pt;
  Pt.x=0 * CharSize.cx;
  Pt.y=0 * CharSize.cy + CharAscent;
  SetCaretPos(Pt);
  ShowCaret();
  CWnd::OnSetFocus(pOldWnd);
  }

//---------------------------------------------------------------------------

void CommandWnd::OnKillFocus(CWnd * pNewWnd)
  {
  HideCaret();
  DestroyCaret();
  CWnd::OnKillFocus(pNewWnd);
  }

//---------------------------------------------------------------------------

HPEN CommandWnd::DigPen = NULL;

//---------------------------------------------------------------------------

void CommandWnd::dParm(double &d)
  {
  if (strcmp(LastToken(), ".") != 0)
    d = atof(LastToken());
  }

//---------------------------------------------------------------------------

void CommandWnd::fParm(float &f)
  {
  if (strcmp(LastToken(), ".") != 0)
    f = (float)atof(LastToken());
  }

//---------------------------------------------------------------------------

//void CommandWnd::rParm(REAL &d)
//  {
//  if (strcmp(LastToken(), ".") != 0)
//    d = atof(LastToken());
//  }

//---------------------------------------------------------------------------

void CommandWnd::iParm(int &i)
  {
  if (strcmp(LastToken(), ".") != 0)
    i = atoi(LastToken());
  }

//---------------------------------------------------------------------------

flag CommandWnd::FindCmdID(int nID)
  {
  if (!bEnabled)
    return false;
  if (nID==0)
    return 0;
  ActivateVNT();
  for (int v = LastCmdBlk_Cur; (v >= 0); v--)
    for (int CmdNo = 0; CmdNo < CmdBlk_Cur[v]->CSLen; CmdNo++)
      if (CmdBlk_Cur[v]->CSTable[CmdNo].iID==nID)
        return 1;
  return 0;
  }

//---------------------------------------------------------------------------

flag CommandWnd::UpdateCmdID(int nID, CCmdUI* pCmdUI)
  {
  if (bEnabled)
    for (int v = LastCmdBlk_Cur; (v >= 0); v--)
      for (int CmdNo = 0; CmdNo < CmdBlk_Cur[v]->CSLen; CmdNo++)
        if (CmdBlk_Cur[v]->CSTable[CmdNo].iID==nID)
          {
          return CmdBlk_Cur[v]->UpdateCmdStr(nID, pCmdUI);
          }
  return 0;
  }

//---------------------------------------------------------------------------

flag CommandWnd::ExecCmdID(int nID)
  {
  if (bEnabled)
    for (int v = LastCmdBlk_Cur; (v >= 0); v--)
      for (int CmdNo = 0; CmdNo < CmdBlk_Cur[v]->CSLen; CmdNo++)
        if (CmdBlk_Cur[v]->CSTable[CmdNo].iID==nID)
          {
          pCmdIDStrEntry pEntry=&CmdBlk_Cur[v]->CSTable[CmdNo];
          Strng S(pEntry->pStr);
          if (CmdBlk_Cur[v]->ProcessCmdStr(nID, S))
            {
            if (CmdBlk_Cur[v]->CSTable[CmdNo].bOnNewline && strlen(CL.Text)>0)
              ProcessAStr("\r");
            //bStrIsCmdFromMenu=1;
            ProcessAStr(S());
            //bStrIsCmdFromMenu=0;
            }
          }
  return 1;
  }

//---------------------------------------------------------------------------

void CommandWnd::SetEnable(flag On)
  {
  bEnabled=On;
  };

//---------------------------------------------------------------------------
/*
flag CommandWnd::CmdFromMenu()
  {
  return bCmdFromMenu;
  }
*/
//---------------------------------------------------------------------------
/*
void CommandWnd::VNTInvalid() 
  { 
  bVNTInvalid=1;
  };
*/
//---------------------------------------------------------------------------

void CommandWnd::VNTChanged() 
  { 
  bVNTChanged=1;
  bEnabled=true;
  };

//---------------------------------------------------------------------------
/*
void CommandWnd::SetDocForCmds(CDocument* pDoc)
  {
  //VERIFY(!bExecBusy);
  pDocForCmds=pDoc; 
  bVNTChanged=1;
  };
*/
//===========================================================================

CMDIChildWnd* gs_pCmdFrame=NULL;
CommandWnd*   gs_pCmd=NULL;

//===========================================================================
