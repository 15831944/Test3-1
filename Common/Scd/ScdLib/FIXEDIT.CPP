//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "resource.h"

#define __FIXEDIT_CPP

#include "sc_defs.h"
#include "gpfuncs.h"
#include "datacnvs.h"
#include "datatype.h"
#include "tagobj.h"
#include "fixedit.h"
#include "scd_wm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgStructure 0

//===========================================================================

const byte PC_NoRedraw          = 0x0;
const byte PC_Redraw            = 0x01;
const byte PC_NoClearSlct       = 0x0;
const byte PC_ClearSlct         = 0x02;
const byte PC_NoSetSlct         = 0x0;
const byte PC_SetSlct           = 0x04;
const byte PC_ClearIfNotSlcted  = 0x04;

struct MarksInfo 
  { 
  bool m_DoIt;
  bool m_Large; 
  bool m_Top; 
  bool m_Error; 
  bool m_Fill; 
  int m_nPts;
  POINT    m_Pt[5];
  COLORREF m_Col; 
  };
MarksInfo Marks[] = 
  {
    {true, false, true,  false, false,  3, {{0,2}, {4,4}, {4,0},    {},   {}}, RGB(0x7f ,0xff ,0x00)  },  // GetXRef
    {true, true,  true,  false, true,   3, {{0,2}, {4,4}, {4,0},    {},   {}}, RGB(0x7f ,0xff ,0x00)  },  // GetXRefCnt
    {true, false, false, false, false,  3, {{0,4}, {0,8}, {4,6},    {},   {}}, RGB(0x00 ,0x00 ,0xff)  },  // SetXRef
    {true, true,  false, false, true,   3, {{0,4}, {0,8}, {4,6},    {},   {}}, RGB(0x00 ,0x00 ,0xff)  },  // SetXRefCnt
    {false,true,  false, true,  false,  3, {{4,4}, {4,8}, {8,6},    {},   {}}, RGB(0xff ,0x00 ,0x00)  },  // SetXRefCntMulti
    {true, true,  false, true,  true,   3, {{4,4}, {4,8}, {8,6},    {},   {}}, RGB(0xff ,0x00 ,0x00)  },  // SetXRefCntMulti
    {true, true,  false, false, true,   3, {{2,4}, {2,8}, {6,6},    {},   {}}, RGB(0xff ,0x00 ,0xff)  },  // Reset
    {true, true,  true,  false, true,   0, {{0,2}, {4,4}, {4,0},    {},   {}}, RGB(0x00 ,0xff ,0x80)  },  // InActive
  };

//===========================================================================
//
//
//
//===========================================================================

FxdEdtFld::FxdEdtFld(pFxdEdtView pView_, FxdEdtRow* pRow_, int BlkId_, int FieldId_, int Col1_, int ColN_, pchar Str_, UCHAR Just_, BOOL Editable_, byte FieldType_)
  {
  pView=pView_;
  pRow=pRow_;
  FieldId=FieldId_;
  FieldType=FieldType_;
  Col1=Col1_;
  ColN=ColN_;
  BlkId=BlkId_;
  Len=ColN-Col1;
  Just=Just_;
  BigStrLength=0;

  Tag=NULL;
  fEditable=Editable_;
  fEditing=0;
  AcceptChanges=0;
  Changed=0;
  IsButton=0;
  IsIndexTab=0;
  ButtonDown=0;
  IsFxdStrings=0;
  HasNANStrings=0;
  HasDDLst=0;
  HasEdtBox=0;
  iCtrlIndex=-1;
  m_iCtrlType=0;
  fItalic=0;
  fInError=0;
  fIsResult=0;
  fDisabled=0;
  fSelected=0;
  fSelectable=1;
  fProtected=0;
  fUnderLine=0;
  m_MarksChgd=0;
  m_Marks=0;
  fFixedPos=pView->fFixedPos;

  Str.SetLength(Len);
  if (Str_)
    {
    Str=Str_;
    FixAfterLoad(Str);
    }
  
  if (fFixedPos)
    pView->CPg->FixedRows=Max((int)pView->CPg->FixedRows, pRow->RowNo+1);
  iPg=0;
  }

//---------------------------------------------------------------------------

FxdEdtFld::~FxdEdtFld()
  {
  //delete[] Str;
  delete[] Tag;
  };

// --------------------------------------------------------------------------

byte FxdEdtFld::CtrlType()
  {
  if (m_iCtrlType==FXWC_DDLSTOPT)
    {
    if (HasDDLst)
      return FXWC_DDLST;
    }
  return m_iCtrlType;
  };   

//---------------------------------------------------------------------------

void FxdEdtFld::SetTag(char * TagStr, char * CnvTxt)
  {
  delete[] Tag;
  if (TagStr)
    {
    int DoCnv=(CnvTxt && strlen(CnvTxt)>0);
    int L=strlen(TagStr);
    if (DoCnv)
      L+=2+strlen(CnvTxt)+1;
    Tag=new char[L+1];
    strcpy(Tag, TagStr);
    if (DoCnv)
      {
      strcat(Tag, " (");
      strcat(Tag, CnvTxt);
      strcat(Tag, ")");
      }
    }
  else
    Tag=NULL;
  };

//---------------------------------------------------------------------------

void FxdEdtFld::DrawMark(rGDIBlk GB, int iMark, bool InActiveColour)
  {
  MarksInfo &I=Marks[iMark];
  if (I.m_DoIt)
    {
    int x=Xs(GB);
    int y=Ys(GB);
    //GDIBlkCfg &gbc = GB.GBC();
    CDC &dc = GB.DC();
    int EdgeW=GB.EdgeWdt();
    int RowH=GB.RowHgt();
    int ColW=GB.ColWdt();
    int H=RowH-EdgeW;
    int xs=x+EdgeW;
    int ys=y+EdgeW;
    POINT Mrk[5];
    for (int iPt=0; iPt<I.m_nPts; iPt++)
      {
      Mrk[iPt].x=xs+(ColW*I.m_Pt[iPt].x)/5;
      Mrk[iPt].y=ys+(H*I.m_Pt[iPt].y)/8;
      }
    Mrk[iPt].x=xs+(ColW*I.m_Pt[0].x)/5;
    Mrk[iPt].y=ys+(H*I.m_Pt[0].y)/8;

    COLORREF Colour=InActiveColour ? RGB(0x00 ,0xff ,0xff) : I.m_Col;
    CBrush MrkBrush(Colour);
    CPen MrkPen(PS_SOLID, 0, Colour);
    CPen *OldPen=dc.SelectObject(&MrkPen);
    if (I.m_Fill)
      {
      CBrush *OldBrush=dc.SelectObject(&MrkBrush);
      dc.Polygon(Mrk, iPt+1);
      dc.SelectObject(OldBrush);
      }
    else
      dc.Polyline(Mrk, iPt+1);
    dc.SelectObject(OldPen);
    }
  }

//---------------------------------------------------------------------------

void FxdEdtFld::Write(rGDIBlk GB, flag WithEdge, flag FullDraw)
  {
  if ((FieldType&(SEPAR_FIELDS|DESC_FIELDS)) && (Str.XStrSpn(" ")==Str.Length()))
    return;
  if ( (IsButton && !GB.IsPrinting()) || 
       (IsButton && GB.IsPrinting() && m_iCtrlType!=FXWC_CHECK && m_iCtrlType!=FXWC_CHECKBTN) )
    return;

  int x=Xs(GB);
  int y=Ys(GB);
  GDIBlkCfg &gbc = GB.GBC();
  CDC &dc = GB.DC();
  CDCResChk ResChk(dc);

  int EdgeW=GB.EdgeWdt();
  int RowH=GB.RowHgt();
  int ColW=GB.ColWdt();

  int xr=x+ColW*Len;
  int yb=IsButton ? y+RowH-EdgeW : y+RowH;

  int TxtInv=0;
  //COLORREF BackColor;
  CBrush *pBackBrush;
  if (IsButton && ButtonDown || GB.IsPrinting())
    pBackBrush=GB.pBrushTxtFore;
  else if (fSelected)
    {
    pBackBrush=GB.pBrushTxtBackSelect;
    TxtInv=1;
    }
  else if (fInError)
    pBackBrush=GB.pBrushTxtBackError;
  else if (fEditable)
    pBackBrush=GB.pBrushTxtFore;
  else
    pBackBrush=GB.pBrushTxtBack;
  //CBrush BackBrush(BackColor);


  if (FullDraw)
    {
    CRect Bck  (x+EdgeW, y+EdgeW, xr+EdgeW, yb+EdgeW);
    dc.FillRect(&Bck, pBackBrush);

    if ((FieldType&(DESC_FIELDS|SEPAR_FIELDS))==0 && WithEdge)
      {
      CRect top   (x,       y,       xr,       y+EdgeW);
      CRect bottom(x+EdgeW, yb,      xr+EdgeW, yb+EdgeW);
      CRect left  (x,       y+EdgeW, x+EdgeW,  yb+EdgeW);
      CRect right (xr,      y,       xr+EdgeW, yb);

      dc.FillRect(&top   , IsButton ? GB.pBrushTxtEdgeLight : GB.pBrushTxtEdgeDark);
      dc.FillRect(&left  , IsButton ? GB.pBrushTxtEdgeLight : GB.pBrushTxtEdgeDark);
      dc.FillRect(&right , IsButton || GB.IsPrinting() ? GB.pBrushTxtEdgeDark : GB.pBrushTxtEdgeLight);
      dc.FillRect(&bottom, IsButton || GB.IsPrinting() ? GB.pBrushTxtEdgeDark : GB.pBrushTxtEdgeLight);
      }
    }
  else
    {
    CRect TxtBck  (x+EdgeW, y+EdgeW, xr, yb);
    dc.FillRect(&TxtBck, pBackBrush);
    }

  if (fUnderLine)
    {
    CPen LnPen(PS_SOLID, 0, RGB(0xB0,0xB0,0xB0));
    CPen *OldPen=dc.SelectObject(&LnPen);
    POINT L={x, yb};
    POINT R={xr, yb};
    dc.MoveTo(L);
    dc.LineTo(R);
    dc.SelectObject(OldPen);
    };

  if (m_Marks)
    {
    UCHAR mM=m_Marks;
    for (int iM=0; mM; iM++, mM=mM>>1)
      if (mM&1)
        DrawMark(GB, iM, (m_Marks & FFM_HasInactive)!=0);
    }

  CRect Txt  (x+EdgeW, y+EdgeW, xr+1, yb);
  CFont * OldFont=NULL;
  if (fItalic)
    OldFont=dc.SelectObject(GB.pItlFont[0]); // Italic
  COLORREF TC;
  if (fInError)
    TC=gbc.crTextError[TxtInv];
  else if (Changed || fEditing)
    TC=gbc.crTextChg[TxtInv];
  else if (fIsResult &&
           !TaggedObject::GlblResultsValid() &&
           !TaggedObject::GlblResultsAlwaysValid())
    TC=gbc.crTextInvalid[TxtInv];
  else
    TC=gbc.crText[TxtInv];
  int OldTextColor=dc.SetTextColor(TC);

  int OldBkMode=dc.SetBkMode(TRANSPARENT);
  const int JustFlags[] = {DT_LEFT, DT_CENTER, DT_RIGHT, 0};
  int JustFlag=JustFlags[Just];
  dc.DrawText(Str(), Str.Length(), Txt, DT_END_ELLIPSIS|DT_SINGLELINE|DT_BOTTOM|DT_NOPREFIX|JustFlag);
  dc.SetBkMode(OldBkMode);
  dc.SetTextColor(OldTextColor);
  if (fItalic)
    dc.SelectObject(OldFont);
  }

//---------------------------------------------------------------------------

void FxdEdtFld::Invalidate(rGDIBlk GB)
  {
  int Row=pRow->RowNo;
  CRect RC(Xs(GB), Ys(GB), Xe(GB)+1, Ye(GB)+1);
  Wnd(pView).InvalidateRect(RC, false);
  }

//---------------------------------------------------------------------------

void FxdEdtFld::FixAfterLoad(Strng &Str)
  {
  if (Just<3)
    Str.Trim();
  if (1)
    {
    Str.RPad(1);
    }
  else
    {
    int m, l;
    l=Str.Length();
    m=Len-l;
    if (m>0)
      switch (Just)
        {
        case 0: // left
          Str.RPad(Len);
          break;
        case 1: // centre
          Str.LRPad(Len);
          break;
        case 2: // right
          Str.LPad(Len);
          break;
        case 3: ;// none
          Str.RPad(Len);
        }
    }
  switch (m_iCtrlType)
    {
    case FXWC_CHECK:
    case FXWC_CHECKBTN:
      {
      CButton* pC=(CButton*)pView->pCtrl[iCtrlIndex].pCtrl;
      if (pC)
        pC->SetCheck(Str.SafeAtoL() ? 1 :0);
      break;
      }
    }
  }

//---------------------------------------------------------------------------

void FxdEdtFld::FixBeforeParse()
  {
  Str.Trim();
  Str.RPad(1); // At least one space
  }

//===========================================================================

FxdEdtRow::FxdEdtRow(pFxdEdtView pView_)
  {
  Cols=0;
  NFields=0;
  pView=pView_;
  fFixedPos=pView->fFixedPos;
  RowNo=0;
  };

//---------------------------------------------------------------------------

FxdEdtRow::~FxdEdtRow()
  {
  for (int f=0; f<NFields; f++)
    delete Field[f];
  };

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtRow::SetFld(pFxdEdtView pView_, int RowNo_, int BlkId_, pchar InitStr, int FieldId, int Width, UCHAR Just, BOOL Editable, byte FieldType)
  {
  RDBASSERT(NFields < MaxFixedFields, "Too many fixed fields");
  pView=pView_;
  RowNo=RowNo_;
  if (InitStr)
    {
    }

  Field[NFields]=new FxdEdtFld(pView, this, BlkId_, FieldId, Cols, Cols+Width, InitStr, Just, Editable, FieldType);
  Cols+=Width;
  NFields++;
  return Field[NFields-1];
  }

//---------------------------------------------------------------------------

int FxdEdtRow::Ys(rGDIBlk GB) {return (int)(GB.LuMargin.cy+GB.LuBorder.cx+(RowNo-GB.ChScroll.cy)*GB.RowHgt());};
int FxdEdtRow::Ye(rGDIBlk GB) {return (int)(GB.LuMargin.cy+GB.LuBorder.cy+(RowNo+1-GB.ChScroll.cy)*GB.RowHgt()-1);};

//---------------------------------------------------------------------------

BOOL FxdEdtRow::Visible(rGDIBlk GB)
  {
//  int y=pView->Offset.y+pView->GB.RowHgt()*RowNo;
//  return (y>=pView->ScrollPt.y+pView->TabIndexSpcY-pView->GB.RowHgt() &&
//          y<=pView->ScrollPt.y+pView->TabIndexSpcY+(pView->ClientR.bottom-pView->ClientR.top));
  if (GB.IsPrinting())
    return true;

  RECT CR;
  pView->GetClientRect(&CR);
  return (Ye(GB) >=0 && Ys(GB) <= (CR.bottom-CR.top));
  }

//---------------------------------------------------------------------------

void FxdEdtRow::Write(rGDIBlk GB)
  {
  for (int f=0; f<NFields; f++)
    Field[f]->Write(GB, true, true);
  }

//---------------------------------------------------------------------------

void FxdEdtRow::Invalidate(rGDIBlk GB)
  {
  for (int f=0; f<NFields; f++)
    Field[f]->Invalidate(GB);
  }

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtRow::FldFromId(int IdReqd)
  {
  for (int f=0; f<NFields; f++)
    if (Field[f]->FieldId==IdReqd)
      return Field[f];
  return NULL;
  }

//===========================================================================

FxdEdtBlk::FxdEdtBlk()
  {
  Pg1=0;
  Row1=0;
  Len=0;
  Index=0;
  Ptr=NULL;
  };

//---------------------------------------------------------------------------

FxdEdtBlk::~FxdEdtBlk()
  {
  }

//===========================================================================

FxdEdtPage::FxdEdtPage(pFxdEdtView pView_, pchar Name_, int No_, bool LoadByCol)
  {
  strcpy(TabName, Name_);
  pView=pView_;
  bLoadByCol=LoadByCol;
  No=No_;
  Rows=0;
  MaxCols=10;
  for (int i=0; i < MaxFixedRows; i++)
    Row[i]=NULL;
  for (int b=0; b<MaxBlkIds; b++)
    {
    Blk[b].Row1=-1;
    Blk[b].Pg1=0;
    Blk[b].Row1=0;
    Blk[b].Len=0;
    Blk[b].Index=0;
    Blk[b].Ptr=NULL;
    }
  Blk[0].Row1=0;
  Blk[0].Len=0;
  Blks=1;
  CurBlkId=0;
  FixedRows=0;
  TabWide=0;
  TabPos=0;
  TabRow=0;
  };

//---------------------------------------------------------------------------

FxdEdtPage::~FxdEdtPage()
  {
  for (int i=0; i < MaxFixedRows; i++)
    if (Row[i])
      delete Row[i];
  }

//---------------------------------------------------------------------------

void FxdEdtPage::SetRows(int NRows_)
  {
  Rows=NRows_;
  for (int i=0; i< MaxFixedRows; i++)
    if ((Row[i]==NULL) && (i < Rows))
      Row[i]=new FxdEdtRow(pView);
    else if ((Row[i]!=NULL) && (i >= Rows))
      {
      delete Row[i];
      Row[i]=NULL;
      }
  }

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtPage::SetFld(byte FieldType, int RowNo, pchar InitStr, int FieldId, int Width, UCHAR Just, pchar Separator, BOOL Editable)
  {
#if dbgStructure
  dbgpln("SetFld:%3i %3i %4i '%s'", FieldType, RowNo, FieldId, InitStr?InitStr:"");
#endif
       //ASSERT(CurBlkId<MaxBlkIds);
  RowNo+=Blk[CurBlkId].Row1;
  if (RowNo>=Rows)
    SetRows(RowNo+1);
  ASSERT(RowNo>=0 && RowNo < Rows);
  //TRACE3("SetFld Row %3i Id%3i %s\n", RowNo, FieldId, InitStr);
  FxdEdtFld * Ret;
  Ret=Row[RowNo]->SetFld(pView, RowNo, CurBlkId, InitStr, FieldId, Width, Just, Editable, FieldType);
  MaxCols=Max(MaxCols, Row[RowNo]->Cols);
  if (Separator && (strlen(Separator)>0))
    Row[RowNo]->SetFld(pView, RowNo, CurBlkId, Separator, -1, strlen(Separator), 3, false, SEPAR_FIELDS);
  if (Ret)
    Ret->iPg=(byte)No;
  return Ret;
  }


//---------------------------------------------------------------------------

void FxdEdtPage::LoadStrings()
  {
#if dbgStructure
  dbgpln("FxdEdtPage::LoadStrings[%i]",No);
#endif
  if (bLoadByCol)
    {
    int GotOne=true;
    for (int f=0; GotOne; f++)
      {
      GotOne=false;
      for (int r=0; r<Rows; r++)
        if (f<Row[r]->NFields)
          {
          GotOne=true;
#if dbgStructure
          dbgpln("LSa %s", Row[r]->Field[f]->Str());
#endif    
          if (Row[r]->Field[f]->FieldId>=0)
            {
            FxdEdtFld * Fld=Row[r]->Field[f];
            CurBlkId=0;
            FxdEdtInfo EI(pView, this, Fld, r);
            //TmpFldBuff Buff(Fld->Str);
            pView->Load(EI, Fld->Str);//, Fld->Len);
            Fld->FixAfterLoad(Fld->Str);

            //switch (Fld->iCtrlType)
            //  {
            //  case FXWC_CHECK:
            //  case FXWC_CHECKBTN:
            //    {
            //    CButton* pC=(CButton*)pView->pCtrl[Fld->iCtrlIndex].pCtrl;
            //    pC->SetCheck(Fld->Str.SafeAtoL() ? 1 :0);
            //    break;
            //    }
            //  }
            }
          }
      }

    }
  else
    {
    for (int r=0; r<Rows; r++)
      for (int f=0; f<Row[r]->NFields; f++)
        {
#if dbgStructure
        dbgpln("LSb %s", Row[r]->Field[f]->Str());
#endif
        if (Row[r]->Field[f]->FieldId>=0)
          {
          FxdEdtFld * Fld=Row[r]->Field[f];
          CurBlkId=0;
          FxdEdtInfo EI(pView, this, Fld, r);
          //TmpFldBuff Buff(Fld->Str);
          pView->Load(EI, Fld->Str);//, Fld->Len);
          Fld->FixAfterLoad(Fld->Str);

          //switch (Fld->iCtrlType)
          //  {
          //  case FXWC_CHECK:
          //  case FXWC_CHECKBTN:
          //    {
          //    CButton* pC=(CButton*)pView->pCtrl[Fld->iCtrlIndex].pCtrl;
          //    pC->SetCheck(Fld->Str.SafeAtoL() ? 1 :0);
          //    break;
          //    }
          //  }
          }
        }
    }
  }

//---------------------------------------------------------------------------

void FxdEdtPage::ParseStrings()
  {
  for (int r=0; r<Rows; r++)
    for (int f=0; f<Row[r]->NFields; f++)
      if (Row[r]->Field[f]->FieldId>=0)
        {
        FxdEdtFld * Fld=Row[r]->Field[f];
        Fld->FixBeforeParse();
        CurBlkId=0;
        FxdEdtInfo EI(pView, this, Fld, r);
        pView->Parse(EI, Fld->Str);
        }
  }

//---------------------------------------------------------------------------

int FxdEdtPage::MoveRows(int RowShift)
  {
  //TRACE2("MoveRows   Pg:%i   Shift::%i\n", No, RowShift);
  if (RowShift==0)
    return 0;

  // Start at FixedRows
  // must not remove any rows
  int Cnt=0;
  for (int r1=FixedRows; r1<Rows; r1++, Cnt++)
    if (Row[r1]->NFields>0)
      break;
  int Shift=Range((int)-Cnt, RowShift, (int)(MaxFixedRows-Rows));

  int as=abs(Shift);
  if (Shift<0)
    {
    for (int r=FixedRows; r<Rows; r++)
      {
      if (r<as)
        delete Row[r];
      if (r<(Rows-as))
        Row[r]=Row[r+as];
      else
        Row[r]=NULL;
      }
    Rows+=Shift;
    }
  else
    {
    for (int r=Rows-1; r>=FixedRows; r--)
      Row[r+Shift]=Row[r];
    for (r=FixedRows; r<FixedRows+Shift; r++)
      Row[r]=new FxdEdtRow(pView);
    Rows+=Shift;
    }

  for (int r=FixedRows; r<Rows; r++)
    {
    Row[r]->RowNo=r;
    }

  for (int b=FixedRows; b<Blks; b++)
    {
    Blk[b].Row1+=Shift;
    }

  return Shift;
  }

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtPage::FldFromId(int RowNo, int FieldId)
  {
  FxdEdtFld * Fld=NULL;
  if (RowNo < 0)
    {
    for (int r=0;r<Rows;r++)
      {
      Fld=Row[r]->FldFromId(FieldId);
      if (Fld)
        return Fld;
      }
    }
  else
    return Row[RowNo]->FldFromId(FieldId);
  return NULL;
  }

//---------------------------------------------------------------------------

void FxdEdtPage::SetFieldValue(rGDIBlk GB,FxdEdtFld * Fld, pchar Val)
  {
  if (Fld && !Fld->fEditing)
    {
    Strng Buff(Val);
    Fld->FixAfterLoad(Buff);//Buff, false);
//  StrRPad(tmp, Fld->Len);
    if (Fld->m_MarksChgd || Fld->Str.XStrICmp(Buff)!=0)//Fld->Str, Buff.Str, Fld->Len) !=0)
      {
      Fld->m_MarksChgd=0;
      Fld->Str=Buff;//f.Str, Fld->Len);
      Fld->Write(GB, true, false);
      }
    }
  }

//---------------------------------------------------------------------------

void FxdEdtPage::SetFieldValue(rGDIBlk GB,int RowNo, int FieldId, pchar Val)
  {
  FxdEdtFld * Fld=FldFromId(RowNo, FieldId);
  if (Fld)
    {
    Strng Buff(Val);
    //char tmp[512];
    //strncpy(tmp, Val, Min(Fld->Len, (int)sizeof(tmp)));
    Fld->FixAfterLoad(Buff);//, false);
//  StrRPad(tmp, Fld->Len);
    if (Fld->Str.XStrICmp(Buff) !=0)
      {
      Fld->Str=Buff;
      Fld->Write(GB, true, false);
      }
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FxdEdtBook

FxdEdtBook::FxdEdtBook()
  {
  iCmdCutTag=0;
  iCmdCutVal=0;
  iCmdCopyTag=0;
  iCmdCopyRHSTag=0;
  iCmdCopyVal=0;
  iCmdPaste=0;
  iSendToTrend=0;
  iSendToQuickView=0;
  iSendToHistory=0;
  iSendToArchive=0;
  iSendToCustom=0;
  fFixedPos=false;
  fForce1Page=false;
  iSlctRow=-1;
  iSlctFld=-1;

  RebuildReqd=false;
  ReloadReqd=false;
  CaretOK=false;
  WasEditing=false;
  CPgNo=-1;
  CPg=NULL;
  for (int i=0; i<MaxFixedPages; i++)
    Pg[i]=NULL;
  Pages=0;
  nCtrls=0;
  pCtrl=NULL;

  pComboBox=NULL;
  pPrevFld=NULL;
  TabIndexRows=-1;

  DownButton=NULL;
  FldEdit=NULL;

  pView=NULL;
  pWnd=NULL;
  }

//---------------------------------------------------------------------------

FxdEdtBook::~FxdEdtBook()
  {
  }

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtBook::AdjustFld(int PgNo, int RowNo, int FldNo, int WdtDelta)
  {
  FxdEdtFld * Fld=NULL;
  if (PgNo>=0 && PgNo<Pages)
    {
    pFxdEdtPage  pPg=Pg[PgNo];
    if (RowNo>=0 && RowNo<pPg->Rows)
      {
      FxdEdtRow* pRow=pPg->Row[RowNo];
      if (FldNo>=0 && FldNo<pRow->NFields)
        {
        Fld=pRow->Field[FldNo];
        if (!Fld->IsButton || Fld->m_iCtrlType==FXWC_CHECK || Fld->m_iCtrlType==FXWC_CHECKBTN)
          {
          if (WdtDelta>0)
            {
            }

          WdtDelta=Max(WdtDelta, (int)(-(Fld->Len-1)));
          Fld->Len+=WdtDelta;
          Fld->ColN+=WdtDelta;

          for (int i=FldNo+1; i<pRow->NFields; i++)
            {
            FxdEdtFld * pFld=pRow->Field[i];
            pFld->Col1+=WdtDelta;
            pFld->ColN+=WdtDelta;
            }
          }
        }
      }
    }

  return Fld;
  }

//---------------------------------------------------------------------------

void FxdEdtBook::FldHasFixedStrValue(int i, pchar pStrVal)
  {
  if (pPrevFld->IsFxdStrings==0)
    {
    pPrevFld->IsFxdStrings=1;
    pPrevFld->iCtrlIndex=nCtrls;
    pPrevFld->m_iCtrlType=FXWC_DDLST;
    AppendCtrlList(pPrevFld);
    }

  AppendStrValue(pPrevFld, i, pStrVal);
  };

//---------------------------------------------------------------------------

void FxdEdtBook::FldHasNANStrValue(int i, pchar pStrVal)
  {
  if (pPrevFld->HasNANStrings==0)
    {
    pPrevFld->HasNANStrings=1;
    pPrevFld->iCtrlIndex=nCtrls;
    pPrevFld->m_iCtrlType=FXWC_DDLSTOPT;
    AppendCtrlList(pPrevFld);
    }

  AppendStrValue(pPrevFld, i, pStrVal);
  };

//---------------------------------------------------------------------------

void FxdEdtBook::AppendCtrlList(FxdEdtFld * pFld_)//, pvoid pCtrl_, int iIndex_, pStrng pStr_)
  {
  int nMax=(((nCtrls / 16)+1)*16)-1;

  if (nCtrls>=nMax || pCtrl==NULL)
    {
    nMax=((((nCtrls+1) / 16)+1)*16)-1;
    pFxdEdtCtrl pNewCtrl=new FxdEdtCtrl[nMax];
    if (pCtrl)
      {
      for (int i=0; i<nCtrls; i++)
        pNewCtrl[i]=pCtrl[i];
      delete[] pCtrl;
      }

    pCtrl=pNewCtrl;
    }

  pCtrl[nCtrls].pFld=pFld_;
  pCtrl[nCtrls].pCtrl=NULL;
  //pCtrl[nCtrls].iIndex=0;
  pCtrl[nCtrls].pStrLst=NULL;
  nCtrls++;
  };

//---------------------------------------------------------------------------

void FxdEdtBook::AppendStrValue(FxdEdtFld * pFld_, int iIndex_, pchar pStr_)
  {
  int i=nCtrls-1;
  ASSERT(pCtrl[i].pFld==pFld_);

  //pCtrl[i].iIndex=iIndex_;
  if (pCtrl[i].pStrLst==NULL)
    pCtrl[i].pStrLst=new Strng_List;

  pStrng p=new Strng(pStr_);
  p->SetIndex(iIndex_);
  pCtrl[i].pStrLst->Append(p);
  };

//---------------------------------------------------------------------------

void FxdEdtBook::DeleteCtrlList()
  {
  if (pCtrl)
    {
    for (int i=0; i<nCtrls; i++)
      {
      if (pCtrl[i].pStrLst)
        delete pCtrl[i].pStrLst;
      if (pCtrl[i].pCtrl!=NULL)
        {
        #ifndef _RELEASE
        DoBreak(); //The control should have been deleted previously!!!
        #endif
        delete pCtrl[i].pCtrl; //to be "safe", delete now
        }
      }
    delete[] pCtrl;
    pCtrl=NULL;
    }
  nCtrls=0;
  }

//---------------------------------------------------------------------------

void FxdEdtBook::StartBuild()
  {
  };

//---------------------------------------------------------------------------

void FxdEdtBook::BuildDone()
  {
  };

//---------------------------------------------------------------------------

FxdEdtInfo::FxdEdtInfo()
  {
  BlkId=0;
  Pg=NULL;
  PageNo=0;
  PgRowNo=0;
  BlkRowNo=0;
  Fld=NULL;
  FieldId=-1;
  Index=0;
  Ptr=NULL;
  fPasteBusy=false;
//  LoadFromField(FE, Pg_, Fld, r);
  }

//---------------------------------------------------------------------------

FxdEdtInfo::FxdEdtInfo(pFxdEdtView pView, pFxdEdtPage Pg_, FxdEdtFld * Fld_, int r)
  {
  BlkId=Fld_->BlkId;
  Pg=Pg_;
  PageNo=Pg->No;
  PgRowNo=r;
  BlkRowNo=r-Pg->Blk[Fld_->BlkId].Row1;
  Fld=Fld_;
  FieldId=Fld_->FieldId;
  Index=Pg->Blk[Fld_->BlkId].Index;
  Ptr=Pg->Blk[Fld_->BlkId].Ptr;
  fPasteBusy=false;
  }

//---------------------------------------------------------------------------

void FxdEdtBook::Build(int NRows, int NCols, int ColIndent, pvoid BuildData)
  {
  };

//---------------------------------------------------------------------------

void FxdEdtBook::ChangeLayout(int TotalRows, int TotalCols)
  {
  };

//---------------------------------------------------------------------------

void FxdEdtBook::PageChanged(int NewPageNo, flag Initial)
  {
  }

//---------------------------------------------------------------------------

void FxdEdtBook::Load(FxdEdtInfo &EI, Strng & Str)
  {
  };

//---------------------------------------------------------------------------

long FxdEdtBook::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  return 0;
  };

//---------------------------------------------------------------------------

long FxdEdtBook::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  return 0;
  };

//---------------------------------------------------------------------------

flag FxdEdtBook::AnyFldsChanged()
  {
  for (int p=0; p<Pages; p++)
    {
    pFxdEdtPage pPg=Pg[p];
    for (int r=0; r<pPg->Rows; r++)
      for (int f=0; f<pPg->Row[r]->NFields; f++)
        if (pPg->Row[r]->Field[f]->Changed)
          return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

flag FxdEdtBook::ClrFldsChanged()
  {
  for (int p=0; p<Pages; p++)
    {
    pFxdEdtPage pPg=Pg[p];
    for (int r=0; r<pPg->Rows; r++)
      for (int f=0; f<pPg->Row[r]->NFields; f++)
        pPg->Row[r]->Field[f]->Changed=0;
    }
  return true;
  }

//---------------------------------------------------------------------------

void FxdEdtBook::OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  if (Pages>(fForce1Page ? 0 : 1) || GB.IsPrinting())
    {
    GDIBlkCfg &gbc = GB.GBC();
    CDC &dc = GB.DC();
    CDCResChk ResChk(dc);

    dc.SetMapMode(MM_TEXT);
    int OldTextColor=dc.SetTextColor(gbc.crText[0]);
    int OldTextMode=dc.SetBkMode(TRANSPARENT);

    if (GB.IsPrinting())
      {
      CRect PgR=GB.LuPageRect();
      CBrush BackBrush(RGB(255, 255, 255)); //KGA
      //dc.FillRect(PgR, GB.pBrushTxtBack);
      dc.FillRect(PgR, &BackBrush); //KGA

      int Yb=PgR.top-1;//CPg->Row[0]->Ys(GB)-1;
      int Yt=Yb-GB.RowHgt();
      //ScrollPt.y;
      int Xt=PgR.left;//GB.LuMargin.cx+GB.LuBorder.cx-(ChScroll.cx)*GB.ColWdt();
      CPoint Tp(Xt,Yt);
      POINT Tab[6];
      Tab[0].x=-10000;                            Tab[0].y=Yb;
      Tab[1].x=10000;                             Tab[1].y=Yb;

      CPen* OldPen=dc.SelectObject(GB.pPenTxtBord);
      //dc.SelectObject(GB.pBrushTxtEdgeLight);
      Strng Title,DtTm;
      Title.Set("%s    [SysCAD  %s]", CPg->TabName, CurDateTime(DtTm));//, FullCopyrightNotice());
      dc.TextOut(Tp.x, Tp.y, Title(), Title.Length());

      dc.MoveTo(Tab[0]);
      dc.LineTo(Tab[1]);
      dc.SelectObject(OldPen  );
      }
    else
      {// Draw Page Index Tabs
      CPen* OldPen=dc.SelectObject(GB.pPenTxtBord);
      CBrush* OldBrush=dc.SelectObject(GB.pBrushTxtFore);

      int Yt=(TabIndexRows)*GB.RowHgt()+GB.RowHgt()/2;
      int Yb=Yt+GB.RowHgt()/3;

      //Yt+=GB.ChScroll.cy;

      POINT Tab[4];
      Tab[0].x=-10000;                    Tab[0].y=Yb;
      Tab[1].x=10000;                     Tab[1].y=Yb;
      Tab[2].x=10000;                     Tab[2].y=Yt;
      Tab[3].x=-10000;                    Tab[3].y=Yt;

      dc.Polygon(Tab, 4);

      for (int p=CPgNo-1, Busy=1; Busy; --p)
        {
        if (p<0)
          p=Pages-1;
        #ifdef RotateTabs
        int DispRow=(Pg[p]->TabRow-CPg->TabRow+NTabRows)%NTabRows;
        #else
        int DispRow=Pg[p]->TabRow;
        #endif
        int Yt=(TabIndexRows-1-DispRow)*GB.RowHgt()+GB.RowHgt()/3;
        int Yb=Yt+GB.RowHgt()-1;
        int Xt=(Pg[p]->TabPos+1)*GB.ColWdt();
        //Yt+=GB.ChScroll.cy;
        //Yb+=GB.ChScroll.cy;
        //Xt+=GB.ChScroll.cx;
        CPoint Tp(Xt,Yt);
        POINT Tab[6];
        Tab[0].x=-10000;                                Tab[0].y=Yb;
        Tab[1].x=Tp.x-GB.ColWdt()+2;                     Tab[1].y=Yb;
        Tab[2].x=Tp.x-1;                                Tab[2].y=Yt;
        Tab[3].x=Tp.x+GB.ColWdt()*Pg[p]->TabWide+1;      Tab[3].y=Yt;
        Tab[4].x=Tp.x+GB.ColWdt()*(Pg[p]->TabWide+1)-2;  Tab[4].y=Yb;
        Tab[5].x=10000;                                 Tab[5].y=Yb;

        dc.SelectObject(GB.pPenTxtBord);
        dc.SelectObject((p==CPgNo) ? GB.pBrushTxtFore : GB.pBrushTxtBack);
        dc.Polygon(Tab, 6);
//        dc.TextOut(Tp.x, Tp.y, Pg[p]->TabName, Pg[p]->TabWide);

        CRect Txt  (Tab[2].x, Yt, Tab[3].x, Yb);
        dc.DrawText(Pg[p]->TabName, Pg[p]->TabWide, Txt, DT_SINGLELINE|DT_BOTTOM|DT_NOPREFIX|DT_CENTER);

        dc.SelectObject(GB.pPenTxtBack);
        Tab[1].x+=1;
        dc.MoveTo(Tab[1]);
        dc.LineTo(Tab[4]);

        Busy=(p!=CPgNo);
        }
      dc.SelectObject(OldPen  );
      dc.SelectObject(OldBrush);
      }
    dc.SetBkMode(OldTextMode);
    dc.SetTextColor(OldTextColor);
    }
  }

//---------------------------------------------------------------------------

void FxdEdtBook::OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
//  if (Pages>(fForce1Page ? 0 : 1) || GB.IsPrinting())
//    {
//    GDIBlkCfg &gbc = GB.GBC();
//    CDC &dc = GB.DC();
//
//    dc.SetMapMode(MM_TEXT);
//    int OldTextColor=dc.SetTextColor(gbc.crText[0]);
//    int OldTextMode=dc.SetBkMode(TRANSPARENT);
//
//    if (GB.IsPrinting())
//      {
//      CRect PgR=GB.LuPageRect();
//      CBrush BackBrush(RGB(255, 255, 255)); //KGA
//      //dc.FillRect(PgR, GB.pBrushTxtBack);
//      dc.FillRect(PgR, &BackBrush); //KGA
//
//      int Yb=PgR.top-1;//CPg->Row[0]->Ys(GB)-1;
//      int Yt=Yb-GB.RowHgt();
//      //ScrollPt.y;
//      int Xt=PgR.left;//GB.LuMargin.cx+GB.LuBorder.cx-(ChScroll.cx)*GB.ColWdt();
//      CPoint Tp(Xt,Yt);
//      POINT Tab[6];
//      Tab[0].x=-10000;                            Tab[0].y=Yb;
//      Tab[1].x=10000;                             Tab[1].y=Yb;
//
//      dc.SelectObject(GB.pPenTxtBord);
//      //dc.SelectObject(GB.pBrushTxtEdgeLight);
//      Strng Title, DtTm;
//      Title.Set("%s    [SysCAD  %s]", CPg->TabName, CurDateTime(DtTm));//, FullCopyrightNotice());
//      dc.TextOut(Tp.x, Tp.y, Title(), Title.Length());
//
//      dc.MoveTo(Tab[0]);
//      dc.LineTo(Tab[1]);
//      }
//    else
//      {// Draw Page Index Tabs
//      CPen* OldPen=dc.SelectObject(GB.pPenTxtBord);
//      CBrush* OldBrush=dc.SelectObject(GB.pBrushTxtFore);
//
//      int Yt=(TabIndexRows)*GB.RowHgt()+GB.RowHgt()/2;
//      int Yb=Yt+GB.RowHgt()/3;
//
//      //Yt+=GB.ChScroll.cy;
//
//      POINT Tab[4];
//      Tab[0].x=-10000;                    Tab[0].y=Yb;
//      Tab[1].x=10000;                     Tab[1].y=Yb;
//      Tab[2].x=10000;                     Tab[2].y=Yt;
//      Tab[3].x=-10000;                    Tab[3].y=Yt;
//
//      dc.Polygon(Tab, 4);
//
//      for (int p=CPgNo-1, Busy=1; Busy; --p)
//        {
//        if (p<0)
//          p=Pages-1;
//        #ifdef RotateTabs
//        int DispRow=(Pg[p]->TabRow-CPg->TabRow+NTabRows)%NTabRows;
//        #else
//        int DispRow=Pg[p]->TabRow;
//        #endif
//        int Yt=(TabIndexRows-1-DispRow)*GB.RowHgt()+GB.RowHgt()/3;
//        int Yb=Yt+GB.RowHgt()-1;
//        int Xt=(Pg[p]->TabPos+1)*GB.ColWdt();
//        //Yt+=GB.ChScroll.cy;
//        //Yb+=GB.ChScroll.cy;
//        //Xt+=GB.ChScroll.cx;
//        CPoint Tp(Xt,Yt);
//        POINT Tab[6];
//        Tab[0].x=-10000;                                Tab[0].y=Yb;
//        Tab[1].x=Tp.x-GB.ColWdt()+2;                     Tab[1].y=Yb;
//        Tab[2].x=Tp.x-1;                                Tab[2].y=Yt;
//        Tab[3].x=Tp.x+GB.ColWdt()*Pg[p]->TabWide+1;      Tab[3].y=Yt;
//        Tab[4].x=Tp.x+GB.ColWdt()*(Pg[p]->TabWide+1)-2;  Tab[4].y=Yb;
//        Tab[5].x=10000;                                 Tab[5].y=Yb;
//
//        dc.SelectObject(GB.pPenTxtBord);
//        dc.SelectObject((p==CPgNo) ? GB.pBrushTxtFore : GB.pBrushTxtBack);
//        dc.Polygon(Tab, 6);
////        dc.TextOut(Tp.x, Tp.y, Pg[p]->TabName, Pg[p]->TabWide);
//
//        CRect Txt  (Tab[2].x, Yt, Tab[3].x, Yb);
//        dc.DrawText(Pg[p]->TabName, Pg[p]->TabWide, Txt, DT_SINGLELINE|DT_BOTTOM|DT_NOPREFIX|DT_CENTER);
//
//        dc.SelectObject(GB.pPenTxtBack);
//        Tab[1].x+=1;
//        dc.MoveTo(Tab[1]);
//        dc.LineTo(Tab[4]);
//
//        Busy=(p!=CPgNo);
//        }
//      dc.SelectObject(OldPen  );
//      dc.SelectObject(OldBrush);
//      }
//    dc.SetBkMode(OldTextMode);
//    dc.SetTextColor(OldTextColor);
//    }
  }

//---------------------------------------------------------------------------

void FxdEdtBook::OnDrawLine(rGDIBlk GB,int PgNo, int RowNo)
  {
  // TODO: add draw code here
  //  int y=Offset.y+RowHgt*R;
  //  Row[R].Visible()=(y>=ScrollPt.y && y<=ScrollPt.y+(ClientR.bottom-ClientR.top));
  //dbgpln("DL %4i %i %s", R, Row[R]. Visible, Row[R].Str);
  if (CRow(RowNo)->Visible(GB))
    CRow(RowNo)->Write(GB);
  }

//---------------------------------------------------------------------------

int FxdEdtBook::EdtRow()
  {
  int r=ChEditPos.y;
  return (CPg && r>=0 && r<CPg->Rows ? r : -1);
  };

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtBook::CurrentFld()
  {
  int r=EdtRow();
  if (r>=0)
    {
    int f=0;
    FxdEdtRow* rp=CPg->Row[r];
    while ((f<rp->NFields) && (rp->Field[f]->ColN<=ChEditPos.x)) f++;
    if (f<rp->NFields)
      return rp->Field[f];
    }
  return NULL;
  };

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtBook::EdtFld()
  {
  int r=EdtRow();
  if (r>=0)
    {
    int f=0;
    FxdEdtRow* rp=CPg->Row[r];
    while ((f<rp->NFields) && (rp->Field[f]->ColN<=ChEditPos.x)) f++;
    if (f<rp->NFields && rp->Field[f]->fEditable)
      return rp->Field[f];
    }
  return NULL;
  };

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtBook::ButtonFld()
  {
  int r=EdtRow();
  if (r>=0)
    {
    int f=0;
    FxdEdtRow* rp=CPg->Row[r];
    while ((f<rp->NFields) && (rp->Field[f]->ColN<=ChEditPos.x)) f++;
    if (f<rp->NFields && (rp->Field[f]->IsButton || rp->Field[f]->IsIndexTab))
      return rp->Field[f];
    }
  return NULL;
  };

//---------------------------------------------------------------------------

int FxdEdtBook::EdtChar(FxdEdtFld * Fld)
  {
    return Range(0L, long(ChEditPos.x-Fld->Col1), Fld->Len-1L);
  };

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtBook::FirstSlctFld()
  {
  iSlctRow=0;
  iSlctFld=-1;
  return  NextSlctFld();
  };

//---------------------------------------------------------------------------

FxdEdtFld * FxdEdtBook::NextSlctFld()
  {
  if (CPg)
    {
    for (;;)
      {
      if (iSlctRow>=CPg->Rows)
        return NULL;

      iSlctFld++;
      if (iSlctFld>=CPg->Row[iSlctRow]->NFields)
        {
        iSlctRow++;
        iSlctFld=-1;
        }
      else if (CPg->Row[iSlctRow]->Field[iSlctFld]->fSelected)
        return CPg->Row[iSlctRow]->Field[iSlctFld];
      }
    }
  return NULL;
  };

//---------------------------------------------------------------------------

flag FxdEdtBook::MoreSlctFlds()
  {
  if (CPg)
    {
    int lclSlctRow=iSlctRow;
    int lclSlctFld=iSlctFld;

    for (;;)
      {
      if (lclSlctRow>=CPg->Rows)
        return false;

      lclSlctFld++;
      if (lclSlctFld>=CPg->Row[lclSlctRow]->NFields)
        {
        lclSlctRow++;
        lclSlctFld=-1;
        }
      else if (CPg->Row[lclSlctRow]->Field[lclSlctFld]->fSelected)
        return true;
      }
    }
  return false;
  };

//---------------------------------------------------------------------------

flag FxdEdtBook::DoLButtonDown(UINT nFlags, CPoint point) { return false;}
flag FxdEdtBook::DoLButtonUp(UINT nFlags, CPoint point) { return false;}
flag FxdEdtBook::DoLButtonDblClk(UINT nFlags, CPoint point) { return false; }
flag FxdEdtBook::DoRButtonDown(UINT nFlags, CPoint point) { return false; }
flag FxdEdtBook::DoRButtonUp(UINT nFlags, CPoint point) { return false; }
flag FxdEdtBook::DoRButtonDblClk(UINT nFlags, CPoint point) { return false; }
flag FxdEdtBook::DoMouseMove(UINT nFlags, CPoint point) { return false; }
flag FxdEdtBook::DoKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) { return false; };

flag FxdEdtBook::DoAccCnv(UINT Id){return false;};
flag FxdEdtBook::DoAccFmt(UINT Id){return false;};

//---------------------------------------------------------------------------

flag FxdEdtBook::DoAccRptTagLists(TagInfoBlk & WrkIB)
  {
  TagInfoBlk * IB = new TagInfoBlk(WrkIB.ObjClassId(), tt_NULL, "", WrkIB.RefTag(), 
                                   WrkIB.CnvOK() ? &WrkIB.Cnv() : NULL, 
                                   WrkIB.FmtOK() ? &WrkIB.Fmt() : NULL);
  AfxGetMainWnd()->PostMessage(WMU_ADDTOQUICKVIEW, 0, (long)(IB));
  return true;
  //return false;
  };

//---------------------------------------------------------------------------

flag FxdEdtBook::DoCutTag(FxdEdtFld * pFld, char *buff, int buffsize)
  {
  strncpy(buff, pFld->Tag ? pFld->Tag : "", buffsize);
  XStrTrim(buff);
  return true;
  }
flag FxdEdtBook::DoCutVal(FxdEdtFld * pFld, char *buff, int buffsize)
  {
  strncpy(buff, pFld->Str()? pFld->Str(): "", buffsize);
  XStrTrim(buff);
  if (pFld->fEditable)
    pFld->Str="";
  return true;
  }
flag FxdEdtBook::DoCopyTag(FxdEdtFld * pFld, char *buff, int buffsize)
  {
  strncpy(buff, pFld->Tag ? pFld->Tag : "", buffsize);
  XStrTrim(buff);
  return true;
  }
flag FxdEdtBook::DoCopyRHSTag(FxdEdtFld * pFld, char *buff, int buffsize)
  {
  if (pFld->Tag)
    strncpy(buff, strchr(pFld->Tag, '.') ? strchr(pFld->Tag, '.')+1 : pFld->Tag, buffsize);
  else
    strncpy(buff, "", buffsize);
  XStrTrim(buff);
  return true;
  }
flag FxdEdtBook::DoCopyVal(FxdEdtFld * pFld, char *buff, int buffsize)
  {
  strncpy(buff, pFld->Str()? pFld->Str(): "", buffsize);
  XStrTrim(buff);
  return true;
  }
flag FxdEdtBook::DoPaste(FxdEdtFld * pFld, char *buff)
  {
  pFld->Str=buff;
  pFld->Str.RPad(pFld->Len);
  return true;
  }

//===========================================================================

IMPLEMENT_SPARES(FxdEdtFld, 5000)
IMPLEMENT_SPARES(FxdEdtRow, 500)
IMPLEMENT_SPARES(FxdEdtInfo, 500)
IMPLEMENT_SPARES(FxdEdtBlk, 100)
IMPLEMENT_SPARES(FxdEdtPage, 10)
IMPLEMENT_SPARES(FxdEdtCtrl, 100)

//===========================================================================

bool FxdEdtView::m_WheelChangeVal = false;
GDIBlkCfg  FxdEdtView::GDICfg("FxdEdtCfg");

//void SetEditControl();
void ClrEditControl();
//void SetCBoxControl();
void ClrCBoxControl();

//=======================================================================
//
//
//
//=======================================================================

class CFxdEdtEdit : public CEdit
  {
  public:
    virtual ~CFxdEdtEdit();
    flag    fAcceptChgs;
    int     nStartCharAtStart;
  protected:
    void FixCaret();

    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnDestroy();
    afx_msg void OnSetFocus( CWnd* pOldWnd );
    afx_msg void OnKillFocus( CWnd* pNewWnd );
    DECLARE_MESSAGE_MAP()
  };

class CFxdEdtCBox : public CComboBox
  {
  public:
    flag    fAcceptChgs;
    int     nStartCharAtStart;
  protected:
    void FixCaret();

    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnDestroy();
    afx_msg void OnSetFocus( CWnd* pOldWnd );
    afx_msg void OnKillFocus( CWnd* pNewWnd );
    DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------

static int fInsertMode=false;
static int fInBigEditMode=false;
static CFxdEdtEdit  *pEdtControl=NULL;
static FxdEdtFld    *pEdtCtrlFld=NULL;
static FxdEdtView   *pEdtCtrlView=NULL;
static int IgnoreOneScroll=false;

static flag fCBoxSelEndDone=false;
static CFxdEdtCBox  *pCBoxControl=NULL;
static FxdEdtFld    *pCBoxCtrlFld=NULL;
static FxdEdtView   *pCBoxCtrlView=NULL;

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFxdEdtEdit, CEdit)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_DESTROY()
//  ON_COMMAND(ID_EDIT_PASTE, OnPaste)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

CFxdEdtEdit::~CFxdEdtEdit()
  {
  TRACE("x");
  }

//---------------------------------------------------------------------------

void CFxdEdtEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CEdit::OnChar(nChar, nRepCnt, nFlags);
  if (nChar>=' ')
    {
    CString S;
    pEdtControl->GetWindowText(S);
    int nStartChar, nEndChar;
    pEdtControl->GetSel(nStartChar, nEndChar);
    if (!fInsertMode)
      {
      CString St(S);
      S=(nStartChar>0) ? St.Left(nStartChar) : "";
      if (nStartChar<St.GetLength()-1)
        S+=St.Right(St.GetLength()-1-nStartChar);//SetAt(j, S[j+1]);
      pEdtControl->SetWindowText(S);
      }

    flag BlanksOnly=1;
    for (int j=pEdtCtrlFld->Len; BlanksOnly && j<S.GetLength(); j++)
      BlanksOnly=(S[j]==' ');
    if (!BlanksOnly)
      {
      if (S[0]==' ')
        {
        CString SN(S.Right(S.GetLength()-1));
        pEdtControl->SetWindowText(SN);
        nStartChar--;
        nEndChar--;
        }
      }
    pEdtControl->SetSel(Max(0,nStartChar), Max(0,nEndChar));
    }
  }

//---------------------------------------------------------------------------

void CFxdEdtEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  switch (nChar)
    {
    case VK_INSERT  :
      fInsertMode=!fInsertMode;
      pEdtCtrlView->CaretOK=0;
      FixCaret();
      break;
    case VK_RETURN  :
      fAcceptChgs=true;
      if (pEdtCtrlView)
        pEdtCtrlView->SetFocus();
      break;
    case VK_ESCAPE  :
      fAcceptChgs=false;
      if (pEdtCtrlView)
        pEdtCtrlView->SetFocus();
      break;
    case VK_UP      :
    case VK_DOWN    :
      if (fInBigEditMode)
        CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
      else
        {
        fAcceptChgs=true;
        if (pEdtCtrlView)
          pEdtCtrlView->SetFocus();
        pEdtCtrlView->SendMessage(WM_KEYDOWN, nChar, 0);
        }
      break;

    default         :
      CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
      break;
    }
  }

//---------------------------------------------------------------------------

void CFxdEdtEdit::OnDestroy( )
  {
  ClrEditControl();
  pEdtControl=NULL;
  CEdit::OnDestroy();
  }

//---------------------------------------------------------------------------

void CFxdEdtEdit::OnSetFocus( CWnd* pOldWnd )
  {
  CEdit::OnSetFocus( pOldWnd );
  FixCaret();
  };

//---------------------------------------------------------------------------

void CFxdEdtEdit::OnKillFocus( CWnd* pNewWnd )
  {
  CEdit::OnKillFocus( pNewWnd );
  HideCaret();
  DestroyCaret();
  };

//---------------------------------------------------------------------------

void CFxdEdtEdit::FixCaret()
  {
  HideCaret();
  DestroyCaret();
  if (fInsertMode)
    ::CreateCaret(m_hWnd, (HBITMAP)0, 2, pEdtCtrlView->ScrGB.RowHgt()-2);
  else
    ::CreateCaret(m_hWnd, (HBITMAP)1, pEdtCtrlView->ScrGB.ColWdt(),pEdtCtrlView->ScrGB.RowHgt()-2);
  ShowCaret();
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFxdEdtCBox, CComboBox)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_DESTROY()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CFxdEdtCBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  CComboBox::OnChar(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

void CFxdEdtCBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  switch (nChar)
    {
    case VK_RETURN  :
      fAcceptChgs=true;
      if (pCBoxCtrlView)
        pCBoxCtrlView->SetFocus();
      break;
    case VK_ESCAPE  :
      fAcceptChgs=false;
      if (pCBoxCtrlView)
        pCBoxCtrlView->SetFocus();
      break;
    default         :
      CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
      break;
    }
  }

//---------------------------------------------------------------------------

void CFxdEdtCBox::OnDestroy( )
  {
  ClrCBoxControl();
  pCBoxControl=NULL;
  CComboBox::OnDestroy();
  }

//---------------------------------------------------------------------------

void CFxdEdtCBox::OnSetFocus( CWnd* pOldWnd )
  {
  CComboBox::OnSetFocus( pOldWnd );
  FixCaret();
  };

//---------------------------------------------------------------------------

void CFxdEdtCBox::OnKillFocus( CWnd* pNewWnd )
  {
  CComboBox::OnKillFocus( pNewWnd );
  HideCaret();
  DestroyCaret();
  };

//---------------------------------------------------------------------------

void CFxdEdtCBox::FixCaret()
  {
  HideCaret();
  DestroyCaret();
  if (fInsertMode)
    ::CreateCaret(m_hWnd, (HBITMAP)0, 2, pCBoxCtrlView->ScrGB.RowHgt()-2);
  else
    ::CreateCaret(m_hWnd, (HBITMAP)1, pCBoxCtrlView->ScrGB.ColWdt(),pCBoxCtrlView->ScrGB.RowHgt()-2);
  ShowCaret();
  }

//=======================================================================
//
//
//
//=======================================================================

//#define Brd 0

void SetEditControl(FxdEdtFld * Fld, pFxdEdtView pView, UINT nChar, int EditPos, flag ClrContent)
  {
  Fld->HasDDLst=false;
  Fld->HasEdtBox=true;

  pEdtCtrlFld=Fld;
  pEdtCtrlView=pView;
  pEdtCtrlView->bShiftDown=0;
  pEdtCtrlView->bControlDown=0;
  pEdtCtrlView->bChanged=true;

  CRect Rect;

  fInBigEditMode=(Fld->BigStrLength>0 && !Fld->IsButton);
  if (fInBigEditMode)
    {
    const int cx=GetSystemMetrics(SM_CXBORDER);
    const int cy=GetSystemMetrics(SM_CYBORDER);
    Rect.SetRect(0, pEdtCtrlFld->Ye(pView->ScrGB)-(cy)+2,
                 Max(pEdtCtrlFld->Xe(pView->ScrGB),(int)(23+16*pView->ScrGB.ColWdt()))+(1+cx+3),
                 pEdtCtrlFld->Ye(pView->ScrGB)+(cy)+1+pView->ScrGB.RowHgt()*5);
    }
  else
    {
    Rect.SetRect(pEdtCtrlFld->Xs(pView->ScrGB), pEdtCtrlFld->Ys(pView->ScrGB)+1,
                 pEdtCtrlFld->Xe(pView->ScrGB)+3, pEdtCtrlFld->Ye(pView->ScrGB));
    }
  pEdtCtrlView->HideCaret();

  // Fix formatting
  switch (pEdtCtrlFld->Just)
    {
    case 0: // left
    case 3: // none
      if (!fInBigEditMode)
        pEdtCtrlFld->Str.RPad(pEdtCtrlFld->Len);
      break;
    case 1: // centre
      pEdtCtrlFld->Str.LRPad(pEdtCtrlFld->Len);
      break;
    case 2: // right
      pEdtCtrlFld->Str.LPad(pEdtCtrlFld->Len);
      break;
    }

  CString S(pEdtCtrlFld->Str());

  if (pEdtControl==NULL)
    {
    pEdtControl=new CFxdEdtEdit;
    CRect SpRect;
    DWORD Styles;
    SpRect.left=Rect.right;
    SpRect.top=Rect.top-2;
    SpRect.bottom=Rect.bottom+2;
    SpRect.right=Rect.right+(Rect.bottom-Rect.top)/2;
    if (fInBigEditMode)
      Styles = WS_CHILD | WS_TABSTOP | WS_DLGFRAME | ES_MULTILINE | ES_AUTOVSCROLL;
    else
      Styles = WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL;
    pEdtControl->Create(Styles, Rect, pView, FxdEdtCtrlID0-2);
    pEdtControl->SetFont(pView->ScrGB.pFont[0]);
    pEdtControl->fAcceptChgs=true;
    }

  pEdtCtrlView->StartCharEdit(pEdtCtrlFld);
  pEdtCtrlFld->fEditing=1;

  DestroyCaret();
  pEdtCtrlView->CaretOK=0;

  int nStartChar=0;
  if (ClrContent)
    {
    // Only Clear the Content if the Start Pos is Clear
    flag BlanksOnly=1;
    for (int j=0; BlanksOnly && j<S.GetLength() && j<=EditPos; j++)
      BlanksOnly=(S[j]==' ');
    if (BlanksOnly)
      S="";
    else
      nStartChar=EditPos;
    }
  else
    {
    S=pEdtCtrlFld->Str();
    nStartChar=EditPos;
    }

  pEdtControl->SetWindowText(S);

  pEdtControl->SetFocus();
  pEdtControl->SetSel(nStartChar, nStartChar);
  pEdtControl->nStartCharAtStart=nStartChar;

  short LimText=255;
  if (fInBigEditMode)
    LimText=Max((short)(pEdtCtrlFld->ColN-pEdtCtrlFld->Col1), Fld->BigStrLength);
  else
    {
    const ExtraCharsInField=256;
    LimText=pEdtCtrlFld->ColN-pEdtCtrlFld->Col1+ExtraCharsInField;
    }
  pEdtControl->SetLimitText(LimText);
  if (nChar)
    pEdtControl->SendMessage(WM_CHAR, (WPARAM)nChar, 0);
  pEdtControl->SetWindowPos(NULL, Rect.left, Rect.top, 0,0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
  }

//---------------------------------------------------------------------------

void ClrEditControl()
  {
  if (pEdtCtrlView && pEdtCtrlFld)
    {
    pEdtCtrlView->ChEditPos.x=Min((short)pEdtCtrlView->ChEditPos.x, pEdtCtrlFld->ColN);

    if (pEdtControl->fAcceptChgs)
      {
      CString WT;
      pEdtControl->GetWindowText(WT);
      if (WT.CompareNoCase(pEdtCtrlFld->Str())!=0)
        {
        pEdtCtrlFld->Str=(const char *)WT;
        //pEdtCtrlFld->Str[pEdtCtrlFld->Len]=0;
        pEdtCtrlFld->AcceptChanges=1;
        }
      }
    else
      {
      pEdtCtrlFld->AcceptChanges=0;
      pEdtCtrlFld->fEditing=0;
      }

    pEdtCtrlView->EndCharEdit(pEdtCtrlFld);
    pEdtCtrlView->FixCaret();
    pEdtCtrlView->PositionCaret(PC_NoRedraw | PC_ClearSlct | PC_SetSlct);
    
    if (fInBigEditMode)
      {
      pEdtCtrlView->Invalidate();//true);
      }
    }

  //pEdtControl=NULL;
  //pEdtSpin=NULL; // DOntClear This
  //pEdtCtrlView=NULL; Dont Clear this
  pEdtCtrlFld=NULL;
  }

//=======================================================================
//
//
//
//=======================================================================
char IndexMarker='[';

void SetCBoxControl(FxdEdtFld * Fld, pFxdEdtView pView, UINT nChar, flag ShowDrpDwn)//, int EditPos)//, flag ClrContent)
  {
  Fld->HasDDLst=true;
  Fld->HasEdtBox=false;

  pCBoxCtrlFld=Fld;
  pCBoxCtrlView=pView;
  pCBoxCtrlView->bShiftDown=0;
  pCBoxCtrlView->bControlDown=0;
  pCBoxCtrlView->bChanged=true;

  CRect Rect;
  const int MinListBoxLen = 15;

  pStrng_List pL=pView->pCtrl[Fld->iCtrlIndex].pStrLst;
  int InitWide=(pCBoxCtrlFld->Xe(pView->ScrGB)-pCBoxCtrlFld->Xs(pView->ScrGB))/pView->ScrGB.ColWdt();
  int MaxWide=InitWide;
       
  for (pStrng pS=pL->First(); pS; pS=pS->Next())
    MaxWide=Max(MaxWide, (int)strlen((pS->Str() ? pS->Str() : "")));

  const int cx=GetSystemMetrics(SM_CXHSCROLL);
  const XtraX=(Fld->HasNANStrings ? 0:3);
  int DDRows=pL->Length()+(Fld->HasNANStrings ?1:0);

  Rect.SetRect(pCBoxCtrlFld->Xs(pView->ScrGB)-2,
               pCBoxCtrlFld->Ys(pView->ScrGB)-2,
               pCBoxCtrlFld->Xe(pView->ScrGB)+(MaxWide-InitWide+XtraX)*pView->ScrGB.ColWdt()+cx,
               pCBoxCtrlFld->Ye(pView->ScrGB)+1+pView->ScrGB.RowHgt()*Min(MinListBoxLen+1, DDRows+1)+2 //Extra for  Integral No Of Rows
               );
  pCBoxCtrlView->HideCaret();
  if (Fld->iCtrlIndex>=0)
    ((CBitmapButton*)(pView->pCtrl[Fld->iCtrlIndex].pCtrl))->ShowWindow(SW_HIDE);

  CString S(pCBoxCtrlFld->Str());

  if (pCBoxControl==NULL)
    {
    pCBoxControl= new CFxdEdtCBox();
    pCBoxControl->Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_BORDER | /*LBS_NOTIFY |*/ WS_VSCROLL,
      Rect, pView, FxdEdtCtrlID0-1);//FxdEdtCtrlID0+MaxFixedControls+iCtrl);
    pCBoxControl->SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOMOVE|/*SWP_NOSENDCHANGING|*/SWP_NOSIZE);
    pCBoxControl->SetFont(pView->ScrGB.pFont[0]);
    pCBoxControl->SetItemHeight(-1,pView->ScrGB.RowHgt()-2);
    pCBoxControl->ResetContent();
    bool ValueInList=false;
    for (pStrng pS=pL->First(); pS; pS=pS->Next())
      {
      char *pStr=(pS->Str() ? pS->Str() : "");
      ValueInList = ValueInList || stricmp(pStr, Fld->Str())==0;
      Strng S;
      if (Fld->HasNANStrings)
        S=pStr;
      else
        S.Set("%s%*s%c%2i", pStr, Max(0, (int)(MaxWide-strlen(pStr))),"", IndexMarker, pS->Index());
      pCBoxControl->AddString(S());
      }
    if (Fld->HasNANStrings && !ValueInList) // Optionally Add current value
      pCBoxControl->AddString(Fld->Str());
    pCBoxControl->SetFocus();
    
    if (ShowDrpDwn)
      pCBoxControl->ShowDropDown(true);

    pCBoxControl->fAcceptChgs=false;//true;
    }

  pCBoxCtrlFld->fEditing=1;

  DestroyCaret();
  pCBoxCtrlView->CaretOK=0;

  S=pCBoxCtrlFld->Str();
  pCBoxControl->SelectString(-1, S);

  pCBoxControl->SetFocus();
  if (nChar)
    pCBoxControl->SendMessage(WM_CHAR, nChar, 0);
    //pCBoxControl->OnChar(nChar, nRepCnt, nFlags);
//  pCBoxControl->SetWindowPos(NULL, Rect.left, Rect.top, 0,0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
  pCBoxControl->SetWindowPos(&CWnd::wndTop, Rect.left, Rect.top, 0,0, SWP_NOSIZE | SWP_SHOWWINDOW);
  fCBoxSelEndDone=false;
  }

//---------------------------------------------------------------------------

void ClrCBoxControl()
  {
  if (pCBoxCtrlView && pCBoxCtrlFld)
    {
    pCBoxCtrlView->ChEditPos.x=Min((short)pCBoxCtrlView->ChEditPos.x, pCBoxCtrlFld->ColN);

    if (pCBoxControl->fAcceptChgs)
      {
      CString WT;
      pCBoxControl->GetWindowText(WT);
      int i=WT.ReverseFind(IndexMarker);
      if (i>=0)
        WT.Delete(i,1024);
      WT.TrimRight(" ");
      if (WT.CompareNoCase(pCBoxCtrlFld->Str() ? pCBoxCtrlFld->Str() : "")!=0)
        {
        pCBoxCtrlFld->Str=(const char *)WT;
        pCBoxCtrlFld->AcceptChanges=1;
        }
      }
    else
      {
      pCBoxCtrlFld->AcceptChanges=0;
      pCBoxCtrlFld->fEditing=0;
      }

    pCBoxCtrlView->EndCharEdit(pCBoxCtrlFld);
    pCBoxCtrlView->FixCaret();
    pCBoxCtrlView->PositionCaret(PC_NoRedraw | PC_ClearSlct | PC_SetSlct);;
    if (fInBigEditMode)
      {
      pCBoxCtrlView->Invalidate();//true);
      }
    }

  pCBoxCtrlFld=NULL;
  fCBoxSelEndDone=false;
  }

//=======================================================================
//
//
//
//=======================================================================
// FxdEdtView

IMPLEMENT_DYNCREATE(FxdEdtView, CView) //CScrollView)

FxdEdtView::FxdEdtView()
  {
  pView=this;
  //for (int i=0; i < MaxFixedRows; i++)
  //  Row[i]=NULL;
  for (int i=0; i < MaxFixedPages; i++)
    Pg[i]=NULL;

  RebuildReqd=true;
  ReloadReqd=true;
  bShiftDown=false;
  bControlDown=false;
  bChanged=false;
  bWinCtrlListBusy=0;
  Pages=0;
  CPgNo=-1;
  CPg=NULL;
  TabIndexRows=0;
  //FixedRows=0;
  ChEditPos.x=0;
  ChEditPos.y=0;
  CaretOK=0;
  pWnd=this;

  DownButton=NULL;

  CDC DC;
  DC.CreateCompatibleDC(NULL);
  CDCResChk ResChk(DC);
  ScrGB.Create(&GDICfg, this, &DC, false);
  ScrGB.Attach(&DC, NULL); //  this attach detach will set up internal variables
  ScrGB.Detach();          //

  VScrl=NULL;
  HScrl=NULL;
  VScrlOn=1;
  HScrlOn=1;
  RedrawNeeded=0;

  //pTestBtn=NULL;
  //pTestChk=NULL;
  //pTestLst=NULL;
//  ypLstBox=NULL;
  pComboBox=NULL;

  nCtrls=0;
  pCtrl=NULL;
  CPgNoSP=0;

  dwRebuildCnt=0;
  hRqdCursor=NULL;
  }

//---------------------------------------------------------------------------

FxdEdtView::~FxdEdtView()
  {

  if (VScrl) delete VScrl;
  if (HScrl) delete HScrl;
  //delete pScrGB;

//  if (ypLstBox)
//    delete ypLstBox;
  if (pComboBox)
    delete pComboBox;

  DeleteWinControls();

  if (pEdtControl)
    pEdtControl->DestroyWindow();
  if (pCBoxControl)
    pCBoxControl->DestroyWindow();
  //if (pEdtSpin)
  //  pEdtSpin->DestroyWindow();

  for (int i=0; i < MaxFixedPages; i++)
    if (Pg[i])
      delete Pg[i];

  }

//---------------------------------------------------------------------------

BOOL FxdEdtView::PreCreateWindow(CREATESTRUCT &cs)
  {
  //KGA 27/2/97: CS_CLASSDC style for the flow engine/model DLL window does not
  // work on a Windows 95 PC. It seems to work for other derived windows. The
  // tagvtext (trend window) overrides this function and uses the CS_CLASSDC style.

  //cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_CLASSDC,
  //                                 AfxGetApp()->LoadStandardCursor(IDC_ARROW), //IBEAM),
  //                                 (HBRUSH)GetStockObject(/*WHITE*/LTGRAY_BRUSH), 0);
  cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW/* | CS_CLASSDC*/,
                                   AfxGetApp()->LoadStandardCursor(IDC_ARROW),
                                   (HBRUSH)GetStockObject(LTGRAY_BRUSH), 0);
  return CView::PreCreateWindow(cs);
  }

//---------------------------------------------------------------------------

//void FxdEdtView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
//  {
//  CView::OnActivateView( bActivate, pActivateView, pDeactiveView );
//  if (bActivate)
//    FixCaret();
//  else
//    {
//    CaretOK=0;
//    HideCaret();
//    DestroyCaret();
//    }
//  };

//---------------------------------------------------------------------------

BOOL FxdEdtView::Create(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle,
                        const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
  {
  return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(FxdEdtView, CView)
  //{{AFX_MSG_MAP(FxdEdtView)
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
	ON_WM_TIMER()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_HSCROLL()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_RBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
  ON_COMMAND(ID_EDIT_CUT, OnCut)
  ON_COMMAND(ID_EDIT_COPY, OnCopy)
  ON_COMMAND(ID_EDIT_PASTE, OnPaste)
  ON_MESSAGE(WMU_FXDEDTEDITDONE, OnEditDone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void FxdEdtView::SelectPage(int No, flag Initial)
  {
  No=Min(Pages-1,(int)No);
  if (No>=0)
    {
    CPg=Pg[No];
    CPgNo=No;
    for (int r=0; r<CPg->Rows; r++)
      for (int f=0; f<CPg->Row[r]->NFields; f++)
        CPg->Row[r]->Field[f]->fSelected=false;
    }
  else
    {
    CPg=NULL;
    CPgNo=-1;
    }
  PageChanged(CPgNo, Initial);
  FixScrollSizes(true);

  InvalidateRect(NULL);
  }

//---------------------------------------------------------------------------

flag FxdEdtView::LocateFromCR(int ColNo, int RowNo, FxdEdtInfo &EI)
  {
  int cb=0;
  while (cb<CPg->Blks-1 && CPg->Blk[cb+1].Row1<=RowNo && CPg->Blk[cb+1].Row1>=0) cb++;
  //dbgpln("cb %i",cb);

  FxdEdtFld * Fld=NULL;
  if (cb<CPg->Blks && CPg->Blk[cb].Row1>=0 && RowNo<CPg->Rows)
    {
  //  if (RowNo<CPg->Rows)
    for (int f=0; f<CPg->Row[RowNo]->NFields && Fld==NULL; f++)
      {
      FxdEdtFld * FldT=CPg->Row[RowNo]->Field[f];
      if (FldT->Col1<=ColNo && FldT->ColN>ColNo)
        Fld=FldT;
      }
    }

  if (Fld)
    {
    EI.BlkId=cb;
    EI.Pg=CPg;
    EI.PageNo=CPgNo;
    EI.PgRowNo=RowNo;
    EI.BlkRowNo=RowNo-EI.Pg->Blk[cb].Row1;
    EI.Fld=Fld;
    EI.FieldId=Fld ? Fld->FieldId : -1;
    EI.Index=EI.Pg->Blk[cb].Index;
    EI.Ptr=EI.Pg->Blk[cb].Ptr;
    return true;
    }
  else
    {
    EI.BlkId=-1;
    return false;
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnInitialUpdate()
  {
  CView::OnInitialUpdate();
  //MFCData.hWnd=m_hWnd;
  LoadStrings();
  FixScrollSizes(true);
  }

//---------------------------------------------------------------------------

void FxdEdtView::DeleteWinControls()
  {
  ASSERT_RDB(bWinCtrlListBusy==0, "Would usually expect this to be zero");
  if (bWinCtrlListBusy==0)
    {
    bWinCtrlListBusy = 1;
    for (int i=0; i<nCtrls; i++)
      {
      FxdEdtFld * pFld=pCtrl[i].pFld;
      switch (pFld->m_iCtrlType)
        {
        case FXWC_BTN:
        case FXWC_CHECK:
        case FXWC_CHECKBTN:
          {
          CButton *pC=(CButton*)pCtrl[i].pCtrl;
          delete pC;
          pCtrl[i].pCtrl=NULL;
          break;
          }
        case FXWC_DDLST:
        case FXWC_DDLSTOPT:
        case FXWC_TREEND:
        case FXWC_BM1BTN:
        case FXWC_BM2BTN:
          {
          CBitmapButton *pC=(CBitmapButton*)pCtrl[i].pCtrl;
          delete pC;
          pCtrl[i].pCtrl=NULL;
          break;
          }
        }
      }
    DeleteCtrlList();
    bWinCtrlListBusy = 0;
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::PositionWinControls()
  {
  //ASSERT_RDB(bWinCtrlListBusy==0, "Would usually expect this to be zero");
  if (bWinCtrlListBusy==0)
    {
    bWinCtrlListBusy = 1;
    for (int i=0; i<nCtrls; i++)
      {
      FxdEdtFld * pFld=pCtrl[i].pFld;
      CRect Rect(pFld->Xs(ScrGB), pFld->Ys(ScrGB), pFld->Xe(ScrGB)+1, pFld->Ye(ScrGB)+1);

      if (pCtrl[i].pCtrl==NULL)
        {
        DWORD StyleMsk=0;
        switch (pCtrl[i].pFld->m_iCtrlType)
          {
          case FXWC_CHECK:
          case FXWC_CHECKBTN:
            {
            StyleMsk=BS_CHECKBOX;// | BS_LEFTTEXT;
            CButton *pC=new CButton();
            //Strng Titl(pFld->Str);
            //Titl.Trim();
            Rect.top+=2;
            Rect.bottom+=2;
            DWORD Flags=BS_NOTIFY | WS_CHILD | WS_TABSTOP | StyleMsk;
            //if (!fEditable)
            //  Flags|=WS_DISABLE;
            pC->Create(" ", Flags, Rect, this, FxdEdtCtrlID0+pFld->iCtrlIndex);
            pC->SetFont(ScrGB.pFont[0]);
            pC->SetCheck(pFld->Str.SafeAtoL() ? 1 :0);
            pC->EnableWindow(pFld->fEditable);
            pCtrl[i].pCtrl=pC;
            break;
            }
          case FXWC_BTN:
            {
            CButton *pC=new CButton();
            Strng Titl(pFld->Str);
            Titl.Trim();
            DWORD Flags=BS_NOTIFY | WS_CHILD | WS_TABSTOP | StyleMsk;
            //if (!fEditable)
            //  Flags|=WS_DISABLE;
            pC->Create(Titl(), Flags, Rect, this, FxdEdtCtrlID0+pFld->iCtrlIndex);
            pC->SetFont(ScrGB.pSmlFont[0]);
            pCtrl[i].pCtrl=pC;
            break;
            }
          case FXWC_BM1BTN:
          case FXWC_BM2BTN:
            {
            CBitmapButton *pC=new CBitmapButton();
            DWORD Flags=WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW;// | WS_BORDER;// | BS_NOTIFY;//BS_PUSHBUTTON;
            pC->Create("*", Flags, Rect, this, FxdEdtCtrlID0+pFld->iCtrlIndex);
            if (pCtrl[i].pFld->m_iCtrlType==FXWC_BM1BTN)
              pC->LoadBitmaps(MAKEINTRESOURCE(pFld->BigStrLength));
            else
              pC->LoadBitmaps(MAKEINTRESOURCE(pFld->BigStrLength), MAKEINTRESOURCE(pFld->BigStrLength+1));
            pC->SizeToContent();
            pCtrl[i].pCtrl=pC;
            break;
            }
          case FXWC_DDLST:
          case FXWC_DDLSTOPT:
            {
            CBitmapButton *pC=new CBitmapButton();
            DWORD Flags=WS_CHILD | BS_OWNERDRAW;
            //if (!fEditable)
            //  Flags|=WS_DISABLE;
            pC->Create("", Flags, Rect, this, FxdEdtCtrlID0+pFld->iCtrlIndex);
  //          pC->LoadBitmaps(MAKEINTRESOURCE(IDB_TESTBU), MAKEINTRESOURCE(IDB_TESTBD));
            pC->LoadBitmaps(MAKEINTRESOURCE(IDB_DDLST), MAKEINTRESOURCE(IDB_DDLSTDN));
            pC->SizeToContent();
            pCtrl[i].pCtrl=pC;
  //          pCtrl[i].pCtrl=NULL;
            break;
            }
          case FXWC_TREEND:
            {
            CBitmapButton *pC=new CBitmapButton();
            pC->Create("", WS_CHILD | BS_OWNERDRAW, Rect, this, FxdEdtCtrlID0+pFld->iCtrlIndex);
            pC->LoadBitmaps(MAKEINTRESOURCE(IDB_TREEPLUS), MAKEINTRESOURCE(IDB_TREEMINUS));
            pC->SizeToContent();
            pCtrl[i].pCtrl=pC;
            break;
            }
          }
        }

      CButton *pC=(CButton*)pCtrl[i].pCtrl;
      if (pC)
        {
        switch (pCtrl[i].pFld->m_iCtrlType)
          {
          case FXWC_BTN:
          case FXWC_CHECK:
          case FXWC_CHECKBTN:
          case FXWC_BM1BTN:
          case FXWC_BM2BTN:
            break;
          case FXWC_TREEND:
            {
            CBitmapButton* pCbm=(CBitmapButton*)pC;
            if (pCtrl[i].fPlus)
              pCbm->LoadBitmaps(MAKEINTRESOURCE(IDB_TREEPLUS), MAKEINTRESOURCE(IDB_TREEMINUS));
            else
              pCbm->LoadBitmaps(MAKEINTRESOURCE(IDB_TREEMINUS), MAKEINTRESOURCE(IDB_TREEPLUS));
            pCbm->SizeToContent();
            CRect CR;
            pC->GetClientRect(&CR);
            Rect.OffsetRect((Rect.Width()-CR.Width())/2, (Rect.Height()-CR.Height())/2);
            break;
            }
          case FXWC_DDLST:
          case FXWC_DDLSTOPT:
            Rect.left=pFld->Xe(ScrGB)+4;
            Rect.right=pFld->Xe(ScrGB)+16;
            Rect.top+=1;
            Rect.bottom+=1;
            break;
          }

        if (pFld->iPg==CPgNo &&
            (pFld->pRow->RowNo>=ScrGB.ChScroll.cy+CPg->FixedRows || pFld->fFixedPos))
  //        pC->SetWindowPos(NULL, Rect.left, Rect.top, 0,0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
            pC->SetWindowPos(&CWnd::wndBottom, Rect.left, Rect.top, 0,0, SWP_NOSIZE | SWP_SHOWWINDOW);
        else
          pC->ShowWindow(SW_HIDE);
        }
      }
    bWinCtrlListBusy = 0;
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::CalculateLayoutSize(rGDIBlk GB)
  {
  short EdMaxCols=0;
  short EdMaxRows=0;

  //FixedRows=0;

  for (int p=0; p<Pages; p++)
    {
    Pg[p]->TabWide=strlen(Pg[p]->TabName);
    EdMaxCols=Max(EdMaxCols, Max(Pg[p]->MaxCols, (short)(Pg[p]->TabWide+2)));
    EdMaxRows=Max(EdMaxRows, Pg[p]->Rows);
    }

  ChEditSize.cx=EdMaxCols;//(CR.Width()-Margin.cx-Extra.x)/ColWdt;
  if (GB.IsPrinting())
    {
    GB.LuMargin.cy=GB.RowHgt()*3;
    GB.LuMargin.cx=GB.ColWdt()*10;
    }
  else if (Pages>(fForce1Page ? 0 : 1)) // Always have Tabs
    {
    Pg[0]->TabPos=0;
    Pg[0]->TabRow=0;
    TabIndexRows=1;
    // Allow ChWorkWidth to work
    GB.LuMargin.cx=0;
    GB.LuBorder.cx=0;
    GB.LuMargin.cy=0;
    GB.LuMargin.cy=0;
    GB.ChDataSize.cx=10000;
    GB.ChDataSize.cy=10000;

    for (int p=1; p<Pages; p++)
      {
      Pg[p]->TabPos=Pg[p-1]->TabPos+Pg[p-1]->TabWide+1;
      Pg[p]->TabRow=Pg[p-1]->TabRow;
      if (Pg[p]->TabPos+Pg[p]->TabWide+2 > GB.ChWorkWidth(true))//ChEditSize.cx)
        {
        Pg[p]->TabPos=0;
        ++Pg[p]->TabRow;
        ++TabIndexRows;
        }
      }
    //TabIndexSpcY=TabIndexRows;//*RowHgt+1;
    //if (CPg)
    //  GB.LuMargin.cy=(TabIndexRows+1+CPg->FixedRows)*GB.RowHgt();
    //else
    GB.LuMargin.cy=(TabIndexRows+1)*GB.RowHgt();
    }
  else
    {
    TabIndexRows=0;
    //TabIndexSpcY=0;
    //GB.LuMargin.cy=0;
    //if (CPg)
    //  GB.LuMargin.cy=(CPg->FixedRows)*GB.RowHgt();
    //else
    GB.LuMargin.cy=0;
    }
  GB.LuBorder.cy=GB.RowHgt()/4;
  GB.LuBorder.cx=GB.ColWdt()/2;
  ChEditSize.cy=EdMaxRows;//(CR.Height()-Margin.cy-Extra.y)/RowHgt;
  GB.ChDataSize=ChEditSize;
  GB.LuDataSize.cx=GB.LuMargin.cx+2*GB.LuBorder.cx+GB.ColWdt()*ChEditSize.cx;
  GB.LuDataSize.cy=GB.LuMargin.cy+2*GB.LuBorder.cy+GB.RowHgt()*ChEditSize.cy;
  }

//---------------------------------------------------------------------------

void FxdEdtView::LoadStrings()
  {
  if (RebuildReqd)
    {
    bChanged=false;
    dwRebuildCnt++;
    DownButton=NULL;
    //Rows=0;
    DeleteWinControls();

    for (int i=0; i < MaxFixedPages; i++)
      {
      if (Pg[i])
        {
        delete Pg[i];
        Pg[i]=NULL;
        }
      }

    Pages=0;
    //int PrvPgNo=CPgNo;
    CPgNo=-1;
    CPg=NULL;

    StartBuild();
    Build(0,0,0);

    CalculateLayoutSize(ScrGB);
    ChangeLayout((int)ChEditSize.cy, (int)ChEditSize.cx);

    SelectPage(0, true);//Max(0,PrvPgNo));
    RebuildReqd=false;
    BuildDone();
    }

  for (int p=0; p<Pages; p++)
    Pg[p]->LoadStrings();

  //PositionWinColtrols();

  ReloadReqd=false;
  }

//---------------------------------------------------------------------------

void FxdEdtView::ParseStrings()
  {
  for (int p=0; p<Pages; p++)
    Pg[p]->ParseStrings();
  }

//---------------------------------------------------------------------------

void FxdEdtView::InitDC(CDC * pDC)
  {

  /**
  pDC->SetMapMode(MM_TEXT);

  int ChH = pDC->GetDeviceCaps(LOGPIXELSY)/8;
  int ChW = pDC->GetDeviceCaps(LOGPIXELSX)/10;

  //CFont Font;
  //Font.CreateFontIndirect(&mfc.LogFont);

  pDC->SelectObject(&Font);
  TEXTMETRIC tm;
  pDC->GetTextMetrics(&tm);

  CharSize.cx=tm.tmMaxCharWidth;
  CharSize.cy=tm.tmHeight;
  */

  pDC->SetMapMode(MM_TEXT);
  //int ChPixW = pDC->GetDeviceCaps(LOGPIXELSX)/10; // or something else
/**
  if (pDC->IsPrinting() || mfc.pFont==NULL)
    {
    CFont Font;
    TEXTMETRIC tm;
    mfc.LogFont.lfHeight = pDC->GetDeviceCaps(LOGPIXELSY)/10; // or something else
    Font.CreateFontIndirect(&mfc.LogFont);
    pDC->SelectObject(&Font);
    pDC->GetTextMetrics(&tm);
    mfc.CharSize.cx=tm.tmMaxCharWidth;
    mfc.CharSize.cy=tm.tmHeight;
    }

  pDC->SetMapMode(MM_ANISOTROPIC);
  pDC->SetWindowExt(ColWdt, RowHgt);
  pDC->SetViewportExt(mfc.CharSize.cx, mfc.CharSize.cy);
  if (pDC->IsPrinting())
    if (mfc.pFont!=NULL)
      {
      delete mfc.pFont;
      mfc.pFont=NULL;
      }

  if (mfc.pFont==NULL)
    {
    mfc.LogFont.lfHeight = RowHgt;
    mfc.pFont=new CFont();
    mfc.pFont->CreateFontIndirect(&mfc.LogFont);
    }


  if (pDC->IsPrinting())
    if (mfc.pTxtLightPen!=NULL)
      {
      delete mfc.pTxtLightPen;
      delete mfc.pTxtBackPen;
      delete mfc.pTxtDarkPen;
      delete mfc.pBorderPen;
      delete mfc.pBackBrush;
      delete mfc.pTxtLightBrush;
      mfc.pTxtLightPen   = NULL;
      }

  if (mfc.pTxtLightPen==NULL)
    {
    mfc.pTxtLightPen   = new CPen(PS_SOLID, 1, mfc.TxtColourLight);
    mfc.pTxtBackPen    = new CPen(PS_SOLID, 0, mfc.TxtColourBack);
    mfc.pTxtDarkPen    = new CPen(PS_SOLID, 1, mfc.TxtColourDark);
    mfc.pBorderPen     = new CPen(PS_SOLID,1,pDC->GetNearestColor(mfc.BordColour));
    mfc.pBackBrush     = new CBrush(mfc.BackColour);
    mfc.pTxtLightBrush = new CBrush(pDC->GetNearestColor(mfc.TxtColourLight));
    }

//  pDC->SetMapMode(MM_ANISOTROPIC);
//  pDC->SetWindowExt(ColWdt, RowHgt);
//  pDC->SetViewportExt(mfc.CharSize.cx, mfc.CharSize.cy);

  pDC->SelectObject(mfc.pFont);
  if (pDC->IsPrinting())
    {
    CRect rc(Margin.cx, Margin.cy+RowHgt, Margin.cx+ColWdt*CPg->MaxCols, Margin.cy+RowHgt*CPg->Rows);
    pDC->FillRect(&rc, mfc.pBackBrush);
    }

  TRACE2("INITDC %i  %i",RowHgt,mfc.CharSize.cy);
  TRACE2("       %i  %i",Margin.cy,Extra.y);
  TRACE2("       %i  %i",ScrollBy.cx,ScrollBy.cy);
*/
  //pDC->SelectObject(mfc.pBackBrush);
  /*
  pDC->MoveTo(Margin.cx, Margin.cy+RowHgt);
  pDC->LineTo(Margin.cx+ColWdt*CPg->MaxCols, Margin.cy+RowHgt*CPg->Rows);
  */
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnDraw(CDC* pDC)
  {
  CalculateLayoutSize(ScrGB);
  InitDC(pDC);
  ScrGB.Attach(pDC, NULL);
  DoOnDraw(ScrGB);
  ScrGB.Detach();
  }

//---------------------------------------------------------------------------

void FxdEdtView::DoOnDraw(rGDIBlk GB)
  {
  //ASSERT(MFCData.pDC==NULL);
  /*
  TRACE("OnDraw %i ---------",CPgNo);
  TRACE2("   Margin    %4i %4i",GB.LuMargin.cx,GB.LuMargin.cy);
  TRACE2("   Scroll    %4i %4i",GB.ChScroll.cx,GB.ChScroll.cy);
  TRACE2("   ScrollBy  %4i %4i",GB.ChScrollBy.cx,GB.ChScrollBy.cy);
  */
  //MFCData.pDC=pDC;
  // TODO: add draw code here
  if (!RebuildReqd)
    {
    int Err;
    CRgn   ClipRgn;
    CRect ClipRect;

    Err=GB.DC().SelectClipRgn(NULL);

    ClipRect=GB.LuClipRect(VScrlOn, HScrlOn);
    if (CPg)
      ClipRect.top+=(CPg->FixedRows)*GB.RowHgt()+GB.LuBorder.cy;

    ClipRgn.CreateRectRgnIndirect(&ClipRect);

    OnDrawBack(GB, CPgNo, ClipRgn);

    Err=GB.DC().SelectClipRgn(NULL);

    if (CPg)
      for (int i=0;i<Min(CPg->FixedRows, CPg->Rows); i++)
        OnDrawLine(GB, CPgNo, i);

    Err=GB.DC().SelectClipRgn(&ClipRgn);

    if (CPg)
      for (int i=CPg->FixedRows;i<CPg->Rows;i++)
        OnDrawLine(GB, CPgNo, i);

    OnOverDraw(GB, CPgNo, ClipRgn);

    Err=GB.DC().SelectClipRgn(NULL);
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::FixCaret()
  {
  FxdEdtFld * pFld=CurrentFld();

//  if (!CaretOK)
//    {
    CaretOK=1;
    HideCaret();
    DestroyCaret();
    if (pFld==NULL || !pFld->fEditable)
      ::CreateCaret(m_hWnd, (HBITMAP)0, 1,ScrGB.RowHgt());//-2);
    else if (fInsertMode)
      //CreateSolidCaret(2,ScrGB.RowHgt());
      ::CreateCaret(m_hWnd, (HBITMAP)0, 2,ScrGB.RowHgt());//-2);
    else
      ::CreateCaret(m_hWnd, (HBITMAP)1, ScrGB.ColWdt(),ScrGB.RowHgt());//-2);
    ShowCaret();
//    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::PositionCaret(byte Options)//flag fRedraw, flag ClearSelected, flag SetSelected)
  {
  //TRACE2("PC Scroll    %4i %4i",ScrGB.ChScroll.cx,ScrGB.ChScroll.cy);
  //TRACE2("PC  ScrollBy  %4i %4i",ScrGB.ChScrollBy.cx,ScrGB.ChScrollBy.cy);
  //TRACE2("   EditPos   %4i %4i",ChEditPos.x,ChEditPos.y);

  CSize ChDiff, SPt=ScrGB.ChScroll;
  if (ScrGB.ChScrollBy.cx!=0 || ScrGB.ChScrollBy.cy!=0)
    {
    ScrGB.ChScroll.cx+=ScrGB.ChScrollBy.cx;
    ScrGB.ChScroll.cy+=ScrGB.ChScrollBy.cy;
    //ChEditPos.y+=ScrGB.ChScrollBy.cy;///RowHgt;
    //ChEditPos.x+=ScrGB.ChScrollBy.cx;///ColWdt;
    }
  else
    {
    CRect VR=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
    if (ChEditPos.x<VR.left)
      ScrGB.ChScroll.cx+=ChEditPos.x-VR.left;
    else if (ChEditPos.x>VR.right)
      ScrGB.ChScroll.cx+=ChEditPos.x-VR.right;

    if (ChEditPos.y<VR.top)
      ScrGB.ChScroll.cy+=ChEditPos.y-VR.top;
    else if (ChEditPos.y>VR.bottom)
      ScrGB.ChScroll.cy+=ChEditPos.y-VR.bottom;
    }

  if (CPg)
    {
    CRect VR=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
    ScrGB.ChScroll.cx=Min((int)ScrGB.ChScroll.cx, (CPg->MaxCols-(ScrGB.ChWorkWidth(VScrlOn))));
    ScrGB.ChScroll.cy=Min((int)ScrGB.ChScroll.cy, (CPg->Rows-(ScrGB.ChWorkHeight(HScrlOn)-TabIndexRows-1)));
    ScrGB.ChScroll.cx=Max(0,(int)ScrGB.ChScroll.cx);
    ScrGB.ChScroll.cy=Max(0,(int)ScrGB.ChScroll.cy);
    }
  else
    {
    ScrGB.ChScroll.cx=0;
    ScrGB.ChScroll.cy=0;
    }

  CRect VR=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
  ChEditPos.x=Range(VR.left, ChEditPos.x, VR.right);
  ChEditPos.y=Range(VR.top,  ChEditPos.y, VR.bottom);

  CPoint LuCaretPt;
  LuCaretPt=ScrGB.LuPt(ChEditPos);
  LuCaretPt.x+=1;

  SetCaretPos(LuCaretPt);
  ScrGB.ChScrollBy.cx=0;
  ScrGB.ChScrollBy.cy=0;

  FixScrollSizes(false);

  ChDiff.cx=ScrGB.ChScroll.cx-SPt.cx;
  ChDiff.cy=ScrGB.ChScroll.cy-SPt.cy;
  if (ChDiff.cx !=0 || ChDiff.cy!=0)
    RedrawNeeded=1;
  
  if (Options & PC_ClearIfNotSlcted)
    {
    FxdEdtFld * Fld=CurrentFld();//EdtFld();
    if (Fld && !Fld->fSelected)
      Options |= PC_ClearSlct;
    }
  
  if ((Options & PC_ClearSlct) && CPg)
    {
    for (int r=0; r<CPg->Rows; r++)
      {
      for (int f=0; f<CPg->Row[r]->NFields; f++)
        {
        FxdEdtFld * Fld=CPg->Row[r]->Field[f];
        if (Fld->fSelected)
          Fld->Invalidate(ScrGB);
        Fld->fSelected=false;
        }
      }
    }
  
  if (Options & PC_SetSlct)
    {
    FxdEdtFld * Fld=CurrentFld();//EdtFld();
    if (Fld)
      {
      Fld->fSelected=Fld->fSelectable;
      Fld->Invalidate(ScrGB);
      }
    }

  if ((Options & PC_Redraw) && RedrawNeeded)
    {
    Invalidate();
    RedrawNeeded=0;
    }
  }


//---------------------------------------------------------------------------

void FxdEdtView::PositionCaretAt(int ColNo, int RowNo)
  {
  FxdEdtFld * Fld=EdtFld();
  StartCharEdit(Fld);
  ChEditPos.x=ColNo;
  ChEditPos.y=RowNo;
  EndCharEdit(Fld);
  FixCaret();
  PositionCaret(PC_Redraw | PC_ClearSlct | PC_SetSlct);
  }

//---------------------------------------------------------------------------

void FxdEdtView::PositionScrollBarsAt(int X, int Y)
  {
  HScrlOn=(X>=0);
  ScrGB.ChScroll.cx=X;
  VScrlOn=(Y>=0);
  ScrGB.ChScroll.cy=Y;
  FixScrollSizes(true);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnSetFocus(CWnd* pOldWnd)
  {
  CView::OnSetFocus(pOldWnd);

  //if (pScrGB)
  CaretOK=0;
  FixCaret();
  PositionCaret(PC_Redraw | PC_NoClearSlct | PC_SetSlct);

  if (pEdtControl)
    pEdtControl->DestroyWindow();
  if (pCBoxControl)
    pCBoxControl->DestroyWindow();
  //if (pEdtSpin)
  //  pEdtSpin->DestroyWindow();
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnKillFocus(CWnd* pNewWnd)
  {
  /*
  FxdEdtFld * Fld=EdtFld();
  if (Fld)
    {
    Fld->AcceptChanges=1;
    EndCharEdit(Fld);
    }
  */

  CView::OnKillFocus(pNewWnd);

  HideCaret();
  DestroyCaret();
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnTimer(UINT nIDEvent)
  {
  };

//---------------------------------------------------------------------------

void FxdEdtView::OnUpdate(CView*pSender, LPARAM lHint, CObject* pHint)
  {
  #if dbgOnUpdate
  dbgpln("OnUpdate %08x",lHint);
  #endif
  if (lHint==0)
    Invalidate();
  else
    for (int r=0; r<CPg->Rows; r++)
      if (CPg->Row[r]->Visible(ScrGB))
        {
        #if dbgOnUpdate
        dbgp("  %3i]",r);
        #endif
        for (int f=0; f<CPg->Row[r]->NFields; f++)
          {
          FxdEdtFld * Fld=CPg->Row[r]->Field[f];
          if (!Fld->fEditing)
            {
            if (lHint & Fld->FieldType)
              {
                //if (r>=RowShift+1)
               //if (Doc().DS[r-RowShift-1].MeasChanged())
          //    if (Fld->Changed())
              CPg->CurBlkId=0;
              FxdEdtInfo EI(this, CPg, Fld, r);
//cnm              if ((Fld->FieldId >=0) && (Fld->Changed || Changed(EI)))
              if ((Fld->FieldId >=0) && (Fld->Changed))
                {
                #if dbgOnUpdate
                dbgp("(%i %i)",f,Fld->FieldId);
                #endif
                //  Doc().DS[r-RowShift-1].ClearChanged();
                CPg->CurBlkId=0;
                FxdEdtInfo EI1(this, CPg, Fld, r);
//cnm                ClearChanged(EI1);//r,Fld->FieldId);

                //TmpFldBuff Buff(Fld->Str);

                CPg->CurBlkId=0;
                FxdEdtInfo EI2(this, CPg, Fld, r);
                Load(EI2, Fld->Str);//Buff.Str, Fld->Len);
                Fld->FixAfterLoad(Fld->Str);
                Fld->Invalidate(ScrGB);
                Fld->Changed=0;
                }
              #if dbgOnUpdate
              dbgp("%s ",Fld->Str);
              #endif
              }
            }
          }
        #if dbgOnUpdate
        dbgpln("");
        #endif
        }
  }

//---------------------------------------------------------------------------

void FxdEdtView::StartCharEdit(FxdEdtFld * Fld)
  {
  FldEdit=Fld;
  WasEditing=(Fld ? Fld->fEditing : 0);
  }

//---------------------------------------------------------------------------

void FxdEdtView::EndCharEdit(FxdEdtFld * Fld)
  {
  FxdEdtFld * FldE=EdtFld();
  if (Fld)
    if (FldE != Fld || Fld->AcceptChanges)
      {
      if (Fld->fEditing || Fld->AcceptChanges)
        {
        Fld->fEditing=0;
        Fld->Changed=1;
        Fld->AcceptChanges=0;
        if (GetDocument())
          GetDocument()->SetModifiedFlag(true);
        Fld->FixBeforeParse();
        if (CPg)
          {
          CPg->CurBlkId=0;
          FxdEdtInfo EI(this, CPg, Fld, Fld->pRow->RowNo);
          DWORD RebuildCnt=dwRebuildCnt;
          Parse(EI, Fld->Str);
          if (!RebuildReqd && !ReloadReqd && (RebuildCnt==dwRebuildCnt))
            {
            CPg->CurBlkId=0;
            //TmpFldBuff Buff(Fld->Str);
            Load(EI, Fld->Str);//Buff.Str, Fld->Len);
            Fld->FixAfterLoad(Fld->Str);//Buff);
            Fld->Invalidate(ScrGB);
            }
          }
        }
      }
    else if (WasEditing || Fld->fEditing)
      {
      Fld->Invalidate(ScrGB);
      }
  if (RebuildReqd || ReloadReqd)
    {
    LoadStrings();
    Invalidate();
    FixScrollSizes(false);
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::FindNextEdtFld(BOOL Editable)
  {
  FxdEdtFld * StartFld=EdtFld();
  CPoint StartChEditPos= ChEditPos;
  ChEditPos.y=Range(0L, ChEditPos.y, CPg->Rows-1L);
  StartChEditPos.x=Min(StartChEditPos.x, Min(CPg->Row[ChEditPos.y]->Cols-1L, CPg->Rows-1L));
  do
    {
    ChEditPos.x++;
    if (ChEditPos.x>=CPg->Row[ChEditPos.y]->Cols)
      {
      ChEditPos.y++;
      if (ChEditPos.y>=CPg->Rows)
        ChEditPos.y=0;
      ChEditPos.x=0;
      }
    FxdEdtFld * Fld=EdtFld();
    if (Fld)
      if (Fld!=StartFld && Fld->fEditable && Editable || !Editable)
        break;
    }
  while (StartChEditPos!=ChEditPos);
  }
 
//---------------------------------------------------------------------------

void FxdEdtView::FindPrevEdtFld(BOOL Editable)
  {
  //TODO Improve Shift-Tab to place cursor at begining of field
  FxdEdtFld * StartFld=EdtFld();
  CPoint StartChEditPos= ChEditPos;
  StartChEditPos.x=Min(StartChEditPos.x, CPg->Rows-1L);
  do
    {
    ChEditPos.x--;
    if (ChEditPos.x<0)
      {
      ChEditPos.y--;
      if (ChEditPos.y<0)
        ChEditPos.y=CPg->Rows-1;
      ChEditPos.x=CPg->Row[ChEditPos.y]->Cols-1;
      }
    FxdEdtFld * Fld=EdtFld();
    if (Fld)
      if (Fld!=StartFld && Fld->fEditable && Editable || !Editable)
        break;
    }
  while (StartChEditPos!=ChEditPos);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if (CPg)
    {
    FxdEdtFld * Fld=EdtFld();
    StartCharEdit(Fld);
    flag Reposition=true;

    if (!DoKeyDown(nChar, nRepCnt, nFlags))
      {
      switch (nChar)
        {
        case VK_LEFT    : ChEditPos.x--; break;
        case VK_RIGHT   : ChEditPos.x++; break;
        case VK_UP      : ChEditPos.y--; break;
        case VK_DOWN    : ChEditPos.y++; break;
        case VK_RETURN  : if (Fld && Fld->CtrlType()==FXWC_DDLST)
                            {
                            SetCBoxControl(Fld, this, 0, false);
                            pCBoxControl->SendMessage(WM_KEYDOWN, nChar, 1 | nFlags<<16);
                            }
                          break;
        case VK_SHIFT   : bShiftDown = 1; Reposition=false; break;
        case VK_CONTROL : bControlDown = 1; Reposition=false; break;
        case VK_PRIOR   :
        case VK_NEXT    : {
                          int PgL=(ScrGB.ChWorkHeight(HScrlOn)-TabIndexRows-1);
                          ScrGB.ChScrollBy.cy= (nChar==VK_NEXT) ? PgL : -PgL;
                          }
                          break;
        case VK_END     : if (Fld)
                            ChEditPos.x=Fld->ColN;
                          break;
        case VK_HOME    : if (Fld)
                            ChEditPos.x=Fld->Col1;
                          break;
        case VK_INSERT  : fInsertMode=!fInsertMode; CaretOK=0; break;
        case VK_DELETE  : if (Fld)
                            {
                            if (Fld->CtrlType()==FXWC_DDLST)
                              {
                              SetCBoxControl(Fld, this, 0, false);
                              pCBoxControl->SendMessage(WM_KEYDOWN, nChar, 1 | nFlags<<16);
                              }
                            else
                              {
                              SetEditControl(Fld, this, 0, (ChEditPos.x - Fld->Col1), true);
                              pEdtControl->SendMessage(WM_KEYDOWN, nChar, 1 | nFlags<<16);
                              }
                            }
                          break;
        case VK_F4      : if (Fld && Fld->fEditable)
                            {
                            flag DoTogl=true;
                            Strng Tmp(Fld->Str);
                            Tmp.LRTrim();
                            if ((Tmp.Length()==1) || ((Tmp.Length()>1) && (Tmp[1]=='-')))
                              if (Tmp[0]=='0')
                                {
                                Fld->Str="1";
                                Fld->AcceptChanges=true;
                                }
                              else if (Tmp[0]=='1')
                                {
                                Fld->Str="0";
                                Fld->AcceptChanges=true;
                                }
                            }
                          break;
        default         : Reposition=false; break;
        }
      }
    EndCharEdit(Fld);
    if (Reposition)
      {
      FixCaret();
      PositionCaret(PC_Redraw | (bShiftDown ? PC_NoClearSlct : PC_ClearSlct)| PC_SetSlct);
      }
    }
  CView::OnKeyDown(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  switch (nChar)
    {
    case VK_SHIFT: bShiftDown = 0; break;
    case VK_CONTROL: bControlDown = 0; break;
    default: break;
    }
  CView::OnKeyUp(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if (CPg)
    {
    FxdEdtFld * Fld=EdtFld();
    StartCharEdit(Fld);
    switch (nChar)
      {
      case 0x08 : // BS
        ChEditPos.x--;
        break;
      case 0x0a : // LF
      case 0x0d : // CR
        if (Fld)
          {
          Fld->AcceptChanges=Fld->fEditing;
          ChEditPos.x=Fld->Col1;
          }
        break;
      case 0x09 : //  Tab
        {  // Tab
        if (bShiftDown)
          FindPrevEdtFld();
        else
          FindNextEdtFld();
        break;
        }
      default :  // Displayable
        if (nChar >=' ')
          {
//          if (!Fld)
//            {
//            FindNextEdtFld();
//            Fld=EdtFld();
//            }
          if (Fld && Fld->fEditable)
            {
            if (Fld->m_iCtrlType==FXWC_DDLST)
              SetCBoxControl(Fld, this, nChar, true);
            else if (Fld->m_iCtrlType==FXWC_CHECK || Fld->m_iCtrlType==FXWC_CHECKBTN)
              {
              flag DoTogl=true;
              Strng Tmp(Fld->Str);
              Tmp.LRTrim();
              if (Tmp.Length()==1)
                if (Tmp[0]=='0')
                  {
                  Fld->Str="1";
                  Fld->AcceptChanges=true;
                  }
                else if (Tmp[0]=='1')
                  {
                  Fld->Str="0";
                  Fld->AcceptChanges=true;
                  }
              ((CButton*)pCtrl[Fld->iCtrlIndex].pCtrl)->SetCheck(Fld->Str.SafeAtoL() ? 1 :0);
              }
            else if ((Fld->m_iCtrlType==FXWC_DDLSTOPT) && Fld->HasNANStrings && isalpha(nChar))
              SetCBoxControl(Fld, this, nChar, true);
            else
              SetEditControl(Fld, this, nChar, (ChEditPos.x - Fld->Col1), true);
            Fld->fEditing=1;
            }
          break;
          }
      }
    EndCharEdit(Fld);
    }
  CView::OnChar(nChar, nRepCnt, nFlags);
  }


//---------------------------------------------------------------------------

/*KGA 23/4/96: this function does NOT work for release version with optimisations!!!
CPoint& FxdEdtView::Point2RowCols(rGDIBlk GB, CPoint point)
  {
  CPoint RP;
  RP.x=(point.x-GB.LuMargin.cx-GB.LuBorder.cx)/GB.ColWdt()+GB.ChScroll.cx;
  RP.y=(point.y-GB.LuMargin.cy-GB.LuBorder.cy)/GB.RowHgt()+GB.ChScroll.cy;
  return RP;
  }*/

CPoint& FxdEdtView::Point2RowCols(rGDIBlk GB, CPoint point, CPoint& RP)
  {
  RP.x=(point.x-GB.LuMargin.cx-GB.LuBorder.cx)/GB.ColWdt()+GB.ChScroll.cx;
  RP.y=(point.y-GB.LuMargin.cy-GB.LuBorder.cy)/GB.RowHgt()+GB.ChScroll.cy;
  return RP;
  }

//---------------------------------------------------------------------------

void FxdEdtView::SetEditPos(CPoint point)
  {
  CPoint RP;
  Point2RowCols(ScrGB, point, RP);
//  ChEditPos.x=Range((int)0, (int)(RP.x), (int)(CPg->MaxCols-1));
//  ChEditPos.y=Range((int)0, (int)(RP.y), (int)(CPg->Rows-1));
  ChEditPos.x=Range((int)0, (int)(RP.x), (int)(CPg->MaxCols+1));
  ChEditPos.y=Range((int)0, (int)(RP.y), (int)(CPg->Rows+1));
  }

//---------------------------------------------------------------------------

void FxdEdtView::ClrCurrentEdit(BOOL Cancel)
  {//KGA 6/10/98
  FxdEdtFld * pEdtFld=EdtFld();
  if (pEdtFld)
    {
    pEdtFld->AcceptChanges = (Cancel ? 0 : pEdtFld->fEditing);
    ClrEditControl();
    ClrCBoxControl();
    FixCaret();
    }
  }

//---------------------------------------------------------------------------

flag MDIActivateThis(CWnd* This, int Deep)
  {
  CMDIChildWnd *p=(CMDIChildWnd*)This->GetParent();
  while (p && Deep-->0)
    {
    if (p->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
      {
      p->MDIActivate();
      return 1;
      }
    else
      p=(CMDIChildWnd*)p->GetParent();
    }
  return 0;
  }

//---------------------------------------------------------------------------
static int GotLButtonDn=0;

void FxdEdtView::OnLButtonDown(UINT nFlags, CPoint point)
  {
  GotLButtonDn=true;

  MDIActivateThis(this);
  SetFocus();

  CClientDC DC(this);
  CDCResChk ResChk(DC);

  ScrGB.Attach(&DC, NULL);
  if (CPg)
    {
    int X,Y;
    CPoint OldChEditPos=ChEditPos;
    FxdEdtFld * OldEdtFld=EdtFld();
    CPoint MP;//=Point2RowCols(ScrGB, point);
    //MP.x=(point.x-GB.LuMargin.cx-GB.LuBorder.cx)/GB.ColWdt();
    Y=(point.y-(int)(ScrGB.LuMargin.cy+ScrGB.LuBorder.cy));
    MP.y=Y/ScrGB.RowHgt()+(Y<0?-1:0);
    if (MP.y<0)
      {
      FxdEdtFld * pEdtFld=EdtFld();
      if(pEdtFld)
        {
        pEdtFld->AcceptChanges=pEdtFld->fEditing;
        EndCharEdit(pEdtFld);
        FixCaret();
        }
//      point.y-=ScrGB.RowHgt()/3;
      //MP=Point2RowCols(ScrGB, point);
      X=(point.x-(int)(ScrGB.LuMargin.cx+ScrGB.LuBorder.cx));
      MP.x=X/ScrGB.ColWdt()+(X<0?-1:0);
      Y=(point.y-(int)(ScrGB.LuMargin.cy+ScrGB.LuBorder.cy));
      MP.y=Y/ScrGB.RowHgt()+(Y<0?-1:0);
      int r=-(MP.y+2);
      int c=MP.x;
      //TRACE2("LB %3i %3i",MP.y,r);
      //#ifdef RotateTabs
      //r=(r-CPg->TabRow+TabIndexRows)%TabIndexRows;
      //#endif

      for (int p=0; p<Pages; p++)
        if ((Pg[p]->TabRow==r) &&
            (Pg[p]->TabPos<=c) &&
            (Pg[p]->TabPos+1+Pg[p]->TabWide>c))
          {
          SelectPage(p);
          ChEditPos=OldChEditPos;
          break;
          }
      PositionCaret(PC_Redraw | (bShiftDown ? PC_NoClearSlct : PC_ClearSlct)| PC_SetSlct);
      }
    else
      {
      SetEditPos(point);
      EndCharEdit(OldEdtFld);
      FixCaret();
      DownButton=ButtonFld();
      if (DownButton)
        if (DownButton->IsButton && DownButton->fEditable)
          {
          CPg->CurBlkId=0;
          FxdEdtInfo EIB(this, CPg, DownButton, DownButton->pRow->RowNo);
          ButtonPushed(EIB, DownButton->Str);//, DownButton->Len);

          CPg->CurBlkId=0;
          FxdEdtInfo EIL(this, CPg, DownButton, DownButton->pRow->RowNo);
          //TmpFldBuff Buff(DownButton->Str);
          Load(EIL, DownButton->Str);//Buff.Str, DownButton->Len);

          DownButton->FixAfterLoad(DownButton->Str);
          DownButton->ButtonDown=1;
          //DownButton->Invalidate();
          //CRect DummyRect(0,0,1,1);
          //RedrawWindow(&DummyRect);
          ChEditPos=OldChEditPos;
          }
      PositionCaret(PC_Redraw | (bShiftDown ? PC_NoClearSlct : PC_ClearSlct)| PC_SetSlct);

      if ((nFlags & MK_CONTROL)==0)
        {
        for (int r=0; r<CPg->Rows; r++)
          for (int f=0; f<CPg->Row[r]->NFields; f++)
            {
            FxdEdtFld * Fld=CPg->Row[r]->Field[f];
            if (Fld->fSelected)
              {
              Fld->fSelected=false;
              Fld->Invalidate(ScrGB);
              }
            }
//        Invalidate();
        }

      FxdEdtFld * Fld=CurrentFld();//EdtFld();
      if (Fld)
        {
        bool WasSelected=Fld->fSelected;
        Fld->fSelected=Fld->fSelectable;
        Fld->Invalidate(ScrGB);

        if (WasSelected && (Fld->iCtrlIndex>=0))
          {
          if (Fld->m_iCtrlType==FXWC_DDLST)
            SetCBoxControl(Fld, this, 0, true);
          else
            PostMessage(WM_COMMAND, MAKEWPARAM(FxdEdtCtrlID0+Fld->iCtrlIndex, BN_CLICKED), 0);
          }
        }
      }

    if (RebuildReqd || ReloadReqd)
      {
      LoadStrings();
      Invalidate();
      //FixScrollSizes();
      }
    }

  if (DownButton==NULL)
    DoLButtonDown(nFlags, point);
  ScrGB.Detach();
  CView::OnLButtonDown(nFlags, point);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnLButtonUp(UINT nFlags, CPoint point)
  {
//  if (!GotLButtonDn)
//    return;
  GotLButtonDn=false;
  CClientDC DC(this);
  CDCResChk ResChk(DC);

  ScrGB.Attach(&DC, NULL);

  if (0 && CPg)
    {
    int Y;
    CPoint OldChEditPos=ChEditPos;
    FxdEdtFld * OldEdtFld=EdtFld();
    CPoint MP;//=Point2RowCols(ScrGB, point);
    //MP.x=(point.x-GB.LuMargin.cx-GB.LuBorder.cx)/GB.ColWdt();
    Y=(point.y-(int)(ScrGB.LuMargin.cy+ScrGB.LuBorder.cy));
    MP.y=Y/ScrGB.RowHgt()+(Y<0?-1:0);
    if (MP.y>=0)
      {
      SetEditPos(point);
      EndCharEdit(OldEdtFld);
      FixCaret();
      PositionCaret(PC_Redraw | (bShiftDown ? PC_NoClearSlct : PC_ClearSlct)| PC_SetSlct);
      }
    }
  
  //if (DownButton==NULL)
    DoLButtonUp(nFlags, point);
  //else
  if (CPg && DownButton)
    {
    DownButton->ButtonDown=0;
    DownButton->Invalidate(ScrGB);
    DownButton=NULL;
    }

  if (RebuildReqd || ReloadReqd)
    {
    LoadStrings();
    Invalidate();
    //FixScrollSizes();
    }

  ScrGB.Detach();
  CView::OnLButtonUp(nFlags, point);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnLButtonDblClk(UINT nFlags, CPoint point)
  {
  CClientDC DC(this);
  CDCResChk ResChk(DC);

  ScrGB.Attach(&DC, NULL);
  SetFocus();

  FxdEdtFld * Fld=NULL;
  if (CPg)
    {
    FixCaret();
    SetEditPos(point);
    PositionCaret(PC_Redraw | PC_ClearSlct | PC_SetSlct);;
    Fld=EdtFld();
    if (Fld)
      {
      if (Fld->m_iCtrlType==FXWC_DDLST)
        {
        SetCBoxControl(Fld, this, 0, true);
        }
      else
        {
        bool DoSetEdit=true;
        bool AllowToggle=(nFlags & MK_CONTROL)!=0;
        if (AllowToggle && Fld->fEditable)
          {
          Strng Tmp(Fld->Str);
          Tmp.LRTrim();
          if ((Tmp.Length()==1) || ((Tmp.Length()>1) && (Tmp[1]=='-')))
            {
            if (Tmp[0]=='0')
              {
              Fld->Str="1";
              Fld->AcceptChanges=true;
              EndCharEdit(Fld);
              DoSetEdit=false;
              }
            else if (Tmp[0]=='1')
              {
              Fld->Str="0";
              Fld->AcceptChanges=true;
              EndCharEdit(Fld);
              DoSetEdit=false;
              }
            }
          }
        if (DoSetEdit)
          SetEditControl(Fld, this, 0, (ChEditPos.x - Fld->Col1), false);
        }
      }
    }

  if (!Fld)
    DoLButtonDblClk(nFlags, point);

  if (RebuildReqd || ReloadReqd)
    {
    LoadStrings();
    Invalidate();
    //FixScrollSizes();
    }

  ScrGB.Detach();
  CView::OnLButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnSize(UINT nType, int cx, int cy)
  {
  CView::OnSize(nType, cx, cy);
  CalculateLayoutSize(ScrGB);
  ChangeLayout((int)ChEditSize.cy, (int)ChEditSize.cx);

  if (CPgNo>=0)
    SelectPage(CPgNo);
  FixScrollSizes(true);
  }

//---------------------------------------------------------------------------

void FxdEdtView::FixScrollSizes(flag fShowHideBars)
  {
  if (CPg)
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
    //??
    //int sbs=ScrGB.GBC().ScrollBarSize;
    int vsbs=ScrGB.VScrlWidth();//sbs>0 ? ScrGB.ColWdt()*sbs/10 : GetSystemMetrics(SM_CXVSCROLL);
    int hsbs=ScrGB.HScrlHeight();//sbs>0 ? ScrGB.ColWdt()*sbs/10 : GetSystemMetrics(SM_CXHSCROLL);
    CRect Vr=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);

    if (fShowHideBars)
      {
      flag OldVScrlOn=VScrlOn;
      flag OldHScrlOn=HScrlOn;
      //VScrlOn=(Vr.top>0) || (ScrGB.ChWorkHeight(false))<=CPg->Rows;
      //HScrlOn=(Vr.left>0) || (ScrGB.ChWorkWidth(false))<=CPg->MaxCols;
      flag HOn, VOn;
      HScrlOn=false;
      VScrlOn=false;
      do
        {
        HOn=HScrlOn;
        VOn=VScrlOn;
        VScrlOn=VScrlOn || (ScrGB.ChWorkHeight(HScrlOn))< CPg->Rows-TabIndexRows;
        HScrlOn=HScrlOn || (ScrGB.ChWorkWidth(VScrlOn))< CPg->MaxCols;
        }
      while (HOn!=HScrlOn || VOn!=VScrlOn);

      if (OldVScrlOn!=VScrlOn)
        {
        VScrl->ShowScrollBar(VScrlOn);
        if (!VScrlOn)
          ScrGB.ChScrollBy.cy=0;
        }
      if (OldHScrlOn!=HScrlOn)
        {
        HScrl->ShowScrollBar(HScrlOn);
        if (!HScrlOn)
          ScrGB.ChScrollBy.cx=0;
        }

      r=Cr;
      r.InflateRect(1,1);
      //++r.bottom;
      //++r.right;
      r.left=r.right-vsbs;
      if (HScrlOn)
        r.bottom=r.bottom-hsbs;
      VScrl->MoveWindow(&r, false);
      r=Cr;
      r.InflateRect(1,1);//--r.left;
      //++r.right;
      //++r.bottom;
      if (VScrlOn)
        r.right=r.right-hsbs;
      r.top=r.bottom-vsbs;
      HScrl->MoveWindow(&r, false);
      }

    SCROLLINFO SCI;

    int PgL=CPg->Rows-TabIndexRows-1;
    //int PgL=CPg->Rows-ScrGB.ChWorkHeight(HScrlOn)-TabIndexRows-1;
    ScrGB.ChScroll.cy=Range(0, (int)ScrGB.ChScroll.cy, PgL);

    SCI.cbSize=sizeof(SCI);
    SCI.fMask=SIF_ALL;
    SCI.nMin=0;
    SCI.nMax=Max(1, PgL);//(int)(CPg->Rows));//-(ScrGB.ChWorkHeight(HScrlOn))));
    SCI.nPage=ScrGB.ChWorkHeight(HScrlOn);
    SCI.nPos=ScrGB.ChScroll.cy;
    SCI.nTrackPos=0;
    VScrl->SetScrollInfo(&SCI, true);


    //VScrl->SetScrollRange(0, Max(1, (int)(CPg->Rows-(ScrGB.ChWorkHeight(HScrlOn)/*-TabIndexRows-1*/))), false);
    //VScrl->SetScrollPos(ScrGB.ChScroll.cy, true);

    SCI.cbSize=sizeof(SCI);
    SCI.fMask=SIF_ALL;
    SCI.nMin=0;
    SCI.nMax=Max(1, (int)(CPg->MaxCols));//-(ScrGB.ChWorkHeight(HScrlOn))));
    SCI.nPage=ScrGB.ChWorkWidth(VScrlOn);
    //SCI.nPos=ScrGB.ChScroll.cx;
    SCI.nPos=Range((int)0, (int)ScrGB.ChScroll.cx, (int)(CPg->MaxCols-SCI.nPage));
    SCI.nTrackPos=0;
    HScrl->SetScrollInfo(&SCI, true);
    //HScrl->SetScrollRange(0, Max(1, (int)(CPg->MaxCols-(ScrGB.ChWorkWidth(VScrlOn)))), false);
    //HScrl->SetScrollPos(ScrGB.ChScroll.cx, true);

    PositionWinControls();
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  if (CPg)
    {
    int sp=VScrl->GetScrollPos();
//    TRACE1("VScroll %i\n",sp);
    CRect Vr=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
    int PgL=(ScrGB.ChWorkHeight(HScrlOn)-TabIndexRows-1);
    /*
    int nMapMode;
    SIZE sizeTotal, sizePage, sizeLine;
    GetDeviceScrollSizes(nMapMode, sizeTotal, sizePage, sizeLine);
    */
    if (!IgnoreOneScroll)
      {
      switch (nSBCode)
        {
        case SB_BOTTOM        : ScrGB.ChScrollBy.cy=-ScrGB.ChScroll.cy; break;
        case SB_LINEDOWN      : ScrGB.ChScrollBy.cy+=1; break;
        case SB_LINEUP        : ScrGB.ChScrollBy.cy-=1; break;
        case SB_PAGEDOWN      : ScrGB.ChScrollBy.cy+=PgL; break;
        case SB_PAGEUP        : ScrGB.ChScrollBy.cy-=PgL; break;
        case SB_THUMBPOSITION :  // Scroll to the absolute position. The current position is provided in nPos.
        case SB_THUMBTRACK    : ScrGB.ChScrollBy.cy=(nPos-sp); break; // Drag scroll box to specified position. The current position is provided in nPos.
        case SB_TOP           : ScrGB.ChScrollBy.cy=-ScrGB.ChScroll.cy; break;
        case SB_ENDSCROLL     : break;
        }

      FixScrollSizes(false);
      PositionCaret(PC_Redraw | PC_NoClearSlct | PC_NoSetSlct);;//nSBCode==SB_ENDSCROLL || nSBCode==SB_THUMBTRACK || nSBCode==SB_THUMBPOSITION);

    /**/
  //  CPoint XScrollPt=GetScrollPosition();
  //  ScrollBy = XScrollPt-ScrollPt;
  //  PositionCaret();
      }
    else if (nSBCode==SB_ENDSCROLL)
      IgnoreOneScroll=false;
    }
  else
    CView::OnVScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  if (CPg)
    {
    int sp=HScrl->GetScrollPos();
    //TRACE1("HScroll %i\n",sp);
    CRect Vr=ScrGB.ChVisibleRect(VScrlOn, HScrlOn);
    int PgL=(ScrGB.ChWorkWidth(VScrlOn));
    switch (nSBCode)
      {
      case SB_RIGHT         : ScrGB.ChScrollBy.cx=-ScrGB.ChScroll.cx; break;
      case SB_LINERIGHT     : ScrGB.ChScrollBy.cx+=1; break;
      case SB_LINELEFT      : ScrGB.ChScrollBy.cx-=1; break;
      case SB_PAGERIGHT     : ScrGB.ChScrollBy.cx+=PgL; break;
      case SB_PAGELEFT      : ScrGB.ChScrollBy.cx-=PgL; break;
      case SB_THUMBPOSITION :  // Scroll to the absolute position. The current position is provided in nPos.
      case SB_THUMBTRACK    : ScrGB.ChScrollBy.cx=(nPos-sp); break;
      case SB_LEFT          : ScrGB.ChScrollBy.cx=-ScrGB.ChScrollBy.cx; break;
      case SB_ENDSCROLL     : break;
      }
    FixScrollSizes(false);
    PositionCaret(PC_Redraw | PC_NoClearSlct | PC_NoSetSlct);;//nSBCode==SB_ENDSCROLL || nSBCode==SB_THUMBTRACK || nSBCode==SB_THUMBPOSITION);
//    if(nSBCode!=SB_ENDSCROLL)
//      FixScrollSizes(false);
    //PositionCaret();
    }
  else
    CView::OnHScroll(nSBCode, nPos, pScrollBar);
  }

//---------------------------------------------------------------------------
/*
void FxdEdtView::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  DoLButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------

void FxdEdtView::OnLButtonUp(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  DoLButtonUp(nFlags, point);
}

//---------------------------------------------------------------------------

void FxdEdtView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  DoLButtonDblClk(nFlags, point);
}
*/
//---------------------------------------------------------------------------

void FxdEdtView::OnRButtonDown(UINT nFlags, CPoint point)
  {
  CClientDC DC(this);
  CDCResChk ResChk(DC);

  ScrGB.Attach(&DC, NULL);
  SetFocus();
  if (CPg)
    {
    CPoint OldChEditPos=ChEditPos;
    FxdEdtFld * OldEdtFld=EdtFld();
    SetEditPos(point);
    EndCharEdit(OldEdtFld);
    FixCaret();
    PositionCaret(PC_Redraw | PC_NoClearSlct | PC_NoSetSlct | PC_ClearIfNotSlcted);

    ScrGB.Detach();

    DoRButtonDown(nFlags, point);

    if (RebuildReqd || ReloadReqd)
      {
      LoadStrings();
      Invalidate();
      //FixScrollSizes();
      }
    }
  else
    ScrGB.Detach();
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnRButtonUp(UINT nFlags, CPoint point)
  {
  CClientDC DC(this);
  CDCResChk ResChk(DC);

  if (CPg)
    {
    ScrGB.Attach(&DC, NULL);
    CPoint OldChEditPos=ChEditPos;
    FxdEdtFld * OldEdtFld=EdtFld();
    SetEditPos(point);
    EndCharEdit(OldEdtFld);
    FixCaret();
    PositionCaret(PC_Redraw | PC_NoClearSlct | PC_SetSlct);;//false); // true, true);

    ScrGB.Detach();
  // Creates A Crash to have this here  GDICfg.DoConfigDialog();
    DoRButtonUp(nFlags, point);

    if (RebuildReqd || ReloadReqd)
      {
      LoadStrings();
      Invalidate();
      //FixScrollSizes();
      }
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnRButtonDblClk(UINT nFlags, CPoint point)
  {
  CClientDC DC(this);
  CDCResChk ResChk(DC);

  ScrGB.Attach(&DC, NULL);
  SetFocus();

  if (CPg)
    {
    }

  ScrGB.Detach();

  DoRButtonDblClk(nFlags, point);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnMouseMove(UINT nFlags, CPoint point)
  {
  CClientDC DC(this);
  CDCResChk ResChk(DC);

  ScrGB.Attach(&DC, NULL);
  if (GotLButtonDn && (nFlags & MK_LBUTTON) && CPg)
    {
    CPoint OldChEditPos=ChEditPos;
    SetEditPos(point);
    CPoint NewChEditPos=ChEditPos;
    ChEditPos=OldChEditPos;
    if (NewChEditPos.y>0)
      {
      int r0=Min(NewChEditPos.y, ChEditPos.y);
      int rn=Max(NewChEditPos.y, ChEditPos.y);
      int c0=Min(NewChEditPos.x, ChEditPos.x);
      int cn=Max(NewChEditPos.x, ChEditPos.x);
      FxdEdtInfo EI;
      for (int r=r0; r<=rn; r++)
        for (int c=c0; c<=cn; c++)
          if (LocateFromCR(c, r, EI))
            {
            EI.Fld->fSelected=EI.Fld->fSelectable;
            EI.Fld->Invalidate(ScrGB);
            }
      }
    }

  DoMouseMove(nFlags, point);

  ScrGB.Detach();

  if (CPg)
    {
    if (RebuildReqd || ReloadReqd)
      {
      LoadStrings();
      Invalidate();
      //FixScrollSizes();
      }
    }
  }

//---------------------------------------------------------------------------

BOOL FxdEdtView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
  {
  if (m_WheelChangeVal)
    {
    FxdEdtInfo EI;
    if (LocateFromCR(ChEditPos.x, ChEditPos.y, EI))
      {
      if (EI.Fld->fEditable && EI.Fld->Str())
        {
        Strng S=EI.Fld->Str();
        int IsNum=1;
        int DotPos=-1;
        for (int i=0; i<S.Length(); i++)
          {
          int C=S[i];
          IsNum=IsNum && (isdigit(C) || C=='-' || C=='+' || C=='.' || C=='e' || C=='E');
          //if (C=='-' || C=='+' || C=='.' || C=='e' || C=='E');
          }
        if (IsNum)
          {
          CString Ss(S());
          AdjustValue(Ss, zDelta/120>0, nFlags&MK_SHIFT ?1:2);
          }
        }
      }
    }
  else
    {
    //todo...
    //TODO Get mouse wheel to scroll window
    }
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
  }

//---------------------------------------------------------------------------

BOOL FxdEdtView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
  {
  // TODO: Add your message handler code here and/or call default
  if (hRqdCursor)
    {
    ::SetCursor(hRqdCursor);
    }

  return CView::OnSetCursor(pWnd, nHitTest, message);
  }

//---------------------------------------------------------------------------

void FxdEdtView::SetRqdCursor(HCURSOR hCursor)
  {
  hRqdCursor = hCursor; 
  if (hRqdCursor)
    SetCursor(hRqdCursor);
  //else
  //  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }

//---------------------------------------------------------------------------

int GetLineOfFlds(char ** pStart, CSVector &FldBuff)
  {
  char * p=*pStart;
  long NFlds=0;
  for (int i=0; i<FldBuff.GetSize(); i++)
    FldBuff[i]="";
  for (;;)
    {
    int i=NFlds++;
    FldBuff.SetSize(Max(FldBuff.GetSize(), NFlds));
    char *p1=strpbrk(p, "\t\n\r");
    if (p1)
      {
      FldBuff[i].Set("%*.*s", p1-p, p1-p, p);
      p=p1+1;
      if (*p1=='\r' && *p=='\n') // CR-LF
        {
        p++;
        break;
        }
      else if (*p1=='\n' || *p1=='\r') // newline
        break;
      }
    else
      {
      FldBuff[i]=p;
      p=p+strlen(p);
      break;
      }
    }
  *pStart=p;
  return NFlds;
  }

//---------------------------------------------------------------------------

void FxdEdtView::PutToFld(FxdEdtFld * pFld, Strng & Buff)
  {
  pFld->fEditing=0;
  pFld->Changed=1;
  pFld->AcceptChanges=0;
  if (GetDocument())
    GetDocument()->SetModifiedFlag(true);
  pFld->FixBeforeParse();
  CPg->CurBlkId=0;
  FxdEdtInfo EI(this, CPg, pFld, pFld->pRow->RowNo);
  EI.fPasteBusy=MoreSlctFlds();
  DWORD RebuildCnt=dwRebuildCnt;
  Parse(EI, Buff);
  if (RebuildReqd || ReloadReqd || (RebuildCnt!=dwRebuildCnt))
    {
    }
  else
    {
    CPg->CurBlkId=0;
    FxdEdtInfo EI(this, CPg, pFld, pFld->pRow->RowNo);
    Load(EI, pFld->Str);
    pFld->FixAfterLoad(pFld->Str);
    pFld->Invalidate(ScrGB);
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::PasteFromClipBoard()//FxdEdtFld * pFld)
  {
  HANDLE hData=GetClipboardData(CF_TEXT);
  if (hData!=NULL)
    {
    Strng DataBuff;
    char *p= (char *)GlobalLock(hData);
    int iF=0;

    flag SingleSrcLine=((strchr(p, '\n')==NULL)||(strchr(p, '\n')>=(p+strlen(p)-1)));
    flag MultipleSrcLines=!SingleSrcLine;

    FxdEdtFld * pPrvFld=NULL;
    FxdEdtFld * pFld=FirstSlctFld();
    if (pFld)
      {
      CSVector FldBuff;
      int R1=pFld->pRow->RowNo;
      int NFlds=GetLineOfFlds(&p, FldBuff);
      int iF=0;
      for ( ; pFld; pFld=NextSlctFld())
        //if (pFld->FieldType!=SEPAR_FIELDS)
          {
          if (pFld->pRow->RowNo!=R1)
            {
            if (MultipleSrcLines)
              NFlds=GetLineOfFlds(&p, FldBuff);
            iF=0;
            }
          R1=pFld->pRow->RowNo;
          if (pFld && pFld->fEditable)
            {
            if (FldBuff[iF].Length()==0)
              FldBuff[iF]=" ";  // Kludge to prevent a crash
            PutToFld(pFld, FldBuff[iF]);
            iF=Min(NFlds-1, iF+1);
            }
          }
      }

    if (RebuildReqd || ReloadReqd)
      {
      LoadStrings();
      Invalidate();
      FixScrollSizes(true);
      }
    else
      CPg->CurBlkId=0;

    GlobalUnlock(hData);
    }
  };

//---------------------------------------------------------------------------

void FxdEdtView::OnCut()
  {
  if (pEdtControl)
    pEdtControl->Cut();
  else if (pCBoxControl)
    pCBoxControl->Cut();
  else
    DoCopyCut(DCC_CutVal);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnCopy()
  {
  if (pEdtControl)
    pEdtControl->Copy();
  else if (pCBoxControl)
    pCBoxControl->Copy();
  else
    DoCopyCut(DCC_CopyVal);
  };

//---------------------------------------------------------------------------

void FxdEdtView::OnPaste()
  {
  if (pEdtControl)
    pEdtControl->Paste();
  else if (pCBoxControl)
    pCBoxControl->Paste();
  else if (CPg)
    {
    if (OpenClipboard())
      {
      PasteFromClipBoard();//pFld);
      CloseClipboard();
      }
    }
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnCutTag()
  {
  DoCopyCut(DCC_CutTag);
  };

//---------------------------------------------------------------------------

void FxdEdtView::OnCopyTag()
  {
  DoCopyCut(DCC_CopyTag);
  };

//---------------------------------------------------------------------------

void FxdEdtView::OnCopyRHSTag()
  {
  DoCopyCut(DCC_CopyRHSTag);
  };

//---------------------------------------------------------------------------

void FxdEdtView::DoCopyCut(int What)
  {
  if (pEdtControl)
    pEdtControl->DestroyWindow();
  if (pCBoxControl)
    pCBoxControl->DestroyWindow();
  //if (pEdtSpin)
  //  pEdtSpin->DestroyWindow();
  if (CPg)
    {
    char BigBuff[65000];
    BigBuff[0]=0;
    FxdEdtFld * pPrvFld=NULL;
    for (FxdEdtFld * pFld=FirstSlctFld(); pFld; pFld=NextSlctFld())
      {
      if (pFld)// && (pFld->FieldType!=SEPAR_FIELDS))
        {
        char buff[4096];
        flag OK=false;
        switch (What)
          {
          case DCC_CutTag    : OK= DoCutTag(pFld, buff, sizeof(buff)); break;
          case DCC_CutVal    : OK= DoCutVal(pFld, buff, sizeof(buff)); break;
          case DCC_CopyTag   : OK= DoCopyTag(pFld, buff, sizeof(buff)); break;
          case DCC_CopyRHSTag: OK= DoCopyRHSTag(pFld, buff, sizeof(buff)); break;
          case DCC_CopyVal   : OK= DoCopyVal(pFld, buff, sizeof(buff)); break;
          }
        if (OK)//DoCopyVal(pFld, buff, sizeof(buff)))
          {
          if (pPrvFld)
            {
            if (pPrvFld->pRow==pFld->pRow)
              strcat(BigBuff, "\t");
            else
              strcat(BigBuff, "\r\n");
            }
          strcat(BigBuff, buff);
          pPrvFld=pFld;
          }
        }
      }
    if (BigBuff[0])
      CopyTextToClipboard(this, BigBuff);
    }
  };

//---------------------------------------------------------------------------

void FxdEdtView::AdjustValue(CString S, bool Up, int deltadecimal)
  {
  CString S1;
  S.TrimLeft();
  double Val=SafeAtoF(S.GetBuffer(0));
  double Val0=Val;
  if (S.Find('.')<0)
  //if (fmod(Val, 1.0)<1.0e-10)
    { //integer val
    if (!Up)
     Val-=1;
    else
     Val+=1;
    }
  else
    {
    int  decimal, sign;
    _fcvt(Val!=0.0 ? Val : 1.0, 300, &decimal, &sign );
    // adj decimal;
    decimal-=deltadecimal;
 
    if (!Up)
      Val-=pow(10.0, decimal);
    else
      Val+=pow(10.0, decimal);
    }
  S1.Format("%g",Val);
  // dbgpln("%10.4f -> %10.4f", Val0, Val);

  FxdEdtFld * Fld=EdtFld();
  if (Fld)
    {
    Fld->Str=(const char *)S1;
    Fld->Changed=1;
    Fld->AcceptChanges=0;
    if (GetDocument())
      GetDocument()->SetModifiedFlag(true);
    Fld->FixBeforeParse();
    CPg->CurBlkId=0;
    FxdEdtInfo EI(this, CPg, Fld, Fld->pRow->RowNo);
    DWORD RebuildCnt=dwRebuildCnt;
    Parse(EI, Fld->Str);
//      if (!RebuildReqd && !ReloadReqd)
    if (RebuildCnt==dwRebuildCnt)
      {
      CPg->CurBlkId=0;
      //TmpFldBuff Buff(Fld->Str);
      Load(EI, Fld->Str);//Buff.Str, Fld->Len);
      Fld->FixAfterLoad(Fld->Str);//Buff);
      Fld->Invalidate(ScrGB);
      }
    }
  }

//---------------------------------------------------------------------------

BOOL FxdEdtView::OnUDNDeltaPos(UINT id, NMHDR * pNotifyStruct, LRESULT * result)
  {
  _NM_UPDOWN *Hdr = (_NM_UPDOWN*)pNotifyStruct;

  IgnoreOneScroll=true;
  if (pEdtControl)
    {
    CString S;
    pEdtControl->GetWindowText(S);
    AdjustValue(S, Hdr->iDelta<=0, abs(Hdr->iDelta)==1 ?2:1);
    FxdEdtFld * Fld=EdtFld();
    if (Fld)
      {
      S=Fld->Str();
      pEdtControl->SetWindowText(S);
      pEdtControl->SetSel(-1,-1);
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

LRESULT FxdEdtView::OnEditDone(WPARAM wParam, LPARAM lParam)
  {
  SetFocus();
  return True;
  }

//---------------------------------------------------------------------------

void FxdEdtView::MoveCursorTo(int PageNo, int RowNo, int FldNo, flag SelectFld)
  {
  SelectPage(PageNo);
  if (CPg)
    {
    CPoint OldChEditPos=ChEditPos;
    FxdEdtFld * OldEdtFld=EdtFld();
    int ColNo=1;
    if (RowNo<CPg->Rows)
      {
      FxdEdtRow &R=*(CPg->Row[RowNo]);
      for (int f=0; f<Min((short)(FldNo+1), R.NFields); f++)
        ColNo=R.Field[f]->Col1;
      }
    ChEditPos.x=Range((int)0, (int)(ColNo), (int)(CPg->MaxCols+1));
    ChEditPos.y=Range((int)0, (int)(RowNo), (int)(CPg->Rows+1));

//    SetEditPos(point);
    EndCharEdit(OldEdtFld);
    FixCaret();
    PositionCaret(PC_Redraw | PC_ClearSlct | PC_SetSlct);;
    FxdEdtFld * Fld=CurrentFld();//EdtFld();
    if (Fld)
      {
      Fld->fSelected=Fld->fSelectable;
      Fld->Invalidate(ScrGB);

      if (Fld->iCtrlIndex>=0)
        PostMessage(WM_COMMAND, MAKEWPARAM(FxdEdtCtrlID0+Fld->iCtrlIndex, BN_CLICKED), 0);
      }
    }

  /***
//  // TODO: Add your message handler code here and/or call default
//  MDIActivateThis(this);
//  SetFocus();
//
//  //CDC DC;
//  //DC.CreateCompatibleDC(NULL);
//  CClientDC DC(this);
//  ScrGB.Attach(&DC, NULL);
  if (CPg)
    {
//    int X,Y;
    CPoint OldChEditPos=ChEditPos;
    FxdEdtFld * OldEdtFld=EdtFld();
//    CPoint MP;//=Point2RowCols(ScrGB, point);
//    //MP.x=(point.x-GB.LuMargin.cx-GB.LuBorder.cx)/GB.ColWdt();
//    Y=(point.y-(int)(ScrGB.LuMargin.cy+ScrGB.LuBorder.cy));
//    MP.y=Y/ScrGB.RowHgt()+(Y<0?-1:0);
//    if (MP.y<0)
//      {
//      FxdEdtFld * pEdtFld=EdtFld();
//      if(pEdtFld)
//        {
//        pEdtFld->AcceptChanges=pEdtFld->fEditing;
//        EndCharEdit(pEdtFld);
//        FixCaret();
//        }
////      point.y-=ScrGB.RowHgt()/3;
//      //MP=Point2RowCols(ScrGB, point);
//      X=(point.x-(int)(ScrGB.LuMargin.cx+ScrGB.LuBorder.cx));
//      MP.x=X/ScrGB.ColWdt()+(X<0?-1:0);
//      Y=(point.y-(int)(ScrGB.LuMargin.cy+ScrGB.LuBorder.cy));
//      MP.y=Y/ScrGB.RowHgt()+(Y<0?-1:0);
//      int r=-(MP.y+2);
//      int c=MP.x;
//      //TRACE2("LB %3i %3i",MP.y,r);
//      //#ifdef RotateTabs
//      //r=(r-CPg->TabRow+TabIndexRows)%TabIndexRows;
//      //#endif
//
//      for (int p=0; p<Pages; p++)
//        if ((Pg[p]->TabRow==r) &&
//            (Pg[p]->TabPos<=c) &&
//            (Pg[p]->TabPos+1+Pg[p]->TabWide>c))
//          {
//          SelectPage(p);
//          ChEditPos=OldChEditPos;
//          break;
//          }
//      PositionCaret(true, !bShiftDown);
//      }
//    else
//      {
      SetEditPos(point);
      EndCharEdit(OldEdtFld);
      FixCaret();
//      DownButton=ButtonFld();
//      if (DownButton)
//        if (DownButton->IsButton)
//          {
//          CPg->CurBlkId=0;
//          FxdEdtInfo EIB(this, CPg, DownButton, DownButton->pRow->RowNo);
//          ButtonPushed(EIB, DownButton->Str);//, DownButton->Len);
//
//          CPg->CurBlkId=0;
//          FxdEdtInfo EIL(this, CPg, DownButton, DownButton->pRow->RowNo);
//          //TmpFldBuff Buff(DownButton->Str);
//          Load(EIL, DownButton->Str);//Buff.Str, DownButton->Len);
//
//          DownButton->FixAfterLoad(DownButton->Str);
//          DownButton->ButtonDown=1;
//          //DownButton->Invalidate();
//          //CRect DummyRect(0,0,1,1);
//          //RedrawWindow(&DummyRect);
//          ChEditPos=OldChEditPos;
//          }
      PositionCaret(true, !bShiftDown);

//      if ((nFlags & MK_CONTROL)==0)
//        {
//        for (int r=0; r<CPg->Rows; r++)
//          for (int f=0; f<CPg->Row[r]->NFields; f++)
//            {
//            FxdEdtFld * Fld=CPg->Row[r]->Field[f];
//            if (Fld->fSelected)
//              {
//              Fld->fSelected=false;
//              Fld->Invalidate(ScrGB);
//              }
//            }
////        Invalidate();
//        }

      FxdEdtFld * Fld=CurrentFld();//EdtFld();
      if (Fld)
        {
        Fld->fSelected=Fld->fSelectable;
        Fld->Invalidate(ScrGB);

        if (Fld->iCtrlIndex>=0)
          PostMessage(WM_COMMAND, MAKEWPARAM(FxdEdtCtrlID0+Fld->iCtrlIndex, BN_CLICKED), 0);
        }
//      }

    if (RebuildReqd || ReloadReqd)
      {
      LoadStrings();
      Invalidate();
      //FixScrollSizes();
      }
    }

//  if (DownButton==NULL)
//    DoLButtonDown(nFlags, point);
//  CView::OnLButtonDown(nFlags, point);
//  ScrGB.Detach();
*/
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFxdEdtListBox, CListBox)
  ON_WM_CHAR()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CFxdEdtListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if (nChar=='\r')
    GetParent()->SetFocus();
  else
    CListBox::OnChar(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

void CFxdEdtListBox::OnLButtonDown(UINT nFlags, CPoint point)
  {
  CRect Rect;
  GetWindowRect(&Rect);
  fCloseIt = (point.x<0 || point.y<0 ||
              point.x>(Rect.right-Rect.left) || point.y>(Rect.bottom-Rect.top));
  if (fCloseIt)
    {
    CWnd* p = GetParent();
    if (p)
      p->SetFocus(); // Kill List Box;
    }
  else
    {
    CListBox::OnLButtonDown(nFlags, point);
    }
  }

//---------------------------------------------------------------------------

void CFxdEdtListBox::OnLButtonUp(UINT nFlags, CPoint point)
  {
  if (fCloseIt)
    {
    //CWnd* p = GetParent();
    //if (p)
    //  p->SetFocus(); // Kill List Box;
    }
  else
    {
    CListBox::OnLButtonUp(nFlags, point);
    }
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFxdEdtComboBox, CComboBox)
  ON_WM_CHAR()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

void CFxdEdtComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
  if (nChar=='\r')
    GetParent()->SetFocus();
  else
    CComboBox::OnChar(nChar, nRepCnt, nFlags);
  }

//---------------------------------------------------------------------------

void CFxdEdtComboBox::OnLButtonDown(UINT nFlags, CPoint point)
  {
//  CRect Rect;
//  GetWindowRect(&Rect);
//  fCloseIt = (point.x<0 || point.y<0 ||
//              point.x>(Rect.right-Rect.left) || point.y>(Rect.bottom-Rect.top));
//  if (fCloseIt)
//    {
//    CWnd* p = GetParent();
//    if (p)
//      p->SetFocus(); // Kill Combo Box;
//    }
//  else
    {
    CComboBox::OnLButtonDown(nFlags, point);
    }
  }

//---------------------------------------------------------------------------

void CFxdEdtComboBox::OnLButtonUp(UINT nFlags, CPoint point)
  {
//  if (fCloseIt)
//    {
//    //CWnd* p = GetParent();
//    //if (p)
//    //  p->SetFocus(); // Kill Combo Box;
//    }
//  else
    {
    CComboBox::OnLButtonUp(nFlags, point);
    }
  }

//---------------------------------------------------------------------------

BOOL FxdEdtView::OnCommand( WPARAM wParam, LPARAM lParam )
  {
  int iCtrl=LOWORD(wParam);
  int iNotify=HIWORD(wParam);
  //TRACE ("FxdEdtView::OnCommand %4i %4i %8x\n", iCtrl, iNotify, lParam);
  switch (iCtrl)
    {
    case FxdEdtCtrlID0-2: // Common Edit
      {
      }
      break;
    case FxdEdtCtrlID0-1: // Common ComboBox
      {
      switch (iNotify)
        {
        case CBN_CLOSEUP:
          {
          PostMessage(WMU_FXDEDTEDITDONE, (WPARAM)0, (LPARAM)0);
          return true;
          }
        case CBN_SELENDOK:
          {
          if (pCBoxControl && !fCBoxSelEndDone)
            pCBoxControl->fAcceptChgs=true;
          fCBoxSelEndDone=true;
          break;
          }
        case CBN_SELENDCANCEL:
          {
          if (pCBoxControl && !fCBoxSelEndDone)
            pCBoxControl->fAcceptChgs=false;
          fCBoxSelEndDone=true;
          break;
          }
        }
      }
      break;
    default:
      {
      iCtrl-=FxdEdtCtrlID0;
      if (iCtrl>=0 && iCtrl<nCtrls)
        {
        FxdEdtFld * pFld=pCtrl[iCtrl].pFld;

        if (iNotify==BN_CLICKED)
          {
          //CPoint OldChEditPos=ChEditPos;
          SetFocus();
          //ClrCurrentEdit(false); //KGA 6/10/98
          switch (pFld->m_iCtrlType)
            {
            case FXWC_CHECK:
            case FXWC_BTN:
            case FXWC_CHECKBTN:
            case FXWC_TREEND:
            case FXWC_BM1BTN:
            case FXWC_BM2BTN:
              {
              CPg->CurBlkId=0;
              FxdEdtInfo EIB(this, CPg, pFld, pFld->pRow->RowNo);
              if (pFld->m_iCtrlType==FXWC_CHECK ||
                  pFld->m_iCtrlType==FXWC_CHECKBTN)
                {
                pFld->Str=SafeAtoL(pFld->Str()) ? "0" : "1";
                }
              ButtonPushed(EIB, pFld->Str); //be carefull! if logic behind ButtonPushed "rebuilds" fixedit view, then code (pFld, etc) below becomes unstable!!

              if (!RebuildReqd && CPg)
                {
                CPg->CurBlkId=0;
                FxdEdtInfo EIL(this, CPg, pFld, pFld->pRow->RowNo);
                //TmpFldBuff Buff(pFld->Str);
                Load(EIL, pFld->Str);

                pFld->FixAfterLoad(pFld->Str);
                pFld->ButtonDown=1;
                //((CButton*)pCtrl[iCtrl].pCtrl)->SetCheck(pFld->Str.SafeAtoL() ? 1 :0);
                }
              SetFocus();  // remove Focus from the button

              PositionCaret(PC_Redraw | (bShiftDown ? PC_NoClearSlct : PC_ClearSlct)| PC_SetSlct);

              if (RebuildReqd || ReloadReqd)
                {
                LoadStrings();
                Invalidate();
                FixScrollSizes(true);
                }
              break;
              }
            case FXWC_DDLST:
            case FXWC_DDLSTOPT:
              {
              //pCFxdEdtListBox &pLstBox=pCtrl[iCtrl].pLstBox;
              //Strng & sLstBoxOrigValue = pCtrl[iCtrl].sLstBoxOrigValue;
              SetCBoxControl(pFld, this, 0, true);//, int EditPos)//, flag ClrContent)
              break;

              /*
              if (pComboBox)
                {
                pComboBox->ShowWindow(SW_SHOW);
                SetFocus(); // When List box loses the focus it will close
                }
              else
                {
                const int MinListBoxLen = 12;
                pStrng_List pL=pCtrl[iCtrl].pStrLst;
                CRect Rect(pFld->Xs(ScrGB)-1, pFld->Ys(ScrGB)-1,
                           pFld->Xe(ScrGB)+1+24,
                           pFld->Ye(ScrGB)+3+ScrGB.RowHgt()*Min(MinListBoxLen+1, pL->Length()+1)+2);//Extra for  Integral No Of Rows
    //                       pFld->Ye(ScrGB)+3+ScrGB.RowHgt()*Min(MinListBoxLen+1, pL->Length()+1)+2);//Extra for  Integral No Of Rows
                RECT CR;
                GetClientRect(&CR);
                //if (Rect.bottom>CR.bottom)
                //  Rect.OffsetRect(0, -(Rect.bottom-Rect.top+ScrGB.RowHgt()-10));
                pComboBox = new CFxdEdtComboBox();
                pComboBox->Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
                  Rect, this, FxdEdtCtrlID0+MaxFixedControls+iCtrl);
                pComboBox->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
    //            pComboBox->SetWindowPos(&wndBottom, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
                pComboBox->SetFont(ScrGB.pFont[0]);
                pComboBox->ResetContent();
                for (pStrng pS=pL->First(); pS; pS=pS->Next())
                  pComboBox->AddString(pS->Str() ? pS->Str() : "");

                Strng sLstBoxOrigValue=pFld->Str;
                sLstBoxOrigValue.Trim();
                pComboBox->SelectString(-1, sLstBoxOrigValue() ? sLstBoxOrigValue() : "");
                pComboBox->SetFocus();
                pComboBox->ShowDropDown(true);
                }
              */
              }
            }
          return true;
          }
        }
      else
        {
        iCtrl-=MaxFixedControls;
        if (iCtrl>=0 && iCtrl<nCtrls)
          {
          FxdEdtFld * pFld=pCtrl[iCtrl].pFld;
          switch (iNotify)
            {
            case CBN_EDITCHANGE:
            case CBN_SELENDOK:
              {
              int i=pComboBox->GetCurSel();
              if (i!=LB_ERR)
                {
                char Buff[1024];
                strncpy(Buff, pFld->Str(), sizeof(Buff));
                int i=pComboBox->GetCurSel();
                if (i>=0)
                  pComboBox->GetLBText(i, Buff);
                pFld->Str=Buff;
                pFld->FixAfterLoad(pFld->Str);
                pFld->Changed=1;
                pFld->Invalidate(ScrGB);
                }
              break;
              }
            case CBN_DBLCLK:
              {
              SetFocus(); // Kill List Box;
              break;
              }
            case CBN_SELCHANGE:
            case CBN_CLOSEUP:
              {
              if (pComboBox)
                {
                if (pFld->Changed)
                  {
                  pFld->fEditing=0;
                  pFld->AcceptChanges=0;
                  if (GetDocument())
                    GetDocument()->SetModifiedFlag(true);
                  pFld->FixBeforeParse();
                  CPg->CurBlkId=0;
                  FxdEdtInfo EI(this, CPg, pFld, pFld->pRow->RowNo);
                  DWORD RebuildCnt=dwRebuildCnt;
                  Parse(EI, pFld->Str);
                  if (RebuildReqd || ReloadReqd)
                    {
                    LoadStrings();
                    Invalidate();
                    FixScrollSizes(true);
                    }
                  else if (RebuildCnt==dwRebuildCnt)
                    {
                    CPg->CurBlkId=0;
                    Load(EI, pFld->Str);
                    pFld->FixAfterLoad(pFld->Str);
                    pFld->Invalidate(ScrGB);
                    }
                  }
                delete pComboBox;
                pComboBox=NULL;
                }
              break;
              }
            }
          return true;
          }
        }
      }
    }

  return CView::OnCommand(wParam, lParam);
  }

//---------------------------------------------------------------------------

//BOOL FxdEdtView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//  {
//  switch ()
//  return CView::OnNotify(wParam, lParam, pResult);
//  };

//---------------------------------------------------------------------------

BOOL FxdEdtView::OnPreparePrinting(CPrintInfo* pInfo)
  {
  pInfo->SetMaxPage(Pages);
  return DoPreparePrinting(pInfo);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
  {
  //if (pInfo->m_bPreview)
  //  Doc().HoldUpdateAllViews(1);
  CPgNoSP=CPgNo;
  CView::OnBeginPrinting(pDC, pInfo);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
  {
  CView::OnEndPrinting(pDC, pInfo);
  SelectPage(CPgNoSP);
  //Doc().HoldUpdateAllViews(0);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView )
  {
  CView::OnEndPrintPreview(pDC, pInfo, point, pView);
  }

//---------------------------------------------------------------------------

void FxdEdtView::OnPrint(CDC *pDC, CPrintInfo* pInfo)
  {
  // draw body text
  TRACE("PRINT [%i]\n", pInfo->m_nCurPage);
  SelectPage(pInfo->m_nCurPage-1);

  InitDC(pDC);
  GDIBlk PrnGB;
  PrnGB.Create(&GDICfg, this, pDC, true);
  PrnGB.Attach(pDC, pInfo);
  CalculateLayoutSize(PrnGB);
  DoOnDraw(PrnGB);
  PrnGB.Detach();
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

